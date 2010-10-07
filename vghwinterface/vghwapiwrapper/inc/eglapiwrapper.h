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
// 

#ifndef EGLAPIWRAPPER_H
#define EGLAPIWRAPPER_H

#ifndef PSU_PLATFORMTYPES_H
#include "platformtypes.h"
#endif

#include <map>

#include "eglrfc.h"
#include "apiwrapper.h"
#include "egltypes.h"

class KhronosAPIWrapper;


struct TSurfaceKey
{
public:
	int iDpy;
	int iSurfaceId;
	TUint64 iProcId;
public:
	TSurfaceKey( int aDpy, int aSurfaceId, TUint32 aProcId ) : iDpy( aDpy ), iSurfaceId( aSurfaceId ), iProcId( aProcId )
	{
	}

	void set( int aDpy, int aSurfaceId, TUint32 aProcId )
	{
		iDpy = aDpy; iSurfaceId = aSurfaceId; iProcId = aProcId;
	}

	bool operator<(const TSurfaceKey& aOtherKey) const
	{ 
		if( iDpy != aOtherKey.iDpy )					{ return iDpy < aOtherKey.iDpy; }
		else if( iSurfaceId != aOtherKey.iSurfaceId )	{ return iSurfaceId < aOtherKey.iSurfaceId; }
		else if( iProcId!= aOtherKey.iProcId )			{ return iProcId < aOtherKey.iProcId; }
		else {return false; }
	}
};

struct TSurfaceData
{
public:
	SymbianWindow* iWindow;
	//sgImage-related data here later on
public:
	TSurfaceData( SymbianWindow* aWindow ) : iWindow( aWindow )
	{
	}
};
/**
* This maps surfaces to their native symbian windows, and other metadata
*
*/
typedef std::map< TSurfaceKey, TSurfaceData > TSurfaceMap;

class MGraphicsVHWCallback;

class EGLAPIWrapper: public APIWrapper
{
public:

    EGLAPIWrapper( RemoteFunctionCallData& currentFunctionCallData, APIWrapperStack* stack,
	        void* result,
			MGraphicsVHWCallback* serviceIf,
			void* surfacebuffer, KhronosAPIWrapper* aAPIWrapper );

    int WriteReply();
    int DispatchRequest( unsigned long aCode );
	void SetProcessInformation( TUint32 aProcess, TUint32 aThread );
	void Cleanup( TUint32 aProcess, TUint32 aThread );

private:

	int eglGetError();
    int eglGetDisplay();
    int eglInitialize();
    int eglTerminate();
    int eglQueryString();
    int eglGetConfigs();
    int eglChooseConfig();
    int eglGetConfigAttrib();
    int eglCreateWindowSurface();
    int eglCreatePbufferSurface();
    int eglCreatePixmapSurface();
    int eglDestroySurface();
    int eglQuerySurface();
    int eglBindAPI();
    int eglQueryAPI();
    int eglWaitClient();
    int eglReleaseThread();
    int eglCreatePbufferFromClientBuffer();
    int eglSurfaceAttrib();
    int eglBindTexImage();
    int eglReleaseTexImage();
    int eglSwapInterval();
    int eglCreateContext();
    int eglDestroyContext();
    int eglMakeCurrent();
    int eglGetCurrentContext();
    int eglGetCurrentSurface();
    int eglGetCurrentDisplay();
    int eglQueryContext();
    int eglWaitGL();
    int eglWaitNative();
    int eglSwapBuffers();
    int eglCopyBuffers();
    int eglSimulatorSetSurfaceParams();
	int eglSimulatorCopyImageData();
	int eglPixmapSurfaceSizeChanged();

private://Meta calls
	int eglMetaGetConfigs();

private:
	int m_lastEglError;
	EglRFC m_currentFunctionCall;
	void* m_surfaceBuffer;
	TSurfaceMap m_windowMap;
	KhronosAPIWrapper* m_APIWrapper;
};
#endif
