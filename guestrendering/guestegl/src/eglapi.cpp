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
// Implementation of guest egl stub functions

#include "eglapi.h"
#include <e32atomics.h>

/*
 Only having the pointer as "Writeable Static Data" makes it very easy to tell if
 initialisation has been done: the pointer is not NULL!
 */
CGuestEGL* guestEGL;
// Writeable Static Data - causes constructor to be called at load DLL time.
XGuestEglInitialiser GuestEglInitialiser;



XGuestEglInitialiser::XGuestEglInitialiser()
	{
	EGL_TRACE("XGuestEglInitialiser::XGuestEglInitialiser() - guestEGL=0x%x", guestEGL);
	if (!guestEGL)
		{
		// ensure VGHW Memory heap is created & channel to LDD is opened
		CVghwUtils::InitStatics();
		// Multi-Threading safe creation of CGuestEGL instance
		if (!guestEGL)
			{
			// guarantee that initialisation of the object is flushed to memory before the pointer is published
			__e32_atomic_store_rel_ptr(&guestEGL, CGuestEGL::New());
			}
	
		// cast away volatile attribute
		CGuestEGL* instance = (CGuestEGL*)guestEGL;
		// cannot continue if alloc failed
		EGLPANIC_ASSERT(guestEGL, EEglPanicCGuestEGLAllocFailed);
		CVghwUtils::SetEglManagementApi(instance);
	
		CVghwUtils::CreateThreadState();
		}
	}


XGuestEglInitialiser::~XGuestEglInitialiser()
	{
	EGL_TRACE("XGuestEglInitialiser::~XGuestEglInitialiser() - guestEGL=0x%x", guestEGL);
	if (guestEGL)
		{
		delete guestEGL;
		guestEGL = NULL;
		}
	CVghwUtils::DestroyStatics();
	}


_LIT(KEglPanicCategory, "Guest EGL");

void EglPanic(TEglPanic aPanicCode, char* aPanicName, char* aCondition, char* aFile, TInt aLine)
	{
	if (aPanicName && aCondition && aFile)
		{
		RDebug::Printf("Guest EGL DLL Panic %s for failed Assert (%s),\n\tat %s:%d", aPanicName, aCondition, aFile, aLine);
		}
	else if (aPanicName && aFile)
		{
		RDebug::Printf("Guest EGL DLL Panic %s at %s:%d", aPanicName, aFile, aLine);
		}
	else
		{
		RDebug::Printf("Guest EGL DLL Panic %d (line %d)", aPanicCode, aLine);
		}

	User::Panic(KEglPanicCategory, aPanicCode);
	}


extern "C" {
/*
 Note: Comments at the start of each EGL api are adapted from the Khronos EGL 1.4 specification.
 The text has been chosen/adapted to give a helpful overview of the function, and the errors
 that it may generate.  For more details see the full Khronos EGL specification.
 */


/*
 Get details of the last EGL api error in this thread.

 Returns EGL_SUCCESS or an EGL_xxxx error constant.
 */
EXPORT_C EGLint eglGetError(void)
    {
	EGL_TRACE("eglGetError -->");

	EGLint eglError = EGL_SUCCESS;
	// threadState is non-null if an EGL api has been called in this thread
	TEglThreadState* threadState = CVghwUtils::EglThreadState(); // do not create thread object for this API
	if (threadState)
		{ // get error (may be from parameter checking in this DLL or from Host EGL) - fetching resets error to EGL_SUCCESS
		eglError = threadState->EglError();
		}
	EGL_TRACE("eglGetError returning eglError=0x%x <--", eglError);
	return eglError;
    }

/*
 Get display handle for display with requested properties.

 If display id is EGL_DEFAULT_DISPLAY, a default display is returned.
 Multiple calls made to eglGetDisplay with the same display-id will
 all return the same EGLDisplay handle.
 If no display matching display id is available, EGL_NO_DISPLAY is
 returned; no error condition is raised in this case.
 */
EXPORT_C EGLDisplay eglGetDisplay(EGLNativeDisplayType aDisplayId)
	{
	EGL_TRACE("eglGetDisplay Display Id=%d -->", aDisplayId);

	EGLDisplay display = EGL_NO_DISPLAY;
	// Most likely eglGetDisplay is the first API called, so try initialising EGL instance first
	CGuestEGL& instance = Instance();
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		display = instance.eglGetDisplay(*threadState, aDisplayId);
		}
	EGL_TRACE("eglGetDisplay result Display=%d <--", display);
	return display;
	}

/*
 Initialize EGL on a display.

 EGL_TRUE is returned on success, and major and minor are updated with the major
 and minor version numbers of the EGL implementation (for example, in an EGL
 1.2 implementation, the values of *major and *minor would be 1 and 2, respectively).
 major and minor are not updated if they are specified as NULL.
 EGL_FALSE is returned on failure and major and minor are not updated. An
 EGL_BAD_DISPLAY error is generated if the dpy argument does not refer to a valid
 EGLDisplay. An EGL_NOT_INITIALIZED error is generated if EGL cannot be
 initialized for an otherwise valid dpy.
 */
EXPORT_C EGLBoolean eglInitialize(EGLDisplay aDisplay, EGLint *aMajor, EGLint *aMinor)
	{
	EGL_TRACE("eglInitialize Display=%d, Major 0x%x, Minor 0x%x", aDisplay, aMajor, aMinor );

	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	EGLBoolean success = EGL_FALSE;
	if (threadState)
		{
		success = Instance().eglInitialize(*threadState, aDisplay, aMajor, aMinor);
		}
	EGL_TRACE("eglInitialize <- result=%d", success);
	return success;
	}

