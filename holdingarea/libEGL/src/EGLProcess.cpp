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

#include "EGLProcess.h"
#include "EGLThread.h"
#include "eglInternal.h"

CEGLProcess::CEGLProcess( EGLint processId ) :
    m_id( processId ),
    m_currentThread( NULL )
    {
    }

CEGLProcess::~CEGLProcess(void)
    {
    DestroyPointerVector<CEGLThread>(m_threads);
    }

CEGLThread* CEGLProcess::AddThread( EGLI_THREAD_ID threadId, bool setCurrent, EGLint supportedApis )
    {
    CEGLThread* thread = HasThread( threadId );
    if( !thread )
        {
        thread = EGLI_NEW CEGLThread( threadId, supportedApis );
        if( thread )
            {
            AddObject<CEGLThread>( m_threads, thread );
            }
        }    
    if( setCurrent && thread )
        {
        m_currentThread = thread;
        }
    return thread;
    }

void CEGLProcess::RemoveThread( EGLI_THREAD_ID threadId )
    {
    if( m_currentThread && m_currentThread->Id() == threadId )
        {
        m_currentThread = NULL;
        }
    DeleteObjectById<CEGLThread>( m_threads, threadId );
    }

CEGLThread* CEGLProcess::HasThread( EGLI_THREAD_ID threadId ) const
    {
    return FindObjectById<CEGLThread>( m_threads, threadId, NULL );
    }

void CEGLProcess::SetCurrentThread( EGLI_THREAD_ID threadId )
    {
#if defined(EGLI_USE_PLATSIM_EXTENSIONS)
    EGLI_ASSERT(m_currentThread != NULL);
    if( m_currentThread->Id() != threadId )
        {
        m_currentThread = FindObjectById<CEGLThread>( m_threads, threadId, NULL );
        EGLI_ASSERT( m_currentThread != NULL );
        }
#else
    EGLI_ASSERT( false );
#endif
    }

CEGLThread* CEGLProcess::CurrentThread() const
    { 
#if defined(EGLI_USE_PLATSIM_EXTENSIONS)
    EGLI_ASSERT( m_currentThread != NULL );
    return m_currentThread;
#elif defined(_WIN32)
    EGLI_THREAD_ID threadId = GetCurrentThreadId();
    CEGLThread* thread = FindObjectById( m_threads, threadId, NULL );
    //EGLI_ASSERT( thread != NULL );
    return thread;
#else // Linux
    // \todo
#endif
    }

CEGLSurface* CEGLProcess::FindBoundSurface( CEGLContext* context, bool readSurface ) const
     {
     EGLI_ASSERT( context != NULL );
     CEGLSurface* ret = NULL;         
     std::vector<CEGLThread*>::const_iterator iter = m_threads.begin();
     while( iter != m_threads.end() )
         {
         if( (*iter)->CurrentVGContext() == context )
             {
             ret = (*iter)->CurrentVGSurface();
             break;
             }
         else if( (*iter)->CurrentGLESContext() == context )
             {
             //TODO
             EGLI_ASSERT( false );
             }
         }
     return ret;
     }


