/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>

// Correct defines for platform detection.
#if(CONFIG_X11 == 1)
#	ifndef __unix__
#		define __unix__
#	endif
#elif(CONFIG_OFFSCREEN == 1)
#	define __offscreen__
#elif(CONFIG_WIN32 == 1)
#	define __win32__
#else
#	error "Backend needs to be enabled!"
#endif // else

// DLL-mangling.
#ifdef _WIN32
#	define DGLES2_EXPORT __declspec(dllexport)
#	define DGLES2_IMPORT __declspec(dllimport)
#	define DGLES2_CALL	 __stdcall
#else
#	define DGLES2_EXPORT
#	define DGLES2_IMPORT extern
#	define DGLES2_CALL
#endif

// For proper imports and exports.
#if(defined BUILD_EGL)
#	define EGLAPI       extern
#	define EGLAPI_BUILD DGLES2_EXPORT
#elif(defined BUILD_GLES2)
#	define GL_APICALL       DGLES2_EXPORT
#	define GL_APICALL_BUILD DGLES2_EXPORT
#else
#	error "Only to be used with EGL or GLES!"
#endif

// The actual standard headers.
#include "EGL/egl.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

// For malloc
#ifdef __APPLE__
#	include <stdlib.h>
#else
#	include <malloc.h>
#endif

// Debug location aids.
#ifdef _MSC_VER
#	define __PRETTY_FUNCTION__ __FUNCSIG__
#	ifdef _DEBUG
#		define CONFIG_DEBUG 1
#	endif
#else
#   if !defined(NDEBUG)
#       define CONFIG_DEBUG 1
#   endif
#endif

#define COMMON_STAMP_FMT "%s:%d(%s)"
#define COMMON_STAMP_ARGS ,(strchr(__FILE__, '/')?"":__FILE__), __LINE__, __PRETTY_FUNCTION__

#define DUMMY() \
	fprintf(stderr, "\x1b[41mDUMMY\x1b[0m " COMMON_STAMP_FMT ": Unimplemented!\n" COMMON_STAMP_ARGS)
#define STUB_ONCE(format, ...) \
	{ \
		static int once = 1; \
		if(once) \
		{ \
			fprintf(stderr, "\x1b[43mSTUB ONCE\x1b[0m " COMMON_STAMP_FMT ": " format COMMON_STAMP_ARGS, __VA_ARGS__); \
			once = 0; \
		} \
	} (void) 0
#define STUB(format, ...) \
	fprintf(stderr, "STUB " COMMON_STAMP_FMT ": " format COMMON_STAMP_ARGS, __VA_ARGS__)

#if(CONFIG_DEBUG == 1 && !defined NDEBUG)
#	define Dprintf(format, ...) fprintf(stderr, "DEBUG " COMMON_STAMP_FMT ": " format  COMMON_STAMP_ARGS, __VA_ARGS__)
#else // NDEBUG
#	define Dprintf(format, ...) (void)0
#endif // !NDEBUG

#if(CONFIG_DEBUG == 1 && !defined NDEBUG)
#	define DGLES2_ASSERT assert
#else
#	define DGLES2_ASSERT
#endif

// Context acquisition
#define DGLES2_NO_RETVAL ;

#define DGLES2_ENTER_RET(retval) \
	DGLContext* ctx; \
	{ \
		dglGetLock(); \
		ctx = deglGetCurrentContext(); \
		if(!ctx) \
		{ \
			dglReleaseLock(); \
			return retval; \
		} \
		if(!ctx->initialized) \
		{ \
			if(!DGLContext_initialize(ctx)) \
			{ \
				dglReleaseLock(); \
				return retval; \
			} \
		} \
	} (void) 0

// Check the host error API function calls so that the error flag
// in the wrapper always contains the latest error.
#define DGLES2_LEAVE_RET(retval) \
	{ \
		DGLContext_getHostError(ctx); \
		dglReleaseLock(); \
		return retval; \
	} (void) 0

// Error checking is not necessary when DGLContext_getHostError was already
// called at the end of the function body. Also, the EGL<->GLES interface
// functions should preserve the error flag, so this macro should be used there.
#define DGLES2_LEAVE_NO_ERROR_CHECK_RET(retval) \
	{ \
		dglReleaseLock(); \
		return retval; \
	} (void) 0

