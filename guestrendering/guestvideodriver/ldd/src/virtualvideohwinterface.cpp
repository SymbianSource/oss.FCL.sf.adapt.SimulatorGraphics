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


// CONSTANTS

_LIT( KVirtualVideoHwInterfacePanic, "DVirtualVideoHwInterface" );

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
    VVI_R_REQUEST_ID
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
    return InitPhysicalMemory( VVI_PARAMETERS_INPUT_BASE_ADDRESS, 
            VVI_PARAMETERS_INPUT_MEMORY_SIZE, iInputParametersMemoryChunk, 
            iInputParametersChunkKernelAddress );    
    }

TInt DVirtualVideoHwInterface::InitParametersOutputMemory()
    {
    return InitPhysicalMemory( VVI_PARAMETERS_OUTPUT_BASE_ADDRESS, 
            VVI_PARAMETERS_OUTPUT_MEMORY_SIZE, iOutputParametersMemoryChunk, 
            iOutputParametersChunkKernelAddress );    
    }

TInt DVirtualVideoHwInterface::InitRegisterMemory()
    {
    return InitPhysicalMemory( VVI_REGISTERS_BASE_ADDRESS, 
            VVI_REGISTERS_MEMORY_SIZE, iRegisterMemoryChunk, 
            iRegisterChunkKernelAddress );        
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

// -----------------------------------------------------------------------------
// DVirtualVideoHwInterface::GetRegisterValue
// -----------------------------------------------------------------------------
//
void DVirtualVideoHwInterface::GetRegisterValue(
    TRegister aRegister,
    TUint32& aValue )
    {
    if ( iRegisterMemoryChunk )
        {
        TLinAddr offset = RegisterOffset( aRegister );
        TUint32* ptr = reinterpret_cast<TUint32*>( iRegisterChunkKernelAddress + offset );
        aValue = *ptr;
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

// TODO FAISALMEMON This is the first portion of the port of Jani's
// Dynamic framebuffer base address code.  Inspect this code, and also
// put in the rest of the code.
EXPORT_C TPhysAddr  DVirtualVideoHwInterface::GetFrameBase()
	{
#ifdef FAISALMEMON_DONE_DYNAMIC_FRAMEBUFFER_PORT 
	TPhysAddr ret = 0;
	if(DVirtualVideoHwInterface::pVVHIf != NULL)
		{
		ret = DVirtualVideoHwInterface::pVVHIf->iFrameRamPhys;
		}
	return ret;
#else
	return 0;
#endif
}
