// XTPDockingPaneKeyboardHook.cpp : implementation of the CXTPDockingPaneKeyboardHook class.
//
// This file is a part of the XTREME DOCKINGPANE MFC class library.
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

#include "Common/XTPResourceManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/Resource.h"
#include "Resource.h"

#include "TabManager/XTPTabPaintManager.h"
#include "TabManager/XTPTabManager.h"

#include "XTPDockingPaneKeyboardHook.h"
#include "XTPDockingPane.h"
#include "XTPDockingPanePaintManager.h"
#include "XTPDockingPaneManager.h"
#include "XTPDockingPaneMiniWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneWindowSelect
IMPLEMENT_DYNCREATE(CXTPDockingPaneWindowSelect, CMiniFrameWnd)

CXTPDockingPaneWindowSelect::CXTPDockingPaneWindowSelect()
{
	LOGFONT lfIcon;
	VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0));

	lfIcon.lfWeight = FW_NORMAL;
	m_fnt.CreateFontIndirect(&lfIcon);

	lfIcon.lfWeight = FW_BOLD;
	m_fntBold.CreateFontIndirect(&lfIcon);

	XTPResourceManager()->LoadString(&m_strActiveTools, XTP_IDS_DOCKINGPANE_SELECTWINDOW_ACTIVETOOLS);
	XTPResourceManager()->LoadString(&m_strActiveFiles, XTP_IDS_DOCKINGPANE_SELECTWINDOW_ACTIVEFILES);

	m_pSelected = NULL;
	m_pManager = NULL;
	m_bActivatePanes = FALSE;

	m_nPaneCount = 0;
	m_nFirstFile = 0;

	m_hHandCursor = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649));

	if (m_hHandCursor == 0)
		m_hHandCursor = XTPResourceManager()->LoadCursor(XTP_IDC_HAND);

	m_hArrowCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

CXTPDockingPaneWindowSelect::~CXTPDockingPaneWindowSelect()
{
	for (int i = 0; i < (int)m_arrItems.GetSize(); i++)
	{
		delete m_arrItems[i];
	}
	m_arrItems.RemoveAll();
}

int CXTPDockingPaneWindowSelect::CalcItemHeight(CDC* pDC)
{
	CXTPFontDC font(pDC, &m_fnt);
	CSize szFont = pDC->GetTextExtent(_T(" "), 1);

	CSize szIcon = m_pManager->GetPaintManager()->GetTabPaintManager()->m_szIcon;

	return max(szIcon.cy + 4, szFont.cy + 4);
}

CXTPDockingPaneWindowSelect::CItem* CXTPDockingPaneWindowSelect::HitTest(CPoint point) const
{
	for (int i = 0; i < (int)m_arrItems.GetSize(); i++)
	{
		CItem* pItem = m_arrItems[i];
		if (pItem->rc.PtInRect(point))
		{
			return pItem;
		}
	}
	return 0;
}

CString CXTPDockingPaneWindowSelect::GetItemCaption(CItem* pItem) const
{
	if (pItem->type == itemPane)
	{
		return pItem->pPane->GetTitle();
	}
	else
	{
		CString strCaption;
		CFrameWnd* pChild = DYNAMIC_DOWNCAST(CFrameWnd, CWnd::FromHandle(pItem->hWndFrame));
		if (pChild && pChild->GetActiveDocument())
		{
			strCaption = pChild->GetActiveDocument()->GetTitle();
		}
		else
		{
			CWnd::FromHandle(pItem->hWndFrame)->GetWindowText(strCaption);
		}
		return strCaption;
	}
}


CString CXTPDockingPaneWindowSelect::GetItemDescription(CItem* pItem) const
{
	if (pItem->type == itemPane)
		return _T("");

	CFrameWnd* pChild = DYNAMIC_DOWNCAST(CFrameWnd, CWnd::FromHandle(pItem->hWndFrame));

	if (!pChild)
		return _T("");

	CDocument* pDocument = pChild->GetActiveDocument();
	if (!pDocument)
		return _T("");

	CString strTypeName;
	if (pDocument->GetDocTemplate() != NULL) pDocument->GetDocTemplate()->GetDocString(strTypeName, CDocTemplate::regFileTypeName);

	return strTypeName;
}

