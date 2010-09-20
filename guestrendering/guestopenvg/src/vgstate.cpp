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
// Client-side state information for Symbian DLL

#include "vgstate.h"
#include "remotefunctioncall.h"
#include "openvgrfc.h"



// VG Client-side state is Writeable Static Data in the DLL, (holds state information for the process)
// Constructor/Destructor called on process load/unload to perform initialiser/release of static resources
XOpenVgState OpenVgState;



_LIT(KVgPanicCategory, "Guest VG");

void VgPanic(TVgPanic aPanicCode, char* aPanicName, char* aCondition, char* aFile, TInt aLine)
	{
	if (aPanicName && aCondition && aFile)
		{ // normal for Debug builds
		RDebug::Printf("Guest Open VG DLL Panic %s for failed Assert (%s),\n\tat %s:%d", aPanicName, aCondition, aFile, aLine);
		}
	else if (aPanicName && aFile)
		{ // Debug builds Assert Always
		RDebug::Printf("Guest Open VG DLL Panic %s at %s:%d", aPanicName, aFile, aLine);
		}
	else
		{ // normal for Release builds
		RDebug::Printf("Guest Open VG DLL Panic %d (line %d)", aPanicCode, aLine);
		}

	User::Panic(KVgPanicCategory, aPanicCode);
	}



/////////////////////////////////////////////////////////////////////////////////////////////
// TCheck - parameter checking utility functions
/////////////////////////////////////////////////////////////////////////////////////////////


/*
 aWidth & aHeight parameters must be >0, also ax, ay, aX + aWidth and aY + aHeight must all
 be within image size, otherwise a VG_ILLEGAL_ARGUMENT_ERROR is flagged.
 */
TBool TCheck::ChkAreaIsWithinImage(MVgContext& aVgContext, CVgImageBase* aImageInfo, VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	VGPANIC_ASSERT_DEBUG(aImageInfo, EVgPanicTemp);
	if ( (aX >= 0) && (aY >= 0) && (aWidth > 0) && (aHeight > 0) )
		{
		VGint maxX = aWidth + aX;
		VGint maxY = aHeight + aY;
		if ( (maxX > 0) && (maxY > 0) && (aImageInfo->Width() >= maxX) && (aImageInfo->Height() >= maxY) )
			{
			return ETrue;
			}
		}
	OPENVG_TRACE("TCheck::ChkAreaIsWithinImage setting VG_ILLEGAL_ARGUMENT_ERROR");
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return EFalse;
	}


VGint TCheck::ImageFormatByteDepth(VGImageFormat aImageFormat)
	{
	switch (aImageFormat)
		{
		case VG_sRGBX_8888:
		case VG_sRGBA_8888:
		case VG_sRGBA_8888_PRE:
		case VG_lRGBX_8888:
		case VG_lRGBA_8888:
		case VG_lRGBA_8888_PRE:
		case VG_sXRGB_8888:
		case VG_sARGB_8888:
		case VG_sARGB_8888_PRE:
		case VG_lXRGB_8888:
		case VG_lARGB_8888:
		case VG_lARGB_8888_PRE:
		case VG_sBGRX_8888:
		case VG_sBGRA_8888:
		case VG_sBGRA_8888_PRE:
		case VG_lBGRX_8888:
		case VG_lBGRA_8888:
		case VG_lBGRA_8888_PRE:
		case VG_sXBGR_8888:
		case VG_sABGR_8888:
		case VG_sABGR_8888_PRE:
		case VG_lXBGR_8888:
		case VG_lABGR_8888:
		case VG_lABGR_8888_PRE:
			return 4;
		case VG_sRGB_565:
		case VG_sRGBA_5551:
		case VG_sRGBA_4444:
		case VG_sARGB_1555:
		case VG_sARGB_4444:
		case VG_sBGR_565:
		case VG_sBGRA_5551:
		case VG_sBGRA_4444:
		case VG_sABGR_1555:
		case VG_sABGR_4444:
			return 2;
		case VG_sL_8:
		case VG_lL_8:
		case VG_A_8:
		case VG_A_4:
		case VG_A_1:
		case VG_BW_1:
			return 1;
		case VG_IMAGE_FORMAT_INVALID:
		default:
			return 0;
		}
	}


