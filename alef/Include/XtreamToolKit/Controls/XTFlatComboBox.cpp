// XTFlatComboBox.cpp : implementation of the CXTFlatComboBox class.
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

#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTMemDC.h"
#include "XTFlatComboBox.h"
#include "XTFlatControlsTheme.h"
#include "XTFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EVENT_TIMER   1000

/////////////////////////////////////////////////////////////////////////////
// CXTFlatComboBox

CXTFlatComboBox::CXTFlatComboBox()
: CXTThemeManagerStyleHost(GetThemeFactoryClass())
, m_bDisableAC(FALSE)
, m_bFlatLook(TRUE)
, m_bPainted(FALSE)
, m_bHasFocus(FALSE)
, m_bAutoComp(FALSE)
, m_nStyle(0)
, m_nStyleEx(0)
, m_crBack(COLORREF_NULL)
, m_crText(COLORREF_NULL)
{
}

CXTFlatComboBox::~CXTFlatComboBox()
{
}

IMPLEMENT_THEME_HOST(CXTFlatComboBox)
IMPLEMENT_THEME_REFRESH(CXTFlatComboBox, CComboBox)

IMPLEMENT_DYNAMIC(CXTFlatComboBox, CComboBox)

BEGIN_MESSAGE_MAP(CXTFlatComboBox, CComboBox)
	//{{AFX_MSG_MAP(CXTFlatComboBox)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_CONTROL_REFLECT_EX(CBN_SETFOCUS, OnSetFocus)
	ON_CONTROL_REFLECT_EX(CBN_KILLFOCUS, OnKillFocus)
	ON_WM_SETCURSOR()
	ON_CONTROL_REFLECT_EX(CBN_EDITUPDATE, OnEditUpdate)
	ON_CONTROL_REFLECT_EX(CBN_CLOSEUP, OnEndSel)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTFlatComboBox message handlers

BOOL CXTFlatComboBox::PointInRect()
{
	ASSERT(::IsWindow(m_hWnd));

	CRect rc;
	GetWindowRect(rc);

	CPoint pt;
	GetCursorPos(&pt);

	return rc.PtInRect(pt);
}

BOOL CXTFlatComboBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (IsFlat() && (message == WM_MOUSEMOVE) && PointInRect())
	{
		SetTimer(EVENT_TIMER, 10, NULL);
		OnTimer(EVENT_TIMER);
	}

	return CComboBox::OnSetCursor(pWnd, nHitTest, message);
}

void CXTFlatComboBox::DisableFlatLook(BOOL bDisable)
{
	if (IsFlat() == bDisable)
	{
		m_bFlatLook = !bDisable;

		if (::IsWindow(m_hWnd))
		{
			if (IsFlat())
			{
				// save style.
				m_nStyle = GetStyle() & (WS_BORDER);
				m_nStyleEx = GetExStyle() & (WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

				ModifyStyle(WS_BORDER, 0);
				ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0);
			}
			else
			{
				ModifyStyle(0, m_nStyle);
				ModifyStyleEx(0, m_nStyleEx);
			}

			SetWindowPos(NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER);
		}
	}
}

void CXTFlatComboBox::OnTimer(UINT_PTR nIDEvent)
{
	if (EVENT_TIMER == nIDEvent)
	{
		if (!PointInRect())
		{
			KillTimer(EVENT_TIMER);

			if (m_bPainted == TRUE)
			{
				RedrawWindow();
			}

			m_bPainted = FALSE;
		}

		// on mouse over, show raised.
		else if (!m_bPainted)
		{
			RedrawWindow();
			m_bPainted = TRUE;
		}
	}
	else
	{
		CComboBox::OnTimer(nIDEvent);
	}
}

BOOL CXTFlatComboBox::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTFlatComboBox::OnPaint()
{
	// do default rendering first.
	CComboBox::OnPaint();

	// get the client device context.
	CClientDC dc(this);
	DoPaint(&dc);
}

void CXTFlatComboBox::CalcClientRect(CRect& rc)
{
	// deflate by 3D border, 3D edge and drop arrow (thumb).
	rc.DeflateRect(GetTheme()->GetBorderSize());
	rc.DeflateRect(GetTheme()->GetEdgeSize());
	rc.right -= GetTheme()->GetThumbSize().cx;
}

