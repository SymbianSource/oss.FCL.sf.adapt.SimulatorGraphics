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
// C++ version of Open VG API.

#include "openvg.inl"
#include "remotefunctioncall.h"
#include "openvgrfc.h"


// normalized IEEE 754 representations
const float KFloatMinusOne = -1.0f;
const float KFloatZero = 0.0f;

const TInt KTransformMatrixSize = 9;

/////////////////////////////////////////////////////////////////////////////////////////////
// TGuestOpenVg
/////////////////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------
// TGuestOpenVg - private inline & paramater checking functions

// helper function
///////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of Open VG APIs ... with comments summarising possible errors.

////////////////////////////////////////////////////////////////////////////////////////////
//Functions returning value
////////////////////////////////////////////////////////////////////////////////////////////

/*
 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paintMode is not a valid value from the VGPaintMode enumeration
 */
VGPaint TGuestOpenVg::vgGetPaint(VGPaintMode aPaintMode)
	{
	VGPaint paintHandle = VG_INVALID_HANDLE;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext && TCheck::ChkVGPaintMode(*vgContext, aPaintMode))
		{
		TCleanupVgLocks vgLock(*vgContext);
		paintHandle = vgLock.GetPaint(aPaintMode);
		}
	else if (!vgContext)
		{
		OPENVG_TRACE("  TGuestOpenVg::vgGetPaint - no VG context");
		}
	else
		{
		OPENVG_TRACE("  TGuestOpenVg::vgGetPaint - ParamCheck failed");
		}
	return paintHandle;
	}


/*
 If insufficient memory is available to allocate a new object, VG_INVALID_HANDLE is returned.
 */
VGPaint TGuestOpenVg::vgCreatePaint()
	{
	VGPaint paintHandle = VG_INVALID_HANDLE;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		TCleanupVgLocks vgLock(*vgContext);
		paintHandle = vgLock.CreatePaint();
		}
	else
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreatePaint - no VG context");
		}

	return paintHandle;
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if any of dstPath, startPath, or endPath is not a valid path handle, or is
     not shared with the current context
   VG_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_INTERPOLATE_TO is not enabled for dstPath
   – if VG_PATH_CAPABILITY_INTERPOLATE_FROM is not enabled for startPath or endPath
 */
VGboolean TGuestOpenVg::vgInterpolatePath(VGPath aDstPath, VGPath aStartPath, VGPath aEndPath, VGfloat aAmount)
	{
	VGboolean result = VG_FALSE;
	MVgContext* vgContext = CVghwUtils::VgContext();

	if ( vgContext )
		{
		CVgPathInfo* dstPathInfo = NULL;
		CVgPathInfo* startPathInfo = NULL;
		CVgPathInfo* endPathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aDstPath, &dstPathInfo, VG_PATH_CAPABILITY_INTERPOLATE_TO) &&
				vgLock.CheckVGPath(aStartPath, &startPathInfo, VG_PATH_CAPABILITY_INTERPOLATE_FROM) &&
				vgLock.CheckVGPath(aEndPath, &endPathInfo, VG_PATH_CAPABILITY_INTERPOLATE_FROM) )
			{
			result = dstPathInfo->InterpolatePath(*vgContext, *startPathInfo, *endPathInfo, aAmount);
			}
		}
	return result;
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VG_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_PATH_LENGTH is not enabled for path
   VG_ILLEGAL_ARGUMENT_ERROR
   – if startSegment is less than 0 or greater than the index of the final path
     segment
   – if numSegments is less than or equal to 0
   – if (startSegment + numSegments – 1) is greater than the index of the final
     path segment
 */
VGfloat TGuestOpenVg::vgPathLength(VGPath aPath, VGint aStartSegment, VGint aNumSegments)
	{
	VGfloat result = KFloatMinusOne;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aPath, &pathInfo, VG_PATH_CAPABILITY_PATH_LENGTH) )
			{
			if ( (aStartSegment < 0) || (aNumSegments <= 0) )
				{
				vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
				}
			else
				{
				result = pathInfo->PathLength(*vgContext, aStartSegment, aNumSegments);
				}
			}
		}
	return result;
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
 */
VGbitfield TGuestOpenVg::vgGetPathCapabilities(VGPath aPath)
	{
	VGbitfield result = 0;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aPath, &pathInfo) )
			{
			result = pathInfo->GetPathCapabilities(*vgContext);
			}
		}
	return result;
	}


/*
 ERRORS
   VG_UNSUPPORTED_PATH_FORMAT_ERROR
   – if pathFormat is not a supported format
   VG_ILLEGAL_ARGUMENT_ERROR
   – if datatype is not a valid value from the VGPathDatatype enumeration
   – if scale is equal to 0
 */
VGPath TGuestOpenVg::vgCreatePath(VGint aPathFormat, VGPathDatatype aDatatype, VGfloat aScale, VGfloat aBias,
		VGint aSegmentCapacityHint, VGint aCoordCapacityHint, VGbitfield aCapabilities)
	{
	VGPath pathHandle = VG_INVALID_HANDLE;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::ChkVGPathFormat(*vgContext, aPathFormat) && TCheck::ChkVGPathDataType(*vgContext, aDatatype) )
		{
		if (aScale == KFloatZero)
			{
			vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			}
		else
			{
			TCleanupVgLocks vgLock(*vgContext); // Waits & holds VG mutex until destruction 
			pathHandle = vgLock.CreatePath(aPathFormat, aDatatype, aScale, aBias, aSegmentCapacityHint, aCoordCapacityHint, aCapabilities);
			}
		}
	else if (!vgContext)
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreatePath - no VG context");
		}
	else
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreatePath - ParamCheck failed");
		}

	return pathHandle;
	}


/* For vgGeti, vgGetf, and vgGetVectorSize.
If an error occurs during a call to vgGetf, vgGeti, or vgGetVectorSize, the return value
is undefined.

ERRORS
  VG_ILLEGAL_ARGUMENT_ERROR
  – if paramType is not a valid value from the VGParamType enumeration
  – if paramType refers to a vector parameter in vgGetf or vgGeti
*/
VGint TGuestOpenVg::HostGeti(MVgContext& aVgContext, VGParamType aType)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGeti);
	vgApiData.AppendParam(aType);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return static_cast<VGint>(vgApiData.ReturnValue());
	}


VGfloat TGuestOpenVg::HostGetf(MVgContext& aVgContext, VGParamType aType)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetf);
	vgApiData.AppendParam(aType);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return static_cast<VGfloat>(vgApiData.ReturnValue());
	}


VGint TGuestOpenVg::vgGeti(VGParamType aType)
	{
	// TODO validate aType
	VGint result = 0;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		result = HostGeti(*vgContext, aType);
		}
	return result;
	}


VGfloat TGuestOpenVg::vgGetf(VGParamType aType)
	{
	// TODO validate aType
	VGfloat result = KFloatMinusOne;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		result = HostGetf(*vgContext, aType);
		}
	return result;
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if object is not a valid handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paramType is not a valid value from the appropriate enumeration
   – if paramType refers to a vector parameter in vgGetParameterf or
     vgGetParameteri
 */
VGint TGuestOpenVg::vgGetParameteri(VGHandle aObject, VGint aParamType)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgHandleBase* handleInfo;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGAnyHandle(aObject, &handleInfo) )
			{
			return handleInfo->GetParameteri(*vgContext, aParamType);
			}
		}
	return 0;
	}


VGint TGuestOpenVg::HostGetVectorSize(MVgContext& aVgContext, VGParamType aType)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetVectorSize);
	vgApiData.AppendParam(aType);
	VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
	aVgContext.ExecuteVgCommand(vgApiData);
	return static_cast<VGint>(vgApiData.ReturnValue());
	}


VGint TGuestOpenVg::vgGetVectorSize(VGParamType aType)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{ // **** Desirable: check aType
		return HostGetVectorSize(*vgContext, aType);
		}
	return 0;
	}


VGfloat TGuestOpenVg::vgGetParameterf(VGHandle aObject, VGint aParamType)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgHandleBase* handleInfo;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGAnyHandle(aObject, &handleInfo) )
			{
			return handleInfo->GetParameterf(*vgContext, aParamType);
			}
		}
	return KFloatMinusOne;
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if object is not a valid handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paramType is not a valid value from the appropriate enumeration
 */
