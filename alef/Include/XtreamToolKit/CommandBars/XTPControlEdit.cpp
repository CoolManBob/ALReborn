// XTPControlEdit.cpp : implementation of the CXTPControlEdit class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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
#include "Common/XTPColorManager.h"
#include "Common/XTPToolTipContext.h"

#include "XTPControlEdit.h"
#include "XTPCommandBar.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"
#include "XTPPaintManager.h"
#include "XTPMouseManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CXTPControlEdit

IMPLEMENT_XTP_CONTROL(CXTPControlEdit, CXTPControl)

CXTPControlEdit::CXTPControlEdit()
{
	EnableAutomation();

	m_controlType = xtpControlEdit;
	m_nWidth = 100;
	m_nLabelWidth = 0;
	m_bLabel = FALSE;
	m_strEditText = _T("");
	m_pEdit = NULL;
	m_bReadOnly = FALSE;
	m_bDelayReposition = FALSE;
	m_bDelayDestroy = FALSE;
	m_bFocused = FALSE;

	m_pAutoCompleteWnd = NULL;
	m_dwShellAutoCompleteFlags = 0;
	m_bEditChanged = TRUE;
	m_dwEditStyle = 0;

	m_bShowSpinButtons = FALSE;
}

CXTPControlEdit::~CXTPControlEdit()
{
	if (m_pEdit)
	{
		m_pEdit->m_pControl = NULL;
		delete m_pEdit;
	}
	SAFE_DELETE(m_pAutoCompleteWnd);
}

void CXTPControlEdit::OnEditChanged()
{
	NotifySite(EN_CHANGE);
}

CString CXTPControlEdit::GetEditText() const
{
	CString strEditText = _GetEditText();

	return strEditText == GetEditHint() ? _T("") : strEditText;
}

CString CXTPControlEdit::_GetEditText() const
{
	if (m_pEdit && m_pEdit->GetSafeHwnd() && m_bEditChanged)
	{
		m_pEdit->GetWindowTextEx(m_strEditText);
		m_bEditChanged = FALSE;
	}
	return m_strEditText;
}


void CXTPControlEdit::SetEditText(const CString& strText)
{
	_SetEditText(strText.IsEmpty() ? GetEditHint() : strText);
}

void CXTPControlEdit::_SetEditText(const CString& strText)
{
	if (m_pEdit && m_pEdit->GetSafeHwnd() && _GetEditText() != strText)
	{
		m_pEdit->SetWindowTextEx(strText);
	}
	m_strEditText = strText;
	m_bEditChanged = FALSE;
}

BOOL CXTPControlEdit::OnHookKeyDown(UINT nChar, LPARAM lParam)
{
	ASSERT(IsFocused());

	if (!IsFocused() || !m_pEdit->GetSafeHwnd())
		return FALSE;

	if (m_pEdit && m_pEdit->m_bImeMode)
		return FALSE_EXIT;

	if (nChar == VK_RETURN || nChar == VK_TAB)
		return FALSE;

	if (nChar == VK_ESCAPE)
	{
		SetEditText(m_strLastText);
		SetFocused(FALSE);

		return GetParent()->GetType() == xtpBarTypePopup;
	}

	if (m_pEdit && m_pEdit->GetSafeHwnd() && GetKeyState(VK_MENU) < 0 && GetKeyState(VK_CONTROL) >= 0)
	{
		CXTPCommandBars* pCommandBars = m_pParent->GetCommandBars();
		if (pCommandBars && pCommandBars->OnFrameAccel((TCHAR)nChar))
			return TRUE;
	}

	if (IsSpinButtonsVisible() && (nChar == VK_UP || nChar == VK_DOWN))
	{
		NotifySpinChanged(1, nChar == VK_UP ? +1 : -1);
		if (m_pEdit && ::GetFocus() == m_pEdit->GetSafeHwnd()) m_pEdit->SetSel(0, -1);
		return TRUE;
	}

	if (m_pAutoCompleteWnd && m_pAutoCompleteWnd->IsDialogCode(nChar, lParam))
		return TRUE;

	if (m_pEdit->IsDialogCode(nChar, lParam))
		return TRUE;

	return FALSE_EXIT;
}

