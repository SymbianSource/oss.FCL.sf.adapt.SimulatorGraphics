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
// 

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN                       // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include <EGL/egl.h>
#include <vg/openvg.h>
#include "GLES/gl.h"

#include "KhronosAPIWrapper.h"
#include "serializedfunctioncall.h"
#include "remotefunctioncall.h"
#include "openvgrfc.h"
#include "eglrfc.h"
#include "driverrfc.h"
#include "apiwrapper.h"
#include "driverapiwrapper.h"
#include "eglapiwrapper.h"
#include "openvgapiwrapper.h"
#include "opengles11apiwrapper.h"

#include "graphicsvhwcallback.h"        //callback interface Wrapper=>Virtual HW

#include "platformthreading.h"
#include "serialisedapiuids.h"
#include "khronosapiwrapperdefs.h"




Psu::PLATFORM_THREADFUNC_RETTYPE WINAPI WorkerThreadFunction(void* aParam)
{
    TRACE("KhronosAPIWrapper WorkerThreadFunction ->\n");
    KhronosAPIWrapper* impl=(KhronosAPIWrapper*) aParam;
    impl->WorkerThread();
    TRACE("KhronosAPIWrapper WorkerThreadFunction <-\n");
    return 0;
}

int KhronosAPIWrapper::HandleNextRequest()
{
    TRACE("KhronosAPIWrapper::HandleNextRequest() ->\n");
    int ret(0);
    uint32_t inputBufferTail = m_InputRequestBuffer->GetReadIndex();
	RemoteFunctionCallData& rfc( *iRFCData );
    SerializedFunctionCall remotefcall(rfc);
    TRACE("KhronosAPIWrapper::HandleNextRequest ParseBuffer from %u\n",  inputBufferTail);
    TInt len = remotefcall.ParseBuffer( ((TUint8*)(m_currentInput))+inputBufferTail,
        VVI_PARAMETERS_INPUT_MEMORY_SIZE - inputBufferTail );
    ASSERT( len != -1 );
    m_InputRequestBuffer->FreeBytes( len );

    //TRACE("KhronosAPIWrapper::WorkerThread DispatchRequest -> %u\n",  m_currentFunctionCall.Data().Header().iTransactionId);
	TRACE("KhronosAPIWrapper::HandleNextRequest DispatchRequest opCode -> %u / retval initially:%d\n",  rfc.Header().iOpCode, rfc.Header().iReturnValue );

    const TUint32 processId(rfc.Header().iProcessId);
    const TUint32 threadId(rfc.Header().iThreadId);

	if ( processId != m_lastProcessId || threadId != m_lastThreadId )
		{
		m_lastProcessId = processId;
		m_lastThreadId = threadId;
		TRACE("KhronosAPIWrapper::HandleNextRequest Set process info for %u / %u \n", processId, threadId );
		m_EGLWrapper->SetProcessInformation( processId, threadId );
		}

	switch ( rfc.Header().iApiUid )
		{
		case SERIALISED_DRIVER_API_UID:
			{
			TRACE("KhronosAPIWrapper::HandleNextRequest SERIALISED_DRIVER_API_UID \n" );
			if ( DriverRFC::EDrvClientShutdown == rfc.Header().iOpCode )
				{
				m_OpenVGWrapper->Cleanup( processId, threadId );
				m_EGLWrapper->Cleanup( processId, threadId );
				m_OGLESWrapper->Cleanup( processId, threadId );
				}
			else
				{
				m_DriverAPIWrapper->DispatchRequest( rfc.Header().iOpCode );
				}
			break;
			}
		case SERIALISED_OPENVG_API_UID:
			{
			ret = m_OpenVGWrapper->DispatchRequest( rfc.Header().iOpCode );
			break;
			}
		case SERIALISED_EGL_API_UID:
			{
			ret = m_EGLWrapper->DispatchRequest( rfc.Header().iOpCode );
			break;
			}
		case SERIALISED_OPENGLES_1_1_API_UID:
			{
			m_OGLESWrapper->SetProcessInformation( processId, threadId );
			ret = m_OGLESWrapper->DispatchRequest( rfc.Header().iOpCode );
			break;
			}
		default:
			break;
		}

    TRACE("KhronosAPIWrapper::HandleNextRequest DispatchRequest <-\n");

	if ( RemoteFunctionCallData::EOpReply ==  rfc.Header().iOpType )
    {
        TRACE("KhronosAPIWrapper::HandleNextRequest ProsessingDone ->\n");
        iServiceIf->ProcessingDone( rfc.Header().iTransactionId );
        TRACE("KhronosAPIWrapper::HandleNextRequest ProsessingDone <-\n");
    }
    else if ( !m_InputRequestBuffer->IsDataAvailable() )
    {
        iServiceIf->LockOutputBuffer(); //We need to use the lock buffer so that the request id register is protected
        iServiceIf->ProcessingDone( 0 ); //Signal driver that buffer is empty
    }
    TRACE("ret = %d\n", ret);
    TRACE("KhronosAPIWrapper::HandleNextRequest() <-\n");
    return ret;
}


