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
// Header file defining Open VG client-side state classes.

#ifndef VGSTATE_H
#define VGSTATE_H

#include <EGL/egl.h>
#include <VG/openvg.h>
#include <e32hashtab.h>
#include "vghwutils.h"

#define _DEBUG // FAISALMEMON temporary

// tracing

#include <e32debug.h>
#ifdef _DEBUG
	#define OPENVG_TRACE(fmt, args...) RDebug::Printf(fmt, ##args)
	#define VGPANIC_ASSERT(condition, panic) if (!(condition)) { VgPanic(panic, #panic, #condition, __FILE__, __LINE__); }
	#define VGPANIC_ASSERT_DEBUG(condition, panic) if (!(condition)) { VgPanic(panic, #panic, #condition, __FILE__, __LINE__); }
	#define VGPANIC_ALWAYS(panic) { VgPanic(panic, #panic, NULL, __FILE__, __LINE__); }
#else
	#define OPENVG_TRACE(fmt, args...)
	#define VGPANIC_ASSERT(condition, panic) if (!(condition)) { VgPanic(panic, NULL, NULL, NULL, __LINE__); }
	#define VGPANIC_ASSERT_DEBUG(condition, panic)
	#define VGPANIC_ALWAYS(panic) { VgPanic(panic, NULL, NULL, NULL, __LINE__); }
#endif


// Guest Open VG panic codes
typedef enum
	{
	EVgPanicTemp = 1, // note: for development, references should be replaced with a more specific panic
	EVgPanicNullPixmapPointer,
	EVgPanicPixmapSizeError,
	EVgPanicSingletonAlreadyInitialised,
	EVgPanicSingletonNotInitialised,
	EVgPanicNullPointer,
	EVgPanicBadLastBitsParam,
	EVgPanicBadRowLengthParam,
	EVgPanicBadRowCountParam,
	EVgPanicStrideSmallerThanRowLength, // 10
	EVgPanicNotReplyOpcode,
	EVgPanicDeleteInUseVgImageInfo,
	EVgPanicUidGeneratorZeroSeed,
	EVgPanicMissingEglInterface,
	EVgPanicNullCVgHandlePointer,
	EVgPanicWrongVgHandleType,
	EVgPanicHandleAlreadyMarkedDestroyed,
	EVgPanicUnknownVgHandleType,
	EVgPanicHostHandleIsInvalid,
	EVgPanicImageParentIsInvalid, // 20
	EVgPanicFunctionParameterIsNull,
	EVgPanicValueFromHostVgIsInvalid,
	EVgPanicHostHandleAlreadySet,
	EVgPanicParentImageAlreadyDestroyed,
	EVgPanicStateLockMutexNotHeld,
	} TVgPanic;

void VgPanic(TVgPanic aPanicCode, char* aPanicName, char* aCondition, char* aFile, TInt aLine);


// state singleton, in Writeable Static Data memory
class XOpenVgState;
extern XOpenVgState OpenVgState;

// classes declared in this header
class CVgHandleBase;
class CVgImageBase;
class CVgMaskLayerInfo;
class CVgFontInfo;
class CVgImageInfo;
class CVgPaintInfo;
class CVgPathInfo;
class THandleGen;
class TVgLockHolder;

extern const float KFloatMinusOne;


class THandleGen
	{
public:
	inline void Init(TUint aInit);
	TUint NextUid();
private:
	TUint iValue;
	};

typedef enum
	{
	EVgHandleAny = -1, // match any valid handle type, e.g. for vgSetParameteri
	EVgHandleForFont = 0, // specific types ...
	EVgHandleForImage,
	EVgHandleForPaint,
	EVgHandleForPath,
	EVgHandleForMaskLayer,
	} TVgHandleType;


// state structures for VG objects (i.e. anything that is a VGHandle): VGImage, VGPath, VGMaskLayer, VGPaint, VGFont
// ToDo review how to handle thread locking these objects whilst they are in use
NONSHARABLE_CLASS(CVgHandleBase) : public CBase // Base Class
	{
public:
	static void InitStatics();

	// public inline methods 
	inline VGHandle HostHandle() const;
	inline TVgHandleType HandleType() const;
	inline TBool IsDestroyed() const;
	inline void SetHostHandle(VGHandle aHostHandle);
	inline TUint ClientHandle() const;

	// general & specific portions of derived object destruction
	void Destroy(MVgContext& aVgContext);
	virtual TBool DestroyObject(MVgContext& aVgContext) = 0;

	// Each VGHandle object type must implement GetParameterVectorSize
	virtual VGint GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType) = 0;
	// Base version of  GetParameter* uses GetParameterVectorSize to validate aParamType then calls Host VG
	virtual VGfloat GetParameterf(MVgContext& aVgContext, VGint aParamType);
	virtual VGint GetParameteri(MVgContext& aVgContext, VGint aParamType);
	// For details of Base versions of GetParameter*v see vghandle.cpp
	virtual void GetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, VGfloat * aValues);
	virtual void GetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, VGint * aValues);
	// Base versions of SetParameter* assume parameters are read-only, and give an error return
	virtual void SetParameterf(MVgContext& aVgContext, VGint aParamType, VGfloat aValue);
	virtual void SetParameteri(MVgContext& aVgContext, VGint aParamType, VGint aValue);
	virtual void SetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGfloat * aValues);
	virtual void SetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGint * aValues);

