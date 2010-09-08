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
// CVgPaintInfo
/////////////////////////////////////////////////////////////////////////////////////////////

CVgPaintInfo* CVgPaintInfo::New()
	{
	RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
	CVgPaintInfo* self = new CVgPaintInfo();
	CVghwUtils::SwitchFromVghwHeap(clientHeap);
	return self;
	}


CVgPaintInfo::~CVgPaintInfo()
	{}


CVgPaintInfo::CVgPaintInfo() :
		CVgHandleBase(EVgHandleForPaint),
		iPaintType(VG_PAINT_TYPE_COLOR), iColorRampSpreadMode(VG_COLOR_RAMP_SPREAD_PAD),
		iColorRampPremultiplied(VG_TRUE), iPatternTilingMode(VG_TILE_FILL)
	{}


TBool CVgPaintInfo::DestroyObject(MVgContext& aVgContext)
	{
	VGPANIC_ASSERT_DEBUG(iIsDestroyed, EVgPanicTemp);
	OPENVG_TRACE("  CVgPaintInfo::DestroyObject HostHandle=0x%x", iHostHandle);

	if (iHostHandle)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgDestroyPaint, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(iHostHandle);
		aVgContext.ExecuteVgCommand(vgApiData);
		}

	return ETrue;
	}


VGint CVgPaintInfo::GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_PAINT_TYPE:
		case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
		case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
		case VG_PAINT_PATTERN_TILING_MODE:
			return 1;

		case VG_PAINT_COLOR:
		case VG_PAINT_LINEAR_GRADIENT:
			return 4;

		case VG_PAINT_COLOR_RAMP_STOPS:
			return 10000; // note: finite limit due to Guest serialisation limits 

		case VG_PAINT_RADIAL_GRADIENT:
			return 5;
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


VGfloat CVgPaintInfo::GetParameterf(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_PAINT_TYPE:
			return (VGfloat) iPaintType;
		case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
			return (VGfloat) iColorRampSpreadMode;
		case VG_PAINT_PATTERN_TILING_MODE:
			return (VGfloat) iPatternTilingMode;
		case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
			return (VGfloat) iColorRampPremultiplied;
		case VG_PAINT_COLOR:
		case VG_PAINT_COLOR_RAMP_STOPS:
		case VG_PAINT_LINEAR_GRADIENT:
		case VG_PAINT_RADIAL_GRADIENT:
			return HostVgGetParameterf(aVgContext, aParamType);
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


VGint CVgPaintInfo::GetParameteri(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_PAINT_TYPE:
			return iPaintType;
		case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
			return iColorRampSpreadMode;
		case VG_PAINT_PATTERN_TILING_MODE:
			return iPatternTilingMode;
		case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
			return iColorRampPremultiplied;
		case VG_PAINT_COLOR:
		case VG_PAINT_COLOR_RAMP_STOPS:
		case VG_PAINT_LINEAR_GRADIENT:
		case VG_PAINT_RADIAL_GRADIENT:
			return HostVgGetParameteri(aVgContext, aParamType);
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


void CVgPaintInfo::GetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, VGfloat * aValues)
	{
	if (aCount > 0)
		{
		// vSize = 0 means the ParamType is not valid for this object, and VG error is already set 
		VGint vSize = GetParameterVectorSize(aVgContext, aParamType);
		if (vSize == 0)
			{ // VG Error already set
			return;
			}

		if ( (aCount <= vSize) )
			{
			switch (aParamType)
				{
				case VG_PAINT_TYPE:
					*aValues = (VGfloat) iPaintType;
					return;
				case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
					*aValues = (VGfloat) iColorRampSpreadMode;
					return;
				case VG_PAINT_PATTERN_TILING_MODE:
					*aValues = (VGfloat) iPatternTilingMode;
					return;
				case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
					*aValues = (VGfloat) iColorRampPremultiplied;
					return;
				case VG_PAINT_COLOR:
				case VG_PAINT_COLOR_RAMP_STOPS:
				case VG_PAINT_LINEAR_GRADIENT:
				case VG_PAINT_RADIAL_GRADIENT:
					HostVgGetParameterfv(aVgContext, aParamType, aCount, aValues);
					return;
				}
			}
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return;
	}


void CVgPaintInfo::GetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, VGint * aValues)
	{
	if (aCount > 0)
		{
		// vSize = 0 means the ParamType is not valid for this object, and VG error is already set 
		VGint vSize = GetParameterVectorSize(aVgContext, aParamType);
		if (vSize == 0)
			{ // VG Error already set
			return;
			}

		if ( (aCount <= vSize) )
			{
			switch (aParamType)
				{
				case VG_PAINT_TYPE:
					*aValues = iPaintType;
					return;
				case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
					*aValues = iColorRampSpreadMode;
					return;
				case VG_PAINT_PATTERN_TILING_MODE:
					*aValues = iPatternTilingMode;
					return;
				case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
					*aValues = iColorRampPremultiplied;
					return;
				case VG_PAINT_COLOR:
				case VG_PAINT_COLOR_RAMP_STOPS:
				case VG_PAINT_LINEAR_GRADIENT:
				case VG_PAINT_RADIAL_GRADIENT:
					HostVgGetParameteriv(aVgContext, aParamType, aCount, aValues);
					return;
				}
			}
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return;
	}


void CVgPaintInfo::SetParameterf(MVgContext& aVgContext, VGint aParamType, VGfloat aValue)
	{
	switch (aParamType)
		{
		case VG_PAINT_TYPE:
		case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
		case VG_PAINT_PATTERN_TILING_MODE:
		case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
			SetParameteri(aVgContext, aParamType, (VGint) aValue);
			return;
			// **** desireable: add full checks for valid Values
		case VG_PAINT_COLOR:
		case VG_PAINT_COLOR_RAMP_STOPS:
		case VG_PAINT_LINEAR_GRADIENT:
		case VG_PAINT_RADIAL_GRADIENT:
			HostVgSetParameterf(aVgContext, aParamType, aValue);
			return;
		}
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR); // invalid or Read-Only parameter
	}


void CVgPaintInfo::SetParameteri(MVgContext& aVgContext, VGint aParamType, VGint aValue)
	{
	switch (aParamType)
		{
		case VG_PAINT_TYPE:
			if ( (aValue >= VG_PAINT_TYPE_COLOR) && (aValue <= VG_PAINT_TYPE_PATTERN) )
				{
				iPaintType = (VGPaintType) aValue;
				HostVgSetParameteri(aVgContext, aParamType, aValue);
				return;
				}
			break;
		case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
			if ( (aValue >= VG_COLOR_RAMP_SPREAD_PAD) && (aValue <= VG_COLOR_RAMP_SPREAD_REFLECT) )
				{
				iColorRampSpreadMode = (VGColorRampSpreadMode) aValue;
				HostVgSetParameteri(aVgContext, aParamType, aValue);
				return;
				}
			break;
		case VG_PAINT_PATTERN_TILING_MODE:
			if ( (aValue >= VG_TILE_FILL) && (aValue <= VG_TILE_REFLECT) )
				{
				iPatternTilingMode = (VGTilingMode) aValue;
				HostVgSetParameteri(aVgContext, aParamType, aValue);
				return;
				}
			break;
		case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
			if ( (aValue == VG_FALSE) || (aValue == VG_TRUE) )
				{
				iColorRampPremultiplied = (VGboolean) aValue;
				HostVgSetParameteri(aVgContext, aParamType, aValue);
				return;
				}
			break;
			// **** desireable: add full checks for valid Values
		case VG_PAINT_COLOR:
		case VG_PAINT_COLOR_RAMP_STOPS:
		case VG_PAINT_LINEAR_GRADIENT:
		case VG_PAINT_RADIAL_GRADIENT:
			HostVgSetParameteri(aVgContext, aParamType, aValue);
			return;
		}
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR); // invalid or Read-Only parameter
	}


