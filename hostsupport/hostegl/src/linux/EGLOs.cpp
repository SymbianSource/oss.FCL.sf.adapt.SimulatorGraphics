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

#include <dlfcn.h>

void CEGLOs::InitializeLock( EGLI_LOCK *lock )
    {
    // \todo
    EGLI_ASSERT( false );
    }

void CEGLOs::GetLock( EGLI_LOCK *lock )
    {
    // \todo
    EGLI_ASSERT( false );
    }

void CEGLOs::ReleaseLock(EGLI_LOCK *lock )
    {
    // \todo
    EGLI_ASSERT( false );
    }

void CEGLOs::DestroyLock( EGLI_LOCK *lock )
    {
    // \todo
    EGLI_ASSERT( false );
    }

void CEGLOs::ConfigToNativePixelFormat( const CEGLConfig& config, EGLINativePixelFormatType* nativeFormat )
    {
    // \todo
    EGLI_ASSERT( false );
    }

/*static*/ EGLINativeContextType CEGLOs::CreateNativeContext( const CEGLConfig& config, EGLINativeDisplayType display, EGLINativeContextType shareContext )
    {
    // \todo
    EGLI_ASSERT( false );
    return NULL;
    }

EGLINativeContextType CEGLOs::CurrentNativeContext()
    {
    // \todo
    EGLI_ASSERT( false );
    return NULL;
    }

EGLINativeDisplayType CEGLOs::CurrentNativeSurface()
    {
    // \todo
    EGLI_ASSERT( false );
    return NULL;
    }

bool CEGLOs::MakeNativeContextCurrent( struct EGLINativeGLFunctions* func, EGLINativeDisplayType draw, EGLINativeDisplayType read, EGLINativeContextType context )
    {
    // \todo
    EGLI_ASSERT( false );
    return false;
    }

bool CEGLOs::DestroyNativeContext( EGLINativeContextType context )
    {
    // \todo
    EGLI_ASSERT( false );
    return false;
    }

bool CEGLOs::InitializeNativeGLFunctions( struct EGLINativeGLFunctions* func, EGLINativeDisplayType display, EGLINativeContextType context )
    {
    // \todo
    EGLI_ASSERT( false );
    return false;
    }

struct EGLINativePbufferContainer* CEGLOs::CreateNativePbuffer( EGLINativeDisplayType display,
        const CEGLConfig& config, EGLint width, EGLint height, EGLBoolean largestPbuffer,
        EGLint textureFormat, EGLint textureTarget )
    {
    // \todo
    EGLI_ASSERT( false );
    return NULL;
    }

bool CEGLOs::DestroyNativePbuffer( struct EGLINativePbufferContainer* container )
    {
    // \todo
    EGLI_ASSERT( false );
    return false;
    }

EGLINativeWindowType CEGLOs::CreateNativeWindow( int width, int height )
    {
    // \todo
    EGLI_ASSERT( false );
    return NULL;
    }

bool CEGLOs::IsValidNativeWindow( EGLINativeWindowType wnd )
    {
    // \todo
    EGLI_ASSERT( false );
    return false;
    }

bool CEGLOs::GetNativeWindowSize( EGLINativeWindowType wnd, int& width, int& height )
    {
    // \todo
    EGLI_ASSERT( false );
    return false;
    }

EGLIOsWindowContext* CEGLOs::CreateOSWindowContext( EGLINativeWindowType wnd, const CEGLConfig& config )
    {
    // \todo
    EGLI_ASSERT( false );
    return NULL;
    }

void CEGLOs::DestroyOSWindowContext( EGLIOsWindowContext* context )
    {
    // \todo
    EGLI_ASSERT( false );
    }

void CEGLOs::BlitToOSWindow( EGLenum api, CEGLDisplay* display, EGLIOsWindowContext* context, void* buf, int w, int h, int stride )
    {
    // \todo
    EGLI_ASSERT( false );
    }

void* CEGLOs::LoadHostGL()
    {
    EGLI_ASSERT(0);
    return NULL;
    }

void* CEGLOs::GetGLProcAddress(EGLILibraryHandle& libraryHandle, const char* proc)
    {
    EGLI_ASSERT(0);
    return NULL;
    }

bool CEGLOs::FreeClientLibrary(EGLILibraryHandle& libraryHandle)
    {
    //assert(libraryHandle);
    return (dlclose(libraryHandle) == 0);
    }

IEGLtoVGInterface* CEGLOs::LoadVGInterface( void*& libraryHandle )
    {
    fpGetVGInterface proc;
    void* libHandle = dlopen("libopenvg_sf.so", RTLD_NOW);

    if (libHandle == NULL)
        return NULL;

    libraryHandle = libHandle;
    proc = (fpGetVGInterface)dlsym(libHandle, "getVGInterface");

    if (!proc)
        return NULL;

    return proc();
    }

IEGLtoGLES2Interface* CEGLOs::LoadGLES2Interface( void*& libraryHandle )
    {
    EGLI_ASSERT( false );
    return NULL;
    }
