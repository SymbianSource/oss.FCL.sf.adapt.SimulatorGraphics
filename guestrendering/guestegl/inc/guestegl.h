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
// Guest Egl Header file

#ifndef __GUEST__EGL_H
#define __GUEST__EGL_H

#include <graphics/surfacemanager.h>

// CLASS DECLARATION
typedef enum
	{
    EPixmapTypeNone,
    EPixmapTypeFbsBitmap,
    EPixmapTypeSgImage,
	} TSymbianPixmapTypeId;


typedef enum
	{
	ESurfaceTypeWindow = 0,
	ESurfaceTypePbuffer = 1,
	ESurfaceTypePixmapFbsBitmap = 2,
	ESurfaceTypePixmapSgImage = 3,
	} TEglSurfaceType;


class RSurfaceManager;
class CEglContext;

// client side info about a surface
struct TSurfaceInfo
    {
    TSize                   iSize;
    TEglSurfaceType         iSurfaceType; //Surface type
    EGLConfig               iConfigId;
    CFbsBitmap*             iFbsBitmap;       // Handle for CFbsBitmap
#ifdef FAISALMEMON_S4_SGIMAGE
    RSgDrawable             iSgHandle;        // Handle for RSgImge - keeps image open
    TSgDrawableId           iSgId;            // SgImage Id - to detect 2nd attempted surface using the same SgImage 
#endif
    EGLSurface              iHostSurfaceId;
	// Note: most member data is only used by Window surfaces
    TSurfaceId              iSurfaceId;        /*<! Target system surface allocated by EGL.                                              */
    // FAISALMEMON HOLE 0
    RSurfaceManager			iSurfaceManager;
    RSurfaceUpdateSession	iSurfaceUpdateSession;
    void*                   iBuffer0;          /*<! Pointer to the first buffer pixels                                                   */
    void*                   iBuffer1;          /*<! Pointer to the second buffer pixels                                                  */
    TInt                    iBuffer0Index;          /*<! Pointer to the first buffer pixels                                                   */
    TInt                    iBuffer1Index;          /*<! Pointer to the second buffer pixels                                                  */
    RChunk                  iChunk;            /*<! chunk of the backbuffer surface memory                                               */
    TInt					iFrontBuffer;		// We assume zero based index, 0 -> iBuffer0, 1 -> iBuffer1 should be display
    RWindow*                iNativeWindow;
    EGLint                  iRedBits;
    EGLint                  iGreenBits;
    EGLint                  iBlueBits;
    EGLint                  iAlphaBits;
    EGLenum                 iAlphaFormat;
    EGLint                  iColorBits;
    EGLint                  iStride;
    // FAISALMEMON HOLE 2
    };


// Container for Symbian client side information about an EGL Display
NONSHARABLE_CLASS(CEglDisplayInfo) : public CBase
	{
public:
	TBool                              iInitialized;
//	RMutex                             iSurfaceMapLock;
    RHashMap<TInt, TSurfaceInfo*>      iSurfaceMap;
//private:
//	RMutex                             iContextMapLock;
    RHashMap<EGLContext, CEglContext*> iContextMap;
    };


// prototype to help eglGetProcAddress() API 
typedef void (*ProcPointer)(...);


// Collection of static functions for processing EGL Attribute Lists
class TAttribUtils
	{
public: // definitions in eglattribs.cpp
	static TInt AttribListLength(const EGLint* aAttribList);
	static const EGLint* FindAttribValue(const EGLint* aAttribList, EGLint aAttrib);
	// NB only use these on AttribLists with adequate space for editing
	static EGLint* FindAttribValue(EGLint* aAttribList, EGLint aAttrib);
	static void AppendAttribValue(EGLint* aAttribList, EGLint aAttrib, EGLint aValue);
	static void RemoveAttrib(EGLint* aAttribList, EGLint aAttrib);
#ifdef _DEBUG
	// definitions in egldebug.cpp
	static void TraceAttribList(const EGLint* aAttribList);
	static void TraceAttribNames(EGLint aAttrib, EGLint aValue, char** aAttrName, char** aValueName);
	static void TraceGetAttrib(char* aApiName, char* aObjType, EGLDisplay aDisplay, EGLint aObject, EGLint aAttribute, EGLint *aValue, EGLBoolean aResult);
	static void TraceSetAttrib(char* aApiName, char* aObjType, EGLDisplay aDisplay, EGLint aObject, EGLint aAttribute, EGLint aValue);
#endif
	};


