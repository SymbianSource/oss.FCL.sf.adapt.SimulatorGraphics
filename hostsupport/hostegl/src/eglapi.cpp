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

#include <string.h>
#include <algorithm>
#include <EGL/egl.h>
#include "eglDefinitions.h"
#include "eglInternal.h"
#include "EGLState.h"
#include "EGLProcess.h"
#include "EGLDisplay.h"
#include "EGLThread.h"
#include "EGLContext.h"
#include "EGLConfig.h"
#include "EGLWindowSurface.h"
#include "EGLPbufferSurface.h"
#include "ColorDescriptor.h"
#include "SurfaceDescriptor.h"
#include "EGLExtension.h"
#include "EGLOs.h"

#if defined(_WIN32)
#include <windows.h>
#else
    // \todo other platforms
#endif

#if defined(_WIN32) && !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
static DWORD g_tlsIndex = TLS_OUT_OF_INDEXES;
#endif

static CEGLState* g_eglState = NULL;
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
static bool g_lockInitialized = false;
#endif
EGLI_LOCK g_eglLock;

CEGLState* getState()
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if( !g_eglState )
        {
        g_eglState = EGLI_NEW CEGLState();
        if( !g_eglState )
            {
            return NULL;
            }
        if( !(g_eglState->Initialize()) )
            {
            delete g_eglState;
            g_eglState = NULL;
            return NULL;
            }
        }
    return g_eglState;
#elif defined(_WIN32)
    CEGLOs::GetLock( &g_eglLock );
    if( !g_eglState )
        {
        g_eglState = EGLI_NEW CEGLState();
        if( !g_eglState )
            {
            CEGLOs::ReleaseLock( &g_eglLock );
            return NULL;
            }
        if( !(g_eglState->Initialize()) )
            {
            delete g_eglState;
            g_eglState = NULL;
            CEGLOs::ReleaseLock( &g_eglLock );
            return NULL;
            }
        }
    EGLI_ASSERT( g_tlsIndex != TLS_OUT_OF_INDEXES );
    if( g_tlsIndex == TLS_OUT_OF_INDEXES )
        {
        CEGLOs::ReleaseLock( &g_eglLock );
        return NULL;
        }
    CEGLThread* thread = (CEGLThread*)TlsGetValue( g_tlsIndex );
    if( g_eglState && !thread )
        {
        // \todo Remove CEGLProcess when Simulator extensions are not needed any more
        thread = g_eglState->GetCurrentProcess()->AddThread( GetCurrentThreadId(), true, g_eglState->SupportedApis() );
        if( !thread ) 
            {
            CEGLOs::ReleaseLock( &g_eglLock );
            return NULL;
            }
        if( !(TlsSetValue( g_tlsIndex, (void *)thread )) )
            {
            CEGLOs::ReleaseLock( &g_eglLock );
            return NULL;
            }
        g_eglState->AddRef();
        }
    CEGLOs::ReleaseLock( &g_eglLock );
    return g_eglState;
#else // Linux
    // \todo
    return NULL;
#endif
    }

static void releaseState()
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if( g_eglState )
        {
        if( g_eglState->RemoveRef() )
            {
            delete g_eglState;
            g_eglState = NULL;
            }
        }
#elif defined(_WIN32)
    CEGLOs::GetLock( &g_eglLock );
    if( g_eglState )
        {
        g_eglState->GetCurrentProcess()->RemoveThread( GetCurrentThreadId() );
        EGLI_ASSERT( g_tlsIndex != TLS_OUT_OF_INDEXES );
        if( g_tlsIndex != TLS_OUT_OF_INDEXES )
            {
            TlsSetValue( g_tlsIndex, 0 );
            }
        }
    if( g_eglState && g_eglState->RemoveRef() )
        {
        delete g_eglState;
        g_eglState = NULL;
        }
    CEGLOs::ReleaseLock( &g_eglLock );
#else // Linux
    // \todo
    EGLI_ASSERT( false );
#endif
    }

/*static*/ CEGLThread* getThread()
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    CEGLState* state = getState();
    CEGLProcess* process = state->GetCurrentProcess();
    if( process )
        {
        return process->CurrentThread();
        }
    return NULL;
#elif defined(_WIN32)
    EGLI_ASSERT( g_tlsIndex != TLS_OUT_OF_INDEXES );
    if( g_tlsIndex == TLS_OUT_OF_INDEXES )
        return NULL;
    CEGLThread* thread = (CEGLThread*)TlsGetValue( g_tlsIndex );
    if( !thread && GetLastError() == ERROR_SUCCESS )
        {
        // TlsGetValue() succeeded but didn't have thread => create one now
        CEGLState* state = getState();
        if( state ) 
            {
            thread = (CEGLThread*)TlsGetValue( g_tlsIndex );
            }
        }
    return thread;
#else // Linux
    // \todo
    EGLI_ASSERT( false );
#endif
    } 

void setEGLError( EGLint error )
    {
    CEGLThread* thread = getThread();
    if( thread )
        {
        thread->SetError( error );
        }
    }

#define EGLI_GET_DISPLAY_RET(id,ret)\
    CEGLDisplay* display = state->GetDisplay( id );\
    if( !display )\
        {\
        EGLI_LEAVE_RET( ret, EGL_BAD_DISPLAY );\
        }\
    EGLI_ASSERT( display->ProcessId() == process->Id() );\
    if( !(display->IsInitialized()) )\
        {\
        EGLI_LEAVE_RET( ret, EGL_NOT_INITIALIZED );\
        }

#ifdef __cplusplus
extern "C" {
#endif
/*When there is no status to return (in other words, when eglGetError is called
as the first EGL call in a thread, or immediately after calling eglReleaseThread),
EGL_SUCCESS will be returned.*/
EGLAPI EGLint EGLAPIENTRY eglGetError(void)
    {
    EGLint err = EGL_SUCCESS;
    CEGLThread* thread = getThread();
    if( thread )
        {
        err = thread->Error();
        thread->SetError( EGL_SUCCESS );
        }
    else
        {
        err = EGL_BAD_ALLOC;
        }
    return err;
    }

EGLAPI EGLDisplay EGLAPIENTRY eglGetDisplay(EGLNativeDisplayType display_id)
    {
    EGLI_ENTER_RET( EGL_NO_DISPLAY );

	if( display_id == EGL_DEFAULT_DISPLAY )
		{
		display_id = (EGLNativeDisplayType)state->DefaultDisplay();
		}

    CEGLDisplay* display = state->GetDisplayByNativeType( (EGLINativeDisplayType)display_id, process->Id() );
    if( !display )
        {
        if( !(CEGLOs::IsValidNativeDisplay( (EGLINativeDisplayType)display_id )) )
            {
            EGLI_LEAVE_RET( EGL_NO_DISPLAY, EGL_BAD_PARAMETER );
            }
        display = state->AddDisplay( (EGLINativeDisplayType)display_id, process->Id() );
        if( !display )
            {
            EGLI_LEAVE_RET( EGL_NO_DISPLAY, EGL_BAD_ALLOC );
            }
		display->AddRef();
        }
    EGLI_LEAVE_RET( (EGLDisplay)display, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
    {
    EGLI_ENTER_RET( EGL_FALSE );

    CEGLDisplay* display = state->GetDisplay( dpy );
    if( !display )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_DISPLAY );
        }

    if( !(display->IsInitialized()) && !(display->Initialize()) )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_NOT_INITIALIZED );
        }

    if( major ) *major = 1;
    if( minor ) *minor = 4;

    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglTerminate(EGLDisplay dpy)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    CEGLDisplay* display = state->GetDisplay( dpy );
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if( display && display->ProcessId() != process->Id() )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_DISPLAY );
        }
#endif
    if( !display )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_DISPLAY );
        }

    if( !(display->IsInitialized()) )
        {
        EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
        }
    display->RemoveRef();
    if( display->TerminateDisplay() )
        {
        state->RemoveDisplay( display );
        }

    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

/*
    eglQueryString returns a pointer to a static, zero-terminated string describing
    some aspect of the EGL implementation running on the specified display. name
    may be one of EGL_CLIENT_APIS, EGL_EXTENSIONS, EGL_VENDOR, or EGL_-
    VERSION.
    The EGL_CLIENT_APIS string describes which client rendering APIs are
    supported. It is zero-terminated and contains a space-separated list of API
    names, which must include at least one of ‘‘OpenGL’’, ‘‘OpenGL_ES’’ or
    ‘‘OpenVG’’.
    The EGL_EXTENSIONS string describes which EGL extensions are supported
    by the EGL implementation running on the specified display. The string is zeroterminated
    and contains a space-separated list of extension names; extension names
    themselves do not contain spaces. If there are no extensions to EGL, then the empty
    string is returned.
    The format and contents of the EGL_VENDOR string is implementation dependent.
    The format of the EGL_VERSION string is:
    <major version.minor version><space><vendor specific info>
*/

