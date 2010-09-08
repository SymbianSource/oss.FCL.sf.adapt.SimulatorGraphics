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
// Guest EGL implementation

#include <graphics/surfacemanager.h>
#include <graphics/surfaceconfiguration.h>
#include <graphics/suerror.h>
#include <graphics/surface_hints.h>
#include <e32debug.h>
#include <stdlib.h> // for malloc
#include "eglapi.h"

#include <graphics/guestvideodriverinterfaceconstants.h>


// FAISALMEMON HOLE 0

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
//

// FAISALMEMON STUB CODE
#define EGL_CHECK_ERROR(a, b, c)      /* This does no checking; just a stub */
void CGuestEGL::EglInternalFunction_DestroyWindowSurface(TSurfaceInfo&)
	{
	return; // stub code
	}

EGLBoolean CGuestEGL::EglInternalFunction_SurfaceResized(TEglThreadState&, TSurfaceInfo&, int, int) 
	{
	return EFalse; // stub code
	}
	
TBool CGuestEGL::EglInternalFunction_SwapWindowSurface(TEglThreadState&, int, int)
	{
	return EFalse; // stub code
	}
TSymbianPixmapTypeId CGuestEGL::EglInternalFunction_GetNativePixmapType(void*)
	{
	return EPixmapTypeNone; // stub code
	}
TBool CGuestEGL::EglInternalFunction_IsValidNativePixmap(void*, TSymbianPixmapTypeId)
	{
	return EFalse; // stub code
	}
const char * CGuestEGL::EglInternalFunction_QueryExtensionList()
	{
	return NULL; // stub code
	}
ProcPointer CGuestEGL::eglGetProcAddress(const char*)
	{
	return NULL; // stub code
	}

EGLSurface CGuestEGL::eglCreateWindowSurface(TEglThreadState&, int, int, void*, const int*)
	{
	return 0; // stub code
	}
const char* CGuestEGL::eglQueryString(EGLDisplay aDisplay, EGLint aName)
	{
	return NULL; // stub code
	}
// FAISALMEMON END OF STUB CODE

CGuestEGL::CGuestEGL() :
		iEglSyncExtension(NULL)
	{
	}

#ifdef FAISALMEMON_S4_SGIMAGE
void CGuestEGL::OpenSgResources()
	{
	// ToDo delay opening SgDriver until needed, as it force loads Open GL ES 1.1, GL ES 2 and Open VG DLLs if it finds them
	TInt err = iSgDriver.Open();
	EGL_TRACE("  CGuestEGL::Create SgDriver.Open err=%d", err);
	EGLPANIC_ASSERT(err == KErrNone, EEglPanicSgDriverCreateLocalFailed);

	TVersion sgImageVer = RSgDriver::Version();
	EGL_TRACE("  CGuestEGL::Create  sgImageVer=%d.%d", sgImageVer.iMajor, sgImageVer.iMinor);
	EGLPANIC_ASSERT(sgImageVer.iMajor == 1, EEglPanicBadSgDriverVersion);

	}
void CGuestEGL::CloseSgResources()
	{
	iSgDriver.Close();
	}
#else
void CGuestEGL::CloseSgResources()
	{
	}
    
void CGuestEGL::OpenSgResources()
	{
	}
#endif

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
//
CGuestEGL::~CGuestEGL()
	{
	EGL_TRACE("CGuestEGL::~CGuestEGL");
	// only expected to be called during process termination
	if (iEglSyncExtension)
		{
		delete iEglSyncExtension;
		iEglSyncExtension = NULL;
		}
	iDisplayMapLock.Close();
	CloseSgResources();
	iEglImageLock.Close();
	}

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CGuestEGL::Create()
	{
	EGL_TRACE("  CGuestEGL::Create -->");
    OpenSgResources();
    TInt err;
	err = iDisplayMapLock.CreateLocal();
	EGLPANIC_ASSERT(err == KErrNone, EEglPanicDisplayMapLockCreateLocalFailed);

	err = iEglImageLock.CreateLocal();
	EGLPANIC_ASSERT(err == KErrNone, EEglPanicEglImageLockCreateLocalFailed);	

	InitialiseExtensions();

	const char* initExtensionList = EglInternalFunction_QueryExtensionList();
	EGL_TRACE("  CGuestEGL::Create  initExtensionList=0x%x (\"%s\") <--",
			initExtensionList, initExtensionList ? initExtensionList : "");
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CGuestEGL* CGuestEGL::New()
	{
	EGL_TRACE("CGuestEGL::New  start -->");
	RHeap* threadHeap = CVghwUtils::SwitchToVghwHeap();
	CGuestEGL* result = new CGuestEGL();
	CVghwUtils::SwitchFromVghwHeap(threadHeap);
	EGLPANIC_ASSERT(result, EEglPanicGuestGraphicsAllocFailed);
	if (result)
		{
		result->Create();
		}
	EGL_TRACE("CGuestEGL::New end - result=0x%x  <--", result);
	return result;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CGuestEGL::SetError(EGLint aError)
	{ // ToDo remove - everything except EGL Sync already uses threadState->SetEglError
	EGL_TRACE( "CGuestEGL::SetError EGL error=0x%x", aError);
	TEglThreadState* threadState = CVghwUtils::EglThreadState();
	if (threadState)
		{
		threadState->SetEglError(aError);
		}
	}

EGLint CGuestEGL::CheckColorAttributes(const EGLint* aAttribList, EGLint aColorBufferType, EGLint aLuminanceBits, EGLint aRedBits,
		EGLint aGreenBits, EGLint aBlueBits, EGLint aAlphaBits)
	{
	const EGLint* pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_COLOR_BUFFER_TYPE);
	if (pValue && (*pValue != aColorBufferType))
		{
		return EGL_BAD_MATCH;
		}
	EGLint colorBits = 0;
	if (aColorBufferType == EGL_RGB_BUFFER)
		{
		colorBits = aRedBits + aGreenBits + aBlueBits + aAlphaBits;
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_RED_SIZE);
		if (pValue && (*pValue < aRedBits))
			{
			return EGL_BAD_MATCH;
			}
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_GREEN_SIZE);
		if (pValue && (*pValue < aGreenBits))
			{
			return EGL_BAD_MATCH;
			}
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_BLUE_SIZE);
		if (pValue && (*pValue < aBlueBits))
			{
			return EGL_BAD_MATCH;
			}
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_LUMINANCE_SIZE);
		if (pValue && (*pValue != 0))
			{
			return EGL_BAD_MATCH;
			}
		}
	else
		{ // EGL_LUMINANCE_BUFFER
		colorBits = aLuminanceBits + aAlphaBits;
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_RED_SIZE);
		if (pValue && (*pValue != 0))
			{
			return EGL_BAD_MATCH;
			}
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_GREEN_SIZE);
		if (pValue && (*pValue != 0))
			{
			return EGL_BAD_MATCH;
			}
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_BLUE_SIZE);
		if (pValue && (*pValue != 0))
			{
			return EGL_BAD_MATCH;
			}
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_LUMINANCE_SIZE);
		if (pValue && (*pValue < aLuminanceBits))
			{
			return EGL_BAD_MATCH;
			}
		}

	if (aAlphaBits)
		{
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_ALPHA_SIZE);
		if (pValue && (*pValue < aAlphaBits))
			{
			return EGL_BAD_MATCH;
			}
		}

	pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_BUFFER_SIZE);
	if (pValue && (*pValue < colorBits))
		{
		return EGL_BAD_MATCH;
		}
	return EGL_SUCCESS;
	}

void CGuestEGL::AppendColorAttributes(EGLint* aAttribList, EGLint aColorBufferType, EGLint aLuminanceBits, EGLint aRedBits,
		EGLint aGreenBits, EGLint aBlueBits, EGLint aAlphaBits, TBool aSetVgPreMultAlpha)
	{
	const EGLint* pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_COLOR_BUFFER_TYPE);
	if (!pValue)
		{
		TAttribUtils::AppendAttribValue(aAttribList, EGL_COLOR_BUFFER_TYPE, aColorBufferType);
		}

	EGLint colorBits = 0;
	if (aColorBufferType == EGL_RGB_BUFFER)
		{
		colorBits = aRedBits + aGreenBits + aBlueBits + aAlphaBits;
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_RED_SIZE);
		if (!pValue)
			{
			TAttribUtils::AppendAttribValue(aAttribList, EGL_RED_SIZE, aRedBits);
			}
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_GREEN_SIZE);
		if (!pValue)
			{
			TAttribUtils::AppendAttribValue(aAttribList, EGL_GREEN_SIZE, aGreenBits);
			}
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_BLUE_SIZE);
		if (!pValue)
			{
			TAttribUtils::AppendAttribValue(aAttribList, EGL_BLUE_SIZE, aBlueBits);
			}
		}
	else
		{ // EGL_LUMINANCE_BUFFER
		colorBits = aLuminanceBits + aAlphaBits;
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_LUMINANCE_SIZE);
		if (!pValue)
			{
			TAttribUtils::AppendAttribValue(aAttribList, EGL_LUMINANCE_SIZE, aLuminanceBits);
			}
		}

	pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_BUFFER_SIZE);
	if (!pValue)
		{
		TAttribUtils::AppendAttribValue(aAttribList, EGL_BUFFER_SIZE, colorBits);
		}

	if (aAlphaBits)
		{
		pValue = TAttribUtils::FindAttribValue(aAttribList, EGL_ALPHA_SIZE);
		if (!pValue)
			{
			TAttribUtils::AppendAttribValue(aAttribList, EGL_ALPHA_SIZE, aAlphaBits);
			}
		}
	if (aSetVgPreMultAlpha)
		{
		EGLint* pSurfaceType = TAttribUtils::FindAttribValue(aAttribList, EGL_SURFACE_TYPE);
		if (pSurfaceType)
			{
			*pSurfaceType |= EGL_VG_ALPHA_FORMAT_PRE_BIT;
			}
		}
	}

EGLBoolean CGuestEGL::ChooseConfigForPixmapSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, const EGLint* aAttribList, EGLConfig* aConfigs, EGLint aConfigSize,
		EGLint* aNumConfig, const void* aPixmap)
	{
	EGLint error = EGL_SUCCESS;
	EGLint* newList = NULL;

	switch ( EglInternalFunction_GetNativePixmapType((EGLNativePixmapType) (aPixmap)) )
		{
		case EPixmapTypeFbsBitmap:
			EGL_TRACE("CGuestEGL::ChooseConfigForPixmapSurface 1.a pixmap type is FbsBitmap (aPixmap=0x%x)", aPixmap);
			error = ChooseConfigAttribsForFbsBitmap(aThreadState, aAttribList, reinterpret_cast<const CFbsBitmap*>(aPixmap), &newList);
			break;

#ifdef FAISALMEMON_S4_SGIMAGE
		case EPixmapTypeSgImage:
			EGL_TRACE("CGuestEGL::ChooseConfigForPixmapSurface 1.b pixmap type is SgImage (aPixmap=0x%x)", aPixmap);
			error = ChooseConfigAttribsForSgImage(aThreadState, aAttribList, reinterpret_cast<const RSgImage*>(aPixmap), &newList);
			break;
#endif

		case EPixmapTypeNone:
		default:
			EGL_TRACE("CGuestEGL::ChooseConfigForPixmapSurface 1.c pixmap type is unknown (aPixmap=0x%x)", aPixmap);
			error = EGL_BAD_NATIVE_PIXMAP;
			break;
		}

	if (error != EGL_SUCCESS)
		{
		aThreadState.SetEglError(error);
		EGL_TRACE("CGuestEGL::ChooseConfigForPixmapSurface 2.a encountered error=0x%x", error);
		if (newList)
			{
			CVghwUtils::Free(newList);
			}
		return EGL_FALSE;
		}

	EGLPANIC_ASSERT(newList, EEglPanicTemp);
	EGL_TRACE("CGuestEGL::ChooseConfigForPixmapSurface 2.b temp AttribList ... (*newList=0x%x)", *newList);
	EGL_TRACE_ATTRIB_LIST(newList);

	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init( EglRFC::EeglChooseConfig );
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLintVector(newList, TAttribUtils::AttribListLength(newList) );
	eglApiData.AppendEGLConfigVector(aConfigs, aConfigSize, RemoteFunctionCallData::EOut);
	eglApiData.AppendEGLint(aConfigSize);
	eglApiData.AppendEGLintVector(aNumConfig, 1, RemoteFunctionCallData::EOut);
	EGLBoolean result = aThreadState.ExecEglBooleanCmd(eglApiData);
	EGL_TRACE("CGuestEGL::ChooseConfigForPixmapSurface 3. Host EGL success=%d", result);

	CVghwUtils::Free(newList);
	return result;
	}