CString CXTPDockingPaneWindowSelect::GetItemPath(CItem* pItem) const
{
	if (pItem->type == itemPane)
		return _T("");

	CFrameWnd* pChild = DYNAMIC_DOWNCAST(CFrameWnd, CWnd::FromHandle(pItem->hWndFrame));

	if (!pChild)
		return _T("");

	CDocument* pDocument = pChild->GetActiveDocument();
	if (!pDocument)
		return  _T("");

	return pDocument->GetPathName();
}


HWND CXTPDockingPaneWindowSelect::GetMDIClient() const
{
	CMDIFrameWnd* pFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, m_pManager->GetSite());
	HWND hWndClient = pFrame ? pFrame->m_hWndMDIClient : NULL;
	return hWndClient;
}

BOOL CXTPDockingPaneWindowSelect::Reposition()
{
	CClientDC dc(this);
	int nItemHeight = CalcItemHeight(&dc);
	int nItemWidth = 163;

	int y = nItemHeight + 13;
	int x = 9;
	int nRow = 0;
	int nColumn = 0;

	m_pSelected = 0;

	CXTPDockingPaneInfoList& paneList = m_pManager->GetPaneList();
	POSITION pos = paneList.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = paneList.GetNext(pos);
		if ((pPane->GetEnabled() & xtpPaneEnableClient) == 0)
			continue;

		CItem* pItem = new CItem;
		pItem->pPane = pPane;
		pItem->type = itemPane;

		pItem->rc = CRect(x, y, x + nItemWidth, y + nItemHeight);
		y += nItemHeight + 1;

		pItem->nIndex = (int)m_arrItems.Add(pItem);
		pItem->nColumn = nColumn;
		pItem->nRow = nRow;

		if (m_pManager->GetActivePane() == pPane)
			m_pSelected = pItem;

		nRow++;
	}
	m_nPaneCount = nRow;

	if (m_bActivatePanes && m_nPaneCount > 0)
	{
		if (m_pSelected == 0)
		{
			m_pSelected = m_arrItems[GetKeyState(VK_SHIFT) >= 0 ? 0 : nRow - 1];
		}
		else
		{
			if (GetKeyState(VK_SHIFT) >= 0)
			{
				m_pSelected = m_arrItems[m_pSelected->nIndex >= nRow - 1 ? 0 : m_pSelected->nIndex + 1];
			}
			else
			{
				m_pSelected = m_arrItems[m_pSelected->nIndex > 0 ? m_pSelected->nIndex - 1 : nRow - 1];
			}
		}
	}


	int nTotalRow = nRow;

	if (m_nPaneCount > 0)
	{
		CColumn col;
		col.nFirst = 0;
		col.nLast = nRow - 1;
		m_arrColumns.Add(col);
	}
	else
	{
		m_bActivatePanes = FALSE;
	}

	m_nFirstFile = (int)m_arrItems.GetSize();
	int nFirstColumn = m_nFirstFile;

	nRow = 0;
	if (m_nPaneCount > 0)
	{
		nColumn++;
		x += nItemWidth + 12;
		y = nItemHeight + 13;
	}

	HWND hWndClient = GetMDIClient();
	HWND hWndActive = hWndClient ? (HWND)::SendMessage(hWndClient, WM_MDIGETACTIVE, 0, 0) : 0;
	if (hWndActive)
	{
		HWND hWndFrame = hWndActive;
		while (hWndFrame)
		{
			DWORD dwStyle = GetWindowLong(hWndFrame, GWL_STYLE);
			if (((dwStyle & WS_VISIBLE) != 0) && ((dwStyle & WS_DISABLED) == 0))
			{
				if (nRow > 14)
				{
					CColumn colNext;
					colNext.nFirst = nFirstColumn;
					colNext.nLast = (int)m_arrItems.GetSize() - 1;
					m_arrColumns.Add(colNext);

					nTotalRow = max(nRow, nTotalRow);
					nFirstColumn = (int)m_arrItems.GetSize();
					nRow = 0;
					nColumn++;
					y = nItemHeight + 13;
					x += nItemWidth + 12;
				}
				CItem* pItem = new CItem;
				pItem->hWndFrame = hWndFrame;
				pItem->type = itemMDIFrame;

				pItem->rc = CRect(x, y, x + nItemWidth, y + nItemHeight);
				y += nItemHeight + 1;

				pItem->nIndex = (int)m_arrItems.Add(pItem);
				pItem->nColumn = nColumn;
				pItem->nRow = nRow;

				if (hWndActive == hWndFrame && !m_bActivatePanes)
					m_pSelected = pItem;

				nRow++;
			}

			hWndFrame = ::GetWindow(hWndFrame, GW_HWNDNEXT);
		}
	}
	else if (!hWndClient)
	{
		CItem* pItem = new CItem;
		pItem->hWndFrame = m_pManager->GetSite()->GetSafeHwnd();
		pItem->type = itemSDIFrame;

		pItem->rc = CRect(x, y, x + nItemWidth, y + nItemHeight);

		pItem->nIndex = (int)m_arrItems.Add(pItem);
		pItem->nColumn = nColumn;
		pItem->nRow = nRow;

		if (m_pSelected == NULL)
			m_pSelected = pItem;

		m_nFirstFile++;
	}

	if (nFirstColumn < m_arrItems.GetSize())
	{
		CColumn colNext;
		colNext.nFirst = nFirstColumn;
		colNext.nLast = (int)m_arrItems.GetSize() - 1;
		m_arrColumns.Add(colNext);
	}

	if (m_nFirstFile < m_arrItems.GetSize() - 1 && !m_bActivatePanes)
	{
		if (GetKeyState(VK_SHIFT) >= 0)
			m_pSelected = m_arrItems[m_nFirstFile + 1];
		else
			m_pSelected = m_arrItems[ m_arrItems.GetSize() - 1];
	}

	nTotalRow = max(nRow, nTotalRow);
	int nTotalColumns = max(2, (int)m_arrColumns.GetSize());

	CSize sz(4 + (nItemWidth + 12) * nTotalColumns,
		nItemHeight + 13 + nTotalRow * (nItemHeight + 1) + 110);

	CXTPWindowRect rcWindow(m_pManager->GetSite());
	CPoint ptCenter = rcWindow.CenterPoint();

	CRect rect(CPoint(ptCenter.x - sz.cx / 2, ptCenter.y - sz.cy /2), sz);
	SetWindowPos(&CWnd::wndTopMost, rect.left, rect.top, rect.Width(), rect.Height(),
		SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CXTPDockingPaneWindowSelect, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CXTPDockingPaneWindowSelect)
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define CS_DROPSHADOW       0x00020000

