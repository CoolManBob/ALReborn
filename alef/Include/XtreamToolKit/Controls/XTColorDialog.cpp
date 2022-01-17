// XTColorDialog.cpp : implementation file
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
#include "Resource.h"

#include "Common/XTPResourceManager.h"
#include "Common/XTPVC80Helpers.h"  // Visual Studio 2005 helper functions
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTDefines.h"
#include "XTColorDialog.h"
#include "XTColorPageCustom.h"
#include "XTColorPageStandard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTColorDialog

IMPLEMENT_DYNAMIC(CXTColorDialog, CPropertySheet)

CXTColorDialog::CXTColorDialog(COLORREF clrNew, COLORREF clrCurrent,
	DWORD dwFlags/*= 0L*/, CWnd* pWndParent/*= NULL*/)
{
	CString strCaption;
	XTPResourceManager()->LoadString(&strCaption, XT_IDS_COLOR_CAPTION);

	Construct(strCaption, pWndParent);

	m_psh.dwFlags = (m_psh.dwFlags & ~PSH_HASHELP) | PSH_NOAPPLYNOW;

	m_clrNew = clrNew;
	m_clrCurrent = clrCurrent;
	m_dwStyle = dwFlags;

	AddPage(new CXTColorPageStandard(this));
	AddPage(new CXTColorPageCustom(this));

}

void CXTColorDialog::AddPage(CPropertyPage* pPage)
{
	CPropertySheet::AddPage(pPage);

	LPCDLGTEMPLATE pResource = XTPResourceManager()->LoadDialogTemplate((UINT)(UINT_PTR)pPage->m_psp.pszTemplate);

	if (pResource)
	{
		pPage->m_psp.pResource = pResource;
		pPage->m_psp.dwFlags |= PSP_DLGINDIRECT;
	}
}

CXTColorDialog::~CXTColorDialog()
{
	int iPage;
	for (iPage = 0; iPage < GetPageCount(); ++iPage)
	{
		CPropertyPage* pPage = GetPage(iPage);
		SAFE_DELETE (pPage);
	}
}


BEGIN_MESSAGE_MAP(CXTColorDialog, CPropertySheet)
	//{{AFX_MSG_MAP(CXTColorDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTColorDialog message handlers

void CXTColorDialog::CalculateRects()
{
	CRect rcBtnOK;
	CRect rcBtnCancel;
	CRect rcTabCtrl;
	CRect rcItem;

	// get the tab control size.
	CTabCtrl* pTabCtrl = GetTabControl();
	pTabCtrl->GetWindowRect(&rcTabCtrl);
	ScreenToClient(&rcTabCtrl);

	// get the size of the first tab item.
	pTabCtrl->GetItemRect(0, &rcItem);

	// get the OK button size.
	CButton* pBtnOK = (CButton*)GetDlgItem(IDOK);
	pBtnOK->GetWindowRect(&rcBtnOK);
	ScreenToClient(&rcBtnOK);

	// get the Cancel button size.
	CButton* pBtnCancel = (CButton*)GetDlgItem(IDCANCEL);
	pBtnCancel->GetWindowRect(&rcBtnCancel);
	ScreenToClient(&rcBtnCancel);
	rcBtnCancel.OffsetRect(-15, 0);

	// resize the tab control
	rcTabCtrl.right = rcBtnCancel.left - 5;
	rcTabCtrl.bottom = rcBtnCancel.top - 15;
	pTabCtrl->MoveWindow(&rcTabCtrl);

	// reposition the OK button.
	rcBtnOK = rcBtnCancel;
	rcBtnOK.top = rcTabCtrl.top + rcItem.Height() + 1;
	rcBtnOK.bottom = rcBtnOK.top + rcBtnCancel.Height();
	pBtnOK->MoveWindow(&rcBtnOK);

	// reposition the Cancel button.
	rcBtnCancel = rcBtnOK;
	rcBtnCancel.top = rcBtnOK.bottom + 5;
	rcBtnCancel.bottom = rcBtnCancel.top + rcBtnOK.Height();
	pBtnCancel->MoveWindow(&rcBtnCancel);

	// reposition the hex display
	if (::IsWindow(m_editHex.m_hWnd))
	{
		CRect rcHex;
		rcHex = rcBtnCancel;
		rcHex.top = rcBtnCancel.bottom + 5;
		rcHex.bottom = rcHex.top + 18;
		m_editHex.MoveWindow(&rcHex);
	}

	// resize the property sheet.
	CXTPWindowRect rcWindow(this);
	ClientToScreen(&rcTabCtrl);
	rcWindow.bottom = rcTabCtrl.bottom + 10;
	rcWindow.right -= 15;
	MoveWindow(&rcWindow);
}

BOOL CXTColorDialog::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	CalculateRects();
	SetActivePage(0);

	if (m_editHex.GetSafeHwnd())
	{
		m_editHex.SetFont(GetFont());
	}


	return bResult;
}

void CXTColorDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(GetXtremeColor(COLOR_WINDOWTEXT));

	CXTPFontDC fontDC(&dc, GetFont());

	// get the Cancel button size.
	CXTPWindowRect rcBtnCancel(GetDlgItem(IDCANCEL));
	ScreenToClient(&rcBtnCancel);

	// construct the size for the the new / current color box.
	CXTPClientRect rect(this);
	rect.right = rcBtnCancel.right;
	rect.left = rcBtnCancel.left;
	rect.bottom -= 32;
	rect.top = rect.bottom - 66;

	// draw the borders for teh new / current color box.
	if (m_dwStyle & CPS_XT_SHOW3DSELECTION)
	{
		rect.InflateRect(3, 3);
		dc.Draw3dRect(&rect, GetXtremeColor(COLOR_3DHILIGHT),
			GetXtremeColor(COLOR_3DDKSHADOW));

		rect.DeflateRect(3, 3);
		dc.Draw3dRect(&rect, GetXtremeColor(COLOR_3DSHADOW),
			GetXtremeColor(COLOR_3DHILIGHT));
	}
	else
	{
		dc.Draw3dRect(&rect, GetXtremeColor(COLOR_WINDOWFRAME),
			GetXtremeColor(COLOR_WINDOWFRAME));
	}

	// draw the new text string.
	CRect rcText = rect;
	rcText = rect;
	rcText.top -= 22;
	rcText.bottom = rcText.top + 22;

	CString strText;
	VERIFY(XTPResourceManager()->LoadString(&strText, XT_IDS_NEW));
	dc.DrawText(strText, rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// draw the current text string.
	rcText = rect;
	rcText.top = rcText.bottom;
	rcText.bottom = rcText.top + 22;

	VERIFY(XTPResourceManager()->LoadString(&strText, XT_IDS_CURRENT));
	dc.DrawText(strText, rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect.DeflateRect(1, 1);
	int cy = rect.Height()/2;

	// fill the new color box.
	m_rcNew = rect;
	m_rcNew.bottom = rect.top + cy;
	dc.FillSolidRect(&m_rcNew, m_clrNew);

	// fill the current color box.
	m_rcCurrent = rect;
	m_rcCurrent.top = rect.bottom - cy;
	dc.FillSolidRect(&m_rcCurrent, m_clrCurrent);
}

void CXTColorDialog::SetNewColor(COLORREF clr, BOOL bNotify/*= TRUE*/)
{
	m_clrNew = clr;

	if (IsWindowVisible())
	{
		CClientDC dc(this);
		dc.FillSolidRect(&m_rcNew, m_clrNew);

		if (m_dwStyle & CPS_XT_SHOWHEXVALUE)
		{
			m_editHex.SetWindowText(RGBtoHex(m_clrNew));
		}
	}

	if (bNotify && GetTabControl() && GetTabControl()->GetSafeHwnd())
	{
		int iPage;
		for (iPage = 0; iPage < GetPageCount(); ++iPage)
		{
			GetPage(iPage)->SendMessage(XTWM_UPDATECOLOR,
				(WPARAM)(COLORREF)m_clrNew);
		}
	}
}

void CXTColorDialog::SetCurrentColor(COLORREF clr)
{
	m_clrCurrent = clr;

	if (IsWindowVisible())
	{
		CClientDC dc(this);
		dc.FillSolidRect(&m_rcCurrent, m_clrCurrent);
	}
}

BOOL CXTColorDialog::CopyToClipboard(const CString& strText)
{
	if (::OpenClipboard(m_hWnd))
	{
		::EmptyClipboard();

		HGLOBAL hGlobalBuff = ::GlobalAlloc(GMEM_MOVEABLE, strText.GetLength() + 1);

		CHAR* szBuffer = (CHAR*)::GlobalLock(hGlobalBuff);

		WCSTOMBS_S(szBuffer, strText, strText.GetLength() + 1);
		::GlobalUnlock(hGlobalBuff);

		if (::SetClipboardData(CF_TEXT, hGlobalBuff) == NULL)
			return FALSE;

		::CloseClipboard();
		return TRUE;
	}

	return FALSE;
}

CString CXTColorDialog::RGBtoHex(COLORREF clr)
{
	int r = GetRValue(clr);
	int g = GetGValue(clr);
	int b = GetBValue(clr);

	CString strHex;

	if (r < 16 && g < 16 && b < 16)
		strHex.Format(_T("Hex= 0%X0%X0%X"), r, g, b);
	else if (r < 16 && g < 16)
		strHex.Format(_T("Hex= 0%X0%X%X"), r, g, b);
	else if (r < 16 && b < 16)
		strHex.Format(_T("Hex= 0%X%X0%X"), r, g, b);
	else if (g < 16 && b < 16)
		strHex.Format(_T("Hex=%X0%X0%X"), r, g, b);
	else if (r < 16)
		strHex.Format(_T("Hex= 0%X%X%X"), r, g, b);
	else if (g < 16)
		strHex.Format(_T("Hex=%X0%X%X"), r, g, b);
	else if (b < 16)
		strHex.Format(_T("Hex=%X%X0%X"), r, g, b);
	else
		strHex.Format(_T("Hex=%X%X%X"), r, g, b);

	return strHex;
}

int CXTColorDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_dwStyle & CPS_XT_SHOWHEXVALUE)
	{
		if (!m_editHex.CreateEx(WS_EX_STATICEDGE, _T("EDIT"), RGBtoHex(m_clrNew),
			WS_CHILD | WS_VISIBLE | ES_READONLY, CRect(0, 0, 0, 0), this, AFX_IDC_CHANGE))
		{
			TRACE0("Failed to create edit control.\n");
			return -1;
		}
	}

	((CXTColorPageStandard*)GetPage(0))->SetColor(m_clrNew);

	return 0;
}
