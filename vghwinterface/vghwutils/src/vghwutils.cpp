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


#include <e32atomics.h>
#include <e32debug.h>

#include <graphics/guestvideodriver.h>

#include "vghwutils.h"
#include "eglrfc.h"
#include "openvgrfc.h"
#include "opengles11rfc.h"

#ifdef _DEBUG
#define EGL_ERROR_PRINT(T, E)  EglErrorPrint(T, E)
#define GLES_ERROR_PRINT(T, E) GlesErrorPrint(T, E)
#define VG_ERROR_PRINT(T, E)   VgErrorPrint(T, E)
static void EglErrorPrint(char* aTitle, EGLint aEglError);
static void GlesErrorPrint(char* aTitle, GLenum aGlesError);
static void VgErrorPrint(char* aTitle, VGErrorCode aVgError);
#else
#define EGL_ERROR_PRINT(T, E)
#define GLES_ERROR_PRINT(T, E)
#define VG_ERROR_PRINT(T, E)
#endif


// Min & Max sizes for creating Memory Heap for Guest Graphics DLLs
#define KVghwHeapMin 0x1000
#define KVghwHeapMax 0x100000


// protection against concurrent initialisation from multiple threads
volatile TBool       CVghwUtils::iInitialized = EFalse;
TInt                 CVghwUtils::iVghwInitMutex = 0;

// static member objects - Writeable Static Data
RGuestVideoDriver* CVghwUtils::iDriver = NULL;
RHeap*               CVghwUtils::iHeap = NULL;
MEglManagementApi*   CVghwUtils::iEglManagementApi = NULL;
MVgApiForEgl*        CVghwUtils::iVgApiForEgl = NULL;
MGles11ApiForEgl*    CVghwUtils::iGles11ApiForEgl = NULL;
MGles2ApiForEgl*     CVghwUtils::iGles2ApiForEgl = NULL;
TBool                CVghwUtils::iLoadedOpenVgDll = EFalse;
TBool                CVghwUtils::iLoadedOpenGles11Dll = EFalse;
TBool                CVghwUtils::iLoadedOpenGles2Dll = EFalse;	



_LIT(KVghwPanicCategory, "Guest VGHW");


// NOTE: Exported because some VGHW functions are inline and could Assert & Panic from somewhere else
EXPORT_C void VghwPanic(TVghwPanic aPanicCode, char* aPanicName, char* aCondition, char* aFile, TInt aLine)
	{
	if (aCondition && aFile)
		{
		RDebug::Printf("VghwUtils Panic %s for failed Assert (%s), at %s:%d", aPanicName, aCondition, aFile, aLine);
		}
	else
		{  
		RDebug::Printf("VghwUtils Panic %d for failed Assert (line %d)", aPanicCode, aLine);
		}
		
	User::Panic(KVghwPanicCategory, aPanicCode);
	}

// inline functions

RGuestVideoDriver& CVghwUtils::Driver()
	{
	// TODO remove this later in refactoring - Open VG & GL ES commands should not reach here before EGL has initialized the singleton
	if (!iInitialized || !iDriver)
		{
		UTIL_TRACE("CVghwUtils::Driver called before EGL has initialised VghwUtils: iInitialized=%d, iDriver=0x%x", iInitialized, iDriver);
		InitStatics();
		}
	VGHWPANIC_ASSERT(iInitialized && iDriver, EVghwPanicGraphicsDriverNotOpen);
	return *iDriver;
	}


EXPORT_C void CVghwUtils::DestroyStatics()
	{
#ifdef _DEBUG
	TUint64 processId = RProcess().Id().Id();
	TUint64 threadId = RThread().Id().Id();
	UTIL_TRACE("CVghwUtils::DestroyStatics iInitialized=%d, iDriver=0x%x, iHeap=0x%x, Process=0x%lx, Thread=0x%lx",
		iInitialized, iDriver, iHeap, processId, threadId);
#endif
	if (iHeap)
		{
		if (iDriver)
			{
			iDriver->Flush();
			iDriver->Close();
			RHeap* threadHeap = SwitchToVghwHeap();
			delete iDriver;
			SwitchFromVghwHeap(threadHeap);
			iDriver = NULL;
			}
		iHeap->Reset();
		iHeap->Close();
		iHeap = NULL;
		}
	if (iInitialized)
		{
		iInitialized = EFalse;
		}
	}


