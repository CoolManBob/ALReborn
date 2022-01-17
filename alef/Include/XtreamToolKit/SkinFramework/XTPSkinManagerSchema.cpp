// XTPSkinManagerSchema.cpp: implementation of the CXTPSkinManagerSchema class.
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Common/Tmschema.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPSkinManagerSchema.h"
#include "XTPSkinManagerResource.h"
#include "XTPSkinManager.h"
#include "XTPSkinImage.h"
#include "XTPSkinObjectFrame.h"
#include "XTPSkinDrawTools.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef OIC_WINLOGO
#define OIC_WINLOGO         32517
#endif

#ifndef LAYOUT_BITMAPORIENTATIONPRESERVED
#define LAYOUT_BITMAPORIENTATIONPRESERVED 0x00000008
#endif

void InflateBorders(CRect& rcBorders, int cxFrame, int cyFrame)
{
	rcBorders.left += cxFrame;
	rcBorders.right +=  cxFrame;

	rcBorders.top += cyFrame;
	rcBorders.bottom +=  cyFrame;
}


CXTPSkinManagerSchemaProperty::CXTPSkinManagerSchemaProperty()
{
	propType = XTP_SKINPROPERTY_UNKNOWN;
	nPropertyCode = 0;
	ZeroMemory(&rcVal, sizeof(RECT));
}

CXTPSkinManagerSchemaProperty::~CXTPSkinManagerSchemaProperty()
{
	ClearProperty();
}

void CXTPSkinManagerSchemaProperty::ClearProperty()
{
	if (propType == XTP_SKINPROPERTY_STRING)
		delete[] lpszVal;

	if (propType == XTP_SKINPROPERTY_FONT)
		delete lfVal;

	propType = XTP_SKINPROPERTY_UNKNOWN;
}

void CXTPSkinManagerSchemaProperty::SetPropertyColor(LPCTSTR lpszValue)
{
	ClearProperty();
	int r = 0, g = 0, b = 0;
	SCANF_S(lpszValue, _T("%i %i %i"), &r, &g, &b);
	clrVal = RGB(r, g, b);
}

void CXTPSkinManagerSchemaProperty::SetPropertyString(LPCTSTR lpszValue)
{
	ClearProperty();

	int nLen = (int)_tcslen(lpszValue);
	lpszVal = new TCHAR[nLen + 1];
	STRCPY_S(lpszVal, nLen + 1, lpszValue);

	propType = XTP_SKINPROPERTY_STRING;
}

void CXTPSkinManagerSchemaProperty::SetPropertyBool(LPCTSTR lpszValue)
{
	ClearProperty();
	bVal = _tcsicmp(lpszValue, _T("TRUE")) == 0;
}

void CXTPSkinManagerSchemaProperty::SetPropertyInt(LPCTSTR lpszValue)
{
	ClearProperty();
	iVal = _ttoi(lpszValue);
}

void CXTPSkinManagerSchemaProperty::SetPropertyEnum(int nEnumValue)
{
	ClearProperty();
	iVal = nEnumValue;
}


void CXTPSkinManagerSchemaProperty::SetPropertySize(LPCTSTR lpszValue)
{
	ClearProperty();
	SCANF_S(lpszValue, _T("%i,%i"), &szVal.cx, &szVal.cy);
}

void CXTPSkinManagerSchemaProperty::SetPropertyRect(LPCTSTR lpszValue)
{
	ClearProperty();
	SCANF_S(lpszValue, _T("%ld,%ld,%ld,%ld"), &rcVal.left, &rcVal.right, &rcVal.top, &rcVal.bottom);
}

void CXTPSkinManagerSchemaProperty::SetPropertyFont(LPCTSTR lpszValue)
{
	ClearProperty();
	lfVal = new LOGFONT;
	ZeroMemory(lfVal, sizeof(LOGFONT));

	CWindowDC dc (NULL);
	lfVal->lfCharSet = (BYTE)::GetTextCharsetInfo (dc, NULL, 0);

	TCHAR chBold[50];
	chBold[0] = 0;
	SCANF_S(lpszValue, _T("%[^,], %ld, %s"), SCANF_PARAM_S(lfVal->lfFaceName, LF_FACESIZE), &lfVal->lfHeight, SCANF_PARAM_S(chBold, 50));
	lfVal->lfWeight = _tcsicmp(chBold, _T("BOLD")) != 0?  FW_NORMAL: FW_BOLD;

	int ppi = dc.GetDeviceCaps(LOGPIXELSY);

	lfVal->lfHeight = - MulDiv(lfVal->lfHeight, ppi, 72);
}

