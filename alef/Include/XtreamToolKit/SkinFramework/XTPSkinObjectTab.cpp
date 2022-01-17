// XTPSkinObjectTab.cpp: implementation of the CXTPSkinObjectTab class.
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

#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPSkinObjectTab.h"
#include "XTPSkinManager.h"
#include "XTPSkinImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define DT_HIDEPREFIX       0x00100000

AFX_INLINE BOOL IsLeft(DWORD dwStyle)
{
	return ((dwStyle & (TCS_VERTICAL|TCS_RIGHT|TCS_BOTTOM)) == TCS_VERTICAL);
}
AFX_INLINE BOOL IsRight(DWORD dwStyle)
{
	return ((dwStyle & (TCS_VERTICAL|TCS_RIGHT|TCS_BOTTOM)) == (TCS_VERTICAL|TCS_RIGHT));
}
AFX_INLINE BOOL IsTop(DWORD dwStyle)
{
	return ((dwStyle & (TCS_VERTICAL|TCS_RIGHT|TCS_BOTTOM)) == 0);
}
AFX_INLINE BOOL IsBottom(DWORD dwStyle)
{
	return ((dwStyle & (TCS_VERTICAL|TCS_RIGHT|TCS_BOTTOM)) == TCS_BOTTOM);
}
AFX_INLINE BOOL IsVert(DWORD dwStyle)
{
	return (IsRight(dwStyle) || IsLeft(dwStyle));
}
AFX_INLINE BOOL IsHorz(DWORD dwStyle)
{
	return (IsTop(dwStyle) || IsBottom(dwStyle));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectTab, CXTPSkinObjectFrame)

CXTPSkinObjectTab::CXTPSkinObjectTab()
{
	m_strClassName = _T("TAB");
	m_nHotItem = -1;
}

CXTPSkinObjectTab::~CXTPSkinObjectTab()
{

}


BEGIN_MESSAGE_MAP(CXTPSkinObjectTab, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectTab)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectTab message handlers


BOOL CXTPSkinObjectTab::OnEraseBkgnd(CDC* pDC)
{
	CXTPClientRect rc(this);
	CXTPBufferDC dcMem(pDC->GetSafeHdc(), rc);
	FillClient(&dcMem, rc);

	return TRUE;
}

CRect CXTPSkinObjectTab::GetHeaderRect()
{
	CRect rcHeader;
	GetClientRect(&rcHeader);

	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	// get the selected tab item rect.
	CRect rcItem;
	pTabCtrl->GetItemRect(pTabCtrl->GetCurSel(), &rcItem);

	DWORD dwStyle = GetStyle();

	if (IsRight(dwStyle))
	{
		rcHeader.left = rcItem.left;
	}
	else if (IsLeft(dwStyle))
	{
		rcHeader.right = rcItem.right;
	}
	else if (IsBottom(dwStyle))
	{
		rcHeader.top = rcItem.top;
	}
	else
	{
		rcHeader.bottom = (rcHeader.top + rcItem.Height()) * pTabCtrl->GetRowCount() + GetMetrics()->m_cyEdge;
	}
	return rcHeader;
}

void CXTPSkinObjectTab::FillClient(CDC* pDC, CRect rc)
{
	FillBackground(pDC, rc);

	if (GetStyle() & TCS_BUTTONS)
		return;

	CXTPSkinManagerClass* pClass = GetSkinClass();

	CRect rcClient(rc);
	CRect rcHeader = GetHeaderRect();

	DWORD dwStyle = GetStyle();

	if (IsRight(dwStyle))
	{
		rcClient.right = rcHeader.left;
	}
	else if (IsLeft(dwStyle))
	{
		rcClient.left = rcHeader.right;
	}
	else if (IsBottom(dwStyle))
	{
		rcClient.bottom = rcHeader.top;
	}
	else
	{
		rcClient.top = rcHeader.bottom;
	}

	pClass->DrawThemeBackground(pDC, TABP_PANE, 0, &rcClient);
}


void CXTPSkinObjectTab::FillTabFace(CDC* pDC, CRect rcItem, int iItem, int iCount, BOOL bSelected)
{
	if (GetStyle() & TCS_BUTTONS)
	{
		CXTPSkinManagerClass* pClassButton = GetSkinManager()->GetSkinClass(_T("BUTTON"));
		int nState = bSelected? PBS_PRESSED: PBS_NORMAL;
		pClassButton->DrawThemeBackground(pDC, BP_PUSHBUTTON, nState, rcItem);
		return;
	}

	if (bSelected)
	{
		rcItem.InflateRect(2, 2, 2, 2);
	}

	CXTPSkinManagerClass* pClass = GetSkinClass();

	int nStateId = bSelected ? TIS_SELECTED: m_nHotItem == iItem ? TIS_HOT : TIS_NORMAL;

	pClass->DrawThemeBackground(pDC, iItem == 0 ? TABP_TOPTABITEMLEFTEDGE :
		iItem == iCount - 1 && !bSelected ? TABP_TOPTABITEMRIGHTEDGE : TABP_TOPTABITEM,
		nStateId, &rcItem);
}