// Ensure Graphics memory heap is created, and Open driver channel
EXPORT_C void CVghwUtils::InitStatics()
	{
#ifdef _DEBUG
	TUint64 processId = RProcess().Id().Id();
	TUint64 threadId = RThread().Id().Id();
	UTIL_TRACE("CVghwUtils::InitStatics start Process=0x%lx, Thread=0x%lx, iInitialized=%d",
		processId, threadId, iInitialized);
#endif
	// bootstrap creation of mutexes and Graphics memory heap
	while (!iInitialized)
		{
		TInt creationError = KErrNone;
		// thread safe initialization
		// LockedDec() returns the value prior to decrement
		if (0 != User::LockedDec(iVghwInitMutex))
			{
			User::AfterHighRes(1000);     // Yield X microseconds, review length if code is changed
			}
		else
			{
			// volatile attribute forces rereading of the pointer, in case another thread has done the allocation
			if (!iInitialized)
				{ // loop style init exits if any stage fails, and gives per step debug logging
				TInt step = 0;
				do
					{
					switch (step)
						{
						case 0:
							// guarantee that initialisation of the object is flushed before the pointer is published
							__e32_atomic_store_rel_ptr(&iHeap, User::ChunkHeap(NULL, KVghwHeapMin, KVghwHeapMax));
							if (!iHeap)
								{
								creationError = KErrNoMemory;
								}
							break;
						case 1:
							{
							VGHWPANIC_ASSERT_DEBUG(iHeap, EVghwPanicVghwHeapDoesNotExist);
							RHeap* threadHeap = User::SwitchHeap(iHeap); // switch to VGHW graphics Heap
							iDriver = new RGuestVideoDriver();
							if (!iDriver)
								{
								creationError = KErrNoMemory;
								}
							User::SwitchHeap(threadHeap); // switch back to client Heap
							}
							break;
						case 2:
							creationError = User::LoadLogicalDevice(_L("guestvideohw"));
							if ( (KErrNone == creationError) || (KErrAlreadyExists == creationError) )
								{
								creationError = iDriver->Open();
								}
							break;
						default: // all initialisation steps completed
							iInitialized = ETrue;
						}
					++step;
					}
				while (!iInitialized && !creationError);
				UTIL_TRACE("CVghwUtils::InitStatics %s step=%d; creationError=%d, iInitialized=%d, iDriver=0x%x, iHeap=0x%x",
						creationError ? "failed at" : "completed", step, creationError, iInitialized, iDriver, iHeap);
				}
			}
		User::LockedInc(iVghwInitMutex);

		VGHWPANIC_ASSERT(creationError == KErrNone, EVghwPanicInitializeFailed);
		}
	}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CVghwUtils::DriverExecuteCommand(RemoteFunctionCallData& aRequestData)
	{
	if (!iInitialized || !iDriver)
		{
		UTIL_TRACE("CVghwUtils::DriverExecuteCommand called before EGL has initialised VghwUtils: iInitialized=%d, iDriver=0x%x", iInitialized, iDriver);
		InitStatics();
		}
	VGHWPANIC_ASSERT(iInitialized && iDriver, EVghwPanicGraphicsDriverNotOpen);
	iDriver->ExecuteCommand(aRequestData);
	}


// -----------------------------------------------------------------------------
// Use driver to discover linear memory address of chunk.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVghwUtils::MapToHWAddress( const TInt aChunkHandle, TUint32& aHWAddress )
	{
	UTIL_TRACE("CVghwUtils::MapToHWAddress");
	return Driver().MapToHWAddress( aChunkHandle, aHWAddress );
	}


// SgImage support function - get underlying VGImage & Pbuffer handles
EXPORT_C TInt CVghwUtils::EglGetSgHandles( const TUint64 aId, TUint64 *aSgHandles )
	{
	UTIL_TRACE("CVghwUtils::EglGetSgHandles");
	return Driver().EglGetSgHandles( aId, aSgHandles );
	}


// Memory Heap Management functions

EXPORT_C TAny* CVghwUtils::Alloc(TInt aSize)
	{
	if (!iInitialized || !iHeap)
		{
		UTIL_TRACE("CVghwUtils::Alloc called before EGL has initialised VghwUtils: iInitialized=%d, iDriver=0x%x", iInitialized, iDriver);
		InitStatics();
		}
	VGHWPANIC_ASSERT(iInitialized && iHeap, EVghwPanicVghwHeapDoesNotExist);
	return iHeap->Alloc(aSize);
	}


