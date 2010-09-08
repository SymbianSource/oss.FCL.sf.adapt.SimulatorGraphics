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
// Implementation of guest egl context

#include <e32debug.h>
#include "eglapi.h"



// factory function
CEglContext* CEglContext::Create(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, CEglContext* aShareContext, const EGLint* aAttribList)
	{
	// ToDo validate aConfig
	// ToDo validate aShareContext

	// which Khronos Graphics API is the Context for: Open GL ES, Open VG, ...
	EGLenum api = aThreadState.EglBoundApi();
	// (in case supported APIs ever change) ensure the current API is valid 
	if (api == EGL_NONE)
		{
		aThreadState.SetEglError(EGL_BAD_MATCH);
		return EGL_NO_CONTEXT;
		}

	EGLint glesClientVersion = 1;
	// validate AttribList parameter
	if ( aAttribList && (*aAttribList != EGL_NONE) )
		{
		TBool attribsOkay = EFalse;
		// As of EGL 1.4 only Open GL ES api supports any attributes for eglCreateContext
		if (api == EGL_OPENGL_ES_API)
			{
			// only supported attribute for Open GL ES is EGL_CONTEXT_CLIENT_VERSION
			if ( (aAttribList[0] == EGL_CONTEXT_CLIENT_VERSION) && (aAttribList[2] == EGL_NONE) )
				{
				glesClientVersion = aAttribList[1];
				// modify this code when GL ES 2 support is added
				if (glesClientVersion == 1)
					{
					attribsOkay = ETrue;
					}
				}
			}
		if (!attribsOkay)
			{
			aThreadState.SetEglError(EGL_BAD_PARAMETER);
			return EGL_NO_CONTEXT;
			}
		}

	// ToDo validate that aConfig supports this API (inc GL ES v2 versus v1)
	EGLContext shareContextFamily = EGL_NO_CONTEXT;
	if (aShareContext)
		{
		shareContextFamily = aShareContext->ShareContextFamily();
		}
	RHeap* threadHeap = CVghwUtils::SwitchToVghwHeap();
	CEglContext* self = new CEglContext(aDisplay, aConfig, shareContextFamily, api, glesClientVersion);
	if (self == EGL_NO_CONTEXT)
		{
		aThreadState.SetEglError(EGL_BAD_ALLOC);
		}
	else
		{
		// call Host EGL, & set iHostContext
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init(EglRFC::EeglCreateContext);
		eglApiData.AppendEGLDisplay(aDisplay);
		eglApiData.AppendEGLConfig(aConfig);
		eglApiData.AppendEGLContext(aShareContext ? aShareContext->HostContext() : EGL_NO_CONTEXT);
		eglApiData.AppendEGLintVector(aAttribList, TAttribUtils::AttribListLength(aAttribList));
		self->iHostContext = aThreadState.ExecEglContextCmd(eglApiData);
		if (self->iHostContext == EGL_NO_CONTEXT)
			{ // Host EGL error
			delete self;
			self = EGL_NO_CONTEXT;
			}
		}
	CVghwUtils::SwitchFromVghwHeap(threadHeap);
	return self;
	}


CEglContext::CEglContext(EGLDisplay aDisplay, EGLConfig aConfig, EGLContext aShareContextFamily, EGLenum aRenderingApi,
			EGLint aGlesClientVersion) :
		iFirstUse(ETrue), iHostContext(EGL_NO_CONTEXT), iDisplay(aDisplay), iShareContextFamily(aShareContextFamily),
		iConfigId(aConfig), iRenderingApi(aRenderingApi), iDrawSurface(EGL_NO_SURFACE), iReadSurface(EGL_NO_SURFACE),
		iGlesClientVersion(aGlesClientVersion), iIsDestroyed(EFalse)
	{
	iCtxMutex.CreateLocal(EOwnerProcess);
	}


void CEglContext::Delete(TEglThreadState& aThreadState)
	{
	EGLPANIC_ASSERT_DEBUG(iCtxMutex.IsHeld(), EEglPanicTemp);

	// tell Host EGL to destroy the Context now
	RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
	eglApiData.Init(EglRFC::EeglDestroyContext);
	eglApiData.AppendEGLDisplay(iDisplay);
	eglApiData.AppendEGLContext(iHostContext);
	EGLBoolean hostResult = aThreadState.ExecEglBooleanCmd(eglApiData);
	EGLPANIC_ASSERT_DEBUG(hostResult,  EEglPanicTemp);

	// free
	iCtxMutex.Close();
	RHeap* threadHeap = CVghwUtils::SwitchToVghwHeap();
	delete this;
	CVghwUtils::SwitchFromVghwHeap(threadHeap);
	}


CEglContext::~CEglContext()
	{
	if (iCtxMutex.Handle())
		{
	iCtxMutex.Close();
		}
	}


