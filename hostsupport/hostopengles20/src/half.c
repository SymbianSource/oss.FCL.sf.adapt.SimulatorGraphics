/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#include "half.h"

GLfloat dglConvertHalfToFloat(khronos_int16_t half)
{
	// INF and NaN are mapped to floating point values.

	khronos_int32_t exponent = half & 0x7c00;
	khronos_int16_t mantissa = half & 0x03ff;
	if(!exponent)
	{
		if(mantissa)
		{
			// Subnormals are mapped to zero.
			mantissa = 0;
		}
	}
	else
	{
		// Convert bias.
		exponent += 0x1c000;
	}

	{
		khronos_int32_t conv = ((half & 0x8000) << 16) |
							   (exponent << 13) |
							   (mantissa << 13);
		return *(GLfloat*)&conv;
	}
}
