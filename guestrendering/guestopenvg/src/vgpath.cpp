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
// Client-side state information for OpenVG handle based objects

#include "vgstate.h"
#include "remotefunctioncall.h"
#include "openvgrfc.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CVgPathInfo
/////////////////////////////////////////////////////////////////////////////////////////////

CVgPathInfo* CVgPathInfo::New(VGPathDatatype aDatatype, VGfloat aScale, VGfloat aBias, VGbitfield aCapabilities)
	{
	RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
	CVgPathInfo* self = new CVgPathInfo(aDatatype, aScale, aBias, aCapabilities);
	CVghwUtils::SwitchFromVghwHeap(clientHeap);
	return self;
	}


void CVgPathInfo::ClearPath(MVgContext& aVgContext, VGbitfield aCapabilities)
	{
	iCapabilities = aCapabilities & VG_PATH_CAPABILITY_ALL;
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgClearPath, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(HostHandle());
	vgApiData.AppendParam(aCapabilities);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgPathInfo::RemovePathCapabilities(MVgContext& aVgContext, VGbitfield aCapabilities)
	{
	if (aCapabilities & VG_PATH_CAPABILITY_ALL)
		{
		iCapabilities = (iCapabilities & ~aCapabilities);
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgRemovePathCapabilities, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(HostHandle());
		vgApiData.AppendParam(aCapabilities & VG_PATH_CAPABILITY_ALL);
		aVgContext.ExecuteVgCommand(vgApiData);
		}
	}


VGbitfield CVgPathInfo::GetPathCapabilities(MVgContext& aVgContext)
	{
	// value is tracked in Client side - but Debug builds verify state with the Host Open VG
#ifdef _DEBUG
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetPathCapabilities);
	vgApiData.AppendParam(HostHandle());
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	VGbitfield hostCapabilities = static_cast<VGbitfield>(vgApiData.ReturnValue());
	VGPANIC_ASSERT_DEBUG(hostCapabilities == iCapabilities, EVgPanicTemp);
#endif
	return iCapabilities;
	}


CVgPathInfo::~CVgPathInfo()
	{}

CVgPathInfo::CVgPathInfo(VGPathDatatype aDatatype, VGfloat aScale, VGfloat aBias, VGbitfield aCapabilities) :
	CVgHandleBase(EVgHandleForPath), iDatatype(aDatatype), iScale(aScale), iBias(aBias), iCapabilities(aCapabilities)
	{}


VGPathDatatype CVgPathInfo::PathDatatype() const
	{
	return iDatatype;
	}


TBool CVgPathInfo::CheckHasPathCapabilities(MVgContext& aVgContext, VGbitfield aCapabilities)
	{
	if ( aCapabilities == (aCapabilities & iCapabilities) )
		{
		return ETrue;
		}

	OPENVG_TRACE("CVgPathInfo::CheckHasPathCapabilities (iCapabilities=0x%x, aCapabilities=0x%x) setting VG_PATH_CAPABILITY_ERROR",
			iCapabilities, aCapabilities);
	aVgContext.SetVgError(VG_PATH_CAPABILITY_ERROR);
	return EFalse;
	}