EGLAPI const char * EGLAPIENTRY eglQueryString(EGLDisplay dpy, EGLint name)
    {
    EGLI_ENTER_RET( NULL );
    EGLI_GET_DISPLAY_RET( dpy, NULL );

    const char* ret = NULL;
    switch(name)
        {
        case EGL_CLIENT_APIS:
            {
            switch( state->SupportedApis() )
                {
                case EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT:
                    {
                    ret = "OpenVG OpenGL_ES OpenGL_ES2";
                    break;
                    }

                case EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT:
                    {
                    ret = "OpenVG OpenGL_ES";
                    break;
                    }

                case EGL_OPENVG_BIT | EGL_OPENGL_ES2_BIT:
                    {
                    ret = "OpenVG OpenGL_ES2";
                    break;
                    }

                case EGL_OPENVG_BIT:
                    {
                    ret = "OpenVG";
                    break;
                    }

                case EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT:
                    {
                    ret = "OpenGL_ES OpenGL_ES2";
                    break;
                    }

                case EGL_OPENGL_ES_BIT:
                    {
                    ret = "OpenGL_ES";
                    break;
                    }

                case EGL_OPENGL_ES2_BIT:
                    {
                    ret = "OpenGL_ES2";
                    break;
                    }

                default:
                    {
                    ret = "";
                    break;
                    }
                }
            break;
            }
        case EGL_EXTENSIONS:
            {
            // TODO
            ret = "EGL_KHR_image_base";
            break;
            }
        case EGL_VENDOR:
            {
            ret = "Nokia";
            break;
            }
        case EGL_VERSION:
            {
            ret ="1.4 Simulator";
            break;
            }

        default:
            {
            EGLI_LEAVE_RET( NULL, EGL_BAD_PARAMETER );
            break;
            }
        }
    EGLI_LEAVE_RET( ret, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigs(EGLDisplay dpy, EGLConfig *configs,
			 EGLint config_size, EGLint *num_config)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    if( !num_config )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_PARAMETER );
        }
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );

    EGLint configCount = state->ConfigCount();
    if( !configs )
        {
        *num_config = configCount;
        EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
        }

    *num_config = EGLI_MIN( configCount, config_size );

    state->FillConfigs( configs, *num_config );

    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list,
			   EGLConfig *configs, EGLint config_size,
			   EGLint *num_config)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );

    CColorDescriptor* colorDesc = EGLI_NEW CColorDescriptor( 0, 0, 0, 0, true, 0, false, 0, 0 );
    if( !colorDesc )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ALLOC );
        }
    colorDesc->SetAttribute( EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER );
    CEGLConfig* filter = EGLI_NEW CEGLConfig(
                    *colorDesc,
                    (EGLBoolean)EGL_DONT_CARE,  
                    (EGLBoolean)EGL_DONT_CARE,
                    (EGLenum)EGL_DONT_CARE,
                    EGL_DONT_CARE,
                    0, 0, 0,
                    EGL_DONT_CARE,
                    EGL_DONT_CARE,
                    (EGLBoolean)EGL_DONT_CARE,
                    EGL_DONT_CARE,
                    EGL_OPENGL_ES_BIT,
                    0, 0, 0,
                    EGL_WINDOW_BIT,
                    NULL );          // transparent color    
    delete colorDesc;
    colorDesc = NULL;
    if( !filter )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ALLOC );
        }

    EGLint configId = 0;
    EGLint matchPixmap = EGL_NONE;

    if( attrib_list && attrib_list[0] != EGL_NONE )
        {
        for( EGLint i=0; attrib_list[i] != EGL_NONE && configId == 0; i+=2 )
            {
            switch( attrib_list[i] )
                {
                case EGL_NATIVE_VISUAL_ID:
                case EGL_MAX_PBUFFER_HEIGHT:
                case EGL_MAX_PBUFFER_PIXELS:
                case EGL_MAX_PBUFFER_WIDTH:
                    {
                    // IGNORED
                    break;
                    }

                case EGL_CONFIG_ID:
                    {
                    configId = attrib_list[i+1];
                    break;
                    }
                    
                case EGL_MATCH_NATIVE_PIXMAP:
                    {
                    matchPixmap = attrib_list[i+1];
                    break;
                    }
                default:
                    {
                    if( !(filter->SetAttribute(attrib_list[i], attrib_list[i+1])) )
                        {
                        delete filter;
                        filter = NULL;
                        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ATTRIBUTE );
                        }
                    }
                }
            }
        }

    if( configId > 0 )
        {
        CEGLConfig* config = state->FindConfigById( configId );
        if( !config )
            {
            delete filter;
            filter = NULL;
            // TODO: is this correct error code? 
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ALLOC ); 
            }
        *num_config = 1;
        configs[0] = (EGLConfig)config;
        delete filter;
        filter = NULL;
        EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
        }

    if( matchPixmap != EGL_NONE )
        {
        // eglChooseConfig – only pbuffer supported, so no need for EGL_MATCH_NATIVE_PIXMAP
        }
    
    *num_config = state->MatchConfigs( filter, configs, config_size );
    delete filter;
    filter = NULL;
    
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigAttrib(EGLDisplay dpy, EGLConfig configId,
			      EGLint attribute, EGLint *value)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );
    if( !value )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_PARAMETER );
        }
    CEGLConfig* config = state->GetConfig( configId );
    if( !config )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_CONFIG );
        }
    EGLint val = config->GetAttribute( attribute );
    if( val == EGL_BAD_ATTRIBUTE )
        {
        EGLI_LEAVE_RET( EGL_FALSE, val );
        }
    *value = val;
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }


EGLAPI EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig configId,
				  EGLNativeWindowType win,
				  const EGLint *attrib_list)
    {
    EGLI_ENTER_RET( EGL_NO_SURFACE );
    EGLI_GET_DISPLAY_RET ( dpy, EGL_NO_SURFACE );
    // \todo eglCreateWindowSurface – EGL_BAD_MATCH raised (as no window-supporting configs)
    CEGLConfig* config = state->GetConfig( configId );
    if( !config )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_CONFIG );
        }
    EGLint surfaceType = config->GetAttribute( EGL_SURFACE_TYPE );
    if( !(surfaceType & EGL_WINDOW_BIT) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_MATCH );
        }
    CEGLSurface* surface = display->GetSurfaceByNativeType( (EGLINativeWindowType)win );
    if( surface && surface->Config() == config )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }

	EGLint renderBuffer = EGL_BACK_BUFFER;
	EGLint colorSpace = EGL_VG_COLORSPACE_sRGB;
	EGLint alphaFormat = EGL_VG_ALPHA_FORMAT_NONPRE;
    EGLint renderableType = config->GetAttribute( EGL_RENDERABLE_TYPE );
	if( attrib_list && attrib_list[0] != EGL_NONE )
	    {
		for( int i=0; attrib_list[i] != EGL_NONE; i+=2 )
		    {
			switch( attrib_list[i] )
			    {
			    case EGL_RENDER_BUFFER:
				    renderBuffer = attrib_list[i+1];
				    break;

			    case EGL_VG_COLORSPACE:
                    if( !(renderableType & EGL_OPENVG_BIT) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
				    colorSpace = attrib_list[i+1];
				    break;

			    case EGL_VG_ALPHA_FORMAT:
                    if( !(renderableType & EGL_OPENVG_BIT) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
				    alphaFormat = attrib_list[i+1];
				    break;

			    default:
				    EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                }
            }
        }
	
    if( !(surfaceType & colorSpace) || !(surfaceType & alphaFormat) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_MATCH );
        }
    
    if( !CEGLOs::IsValidNativeWindow( win ) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_NATIVE_WINDOW );
        }

    EGLIOsWindowContext* osContext = CEGLOs::CreateOSWindowContext( win, *config );
    if( !osContext )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
    
    surface = EGLI_NEW CEGLWindowSurface( config, colorSpace, alphaFormat, renderBuffer, osContext );
    if( !surface )
        {
        CEGLOs::DestroyOSWindowContext( osContext );
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
    // \note AddSurface() will destroy the object if it fails to take ownership
    surface = display->AddSurface( surface );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    int width = 0;
    int height = 0;
    if( !CEGLOs::GetNativeWindowSize( win, width, height ) ||
        !(surface->Resize( width, height )) )
        {
        // \note RemoveSurface() will destroy the object
        display->RemoveSurface( surface );
        // \todo Is this correct error?
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
    if( (state->SupportedApis() & EGL_OPENVG_BIT) && (renderableType & EGL_OPENVG_BIT) )
        {
        SurfaceDescriptor* surfaceDesc = surface->Descriptor();
        if( !(state->VGInterface()->CreateSurface( surfaceDesc, surface->VGBuffers(), NULL )) )
            {
            // \note RemoveSurface() will destroy the object
            display->RemoveSurface( surface );
            EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
            }
        }
#endif // EGLI_USE_SIMULATOR_EXTENSIONS

    surface->AddRef();

    EGLI_LEAVE_RET( (EGLSurface)surface, EGL_SUCCESS );
    }

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig configId,
				   const EGLint *attrib_list)
    {
    // \todo combine with eglCreatePbufferFromClientBuffer()
    EGLI_ENTER_RET( EGL_NO_SURFACE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_NO_SURFACE );
    CEGLConfig* config = state->GetConfig( configId );
    if( !config )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_CONFIG );
        }

    if( !(config->GetAttribute( EGL_SURFACE_TYPE ) & EGL_PBUFFER_BIT) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_MATCH );
        }
    
    EGLint width = 0;
    EGLint height = 0;
    EGLint textureFormat = EGL_NO_TEXTURE;
    EGLint textureTarget = EGL_NO_TEXTURE;
    EGLint mipmapTexture = EGL_FALSE;
    EGLint largestPbuffer = EGL_FALSE;
    EGLint colorSpace = EGL_VG_COLORSPACE_sRGB;
    EGLint alphaFormat = EGL_VG_ALPHA_FORMAT_NONPRE;

    EGLint renderableType = config->GetAttribute( EGL_RENDERABLE_TYPE );
    if( attrib_list && attrib_list[0] != EGL_NONE )
        {
        for( int i=0; attrib_list[i] != EGL_NONE; i+=2 )
            {
            EGLint value = attrib_list[i+1];
            switch( attrib_list[i] )
                {
                case EGL_WIDTH:
                    {
                    if( value < 0 )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_PARAMETER );
                        }
                    width = value;
                    break;
                    }

                case EGL_HEIGHT:
                    {
                    if( value < 0 )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_PARAMETER );
                        }
                    height = value;
                    break;
                    }

                case EGL_LARGEST_PBUFFER:
                    {
                    largestPbuffer = value;
                    break;
                    }

                case EGL_TEXTURE_FORMAT:
                    {
                    if( !(renderableType & EGL_OPENGL_ES_BIT) &&
                        !(renderableType & EGL_OPENGL_ES2_BIT) ||
                        (value != EGL_TEXTURE_RGB &&
                         value != EGL_TEXTURE_RGBA &&
                         value != EGL_NO_TEXTURE) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
                    textureFormat = value;
                    break;
                    }

                case EGL_TEXTURE_TARGET:
                    {
                    if( !(renderableType & EGL_OPENGL_ES_BIT) &&
                        !(renderableType & EGL_OPENGL_ES2_BIT) ||
                        (value != EGL_TEXTURE_2D &&
                         value != EGL_NO_TEXTURE) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
                    textureTarget = value;
                    break;
                    }

                case EGL_MIPMAP_TEXTURE:
                    {
                    if( !(renderableType & EGL_OPENGL_ES_BIT) ||
                        !(renderableType & EGL_OPENGL_ES2_BIT) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
                    mipmapTexture = value;
                    break;
                    }

                case EGL_VG_COLORSPACE:
                    {
                    if( !(renderableType & EGL_OPENVG_BIT) ||
                        (value != EGL_VG_COLORSPACE_sRGB &&
                         value != EGL_VG_COLORSPACE_LINEAR) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
                    colorSpace = value;
                    break;
                    }

                case EGL_VG_ALPHA_FORMAT:
                    {
                    if( !(renderableType & EGL_OPENVG_BIT) ||
                        (value != EGL_VG_ALPHA_FORMAT_PRE &&
                         value != EGL_VG_ALPHA_FORMAT_NONPRE) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
                    alphaFormat = value;
                    break;
                    }

                default:
                    EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                }
            }
        }

    if( (textureFormat == EGL_NO_TEXTURE && textureTarget != EGL_NO_TEXTURE) ||
        (textureFormat != EGL_NO_TEXTURE && textureTarget == EGL_NO_TEXTURE) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_MATCH );
        }

    CEGLPbufferSurface* surface = EGLI_NEW CEGLPbufferSurface( colorSpace, alphaFormat,
        EGL_BACK_BUFFER, config, largestPbuffer, textureFormat, textureTarget, mipmapTexture );
    // \note AddSurface() will destroy the object if it fails to take ownership
    surface = (CEGLPbufferSurface*)display->AddSurface( surface );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
    if( !(surface->Resize(width, height)) )
        {
        display->RemoveSurface( surface );
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
    // \todo refactor all this surface creation stuff
    if( (renderableType & EGL_OPENVG_BIT) && (state->SupportedApis() & EGL_OPENVG_BIT) )
        {
        // \todo Delay surface creation until needed.
        // \todo largest Pbuffer
        if( !(state->VGInterface()->CreateSurface( surface->Descriptor(), surface->VGBuffers(), NULL )) )
            {
            // \note RemoveSurface() will destroy the object
            display->RemoveSurface( surface );
            EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
            }
        }

    if( renderableType & EGL_OPENGL_ES_BIT && (state->SupportedApis() & EGL_OPENGL_ES_BIT) )
        {
        struct EGLINativePbufferContainer* nativePbuffer = CEGLOs::CreateNativePbuffer( display->NativeType(),
            *config, width, height, largestPbuffer, textureFormat, textureTarget );
        if( !nativePbuffer )
            {
            if( surface->VGClientSurface() )
                {
                state->VGInterface()->ReleaseSurface( surface->VGClientSurface() );
                }
            // \note RemoveSurface() will destroy the object
            display->RemoveSurface( surface );
            EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
            }
        surface->SetNativePbufferContainer( nativePbuffer );
        }

    if( renderableType & EGL_OPENGL_ES2_BIT && (state->SupportedApis() & EGL_OPENGL_ES2_BIT) )
        {
        struct EGLINativePbufferContainer* nativePbuffer = CEGLOs::CreateNativePbuffer( display->NativeType(),
            *config, width, height, largestPbuffer, textureFormat, textureTarget );
        if( !nativePbuffer )
            {
            if( surface->VGClientSurface() )
                {
                state->VGInterface()->ReleaseSurface( surface->VGClientSurface() );
                }
            // \note RemoveSurface() will destroy the object
            display->RemoveSurface( surface );
            EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
            }
        surface->SetNativePbufferContainer( nativePbuffer );
        }

    surface->AddRef();

    EGLI_LEAVE_RET( (EGLSurface)surface, EGL_SUCCESS );
    }

EGLAPI EGLSurface EGLAPIENTRY eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config,
				  EGLNativePixmapType pixmap,
				  const EGLint *attrib_list)
    {
    EGLI_ENTER_RET( EGL_NO_SURFACE );
    // eglCreatePixmapSurface – EGL_BAD_MATCH raised (as no pixmap-supporting configs)
    EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_MATCH );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurface(EGLDisplay dpy, EGLSurface surfaceId)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );
    CEGLSurface* surface = display->GetSurface( surfaceId );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
        }

    // \note EGLI_ENTER_RET macro will fail if thread allocation didn't succeed

    surface->Lock();
    //TODO: client apis
    if( surface->RemoveRef() )
        {
        if( surface->VGClientSurface() )
            {
            if( !(state->VGInterface()->ReleaseSurface(surface->VGClientSurface())) )
                {
                // need to add reference because surface is still alive
                surface->AddRef();
                surface->Unlock();
                // \todo error code?
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ACCESS );
                }
            }
        // \note RemoveSurface() will destroy the object
        display->RemoveSurface( surface );
        surface = NULL;
        }
    else
        {
        surface->Terminate();
        }
    if( surface ) surface->Unlock();
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurface(EGLDisplay dpy, EGLSurface surfaceId,
				  EGLint attribute, EGLint *value)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    if( !value )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_PARAMETER );
        }
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );
    
    CEGLSurface* surface = display->GetSurface( surfaceId );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
        }
    surface->Lock();
    EGLint ret = EGL_BAD_ATTRIBUTE;
    switch( attribute )
        {
        case EGL_VG_ALPHA_FORMAT:
            {
            ret = surface->AlphaFormat();
            break;
            }
        case EGL_VG_COLORSPACE:
            {
            ret = surface->ColorSpace();
            break;
            }
        case EGL_CONFIG_ID:
            {
            ret = surface->Config()->Id();
            break;
            }
        case EGL_HEIGHT:
            {
            ret = surface->Height();
            break;
            }
        case EGL_WIDTH:
            {
            ret = surface->Width();
            break;
            }
        case EGL_HORIZONTAL_RESOLUTION:
            {
            if( surface->Type() == CEGLSurface::WINDOW_SURFACE )
                {
                //TODO
                EGLI_ASSERT( false );
                }
            else
                {
                ret = EGL_UNKNOWN;
                }
            break;
            }
        case EGL_VERTICAL_RESOLUTION:
            {
            if( surface->Type() == CEGLSurface::WINDOW_SURFACE )
                {
                //TODO
                EGLI_ASSERT( false );
                }
            else
                {
                ret = EGL_UNKNOWN;
                }
            break;
            }
        case EGL_PIXEL_ASPECT_RATIO:
            {
            if( surface->Type() == CEGLSurface::WINDOW_SURFACE )
                {
                //TODO
                EGLI_ASSERT( false );
                }
            else
                {
                ret = EGL_UNKNOWN;
                }
            break;
            }
        case EGL_LARGEST_PBUFFER:
            {
            if( surface->Type() == CEGLSurface::PBUFFER_SURFACE )
                {
                ret = ((CEGLPbufferSurface*)surface)->IsLargestPbuffer();
                }            
            break;
            }
        case EGL_MIPMAP_TEXTURE:
            {
            if( surface->Type() == CEGLSurface::PBUFFER_SURFACE )
                {
				ret = ((CEGLPbufferSurface*)surface)->MipmapTexture();
                }
            break;
            }
        case EGL_MIPMAP_LEVEL:
            {
            if( surface->Type() == CEGLSurface::PBUFFER_SURFACE )
                {
                ret = surface->MipmapLevel();
                }
            break;
            }
        case EGL_MULTISAMPLE_RESOLVE:
            {
            ret = surface->MultisampleResolve();
            break;
            }
        case EGL_RENDER_BUFFER:
            {
            ret = surface->RenderBuffer();
            break;
            }
        case EGL_SWAP_BEHAVIOR:
            {
            ret = surface->SwapBehavior();
            break;
            }
        case EGL_TEXTURE_FORMAT:
            {
            if( surface->Type() == CEGLSurface::PBUFFER_SURFACE )
                {
				ret = ((CEGLPbufferSurface*)surface)->TextureFormat();
                }
            break;
            }
        case EGL_TEXTURE_TARGET:
            {
            if( surface->Type() == CEGLSurface::PBUFFER_SURFACE )
                {
                ret = ((CEGLPbufferSurface*)surface)->TextureTarget();
                }
            break;
            }
        default:
            {
            surface->Unlock();
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ATTRIBUTE );
            }
        }
    *value = ret;
    surface->Unlock();
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }


EGLAPI EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum api)
    {    
    CEGLThread* thread = getThread();
    EGLI_ASSERT( thread != NULL );
    
    if( thread && thread->IsSupportedApi( api ) )
        {
        thread->SetApi( api );
        thread->SetError( EGL_SUCCESS );
        return EGL_TRUE;        
        }
    else if( thread )
        {
        thread->SetError( EGL_BAD_PARAMETER );
        }
    return EGL_FALSE;
    }

EGLAPI EGLenum EGLAPIENTRY eglQueryAPI(void)
    {    
    CEGLThread* thread = getThread();
    EGLI_ASSERT( thread != NULL );
    
    EGLenum api = EGL_NONE;
    if( thread )
        {
        api = thread->CurrentApi();
        thread->SetError( EGL_SUCCESS );
        }
    return api;
    }

EGLAPI EGLBoolean EGLAPIENTRY eglWaitClient(void)
    {
    EGLI_ENTER_RET( EGL_FALSE );    
    // Macro above fails if thread is not available.
    CEGLThread* thread = getThread();

    switch( thread->CurrentApi() )
        {
        case EGL_OPENVG_API:
            {
            if( !(thread->CurrentVGSurface()) )
                {
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_CURRENT_SURFACE );
                }
            if( !(thread->CurrentVGContext()) )
                {
                EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
                }
            if( !(state->VGInterface()) )
                {
                // \todo error code?
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ACCESS );
                }
            state->VGInterface()->Finish();
            break;
            }
        case EGL_OPENGL_ES_API:
            {
            CEGLSurface* read = NULL;
            CEGLSurface* draw = NULL;
            thread->CurrentGLESSurfaces( &read, &draw );
            if( !read || !draw )
                {
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_CURRENT_SURFACE );
                }
            if( !(thread->CurrentGLESContext()) )
                {
                EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
                }
			state->GLESInterface( thread->CurrentGLESContext()->ClientVersion() )->Finish();
            break;
            }
        }
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseThread(void)
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
#if defined(_WIN32)
    EGLI_ASSERT( g_tlsIndex != TLS_OUT_OF_INDEXES );
    if( TlsGetValue( g_tlsIndex ) == NULL )
        {
        // Already released or first call => success
        return EGL_TRUE;
        }
#else // Linux
    // \todo
    EGLI_ASSERT( false );
#endif
#else
    EGLI_ENTER_RET( EGL_FALSE );
