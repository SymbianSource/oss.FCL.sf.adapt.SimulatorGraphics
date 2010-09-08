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
// Guest egl implementation debugging support

#include <e32debug.h>
#include "eglapi.h"


#ifdef _DEBUG

// These functions help with verbose debug logging of EGL Attibutes

// Try to get an attribute name string, and a description of the value - switch statements can be extended for later EGL versions
void TAttribUtils::TraceAttribNames(EGLint aAttrib, EGLint aValue, char** aAttrName, char** aValueName)
	{
	char* attrName = NULL;
	char* valueName = NULL;
	switch (aAttrib)
		{
		case EGL_BUFFER_SIZE: attrName = "EGL_BUFFER_SIZE"; break;
		case EGL_ALPHA_SIZE: attrName = "EGL_ALPHA_SIZE"; break;
		case EGL_BLUE_SIZE: attrName = "EGL_BLUE_SIZE"; break;
		case EGL_GREEN_SIZE: attrName = "EGL_GREEN_SIZE"; break;
		case EGL_RED_SIZE: attrName = "EGL_RED_SIZE"; break;
		case EGL_DEPTH_SIZE: attrName = "EGL_DEPTH_SIZE"; break;
		case EGL_STENCIL_SIZE: attrName = "EGL_STENCIL_SIZE"; break;
		case EGL_CONFIG_CAVEAT: attrName = "EGL_CONFIG_CAVEAT";
			switch (aValue)
				{
				// Config attribute values
				case EGL_SLOW_CONFIG: valueName = "EGL_SLOW_CONFIG"; break;
				case EGL_NON_CONFORMANT_CONFIG: valueName = "EGL_NON_CONFORMANT_CONFIG"; break;
				}
			break;
		case EGL_CONFIG_ID: attrName = "EGL_CONFIG_ID"; break;
		case EGL_LEVEL: attrName = "EGL_LEVEL"; break;
		case EGL_MAX_PBUFFER_HEIGHT: attrName = "EGL_MAX_PBUFFER_HEIGHT"; break;
		case EGL_MAX_PBUFFER_PIXELS: attrName = "EGL_MAX_PBUFFER_PIXELS"; break;
		case EGL_MAX_PBUFFER_WIDTH: attrName = "EGL_MAX_PBUFFER_WIDTH"; break;
		case EGL_NATIVE_RENDERABLE: attrName = "EGL_NATIVE_RENDERABLE"; break;
		case EGL_NATIVE_VISUAL_ID: attrName = "EGL_NATIVE_VISUAL_ID"; break;
		case EGL_NATIVE_VISUAL_TYPE: attrName = "EGL_NATIVE_VISUAL_TYPE"; break;
		case EGL_PRESERVED_RESOURCES: attrName = "EGL_PRESERVED_RESOURCES"; break;
		case EGL_SAMPLES: attrName = "EGL_SAMPLES"; break;
		case EGL_SAMPLE_BUFFERS: attrName = "EGL_SAMPLE_BUFFERS"; break;
		case EGL_SURFACE_TYPE: attrName = "EGL_SURFACE_TYPE";
			switch(aValue & 7)
				{
				case 0: valueName = "* | EGL_NONE"; break;
				case 1: valueName = "* | PBUFFER"; break; // EGL_PBUFFER_BIT
				case 2: valueName = "* | PIXMAP"; break; // EGL_PIXMAP_BIT
				case 3: valueName = "* | PIXMAP | PBUFFER"; break;
				case 4: valueName = "* | WINDOW"; break; // EGL_WINDOW_BIT
				case 5: valueName = "* | WINDOW | PBUFFER"; break;
				case 6: valueName = "* | WINDOW | PIXMAP"; break;
				case 7: valueName = "* | WINDOW | PIXMAP | PBUFFER"; break;
				}
			break;
		case EGL_TRANSPARENT_TYPE: attrName = "EGL_TRANSPARENT_TYPE";
			if (aValue == EGL_TRANSPARENT_RGB)
				valueName = "EGL_TRANSPARENT_RGB";
			break;
		case EGL_TRANSPARENT_BLUE_VALUE: attrName = "EGL_TRANSPARENT_BLUE_VALUE"; break;
		case EGL_TRANSPARENT_GREEN_VALUE: attrName = "EGL_TRANSPARENT_GREEN_VALUE"; break;
		case EGL_TRANSPARENT_RED_VALUE: attrName = "EGL_TRANSPARENT_RED_VALUE"; break;
		case EGL_BIND_TO_TEXTURE_RGB: attrName = "EGL_BIND_TO_TEXTURE_RGB"; break;
		case EGL_BIND_TO_TEXTURE_RGBA: attrName = "EGL_BIND_TO_TEXTURE_RGBA"; break;
		case EGL_MIN_SWAP_INTERVAL: attrName = "EGL_MIN_SWAP_INTERVAL"; break;
		case EGL_MAX_SWAP_INTERVAL: attrName = "EGL_MAX_SWAP_INTERVAL"; break;
		case EGL_LUMINANCE_SIZE: attrName = "EGL_LUMINANCE_SIZE"; break;
		case EGL_ALPHA_MASK_SIZE: attrName = "EGL_ALPHA_MASK_SIZE"; break;
		case EGL_COLOR_BUFFER_TYPE: attrName = "EGL_COLOR_BUFFER_TYPE";
			switch (aValue)
				{
				// EGL_COLOR_BUFFER_TYPE values
				case EGL_RGB_BUFFER: valueName = "EGL_RGB_BUFFER"; break;
				case EGL_LUMINANCE_BUFFER: valueName = "EGL_LUMINANCE_BUFFER"; break;
				}
			break;
		case EGL_RENDERABLE_TYPE: attrName = "EGL_RENDERABLE_TYPE";
			switch(aValue)
				{
				case 0: valueName = "EGL_NONE"; break;
				case 1: valueName = "GL-ES"; break;
				case 2: valueName = "VG"; break;
				case 3: valueName = "VG | GL-ES"; break;
				case 4: valueName = "GL-ES2"; break;
				case 5: valueName = "GL-ES2 | GL-ES"; break;
				case 6: valueName = "GL-ES2 | VG"; break;
				case 7: valueName = "GL-ES2 | VG | GL-ES"; break;
				case 8: valueName = "GL"; break;
				case 9: valueName = "GL | GL-ES"; break;
				case 10: valueName = "GL | VG"; break;
				case 11: valueName = "GL | VG | GL-ES"; break;
				case 12: valueName = "GL | GL-ES2"; break;
				case 13: valueName = "GL | GL-ES2 | GL-ES"; break;
				case 14: valueName = "GL | GL-ES2 | VG"; break;
				case 15: valueName = "GL | GL-ES2 | VG | GL-ES"; break;
				}
			break;
		case EGL_MATCH_NATIVE_PIXMAP: attrName = "EGL_MATCH_NATIVE_PIXMAP"; break; // Pseudo-attribute (not queryable)
		case EGL_CONFORMANT: attrName = "EGL_CONFORMANT"; break;
		case EGL_HEIGHT: attrName = "EGL_HEIGHT"; break;
		case EGL_WIDTH: attrName = "EGL_WIDTH"; break;
		case EGL_LARGEST_PBUFFER: attrName = "EGL_LARGEST_PBUFFER"; break;
		case EGL_TEXTURE_FORMAT: attrName = "EGL_TEXTURE_FORMAT";
			switch (aValue)
				{
				// config attribute values, for EGL_TEXTURE_FORMAT
				case EGL_NO_TEXTURE: valueName = "EGL_NO_TEXTURE"; break;
				case EGL_TEXTURE_RGB: valueName = "EGL_TEXTURE_RGB"; break;
				case EGL_TEXTURE_RGBA: valueName = "EGL_TEXTURE_RGBA"; break;
				case EGL_TEXTURE_2D: valueName = "EGL_TEXTURE_2D"; break;
				}
			break;
		case EGL_TEXTURE_TARGET: attrName = "EGL_TEXTURE_TARGET"; break;
		case EGL_MIPMAP_TEXTURE: attrName = "EGL_MIPMAP_TEXTURE"; break;
		case EGL_MIPMAP_LEVEL: attrName = "EGL_MIPMAP_LEVEL"; break;
		case EGL_RENDER_BUFFER: attrName = "EGL_RENDER_BUFFER";
			switch (aValue)
				{
				// EGL_RENDER_BUFFER values / BindTexImage / ReleaseTexImage buffer targets
				case EGL_BACK_BUFFER: valueName = "EGL_BACK_BUFFER"; break;
				case EGL_SINGLE_BUFFER: valueName = "EGL_SINGLE_BUFFER"; break;
				}
			break;
		case EGL_VG_COLORSPACE: attrName = "EGL_VG_COLORSPACE";
			switch (aValue)
				{
				// OpenVG color spaces
				case EGL_VG_COLORSPACE_sRGB: valueName = "EGL_VG_COLORSPACE_sRGB"; break;
				case EGL_VG_COLORSPACE_LINEAR: valueName = "EGL_VG_COLORSPACE_LINEAR"; break;
				}
			break;
		case EGL_VG_ALPHA_FORMAT: attrName = "EGL_VG_ALPHA_FORMAT";
			switch (aValue)
				{
				// OpenVG alpha formats
				case EGL_VG_ALPHA_FORMAT_NONPRE: valueName = "EGL_VG_ALPHA_FORMAT_NONPRE"; break;
				case EGL_VG_ALPHA_FORMAT_PRE: valueName = "EGL_VG_ALPHA_FORMAT_PRE"; break;
				}
			break;
		case EGL_HORIZONTAL_RESOLUTION: attrName = "EGL_HORIZONTAL_RESOLUTION"; break;
		case EGL_VERTICAL_RESOLUTION: attrName = "EGL_VERTICAL_RESOLUTION"; break;
		case EGL_PIXEL_ASPECT_RATIO: attrName = "EGL_PIXEL_ASPECT_RATIO"; break;
		case EGL_SWAP_BEHAVIOR: attrName = "EGL_SWAP_BEHAVIOR"; break;
		case EGL_MULTISAMPLE_RESOLVE: attrName = "EGL_MULTISAMPLE_RESOLVE"; break;
		default: // unknown value - maybe buggy data or a later EGL version.  Debug prints include the hex anyway.
			break;
		}

	if (aValue == EGL_NONE)
		valueName = "EGL_NONE";
	if (aAttrib == EGL_NONE)
		attrName = "EGL_NONE";

	*aAttrName = attrName;
	*aValueName = valueName;
	}

