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

#ifndef __RIUTILS_H_
#define __RIUTILS_H_

#ifndef __RIDEFS_H
#   include "riDefs.h"
#endif

#include "VG/openvg.h"
#include <string.h>

// This file contains "utility" functions that did not "fit" into existing RI files.
// Once more functionality is accumulated, the corresponding functions/classes should be
// moved to proper files asap. For example, the memcopy functions could go into file
// "riMemory.xxx".

namespace OpenVGRI
{

            void    riMemSet32(void* dst, RIuint32 c, size_t nElements, size_t nBytesPerElement);
RI_INLINE   void    ri_memcpy(void* dst, const void* src, size_t n);
            int     riInsertBits32(RIuint32* hash, size_t hashSize, RIuint32 bits, RIuint32 bitSize, int bitPtr);
            int     riVGImageFormatToIndex(VGImageFormat format);

RI_INLINE void ri_memcpy(void* dst, const void* src, size_t n)
{
#if !defined(RI_DEBUG)
    memcpy(dst, src, n);
#else
    RIuint8 *p = (RIuint8*)dst;
    RIuint8 *q = (RIuint8*)src;
    for (size_t i = 0; i < n; i++, p++, q++)
    {
        *p = *q;
    }
#endif
}

} // namespace OpenVG

#endif