BOOL CXTPSkinManagerSchemaProperty::operator==(const CXTPSkinManagerSchemaProperty& prop) const
{
	if (&prop == this)
		return TRUE;

	if (prop.propType != propType)
		return FALSE;

	switch (propType)
	{
		case XTP_SKINPROPERTY_COLOR:    return clrVal == prop.clrVal;
		case XTP_SKINPROPERTY_STRING:   return _tcsicmp(lpszVal, prop.lpszVal) == 0;
		case XTP_SKINPROPERTY_BOOL:     return bVal == prop.bVal;
		case XTP_SKINPROPERTY_RECT:     return ::EqualRect(&prop.rcVal, &rcVal);
		case XTP_SKINPROPERTY_INT:      return iVal == prop.iVal;
		case XTP_SKINPROPERTY_POSITION: return (szVal.cx == prop.szVal.cx) && (szVal.cy == prop.szVal.cy);
		case XTP_SKINPROPERTY_ENUM:     return iVal == prop.iVal;
		case XTP_SKINPROPERTY_FONT:     return memcmp(lfVal, prop.lfVal, sizeof(LOGFONT)) == 0;
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPSkinManagerSchema::CXTPSkinManagerSchema(CXTPSkinManager* pManager)
	: m_pManager(pManager)
{
	m_bPreMultiplyImages = TRUE;

	m_nGlobalClassId = GetClassCode(_T("GLOBALS"));
	m_nWindowClassId = GetClassCode(_T("WINDOW"));

	m_mapProperties.InitHashTable(127, FALSE);
}

CXTPSkinManagerSchema::~CXTPSkinManagerSchema()
{
	RemoveAllProperties();
}

void CXTPSkinManagerSchema::RemoveAllProperties()
{
	POSITION pos = m_mapProperties.GetStartPosition();
	UINT nProperty;
	CXTPSkinManagerSchemaProperty* pProperty;
	while (pos != NULL)
	{
		m_mapProperties.GetNextAssoc( pos, nProperty, pProperty);
		delete pProperty;
	}
	m_mapProperties.RemoveAll();

}


UINT CXTPSkinManagerSchema::GetClassCode(LPCTSTR lpszClass)
{
	if (_tcsicmp(_T("Globals"), lpszClass) == 0) return 1;
	if (_tcsicmp(_T("Documentation"), lpszClass) == 0) return 2;
	if (_tcsicmp(_T("SysMetrics"), lpszClass) == 0) return 3;
	if (_tcsicmp(_T("ListBox"), lpszClass) == 0) return 4;
	if (_tcsicmp(_T("Dialog"), lpszClass) == 0) return 5;

	UINT nClass = 5;

	#include "XTPSkinManagerSchema.inc"
	#undef BEGIN_TM_CLASS_PARTS
	#define BEGIN_TM_CLASS_PARTS(name)            nClass++; if (_tcsicmp(_T(#name), lpszClass) == 0) return nClass;
	#include "Common/Tmschema.h"

	TRACE(_T("Warning: %s class not supported"), lpszClass);
	return 0; // Not supported
}

int CXTPSkinManagerSchema::GetEnumCode(LPCTSTR lpszProperty, LPCTSTR lpszValue)
{
	#include "XTPSkinManagerSchema.inc"
	#undef BEGIN_TM_ENUM
	#undef TM_ENUM
	#undef END_TM_ENUM
	#define BEGIN_TM_ENUM(name)            if (_tcsicmp(_T(#name), lpszProperty) == 0) {
	#define TM_ENUM(val, prefix, name)     if (_tcsicmp(_T(#name), lpszValue) == 0) return val;
	#define END_TM_ENUM()                      }
	#include "Common/Tmschema.h"

	return -1;
}

UINT CXTPSkinManagerSchema::GetClassPartCode(LPCTSTR lpszClass, LPCTSTR lpszClassPart)
{
	#include "XTPSkinManagerSchema.inc"
	#undef BEGIN_TM_CLASS_PARTS
	#undef TM_PART
	#undef END_TM_CLASS_PARTS
	#define BEGIN_TM_CLASS_PARTS(name)          if (_tcsicmp(_T(#name), lpszClass) == 0) {
	#define TM_PART(val, prefix, name)          if (_tcsicmp(_T(#name), lpszClassPart) == 0) return val;
	#define END_TM_CLASS_PARTS()                }
	#include "Common/Tmschema.h"

	return 0;
}

UINT CXTPSkinManagerSchema::GetStateCode(LPCTSTR lpszClassPart, LPCTSTR lpszState)
{
	#include "XTPSkinManagerSchema.inc"
	#undef BEGIN_TM_PART_STATES
	#undef TM_STATE
	#undef END_TM_PART_STATES
	#define BEGIN_TM_PART_STATES(name)          if (_tcsicmp(_T(#name), lpszClassPart) == 0) {
	#define TM_STATE(val, prefix, name)         if (_tcsicmp(_T(#name), lpszState) == 0) return val;
	#define END_TM_PART_STATES()                }
	#include "Common/Tmschema.h"

	return 0;
}

UINT CXTPSkinManagerSchema::GetPropertyCode(LPCTSTR lpszProperty, XTPSkinManagerProperty& nPropertyType)
{

	#include "XTPSkinManagerSchema.inc"
	#undef TM_PROP
	#define TM_PROP(val, prefix, name, primval)   if (_tcsicmp(_T(#name), lpszProperty) == 0) { nPropertyType = XTP_SKINPROPERTY_##primval; return val; }
	#include "Common/Tmschema.h"

	return 0;
}

UINT CXTPSkinManagerSchema::CalculatePropertyCode(UINT iClassId, int iPartId, int iStateId, int iPropId)
{
	return iPropId + ((iClassId + ((iPartId + (iStateId << 6 )) << 6)) << 14);
}

UINT CXTPSkinManagerSchema::GetSchemaPartCode(CString strSchemaPart)
{
	if (strSchemaPart.Find(_T(':')) != -1)
		return 0; // Not Supported;

	REMOVE_S(strSchemaPart, _T(' '));

	CString strClass, strClassPart, strState;

	int nStateIndex = strSchemaPart.Find(_T('('));
	if (nStateIndex != -1)
	{
		strState = strSchemaPart.Mid(nStateIndex + 1, strSchemaPart.GetLength() - nStateIndex - 2);
		strSchemaPart = strSchemaPart.Left(nStateIndex);
	}


	int nClassPartIndex = strSchemaPart.Find(_T('.'));
	if (nClassPartIndex != -1)
	{
		strClass = strSchemaPart.Left(nClassPartIndex);
		strClassPart = strSchemaPart.Mid(nClassPartIndex + 1);
	}
	else
	{
		strClass = strSchemaPart;
	}

	int nClassPartCode = 0;
	if (!strClassPart.IsEmpty())
	{
		nClassPartCode = GetClassPartCode(strClass, strClassPart);
		if (nClassPartCode == 0)
		{
			ASSERT(FALSE);
			return 0;
		}
	}

	int nStateCode = 0;
	if (!strState.IsEmpty())
	{
		nStateCode = GetStateCode(strClassPart.IsEmpty()? strClass: strClassPart, strState);
		if (nStateCode == 0)
		{
			ASSERT(FALSE);
			return 0;
		}
	}

	int nClassCode = GetClassCode(strClass);
	if (nClassCode == 0)
		return 0;

	return CalculatePropertyCode(nClassCode, nClassPartCode, nStateCode, 0);
}

CXTPSkinManagerSchemaProperty* CXTPSkinManagerSchema::CreateProperty(LPCTSTR lpszProperty, XTPSkinManagerProperty nPropertyType, LPCTSTR lpszValue)
{
	CXTPSkinManagerSchemaProperty* pProperty = new CXTPSkinManagerSchemaProperty();

	switch (nPropertyType)
	{
		case XTP_SKINPROPERTY_COLOR:    pProperty->SetPropertyColor(lpszValue); break;
		case XTP_SKINPROPERTY_STRING:   pProperty->SetPropertyString(lpszValue); break;
		case XTP_SKINPROPERTY_BOOL:     pProperty->SetPropertyBool(lpszValue); break;
		case XTP_SKINPROPERTY_RECT:     pProperty->SetPropertyRect(lpszValue); break;
		case XTP_SKINPROPERTY_INT:      pProperty->SetPropertyInt(lpszValue); break;
		case XTP_SKINPROPERTY_POSITION: pProperty->SetPropertySize(lpszValue); break;
		case XTP_SKINPROPERTY_ENUM:     pProperty->SetPropertyEnum(GetEnumCode(lpszProperty, lpszValue)); break;
		case XTP_SKINPROPERTY_FONT:     pProperty->SetPropertyFont(lpszValue); break;

		default:
			delete pProperty;
			return NULL;
	}

	pProperty->propType = nPropertyType;
	return pProperty;
}


HRESULT CXTPSkinManagerSchema::ReadProperties(CXTPSkinManagerResourceFile* pResourceFile)
{
	RemoveAllProperties();

	CString str, strSchemaPart;

	UINT nSchemaPart = 0;

	while (pResourceFile->ReadString(str))
	{
		str.TrimLeft();

		int nCommentIndex = str.Find(_T(';'));

		if (nCommentIndex == 0)
			continue;

		if (nCommentIndex > 0)
		{
			str = str.Left(nCommentIndex);
		}

		if (str.IsEmpty())
			continue;

		str.MakeUpper();

		if (str[0] == _T('['))
		{
			strSchemaPart = str.Mid(1, str.GetLength() - 2);
			nSchemaPart = GetSchemaPartCode(strSchemaPart);
		}
		else if (nSchemaPart)
		{
			int nPos = str.Find(_T('='));
			if (nPos <= 0)
				continue;

			CString strProperty = str.Left(nPos);
			strProperty.TrimRight();
			strProperty.MakeUpper();

			XTPSkinManagerProperty nPropertyType = XTP_SKINPROPERTY_UNKNOWN;
			UINT nPropertyCode = GetPropertyCode(strProperty, nPropertyType);

			if (nPropertyCode == 0 || nPropertyType == XTP_SKINPROPERTY_UNKNOWN)
			{
				continue;
			}

			CString strValue = str.Mid(nPos + 1);
			strValue.TrimLeft();
			strValue.TrimRight();
			strValue.MakeUpper();

			CXTPSkinManagerSchemaProperty* pProperty = 0;


			if (m_mapProperties.Lookup(nSchemaPart + nPropertyCode, pProperty))
			{
				m_mapProperties.RemoveKey(nSchemaPart + nPropertyCode);
				delete pProperty;
			}

			pProperty = CreateProperty(strProperty, nPropertyType, strValue);

			if (pProperty == 0)
			{
				continue;
			}

			pProperty->nPropertyCode = nPropertyCode;

			m_mapProperties.SetAt(nSchemaPart + nPropertyCode, pProperty);
		}
	}

	int nAlpha = 0;
	if (SUCCEEDED(GetIntProperty(GetClassCode(_T("DOCUMENTATION")), 0, 0, TMT_ALPHALEVEL, nAlpha)) && nAlpha == 255)
	{
		m_bPreMultiplyImages = FALSE;
	}

	return S_OK;
}


CXTPSkinManagerSchemaProperty* CXTPSkinManagerSchema::GetProperty(UINT iClassId, int iPartId, int iStateId, int iPropId)
{
	CXTPSkinManagerSchemaProperty* pProperty;
	if (iStateId)
	{
		if (m_mapProperties.Lookup(CalculatePropertyCode(iClassId, iPartId, iStateId, iPropId), pProperty))
			return pProperty;
	}

	if (m_mapProperties.Lookup(CalculatePropertyCode(iClassId, iPartId, 0, iPropId), pProperty))
		return pProperty;

	if (m_mapProperties.Lookup(CalculatePropertyCode(iClassId, 0, 0, iPropId), pProperty))
		return pProperty;

	if (m_mapProperties.Lookup(CalculatePropertyCode(m_nGlobalClassId, 0, 0, iPropId), pProperty))
		return pProperty;

	return NULL;
}

HRESULT CXTPSkinManagerSchema::GetStringProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, CString& str)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(iClassId, iPartId, iStateId, iPropId);

	if (!pProperty)
		return E_FAIL;

	if (pProperty->propType != XTP_SKINPROPERTY_STRING)
		return E_INVALIDARG;

	str = pProperty->lpszVal;

	return S_OK;
}

HRESULT CXTPSkinManagerSchema::GetRectProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, CRect& rc)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(iClassId, iPartId, iStateId, iPropId);

	if (!pProperty)
		return E_FAIL;

	if (pProperty->propType != XTP_SKINPROPERTY_RECT)
		return E_INVALIDARG;

	rc = pProperty->rcVal;

	return S_OK;
}


HRESULT CXTPSkinManagerSchema::GetIntProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, int& iVal)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(iClassId, iPartId, iStateId, iPropId);

	if (!pProperty)
		return E_FAIL;

	if (pProperty->propType != XTP_SKINPROPERTY_INT)
		return E_INVALIDARG;

	iVal = pProperty->iVal;

	return S_OK;
}

HRESULT CXTPSkinManagerSchema::GetBoolProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, BOOL& bVal)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(iClassId, iPartId, iStateId, iPropId);

	if (!pProperty)
		return E_FAIL;

	if (pProperty->propType != XTP_SKINPROPERTY_BOOL)
		return E_INVALIDARG;

	bVal = pProperty->bVal;

	return S_OK;
}

HRESULT CXTPSkinManagerSchema::GetColorProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, COLORREF& clrVal)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(iClassId, iPartId, iStateId, iPropId);

	if (!pProperty)
		return E_FAIL;

	if (pProperty->propType != XTP_SKINPROPERTY_COLOR)
		return E_INVALIDARG;

	clrVal = pProperty->clrVal;
	GetSkinManager()->ApplyColorFilter(clrVal);

	return S_OK;
}

HRESULT CXTPSkinManagerSchema::GetEnumProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, int& nVal)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(iClassId, iPartId, iStateId, iPropId);

	if (!pProperty)
		return E_FAIL;

	if (pProperty->propType != XTP_SKINPROPERTY_ENUM)
		return E_INVALIDARG;

	nVal = pProperty->iVal;

	return S_OK;
}

HRESULT CXTPSkinManagerSchema::GetFontProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, LOGFONT& lfVal)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(iClassId, iPartId, iStateId, iPropId);

	if (!pProperty)
		return E_FAIL;

	if (pProperty->propType != XTP_SKINPROPERTY_FONT)
		return E_INVALIDARG;

	lfVal = *pProperty->lfVal;

	return S_OK;
}

HRESULT CXTPSkinManagerSchema::GetSizeProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, CSize& szVal)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(iClassId, iPartId, iStateId, iPropId);

	if (!pProperty)
		return E_FAIL;

	if (pProperty->propType != XTP_SKINPROPERTY_POSITION)
		return E_INVALIDARG;

	szVal = pProperty->szVal;

	return S_OK;

}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerSchemaDefault

CXTPSkinManagerSchemaDefault::CXTPSkinManagerSchemaDefault(CXTPSkinManager* pManager)
	: CXTPSkinManagerSchema(pManager)
{
	m_strSchemaName = _T("Default");
	m_nFrameButtonOffset = 0;
}

CXTPSkinManagerSchemaDefault::~CXTPSkinManagerSchemaDefault()
{
	FreeFrameRegions();

}

int CXTPSkinManagerSchema::FindBestImageGlyphSize(CXTPSkinManagerClass* pClass, int iPartId, int iStateId, const CRect& rcDest, int nImageCount, BOOL bHorizontalImageLayout)
{
	int nImageFile = TMT_IMAGEFILE5;
	CString strImageFile;

	while (nImageFile >= TMT_IMAGEFILE1)
	{
		strImageFile = pClass->GetThemeString(iPartId, iStateId, nImageFile);
		if (!strImageFile.IsEmpty())
		{
			CSize sz = pClass->GetImages()->GetExtent(m_pManager->GetResourceFile(), strImageFile);
			if (sz == CSize(0))
				return -1;

			if (bHorizontalImageLayout) sz.cx /= nImageCount; else sz.cy /= nImageCount;

			if ((nImageFile == TMT_IMAGEFILE1) || (rcDest.Height() >= sz.cy && rcDest.Width() >= sz.cx))
				return nImageFile;
		}
		nImageFile--;
	}

	return nImageFile;
}

BOOL CXTPSkinManagerSchema::DrawThemeBackgroundGlyph(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId, int iStateId, const RECT *pRect)
{
	int nImageFile = TMT_GLYPHIMAGEFILE;

	int nImageCount = pClass->GetThemeInt(iPartId, iStateId, TMT_IMAGECOUNT, 1);
	ASSERT(nImageCount >= 1);
	if (nImageCount < 1)
		return FALSE;

	BOOL bHorizontalImageLayout = pClass->GetThemeEnumValue(iPartId, iStateId, TMT_IMAGELAYOUT, IL_HORIZONTAL) == IL_HORIZONTAL;

	CRect rcDest(pRect);
	CRect rcContentMargins = pClass->GetThemeRect(iPartId, iStateId, TMT_CONTENTMARGINS);
	rcDest.DeflateRect(rcContentMargins);


	if (pClass->GetThemeEnumValue(iPartId, iStateId, TMT_IMAGESELECTTYPE) == IST_SIZE)
	{
		nImageFile = FindBestImageGlyphSize(pClass, iPartId, iStateId, rcDest, nImageCount, bHorizontalImageLayout);
		if (nImageFile == -1)
			return FALSE;
	}

	CString strImageFile = pClass->GetThemeString(iPartId, iStateId, nImageFile);
	if (strImageFile.IsEmpty())
		return FALSE;

	CXTPSkinImage* pImage = pClass->GetImages()->LoadFile(m_pManager->GetResourceFile(), strImageFile);
	if (!pImage)
	{
		return FALSE;
	}


	CSize sz(pImage->GetWidth(), pImage->GetHeight());
	if (bHorizontalImageLayout) sz.cx /= nImageCount; else sz.cy /= nImageCount;

	BOOL bTransparent = pImage->IsAlphaImage()? FALSE: pClass->GetThemeBool(iPartId, iStateId, TMT_GLYPHTRANSPARENT);

	CPoint ptTopLeft((rcDest.left + rcDest.right - sz.cx)/2, (rcDest.top + rcDest.bottom - sz.cy)/2);
	if (ptTopLeft.y < rcDest.top)
		ptTopLeft.y  = rcDest.top;

	CRect rcGlyph(ptTopLeft, sz);

	CRect rcSrcImage = bHorizontalImageLayout? CRect((iStateId - 1) * sz.cx,  0, iStateId * sz.cx, sz.cy):
	CRect(0, (iStateId - 1) * sz.cy, sz.cx, iStateId * sz.cy);

	if (nImageCount <= iStateId - 1)
		rcSrcImage = CRect(0, 0, sz.cx, sz.cy);

	COLORREF clrTransparent = COLORREF_NULL;
	if (bTransparent)
	{
		clrTransparent = pClass->GetThemeColor(iPartId, iStateId, TMT_TRANSPARENTCOLOR, RGB(0xFF, 0, 0xFF));
	}
	pImage->DrawImage(pDC, rcGlyph, rcSrcImage, CRect(0, 0, 0, 0), clrTransparent, ST_TRUESIZE, FALSE);

	return TRUE;
}

BOOL CXTPSkinManagerSchema::DrawThemeBackgroundBorder(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId, int iStateId, const RECT *pRect)
{
	int nBorderSize = pClass->GetThemeInt(iPartId, iStateId, TMT_BORDERSIZE, 1);

	COLORREF clrBorderColor = pClass->GetThemeColor(iPartId, iStateId, TMT_BORDERCOLOR);
	COLORREF clrFillColor = pClass->GetThemeColor(iPartId, iStateId, TMT_FILLCOLOR);
	BOOL bBorderOnly = pClass->GetThemeBool(iPartId, iStateId, TMT_BORDERONLY, FALSE);

	CRect rc(pRect);

	if ((nBorderSize > 0) && (clrBorderColor != COLORREF_NULL))
	{
		pDC->FillSolidRect(rc.left, rc.top, rc.Width(), nBorderSize, clrBorderColor);
		pDC->FillSolidRect(rc.left, rc.bottom - nBorderSize, rc.Width(), nBorderSize, clrBorderColor);

		pDC->FillSolidRect(rc.left, rc.top + nBorderSize, nBorderSize, rc.Height() - nBorderSize * 2, clrBorderColor);
		pDC->FillSolidRect(rc.right - nBorderSize, rc.top + nBorderSize, nBorderSize, rc.Height() - nBorderSize * 2, clrBorderColor);

		rc.DeflateRect(nBorderSize, nBorderSize);
	}

	if ((clrFillColor != COLORREF_NULL) && (!bBorderOnly))
	{
		pDC->FillSolidRect(rc, clrFillColor);
	}

	return TRUE;
}


BOOL CXTPSkinManagerSchemaDefault::DrawThemeBackground(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId, int iStateId, const RECT *pRect)
{
	int nBackgroundType = pClass->GetThemeEnumValue(iPartId, iStateId, TMT_BGTYPE, BT_IMAGEFILE);

	if (nBackgroundType == BT_BORDERFILL)
	{
		return DrawThemeBackgroundBorder(pDC, pClass, iPartId, iStateId, pRect);
	}

	if (nBackgroundType != BT_IMAGEFILE)
		return TRUE;

	int nImageFile = (pClass->GetThemeEnumValue(iPartId, iStateId, TMT_IMAGESELECTTYPE) != IST_NONE) &&
		(pClass->GetThemeEnumValue(iPartId, iStateId, TMT_GLYPHTYPE, GT_NONE) == GT_NONE) ?
	TMT_IMAGEFILE1: TMT_IMAGEFILE;

	CString strImageFile = pClass->GetThemeString(iPartId, iStateId, nImageFile);
	if (strImageFile.IsEmpty())
	{
		if (nImageFile != TMT_IMAGEFILE1)
			return FALSE;

		strImageFile = pClass->GetThemeString(iPartId, iStateId, TMT_IMAGEFILE);
		if (strImageFile.IsEmpty())
			return FALSE;

	}

	CRect rcSizingMargins = pClass->GetThemeRect(iPartId, iStateId, TMT_SIZINGMARGINS);

	CXTPSkinImage* pImage = pClass->GetImages()->LoadFile(m_pManager->GetResourceFile(), strImageFile);
	if (!pImage)
	{
		return FALSE;
	}

	int nImageCount = pClass->GetThemeInt(iPartId, iStateId, TMT_IMAGECOUNT, 1);
	if (nImageCount < 1)
		nImageCount = 1;

	BOOL bHorizontalImageLayout = pClass->GetThemeEnumValue(iPartId, iStateId, TMT_IMAGELAYOUT, IL_HORIZONTAL) == IL_HORIZONTAL;

	CSize sz(pImage->GetWidth(), pImage->GetHeight());
	if (bHorizontalImageLayout) sz.cx /= nImageCount; else sz.cy /= nImageCount;

	BOOL bTransparent = pImage->IsAlphaImage()? FALSE: pClass->GetThemeBool(iPartId, iStateId, TMT_TRANSPARENT);

	CRect rcImage(*pRect);

	int nSizingType = pClass->GetThemeEnumValue(iPartId, iStateId, TMT_SIZINGTYPE, ST_STRETCH);
	if (nSizingType == ST_TRUESIZE)
	{
		CSize szDest(sz);

		if (pClass->GetThemeInt(iPartId, iStateId, TMT_TRUESIZESTRETCHMARK) > 0)
		{
			if (szDest.cx > rcImage.Width())
			{
				szDest.cx = rcImage.Width();
			}

			if (szDest.cy > rcImage.Height())
			{
				szDest.cy = rcImage.Height();
			}
		}

		CPoint ptTopLeft((rcImage.left + rcImage.right - szDest.cx)/2, (rcImage.top + rcImage.bottom - szDest.cy)/2);
		if (ptTopLeft.y < rcImage.top)
			ptTopLeft.y = rcImage.top;

		if (ptTopLeft.x < rcImage.left)
			ptTopLeft.x = rcImage.left;

		if (pClass->GetThemeEnumValue(iPartId, iStateId, TMT_VALIGN) == VA_BOTTOM)
			rcImage = CRect(CPoint(ptTopLeft.x, rcImage.bottom - szDest.cy), szDest);
		else
			rcImage = CRect(ptTopLeft, szDest);

		rcSizingMargins = CRect(0, 0, 0, 0);

	}

	CRect rcSrcImage = bHorizontalImageLayout? CRect((iStateId - 1) * sz.cx,  0, iStateId * sz.cx, sz.cy):
		CRect(0, (iStateId - 1) * sz.cy, sz.cx, iStateId * sz.cy);

	if ((nImageCount <= iStateId - 1) || (iStateId == 0))
		rcSrcImage = CRect(0, 0, sz.cx, sz.cy);

	COLORREF clrTransparent = COLORREF_NULL;
	if (bTransparent)
	{
		clrTransparent = pClass->GetThemeColor(iPartId, iStateId, TMT_TRANSPARENTCOLOR, RGB(0xFF, 0, 0xFF));
	}

	BOOL bBorderOnly = pClass->GetThemeBool(iPartId, iStateId, TMT_BORDERONLY);

	if (pClass->GetClassCode() == m_nWindowClassId && (iPartId >= WP_FRAMELEFT && iPartId <= WP_SMALLFRAMEBOTTOM))
		bBorderOnly = FALSE;

	pImage->m_bMirrorImage =  pClass->GetThemeBool(iPartId, iStateId, TMT_MIRRORIMAGE, TRUE);

	pImage->DrawImage(pDC, rcImage, rcSrcImage, rcSizingMargins, clrTransparent, nSizingType, bBorderOnly);

	if (pClass->GetThemeEnumValue(iPartId, iStateId, TMT_GLYPHTYPE) == GT_IMAGEGLYPH)
	{
		DrawThemeBackgroundGlyph(pDC, pClass, iPartId, iStateId, pRect);
	}

	return TRUE;
}

