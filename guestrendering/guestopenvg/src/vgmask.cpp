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
// Client-side state information for Open VG handle based objects

#include "vgstate.h"
#include "remotefunctioncall.h"
#include "openvgrfc.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CVgMaskLayerInfo
/////////////////////////////////////////////////////////////////////////////////////////////

CVgMaskLayerInfo* CVgMaskLayerInfo::New(VGint aWidth, VGint aHeight)
	{
	RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
	CVgMaskLayerInfo* self = new CVgMaskLayerInfo(aWidth, aHeight);
	CVghwUtils::SwitchFromVghwHeap(clientHeap);
	return self;
	}


CVgMaskLayerInfo::~CVgMaskLayerInfo()
	{}


CVgMaskLayerInfo::CVgMaskLayerInfo(VGint aWidth, VGint aHeight) :
		CVgImageBase(EVgHandleForMaskLayer, aWidth, aHeight)
	{}


TBool CVgMaskLayerInfo::DestroyObject(MVgContext& aVgContext)
	{
	VGPANIC_ASSERT_DEBUG(iIsDestroyed, EVgPanicTemp);
	OPENVG_TRACE("  CVgMaskLayerInfo::DestroyObject HostHandle=0x%x", iHostHandle);

	if (iHostHandle)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgDestroyMaskLayer, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(iHostHandle);
		aVgContext.ExecuteVgCommand(vgApiData);
		}

	return ETrue;
	}


VGint CVgMaskLayerInfo::GetParameterVectorSize(MVgContext& aVgContext, VGint)
	{
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR); // VGMasks do not have any parameters
	return 0;
	}


void CVgMaskLayerInfo::FillMaskLayer(MVgContext& aVgContext, VGint aX, VGint aY, VGint aWidth, VGint aHeight, VGfloat aValue)
	{
	if ( (aValue < 0.0f) || (aValue > 1.0f) )
		{
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	else
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgFillMaskLayer, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(iHostHandle);
		vgApiData.AppendParam(aX);
		vgApiData.AppendParam(aY);
		vgApiData.AppendParam(aWidth);
		vgApiData.AppendParam(aHeight);
		vgApiData.AppendParam(aValue);
		aVgContext.ExecuteVgCommand(vgApiData);
		}
	}


void CVgMaskLayerInfo::CopyMask(MVgContext& aVgContext, VGint aSx, VGint aSy, VGint aDx, VGint aDy, VGint aWidth, VGint aHeight)
	{
	// **** Desirable: check maskLayer is compatible with the current surface mask
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgCopyMask, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSx);
	vgApiData.AppendParam(aSy);
	vgApiData.AppendParam(aDx);
	vgApiData.AppendParam(aDy);
	vgApiData.AppendParam(aWidth);
	vgApiData.AppendParam(aHeight);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


// end of file vgmask.cpp
