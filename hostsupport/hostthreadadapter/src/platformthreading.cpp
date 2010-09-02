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
// Cross platform threading support library

#include "platformthreading.h"

#ifdef WIN32
#include <windows.h>
#include <WinBase.h>
#else
#include <unistd.h>
#include <time.h>
#include <signal.h>
#endif

#include <stdio.h>

#ifdef WIN32
#ifdef __GNUG__
static void CALLBACK default_interrupt_handler(ULONG_PTR)
#else // MSVC
static VOID CALLBACK default_interrupt_handler(__in  ULONG_PTR)
#endif
#else // LINUX
static void default_interrupt_handler(int)
#endif
{
    // does nothing
    ;
}

namespace Psu
{

#ifndef WIN32
    const int MicrosecsInMillisec = 1000;
#endif


    int platform_create_simple_thread(PLATFORM_THREAD_T * pThreadHandle,
                                      PLATFORM_THREADFUNC pFunc,
                                      void * param)
    {
        int
            rv = 0;

#ifdef WIN32
        DWORD
            threadId,
            stackSize = 0,
            creationFlags = 0;

        PLATFORM_THREAD_T
            tmp =  CreateThread(NULL,
                                stackSize,
                                pFunc,
                                param,
                                creationFlags,
                                &threadId);
        if (tmp == NULL)
            {
                rv = GetLastError();
            }
        else
            {
                *pThreadHandle = tmp;
            }
#else
        rv = pthread_create(pThreadHandle,
                            0,
                            pFunc,
                            param);
#endif

        return rv;
    }


    void platform_release_thread(PLATFORM_THREAD_T threadHandle)
    {
#ifdef WIN32
        CloseHandle(threadHandle);
#else
        pthread_detach(threadHandle);
#endif
    }


    STATUS_T platform_join_thread(PLATFORM_THREAD_T threadHandle)
    {
		STATUS_T retVal;
#ifdef WIN32

        retVal = WaitForSingleObject(threadHandle,INFINITE);
#else
        retVal = pthread_join(threadHandle, NULL);
#endif
		return retVal;
    }


    void platform_mutex_init(PLATFORM_MUTEX_T * pMutex)
    {
#ifdef WIN32
        InitializeCriticalSection(pMutex);
#else
        pthread_mutex_init(pMutex,
                           0);
#endif
    }


    void platform_mutex_destroy(PLATFORM_MUTEX_T * pMutex)
    {
#ifdef WIN32
        DeleteCriticalSection(pMutex);
#else
        pthread_mutex_destroy(pMutex);
#endif
    }


    void platform_mutex_lock(PLATFORM_MUTEX_T * pMutex)
    {
#ifdef WIN32
        EnterCriticalSection(pMutex);
#else
        pthread_mutex_lock(pMutex);
#endif
    }


    void platform_mutex_unlock(PLATFORM_MUTEX_T * pMutex)
    {
#ifdef WIN32
        LeaveCriticalSection(pMutex);
#else
        pthread_mutex_unlock(pMutex);
#endif
    }


    void platform_sleep(int millisecs)
    {
#ifdef WIN32
        Sleep(millisecs);
#else
        usleep(millisecs * MicrosecsInMillisec);
#endif
    }


    STATUS_T platform_create_semaphore(PLATFORM_SEMAPHORE_T& semHandle,
    					int initialCount,int maximumCount)
    {

 		STATUS_T rv =0;
#ifdef WIN32

        PLATFORM_SEMAPHORE_T
            tmp =  CreateSemaphore(NULL,
                                initialCount,
                                maximumCount,
                                NULL);
        if (tmp == NULL)
            {
                rv = GetLastError();
            }
        else
            {
                semHandle = tmp;
            }
#else
        rv = sem_init(&semHandle,
                            0,
                            initialCount);
#endif

        return rv;
    }


    STATUS_T platform_wait_for_signal(PLATFORM_SEMAPHORE_T& semHandle)
    {
		STATUS_T retVal;
#ifdef WIN32

        retVal = WaitForSingleObject(semHandle,	INFINITE);
#else
        retVal = sem_wait(&semHandle);
#endif
		return retVal;
    }


    void platform_signal_semaphore(PLATFORM_SEMAPHORE_T& semHandle)
    {
#ifdef WIN32
        ReleaseSemaphore(semHandle, 1, NULL);
#else
        sem_post(&semHandle);
#endif
    }

    void platform_release_semaphore(PLATFORM_SEMAPHORE_T& semHandle)
    {
#ifdef WIN32

        CloseHandle(semHandle);
#else

        sem_destroy(&semHandle);
#endif
    }

    int32_t platform_interruptable_sleep(int64_t length)
    {
#ifdef WIN32
        return SleepEx(static_cast<DWORD>(length), TRUE);
#else
        struct timespec req = {0};
        time_t sec = (int)(length/1000);
        length = length - (sec*1000);
        req.tv_sec = sec;
        req.tv_nsec = length*1000000L;

        int ret = nanosleep(&req, NULL);

        return ret;
#endif
    }

    bool platform_interrupt_sleep(PLATFORM_THREAD_T handle)
    {
#ifdef WIN32
        int32_t result = QueueUserAPC(default_interrupt_handler, handle, 0);

        return (result!=0);
#else
        return (0 == pthread_kill(handle, SIGUSR1));
#endif
    }

#ifndef WIN32
    SignalUSR1Handler SignalUSR1Handler::instance;

    SignalUSR1Handler::SignalUSR1Handler()
    {
        //sa = {0}, old = {0};
        sa.sa_handler = &default_interrupt_handler;
        sigaction(SIGUSR1, &sa, &old);
    }

    SignalUSR1Handler::~SignalUSR1Handler()
    {
        sigaction(SIGUSR1, &old, NULL);
    }
#endif

}