/*
 Marks all EGL-specific resources associated with the specified display
 for deletion.

 If the dpy argument does not refer to a valid EGLDisplay, EGL_FALSE is
 returned, and an EGL_BAD_DISPLAY error is generated.
 */
EXPORT_C EGLBoolean eglTerminate(EGLDisplay aDisplay)
	{
	EGL_TRACE("eglTerminate Display=%d", aDisplay);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglTerminate(*threadState, aDisplay);
		}
	return result;
	}

/*
 Returns a pointer to a static, zero-terminated string describing some aspect
 of the EGL implementation running on the specified display. name may be one
 of EGL_CLIENT_APIS, EGL_EXTENSIONS, EGL_VENDOR, or EGL_VERSION.

 The EGL_CLIENT_APIS string describes which client rendering APIs are
 supported. It is zero-terminated and contains a space-separated list of API
 names, which must include at least one of ‘‘OpenGL’’, ‘‘OpenGL_ES’’ or
 ‘‘OpenVG’’.

 The EGL_EXTENSIONS string describes which EGL extensions are supported
 by the EGL implementation running on the specified display, and for the
 current client API context. The string is zero terminated and contains a
 space-separated list of extension names;  extension names themselves do
 not contain spaces. If there are no extensions to EGL, then the empty string
 is returned.

 The format and contents of the EGL_VENDOR string is implementation dependent.

 The format of the EGL_VERSION string is:
 <major version.minor version><space><vendor specific info>

 On failure, NULL is returned. An EGL_NOT_INITIALIZED error is generated
 if EGL is not initialized for dpy. An EGL_BAD_PARAMETER error is generated if
 name is not one of the values described above.
 */
EXPORT_C const char* eglQueryString(EGLDisplay aDisplay, EGLint aName)
    {
    EGL_TRACE("eglQueryString Display=%d, Name=0x%x", aDisplay, aName);

    return Instance().eglQueryString(aDisplay, aName);
    }

/*
 Get the list of all EGLConfigs that are available on the specified display.

 On failure, EGL_FALSE is returned. An EGL_NOT_INITIALIZED error is generated
 if EGL is not initialized on dpy. An EGL_BAD_PARAMETER error is generated
 if num config is NULL.
 */
EXPORT_C EGLBoolean eglGetConfigs(EGLDisplay aDisplay, EGLConfig *aConfigs,
			 EGLint aConfigSize, EGLint *aNumConfig)
	{
	EGL_TRACE("eglGetConfigs Display=%d, ConfigSize=%d , NumConfig=0x%x -->", aDisplay, aConfigSize, aNumConfig);
	
	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglGetConfigs(*threadState, aDisplay, aConfigs, aConfigSize, aNumConfig);
		}
	EGL_TRACE("eglGetConfigs hostResult=%d, NumConfig-%d <--", result, aNumConfig ? *aNumConfig : 0);
	return result;
	}

/*
 Get EGLConfigs that match a list of attributes. The return value and the meaning
 of configs, config size, and num config are the same as for eglGetConfigs.
 However, only configurations matching attrib list will be returned.

 On failure, EGL_FALSE is returned. An EGL_BAD_ATTRIBUTE error is generated
 if attrib list contains an undefined EGL attribute or an attribute value that is
 unrecognized or out of range.
 */
EXPORT_C EGLBoolean eglChooseConfig(EGLDisplay aDisplay, const EGLint *aAttribList,
			   EGLConfig *aConfigs, EGLint aConfigSize, EGLint *aNumConfig)
	{
	EGL_TRACE("eglChooseConfig Display=%d, AttribList=0x%x, Configs=0x%x, ConfigSize=%d, NumConfig=0x%x -->",
			aDisplay, aAttribList, aConfigs, aConfigSize, aNumConfig);
	EGL_TRACE_ATTRIB_LIST(aAttribList);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglChooseConfig(*threadState, aDisplay, aAttribList, aConfigs, aConfigSize, aNumConfig);
		}

	EGL_TRACE("eglChooseConfig Result=%s, Num configs=%d, First config=%d <--",
			result ? "success" : "fail", 
			aNumConfig ? *aNumConfig : 0,
			(aConfigs && aNumConfig && (*aNumConfig > 0)) ? *aConfigs : -1);
	return result;
	}

/*
 Get the value of an EGLConfig attribute.

 On failure returns EGL_FALSE. If attribute
 is not a valid attribute then EGL_BAD_ATTRIBUTE is generated.
 */
EXPORT_C EGLBoolean eglGetConfigAttrib(EGLDisplay aDisplay, EGLConfig aConfig,
			      EGLint aAttribute, EGLint *aValue)
	{
	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglGetConfigAttrib(*threadState, aDisplay, aConfig, aAttribute, aValue);
		}
	return result;
	}

/*
 Creates an onscreen EGLSurface and returns a handle to it. Any EGL context created
 with a compatible EGLConfig can be used to render into this surface.

 On failure returns EGL_NO_SURFACE. If the attributes of win do not correspond to
 config, then an EGL_BAD_MATCH error is generated. If config does not support
 rendering to windows (the EGL_SURFACE_TYPE attribute does not contain
 EGL_WINDOW_BIT), an EGL_BAD_MATCH error is generated. If config does not support
 the colorspace or  alpha format attributes specified in attrib list (as defined
 for eglCreateWindowSurface), an EGL_BAD_MATCH error is generated. If config is
 not a valid EGLConfig, an EGL_BAD_CONFIG error is generated. If win is not a valid
 native window handle, then an EGL_BAD_NATIVE_WINDOW error should be generated. If
 there is already an EGLConfig associated with win (as a result of a previous
 eglCreateWindowSurface call), then an EGL_BAD_ALLOC error is generated. Finally,
 if the implementation cannot allocate resources for the new EGL window, an
 EGL_BAD_ALLOC error is generated.
 */
EXPORT_C EGLSurface eglCreateWindowSurface(EGLDisplay aDisplay, EGLConfig aConfig,
				  EGLNativeWindowType aWindow, const EGLint *aAttribList)
	{
	EGL_TRACE("eglCreateWindowSurface Display=%d, Config=%d, Window=0x%x -->", aDisplay, aConfig, aWindow);

	EGLSurface newSurface = EGL_NO_SURFACE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		newSurface = Instance().eglCreateWindowSurface(*threadState, aDisplay, aConfig, aWindow, aAttribList);
		}
	EGL_TRACE("eglCreateWindowSurface new Surface=0x%x <--", newSurface);
	return newSurface;
	}

/*
 Creates a single off-screen pbuffer surface and returns a handle to it.

 On failure returns EGL_NO_SURFACE. If the pbuffer could not be created due
 to insufficient resources, then an EGL_BAD_ALLOC error is generated. If
 config is not a valid EGLConfig, an EGL_BAD_CONFIG error is generated. If
 the value specified for either EGL_WIDTH or EGL_HEIGHT is less than zero,
 an EGL_BAD_PARAMETER error is generated. If config does not support
 pbuffers, an EGL_BAD_MATCH error is generated. In addition, an EGL_BAD_MATCH
 error is generated if any of the following conditions are true:
 * The EGL_TEXTURE_FORMAT attribute is not EGL_NO_TEXTURE, and EGL_WIDTH
   and/or EGL_HEIGHT specify an invalid size (e.g., the texture size is
   not a power of two, and the underlying OpenGL ES implementation does not
   support non-power-of-two textures).
 * The EGL_TEXTURE_FORMAT attribute is EGL_NO_TEXTURE, and EGL_TEXTURE_TARGET
   is something other than EGL_NO_TEXTURE; or, EGL_TEXTURE_FORMAT is
   something other than EGL_NO_TEXTURE, and EGL_TEXTURE_TARGET is
   EGL_NO_TEXTURE.
 Finally, an EGL_BAD_ATTRIBUTE error is generated if any of the EGL_-
 TEXTURE_FORMAT, EGL_TEXTURE_TARGET, or EGL_MIPMAP_TEXTURE attributes
 are specified, but config does not support OpenGL ES rendering (e.g.
 the EGL_RENDERABLE_TYPE attribute does not include at least one of EGL_-
 OPENGL_ES_BIT or EGL_OPENGL_ES2_BIT.
 */
EXPORT_C EGLSurface eglCreatePbufferSurface(EGLDisplay aDisplay, EGLConfig aConfig, const EGLint *aAttribList)
	{
	EGL_TRACE("eglCreatePbufferSurface Display=%d, Config=%d -->", aDisplay, aConfig);
	EGL_TRACE_ATTRIB_LIST(aAttribList);

	EGLSurface newSurface = EGL_NO_SURFACE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		newSurface = Instance().eglCreatePbufferSurface(*threadState, aDisplay, aConfig, aAttribList);
		}
	EGL_TRACE("eglCreatePbufferSurface new Surface=0x%x <--", newSurface);
	return newSurface;
	}

/*
 Creates an offscreen EGLSurface and returns a handle to it. Any EGL context
 created with a compatible EGLConfig can be used to render into this surface.

 To create a pixmap rendering surface, first create a native platform pixmap,
 then select an EGLConfig matching the pixel format of that pixmap (calling
 eglChooseConfig with an attribute list including EGL_MATCH_NATIVE_PIXMAP
 returns only EGLConfigs matching the pixmap specified in the attribute list).

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
EXPORT_C EGLSurface eglCreatePixmapSurface(EGLDisplay aDisplay, EGLConfig aConfig, EGLNativePixmapType aPixmap, const EGLint *aAttribList)
	{
	EGL_TRACE("eglCreatePixmapSurface Display=%d, Config=%d, Pixmap=0x%x -->", aDisplay, aConfig, aPixmap);

	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	EGLSurface newSurface = EGL_NO_SURFACE;
	if (threadState)
		{
		newSurface = Instance().eglCreatePixmapSurface(*threadState, aDisplay, aConfig, aPixmap, aAttribList);
		}
	EGL_TRACE("eglCreatePixmapSurface new Surface=0x%x <--", newSurface);
	return newSurface;
	}

/*
 Destroy an EGLSurface of any type (window, pbuffer, or pixmap).

 All resources associated with surface which were allocated by EGL are marked
 for deletion as soon as possible. Following eglDestroySurface, the surface
 and  the handle referring to it are treated in the same fashion as a surface
 destroyed by eglTerminate.

 Resources associated with surface but not allocated by EGL, such as native
 windows, native pixmaps, or client API buffers, are not affected when the
 surface is destroyed. Only storage actually allocated by EGL is marked for
 deletion. Furthermore, resources associated with a pbuffer surface are not
 released until all color buffers of that pbuffer bound to a OpenGL ES texture
 object have been released.

 Returns EGL_FALSE on failure. An EGL_BAD_SURFACE error is generated if surface
 is not a valid rendering surface.
 */
EXPORT_C EGLBoolean eglDestroySurface(EGLDisplay aDisplay, EGLSurface aSurface)
	{
	EGL_TRACE("eglDestroySurface Display=%d, Surface=0x%x", aDisplay, aSurface);

	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	EGLBoolean result = EGL_FALSE;
	if (threadState)
		{
		result = Instance().eglDestroySurface(*threadState, aDisplay, aSurface);
		}
	return result;
	}

/*
 Returns in value the value of attribute for surface. attribute must be set
 to one of the attributes in table 3.5 of the EGL specification.

 Returns EGL_FALSE on failure and value is not updated. If attribute is not a
 valid EGL surface attribute, then an EGL_BAD_ATTRIBUTE error is generated. If
 surface is not a valid EGLSurface then an EGL_BAD_SURFACE error is generated.
 */
EXPORT_C EGLBoolean eglQuerySurface(EGLDisplay aDisplay, EGLSurface aSurface, EGLint aAttribute, EGLint *aValue)
    {
    EGL_TRACE("eglQuerySurface display=%d, surface=%d, attribute=%d", aDisplay, aSurface, aAttribute);
    TEglThreadState* threadState = CVghwUtils::CreateThreadState();
    if (threadState)
    	{
		return Instance().eglQuerySurface(*threadState, aDisplay, aSurface, aAttribute, aValue);
    	}
    return EGL_FALSE;
    }

/*
 Set the current rendering API, this is set on a per-thread basis.

 api must specify one of the supported client APIs , either EGL_OPENVG_API
 or EGL_OPENGL_ES_API. (EGL_OPENGL_API is not currently supported by Symbian.)

 Returns EGL_FALSE on failure. If api is not one of the values specified
 above, or if the client API specified by api is not supported by the
 implementation, an EGL_BAD_PARAMETER error is generated.
 */
EXPORT_C EGLBoolean eglBindAPI(EGLenum aApi)
	{
	EGL_TRACE("eglBindAPI 0x%x (%s)", aApi, (aApi == EGL_OPENGL_ES_API) ? "EGL_OPENGL_ES_API" : (aApi == EGL_OPENVG_API) ? "EGL_OPENVG_API" : "???");

	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	EGLBoolean result = EGL_FALSE;
	if (threadState)
		{
		if ( (aApi == EGL_OPENGL_ES_API) || (aApi == EGL_OPENVG_API) )
			{
			threadState->SetEglBoundApi(aApi);
			result = EGL_TRUE;
			RemoteFunctionCallData rfcdata; EglRFC eglApiData(rfcdata);
			eglApiData.Init(EglRFC::EeglBindAPI);
			eglApiData.AppendEGLenum(aApi);
			EGLBoolean hostResult = threadState->ExecEglBooleanCmd(eglApiData);
			// confirm Host EGL matches what we think about the parameter
			EGLPANIC_ASSERT_DEBUG(hostResult == result, EEglPanicTemp);
			}
		else
			{
			threadState->SetEglError(EGL_BAD_PARAMETER);
			}
		}
	return result;
	}

/*
 Get the current rendering API, this is set on a per-thread basis.

 The value returned will be one of the valid api parameters to eglBindAPI,
 or EGL_NONE.

 The initial value of the current rendering API is EGL_OPENGL_ES_API.
 Applications using multiple client APIs are responsible for ensuring
 the current rendering API is correct before calling the functions
 eglCreateContext, eglGetCurrentContext, eglGetCurrentDisplay,
 eglGetCurrentSurface, eglMakeCurrent (when its ctx parameter is
 EGL_NO_CONTEXT), eglWaitClient, or eglWaitNative.
 */
EXPORT_C EGLenum eglQueryAPI(void)
	{
	EGL_TRACE("eglQueryAPI");

	EGLenum result = EGL_OPENGL_ES_API;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{ // EGL threadState exists
		result = threadState->EglBoundApi();
		threadState->SetEglError(EGL_SUCCESS);
#ifdef _DEBUG
		// Debug build checks that local threadState is in sync with Host EGL state
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init(EglRFC::EeglQueryAPI);
		threadState->ExecuteEglNeverErrorCmd(eglApiData);
		EGLPANIC_ASSERT(result == (EGLenum)eglApiData.ReturnValue(), EEglPanicHostAndClientBoundApiOutOfSync);
#endif
		}
	return result;
	}

/*
 Wait for client (Open GL ES, VG, ...) rendering to complete, before
 using Symbian native rendering.

 Returns EGL_TRUE on success. If there is no current context for  the current
 rendering API, the function has no effect but still returns EGL_TRUE. If the
 surface associated with the calling thread’s current context is no longer valid,
 EGL_FALSE is returned and an EGL_BAD_CURRENT_SURFACE error is generated.
 */
EXPORT_C EGLBoolean eglWaitClient(void)
    {
	EGL_TRACE("eglWaitClient");

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglWaitClient(*threadState);
		}
	return result;
    }

/*
 Return EGL to its state at thread initialization.

 EGL_TRUE is returned on success, and the following actions are taken:
 * For each client API supported by EGL, if there is a currently bound context,
   that context is released. This is equivalent to calling eglMakeCurrent
   with ctx set to EGL_NO_CONTEXT and both draw and read set to EGL_NO_SURFACE.
 * The current rendering API is reset to its value at thread initialization.
 * Any additional implementation-dependent per-thread state maintained by
   EGL is marked for deletion as soon as possible.
 Returns EGL_FALSE on failure, there are no defined conditions under which
 failure will occur.
 */
EXPORT_C EGLBoolean eglReleaseThread(void)
	{
	EGL_TRACE("eglReleaseThread -->");

	TEglThreadState* threadState = CVghwUtils::EglThreadState(); // fetching pre-existing thread state, if any
	if (threadState)
		{
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init(EglRFC::EeglReleaseThread);
	    threadState->ExecuteEglNeverErrorCmd(eglApiData);

	    // release client side memory
		CVghwUtils::ReleaseThreadState();
		}

	EGL_TRACE("eglReleaseThread <--");
	return EGL_TRUE;
	}

/*
 Creates a single pbuffer surface bound to the specified buffer for part or
 all of its buffer storage, and returns a handle to it. The width  and height
 of the pbuffer are determined by the width and height of buffer.

 Currently, the only client API resources which may be bound in this fashion
 are OpenVG VGImage objects.

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
EXPORT_C EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay aDisplay, EGLenum aBufType, EGLClientBuffer aBuffer,
			EGLConfig aConfig, const EGLint *aAttribList)
	{
	EGL_TRACE("eglCreatePbufferFromClientBuffer Display=%d, BufType=%d, Config=%d -->", aDisplay, aBufType, aConfig);
	EGL_TRACE_ATTRIB_LIST(aAttribList);

	EGLSurface newSurface = EGL_NO_SURFACE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		newSurface = Instance().eglCreatePbufferFromClientBuffer(*threadState, aDisplay, aBufType, aBuffer, aConfig, aAttribList);
		}
	EGL_TRACE("eglCreateWindowSurface new Surface=0x%x <--", newSurface);
	return newSurface;
	}

/*
 Set an attribute for an EGLSurface.

 The specified attribute of surface is set to value. Attributes that can be
 specified are EGL_MIPMAP_LEVEL, EGL_MULTISAMPLE_RESOLVE, and EGL_SWAP_BEHAVIOR.

 Returns EGL_FALSE on failure and value is not updated. If attribute is not a
 valid EGL surface attribute, then an EGL_BAD_ATTRIBUTE error is generated. If
 surface is not a valid EGLSurface then an EGL_BAD_SURFACE error is generated.
 */
EXPORT_C EGLBoolean eglSurfaceAttrib(EGLDisplay aDisplay, EGLSurface aSurface,
			    EGLint aAttribute, EGLint aValue)
	{
	EGL_TRACE("eglSurfaceAttrib Display=%d, Surface=0x%x, Attribute=0x%x, Value=%d", aDisplay, aSurface, aAttribute, aValue);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglSurfaceAttrib(*threadState, aDisplay, aSurface, aAttribute, aValue);
		}
	return result;
	}

/*
 Defines a two-dimensional texture image. The texture image consists of the image
 data in buffer for the specified surface, and need not be copied. Currently the
 only value accepted for buffer is EGL_BACK_BUFFER, which indicates the buffer into
 which OpenGL ES rendering is taking place (this is true even when using a
 singlebuffered surface, such as a pixmap). In future versions of EGL, additional buffer
 values may be allowed to bind textures to other buffers in an EGLSurface.

 eglBindTexImage is ignored if there is no current rendering context.

 If eglBindTexImage is called and the surface attribute EGL_TEXTURE_FORMAT is set
 to EGL_NO_TEXTURE, then an EGL_BAD_MATCH error is returned. If buffer is already
 bound to a texture then an EGL_BAD_ACCESS error is returned. If buffer is not a
 valid buffer, then an EGL_BAD_PARAMETER error is generated. If surface is not a
 valid EGLSurface, or is not a pbuffer surface supporting texture
 binding, then an EGL_BAD_SURFACE error is generated.
 */
EXPORT_C EGLBoolean eglBindTexImage(EGLDisplay aDisplay, EGLSurface aSurface, EGLint aBuffer)
	{
	EGL_TRACE("eglBindTexImage Display=%d, Surface=0x%x, Buffer=%d", aDisplay, aSurface, aBuffer);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglBindTexImage(*threadState, aDisplay, aSurface, aBuffer);
		}
	return result;
	}

/*
 The specified color buffer is released back to the surface. The surface is
 made  available for reading and writing when it no longer has any color
 buffers bound as textures.

 If the value of surface attribute EGL_TEXTURE_FORMAT is EGL_NO_TEXTURE,
 then an EGL_BAD_MATCH error is returned. If buffer is not a valid buffer
 (currently only EGL_BACK_BUFFER may  be specified), then an
 EGL_BAD_PARAMETER error is generated. If surface is not a valid EGLSurface,
 or is not a bound pbuffer surface, then an EGL_BAD_SURFACE error is
 returned.
 */
EXPORT_C EGLBoolean eglReleaseTexImage(EGLDisplay aDisplay, EGLSurface aSurface, EGLint aBuffer)
	{
	EGL_TRACE("eglReleaseTexImage Display=%d, Surface=0x%x, Buffer=%d", aDisplay, aSurface, aBuffer);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglReleaseTexImage(*threadState, aDisplay, aSurface, aBuffer);
		}
	return result;
	}

/*
 Specifies the minimum number of video frame periods per buffer swap for
 the window associated with the current context. The interval takes effect when
 eglSwapBuffers is first called subsequent to the eglSwapInterval call.

 Returns EGL_FALSE on failure. If there is no current context on the calling
 thread, a EGL_BAD_CONTEXT error is generated. If there is no surface bound
 to the current context, a EGL_BAD_SURFACE error is generated.
 */
EXPORT_C EGLBoolean eglSwapInterval(EGLDisplay aDisplay, EGLint aInterval)
	{
	EGL_TRACE("eglSwapInterval Display=%d, Interval=%d frames", aDisplay, aInterval);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglSwapInterval(*threadState, aDisplay, aInterval);
		}
	return result;
	}

/*
 Create a rendering context for the current rendering API.

 If eglCreateContext succeeds, it initializes the context to the initial
 state defined for the current rendering API, and returns a handle to it.
 The context can be used to render to any compatible EGLSurface. Although
 contexts are specific to a single client API , all contexts created in
 EGL exist in a single namespace. This allows many EGL calls which manage
 contexts to avoid use of the current rendering API.

 On failure returns EGL_NO_CONTEXT. If the current rendering api is EGL_NONE,
 then an EGL_BAD_MATCH error is generated (this situation can only arise in
 an implementation which does not support OpenGL ES, and prior to the first
 call to eglBindAPI). If share context is neither zero nor a valid context
 of the same client API type as the newly created context, then an EGL_-
 BAD_CONTEXT error is generated.

 If config is not a valid EGLConfig, or does not support the requested client
 API , then an EGL_BAD_CONFIG error is generated (this includes requesting creation
 of an OpenGL ES 1.x context when the EGL_RENDERABLE_TYPE attribute
 of config does not contain EGL_OPENGL_ES_BIT, or creation of an OpenGL ES
 2.x context when the attribute does not contain EGL_OPENGL_ES2_BIT).

 If the OpenGL or OpenGL ES server context state for share context exists in
 an address space that cannot be shared with the newly created context, if share -
 context was created on a different display than the one referenced by config, or if
 the contexts are otherwise incompatible (for example, one context being associated
 with a hardware device driver and the other with a software renderer), then an
 EGL_BAD_MATCH error is generated. If the server does not have enough resources
 to allocate the new context, then an EGL_BAD_ALLOC error is generated.
 */
EXPORT_C EGLContext eglCreateContext(EGLDisplay aDisplay, EGLConfig aConfig,
				EGLContext aShareContext, const EGLint *aAttribList)
	{
	EGL_TRACE("eglCreateContext Display=%d, Config=%d, ShareContext=%d -->", aDisplay, aConfig, aShareContext);
	EGL_TRACE_ATTRIB_LIST(aAttribList);

	EGLContext result = EGL_NO_CONTEXT;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglCreateContext(*threadState, aDisplay, aConfig, aShareContext, aAttribList);
		}
	EGL_TRACE("eglCreateContext new context=%d <--", result);
	return result;
	}

/*
 Destroy a rendering context.

 All resources associated with ctx are marked for deletion as soon as possible.
 Following eglDestroyContext, the context and the handle referring to it are
 treated in the same fashion as a context destroyed by eglTerminate.

 Returns EGL_FALSE on failure. An EGL_BAD_CONTEXT error is generated if ctx is
 not a valid context.
 */
EXPORT_C EGLBoolean eglDestroyContext(EGLDisplay aDisplay, EGLContext aContext)
	{
	EGL_TRACE("eglDestroyContext Display=%d, Context=%d", aDisplay, aContext);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglDestroyContext(*threadState, aDisplay, aContext);
		}
	return result;
	}

/*
 Make a context current, binds ctx to the current rendering thread
 and to the draw and read surfaces.

 For an OpenVG context, the same EGLSurface must be specified for both
 draw and read.

 If the calling thread already has a current context of the same client
 API type as ctx, then that context is flushed and marked as no longer
 current. ctx is then made the current context for the calling thread.

 To release the current context without assigning a new one, set ctx to
 EGL_NO_CONTEXT and set draw and read to EGL_NO_SURFACE. The currently
 bound context for the client API specified by the current rendering API
 is flushed and marked as no longer current, and there will be no current
 context for that client API after eglMakeCurrent returns. This is the
 only case in which eglMakeCurrent respects the current rendering API.
 In all other cases, the client API affected is determined by ctx. This
 is the only case where an uninitialized display may be passed to
 eglMakeCurrent.

 Returns EGL_FALSE on failure. Errors generated may include:
 * If draw or read are not compatible with ctx, then an EGL_BAD_MATCH
   error is generated.
 * If ctx is current to some other thread, or if either draw or read are
   bound to contexts in another thread, an EGL_BAD_ACCESS error is
   generated.
 * If binding ctx would exceed the number of current contexts of that
   client API type supported by the implementation, an EGL_BAD_ACCESS
   error is generated.
 * If either draw or read are pbuffers created with eglCreatePbufferFrom-
   ClientBuffer, and the underlying bound client API buffers are in use
   by the client API that created them, an EGL_BAD_ACCESS error is
   generated.
 * If ctx is not a valid context, an EGL_BAD_CONTEXT error is generated.
 * If either draw or read are not valid EGL surfaces, an EGL_BAD_SURFACE
   error is generated.
 * If a native window underlying either draw or read is no longer valid,
   an EGL_BAD_NATIVE_WINDOW error is generated.
 * If draw and read cannot fit into graphics memory simultaneously, an
   EGL_BAD_MATCH error is generated.
 * If the previous context of the calling thread has unflushed commands,
   and the previous surface is no longer valid, an EGL_BAD_CURRENT_SURFACE
   error is generated.
 * If the ancillary buffers for draw and read cannot be allocated, an
   EGL_BAD_ALLOC error is generated.
 * If a power management event has occurred, an EGL_CONTEXT_LOST error
   is generated.
 * As with other commands taking EGLDisplay parameters, if dpy is not a
   valid EGLDisplay handle, an EGL_BAD_DISPLAY error is generated.
 Other errors may arise when the context state is inconsistent with the
 surface state, as described in the EGL specification.
 */
EXPORT_C EGLBoolean eglMakeCurrent(EGLDisplay aDisplay, EGLSurface aDraw, EGLSurface aRead, EGLContext aContext)
	{
	EGL_TRACE("eglMakeCurrent Display=%d, DrawSurface=0x%x, ReadSurface=0x%x, Context=%d", aDisplay, aDraw, aRead, aContext);

	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		return Instance().eglMakeCurrent(*threadState, aDisplay, aDraw, aRead, aContext);
		}
	return EGL_FALSE;
	}

/*
 Get the current context for the current rendering API.

 If there is no current context for the current rendering API, or if the
 current rendering API is EGL_NONE, then EGL_NO_CONTEXT is returned (this
 is not an error).
 If the current context has been marked for deletion as a result of calling
 eglTerminate or eglDestroyContext, the handle returned by eglGetCurrentContext
 is not valid, and cannot be passed successfully to any other EGL function,
 as discussed in section 3.2 of the EGL Specification.
 */
EXPORT_C EGLContext eglGetCurrentContext(void)
	{
	EGL_TRACE("eglGetCurrentContext -->");

	EGLContext context = EGL_NO_SURFACE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		context = Instance().eglGetCurrentContext(*threadState);
		}
	EGL_TRACE("eglGetCurrentContext context=%d <--", context);
	return context;
	}