// Checks aCount & aValues for SetParameteriv & SetParameterfv
TBool TCheck::ChkParamCountAndValuesPtr(MVgContext& aVgContext, VGint aCount, const void* aValues)
	{
	VGErrorCode error = VG_NO_ERROR;
	if (aValues == NULL)
		{
		if (aCount != 0)
			{
			error = VG_ILLEGAL_ARGUMENT_ERROR;
			}
		}
	// we should check count for not being too large for serialization, but
	// there is no limit in spec for VG_SCISSOR_RECTS and VG_STROKE_DASH_PATTERN
	else if ( (3ul & (unsigned)aValues) || (aCount < 0) || (aCount > 100000) )
		{
		error = VG_ILLEGAL_ARGUMENT_ERROR;
		}

	if (error != VG_NO_ERROR)
		{
		OPENVG_TRACE("TCheck::ChkParamCountAndValuesPtr setting error=0x%x", error);
		aVgContext.SetVgError(error);
		return EFalse;
		}
	return ETrue;
	}


/////////////////////////////////////////////////////////////////////////////////////////////
// XOpenVgState
/////////////////////////////////////////////////////////////////////////////////////////////

// Singleton object is in WSD memory, so this Constructor is called when the DLL is loaded
XOpenVgState::XOpenVgState() :
		iEglManagementApi(NULL), iKVgMaxKernelSize(0), iKVgMaxSeparableKernelSize(0)
	{
	OPENVG_TRACE("XOpenVgState::XOpenVgState 1. start Process=0x%lx, Thread=0x%lx", RProcess().Id().Id(), RThread().Id().Id());

	TInt createErr1 = iStateLock.CreateLocal(EOwnerProcess);
	OPENVG_TRACE("XOpenVgState::XOpenVgState 2. iStateLock.CreateLocal error=%d, Handle=0x%x", createErr1, iStateLock.Handle());

	TInt createErr2 = iMapLock.CreateLocal(EOwnerProcess);
	OPENVG_TRACE("XOpenVgState::XOpenVgState 2. iMapLock.CreateLocal error=%d, Handle=0x%x", createErr2, iMapLock.Handle());
	// cannot continue if there is an error, so Panic
	VGPANIC_ASSERT( (createErr1 == KErrNone) && (createErr2 == KErrNone), EVgPanicTemp);

	CVgHandleBase::InitStatics();

	// publish MVgApiforEgl vtable
	CVghwUtils::SetVgApiForEgl(this);
	}


// Desstructor is called when the DLL is unloaded
XOpenVgState::~XOpenVgState()
	{
	OPENVG_TRACE("XOpenVgState::~XOpenVgState 1. iStateLock handle=0x%x, iMapLock handle=0x%x, Process=0x%lx, Thread=0x%lx",
			iStateLock.Handle(), iMapLock.Handle(), RProcess().Id().Id(), RThread().Id().Id());

	if (iStateLock.Handle())
		{
		iStateLock.Close();
		OPENVG_TRACE("XOpenVgState::~XOpenVgState 2. iStateLock Handle=0x%x", iStateLock.Handle());
		}
	if (iMapLock.Handle())
		{
		iMapLock.Close();
		OPENVG_TRACE("XOpenVgState::~XOpenVgState 3. iMapLock Handle=0x%x", iMapLock.Handle());
		}

	// unpublish MVgApiforEgl vtable
	CVghwUtils::SetVgApiForEgl(NULL);
	}


