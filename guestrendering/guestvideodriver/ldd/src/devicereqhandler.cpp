// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Device Request Handler Implementation


#include <platform.h>
#include <assp.h>
#include <kernel/kernel.h>
#include <e32cmn.h>

#include <graphics/devicereqhandler.h>
#include <graphics/virtualvideohwinterface.h>
#include <graphics/virtualvideotracing.h>

#include "remotefunctioncall.h"
#include "serializedfunctioncall.h"
#include "driverrfc.h"
#include "openvgrfc.h" //For the opcodes
#include "eglrfc.h" //For the opcodes
#include "opengles11rfc.h" //For the opcodes


using namespace GuestVideoDriver;

// LOCAL DATA TYPES
TInt CmpTPbsIds(const TPbSId& a, const TPbSId& b)
    {
    if( a.iPbuffer == b.iPbuffer )
        return 0;
    else
        return b.iPbuffer - a.iPbuffer;
    }

TInt CmpTVGIsIds(const TVGISId& a, const TVGISId& b)
    {
    if( a.iVGImage == b.iVGImage )
        return 0;
    else
        return b.iVGImage - a.iVGImage;
    }
/*
 * Copies data from client space to the request buffer during serialisation
 */
class TDataFromClientSpaceCopier: public MDataCopier
    {
public:
    TDataFromClientSpaceCopier( DThread* aUserThread ):
        iUserThread(aUserThread)
        {
        }
    
    TInt CopyData( TUint8* aDest, const TUint8* aSource, TUint32 aSize )
        {
        memcpy( aDest, aSource, aSize );
        return KErrNone;
        }
    
    TInt CopyVector( TUint8* aDest, const TUint8* aSource, TUint32 aSize )
        {
        return Kern::ThreadRawRead( iUserThread, aSource, aDest, aSize );
        }

    DThread* iUserThread;
    };

/*
 * Copies data to client space from the request buffer during deserialisation
 */
class TDataToClientSpaceCopier: public MDataCopier
    {
public:
    TDataToClientSpaceCopier( DThread* aUserThread ):
        iUserThread(aUserThread)
        {
        }
    
    TInt CopyData( TUint8* aDest, const TUint8* aSource, TUint32 aSize )
        {
        memcpy( aDest, aSource, aSize );
        return KErrNone;
        }
    
    TInt CopyVector( TUint8* aDest, const TUint8* aSource, TUint32 aSize )
        {
        return Kern::ThreadRawWrite(iUserThread, aDest, aSource, aSize );
        }

    DThread* iUserThread;
    };

// LOCAL FUNCTION DEFINITIONS

// -----------------------------------------------------------------------------
// ListRemoteFunctionCall
// -----------------------------------------------------------------------------
//
inline void ListRemoteFunctionCall(RemoteFunctionCallData& call)
    {
#if _DEBUG
    VVHW_TRACE("RFC: %u %u %u %u %u %u %u",
    call.Header().iOpCode,
    call.Header().iTransactionId,
    call.Header().iProcessId,
    call.Header().iThreadId,
    call.Header().iParameterCount,
    call.Header().iOpType,
    call.Header().iReturnValue);
    
    for ( TInt i = 0; i < call.Header().iParameterCount; i++)
        {
        RemoteFunctionCallData::TParam param = call.Parameters()[i];

        TUint32 dir = param.iDir;
        TUint32 type = param.iType;

        
        if ( RemoteFunctionCallData::TParam::ESimple == param.iType )
            {
            TUint32 datatype = param.iSimpleParam.iDataType; 
            VVHW_TRACE("RFC Simple Param %d: %u %u %u",i, dir, type, datatype );
            }
        else if ( RemoteFunctionCallData::TParam::EVector == param.iType )
            {
            TUint32 datatype = param.iVectorParam.iDataType; 
            TUint32 size = param.iVectorParam.iVectorLength;
            VVHW_TRACE("RFC Vec Param %d: %u %u %u %u",i, dir, type, datatype, size );
            }
        }
#endif // _DEBUG
    }


// CONSTANTS
const TInt KDfcPriority = 1;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Isr
// Interrupt service routine
// -----------------------------------------------------------------------------
//
void Isr( TAny* aParam )
    {
    DDeviceReqHandler* handler = ( DDeviceReqHandler* )aParam;
    Interrupt::Disable( handler->InterruptId() );
    handler->Dfc().Add();
    }

// -----------------------------------------------------------------------------
// DfcFunc
// Function to call when HW has an interrupt
// -----------------------------------------------------------------------------
//
void DfcFunc( TAny* aParam )
    {
    DDeviceReqHandler* handler = ( DDeviceReqHandler* )aParam;
    handler->ProcessDfc();
    }

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// DDeviceReqHandler::DDeviceReqHandler
// -----------------------------------------------------------------------------
//
DDeviceReqHandler::DDeviceReqHandler(
    DVirtualVideoHwInterface& aHwInterface,
    TDfcQue* aQueue) :
    iPbufferSgMapOrder ( &CmpTPbsIds ),
    iVGImageSgMapOrder ( &CmpTVGIsIds ),
    iCurrClientData(NULL),
    iInitialized(EFalse),
    iHwInterface( aHwInterface ),
    iDfc( DfcFunc, (TAny*)this, aQueue, KDfcPriority ),
    iInterruptId( 0 ),
    iTransactionId( 1 ),
    iRequestBuffer( *this, VVI_PARAMETERS_INPUT_MEMORY_SIZE )
    {
    iDriverRfcProcessId = 0;
    iDriverRfcThreadId  = 0;
    iOpType       = 0; 
    iOpChainPhase = 0;
    iInterruptId  = VVI_IRQ;
    VVHW_TRACE( "DDeviceReqHandler constructor" );
    VVHW_TRACE( "Called in process/thread %0x / %0x", Kern::CurrentProcess().iId, Kern::CurrentThread().iId );
    TInt err = Interrupt::Bind( iInterruptId, Isr, ( TAny* )this );
    if ( err != KErrNone )
        {
        VVHW_TRACE( "Interrupt::Bind Error: %d", err );
        }
    err = Interrupt::Enable( iInterruptId );
    if ( err != KErrNone )
        {
        Interrupt::Unbind( iInterruptId );
        iInterruptId = 0;
        VVHW_TRACE( "Interrupt::Enable Error: %d", err );
        }
    iRequestBuffer.InitBuffer();
    //Register this object with the extension
    VVHW_TRACE( "DDeviceReqHandler calling ReqHandlerExtension" );
#ifdef FAISALMEMON_S4_SGIMAGE
    ReqHandlerExtension::SetReqHandler( this );
#endif
    }

