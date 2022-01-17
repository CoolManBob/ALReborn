// XTSplitterWndTheme.cpp: implementation of the XTSplitterWndTheme class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTThemeManager.h"
#include "XTSplitterWndTheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_THEME_FACTORY(CXTSplitterWndTheme)

/////////////////////////////////////////////////////////////////////////////
// CXTSplitterWndTheme

CXTSplitterWndTheme::CXTSplitterWndTheme()
{
}

void CXTSplitterWndTheme::RefreshMetrics()
{
	CXTThemeManagerStyle ::RefreshMetrics();

	m_clrSplitterFace = GetSysColor(COLOR_3DFACE);
	m_clrSplitterBorders = GetSysColor(COLOR_3DSHADOW);
}

/////////////////////////////////////////////////////////////////////////////
// CXTSplitterWndThemeOfficeXP

void CXTSplitterWndThemeOfficeXP::RefreshMetrics()
{
	CXTSplitterWndTheme::RefreshMetrics();
}

/////////////////////////////////////////////////////////////////////////////
// CXTSplitterWndThemeOffice2003

void CXTSplitterWndThemeOffice2003::RefreshMetrics()
{
	CXTSplitterWndThemeOfficeXP::RefreshMetrics();

	if (!XTPColorManager()->IsLunaColorsDisabled())
	{
		XTPCurrentSystemTheme systemTheme = XTPColorManager()->GetCurrentSystemTheme();

		switch (systemTheme)
		{
		case xtpSystemThemeBlue:
			m_clrSplitterFace = RGB(216, 231, 252);
			m_clrSplitterBorders = RGB(158, 190, 245);
			break;

		case xtpSystemThemeOlive:
			m_clrSplitterFace = RGB(226, 231, 191);
			m_clrSplitterBorders = RGB(171, 192, 138);
			break;

		case xtpSystemThemeSilver:
			m_clrSplitterFace = RGB(223, 223, 234);
			m_clrSplitterBorders = RGB(177, 176, 195);
			break;
		}

	}
}