TBool XOpenVgState::CheckVGHandle(MVgContext& aVgContext, VGHandle aHandle, CVgHandleBase** aHandleInfo, TVgHandleType aMatchType)
	{ // client side VGHandle is an opaque data handle to Host Open VG objects such as VGPath or VGImage
	// only check for logic errors in VG code in Debug builds
	VGPANIC_ASSERT_DEBUG(iStateLock.IsHeld(), EVgPanicStateLockMutexNotHeld);
	VGPANIC_ASSERT_DEBUG(aHandleInfo, EVgPanicFunctionParameterIsNull);

	if (aHandle != VG_INVALID_HANDLE)
		{
		iMapLock.Wait();
		CVgHandleBase** pHandleInfo = iHandleHashMap.Find(aHandle);
		if (pHandleInfo)
			{
			CVgHandleBase* handleInfo = *pHandleInfo;
			if (handleInfo && !handleInfo->IsDestroyed() &&
					( (aMatchType == EVgHandleAny) || (aMatchType == handleInfo->HandleType()) ) )
				{
				// ToDo confirm handle belongs to this context or is shared with this context
				*aHandleInfo = handleInfo;
				OPENVG_TRACE("  XOpenVgState::CheckVGHandle client handle=0x%x, match type=%d; found matching Host VG Handle=0x%x",
						aHandle, aMatchType, handleInfo->HostHandle());
				iMapLock.Signal();
				return ETrue;
				}
			}
		iMapLock.Signal();
		}
	OPENVG_TRACE("  XOpenVgState::CheckVGHandle client handle=0x%x, match type=%d, setting VG_BAD_HANDLE_ERROR", aHandle, aMatchType);
	aVgContext.SetVgError(VG_BAD_HANDLE_ERROR);
	return EFalse;
	}


/*
 aKernelWidth or aKernelHeight must be >0 and <= [Size Limit], otherwise a VG_ILLEGAL_ARGUMENT_ERROR is flagged
 */
TBool XOpenVgState::CheckKernelWidthAndHeight(MVgContext& aVgContext, VGint aKernelWidth, VGint aKernelHeight, VGParamType aLimitType)
	{
	VGPANIC_ASSERT_DEBUG(iStateLock.IsHeld(), EVgPanicStateLockMutexNotHeld);
	VGint limit = 0;
	// aLimitType must be either VG_MAX_KERNEL_SIZE or VG_MAX_SEPARABLE_KERNEL_SIZE
	if (aLimitType == VG_MAX_KERNEL_SIZE)
		{
		if (iKVgMaxKernelSize == 0)
			{ // first access, so fetch value from Host
			iKVgMaxKernelSize = TGuestOpenVg::HostGeti(aVgContext, VG_MAX_KERNEL_SIZE);
			}
		limit = iKVgMaxKernelSize;
		}
	else if (aLimitType == VG_MAX_SEPARABLE_KERNEL_SIZE)
		{
		if (iKVgMaxSeparableKernelSize == 0)
			{ // first access, so fetch value from Host
			iKVgMaxSeparableKernelSize = TGuestOpenVg::HostGeti(aVgContext, VG_MAX_SEPARABLE_KERNEL_SIZE);
			}
		limit = iKVgMaxSeparableKernelSize;
		}
	VGPANIC_ASSERT(limit > 0, EVgPanicValueFromHostVgIsInvalid);

	if ( (aKernelHeight > 0) && (aKernelHeight <= limit) && (aKernelWidth > 0) && (aKernelWidth <= limit) )
		{
		return ETrue;
		}
	OPENVG_TRACE("XOpenVgState::CheckKernelWidthAndHeight setting VG_ILLEGAL_ARGUMENT_ERROR");
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return EFalse;
	}


TBool XOpenVgState::AddToHashMap(MVgContext& aVgContext, CVgHandleBase* aHandleInfo, VGHandle aHostHandle)
	{
	VGPANIC_ASSERT_DEBUG(iStateLock.IsHeld(), EVgPanicStateLockMutexNotHeld);

	OPENVG_TRACE("  XOpenVgState::AddToHashMap aHandleInfo=0x%x, aHostHandle=0x%x, ClientHandle=0x%x",
			aHandleInfo, aHostHandle, aHandleInfo->ClientHandle());
	aHandleInfo->SetHostHandle(aHostHandle);

	iMapLock.Wait();
	RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
	// ClientHandle is key to finding object, its Host Handle, object type, etc...
	TInt err = iHandleHashMap.Insert(aHandleInfo->ClientHandle(), aHandleInfo);
	CVghwUtils::SwitchFromVghwHeap(clientHeap);
	iMapLock.Signal();

	if (err == KErrNone)
		{
		return ETrue;
		}

	// Failed, Cleanup ...
	OPENVG_TRACE("  XOpenVgState::AddToHashMap insert error = %d", err);
	aVgContext.SetVgError(VG_OUT_OF_MEMORY_ERROR);
	return EFalse;
	}


