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

#include "EGLOs.h"
#include "EGLState.h"
#include "EGLProcess.h"
#include "EGLThread.h"
#include "EGLWindowSurface.h"
#include "EGLDisplay.h"
#include "EGLContext.h"

#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
void doBlit( EGLIOsWindowContext* context, void* buf, int width, int height, int stride )
    {
    if( !context->osBuffer || !context->pixmap || context->width != width || context->height != height )
        {
            if( context->pixmap )
                DeleteObject( context->pixmap );
            context->osBuffer = NULL;
            context->pixmap = NULL;

            context->width = width;
            context->height = height;

            struct
            {
                BITMAPINFOHEADER	header;
                DWORD				rMask;
                DWORD				gMask;
                DWORD				bMask;
            } bmi;
            bmi.header.biSize			= sizeof( BITMAPINFOHEADER );
            bmi.header.biWidth			= width;
            bmi.header.biHeight			= height;
            bmi.header.biPlanes			= 1;
            bmi.header.biBitCount		= (WORD)32;
            bmi.header.biCompression	= BI_BITFIELDS;
            bmi.rMask = 0x00FF0000;
            bmi.gMask = 0x0000FF00;
            bmi.bMask = 0x000000FF;
            
            context->pixmap = CreateDIBSection( context->vgDisplay, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, (void**)&context->osBuffer, NULL, 0 );
            if( !context->pixmap )
            {
                context->osBuffer = NULL;
                return;
            }
        }
     
        if( context->osBuffer )
            {
            HDC winDC = GetDC( context->window );
			GdiFlush();
            memcpy( context->osBuffer, buf, stride*height );
            SelectObject( context->vgDisplay, context->pixmap );
            EGLI_ASSERT( BitBlt(winDC, 0, 0, width, height, context->vgDisplay, 0, 0, SRCCOPY) );
            ReleaseDC( context->window, winDC );
            SelectObject( context->vgDisplay, NULL );
        }
    }
#endif

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
    {
    case WM_CLOSE:
    case WM_DESTROY:
        //PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        {
        /*CEGLState* state = getState();
        if( state )
            {
            CEGLThread* thread = state->GetCurrentProcess()->CurrentThread();
            if( thread && thread->CurrentVGSurface() )
                {
                if( thread->CurrentVGSurface()->Type() == CEGLSurface::WINDOW_SURFACE )
                    {
                    CEGLWindowSurface* surface = (CEGLWindowSurface*)thread->CurrentVGSurface();
                    InvalidateRect(hwnd, NULL, 0);
                    doBlit( surface->OsContext(), surface->VGColorBuffer(), surface->Width(), surface->Height(), surface->Stride() );
                    }
                }
            }*/
        return 0;
        }
    default:
        break;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
}

void CEGLOs::InitializeLock( EGLI_LOCK *lock )
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    InitializeCriticalSection( lock );
#endif
    }

void CEGLOs::GetLock( EGLI_LOCK *lock )
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    EnterCriticalSection( lock );
#endif
    }

void CEGLOs::ReleaseLock(EGLI_LOCK *lock )
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    LeaveCriticalSection( lock );
#endif
    }

void CEGLOs::DestroyLock( EGLI_LOCK *lock )
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    DeleteCriticalSection( lock );
#endif
    }

void CEGLOs::ConfigToNativePixelFormat( const CEGLConfig& config, EGLINativePixelFormatType* nativeFormat )
    {
    EGLI_ASSERT( nativeFormat != NULL );
    memset( nativeFormat, 0, sizeof(PIXELFORMATDESCRIPTOR) );
    nativeFormat->nSize = sizeof( PIXELFORMATDESCRIPTOR );
    nativeFormat->nVersion = 1;
    nativeFormat->dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; // always support OGL
    
    // \todo Do we need color index?
    nativeFormat->iPixelType = PFD_TYPE_RGBA;
    
    // \todo overlay/underlay?
    nativeFormat->iLayerType = PFD_MAIN_PLANE;

    EGLint surfaceType = config.GetAttribute( EGL_SURFACE_TYPE );
    if( surfaceType & EGL_WINDOW_BIT )
        {
        nativeFormat->dwFlags |= PFD_DRAW_TO_WINDOW;
        }
    if( surfaceType & EGL_PIXMAP_BIT )
        {
        nativeFormat->dwFlags |= PFD_DRAW_TO_BITMAP;
        }
    nativeFormat->cColorBits = config.GetAttribute( EGL_RED_SIZE ) +
                               config.GetAttribute( EGL_GREEN_SIZE ) +
                               config.GetAttribute( EGL_BLUE_SIZE );

    nativeFormat->cAlphaBits = config.GetAttribute( EGL_ALPHA_SIZE );
    nativeFormat->cDepthBits = config.GetAttribute( EGL_DEPTH_SIZE );
    nativeFormat->cStencilBits = config.GetAttribute( EGL_STENCIL_SIZE );

    // \todo Aux buffers, accumulation buffers?
    }

