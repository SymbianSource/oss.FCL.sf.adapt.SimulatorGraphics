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

#ifndef _EGLINTERNAL_H_
#define _EGLINTERNAL_H_

#include <EGL/egl.h>
#include <vector>
#include "VGInterface.h"
#include "GLESInterface.h"
#include "GLES2Interface.h"
#include "eglDefinitions.h"
#include "EGLStructs.h"
#include "EGLUtils.h"

class CEGLState;
class CEGLThread;
extern CEGLState* getState();
extern CEGLThread* getThread();
extern void setEGLError( EGLint error );

// Global EGL lock
extern EGLI_LOCK g_eglLock;  

#define EGLI_ENTER_RET(ret)\
    CEGLOs::GetLock( &g_eglLock );\
    CEGLState* state = getState();\
    if( !state )\
        {\
        CEGLOs::ReleaseLock( &g_eglLock );\
        return ret;\
        }\
    CEGLProcess* process = state->GetCurrentProcess(); \
    (void)process;

#define EGLI_LEAVE_RET(ret, error)\
    setEGLError( error );\
    CEGLOs::ReleaseLock( &g_eglLock );\
    return ret
#endif // _EGLINTERNAL_H_
