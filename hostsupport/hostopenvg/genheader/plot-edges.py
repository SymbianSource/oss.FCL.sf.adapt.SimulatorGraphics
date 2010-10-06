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

import sys
import matplotlib
matplotlib.use("wxAgg")
import matplotlib.pyplot
import lines


if __name__ == '__main__':

    start = 0
    if len(sys.argv) > 2:
        start = int(sys.argv[1])

    minx = 65535.
    miny = 65535.
    maxx = -65535.
    maxy = -65535.

    z = zip(lines.lines[::2], lines.lines[1::2]) 

    if len(sys.argv) == 2:
        end = int(sys.argv[1])
    elif len(sys.argv) > 2:
        end = int(sys.argv[2])
    else:
        assert(len(sys.argv) == 1)
        end = len(z)

    print start, end

    legend = False
    if end - start < 10:
        legend = True

    for seg in z:
        px0 = seg[0][0]/128.
        py0 = seg[0][1]/8.
        px1 = seg[1][0]/128.
        py1 = seg[1][1]/8.
        minx = min(px0, minx)
        miny = min(py0, miny)
        maxx = max(px0, maxx)
        maxy = max(py0, maxy)
        minx = min(px1, minx)
        miny = min(py1, miny)
        maxx = max(px1, maxx)
        maxy = max(py1, maxy)

    currLegend = 0
    for seg in z[start:end]:
        px0 = seg[0][0]/128.
        py0 = seg[0][1]/8.
        px1 = seg[1][0]/128.
        py1 = seg[1][1]/8.

        if not legend:
            matplotlib.pyplot.plot([px0, px1], [py0, py1])
        else:
            matplotlib.pyplot.plot([px0, px1], [py0, py1], label = str(currLegend))
            currLegend += 1

    minx = min(minx, miny)
    maxx = max(maxx, maxy)
    
    #matplotlib.pyplot.plot(x, y)
    matplotlib.pyplot.axis([minx,maxx,minx,maxx])
    if legend:
        matplotlib.pyplot.legend()
    matplotlib.pyplot.show()