void XOpenVgState::UnMapHandle(TUint32 aClientHandle)
	{
	OPENVG_TRACE("  XOpenVgState::UnMapHandle aHandleInfo=0x%x -->", aClientHandle);
	VGPANIC_ASSERT_DEBUG(iStateLock.IsHeld(), EVgPanicStateLockMutexNotHeld);
	VGPANIC_ASSERT_DEBUG( CVghwUtils::UsingVghwHeap(), EVgPanicTemp);

	iMapLock.Wait();
#ifdef _DEBUG
	TInt dbgErr =
#endif
			iHandleHashMap.Remove(aClientHandle);
	iMapLock.Signal();

	OPENVG_TRACE("  XOpenVgState::UnMapHandle ... dbgErr =%d <--", dbgErr);
	}


TBool XOpenVgState::EglImageOpenForVgImage(EGLImageKHR aImage, TSize& aSize, VGHandle& aVgHandle, TUint64& aSgImageId)
	{
	if (!iEglManagementApi)
		{ // try to get EGL interface via VghwUtils
		iEglManagementApi = CVghwUtils::EglManagementApi();
		VGPANIC_ASSERT_DEBUG(iEglManagementApi, EVgPanicMissingEglInterface);
		}
	if (iEglManagementApi)
		{
		return iEglManagementApi->EglImageOpenForVgImage(aImage, aSize, aVgHandle, aSgImageId);
		}
	return EFalse;
	}


void XOpenVgState::EglImageClose(EGLImageKHR aImage)
	{
	if (iEglManagementApi)
		{
		iEglManagementApi->EglImageClose(aImage);
		}
	}


// Guest Open VG extension export support for eglGetProcAddress
typedef struct
	{
	const char*          procName;		// procedure name
	ExtensionProcPointer procAddr;
	} TVgExtnInfo;

// VG extension functions
static const TVgExtnInfo vgProcedures[] =
	{
		{ "vgCreateEGLImageTargetKHR", (ExtensionProcPointer)vgCreateEGLImageTargetKHR },
	};

const TInt KVgProcCount = sizeof(vgProcedures) / sizeof(TVgExtnInfo);

ExtensionProcPointer XOpenVgState::guestGetVgProcAddress (const char *aProcName)
	{
	// exhaustive search
	for (TInt idx = 0; idx < KVgProcCount; idx++)
	  {
	  if (!strcmp(aProcName, vgProcedures[idx].procName))
		  return vgProcedures[idx].procAddr;
	  }
	return NULL;
	}


/////////////////////////////////////////////////////////////////////////////////////////////
// TCleanupVgLocks
/////////////////////////////////////////////////////////////////////////////////////////////

TCleanupVgLocks::TCleanupVgLocks(MVgContext& aVgContext) :
		iVgContext(aVgContext), iMutex(OpenVgState.MutexWait()), iIsHeld(ETrue)
	{}

TCleanupVgLocks::~TCleanupVgLocks()
	{
	if (iIsHeld)
		{
		SignalMutex();
		}
	}

void TCleanupVgLocks::SignalMutex()
	{
	VGPANIC_ASSERT_DEBUG(iIsHeld && iMutex.IsHeld(), EVgPanicTemp);
	iMutex.Signal();
	iIsHeld = EFalse;
	}


/*
 Returns false & sets VG error to VG_ILLEGAL_ARGUMENT_ERROR if VGMaskOperation is not a supported operation
 */