CSize CXTPSkinManagerSchemaDefault::GetCaptionButtonSize(CXTPSkinManagerClass* pClass, int yButton)
{
	int iPartId = WP_CLOSEBUTTON, iStateId = 1;

	if (pClass->GetThemeEnumValue(iPartId, iStateId, TMT_BGTYPE) != BT_IMAGEFILE)
		return CSize(yButton, yButton);

	int nImageFile = (pClass->GetThemeEnumValue(iPartId, iStateId, TMT_IMAGESELECTTYPE) != IST_NONE) &&
		(pClass->GetThemeEnumValue(iPartId, iStateId, TMT_GLYPHTYPE, GT_NONE) == GT_NONE) ?
		TMT_IMAGEFILE1: TMT_IMAGEFILE;

	CString strImageFile = pClass->GetThemeString(iPartId, iStateId, nImageFile);
	if (strImageFile.IsEmpty())
	{
		if (nImageFile != TMT_IMAGEFILE1)
			return CSize(yButton, yButton);

		strImageFile = pClass->GetThemeString(iPartId, iStateId, TMT_IMAGEFILE);
		if (strImageFile.IsEmpty())
			return CSize(yButton, yButton);

	}


	CXTPSkinImage* pImage = pClass->GetImages()->LoadFile(m_pManager->GetResourceFile(), strImageFile);
	if (!pImage)
	{
		return CSize(yButton, yButton);
	}

	int nImageCount = pClass->GetThemeInt(iPartId, iStateId, TMT_IMAGECOUNT, 1);
	if (nImageCount < 1)
		nImageCount = 1;

	BOOL bHorizontalImageLayout = pClass->GetThemeEnumValue(iPartId, iStateId, TMT_IMAGELAYOUT, IL_HORIZONTAL) == IL_HORIZONTAL;

	CSize sz(pImage->GetWidth(), pImage->GetHeight());
	if (bHorizontalImageLayout) sz.cx /= nImageCount; else sz.cy /= nImageCount;

	int xButton = MulDiv(sz.cx, yButton, sz.cy) + MulDiv(sz.cx - sz.cy, 4, sz.cy);

	return CSize(xButton, yButton);
}

int CXTPSkinManagerSchemaDefault::DrawThemeFrameButtons(CDC* pDC, CXTPSkinObjectFrame* pFrame)
{
	CXTPWindowRect rc(pFrame);
	rc.OffsetRect(-rc.TopLeft());

	CRect rcBorders = pFrame->GetBorders();
	int nCaptionHeight = rcBorders.top;
	LONG lStyle = pFrame->GetStyle();
	DWORD dwExStyle = pFrame->GetExStyle();

	int cBorders = GetWindowBorders(lStyle, dwExStyle, TRUE, FALSE);

	int cxFrame = cBorders * m_pManager->GetMetrics()->m_cxBorder;

	int nTop = 0;
	if (dwExStyle & WS_EX_WINDOWEDGE)
		nTop++;
	else if (dwExStyle & WS_EX_STATICEDGE)
		nTop++;

	if (lStyle & WS_SIZEBOX)
		nTop += (1 + m_pManager->GetMetrics()->m_nBorderSize) / 2;

	int yButton = nCaptionHeight - 5 - cxFrame + m_nFrameButtonOffset;

	if (dwExStyle & WS_EX_CLIENTEDGE)
	{
		yButton -= 2;
		nTop -= 1;
	}

	CXTPSkinManagerClass* pClass = m_pManager->GetSkinClass(_T("WINDOW"));
	CSize szButton = dwExStyle & WS_EX_TOOLWINDOW ? CSize(yButton, yButton) : GetCaptionButtonSize(pClass, yButton);

	CRect rcButton = CRect(CPoint(rc.right - szButton.cx - cBorders - 2, nTop + (nCaptionHeight - szButton.cy)/2), szButton);

	CXTPSkinObjectFrame::CCaptionButtons* pButtons = pFrame->GetCaptionButtons();

	for (int i = 0; i < (int)pButtons->GetSize(); i++)
	{
		CXTPSkinObjectFrame::CCaptionButton* pButton = pButtons->GetAt(i);

		pButton->m_rcButton = rcButton;
		pButton->Draw(pDC, pFrame->IsActive());

		rcButton.OffsetRect(-rcButton.Width() - 2, 0);
	}

	return rcButton.right + 2;
}

void CXTPSkinManagerSchema::DrawNonClientRect(CDC* pDC, CRect rcFrame, CXTPSkinObjectFrame* pFrame)
{
	DWORD dwExStyle = pFrame->GetExStyle();
	DWORD dwStyle = pFrame->GetStyle();

	if (dwExStyle & WS_EX_WINDOWEDGE)
	{
		DrawEdge(pDC->GetSafeHdc(), &rcFrame, EDGE_RAISED, BF_RECT | BF_ADJUST);
	}
	else if (dwExStyle & WS_EX_STATICEDGE)
	{
		DrawEdge(pDC->GetSafeHdc(), &rcFrame, BDR_SUNKENOUTER, BF_RECT | BF_ADJUST);
	}

	if ((dwStyle & WS_CAPTION) || (dwExStyle & WS_EX_DLGMODALFRAME))
	{
		COLORREF clr = GetSysColor((dwExStyle & (WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE))
			|| (dwStyle & WS_DLGFRAME) ? COLOR_3DFACE : COLOR_WINDOWFRAME);

		pDC->Draw3dRect(rcFrame, clr, clr);
		rcFrame.DeflateRect(1, 1);
	}

	if (dwStyle & WS_SIZEBOX)
	{
		int nSize = m_pManager->GetMetrics()->m_nBorderSize;
		XTPSkinFrameworkDrawFrame(pDC->GetSafeHdc(), rcFrame, nSize, GetSysColor(COLOR_3DFACE));

		rcFrame.DeflateRect(nSize, nSize);
	}


	if (dwExStyle & WS_EX_CLIENTEDGE)
	{
		DrawClientEdge(pDC, rcFrame, pFrame);
	}
}

void CXTPSkinManagerSchema::DrawClientEdge(CDC* pDC, const CRect& rcFrame, CXTPSkinObjectFrame* pFrame)
{
	CRect rc = rcFrame;

	CXTPSkinManagerClass* pClass = m_pManager->GetSkinClass(pFrame->GetClassName());

	if (pClass->GetThemeEnumValue(0, 0, TMT_BGTYPE) == BT_BORDERFILL)
	{
		COLORREF clrBorderColor = pClass->GetThemeColor(0, 0, TMT_BORDERCOLOR);

		pDC->Draw3dRect(rc, clrBorderColor, clrBorderColor);
		rc.DeflateRect(1, 1);

		HBRUSH hBrush = pFrame->GetClientBrush(pDC);
		XTPSkinFrameworkDrawFrame(pDC->GetSafeHdc(), &rc, 1, hBrush);
	}
	else
	{
		pDC->Draw3dRect(rc, m_pManager->GetMetrics()->m_clrEdgeShadow, m_pManager->GetMetrics()->m_clrEdgeHighLight);
		rc.DeflateRect(1, 1);
		pDC->Draw3dRect(rc, m_pManager->GetMetrics()->m_clrEdgeDkShadow, m_pManager->GetMetrics()->m_clrEdgeLight);
		rc.DeflateRect(1, 1);
	}
}


HICON GetFrameSmIcon(CXTPSkinObjectFrame* pFrame)
{
	DWORD dwStyle = pFrame->GetStyle();
	DWORD dwExStyle = pFrame->GetExStyle();

	if (dwExStyle & WS_EX_TOOLWINDOW)
		return NULL;

	if ((dwStyle & WS_SYSMENU) == 0)
		return NULL;

	HICON hIcon = (HICON)(DWORD_PTR)::SendMessage(pFrame->m_hWnd, WM_GETICON, ICON_SMALL, 0);
	if (hIcon)
		return hIcon;

	hIcon = (HICON)(DWORD_PTR)::GetClassLongPtr(pFrame->m_hWnd, GCLP_HICONSM);
	if (hIcon)
		return hIcon;

	if (((dwStyle & (WS_BORDER | WS_DLGFRAME)) != WS_DLGFRAME) && ((dwExStyle & WS_EX_DLGMODALFRAME) == 0))
	{
		ULONG_PTR dwResult;

		if (SendMessageTimeout(pFrame->GetSafeHwnd(),
			WM_QUERYDRAGICON,
			0,
			0,
			SMTO_NORMAL,
			100,
			&dwResult))
		{
			hIcon = (HICON)dwResult;
		}

		if (hIcon == NULL)
		{
			hIcon = AfxGetApp()->LoadOEMIcon(OIC_WINLOGO);
		}
	}

	return hIcon;
}