EXPORT_C void CVghwUtils::Free(TAny* aPtr)
	{
	if (!iInitialized || !iHeap)
		{
		UTIL_TRACE("CVghwUtils::Free called before EGL has initialised VghwUtils: iInitialized=%d, iDriver=0x%x", iInitialized, iDriver);
		InitStatics();
		}
	VGHWPANIC_ASSERT(iInitialized && iHeap, EVghwPanicVghwHeapDoesNotExist);
	iHeap->Free(aPtr);
	}


EXPORT_C RHeap* CVghwUtils::GetHeap()
	{
	if (!iInitialized || !iHeap)
		{
		UTIL_TRACE("CVghwUtils::GetHeap called before EGL has initialised VghwUtils: iInitialized=%d, iDriver=0x%x", iInitialized, iDriver);
		InitStatics();
		}
	VGHWPANIC_ASSERT(iInitialized && iHeap, EVghwPanicVghwHeapDoesNotExist);
	return iHeap;
	}


// Switch to VGHW Memory Heap.
EXPORT_C RHeap* CVghwUtils::SwitchToVghwHeap()
	{
	if (!iInitialized || !iHeap)
		{
		UTIL_TRACE("CVghwUtils::SwitchToVghwHeap called before EGL has initialised VghwUtils: iInitialized=%d, iDriver=0x%x", iInitialized, iDriver);
		InitStatics();
		}
	VGHWPANIC_ASSERT(iHeap, EVghwPanicVghwHeapDoesNotExist);
	RHeap* oldHeap = User::SwitchHeap(iHeap); // switch to Guest graphics heap
	VGHWPANIC_ASSERT(oldHeap != iHeap, EVghwPanicSwitchToVghwHeapOldHeapIsVghwHeap);
	return oldHeap;
	}


// Switch back to original heap 
EXPORT_C void CVghwUtils::SwitchFromVghwHeap(RHeap* aOldHeapPtr)
	{
	if (!iInitialized || !iHeap)
		{
		UTIL_TRACE("CVghwUtils::SwitchFromVghwHeap called before EGL has initialised VghwUtils: iInitialized=%d, iDriver=0x%x", iInitialized, iDriver);
		InitStatics();
		}
	VGHWPANIC_ASSERT(iHeap, EVghwPanicVghwHeapDoesNotExist);
	VGHWPANIC_ASSERT(aOldHeapPtr, EVghwPanicSwitchFromVghwHeapParamIsNull);
	VGHWPANIC_ASSERT(aOldHeapPtr != iHeap, EVghwPanicSwitchFromVghwHeapParamIsVghwHeap);

	RHeap* oldHeap = User::SwitchHeap(aOldHeapPtr); // switch from Guest graphics heap back to thread heap
	VGHWPANIC_ASSERT(oldHeap == iHeap, EVghwPanicSwitchFromVghwHeapOldHeapIsNotVghwHeap);
	}


// if this thread does not have an object try to alloc a new one
EXPORT_C TEglThreadState* CVghwUtils::CreateThreadState()
	{
	TEglThreadState* threadState = reinterpret_cast<TEglThreadState*>(Dll::Tls());
	if (!threadState)
		{
		UTIL_TRACE("CVghwUtils::CreateThreadState - object is currently 0x%x, iInitialized=%d",
				threadState, iInitialized);
		if (!iInitialized)
			{
			InitStatics();
			}
		threadState = TEglThreadState::New();
		Dll::SetTls(threadState);
		}
	return threadState;
	}


// current state object, if any, for this thread
EXPORT_C TEglThreadState* CVghwUtils::EglThreadState()
	{
	TEglThreadState* threadState = reinterpret_cast<TEglThreadState*>(Dll::Tls());
	return threadState;
	}


// current state object, if Open VG is the current bound API, for this thread 
EXPORT_C MVgContext* CVghwUtils::VgContext()
	{
	TEglThreadState* threadState = reinterpret_cast<TEglThreadState*>(Dll::Tls());
	return threadState;
	// ToDo sort this to check for valid Open VG context
	/*
	if (threadState && threadState->VgEglContext())
		return threadState;
	return NULL;
	*/
	}


// current state object, if Open GL ES is the current bound API, for this thread (for EGL)
EXPORT_C MGlesContext* CVghwUtils::GlesContext()
	{
	TEglThreadState* threadState = reinterpret_cast<TEglThreadState*>(Dll::Tls());
	return threadState;
	// ToDo sort this to check for valid Open GL ES context
	/*
	if (threadState && threadState->GlesEglContext())
		return threadState;
	return NULL; */
	}