CXTPControlEditCtrl* CXTPControlEdit::CreateEditControl()
{
	return new CXTPControlEditCtrl();
}

void CXTPControlEdit::EnableShellAutoComplete(DWORD dwFlags /*= SHACF_FILESYSTEM | SHACF_URLALL*/)
{
	m_dwShellAutoCompleteFlags = dwFlags;
	SAFE_DELETE(m_pAutoCompleteWnd);

	if (m_pEdit->GetSafeHwnd() && dwFlags)
	{
		m_pAutoCompleteWnd = new CXTPControlComboBoxAutoCompleteWnd();

		if (FAILED(m_pAutoCompleteWnd->ShellAutoComplete(m_pEdit->GetSafeHwnd(), m_dwShellAutoCompleteFlags)))
		{
			SAFE_DELETE(m_pAutoCompleteWnd);
		}
	}
}

BOOL CXTPControlEdit::HasFocus() const
{
	return IsFocused();
}

BOOL CXTPControlEdit::IsFocusable() const
{
	return m_pEdit && m_pEdit->GetSafeHwnd();
}

void CXTPControlEdit::SetRect(CRect rcControl)
{
	CString strEditText = GetEditText();

	if (!m_pEdit)
	{
		m_pEdit = CreateEditControl();
		m_pEdit->m_pControl = this;
	}

	if (m_bDelayDestroy && m_pEdit && m_pEdit->GetSafeHwnd())
	{
		m_pEdit->DestroyWindow();
		m_bDelayDestroy = FALSE;
	}
	if (m_pEdit && m_pEdit->GetSafeHwnd() && m_pEdit->GetExStyle() & WS_EX_LAYOUTRTL)
	{
		m_pEdit->DestroyWindow();
	}

	if (m_pEdit && m_pEdit->GetSafeHwnd() && m_pEdit->GetParent() != m_pParent)
	{
		m_pEdit->DestroyWindow();
	}

	if (m_rcControl == rcControl && !(m_pEdit && !m_pEdit->GetSafeHwnd()) && (m_bDelayReposition == FALSE))
	{
		if (!(m_pEdit && m_pEdit->GetSafeHwnd() && m_pEdit->GetParent() != m_pParent))
			return;
	}

	m_rcControl = rcControl;

	if (m_pEdit)
	{
		if (!m_pEdit->GetSafeHwnd())
		{
			m_pEdit->CreateEdit(WS_CHILD | ES_AUTOHSCROLL | m_dwEditStyle, m_pParent);

			if (!GetEnabled()) m_pEdit->EnableWindow(FALSE);

			if (m_dwShellAutoCompleteFlags != 0) EnableShellAutoComplete(m_dwShellAutoCompleteFlags);
		}

		m_pEdit->SetFont(GetPaintManager()->GetIconFont(), FALSE);
		m_pEdit->SetMargins(0, 0);
		m_pEdit->UpdateCharFormat();
		if (m_pEdit->GetSafeHwnd() != ::GetFocus()) SetEditText(strEditText);
		m_pEdit->m_bComposited = HasDwmCompositedRect();

		m_pEdit->SetReadOnly(m_bReadOnly);
		rcControl.DeflateRect(m_nLabelWidth + 1 + 3, 3, 3 + (m_bShowSpinButtons ? 17 : 0), 3);
		m_pEdit->MoveWindow(rcControl);
		ShowHideEditControl();
	}
	m_bDelayReposition = FALSE;
}

