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

# Extremely coarse lrgb<->srgb conversion table generation

from c_utils import *

GAMMA_BITS = 8

def linear_to_gamma(f):
    if f <= 0.00304:
        return 12.92 * f
    return 1.0556*pow(f, 1/2.4) - 0.0556

def gamma_to_linear(f):
    if f <= 0.03928:
        return f / 12.92
    return pow((f+0.0556)/1.0556, 2.4)

def clamp(i, mn, mx):
    if i < mn:
        return mn
    if i > mx:
        return mx
    return i

def icolortof(i):
    return i / 255.0

def fcolortoi(f):
    return int(f * ((1<<GAMMA_BITS)-1) + 0.5)

all_components = range(0,256)
all_floats = map(icolortof, all_components)

def make_color_list(conv_func):
    return map(fcolortoi, map(conv_func, all_floats))

def make_lut_array(name, data):
    make_array("static const", "RIuint8", name, data, hex_formatter)  

def print_lrgb_to_srgb_table():
    make_lut_array("sc_lRGB_to_sRGB", make_color_list(linear_to_gamma))

def print_srgb_to_lrgb_table():
    make_lut_array("sc_sRGB_to_lRGB", make_color_list(gamma_to_linear))

def print_lookups():
    begin_include_guard("_SFGAMMARCP_H_") 
    print ""
    make_include("riDefs.h")
    print ""
    begin_namespace("OpenVGRI")
    print_srgb_to_lrgb_table()
    print ""
    print_lrgb_to_srgb_table()
    end_namespace()
    end_include_guard()

if __name__ == '__main__':
    print_lookups()