void CVgPaintInfo::SetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGfloat * aValues)
	{
	VGint vSize = GetParameterVectorSize(aVgContext, aParamType);
	if (vSize > 0)
		{
		if ( (aCount == 0) && (aValues == NULL) )
			{ // pointless but legal according to Open VG 1.1 spec
			return;
			}

		if ( (aCount > 0) && (aCount <= vSize) )
			{
			switch (aParamType)
				{
				case VG_PAINT_TYPE:
				case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
				case VG_PAINT_PATTERN_TILING_MODE:
				case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
					SetParameteri(aVgContext, aParamType, (VGint) *aValues);
					return;
				case VG_PAINT_COLOR:
				case VG_PAINT_COLOR_RAMP_STOPS:
				case VG_PAINT_LINEAR_GRADIENT:
				case VG_PAINT_RADIAL_GRADIENT:
					HostVgSetParameterfv(aVgContext, aParamType, aCount, aValues);
					return;
				}
			}
		// VGPaint parameter is Read-Only or should have a smaller vector
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR); 
		}
	}


void CVgPaintInfo::SetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGint * aValues)
	{
	VGint vSize = GetParameterVectorSize(aVgContext, aParamType);
	if (vSize > 0)
		{
		if ( (aCount == 0) && (aValues == NULL) )
			{ // pointless but legal according to Open VG 1.1 spec
			return;
			}

		if ( (aCount > 0) && (aCount <= vSize) )
			{
			switch (aParamType)
				{
				case VG_PAINT_TYPE:
				case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
				case VG_PAINT_PATTERN_TILING_MODE:
				case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
					SetParameteri(aVgContext, aParamType, *aValues);
					return;
				case VG_PAINT_COLOR:
				case VG_PAINT_COLOR_RAMP_STOPS:
				case VG_PAINT_LINEAR_GRADIENT:
				case VG_PAINT_RADIAL_GRADIENT:
					HostVgSetParameteriv(aVgContext, aParamType, aCount, aValues);
					return;
				}
			}
		// VGPaint parameter is Read-Only or should have a smaller vector
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	}


void CVgPaintInfo::SetPaint(MVgContext& aVgContext, VGbitfield aPaintModes)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetPaint, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aPaintModes);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


//static
void CVgPaintInfo::ResetPaint(MVgContext& aVgContext)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetPaint, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(VG_INVALID_HANDLE);
	const VGuint KZero = 0; 
	vgApiData.AppendParam(KZero);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgPaintInfo::SetColor(MVgContext& aVgContext, VGuint aRgba)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetColor, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aRgba);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


VGuint CVgPaintInfo::GetColor(MVgContext& aVgContext)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetColor);
	vgApiData.AppendParam(iHostHandle);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return vgApiData.ReturnValue();
	}


void CVgPaintInfo::PaintPattern(MVgContext& aVgContext, CVgImageInfo* aPatternInfo)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgPaintPattern, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	TUint64 sgId(0L);
	if (aPatternInfo == NULL)
		{
		vgApiData.AppendParam(VG_INVALID_HANDLE);
		}
	else
		{
		vgApiData.AppendParam(aPatternInfo->HostHandle());
		if (aPatternInfo->IsEglSibling())
			{
			sgId = aPatternInfo->SgImageId();
			}
		}
	vgApiData.AppendTUint64(sgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


// end of file vgpaint.cpp
