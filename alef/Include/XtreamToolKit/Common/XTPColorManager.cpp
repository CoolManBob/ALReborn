// XTPColorManager.cpp: implementation of the CXTPColorManager class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#include <math.h>

#include "XTPVC80Helpers.h"
#include "XTPWinThemeWrapper.h"
#include "XTPColorManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================
// CXTPPaintManagerColor class
//===========================================================================

CXTPPaintManagerColor::CXTPPaintManagerColor()
: m_clrStandardValue(COLORREF_NULL)
, m_clrCustomValue(COLORREF_NULL)
{

}

CXTPPaintManagerColor::CXTPPaintManagerColor(COLORREF clr)
: m_clrStandardValue(clr)
, m_clrCustomValue(COLORREF_NULL)
{

}

void CXTPPaintManagerColor::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << (DWORD)m_clrStandardValue;
		ar << (DWORD)m_clrCustomValue;
	}
	else
	{
		ar >> (DWORD)m_clrStandardValue;
		ar >> (DWORD)m_clrCustomValue;
	}
}

//===========================================================================
// CXTPPaintManagerColorGradient class
//===========================================================================

CXTPPaintManagerColorGradient::CXTPPaintManagerColorGradient()
{
	fGradientFactor = 0.5f;
}
CXTPPaintManagerColorGradient::CXTPPaintManagerColorGradient(const CXTPPaintManagerColor clr)
{
	clrLight = clr;
	clrDark = clr;
	fGradientFactor = 0.5f;
}

CXTPPaintManagerColorGradient::CXTPPaintManagerColorGradient(const COLORREF clr)
{
	fGradientFactor = 0.5f;
	SetStandardValue(clr);
}

CXTPPaintManagerColorGradient::CXTPPaintManagerColorGradient(COLORREF clrLight, COLORREF clrDark)
{
	fGradientFactor = 0.5f;
	SetStandardValue(clrLight, clrDark);
}

//---------------------------------------------------------------------------
// Settings for standard values
//---------------------------------------------------------------------------

void CXTPPaintManagerColorGradient::SetStandardValue(const COLORREF clr)
{
	SetStandardValue(clr, clr);
}

void CXTPPaintManagerColorGradient::SetStandardValue(COLORREF _clrLight, COLORREF _clrDark, float _fGradientFactor)
{
	clrLight.SetStandardValue(_clrLight);
	clrDark.SetStandardValue(_clrDark);
	fGradientFactor = _fGradientFactor;
}

void CXTPPaintManagerColorGradient::SetStandardValue(CXTPPaintManagerColorGradient& clr)
{
	clrLight.SetStandardValue(clr.clrLight);
	clrDark.SetStandardValue(clr.clrDark);
	fGradientFactor = clr.fGradientFactor;
}

//---------------------------------------------------------------------------
// Settings for custom values
//---------------------------------------------------------------------------

void CXTPPaintManagerColorGradient::SetCustomValue(const COLORREF clr)
{
	SetCustomValue(clr, clr);
}

void CXTPPaintManagerColorGradient::SetCustomValue(CXTPPaintManagerColorGradient& clr)
{
	clrLight.SetCustomValue(clr.clrLight);
	clrDark.SetCustomValue(clr.clrDark);
	fGradientFactor = clr.fGradientFactor;

}

void CXTPPaintManagerColorGradient::SetCustomValue(COLORREF _clrLight, COLORREF _clrDark)
{
	clrLight.SetCustomValue(_clrLight);
	clrDark.SetCustomValue(_clrDark);
}

void CXTPPaintManagerColorGradient::Serialize(CArchive& ar)
{
	clrLight.Serialize(ar);
	clrDark.Serialize(ar);

	if (ar.IsStoring())
	{
		ar << fGradientFactor;
	}
	else
	{
		ar >> fGradientFactor;
	}
}
//===========================================================================
// CXTPColorManager class
//===========================================================================
#pragma warning(disable  : 4244)

#define clrToolBar GetColor(XPCOLOR_TOOLBAR_FACE)
#define clrWindow GetColor(COLOR_WINDOW)
#define clrHighlight GetColor(COLOR_HIGHLIGHT)
#define pow2(x) pow(x, 2)
#define NORMVALUE(a, b) a = (a < 0) ? 0 : (a > b) ? b : a
//#define UNIQUEVALUE _T("173VEDHQQM1MQE5Z45GYJ4SVS8J77C1")

CXTPColorManager::CXTPColorManager()
{
	m_bDisableLunaColors = FALSE;
	m_systemTheme = xtpSystemThemeAuto;
	m_winThemeWrapperTheme = xtpSystemThemeUnknown;
	m_pfnGetSysColor = 0;

	m_bEnableLunaBlueForRoyaleTheme = FALSE;

	for (int i = 0; i <= XPCOLOR_LAST; i++)
		m_arrCustomColor[i] = m_arrLunaColor[i] = COLORREF_NULL;

	RefreshColors();

	grcLunaChecked.SetStandardValue(RGB(255, 213, 140), RGB(255, 173, 85));
	grcLunaPushed.SetStandardValue(RGB(254, 142, 75), RGB(255, 207, 139));
	grcLunaSelected.SetStandardValue(RGB(255, 242, 200), RGB(255, 212, 151));

}

