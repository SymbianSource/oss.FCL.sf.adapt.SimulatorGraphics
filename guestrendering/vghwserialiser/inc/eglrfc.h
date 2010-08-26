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

#ifndef EGLREMOTEFUNCTIONCALL_H_
#define EGLREMOTEFUNCTIONCALL_H_

#include "remotefunctioncall.h"

#ifdef __SYMBIAN32__
	#include <e32const.h>
	#include <e32def.h>
	#include <EGL/egl.h>
#else
	#include "EGL/egl.h"
#endif


enum TSelectionCriteria
    {
    EExact,
    EAtLeast,
    EMask,
    ESpecial //This essentially means, don't compare
    };

//For adding a sync value to the egl calls
const TInt KSyncReadSurface = 2;
const TInt KSyncDrawSurface = 4;

//This is the maximum number of configs that can be processed
//when the client asks for sg-compatible or sg-incompatible configs
const TInt KConfigsMaxCnt = 200;

enum TMetaGetConfigsMode
    {
    EMetaGetConfigsAll,   //get all the configs available
    EMetaGetConfigsSg,    //get configs supporting sg images
    EMetaGetConfigsNonSg, //get configs not supporting sg images
    };

NONSHARABLE_CLASS(EglRFC): public RemoteFunctionCall
    {
public:
    // EGL operation code
    enum TOperationCode
        {
        // EGL 1.4
        EeglGetError, // 0
        EeglGetDisplay,
        EeglInitialize,
        EeglTerminate,
        EeglQueryString, // not used
        EeglGetConfigs,
        EeglChooseConfig,
        EeglGetConfigAttrib,
        EeglCreateWindowSurface,
        EeglCreatePbufferSurface,
        EeglCreatePixmapSurface, // 10
        EeglDestroySurface,
        EeglQuerySurface,
        EeglBindAPI,
        EeglQueryAPI,
        EeglWaitClient,
        EeglReleaseThread,
        EeglCreatePbufferFromClientBuffer,
        EeglSurfaceAttrib,
        EeglBindTexImage,
        EeglReleaseTexImage, // 20
        EeglSwapInterval,
        EeglCreateContext,
        EeglDestroyContext,
        EeglMakeCurrent,
        EeglGetCurrentContext,
        EeglGetCurrentSurface,
        EeglGetCurrentDisplay,
        EeglQueryContext,
        EeglWaitGL,
        EeglWaitNative, // 30
        EeglSwapBuffers,
        EeglCopyBuffers,
        EeglSimulatorSetSurfaceParams,
        EeglSimulatorCopyImageData,
        EeglPixmapSurfaceSizeChanged,
        EeglMakeCurrentSg, //sgImage support
        EeglCreatePixmapSurfaceSg,
        EeglMetaGetConfigs,
        EeglMetaSgGetHandles
        };

	//Parameter data type
    enum TParamType
        {        
        EEGLBoolean,
        EEGLint,
        EEGLenum,
        EEGLDisplay,
        EEGLConfig,
        EEGLSurface,
        EEGLContext,
        EEGLClientBuffer,
        EEGLImageKHR,
        EEGLNativeDisplayType,
        EEGLNativeWindowType,
        EEGLNativePixmapType,
        EEGLVoid,
        ETUint64
        };

    EglRFC( RemoteFunctionCallData& aData );
    
    inline void AppendTUint64( const TUint64& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
	inline void AppendEGLBoolean( const EGLBoolean& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLint( const EGLint& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLenum( const EGLenum& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLDisplay( const EGLDisplay& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLConfig( const EGLConfig& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLSurface( const EGLSurface& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLContext( const EGLContext& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLClientBuffer( const EGLClientBuffer& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLImageKHR( const EGLImageKHR& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLNativeDisplayType( const NativeDisplayType& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLNativeWindowType( const NativeWindowType& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLNativePixmapType( const NativePixmapType& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
#ifdef __SYMBIAN32__
    inline void AppendEGLSize( const TSize& aSize, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
#endif

    //Gets a simple parameter value from parameter array
    inline void GetTUint64( TUint64& aParam, TInt aIndex );
    inline void GetEGLBoolean( EGLBoolean& aParam, TInt aIndex );
    inline void GetEGLint( EGLint& aParam, TInt aIndex );
    inline void GetEGLenum( EGLenum& aParam, TInt aIndex );
    inline void GetEGLDisplay( EGLDisplay& aParam, TInt aIndex );
    inline void GetEGLConfig( EGLConfig& aParam, TInt aIndex );
    inline void GetEGLSurface( EGLSurface& aParam, TInt aIndex );
    inline void GetEGLContext( EGLContext& aParam, TInt aIndex );
    inline void GetEGLClientBuffer( EGLClientBuffer& aParam, TInt aIndex );
    inline void GetEGLImageKHR( EGLImageKHR& aParam, TInt aIndex );
    inline void GetEGLNativeDisplayType( NativeDisplayType& aParam, TInt aIndex );
    inline void GetEGLNativeWindowType( NativeWindowType& aParam, TInt aIndex );
    inline void GetEGLNativePixmapType( NativePixmapType& aParam, TInt aIndex );

    //Gets a pointer to an array parameter from parameter array
    inline void GetEGLintVectorData( EGLint* &aData, TInt& aSize, TInt aIndex );
    inline void GetEGLConfigVectorData( EGLConfig* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( void* &aData, TInt& aSize, TInt aIndex );

    //Sets a pointer to the data of an array parameter
    inline void SetEGLintVectorData( const EGLint* aData, TInt aLength, TInt aIndex );
    inline void SetEGLConfigVectorData( const EGLConfig* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const void* aData, TInt aLength, TInt aIndex );

    //Appends an array parameter to the parameter array
    // Does not serialize any data
    inline void AppendEGLintVector( const EGLint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendEGLConfigVector( const EGLConfig* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const void* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );

	//Size of type aType
	virtual TInt GetTypeSize( TInt32 aParamType ) const;

    //Alignemnt of type aType
	virtual TInt GetTypeAlignment( TInt32 aParamType ) const;

    //Alignemnt of type aType
	virtual TUint32 GetAPIUid() const;

	static EGLint MetaGetConfigAttribute( int aIndex );
	static EGLint MetaGetConfigAttributeSelCriteria( int aIndex );
	static TInt MetaGetConfigAttributeCnt();
    };

#include "eglrfc.inl"

#endif