// CEglContext represents an EGLContext on Host EGL
NONSHARABLE_CLASS(CEglContext) : public CBase
	{
public:
	// factory function
	static CEglContext* Create(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, CEglContext* aShareContext, const EGLint* aAttribList);
	EGLBoolean MakeCurrent(TEglThreadState& aThreadState, EGLSurface aDraw, EGLSurface aRead);
	EGLBoolean QueryAttribute(TEglThreadState& aThreadState, EGLint aAttribute, EGLint* aValue);
	// These functions return ETrue if they delete themselves 
	TBool Destroy(TEglThreadState& aThreadState);
	TBool MakeNotCurrent(TEglThreadState& aThreadState);
	EGLContext ShareContextFamily();
	inline EGLContext HostContext() { return iHostContext; }
	inline EGLContext ClientContext() { return iHostContext; }
	inline TBool IsDestroyed() { return iIsDestroyed; }

private:
	CEglContext(EGLDisplay aDisplay, EGLConfig aConfig, EGLContext aShareContextFamily, EGLenum aRenderingApi, EGLint aGlesClientVersion);
	void Delete(TEglThreadState& aThreadState);
	~CEglContext();
private:
	RMutex     iCtxMutex;
	TBool      iFirstUse;    // for eglMakeCurrent
	EGLContext iHostContext; // corresponding Host EGL Context
	// info from eglCreateContext & eglMakeCurrent
	EGLDisplay iDisplay;
	EGLContext iShareContextFamily;
	EGLConfig  iConfigId;     // Config id used to create context
	EGLenum    iRenderingApi; // set by current bound API at creation
	EGLSurface iDrawSurface;  // currently bound draw surface, if any
	// for GL ES Contexts
	EGLSurface iReadSurface;  // currently bound read surface, if any
	EGLint     iGlesClientVersion; // set by EGL_CONTEXT_CLIENT_VERSION attribute at creation (default is 1)
	// object lifetime management
	TBool      iIsDestroyed;
	};


NONSHARABLE_CLASS(CEglImage) : public CBase
	{
public:
	CEglImage();
	~CEglImage();
#ifdef FAISALMEMON_S4_SGIMAGE 
	inline void Create(TSgDrawableId aSgImageId, EGLDisplay aDisplay, TSgImageInfo& aSgImageInfo, TInt aPbufferHandle, VGHandle aVgHandle);
	TSgDrawableId SgImageId() const;
#endif
	inline void Duplicate();
	inline TBool OpenForVgImage(TSize& aSize, VGHandle& aVgHandle, TUint64& aSgImageId);
	inline TBool Close();
	inline TBool Destroy();
	inline TBool IsDestroyed() const;
	inline TInt RefCount();
	inline EGLDisplay Display() const;

private:
#ifdef FAISALMEMON_S4_SGIMAGE
	RSgDrawable  iSgHandle;				// Client Handle
	TSgImageInfo iSgImageInfo;
#endif
	EGLDisplay   iDisplay;
	TInt         iPbufferHandle;		// Handle of the underlying Pbuffer, if any
	VGHandle     iVgHandle;				// Handle of the underlying Host VgImage, if any
	TInt         iCreateCount;
	TInt         iOpenCount;
	TBool        iIsDestroyed;
	};

const TInt KEglConfigSize = 29;


class XGuestEglInitialiser
	{
public:
	XGuestEglInitialiser();
	~XGuestEglInitialiser();
	};


/**
 *  CCGuestEGL
 *
 */
