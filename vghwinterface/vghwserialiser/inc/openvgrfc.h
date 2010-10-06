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

#ifndef OPENVGREMOTEFUNCTIONCALL_H_
#define OPENVGREMOTEFUNCTIONCALL_H_

#include "remotefunctioncall.h"

#ifdef __SYMBIAN32__
	#define __VG_OPENVG_H_
	#include <VG/1.1/openvg.h>
	#include <VG/vgu.h>
#else
    #define __OPENVGHEADERS_USE_VG_1_1
    #include <VG/openvg.h>
    #include "VG/vgu.h"
#endif

//OpenVG remote function call
// Represents a function call and contains information about 
// parameters but does not serialize the data
NONSHARABLE_CLASS(OpenVgRFC): public RemoteFunctionCall
    {
public:

    //Openvg pperation code
    enum TOperationCode
        {
        EvgGetError,
        EvgGetPaint,
        EvgCreatePaint,
        EvgInterpolatePath,
        EvgPathLength,
        EvgGetPathCapabilities,
        EvgCreatePath,
        EvgGetf,
        EvgGeti,
        EvgGetVectorSize,
        EvgGetParameterf, //10
        EvgGetParameteri,
        EvgGetParameterVectorSize,
        EvgCreateMaskLayer,
        EvgGetColor,
        EvgCreateImage,
        EvgChildImage,
        EvgGetParent, // now handled in Client DLL
        EvgCreateFont,
        EvgHardwareQuery,
        EvgGetParameterfv, //20
        EvgGetParameteriv,
        EvgGetString,
        EvgGetfv,
        EvgGetiv,
        EvgGetMatrix,
        EvgGetImageSubData,
        EvgReadPixels,
        EvgFlush,
        EvgFinish,
        EvgSetf, //30
        EvgSeti,
        EvgSetfv,
        EvgSetiv,
        EvgSetParameterf,
        EvgSetParameteri,
        EvgSetParameterfv,
        EvgSetParameteriv,
        EvgLoadIdentity,
        EvgLoadMatrix,
        EvgMultMatrix, //40
        EvgTranslate,
        EvgScale,
        EvgShear,
        EvgRotate,
        EvgMask,
        EvgRenderToMask,
        EvgDestroyMaskLayer,
        EvgFillMaskLayer,
        EvgCopyMask,
        EvgClear, //50
        EvgClearPath,
        EvgDestroyPath,
        EvgRemovePathCapabilities,
        EvgAppendPath,
        EvgAppendPathData, //55
        EvgModifyPathCoords,
        EvgTransformPath,
        EvgPointAlongPath,
        EvgPathBounds,
        EvgPathTransformedBounds, //60
        EvgDrawPath,
        EvgDestroyPaint,
        EvgSetPaint,
        EvgSetColor,
        EvgPaintPattern,
        EvgDestroyImage,
        EvgClearImage,
        EvgImageSubData,
        EvgCopyImage,
        EvgDrawImage,
        EvgSetPixels,
        EvgWritePixels,
        EvgGetPixels,
        EvgCopyPixels,
        EvgDestroyFont,
        EvgSetGlyphToPath,
        EvgSetGlyphToImage,
        EvgClearGlyph,
        EvgDrawGlyph,
        EvgDrawGlyphs, //80
        EvgColorMatrix,
        EvgConvolve,
        EvgSeparableConvolve,
        EvgGaussianBlur,
        EvgLookup,
        EvgLookupSingle,
		EvgePathCoordsSizeInBytes, //Not part of OpenVG 1.1
        //Openvgu 1.1
        EvguLine,
        EvguPolygon,
        EvguRect,
        EvguRoundRect,
        EvguEllipse,
        EvguArc,
        EvguComputeWarpQuadToSquare,
        EvguComputeWarpSquareToQuad,
        EvguComputeWarpQuadToQuad,
		EClientShutdown,
		// /* OpenVG extension #4, KHR_EGL_image */
		EvgCreateEGLImageTargetKHR,
        };

    //Parameter data type
    enum TParamType
        {
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
        EVGeglImageKHR,
        ETUint64
        };

    OpenVgRFC( RemoteFunctionCallData& aData );
    
    //Appends a simple parameter to the parameter array
    // Does not serialize any data
    inline void AppendTUint64( const TUint64& aParam, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGfloat& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGbyte& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGubyte& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGshort& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGint& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGuint& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGboolean& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGErrorCode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGParamType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGRenderingQuality& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGPixelLayout& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGMatrixMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGMaskOperation& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGPathDatatype& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGPathAbsRel& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGPathSegment& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGPathCommand& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );   
    inline void AppendParam( const VGPathCapabilities& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGPathParamType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGCapStyle& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGJoinStyle& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGFillRule& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGPaintMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGPaintParamType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGPaintType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGColorRampSpreadMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGTilingMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGImageFormat& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGImageQuality& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGImageParamType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGImageMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGImageChannel& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGBlendMode& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGHardwareQueryType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGHardwareQueryResult& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGStringID& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendParam( const VGUArcType& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
#ifdef __SYMBIAN32__
    inline void AppendParam( const VGeglImageKHR& aParam, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
#endif
        
    //Gets a simple parameter value from parameter array
    inline void GetTUint64( TUint64& aParam, TInt aIndex );
    inline void GetParamValue( VGfloat& aParam, TInt aIndex );
    inline void GetParamValue( VGbyte& aParam, TInt aIndex );
    inline void GetParamValue( VGubyte& aParam, TInt aIndex );
    inline void GetParamValue( VGshort& aParam, TInt aIndex );
    inline void GetParamValue( VGint& aParam, TInt aIndex );
    inline void GetParamValue( VGuint& aParam, TInt aIndex );
    inline void GetParamValue( VGboolean& aParam, TInt aIndex );
    inline void GetParamValue( VGErrorCode& aParam, TInt aIndex );
    inline void GetParamValue( VGParamType& aParam, TInt aIndex );
    inline void GetParamValue( VGRenderingQuality& aParam, TInt aIndex );
    inline void GetParamValue( VGPixelLayout& aParam, TInt aIndex );
    inline void GetParamValue( VGMatrixMode& aParam, TInt aIndex );
    inline void GetParamValue( VGMaskOperation& aParam, TInt aIndex );
    inline void GetParamValue( VGPathDatatype& aParam, TInt aIndex );
    inline void GetParamValue( VGPathAbsRel& aParam, TInt aIndex );
    inline void GetParamValue( VGPathSegment& aParam, TInt aIndex );
    inline void GetParamValue( VGPathCommand& aParam, TInt aIndex );   
    inline void GetParamValue( VGPathCapabilities& aParam, TInt aIndex );
    inline void GetParamValue( VGPathParamType& aParam, TInt aIndex );
    inline void GetParamValue( VGCapStyle& aParam, TInt aIndex );
    inline void GetParamValue( VGJoinStyle& aParam, TInt aIndex );
    inline void GetParamValue( VGFillRule& aParam, TInt aIndex );
    inline void GetParamValue( VGPaintMode& aParam, TInt aIndex );
    inline void GetParamValue( VGPaintParamType& aParam, TInt aIndex );
    inline void GetParamValue( VGPaintType& aParam, TInt aIndex );
    inline void GetParamValue( VGColorRampSpreadMode& aParam, TInt aIndex );
    inline void GetParamValue( VGTilingMode& aParam, TInt aIndex );
    inline void GetParamValue( VGImageFormat& aParam, TInt aIndex );
    inline void GetParamValue( VGImageQuality& aParam, TInt aIndex );
    inline void GetParamValue( VGImageParamType& aParam, TInt aIndex );
    inline void GetParamValue( VGImageMode& aParam, TInt aIndex );
    inline void GetParamValue( VGImageChannel& aParam, TInt aIndex );
    inline void GetParamValue( VGBlendMode& aParam, TInt aIndex );
    inline void GetParamValue( VGHardwareQueryType& aParam, TInt aIndex );
    inline void GetParamValue( VGHardwareQueryResult& aParam, TInt aIndex );
    inline void GetParamValue( VGStringID& aParam, TInt aIndex );
    inline void GetParamValue( VGUArcType& aParam, TInt aIndex );

    //Gets a pointer to an array parameter from parameter array
    inline void GetVectorData( VGfloat* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( VGint* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( VGuint* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( VGshort* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( VGubyte* &aData, TInt& aSize, TInt aIndex );
    inline void GetVectorData( void* &aData, TInt& aSize, TInt aIndex );

    //Sets a pointer to the data of an array parameter
    inline void SetVectorData( const VGfloat* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const VGint* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const VGuint* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const VGshort* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const VGubyte* aData, TInt aLength, TInt aIndex );
    inline void SetVectorData( const void* aData, TInt aLength, TInt aIndex );

    //Appends an array parameter to the parameter array
    // Does not serialize any data
    inline void AppendVector( const VGfloat* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const VGint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const VGuint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const VGshort* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const VGubyte* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    inline void AppendVector( const void* aData, TInt aLength, RemoteFunctionCallData::TParamDirection iDir = RemoteFunctionCallData::EIn );
    
    //Size of type aType
    TInt GetTypeSize( TInt32 aParamType ) const;

    //Alignemnt of type aType
	TInt GetTypeAlignment( TInt32 aParamType ) const;

    //Alignemnt of type aType
	TUint32 GetAPIUid() const;
    };

#include "openvgrfc.inl"
#endif
