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


#ifndef __VGHWUTILS_H__
#define __VGHWUTILS_H__

//  Include Files
#include <e32base.h>	// CBase
#include <EGL/egl.h>
#include <VG/openvg.h>
#include <GLES/gl.h>
#include "eglrfc.h"


class RGuestVideoDriver;

// Virtual Graphics Hardware per thread data: TEglThreadState is concrete, MVgContext & MGlesContext are per API interfaces
class TEglThreadState;
class MGlesContext;
class MVgContext;

// data serialisation packages
class OpenVgRFC;
class EglRFC;
class RemoteFunctionCall;
class RemoteFunctionCallData;

// tracing
#ifdef _DEBUG
	#include <e32debug.h>
    #define UTIL_TRACE(fmt, args...) RDebug::Printf(fmt, ##args)
	#define VGHWPANIC_ASSERT(condition, panic) if (!(condition)) { VghwPanic(panic, #panic, #condition, __FILE__, __LINE__); }
	#define VGHWPANIC_ASSERT_DEBUG(condition, panic) if (!(condition)) { VghwPanic(panic, #panic, #condition, __FILE__, __LINE__); }
#else
    #define UTIL_TRACE(fmt, args...)
	#define VGHWPANIC_ASSERT(condition, panic) if (!(condition)) { VghwPanic(panic, NULL, NULL, NULL, __LINE__); }
	#define VGHWPANIC_ASSERT_DEBUG(condition, panic)
#endif


// Simulator Virtual Graphics Hardware panic codes
typedef enum
	{
	EVghwPanicNoVideoChannel = 1,
	EVghwPanicOperationDataTooBig,
	EVghwPanicDriverOpenError,
	EVghwPanicVghwHeapDoesNotExist,
	EVghwPanicInitializeFailed,
	EVghwPanicGraphicsDriverNotOpen,
	EVghwPanicSwitchToVghwHeapOldHeapIsVghwHeap,
	EVghwPanicSwitchFromVghwHeapParamIsNull,
	EVghwPanicSwitchFromVghwHeapOldHeapIsNotVghwHeap,
	EVghwPanicSwitchFromVghwHeapParamIsVghwHeap, // 10
	EVghwPanicGraphicsCreationLockDoesNotExist,
	EVghwPanicExecuteCommandFailed,
	EVghwPanicBadVgErrorValue,
	EVghwPanicBadGlesErrorValue,
	EVghwPanicBadEglErrorValue,
	EVghwPanicBadEglBoundApi,
	} TVghwPanic;

// This panic function is exported to allow inline functions to use it
IMPORT_C void VghwPanic(TVghwPanic aPanicCode, char* aPanicName, char* aCondition, char* aFile, TInt aLine);


// Mix-in class: EGL support functions, including SgImage support, expected to be called by Open VG and Open GL ES
class MEglManagementApi
	{
public:
	virtual TBool EglImageOpenForVgImage(EGLImageKHR aImage, TSize& aSize, VGHandle& aVgHandle, TUint64& aSgImageId) = 0;
	virtual void EglImageClose(EGLImageKHR aImage) = 0;
	// ToDo add an API so that Open VG can query the size of the current Context's surface
	};


typedef void (*ExtensionProcPointer)(...);


// Private interfaces for EGL to call into Open VG 
class MVgApiForEgl
	{
public:
	virtual ExtensionProcPointer guestGetVgProcAddress (const char *aProcName) = 0;
	};


// Private interfaces for EGL to call into Open GL ES 1.1 
class MGles11ApiForEgl
	{
public:
	virtual ExtensionProcPointer guestGetGles11ProcAddress (const char *aProcName) = 0;
	};


// Private interfaces for EGL to call into Open GL ES 2 
class MGles2ApiForEgl
	{
public:
	virtual ExtensionProcPointer guestGetGles2ProcAddress (const char *aProcName) = 0;
	};


/*
 VGHWUtils APIs are static for ease of use by the other Simulator Graphics DLLs.

 One (singleton) instance is created as Writeable Static Data when the DLL is loaded.
 After initialization this will hold process-wide data - e.g. a memory heap for Graphics DLLs
 to store local Symbian client state information.

 ToDo maybe make this a Symbian X class because it owns resources, but only as static data.
 */
