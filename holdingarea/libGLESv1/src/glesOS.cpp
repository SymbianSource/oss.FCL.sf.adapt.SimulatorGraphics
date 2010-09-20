/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#include "glesOS.h"
#include "glesInternal.h"

namespace
{
static bool g_lockInitialized = false;
static GLES_LOCK g_lock;
}

#if defined(_WIN32)
static void initializeLock()
{
    GLES_ASSERT(!g_lockInitialized);
    InitializeCriticalSection(&g_lock);
    g_lockInitialized = true;
}

void glesGetLock()
{
    if(!g_lockInitialized)
    {
        initializeLock();
    }
    EnterCriticalSection(&g_lock);
}

void glesReleaseLock()
{
    GLES_ASSERT(g_lockInitialized);
    LeaveCriticalSection(&g_lock);
}
#else // defined(_WIN32)
static void initializeLock()
{
    int ret;

    GLES_ASSERT(!g_lockInitialized);

    pthread_mutexattr_t attr;
    ret = pthread_mutexattr_init(&attr);	//initially not locked
    GLES_ASSERT(!ret);	//check that there aren't any errors
    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);	//count the number of recursive locks
    GLES_ASSERT(!ret);	//check that there aren't any errors
    ret = pthread_mutex_init(&g_lock, &attr);
    pthread_mutexattr_destroy(&attr);
    GLES_ASSERT(!ret);	//check that there aren't more errors

    g_lockInitialized = true;
}

void glesGetLock()
{
    if(!g_lockInitialized)
    {
        initializeLock();
    }
    int ret = pthread_mutex_lock(&g_lock);
    GLES_ASSERT(!ret);
}

void glesReleaseLock()
{
    GLES_ASSERT(g_lockInitialized);
    int ret = pthread_mutex_unlock(&g_lock);
    GLES_ASSERT(!ret);
}
#endif // !defined(_WIN32)