/*
 Get the surfaces used for rendering by a current context.

 readdraw is either EGL_READ or EGL_DRAW, to return respectively the read or
 draw surfaces bound to the current context in the calling thread, for the
 current rendering API.

 If there is no current context for the current rendering API, then
 EGL_NO_SURFACE is returned (this is not an error). If readdraw is neither
 EGL_READ nor EGL_DRAW, EGL_NO_SURFACE is returned and an EGL_BAD_PARAMETER
 error is generated. If a current surface has been marked for deletion as
 a result of calling eglTerminate or eglDestroySurface, the handle returned
 by eglGetCurrentSurface is not valid, and cannot be passed successfully to
 any other EGL function, as discussed in section 3.2 of the EGL Specification.
 */
EXPORT_C EGLSurface eglGetCurrentSurface(EGLint aReadDraw)
	{
	EGL_TRACE("eglGetCurrentSurface ReadDraw=%d -->", aReadDraw);

	EGLSurface surface = EGL_NO_SURFACE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		surface = Instance().eglGetCurrentSurface(*threadState, aReadDraw);
		}
	EGL_TRACE("eglGetCurrentSurface surface=0x%x <--", surface);
	return surface;
	}

/*
 Get the display associated with a current context.

 The display for the current context in the calling thread, for the current
 rendering API, is returned. If there is no current context for the current
 rendering API, EGL_NO_DISPLAY is returned (this is not an error).

 Note that EGL_NO_DISPLAY is used solely to represent an error condition,
 and is not a valid EGLDisplay handle. Passing EGL_NO_DISPLAY to any command
 taking an EGLDisplay parameter will generate either an EGL_BAD_DISPLAY
 error if the EGL implementation validates EGLDisplay handles, or undefined
 behavior as described at the end of section 3.1 of the EGL Specification.
 */
EXPORT_C EGLDisplay eglGetCurrentDisplay(void)
	{
	EGL_TRACE("eglGetCurrentDisplay -->");

	EGLDisplay display = EGL_NO_DISPLAY;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		display = Instance().eglGetCurrentDisplay(*threadState);
		}
	EGL_TRACE("eglGetCurrentDisplay Display=%d <--", display);
	return display;
	}

/*
 Obtain the value of context attributes.

 Returns in value the value of attribute for ctx. attribute must be set to
 EGL_CONFIG_ID, EGL_CONTEXT_CLIENT_TYPE, EGL_CONTEXT_CLIENT_VERSION, or
 EGL_RENDER_BUFFER.

 Returns EGL_FALSE on failure and value is not updated. If attribute is not
 a valid EGL context attribute, then an EGL_BAD_ATTRIBUTE error is generated.
 If ctx is invalid, an EGL_BAD_CONTEXT error is generated.
 */
EXPORT_C EGLBoolean eglQueryContext(EGLDisplay aDisplay, EGLContext aContext, EGLint aAttribute, EGLint *aValue)
	{
	EGL_TRACE("eglQueryContext Display=%d, Context=%d, Attribute=0x%x, aValue=0x%x", aDisplay, aContext, aAttribute, aValue);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglQueryContext(*threadState, aDisplay, aContext, aAttribute, aValue);
		}
	EGL_TRACE_GET_ATTRIB("eglQueryContext", "context", aDisplay, aContext, aAttribute, aValue, result);
	return result;
	}

/*
 EGL function is for backwards compatibility only.

 Equivalent to:
 EGLenum api = eglQueryAPI();
 eglBindAPI(EGL_OPENGL_ES_API);
 eglWaitClient();
 eglBindAPI(api);
 */
EXPORT_C EGLBoolean eglWaitGL(void)
	{
	EGL_TRACE("eglWaitGL");

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init(EglRFC::EeglWaitGL);
		result = threadState->ExecEglBooleanCmd(eglApiData);
		}
	return result;
	}

/*
 Wait for Symbian native rendering to complete, before performing
 client (Open GL ES, VG, ...) rendering.

 Returns EGL_TRUE on success. If there is no current context,
 the function has no effect but still returns EGL_TRUE. If the surface does not support
 native rendering (e.g. pbuffer and in most cases window surfaces), the function
 has no effect but still returns EGL_TRUE. If the surface associated with the
 calling thread’s current context is no longer valid, EGL_FALSE is returned and an
 EGL_BAD_CURRENT_SURFACE error is generated. If engine does not denote a recognized
 marking engine, EGL_FALSE is returned and an EGL_BAD_PARAMETER
 error is generated.
 */
EXPORT_C EGLBoolean eglWaitNative(EGLint aEngine)
	{
	EGL_TRACE("eglWaitNative %d", aEngine);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		RemoteFunctionCallData rfcdata; EglRFC eglApiData( rfcdata );
		eglApiData.Init( EglRFC::EeglWaitNative);
		eglApiData.AppendEGLint(aEngine);
		result = threadState->ExecEglBooleanCmd(eglApiData);

		if ( EGL_CORE_NATIVE_ENGINE == aEngine )
			{
			//Do native draw api sync
			}
		}
	return result;
	}

