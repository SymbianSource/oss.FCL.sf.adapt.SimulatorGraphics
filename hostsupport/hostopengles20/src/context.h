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

#ifndef _GLESCONTEXT_H_
#define _GLESCONTEXT_H_

#include "platform.h"
#include "degl.h"
#include "vertex.h"
#include "buffer.h"
#include "shader.h"
#include "program.h"
#include "texture.h"
#include "fbo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DGLContext
{
    GLboolean	initialized;
    void*		native_context;
    HGL			hgl;	// Extension procedure addresses are context-specific on WGL.

    unsigned int max_vertex_attribs;
    int max_texture_level;

    GLenum error;
    GLuint buffer_binding;
    GLuint texture_binding_2d;
    GLuint texture_binding_cube_map;
	GLuint framebuffer_binding;
	GLuint renderbuffer_binding;

    GLfloat attrib_zero[4];	// Attribute zero can't be set in desktop GL.

    DGLVertexArray*		vertex_arrays;
    DGLBuffer*			buffers;
    DGLShader*			shaders;
    DGLProgram*			programs;
    DGLTexture*			textures;
	DGLRenderbuffer*	renderbuffers;
} DGLContext;

DGLContext* DGLContext_create(void* native_context);
GLboolean	DGLContext_initialize(DGLContext* ctx);
void		DGLContext_destroy(DGLContext* ctx);

void		DGLContext_setError(DGLContext* ctx, GLenum error);
GLenum		DGLContext_getHostError(DGLContext* ctx);

GLboolean	DGLContext_createBuffer(DGLContext* ctx, GLuint buffer);
void		DGLContext_destroyBuffer(DGLContext* ctx, GLuint buffer);
DGLBuffer*	DGLContext_findBuffer(DGLContext* ctx, GLuint buffer);
GLboolean	DGLContext_bindBuffer(DGLContext* ctx, GLuint buffer);

GLboolean	DGLContext_createShader(DGLContext* ctx, GLuint shader);
void		DGLContext_destroyShader(DGLContext* ctx, GLuint shader);
DGLShader*	DGLContext_findShader(DGLContext* ctx, GLuint shader);
GLboolean	DGLContext_setShaderSource(DGLContext* ctx, GLuint shader, const char* source, int length);

GLboolean	DGLContext_createProgram(DGLContext* ctx, GLuint program);
void		DGLContext_destroyProgram(DGLContext* ctx, GLuint program);
DGLProgram*	DGLContext_findProgram(DGLContext* ctx, GLuint program);

GLboolean			DGLContext_createTexture(DGLContext* ctx, GLuint texture, DGLTextureType type);
void				DGLContext_destroyTexture(DGLContext* ctx, GLuint texture);
DGLTexture*			DGLContext_findTexture(DGLContext* ctx, GLuint texture);
GLboolean			DGLContext_bindTexture(DGLContext* ctx, GLenum target, GLuint name);
DGLTexture*			DGLContext_getTexture(DGLContext* ctx, GLenum target);
DGLTextureLevel*	DGLContext_getTextureLevel(DGLContext* ctx, GLenum target, GLint level);
GLboolean			DGLContext_specifyTextureFromEGLImage(DGLContext* ctx, GLeglImageOES image, GLenum target);

GLboolean			DGLContext_createRenderbuffer(DGLContext* ctx, GLuint buffer);
void				DGLContext_destroyRenderbuffer(DGLContext* ctx, GLuint buffer);
DGLRenderbuffer*	DGLContext_getColorRenderbuffer(DGLContext* ctx);
DGLRenderbuffer*	DGLContext_findRenderbuffer(DGLContext* ctx, GLuint buffer);
GLboolean			DGLContext_bindRenderbuffer(DGLContext* ctx, GLuint buffer);
GLboolean			DGLContext_specifyRenderbufferFromEGLImage(DGLContext* ctx, GLeglImageOES image);

void	DGLContext_updateFBOAttachmentSiblings(DGLContext* ctx);

#ifdef __cplusplus
}
#endif

#endif // _GLESCONTEXT_H_
