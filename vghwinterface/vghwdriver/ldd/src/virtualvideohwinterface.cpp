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

#ifdef FIXED_MEMORY_LOCATION
_LIT( KVirtualVideoHwInterfacePanic, "DVirtualVideoHwInterface" );
#else
#include "syborg.h"
#endif

// ============================ LOCAL DATA TYPES ===============================

TPhysAddr DVirtualVideoHwInterface::iVideoRamBasePhys = NULL;

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
	VVI_R_SHARED_SURFACEBUFFER_MEMORY_BASE
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
    VVHW_TRACE("DVirtualVideoHwInterface::DVirtualVideoHwInterface()>");
    iInputParametersMemoryChunk = NULL;
    iOutputParametersMemoryChunk = NULL;
    iRegisterMemoryChunk = NULL;

#ifdef FIXED_MEMORY_LOCATION
	iVideoRamBasePhys = VVI_BASE;
#else
	// Reserve a contiguous memory chunk for graphics usage
	TUint32 ramSize = VVI_PARAMETERS_INPUT_MEMORY_SIZE +
						VVI_PARAMETERS_OUTPUT_MEMORY_SIZE + 
						VVI_SURFACEBUFFER_MEMORY_SIZE;
	TInt r = Epoc::AllocPhysicalRam( ramSize, iVideoRamBasePhys );
    VVHW_TRACE("DVirtualVideoHwInterface::DVirtualVideoHwInterface() AllocPhysicalRam %d", r);
	if (r != KErrNone)
		{
	    NKern::ThreadLeaveCS();
		Kern::Fault("DVirtualVideoHwInterface Allocate Ram %d",r);
		}
 	SetSharedCmdMemBase( iVideoRamBasePhys + VVI_PARAMETERS_INPUT_BASE_ADDRESS );
	SetSharedSurfacebufferMemBase( iVideoRamBasePhys + VVI_SURFACEBUFFER_BASE_ADDRESS );

#endif // FIXED_MEMORY_LOCATION
    VVHW_TRACE("DVirtualVideoHwInterface::DVirtualVideoHwInterface()<");
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
    return InitPhysicalMemory(  iVideoRamBasePhys + VVI_PARAMETERS_INPUT_BASE_ADDRESS, 
            VVI_PARAMETERS_INPUT_MEMORY_SIZE, iInputParametersMemoryChunk, 
            iInputParametersChunkKernelAddress );    
    }

TInt DVirtualVideoHwInterface::InitParametersOutputMemory()
    {
    return InitPhysicalMemory( iVideoRamBasePhys + VVI_PARAMETERS_OUTPUT_BASE_ADDRESS, 
            VVI_PARAMETERS_OUTPUT_MEMORY_SIZE, iOutputParametersMemoryChunk, 
            iOutputParametersChunkKernelAddress );    
    }

TInt DVirtualVideoHwInterface::InitRegisterMemory()
    {
#ifdef FIXED_MEMORY_LOCATION
    return InitPhysicalMemory( VVI_REGISTERS_BASE_ADDRESS, 
            VVI_REGISTERS_MEMORY_SIZE, iRegisterMemoryChunk, 
            iRegisterChunkKernelAddress );        
#else
    return KErrNone;    
#endif // FIXED_MEMORY_LOCATION	
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

void DVirtualVideoHwInterface::SetSharedSurfacebufferMemBase( TUint32 aPhysicalAddress )
    {
    VVHW_TRACE("DVirtualVideoHwInterface::SetSharedSurfacebufferMemBase 0x%08x", aPhysicalAddress);
    SetRegisterValue( ERegSharedSurfacebufferMemBase, aPhysicalAddress );
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
#ifdef FIXED_MEMORY_LOCATION
        Kern::PanicCurrentThread( KVirtualVideoHwInterfacePanic, KErrNotReady );
#else
        TLinAddr offset = RegisterOffset( aRegister );
		aValue = ReadReg( KHwGraphicsRegBase, offset );
#endif // FIXED_MEMORY_LOCATION
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
#ifdef FIXED_MEMORY_LOCATION
        Kern::PanicCurrentThread( KVirtualVideoHwInterfacePanic, KErrNotReady );
#else
        TLinAddr offset = RegisterOffset( aRegister );
		WriteReg( KHwGraphicsRegBase, offset, aValue );
#endif // FIXED_MEMORY_LOCATION
        }
    }

EXPORT_C TPhysAddr  DVirtualVideoHwInterface::GetSurfaceBufferBase()
	{
	TPhysAddr ret = 0;
	if( iVideoRamBasePhys != 0 )
		{
		ret = iVideoRamBasePhys + VVI_SURFACEBUFFER_BASE_ADDRESS;
		}
	return ret;
    }

