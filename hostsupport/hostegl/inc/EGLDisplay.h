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

#ifndef _EGLDISPLAY_H_
#define _EGLDISPLAY_H_

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "eglInternal.h"
#include "EGLSurface.h"


//FORWARD DECLARATIONS
class CEGLConfig;
class CEGLContext;
class CEGLImage;

class CEGLDisplay : public RefCountingObject
{
public:
	CEGLDisplay( EGLINativeDisplayType nativeType, EGLint processId );
	~CEGLDisplay(void);

public:
    CEGLContext* AddContext( CEGLContext* context );
    void RemoveContext( EGLContext context );
    CEGLContext* GetContext( EGLContext context ) const;

    CEGLSurface* AddSurface( CEGLSurface* surface );
    void RemoveSurface( EGLSurface surfaceId );
    CEGLSurface* GetSurface( EGLSurface surfaceId ) const;
    CEGLSurface* GetSurfaceByNativeType( EGLINativeWindowType nativeType ) const;
    CEGLSurface* FindSurfaceByClientSurface( void* clientSurface, EGLenum api, EGLint apiVersion  ) const;
    CEGLSurface* FindSurfaceByClientBuffer( void* clientBuffer ) const;

    inline EGLint ProcessId() const { return m_processId; }

    EGLINativeDisplayType NativeType() const;

    EGLBoolean Initialize();
    bool TerminateDisplay();
    inline EGLBoolean IsInitialized() const { return m_initialized; }
    CEGLImage* AddEGLImage( CEGLImage* image );
    void RemoveEGLImage( EGLImageKHR image );
    CEGLImage* GetImage( EGLImageKHR image ) const;

private:
    EGLINativeDisplayType       m_nativeType;
    EGLBoolean                  m_initialized;
    std::vector<CEGLContext*>   m_contexts;
    std::vector<CEGLSurface*>   m_surfaces;
    std::vector<CEGLImage*>     m_images;
    EGLint                      m_processId;
};
#endif //_EGLDISPLAY_H_