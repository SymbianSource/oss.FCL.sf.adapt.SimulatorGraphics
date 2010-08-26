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
// Implementation of Virtual Video Hardware Interface


// INCLUDE FILES
#include <platform.h>
#include <kern_priv.h>

#include <graphics/guestvideodriverinterfaceconstants.h>
#include <graphics/virtualvideohwinterface.h>
#include <graphics/virtualvideotracing.h>
#include "syborg.h"


// CONSTANTS

_LIT( KVirtualVideoHwInterfacePanic, "DVirtualVideoHwInterface" );
DVirtualVideoHwInterface * DVirtualVideoHwInterface::pVVHIf = NULL;

// ============================ LOCAL DATA TYPES ===============================

// Register offsets for playback and recording channels
// Note: The orders of these must match the order of enumarations
const TLinAddr KRegisterOffsets[] =
    {
    VVI_R_ID,
    VVI_R_IRQ_ENABLE,
    VVI_R_IRQ_STATUS,
    VVI_R_COMMAND,
    VVI_R_PARAMETER_LOAD,
    VVI_R_ERROR,
    VVI_R_INPUT_BUFFER_TAIL,
    VVI_R_INPUT_BUFFER_HEAD,
    VVI_R_INPUT_BUFFER_READ_COUNT,
    VVI_R_INPUT_BUFFER_WRITE_COUNT,
    VVI_R_INPUT_BUFFER_MAX_TAIL,
    VVI_R_REQUEST_ID,
	VVI_R_SHARED_CMD_MEMORY_BASE,
	VVI_R_SHARED_FRAMEBUFFER_MEMORY_BASE,
    };
#define ASSERT_PANIC(c,p) __ASSERT_DEBUG(c,Kern::PanicCurrentThread(KVirtualVideoHwInterfacePanic,p));

// ============================ LOCAL FUNCTIONS ================================