TBool CVgPathInfo::CheckPathDataAlignment(MVgContext& aVgContext, const void* aPathData)
	{
	TBool result = EFalse;
	if (aPathData != NULL)
		{
		switch (iDatatype)
			{
			case VG_PATH_DATATYPE_S_8:
				result = ETrue;
				break;
			case VG_PATH_DATATYPE_S_16:
				result = !( 1ul & (unsigned)aPathData );
				break;
			case VG_PATH_DATATYPE_S_32:
			case VG_PATH_DATATYPE_F:
				result = !( 3ul & (unsigned)aPathData );
				break;
			}
		}
	if (!result)
		{
		OPENVG_TRACE("CVgPathInfo::CheckPathDataAlignment (iDatatype=%d, aPathData=0x%x) setting VG_ILLEGAL_ARGUMENT_ERROR",
				iDatatype, aPathData);
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return result;
	}


//
TBool CVgPathInfo::CheckPathDataSize(MVgContext& aVgContext, const VGubyte* aPathSegments, VGint aNumSegments, VGint* aPathDataSize)
	{
	TBool checkOkay = EFalse;
	VGint pathDataSize = 0;

	if ( (aNumSegments > 0) && (aPathSegments != NULL) )
		{
		checkOkay = ETrue;

#define SEGMENT_COMMAND(command) ((command) >> 1)
		// Number of coordinates for each command
		static const VGint KNumCoords[] = {0,2,2,1,1,4,6,2,4,5,5,5,5};

		for (VGint segIdx = 0; segIdx < aNumSegments; )
			{
			VGubyte segmentType = aPathSegments[segIdx++];
			if (segmentType > 25) // VGPathSegment max value 12 << 1 or'd with 0|1
				{
				OPENVG_TRACE("   CheckPathDataSize - unknown segmentType-0x%x", segmentType);
				checkOkay = EFalse;
				break;
				}
			VGint command = SEGMENT_COMMAND(segmentType);
			pathDataSize += KNumCoords[command];
			OPENVG_TRACE("   CheckPathDataSize segmentType=0x%x: command=0x%x, Total Coord Count=%d", segmentType, command, pathDataSize);
			}
		}

	if ( !checkOkay )
		{
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		OPENVG_TRACE("   CheckPathDataSize - Illegal Argument Error, aPathSegments=0x%x, aNumSegments=%d", aPathSegments, aNumSegments);
		}
	else
		{
		// Number of bytes for each VGPathDatatype
		static const VGint KNumBytes[] = {1,2,4,4};
		// convert coord count to bytes
		pathDataSize *= KNumBytes[iDatatype];

		*aPathDataSize = pathDataSize;
		OPENVG_TRACE("   CheckPathDataSize - success, pathDataSize=%d bytes", pathDataSize);
		}
	return checkOkay;
	}


// Check path parameters for vgAppendPathData
TBool CVgPathInfo::CheckAppendPathData(MVgContext& aVgContext, VGint aNumSegments, const VGubyte* aPathSegments, const void* aPathData,
		VGint* aPathDataSize)
	{
	return CheckPathDataAlignment(aVgContext, aPathData) && CheckPathDataSize(aVgContext, aPathSegments, aNumSegments, aPathDataSize);
	}


TBool CVgPathInfo::DestroyObject(MVgContext& aVgContext)
	{
	VGPANIC_ASSERT_DEBUG(iIsDestroyed, EVgPanicTemp);
	OPENVG_TRACE("  CVgPathInfo::DestroyObject HostHandle=0x%x", iHostHandle);

	if (iHostHandle)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgDestroyPath, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(iHostHandle);
		aVgContext.ExecuteVgCommand(vgApiData);
		}

	return ETrue;
	}


VGint CVgPathInfo::GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_PATH_FORMAT:
		case VG_PATH_DATATYPE:
		case VG_PATH_SCALE:
		case VG_PATH_BIAS:
		case VG_PATH_NUM_SEGMENTS:
		case VG_PATH_NUM_COORDS:
			return 1;
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