// free current state object, if any, for this thread
EXPORT_C void CVghwUtils::ReleaseThreadState()
	{
	TEglThreadState* threadState = reinterpret_cast<TEglThreadState*>(Dll::Tls());
	UTIL_TRACE("CVghwUtils::ReleaseThreadState - object = 0x%x", threadState);
	if (threadState)
		{
		threadState->Destroy();
		Dll::FreeTls();
		}
	}


EXPORT_C MEglManagementApi* CVghwUtils::EglManagementApi()
	{
	return iEglManagementApi;
	}


EXPORT_C void CVghwUtils::SetEglManagementApi(MEglManagementApi* aEglManagementApi)
	{
	UTIL_TRACE("CVghwUtils::SetEglManagementApi  aEglManagementApi=0x%x", aEglManagementApi);
	iEglManagementApi = aEglManagementApi;
	}


/*
 Stored pointers into Open GL ES 1.1, GL ES 2 and Open VG are problematic if the DLL is later unloaded.
 (RSgDriver::Open currently loads & unloads all the Open VG & GL ES DLLs, to determine features to publish.)
 Possibly Open the DLL again when the non-NULL pointer is fetched by EGL, probably needs flags so that
 this is only done once.
 */
EXPORT_C void CVghwUtils::SetVgApiForEgl(MVgApiForEgl* aVgApiForEgl)
	{
	UTIL_TRACE("CVghwUtils::SetVgApiForEgl  aVgApiForEgl=0x%x, iVgApiForEgl=0x%x, iLoadedOpenVgDll=0x%x",
			aVgApiForEgl, iVgApiForEgl, iLoadedOpenVgDll);
	// don't let a thread set this to NULL if we have forced the DLL to stay loaded
	if (aVgApiForEgl || !iLoadedOpenVgDll)
		{
		iVgApiForEgl = aVgApiForEgl;
		}
	}


EXPORT_C MVgApiForEgl* CVghwUtils::VgApiForEgl()
	{
	if (iVgApiForEgl && !iLoadedOpenVgDll)
		{ // ensure Open VG DLL stays loaded from now on
		_LIT(KLibOpenVg, "libOpenVG.dll");
		RLibrary lib;
		if (lib.Load(KLibOpenVg) == KErrNone)
			{
			UTIL_TRACE("CVghwUtils::VgApiForEgl - Open VG dll loaded, iVgApiForEgl=0x%x", iVgApiForEgl);
			iLoadedOpenVgDll = ETrue;
			return iVgApiForEgl;
			}
		}
	UTIL_TRACE("CVghwUtils::VgApiForEgl - Open VG dll not loaded or not safe to use");
	return NULL;
	}


EXPORT_C void CVghwUtils::SetGles11ApiForEgl(MGles11ApiForEgl* aGles11ApiForEgl)
	{
	UTIL_TRACE("CVghwUtils::SetGles11ApiForEgl  aGles11ApiForEgl=0x%x, iGles11ApiForEgl=0x%x, iLoadedOpenGles11Dll=0x%x",
			aGles11ApiForEgl, iGles11ApiForEgl, iLoadedOpenGles11Dll);
	// don't let a thread set this to NULL if we have forced the DLL to stay loaded
	if (aGles11ApiForEgl || !iLoadedOpenGles11Dll)
		{
		iGles11ApiForEgl = aGles11ApiForEgl;
		}
	}


EXPORT_C MGles11ApiForEgl* CVghwUtils::Gles11ApiForEgl()
	{
	if (iGles11ApiForEgl && !iLoadedOpenGles11Dll)
		{ // ensure Open VG DLL stays loaded from now on
		_LIT(KLibOpenGles, "libGLESv1_CM.dll");
		RLibrary lib;
		if (lib.Load(KLibOpenGles) == KErrNone)
			{
			UTIL_TRACE("CVghwUtils::Gles11ApiForEgl - Open GL ES 1.1 dll loaded, iGles11ApiForEgl=0x%x", iGles11ApiForEgl);
			iLoadedOpenGles11Dll = ETrue;
			return iGles11ApiForEgl;
			}
		}
	UTIL_TRACE("CVghwUtils::Gles11ApiForEgl - Open GL ES 1.1 dll not loaded or not safe to use");
	return NULL;
	}