void CXTPSkinManagerSchemaDefault::DrawThemeFrame(CDC* pDC, CXTPSkinObjectFrame* pFrame)
{
	CXTPWindowRect rc(pFrame);
	rc.OffsetRect(-rc.TopLeft());

	DWORD dwStyle = pFrame->GetStyle();
	DWORD dwExStyle = pFrame->GetExStyle();

	if ((dwStyle & WS_CAPTION) != WS_CAPTION)
	{
		DrawNonClientRect(pDC, rc, pFrame);
		return;
	}

	BOOL bToolWindow = (dwExStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW;

	CRect rcBorders = pFrame->GetBorders();

	if (dwExStyle & WS_EX_CLIENTEDGE)
	{
		DrawClientEdge(pDC, CRect(rcBorders.left - 2, rcBorders.top - 2, rc.Width() - rcBorders.right + 2,
			rc.Height() - rcBorders.bottom + 2), pFrame);

		InflateBorders(rcBorders, -2, -2);
	}


	CRect rcCaption(0, 0, rc.right, rcBorders.top);
	CRect rcFrame(0, rcBorders.top, rc.Width(), rc.Height());

	CXTPSkinManagerClass* pClassWindow = m_pManager->GetSkinClass(_T("WINDOW"));

	int nFrameState = pFrame->IsActive()? FS_ACTIVE: FS_INACTIVE;

	pClassWindow->DrawThemeBackground(pDC, bToolWindow? WP_SMALLFRAMELEFT: WP_FRAMELEFT, nFrameState,
		CRect(rcFrame.left, rcFrame.top, rcFrame.left + rcBorders.left, rcFrame.bottom - rcBorders.bottom));

	pClassWindow->DrawThemeBackground(pDC, bToolWindow? WP_SMALLFRAMERIGHT: WP_FRAMERIGHT, nFrameState,
		CRect(rcFrame.right - rcBorders.right, rcFrame.top, rcFrame.right , rcFrame.bottom - rcBorders.bottom));

	pClassWindow->DrawThemeBackground(pDC, bToolWindow? WP_SMALLFRAMEBOTTOM: WP_FRAMEBOTTOM, nFrameState,
		CRect(rcFrame.left, rcFrame.bottom - rcBorders.bottom, rcFrame.right , rcFrame.bottom));

	{
		CXTPLockGuard lock(m_csDraw);
		CXTPBufferDC dc(*pDC, rcCaption);

		pClassWindow->DrawThemeBackground(&dc, bToolWindow? WP_SMALLCAPTION: WP_CAPTION, nFrameState, rcCaption);


		CSize szIcon(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));

		int nTextLeft = rcBorders.left;

		HICON hIcon = GetFrameSmIcon(pFrame);

		int cBorders = GetWindowBorders(dwStyle, dwExStyle, TRUE, FALSE) - m_pManager->GetMetrics()->m_cyBorder;

		if (hIcon)
		{

			int cxySlot = rcBorders.top - cBorders;
			int nTop = cBorders  + (cxySlot  - szIcon.cy)/2;

			CRect rcButn(rcBorders.left + 2, nTop, rcBorders.left + 2 + szIcon.cx, nTop + szIcon.cy);

			DWORD dwLayout = XTPDrawHelpers()->IsContextRTL(&dc);
			if (dwLayout & LAYOUT_RTL)
				XTPDrawHelpers()->SetContextRTL(&dc, dwLayout | LAYOUT_BITMAPORIENTATIONPRESERVED);

			DrawIconEx(dc.m_hDC, rcButn.left, rcButn.top,hIcon,
				rcButn.Width(), rcButn.Height(), 0, NULL, DI_NORMAL);

			if (dwLayout & LAYOUT_RTL)
				XTPDrawHelpers()->SetContextRTL(&dc, dwLayout);


			nTextLeft = 2 + szIcon.cx + rcBorders.left;
		}


		int nTextRight = DrawThemeFrameButtons(&dc, pFrame);

		dc.SetTextColor(m_pManager->GetThemeSysColor(pFrame->IsActive() ? TMT_CAPTIONTEXT : TMT_INACTIVECAPTIONTEXT));

		CFont* pOldFont = dc.SelectObject(bToolWindow ? &m_pManager->GetMetrics()->m_fntSmCaption : &m_pManager->GetMetrics()->m_fntCaption);

		CString strCaption;
		pFrame->GetWindowText(strCaption);

		CRect rcText(nTextLeft, 1 + cBorders, nTextRight, rcBorders.top);
		pClassWindow->DrawThemeText(&dc, bToolWindow ?  WP_SMALLCAPTION : WP_CAPTION, pFrame->IsActive() ? CS_ACTIVE : CS_INACTIVE,
			strCaption, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX
			| (dwExStyle & WS_EX_RTLREADING ? DT_RTLREADING : 0) | (dwExStyle & WS_EX_RIGHT ? DT_RIGHT : 0), rcText);

		dc.SelectObject(pOldFont);
	}
}

int CXTPSkinManagerSchemaDefault::GetWindowBorders(LONG lStyle, DWORD dwExStyle, BOOL fWindow, BOOL fClient)
{
	int cBorders = 0;

	if (fWindow)
	{
		if (dwExStyle & WS_EX_WINDOWEDGE)
			cBorders += 2;
		else if (dwExStyle & WS_EX_STATICEDGE)
			cBorders++;

		if ((lStyle & WS_CAPTION) || (dwExStyle & WS_EX_DLGMODALFRAME))
			cBorders++;

		if (lStyle & WS_SIZEBOX)
			cBorders += m_pManager->GetMetrics()->m_nBorderSize;
	}

	if (fClient)
	{
		if (dwExStyle & WS_EX_CLIENTEDGE)
			cBorders += 2;
	}

	return(cBorders);
}