TBool TCleanupVgLocks::CheckVGMaskOperationAndHandle(VGMaskOperation aOperation, VGHandle aMask, CVgImageBase** aHandleInfo)
	{
	VGPANIC_ASSERT_DEBUG(iIsHeld, EVgPanicStateLockMutexNotHeld);
	*aHandleInfo = NULL;
	switch (aOperation)
		{
		case VG_CLEAR_MASK:
		case VG_FILL_MASK:
			// aMask is not used for these operations
			return ETrue;
		case VG_SET_MASK:
		case VG_UNION_MASK:
		case VG_INTERSECT_MASK:
		case VG_SUBTRACT_MASK:
			{
			if (aMask == VG_INVALID_HANDLE)
				return ETrue;

			CVgHandleBase* tempInfo;
			if (CheckVGAnyHandle(aMask, &tempInfo))
				{
				if ( (tempInfo->HandleType() == EVgHandleForMaskLayer) || (tempInfo->HandleType() == EVgHandleForImage) )
					{
					*aHandleInfo = (CVgImageBase*)tempInfo;
					// TODO if handle is a VgImage verify that it is not a rendering target
					return ETrue;
					}
				OPENVG_TRACE("TCleanupVgLocks::CheckVGMaskOperationAndHandle setting VG_BAD_HANDLE_ERROR");
				iVgContext.SetVgError(VG_BAD_HANDLE_ERROR);
				}
			return EFalse;
			}
		default:
			OPENVG_TRACE("TCleanupVgLocks::CheckVGMaskOperationAndHandle setting VG_ILLEGAL_ARGUMENT_ERROR");
			iVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
			return EFalse;
		}
	}


VGFont TCleanupVgLocks::CreateFont(VGint aGlyphCapacityHint)
	{
	VGPANIC_ASSERT_DEBUG(iIsHeld && iMutex.IsHeld(), EVgPanicTemp);

	CVgFontInfo* fontInfo = CVgFontInfo::New();
	if (fontInfo == NULL)
		{
		OPENVG_TRACE("  TGuestOpenVg::vgCreateFont - CVgFontInfo::New() failed");
		iVgContext.SetVgError(VG_OUT_OF_MEMORY_ERROR);
		}
	else
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgCreateFont);
		vgApiData.AppendParam(aGlyphCapacityHint);
		VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
		iVgContext.ExecuteVgCommand(vgApiData);
		VGFont hostFont = static_cast<VGFont>(vgApiData.ReturnValue());
		OPENVG_TRACE("  TCleanupVgLocks::CreateFont - CVgFontInfo::New() success, hostFont=0x%x", hostFont);

		if ( (hostFont != VG_INVALID_HANDLE) && OpenVgState.AddToHashMap(iVgContext, fontInfo, hostFont) )
			{
			return fontInfo->ClientHandle();
			}

		fontInfo->Destroy(iVgContext);
		}

	return VG_INVALID_HANDLE;
	}


VGImage TCleanupVgLocks::ChildImage(CVgImageInfo& aParentInfo, VGint aX, VGint aY, VGint aWidth, VGint aHeight)
	{
	CVgImageInfo* imageInfo = CVgImageInfo::New(aWidth, aHeight, &aParentInfo);
	if (imageInfo == NULL)
		{
		iVgContext.SetVgError(VG_OUT_OF_MEMORY_ERROR);
		OPENVG_TRACE("  TCleanupVgLocks::ChildImage - CVgImageInfo::New() failed");
		}
	else
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgChildImage);
		vgApiData.AppendParam(aParentInfo.HostHandle());
		vgApiData.AppendParam(aX);
		vgApiData.AppendParam(aY);
		vgApiData.AppendParam(aWidth);
		vgApiData.AppendParam(aHeight);
		VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
		iVgContext.ExecuteVgCommand(vgApiData);
		VGImage hostImageHandle = static_cast<VGImage>(vgApiData.ReturnValue());

		OPENVG_TRACE("  TCleanupVgLocks::ChildImage - CVgImageInfo::New() success, hostImageHandle=0x%x", hostImageHandle);
		if ( (hostImageHandle != VG_INVALID_HANDLE) && OpenVgState.AddToHashMap(iVgContext, imageInfo, hostImageHandle) )
			{
			return imageInfo->ClientHandle();
			}
		imageInfo->Destroy(iVgContext);
		}

	return VG_INVALID_HANDLE;
	}


