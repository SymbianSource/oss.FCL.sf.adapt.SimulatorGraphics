#! /usr/bin/python
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and /or associated documentation files
# (the "Materials "), to deal in the Materials without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Materials,
# and to permit persons to whom the Materials are furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Materials.
#
# THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
# THE USE OR OTHER DEALINGS IN THE MATERIALS.

from c_utils import *

# \todo Might require a more accurate table for lookups.

RCP_BITS = 8
RCP_DATATYPE_BITS = RCP_BITS + 8

def tofx(f):
    return int(f * (1<<RCP_BITS) + 0.5)

def alpha_to_uintrcp(alpha):
    if alpha == 0:
        rf = 255 # Max out
    else:
        rf = 255.0/alpha
    i = tofx(rf)
    assert(i < (1<<RCP_DATATYPE_BITS))
    return i

def test_error():
    max_err = 0
    for alpha in range(1, 256):
        for color in range(0, 256):
            if color > alpha:
                # Undefined result -> ignore test.
                continue
            fa = alpha / 255.0
            fc = color / 255.0
            ex = fc / fa
            exi = int(ex * 255.0 + 0.5)
            ia = alpha_to_uintrcp(alpha)
            m = (ia * color) >> RCP_BITS
            e = abs(exi - m)
            if e > max_err:
                print ia * color
                print alpha, color, exi, m, fa, fc, ex, ia / float(1<<RCP_BITS), ia
                max_err = e
    print max_err
    return max_err

if __name__ == '__main__':
    vals = []
    for alpha in range(0, 256):
       i = alpha_to_uintrcp(alpha)
       vals += [i]
    
    begin_include_guard("_SFALPHARCP_H_") 
    print ""
    make_include("riDefs.h")
    print ""
    begin_namespace("OpenVGRI")
    make_constant("ALPHA_RCP_BITS", RCP_BITS)
    make_array("static const", "RIuint16", "sc_alphaRcp", vals, hex_formatter)
    end_namespace()
    end_include_guard()

