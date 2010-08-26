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
// Guest Video Driver Implementation

#include <e32cmn.h>
#include <e32des8.h> 
#include <e32std.h> 
#include <e32debug.h> 

#include <graphics/guestvideodriver.h>
#include <graphics/guestvideodriverprotocol.h>
#include <graphics/guestvideodriverinterfaceconstants.h>

#include "remotefunctioncall.h"
#include "serializedfunctioncall.h"

// tracing
#ifdef _DEBUG
#include <e32debug.h>
    #define UTIL_TRACE(fmt, args...) RDebug::Printf(fmt, ##args)
	#define DRVRPANIC_ASSERT(condition, panic) if (!(condition)) { DriverPanic(panic, #condition, __LINE__); }
	#define DRVRPANIC_ASSERT_DEBUG(condition, panic) if (!(condition)) { DriverPanic(panic, #condition, __LINE__); }
#else
    #define UTIL_TRACE(fmt, args...)
	#define DRVRPANIC_ASSERT(condition, panic) if (!(condition)) { DriverPanic(panic, NULL, __LINE__); }
	#define DRVRPANIC_ASSERT_DEBUG(condition, panic)
#endif

//Max supported request size
const TUint32 KMaxRequestSize( VVI_PARAMETERS_INPUT_MEMORY_SIZE );

typedef enum
	{
	EDriverPanicSendBufferFailed=1,
	EDriverPanicInvalidOperationType,
	EDriverPanicOperationDataTooBig,
	EDriverPanicDriverAlreadyOpen,
	EDriverPanicCreateDriverChannelFailed,
	EDriverPanicCreateThreadLockFailed,
	EDriverPanicSendBufferFnDoesNotHaveThreadLock,
	EDriverPanicBufferCommandFnDoesNotHaveThreadLock,
	EDriverPanicDriverNotOpenForExecuteCommandFn,
	EDriverPanicRequestStatusErrorInExecuteCommandFn, // 10
	EDriverPanicBufferWriteFailed,
	} TDriverPanic;

_LIT(KDriverPanicCategory, "Guest VidDrv");

void DriverPanic(TDriverPanic aPanicCode, char* aCondition, TInt aLine)
	{
	UTIL_TRACE("Guest Video Driver Panic %d for failed Assert (%s), at guestvideodriver.cpp:%d", aPanicCode, aCondition, aLine);
		
	User::Panic(KDriverPanicCategory, aPanicCode);
	}


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RGuestVideoDriver::RGuestVideoDriver
// -----------------------------------------------------------------------------
//
EXPORT_C RGuestVideoDriver::RGuestVideoDriver() : iIsOpen(EFalse), iProcessId(0)
    {
    }

// -----------------------------------------------------------------------------
// RGuestVideoDriver::Open
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RGuestVideoDriver::Open()
    {
	UTIL_TRACE("RGuestVideoDriver::Open start iProcessId=0x%x, iIsOpen=%d", iProcessId, iIsOpen);
	DRVRPANIC_ASSERT(iIsOpen == EFalse, EDriverPanicDriverAlreadyOpen);
	iProcessId = RProcess().Id();
	TInt error = iThreadLock.CreateLocal(EOwnerProcess);
	DRVRPANIC_ASSERT_DEBUG( error == KErrNone, EDriverPanicCreateThreadLockFailed);
	if (!error)
		{
		error = DoCreate(
			GuestVideoDriver::KDeviceName,
			TVersion( GuestVideoDriver::KMajorVer,
					  GuestVideoDriver::KMinorVer,
					  GuestVideoDriver::KBuildVer ),
			KNullUnit,
			NULL,
			NULL);
		DRVRPANIC_ASSERT_DEBUG( error == KErrNone, EDriverPanicCreateDriverChannelFailed);
		if (!error)
			{
			iIsOpen = ETrue;
			}
		}
	UTIL_TRACE("RGuestVideoDriver::Open end iProcessId=0x%x, error=%d", iProcessId, error);
	return error;
	}

// -----------------------------------------------------------------------------
// RGuestVideoDriver::~RGuestVideoDriver
// -----------------------------------------------------------------------------
//
EXPORT_C RGuestVideoDriver::~RGuestVideoDriver()
    {
	UTIL_TRACE("RGuestVideoDriver::~RGuestVideoDriver iProcessId=0x%x", iProcessId);
	iThreadLock.Close();
    }