void KhronosAPIWrapper::WorkerThread()
{
    TRACE("KhronosAPIWrapper::WorkerThread ->\n");
    while(1)
    {
        int ret;

        while ( !m_exit && !m_InputRequestBuffer->IsDataAvailable() )
        {
            TRACE("KhronosAPIWrapper::WorkerThread Waiting\n");
			//No more data, wait for some more
            Psu::platform_wait_for_signal(m_SemapHandle);
            TRACE("KhronosAPIWrapper::WorkerThread Signaled\n");
        }

        TRACE("KhronosAPIWrapper::WorkerThread Process request\n");

        if ( m_exit )
        {
            TRACE("KhronosAPIWrapper::WorkerThread m_exit\n");
            return;
        }
        ret = HandleNextRequest();
        TRACE("KhronosAPIWrapper::WorkerThread <-\n");
    }
}


TUint32 KhronosAPIWrapper::GetWriteCount()
{
    TRACE("KhronosAPIWrapper::GetWriteCount()\n");
    return InputBufferWriteCount();
}


void KhronosAPIWrapper::IncrementReadCount( TUint32 aReadCount )
{
    TRACE("KhronosAPIWrapper::IncrementReadCount()\n");
    IncrementInputBufferReadCount( aReadCount );
}


TUint32 KhronosAPIWrapper::GetReadCount()
{
    TRACE("KhronosAPIWrapper::GetReadCount()\n");
    return InputBufferReadCount();
}


TUint32 KhronosAPIWrapper::BufferTail()
{
    TRACE("KhronosAPIWrapper::BufferTail()\n");
    return InputBufferTail();
}


void KhronosAPIWrapper::SetBufferTail( TUint32 aIndex )
{
    TRACE("KhronosAPIWrapper::SetBufferTail()\n");
    SetInputBufferTail( aIndex );
}


TUint32 KhronosAPIWrapper::BufferHead()
{
    TRACE("KhronosAPIWrapper::BufferHead()\n");
    return InputBufferHead();
}


TUint32 KhronosAPIWrapper::MaxTailIndex()
{
    TRACE("KhronosAPIWrapper::MaxTailIndex()\n");
    return InputMaxTailIndex();
}


void KhronosAPIWrapper::SetMaxTailIndex( TUint32 aIndex )
{
    TRACE("KhronosAPIWrapper::SetMaxTailIndex()\n");
    SetInputMaxTailIndex( aIndex );
}


TUint32 KhronosAPIWrapper::InputMaxTailIndex()
{
    TRACE("KhronosAPIWrapper::InputMaxTailIndex()\n");
    TUint32 ret = m_InputBufferMaxTailIndex;
    return ret;
}


void KhronosAPIWrapper::SetInputMaxTailIndex( TUint32 aIndex )
{
    TRACE("KhronosAPIWrapper::SetInputMaxTailIndex aIndex %u\n", aIndex );
    m_InputBufferMaxTailIndex = aIndex;
}


void KhronosAPIWrapper::SetInputBufferTail(uint32_t p_val)
{
    TRACE("KhronosAPIWrapper::SetInputBufferTail aIndex %u\n", p_val );
    m_InputBufferTail = p_val;
}


uint32_t KhronosAPIWrapper::InputBufferTail()
{
    uint32_t val = m_InputBufferTail;
    TRACE("KhronosAPIWrapper::InputBufferTail %u\n", val );
    return val;
}


void KhronosAPIWrapper::SetInputBufferHead(uint32_t p_val)
{
    TRACE("KhronosAPIWrapper::SetInputBufferHead aIndex %u\n", p_val );
    m_InputBufferHead = p_val;
}


uint32_t KhronosAPIWrapper::InputBufferHead()
{
    uint32_t val = m_InputBufferHead;
    TRACE("KhronosAPIWrapper::InputBufferHead %u\n", val );
    return val;
}


void KhronosAPIWrapper::SetInputBufferReadCount(uint32_t p_val)
{
    TRACE("KhronosAPIWrapper::SetInputBufferReadCount aIndex %u\n", p_val );
    m_InputBufferReadCount = p_val;
}