#ifdef FAISALMEMON_S4_SGIMAGE
EGLBoolean CGuestEGL::ChooseConfigForNativeSgImagePixmapSurface( TEglThreadState& aThreadState, EGLDisplay aDisplay, const EGLint* aAttribList, EGLConfig* aConfigs, EGLint aConfigSize,
        EGLint* aNumConfig, const EGLint* aPixmap )
    {
    EGLConfig* sgConfigs;
    EGLint sgConfigCnt;
    TBool* sgConfigsMatchingAttributes;
    TInt sgConfigsMatchingAttributesCnt=0;
    EGLint* sgConfigAttribs;
    EGLint sgConfigsAttribtCnt;
    EGL_TRACE( "CGuestEGL::eglChooseConfig: ChooseConfigForNativePixmapSurface" );
    EglInternalFunction_MetaGetConfigs( aThreadState, sgConfigs, sgConfigCnt, sgConfigAttribs, sgConfigsAttribtCnt );
    EGL_TRACE( "EglInternalFunction_MetaGetConfigAttirb cnt = %d ", sgConfigsAttribtCnt );
    
    sgConfigsMatchingAttributes = new TBool[sgConfigCnt];
    
    TInt attribs_per_config = EglRFC::MetaGetConfigAttributeCnt();
    TInt configcnt = (TInt) sgConfigsAttribtCnt / attribs_per_config;
    EGL_TRACE( "EglInternalFunction_MetaGetConfigs cnt = %d ", configcnt );
    
    //The attribute values of aAttribList, in the order of MetaGetConfigAttribute(i)
    EGLint* specAttribVals = new EGLint[ attribs_per_config ];
    for( TInt i=0;i<attribs_per_config;++i )
        {
        *( specAttribVals + i ) = -1; 
        }
    int spec_attrib_kind;
    int i_spec_attrib = 0;
    //Go through all the given attributes
    while( (spec_attrib_kind =  *(aAttribList + i_spec_attrib )) != EGL_NONE )
        {
        //Check which type of attribute is specified, then set the value if types match
        for( TInt i=0;i<attribs_per_config;++i )
            {
            if( spec_attrib_kind == EglRFC::MetaGetConfigAttribute( i ) )
            *(specAttribVals + i) = *( aAttribList + i_spec_attrib );
            }
        i_spec_attrib += 2;
        }
    
    //Go through the configurations
    for( TInt i_config = 0;i_config < configcnt; ++i_config )
        {
        TBool pass = ETrue;//true, unless we bump into an attribute that doesn't match
        //Go throught the attributes of this configuration
        for( TInt i_attr = 0; i_attr < attribs_per_config; ++i_attr )
            {
            EGL_TRACE( "cnf %d, attr %d = %d ", i_config, i_attr, *( sgConfigAttribs + i_config*attribs_per_config + i_attr ) );
            //Match attribute values here, if the client-specified attribute value isn't empty
            if( *( specAttribVals + i_attr ) != -1 )
                {
                switch( EglRFC::MetaGetConfigAttributeSelCriteria( i_attr ) )
                    {
                    case EExact:
                        {
                        if( *( specAttribVals + i_attr ) != *( sgConfigAttribs + i_config*attribs_per_config + i_attr ) )
                            {
                            pass = EFalse;
                            }
                        break;
                        }
                    case EAtLeast:
                        {
                        if( *( specAttribVals + i_attr ) > *( sgConfigAttribs + i_config*attribs_per_config + i_attr ) )
                            {
                            pass = EFalse;
                            }
                        break;
                        }
                    case EMask:
                        {
                        if( !(*( specAttribVals + i_attr ) & *( sgConfigAttribs + i_config*attribs_per_config + i_attr )) )
                            {
                            pass = EFalse;
                            }
                        break;
                        }
                    }//switch comparison method
                
                }//if attribute value specified by client
            }//for through the attributes of a configuration
            if( pass )
                {
                *(sgConfigsMatchingAttributes + i_config) = ETrue; 
                }
            else
                {
                *(sgConfigsMatchingAttributes + i_config) = EFalse;
                }
        }//end for through the configurations
    
    //Now get the configs that match, and return those
    TInt aConfigsIndex = 0;
    for( TInt i_config = 0;i_config < configcnt; ++i_config )
        {
        if( *(sgConfigsMatchingAttributes + i_config) )
            {
            if( aConfigsIndex < aConfigSize )
                {
                *(aConfigs + (aConfigsIndex++)) = *(sgConfigs + i_config);
                ++sgConfigsMatchingAttributesCnt;
                }
            }
        }
    //Ok, all done. Delete allocated memory
	// ToDo use correct Heap!
    delete[] sgConfigs;
    delete[] sgConfigsMatchingAttributes;
    delete[] sgConfigAttribs;
    delete[] specAttribVals;
	return EGL_TRUE;
    }
#endif

EGLint CGuestEGL::ChooseConfigAttribsForFbsBitmap(TEglThreadState& aThreadState, const EGLint* aAttribList, const CFbsBitmap* aBitmap, EGLint** aNewList)
	{
	EGLPANIC_ASSERT_DEBUG(aNewList, EEglPanicTemp);
	EGLint error = EGL_SUCCESS;
	const TInt listLength = TAttribUtils::AttribListLength(aAttribList);
	ASSERT(listLength);
	TDisplayMode mode = ENone;
	const EGLint* pRenderType = TAttribUtils::FindAttribValue(aAttribList, EGL_RENDERABLE_TYPE);

	mode = aBitmap->DisplayMode();
	EGL_TRACE("CGuestEGL::ChooseConfigAttribsForFbsBitmap bitmap addr=0x%x, Display Mode=%d", aBitmap, mode);
	switch (mode)
		{
		case EColor64K:
			error = CheckColorAttributes(aAttribList, EGL_RGB_BUFFER, 0, 5, 6, 5);
			break;
		case EColor16M:
		case EColor16MU:
			error = CheckColorAttributes(aAttribList, EGL_RGB_BUFFER, 0, 8, 8, 8);
			break;
		case EColor16MA:
			error = CheckColorAttributes(aAttribList, EGL_RGB_BUFFER, 0, 8, 8, 8, 8);
			break;
		case EColor16MAP:
			error = CheckColorAttributes(aAttribList, EGL_RGB_BUFFER, 0, 8, 8, 8, 8);
			break;
		default: // pixmap not supported
			error = EGL_BAD_NATIVE_PIXMAP;
			break;
		}

	if (error != EGL_SUCCESS)
		{
		return error;
		}

	const TInt KExpansionSpace = 10 * 2; // want enough space for 10 extra attribute/value pairs
	*aNewList = (EGLint*) CVghwUtils::Alloc( (listLength + KExpansionSpace) * sizeof(EGLint) );
	if (!*aNewList)
		{
		return EGL_BAD_ALLOC;
		}
	memcpy(*aNewList, aAttribList, listLength * sizeof(EGLint));

	switch (mode)
		{
		case EColor64K:
			AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 5, 6, 5);
			break;
		case EColor16M:
		case EColor16MU:
			AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 8, 8, 8);
			break;
		case EColor16MA:
			AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 8, 8, 8, 8);
			break;
		case EColor16MAP:
			if (pRenderType && ( (*pRenderType) & EGL_OPENVG_BIT) )
				{
				AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 8, 8, 8, 8, ETrue);
				}
			else
				{
				AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 8, 8, 8, 8);
				}
			break;
		default:
			EGLPANIC_ALWAYS(EEglPanicTemp);
		}

	return EGL_SUCCESS;
	}

#ifdef FAISALMEMON_S4_SGIMAGE
EGLint CGuestEGL::ChooseConfigAttribsForSgImage(TEglThreadState& aThreadState, const EGLint* aAttribList, const RSgImage* aSgImage, EGLint** aNewList)
	{
	ASSERT(aNewList);
	EGLint error = EGL_SUCCESS;
	const TInt listLength = TAttribUtils::AttribListLength(aAttribList);
	ASSERT(listLength);
	const EGLint* pRenderType = TAttribUtils::FindAttribValue(aAttribList, EGL_RENDERABLE_TYPE);

	/*
	if (!iSgConfigAttribs)
		{
		EGL_TRACE( "CGuestEGL::ChooseConfigAttribsForSgImage 1. EglInternalFunction_MetaGetConfigs" );
		EGLConfig* config;
		EGLint config_cnt;
		EglInternalFunction_MetaGetConfigs(aThreadState, config, config_cnt, iSgConfigAttribs, iSgConfigsAttribtCnt);
		EGL_TRACE( "EglInternalFunction_MetaGetConfigs cnt = %d ", iSgConfigsAttribtCnt );
		}
	if (!iSgConfigAttribs)
		{ // exit if EglInternalFunction_MetaGetConfigs still failed
		return EGL_BAD_ALLOC;
		}
	*/
	// ToDo use iSgConfigAttribs

	// temporarily open a handle to the SgImage
	TSgDrawableId sgId = aSgImage->Id();
	RSgDrawable sgHandle;
	TSgImageInfo imgInfo;
	EGL_TRACE("CGuestEGL::ChooseConfigAttribsForSgImage 1. SgImage Id=0x%lx", sgId);

	// ToDo check SgImage usage bits 
	if ( (sgId != KSgNullDrawableId) && (KErrNone == sgHandle.Open(sgId)) && (KErrNone == aSgImage->GetInfo(imgInfo)) )
		{
		EGL_TRACE("CGuestEGL::ChooseConfigAttribsForSgImage 2. SgImage PixelFormat=0x%x; size=%d,%d; Usage=0x%x",
				imgInfo.iPixelFormat, imgInfo.iSizeInPixels.iWidth, imgInfo.iSizeInPixels.iHeight, imgInfo.iUsage);

		switch (imgInfo.iPixelFormat)
			{
			case ESgPixelFormatARGB_8888_PRE: // == EUidPixelFormatARGB_8888_PRE
			case ESgPixelFormatARGB_8888: // == EUidPixelFormatARGB_8888,
				error = CheckColorAttributes(aAttribList, EGL_RGB_BUFFER, 0, 8, 8, 8, 8);
				break;
			case ESgPixelFormatXRGB_8888: // == EUidPixelFormatXRGB_8888,
				error = CheckColorAttributes(aAttribList, EGL_RGB_BUFFER, 0, 8, 8, 8);
				break;
			case ESgPixelFormatRGB_565: // == EUidPixelFormatRGB_565,
				error = CheckColorAttributes(aAttribList, EGL_RGB_BUFFER, 0, 5, 6, 5);
				break;
			case ESgPixelFormatA_8: // == EUidPixelFormatA_8
				error = CheckColorAttributes(aAttribList, EGL_LUMINANCE_BUFFER, 8, 0, 0, 0);
				break;
			default: // pixmap not supported
				error = EGL_BAD_NATIVE_PIXMAP;
				break;
			}
		}
	else
		{
		error = EGL_BAD_PARAMETER;
		}
	sgHandle.Close();


	if (error != EGL_SUCCESS)
		{
		return error;
		}

	const TInt KExpansionSpace = 10 * 2; // want enough space for 10 extra attribute/value pairs
	*aNewList = (EGLint*) CVghwUtils::Alloc( (listLength + KExpansionSpace) * sizeof(EGLint) );
	if (!*aNewList)
		{
		return EGL_BAD_ALLOC;
		}
	memcpy(*aNewList, aAttribList, listLength * sizeof(EGLint));

	switch (imgInfo.iPixelFormat)
		{
		case ESgPixelFormatARGB_8888_PRE: // == EUidPixelFormatARGB_8888_PRE
			if (pRenderType && ( (*pRenderType) & EGL_OPENVG_BIT) )
				{
				AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 8, 8, 8, 8, ETrue);
				}
			else
				{
				AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 8, 8, 8, 8);
				}
			break;
		case ESgPixelFormatARGB_8888: // == EUidPixelFormatARGB_8888,
			AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 8, 8, 8, 8);
			break;
		case ESgPixelFormatXRGB_8888: // == EUidPixelFormatXRGB_8888,
			AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 8, 8, 8);
			break;
		case ESgPixelFormatRGB_565: // == EUidPixelFormatRGB_565,
			AppendColorAttributes(*aNewList, EGL_RGB_BUFFER, 0, 5, 6, 5);
			break;
		case ESgPixelFormatA_8: // == EUidPixelFormatA_8
			AppendColorAttributes(*aNewList, EGL_LUMINANCE_BUFFER, 8, 0, 0, 0);
			break;
		default: // pixmap not supported - panic because this code should be in-sync with supported SgImage formats
			EGLPANIC_ALWAYS(EEglPanicTemp);
			break;
		}

	// change requested surface type from pixmap to Pbuffer
	EGLint* pSurfaceType = TAttribUtils::FindAttribValue(*aNewList, EGL_SURFACE_TYPE);
	EGLint surfaceType = *pSurfaceType;
	*pSurfaceType = EGL_PBUFFER_BIT | (surfaceType & ~EGL_PIXMAP_BIT);

	TAttribUtils::RemoveAttrib(*aNewList, EGL_MATCH_NATIVE_PIXMAP);
	return EGL_SUCCESS;
	}
#endif

/*
  Create an information object for an opened Display.
 */
TBool CGuestEGL::CreateDisplayInfo(EGLDisplay aDisplay)
	{
	TBool result = EFalse;
	EGL_TRACE("CGuestEGL::CreateDisplayInfo begin aDisplay=%d", aDisplay);
	iDisplayMapLock.WriteLock();
	RHeap* threadHeap = CVghwUtils::SwitchToVghwHeap();
	if (NULL != iDisplayMap.Find( aDisplay))
		{
		result = ETrue;
		}
	else
		{
		TInt err = KErrNoMemory; 
		CEglDisplayInfo* dispInfo = new CEglDisplayInfo;

		if (dispInfo)
			{
			err = iDisplayMap.Insert(aDisplay, dispInfo);
	//		EGL_TRACE("CreateDisplayInfo - DisplayMap insert error %d", err);
			EGLPANIC_ASSERT_DEBUG(err == KErrNone, EEglPanicDisplayMapInsertFailed);
	
			//added for egl sync extension benefit
			if (iEglSyncExtension)
				{
				err = iEglSyncExtension->EglSyncDisplayCreate(aDisplay);
	//			EGL_TRACE("CreateDisplayInfo - EglSyncDisplayCreate error %d", err);
				EGLPANIC_ASSERT_DEBUG(err == KErrNone, EEglPanicEglSyncDisplayCreateFailed);
	
				if (err)
					{
					iDisplayMap.Remove(aDisplay);
					}
				}
			}

		if (err == KErrNone)
			{
			result = ETrue;
			}
		}
	CVghwUtils::SwitchFromVghwHeap(threadHeap);
	iDisplayMapLock.Unlock();

	EGL_TRACE("CreateDisplayInfo end, result=%d", result);
	return result;
	}

/*
  Mark information object for Display as Initialised
 */
TBool CGuestEGL::InitializeDisplayInfo(EGLDisplay aDisplay)
	{
	TBool result = EFalse;
	EGL_TRACE("CGuestEGL::InitialiseDisplayInfo begin aDisplay=%d", aDisplay);
	iDisplayMapLock.WriteLock();
	CEglDisplayInfo** pDispInfo = iDisplayMap.Find(aDisplay);
	if (pDispInfo && *pDispInfo)
		{
		(*pDispInfo)->iInitialized = ETrue;
		result = ETrue;
		}
	iDisplayMapLock.Unlock();

	EGL_TRACE("InitialiseDisplayInfo end, result=%d", result);
	return result;
	}

/*
  Check whether Display exists and is Initialised
 */
TBool CGuestEGL::IsDisplayInitialized(EGLDisplay aDisplay)
	{
	TBool result = EFalse;
	EGL_TRACE("CGuestEGL::IsDisplayInitialized begin aDisplay=%d", aDisplay);
	iDisplayMapLock.ReadLock();
	CEglDisplayInfo** pDispInfo = iDisplayMap.Find(aDisplay);
	if ( pDispInfo && *pDispInfo && (*pDispInfo)->iInitialized)
		{
		result = ETrue;
		}
	iDisplayMapLock.Unlock();

	EGL_TRACE("IsDisplayInitialized end, result=%d", result);
	return result;
	}

EGLBoolean CGuestEGL::eglSwapBuffers(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface)
    {
    EglInternalFunction_SwapWindowSurface(aThreadState, aDisplay, aSurface);

    // ToDo when all surfaces are recorded in client validate BEFORE sending cmd to host
    TSurfaceInfo* surfaceInfo = EglInternalFunction_GetPlatformSurface( aDisplay, aSurface );
    EGL_CHECK_ERROR( surfaceInfo, EGL_BAD_SURFACE, EGL_FALSE );

    //Check if surface size has changed
    TSize size = surfaceInfo->iNativeWindow->Size();

    if (size != surfaceInfo->iSize)
        {
		EGL_TRACE("CGuestEGL::eglSwapBuffers Surface Resized size=%d,%d, surfaceInfo->iSize=%d,%d",
				size.iHeight, size.iWidth, surfaceInfo->iSize.iHeight, surfaceInfo->iSize.iWidth);
        return EglInternalFunction_SurfaceResized(aThreadState, *surfaceInfo, aDisplay, aSurface);
        }
    return EGL_TRUE;
    }

EGLBoolean CGuestEGL::eglMakeCurrent(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aDraw, EGLSurface aRead, EGLContext aContext)
    {
	if (aContext == EGL_NO_CONTEXT)
		{
		if ( (aDraw != EGL_NO_SURFACE) || (aRead != EGL_NO_SURFACE) )
			{
			aThreadState.SetEglError(EGL_BAD_SURFACE);
			return EGL_FALSE;
			}
		EGL_TRACE("CGuestEGL::eglMakeCurrent call host");
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init( EglRFC::EeglMakeCurrent );
		eglApiData.AppendEGLDisplay(aDisplay);
		eglApiData.AppendEGLSurface(EGL_NO_SURFACE);
		eglApiData.AppendEGLSurface(EGL_NO_SURFACE);
		eglApiData.AppendEGLContext(EGL_NO_CONTEXT);

		EGLBoolean ret = aThreadState.ExecEglBooleanCmd(eglApiData);
		EGL_TRACE("CGuestEGL::eglMakeCurrent end success=%d", ret);
		return (EGLBoolean)ret;
		}
	else
		{
		if ( (aDraw == EGL_NO_SURFACE) || (aRead == EGL_NO_SURFACE) )
			{
			aThreadState.SetEglError(EGL_BAD_SURFACE);
			return EGL_FALSE;
			}
		// ToDo use new CEglContext code
		const TInt KMaxSurfaces( 2 );
		EGLSurface surfaces[KMaxSurfaces];
		TSurfaceInfo* surfaceInfo[KMaxSurfaces] = {NULL, NULL};
		surfaces[0] = aDraw;
		if (aDraw != aRead)
			{
			surfaces[1] = aRead;
			}
		else
			{
			surfaces[1] = EGL_NO_SURFACE;
			}

		for ( TInt i = 0; i < KMaxSurfaces; i++ )
			{
			if ( EGL_NO_SURFACE != surfaces[i] )
				{
				EGL_TRACE("CGuestEGL::eglMakeCurrent check surface %d", surfaces[i] );
				surfaceInfo[i] = EglInternalFunction_GetPlatformSurface( aDisplay, surfaces[i] );
				//EGL_CHECK_ERROR( surfaceInfo, EGL_BAD_SURFACE , EGL_FALSE );
				if ( surfaceInfo[i] )
					{
					TSize newSize;
					switch (surfaceInfo[i]->iSurfaceType)
						{
						case ESurfaceTypePixmapFbsBitmap:
							EGLPANIC_ASSERT_DEBUG(surfaceInfo[i]->iFbsBitmap, EEglPanicTemp);
							newSize = surfaceInfo[i]->iFbsBitmap->SizeInPixels();
							break;
						case ESurfaceTypeWindow:
							EGLPANIC_ASSERT_DEBUG(surfaceInfo[i]->iNativeWindow, EEglPanicTemp);
							newSize = surfaceInfo[i]->iNativeWindow->Size();
							break;
						default:
							// size cannot change for other surface types
							newSize = surfaceInfo[i]->iSize;
							break;
						}
					if (newSize != surfaceInfo[i]->iSize)
						{
						EGL_TRACE("CGuestEGL::eglMakeCurrent resize surface");
						if ( !EglInternalFunction_SurfaceResized(aThreadState, *surfaceInfo[i], aDisplay, surfaces[i] ) )
							{
							return EGL_FALSE;
							}
						surfaceInfo[i]->iSize = newSize;
						}
					}
				}
			}

		// adapt to only some surfaces having CEglSurfaceInfo objects so far 
		EGLSurface drawId = surfaceInfo[0] ? surfaceInfo[0]->iHostSurfaceId : aDraw;
		EGLSurface readId = aRead;
		if ((aRead == aDraw) && surfaceInfo[0])
			{
			readId = surfaceInfo[0]->iHostSurfaceId;
			}
		else if (surfaceInfo[1])
			{
			readId = surfaceInfo[1]->iHostSurfaceId;
			}

		EGL_TRACE("  eglMakeCurrent surfaces[0]=0x%x, surfaces[1]=0x%x", surfaces[0], surfaces[1]);
		EGL_TRACE("  eglMakeCurrent surfacesInfo[0]=0x%x, surfacesInfo[0].iHostSurfaceId=0x%x",
				surfaceInfo[0], surfaceInfo[0] ? surfaceInfo[0]->iHostSurfaceId : NULL);
		EGL_TRACE("  eglMakeCurrent surfacesInfo[1]=0x%x, surfacesInfo[1].iHostSurfaceId=0x%x",
				surfaceInfo[1], surfaceInfo[1] ? surfaceInfo[1]->iHostSurfaceId : NULL);

		EGL_TRACE("CGuestEGL::eglMakeCurrent call host");
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init( EglRFC::EeglMakeCurrent );
		eglApiData.AppendEGLDisplay(aDisplay);

		EGL_TRACE("  eglApiData.AppendEGLSurface(drawId = 0x%x)", drawId);
		eglApiData.AppendEGLSurface(drawId);
		EGL_TRACE("  eglApiData.AppendEGLSurface(readId = 0x%x);", readId);
		eglApiData.AppendEGLSurface(readId);

		eglApiData.AppendEGLContext(aContext);
		EGLBoolean ret = aThreadState.ExecEglBooleanCmd(eglApiData);
		EGL_TRACE("CGuestEGL::eglMakeCurrent end success=%d", ret);
		return (EGLBoolean)ret;
		}
	}