protected:
	inline CVgHandleBase(TVgHandleType aHandleType);
	inline ~CVgHandleBase();

	VGint HostVgGetParameterVectorSize(MVgContext& aVgContext, VGint aParamType);
	VGint HostVgGetParameteri(MVgContext& aVgContext, VGint aParamType);
	VGfloat HostVgGetParameterf(MVgContext& aVgContext, VGint aParamType);
	void HostVgGetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGfloat * aValues);
	void HostVgGetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGint * aValues);
	void HostVgSetParameterf(MVgContext& aVgContext, VGint aParamType, VGfloat aValue);
	void HostVgSetParameteri(MVgContext& aVgContext, VGint aParamType, VGint aValue);
	void HostVgSetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGfloat * aValues);
	void HostVgSetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGint * aValues);

protected:
	TUint iClientHandle;
	TVgHandleType iHandleType;
	VGHandle iHostHandle;
	TBool iIsDestroyed;

private:
	static THandleGen iHandleGen; // generator for unique client side handles
	};


NONSHARABLE_CLASS(CVgImageBase) : public CVgHandleBase
	{
public:
	inline VGint Width() const;
	inline VGint Height() const;

	TBool DestroyObject(MVgContext& aVgContext) = 0;

	virtual VGint GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType) = 0;

protected:
	inline CVgImageBase(TVgHandleType aHandleType, VGint aWidth, VGint aHeight);

protected:
	VGint const   iWidth;
	VGint const   iHeight;
	};


NONSHARABLE_CLASS(CVgMaskLayerInfo) : public CVgImageBase
	{
public:
	static CVgMaskLayerInfo* New(VGint aWidth, VGint aHeight);
	virtual TBool DestroyObject(MVgContext& aVgContext);

	virtual VGint GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType);

	// Mask specific operations
	void FillMaskLayer(MVgContext& aVgContext, VGint aX, VGint aY, VGint aWidth, VGint aHeight, VGfloat aValue);
	void CopyMask(MVgContext& aVgContext, VGint aSx, VGint aSy, VGint aDx, VGint aDy, VGint aWidth, VGint aHeight);

private:
	inline CVgMaskLayerInfo(VGint aWidth, VGint aHeight);
	inline ~CVgMaskLayerInfo();
	};