CRect CXTPSkinManagerSchemaDefault::CalcFrameBorders(CXTPSkinObjectFrame* pFrame)
{
	DWORD dwExStyle = pFrame->GetExStyle();
	DWORD dwStyle = pFrame->GetStyle();
	BOOL bToolWindow = (dwExStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW;
	CXTPSkinManagerMetrics* pMetrics = m_pManager->GetMetrics();

	if (((dwStyle & (WS_CAPTION | WS_MAXIMIZE)) == (WS_CAPTION | WS_MAXIMIZE) && ((dwStyle & WS_CHILD) == 0)))
	{
		int nCaptionHeight = bToolWindow ? pMetrics->m_cySmallCaption: pMetrics->m_cyCaption;

		CRect rc(0, 0, 100, 100);
		AdjustWindowRectEx(rc, dwStyle, FALSE, dwExStyle);
		int nBorder = -rc.left;
		return CRect(nBorder, nBorder + nCaptionHeight, nBorder, nBorder);
	}



	CRect rcBorders(0, 0, 0, 0);
	if ((dwStyle & WS_CAPTION) == WS_CAPTION)
	{
		int nCaptionHeight = bToolWindow ? pMetrics->m_cySmallCaption: pMetrics->m_cyCaption;
		rcBorders.top += nCaptionHeight;
	}

	int cBorders = GetWindowBorders(dwStyle, dwExStyle, TRUE, FALSE);

	InflateBorders(rcBorders, cBorders * pMetrics->m_cxBorder, cBorders * pMetrics->m_cyBorder);

	if ((dwExStyle & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE)
	{
		InflateBorders(rcBorders, pMetrics->m_cxEdge, pMetrics->m_cyEdge);
	}

	return rcBorders;
}

AFX_INLINE void IncludeRgnPart(CRgn* pRgn, int x1, int y, int x2)
{
	if (x1 < x2)
	{
		if (pRgn->GetSafeHandle() == NULL)
		{
			pRgn->CreateRectRgn(0, 0, 0, 0);
		}
		CRgn rgnExclude;
		rgnExclude.CreateRectRgn(x1, y, x2, y + 1);
		pRgn->CombineRgn(pRgn, &rgnExclude, RGN_OR);
	}
}

void RegionFromBitmap(CDC* pDC, CRgn* pRgnLeft, CRgn* pRgnRight, int& nRgnFrameCenter, const CRect& rc, const CRect& rcSizingMargins, COLORREF clrTransparent)
{
	CSize sz = rc.Size();
	nRgnFrameCenter = 0;
	BOOL bCheckCenter = TRUE;

	for (int y = 0; y < sz.cy; y++)
	{
		int x = 0;

		while (pDC->GetPixel(x, y) == clrTransparent && x < rcSizingMargins.left)
		{
			x++;
		}

		if (x > 0)
		{
			IncludeRgnPart(pRgnLeft, 0, y, x);
		}

		x = sz.cx - 1;

		while (pDC->GetPixel(x, y) == clrTransparent && x >= sz.cx - rcSizingMargins.right)
		{
			x--;
		}

		if (x != sz.cx - 1)
		{
			IncludeRgnPart(pRgnRight, x - (sz.cx - rcSizingMargins.right) + 1, y, rcSizingMargins.right);
		}

		if (x > sz.cx - rcSizingMargins.right)
		{
			while (pDC->GetPixel(x, y) != clrTransparent && x >= sz.cx - rcSizingMargins.right)
			{
				x--;
			}

			if (x > sz.cx - rcSizingMargins.right)
			{
				IncludeRgnPart(pRgnRight, 0, y, x - (sz.cx - rcSizingMargins.right) + 1);
			}
		}

		if (bCheckCenter)
		{
			x = rcSizingMargins.left;

			while (pDC->GetPixel(x, y) == clrTransparent && x < sz.cx - rcSizingMargins.right)
			{
				x++;
			}
			if (x == sz.cx - rcSizingMargins.right)
			{
				nRgnFrameCenter++;
			}
			else
			{
				bCheckCenter = FALSE;
			}
		}
	}
}

CXTPSkinManagerSchemaDefault::FRAMEREGION* CXTPSkinManagerSchemaDefault::CreateFrameRegion(CXTPSkinObjectFrame* pFrame, CSize szDest)
{
	int nCaptionHeight = pFrame->GetBorders().top;
	BOOL bToolWindow = pFrame->GetExStyle() & WS_EX_TOOLWINDOW;

	for (int i = 0; i < m_arrFrameRegions.GetSize(); i++)
	{
		FRAMEREGION* pRegion = m_arrFrameRegions[i];

		if (nCaptionHeight == pRegion->nCaptionHeight && bToolWindow == pRegion->bToolWindow)
		{
			if (szDest.cx < pRegion->rcFrameSizingMargins.left + pRegion->rcFrameSizingMargins.right)
				break;
			return pRegion;
		}
	}

	int iPartId = bToolWindow ? WP_SMALLCAPTION : WP_CAPTION;

	CXTPSkinManagerClass* pClass = m_pManager->GetSkinClass(_T("WINDOW"));

	CString strImageFile = pClass->GetThemeString(iPartId, FS_ACTIVE, TMT_IMAGEFILE);
	if (strImageFile.IsEmpty())
		return  NULL;

	CSize sz = pClass->GetImages()->GetExtent(m_pManager->GetResourceFile(), strImageFile);
	if (sz == CSize(0))
		return  NULL;

	CXTPSkinImage* pImage = pClass->GetImages()->LoadFile(m_pManager->GetResourceFile(), strImageFile);
	if (!pImage)
		return NULL;

	int nImageCount = pClass->GetThemeInt(iPartId, FS_ACTIVE, TMT_IMAGECOUNT, 1);
	ASSERT(nImageCount >= 1);
	if (nImageCount < 1)
		return NULL;


	FRAMEREGION* pRegion = new FRAMEREGION();
	pRegion->bAutoDelete = FALSE;
	pRegion->rcFrameSizingMargins.SetRectEmpty();

	CWindowDC dcWindow(pFrame);
	CDC dc;
	dc.CreateCompatibleDC(&dcWindow);

	CRect rcSrc(0, 0, sz.cx, sz.cy / nImageCount);
	CRect rcDest(0, 0, sz.cx, nCaptionHeight);


	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dcWindow, rcDest.Width(), rcDest.Height());

	CBitmap* pOldBitmap = dc.SelectObject(&bmp);

	BOOL bTransparent = pImage->IsAlphaImage()? FALSE: pClass->GetThemeBool(iPartId, FS_ACTIVE, TMT_TRANSPARENT);
	COLORREF clrTransparent = COLORREF_NULL;
	if (bTransparent)
	{
		clrTransparent = pClass->GetThemeColor(iPartId, FS_ACTIVE, TMT_TRANSPARENTCOLOR, RGB(0xFF, 0, 0xFF));
	}

	dc.FillSolidRect(rcDest, clrTransparent != COLORREF_NULL ? clrTransparent : RGB(0xFF, 0, 0xFF));

	CRect rcSizingMargins = pClass->GetThemeRect(iPartId, FS_ACTIVE, TMT_SIZINGMARGINS);

	if (rcSizingMargins.top + rcSizingMargins.bottom > rcDest.Height())
	{
		rcSizingMargins.bottom = max(0, rcDest.Height() - rcSizingMargins.top);
	}

	CRect rcDestSizingMargins = rcSizingMargins;
	if (szDest.cx < rcSizingMargins.left + rcSizingMargins.right)
	{
		rcDest.right = szDest.cx;
		pRegion->bAutoDelete = TRUE;

		rcDestSizingMargins.left = MulDiv(rcDest.Width(), rcSizingMargins.left, (rcSizingMargins.left + rcSizingMargins.right));
		rcDestSizingMargins.right = rcDest.Width() - rcSizingMargins.left;
	}


	pImage->DrawImage(&dc, rcDest, rcSrc, rcSizingMargins, clrTransparent, ST_STRETCH, FALSE);


	RegionFromBitmap(&dc, &pRegion->rgnFrameLeft, &pRegion->rgnFrameRight, pRegion->nRgnFrameCenter,
		rcDest, rcDestSizingMargins, clrTransparent != COLORREF_NULL ? clrTransparent : RGB(0xFF, 0, 0xFF));

	dc.SelectObject(pOldBitmap);

	pRegion->rcFrameSizingMargins = rcDestSizingMargins;
	pRegion->bToolWindow = bToolWindow;
	pRegion->nCaptionHeight = nCaptionHeight;

	if (!pRegion->bAutoDelete)
		m_arrFrameRegions.Add(pRegion);

	return pRegion;
}

void CXTPSkinManagerSchemaDefault::FreeFrameRegions()
{
	for (int i = 0; i < m_arrFrameRegions.GetSize(); i++)
	{
		delete m_arrFrameRegions[i];
	}
	m_arrFrameRegions.RemoveAll();
}

void CXTPSkinManagerSchemaDefault::RefreshMetrcis()
{
	FreeFrameRegions();
}

HRGN CXTPSkinManagerSchemaDefault::CalcFrameRegion(CXTPSkinObjectFrame* pFrame, CSize sz)
{
	CRgn rgnResult;

	if ((pFrame->GetStyle() & WS_MAXIMIZE) && ((pFrame->GetStyle() & WS_CHILD) == 0))
	{
		int nBorder = pFrame->GetBorders().left;

		CRect rc = XTPMultiMonitor()->GetWorkArea(pFrame);
		if (rc.Width() == sz.cx)
			rgnResult.CreateRectRgn(nBorder, nBorder, sz.cx + 2 * nBorder, sz.cy + 2 * nBorder);
		else
			rgnResult.CreateRectRgn(nBorder, nBorder, sz.cx - nBorder, sz.cy - nBorder);

		return (HRGN)rgnResult.Detach();
	}
	rgnResult.CreateRectRgn(0, 0, sz.cx, sz.cy);

	FRAMEREGION* pRegion = CreateFrameRegion(pFrame, sz);
	if (!pRegion)
	{
		return (HRGN)rgnResult.Detach();
	}

	if (pRegion->rgnFrameLeft.GetSafeHandle())
	{
		rgnResult.CombineRgn(&rgnResult, &pRegion->rgnFrameLeft, RGN_DIFF);
	}

	if (pRegion->rgnFrameRight.GetSafeHandle())
	{
		CRgn rgnFrameRight;
		rgnFrameRight.CreateRectRgn(0, 0, 0, 0);
		rgnFrameRight.CopyRgn(&pRegion->rgnFrameRight);

		rgnFrameRight.OffsetRgn(sz.cx - pRegion->rcFrameSizingMargins.right, 0);

		rgnResult.CombineRgn(&rgnResult, &rgnFrameRight, RGN_DIFF);
	}

	if (pRegion->nRgnFrameCenter > 0)
	{
		CRgn rgnFrameCenter;
		rgnFrameCenter.CreateRectRgn(pRegion->rcFrameSizingMargins.left, 0, sz.cx - pRegion->rcFrameSizingMargins.right, pRegion->nRgnFrameCenter);

		rgnResult.CombineRgn(&rgnResult, &rgnFrameCenter, RGN_DIFF);
	}

	if (pRegion->bAutoDelete)
		delete pRegion;

	return (HRGN)rgnResult.Detach();
}

COLORREF CXTPSkinManagerSchema::GetScrollBarSizeBoxColor(CXTPSkinObjectFrame* /*pFrame*/)
{
	return GetSysColor(COLOR_3DFACE);
}

void CXTPSkinManagerSchemaDefault::DrawThemeScrollBar(CDC* pDC, CXTPSkinObjectFrame* pFrame, XTP_SKINSCROLLBARPOSINFO* pSBInfo)
{
	#define ABS_UPACTIVE  17
	#define ABS_DOWNACTIVE 18
	#define ABS_LEFTACTIVE 19
	#define ABS_RIGHTACTIVE 20

	#define GETPARTSTATE(ht, pressed, hot, normal, disabled, active) \
		(!bEnabled ? disabled : nPressetHt == ht ? pressed : \
		nHotHt == ht ? hot : bActive ? (active) : normal)

	XTP_SKINSCROLLBARTRACKINFO* pSBTrack = pFrame->GetScrollBarTrackInfo();

	BOOL nPressetHt = pSBTrack && pSBTrack->pSBInfo == pSBInfo ?
		(pSBTrack->bTrackThumb || pSBTrack->fHitOld ? pSBInfo->ht : -1) : -1;

	BOOL nHotHt = pSBTrack ? -1 : pSBInfo->ht;
	BOOL bActive = (pSBTrack && pSBTrack->pSBInfo == pSBInfo) || nHotHt  > 0 ;


	int cWidth = (pSBInfo->pxRight - pSBInfo->pxLeft);
	BOOL fVert = pSBInfo->fVert;

	if (cWidth <= 0)
	{
		return;
	}

	BOOL bEnabled = pSBInfo->posMax - pSBInfo->posMin - pSBInfo->page + 1 > 0;
	if (bEnabled && pSBInfo->nBar == SB_CTL)
		bEnabled = (pFrame->GetStyle() & WS_DISABLED) == 0;

	CXTPSkinManagerClass* pClassScrollBar = m_pManager->GetSkinClass(_T("SCROLLBAR"));

	int nBtnTrackSize =   pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
	int nBtnTrackPos = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;

	if (!bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
		nBtnTrackPos = nBtnTrackSize = 0;

	int nStateCount = pClassScrollBar->GetThemeInt(SBP_ARROWBTN, 0, TMT_IMAGECOUNT, 16);


	if (!fVert)
	{
		CRect rcHScroll(pSBInfo->rc);

		CRect rcArrowLeft(rcHScroll.left, rcHScroll.top, pSBInfo->pxUpArrow, rcHScroll.bottom);
		CRect rcArrowRight(pSBInfo->pxDownArrow, rcHScroll.top, rcHScroll.right, rcHScroll.bottom);

		pClassScrollBar->DrawThemeBackground(pDC, SBP_ARROWBTN, GETPARTSTATE(XTP_HTSCROLLUP,
			ABS_LEFTPRESSED, ABS_LEFTHOT, ABS_LEFTNORMAL, ABS_LEFTDISABLED, nStateCount == 20 ? ABS_LEFTACTIVE : ABS_LEFTNORMAL),  rcArrowLeft);
		pClassScrollBar->DrawThemeBackground(pDC, SBP_ARROWBTN, GETPARTSTATE(XTP_HTSCROLLDOWN,
			ABS_RIGHTPRESSED, ABS_RIGHTHOT, ABS_RIGHTNORMAL, ABS_RIGHTDISABLED, nStateCount == 20 ? ABS_RIGHTACTIVE : ABS_RIGHTNORMAL),  rcArrowRight);

		CRect rcTrack(rcArrowLeft.right, rcHScroll.top, rcArrowRight.left, rcHScroll.bottom);

		if (!rcTrack.IsRectEmpty())
		{
			CRect rcLowerTrack(rcTrack.left, rcTrack.top, rcTrack.left + nBtnTrackPos, rcTrack.bottom);
			CRect rcBtnTrack(rcLowerTrack.right, rcTrack.top, rcLowerTrack.right + nBtnTrackSize, rcTrack.bottom);
			CRect rcUpperTrack(rcBtnTrack.right, rcTrack.top, rcTrack.right, rcTrack.bottom);

			if (!rcLowerTrack.IsRectEmpty())
				pClassScrollBar->DrawThemeBackground(pDC, SBP_LOWERTRACKHORZ, GETPARTSTATE(XTP_HTSCROLLUPPAGE, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED, SCRBS_NORMAL),  rcLowerTrack);

			if (!rcBtnTrack.IsRectEmpty())
			{
				pClassScrollBar->DrawThemeBackground(pDC, SBP_THUMBBTNHORZ, GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED, SCRBS_NORMAL),  rcBtnTrack);

				if (rcBtnTrack.Width() > 9)
					pClassScrollBar->DrawThemeBackground(pDC, SBP_GRIPPERHORZ, GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED, SCRBS_NORMAL),  rcBtnTrack);
			}

			if (!rcUpperTrack.IsRectEmpty())
				pClassScrollBar->DrawThemeBackground(pDC, SBP_UPPERTRACKHORZ, GETPARTSTATE(XTP_HTSCROLLDOWNPAGE, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED, SCRBS_NORMAL),  rcUpperTrack);
		}
	}
	else
	{
		CRect rcVScroll(pSBInfo->rc);

		CRect rcArrowUp(rcVScroll.left, rcVScroll.top, rcVScroll.right, pSBInfo->pxUpArrow);
		CRect rcArrowDown(rcVScroll.left, pSBInfo->pxDownArrow, rcVScroll.right, rcVScroll.bottom);

		pClassScrollBar->DrawThemeBackground(pDC, SBP_ARROWBTN, GETPARTSTATE(XTP_HTSCROLLUP,
			ABS_UPPRESSED, ABS_UPHOT, ABS_UPNORMAL, ABS_UPDISABLED,  nStateCount == 20 ? ABS_UPACTIVE : ABS_UPNORMAL),  rcArrowUp);
		pClassScrollBar->DrawThemeBackground(pDC, SBP_ARROWBTN, GETPARTSTATE(XTP_HTSCROLLDOWN,
			ABS_DOWNPRESSED, ABS_DOWNHOT, ABS_DOWNNORMAL, ABS_DOWNDISABLED, nStateCount == 20 ? ABS_DOWNACTIVE : ABS_DOWNNORMAL),  rcArrowDown);

		CRect rcTrack(rcVScroll.left, rcArrowUp.bottom, rcVScroll.right, rcArrowDown.top);

		if (!rcTrack.IsRectEmpty())
		{
			CRect rcLowerTrack(rcTrack.left, rcTrack.top, rcTrack.right, rcTrack.top + nBtnTrackPos);
			CRect rcBtnTrack(rcTrack.left, rcLowerTrack.bottom, rcTrack.right, rcLowerTrack.bottom + nBtnTrackSize);
			CRect rcUpperTrack(rcTrack.left, rcBtnTrack.bottom, rcTrack.right, rcTrack.bottom);

			if (!rcLowerTrack.IsRectEmpty())
				pClassScrollBar->DrawThemeBackground(pDC, SBP_LOWERTRACKVERT, GETPARTSTATE(XTP_HTSCROLLUPPAGE, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED, SCRBS_NORMAL),  rcLowerTrack);

			if (!rcBtnTrack.IsRectEmpty())
			{
				pClassScrollBar->DrawThemeBackground(pDC, SBP_THUMBBTNVERT, GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED, SCRBS_NORMAL),  rcBtnTrack);
				if (rcBtnTrack.Height() > 13)
					pClassScrollBar->DrawThemeBackground(pDC, SBP_GRIPPERVERT, GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED, SCRBS_NORMAL),  rcBtnTrack);
			}

			if (!rcUpperTrack.IsRectEmpty())
				pClassScrollBar->DrawThemeBackground(pDC, SBP_UPPERTRACKVERT, GETPARTSTATE(XTP_HTSCROLLDOWNPAGE, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED, SCRBS_NORMAL),  rcUpperTrack);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerSchemaOffice2007

CXTPSkinManagerSchemaOffice2007::CXTPSkinManagerSchemaOffice2007(CXTPSkinManager* pManager)
	: CXTPSkinManagerSchemaDefault(pManager)
{
	m_strSchemaName = _T("Office2007");
	m_bPreMultiplyImages = FALSE;
}

void CXTPSkinManagerSchemaOffice2007::DrawThumbButton(CDC* pDC, CRect rcThumb, int nState, BOOL bHorz)
{
	if (nState > 0) nState --;

	CXTPSkinManagerClass* pClassScrollBar = m_pManager->GetSkinClass(_T("SCROLLBAR"));

	CXTPSkinImage* pImage = pClassScrollBar->GetImages()->LoadFile(m_pManager->GetResourceFile(),
		bHorz ? _T("OFFICE2007\\SCROLLTHUMBVERTICAL.BMP") : _T("OFFICE2007\\SCROLLTHUMBHORIZONTAL.BMP"));
	ASSERT(pImage);
	if (!pImage)
		return;

	CRect rcSrc = CRect(0, 0, pImage->GetWidth(), pImage->GetHeight() / 3);
	rcSrc.OffsetRect(0, nState * rcSrc.Height());
	pImage->DrawImage(pDC, rcThumb, rcSrc, CRect(5, 5, 5, 5), COLORREF_NULL, ST_STRETCH, FALSE);
}

COLORREF CXTPSkinManagerSchemaOffice2007::GetScrollBarSizeBoxColor(CXTPSkinObjectFrame* pFrame)
{
	return pFrame->IsFrameScrollBars() ? RGB(121, 157, 207) : RGB(245, 245, 245);
}

AFX_INLINE CRect OffsetSourceRect(CRect rc, int nState)
{
	rc.OffsetRect(0, (nState - 1) * rc.Height());
	return rc;
}

void CXTPSkinManagerSchemaOffice2007::DrawThemeScrollBar(CDC* pDC, CXTPSkinObjectFrame* pFrame, XTP_SKINSCROLLBARPOSINFO* pSBInfo)
{
	#define GETPARTSTATE2(ht) \
	(!bEnabled ? 0 : nPressetHt == ht ? 3 : nHotHt == ht ? 2 : nHotHt > 0 || nPressetHt > 0 ? 1 : 0)

	XTP_SKINSCROLLBARTRACKINFO* pSBTrack = pFrame->GetScrollBarTrackInfo();

	BOOL nPressetHt = pSBTrack ? pSBInfo->ht : -1;
	BOOL nHotHt = pSBTrack ? -1 : pSBInfo->ht;

	int cWidth = (pSBInfo->pxRight - pSBInfo->pxLeft);
	BOOL fVert = pSBInfo->fVert;

	if (cWidth <= 0)
	{
		return;
	}

	BOOL bEnabled = pSBInfo->posMax - pSBInfo->posMin - pSBInfo->page + 1 > 0;
	if (bEnabled && pSBInfo->nBar == SB_CTL)
		bEnabled = (pFrame->GetStyle() & WS_DISABLED) == 0;

	CXTPSkinManagerClass* pClassScrollBar = m_pManager->GetSkinClass(_T("SCROLLBAR"));

	int nBtnTrackSize =   pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
	int nBtnTrackPos = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;

	if (!bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
		nBtnTrackPos = nBtnTrackSize = 0;

	BOOL bLight = !pFrame->IsFrameScrollBars();

	CXTPSkinImage* pImageArrowGlyphs = pClassScrollBar->GetImages()->LoadFile(m_pManager->GetResourceFile(), _T("OFFICE2007\\SCROLLARROWGLYPHS.BMP"));
	ASSERT(pImageArrowGlyphs);
	if (!pImageArrowGlyphs)
		return;
	CRect rcArrowGripperSrc(0, 0, 9, 9);

	CXTPSkinImage* pImageBackground = pClassScrollBar->GetImages()->LoadFile(m_pManager->GetResourceFile(),
		bLight ?
		!fVert ? _T("OFFICE2007\\SCROLLHORIZONTALLIGHT.BMP") : _T("OFFICE2007\\SCROLLVERTICALLIGHT.BMP"):
		!fVert ? _T("OFFICE2007\\SCROLLHORIZONTALDARK.BMP") : _T("OFFICE2007\\SCROLLVERTICALDARK.BMP"));
	ASSERT(pImageBackground);
	if (!pImageBackground)
		return;

	if (!fVert)
	{
		pImageBackground->DrawImage(pDC, pSBInfo->rc, pImageBackground->GetSource(0, 2), CRect(0, 1, 0, 1), ST_STRETCH, FALSE);

		CRect rcHScroll(pSBInfo->rc);
		rcHScroll.DeflateRect(0, 1);

		CRect rcArrowLeft(rcHScroll.left, rcHScroll.top, pSBInfo->pxUpArrow, rcHScroll.bottom);
		CRect rcArrowRight(pSBInfo->pxDownArrow, rcHScroll.top, rcHScroll.right, rcHScroll.bottom);

		CXTPSkinImage* pImage = pClassScrollBar->GetImages()->LoadFile(m_pManager->GetResourceFile(),
			bLight ? _T("OFFICE2007\\SCROLLARROWSHORIZONTALLIGHT.BMP") : _T("OFFICE2007\\SCROLLARROWSHORIZONTALDARK.BMP"));
		ASSERT(pImage);
		if (!pImage)
			return;

		CRect rcSrc = CRect(0, 0, pImage->GetWidth(), pImage->GetHeight() / 4);
		int nState = GETPARTSTATE2(XTP_HTSCROLLUP);
		if (nState != 0)
		{
			rcSrc.OffsetRect(0, nState * rcSrc.Height());
			pImage->DrawImage(pDC, rcArrowLeft, rcSrc, CRect(3, 3, 3, 3), COLORREF_NULL, ST_STRETCH, FALSE);
		}

		CRect rcArrowLeftGripper(CPoint((rcArrowLeft.left + rcArrowLeft.right - 9) / 2, (rcArrowLeft.top + rcArrowLeft.bottom - 9) / 2), CSize(9, 9));
		pImageArrowGlyphs->DrawImage(pDC, rcArrowLeftGripper, OffsetSourceRect(rcArrowGripperSrc, bEnabled ? ABS_LEFTNORMAL : ABS_LEFTDISABLED), CRect(0, 0, 0, 0), RGB(255, 0, 255), ST_TRUESIZE, FALSE);


		nState = GETPARTSTATE2(XTP_HTSCROLLDOWN);
		if (nState != 0)
		{
			rcSrc = CRect(0, 0, pImage->GetWidth(), pImage->GetHeight() / 4);
			rcSrc.OffsetRect(0, nState * rcSrc.Height());
			pImage->DrawImage(pDC, rcArrowRight, rcSrc, CRect(3, 3, 3, 3), COLORREF_NULL, ST_STRETCH, FALSE);
		}

		CRect rcArrowRightGripper(CPoint((rcArrowRight.left + rcArrowRight.right - 9) / 2, (rcArrowRight.top + rcArrowRight.bottom - 9) / 2), CSize(9, 9));
		pImageArrowGlyphs->DrawImage(pDC, rcArrowRightGripper, OffsetSourceRect(rcArrowGripperSrc, bEnabled ? ABS_RIGHTNORMAL : ABS_RIGHTDISABLED), CRect(0, 0, 0, 0), RGB(255, 0, 255), ST_TRUESIZE, FALSE);

		CRect rcTrack(rcArrowLeft.right, rcHScroll.top, rcArrowRight.left, rcHScroll.bottom);

		if (!rcTrack.IsRectEmpty())
		{
			CRect rcLowerTrack(rcTrack.left, rcTrack.top - 1, rcTrack.left + nBtnTrackPos, rcTrack.bottom + 1);
			CRect rcBtnTrack(rcLowerTrack.right, rcTrack.top, rcLowerTrack.right + nBtnTrackSize, rcTrack.bottom);
			CRect rcUpperTrack(rcBtnTrack.right, rcTrack.top - 1, rcTrack.right, rcTrack.bottom + 1);

			if (!rcLowerTrack.IsRectEmpty() && GETPARTSTATE2(XTP_HTSCROLLUPPAGE) == 3)
			{
				pImageBackground->DrawImage(pDC, rcLowerTrack,
					pImageBackground->GetSource(1, 2), CRect(0, 1, 0, 1), COLORREF_NULL, ST_STRETCH, FALSE);
			}

			if (!rcBtnTrack.IsRectEmpty())
			{
				DrawThumbButton(pDC, rcBtnTrack, GETPARTSTATE2(XTP_HTSCROLLTHUMB), TRUE);

				if (rcBtnTrack.Width() > 10)
					pClassScrollBar->DrawThemeBackground(pDC, SBP_GRIPPERHORZ, 0,  rcBtnTrack);
			}

			if (!rcUpperTrack.IsRectEmpty() && GETPARTSTATE2(XTP_HTSCROLLDOWNPAGE) == 3)
			{
				pImageBackground->DrawImage(pDC, rcUpperTrack,
					pImageBackground->GetSource(1, 2), CRect(0, 1, 0, 1), COLORREF_NULL, ST_STRETCH, FALSE);
			}
		}
	}
	else
	{
		pImageBackground->DrawImage(pDC, pSBInfo->rc, pImageBackground->GetSource(0, 2), CRect(1, 0, 1, 0), ST_STRETCH, FALSE);

		CRect rcVScroll(pSBInfo->rc);
		rcVScroll.DeflateRect(1, 0);

		CRect rcArrowUp(rcVScroll.left, rcVScroll.top, rcVScroll.right, pSBInfo->pxUpArrow);
		CRect rcArrowDown(rcVScroll.left, pSBInfo->pxDownArrow, rcVScroll.right, rcVScroll.bottom);

		CXTPSkinImage* pImage = pClassScrollBar->GetImages()->LoadFile(m_pManager->GetResourceFile(),
			bLight ? _T("OFFICE2007\\SCROLLARROWSVERTICALLIGHT.BMP") : _T("OFFICE2007\\SCROLLARROWSVERTICALDARK.BMP"));
		ASSERT(pImage);
		if (!pImage)
			return;

		CRect rcSrc = CRect(0, 0, pImage->GetWidth(), pImage->GetHeight() / 4);
		int nState = GETPARTSTATE2(XTP_HTSCROLLUP);
		if (nState != 0)
		{
			rcSrc.OffsetRect(0, nState * rcSrc.Height());
			pImage->DrawImage(pDC, rcArrowUp, rcSrc, CRect(3, 3, 3, 3), COLORREF_NULL, ST_STRETCH, FALSE);
		}

		CRect rcArrowUpGripper(CPoint((rcArrowUp.left + rcArrowUp.right - 9) / 2, (rcArrowUp.top + rcArrowUp.bottom - 9) / 2), CSize(9, 9));
		pImageArrowGlyphs->DrawImage(pDC, rcArrowUpGripper, OffsetSourceRect(rcArrowGripperSrc, bEnabled ? ABS_UPNORMAL : ABS_UPDISABLED), CRect(0, 0, 0, 0), RGB(255, 0, 255), ST_TRUESIZE, FALSE);



		nState = GETPARTSTATE2(XTP_HTSCROLLDOWN);
		if (nState != 0)
		{
			rcSrc = CRect(0, 0, pImage->GetWidth(), pImage->GetHeight() / 4);
			rcSrc.OffsetRect(0, nState * rcSrc.Height());
			pImage->DrawImage(pDC, rcArrowDown, rcSrc, CRect(3, 3, 3, 3), COLORREF_NULL, ST_STRETCH, FALSE);
		}

		CRect rcArrowDownGripper(CPoint((rcArrowDown.left + rcArrowDown.right - 9) / 2, (rcArrowDown.top + rcArrowDown.bottom - 9) / 2), CSize(9, 9));
		pImageArrowGlyphs->DrawImage(pDC, rcArrowDownGripper, OffsetSourceRect(rcArrowGripperSrc, bEnabled ? ABS_DOWNNORMAL : ABS_DOWNDISABLED), CRect(0, 0, 0, 0), RGB(255, 0, 255), ST_TRUESIZE, FALSE);


		CRect rcTrack(rcVScroll.left, rcArrowUp.bottom, rcVScroll.right, rcArrowDown.top);

		if (!rcTrack.IsRectEmpty())
		{
			CRect rcLowerTrack(rcTrack.left - 1, rcTrack.top, rcTrack.right + 1, rcTrack.top + nBtnTrackPos);
			CRect rcBtnTrack(rcTrack.left, rcLowerTrack.bottom, rcTrack.right, rcLowerTrack.bottom + nBtnTrackSize);
			CRect rcUpperTrack(rcTrack.left - 1, rcBtnTrack.bottom, rcTrack.right + 1, rcTrack.bottom);

			if (!rcLowerTrack.IsRectEmpty() && GETPARTSTATE2(XTP_HTSCROLLUPPAGE) == 3)
			{
				pImageBackground->DrawImage(pDC, rcLowerTrack,
					pImageBackground->GetSource(1, 2), CRect(1, 0, 1, 0), COLORREF_NULL, ST_STRETCH, FALSE);
			}

			if (!rcBtnTrack.IsRectEmpty())
			{
				DrawThumbButton(pDC, rcBtnTrack, GETPARTSTATE2(XTP_HTSCROLLTHUMB), FALSE);

				if (rcBtnTrack.Height() > 10)
					pClassScrollBar->DrawThemeBackground(pDC, SBP_GRIPPERVERT, 0,  rcBtnTrack);
			}

			if (!rcUpperTrack.IsRectEmpty() && GETPARTSTATE2(XTP_HTSCROLLDOWNPAGE) == 3)
			{
				pImageBackground->DrawImage(pDC, rcUpperTrack,
					pImageBackground->GetSource(1, 2), CRect(1, 0, 1, 0), COLORREF_NULL, ST_STRETCH, FALSE);
			}
		}
	}
}
