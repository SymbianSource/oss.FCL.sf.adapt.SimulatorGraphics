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


#ifndef DRIVERREMOTEFUNCTIONCALL_H_
#define DRIVERREMOTEFUNCTIONCALL_H_

#include "remotefunctioncall.h"
#include "serialisedapiuids.h"

#include "remotefunctioncall.h"

#ifdef __SYMBIAN32__
    #include <e32const.h>
    #include <e32def.h>
    #include <EGL/egl.h>
#else
    #include "EGL/egl.h"
    #include "EGL/eglext.h"
#endif

#ifdef __SYMBIAN32__
    #define __VG_OPENVG_H_
    #include <VG/1.1/openvg.h>
    #include <VG/vgu.h>
#else
    #define __OPENVGHEADERS_USE_VG_1_1
    #include <VG/openvg.h>
    #include "VG/vgu.h"
#endif

class DriverRFC: public RemoteFunctionCall
    {
public:
    //Openvg pperation code
    enum TOperationCode
        {
        EDrvClientShutdown,
        EDrvCreatePoolContext, //for creating the pool context hostside
        EDrvCreatePbufferSg,
        EDrvCreateVGImageSg,
        EDrvDeleteSgImage,
        EDrvSyncVGImageFromPBuffer
        };

	//Parameter data type
    enum TParamType
        {        
        EPTVoid,
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
        ETUint32,
        //VG parameters
        EVGfloat,
        EVGbyte,
        EVGubyte,
        EVGshort,
        EVGint,
        EVGuint,
        EVGbitfield,
        EVGboolean,
        EVGHandle,
        EVGErrorCode,
        EVGParamType, //10
        EVGRenderingQuality,
        EVGPixelLayout,
        EVGMatrixMode,
        EVGMaskOperation,
        EVGPathDatatype,
        EVGPathAbsRel,
        EVGPathSegment,
        EVGPathCommand,
        EVGPath,
        EVGPathCapabilities, //20
        EVGPathParamType,
        EVGCapStyle,
        EVGJoinStyle,
        EVGFillRule,
        EVGPaintMode,
        EVGPaint,
        EVGPaintParamType,
        EVGPaintType,
        EVGColorRampSpreadMode,
        EVGTilingMode, //30
        EVGImageFormat,
        EVGImage,
        EVGImageQuality,
        EVGImageParamType,
        EVGImageMode,
        EVGImageChannel,
        EVGBlendMode,
        EVGHardwareQueryType,
        EVGHardwareQueryResult,
        EVGStringID, //40
        EVGvoid,
        EVGUArcType,
        EVGeglImageKHR
        };

    DriverRFC( RemoteFunctionCallData& aData ):
        RemoteFunctionCall( aData )
        {
        }
    
	//Size of type aType
	TInt GetTypeSize( TInt32 aParamType ) const;

    //Alignemnt of type aType
	TInt GetTypeAlignment( TInt32 aParamType ) const;

    //Alignemnt of type aType
	TUint32 GetAPIUid() const { return SERIALISED_DRIVER_API_UID; }
	
	
	
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
    
    
     inline void AppendTUint32( const TUint32& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
     inline void GetTUint32( TUint32& aParam, TInt aIndex );
    
//    
    //VG parameters
    //Appends a simple parameter to the parameter array
    // Does not serialize any data
    inline void AppendVGParam( const VGfloat& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGbyte& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGubyte& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGshort& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGint& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGuint& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGboolean& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGErrorCode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGParamType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGRenderingQuality& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGPixelLayout& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGMatrixMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGMaskOperation& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGPathDatatype& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGPathAbsRel& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGPathSegment& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGPathCommand& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );   
    inline void AppendVGParam( const VGPathCapabilities& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGPathParamType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGCapStyle& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGJoinStyle& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGFillRule& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGPaintMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGPaintParamType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGPaintType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGColorRampSpreadMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGTilingMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGImageFormat& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGImageQuality& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGImageParamType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGImageMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGImageChannel& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGBlendMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGHardwareQueryType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGHardwareQueryResult& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGStringID& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVGParam( const VGUArcType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
#ifdef __SYMBIAN32__
    inline void AppendVGParam( const VGeglImageKHR& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
#endif
        
    //Gets a simple parameter value from parameter array
    inline void GetVGParamValue( VGfloat& aParam, TInt aIndex );
    inline void GetVGParamValue( VGbyte& aParam, TInt aIndex );
    inline void GetVGParamValue( VGubyte& aParam, TInt aIndex );
    inline void GetVGParamValue( VGshort& aParam, TInt aIndex );
    inline void GetVGParamValue( VGint& aParam, TInt aIndex );
    inline void GetVGParamValue( VGuint& aParam, TInt aIndex );
    inline void GetVGParamValue( VGboolean& aParam, TInt aIndex );
    inline void GetVGParamValue( VGErrorCode& aParam, TInt aIndex );
    inline void GetVGParamValue( VGParamType& aParam, TInt aIndex );
    inline void GetVGParamValue( VGRenderingQuality& aParam, TInt aIndex );
    inline void GetVGParamValue( VGPixelLayout& aParam, TInt aIndex );
    inline void GetVGParamValue( VGMatrixMode& aParam, TInt aIndex );
    inline void GetVGParamValue( VGMaskOperation& aParam, TInt aIndex );
    inline void GetVGParamValue( VGPathDatatype& aParam, TInt aIndex );
    inline void GetVGParamValue( VGPathAbsRel& aParam, TInt aIndex );
    inline void GetVGParamValue( VGPathSegment& aParam, TInt aIndex );
    inline void GetVGParamValue( VGPathCommand& aParam, TInt aIndex );   
    inline void GetVGParamValue( VGPathCapabilities& aParam, TInt aIndex );
    inline void GetVGParamValue( VGPathParamType& aParam, TInt aIndex );
    inline void GetVGParamValue( VGCapStyle& aParam, TInt aIndex );
    inline void GetVGParamValue( VGJoinStyle& aParam, TInt aIndex );
    inline void GetVGParamValue( VGFillRule& aParam, TInt aIndex );
    inline void GetVGParamValue( VGPaintMode& aParam, TInt aIndex );
    inline void GetVGParamValue( VGPaintParamType& aParam, TInt aIndex );
    inline void GetVGParamValue( VGPaintType& aParam, TInt aIndex );
    inline void GetVGParamValue( VGColorRampSpreadMode& aParam, TInt aIndex );
    inline void GetVGParamValue( VGTilingMode& aParam, TInt aIndex );
    inline void GetVGParamValue( VGImageFormat& aParam, TInt aIndex );
    inline void GetVGParamValue( VGImageQuality& aParam, TInt aIndex );
    inline void GetVGParamValue( VGImageParamType& aParam, TInt aIndex );
    inline void GetVGParamValue( VGImageMode& aParam, TInt aIndex );
    inline void GetVGParamValue( VGImageChannel& aParam, TInt aIndex );
    inline void GetVGParamValue( VGBlendMode& aParam, TInt aIndex );
    inline void GetVGParamValue( VGHardwareQueryType& aParam, TInt aIndex );
    inline void GetVGParamValue( VGHardwareQueryResult& aParam, TInt aIndex );
    inline void GetVGParamValue( VGStringID& aParam, TInt aIndex );
    inline void GetVGParamValue( VGUArcType& aParam, TInt aIndex );
    

    };
#include "driverrfc.inl"

#endif
