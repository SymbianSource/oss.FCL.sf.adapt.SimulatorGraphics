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

#include "eglrfc.h"
#include "serialisedapiuids.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EglRFC::EglRFC( RemoteFunctionCallData& aData ):
    RemoteFunctionCall( aData )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint32 EglRFC::GetAPIUid() const
    {
	return SERIALISED_EGL_API_UID;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt EglRFC::GetTypeAlignment( TInt32 aType ) const
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
TInt EglRFC::GetTypeSize( TInt32 aType ) const
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
            size = 4;
            break;
        case ETUint64:
            size = 8;
            break;
        default:
            ASSERT( EFalse );
            break;
        }
	return size;
    }

EGLint EglRFC::MetaGetConfigAttribute( int aIndex )
        {
    static const EGLint attribs[] =
    {
        EGL_BUFFER_SIZE,
        EGL_RED_SIZE,
        EGL_GREEN_SIZE,
        EGL_BLUE_SIZE,
        EGL_LUMINANCE_SIZE,
        EGL_ALPHA_SIZE,
        EGL_ALPHA_MASK_SIZE,
        EGL_BIND_TO_TEXTURE_RGB,
        EGL_BIND_TO_TEXTURE_RGBA,
        EGL_COLOR_BUFFER_TYPE,
        EGL_CONFIG_CAVEAT,
        EGL_CONFIG_ID,
        EGL_CONFORMANT,
        EGL_DEPTH_SIZE,
        EGL_LEVEL,
        EGL_MATCH_NATIVE_PIXMAP,
        EGL_MAX_SWAP_INTERVAL,
        EGL_MIN_SWAP_INTERVAL,
        EGL_NATIVE_RENDERABLE,
        EGL_NATIVE_VISUAL_TYPE,
        EGL_RENDERABLE_TYPE,
        EGL_SAMPLE_BUFFERS,
        EGL_SAMPLES,
        EGL_STENCIL_SIZE,
        EGL_SURFACE_TYPE,
        EGL_TRANSPARENT_TYPE,
        EGL_TRANSPARENT_RED_VALUE,
        EGL_TRANSPARENT_GREEN_VALUE,
        EGL_TRANSPARENT_BLUE_VALUE
    };
    return attribs[aIndex];
        }
EGLint EglRFC::MetaGetConfigAttributeSelCriteria( int aIndex )
    {
    static const EGLint attrib_selection_criteria[] =
    {
    EAtLeast,
    EAtLeast,
    EAtLeast,
    EAtLeast,
    EAtLeast,
    EAtLeast,
    EAtLeast,
    EExact,
    EExact,
    EExact,
    EExact,
    EExact,
    EMask,
    EAtLeast,
    EExact,
    ESpecial,
    EExact,
    EExact,
    EExact,
    EExact,
    EMask,
    EAtLeast,
    EAtLeast,
    EAtLeast,
    EMask,
    EExact,
    EExact,
    EExact,
    EExact
    };
    return attrib_selection_criteria[aIndex];
    }
TInt EglRFC::MetaGetConfigAttributeCnt()
        {
    return 29;
        }
