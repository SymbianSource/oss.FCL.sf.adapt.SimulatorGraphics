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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN                       // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <vg/openvg.h>
#include <EGL/egl.h>
#include "KhronosAPIWrapper.h"
#include "serializedfunctioncall.h"
#include "remotefunctioncall.h"
#include "openvgrfc.h"
#include "graphicsvhwcallback.h"        //callback interface Wrapper=>Virtual HW
#include "serialisedapiuids.h"
#include "openvgapiwrapper.h"
#include "khronosapiwrapperdefs.h"
#include "driverapiwrapper.h" //For calling the sync functions

int OpenVGAPIWrapper::vgePathCoordsSizeInBytes()
{
    VGPath dstPath;
    m_currentFunctionCall.GetParamValue( dstPath, 0 );
    VGint startIndex;
    m_currentFunctionCall.GetParamValue( startIndex, 1 );
    VGint numSegments;
    m_currentFunctionCall.GetParamValue( numSegments, 2 );
    if ( !vgePathCoordsSizeInBytesPtr )
        {
        vgePathCoordsSizeInBytesPtr = (vgePathCoordsSizeInBytesPtrType)::eglGetProcAddress("vgePathCoordsSizeInBytes");
        }
	if ( vgePathCoordsSizeInBytesPtr )
	{
		VGint ret = vgePathCoordsSizeInBytesPtr( dstPath, startIndex, numSegments );
		m_currentFunctionCall.SetReturnValue( (TUint32)ret );
	}
	else
	{
		m_currentFunctionCall.SetReturnValue( (TUint32)(-1) );
	}

    return WriteReply();
}

int OpenVGAPIWrapper::vgCreatePaint()
{
    VGPaint paint = ::vgCreatePaint();
    m_currentFunctionCall.SetReturnValue((TUint32)paint);
    return WriteReply();
}


int OpenVGAPIWrapper::vgDestroyPaint()
{
    VGPaint paint;
    m_currentFunctionCall.GetParamValue( paint, 0 );
    ::vgDestroyPaint( paint );
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetPaint()
{
    VGPaint paint;
    m_currentFunctionCall.GetParamValue( paint, 0 );
    VGbitfield paintModes;
    m_currentFunctionCall.GetParamValue( paintModes, 1 );
    ::vgSetPaint(paint, paintModes);
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetParameteri()
{
    VGHandle object;
    m_currentFunctionCall.GetParamValue( object, 0 );
    VGint paramType;
    m_currentFunctionCall.GetParamValue( paramType, 1 );
    VGint value;
    m_currentFunctionCall.GetParamValue( value, 2 );
    ::vgSetParameteri(object, paramType, value);
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetParameterf()
{
    VGHandle object;
    m_currentFunctionCall.GetParamValue( object, 0 );
    VGint paramType;
    m_currentFunctionCall.GetParamValue( paramType, 1 );
    VGfloat value;
    m_currentFunctionCall.GetParamValue( value, 2 );
    ::vgSetParameterf(object, paramType, value);
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetParameterfv()
{
    VGHandle object;
    m_currentFunctionCall.GetParamValue( object, 0 );
    VGint paramType;
    m_currentFunctionCall.GetParamValue( paramType, 1 );
    VGint count;
    m_currentFunctionCall.GetParamValue( count, 2 );
    VGfloat * values;
    TInt size;
    m_currentFunctionCall.GetVectorData( values, size, 3 );

    ::vgSetParameterfv(object, paramType, count, values);
    return WriteReply();
}

int OpenVGAPIWrapper::vgCreatePath()
{
    VGint pathFormat;
    m_currentFunctionCall.GetParamValue( pathFormat, 0 );

    VGPathDatatype datatype;
    m_currentFunctionCall.GetParamValue( datatype, 1 );

    VGfloat scale;
    m_currentFunctionCall.GetParamValue( scale, 2 );

    VGfloat bias;
    m_currentFunctionCall.GetParamValue( bias, 3 );

    VGint segmentCapacityHint;
    m_currentFunctionCall.GetParamValue( segmentCapacityHint, 4 );

    VGint coordCapacityHint;
    m_currentFunctionCall.GetParamValue( coordCapacityHint, 5 );

    VGbitfield capabilities;
    m_currentFunctionCall.GetParamValue( capabilities, 6 );

    VGPath path = ::vgCreatePath( pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities);
    m_currentFunctionCall.SetReturnValue((TUint32)path);
    return WriteReply();

}

int OpenVGAPIWrapper::vgDrawPath()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );

    VGbitfield paintModes;
    m_currentFunctionCall.GetParamValue( paintModes, 1 );

    ::vgDrawPath( path, paintModes );
    return WriteReply();
}

int OpenVGAPIWrapper::vgDestroyPath()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );

    ::vgDestroyPath(path);
    return WriteReply();
}