// FAISALMEMON HOLE 0.1


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CGuestEGL::EglInternalFunction_CreateSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLConfig aConfig, RWindow* aNativeWindow,
		TSurfaceInfo& aSurfaceInfo)
	{
    RSurfaceManager::TSurfaceCreationAttributesBuf attributes;
    RSurfaceManager::TInfoBuf info;

    aSurfaceInfo.iNativeWindow = aNativeWindow;
    aSurfaceInfo.iSurfaceType = ESurfaceTypeWindow;
    aSurfaceInfo.iSize = aNativeWindow->Size();
    aSurfaceInfo.iConfigId = aConfig;
	aSurfaceInfo.iHostSurfaceId = aSurface;

    // ToDo have function variants that do not validate parameters
    eglQuerySurface(aThreadState, aDisplay, aSurface, EGL_VG_ALPHA_FORMAT, &aSurfaceInfo.iAlphaFormat);
    eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_BUFFER_SIZE, &aSurfaceInfo.iColorBits);
    eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_RED_SIZE, &aSurfaceInfo.iRedBits);
    eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_GREEN_SIZE, &aSurfaceInfo.iGreenBits);
    eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_BLUE_SIZE, &aSurfaceInfo.iBlueBits);
    eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_ALPHA_SIZE, &aSurfaceInfo.iAlphaBits);

    /*
    TInt err = LastError();
    EGL_CHECK_ERROR( EGL_SUCCESS == err, err, EGL_FALSE );
    */
    EGL_TRACE("  Win surface details: width=%d height=%d colorbits=%d red=%d green=%d blue=%d alpha=%d alphaformat=0x%x",
    		aSurfaceInfo.iSize.iWidth, aSurfaceInfo.iSize.iHeight, aSurfaceInfo.iColorBits, aSurfaceInfo.iRedBits,
    		aSurfaceInfo.iGreenBits, aSurfaceInfo.iBlueBits, aSurfaceInfo.iAlphaBits, aSurfaceInfo.iAlphaFormat);

    TSize size;

    // FAISALMEMON HOLE 1

    // FAISALMEMON STUB CODE
    TUint8 offsetToFirstBuffer = 0; // This is wrong; just stub code
    TUint8 offsetToSecondBuffer = 0; // This is wrong; just stub code
    // FAISALMEMON END OF STUB CODE
    
    TUint32 chunkHWBase = 0;
    (void)CVghwUtils::MapToHWAddress(aSurfaceInfo.iChunk->Handle(), chunkHWBase);
    // FAISALMEMON write code to handle errors in the above function
    EGL_TRACE("CGuestEGL::EglInternalFunction_CreateSurface AFTER VGHWUtils::MapToHWAddress");

    /* Store the pointer to the pixel data */
    aSurfaceInfo.iBuffer0 = aSurfaceInfo.iChunk->Base() + offsetToFirstBuffer;
    aSurfaceInfo.iBuffer1 = aSurfaceInfo.iChunk->Base() + offsetToSecondBuffer;

    aSurfaceInfo.iBuffer0Index = (chunkHWBase + offsetToFirstBuffer) - VVI_FRAMEBUFFER_BASE_ADDRESS;
    aSurfaceInfo.iBuffer1Index = (chunkHWBase + offsetToSecondBuffer) - VVI_FRAMEBUFFER_BASE_ADDRESS;
    EGL_TRACE("CGuestEGL::EglInternalFunction_CreateSurface %u %x %x %x %x",chunkHWBase, offsetToFirstBuffer, offsetToSecondBuffer,
    		aSurfaceInfo.iBuffer0Index,
    		aSurfaceInfo.iBuffer1Index);

    if ( !EglInternalFunction_CallSetSurfaceParams(aThreadState, aDisplay, aSurface, aSurfaceInfo) )
        {
        EGL_TRACE("CGuestEGL::EglInternalFunction_CreateSurface end failure");

        return EGL_FALSE;
        }
    EGL_TRACE("CGuestEGL::EglInternalFunction_CreateSurface end success");

    return EGL_TRUE;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EGLBoolean CGuestEGL::eglDestroySurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface)
	{
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	EGLBoolean result = EGL_FALSE;

	// do not destroy SgImage surfaces on the Host!
	TSurfaceInfo* surfaceInfo = EglInternalFunction_GetPlatformSurface(aDisplay, aSurface);
	if (!surfaceInfo)
		{ // Note: Pbuffer surfaces are not currently recorded in client
		eglApiData.Init( EglRFC::EeglDestroySurface );
		eglApiData.AppendEGLDisplay(aDisplay);
		eglApiData.AppendEGLSurface(aSurface);
		result = aThreadState.ExecEglBooleanCmd(eglApiData);
		}
	else 
		{
		if (surfaceInfo->iSurfaceType != ESurfaceTypePixmapSgImage)
			{ // destroy surface allocated by Host EGL 
			eglApiData.Init( EglRFC::EeglDestroySurface );
			eglApiData.AppendEGLDisplay(aDisplay);
			eglApiData.AppendEGLSurface(surfaceInfo->iHostSurfaceId);
			result = aThreadState.ExecEglBooleanCmd(eglApiData);
			}
#ifdef FAISALMEMON_S4_SGIMAGE
		else
			{ // release SgImage handle
			surfaceInfo->iSgHandle.Close();
			}
#endif

		DestroySurfaceInfo(aDisplay, aSurface);
		}

	return result;
	}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CGuestEGL::DestroySurfaceInfo(EGLDisplay aDisplay, EGLSurface aSurface)
    {
    EGL_TRACE("DestroySurfaceInfo begin");

    iDisplayMapLock.ReadLock();
    CEglDisplayInfo** pDispInfo = iDisplayMap.Find(aDisplay);
    if (!pDispInfo || !*pDispInfo)
        {
        EGL_TRACE("cannot find display %d", aDisplay);
        }
	else
		{
		TSurfaceInfo** pSurfaceInfo = (*pDispInfo)->iSurfaceMap.Find(aSurface);
		if (!pSurfaceInfo)
			{
			EGL_TRACE("cannot find surface %d for display %d", aSurface, aDisplay);
			}
		else
			{
			TSurfaceInfo* surfaceInfo = *pSurfaceInfo;
			RHeap* threadHeap = CVghwUtils::SwitchToVghwHeap();
			if (surfaceInfo->iNativeWindow)
				{
				EglInternalFunction_DestroyWindowSurface(*surfaceInfo);
				}
			delete surfaceInfo;
			(*pDispInfo)->iSurfaceMap.Remove( aSurface );
			CVghwUtils::SwitchFromVghwHeap(threadHeap);
			EGL_TRACE("DestroySurfaceInfo end");
			}
		}

	iDisplayMapLock.Unlock();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CGuestEGL::DestroyDisplayInfo (EGLDisplay aDisplay)
    {
    EGL_TRACE("DestroyDisplayInfo begin aDisplay=%d", aDisplay);
    TBool success = EFalse;

	iDisplayMapLock.WriteLock();
    CEglDisplayInfo** pDispInfo = iDisplayMap.Find(aDisplay);
    if (!pDispInfo || !*pDispInfo)
        {
        EGL_TRACE("cannot find display %d", aDisplay);
        }
	else
		{
		success = ETrue;
		RHeap* threadHeap = CVghwUtils::SwitchToVghwHeap();
		RHashMap<TInt, TSurfaceInfo*>::TIter iter((*pDispInfo)->iSurfaceMap);
		for (;;)
			{
			TSurfaceInfo** pSurfaceInfo = const_cast<TSurfaceInfo**>(iter.NextValue());
			if (!pSurfaceInfo)
				{
				break;
				}

			EGL_TRACE("destroying surface %d", *iter.CurrentKey());

			TSurfaceInfo* surfaceInfo = *pSurfaceInfo;
			if (surfaceInfo->iNativeWindow)
				{
				EglInternalFunction_DestroyWindowSurface(*surfaceInfo);
				}
			delete surfaceInfo;
			}
		iDisplayMap.Remove(aDisplay);
		CVghwUtils::SwitchFromVghwHeap(threadHeap);

		//added for egl sync extension benefit
		if (iEglSyncExtension)
			{
			iEglSyncExtension->EglSyncDisplayDestroy(aDisplay);
			}
		}

	iDisplayMapLock.Unlock();
	EGL_TRACE("DestroyDisplayInfo end - ret=%d", success);
	return success;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EGLBoolean CGuestEGL::EglInternalFunction_CallSetSurfaceParams(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, TSurfaceInfo& aSurfaceInfo)
    {
    EGL_TRACE("EglInternalFunction_CallSetSurfaceParams begin");

    RemoteFunctionCallData rfcdata;
    EglRFC eglApiData( rfcdata );
    eglApiData.Init(EglRFC::EeglSimulatorSetSurfaceParams);
    eglApiData.AppendEGLDisplay(aDisplay);
    eglApiData.AppendEGLSurface( aSurface );
    EGLNativeWindowType win = (EGLNativeWindowType)aSurfaceInfo.iNativeWindow->ClientHandle();
    eglApiData.AppendEGLNativeWindowType( win );
    eglApiData.AppendEGLSize(aSurfaceInfo.iSize);
    eglApiData.AppendEGLint(aSurfaceInfo.iStride);
    eglApiData.AppendEGLint(aSurfaceInfo.iBuffer0Index);
    eglApiData.AppendEGLint(aSurfaceInfo.iBuffer1Index);
    aThreadState.ExecuteEglNeverErrorCmd(eglApiData);

    EGL_TRACE("EglInternalFunction_CallSetSurfaceParams end");
    return EGL_TRUE;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EGLint CGuestEGL::ConfigMatchesFbsBitmapPixmap(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, const EGLint* aAttribList, TDisplayMode aMode)
	{
	// ToDo check EGL_VG_ALPHA_FORMAT ?
	EGLint colorBits, redBits, greenBits, blueBits, alphaBits;
	EGLint wantColorBits, wantRedBits, wantGreenBits, wantBlueBits;
	EGLint wantAlphaBits = 0;
	switch (aMode)
		{
		case EColor64K:
			wantColorBits = 12;
			wantRedBits = 5;
			wantGreenBits = 6;
			wantBlueBits = 5;
			break;
		case EColor16M:
		case EColor16MU:
			wantColorBits = 24;
			wantRedBits = 8;
			wantGreenBits = 8;
			wantBlueBits = 8;
			break;
		case EColor16MA:
			wantColorBits = 32;
			wantRedBits = 8;
			wantGreenBits = 8;
			wantBlueBits = 8;
			wantAlphaBits = 8;
			break;
		case EColor16MAP:
			wantColorBits = 32;
			wantRedBits = 8;
			wantGreenBits = 8;
			wantBlueBits = 8;
			wantAlphaBits = 8;
			break;
		default: // pixmap format not supported
			return EGL_BAD_NATIVE_PIXMAP;
		}

	// ToDo version of these functions lighter on parameter checking - maybe use cached values
	if (!eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_BUFFER_SIZE, &colorBits) || 
			!eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_RED_SIZE, &redBits) ||
			!eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_GREEN_SIZE, &greenBits) ||
			!eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_BLUE_SIZE, &blueBits) ||
			!eglGetConfigAttrib(aThreadState, aDisplay, aConfig, EGL_ALPHA_SIZE, &alphaBits) )
		{ // fetch failed
		return EGL_BAD_MATCH;
		}
	
	EGL_TRACE("CGuestEGL::ConfigMatchesFbsBitmapPixmap: want %d bpp, %d red, %d green, %d blue, %d alpha\n\tconfig has %d bpp, %d red, %d green, %d blue, %d alpha",
			wantColorBits, wantRedBits, wantGreenBits, wantBlueBits, wantAlphaBits,
			colorBits, redBits, greenBits, blueBits, alphaBits);

	if ( (colorBits < wantColorBits) || (redBits < wantRedBits) || (greenBits < wantGreenBits) ||
			(blueBits < wantBlueBits) || (alphaBits < wantAlphaBits) )
		{ // config does not match bitmap
		return EGL_BAD_MATCH;
		}

	return EGL_SUCCESS;
	}