#define DGLES2_ENTER() DGLES2_ENTER_RET(DGLES2_NO_RETVAL)
#define DGLES2_LEAVE() DGLES2_LEAVE_RET(DGLES2_NO_RETVAL)
#define DGLES2_LEAVE_NO_ERROR_CHECK() DGLES2_LEAVE_NO_ERROR_CHECK_RET(DGLES2_NO_RETVAL)

// Surface locking
#define DGLES2_LOCK_SURFACE_RET(read, draw, retval) \
	do \
	{ \
		if(!deglLockSurface(read, draw)) \
		{ \
			DGLES2_LEAVE_NO_ERROR_CHECK_RET(retval); \
		} \
	} while(0)

#define DGLES2_LOCK_DRAW_SURFACE_RET(retval) DGLES2_LOCK_SURFACE_RET(0, 1, retval)
#define DGLES2_LOCK_READ_SURFACE_RET(retval) DGLES2_LOCK_SURFACE_RET(1, 0, retval)

#define DGLES2_UNLOCK_SURFACE_RET(retval) \
	do \
	{ \
		if(!deglUnlockSurface()) \
		{ \
			DGLES2_LEAVE_RET(retval); \
		} \
	} while(0)

#define DGLES2_UNLOCK_SURFACE() DGLES2_UNLOCK_SURFACE_RET(DGLES2_NO_RETVAL)

#define DGLES2_BEGIN_DRAWING_RET(retval) \
	do \
	{ \
		if(ctx->framebuffer_binding == 0) \
		{ \
			/* Using default framebuffer. */ \
			DGLES2_LOCK_DRAW_SURFACE_RET(retval); \
		} \
	} while(0)

#define DGLES2_BEGIN_DRAWING() DGLES2_BEGIN_DRAWING_RET(DGLES2_NO_RETVAL)

#define DGLES2_END_DRAWING_RET(retval) \
	do \
	{ \
		if(ctx->framebuffer_binding == 0) \
		{ \
			/* Using default framebuffer. */ \
			DGLES2_UNLOCK_SURFACE_RET(retval); \
		} \
		else \
		{ \
			/* Using user-created framebuffer. */ \
			DGLContext_updateFBOAttachmentSiblings(ctx); \
		} \
	} while(0)

#define DGLES2_END_DRAWING() DGLES2_END_DRAWING_RET(DGLES2_NO_RETVAL)

#define DGLES2_BEGIN_READING_RET(retval) \
	do \
	{ \
		if(ctx->framebuffer_binding == 0) \
		{ \
			/* Using default framebuffer. */ \
			DGLES2_LOCK_READ_SURFACE_RET(retval); \
		} \
	} while(0)

#define DGLES2_BEGIN_READING() DGLES2_BEGIN_READING_RET(DGLES2_NO_RETVAL)

#define DGLES2_END_READING_RET(retval) \
	do \
	{ \
		if(ctx->framebuffer_binding == 0) \
		{ \
			/* Using default framebuffer. */ \
			DGLES2_UNLOCK_SURFACE_RET(retval); \
		} \
	} while(0)

#define DGLES2_END_READING() DGLES2_END_READING_RET(DGLES2_NO_RETVAL)

// GL error handling
#define DGLES2_ERROR_IF_RET(cond, error_, retval) \
	{ \
		if(cond) \
		{ \
			/* Clear the host GL error flag(s). */ \
			while(ctx->hgl.GetError() != GL_NO_ERROR); \
			DGLContext_setError(ctx, error_); \
			dglReleaseLock(); \
			return retval; \
		} \
	} (void) 0

#define DGLES2_ERROR_IF(cond, error) DGLES2_ERROR_IF_RET(cond, error, DGLES2_NO_RETVAL)
#define DGLES2_ERROR_RET(error, retval) DGLES2_ERROR_IF_RET(1, error, retval)
#define DGLES2_ERROR(error) DGLES2_ERROR_IF(1, error)

#endif /* COMMON_H_ */