void CXTPControlEdit::ShowHideEditControl()
{
	if (m_pEdit && m_pEdit->GetSafeHwnd())
	{
		m_pEdit->SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
			| (m_pParent && IsVisible() && m_pParent->GetSafeHwnd() ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
	}
}

void CXTPControlEdit::OnRemoved()
{
	ShowHideEditControl();

	CXTPControl::OnRemoved();
}

void CXTPControlEdit::SetReadOnly(BOOL bReadOnly /* = TRUE */)
{
	m_bReadOnly = bReadOnly;

	if (m_pEdit && m_pEdit->GetSafeHwnd())
	{
		m_pEdit->SetReadOnly(bReadOnly);
	}
}

BOOL CXTPControlEdit::GetReadOnly() const
{
	return m_bReadOnly;
}

void CXTPControlEdit::SetEnabled(BOOL bEnabled)
{
	if (bEnabled != m_bEnabled)
	{
		m_bEnabled = bEnabled;
		if (m_pEdit && m_pEdit->GetSafeHwnd())
		{
			m_pEdit->EnableWindow(GetEnabled());
			m_pEdit->UpdateCharFormat();
		}
		RedrawParent();
	}
}
void CXTPControlEdit::SetParent(CXTPCommandBar* pParent)
{
	CXTPControl::SetParent(pParent);

	if (!pParent && m_pEdit && m_pEdit->GetSafeHwnd())
	{
		m_pEdit->DestroyWindow();
	}
}

BOOL  CXTPControlEdit::OnSetSelected(int bSelected)
{
	if (!CXTPControl::OnSetSelected(bSelected))
		return FALSE;

	if (!bSelected)
	{
		SetFocused(FALSE);
	}

	if (m_pEdit->GetSafeHwnd())
	{
		m_pEdit->UpdateCharFormat();
	}

	return TRUE;
}

void CXTPControlEdit::OnUnderlineActivate()
{
	m_pParent->HideKeyboardTips();
	SetFocused(TRUE);
}

void CXTPControlEdit::OnClick(BOOL bKeyboard, CPoint pt)
{
	if (IsCustomizeMode())
	{
		m_pParent->SetPopuped(-1);
		m_pParent->SetSelected(-1);
		CustomizeStartDrag(pt);
	}
	else
	{
		if (bKeyboard && CWnd::GetFocus() == m_pEdit)
		{
			OnExecute();
		}
		else if (!bKeyboard && GetSpinButtonsRect().PtInRect(pt))
		{
			TrackSpinButton(pt);
		}
		else
		{
			m_pEdit->SetFocus();
		}
	}
}

void CXTPControlEdit::NotifySpinChanged(int increment, int direction)
{
	NMXTPUPDOWN nm;
	nm.iDelta = increment * direction;

	if (NotifySite(direction > 0 ? XTP_FN_SPINUP : XTP_FN_SPINDOWN, &nm) != 0)
		return;

	int nPos = _ttoi(GetEditText());
	nPos += nm.iDelta;

	while (nPos % increment)
	{
		nPos += direction;
	}

	CString strEditText;
	strEditText.Format(_T("%i"), nPos);
	SetEditText(strEditText);

	NotifySite(CBN_XTP_EXECUTE);
}

void CXTPControlEdit::TrackSpinButton(CPoint pt)
{
	m_pParent->SetSelected(-1);
	m_pParent->SetSelected(GetIndex(), FALSE);
	#define TID_TIMER 435

	CRect rc = GetSpinButtonsRect();
	CRect rcTop(rc.left, rc.top, rc.right, rc.CenterPoint().y);
	CRect rcBottom(rc.left, rcTop.bottom, rc.right, rc.bottom);

	const BOOL bTop = rcTop.PtInRect(pt);
	const CRect rcActiveRect = bTop ? rcTop : rcBottom;
	const int direction = bTop ? +1 : -1;
	int increment = 1;

	m_bSelected = m_bPressed = bTop ? 3 : 4;
	RedrawParent(FALSE);

	NotifySpinChanged(increment, direction);

	m_pParent->SetCapture();

	m_pParent->SetTimer(TID_TIMER, 530, NULL);
	BOOL bFirst = TRUE;
	DWORD dwStart = GetTickCount();

	for (;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (msg.message == WM_LBUTTONUP)
		{
			break;
		}

		if (msg.message == WM_MOUSEMOVE)
		{
			pt = CPoint(LOWORD(msg.lParam), HIWORD(msg.lParam));

			if (OnSetSelected(rcActiveRect.PtInRect(pt)))
			{
				dwStart = GetTickCount();
				RedrawParent();
			}
			continue;
		}
		if (msg.message == WM_TIMER && msg.wParam == TID_TIMER)
		{
			if (m_bSelected)
			{
				UINT uElapsed = (UINT)((GetTickCount() - dwStart) / 1024);

				increment = 1;
				if (uElapsed >= 5)
					increment = 20;
				else if (uElapsed >= 2)
					increment = 5;

				NotifySpinChanged(increment, direction);
			}

			if (bFirst)
			{
				m_pParent->SetTimer(TID_TIMER, 100, NULL);
			}
			bFirst = FALSE;
		}

		DispatchMessage (&msg);

		if (CWnd::GetCapture() != m_pParent)
			break;
	}

	m_pParent->KillTimer(TID_TIMER);
	m_bPressed = FALSE;
	ReleaseCapture();

	RedrawParent(FALSE);

	if (GetParent()->GetPosition() != xtpBarPopup)
		GetParent()->SetTrackingMode(FALSE);
}

CRect CXTPControlEdit::GetSpinButtonsRect() const
{
	if (!IsSpinButtonsVisible())
		return CRect(0, 0, 0, 0);

	CRect rc(GetRect());
	return CRect(rc.right - 18, rc.top + 1, rc.right - 1, rc.bottom - 1);
}

void CXTPControlEdit::OnMouseMove(CPoint point)
{
	CXTPControl::OnMouseMove(point);

	if (GetEnabled() && IsSpinButtonsVisible() && m_bSelected)
	{
		CRect rc(GetSpinButtonsRect());

		CRect rcTop(rc.left, rc.top, rc.right, rc.CenterPoint().y);
		CRect rcBottom(rc.left, rcTop.bottom, rc.right, rc.bottom);

		if (rcTop.PtInRect(point) && m_bSelected != 3)
		{
			m_bSelected = 3;
			RedrawParent(FALSE);
		}
		else if (rcBottom.PtInRect(point) && m_bSelected != 4)
		{
			m_bSelected = 4;
			RedrawParent(FALSE);
		}
		else if ((m_bSelected == 3 || m_bSelected == 4) && !rc.PtInRect(point))
		{
			m_bSelected = 1;
			RedrawParent(FALSE);
		}
	}
}

void CXTPControlEdit::OnMouseHover()
{
	if (!XTPMouseManager()->IsMouseLocked())
		CXTPControl::OnMouseHover();
}


void CXTPControlEdit::SetHideFlags(DWORD dwFlags)
{
	if (m_dwHideFlags != dwFlags)
	{
		m_dwHideFlags = dwFlags;

		ShowHideEditControl();
		DelayLayoutParent();
	}
}


void CXTPControlEdit::OnActionChanged(int nProperty)
{
	if (nProperty == 2)
	{
		ShowHideEditControl();
	}

	if (nProperty == 0 && m_pEdit && m_pEdit->GetSafeHwnd())    // Enabled
	{
		m_pEdit->EnableWindow(GetEnabled());
		m_pEdit->UpdateCharFormat();
	}

	if (nProperty == 4)
	{
		if (_GetEditText().IsEmpty() && !GetEditHint().IsEmpty())
		{
			_SetEditText(GetEditHint());
		}
	}
}

void CXTPControlEdit::OnActionChanging(int nProperty)
{
	if (nProperty == 4 && !GetEditHint().IsEmpty() && _GetEditText() == GetEditHint())
	{
		_SetEditText(_T(""));
	}
}

void CXTPControlEdit::OnCalcDynamicSize(DWORD dwMode)
{
	if (dwMode & LM_VERTDOCK) SetHideFlags(m_dwHideFlags | xtpHideDockingPosition);
	else SetHideFlags(m_dwHideFlags & ~xtpHideDockingPosition);
}

void CXTPControlEdit::SetEditHint(LPCTSTR lpszEditHint)
{
	if (!GetEditHint().IsEmpty() && _GetEditText() == GetEditHint())
	{
		_SetEditText(_T(""));
	}

	m_strEditHint = lpszEditHint;

	if (_GetEditText().IsEmpty() && !GetEditHint().IsEmpty())
	{
		_SetEditText(GetEditHint());
	}
}

CString CXTPControlEdit::GetEditHint() const
{
	return !m_strEditHint.IsEmpty() ? m_strEditHint : m_pAction ? m_pAction->GetEditHint() : _T("");
}

DWORD CXTPControlEdit::GetEditStyle() const
{
	return m_dwEditStyle;
}

void CXTPControlEdit::SetEditStyle(DWORD dwStyle)
{
	m_dwEditStyle = dwStyle;

	if (m_pEdit && m_pEdit->GetSafeHwnd())
	{
		m_bDelayDestroy = TRUE;
		DelayLayoutParent();
	}
}

void CXTPControlEdit::Copy(CXTPControl* pControl, BOOL bRecursive)
{
	CXTPControlEdit* pControlEdit = DYNAMIC_DOWNCAST(CXTPControlEdit, pControl);
	ASSERT(pControlEdit);

	m_dwEditStyle = pControlEdit->m_dwEditStyle;
	SetEditText(pControlEdit->GetEditText());
	m_bLabel = pControlEdit->m_bLabel;
	SetReadOnly(pControlEdit->m_bReadOnly);
	SetEditHint(pControlEdit->m_strEditHint);
	EnableShellAutoComplete(pControlEdit->m_dwShellAutoCompleteFlags);
	m_bShowSpinButtons = pControlEdit->m_bShowSpinButtons;

	CXTPControl::Copy(pControl, bRecursive);
}

BOOL CXTPControlEdit::Compare(CXTPControl* pOther)
{
	if (!CXTPControl ::Compare(pOther))
		return FALSE;

	if (!pOther->IsKindOf(RUNTIME_CLASS(CXTPControlEdit)))
		return FALSE;


	return TRUE;
}


BOOL CXTPControlEdit::IsFocused() const
{
	return m_bFocused;
}

void CXTPControlEdit::SetFocused(BOOL bFocused)
{
	if (m_bFocused == bFocused)
		return;

	m_bFocused = bFocused;

	if (bFocused)
	{
		if (m_pEdit->GetSafeHwnd() && ::GetFocus() != m_pEdit->GetSafeHwnd())
			m_pEdit->SetFocus();

		m_pParent->SetTrackingMode(TRUE, FALSE);
		m_pParent->SetSelected(m_nIndex);

		XTPMouseManager()->LockMouseMove();

		NotifySite(EN_SETFOCUS);
	}
	else
	{
		if (m_pEdit->GetSafeHwnd() && ::GetFocus() == m_pEdit->GetSafeHwnd())
			::SetFocus(m_pParent->GetTrackFocus());

		XTPMouseManager()->UnlockMouseMove();

		NotifySite(EN_KILLFOCUS);
	}

	RedrawParent();
}


void CXTPControlEdit::OnSetFocus(CWnd* pOldWnd)
{
	if (!GetEditHint().IsEmpty() && m_pEdit->GetSafeHwnd() && GetEditHint() == _GetEditText())
	{
		_SetEditText(_T(""));
	}

	m_strLastText = GetEditText();

	if (m_pParent->GetTrackFocus() == 0)
		m_pParent->SetTrackFocus(pOldWnd ? pOldWnd->GetSafeHwnd() : m_pParent->GetSite()->GetSafeHwnd());

	m_pParent->SetPopuped(-1);

	SetFocused(TRUE);
	if (m_pAutoCompleteWnd) m_pAutoCompleteWnd->SetupMessageHook(TRUE);
}

void CXTPControlEdit::OnKillFocus()
{
	if (!GetEditHint().IsEmpty() && m_pEdit->GetSafeHwnd() && _GetEditText().IsEmpty())
	{
		_SetEditText(GetEditHint());
	}

	SetFocused(FALSE);
	if (m_pAutoCompleteWnd) m_pAutoCompleteWnd->SetupMessageHook(FALSE);
}

BOOL CXTPControlEdit::IsImageVisible() const
{
	if (GetParent()->GetType() == xtpBarTypePopup)
		return GetImage(0) != NULL;

	BOOL bImageVisible = (CXTPControl::GetStyle() == xtpButtonIcon || CXTPControl::GetStyle() == xtpButtonIconAndCaption)
		&& (GetImage(0) != NULL);

	return bImageVisible;
}

BOOL CXTPControlEdit::IsCaptionVisible() const
{
	if (GetParent()->GetType() == xtpBarTypePopup)
		return TRUE;

	if (GetCaption().IsEmpty())
		return FALSE;

	XTPButtonStyle buttonStyle = GetStyle();

	if ((buttonStyle == xtpButtonCaption) || (buttonStyle == xtpButtonIconAndCaption) || (buttonStyle == xtpButtonIconAndCaptionBelow))
		return TRUE;

	return m_bLabel;
}

BEGIN_MESSAGE_MAP(CXTPControlEditCtrl, CXTPEdit)
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(XTP_TTM_WINDOWFROMPOINT, OnWindowFromPoint)
	ON_CONTROL_REFLECT(EN_CHANGE, OnEditChanged)
END_MESSAGE_MAP()

CXTPControlEditCtrl::CXTPControlEditCtrl()
{
	m_pControl = NULL;
}

void CXTPControlEditCtrl::OnEditChanged()
{
	m_pControl->m_bEditChanged = TRUE;

	if (::GetFocus() == m_hWnd && !m_bIgonoreEditChanged)
		m_pControl->OnEditChanged();

	SetMargins(0, 0);
	UpdateCharFormat();
}

void CXTPControlEditCtrl::UpdateCharFormat()
{
	CHARFORMAT2 cf = m_pControl->GetDefaultCharFormat();
	cf.cbSize = GetRichEditContext().m_bRichEdit2 ? sizeof(CHARFORMAT2) : sizeof(CHARFORMAT);

	::SendMessage(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
	::SendMessage(m_hWnd, EM_SETBKGNDCOLOR, FALSE, cf.crBackColor);
}

void CXTPControlEditCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
}


void CXTPControlEditCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pControl->IsCustomizeMode())
	{
		if (m_pControl->IsCustomizeMovable())
			m_pControl->OnClick();
	}
	else CEdit::OnLButtonDown(nFlags, point);
}

void CXTPControlEditCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pControl->IsCustomizeMode())
	{
		ClientToScreen(&point);
		m_pControl->GetParent()->ScreenToClient(&point);

		m_pControl->GetParent()->OnRButtonDown(nFlags, point);
		return;
	}

	m_pControl->SetFocused(TRUE);

	if (!ShowContextMenu(m_pControl, point))
		CXTPEdit::OnRButtonDown(nFlags, point);
}

void CXTPControlEditCtrl::OnDestroy()
{
	if (m_pControl) GetWindowTextEx(m_pControl->m_strEditText);
	CEdit::OnDestroy();
}


void CXTPControlEditCtrl::OnSetFocus(CWnd* pOldWnd)
{
	if (!m_pControl->IsCustomizeMode())
	{
		m_pControl->OnSetFocus(pOldWnd);
		CEdit::OnSetFocus(pOldWnd);

		SetSel(0, 0);
		::PostMessage(m_hWnd, EM_SETSEL, 0, -1);
	}
}

void CXTPControlEditCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CXTPEdit::OnKillFocus(pNewWnd);
	m_pControl->OnKillFocus();
}

void CXTPControlEditCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CEdit::OnMouseMove(nFlags, point);

	MapWindowPoints(m_pControl->GetParent(), &point, 1);
	m_pControl->GetParent()->OnMouseMove(nFlags, point);
}


CHARFORMAT2 CXTPControlEdit::GetDefaultCharFormat()
{
	CHARFORMAT2 cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT2));
	cf.dwMask = CFM_COLOR | CFM_BACKCOLOR;

	if (GetEnabled())
	{
		cf.crTextColor = GetXtremeColor(COLOR_WINDOWTEXT);

		if (!GetEditHint().IsEmpty() && GetEditHint() == _GetEditText())
		{
			cf.crTextColor = GetXtremeColor(COLOR_GRAYTEXT);
		}
	}
	else
	{
		cf.crTextColor = GetXtremeColor(COLOR_GRAYTEXT);
	}

	cf.crBackColor = GetPaintManager()->GetControlEditBackColor(this);

	return cf;
}

BOOL CXTPControlEditCtrl::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	CXTPCommandBar* pCommandBar = m_pControl->GetParent();

	if (pCommandBar)
	{
		pCommandBar->FilterToolTipMessage(message, wParam, lParam);
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

LRESULT CXTPControlEditCtrl::OnWindowFromPoint(WPARAM, LPARAM)
{
	return 1;
}




