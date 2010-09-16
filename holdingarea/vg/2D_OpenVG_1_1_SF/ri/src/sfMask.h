/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef __SFMASK_H
#define __SFMASK_H

#include "VG/openvg.h"

#ifndef __RIDEFS_H
#   include "riDefs.h"
#endif

#ifndef __RIIMAGE_H
#   include "riImage.h"
#endif

// Contains implementations of some mask operations that are needed in both the
// pixel-pipeline and the blitter.

namespace OpenVGRI {

RI_INLINE IntegerColor intMaskOperation(RIuint32 coverage, const IntegerColor& d, VGMaskOperation maskOperation);

RI_INLINE IntegerColor intMaskOperation(RIuint32 coverage, const IntegerColor& d, VGMaskOperation maskOperation)
{
    RIuint32 newCoverage;
    switch (maskOperation)
    {
        case VG_SET_MASK:
            // \note This should work approximately if coverage < 256 always.
            // See other cases for proper conversions.
            return IntegerColor(0, 0, 0, coverage); // nop.
            break;
        case VG_UNION_MASK:
        {
            RIuint32 oldCoverage = d.a;
            oldCoverage += (oldCoverage >> 7);
            RIuint32 im = 256 - coverage;
            RIuint32 ip = 256 - oldCoverage;
            newCoverage = 256 - ((im * ip) >> 8);
            break;
        }
        case VG_INTERSECT_MASK:
        {
            RIuint32 oldCoverage = d.a;
            oldCoverage += (oldCoverage >> 7);
            newCoverage = (coverage * oldCoverage) >> 8;
            break;
        }
        default:
        {
            RI_ASSERT(maskOperation == VG_SUBTRACT_MASK);
            RIuint32 im = 256 - coverage;
            RIuint32 oldCoverage = d.a;
            oldCoverage += (oldCoverage >> 7);
            newCoverage = (oldCoverage * im) >> 8;
            break;
        }
    }
    return IntegerColor(0, 0, 0, newCoverage - (newCoverage >> 8));
}

}

#endif

