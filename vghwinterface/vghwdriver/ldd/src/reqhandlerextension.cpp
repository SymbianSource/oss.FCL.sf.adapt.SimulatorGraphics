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
// Guest video driver - request handler extension (a.k.a. Command Scheduler extension)
//

#include <kernel/kern_priv.h>
#include <kernel.h>
#include <graphics/reqhandlerextension.h>

DReqHandlerExtensionImpl* TheReqHandlerExtensionImpl = NULL;

DECLARE_STANDARD_EXTENSION()
	{
	__KTRACE_OPT(KBOOT, Kern::Printf("Starting TheReqHandlerExtension"));
	TInt err;
	TheReqHandlerExtensionImpl = new DReqHandlerExtensionImpl;
	if (TheReqHandlerExtensionImpl)
		{
		err = TheReqHandlerExtensionImpl->Construct();
		if (err != KErrNone)
			{
			delete TheReqHandlerExtensionImpl;
			TheReqHandlerExtensionImpl = NULL;
			}
		}
	else
		{
		err = KErrNoMemory;
		}
	if (err != KErrNone)
		{
		__KTRACE_OPT(KBOOT, Kern::Printf("Error: TheReqHandlerExtension failed to start"));
		}
	return err;
	}


EXPORT_C TInt ReqHandlerExtension::CreateSgImagePbuffer( const TSgImageMetaData& aInfo )
	{
    VVHW_TRACE( "ReqHandlerExtension::CreateSgImagePbuffer ->" );
	if ( !TheReqHandlerExtensionImpl )
		{
		__KTRACE_OPT(KPANIC, Kern::Printf("Error: The ReqHandlerExtension not ready"));
		return KErrNotReady;
		}
	TRequestStatus status = KRequestPending;
	TheReqHandlerExtensionImpl->CreateSgImagePbuffer( aInfo, &status, &Kern::CurrentThread() );
	//Kern::WaitForRequest( status );
	VVHW_TRACE( "ReqHandlerExtension::CreateSgImagePbuffer <-" );
	return 0;
	}

EXPORT_C TInt ReqHandlerExtension::CreateSgImageVGImage( const TSgImageMetaData& aInfo )
    {
    VVHW_TRACE( "ReqHandlerExtension::CreateSgImageVGImage" );
    if ( !TheReqHandlerExtensionImpl )
        {
        __KTRACE_OPT(KPANIC, Kern::Printf("Error: The ReqHandlerExtension not ready"));
        return KErrNotReady;
        }
    TRequestStatus status;
    TheReqHandlerExtensionImpl->CreateSgImageVGImage( aInfo, &status, &Kern::CurrentThread() );
    //Kern::WaitForRequest( status );
    return 0;
    }


EXPORT_C TInt ReqHandlerExtension::DestroySgImage( const TUint64 aId )
	{
    VVHW_TRACE( "ReqHandlerExtension::DestroySgImage" );
	if ( !TheReqHandlerExtensionImpl )
		{
		__KTRACE_OPT(KPANIC, Kern::Printf("Error: The ReqHandlerExtension not ready"));
		return KErrNotReady;
		}
	return TheReqHandlerExtensionImpl->DestroySgImage( aId );
	}

EXPORT_C TInt ReqHandlerExtension::SetReqHandler( MReqHandlerCallback* aHandler )
	{
	if ( !TheReqHandlerExtensionImpl )
		{
		__KTRACE_OPT(KPANIC, Kern::Printf("Error: The ReqHandlerExtension not ready"));
		return KErrNotReady;
		}
	return TheReqHandlerExtensionImpl->SetReqHandler( aHandler );
	}

//Implementation

TInt DReqHandlerExtensionImpl::Construct()
	{
	return Kern::MutexCreate(iMutex, KNullDesC8, KMutexOrdGeneral0);
	}

void DReqHandlerExtensionImpl::CreateSgImagePbuffer( const TSgImageMetaData& aInfo, TRequestStatus* aStatus, DThread* aThread )
	{
	if( iCallBackHandler )
        {
		return iCallBackHandler->CreateSgImagePbuffer( aInfo, aStatus, aThread );
		}
	}

void DReqHandlerExtensionImpl::CreateSgImageVGImage( const TSgImageMetaData& aInfo, TRequestStatus* aStatus, DThread* aThread )
    {
    if( iCallBackHandler )
        {
        return iCallBackHandler->CreateSgImageVGImage( aInfo, aStatus, aThread );
        }
    }

TInt DReqHandlerExtensionImpl::DestroySgImage( const TUint64 aId )
	{
	if( iCallBackHandler )
        {
        TRequestStatus status;
	    TInt err = iCallBackHandler->DestroySgImage( aId );
	    return err; 
		}
    return 0;
	}

TInt DReqHandlerExtensionImpl::SetReqHandler( MReqHandlerCallback* aHandler )
	{
	iCallBackHandler = aHandler;
	return 1;
	}