NONSHARABLE_CLASS(CGuestEGL) : public CBase, public MEglManagementApi,
		public MVgApiForEgl, public MGles11ApiForEgl, public MGles2ApiForEgl
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CGuestEGL();

    /**
     * Public constructor.
     */
    static CGuestEGL* New();

	// class MEglManagementApi - exported as a vtable via CVghwUtils for Open VG and Open GL ES to access EGL info
	virtual TBool EglImageOpenForVgImage(EGLImageKHR aImage, TSize& aSize, VGHandle& aVgHandle, TUint64& aSgImageId);
	virtual void EglImageClose(EGLImageKHR aImage);


	// Private interfaces for EGL to call into Open VG & Open GL ES 
	// class MVgApiForEgl - redirects via CVghwUtils to exported functions from Open VG
	virtual ExtensionProcPointer guestGetVgProcAddress (const char *aProcName);
	// class MGles11ApiForEgl - redirects via CVghwUtils to exported functions from Open GL ES 1.1
	virtual ExtensionProcPointer guestGetGles11ProcAddress (const char *aProcName);
	// class MGles2ApiForEgl - redirects via CVghwUtils to exported functions from Open GL ES 2
	virtual ExtensionProcPointer guestGetGles2ProcAddress (const char *aProcName);


	// public support functions for EGL C API
	static EGLint CheckColorAttributes(const EGLint* aAttribList, EGLint aColorBufferType, EGLint aLuminanceBits,
			EGLint aRedBits, EGLint aGreenBits, EGLint aBlueBits, EGLint aAlphaBits=0);
	static void AppendColorAttributes(EGLint* aAttribList, EGLint aColorBufferType, EGLint aLuminanceBits, EGLint aRedBits,
			EGLint aGreenBits, EGLint aBlueBits, EGLint aAlphaBits=0, TBool aSetVgPreMultAlpha=EFalse);

	const char *QueryExtensionList();

	// ToDo probably remove this after EGL Sync refactoring
	void SetError( EGLint aError );
	void DestroySurfaceInfo(EGLDisplay aDisplay, EGLSurface aSurface);

	// static functions directly implementing an EGL API
	ProcPointer eglGetProcAddress(const char *aProcName);

	// functions directly implementing an EGL API
	EGLBoolean eglBindTexImage(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLint aBuffer);
	EGLBoolean eglChooseConfig(TEglThreadState& aThreadState, EGLDisplay aDisplay, const EGLint *aAttribList,
				   EGLConfig *aConfigs, EGLint aConfigSize, EGLint *aNumConfig);
	EGLBoolean eglCopyBuffers(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLNativePixmapType aTarget);
	EGLContext eglCreateContext(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig,
					EGLContext aShareContext, const EGLint *aAttribList);
	EGLImageKHR eglCreateImageKHR(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLContext aContext, EGLenum aTarget,
			EGLClientBuffer aBuffer, const EGLint *aAttribList);
	EGLSurface eglCreatePbufferFromClientBuffer(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLenum aBufType,
			EGLClientBuffer aBuffer, EGLConfig aConfig, const EGLint *aAttribList);
	EGLSurface eglCreatePbufferSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, const EGLint *aAttribList);
	EGLSurface eglCreatePixmapSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, 
			EGLNativePixmapType aNativePixmap, const EGLint *aAttribList);
	EGLSurface eglCreateWindowSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, 
			EGLNativeWindowType aNativeWindow, const EGLint *aAttribList);
	EGLBoolean eglDestroyContext(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLContext aContext);
	EGLBoolean eglDestroyImageKHR(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLImageKHR aImage);
	EGLBoolean eglDestroySurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface);
	EGLBoolean eglGetConfigAttrib(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig,
				      EGLint aAttribute, EGLint *aValue);
	EGLBoolean eglGetConfigs(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig *aConfigs,
				 EGLint aConfigSize, EGLint *aNumConfig);
	EGLContext eglGetCurrentContext(TEglThreadState& aThreadState);
	EGLDisplay eglGetCurrentDisplay(TEglThreadState& aThreadState);
	EGLSurface eglGetCurrentSurface(TEglThreadState& aThreadState, EGLint aReadDraw);
	EGLDisplay eglGetDisplay(TEglThreadState& aThreadState, EGLNativeDisplayType aDisplayId);
	EGLBoolean eglInitialize(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLint *aMajor, EGLint *aMinor);
	EGLBoolean eglMakeCurrent(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aDraw, EGLSurface aRead,
			EGLContext aContext);
	EGLBoolean eglQueryContext(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLContext aContext, EGLint aAttribute, EGLint *aValue);
	const char* eglQueryString(EGLDisplay aDisplay, EGLint aName);
	EGLBoolean eglQuerySurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLint aAttribute,
			EGLint *aValue);
	EGLBoolean eglReleaseTexImage(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLint aBuffer);
	EGLBoolean eglSurfaceAttrib(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface,
				    EGLint aAttribute, EGLint aValue);
	EGLBoolean eglSwapBuffers(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface);
	EGLBoolean eglSwapInterval(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLint aInterval);
	EGLBoolean eglTerminate(TEglThreadState& aThreadState, EGLDisplay aDisplay);
	EGLBoolean eglWaitClient(TEglThreadState& aThreadState);