CXTPColorManager& AFX_CDECL CXTPColorManager::Instance()
{
	static CXTPColorManager instance;
	return instance;
}


float CXTPColorManager::ColorWidth(int nLength, int nWidth)
{
	if (nLength > 220) nLength = 220;
	return (2.0 - (float)nLength / 220) * nWidth;
}


float CXTPColorManager::ColorDelta(COLORREF clrA, COLORREF clrB)
{
	return pow2(GetRDelta(clrA) - GetRDelta(clrB)) +
		pow2(GetGDelta(clrA) - GetGDelta(clrB)) +
		pow2(GetBDelta(clrA) - GetBDelta(clrB));
}

float CXTPColorManager::Length(COLORREF clrA, COLORREF clrB)
{
	return sqrt(ColorDelta(clrA, clrB));
}

BOOL CXTPColorManager::LongColor(COLORREF clrMain, COLORREF clrSub, BOOL bCalcLength, float fDistance)
{
	if (bCalcLength)
		fDistance = ColorWidth(Length(clrMain, 0), fDistance);

	return pow2(fDistance) < ColorDelta(clrMain, clrSub);
}

COLORREF CXTPColorManager::MixColor(COLORREF clrMain, COLORREF clrSub, float fDistance)
{
	float fMainLength = Length(clrMain, 0);

	float fGate = ColorWidth(fMainLength, fDistance);

	BOOL bReverse = TRUE;

	if (Length(clrSub, 0) > fMainLength)
	{
		if (fMainLength > 442 - fGate)
			bReverse = FALSE;

	}
	else
	{
		if (fMainLength > fGate)
			bReverse = FALSE;
	}

	float fSubRed = GetRDelta(clrSub);
	float fSubGreen = GetGDelta(clrSub);
	float fSubBlue = GetBDelta(clrSub);

	float fMainRed = GetRDelta(clrMain);
	float fMainGreen = GetGDelta(clrMain);
	float fMainBlue = GetBDelta(clrMain);

	if (bReverse)
	{
		fSubRed = 195.0 - fSubRed;
		fSubGreen = 390.0 - fSubGreen;
		fSubBlue = 65.0 - fSubBlue;

		fMainRed = 195.0 - fMainRed;
		fMainGreen = 390.0 - fMainGreen;
		fMainBlue = 65.0 - fMainBlue;
	}

	float A = __max(0.001, sqrt(pow2(fSubRed) + pow2(fSubGreen) + pow2(fSubBlue)));
	float B = fMainRed * fSubRed + fMainGreen * fSubGreen + fMainBlue * fSubBlue;
	float X = B / pow2(A);

	float fR = X * fSubRed - fMainRed;
	float fG = X * fSubGreen - fMainGreen;
	float fB = X * fSubBlue - fMainBlue;

	float Y = sqrt(pow2(fGate) - (pow2(fR) + pow2(fG) + pow2(fB)));

	float Z = (B / A - Y);

	fR = Z * fSubRed / A;
	fG = Z * fSubGreen / A;
	fB = Z * fSubBlue / A;

	if (bReverse)
	{
		fR = 195.0 - fR;
		fG = 390.0 - fG;
		fB = 65.0 - fB;
	}

	NORMVALUE(fR, 195.0);
	NORMVALUE(fG, 390.0);
	NORMVALUE(fB, 65.0);

	int nR = int(.5 + fR * (255.0/195.0));
	int nG = int(.5 + fG * (255.0/390.0));
	int nB = int(.5 + fB * (255.0/65.0));

	return RGB(NORMVALUE(nR, 255), NORMVALUE(nG, 255), NORMVALUE(nB, 255));
}

COLORREF CXTPColorManager::AdjustColor(COLORREF clrMain, COLORREF clrSub, float fDistance)
{
	float Z = sqrt(
		pow2(GetRDelta(clrMain) - GetRDelta(clrSub)) +
		pow2(GetGDelta(clrMain) - GetGDelta(clrSub)) +
		pow2(GetBDelta(clrMain) - GetBDelta(clrSub)));

	float Q = (Z - ColorWidth(Length(0, clrMain), fDistance)) / Z;

	float fR = Q * (GetRDelta(clrMain) - GetRDelta(clrSub)) + GetRDelta(clrSub);
	float fG = Q * (GetGDelta(clrMain) - GetGDelta(clrSub)) + GetGDelta(clrSub);
	float fB = Q * (GetBDelta(clrMain) - GetBDelta(clrSub)) + GetBDelta(clrSub);

	NORMVALUE(fR, 195.0);
	NORMVALUE(fG, 390.0);
	NORMVALUE(fB, 65.0);

	int nR = int(.5 + fR * (255.0/195.0));
	int nG = int(.5 + fG * (255.0/390.0));
	int nB = int(.5 + fB * (255.0/65.0));

	return RGB(NORMVALUE(nR, 255), NORMVALUE(nG, 255), NORMVALUE(nB, 255));
}

