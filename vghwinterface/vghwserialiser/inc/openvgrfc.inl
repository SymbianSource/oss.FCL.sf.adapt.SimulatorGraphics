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

inline void OpenVgRFC::GetTUint64( TUint64& aParam, TInt aIndex )
    {
    // note that 64-bit values consume 2 param slots!
    TUint32 lower, upper;
    iData.GetParamValue( OpenVgRFC::EVGuint, (TUint8*)&lower, aIndex );
    iData.GetParamValue( OpenVgRFC::EVGuint, (TUint8*)&upper, aIndex+1 );
    aParam = ((TUint64)upper << 32) + lower;
    }
inline void OpenVgRFC::GetParamValue( VGfloat& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGfloat, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGbyte& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGbyte, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGubyte& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGubyte, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGshort& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGshort, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGint& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGint, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGuint& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGuint, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGboolean& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGboolean, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGErrorCode& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGErrorCode, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGParamType& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGParamType, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGRenderingQuality& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGRenderingQuality, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPixelLayout& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPixelLayout, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGMatrixMode& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGMatrixMode, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGMaskOperation& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGMaskOperation, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPathDatatype& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPathDatatype, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPathAbsRel& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPathAbsRel, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPathSegment& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPathSegment, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPathCommand& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPathCommand, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPathCapabilities& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPathCapabilities, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPathParamType& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPathParamType, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGCapStyle& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGCapStyle, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGJoinStyle& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGJoinStyle, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGFillRule& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGFillRule, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPaintMode& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPaintMode, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPaintParamType& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPaintParamType, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGPaintType& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGPaintType, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGColorRampSpreadMode& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGColorRampSpreadMode, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGTilingMode& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGTilingMode, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGImageFormat& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGImageFormat, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGImageQuality& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGImageQuality, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGImageParamType& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGImageParamType, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGImageMode& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGImageMode, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGImageChannel& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGImageChannel, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGBlendMode& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGBlendMode, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGHardwareQueryType& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGHardwareQueryType, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGHardwareQueryResult& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGHardwareQueryResult, (TUint8*)&aParam, aIndex );
	}
inline void OpenVgRFC::GetParamValue( VGStringID& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGStringID, (TUint8*)&aParam, aIndex );
	}

inline void OpenVgRFC::GetParamValue( VGUArcType& aParam, TInt aIndex )
	{
	iData.GetParamValue( OpenVgRFC::EVGUArcType, (TUint8*)&aParam, aIndex );
	}

inline void OpenVgRFC::GetVectorData( VGfloat* &aData, TInt& aSize, TInt aIndex )
	{
	iData.GetVectorData( EVGfloat, (void**)&aData, aSize, aIndex );
	}

inline void OpenVgRFC::GetVectorData( VGint* &aData, TInt& aSize, TInt aIndex )
	{
	iData.GetVectorData( EVGint, (void**)&aData, aSize, aIndex );
	}

inline void OpenVgRFC::GetVectorData( VGuint* &aData, TInt& aSize, TInt aIndex )
	{
	iData.GetVectorData( EVGuint, (void**)&aData, aSize, aIndex );
	}

inline void OpenVgRFC::GetVectorData( VGshort* &aData, TInt& aSize, TInt aIndex )
	{
	iData.GetVectorData( EVGshort, (void**)&aData, aSize, aIndex );
	}

inline void OpenVgRFC::GetVectorData( VGubyte* &aData, TInt& aSize, TInt aIndex )
	{
	iData.GetVectorData( EVGubyte, (void**)&aData, aSize, aIndex );
	}

inline void OpenVgRFC::GetVectorData( void* &aData, TInt& aSize, TInt aIndex )
	{
	iData.GetVectorData( EVGvoid, &aData, aSize, aIndex );
	}

inline void OpenVgRFC::SetVectorData( const VGfloat* aData, TInt aLength, TInt aIndex )
	{
	RemoteFunctionCall::SetVectorData( EVGfloat, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
	}

inline void OpenVgRFC::SetVectorData( const VGint* aData, TInt aLength, TInt aIndex )
	{
	RemoteFunctionCall::SetVectorData( EVGint, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
	}

inline void OpenVgRFC::SetVectorData( const VGuint* aData, TInt aLength, TInt aIndex )
	{
	RemoteFunctionCall::SetVectorData( EVGuint, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
	}

inline void OpenVgRFC::SetVectorData( const VGshort* aData, TInt aLength, TInt aIndex )
	{
	RemoteFunctionCall::SetVectorData( EVGshort, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
	}

inline void OpenVgRFC::SetVectorData( const VGubyte* aData, TInt aLength, TInt aIndex )
	{
	RemoteFunctionCall::SetVectorData( EVGubyte, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
	}

inline void OpenVgRFC::SetVectorData( const void* aData, TInt aLength, TInt aIndex )
	{
	RemoteFunctionCall::SetVectorData( EVGvoid, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
	}
    
inline void OpenVgRFC::AppendTUint64( const TUint64& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    // note that 64-bit values consume 2 param slots!
    TUint32 lower = (TUint32)(aParam & 0xFFFFFFFF);
    TUint32 upper = (TUint32)((aParam >> 32) & 0xFFFFFFFF);
    RemoteFunctionCall::AppendParam( EVGuint, reinterpret_cast<const TUint8*>(&lower), aDir );
    RemoteFunctionCall::AppendParam( EVGuint, reinterpret_cast<const TUint8*>(&upper), aDir );
    }
inline void OpenVgRFC::AppendParam( const VGfloat& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGfloat, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGbyte& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGbyte, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGubyte& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGubyte, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGshort& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGshort, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGint& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGint, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGuint& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGuint, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }


inline void OpenVgRFC::AppendParam( const VGboolean& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGboolean, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGErrorCode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGErrorCode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGParamType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGRenderingQuality& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGRenderingQuality, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPixelLayout& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPixelLayout, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGMatrixMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGMatrixMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGMaskOperation& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGMaskOperation, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPathDatatype& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathDatatype, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPathAbsRel& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathAbsRel, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPathSegment& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathSegment, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPathCommand& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathSegment, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPathCapabilities& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathCapabilities, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPathParamType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPathParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGCapStyle& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGCapStyle, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGJoinStyle& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGJoinStyle, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGFillRule& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGFillRule, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPaintMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPaintMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPaintParamType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPaintParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGPaintType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGPaintParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGColorRampSpreadMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGColorRampSpreadMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGTilingMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGTilingMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGImageFormat& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageFormat, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGImageQuality& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageQuality, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGImageParamType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageParamType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGImageMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGImageChannel& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGImageChannel, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGBlendMode& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGBlendMode, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGHardwareQueryType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGHardwareQueryType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGHardwareQueryResult& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGHardwareQueryResult, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGStringID& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGStringID, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void OpenVgRFC::AppendParam( const VGUArcType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGUArcType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

#ifdef __SYMBIAN32__
inline void OpenVgRFC::AppendParam( const VGeglImageKHR& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendParam( EVGeglImageKHR, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }
#endif

inline void OpenVgRFC::AppendVector( const VGfloat* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EVGfloat, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

inline void OpenVgRFC::AppendVector( const VGint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EVGint, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

inline void OpenVgRFC::AppendVector( const VGuint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EVGuint, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

inline void OpenVgRFC::AppendVector( const VGshort* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EVGshort, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

inline void OpenVgRFC::AppendVector( const VGubyte* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EVGubyte, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

inline void OpenVgRFC::AppendVector( const void* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EVGvoid, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }
