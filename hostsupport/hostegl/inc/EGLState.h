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

#ifndef _EGLSTATE_H_
#define _EGLSTATE_H_

#include <EGL/egl.h>
#include "eglInternal.h"
#include <vector>

class CEGLDisplay;
class CEGLProcess;
class CEGLConfig;
class CEGLSurface;
struct SurfaceDescriptor;

class CEGLState : public RefCountingObject, public IVGtoEGLInterface, public IGLEStoEGLInterface
{
public:
	CEGLState(void);
	~CEGLState(void);

public:
    bool Initialize();
    EGLint SupportedApis() const { return m_supportedApis; }
    inline bool IsInitialized() const { return m_initialized; }
    CEGLDisplay* AddDisplay( EGLINativeDisplayType nativeType, EGLint processId );
    void RemoveDisplay( EGLDisplay displayId );
    CEGLDisplay* GetDisplayByNativeType( EGLINativeDisplayType nativeType, EGLint processId ) const;
    CEGLDisplay* GetDisplay( EGLDisplay displayId ) const;
	EGLINativeDisplayType DefaultDisplay() const { return m_defaultDisplay; }

    CEGLProcess* AddProcess( EGLint processId, bool setCurrent);
    void RemoveProcess( EGLint processId );
    CEGLProcess* GetProcess( EGLint processId ) const ;
    inline CEGLProcess* GetCurrentProcess() const { return m_currentProcess; }
    void SetCurrentProcessThread( EGLint processId, EGLI_THREAD_ID threadId );

    inline EGLint ConfigCount() const { return m_configs.size(); }
    void FillConfigs( EGLConfig* configs, EGLint size ) const;
    CEGLConfig* FindConfigById( EGLint id ) const;
    EGLint MatchConfigs( CEGLConfig* filter, EGLConfig* configs, EGLint maxCount ) const;
    CEGLConfig* GetConfig( EGLConfig config ) const;
    
    inline IEGLtoVGInterface* VGInterface() const { return m_VGInterface; }
	IEGLtoGLESInterface* GLESInterface( EGLint clientVersion ) const;

	bool SyncSurface( EGLenum api, EGLint apiVersion, CEGLSurface* surface, CEGLSurface* currentGLESReadSurface );

public: // from IVGtoEGLInterface and IGLEStoEGLInterface
	void GetDescForImage( void* image, SurfaceDescriptor& ImageDesc );
    void* GetDataForImage( void* image );
	void RegisterImageTarget( void* image, EImageTarget target, void* buffer );
	void UnregisterImageTarget( void* image, EImageTarget target, void* buffer );
	void UpdateImageSiblings( void* image, EImageTarget target, void* buffer );

public: // from IVGtoEGLInterface
    void* GetVGContext();
    bool IsImageInUse( void* image );
    bool LockVGSurface( bool read, bool write );
    bool UnlockVGSurface();

public: // from IGLEStoEGLInterface
	void* GetGLESContext();
	void* GetHostProcAddress(const char* proc);
    bool LockGLESSurface( bool read, bool write );
    bool UnlockGLESSurface();
	void ReleaseTexImage(void* surface, int name, int level);

private:
    bool CreateConfigs();
    bool LockSurface( EGLenum api, bool read, bool write );
    bool UnlockSurface( EGLenum api );

private:
    bool                        m_initialized;
    std::vector<CEGLDisplay*>   m_displays;
    std::vector<CEGLProcess*>   m_processes;
    std::vector<CEGLConfig*>    m_configs;
    CEGLProcess*                m_currentProcess;
    EGLILibraryHandle           m_VGLib;
    IEGLtoVGInterface*          m_VGInterface;
    EGLILibraryHandle           m_GLES1Lib;
    IEGLtoGLESInterface*        m_GLES1Interface;
    EGLILibraryHandle           m_GLES2Lib;
    IEGLtoGLES2Interface*       m_GLES2Interface;
	EGLILibraryHandle			m_hostGL;
    EGLint                      m_supportedApis;
	EGLINativeDisplayType       m_defaultDisplay;
    EGLINativeWindowType        m_dummyWindow;
};
#endif //_EGLSTATE_H_