/////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneWindowSelect message handlers

BOOL CXTPDockingPaneWindowSelect::DoModal()
{
	CWnd* pSite = m_pManager->GetSite();
	HWND hWndParent = pSite->GetSafeHwnd();
	HWND hwndFocus = ::GetFocus();

	UINT nClassStyle = CS_SAVEBITS | CS_OWNDC;

	if (XTPSystemVersion()->IsWinXPOrGreater()) // Windows XP only
	{
		nClassStyle |= CS_DROPSHADOW;
	}

	if (!CreateEx(WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(nClassStyle, m_hArrowCursor), 0,
		WS_POPUP | MFS_SYNCACTIVE, CRect(0, 0, 0, 0), pSite, 0))
		return FALSE;

	if (!Reposition())
	{
		DestroyWindow();
		return FALSE;
	}

	SetFocus();
	SetCapture();

	BOOL bEnableParent = FALSE;
	if (hWndParent != NULL && ::IsWindowEnabled(hWndParent))
	{
		CWnd::ModifyStyle(hWndParent, 0, WS_DISABLED, 0);
		bEnableParent = TRUE;
	}

	m_nFlags |= WF_CONTINUEMODAL;

	if (m_nFlags & WF_CONTINUEMODAL)
	{
		// enter modal loop
		DWORD dwFlags = MLF_SHOWONIDLE;

		VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
	}

	// hide the window before enabling the parent, etc.
	if (m_hWnd != NULL)
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
		SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

	if (bEnableParent)
		CWnd::ModifyStyle(hWndParent, WS_DISABLED, 0, 0);
	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);

	if (m_nModalResult == IDOK && m_pSelected)
	{
		if (m_pSelected->type == itemPane)
		{
			m_pManager->ShowPane(m_pSelected->pPane);
		}
		else if (m_pSelected->type == itemMDIFrame)
		{
			HWND hWndClient = GetMDIClient();
			::SendMessage(hWndClient, WM_MDIACTIVATE, (WPARAM)m_pSelected->hWndFrame, 0);

			CWnd* pWnd = CWnd::FromHandle(m_pSelected->hWndFrame);
			CWnd* pFocus = GetFocus();

			BOOL bHasFocus = pFocus->GetSafeHwnd() &&
				(pFocus == pWnd || pWnd->IsChild(pFocus) || (pFocus->GetOwner()->GetSafeHwnd() && pWnd->IsChild(pFocus->GetOwner())));

			if (!bHasFocus)
				pWnd->SetFocus();
		}
	}
	else
	{
		::SetFocus(hwndFocus);
	}

	DestroyWindow();
	return TRUE;
}

