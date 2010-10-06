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

#ifndef OPENGLES11APIWRAPPER_H
#define OPENGLES11APIWRAPPER_H

#ifndef PSU_PLATFORMTYPES_H
#include "platformtypes.h"
#endif

#include "opengles11rfc.h"
#include "apiwrapper.h"

class MGraphicsVHWCallback;

/* vertex array pointers */
enum
{
    COLOR_ARRAY,
    NORMAL_ARRAY,
    POINT_SIZE_ARRAY,
    TEX_COORD_ARRAY,
    VERTEX_ARRAY,
    MATRIX_INDEX_ARRAY,
    WEIGHT_ARRAY,
    NUM_ARRAYS
};
typedef struct
{
    GLint size;
    GLenum type;
    GLsizei stride;
    GLvoid *pointer;
} vertexArrayInfo;

class OGLES11Wrapper: public APIWrapper
{
public:
    OGLES11Wrapper(RemoteFunctionCallData& currentFunctionCallData,
            APIWrapperStack* stack,
            void* result,
            MGraphicsVHWCallback* serviceIf);

    void SetProcessInformation(TUint32 aProcess, TUint32 aThread) {}
    void Cleanup(TUint32 aProcess, TUint32 aThread) {}
    int DispatchRequest(unsigned long aCode);

    int WriteReply();
private:
    int m_lastGlError;
    OpenGlES11RFC m_currentFunctionCall;

    // Here follows the functions that map to the GL calls

    int glActiveTexture();
    int glAlphaFunc();
    int glAlphaFuncx();
    int glBindBuffer();
    int glBindTexture();
    int glBlendFunc();
    int glBufferData();
    int glBufferSubData();
    int glClear();
    int glClearColor();
    int glClearColorx();
    int glClearDepthf();
    int glClearDepthx();
    int glClearStencil();
    int glClientActiveTexture();
    int glClipPlanef();
    int glClipPlanex();
    int glColor4f();
    int glColor4ub();
    int glColor4x();
    int glColorMask();
    int glColorPointer();
    int glCompressedTexImage2D();
    int glCompressedTexSubImage2D();
    int glCopyTexImage2D();
    int glCopyTexSubImage2D();
    int glCullFace();
    int glDeleteBuffers();
    int glDeleteTextures();
    int glDepthFunc();
    int glDepthMask();
    int glDepthRangef();
    int glDepthRangex();
    int glDisable();
    int glDisableClientState();
    int glDrawArrays();
    int glDrawElements();
    int glEnable();
    int glEnableClientState();
    int glFinish();
    int glFlush();
    int glFogf();
    int glFogfv();
    int glFogx();
    int glFogxv();
    int glFrontFace();
    int glFrustumf();
    int glFrustumx();
    int glGenBuffers();
    int glGenTextures();
    int glGetBooleanv();
    int glGetBufferParameteriv();
    int glGetClipPlanef();
    int glGetClipPlanex();
    int glGetError();
    int glGetFixedv();
    int glGetFloatv();
    int glGetIntegerv();
    int glGetLightfv();
    int glGetLightxv();
    int glGetMaterialfv();
    int glGetMaterialxv();
    int glGetPointerv();
    int glGetString();
    int glGetTexEnvfv();
    int glGetTexEnviv();
    int glGetTexEnvxv();
    int glGetTexParameterfv();
    int glGetTexParameteriv();
    int glGetTexParameterxv();
    int glHint();
    int glIsBuffer();
    int glIsEnabled();
    int glIsTexture();
    int glLightModelf();
    int glLightModelfv();
    int glLightModelx();
    int glLightModelxv();
    int glLightf();
    int glLightfv();
    int glLightx();
    int glLightxv();
    int glLineWidth();
    int glLineWidthx();
    int glLoadIdentity();
    int glLoadMatrixf();
    int glLoadMatrixx();
    int glLogicOp();
    int glMaterialf();
    int glMaterialfv();
    int glMaterialx();
    int glMaterialxv();
    int glMatrixMode();
    int glMultMatrixf();
    int glMultMatrixx();
    int glMultiTexCoord4f();
    int glMultiTexCoord4x();
    int glNormal3f();
    int glNormal3x();
    int glNormalPointer();
    int glOrthof();
    int glOrthox();
    int glPixelStorei();
    int glPointParameterf();
    int glPointParameterfv();
    int glPointParameterx();
    int glPointParameterxv();
    int glPointSize();
    int glPointSizex();
    int glPolygonOffset();
    int glPolygonOffsetx();
    int glPopMatrix();
    int glPushMatrix();
    int glReadPixels();
    int glRotatef();
    int glRotatex();
    int glSampleCoverage();
    int glSampleCoveragex();
    int glScalef();
    int glScalex();
    int glScissor();
    int glShadeModel();
    int glStencilFunc();
    int glStencilMask();
    int glStencilOp();
    int glTexCoordPointer();
    int glTexEnvf();
    int glTexEnvfv();
    int glTexEnvi();
    int glTexEnviv();
    int glTexEnvx();
    int glTexEnvxv();
    int glTexImage2D();
    int glTexParameterf();
    int glTexParameterfv();
    int glTexParameteri();
    int glTexParameteriv();
    int glTexParameterx();
    int glTexParameterxv();
    int glTexSubImage2D();
    int glTranslatef();
    int glTranslatex();
    int glVertexPointer();
    int glViewport();
    int glCurrentPaletteMatrixOES();
    int glDrawTexfOES();
    int glDrawTexfvOES();
    int glDrawTexiOES();
    int glDrawTexivOES();
    int glDrawTexsOES();
    int glDrawTexsvOES();
    int glDrawTexxOES();
    int glDrawTexxvOES();
    int glLoadPaletteFromModelViewMatrixOES();
    int glQueryMatrixxOES();

};

#endif