EGLBoolean CEglContext::MakeCurrent(TEglThreadState& aThreadState, EGLSurface aDraw, EGLSurface aRead)
	{
	// ToDo support different number spaces for Host & Client EGL Surfaces 
	EGLBoolean hostResult = EGL_FALSE;
	EGLint error = EGL_SUCCESS;
	iCtxMutex.Wait();
	if (iIsDestroyed)
		{
		error = EGL_BAD_CONTEXT;
		}
	else
		{
		// ToDo validate aDraw & aRead are compatible with API
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init( EglRFC::EeglMakeCurrent );
		eglApiData.AppendEGLDisplay(iDisplay);
		eglApiData.AppendEGLSurface(aDraw);
		eglApiData.AppendEGLSurface(aRead);
		eglApiData.AppendEGLContext(iHostContext);
		hostResult = aThreadState.ExecEglBooleanCmd(eglApiData);
		if (hostResult)
			{
			iDrawSurface = aDraw;
			iReadSurface = aRead;
			}
		}
	iCtxMutex.Signal();
	if (error != EGL_SUCCESS)
		{ // error in parameter checks 
		aThreadState.SetEglError(error);
		}
	return hostResult;
	}


TBool CEglContext::Destroy(TEglThreadState& aThreadState)
	{
	iCtxMutex.Wait();
	iIsDestroyed = ETrue;
	if ( (iDrawSurface == EGL_NO_SURFACE) && (iReadSurface == EGL_NO_SURFACE) )
		{ // Context not in use
		Delete(aThreadState);
		return ETrue;
		}
	iCtxMutex.Signal();
	return EFalse;
	}


TBool CEglContext::MakeNotCurrent(TEglThreadState& aThreadState)
	{
	iCtxMutex.Wait();
	iDrawSurface = EGL_NO_SURFACE;
	iReadSurface = EGL_NO_SURFACE;

	if (iIsDestroyed)
		{ // Destroyed & no longer in use
		Delete(aThreadState);
		return ETrue;
		}
	iCtxMutex.Signal();
	return EFalse;
	}


EGLContext CEglContext::ShareContextFamily()
	{
	EGLContext result = EGL_NO_CONTEXT;
	// ToDo review - maybe just check Mutex is held
	iCtxMutex.Wait();
	if (!iIsDestroyed)
		{
		if (iShareContextFamily)
			{
			result = iShareContextFamily;
			}
		else
			{
			result = iHostContext;
			}
		}
	iCtxMutex.Signal();
	return result;
	}

// Valid attributes are: EGL_CONFIG_ID, EGL_CONTEXT_CLIENT_TYPE, EGL_CONTEXT_CLIENT_VERSION, or EGL_RENDER_BUFFER
EGLBoolean CEglContext::QueryAttribute(TEglThreadState& aThreadState, EGLint aAttribute, EGLint* aValue)
	{
	EGLint error = EGL_SUCCESS;
	iCtxMutex.Wait();
	if (iIsDestroyed)
		{
		error = EGL_BAD_CONTEXT;
		}
	else
		{
		switch (aAttribute)
			{
			case EGL_CONFIG_ID:
				*aValue = iConfigId;
				break;
	
			case EGL_CONTEXT_CLIENT_TYPE:
				*aValue = iRenderingApi;
				break;
	
			case EGL_CONTEXT_CLIENT_VERSION:
				*aValue = iGlesClientVersion;
				break;
	
			case EGL_RENDER_BUFFER:
				if (iDrawSurface == NULL)
					{ // context is not bound to any surface
					*aValue = EGL_NONE;
					}
				else
					// ToDo check the currently bound surface type to answer this
					{
					RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
					eglApiData.Init(EglRFC::EeglQueryContext);
					eglApiData.AppendEGLDisplay(iDisplay);
					eglApiData.AppendEGLContext(iHostContext);
					eglApiData.AppendEGLint(aAttribute);
					eglApiData.AppendEGLintVector(aValue, 1, RemoteFunctionCallData::EOut);
					EGLBoolean hostResult = aThreadState.ExecEglBooleanCmd(eglApiData);
					return hostResult;
					}
				break;
	
			default:
				error = EGL_BAD_ATTRIBUTE;
				break;
			}
		// Debug compare with Host EGL result
#ifdef _DEBUG
		EGLint hostValue = 0;
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init(EglRFC::EeglQueryContext);
		eglApiData.AppendEGLDisplay(iDisplay);
		eglApiData.AppendEGLContext(iHostContext);
		eglApiData.AppendEGLint(aAttribute);
		eglApiData.AppendEGLintVector(&hostValue, 1, RemoteFunctionCallData::EOut);
		EGLBoolean hostResult = aThreadState.ExecEglBooleanCmd(eglApiData);
		if (error == EGL_SUCCESS)
			{ // Host EGl result should match ours
			EGLPANIC_ASSERT(hostResult,  EEglPanicTemp);
			EGLPANIC_ASSERT(hostValue == *aValue, EEglPanicTemp);
			}
		else
			{
			EGLPANIC_ASSERT(!hostResult,  EEglPanicTemp);		
			}
#endif
		}
	aThreadState.SetEglError(error);
	iCtxMutex.Signal();
	return (error == EGL_SUCCESS);
	}

// end of file eglcontext.cpp