VGint TGuestOpenVg::vgGetParameterVectorSize(VGHandle aObject, VGint aParamType)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgHandleBase* handleInfo;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		// **** Desirable: check aParamType
		if ( vgLock.CheckVGAnyHandle(aObject, &handleInfo) )
			{
			return handleInfo->GetParameterVectorSize(*vgContext, aParamType);
			}
		}
	return 0;
 	}


/*
  ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height are less than or equal to 0
   – if width is greater than VG_MAX_IMAGE_WIDTH
   – if height is greater than VG_MAX_IMAGE_HEIGHT
   – if width*height is greater than VG_MAX_IMAGE_PIXELS
 */
VGMaskLayer TGuestOpenVg::vgCreateMaskLayer(VGint aWidth, VGint aHeight)
	{
	VGMaskLayer maskLayerHandle = VG_INVALID_HANDLE;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::ChkNewImageSize(*vgContext, aWidth, aHeight) )
		{
		TCleanupVgLocks vgLock(*vgContext);
		maskLayerHandle = vgLock.CreateMaskLayer(aWidth, aHeight);
		}
	else if (!vgContext)
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreateMaskLayer - no VG context");
		}
	else
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreateMaskLayer - ParamCheck failed");
		}

	return maskLayerHandle;
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if paint is not a valid paint handle, or is not shared with the current context
 */
VGuint TGuestOpenVg::vgGetColor(VGPaint aPaint)
	{
	VGuint result = 0;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPaintInfo* paintInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPaint(aPaint, &paintInfo) )
			{
			result = paintInfo->GetColor(*vgContext);
			}
		}
	return result;
	}


/*
 ERRORS
   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR
   – if format is not a valid value from the VGImageFormat enumeration
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height are less than or equal to 0
   – if width is greater than VG_MAX_IMAGE_WIDTH
   – if height is greater than VG_MAX_IMAGE_HEIGHT
   – if width*height is greater than VG_MAX_IMAGE_PIXELS
   – if width*height*(pixel size of format) is greater than
   VG_MAX_IMAGE_BYTES
   – if allowedQuality is not a bitwise OR of values from the
     VGImageQuality enumeration
 */
VGImage TGuestOpenVg::vgCreateImage(VGImageFormat aFormat, VGint aWidth, VGint aHeight, VGbitfield aAllowedQuality)
	{
	VGHandle imageHandle = VG_INVALID_HANDLE;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::ChkNewImageSize(*vgContext, aWidth, aHeight) && TCheck::ChkVgImageFormat(*vgContext, aFormat) )
		{ // width > 0 && height > 0
		TCleanupVgLocks vgLock(*vgContext);
		imageHandle = vgLock.CreateImage(aFormat, aWidth, aHeight, aAllowedQuality);
		}
	else if (!vgContext)
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreateImage - no VG context");
		}
	else
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreateImage - ParamCheck failed");
		}

	return imageHandle;
	}


/*
 Returns a new VGImage handle that refers to a portion of the parent image. The
 region is given by the intersection of the bounds of the parent image with the
 rectangle beginning at pixel (x, y) with dimensions width and height, which
 must define a positive region contained entirely within parent.

 ERRORS
   VG_BAD_HANDLE_ERROR
   – if parent is not a valid image handle, or is not shared with the current
     context
   VG_IMAGE_IN_USE_ERROR
   – if parent is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if x is less than 0 or greater than or equal to the parent width
   – if y is less than 0 or greater than or equal to the parent height
   – if width or height is less than or equal to 0
   – if x + width is greater than the parent width
   – if y + height is greater than the parent height
 */
VGImage TGuestOpenVg::vgChildImage(VGImage aParent, VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	VGHandle imageHandle = VG_INVALID_HANDLE;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* parentInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGImage(aParent, &parentInfo) && TCheck::ChkAreaIsWithinImage(*vgContext, parentInfo, aX, aY, aWidth, aHeight) )
			{ // child image area is within parent image
			imageHandle = vgLock.ChildImage(*parentInfo, aX, aY, aWidth, aHeight);
			}
		}
	else
		{
		OPENVG_TRACE("  TGuestOpenVg::vgChildImage - no VG context");
		}

	return imageHandle;
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if image is not a valid image handle, or is not shared with the current
     context
   VG_IMAGE_IN_USE_ERROR
   – if image is currently a rendering target
 */
VGImage TGuestOpenVg::vgGetParent(VGImage aImage)
	{
	VGImage parentImage = VG_INVALID_HANDLE;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGImage(aImage, &imageInfo) )
			{
			// baseline for a valid image is to return its own ClientHandle
			parentImage = aImage;
			// try to find a non-destroyed ancestor
			// ToDo add getParent Api to CVgImageInfo
			CVgImageInfo* parentInfo = imageInfo->Parent();
			while (parentInfo)
				{
				if ( !parentInfo->IsDestroyed() )
					{ // nearest an ancestor
					parentImage = parentInfo->ClientHandle();
					break;
					}
				parentInfo = parentInfo->Parent();
				}
			}
		}
	return parentImage;
	}


/*
 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if glyphCapacityHint is negative
 */
VGFont TGuestOpenVg::vgCreateFont(VGint aGlyphCapacityHint)
	{
	VGHandle fontHandle = VG_INVALID_HANDLE;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		TCleanupVgLocks vgLock(*vgContext);
		fontHandle = vgLock.CreateFont(aGlyphCapacityHint);
		}
	else
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreateFont - no VG context");
		}

	return fontHandle;
	}


/* Hardware Queries */

/*
 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if key is not one of the values from the VGHardwareQueryType enumeration
   – if setting is not one of the values from the enumeration associated with
     key
 */
VGHardwareQueryResult TGuestOpenVg::vgHardwareQuery(VGHardwareQueryType aKey, VGint aSetting)
	{
	TBool paramsValid = EFalse;

	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		switch (aKey)
			{
			case VG_IMAGE_FORMAT_QUERY:
				if (TCheck::ImageFormatByteDepth(static_cast<VGImageFormat>(aSetting)) > 0)
					paramsValid = ETrue;
				break;
			case VG_PATH_DATATYPE_QUERY:
				switch (aSetting)
					{
					case VG_PATH_DATATYPE_S_8:
					case VG_PATH_DATATYPE_S_16:
					case VG_PATH_DATATYPE_S_32:
					case VG_PATH_DATATYPE_F:
						paramsValid = ETrue;
					}
				break;
			}
		if (paramsValid)
			{
			RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
			vgApiData.Init(OpenVgRFC::EvgHardwareQuery);
			vgApiData.AppendParam(aKey);
			vgApiData.AppendParam(aSetting);
			VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
			vgContext->ExecuteVgCommand(vgApiData);
			return static_cast<VGHardwareQueryResult>(vgApiData.ReturnValue());
			}
		vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return VG_HARDWARE_UNACCELERATED;
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if object is not a valid handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paramType is not a valid value from the appropriate enumeration
   – if values is NULL in vgGetParameterfv or vgGetParameteriv
   – if values is not properly aligned in vgGetParameterfv or vgGetParameteriv
   – if count is less than or equal to 0 in vgGetParameterfv or vgGetParameteriv
   – if count is greater than the value returned by vgGetParameterVectorSize for
     the given parameter in vgGetParameterfv or vgGetParameteriv
 */
void TGuestOpenVg::vgGetParameterfv(VGHandle aObject, VGint aParamType, VGint aCount, VGfloat * aValues)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgHandleBase* handleInfo;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGAnyHandle(aObject, &handleInfo) && TCheck::Chk32bitPtr(*vgContext, aValues) )
			{
			handleInfo->GetParameterfv(*vgContext, aParamType, aCount, aValues);
			}
		}
	}


void TGuestOpenVg::vgGetParameteriv(VGHandle aObject, VGint aParamType, VGint aCount, VGint * aValues)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgHandleBase* handleInfo;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGAnyHandle(aObject, &handleInfo) && TCheck::Chk32bitPtr(*vgContext, aValues) )
			{
			handleInfo->GetParameteriv(*vgContext, aParamType, aCount, aValues);
			}
		}
	}