COLORREF CXTPColorManager::LightColor(COLORREF clrLight, COLORREF clrDark, int nDelta) const
{
	int nParam = (nDelta < 100 ? 100 : 1000);

	int nR = (GetRValue(clrDark) * (nParam - nDelta) + nParam / 2 +  GetRValue(clrLight) * nDelta) / nParam;
	int nG = (GetGValue(clrDark) * (nParam - nDelta) + nParam / 2 +  GetGValue(clrLight) * nDelta) / nParam;
	int nB = (GetBValue(clrDark) * (nParam - nDelta) + nParam / 2 +  GetBValue(clrLight) * nDelta) / nParam;

	return RGB(nR, nG, nB);
}

COLORREF CXTPColorManager::GetColor(int nIndex) const
{
	if (nIndex > XPCOLOR_LAST) return nIndex;
	return m_arrCustomColor[nIndex] == COLORREF_NULL ? m_arrStandardColor[nIndex]: m_arrCustomColor[nIndex];
}

void CXTPColorManager::RefreshSysColors()
{
	m_arrStandardColor[0] = 0;
	for (int i = 1; i < 30; i++) m_arrStandardColor[i] = (m_pfnGetSysColor != 0 ? (*m_pfnGetSysColor)(i) : GetSysColor(i));
}

void CXTPColorManager::SetGetSysColorPtr(PFNGETSYSCOLOR pfnGetSysColor)
{
	m_pfnGetSysColor = pfnGetSysColor;
	RefreshColors();
}

void CXTPColorManager::RefreshXPColors()
{
	COLORREF clr3DFace = GetColor(COLOR_3DFACE);
	COLORREF clr3DShadow = GetColor(COLOR_3DSHADOW);

	m_arrStandardColor[XPCOLOR_TOOLBAR_FACE] = clr3DFace;
	m_arrStandardColor[XPCOLOR_HIGHLIGHT] = GetColor(COLOR_WINDOW);
	m_arrStandardColor[XPCOLOR_HIGHLIGHT_PUSHED] = GetColor(COLOR_HIGHLIGHT);
	m_arrStandardColor[XPCOLOR_MENUBAR_FACE] = GetColor(COLOR_WINDOW);
	m_arrStandardColor[XPCOLOR_MENUBAR_GRAYTEXT] = GetColor(COLOR_GRAYTEXT);
	m_arrStandardColor[XPCOLOR_TOOLBAR_GRIPPER] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_SEPARATOR] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_MENUBAR_BORDER] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_FLOATBAR_BORDER] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_DISABLED] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_HIGHLIGHT_CHECKED] = GetColor(COLOR_WINDOW);
	m_arrStandardColor[XPCOLOR_HIGHLIGHT_BORDER] = GetColor(COLOR_HIGHLIGHT);
	m_arrStandardColor[XPCOLOR_HIGHLIGHT_CHECKED_BORDER] = GetColor(COLOR_HIGHLIGHT);
	m_arrStandardColor[XPCOLOR_MENUBAR_TEXT] = GetColor(COLOR_WINDOWTEXT);
	m_arrStandardColor[XPCOLOR_HIGHLIGHT_TEXT] = GetColor(COLOR_MENUTEXT);
	m_arrStandardColor[XPCOLOR_TOOLBAR_TEXT] = GetColor(COLOR_BTNTEXT);
	m_arrStandardColor[XPCOLOR_PUSHED_TEXT] = GetColor(COLOR_HIGHLIGHTTEXT);
	m_arrStandardColor[XPCOLOR_TAB_INACTIVE_BACK] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_TAB_INACTIVE_TEXT] = GetColor(COLOR_HIGHLIGHTTEXT);
	m_arrStandardColor[XPCOLOR_MENUBAR_EXPANDED] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_HIGHLIGHT_PUSHED_BORDER] = GetColor(COLOR_HIGHLIGHT);
	m_arrStandardColor[XPCOLOR_ICONSHADDOW] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_3DFACE] = clr3DFace;
	m_arrStandardColor[XPCOLOR_3DSHADOW] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_EDITCTRLBORDER] = clr3DFace;
	m_arrStandardColor[XPCOLOR_FRAME] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_SPLITTER_FACE] = clr3DFace;
	m_arrStandardColor[XPCOLOR_LABEL] = clr3DFace;
	m_arrStandardColor[XPCOLOR_STATICFRAME] = clr3DShadow;
	m_arrStandardColor[XPCOLOR_HIGHLIGHT_DISABLED_BORDER] = GetColor(COLOR_GRAYTEXT);
	m_arrStandardColor[XPCOLOR_SHADOW_FACTOR] = 0;
	m_arrStandardColor[XPCOLOR_TOOLBAR_GRAYTEXT] = clr3DShadow;
}