// a helper for rendering the control appearance
void CXTFlatComboBox::DoPaint(CDC* pDC)
{
	// draw theme.
	if (IsFlat())
	{
		// Get the client rect.
		CXTPClientRect r(this);

		// exclude client portions from redraw.
		CRect rClip(r);
		CalcClientRect(rClip);
		pDC->ExcludeClipRect(&rClip);

		// Paint to a memory device context to help
		// eliminate screen flicker.
		CXTMemDC memDC(pDC, r, GetXtremeColor(COLOR_3DFACE));

		if (m_bHasFocus || PointInRect())
		{
			DrawCombo(&memDC, GetDroppedState() ? xtMouseSelect : xtMouseHover);
		}
		else
		{
			DrawCombo(&memDC, xtMouseNormal);
		}
	}
}

void CXTFlatComboBox::DrawCombo(CDC* pDC, XTMouseState  eState)
{
	GetTheme()->DrawFlatComboBox(pDC, this, eState);
}

BOOL CXTFlatComboBox::OnSetFocus()
{
	if (IsFlat())
	{
		m_bHasFocus = TRUE;
		RedrawWindow();
	}

	return FALSE;
}

BOOL CXTFlatComboBox::OnKillFocus()
{
	if (IsFlat())
	{
		m_bHasFocus = FALSE;
		RedrawWindow();
	}

	return FALSE;
}

BOOL CXTFlatComboBox::PreTranslateMessage(MSG* pMsg)
{
	// Make sure that the keystrokes continue to the edit control.
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		// if tab, return or escape key, just use default.
		switch (pMsg->wParam)
		{
		case VK_DELETE:
		case VK_BACK:
			{
				if (m_bAutoComp)
				{
					m_bDisableAC = (pMsg->message == WM_KEYDOWN);
				}
				break;
			}
		case VK_TAB:
		case VK_RETURN:
		case VK_ESCAPE:
			{
				return CComboBox::PreTranslateMessage(pMsg);
			}
		}

		// If the combo box has an edit control, don't allow
		// the framework to process accelerators, let the edit
		// control handle it instead.  GetEditSel() will return
		// CB_ERR if there is no edit control present...

		if (GetEditSel() != (DWORD)CB_ERR)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);

			return TRUE;
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

BOOL CXTFlatComboBox::OnEditUpdate()
{
	// if we are not to auto update the text, get outta here
	if (m_bAutoComp)
	{
		if (m_bDisableAC)
		{
			Default();
		}
		else
		{
			// Get the text in the edit box
			CString strItemTyped;
			GetWindowText(strItemTyped);
			int nLength = strItemTyped.GetLength();

			if (nLength >= 1)
			{
				// Currently selected range
				DWORD dwCurSel = GetEditSel();
				int nStart = LOWORD(dwCurSel);
				int nEnd = HIWORD(dwCurSel);

				// Search for, and select in, and string in the combo box that is prefixed
				// by the text in the edit box
				if (SelectString(-1, strItemTyped) == CB_ERR)
				{
					SetWindowText(strItemTyped);     // No text selected, so restore what was there before
					if (dwCurSel != (DWORD)CB_ERR)
					{
						SetEditSel(nStart, nEnd);   //restore cursor postion
					}
				}

				// Set the text selection as the additional text that we have added
				if (nEnd < nLength && dwCurSel != (DWORD)CB_ERR)
				{
					SetEditSel(nStart, nEnd);
				}
				else
				{
					SetEditSel(nLength, -1);
				}
			}
		}
	}

	return FALSE;
}

BOOL CXTFlatComboBox::OnEndSel()
{
	if (IsFlat())
	{
		Invalidate();
	}

	return FALSE;
}

