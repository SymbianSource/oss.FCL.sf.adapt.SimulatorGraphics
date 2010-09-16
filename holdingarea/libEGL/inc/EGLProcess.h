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

#ifndef _EGLPROCESS_H_
#define _EGLPROCESS_H_

#include <vector>
#include "eglInternal.h"

class CEGLThread;
class CEGLContext;
class CEGLSurface;

class CEGLProcess
{
public:
	CEGLProcess( EGLint processId );
	~CEGLProcess(void);

public:
    inline EGLint Id() const { return m_id; }
    CEGLThread* AddThread( EGLI_THREAD_ID threadId, bool setCurrent, EGLint supportedApis );
    void RemoveThread( EGLI_THREAD_ID threadId );
    CEGLThread* HasThread( EGLI_THREAD_ID threadId ) const;
    inline EGLint ThreadCount() const { return m_threads.size(); }
    void SetCurrentThread( EGLI_THREAD_ID threadId );
    CEGLThread* CurrentThread() const;
    CEGLSurface* FindBoundSurface( CEGLContext* context, bool readSurface ) const;

private:
    EGLint                      m_id;
    std::vector<CEGLThread*>    m_threads;
    CEGLThread*                 m_currentThread;
};
#endif //_EGLPROCESS_H_