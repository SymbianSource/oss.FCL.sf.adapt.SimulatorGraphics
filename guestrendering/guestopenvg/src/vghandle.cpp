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
// Client-side state information for Open VG handle based objects.

#include "vgstate.h"
#include "remotefunctioncall.h"
#include "openvgrfc.h"


// defintions for all static member data for CVgHandleBase family of classes
THandleGen   CVgHandleBase::iHandleGen; // for creating local UIDs for clients to use as VG Handles



/////////////////////////////////////////////////////////////////////////////////////////////
// THandleGen
/////////////////////////////////////////////////////////////////////////////////////////////

void THandleGen::Init(TUint aInit)
	{
	OPENVG_TRACE("THandleGen::Init aInit=0x%x", aInit);
	VGPANIC_ASSERT_DEBUG(aInit != 0, EVgPanicUidGeneratorZeroSeed);
	iValue = aInit;
	}


TUint THandleGen::NextUid()
	{
	VGPANIC_ASSERT_DEBUG(iValue != 0, EVgPanicUidGeneratorZeroSeed);
	TUint result = iValue;
	++iValue;
	if (iValue == 0)
		iValue = 1;
	return result;
	}


/////////////////////////////////////////////////////////////////////////////////////////////
// CVgHandleBase = base class for CVgImageInfo etc...
/////////////////////////////////////////////////////////////////////////////////////////////

// Client Handle generates local UIDs for each VG Handle based object created
void CVgHandleBase::InitStatics()
	{
	// valued of first Client side VG Handle
	TUint seed = 0x00010000;
	iHandleGen.Init(seed);
	}


void CVgHandleBase::Destroy(MVgContext& aVgContext)
	{
	VGPANIC_ASSERT_DEBUG(!IsDestroyed(), EVgPanicHandleAlreadyMarkedDestroyed);
	iIsDestroyed = ETrue;
	if (DestroyObject(aVgContext))
		{
		RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
		OpenVgState.UnMapHandle(iClientHandle);
		delete this;
		CVghwUtils::SwitchFromVghwHeap(clientHeap);
		}
	}


VGint CVgHandleBase::HostVgGetParameterVectorSize(MVgContext& aVgContext, VGint aParamType)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetParameterVectorSize);
	vgApiData.AppendParam(HostHandle());
	vgApiData.AppendParam(aParamType);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return static_cast<VGint>(vgApiData.ReturnValue());
	}


VGint CVgHandleBase::HostVgGetParameteri(MVgContext& aVgContext, VGint aParamType)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetParameteri);
	vgApiData.AppendParam(HostHandle());
	vgApiData.AppendParam(aParamType);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return static_cast<VGint>(vgApiData.ReturnValue());
	}


VGfloat CVgHandleBase::HostVgGetParameterf(MVgContext& aVgContext, VGint aParamType)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetParameterf);
	vgApiData.AppendParam(HostHandle());
	vgApiData.AppendParam(aParamType);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return static_cast<VGfloat>(vgApiData.ReturnValue());
	}


void CVgHandleBase::HostVgGetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGfloat * aValues)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetParameterfv);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aParamType);
	vgApiData.AppendParam(aCount);
	vgApiData.AppendVector(aValues, aCount, RemoteFunctionCallData::EOut);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgHandleBase::HostVgGetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGint * aValues)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetParameteriv);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aParamType);
	vgApiData.AppendParam(aCount);
	vgApiData.AppendVector(aValues, aCount, RemoteFunctionCallData::EOut);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgHandleBase::HostVgSetParameterf(MVgContext& aVgContext, VGint aParamType, VGfloat aValue)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetParameterf, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aParamType);
	vgApiData.AppendParam(aValue);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgHandleBase::HostVgSetParameteri(MVgContext& aVgContext, VGint aParamType, VGint aValue)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetParameteri, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aParamType);
	vgApiData.AppendParam(aValue);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgHandleBase::HostVgSetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGfloat * aValues)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetParameterfv, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aParamType);
	vgApiData.AppendParam(aCount);
	vgApiData.AppendVector(aValues, aCount);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgHandleBase::HostVgSetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGint * aValues)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetParameteriv, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aParamType);
	vgApiData.AppendParam(aCount);
	vgApiData.AppendVector(aValues, aCount);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


// Base version of  GetParameter* use GetParameterVectorSize to validate aParamType then cal Host VG
VGfloat CVgHandleBase::GetParameterf(MVgContext& aVgContext, VGint aParamType)
	{
	VGint vSize = GetParameterVectorSize(aVgContext, aParamType);
	if (vSize > 0)
		{
		return HostVgGetParameterf(aVgContext, aParamType);
		}
	return KFloatMinusOne;
	}


VGint CVgHandleBase::GetParameteri(MVgContext& aVgContext, VGint aParamType)
	{
	VGint vSize = GetParameterVectorSize(aVgContext, aParamType);
	if (vSize > 0)
		{
		return HostVgGetParameteri(aVgContext, aParamType);
		}
	return 0;
	}


/* Base versions of GetParameter*v call:
   * GetParameterVectorSize to validate aParamtType, 
   * if aCount is 1 they call GetParameterf/i for the object
   * otherwise if aCount is valid they call HostVgGetParameter*v
   * */
void CVgHandleBase::GetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, VGfloat * aValues)
	{
	VGint vSize = GetParameterVectorSize(aVgContext, aParamType);
	if (vSize > 0)
		{
		if (aCount == 1)
			{
			*aValues = GetParameterf(aVgContext, aParamType);
			return;
			}
		else if ( (aCount > 0) && (aCount <= vSize) )
			{
			return HostVgGetParameterfv(aVgContext, aParamType, aCount, aValues);
			}

		// invalid aCount
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return;
	}


void CVgHandleBase::GetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, VGint * aValues)
	{
	VGint vSize = GetParameterVectorSize(aVgContext, aParamType);
	if (vSize > 0)
		{
		if (aCount == 1)
			{
			*aValues = GetParameteri(aVgContext, aParamType);
			return;
			}
		else if ( (aCount > 0) && (aCount <= vSize) )
			{
			return HostVgGetParameteriv(aVgContext, aParamType, aCount, aValues);
			}

		// invalid aCount
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return;
	}


// Base versions of SetParameter* assume any parameters are read-only, and give an error return
void CVgHandleBase::SetParameterf(MVgContext& aVgContext, VGint, VGfloat)
	{
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	}


void CVgHandleBase::SetParameteri(MVgContext& aVgContext, VGint, VGint)
	{
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	}


void CVgHandleBase::SetParameterfv(MVgContext& aVgContext, VGint, VGint, const VGfloat *)
	{
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	}


void CVgHandleBase::SetParameteriv(MVgContext& aVgContext, VGint, VGint, const VGint *)
	{
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	}


// end of file vghandle.cpp
