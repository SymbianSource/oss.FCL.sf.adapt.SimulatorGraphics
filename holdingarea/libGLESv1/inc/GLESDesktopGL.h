/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#ifndef _DESKTOPGL_H_
#define _DESKTOPGL_H_

#include <GLES/gl.h>
#include "glesInternal.h"

class GLESDesktopGL
{
public:
	GLESDesktopGL();

	bool Load();

	void (GL_APIENTRY *glActiveTexture) ( GLenum texture );
	void (GL_APIENTRY *glAlphaFunc) (GLenum func, GLclampf ref);
	void (GL_APIENTRY *glBegin)( GLenum mode );
	void (GL_APIENTRY *glBindBuffer) (GLenum target, GLuint buffer);
	void (GL_APIENTRY *glBindTexture) (GLenum target, GLuint texture);
	void (GL_APIENTRY *glBlendFunc) (GLenum sfactor, GLenum dfactor);
	void (GL_APIENTRY *glBufferData) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
	void (GL_APIENTRY *glBufferSubData) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
	void (GL_APIENTRY *glClear) (GLbitfield mask);
	void (GL_APIENTRY *glClearColor) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
	void (GL_APIENTRY *glClearDepth) (GLclampd depth);
	void (GL_APIENTRY *glClearStencil) (GLint s);
	void (GL_APIENTRY *glClientActiveTexture) ( GLenum texture );
	void (GL_APIENTRY *glClipPlane) (GLenum plane, const GLdouble *equation);
	void (GL_APIENTRY *glColor4d) (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
	void (GL_APIENTRY *glColor4f) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void (GL_APIENTRY *glColor4fv) ( const GLfloat *v );
	void (GL_APIENTRY *glColor4ub) (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
	void (GL_APIENTRY *glColor4ubv) ( const GLubyte *v );
	void (GL_APIENTRY *glColorMask) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	void (GL_APIENTRY *glColorPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GL_APIENTRY *glCompressedTexImage2D) ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data );
	void (GL_APIENTRY *glCompressedTexSubImage2D) ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data );
	void (GL_APIENTRY *glCopyTexImage2D) (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	void (GL_APIENTRY *glCopyTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glCullFace) (GLenum mode);
	void (GL_APIENTRY *glDeleteBuffers) (GLsizei n, const GLuint *buffers);
	void (GL_APIENTRY *glDeleteTextures) (GLsizei n, const GLuint *textures);
	void (GL_APIENTRY *glDepthFunc) (GLenum func);
	void (GL_APIENTRY *glDepthMask) (GLboolean flag);
	void (GL_APIENTRY *glDepthRange) (GLclampd zNear, GLclampd zFar);
	void (GL_APIENTRY *glDisable) (GLenum cap);
	void (GL_APIENTRY *glDisableClientState) (GLenum array);
	void (GL_APIENTRY *glDrawArrays) (GLenum mode, GLint first, GLsizei count);
	void (GL_APIENTRY *glDrawElements) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
	void (GL_APIENTRY *glEnable) (GLenum cap);
	void (GL_APIENTRY *glEnableClientState) (GLenum array);
	void (GL_APIENTRY *glEnd) ( void );
	void (GL_APIENTRY *glFinish) (void);
	void (GL_APIENTRY *glFlush) (void);
	void (GL_APIENTRY *glFogf) (GLenum pname, GLfloat param);
	void (GL_APIENTRY *glFogfv) (GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glFrontFace) (GLenum mode);
	void (GL_APIENTRY *glFrustum) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
	void (GL_APIENTRY *glGenBuffers) (GLsizei n, GLuint *buffers);
	void (GL_APIENTRY *glGenTextures) (GLsizei n, GLuint *textures);
	void (GL_APIENTRY *glGetBooleanv) (GLenum pname, GLboolean *params);
	void (GL_APIENTRY *glGetBufferParameteriv) (GLenum, GLenum, GLint *);
	void (GL_APIENTRY *glGetClipPlane) (GLenum plane, GLdouble *equation);
	void (GL_APIENTRY *glGetDoublev) ( GLenum pname, GLdouble *params );
	GLenum (GL_APIENTRY *glGetError) (void);
	void (GL_APIENTRY *glGetFloatv) (GLenum pname, GLfloat *params);
	void (GL_APIENTRY *glGetIntegerv) (GLenum pname, GLint *params);
	void (GL_APIENTRY *glGetLightfv) (GLenum light, GLenum pname, GLfloat *params);
	void (GL_APIENTRY *glGetMaterialfv) (GLenum face, GLenum pname, GLfloat *params);
	void (GL_APIENTRY *glGetPointerv) (GLenum pname, GLvoid* *params);
	const GLubyte * (GL_APIENTRY *glGetString) (GLenum name);
	void (GL_APIENTRY *glGetTexEnvfv) (GLenum target, GLenum pname, GLfloat *params);
	void (GL_APIENTRY *glGetTexEnviv) (GLenum target, GLenum pname, GLint *params);
	void (GL_APIENTRY *glGetTexParameterfv) (GLenum target, GLenum pname, GLfloat *params);
	void (GL_APIENTRY *glGetTexParameteriv) (GLenum target, GLenum pname, GLint *params);
	void (GL_APIENTRY *glHint) (GLenum target, GLenum mode);
	GLboolean (GL_APIENTRY *glIsBuffer) (GLuint);
	GLboolean (GL_APIENTRY *glIsEnabled) (GLenum cap);
	GLboolean (GL_APIENTRY *glIsTexture) (GLuint texture);
	void (GL_APIENTRY *glLightf) (GLenum light, GLenum pname, GLfloat param);
	void (GL_APIENTRY *glLightfv) (GLenum light, GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glLightModelf) (GLenum pname, GLfloat param);
	void (GL_APIENTRY *glLightModelfv) (GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glLineWidth) (GLfloat width);
	void (GL_APIENTRY *glLoadIdentity) (void);
	void (GL_APIENTRY *glLoadMatrixf) (const GLfloat *m);
	void (GL_APIENTRY *glLogicOp) (GLenum opcode);
	void (GL_APIENTRY *glMaterialf) (GLenum face, GLenum pname, GLfloat param);
	void (GL_APIENTRY *glMaterialfv) (GLenum face, GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glMultiTexCoord2fv) ( GLenum target, const GLfloat *v );
	void (GL_APIENTRY *glMultiTexCoord2sv) ( GLenum target, const GLshort *v );
	void (GL_APIENTRY *glMultiTexCoord3fv) ( GLenum target, const GLfloat *v );
	void (GL_APIENTRY *glMultiTexCoord3sv) ( GLenum target, const GLshort *v );
	void (GL_APIENTRY *glMultiTexCoord4f) ( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q );
	void (GL_APIENTRY *glMultiTexCoord4fv) ( GLenum target, const GLfloat *v );
	void (GL_APIENTRY *glMultiTexCoord4sv) ( GLenum target, const GLshort *v );
	void (GL_APIENTRY *glMultMatrixf) (const GLfloat *m);
	void (GL_APIENTRY *glNormal3f) (GLfloat nx, GLfloat ny, GLfloat nz);
	void (GL_APIENTRY *glNormal3fv) ( const GLfloat *v );
	void (GL_APIENTRY *glNormal3sv) ( const GLshort *v );
	void (GL_APIENTRY *glOrtho) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
	void (GL_APIENTRY *glPointParameterf) (GLenum, GLfloat);
	void (GL_APIENTRY *glPointParameterfv) (GLenum, const GLfloat *);
	void (GL_APIENTRY *glPointSize) (GLfloat size);
	void (GL_APIENTRY *glPolygonOffset) (GLfloat factor, GLfloat units);
	void (GL_APIENTRY *glRotatef) (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *glScalef) (GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *glTexEnvf) (GLenum target, GLenum pname, GLfloat param);
	void (GL_APIENTRY *glTexEnvfv) (GLenum target, GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glTexParameterf) (GLenum target, GLenum pname, GLfloat param);
	void (GL_APIENTRY *glTexParameterfv) (GLenum target, GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glMatrixMode) (GLenum mode);
	void (GL_APIENTRY *glNormalPointer) (GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GL_APIENTRY *glPixelStorei) (GLenum pname, GLint param);
	void (GL_APIENTRY *glPopMatrix) (void);
	void (GL_APIENTRY *glPushMatrix) (void);
	void (GL_APIENTRY *glReadPixels) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
	void (GL_APIENTRY *glSampleCoverage) ( GLclampf value, GLboolean invert );
	void (GL_APIENTRY *glScissor) (GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glShadeModel) (GLenum mode);
	void (GL_APIENTRY *glStencilFunc) (GLenum func, GLint ref, GLuint mask);
	void (GL_APIENTRY *glStencilMask) (GLuint mask);
	void (GL_APIENTRY *glStencilOp) (GLenum fail, GLenum zfail, GLenum zpass);
	void (GL_APIENTRY *glTexCoordPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GL_APIENTRY *glTexEnvi) (GLenum target, GLenum pname, GLint param);
	void (GL_APIENTRY *glTexEnviv) (GLenum target, GLenum pname, const GLint *params);
	void (GL_APIENTRY *glTexImage2D) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
	void (GL_APIENTRY *glTexParameteri) (GLenum target, GLenum pname, GLint param);
	void (GL_APIENTRY *glTexParameteriv) (GLenum target, GLenum pname, const GLint *params);
	void (GL_APIENTRY *glTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
	void (GL_APIENTRY *glTranslatef) (GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *glVertex2fv) ( const GLfloat *v );
	void (GL_APIENTRY *glVertex2sv) ( const GLshort *v );
	void (GL_APIENTRY *glVertex3fv) ( const GLfloat *v );
	void (GL_APIENTRY *glVertex3sv) ( const GLshort *v );
	void (GL_APIENTRY *glVertex4fv) ( const GLfloat *v );
	void (GL_APIENTRY *glVertex4sv) ( const GLshort *v );
	void (GL_APIENTRY *glVertexPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GL_APIENTRY *glViewport) (GLint x, GLint y, GLsizei width, GLsizei height);
    
    // for internal use in EGLtoGLESInterface::UpdateBuffers()
    void (GL_APIENTRY *glDrawPixels) (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);

private:
	bool m_loaded;
};

#endif // _DESKTOPGL_H_
