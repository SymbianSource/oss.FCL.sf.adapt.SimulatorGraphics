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

#ifndef HGL_H_
#define HGL_H_

#include <GLES2/gl2.h>

// Things that desktop GL has, but GL ES doesn't.

#define GL_BGR                              0x80E0
#define GL_BGRA                             0x80E1
#define GL_RGB8                             0x8051
#define GL_RGBA8                            0x8058
#ifndef DGLES2_NO_SRGB
#define GL_SRGB8							0x8C41
#define GL_SRGB8_ALPHA8						0x8C43
#define GL_SLUMINANCE8						0x8C47
#else
// No sRGB support in OpenGL 2.0.
#define GL_SRGB8							GL_RGB8
#define GL_SRGB8_ALPHA8						GL_RGBA8
#define GL_SLUMINANCE8						GL_LUMINANCE8
#endif
#define GL_LUMINANCE8                       0x8040
#define GL_ALPHA8                           0x803C
#define GL_DEPTH_COMPONENT24				0x81A6
#define GL_DEPTH_COMPONENT32				0x81A7
#define GL_UNSIGNED_INT_8_8_8_8_REV         0x8367
#define GL_UNSIGNED_SHORT_1_5_5_5_REV       0x8366
#define GL_UNSIGNED_SHORT_4_4_4_4_REV       0x8365
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS	0x8B49
#define GL_MAX_VARYING_FLOATS				0x8B4B
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS	0x8B4A
#define GL_TEXTURE_WIDTH                    0x1000
#define GL_TEXTURE_HEIGHT					0x1001
#define GL_TEXTURE_RED_SIZE                 0x805C
#define GL_TEXTURE_GREEN_SIZE               0x805D
#define GL_TEXTURE_BLUE_SIZE                0x805E
#define GL_TEXTURE_ALPHA_SIZE               0x805F
#define GL_TEXTURE_LUMINANCE_SIZE           0x8060
#define GL_TEXTURE_INTERNAL_FORMAT          0x1003
typedef char GLchar;		/* native character */
typedef double GLclampd;    /* double precision float in [0,1] */