#endif //EGLI_USE_SIMULATOR_EXTENSIONS

    CEGLThread* thread = getThread();
    EGLenum api = thread->CurrentApi();
    if( thread->CurrentVGContext() )
        {
        const CEGLDisplay* display = thread->CurrentVGContext()->Display();
        EGLI_ASSERT( display );
        thread->SetApi( EGL_OPENVG_API );
        eglMakeCurrent( (EGLDisplay)display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
        }
    if( thread->CurrentGLESContext() )
        {
        const CEGLDisplay* display = thread->CurrentGLESContext()->Display();
        EGLI_ASSERT( display );
        thread->SetApi( EGL_OPENGL_ES_API );
        eglMakeCurrent( (EGLDisplay)display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
        }
    thread->SetApi( api );
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    process->RemoveThread( thread->Id() );
    if( process->ThreadCount() == 0 )
        {
        // All threads destroyed from process => it's not a process any more
        state->RemoveProcess( process->Id() );
        process = NULL;
        }
#endif
    releaseState();
    return EGL_TRUE;
    }

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferFromClientBuffer(
	      EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer,
	      EGLConfig configId, const EGLint *attrib_list)
    {
    // \todo combine with eglCreatePbufferSurface()
    EGLI_ENTER_RET( EGL_NO_SURFACE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_NO_SURFACE );
    
    // EGLI_ENTER_RET macro above will fail if thread is not available
    CEGLThread* thread = getThread();
    switch( buftype )
        {
        case EGL_OPENVG_IMAGE:
            {
            if( !(thread->CurrentVGContext()) )
                {
                EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ACCESS );
                }
            break;
            }

        default:
            EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_PARAMETER );
        }

    CEGLConfig* config = state->GetConfig( configId );
    if( !config )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_CONFIG );
        }
    
    if( !(state->VGInterface()) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }

    CColorDescriptor colorDescriptor;
    int width = 0;
    int height = 0;
    if( !(state->VGInterface()->IsValidImage(buffer, &colorDescriptor, &width, &height)) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_PARAMETER );
        }
    if( display->FindSurfaceByClientBuffer( buffer ) != NULL ||
        state->VGInterface()->IsImageInUse( buffer ) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ACCESS );
        }
    if( !(colorDescriptor.MatchBitDepth(*(config->GetDescriptor()))) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_MATCH );
        }
    
    EGLint textureFormat = EGL_NO_TEXTURE;
    EGLint textureTarget = EGL_NO_TEXTURE;
    EGLint mipmapTexture = EGL_FALSE;
    EGLint largestPbuffer = EGL_FALSE;
    EGLint colorSpace = colorDescriptor.isNonlinear() ? EGL_VG_COLORSPACE_sRGB : EGL_VG_COLORSPACE_LINEAR;
    EGLint alphaFormat = colorDescriptor.isPremultiplied() ? EGL_VG_ALPHA_FORMAT_PRE : EGL_VG_ALPHA_FORMAT_NONPRE;
    
    EGLint renderableType = config->GetAttribute( EGL_RENDERABLE_TYPE );
    if( attrib_list && attrib_list[0] != EGL_NONE )
        {
        for( int i=0; attrib_list[i] != EGL_NONE; i+=2 )
            {
            EGLint value = attrib_list[i+1];
            switch( attrib_list[i] )
                {
                case EGL_TEXTURE_FORMAT:
                    {
                    if( !(renderableType & EGL_OPENGL_ES_BIT) &&
                        !(renderableType & EGL_OPENGL_ES2_BIT) ||
                        (value != EGL_TEXTURE_RGB &&
                         value != EGL_TEXTURE_RGBA &&
                         value != EGL_NO_TEXTURE) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
                    textureFormat = value;
                    break;
                    }

                case EGL_TEXTURE_TARGET:
                    {
                    if( !(renderableType & EGL_OPENGL_ES_BIT) &&
                        !(renderableType & EGL_OPENGL_ES2_BIT) ||
                        (value != EGL_TEXTURE_2D &&
                         value != EGL_NO_TEXTURE) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
                    textureTarget = value;
                    break;
                    }

                case EGL_MIPMAP_TEXTURE:
                    {
                    if( !(renderableType & EGL_OPENGL_ES_BIT) ||
                        !(renderableType & EGL_OPENGL_ES2_BIT) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                        }
                    mipmapTexture = value;
                    break;
                    }

                default:
                    EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE );
                }
            }
        }

    if( (textureFormat == EGL_NO_TEXTURE && textureTarget != EGL_NO_TEXTURE) ||
        (textureFormat != EGL_NO_TEXTURE && textureTarget == EGL_NO_TEXTURE) )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_MATCH );
        }

    CEGLPbufferSurface* surface = EGLI_NEW CEGLPbufferSurface( colorSpace, alphaFormat,
        EGL_BACK_BUFFER, config, largestPbuffer, textureFormat, textureTarget, mipmapTexture, buffer );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
    // \note AddSurface() will destroy the object if it fails to take ownership
    surface = (CEGLPbufferSurface*)display->AddSurface( surface );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
    if( !(surface->Resize(width, height)) )
        {
        display->RemoveSurface( surface );
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
    
    if( !(state->VGInterface()->CreateSurface(surface->Descriptor(), surface->VGBuffers(), buffer)) )
        {
        // \note RemoveSurface() will destroy the object
        display->RemoveSurface( surface );
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_ALLOC );
        }
    surface->AddRef();
    
    EGLI_LEAVE_RET( surface, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surfaceId,
			    EGLint attribute, EGLint value)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );
    CEGLSurface* surface = display->GetSurface( surfaceId );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
        }
    surface->Lock();
    switch( attribute )
        {
        case EGL_MIPMAP_LEVEL:
            {
            if( !(surface->Config()->GetAttribute( EGL_RENDERABLE_TYPE ) & EGL_OPENGL_ES_BIT ))
                {
                surface->Unlock();
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_PARAMETER );
                }
            break;
            }
        case EGL_MULTISAMPLE_RESOLVE:
            {
			bool hasBoxBit = !!(surface->Config()->GetAttribute( EGL_SURFACE_TYPE ) & EGL_MULTISAMPLE_RESOLVE_BOX_BIT );
            if( value != EGL_MULTISAMPLE_RESOLVE_BOX ||
                value != EGL_MULTISAMPLE_RESOLVE_DEFAULT )
                {
                surface->Unlock();
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ATTRIBUTE );
                } 
            else if( value == EGL_MULTISAMPLE_RESOLVE_BOX && !hasBoxBit )
                {
                surface->Unlock();
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_MATCH );
                }
            break;
            }
        case EGL_SWAP_BEHAVIOR:
            {
			bool hasPreservedBit = !!(surface->Config()->GetAttribute( EGL_SURFACE_TYPE ) & EGL_SWAP_BEHAVIOR_PRESERVED_BIT );
            if( value != EGL_BUFFER_PRESERVED ||
                value != EGL_BUFFER_DESTROYED )
                {
                surface->Unlock();
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ATTRIBUTE );
                }
            else if( value == EGL_BUFFER_PRESERVED && !hasPreservedBit )
                {
                surface->Unlock();
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_MATCH );
                }
            break;
            }
        default:
            {
            surface->Unlock();
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ATTRIBUTE );
            }

        }
    surface->SetAttribute( attribute, value );
    surface->Unlock();
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglBindTexImage(EGLDisplay dpy, EGLSurface surfaceId, EGLint buffer)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );

    // EGLI_ENTER_RET macro above will fail if thread is not available
    CEGLThread* thread = getThread();

    CEGLSurface* surface = display->GetSurface( surfaceId );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
        }

    if( buffer != EGL_BACK_BUFFER )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_PARAMETER );
        }

	if( thread->CurrentGLESContext() == NULL)
		{
		EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
		}

	if( surface->IsLocked() )
		{
		EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ACCESS );
		}
	surface->Lock();

	CEGLSurface* glesReadSurface;
	thread->CurrentGLESSurfaces( &glesReadSurface, NULL );
	state->SyncSurface( EGL_OPENVG_API, 0, surface, glesReadSurface );

    EGLint renderableType = surface->Config()->GetAttribute( EGL_RENDERABLE_TYPE );

    if( (!(renderableType & EGL_OPENGL_ES_BIT) && !(renderableType & EGL_OPENGL_ES2_BIT)) ||
        surface->Type() != CEGLSurface::PBUFFER_SURFACE ||
        (surface->Config()->GetAttribute( EGL_BIND_TO_TEXTURE_RGB ) == EGL_FALSE &&
         surface->Config()->GetAttribute( EGL_BIND_TO_TEXTURE_RGBA ) == EGL_FALSE ) )
        {
        surface->Unlock();
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
        }
    if( ((CEGLPbufferSurface*)surface)->TextureFormat() == EGL_NO_TEXTURE )
        {
        surface->Unlock();
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_MATCH );
        }
	bool mipmapTexture = !!((CEGLPbufferSurface*)surface)->MipmapTexture();
	IEGLtoGLESInterface* iFace = state->GLESInterface( thread->CurrentGLESContext()->ClientVersion() );
	if( surface->BoundContext() == thread->CurrentGLESContext() )
		{
		iFace->Flush();
		}
	else
		{
		// \todo Wait for all issued client API commands drawing to the surface to complete.
		}
	int level = surface->Attribute( EGL_MIPMAP_LEVEL );
	int textureName = iFace->BindTexImage( surface, level, mipmapTexture, surface->Descriptor(), surface->VGColorBuffer());
	EGLITextureBinding& binding = surface->TextureBinding();
	binding.name = textureName;
	binding.level = level;
	binding.clientVersion = thread->CurrentGLESContext()->ClientVersion();
	binding.context = eglGetCurrentContext();
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseTexImage(EGLDisplay dpy, EGLSurface surfaceId, EGLint buffer)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );

	// EGLI_ENTER_RET macro above will fail if thread is not available
    CEGLThread* thread = getThread();

    if( buffer != EGL_BACK_BUFFER )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_PARAMETER );
        }
    CEGLSurface* surface = display->GetSurface( surfaceId );
    // \todo errors etc.
    if( !surface || (surface && surface->Type() != CEGLSurface::PBUFFER_SURFACE) )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
        }
    if( ((CEGLPbufferSurface*)surface)->TextureFormat() == EGL_NO_TEXTURE )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_MATCH );
        }

	EGLITextureBinding& binding = surface->TextureBinding();

	if( display->GetContext( binding.context ))
		{
		EGLenum origAPI = eglQueryAPI();
		EGLSurface origReadSurface, origDrawSurface;
		EGLContext origContext;
		bool switchAPI = origAPI == EGL_OPENGL_ES_API;
		bool switchContext = binding.context != thread->CurrentGLESContext();
		if( switchAPI )
			{
			eglBindAPI( EGL_OPENGL_ES_API );
			}
		if( switchContext )
			{
			// Switch to the context where the texture lives.
			origReadSurface = eglGetCurrentSurface(EGL_READ);
			origDrawSurface = eglGetCurrentSurface(EGL_DRAW);
			origContext = eglGetCurrentContext();
			eglMakeCurrent( display, surfaceId, surfaceId, binding.context );
			}
		state->GLESInterface(binding.clientVersion)->ReleaseTexImage( binding.name, binding.level );
		if( switchContext )
			{
			// Switch back to the original context.
			eglMakeCurrent( display, origDrawSurface, origReadSurface, origContext );
			}
		if( switchAPI )
			{
			eglBindAPI( origAPI );
			}
		}
	else
		{
		// The context has been destroyed. Ignore the call.
		}
	binding.name = 0;
	surface->Unlock();

    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );
    CEGLThread* thread = process->CurrentThread();
    EGLI_ASSERT( thread != NULL );
    
    if( !(thread->CurrentVGContext()) )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_CONTEXT );
        }
    if( !(thread->CurrentVGSurface()) )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
        }
    // eglSwapInterval – hardcode EGL_MAX_SWAP_INTERVAL and EGL_MIN_SWAP_INTERVAL to 1, so nothing here
    if( thread->CurrentVGSurface()->Type() == CEGLSurface::WINDOW_SURFACE )
        {
        ((CEGLWindowSurface*)thread->CurrentVGSurface())->SetSwapInterval( /*interval*/ 1 );
        }

    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig configId,
			    EGLContext share_context,
			    const EGLint *attrib_list)
    {
    EGLI_ENTER_RET( EGL_NO_CONTEXT );
    EGLI_GET_DISPLAY_RET( dpy, EGL_NO_CONTEXT );

    CEGLConfig* config = state->GetConfig( configId );
    if( !config )
        {
        EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_CONFIG );
        }

    CEGLThread* thread = process->CurrentThread();
    EGLI_ASSERT( thread != NULL );

    EGLenum currentApi = thread->CurrentApi();
    if( currentApi == EGL_NONE )
        {
        EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_MATCH );
        }
    
    CEGLContext* shareContext = NULL;
    if( share_context != EGL_NO_CONTEXT )
        {
        shareContext = display->GetContext( share_context );
        if( !shareContext || (shareContext && process->Id() != shareContext->ProcessId()) )
            {
            EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_MATCH );
            }
        if( currentApi != shareContext->ClientApi() )
            {
            EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_CONTEXT );
            }
        }
    
    EGLint renderableType = config->GetAttribute( EGL_RENDERABLE_TYPE );
    EGLint clientVersion = 1;
    if( attrib_list && attrib_list[0] != EGL_NONE )
        {
        switch( attrib_list[0] )
            {
            case EGL_CONTEXT_CLIENT_VERSION:
                {
                if( currentApi != EGL_OPENGL_ES_API )
                    {
                    EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_ATTRIBUTE );
                    }
                if( attrib_list[1] == 1 ) //GLES 1.1
                    {
                    if( !(renderableType & EGL_OPENGL_ES_BIT) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_CONFIG );
                        }
                    clientVersion = 1;
                    }
                else if( attrib_list[1] == 2 ) //GLES 2.0
                    {
                    if( !(renderableType & EGL_OPENGL_ES2_BIT) )
                        {
                        EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_CONFIG );
                        }
                    clientVersion = 2;
                    }
                else
                    {
                    EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_ATTRIBUTE );
                    }
                break;
                }

            default:
                {
                EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_ATTRIBUTE );
                break;
                }
            }
        }
    
    void* clientContext = NULL;
    CEGLContext* context = NULL;
    switch( currentApi )
        {
        case EGL_OPENVG_API:
            {
            EGLI_ASSERT( state->VGInterface() != NULL );
            clientContext = state->VGInterface()->CreateContext( 
                shareContext != NULL ? shareContext->ClientContext() : shareContext );
            if( clientContext )
                {
                context = EGLI_NEW CEGLContext( process->Id(), display, config, currentApi, clientVersion, clientContext );
                // \note AddContext() will destroy the object if it fails to take ownership
                context = display->AddContext( context );
                if( !context )
                    {
                    state->VGInterface()->ReleaseContext( clientContext );
                    EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_ALLOC );
                    }
                }
            break;
            }

        case EGL_OPENGL_ES_API:
            {
			EGLINativeContextType nativeShareContext = shareContext != NULL ? shareContext->NativeContext() : NULL;
			EGLINativeContextType nativeContext = CEGLOs::CreateNativeContext( *config, display->NativeType(), nativeShareContext );
            if( !nativeContext )
                {
                EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_ALLOC );
                }

			IEGLtoGLESInterface* iFace = state->GLESInterface( clientVersion );
			EGLI_ASSERT( iFace != NULL );
			clientContext = state->GLESInterface( clientVersion )->CreateContext( nativeContext );
			if( clientContext )
		        {
                context = EGLI_NEW CEGLContext( process->Id(), display, config, currentApi, clientVersion, clientContext );
                if( context )
                    // \note AddContext() will destroy the object if it fails to take ownership
		            context = display->AddContext( context );
		        if( !context )
			        {
			        iFace->ReleaseContext( clientContext );
					CEGLOs::DestroyNativeContext( nativeContext );
                    EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_ALLOC );
			        }
                context->SetNativeContext( nativeContext );
		        }
	        else
		        {
				CEGLOs::DestroyNativeContext( nativeContext );
                EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_ALLOC );
				}
			break;
			}

        case EGL_NONE:
            {
            EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_MATCH );
            break;
            }
        }
    
    if( context )
        {
        context->AddRef();
        }
    else
        {
        EGLI_LEAVE_RET( EGL_NO_CONTEXT, EGL_BAD_ALLOC );
        }

    EGLI_LEAVE_RET( (EGLContext)context, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );

    CEGLThread* thread = process->CurrentThread();
    EGLI_ASSERT( thread != NULL );

    CEGLContext* context = display->GetContext( ctx );
    if( !context )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_CONTEXT );
        }

    EGLenum clientApi = context->ClientApi();
    switch( clientApi )
        {
        case EGL_OPENVG_API:
            {
            if( context->RemoveRef() )
                {
                state->VGInterface()->ReleaseContext(context->ClientContext());
                // \note RemoveContext() will destroy the object
                display->RemoveContext( context );
                }
            else
                {
                context->Terminate();
                }
            break;
            }

        case EGL_OPENGL_ES_API:
            {
            if( context->RemoveRef() )
                {
				if( !(state->GLESInterface( context->ClientVersion() )->ReleaseContext( context->ClientContext() )) )
					{
					// \todo Handle errors!
                    EGLI_ASSERT( false );
					}
                if( context->NativeContext() && !(CEGLOs::DestroyNativeContext(context->NativeContext())) )
                    {
                    // \todo Handle errors!
                    EGLI_ASSERT( false );
                    }
                // \note RemoveContext() will destroy the object
                display->RemoveContext( context );
                }
            else
                {
                context->Terminate();
                }
            break;
            }
        }

    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,
			  EGLSurface read, EGLContext ctx)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );

    CEGLThread* thread = process->CurrentThread();
    EGLI_ASSERT( thread );
    
    bool releaseCurrent = ( draw == EGL_NO_SURFACE && read == EGL_NO_SURFACE && ctx == EGL_NO_CONTEXT );
    
    if( (ctx == EGL_NO_CONTEXT && draw != EGL_NO_SURFACE && read != EGL_NO_SURFACE) ||
        ((draw == EGL_NO_SURFACE || read == EGL_NO_SURFACE) && ctx != EGL_NO_CONTEXT) )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_MATCH );
        }
    
    CEGLContext* context = display->GetContext( ctx );
    if( (!releaseCurrent && !context ) || (context && context->ProcessId() != process->Id()) )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_CONTEXT );
        }

    EGLenum currentApi = thread->CurrentApi();
    CEGLContext* currentContext = NULL;
    CEGLSurface* currentDrawSurface = NULL;
    CEGLSurface* currentReadSurface = NULL;
    CEGLSurface* drawSurface = NULL;
    CEGLSurface* readSurface = NULL;

    if( !releaseCurrent )
        {    
        if( context && context->ClientApi() == EGL_OPENVG_API && ( draw != read ) )
            {
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_MATCH );
            }
        drawSurface = display->GetSurface( draw );
        readSurface = display->GetSurface( read );
        if( !drawSurface || !readSurface )
            {
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
            }

        if( !(context->Config()->IsCompatible( *(drawSurface->Config()), context->ClientApi())) ||
            !(context->Config()->IsCompatible( *(readSurface->Config()), context->ClientApi())) )
            {
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_MATCH );
            }

        if( (context->Thread() && context->Thread() != thread) ||
            (drawSurface->BoundContext() && drawSurface->BoundContext()->Thread() != thread) ||
            (readSurface->BoundContext() && readSurface->BoundContext()->Thread() != thread) )
            {
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ACCESS );
            }

        if( drawSurface->Type() == CEGLSurface::WINDOW_SURFACE &&
            !(CEGLOs::IsValidNativeWindow(((CEGLWindowSurface*)drawSurface)->NativeType())) )
            {
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_NATIVE_WINDOW );
            }
        if( readSurface != drawSurface && readSurface->Type() == CEGLSurface::WINDOW_SURFACE &&
            !(CEGLOs::IsValidNativeWindow(((CEGLWindowSurface*)readSurface)->NativeType())) )
            {
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_NATIVE_WINDOW );
            }

        // TODO: If binding ctx would exceed the number of current contexts of that client
        //       API type supported by the implementation, an EGL_BAD_ACCESS error is
        //       generated.

        // TODO: If either draw or read are pbuffers created with eglCreatePbufferFromClientBuffer,
        //       and the underlying bound client API buffers are in use by the
        //       client API that created them, an EGL_BAD_ACCESS error is generated        

        // TODO: If draw and read cannot fit into graphics memory simultaneously, an EGL_BAD_MATCH error is generated.

        // TODO: If the previous context of the calling thread has unflushed commands, and
        //       the previous surface is no longer valid, an EGL_BAD_CURRENT_SURFACE
        //       error is generated.

        // TODO: If the ancillary buffers for draw and read cannot be allocated, an EGL_BAD_ALLOC error is generated.

        // TODO: If a power management event has occurred, an EGL_CONTEXT_LOST error is generated.
        
        // TODO: flush context
        switch( context->ClientApi() )
            {
            case EGL_OPENVG_API:
                {
                state->VGInterface()->Flush();
                currentContext = thread->SwitchVGContext( context );
                currentDrawSurface = thread->SwitchVGSurface( drawSurface );
                drawSurface->BindToContext( context );
                if( !(state->VGInterface()->SetCurrentSurface(context->ClientContext(), drawSurface->VGClientSurface())) )
                    {
                    // Restore previous context/surface
                    thread->SwitchVGContext( currentContext );
                    thread->SwitchVGSurface( currentDrawSurface );
                    // \todo error code?
                    EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ACCESS );
                    }
                // \todo track changes through locking
                drawSurface->SetVGDirty( true );
                break;
                }

            case EGL_OPENGL_ES_API:
                {
				if( thread->CurrentGLESContext() )
					{
					state->GLESInterface(context->ClientVersion())->Flush();
					}
                currentContext = thread->SwitchGLESContext( context );
                thread->SwitchGLESSurfaces( drawSurface, readSurface, currentReadSurface, currentDrawSurface );
				drawSurface->BindToContext( context );
                if( drawSurface != readSurface )
                    {
                    readSurface->BindToContext( context );
                    }
				EGLINativeContextType nativeContext = (EGLINativeContextType)state->GLESInterface(context->ClientVersion())->GetNativeContext( context->ClientContext() );
                if( !(context->NativeGLFunctions()) )
                    {
                    EGLINativeGLFunctions* nativeFuncs = EGLI_NEW EGLINativeGLFunctions;
                    if( !nativeFuncs )
                        {
                        // Restore previous context/surface
                        thread->SwitchGLESContext( currentContext );
                        thread->SwitchGLESSurfaces( currentReadSurface, currentDrawSurface, readSurface, drawSurface );
                        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ALLOC );
                        }
                    memset( nativeFuncs, 0, sizeof(EGLINativeGLFunctions) );
                    if( !(CEGLOs::InitializeNativeGLFunctions(nativeFuncs, display->NativeType(), nativeContext)) )
                        {
                        // Restore previous context/surface
                        thread->SwitchGLESContext( currentContext );
                        thread->SwitchGLESSurfaces( currentReadSurface, currentDrawSurface, readSurface, drawSurface );
                        delete nativeFuncs;
                        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ALLOC );
                        }
                    context->SetNativeGLFucntions( nativeFuncs );
                    }
                EGLINativeDisplayType drawDC = 0;
                EGLINativeDisplayType readDC = 0;
                if( drawSurface->Type() == CEGLSurface::PBUFFER_SURFACE ) 
                    {
                    drawDC = ((CEGLPbufferSurface*)drawSurface)->NativeDisplay();
                    }
                else if( drawSurface->Type() == CEGLSurface::WINDOW_SURFACE )
                    {
                    drawDC = ((CEGLWindowSurface*)drawSurface)->OsContext()->glesDisplay;
                    }

                if( readSurface == drawSurface )
                    {
                    readDC = drawDC;
                    }
                else if( readSurface->Type() == CEGLSurface::PBUFFER_SURFACE )
                    {
                    readDC = ((CEGLPbufferSurface*)readSurface)->NativeDisplay();
                    }
                else if( readSurface->Type() == CEGLSurface::WINDOW_SURFACE )
                    {
                    readDC = ((CEGLWindowSurface*)readSurface)->OsContext()->glesDisplay;
                    }

                if( !(CEGLOs::MakeNativeContextCurrent(context->NativeGLFunctions(), drawDC, readDC, context->NativeContext())) )
                    {
                    // Restore previous context/surface. CEGLOs::MakeNativeContextCurrent() will restore previous
                    // native context/surface on failure
                    thread->SwitchGLESContext( currentContext );
                    thread->SwitchGLESSurfaces( currentReadSurface, currentDrawSurface, readSurface, drawSurface );
                    // \todo error code?
                    EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ACCESS );
                    }
                break;
                }

            default:
                {
                EGLI_ASSERT( false );
                break;
                }
            }        
        }
    else
        {
        switch( currentApi )
            {
            case EGL_OPENVG_API:
                {
                state->VGInterface()->Flush();
				if( thread->CurrentGLESContext() )
					{
					// Sync surface before releasing the VG context because there
					// might not be a VG context if GLES locks the surface later.
					CEGLSurface* glesReadSurface;
					thread->CurrentGLESSurfaces( &glesReadSurface, NULL );
					state->SyncSurface( EGL_OPENGL_API, thread->CurrentGLESContext()->ClientVersion(), thread->CurrentVGSurface(), glesReadSurface );
					}
                currentContext = thread->SwitchVGContext( NULL );
                currentDrawSurface = thread->SwitchVGSurface( NULL );
                break;
                }

            case EGL_OPENGL_ES_API:
                {
				if( thread->CurrentGLESContext() )
					{
					state->GLESInterface(thread->CurrentGLESContext()->ClientVersion())->Flush();
					}
				if( thread->CurrentVGContext() )
					{
					// Sync surface before releasing the GLES context because there
					// might not be a GLES context if VG locks the surface later.
					CEGLSurface* glesReadSurface;
					CEGLSurface* glesDrawSurface;
					thread->CurrentGLESSurfaces( &glesReadSurface, &glesDrawSurface );
					state->SyncSurface( EGL_OPENVG_API, 0, glesReadSurface, glesReadSurface );
					if( glesDrawSurface != glesReadSurface )
						{
						state->SyncSurface( EGL_OPENVG_API, 0, glesDrawSurface, NULL );
						}
					}
                currentContext = thread->SwitchGLESContext( NULL );
                thread->SwitchGLESSurfaces( NULL, NULL, currentReadSurface, currentDrawSurface );
                break;
                }
            }
        }

    if( currentDrawSurface && currentDrawSurface != drawSurface )
        {
        currentDrawSurface->BindToContext( NULL );
        if( currentDrawSurface->RemoveRef() )
            {
            if( currentDrawSurface->VGClientSurface() )
                {
                if( !(state->VGInterface()->ReleaseSurface(currentDrawSurface->VGClientSurface())) )
                    {
                    // \todo error code?
                    EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ACCESS );
                    }
                }
            if( currentDrawSurface->Type() == CEGLSurface::PBUFFER_SURFACE )
                {
                //((CEGLPbufferSurface*)currentDrawSurface)->SetNativePbufferContainer( NULL );
                //EGLI_ASSERT( false );
                }
            // \note RemoveSurface() will destroy the object
            display->RemoveSurface( currentDrawSurface );
            }
        if( currentReadSurface == currentDrawSurface )
            {
            currentReadSurface = NULL;
            }
        currentDrawSurface = NULL;
        }

    if( currentReadSurface && currentReadSurface != readSurface )
        {
        currentReadSurface->BindToContext( NULL );
        if( currentReadSurface->RemoveRef() )
            {
            if( currentReadSurface->VGClientSurface() )
                {
                if( !(state->VGInterface()->ReleaseSurface(currentReadSurface->VGClientSurface())) )
                    {
                    // \todo error code?
                    EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ACCESS );
                    }
                }
            if( currentReadSurface->Type() == CEGLSurface::PBUFFER_SURFACE )
                {
                // \todo 
                ((CEGLPbufferSurface*)currentReadSurface)->SetNativePbufferContainer( NULL );
                }
            // \note RemoveSurface() will destroy the object
            display->RemoveSurface( currentReadSurface );
            }
        }

    if( currentContext && currentContext != context )
        {
        currentContext->SetThread( NULL );
        if( currentContext->RemoveRef() )
            {
            if( currentContext->ClientContext() )
                {
                if( currentContext->ClientApi() == EGL_OPENVG_API )
                    {
					bool success = state->VGInterface()->ReleaseContext( currentContext->ClientContext() );
                    EGLI_ASSERT( success );
                    }
                if( currentContext->ClientApi() == EGL_OPENGL_ES_API )
                    {
					bool success = state->GLESInterface(currentContext->ClientVersion())->ReleaseContext(currentContext->ClientContext());
                    EGLI_ASSERT( success );
                    }
                }
            if( currentContext->NativeContext() )
                {
                if( !(CEGLOs::DestroyNativeContext(currentContext->NativeContext())) )
                    {
                    // \todo Handle errors!
                    EGLI_ASSERT( false );
                    }
                }
            // \note RemoveContext() will destroy the object
            display->RemoveContext( currentContext );
            currentContext = NULL;
            }
        }
    if( context && currentContext != context ) 
        {
        context->AddRef();
        }
    if( drawSurface && currentDrawSurface != drawSurface ) 
        {
        drawSurface->AddRef();
        }
    if( readSurface && readSurface != drawSurface && readSurface != currentReadSurface )
        {
        readSurface->AddRef();
        }
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }


EGLAPI EGLContext EGLAPIENTRY eglGetCurrentContext(void)
    {
    EGLI_ENTER_RET( EGL_NO_CONTEXT );
    CEGLThread* thread = process->CurrentThread();
    EGLI_ASSERT( thread != NULL );
    EGLContext ret = EGL_NO_CONTEXT;
    switch( thread->CurrentApi() )
        {
        case EGL_OPENVG_API:
            {
            if( thread->CurrentVGContext() )
                {
                ret = thread->CurrentVGContext();
                }
            break;
            }
        case EGL_OPENGL_ES_API:
            {
            if( thread->CurrentGLESContext() )
                {
                ret = thread->CurrentGLESContext();
                }
            break;
            }
        }
    EGLI_LEAVE_RET( ret, EGL_SUCCESS );
    }

EGLAPI EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
    {    
    EGLI_ENTER_RET( EGL_NO_SURFACE );
    if( readdraw != EGL_READ && readdraw != EGL_DRAW )
        {
        EGLI_LEAVE_RET( EGL_NO_SURFACE, EGL_BAD_PARAMETER );
        }

    CEGLThread* thread = process->CurrentThread();
    EGLI_ASSERT( thread != NULL );

    EGLSurface ret = EGL_NO_SURFACE;
    EGLenum currentApi = thread->CurrentApi();
    switch( currentApi )
        {
        case EGL_OPENVG_API:
            {
            CEGLContext* context = thread->CurrentVGContext();
            if( context )
                {
                ret = thread->CurrentVGSurface();
                }
            break;
            }

        case EGL_OPENGL_ES_API:
            {
            CEGLSurface* read = NULL;
            CEGLSurface* draw = NULL;
            thread->CurrentGLESSurfaces( &read, &draw );
            switch( readdraw )
                {
                case EGL_READ:
                    {
                    ret = (EGLSurface)read;
                    break;
                    }

                case EGL_DRAW:
                    {
                    ret = (EGLSurface)draw;
                    break;
                    }
                }
            break;
            }

        default:
            {
            EGLI_ASSERT( false );
            }
        }
    EGLI_LEAVE_RET( ret, EGL_SUCCESS );
    }

