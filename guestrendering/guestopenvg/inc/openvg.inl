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
// Inline functions for Open VG client-side state classes.

#ifndef OPENVG_INL
#define OPENVG_INL

#include "vgstate.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// XOpenVgState
/////////////////////////////////////////////////////////////////////////////////////////////

RMutex& XOpenVgState::MutexWait()
	{
	iStateLock.Wait();
	return iStateLock;
	}


/////////////////////////////////////////////////////////////////////////////////////////////
// CVgHandleBase
/////////////////////////////////////////////////////////////////////////////////////////////

CVgHandleBase::CVgHandleBase(TVgHandleType aHandleType) :
		iClientHandle(iHandleGen.NextUid()), iHandleType(aHandleType), iHostHandle(VG_INVALID_HANDLE), iIsDestroyed(EFalse)
	{
	}


CVgHandleBase::~CVgHandleBase()
	{
	VGPANIC_ASSERT_DEBUG( iIsDestroyed || (iHostHandle == VG_INVALID_HANDLE), EVgPanicTemp);
	}


TUint CVgHandleBase::ClientHandle() const
	{
	return iClientHandle;
	}


VGHandle CVgHandleBase::HostHandle() const
	{
	VGPANIC_ASSERT(iHostHandle, EVgPanicHostHandleIsInvalid);
	return iHostHandle;
	}


TVgHandleType CVgHandleBase::HandleType() const
	{
	return iHandleType;
	}


TBool CVgHandleBase::IsDestroyed() const
	{
	return iIsDestroyed;
	}


void CVgHandleBase::SetHostHandle(VGHandle aHostHandle)
	{ // should be set exactly once (For EglImages it should be the underlying VG Image)
	VGPANIC_ASSERT(iHostHandle == 0, EVgPanicHostHandleAlreadySet);
	VGPANIC_ASSERT(aHostHandle, EVgPanicHostHandleIsInvalid);
	iHostHandle = aHostHandle;
	}


/////////////////////////////////////////////////////////////////////////////////////////////
// CVgImageBase
/////////////////////////////////////////////////////////////////////////////////////////////

CVgImageBase::CVgImageBase(TVgHandleType aHandleType, VGint aWidth, VGint aHeight) :
		CVgHandleBase(aHandleType), iWidth(aWidth), iHeight(aHeight)
	{
	}


VGint CVgImageBase::Width() const
	{
	return iWidth;
	}


VGint CVgImageBase::Height() const
	{
	return iHeight;
	}


/////////////////////////////////////////////////////////////////////////////////////////////
// TCleanupVgLocks
/////////////////////////////////////////////////////////////////////////////////////////////

/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aPath is not a valid VGPath handle
 */
TBool TCleanupVgLocks::CheckVGPath(VGPath aPath, CVgPathInfo** aPathInfo, VGbitfield aReqdCapabilities)
	{ // VGPath is an opaque data handle
	TBool valid = OpenVgState.CheckVGHandle(iVgContext, aPath, (CVgHandleBase**)aPathInfo, EVgHandleForPath);
	if (valid && aReqdCapabilities)
		{  // Check for VG_PATH_CAPABILITY_ERROR
		return (*aPathInfo)->CheckHasPathCapabilities(iVgContext, aReqdCapabilities);
		}
	return valid;
	}


/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aPath is neither VG_INVALID_HANDLE or a valid VGPath handle
 */
TBool TCleanupVgLocks::CheckOptionalVGPath(VGPath aPath, CVgPathInfo** aPathInfo)
	{ // VGPath is an opaque data handle
	if (aPath == VG_INVALID_HANDLE)
		{
		return ETrue;
		}
	return OpenVgState.CheckVGHandle(iVgContext, aPath, (CVgHandleBase**)aPathInfo, EVgHandleForPath);
	}


/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aPaint is not a valid VGPaint handle
 */
TBool TCleanupVgLocks::CheckVGPaint(VGPaint aPaint, CVgPaintInfo** aPaintInfo)
	{ // VGPaint is an opaque data handle
	return OpenVgState.CheckVGHandle(iVgContext, aPaint, (CVgHandleBase**)aPaintInfo, EVgHandleForPaint);
	}


/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aPaint is neither VG_INVALID_HANDLE or a valid VGPaint handle
 */
TBool TCleanupVgLocks::CheckOptionalVGPaint(VGPaint aPaint, CVgPaintInfo** aPaintInfo)
	{ // VGPaint is an opaque data handle
	if (aPaint == VG_INVALID_HANDLE)
		{
		return ETrue;
		}
	return OpenVgState.CheckVGHandle(iVgContext, aPaint, (CVgHandleBase**)aPaintInfo, EVgHandleForPaint);
	}