EGLINativeContextType CEGLOs::CreateNativeContext( const CEGLConfig& config, EGLINativeDisplayType display, EGLINativeContextType shareContext )
    {
    EGLINativeContextType ret = NULL;
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    DWORD error = ERROR_SUCCESS;
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    CEGLOs::ConfigToNativePixelFormat( config, &pfd );
	int pixelFormat = ChoosePixelFormat( display, &pfd );
	if( !pixelFormat )
		{
        return ret;
		}
    int currentPixelFormat = GetPixelFormat( display );
	if( !currentPixelFormat && !SetPixelFormat( display, pixelFormat, &pfd ) )
		{
        error = GetLastError();
		return ret;
		}
    ret = wglCreateContext( display );
	if( ret != NULL && shareContext != NULL)
		{
		// Share context data
		if( !wglShareLists( shareContext, ret ) )
			{
			wglDeleteContext( ret );
			ret = NULL;
			}
		}
#endif
    return ret;
    }

EGLINativeContextType CEGLOs::CurrentNativeContext()
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    return 0;
#else
    return wglGetCurrentContext();
#endif
    }

EGLINativeDisplayType CEGLOs::CurrentNativeSurface()
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    return wglGetCurrentDC();
#else
    return 0;
#endif
    }

bool CEGLOs::MakeNativeContextCurrent( struct EGLINativeGLFunctions* func, EGLINativeDisplayType draw, EGLINativeDisplayType read, EGLINativeContextType context )
    {
    bool ret = true;
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    DWORD err;
    EGLINativeContextType prevContext = wglGetCurrentContext();
    EGLINativeDisplayType prevDraw = wglGetCurrentDC();
    EGLINativeDisplayType prevRead = NULL;
    if( prevDraw ) prevRead = func->wglGetCurrentReadDCARB();
    if( context == NULL )
        {
        if( wglGetCurrentContext() != NULL )
            {
            //warning C4800: 'BOOL' : forcing value to bool 'true' or 'false' (performance warning)
            ret = (wglMakeCurrent( NULL, context ) != 0);
            }
        }
    else
        {
        //warning C4800: 'BOOL' : forcing value to bool 'true' or 'false' (performance warning)
        ret = (func->wglMakeContextCurrentARB( draw, read, context ) != 0);
        }
    if( !ret )
        {
        // \todo Handle error
        err = GetLastError();
        // Restore previous context/surface on failure
        func->wglMakeContextCurrentARB( prevDraw, prevRead, prevContext );        
        }
#endif
    return ret;
    }

bool CEGLOs::DestroyNativeContext( EGLINativeContextType context )
    {
    bool ret = true;
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if( context != NULL )
        {
        //warning C4800: 'BOOL' : forcing value to bool 'true' or 'false' (performance warning)
        ret = (wglDeleteContext( context ) != 0);
        }
#endif
    return ret;
    }

