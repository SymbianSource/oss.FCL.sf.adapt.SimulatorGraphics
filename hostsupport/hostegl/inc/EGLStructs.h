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

#ifndef _EGLSTRUCTS_H_
#define _EGLSTRUCTS_H_

struct EGLIOsWindowContext
    {
    EGLINativeWindowType    window;
	EGLINativeDisplayType   vgDisplay;
    EGLINativeDisplayType   glesDisplay;
	EGLINativePixmapType 	pixmap;
    unsigned int*           osBuffer;
    int                     width;
    int                     height;
    void*                   colorBuf;
    };

struct EGLINativeGLFunctions 
    {
#if defined(_WIN32)
    // PBuffer
    PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB;
    PFNWGLGETCURRENTREADDCARBPROC wglGetCurrentReadDCARB;
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
    PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;
    PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;
    PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;
    PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
    PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;
#else // Linux
    // \todo
#endif
    };

struct EGLINativePbufferContainer
    {
    EGLINativeContextType   copyContext;
    EGLINativePbufferType   pbuffer;
    EGLINativeDisplayType   display;
    EGLINativeGLFunctions   functions;
    };

struct EGLITextureBinding
	{
	EGLint		name;
	EGLint		level;
	EGLint		clientVersion;
	EGLContext	context;
	};

#endif // _EGLSTRUCTS_H_
