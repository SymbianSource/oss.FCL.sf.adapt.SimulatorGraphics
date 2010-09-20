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
#   include "riUtils.h"
#endif

#include <string.h>

namespace OpenVGRI
{

/**
 * \brief   Sets mem areas to byte(s) in c.
 * \param   dst     Destination pointer.
 * \param   c       Data to set into dst.
 * \param   nElements   Amount of elements to set.
 * \param   nBytesPerElement    Amount of bytes for each element.
 * \note    This is moslty an image-settings support function. It is assumed that several
 *          bytes / elements can be set at once, especially in 3-byte case.
 */
void riMemSet32(void* dst, RIuint32 c, size_t nElements, size_t nBytesPerElement)
{
    // \todo This function should be called from a function that handles npot element sizes.
    // \todo Investigate the status of (open) std::fill implementations. Some of that code 
    // did not _seem_ to bundle sets or use SSE, etc.
    // \todo Use SSE instructions on Intel? 
    
    RI_ASSERT(dst);
    RI_ASSERT(nElements);

    switch(nBytesPerElement)
    {
    case 4:
    {
        RIuint32* ptr = (RIuint32*)dst;
        do {
            *ptr++ = c;
        } while(--nElements);
        break;
    }
    case 3:
    {
        // \todo Endianness.
        RIuint8* ptr = (RIuint8*)dst;
        RIuint8 b[3];
        b[0] = c & 0xff;
        b[1] = (c >> 8)&0xff;
        b[2] = (c >> 16)&0xff;
        do {
            *ptr++ = b[0];
            *ptr++ = b[1];
            *ptr++ = b[2];
        } while(--nElements);
        break;
    }
    case 2:
    {
        size_t dws = nElements / 2; 
        if (dws)
        {
            RIuint32* ptr32 = (RIuint32*)dst;
            dst = (void*)(((RIuint8*)dst + dws * 4));
            RIuint32 dw = c | (c<<16);
            do {
                *ptr32++ = dw;
            } while(--dws);
            nElements &= 0x01;
        }
        if (nElements)
        {
            RIuint16 *ptr16 = (RIuint16*)dst;
            const RIuint16 w = (RIuint16)c;
            do {
                *ptr16++ = w;
            } while(--nElements);
        }
        break;
    }
    default:
    {
        RI_ASSERT(nBytesPerElement == 1);
        memset(dst, c, nElements);
        break;
    }
    }

}

/**
 * \brief   Insert bits into an array of 32-bit integers.
 * \param   hashSize    Size of array in bytes.
 * \param   bits        Actual bits to insert.
 * \param   bitSize     Amount of bits to insert (max 32).
 * \param   bitPtr      Pointer to first bit to insert [0 .. (hashSize*8)-1]
 */
int riInsertBits32(RIuint32* hash, size_t hashSize, RIuint32 bits, RIuint32 bitSize, int bitPtr)
{
    RI_ASSERT(bitSize > 0 && bitSize <= 32);
    RI_ASSERT(bits < (1u<<bitSize));
    RI_ASSERT((bitPtr + bitSize) < (hashSize * 32));

    int idw = bitPtr >> 5;
    int ib = bitPtr - (idw << 5);

    if ((ib + bitSize) > 32)
    {
        int rb = ((ib + bitSize) - 32) + 1;
        RI_ASSERT(rb > 0);
        hash[idw] |= (bits << ib);
        hash[idw] |= (bits >> rb);
    } 
    else
    {
        int new_ib = (ib + bitSize) & 0x1f;
        RI_ASSERT((ib + bitSize == 32) ? new_ib == 0 : true);
        hash[idw] |= (bits << ib);
    }
    return bitPtr + bitSize;
}

/**
 * \brief   Convert an image format to (internal) index.
 */
int riVGImageFormatToIndex(VGImageFormat format)
{
    switch(format)
    {
    /* RGB{A,X} channel ordering */
    case VG_sRGBX_8888:
        return 0;
    case VG_sRGBA_8888:
        return 1;
    case VG_sRGBA_8888_PRE:
        return 2;
    case VG_sRGB_565:
        return 3;
    case VG_sRGBA_5551:
        return 4;
    case VG_sRGBA_4444:
        return 5;
    case VG_sL_8:
        return 6;
    case VG_lRGBX_8888:
        return 7;
    case VG_lRGBA_8888:
        return 8;
    case VG_lRGBA_8888_PRE:
        return 9;
    case VG_lL_8:
        return 10;
    case VG_A_8:
        return 11;
    case VG_BW_1:
        return 12;
    case VG_A_1:
        return 13;
    case VG_A_4:
        return 14;

    /* {A,X}RGB channel ordering */
    case VG_sXRGB_8888:
        return 15;
    case VG_sARGB_8888:
        return 16;
    case VG_sARGB_8888_PRE:
        return 17;
    case VG_sARGB_1555:
        return 18;
    case VG_sARGB_4444:
        return 19;
    case VG_lXRGB_8888:
        return 20;
    case VG_lARGB_8888:
        return 21;
    case VG_lARGB_8888_PRE:
        return 22;

    /* BGR{A,X} channel ordering */
    case VG_sBGRX_8888:
        return 23;
    case VG_sBGRA_8888:
        return 24;
    case VG_sBGRA_8888_PRE:
        return 25;
    case VG_sBGR_565:
        return 26;
    case VG_sBGRA_5551:
        return 27;
    case VG_sBGRA_4444:
        return 28;
    case VG_lBGRX_8888:
        return 29;
    case VG_lBGRA_8888:
        return 30;
    case VG_lBGRA_8888_PRE:
        return 31;

    /* {A,X}BGR channel ordering */
    case VG_sXBGR_8888:
        return 32;
    case VG_sABGR_8888:
        return 33;
    case VG_sABGR_8888_PRE:
        return 34;
    case VG_sABGR_1555:
        return 35;
    case VG_sABGR_4444:
        return 36;
    case VG_lXBGR_8888:
        return 37;
    case VG_lABGR_8888:
        return 38;
    default:
        RI_ASSERT(format == VG_lABGR_8888_PRE);
        return 39;
    }
}

}