/*
 Returns EGL_NO_SURFACE on failure. If the attributes of pixmap do not
 correspond to config, then an EGL_BAD_MATCH error is generated. If config does
 not support rendering to pixmaps (the EGL_SURFACE_TYPE attribute does not
 contain EGL_PIXMAP_BIT), an EGL_BAD_MATCH error is generated. If config does
 not support the colorspace or alpha format attributes specified in attrib list
 (as defined for eglCreateWindowSurface), an EGL_BAD_MATCH error is generated.
 If config is not a valid EGLConfig, an EGL_BAD_CONFIG error is generated. If
 pixmap is not a valid native pixmap handle, then an EGL_BAD_NATIVE_PIXMAP
 error should be generated. If there is already an EGLSurface associated with
 pixmap (as a result of a previous eglCreatePixmapSurface call), then a
 EGL_BAD_ALLOC error is generated. Finally, if the implementation cannotallocate
 resources for the new EGL pixmap, an EGL_BAD_ALLOC error is generated.
 */
EGLSurface CGuestEGL::eglCreatePixmapSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, EGLNativePixmapType aNativePixmap, const EGLint *aAttribList)
	{
	EGL_TRACE( "CGuestEGL::eglCreatePixmapSurface");
	EGL_TRACE_ATTRIB_LIST(aAttribList);

	EGLSurface newSurfaceId = EGL_NO_SURFACE;
	TSurfaceInfo* surfaceInfo = NULL;
	EGLint error = EGL_BAD_DISPLAY;
	TSymbianPixmapTypeId pixmapType = EPixmapTypeNone;

	const EGLint* pixmapMatch = TAttribUtils::FindAttribValue(aAttribList, EGL_MATCH_NATIVE_PIXMAP);
	if ( pixmapMatch && ( (*pixmapMatch) != (EGLint)(aNativePixmap) ) )
		{ // if EGL_MATCH_NATIVE_PIXMAP is in the attribute list it must be the same pixmap as aNativePixmap
		error = EGL_BAD_MATCH;
		}
	else
		{
		iDisplayMapLock.WriteLock();
		CEglDisplayInfo** pDispInfo;
		pDispInfo = iDisplayMap.Find(aDisplay);
	
		if (pDispInfo && *pDispInfo)
			{
			RHeap* threadHeap = CVghwUtils::SwitchToVghwHeap();
	
			surfaceInfo = new TSurfaceInfo();
			if (surfaceInfo)
				{
				surfaceInfo->iConfigId = aConfig;
				pixmapType = EglInternalFunction_GetNativePixmapType(aNativePixmap);
				switch (pixmapType)
					{
					case EPixmapTypeFbsBitmap:
						newSurfaceId = CreateFbsBitmapSurface(aThreadState, aDisplay, aConfig, reinterpret_cast<CFbsBitmap*>(aNativePixmap), aAttribList,
								*surfaceInfo);
						break;
#ifdef FAISALMEMON_S4_SGIMAGE
					case EPixmapTypeSgImage:
						newSurfaceId = CreateSgImageSurface(aThreadState, aDisplay, aConfig, reinterpret_cast<const RSgImage*>(aNativePixmap), aAttribList,
								*surfaceInfo);
						break;
#endif
					default:
						break;
					}
				if (newSurfaceId == EGL_NO_SURFACE)
					{
					error = EGL_BAD_NATIVE_PIXMAP;
					}
				else
					{
					error = EGL_SUCCESS;
					EGL_TRACE( "CGuestEGL::eglCreatePixmapSurface inserting surface 0x%x to display %d", newSurfaceId, aDisplay);
					if (KErrNone != (*pDispInfo)->iSurfaceMap.Insert(newSurfaceId, surfaceInfo))
						{
						error = EGL_BAD_ALLOC;
						if (pixmapType == EPixmapTypeFbsBitmap)
							{
							(void) eglDestroySurface(aThreadState, aDisplay, newSurfaceId);
							}
						newSurfaceId = EGL_NO_SURFACE;
						}
					}
				if (error != EGL_SUCCESS)
					{
					delete surfaceInfo;
					surfaceInfo = NULL;
					}
				}
			else
				{
				error = EGL_BAD_ALLOC;
				}
			CVghwUtils::SwitchFromVghwHeap(threadHeap);
			}  // dispInfo
		iDisplayMapLock.Unlock();
		}

	aThreadState.SetEglError(error);

	// parameter check failed
	if (error != EGL_SUCCESS)
		{
		EGLPANIC_ASSERT_DEBUG(newSurfaceId == EGL_NO_SURFACE, EEglPanicTemp);
		EGLPANIC_ASSERT_DEBUG(surfaceInfo == NULL, EEglPanicTemp);
		return EGL_NO_SURFACE;
		}

	EGLPANIC_ASSERT_DEBUG(newSurfaceId != EGL_NO_SURFACE, EEglPanicTemp);
	EGLPANIC_ASSERT_DEBUG(surfaceInfo != NULL, EEglPanicTemp);

	return newSurfaceId;
	}

EGLSurface CGuestEGL::CreateFbsBitmapSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, CFbsBitmap* aBitmap,
		const EGLint *aAttribList, TSurfaceInfo& aSurfaceInfo)
	{
	EGLSurface newSurfaceId = EGL_NO_SURFACE;
	aSurfaceInfo.iSurfaceType = ESurfaceTypePixmapFbsBitmap;
	aSurfaceInfo.iFbsBitmap = aBitmap;

	TDisplayMode mode = ENone;
	TInt stride;
	aSurfaceInfo.iSize = aBitmap->SizeInPixels(); 
	EGL_TRACE("CGuestEGL::CreateFbsBitmapSurface image width=%d, height=%d", aSurfaceInfo.iSize.iWidth, aSurfaceInfo.iSize.iHeight);

	// check that a pixmap surface has not previously been created from this CFbsBitmap
	if (!EglInternalFunction_PixmapSurfacePreviouslyCreated(aBitmap, EPixmapTypeFbsBitmap))
		{
		// error = EGL_BAD_ALLOC;
		return EGL_NO_SURFACE;
		}
	else
		{
		mode = aBitmap->DisplayMode();
		if (EGL_SUCCESS != ConfigMatchesFbsBitmapPixmap(aThreadState, aDisplay, aConfig, aAttribList, mode))
			{
			return EGL_NO_SURFACE;
			}
		}

	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	stride = CFbsBitmap::ScanLineLength(aSurfaceInfo.iSize.iWidth, mode);
	eglApiData.Init( EglRFC::EeglCreatePixmapSurface );
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLConfig(aConfig);
	eglApiData.AppendEGLNativePixmapType((EGLNativePixmapType) aBitmap);
	eglApiData.AppendEGLint(mode);
	eglApiData.AppendEGLSize(aSurfaceInfo.iSize);
	eglApiData.AppendEGLint(stride);
	eglApiData.AppendEGLint(EPixmapTypeFbsBitmap);
	eglApiData.AppendEGLintVector(aAttribList, TAttribUtils::AttribListLength(aAttribList));
	newSurfaceId = aThreadState.ExecEglSurfaceCmd(eglApiData); 
	EGL_TRACE( "EeglCreatePixmapSurface aDisplay=%d, config=%d, format=%d, width=%d, height=%d, stride=%d, pixmapType=%d, newSurface=%d",
			aDisplay, aConfig, mode, aSurfaceInfo.iSize.iWidth, aSurfaceInfo.iSize.iHeight, stride, EPixmapTypeFbsBitmap, newSurfaceId);

	aSurfaceInfo.iHostSurfaceId = newSurfaceId;
	return newSurfaceId;
	}

