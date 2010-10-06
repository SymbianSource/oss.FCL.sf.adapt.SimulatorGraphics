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

inline void DriverRFC::GetEGLBoolean( EGLBoolean& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLBoolean, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLint( EGLint& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLint, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLenum( EGLenum& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLenum, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLDisplay( EGLDisplay& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLDisplay, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLConfig( EGLConfig& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLConfig, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLSurface( EGLSurface& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLSurface, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLContext( EGLContext& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLContext, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLClientBuffer( EGLClientBuffer& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLClientBuffer, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLImageKHR( EGLImageKHR& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLImageKHR, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLNativeDisplayType( NativeDisplayType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLNativeDisplayType, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLNativeWindowType( NativeWindowType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLNativeWindowType, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLNativePixmapType( NativePixmapType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EEGLNativePixmapType, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetEGLintVectorData( EGLint* &aData, TInt& aSize, TInt aIndex )
    {
    iData.GetVectorData( EEGLint, (void**)&aData, aSize, aIndex );
    }

inline void DriverRFC::GetEGLConfigVectorData( EGLConfig* &aData, TInt& aSize, TInt aIndex )
    {
    iData.GetVectorData( EEGLConfig, (void**)&aData, aSize, aIndex );
    }

inline void DriverRFC::SetEGLintVectorData( const EGLint* aData, TInt aLength, TInt aIndex )
    {
    RemoteFunctionCall::SetVectorData( EEGLint, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
    }

inline void DriverRFC::SetEGLConfigVectorData( const EGLConfig* aData, TInt aLength, TInt aIndex )
    {
    RemoteFunctionCall::SetVectorData( EEGLConfig, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
    }
    

inline void DriverRFC::AppendEGLBoolean( const EGLBoolean& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLBoolean, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLint( const EGLint& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLint, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLenum( const EGLenum& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLenum, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLDisplay( const EGLDisplay& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLDisplay, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLConfig( const EGLConfig& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLConfig, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLSurface( const EGLSurface& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLSurface, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLContext( const EGLContext& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLContext, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLClientBuffer( const EGLClientBuffer& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLClientBuffer, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLImageKHR( const EGLImageKHR& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLImageKHR, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLNativeDisplayType( const NativeDisplayType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLNativeDisplayType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLNativeWindowType( const NativeWindowType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLNativeWindowType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLNativePixmapType( const NativePixmapType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLNativePixmapType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendEGLintVector( const EGLint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EEGLint, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

inline void DriverRFC::AppendEGLConfigVector( const EGLConfig* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EEGLConfig, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

#ifdef __SYMBIAN32__
inline void DriverRFC::AppendEGLSize( const TSize& aSize, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLint, reinterpret_cast<const TUint8*>(&aSize.iWidth), aDir );
    AppendParam( EEGLint, reinterpret_cast<const TUint8*>(&aSize.iHeight), aDir );
    }
#endif

inline void DriverRFC::AppendVector( const void* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EEGLVoid, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

inline void DriverRFC::GetVectorData( void* &aData, TInt& aSize, TInt aIndex )
    {
    iData.GetVectorData( EEGLVoid, &aData, aSize, aIndex );
    }

inline void DriverRFC::SetVectorData( const void* aData, TInt aLength, TInt aIndex )
    {
    RemoteFunctionCall::SetVectorData( EEGLVoid, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
    }


inline void DriverRFC::AppendTUint32( const TUint32& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( ETUint32, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::GetTUint32( TUint32& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::ETUint32, (TUint8*)&aParam, aIndex );
    }

//
//
//
inline void DriverRFC::GetVGParamValue( VGfloat& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGfloat, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGbyte& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGbyte, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGubyte& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGubyte, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGshort& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGshort, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGint& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGint, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGuint& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGuint, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGboolean& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGboolean, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGErrorCode& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGErrorCode, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGParamType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGParamType, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGRenderingQuality& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGRenderingQuality, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPixelLayout& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPixelLayout, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGMatrixMode& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGMatrixMode, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGMaskOperation& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGMaskOperation, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPathDatatype& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPathDatatype, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPathAbsRel& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPathAbsRel, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPathSegment& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPathSegment, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPathCommand& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPathCommand, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPathCapabilities& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPathCapabilities, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPathParamType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPathParamType, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGCapStyle& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGCapStyle, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGJoinStyle& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGJoinStyle, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGFillRule& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGFillRule, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPaintMode& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPaintMode, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPaintParamType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPaintParamType, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGPaintType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGPaintType, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGColorRampSpreadMode& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGColorRampSpreadMode, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGTilingMode& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGTilingMode, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGImageFormat& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGImageFormat, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGImageQuality& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGImageQuality, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGImageParamType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGImageParamType, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGImageMode& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGImageMode, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGImageChannel& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGImageChannel, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGBlendMode& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGBlendMode, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGHardwareQueryType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGHardwareQueryType, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGHardwareQueryResult& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGHardwareQueryResult, (TUint8*)&aParam, aIndex );
    }
inline void DriverRFC::GetVGParamValue( VGStringID& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGStringID, (TUint8*)&aParam, aIndex );
    }

inline void DriverRFC::GetVGParamValue( VGUArcType& aParam, TInt aIndex )
    {
    iData.GetParamValue( DriverRFC::EVGUArcType, (TUint8*)&aParam, aIndex );
    }
    
inline void DriverRFC::AppendVGParam( const VGfloat& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGfloat, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGbyte& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGbyte, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGubyte& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGubyte, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGshort& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGshort, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGint& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGint, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGuint& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGuint, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }


inline void DriverRFC::AppendVGParam( const VGboolean& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGboolean, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGErrorCode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGErrorCode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGParamType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGRenderingQuality& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGRenderingQuality, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPixelLayout& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPixelLayout, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGMatrixMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGMatrixMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGMaskOperation& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGMaskOperation, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPathDatatype& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathDatatype, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPathAbsRel& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathAbsRel, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPathSegment& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathSegment, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPathCommand& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathSegment, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPathCapabilities& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathCapabilities, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPathParamType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGCapStyle& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGCapStyle, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGJoinStyle& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGJoinStyle, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGFillRule& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGFillRule, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPaintMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPaintMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPaintParamType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPaintParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGPaintType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPaintParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGColorRampSpreadMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGColorRampSpreadMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGTilingMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGTilingMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGImageFormat& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageFormat, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGImageQuality& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageQuality, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGImageParamType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGImageMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGImageChannel& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageChannel, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGBlendMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGBlendMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGHardwareQueryType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGHardwareQueryType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGHardwareQueryResult& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGHardwareQueryResult, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGStringID& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGStringID, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void DriverRFC::AppendVGParam( const VGUArcType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGUArcType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

#ifdef __SYMBIAN32__
inline void DriverRFC::AppendVGParam( const VGeglImageKHR& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGeglImageKHR, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }
#endif