NONSHARABLE_CLASS(CVgImageInfo) : public CVgImageBase
	{
public:
	static CVgImageInfo* New(VGImageFormat aFormat, VGint aWidth, VGint aHeight, VGbitfield aAllowedQuality);
	static CVgImageInfo* New(VGImageFormat aFormat, VGint aWidth, VGint aHeight, EGLImageKHR aEglImage, TUint64& aSgImageId);
	static CVgImageInfo* New(VGint aWidth, VGint aHeight, CVgImageInfo* aParent);
	virtual TBool DestroyObject(MVgContext& aVgContext);

	TInt ChildCount() const;
	CVgImageInfo* Parent() const;
	VGbitfield AllowedQuality() const;
	TBool IsEglSibling() const;
	TUint64 SgImageId() const;
	EGLImageKHR EglImage() const;

	static TInt BitsPerPixelForVgImageFormat(VGImageFormat aFormat);
	static void PixmapBlit(TUint8* aDest, const TUint8* aSource, TInt aDestStride, TInt aSourceStride, TInt aRowCount,
			size_t aRowLength, TInt aLastBits=0);

	virtual VGint GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType);
	virtual VGfloat GetParameterf(MVgContext& aVgContext, VGint aParamType);
	virtual VGint GetParameteri(MVgContext& aVgContext, VGint aParamType);

	void ClearImage(MVgContext& aVgContext, VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	void ImageSubData(MVgContext& aVgContext, const void * aData, VGint aDataStride, VGImageFormat aDataFormat, VGint aX, VGint aY,
			VGint aWidth, VGint aHeight);
	void GetImageSubData(MVgContext& aVgContext, void * aData, VGint aDataStride, VGImageFormat aDataFormat, VGint aX, VGint aY,
			VGint aWidth, VGint aHeight);
	void CopyImage(MVgContext& aVgContext, VGint aDx, VGint aDy, CVgImageInfo& aSrcImageInfo, VGint aSx, VGint aSy, VGint aWidth,
			VGint aHeight, VGboolean aDither);
	void DrawImage(MVgContext& aVgContext);
	void SetPixels(MVgContext& aVgContext, VGint aDx, VGint aDy, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight);
	void GetPixels(MVgContext& aVgContext, VGint aDx, VGint aDy, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight);
	void ColorMatrix(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, const VGfloat * aMatrix);
	void Convolve(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, VGint aKernelWidth, VGint aKernelHeight,
			VGint aShiftX, VGint aShiftY, const VGshort * aKernel, VGfloat aScale, VGfloat aBias, VGTilingMode aTilingMode);
	void SeparableConvolve(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, VGint aKernelWidth, VGint aKernelHeight,
			VGint aShiftX, VGint aShiftY, const VGshort * aKernelX, const VGshort * aKernelY, 
			VGfloat aScale, VGfloat aBias, VGTilingMode aTilingMode);
	void GaussianBlur(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, VGfloat aStdDeviationX, VGfloat aStdDeviationY, VGTilingMode aTilingMode);
	void Lookup(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, const VGubyte * aRedLUT, const VGubyte * aGreenLUT, const VGubyte * aBlueLUT,
			const VGubyte * aAlphaLUT, VGboolean aOutputLinear, VGboolean aOutputPremultiplied);
	void LookupSingle(MVgContext& aVgContext, CVgImageInfo& aSrcImageInfo, const VGuint * aLookupTable, VGImageChannel aSourceChannel,
			VGboolean aOutputLinear, VGboolean aOutputPremultiplied);

private:
	inline CVgImageInfo(VGImageFormat aFormat, VGint aWidth, VGint aHeight, CVgImageInfo* aParent, VGbitfield aAllowedQuality,
			EGLImageKHR aEglImage, TUint64 aSgImageId);
	inline ~CVgImageInfo();
	inline void IncChildCount();
	inline void DecChildCount();
	void HostVgGetImageSubData(MVgContext& aVgContext, void* aPixmap, size_t aPixmapSize, VGint aHostDataStride,
			VGImageFormat aDataFormat, VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	void HostVgImageSubData(MVgContext& aVgContext, const void* aPixmap, size_t aPixmapSize, VGint aHostDataStride,
			VGImageFormat aDataFormat, VGint aX, VGint aY, VGint aWidth, VGint aHeight);


	/* **** desireable: expand CVgImageInfo to include and use for validating for various appropriate:
		(TBool iIsRenderTarget; - VgImage is a surface currently being rendered, blocks some operations)
		(TUint iUsedAsPaintPattern; - non-zero stops use as a render target)
		(TUint iUsedAsGlyph; - count of how many places the VgImage is being used as a font glyph)
	   */
private:
	VGImageFormat       iFormat; // not const because this is not currently initialised at creation for EglImages
	CVgImageInfo* const iParent; // immediate parent VgImage, NULL if not a child
	VGbitfield    const iAllowedQuality;
	// EglImage / SgImage info ...
	TBool         const iIsEglSibling;
	EGLImageKHR   const iEglImage;
	TUint64       const iSgImageId;      // Set or copied from parent for Egl Sibling - used to prompt Cmd Scheduler to sync pbuffer & vgimage
	// non-const state
	TInt          iChildCount;     // count of direct child images
	};


NONSHARABLE_CLASS(CVgFontInfo) : public CVgHandleBase
	{
public:
	static CVgFontInfo* New();
	virtual TBool DestroyObject(MVgContext& aVgContext);

	virtual VGint GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType);

	void SetGlyphToPath(MVgContext& aVgContext, VGuint aGlyphIndex, CVgPathInfo* aPathInfo, VGboolean aIsHinted,
			const VGfloat aGlyphOrigin [2], const VGfloat aEscapement[2]);
	void SetGlyphToImage(MVgContext& aVgContext, VGuint aGlyphIndex, CVgImageInfo* aImageInfo, const VGfloat aGlyphOrigin [2],
			const VGfloat aEscapement[2]);
	void ClearGlyph(MVgContext& aVgContext, VGuint aGlyphIndex);
	void DrawGlyph(MVgContext& aVgContext, VGuint aGlyphIndex, VGbitfield aPaintModes, VGboolean aAllowAutoHinting);
	void DrawGlyphs(MVgContext& aVgContext, VGint aGlyphCount, const VGuint * aGlyphIndices, const VGfloat * aAdjustmentsX,
			const VGfloat * aAdjustmentsY, VGbitfield aPaintModes, VGboolean aAllowAutoHinting);

private:
	inline CVgFontInfo();
	inline ~CVgFontInfo();
	};


