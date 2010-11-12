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

#ifdef __cplusplus
extern "C"
{
GL_APICALL void GL_APIENTRY glEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image);
}
#endif

DGLRenderbuffer* DGLRenderbuffer_create(GLuint name)
{
	DGLRenderbuffer* buffer = (DGLRenderbuffer*)malloc(sizeof(DGLRenderbuffer));
	if(buffer == NULL)
	{
		return NULL;
	}

	buffer->obj.name = name;
	buffer->obj.next = NULL;

	buffer->egl_image = NULL;

	return buffer;
}

void DGLRenderbuffer_destroy(DGLRenderbuffer *buffer)
{
	DGLES2_ASSERT(buffer != NULL);
	if(buffer->egl_image != NULL)
	{
		deglUnregisterImageTarget(buffer->egl_image, GL_RENDERBUFFER, buffer->obj.name);
		buffer->egl_image = NULL;
	}
	free(buffer);
}

GL_APICALL_BUILD void GL_APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	DGLES2_ENTER();
	DGLContext_getHostError(ctx);
	ctx->hgl.BindFramebufferEXT(target, framebuffer);
	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		ctx->framebuffer_binding = framebuffer;
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
	DGLES2_ENTER();
	DGLContext_getHostError(ctx);
	ctx->hgl.BindRenderbufferEXT(target, renderbuffer);
	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		if(!DGLContext_bindRenderbuffer(ctx, renderbuffer))
		{
			DGLES2_ERROR(GL_OUT_OF_MEMORY);
		}
	}
	DGLES2_LEAVE_NO_ERROR_CHECK();
}

GL_APICALL_BUILD GLenum GL_APIENTRY glCheckFramebufferStatus(GLenum target)
{
	DGLES2_ENTER_RET(GL_FALSE);
	DGLES2_LEAVE_RET(ctx->hgl.CheckFramebufferStatusEXT(target));
}

GL_APICALL_BUILD void GL_APIENTRY glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
	DGLES2_ENTER();
	DGLContext_getHostError(ctx);
	ctx->hgl.DeleteFramebuffersEXT(n, framebuffers);
	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		// Unbind framebuffer when deleted.
		int i;
		for(i = 0; i < n; i++)
		{
			if(ctx->framebuffer_binding == framebuffers[i])
			{
				ctx->framebuffer_binding = 0;
				break;
			}
		}
	}
	DGLES2_LEAVE_NO_ERROR_CHECK();
}

GL_APICALL_BUILD void GL_APIENTRY glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
	DGLES2_ENTER();
	DGLContext_getHostError(ctx);
	ctx->hgl.DeleteRenderbuffersEXT(n, renderbuffers);
	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		int i;
		for(i = 0; i < n; i++)
		{
			DGLContext_destroyRenderbuffer(ctx, renderbuffers[n]);
		}
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(attachment != GL_COLOR_ATTACHMENT0 &&
					attachment != GL_DEPTH_ATTACHMENT &&
					attachment != GL_STENCIL_ATTACHMENT,
					GL_INVALID_ENUM);
	ctx->hgl.FramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(attachment != GL_COLOR_ATTACHMENT0 &&
					attachment != GL_DEPTH_ATTACHMENT &&
					attachment != GL_STENCIL_ATTACHMENT,
					GL_INVALID_ENUM);
	ctx->hgl.FramebufferTexture2DEXT(target, attachment, textarget, texture, level);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGenFramebuffers(GLsizei n, GLuint* framebuffers)
{
	DGLES2_ENTER();
	ctx->hgl.GenFramebuffersEXT(n, framebuffers);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
	DGLES2_ENTER();
	ctx->hgl.GenRenderbuffersEXT(n, renderbuffers);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(attachment != GL_COLOR_ATTACHMENT0 &&
					attachment != GL_DEPTH_ATTACHMENT &&
					attachment != GL_STENCIL_ATTACHMENT,
					GL_INVALID_ENUM);
	DGLES2_ERROR_IF(pname != GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE &&
					pname != GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME &&
					pname != GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL &&
					pname != GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE,
					GL_INVALID_ENUM);
	ctx->hgl.GetFramebufferAttachmentParameterivEXT(target, attachment, pname, params);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	DGLES2_ENTER();
	ctx->hgl.GetRenderbufferParameterivEXT(target, pname, params);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD GLboolean GL_APIENTRY glIsFramebuffer(GLuint framebuffer)
{
	DGLES2_ENTER_RET(GL_FALSE);
	ctx->hgl.IsFramebufferEXT(framebuffer);
	DGLES2_LEAVE_RET(GL_FALSE);
}

GL_APICALL_BUILD GLboolean    GL_APIENTRY glIsRenderbuffer(GLuint renderbuffer)
{
	DGLES2_ENTER_RET(GL_FALSE);
	ctx->hgl.IsRenderbufferEXT(renderbuffer);
	DGLES2_LEAVE_RET(GL_FALSE);
}

GL_APICALL_BUILD void GL_APIENTRY glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	DGLES2_ENTER();
	
	switch(internalformat)
	{
		case GL_DEPTH_COMPONENT16:
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_STENCIL_INDEX8:
			break;

		case GL_RGB565:	// RGB565 is not supported in desktop GL.
			internalformat = GL_RGB8;
			break;

		case GL_DEPTH_COMPONENT24_OES: // extension GL_OES_depth24
			internalformat = GL_DEPTH_COMPONENT24;
			break;

		case GL_DEPTH_COMPONENT32_OES: // extension GL_OES_depth32
			internalformat = GL_DEPTH_COMPONENT32;
			break;

		case GL_RGB8_OES: // extension GL_OES_rgb8_rgba8
			internalformat = GL_RGB8;
			break;

		case GL_RGBA8_OES: // extension GL_OES_rgb8_rgba8
			internalformat = GL_RGBA8;
			break;
	}
	
	DGLContext_getHostError(ctx);
	ctx->hgl.RenderbufferStorageEXT(target, internalformat, width, height);
	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		DGLRenderbuffer* buffer = DGLContext_findRenderbuffer(ctx, ctx->renderbuffer_binding);
		DGLES2_ASSERT(buffer != NULL);
		if(buffer->egl_image != NULL)
		{
			// Renderbuffer is respecified. It is no longer an EGLImage sibling.
			deglUnregisterImageTarget(buffer->egl_image, GL_RENDERBUFFER, buffer->obj.name);
			buffer->egl_image = NULL;
		}
	}

	DGLES2_LEAVE_NO_ERROR_CHECK();
}

GL_APICALL_BUILD void GL_APIENTRY glEGLImageTargetRenderbufferStorageOES(GLenum target, GLeglImageOES image)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_RENDERBUFFER, GL_INVALID_ENUM);
	DGLES2_ERROR_IF(image == NULL, GL_INVALID_OPERATION);
	DGLContext_getHostError(ctx);
	if(!DGLContext_specifyRenderbufferFromEGLImage(ctx, image))
	{
		DGLES2_ERROR(GL_INVALID_OPERATION);
	}
	DGLES2_LEAVE_NO_ERROR_CHECK();
}