/* Renderer and Extension Information */

/* Getxv supports vgGetfv & vgGetiv APIs.

 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paramType is not a valid value from the VGParamType enumeration
   – if values is NULL in vgGetfv or vgGetiv
   – if values is not properly aligned in vgGetfv or vgGetiv
   – if count is less than or equal to 0 in vgGetfv or vgGetiv
   – if count is greater than the value returned by vgGetVectorSize for the
     given parameter in vgGetfv or vgGetiv
 */
// **** Desirable: can probably do common VGParamType checking for Getx & Getxv, Setx, & Setxv.
void TGuestOpenVg::vgGetfv(VGParamType aType, VGint aCount, const VGfloat * aValues)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext && TCheck::Chk32bitPtr(*vgContext, aValues))
		{
		// **** Desirable: check all parameters
		if ( (aCount <= 0) || (aCount > HostGetVectorSize(*vgContext, aType)) )
			{
			vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			}
		else
			{
			RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
			vgApiData.Init(OpenVgRFC::EvgGetfv);
			vgApiData.AppendParam(aType);
			vgApiData.AppendParam(aCount);
			vgApiData.AppendVector(aValues, aCount, RemoteFunctionCallData::EOut);
			vgContext->ExecuteVgCommand(vgApiData);
			}
		}
	}


void TGuestOpenVg::vgGetiv(VGParamType aType, VGint aCount, const VGint * aValues)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::Chk32bitPtr(*vgContext, aValues) )
		{
		// **** Desirable: check all parameters
		if ( (aCount <= 0) || (aCount > HostGetVectorSize(*vgContext, aType)) )
			{
			vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			}
		else
			{
			RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
			vgApiData.Init(OpenVgRFC::EvgGetiv);
			vgApiData.AppendParam(aType);
			vgApiData.AppendParam(aCount);
			vgApiData.AppendVector(aValues, aCount, RemoteFunctionCallData::EOut);
			vgContext->ExecuteVgCommand(vgApiData);
			}
		}
	}


/*
 ERRORS
 VG_ILLEGAL_ARGUMENT_ERROR
 – if m is NULL
 – if m is not properly aligned
 */
void TGuestOpenVg::vgGetMatrix(VGfloat* aM)
	{
	// check mat pointer is 32-bit aligned
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::Chk32bitPtr(*vgContext, aM) )
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgGetMatrix);
		vgApiData.AppendVector(aM, KTransformMatrixSize, RemoteFunctionCallData::EOut);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if image is not a valid image handle, or is not shared with the current context
   VG_IMAGE_IN_USE_ERROR
   – if image is currently a rendering target
   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR
   – if dataFormat is not a valid value from the VGImageFormat enumeration
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
   – if data is NULL
   – if data is not properly aligned
 */
void TGuestOpenVg::vgGetImageSubData(VGImage aImage, void* aData, VGint aDataStride, VGImageFormat aDataFormat,
		VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGImage(aImage, &imageInfo) && TCheck::ChkImageAlignment(*vgContext, aDataFormat, aData) &&
				TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
			{
			imageInfo->GetImageSubData(*vgContext, aData, aDataStride, aDataFormat, aX, aY, aWidth, aHeight);
			}
		}
	}


/*
 ERRORS
   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR
   – if dataFormat is not a valid value from the VGImageFormat enumeration
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
   – if data is NULL
   – if data is not properly aligned
 */
void TGuestOpenVg::HostVgReadPixels(MVgContext& aVgContext, void* aPixmap, size_t aPixmapSize, VGint aHostDataStride,
		VGImageFormat aDataFormat, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight)
	{
	VGPANIC_ASSERT(aPixmap != NULL, EVgPanicNullPixmapPointer);
	VGPANIC_ASSERT(aPixmapSize >= (aHostDataStride * aHeight), EVgPanicPixmapSizeError);
	RemoteFunctionCallData rfcdata; OpenVgRFC vgApiData(rfcdata);
	vgApiData.Init(OpenVgRFC::EvgReadPixels);
	vgApiData.AppendVector(aPixmap, aPixmapSize, RemoteFunctionCallData::EOut);
	vgApiData.AppendParam(aHostDataStride);
	vgApiData.AppendParam(aDataFormat);
	vgApiData.AppendParam(aSx);
	vgApiData.AppendParam(aSy);
	vgApiData.AppendParam(aWidth);
	vgApiData.AppendParam(aHeight);
	vgApiData.AppendParam((VGint)aPixmapSize);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void TGuestOpenVg::vgReadPixels(void* aData, VGint aDataStride, VGImageFormat aDataFormat, VGint aSx, VGint aSy,
		VGint aWidth, VGint aHeight)
	{
	// **** Desirable: check all parameters
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::ChkImageAlignment(*vgContext, aDataFormat, aData) && TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
		{ // width > 0 && height > 0
		TInt bitsPerPixel = CVgImageInfo::BitsPerPixelForVgImageFormat(aDataFormat);
		TUint32 lineLength = static_cast<TUint32>(aWidth) * static_cast<TUint32>(bitsPerPixel);
		TUint32 tailBits = lineLength & 7;
		lineLength = (lineLength + 7) / 8;
		OPENVG_TRACE("vgReadPixels.1 bitsPerPixel=%d, lineLength=%d, tailBits=%d", bitsPerPixel, lineLength, tailBits);
		// ToDo clip aWidth & aHeight to the surface width & height

		if (bitsPerPixel <= 0)
			{
			vgContext->SetVgError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
			}
		else
			{
			if (lineLength  == aDataStride)
				{ // use original params
				OPENVG_TRACE("vgReadPixels.2a: lineLength == dataStride");
				HostVgReadPixels(*vgContext, aData, aDataStride * aHeight, aDataStride, aDataFormat, aSx, aSy, aWidth, aHeight);
				}
			else if (0 == aDataStride)
				{ // unlikely unless aHeight = 1, but symmetric to fill function for vgWritePixels
				OPENVG_TRACE("vgReadPixels.2b: 0 == dataStride");
				HostVgReadPixels(*vgContext, aData, lineLength, aDataStride, aDataFormat, aSx, aSy, aWidth, aHeight);
				}
			else
				{ // datastride maybe negative or simply > lineLength
				TInt pixmapSize = lineLength * aHeight;
				TUint8* localBuffer = (TUint8*) CVghwUtils::Alloc(pixmapSize);
				OPENVG_TRACE("vgReadPixels.2c: dataStride not 0 or lineLength, localBuffer=0x%x", localBuffer);

				if (localBuffer != NULL)
					{ // read pixels into temporary buffer
					HostVgReadPixels(*vgContext, localBuffer, pixmapSize, lineLength, aDataFormat, aSx, aSy, aWidth, aHeight);
					// reformat into client memory
					CVgImageInfo::PixmapBlit(static_cast<TUint8*>(aData), localBuffer, aDataStride, lineLength, aHeight, lineLength, tailBits);
					CVghwUtils::Free(localBuffer);
					}
				else
					{ // alloc failed, so do VG operation row by row
					TUint8* dest = static_cast<TUint8*>(aData);
					for (VGint row = 0; row < aHeight; ++row)
						{
						HostVgReadPixels(*vgContext, dest, lineLength, lineLength, aDataFormat, aSx + row, aSy, aWidth, 1);
						dest += aDataStride;
						}
					}
				}
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if minX, minY, width, or height is NULL
   – if minX, minY, width, or height is not properly aligned
   VG_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_PATH_BOUNDS is not enabled for path
 */
void TGuestOpenVg::vgPathBounds(VGPath aPath, VGfloat* aMinX, VGfloat* aMinY, VGfloat* aWidth, VGfloat* aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aPath, &pathInfo, VG_PATH_CAPABILITY_PATH_BOUNDS) )
			{
			pathInfo->PathBounds(*vgContext, aMinX, aMinY, aWidth, aHeight);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if minX, minY, width, or height is NULL
   – if minX, minY, width, or height is not properly aligned
   VG_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_PATH_TRANSFORMED_BOUNDS is not enabled
     for path
 */
void  TGuestOpenVg::vgPathTransformedBounds(VGPath aPath, VGfloat* aMinX, VGfloat* aMinY, VGfloat* aWidth, VGfloat* aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aPath, &pathInfo, VG_PATH_CAPABILITY_PATH_TRANSFORMED_BOUNDS) )
			{
			pathInfo->PathTransformedBounds(*vgContext, aMinX, aMinY, aWidth, aHeight);
			}
		}
	}


////////////////////////////////////////////////////////////////////////////////////////////
//Functions not returning value (possible to buffer)
////////////////////////////////////////////////////////////////////////////////////////////
/* Getters and Setters */

/*
 vgSeti and vgSetf

 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paramType is not a valid value from the VGParamType enumeration
   – if paramType refers to a vector parameter in vgSetf or vgSeti
   – if value is not a legal enumerated value for the given parameter in vgSetf or
     vgSeti
 */
void TGuestOpenVg::vgSeti(VGParamType aType, VGint aValue)
	{
	// **** Desirable: check aType
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgSeti, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(aType);
		vgApiData.AppendParam(aValue);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


void TGuestOpenVg::vgSetf(VGParamType aType, VGfloat aValue)
	{
	// **** Desirable: check aType
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgSetf, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(aType);
		vgApiData.AppendParam(aValue);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 vgSetiv and vgSetfv

 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paramType is not a valid value from the VGParamType enumeration
   – if paramType refers to a scalar parameter in vgSetfv or vgSetiv and count is
     not equal to 1
   – if values[i] is not a legal enumerated value for the given parameter in vgSetfv
     or vgSetiv for 0 <= i < count
   – if values is NULL in vgSetfv or vgSetiv and count is greater than 0
   – if values is not properly aligned in vgSetfv or vgSetiv
   – if count is less than 0 in vgSetfv or vgSetiv
   – if count is not a valid value for the given parameter
 */
void TGuestOpenVg::vgSetfv(VGParamType aType, VGint aCount, const VGfloat * aValues)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext && TCheck::Chk32bitPtr(*vgContext, aValues))
		{
		// **** Desirable: check all params
		// we should check count for not being too large for serialization, but
		// there is no limit in spec for VG_SCISSOR_RECTS and VG_STROKE_DASH_PATTERN
		if ( (aCount < 0) || (aCount > 100000) )
			{
			vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			}
		else
			{
			RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
			vgApiData.Init(OpenVgRFC::EvgSetfv, RemoteFunctionCallData::EOpRequest);
			vgApiData.AppendParam(aType);
			vgApiData.AppendParam(aCount);
			vgApiData.AppendVector(aValues, aCount);
			vgContext->ExecuteVgCommand(vgApiData);
			}
		}
	}


void TGuestOpenVg::vgSetiv(VGParamType aType, VGint aCount, const VGint * aValues)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::Chk32bitPtr(*vgContext, aValues) )
		{
		// **** Desirable: check all params
		// we should check count for not being too large for serialization, but
		// there is no limit in spec for VG_SCISSOR_RECTS and VG_STROKE_DASH_PATTERN
		if ( (aCount < 0) || (aCount > 100000) )
			{
			vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			}
		else
			{
			RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
			vgApiData.Init(OpenVgRFC::EvgSetiv, RemoteFunctionCallData::EOpRequest);
			vgApiData.AppendParam(aType);
			vgApiData.AppendParam(aCount);
			vgApiData.AppendVector(aValues, aCount);
			vgContext->ExecuteVgCommand(vgApiData);
			}
		}
	}