VGfloat CVgPathInfo::GetParameterf(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_PATH_DATATYPE:
			return (VGfloat) iDatatype;

		case VG_PATH_SCALE:
			return iScale;

		case VG_PATH_BIAS:
			return iBias;

		case VG_PATH_FORMAT:
		case VG_PATH_NUM_SEGMENTS:
		case VG_PATH_NUM_COORDS:
			return HostVgGetParameterf(aVgContext, aParamType);
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


VGint CVgPathInfo::GetParameteri(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_PATH_DATATYPE:
			return iDatatype;

		case VG_PATH_SCALE:
			return (VGint) iScale;

		case VG_PATH_BIAS:
			return (VGint) iBias;

		case VG_PATH_FORMAT:
		case VG_PATH_NUM_SEGMENTS:
		case VG_PATH_NUM_COORDS:
			return HostVgGetParameteri(aVgContext, aParamType);
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


void CVgPathInfo::AppendPath(MVgContext& aVgContext, CVgPathInfo& aSrcPathInfo)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgAppendPath, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSrcPathInfo.HostHandle());
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgPathInfo::AppendPathData(MVgContext& aVgContext, VGint aNumSegments, const VGubyte * aPathSegments, const void* aPathData)
	{
	VGint pathDataSize = 0;
	if ( CheckAppendPathData(aVgContext, aNumSegments, aPathSegments, aPathData, &pathDataSize) )
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgAppendPathData, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(iHostHandle);
		vgApiData.AppendParam(aNumSegments);
		vgApiData.AppendVector(aPathSegments, aNumSegments);
		vgApiData.AppendVector(aPathData, pathDataSize);
		aVgContext.ExecuteVgCommand(vgApiData);
		}
	}


//Private extension to Open VG API to get the path length
VGint CVgPathInfo::HostVgeGetPathCoordsSizeInBytes(MVgContext& aVgContext, VGint aStartIndex, VGint aNumSegments)
	{
	RemoteFunctionCallData rfcdata; OpenVgRFC vgApiData(rfcdata);
	vgApiData.Init(OpenVgRFC::EvgePathCoordsSizeInBytes);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aStartIndex);
	vgApiData.AppendParam(aNumSegments);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return static_cast<VGint>(vgApiData.ReturnValue());
	}


void CVgPathInfo::ModifyPathCoords(MVgContext& aVgContext, VGint aStartIndex, VGint aNumSegments, const void* aPathData)
	{
	if (CheckPathDataAlignment(aVgContext, aPathData))
		{
		// The host OpenVG implementation returns -1 if vgePathCoordsSizeInBytes is not supported
		// It does not seem simple to reliably implement vgePathCoordsSizeInBytes in the Symbian client
		VGint dataSize = HostVgeGetPathCoordsSizeInBytes(aVgContext, aStartIndex, aNumSegments);
		OPENVG_TRACE("TGuestOpenVg::vgModifyPathCoords - dataSize=%d", dataSize);
		if (dataSize <= 0)
			{
			aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			}
		else
			{
			RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
			vgApiData.Init(OpenVgRFC::EvgModifyPathCoords, RemoteFunctionCallData::EOpRequest);
			vgApiData.AppendParam(iHostHandle);
			vgApiData.AppendParam(aStartIndex);
			vgApiData.AppendParam(aNumSegments);
			vgApiData.AppendVector(aPathData, dataSize);
			aVgContext.ExecuteVgCommand(vgApiData);
			}
		}
	}


void CVgPathInfo::TransformPath(MVgContext& aVgContext, CVgPathInfo& aSrcPathInfo)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgTransformPath, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSrcPathInfo.HostHandle());
	aVgContext.ExecuteVgCommand(vgApiData);
	}


VGboolean CVgPathInfo::InterpolatePath(MVgContext& aVgContext, CVgPathInfo& aStartPathInfo, CVgPathInfo& aEndPathInfo, VGfloat aAmount)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgInterpolatePath);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aStartPathInfo.HostHandle());
	vgApiData.AppendParam(aEndPathInfo.HostHandle());
	vgApiData.AppendParam(aAmount);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return static_cast<VGboolean>(vgApiData.ReturnValue());
	}


VGfloat CVgPathInfo::PathLength(MVgContext& aVgContext, VGint aStartSegment, VGint aNumSegments)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgPathLength);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aStartSegment);
	vgApiData.AppendParam(aNumSegments);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return static_cast<VGfloat>(vgApiData.ReturnValue());
	}