EGLAPI EGLDisplay EGLAPIENTRY eglGetCurrentDisplay(void)
    {
    EGLI_ENTER_RET( EGL_NO_DISPLAY );
    CEGLThread* thread = process->CurrentThread();
    EGLI_ASSERT( thread );
    EGLenum currentApi = thread->CurrentApi();
    EGLDisplay ret = EGL_NO_DISPLAY;
    switch( currentApi )
        {
        case EGL_OPENVG_API:
            {
            if( thread->CurrentVGContext() )
                {
                ret = (EGLDisplay)thread->CurrentVGContext()->Display();
                }
            break;
            }
        case EGL_OPENGL_ES_API:
            {
            if( thread->CurrentGLESContext() )
                {
                ret = (EGLDisplay)thread->CurrentGLESContext()->Display();
                }
            break;
            }

        default:
            EGLI_ASSERT( false );
        }
    EGLI_LEAVE_RET( ret, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglQueryContext(EGLDisplay dpy, EGLContext ctx,
			   EGLint attribute, EGLint *value)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE )
    if( !value )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_PARAMETER );
        }
    CEGLContext* context = display->GetContext( ctx );
    if( !context )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_CONTEXT );
        }
    switch( attribute )
        {
        case EGL_CONFIG_ID:
            {
            *value = ((CEGLConfig*)context->Config())->GetAttribute( EGL_CONFIG_ID );
            break;
            }

        case EGL_CONTEXT_CLIENT_TYPE:
            {
            *value = context->ClientApi();
            break;
            }

        case EGL_CONTEXT_CLIENT_VERSION:
            {
            *value = context->ClientVersion();
            break;
            }

        case EGL_RENDER_BUFFER:
            {
            CEGLSurface* surface = NULL;
            CEGLThread* thread = process->CurrentThread();
            EGLI_ASSERT( thread != NULL );
            if( thread->CurrentVGContext() == context )
                {
                surface = thread->CurrentVGSurface();
                }
            else if( thread->CurrentGLESContext() == context )
                {
                thread->CurrentGLESSurfaces( NULL, &surface );
                }
            else
                {
                surface = process->FindBoundSurface( context, false );
                }
            if( surface )
                {
                *value = surface->RenderBuffer();
                }
            else 
                {
                *value = EGL_NONE;
                }
            break;
            }

        default:
            {
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ATTRIBUTE );
            }
        }
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }


EGLAPI EGLBoolean EGLAPIENTRY eglWaitGL(void)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    // EGLI_ENTER_RET macro above will fail if thread is not available
    CEGLThread* thread = getThread();
    EGLBoolean ret = EGL_TRUE;
    EGLenum api = eglQueryAPI();
    if( eglBindAPI( EGL_OPENGL_ES_API ) )
        {
        ret = eglWaitClient();
        if( thread->IsSupportedApi(api) )
            {
            eglBindAPI( api );
            }
        }    
    EGLI_LEAVE_RET( ret, thread->Error() );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglWaitNative(EGLint engine)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    // eglWaitNative – no-op, as no such thing as native rendering to pbuffers
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surfaceId)
    {
    // \todo eglSwapBuffers – all surfaces are single-buffered pbuffers, so this just needs to do a flush in the current client API
    EGLI_ENTER_RET( EGL_FALSE );
    EGLI_GET_DISPLAY_RET( dpy, EGL_FALSE );

    // EGLI_ENTER_RET macro above will fail if thread is not available
    CEGLThread* thread = getThread();

    CEGLSurface* surface = display->GetSurface( surfaceId );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
        }
    EGLenum currentApi = thread->CurrentApi();
    switch( currentApi )
        {
        case EGL_OPENVG_API:
            {
            if( thread->CurrentVGContext() != surface->BoundContext() )
                {
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
                }
            break;
            }
        case EGL_OPENGL_ES_API:
            {
            if( thread->CurrentGLESContext() != surface->BoundContext() )
                {
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
                }
            // \todo other GLES stuff?
            }
        }
    if( surface->Type() != CEGLSurface::WINDOW_SURFACE || surface->RenderBuffer() == EGL_SINGLE_BUFFER )
        {
        EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
        }

    if( !(CEGLOs::IsValidNativeWindow(((CEGLWindowSurface*)surface)->NativeType())) )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_NATIVE_WINDOW ); 
        }
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    int w, h;
    if( !(CEGLOs::GetNativeWindowSize(((CEGLWindowSurface*)surface)->NativeType(), w, h)) )
        {
        // \todo Error code?
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_SURFACE );
        }