VGImage TCleanupVgLocks::CreateImage(VGImageFormat aFormat, VGint aWidth, VGint aHeight, VGbitfield aAllowedQuality)
	{
	CVgImageInfo* imageInfo = CVgImageInfo::New(aFormat, aWidth, aHeight, aAllowedQuality);
	if (imageInfo == NULL)
		{
		iVgContext.SetVgError(VG_OUT_OF_MEMORY_ERROR);
		OPENVG_TRACE("  TCleanupVgLocks::CreateImage - CVgImageInfo::New() failed");
		}
	else
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgCreateImage);
		vgApiData.AppendParam(aFormat);
		vgApiData.AppendParam(aWidth);
		vgApiData.AppendParam(aHeight);
		vgApiData.AppendParam(aAllowedQuality);
		VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
		iVgContext.ExecuteVgCommand(vgApiData);
		VGImage hostImageHandle = static_cast<VGImage>(vgApiData.ReturnValue());

		OPENVG_TRACE("  TCleanupVgLocks::CreateImage - CVgImageInfo::New() success, hostImageHandle=0x%x", hostImageHandle);
		if ( (hostImageHandle != VG_INVALID_HANDLE) && OpenVgState.AddToHashMap(iVgContext, imageInfo, hostImageHandle) )
			{
			return imageInfo->ClientHandle();
			}
		imageInfo->Destroy(iVgContext);
		}

	return VG_INVALID_HANDLE;
	}


VGMaskLayer TCleanupVgLocks::CreateMaskLayer(VGint aWidth, VGint aHeight)
	{
	CVgMaskLayerInfo* maskLayerInfo = CVgMaskLayerInfo::New(aWidth, aHeight);
	if (maskLayerInfo == NULL)
		{
		OPENVG_TRACE("  TCleanupVgLocks::CreateMaskLayer - CVgMaskLayerInfo::New() failed");
		iVgContext.SetVgError(VG_OUT_OF_MEMORY_ERROR);
		}
	else
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgCreateMaskLayer);
		vgApiData.AppendParam(aWidth);
		vgApiData.AppendParam(aHeight);
		VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
		iVgContext.ExecuteVgCommand(vgApiData);
		VGMaskLayer hostMaskHandle = static_cast<VGMaskLayer>(vgApiData.ReturnValue());

		OPENVG_TRACE("  TCleanupVgLocks::CreateMaskLayer - CVgMaskLayerInfo::New() success, hostMaskHandle=0x%x", hostMaskHandle);
		if ( (hostMaskHandle != VG_INVALID_HANDLE) && OpenVgState.AddToHashMap(iVgContext, maskLayerInfo, hostMaskHandle) )
			{
			return maskLayerInfo->ClientHandle();
			}
		maskLayerInfo->Destroy(iVgContext);
		}

	return VG_INVALID_HANDLE;
	}


VGPaint TCleanupVgLocks::CreatePaint()
	{
	CVgPaintInfo* paintInfo = CVgPaintInfo::New();
	if (paintInfo == NULL)
		{
		OPENVG_TRACE("  TCleanupVgLocks::CreatePaint - CVgPaintInfo::New() failed");
		iVgContext.SetVgError(VG_OUT_OF_MEMORY_ERROR);
		}
	else
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgCreatePaint);
		VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
		iVgContext.ExecuteVgCommand(vgApiData);
		VGPaint hostPaint = static_cast<VGPaint>(vgApiData.ReturnValue());

		OPENVG_TRACE("  TCleanupVgLocks::CreatePaint - CVgPaintInfo::New() success, clientHandle=0x%x, hostHandle=0x%x",
				paintInfo->ClientHandle(), hostPaint);
		if ( (hostPaint != VG_INVALID_HANDLE) && OpenVgState.AddToHashMap(iVgContext, paintInfo, hostPaint) )
			{
			return paintInfo->ClientHandle();
			}
		paintInfo->Destroy(iVgContext);
		}

	return VG_INVALID_HANDLE;
	}


