// XTPSkinObjectHeader.cpp: implementation of the CXTPSkinObjectHeader class.
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

#include "Common/XTPDrawHelpers.h"

#include "XTPSkinObjectHeader.h"
#include "XTPSkinManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CXTPSkinObjectHeader, CXTPSkinObjectFrame)

CXTPSkinObjectHeader::CXTPSkinObjectHeader()
{
	m_strClassName = _T("HEADER");
	m_bLBtnDown = FALSE;
	m_nHotItem = -1;
}

CXTPSkinObjectHeader::~CXTPSkinObjectHeader()
{

}


BEGIN_MESSAGE_MAP(CXTPSkinObjectHeader, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectHeader)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectHeader message handlers

void CXTPSkinObjectHeader::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags = 0;

	if (HitTest(point, &uFlags) != -1 && ((uFlags & HHT_ONHEADER) == HHT_ONHEADER))
	{
		m_bLBtnDown = TRUE;
	}

	CXTPSkinObjectFrame::OnLButtonDown(nFlags, point);
}

void CXTPSkinObjectHeader::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bLBtnDown)
	{
		m_bLBtnDown = FALSE;
		Invalidate(FALSE);
	}
	CXTPSkinObjectFrame::OnLButtonUp(nFlags, point);
}

int CXTPSkinObjectHeader::HitTest(CPoint pt, UINT* pFlags /*=NULL*/) const
{
	HDHITTESTINFO hti;
	hti.pt.x = pt.x;
	hti.pt.y = pt.y;

	int iItem = (int)::SendMessage(m_hWnd, HDM_HITTEST, 0, (LPARAM)(&hti));

	if (pFlags != NULL)
		*pFlags = hti.flags;

	return iItem;
}

void CXTPSkinObjectHeader::OnMouseMove(UINT nFlags, CPoint point)
{
	int nHotItem = HitTest(point);

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

	CXTPSkinObjectFrame::OnMouseMove(nFlags, point);
}

void CXTPSkinObjectHeader::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

#ifndef HDS_FILTERBAR
#define HDS_FILTERBAR 0x0100
#endif

LRESULT CXTPSkinObjectHeader::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	if ((lParam & PRF_CLIENT) == 0)
		return Default();

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC) OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectHeader::OnPaint()
{
	CXTPSkinObjectPaintDC dc(this);
	OnDraw(&dc);
}

void CXTPSkinObjectHeader::OnDraw(CDC* pDC)
{
	CXTPClientRect rcClient(this);
	CRect rcBackground(rcClient);

	CXTPBufferDC dc(*pDC, rcClient);

	BOOL bFilterBar = (GetStyle() & HDS_FILTERBAR) && XTPSystemVersion()->GetComCtlVersion() >= MAKELONG(80, 5);
	CRect rcFilter(0, 0, 0, 0);

	dc.FillSolidRect(rcClient, GetColor(COLOR_3DFACE));

	if (bFilterBar)
	{
		INT cyFilter = (rcClient.Height() - 1)/2;

		rcFilter = rcClient;

		rcClient.bottom = rcClient.top + cyFilter;
		rcFilter.top = rcClient.bottom;

		DefWindowProc(WM_PAINT, (WPARAM)dc.GetSafeHdc(), 0);
	}

	CXTPSkinManagerClass* pClass = GetSkinClass();

	CXTPFontDC fontDC(&dc, GetFont());
	dc.SetTextColor(GetColor(COLOR_BTNTEXT));
	dc.SetBkMode(TRANSPARENT);

	CHeaderCtrl* pHeaderCtrl = (CHeaderCtrl*)this;

	int iItemCount = pHeaderCtrl->GetItemCount();



	// Draw each header item
	for (int iItem = 0; iItem < iItemCount; ++iItem)
	{
		int nIndex = Header_OrderToIndex(m_hWnd, iItem);

		// initialize draw item structure.
		CRect rcItem(0, 0, 0, 0);
		Header_GetItemRect(m_hWnd, nIndex, &rcItem);

		if ((rcItem.right < rcClient.left) || (rcItem.left > rcClient.right))
			continue;

		if (bFilterBar)
		{
			rcItem.bottom = rcFilter.top;
		}

		int nState = HIS_NORMAL;

		if (nIndex == m_nHotItem)
		{
			nState = m_bLBtnDown ? HIS_PRESSED : HIS_HOT;
		}

		rcBackground.left = max(rcBackground.left, rcItem.right);


		pClass->DrawThemeBackground(&dc, HP_HEADERITEM, nState, &rcItem);

		DrawItemEntry(&dc, nIndex, rcItem, nState);
	}

	if (rcBackground.left < rcBackground.right)
		pClass->DrawThemeBackground(&dc, 0, 0, &rcBackground);

}

#ifndef HDM_GETBITMAPMARGIN
#define HDM_GETBITMAPMARGIN          (HDM_FIRST + 21)
#endif

#ifndef HDF_SORTUP
#define HDF_SORTUP              0x0400
#define HDF_SORTDOWN            0x0200
#endif