public:
    // API supporting EGL sync extension
    /*-------------------------------------------------------------------*//*!
     * \brief   Query and request to lock a specified display
     * \ingroup eglSync
     * \param   aEglDisplay a display identifier
     * \return  EGL_SUCCESS if successful;
     *          EGL_BAD_DISPLAY is not a name of a valid EGLDisplay
     *          EGL_NOT_INITIALIZED if the display object associated
     *          with the <aEglDisplay> has not been initialized
     *//*-------------------------------------------------------------------*/
    EGLint FindAndLockDisplay(EGLDisplay aDisplay);

    /*-------------------------------------------------------------------*//*!
     * \brief   Releases the lock associated with a valid EGLDisplay
     * \ingroup eglSync
     * \param   aEglDisplay a display identifier
     *//*-------------------------------------------------------------------*/
    void ReleaseDisplayLock(EGLDisplay aDisplay);

    /*-------------------------------------------------------------------*//*!
     * \brief   Returns pointer to the EGL sync instance
     * \ingroup eglSync
     *//*-------------------------------------------------------------------*/
    CEglSyncExtension* EGLSyncExtension();

private:
    /**
     * private constructor, 2nd phase constructor
     */
	CGuestEGL();
	void Create();
	void CloseSgResources();
	void OpenSgResources();

	// private function with "EglInternalFunction_" prefix
	TBool EglInternalFunction_CreateSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, EGLConfig aConfig, RWindow* aNativeWindow, TSurfaceInfo& aSurfaceInfo);
	void EglInternalFunction_DestroyWindowSurface(TSurfaceInfo& aSurfaceInfo);
	TUidPixelFormat EglInternalFunction_GetSymbianPixelFormat(const TSurfaceInfo& aSurfaceInfo);
	EGLBoolean EglInternalFunction_CallSetSurfaceParams(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, TSurfaceInfo& aSurfaceInfo);
	TBool EglInternalFunction_PixmapSurfacePreviouslyCreated(EGLNativePixmapType pixmap, TSymbianPixmapTypeId pixmapType);
	static TSymbianPixmapTypeId EglInternalFunction_GetNativePixmapType(EGLNativePixmapType pixmap);
	const char* EglInternalFunction_QueryExtensionList();
	TBool EglInternalFunction_IsValidNativePixmap(EGLNativePixmapType pixmap, TSymbianPixmapTypeId pixmapType);
	EGLBoolean EglInternalFunction_SurfaceResized(TEglThreadState& aThreadState, TSurfaceInfo& aSurfaceInfo, EGLDisplay aDisplay, EGLSurface aSurface);
	TSurfaceInfo* EglInternalFunction_GetPlatformSurface( EGLDisplay display, EGLSurface surface );
	void EglInternalFunction_AbortWindowInit(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface, TSurfaceInfo* ps, RWindow* window, CWsScreenDevice* screenDevice);
	TBool EglInternalFunction_SwapWindowSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLSurface aSurface);
	TBool EglInternalFunction_MetaGetConfigs( TEglThreadState& aThreadState, EGLConfig*& aConfigs, EGLint& aConfigCnt, EGLint*& aConfigAttribs, EGLint& aConfigAttribsLen, TMetaGetConfigsMode aFetchMode = EMetaGetConfigsSg );

	// more private functions
	
	EGLint ConfigMatchesFbsBitmapPixmap(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, const EGLint* aAttribList, TDisplayMode aMode);
	EGLBoolean ChooseConfigForPixmapSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, const EGLint* aAttribList, EGLConfig* aConfigs, EGLint aConfigSize,
			EGLint* aNumConfig, const void* aPixmap);
	EGLSurface CreateFbsBitmapSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, CFbsBitmap* aBitmap, const EGLint *aAttribList, TSurfaceInfo& aSurfaceInfo);
	
	EGLint ChooseConfigAttribsForFbsBitmap(TEglThreadState& aThreadState, const EGLint* aAttribList, const CFbsBitmap* aBitmap, EGLint** aNewList);
	
	EGLint ValidateEglImageTarget(EGLDisplay aDisplay, EGLContext aContext, EGLenum aTarget,
			EGLClientBuffer aBuffer, const EGLint *aAttribList, TSymbianPixmapTypeId aTargetPixmapType);
	
	
	TBool CreateDisplayInfo(EGLDisplay aDisplay);
	TBool InitializeDisplayInfo(EGLDisplay aDisplay);
	TBool IsDisplayInitialized(EGLDisplay aDisplay);
	TBool DestroyDisplayInfo(EGLDisplay aDisplay);
    
