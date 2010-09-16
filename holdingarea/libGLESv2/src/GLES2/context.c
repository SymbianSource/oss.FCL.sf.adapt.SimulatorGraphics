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

#include "context.h"
#include "util.h"

DGLContext* DGLContext_create(void* native_context)
{
    DGLContext* ctx = malloc(sizeof(DGLContext));
    if(ctx == NULL)
    {
        return NULL;
    }

    ctx->native_context = native_context;
    ctx->initialized = GL_FALSE;

    return ctx;
}

GLboolean DGLContext_initialize(DGLContext* ctx)
{
    GLint temp;

    DGLES2_ASSERT(ctx != NULL);
    DGLES2_ASSERT(!ctx->initialized);

    hglLoad(&ctx->hgl);

    ctx->buffers = NULL;
    ctx->shaders = NULL;
    ctx->programs = NULL;
    ctx->textures = NULL;
	ctx->renderbuffers = NULL;

    ctx->hgl.GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &temp);
    ctx->max_vertex_attribs = temp;

    ctx->vertex_arrays = malloc(ctx->max_vertex_attribs * sizeof(DGLVertexArray));
    if(ctx->vertex_arrays == NULL)
    {
        return GL_FALSE;
    }

    {
        int max_texture_size;
        ctx->hgl.GetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
        ctx->max_texture_level = dglLog2(max_texture_size);
    }

    // Initialize state.
    ctx->error = GL_NO_ERROR;
    ctx->buffer_binding = 0;
	ctx->framebuffer_binding = 0;
	ctx->renderbuffer_binding = 0;

    DGLContext_bindTexture(ctx, GL_TEXTURE_2D, 0);
    DGLContext_bindTexture(ctx, GL_TEXTURE_CUBE_MAP, 0);

    ctx->attrib_zero[0] = 0;
    ctx->attrib_zero[1] = 0;
    ctx->attrib_zero[2] = 0;
    ctx->attrib_zero[3] = 1;

    // Initialize vertex arrays.
    {
        unsigned int i;
        for(i = 0; i < ctx->max_vertex_attribs; i++)
        {
            DGLVertexArray* va = &ctx->vertex_arrays[i];

            va->size = 4;
            va->type = GL_FLOAT;
            va->normalized = 0;
            va->stride = 0;
            va->ptr = NULL;

            va->enabled = 0;
            va->floatptr = NULL;
            va->buffer = NULL;
        }
    }

    ctx->initialized = GL_TRUE;

    return GL_TRUE;
}

void DGLContext_destroy(DGLContext *ctx)
{
    DGLES2_ASSERT(ctx != NULL);

    if(ctx->vertex_arrays != NULL)
    {
        unsigned int i;
        for(i = 0; i < ctx->max_vertex_attribs; i++)
        {
            if(ctx->vertex_arrays[i].floatptr != NULL)
            {
                free(ctx->vertex_arrays[i].floatptr);
                ctx->vertex_arrays[i].floatptr = NULL;
            }
        }
        free(ctx->vertex_arrays);
        ctx->vertex_arrays = NULL;
    }

    // Destroy buffers
    {
        DGLBuffer* buffer = ctx->buffers;
        DGLBuffer* next;
        while(buffer != NULL)
        {
            next = (DGLBuffer*)buffer->obj.next;
            DGLBuffer_destroy(buffer);
            buffer = next;
        }
        ctx->buffers = NULL;
    }

    // Destroy shaders
    {
        DGLShader* shader = ctx->shaders;
        DGLShader* next;
        while(shader != NULL)
        {
            next = (DGLShader*)shader->obj.next;
            DGLShader_destroy(shader);
            shader = next;
        }
        ctx->shaders = NULL;
    }

    // Destroy programs
    {
        DGLProgram* program = ctx->programs;
        DGLProgram* next;
        while(program != NULL)
        {
            next = (DGLProgram*)program->obj.next;
            DGLProgram_destroy(program);
            program = next;
        }
        ctx->programs = NULL;
    }

    // Destroy textures
    {
        DGLTexture* texture = ctx->textures;
        DGLTexture* next;
        while(texture != NULL)
        {
            next = (DGLTexture*)texture->obj.next;
            DGLTexture_destroy(texture);
            texture = next;
        }
        ctx->textures = NULL;
    }

	// Destroy renderbuffers
    {
        DGLRenderbuffer* buffer = ctx->renderbuffers;
        DGLRenderbuffer* next;
        while(buffer != NULL)
        {
            next = (DGLRenderbuffer*)buffer->obj.next;
            DGLRenderbuffer_destroy(buffer);
            buffer = next;
        }
        ctx->renderbuffers = NULL;
    }

    free(ctx);
}