#define HGL_FUNCS \
	HGL_FUNC(GLenum, GetError, (void)) \
	HGL_FUNC(GLuint, CreateShader, (GLenum)) \
	HGL_FUNC(void, ShaderSource, (GLuint, GLsizei, const GLchar* *, const GLint *)) \
	HGL_FUNC(void, CompileShader, (GLuint)) \
	HGL_FUNC(void, GetShaderiv, (GLuint, GLenum, GLint *)) \
	HGL_FUNC(void, GetShaderInfoLog, (GLuint, GLsizei, GLsizei *, GLchar *)) \
	HGL_FUNC(GLuint, CreateProgram, (void)) \
	HGL_FUNC(void, AttachShader, (GLuint, GLuint)) \
	HGL_FUNC(void, BindAttribLocation, (GLuint, GLuint, const GLchar *)) \
	HGL_FUNC(void, LinkProgram, (GLuint)) \
	HGL_FUNC(void, UseProgram, (GLuint)) \
	HGL_FUNC(void, GetProgramiv, (GLuint, GLenum, GLint *)) \
	HGL_FUNC(void, GetProgramInfoLog, (GLuint, GLsizei, GLsizei *, GLchar *)) \
	HGL_FUNC(GLint, GetUniformLocation, (GLuint, const GLchar *)) \
	HGL_FUNC(void, Uniform1f, (GLint, GLfloat)) \
	HGL_FUNC(void, Uniform2f, (GLint, GLfloat, GLfloat)) \
	HGL_FUNC(void, Uniform3f, (GLint, GLfloat, GLfloat, GLfloat)) \
	HGL_FUNC(void, Uniform4f, (GLint, GLfloat, GLfloat, GLfloat, GLfloat)) \
	HGL_FUNC(void, Uniform1i, (GLint, GLint)) \
	HGL_FUNC(void, Uniform2i, (GLint, GLint, GLint)) \
	HGL_FUNC(void, Uniform3i, (GLint, GLint, GLint, GLint)) \
	HGL_FUNC(void, Uniform4i, (GLint, GLint, GLint, GLint, GLint)) \
	HGL_FUNC(void, Uniform1fv, (GLint, GLsizei, const GLfloat *)) \
	HGL_FUNC(void, Uniform2fv, (GLint, GLsizei, const GLfloat *)) \
	HGL_FUNC(void, Uniform3fv, (GLint, GLsizei, const GLfloat *)) \
	HGL_FUNC(void, Uniform4fv, (GLint, GLsizei, const GLfloat *)) \
	HGL_FUNC(void, Uniform1iv, (GLint, GLsizei, const GLint *)) \
	HGL_FUNC(void, Uniform2iv, (GLint, GLsizei, const GLint *)) \
	HGL_FUNC(void, Uniform3iv, (GLint, GLsizei, const GLint *)) \
	HGL_FUNC(void, Uniform4iv, (GLint, GLsizei, const GLint *)) \
	HGL_FUNC(void, UniformMatrix2fv, (GLint, GLsizei, GLboolean, const GLfloat *)) \
	HGL_FUNC(void, UniformMatrix3fv, (GLint, GLsizei, GLboolean, const GLfloat *)) \
	HGL_FUNC(void, UniformMatrix4fv, (GLint, GLsizei, GLboolean, const GLfloat *)) \
	HGL_FUNC(void, Clear, (GLbitfield mask )) \
	HGL_FUNC(void, ClearColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )) \
	HGL_FUNC(void, EnableVertexAttribArray, (GLuint)) \
	HGL_FUNC(void, DisableVertexAttribArray, (GLuint)) \
	HGL_FUNC(void, VertexAttribPointer, (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *)) \
	HGL_FUNC(void, DrawArrays, (GLenum mode, GLint first, GLsizei count )) \
	HGL_FUNC(void, GenBuffers, (GLsizei, GLuint *)) \
	HGL_FUNC(void, BindBuffer, (GLenum, GLuint)) \
	HGL_FUNC(void, BufferData, (GLenum, GLsizeiptr, const GLvoid *, GLenum)) \
	HGL_FUNC(void, GenTextures, (GLsizei n, GLuint *textures)) \
	HGL_FUNC(void, DeleteTextures, (GLsizei n, const GLuint *textures)) \
	HGL_FUNC(void, BindTexture, (GLenum target, GLuint texture)) \
	HGL_FUNC(void, TexImage2D, (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)) \
	HGL_FUNC(void, TexParameterf, (GLenum target, GLenum pname, GLfloat param)) \
	HGL_FUNC(void, TexParameteri, (GLenum target, GLenum pname, GLint param)) \
	HGL_FUNC(void, TexParameterfv, (GLenum target, GLenum pname, const GLfloat *params)) \
	HGL_FUNC(void, TexParameteriv, (GLenum target, GLenum pname, const GLint *params)) \
	HGL_FUNC(const GLubyte*, GetString, (GLenum name)) \
	HGL_FUNC(void, GetBooleanv, (GLenum pname, GLboolean *params)) \
	HGL_FUNC(void, GetFloatv, (GLenum pname, GLfloat *params)) \
	HGL_FUNC(void, GetIntegerv, (GLenum pname, GLint *params)) \
	HGL_FUNC(void, BlendFunc, (GLenum sfactor, GLenum dfactor)) \
	HGL_FUNC(void, ColorMask, (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)) \
	HGL_FUNC(void, CullFace, (GLenum mode)) \
	HGL_FUNC(void, Enable, (GLenum cap)) \
	HGL_FUNC(void, Disable, (GLenum cap)) \
	HGL_FUNC(void, Viewport, (GLint x, GLint y, GLsizei width, GLsizei height)) \
	HGL_FUNC(void, PixelStorei, (GLenum pname, GLint param)) \
	HGL_FUNC(void, TexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)) \
	HGL_FUNC(void, VertexAttrib1f, (GLuint, GLfloat)) \
	HGL_FUNC(void, VertexAttrib1fv, (GLuint, const GLfloat *)) \
	HGL_FUNC(void, VertexAttrib2f, (GLuint, GLfloat, GLfloat)) \
	HGL_FUNC(void, VertexAttrib2fv, (GLuint, const GLfloat *)) \
	HGL_FUNC(void, VertexAttrib3f, (GLuint, GLfloat, GLfloat, GLfloat)) \
	HGL_FUNC(void, VertexAttrib3fv, (GLuint, const GLfloat *)) \
	HGL_FUNC(void, VertexAttrib4f, (GLuint, GLfloat, GLfloat, GLfloat, GLfloat)) \
	HGL_FUNC(void, VertexAttrib4fv, (GLuint, const GLfloat *)) \
	HGL_FUNC(void, DeleteProgram, (GLuint)) \
	HGL_FUNC(void, DeleteShader, (GLuint)) \
	HGL_FUNC(void, DetachShader, (GLuint, GLuint)) \
	HGL_FUNC(void, Hint, (GLenum target, GLenum mode)) \
	HGL_FUNC(void, Scissor, (GLint x, GLint y, GLsizei width, GLsizei height)) \
	HGL_FUNC(void, ReadPixels, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)) \
	HGL_FUNC(void, GenerateMipmapEXT, (GLenum)) \
	HGL_FUNC(void, BlendEquation, (GLenum mode)) \
	HGL_FUNC(void, BlendColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)) \
	HGL_FUNC(void, BlendFuncSeparate, (GLenum, GLenum, GLenum, GLenum)) \
	HGL_FUNC(void, BlendEquationSeparate, (GLenum, GLenum)) \
	HGL_FUNC(void, BindFramebufferEXT, (GLenum, GLuint)) \
	HGL_FUNC(void, DeleteFramebuffersEXT, (GLsizei, const GLuint *)) \
	HGL_FUNC(void, GenFramebuffersEXT, (GLsizei, GLuint *)) \
	HGL_FUNC(void, BindRenderbufferEXT, (GLenum, GLuint)) \
	HGL_FUNC(void, DeleteRenderbuffersEXT, (GLsizei, const GLuint *)) \
	HGL_FUNC(void, GenRenderbuffersEXT, (GLsizei, GLuint *)) \
	HGL_FUNC(GLboolean, IsEnabled, (GLenum cap)) \
	HGL_FUNC(void, ActiveTexture, (GLenum texture)) \
	HGL_FUNC(void, GetTexParameterfv, (GLenum target, GLenum pname, const GLfloat *params)) \
	HGL_FUNC(void, GetTexParameteriv, (GLenum target, GLenum pname, const GLint *params)) \
	HGL_FUNC(GLboolean, IsTexture, (GLuint texture)) \
	HGL_FUNC(GLboolean, IsProgram, (GLuint program)) \
	HGL_FUNC(GLboolean, IsShader, (GLuint shader)) \
	HGL_FUNC(void, BufferSubData, (GLenum, GLintptr, GLsizeiptr, const GLvoid *)) \
	HGL_FUNC(void, DeleteBuffers, (GLsizei, const GLuint *)) \
	HGL_FUNC(GLboolean, IsBuffer, (GLuint)) \
	HGL_FUNC(void, DrawElements, (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)) \
	HGL_FUNC(GLint, GetAttribLocation, (GLuint, const GLchar *)) \
	HGL_FUNC(void, StencilFunc, (GLenum func, GLint ref, GLuint mask)) \
	HGL_FUNC(void, StencilMask, (GLuint mask)) \
	HGL_FUNC(void, StencilOp, (GLenum fail, GLenum zfail, GLenum zpass)) \
	HGL_FUNC(void, ClearStencil, (GLint s)) \
	HGL_FUNC(void, FramebufferTexture2DEXT, (GLenum, GLenum, GLenum, GLuint, GLint)) \
	HGL_FUNC(void, RenderbufferStorageEXT, (GLenum, GLenum, GLsizei, GLsizei)) \
	HGL_FUNC(void, FramebufferRenderbufferEXT, (GLenum, GLenum, GLenum, GLuint)) \
	HGL_FUNC(GLenum, CheckFramebufferStatusEXT, (GLenum)) \
	HGL_FUNC(void, Finish, (void)) \
	HGL_FUNC(void, Flush, (void)) \
	HGL_FUNC(void, DepthFunc, (GLenum func)) \
	HGL_FUNC(void, DepthMask, (GLboolean flag)) \
	HGL_FUNC(void, DepthRange, (GLclampd zNear, GLclampd zFar)) \
	HGL_FUNC(void, ClearDepth, (GLclampd depth)) \
	HGL_FUNC(void, GetBufferParameteriv, (GLenum target, GLenum pname, GLint* params)) \
	HGL_FUNC(void, GetActiveAttrib, (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)) \
	HGL_FUNC(void, GetActiveUniform, (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)) \
	HGL_FUNC(void, GetAttachedShaders, (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *obj)) \
	HGL_FUNC(void, GetUniformfv, (GLuint program, GLint location, GLfloat *params)) \
	HGL_FUNC(void, GetUniformiv, (GLuint program, GLint location, GLint *params)) \
	HGL_FUNC(void, ValidateProgram, (GLuint program)) \
	HGL_FUNC(void, GetShaderSource, (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)) \
	HGL_FUNC(void, LineWidth, (GLfloat width)) \
	HGL_FUNC(void, FrontFace, (GLenum mode)) \
	HGL_FUNC(void, PolygonOffset, (GLfloat factor, GLfloat units)) \
	HGL_FUNC(void, SampleCoverage, (GLclampf value, GLboolean invert)) \
	HGL_FUNC(void, StencilMaskSeparate, (GLenum face, GLuint mask)) \
	HGL_FUNC(void, StencilFuncSeparate, (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)) \
	HGL_FUNC(void, StencilOpSeparate, (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)) \
	HGL_FUNC(void, CopyTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)) \
	HGL_FUNC(void, GetFramebufferAttachmentParameterivEXT, (GLenum target, GLenum attachment, GLenum pname, GLint *params)) \
	HGL_FUNC(void, GetRenderbufferParameterivEXT, (GLenum target, GLenum pname, GLint *params)) \
	HGL_FUNC(void, CopyTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)) \
	HGL_FUNC(void, IsRenderbufferEXT, (GLuint renderbuffer)) \
	HGL_FUNC(void, IsFramebufferEXT, (GLuint framebuffer)) \
	HGL_FUNC(void, GetVertexAttribiv, (GLuint index, GLenum pname, GLint *params)) \
	HGL_FUNC(void, GetVertexAttribfv, (GLuint index, GLenum pname, GLfloat *params)) \
	HGL_FUNC(void, GetTexLevelParameteriv, (GLenum target, GLint level, GLenum pname, GLint *params)) \
	HGL_FUNC(void, GetTexImage, (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels)) \
	HGL_FUNC(void, DrawPixels, (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels))

typedef struct HGL
{
#define HGL_FUNC(ret, name, attr) ret (GL_APIENTRY *name)attr;
	HGL_FUNCS
#undef HGL_FUNC
} HGL;

#ifndef HGL_NO_VARIABLES
extern HGL hgl;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern int hglLoad(HGL* hgl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // HGL_NO_VARIABLES

#endif // HGL_H_
