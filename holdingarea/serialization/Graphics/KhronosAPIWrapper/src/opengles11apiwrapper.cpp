// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// 

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN                       // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <GLES/gl.h>
#include "opengles11apiwrapper.h"
#include "serializedfunctioncall.h"
#include "remotefunctioncall.h"
#include "khronosapiwrapperdefs.h"

#include "openvgrfc.h"


OGLES11Wrapper::OGLES11Wrapper(RemoteFunctionCallData& currentFunctionCallData,
            APIWrapperStack* stack,
            void* result,
            MGraphicsVHWCallback* serviceIf):
APIWrapper(currentFunctionCallData, stack, result, serviceIf),
m_currentFunctionCall(m_currentFunctionCallData)
{
}

int OGLES11Wrapper::DispatchRequest(unsigned long aCode)
{
    int ret(0);
    TRACE("OpenGLES11APIWrapper: OpenGL ES Op command received: %u\n", aCode);

    switch ( aCode )
    {
    case OpenGlES11RFC::EglActiveTexture:
        ret = glActiveTexture();
        break;
    case OpenGlES11RFC::EglAlphaFunc:
        ret = glAlphaFunc();
        break;
    case OpenGlES11RFC::EglAlphaFuncx:
        ret = glAlphaFuncx();
        break;
    case OpenGlES11RFC::EglBindBuffer:
        ret = glBindBuffer();
        break;
    case OpenGlES11RFC::EglBindTexture:
        ret = glBindTexture();
        break;
    case OpenGlES11RFC::EglBlendFunc:
        ret = glBlendFunc();
        break;
    case OpenGlES11RFC::EglBufferData:
        ret = glBufferData();
        break;
    case OpenGlES11RFC::EglBufferSubData:
        ret = glBufferSubData();
        break;
    case OpenGlES11RFC::EglClear:
        ret = glClear();
        break;
    case OpenGlES11RFC::EglClearColor:
        ret = glClearColor();
        break;
    case OpenGlES11RFC::EglClearColorx:
        ret = glClearColorx();
        break;
    case OpenGlES11RFC::EglClearDepthf:
        ret = glClearDepthf();
        break;
    case OpenGlES11RFC::EglClearDepthx:
        ret = glClearDepthx();
        break;
    case OpenGlES11RFC::EglClearStencil:
        ret = glClearStencil();
        break;
    case OpenGlES11RFC::EglClientActiveTexture:
        ret = glClientActiveTexture();
        break;
    case OpenGlES11RFC::EglClipPlanef:
        ret = glClipPlanef();
        break;
    case OpenGlES11RFC::EglClipPlanex:
        ret = glClipPlanex();
        break;
    case OpenGlES11RFC::EglColor4f:
        ret = glColor4f();
        break;
    case OpenGlES11RFC::EglColor4ub:
        ret = glColor4ub();
        break;
    case OpenGlES11RFC::EglColor4x:
        ret = glColor4x();
        break;
    case OpenGlES11RFC::EglColorMask:
        ret = glColorMask();
        break;
    case OpenGlES11RFC::EglCompressedTexImage2D:
        ret = glCompressedTexImage2D();
        break;
    case OpenGlES11RFC::EglCompressedTexSubImage2D:
        ret = glCompressedTexSubImage2D();
        break;
    case OpenGlES11RFC::EglCopyTexImage2D:
        ret = glCopyTexImage2D();
        break;
    case OpenGlES11RFC::EglCopyTexSubImage2D:
        ret = glCopyTexSubImage2D();
        break;
    case OpenGlES11RFC::EglCullFace:
        ret = glCullFace();
        break;
    case OpenGlES11RFC::EglDeleteBuffers:
        ret = glDeleteBuffers();
        break;
    case OpenGlES11RFC::EglDeleteTextures:
        ret = glDeleteTextures();
        break;
    case OpenGlES11RFC::EglDepthFunc:
        ret = glDepthFunc();
        break;
    case OpenGlES11RFC::EglDepthMask:
        ret = glDepthMask();
        break;
    case OpenGlES11RFC::EglDepthRangef:
        ret = glDepthRangef();
        break;
    case OpenGlES11RFC::EglDepthRangex:
        ret = glDepthRangex();
        break;
    case OpenGlES11RFC::EglDisable:
        ret = glDisable();
        break;
    case OpenGlES11RFC::EglDisableClientState:
        ret = glDisableClientState();
        break;
    case OpenGlES11RFC::EglDrawArrays:
        ret = glDrawArrays();
        break;
    case OpenGlES11RFC::EglDrawElements:
        ret = glDrawElements();
        break;
    case OpenGlES11RFC::EglEnable:
        ret = glEnable();
        break;
    case OpenGlES11RFC::EglEnableClientState:
        ret = glEnableClientState();
        break;
    case OpenGlES11RFC::EglFinish:
        ret = glFinish();
        break;
    case OpenGlES11RFC::EglFlush:
        ret = glFlush();
        break;
    case OpenGlES11RFC::EglFogf:
        ret = glFogf();
        break;
    case OpenGlES11RFC::EglFogfv:
        ret = glFogfv();
        break;
    case OpenGlES11RFC::EglFogx:
        ret = glFogx();
        break;
    case OpenGlES11RFC::EglFogxv:
        ret = glFogxv();
        break;
    case OpenGlES11RFC::EglFrontFace:
        ret = glFrontFace();
        break;
    case OpenGlES11RFC::EglFrustumf:
        ret = glFrustumf();
        break;
    case OpenGlES11RFC::EglFrustumx:
        ret = glFrustumx();
        break;
    case OpenGlES11RFC::EglGenBuffers:
        ret = glGenBuffers();
        break;
    case OpenGlES11RFC::EglGenTextures:
        ret = glGenTextures();
        break;
    case OpenGlES11RFC::EglGetBooleanv:
        ret = glGetBooleanv();
        break;
    case OpenGlES11RFC::EglGetBufferParameteriv:
        ret = glGetBufferParameteriv();
        break;
    case OpenGlES11RFC::EglGetClipPlanef:
        ret = glGetClipPlanef();
        break;
    case OpenGlES11RFC::EglGetClipPlanex:
        ret = glGetClipPlanex();
        break;
    case OpenGlES11RFC::EglGetError:
        ret = glGetError();
        break;
    case OpenGlES11RFC::EglGetFixedv:
        ret = glGetFixedv();
        break;
    case OpenGlES11RFC::EglGetFloatv:
        ret = glGetFloatv();
        break;
    case OpenGlES11RFC::EglGetIntegerv:
        ret = glGetIntegerv();
        break;
    case OpenGlES11RFC::EglGetLightfv:
        ret = glGetLightfv();
        break;
    case OpenGlES11RFC::EglGetLightxv:
        ret = glGetLightxv();
        break;
    case OpenGlES11RFC::EglGetMaterialfv:
        ret = glGetMaterialfv();
        break;
    case OpenGlES11RFC::EglGetMaterialxv:
        ret = glGetMaterialxv();
        break;
    case OpenGlES11RFC::EglGetPointerv:
        ret = glGetPointerv();
        break;
//  case OpenGlES11RFC::EglGetString:
//      ret = glGetString();
//      break;
    case OpenGlES11RFC::EglGetTexEnvfv:
        ret = glGetTexEnvfv();
        break;
    case OpenGlES11RFC::EglGetTexEnviv:
        ret = glGetTexEnviv();
        break;
    case OpenGlES11RFC::EglGetTexEnvxv:
        ret = glGetTexEnvxv();
        break;
    case OpenGlES11RFC::EglGetTexParameterfv:
        ret = glGetTexParameterfv();
        break;
    case OpenGlES11RFC::EglGetTexParameteriv:
        ret = glGetTexParameteriv();
        break;
    case OpenGlES11RFC::EglGetTexParameterxv:
        ret = glGetTexParameterxv();
        break;
    case OpenGlES11RFC::EglHint:
        ret = glHint();
        break;
    case OpenGlES11RFC::EglIsBuffer:
        ret = glIsBuffer();
        break;
    case OpenGlES11RFC::EglIsEnabled:
        ret = glIsEnabled();
        break;
    case OpenGlES11RFC::EglIsTexture:
        ret = glIsTexture();
        break;
    case OpenGlES11RFC::EglLightModelf:
        ret = glLightModelf();
        break;
    case OpenGlES11RFC::EglLightModelfv:
        ret = glLightModelfv();
        break;
    case OpenGlES11RFC::EglLightModelx:
        ret = glLightModelx();
        break;
    case OpenGlES11RFC::EglLightModelxv:
        ret = glLightModelxv();
        break;
    case OpenGlES11RFC::EglLightf:
        ret = glLightf();
        break;
    case OpenGlES11RFC::EglLightfv:
        ret = glLightfv();
        break;
    case OpenGlES11RFC::EglLightx:
        ret = glLightx();
        break;
    case OpenGlES11RFC::EglLightxv:
        ret = glLightxv();
        break;
    case OpenGlES11RFC::EglLineWidth:
        ret = glLineWidth();
        break;
    case OpenGlES11RFC::EglLineWidthx:
        ret = glLineWidthx();
        break;
    case OpenGlES11RFC::EglLoadIdentity:
        ret = glLoadIdentity();
        break;
    case OpenGlES11RFC::EglLoadMatrixf:
        ret = glLoadMatrixf();
        break;
    case OpenGlES11RFC::EglLoadMatrixx:
        ret = glLoadMatrixx();
        break;
    case OpenGlES11RFC::EglLogicOp:
        ret = glLogicOp();
        break;
    case OpenGlES11RFC::EglMaterialf:
        ret = glMaterialf();
        break;
    case OpenGlES11RFC::EglMaterialfv:
        ret = glMaterialfv();
        break;
    case OpenGlES11RFC::EglMaterialx:
        ret = glMaterialx();
        break;
    case OpenGlES11RFC::EglMaterialxv:
        ret = glMaterialxv();
        break;
    case OpenGlES11RFC::EglMatrixMode:
        ret = glMatrixMode();
        break;
    case OpenGlES11RFC::EglMultMatrixf:
        ret = glMultMatrixf();
        break;
    case OpenGlES11RFC::EglMultMatrixx:
        ret = glMultMatrixx();
        break;
    case OpenGlES11RFC::EglMultiTexCoord4f:
        ret = glMultiTexCoord4f();
        break;
    case OpenGlES11RFC::EglMultiTexCoord4x:
        ret = glMultiTexCoord4x();
        break;
    case OpenGlES11RFC::EglNormal3f:
        ret = glNormal3f();
        break;
    case OpenGlES11RFC::EglNormal3x:
        ret = glNormal3x();
        break;
    case OpenGlES11RFC::EglOrthof:
        ret = glOrthof();
        break;
    case OpenGlES11RFC::EglOrthox:
        ret = glOrthox();
        break;
    case OpenGlES11RFC::EglPixelStorei:
        ret = glPixelStorei();
        break;
    case OpenGlES11RFC::EglPointParameterf:
        ret = glPointParameterf();
        break;
    case OpenGlES11RFC::EglPointParameterfv:
        ret = glPointParameterfv();
        break;
    case OpenGlES11RFC::EglPointParameterx:
        ret = glPointParameterx();
        break;
    case OpenGlES11RFC::EglPointParameterxv:
        ret = glPointParameterxv();
        break;
    case OpenGlES11RFC::EglPointSize:
        ret = glPointSize();
        break;
    case OpenGlES11RFC::EglPointSizex:
        ret = glPointSizex();
        break;
    case OpenGlES11RFC::EglPolygonOffset:
        ret = glPolygonOffset();
        break;
    case OpenGlES11RFC::EglPolygonOffsetx:
        ret = glPolygonOffsetx();
        break;
    case OpenGlES11RFC::EglPopMatrix:
        ret = glPopMatrix();
        break;
    case OpenGlES11RFC::EglPushMatrix:
        ret = glPushMatrix();
        break;
    case OpenGlES11RFC::EglReadPixels:
        ret = glReadPixels();
        break;
    case OpenGlES11RFC::EglRotatef:
        ret = glRotatef();
        break;
    case OpenGlES11RFC::EglRotatex:
        ret = glRotatex();
        break;
    case OpenGlES11RFC::EglSampleCoverage:
        ret = glSampleCoverage();
        break;
    case OpenGlES11RFC::EglSampleCoveragex:
        ret = glSampleCoveragex();
        break;
    case OpenGlES11RFC::EglScalef:
        ret = glScalef();
        break;
    case OpenGlES11RFC::EglScalex:
        ret = glScalex();
        break;
    case OpenGlES11RFC::EglScissor:
        ret = glScissor();
        break;
    case OpenGlES11RFC::EglShadeModel:
        ret = glShadeModel();
        break;
    case OpenGlES11RFC::EglStencilFunc:
        ret = glStencilFunc();
        break;
    case OpenGlES11RFC::EglStencilMask:
        ret = glStencilMask();
        break;
    case OpenGlES11RFC::EglStencilOp:
        ret = glStencilOp();
        break;
    case OpenGlES11RFC::EglTexEnvf:
        ret = glTexEnvf();
        break;
    case OpenGlES11RFC::EglTexEnvfv:
        ret = glTexEnvfv();
        break;
    case OpenGlES11RFC::EglTexEnvi:
        ret = glTexEnvi();
        break;
    case OpenGlES11RFC::EglTexEnviv:
        ret = glTexEnviv();
        break;
    case OpenGlES11RFC::EglTexEnvx:
        ret = glTexEnvx();
        break;
    case OpenGlES11RFC::EglTexEnvxv:
        ret = glTexEnvxv();
        break;
    case OpenGlES11RFC::EglTexImage2D:
        ret = glTexImage2D();
        break;
    case OpenGlES11RFC::EglTexParameterf:
        ret = glTexParameterf();
        break;
    case OpenGlES11RFC::EglTexParameterfv:
        ret = glTexParameterfv();
        break;
    case OpenGlES11RFC::EglTexParameteri:
        ret = glTexParameteri();
        break;
    case OpenGlES11RFC::EglTexParameteriv:
        ret = glTexParameteriv();
        break;
    case OpenGlES11RFC::EglTexParameterx:
        ret = glTexParameterx();
        break;
    case OpenGlES11RFC::EglTexParameterxv:
        ret = glTexParameterxv();
        break;
    case OpenGlES11RFC::EglTexSubImage2D:
        ret = glTexSubImage2D();
        break;
    case OpenGlES11RFC::EglTranslatef:
        ret = glTranslatef();
        break;
    case OpenGlES11RFC::EglTranslatex:
        ret = glTranslatex();
        break;
    case OpenGlES11RFC::EglViewport:
        ret = glViewport();
        break;

    // from Eglextplatform.h
    case OpenGlES11RFC::EglCurrentPaletteMatrixOES:
        ret = glCurrentPaletteMatrixOES();
        break;
    case OpenGlES11RFC::EglDrawTexfOES:
        ret = glDrawTexfOES();
        break;
    case OpenGlES11RFC::EglDrawTexfvOES:
        ret = glDrawTexfvOES();
        break;
    case OpenGlES11RFC::EglDrawTexiOES:
        ret = glDrawTexiOES();
        break;
    case OpenGlES11RFC::EglDrawTexivOES:
        ret = glDrawTexivOES();
        break;
    case OpenGlES11RFC::EglDrawTexsOES:
        ret = glDrawTexsOES();
        break;
    case OpenGlES11RFC::EglDrawTexsvOES:
        ret = glDrawTexsvOES();
        break;
    case OpenGlES11RFC::EglDrawTexxOES:
        ret = glDrawTexxOES();
        break;
    case OpenGlES11RFC::EglDrawTexxvOES:
        ret = glDrawTexxvOES();
        break;
    case OpenGlES11RFC::EglLoadPaletteFromModelViewMatrixOES:
        ret = glLoadPaletteFromModelViewMatrixOES();
        break;
    case OpenGlES11RFC::EglQueryMatrixxOES:
        ret = glQueryMatrixxOES();
        break;

    case OpenGlES11RFC::EglColorPointer:
    case OpenGlES11RFC::EglNormalPointer:
    case OpenGlES11RFC::EglTexCoordPointer:
    case OpenGlES11RFC::EglVertexPointer:
    case OpenGlES11RFC::EglMatrixIndexPointerOES:
    case OpenGlES11RFC::EglWeightPointerOES:    
        TRACE("OpenGLES11APIWrapper: ERROR! Vertex pointer functions are not handled directly, should not be here\n");
        break;
        
    // client shutdown
    case OpenGlES11RFC::EClientShutdown:
        TRACE("OpenGLES11APIWrapper: Client shutdown unimplemented\n");
        break;

    default:
        TRACE("OpenGLES11APIWrapper: Unimplemented OpenGL ES Op code %u\n",aCode);
        break;
    }
    TRACE("OpenGLES11APIWrapper: OpenGL ES 1.1 command processed\n");
    return ret;
}

