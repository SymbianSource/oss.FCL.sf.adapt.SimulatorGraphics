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

#ifndef _EGLTHREAD_H_
#define _EGLTHREAD_H_

#include <EGL/egl.h>
#include "eglInternal.h"

class CEGLSurface;
class CEGLContext;

class CEGLThread
{
public:
	CEGLThread( EGLI_THREAD_ID threadId, EGLint supportedApis );
	~CEGLThread(void);

public:
    inline EGLI_THREAD_ID Id() const { return m_id; }
    void SetError( EGLint error ) { m_lastError = error; }
    inline EGLint Error() const { return m_lastError; }

    inline void SetApi( EGLint api ) { m_currentApi = api; }
    inline EGLenum CurrentApi() const { return m_currentApi; }
    bool IsSupportedApi( EGLenum api ) const;

    CEGLContext* SwitchVGContext( CEGLContext* vgContext );
    inline CEGLContext* CurrentVGContext() const { return m_currentVGContext; }
    CEGLSurface* SwitchVGSurface( CEGLSurface* vgSurface );
    inline CEGLSurface* CurrentVGSurface() const { return m_currentVGSurface; }

	CEGLContext* SwitchGLESContext( CEGLContext* glesContext );
	inline CEGLContext* CurrentGLESContext() const { return m_currentGLESContext; }
	void SwitchGLESSurfaces( CEGLSurface* read, CEGLSurface* draw, CEGLSurface*& previousRead, CEGLSurface*& previousDraw );
    void CurrentGLESSurfaces( CEGLSurface** read, CEGLSurface** draw ) const;

private:
    EGLI_THREAD_ID  m_id;
	CEGLContext*    m_currentVGContext;
    CEGLSurface*    m_currentVGSurface;
    CEGLContext*    m_currentGLESContext;
	CEGLSurface*    m_currentGLESReadSurface;
    CEGLSurface*    m_currentGLESDrawSurface;
    EGLenum         m_currentApi;
    EGLint          m_lastError;
    // Copy of CEGLState::m_supportedApis so we don't need to access state
    // on thread specific API calls.
    EGLint          m_supportedApis; 
};
#endif //_EGLTHREAD_H_