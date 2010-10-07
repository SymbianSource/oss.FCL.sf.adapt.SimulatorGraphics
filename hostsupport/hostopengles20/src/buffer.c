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

DGLBuffer* DGLBuffer_create(GLuint name)
{
	DGLBuffer* buffer = malloc(sizeof(DGLBuffer));
	if(buffer == NULL)
	{
		return NULL;
	}

	buffer->obj.name = name;
	buffer->obj.next = NULL;

	buffer->data = NULL;
	buffer->size = 0;
	buffer->usage = GL_STATIC_DRAW;

	return buffer;
}

void DGLBuffer_destroy(DGLBuffer *buffer)
{
	DGLES2_ASSERT(buffer != NULL);
	if(buffer->data != NULL)
	{
		free(buffer->data);
		buffer->data = NULL;
	}
	free(buffer);
}

GL_APICALL_BUILD void GL_APIENTRY glBindBuffer(GLenum target, GLuint buffer)
{
	DGLES2_ENTER();

	DGLContext_getHostError(ctx);
	
	// The buffers are stored in both the wrapper and host.
	ctx->hgl.BindBuffer(target, buffer);

	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		if(target == GL_ARRAY_BUFFER)
		{
			if(!DGLContext_bindBuffer(ctx, buffer))
			{
				DGLES2_ERROR(GL_OUT_OF_MEMORY);
			}
		}
		else
		{
			DGLES2_ERROR_IF(target != GL_ELEMENT_ARRAY_BUFFER, GL_INVALID_ENUM);
		}
	}

	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(size < 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(usage != GL_STATIC_DRAW && usage != GL_DYNAMIC_DRAW && usage != GL_STREAM_DRAW, GL_INVALID_ENUM);
	
	DGLContext_getHostError(ctx);

	// The buffers are stored in both the wrapper and host.
	ctx->hgl.BufferData(target, size, data, usage);

	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		if(target == GL_ARRAY_BUFFER)
		{
			DGLBuffer* buffer;

			DGLES2_ERROR_IF(ctx->buffer_binding == 0, GL_INVALID_OPERATION);
			
			buffer = DGLContext_findBuffer(ctx, ctx->buffer_binding);
			DGLES2_ASSERT(buffer != NULL);
			
			if(buffer->data != NULL)
			{
				// Delete old data.
				free(buffer->data);
			}

			buffer->data = malloc(size);
			if(buffer->data == NULL)
			{
				DGLES2_ERROR(GL_OUT_OF_MEMORY);
			}

			if(data != NULL)
			{
				// Copy new data.
				memcpy(buffer->data, data, size);
			}

			buffer->size = size;
			buffer->usage = usage;
		}
		else
		{
			DGLES2_ERROR_IF(target != GL_ELEMENT_ARRAY_BUFFER, GL_INVALID_ENUM);
		}
	}

	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(offset < 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(size < 0, GL_INVALID_VALUE);
	
	DGLContext_getHostError(ctx);

	// The buffers are stored in both the wrapper and host.
	ctx->hgl.BufferSubData(target, offset, size, data);

	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		if(target == GL_ARRAY_BUFFER)
		{
			DGLBuffer* buffer;

			DGLES2_ERROR_IF(target != GL_ARRAY_BUFFER, GL_INVALID_ENUM);
			DGLES2_ERROR_IF(ctx->buffer_binding == 0, GL_INVALID_OPERATION);

			buffer = DGLContext_findBuffer(ctx, ctx->buffer_binding);
			DGLES2_ASSERT(buffer != NULL);

			memcpy((char*)buffer->data + offset, data, size);
		}
		else
		{
			DGLES2_ERROR_IF(target != GL_ELEMENT_ARRAY_BUFFER, GL_INVALID_ENUM);
		}
	}

	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(n < 0, GL_INVALID_VALUE);
	
	DGLContext_getHostError(ctx);

	// The buffers are stored in both the wrapper and host.
	ctx->hgl.DeleteBuffers(n, buffers);

	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		int i;
		for(i = 0; i < n; i++)
		{
			DGLContext_destroyBuffer(ctx, buffers[i]);
		}
	}

	DGLES2_LEAVE();
}


GL_APICALL_BUILD void GL_APIENTRY glGenBuffers(GLsizei n, GLuint* buffers)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(n < 0, GL_INVALID_VALUE);

	// The buffers are stored in both the wrapper and host.
	// Let the host come up with free buffer names. 
	ctx->hgl.GenBuffers(n, buffers);

	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(pname != GL_BUFFER_USAGE && pname != GL_BUFFER_SIZE, GL_INVALID_ENUM);
	ctx->hgl.GetBufferParameteriv(target, pname, params);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD GLboolean GL_APIENTRY glIsBuffer(GLuint buffer)
{
	DGLES2_ENTER_RET(GL_FALSE);
	DGLES2_LEAVE_RET(ctx->hgl.IsBuffer(buffer));
}
