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
// Implementation of SgImage functions exported through eglGetProcAddress


#include "eglapi.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// CEgLImage
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef FAISALMEMON_S4_SGIMAGE
CEglImage::CEglImage() : iSgHandle(), iDisplay(0), iSgImageInfo(), iPbufferHandle(0), iVgHandle(VG_INVALID_HANDLE),
		iCreateCount(1), iOpenCount(0), iIsDestroyed(EFalse)
	{
	EGL_TRACE("CEglImage::CEglImage(0x%x)", this);
	}

CEglImage::~CEglImage()
	{
	EGL_TRACE("CEglImage::~CEglImage(0x%x) iCreateCount=%d, iOpenCount=%d", this, iCreateCount, iOpenCount);
	EGLPANIC_ASSERT_DEBUG( (iCreateCount == 0) && (iOpenCount == 0), EEglPanicEglImageRefCountNonZero);
	iSgHandle.Close();
	}

// initialise member data
void CEglImage::Create(TSgDrawableId aSgImageId, EGLDisplay aDisplay, TSgImageInfo& aSgImageInfo, TInt aPbufferHandle,
		VGHandle aVgHandle)
	{
	EGL_TRACE("CEglImage::Create iCreateCount=%d, iOpenCount=%d", iCreateCount, iOpenCount);
	EGLPANIC_ASSERT_DEBUG(iCreateCount == 0, EEglPanicEglImageRefCountNonZero);
	EGLPANIC_ASSERT_DEBUG(!iIsDestroyed, EEglPanicEglImageIsDestroyed);
	EGLPANIC_ASSERT_DEBUG(aSgImageId != KSgNullDrawableId, EEglPanicTemp);
	++iCreateCount;
	TInt err = iSgHandle.Open(aSgImageId);
	EGLPANIC_ASSERT_DEBUG(err == KErrNone, EEglPanicTemp);
	iDisplay = aDisplay;
	iSgImageInfo = aSgImageInfo;
	iVgHandle = aVgHandle;
	iPbufferHandle = aPbufferHandle;
	}

// Duplicate open
void CEglImage::Duplicate()
	{
	EGL_TRACE("CEglImage::Duplicate iCreateCount=%d, iOpenCount=%d", iCreateCount, iOpenCount);
	EGLPANIC_ASSERT_DEBUG( (iCreateCount > 0) && !iIsDestroyed, EEglPanicEglImageIsDestroyed);
	++iCreateCount;
	}


// If successful increments reference count & returns ETrue. (Ultimtately called from e.g. vgCreateEGLImageTargetKHR.) 
TBool CEglImage::OpenForVgImage(TSize& aSize, VGHandle& aVgHandle, TUint64& aSgImageId)
	{
	EGL_TRACE("CEglImage::OpenForVgImage iCreateCount=%d, iOpenCount=%d", iCreateCount, iOpenCount);
	if ( (iCreateCount > 0) && (iSgImageInfo.iUsage & ESgUsageBitOpenVgImage) )
		{
		++iOpenCount;
		aSize = iSgImageInfo.iSizeInPixels;
		aVgHandle = iVgHandle;
		aSgImageId = iSgHandle.Id().iId;
		return ETrue;
		}
	return EFalse;
	}

// If successful decrements reference count & returns ETrue. (Ultimtately called from e.g. vgDestroyImage.) 
TBool CEglImage::Close()
	{
	EGL_TRACE("CEglImage::Close iCreateCount=%d, iOpenCount=%d", iCreateCount, iOpenCount);
	if (iOpenCount > 0)
		{
		--iOpenCount;
		if ( RefCount() == 0 )
			{
			iSgHandle.Close();
			}
		return ETrue;
		}
	return EFalse;
	}

// If not already destroyed: mark EglImage as destroyed, dec reference count, return ETrue. (Called from eglDestroyImageKHR.) 
TBool CEglImage::Destroy()
	{
	EGL_TRACE("CEglImage::Destroy iCreateCount=%d, iOpenCount=%d", iCreateCount, iOpenCount);
	if (!iIsDestroyed)
		{
		--iCreateCount;
		if (iCreateCount == 0)
			{
			iIsDestroyed = ETrue;
			if ( RefCount() == 0 )
				{
				iSgHandle.Close();
				}
			return ETrue;
			}
		}
	return EFalse;
	}

// Reference Count: count of unbalanced Create() plus Open() calls.
TInt CEglImage::RefCount()
	{
	return iOpenCount + iCreateCount;
	}

TSgDrawableId CEglImage::SgImageId() const
	{
	return iSgHandle.Id();
	}

EGLDisplay CEglImage::Display() const
	{
	return iDisplay;
	}

TBool CEglImage::IsDestroyed() const
	{
	return iIsDestroyed;
	}
