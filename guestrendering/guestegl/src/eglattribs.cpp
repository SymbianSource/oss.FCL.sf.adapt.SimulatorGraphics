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
// Guest Egl implementation of attributes and attribute lists

#include <e32debug.h>
#include "eglapi.h"



TInt TAttribUtils::AttribListLength(const EGLint* aAttribList)
	{
	int length = 0;
	if (aAttribList)
		{
		while (aAttribList[length] != EGL_NONE)
			{
			length += 2;
			EGLPANIC_ASSERT_DEBUG(length < 100, EEglPanicAtribListLengthTooLong);
			}
		++length;
		}
	return length;
	}

const EGLint* TAttribUtils::FindAttribValue(const EGLint* aAttribList, EGLint aAttrib)
	{
	EGLPANIC_ASSERT_DEBUG(aAttrib != EGL_NONE, EEglPanicTemp);

	if (aAttribList && (aAttrib != EGL_NONE) )
		{
		TInt idx = 0;
		while (aAttribList[idx] != EGL_NONE)
			{
			if (aAttribList[idx] == aAttrib)
				{
				return aAttribList + idx + 1;
				}
			idx += 2;
			EGLPANIC_ASSERT_DEBUG(idx < 100, EEglPanicAtribListLengthTooLong);
			}
		}
	return NULL;
	}

EGLint* TAttribUtils::FindAttribValue(EGLint* aAttribList, EGLint aAttrib)
	{
	EGLPANIC_ASSERT_DEBUG(aAttrib != EGL_NONE, EEglPanicTemp);

	if (aAttribList && (aAttrib != EGL_NONE) )
		{
		TInt idx = 0;
		while (aAttribList[idx] != EGL_NONE)
			{
			if (aAttribList[idx] == aAttrib)
				{
				return aAttribList + idx + 1;
				}
			idx += 2;
			EGLPANIC_ASSERT_DEBUG(idx < 100, EEglPanicAtribListLengthTooLong);
			}
		}
	return NULL;
	}

void TAttribUtils::AppendAttribValue(EGLint* aAttribList, EGLint aAttrib, EGLint aValue)
	{
	EGLPANIC_ASSERT_DEBUG(aAttrib != EGL_NONE, EEglPanicTemp);

	if (aAttribList && (aAttrib != EGL_NONE))
		{
		TInt idx = 0;
		while (aAttribList[idx] != EGL_NONE)
			{
			idx += 2;
			EGLPANIC_ASSERT_DEBUG(idx < 100, EEglPanicAtribListLengthTooLong);
			}
		aAttribList[idx++] = aAttrib;
		aAttribList[idx++] = aValue;
		aAttribList[idx++] = EGL_NONE;
		}
	}

void TAttribUtils::RemoveAttrib(EGLint* aAttribList, EGLint aAttrib)
	{
	EGLPANIC_ASSERT_DEBUG(aAttrib != EGL_NONE, EEglPanicTemp);

	if (aAttribList && (aAttrib != EGL_NONE) && (*aAttribList != EGL_NONE))
		{
		TInt dstIdx = 0;

		while ( (aAttribList[dstIdx] != EGL_NONE) && (aAttribList[dstIdx] != aAttrib) )
			{
			dstIdx += 2;
			EGLPANIC_ASSERT_DEBUG(dstIdx < 100, EEglPanicAtribListLengthTooLong);
			}

		if (aAttribList[dstIdx] == aAttrib)
			{ // attrib found
			TInt srcIdx = dstIdx + 2;
			while (aAttribList[srcIdx] != EGL_NONE)
				{ // copy up succeeding attrib / value pairs
				aAttribList[dstIdx++] = aAttribList[srcIdx++];
				aAttribList[dstIdx++] = aAttribList[srcIdx++];
				EGLPANIC_ASSERT_DEBUG(dstIdx < 100, EEglPanicAtribListLengthTooLong);
				}
			aAttribList[dstIdx++] = EGL_NONE;
			}
		}
	}

// end of file eglattrib.cpp