#ifdef FAISALMEMON_S4_SGIMAGE
EGLSurface CGuestEGL::CreateSgImageSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, const RSgImage* aSgImage,
		const EGLint *aAttribList, TSurfaceInfo& aSurfaceInfo)
	{
	// ToDo check that the SgImage is not already being used for a surface or EglImage
	aSurfaceInfo.iSurfaceType = ESurfaceTypePixmapSgImage;
	aSurfaceInfo.iSgId = aSgImage->Id();

    EGL_TRACE("CGuestEGL::CreateSgImageSurface sgImage id 0x%lx", aSurfaceInfo.iSgId);
	TSgImageInfo imgInfo;
	// ToDo ensure SgImage has correct suitable usage bits 
	if ( (aSurfaceInfo.iSgId != KSgNullDrawableId) && (KErrNone == aSurfaceInfo.iSgHandle.Open(aSurfaceInfo.iSgId)) &&
			(KErrNone == aSgImage->GetInfo(imgInfo)) )
		{
		EGL_TRACE("CGuestEGL::CreateSgImageSurface 1. SgImage PixelFormat=%d; size=%d,%d; Usage=0x%x",
				imgInfo.iPixelFormat, imgInfo.iSizeInPixels.iWidth, imgInfo.iSizeInPixels.iHeight, imgInfo.iUsage);
		aSurfaceInfo.iSize = imgInfo.iSizeInPixels;

        /* Package the handles to 64-bit value, since there's only one parameter available.
           pbufferHandle is the lower 32 bits, VGImageHandle is the upper bits. */
        TUint64 sgHandles;
		EGLint hostResult = CVghwUtils::EglGetSgHandles(aSurfaceInfo.iSgId.iId, &sgHandles);
		EGL_TRACE("CGuestEGL::CreateSgImageSurface 2. EglGetSgHandles result=%d, sgHandles=0x%lx", hostResult, sgHandles);
		aSurfaceInfo.iHostSurfaceId = (EGLSurface)(sgHandles&0xFFFFFFFF);
		return aSurfaceInfo.iHostSurfaceId;
		}
	
	aSurfaceInfo.iSgHandle.Close();
	return EGL_NO_SURFACE;
	}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TSurfaceInfo* CGuestEGL::EglInternalFunction_GetPlatformSurface( EGLDisplay display, EGLSurface surface )
	{
	EGL_TRACE( "CGuestEGL::EglInternalFunction_GetPlatformSurface");
	TSurfaceInfo* result = NULL;

	iDisplayMapLock.ReadLock();

	CEglDisplayInfo** pDispInfo = iDisplayMap.Find( display );
	if (pDispInfo && *pDispInfo)
		{
		TSurfaceInfo** pSurfaceInfo = (*pDispInfo)->iSurfaceMap.Find( surface );
		if (pSurfaceInfo)
			{
			result = *pSurfaceInfo;
			}
		}

	// TODO on success should probably Unlock() the surface in the caller
	iDisplayMapLock.Unlock();

	/* TODO review calling code, to see if this suggestion makes sense
	if (result == NULL)
		{
		EGL_RAISE_ERROR( EGL_BAD_SURFACE, NULL); //Enable this when all surfaces are in surface map
		}
		*/
	return result;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EGLBoolean CGuestEGL::eglWaitClient(TEglThreadState& aThreadState)
	{
	EGL_TRACE( "CGuestEGL::eglWaitClient");
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglWaitClient);
	EGLBoolean result = aThreadState.ExecEglBooleanCmd(eglApiData);

	if (result)
		{
		// ToDo cache in client, check results are not EGL_NO_DISPLAY / EGL_NO_SURFACE 
		EGLDisplay display = eglGetCurrentDisplay(aThreadState);
		EGLSurface surface = eglGetCurrentSurface(aThreadState, EGL_DRAW);
		iDisplayMapLock.ReadLock();
		CEglDisplayInfo** pDispInfo = iDisplayMap.Find( display );
		TSurfaceInfo* surfaceInfo = NULL;

		if (!pDispInfo || !*pDispInfo)
			{
			EGL_TRACE( "cannot find display %d", display );
			}
		else
			{
			TSurfaceInfo** pSurfaceInfo = (*pDispInfo)->iSurfaceMap.Find( surface );
			if (!pSurfaceInfo)
				{
				EGL_TRACE( "cannot find surface %d for display %d", surface, display );
				}
			else
				{
				surfaceInfo = *pSurfaceInfo;
				}
			}
		iDisplayMapLock.Unlock();
		if (surfaceInfo == NULL)
			{
			return EGL_FALSE;
			}
		if (surfaceInfo->iSurfaceType == ESurfaceTypePixmapFbsBitmap)
			{
			EGLPANIC_ASSERT_DEBUG(surfaceInfo->iFbsBitmap, EEglPanicTemp);
			surfaceInfo->iFbsBitmap->BeginDataAccess();

			RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
			eglApiData.Init( EglRFC::EeglSimulatorCopyImageData );
			const TSize sizePixels( surfaceInfo->iFbsBitmap->Header().iSizeInPixels );
			eglApiData.AppendVector( (void*)surfaceInfo->iFbsBitmap->DataAddress(),
			CFbsBitmap::ScanLineLength( sizePixels.iWidth, surfaceInfo->iFbsBitmap->DisplayMode() )*sizePixels.iHeight,
				RemoteFunctionCallData::EOut );
			aThreadState.ExecuteEglNeverErrorCmd(eglApiData);

			surfaceInfo->iFbsBitmap->EndDataAccess();
			}
		}

	return result;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EGLSurface CGuestEGL::eglGetCurrentSurface(TEglThreadState& aThreadState, EGLint aReadDraw)
	{
	// ToDo cache in client
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglGetCurrentSurface);
	eglApiData.AppendEGLint(aReadDraw);
	return ExecEglSurfaceNoErrorCmd(aThreadState, eglApiData); 
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EGLDisplay CGuestEGL::eglGetCurrentDisplay(TEglThreadState& aThreadState)
	{
	// ToDo cache in client
	EGL_TRACE("CGuestEGL::eglGetCurrentDisplay");
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglGetCurrentDisplay);
	return ExecEglDisplayNoErrorCmd(aThreadState, eglApiData); 
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EGLBoolean CGuestEGL::eglCopyBuffers(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLNativePixmapType aTarget)
	{
	TSize pixmapSize;

	EGL_TRACE("CGuestEGL::eglCopyBuffers");
	EGLBoolean hostResult = EGL_FALSE;
	TSymbianPixmapTypeId targetPixmapType = EglInternalFunction_GetNativePixmapType(aTarget);

	// Only CFbsBitmap native pixmaps are supported by this API.  (SgImages are not supported, as per the SgImsge Lite spec.)  
	if ( (targetPixmapType == EPixmapTypeFbsBitmap) && EglInternalFunction_IsValidNativePixmap(aTarget, targetPixmapType) )
		{
		RemoteFunctionCallData rfcdata; EglRFC eglApiData(rfcdata);
		CFbsBitmap* bitmap = (CFbsBitmap*)aTarget;
		TSize pixmapSize = bitmap->SizeInPixels(); 
		bitmap->BeginDataAccess();

		TDisplayMode mode = bitmap->DisplayMode();
		EGLint stride = CFbsBitmap::ScanLineLength(bitmap->Header().iSizeInPixels.iWidth, mode);
		void* data = bitmap->DataAddress();

#ifdef _DEBUG
		char* modeName = NULL;
		switch (mode)
			{
			case ENone: modeName = "ENone"; break;
			case EGray2: modeName = "EGray2"; break;
			case EGray4: modeName = "EGray4"; break;
			case EGray16: modeName = "EGray16"; break;
			case EGray256: modeName = "EGray256"; break;
			case EColor16: modeName = "EColor16"; break;
			case EColor256: modeName = "EColor256"; break;
			case EColor64K: modeName = "EColor64K"; break;
			case EColor16M: modeName = "EColor16M"; break;
			case ERgb: modeName = "ERgb"; break;
			case EColor4K: modeName = "EColor4K"; break;
			case EColor16MU: modeName = "EColor16MU"; break;
			case EColor16MA: modeName = "EColor16MA"; break;
			case EColor16MAP: modeName = "EColor16MAP"; break;
			case EColorLast: modeName = "EColorLast"; break;
			default: modeName = "unknown"; break;
			}
		EGL_TRACE("EglInternalFunction_GetNativePixmapInfo (0x%x) -> CFbsBitmap: DisplayMode=%d (\"%s\"), ScanLineLength=%d, data addr=0x%x",
				bitmap, mode, modeName, stride, data);
#endif

		eglApiData.Init(EglRFC::EeglCopyBuffers);
		eglApiData.AppendEGLDisplay(aDisplay);
		eglApiData.AppendEGLSurface(aSurface);
		const TSize sizePixels( bitmap->Header().iSizeInPixels );
		eglApiData.AppendVector((void*)bitmap->DataAddress(), 
				stride*pixmapSize.iHeight,
				RemoteFunctionCallData::EOut);

		eglApiData.AppendEGLint((EGLint) mode);
		eglApiData.AppendEGLSize(pixmapSize);
		eglApiData.AppendEGLint(stride);
		eglApiData.AppendEGLint(targetPixmapType);

		hostResult = aThreadState.ExecEglBooleanCmd(eglApiData);
		bitmap->EndDataAccess();
		}
	else
		{
		aThreadState.SetEglError(EGL_BAD_NATIVE_PIXMAP);
		}

	return hostResult;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
// Beware odd logic: EFalse if surface is found, otherwise ETrue
TBool CGuestEGL::EglInternalFunction_PixmapSurfacePreviouslyCreated(EGLNativePixmapType pixmap, TSymbianPixmapTypeId pixmapType)
	{
	// ToDo keep a hashmap of pixmap addresses currently used for surfaces
	TBool result = ETrue;
    if(pixmapType == EPixmapTypeFbsBitmap)
        {
		iDisplayMapLock.ReadLock();
        RHashMap<TInt, CEglDisplayInfo*>::TIter iter( iDisplayMap );
        for (;;)
            {
            CEglDisplayInfo** pDispInfo = const_cast<CEglDisplayInfo**>(iter.NextValue());
            if (!pDispInfo || !*pDispInfo)
                {
                break;
                }

            RHashMap<TInt, TSurfaceInfo*>::TIter iter2((*pDispInfo)->iSurfaceMap);
            for (;;)
                {
                TSurfaceInfo** pSurfaceInfo = const_cast<TSurfaceInfo**>(iter2.NextValue());
                if (!pSurfaceInfo)
                    {
                    break;
                    }

                TSurfaceInfo* surfaceInfo = *pSurfaceInfo;
                if ( (surfaceInfo->iSurfaceType == ESurfaceTypePixmapFbsBitmap) && (surfaceInfo->iFbsBitmap == pixmap) )
                    {
                    result = EFalse;
                    }
                }
            }
		iDisplayMapLock.Unlock();
        }

	EGL_TRACE("CGuestEGL::EglInternalFunction_PixmapSurfacePreviouslyCreated %d", result);
	return result;
	}

// FAISALMEMON HOLE 2

EGLDisplay CGuestEGL::eglGetDisplay(TEglThreadState& aThreadState, EGLNativeDisplayType aDisplayId)
	{
	RemoteFunctionCallData rfcdata; EglRFC eglApiData(rfcdata);
	eglApiData.Init(EglRFC::EeglGetDisplay);
	eglApiData.AppendEGLNativeDisplayType(aDisplayId);
	EGLDisplay display = ExecEglDisplayNoErrorCmd(aThreadState, eglApiData);

	if (display != EGL_NO_DISPLAY)
		{
		if (!CreateDisplayInfo(display))
			{ // alloc failed
			display = EGL_NO_DISPLAY;
			}
		}
	return display;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EGLint CGuestEGL::InitialiseExtensions()
	{
	iEglSyncExtension = CEglSyncExtension::Create(*this);
	return EGL_SUCCESS;
	}


// API supporting EGL sync extension
// lock the display once found
EGLint CGuestEGL::FindAndLockDisplay(EGLDisplay aDisplay)
	{
	EGLint result = EGL_BAD_DISPLAY;
	EGL_TRACE("CGuestEGL::FindAndLockDisplay aDisplay=%d", aDisplay);
	iDisplayMapLock.ReadLock();
	CEglDisplayInfo** pDisp = iDisplayMap.Find(aDisplay);
	if (pDisp && *pDisp)
		{
		CEglDisplayInfo* disp = *pDisp;
		if (disp->iInitialized)
			{
			EGL_TRACE("CGuestEGL::FindAndLockDisplay display found");
			result = EGL_SUCCESS;
			}
		else
			{
			EGL_TRACE("CGuestEGL::FindAndLockDisplay display not initialized");
			result = EGL_NOT_INITIALIZED;
			}
		}
	else
		{
		EGL_TRACE("CGuestEGL::FindAndLockDisplay cannot find display");
		}
	if (result != EGL_SUCCESS)
		{
		iDisplayMapLock.Unlock();
		}
	return result;
	}

// release the lock
void CGuestEGL::ReleaseDisplayLock(EGLDisplay aDisplay)
	{
	EGL_TRACE("CGuestEGL::ReleaseDisplayLock aDisplay=%d", aDisplay);
	iDisplayMapLock.Unlock();
	}


CEglSyncExtension* CGuestEGL::EGLSyncExtension()
    {
    return iEglSyncExtension;
    }


/*
 EGL_FALSE is returned on failure and major and minor are not updated. An
 EGL_BAD_DISPLAY error is generated if the dpy argument does not refer to a valid
 EGLDisplay. An EGL_NOT_INITIALIZED error is generated if EGL cannot be
 initialized for an otherwise valid dpy.
 */
EGLBoolean CGuestEGL::eglInitialize(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLint *aMajor, EGLint *aMinor)
	{
	if ((aDisplay == EGL_NO_DISPLAY) || !InitializeDisplayInfo(aDisplay))
		{
		aThreadState.SetEglError(EGL_BAD_DISPLAY);
		return EGL_FALSE;
		}

	RemoteFunctionCallData rfcdata; EglRFC eglApiData(rfcdata);
	eglApiData.Init(EglRFC::EeglInitialize);
	eglApiData.AppendEGLDisplay(aDisplay);
	EGLBoolean ret = aThreadState.ExecEglBooleanCmd(eglApiData);

	if(ret)
		{
		if(aMajor)
			*aMajor = EGL_VERSION_MAJOR;
		if(aMinor)
			*aMinor = EGL_VERSION_MINOR;
		}
	EGL_TRACE("eglInitialize <-" );
	return ret;
	}

/*
 Returns EGL_FALSE on failure and value is not updated. If attribute is not a
 valid EGL surface attribute, then an EGL_BAD_ATTRIBUTE error is generated. If
 surface is not a valid EGLSurface then an EGL_BAD_SURFACE error is generated.
 */
EGLBoolean CGuestEGL::eglQuerySurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLint aAttribute, EGLint *aValue)
    {
	// ToDo more parameter validation, and possibly use cached values
	if ( (aValue == NULL) || (3 & (TUint32)aValue) )
		{
		aThreadState.SetEglError(EGL_BAD_PARAMETER);
		return EGL_FALSE;
		}
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglQuerySurface);
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLSurface(aSurface);
	eglApiData.AppendEGLint(aAttribute);
	eglApiData.AppendEGLintVector(aValue, 1, RemoteFunctionCallData::EOut);

	EGLBoolean result = aThreadState.ExecEglBooleanCmd(eglApiData);
	EGL_TRACE_GET_ATTRIB("eglQuerySurface", "surface", aDisplay, aSurface, aAttribute, aValue, result);
	return result;
    }

/*
 On failure eglCreatePbufferFromClientBuffer returns EGL_NO_SURFACE. In
 addition to the errors described eglCreatePbufferSurface,
 eglCreatePbufferFromClientBuffer may fail and generate errors for the
 following reasons:
 * If buftype is not a recognized client API resource type (e.g. is not
   EGL_OPENVG_IMAGE), an EGL_BAD_PARAMETER error is generated.
 * If buffer is not a valid handle or name of a client API resource of the
   specified buftype in the currently bound context corresponding to that
   type, an EGL_BAD_PARAMETER error is generated.
 * If the buffers contained in buffer do not correspond to a proper subset
   of the buffers described by config, and match the bit depths for those
   buffers specified in config, then an EGL_BAD_MATCH error is generated.
   For example, a VGImage with pixel format VG_lRGBA_8888 corresponds to an
   EGLConfig with EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE, and
   EGL_ALPHA_SIZE values of 8.
 * If no context corresponding to the specified buftype is current, an
   EGL_BAD_ACCESS error is generated.
 * There may be additional constraints on which types of buffers may be
   bound to EGL surfaces, as described in client API specifications. If
   those constraints are violated, then an EGL_BAD_MATCH error is generated.
 * If buffer is already bound to another pbuffer, or is in use by a client
   API an EGL_BAD_ACCESS error is generated.
 */
EGLSurface CGuestEGL::eglCreatePbufferFromClientBuffer(TEglThreadState& aThreadState,
	      EGLDisplay aDisplay, EGLenum aBufType, EGLClientBuffer aBuffer, EGLConfig aConfig, const EGLint *aAttribList)
	{
	// ToDo validate parameters
	// ToDo SgImage Design Spec 5.12 - eglCreatePbufferFromClientBuffer should fail with EGL_BAD_ACCESS for VgImages derived from SgImages
    EGL_TRACE("eglCreatePbufferFromClientBuffer %d %d %d", aDisplay, aBufType, aConfig);
    RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
    eglApiData.Init(EglRFC::EeglCreatePbufferFromClientBuffer);
    eglApiData.AppendEGLDisplay(aDisplay);
    eglApiData.AppendEGLenum(aBufType);
    eglApiData.AppendEGLClientBuffer(aBuffer);
    eglApiData.AppendEGLConfig(aConfig);
    eglApiData.AppendEGLintVector(aAttribList, TAttribUtils::AttribListLength(aAttribList) );

    return aThreadState.ExecEglSurfaceCmd(eglApiData);
	}

/*
 If the dpy argument does not refer to a valid EGLDisplay, EGL_FALSE is
 returned, and an EGL_BAD_DISPLAY error is generated.
 */
EGLBoolean CGuestEGL::eglTerminate(TEglThreadState& aThreadState, EGLDisplay aDisplay)
	{
	EGLBoolean success = EGL_FALSE;
	if (DestroyDisplayInfo(aDisplay))
		{
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init(EglRFC::EeglTerminate);
		eglApiData.AppendEGLDisplay(aDisplay);

		success = aThreadState.ExecEglBooleanCmd(eglApiData);
		EGL_TRACE("eglTerminate display=%d ret=%d", aDisplay, success);
		}
	else
		{
		aThreadState.SetEglError(EGL_BAD_DISPLAY);
		}

	return success;
	}

/*
 On failure, EGL_FALSE is returned. An EGL_NOT_INITIALIZED error is generated
 if EGL is not initialized on dpy. An EGL_BAD_PARAMETER error is generated
 if num config is NULL.
 */
EGLBoolean CGuestEGL::eglGetConfigs(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig *aConfigs,
			 EGLint aConfigSize, EGLint *aNumConfig)
	{
	EGLint error = EGL_SUCCESS;
	
	if (aNumConfig == NULL)
		{
		error = EGL_BAD_PARAMETER;
		}

	// ToDo check display is valid 
	if (error != EGL_SUCCESS)
		{
		aThreadState.SetEglError(EGL_BAD_PARAMETER);
		return EGL_FALSE;
		}
	if (aConfigs && !aConfigSize)
		aConfigSize = 1;
	else if (!aConfigs)
		aConfigSize = 0;

	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglGetConfigs);
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLConfigVector(aConfigs, aConfigSize, RemoteFunctionCallData::EOut);
	eglApiData.AppendEGLint(aConfigSize);
	eglApiData.AppendEGLintVector(aNumConfig, 1, RemoteFunctionCallData::EOut);

	// ToDo cache successful result for next time 
	EGLBoolean result = aThreadState.ExecEglBooleanCmd(eglApiData);
	return result;
	}