bool CEGLOs::InitializeNativeGLFunctions( struct EGLINativeGLFunctions* func, EGLINativeDisplayType display, EGLINativeContextType context )
    {
    bool ret = true;
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    DWORD error = ERROR_SUCCESS;
    HDC currentDC = wglGetCurrentDC();
    HGLRC currentContext = wglGetCurrentContext();
    
	ret = !!wglMakeCurrent( display, context );
    if( ret )
        {
        func->wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)wglGetProcAddress( "wglMakeContextCurrentARB" );
        func->wglGetCurrentReadDCARB = (PFNWGLGETCURRENTREADDCARBPROC)wglGetProcAddress ( "wglGetCurrentReadDCARB" );
        func->wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress( "wglChoosePixelFormatARB" );
        func->wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress( "wglCreatePbufferARB" );
        func->wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress( "wglDestroyPbufferARB" );
        func->wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress( "wglGetPbufferDCARB" );
        func->wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress( "wglReleasePbufferDCARB" );
        func->wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress( "wglQueryPbufferARB" );

        if( !(func->wglChoosePixelFormatARB &&
              func->wglCreatePbufferARB &&
              func->wglDestroyPbufferARB &&
              func->wglGetPbufferDCARB &&
              func->wglMakeContextCurrentARB &&
              func->wglGetCurrentReadDCARB &&
              func->wglQueryPbufferARB &&
              func->wglReleasePbufferDCARB) )
              ret = false;
        }

    // \todo Other needed extensions.

    if( !(wglMakeCurrent( currentDC, currentContext )) )
        {
        error = GetLastError();
        if( error == ERROR_INVALID_HANDLE )
            {
            // For some reason the current DC or current wgl context was invalid.
            // Just set them NULL for now.
            wglMakeCurrent(NULL, NULL);
            }
        else
            {
            ret = false;
            }
        }
#endif //EGLI_USE_SIMULATOR_EXTENSIONS
    return ret;
    }

struct EGLINativePbufferContainer* CEGLOs::CreateNativePbuffer( EGLINativeDisplayType display,
        const CEGLConfig& config, EGLint width, EGLint height, EGLBoolean largestPbuffer, 
        EGLint textureFormat, EGLint textureTarget )
    {
    struct EGLINativePbufferContainer* ret = EGLI_NEW EGLINativePbufferContainer;
    if( !ret ) return NULL;
    memset( ret, 0, sizeof(struct EGLINativePbufferContainer) );
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    EGLINativeContextType context = CEGLOs::CreateNativeContext( config, display, NULL );
    if( !context )
        {
        delete ret;
        ret = NULL;
        return ret;
        }
    ret->copyContext = context;
    if( !(CEGLOs::InitializeNativeGLFunctions( &(ret->functions), display, ret->copyContext )) )
        {
        CEGLOs::DestroyNativePbuffer( ret );
        delete ret;
        ret = NULL;
        return ret;
        }
    
    int redSize = config.GetAttribute( EGL_RED_SIZE );
    int greenSize = config.GetAttribute( EGL_GREEN_SIZE );
    int blueSize = config.GetAttribute( EGL_BLUE_SIZE );
    int alphaSize = config.GetAttribute( EGL_ALPHA_SIZE );
    int depthSize = config.GetAttribute( EGL_DEPTH_SIZE );
    int stencilSize = config.GetAttribute( EGL_STENCIL_SIZE );
    
    // \todo Map EGL config properly to WGL config.
    int attributes[] =
        {
        WGL_SUPPORT_OPENGL_ARB, TRUE,
        WGL_DRAW_TO_PBUFFER_ARB, TRUE,
        WGL_RED_BITS_ARB, redSize,
        WGL_GREEN_BITS_ARB, greenSize,
        WGL_BLUE_BITS_ARB, blueSize,
        WGL_ALPHA_BITS_ARB, alphaSize,
        WGL_DEPTH_BITS_ARB, depthSize,
        WGL_STENCIL_BITS_ARB, stencilSize,
        WGL_DOUBLE_BUFFER_ARB, TRUE,
        0
        };
    unsigned int formatCount = 0;
    int pixelFormat = 0;
    if( !(ret->functions.wglChoosePixelFormatARB( display, attributes, NULL, 1, &pixelFormat, &formatCount )) )
        {
        EGLI_ASSERT(false);
        DWORD err = GetLastError();
        CEGLOs::DestroyNativePbuffer( ret );
        delete ret;
        ret = NULL;
        return ret;
        }
    if( formatCount == 0 )
        {
        EGLI_ASSERT(false);
        DWORD err = GetLastError();
        CEGLOs::DestroyNativePbuffer( ret );
        delete ret;
        ret = NULL;
        return ret;
        }
    int wglTextureFormat = WGL_NO_TEXTURE_ARB;
    switch( textureFormat )
        {
        case EGL_TEXTURE_RGB:
            {
            wglTextureFormat = WGL_TEXTURE_RGB_ARB;
            break;
            }
        case EGL_TEXTURE_RGBA:
            {
            wglTextureFormat = WGL_TEXTURE_RGBA_ARB;
            break;
            }
        }
    int wglTextureTarget = WGL_NO_TEXTURE_ARB;
    switch( textureTarget )
        {
        case EGL_TEXTURE_2D:
            {
            wglTextureTarget = WGL_TEXTURE_2D_ARB;
            break;
            }      
        }
    int pbufferAttributes[] =
        {
        WGL_TEXTURE_FORMAT_ARB, wglTextureFormat,
        WGL_TEXTURE_TARGET_ARB, wglTextureTarget,
        WGL_PBUFFER_LARGEST_ARB, largestPbuffer,
        0
        };
    ret->pbuffer = ret->functions.wglCreatePbufferARB( display, pixelFormat,
        width, height, pbufferAttributes );
    if( !(ret->pbuffer) )
        {
        EGLI_ASSERT(false);
        DWORD err = GetLastError();
        CEGLOs::DestroyNativePbuffer( ret );
        delete ret;
        ret = NULL;
        return ret;
        }
    ret->display = ret->functions.wglGetPbufferDCARB( ret->pbuffer );
    if( !(ret->display) )
        {
        EGLI_ASSERT(false);
        DWORD err = GetLastError();
        CEGLOs::DestroyNativePbuffer( ret );
        delete ret;
        ret = NULL;
        return ret;
        }
#endif // EGLI_USE_SIMULATOR_EXTENSIONS
    return ret;
    }

