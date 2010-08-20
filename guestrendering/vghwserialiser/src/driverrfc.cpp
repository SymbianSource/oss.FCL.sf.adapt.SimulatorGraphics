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

#include "driverrfc.h"
#include "serialisedapiuids.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
//DriverRFC::DriverRFC( RemoteFunctionCallData& aData ):
//    RemoteFunctionCall( aData )
//    {
//    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt DriverRFC::GetTypeAlignment( TInt32 aType ) const
    {
    TInt ret(0);
    if ( EEGLVoid == aType )
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
TInt DriverRFC::GetTypeSize( TInt32 aType ) const
    {
    TInt size(0);
    switch ( aType )
        {
        case EEGLVoid:
            size = 1;
            break;
        case EEGLBoolean:
        case EEGLint:
        case EEGLenum:
        case EEGLDisplay:
        case EEGLConfig:
        case EEGLSurface:
        case EEGLContext:
        case EEGLClientBuffer:
        case EEGLImageKHR:
        case EEGLNativeDisplayType:
        case EEGLNativeWindowType:
        case EEGLNativePixmapType:
        case ETUint32:
            size = 4;
            break;
        //VG Types
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
        default:
            ASSERT( EFalse );
            break;
        }
    return size;
    }
