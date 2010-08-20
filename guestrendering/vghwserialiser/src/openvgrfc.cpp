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


#include "openvgrfc.h"
#include "serialisedapiuids.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
OpenVgRFC::OpenVgRFC( RemoteFunctionCallData& aData ):
    RemoteFunctionCall( aData )
    {    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint32 OpenVgRFC::GetAPIUid() const
    {
	return SERIALISED_OPENVG_API_UID;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt OpenVgRFC::GetTypeAlignment( TInt32 aType ) const
    {
    TInt ret(0);
    if ( EVGvoid == aType )
        {
        ret = 4;
        }
    else
        {
        ret = GetTypeSize( aType );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt OpenVgRFC::GetTypeSize( TInt32 aType ) const
    {
	TInt size(0);
	switch ( aType )
        {
        case EVGvoid:
        case EVGbyte:
        case EVGubyte:
            size = 1;
            break;
        case EVGshort:
            size = 2;
            break;
        case EVGfloat:
        case EVGint:
        case EVGuint:
        case EVGbitfield:
        case EVGboolean:
        case EVGHandle:
        case EVGErrorCode:
        case EVGParamType:
        case EVGRenderingQuality:
        case EVGPixelLayout:
        case EVGMatrixMode:
        case EVGMaskOperation:
        case EVGPathDatatype:
        case EVGPathAbsRel:
        case EVGPathSegment:
        case EVGPathCommand:
        case EVGPath:
        case EVGPathCapabilities:
        case EVGPathParamType:
        case EVGCapStyle:
        case EVGJoinStyle:
        case EVGFillRule:
        case EVGPaintMode:
        case EVGPaint:
        case EVGPaintParamType:
        case EVGPaintType:
        case EVGColorRampSpreadMode:
        case EVGTilingMode:
        case EVGImageFormat:
        case EVGImage:
        case EVGImageQuality:
        case EVGImageParamType:
        case EVGImageMode:
        case EVGImageChannel:
        case EVGBlendMode:
        case EVGHardwareQueryType:
        case EVGHardwareQueryResult:
        case EVGStringID:
        case EVGeglImageKHR:
            size = 4;
            break;
        case ETUint64:
            size = 8;
            break;
        default:
            break;
        }
	return size;
    }