#else
void CEglImage::Duplicate() {}
TBool CEglImage::OpenForVgImage(TSize& aSize, VGHandle& aVgHandle, TUint64& aSgImageId) {}
TBool CEglImage::Close() {}
TBool CEglImage::Destroy() {}
TBool CEglImage::IsDestroyed() const {}
TInt CEglImage::RefCount() {}
EGLDisplay CEglImage::Display() const {}
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// SgImage stuff in CGuestEGL
////////////////////////////////////////////////////////////////////////////////////////////////////


EGLint CGuestEGL::ValidateEglImageTarget(EGLDisplay aDisplay, EGLContext aContext, EGLenum aTarget,
		EGLClientBuffer aBuffer, const EGLint *aAttribList, TSymbianPixmapTypeId aTargetPixmapType)
	{
	if (!IsDisplayInitialized(aDisplay)) // ToDo just check display is valid, don't care if it is initialised
		{
		return EGL_BAD_DISPLAY;
		}
	if ( (aContext != EGL_NO_CONTEXT) || (aTarget != EGL_NATIVE_PIXMAP_KHR) )
		{
		return EGL_BAD_PARAMETER;
		}
	aTargetPixmapType = EglInternalFunction_GetNativePixmapType((EGLNativePixmapType) aBuffer);
	if ( (aTargetPixmapType != EPixmapTypeSgImage) || !EglInternalFunction_IsValidNativePixmap((EGLNativePixmapType) aBuffer, aTargetPixmapType) )
		{
		return EGL_BAD_PARAMETER;
		}
	// ToDo check Attrib List is valid
	return EGL_SUCCESS;
	}

#ifdef FAISALMEMON_S4_SGIMAGE
EGLImageKHR CGuestEGL::DuplicateEglImageIfItExists(EGLDisplay aDisplay, TSgDrawableId aSgId)
	{ // ToDo something smarter with a Hash Map ?
	TInt imageCount = iEglImageArray.Count();
	for (TInt index = 0; index < imageCount; ++index)
		{
		CEglImage* imagePtr = iEglImageArray[index];
		if ( !imagePtr->IsDestroyed() && (imagePtr->SgImageId() == aSgId)  && (imagePtr->Display() == aDisplay) )
			{
			imagePtr->Duplicate();
			EGL_TRACE("CGuestEGL::DuplicateEglImageIfItExists SgImage already used for eglImage=0x%x", imagePtr);
			return (EGLImageKHR)imagePtr;
			}
		}
	return EGL_NO_IMAGE_KHR;
	}



EGLImageKHR CGuestEGL::CreateNewEglImage(TEglThreadState& aThreadState, EGLDisplay aDisplay, TSgDrawableId aSgId, TSgImageInfo aSgImageInfo)
	{
	RHeap* oldHeap = CVghwUtils::SwitchToVghwHeap();
	CEglImage* newImage = new CEglImage();
	if (newImage)
		{
		if (KErrNone != iEglImageArray.InsertInAddressOrder(newImage))
			{
			delete newImage;
			newImage = NULL;
			}
		}
	CVghwUtils::SwitchFromVghwHeap(oldHeap);
	if (newImage == NULL)
		{
		EGL_TRACE("CGuestEGL::CreateNewEglImage 1.a error creating new CEglImage");
		return EGL_NO_IMAGE_KHR;
		}

	// success ...
	EGL_TRACE("CGuestEGL::CreateNewEglImage 1.b Created new eglImage=0x%x", newImage);
	TInt pbufferHandle;
	TInt vgImageHandle;
	TUint64 sgHandles;
    EGLint hostResult = CVghwUtils::EglGetSgHandles(aSgId.iId, &sgHandles);
    pbufferHandle = (TInt)(sgHandles&0xFFFFFFFF);
    vgImageHandle = (TInt)((sgHandles>>32)&0xFFFFFFFF);
	EGL_TRACE("CGuestEGL::CreateNewEglImage 2. CVghwUtils::EglGetSgHandles result=%d, pbufferHandle=0x%x, vgImageHandle=0x%x", 
			hostResult, pbufferHandle, vgImageHandle);
	EGLPANIC_ASSERT(KErrNone == hostResult, EEglPanicTemp);

	newImage->Create(aSgId, aDisplay, aSgImageInfo, pbufferHandle, vgImageHandle);

	return (EGLImageKHR)newImage;
	}

#endif // FAISALMEMON_S4_SGIMAGE


/*
 * eglCreateImageKHR supports Khronos EGL extension #8, "KHR_image_base"
 *
 * Supported values for target parameter:
 * EGL_NATIVE_PIXMAP_KHR for Khronos EGL extension #9, "KHR_image_pixmap"
 */