// verbose debug logging of EGL Attibute Lists
void TAttribUtils::TraceAttribList(const EGLint* aAttribList)
    {
    int length = 0;
    if (aAttribList)
        {
		while (aAttribList[length] != EGL_NONE)
			{
			char* attrName = NULL;
			char* valueName = NULL;
			// try to get an attribute name string, and a description of the value
			TraceAttribNames(aAttribList[length], aAttribList[length+1], &attrName, &valueName);
			// print attribute & value as numbers, and also print a string name/description if one could be determined  
			if (attrName)
				{
				if (valueName)
					{
					EGL_TRACE("  TraceAttribList aAttribList[%d]=0x%x (\"%s\"), aAttribList[%d]=0x%x (\"%s\")",
							length, aAttribList[length], attrName, length+1, aAttribList[length+1], valueName);
					}
				else
					{
					EGL_TRACE("  TraceAttribList aAttribList[%d]=0x%x (\"%s\"), aAttribList[%d]=0x%x (%d)",
							length, aAttribList[length], attrName, length+1, aAttribList[length+1], aAttribList[length+1]);
					}
				}
			else
				{
				EGL_TRACE("  TraceAttribList aAttribList[%d]=0x%x, aAttribList[%d]=0x%x (%d)",
						length, aAttribList[length], length+1, aAttribList[length+1], aAttribList[length+1]);			
				}
			length += 2;
			}
        ++length;
        }
    }

