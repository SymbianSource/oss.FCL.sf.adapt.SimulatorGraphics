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
// Virtual Video Hardware Device Implementation

#include <kernel/kernel.h>
#include <assp.h>

#include <graphics/guestvideodriverprotocol.h>
#include <graphics/virtualvideohwdevice.h>
#include <graphics/virtualvideohwchannel.h>
#include <graphics/virtualvideohwinterface.h>
#include <graphics/virtualvideotracing.h>
#include <graphics/guestvideodrivercommon.h>
#include <graphics/devicereqhandler.h>

const TInt KPriority = 27; // DfcQue0 has same priority

// a .LDD requires that the first exported function creates the device. By
// returning a concrete DLogicalDevice-derived class, this implements
// polymorphism using a named DLL.
DECLARE_STANDARD_LDD()
    {
    return new DVirtualVideoHwDevice; 
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DVirtualVideoHwDevice::DVirtualVideoHwDevice
// -----------------------------------------------------------------------------
//
DVirtualVideoHwDevice::DVirtualVideoHwDevice()
    {
    // No need to set iParseMask as we don't have PDD.
    // Just set the version.
    using namespace GuestVideoDriver;
    iVersion = TVersion( KMajorVer, KMinorVer, KBuildVer );
	iHwInterface = new DVirtualVideoHwInterface;
	Kern::Printf("DVirtualVideoHwDevice::DVirtualVideoHwDevice()<");
    VVHW_TRACE("DVirtualVideoHwDevice::DVirtualVideoHwDevice");
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwDevice::Install
// -----------------------------------------------------------------------------
//
TInt DVirtualVideoHwDevice::Install()
    {
    // Our name, used by User::FreeLogicalDevice 
    TInt err = SetName( &GuestVideoDriver::KDeviceName );
    if ( err != KErrNone )
        {
        VVHW_TRACE("DVirtualVideoHwDevice::Install SetName %d", err);
        return err;
        }

    // Enter critical section
    NKern::ThreadEnterCS();
    
    err = Kern::DfcQCreate( iDfcQ, KPriority, &GuestVideoDriver::KDfcThreadName );

    // Leave critical section
    NKern::ThreadLeaveCS();

    if ( err != KErrNone )
        {
        VVHW_TRACE("DVirtualVideoHwDevice::Install Kern::DfcQCreate %d", err);
        return err;
        }
    
	// We have already created this during construction
    // iHwInterface = new DVirtualVideoHwInterface;

    if ( !iHwInterface )
        {
        return KErrNoMemory;
        }

    err = iHwInterface->InitRegisterMemory();
    if ( err != KErrNone )
        {
        VVHW_TRACE("DVirtualVideoHwDevice::Install InitRegisterMemory %d", err);
        return err;
        }

    err = iHwInterface->InitParametersInputMemory();
    if ( err != KErrNone )
        {
        VVHW_TRACE("DVirtualVideoHwDevice::Install InitParametersInputMemory %d", err);
        return err;
        }

    err = iHwInterface->InitParametersOutputMemory();
    if ( err != KErrNone )
        {
        VVHW_TRACE("DVirtualVideoHwDevice::Install InitParametersOutputMemory %d", err);
        return err;
        }

    iDeviceReqHandler = new DDeviceReqHandler( *iHwInterface, iDfcQ );
    if ( !iDeviceReqHandler )
        {
        return KErrNoMemory;
        }

    VVHW_TRACE("DVirtualVideoHwDevice::Install return %d", err);
    return err;
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwDevice::~DVirtualVideoHwDevice
// -----------------------------------------------------------------------------
//
DVirtualVideoHwDevice::~DVirtualVideoHwDevice()
    {
    delete iDeviceReqHandler;
    delete iDfcQ;
    delete iHwInterface;
    VVHW_TRACE("DVirtualVideoHwDevice:~DVirtualVideoHwDevice");
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwDevice::GetCaps
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwDevice::GetCaps( TDes8& aDes ) const
    {
    TVersion version; 

    // fill it with information
    version = iVersion; 

    // copy as much of our information as will fit into this descriptor
    aDes.Copy( (TUint8*)&version, Min( aDes.MaxLength(), sizeof( version ) ) );
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwDevice::Create
// -----------------------------------------------------------------------------
//
TInt DVirtualVideoHwDevice::Create( DLogicalChannelBase*& aChannel )
    { 
    aChannel = NULL;
    TInt error = KErrNone;
    aChannel = new DVirtualVideoHwChannel(
            *this,
            *iHwInterface,
            *iDeviceReqHandler,
            iDfcQ);
    
    if ( !aChannel )
        {
        error = KErrNoMemory;
        }
    return error;
    }
