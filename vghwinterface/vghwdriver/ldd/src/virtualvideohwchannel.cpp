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
// Virtual Video Hardware Channel implementation

#include <kernel/kernel.h>
#include <assp.h>

#include <graphics/virtualvideohwchannel.h>
#include <graphics/virtualvideohwdeviceinterface.h>
#include <graphics/guestvideodriverprotocol.h>
#include <graphics/guestvideodrivercommon.h>
#include <graphics/devicereqhandler.h>

#include "serializedfunctioncall.h"

// -----------------------------------------------------------------------------
// DVirtualVideoHwChannel::DVirtualVideoHwChannel
// -----------------------------------------------------------------------------
//
DVirtualVideoHwChannel::DVirtualVideoHwChannel(
    MVirtualVideoHwDeviceInterface& aDeviceInterface,
    DVirtualVideoHwInterface& aHwInterface,
    DDeviceReqHandler& aDeviceReqHandler,
    TDfcQue* aQueue ) :
    iDeviceInterface( aDeviceInterface ),
    iHwInterface( aHwInterface ),
    iReqHandler( aDeviceReqHandler )
    {    
    SetDfcQ( aQueue );
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwChannel::~DVirtualVideoHwChannel
// -----------------------------------------------------------------------------
//
DVirtualVideoHwChannel::~DVirtualVideoHwChannel()
    {
    // Enter critical section
    NKern::ThreadEnterCS();
    iMsgQ.CompleteAll( KErrAbort );
    // Leave critical section
    NKern::ThreadLeaveCS();
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwChannel::Request
// -----------------------------------------------------------------------------
//
TInt DVirtualVideoHwChannel::Request( TInt aReqNo, TAny *a1, TAny *a2 )
    {
    TInt result = DLogicalChannel::Request( aReqNo, a1, a2 );
    return result;
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwChannel::HandleMsg
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwChannel::HandleMsg( TMessageBase* aMsgRequest )
    {
    DThread* userThread = aMsgRequest->Client();
    TThreadMessage& msg = *(TThreadMessage*)aMsgRequest;
    // Default error condition
    TInt err = KErrNone; 

    if ( msg.iValue == ECloseMsg )
        {
        iReqHandler.HandleClientShutdown( userThread->iOwningProcess->iId, userThread->iId );
        msg.Complete( KErrNone, EFalse );
        return;
        }
    
    if ( msg.iValue < 0 )
        {
        TRequestStatus* status = (TRequestStatus*)msg.Ptr0();
        err = DoRequest( userThread, ~msg.iValue, status, msg.Ptr1(), msg.Ptr2() );
        if( err != KErrNone )
            {
            Kern::RequestComplete( userThread, status, err );
            }
        err = KErrNone;
        }
    else
        {
        err = DoControl( userThread, msg.iValue, msg.Ptr0(), msg.Ptr1() );
        }

    msg.Complete( err, ETrue );
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwChannel::DoCreate
// -----------------------------------------------------------------------------
//
TInt DVirtualVideoHwChannel::DoCreate( TInt /*aUnit*/, const TDesC* /*aInfo*/, const TVersion& aVer )
    {
    using namespace GuestVideoDriver;
    TInt err = KErrNone;
    
    if ( ! Kern::QueryVersionSupported(
            TVersion(
                    KMajorVer,
                    KMinorVer,
                    KBuildVer ),
            aVer ) )
        {
        return KErrNotSupported; 
        }

    iMsgQ.Receive();
    return err;
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwChannel::DoRequest
// -----------------------------------------------------------------------------
//
TInt DVirtualVideoHwChannel::DoRequest(
        DThread*& aUserThread,
        TInt aReqNo,
        TRequestStatus* aStatus,
        TAny* a1,
        TAny* a2 )
    {
    using namespace GuestVideoDriver;

    // Default error condition
    TInt err = KErrNone; 

    switch( aReqNo )
        {
        case ERequestExecuteCommand:
            {
            err = iReqHandler.HandleExecuteAsync(
                    aUserThread,
                    aStatus,
                    a1);
            break;
            }
        case ERequestLoadCommands:
            {
            err = iReqHandler.HandleLoadCommands(
                    aUserThread, 
                    aStatus,
                    a1 );
            break;
            }
        default:
            {
            // Other requests are not supported
            err = KErrNotSupported;
            }
            break;
        }
    return err;
    }



#ifdef FAISALMEMON_S4_SGIMAGE
TInt DVirtualVideoHwChannel::GetSgImageMetaData(TSgImageMetaData& aSgInfo, const TUint64 aId)
	{
	VVHW_TRACE("DVirtualVideoHwChannel::OpenSgImageMetaData 0x%lx", aId );
	DSgResource* sgResource = NULL;
	TInt metaDataErr = SgExtension::FindAndOpenResource( aId, sgResource );//Open the sgimage resource
	
	if ( metaDataErr == KErrNone )
		{
		TPckgBuf<TSgImageMetaData> metaDataPckg;
		
		metaDataErr = sgResource->GetMetaData( metaDataPckg );
		if ( metaDataErr == KErrNone )
			{
			aSgInfo = metaDataPckg();
			}

		sgResource->Close();
		}
	return metaDataErr;
	}
#endif


// -----------------------------------------------------------------------------
// DVirtualVideoHwChannel::DoControl
// -----------------------------------------------------------------------------
//
TInt DVirtualVideoHwChannel::DoControl(
        DThread*& aUserThread,
        TInt aFunction,
        TAny* a1,
        TAny* a2 )
    {
    using namespace GuestVideoDriver;
    
    // Default error condition
    TInt err = KErrNone; 

    switch( aFunction )
        {
        case ERequestMapAddress:
            {
            TPckgBuf<TInt> handle;
            err = Kern::ThreadDesRead( aUserThread, a1, handle, 0, 0 );
            if ( KErrNone == err )
                {
                DChunk* chunk = Kern::OpenSharedChunk( aUserThread, handle(), EFalse );
                if ( chunk )
                    {
                    TLinAddr kernelAddress;
                    TUint32 mapAttr;
                    TUint32 physicalAddress( 0 );
                    TInt ret = Kern::ChunkPhysicalAddress( chunk, 0, chunk->Size(), 
                            kernelAddress, mapAttr, physicalAddress );
                    
                    TPckgC<TUint32> address( physicalAddress );
                    err = Kern::ThreadDesWrite( aUserThread, a2, address, 0, 0, aUserThread );
                    Kern::ChunkClose( chunk );
                    chunk = NULL;
                    }
                else
                    {
                    err = KErrGeneral;
                    }
                }
            break;
            }
        case ERequestSurfaceBufferBaseAddress:
            {
            TPhysAddr physicalAddress(0);
			physicalAddress = iHwInterface.GetSurfaceBufferBase();
            TPckgC<TPhysAddr> address(physicalAddress);
            err = Kern::ThreadDesWrite(aUserThread, a1, address, 0, 0, aUserThread);
            break;
            }
#ifdef FAISALMEMON_S4_SGIMAGE
        case ERequestSgHandles:
            {
            VVHW_TRACE("DVirtualVideoHwChannel::DoControl - ERequestSgHandles" );
            TPckgBuf<TUint64> sgId;
            err = Kern::ThreadDesRead( aUserThread, a1, sgId, 0, 0 );
            if( KErrNone == err )
                {
				TSgImageMetaData sgInfo;
				err = GetSgImageMetaData( sgInfo, sgId());
				if ( KErrNone == err )
					{
					// package the handles into 64-bit value, since there's only one parameter (a2) available
					TUint64 sgHandles = (((TUint64)sgInfo.iPbufferHandle)&0xFFFFFFFF) + (((TUint64)sgInfo.iVGImageHandle)<<32);
					TPckg<TUint64> handles(sgHandles);
					err = Kern::ThreadDesWrite( aUserThread, a2, handles, 0, 0);
					if(err)
					    VVHW_TRACE("DVirtualVideoHwChannel::DoControl - ThreadDesWrite error=%d", err);
					}
				else
					{
					VVHW_TRACE("DVirtualVideoHwChannel::DoControl - OpenSgImageMetaData failed" );
					}
                }
            else
            	{
				VVHW_TRACE("DVirtualVideoHwChannel::DoControl - ThreadDesRead err=%i", err );
                }
            break;
            }
#endif
        default:
            {
            // Other functions are not supported
            err = KErrNotSupported;
            }
            break;
        }

    return err;
    }

