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


#include "opengles11rfc.h"
#include "serialisedapiuids.h"

OpenGlES11RFC::OpenGlES11RFC( RemoteFunctionCallData& aData ):
    RemoteFunctionCall( aData )
    {    
    }

TUint32 OpenGlES11RFC::GetAPIUid() const
{
	return SERIALISED_OPENGLES_1_1_API_UID;
}

TInt OpenGlES11RFC::GetTypeAlignment( TInt32 aType ) const
{
    TInt ret(0);
    if ( EGLvoid == aType )
        {
        ret = 4;
        }
    else
        {
        ret = GetTypeSize( aType );
        }
    return ret;
}

TInt OpenGlES11RFC::GetTypeSize( TInt32 aType ) const
{
    TInt size(0);
    switch ( aType )
    {
    case EGLvoid:
    case EGLubyte:
    case EGLboolean:
        size = 1;
        break;
    case EGLshort:
        size = 2;
        break;
    case EGLbitfield:
    case EGLclampf:
    case EGLclampx:
    case EGLenum:
    case EGLfixed:
    case EGLfloat:
    case EGLint:
    case EGLintptr:
    case EGLsizei:
    case EGLsizeiptr:
    case EGLuint:
        size = 4;
        break;
    default:
        break;
    }
    return size;
}