/*
 Post the color buffer to a window.

 If surface is a back-buffered window surface, then the color buffer is
 copied to the native window associated with that surface. If surface is a
 single-buffered window, pixmap, or pbuffer surface, eglSwapBuffers has no
 effect.

 The contents of the color buffer of surface may be affected by
 eglSwapBuffers, depending on the value of the EGL_SWAP_BEHAVIOR attribute
 of surface. See section 3.5.6 of the EGL Specification.

 Returns EGL_FALSE on failure. If surface is not a valid EGL surface, an
 EGL_BAD_SURFACE error is generated. If surface is not bound to the calling
 thread’s current context, an EGL_BAD_SURFACE error is generated. If target is
 not a valid native pixmap handle, an EGL_BAD_NATIVE_PIXMAP error should be
 generated. If the format of target is not compatible with the color buffer,
 or if the size of target is not the same as the size of the color buffer, and
 there is no defined conversion between the source and target formats, an
 EGL_BAD_MATCH error is generated. If called after a power management event
 has occurred, a EGL_CONTEXT_LOST error is generated. If the native window
 associated with surface is no longer valid, an EGL_ BAD_NATIVE_WINDOW error
 is generated.
 */
EXPORT_C EGLBoolean eglSwapBuffers(EGLDisplay aDisplay, EGLSurface aSurface)
	{
	EGL_TRACE("eglSwapBuffers begin Display=%d, Surface=0x%x -->", aDisplay, aSurface);

	EGLBoolean result = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		result = Instance().eglSwapBuffers(*threadState, aDisplay, aSurface);
		}

	EGL_TRACE("eglSwapBuffers success=%d <--", result);
	return result;
	}

/*
 Copy the color buffer to a native pixmap.

 The mapping of pixels in the color buffer to pixels in the pixmap is platform
 dependent, since the native platform pixel coordinate system may differ from
 that of client APIs.

 The color buffer of surface is left unchanged after calling eglCopyBuffers.
 Returns EGL_FALSE on failure. If surface is not a valid EGL surface, an
 EGL_BAD_SURFACE error is generated. If surface is not bound to the calling
 thread’s current context, an EGL_BAD_SURFACE error is generated. If target is
 not a valid native pixmap handle, an EGL_BAD_NATIVE_PIXMAP error should be
 generated. If the format of target is not compatible with the color buffer,
 or if the size of target is not the same as the size of the color buffer, and
 there is no defined conversion between the source and target formats, an
 EGL_BAD_MATCH error is generated. If called after a power management event
 has occurred, a EGL_CONTEXT_LOST error is generated. If the egl
 implementation does not support native pixmaps, an EGL_BAD_NATIVE_PIXMAP
 error is generated.
 */
EXPORT_C EGLBoolean eglCopyBuffers(EGLDisplay aDisplay, EGLSurface aSurface, EGLNativePixmapType aTarget)
	{
	// Note: API supports CFbsBitmap native pixmap but not SgImage
	EGL_TRACE("eglCopyBuffers Display=%d, Surface=0x%x, Target=0x%x", aDisplay, aSurface, aTarget);

	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{ // EGL is initialized for thread
		return Instance().eglCopyBuffers(*threadState, aDisplay, aSurface, aTarget);
		}
	return EGL_FALSE;
	}

/*
 * eglCreateImageKHR supports Khronos EGL extension #8, "KHR_image_base"
 *
 * Supported values for target parameter:
 * EGL_NATIVE_PIXMAP_KHR for Khronos EGL extension #9, "KHR_image_pixmap"
 */
EGLImageKHR eglCreateImageKHR(EGLDisplay aDisplay, EGLContext aContext, EGLenum aTarget, EGLClientBuffer aBuffer, const EGLint *aAttribList)
	{
	EGL_TRACE("eglCreateImageKHR Display=%d, Context=%d, Target=0x%x, Buffer=0x%x, AttribList=0x%x -->",
			aDisplay, aContext, aTarget, aBuffer, aAttribList);

	EGLImageKHR image = NULL;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		image = Instance().eglCreateImageKHR(*threadState, aDisplay, aContext, aTarget, aBuffer, aAttribList);
		}
	EGL_TRACE("eglCreateImageKHR image=0x%x <--", image);
	return image;
	}

/*
 * eglDestroyImageKHR supports Khronos EGL extension #8, "KHR_image_base"
 */
EGLBoolean eglDestroyImageKHR(EGLDisplay aDisplay, EGLImageKHR aImage)
	{
	EGL_TRACE("eglDestroyImageKHR Display=%d, Image=0x%x -->", aDisplay, aImage);

	EGLBoolean success = EGL_FALSE;
	TEglThreadState* threadState = CVghwUtils::CreateThreadState();
	if (threadState)
		{
		success = Instance().eglDestroyImageKHR(*threadState, aDisplay, aImage);
		}
	EGL_TRACE("eglDestroyImageKHR success=%d <--", success);
	return success;
	}


/*
 Returns the address of the extension function named by procName. procName
 must be a NULL-terminated string. The pointer returned should be cast to
 a function pointer type matching the extension function’s definition in
 that extension specification.

 A return value of NULL indicates that the specified function does not exist
 for the implementation.

 A non-NULL return value for eglGetProcAddress does not guarantee that an
 extension function is actually supported at runtime. The client must also make
 a corresponding query, such as glGetString(GL_EXTENSIONS) for OpenGL and
 OpenGL ES extensions; vgGetString(VG_EXTENSIONS) for OpenVG extensions;
 or eglQueryString(dpy, EGL_EXTENSIONS) for EGL extensions, to determine if
 an extension is supported by a particular client API context.

 Function pointers returned by eglGetProcAddress are independent of the display
 and the currently bound context, and may be used by any context which supports
 the extension.

 eglGetProcAddress may be queried for all of the following functions:
 * All EGL and client API extension functions supported by the implementation
   (whether those extensions are supported by the current context or not).
   This includes any mandatory OpenGL ES extensions.
 */
EXPORT_C void (*eglGetProcAddress (const char *aProcname))(...)
	{
	EGL_TRACE("eglGetProcAddress");

	if (aProcname)
		{
		return Instance().eglGetProcAddress(aProcname);
		}
	return NULL;
	}

} /* extern "C" */
