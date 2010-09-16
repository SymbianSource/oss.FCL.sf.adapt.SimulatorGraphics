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

inline void EglRFC::GetTUint64( TUint64& aParam, TInt aIndex )
    {
    // note that 64-bit values consume 2 param slots!
    TUint32 lower, upper;
    iData.GetParamValue( EglRFC::EEGLint, (TUint8*)&lower, aIndex );
    iData.GetParamValue( EglRFC::EEGLint, (TUint8*)&upper, aIndex+1 );
    aParam = ((TUint64)upper << 32) + lower;
    }
inline void EglRFC::GetEGLBoolean( EGLBoolean& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLBoolean, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLint( EGLint& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLint, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLenum( EGLenum& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLenum, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLDisplay( EGLDisplay& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLDisplay, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLConfig( EGLConfig& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLConfig, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLSurface( EGLSurface& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLSurface, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLContext( EGLContext& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLContext, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLClientBuffer( EGLClientBuffer& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLClientBuffer, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLImageKHR( EGLImageKHR& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLImageKHR, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLNativeDisplayType( NativeDisplayType& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLNativeDisplayType, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLNativeWindowType( NativeWindowType& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLNativeWindowType, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLNativePixmapType( NativePixmapType& aParam, TInt aIndex )
	{
	iData.GetParamValue( EglRFC::EEGLNativePixmapType, (TUint8*)&aParam, aIndex );
	}

inline void EglRFC::GetEGLintVectorData( EGLint* &aData, TInt& aSize, TInt aIndex )
	{
	iData.GetVectorData( EEGLint, (void**)&aData, aSize, aIndex );
	}

inline void EglRFC::GetEGLConfigVectorData( EGLConfig* &aData, TInt& aSize, TInt aIndex )
	{
	iData.GetVectorData( EEGLConfig, (void**)&aData, aSize, aIndex );
	}

inline void EglRFC::SetEGLintVectorData( const EGLint* aData, TInt aLength, TInt aIndex )
	{
	RemoteFunctionCall::SetVectorData( EEGLint, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
	}

inline void EglRFC::SetEGLConfigVectorData( const EGLConfig* aData, TInt aLength, TInt aIndex )
	{
	RemoteFunctionCall::SetVectorData( EEGLConfig, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
	}
    

inline void EglRFC::AppendTUint64( const TUint64& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    // note that 64-bit values consume 2 param slots!
    TUint32 lower = (TUint32)(aParam & 0xFFFFFFFF);
    TUint32 upper = (TUint32)((aParam >> 32) & 0xFFFFFFFF);
    AppendParam( EEGLint, reinterpret_cast<const TUint8*>(&lower), aDir );
    AppendParam( EEGLint, reinterpret_cast<const TUint8*>(&upper), aDir );
    }

inline void EglRFC::AppendEGLBoolean( const EGLBoolean& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLBoolean, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLint( const EGLint& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLint, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLenum( const EGLenum& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLenum, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLDisplay( const EGLDisplay& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLDisplay, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLConfig( const EGLConfig& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLConfig, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLSurface( const EGLSurface& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLSurface, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLContext( const EGLContext& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLContext, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLClientBuffer( const EGLClientBuffer& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLClientBuffer, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLImageKHR( const EGLImageKHR& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLImageKHR, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLNativeDisplayType( const NativeDisplayType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLNativeDisplayType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLNativeWindowType( const NativeWindowType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLNativeWindowType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLNativePixmapType( const NativePixmapType& aParam, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLNativePixmapType, reinterpret_cast<const TUint8*>(&aParam), aDir );
    }

inline void EglRFC::AppendEGLintVector( const EGLint* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EEGLint, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

inline void EglRFC::AppendEGLConfigVector( const EGLConfig* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EEGLConfig, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

#ifdef __SYMBIAN32__
// Appends Symbian TSize object as 2 integers, for width and height
inline void EglRFC::AppendEGLSize( const TSize& aSize, RemoteFunctionCallData::TParamDirection aDir )
    {
    AppendParam( EEGLint, reinterpret_cast<const TUint8*>(&aSize.iWidth), aDir );
    AppendParam( EEGLint, reinterpret_cast<const TUint8*>(&aSize.iHeight), aDir );
    }
#endif

inline void EglRFC::AppendVector( const void* aData, TInt aLength, RemoteFunctionCallData::TParamDirection aDir )
    {
    RemoteFunctionCall::AppendVector( EEGLVoid, aLength, reinterpret_cast<const TUint8*>( aData ), aDir );
    }

inline void EglRFC::GetVectorData( void* &aData, TInt& aSize, TInt aIndex )
	{
	iData.GetVectorData( EEGLVoid, &aData, aSize, aIndex );
	}

inline void EglRFC::SetVectorData( const void* aData, TInt aLength, TInt aIndex )
	{
	RemoteFunctionCall::SetVectorData( EEGLVoid, reinterpret_cast<const TUint8*>(aData), aLength, aIndex );
	}