bool CEGLOs::DestroyNativePbuffer( struct EGLINativePbufferContainer* container )
    {
    // \todo Handle errors!
    bool ret = true;
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    DWORD err;
    if( container->display && container->pbuffer && container->functions.wglReleasePbufferDCARB )
        {
        if( !(container->functions.wglReleasePbufferDCARB(container->pbuffer, container->display)) )
            {
            err = GetLastError();
            ret = false;
            }
        }
    if( container->pbuffer && container->functions.wglDestroyPbufferARB )
        {
        if( !(container->functions.wglDestroyPbufferARB(container->pbuffer)) )
            {
            err = GetLastError();
            ret = false;
            }
        }
    if( container->copyContext )
        {
        ret = CEGLOs::DestroyNativeContext( container->copyContext );
        if( !ret )
            {
            err = GetLastError();
            }
        }
#endif //EGLI_USE_SIMULATOR_EXTENSIONS
    return ret;
    }

EGLINativeDisplayType CEGLOs::CreateDefaultDisplay()
    {
    // \todo It seems that we can't use any of these DC-functions.
    //       Problems with pixel format selection etc. at least with
    //       NVidia and Intel GPUs, ATI/AMD seems to work with GetDC(NULL)
    //       or CreateDC(). No idea how to create EGL_DEFAULT_DISPLAY for
    //       NV/Intel GPUs. For now CEGLDisplay::Initialize() creates dummy
    //       window and uses DC from that window.
    //return (EGLINativeDisplayType)CreateDC(TEXT("DISPLAY"), TEXT("DISPLAY")/*NULL*/, NULL, NULL);
    //return (EGLINativeDisplayType)GetDC(NULL);
    //return GetWindowDC(NULL);
    return 0;
    }

void CEGLOs::DestroyDefaultDisplay( EGLINativeDisplayType display )
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    // no-op
#else
    DeleteDC( display );
#endif
    }

EGLINativeWindowType CEGLOs::CreateNativeWindow( int width, int height )
    {
    WNDCLASS wndclass;
    wndclass.style		   = 0;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance	   = (HINSTANCE)GetModuleHandle(NULL);
    wndclass.hIcon		   = LoadIcon(wndclass.hInstance, MAKEINTRESOURCE(101));
    wndclass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = "EGLMainWndClass";
    if (!wndclass.hIcon)
        wndclass.hIcon = LoadIcon(NULL, IDI_EXCLAMATION);
    
    // \todo We should store this class somewhere
    RegisterClass(&wndclass);

    EGLINativeWindowType window = CreateWindow(
        "EGLMainWndClass",
        "Simulator EGL window",
        WS_OVERLAPPEDWINDOW,
        0, 0,
        width /*+ 2*xBorder*/,
        height /*+ 2*yBorder + caption*/,
        NULL,
        NULL,
        (HINSTANCE)GetModuleHandle(NULL),
        NULL);
    return window;
    }

