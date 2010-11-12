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
#include "vertex.h"
#include "get.h"
#include "half.h"

#define DEBUG_VERTEX 0

GLboolean dglGetVertexAttrib(const DGLContext *ctx, GLuint index, GLenum pname, DGLType type, void* params)
{
	GLint i;

	switch(pname)
	{
		case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
			i = ctx->vertex_arrays[index].enabled;
			break;

		case GL_VERTEX_ATTRIB_ARRAY_SIZE:
			i = ctx->vertex_arrays[index].size;
			break;

		case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
			i = ctx->vertex_arrays[index].stride;
			break;

		case GL_VERTEX_ATTRIB_ARRAY_TYPE:
			i = ctx->vertex_arrays[index].type;
			break;

		case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
			i = ctx->vertex_arrays[index].normalized;
			break;

		case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
			i = ctx->vertex_arrays[index].buffer != NULL ? ctx->vertex_arrays[index].buffer->obj.name : 0;
			break;

		case GL_CURRENT_VERTEX_ATTRIB:
			if(index == 0)
			{
				// Attribute zero can't be set in desktop GL.
				if(type == DGLES2_TYPE_INT)
				{
					((GLint*)params)[0] = (GLint)ctx->attrib_zero[0];
					((GLint*)params)[1] = (GLint)ctx->attrib_zero[1];
					((GLint*)params)[2] = (GLint)ctx->attrib_zero[2];
					((GLint*)params)[3] = (GLint)ctx->attrib_zero[3];
				}
				else
				{
					DGLES2_ASSERT(type == DGLES2_TYPE_FLOAT);
					((GLfloat*)params)[0] = ctx->attrib_zero[0];
					((GLfloat*)params)[1] = ctx->attrib_zero[1];
					((GLfloat*)params)[2] = ctx->attrib_zero[2];
					((GLfloat*)params)[3] = ctx->attrib_zero[3];
				}
			}
			else
			{
				if(type == DGLES2_TYPE_INT)
				{
					ctx->hgl.GetVertexAttribiv(index, GL_CURRENT_VERTEX_ATTRIB, (GLint*)params);
				}
				else
				{
					DGLES2_ASSERT(type == DGLES2_TYPE_FLOAT);
					ctx->hgl.GetVertexAttribfv(index, GL_CURRENT_VERTEX_ATTRIB, (GLfloat*)params);
				}
			}
			return GL_TRUE;

		default:
			return GL_FALSE;
	}

	if(type == DGLES2_TYPE_INT)
	{
		*(GLint*)params = i;
	}
	else
	{
		DGLES2_ASSERT(type == DGLES2_TYPE_FLOAT);
		*(GLfloat*)params = (GLfloat)i;
	}

	return GL_TRUE;
}

GL_APICALL_BUILD void GL_APIENTRY glDisableVertexAttribArray(GLuint index)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(index >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
#	if(DEBUG_VERTEX == 1)
	Dprintf("Disabling array %d\n", index);
#	endif
	ctx->vertex_arrays[index].enabled = GL_FALSE;
	ctx->hgl.DisableVertexAttribArray(index);
	DGLES2_LEAVE();
}

// Function to convert fixed and half float arrays to float arrays.
static void dglConvertArrays(DGLContext* ctx, GLint first, GLsizei count)
{
	unsigned int i;
	for(i = 0; i < ctx->max_vertex_attribs; ++i)
	{
		DGLVertexArray* va = ctx->vertex_arrays + i;
		if(!va->enabled)
		{
			continue;
		}
		{
			if(va->type == GL_FIXED || va->type == GL_HALF_FLOAT_OES)
			{
				const void* ptr = va->buffer != NULL ? va->buffer->data : va->ptr;

				GLsizei stride;
				if(va->stride)
				{
					stride = va->stride;
				}
				else if(va->type == GL_FIXED)
				{
					stride = va->size * sizeof(GLfixed);
				}
				else
				{
					DGLES2_ASSERT(va->type == GL_HALF_FLOAT_OES);
					stride = va->size * sizeof(khronos_int16_t);
				}

	#			if(DEBUG_VERTEX == 1)
				Dprintf("Converting %d fixed/half float indices to float.\n", count);
	#			endif

				if(va->floatptr) free(va->floatptr);

				va->floatptr = (GLfloat*)malloc(sizeof(GLfloat)*count*va->size);
				{
					GLsizei j;
					for(j = 0; j < count; ++j)
					{
						signed k;
						for(k = 0; k < va->size; ++k)
						{
							if(va->type == GL_FIXED)
							{
								va->floatptr[j*va->size + k] = ((GLfixed*)(((char*)ptr) + stride*(first + j)))[k] / 65536.0f;
							}
							else
							{
								khronos_int16_t half;
								DGLES2_ASSERT(va->type == GL_HALF_FLOAT_OES);
								half = ((khronos_int16_t*)(((char*)ptr) + stride*(first + j)))[k];
								va->floatptr[j*va->size + k] = dglConvertHalfToFloat(half);
							}
						}
					}
				}
				ctx->hgl.VertexAttribPointer(i, va->size, GL_FLOAT, va->normalized, 0, va->floatptr - first);
			}
		}
	}
}