// verbose debug logging of EGL Get Attibute fn
void TAttribUtils::TraceGetAttrib(char* aApiName, char* aObjType, EGLDisplay aDisplay, EGLint aObject, EGLint aAttribute, EGLint *aValue, EGLBoolean aResult)
	{
	char* attrName = NULL;
	char* valueName = NULL;
	if (aResult)
		{
		// try to get an attribute name string, and a description of the value
		TraceAttribNames(aAttribute, *aValue, &attrName, &valueName);
		// print attribute & value as numbers, and also print a string name/description if one could be determined  
		if (attrName)
			{
			if (valueName)
				{
				EGL_TRACE("%s succeeded: aDisplay=%d, %s=%d,  aAttrib=0x%x (\"%s\"), aValue=0x%x (\"%s\")",
						aApiName, aDisplay, aObjType, aObject, aAttribute, attrName, *aValue, valueName);			
				}
			else
				{
				EGL_TRACE("%s succeeded: aDisplay=%d, %s=%d,  aAttrib=0x%x (\"%s\"), aValue=0x%x (%d)",
						aApiName, aDisplay, aObjType, aObject, aAttribute, attrName, *aValue, *aValue);			
				}
			}
		else
			{
			EGL_TRACE("%s succeeded: aDisplay=%d, %s=%d,  aAttrib=0x%x, aValue=0x%x (%d)",
					aApiName, aDisplay, aObjType, aObject, aAttribute, *aValue, *aValue);			
			}
		}
	else
		{
		// try to get an attribute name string
		TraceAttribNames(aAttribute, 0, &attrName, &valueName);
		if (attrName)
			{
			EGL_TRACE("%s failed: aDisplay=%d, %s=%d, attribute=0x%x (\"%s\")",
					aApiName, aDisplay, aObjType, aObject, aAttribute, attrName);
			}
		else
			{
			EGL_TRACE("%s failed: aDisplay=%d, %s=%d, attribute=0x%x",
					aApiName, aDisplay, aObjType, aObject, aAttribute);
			}
		}
	}


