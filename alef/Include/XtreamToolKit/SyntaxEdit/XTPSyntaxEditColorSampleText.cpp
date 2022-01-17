// XTPSyntaxEditColorSampleText.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"

// common includes
#include "Common/XTPDrawHelpers.h"

// syntax editor includes
#include "XTPSyntaxEditColorSampleText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditColorSampleText

CXTPSyntaxEditColorSampleText::CXTPSyntaxEditColorSampleText()
: m_crBack(::GetSysColor(COLOR_WINDOW))
, m_crText(::GetSysColor(COLOR_WINDOWTEXT))
, m_crBorder(::GetSysColor(COLOR_3DDKSHADOW))
{
}

CXTPSyntaxEditColorSampleText::~CXTPSyntaxEditColorSampleText()
{
}

BEGIN_MESSAGE_MAP(CXTPSyntaxEditColorSampleText, CStatic)
	//{{AFX_MSG_MAP(CXTPSyntaxEditColorSampleText)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditColorSampleText message handlers

void CXTPSyntaxEditColorSampleText::OnNcPaint()
{
	CWindowDC dc(this);

	CXTPWindowRect rc(this);
	rc.OffsetRect(-rc.TopLeft());

	dc.Draw3dRect(rc, GetBorderColor(), GetBorderColor());
	rc.DeflateRect(1,1);
	dc.Draw3dRect(rc, GetBackColor(), GetBackColor());
}

void CXTPSyntaxEditColorSampleText::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CXTPClientRect rc(this);
	dc.FillSolidRect(&rc, GetBackColor());

	CString csText;
	GetWindowText(csText);

	dc.SetBkColor(GetBackColor());
	dc.SetTextColor(GetTextColor());

	CXTPFontDC fontDC(&dc, GetFont());
	dc.DrawText(csText, &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
}

void CXTPSyntaxEditColorSampleText::Refresh()
{
	if (::IsWindow(m_hWnd))
	{
		RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME |
			RDW_INVALIDATE | RDW_ERASE);
	}
}

void CXTPSyntaxEditColorSampleText::SetTextColor(COLORREF crText)
{
	m_crText = crText;
	Refresh();
}

COLORREF CXTPSyntaxEditColorSampleText::GetTextColor() const
{
	return (IsWindowEnabled()? m_crText: ::GetSysColor(COLOR_GRAYTEXT));
}

void CXTPSyntaxEditColorSampleText::SetBackColor(COLORREF crBack)
{
	m_crBack = crBack;
	Refresh();
}

COLORREF CXTPSyntaxEditColorSampleText::GetBackColor() const
{
	return (IsWindowEnabled()? m_crBack: ::GetSysColor(COLOR_3DFACE));
}

void CXTPSyntaxEditColorSampleText::SetBorderColor(COLORREF crBorder)
{
	m_crBorder = crBorder;
	Refresh();
}

COLORREF CXTPSyntaxEditColorSampleText::GetBorderColor() const
{
	return (IsWindowEnabled()? m_crBorder: ::GetSysColor(COLOR_GRAYTEXT));
}

void CXTPSyntaxEditColorSampleText::OnEnable(BOOL bEnable)
{
	CWnd::OnEnable(bEnable);
	Refresh();
}

void CXTPSyntaxEditColorSampleText::OnSysColorChange()
{
	CStatic::OnSysColorChange();
	m_crBack.SetStandardValue(::GetSysColor(COLOR_WINDOW));
	m_crText.SetStandardValue(::GetSysColor(COLOR_WINDOWTEXT));
	m_crBorder.SetStandardValue(::GetSysColor(COLOR_3DDKSHADOW));
}