void DGLContext_loadGL(DGLContext* ctx)
{
    DGLES2_ASSERT(ctx != NULL);
    hglLoad(&ctx->hgl);
}

void DGLContext_setError(DGLContext* ctx, GLenum error)
{
    DGLES2_ASSERT(ctx != NULL);
    ctx->error = error;
}

/* This function should be called before calling state-changing
   host GL functions to clear the host GL error flag. After calling
   the host GL functions, this function should be called again
   to verify the success of the state change. Only then should
   the wrapper state be modified to reflect the changes made
   in the host state. */
GLenum DGLContext_getHostError(DGLContext* ctx)
{
    DGLES2_ASSERT(ctx != NULL);
    {
        GLenum host_error = ctx->hgl.GetError();
        if(host_error != GL_NO_ERROR)
        {
            DGLContext_setError(ctx, host_error);
        }
        return host_error;
    }
}

GLboolean DGLContext_createBuffer(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    DGLES2_ASSERT(DGLContext_findBuffer(ctx, name) == NULL);
    DGLES2_ASSERT(name != 0);

    {
        DGLBuffer* buffer = DGLBuffer_create(name);
        if(buffer == NULL)
        {
            return GL_FALSE;
        }
        DGLObject_insert((DGLObject**)&ctx->buffers, (DGLObject*)buffer);
    }

    return GL_TRUE;
}

void DGLContext_destroyBuffer(DGLContext *ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    if(name == 0)
    {
        return;
    }

    // Release buffer from the vertex arrays and context.
    {
        unsigned int i;
        for(i = 0; i < ctx->max_vertex_attribs; i++)
        {
            DGLVertexArray* va = &ctx->vertex_arrays[i];
            if(va->buffer != NULL && va->buffer->obj.name == name)
            {
                va->buffer = NULL;
            }
        }

        if(ctx->buffer_binding == name)
        {
            ctx->buffer_binding = 0;
        }
    }

    {
        DGLBuffer* removed = (DGLBuffer*)DGLObject_remove((DGLObject**)&ctx->buffers, name);
        if(removed != NULL)
        {
            DGLBuffer_destroy(removed);
        }
    }
}

DGLBuffer* DGLContext_findBuffer(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    return (DGLBuffer*)DGLObject_find((DGLObject*)ctx->buffers, name);
}

GLboolean DGLContext_bindBuffer(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    if(name != 0 && DGLContext_findBuffer(ctx, name) == NULL)
    {
        // A new buffer must be created.
        if(!DGLContext_createBuffer(ctx, name))
        {
            return GL_FALSE;
        }
    }
    ctx->buffer_binding = name;

    return GL_TRUE;
}

GLboolean DGLContext_createShader(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    DGLES2_ASSERT(DGLContext_findShader(ctx, name) == NULL);
    DGLES2_ASSERT(name != 0);

    {
        DGLShader* shader = DGLShader_create(name);
        if(shader == NULL)
        {
            return GL_FALSE;
        }
        DGLObject_insert((DGLObject**)&ctx->shaders, (DGLObject*)shader);
    }

    return GL_TRUE;
}

void DGLContext_destroyShader(DGLContext *ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    if(name == 0)
    {
        return;
    }

    {
        DGLShader* removed = (DGLShader*)DGLObject_remove((DGLObject**)&ctx->shaders, name);
        if(removed != NULL)
        {
            DGLShader_destroy(removed);
        }
    }
}

DGLShader* DGLContext_findShader(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    return (DGLShader*)DGLObject_find((DGLObject*)ctx->shaders, name);
}

