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

#ifndef _EGLCONTEXT_H_
#define _EGLCONTEXT_H_

#include <EGL/egl.h>
#include "eglInternal.h"
#include <vector>

class CEGLConfig;
class CEGLThread;
class CEGLDisplay;

class CEGLContext : public RefCountingObject
{
public:
	CEGLContext( EGLint processId, CEGLDisplay* display, CEGLConfig* config, EGLenum& clientApi, EGLint& clientVersion, void* clientContext );
	~CEGLContext(void);

public:
    inline EGLint ProcessId() const { return m_processId; }
    inline EGLenum ClientApi() const { return m_clientApi; }
    inline EGLint ClientVersion() const { return m_clientVersion; }
    inline void* ClientContext() const { return  m_clientContext; }
    inline CEGLConfig* Config() const { return m_config; }
    inline void SetThread( CEGLThread* thread ) { m_thread = thread; }
    inline CEGLThread* Thread() const { return m_thread; }
    inline CEGLDisplay* Display() const { return m_display; }

    inline void SetNativeContext( EGLINativeContextType context ) { m_nativeContext = context; }
    inline EGLINativeContextType NativeContext() const { return m_nativeContext; }
    inline void SetNativeGLFucntions( struct EGLINativeGLFunctions* functions ) { m_nativeGLFunctions = functions; } 
    inline struct EGLINativeGLFunctions* NativeGLFunctions() const { return m_nativeGLFunctions; }

private:
    EGLint                          m_processId;
    CEGLThread*                     m_thread;
    CEGLConfig*                     m_config;
    EGLenum                         m_clientApi;
    EGLint                          m_clientVersion;
	void*                           m_clientContext;
    std::vector<CEGLContext*>       m_sharedContexts;
    CEGLDisplay*                    m_display;
    EGLINativeContextType           m_nativeContext;
    struct EGLINativeGLFunctions*   m_nativeGLFunctions;
};

#endif //_EGLCONTEXT_H_
