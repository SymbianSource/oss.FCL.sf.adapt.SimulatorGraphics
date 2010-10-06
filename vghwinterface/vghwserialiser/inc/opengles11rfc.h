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

#ifndef OPENGLES11REMOTEFUNCTIONCALL_H_
#define OPENGLES11REMOTEFUNCTIONCALL_H_

#include "remotefunctioncall.h"

#ifdef __SYMBIAN32__
	#include <GLES/gl.h>
#else
	#include "GLES/gl.h"
	#include "GLES/glextplatform.h"
#endif

//OpenGlES11 remote function call
// Represents a function call and contains information about 
// parameters but does not serialize the data
NONSHARABLE_CLASS(OpenGlES11RFC): public RemoteFunctionCall
    {
public:

    //OpenEglES11 operation code
    enum TOperationCode
    {
        EglActiveTexture,
        EglAlphaFunc,
        EglAlphaFuncx,
        EglBindBuffer,
        EglBindTexture,
        EglBlendFunc,
        EglBufferData,
        EglBufferSubData,
        EglClear,
        EglClearColor,
        EglClearColorx, // 10
        EglClearDepthf,
        EglClearDepthx,
        EglClearStencil,
        EglClientActiveTexture,
        EglClipPlanef,
        EglClipPlanex,
        EglColor4f,
        EglColor4ub,
        EglColor4x,
        EglColorMask, // 20
        EglColorPointer,
        EglCompressedTexImage2D,
        EglCompressedTexSubImage2D,
        EglCopyTexImage2D,
        EglCopyTexSubImage2D,
        EglCullFace,
        EglDeleteBuffers,
        EglDeleteTextures,
        EglDepthFunc,
        EglDepthMask, // 30
        EglDepthRangef,
        EglDepthRangex,
        EglDisable,
        EglDisableClientState,
        EglDrawArrays,
        EglDrawElements,
        EglEnable,
        EglEnableClientState,
        EglFinish,
        EglFlush, // 40
        EglFogf,
        EglFogfv,
        EglFogx,
        EglFogxv,
        EglFrontFace,
        EglFrustumf,
        EglFrustumx,
        EglGenBuffers,
        EglGenTextures,
        EglGetBooleanv, // 50
        EglGetBufferParameteriv,
        EglGetClipPlanef,
        EglGetClipPlanex,
        EglGetError,
        EglGetFixedv,
        EglGetFloatv,
        EglGetIntegerv,
        EglGetLightfv,
        EglGetLightxv,
        EglGetMaterialfv, // 60
        EglGetMaterialxv,
        EglGetPointerv,
        EglGetString,
        EglGetTexEnvfv,
        EglGetTexEnviv,
        EglGetTexEnvxv,
        EglGetTexParameterfv,
        EglGetTexParameteriv,
        EglGetTexParameterxv,
        EglHint, // 70
        EglIsBuffer,
        EglIsEnabled,
        EglIsTexture,
        EglLightModelf,
        EglLightModelfv,
        EglLightModelx,
        EglLightModelxv,
        EglLightf,
        EglLightfv,
        EglLightx, // 80
        EglLightxv,
        EglLineWidth,
        EglLineWidthx,
        EglLoadIdentity,
        EglLoadMatrixf,
        EglLoadMatrixx,
        EglLogicOp,
        EglMaterialf,
        EglMaterialfv,
        EglMaterialx, // 90
        EglMaterialxv,
        EglMatrixMode,
        EglMultMatrixf,
        EglMultMatrixx,
        EglMultiTexCoord4f,
        EglMultiTexCoord4x,
        EglNormal3f,
        EglNormal3x,
        EglNormalPointer,
        EglOrthof, // 100
        EglOrthox,
        EglPixelStorei,
        EglPointParameterf,
        EglPointParameterfv,
        EglPointParameterx,
        EglPointParameterxv,
        EglPointSize,
        EglPointSizex,
        EglPolygonOffset,
        EglPolygonOffsetx, // 110
        EglPopMatrix,
        EglPushMatrix,
        EglReadPixels,
        EglRotatef,
        EglRotatex,
        EglSampleCoverage,
        EglSampleCoveragex,
        EglScalef,
        EglScalex,
        EglScissor, // 120
        EglShadeModel,
        EglStencilFunc,
        EglStencilMask,
        EglStencilOp,
        EglTexCoordPointer,
        EglTexEnvf,
        EglTexEnvfv,
        EglTexEnvi,
        EglTexEnviv,
        EglTexEnvx, // 130
        EglTexEnvxv,
        EglTexImage2D,
        EglTexParameterf,
        EglTexParameterfv,
        EglTexParameteri,
        EglTexParameteriv,
        EglTexParameterx,
        EglTexParameterxv,
        EglTexSubImage2D,
        EglTranslatef, // 140
        EglTranslatex,
        EglVertexPointer,
        EglViewport,
        // from Eglextplatform.h
        EglCurrentPaletteMatrixOES,
        EglDrawTexfOES,
        EglDrawTexfvOES,
        EglDrawTexiOES,
        EglDrawTexivOES,
        EglDrawTexsOES,
        EglDrawTexsvOES, // 150
        EglDrawTexxOES,
        EglDrawTexxvOES,
        EglLoadPaletteFromModelViewMatrixOES,
        EglMatrixIndexPointerOES,
        EglQueryMatrixxOES,
        EglWeightPointerOES,
        // client shutdown
        EClientShutdown

    };

    //Parameter data type
    enum TParamType
    {
        EGLbyte,
        EGLubyte,
        EGLshort,
        EGLushort,
        EGLint,
        EGLuint,
        EGLfloat,
        EGLvoid,
        // the following parameters are typedeffed to same types as the previous ones
        EGLboolean,
        EGLbitfield,
        EGLclampf,
        EGLclampx,
        EGLenum,
        EGLfixed,
        EGLintptr,
        EGLsizei,
        EGLsizeiptr
    };

    OpenGlES11RFC( RemoteFunctionCallData& aData );
    
    //Appends a simple parameter to the parameter array
    // Does not serialize any data
    inline void AppendParam( const GLbyte& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const GLubyte& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const GLshort& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const GLushort& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const GLint& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const GLuint& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const GLfloat& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
        
    //Gets a simple parameter value from parameter array
    inline void GetParamValue( GLbyte& aParam, TInt aIndex );
    inline void GetParamValue( GLubyte& aParam, TInt aIndex );
    inline void GetParamValue( GLshort& aParam, TInt aIndex );
    inline void GetParamValue( GLushort& aParam, TInt aIndex );
    inline void GetParamValue( GLint& aParam, TInt aIndex );
    inline void GetParamValue( GLuint& aParam, TInt aIndex );
    inline void GetParamValue( GLfloat& aParam, TInt aIndex );
    
    //Gets a pointer to an array parameter from parameter array
    inline void GetVectorData( GLint* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( GLuint* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( GLshort* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( GLfloat* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( GLboolean* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( void* &aData, TInt& aSize, TInt aIndex );

    //Sets a pointer to the data of an array parameter
    inline void SetVectorData( const GLint* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const GLuint* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const GLshort* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const GLfloat* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const GLboolean* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const void* aData, TInt aLength, TInt aIndex );

    //Appends an array parameter to the parameter array
    // Does not serialize any data
    inline void AppendVector( const GLint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const GLuint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const GLshort* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const GLfloat* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const GLboolean* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const void* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    
    //Size of type aType
    TInt GetTypeSize( TInt32 aParamType ) const;

    //Alignemnt of type aType
	TInt GetTypeAlignment( TInt32 aParamType ) const;

    //Alignemnt of type aType
	TUint32 GetAPIUid() const;
    };

#include "opengles11rfc.inl"
#endif