EGLBoolean CGuestEGL::eglChooseConfig(TEglThreadState& aThreadState, EGLDisplay aDisplay, const EGLint *aAttribList,
				EGLConfig *aConfigs, EGLint aConfigSize, EGLint *aNumConfig)
	{
	if (aConfigs && !aConfigSize)
		{
		aConfigSize = 1;
		}
	else if (!aConfigs)
		{
		aConfigSize = 0;
		}

	const EGLint* surfaceType = TAttribUtils::FindAttribValue(aAttribList, EGL_SURFACE_TYPE);
	if (surfaceType && (*surfaceType & EGL_PIXMAP_BIT))
		{
		EGL_TRACE("CGuestEGL::eglChooseConfig for Pixmap Surface");
		const EGLint* pixmapMatch = TAttribUtils::FindAttribValue(aAttribList, EGL_MATCH_NATIVE_PIXMAP);
		if (pixmapMatch && *pixmapMatch)
			{
			EGL_TRACE("CGuestEGL::eglChooseConfig attributes include EGL_MATCH_NATIVE_PIXMAP, value is 0x%x", *pixmapMatch);
			// check/copy color & alpha sizes from pixmap
			return ChooseConfigForPixmapSurface(aThreadState, aDisplay, aAttribList, aConfigs, aConfigSize, aNumConfig, (const void*)*pixmapMatch);
			}
		else
			{ // Pixmap to match is missing
			EGL_TRACE("CGuestEGL::eglChooseConfig EGL_MATCH_NATIVE_PIXMAP attribute is missing or NULL");
			aThreadState.SetEglError(EGL_BAD_PARAMETER);
			return EGL_FALSE;
			}
		}

	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init( EglRFC::EeglChooseConfig );
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLintVector(aAttribList, TAttribUtils::AttribListLength(aAttribList));
	eglApiData.AppendEGLConfigVector(aConfigs, aConfigSize, RemoteFunctionCallData::EOut );
	eglApiData.AppendEGLint(aConfigSize);
	eglApiData.AppendEGLintVector(aNumConfig, 1, RemoteFunctionCallData::EOut);

	return aThreadState.ExecEglBooleanCmd(eglApiData); 
	}

/*
 On failure returns EGL_FALSE. If attribute
 is not a valid attribute then EGL_BAD_ATTRIBUTE is generated.
 */
EGLBoolean CGuestEGL::eglGetConfigAttrib(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig,
					EGLint aAttribute, EGLint *aValue)
	{
	// ToDo validate display & aAttribute, and maybe get result from local cache
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglGetConfigAttrib);
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLConfig(aConfig);
	eglApiData.AppendEGLint(aAttribute);
	eglApiData.AppendEGLintVector(aValue, 1, RemoteFunctionCallData::EOut);

	EGLBoolean result = aThreadState.ExecEglBooleanCmd(eglApiData);
	EGL_TRACE_GET_ATTRIB("eglGetConfigAttrib", "config", aDisplay, aConfig, aAttribute, aValue, result);
	return result;
	}

/*
 Returns EGL_FALSE on failure and value is not updated. If attribute is not a
 valid EGL surface attribute, then an EGL_BAD_ATTRIBUTE error is generated. If
 surface is not a valid EGLSurface then an EGL_BAD_SURFACE error is generated.
 */
EGLBoolean CGuestEGL::eglSurfaceAttrib(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface,
			    EGLint aAttribute, EGLint aValue)
	{
	EGL_TRACE_SET_ATTRIB("eglSurfaceAttrib", "surface", aDisplay, aSurface, aAttribute, aValue);

	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglSurfaceAttrib);
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLSurface(aSurface);
	eglApiData.AppendEGLint(aAttribute);
	eglApiData.AppendEGLint(aValue);
	return aThreadState.ExecEglBooleanCmd(eglApiData);
	}

/*
 If eglBindTexImage is called and the surface attribute EGL_TEXTURE_FORMAT is set
 to EGL_NO_TEXTURE, then an EGL_BAD_MATCH error is returned. If buffer is already
 bound to a texture then an EGL_BAD_ACCESS error is returned. If buffer is not a
 valid buffer, then an EGL_BAD_PARAMETER error is generated. If surface is not a
 valid EGLSurface, or is not a pbuffer surface supporting texture
 binding, then an EGL_BAD_SURFACE error is generated.
 */
EGLBoolean CGuestEGL::eglBindTexImage(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLint aBuffer)
	{
	// ToDo validate parameters
	if ( (aBuffer == NULL) || (3 & (TUint)aBuffer))
		{
		aThreadState.SetEglError(EGL_BAD_PARAMETER);
		return EGL_FALSE;
		}
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglBindTexImage);
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLSurface(aSurface);
	eglApiData.AppendEGLint(aBuffer);
	return aThreadState.ExecEglBooleanCmd(eglApiData);
	}

/*
 If the value of surface attribute EGL_TEXTURE_FORMAT is EGL_NO_TEXTURE,
 then an EGL_BAD_MATCH error is returned. If buffer is not a valid buffer
 (currently only EGL_BACK_BUFFER may  be specified), then an
 EGL_BAD_PARAMETER error is generated. If surface is not a valid EGLSurface,
 or is not a bound pbuffer surface, then an EGL_BAD_SURFACE error is
 returned.
 */
