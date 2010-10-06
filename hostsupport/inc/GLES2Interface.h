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
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#ifndef _GLES2INTERFACE_H_
#define _GLES2INTERFACE_H_

#include "GLESInterface.h"
#include "SurfaceDescriptor.h"

enum EImageTarget;

enum EStatus {
	DGLES2_SUCCESS,
	DGLES2_BAD_PARAMETER,
	DGLES2_BAD_MATCH
};

class CColorDescriptor;

class IEGLtoGLES2Interface : public IEGLtoGLESInterface
    {
public:
    virtual EStatus GetTextureInfo( void* context, EImageTarget target, void* texture, int textureLevel, SurfaceDescriptor& desc ) = 0;
    virtual EStatus GetTextureData( void* context, EImageTarget target, void* texture, int textureLevel, void* data ) = 0;
	virtual bool UpdateImageSibling( EImageTarget target, void* name ) = 0;
    };

typedef IEGLtoGLES2Interface* (*fpGetGLES2Interface)(void);
IEGLtoGLES2Interface* LoadGLES2Interface( void*& libraryHandle );

#endif // _GLES2INTERFACE_H_