NONSHARABLE_CLASS(CVghwUtils): public CBase
    {
	friend class TEglThreadState; // to limit visibility of DriverExecuteCommand
public:
	// open Driver connection, create Memory Heap, etc...
	IMPORT_C static void InitStatics();
	IMPORT_C static void DestroyStatics();

	IMPORT_C static TInt MapToHWAddress(const TInt aChunkHandle, TUint32& aHWAddress);
	IMPORT_C static TInt GetSurfaceBufferBaseAddress(TUint32& aHWAddress);
	IMPORT_C static TInt EglGetSgHandles(const TUint64 aId, TUint64 *aSgHandles);

	// VG Memory Pool APIs (One heap is created for the process, for use by all Simulator Graphics DLLs)
	IMPORT_C static TAny* Alloc(TInt aSize); 
	IMPORT_C static void Free(TAny* aPtr);
	IMPORT_C static RHeap* GetHeap();
	IMPORT_C static RHeap* SwitchToVghwHeap();
	IMPORT_C static void SwitchFromVghwHeap(RHeap* aOldHeapPtr);
	// handy function for Debug Asserts
	static inline TBool UsingVghwHeap() { return &User::Heap() == GetHeap(); }

	// if this thread does not have a state object try to alloc a new one
	IMPORT_C static TEglThreadState* CreateThreadState();
	// current state object, if any, for this thread (for EGL)
	IMPORT_C static TEglThreadState* EglThreadState();
	// current state object, if Open VG, for this thread (as tracked by EGL DLL)
	IMPORT_C static MVgContext* VgContext();
	// current state object for Open GL ES 1.1, for this thread (as tracked by EGL DLL)
	IMPORT_C static MGlesContext* GlesContext();
	// free current state object, if any, for this thread
	IMPORT_C static void ReleaseThreadState();

	IMPORT_C static void SetEglManagementApi(MEglManagementApi* aEglManagementApi);
	IMPORT_C static MEglManagementApi* EglManagementApi();

	// Private interfaces for EGL to call into Open GL ES and Open VG (avoids breaking DEF file compatibility) 
	IMPORT_C static void SetVgApiForEgl(MVgApiForEgl* aVgApiForEgl);
	IMPORT_C static MVgApiForEgl* VgApiForEgl();

	IMPORT_C static void SetGles11ApiForEgl(MGles11ApiForEgl* aGles11ApiForEgl);
	IMPORT_C static MGles11ApiForEgl* Gles11ApiForEgl();

	IMPORT_C static void SetGles2ApiForEgl(MGles2ApiForEgl* aGles2ApiForEgl);
	IMPORT_C static MGles2ApiForEgl* Gles2ApiForEgl();

protected:
	IMPORT_C static void DriverExecuteCommand(RemoteFunctionCallData& aRequestData);

private:
	inline static RGuestVideoDriver& Driver();

private:
	// everything is zeroed on construction, call Initialize() to prepare
	volatile static TBool iInitialized;    // NB "volatile" used for thread safety in InitStaticse()
	static TInt           iVghwInitMutex;  // required for thread safety in Initialize()

	// static member objects
	static RGuestVideoDriver*   iDriver;
	static RHeap*               iHeap;
	static MEglManagementApi*   iEglManagementApi;
	static MVgApiForEgl*        iVgApiForEgl;
	static MGles11ApiForEgl*    iGles11ApiForEgl;
	static MGles2ApiForEgl*     iGles2ApiForEgl;
	static TBool                iLoadedOpenVgDll;
	static TBool                iLoadedOpenGles11Dll;
	static TBool                iLoadedOpenGles2Dll;	
	};


// Basic interface for sending GL ES 1.1 commands down to Host Open GL ES implementation - can be expanded later
class MGlesContext
	{
public:
	// Execute Open GL ES 1.1 commands
	virtual void ExecuteGlesCommand(RemoteFunctionCall& aGlesRequestData) = 0;
	virtual void ExecuteGlesFlushCommand() = 0;
	virtual void ExecuteGlesFinishCommand() = 0;
	// GLES state
	virtual void SetGlesError(GLenum aGlesErrorCode) = 0;
	virtual GLenum GlesError() = 0;
	// ToDo make context tracking work for VG & GL ES
	virtual EGLContext GlesEglContext() = 0;
	};


// Basic interface for sending VG commands down to Host Open VG implementation - can be expanded later
class MVgContext
	{
public:
	// Execute Open VG commands
	virtual void ExecuteVgCommand(OpenVgRFC& aVgApiData) = 0;
	virtual void ExecuteVgFlushCommand() = 0;
	virtual void ExecuteVgFinishCommand() = 0;
	// VG state
	virtual void SetVgError(VGErrorCode aVgErrorCode) = 0;
	virtual VGErrorCode VgError() = 0;
	virtual EGLContext VgEglContext() = 0;
	};