void CXTPColorManager::RefreshGradientColors()
{
	XTPCurrentSystemTheme systemTheme = GetCurrentSystemTheme();

	if (m_bDisableLunaColors)
		systemTheme = xtpSystemThemeUnknown;

	switch (systemTheme)
	{
	case xtpSystemThemeBlue:
		{
			m_arrStandardColor[XPCOLOR_FRAME] = RGB(0, 45, 150);
			grcCaption.SetStandardValue(RGB(89, 135, 214), RGB(0, 45, 150));
			grcDockBar.SetStandardValue(RGB(158, 190, 245), RGB(196, 218, 250));
			grcShortcutBarGripper.SetStandardValue(RGB(89, 135, 214), RGB(0, 45, 150));

			// Toolbar gradients.
			grcToolBar.SetStandardValue(RGB(221, 236, 254), RGB(129, 169, 226), 0.75f);

			// Menu gradients.
			grcMenu.SetStandardValue(RGB(227, 239, 255), RGB(135, 173, 228));
			grcMenuExpanded.SetStandardValue(RGB(203, 221, 246), RGB(121, 161, 220));
			grcMenuItemPopup.SetStandardValue(RGB(227, 239, 255), RGB(147, 181, 231));
		}
		break;

	case xtpSystemThemeOlive:
		{
			m_arrStandardColor[XPCOLOR_FRAME] = RGB(96, 128, 88);
			grcCaption.SetStandardValue(RGB(175, 192, 130), RGB(99, 122, 68));
			grcDockBar.SetStandardValue(RGB(217, 217, 167), RGB(242, 241, 228));
			grcShortcutBarGripper.SetStandardValue(RGB(120, 142, 111), RGB(73, 91, 67));

			// Toolbar gradients.
			grcToolBar.SetStandardValue(RGB(244, 247, 222), RGB(183, 198, 145), 0.3f);

			// Menu gradients.
			grcMenu.SetStandardValue(RGB(255, 255, 237), RGB(184, 199, 146), 0.3f);
			grcMenuExpanded.SetStandardValue(RGB(230, 230, 209), RGB(164, 180, 120), 0.3f);
			grcMenuItemPopup.SetStandardValue(RGB(236, 240, 213), RGB(194, 206, 159));
		}
		break;

	case xtpSystemThemeSilver:
		{
			m_arrStandardColor[XPCOLOR_FRAME] = RGB(124, 124, 148);
			grcCaption.SetStandardValue(RGB(168, 167, 191), RGB(112, 111, 145));
			grcDockBar.SetStandardValue(RGB(215, 215, 229), RGB(243, 243, 247));
			grcShortcutBarGripper.SetStandardValue(RGB(168, 167, 191), RGB(119, 118, 151));

			// Toolbar gradients.
			grcToolBar.SetStandardValue(RGB(249, 249, 255), RGB(156, 155, 183), 0.75f);

			// Menu gradients.
			grcMenu.SetStandardValue(RGB(249, 249, 255), RGB(159, 157, 185), 0.75f);
			grcMenuExpanded.SetStandardValue(RGB(215, 215, 226), RGB(128, 126, 158), 0.75f);
			grcMenuItemPopup.SetStandardValue(RGB(233, 231, 241), RGB(186, 185, 205));
		}
		break;

	default:
		{
			grcCaption.SetStandardValue(GetColor(COLOR_3DSHADOW), GetColor(COLOR_3DSHADOW));
			grcDockBar.SetStandardValue(GetColor(COLOR_3DFACE), LightColor(GetColor(COLOR_3DFACE), GetColor(COLOR_WINDOW), 0xd7));
			grcShortcutBarGripper.SetStandardValue(GetColor(COLOR_3DFACE), GetColor(COLOR_3DSHADOW));

			// Toolbar gradients.
			grcToolBar.SetStandardValue(LightColor(GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_WINDOW), 0xcd), GetXtremeColor(COLOR_3DFACE));

			// Menu gradients.
			grcMenu.SetStandardValue(LightColor(GetColor(COLOR_3DFACE), GetColor(COLOR_WINDOW), 0x91), LightColor(GetColor(COLOR_3DFACE), GetColor(COLOR_WINDOW), 0x5d));
			grcMenuExpanded.SetStandardValue(LightColor(GetColor(COLOR_3DFACE), GetColor(COLOR_WINDOW), 0x28), LightColor(GetColor(COLOR_3DFACE), GetColor(COLOR_3DSHADOW), 0x5f));
			grcMenuItemPopup.SetStandardValue(LightColor(GetColor(COLOR_3DFACE), GetColor(COLOR_WINDOW), 0xa5), LightColor(GetColor(COLOR_3DFACE), GetColor(COLOR_WINDOW), 0x2a));
		}
		break;
	}

	if (IsLowResolution())
	{
		grcToolBar.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
		grcDockBar.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
	}
}

_XTP_EXT_CLASS void AFX_CDECL RefreshXtremeColors()
{
#if 1
	XTPColorManager()->RefreshColors();
#else
	static DWORD dwLastRefresh = 0;

	if (GetTickCount() - dwLastRefresh > 500)
	{
		XTPColorManager()->RefreshColors();

		if (AfxGetMainWnd() != NULL)
		{
			dwLastRefresh = GetTickCount();
		}
	}
#endif
}