VGPaint TCleanupVgLocks::GetPaint(VGPaintMode aPaintMode)
	{
	CVgPaintInfo* paintInfo = CVgPaintInfo::New();
	if (paintInfo == NULL)
		{
		iVgContext.SetVgError(VG_OUT_OF_MEMORY_ERROR);
		OPENVG_TRACE("  TCleanupVgLocks::GetPaint - CVgPaintInfo::New() failed");
		}
	else
		{
		RemoteFunctionCallData rfcdata; OpenVgRFC vgApiData(rfcdata);
		vgApiData.Init(OpenVgRFC::EvgGetPaint);
		vgApiData.AppendParam(aPaintMode);
		VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
		iVgContext.ExecuteVgCommand(vgApiData);
		VGPaint hostPaint = static_cast<VGPaint>(vgApiData.ReturnValue());

		OPENVG_TRACE("  TCleanupVgLocks::GetPaint - CVgPaintInfo::New() success, clientHandle=0x%x, hostHandle=0x%x",
				paintInfo->ClientHandle(), hostPaint);
		if ( (hostPaint != VG_INVALID_HANDLE) && OpenVgState.AddToHashMap(iVgContext, paintInfo, hostPaint) )
			{
			return paintInfo->ClientHandle();
			}
		paintInfo->Destroy(iVgContext);
		}

	return VG_INVALID_HANDLE;
	}


VGPath TCleanupVgLocks::CreatePath(VGint aPathFormat, VGPathDatatype aDatatype, VGfloat aScale, VGfloat aBias,
		VGint aSegmentCapacityHint, VGint aCoordCapacityHint, VGbitfield aCapabilities)
	{
	VGPANIC_ASSERT_DEBUG(iIsHeld && iMutex.IsHeld(), EVgPanicTemp);

	aCapabilities &= VG_PATH_CAPABILITY_ALL;
	CVgPathInfo* pathInfo = CVgPathInfo::New(aDatatype, aScale, aBias, aCapabilities);
	if (pathInfo == NULL)
		{
		OPENVG_TRACE("  TCleanupVgLocks::CreatePath - CVgPathInfo::New() failed");
		iVgContext.SetVgError(VG_OUT_OF_MEMORY_ERROR);
		}
	else
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgCreatePath);
		vgApiData.AppendParam(aPathFormat);
		vgApiData.AppendParam(aDatatype);
		vgApiData.AppendParam(aScale);
		vgApiData.AppendParam(aBias);
		vgApiData.AppendParam(aSegmentCapacityHint);
		vgApiData.AppendParam(aCoordCapacityHint);
		vgApiData.AppendParam(aCapabilities);
		VGPANIC_ASSERT_DEBUG(vgApiData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EVgPanicNotReplyOpcode);
		iVgContext.ExecuteVgCommand(vgApiData);
		VGPath hostPath = static_cast<VGPath>(vgApiData.ReturnValue());
		OPENVG_TRACE("  TCleanupVgLocks::CreatePath - CVgPathInfo::New() success, hostHandle=0x%x", hostPath);

		if ( (hostPath != VG_INVALID_HANDLE) && OpenVgState.AddToHashMap(iVgContext, pathInfo, hostPath) )
			{
			return pathInfo->ClientHandle();
			}

		pathInfo->Destroy(iVgContext);
		}
	
	return VG_INVALID_HANDLE;
	}


