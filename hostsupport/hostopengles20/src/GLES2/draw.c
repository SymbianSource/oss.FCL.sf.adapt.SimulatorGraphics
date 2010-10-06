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

static GLboolean dglIsValidCapability(GLenum cap)
{
	switch(cap)
	{
		case GL_TEXTURE_2D:
		case GL_CULL_FACE:
		case GL_POLYGON_OFFSET_FILL:
		case GL_SAMPLE_ALPHA_TO_COVERAGE:
		case GL_SAMPLE_COVERAGE:
		case GL_SCISSOR_TEST:
		case GL_STENCIL_TEST:
		case GL_DEPTH_TEST:
		case GL_BLEND:
		case GL_DITHER:
			return GL_TRUE;

		default:
			return GL_FALSE;
	}
}

GL_APICALL_BUILD void GL_APIENTRY glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	DGLES2_ENTER();
	ctx->hgl.ColorMask(red, green, blue, alpha);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glCullFace(GLenum mode)
{
	DGLES2_ENTER();
	ctx->hgl.CullFace(mode);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glDisable(GLenum cap)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValidCapability(cap), GL_INVALID_ENUM);
	ctx->hgl.Disable(cap);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glEnable(GLenum cap)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValidCapability(cap), GL_INVALID_ENUM);
	ctx->hgl.Enable(cap);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glFinish(void)
{
	DGLES2_ENTER();
	DGLES2_BEGIN_DRAWING();
	ctx->hgl.Finish();
	DGLES2_END_DRAWING();
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glFlush(void)
{
	DGLES2_ENTER();
	DGLES2_BEGIN_DRAWING();
	ctx->hgl.Flush();
	DGLES2_END_DRAWING();
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glFrontFace(GLenum mode)
{
	DGLES2_ENTER();
	ctx->hgl.FrontFace(mode);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD GLboolean GL_APIENTRY glIsEnabled(GLenum cap)
{
	DGLES2_ENTER_RET(GL_FALSE);
	DGLES2_ERROR_IF_RET(!dglIsValidCapability(cap), GL_INVALID_ENUM, GL_FALSE);
	DGLES2_LEAVE_RET(ctx->hgl.IsEnabled(cap));
}

GL_APICALL_BUILD void GL_APIENTRY glHint(GLenum target, GLenum mode)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_GENERATE_MIPMAP_HINT, GL_INVALID_ENUM);
	ctx->hgl.Hint(target, mode);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glLineWidth (GLfloat width)
{
	DGLES2_ENTER();
	ctx->hgl.LineWidth(width);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glPolygonOffset(GLfloat factor, GLfloat units)
{
	DGLES2_ENTER();
	ctx->hgl.PolygonOffset(factor, units);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glSampleCoverage(GLclampf value, GLboolean invert)
{
	DGLES2_ENTER();
	ctx->hgl.SampleCoverage(value, invert);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	DGLES2_ENTER();
	ctx->hgl.Scissor(x, y, width, height);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	DGLES2_ENTER();
	ctx->hgl.Viewport(x, y, width, height);
	DGLES2_LEAVE();
}