/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aImage is not a valid VGImage handle
 */
TBool TCleanupVgLocks::CheckVGImage(VGImage aImage, CVgImageInfo** aImageInfo)
	{ // VGImage is an opaque data handle
	return OpenVgState.CheckVGHandle(iVgContext, aImage, (CVgHandleBase**)aImageInfo, EVgHandleForImage);
	}


/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aImage is neither VG_INVALID_HANDLE or a valid VGImage handle
 */
TBool TCleanupVgLocks::CheckOptionalVGImage(VGImage aImage, CVgImageInfo** aImageInfo)
	{ // VGImage is an opaque data handle
	if (aImage == VG_INVALID_HANDLE)
		{
		return ETrue;
		}
	return OpenVgState.CheckVGHandle(iVgContext, aImage, (CVgHandleBase**)aImageInfo, EVgHandleForImage);
	}


/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aImage is not a valid VGImage handle,
 or if the 2 Images are the same.
 */
TBool TCleanupVgLocks::Check2VGImages(VGImage aImage1, CVgImageInfo** aImageInfo1, VGImage aImage2, CVgImageInfo** aImageInfo2)
	{ // VGImage is an opaque data handle
	// ToDo some (all?) callers need the 2 images to not be overlapping
	if (aImage1 != aImage2)
		{
		return OpenVgState.CheckVGHandle(iVgContext, aImage1, (CVgHandleBase**)aImageInfo1, EVgHandleForImage) &&
				OpenVgState.CheckVGHandle(iVgContext, aImage2, (CVgHandleBase**)aImageInfo2, EVgHandleForImage);
		}
	iVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return EFalse;
	}

/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aFont is not a valid VGFont handle
 */
TBool TCleanupVgLocks::CheckVGFont(VGFont aFont, CVgFontInfo** aFontInfo)
	{ // VGFont is an opaque data handle
	return OpenVgState.CheckVGHandle(iVgContext, aFont, (CVgHandleBase**)aFontInfo, EVgHandleForFont);
	}


/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aMaskLayer is not a valid VGMaskLayer handle
 */
TBool TCleanupVgLocks::CheckVGMaskLayer(VGMaskLayer aMaskLayer, CVgMaskLayerInfo** aMaskLayerInfo)
	{ // VGMaskLayer is an opaque data handle
	return OpenVgState.CheckVGHandle(iVgContext, aMaskLayer, (CVgHandleBase**)aMaskLayerInfo, EVgHandleForMaskLayer);
	}


/*
 Returns false & sets VG error to VG_BAD_HANDLE_ERROR if aHandle is not a valid Open VG handle
 */
TBool TCleanupVgLocks::CheckVGAnyHandle(VGHandle aHandle, CVgHandleBase** aHandleInfo)
	{
	return OpenVgState.CheckVGHandle(iVgContext, aHandle, aHandleInfo, EVgHandleAny);
	}


TBool TCleanupVgLocks::CheckKernelWidthAndHeight(VGint aKernelWidth, VGint aKernelHeight, VGParamType aLimitType)
	{
	return OpenVgState.CheckKernelWidthAndHeight(iVgContext, aKernelWidth, aKernelHeight, aLimitType);
	}


/////////////////////////////////////////////////////////////////////////////////////////////
// TCheck - parameter checking utility functions
/////////////////////////////////////////////////////////////////////////////////////////////

/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if aDataPtr is NULL or not 32-bit aligned
 */