void CXTPSkinObjectTab::DrawTabIcon(CDC* pDC, CRect& rcItem, int iItem)
{
	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	TC_ITEM tci;
	tci.mask = TCIF_IMAGE;

	if (!pTabCtrl->GetItem(iItem, &tci))
		return;

	CImageList* pImageList = pTabCtrl->GetImageList();
	if (!pImageList || tci.iImage < 0)
		return;

	CPoint point = rcItem.TopLeft();

	DWORD dwStyle = GetStyle();

	if (IsHorz(dwStyle))
	{
		point.Offset(GetMetrics()->m_cxEdge * 2, IsTop(dwStyle) ? GetMetrics()->m_cyEdge : 1);
	}
	else
	{
		point.Offset(IsLeft(dwStyle) ? GetMetrics()->m_cxEdge : 1, GetMetrics()->m_cyEdge * 2);
	}

	// Draw any associated icons.
	pImageList->Draw(pDC, tci.iImage, point, ILD_TRANSPARENT);

	IMAGEINFO info;
	pImageList->GetImageInfo(tci.iImage, &info);
	CRect rcImage(info.rcImage);

	if (IsHorz(dwStyle))
	{
		rcItem.left += rcImage.Width() + GetMetrics()->m_cxEdge;
	}
	else
	{
		rcItem.top += rcImage.Height() + GetMetrics()->m_cyEdge;
	}
}

void CXTPSkinObjectTab::DrawTabText(CDC* pDC, CRect& rcItem, int iItem, int iCount,  BOOL bSelected)
{
	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	CString strCaption;
	LPTSTR pszText = strCaption.GetBuffer(256);
	pszText[0] = 0;

	TC_ITEM tci;
	tci.mask = TCIF_TEXT;
	tci.pszText = pszText;
	tci.cchTextMax = 255;

	BOOL bResult = pTabCtrl->GetItem(iItem, &tci);
	strCaption.ReleaseBuffer();

#ifdef _UNICODE
	if (!bResult)
	{
		char tText[256];
		tText[0] = 0;

		TC_ITEMA tcia;
		::ZeroMemory(&tcia, sizeof(TC_ITEMA));

		tcia.mask = TCIF_TEXT;

		// Get the header item text and format
		tcia.pszText = tText;
		tcia.cchTextMax = 255;

		::SendMessage(pTabCtrl->m_hWnd, TCM_GETITEMA, iItem, (LPARAM)&tcia);

		strCaption = tText;
	}
#else
	if (!bResult)
		return;
#endif

	if (strCaption.GetLength() == 0)
		return;

	pDC->SetTextColor(GetColor(COLOR_BTNTEXT));

	// Set the font for the tab label.
	DWORD dwStyle = GetStyle();

	CXTPSkinManagerClass* pClass = GetSkinClass();

	int nStateId = bSelected ? TIS_SELECTED: m_nHotItem == iItem ? TIS_HOT : TIS_NORMAL;
	int nPartId = iItem == 0 ? TABP_TOPTABITEMLEFTEDGE :
		iItem == iCount - 1 && !bSelected ? TABP_TOPTABITEMRIGHTEDGE : TABP_TOPTABITEM;


	// Draw the tab label.
	if (IsHorz(dwStyle))
	{
		UINT dsFlags = DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS;
		if (SendMessage(WM_QUERYUISTATE) & UISF_HIDEACCEL)
		{
			dsFlags |= DT_HIDEPREFIX;
		}
		if (!(GetStyle() & TCS_BUTTONS))
		{
			rcItem.top += bSelected ? -2 : 1;
			rcItem.right -= iItem == 0 ? 3 : 0;
		}
		else
		{
			rcItem.top += bSelected ? 0 : -1;
		}
		pClass->DrawThemeText(pDC, nPartId, nStateId, strCaption, dsFlags, &rcItem);
	}
	else
	{
		CSize sz = pDC->GetTextExtent(strCaption);
		rcItem.left = rcItem.right-(rcItem.Width()-sz.cy + 1)/ 2;
		rcItem.top = rcItem.top+(rcItem.Height()-sz.cx + 1)/ 2;

		pClass->DrawThemeText(pDC, nPartId, nStateId, strCaption, DT_SINGLELINE | DT_NOCLIP, &rcItem);
	}
}