BOOL CXTPColorManager::IsLowResolution(HDC hDC/* = 0*/)
{
	if (hDC)
	{
		int nColors = ::GetDeviceCaps(hDC, BITSPIXEL);
		return (nColors > 0 && nColors <= 8);
	}
	hDC = ::GetDC(::GetDesktopWindow());
	if (hDC)
	{
		int nColors = ::GetDeviceCaps(hDC, BITSPIXEL);
		::ReleaseDC(::GetDesktopWindow(), hDC);
		return (nColors > 0 && nColors <= 8);
	}
	return FALSE;
}

void CXTPColorManager::RefreshColors()
{
	m_winThemeWrapperTheme = _GetWinThemeWrapperTheme();

	BOOL bSimpleColors = IsLowResolution();

	RefreshSysColors();
	RefreshXPColors();
	RefreshLunaColors();

	if (bSimpleColors)
	{
		RefreshGradientColors();
		return;
	}

	struct MUTECOLOR
	{
		int nIndex;
		int clrMain;
		int clrSub;
		int nDistance;
	};

	const MUTECOLOR IndexTable[] =
	{
		{ XPCOLOR_TOOLBAR_FACE, COLOR_WINDOW, COLOR_3DFACE, 165 },
		{ XPCOLOR_HIGHLIGHT, COLOR_HIGHLIGHT, COLOR_WINDOW, 0x1E },
		{ XPCOLOR_HIGHLIGHT_PUSHED, COLOR_HIGHLIGHT, COLOR_WINDOW, 0x32 },
		{ XPCOLOR_MENUBAR_FACE, COLOR_3DFACE, COLOR_WINDOW, 0x8f },
		{ XPCOLOR_MENUBAR_GRAYTEXT, COLOR_GRAYTEXT, COLOR_WINDOW, 0x46 },
		{ XPCOLOR_TOOLBAR_GRIPPER, COLOR_3DSHADOW, COLOR_WINDOW, 0x4b },
		{ XPCOLOR_SEPARATOR, COLOR_3DSHADOW, COLOR_WINDOW, 0x46 },
		{ XPCOLOR_MENUBAR_BORDER, XPCOLOR_TOOLBAR_TEXT, COLOR_BTNSHADOW, 0x14 },
		{ XPCOLOR_FLOATBAR_BORDER, XPCOLOR_TOOLBAR_TEXT, COLOR_BTNSHADOW, 0xF },
		{ XPCOLOR_DISABLED, COLOR_3DSHADOW, COLOR_WINDOW, 0x5A },
		{ XPCOLOR_MENUBAR_EXPANDED, COLOR_3DFACE, COLOR_3DSHADOW, 90 }
	};

	int i;
	for (i = 0; i < sizeof(IndexTable) / sizeof(IndexTable[0]); i++)
	{
		m_arrStandardColor[IndexTable[i].nIndex] = LightColor(GetColor(IndexTable[i].clrMain),
			GetColor(IndexTable[i].clrSub), IndexTable[i].nDistance);

	}

	const MUTECOLOR LongTable[] =
	{
		{ XPCOLOR_HIGHLIGHT, XPCOLOR_TOOLBAR_FACE, XPCOLOR_HIGHLIGHT, 50 },
		{ XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_TOOLBAR_FACE, COLOR_HIGHLIGHT, 100 },
		{ XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_HIGHLIGHT_PUSHED, 30  },
		{ XPCOLOR_MENUBAR_GRAYTEXT, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_GRAYTEXT, 80 },
		{ XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, 100 },
		{ XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_TOOLBAR_FACE, XPCOLOR_TOOLBAR_GRIPPER, 85  },
		{ XPCOLOR_SEPARATOR, XPCOLOR_TOOLBAR_FACE, XPCOLOR_SEPARATOR, 50 },
		{ XPCOLOR_MENUBAR_BORDER, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, 100 },
		{ XPCOLOR_FLOATBAR_BORDER, XPCOLOR_TOOLBAR_FACE, XPCOLOR_FLOATBAR_BORDER, 100 },
		{ XPCOLOR_DISABLED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_DISABLED, 80 },
		{ XPCOLOR_MENUBAR_TEXT, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_TEXT, 180 },
		{ XPCOLOR_HIGHLIGHT_TEXT, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_TEXT, 180 },
		{ XPCOLOR_TOOLBAR_TEXT, XPCOLOR_TOOLBAR_FACE, XPCOLOR_TOOLBAR_TEXT, 180 },
		{ XPCOLOR_PUSHED_TEXT, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_PUSHED_TEXT, 180 }
	};

	if (LongColor(GetColor(COLOR_3DFACE), GetColor(XPCOLOR_TOOLBAR_FACE), 1, 35))
	{
		m_arrStandardColor[XPCOLOR_TOOLBAR_FACE] = AdjustColor(GetColor(COLOR_3DFACE), GetColor(XPCOLOR_TOOLBAR_FACE), 35);
	}

	for (i = 0; i < sizeof(LongTable) / sizeof(LongTable[0]); i++)
	{
		if (!LongColor(GetColor(LongTable[i].clrMain), GetColor(LongTable[i].clrSub) , 1, LongTable[i].nDistance))
		{
			m_arrStandardColor[LongTable[i].nIndex] = MixColor(GetColor(LongTable[i].clrMain), GetColor(LongTable[i].clrSub), LongTable[i].nDistance);
		}
	}

	if (LongColor(GetColor(XPCOLOR_MENUBAR_FACE), GetColor(XPCOLOR_MENUBAR_GRAYTEXT), 1, 145))
	{

		m_arrStandardColor[XPCOLOR_MENUBAR_GRAYTEXT] = AdjustColor(GetColor(XPCOLOR_MENUBAR_FACE), GetColor(XPCOLOR_MENUBAR_GRAYTEXT), 145);
	}

	m_arrStandardColor[XPCOLOR_HIGHLIGHT_CHECKED] =
		RGB(GetRValue(clrWindow) * 40 / 100 + GetRValue(clrHighlight) * 10 / 100 + GetRValue(clrToolBar) * 50 / 100,
		GetGValue(clrWindow) * 40 / 100 + GetGValue(clrHighlight) * 10 / 100 + GetGValue(clrToolBar) * 50 / 100,
		GetBValue(clrWindow) * 40 / 100 + GetBValue(clrHighlight) * 10 / 100 + GetBValue(clrToolBar) * 50 / 100);


	COLORREF clrBtn = GetColor(COLOR_3DFACE);
	int r = GetRValue(clrBtn), g = GetGValue(clrBtn), b = GetBValue(clrBtn);
	int nMax = __max(__max(r, g), b);
	if (nMax == 0)
		m_arrStandardColor[XPCOLOR_TAB_INACTIVE_BACK] = RGB(35, 35, 35); else
	{
		int nDelta = __min(35, 255 - nMax) + nMax;
		m_arrStandardColor[XPCOLOR_TAB_INACTIVE_BACK] = RGB(r * nDelta / nMax, g * nDelta / nMax, b * nDelta / nMax);
	}

	COLORREF clrShadow = GetColor(COLOR_3DSHADOW);
	r = GetRValue(clrShadow);
	g = GetGValue(clrShadow);
	b = GetBValue(clrShadow);
	nMax = __max(__max(r, g), b);

	if (clrBtn == 0) m_arrStandardColor[XPCOLOR_TAB_INACTIVE_TEXT] = RGB(0x7f, 0x7f, 0x7f);
	else if (nMax == 0) m_arrStandardColor[XPCOLOR_TAB_INACTIVE_TEXT] = RGB(0, 0, 0);
	else
	{
		int nDelta = __max(0, nMax - 43);
		m_arrStandardColor[XPCOLOR_TAB_INACTIVE_TEXT] = RGB(r * nDelta / nMax, g * nDelta / nMax, b * nDelta / nMax);
	}

	COLORREF clrBackground = m_arrStandardColor[XPCOLOR_HIGHLIGHT];
	m_arrStandardColor[XPCOLOR_ICONSHADDOW] = RGB(GetRValue(clrBackground) * .75, GetGValue(clrBackground) * .75, GetBValue(clrBackground) * .75);

	m_arrStandardColor[COLOR_3DLIGHT] = RGB(min(GetRValue(clrBtn) + 15, 255), min(GetGValue(clrBtn) + 15, 255), min(GetBValue(clrBtn) + 15, 255));
	m_arrStandardColor[XPCOLOR_EDITCTRLBORDER] = GetColor(XPCOLOR_TOOLBAR_FACE);
	m_arrStandardColor[XPCOLOR_HIGHLIGHT_DISABLED_BORDER] = GetColor(XPCOLOR_DISABLED);
	m_arrStandardColor[XPCOLOR_TOOLBAR_GRAYTEXT] = m_arrStandardColor[XPCOLOR_DISABLED];

	RefreshGradientColors();

	CXTPWinThemeWrapper wrpTreeView;
	wrpTreeView.OpenTheme(0, L"TREEVIEW");
	if (wrpTreeView.IsAppThemed())
	{
		wrpTreeView.GetThemeColor(0, 0, TMT_BORDERCOLOR, &m_arrStandardColor[XPCOLOR_STATICFRAME]);
	}
}

