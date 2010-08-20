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

#ifndef REMOTEFUNCTIONCALL_H_
#define REMOTEFUNCTIONCALL_H_

#include "serializerplatform.h"
#include <string.h>
class RemoteFunctionCall;

class RemoteFunctionCallData
    {
    friend class SerializedFunctionCall;
    friend class RemoteFunctionCall;
public:
    //Simple data copier
    SER_IMPORT_C static void CopyData( TUint8* aDest, const TUint8* aSource, TInt32 aSize );
    SER_IMPORT_C static TInt AlignIndex( TInt aIndex, const TInt32 aAlignment );
    
public:
    //Max number of parameters
    enum
        {
        KMaxParameterCount = 34
        };

    //Operation type
    enum TOperationType
        {
        EOpRequest,
        EOpRequestWithReply,
        EOpReply
        };

    //The direction of parameters
    enum TParamDirection
        {
        EIn = 1,
        EOut = 2,
        EInOut = 3
        };

	//Parameters of max 4 bytes of size
    class TSimpleParam
        {
		public:
	    TUint32 Size() const;
        TInt32 iDataType;
        TUint32 iData;
        TInt8 iDataTypeSize;
        };

    //Parameters with arbitrary size
    class TVectorParam
        {
		public:
	    TUint32 Size( TParamDirection aDir, TOperationType aOpType, TInt aIndexToAlign ) const;
        TInt32 iDataTypeSize;
        TInt32 iDataType;
        TUint32 iVectorLength;
        TUint8* iCallerAddress;
        const TUint8* iData;
        TInt8 iDataTypeAlignment;
        };

    //Parameter
    class TParam
        {
		public:
        enum TType
            {
            ESimple,
            EVector
            };

        TUint32 Size( TOperationType aOpType, TInt aIndexToAlign = 0 ) const;

		//Data
        TParamDirection iDir;
        TParam::TType iType;
        union
            {
            TSimpleParam iSimpleParam;
            TVectorParam iVectorParam;
            };
        };
    
    //Function call header, common information about function call
    struct THeader
        {
        TInt32 iOpCode;
        TUint32 iTransactionId;
        TUint32 iProcessId;
        TUint32 iThreadId;
        TInt32 iParameterCount;
        TInt32 iOpType;
        TUint32 iReturnValue;
		TUint32 iApiUid;
        };

    //Identifier for the call sequency
    SER_IMPORT_C void SetTransactionId( TUint32 aTransactionId );

    //Operation type
    SER_IMPORT_C void SetOperationType( TInt32 aOpType );

    //Return value
    SER_IMPORT_C void SetReturnValue( TUint32 aRetVal );

    //General information about function call
    SER_IMPORT_C const THeader& Header() const;

    //Parameters
    SER_IMPORT_C TInt ParameterCount();

    //Parameter array
    SER_IMPORT_C TParam* Parameters();

    //Sets thread information for request
    SER_IMPORT_C void SetThreadInformation( const TUint32 aProcessId, const TUint32 aThreadId );

    //Length of serialised function call
    SER_IMPORT_C TUint32 SerialisedLength() const;

    //Get pointer to vector data
    SER_IMPORT_C void GetVectorData( TInt32 aParamType, void** aData, TInt& aSize, TInt aIndex );

    //Get parameter value
    SER_IMPORT_C void GetParamValue( TInt32 aParamType, TUint8* aData, TInt aIndex );

protected:
    //General information about function call
    THeader& HeaderData();

    //Function information
    THeader iHeader;

    //Function parameters
    TParam iParameters[KMaxParameterCount];
	};

//Remote function call
// Represents a function call and contains information about 
// parameters but does not serialize the data
class RemoteFunctionCall
    {
public:
    SER_IMPORT_C RemoteFunctionCall( RemoteFunctionCallData& aData );
    
    //Initialises this object
    SER_IMPORT_C void Init( TInt32 aOpCode, RemoteFunctionCallData::TOperationType aOpType = RemoteFunctionCallData::EOpRequestWithReply );

    SER_IMPORT_C RemoteFunctionCallData& Data();
    
    SER_IMPORT_C TUint32 ReturnValue();

    SER_IMPORT_C void SetReturnValue( TUint32 aRetValue );

    //Size of type aType
	virtual TInt GetTypeSize( TInt32 aParamType ) const = 0;

    //Alignemnt of type aType
	virtual TInt GetTypeAlignment( TInt32 aParamType ) const = 0;

    //Alignemnt of type aType
	virtual TUint32 GetAPIUid() const = 0;

protected:
	SER_IMPORT_C void AppendParam( TInt32 aParamType, const TUint8* aData, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    SER_IMPORT_C void AppendVector( TInt32 aParamType, TUint32 aLength, const TUint8* aData, RemoteFunctionCallData::TParamDirection aDir = RemoteFunctionCallData::EIn );
    SER_IMPORT_C void SetParamData( TInt32 aParamType, const TUint8* aData, TInt aIndex );
    SER_IMPORT_C void SetVectorData( TInt32 aParamType, const TUint8* aData, TInt aLength, TInt aIndex );

protected:
    RemoteFunctionCallData& iData;
    };

#endif