// -----------------------------------------------------------------------------
// RGuestVideoDriver::ExecuteCommand
// Syncrhonous version with local buffering
// -----------------------------------------------------------------------------
//
EXPORT_C void RGuestVideoDriver::ExecuteCommand(RemoteFunctionCallData& aRequestData)
	{
	DRVRPANIC_ASSERT(iIsOpen, EDriverPanicDriverNotOpenForExecuteCommandFn);
	DRVRPANIC_ASSERT( (RemoteFunctionCallData::EOpRequestWithReply == aRequestData.Header().iOpType) ||
			(RemoteFunctionCallData::EOpRequest == aRequestData.Header().iOpType),
			EDriverPanicInvalidOperationType);

	//Set thread and process id
	aRequestData.SetThreadInformation(iProcessId, RThread().Id());
	TBool sendNow = (RemoteFunctionCallData::EOpRequest != aRequestData.Header().iOpType); 
	if (!sendNow)
		{
		iThreadLock.Wait();
		while (!BufferCommand(aRequestData))
			{
			// Flush any queued commands & retry
			if (iBuffer.Length())
				{
				SendBuffer();
				}
			else
				{ // Too big for buffer
				sendNow = ETrue;
				break;
				}
			}
		iThreadLock.Signal();
		}

	if (sendNow)
		{
		// Maintain order of operations by flushing queue
		iThreadLock.Wait();
		if (iBuffer.Length())
			{
			SendBuffer();
			}
		iThreadLock.Signal();

		TRequestStatus status;
		TPckg<RemoteFunctionCallData> data(aRequestData);
		DRVRPANIC_ASSERT(data().SerialisedLength() <= KMaxRequestSize, EDriverPanicOperationDataTooBig);
		UTIL_TRACE("RGuestVideoDriver::ExecuteCommand direct send, req length=%d", data().SerialisedLength());
		DoRequest(GuestVideoDriver::ERequestExecuteCommand, status, (TAny*)&data);
		User::WaitForRequest(status);
		// status <> 0 if write of reply data failed
		DRVRPANIC_ASSERT_DEBUG(status.Int() == KErrNone, EDriverPanicRequestStatusErrorInExecuteCommandFn);
		}
	}

// Flush video Command buffer
EXPORT_C void RGuestVideoDriver::Flush()
	{
	iThreadLock.Wait();
	if (iIsOpen && iBuffer.Length())
		{
		SendBuffer();
		}
	iThreadLock.Signal();
	}


// -----------------------------------------------------------------------------
// RGuestVideoDriver::BufferCommand
// -----------------------------------------------------------------------------
//
TBool RGuestVideoDriver::BufferCommand( RemoteFunctionCallData& aRequestData )
	{
	DRVRPANIC_ASSERT_DEBUG(iThreadLock.IsHeld(), EDriverPanicBufferCommandFnDoesNotHaveThreadLock);
	TBool result = EFalse;
	SerializedFunctionCall data( aRequestData );

	const TUint32 len = aRequestData.SerialisedLength();
	const TInt alignmentAdjIndex( RemoteFunctionCallData::AlignIndex( iBuffer.Length(), 4 ) );

	if ( (alignmentAdjIndex + len) < iBuffer.MaxLength() )
		{
		//There is enough space left on local buffer
		iBuffer.SetLength( alignmentAdjIndex + len );
		TPtrC8 serialisedDataPtr = iBuffer.Right( len );
		TInt wlen = data.WriteToBuffer( const_cast<TUint8*>(serialisedDataPtr.Ptr()), len, 0 );
		DRVRPANIC_ASSERT(wlen == len, EDriverPanicBufferWriteFailed);
		result = ETrue;
		}

	UTIL_TRACE("RGuestVideoDriver::BufferCommand length=%d, Req len=%d, result=%d",
			iBuffer.Length(), len, result);
	return result;
	}

// -----------------------------------------------------------------------------
// RGuestVideoDriver::MapToHWAddress
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RGuestVideoDriver::MapToHWAddress( const TInt aChunkHandle, TUint32& aHWAddress )
    {
    TPckgBuf<TInt> handle( aChunkHandle );
    TPckg<TUint32> address( aHWAddress );
    return DoControl( GuestVideoDriver::ERequestMapAddress, (TAny*)&handle, (TAny*)&address );
    }

// -----------------------------------------------------------------------------
// RGuestVideDriver::MapToHWAddress
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RGuestVideoDriver::GetFrameBufferBaseAddress( TUint32& aFrameBufferBaseAddress )
    {
    TPckg<TUint32> address( aFrameBufferBaseAddress );
    return DoControl( GuestVideoDriver::ERequestFrameBaseAddress, (TAny*)&address );
    }

// -----------------------------------------------------------------------------
// RGuestVideoDriver::EglGetSgHandles
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RGuestVideoDriver::EglGetSgHandles( const TUint64 aId, TUint64 *aSgHandles )
    {
    TPckg<TUint64> sgId( aId );
    TPckg<TUint64> sgHandles( *aSgHandles );
    return DoControl( GuestVideoDriver::ERequestSgHandles, (TAny*)&sgId, (TAny*)&sgHandles );
    }

// -----------------------------------------------------------------------------
// RGuestVideoDriver::SendBuffer
// -----------------------------------------------------------------------------
//
void RGuestVideoDriver::SendBuffer()
    {
	UTIL_TRACE("RGuestVideoDriver::SendBuffer length=%d", iBuffer.Length());
	DRVRPANIC_ASSERT_DEBUG(iThreadLock.IsHeld(), EDriverPanicSendBufferFnDoesNotHaveThreadLock);
    TRequestStatus status;

    DoRequest(GuestVideoDriver::ERequestLoadCommands, status, (TAny*)&iBuffer);

    User::WaitForRequest( status );

    iBuffer.Zero();
	UTIL_TRACE("RGuestVideoDriver::SendBuffer status=%d", status.Int());
	// Commands expecting a reply should never come through here, so status should always be KErrNone
	DRVRPANIC_ASSERT(status.Int() == KErrNone, EDriverPanicSendBufferFailed);
    }