LRESULT CXTFlatComboBox::OnPrintClient(WPARAM wp, LPARAM)
{
	LRESULT lResult = Default();

	CDC* pDC = CDC::FromHandle((HDC)wp);
	DoPaint(pDC);

	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
// CXTEdit
/////////////////////////////////////////////////////////////////////////////

CXTEdit::CXTEdit()
: m_pParentWnd(NULL)
{
}

CXTEdit::~CXTEdit()
{
}

IMPLEMENT_DYNAMIC(CXTEdit, CEdit)

BEGIN_MESSAGE_MAP(CXTEdit, CEdit)
	//{{AFX_MSG_MAP(CXTEdit)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTEdit message handlers

bool CXTEdit::Initialize(CWnd* pParentWnd)
{
	// edit control must be valid in order to initialize.
	ASSERT_VALID(this);
	if (!::IsWindow(m_hWnd))
		return false;

	// If the parent is not valid, return false.
	ASSERT_VALID(pParentWnd);
	if (!pParentWnd || !::IsWindow(pParentWnd->m_hWnd))
		return false;

	// set the font and parent for the browse edit.
	SetFont(&XTAuxData().font);
	m_pParentWnd = pParentWnd;

	return true;
}

void CXTEdit::OnContextMenu(CWnd*, CPoint point)
{
	if (point.x == -1 && point.y == -1)
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	SetFocus();

	CMenu menu;
	VERIFY(XTPResourceManager()->LoadMenu(&menu, XT_IDM_POPUP));

	CMenu* pPopup = menu.GetSubMenu(1);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	XTFuncContextMenu(pPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, pWndPopupOwner, XT_IDR_TBAR_HEXEDIT, FALSE);
}

void CXTEdit::OnEditUndo()
{
	Undo();
}

void CXTEdit::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanUndo());
}

void CXTEdit::OnEditCut()
{
	Cut();
}

void CXTEdit::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SelectionMade() && ((GetStyle() & ES_READONLY) == 0));
}

void CXTEdit::OnEditCopy()
{
	Copy();
}

void CXTEdit::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SelectionMade());
}

void CXTEdit::OnEditPaste()
{
	Paste();
}

void CXTEdit::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanPaste());
}

void CXTEdit::OnEditClear()
{
	Clear();
}

void CXTEdit::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SelectionMade() && ((GetStyle() & ES_READONLY) == 0));
}

void CXTEdit::OnEditSelectAll()
{
	SetSel(0, -1);
}

void CXTEdit::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetWindowTextLength() > 0);
}

BOOL CXTEdit::CanPaste()
{
	return IsWindowEnabled() && ((GetStyle() & ES_READONLY) == 0) & ::IsClipboardFormatAvailable(CF_TEXT);
}

BOOL CXTEdit::SelectionMade()
{
	int nStartChar, nEndChar;
	GetSel(nStartChar, nEndChar);
	return (nStartChar < nEndChar);
}

BOOL CXTEdit::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Get the client rect.
	CRect r;
	GetClientRect(&r);

	// Paint to a memory device context to help
	// eliminate screen flicker.
	CXTMemDC memDC(&dc, r, IsWindowEnabled() ? GetXtremeColor(COLOR_WINDOW) : GetXtremeColor(COLOR_3DFACE));

	// Now let the window do its default painting...
	CEdit::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
}