GLboolean DGLContext_setShaderSource(DGLContext* ctx, GLuint name, const char* source, int length)
{
    DGLES2_ASSERT(ctx != NULL);
    {
        DGLShader* shader = DGLContext_findShader(ctx, name);
        DGLES2_ASSERT(shader != NULL);

        // Erase the previous source.
        if(shader->source != NULL)
        {
            free(shader->source);
            shader->source = NULL;
        }
        shader->length = 0;

        DGLES2_ASSERT(source != NULL);
        DGLES2_ASSERT(length >= 0);

        shader->source = malloc(length + 1);
        if(shader->source == NULL)
        {
            return GL_FALSE;
        }
        strncpy(shader->source, source, length);
        shader->source[length] = 0;
        shader->length = length;

        return GL_TRUE;
    }
}

GLboolean DGLContext_createProgram(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    DGLES2_ASSERT(DGLContext_findProgram(ctx, name) == NULL);
    DGLES2_ASSERT(name != 0);

    {
        DGLProgram* program = DGLProgram_create(name);
        if(program == NULL)
        {
            return GL_FALSE;
        }
        DGLObject_insert((DGLObject**)&ctx->programs, (DGLObject*)program);
    }

    return GL_TRUE;
}

void DGLContext_destroyProgram(DGLContext *ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    if(name == 0)
    {
        return;
    }

    {
        DGLProgram* removed = (DGLProgram*)DGLObject_remove((DGLObject**)&ctx->programs, name);
        if(removed != NULL)
        {
            DGLProgram_destroy(removed);
        }
    }
}

DGLProgram* DGLContext_findProgram(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    return (DGLProgram*)DGLObject_find((DGLObject*)ctx->programs, name);
}

GLboolean DGLContext_createTexture(DGLContext* ctx, GLuint name, DGLTextureType type)
{
    DGLES2_ASSERT(ctx != NULL);
    DGLES2_ASSERT(DGLContext_findTexture(ctx, name) == NULL);

    {
        DGLTexture* texture = DGLTexture_create(name, type, ctx->max_texture_level + 1);
        if(texture == NULL)
        {
            return GL_FALSE;
        }
        DGLObject_insert((DGLObject**)&ctx->textures, (DGLObject*)texture);
    }

    return GL_TRUE;
}

void DGLContext_destroyTexture(DGLContext *ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    if(name == 0)
    {
        // The texture named zero cannot be destroyed.
        return;
    }

    // Unbind texture.
    if(ctx->texture_binding_2d == name)
    {
        ctx->texture_binding_2d = 0;
    }
    if(ctx->texture_binding_cube_map == name)
    {
        ctx->texture_binding_cube_map = 0;
    }

    {
        DGLTexture* removed = (DGLTexture*)DGLObject_remove((DGLObject**)&ctx->textures, name);
        if(removed != NULL)
        {
            DGLTexture_destroy(removed);
        }
    }
}

DGLTexture* DGLContext_findTexture(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    return (DGLTexture*)DGLObject_find((DGLObject*)ctx->textures, name);
}

GLboolean DGLContext_bindTexture(DGLContext* ctx, GLenum target, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    if(target == GL_TEXTURE_2D)
    {
        if(DGLContext_findTexture(ctx, name) == NULL)
        {
            // A new texture must be created.
            if(!DGLContext_createTexture(ctx, name, DGLES2_TEXTURE_2D))
            {
                return GL_FALSE;
            }
        }
        ctx->texture_binding_2d = name;
    }
    else if(target == GL_TEXTURE_CUBE_MAP)
    {
        if(DGLContext_findTexture(ctx, name) == NULL)
        {
            // A new texture must be created.
            if(!DGLContext_createTexture(ctx, name, DGLES2_TEXTURE_CUBE_MAP))
            {
                return GL_FALSE;
            }
        }
        ctx->texture_binding_cube_map = name;
    }
    else
    {
        DGLES2_ASSERT(GL_FALSE);
    }

    return GL_TRUE;
}

GLuint DGLContext_getTextureBinding(const DGLContext* ctx, GLenum target)
{
    DGLES2_ASSERT(ctx != NULL);
    switch(target)
    {
        case GL_TEXTURE_2D:
            return ctx->texture_binding_2d;

        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            return ctx->texture_binding_cube_map;

        default:
            DGLES2_ASSERT(GL_FALSE);
    }

    // No reached.
    return 0;
}

