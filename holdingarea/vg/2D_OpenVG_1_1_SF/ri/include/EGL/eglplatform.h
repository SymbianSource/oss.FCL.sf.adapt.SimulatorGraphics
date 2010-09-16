/* Copyright (c) 2007 The Khronos Group Inc.
 * Portions copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 */

/* Platform-specific types and definitions for egl.h */

#ifndef __eglplatform_h_
#define __eglplatform_h_

/* #define EGLAPIENTRY */       /* empty */
#if defined(_WIN32) && !defined(_WIN32_WCE) && !defined(__SCITECH_SNAP__)
    /* Win32 but not WinCE */
#   define EGLAPIENTRY __stdcall
#else
#   define EGLAPIENTRY
#endif


#if defined(EGL_STATIC_LIBRARY)
#	define EGLAPI
#else
#	if defined(_WIN32) || defined(__VC32__)				/* Win32 */
#		if defined (OPENVG_DLL_EXPORTS)
#			define EGLAPI __declspec(dllexport)
#		else
#			define EGLAPI __declspec(dllimport)
#		endif
#	elif defined(__unix__) || defined(__APPLE__)                             /* Mac OS X */
#		define EGLAPI extern
#	endif
#endif /* defined(EGL_STATIC_LIBRARY) */

typedef struct
{
	int				format;		/*VGImageFormat cast to int*/
	int				width;
	int				height;
	int				stride;		/*in bytes*/
	void*			data;
} NativePixmap;

typedef void*           NativeDisplayType;
typedef void*           NativeWindowType;
typedef NativePixmap*	NativePixmapType;

/* EGL 1.2 types, renamed for consistency in EGL 1.3 */
typedef NativeDisplayType EGLNativeDisplayType;
typedef NativePixmapType  EGLNativePixmapType;
typedef NativeWindowType  EGLNativeWindowType;

#define EGL_NOKIA_RI

#endif /* __eglplatform_h */

