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


const TInt KColorMatrixSize = 20;
const TInt KLUTLength = 256;


/////////////////////////////////////////////////////////////////////////////////////////////
// CVgImageInfo
/////////////////////////////////////////////////////////////////////////////////////////////

CVgImageInfo::CVgImageInfo(VGImageFormat aFormat, VGint aWidth, VGint aHeight, CVgImageInfo* aParent, VGbitfield aAllowedQuality,
			EGLImageKHR aEglImage, TUint64 aSgImageId) :
		CVgImageBase(EVgHandleForImage, aWidth, aHeight), iFormat(aFormat), iParent(aParent),
		iAllowedQuality(aAllowedQuality), iIsEglSibling( aSgImageId ? ETrue : EFalse),
		iEglImage(aEglImage), iSgImageId(aSgImageId),
		iChildCount(0)
	{
	if (aParent)
		{
		aParent->IncChildCount();
		}
	}


// eglChildImage
CVgImageInfo* CVgImageInfo::New(VGint aWidth, VGint aHeight, CVgImageInfo* aParent)
	{
	VGPANIC_ASSERT_DEBUG(aParent, EVgPanicImageParentIsInvalid);
	RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
	CVgImageInfo* self = new CVgImageInfo(aParent->iFormat, aWidth, aHeight, aParent, aParent->AllowedQuality(),
			aParent->EglImage(), aParent->SgImageId());
	CVghwUtils::SwitchFromVghwHeap(clientHeap);
	return self;
	}


// vgCreateEGLImageTargetKHR
CVgImageInfo* CVgImageInfo::New(VGImageFormat aFormat, VGint aWidth, VGint aHeight, EGLImageKHR aEglImage, TUint64& aSgImageId)
	{
	RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
	CVgImageInfo* self = new CVgImageInfo(aFormat, aWidth, aHeight, VG_INVALID_HANDLE, 0, aEglImage, aSgImageId);
	CVghwUtils::SwitchFromVghwHeap(clientHeap);
	return self;
	}


// eglCreateImage
CVgImageInfo* CVgImageInfo::New(VGImageFormat aFormat, VGint aWidth, VGint aHeight, VGbitfield aAllowedQuality)
	{
	RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
	CVgImageInfo* self = new CVgImageInfo(aFormat, aWidth, aHeight, VG_INVALID_HANDLE, aAllowedQuality, NULL, 0l);
	CVghwUtils::SwitchFromVghwHeap(clientHeap);
	return self;
	}


CVgImageInfo::~CVgImageInfo()
	{
	VGPANIC_ASSERT(iChildCount == 0, EVgPanicDeleteInUseVgImageInfo);
	if (iParent)
		{
		iParent->DecChildCount();
		}
	}


CVgImageInfo* CVgImageInfo::Parent() const
	{
	return iParent;
	}


TInt CVgImageInfo::ChildCount() const
	{
	return iChildCount;
	}


void CVgImageInfo::IncChildCount()
	{
	VGPANIC_ASSERT(!iIsDestroyed, EVgPanicParentImageAlreadyDestroyed);
	User::LockedInc(iChildCount);
	}


void CVgImageInfo::DecChildCount()
	{
	User::LockedDec(iChildCount);
	if ((iChildCount == 0) && iIsDestroyed)
		{
		if (iParent == NULL)
			{
			if (IsEglSibling())
				{ // close the root EglImage
				OpenVgState.EglImageClose(iEglImage);
				}
			}
		else
			{
			iParent->DecChildCount();
			}
		// delete this image & remove it from HandleMap
		RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
		OpenVgState.UnMapHandle(iClientHandle);
		delete this;
		CVghwUtils::SwitchFromVghwHeap(clientHeap);
		}
	}


VGbitfield CVgImageInfo::AllowedQuality() const
	{
	return iAllowedQuality;
	}


TBool CVgImageInfo::IsEglSibling() const
	{
	return iIsEglSibling;
	}


EGLImageKHR CVgImageInfo::EglImage() const
	{
	return iEglImage;
	}


TUint64 CVgImageInfo::SgImageId() const
	{
	return iSgImageId;
	}