void CXTPDockingPaneWindowSelect::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	if (m_nFlags & WF_CONTINUEMODAL) EndModalLoop(IDCANCEL);
}

void CXTPDockingPaneWindowSelect::OnCaptureChanged(CWnd *pWnd)
{
	if (m_nFlags & WF_CONTINUEMODAL) EndModalLoop(IDCANCEL);

	CWnd::OnCaptureChanged(pWnd);
}

void CXTPDockingPaneWindowSelect::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting
	CXTPClientRect rc(this);

	CXTPBufferDC dc(dcPaint, rc);

	dc.FillSolidRect(rc, GetSysColor(COLOR_3DFACE));

	dc.Draw3dRect(rc, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER), GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));

	int nItemHeight = CalcItemHeight(&dc);

	CXTPFontDC font(&dc, &m_fntBold);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(COLOR_BTNTEXT);
	if (m_nPaneCount > 0)
	{
		CRect rcActiveTools(9, 0, 9 + 163, nItemHeight + 12);
		dc.DrawText(m_strActiveTools, rcActiveTools, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

		CRect rcActiveFiles( 9 + 13 + 163, 0, 9 + 13 + 2 * 163, nItemHeight + 12);
		dc.DrawText(m_strActiveFiles, rcActiveFiles, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	}
	else
	{
		CRect rcActiveFiles( 9, 0, 9 + 2 * 163, nItemHeight + 12);
		dc.DrawText(m_strActiveFiles, rcActiveFiles, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	}

	font.SetFont(&m_fnt);

	CSize szIcon = m_pManager->GetPaintManager()->GetTabPaintManager()->m_szIcon;

	for (int i = 0; i < (int)m_arrItems.GetSize(); i++)
	{
		CItem* pItem = m_arrItems[i];

		if (m_pSelected == pItem)
		{
			dc.FillSolidRect(pItem->rc, GetXtremeColor(XPCOLOR_HIGHLIGHT));
			dc.Draw3dRect(pItem->rc, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER), GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));

		}

		if (pItem->type == itemPane)
		{
			CPoint ptIcon(pItem->rc.left + 2, pItem->rc.CenterPoint().y - szIcon.cy/2);

			CXTPImageManagerIcon* pImage = pItem->pPane->GetIcon(szIcon.cx);
			if (pImage) pImage->Draw(&dc, ptIcon, szIcon);
		}
		else
		{
			CPoint ptIcon(pItem->rc.left + 2, pItem->rc.CenterPoint().y - szIcon.cy/2);

			HICON hIcon = (HICON)::SendMessage(pItem->hWndFrame, WM_GETICON, ICON_SMALL, 0);
			if (hIcon == NULL) hIcon = (HICON)::SendMessage(pItem->hWndFrame, WM_GETICON, ICON_BIG, 0);
			if (hIcon == NULL) hIcon = (HICON)(ULONG_PTR)::GetClassLongPtr(pItem->hWndFrame, GCLP_HICONSM);

			if (hIcon) ::DrawIconEx(dc, ptIcon.x, ptIcon.y, hIcon, szIcon.cx, szIcon.cy, 0, 0, DI_NORMAL);
		}

		CRect rcText(pItem->rc);
		rcText.left += 5 + szIcon.cx;
		dc.DrawText(GetItemCaption(pItem), rcText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
	}

	CRect rcInfoPane(11, rc.bottom - 100, rc.right - 11, rc.bottom - 10);
	dc.Draw3dRect(rcInfoPane, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER), GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));

	if (!m_pSelected)
		return;

	rcInfoPane.DeflateRect(15, 13, 15, 13);

	font.SetFont(&m_fntBold);

	CRect rcCaption(rcInfoPane.left, rcInfoPane.top, rcInfoPane.right, rcInfoPane.top + rcInfoPane.Height() / 3);
	dc.DrawText(GetItemCaption(m_pSelected), rcCaption, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

	font.SetFont(&m_fnt);

	CRect rcTypeName(rcInfoPane.left, rcCaption.bottom, rcInfoPane.right, rcCaption.bottom + rcInfoPane.Height() / 3);
	dc.DrawText(GetItemDescription(m_pSelected), rcTypeName, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

	CRect rcPathName(rcInfoPane.left, rcTypeName.bottom, rcInfoPane.right, rcInfoPane.bottom);
	dc.DrawText(GetItemPath(m_pSelected), rcPathName, DT_SINGLELINE | DT_VCENTER | DT_PATH_ELLIPSIS | DT_NOPREFIX);
}

void CXTPDockingPaneWindowSelect::Select(int nItem)
{
	Select(nItem >= 0 && nItem < m_arrItems.GetSize() ? m_arrItems[nItem] : NULL);
}


void CXTPDockingPaneWindowSelect::Select(CItem* pItem)
{
	if (m_pSelected != pItem)
	{
		m_pSelected = pItem;
		Invalidate(FALSE);
	}
}

void CXTPDockingPaneWindowSelect::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CMiniFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CXTPDockingPaneWindowSelect::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	OnKeyDown(nChar, nRepCnt, nFlags);
}