GL_APICALL_BUILD void GL_APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(first < 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(count < 0, GL_INVALID_VALUE);
	dglConvertArrays(ctx, first, count);
	DGLES2_BEGIN_DRAWING();
	ctx->hgl.DrawArrays(mode, first, count);
	DGLES2_END_DRAWING();
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(count < 0, GL_INVALID_VALUE);
	{
		GLsizei indice_size;
		switch(type)
		{
			case GL_UNSIGNED_BYTE: indice_size = sizeof(GLubyte); break;
			case GL_UNSIGNED_SHORT: indice_size = sizeof(GLushort); break;
			default:
			{
				DGLES2_ERROR(GL_INVALID_ENUM);
			}
		}
		dglConvertArrays(ctx, (GLint)((size_t)indices/indice_size), count);
		DGLES2_BEGIN_DRAWING();
		ctx->hgl.DrawElements(mode, count, type, indices);
		DGLES2_END_DRAWING();
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glEnableVertexAttribArray(GLuint index)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(index >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
#	if(DEBUG_VERTEX == 1)
	Dprintf("Enabling array %d\n", index);
#	endif
	ctx->vertex_arrays[index].enabled = GL_TRUE;
	ctx->hgl.EnableVertexAttribArray(index);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(index >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(!dglGetVertexAttrib(ctx, index, pname, DGLES2_TYPE_FLOAT, params))
	{
		DGLES2_ERROR(GL_INVALID_ENUM);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(index >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(!dglGetVertexAttrib(ctx, index, pname, DGLES2_TYPE_INT, params))
	{
		DGLES2_ERROR(GL_INVALID_ENUM);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(index >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(pname != GL_VERTEX_ATTRIB_ARRAY_POINTER, GL_INVALID_ENUM);
	*pointer = (void*)ctx->vertex_arrays[index].ptr;
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glVertexAttrib1f(GLuint indx, GLfloat x)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(indx >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(indx == 0)
	{
		// Attribute zero can't be set in desktop GL.
		ctx->attrib_zero[0] = x;
	}
	else
	{
		ctx->hgl.VertexAttrib1f(indx, x);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glVertexAttrib1fv(GLuint indx, const GLfloat* values)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(indx >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(indx == 0)
	{
		// Attribute zero can't be set in desktop GL.
		ctx->attrib_zero[0] = values[0];
	}
	else
	{
		ctx->hgl.VertexAttrib1fv(indx, values);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glVertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(indx >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(indx == 0)
	{
		// Attribute zero can't be set in desktop GL.
		ctx->attrib_zero[0] = x;
		ctx->attrib_zero[1] = y;
	}
	else
	{
		ctx->hgl.VertexAttrib2f(indx, x, y);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glVertexAttrib2fv(GLuint indx, const GLfloat* values)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(indx >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(indx == 0)
	{
		// Attribute zero can't be set in desktop GL.
		ctx->attrib_zero[0] = values[0];
	}
	else
	{
		ctx->hgl.VertexAttrib2fv(indx, values);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glVertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(indx >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(indx == 0)
	{
		// Attribute zero can't be set in desktop GL.
		ctx->attrib_zero[0] = x;
		ctx->attrib_zero[1] = y;
		ctx->attrib_zero[2] = z;
	}
	else
	{
		ctx->hgl.VertexAttrib3f(indx, x, y, z);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glVertexAttrib3fv(GLuint indx, const GLfloat* values)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(indx >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(indx == 0)
	{
		// Attribute zero can't be set in desktop GL.
		ctx->attrib_zero[0] = values[0];
		ctx->attrib_zero[1] = values[1];
		ctx->attrib_zero[2] = values[2];
	}
	else
	{
		ctx->hgl.VertexAttrib3fv(indx, values);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glVertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(indx >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(indx == 0)
	{
		// Attribute zero can't be set in desktop GL.
		ctx->attrib_zero[0] = x;
		ctx->attrib_zero[1] = y;
		ctx->attrib_zero[2] = z;
		ctx->attrib_zero[3] = w;
	}
	else
	{
		ctx->hgl.VertexAttrib4f(indx, x, y, z, w);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glVertexAttrib4fv(GLuint indx, const GLfloat* values)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(indx >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	if(indx == 0)
	{
		// Attribute zero can't be set in desktop GL.
		ctx->attrib_zero[0] = values[0];
		ctx->attrib_zero[1] = values[1];
		ctx->attrib_zero[2] = values[2];
		ctx->attrib_zero[3] = values[3];
	}
	else
	{
		ctx->hgl.VertexAttrib4fv(indx, values);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(indx >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(size < 1 || size > 4, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(type != GL_BYTE &&
					type != GL_UNSIGNED_BYTE &&
					type != GL_SHORT &&
					type != GL_UNSIGNED_SHORT &&
					type != GL_FIXED &&
					type != GL_FLOAT &&
					type != GL_HALF_FLOAT_OES, 
					GL_INVALID_ENUM);
	DGLES2_ERROR_IF(stride < 0, GL_INVALID_VALUE);
#	if(DEBUG_VERTEX == 1)
	Dprintf("Array %d at %p (%d elements every %d bytes)\n", indx, ptr, size, stride);
#	endif

	{
		DGLVertexArray* va = ctx->vertex_arrays + indx;

		if(type != GL_FIXED && type != GL_HALF_FLOAT_OES)
		{
			ctx->hgl.VertexAttribPointer(indx, size, type, normalized, stride, ptr);
		}

		if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
		{
			va->size = size;
			va->type = type;
			va->normalized = normalized;
			va->stride = stride;
			va->ptr = ptr;
			
			va->buffer = DGLContext_findBuffer(ctx, ctx->buffer_binding);
		}
	}
	DGLES2_LEAVE();
}
