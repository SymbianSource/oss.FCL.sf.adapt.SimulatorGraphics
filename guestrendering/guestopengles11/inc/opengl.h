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
// Header file for the Guest OpenGL ES 1.1 serialization stubs

#ifndef __GUESTOPENGLES11_OPENGL_H__
#define __GUESTOPENGLES11_OPENGL_H__

#include <e32debug.h>

// set trace level here
#ifndef _OPENGL_TRACE_LEVEL_
#define _OPENGL_TRACE_LEVEL_ ( _APICALL_ )
#endif

// trace levels (in udeb mode)

#define _NONE_      0x0000      // no traces in udeb mode
#define _GEN_       (1 << 0)    // general traces (default)
#define _APICALL_   (1 << 1)    // API call traces (function entry and exit)
#define _DETAIL_    (1 << 2)    // detailed traces
#define _ALL_       0xFFFF


#ifdef _DEBUG
    #define OPENGL_TRACE(level, fmt, args...)   \
    {                                           \
        if(level & _OPENGL_TRACE_LEVEL_)        \
        {                                       \
            RDebug::Printf(fmt, ##args);        \
        }                                       \
    }
	#define GLPANIC_ASSERT(condition, panic) if (!(condition)) { GlesPanic(panic, #condition, __FILE__, __LINE__); }
	#define GLPANIC_ASSERT_DEBUG(condition, panic) if (!(condition)) { GlesPanic(panic, #condition, __FILE__, __LINE__); }
#else
    #define OPENGL_TRACE(level, fmt, args...)
	#define GLPANIC_ASSERT(condition, panic) if (!(condition)) { GlesPanic(panic, NULL, NULL, __LINE__); }
	#define GLPANIC_ASSERT_DEBUG(condition, panic)
#endif

// Guest Open VG panic codes
typedef enum
	{
	EGlPanicNotReplyOpcode = 1,
	} TGlPanic;

void GlesPanic(TGlPanic aPanicCode, char* aCondition, char* aFile, TInt aLine);


/* String constants */
#define OGL_VENDOR_STRING        "Nokia"
#define OGL_RENDERER_STRING      "1.1.0"
#define OGL_VERSION_STRING       "OpenGL ES-CM 1.1"
#define OGL_EXTENSION_STRING     "GL_OES_byte_coordinates GL_OES_fixed_point GL_OES_single_precision "               \
                                 "GL_OES_read_format GL_OES_query_matrix GL_OES_compressed_paletted_texture "        \
                                 "GL_OES_matrix_palette GL_OES_point_sprite GL_OES_point_size_array "                \
                                 "GL_OES_draw_texture GL_OES_matrix_get GL_OES_swap_control GL_OES_render_texture"

static const char* const g_strings[] =
{
    OGL_VENDOR_STRING,
    OGL_RENDERER_STRING,
    OGL_VERSION_STRING,
    OGL_EXTENSION_STRING
};

/* vertex array pointers */
enum
{
    COLOR_ARRAY,
    NORMAL_ARRAY,
    POINT_SIZE_ARRAY,
    TEX_COORD_ARRAY,
    VERTEX_ARRAY,
    MATRIX_INDEX_ARRAY,
    WEIGHT_ARRAY,
    NUM_ARRAYS
};

typedef struct
{
    GLint size;
    GLenum type;
    GLsizei stride;
    const GLvoid *pointer;
} vertexArrayInfo;


NONSHARABLE_CLASS(TGles11ApiForEgl) : public MGles11ApiForEgl
	{
public:
	TGles11ApiForEgl(); // constructor used to publish the MGles11ApiForEgl vtable
	~TGles11ApiForEgl(); // destructor used to unpublish the MGles11ApiForEgl vtable
	virtual ExtensionProcPointer guestGetGles11ProcAddress (const char *aProcName);
	};

#endif // __GUESTOPENGLES11_OPENGL_H__
