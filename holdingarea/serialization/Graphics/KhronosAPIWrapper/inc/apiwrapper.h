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
// 

#ifndef APIWRAPPER_H
#define APIWRAPPER_H

#ifndef PSU_PLATFORMTHREADING_H
#include "platformthreading.h"
#endif

#ifndef PSU_PLATFORMTYPES_H
#include "platformtypes.h"
#endif
#include "remotefunctioncall.h"

class RemoteFunctionCall;
class MGraphicsVHWCallback;

class APIWrapperStack
{
    public:
        APIWrapperStack(): iStack( NULL ), iStackIndex(0), iStackSize(0)
        {
        }

        bool InitStack( int stackSize )
        {
            if ( iStack )
            {
                delete [] iStack;
            }

            iStack = (char*)new unsigned long[(stackSize+3)/4];
            iStackIndex = 0;
            iStackSize = stackSize;
            return iStack != NULL;
        }

        void* AllocFromStack( int size, int alignment )
        {
            unsigned int adjustedIndex = RemoteFunctionCallData::AlignIndex( iStackIndex, alignment );
            if ( adjustedIndex + size < iStackSize )
            {
                iStackIndex = adjustedIndex + size;
                return (void*)(iStack + adjustedIndex);
            }
            else
            {
                return NULL;
            }
        }

        void ClearStack()
        {
            iStackIndex = 0;
        }

        ~APIWrapperStack()
        {
            delete [] iStack;
            iStack = NULL;
        }

    private:

        char* iStack;
        unsigned int iStackIndex;
        unsigned int iStackSize;
};


class APIWrapper
{
public:
	virtual ~APIWrapper();

	//void SetCurrentRFC(  );

	virtual void SetProcessInformation( TUint32 aProcess, TUint32 aThread ) = 0;
	virtual void Cleanup( TUint32 aProcess, TUint32 aThread ) = 0;
    virtual int DispatchRequest( unsigned long aCode ) = 0;

protected:
    APIWrapper( RemoteFunctionCallData& currentFunctionCall, 
		APIWrapperStack* stack,
        void* result,
		MGraphicsVHWCallback* serviceIf);

	virtual int WriteReply();

protected:
	RemoteFunctionCallData& m_currentFunctionCallData;
    void* m_currentResult;
    APIWrapperStack* iStack;
    MGraphicsVHWCallback* iServiceIf;
};

#endif