EGLBoolean CGuestEGL::eglReleaseTexImage(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLint aBuffer)
	{
	// ToDo validate parameters
	if ( (aBuffer == NULL) || (3 & (TUint)aBuffer))
		{
		aThreadState.SetEglError(EGL_BAD_PARAMETER);
		return EGL_FALSE;
		}
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglReleaseTexImage);
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLSurface(aSurface);
	eglApiData.AppendEGLint(aBuffer);
	return aThreadState.ExecEglBooleanCmd(eglApiData);
	}

/*
 Returns EGL_FALSE on failure. If there is no current context on the calling
 thread, a EGL_BAD_CONTEXT error is generated. If there is no surface bound
 to the current context, a EGL_BAD_SURFACE error is generated.
 */
EGLBoolean CGuestEGL::eglSwapInterval(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLint aInterval)
	{
	// ToDo validate parameters
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglSwapInterval);
	eglApiData.AppendEGLDisplay(aDisplay);
	eglApiData.AppendEGLint(aInterval);
	return aThreadState.ExecEglBooleanCmd(eglApiData);
	}

/*
 On failure returns EGL_NO_CONTEXT. If the current rendering api is EGL_NONE,
 then an EGL_BAD_MATCH error is generated (this situation can only arise in
 an implementation which does not support OpenGL ES, and prior to the first
 call to eglBindAPI). If share context is neither zero nor a valid context
 of the same client API type as the newly created context, then an EGL_-
 BAD_CONTEXT error is generated.

 ...
 */
EGLContext CGuestEGL::eglCreateContext(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig,
				EGLContext aShareContext, const EGLint *aAttribList)
	{
	EGLContext result = EGL_NO_CONTEXT;
    iDisplayMapLock.ReadLock();
    CEglDisplayInfo** pDispInfo = iDisplayMap.Find(aDisplay);
    if (!pDispInfo || !*pDispInfo)
        {
        EGL_TRACE("cannot find display %d", aDisplay);
        }
	else
		{
		CEglContext* shareContext = NULL;
		if (aShareContext)
			{
			CEglContext** pContext = (*pDispInfo)->iContextMap.Find(aShareContext);
			if (!pContext || (*pContext)->IsDestroyed())
				{
				EGL_TRACE("cannot find share context %d for display %d, or it is destroyed", aShareContext, aDisplay);
				aThreadState.SetEglError(EGL_BAD_CONTEXT);
				iDisplayMapLock.Unlock();
				return EGL_NO_CONTEXT;
				}
			}

		CEglContext* newContext = CEglContext::Create(aThreadState, aDisplay, aConfig, shareContext, aAttribList);
		if (newContext)
			{
			RHeap* threadHeap = CVghwUtils::SwitchToVghwHeap();
			TInt err = (*pDispInfo)->iContextMap.Insert(newContext->ClientContext(), newContext);
			CVghwUtils::SwitchFromVghwHeap(threadHeap);
			if (err != KErrNone)
				{
				newContext->Destroy(aThreadState);
				aThreadState.SetEglError(EGL_BAD_ALLOC);
				}
			else
				{
				result = newContext->ClientContext();
				}
			}
		}

	iDisplayMapLock.Unlock();
	return result;
	}

/*
 Returns EGL_FALSE on failure. An EGL_BAD_CONTEXT error is generated if ctx is
 not a valid context.
 */
EGLBoolean CGuestEGL::eglDestroyContext(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLContext aContext)
	{
	EGLBoolean success = EGL_FALSE;
    iDisplayMapLock.ReadLock();
    CEglDisplayInfo** pDispInfo = iDisplayMap.Find(aDisplay);
    if (!pDispInfo || !*pDispInfo)
        {
        EGL_TRACE("cannot find display %d", aDisplay);
		aThreadState.SetEglError(EGL_BAD_DISPLAY);
        }
	else
		{
		CEglContext** pContext = (*pDispInfo)->iContextMap.Find(aContext);
		if (!pContext || (*pContext)->IsDestroyed())
			{
			EGL_TRACE("cannot find context %d for display %d, or it is destroyed", aContext, aDisplay);
			aThreadState.SetEglError(EGL_BAD_CONTEXT);
			}
		else
			{
			success = EGL_TRUE;
			if ((*pContext)->Destroy(aThreadState))
				{
				RHeap* threadHeap = CVghwUtils::SwitchToVghwHeap();
				(*pDispInfo)->iContextMap.Remove(aContext);
				CVghwUtils::SwitchFromVghwHeap(threadHeap);
				}
			EGL_TRACE("eglDestroyContext end");
			}
		}

	iDisplayMapLock.Unlock();
	return success;
	}

/*
 Returns EGL_FALSE on failure and value is not updated. If attribute is not
 a valid EGL context attribute, then an EGL_BAD_ATTRIBUTE error is generated.
 If ctx is invalid, an EGL_BAD_CONTEXT error is generated.
 */
EGLBoolean CGuestEGL::eglQueryContext(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLContext aContext, EGLint aAttribute, EGLint *aValue)
	{
	EGLBoolean success = EGL_FALSE;
    iDisplayMapLock.ReadLock();
    CEglDisplayInfo** pDispInfo = iDisplayMap.Find(aDisplay);
    if (!pDispInfo || !*pDispInfo)
        {
        EGL_TRACE("cannot find display %d", aDisplay);
		aThreadState.SetEglError(EGL_BAD_DISPLAY);
        }
	else
		{
		CEglContext** pContext = (*pDispInfo)->iContextMap.Find(aContext);
		if (!pContext || (*pContext)->IsDestroyed())
			{
			EGL_TRACE("cannot find context %d for display %d, or it is destroyed", aContext, aDisplay);
			aThreadState.SetEglError(EGL_BAD_CONTEXT);
			}
		else
			{
			success = (*pContext)->QueryAttribute(aThreadState, aAttribute, aValue);
			}
		}

	iDisplayMapLock.Unlock();
	return success;
	}

/*
 On failure returns EGL_NO_SURFACE. If the pbuffer could not be created due
 to insufficient resources, then an EGL_BAD_ALLOC error is generated. If
 config is not a valid EGLConfig, an EGL_BAD_CONFIG error is generated. If
 the value specified for either EGL_WIDTH or EGL_HEIGHT is less than zero,
 an EGL_BAD_PARAMETER error is generated. If config does not support
 pbuffers, an EGL_BAD_MATCH error is generated.
 
 ... see also comment on exported API
 */
EGLSurface CGuestEGL::eglCreatePbufferSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, const EGLint *aAttribList)
    {
	// ToDo validate parameters
	// ToDo store info about surface
    RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
    eglApiData.Init(EglRFC::EeglCreatePbufferSurface);
    eglApiData.AppendEGLDisplay(aDisplay);
    eglApiData.AppendEGLConfig(aConfig);
    eglApiData.AppendEGLintVector(aAttribList, TAttribUtils::AttribListLength(aAttribList) );
    return aThreadState.ExecEglBooleanCmd(eglApiData);
    }

/*
 If there is no current context for the current rendering API, or if the
 current rendering API is EGL_NONE, then EGL_NO_CONTEXT is returned (this
 is not an error).
 */
EGLContext CGuestEGL::eglGetCurrentContext(TEglThreadState& aThreadState)
	{
	// ToDo should have this information cached
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglGetCurrentContext);
	return ExecEglContextNoErrorCmd(aThreadState, eglApiData);
	}

/**
 * 
 * 
 * @param aConfigs handle to an EGLConfig pointer, where the new configuration data will be. Ownership is transferred to the client.
 * @param aConfigsCnt number of configs that will be in aConfigs upon returning
 * 
 * @param aConfigAttribs handle to a pointer, where the memory will be allocated, and data copied. This will contain a list of
 *          config attribute _values_, in the order set out in eglrfc::MetaGetConfigAttribute().
 *          Format: c1attrval1, c1attrval2, ..., c2attrval1, c2attrval2, ... cNattrvalM
 *          (the number of attribute values per config is eglrfc::MetaGetConfigAttributeCnt()
 *          Ownership is transferred to the client.
 * @param aConfigAttribsLen handle to an integer value, where the size of the above vector will be stored.
 * @param aFetchMode: which configs to fetch, default = EMetaGetConfigsSg (get configs that support sgImage), possible values are
 *              EMetaGetConfigsAll,   //get all the configs available
 *              EMetaGetConfigsSg,    //get configs supporting sg images
 *              EMetaGetConfigsNonSg, //get configs not supporting sg images
 *              (only sgImage implemented!)
 * 
 */
TBool CGuestEGL::EglInternalFunction_MetaGetConfigs(TEglThreadState& aThreadState, EGLConfig*& aConfigs, EGLint& aConfigCnt, EGLint*& aConfigAttribs, EGLint& aConfigAttribsLen, TMetaGetConfigsMode aFetchMode )
    {
    aConfigCnt = KConfigsMaxCnt;
	//TODO: optimize this so that the length is the number of returned items
    aConfigs = (EGLConfig*) CVghwUtils::Alloc( sizeof(EGLConfig) * aConfigCnt );
    aConfigAttribsLen = KConfigsMaxCnt * 29;
	//TODO: optimize this so that the length is the number of returned items
    aConfigAttribs = (EGLint*) CVghwUtils::Alloc( sizeof(EGLint) * aConfigAttribsLen );

    RemoteFunctionCallData rfcdata; EglRFC call( rfcdata );
    call.Init( EglRFC::EeglMetaGetConfigs );
    call.AppendEGLintVector( aConfigs, aConfigCnt, RemoteFunctionCallData::EInOut );
    call.AppendEGLintVector( aConfigAttribs, aConfigAttribsLen, RemoteFunctionCallData::EInOut );

    call.AppendEGLint( (EGLint)aFetchMode );

    return aThreadState.ExecEglBooleanCmd( call );
    }


// Private interfaces for EGL to call into Open VG & Open GL ES 
// class MVgApiForEgl - redirects via CVghwUtils to exported functions from Open VG
ExtensionProcPointer CGuestEGL::guestGetVgProcAddress (const char *aProcName)
	{
	if (!iVgApiForEgl)
		{
		iVgApiForEgl = CVghwUtils::VgApiForEgl();
		}
	if (iVgApiForEgl)
		{
		return iVgApiForEgl->guestGetVgProcAddress(aProcName);
		}
	return NULL;
	}


// class MGles11ApiForEgl - redirects via CVghwUtils to exported functions from Open GL ES 1.1
ExtensionProcPointer CGuestEGL::guestGetGles11ProcAddress (const char *aProcName)
	{
	if (!iGles11ApiForEgl)
		{
		iGles11ApiForEgl = CVghwUtils::Gles11ApiForEgl();
		}
	if (iGles11ApiForEgl)
		{
		return iGles11ApiForEgl->guestGetGles11ProcAddress(aProcName);
		}
	return NULL;
	}


// class MGles2ApiForEgl - redirects via CVghwUtils to exported functions from Open GL ES 2
ExtensionProcPointer CGuestEGL::guestGetGles2ProcAddress (const char *aProcName)
	{
	if (!iGles2ApiForEgl)
		{
		iGles2ApiForEgl = CVghwUtils::Gles2ApiForEgl();
		}
	if (iGles2ApiForEgl)
		{
		return iGles2ApiForEgl->guestGetGles2ProcAddress(aProcName);
		}
	return NULL;
	}