TBool CVgImageInfo::DestroyObject(MVgContext& aVgContext)
	{
	VGPANIC_ASSERT_DEBUG(iIsDestroyed, EVgPanicTemp);
	OPENVG_TRACE("  CVgImageInfo::DestroyObject HostHandle=0x%x; ChildCount=%d, Parent=0x%x, IsEglSibling=%d, EglImage=0x%x",
			iHostHandle, ChildCount(), Parent(), IsEglSibling(), EglImage());

	if (iHostHandle)
		{
		// Destroy image, but not for eglImages/SgImages
		if (!IsEglSibling())
			{
			RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
			vgApiData.Init(OpenVgRFC::EvgDestroyImage, RemoteFunctionCallData::EOpRequest);
			vgApiData.AppendParam(iHostHandle);
			aVgContext.ExecuteVgCommand(vgApiData);
			}
		iHostHandle = VG_INVALID_HANDLE;
		}

	if (iChildCount == 0)
		{
		if (iParent == NULL)
			{
			if (IsEglSibling())
				{ // close the root EglImage
				OpenVgState.EglImageClose(iEglImage);
				}
			}
		else
			{
			iParent->DecChildCount();
			}
		return ETrue;
		}
	// don't delete this image until all children have been destroyed
	return EFalse;
	}


TInt CVgImageInfo::BitsPerPixelForVgImageFormat(VGImageFormat aFormat)
	{
	TInt result = -1;
	if ((aFormat == VG_BW_1) || (aFormat == VG_A_1))
		{
		result = 1;
		}
	else if (aFormat == VG_A_4)
		{
		result = 4;
		}
	else if ((aFormat == VG_sL_8) || (aFormat == VG_lL_8) || (aFormat == VG_A_8))
		{
		result = 8;
		}
	else if ((aFormat >= 0) && (aFormat < 256))
		{ // low bits of format number repeat in a pattern for 16/32 bit per pel
		TInt format = aFormat & 0x3f;
		if ((format >= VG_sRGB_565) && (format <= VG_sRGBA_4444))
			{
			result = 16;
			}
		else if (format <= VG_lRGBA_8888_PRE)
			{
			result = 32;
			}
		}
	return result;
	}


void CVgImageInfo::PixmapBlit(TUint8* aDest, const TUint8* aSource, TInt aDestStride, TInt aSourceStride, TInt aRowCount,
		size_t aRowLength, TInt aLastBits)
	{
	VGPANIC_ASSERT_DEBUG(aDest, EVgPanicNullPointer);
	VGPANIC_ASSERT_DEBUG(aSource, EVgPanicNullPointer);
	VGPANIC_ASSERT_DEBUG(aDestStride >= aRowLength, EVgPanicStrideSmallerThanRowLength);
	VGPANIC_ASSERT_DEBUG(aSourceStride >= aRowLength, EVgPanicStrideSmallerThanRowLength);
	VGPANIC_ASSERT_DEBUG(aRowCount > 0, EVgPanicBadRowCountParam);
	VGPANIC_ASSERT_DEBUG(aRowLength > 0, EVgPanicBadRowLengthParam);
	VGPANIC_ASSERT_DEBUG( (aLastBits >= 0) && (aLastBits <= 7), EVgPanicBadLastBitsParam);

	if (aLastBits)
		{ // bits per pixel < 8, only copy lowest <aLastBits> from source to dest
		TUint8 destMask = 0xFF << aLastBits; // high bits
		TUint8 srcMask = 0xFF ^ destMask; // low bits
		aRowLength -= 1;
		for (VGint row = 0; row < aRowCount; ++row)
			{
			memcpy(aDest, aSource, aRowLength);
			TUint8 byte = (aDest[aRowLength] & destMask) | (aSource[aRowLength] & srcMask);
			aDest[aRowLength] = byte;
			aSource += aSourceStride;
			aDest += aDestStride;
			}
		}
	else
		{
		for (VGint row = 0; row < aRowCount; ++row)
			{
			memcpy(aDest, aSource, aRowLength);
			aSource += aSourceStride;
			aDest += aDestStride;
			}
		}
	}


VGint CVgImageInfo::GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_IMAGE_FORMAT:
		case VG_IMAGE_WIDTH:
		case VG_IMAGE_HEIGHT:
			return 1;
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