void CEGLOs::DestroyNativeWindow( EGLINativeWindowType wnd )
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if( wnd )
        EGLI_ASSERT( DestroyWindow( wnd ) );
    // \todo Should use stored WNDCLASS to unregister, see CEGLOs::CreateNativeWindow()
    UnregisterClass( "EGLMainWndClass", (HINSTANCE)GetModuleHandle(NULL) );
#endif
    }

bool CEGLOs::IsValidNativeDisplay( EGLINativeDisplayType dsp )
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    // we can't validate symbian display
    return true;
#else
    // Check if we have invalid handle or handle is not display
    bool ret = true;
    DWORD err = GetLastError();
    SetLastError( ERROR_SUCCESS );
    int technology = GetDeviceCaps( dsp, TECHNOLOGY );
    if( GetLastError() == ERROR_INVALID_HANDLE || technology != DT_RASDISPLAY )
        {
        ret = false;
        }
    SetLastError( err );
    return ret;
#endif
    }

bool CEGLOs::IsValidNativeWindow( EGLINativeWindowType wnd )
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    // We can't validate symbian window
    return true;
#else
    bool ret = false;
    if( wnd )
        {
        // \todo should we skip thread id check?
        DWORD processId = GetCurrentProcessId();
        DWORD threadId = GetCurrentThreadId();
        DWORD windowProcessId;
        DWORD windowThreadId = GetWindowThreadProcessId( wnd, &windowProcessId );
        ret = ((threadId == windowThreadId) && (processId == windowProcessId));
        }
    return ret;
#endif //EGLI_USE_SIMULATOR_EXTENSIONS
    }

bool CEGLOs::GetNativeWindowSize( EGLINativeWindowType wnd, int& width, int& height )
    {
#if defined( EGLI_USE_SIMULATOR_EXTENSIONS )
    // Window size is transferred through eglSimulatorSetSurfaceParameters()
    return true;

#else
    bool ret = false;
    RECT rect;
    ret = !!GetClientRect( wnd, &rect );
    if( ret )
        {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
        }
    return ret;
#endif
    }

EGLIOsWindowContext* CEGLOs::CreateOSWindowContext( EGLINativeWindowType wnd, const CEGLConfig& config )
    {
    EGLIOsWindowContext* ctx = NULL;
    ctx = EGLI_NEW EGLIOsWindowContext;
    if( !ctx )
        {
        return NULL;
        }
    ctx->window = wnd;
    ctx->vgDisplay = 0;
    ctx->glesDisplay = 0;
    ctx->pixmap = NULL;
    ctx->osBuffer = NULL;
    ctx->width = 0;
    ctx->height = 0;
    ctx->colorBuf = NULL;
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    // no-op
#else
    if( ctx->window )
        {
        EGLint renderableType = config.GetAttribute( EGL_RENDERABLE_TYPE );
        if( renderableType & EGL_OPENVG_BIT )
            {
            HDC winDC = GetDC(ctx->window);
            ctx->vgDisplay = CreateCompatibleDC(winDC);
            ReleaseDC(ctx->window, winDC);
            if(!ctx->vgDisplay)
                {
                delete ctx;
                return NULL;
                }
            }
        if( renderableType & EGL_OPENGL_ES_BIT )
            {
            HDC winDC = GetDC(ctx->window);
            ctx->glesDisplay = winDC;
            if(!ctx->glesDisplay)
                {
                delete ctx;
                return NULL;
                }
            PIXELFORMATDESCRIPTOR pfd;
            CEGLOs::ConfigToNativePixelFormat( config, &pfd );

	        int pixelFormat = ChoosePixelFormat( ctx->glesDisplay, &pfd );
	        if( !pixelFormat )
		        {
                EGLI_ASSERT( false );
		        }
	        if( !SetPixelFormat(ctx->glesDisplay, pixelFormat, &pfd ) )
		        {
		        EGLI_ASSERT( false );
		        }
            }
        }
#endif
    return ctx;
    }