NONSHARABLE_CLASS(CVgPaintInfo) : public CVgHandleBase
	{
public:
	static CVgPaintInfo* New();
	virtual TBool DestroyObject(MVgContext& aVgContext);

	virtual VGint GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType);
	virtual VGfloat GetParameterf(MVgContext& aVgContext, VGint aParamType);
	virtual VGint GetParameteri(MVgContext& aVgContext, VGint aParamType);
	virtual void GetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, VGfloat * aValues);
	virtual void GetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, VGint * aValues);

	virtual void SetParameterf(MVgContext& aVgContext, VGint aParamType, VGfloat aValue);
	virtual void SetParameteri(MVgContext& aVgContext, VGint aParamType, VGint aValue);
	virtual void SetParameterfv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGfloat * aValues);
	virtual void SetParameteriv(MVgContext& aVgContext, VGint aParamType, VGint aCount, const VGint * aValues);

	static void ResetPaint(MVgContext& aVgContext);
	void SetPaint(MVgContext& aVgContext, VGbitfield aPaintModes);
	void SetColor(MVgContext& aVgContext, VGuint aRgba);
	VGuint GetColor(MVgContext& aVgContext);
	void PaintPattern(MVgContext& aVgContext, CVgImageInfo* aPatternInfo);

private:
	inline CVgPaintInfo();
	inline ~CVgPaintInfo();

private:
	// cached parameters
	VGPaintType           iPaintType;
	VGColorRampSpreadMode iColorRampSpreadMode;
	VGboolean             iColorRampPremultiplied;
	VGTilingMode          iPatternTilingMode;
	// VGfloat vector parameters are not currently cached in client
	};


NONSHARABLE_CLASS(CVgPathInfo) : public CVgHandleBase
	{
public:
	static CVgPathInfo* CVgPathInfo::New(VGPathDatatype aDatatype, VGfloat aScale, VGfloat aBias, VGbitfield aCapabilities);
	virtual TBool DestroyObject(MVgContext& aVgContext);

	void ClearPath(MVgContext& aVgContext, VGbitfield aCapabilities);
	void RemovePathCapabilities(MVgContext& aVgContext, VGbitfield aCapabilities);
	VGbitfield GetPathCapabilities(MVgContext& aVgContext);

	void AppendPath(MVgContext& aVgContext, CVgPathInfo& aSrcPathInfo);
	void AppendPathData(MVgContext& aVgContext, VGint aNumSegments, const VGubyte * aPathSegments, const void* aPathData);
	void ModifyPathCoords(MVgContext& aVgContext, VGint aStartIndex, VGint aNumSegments, const void* aPathData);
	void TransformPath(MVgContext& aVgContext, CVgPathInfo& aSrcPathInfo);
	VGboolean InterpolatePath(MVgContext& aVgContext, CVgPathInfo& aStartPathInfo, CVgPathInfo& aEndPathInfo, VGfloat aAmount);
	VGfloat PathLength(MVgContext& aVgContext, VGint aStartSegment, VGint aNumSegments);
	void PointAlongPath(MVgContext& aVgContext, VGint aStartSegment, VGint aNumSegments, VGfloat aDistance,
			VGfloat* aX, VGfloat* aY, VGfloat* aTangentX, VGfloat* aTangentY);
	void PathBounds(MVgContext& aVgContext, VGfloat* aMinX, VGfloat* aMinY, VGfloat* aWidth, VGfloat* aHeight);
	void PathTransformedBounds(MVgContext& aVgContext, VGfloat* aMinX, VGfloat* aMinY, VGfloat* aWidth, VGfloat* aHeight);
	void DrawPath(MVgContext& aVgContext, VGbitfield aPaintModes);
	void RenderToMask(MVgContext& aVgContext, VGbitfield aPaintModes, VGMaskOperation aOperation);

	inline VGPathDatatype PathDatatype() const;

	TBool CheckHasPathCapabilities(MVgContext& aVgContext, VGbitfield aCapabilities);

	virtual VGint GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType);
	virtual VGfloat GetParameterf(MVgContext& aVgContext, VGint aParamType);
	virtual VGint GetParameteri(MVgContext& aVgContext, VGint aParamType);