int OGLES11Wrapper::WriteReply()
{
#ifdef LOG_ERROR
    int operationid = (int)m_currentFunctionCall.Data().Header().iOpCode;
    
    int glerror = ::glGetError();
    if ( m_lastGlError != glerror )
    {
        if ( GL_NO_ERROR != glerror )
        {
            TRACE("OpenGLES11APIWrapper: OpenGL error 0x%X, for request %d\n", glerror, operationid );
        }
        m_lastGlError = glerror;
    }
#endif
    return APIWrapper::WriteReply();
}

// ///////////////////////////////////////////////////
// Here follows the functions that map to the GL calls


int OGLES11Wrapper::glActiveTexture()
{
    GLenum texture;
    m_currentFunctionCall.GetParamValue(texture, 0);
    ::glActiveTexture(texture);
    return WriteReply();
}
int OGLES11Wrapper::glAlphaFunc()
{
    GLenum func;
    m_currentFunctionCall.GetParamValue(func, 0);
    GLclampf ref;
    m_currentFunctionCall.GetParamValue(ref, 1);
    ::glAlphaFunc(func, ref);
    return WriteReply();
}
int OGLES11Wrapper::glAlphaFuncx()
{
    GLenum func;
    m_currentFunctionCall.GetParamValue(func, 0);
    GLclampx ref;
    m_currentFunctionCall.GetParamValue(ref, 1);
    ::glAlphaFuncx(func, ref);
    return WriteReply();
}
int OGLES11Wrapper::glBindBuffer()
{
    GLenum target;
    m_currentFunctionCall.GetParamValue(target, 0);    
    GLuint buffer;
    m_currentFunctionCall.GetParamValue(buffer, 1);
    ::glBindBuffer(target, buffer);
    return WriteReply();
}
int OGLES11Wrapper::glBindTexture()
{
    GLenum target;
    m_currentFunctionCall.GetParamValue(target, 0);    
    GLuint texIndex;
    m_currentFunctionCall.GetParamValue(texIndex, 1);
    ::glBindTexture(target, texIndex);
    return WriteReply();
}
int OGLES11Wrapper::glBlendFunc()
{
    GLenum src;
    m_currentFunctionCall.GetParamValue(src, 0);
    GLenum dst;
    m_currentFunctionCall.GetParamValue(dst, 1);
    ::glBlendFunc(src, dst);
    return WriteReply();
}
int OGLES11Wrapper::glBufferData()
{
    GLenum target;
    m_currentFunctionCall.GetParamValue(target, 0);
    //GLsizeiptr size;    
    GLint size;
    m_currentFunctionCall.GetParamValue(size, 1);
    
    void* data;
    //GLsizeiptr size2;
    GLint size2;
    m_currentFunctionCall.GetVectorData(data, size2, 2); //is this correct??
    //size == size2 ?
    
    GLenum usage;
    m_currentFunctionCall.GetParamValue(usage, 3);
    ::glBufferData(target, size, data, usage);
    return WriteReply();
}
int OGLES11Wrapper::glBufferSubData()
{
    GLenum target;
    m_currentFunctionCall.GetParamValue(target, 0);
    //GLintptr offset;
    GLint offset;
    m_currentFunctionCall.GetParamValue(offset, 1);
    //GLsizeiptr size;
    GLint size;
    m_currentFunctionCall.GetParamValue(size, 2);
    void* data;
    //GLsizeiptr size2;
    GLint size2;
    m_currentFunctionCall.GetVectorData(data, size2, 3); //is this correct??
    ::glBufferSubData(target,offset,size,data);
    return WriteReply();
}
int OGLES11Wrapper::glClear()
{
    GLbitfield buf;
    m_currentFunctionCall.GetParamValue(buf, 0);
    ::glClear(buf);
    return WriteReply();
}
int OGLES11Wrapper::glClearColor()
{
    GLclampf r;
    m_currentFunctionCall.GetParamValue(r, 0);
    GLclampf g;
    m_currentFunctionCall.GetParamValue(g, 1);
    GLclampf b;
    m_currentFunctionCall.GetParamValue(b, 2);
    GLclampf a;
    m_currentFunctionCall.GetParamValue(a, 3);
    ::glClearColor(r,g,b,a);
    return WriteReply();
}
int OGLES11Wrapper::glClearColorx()
{
    GLclampx r;
    m_currentFunctionCall.GetParamValue(r, 0);
    GLclampx g;
    m_currentFunctionCall.GetParamValue(g, 1);
    GLclampx b;
    m_currentFunctionCall.GetParamValue(b, 2);
    GLclampx a;
    m_currentFunctionCall.GetParamValue(a, 3);
    ::glClearColorx(r,g,b,a);
    return WriteReply();
}
int OGLES11Wrapper::glClearDepthf()
{
    GLclampf d;
    m_currentFunctionCall.GetParamValue(d, 0);
    ::glClearDepthf(d);
    return WriteReply();
}
int OGLES11Wrapper::glClearDepthx()
{
    GLfixed d;
    m_currentFunctionCall.GetParamValue(d, 0);
    ::glClearDepthx(d);    
    return WriteReply();
}
int OGLES11Wrapper::glClearStencil()
{
    GLint s;
    m_currentFunctionCall.GetParamValue(s, 0);
    ::glClearStencil(s);     
    return WriteReply();
}
int OGLES11Wrapper::glClientActiveTexture()
{
    GLenum texture;
    m_currentFunctionCall.GetParamValue(texture, 0);
    ::glClientActiveTexture(texture);     
    return WriteReply();
}
int OGLES11Wrapper::glClipPlanef()
{
    GLenum plane;
    GLfloat* equation;
    m_currentFunctionCall.GetParamValue(plane, 0);
    //GLsizeiptr size;
    GLint size;
    m_currentFunctionCall.GetVectorData(equation, size, 1);
    ::glClipPlanef(plane,equation);
    return WriteReply();
}
int OGLES11Wrapper::glClipPlanex()
{
    GLenum plane;
    GLfixed* equation;
    //GLsizeiptr size;
    GLint size;
    m_currentFunctionCall.GetParamValue(plane, 0);
    m_currentFunctionCall.GetVectorData(equation, size, 1);
    ::glClipPlanex(plane,equation);
    return WriteReply();
}
int OGLES11Wrapper::glColor4f()
{
    GLfloat r,g,b,a;
    m_currentFunctionCall.GetParamValue(r, 0);
    m_currentFunctionCall.GetParamValue(g, 1);
    m_currentFunctionCall.GetParamValue(b, 2);
    m_currentFunctionCall.GetParamValue(a, 3);
    ::glColor4f(r,g,b,a);    
    return WriteReply();
}
int OGLES11Wrapper::glColor4ub()
{
    GLubyte r,g,b,a;
    m_currentFunctionCall.GetParamValue(r, 0);
    m_currentFunctionCall.GetParamValue(g, 1);
    m_currentFunctionCall.GetParamValue(b, 2);
    m_currentFunctionCall.GetParamValue(a, 3);
    ::glColor4ub(r,g,b,a);  
    return WriteReply();
}
int OGLES11Wrapper::glColor4x()
{
    GLfixed r,g,b,a;
    m_currentFunctionCall.GetParamValue(r, 0);
    m_currentFunctionCall.GetParamValue(g, 1);
    m_currentFunctionCall.GetParamValue(b, 2);
    m_currentFunctionCall.GetParamValue(a, 3);
    ::glColor4x(r,g,b,a);
    return WriteReply();
}
int OGLES11Wrapper::glColorMask()
{
    GLboolean r,g,b,a;
    m_currentFunctionCall.GetParamValue(r, 0);
    m_currentFunctionCall.GetParamValue(g, 1);
    m_currentFunctionCall.GetParamValue(b, 2);
    m_currentFunctionCall.GetParamValue(a, 3);
    ::glColorMask(r,g,b,a);
    return WriteReply();
}

