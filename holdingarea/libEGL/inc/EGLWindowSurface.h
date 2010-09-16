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

#ifndef _EGLWINDOWSURFACE_H_
#define _EGLWINDOWSURFACE_H_

#include "EGLSurface.h"

class CEGLWindowSurface :
	public CEGLSurface
{
public:
	CEGLWindowSurface( CEGLConfig* config, EGLint colorSpace, EGLint alphaFormat, EGLint renderBuffer, EGLIOsWindowContext* osContext );
	~CEGLWindowSurface(void);

public:
    inline EGLINativeWindowType NativeType() const { EGLI_ASSERT( m_osContext != NULL); if( m_osContext ) return m_osContext->window; else return NULL; }
    inline void SetWindowBuffers( void* buffer0, void* buffer1 ) { m_buffer0 = buffer0; m_buffer1 = buffer1; }
    void* CurrentBuffer();
    void SetSwapInterval( EGLint interval );
    inline EGLint SwapInterval() const { return m_swapInterval; }
    void SetOsContext( EGLIOsWindowContext* osContext );
    inline EGLIOsWindowContext* OsContext() const { return m_osContext; }

private:
    CEGLWindowSurface();

private:
    void*               m_buffer0;
    void*               m_buffer1;
    void*               m_currentBuffer;
    EGLint              m_swapInterval;
    EGLIOsWindowContext* m_osContext;
};
#endif //_EGLWINDOWSURFACE_H_