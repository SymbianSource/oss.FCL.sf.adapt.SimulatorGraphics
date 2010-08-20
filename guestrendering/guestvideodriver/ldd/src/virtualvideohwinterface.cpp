/*
 * =============================================================================
 *  Name        : virtualvideohwinterface.cpp
 *  Part of     : 
 *  Description : Implementation file for the class DVirtualVideoHwInterface.
 *  Version     : %version:3 %
 *
 *  Copyright © 2002-2009 Nokia. All rights reserved.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia. All rights are reserved. Copying, including
 *  reproducing, storing, adapting or translating, any
 *  or all of this material requires the prior written consent of
 *  Nokia. This material also contains confidential
 *  information which may not be disclosed to others without the
 *  prior written consent of Nokia.
 * =============================================================================
 */


// INCLUDE FILES
#include <platform.h>
#include <kern_priv.h>

#include "virtualvideointerfaceconstants.h"
#include "virtualvideohwinterface.h"
#include "virtualvideotracing.h"


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


// End of file