// verbose debug logging of EGL Get Attibute fn
void TAttribUtils::TraceSetAttrib(char* aApiName, char* aObjType, EGLDisplay aDisplay, EGLint aObject, EGLint aAttribute, EGLint aValue)
	{
	char* attrName = NULL;
	char* valueName = NULL;
	// try to get an attribute name string, and a description of the value
	TraceAttribNames(aAttribute, aValue, &attrName, &valueName);
	// print attribute & value as numbers, and also print a string name/description if one could be determined  
	if (attrName)
		{
		if (valueName)
			{
			EGL_TRACE("%s: aDisplay=%d, %s=%d,  aAttrib=0x%x (\"%s\"), aValue=0x%x (\"%s\")",
					aApiName, aDisplay, aObjType, aObject, aAttribute, attrName, aValue, valueName);			
			}
		else
			{
			EGL_TRACE("%s: aDisplay=%d, %s=%d,  aAttrib=0x%x (\"%s\"), aValue=0x%x (%d)",
					aApiName, aDisplay, aObjType, aObject, aAttribute, attrName, aValue, aValue);			
			}
		}
	else
		{
		EGL_TRACE("%s: aDisplay=%d, %s=%d,  aAttrib=0x%x, aValue=0x%x (%d)",
				aApiName, aDisplay, aObjType, aObject, aAttribute, aValue, aValue);			
		}
	}


#endif