EXPORT_C void CVghwUtils::SetGles2ApiForEgl(MGles2ApiForEgl* aGles2ApiForEgl)
	{
	UTIL_TRACE("CVghwUtils::SetGles2ApiForEgl  aGles2ApiForEgl=0x%x, iGles2ApiForEgl=0x%x, iLoadedOpenGles2Dll=0x%x",
			aGles2ApiForEgl, iGles2ApiForEgl, iLoadedOpenGles2Dll);
	// don't let a thread set this to NULL if we have forced the DLL to stay loaded
	if (aGles2ApiForEgl || !iLoadedOpenGles2Dll)
		{
		iGles2ApiForEgl = aGles2ApiForEgl;
		}
	}


EXPORT_C MGles2ApiForEgl* CVghwUtils::Gles2ApiForEgl()
	{
	if (iGles2ApiForEgl && !iLoadedOpenGles2Dll)
		{ // ensure Open VG DLL stays loaded from now on
		_LIT(KLibOpenGles2, "libGLESv2.dll");
		RLibrary lib;
		if (lib.Load(KLibOpenGles2) == KErrNone)
			{
			UTIL_TRACE("CVghwUtils::Gles2ApiForEgl - Open GL ES 2 dll loaded, iGles2ApiForEgl=0x%x", iGles2ApiForEgl);
			iLoadedOpenGles2Dll = ETrue;
			return iGles2ApiForEgl;
			}
		}
	UTIL_TRACE("CVghwUtils::Gles2ApiForEgl - Open GL ES 2 dll not loaded or not safe to use");
	return NULL;
	}


////////////////////////////////////////////////////////////////////////////////////

// factory method
TEglThreadState* TEglThreadState::New()
	{
	UTIL_TRACE("TEglThreadState::New");
	RHeap* currentHeap = CVghwUtils::SwitchToVghwHeap();
	TEglThreadState* self = new TEglThreadState();
	CVghwUtils::SwitchFromVghwHeap(currentHeap);
	return self;
	}


TEglThreadState::TEglThreadState() :
	// EGL thread state
	iEglError(EGL_SUCCESS),
	iEglHostHasRecentError(EFalse),
	// currently bound graphics API, initial value from EGL spec 
	iEglBoundApi(EGL_OPENGL_ES_API),
	// Open VG thread state 
	iVgError(VG_NO_ERROR),
	iVgEglContext(EGL_NO_CONTEXT),
	iVgCommandsSinceGetError(EFalse),
	iVgCommandsSinceFlush(EFalse),
	iVgCommandsSinceFinish(EFalse),
	// Open GL ES 1.1 state
	iGlesError(GL_NO_ERROR),
	iGlesEglContext(EGL_NO_CONTEXT),
	iGlesCommandsSinceGetError(EFalse),
	iGlesCommandsSinceFlush(EFalse),
	iGlesCommandsSinceFinish(EFalse)
	{}


void TEglThreadState::Destroy()
	{
	UTIL_TRACE("TEglThreadState::Destroy");
	RHeap* currentHeap = CVghwUtils::SwitchToVghwHeap();
	delete this;
	CVghwUtils::SwitchFromVghwHeap(currentHeap);
	}


TEglThreadState::~TEglThreadState()
	{}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C EGLBoolean TEglThreadState::ExecEglBooleanCmd(EglRFC& aEglApiData)
	{
	CVghwUtils::DriverExecuteCommand(aEglApiData.Data());

	EGLBoolean hostResult = (EGLBoolean) aEglApiData.ReturnValue();
	if (hostResult == EGL_TRUE)
		{
		ClearEglError();
		}
	else
		{ // error present in host graphics stack
		iEglHostHasRecentError = ETrue;
		}
	return hostResult;
	}


EXPORT_C EGLContext TEglThreadState::ExecEglContextCmd(EglRFC& aEglApiData)
	{
	CVghwUtils::DriverExecuteCommand(aEglApiData.Data());

	EGLContext hostResult = (EGLContext) aEglApiData.ReturnValue();
	if (hostResult != EGL_NO_CONTEXT)
		{
		ClearEglError();
		}
	else
		{ // error present in host graphics stack
		iEglHostHasRecentError = ETrue;
		}
	return hostResult;
	}


