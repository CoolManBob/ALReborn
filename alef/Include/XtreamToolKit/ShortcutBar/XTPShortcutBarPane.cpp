// XTPShortcutBarPane.cpp : implementation of the CXTPShortcutBarPane class.
//
// This file is a part of the XTREME SHORTCUTBAR MFC class library.
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

#include "XTPShortcutBarPane.h"
#include "XTPShortcutBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CXTPShortcutBarPaneItem

CXTPShortcutBarPaneItem::CXTPShortcutBarPaneItem(LPCTSTR lpszCaption, CWnd* pWnd, int nHeight)
{
	m_strCaption = lpszCaption;
	m_pWndClient = pWnd;
	m_nHeight = nHeight;
	m_bShowCaption = TRUE;
	m_pPane = NULL;
}

void CXTPShortcutBarPaneItem::SetHeight(int nHeight)
{
	m_nHeight = max(0, nHeight);
}


/////////////////////////////////////////////////////////////////////////////
// CXTPShortcutBarPane

IMPLEMENT_DYNAMIC(CXTPShortcutBarPane, CWnd)

CXTPShortcutBarPane::CXTPShortcutBarPane()
{
	m_pShortcutBar = NULL;

	m_bShowCaption = TRUE;
	m_nMinClientHeight = 0;

	m_rcIndent.SetRect(0, 5, 0, 5);
}

CXTPShortcutBarPane::~CXTPShortcutBarPane()
{
	for (int i = 0; i < m_arrItems.GetSize(); i++)
	{
		delete m_arrItems[i];
	}
}

CXTPShortcutBarPaneItem* CXTPShortcutBarPane::AddItem(LPCTSTR lpszCaption, CWnd* pWnd, int nHeight)
{
	CXTPShortcutBarPaneItem* pItem = new CXTPShortcutBarPaneItem(lpszCaption, pWnd, nHeight);
	pItem->m_pPane = this;

	m_arrItems.Add(pItem);

	return pItem;
}


BOOL CXTPShortcutBarPane::Create(LPCTSTR lpszCaption, CXTPShortcutBar* pParent)
{
	m_pShortcutBar = pParent;
	m_strCaption = lpszCaption;

	if (!CWnd::Create(AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CXTPEmptyRect(), pParent, 0))
	{
		return FALSE;
	}

	return TRUE;
}

CXTPShortcutBarPaneItem* CXTPShortcutBarPane::GetItem(int nIndex) const
{
	if (nIndex >= 0 && nIndex < m_arrItems.GetSize())
		return m_arrItems[nIndex];
	return NULL;
}

void CXTPShortcutBarPane::SetCaption(LPCTSTR lpszCaption)
{
	m_strCaption = lpszCaption;

	if (m_hWnd)
	{
		Invalidate(FALSE);
	}
}

void CXTPShortcutBarPaneItem::SetCaption(LPCTSTR lpszCaption)
{
	m_strCaption = lpszCaption;

	if (m_pPane && m_pPane->m_hWnd)
	{
		m_pPane->Invalidate(FALSE);
	}
}

BEGIN_MESSAGE_MAP(CXTPShortcutBarPane, CWnd)
	//{{AFX_MSG_MAP(CXTPShortcutBarPane)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPShortcutBarPane message handlers

void CXTPShortcutBarPane::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CXTPShortcutBarPaintManager* pPaintManager = m_pShortcutBar->GetPaintManager();
	ASSERT(pPaintManager);

	int nPaneCaptionHeight = m_bShowCaption ? pPaintManager->DrawPaneCaption(NULL, this, FALSE) : 0;
	int nItemCaptionHeight = pPaintManager->DrawPaneItemCaption(NULL, NULL, FALSE);

	int nTop = nPaneCaptionHeight;

	for (int i = 0; i < m_arrItems.GetSize(); i++)
	{
		CXTPShortcutBarPaneItem* pItem = m_arrItems[i];

		int nClientTop = nTop + (pItem->m_bShowCaption ? nItemCaptionHeight : 0);
		pItem->m_rcCaption.SetRect(0, nTop, cx, nClientTop);

		CRect rcClient(0 + m_rcIndent.left, m_rcIndent.top + nClientTop, cx - m_rcIndent.right, m_rcIndent.top + nClientTop + pItem->m_nHeight);
		if (rcClient.bottom > cy) rcClient.bottom = cy;
		if (i == m_arrItems.GetSize() - 1) rcClient.bottom = cy;

		pItem->m_rcClient = rcClient;

		nTop = nClientTop + pItem->m_nHeight + m_rcIndent.top + m_rcIndent.bottom;
		if (pItem->m_pWndClient)
		{
			pItem->m_pWndClient->MoveWindow(rcClient);
		}
	}

	Invalidate(FALSE);

}

BOOL CXTPShortcutBarPane::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTPShortcutBarPane::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting

	CXTPClientRect rc(this);
	CXTPBufferDC dc(dcPaint, rc);

	dc.FillSolidRect(rc, GetXtremeColor(COLOR_WINDOW));

	CXTPShortcutBarPaintManager* pPaintManager = m_pShortcutBar->GetPaintManager();
	ASSERT(pPaintManager);

	if (m_bShowCaption)
	{
		pPaintManager->DrawPaneCaption(&dc, this, TRUE);
	}

	for (int i = 0; i < m_arrItems.GetSize(); i++)
	{
		CXTPShortcutBarPaneItem* pItem = m_arrItems[i];

		if (pItem->m_bShowCaption)
		{
			pPaintManager->DrawPaneItemCaption(&dc, pItem, TRUE);
		}
	}
}
