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

inline void OpenGlES11RFC::GetParamValue( GLbyte& aParam, TInt aIndex )
{
    iData.GetParamValue( OpenGlES11RFC::EGLbyte, (TUint8*)&aParam, aIndex );
}
inline void OpenGlES11RFC::GetParamValue( GLubyte& aParam, TInt aIndex )
{
    iData.GetParamValue( OpenGlES11RFC::EGLubyte, (TUint8*)&aParam, aIndex );
}
inline void OpenGlES11RFC::GetParamValue( GLshort& aParam, TInt aIndex )
{
    iData.GetParamValue( OpenGlES11RFC::EGLshort, (TUint8*)&aParam, aIndex );
}
inline void OpenGlES11RFC::GetParamValue( GLushort& aParam, TInt aIndex )
{
    iData.GetParamValue( OpenGlES11RFC::EGLushort, (TUint8*)&aParam, aIndex );
}
inline void OpenGlES11RFC::GetParamValue( GLint& aParam, TInt aIndex )
{
    iData.GetParamValue( OpenGlES11RFC::EGLint, (TUint8*)&aParam, aIndex );
}
inline void OpenGlES11RFC::GetParamValue( GLuint& aParam, TInt aIndex )
{
    iData.GetParamValue( OpenGlES11RFC::EGLuint, (TUint8*)&aParam, aIndex );
}
inline void OpenGlES11RFC::GetParamValue( GLfloat& aParam, TInt aIndex )
{
    iData.GetParamValue( OpenGlES11RFC::EGLfloat, (TUint8*)&aParam, aIndex );
}


inline void OpenGlES11RFC::GetVectorData( GLint* &aData, TInt& aSize, TInt aIndex )
{
    iData.GetVectorData( EGLint, (void**)&aData, aSize, aIndex );
}
inline void OpenGlES11RFC::GetVectorData( GLuint* &aData, TInt& aSize, TInt aIndex )
{
    iData.GetVectorData( EGLuint, (void**)&aData, aSize, aIndex );
}
inline void OpenGlES11RFC::GetVectorData( GLshort* &aData, TInt& aSize, TInt aIndex )
{
    iData.GetVectorData( EGLshort, (void**)&aData, aSize, aIndex );
}
inline void OpenGlES11RFC::GetVectorData( GLfloat* &aData, TInt& aSize, TInt aIndex )
{
    iData.GetVectorData( EGLfloat, (void**)&aData, aSize, aIndex );
}
inline void OpenGlES11RFC::GetVectorData( GLboolean* &aData, TInt& aSize, TInt aIndex )
{
    iData.GetVectorData( EGLboolean, (void**)&aData, aSize, aIndex );
}
inline void OpenGlES11RFC::GetVectorData( void* &aData, TInt& aSize, TInt aIndex )
{
    iData.GetVectorData( EGLvoid, &aData, aSize, aIndex );
}


inline void OpenGlES11RFC::SetVectorData( const GLint* aData, TInt aLength, TInt aIndex )
{
    RemoteFunctionCall::SetVectorData( EGLint, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
}
inline void OpenGlES11RFC::SetVectorData( const GLuint* aData, TInt aLength, TInt aIndex )
{
    RemoteFunctionCall::SetVectorData( EGLuint, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
}
inline void OpenGlES11RFC::SetVectorData( const GLshort* aData, TInt aLength, TInt aIndex )
{
    RemoteFunctionCall::SetVectorData( EGLshort, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
}
inline void OpenGlES11RFC::SetVectorData( const GLfloat* aData, TInt aLength, TInt aIndex )
{
    RemoteFunctionCall::SetVectorData( EGLfloat, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
}
inline void OpenGlES11RFC::SetVectorData( const GLboolean* aData, TInt aLength, TInt aIndex )
{
    RemoteFunctionCall::SetVectorData( EGLboolean, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
}
inline void OpenGlES11RFC::SetVectorData( const void* aData, TInt aLength, TInt aIndex )
{
    RemoteFunctionCall::SetVectorData( EGLvoid, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
}


inline void OpenGlES11RFC::AppendParam( const GLbyte& aParam, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendParam( EGLbyte, reinterpret_cast<const TUint8*>(&aParam), aDir );
}
inline void OpenGlES11RFC::AppendParam( const GLubyte& aParam, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendParam( EGLubyte, reinterpret_cast<const TUint8*>(&aParam), aDir );
}
inline void OpenGlES11RFC::AppendParam( const GLshort& aParam, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendParam( EGLshort, reinterpret_cast<const TUint8*>(&aParam), aDir );
}
inline void OpenGlES11RFC::AppendParam( const GLushort& aParam, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendParam( EGLushort, reinterpret_cast<const TUint8*>(&aParam), aDir );
}
inline void OpenGlES11RFC::AppendParam( const GLint& aParam, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendParam( EGLint, reinterpret_cast<const TUint8*>(&aParam), aDir );
}
inline void OpenGlES11RFC::AppendParam( const GLuint& aParam, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendParam( EGLuint, reinterpret_cast<const TUint8*>(&aParam), aDir );
}
inline void OpenGlES11RFC::AppendParam( const GLfloat& aParam, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendParam( EGLfloat, reinterpret_cast<const TUint8*>(&aParam), aDir );
}


inline void OpenGlES11RFC::AppendVector( const GLint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendVector( EGLint, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
}
inline void OpenGlES11RFC::AppendVector( const GLuint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendVector( EGLuint, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
}
inline void OpenGlES11RFC::AppendVector( const GLshort* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendVector( EGLshort, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
}
inline void OpenGlES11RFC::AppendVector( const GLfloat* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendVector( EGLfloat, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
}
inline void OpenGlES11RFC::AppendVector( const GLboolean* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendVector( EGLboolean, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
}
inline void OpenGlES11RFC::AppendVector( const void* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
{
    RemoteFunctionCall::AppendVector( EGLvoid, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
}
