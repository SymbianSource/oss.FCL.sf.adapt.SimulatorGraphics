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
// 

#ifndef OPENVGAPIWRAPPER_H
#define OPENVGAPIWRAPPER_H

#ifndef PSU_PLATFORMTYPES_H
#include "platformtypes.h"
#endif

#include "requestbuffer.h"
#include "apiwrapper.h"

class MGraphicsVHWCallback;
class APIWrapperStack;

class OpenVGAPIWrapper: public APIWrapper
{
public:
    OpenVGAPIWrapper( RemoteFunctionCallData& currentFunctionCallData, APIWrapperStack* stack,
	        void* result,
			MGraphicsVHWCallback* serviceIf, KhronosAPIWrapper* aAPIWrapper );

	int WriteReply();
	
	void SetProcessInformation( TUint32 aProcess, TUint32 aThread )
	{
	}

	void Cleanup( TUint32 aProcess, TUint32 aThread )
	{
	}

    int DispatchRequest( unsigned long aCode );
    int vgCreatePaint();
    int vgDestroyPaint();
    int vgSetPaint();
    int vgSetParameteri();
    int vgSetParameterf();
    int vgSetParameterfv();
    int vgCreatePath();
    int vgDrawPath();
    int vgDestroyPath();
    int vgAppendPathData();
    int vgGetError();
    int vgSeti();
    int vgSetf();
    int vgSetfv();
    int vgClear();
    int vgLoadIdentity();
    int vgRotate();
    int vgScale();
    int vgTranslate();
    int vgReadPixels();
    int vgFlush();
    int vgFinish();
    int vgSetiv();

    int vgGetf();
    int vgGeti();
    int vgGetVectorSize();
    int vgGetParameterf();
    int vgGetParameteri();
    int vgGetParameterVectorSize();
    int vgShear();
    int vgMask();
    int vgRenderToMask();
    int vgCreateMaskLayer();
    int vgDestroyMaskLayer();
    int vgFillMaskLayer();
    int vgCopyMask();
    int vgClearPath();
    int vgRemovePathCapabilities();
    int vgGetPathCapabilities();
    int vgAppendPath();
    int vgTransformPath();
    int vgInterpolatePath();
    int vgPathLength();
    int vgGetPaint();
    int vgSetColor();
    int vgGetColor();
    int vgPaintPattern();
    int vgCreateImage();
    int vgDestroyImage();
    int vgClearImage();
    int vgChildImage();
    int vgGetParent();
    int vgCopyImage();
    int vgDrawImage();
    int vgSetPixels();
    int vgGetPixels();
    int vgCopyPixels();
    int vgCreateFont();
    int vgDestroyFont();
    int vgClearGlyph();
    int vgDrawGlyph();
    int vgGaussianBlur();
    int vgHardwareQuery();
    int vgConvolve();
    int vgColorMatrix();
    int vgWritePixels();
    int vgImageSubData();
    int vgSetParameteriv();
    int vgLoadMatrix();
    int vgMultMatrix();
    int vgModifyPathCoords();
    int vgSetGlyphToPath();
    int vgSetGlyphToImage();
    int vgDrawGlyphs();
    int vgSeparableConvolve();
    int vgLookup();
    int vgLookupSingle();
    int vgGetMatrix();
    int vgGetfv();
    int vgGetiv();
    int vgGetParameterfv();
    int vgGetParameteriv();
    int vgGetImageSubData();
    int vgPointAlongPath();
    int vgPathBounds();
    int vgPathTransformedBounds();
    int vgePathCoordsSizeInBytes();

    int vguLine();
    int vguPolygon();
    int vguRect();
    int vguRoundRect();
    int vguEllipse();
    int vguArc();
    int vguComputeWarpQuadToSquare();
    int vguComputeWarpSquareToQuad();
    int vguComputeWarpQuadToQuad();
private:
	void doSync( int aParamIndex, VGImage aDst, VGImage aSrc = 0 );
private:
	int m_lastVgError;
	OpenVgRFC m_currentFunctionCall;
	typedef VGint (*vgePathCoordsSizeInBytesPtrType)(VGPath, VGint, VGint);
	//VGint (*vgePathCoordsSizeInBytesPtr)(VGPath, VGint, VGint);
	vgePathCoordsSizeInBytesPtrType vgePathCoordsSizeInBytesPtr;
	KhronosAPIWrapper* m_APIWrapper;
};
#endif