private:
	inline CVgPathInfo(VGPathDatatype aPathDatatype, VGfloat aScale, VGfloat aBias, VGbitfield aCapabilities);
	inline ~CVgPathInfo();
	TBool CheckAppendPathData(MVgContext& aVgContext, VGint aNumSegments, const VGubyte* aPathSegments, const void* aPathData,
			VGint* aPathDataSize);
	TBool CheckPathDataAlignment(MVgContext& aVgContext, const void* aPathData);
	TBool CheckPathDataSize(MVgContext& aVgContext, const VGubyte* aPathSegments, VGint aNumSegments, VGint* aPathDataSize);
	VGint HostVgeGetPathCoordsSizeInBytes(MVgContext& aVgContext, VGint aStartIndex, VGint aNumSegments);

private:
	VGPathDatatype const iDatatype;
	VGfloat const        iScale;
	VGfloat const        iBias; 
	VGbitfield           iCapabilities;
	};


// Collection of static parameter checking functions - all "Chk" functions set the appropriate last VG Error on failure 
class TCheck
	{
public:
	inline static TBool Chk16bitPtr(MVgContext& aVgContext, const void* aDataPtr);
	inline static TBool Chk2x16bitPtr(MVgContext& aVgContext, const void* aDataPtr1, const void* aDataPtr2);
	inline static TBool Chk32bitPtr(MVgContext& aVgContext, const void* aDataPtr);
	inline static TBool Chk2x32bitPtr(MVgContext& aVgContext, const void* aDataPtr1, const void* aDataPtr2);
	inline static TBool Chk4x32bitPtr(MVgContext& aVgContext, const void* aDataPtr1, const void* aDataPtr2, const void* aDataPtr3, const void* aDataPtr4);
	inline static TBool Chk2xOptional32bitPtr(MVgContext& aVgContext, const void* aDataPtr1, const void* aDataPtr2);
	inline static TBool ChkVGMaskOperation(MVgContext& aVgContext, VGMaskOperation aOperation);
	inline static TBool ChkVGPaintMode(MVgContext& aVgContext, VGPaintMode aPaintMode);
	inline static TBool ChkVGPathDataType(MVgContext& aVgContext, VGPathDatatype aDatatype);
	inline static TBool ChkVGTilingMode(MVgContext& aVgContext, VGTilingMode aTilingMode);
	inline static TBool ChkVgImageFormat(MVgContext& aVgContext, VGImageFormat aFormat);
	inline static TBool ChkImageAlignment(MVgContext& aVgContext, VGImageFormat aImageFormat, const void* aData);
	inline static TBool ChkPositiveImageSize(MVgContext& aVgContext, VGint aWidth, VGint aHeight);
	inline static TBool ChkNewImageSize(MVgContext& aVgContext, VGint aWidth, VGint aHeight);
	inline static TBool ChkVGPaintModesCombination(MVgContext& aVgContext, VGbitfield aPaintModes);
	inline static TBool ChkVGPathFormat(MVgContext& aVgContext, VGint aPathFormat);
	static TBool ChkAreaIsWithinImage(MVgContext& aVgContext, CVgImageBase* aImageInfo, VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	static TBool ChkParamCountAndValuesPtr(MVgContext& aVgContext, VGint aCount, const void* aValues);
	static VGint ImageFormatByteDepth(VGImageFormat aImageFormat);
	};


/*
 Class holds mutex for Open VG client side state on construction, & releases it on destruction.

 Placing methods here ensures the procedural requirement that the Mutex is held before these methods are called.
 */ 
class TCleanupVgLocks
	{
public:
	// having functions here forces the caller to hold the Open VG Mutex before they are called (when the lock object is constructed)
	TCleanupVgLocks(MVgContext& aVgContext);
	~TCleanupVgLocks();
	void WaitForMutex(RMutex& aMutex);
	void SignalMutex();

	inline TBool CheckVGFont(VGFont aFont, CVgFontInfo** aFontInfo);
	inline TBool CheckVGImage(VGImage aImage, CVgImageInfo** aImageInfo);
	inline TBool CheckOptionalVGImage(VGImage aImage, CVgImageInfo** aImageInfo);
	inline TBool Check2VGImages(VGImage aImage1, CVgImageInfo** aImageInfo1, VGImage aImage2, CVgImageInfo** aImageInfo2);
	inline TBool CheckVGMaskLayer(VGMaskLayer aMaskLayer, CVgMaskLayerInfo** aHandleInfo);
	inline TBool CheckVGPaint(VGPaint aPaint, CVgPaintInfo** aPaintInfo);
	inline TBool CheckOptionalVGPaint(VGPaint aPaint, CVgPaintInfo** aPaintInfo);
	// ToDo maybe add a version that checks Start Segment & Segment Count
	inline TBool CheckVGPath(VGPath aPath, CVgPathInfo** aPathInfo, VGbitfield aReqdCapabilities=0);
	inline TBool CheckOptionalVGPath(VGPath aPath, CVgPathInfo** aPathInfo);
	inline TBool CheckVGAnyHandle(VGHandle aHandle, CVgHandleBase** aHandleInfo);
	TBool CheckVGMaskOperationAndHandle(VGMaskOperation aOperation, VGHandle aMask, CVgImageBase** aImageInfo);
	inline TBool CheckKernelWidthAndHeight(VGint aKernelWidth, VGint aKernelHeight, VGParamType aLimitType);

	VGFont CreateFont(VGint aGlyphCapacityHint);
	VGImage ChildImage(CVgImageInfo& aParentInfo, VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	VGImage CreateImage(VGImageFormat aFormat, VGint aWidth, VGint aHeight, VGbitfield aAllowedQuality);
	VGMaskLayer CreateMaskLayer(VGint aWidth, VGint aHeight);
	VGPaint CreatePaint();
	VGPaint GetPaint(VGPaintMode aPaintMode);
	VGPath CreatePath(VGint aPathFormat, VGPathDatatype aDatatype, VGfloat aScale, VGfloat aBias,
			VGint aSegmentCapacityHint, VGint aCoordCapacityHint, VGbitfield aCapabilities);
	VGImage CreateEGLImageTargetKHR(VGeglImageKHR aImage);

private:
	MVgContext& const iVgContext;
	RMutex&           iMutex;
	TBool             iIsHeld;
	};



// per VG client state
NONSHARABLE_CLASS(XOpenVgState) : public MEglManagementApi, public MVgApiForEgl
	{
	friend class TCleanupVgLocks;
public:
	XOpenVgState();
	~XOpenVgState();

	// functions using/maintaining Open Vg Handle Map
	TBool AddToHashMap(MVgContext& aVgContext, CVgHandleBase* aHandleInfo, VGHandle aHostHandle);
	void UnMapHandle(TUint32 aClientHandle);

	// MEglManagementApi - redirects via CVghwUtils to the EGL implementation
	virtual TBool EglImageOpenForVgImage(EGLImageKHR aImage, TSize& aSize, VGHandle& aVgHandle, TUint64& aSgImageId);
	virtual void EglImageClose(EGLImageKHR aImage);

	// MVgApiForEgl - exported via CvghwUtils to the Guest EGL implementation
	virtual ExtensionProcPointer guestGetVgProcAddress (const char *aProcName);

protected: // fns only visible to TVgLockHolder class
	inline RMutex& MutexWait();
	TBool CheckVGHandle(MVgContext& aVgContext, VGHandle aHandle, CVgHandleBase** aHandleInfo, TVgHandleType aMatchType);
	// functions using cached values from Host Open VG implementation
	TBool CheckKernelWidthAndHeight(MVgContext& aVgContext, VGint aKernelWidth, VGint aKernelHeight, VGParamType aLimitType);

private:
	RMutex                          iStateLock;

	// Map holds info about VG Handle based objects
	RMutex                          iMapLock;
	RHashMap<TUint, CVgHandleBase*> iHandleHashMap;
	MEglManagementApi*              iEglManagementApi;

	// 'constants' initialised from x86 VG implementation the first time they are required
	VGint iKVgMaxKernelSize;
	VGint iKVgMaxSeparableKernelSize;
	// ToDo: more values to cache: VG_MAX_IMAGE_WIDTH, VG_MAX_IMAGE_HEIGHT, VG_MAX_IMAGE_PIXELS

	// ToDo Record when VG fonts use glyphs based on SgImages, required for Ref counting
	};




// class of static methods to implement Open VG APIs
class TGuestOpenVg
	{
public:
	// VG APIs
	static VGPaint vgGetPaint(VGPaintMode aPaintMode);
	static VGPaint vgCreatePaint();
	static VGboolean vgInterpolatePath(VGPath aDstPath, VGPath aStartPath, VGPath aEndPath, VGfloat aAmount);
	static VGfloat vgPathLength(VGPath aPath, VGint aStartSegment, VGint aNumSegments);
	static VGbitfield vgGetPathCapabilities(VGPath aPath);
	static VGPath vgCreatePath(VGint aPathFormat, VGPathDatatype aDatatype, VGfloat aScale, VGfloat aBias,
		VGint aSegmentCapacityHint, VGint aCoordCapacityHint, VGbitfield aCapabilities);
	static VGfloat vgGetf(VGParamType aType);
	static VGint vgGeti(VGParamType aType);
	static VGfloat vgGetParameterf(VGHandle aObject,  VGint aParamType);
	static VGint vgGetParameteri(VGHandle aObject,  VGint aParamType);
	static VGint vgGetParameterVectorSize(VGHandle aObject, VGint aParamType);
	static VGMaskLayer vgCreateMaskLayer(VGint aWidth, VGint aHeight);
	static VGuint vgGetColor(VGPaint aPaint);
	static VGImage vgCreateImage(VGImageFormat aFormat, VGint aWidth, VGint aHeight, VGbitfield aAllowedQuality);
	static VGImage vgChildImage(VGImage aParent, VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	static VGImage vgGetParent(VGImage aImage);
	static VGImage vgCreateEGLImageTargetKHR(VGeglImageKHR aImage);
	static VGFont vgCreateFont(VGint aGlyphCapacityHint);
	static VGHardwareQueryResult vgHardwareQuery(VGHardwareQueryType key, VGint setting);
	static void vgGetParameterfv(VGHandle aObject, VGint aParamType, VGint aCount, VGfloat * aValues);
	static void vgGetParameteriv(VGHandle aObject, VGint aParamType, VGint aCount, VGint * aValues);
	static VGint vgGetVectorSize(VGParamType type);
	static void vgGetfv(VGParamType aType, VGint aCount, const VGfloat * aValues);
	static void vgGetiv(VGParamType aType, VGint aCount, const VGint * aValues);
	static void vgGetMatrix(VGfloat* aM);
	static void vgGetImageSubData(VGImage aImage, void* aData, VGint aDataStride, VGImageFormat aDataFormat,
		VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	static void vgReadPixels(void* aData, VGint aDataStride, VGImageFormat aDataFormat, VGint aSx, VGint aSy,
		VGint aWidth, VGint aHeight);
	static void vgPathBounds(VGPath aPath, VGfloat* aMinX, VGfloat* aMinY, VGfloat* aWidth, VGfloat* aHeight);
	static void vgPathTransformedBounds(VGPath aPath, VGfloat* aMinX, VGfloat* aMinY, VGfloat* aWidth, VGfloat* aHeight);
	static void vgSetf(VGParamType aType, VGfloat aValue);
	static void vgSeti(VGParamType aType, VGint aVvalue);
	static void vgSetfv(VGParamType aType, VGint aCount, const VGfloat * aValues);
	static void vgSetiv(VGParamType aType, VGint aCount, const VGint * aValues);
	static void vgSetParameterf(VGHandle aObject, VGint aParamType, VGfloat aValue);
	static void vgSetParameteri(VGHandle aObject, VGint aParamType, VGint aValue);
	static void vgSetParameterfv(VGHandle aObject, VGint aParamType, VGint aCount, const VGfloat * aValues);
	static void vgSetParameteriv(VGHandle aObject, VGint aParamType, VGint aCount, const VGint * aValues);
	static void vgLoadIdentity();
	static void vgLoadMatrix(const VGfloat* aM);
	static void vgMultMatrix(const VGfloat* aM);
	static void vgTranslate(VGfloat aTx, VGfloat aTy);
	static void vgScale(VGfloat aSx, VGfloat aSy);
	static void vgShear(VGfloat aShx, VGfloat aShy);
	static void vgRotate(VGfloat aAngle);
	static void vgMask(VGHandle aMask, VGMaskOperation aOperation, VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	static void vgRenderToMask(VGPath aPath, VGbitfield aPaintModes, VGMaskOperation aOperation);
	static void vgDestroyMaskLayer(VGMaskLayer maskLayer);
	static void vgFillMaskLayer(VGMaskLayer aMaskLayer, VGint aX, VGint aY, VGint aWidth, VGint aHeight, VGfloat aValue);
	static void vgCopyMask(VGMaskLayer aMaskLayer, VGint aSx, VGint aSy, VGint aDx, VGint aDy, VGint aWidth, VGint aHeight);
	static void vgClear(VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	static void vgClearPath(VGPath aPath, VGbitfield aCapabilities);
	static void vgDestroyPath(VGPath aPath);
	static void vgRemovePathCapabilities(VGPath aPath, VGbitfield aCapabilities);
	static void vgAppendPath(VGPath aDstPath, VGPath aSrcPath);
	static void vgAppendPathData(VGPath aDstPath, VGint aNumSegments, const VGubyte* aPathSegments, const void* aPathData);
	static void vgModifyPathCoords(VGPath aDstPath, VGint aStartIndex, VGint aNumSegments, const void* aPathData);
	static void vgTransformPath(VGPath aDstPath, VGPath aSrcPath);
	static void vgPointAlongPath(VGPath aPath, VGint aStartSegment, VGint aNumSegments, VGfloat aDistance,
		VGfloat* aX, VGfloat* aY, VGfloat* aTangentX, VGfloat* aTangentY);
	static void vgDrawPath(VGPath aPath, VGbitfield aPaintModes);
	static void vgDestroyPaint(VGPaint aPaint);
	static void vgSetPaint(VGPaint aPaint, VGbitfield aPaintModes);
	static void vgSetColor(VGPaint aPaint, VGuint aRgba);
	static void vgPaintPattern(VGPaint aPaint, VGImage aPattern);
	static void vgDestroyImage(VGImage aImage);
	static void vgClearImage(VGImage aImage, VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	static void vgImageSubData(VGImage aImage, const void* aData, VGint aDataStride, VGImageFormat aDataFormat,
		VGint aX, VGint aY, VGint aWidth, VGint aHeight);
	static void vgCopyImage(VGImage aDst, VGint aDx, VGint aDy, VGImage aSrc, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight,
		VGboolean aDither);
	static void vgDrawImage(VGImage aImage);
	static void vgSetPixels(VGint aDx, VGint aDy, VGImage aSrc, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight);
	static void vgWritePixels(const void* data, VGint aDataStride, VGImageFormat aDataFormat, VGint aDx, VGint aDy,
		VGint aWidth, VGint aHeight);
	static void vgGetPixels(VGImage aDst, VGint aDx, VGint aDy, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight);
	static void vgCopyPixels(VGint aDx, VGint aDy, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight);
	static void vgDestroyFont(VGFont aFont);
	static void  vgSetGlyphToPath(VGFont aFont, VGuint aGlyphIndex, VGPath aPath, VGboolean aIsHinted, const VGfloat aGlyphOrigin [2],
		const VGfloat aEscapement[2]);
	static void  vgSetGlyphToImage(VGFont aFont, VGuint aGlyphIndex, VGImage aImage, const VGfloat aGlyphOrigin [2],
		const VGfloat aEscapement[2]);
	static void vgClearGlyph(VGFont aFont, VGuint aGlyphIndex);
	static void vgDrawGlyph(VGFont aFont, VGuint aGlyphIndex, VGbitfield aPaintModes, VGboolean aAllowAutoHinting);
	static void vgDrawGlyphs(VGFont aFont, VGint aGlyphCount, const VGuint* aGlyphIndices, const VGfloat* aAdjustmentsX,
		const VGfloat* aAdjustmentsY, VGbitfield aPaintModes, VGboolean aAllowAutoHinting);
	static void vgColorMatrix(VGImage aDst, VGImage aSrc, const VGfloat* aMatrix);
	static void vgConvolve(VGImage aDst, VGImage aSrc, VGint aKernelWidth, VGint aKernelHeight, VGint aShiftX, VGint aShiftY,
		const VGshort* aKernel, VGfloat aScale, VGfloat aBias, VGTilingMode aTilingMode);
	static void vgSeparableConvolve(VGImage aDst, VGImage aSrc, VGint aKernelWidth, VGint aKernelHeight, VGint aShiftX, VGint aShiftY,
		const VGshort* aKernelX, const VGshort* aKernelY, VGfloat aScale, VGfloat aBias, VGTilingMode aTilingMode);
	static void vgGaussianBlur(VGImage aDst, VGImage aSrc, VGfloat aStdDeviationX, VGfloat aStdDeviationY, VGTilingMode aTilingMode);
	static void vgLookup(VGImage aDst, VGImage aSrc, const VGubyte* aRedLUT, const VGubyte* aGreenLUT, const VGubyte* aBlueLUT,
		const VGubyte* aAlphaLUT, VGboolean aOutputLinear, VGboolean aOutputPremultiplied);
	static void vgLookupSingle(VGImage aDst, VGImage aSrc, const VGuint* aLookupTable, VGImageChannel aSourceChannel,
		VGboolean aOutputLinear, VGboolean aOutputPremultiplied);

private:
	// fns that directly talk to Host Open VG (no parameter checking)
	static void HostVgReadPixels(MVgContext& aVgContext, void* aPixmap, size_t aPixmapSize, VGint aHostDataStride,
			VGImageFormat aDataFormat, VGint aSx, VGint aSy, VGint aWidth, VGint aHeight);
	static void HostVgWritePixels(MVgContext& aVgContext, const void* aPixmap, size_t aPixmapSize, VGint aHostDataStride,
			VGImageFormat aDataFormat, VGint aDx, VGint aDy, VGint aWidth, VGint aHeight);

	static VGint HostGetVectorSize(MVgContext& aVgContext, VGParamType aType);

public: // ToDo find a more elegant way of calling this from XOpenVgState
	static VGint HostGeti(MVgContext& aVgContext, VGParamType type);
private:
	static inline VGfloat HostGetf(MVgContext& aVgContext, VGParamType type);
	};


// C api extensions
#ifdef __cplusplus
extern "C" {
#endif

VGImage vgCreateEGLImageTargetKHR(VGeglImageKHR image);

#ifdef __cplusplus
}
#endif


#include "openvg.inl"

#endif // VGSTATE_H
