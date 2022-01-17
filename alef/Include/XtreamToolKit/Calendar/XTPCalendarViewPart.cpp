// XTPCalendarViewPart.cpp : implementation file
//
// This file is a part of the XTREME CALENDAR MFC class library.
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

#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPPropExchange.h"

#include "XTPCalendarViewPart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarViewPart

CXTPCalendarViewPart::CXTPCalendarViewPart(CXTPCalendarViewPart* pParentPart)
		: m_pParentPart(pParentPart)
{
	m_pPaintManager = NULL;
}

CXTPCalendarViewPart::~CXTPCalendarViewPart()
{
}

void CXTPCalendarViewPart::RefreshMetrics()
{
}



COLORREF CXTPCalendarViewPart::GetBackgroundColor()
{
	CXTPCalendarViewPart* pViewPart = this;
	for (;;)
	{
		if (!pViewPart->m_clrBackground.IsDefaultValue())
			return pViewPart->m_clrBackground;

		pViewPart = pViewPart->m_pParentPart;
		ASSERT(pViewPart);
	};
}

COLORREF CXTPCalendarViewPart::GetTextColor()
{
	CXTPCalendarViewPart* pViewPart = this;
	for (;;)
	{
		if (!pViewPart->m_clrTextColor.IsDefaultValue())
			return pViewPart->m_clrTextColor;

		pViewPart = pViewPart->m_pParentPart;
		ASSERT(pViewPart);
	};
}

CFont& CXTPCalendarViewPart::GetTextFont()
{
	CXTPCalendarViewPart* pViewPart = this;

	for (;;)
	{
		if (!pViewPart->m_fntText.IsDefaultValue())
			return pViewPart->m_fntText;

		pViewPart = pViewPart->m_pParentPart;
		ASSERT(pViewPart);
	};

}

BOOL AFX_CDECL PX_Font(CXTPPropExchange* pPX, LPCTSTR pcszPropName, LOGFONT& rLogFont)
{
	CXTPPropExchangeSection secLF(pPX->GetSection(pcszPropName));

	if (pPX->IsLoading())
	{
		::ZeroMemory(&rLogFont, sizeof(rLogFont));
	}
	else
	{
		secLF->EmptySection();
	}

	long nVersion = 1;
	PX_Long(&secLF, _T("Version"), nVersion, 1);

	if (nVersion != 1 && pPX->IsLoading())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	PX_Long(&secLF, _T("Height"), rLogFont.lfHeight, 0);
	PX_Long(&secLF, _T("Width"), rLogFont.lfWidth, 0);
	PX_Long(&secLF, _T("Escapement"), rLogFont.lfEscapement, 0);
	PX_Long(&secLF, _T("Orientation"), rLogFont.lfOrientation, 0);
	PX_Long(&secLF, _T("Weight"), rLogFont.lfWeight, 0);

	PX_Byte(&secLF, _T("Italic"), rLogFont.lfItalic, 0);
	PX_Byte(&secLF, _T("Underline"), rLogFont.lfUnderline, 0);
	PX_Byte(&secLF, _T("StrikeOut"), rLogFont.lfStrikeOut, 0);
	PX_Byte(&secLF, _T("CharSet"), rLogFont.lfCharSet, 0);
	PX_Byte(&secLF, _T("OutPrecision"), rLogFont.lfOutPrecision, 0);
	PX_Byte(&secLF, _T("ClipPrecision"), rLogFont.lfClipPrecision, 0);
	PX_Byte(&secLF, _T("Quality"), rLogFont.lfQuality, 0);
	PX_Byte(&secLF, _T("PitchAndFamily"), rLogFont.lfPitchAndFamily, 0);

	CString strFaceName;
	if (pPX->IsStoring())
	{
		strFaceName = rLogFont.lfFaceName;
		PX_String(&secLF, _T("FaceName"), strFaceName);
	}
	else
	{
		PX_String(&secLF, _T("FaceName"), strFaceName);
		STRCPY_S(rLogFont.lfFaceName, LF_FACESIZE, strFaceName);
	}

	return TRUE;
}