EXPORT_C EGLSurface TEglThreadState::ExecEglSurfaceCmd(EglRFC& aEglApiData)
	{
	CVghwUtils::DriverExecuteCommand(aEglApiData.Data());

	EGLSurface hostResult = (EGLSurface) aEglApiData.ReturnValue();
	if (hostResult != EGL_NO_SURFACE)
		{
		ClearEglError();
		}
	else
		{ // error present in host graphics stack
		iEglHostHasRecentError = ETrue;
		}
	return hostResult;
	}


/*
 Get details of the last EGL api error.
 (Resets stored last error value to EGL_SUCCESS.)

 @return EGL_SUCCESS or an EGL_xxxx error constant.
 */
EXPORT_C EGLint TEglThreadState::EglError()
	{
	EGLint eglError = iEglError;
	if (iEglHostHasRecentError)
		{
		// last EGL command was executed/failed in the Host side, so need to ask it for the error
		RemoteFunctionCallData rfcdata; EglRFC eglApiData(rfcdata);
		eglApiData.Init(EglRFC::EeglGetError);
		CVghwUtils::DriverExecuteCommand(eglApiData.Data());
		eglError = (EGLint)eglApiData.ReturnValue();
		EGL_ERROR_PRINT("TEglThreadState::EglError newest error fetched from Host EGL", eglError);
		iEglHostHasRecentError = EFalse;
		}
	else
		{
		EGL_ERROR_PRINT("TEglThreadState::EglError newest EGL error set by DLL", eglError);
		}
	iEglError = EGL_SUCCESS;
	return eglError;
	}


void TEglThreadState::ExecuteVgCommand(OpenVgRFC& aVgApiData)
	{
	CVghwUtils::DriverExecuteCommand(aVgApiData.Data());
	iVgCommandsSinceGetError = ETrue;
	iVgCommandsSinceFlush = ETrue;
	iVgCommandsSinceFinish = ETrue;
	}


void TEglThreadState::ExecuteVgFlushCommand()
	{
	if (iVgCommandsSinceFlush)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgFlush);

		CVghwUtils::DriverExecuteCommand(vgApiData.Data());
		iVgCommandsSinceFlush = EFalse;
		iVgCommandsSinceFinish = EFalse;
		}
	}


void TEglThreadState::ExecuteVgFinishCommand()
	{
	if (iVgCommandsSinceFinish)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgFinish);

		CVghwUtils::DriverExecuteCommand(vgApiData.Data());
		iVgCommandsSinceFinish = EFalse;
		}
	}


VGErrorCode TEglThreadState::VgError()
	{
	VGErrorCode vgError = iVgError;
	VG_ERROR_PRINT("TEglThreadState::VgError oldest Open VG DLL error", vgError);

	if (iVgCommandsSinceGetError)
		{ // fetch and clear the host error state
		VGErrorCode hostVgError = GetHostVgError();
		VG_ERROR_PRINT("TEglThreadState::VgError oldest Host Open VG DLL error", hostVgError);

		if (vgError == VG_NO_ERROR)
			{
			vgError = hostVgError;
			}
		iVgCommandsSinceGetError = EFalse;
		}

	iVgError = VG_NO_ERROR;
	return vgError;
	}


VGErrorCode TEglThreadState::GetHostVgError()
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init( OpenVgRFC::EvgGetError );

	CVghwUtils::DriverExecuteCommand(vgApiData.Data());
	return (VGErrorCode)vgApiData.ReturnValue();
	}


// Open GL ES

GLenum TEglThreadState::GlesError()
	{
	GLenum glesError = iGlesError;
	GLES_ERROR_PRINT("TEglThreadState::GlesError oldest Open GL ES DLL error", glesError);

	if (iGlesCommandsSinceGetError)
		{ // fetch and clear the host error state
		GLenum hostGlesError = GetHostGlesError();
		GLES_ERROR_PRINT("TEglThreadState::GlesError oldest Host Open GL ES DLL error", hostGlesError);

		if (glesError == GL_NO_ERROR)
			{
			glesError = hostGlesError;
			}
		iGlesCommandsSinceGetError = EFalse;
		}

	iGlesError = GL_NO_ERROR;
	return glesError;
	}


GLenum TEglThreadState::GetHostGlesError()
	{
	RemoteFunctionCallData data;
	OpenGlES11RFC glesApiData(data);
	glesApiData.Init(OpenGlES11RFC::EglGetError);
	CVghwUtils::DriverExecuteCommand(glesApiData.Data());
	return (GLint)glesApiData.ReturnValue();
	}


