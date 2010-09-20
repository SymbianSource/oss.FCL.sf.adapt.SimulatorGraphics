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
// Client-side state information for Open VG handle based objects

#include "vgstate.h"
#include "remotefunctioncall.h"
#include "openvgrfc.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CVgFontInfo
/////////////////////////////////////////////////////////////////////////////////////////////

CVgFontInfo* CVgFontInfo::New()
	{
	RHeap* clientHeap = CVghwUtils::SwitchToVghwHeap();
	CVgFontInfo* self = new CVgFontInfo();
	CVghwUtils::SwitchFromVghwHeap(clientHeap);
	return self;
	}


CVgFontInfo::~CVgFontInfo()
	{}


CVgFontInfo::CVgFontInfo() :
		CVgHandleBase(EVgHandleForFont)
	{}


TBool CVgFontInfo::DestroyObject(MVgContext& aVgContext)
	{
	VGPANIC_ASSERT_DEBUG(iIsDestroyed, EVgPanicTemp);
	OPENVG_TRACE("  CVgFontInfo::DestroyObject HostHandle=0x%x", iHostHandle);

	if (iHostHandle)
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgDestroyFont, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(iHostHandle);
		aVgContext.ExecuteVgCommand(vgApiData);
		}

	return ETrue;
	}


VGint CVgFontInfo::GetParameterVectorSize(MVgContext& aVgContext, VGint aParamType)
	{
	switch (aParamType)
		{
		case VG_FONT_NUM_GLYPHS:
			return 1;
		}

	// invalid ParamType
	aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
	return 0;
	}


void CVgFontInfo::SetGlyphToPath(MVgContext& aVgContext, VGuint aGlyphIndex, CVgPathInfo* aPathInfo, VGboolean aIsHinted,
		const VGfloat aGlyphOrigin [2], const VGfloat aEscapement[2])
	{
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetGlyphToPath, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aGlyphIndex);
	vgApiData.AppendParam( (aPathInfo == NULL) ? VG_INVALID_HANDLE : aPathInfo->HostHandle());
	vgApiData.AppendParam(aIsHinted);
	vgApiData.AppendVector(aGlyphOrigin, 2);
	vgApiData.AppendVector(aEscapement, 2);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgFontInfo::SetGlyphToImage(MVgContext& aVgContext, VGuint aGlyphIndex, CVgImageInfo* aImageInfo, const VGfloat aGlyphOrigin [2],
        const VGfloat aEscapement[2])
	{
	// ToDo track use of VG Images (esp SgImages) as Font Glyphs
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgSetGlyphToImage, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aGlyphIndex);
	vgApiData.AppendParam( (aImageInfo == NULL) ? VG_INVALID_HANDLE : aImageInfo->HostHandle());
	vgApiData.AppendVector(aGlyphOrigin, 2);
	vgApiData.AppendVector(aEscapement, 2);
	TUint64 sgId(0L);
	if (aImageInfo && aImageInfo->IsEglSibling())
		{
		sgId = aImageInfo->SgImageId();
		}
	vgApiData.AppendTUint64(sgId);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgFontInfo::ClearGlyph(MVgContext& aVgContext, VGuint aGlyphIndex)
	{
	// **** Desirable: verify aGlyphIndex, ToDo  decrement SgImage reference count
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgClearGlyph, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aGlyphIndex);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgFontInfo::DrawGlyph(MVgContext& aVgContext, VGuint aGlyphIndex, VGbitfield aPaintModes, VGboolean aAllowAutoHinting)
	{
	// **** Desirable: verify aGlyphIndex
	RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
	vgApiData.Init(OpenVgRFC::EvgDrawGlyph, RemoteFunctionCallData::EOpRequest);
	vgApiData.AppendParam(iHostHandle);
	vgApiData.AppendParam(aGlyphIndex);
	vgApiData.AppendParam(aPaintModes);
	vgApiData.AppendParam(aAllowAutoHinting);
	aVgContext.ExecuteVgCommand(vgApiData);
	}


void CVgFontInfo::DrawGlyphs(MVgContext& aVgContext, VGint aGlyphCount, const VGuint * aGlyphIndices, const VGfloat * aAdjustmentsX,
        const VGfloat * aAdjustmentsY, VGbitfield aPaintModes, VGboolean aAllowAutoHinting)
	{
	// **** Desirable: verify glyph indices?
	if (aGlyphCount <= 0)
		{
		aVgContext.SetVgError(VG_ILLEGAL_ARGUMENT_ERROR);
		}
	else
		{
		RemoteFunctionCallData data; OpenVgRFC vgApiData(data);
		vgApiData.Init(OpenVgRFC::EvgDrawGlyphs, RemoteFunctionCallData::EOpRequest);
		vgApiData.AppendParam(iHostHandle);
		vgApiData.AppendParam(aGlyphCount);
		vgApiData.AppendVector(aGlyphIndices, aGlyphCount);
		vgApiData.AppendVector(aAdjustmentsX, (aAdjustmentsX == NULL) ? 0 : aGlyphCount);
		vgApiData.AppendVector(aAdjustmentsY, (aAdjustmentsY == NULL) ? 0 : aGlyphCount);
		vgApiData.AppendParam(aPaintModes);
		vgApiData.AppendParam(aAllowAutoHinting);
		aVgContext.ExecuteVgCommand(vgApiData);
		}
	}


// end of file vgfont.cpp