DGLTexture* DGLContext_getTexture(DGLContext* ctx, GLenum target)
{
    DGLES2_ASSERT(ctx != NULL);
    {
        GLuint binding = DGLContext_getTextureBinding(ctx, target);
        return DGLContext_findTexture(ctx, binding);
    }
}

DGLTextureLevel* DGLContext_getTextureLevel(DGLContext* ctx, GLenum target, GLint level)
{
    DGLES2_ASSERT(ctx != NULL);
    {
        DGLTexture* texture = DGLContext_getTexture(ctx, target);
        DGLES2_ASSERT(texture != NULL);

        DGLES2_ASSERT(level < ctx->max_texture_level);
        return DGLTexture_getLevel(texture, target, level);
    }
}

GLboolean DGLContext_specifyTextureFromEGLImage(DGLContext* ctx, GLeglImageOES image, GLenum target)
{
	DGLES2_ASSERT(ctx != NULL);
	{
		DGLTexture* texture;
		GLenum internal_format;
		GLsizei width, height;
		GLsizei stride;
		GLenum data_format;
		GLenum data_type;
		void* data;
		GLenum error;

		DGLContext_getHostError(ctx);

		texture = DGLContext_getTexture(ctx, target);
		DGLES2_ASSERT(texture != NULL);

		deglGetImageInfo(image, &internal_format, &width, &height, &stride, &data_format, &data_type);
		data = deglGetImageData(image);
		
		ctx->hgl.TexImage2D(target, 0, internal_format, width, height, 0, data_format, data_type, data);

		error = ctx->hgl.GetError();
		if(error == GL_NO_ERROR)
		{
			DGLTexture_setLevel(texture, target, 0, internal_format, width, height);
			DGLTexture_setEGLImage(texture, target, image);
			deglRegisterImageTarget(image, target, texture->obj.name);
			return GL_TRUE;
		}
		else
		{
			return GL_FALSE;
		}
	}
}

GLboolean DGLContext_createRenderbuffer(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    DGLES2_ASSERT(DGLContext_findRenderbuffer(ctx, name) == NULL);
    DGLES2_ASSERT(name != 0);

    {
        DGLRenderbuffer* buffer = DGLRenderbuffer_create(name);
        if(buffer == NULL)
        {
            return GL_FALSE;
        }
        DGLObject_insert((DGLObject**)&ctx->renderbuffers, (DGLObject*)buffer);
    }

    return GL_TRUE;
}

void DGLContext_destroyRenderbuffer(DGLContext *ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    if(name == 0)
    {
        return;
    }

    {
        DGLRenderbuffer* removed = (DGLRenderbuffer*)DGLObject_remove((DGLObject**)&ctx->renderbuffers, name);
        if(removed != NULL)
        {
            DGLRenderbuffer_destroy(removed);
        }
    }
}

DGLRenderbuffer* DGLContext_getColorRenderbuffer(DGLContext* ctx)
{
	DGLES2_ASSERT(ctx != NULL);
	
	if(ctx->framebuffer_binding == 0)
	{
		return NULL;
	}

	{
		GLint type;
		GLint name;
		DGLRenderbuffer* buffer;

		ctx->hgl.GetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type);

		if(type != GL_RENDERBUFFER)
		{
			return NULL;
		}

		ctx->hgl.GetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &name);
		buffer = DGLContext_findRenderbuffer(ctx, name);
		DGLES2_ASSERT(buffer != NULL);
		return buffer;
	}
}

DGLRenderbuffer* DGLContext_findRenderbuffer(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    return (DGLRenderbuffer*)DGLObject_find((DGLObject*)ctx->renderbuffers, name);
}

GLboolean DGLContext_bindRenderbuffer(DGLContext* ctx, GLuint name)
{
    DGLES2_ASSERT(ctx != NULL);
    if(name != 0 && DGLContext_findRenderbuffer(ctx, name) == NULL)
    {
        // A new renderbuffer must be created.
        if(!DGLContext_createRenderbuffer(ctx, name))
        {
            return GL_FALSE;
        }
    }
    ctx->renderbuffer_binding = name;

    return GL_TRUE;
}