VGImage TCleanupVgLocks::CreateEGLImageTargetKHR(VGeglImageKHR aImage)
	{
	VGErrorCode error = VG_NO_ERROR;

	if (aImage == NULL)
		{
		error = VG_ILLEGAL_ARGUMENT_ERROR;
		}
	else
		{
		// Try to open EGL Image from handle, and get info such as image size
		TSize imageSize;
		TUint64 sgImageId;
		VGHandle vgHandle;
		if (!OpenVgState.EglImageOpenForVgImage((EGLImageKHR) aImage, imageSize, vgHandle, sgImageId))
			{
			error = VG_UNSUPPORTED_IMAGE_FORMAT_ERROR;
			}
		else
			{ // Open success, create a CVgImageInfo with all the details ...
			// ToDo get & store VGImageFormat of underlying VGImage
			CVgImageInfo* newImageInfo = CVgImageInfo::New(VG_IMAGE_FORMAT_INVALID, imageSize.iWidth, imageSize.iHeight,
					(EGLImageKHR) aImage, sgImageId);
			OPENVG_TRACE("TGuestOpenVg::vgCreateEGLImageTargetKHR imageSize=%d,%d, vgHandle=0x%x, sgImageId=0x%lx  newImageInfo=0x%x",
					imageSize.iWidth, imageSize.iHeight, vgHandle, sgImageId, newImageInfo);
			if (newImageInfo == NULL)
				{
				error = VG_OUT_OF_MEMORY_ERROR;
				OpenVgState.EglImageClose((EGLImageKHR) aImage);
				}
			else
				{
				if (OpenVgState.AddToHashMap(iVgContext, newImageInfo, vgHandle))
					{
					// Notify Command Scheduler & KhronosWrappers
					RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
					vgApiData.Init(OpenVgRFC::EvgCreateEGLImageTargetKHR, RemoteFunctionCallData::EOpRequestWithReply);
					vgApiData.AppendParam(aImage);
					// ToDo any other parameters needed? Is this really a Request with Reply?
					iVgContext.ExecuteVgCommand(vgApiData);
					return newImageInfo->ClientHandle();  
					}
				newImageInfo->Destroy(iVgContext);
				}
			}
		}


	if (error != VG_NO_ERROR)
		{
		iVgContext.SetVgError(error);
		OPENVG_TRACE("TGuestOpenVg::vgCreateEGLImageTargetKHR fail - error=0x%x", error);
		}

	return VG_INVALID_HANDLE;
	}




// **** Desirable: could check VGParamType for vgGet & vgSet scalar & vector operations.
/*
  // Mode settings
  VG_MATRIX_MODE  ???
  VG_FILL_RULE  ???
  VG_IMAGE_QUALITY  ???
  VG_RENDERING_QUALITY  ???
  VG_BLEND_MODE  ???
  VG_IMAGE_MODE  ???

  // Scissoring rectangles
  VG_SCISSOR_RECTS  ???

  // Color Transformation
  VG_COLOR_TRANSFORM  ???
  VG_COLOR_TRANSFORM_VALUES  ???

  // Stroke parameters
  VG_STROKE_LINE_WIDTH  ???
  VG_STROKE_CAP_STYLE  ???
  VG_STROKE_JOIN_STYLE  ???
  VG_STROKE_MITER_LIMIT  ???
  VG_STROKE_DASH_PATTERN  ???
  VG_STROKE_DASH_PHASE  ???
  VG_STROKE_DASH_PHASE_RESET  ???

  // Edge fill color for VG_TILE_FILL tiling mode
  VG_TILE_FILL_COLOR  ???

  // Color for vgClear
  VG_CLEAR_COLOR  ???

  // Glyph origin
  VG_GLYPH_ORIGIN  ???

  // Enable/disable alpha masking and scissoring
  VG_MASKING  ???
  VG_SCISSORING  ???

  // Pixel layout information
  VG_PIXEL_LAYOUT  ???
  VG_SCREEN_LAYOUT  ???

  // Source format selection for image filters
  VG_FILTER_FORMAT_LINEAR  ???
  VG_FILTER_FORMAT_PREMULTIPLIED  ???

  // Destination write enable mask for image filters
  VG_FILTER_CHANNEL_MASK  ???

  // Implementation limits (read-only)
  VG_MAX_SCISSOR_RECTS, ReadOnly, Scalar
  VG_MAX_DASH_COUNT  ???
  VG_MAX_KERNEL_SIZE  ???
  VG_MAX_SEPARABLE_KERNEL_SIZE  ???
  VG_MAX_COLOR_RAMP_STOPS  ???
  VG_MAX_IMAGE_WIDTH  ???
  VG_MAX_IMAGE_HEIGHT  ???
  VG_MAX_IMAGE_PIXELS  ???
  VG_MAX_IMAGE_BYTES  ???
  VG_MAX_FLOAT  ???
  VG_MAX_GAUSSIAN_STD_DEVIATION  ???
  */

//////////////////////////////////////////////////////////////////////////

// end of file vgstate.cpp