int OpenVGAPIWrapper::vgAppendPathData()
{
    VGPath dstPath;
    m_currentFunctionCall.GetParamValue( dstPath, 0 );
    VGint numSegments;
    m_currentFunctionCall.GetParamValue( numSegments, 1 );
    VGubyte * pathSegments;
    TInt size;
    m_currentFunctionCall.GetVectorData( pathSegments, size, 2 );

    void * pathData;
    m_currentFunctionCall.GetVectorData( pathData, size, 3 );

    ::vgAppendPathData( dstPath, numSegments, pathSegments, pathData );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetError()
{
    VGErrorCode error = ::vgGetError();
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}


int OpenVGAPIWrapper::vgSeti()
{
    VGParamType type;
    m_currentFunctionCall.GetParamValue( type, 0 );
    VGint value;
    m_currentFunctionCall.GetParamValue( value, 1 );
    ::vgSeti( type, value);
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetf()
{
    VGParamType type;
    m_currentFunctionCall.GetParamValue( type, 0 );
    VGfloat value;
    m_currentFunctionCall.GetParamValue( value, 1 );
    ::vgSetf( type, value );
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetfv()
{
    VGParamType type;
    m_currentFunctionCall.GetParamValue( type, 0 );
    VGint count;
    m_currentFunctionCall.GetParamValue( count, 1 );
    VGfloat * values;
    TInt size;
    m_currentFunctionCall.GetVectorData( values, size, 2 );
    ::vgSetfv( type, count, values);
    return WriteReply();
}


int OpenVGAPIWrapper::vgClear()
{
    VGint x;
    m_currentFunctionCall.GetParamValue( x, 0 );
    VGint y;
    m_currentFunctionCall.GetParamValue( y, 1 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 2 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 3 );
    ::vgClear( x, y, width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgLoadIdentity()
{
    ::vgLoadIdentity();
    return WriteReply();
}


int OpenVGAPIWrapper::vgRotate()
{
    VGfloat angle;
    m_currentFunctionCall.GetParamValue( angle, 0 );
    ::vgRotate( angle );
    return WriteReply();
}


int OpenVGAPIWrapper::vgScale()
{
    VGfloat sx;
    m_currentFunctionCall.GetParamValue( sx, 0 );
    VGfloat sy;
    m_currentFunctionCall.GetParamValue( sy, 1 );
    ::vgScale( sx, sy );
    return WriteReply();
}


int OpenVGAPIWrapper::vgTranslate()
{
    VGfloat tx;
    m_currentFunctionCall.GetParamValue( tx, 0 );
    VGfloat ty;
    m_currentFunctionCall.GetParamValue( ty, 1 );
    ::vgTranslate( tx, ty );
    return WriteReply();
}


int OpenVGAPIWrapper::vgReadPixels()
{
    VGint dataStride;
    m_currentFunctionCall.GetParamValue( dataStride, 1 );
    VGImageFormat dataFormat;
    m_currentFunctionCall.GetParamValue( dataFormat, 2 );
    VGint sx;
    m_currentFunctionCall.GetParamValue( sx, 3 );
    VGint sy;
    m_currentFunctionCall.GetParamValue( sy, 4 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 5 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 6 );
	VGint pixmapSize;
	m_currentFunctionCall.GetParamValue(pixmapSize, 7);

    void *dataBuffer = iStack->AllocFromStack(pixmapSize, 4);

    ::vgReadPixels( dataBuffer, dataStride, dataFormat, sx, sy, width, height );
    m_currentFunctionCall.SetVectorData( dataBuffer, pixmapSize, 0 );
    int ret = WriteReply();
    iStack->ClearStack();
    dataBuffer = NULL;
    return ret;
}


int OpenVGAPIWrapper::vgFlush()
{
    ::vgFlush();
    return WriteReply();
}


int OpenVGAPIWrapper::vgFinish()
{
    ::vgFinish();
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetiv()
{
    VGParamType type;
    m_currentFunctionCall.GetParamValue( type, 0 );
    VGint count;
    m_currentFunctionCall.GetParamValue( count, 1 );
    VGint * values;
    TInt size;
    m_currentFunctionCall.GetVectorData( values, size, 2 );
    ::vgSetiv( type, count, values);
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetf()
{
    VGParamType type;
    m_currentFunctionCall.GetParamValue( type, 0 );
    VGfloat ret = ::vgGetf( type );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGeti()
{
    VGParamType type;
    m_currentFunctionCall.GetParamValue( type, 0 );
    VGint ret = ::vgGeti( type );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetVectorSize()
{
    VGParamType type;
    m_currentFunctionCall.GetParamValue( type, 0 );
    VGint ret = ::vgGetVectorSize( type );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetParameterf()
{
    VGHandle object;
    m_currentFunctionCall.GetParamValue( object, 0 );
    VGint paramType;
    m_currentFunctionCall.GetParamValue( paramType, 1 );
    VGfloat ret = ::vgGetParameterf( object, paramType );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetParameteri()
{
    VGHandle object;
    m_currentFunctionCall.GetParamValue( object, 0 );
    VGint paramType;
    m_currentFunctionCall.GetParamValue( paramType, 1 );
    VGint ret = ::vgGetParameteri( object, paramType );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetParameterVectorSize()
{
    VGHandle object;
    m_currentFunctionCall.GetParamValue( object, 0 );
    VGint paramType;
    m_currentFunctionCall.GetParamValue( paramType, 1 );
    VGint ret = ::vgGetParameterVectorSize( object, paramType );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgShear()
{
    VGfloat shx;
    m_currentFunctionCall.GetParamValue( shx, 0 );
    VGfloat shy;
    m_currentFunctionCall.GetParamValue( shy, 1 );
    ::vgShear( shx, shy );
    return WriteReply();
}


int OpenVGAPIWrapper::vgMask()
{
    VGHandle mask;
    m_currentFunctionCall.GetParamValue( mask, 0 );
    VGMaskOperation operation;
    m_currentFunctionCall.GetParamValue( operation, 1 );
    VGint x;
    m_currentFunctionCall.GetParamValue( x, 2 );
    VGint y;
    m_currentFunctionCall.GetParamValue( y, 3 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 4 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 5 );

    ::vgMask( mask, operation, x, y, width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgRenderToMask()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );
    VGbitfield paintModes;
    m_currentFunctionCall.GetParamValue( paintModes, 1 );
    VGMaskOperation operation;
    m_currentFunctionCall.GetParamValue( operation, 2 );

    ::vgRenderToMask( path, paintModes, operation);
    return WriteReply();
}


int OpenVGAPIWrapper::vgCreateMaskLayer()
{
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 0 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 1 );
    ::vgCreateMaskLayer( width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgDestroyMaskLayer()
{
    VGMaskLayer maskLayer;
    m_currentFunctionCall.GetParamValue( maskLayer, 0 );
    ::vgDestroyMaskLayer( maskLayer );
    return WriteReply();
}


int OpenVGAPIWrapper::vgFillMaskLayer()
{
    VGMaskLayer maskLayer;
    m_currentFunctionCall.GetParamValue( maskLayer, 0 );
    VGint x;
    m_currentFunctionCall.GetParamValue( x, 1 );
    VGint y;
    m_currentFunctionCall.GetParamValue( y, 2 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 3 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 4 );
    VGfloat value;
    m_currentFunctionCall.GetParamValue( value, 5 );

    ::vgFillMaskLayer( maskLayer, x, y,
        width, height, value );
    return WriteReply();
}


int OpenVGAPIWrapper::vgCopyMask()
{
    VGMaskLayer maskLayer;
    m_currentFunctionCall.GetParamValue( maskLayer, 0 );
    VGint sx;
    m_currentFunctionCall.GetParamValue( sx, 1 );
    VGint sy;
    m_currentFunctionCall.GetParamValue( sy, 2 );
    VGint dx;
    m_currentFunctionCall.GetParamValue( dx, 3 );
    VGint dy;
    m_currentFunctionCall.GetParamValue( dy, 4 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 5 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 6 );

    ::vgCopyMask( maskLayer, sx, sy, dx, dy,
        width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgClearPath()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );
    VGbitfield capabilities;
    m_currentFunctionCall.GetParamValue( capabilities, 1 );

    ::vgClearPath( path, capabilities );
    return WriteReply();
}


int OpenVGAPIWrapper::vgRemovePathCapabilities()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );
    VGbitfield capabilities;
    m_currentFunctionCall.GetParamValue( capabilities, 1 );
    ::vgRemovePathCapabilities( path, capabilities );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetPathCapabilities()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );
    VGbitfield ret = ::vgGetPathCapabilities( path );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgAppendPath()
{
    VGPath dstPath;
    m_currentFunctionCall.GetParamValue( dstPath, 0 );
    VGPath srcPath;
    m_currentFunctionCall.GetParamValue( srcPath, 1 );
    ::vgAppendPath( dstPath, srcPath );
    return WriteReply();
}


int OpenVGAPIWrapper::vgTransformPath()
{
    VGPath dstPath;
    m_currentFunctionCall.GetParamValue( dstPath, 0 );
    VGPath srcPath;
    m_currentFunctionCall.GetParamValue( srcPath, 1 );
    ::vgTransformPath( dstPath, srcPath );
    return WriteReply();
}


int OpenVGAPIWrapper::vgInterpolatePath()
{
    VGPath dstPath;
    m_currentFunctionCall.GetParamValue( dstPath, 0 );
    VGPath startPath;
    m_currentFunctionCall.GetParamValue( startPath, 1 );
    VGPath endPath;
    m_currentFunctionCall.GetParamValue( endPath, 2 );
    VGfloat amount;
    m_currentFunctionCall.GetParamValue( amount, 3 );

    ::vgInterpolatePath( dstPath, startPath, endPath, amount );
    return WriteReply();
}


int OpenVGAPIWrapper::vgPathLength()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );
    VGint startSegment;
    m_currentFunctionCall.GetParamValue( startSegment, 1 );
    VGint numSegments;
    m_currentFunctionCall.GetParamValue( numSegments, 2 );

    VGfloat ret = ::vgPathLength( path, startSegment, numSegments );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetPaint()
{
    VGPaintMode paintMode;
    m_currentFunctionCall.GetParamValue( paintMode, 0 );
    VGPaint ret = ::vgGetPaint( paintMode );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetColor()
{
    VGPaint paint;
    m_currentFunctionCall.GetParamValue( paint, 0 );
    VGuint rgba;
    m_currentFunctionCall.GetParamValue( rgba, 1 );

    ::vgSetColor( paint, rgba );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetColor()
{
    VGPaint paint;
    m_currentFunctionCall.GetParamValue( paint, 0 );
    VGuint ret = ::vgGetColor( paint );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgPaintPattern()
{
    VGPaint paint;
    m_currentFunctionCall.GetParamValue( paint, 0 );
    VGImage pattern;
    m_currentFunctionCall.GetParamValue( pattern, 1 );

    ::vgPaintPattern( paint, pattern );
    return WriteReply();
}


int OpenVGAPIWrapper::vgCreateImage()
{
    VGImageFormat format;
    m_currentFunctionCall.GetParamValue( format, 0 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 1 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 2 );
    VGbitfield allowedQuality;
    m_currentFunctionCall.GetParamValue( allowedQuality, 3 );

    VGImage ret = ::vgCreateImage( format, width, height, allowedQuality );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgDestroyImage()
{
    VGImage image;
    m_currentFunctionCall.GetParamValue( image, 0 );
    ::vgDestroyImage( image );
    return WriteReply();
}


int OpenVGAPIWrapper::vgClearImage()
{
    VGImage image;
    m_currentFunctionCall.GetParamValue( image, 0 );
    VGint x;
    m_currentFunctionCall.GetParamValue( x, 1 );
    VGint y;
    m_currentFunctionCall.GetParamValue( y, 2 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 3 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 4 );

	doSync( 5, image );

    ::vgClearImage( image, x, y, width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgChildImage()
{
    VGImage parent;
    m_currentFunctionCall.GetParamValue( parent, 0 );
    VGint x;
    m_currentFunctionCall.GetParamValue( x, 1 );
    VGint y;
    m_currentFunctionCall.GetParamValue( y, 2 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 3 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 4 );
    VGImage ret = ::vgChildImage( parent, x, y, width, height );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetParent()
{
    VGImage image;
    m_currentFunctionCall.GetParamValue( image, 0 );
    VGImage ret = ::vgGetParent( image );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgCopyImage()
{
    VGImage dst;
    m_currentFunctionCall.GetParamValue( dst, 0 );
    VGint dx;
    m_currentFunctionCall.GetParamValue( dx, 1 );
    VGint dy;
    m_currentFunctionCall.GetParamValue( dy, 2 );
    VGImage src;
    m_currentFunctionCall.GetParamValue( src, 3 );
    VGint sx;
    m_currentFunctionCall.GetParamValue( sx, 4 );
    VGint sy;
    m_currentFunctionCall.GetParamValue( sy, 5 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 6 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 7 );
    VGboolean dither;
    m_currentFunctionCall.GetParamValue( dither, 8 );

	doSync( 9, dst, src );

    ::vgCopyImage( dst, dx, dy, src, sx, sy, width, height, dither );
    return WriteReply();
}


int OpenVGAPIWrapper::vgDrawImage()
{
    VGImage image;
    m_currentFunctionCall.GetParamValue( image, 0 );


    ::vgDrawImage( image );
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetPixels()
{
    VGint dx;
    m_currentFunctionCall.GetParamValue( dx, 0 );
    VGint dy;
    m_currentFunctionCall.GetParamValue( dy, 1 );
    VGImage src;
    m_currentFunctionCall.GetParamValue( src, 2 );
    VGint sx;
    m_currentFunctionCall.GetParamValue( sx, 3 );
    VGint sy;
    m_currentFunctionCall.GetParamValue( sy, 4 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 5 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 6 );

	doSync( 7, src );

    ::vgSetPixels( dx, dy, src, sx, sy, width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetPixels()
{
    VGImage dst;
    m_currentFunctionCall.GetParamValue( dst, 0 );
    VGint dx;
    m_currentFunctionCall.GetParamValue( dx, 1 );
    VGint dy;
    m_currentFunctionCall.GetParamValue( dy, 2 );
    VGint sx;
    m_currentFunctionCall.GetParamValue( sx, 3 );
    VGint sy;
    m_currentFunctionCall.GetParamValue( sy, 4 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 5 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 6 );

	doSync( 7, dst );

    ::vgGetPixels( dst, dx, dy, sx, sy, width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgCopyPixels()
{
    VGint dx;
    m_currentFunctionCall.GetParamValue( dx, 0 );
    VGint dy;
    m_currentFunctionCall.GetParamValue( dy, 1 );
    VGint sx;
    m_currentFunctionCall.GetParamValue( sx, 2 );
    VGint sy;
    m_currentFunctionCall.GetParamValue( sy, 3 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 4 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 5 );

    ::vgCopyPixels( dx, dy, sx, sy, width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgCreateFont()
{
    VGint glyphCapacityHint;
    m_currentFunctionCall.GetParamValue( glyphCapacityHint, 0 );
    VGFont ret = ::vgCreateFont( glyphCapacityHint );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgDestroyFont()
{
    VGFont font;
    m_currentFunctionCall.GetParamValue( font, 0 );
    ::vgDestroyFont( font );
    return WriteReply();
}


int OpenVGAPIWrapper::vgClearGlyph()
{
    VGFont font;
    m_currentFunctionCall.GetParamValue( font, 0 );
    VGuint glyphIndex;
    m_currentFunctionCall.GetParamValue( glyphIndex, 1 );
    ::vgClearGlyph( font, glyphIndex );
    return WriteReply();
}


int OpenVGAPIWrapper::vgDrawGlyph()
{
    VGFont font;
    m_currentFunctionCall.GetParamValue( font, 0 );

    VGuint glyphIndex;
    m_currentFunctionCall.GetParamValue( glyphIndex, 1 );

    VGbitfield paintModes;
    m_currentFunctionCall.GetParamValue( paintModes, 2 );

    VGboolean allowAutoHinting;
    m_currentFunctionCall.GetParamValue( allowAutoHinting, 3 );

    ::vgDrawGlyph( font, glyphIndex, paintModes, allowAutoHinting );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGaussianBlur()
{
    VGImage dst;
    m_currentFunctionCall.GetParamValue( dst, 0 );
    VGImage src;
    m_currentFunctionCall.GetParamValue( src, 1 );
    VGfloat stdDeviationX;
    m_currentFunctionCall.GetParamValue( stdDeviationX, 2 );
    VGfloat stdDeviationY;
    m_currentFunctionCall.GetParamValue( stdDeviationY, 3 );
    VGTilingMode tilingMode;
    m_currentFunctionCall.GetParamValue( tilingMode, 4 );

	doSync( 5, dst, src );

    ::vgGaussianBlur( dst, src, stdDeviationX, stdDeviationY, tilingMode );
    return WriteReply();
}


int OpenVGAPIWrapper::vgHardwareQuery()
{
    VGHardwareQueryType key;
    m_currentFunctionCall.GetParamValue( key, 0 );
    VGint setting;
    m_currentFunctionCall.GetParamValue( setting, 1 );
    VGHardwareQueryResult ret =::vgHardwareQuery( key, setting );
    m_currentFunctionCall.SetReturnValue( (TUint32)ret );
    return WriteReply();
}


int OpenVGAPIWrapper::vgWritePixels()
{
    void* data;
    TInt size;
    m_currentFunctionCall.GetVectorData( data, size, 0 );
    VGint dataStride;
    m_currentFunctionCall.GetParamValue( dataStride, 1 );
    VGImageFormat dataFormat;
    m_currentFunctionCall.GetParamValue( dataFormat, 2 );
    VGint dx;
    m_currentFunctionCall.GetParamValue( dx, 3 );
    VGint dy;
    m_currentFunctionCall.GetParamValue( dy, 4 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 5 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 6 );

    ::vgWritePixels( data, dataStride, dataFormat, dx, dy, width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgImageSubData()
{
    VGImage image;
    m_currentFunctionCall.GetParamValue( image, 0 );
    void* data;
    TInt size;
    m_currentFunctionCall.GetVectorData( data, size, 1 );
    VGint dataStride;
    m_currentFunctionCall.GetParamValue( dataStride, 2 );
    VGImageFormat dataFormat;
    m_currentFunctionCall.GetParamValue( dataFormat, 3 );
    VGint x;
    m_currentFunctionCall.GetParamValue( x, 4 );
    VGint y;
    m_currentFunctionCall.GetParamValue( y, 5 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 6 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 7 );

	doSync( 8, image );

    ::vgImageSubData( image, data, dataStride, dataFormat, x, y, width, height );
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetParameteriv()
{
    VGHandle object;
    m_currentFunctionCall.GetParamValue( object, 0 );
    VGint paramType;
    m_currentFunctionCall.GetParamValue( paramType, 1 );
    VGint count;
    m_currentFunctionCall.GetParamValue( count, 2 );
    VGint * values;
    TInt size;
    m_currentFunctionCall.GetVectorData( values, size, 3 );

    ::vgSetParameteriv( object, paramType, count, values );
    return WriteReply();
}


int OpenVGAPIWrapper::vgLoadMatrix()
{
    VGfloat * m;
    TInt size;
    m_currentFunctionCall.GetVectorData( m, size, 0 );
    ::vgLoadMatrix( m );
    return WriteReply();
}


int OpenVGAPIWrapper::vgMultMatrix()
{
    VGfloat * m;
    TInt size;
    m_currentFunctionCall.GetVectorData( m, size, 0 );
    ::vgMultMatrix( m );
    return WriteReply();
}


int OpenVGAPIWrapper::vgModifyPathCoords()
{
    VGPath dstPath;
    m_currentFunctionCall.GetParamValue( dstPath, 0 );
    VGint startIndex;
    m_currentFunctionCall.GetParamValue( startIndex, 1 );
    VGint numSegments;
    m_currentFunctionCall.GetParamValue( numSegments, 2 );
    void * pathData;
    TInt size;
    m_currentFunctionCall.GetVectorData( pathData, size, 3 );

    ::vgModifyPathCoords( dstPath, startIndex, numSegments, pathData);
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetGlyphToPath()
{
    VGFont font;
    m_currentFunctionCall.GetParamValue( font, 0 );
    VGuint glyphIndex;
    m_currentFunctionCall.GetParamValue( glyphIndex, 1 );
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 2 );
    VGboolean isHinted;
    m_currentFunctionCall.GetParamValue( isHinted, 3 );
    VGfloat* glyphOrigin;
    TInt size;
    m_currentFunctionCall.GetVectorData( glyphOrigin, size, 4 );
    VGfloat* escapement;
    m_currentFunctionCall.GetVectorData( escapement, size, 5 );

    ::vgSetGlyphToPath( font, glyphIndex, path, isHinted, glyphOrigin, escapement );
    return WriteReply();
}


int OpenVGAPIWrapper::vgSetGlyphToImage()
{
    VGFont font;
    m_currentFunctionCall.GetParamValue( font, 0 );
    VGuint glyphIndex;
    m_currentFunctionCall.GetParamValue( glyphIndex, 1 );
    VGImage image;
    m_currentFunctionCall.GetParamValue( image, 2 );
    VGfloat* glyphOrigin;
    TInt size;
    m_currentFunctionCall.GetVectorData( glyphOrigin, size, 3 );
    VGfloat* escapement;
    m_currentFunctionCall.GetVectorData( escapement, size, 4 );

    ::vgSetGlyphToImage( font, glyphIndex, image, glyphOrigin, escapement );
    return WriteReply();
}


int OpenVGAPIWrapper::vgDrawGlyphs()
{
    VGFont font;
    m_currentFunctionCall.GetParamValue( font, 0 );
    VGint glyphCount;
    m_currentFunctionCall.GetParamValue( glyphCount, 1 );
    VGuint * glyphIndices;
    TInt size;
    m_currentFunctionCall.GetVectorData( glyphIndices, size, 2 );
    VGfloat * adjustments_x;
    m_currentFunctionCall.GetVectorData( adjustments_x, size, 3 );
    VGfloat * adjustments_y;
    m_currentFunctionCall.GetVectorData( adjustments_y, size, 4 );
    VGbitfield paintModes;
    m_currentFunctionCall.GetParamValue( paintModes, 5 );
    VGboolean allowAutoHinting;
    m_currentFunctionCall.GetParamValue( allowAutoHinting, 6 );

    ::vgDrawGlyphs( font, glyphCount, glyphIndices, adjustments_x, adjustments_y, paintModes, allowAutoHinting );
    return WriteReply();
}


int OpenVGAPIWrapper::vgColorMatrix()
{
    VGImage dst;
    m_currentFunctionCall.GetParamValue( dst, 0 );
    VGImage src;
    m_currentFunctionCall.GetParamValue( src, 1 );
    VGfloat * matrix;
    TInt size;
    m_currentFunctionCall.GetVectorData( matrix, size, 2 );

	doSync( 3, dst, src );

    ::vgColorMatrix( dst, src, matrix );
    return WriteReply();
}


int OpenVGAPIWrapper::vgConvolve()
{
    VGImage dst;
    m_currentFunctionCall.GetParamValue( dst, 0 );
    VGImage src;
    m_currentFunctionCall.GetParamValue( src, 1 );
    VGint kernelWidth;
    m_currentFunctionCall.GetParamValue( kernelWidth, 2 );
    VGint kernelHeight;
    m_currentFunctionCall.GetParamValue( kernelHeight, 3 );
    VGint shiftX;
    m_currentFunctionCall.GetParamValue( shiftX, 4 );
    VGint shiftY;
    m_currentFunctionCall.GetParamValue( shiftY, 5 );
    VGshort * kernel;
    TInt size;
    m_currentFunctionCall.GetVectorData( kernel, size, 6 );
    VGfloat scale;
    m_currentFunctionCall.GetParamValue( scale, 7 );
    VGfloat bias;
    m_currentFunctionCall.GetParamValue( bias, 8 );
    VGTilingMode tilingMode;
    m_currentFunctionCall.GetParamValue( tilingMode, 9 );

	doSync( 10, dst, src );

    ::vgConvolve( dst, src, kernelWidth, kernelHeight, shiftX, shiftY, kernel, scale, bias, tilingMode );
    return WriteReply();
}


int OpenVGAPIWrapper::vgSeparableConvolve()
{
    VGImage dst;
    m_currentFunctionCall.GetParamValue( dst, 0 );
    VGImage src;
    m_currentFunctionCall.GetParamValue( src, 1 );
    VGint kernelWidth;
    m_currentFunctionCall.GetParamValue( kernelWidth, 2 );
    VGint kernelHeight;
    m_currentFunctionCall.GetParamValue( kernelHeight, 3 );
    VGint shiftX;
    m_currentFunctionCall.GetParamValue( shiftX, 4 );
    VGint shiftY;
    m_currentFunctionCall.GetParamValue( shiftY, 5 );
    VGshort * kernelX;
    TInt size;
    m_currentFunctionCall.GetVectorData( kernelX, size, 6 );
    VGshort * kernelY;
    m_currentFunctionCall.GetVectorData( kernelY, size, 7 );
    VGfloat scale;
    m_currentFunctionCall.GetParamValue( scale, 8 );
    VGfloat bias;
    m_currentFunctionCall.GetParamValue( bias, 9 );
    VGTilingMode tilingMode;
    m_currentFunctionCall.GetParamValue( tilingMode, 10 );

	doSync( 11, dst, src );

    ::vgSeparableConvolve( dst, src, kernelWidth, kernelHeight, shiftX, shiftY, kernelX, kernelY, scale, bias, tilingMode );
    return WriteReply();
}


int OpenVGAPIWrapper::vgLookup()
{
    VGImage dst;
    m_currentFunctionCall.GetParamValue( dst, 0 );
    VGImage src;
    m_currentFunctionCall.GetParamValue( src, 1 );
    VGubyte * redLUT;
    TInt size;
    m_currentFunctionCall.GetVectorData( redLUT, size, 2 );
    VGubyte * greenLUT;
    m_currentFunctionCall.GetVectorData( greenLUT, size, 3 );
    VGubyte * blueLUT;
    m_currentFunctionCall.GetVectorData( blueLUT, size, 4 );
    VGubyte * alphaLUT;
    m_currentFunctionCall.GetVectorData( alphaLUT, size, 5 );
    VGboolean outputLinear;
    m_currentFunctionCall.GetParamValue( outputLinear, 6 );
    VGboolean outputPremultiplied;
    m_currentFunctionCall.GetParamValue( outputPremultiplied, 7 );

	doSync( 8, dst, src );

    ::vgLookup( dst, src, redLUT, greenLUT, blueLUT, alphaLUT, outputLinear, outputPremultiplied );
    return WriteReply();
}


int OpenVGAPIWrapper::vgLookupSingle()
{
    VGImage dst;
    m_currentFunctionCall.GetParamValue( dst, 0 );
    VGImage src;
    m_currentFunctionCall.GetParamValue( src, 1 );
    VGuint * lookupTable;
    TInt size;
    m_currentFunctionCall.GetVectorData( lookupTable, size, 2 );
    VGImageChannel sourceChannel;
    m_currentFunctionCall.GetParamValue( sourceChannel, 3 );
    VGboolean outputLinear;
    m_currentFunctionCall.GetParamValue( outputLinear, 4 );
    VGboolean outputPremultiplied;
    m_currentFunctionCall.GetParamValue( outputPremultiplied, 5 );

	doSync( 6, dst, src );

    ::vgLookupSingle( dst, src, lookupTable, sourceChannel, outputLinear, outputPremultiplied );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetMatrix()
{
    VGfloat m[9];
    ::vgGetMatrix( m );
    m_currentFunctionCall.SetVectorData( m, 9, 0 );
    return WriteReply();
}


int OpenVGAPIWrapper::vgGetfv()
{
    VGParamType type;
    m_currentFunctionCall.GetParamValue( type, 0 );
    VGint count;
    m_currentFunctionCall.GetParamValue( count, 1 );
    VGfloat *values = (VGfloat*)iStack->AllocFromStack( count*m_currentFunctionCall.GetTypeSize( OpenVgRFC::EVGfloat),
        m_currentFunctionCall.GetTypeAlignment( OpenVgRFC::EVGfloat) );
    ::vgGetfv( type, count, values);
    m_currentFunctionCall.SetVectorData( values, count, 2 );
    int ret = WriteReply();
    iStack->ClearStack();
    values = NULL;
    return ret;
}


int OpenVGAPIWrapper::vgGetiv()
{
    VGParamType type;
    m_currentFunctionCall.GetParamValue( type, 0 );
    VGint count;
    m_currentFunctionCall.GetParamValue( count, 1 );
    VGint* values = (VGint*)iStack->AllocFromStack( count*m_currentFunctionCall.GetTypeSize( OpenVgRFC::EVGint),
        m_currentFunctionCall.GetTypeAlignment( OpenVgRFC::EVGint) );
    ::vgGetiv( type, count, values);
    m_currentFunctionCall.SetVectorData( values, count, 2 );
    int ret = WriteReply();
    iStack->ClearStack();
    values = NULL;
    return ret;
}


int OpenVGAPIWrapper::vgGetParameterfv()
{
    VGHandle object;
    m_currentFunctionCall.GetParamValue( object, 0 );
    VGint paramType;
    m_currentFunctionCall.GetParamValue( paramType, 1 );
    VGint count;
    m_currentFunctionCall.GetParamValue( count, 2 );
    VGfloat *values = (VGfloat*)iStack->AllocFromStack( count*m_currentFunctionCall.GetTypeSize( OpenVgRFC::EVGfloat),
        m_currentFunctionCall.GetTypeAlignment( OpenVgRFC::EVGfloat) );

    ::vgGetParameterfv( object, paramType, count, values );
    m_currentFunctionCall.SetVectorData( values, count, 3 );
    int ret = WriteReply();
    return ret;
}


int OpenVGAPIWrapper::vgGetParameteriv()
{
    VGHandle object;
    m_currentFunctionCall.GetParamValue( object, 0 );
    VGint paramType;
    m_currentFunctionCall.GetParamValue( paramType, 1 );
    VGint count;
    m_currentFunctionCall.GetParamValue( count, 2 );
    VGint* values = (VGint*)iStack->AllocFromStack( count*m_currentFunctionCall.GetTypeSize( OpenVgRFC::EVGint),
        m_currentFunctionCall.GetTypeAlignment( OpenVgRFC::EVGint) );

    ::vgGetParameteriv( object, paramType, count, values );
    m_currentFunctionCall.SetVectorData( values, count, 3 );
    int ret = WriteReply();
    iStack->ClearStack();
    values = NULL;

    return ret;
}


int OpenVGAPIWrapper::vgGetImageSubData()
{
    VGImage image;
    m_currentFunctionCall.GetParamValue( image, 0 );
    VGint dataStride;
    m_currentFunctionCall.GetParamValue( dataStride, 2 );
    VGImageFormat dataFormat;
    m_currentFunctionCall.GetParamValue( dataFormat, 3 );
    VGint x;
    m_currentFunctionCall.GetParamValue( x, 4 );
    VGint y;
    m_currentFunctionCall.GetParamValue( y, 5 );
    VGint width;
    m_currentFunctionCall.GetParamValue( width, 6 );
    VGint height;
    m_currentFunctionCall.GetParamValue( height, 7 );
	VGint pixmapSize;
	m_currentFunctionCall.GetParamValue(pixmapSize, 8);

    void *data = iStack->AllocFromStack(pixmapSize, 4 );

    ::vgGetImageSubData( image, data, dataStride, dataFormat, x, y, width, height );
    m_currentFunctionCall.SetVectorData( data, pixmapSize, 1 );

    int ret = WriteReply();

    iStack->ClearStack();
    data = NULL;

    return ret;
}


int OpenVGAPIWrapper::vgPointAlongPath()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );
    VGint startSegment;
    m_currentFunctionCall.GetParamValue( startSegment, 1 );
    VGint numSegments;
    m_currentFunctionCall.GetParamValue( numSegments, 2 );
    VGfloat distance;
    m_currentFunctionCall.GetParamValue( distance, 3 );
    VGfloat * x(NULL);
    VGfloat * y(NULL);
    VGfloat * tangentX(NULL);
    VGfloat * tangentY(NULL);
    VGfloat xval;
    VGfloat yval;
    VGfloat tangentXval;
    VGfloat tangentYval;

    TInt size;
    VGfloat *data;
    m_currentFunctionCall.GetVectorData( data, size, 4 );
    if ( size )
    {
        x = &xval;
    }
    m_currentFunctionCall.GetVectorData( data, size, 5 );
    if ( size )
    {
        y = &yval;
    }
    m_currentFunctionCall.GetVectorData( data, size, 6 );
    if ( size )
    {
        tangentX = &tangentXval;
    }
    m_currentFunctionCall.GetVectorData( data, size, 7 );
    if ( size )
    {
        tangentY = &tangentYval;
    }

    ::vgPointAlongPath( path, startSegment, numSegments, distance, x, y, tangentX, tangentY );

    if ( x )
    {
        m_currentFunctionCall.SetVectorData( x, 1, 4 );
    }
    if ( y )
    {
        m_currentFunctionCall.SetVectorData( y, 1, 5 );
    }
    if ( tangentX )
    {
        m_currentFunctionCall.SetVectorData( tangentX, 1, 6 );
    }
    if ( tangentY )
    {
        m_currentFunctionCall.SetVectorData( tangentY, 1, 7 );
    }

    int ret = WriteReply();

    return ret;
}


int OpenVGAPIWrapper::vgPathBounds()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );
    VGfloat minX;
    VGfloat minY;
    VGfloat width;
    VGfloat height;

    ::vgPathBounds( path, &minX, &minY, &width, &height );

    m_currentFunctionCall.SetVectorData( &minX, 1, 1 );
    m_currentFunctionCall.SetVectorData( &minY, 1, 2 );
    m_currentFunctionCall.SetVectorData( &width, 1, 3 );
    m_currentFunctionCall.SetVectorData( &height, 1, 4 );
    return WriteReply();
}


int OpenVGAPIWrapper::vgPathTransformedBounds()
{
    VGPath path;
    m_currentFunctionCall.GetParamValue( path, 0 );
    VGfloat minX;
    VGfloat minY;
    VGfloat width;
    VGfloat height;

    ::vgPathTransformedBounds( path, &minX, &minY, &width, &height );

    m_currentFunctionCall.SetVectorData( &minX, 1, 1 );
    m_currentFunctionCall.SetVectorData( &minY, 1, 2 );
    m_currentFunctionCall.SetVectorData( &width, 1, 3 );
    m_currentFunctionCall.SetVectorData( &height, 1, 4 );
    return WriteReply();
}

//vgu 1.1
int OpenVGAPIWrapper::vguLine()
{
    VGHandle path;
    VGfloat x0, y0, x1, y1;

    m_currentFunctionCall.GetParamValue( path, 0 );
    m_currentFunctionCall.GetParamValue( x0, 1 );
    m_currentFunctionCall.GetParamValue( y0, 2 );
    m_currentFunctionCall.GetParamValue( x1, 3 );
    m_currentFunctionCall.GetParamValue( y1, 4 );

    VGUErrorCode error = ::vguLine(path, x0, y0, x1, y1);
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}


int OpenVGAPIWrapper::vguPolygon()
{
    VGHandle path;
    VGint count;
    VGfloat *points;
    TInt size;
    VGboolean closed;

    m_currentFunctionCall.GetParamValue( path, 0 );
    m_currentFunctionCall.GetParamValue( count, 1 );
    m_currentFunctionCall.GetParamValue( closed, 2 );
    m_currentFunctionCall.GetVectorData( points, size, 3 );

    VGUErrorCode error = ::vguPolygon(path, points, count, closed );
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}


int OpenVGAPIWrapper::vguRect()
{
    VGHandle path;
    VGfloat x, y, width, height;

    m_currentFunctionCall.GetParamValue( path, 0 );
    m_currentFunctionCall.GetParamValue( x, 1 );
    m_currentFunctionCall.GetParamValue( y, 2 );
    m_currentFunctionCall.GetParamValue( width, 3 );
    m_currentFunctionCall.GetParamValue( height, 4 );

    VGUErrorCode error = ::vguRect(path, x, y, width, height);
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}


int OpenVGAPIWrapper::vguRoundRect()
{
    VGHandle path;
    VGfloat x, y, width, height, arcWidth, arcHeight;

    m_currentFunctionCall.GetParamValue( path, 0 );
    m_currentFunctionCall.GetParamValue( x, 1 );
    m_currentFunctionCall.GetParamValue( y, 2 );
    m_currentFunctionCall.GetParamValue( width, 3 );
    m_currentFunctionCall.GetParamValue( height, 4 );
    m_currentFunctionCall.GetParamValue( arcWidth, 5 );
    m_currentFunctionCall.GetParamValue( arcHeight, 6 );

    VGUErrorCode error = ::vguRoundRect(path, x, y, width, height,
        arcWidth, arcHeight);
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}


int OpenVGAPIWrapper::vguEllipse()
{
    VGHandle path;
    VGfloat cx, cy, width, height;

    m_currentFunctionCall.GetParamValue( path, 0 );
    m_currentFunctionCall.GetParamValue( cx, 1 );
    m_currentFunctionCall.GetParamValue( cy, 2 );
    m_currentFunctionCall.GetParamValue( width, 3 );
    m_currentFunctionCall.GetParamValue( height, 4 );

    VGUErrorCode error = ::vguEllipse(path, cx, cy, width, height);
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}


int OpenVGAPIWrapper::vguArc()
{
    VGHandle path;
    VGfloat x, y, width, height, startAngle, angleExtent;
    VGUArcType arcType;
    m_currentFunctionCall.GetParamValue( path, 0 );
    m_currentFunctionCall.GetParamValue( x, 1 );
    m_currentFunctionCall.GetParamValue( y, 2 );
    m_currentFunctionCall.GetParamValue( width, 3 );
    m_currentFunctionCall.GetParamValue( height, 4 );
    m_currentFunctionCall.GetParamValue( startAngle, 5 );
    m_currentFunctionCall.GetParamValue( angleExtent, 6 );
    m_currentFunctionCall.GetParamValue( arcType, 7 );

    VGUErrorCode error = ::vguArc(path, x, y, width, height,
        startAngle, angleExtent, arcType);
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}


int OpenVGAPIWrapper::vguComputeWarpQuadToSquare()
{
    VGfloat sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3;
    VGfloat matrix[9];

    m_currentFunctionCall.GetParamValue( sx0, 0 );
    m_currentFunctionCall.GetParamValue( sy0, 1 );
    m_currentFunctionCall.GetParamValue( sx1, 2 );
    m_currentFunctionCall.GetParamValue( sy1, 3 );
    m_currentFunctionCall.GetParamValue( sx2, 4 );
    m_currentFunctionCall.GetParamValue( sy2, 5 );
    m_currentFunctionCall.GetParamValue( sx3, 6 );
    m_currentFunctionCall.GetParamValue( sy3, 7 );

    VGUErrorCode error = ::vguComputeWarpQuadToSquare( sx0, sy0,
        sx1, sy1, sx2, sy2, sx3, sy3, matrix );
    m_currentFunctionCall.SetVectorData( matrix, 9, 8 );
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}


int OpenVGAPIWrapper::vguComputeWarpSquareToQuad()
{
    VGfloat dx0, dy0, dx1, dy1, dx2, dy2, dx3, dy3;
    VGfloat matrix[9];

    m_currentFunctionCall.GetParamValue( dx0, 0 );
    m_currentFunctionCall.GetParamValue( dy0, 1 );
    m_currentFunctionCall.GetParamValue( dx1, 2 );
    m_currentFunctionCall.GetParamValue( dy1, 3 );
    m_currentFunctionCall.GetParamValue( dx2, 4 );
    m_currentFunctionCall.GetParamValue( dy2, 5 );
    m_currentFunctionCall.GetParamValue( dx3, 6 );
    m_currentFunctionCall.GetParamValue( dy3, 7 );

    VGUErrorCode error = ::vguComputeWarpSquareToQuad( dx0, dy0,
        dx1, dy1, dx2, dy2, dx3, dy3, matrix );
    m_currentFunctionCall.SetVectorData( matrix, 9, 8 );
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}


int OpenVGAPIWrapper::vguComputeWarpQuadToQuad()
{
    VGfloat dx0, dy0, dx1, dy1, dx2, dy2, dx3, dy3;
    VGfloat sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3;
    VGfloat matrix[9];

    m_currentFunctionCall.GetParamValue( dx0, 0 );
    m_currentFunctionCall.GetParamValue( dy0, 1 );
    m_currentFunctionCall.GetParamValue( dx1, 2 );
    m_currentFunctionCall.GetParamValue( dy1, 3 );
    m_currentFunctionCall.GetParamValue( dx2, 4 );
    m_currentFunctionCall.GetParamValue( dy2, 5 );
    m_currentFunctionCall.GetParamValue( dx3, 6 );
    m_currentFunctionCall.GetParamValue( dy3, 7 );
    m_currentFunctionCall.GetParamValue( sx0, 8 );
    m_currentFunctionCall.GetParamValue( sy0, 9 );
    m_currentFunctionCall.GetParamValue( sx1, 10 );
    m_currentFunctionCall.GetParamValue( sy1, 11 );
    m_currentFunctionCall.GetParamValue( sx2, 12 );
    m_currentFunctionCall.GetParamValue( sy2, 13 );
    m_currentFunctionCall.GetParamValue( sx3, 14 );
    m_currentFunctionCall.GetParamValue( sy3, 15 );

    VGUErrorCode error = ::vguComputeWarpQuadToQuad( dx0, dy0, dx1, dy1, dx2,
        dy2, dx3, dy3, sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3, matrix );
    m_currentFunctionCall.SetVectorData( matrix, 9, 16 );
    m_currentFunctionCall.SetReturnValue((TUint32)error);
    return WriteReply();
}

/**
*
*@param aParamIndex the index where the first pbuffer surface handle is located. The other(s) are located in the subsequent positions.
*@param aDst destination image
*@param aSrc source image
*/

void OpenVGAPIWrapper::doSync( int aParamIndex, VGImage aDst, VGImage aSrc )
{
	TRACE("DriverAPIWrapper::doSync ->\n");
	int pbuffer(0);

	if( aParamIndex >= m_currentFunctionCall.Data().Header().iParameterCount ){return;}
	m_currentFunctionCall.GetParamValue( pbuffer, aParamIndex++ );
	if( pbuffer )
	{	
		m_APIWrapper->GetDriverWrapper()->SyncVGImageFromPBuffer( (EGLSurface)pbuffer, aDst );
	}

	if( aParamIndex >= m_currentFunctionCall.Data().Header().iParameterCount ){return;}
	m_currentFunctionCall.GetParamValue( pbuffer, aParamIndex++ );
	if( pbuffer )
	{
		m_APIWrapper->GetDriverWrapper()->SyncVGImageFromPBuffer( (EGLSurface)pbuffer, aSrc );
	}
	TRACE("DriverAPIWrapper::doSync <-\n");
}

OpenVGAPIWrapper::OpenVGAPIWrapper(RemoteFunctionCallData& currentFunctionCallData, APIWrapperStack* stack, void* result, MGraphicsVHWCallback* serviceIf, KhronosAPIWrapper *aAPIWrapper ):
	APIWrapper( currentFunctionCallData, stack, result, serviceIf ),
	m_currentFunctionCall( m_currentFunctionCallData ),
	vgePathCoordsSizeInBytesPtr( NULL ),
	m_APIWrapper( aAPIWrapper )
{
	//vgePathCoordsSizeInBytesPtr = (vgePathCoordsSizeInBytesPtrType)::eglGetProcAddress("vgePathCoordsSizeInBytes");
}

int OpenVGAPIWrapper::WriteReply()
{
#ifdef LOG_ERROR
	int operationid = (int)m_currentFunctionCall.Data().Header().iOpCode;
	
	/*int vgerror*/VGErrorCode vgerror = ::vgSimulatorGetError();
	if ( m_lastVgError != vgerror )
	{
		if ( VG_NO_ERROR != vgerror )
		{
			printf("OpenVG error 0x%X, for request %d\n", vgerror, operationid );
		}
		m_lastVgError = vgerror;
	}
#endif
	return APIWrapper::WriteReply();
}

int OpenVGAPIWrapper::DispatchRequest( unsigned long aCode )
{
    int ret(0);
    switch ( aCode )
    {
        case OpenVgRFC::EvgCreatePaint:
        {
            ret = vgCreatePaint();
            break;
        }
        case OpenVgRFC::EvgDestroyPaint:
        {
            ret = vgDestroyPaint();
            break;
        }
        case OpenVgRFC::EvgSetPaint:
        {
            ret = vgSetPaint();
            break;
        }
        case OpenVgRFC::EvgSetParameteri:
        {
            ret = vgSetParameteri();
            break;
        }
        case OpenVgRFC::EvgSetParameterfv:
        {
            ret = vgSetParameterfv();
            break;
        }
        case OpenVgRFC::EvgCreatePath:
        {
            ret = vgCreatePath();
            break;
        }
        case OpenVgRFC::EvgDrawPath:
        {
            ret = vgDrawPath();
            break;
        }
        case OpenVgRFC::EvgDestroyPath:
        {
            ret = vgDestroyPath();
            break;
        }
        case OpenVgRFC::EvgAppendPathData:
        {
            ret = vgAppendPathData();
            break;
        }
        case OpenVgRFC::EvgGetError:
        {
            ret = vgGetError();
            break;
        }
        case OpenVgRFC::EvgSeti:
        {
            ret = vgSeti();
            break;
        }
        case OpenVgRFC::EvgSetf:
        {
            ret = vgSetf();
            break;
        }
        case OpenVgRFC::EvgSetfv:
        {
            ret = vgSetfv();
            break;
        }
        case OpenVgRFC::EvgClear:
        {
            ret = vgClear();
            break;
        }
        case OpenVgRFC::EvgLoadIdentity:
        {
            ret = vgLoadIdentity();
            break;
        }
        case OpenVgRFC::EvgRotate:
        {
            ret = vgRotate();
            break;
        }
        case OpenVgRFC::EvgScale:
        {
            ret = vgScale();
            break;
        }
        case OpenVgRFC::EvgTranslate:
        {
            ret = vgTranslate();
            break;
        }
        case OpenVgRFC::EvgReadPixels:
        {
            ret = vgReadPixels();
            break;
        }
        case OpenVgRFC::EvgFlush:
        {
            ret = vgFlush();
            break;
        }
        case OpenVgRFC::EvgFinish:
        {
            ret = vgFinish();
            break;
        }
        case OpenVgRFC::EvgSetiv:
        {
            ret = vgSetiv();
            break;
        }
        case OpenVgRFC::EvgGetPaint:
        {
            ret = vgGetPaint();
            break;
        }
        case OpenVgRFC::EvgInterpolatePath:
        {
            ret =vgInterpolatePath();
            break;
        }
        case OpenVgRFC::EvgPathLength:
        {
            ret = vgPathLength();
            break;
        }
        case OpenVgRFC::EvgGetPathCapabilities:
        {
            ret = vgGetPathCapabilities();
            break;
        }
        case OpenVgRFC::EvgGetf:
        {
            ret = vgGetf();
            break;
        }
        case OpenVgRFC::EvgGeti:
        {
            ret = vgGeti();
            break;
        }
        case OpenVgRFC::EvgGetVectorSize:
        {
            ret = vgGetVectorSize();
            break;
        }
        case OpenVgRFC::EvgGetParameterf://10
        {
            ret = vgGetParameterf();
            break;
        }
        case OpenVgRFC::EvgGetParameteri:
        {
            ret = vgGetParameteri();
            break;
        }
        case OpenVgRFC::EvgGetParameterVectorSize:
        {
            ret = vgGetParameterVectorSize();
            break;
        }
        case OpenVgRFC::EvgCreateMaskLayer:
        {
            ret = vgCreateMaskLayer();
            break;
        }
        case OpenVgRFC::EvgGetColor:
        {
            ret = vgGetColor();
            break;
        }
        case OpenVgRFC::EvgCreateImage:
        {
            ret = vgCreateImage();
            break;
        }
        case OpenVgRFC::EvgChildImage:
        {
            ret = vgChildImage();
            break;
        }
        case OpenVgRFC::EvgGetParent:
        {
            ret = vgGetParent();
            break;
        }
        case OpenVgRFC::EvgCreateFont:
        {
            ret = vgCreateFont();
            break;
        }
        case OpenVgRFC::EvgHardwareQuery:
        {
            ret = vgHardwareQuery();
            break;
        }
//20
        case OpenVgRFC::EvgGetParameterfv:
        {
            ret = vgGetParameterfv();
            break;
        }
        case OpenVgRFC::EvgGetParameteriv:
        {
            ret = vgGetParameteriv();
            break;
        }
        case OpenVgRFC::EvgGetfv:
        {
            ret = vgGetfv();
            break;
        }
        case OpenVgRFC::EvgGetiv:
        {
            ret = vgGetiv();
            break;
        }
        case OpenVgRFC::EvgGetMatrix:
        {
            ret = vgGetMatrix();
            break;
        }
        case OpenVgRFC::EvgGetImageSubData:
        {
            ret = vgGetImageSubData();
            break;
        }
        case OpenVgRFC::EvgSetParameterf:
        {
            ret = vgSetParameterf();
            break;
        }
        case OpenVgRFC::EvgSetParameteriv:
        {
            ret = vgSetParameteriv();
            break;
        }
        case OpenVgRFC::EvgLoadMatrix:
        {
            ret = vgLoadMatrix();
            break;
        }
        case OpenVgRFC::EvgMultMatrix:   //40
        {
            ret = vgMultMatrix();
            break;
        }
        case OpenVgRFC::EvgShear:
        {
            ret = vgShear();
            break;
        }
        case OpenVgRFC::EvgMask:
        {
            ret = vgMask();
            break;
        }
        case OpenVgRFC::EvgRenderToMask:
        {
            ret = vgRenderToMask();
            break;
        }
        case OpenVgRFC::EvgDestroyMaskLayer:
        {
            ret = vgDestroyMaskLayer();
            break;
        }
        case OpenVgRFC::EvgFillMaskLayer:
        {
            ret = vgFillMaskLayer();
            break;
        }
        case OpenVgRFC::EvgCopyMask:
        {
            ret = vgCopyMask();
            break;
        }
        case OpenVgRFC::EvgClearPath:
        {
            ret = vgClearPath();
            break;
        }
        case OpenVgRFC::EvgRemovePathCapabilities:
        {
            ret = vgRemovePathCapabilities();
            break;
        }
        case OpenVgRFC::EvgAppendPath:
        {
            ret = vgAppendPath();
            break;
        }
        case OpenVgRFC::EvgModifyPathCoords:
        {
            ret = vgModifyPathCoords();
            break;
        }
        case OpenVgRFC::EvgTransformPath:
        {
            ret = vgTransformPath();
            break;
        }
        case OpenVgRFC::EvgPointAlongPath:
        {
            ret = vgPointAlongPath();
            break;
        }
        case OpenVgRFC::EvgPathBounds:
        {
            ret = vgPathBounds();
            break;
        }
        case OpenVgRFC::EvgPathTransformedBounds:
        {
            ret = vgPathTransformedBounds();
            break;
        }
        case OpenVgRFC::EvgSetColor:
        {
            ret = vgSetColor();
            break;
        }
        case OpenVgRFC::EvgPaintPattern:
        {
            ret = vgPaintPattern();
            break;
        }
        case OpenVgRFC::EvgDestroyImage:
        {
            ret = vgDestroyImage();
            break;
        }
        case OpenVgRFC::EvgClearImage:
        {
            ret = vgClearImage();
            break;
        }
        case OpenVgRFC::EvgImageSubData:
        {
            ret = vgImageSubData();
            break;
        }
        case OpenVgRFC::EvgCopyImage:
        {
            ret = vgCopyImage();
            break;
        }
        case OpenVgRFC::EvgDrawImage:
        {
            ret = vgDrawImage();
            break;
        }
        case OpenVgRFC::EvgSetPixels:
        {
            ret = vgSetPixels();
            break;
        }
        case OpenVgRFC::EvgWritePixels:
        {
            ret = vgWritePixels();
            break;
        }
        case OpenVgRFC::EvgGetPixels:
        {
            ret = vgGetPixels();
            break;
        }
        case OpenVgRFC::EvgCopyPixels:
        {
            ret = vgCopyPixels();
            break;
        }
        case OpenVgRFC::EvgDestroyFont:
        {
            ret = vgDestroyFont();
            break;
        }
        case OpenVgRFC::EvgSetGlyphToPath:
        {
            ret = vgSetGlyphToPath();
            break;
        }
        case OpenVgRFC::EvgSetGlyphToImage:
        {
            ret = vgSetGlyphToImage();
            break;
        }
        case OpenVgRFC::EvgClearGlyph:
        {
            ret = vgClearGlyph();
            break;
        }
        case OpenVgRFC::EvgDrawGlyph:
        {
            ret = vgDrawGlyph();
            break;
        }
        case OpenVgRFC::EvgDrawGlyphs:
        {
            ret = vgDrawGlyphs();
            break;
        }
        case OpenVgRFC::EvgColorMatrix:
        {
            ret = vgColorMatrix();
            break;
        }
        case OpenVgRFC::EvgConvolve:
        {
            ret = vgConvolve();
            break;
        }
        case OpenVgRFC::EvgSeparableConvolve:
        {
            ret = vgSeparableConvolve();
            break;
        }
        case OpenVgRFC::EvgGaussianBlur:
        {
            ret = vgGaussianBlur();
            break;
        }
        case OpenVgRFC::EvgLookup:
        {
            ret = vgLookup();
            break;
        }
        case OpenVgRFC::EvgLookupSingle:
        {
            ret = vgLookupSingle();
            break;
        }
        case OpenVgRFC::EvgePathCoordsSizeInBytes:
        {
            ret = vgePathCoordsSizeInBytes();
            break;
        }
        case OpenVgRFC::EvgGetString:
//		{
//		ret = vgGetString();
//		break;
//		}
        case OpenVgRFC::EvguLine:
        {
            ret = vguLine();
            break;
        }
        case OpenVgRFC::EvguPolygon:
        {
            ret = vguPolygon();
            break;
        }
        case OpenVgRFC::EvguRect:
        {
            ret = vguRect();
            break;
        }
        case OpenVgRFC::EvguRoundRect:
        {
            ret = vguRoundRect();
            break;
        }
        case OpenVgRFC::EvguEllipse:
        {
            ret = vguEllipse();
            break;
        }
        case OpenVgRFC::EvguArc:
        {
            ret = vguArc();
            break;
        }
        case OpenVgRFC::EvguComputeWarpQuadToSquare:
        {
            ret = vguComputeWarpQuadToSquare();
            break;
        }
        case OpenVgRFC::EvguComputeWarpSquareToQuad:
        {
            ret = vguComputeWarpSquareToQuad();
            break;
        }
        case OpenVgRFC::EvguComputeWarpQuadToQuad:
        {
            ret = vguComputeWarpQuadToQuad();
            break;
        }
        default:
            TRACE("Unimplemented Openvg Op code %u\n",aCode);
            break;
    }
    return ret;
}