/*
 For vgSetParameterf and vgSetParameteri.

 ERRORS
   VG_BAD_HANDLE_ERROR
   – if object is not a valid handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paramType is not a valid value from the appropriate enumeration
   – if paramType refers to a vector parameter in vgSetParameterf or
     vgSetParameteri
   – if value is not a legal enumerated value for the given parameter in
     vgSetParameterf or vgSetParameteri
 */

void TGuestOpenVg::vgSetParameteri(VGHandle aObject, VGint aParamType, VGint aValue)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgHandleBase* handleInfo;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGAnyHandle(aObject, &handleInfo) )
			{
			handleInfo->SetParameteri(*vgContext, aParamType, aValue);
			}
		}
	}


void TGuestOpenVg::vgSetParameterf(VGHandle aObject, VGint aParamType, VGfloat aValue)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgHandleBase* handleInfo;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGAnyHandle(aObject, &handleInfo) )
			{
			handleInfo->SetParameterf(*vgContext, aParamType, aValue);
			}
		}
	}


/*
 For vgSetParameterfv and vgSetParameteriv.

 The vgSetParameter functions set the value of a parameter on a given VGHandlebased
 object.

 ERRORS
   VG_BAD_HANDLE_ERROR
   – if object is not a valid handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paramType is not a valid value from the appropriate enumeration
   – if paramType refers to a scalar parameter in vgSetParameterfv or
     vgSetParameteriv and count is not equal to 1
   – if values[i] is not a legal enumerated value for the given parameter
     in vgSetParameterfv or vgSetParameteriv for 0 <= i < count
   – if values is NULL in vgSetParameterfv or vgSetParameteriv and count is
     greater than 0
   – if values is not properly aligned in vgSetParameterfv or vgSetParameteriv
   – if count is less than 0 in vgSetParameterfv or vgSetParameteriv
   – if count is not a valid value for the given parameter
 */
void TGuestOpenVg::vgSetParameterfv(VGHandle aObject, VGint aParamType, VGint aCount, const VGfloat * aValues)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgHandleBase* handleInfo;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		// ToDo review overlapping checks between TCheck::ParamCountAndValuesPtr to SetParameterfv
		if ( vgLock.CheckVGAnyHandle(aObject, &handleInfo) && TCheck::ChkParamCountAndValuesPtr(*vgContext, aCount, aValues) )
			{
			handleInfo->SetParameterfv(*vgContext, aParamType, aCount, aValues);
			}
		}
	}


void TGuestOpenVg::vgSetParameteriv(VGHandle aObject, VGint aParamType, VGint aCount, const VGint * aValues)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgHandleBase* handleInfo;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGAnyHandle(aObject, &handleInfo) && TCheck::ChkParamCountAndValuesPtr(*vgContext, aCount, aValues) )
			{
			handleInfo->SetParameteriv(*vgContext, aParamType, aCount, aValues);
			}
		}
	}


/* Matrix Manipulation */
/*
 No errors if Context is valid.
 */
void TGuestOpenVg::vgLoadIdentity()
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgLoadIdentity, RemoteFunctionCallData::EOpRequest);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if m is NULL
   – if m is not properly aligned
 */
void TGuestOpenVg::vgLoadMatrix(const VGfloat* aM)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::Chk32bitPtr(*vgContext, aM) )
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgLoadMatrix, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendVector(aM, KTransformMatrixSize);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if m is NULL
   – if m is not properly aligned
 */
void TGuestOpenVg::vgMultMatrix(const VGfloat* aM)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::Chk32bitPtr(*vgContext, aM) )
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgMultMatrix, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendVector(aM, KTransformMatrixSize);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 No errors if Context is valid.
 */