void TEglThreadState::ExecuteGlesCommand(RemoteFunctionCall& aGlesRequestData)
	{
	CVghwUtils::DriverExecuteCommand(aGlesRequestData.Data());
	iGlesCommandsSinceGetError = ETrue;
	iGlesCommandsSinceFlush = ETrue;
	iGlesCommandsSinceFinish = ETrue;
	}


void TEglThreadState::ExecuteGlesFlushCommand()
	{
	if (iGlesCommandsSinceFlush)
		{
		RemoteFunctionCallData rfcData;
		OpenGlES11RFC glesApiData( rfcData );
		glesApiData.Init(OpenGlES11RFC::EglFlush, RemoteFunctionCallData::EOpRequest);
		CVghwUtils::DriverExecuteCommand(glesApiData.Data());
		iGlesCommandsSinceFlush = EFalse;
		iGlesCommandsSinceFinish = EFalse;
		}
	}


void TEglThreadState::ExecuteGlesFinishCommand()
	{
	if (iGlesCommandsSinceFinish)
		{
		RemoteFunctionCallData rfcData;
		OpenGlES11RFC glesApiData( rfcData );
		glesApiData.Init(OpenGlES11RFC::EglFinish, RemoteFunctionCallData::EOpRequest);
		CVghwUtils::DriverExecuteCommand(glesApiData.Data());
		iGlesCommandsSinceFinish = EFalse;
		}
	}


// Open VG state
void TEglThreadState::SetVgError(VGErrorCode aVgError)
	{
	VGHWPANIC_ASSERT_DEBUG( (aVgError >= VG_BAD_HANDLE_ERROR) && (aVgError <= 0x10FF), EVghwPanicBadVgErrorValue);
	// for Open VG track the oldest uncleared error
	VG_ERROR_PRINT("TEglThreadState::SetVgError error", aVgError);
	if ( (iVgError == VG_NO_ERROR) && (aVgError >= VG_BAD_HANDLE_ERROR) && (aVgError <= 0x10FF) )
		{
		if (iVgCommandsSinceGetError)
			{
			iVgError = GetHostVgError();
			VG_ERROR_PRINT("TEglThreadState::SetVgError oldest Host Open VG DLL error", iVgError);
			if (iVgError != VG_NO_ERROR)
				{
				return;
				}
			}
		iVgError = aVgError;
		}
	}


EGLContext TEglThreadState::VgEglContext()
	{
	return iVgEglContext;
	}


// Open GL ES thread state
void TEglThreadState::SetGlesError(GLenum aGlesError)
	{
	VGHWPANIC_ASSERT_DEBUG( (aGlesError >= GL_INVALID_ENUM) && (aGlesError <= 0x05FF), EVghwPanicBadGlesErrorValue);
	// for Open GL ES track the oldest uncleared error
	GLES_ERROR_PRINT("TEglThreadState::SetGlesError error", aGlesError);
	if ( (iGlesError == GL_NO_ERROR) && (aGlesError >= GL_INVALID_ENUM) && (aGlesError <= 0x05FF) )
		{
		if (iGlesCommandsSinceGetError)
			{
			iGlesError = GetHostGlesError();
			GLES_ERROR_PRINT("TEglThreadState::SetGlesError oldest Host Open GL ES DLL error", iGlesError);
			if (iGlesError != GL_NO_ERROR)
				{
				return;
				}
			}
		iGlesError = aGlesError;
		}
	}


EGLContext TEglThreadState::GlesEglContext()
	{
	return iGlesEglContext;
	}

// -----------------------------------------------------------------------------
// Use driver to discover base address of the surface memory.
// -----------------------------------------------------------------------------
//	  
EXPORT_C TInt CVghwUtils::GetSurfaceBufferBaseAddress( TUint32& aHWAddress )
	{
	UTIL_TRACE("CVghwUtils::GetSurfaceBufferBaseAddress");
	if (!iInitialized || !iDriver)
		{
		UTIL_TRACE("CVghwUtils::GetSurfaceBufferBaseAddress called before initialised VghwUtils: iInitialized=%d, iDriver=0x%x", iInitialized, iDriver);
		InitStatics();
		}
	VGHWPANIC_ASSERT(iInitialized && iDriver, EVghwPanicGraphicsDriverNotOpen);
	return iDriver->GetSurfaceBufferBaseAddress( aHWAddress );
	}

