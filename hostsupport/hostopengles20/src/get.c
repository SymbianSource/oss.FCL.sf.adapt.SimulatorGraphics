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
#include "get.h"

static GLboolean dglGet(const DGLContext* ctx, GLenum pname, DGLType dstType, void* params)
{
	int j;
	DGLType srcType;
	GLboolean b[1];
	GLint i[11];

	int n = 1;

	switch(pname)
	{
		case GL_SHADER_COMPILER:
			srcType = DGLES2_TYPE_BOOL;
			b[0] = GL_TRUE;
			break;

		case GL_NUM_SHADER_BINARY_FORMATS:
			srcType = DGLES2_TYPE_INT;
			i[0] = 0;
			break;

		case GL_SHADER_BINARY_FORMATS:
			// None.
			return GL_TRUE;

		case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
			srcType = DGLES2_TYPE_INT;
			i[0] = 11;
			break;

		case GL_COMPRESSED_TEXTURE_FORMATS:
			srcType = DGLES2_TYPE_ENUM;
			i[0] = GL_PALETTE4_RGB8_OES;
			i[1] = GL_PALETTE4_RGBA8_OES;
			i[2] = GL_PALETTE4_R5_G6_B5_OES;
			i[3] = GL_PALETTE4_RGBA4_OES;
			i[4] = GL_PALETTE4_RGB5_A1_OES;
			i[5] = GL_PALETTE8_RGB8_OES;
			i[6] = GL_PALETTE8_RGBA8_OES;
			i[7] = GL_PALETTE8_R5_G6_B5_OES;
			i[8] = GL_PALETTE8_RGBA4_OES;
			i[9] = GL_PALETTE8_RGB5_A1_OES;
			i[10] = GL_ETC1_RGB8_OES;
			n = 11;
			break;

		case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
			i[0] = GL_RGBA;
			srcType = DGLES2_TYPE_ENUM;
			break;

		case GL_IMPLEMENTATION_COLOR_READ_TYPE:
			i[0] = GL_UNSIGNED_BYTE;
			srcType = DGLES2_TYPE_ENUM;
			break;

		case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
			ctx->hgl.GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, i);
			i[0] /= 4;
			srcType = DGLES2_TYPE_INT;
			break;

		case GL_MAX_VERTEX_UNIFORM_VECTORS:
			ctx->hgl.GetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, i);
			i[0] /= 4;
			srcType = DGLES2_TYPE_INT;
			break;

		case GL_MAX_VARYING_VECTORS:
			ctx->hgl.GetIntegerv(GL_MAX_VARYING_FLOATS, i);
			i[0] /= 4;
			srcType = DGLES2_TYPE_INT;
			break;

		default:
			// Let the host GL handle this case.
			return GL_FALSE;
	}

	// Type conversions.
	for(j = 0; j < n; j++)
	{
		switch(dstType)
		{
			case DGLES2_TYPE_FLOAT:
				switch(srcType)
				{
					case DGLES2_TYPE_BOOL:
						((GLfloat*)params)[j] = b[j] ? 1.f : 0.f;
						break;

					case DGLES2_TYPE_INT:
					case DGLES2_TYPE_ENUM:
						((GLfloat*)params)[j] = (GLfloat)i[j];
						break;

					default:
						DGLES2_ASSERT(GL_FALSE);
				}
				break;

			case DGLES2_TYPE_BOOL:
				switch(srcType)
				{
					case DGLES2_TYPE_BOOL:
						((GLboolean*)params)[j] = b[j];
						break;

					case DGLES2_TYPE_INT:
					case DGLES2_TYPE_ENUM:
						((GLboolean*)params)[j] = i[j] ? GL_TRUE : GL_FALSE;
						break;

					default:
						DGLES2_ASSERT(GL_FALSE);
				}
				break;

			case DGLES2_TYPE_INT:
				switch(srcType)
				{
					case DGLES2_TYPE_BOOL:
						((GLint*)params)[j] = b[j] ? 1 : 0;
						break;

					case DGLES2_TYPE_INT:
					case DGLES2_TYPE_ENUM:
						((GLint*)params)[j] = i[j];
						break;

					default:
						DGLES2_ASSERT(GL_FALSE);
				}
				break;

			default:
				DGLES2_ASSERT(GL_FALSE);
		}
	}

	return GL_TRUE;
}

GL_APICALL_BUILD void GL_APIENTRY glGetBooleanv(GLenum pname, GLboolean* params)
{
	DGLES2_ENTER();
	if(!dglGet(ctx, pname, DGLES2_TYPE_BOOL, params))
	{
		ctx->hgl.GetBooleanv(pname, params);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD GLenum GL_APIENTRY glGetError(void)
{
	DGLES2_ENTER_RET(GL_NO_ERROR);
	{
		GLenum host_error = ctx->hgl.GetError();
		GLenum wrapper_error = ctx->error;
		ctx->error = GL_NO_ERROR;
		DGLES2_LEAVE_RET(host_error != GL_NO_ERROR ? host_error : wrapper_error);
	}
}

GL_APICALL_BUILD void GL_APIENTRY glGetFloatv(GLenum pname, GLfloat* params)
{
	DGLES2_ENTER();
	if(!dglGet(ctx, pname, DGLES2_TYPE_FLOAT, params))
	{
		ctx->hgl.GetFloatv(pname, params);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetIntegerv(GLenum pname, GLint* params)
{
	DGLES2_ENTER();
	if(!dglGet(ctx, pname, DGLES2_TYPE_INT, params))
	{
		ctx->hgl.GetIntegerv(pname, params);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD const GLubyte* GL_APIENTRY glGetString(GLenum name)
{
	DGLES2_ENTER_RET(NULL);
	{
		static const GLubyte vendor[] = "Nokia";
		static const GLubyte renderer[] = "OpenGL ES 2.0";
		static const GLubyte version[] = "OpenGL ES 2.0";
		static const GLubyte slversion[] = "OpenGL ES GLSL ES 1.0";
		static const GLubyte extensions[] = "GL_OES_depth24 GL_OES_depth32 GL_OES_rgb8_rgba8 "
											"GL_OES_vertex_half_float GL_OES_texture_half_float "
											"GL_OES_texture_half_float_linear GL_OES_texture_npot "
											"GL_OES_compressed_paletted_texture "
											"GL_OES_compressed_ETC1_RGB8_texture "
											"GL_OES_EGL_image";
		const GLubyte* str = NULL;
		switch(name)
		{
		case GL_VENDOR:
			str = vendor;
			break;
		case GL_RENDERER:
			str = renderer;
			break;
		case GL_VERSION:
			str = version;
			break;
		case GL_SHADING_LANGUAGE_VERSION:
			str = slversion;
			break;
		case GL_EXTENSIONS:
			str = extensions;
			break;
		default:
			DGLES2_ERROR_RET(GL_INVALID_ENUM, NULL);
			break;
		}
		DGLES2_LEAVE_RET(str);
	}
}