EGLImageKHR CGuestEGL::eglCreateImageKHR(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLContext aContext, EGLenum aTarget,
		EGLClientBuffer aBuffer, const EGLint *aAttribList)
	{
	// ToDo lock aDisplay
	EGLImageKHR eglImage = EGL_NO_IMAGE_KHR;
	iEglImageLock.Wait();
	TSymbianPixmapTypeId targetPixmapType = EPixmapTypeNone;
	EGLint error = ValidateEglImageTarget(aDisplay, aContext, aTarget, aBuffer, aAttribList, targetPixmapType);
	EGL_TRACE("CGuestEGL::eglCreateImageKHR 1. validate error=%d, targetPixmapType=%d", error, targetPixmapType);

	if (error == EGL_SUCCESS)
		{
		switch(targetPixmapType)
			{
			case EPixmapTypeSgImage:
				{
#ifdef FAISALMEMON_S4_SGIMAGE
				// try to lock SgImage from disappearing by opening a temporary handle
				RSgImage* sgImagePtr = reinterpret_cast<RSgImage*>(aBuffer);
				TSgDrawableId sgId = sgImagePtr->Id();
				RSgDrawable sgHandle;
				TSgImageInfo imgInfo;
				// ToDo ensure SgImage has correct suitable usage bits 
				if ( (sgId != KSgNullDrawableId) && (KErrNone == sgHandle.Open(sgId)) && (KErrNone == sgImagePtr->GetInfo(imgInfo)) )
					{
					EGL_TRACE("CGuestEGL::eglCreateImageKHR 2. SgImage PixelFormat=%d; size=%d,%d; Usage=0x%x",
							imgInfo.iPixelFormat, imgInfo.iSizeInPixels.iWidth, imgInfo.iSizeInPixels.iHeight, imgInfo.iUsage);

					// find / create CEglImage
					if (error == EGL_SUCCESS)
						{ // SgImage may already be used for an EglImage
						eglImage = DuplicateEglImageIfItExists(aDisplay, sgId);
						if (eglImage == EGL_NO_IMAGE_KHR)
							{ // first use
							eglImage = CreateNewEglImage(aThreadState, aDisplay, sgId, imgInfo);
							if (eglImage == EGL_NO_IMAGE_KHR)
								{
								error = EGL_BAD_ALLOC;
								}
							}
						}
					}
				else
					{
					error = EGL_BAD_PARAMETER;
					}
				sgHandle.Close();
#endif
				}
				break;

			case EPixmapTypeFbsBitmap:
			case EPixmapTypeNone:
			default: // pixmap type not supported by this API
				error = EGL_BAD_PARAMETER;
				break;
			}
		}

	iEglImageLock.Signal();
	aThreadState.SetEglError(error);

	// we either failed or should have a non-NULL handle
	EGLPANIC_ASSERT_DEBUG( (error != EGL_SUCCESS) || (eglImage != EGL_NO_IMAGE_KHR), EEglPanicTemp);

	return eglImage;
	}

/*
 * eglDestroyImageKHR supports Khronos EGL extension #8, "KHR_image_base"
 */
EGLBoolean CGuestEGL::eglDestroyImageKHR(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLImageKHR aImage)
	{
	// ToDo lock aDisplay
	EGLBoolean result = EGL_FALSE;
	iEglImageLock.Wait();
	TInt index = iEglImageArray.FindInAddressOrder((CEglImage*)aImage);
	if (index >= 0)
		{
		CEglImage* eglImage = iEglImageArray[index];
		if ( (eglImage->Display() && aDisplay) && eglImage->Destroy())
			{
			result = EGL_TRUE;
			if (eglImage->RefCount() == 0)
				{
				iEglImageArray.Remove(index);
				delete eglImage;
				}
			}
		}
	iEglImageLock.Signal();

	// set EGL Error appropriately
	aThreadState.SetEglError( result ? EGL_SUCCESS : EGL_BAD_PARAMETER);
	return result;
	}


TBool CGuestEGL::EglImageOpenForVgImage(EGLImageKHR aImage, TSize& aSize, VGHandle& aVgHandle, TUint64& aSgImageId)
	{
	EGLBoolean result = EGL_FALSE;
	iEglImageLock.Wait();
	TInt index = iEglImageArray.FindInAddressOrder((CEglImage*)aImage);
	if (index >= 0)
		{
		CEglImage* eglImage = iEglImageArray[index];
		if (eglImage->OpenForVgImage(aSize, aVgHandle, aSgImageId))
			{
			result = EGL_TRUE;
			}
		}
	iEglImageLock.Signal();
	return result;
	}

void CGuestEGL::EglImageClose(EGLImageKHR aImage)
	{
	iEglImageLock.Wait();
	TInt index = iEglImageArray.FindInAddressOrder((CEglImage*)aImage);
	if (index >= 0)
		{
		CEglImage* eglImage = iEglImageArray[index];
		if (eglImage->Close())
			{
			if (eglImage->RefCount() == 0)
				{
				iEglImageArray.Remove(index);
				delete eglImage;
				}
			}
		}
	iEglImageLock.Signal();
	}

// end of file eglsgimage.cpp