void KhronosAPIWrapper::IncrementInputBufferReadCount( TUint32 aReadCount )
{
    TRACE("KhronosAPIWrapper::IncrementInputBufferReadCount m_InputBufferReadCount++ %u\n", m_InputBufferReadCount );
    m_InputBufferReadCount += aReadCount;
}


uint32_t KhronosAPIWrapper::InputBufferReadCount()
{
    uint32_t val = m_InputBufferReadCount;
    TRACE("KhronosAPIWrapper::InputBufferReadCount %u\n", val );
    return val;
}


void KhronosAPIWrapper::SetInputBufferWriteCount(uint32_t p_val)
{
    TRACE("KhronosAPIWrapper::SetInputBufferWriteCount	%u\n", p_val );
    m_InputbufferWriteCount = p_val;
}


uint32_t KhronosAPIWrapper::InputBufferWriteCount()
{
    uint32_t val = m_InputbufferWriteCount;
    TRACE("KhronosAPIWrapper::InputBufferWriteCount %u\n", val );
    return val;
}


KhronosAPIWrapper::KhronosAPIWrapper( MGraphicsVHWCallback* aServiceIf, void* framebuffer, void* inputdata, void* outputdata )
{
    TRACE("KhronosAPIWrapper::KhronosAPIWrapper()\n");
	m_lastVgError = VG_NO_ERROR;
	m_lastEglError = EGL_SUCCESS;

	m_lastProcessId = 0;
	m_lastThreadId = 0;
    m_currentInput = inputdata;
    m_currentResult = outputdata;
    m_frameBuffer = framebuffer;
    m_exit = false;
    iServiceIf = aServiceIf;
    m_initDone = false;
	iRFCData = new RemoteFunctionCallData;
    m_InputRequestBuffer = new RequestBufferReader( *this, VVI_PARAMETERS_INPUT_MEMORY_SIZE );

	Psu::platform_create_semaphore(m_SemapHandle, 0, 1000000);

    Psu::platform_mutex_init( &m_InputBufferMutex );
    m_InputBufferTail = 0;
    m_InputBufferHead = 0;
    m_InputBufferReadCount = 0;
    m_InputbufferWriteCount = 0;

    iStack = new APIWrapperStack();
    iStack->InitStack( KMaxStackSize );

	m_OpenVGWrapper = new OpenVGAPIWrapper( *iRFCData, iStack, m_currentResult, iServiceIf, this );
	m_OGLESWrapper = new OGLES11Wrapper(*iRFCData, iStack, m_currentResult, iServiceIf);
	m_EGLWrapper = new EGLAPIWrapper( *iRFCData, iStack, m_currentResult, iServiceIf, m_frameBuffer, this );
	m_DriverAPIWrapper = new DriverAPIWrapper( *iRFCData, iStack, m_currentResult, iServiceIf, this );

#ifdef KHRONOS_API_W_MULTITHREAD
    int result = Psu::platform_create_simple_thread(&m_threadHandle,
        &WorkerThreadFunction, this);
    if (result)
    {
//Error
    }
#endif
}

KhronosAPIWrapper::~KhronosAPIWrapper()
{
    TRACE("KhronosAPIWrapper::~KhronosAPIWrapper\n");
    m_exit = true;
    Psu::platform_signal_semaphore(m_SemapHandle);
    Psu::platform_join_thread(m_threadHandle);
    Psu::platform_release_semaphore(m_SemapHandle);
    Psu::platform_mutex_destroy( &m_InputBufferMutex );
    delete iStack; iStack = NULL;
	delete m_InputRequestBuffer; m_InputRequestBuffer = NULL;
	delete m_OpenVGWrapper; m_OpenVGWrapper = NULL;
	delete m_EGLWrapper; m_EGLWrapper = NULL;
	delete m_DriverAPIWrapper; m_DriverAPIWrapper = NULL;
	delete m_OGLESWrapper; m_OGLESWrapper = NULL;
	delete iRFCData; iRFCData = NULL;
}


unsigned long KhronosAPIWrapper::InputParameterOffset()
{
    TRACE("KhronosAPIWrapper::InputParameterOffset()\n");
    return 0;
}


unsigned long KhronosAPIWrapper::Execute()
{
    TRACE("KhronosAPIWrapper::Execute\n");
    TInt ret(0);

#ifdef KHRONOS_API_W_MULTITHREAD
    Psu::platform_signal_semaphore(m_SemapHandle);
#else
    while ( m_InputRequestBuffer->IsDataAvailable() )
    {
        HandleNextRequest();
    }
#endif

    return ret;
}


#ifdef WIN32

BOOL APIENTRY DllMain( HMODULE hModule,
DWORD  ul_reason_for_call,
LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return true;
}
#endif