GLboolean DGLContext_specifyRenderbufferFromEGLImage(DGLContext* ctx, GLeglImageOES image)
{
	GLenum imageFormat;
	GLenum storageFormat;
	GLsizei width, height;
	GLint max_renderbuffer_size;
	GLsizei stride;
	GLenum dataFormat;
	GLenum dataType;
	void* data;

	deglGetImageInfo(image, &imageFormat, &width, &height, &stride, &dataFormat, &dataType);

	ctx->hgl.GetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &max_renderbuffer_size);
	if(width > max_renderbuffer_size || width > max_renderbuffer_size)
	{
		return GL_FALSE;
	}
	
	data = deglGetImageData(image);

	// Map optimum format to available formats.
	switch(imageFormat)
	{
#ifndef DGLES2_NO_SRGB
		case GL_SRGB8:
		case GL_SLUMINANCE8:
#endif
		case GL_RGB8:
		case GL_LUMINANCE8:
			storageFormat = GL_RGB8;
			break;

#ifndef DGLES2_NO_SRGB
		case GL_SRGB8_ALPHA8:
#endif
		case GL_RGBA8:
		case GL_ALPHA8:
			storageFormat = GL_RGBA8;
			break;
			
		default:
			DGLES2_ASSERT(GL_FALSE);
	}
	
	{
		// Use a temporary FBO to set the renderbuffer contents.

		GLint renderbuffer_binding;
		GLint unpack_alignment;
		GLuint fbo;

		ctx->hgl.GetIntegerv(GL_RENDERBUFFER_BINDING, &renderbuffer_binding);
		ctx->hgl.GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);

		ctx->hgl.GenFramebuffersEXT(1, &fbo);
		ctx->hgl.BindFramebufferEXT(GL_FRAMEBUFFER, fbo);
		ctx->hgl.FramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer_binding);

		ctx->hgl.RenderbufferStorageEXT(GL_RENDERBUFFER, storageFormat, width, height);
		DGLES2_ASSERT(ctx->hgl.CheckFramebufferStatusEXT(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		ctx->hgl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
		ctx->hgl.DrawPixels(width, height, dataFormat, dataType, data);

		// Restore state.
		ctx->hgl.PixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);
		ctx->hgl.BindFramebufferEXT(GL_FRAMEBUFFER, ctx->framebuffer_binding);
		ctx->hgl.DeleteFramebuffersEXT(1, &fbo);
	}

	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		DGLRenderbuffer* buffer = DGLContext_findRenderbuffer(ctx, ctx->renderbuffer_binding);
		DGLES2_ASSERT(buffer != NULL);
		buffer->egl_image = image;
		deglRegisterImageTarget(image, GL_RENDERBUFFER, buffer->obj.name);
	}
	else
	{
		return GL_FALSE;
	}

	return GL_TRUE;
}

void DGLContext_updateFBOAttachmentSiblings(DGLContext* ctx)
{
	DGLES2_ASSERT(ctx != NULL);

	if(ctx->framebuffer_binding != 0)
	{
		GLenum type;
		GLuint name;

		ctx->hgl.GetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, (GLint*)&type);

		if(type == GL_RENDERBUFFER)
		{
			DGLRenderbuffer* buffer;
			ctx->hgl.GetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&name);
			buffer = DGLContext_findRenderbuffer(ctx, name);
			if(buffer != NULL && buffer->egl_image != NULL)
			{
				deglUpdateImageSiblings(buffer->egl_image, GL_RENDERBUFFER, buffer->obj.name);
			}
		}
		else if(type == GL_TEXTURE)
		{
			DGLTexture* texture;
			ctx->hgl.GetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&name);
			texture = DGLContext_findTexture(ctx, name);
			if(texture != NULL)
			{
				GLeglImageOES image;

				GLenum target = GL_TEXTURE_2D;
				if(texture->type == DGLES2_TEXTURE_CUBE_MAP)
				{
					ctx->hgl.GetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE, (GLint*)&target);
				}

				image = DGLTexture_getEGLImage(texture, target);
				if(image != NULL)
				{
					deglUpdateImageSiblings(image, target, texture->obj.name);
				}
			}
		}
	}
}
