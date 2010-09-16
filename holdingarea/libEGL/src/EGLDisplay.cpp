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

#include "EGLDisplay.h"
#include "EGLContext.h"
#include "EGLConfig.h"
#include "EGLSurface.h"
#include "EGLImage.h"
#include "EGLWindowSurface.h"
#include "EGLPbufferSurface.h"
#include "ColorDescriptor.h"
#include "EGLOs.h"
#include "EGLUtils.h"
#include "EGLState.h"
#include "EGLProcess.h"
#include "EGLThread.h"


CEGLDisplay::CEGLDisplay( EGLINativeDisplayType nativeType, EGLint processId ) :
    m_nativeType( nativeType ),
    m_initialized( EGL_FALSE ),
    m_processId( processId )
    {
    }

CEGLDisplay::~CEGLDisplay(void)
    {
    DestroyPointerVector<CEGLContext>( m_contexts );
    DestroyPointerVector<CEGLSurface>( m_surfaces );
    DestroyPointerVector<CEGLImage>( m_images );
    }

CEGLContext* CEGLDisplay::AddContext( CEGLContext* context )
    {
    if( context )
        {
        AddObject<CEGLContext>( m_contexts, context );
        }
    return context;
    }

void CEGLDisplay::RemoveContext( EGLContext context )
    {
    DeleteObjectByPointer<CEGLContext>( m_contexts, context );
    }

CEGLContext* CEGLDisplay::GetContext( EGLContext context ) const
    {
    CEGLContext* ret = FindObjectByPointer<CEGLContext>( m_contexts, context, NULL );
    if( ret && ret->IsTerminated() ) ret = NULL;
    return ret;
    }

CEGLSurface* CEGLDisplay::AddSurface( CEGLSurface* surface )
    {
    if( surface )
        {
        AddObject<CEGLSurface>( m_surfaces, surface );
        }
    return surface;
    }

void CEGLDisplay::RemoveSurface( EGLSurface surfaceId )
    {
    DeleteObjectByPointer<CEGLSurface>( m_surfaces, surfaceId );
    }

CEGLSurface* CEGLDisplay::GetSurface( EGLSurface surfaceId ) const
    {
    CEGLSurface* ret = FindObjectByPointer<CEGLSurface>( m_surfaces, surfaceId, NULL );
    if( ret && ret->IsTerminated() ) ret = NULL;
    return ret;
    }

CEGLSurface* CEGLDisplay::GetSurfaceByNativeType( EGLINativeWindowType nativeType ) const
    {
    CEGLSurface* ret = NULL;
    std::vector<CEGLSurface*>::const_iterator iter = m_surfaces.begin();
    while( iter != m_surfaces.end() )
        {
        if( (*iter)->Type() == CEGLSurface::WINDOW_SURFACE )
            {
            if( ((CEGLWindowSurface*)(*iter))->NativeType() == nativeType  && !((*iter)->IsTerminated()) )
                {
                ret = (*iter);
                break;
                }
            }
        iter++;
        }
    return ret;
    }

CEGLSurface* CEGLDisplay::FindSurfaceByClientSurface( void* clientSurface, EGLenum api, EGLint apiVersion ) const
    {
    CEGLSurface* ret = NULL;
    std::vector<CEGLSurface*>::const_iterator iter = m_surfaces.begin();
    while( iter != m_surfaces.end() )
        {
        switch( api )
            {
            case EGL_OPENVG_API:
                {
                if( (*iter)->VGClientSurface() == clientSurface )
                    {
                    ret = (*iter);
                    break;
                    }
                }
            case EGL_OPENGL_ES_API:
                {
                if( (*iter)->GLESClientSurface() == clientSurface )
                    {
                    ret = (*iter);
                    break;
                    }
                }
            }
        iter++;
        }
    return ret;
    }

CEGLSurface* CEGLDisplay::FindSurfaceByClientBuffer( void* clientBuffer ) const
    {
    CEGLSurface* ret = NULL;
    std::vector<CEGLSurface*>::const_iterator iter = m_surfaces.begin();
    while( iter != m_surfaces.end() )
        {
        if( (*iter)->Type() == CEGLSurface::PBUFFER_SURFACE &&
            ((CEGLPbufferSurface*)(*iter))->ClientBuffer() == clientBuffer )
            {
            ret = (*iter);
            break;
            }
        iter++;
        }
    return ret;
    }

EGLINativeDisplayType CEGLDisplay::NativeType() const
    {
#if defined(EGLI_USE_PLATSIM_EXTENSIONS)
    return m_nativeType; 
#else
    return m_nativeType; 
#endif
    }

EGLBoolean CEGLDisplay::Initialize()
    {
    m_initialized = EGL_TRUE;
    return m_initialized;
    }
	
bool CEGLDisplay::TerminateDisplay()
    {
    //m_initialized = EGL_FALSE;
    bool ret = true;
    std::vector<CEGLSurface*> boundSurfaces;
    std::vector<CEGLSurface*>::iterator sIter = m_surfaces.begin();
    CEGLState* state = getState();
    CEGLThread* thread = state->GetCurrentProcess()->CurrentThread();
    while( sIter != m_surfaces.end() )
        {
        if( (*sIter)->IsTerminated() || !((*sIter)->RemoveRef()) )
            {
            (*sIter)->Terminate();
            boundSurfaces.push_back( (*sIter) );
            sIter = m_surfaces.erase( sIter );
            }
        else
            {
            sIter++;
            }
        }
    DestroyPointerVector<CEGLSurface>(m_surfaces);
    m_surfaces.swap( boundSurfaces );
    boundSurfaces.clear();

    std::vector<CEGLContext*> boundContexts;
    std::vector<CEGLContext*>::iterator cIter = m_contexts.begin();
    while( cIter != m_contexts.end() )
        {
        if( (*cIter)->IsTerminated() || !((*cIter)->RemoveRef()) )
            {
            (*cIter)->Terminate();
            boundContexts.push_back( (*cIter) );
            cIter = m_contexts.erase( cIter );
            }
        else
            {
            cIter++;
            }
        }
    DestroyPointerVector<CEGLContext>(m_contexts);
    m_contexts.swap( boundContexts );
    ret = (m_contexts.size() <= 0 && m_surfaces.size() <= 0);
    Terminate();
    return ret;
    //TODO
    //EGLI_ASSERT( false );
    }

CEGLImage* CEGLDisplay::AddEGLImage( CEGLImage* image )
    {
    if( image )
        {
        AddObject<CEGLImage>( m_images, image );
        }
    return image;
    }

void CEGLDisplay::RemoveEGLImage( EGLImageKHR image )
    {
    DeleteObjectByPointer<CEGLImage>( m_images, image );
    }

CEGLImage* CEGLDisplay::GetImage( EGLImageKHR image ) const
    {
    CEGLImage* ret = FindObjectByPointer<CEGLImage>( m_images, image, NULL );
    return ret;
    }
