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


#include "EGLContext.h"
#include "EGLOs.h"

CEGLContext::CEGLContext( EGLint processId, CEGLDisplay* display,
                         CEGLConfig* config, EGLenum& clientApi,
                         EGLint& clientVersion, void* clientContext ) :
    m_processId( processId ),
    m_thread( NULL ),
    m_config( config ),
    m_clientApi( clientApi ),
    m_clientVersion( clientVersion ),
    m_clientContext( clientContext ),
    m_display( display ),
    m_nativeContext( NULL ),
    m_nativeGLFunctions(NULL)
    {
    }

CEGLContext::~CEGLContext(void)
    {
    // objects not owned
    m_sharedContexts.clear();
    if( m_nativeContext ) CEGLOs::DestroyNativeContext( m_nativeContext );
    delete m_nativeGLFunctions;
    }
