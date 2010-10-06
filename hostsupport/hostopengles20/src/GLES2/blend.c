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

#include "common.h"
#include "hgl.h"
#include "context.h"

static GLboolean dglIsValidBlendMode(GLenum mode)
{
	switch(mode)
	{
		case GL_FUNC_ADD:
		case GL_FUNC_SUBTRACT:
		case GL_FUNC_REVERSE_SUBTRACT:
			return GL_TRUE;

		default:
			return GL_FALSE;
	}
}

GL_APICALL_BUILD void GL_APIENTRY glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	DGLES2_ENTER();
	ctx->hgl.BlendColor(red, green, blue, alpha);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glBlendEquation(GLenum mode)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValidBlendMode(mode), GL_INVALID_ENUM);
	ctx->hgl.BlendEquation(mode);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValidBlendMode(modeRGB), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsValidBlendMode(modeAlpha), GL_INVALID_ENUM);
	ctx->hgl.BlendEquationSeparate(modeRGB, modeAlpha);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	DGLES2_ENTER();
	ctx->hgl.BlendFunc(sfactor, dfactor);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
	DGLES2_ENTER();
	ctx->hgl.BlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
	DGLES2_LEAVE();
}


