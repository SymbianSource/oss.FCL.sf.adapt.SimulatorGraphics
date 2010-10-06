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
// EGL/EGLTYPES.H
// 
//

/* NOTE: If you change the signature of the library and it causes a build
         break, please see readme.txt on EGL root folder
*/

#ifndef __EGL_EGLTYPES_H__
#define __EGL_EGLTYPES_H__




/**
@publishedAll
@released
*/

/*
** egltypes.h is platform dependent. It defines:
**
**     - EGL types and resources
**     - Native types
**     - EGL and native handle values
**
** EGL types and resources are to be typedef'ed with appropriate platform
** dependent resource handle types. EGLint must be an integer of at least
** 32-bit.
**
** NativeDisplayType, NativeWindowType and NativePixmapType are to be
** replaced with corresponding types of the native window system in egl.h.
**
** EGL and native handle values must match their types.
**
*/

typedef struct
{
	void* buffer0;
	void* buffer1;
} PlatformSpecificSurface;

enum TDisplayMode
	{
	/** No display mode */
	ENone,
	/** Monochrome display mode (1 bpp) */
	EGray2,
	/** Four grayscales display mode (2 bpp) */
	EGray4,
	/** 16 grayscales display mode (4 bpp) */
	EGray16,
	/** 256 grayscales display mode (8 bpp) */
	EGray256,
	/** Low colour EGA 16 colour display mode (4 bpp) */
	EColor16,
	/** 256 colour display mode (8 bpp) */
	EColor256,
	/** 64,000 colour display mode (16 bpp) */
	EColor64K,
	/** True colour display mode (24 bpp) */
	EColor16M,
	/** (Not an actual display mode used for moving buffers containing bitmaps) */
	ERgb,
	/** 4096 colour display (12 bpp). */
	EColor4K,
	/** True colour display mode (32 bpp, but top byte is unused and unspecified) */
	EColor16MU,
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	EColor16MA,
	/** Pre-multiplied Alpha display mode (24bpp color multiplied with the alpha channel value, plus 8bpp alpha) */
	EColor16MAP,
	//Any new display mode should be insterted here!
	//There might be up to 255 display modes, so value of the last
	//inserted EColorXXX enum item should be less than 256 -
	//BC reasons!
	EColorLast
	};

struct SymbianWindow
{
	int id;
	int width;
	int height;
	int horizontalPitch;
    int verticalPitch;
};

struct SymbianPixmap
{
	enum TDisplayMode format;
	unsigned long* data;
	int width;
	int height;
	int stride;
	int type;
};

#endif // __EGL_EGLTYPES_H__