void CEGLOs::DestroyOSWindowContext( EGLIOsWindowContext* context )
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if(!context->vgDisplay)
        {
        SelectObject(context->vgDisplay, NULL);
        DeleteDC(context->vgDisplay);
        }
    if( context->glesDisplay )
        {
        ReleaseDC( context->window, context->glesDisplay );
        }
    if( context->pixmap )
        DeleteObject( context->pixmap );
    if( context->colorBuf )
        free( context->colorBuf );
    delete context;
#else
    delete context;
#endif
    }

void CEGLOs::BlitToOSWindow( EGLenum api, CEGLDisplay* display, EGLIOsWindowContext* context, void* buf, int width, int height, int stride )
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    // \todo GLES
    CEGLState* state = getState();
    if( !state )
        return;
    CEGLThread* thread = state->GetCurrentProcess()->CurrentThread();
    switch( api )
        {
        case EGL_OPENVG_API:
            {
            if( thread && state->VGInterface() )
                {                
                if( thread->CurrentVGSurface() )
                    {
                    CEGLSurface* surface = thread->CurrentVGSurface();
                    EGLI_ASSERT( surface->Type() == CEGLSurface::WINDOW_SURFACE );
                    state->VGInterface()->CopyBuffers( buf, -stride, surface->VGClientSurface() );
                    }
                }
            break;
            }
        case EGL_OPENGL_ES_API:
            {
                if (state->GLESInterface(thread->CurrentGLESContext()->ClientVersion()))
                {
                // \todo
                }
            break;
            }
        }
#else
    if( api == EGL_OPENGL_ES_API )
        {
        SwapBuffers( context->glesDisplay );
        }
    else
        {
        doBlit(context, buf, width, height, stride);
        }
#endif
    }

EGLILibraryHandle CEGLOs::LoadHostGL()
    {
    return LoadLibrary( TEXT("opengl32.dll") );
    }

void* CEGLOs::GetGLProcAddress(EGLILibraryHandle& libraryHandle, const char* proc)
    {
    void* addr = (void*)GetProcAddress(libraryHandle, proc);
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
	if(!addr)
		{
		addr = (void*)wglGetProcAddress(proc);
		}
#endif

	return addr;
    }

bool CEGLOs::FreeClientLibrary(EGLILibraryHandle& libraryHandle)
    {
    // != 0 added to expression below because of
    // warning C4800: 'BOOL' : forcing value to bool 'true' or 'false' (performance warning)
    return (FreeLibrary( libraryHandle ) != 0);
    }

IEGLtoVGInterface* CEGLOs::LoadVGInterface( EGLILibraryHandle& libraryHandle )
    {
    IEGLtoVGInterface* iFace = NULL;
    fpGetVGInterface proc;
    libraryHandle = LoadLibrary( TEXT("libOpenVG.dll") );
    if( libraryHandle != NULL )
        {
        proc = (fpGetVGInterface)GetProcAddress( libraryHandle, "getVGInterface" );
        if( proc != NULL )
            {
            iFace = proc();
            }
        }
    return iFace;
    }

IEGLtoGLESInterface* CEGLOs::LoadGLES1Interface( EGLILibraryHandle& libraryHandle )
    {
    IEGLtoGLESInterface* iFace = NULL;
    fpGetGLES1Interface proc;
    libraryHandle = LoadLibrary( TEXT("libGLESv1.dll") );
    if( libraryHandle != NULL )
        {
        proc = (fpGetGLES1Interface)GetProcAddress( libraryHandle, "getGLESInterface" );
        if( proc != NULL )
            {
            iFace = proc();
            }
        }
    return iFace;
    }

IEGLtoGLES2Interface* CEGLOs::LoadGLES2Interface( EGLILibraryHandle& libraryHandle )
    {
    IEGLtoGLES2Interface* iFace = NULL;
    fpGetGLES2Interface proc;
    libraryHandle = LoadLibrary( TEXT("libGLESv2.dll") );
    if( libraryHandle != NULL )
        {
        proc = (fpGetGLES2Interface)GetProcAddress( libraryHandle, "getGLES2Interface" );
        if( proc != NULL )
            {
            iFace = proc();
            }
        }
    return iFace;
    }