COLORREF CXTPColorManager::GetLunaColor(int nIndex) const
{
	// index out of range try standard colors
	if (nIndex < XPCOLOR_BASE || nIndex > XPCOLOR_LAST)
		return GetColor(nIndex);

	return (m_arrLunaColor[nIndex] == COLORREF_NULL) ? GetColor(nIndex) : m_arrLunaColor[nIndex];
}

void CXTPColorManager::RefreshLunaColors()
{
	XTPCurrentSystemTheme systemTheme = GetCurrentSystemTheme();

	if (m_bDisableLunaColors)
		systemTheme = xtpSystemThemeUnknown;

	int nLunaElements[] =
	{
		XPCOLOR_TOOLBAR_GRIPPER,
		XPCOLOR_SEPARATOR,
		XPCOLOR_DISABLED,
		XPCOLOR_MENUBAR_FACE,
		XPCOLOR_MENUBAR_BORDER,
		XPCOLOR_HIGHLIGHT,
		XPCOLOR_HIGHLIGHT_BORDER,
		XPCOLOR_HIGHLIGHT_PUSHED_BORDER,
		XPCOLOR_HIGHLIGHT_CHECKED_BORDER,
		XPCOLOR_HIGHLIGHT_PUSHED,
		XPCOLOR_HIGHLIGHT_CHECKED,
		XPCOLOR_TOOLBAR_FACE,
		XPCOLOR_PUSHED_TEXT,
		XPCOLOR_SHADOW_FACTOR
	};

	switch (systemTheme)
	{
	case xtpSystemThemeBlue:
		{
			COLORREF clrValues[] =
			{
				RGB(0x27, 0x41, 0x76), // XPCOLOR_TOOLBAR_GRIPPER
				RGB(0x6a, 0x8c, 0xcb), // XPCOLOR_SEPARATOR
				RGB(0x6d, 0x96, 0xd0), // XPCOLOR_DISABLED
				RGB(0xf6, 0xf6, 0xf6), // XPCOLOR_MENUBAR_FACE
				RGB(0x00, 0x2d, 0x96), // XPCOLOR_MENUBAR_BORDER
				RGB(0xff, 0xee, 0xc2), // XPCOLOR_HIGHLIGHT
				RGB(0x00, 0x00, 0x80), // XPCOLOR_HIGHLIGHT_BORDER
				RGB(0x00, 0x00, 0x80), // XPCOLOR_HIGHLIGHT_PUSHED_BORDER
				RGB(0x00, 0x00, 0x80), // XPCOLOR_HIGHLIGHT_CHECKED_BORDER
				RGB(0xfe, 0x80, 0x3e), // XPCOLOR_HIGHLIGHT_PUSHED
				RGB(0xff, 0xc0, 0x6f), // XPCOLOR_HIGHLIGHT_CHECKED
				RGB(0x9e, 0xbe, 0xf5), // XPCOLOR_TOOLBAR_FACE
				RGB(0x00, 0x00, 0x00), // XPCOLOR_PUSHED_TEXT
				RGB(0x16, 0x1E, 0x36), // XPCOLOR_SHADOW_FACTOR
				LightColor(GetColor(COLOR_APPWORKSPACE), GetColor(COLOR_WINDOW), 0x5a)
			};

			for (int i = 0; i < _countof(nLunaElements); i++)
				m_arrLunaColor[nLunaElements[i]] = clrValues[i];
		}
		break;

	case xtpSystemThemeOlive:
		{
			COLORREF clrValues[] =
			{
				RGB(0x51, 0x5e, 0x33), // XPCOLOR_TOOLBAR_GRIPPER
				RGB(0x60, 0x80, 0x58), // XPCOLOR_SEPARATOR
				RGB(0x9f, 0xae, 0x7a), // XPCOLOR_DISABLED
				RGB(0xf4, 0xf4, 0xee), // XPCOLOR_MENUBAR_FACE
				RGB(0x75, 0x8d, 0x5e), // XPCOLOR_MENUBAR_BORDER
				RGB(0xff, 0xee, 0xc2), // XPCOLOR_HIGHLIGHT
				RGB(0x3f, 0x5d, 0x38), // XPCOLOR_HIGHLIGHT_BORDER
				RGB(0x3f, 0x5d, 0x38), // XPCOLOR_HIGHLIGHT_PUSHED_BORDER
				RGB(0x3f, 0x5d, 0x38), // XPCOLOR_HIGHLIGHT_CHECKED_BORDER
				RGB(0xfe, 0x80, 0x3e), // XPCOLOR_HIGHLIGHT_PUSHED
				RGB(0xff, 0xc0, 0x6f), // XPCOLOR_HIGHLIGHT_CHECKED
				RGB(0xd9, 0xd9, 0xa7), // XPCOLOR_TOOLBAR_FACE
				RGB(0x00, 0x00, 0x00), // XPCOLOR_PUSHED_TEXT
				RGB(0x1F, 0x22, 0x19), // XPCOLOR_SHADOW_FACTOR
				LightColor(GetColor(COLOR_APPWORKSPACE), GetColor(COLOR_WINDOW), 0x5a)
			};

			for (int i = 0; i < _countof(nLunaElements); i++)
				m_arrLunaColor[nLunaElements[i]] = clrValues[i];
		}
		break;

	case xtpSystemThemeSilver:
		{
			COLORREF clrValues[] =
			{
				RGB(0x54, 0x54, 0x75), // XPCOLOR_TOOLBAR_GRIPPER
				RGB(0x6e, 0x6d, 0x8f), // XPCOLOR_SEPARATOR
				RGB(0xa8, 0xa7, 0xbe), // XPCOLOR_DISABLED
				RGB(0xfd, 0xfa, 0xff), // XPCOLOR_MENUBAR_FACE
				RGB(0x7c, 0x7c, 0x94), // XPCOLOR_MENUBAR_BORDER
				RGB(0xff, 0xee, 0xc2), // XPCOLOR_HIGHLIGHT
				RGB(0x4b, 0x4b, 0x6f), // XPCOLOR_HIGHLIGHT_BORDER
				RGB(0x4b, 0x4b, 0x6f), // XPCOLOR_HIGHLIGHT_PUSHED_BORDER
				RGB(0x4b, 0x4b, 0x6f), // XPCOLOR_HIGHLIGHT_CHECKED_BORDER
				RGB(0xfe, 0x80, 0x3e), // XPCOLOR_HIGHLIGHT_PUSHED
				RGB(0xff, 0xc0, 0x6f), // XPCOLOR_HIGHLIGHT_CHECKED
				RGB(0xd7, 0xd7, 0xe5), // XPCOLOR_TOOLBAR_FACE
				RGB(0x00, 0x00, 0x00), // XPCOLOR_PUSHED_TEXT
				RGB(0x24, 0x20, 0x29), // XPCOLOR_SHADOW_FACTOR
				LightColor(GetColor(COLOR_APPWORKSPACE), GetColor(COLOR_WINDOW), 0x5a)
			};

			for (int i = 0; i < _countof(nLunaElements); i++)
				m_arrLunaColor[nLunaElements[i]] = clrValues[i];
		}
		break;

	default:
		{
			for (int i = 0; i <= XPCOLOR_LAST; i++)
				m_arrLunaColor[i] = COLORREF_NULL;
		}
		break;
	}
}