void CXTEdit::OnInitMenuPopup(CMenu* pPopupMenu, UINT /*nIndex*/, BOOL bSysMenu)
{
	Default();

	if (!bSysMenu)
	{
		ASSERT(pPopupMenu != NULL);
		if (!pPopupMenu)
			return;

		// check the enabled state of various menu items
		CCmdUI state;
		state.m_pMenu = pPopupMenu;
		ASSERT(state.m_pOther == NULL);

		state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
		for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
			state.m_nIndex++)
		{
			state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
			if (state.m_nID == 0)
				continue; // menu separator or invalid cmd - ignore it

			ASSERT(state.m_pOther == NULL);
			ASSERT(state.m_pMenu != NULL);
			if (state.m_nID == (UINT)-1)
			{
				// possibly a popup menu, route to first item of that popup
				state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
				if (state.m_pSubMenu == NULL)
					continue;

				state.m_nID = state.m_pSubMenu->GetMenuItemID(0);
				if (state.m_nID == 0 || state.m_nID == (UINT)-1)
					continue; // first item of popup can't be routed to

				state.DoUpdate(this, FALSE);  // popups are never auto disabled
			}
			else
			{
				// normal menu item
				// Auto enable/disable if command is _not_ a system command
				state.m_pSubMenu = NULL;
				state.DoUpdate(this, state.m_nID < 0xF000);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTFlatEdit
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_THEME_HOST(CXTFlatEdit)
IMPLEMENT_THEME_REFRESH(CXTFlatEdit, CXTEdit)

CXTFlatEdit::CXTFlatEdit()
: CXTThemeManagerStyleHost(GetThemeFactoryClass())
, m_bFlatLook(TRUE)
, m_bHasFocus(FALSE)
, m_bPainted(FALSE)
{
	m_nStyle = m_nStyleEx = 0;
}

CXTFlatEdit::~CXTFlatEdit()
{
}

IMPLEMENT_DYNAMIC(CXTFlatEdit, CXTEdit)

BEGIN_MESSAGE_MAP(CXTFlatEdit, CXTEdit)
	//{{AFX_MSG_MAP(CXTFlatEdit)
	ON_WM_NCPAINT()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTFlatEdit message handlers

BOOL CXTFlatEdit::PointInRect()
{
	ASSERT(::IsWindow(m_hWnd));

	CRect rc;
	GetWindowRect(rc);

	CPoint pt;
	GetCursorPos(&pt);

	return rc.PtInRect(pt);
}

BOOL CXTFlatEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (IsFlat() && (message == WM_MOUSEMOVE) && PointInRect())
	{
		SetTimer(EVENT_TIMER, 10, NULL);
		OnTimer(EVENT_TIMER);
	}

	return CXTEdit::OnSetCursor(pWnd, nHitTest, message);
}

void CXTFlatEdit::DisableFlatLook(BOOL bDisable)
{
	if (m_bFlatLook == bDisable)
	{
		m_bFlatLook = !bDisable;
		SendMessage(WM_NCPAINT);
	}
}

void CXTFlatEdit::OnTimer(UINT_PTR nIDEvent)
{
	if (EVENT_TIMER == nIDEvent)
	{
		if (!PointInRect())
		{
			KillTimer(EVENT_TIMER);

			if (m_bPainted == TRUE)
			{
				SendMessage(WM_NCPAINT);
			}

			m_bPainted = FALSE;
		}

		// on mouse over, show raised.
		else if (!m_bPainted)
		{
			SendMessage(WM_NCPAINT);
			m_bPainted = TRUE;
		}
	}
	else
	{
		CXTEdit::OnTimer(nIDEvent);
	}
}

void CXTFlatEdit::OnNcPaint()
{
	if (IsFlat())
	{
		CWindowDC dc(this);

		CXTPClientRect rClient(this);
		CXTPWindowRect rWindow(this);
		ScreenToClient(rWindow);
		rClient.OffsetRect(-rWindow.left, -rWindow.top);
		dc.ExcludeClipRect(rClient);
		rWindow.OffsetRect(-rWindow.left, -rWindow.top);

		CXTMemDC memDC(&dc, rWindow, IsWindowEnabled() ? GetXtremeColor(COLOR_WINDOW) : GetXtremeColor(COLOR_3DFACE));

		DrawBorders(&memDC, rWindow);
	}
	else
	{
		Default();
	}
}

// a helper for rendering the control appearance
void CXTFlatEdit::DrawBorders(CDC* pDC, const CRect& rWindow)
{
	if (m_bHasFocus || PointInRect())
	{
		GetTheme()->DrawBorders(pDC, this, rWindow, xtMouseHover);
	}
	else
	{
		GetTheme()->DrawBorders(pDC, this, rWindow, xtMouseNormal);
	}
}

void CXTFlatEdit::OnSetFocus(CWnd* pOldWnd)
{
	CXTEdit::OnSetFocus(pOldWnd);

	if (IsFlat())
	{
		m_bHasFocus = TRUE;
		Invalidate();
	}
}

void CXTFlatEdit::OnKillFocus(CWnd* pNewWnd)
{
	CXTEdit::OnKillFocus(pNewWnd);

	if (IsFlat())
	{
		m_bHasFocus = FALSE;
		Invalidate();
	}
}
