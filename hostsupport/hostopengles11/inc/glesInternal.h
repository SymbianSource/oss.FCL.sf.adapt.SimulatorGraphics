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

#ifndef _GLINTERNAL_H_
#define _GLINTERNAL_H_

#include "glesOS.h"
#include <GLES/gl.h>
#include <new>

#define X_TO_D(x) ((x) / 65536.0)
#define X_TO_F(x) ((x) / 65536.0f)
#define D_TO_X(d) ((d) > 32767.65535 ? 32767 * 65536 + 65535 : \
                   (d) < -32768.65535 ? -32768 * 65536 + 65535 : \
                   ((GLfixed) ((d) * 65536)))
#define F_TO_X(f) D_TO_X(f)
#define F_TO_D(f) ((double) (f))
#define I_TO_X(i) ((GLfixed) ((i) * 65536))
#define X_TO_I(x) ((x) / 65536)

#include <assert.h>

#if defined(_DEBUG)
#define GLES_ASSERT assert
#else // defined(_DEBUG)
#define GLES_ASSERT
#endif // !defined(_DEBUG)

#define GLES_NEW new(std::nothrow)

// Types not in OpenGL ES
typedef double GLdouble;
typedef double GLclampd;

// Definitions not in OpenGL ES
#define GL_POINT_SIZE_ARRAY 			0x8B9C
#define GL_POINT_SPRITE					0x8861
#define GL_COORD_REPLACE				0x8862
#define GL_BGR              			0x80E0
#define GL_BGRA             			0x80E1
#define GL_RGB8             			0x8051
#define GL_RGBA8            			0x8058
#ifndef GLES_NO_SRGB
#define GL_SRGB8						0x8C41
#define GL_SRGB8_ALPHA8					0x8C43
#define GL_SLUMINANCE8					0x8C47
#else
// No sRGB support in OpenGL 2.0.
#define GL_SRGB8						GL_RGB8
#define GL_SRGB8_ALPHA8					GL_RGBA8
#define GL_SLUMINANCE8					GL_LUMINANCE8
#endif
#define GL_LUMINANCE8					0x8040
#define GL_ALPHA8						0x803C
#define GL_UNSIGNED_INT_8_8_8_8_REV		0x8367
#define GL_UNSIGNED_SHORT_1_5_5_5_REV   0x8366
#define GL_UNSIGNED_SHORT_4_4_4_4_REV   0x8365

#define GLES_API_CALL GL_API

#define GLES_MIN(A, B) (A < B ? A : B)
#define GLES_MAX(A, B) (A > B ? A : B)

#define GLES_NO_RETVAL ;

// Surface locking
#define GLES_LOCK_SURFACE_RET(READ, DRAW, RETVAL) \
	do \
	{ \
		if(!EGLtoGLESInterface::GetEGLInterface()->LockGLESSurface(READ, DRAW)) \
		{ \
			GLES_LEAVE_NO_ERROR_CHECK_RET(RETVAL); \
		} \
	} while(0)

#define GLES_LOCK_SURFACE(READ, DRAW) GLES_LOCK_SURFACE_RET(READ, DRAW, GLES_NO_RETVAL)

#define GLES_LOCK_DRAW_SURFACE_RET(RETVAL) GLES_LOCK_SURFACE_RET(false, true, RETVAL)
#define GLES_LOCK_READ_SURFACE_RET(RETVAL) GLES_LOCK_SURFACE_RET(true, false, RETVAL)
#define GLES_LOCK_DRAW_SURFACE() GLES_LOCK_SURFACE_RET(false, true, GLES_NO_RETVAL)
#define GLES_LOCK_READ_SURFACE() GLES_LOCK_SURFACE_RET(true, false, GLES_NO_RETVAL)

#define GLES_UNLOCK_SURFACE_RET(RETVAL) \
	do \
	{ \
		if(!EGLtoGLESInterface::GetEGLInterface()->UnlockGLESSurface()) \
		{ \
			GLES_LEAVE_RET(RETVAL); \
		} \
	} while(0)

#define GLES_UNLOCK_SURFACE() GLES_UNLOCK_SURFACE_RET(GLES_NO_RETVAL)

inline bool glesIsPowerOfTwo(int x)
{
	return (x != 0) && ((x & (x - 1)) == 0);
}

inline int glesLog2(unsigned int x)
{
	int r = 0;
	while(x >>= 1)
	{
		r++;
	}
	return r;
}

#endif // _GLINTERNAL_H_