void CXTPColorManager::SetColors(int cElements, CONST INT* lpaElements, CONST COLORREF* lpaRgbValues)
{
	for (int i = 0; i < cElements; i++)
		m_arrCustomColor[lpaElements[i]] = lpaRgbValues[i];
}

void CXTPColorManager::SetColor(int nIndex, COLORREF clrValue)
{
	m_arrCustomColor[nIndex] = clrValue;
}

double CXTPColorManager::GetRDelta(COLORREF clr)
{
	return GetRValue(clr) * (195.0/255.0);
}

double CXTPColorManager::GetGDelta(COLORREF clr)
{
	return GetGValue(clr) * (390.0/255.0);
}

double CXTPColorManager::GetBDelta(COLORREF clr)
{
	return GetBValue(clr) * (65.0/255.0);
}

void CXTPColorManager::DisableLunaColors(BOOL bDisable)
{
	m_bDisableLunaColors = bDisable;
}

BOOL CXTPColorManager::IsLunaColorsDisabled() const
{
	return m_bDisableLunaColors;
}

void CXTPColorManager::SetLunaTheme(XTPCurrentSystemTheme systemTheme)
{
	m_systemTheme = systemTheme;
	RefreshColors();
}

XTPCurrentSystemTheme CXTPColorManager::GetWinThemeWrapperTheme() const
{
	return m_winThemeWrapperTheme;
}