int OGLES11Wrapper::glCompressedTexImage2D()
{
    GLenum target;
    GLint level;
    GLenum internalFormat;
    GLsizei width;
    GLsizei height;
    GLint border;
    GLsizei imageSize;
    GLvoid* data;
    //GLsizeiptr size;
    GLint size;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(level, 1);
    m_currentFunctionCall.GetParamValue(internalFormat, 2);
    m_currentFunctionCall.GetParamValue(width, 3);
    m_currentFunctionCall.GetParamValue(height, 4);
    m_currentFunctionCall.GetParamValue(border, 5);
    m_currentFunctionCall.GetParamValue(imageSize, 6);
    m_currentFunctionCall.GetVectorData(data, size, 7);
    ::glCompressedTexImage2D(target,level,internalFormat,width,height,border,imageSize,data);
    return WriteReply();
}
int OGLES11Wrapper::glCompressedTexSubImage2D()
{
    GLenum target;
    GLint level;
    GLint xoffset;
    GLint yoffset;
    GLsizei width;
    GLsizei height;
    GLenum format;
    GLsizei imageSize;
    GLvoid*   data;
    //GLsizeiptr size;
    GLint size;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(level, 1);
    m_currentFunctionCall.GetParamValue(xoffset, 2);
    m_currentFunctionCall.GetParamValue(yoffset, 3);
    m_currentFunctionCall.GetParamValue(width, 4);
    m_currentFunctionCall.GetParamValue(height, 5);
    m_currentFunctionCall.GetParamValue(format, 6);
    m_currentFunctionCall.GetParamValue(imageSize, 7);
    m_currentFunctionCall.GetVectorData(data, size, 8);
    ::glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    return WriteReply();
}
int OGLES11Wrapper::glCopyTexImage2D()
{
    GLenum target;    
    GLint level;
    GLenum internalFormat;
    GLint x;
    GLint y;
    GLsizei width;
    GLsizei height;
    GLint border;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(level, 1);
    m_currentFunctionCall.GetParamValue(internalFormat, 2);
    m_currentFunctionCall.GetParamValue(x, 3);
    m_currentFunctionCall.GetParamValue(y, 4);    
    m_currentFunctionCall.GetParamValue(width, 5);    
    m_currentFunctionCall.GetParamValue(height, 6);    
    m_currentFunctionCall.GetParamValue(border, 7);
    ::glCopyTexImage2D(target,level,internalFormat,x,y,width,height,border);
    return WriteReply();
}
int OGLES11Wrapper::glCopyTexSubImage2D()
{
    GLenum  target;
    GLint   level;
    GLint   xoffset;
    GLint   yoffset;
    GLint   x;
    GLint   y;
    GLsizei width;
    GLsizei height;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(level, 1);
    m_currentFunctionCall.GetParamValue(xoffset, 2);
    m_currentFunctionCall.GetParamValue(yoffset, 3);
    m_currentFunctionCall.GetParamValue(x, 4);
    m_currentFunctionCall.GetParamValue(y, 5);
    m_currentFunctionCall.GetParamValue(width, 6);
    m_currentFunctionCall.GetParamValue(height, 7);    
    ::glCopyTexSubImage2D(target,level,xoffset,yoffset,x,y,width,height);
    return WriteReply();
}
int OGLES11Wrapper::glCullFace()
{
    GLenum mode;

    m_currentFunctionCall.GetParamValue(mode, 0);

    ::glCullFace(mode);
    return WriteReply();
}
int OGLES11Wrapper::glDeleteBuffers()
{
    GLsizei n;
    GLuint *buffers;

    m_currentFunctionCall.GetParamValue(n, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(buffers, size1, 1);

    ::glDeleteBuffers(n, buffers);
    return WriteReply();
}
int OGLES11Wrapper::glDeleteTextures()
{
    GLsizei n;
    GLuint *textures;

    m_currentFunctionCall.GetParamValue(n, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(textures, size1, 1);

    ::glDeleteTextures(n, textures);
    return WriteReply();
}
int OGLES11Wrapper::glDepthFunc()
{
    GLenum func;

    m_currentFunctionCall.GetParamValue(func, 0);

    ::glDepthFunc(func);
    return WriteReply();
}
int OGLES11Wrapper::glDepthMask()
{
    GLboolean flag;

    m_currentFunctionCall.GetParamValue(flag, 0);

    ::glDepthMask(flag);
    return WriteReply();
}
int OGLES11Wrapper::glDepthRangef()
{
    GLclampf zNear;
    GLclampf zFar;

    m_currentFunctionCall.GetParamValue(zNear, 0);
    m_currentFunctionCall.GetParamValue(zFar, 1);

    ::glDepthRangef(zNear, zFar);

    return WriteReply();
}
int OGLES11Wrapper::glDepthRangex()
{
    GLclampx zNear;
    GLclampx zFar;

    m_currentFunctionCall.GetParamValue(zNear, 0);
    m_currentFunctionCall.GetParamValue(zFar, 1);

    ::glDepthRangex(zNear, zFar);

    return WriteReply();
}
int OGLES11Wrapper::glDisable()
{
    GLenum cap;

    m_currentFunctionCall.GetParamValue(cap, 0);

    ::glDisable(cap);

    return WriteReply();
}
int OGLES11Wrapper::glDisableClientState()
{
    GLenum array;

    m_currentFunctionCall.GetParamValue(array, 0);

    ::glDisableClientState(array);

    return WriteReply();
}
int OGLES11Wrapper::glDrawArrays()
{
    GLenum mode;
    GLint first;
    GLsizei count;

    m_currentFunctionCall.GetParamValue(mode, 0);
    m_currentFunctionCall.GetParamValue(first, 1);
    m_currentFunctionCall.GetParamValue(count, 2);

    // Get the data arrays themselves -- note that this is additional data
    TInt arrayCount, arrayEnabled;
    m_currentFunctionCall.GetParamValue(arrayCount, 3);
    m_currentFunctionCall.GetParamValue(arrayEnabled, 4);

    vertexArrayInfo *vertexArrays;
    vertexArrays = (vertexArrayInfo *) malloc(arrayCount * sizeof(vertexArrayInfo));

    for (int i=0; i<arrayCount; i++)
    {
        m_currentFunctionCall.GetParamValue(vertexArrays[i].size, 5 + 4*i);
        m_currentFunctionCall.GetParamValue(vertexArrays[i].type, 6 + 4*i);
        m_currentFunctionCall.GetParamValue(vertexArrays[i].stride, 7 + 4*i);
        TInt size;
        m_currentFunctionCall.GetVectorData(vertexArrays[i].pointer, size, 8 + 4*i);
    }
    
    int x = 0; //position in the vertexArrays, 'i' in the loop above
    for(int i=0; i<NUM_ARRAYS; i++) 
    {
        switch( i )
        {
            case COLOR_ARRAY:
                if (arrayEnabled & (1<<6))
                {
                    ::glColorPointer(vertexArrays[x].size, vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case NORMAL_ARRAY:
                if (arrayEnabled & (1<<5))
                {
                    ::glNormalPointer(vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case POINT_SIZE_ARRAY:
                if (arrayEnabled & (1<<4))
                {
                    ::glPointSizePointerOES(vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case TEX_COORD_ARRAY:
                if (arrayEnabled & (1<<3))
                {
                    ::glTexCoordPointer(vertexArrays[x].size, vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case VERTEX_ARRAY:
                if (arrayEnabled & (1<<2)){
                    ::glVertexPointer(vertexArrays[x].size, vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case MATRIX_INDEX_ARRAY:
                break;
            case WEIGHT_ARRAY:
                break;
            default:
                // Do nothing
                break;
        }   
    }

    ::glDrawArrays(mode, first, count);
    return WriteReply();
}
int OGLES11Wrapper::glDrawElements()
{
    GLenum mode;
    GLsizei count;
    GLenum type;
    GLvoid *indices;

    m_currentFunctionCall.GetParamValue(mode, 0);
    m_currentFunctionCall.GetParamValue(count, 1);
    m_currentFunctionCall.GetParamValue(type, 2);
    TInt size3;
    m_currentFunctionCall.GetVectorData(indices, size3, 3);

    // Get the data arrays themselves -- note that this is additional data
    TInt arrayCount, arrayEnabled;
    m_currentFunctionCall.GetParamValue(arrayCount, 4);
    m_currentFunctionCall.GetParamValue(arrayEnabled, 5);

    vertexArrayInfo *vertexArrays;
    vertexArrays = (vertexArrayInfo *) malloc(arrayCount * sizeof(vertexArrayInfo));

    for (int i=0; i<arrayCount; i++)
    {
        m_currentFunctionCall.GetParamValue(vertexArrays[i].size, 6 + 4*i);
        m_currentFunctionCall.GetParamValue(vertexArrays[i].type, 7 + 4*i);
        m_currentFunctionCall.GetParamValue(vertexArrays[i].stride, 8 + 4*i);
        TInt size;
        m_currentFunctionCall.GetVectorData(vertexArrays[i].pointer, size, 9 + 4*i);
    }
    
    int x = 0; //position in the vertexArrays, 'i' in the loop above
    for(int i=0; i<NUM_ARRAYS; i++) 
    {
        switch( i )
        {
            case COLOR_ARRAY:
                if (arrayEnabled & (1<<6))
                {
                    ::glColorPointer(vertexArrays[x].size, vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case NORMAL_ARRAY:
                if (arrayEnabled & (1<<5))
                {
                    ::glNormalPointer(vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case POINT_SIZE_ARRAY:
                if (arrayEnabled & (1<<4))
                {
                    ::glPointSizePointerOES(vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case TEX_COORD_ARRAY:
                if (arrayEnabled & (1<<3))
                {
                    ::glTexCoordPointer(vertexArrays[x].size, vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case VERTEX_ARRAY:
                if (arrayEnabled & (1<<2)){
                    ::glVertexPointer(vertexArrays[x].size, vertexArrays[x].type, vertexArrays[x].stride, vertexArrays[x].pointer);
                    x++;
                }
                break;
            case MATRIX_INDEX_ARRAY:
                break;
            case WEIGHT_ARRAY:
                break;
            default:
                // Do nothing
                break;
        }   
    }


    ::glDrawElements(mode, count, type, indices);
    return WriteReply();
}
int OGLES11Wrapper::glEnable()
{
    GLenum cap;

    m_currentFunctionCall.GetParamValue(cap, 0);

    ::glEnable(cap);

    return WriteReply();
}
int OGLES11Wrapper::glEnableClientState()
{
    GLenum array;

    m_currentFunctionCall.GetParamValue(array, 0);

    ::glEnableClientState(array);

    return WriteReply();
}
int OGLES11Wrapper::glFinish()
{
    ::glFinish();
    return WriteReply();
}
int OGLES11Wrapper::glFlush()
{
    ::glFlush();
    return WriteReply();
}
int OGLES11Wrapper::glFogf()
{
    GLenum pname;
    GLfloat param;

    m_currentFunctionCall.GetParamValue(pname, 0);
    m_currentFunctionCall.GetParamValue(param, 1);

    ::glFogf(pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glFogfv()
{
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);

    ::glFogfv(pname, params);

    return WriteReply();
}
int OGLES11Wrapper::glFogx()
{
    GLenum pname;
    GLfixed param;

    m_currentFunctionCall.GetParamValue(pname, 0);
    m_currentFunctionCall.GetParamValue(param, 1);

    ::glFogx(pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glFogxv()
{
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);

    ::glFogxv(pname, params);

    return WriteReply();
}
int OGLES11Wrapper::glFrontFace()
{
    GLenum mode;

    m_currentFunctionCall.GetParamValue(mode, 0);

    ::glFrontFace(mode);

    return WriteReply();
}
int OGLES11Wrapper::glFrustumf()
{
    GLfloat left;
    GLfloat right;
    GLfloat bottom;
    GLfloat top;
    GLfloat zNear;
    GLfloat zFar;

    m_currentFunctionCall.GetParamValue(left, 0);
    m_currentFunctionCall.GetParamValue(right, 1);
    m_currentFunctionCall.GetParamValue(bottom, 2);
    m_currentFunctionCall.GetParamValue(top, 3);
    m_currentFunctionCall.GetParamValue(zNear, 4);
    m_currentFunctionCall.GetParamValue(zFar, 5);

    ::glFrustumf(left, right, bottom, top, zNear, zFar);

    return WriteReply();
}
int OGLES11Wrapper::glFrustumx()
{
    GLfixed left;
    GLfixed right;
    GLfixed bottom;
    GLfixed top;
    GLfixed zNear;
    GLfixed zFar;

    m_currentFunctionCall.GetParamValue(left, 0);
    m_currentFunctionCall.GetParamValue(right, 1);
    m_currentFunctionCall.GetParamValue(bottom, 2);
    m_currentFunctionCall.GetParamValue(top, 3);
    m_currentFunctionCall.GetParamValue(zNear, 4);
    m_currentFunctionCall.GetParamValue(zFar, 5);

    ::glFrustumx(left, right, bottom, top, zNear, zFar);

    return WriteReply();
}
int OGLES11Wrapper::glGenBuffers()
{
    GLsizei n;
    m_currentFunctionCall.GetParamValue(n, 0);
    
    GLuint *buffers = (GLuint*)iStack->AllocFromStack( n*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLuint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLuint ) );

    ::glGenBuffers(n, buffers);

    m_currentFunctionCall.SetVectorData( buffers, n, 1 ); // EOut
    //TODO: when do we free the allocated buffer??
    
    int ret = WriteReply(); //TODO: clarify the copy-pasted magic sequence
    iStack->ClearStack();
    buffers = NULL;
    return ret; 
}
int OGLES11Wrapper::glGenTextures()
{
    GLsizei n;
    m_currentFunctionCall.GetParamValue(n, 0);
    
    GLuint *textures = (GLuint*)iStack->AllocFromStack( n*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLuint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLuint ) );

    ::glGenTextures(n, textures);

    m_currentFunctionCall.SetVectorData( textures, n, 1 ); // EOut

    int ret = WriteReply();
    iStack->ClearStack();
    textures = NULL;
    return ret;    
}
int OGLES11Wrapper::glGetBooleanv()
{
    GLenum pname;
    GLboolean *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);
    
    params = (GLboolean*)iStack->AllocFromStack( size1*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLboolean ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLboolean ) );
        
    ::glGetBooleanv(pname, params); //TODO: how does the ::glGetBooleanv know the array size?
    
    m_currentFunctionCall.SetVectorData( params, size1, 1 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret; 
}
int OGLES11Wrapper::glGetBufferParameteriv()
{
    GLenum target;
    GLenum pname;
    GLint *params;

    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLint*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) );

    ::glGetBufferParameteriv(target, pname, params);

    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret; 
}
int OGLES11Wrapper::glGetClipPlanef()
{
    GLenum pname;
    GLfloat *equation;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(equation, size1, 1);

    equation = (GLfloat*)iStack->AllocFromStack( size1*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLfloat ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLfloat ) );


    ::glGetClipPlanef(pname, equation);

    m_currentFunctionCall.SetVectorData( equation, size1, 1 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    equation = NULL;
    return ret; 
}
int OGLES11Wrapper::glGetClipPlanex()
{
    GLenum pname;
    GLfixed *eqn;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(eqn, size1, 1);

    eqn = (GLfixed*)iStack->AllocFromStack( size1*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) );

    ::glGetClipPlanex(pname, eqn);
    
    m_currentFunctionCall.SetVectorData( eqn, size1, 1 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    eqn = NULL;
    return ret; 
}
int OGLES11Wrapper::glGetError()
{
    GLenum ret = ::glGetError();
    m_currentFunctionCall.SetReturnValue((TUint32) ret);
    return WriteReply();
}
int OGLES11Wrapper::glGetFixedv()
{
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);

    params = (GLfixed*)iStack->AllocFromStack( size1*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
    m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) );        

    ::glGetFixedv(pname, params);

    m_currentFunctionCall.SetVectorData( params, size1, 1 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret; 
}
int OGLES11Wrapper::glGetFloatv()
{
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);

    params = (GLfloat*)iStack->AllocFromStack( size1*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLfloat ),
    m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLfloat ) ); 
    
    ::glGetFloatv(pname, params);

    m_currentFunctionCall.SetVectorData( params, size1, 1 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret; 
}
int OGLES11Wrapper::glGetIntegerv()
{
    GLenum pname;
    GLint *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);

    params = (GLint*)iStack->AllocFromStack( size1*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
    m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) ); 
    
    ::glGetIntegerv(pname, params);

    m_currentFunctionCall.SetVectorData( params, size1, 1 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetLightfv()
{
    GLenum light;
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(light, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLfloat*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLfloat ),
    m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLfloat ) );
    
    ::glGetLightfv(light, pname, params);
    
    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetLightxv()
{
    GLenum light;
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(light, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);
    
    params = (GLfixed*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) );

    ::glGetLightxv(light, pname, params);

    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetMaterialfv()
{
    GLenum face;
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(face, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLfloat*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLfloat ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLfloat ) );
    
    ::glGetMaterialfv(face, pname, params);

    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetMaterialxv()
{
    GLenum face;
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(face, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLfixed*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) );
        
    ::glGetMaterialxv(face, pname, params);

    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetPointerv()
{
    //never gets called?
    return WriteReply();
}
int OGLES11Wrapper::glGetString()
{
    // Not used
    return WriteReply();
}
int OGLES11Wrapper::glGetTexEnvfv()
{
    GLenum env;
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(env, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLfloat*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLfloat ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLfloat ) );
        
    ::glGetTexEnvfv(env, pname, params);

    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetTexEnviv()
{
    GLenum env;
    GLenum pname;
    GLint *params;

    m_currentFunctionCall.GetParamValue(env, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLint*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) );
        
    ::glGetTexEnviv(env, pname, params);

    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetTexEnvxv()
{
    GLenum env;
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(env, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLfixed*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) );
        
    ::glGetTexEnvxv(env, pname, params);

    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetTexParameterfv()
{
    GLenum target;
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLfloat*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLfloat ),    
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLfloat ) );
        
    ::glGetTexParameterfv(target, pname, params);

    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetTexParameteriv()
{
    GLenum target;
    GLenum pname;
    GLint *params;

    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLint*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) );
        
    ::glGetTexParameteriv(target, pname, params);

    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glGetTexParameterxv()
{
    GLenum target;
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    params = (GLint*)iStack->AllocFromStack( size2*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLint ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLint ) );
        
    ::glGetTexParameterxv(target, pname, params);
    
    m_currentFunctionCall.SetVectorData( params, size2, 2 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    params = NULL;
    return ret;
}
int OGLES11Wrapper::glHint()
{
    GLenum target;
    GLenum mode;

    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(mode, 1);

    ::glHint(target, mode);
    
    return WriteReply();
}
int OGLES11Wrapper::glIsBuffer()
{
    GLuint buffer;

    m_currentFunctionCall.GetParamValue(buffer, 0);

    GLboolean ret = ::glIsBuffer(buffer);
    m_currentFunctionCall.SetReturnValue( ret);

    return WriteReply();
}
int OGLES11Wrapper::glIsEnabled()
{
    GLenum cap;
    m_currentFunctionCall.GetParamValue(cap, 0);
    
    GLboolean ret = ::glIsEnabled(cap);
    m_currentFunctionCall.SetReturnValue( ret);
    
    return WriteReply();
}
int OGLES11Wrapper::glIsTexture()
{
    GLuint texIndex;
    m_currentFunctionCall.GetParamValue(texIndex, 0);
    
    GLboolean ret = ::glIsEnabled(texIndex);
    m_currentFunctionCall.SetReturnValue( ret);
        
    return WriteReply();
}
int OGLES11Wrapper::glLightModelf()
{
    GLenum pname;
    GLfloat param;

    m_currentFunctionCall.GetParamValue(pname, 0);
    m_currentFunctionCall.GetParamValue(param, 1);

    ::glLightModelf(pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glLightModelfv()
{
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);

    ::glLightModelfv(pname, params);

    return WriteReply();
}
int OGLES11Wrapper::glLightModelx()
{
    GLenum pname;
    GLfixed param;

    m_currentFunctionCall.GetParamValue(pname, 0);
    m_currentFunctionCall.GetParamValue(param, 1);

    ::glLightModelx(pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glLightModelxv()
{
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);

    ::glLightModelxv(pname, params);

    return WriteReply();
}
int OGLES11Wrapper::glLightf()
{
    GLenum light;
    GLenum pname;
    GLfloat param;

    m_currentFunctionCall.GetParamValue(light, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);

    ::glLightf(light, pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glLightfv()
{
    GLenum light;
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(light, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    ::glLightfv(light, pname, params);

    return WriteReply();
}
int OGLES11Wrapper::glLightx()
{
    GLenum light;
    GLenum pname;
    GLfixed param;

    m_currentFunctionCall.GetParamValue(light, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);

    ::glLightx(light, pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glLightxv()
{
    GLenum light;
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(light, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    ::glLightxv(light, pname, params);
    return WriteReply();
}
int OGLES11Wrapper::glLineWidth()
{
    GLfloat width;

    m_currentFunctionCall.GetParamValue(width, 0);

    ::glLineWidth(width);

    return WriteReply();
}
int OGLES11Wrapper::glLineWidthx()
{
    GLfixed width;

    m_currentFunctionCall.GetParamValue(width, 0);

    ::glLineWidthx(width);

    return WriteReply();
}
int OGLES11Wrapper::glLoadIdentity()
{
    ::glLoadIdentity();
    return WriteReply();
}
int OGLES11Wrapper::glLoadMatrixf()
{
    GLfloat *m;

    TInt size0;
    m_currentFunctionCall.GetVectorData(m, size0, 0);

    ::glLoadMatrixf(m);

    return WriteReply();
}
int OGLES11Wrapper::glLoadMatrixx()
{
    GLfixed *m;

    TInt size0;
    m_currentFunctionCall.GetVectorData(m, size0, 0);

    ::glLoadMatrixx(m);
    return WriteReply();
}
int OGLES11Wrapper::glLogicOp()
{
    GLenum opcode;

    m_currentFunctionCall.GetParamValue(opcode, 0);

    ::glLogicOp(opcode);

    return WriteReply();
}
int OGLES11Wrapper::glMaterialf()
{
    GLenum face;
    GLenum pname;
    GLfloat param;

    m_currentFunctionCall.GetParamValue(face, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);

    ::glMaterialf(face, pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glMaterialfv()
{
    GLenum face;
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(face, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    ::glMaterialfv(face, pname, params);

    return WriteReply();
}
int OGLES11Wrapper::glMaterialx()
{
    GLenum face;
    GLenum pname;
    GLfixed param;

    m_currentFunctionCall.GetParamValue(face, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);

    ::glMaterialx(face, pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glMaterialxv()
{
    GLenum face;
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(face, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    TInt size2;
    m_currentFunctionCall.GetVectorData(params, size2, 2);

    ::glMaterialxv(face, pname, params);

    return WriteReply();
}
int OGLES11Wrapper::glMatrixMode()
{
    GLenum mode;

    m_currentFunctionCall.GetParamValue(mode, 0);

    ::glMatrixMode(mode);

    return WriteReply();
}
int OGLES11Wrapper::glMultMatrixf()
{
    GLfloat *m;

    TInt size0;
    m_currentFunctionCall.GetVectorData(m, size0, 0);

    ::glMultMatrixf(m);

    return WriteReply();
}
int OGLES11Wrapper::glMultMatrixx()
{
    GLfixed *m;

    TInt size0;
    m_currentFunctionCall.GetVectorData(m, size0, 0);

    ::glMultMatrixx(m);

    return WriteReply();
}
int OGLES11Wrapper::glMultiTexCoord4f()
{
    GLenum target;
    GLfloat s;
    GLfloat t;
    GLfloat r;
    GLfloat q;

    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(s, 1);
    m_currentFunctionCall.GetParamValue(t, 2);
    m_currentFunctionCall.GetParamValue(r, 3);
    m_currentFunctionCall.GetParamValue(q, 4);

    ::glMultiTexCoord4f(target, s, t, r, q);

    return WriteReply();
}
int OGLES11Wrapper::glMultiTexCoord4x()
{
    GLenum target;
    GLfixed s;
    GLfixed t;
    GLfixed r;
    GLfixed q;

    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(s, 1);
    m_currentFunctionCall.GetParamValue(t, 2);
    m_currentFunctionCall.GetParamValue(r, 3);
    m_currentFunctionCall.GetParamValue(q, 4);

    ::glMultiTexCoord4x(target, s, t, r, q);

    return WriteReply();
}
int OGLES11Wrapper::glNormal3f()
{
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;

    m_currentFunctionCall.GetParamValue(nx, 0);
    m_currentFunctionCall.GetParamValue(ny, 1);
    m_currentFunctionCall.GetParamValue(nz, 2);

    ::glNormal3f(nx, ny, nz);

    return WriteReply();
}
int OGLES11Wrapper::glNormal3x()
{
    GLfixed nx;
    GLfixed ny;
    GLfixed nz;

    m_currentFunctionCall.GetParamValue(nx, 0);
    m_currentFunctionCall.GetParamValue(ny, 1);
    m_currentFunctionCall.GetParamValue(nz, 2);

    ::glNormal3x(nx, ny, nz);

    return WriteReply();
}

int OGLES11Wrapper::glOrthof()
{
    GLfloat left;
    GLfloat right;
    GLfloat bottom;
    GLfloat top;
    GLfloat zNear;
    GLfloat zFar;

    m_currentFunctionCall.GetParamValue(left, 0);
    m_currentFunctionCall.GetParamValue(right, 1);
    m_currentFunctionCall.GetParamValue(bottom, 2);
    m_currentFunctionCall.GetParamValue(top, 3);
    m_currentFunctionCall.GetParamValue(zNear, 4);
    m_currentFunctionCall.GetParamValue(zFar, 5);

    ::glOrthof(left, right, bottom, top, zNear, zFar);

    return WriteReply();
}
int OGLES11Wrapper::glOrthox()
{
    GLfixed left;
    GLfixed right;
    GLfixed bottom;
    GLfixed top;
    GLfixed zNear;
    GLfixed zFar;

    m_currentFunctionCall.GetParamValue(left, 0);
    m_currentFunctionCall.GetParamValue(right, 1);
    m_currentFunctionCall.GetParamValue(bottom, 2);
    m_currentFunctionCall.GetParamValue(top, 3);
    m_currentFunctionCall.GetParamValue(zNear, 4);
    m_currentFunctionCall.GetParamValue(zFar, 5);

    ::glOrthox(left, right, bottom, top, zNear, zFar);

    return WriteReply();
}
int OGLES11Wrapper::glPixelStorei()
{
    GLenum pname;
    GLint param;

    m_currentFunctionCall.GetParamValue(pname, 0);
    m_currentFunctionCall.GetParamValue(param, 1);

    ::glPixelStorei(pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glPointParameterf()
{
    GLenum pname;
    GLfloat param;

    m_currentFunctionCall.GetParamValue(pname, 0);
    m_currentFunctionCall.GetParamValue(param, 1);

    ::glPointParameterf(pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glPointParameterfv()
{
    GLenum pname;
    GLfloat *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);

    ::glPointParameterfv(pname, params);

    return WriteReply();
}
int OGLES11Wrapper::glPointParameterx()
{
    GLenum pname;
    GLfixed param;

    m_currentFunctionCall.GetParamValue(pname, 0);
    m_currentFunctionCall.GetParamValue(param, 1);

    ::glPointParameterx(pname, param);

    return WriteReply();
}
int OGLES11Wrapper::glPointParameterxv()
{
    GLenum pname;
    GLfixed *params;

    m_currentFunctionCall.GetParamValue(pname, 0);
    TInt size1;
    m_currentFunctionCall.GetVectorData(params, size1, 1);

    ::glPointParameterxv(pname, params);

    return WriteReply();
}
int OGLES11Wrapper::glPointSize()
{
    GLfloat size;

    m_currentFunctionCall.GetParamValue(size, 0);

    ::glPointSize(size);

    return WriteReply();
}
int OGLES11Wrapper::glPointSizex()
{
    GLfixed size;

    m_currentFunctionCall.GetParamValue(size, 0);

    ::glPointSizex(size);

    return WriteReply();
}
int OGLES11Wrapper::glPolygonOffset()
{
    GLfloat factor;
    GLfloat units;

    m_currentFunctionCall.GetParamValue(factor, 0);
    m_currentFunctionCall.GetParamValue(units, 1);

    ::glPolygonOffset(factor, units);

    return WriteReply();
}
int OGLES11Wrapper::glPolygonOffsetx()
{
    GLfixed factor;
    GLfixed units;

    m_currentFunctionCall.GetParamValue(factor, 0);
    m_currentFunctionCall.GetParamValue(units, 1);

    ::glPolygonOffsetx(factor, units);

    return WriteReply();
}
int OGLES11Wrapper::glPopMatrix()
{
    ::glPopMatrix();
    return WriteReply();
}
int OGLES11Wrapper::glPushMatrix()
{
    ::glPushMatrix();
    return WriteReply();
}
int OGLES11Wrapper::glReadPixels()
{
    GLint x;
    GLint y;
    GLsizei width;
    GLsizei height;
    GLenum format;
    GLenum type;
    GLvoid *pixels;

    m_currentFunctionCall.GetParamValue(x, 0);
    m_currentFunctionCall.GetParamValue(y, 1);
    m_currentFunctionCall.GetParamValue(width, 2);
    m_currentFunctionCall.GetParamValue(height, 3);
    m_currentFunctionCall.GetParamValue(format, 4);
    m_currentFunctionCall.GetParamValue(type, 5);
    TInt size6;
    m_currentFunctionCall.GetVectorData(pixels, size6, 6);

    pixels = (GLvoid*)iStack->AllocFromStack( size6*m_currentFunctionCall.GetTypeSize( OpenGlES11RFC::EGLvoid ),
        m_currentFunctionCall.GetTypeAlignment( OpenGlES11RFC::EGLvoid ) );

    ::glReadPixels(x, y, width, height, format, type, pixels);

    m_currentFunctionCall.SetVectorData( pixels, size6, 6 ); // EOut    

    int ret = WriteReply();
    iStack->ClearStack();
    pixels = NULL;
    return ret;
}
int OGLES11Wrapper::glRotatef()
{
    GLfloat angle, x, y, z;
    m_currentFunctionCall.GetParamValue(angle, 0);
    m_currentFunctionCall.GetParamValue(x, 1);
    m_currentFunctionCall.GetParamValue(y, 2);
    m_currentFunctionCall.GetParamValue(z, 3);
    ::glRotatef(angle, x, y, z);
    return WriteReply();
}
int OGLES11Wrapper::glRotatex()
{
    GLfixed angle, x, y, z;
    m_currentFunctionCall.GetParamValue(angle, 0);
    m_currentFunctionCall.GetParamValue(x, 1);
    m_currentFunctionCall.GetParamValue(y, 2);
    m_currentFunctionCall.GetParamValue(z, 3);
    ::glRotatex(angle, x, y, z);
    return WriteReply();
}
int OGLES11Wrapper::glSampleCoverage()
{
    GLclampf value;
    GLboolean invert;
    m_currentFunctionCall.GetParamValue(value, 0);
    m_currentFunctionCall.GetParamValue(invert, 1);
    ::glSampleCoverage(value, invert);
    return WriteReply();
}
int OGLES11Wrapper::glSampleCoveragex()
{
    GLclampx value;
    GLboolean invert;
    m_currentFunctionCall.GetParamValue(value, 0);
    m_currentFunctionCall.GetParamValue(invert, 1);
    ::glSampleCoveragex(value, invert);
    return WriteReply();
}
int OGLES11Wrapper::glScalef()
{
    GLfloat x, y, z;
    m_currentFunctionCall.GetParamValue(x, 0);
    m_currentFunctionCall.GetParamValue(y, 1);
    m_currentFunctionCall.GetParamValue(z, 2);
    ::glScalef(x, y, z);
    return WriteReply();
}
int OGLES11Wrapper::glScalex()
{
    GLfixed x, y, z;
    m_currentFunctionCall.GetParamValue(x, 0);
    m_currentFunctionCall.GetParamValue(y, 1);
    m_currentFunctionCall.GetParamValue(z, 2);
    ::glScalex(x, y, z);
    return WriteReply();
}
int OGLES11Wrapper::glScissor()
{
    GLint x, y;
    GLsizei width, height;
    m_currentFunctionCall.GetParamValue(x, 0);
    m_currentFunctionCall.GetParamValue(y, 1);
    m_currentFunctionCall.GetParamValue(width, 2);
    m_currentFunctionCall.GetParamValue(height, 3);
    ::glScissor(x, y, width, height);
    return WriteReply();
}
int OGLES11Wrapper::glShadeModel()
{
    GLenum mode;
    m_currentFunctionCall.GetParamValue(mode, 0);
    ::glShadeModel(mode);
    return WriteReply();
}
int OGLES11Wrapper::glStencilFunc()
{
    GLenum func;
    GLint ref;
    GLuint mask;
    m_currentFunctionCall.GetParamValue(func, 0);
    m_currentFunctionCall.GetParamValue(ref, 1);
    m_currentFunctionCall.GetParamValue(mask, 2);
    ::glStencilFunc(func, ref, mask);
    return WriteReply();
}
int OGLES11Wrapper::glStencilMask()
{
    GLuint mask;
    m_currentFunctionCall.GetParamValue(mask, 0);
    ::glStencilMask(mask);
    return WriteReply();
}
int OGLES11Wrapper::glStencilOp()
{
    GLenum fail, zfail, zpass;
    m_currentFunctionCall.GetParamValue(fail, 0);
    m_currentFunctionCall.GetParamValue(zfail, 1);
    m_currentFunctionCall.GetParamValue(zpass, 2);
    ::glStencilOp (fail, zfail, zpass);
    return WriteReply();
}
int OGLES11Wrapper::glTexEnvf()
{
    GLenum target, pname;
    GLfloat param;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);
    ::glTexEnvf(target, pname, param);
    return WriteReply();
}
int OGLES11Wrapper::glTexEnvfv()
{
    GLenum target, pname;
    GLfloat *params;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);

    TInt size;
    m_currentFunctionCall.GetVectorData(params, size, 2);

    ::glTexEnvfv(target, pname, params);
    return WriteReply();
}
int OGLES11Wrapper::glTexEnvi()
{
    GLenum target, pname;
    GLint param;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);
    ::glTexEnvi(target, pname, param);
    return WriteReply();
}
int OGLES11Wrapper::glTexEnviv()
{
    GLenum target, pname;
    GLint *params;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);

    TInt size;
    m_currentFunctionCall.GetVectorData(params, size, 2);

    ::glTexEnviv(target, pname, params);
    return WriteReply();
}
int OGLES11Wrapper::glTexEnvx()
{
    GLenum target, pname;
    GLfixed param;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);
    ::glTexEnvx(target, pname, param);
    return WriteReply();
}
int OGLES11Wrapper::glTexEnvxv()
{
    GLenum target, pname;
    GLfixed *params;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);

    TInt size;
    m_currentFunctionCall.GetVectorData(params, size, 2);

    ::glTexEnvxv(target, pname, params);
    return WriteReply();
}
int OGLES11Wrapper::glTexImage2D()
{
    GLenum target;
    GLint level;
    GLint internalformat;
    GLsizei width;
    GLsizei height;
    GLint border;
    GLenum format;
    GLenum type;
    GLvoid *pixels;

    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(level, 1);
    m_currentFunctionCall.GetParamValue(internalformat, 2);
    m_currentFunctionCall.GetParamValue(width, 3);
    m_currentFunctionCall.GetParamValue(height, 4);
    m_currentFunctionCall.GetParamValue(border, 5);
    m_currentFunctionCall.GetParamValue(format, 6);
    m_currentFunctionCall.GetParamValue(type, 7);

    TInt size;
    m_currentFunctionCall.GetVectorData(pixels, size, 8);

    ::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    return WriteReply();
}
int OGLES11Wrapper::glTexParameterf()
{
    GLenum target, pname;
    GLfloat param;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);
    ::glTexParameterf(target, pname, param);
    return WriteReply();
}
int OGLES11Wrapper::glTexParameterfv()
{
    GLenum target, pname;
    GLfloat *params;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);

    TInt size;
    m_currentFunctionCall.GetVectorData(params, size, 2);

    ::glTexParameterfv(target, pname, params);
    return WriteReply();
}
int OGLES11Wrapper::glTexParameteri()
{
    GLenum target, pname;
    GLint param;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);
    ::glTexParameteri(target, pname, param);
    return WriteReply();
}
int OGLES11Wrapper::glTexParameteriv()
{
    GLenum target, pname;
    GLint *params;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);

    TInt size;
    m_currentFunctionCall.GetVectorData(params, size, 2);

    ::glTexParameteriv(target, pname, params);
    return WriteReply();
}
int OGLES11Wrapper::glTexParameterx()
{
    GLenum target, pname;
    GLfixed param;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);
    m_currentFunctionCall.GetParamValue(param, 2);
    ::glTexParameterx(target, pname, param);
    return WriteReply();
}
int OGLES11Wrapper::glTexParameterxv()
{
    GLenum target, pname;
    GLfixed *params;
    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(pname, 1);

    TInt size;
    m_currentFunctionCall.GetVectorData(params, size, 2);

    ::glTexParameterxv(target, pname, params);
    return WriteReply();
}
int OGLES11Wrapper::glTexSubImage2D()
{
    GLenum target;
    GLint level;
    GLint xoffset;
    GLint yoffset;
    GLsizei width;
    GLsizei height;
    GLenum format;
    GLenum type;
    GLvoid *pixels;

    m_currentFunctionCall.GetParamValue(target, 0);
    m_currentFunctionCall.GetParamValue(level, 1);
    m_currentFunctionCall.GetParamValue(xoffset, 2);
    m_currentFunctionCall.GetParamValue(yoffset, 3);
    m_currentFunctionCall.GetParamValue(width, 4);
    m_currentFunctionCall.GetParamValue(height, 5);
    m_currentFunctionCall.GetParamValue(format, 6);
    m_currentFunctionCall.GetParamValue(type, 7);
    TInt size8;
    m_currentFunctionCall.GetVectorData(pixels, size8, 8);

    ::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    return WriteReply();
}
int OGLES11Wrapper::glTranslatef()
{
    GLfloat x, y, z;
    m_currentFunctionCall.GetParamValue(x, 0);
    m_currentFunctionCall.GetParamValue(y, 1);
    m_currentFunctionCall.GetParamValue(z, 2);
    ::glTranslatef(x, y, z);
    return WriteReply();
}
int OGLES11Wrapper::glTranslatex()
{
    GLfixed x, y, z;
    m_currentFunctionCall.GetParamValue(x, 0);
    m_currentFunctionCall.GetParamValue(y, 1);
    m_currentFunctionCall.GetParamValue(z, 2);
    ::glTranslatex(x, y, z);
    return WriteReply();
}
int OGLES11Wrapper::glViewport()
{
    GLint x, y;
    GLsizei width, height;
    m_currentFunctionCall.GetParamValue(x, 0);
    m_currentFunctionCall.GetParamValue(y, 1);
    m_currentFunctionCall.GetParamValue(width, 2);
    m_currentFunctionCall.GetParamValue(height, 3);
    ::glViewport(x, y, width, height);
    return WriteReply();
}
int OGLES11Wrapper::glCurrentPaletteMatrixOES()
{
    return 0;
}
int OGLES11Wrapper::glDrawTexfOES()
{
    return 0;
}
int OGLES11Wrapper::glDrawTexfvOES()
{
    return 0;
}
int OGLES11Wrapper::glDrawTexiOES()
{
    return 0;
}
int OGLES11Wrapper::glDrawTexivOES()
{
    return 0;
}
int OGLES11Wrapper::glDrawTexsOES()
{
    return 0;
}
int OGLES11Wrapper::glDrawTexsvOES()
{
    return 0;
}
int OGLES11Wrapper::glDrawTexxOES()
{
    return 0;
}
int OGLES11Wrapper::glDrawTexxvOES()
{
    return 0;
}
int OGLES11Wrapper::glLoadPaletteFromModelViewMatrixOES()
{
    return 0;
}
int OGLES11Wrapper::glQueryMatrixxOES()
{
    return 0;
}