void CXTPSkinObjectTab::DrawTab(CDC* pDC, int iItem)
{
	CXTPClientRect rc(this);
	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	// Get the current tab selection.
	int iCurSel = pTabCtrl->GetCurSel();

	// Get the tab item size.
	CRect rcItem;
	pTabCtrl->GetItemRect(iItem, &rcItem);

	if (!CRect().IntersectRect(rcItem, rc))
		return;

	CRect rcItemFocus(rcItem);

	// Draw the tab item.
	BOOL bSelected = (iItem == iCurSel);
	FillTabFace(pDC, rcItem, iItem, pTabCtrl->GetItemCount(), bSelected);
	DrawTabIcon(pDC, rcItem, iItem);
	DrawTabText(pDC, rcItem, iItem, iItem, bSelected);

	// draw the focus rect
	if (bSelected && ::GetFocus() == m_hWnd && (SendMessage(WM_QUERYUISTATE) & UISF_HIDEFOCUS) == 0)
	{
		rcItemFocus.DeflateRect(GetMetrics()->m_cxEdge / 2, GetMetrics()->m_cyEdge / 2);
		pDC->DrawFocusRect(&rcItemFocus);
	}
}


void CXTPSkinObjectTab::OnPaint()
{
	CXTPSkinObjectPaintDC dc(this);

	if (dc.m_ps.hdc == 0)
	{
		FillClient(&dc, CXTPClientRect(this));
	}

	OnDraw(&dc);
}

void CXTPSkinObjectTab::OnDraw(CDC* pDC)
{
	CRect rc = GetHeaderRect();
	DWORD dwStyle = GetStyle();

	if (IsRight(dwStyle))
	{
		rc.left -= 2;
	}
	else if (IsLeft(dwStyle))
	{
		rc.right += 2;
	}
	else if (IsBottom(dwStyle))
	{
		rc.top -= 2;
	}
	else
	{
		rc.bottom += 2;
	}

	CXTPBufferDC dcMem(*pDC, rc);
	FillClient(&dcMem, CXTPClientRect(this));

	if (m_bActiveX)
	{
		CXTPBufferDC dcMem2(*pDC, rc);
		DefWindowProc(WM_PAINT, (WPARAM)dcMem2.GetSafeHdc(), 0);
		dcMem2.Discard();
	}

	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	CXTPFontDC font(&dcMem, GetFont());
	dcMem.SetBkMode(TRANSPARENT);

	for (int iItem = 0; iItem < pTabCtrl->GetItemCount(); ++iItem)
	{
		DrawTab(&dcMem, iItem);
	}

	DrawTab(&dcMem, pTabCtrl->GetCurSel());
}

void CXTPSkinObjectTab::OnMouseLeave()
{
	OnMouseMove(0,CPoint(-1, -1));
}

void CXTPSkinObjectTab::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	int nHotItem = -1;
	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	for (int iItem = 0; iItem < pTabCtrl->GetItemCount(); ++iItem)
	{
		CRect rcItem;
		pTabCtrl->GetItemRect(iItem, &rcItem);
		if (rcItem.PtInRect(point))
		{
			nHotItem = iItem;
			break;
		}
	}

	if (nHotItem != m_nHotItem)
	{
		m_nHotItem = nHotItem;
		Invalidate(FALSE);

		if (m_nHotItem != -1)
		{
			TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT};
			_TrackMouseEvent(&tme);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectTabClient

////////////////////////////////////////////////////////////////////

HBRUSH CXTPSkinObjectFrame::GetClientBrush(HDC hDC, HWND hWnd, UINT nCtlColor)
{
	if (m_dwDialogTexture != ETDT_ENABLETAB)
	{
		GrayCtlColor(hDC, hWnd, nCtlColor, GetMetrics()->m_brushDialog, GetColor(COLOR_BTNTEXT));
		return GetMetrics()->m_brushDialog;
	}

	if (GetMetrics()->m_brushTabControl == NULL)
	{
		CWindowDC dcWindow(this);

		CXTPSkinManagerClass* pClass = GetSkinManager()->GetSkinClass(_T("TAB"));

		CString strImageFile = pClass->GetThemeString(TABP_BODY, 0, TMT_STOCKIMAGEFILE);
		if (strImageFile.IsEmpty())
		{
			ASSERT(FALSE);
			return GetMetrics()->m_brushDialog;
		}

		CXTPSkinImage* pImage = pClass->GetImages()->LoadFile(m_pManager->GetResourceFile(), strImageFile);

		if (!pImage)
		{
			ASSERT(FALSE);
			return GetMetrics()->m_brushDialog;
		}

		CRect rc(0, 0, pImage->GetWidth(), pImage->GetHeight());

		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dcWindow, rc.Width(), rc.Height());

		CXTPCompatibleDC dc(&dcWindow, bmp);

		pImage->DrawImage(&dc, rc, rc, CRect(0, 0, 0, 0), COLORREF_NULL, ST_TRUESIZE, FALSE);

		GetMetrics()->m_brushTabControl = ::CreatePatternBrush(bmp);
	}


	if (hWnd != m_hWnd)
	{
		CXTPWindowRect rcPaint(hWnd);
		CXTPWindowRect rcBrush(m_hWnd);
		::SetBrushOrgEx(hDC, rcBrush.left - rcPaint.left, rcBrush.top - rcPaint.top, NULL);

		::SetBkMode(hDC, TRANSPARENT);
		::SetTextColor(hDC, GetColor(COLOR_BTNTEXT));
	}

	return GetMetrics()->m_brushTabControl;

}
