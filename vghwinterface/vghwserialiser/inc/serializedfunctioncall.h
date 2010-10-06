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

#ifndef SERIALIZEDFUNCTIONCALL_H_
#define SERIALIZEDFUNCTIONCALL_H_

#include "serializerplatform.h"

class RemoteFunctionCallData;
//Copy operations for simple and array parameters
class MDataCopier
    {
public:
    virtual TInt CopyData( TUint8* aDest, const TUint8* aSource, TUint32 aSize ) = 0; 
    virtual TInt CopyVector( TUint8* aDest, const TUint8* aSource, TUint32 aSize ) = 0;
    };

/**
 * Serialized function call.
 * Serializes the parameters to a given memory location. 
 */ 
class SerializedFunctionCall
    {
public:    
    
    //Parse error
    enum TError
        {
        ESuccess,
        EMoreData,
        EParseError
        };
    
    //Parse state
    enum TParseState
        {
        EReadingApiUid,
        EReadingOpCode,
        EReadingTransactionId,
        EReadingParameterCount,
        EReadingProcessId,
        EReadingThreadId,
        EReadingOpType,
        EReadingReturnValue,
        EReadingParamDir,
        EReadingParamType,
        EReadingSimpleParamType,
        EReadingSimpleParamData,
        EReadingVectorParamType,
        EReadingVectorParamDataSize,
        EReadingVectorParamVectorLength,
        EReadingVectorParamCallerAddress,
        EReadingVectorParamData,
        EReadingDone
        };
    
    //Constructor
	SER_IMPORT_C SerializedFunctionCall( RemoteFunctionCallData& aRFC, MDataCopier* aDataCopier = NULL );

    SER_IMPORT_C void SetDataCopier( MDataCopier* aDataCopier );

    //Parses a serialized function call. Updates parameter array and utilises data copier to
    // copy array parameters
    SER_IMPORT_C TInt ParseBuffer( const TUint8* aBuffer, const TInt aBufferLength );
        
    //Serializes function call to a stream.
    SER_IMPORT_C TInt WriteToBuffer( TUint8* aBuffer, const TInt aStreamMaxLength, TInt aOffset = 0 );
    
private:

    //Writes header to stream
    TInt WriteHeaderToBuffer( TUint8* aBuffer, const TInt aStreamMaxLength, TInt aOffset = 0 );

    //Reads header from buffer
    TInt ReadHeaderFromBuffer( const TUint8* aBuffer, const TInt aBufferLength );

    //Writes a data. Accounts destination buffer overflow and can continue write
    // from a given offset
    TInt Write( TUint8* aDest, TInt& aDestIndex, const TInt aDestSize,
            const TUint8* aSource, const TInt aSize, TInt& aSourceIndex, 
            const TInt aOffset, TBool aVector );
    
    //Serializes a simple param to a current stream
    TInt WriteParam( const TUint8* aParam, const TInt aParamSize );

    //Serializes an array param to a current stream
    //TInt WriteVector( const TUint8* aParam, const TInt aParamSize );
	TInt WriteVector( const TUint8* aParam, const TInt32 aDataTypeSize, const TInt aVectorSize, const TInt8 aDataTypeAlignment );

    //Initialises write state
    void StartWrite( TUint8* aBuffer, const TInt aStreamMaxLength, TInt aOffset = 0 );
    
    //Gets a direct pointer to the stream data for an array parameter
	//TUint8* ReadVector( const TInt aParamSize );
	TUint8* ReadVectorPointer( const TInt32 aDataTypeSize, const TInt aVectorSize, const TInt8 aDataTypeAlignment );
	
	//Reads simple parameter from a stream
    TInt ReadParam( TUint8* aParam, const TInt aParamSize );
    
    //Reads an array parameter from a stream and utilises data copier for data transfer
	//TInt ReadVectorParam( TUint8* aParam, const TInt aParamSize );
	TInt ReadVectorParam( TUint8* aParam, const TInt32 aDataTypeSize, const TInt aVectorSize, const TInt8 aDataTypeAlignment );

    //Reads a data. Accounts source buffer overflow and can continue read
    // from a given offset
	TInt Read( TUint8* aDest, TInt& aDestIndex, const TInt aDestSize,
            const TUint8* aSource, TInt& aSourceIndex, const TInt aSourceSize, TBool aVector );

    //Initialises a read state
    void StartRead( const TUint8* aBuffer, const TInt aBufferLength );

    //Last error
    TError GetLastError() { return iError; }

	RemoteFunctionCallData& GetRemoteFunctionCall() { return iRFC; }

private:
	RemoteFunctionCallData& iRFC;
    MDataCopier* iDataCopier;

    TError iError;

	struct WriteState
		{
		//Write state
		TUint8* iBuffer;
		TInt iDestIndex;
		TInt iStreamMaxLength;
		TInt iSourceIndex;
		TInt iOffset;        
		};

	struct ReadState
		{
		//Read state
		TInt iReadDestOffset;
		const TUint8* iReadBuffer;
		TInt iReadBufferIndex;
		TInt iReadBufferLength;
		TParseState iParseState;
		TInt iParamsIndex;
		};

	union 
		{
		WriteState iWriteState;
		ReadState iReadState;
		};

	};

#endif