// Get register offset for certain register range and register
inline TLinAddr RegisterOffset( DVirtualVideoHwInterface::TRegister aRegister )
    {
    return KRegisterOffsets[aRegister];
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::DVirtualVideoHwInterface
// -----------------------------------------------------------------------------
//
DVirtualVideoHwInterface::DVirtualVideoHwInterface()
    {
    iInputParametersMemoryChunk = NULL;
    iOutputParametersMemoryChunk = NULL;
    iRegisterMemoryChunk = NULL;

	DVirtualVideoHwInterface::pVVHIf = this;

    Kern::Printf("DVirtualVideoHwInterface::DVirtualVideoHwInterface()>");
	// Reserve a contiguous memory chunk for graphics usage
	TUint32 ramSize = VVI_PARAMETERS_INPUT_MEMORY_SIZE +
						VVI_PARAMETERS_OUTPUT_MEMORY_SIZE + 
						VVI_FRAMEBUFFER_MEMORY_SIZE + 
						VVI_REGISTERS_MEMORY_SIZE;
	TInt r = Epoc::AllocPhysicalRam( ramSize, iVideoRamPhys );
    VVHW_TRACE("DVirtualVideoHwInterface::DVirtualVideoHwInterface() AllocPhysicalRam %d", r);
	if (r != KErrNone)
		{
	    NKern::ThreadLeaveCS();
		Kern::Fault("DVirtualVideoHwInterface Allocate Ram %d",r);
		}
	iFrameRamPhys = iVideoRamPhys + VVI_FRAMEBUFFER_BASE_ADDRESS;
	Kern::Printf("DVirtualVideoHwInterface::DVirtualVideoHwInterface()<");
    }


// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::~DVirtualVideoHwInterface
// -----------------------------------------------------------------------------
//
DVirtualVideoHwInterface::~DVirtualVideoHwInterface()
    {
    // Enter critical section
    NKern::ThreadEnterCS();

    if ( iInputParametersMemoryChunk )
        {
        Kern::ChunkClose( iInputParametersMemoryChunk );
        }

    if ( iOutputParametersMemoryChunk )
        {
        Kern::ChunkClose( iOutputParametersMemoryChunk );
        }

    if ( iRegisterMemoryChunk )
        {
        Kern::ChunkClose( iRegisterMemoryChunk );
        }

    iInputParametersMemoryChunk = NULL;
    iOutputParametersMemoryChunk = NULL;
    iRegisterMemoryChunk = NULL;

    // Leave critical section
    NKern::ThreadLeaveCS();
    }

TInt DVirtualVideoHwInterface::InitParametersInputMemory()
    {
    TInt ret = InitPhysicalMemory( iVideoRamPhys + VVI_PARAMETERS_INPUT_BASE_ADDRESS, 
            VVI_PARAMETERS_INPUT_MEMORY_SIZE, iInputParametersMemoryChunk, 
            iInputParametersChunkKernelAddress );    
    Kern::Printf("DVirtualVideoHwInterface::InitParametersInputMemory - Base phy: 0x%08x lin: 0x%08x ret: %d", 
    		iVideoRamPhys + VVI_PARAMETERS_INPUT_BASE_ADDRESS, 
    		iInputParametersChunkKernelAddress, ret );
    return ret;
	}

TInt DVirtualVideoHwInterface::InitParametersOutputMemory()
    {
    TInt ret = InitPhysicalMemory( iVideoRamPhys + VVI_PARAMETERS_OUTPUT_BASE_ADDRESS, 
            VVI_PARAMETERS_OUTPUT_MEMORY_SIZE, iOutputParametersMemoryChunk, 
            iOutputParametersChunkKernelAddress );
    Kern::Printf("DVirtualVideoHwInterface::InitParametersOutputMemory - Base phy: 0x%08x lin: 0x%08x ret: %d", 
    	iVideoRamPhys + VVI_PARAMETERS_OUTPUT_BASE_ADDRESS, 
    	iOutputParametersChunkKernelAddress, ret );
    return ret;
    }

TInt DVirtualVideoHwInterface::InitRegisterMemory()
    {
    Kern::Printf(" DVirtualVideoHwInterface::InitRegisterMemory>");
    Kern::Printf("DVirtualVideoHwInterface::InitRegisterMemory - parameter base phy: 0x%08x", iVideoRamPhys );
 	SetSharedCmdMemBase( iVideoRamPhys );
   	Kern::Printf("DVirtualVideoHwInterface::InitRegisterMemory - frame buffer phy: 0x%08x", iFrameRamPhys );
	SetSharedFramebufferMemBase( iFrameRamPhys );

    Kern::Printf(" DVirtualVideoHwInterface::InitRegisterMemory<");
    return 0;   
    }

// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::InitPhysicalMemory
// -----------------------------------------------------------------------------
//
TInt DVirtualVideoHwInterface::InitPhysicalMemory( TUint32 aPhysicalAddress, 
        TInt aMaxSize, DChunk*& aChunk, TLinAddr& aLinAddr )
    {
    TInt error = KErrNoMemory;
    TChunkCreateInfo info;
    info.iType = TChunkCreateInfo::ESharedKernelSingle;
    info.iMaxSize = aMaxSize;
	info.iMapAttr = EMapAttrFullyBlocking;
    info.iOwnsMemory = EFalse;
    info.iDestroyedDfc = NULL;
    DChunk* chunk = NULL;
    TUint32 mapAttr = 0;
    TLinAddr chunkKernelAddress = 0;
    
    // Enter critical section while creating the chunk and commiting memory
    NKern::ThreadEnterCS();

    error = Kern::ChunkCreate( info, chunk, chunkKernelAddress, mapAttr );
    if ( !error )
        {
        error = Kern::ChunkCommitPhysical(
            chunk, 0, aMaxSize, aPhysicalAddress );
        if ( error )
            {
            Kern::ChunkClose( chunk );
            }
        else
            {
            // Physical memory has been successfully committed to chunk
            aChunk = chunk; 
            aLinAddr = chunkKernelAddress;
            }
        }
    chunk = NULL;

    // Leave critical section
    NKern::ThreadLeaveCS();
    
    VVHW_TRACE("DVirtualVideoHwInterface::InitPhysicalMemory return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::EnableInterrupts
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwInterface::EnableInterrupts( TUint32 aInterruptBitField )
    {
    SetRegisterValue( ERegIrqEnable, aInterruptBitField );
    }


// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::GetInterruptEnabledState
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwInterface::GetInterruptEnabledState( TUint32& aInterruptBitField )
    {
    GetRegisterValue( ERegIrqEnable, aInterruptBitField );
    }


// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::GetInterruptStatus
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwInterface::GetInterruptStatus( TUint32& aInterruptBitField )
    {
    GetRegisterValue( ERegIrqStatus, aInterruptBitField );
    }


// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::ResetInterruptStatus
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwInterface::ResetInterruptStatus( TUint32 aInterruptBitField )
    {
    SetRegisterValue( ERegIrqStatus, aInterruptBitField );
    }


// DVirtualVideoHwInterface::IssueCommand
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwInterface::IssueCommand( TUint32 aCommand )
    {
    SetRegisterValue( ERegCommand, aCommand );
    }

// DVirtualVideoHwInterface::SetSharedCmdMemBase
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwInterface::SetSharedCmdMemBase( TUint32 aPhysicalAddress )
    {
    VVHW_TRACE("DVirtualVideoHwInterface::SetSharedCmdMemBase 0x%08x", aPhysicalAddress);
    SetRegisterValue( ERegSharedCmdMemBase, aPhysicalAddress );
    }

void DVirtualVideoHwInterface::SetSharedFramebufferMemBase( TUint32 aPhysicalAddress )
    {
    VVHW_TRACE("DVirtualVideoHwInterface::SetSharedFramebufferMemBase 0x%08x", aPhysicalAddress);
    SetRegisterValue( ERegSharedFramebufferMemBase, aPhysicalAddress );
    }


// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::GetRegisterValue
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwInterface::GetRegisterValue(
    TRegister aRegister,
    TUint32& aValue )
    {
    if ( 1 /* iRegisterMemoryChunk */ ) // FAISALMEMON check this line of code with Jani.  Can we get rid of it?
        {
        TLinAddr offset = RegisterOffset( aRegister );
		// FAISALMEMON check this with Jani.  Will the code still work for platsim or is this now syborg specific?
        //TUint32* ptr = reinterpret_cast<TUint32*>( iRegisterChunkKernelAddress + offset );
        //aValue = *ptr;
		aValue = ReadReg( KHwGraphicsRegBase, offset );
        }
    else
        {
        Kern::PanicCurrentThread( KVirtualVideoHwInterfacePanic, KErrNotReady );
        }
    }


// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::SetRegisterValue
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwInterface::SetRegisterValue(
    TRegister aRegister,
    TUint32 aValue )
    {
    if ( iRegisterMemoryChunk )
        {
        TLinAddr offset = RegisterOffset( aRegister );
        TUint32* ptr = reinterpret_cast<TUint32*>( iRegisterChunkKernelAddress + offset );
        *ptr = aValue;
        }
    else
        {
        Kern::PanicCurrentThread( KVirtualVideoHwInterfacePanic, KErrNotReady );
        }
    }

EXPORT_C TPhysAddr  DVirtualVideoHwInterface::GetFrameBase()
	{
	TPhysAddr ret = 0;
	if(DVirtualVideoHwInterface::pVVHIf != NULL)
		{
		ret = DVirtualVideoHwInterface::pVVHIf->iFrameRamPhys;
		}
	return ret;
}
