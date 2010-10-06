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

#ifndef KHRONOSAPIWRAPPER_H
#define KHRONOSAPIWRAPPER_H

#ifndef PSU_PLATFORMTHREADING_H
#include "platformthreading.h"
#endif

#ifndef PSU_PLATFORMTYPES_H
#include "platformtypes.h"
#endif

#include "requestbuffer.h"

class APIWrapperStack;
class RemoteFunctionCall;
class RemoteFunctionCallData;
class DriverAPIWrapper;
class OpenVGAPIWrapper;
class EGLAPIWrapper;
class OGLES11Wrapper;

#define KHRONOS_API_W_MULTITHREAD

#ifdef WIN32
#ifdef KHRONOSAPIWRAPPER_EXPORTS
#define KHRONOSAPIWRAPPER_API __declspec(dllexport)
#else
#define KHRONOSAPIWRAPPER_API __declspec(dllimport)
#endif
#else
#define KHRONOSAPIWRAPPER_API
#endif

class MGraphicsVHWCallback;

class KHRONOSAPIWRAPPER_API KhronosAPIWrapper: private MRequestBufferBookKeepingReader
{
    public:

        KhronosAPIWrapper( MGraphicsVHWCallback* aServiceIf, void* surfacebuffer,
            void* inputdata, void* outputdata );
        ~KhronosAPIWrapper();

        void WorkerThread();

        unsigned long Execute();
        unsigned long InputParameterOffset();

        void SetInputBufferTail(uint32_t p_val);
        uint32_t InputBufferTail();
        void SetInputBufferHead(uint32_t p_val);
        uint32_t InputBufferHead();
        void SetInputBufferReadCount(uint32_t p_val);
        uint32_t InputBufferReadCount();
        void SetInputBufferWriteCount(uint32_t p_val);
        uint32_t InputBufferWriteCount();
        void IncrementInputBufferReadCount( TUint32 aReadCount );
        TUint32 InputMaxTailIndex();
        void SetInputMaxTailIndex( TUint32 aIndex );

		//For the purpose of wrappers that need each other's services
		DriverAPIWrapper* GetDriverWrapper() { return m_DriverAPIWrapper; }
		EGLAPIWrapper* GetEGLWrapper() { return m_EGLWrapper; }

    private:
//From MRequestBufferBookKeeping
        TUint32 GetWriteCount();
        void IncrementReadCount( TUint32 aReadCount );
        TUint32 GetReadCount();

        TUint32 BufferTail();
        void SetBufferTail( TUint32 aIndex );
        TUint32 BufferHead();

        TUint32 MaxTailIndex();
        void SetMaxTailIndex( TUint32 aIndex );

    protected:
        int HandleNextRequest();
        bool m_initDone;
        void* m_currentResult;
        void* m_currentInput;
        MGraphicsVHWCallback* iServiceIf;
        Psu::PLATFORM_THREAD_T m_threadHandle;
        Psu::PLATFORM_SEMAPHORE_T m_SemapHandle;
        bool m_exit;
        void* m_surfaceBuffer;

//Input buffer
        Psu::PLATFORM_MUTEX_T m_InputBufferMutex;
        uint32_t m_InputBufferTail;
        uint32_t m_InputBufferHead;
        uint32_t m_InputBufferReadCount;
        uint32_t m_InputbufferWriteCount;
        uint32_t m_InputBufferMaxTailIndex;
    private:
        RequestBufferReader* m_InputRequestBuffer;
        APIWrapperStack* iStack;
		TUint32 m_lastProcessId;
		TUint32 m_lastThreadId;
		int m_lastVgError;
		int m_lastEglError;
		DriverAPIWrapper* m_DriverAPIWrapper;
		OpenVGAPIWrapper* m_OpenVGWrapper;
		OGLES11Wrapper* m_OGLESWrapper;
		EGLAPIWrapper* m_EGLWrapper;

		RemoteFunctionCallData* iRFCData;
};
#endif