NONSHARABLE_CLASS(TEglThreadState) : public MGlesContext, public MVgContext
	{
	friend class CVghwUtils; // to manage creation / delete of per thread state
public:
	// Execute EGL commands, and analyse for success
	IMPORT_C EGLBoolean ExecEglBooleanCmd(EglRFC& aEglApiData);
	IMPORT_C EGLContext ExecEglContextCmd(EglRFC& aEglApiData);
	IMPORT_C EGLSurface ExecEglSurfaceCmd(EglRFC& aEglApiData);
	// ToDo add static export for eglTerminate

	// Execute EGL commands whose return value cannot be tested for fail/success
	inline void ExecuteEglNeverErrorCmd(EglRFC& aEglApiData);
	// EGL thread state
	inline void ClearEglError();
	inline void SetEglError(EGLint aEglError);
	IMPORT_C EGLint EglError();
	inline void SetEglBoundApi(EGLenum aEglBoundApi);
	inline EGLenum EglBoundApi();
	inline EGLint PeekEglError(); // reads DLL set error code (only)

	// MGlesContext: Execute Open GL ES 1.1 commands & manage Open GL ES state
	virtual void ExecuteGlesCommand(RemoteFunctionCall& aGlesRequestData);
	virtual void ExecuteGlesFlushCommand();
	virtual void ExecuteGlesFinishCommand();
	virtual void SetGlesError(GLenum aGlesErrorCode);
	virtual GLenum GlesError();
	virtual EGLContext GlesEglContext();

	// MVgContext: Execute Open VG commands & manage Open VG state
	virtual void ExecuteVgCommand(OpenVgRFC& aVgApiData);
	virtual void ExecuteVgFlushCommand();
	virtual void ExecuteVgFinishCommand();
	virtual void SetVgError(VGErrorCode aVgErrorCode);
	virtual VGErrorCode VgError();
	virtual EGLContext VgEglContext();

protected:
	static TEglThreadState* New();
	void Destroy();
	TEglThreadState();
	~TEglThreadState();

private:
	GLenum GetHostGlesError();
	VGErrorCode GetHostVgError();

private:
	// EGL thread state
	EGLint       iEglError;
	TBool        iEglHostHasRecentError;
	// currently bound graphics API
	EGLenum      iEglBoundApi;

	// Open VG context for thread
	VGErrorCode  iVgError;
	EGLContext  iVgEglContext;
	TBool        iVgCommandsSinceGetError;
	TBool        iVgCommandsSinceFlush;
	TBool        iVgCommandsSinceFinish;

	// Open GL ES 1.1 context for thread
	GLenum       iGlesError;
	EGLContext  iGlesEglContext;
	TBool        iGlesCommandsSinceGetError;
	TBool        iGlesCommandsSinceFlush;
	TBool        iGlesCommandsSinceFinish;
	};




///////////////////////////////////////////////////////////////////////////////////////////////////
// TVghwThreadState inline functions

void TEglThreadState::ClearEglError()
	{
	UTIL_TRACE("  TVghwThreadState::ClearEglError");
	iEglError = EGL_SUCCESS;
	iEglHostHasRecentError = EFalse;
	}


void TEglThreadState::SetEglError(EGLint aEglError)
	{
	UTIL_TRACE("  TVghwThreadState::SetEglError error=0x%x", aEglError);
	VGHWPANIC_ASSERT_DEBUG((aEglError >= EGL_SUCCESS) && (aEglError <= 0x301F), EVghwPanicBadEglErrorValue);
	iEglError = aEglError;
	iEglHostHasRecentError = EFalse;
	}


EGLint TEglThreadState::PeekEglError() // reads DLL set error code
	{
	return iEglError;
	}


void TEglThreadState::SetEglBoundApi(EGLenum aEglBoundApi)
	{
	VGHWPANIC_ASSERT_DEBUG((aEglBoundApi == EGL_OPENGL_ES_API) || (aEglBoundApi == EGL_OPENVG_API), EVghwPanicBadEglBoundApi);
	iEglBoundApi = aEglBoundApi;
	}


EGLenum TEglThreadState::EglBoundApi()
	{
	return iEglBoundApi;
	}

// Execute EGL Command simple variants

void TEglThreadState::ExecuteEglNeverErrorCmd(EglRFC& aEglApiData)
	{ // for EGL commands which cannot have a host error
	CVghwUtils::DriverExecuteCommand(aEglApiData.Data());
	iEglHostHasRecentError = EFalse;
	iEglError = EGL_SUCCESS;
	}

#endif  // __VGHWUTILS_H__
