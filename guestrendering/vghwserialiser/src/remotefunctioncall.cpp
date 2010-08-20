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

#include "remotefunctioncall.h"

// Symbian User mode Panic & debugging information
#if defined(__SYMBIAN32__) && !defined(__KERNEL_MODE__)
#define USE_SYMBIAN_CLIENT_PANICS
#include <e32debug.h>
#include "serialisedapiuids.h"
// Platsim Serialiser panic codes
typedef enum
	{
	ESerPanicMaxParamCountExceeded=1,
	ESerPanicRequestOperationHasOutParameter,
	ESerPanicVectorPtrIsNull,
	ESerPanicVectorSizeIsNegative,
	} TSerialiserPanic;

_LIT(KSerialiserPanicCategory, "Vghw Ser");

#ifdef _DEBUG
	#define VGSERPANIC_ASSERT(condition, panic, api, opcode, pcount) if (!(condition)) \
			{ SerialiserPanic(panic, #condition, __FILE__, __LINE__, api, opcode, pcount); }
#else
	#define VGSERPANIC_ASSERT(condition, panic, api, opcode, pcount) if (!(condition)) \
			{ SerialiserPanic(panic, NULL, NULL, __LINE__, api, opcode, pcount); }
#endif

void SerialiserPanic(TSerialiserPanic aPanicCode, char* aCondition, char* aFile, TInt aLine, TInt32 aApiUid, TInt32 aOpcode, TInt32 aParamCount)
	{
	if (aCondition && aFile)
		{
		RDebug::Printf("Vghw Serialiser Panic %d for failed Assert (%s), at %s:%d", aPanicCode, aCondition, aFile, aLine);
		}
	else
		{  
		RDebug::Printf("Vghw Serialiser Panic %d for failed Assert (line %d)", aPanicCode, aLine);
		}
		
	switch (aApiUid)
		{
		case SERIALISED_EGL_API_UID:
			RDebug::Printf("Api=SERIALISED_EGL_API_UID, aOpcode=%d, ParamCount=%d", aOpcode, aParamCount);
			break;
		case SERIALISED_OPENGLES_1_1_API_UID:
			RDebug::Printf("Api=SERIALISED_OPENGLES_1_1_API_UID, aOpcode=%d, ParamCount=%d", aOpcode, aParamCount);
			break;
		case SERIALISED_OPENVG_API_UID:
			RDebug::Printf("Api=SERIALISED_OPENVG_API_UID, aOpcode=%d, ParamCount=%d", aOpcode, aParamCount);
			break;
		default:
			RDebug::Printf("Api=%d, aOpcode=%d, ParamCount=%d", aApiUid, aOpcode, aParamCount);
			break;
		}
	User::Panic(KSerialiserPanicCategory, aPanicCode);
	}
#endif


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint32 RemoteFunctionCallData::TSimpleParam::Size() const
	{
	TUint32 size(0);
    size += sizeof( iDataType );
    size += sizeof( iDataTypeSize );
    size += iDataTypeSize;
	return size;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint32 RemoteFunctionCallData::TVectorParam::Size( TParamDirection aDir, TOperationType aOpType, TInt aIndexToAlign ) const
    {
    TUint32 size(0);
    size += sizeof( iDataType );
    size += sizeof( iVectorLength );
    size += sizeof( iCallerAddress );
    size += sizeof( iDataTypeSize );
    size += sizeof( iDataTypeAlignment );

    if ( ( (aDir & EIn) && ( EOpRequestWithReply == aOpType || EOpRequest == aOpType ) ) 
            ||
         ( ( aDir & EOut ) && aOpType == EOpReply ) 
        )
        {            
        if ( iVectorLength )
            {
            TInt aNewIndex = RemoteFunctionCallData::AlignIndex( aIndexToAlign + size, iDataTypeAlignment );
            size += aNewIndex - ( aIndexToAlign + size );
            }
        size += iDataTypeSize*iVectorLength;
        }
    return size;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint32 RemoteFunctionCallData::TParam::Size( TOperationType aOpType, TInt aIndexToAlign ) const
    {
	TUint32 size(0);
    size += sizeof( iDir );
    size += sizeof( iType );
    if ( TParam::ESimple == iType )
        {
        size += iSimpleParam.Size();
        }
    else if ( TParam::EVector == iType )
        {
        size += iVectorParam.Size( iDir, aOpType, aIndexToAlign + size );
        }
	return size;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCallData::CopyData( TUint8* aDest, const TUint8* aSource, TInt32 aSize )
    {
    memcpy( aDest, aSource, aSize );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C TInt RemoteFunctionCallData::AlignIndex( TInt aIndex, const TInt32 aAlignment )
    {
    const TInt remainder = aIndex % aAlignment;
    if ( remainder )
        {
        aIndex += aAlignment - remainder;
        }
    return aIndex;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C TUint32 RemoteFunctionCallData::SerialisedLength() const
    {
    TUint32 size(0);
    size += sizeof( iHeader.iOpCode );
    size += sizeof( iHeader.iTransactionId );
    size += sizeof( iHeader.iProcessId );
    size += sizeof( iHeader.iThreadId );
    size += sizeof( iHeader.iParameterCount );
    size += sizeof( iHeader.iOpType );
    size += sizeof( iHeader.iReturnValue );
    size += sizeof( iHeader.iApiUid );

    for ( TInt i = 0; i < iHeader.iParameterCount; i++ )
        {
        size += iParameters[i].Size( (RemoteFunctionCallData::TOperationType)iHeader.iOpType, size );
        }
    
    return size;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCallData::SetThreadInformation( const TUint32 aProcessId, const TUint32 aThreadId )
    { 
    iHeader.iProcessId = aProcessId; iHeader.iThreadId = aThreadId; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCallData::SetTransactionId( TUint32 aTransactionId )
    { 
    iHeader.iTransactionId = aTransactionId; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCallData::SetOperationType( TInt32 aOpType )
    { 
    iHeader.iOpType = aOpType; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCallData::SetReturnValue( TUint32 aRetVal )
    { 
    iHeader.iReturnValue = aRetVal; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C const RemoteFunctionCallData::THeader& RemoteFunctionCallData::Header() const 
    { 
    return iHeader; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C TInt RemoteFunctionCallData::ParameterCount()
    { 
    return iHeader.iParameterCount; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C RemoteFunctionCallData::TParam* RemoteFunctionCallData::Parameters()
    { 
    return iParameters; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
RemoteFunctionCallData::THeader& RemoteFunctionCallData::HeaderData()
    { 
    return iHeader; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCallData::GetVectorData( TInt32 aParamType, void** aData, TInt& aSize, TInt aIndex )
    {
    ASSERT( iParameters[aIndex].iVectorParam.iDataType == aParamType );
    ASSERT( aIndex < iHeader.iParameterCount );

    *aData = (void*)iParameters[aIndex].iVectorParam.iData;
    aSize = iParameters[aIndex].iVectorParam.iVectorLength;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCallData::GetParamValue( TInt32 aParamType, TUint8* aData, TInt aIndex )
    {
    ASSERT( iParameters[aIndex].iSimpleParam.iDataType == aParamType );
    ASSERT( aIndex < iHeader.iParameterCount );

    RemoteFunctionCallData::CopyData( aData, (TUint8*)&iParameters[aIndex].iSimpleParam.iData, iParameters[aIndex].iSimpleParam.iDataTypeSize );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCall::Init( TInt32 aOpCode, RemoteFunctionCallData::TOperationType aOpType )
    {
    iData.iHeader.iOpCode = aOpCode;
    iData.iHeader.iParameterCount = 0;
    iData.iHeader.iTransactionId = 0;
    iData.iHeader.iOpType = aOpType;
    iData.iHeader.iProcessId = 0;
    iData.iHeader.iThreadId = 0;
    iData.iHeader.iApiUid = GetAPIUid();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C RemoteFunctionCall::RemoteFunctionCall( RemoteFunctionCallData& aData ):
    iData( aData )
    {
    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCall::SetVectorData( TInt32 aParamType, const TUint8* aData, TInt aLength, TInt aIndex )
	{
	ASSERT( iData.iParameters[aIndex].iVectorParam.iDataType == aParamType );
	ASSERT( aIndex < iData.iHeader.iParameterCount );
	ASSERT( static_cast<TUint32>(aLength) <= iData.iParameters[aIndex].iVectorParam.iVectorLength );
	ASSERT( RemoteFunctionCallData::EIn != iData.iParameters[aIndex].iDir );
	
	iData.iParameters[aIndex].iVectorParam.iData = aData;
	iData.iParameters[aIndex].iVectorParam.iVectorLength = aLength;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCall::SetParamData( TInt32 aParamType, const TUint8* aData, TInt aIndex )
    {
    ASSERT( iData.iParameters[aIndex].iSimpleParam.iDataType == aParamType );
    ASSERT( aIndex < iData.iHeader.iParameterCount );
    ASSERT( RemoteFunctionCallData::EIn != iData.iParameters[aIndex].iDir );

    RemoteFunctionCallData::CopyData( (TUint8*)&iData.iParameters[aIndex].iSimpleParam.iData, aData, iData.iParameters[aIndex].iSimpleParam.iDataTypeSize );    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCall::AppendParam( TInt32 aParamType, const TUint8* aData, RemoteFunctionCallData::TParamDirection aDir )
    {
#ifdef USE_SYMBIAN_CLIENT_PANICS
	VGSERPANIC_ASSERT( iData.iHeader.iParameterCount < RemoteFunctionCallData::KMaxParameterCount,
		ESerPanicMaxParamCountExceeded, iData.iHeader.iApiUid, iData.iHeader.iOpCode, iData.iHeader.iParameterCount);
	VGSERPANIC_ASSERT( (RemoteFunctionCallData::EIn == aDir) || (iData.iHeader.iOpCode != RemoteFunctionCallData::EOpRequest),
		ESerPanicRequestOperationHasOutParameter, iData.iHeader.iApiUid, iData.iHeader.iOpCode, iData.iHeader.iParameterCount);
#else
	ASSERT( iData.iHeader.iParameterCount < RemoteFunctionCallData::KMaxParameterCount );
#endif

	RemoteFunctionCallData::TParam param;
    param.iType = RemoteFunctionCallData::TParam::ESimple;
    param.iDir = aDir;
    param.iSimpleParam.iDataType = aParamType;
    param.iSimpleParam.iDataTypeSize = GetTypeSize( param.iSimpleParam.iDataType );
    RemoteFunctionCallData::CopyData( (TUint8*)&param.iSimpleParam.iData, aData, param.iSimpleParam.iDataTypeSize );
    iData.iParameters[iData.iHeader.iParameterCount] = param;
    iData.iHeader.iParameterCount++;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCall::AppendVector( TInt32 aParamType, TUint32 aLength, const TUint8* aData, RemoteFunctionCallData::TParamDirection aDir )
    {        
#ifdef USE_SYMBIAN_CLIENT_PANICS
	VGSERPANIC_ASSERT( iData.iHeader.iParameterCount < RemoteFunctionCallData::KMaxParameterCount,
		ESerPanicMaxParamCountExceeded, iData.iHeader.iApiUid, iData.iHeader.iOpCode, iData.iHeader.iParameterCount);
	VGSERPANIC_ASSERT( (aLength & 0x80000000) == 0,
		ESerPanicVectorSizeIsNegative, iData.iHeader.iApiUid, iData.iHeader.iOpCode, iData.iHeader.iParameterCount);
#else
	ASSERT( iData.iHeader.iParameterCount < RemoteFunctionCallData::KMaxParameterCount );
    ASSERT( (RemoteFunctionCallData::EIn == aDir) || (iData.iHeader.iOpCode != RemoteFunctionCallData::EOpRequest) );
#endif


	RemoteFunctionCallData::TParam param;
    param.iType = RemoteFunctionCallData::TParam::EVector;
    param.iDir = aDir;
    param.iVectorParam.iDataType = aParamType;
    param.iVectorParam.iData = aData;
    param.iVectorParam.iDataTypeSize = GetTypeSize( aParamType );
    param.iVectorParam.iDataTypeAlignment = GetTypeAlignment( aParamType );
	
	if ( aData )
		{
		param.iVectorParam.iVectorLength = aLength;
		}
	else
		{
#ifdef USE_SYMBIAN_CLIENT_PANICS
		VGSERPANIC_ASSERT( (aLength == 0) || (aDir != RemoteFunctionCallData::EIn),
				ESerPanicVectorPtrIsNull, iData.iHeader.iApiUid, iData.iHeader.iOpCode, iData.iHeader.iParameterCount);
#endif
		param.iVectorParam.iVectorLength = 0;
		}

    if ( RemoteFunctionCallData::EOut & aDir )
        {
#ifdef USE_SYMBIAN_CLIENT_PANICS
		VGSERPANIC_ASSERT( iData.iHeader.iOpCode != RemoteFunctionCallData::EOpRequest,
				ESerPanicRequestOperationHasOutParameter, iData.iHeader.iApiUid, iData.iHeader.iOpCode, iData.iHeader.iParameterCount);
#endif
        param.iVectorParam.iCallerAddress = const_cast<TUint8*>( aData );
        }
    else
        {
        param.iVectorParam.iCallerAddress = NULL;
        }

    iData.iParameters[iData.iHeader.iParameterCount] = param;
    iData.iHeader.iParameterCount++;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C RemoteFunctionCallData& RemoteFunctionCall::Data()
    {
    return iData;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C TUint32 RemoteFunctionCall::ReturnValue()
    {
    return iData.Header().iReturnValue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void RemoteFunctionCall::SetReturnValue( TUint32 aRetValue )
    {
    iData.SetReturnValue( aRetValue );
    }
