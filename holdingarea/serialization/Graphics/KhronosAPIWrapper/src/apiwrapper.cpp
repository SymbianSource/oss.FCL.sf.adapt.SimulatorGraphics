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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN                       // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include "serializedfunctioncall.h"
#include "remotefunctioncall.h"
#include "apiwrapper.h"
#include "graphicsvhwcallback.h"        //callback interface Wrapper=>Virtual HW
#include "khronosapiwrapperdefs.h"

APIWrapper::~APIWrapper()
{
}
/*
void APIWrapper::SetCurrentRFC( RemoteFunctionCallData* currentFunctionCall )
{
	m_currentFunctionCallData = currentFunctionCall;
}*/

APIWrapper::APIWrapper( RemoteFunctionCallData& currentFunctionCall, 
	APIWrapperStack* stack,
    void* result,
	MGraphicsVHWCallback* serviceIf):
m_currentFunctionCallData( currentFunctionCall ),
m_currentResult( result ),
iStack( stack ),
iServiceIf( serviceIf )
{
}

int APIWrapper::WriteReply()
{
	int ret( 0 );
	if ( RemoteFunctionCallData::EOpRequestWithReply == m_currentFunctionCallData.Header().iOpType )
	{
		TRACE("APIWrapper::WriteReply() retval finally:%d\n", m_currentFunctionCallData.Header().iReturnValue );
		m_currentFunctionCallData.SetOperationType( RemoteFunctionCallData::EOpReply );
		iServiceIf->LockOutputBuffer();
		SerializedFunctionCall sfc( m_currentFunctionCallData );
		ret = sfc.WriteToBuffer( (TUint8*)m_currentResult, VVI_PARAMETERS_OUTPUT_MEMORY_SIZE );
	}
	return ret;
}
