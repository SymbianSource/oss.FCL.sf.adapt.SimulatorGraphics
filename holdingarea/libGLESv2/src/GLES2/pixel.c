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

GL_APICALL_BUILD void GL_APIENTRY glPixelStorei(GLenum pname, GLint param)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(pname != GL_UNPACK_ALIGNMENT && pname != GL_PACK_ALIGNMENT, GL_INVALID_ENUM);
	ctx->hgl.PixelStorei(pname, param);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(format != GL_RGBA, GL_INVALID_ENUM);
	DGLES2_ERROR_IF(type != GL_UNSIGNED_BYTE, GL_INVALID_ENUM);
	DGLES2_BEGIN_READING();
	ctx->hgl.ReadPixels(x, y, width, height, format, type, pixels);
	DGLES2_END_READING();
	DGLES2_LEAVE();
}