// -----------------------------------------------------------------------------
// DDeviceReqHandler::~DDeviceReqHandler
// -----------------------------------------------------------------------------
//
DDeviceReqHandler::~DDeviceReqHandler()
    {
    for( TInt i=0; i < iClientData.Count(); ++i )
        {
        delete iClientData[i];
        }
    iClientData.Close();
    Interrupt::Disable( iInterruptId );
    Interrupt::Unbind( iInterruptId );
    iInterruptId = 0;
    }

// -----------------------------------------------------------------------------
// DDeviceReqHandler::HandleClientShutdown
// -----------------------------------------------------------------------------
//
TInt DDeviceReqHandler::HandleClientShutdown( TUint aProcessId, TUint aThreadId )
    {
    VVHW_TRACE("DDeviceReqHandler::HandleClientShutdown");
    TInt err( KErrNone );
    TAsyncRequest* req = AllocRequest( NULL, NULL, NULL, NULL, TAsyncRequest::ERTDriver );
    
    if ( req )
        {
        DriverRFC drfc( req->iRemoteFunctionCall );
        drfc.Init( DriverRFC::EDrvClientShutdown, RemoteFunctionCallData::EOpRequest );        
        req->iRemoteFunctionCall.SetTransactionId( ++iTransactionId );
        req->iRemoteFunctionCall.SetThreadInformation( aProcessId, aThreadId );        
        iPendingRequestRoot.AppendToLast( req );
        TInt ret = ProcessNextPendingRequest();
        while ( KErrNone == ret )
            {
            ret = ProcessNextPendingRequest();
            }
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// DDeviceReqHandler::ProcessNextPendingRequest
// -----------------------------------------------------------------------------
//
TInt DDeviceReqHandler::ProcessNextPendingRequest()
    {
    VVHW_TRACE("DDeviceReqHandler::ProcessNextPendingRequest");
    TInt err( KErrNone );
    TAsyncRequest* req = iPendingRequestRoot.iNext;
    if ( !req )
        {
        VVHW_TRACE("DDeviceReqHandler::ProcessNextPendingRequest !req");
        return KErrUnderflow;
        }
    //Try to serve the client from here, or do some sgImage specific stuff
    TInt mode = InterpretRequest( req );
    switch( mode )
        {
        case ENormal:
            {
            //Nothing, process as normal
            break;
            }
        case EHandled:
            {
            //Already handled, remove and signal complete
            req->RemoveFromList( &iPendingRequestRoot );
            Kern::RequestComplete( req->iAsyncClient, req->iStatus, KErrNone );
            ReleaseRequest( req );
            return KErrNone;//Change to "err" if you add lines above this that may set it to something else
            }
        case EQueued:
            {
            //To be removed from the usual queues, but not deleted (released) or completed
            req->RemoveFromList( &iPendingRequestRoot );
            return KErrNone;
            }
        default:
            {
            break;
            }
        }
    
    if ( TAsyncRequest::ERTCommandLoad == req->iCommandLoadRequest )
        {
        VVHW_TRACE("DDeviceReqHandler::HandleLoadCommands");
        TInt loaderr( KErrNone );
        TInt len = Kern::ThreadGetDesLength( req->iAsyncClient, req->iA1 );

        if ( len < 0 )
            {
            VVHW_TRACE("DDeviceReqHandler::HandleLoadCommands len < 0");
            return KErrUnderflow;;
            }

        if ( !iRequestBuffer.CheckForSpace( len ) )
           {
           //Not enough space
           VVHW_TRACE("DDeviceReqHandler::HandleLoadCommands No space");
           return KErrNoMemory;
           }

        req->RemoveFromList( &iPendingRequestRoot );

        const TUint32 base( iRequestBuffer.AllocateBytes( len ) );

        //Next, serialize the call to the HW memory
        const TLinAddr paramAddr( iHwInterface.InputParametersAddress() + base );

        TPtr8 ptr(reinterpret_cast<TUint8*>(paramAddr), 0, len );
        
        //Read the RemoveFunctionCall
        loaderr = Kern::ThreadDesRead( req->iAsyncClient, req->iA1, ptr, 0, 0 );
        VVHW_TRACE("DDeviceReqHandler::HandleLoadCommands ThreadDesRead %d", loaderr);

        if ( KErrNone == loaderr )
            {
            VVHW_TRACE("DDeviceReqHandler::HandleLoadCommands CommitBytes");
            iRequestBuffer.CommitBytes( base, len );
            iHwInterface.IssueCommand( VVI_EXECUTE );
            }
        Kern::RequestComplete( req->iAsyncClient, req->iStatus, loaderr );
        ReleaseRequest( req );
        }
    else //ERTRequest or ERTDriver
        {
#if _DEBUG
        ListRemoteFunctionCall( req->iRemoteFunctionCall );
#endif
       VVHW_TRACE("DDeviceReqHandler::ProcessNextPendingRequest req %u tid %u opcode %u", 
            req, req->iRemoteFunctionCall.Header().iTransactionId,
            req->iRemoteFunctionCall.Header().iOpCode );
        const TUint32 serializedLength( req->iRemoteFunctionCall.SerialisedLength() );
        if ( !iRequestBuffer.CheckForSpace( serializedLength ) )
           {
           //Not enough space
           VVHW_TRACE("DDeviceReqHandler::ProcessNextPendingRequest No space");
           return KErrOverflow;
           }
    
        req->RemoveFromList( &iPendingRequestRoot );
    
        if ( RemoteFunctionCallData::EOpRequestWithReply == req->iRemoteFunctionCall.Header().iOpType )
            {
            VVHW_TRACE("DDeviceReqHandler::ProcessNextPendingRequest 3");
            iSerializedRequestRoot.AppendToLast( req );
            }
        TDataFromClientSpaceCopier datacopier( req->iAsyncClient );
        SerializedFunctionCall sfc( req->iRemoteFunctionCall );
        
        if ( TAsyncRequest::ERTRequest == req->iCommandLoadRequest )
            {
            sfc.SetDataCopier( &datacopier );            
            }
        
        const TUint32 base( iRequestBuffer.AllocateBytes( serializedLength ) );
    
        //Next, serialize the call to the HW memory
        const TLinAddr paramAddr( iHwInterface.InputParametersAddress() + base );
    
        TInt len = sfc.WriteToBuffer( reinterpret_cast<TUint8*>(paramAddr),
                VVI_PARAMETERS_INPUT_MEMORY_SIZE - 1 - base );
        
        sfc.SetDataCopier( NULL );
        VVHW_TRACE("DDeviceReqHandler::ProcessNextPendingRequest Write at index %u with length %u", base, len );
        iRequestBuffer.CommitBytes( base, serializedLength );
    
        iHwInterface.IssueCommand( VVI_EXECUTE );
        
        if ( RemoteFunctionCallData::EOpRequest == req->iRemoteFunctionCall.Header().iOpType )
            {
            if ( TAsyncRequest::ERTRequest == req->iCommandLoadRequest )
                {
                VVHW_TRACE("DDeviceReqHandler::ProcessNextPendingRequest RequestComplete");
                Kern::RequestComplete( req->iAsyncClient, req->iStatus, KErrNone );
                }
            ReleaseRequest( req );
            }
        }
    VVHW_TRACE("DDeviceReqHandler::ProcessNextPendingRequest DONE");
    return err;
    }

// -----------------------------------------------------------------------------
// DDeviceReqHandler::HandleLoadCommands
// -----------------------------------------------------------------------------
//
TInt DDeviceReqHandler::HandleLoadCommands(
        DThread*& aUserThread,
        TRequestStatus* aStatus,
        TAny* aA1 )
    {
    VVHW_TRACE("DDeviceReqHandler::HandleLoadCommands");
    TInt err( KErrNone );
    TAsyncRequest* req = AllocRequest( aStatus, aUserThread, aA1, NULL, TAsyncRequest::ERTCommandLoad );
    
    if ( req )
        {
        TInt len = Kern::ThreadGetDesLength( req->iAsyncClient, req->iA1 );
    
        if ( len < 0 )
            {
            err = len;//TODO: this sucks
            ReleaseRequest( req );
            return err;
            }
            
        iPendingRequestRoot.AppendToLast( req );
        TInt ret = ProcessNextPendingRequest();
        while ( KErrNone == ret )
            {
            ret = ProcessNextPendingRequest();
            }
        }
    else
        {
        err = KErrNoMemory;
        }

    return err;
    }

// -----------------------------------------------------------------------------
// DDeviceReqHandler::HandleExecuteAsync
// -----------------------------------------------------------------------------
//
TInt DDeviceReqHandler::HandleExecuteAsync(
        DThread*& aUserThread,
        TRequestStatus* aStatus,
        TAny* a1 )
    {
    VVHW_TRACE("DDeviceReqHandler::HandleExecuteAsync");
    TInt err( KErrNone );
    TAsyncRequest* req = AllocRequest( aStatus, aUserThread, a1, NULL );
    
    if ( req )
        {
        TInt len = Kern::ThreadGetDesLength( req->iAsyncClient, req->iA1 );
    
        if ( len < 0 )
            {
            err = len;
            ReleaseRequest( req );
            return err;
            }
    
        TPckg<RemoteFunctionCallData> rcallbuf( req->iRemoteFunctionCall );
        
        //Read the RemoteFunctionCall
        err = Kern::ThreadDesRead( req->iAsyncClient, req->iA1, rcallbuf, 0, 0 );

        if ( KErrNone != err )
            {
            ReleaseRequest( req );
            return err;
            }
        
        req->iRemoteFunctionCall.SetTransactionId( ++iTransactionId );

        iPendingRequestRoot.AppendToLast( req );
        TInt ret = ProcessNextPendingRequest();
        while ( KErrNone == ret )
            {
            ret = ProcessNextPendingRequest();
            }
        }
    else
        {
        err = KErrNoMemory;
        }
    VVHW_TRACE("DDeviceReqHandler::HandleExecuteAsync <-");
    return err;
    }

// -----------------------------------------------------------------------------
// DDeviceReqHandler::ProcessDfc
// -----------------------------------------------------------------------------
//
void DDeviceReqHandler::ProcessDfc()
    {
    VVHW_TRACE("DDeviceReqHandler::ProcessDfc");
    TUint32 error( 0 );
    iHwInterface.GetRegisterValue( DVirtualVideoHwInterface::ERegError, error );

    TUint32 transactionId;
    iHwInterface.GetRegisterValue( DVirtualVideoHwInterface::ERegRequestId, transactionId );

    TAsyncRequest* req = iSerializedRequestRoot.iNext;
    VVHW_TRACE( "DDeviceReqHandler::ProcessDfc req %u transactionid %u", req, transactionId );
    

    if ( req && transactionId == req->iRemoteFunctionCall.Header().iTransactionId )
        {
        req->RemoveFromList( &iSerializedRequestRoot );
        
        //Check if the request was scheduler-initiated
        TBool driverInitiated(EFalse);
        if( iSchedulerInitiatedTransactionIds.Count() > 0 )
            {
            for( int i=0;i<iSchedulerInitiatedTransactionIds.Count();++i )
                {
                if( iSchedulerInitiatedTransactionIds[i].iTransactionId == req->iRemoteFunctionCall.Header().iTransactionId )
                    {
                driverInitiated = ETrue;
                    //This will be used in the "consume" function later on in many cases
                    iSchedulerInitiatedTransactionData = iSchedulerInitiatedTransactionIds[i].iTransactionData; 
                    iSchedulerInitiatedTransactionIds.Remove( i );
                    break;
                    }
                }
            }
        
        if( driverInitiated )
            {
            //This branch is for the purpose of handling commands that have been initiated from (this) 
            //Command Scheduler, and not a client thread. So far no cases exists, where this would
            //imply notifying a (client) DLL, so RequestComplete does not have to be called here (this could change later).
            //Parse result and write vectors back to client space
            if ( 0 == error )
                {
                //Parse result and write vectors back to client space        
                TDataToClientSpaceCopier datacopier2( req->iAsyncClient );
                RemoteFunctionCallData rfc;
                SerializedFunctionCall sfc( rfc );
                sfc.SetDataCopier( &datacopier2 );
                const TLinAddr paramAddr( iHwInterface.OutputParametersAddress() );
                VVHW_TRACE("DDeviceReqHandler::ProcessDfc parse");
                sfc.ParseBuffer( reinterpret_cast<TUint8*>( paramAddr ), VVI_PARAMETERS_OUTPUT_MEMORY_SIZE );
                TPckg<RemoteFunctionCallData> resultbuf( rfc );
                req->iRemoteFunctionCall = resultbuf();//Get the object; its in the same address space 
                VVHW_TRACE("DDeviceReqHandler::ProcessDfc ThreadDesWrite err=%d (data len=%d)", error, resultbuf.Length());
                sfc.SetDataCopier( NULL );
                }

            error = ResetAndEnableInterrupt( DVirtualVideoHwInterface::EInterruptNewDataAvailable );
            ConsumeSchedulerInitiatedRequestResult( req );
            }
        else
            {
			TInt desWriteError = KErrNone;
            if ( 0 == error )
                {
                //Parse result and write vectors back to client space
				// TODO for performance only write back result & output vectors, not whole request buffer
                TDataToClientSpaceCopier datacopier2( req->iAsyncClient );
                RemoteFunctionCallData rfc;
                SerializedFunctionCall sfc( rfc );
                sfc.SetDataCopier( &datacopier2 );
                const TLinAddr paramAddr( iHwInterface.OutputParametersAddress() );
                VVHW_TRACE("DDeviceReqHandler::ProcessDfc parse");
                sfc.ParseBuffer( reinterpret_cast<TUint8*>( paramAddr ), VVI_PARAMETERS_OUTPUT_MEMORY_SIZE );
                TPckg<RemoteFunctionCallData> resultbuf( rfc );
                desWriteError = Kern::ThreadDesWrite( req->iAsyncClient, req->iA1, resultbuf, 0, 0 );
                VVHW_TRACE("DDeviceReqHandler::ProcessDfc ThreadDesWrite err=%d (data len=%d)",desWriteError, resultbuf.Length());
                sfc.SetDataCopier( NULL );
                }

            Kern::RequestComplete( req->iAsyncClient, req->iStatus, desWriteError );
            ResetAndEnableInterrupt( DVirtualVideoHwInterface::EInterruptNewDataAvailable );
            ReleaseRequest( req );
            }
        }
    else
        {
        ResetAndEnableInterrupt( DVirtualVideoHwInterface::EInterruptNewDataAvailable );
        }
    VVHW_TRACE("DDeviceReqHandler::ProcessDfc ProcessNextPendingRequest");
    
    TInt ret = ProcessNextPendingRequest();
    while ( KErrNone == ret )
        {
        ret = ProcessNextPendingRequest();
        }
    }

// -----------------------------------------------------------------------------
// DDeviceReqHandler::AllocRequest
// -----------------------------------------------------------------------------
//
DDeviceReqHandler::TAsyncRequest* DDeviceReqHandler::AllocRequest( TRequestStatus* aStatus,
        DThread* aAsyncClient,
        TAny* aA1,
        TAny* aA2,
        DDeviceReqHandler::TAsyncRequest::TRequestType aRequestType )
    {
    TAsyncRequest* req( NULL );
    if ( iFreeCount )
        {
        req = iFreeListRoot.iNext;
        req->RemoveFromList( &iFreeListRoot );
        req->Init( aStatus, aAsyncClient, aA1, aA2, aRequestType );
        iFreeCount--;
        }
    else
        {
        VVHW_TRACE("DDeviceReqHandler::AllocRequest new");
        req = new TAsyncRequest( aStatus, aAsyncClient, aA1, aA2, aRequestType );
        }
    return req;
    }

// -----------------------------------------------------------------------------
// DDeviceReqHandler::ReleaseRequest
// -----------------------------------------------------------------------------
//
void DDeviceReqHandler::ReleaseRequest( DDeviceReqHandler::TAsyncRequest* aReq )
    {
    if ( iFreeCount > KMaxFreeRequests )
        {
        VVHW_TRACE("DDeviceReqHandler::AllocRequest delete");
        delete aReq;
        }
    else
        {
        aReq->AppendToList( &iFreeListRoot );
        iFreeCount++;
        }
    }



void DDeviceReqHandler::RemoveClientData( const TUint32 aProcId, const TUint32 aThreadId )
    {
    VVHW_TRACE("DDeviceReqHandler::RemoveClientData (%d/%d)", aProcId, aThreadId );
    TPerThreadData* data( NULL );
    for( TInt i=0; i < iClientData.Count(); ++i )
        {
        if( iClientData[i]->sameThread( aProcId, aThreadId ) )
            {
            data = iClientData[i];
            iClientData.Remove(i);
            if( data == iCurrClientData )
                {
                iCurrClientData = 0;
                }
            delete data;
            break;
            }
        }
    }

TBool DDeviceReqHandler::InitiateRequestWithReply( TAsyncRequest* aRec, TAny* aTransactionData )
    {
    VVHW_TRACE("DDeviceReqHandler::InitiateRequestWithReply" );
    if( iDriverRfcThreadId == 0 || iDriverRfcProcessId == 0 )
        {
        iDriverRfcProcessId = Kern::CurrentProcess().iId;
        iDriverRfcThreadId  = Kern::CurrentThread().iId;
        }
    //iReqIssueMutex.Wait();
    const TUint32 serializedLength( aRec->iRemoteFunctionCall.SerialisedLength() );
    if ( !iRequestBuffer.CheckForSpace( serializedLength ) )
       {
       return EFalse;
       }
    ++iTransactionId;
    aRec->iRemoteFunctionCall.SetTransactionId( iTransactionId );
    aRec->iRemoteFunctionCall.SetOperationType( RemoteFunctionCallData::EOpRequestWithReply );
    aRec->iAsyncClient = &(Kern::CurrentThread());
    aRec->iRemoteFunctionCall.SetThreadInformation( iDriverRfcProcessId, iDriverRfcThreadId );
    
    iSchedulerInitiatedTransactionIds.Append( TTransactionInfo( iTransactionId, aTransactionData ) );
    VVHW_TRACE("DDeviceReqHandler::InitiateRequestWithReply ids len=%d transaction id = %u proc/thread = %u / %u api/op = %0x / %0x", iSchedulerInitiatedTransactionIds.Count(), iTransactionId, Kern::CurrentProcess().iId, Kern::CurrentThread().iId, aRec->iRemoteFunctionCall.Header().iApiUid, aRec->iRemoteFunctionCall.Header().iOpCode );
    if( aRec->iRemoteFunctionCall.Header().iApiUid )
        {
        VVHW_TRACE("DDeviceReqHandler::InitiateRequestWithReply request's api is SERIALISED_DRIVER_API_UID" );
        }
    
    iSerializedRequestRoot.AppendToLast( aRec );//we need the result, so add to the dfc handled requests
    TDataFromClientSpaceCopier datacopier( aRec->iAsyncClient );
    SerializedFunctionCall sfc( aRec->iRemoteFunctionCall );
    
    sfc.SetDataCopier( &datacopier );//We know this is a "request" (precondition)
    const TUint32 base( iRequestBuffer.AllocateBytes( serializedLength ) );
    //Next, serialize the call to the HW memory
    const TLinAddr paramAddr( iHwInterface.InputParametersAddress() + base );
    TInt len = sfc.WriteToBuffer( reinterpret_cast<TUint8*>(paramAddr), VVI_PARAMETERS_INPUT_MEMORY_SIZE - 1 - base );
    VVHW_TRACE("DDeviceReqHandler::InitiateRequestWithReply len = %d", len);
    sfc.SetDataCopier( NULL );
    iRequestBuffer.CommitBytes( base, serializedLength );
    iHwInterface.IssueCommand( VVI_EXECUTE );
    VVHW_TRACE("DDeviceReqHandler::InitiateRequestWithReply issued");
    return ETrue;
    }

/**
 * This routine adds extra info about the sgImage's pbuffer handle for syncing,
 * when that is required. 
 * 1. checks the sgImage handle, and if it's not NULL, then
 *  2. Get the sgImage's metadata
 *  3. Check if the vgImage is "dirty", and if yes,
 *      then append the pbuffer handle as a parameter to the call   
 *  4. Set the pbuffer "dirty", if aSetBufferDirty is ETrue
 */

void DDeviceReqHandler::getVGSyncInOp( TAsyncRequest* aReq, TInt aSgHandleIndexInReq, TBool aSetBufferDirty )
    {    
    VVHW_TRACE( "DDeviceReqHandler::getVGSyncInOp" );
    EGLSurface surface( EGL_NO_SURFACE );//The pbuffer surface to sync from, if needed
    VGboolean syncNeeded = VG_FALSE;
    OpenVgRFC call( aReq->iRemoteFunctionCall );
    TUint64 sgId(NULL);
    
    if( call.Data().Header().iParameterCount > aSgHandleIndexInReq )
        {
        call.GetTUint64( sgId, aSgHandleIndexInReq );
        }
    else
        {
        return;//No sgImage handle appended on the client side, just exit
        }
    
#ifdef FAISALMEMON_S4_SGIMAGE
    if( sgId != NULL )
        {
        VVHW_TRACE( "DDeviceReqHandler::getVGSyncInOp SgImage-backing VGImage found" );
        DSgResource* resource;
        HBuf8* data = OpenSgImageMetaData( sgId, resource );
        if( data )
            {
            TSgImageMetaData sginfo (((TPckgBuf<TSgImageMetaData>*) data)->operator ()());
            
            if( !sginfo.iVGImageClean )
                {
                //set the sync bit as clean now.
                sginfo.iVGImageClean = ETrue;
                //Additional info for the host side to use
                syncNeeded = VG_TRUE;
                surface = sginfo.iPbufferHandle;
                }
            if( aSetBufferDirty )
                {
                sginfo.iPbufferClean = EFalse;
                }
            resource->SetMetaData( *data );
            delete data;//delete the copy of the metadata descriptor
            }
        }
    if( syncNeeded )
        {
        call.AppendParam ( (int)surface );
        }
#endif
    }


void DDeviceReqHandler::ConsumeSchedulerInitiatedRequestResult( TAsyncRequest* aReq )
    {
    const TUint32 proc_id( aReq->iRemoteFunctionCall.Header().iProcessId );
    const TUint32 thread_id( aReq->iRemoteFunctionCall.Header().iThreadId );
    const TUint32 apicode( aReq->iRemoteFunctionCall.Header().iApiUid );
    const TUint32 opcode( aReq->iRemoteFunctionCall.Header().iOpCode );
    VVHW_TRACE( "DDeviceReqHandler::ConsumeSchedulerInitiatedRequestResult: process/thread id : %d / %d", Kern::CurrentProcess().iId, Kern::CurrentThread().iId );
    VVHW_TRACE( "DDeviceReqHandler::ConsumeSchedulerInitiatedRequestResult: process/thread id : %d / %d API/opcode = %0x/%0x", proc_id, thread_id, apicode, opcode );
    TPerThreadData* data(0);
    for( TInt i=0; i < iClientData.Count(); ++i )
        {
        if( iClientData[i]->sameThread( proc_id, thread_id ))
            {
            data = iClientData[i];
            break;
            }
        }
    
    switch ( apicode )
        {
        case SERIALISED_DRIVER_API_UID:
            {
            switch( opcode )
                {
                case DriverRFC::EDrvCreatePbufferSg:
                    {
                    VVHW_TRACE( "DDeviceReqHandler::ConsumeSchedulerInitiatedRequestResult : EDrvCreatePbufferSg" );
#ifdef FAISALMEMON_S4_SGIMAGE
                    if( iSchedulerInitiatedTransactionData )
                        {
                        ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iPbufferHandle = aReq->iRemoteFunctionCall.Header().iReturnValue;
                        VVHW_TRACE( "DDeviceReqHandler::ConsumeSchedulerInitiatedRequestResult pbuffer=%u", ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iPbufferHandle );
                        }
                    TPbSId obj ( ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iPbufferHandle,
                                 ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iSgId );
                    iPbufferSgMap.InsertInOrder( obj, iPbufferSgMapOrder );
                    VVHW_TRACE( "pbuffer creation RequestComplete. Inserted (%u, %u) into sg map. Notify thread %u, request status %u",
                                                                    ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iPbufferHandle,
                                                                    ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iSgId,
                                                                    aReq->iAsyncClient,
                                                                    aReq->iStatus );
                    *(aReq->iStatus) = 0;//REquestComplete doesn't work
#endif
                    VVHW_TRACE( "Pbuffer creation RequestComplete" );
                    break;
                    }
                case DriverRFC::EDrvCreateVGImageSg:
                    {
                    VVHW_TRACE( "DDeviceReqHandler::ConsumeSchedulerInitiatedRequestResult : EDrvCreateVGImageSg" );
#ifdef FAISALMEMON_S4_SGIMAGE
                    if( iSchedulerInitiatedTransactionData )
                        {
                        ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iVGImageHandle = aReq->iRemoteFunctionCall.Header().iReturnValue;
                        }
                    TVGISId obj ( ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iVGImageHandle,
                                 ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iSgId );
                    iVGImageSgMap.InsertInOrder( obj, iVGImageSgMapOrder );
                    VVHW_TRACE( "pbuffer creation RequestComplete. Inserted (%u, %u) into sg map. Notify thread %u, request status %u",
                               ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iVGImageHandle,
                               ((TSgImageMetaData*)iSchedulerInitiatedTransactionData)->iSgId,
                               aReq->iAsyncClient,
                               aReq->iStatus );
                    *(aReq->iStatus) = 0;//REquestComplete doesn't work
#endif
                    VVHW_TRACE( "VGImage creation RequestComplete" );
                    break;
                    }
                case DriverRFC::EDrvDeleteSgImage:
                    {
                    *(aReq->iStatus) = 0;
                    }
                case DriverRFC::EDrvSyncVGImageFromPBuffer:
                    {
                    ((TAsyncRequest*)iSchedulerInitiatedTransactionData)->AppendToList( (TAsyncRequest*)(&iPendingRequestRoot) );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            }
        case SERIALISED_OPENVG_API_UID:
            {
            switch( opcode )
                {
                case OpenVgRFC::EvgGetError:
                    {
                    VVHW_TRACE("OpenVgRFC::EvgGetError");
                    data->iErrorVG = aReq->iRemoteFunctionCall.Header().iReturnValue;
                    data->iErrUpdatedVG = EFalse;//TODO: ETrue
                    break;
                    }
                default:{break;}
                }
            break;
            }
        case SERIALISED_EGL_API_UID:
            {
            switch( opcode )
                {
                case EglRFC::EeglGetError:
                    {
                    VVHW_TRACE("EglRFC::EeglGetError");
                    data->iErrorEGL = aReq->iRemoteFunctionCall.Header().iReturnValue;
                    data->iErrUpdatedEGL = EFalse;//TODO: ETrue
                    break;
                    }
                default:
                    {break;}
                }
            break;
            }
        case SERIALISED_OPENGLES_1_1_API_UID:
            {
            switch( opcode )
                {
                case OpenGlES11RFC::EglGetError:
                    {
                    VVHW_TRACE("OpenGlES11RFC::EglGetError");
                    data->iErrorGLES = aReq->iRemoteFunctionCall.Header().iReturnValue;
                    data->iErrUpdatedGLES = EFalse;//TODO: ETrue
                    }
                default:{break;}
                }
            break;
            }
        default:{break;}
        }

    VVHW_TRACE( "DDeviceReqHandler::ConsumeSchedulerInitiatedRequestResult return value: %d", aReq->iRemoteFunctionCall.Header().iReturnValue );
    if( data )
        {
        VVHW_TRACE("errors EGL/VG/GLES=%0x/%0x/%0x", data->iErrorEGL, data->iErrorVG, data->iErrorGLES );
        }
    //Delete the request object, as it is no longer needed
    aReq->iStatus = 0;
    aReq->iAsyncClient = 0;
    delete aReq;
    VVHW_TRACE("DDeviceReqHandle::ConsumeSchedulerInitiatedRequestResult done.");
    }

#ifdef FAISALMEMON_S4_SGIMAGE
/**
 * DDeviceReqHandler::CreateSgImagePbuffer
 * @param aInfo the info 
 */
void DDeviceReqHandler::CreateSgImagePbuffer( const TSgImageMetaData& aInfo, TRequestStatus* aStatus, DThread* aThread )
    {
    //eglCreatePbufferSurface
    //
    VVHW_TRACE("DDeviceReqHandler::CreateSgImagePbuffer ->");
    if( !iCurrClientData )
        {return;}
    TBool allok = ETrue;
    TAsyncRequest* req(0);
    TPckg<TUint32> res( iOpReturn );
    *aStatus = KRequestPending;
    VVHW_TRACE("DDeviceReqHandler::CreateSgImagePbuffer request from thread %u, requeststatus %u", aThread, aStatus );
    req = AllocRequest( aStatus, aThread, NULL, (TAny*)(&res), TAsyncRequest::ERTRequest );
    if( req )
        {
        DriverRFC call( req->iRemoteFunctionCall );
        call.Init( DriverRFC::EDrvCreatePbufferSg );
        
        call.AppendEGLint  ( aInfo.iSizeInPixels.iWidth );
        call.AppendEGLint  ( aInfo.iSizeInPixels.iHeight );
        call.AppendEGLint  ( aInfo.iPixelFormat );

        TPckg<RemoteFunctionCallData> data( req->iRemoteFunctionCall );
        req->iA1 = (TAny*)( &data );
        allok = InitiateRequestWithReply( req, (TAny*)(&aInfo) );
        }
    if( !allok )
        {
        //request not sent, or something else went wrong. Tell the client its over
        //TODO: somehow dispose of the pbuffer that might have been created
        }
    while( *(aStatus) == KRequestPending )
        {
        NKern::Sleep( 20 );   
        }
    VVHW_TRACE("DDeviceReqHandler::CreateSgImagePBuffer <-");
    return;
    }

void DDeviceReqHandler::CreateSgImageVGImage( const TSgImageMetaData& aInfo, TRequestStatus* aStatus, DThread* aThread )
    {
    //eglCreatePbufferSurface
    //
    VVHW_TRACE("DDeviceReqHandler::CreateSgImageVGImage ->");
    if( !iCurrClientData )
        {return;}
    TBool allok = ETrue;
    TAsyncRequest* req(0);
    TPckg<TUint32> res( iOpReturn );
    *aStatus = KRequestPending;
    //make a request for the vgImage
    req = AllocRequest( aStatus, aThread, NULL, (TAny*)(&res), TAsyncRequest::ERTRequest );
    if( req )
        {
        DriverRFC call( req->iRemoteFunctionCall );
        call.Init( DriverRFC::EDrvCreateVGImageSg );
        
        call.AppendEGLint  ( aInfo.iSizeInPixels.iWidth );
        call.AppendEGLint  ( aInfo.iSizeInPixels.iHeight );
        call.AppendEGLint  ( aInfo.iPixelFormat );

        TPckg<RemoteFunctionCallData> data( req->iRemoteFunctionCall );
        req->iA1 = (TAny*)(&data);
        allok = InitiateRequestWithReply( req, (TAny*)(&aInfo) );
        }
    if( !allok )
        {
        //request not sent, or something else went wrong. Tell the client its over
        //TODO: somehow dispose of the image that might have been created
        //Kern::RequestComplete( aStatus, KErrGeneral );
        }
    while( *(aStatus) == KRequestPending )
        {
        NKern::Sleep( 20 );
        }
    VVHW_TRACE("DDeviceReqHandler::CreateSgImageVGImage <-");
    return;
    }
#endif

TInt DDeviceReqHandler::DestroySgImage( const TUint64 aId )
    {
    VVHW_TRACE("DDeviceReqHandler::DestroySgImage ->");
    TBool allok = ETrue;
    TAsyncRequest* req(0);
    TPckg<TUint32> res( iOpReturn );
    EGLSurface surface(0);
    VGImage    image(0);
    
#ifdef FAISALMEMON_S4_SGIMAGE
    DSgResource* resource;
    HBuf8* data = OpenSgImageMetaData( aId, resource );
    if( data )
        {
        TSgImageMetaData sginfo (((TPckgBuf<TSgImageMetaData>*) data)->operator ()());
        surface = sginfo.iPbufferHandle;
        //TODO: are there other cases: e.g. can the vgimage be destroyed without destroying the surface?
        if( sginfo.iUsage || ESgUsageBitOpenVgImage )
            {
            image = sginfo.iVGImageHandle;
            }
        delete data;
        }
    TRequestStatus status = KRequestPending;
    //make a request for the vgImage
    req = AllocRequest( &status, &Kern::CurrentThread(), NULL, (TAny*)(&res), TAsyncRequest::ERTRequest );
    allok = (req?EFalse:ETrue);
    if( req )
        {
        DriverRFC call( req->iRemoteFunctionCall );
        call.Init( DriverRFC::EDrvDeleteSgImage );
        
        call.AppendEGLSurface( surface );
        call.AppendVGParam ( image );

        TPckg<RemoteFunctionCallData> data( req->iRemoteFunctionCall );
        req->iA1 = (TAny*)(&data);
        allok = InitiateRequestWithReply( req, 0 );
        }
    if( !allok )
        {
        //TODO
        }
    while( status == KRequestPending )
        {
        NKern::Sleep( 20 );
        }
#endif

    VVHW_TRACE("DDeviceReqHandler::DestroySgImage <-");
    return 1;
    }


DDeviceReqHandler::TRequestMode DDeviceReqHandler::InterpretRequest( TAsyncRequest* aReq )
    {
    TRequestMode alreadyProcessed( ENormal );//processed here? The return value
    const TUint32 proc_id( aReq->iRemoteFunctionCall.Header().iProcessId );
    const TUint32 thread_id( aReq->iRemoteFunctionCall.Header().iThreadId );
    const TUint32 apicode( aReq->iRemoteFunctionCall.Header().iApiUid );
    const TUint32 opcode( aReq->iRemoteFunctionCall.Header().iOpCode );
    
    VVHW_TRACE( "DDeviceReqHandler::InterpretRequest in the process/thread %0x / %0x", Kern::CurrentProcess().iId, Kern::CurrentThread().iId );
    
    TBool ErrUpdatedVG = EFalse;//is the error updated during this function call
    TBool ErrUpdatedEGL = EFalse;//is the error updated during this function call
    TBool ErrUpdatedGLES = EFalse;//is the error updated during this function call
    //Get a data object to work with
    //(either a previously used client data object, or create a new one)
    TPerThreadData* data(NULL);
    if( iCurrClientData &&
        iCurrClientData->sameThread( proc_id, thread_id ))
        {
        //the same client as previously
        data = iCurrClientData;
        }
    else
        {
        //Check if this client is already known
        for( TInt i=0; i < iClientData.Count(); ++i )
            {
            if( iClientData[i]->sameThread( proc_id, thread_id ))
                {
                data = iClientData[i];
                break;
                }
            }
        if( !data )
            {
            data = new TPerThreadData( aReq->iStatus, aReq->iAsyncClient, proc_id, thread_id );
            iClientData.Append( data );
            }
        if( iCurrClientData )
            {
            VVHW_TRACE( "DDeviceReqHandler::InterpretRequest: SWITCHING CONTEXT: %d / %d -> %d / %d",
                    iCurrClientData->iProcessId,
                    iCurrClientData->iThreadId,
                    proc_id,
                    thread_id );
            //Some other things need to be done here as well, as per 5.2 in the "Implementing SgImage" design doc
            //TODO: currently the context switch does not need to be done on this level
            }
        //Context switched, or no previous client. Current set to new current.
        iCurrClientData = data;
        }
    //Invariant: "data" and "iCurrClient" are the same and not null after this line
    VVHW_TRACE("DDeviceReqHandler::InterpretRequest: number of clients = %d", iClientData.Count() );
    
    
    iCurrClientData->iCurrApiUid = apicode;
    iCurrClientData->iCurrOpCode = opcode;
    VVHW_TRACE( "DDeviceReqHandler::InterpretRequest: process/thread id : %d / %d API/opcode = %d/%0x", proc_id, thread_id, apicode, opcode );
    
    
    switch ( apicode )
        {
        case SERIALISED_DRIVER_API_UID:
            {
            VVHW_TRACE("Driver request (must be a shutdown for %d / %d)", proc_id, thread_id  );
            if( opcode == DriverRFC::EDrvClientShutdown )
                {
                RemoveClientData( proc_id, thread_id );
                }
            break;
            }
        case SERIALISED_OPENVG_API_UID:
            {
            VVHW_TRACE("OpenVG request" );
            switch( opcode )
                {
                case OpenVgRFC::EvgGetError:
                    {
                    break;
                    }
                //We need to catch all the operations that create or access VGImage buffers
                case OpenVgRFC::EvgChildImage:
                    {
                    VVHW_TRACE("vgChildImage" );
                    break;
                    }
                case OpenVgRFC::EvgDestroyImage:
                    {
                    VVHW_TRACE("vgDestroyImage" );
                    break;
                    }
                case OpenVgRFC::EvgFinish:
                    {
                    VVHW_TRACE("vgFinish" );
                    //A sync is required if there are any native pixmaps backing surfaces in the current context
                    break;
                    }
                /* 
                The sgImage ID is a 64-bit value, it has to be serialised as 2 32-bit values, thus occupying
                2 parameter slots each. The sgImage ID position is thus the second last + last parameters in the 
                parameter list
                */ 
                case OpenVgRFC::EvgClearImage:
                    {
                    VVHW_TRACE("vgClearImage" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );
                    break;
                    }
                case OpenVgRFC::EvgImageSubData:
                    {
                    VVHW_TRACE("vgImageSubData" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );
                    break;
                    }
                case OpenVgRFC::EvgGetPixels:
                    {
                    VVHW_TRACE("vgGetPixels" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );
                    break;
                    }
                case OpenVgRFC::EvgSetPixels:
                    {
                    VVHW_TRACE("vgSetPixels" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );
                    break;
                    }
                case OpenVgRFC::EvgCopyImage:
                    {
                    VVHW_TRACE("vgCopyImage" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );
                    break;
                    }
                case OpenVgRFC::EvgDrawImage:
                    {
                    VVHW_TRACE("vgDrawImage" );
                    break;
                    }
                case OpenVgRFC::EvgColorMatrix:
                    {
                    VVHW_TRACE("vgColorMatrix" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-4 );//dst
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );//src
                    break;
                    }
                case OpenVgRFC::EvgConvolve:
                    {
                    VVHW_TRACE("vgConvolve" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-4 );//dst
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );//src
                    break;
                    }
                case OpenVgRFC::EvgSeparableConvolve:
                    {
                    VVHW_TRACE("vgSeparableConvolve" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-4 );//dst
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );//src
                    break;
                    }
                case OpenVgRFC::EvgGaussianBlur:
                    {
                    VVHW_TRACE("vgGaussianBlur" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-4 );//dst
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );//src
                    break;
                    }
                case OpenVgRFC::EvgLookup:
                    {
                    VVHW_TRACE("vgLookup" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-4 );//dst
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );//src
                    break;
                    }
                case OpenVgRFC::EvgLookupSingle:
                    {
                    VVHW_TRACE("vgLookupSingle" );
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-4 );//dst
                    getVGSyncInOp( aReq, aReq->iRemoteFunctionCall.Header().iParameterCount-2 );//src
                    break;
                    }
                case OpenVgRFC::EvgPathBounds:
                    {
                    VVHW_TRACE("vgPathBounds" );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }
        case SERIALISED_EGL_API_UID:
            {
            VVHW_TRACE("EGL request" );
            switch( opcode )
                {
                case EglRFC::EeglMetaSgGetHandles:
                    {
                    VVHW_TRACE("EglRFC::EeglMetaSgGetHandles" );
                    EglRFC call( aReq->iRemoteFunctionCall );
                    TUint64 id(0);
                    EGLint* handles;
                    EGLint handles_size;
                    
                    call.GetTUint64( id, 0 );
                    call.GetEGLintVectorData( handles, handles_size, 2 );
#ifdef FAISALMEMON_S4_SGIMAGE
                    DSgResource* resource;
                    VVHW_TRACE("EglRFC::EeglMetaSgGetHandles openSgImageMetaData" );
                    HBuf8* data = OpenSgImageMetaData( id, resource );
                    if( data && handles_size >= 2 )
                        {
                        TSgImageMetaData sginfo (((TPckgBuf<TSgImageMetaData>*) data)->operator ()());
                        handles[0] = sginfo.iPbufferHandle;
                        handles[1] = sginfo.iVGImageHandle;
                        call.SetReturnValue( (int)EGL_TRUE );
                        }
                    else
                        {
                        call.SetReturnValue( (int)EGL_FALSE );
                        }
                    alreadyProcessed = EHandled;
#endif
					break;
                    }
                case EglRFC::EeglCreateContext:
                    {
                    //if no share_context specified, pass the sgImage pool context
                    VVHW_TRACE("eglCreateContext" );
                    break;
                    }
                case EglRFC::EeglMakeCurrent:
                    {
                    EGLDisplay dpy;
                    EGLSurface draw;
                    EGLSurface read;
                    EGLContext ctx;
                    EGLint syncRequirement(0);//no sync = 0
                    
                    EglRFC call( aReq->iRemoteFunctionCall );
                    call.GetEGLDisplay( dpy, 0 );
                    call.GetEGLSurface( draw, 1 );
                    call.GetEGLSurface( read, 2 );
                    call.GetEGLContext( ctx, 3 );
                    VGImage img = 0;
                    
                    TPbSId obj ( draw, 0 );

                    TInt sgIdIndex = iPbufferSgMap.FindInOrder( obj, iPbufferSgMapOrder );
                    if( sgIdIndex != KErrNotFound )
                        {
#ifdef FAISALMEMON_S4_SGIMAGE
                        DSgResource* resource;
                        HBuf8* data = OpenSgImageMetaData( iPbufferSgMap[sgIdIndex].iSgId, resource );
                        if( data )
                            {
                            TSgImageMetaData sginfo (((TPckgBuf<TSgImageMetaData>*) data)->operator ()());
                            
                            if( !sginfo.iPbufferClean )
                                {
                                img = sginfo.iVGImageHandle;
                                //sync with the underlying pbuffer surface
                                syncRequirement = syncRequirement | KSyncDrawSurface;
                                call.AppendEGLint( img );
                                }
                            delete data;
                            }
#endif
                        }
                    obj.iPbuffer = read;
                    obj.iSgId = 0;

                    sgIdIndex = iPbufferSgMap.FindInOrder( obj, iPbufferSgMapOrder );
                    if( sgIdIndex != KErrNotFound )
                        {
#ifdef FAISALMEMON_S4_SGIMAGE
                        DSgResource* resource;
                        HBuf8* data = OpenSgImageMetaData( iPbufferSgMap[sgIdIndex].iSgId, resource );
                        if( data )
                            {
                            TSgImageMetaData sginfo (((TPckgBuf<TSgImageMetaData>*) data)->operator ()());
                            
                            if( !sginfo.iPbufferClean )
                                {
                                img = sginfo.iVGImageHandle;
                                //sync with the underlying pbuffer surface
                                syncRequirement = syncRequirement | KSyncReadSurface;
                                }
                            delete data;
                            }
#endif
                        }
                    call.AppendEGLint( syncRequirement );
                    call.AppendEGLint( img );
                    
                    VVHW_TRACE("eglMakeCurrent %u", iCurrClientData->iContext );
                    break;
                    }
                case EglRFC::EeglMakeCurrentSg:
                    {
                    VVHW_TRACE("eglMakeCurrentSg" );
                    break;
                    }
                case EglRFC::EeglCreatePixmapSurfaceSg:
                    {
                    alreadyProcessed = EHandled;//This will be handled here
                    
                    TUint64 sgId;
                    EglRFC call( aReq->iRemoteFunctionCall );
                    call.GetTUint64( sgId, 0 );//get the sgImage id
                    
#ifdef FAISALMEMON_S4_SGIMAGE
                    DSgResource* resource;
                    HBuf8* data = OpenSgImageMetaData( sgId, resource );
                    if( data )
                        {
                            TSgImageMetaData sginfo (((TPckgBuf<TSgImageMetaData>*) data)->operator ()());
                            resource->SetMetaData( (*data) );
                            aReq->iRemoteFunctionCall.SetReturnValue( sginfo.iPbufferHandle );
                            delete data;
                        }
                    else
                        {
                        aReq->iRemoteFunctionCall.SetReturnValue( EGL_NO_SURFACE );
                        }
                    //Find the sgimage's pbuffer surface, then return that
#endif
                    VVHW_TRACE( "EeglCreatePixmapSurfaceSg" );
                    break;
                    }
                case EglRFC::EeglCreatePbufferFromClientBuffer:
                    {
                    //if a sgImage-backing VGImage handle is passed here, then fail with EGL_BAD_ACCESS
                    
                    VVHW_TRACE("eglCreatePbufferFromClientBuffer" );
                    break;
                    }
                case EglRFC::EeglGetError:
                    {
                    VVHW_TRACE("eglGetError" );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }//switch opcode (EGL)
                
            break;
            }
        case SERIALISED_OPENGLES_1_1_API_UID:
            {
            VVHW_TRACE("OpenGLES request" );
            switch( opcode )
                {
                case OpenGlES11RFC::EglGetError:
                    {
                    //Get the GLES error
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }//switch apicode
    //VVHW_TRACE( "DDeviceReqHandler::InterpretRequest: opcode : %d ", data->iCurrOpCode );
    if( iCurrClientData )//This could have been removed, in a call to RemoveClientData
        {
        iCurrClientData->iErrUpdatedVG = ErrUpdatedVG;
        iCurrClientData->iErrUpdatedEGL = ErrUpdatedEGL;
        iCurrClientData->iErrUpdatedGLES = ErrUpdatedGLES;
        }
    return alreadyProcessed;
    }

#ifdef FAISALMEMON_S4_SGIMAGE
HBuf8* DDeviceReqHandler::OpenSgImageMetaData( const TUint64 aId, DSgResource*& aResource )
    {
    VVHW_TRACE("DDeviceReqHandler::OpenSgImageMetaData 0x%lx", aId );
    aResource = 0;
    HBuf8* data(0);
    TInt sgOpenErr( KErrNone );
    
    sgOpenErr = SgExtension::FindAndOpenResource( aId, aResource );//Open the sgimage resource
    VVHW_TRACE(" sgOpenErr %d", sgOpenErr);
    if( sgOpenErr == KErrNone )
        {
        data = HBuf8::New( aResource->GetMetaDataSize() );
        if( data )
            {
            aResource->GetMetaData( (*data) );
            }
        }
    return data;//If there was a problem, this is zero. Ownership belongs to caller
    }
#endif
