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
import sys
import array
import os.path

def doit():
    # \todo There's got to be some easier way to do this...
    inputname = sys.argv[1]
    arrayname = sys.argv[2]
    f = file(inputname, 'rb')
    d = f.read()
    f.close
    sz = len(d)
    i = 0
    items = []
    for x in d:
        items += [str(ord(x))]

    justname = os.path.split(inputname)[1]
    begin_include_guard(get_include_guard_name(justname + ".h"))
    make_generation_notice()
    make_array("static const", "unsigned char", arrayname, items, empty_format, 16)
    end_include_guard()

if __name__ == '__main__':
    doit()