void CXTPDockingPaneWindowSelect::OnKeyDown(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
	if (nChar == VK_TAB)
	{
		if (GetKeyState(VK_SHIFT) >= 0)
		{
			int nSelected = m_pSelected ? m_pSelected->nIndex + 1: 0;
			if (nSelected >= m_arrItems.GetSize())
				nSelected = m_nFirstFile == m_arrItems.GetSize() ? 0 : m_nFirstFile;
			else if (nSelected == m_nFirstFile)
				nSelected = 0;
			Select(nSelected);

		}
		else
		{
			int nSelected = m_pSelected ? m_pSelected->nIndex - 1: (int)m_arrItems.GetSize() - 1;
			if (nSelected < 0)
				nSelected = m_nFirstFile == 0 ? (int)m_arrItems.GetSize() - 1 : m_nFirstFile - 1;
			else if (nSelected == m_nFirstFile - 1)
				nSelected = (int)m_arrItems.GetSize() - 1;
			Select(nSelected);
		}
	}
	else if (nChar == VK_LEFT)
	{
		if (m_arrColumns.GetSize() > 1 && m_pSelected)
		{
			int nColumn = m_pSelected->nColumn;
			int nRow = m_pSelected->nRow;
			CColumn& col = m_arrColumns[nColumn > 0 ? nColumn - 1 : m_arrColumns.GetSize() - 1];
			int nItem = col.nFirst + nRow > col.nLast ? col.nLast : col.nFirst + nRow;
			Select(nItem);
		}

	}
	else if (nChar == VK_RIGHT)
	{
		if (m_arrColumns.GetSize() > 1 && m_pSelected)
		{
			int nColumn = m_pSelected->nColumn;
			int nRow = m_pSelected->nRow;
			CColumn& col = m_arrColumns[nColumn < m_arrColumns.GetSize() - 1 ? nColumn + 1 : 0];
			int nItem = col.nFirst + nRow > col.nLast ? col.nLast : col.nFirst + nRow;
			Select(nItem);

		}
	}
	else if (nChar == VK_DOWN)
	{
		int nSelected = m_pSelected ? m_pSelected->nIndex + 1 : 0;
		Select(nSelected < m_arrItems.GetSize() ? nSelected : 0);
	}
	else if (nChar == VK_UP)
	{
		int nSelected = m_pSelected ? m_pSelected->nIndex - 1 : -1;
		Select(nSelected >= 0 ? nSelected : (int)m_arrItems.GetSize() - 1);
	}
	else if (nChar == VK_SHIFT)
	{

	}
	else
	{
		EndModalLoop(IDOK);
	}
}

