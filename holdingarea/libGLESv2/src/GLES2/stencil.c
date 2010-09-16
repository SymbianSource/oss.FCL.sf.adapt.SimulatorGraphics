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

GL_APICALL_BUILD void GL_APIENTRY glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
	DGLES2_ENTER();
	ctx->hgl.StencilFunc(func, ref, mask);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
	DGLES2_ENTER();
	ctx->hgl.StencilFuncSeparate(face, func, ref, mask);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glStencilMask(GLuint mask)
{
	DGLES2_ENTER();
	ctx->hgl.StencilMask(mask);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glStencilMaskSeparate(GLenum face, GLuint mask)
{
	DGLES2_ENTER();
	ctx->hgl.StencilMaskSeparate(face, mask);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	DGLES2_ENTER();
	ctx->hgl.StencilOp(fail, zfail, zpass);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
	DGLES2_ENTER();
	ctx->hgl.StencilOpSeparate(face, fail, zfail, zpass);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glClearStencil(GLint s)
{
	DGLES2_ENTER();
	ctx->hgl.ClearStencil(s);
	DGLES2_LEAVE();
}