void CVgPathInfo::PointAlongPath(MVgContext& aVgContext, VGint aStartSegment, VGint aNumSegments, VGfloat aDistance,
		VGfloat* aX, VGfloat* aY, VGfloat* aTangentX, VGfloat* aTangentY)
	{
	TBool getPoint = (aX && aY);
	TBool getTangent = (aTangentX && aTangentY);

	if ( getPoint && !TCheck::Chk2x32bitPtr(aVgContext, aX, aY) )
		{
		return;
		}
	if ( getTangent && !TCheck::Chk2x32bitPtr(aVgContext, aTangentX, aTangentY) )
		{
		return;
		}
	if (getPoint || getTangent)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgPointAlongPath);
		vgApiData.AppendParam(iHostHandle);
		vgApiData.AppendParam(aStartSegment);
		vgApiData.AppendParam(aNumSegments);
		vgApiData.AppendParam(aDistance);
		vgApiData.AppendVector(aX, getPoint ? 1 : 0, RemoteFunctionCallData::EOut);
		vgApiData.AppendVector(aY, getPoint ? 1 : 0, RemoteFunctionCallData::EOut);
		vgApiData.AppendVector(aTangentX, getTangent ? 1 : 0, RemoteFunctionCallData::EOut);
		vgApiData.AppendVector(aTangentY, getTangent ? 1 : 0, RemoteFunctionCallData::EOut);
		aVgContext.ExecuteVgCommand(vgApiData);
		}
	}


void CVgPathInfo::PathBounds(MVgContext& aVgContext, VGfloat* aMinX, VGfloat* aMinY, VGfloat* aWidth, VGfloat* aHeight)
	{
	if (TCheck::Chk4x32bitPtr(aVgContext, aMinX, aMinY, aWidth, aHeight) )
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgPathBounds);
		vgApiData.AppendParam(iHostHandle);
		vgApiData.AppendVector(aMinX, 1, RemoteFunctionCallData::EOut);
		vgApiData.AppendVector(aMinY, 1, RemoteFunctionCallData::EOut);
		vgApiData.AppendVector(aWidth, 1, RemoteFunctionCallData::EOut);
		vgApiData.AppendVector(aHeight, 1, RemoteFunctionCallData::EOut);
		aVgContext.ExecuteVgCommand(vgApiData);
		}
	}


void CVgPathInfo::PathTransformedBounds(MVgContext& aVgContext, VGfloat* aMinX, VGfloat* aMinY, VGfloat* aWidth, VGfloat* aHeight)
	{
	if (TCheck::Chk4x32bitPtr(aVgContext, aMinX, aMinY, aWidth, aHeight) )
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgPathTransformedBounds);
		vgApiData.AppendParam(iHostHandle);
		vgApiData.AppendVector(aMinX, 1, RemoteFunctionCallData::EOut);
		vgApiData.AppendVector(aMinY, 1, RemoteFunctionCallData::EOut);
		vgApiData.AppendVector(aWidth, 1, RemoteFunctionCallData::EOut);
		vgApiData.AppendVector(aHeight, 1, RemoteFunctionCallData::EOut);
		aVgContext.ExecuteVgCommand(vgApiData);
		}
	}


void CVgPathInfo::DrawPath(MVgContext& aVgContext, VGbitfield aPaintModes)
	{
	if ( TCheck::ChkVGPaintModesCombination(aVgContext, aPaintModes) )
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgDrawPath, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(iHostHandle);
		vgApiData.AppendParam(aPaintModes);
		aVgContext.ExecuteVgCommand(vgApiData);
		}
	}


void CVgPathInfo::RenderToMask(MVgContext& aVgContext, VGbitfield aPaintModes, VGMaskOperation aOperation)
	{
	if ( TCheck::ChkVGPaintModesCombination(aVgContext, aPaintModes) && TCheck::ChkVGMaskOperation(aVgContext, aOperation) )
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgRenderToMask, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(iHostHandle);
		vgApiData.AppendParam(aPaintModes);
		vgApiData.AppendParam(aOperation);
		aVgContext.ExecuteVgCommand(vgApiData);
		}
	}


// end of file vgpath.cpp
