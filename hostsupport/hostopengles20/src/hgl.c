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

#include "common.h"
#include "hgl.h"
#include "degl.h"

// Load the host renderer OpenGL functions.
int hglLoad(HGL* hgl)
{
    Dprintf("Loading GL functions...\n", 0);

#if(CONFIG_STATIC == 1)
#	define HGL_FUNC(ret, name, attr) \
        if(degl_handle == (void*)-1) \
        { \
            extern ret GL_APIENTRY mgl##name attr; \
            hgl->name = &mgl##name; \
        } \
        else if((hgl->name = deglGetHostProcAddress("gl" #name)) == NULL) \
        { \
            fprintf(stderr, "Function gl" #name " not found!\n"); \
        }
#else
#	define HGL_FUNC(ret, name, attr) \
        { \
        union {void *ptr; ret(GL_APIENTRY* fptr)attr;} ass; \
        ass.ptr = deglGetHostProcAddress("gl" #name); \
        if((hgl->name = ass.fptr) == NULL) \
        { \
            fprintf(stderr, "Function gl" #name " not found!\n"); \
        }\
        }
#endif // CONFIG_STATIC != 1

    HGL_FUNCS

#undef HGL_FUNC

    return 1;
}
