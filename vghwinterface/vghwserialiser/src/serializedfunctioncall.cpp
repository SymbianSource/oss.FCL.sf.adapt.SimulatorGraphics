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

#include "serializedfunctioncall.h"
#include "remotefunctioncall.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt SerializedFunctionCall::Write( TUint8* aDest, TInt& aDestIndex, const TInt aDestSize,
        const TUint8* aSource, const TInt aSize, TInt& aSourceIndex, const TInt aOffset, TBool aVector )
    {        
    if ( aSourceIndex + aSize > aOffset )
        {
        TInt32 offset(0);
        TInt32 size( aSize );  
        if ( aOffset - aSourceIndex > 0 )
            {
            offset = aOffset - aSourceIndex;
            size = aSize - offset;
            }

        if ( aDestIndex + size >= aDestSize )
            {
            size = aDestSize - aDestIndex;
            }

        if ( aSource )
            {
            if ( iDataCopier && aVector )
                {
                iDataCopier->CopyVector( &aDest[aDestIndex], &aSource[offset], size );
                }
            else if ( iDataCopier )
                {
                iDataCopier->CopyData( &aDest[aDestIndex], &aSource[offset], size );            
                }
            else
                {
				RemoteFunctionCallData::CopyData( &aDest[aDestIndex], &aSource[offset], size );
                }
            }
        aSourceIndex += size;
        aDestIndex += size;
        }
    else
        {
        aSourceIndex += aSize;
        }
    return aDestSize - aDestIndex; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt SerializedFunctionCall::WriteParam( const TUint8* aParam, const TInt aParamSize )
    {
    ASSERT( aParam );

    return Write( iWriteState.iBuffer, iWriteState.iDestIndex, iWriteState.iStreamMaxLength,
            aParam, aParamSize, iWriteState.iSourceIndex, iWriteState.iOffset, 0 );        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt SerializedFunctionCall::WriteVector( const TUint8* aParam, const TInt32 aDataTypeSize, const TInt aVectorSize, const TInt8 aDataTypeAlignment )
    {
    ASSERT( aParam );
    iWriteState.iDestIndex = RemoteFunctionCallData::AlignIndex( iWriteState.iDestIndex, aDataTypeAlignment );
    return Write( iWriteState.iBuffer, iWriteState.iDestIndex, iWriteState.iStreamMaxLength,
            aParam, aVectorSize*aDataTypeSize, iWriteState.iSourceIndex, iWriteState.iOffset, 1 );        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt SerializedFunctionCall::ReadParam( TUint8* aParam, const TInt aParamSize )
    {
    return Read( aParam, iReadState.iReadDestOffset, aParamSize, 
        iReadState.iReadBuffer, iReadState.iReadBufferIndex, iReadState.iReadBufferLength, 0 );        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt SerializedFunctionCall::ReadVectorParam( TUint8* aParam, 
		const TInt32 aDataTypeSize, const TInt aVectorSize, const TInt8 aDataTypeAlignment )
    {
	iReadState.iReadBufferIndex = RemoteFunctionCallData::AlignIndex( iReadState.iReadBufferIndex, aDataTypeAlignment );
    return Read( aParam, iReadState.iReadDestOffset, aVectorSize*aDataTypeSize, 
        iReadState.iReadBuffer, iReadState.iReadBufferIndex, iReadState.iReadBufferLength, 1 );        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint8* SerializedFunctionCall::ReadVectorPointer( 
		const TInt32 aDataTypeSize, const TInt aVectorSize, const TInt8 aDataTypeAlignment )
    {
	iReadState.iReadBufferIndex = RemoteFunctionCallData::AlignIndex( iReadState.iReadBufferIndex, aDataTypeAlignment );
	TUint8* data = const_cast<TUint8*>(&iReadState.iReadBuffer[iReadState.iReadBufferIndex]);
	iReadState.iReadBufferIndex += aVectorSize*aDataTypeSize;
    iReadState.iReadDestOffset += 0;
	return data;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt SerializedFunctionCall::Read( TUint8* aDest, TInt& aDestIndex, const TInt aDestSize,
        const TUint8* aSource, TInt& aSourceIndex, const TInt aSourceSize, TBool aVector )
    {
    TInt ret(1);
    TInt size = aDestSize - aDestIndex;
    if ( aSourceIndex + size > aSourceSize )
        {
        //Read buffer overflow
        iError = EMoreData;
        ret = 0;
        size = aSourceSize - aSourceIndex; 
        }
    
    if ( iDataCopier && aVector )
        {
        iDataCopier->CopyVector( &aDest[aDestIndex], &aSource[aSourceIndex], size );
        }
    else if ( iDataCopier )
        {
        iDataCopier->CopyData( &aDest[aDestIndex], &aSource[aSourceIndex], size );            
        }
    else
        {
		RemoteFunctionCallData::CopyData( &aDest[aDestIndex], &aSource[aSourceIndex], size );
        }

    aSourceIndex += size;
    aDestIndex += size;
    
    if ( ret )
        {
        aDestIndex = 0;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void SerializedFunctionCall::StartRead( const TUint8* aBuffer, const TInt aBufferLength )
    {
    iReadState.iReadBuffer = aBuffer;
    iReadState.iReadBufferIndex = 0;
    iReadState.iReadBufferLength = aBufferLength;
    iReadState.iParseState = EReadingApiUid;
    iReadState.iReadDestOffset = 0;
    iError = ESuccess;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt SerializedFunctionCall::ReadHeaderFromBuffer( const TUint8* aBuffer, const TInt aBufferLength )
    {
    iReadState.iParamsIndex = 0;
    StartRead( aBuffer, aBufferLength );

    if ( EReadingApiUid == iReadState.iParseState )
        {
        if ( !ReadParam( (TUint8*)&iRFC.HeaderData().iApiUid, 
                sizeof(iRFC.HeaderData().iApiUid) ) )
            {
            return 0;
            }
        iReadState.iParseState = EReadingOpCode;
        }

	if ( EReadingOpCode == iReadState.iParseState )
        {
        if ( !ReadParam( (TUint8*)&iRFC.HeaderData().iOpCode, 
                sizeof(iRFC.HeaderData().iOpCode) ) )
            {
            return 0;
            }
        iReadState.iParseState = EReadingTransactionId;
        }

    if ( iReadState.iParseState == EReadingTransactionId )
        {
        if ( !ReadParam( (TUint8*)&iRFC.HeaderData().iTransactionId, 
                sizeof(iRFC.HeaderData().iTransactionId) ) )
            {
            return 0;
            }
        iReadState.iParseState = EReadingParameterCount;
        }
    if ( iReadState.iParseState == EReadingParameterCount )
        {
        if ( !ReadParam( (TUint8*)&iRFC.HeaderData().iParameterCount, 
                sizeof(iRFC.HeaderData().iParameterCount) ) )
            {
            return 0;
            }
        iReadState.iParseState = EReadingProcessId;
        }
    if ( iReadState.iParseState == EReadingProcessId )
        {
        if ( !ReadParam( (TUint8*)&iRFC.HeaderData().iProcessId, 
                sizeof(iRFC.HeaderData().iProcessId) ) )
            {
            return 0;
            }
        iReadState.iParseState = EReadingThreadId;
        }
    if ( iReadState.iParseState == EReadingThreadId )
        {
        if ( !ReadParam( (TUint8*)&iRFC.HeaderData().iThreadId, 
                sizeof(iRFC.HeaderData().iThreadId) ) )
            {
            return 0;
            }
        iReadState.iParseState = EReadingOpType;
        }
    if ( iReadState.iParseState == EReadingOpType )
        {
        if ( !ReadParam( (TUint8*)&iRFC.HeaderData().iOpType, 
                sizeof(iRFC.HeaderData().iOpType) ) )
            {
            return 0;
            }
        iReadState.iParseState = EReadingReturnValue;
        }
    if ( iReadState.iParseState == EReadingReturnValue )
        {
        if ( !ReadParam( (TUint8*)&iRFC.HeaderData().iReturnValue, 
                sizeof(iRFC.HeaderData().iReturnValue) ) )
            {
            return 0;
            }
        iReadState.iParseState = EReadingParamDir;
        }
    
    return 1;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C TInt SerializedFunctionCall::ParseBuffer( const TUint8* aBuffer, const TInt aBufferLength )
    {
    StartRead( aBuffer, aBufferLength );
    if ( ReadHeaderFromBuffer( aBuffer, aBufferLength ) )
        {        
        for ( ;iReadState.iParamsIndex < iRFC.Header().iParameterCount; iReadState.iParamsIndex++ )
            {
			RemoteFunctionCallData::TParam& param( iRFC.Parameters()[iReadState.iParamsIndex] );
    
            ReadParam( (TUint8*)&param.iDir, sizeof(param.iDir) );
            if ( ReadParam( (TUint8*)&param.iType, sizeof(param.iType) ) )
                {
    
                if ( RemoteFunctionCallData::TParam::ESimple == param.iType )
                    {
                    ReadParam( (TUint8*)&param.iSimpleParam.iDataType, sizeof(param.iSimpleParam.iDataType) ); 
                    ReadParam( (TUint8*)&param.iSimpleParam.iDataTypeSize, sizeof(param.iSimpleParam.iDataTypeSize) ); 
                    ReadParam( (TUint8*)&param.iSimpleParam.iData, param.iSimpleParam.iDataTypeSize );
                    }
                else if ( RemoteFunctionCallData::TParam::EVector == param.iType )
                    {
                    ReadParam( (TUint8*)&param.iVectorParam.iDataType, sizeof(param.iVectorParam.iDataType) );
                    ReadParam( (TUint8*)&param.iVectorParam.iVectorLength, sizeof(param.iVectorParam.iVectorLength) );            
                    ReadParam( (TUint8*)&param.iVectorParam.iCallerAddress, sizeof(param.iVectorParam.iCallerAddress) );
                    ReadParam( (TUint8*)&param.iVectorParam.iDataTypeSize, sizeof(param.iVectorParam.iDataTypeSize) );
                    ReadParam( (TUint8*)&param.iVectorParam.iDataTypeAlignment, sizeof(param.iVectorParam.iDataTypeAlignment) );
                    
                    if ( !param.iVectorParam.iVectorLength )
                        {
                        param.iVectorParam.iData = NULL;
						param.iVectorParam.iCallerAddress = NULL;
                        }
                    else if ( (param.iDir & RemoteFunctionCallData::EIn) && ( RemoteFunctionCallData::EOpRequest == iRFC.Header().iOpType || RemoteFunctionCallData::EOpRequestWithReply == iRFC.Header().iOpType ) )
                        {
                        param.iVectorParam.iData = ReadVectorPointer( param.iVectorParam.iDataTypeSize, param.iVectorParam.iVectorLength, param.iVectorParam.iDataTypeAlignment );
                        }
                    else if ( (param.iDir & RemoteFunctionCallData::EOut) && iRFC.Header().iOpType == RemoteFunctionCallData::EOpReply ) 
                        {
                        ReadVectorParam( (TUint8*)param.iVectorParam.iCallerAddress, 
                                param.iVectorParam.iDataTypeSize, param.iVectorParam.iVectorLength, param.iVectorParam.iDataTypeAlignment );
                        }
                    }
                else
                    {
                    //Parse error
                    iError = EParseError;
                    return -1;
                    }
                }
            }
        iReadState.iParseState = EReadingDone;
        }
    else
        {
        iError = EParseError;
        return -1;
        }
    return iReadState.iReadBufferIndex;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void SerializedFunctionCall::StartWrite( TUint8* aBuffer, const TInt aStreamMaxLength, TInt aOffset )
    {
    iWriteState.iBuffer = aBuffer;
    iWriteState.iDestIndex = 0;
    iWriteState.iStreamMaxLength = aStreamMaxLength;
    iWriteState.iSourceIndex = 0;
    iWriteState.iOffset = aOffset;
    iError = ESuccess;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt SerializedFunctionCall::WriteHeaderToBuffer( TUint8* aBuffer, const TInt aStreamMaxLength, TInt aOffset )
    {
    StartWrite( aBuffer, aStreamMaxLength, aOffset );
    if ( !WriteParam( (TUint8*)&iRFC.Header().iApiUid, 
            sizeof(iRFC.Header().iApiUid) ) )
        {
        return iWriteState.iSourceIndex;
        }
    if ( !WriteParam( (TUint8*)&iRFC.Header().iOpCode, 
            sizeof(iRFC.Header().iOpCode) ) )
        {
        return iWriteState.iSourceIndex;
        }
    if ( !WriteParam( (TUint8*)&iRFC.Header().iTransactionId, 
            sizeof(iRFC.Header().iTransactionId) ) )
        {
        return iWriteState.iSourceIndex;
        }
    if ( !WriteParam( (TUint8*)&iRFC.Header().iParameterCount, 
            sizeof(iRFC.Header().iParameterCount) ) )
        {
        return iWriteState.iSourceIndex;
        }
    if ( !WriteParam( (TUint8*)&iRFC.Header().iProcessId, 
            sizeof(iRFC.Header().iProcessId) ) )
        {
        return iWriteState.iSourceIndex;
        }
    if ( !WriteParam( (TUint8*)&iRFC.Header().iThreadId, 
            sizeof(iRFC.Header().iThreadId) ) )
        {
        return iWriteState.iSourceIndex;
        }
    if ( !WriteParam( (TUint8*)&iRFC.Header().iOpType, 
            sizeof(iRFC.Header().iOpType) ) )
        {
        return iWriteState.iSourceIndex;
        }
    if ( !WriteParam( (TUint8*)&iRFC.Header().iReturnValue, 
            sizeof(iRFC.Header().iReturnValue) ) )
        {
        return iWriteState.iSourceIndex;
        }
    return iWriteState.iSourceIndex;
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C TInt SerializedFunctionCall::WriteToBuffer( TUint8* aBuffer, const TInt aStreamMaxLength, TInt aOffset )
    {
    WriteHeaderToBuffer( aBuffer, aStreamMaxLength, aOffset );
    for ( TInt i = 0; i < iRFC.Header().iParameterCount; i++ )
        {
        const RemoteFunctionCallData::TParam& param( iRFC.Parameters()[i] );
        
        if ( !WriteParam( (TUint8*)&param.iDir, 
                sizeof(param.iDir) ) )
            {
            break;
            }

        if ( !WriteParam( (TUint8*)&param.iType, 
                sizeof(param.iType) ) )
            {
            break;
            }
        
		if ( RemoteFunctionCallData::TParam::ESimple == param.iType )
            {
            if ( !WriteParam( (TUint8*)&param.iSimpleParam.iDataType, 
                    sizeof(param.iSimpleParam.iDataType) ) )
                {
                break;
                }
            if ( !WriteParam( (TUint8*)&param.iSimpleParam.iDataTypeSize, 
                    sizeof(param.iSimpleParam.iDataTypeSize) ) )
                {
                break;
                }

			if ( !WriteParam( (TUint8*)&param.iSimpleParam.iData, param.iSimpleParam.iDataTypeSize ) )
                {
                break;
                }
            }
        else if ( RemoteFunctionCallData::TParam::EVector == param.iType )
            {
            if ( !WriteParam( (TUint8*)&param.iVectorParam.iDataType, sizeof(param.iVectorParam.iDataType) ))
                {
                break;
                }

            if ( !WriteParam( (TUint8*)&param.iVectorParam.iVectorLength, sizeof(param.iVectorParam.iVectorLength) ))
                {
                break;
                }

            if ( !WriteParam( (TUint8*)&param.iVectorParam.iCallerAddress, sizeof(param.iVectorParam.iCallerAddress) ))
                {
                break;
                }

            if ( !WriteParam( (TUint8*)&param.iVectorParam.iDataTypeSize, sizeof(param.iVectorParam.iDataTypeSize) ))
                {
                break;
                }

            if ( !WriteParam( (TUint8*)&param.iVectorParam.iDataTypeAlignment, sizeof(param.iVectorParam.iDataTypeAlignment) ))
                {
                break;
                }

            if ( param.iVectorParam.iVectorLength && ( ( (param.iDir & RemoteFunctionCallData::EIn) && ( RemoteFunctionCallData::EOpRequestWithReply == iRFC.Header().iOpType || RemoteFunctionCallData::EOpRequest == iRFC.Header().iOpType ) ) ||
                    ( (param.iDir & RemoteFunctionCallData::EOut) && iRFC.Header().iOpType == RemoteFunctionCallData::EOpReply ) ) )
                {
                //Only serialise vector data if data is valid and the direction matches
                if ( !WriteVector( param.iVectorParam.iData, param.iVectorParam.iDataTypeSize, param.iVectorParam.iVectorLength, param.iVectorParam.iDataTypeAlignment ) )
                    {
                    break;
                    }
                }
            }
        }
    
    return iWriteState.iDestIndex;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C void SerializedFunctionCall::SetDataCopier( MDataCopier* aDataCopier )
    {
    iDataCopier = aDataCopier;        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
SER_EXPORT_C SerializedFunctionCall::SerializedFunctionCall( RemoteFunctionCallData& aRFC, MDataCopier* aDataCopier ):
	iRFC( aRFC ),
    iDataCopier( aDataCopier )
    {
    }