void TGuestOpenVg::vgTranslate(VGfloat aTx, VGfloat aTy)
	{
	// **** Desirable: check all parameters
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgTranslate, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(aTx);
		vgApiData.AppendParam(aTy);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 No errors if Context is valid.
 */
// **** Desirable: Coalesce with vgTranslate
void TGuestOpenVg::vgScale(VGfloat aSx, VGfloat aSy)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgScale, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(aSx);
		vgApiData.AppendParam(aSy);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 No errors if Context is valid.
 */
// **** Desirable: Coalesce with vgTranslate
void TGuestOpenVg::vgShear(VGfloat aShx, VGfloat aShy)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgShear, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(aShx);
		vgApiData.AppendParam(aShy);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 No errors if Context is valid.
 */
void TGuestOpenVg::vgRotate(VGfloat aAngle)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgRotate, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(aAngle);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if operation is not VG_CLEAR_MASK or VG_FILL_MASK, and mask is not a
     valid mask layer or image handle, or is not shared with the current context
   VG_IMAGE_IN_USE_ERROR
   – if mask is a VGImage that is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if operation is not a valid value from the VGMaskOperation
     enumeration
   – if width or height is less than or equal to 0
   – if mask is a VGMaskLayer and is not compatible with the current surface
     mask
 */
void TGuestOpenVg::vgMask(VGHandle aMask, VGMaskOperation aOperation, VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageBase* maskInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		// **** Desirable: mask/surface compatibility check
		if ( vgLock.CheckVGMaskOperationAndHandle(aOperation, aMask, &maskInfo) && TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
			{
			RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
			vgApiData.Init(OpenVgRFC::EvgMask, RemoteFunctionCallData::EOpRequest);
			vgApiData.AppendParam( (maskInfo == NULL) ? VG_INVALID_HANDLE : maskInfo->HostHandle() );
			vgApiData.AppendParam(aOperation);
			vgApiData.AppendParam(aX);
			vgApiData.AppendParam(aY);
			vgApiData.AppendParam(aWidth);
			vgApiData.AppendParam(aHeight);
			TUint64 sgId(0L);
			if ( maskInfo && (maskInfo->HandleType() == EVgHandleForImage) )
				{
				CVgImageInfo* imageInfo = (CVgImageInfo*)maskInfo;
				if (imageInfo->IsEglSibling())
					{
					sgId = imageInfo->SgImageId();
					}
				}
			vgApiData.AppendTUint64(sgId);
			vgContext->ExecuteVgCommand(vgApiData);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paintModes is not a valid bitwise OR of values from the VGPaintMode
     enumeration
   – if operation is not a valid value from the VGMaskOperation enumeration
 */
void TGuestOpenVg::vgRenderToMask(VGPath aPath, VGbitfield aPaintModes, VGMaskOperation aOperation)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aPath, &pathInfo) )
			{
			pathInfo->RenderToMask(*vgContext, aPaintModes, aOperation);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if maskLayer is not a valid mask handle
 */
void TGuestOpenVg::vgDestroyMaskLayer(VGMaskLayer aMaskLayer)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		CVgMaskLayerInfo* maskLayerInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGMaskLayer(aMaskLayer, &maskLayerInfo) )
			{
			maskLayerInfo->Destroy(*vgContext);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if maskLayer is not a valid mask layer handle, or is not shared with the
     current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if value is less than 0 or greater than 1
   – if width or height is less than or equal to 0
   – if x or y is less than 0
   – if x + width is greater than the width of the mask
   – if y + height is greater than the height of the mask
 */
void TGuestOpenVg::vgFillMaskLayer(VGMaskLayer aMaskLayer, VGint aX, VGint aY, VGint aWidth, VGint aHeight, VGfloat aValue)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgMaskLayerInfo* maskLayerInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGMaskLayer(aMaskLayer, &maskLayerInfo) && TCheck::ChkAreaIsWithinImage(*vgContext, maskLayerInfo, aX, aY, aWidth, aHeight) )
			{
			maskLayerInfo->FillMaskLayer(*vgContext, aX, aY, aWidth, aHeight, aValue);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if maskLayer is not a valid mask layer handle
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height are less than or equal to 0
   – if maskLayer is not compatible with the current surface mask
 */
void TGuestOpenVg::vgCopyMask(VGMaskLayer aMaskLayer, VGint aSx, VGint aSy, VGint aDx, VGint aDy, VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgMaskLayerInfo* maskLayerInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGMaskLayer(aMaskLayer, &maskLayerInfo) && TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
			{
			maskLayerInfo->CopyMask(*vgContext, aSx, aSy, aDx, aDy, aWidth, aHeight);
			}
		}
	}


/*
 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
 */
void TGuestOpenVg::vgClear(VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
		{ // width > 0 && height > 0
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgClear, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(aX);
		vgApiData.AppendParam(aY);
		vgApiData.AppendParam(aWidth);
		vgApiData.AppendParam(aHeight);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/* Paths */

/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
 */
void TGuestOpenVg::vgClearPath(VGPath aPath, VGbitfield aCapabilities)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aPath, &pathInfo) )
			{
			pathInfo->ClearPath(*vgContext, aCapabilities);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
 */
void TGuestOpenVg::vgDestroyPath(VGPath aPath)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aPath, &pathInfo) )
			{
			pathInfo->Destroy(*vgContext);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
 */
void TGuestOpenVg::vgRemovePathCapabilities(VGPath aPath, VGbitfield aCapabilities)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aPath, &pathInfo) )
			{
			pathInfo->RemovePathCapabilities(*vgContext, aCapabilities);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if either dstPath or srcPath is not a valid path handle, or is not shared
     with the current context
   VG_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_APPEND_FROM is not enabled for srcPath
   – if VG_PATH_CAPABILITY_APPEND_TO is not enabled for dstPath
 */
void TGuestOpenVg::vgAppendPath(VGPath aDstPath, VGPath aSrcPath)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* dstPathInfo = NULL;
		CVgPathInfo* srcPathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aDstPath, &dstPathInfo, VG_PATH_CAPABILITY_APPEND_TO) &&
				vgLock.CheckVGPath(aSrcPath, &srcPathInfo, VG_PATH_CAPABILITY_APPEND_FROM) )
			{
			dstPathInfo->AppendPath(*vgContext, *srcPathInfo);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if dstPath is not a valid path handle, or is not shared with the current
     context
   VG_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_APPEND_TO is not enabled for dstPath
   VG_ILLEGAL_ARGUMENT_ERROR
   – if pathSegments or pathData is NULL
   – if pathData is not properly aligned
   – if numSegments is less than or equal to 0
   – if pathSegments contains an illegal command
 */
void TGuestOpenVg::vgAppendPathData(VGPath aDstPath, VGint aNumSegments, const VGubyte* aPathSegments, const void* aPathData)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* dstPathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aDstPath, &dstPathInfo, VG_PATH_CAPABILITY_APPEND_TO) )
			{
			dstPathInfo->AppendPathData(*vgContext, aNumSegments, aPathSegments, aPathData);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if dstPath is not a valid path handle, or is not shared with the current
     context
   VG_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_MODIFY is not enabled for dstPath
   VG_ILLEGAL_ARGUMENT_ERROR
   – if pathData is NULL
   – if pathData is not properly aligned
   – if startIndex is less than 0
   – if numSegments is less than or equal to 0
   – if startIndex + numSegments is greater than the number of segments in the path
 */
void TGuestOpenVg::vgModifyPathCoords(VGPath aDstPath, VGint aStartIndex, VGint aNumSegments, const void* aPathData)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* dstPathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aDstPath, &dstPathInfo, VG_PATH_CAPABILITY_MODIFY) )
			{
			if ( (aStartIndex < 0) || (aNumSegments <= 0) )
				{
				vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
				}
			else
				{
				dstPathInfo->ModifyPathCoords(*vgContext, aStartIndex, aNumSegments, aPathData);
				}
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if either dstPath or srcPath is not a valid path handle, or is not shared with
     the current context
   VG_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_TRANSFORM_FROM is not enabled for srcPath
   – if VG_PATH_CAPABILITY_TRANSFORM_TO is not enabled for dstPath
 */
void TGuestOpenVg::vgTransformPath(VGPath aDstPath, VGPath aSrcPath)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* dstPathInfo = NULL;
		CVgPathInfo* srcPathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aDstPath, &dstPathInfo, VG_PATH_CAPABILITY_TRANSFORM_TO) &&
				vgLock.CheckVGPath(aSrcPath, &srcPathInfo, VG_PATH_CAPABILITY_TRANSFORM_FROM) )
			{
			dstPathInfo->TransformPath(*vgContext, *srcPathInfo);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VG_PATH_CAPABILITY_ERROR
   – If x and y are both non-NULL, and the VG_PATH_CAPABILITY_POINT_ALONG_PATH is
     not enabled for path
   – If tangentX and tangentY are both non-NULL, and the
     VG_PATH_CAPABILITY_TANGENT_ALONG_PATH capability is not enabled for path
   VG_ILLEGAL_ARGUMENT_ERROR
   – if startSegment is less than 0 or greater than the index of the final path
     segment
   – if numSegments is less than or equal to 0
   – if (startSegment + numSegments – 1) is less than 0 or greater than the index
     of the final path segment
   – if x, y, tangentX or tangentY is not properly aligned
 */
void TGuestOpenVg::vgPointAlongPath(VGPath aPath, VGint aStartSegment, VGint aNumSegments, VGfloat aDistance,
		VGfloat* aX, VGfloat* aY, VGfloat* aTangentX, VGfloat* aTangentY)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		TBool getPoint = (aX && aY);
		TBool getTangent = (aTangentX && aTangentY);
		VGbitfield reqdCapabilities = 0;
		if (getPoint)
			{
			reqdCapabilities |= VG_PATH_CAPABILITY_POINT_ALONG_PATH;
			}
		if (getTangent)
			{
			reqdCapabilities |= VG_PATH_CAPABILITY_TANGENT_ALONG_PATH;
			}
		if ( vgLock.CheckVGPath(aPath, &pathInfo, reqdCapabilities) )
			{
			if ( (aStartSegment < 0) || (aNumSegments <= 0) )
				{
				vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
				}
			else 
				{
				pathInfo->PointAlongPath(*vgContext, aStartSegment, aNumSegments, aDistance, aX, aY, aTangentX, aTangentY);
				}
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paintModes is not a valid bitwise OR of values from the VGPaintMode
     enumeration
 */
void TGuestOpenVg::vgDrawPath(VGPath aPath, VGbitfield aPaintModes)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPath(aPath, &pathInfo) )
			{
			pathInfo->DrawPath(*vgContext, aPaintModes);
			}
		}
	}


