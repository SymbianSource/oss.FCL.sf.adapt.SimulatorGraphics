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

#ifndef _EGLOS_H_
#define _EGLOS_H_

#include "eglInternal.h"
#include "EGLConfig.h"

class CEGLDisplay;

class CEGLOs
{
public:
    static void InitializeLock( EGLI_LOCK *lock );
    static void GetLock( EGLI_LOCK *lock );
    static void ReleaseLock(EGLI_LOCK *lock );
    static void DestroyLock( EGLI_LOCK *lock );

    static void ConfigToNativePixelFormat( const CEGLConfig& config, EGLINativePixelFormatType* nativeFormat );
    static EGLINativeContextType CreateNativeContext( const CEGLConfig& config, EGLINativeDisplayType display, EGLINativeContextType shareContext );
    static EGLINativeContextType CurrentNativeContext();
    static EGLINativeDisplayType CurrentNativeSurface();
    static bool MakeNativeContextCurrent( struct EGLINativeGLFunctions* func, EGLINativeDisplayType draw, EGLINativeDisplayType read, EGLINativeContextType context );
    static bool DestroyNativeContext( EGLINativeContextType context );
    static bool InitializeNativeGLFunctions( struct EGLINativeGLFunctions* func, EGLINativeDisplayType display, EGLINativeContextType context );
    static struct EGLINativePbufferContainer* CreateNativePbuffer( EGLINativeDisplayType display,
            const CEGLConfig& config, EGLint width, EGLint height, EGLBoolean largestPbuffer, 
            EGLint textureFormat, EGLint textureTarget );
    static bool DestroyNativePbuffer( struct EGLINativePbufferContainer* container );

    static EGLINativeDisplayType CreateDefaultDisplay();
    static void DestroyDefaultDisplay( EGLINativeDisplayType display );
    static EGLINativeWindowType CreateNativeWindow( int width, int height );
    static void DestroyNativeWindow( EGLINativeWindowType wnd );

    static bool IsValidNativeDisplay( EGLINativeDisplayType dsp );
    static bool IsValidNativeWindow( EGLINativeWindowType wnd );
    static bool GetNativeWindowSize( EGLINativeWindowType wnd, int& width, int& height );
    static EGLIOsWindowContext* CreateOSWindowContext( EGLINativeWindowType wnd, const CEGLConfig& config );
    static void DestroyOSWindowContext( EGLIOsWindowContext* context );
    static void BlitToOSWindow( EGLenum api, CEGLDisplay* display, EGLIOsWindowContext* context, void* buf, int width, int height, int stride );

    static EGLILibraryHandle LoadHostGL();
    static void* GetGLProcAddress(EGLILibraryHandle& libraryHandle, const char* proc);
    static bool FreeClientLibrary(EGLILibraryHandle& libraryHandle);

    static IEGLtoVGInterface* LoadVGInterface( EGLILibraryHandle& libraryHandle );
    static IEGLtoGLESInterface* LoadGLES1Interface( EGLILibraryHandle& libraryHandle );
    static IEGLtoGLES2Interface* LoadGLES2Interface( EGLILibraryHandle& libraryHandle );
};

#endif //_EGLOS_H_