#else
    int w = surface->Width();
    int h = surface->Height();
#endif
    if( surface->Width() != w || surface->Height() != h )
        {
        if( !(surface->Resize(w, h)) )
            {
            EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ALLOC );
            }
        if( w <= 0 || h <= 0 )
            {
            // invisible window => do nothing
            EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
            }
        if( surface->VGClientSurface() && state->VGInterface() )
            {
            if( !(state->VGInterface()->ResizeSurface(thread->CurrentVGContext()->ClientContext(),
                surface->VGClientSurface(), w, h, surface->VGBuffers())) )
                {
                EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ALLOC );
                }
            }
        }

    EGLint stride = surface->Stride();
    void* buf = ((CEGLWindowSurface*)surface)->CurrentBuffer();
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    buf = (void*)((char*)buf + ((surface->Height() - 1) * stride));
#else
    switch( currentApi )
        {
        case EGL_OPENVG_API:
            {
            state->VGInterface()->CopyBuffers( buf, surface->Stride(), surface->VGClientSurface() );
            break;
            }
        case EGL_OPENGL_ES_API:
            {
			if( surface->BoundContext() && !(state->GLESInterface(surface->BoundContext()->ClientVersion())) )
				{
				// \todo error code?
				EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_ACCESS );
				}
            }
        }
#endif
    CEGLOs::BlitToOSWindow( currentApi, display, ((CEGLWindowSurface*)surface)->OsContext(), buf, surface->Width(), surface->Height(), stride );
    EGLI_LEAVE_RET( EGL_TRUE, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglCopyBuffers(EGLDisplay dpy, EGLSurface surface,
			  EGLNativePixmapType target)
    {
    EGLI_ENTER_RET( EGL_FALSE );
    // eglCopyBuffers always raises EGL_BAD_NATIVE_PIXMAP error (as no native pixmaps supported)
    EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_NATIVE_PIXMAP );
    }

/* Now, define eglGetProcAddress using the generic function ptr. type */
EGLAPI __eglMustCastToProperFunctionPointerType EGLAPIENTRY
       eglGetProcAddress(const char *procname)
    {
    EGLI_ENTER_RET( NULL );
    // \todo EGL
    __eglMustCastToProperFunctionPointerType ret = NULL;
    if( strncmp( procname, "vg", 2 ) == 0 )
        {
        ret = state->VGInterface()->GetVGProcAddress( procname );
        }
    else if( strncmp( procname, "egl", 3 ) == 0 )
        {
        ret = (__eglMustCastToProperFunctionPointerType)CEGLExtension::eglGetProcAddress( procname );
        }
    else if( strncmp( procname, "gl", 2 ) == 0 )
        {
        ret = state->GLESInterface(2)->GetGLProcAddress( procname );
		if( ret == NULL )
			{
			// Not found in GLES2, try GLES1.
			ret = state->GLESInterface(1)->GetGLProcAddress( procname );
			}
        }
    else
        {
        // return just NULL pointer...
        }
    EGLI_LEAVE_RET( ret, EGL_SUCCESS );
    }

#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
// non-standard Simulator functions
EGLAPI void EGLAPIENTRY eglSimulatorSetProcessInformation( EGLint processId, EGLint threadId )
    {
    EGLI_ENTER_RET(EGLI_NO_RET);
    if( process && process->Id() != processId )
        {
        process = state->GetProcess( processId );
        }
    if( !process )
        {
        process = state->AddProcess( processId, true );
        EGLI_ASSERT( process != NULL );
        CEGLThread* thread = process->AddThread( threadId, true, state->SupportedApis() );
        EGLI_ASSERT( thread != NULL );
        state->AddRef();
        }
    else if( !(process->HasThread( threadId )) )
        {
        CEGLThread* thread = process->AddThread( threadId, true, state->SupportedApis() );
        EGLI_ASSERT( thread != NULL );
        state->AddRef();
        }
    else
        {
        state->SetCurrentProcessThread( processId, threadId );
        } 
    EGLI_LEAVE_RET( EGLI_NO_RET, EGL_SUCCESS );
    }

EGLAPI EGLint EGLAPIENTRY eglSimulatorGetError()
    {
    //TODO
    EGLI_ENTER_RET( EGL_SUCCESS );
    EGLI_ASSERT( process != NULL );
    CEGLThread* thread = process->CurrentThread();
    EGLI_ASSERT( thread != NULL );
    EGLint err = thread->Error();
    EGLI_LEAVE_RET( err, err );
    }

// This is called after eglCreateWindowSurface()
EGLAPI void EGLAPIENTRY eglSimulatorSetSurfaceParams(
    EGLDisplay displayId, EGLSurface surfaceId, EGLint width, EGLint height,
    EGLint stride, void* buffer0, void *buffer1)
    {
    EGLI_ENTER_RET( EGLI_NO_RET );
    CEGLDisplay* display = state->GetDisplay( displayId );
    if( !display || (display && !(display->IsInitialized())) )
        {
        EGLI_LEAVE_RET( EGLI_NO_RET, EGL_NOT_INITIALIZED );
        }
    EGLI_ASSERT( display->ProcessId() == process->Id() );

    CEGLSurface* surface = display->GetSurface( surfaceId );
    if( !surface )
        {
        EGLI_LEAVE_RET( EGLI_NO_RET, EGL_BAD_SURFACE );
        }

    EGLI_ASSERT( surface->Type() == CEGLSurface::WINDOW_SURFACE );
    if( !(surface->Resize(width, height)) )
        {
        EGLI_LEAVE_RET( EGLI_NO_RET, EGL_BAD_ALLOC );
        }
    ((CEGLWindowSurface*)surface)->SetStride( stride );
    ((CEGLWindowSurface*)surface)->SetWindowBuffers( buffer0, buffer1 );
    
    CEGLThread* thread = process->CurrentThread();
    EGLI_ASSERT( thread != NULL );
    //TODO: resizing
    switch( thread->CurrentApi() )
        {
        case EGL_OPENVG_API:
            {
            if( !(surface->VGColorBuffer()) )
                {
                EGLI_ASSERT( state->VGInterface() != NULL );
                SurfaceDescriptor* surfaceDesc = surface->Descriptor();                
				bool success = state->VGInterface()->CreateSurface( surfaceDesc, surface->VGBuffers(), NULL );
                EGLI_ASSERT( success );
                }
            break;
            }
        case EGL_OPENGL_ES_API:
            {
            //TODO: GLES not implemented
            //EGLI_ASSERT( false );
            break;
            }
        default:
            {
            EGLI_ASSERT( false );
            }
        }
    
    //TODO: what else? 
    EGLI_LEAVE_RET( EGLI_NO_RET, EGL_SUCCESS );
    }

EGLAPI EGLNativePixmapType EGLAPIENTRY eglSimulatorGetPixmapSurfaceBitmap( EGLDisplay display, EGLSurface surface)
    {
    //TODO
    return NULL;
    }
#endif

#if defined(_WIN32)

BOOL WINAPI DllMain( HINSTANCE hinstDLL,
                     DWORD fdwReason,
                     LPVOID lpvReserved )
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    switch( fdwReason )
        {
        case DLL_PROCESS_ATTACH:
            {
            g_tlsIndex = TlsAlloc();
            if( g_tlsIndex == TLS_OUT_OF_INDEXES )
                return FALSE;
            if( !g_lockInitialized )
                {
                CEGLOs::InitializeLock( &g_eglLock );
                g_lockInitialized = true;
                }
            break;
            }
	    case DLL_THREAD_ATTACH:
            {
            break;
            }

	    case DLL_THREAD_DETACH:
            {
            break;
            }
	    case DLL_PROCESS_DETACH:
            {
            if( g_eglState )
                {
                EGLI_ASSERT( g_eglState->RefCount() == 0 );
                delete g_eglState;
                g_eglState = NULL;
                }
             if( g_lockInitialized )
                {
                CEGLOs::DestroyLock( &g_eglLock );
                g_lockInitialized = false;
                }
            if( g_tlsIndex != TLS_OUT_OF_INDEXES )
                {
                TlsFree( g_tlsIndex );
                }
            break;
            }
        }
#else
switch( fdwReason )
        {
        case DLL_PROCESS_ATTACH:
	    case DLL_THREAD_ATTACH:
	    case DLL_THREAD_DETACH:
            {
            break;
            }
	    case DLL_PROCESS_DETACH:
            {
            EGLI_ASSERT( g_eglState == NULL );
            break;
            }
        }
#endif // EGLI_USE_SIMULATOR_EXTENSIONS

    return TRUE;
    }

#endif // _WIN32

#ifdef __cplusplus
}
#endif