VGfloat CVgImageInfo::GetParameterf(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_IMAGE_FORMAT:
			if (iFormat != VG_IMAGE_FORMAT_INVALID)
				{
				return (VGfloat)iFormat;
				}
			// for EglImage need to get format of underlying VgImage
			return HostVgGetParameterf(aVgContext, aParamType);

		case VG_IMAGE_WIDTH:
			return (VGfloat)iWidth;

		case VG_IMAGE_HEIGHT:
			return (VGfloat)iHeight;
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


VGint CVgImageInfo::GetParameteri(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_IMAGE_FORMAT:
			if (iFormat != VG_IMAGE_FORMAT_INVALID)
				{
				return iFormat;
				}
			// for EglImage need to get format of underlying VgImage
			iFormat = (VGImageFormat) HostVgGetParameteri(aVgContext, aParamType);
			return iFormat;

		case VG_IMAGE_WIDTH:
			return iWidth;

		case VG_IMAGE_HEIGHT:
			return iHeight;
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


void CVgImageInfo::ClearImage(MVgContext& aVgContext, VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	// **** Desirable: check aImage is not rendering target
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgClearImage, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aX);
	vgApiData.AppendParam(aY);
	vgApiData.AppendParam(aWidth);
	vgApiData.AppendParam(aHeight);
	TUint64 sgId(0L);
	if (iIsEglSibling)
		{
		sgId = iSgImageId;
		}
	vgApiData.AppendTUint64(sgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::ImageSubData(MVgContext& aVgContext, const void * aData, VGint aDataStride, VGImageFormat aDataFormat, VGint aX, VGint aY,
		VGint aWidth, VGint aHeight)
	{
	// **** Desirable: check image is not a rendering target

	// Limit aWidth & aHeight to the dimensions of aImage
	if (iWidth < aWidth)
		{
		aWidth = iWidth;
		}
	if (iHeight < aHeight)
		{
		aHeight = iHeight;
		}
	OPENVG_TRACE("CVgImageInfo::ImageSubData.1a dest Image: width=%d, height=%d; clipped width=%d, clipped height=%d",
			iWidth, iHeight, aWidth, aHeight);
	TInt bitsPerPixel = BitsPerPixelForVgImageFormat(aDataFormat);
	TUint32 lineLength = ((static_cast<TUint32>(aWidth) * static_cast<TUint32>(bitsPerPixel)) + 7) / 8;
	OPENVG_TRACE("CVgImageInfo::ImageSubData.1b bitsPerPixel=%d, lineLength=%d", bitsPerPixel, lineLength);

	if (bitsPerPixel <= 0)
		{
		aVgContext.SetVgError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
		}
	else
		{
		if (lineLength  == aDataStride)
			{ // use original parameters
			OPENVG_TRACE("CVgImageInfo::ImageSubData.2a: lineLength == dataStride");
			HostVgImageSubData(aVgContext, aData, aDataStride * aHeight, aDataStride,
					aDataFormat, aX, aY, aWidth, aHeight);
			}
		else if (0 == aDataStride)
			{ // Fill operation, pixmap size = lineLength
			OPENVG_TRACE("CVgImageInfo::ImageSubData.2b: 0 == dataStride");
			HostVgImageSubData(aVgContext, aData, lineLength, aDataStride,
					aDataFormat, aX, aY, aWidth, aHeight);
			}
		else
			{ // try to alloc a translation buffer - datastride maybe negative or simply > lineLength
			size_t pixmapSize = lineLength * aHeight;
			TUint8* localBuffer = (TUint8*) CVghwUtils::Alloc(pixmapSize);
			OPENVG_TRACE("CVgImageInfo::ImageSubData.2c: dataStride not 0 or lineLength, localBuffer=0x%x", localBuffer);
			if (localBuffer != NULL)
				{ // reformat data into temporary buffer
				PixmapBlit(localBuffer, static_cast<const TUint8*>(aData), lineLength, aDataStride, aHeight, lineLength);
				HostVgImageSubData(aVgContext, localBuffer, pixmapSize, lineLength, aDataFormat, aX, aY, aWidth, aHeight);
				CVghwUtils::Free(localBuffer);
				}
			else
				{ // alloc failed, so do VG operation row by row
				const TUint8* source = static_cast<const TUint8*>(aData);
				for (VGint row = 0; row < aHeight; ++row)
					{
					HostVgImageSubData(aVgContext, source, lineLength, lineLength, aDataFormat, aX + row, aY, aWidth, 1);
					source += aDataStride;
					}
				}
			}
		}
	}


void CVgImageInfo::HostVgImageSubData(MVgContext& aVgContext, const void* aPixmap, size_t aPixmapSize, VGint aHostDataStride,
		VGImageFormat aDataFormat, VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	VGPANIC_ASSERT(aPixmap != NULL, EVgPanicNullPixmapPointer);
	VGPANIC_ASSERT(aPixmapSize >= (aHostDataStride * aHeight), EVgPanicPixmapSizeError);

	RemoteFunctionCallData rfcdata; OpenVgRFC vgApiData(rfcdata);
	vgApiData.Init(OpenVgRFC::EvgImageSubData, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendVector(aPixmap, aPixmapSize);
	vgApiData.AppendParam(aHostDataStride);
	vgApiData.AppendParam(aDataFormat);
	vgApiData.AppendParam(aX);
	vgApiData.AppendParam(aY);
	vgApiData.AppendParam(aWidth);
	vgApiData.AppendParam(aHeight);
	TUint64 sgId(0L);
	if (iIsEglSibling)
		{
		sgId = iSgImageId;
		}
	OPENVG_TRACE("CVgImageInfo::HostVgImageSubData sgId 0x%lx", sgId);
	vgApiData.AppendTUint64(sgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::GetImageSubData(MVgContext& aVgContext, void * aData, VGint aDataStride, VGImageFormat aDataFormat, VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	// **** Desirable: check VGImage is not currently a rendering target

	// Limit aWidth & aHeight to the dimensions of aImage
	if (iWidth < aWidth)
		{
		aWidth = iWidth;
		}
	if (iHeight < aHeight)
		{
		aHeight = iHeight;
		}
	OPENVG_TRACE("CVgImageInfo::GetImageSubData.1a source Image: width=%d, height=%d; clipped width=%d, clipped height=%d",
			iWidth, iHeight, aWidth, aHeight);
	TInt bitsPerPixel = BitsPerPixelForVgImageFormat(aDataFormat);
	TUint32 lineLength = static_cast<TUint32>(aWidth) * static_cast<TUint32>(bitsPerPixel);
	TUint32 tailBits = lineLength & 7;
	lineLength = (lineLength + 7) / 8;
	OPENVG_TRACE("CVgImageInfo::GetImageSubData.1b bitsPerPixel=%d, lineLength=%d, tailBits=%d", bitsPerPixel, lineLength, tailBits);

	if (bitsPerPixel <= 0)
		{
		aVgContext.SetVgError(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
		}
	else
		{
		if (lineLength  == aDataStride)
			{ // use original params
			OPENVG_TRACE("CVgImageInfo::GetImageSubData.2a: lineLength == dataStride");
			HostVgGetImageSubData(aVgContext, aData, aDataStride * aHeight, aDataStride, aDataFormat, aX, aY, aWidth, aHeight);
			}
		else if (0 == aDataStride)
			{ // unlikely unless aHeight = 1, symmetric to fill function for vgGetImageSubData
			OPENVG_TRACE("CVgImageInfo::GetImageSubData.2b: 0 == dataStride");
			HostVgGetImageSubData(aVgContext, aData, lineLength, aDataStride, aDataFormat, aX, aY, aWidth, aHeight);
			}
		else
			{ // datastride maybe negative or simply > lineLength
			TInt pixmapSize = lineLength * aHeight;
			TUint8* localBuffer = (TUint8*) CVghwUtils::Alloc(pixmapSize);
			OPENVG_TRACE("CVgImageInfo::GetImageSubData.2c: dataStride not 0 or lineLength, localBuffer=0x%x", localBuffer);

			if (localBuffer != NULL)
				{ // read pixels into temporary buffer
				HostVgGetImageSubData(aVgContext, localBuffer, pixmapSize, lineLength, aDataFormat, aX, aY, aWidth, aHeight);
				// reformat into client memory
				PixmapBlit(static_cast<TUint8*>(aData), localBuffer, aDataStride, lineLength, aHeight, lineLength, tailBits);
				CVghwUtils::Free(localBuffer);
				}
			else
				{ // alloc failed, so do VG operation row by row
				TUint8* dest = static_cast<TUint8*>(aData);
				for (VGint row = 0; row < aHeight; ++row)
					{
					HostVgGetImageSubData(aVgContext, dest, lineLength, lineLength, aDataFormat, aX + row, aY, aWidth, 1);
					dest += aDataStride;
					}
				}
			}
		}
	}


void CVgImageInfo::HostVgGetImageSubData(MVgContext& aVgContext, void* aPixmap, size_t aPixmapSize, VGint aHostDataStride,
		VGImageFormat aDataFormat, VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	VGPANIC_ASSERT_DEBUG(aPixmap != NULL, EVgPanicNullPixmapPointer);
	VGPANIC_ASSERT_DEBUG(aPixmapSize >= (aHostDataStride * aHeight), EVgPanicPixmapSizeError);

	RemoteFunctionCallData rfcdata; OpenVgRFC vgApiData(rfcdata);
	vgApiData.Init(OpenVgRFC::EvgGetImageSubData);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendVector(aPixmap, aPixmapSize, RemoteFunctionCallData::EOut);
	vgApiData.AppendParam(aHostDataStride);
	vgApiData.AppendParam(aDataFormat);
	vgApiData.AppendParam(aX);
	vgApiData.AppendParam(aY);
	vgApiData.AppendParam(aWidth);
	vgApiData.AppendParam(aHeight);
	vgApiData.AppendParam((VGint)aPixmapSize); // ToDo overload AppendParam for TUint32
	TUint64 sgId(0L);
	if (iIsEglSibling)
		{
		sgId = iSgImageId;
		}
	vgApiData.AppendTUint64(sgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::CopyImage(MVgContext& aVgContext, VGint aDx, VGint aDy, CVgImageInfo& aSrcImageInfo, VGint aSx, VGint aSy, VGint aWidth,
		VGint aHeight, VGboolean aDither)
	{
	// **** Desirable: verify src & dst are not a rendering target, ...
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgCopyImage, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aDx);
	vgApiData.AppendParam(aDy);
	vgApiData.AppendParam(aSrcImageInfo.HostHandle());
	vgApiData.AppendParam(aSx);
	vgApiData.AppendParam(aSy);
	vgApiData.AppendParam(aWidth);
	vgApiData.AppendParam(aHeight);
	vgApiData.AppendParam(aDither);
	TUint64 dstSgId(0L);
	TUint64 srcSgId(0L);
	if (iIsEglSibling)
		{
		dstSgId = iSgImageId;
		}
	if (aSrcImageInfo.IsEglSibling())
		{
		srcSgId = aSrcImageInfo.SgImageId();
		}
	vgApiData.AppendTUint64(dstSgId);
	vgApiData.AppendTUint64(srcSgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::DrawImage(MVgContext& aVgContext)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgDrawImage, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	TUint64 sgId(0L);
	if (iIsEglSibling)
		{
		sgId = iSgImageId;
		}
	vgApiData.AppendTUint64(sgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::SetPixels(MVgContext& aVgContext, VGint aDx, VGint aDy, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight)
	{
	// **** Desirable: verify aSrc image is not a rendering target
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetPixels, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(aDx);
	vgApiData.AppendParam(aDy);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSx);
	vgApiData.AppendParam(aSy);
	vgApiData.AppendParam(aWidth);
	vgApiData.AppendParam(aHeight);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::GetPixels(MVgContext& aVgContext, VGint aDx, VGint aDy, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight)
	{
	// **** Desirable: verify aDst is not currently a rendering target
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGetPixels, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aDx);
	vgApiData.AppendParam(aDy);
	vgApiData.AppendParam(aSx);
	vgApiData.AppendParam(aSy);
	vgApiData.AppendParam(aWidth);
	vgApiData.AppendParam(aHeight);
	TUint64 sgId(0L);
	if (iIsEglSibling)
		{
		sgId = iSgImageId;
		}
	vgApiData.AppendTUint64(sgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::ColorMatrix(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, const VGfloat * aMatrix)
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgColorMatrix, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSrcImageInfo.HostHandle());
	vgApiData.AppendVector(aMatrix, KColorMatrixSize);
	TUint64 dstSgId(0L);
	TUint64 srcSgId(0L);
	if (IsEglSibling())
		{
		dstSgId = SgImageId();
		}
	if (aSrcImageInfo.IsEglSibling())
		{
		srcSgId = aSrcImageInfo.SgImageId();
		}
	vgApiData.AppendTUint64(dstSgId);
	vgApiData.AppendTUint64(srcSgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::Convolve(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, VGint aKernelWidth, VGint aKernelHeight,
		VGint aShiftX, VGint aShiftY, const VGshort * aKernel, VGfloat aScale, VGfloat aBias, VGTilingMode aTilingMode)
	{
	// **** Desirable: verify aDst & aSrc are valid and do not overlap
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgConvolve, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSrcImageInfo.HostHandle());
	vgApiData.AppendParam(aKernelWidth);
	vgApiData.AppendParam(aKernelHeight);
	vgApiData.AppendParam(aShiftX);
	vgApiData.AppendParam(aShiftY);
	vgApiData.AppendVector(aKernel, aKernelWidth*aKernelHeight);
	vgApiData.AppendParam(aScale);
	vgApiData.AppendParam(aBias);
	vgApiData.AppendParam(aTilingMode);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::SeparableConvolve(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, VGint aKernelWidth, VGint aKernelHeight,
		VGint aShiftX, VGint aShiftY, const VGshort * aKernelX, const VGshort * aKernelY, 
		VGfloat aScale, VGfloat aBias, VGTilingMode aTilingMode)
	{
	// **** Desirable: verify aDst & aSrc are valid and do not overlap
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSeparableConvolve, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSrcImageInfo.HostHandle());
	vgApiData.AppendParam(aKernelWidth);
	vgApiData.AppendParam(aKernelHeight);
	vgApiData.AppendParam(aShiftX);
	vgApiData.AppendParam(aShiftY);
	vgApiData.AppendVector(aKernelX, aKernelWidth);
	vgApiData.AppendVector(aKernelY, aKernelHeight);
	vgApiData.AppendParam(aScale);
	vgApiData.AppendParam(aBias);
	vgApiData.AppendParam(aTilingMode);
	TUint64 dstSgId(0L);
	TUint64 srcSgId(0L);
	if (IsEglSibling())
		{
		dstSgId = SgImageId();
		}
	if (aSrcImageInfo.IsEglSibling())
		{
		srcSgId = aSrcImageInfo.SgImageId();
		}
	vgApiData.AppendTUint64(dstSgId);
	vgApiData.AppendTUint64(srcSgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::GaussianBlur(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, VGfloat aStdDeviationX, VGfloat aStdDeviationY, VGTilingMode aTilingMode)
	{
	// **** Desirable: verify aDst & src are not currently a rendering target or overlap
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgGaussianBlur, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSrcImageInfo.HostHandle());
	vgApiData.AppendParam(aStdDeviationX);
	vgApiData.AppendParam(aStdDeviationY);
	vgApiData.AppendParam(aTilingMode);
	TUint64 dstSgId(0L);
	TUint64 srcSgId(0L);
	if (IsEglSibling())
		{
		dstSgId = SgImageId();
		}
	if (aSrcImageInfo.IsEglSibling())
		{
		srcSgId = aSrcImageInfo.SgImageId();
		}
	vgApiData.AppendTUint64(dstSgId);
	vgApiData.AppendTUint64(srcSgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::Lookup(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, const VGubyte * aRedLUT, const VGubyte * aGreenLUT, const VGubyte * aBlueLUT,
		const VGubyte * aAlphaLUT, VGboolean aOutputLinear, VGboolean aOutputPremultiplied)
	{
	// **** Desirable: verify aDst & aSrc are not currently a rendering target or overlap
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgLookup, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSrcImageInfo.HostHandle());
	vgApiData.AppendVector(aRedLUT, KLUTLength);
	vgApiData.AppendVector(aGreenLUT, KLUTLength);
	vgApiData.AppendVector(aBlueLUT, KLUTLength);
	vgApiData.AppendVector(aAlphaLUT, KLUTLength);
	vgApiData.AppendParam(aOutputLinear);
	vgApiData.AppendParam(aOutputPremultiplied);
	TUint64 dstSgId(0L);
	TUint64 srcSgId(0L);
	if (IsEglSibling())
		{
		dstSgId = SgImageId();
		}
	if (aSrcImageInfo.IsEglSibling())
		{
		srcSgId = aSrcImageInfo.SgImageId();
		}
	vgApiData.AppendTUint64(dstSgId);
	vgApiData.AppendTUint64(srcSgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgImageInfo::LookupSingle(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, const VGuint * aLookupTable, VGImageChannel aSourceChannel,
		VGboolean aOutputLinear, VGboolean aOutputPremultiplied)
	{
	// **** Desirable: check aSrc is in an RGB pixel format, and that aSourceChannel is okay
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgLookupSingle, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aSrcImageInfo.HostHandle());
	vgApiData.AppendVector(aLookupTable, KLUTLength);
	vgApiData.AppendParam(aSourceChannel);
	vgApiData.AppendParam(aOutputLinear);
	vgApiData.AppendParam(aOutputPremultiplied);
	TUint64 dstSgId(0L);
	TUint64 srcSgId(0L);
	if (IsEglSibling())
		{
		dstSgId = SgImageId();
		}
	if (aSrcImageInfo.IsEglSibling())
		{
		srcSgId = aSrcImageInfo.SgImageId();
		}
	vgApiData.AppendTUint64(dstSgId);
	vgApiData.AppendTUint64(srcSgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


// end of file vgimage.cpp