#ifdef _DEBUG
static void EglErrorPrint(char* aTitle, EGLint aEglError)
	{
	char* errName = NULL;
	switch (aEglError)
		{
		case EGL_SUCCESS: errName = "EGL_SUCCESS"; break;
		case EGL_NOT_INITIALIZED: errName = "EGL_NOT_INITIALIZED"; break;
		case EGL_BAD_ACCESS: errName = "EGL_BAD_ACCESS"; break;
		case EGL_BAD_ALLOC: errName = "EGL_BAD_ALLOC"; break;
		case EGL_BAD_ATTRIBUTE: errName = "EGL_BAD_ATTRIBUTE"; break;
		case EGL_BAD_CONFIG: errName = "EGL_BAD_CONFIG"; break;
		case EGL_BAD_CONTEXT: errName = "EGL_BAD_CONTEXT"; break;
		case EGL_BAD_CURRENT_SURFACE: errName = "EGL_BAD_CURRENT_SURFACE"; break;
		case EGL_BAD_DISPLAY: errName = "EGL_BAD_DISPLAY"; break;
		case EGL_BAD_MATCH: errName = "EGL_BAD_MATCH"; break;
		case EGL_BAD_NATIVE_PIXMAP: errName = "EGL_BAD_NATIVE_PIXMAP"; break;
		case EGL_BAD_NATIVE_WINDOW: errName = "EGL_BAD_NATIVE_WINDOW"; break;
		case EGL_BAD_PARAMETER: errName = "EGL_BAD_PARAMETER"; break;
		case EGL_BAD_SURFACE: errName = "EGL_BAD_SURFACE"; break;
		case EGL_CONTEXT_LOST: errName = "EGL_CONTEXT_LOST"; break;
		default: break; // Error is from a new version of EGL
		}

	if (errName)
		{
		UTIL_TRACE( "  %s=%s (0x%x)", aTitle, errName, aEglError);
		}
	else
		{
		UTIL_TRACE( "  %s=0x%x", aTitle, aEglError);
		}
	}


static void VgErrorPrint(char* aTitle, VGErrorCode aVgError)
	{
	char* errName = NULL;
	switch (aVgError)
		{
		case VG_NO_ERROR: errName = "VG_NO_ERROR"; break;
		case VG_BAD_HANDLE_ERROR: errName = "VG_BAD_HANDLE_ERROR"; break;
		case VG_ILLEGAL_ARGUMENT_ERROR: errName = "VG_ILLEGAL_ARGUMENT_ERROR"; break;
		case VG_OUT_OF_MEMORY_ERROR: errName = "VG_OUT_OF_MEMORY_ERROR"; break;
		case VG_PATH_CAPABILITY_ERROR: errName = "VG_PATH_CAPABILITY_ERROR"; break;
		case VG_UNSUPPORTED_IMAGE_FORMAT_ERROR: errName = "VG_UNSUPPORTED_IMAGE_FORMAT_ERROR"; break;
		case VG_UNSUPPORTED_PATH_FORMAT_ERROR: errName = "VG_UNSUPPORTED_PATH_FORMAT_ERROR"; break;
		case VG_IMAGE_IN_USE_ERROR: errName = "VG_IMAGE_IN_USE_ERROR"; break;
		case VG_NO_CONTEXT_ERROR: errName = "VG_NO_CONTEXT_ERROR"; break;
		default: break; // unknown error code from a new version of Open VG
		}

	if (errName)
		{
		UTIL_TRACE( "  %s=%s (0x%x)", aTitle, errName, aVgError);
		}
	else
		{
		UTIL_TRACE( "  %s=0x%x", aTitle, aVgError);
		}
	}


static void GlesErrorPrint(char* aTitle, GLenum aGlesError)
	{
	char* errName = NULL;
	switch (aGlesError)
		{
		case GL_NO_ERROR: errName = "GL_NO_ERROR"; break;
		case GL_INVALID_ENUM: errName = "GL_INVALID_ENUM"; break;
		case GL_INVALID_VALUE: errName = "GL_INVALID_VALUE"; break;
		case GL_INVALID_OPERATION: errName = "GL_INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW: errName = "GL_STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW: errName = "GL_STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY: errName = "GL_OUT_OF_MEMORY"; break;
		default: break; // unknown error code from a new version of Open GL ES
		}

	if (errName)
		{
		UTIL_TRACE( "  %s=%s (0x%x)", aTitle, errName, aGlesError);
		}
	else
		{
		UTIL_TRACE( "  %s=0x%x", aTitle, aGlesError);
		}
	}
#endif


// end of file vghwutils.cpp
