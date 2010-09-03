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
// This header is to create platform independent
// threading / synchronization API types and values.

#ifndef PSU_PLATFORMTHREADING_H
#define PSU_PLATFORMTHREADING_H

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#endif

#include "platformtypes.h"

namespace Psu
{

#ifdef WIN32

    typedef HANDLE PLATFORM_THREAD_T;
    typedef DWORD PLATFORM_THREADFUNC_RETTYPE;
    typedef LPTHREAD_START_ROUTINE PLATFORM_THREADFUNC;
    typedef CRITICAL_SECTION PLATFORM_MUTEX_T;
    typedef HANDLE PLATFORM_SEMAPHORE_T;
    typedef DWORD STATUS_T;

#ifdef __GNUG__
    typedef void (*PLATFORM_INTERRUPT_HANDLER)(ULONG_PTR);
#else
    typedef VOID CALLBACK PLATFORM_INTERRUPT_HANDLER (__in  ULONG_PTR);
#endif

#else // LINUX

    typedef pthread_t PLATFORM_THREAD_T;
    typedef void * PLATFORM_THREADFUNC_RETTYPE;
    typedef void * (*PLATFORM_THREADFUNC)(void *);
    typedef pthread_mutex_t PLATFORM_MUTEX_T;
    typedef sem_t PLATFORM_SEMAPHORE_T;
    typedef int STATUS_T;

    typedef void (*PLATFORM_INTERRUPT_HANDLER)(int);

#define WINAPI

#endif

#ifndef WIN32
    extern const int MicrosecsInMillisec;
#endif


    /**
     * Creates a thread with the default attributes.
     */
    int platform_create_simple_thread(PLATFORM_THREAD_T * pThreadHandle,
                                      PLATFORM_THREADFUNC pFunc,
                                      void * param);

    /**
     * Releases a thread (does not kill it).
     */
    void platform_release_thread(PLATFORM_THREAD_T threadHandle);

    /**
     * Waits for a thread to terminate.
     */
    STATUS_T platform_join_thread(PLATFORM_THREAD_T threadHandle);

    /**
     * Initializes a mutex (CRITICAL_SECTION on windows).
     */
    void platform_mutex_init(PLATFORM_MUTEX_T * pMutex);

    /**
     * Destroys a mutex (CRITICAL_SECTION on windows).
     */
    void platform_mutex_destroy(PLATFORM_MUTEX_T * pMutex);

    /**
     *Locks a mutex  ("enters" a CRITICAL_SECTION on windows).
     */
    void platform_mutex_lock(PLATFORM_MUTEX_T * pMutex);

    /**
     * Unlocks a mutex ("exits" a CRITICAL_SECTION on windows).
     */
    void platform_mutex_unlock(PLATFORM_MUTEX_T * pMutex);


    /**
     * Sleeps for a certain duration for given milliseconds.
     */
    void platform_sleep(int millisecs);

	 /**
     * Creates a semaphore with the default attributes.
     */
	STATUS_T platform_create_semaphore(PLATFORM_SEMAPHORE_T& semHandle,
    					int initialCount,int maximumCount);


    /**
     * wait for the semaphore signal
     */
    STATUS_T platform_wait_for_signal(PLATFORM_SEMAPHORE_T& semHandle);

    /**
     *  Posts a semaphore signal so that the pending thread can continue
     */
    void platform_signal_semaphore(PLATFORM_SEMAPHORE_T& semHandle);

    /**
     * releases a semaphore object/handle.
     */
    void platform_release_semaphore(PLATFORM_SEMAPHORE_T& semHandle);

    /**
    * do an interruptable sleep
    * @param length the length thread should sleep, in milliseconds
    * @return 0 if the sleep expires normally, non-zero if the sleep has
    * been interrupted
    */
    int32_t platform_interruptable_sleep(int64_t length);

    /**
    * signals a thread to interrupt a sleep
    * @param handle PLATFORM_THREAD_T handle to the thread
    * @return whether the interrupt succeeded or not
    */
    bool platform_interrupt_sleep(PLATFORM_THREAD_T handle);

#ifndef WIN32
    class SignalUSR1Handler
    {
    private:
        SignalUSR1Handler();
        ~SignalUSR1Handler();

        struct sigaction sa;
        struct sigaction old;

    public:
        static SignalUSR1Handler instance;
    };
#endif
}

#endif