/* Paint */

/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if paint is not a valid paint handle, or is not shared with the current context
 */
void TGuestOpenVg::vgDestroyPaint(VGPaint aPaint)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPaintInfo* paintInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPaint(aPaint, &paintInfo) )
			{
			paintInfo->Destroy(*vgContext);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if paint is neither a valid paint handle nor equal to VG_INVALID_HANDLE,
     or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if paintModes is not a valid bitwise OR of values from the VGPaintMode
     enumeration
 */
void TGuestOpenVg::vgSetPaint(VGPaint aPaint, VGbitfield aPaintModes)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPaintInfo* paintInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckOptionalVGPaint(aPaint, &paintInfo) && TCheck::ChkVGPaintModesCombination(*vgContext, aPaintModes))
			{
			if (paintInfo)
				{
				paintInfo->SetPaint(*vgContext, aPaintModes);
				}
			else
				{
				CVgPaintInfo::ResetPaint(*vgContext);
				}
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if paint is not a valid paint handle, or is not shared with the current context
 */
void TGuestOpenVg::vgSetColor(VGPaint aPaint, VGuint aRgba)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPaintInfo* paintInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPaint(aPaint, &paintInfo) )
			{
			paintInfo->SetColor(*vgContext, aRgba);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if paint is not a valid paint handle, or is not shared with the current context
   – if pattern is neither a valid image handle nor equal to VG_INVALID_HANDLE, or is
     not shared with the current context
   VG_IMAGE_IN_USE_ERROR
   – if pattern is currently a rendering target
 */
void TGuestOpenVg::vgPaintPattern(VGPaint aPaint, VGImage aPattern)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgPaintInfo* paintInfo = NULL;
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGPaint(aPaint, &paintInfo) && vgLock.CheckOptionalVGImage(aPattern, &imageInfo) )
			{
			paintInfo->PaintPattern(*vgContext, imageInfo);
			}
		}
	}


/* Images */

/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if image is not a valid image handle, or is not shared with the current
     context
 */