TBool TCheck::Chk32bitPtr(MVgContext& aVgContext, const void* aDataPtr)
	{
	TBool result = (aDataPtr != NULL) && !(3ul & (unsigned)aDataPtr);
	if (!result)
		{
		OPENVG_TRACE("TCheck::Chk32bitPtr setting VG_ILLEGAL_ARGUMENT_ERROR");
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return result;
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if aDataPtrN is NULL or not 32-bit aligned
 */
TBool TCheck::Chk2x32bitPtr(MVgContext& aVgContext, const void* aDataPtr1, const void* aDataPtr2)
	{
	TBool result = (aDataPtr1 != NULL) && !(3ul & (unsigned)aDataPtr1) && (aDataPtr2 != NULL) && !(3ul & (unsigned)aDataPtr2);
	if (!result)
		{
		OPENVG_TRACE("TCheck::Chk2x32bitPtr setting VG_ILLEGAL_ARGUMENT_ERROR");
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return result;
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if aDataPtrN is NULL or not 32-bit aligned
 */
TBool TCheck::Chk4x32bitPtr(MVgContext& aVgContext, const void* aDataPtr1, const void* aDataPtr2, const void* aDataPtr3, const void* aDataPtr4)
	{
	TBool result = (aDataPtr1 != NULL) && !(3ul & (unsigned)aDataPtr1) && (aDataPtr2 != NULL) && !(3ul & (unsigned)aDataPtr2) &&
			 (aDataPtr3 != NULL) && !(3ul & (unsigned)aDataPtr3) && (aDataPtr4 != NULL) && !(3ul & (unsigned)aDataPtr4);
	if (!result)
		{
		OPENVG_TRACE("TCheck::Chk4x32bitPtr setting VG_ILLEGAL_ARGUMENT_ERROR");
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return result;
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if aDataPtrN is not-NULL and not 32-bit aligned
 */
TBool TCheck::Chk2xOptional32bitPtr(MVgContext& aVgContext, const void* aDataPtr1, const void* aDataPtr2)
	{
	TBool result = !(3ul & (unsigned)aDataPtr1) && !(3ul & (unsigned)aDataPtr2);
	if (!result)
		{
		OPENVG_TRACE("TCheck::Chk2xOptional32bitPtr setting VG_ILLEGAL_ARGUMENT_ERROR");
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return result;
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if aDataPtr is NULL or not 16-bit aligned
 */
TBool TCheck::Chk16bitPtr(MVgContext& aVgContext, const void* aDataPtr)
	{
	TBool result = (aDataPtr != NULL) && !(1ul & (unsigned)aDataPtr);
	if (!result)
		{
		OPENVG_TRACE("TCheck::Chk16bitPtr setting VG_ILLEGAL_ARGUMENT_ERROR");
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return result;
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if either aDataPtr1 or aDataPtr2 is NULL or not 16-bit aligned
 */
TBool TCheck::Chk2x16bitPtr(MVgContext& aVgContext, const void* aDataPtr1, const void* aDataPtr2)
	{
	TBool result = (aDataPtr1 != NULL) && !(1ul & (unsigned)aDataPtr1) && (aDataPtr2 != NULL) && !(1ul & (unsigned)aDataPtr2);
	if (!result)
		{
		OPENVG_TRACE("TCheck::Chk2x16bitPtr setting VG_ILLEGAL_ARGUMENT_ERROR");
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return result;
	}


/*
 Returns false & sets VG error to VG_UNSUPPORTED_PATH_FORMAT_ERROR if pathFormat is not a supported format
 */
TBool TCheck::ChkVGPathFormat(MVgContext& aVgContext, VGint aPathFormat)
	{
	switch (aPathFormat)
		{
		case VG_PATH_FORMAT_STANDARD:
			return ETrue;
		default:
			OPENVG_TRACE("TCheck::ChkVGPathFormat setting VG_UNSUPPORTED_PATH_FORMAT_ERROR");
			aVgContext.SetVgError(VG_UNSUPPORTED_PATH_FORMAT_ERROR);
			return EFalse;
		}
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if datatype is not a valid value from the VGPathDatatype enumeration
 */
TBool TCheck::ChkVGPathDataType(MVgContext& aVgContext, VGPathDatatype aDatatype)
	{
	switch (aDatatype)
		{
		case VG_PATH_DATATYPE_S_8:
		case VG_PATH_DATATYPE_S_16:
		case VG_PATH_DATATYPE_S_32:
		case VG_PATH_DATATYPE_F:
			return ETrue;
		default:
			OPENVG_TRACE("TCheck::ChkVGPathDataType setting VG_ILLEGAL_ARGUMENT_ERROR");
			aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			return EFalse;
		}
	}


/*
 aWidth & aHeight parameters must be >0, otherwise a VG_ILLEGAL_ARGUMENT_ERROR is flagged.
 */
TBool TCheck::ChkPositiveImageSize(MVgContext& aVgContext, VGint aWidth, VGint aHeight)
	{
	if ( (aWidth > 0) && (aHeight > 0) )
		{
		return ETrue;
		}
	OPENVG_TRACE("TCheck::ChkPositiveImageSize setting VG_ILLEGAL_ARGUMENT_ERROR");
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return EFalse;
	}


/*
 aWidth & aHeight parameters must be >0, otherwise a VG_ILLEGAL_ARGUMENT_ERROR is flagged.
 */
TBool TCheck::ChkNewImageSize(MVgContext& aVgContext, VGint aWidth, VGint aHeight)
	{
	// ToDo compare with max limits fetched from Host Open VG:VG_MAX_IMAGE_WIDTH, VG_MAX_IMAGE_HEIGHT & VG_MAX_IMAGE_PIXELS
	if ( (aWidth > 0) && (aHeight > 0) )
		{
		return ETrue;
		}
	OPENVG_TRACE("TCheck::ChkNewImageSize setting VG_ILLEGAL_ARGUMENT_ERROR");
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return EFalse;
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if aPaintMode is not a valid VGPaintMode
 */
TBool TCheck::ChkVGPaintMode(MVgContext& aVgContext, VGPaintMode aPaintMode)
	{
	switch(aPaintMode)
		{
		case VG_STROKE_PATH:
		case VG_FILL_PATH:
			return ETrue;
		default:
			OPENVG_TRACE("TCheck::ChkVGPaintMode setting VG_ILLEGAL_ARGUMENT_ERROR");
			aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			return EFalse;
		}
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if aPaintModes has invalid VGPaintMode bits set
 */
TBool TCheck::ChkVGPaintModesCombination(MVgContext& aVgContext, VGbitfield aPaintModes)
	{
	if (aPaintModes & ~(VG_STROKE_PATH | VG_FILL_PATH))
		{
		OPENVG_TRACE("TCheck::ChkVGPaintModesCombination setting VG_ILLEGAL_ARGUMENT_ERROR");
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		return EFalse;
		}
	return ETrue;
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if VGMaskOperation is not a supported operation
 */
TBool TCheck::ChkVGMaskOperation(MVgContext& aVgContext, VGMaskOperation aOperation)
	{
	switch (aOperation)
		{
		case VG_CLEAR_MASK:
		case VG_FILL_MASK:
		case VG_SET_MASK:
		case VG_UNION_MASK:
		case VG_INTERSECT_MASK:
		case VG_SUBTRACT_MASK:
			return ETrue;
		default:
			OPENVG_TRACE("TCheck::ChkVGMaskOperation setting VG_ILLEGAL_ARGUMENT_ERROR");
			aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			return EFalse;
		}
	}


TBool TCheck::ChkVgImageFormat(MVgContext& aVgContext, VGImageFormat aFormat)
	{
	if ( ImageFormatByteDepth(aFormat) )
		{
		return ETrue;
		}
	OPENVG_TRACE("TCheck::ChkVgImageFormat setting VG_UNSUPPORTED_IMAGE_FORMAT_ERROR");
	aVgContext.SetVgError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
	return EFalse;
	}


/*
 Checks aData is aligned according to the specified VGImageFormet.

 On failure sets these VgErrors:
    VG_UNSUPPORTED_IMAGE_FORMAT_ERROR
   – if aImageFormat is not a valid value from the VGImageFormat enumeration
   VG_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
   – if data is NULL
   – if data is not properly aligned

 */
TBool TCheck::ChkImageAlignment(MVgContext& aVgContext, VGImageFormat aImageFormat, const void* aData)
	{
	TBool result = EFalse;
	if (aData != NULL)
		{
		switch( ImageFormatByteDepth(aImageFormat) )
			{
			case 1:
				result = ETrue;
				break;
			case 2:
				result = !( 1ul & (unsigned)aData );
				break;
			case 4:
				result = !( 3ul & (unsigned)aData );
				break;
			default:
				OPENVG_TRACE("TCheck::ImageAlignment setting VG_UNSUPPORTED_IMAGE_FORMAT_ERROR");
				aVgContext.SetVgError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
				return EFalse;
			}
		}
	if (!result)
		{
		OPENVG_TRACE("TCheck::ImageAlignment setting VG_ILLEGAL_ARGUMENT_ERROR");
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	return result;
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if aTilingMode is not a valid value from the
 VGTilingMode enumeration
 */
TBool TCheck::ChkVGTilingMode(MVgContext& aVgContext, VGTilingMode aTilingMode)
	{
	switch (aTilingMode)
		{
		case VG_TILE_FILL:
		case VG_TILE_PAD:
		case VG_TILE_REPEAT:
		case VG_TILE_REFLECT:
			return ETrue;
		default:
			OPENVG_TRACE("TCheck::ChkVGTilingMode setting VG_ILLEGAL_ARGUMENT_ERROR");
			aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			return EFalse;
		}
	}


#endif // OPENVG_INL