#ifdef FAISALMEMON_S4_SGIMAGE
    EGLSurface CreateSgImageSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, EGLConfig aConfig, const RSgImage* aSgImage, const EGLint *aAttribList,
			TSurfaceInfo& aSurfaceInfo);
    EGLImageKHR CreateNewEglImage(TEglThreadState& aThreadState, EGLDisplay aDisplay, TSgDrawableId aSgId, TSgImageInfo aSgImageInfo);
    EGLImageKHR DuplicateEglImageIfItExists(EGLDisplay aDisplay, TSgDrawableId aSgId);
    EGLint ChooseConfigAttribsForSgImage(TEglThreadState& aThreadState, const EGLint* aAttribList, const RSgImage* aSgImage, EGLint** aNewList);
    EGLBoolean ChooseConfigForNativeSgImagePixmapSurface(TEglThreadState& aThreadState, EGLDisplay aDisplay, const EGLint* aAttribList, EGLConfig* aConfigs, EGLint aConfigSize,
			EGLint* aNumConfig, const EGLint* aPixmap);
#endif
private:
	//new methods supporting EGL Sync extension
	EGLint InitialiseExtensions();

private:
	RReadWriteLock				 iDisplayMapLock; // protects the iDisplayMap for multi-threaded clients
	RHashMap<TInt, CEglDisplayInfo*> iDisplayMap;

	RMutex                       iEglImageLock; // protects the iEglImageArray for multi-threaded clients
	RPointerArray<CEglImage>     iEglImageArray; // ToDo use a Unique Id for the index/handle, rather than the object address

#ifdef FAISALMEMON_S4_SGIMAGE
	EGLint* iSgConfigAttribs;
	EGLint  iSgConfigsAttribtCnt;
	// ToDo (SgImage Design Spec 3.5) ARM EGL should maintain its own lists of native window handles, pixmap handles etc

	char* iEglExtnNameList; // supporting data for eglQueryString()
    RSgDriver                    iSgDriver;
#endif
	//data members supporting EGL Sync extension
	CEglSyncExtension* iEglSyncExtension;
	// vtable pointers populated from CVghwUtils - to access Open VG & GL ES internals, e.g. to support eglGetProcAddress
	MVgApiForEgl*        iVgApiForEgl;
	MGles11ApiForEgl*    iGles11ApiForEgl;
	MGles2ApiForEgl*     iGles2ApiForEgl;
	};


/* Execute host EGL functions that cannot fail
   These functions do not set the flag to say that an EGL command has been executed since the last host eglGetError.
   */

inline EGLContext ExecEglContextNoErrorCmd(TEglThreadState& aThreadState, EglRFC& aEglApiData)
	{
	aThreadState.ExecuteEglNeverErrorCmd(aEglApiData);
	return (EGLContext) aEglApiData.ReturnValue();
	}

inline EGLSurface ExecEglSurfaceNoErrorCmd(TEglThreadState& aThreadState, EglRFC& aEglApiData)
	{
	aThreadState.ExecuteEglNeverErrorCmd(aEglApiData);
	return (EGLSurface) aEglApiData.ReturnValue();
	}

inline EGLDisplay ExecEglDisplayNoErrorCmd(TEglThreadState& aThreadState, EglRFC& aEglApiData)
	{
	aThreadState.ExecuteEglNeverErrorCmd(aEglApiData);
	return (EGLDisplay) aEglApiData.ReturnValue();
	}


#endif // __GUEST__EGL_H
