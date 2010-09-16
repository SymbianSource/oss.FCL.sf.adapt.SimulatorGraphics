/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#include "common.h"
#include "platform.h"

static int dgl_lock_initialized = 0;
static DGLES2_LOCK dgl_lock;

#ifdef _WIN32
static void dglInitializeLock(void)
{
    InitializeCriticalSection(&dgl_lock);
    dgl_lock_initialized = 1;
}

void dglGetLock(void)
{
    if(!dgl_lock_initialized)
    {
        dglInitializeLock();
    }
    EnterCriticalSection(&dgl_lock);
}

void dglReleaseLock(void)
{
    DGLES2_ASSERT(dgl_lock_initialized);
    LeaveCriticalSection(&dgl_lock);
}
#else /* _WIN32 */
static void initializeLock()
{
    int ret;

    DGLES2_ASSERT(!dgl_lock_initialized);

    pthread_mutexattr_t attr;
    ret = pthread_mutexattr_init(&attr);	//initially not locked
    DGLES2_ASSERT(!ret);	//check that there aren't any errors
    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);	//count the number of recursive locks
    DGLES2_ASSERT(!ret);	//check that there aren't any errors
    ret = pthread_mutex_init(&dgl_lock, &attr);
    pthread_mutexattr_destroy(&attr);
    DGLES2_ASSERT(!ret);	//check that there aren't more errors

    dgl_lock_initialized = 1;
}

void glesGetLock()
{
    if(!dgl_lock_initialized)
    {
        initializeLock();
    }
    int ret = pthread_mutex_lock(&dgl_lock);
    DGLES2_ASSERT(!ret);
}

void glesReleaseLock()
{
    DGLES2_ASSERT(dgl_lock_initialized);
    int ret = pthread_mutex_unlock(&dgl_lock);
    DGLES2_ASSERT(!ret);
}
#endif