void CXTPDockingPaneWindowSelect::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (::GetKeyState(VK_CONTROL) >= 0)
	{
		EndModalLoop(IDOK);
	}

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CXTPDockingPaneWindowSelect::OnLButtonDown(UINT nFlags, CPoint point)
{
	CXTPClientRect rc(this);
	if (!rc.PtInRect(point))
		EndModalLoop(IDCANCEL);

	CItem* pItem = HitTest(point);
	if (pItem)
	{
		Select(pItem);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CXTPDockingPaneWindowSelect::OnLButtonUp(UINT nFlags, CPoint point)
{
	CItem* pItem = HitTest(point);
	if (pItem)
	{
		m_pSelected = pItem;
		EndModalLoop(IDOK);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CXTPDockingPaneWindowSelect::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	CItem* pItem = HitTest(point);
	::SetCursor(pItem ? m_hHandCursor : m_hArrowCursor);
}

void CXTPDockingPaneWindowSelect::PostNcDestroy()
{

}

//////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneKeyboardHook

CThreadLocal<CXTPDockingPaneKeyboardHook> CXTPDockingPaneKeyboardHook::_xtpKeyboardThreadState;



CXTPDockingPaneKeyboardHook::CXTPDockingPaneKeyboardHook()
{
	m_pWindowSelect = 0;

	m_hHookKeyboard = 0;

#ifdef _AFXDLL
	m_pModuleState = 0;
#endif
}

CXTPDockingPaneKeyboardHook::~CXTPDockingPaneKeyboardHook()
{
	ASSERT(m_mapSites.IsEmpty());
	ASSERT(m_hHookKeyboard == 0);
}

CXTPDockingPaneManager* CXTPDockingPaneKeyboardHook::FindFocusedManager()
{
	HWND hWnd = GetFocus();
	while (hWnd)
	{
		CXTPDockingPaneManager* pManager = Lookup(hWnd);
		if (pManager)
		{
			if (!pManager->GetSite()->IsWindowEnabled())
				return FALSE;

			return pManager;
		}

		LONG dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);

		if (dwStyle & WS_CHILD)
		{
			hWnd = ::GetParent(hWnd);
		}
		else if (::GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
		{
			CXTPDockingPaneMiniWnd* pMinWnd = DYNAMIC_DOWNCAST(CXTPDockingPaneMiniWnd,
				CWnd::FromHandle(hWnd));

			if (pMinWnd != NULL)
				return pMinWnd->GetDockingPaneManager();

			return NULL;
		}
		else return NULL;
	}
	return NULL;
}

LRESULT CALLBACK CXTPDockingPaneKeyboardHook::KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	CXTPDockingPaneKeyboardHook* pKeyboardManager = GetThreadState();

	if (code != HC_ACTION)
		return CallNextHookEx(pKeyboardManager->m_hHookKeyboard, code, wParam, lParam);

	if ((!(HIWORD(lParam) & KF_UP)))
	{
		if (wParam == VK_TAB && GetKeyState(VK_CONTROL) < 0 && !(HIWORD(lParam) & KF_ALTDOWN))
		{
			SAFE_MANAGE_STATE(pKeyboardManager->m_pModuleState);

			CXTPDockingPaneWindowSelect*& pWindowSelect = pKeyboardManager->m_pWindowSelect;

			if (pWindowSelect)
			{
				pWindowSelect->OnKeyDown(VK_TAB, 0, 0);
				return TRUE;
			}

			CXTPDockingPaneManager* pManager = pKeyboardManager->FindFocusedManager();
			if (pManager && (pManager->IsKeyboardNavigateEnabled() & xtpPaneKeyboardUseCtrlTab))
			{
				pWindowSelect = (CXTPDockingPaneWindowSelect*)pManager->GetKeyboardWindowSelectClass()->CreateObject();
				pWindowSelect->m_pManager = pManager;
				pWindowSelect->m_bActivatePanes = FALSE;

				BOOL bResult = pWindowSelect->DoModal();

				delete pWindowSelect;
				pWindowSelect = 0;

				if (bResult)
					return TRUE;
			}
		}

		if ((HIWORD(lParam) & KF_ALTDOWN) && (wParam == VK_F7) && GetKeyState(VK_CONTROL) >= 0)
		{
			SAFE_MANAGE_STATE(pKeyboardManager->m_pModuleState);

			CXTPDockingPaneWindowSelect*& pWindowSelect = pKeyboardManager->m_pWindowSelect;

			if (pWindowSelect)
			{
				pWindowSelect->OnKeyDown(VK_TAB, 0, 0);
				return TRUE;
			}

			CXTPDockingPaneManager* pManager = pKeyboardManager->FindFocusedManager();
			if (pManager && (pManager->IsKeyboardNavigateEnabled() & xtpPaneKeyboardUseAltF7))
			{
				pWindowSelect = (CXTPDockingPaneWindowSelect*)pManager->GetKeyboardWindowSelectClass()->CreateObject();
				pWindowSelect->m_pManager = pManager;
				pWindowSelect->m_bActivatePanes = TRUE;

				BOOL bResult = pWindowSelect->DoModal();

				delete pWindowSelect;
				pWindowSelect = 0;

				if (bResult)
					return TRUE;
			}
		}

		if ((HIWORD(lParam) & KF_ALTDOWN) && (wParam == VK_F6) && GetKeyState(VK_CONTROL) >= 0) // Alt + F6
		{
			SAFE_MANAGE_STATE(pKeyboardManager->m_pModuleState);

			CXTPDockingPaneManager* pManager = pKeyboardManager->FindFocusedManager();
			if (pManager && (pManager->IsKeyboardNavigateEnabled() & xtpPaneKeyboardUseAltF6))
			{
				pManager->PostMessage(WM_SYSCOMMAND,
					(UINT)(GetKeyState(VK_SHIFT) >= 0 ? SC_NEXTWINDOW : SC_PREVWINDOW), 0);
				return TRUE;
			}
		}

		if ((HIWORD(lParam) & KF_ALTDOWN) && ((wParam == 0xBD || wParam == VK_SUBTRACT)) &&
			GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0) //  Alt + '-'
		{
			SAFE_MANAGE_STATE(pKeyboardManager->m_pModuleState);

			CXTPDockingPaneManager* pManager = pKeyboardManager->FindFocusedManager();
			if (pManager && (pManager->IsKeyboardNavigateEnabled() & xtpPaneKeyboardUseAltMinus))
			{
				CXTPDockingPane* pPane = pManager->GetActivePane();
				if (pPane)
				{
					if (!pPane->IsClosed() && !pPane->IsHidden())
						pManager->PostMessage(WM_SYSCOMMAND, SC_KEYMENU, MAKELONG(TEXT('-'), 0));
					return TRUE;
				}
			}
		}
	}

	return CallNextHookEx(pKeyboardManager->m_hHookKeyboard, code, wParam, lParam);
}

CXTPDockingPaneManager* CXTPDockingPaneKeyboardHook::Lookup(HWND hSite) const
{
	CXTPDockingPaneManager* pManager = NULL;
	if (m_mapSites.Lookup(hSite, pManager))
		return pManager;

	return NULL;
}

void CXTPDockingPaneKeyboardHook::SetupKeyboardHook(CXTPDockingPaneManager* pManager, BOOL bSetup)
{
	if (!pManager->GetSite())
		return;

	if (bSetup)
	{
		if (Lookup(pManager->GetSite()->GetSafeHwnd()))
			return;

		if (m_hHookKeyboard == 0)
		{
			m_hHookKeyboard = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, AfxGetInstanceHandle(), GetCurrentThreadId());
		}


#ifdef _AFXDLL
		m_pModuleState = AfxGetModuleState();
#endif

		m_mapSites.SetAt(pManager->GetSite()->GetSafeHwnd(), pManager);
	}
	else
	{
		if (Lookup(pManager->GetSite()->GetSafeHwnd()))
		{
			m_mapSites.RemoveKey(pManager->GetSite()->GetSafeHwnd());
		}

		if (m_hHookKeyboard && m_mapSites.IsEmpty())
		{
			UnhookWindowsHookEx(m_hHookKeyboard);
			m_hHookKeyboard = 0;
		}
	}
}