XTPCurrentSystemTheme CXTPColorManager::_GetWinThemeWrapperTheme()
{
	// windows classic theme is in use we do not need to go further.
	if (!CXTPWinThemeWrapper().IsThemeActive())
		return xtpSystemThemeUnknown;

	if (IsLowResolution())
		return xtpSystemThemeUnknown;

	WCHAR pszThemeFileName[MAX_PATH];
	WCHAR pszColorBuff[MAX_PATH];
	WCHAR pszSizeBuff[MAX_PATH];

	ZeroMemory(pszThemeFileName, MAX_PATH);
	ZeroMemory(pszColorBuff, MAX_PATH);
	ZeroMemory(pszSizeBuff, MAX_PATH);

	// get the name of the current theme in use.
	if (CXTPWinThemeWrapper().GetCurrentThemeName(pszThemeFileName, MAX_PATH,
		pszColorBuff, MAX_PATH, pszSizeBuff, MAX_PATH) != S_OK)
	{
		return xtpSystemThemeUnknown;
	}

	// search for "luna.msstyles" string in theme name.
	if (wcsstr(WCSLWR_S(pszThemeFileName, MAX_PATH), L"luna.msstyles") ||
		wcsstr(WCSLWR_S(pszThemeFileName, MAX_PATH), L"winxp.luna.cjstyles"))
	{
		if (wcscmp(WCSLWR_S(pszColorBuff, MAX_PATH), L"normalcolor") == 0)
			return xtpSystemThemeBlue;

		if (wcscmp(WCSLWR_S(pszColorBuff, MAX_PATH), L"homestead") == 0)
			return xtpSystemThemeOlive;

		if (wcscmp(WCSLWR_S(pszColorBuff, MAX_PATH), L"metallic") == 0)
			return xtpSystemThemeSilver;
	}

	if (m_bEnableLunaBlueForRoyaleTheme)
	{
		// search for "royale.msstyles" (Windows Media Center) string in theme name.
		if (wcsstr(WCSLWR_S(pszThemeFileName, MAX_PATH), L"royale.msstyles") ||
			wcsstr(WCSLWR_S(pszThemeFileName, MAX_PATH), L"winxp.royale.cjstyles"))
		{
			return xtpSystemThemeBlue;
		}
	}

	// theme was found just not luna theme.
	return xtpSystemThemeUnknown;
}

XTPCurrentSystemTheme CXTPColorManager::GetCurrentSystemTheme() const
{
	if (m_systemTheme != xtpSystemThemeAuto && m_systemTheme != xtpSystemThemeDefault)
		return m_systemTheme;

	return m_winThemeWrapperTheme;
}