void CXTPSkinObjectHeader::DrawItemEntry(CDC* pDC, int nIndex, CRect rcItem, int nState)
{
	CHeaderCtrl* pHeaderCtrl = (CHeaderCtrl*)this;
	CImageList* pImageList = CImageList::FromHandle((HIMAGELIST)
		::SendMessage(pHeaderCtrl->m_hWnd, HDM_GETIMAGELIST, 0, 0L));

	// Set up the header item order array.
	HD_ITEM hdi;
	::ZeroMemory(&hdi, sizeof(HD_ITEM));

	hdi.fmt = HDF_STRING | HDF_IMAGE;
	hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_IMAGE | HDI_LPARAM;

	// Get the header item text and format
	CString strCaption;
	LPTSTR pszText = strCaption.GetBuffer(256);
	pszText[0] = 0;
	hdi.pszText = pszText;
	hdi.cchTextMax = 255;

	BOOL bResult = pHeaderCtrl->GetItem(nIndex, &hdi);

	strCaption.ReleaseBuffer();

#ifdef _UNICODE
	if (!bResult)
	{
		char tText[256];
		tText[0] = 0;

		HD_ITEMA hdia;
		::ZeroMemory(&hdia, sizeof(HD_ITEMA));

		hdia.fmt = HDF_STRING | HDF_IMAGE;
		hdia.mask = HDI_TEXT | HDI_FORMAT | HDI_IMAGE | HDI_LPARAM;

		// Get the header item text and format
		hdia.pszText = tText;
		hdia.cchTextMax = 255;

		::SendMessage(pHeaderCtrl->m_hWnd, HDM_GETITEMA, nIndex, (LPARAM)&hdia);

		strCaption = tText;
		hdi.fmt = hdia.fmt;
		hdi.iImage = hdia.iImage;
		hdi.lParam = hdia.lParam;
	}
#else
	bResult;
#endif

	if (hdi.fmt & HDF_OWNERDRAW)
	{
		DRAWITEMSTRUCT dis;

		dis.CtlType = ODT_HEADER;
		dis.CtlID = (UINT)GetDlgCtrlID();
		dis.itemID = nIndex;
		dis.itemAction = ODA_DRAWENTIRE;
		dis.itemState = (nState == HIS_PRESSED) ? ODS_SELECTED : 0;
		dis.hwndItem = m_hWnd;
		dis.hDC = pDC->GetSafeHdc();
		dis.rcItem = rcItem;
		dis.itemData = hdi.lParam;

		// Now send it off to my parent...
		if (GetParent()->SendMessage(WM_DRAWITEM, dis.CtlID,
		   (LPARAM)(DRAWITEMSTRUCT*)&dis))
		{
			return;
		}
	}

	CRect rcText(rcItem);

	if (pImageList && (hdi.fmt & HDF_IMAGE) && hdi.iImage >= 0 && hdi.iImage < pImageList->GetImageCount())
	{
		int iBitmapMargin = (int)SendMessage(HDM_GETBITMAPMARGIN);
		if (iBitmapMargin == 0)
			iBitmapMargin = GetMetrics()->m_cxEdge * 3;
		int cxBitmap = 16, cyBitmap = 16;
		ImageList_GetIconSize(pImageList->GetSafeHandle(), &cxBitmap, &cyBitmap);

		CPoint pt(rcItem.left + iBitmapMargin, (rcItem.bottom + rcItem.top - cyBitmap) / 2);

		if (hdi.fmt & HDF_BITMAP_ON_RIGHT)
		{
			CSize sz = pDC->GetTextExtent(strCaption);

			pt.x += sz.cx + iBitmapMargin + 9;
			if (pt.x + cxBitmap > rcItem.right - 3)
				pt.x = max(rcItem.left + 6, rcItem.right - 3 - cxBitmap);
			if (nState == HIS_PRESSED)
				pt.x ++;

			pImageList->Draw(pDC, hdi.iImage, pt, ILD_TRANSPARENT);
			rcText.right = pt.x + 6;
		}
		else
		{
			if (nState == HIS_PRESSED)
				pt.x ++;
			pImageList->Draw(pDC, hdi.iImage, pt, ILD_TRANSPARENT);

			rcText.left += cxBitmap + iBitmapMargin;
		}
	}

	if (((hdi.fmt & HDF_IMAGE) == 0) && ((hdi.fmt & HDF_SORTUP) || (hdi.fmt & HDF_SORTDOWN)))
	{
		int iBitmapMargin = GetMetrics()->m_cxEdge * 3;
		CSize sz = pDC->GetTextExtent(strCaption);

		CPoint pt(rcItem.left + iBitmapMargin, (rcItem.bottom + rcItem.top - 2) / 2);

		pt.x += sz.cx + iBitmapMargin + 9;
		if (pt.x + 9 > rcItem.right - 3)
			pt.x = max(rcItem.left + 6, rcItem.right - 3 - 9);

		if (hdi.fmt & HDF_SORTUP)
		{
			XTPDrawHelpers()->Triangle(pDC, CPoint(pt.x - 4, pt.y + 2),
				CPoint(pt.x, pt.y - 2), CPoint(pt.x + 4, pt.y + 2), GetColor(COLOR_3DSHADOW));
		}
		else
		{
			XTPDrawHelpers()->Triangle(pDC, CPoint(pt.x - 4, pt.y - 2),
				CPoint(pt.x, pt.y + 2), CPoint(pt.x + 4, pt.y - 2), GetColor(COLOR_3DSHADOW));
		}
		rcText.right = pt.x;
	}


	UINT nFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX;

	// determine justification for text.
	switch (hdi.fmt & HDF_JUSTIFYMASK)
	{
	case HDF_LEFT:
		nFormat |= DT_LEFT;
		rcText.DeflateRect(9, 0, 6, 0);
		break;

	case HDF_CENTER:
		nFormat |= DT_CENTER;
		rcText.DeflateRect(6, 0, 6, 0);
		break;

	case HDF_RIGHT:
		nFormat |= DT_RIGHT;
		rcText.DeflateRect(6, 0, 9, 0);
		break;
	}

	if (rcText.Width() > 0)
	{
		if (nState == HIS_PRESSED)
			rcText.OffsetRect(1, 1);

		// draw text.
		pDC->DrawText(strCaption, &rcText, nFormat);
	}
}
