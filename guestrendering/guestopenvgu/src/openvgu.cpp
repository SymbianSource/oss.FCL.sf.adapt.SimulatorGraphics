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
// Implementation of OpenVGU stub functions

#include "vghwutils.h"

#define __VG_OPENVG_H_
#include <VG/1.1/openvg.h>

#include "openvgrfc.h"

class RemoteFunctionCall;

// tracing
#ifdef _DEBUG
	#include <e32debug.h>
    #define OPENVGU_TRACE(fmt, args...) RDebug::Printf(fmt, ##args)
#else
    #define OPENVGU_TRACE(fmt, args...)
#endif


extern "C" {
/*
 Note: Comments at the start of each Open VG api are adapted from the Open VG 1.1 specification.
 The text has been chosen/adapted to give a helpful overview of the function, and the errors
 that it may generate.  For more details and diagrams see the full Open VG specification.
 */


/*
 Append a line segment to a path.

 ERRORS
   VGU_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VGU_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_APPEND_TO is not enabled for path
 */
EXPORT_C VGUErrorCode
     vguLine(VGPath path,
        VGfloat x0, VGfloat y0,
        VGfloat x1, VGfloat y1)
    {
	OPENVGU_TRACE("vguLine path=%d x0=0x%x, y0=0x%x, x1=0x%x, y1=0x%x", path, x0, y0, x1, y1);

	MVgContext* vgContext = CVghwUtils::VgContext();
	VGUErrorCode error = VGU_OUT_OF_MEMORY_ERROR;  // nearest VGU has to an Invalid Context error
	if (vgContext)
		{
		RemoteFunctionCallData rfcdata; OpenVgRFC vguApiData(rfcdata);
		vguApiData.Init(OpenVgRFC::EvguLine);
		vguApiData.AppendParam(path);
		vguApiData.AppendParam(x0);
		vguApiData.AppendParam(y0);
		vguApiData.AppendParam(x1);
		vguApiData.AppendParam(y1);
		// TODO return type specific functions for VGU operations
		vgContext->ExecuteVgCommand(vguApiData);
		error = (VGUErrorCode)vguApiData.ReturnValue();
		}
	return error;
    }

/*
 Append a polyline (connected sequence of line segments) or polygon to a path.

 ERRORS
   VGU_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VGU_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_APPEND_TO is not enabled for path
   VGU_ILLEGAL_ARGUMENT_ERROR
   – if points is NULL
   – if points is not properly aligned
   – if count is less than or equal to 0
 */
EXPORT_C VGUErrorCode
     vguPolygon(VGPath path,
        const VGfloat * points,
        VGint count,
        VGboolean closed)
    {
	OPENVGU_TRACE("vguPolygon path=%d, points=0x%x, count=%d, closed=%d", path, points, count, closed);

	MVgContext* vgContext = CVghwUtils::VgContext();
	VGUErrorCode error = VGU_OUT_OF_MEMORY_ERROR;  // nearest VGU has to an Invalid Context error
	if (vgContext)
		{
		if ( (points == NULL) || (count <= 0) || (3ul & (unsigned)points) )
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else
			{
			RemoteFunctionCallData rfcdata; OpenVgRFC vguApiData(rfcdata);
			vguApiData.Init(OpenVgRFC::EvguPolygon);
			vguApiData.AppendParam(path);
			vguApiData.AppendParam(count);
			vguApiData.AppendParam(closed);
			vguApiData.AppendVector(points, count * 2);

			vgContext->ExecuteVgCommand(vguApiData);
			error = (VGUErrorCode)vguApiData.ReturnValue();
			}
		}
	return error;
    }

/*
 Append an axis-aligned rectangle with its lower-left corner at (x, y) and
 a given width and height to a path.

 ERRORS
   VGU_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VGU_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_APPEND_TO is not enabled for path
   VGU_ILLEGAL_ARGUMENT_ERROR
   – if width or height are less than or equal to 0
 */
EXPORT_C VGUErrorCode
     vguRect(VGPath path,
        VGfloat x, VGfloat y,
        VGfloat width, VGfloat height)
    {
	OPENVGU_TRACE("vguRect path=%d, x=0x%x, y=0x%x, width=0x%x, height=0x%x", path, x, y, width, height);

	MVgContext* vgContext = CVghwUtils::VgContext();
	VGUErrorCode error = VGU_OUT_OF_MEMORY_ERROR;  // nearest VGU has to an Invalid Context error
	if (vgContext)
		{
		if ( (width <= 0) || (height <= 0) )
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else
			{
			RemoteFunctionCallData rfcdata; OpenVgRFC vguApiData(rfcdata);
			vguApiData.Init(OpenVgRFC::EvguRect);
			vguApiData.AppendParam(path);
			vguApiData.AppendParam(x);
			vguApiData.AppendParam(y);
			vguApiData.AppendParam(width);
			vguApiData.AppendParam(height);

			vgContext->ExecuteVgCommand(vguApiData);
			error = (VGUErrorCode)vguApiData.ReturnValue();
			}
		}
	return error;
    }

/*
 Append an axis-aligned round-cornered rectangle with the lower-left corner of
 its rectangular bounding box at (x, y) and a given width, height, arcWidth,
 and arcHeight to a path.

 ERRORS
   VGU_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VGU_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_APPEND_TO is not enabled for path
   VGU_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
 */
EXPORT_C VGUErrorCode
     vguRoundRect(VGPath path,
        VGfloat x, VGfloat y,
        VGfloat width,
        VGfloat height,
        VGfloat arcWidth,
        VGfloat arcHeight)
    {
	OPENVGU_TRACE("vguRoundRect path=%d, x=0x%x, y=0x%x, width=0x%x, height=0x%x, arcWidth=0x%x, arcHeight=0x%x",
			path, x, y, width, height, arcWidth, arcHeight);

	MVgContext* vgContext = CVghwUtils::VgContext();
	VGUErrorCode error = VGU_OUT_OF_MEMORY_ERROR;  // nearest VGU has to an Invalid Context error
	if (vgContext)
		{
		if ( (width <= 0) || (height <= 0) )
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else
			{
			RemoteFunctionCallData rfcdata; OpenVgRFC vguApiData(rfcdata);
			vguApiData.Init(OpenVgRFC::EvguRoundRect);
			vguApiData.AppendParam(path);
			vguApiData.AppendParam(x);
			vguApiData.AppendParam(y);
			vguApiData.AppendParam(width);
			vguApiData.AppendParam(height);
			vguApiData.AppendParam(arcWidth);
			vguApiData.AppendParam(arcHeight);

			vgContext->ExecuteVgCommand(vguApiData);
			error = (VGUErrorCode)vguApiData.ReturnValue();
			}
		}
	return error;
    }

/*
 Append an axis-aligned ellipse to a path. The center of the ellipse is given by
 (cx, cy) and the dimensions of the axis-aligned rectangle enclosing the ellipse are
 given by width and height. The ellipse begins at (cx + width/2, cy) and is stroked
 as two equal counter-clockwise arcs.

 ERRORS
   VGU_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VGU_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_APPEND_TO is not enabled for path
   VGU_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
 */
EXPORT_C VGUErrorCode
     vguEllipse(VGPath path,
        VGfloat cx, VGfloat cy,
        VGfloat width,
        VGfloat height)
    {
	OPENVGU_TRACE("vguEllipse path=%d, cx=0x%x, cy=0x%x, width=0x%x, height=0x%x", path, cx, cy, width, height);

	MVgContext* vgContext = CVghwUtils::VgContext();
	VGUErrorCode error = VGU_OUT_OF_MEMORY_ERROR;  // nearest VGU has to an Invalid Context error
	if (vgContext)
		{
		if ( (width <= 0) || (height <= 0) )
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else
			{
			RemoteFunctionCallData rfcdata; OpenVgRFC vguApiData(rfcdata);
			vguApiData.Init(OpenVgRFC::EvguEllipse);
			vguApiData.AppendParam(path);
			vguApiData.AppendParam(cx);
			vguApiData.AppendParam(cy);
			vguApiData.AppendParam(width);
			vguApiData.AppendParam(height);

			vgContext->ExecuteVgCommand(vguApiData);
			error = (VGUErrorCode)vguApiData.ReturnValue();
			}
		}
	return error;
    }

/*
 Append an elliptical arc to a path, possibly along with one or two line segments,
 according to the arcType parameter. The startAngle and angleExtent parameters are
 given in degrees, proceeding counter-clockwise from the positive X axis. The arc
 is defined on the unit circle, then scaled by the width and height of the ellipse.

 ERRORS
   VGU_BAD_HANDLE_ERROR
   – if path is not a valid path handle, or is not shared with the current context
   VGU_PATH_CAPABILITY_ERROR
   – if VG_PATH_CAPABILITY_APPEND_TO is not enabled for path
   VGU_ILLEGAL_ARGUMENT_ERROR
   – if width or height is less than or equal to 0
   – if arcType is not one of the values from the VGUArcType enumeration
 */
EXPORT_C VGUErrorCode
     vguArc(VGPath path,
        VGfloat x, VGfloat y,
        VGfloat width, VGfloat height,
        VGfloat startAngle,
        VGfloat angleExtent,
        VGUArcType arcType)
    {
	OPENVGU_TRACE("vguArc path=%d, x=0x%x, y=0x%x, width=0x%x, height=0x%x, startAngle=0x%x, angleExtent=0x%x, arcType=%d",
			path, x, y, width, height, startAngle, angleExtent, arcType);

	MVgContext* vgContext = CVghwUtils::VgContext();
	VGUErrorCode error = VGU_OUT_OF_MEMORY_ERROR;  // nearest VGU has to an Invalid Context error
	if (vgContext)
		{
		const unsigned int n_posinf = 0x7f800000u;
		const unsigned int n_neginf = 0xff800000u;
		VGfloat posinf = *(VGfloat*)&n_posinf;
		VGfloat neginf = *(VGfloat*)&n_neginf;

		if ( (width <= 0) || (height <= 0) || ( (arcType != VGU_ARC_OPEN) && (arcType == VGU_ARC_CHORD) && (arcType == VGU_ARC_PIE) ) )
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		// Tony TODO review NaN detection; not convinced that this works
		else if ( ((x + x - x) != x) || ((y + y - y) != y) ) /* NaN's */
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else if ( ((startAngle + startAngle - startAngle) != startAngle) || ((angleExtent + angleExtent - angleExtent) != angleExtent) )
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else if ( (startAngle <= neginf) || (startAngle >= posinf) )
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else if ( (angleExtent <= neginf) || (angleExtent >= posinf) )
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else if ( (x <= neginf) || (x >= posinf) ) /*Infinity x */
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else if ( (y <= neginf) || (y >= posinf) ) /*Infinity x */
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else
			{
			RemoteFunctionCallData rfcdata; OpenVgRFC vguApiData(rfcdata);
			vguApiData.Init(OpenVgRFC::EvguArc);
			vguApiData.AppendParam(path);
			vguApiData.AppendParam(x);
			vguApiData.AppendParam(y);
			vguApiData.AppendParam(width);
			vguApiData.AppendParam(height);
			vguApiData.AppendParam(startAngle);
			vguApiData.AppendParam(angleExtent);
			vguApiData.AppendParam(arcType);

			vgContext->ExecuteVgCommand(vguApiData);
			error = (VGUErrorCode)vguApiData.ReturnValue();
			}
		}
	return error;
    }

/*
 Sets the entries of matrix to a projective transformation that maps the point
 (sx0, sy0) to (0, 0); (sx1, sy1) to (1, 0); (sx2, sy2) to (0, 1); and (sx3, sy3)
 to (1, 1). If no non-degenerate matrix satisfies the constraints,
 VGU_BAD_WARP_ERROR is returned and matrix is unchanged.

 ERRORS
   VGU_ILLEGAL_ARGUMENT_ERROR
   – if matrix is NULL
   – if matrix is not properly aligned
   VGU_BAD_WARP_ERROR
   – if no non-degenerate transformation satisfies the constraints
 */
EXPORT_C VGUErrorCode
    vguComputeWarpQuadToSquare(VGfloat sx0, VGfloat sy0,
        VGfloat sx1, VGfloat sy1,
        VGfloat sx2, VGfloat sy2,
        VGfloat sx3, VGfloat sy3,
        VGfloat * matrix)
    {
	OPENVGU_TRACE("vguComputeWarpQuadToSquare sx0=0x%x, sy0=0x%x, sx1=0x%x, sy1=0x%x, sx2=0x%x, sy2=0x%x, sx3=0x%x, sy3=0x%x, matrix=0x%x",
			sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3, matrix);

	MVgContext* vgContext = CVghwUtils::VgContext();
	VGUErrorCode error = VGU_OUT_OF_MEMORY_ERROR;  // nearest VGU has to an Invalid Context error
	if (vgContext)
		{
		if ( (matrix == NULL) || (3ul & (unsigned)matrix))
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else
			{
			RemoteFunctionCallData rfcdata; OpenVgRFC vguApiData( rfcdata );
			vguApiData.Init(OpenVgRFC::EvguComputeWarpQuadToSquare);
			vguApiData.AppendParam(sx0);
			vguApiData.AppendParam(sy0);
			vguApiData.AppendParam(sx1);
			vguApiData.AppendParam(sy1);
			vguApiData.AppendParam(sx2);
			vguApiData.AppendParam(sy2);
			vguApiData.AppendParam(sx3);
			vguApiData.AppendParam(sy3);
			vguApiData.AppendVector(matrix, 9, RemoteFunctionCallData::EOut);

			vgContext->ExecuteVgCommand(vguApiData);
			error = (VGUErrorCode)vguApiData.ReturnValue();
			}
		}
	return error;
    }

/*
 Sets the entries of matrix to a projective transformation that maps the point
 (0, 0) to (dx0, dy0); (1, 0) to (dx1, dy1); (0, 1) to (dx2, dy2); and (1, 1) to
 (dx3, dy3). If no non-degenerate matrix satisfies the constraints, VGU_BAD_WARP_ERROR
 is returned and matrix is unchanged.

 ERRORS
   VGU_ILLEGAL_ARGUMENT_ERROR
   – if matrix is NULL
   – if matrix is not properly aligned
   VGU_BAD_WARP_ERROR
   – if no non-degenerate transformation satisfies the constraints
 */
EXPORT_C VGUErrorCode
    vguComputeWarpSquareToQuad(VGfloat dx0, VGfloat dy0,
        VGfloat dx1, VGfloat dy1,
        VGfloat dx2, VGfloat dy2,
        VGfloat dx3, VGfloat dy3,
        VGfloat * matrix)
    {
	OPENVGU_TRACE("vguComputeWarpSquareToQuad dx0=0x%x, dy0=0x%x, dx1=0x%x, dy1=0x%x, dx2=0x%x, dy2=0x%x, dx3=0x%x, dy3=0x%x, matrix=0x%x",
			dx0, dy0, dx1, dy1, dx2, dy2, dx3, dy3, matrix);

	MVgContext* vgContext = CVghwUtils::VgContext();
	VGUErrorCode error = VGU_OUT_OF_MEMORY_ERROR;  // nearest VGU has to an Invalid Context error
	if (vgContext)
		{
		if ( (matrix == NULL) || (3ul & (unsigned)matrix))
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else
			{
			RemoteFunctionCallData rfcdata; OpenVgRFC vguApiData(rfcdata);
			vguApiData.Init(OpenVgRFC::EvguComputeWarpSquareToQuad);
			vguApiData.AppendParam(dx0);
			vguApiData.AppendParam(dy0);
			vguApiData.AppendParam(dx1);
			vguApiData.AppendParam(dy1);
			vguApiData.AppendParam(dx2);
			vguApiData.AppendParam(dy2);
			vguApiData.AppendParam(dx3);
			vguApiData.AppendParam(dy3);
			vguApiData.AppendVector(matrix, 9, RemoteFunctionCallData::EOut);

			vgContext->ExecuteVgCommand(vguApiData);
			error = (VGUErrorCode)vguApiData.ReturnValue();
			}
		}
    return error;
    }

/*
 Sets the entries of matrix to a projective transformation that maps the point
 (sx0, sy0) to (dx0, dy0); (sx1, sy1) to (dx1, dy1); (sx2, sy2) to (dx2, dy2);
 and (sx3, sy3) to (dx3, dy3). If no non-degenerate matrix satisfies the constraints,
 VGU_BAD_WARP_ERROR is returned and matrix is unchanged.

 ERRORS
   VGU_ILLEGAL_ARGUMENT_ERROR
   – if matrix is NULL
   – if matrix is not properly aligned
   VGU_BAD_WARP_ERROR
   – if no non-degenerate transformation satisfies the constraints
 */
EXPORT_C VGUErrorCode
    vguComputeWarpQuadToQuad(VGfloat dx0, VGfloat dy0,
        VGfloat dx1, VGfloat dy1,
        VGfloat dx2, VGfloat dy2,
        VGfloat dx3, VGfloat dy3,
        VGfloat sx0, VGfloat sy0,
        VGfloat sx1, VGfloat sy1,
        VGfloat sx2, VGfloat sy2,
        VGfloat sx3, VGfloat sy3,
        VGfloat * matrix)
    {
	OPENVGU_TRACE("vguComputeWarpQuadToQuad");
	MVgContext* vgContext = CVghwUtils::VgContext();

	VGUErrorCode error = VGU_OUT_OF_MEMORY_ERROR;  // nearest VGU has to an Invalid Context error
	if (vgContext)
		{
		if ( (matrix == NULL) || (3ul & (unsigned)matrix))
			{
			error = VGU_ILLEGAL_ARGUMENT_ERROR;
			}
		else
			{
			RemoteFunctionCallData rfcdata; OpenVgRFC vguApiData(rfcdata);
			vguApiData.Init(OpenVgRFC::EvguComputeWarpQuadToQuad);
			vguApiData.AppendParam(dx0);
			vguApiData.AppendParam(dy0);
			vguApiData.AppendParam(dx1);
			vguApiData.AppendParam(dy1);
			vguApiData.AppendParam(dx2);
			vguApiData.AppendParam(dy2);
			vguApiData.AppendParam(dx3);
			vguApiData.AppendParam(dy3);
			vguApiData.AppendParam(sx0);
			vguApiData.AppendParam(sy0);
			vguApiData.AppendParam(sx1);
			vguApiData.AppendParam(sy1);
			vguApiData.AppendParam(sx2);
			vguApiData.AppendParam(sy2);
			vguApiData.AppendParam(sx3);
			vguApiData.AppendParam(sy3);
			vguApiData.AppendVector(matrix, 9, RemoteFunctionCallData::EOut);

			vgContext->ExecuteVgCommand(vguApiData);
			error = (VGUErrorCode)vguApiData.ReturnValue();
			}
		}
	return error;
    }

} /* extern "C" */