void TGuestOpenVg::vgDestroyImage(VGImage aImage)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGImage(aImage, &imageInfo) )
			{
			imageInfo->Destroy(*vgContext);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if image is not a valid image handle, or is not shared with the current
     context
   VG_IMAGE_IN_USE_ERROR
   – if image is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
 */
void TGuestOpenVg::vgClearImage(VGImage aImage, VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGImage(aImage, &imageInfo) && TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
			{
			imageInfo->ClearImage(*vgContext, aX, aY, aWidth, aHeight);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if image is not a valid image handle, or is not shared with the current
     context
   VG_IMAGE_IN_USE_ERROR
   – if image is currently a rendering target
   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR
   – if dataFormat is not a valid value from the VGImageFormat enumeration
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
   – if data is NULL
   – if data is not properly aligned
 */
void TGuestOpenVg::vgImageSubData(VGImage aImage, const void* aData, VGint aDataStride, VGImageFormat aDataFormat,
		VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGImage(aImage, &imageInfo) && TCheck::ChkImageAlignment(*vgContext, aDataFormat, aData) &&
				TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
			{
			imageInfo->ImageSubData(*vgContext, aData, aDataStride, aDataFormat, aX, aY, aWidth, aHeight);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if either dst or src is not a valid image handle, or is not shared with the
     current context
   VG_IMAGE_IN_USE_ERROR
   – if either dst or src is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
 */
void TGuestOpenVg::vgCopyImage(VGImage aDst, VGint aDx, VGint aDy, VGImage aSrc, VGint aSx, VGint aSy,
		VGint aWidth, VGint aHeight, VGboolean aDither)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* dstImageInfo = NULL;
		CVgImageInfo* srcImageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.Check2VGImages(aDst, &dstImageInfo, aSrc, &srcImageInfo) && TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
			{
			dstImageInfo->CopyImage(*vgContext, aDx, aDy, *srcImageInfo, aSx, aSy, aWidth, aHeight, aDither);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if image is not a valid image handle, or is not shared with the current
     context
   VG_IMAGE_IN_USE_ERROR
   – if image is currently a rendering target
 */
void TGuestOpenVg::vgDrawImage(VGImage aImage)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGImage(aImage, &imageInfo) )
			{
			imageInfo->DrawImage(*vgContext);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if src is not a valid image handle, or is not shared with the current context
   VG_IMAGE_IN_USE_ERROR
   – if src is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
 */
void TGuestOpenVg::vgSetPixels(VGint aDx, VGint aDy, VGImage aSrc, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGImage(aSrc, &imageInfo) && TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
			{
			imageInfo->SetPixels(*vgContext, aDx, aDy, aSx, aSy, aWidth, aHeight);
			}
		}
	}


/*
 ERRORS
   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR
   – if dataFormat is not a valid value from the VGImageFormat enumeration
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
   – if data is NULL
   – if data is not properly aligned
 */
void TGuestOpenVg::HostVgWritePixels(MVgContext& aVgContext, const void* aPixmap, size_t aPixmapSize, VGint aHostDataStride,
		VGImageFormat aDataFormat, VGint aDx, VGint aDy, VGint aWidth, VGint aHeight)
	{
	VGPANIC_ASSERT(aPixmap != NULL, EVgPanicNullPixmapPointer);
	VGPANIC_ASSERT(aPixmapSize >= (aHostDataStride * aHeight), EVgPanicPixmapSizeError);
	RemoteFunctionCallData rfcdata; OpenVgRFC vgApiData(rfcdata);
	vgApiData.Init(OpenVgRFC::EvgWritePixels, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendVector(aPixmap, aPixmapSize);
	vgApiData.AppendParam(aHostDataStride);
	vgApiData.AppendParam(aDataFormat);
	vgApiData.AppendParam(aDx);
	vgApiData.AppendParam(aDy);
	vgApiData.AppendParam(aWidth);
	vgApiData.AppendParam(aHeight);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void TGuestOpenVg::vgWritePixels(const void* aData, VGint aDataStride, VGImageFormat aDataFormat, VGint aDx, VGint aDy,
		VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::ChkImageAlignment(*vgContext, aDataFormat, aData) &&
			TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
		{
		TInt bitsPerPixel = CVgImageInfo::BitsPerPixelForVgImageFormat(aDataFormat);
		size_t lineLength = ((static_cast<size_t>(aWidth) * static_cast<size_t>(bitsPerPixel)) + 7) / 8;
		OPENVG_TRACE("vgWritePixels.1 bitsPerPixel=%d, lineLength=%d", bitsPerPixel, lineLength);
		// ToDo clip aWidth & aHeight to the surface width & height

		if (bitsPerPixel <= 0)
			{
			vgContext->SetVgError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
			}
		else
			{
			if (lineLength  == aDataStride)
				{ // use original parameters
				OPENVG_TRACE("vgWritePixels.2a: lineLength == dataStride");
				HostVgWritePixels(*vgContext, aData, aDataStride * aHeight, aDataStride, aDataFormat, aDx, aDy, aWidth, aHeight);
				}
			else if (0 == aDataStride)
				{ // Fill operation: pixmap size for the memcpy between Symbian & Host EGL is the lineLength
				OPENVG_TRACE("vgWritePixels.2b: 0 == dataStride");
				HostVgWritePixels(*vgContext, aData, lineLength, aDataStride, aDataFormat, aDx, aDy, aWidth, aHeight);
				}
			else
				{ // datastride maybe negative or simply > lineLength
				size_t pixmapSize = lineLength * aHeight;
				TUint8* localBuffer = (TUint8*) CVghwUtils::Alloc(pixmapSize);
				OPENVG_TRACE("vgWritePixels.2c: dataStride not 0 or lineLength, localBuffer=0x%x", localBuffer);
				if (localBuffer != NULL)
					{ // reformat data into temporary buffer
					CVgImageInfo::PixmapBlit(localBuffer, static_cast<const TUint8*>(aData), lineLength, aDataStride, aHeight, lineLength);
					HostVgWritePixels(*vgContext, localBuffer, pixmapSize, lineLength, aDataFormat, aDx, aDy, aWidth, aHeight);
					CVghwUtils::Free(localBuffer);
					}
				else
					{ // alloc failed, so do VG operation row by row
					const TUint8* source = static_cast<const TUint8*>(aData);
					for (VGint row = 0; row < aHeight; ++row)
						{
						HostVgWritePixels(*vgContext, source, lineLength, lineLength, aDataFormat, aDx + row, aDy, aWidth, 1);
						source += aDataStride;
						}
					}
				}
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if dst is not a valid image handle, or is not shared with the current context
   VG_IMAGE_IN_USE_ERROR
   – if dst is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
 */
void TGuestOpenVg::vgGetPixels(VGImage aDst, VGint aDx, VGint aDy, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGImage(aDst, &imageInfo) && TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
			{
			imageInfo->GetPixels(*vgContext, aDx, aDy, aSx, aSy, aWidth, aHeight);
			}
		}
	}


/*
 ERRORS
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
 */
void TGuestOpenVg::vgCopyPixels(VGint aDx, VGint aDy, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext && TCheck::ChkPositiveImageSize(*vgContext, aWidth, aHeight) )
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgCopyPixels, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(aDx);
		vgApiData.AppendParam(aDy);
		vgApiData.AppendParam(aSx);
		vgApiData.AppendParam(aSy);
		vgApiData.AppendParam(aWidth);
		vgApiData.AppendParam(aHeight);
		vgContext->ExecuteVgCommand(vgApiData);
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if font is not a valid font handle, or is not shared with the current context
 */
void TGuestOpenVg::vgDestroyFont(VGFont aFont)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgFontInfo* fontInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGFont(aFont, &fontInfo) )
			{
			fontInfo->Destroy(*vgContext);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if font is not a valid font handle, or is not shared with the current context
   – if path is not a valid path handle or VG_INVALID_HANDLE, or is not shared
     with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if the pointer to glyphOrigin or escapement is NULL or is not properly
     aligned
 */
void TGuestOpenVg::vgSetGlyphToPath(VGFont aFont, VGuint aGlyphIndex, VGPath aPath, VGboolean aIsHinted,
		const VGfloat aGlyphOrigin [2], const VGfloat aEscapement[2])
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgFontInfo* fontInfo = NULL;
		CVgPathInfo* pathInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGFont(aFont, &fontInfo) && vgLock.CheckOptionalVGPath(aPath, &pathInfo) &&
				TCheck::Chk2x32bitPtr(*vgContext, aGlyphOrigin, aEscapement) )
			{
			fontInfo->SetGlyphToPath(*vgContext, aGlyphIndex, pathInfo, aIsHinted, aGlyphOrigin, aEscapement);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if font is not a valid font handle, or is not shared with the current context
   – if image is not a valid image handle or VG_INVALID_HANDLE, or is not
     shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if the pointer to glyphOrigin or escapement is NULL or is not properly
     aligned
   VG_IMAGE_IN_USE_ERROR
   – if image is currently a rendering target
 */
void TGuestOpenVg::vgSetGlyphToImage(VGFont aFont, VGuint aGlyphIndex, VGImage aImage, const VGfloat aGlyphOrigin [2],
		const VGfloat aEscapement[2])
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgFontInfo* fontInfo = NULL;
		CVgImageInfo* imageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGFont(aFont, &fontInfo) && TCheck::Chk2x32bitPtr(*vgContext, aGlyphOrigin, aEscapement) &&
				vgLock.CheckOptionalVGImage(aImage, &imageInfo) )
			{
			fontInfo->SetGlyphToImage(*vgContext, aGlyphIndex, imageInfo, aGlyphOrigin, aEscapement);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if font is not a valid font handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if glyphIndex is not defined for the font
 */
void TGuestOpenVg::vgClearGlyph(VGFont aFont, VGuint aGlyphIndex)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgFontInfo* fontInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGFont(aFont, &fontInfo) )
			{
			fontInfo->ClearGlyph(*vgContext, aGlyphIndex);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if font is not a valid font handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if glyphIndex has not been defined for a given font object
   – if paintModes is not a valid bitwise OR of values from the VGPaintMode
     enumeration, or 0
 */
void TGuestOpenVg::vgDrawGlyph(VGFont aFont, VGuint aGlyphIndex, VGbitfield aPaintModes, VGboolean aAllowAutoHinting)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgFontInfo* fontInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGFont(aFont, &fontInfo) && TCheck::ChkVGPaintModesCombination(*vgContext, aPaintModes) )
			{
			fontInfo->DrawGlyph(*vgContext, aGlyphIndex, aPaintModes, aAllowAutoHinting);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if font is not a valid font handle, or is not shared with the current context
   VG_ILLEGAL_ARGUMENT_ERROR
   – if glyphCount is zero or a negative value
   – if the pointer to the glyphIndices array is NULL or is not properly
     aligned
   – if a pointer to either of the adjustments_x or adjustments_y arrays are
     non-NULL and are not properly aligned
   – if any of the glyphIndices has not been defined in a given font object
   – if paintModes is not a valid bitwise OR of values from the VGPaintMode
     enumeration, or 0
 */
void TGuestOpenVg::vgDrawGlyphs(VGFont aFont, VGint aGlyphCount, const VGuint* aGlyphIndices,
		const VGfloat* aAdjustmentsX, const VGfloat* aAdjustmentsY, VGbitfield aPaintModes, VGboolean aAllowAutoHinting)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgFontInfo* fontInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.CheckVGFont(aFont, &fontInfo) && TCheck::Chk32bitPtr(*vgContext, aGlyphIndices) &&
				TCheck::ChkVGPaintModesCombination(*vgContext, aPaintModes) &&
				TCheck::Chk2xOptional32bitPtr(*vgContext, aAdjustmentsX, aAdjustmentsY) )
			{
			fontInfo->DrawGlyphs(*vgContext, aGlyphCount, aGlyphIndices, aAdjustmentsX, aAdjustmentsY, aPaintModes, aAllowAutoHinting);
			}
		}
	}


/* Image Filters */

/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if either dst or src is not a valid image handle, or is not shared with the
     current context
   VG_IMAGE_IN_USE_ERROR
   – if either dst or src is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if src and dst overlap
   – if matrix is NULL
   – if matrix is not properly aligned
 */
void TGuestOpenVg::vgColorMatrix(VGImage aDst, VGImage aSrc, const VGfloat* aMatrix)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		// **** Desirable: check that neither aDst or aSrc are current rendering targets, or overlapping
		CVgImageInfo* dstImageInfo = NULL;
		CVgImageInfo* srcImageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.Check2VGImages(aDst, &dstImageInfo, aSrc, &srcImageInfo) && TCheck::Chk32bitPtr(*vgContext, aMatrix) )
			{
			dstImageInfo->ColorMatrix(*vgContext, *srcImageInfo, aMatrix);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if either dst or src is not a valid image handle, or is not shared with the
     current context
   VG_IMAGE_IN_USE_ERROR
   – if either dst or src is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if src and dst overlap
   – if kernelWidth or kernelHeight is less than or equal to 0 or greater than
     VG_MAX_KERNEL_SIZE
   – if kernel is NULL
   – if kernel is not properly aligned
   – if tilingMode is not one of the values from the VGTilingMode enumeration
 */
void TGuestOpenVg::vgConvolve(VGImage aDst, VGImage aSrc, VGint aKernelWidth, VGint aKernelHeight,
		VGint aShiftX, VGint aShiftY, const VGshort* aKernel, VGfloat aScale, VGfloat aBias, VGTilingMode aTilingMode)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* dstImageInfo = NULL;
		CVgImageInfo* srcImageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.Check2VGImages(aDst, &dstImageInfo, aSrc, &srcImageInfo) && TCheck::Chk16bitPtr(*vgContext, aKernel) &&
				TCheck::ChkVGTilingMode(*vgContext, aTilingMode) &&
				vgLock.CheckKernelWidthAndHeight(aKernelHeight, aKernelWidth, VG_MAX_KERNEL_SIZE) )
			{
			dstImageInfo->Convolve(*vgContext, *srcImageInfo, aKernelWidth, aKernelHeight,
					aShiftX, aShiftY, aKernel, aScale, aBias, aTilingMode);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if either dst or src is not a valid image handle, or is not shared with the
     current context
   VG_IMAGE_IN_USE_ERROR
   – if either dst or src is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if src and dst overlap
   – if kernelWidth or kernelHeight is less than or equal to 0 or greater than
     VG_MAX_SEPARABLE_KERNEL_SIZE
   – if kernelX or kernelY is NULL
   – if kernelX or kernelY is not properly aligned
   – if tilingMode is not one of the values from the VGTilingMode
     enumeration
 */
void TGuestOpenVg::vgSeparableConvolve(VGImage aDst, VGImage aSrc, VGint aKernelWidth, VGint aKernelHeight,
		VGint aShiftX, VGint aShiftY, const VGshort* aKernelX, const VGshort* aKernelY, VGfloat aScale, VGfloat aBias, VGTilingMode aTilingMode)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* dstImageInfo = NULL;
		CVgImageInfo* srcImageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.Check2VGImages(aDst, &dstImageInfo, aSrc, &srcImageInfo) &&
				TCheck::Chk2x16bitPtr(*vgContext, aKernelX, aKernelY) && TCheck::ChkVGTilingMode(*vgContext, aTilingMode) &&
				vgLock.CheckKernelWidthAndHeight(aKernelWidth, aKernelHeight, VG_MAX_SEPARABLE_KERNEL_SIZE) )
			{
			dstImageInfo->SeparableConvolve(*vgContext, *srcImageInfo, aKernelWidth, aKernelHeight,
					aShiftX, aShiftY, aKernelX, aKernelY, aScale, aBias, aTilingMode);
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if either dst or src is not a valid image handle, or is not shared with the
     current context
   VG_IMAGE_IN_USE_ERROR
   – if either dst or src is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if src and dst overlap
   – if stdDeviationX or stdDeviationY is less than or equal to 0 or greater
     than VG_MAX_GAUSSIAN_STD_DEVIATION
   – if tilingMode is not one of the values from the VGTilingMode
     enumeration
 */
void TGuestOpenVg::vgGaussianBlur(VGImage aDst, VGImage aSrc, VGfloat aStdDeviationX, VGfloat aStdDeviationY, VGTilingMode aTilingMode)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* dstImageInfo = NULL;
		CVgImageInfo* srcImageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.Check2VGImages(aDst, &dstImageInfo, aSrc, &srcImageInfo) && TCheck::ChkVGTilingMode(*vgContext, aTilingMode) )
			{
			// **** Desirable: check against VG_MAX_GAUSSIAN_STD_DEVIATION limit from x86 VG implementation
			if ( (aStdDeviationX <= KFloatZero) || (aStdDeviationY <= KFloatZero))
				{
				vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
				}
			else
				{
				dstImageInfo->GaussianBlur(*vgContext, *srcImageInfo, aStdDeviationX, aStdDeviationY, aTilingMode);
				}
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if either dst or src is not a valid image handle, or is not shared with the
     current context
   VG_IMAGE_IN_USE_ERROR
   – if either dst or src is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if src and dst overlap
   – if any pointer parameter is NULL
 */
void TGuestOpenVg::vgLookup(VGImage aDst, VGImage aSrc, const VGubyte* aRedLUT, const VGubyte* aGreenLUT,
		const VGubyte* aBlueLUT, const VGubyte* aAlphaLUT,  VGboolean aOutputLinear, VGboolean aOutputPremultiplied)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* dstImageInfo = NULL;
		CVgImageInfo* srcImageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.Check2VGImages(aDst, &dstImageInfo, aSrc, &srcImageInfo) )
			{
			if ( (aRedLUT == NULL) || (aGreenLUT == NULL) || (aBlueLUT == NULL) || (aAlphaLUT == NULL) )
				{
				vgContext->SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
				}
			else
				{
				dstImageInfo->Lookup(*vgContext, *srcImageInfo, aRedLUT, aGreenLUT, aBlueLUT, aAlphaLUT, aOutputLinear, aOutputPremultiplied);
				}
			}
		}
	}


/*
 ERRORS
   VG_BAD_HANDLE_ERROR
   – if either dst or src is not a valid image handle, or is not shared with the
     current context
   VG_IMAGE_IN_USE_ERROR
   – if either dst or src is currently a rendering target
   VG_ILLEGAL_ARGUMENT_ERROR
   – if src and dst overlap
   – if src is in an RGB pixel format and sourceChannel is not one of VG_RED,
     VG_GREEN, VG_BLUE or VG_ALPHA from the VGImageChannel enumeration
   – if lookupTable is NULL
   – if lookupTable is not properly aligned
 */
void TGuestOpenVg::vgLookupSingle(VGImage aDst, VGImage aSrc, const VGuint* aLookupTable,
		VGImageChannel aSourceChannel, VGboolean aOutputLinear, VGboolean aOutputPremultiplied)
	{
	MVgContext* vgContext = CVghwUtils::VgContext();
	if ( vgContext )
		{
		CVgImageInfo* dstImageInfo = NULL;
		CVgImageInfo* srcImageInfo = NULL;
		TCleanupVgLocks vgLock(*vgContext); // grab VG State Mutex & automatically release on destruction

		if ( vgLock.Check2VGImages(aDst, &dstImageInfo, aSrc, &srcImageInfo) && TCheck::Chk32bitPtr(*vgContext, aLookupTable) )
			{
			dstImageInfo->LookupSingle(*vgContext, *srcImageInfo, aLookupTable,
					aSourceChannel, aOutputLinear, aOutputPremultiplied);
			}
		}
	}


/*
 ERRORS
   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR
   - if the OpenVG implementation is not able to create a VGImage
     compatible with the provided VGeglImageKHR for an implementation-
     dependent reason (this could be caused by, but not limited to,
     reasons such as unsupported pixel formats, anti-aliasing quality,
     etc.).

   VG_ILLEGAL_ARGUMENT_ERROR
   - if <image> is not a valid VGeglImageKHR.
 */
VGImage TGuestOpenVg::vgCreateEGLImageTargetKHR(VGeglImageKHR aImage)
	{
	VGImage imageHandle = VG_INVALID_HANDLE;
	MVgContext* vgContext = CVghwUtils::VgContext();
	if (vgContext)
		{
		TCleanupVgLocks vgLock(*vgContext);
		imageHandle = vgLock.CreateEGLImageTargetKHR(aImage);
		}
	else
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreateEGLImageTargetKHR - no VG context");
		}

	return imageHandle;
	}


// end of file vgapi.cpp
