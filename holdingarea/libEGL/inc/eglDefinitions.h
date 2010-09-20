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

#ifndef _EGLDEFINITIONS_H_
#define _EGLDEFINITIONS_H_

#if defined(_WIN32)
#include <windows.h>
#include <GL\gl.h>
#include <GL\wglext.h>
#else // Linux
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <assert.h>
#endif

#define EGL_VERSION_MAJOR 1
#define EGL_VERSION_MINOR 4

#define EGLI_NEW new(std::nothrow)

#define EGLI_NO_RET

// \note PlatSim specific hack
#if defined(EGLI_USE_PLATSIM_EXTENSIONS)
    typedef int     EGLINativeDisplayType;
    typedef void*   EGLINativePixmapType;
    typedef void*   EGLINativeWindowType;
#else
    typedef EGLNativeDisplayType    EGLINativeDisplayType;
    typedef EGLNativePixmapType     EGLINativePixmapType;
    typedef EGLNativeWindowType     EGLINativeWindowType;
#endif

#if !defined(EGLI_MIN)
#   define EGLI_MIN(a,b) ((a)<(b)?(a):(b))
#endif

#if !defined(EGLI_MAX)
#   define EGLI_MAX(a,b) ((a)>(b)?(a):(b))
#endif

#if defined(_WIN32)
#   if defined(_DEBUG)
#       define EGLI_ASSERT assert
#   else
#      define EGLI_ASSERT
#   endif //_DEBUG
#else
#   if !defined(NDEBUG)
#       define EGLI_ASSERT assert
#   else
#       define EGLI_ASSERT
#   endif
#endif

#if defined(_WIN32)

#if defined(EGLI_USE_PLATSIM_EXTENSIONS)
#   define EGLI_THREAD_ID EGLint
#else
#   define EGLI_THREAD_ID DWORD
#endif // EGLI_USE_PLATSIM_EXTENSIONS
    typedef CRITICAL_SECTION        EGLI_LOCK;
    typedef HGLRC                   EGLINativeContextType;
    typedef HPBUFFERARB             EGLINativePbufferType;
    typedef PIXELFORMATDESCRIPTOR   EGLINativePixelFormatType;
    typedef HMODULE                 EGLILibraryHandle;
#else // Linux

#if defined(EGLI_USE_PLATSIM_EXTENSIONS)
#   define EGLI_THREAD_ID EGLint
#else
#   define EGLI_THREAD_ID pid_t
#endif // EGLI_USE_PLATSIM_EXTENSIONS
    typedef pthread_mutex_t EGLI_LOCK;
    typedef GLXContext      EGLINativeContextType;
    typedef GLXPbuffer      EGLINativePbufferType;
    typedef XVisualInfo     EGLINativePixelFormatType;
    typedef void*           EGLILibraryHandle;

#endif


#endif // _EGLDEFINITIONS_H_