BOOL AFX_CDECL PX_Font(CXTPPropExchange* pPX, LPCTSTR pcszPropName,
					   CXTPCalendarThemeFontValue& refFont)
{
	CFont* pFontStandard = refFont.GetStandardValue();
	CFont* pFontCustom = refFont.IsCustomValue() ? refFont.GetValue() : NULL;

	LOGFONT lfStandard0, lfCustom0;

	if (!pFontStandard || !pFontStandard->GetLogFont(&lfStandard0))
	{
		::ZeroMemory(&lfStandard0, sizeof(lfStandard0));
	}

	if (!pFontCustom || !pFontCustom->GetLogFont(&lfCustom0))
	{
		::ZeroMemory(&lfCustom0, sizeof(lfCustom0));
	}

	//-------------------------------------------------------------------
	CXTPPropExchangeSection secData(pPX->GetSection(pcszPropName));
	if (pPX->IsStoring())
	{
		secData->EmptySection();

		PX_Font(&secData, _T("Standard"), lfStandard0);
		PX_Font(&secData, _T("Custom"), lfCustom0);
	}
	else
	{
		ASSERT(pPX->IsLoading());

		LOGFONT lfStandard, lfCustom;

		PX_Font(&secData, _T("Standard"), lfStandard);
		PX_Font(&secData, _T("Custom"), lfCustom);

		if (memcmp(&lfStandard, &lfStandard0, sizeof(lfStandard)))
		{
			if ((lfStandard.lfHeight || lfStandard.lfWidth || lfStandard.lfFaceName[0]) )
				refFont.SetStandardValue(&lfStandard);
			else
				refFont.SetStandardValue((CFont*)NULL);
		}

		if (memcmp(&lfCustom, &lfCustom0, sizeof(lfCustom)))
		{
			if (lfCustom.lfHeight || lfCustom.lfWidth || lfCustom.lfFaceName[0] )
				refFont.SetCustomValue(&lfCustom);
			else
				refFont.SetDefaultValue();
		}
	}
	return TRUE;
}

BOOL AFX_CDECL PX_Color(CXTPPropExchange* pPX, LPCTSTR pcszPropName, COLORREF& refColor)
{
	ASSERT(pPX && pcszPropName);
	if (!pPX || !pcszPropName)
		return FALSE;

	CString strColor;

	// save/load color as hex number
	if (pPX->IsStoring())
	{
		if (refColor == COLORREF_NULL)
			strColor = _T("-1");
		else
			strColor.Format(_T("%06X"), refColor);

		return PX_String(pPX, pcszPropName, strColor);
	}
	else
	{
		PX_String(pPX, pcszPropName, strColor, _T("-1"));

		REMOVE_S(strColor, _T(' '));

		if (strColor == _T("-1"))
		{
			refColor = COLORREF_NULL;
		}
		else
		{
			TCHAR* pEnd = NULL;
			refColor = (COLORREF)_tcstol(strColor, &pEnd, 16);
		}
	}

	return TRUE;
}

BOOL AFX_CDECL PX_Color(CXTPPropExchange* pPX, LPCTSTR pcszPropName, CXTPPaintManagerColor& refColor)
{
	class CXTPPaintManagerColor_accesser: public CXTPPaintManagerColor
	{
	public:
		void doPX(CXTPPropExchange* pPX, LPCTSTR pcszPropName)
		{
			CXTPPropExchangeSection secCLR(pPX->GetSection(pcszPropName));

			if (pPX->IsStoring())
				secCLR->EmptySection();

			PX_Color(&secCLR, _T("Standard"),   m_clrStandardValue);
			PX_Color(&secCLR, _T("Custom"),     m_clrCustomValue);
		}
	};

	CXTPPaintManagerColor_accesser* pClrEx = (CXTPPaintManagerColor_accesser*)&refColor;
	pClrEx->doPX(pPX, pcszPropName);

	return TRUE;
}

BOOL AFX_CDECL PX_GrColor(CXTPPropExchange* pPX, LPCTSTR psczPropName,
										 CXTPPaintManagerColorGradient& refGrColor)
{
	CXTPPropExchangeSection secData(pPX->GetSection(psczPropName));

	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("LightColor"), refGrColor.clrLight);
	PX_Color(&secData, _T("DarkColor"), refGrColor.clrDark);
	PX_Float(&secData, _T("GradientFactor"), refGrColor.fGradientFactor, 0.5);

	return TRUE;
}

void CXTPCalendarThemeFontValue::Serialize(CArchive& ar)
{
	LOGFONT lfStandard0, lfCustom0;

	const UINT cnLogFont_size = sizeof(LOGFONT);

	if (!m_pValueStandard || !m_pValueStandard->GetLogFont(&lfStandard0))
	{
		::ZeroMemory(&lfStandard0, cnLogFont_size);
	}

	if (!m_pValueCustom || !m_pValueCustom->GetLogFont(&lfCustom0))
	{
		::ZeroMemory(&lfCustom0, cnLogFont_size);
	}

	//-------------------------------------------------------------------
	if (ar.IsStoring())
	{
		ar.Write((void*)&lfStandard0, cnLogFont_size);
		ar.Write((void*)&lfCustom0, cnLogFont_size);
	}
	else
	{
		ASSERT(ar.IsLoading());

		LOGFONT lfStandard, lfCustom;

		ar.Read((void*)&lfStandard, cnLogFont_size);
		ar.Read((void*)&lfCustom, cnLogFont_size);

		if (memcmp(&lfStandard, &lfStandard0, cnLogFont_size))
		{
			if ((lfStandard.lfHeight || lfStandard.lfWidth || lfStandard.lfFaceName[0]) )
				SetStandardValue(&lfStandard);
			else
				SetStandardValue((CFont*)NULL);
		}

		if (memcmp(&lfCustom, &lfCustom0, cnLogFont_size))
		{
			if (lfCustom.lfHeight || lfCustom.lfWidth || lfCustom.lfFaceName[0] )
				SetCustomValue(&lfCustom);
			else
				SetDefaultValue();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
