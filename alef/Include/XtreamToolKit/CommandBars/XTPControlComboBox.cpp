// XTPControlComboBox.cpp : implementation of the CXTPControlComboBox class.
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
#include "Common/XTPPropExchange.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPToolTipContext.h"

#include "XTPControlComboBox.h"
#include "XTPCommandBar.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"
#include "XTPPaintManager.h"
#include "XTPMouseManager.h"
#include "XTPControls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CXTPEdit::CRichEditContext::CRichEditContext()
{
	m_hInstance = LoadLibraryA("RICHED20.DLL");
	m_bRichEdit2 = TRUE;

	if (m_hInstance)
	{
	#ifdef _UNICODE
		m_strClassName = _T("RichEdit20W");
	#else
		m_strClassName = _T("RichEdit20A");
	#endif
	}
	else
	{
		m_hInstance = LoadLibraryA("RICHED32.DLL");
		m_strClassName = _T("RICHEDIT");
		m_bRichEdit2 = FALSE;
	}
}

CXTPEdit::CRichEditContext::~CRichEditContext()
{
}

CXTPEdit::CRichEditContext& CXTPEdit::GetRichEditContext()
{
	static CRichEditContext s_RichContext;
	return s_RichContext;
}

// CXTPEdit

IMPLEMENT_DYNCREATE(CXTPEdit, CEdit)

CXTPEdit::CXTPEdit()
{
	m_bImeMode = FALSE;
	m_bComposited = FALSE;
	m_bIgonoreEditChanged = FALSE;
}

BOOL CXTPEdit::IsCommandEnabled(UINT nID)
{
	if (nID == ID_EDIT_PASTE)
	{
		return::IsClipboardFormatAvailable(CF_TEXT);
	}
	else if (nID == ID_EDIT_CUT || nID == ID_EDIT_COPY)
	{
		int nStartChar, nEndChar;
		GetSel(nStartChar, nEndChar);
		return (nStartChar != nEndChar);
	}
	return TRUE;
}

BOOL CXTPEdit::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* /*pHandlerInfo*/)
{
	if ((nID == ID_EDIT_PASTE || nID == ID_EDIT_CUT || nID == ID_EDIT_COPY) && (nCode == CN_UPDATE_COMMAND_UI))
	{
		CCmdUI* pCmdUI = (CCmdUI*)pExtra;
		pCmdUI->Enable(IsCommandEnabled(nID));
		return TRUE;
	}

	return FALSE;
}

BOOL CXTPEdit::OnCommand(WPARAM wParam, LPARAM)
{
	UINT nID = LOWORD(wParam);

	if (nID == ID_EDIT_PASTE) Paste();
	else if (nID == ID_EDIT_COPY) Copy();
	else if (nID == ID_EDIT_CUT) Cut();
	else return FALSE;

	return TRUE;
}

BOOL CXTPEdit::ShowContextMenu(CXTPControl* pControl, CPoint point)
{
	CString strPrompt;
	if (!strPrompt.LoadString(ID_EDIT_CUT))
		return FALSE;

	CXTPCommandBars* pCommandBars = pControl->GetParent()->GetCommandBars();
	if (pCommandBars == NULL)
		return FALSE;

	CXTPMouseManager* pMouseManager = pCommandBars->GetMouseManager();
	pMouseManager->UnlockMouseMove();

	ClientToScreen(&point);

	MSG msg;
	if (PeekMessage(&msg, m_hWnd, EM_SETSEL, EM_SETSEL, PM_NOREMOVE))
	{
		SendMessage(EM_SETSEL, 0, -1);
	}

	CXTPPopupBar* pPopupBar = CXTPPopupBar::CreatePopupBar(pCommandBars);
	CXTPControl* pControlCut = pPopupBar->GetControls()->Add(xtpControlButton, ID_EDIT_CUT);
	pControlCut->SetEnabled(IsCommandEnabled(ID_EDIT_CUT));
	CXTPControl* pControlCopy = pPopupBar->GetControls()->Add(xtpControlButton, ID_EDIT_COPY);
	pControlCopy->SetEnabled(IsCommandEnabled(ID_EDIT_COPY));
	CXTPControl* pControlPaste = pPopupBar->GetControls()->Add(xtpControlButton, ID_EDIT_PASTE);
	pControlPaste->SetEnabled(IsCommandEnabled(ID_EDIT_PASTE));

	pPopupBar->SetGrabFocus(FALSE);

	int nAlign = pControl->GetParent()->GetExStyle() & (WS_EX_LAYOUTRTL | WS_EX_RIGHT) ? TPM_RIGHTALIGN : TPM_LEFTALIGN;


	int nCommand = CXTPCommandBars::TrackPopupMenu(pPopupBar, TPM_NONOTIFY | TPM_RECURSE | TPM_RETURNCMD | nAlign, point.x, point.y, this, NULL, this);

	if (nCommand > 0)
	{
		OnCommand(nCommand, 0);
	}

	pMouseManager->LockMouseMove();

	pPopupBar->InternalRelease();

	pControl->GetParent()->Redraw();
	pControl->GetParent()->UpdateWindow();

	return TRUE;
}

BOOL CXTPEdit::CreateEdit(DWORD dwStyle, CWnd* pParentWnd)
{
	if (GetRichEditContext().m_hInstance)
	{
		BOOL bResult = CWnd::Create(GetRichEditContext().m_strClassName, 0, dwStyle, CRect(0, 0, 0, 0), pParentWnd, 0);
		SendMessage(EM_SETEVENTMASK, 0, ENM_CHANGE);
		SendMessage(EM_SETTEXTMODE, TM_PLAINTEXT | TM_SINGLELEVELUNDO);

		return bResult;
	}

	return Create(dwStyle, CRect(0, 0, 0, 0), pParentWnd, 0);
}

BOOL CXTPEdit::IsDialogCode(UINT nChar, LPARAM lParam)
{
	const BOOL bNavigateKeys = (nChar == VK_BACK) || (nChar >= VK_PRIOR && nChar <= VK_DELETE) ||
		((nChar == 'C' || nChar == 'V' || nChar == 'X' || nChar == 'Z') && (GetKeyState(VK_CONTROL) < 0));

	if (!bNavigateKeys)
		return FALSE;

	MSG msg;
	msg.message = WM_KEYDOWN;
	msg.hwnd = m_hWnd;
	msg.wParam = (WPARAM)nChar;
	msg.lParam = lParam;

	if (IsDialogMessage(&msg))
		return TRUE;

	return FALSE;
}

void CXTPEdit::SetWindowTextEx(LPCTSTR lpszString)
{
	m_bIgonoreEditChanged = TRUE;
	SetWindowText(0);
	m_bIgonoreEditChanged = FALSE;
	SetWindowText(lpszString);
}


void CXTPEdit::GetWindowTextEx(CString& rString)
{
#ifdef _UNICODE
	if (GetRichEditContext().m_bRichEdit2)
	{
		GETTEXTLENGTHEX gtl =
		{
			GTL_DEFAULT, 1200
		};

		int nLen = (int)SendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&gtl);

		if (nLen > 0)
		{
			GETTEXTEX gt =
			{
				(nLen + 1) * sizeof(TCHAR), GT_DEFAULT, 1200, 0, 0
			};

			SendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)rString.GetBufferSetLength(nLen));
			rString.ReleaseBuffer();
		}
		else
		{
			rString.Empty();
		}
	}
	else
#endif
	{
		GetWindowText(rString);
	}
}

BEGIN_MESSAGE_MAP(CXTPEdit, CEdit)
	ON_MESSAGE_VOID(WM_IME_STARTCOMPOSITION, OnImeStartComposition)
	ON_MESSAGE_VOID(WM_IME_ENDCOMPOSITION, OnImeEndComposition)
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CXTPEdit::OnImeStartComposition()
{
	Default();
	m_bImeMode = TRUE;
}

void CXTPEdit::OnImeEndComposition()
{
	Default();
	m_bImeMode = FALSE;
}

void CXTPEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	m_bImeMode = FALSE;
}

void CXTPEdit::OnPaint()
{
	Default();

	if (m_bComposited)
	{
		CClientDC dc(this);
		CXTPPaintManager::FillCompositeAlpha(&dc, CXTPClientRect(this));
	}
}


//////////////////////////////////////////////////////////////////////////
// CXTPControlComboBoxAutoCompleteWnd


HHOOK CXTPControlComboBoxAutoCompleteWnd::m_hHookMessage = 0;
CXTPControlComboBoxAutoCompleteWnd* CXTPControlComboBoxAutoCompleteWnd::m_pWndMonitor = 0;

#define WM_XTP_SHELLAUTOCOMPLETESTART (WM_XTP_COMMANDBARS_BASE + 30)

CXTPControlComboBoxAutoCompleteWnd::CXTPControlComboBoxAutoCompleteWnd()
{
	m_hWndAutoComplete = 0;
	m_hWndEdit = 0;
}

CXTPControlComboBoxAutoCompleteWnd::~CXTPControlComboBoxAutoCompleteWnd()
{
	SetupMessageHook(FALSE);
	SetAutoCompeteHandle(NULL);
}

BOOL CXTPControlComboBoxAutoCompleteWnd::IsDialogCode(UINT nChar, LPARAM lParam)
{
	if (!m_hWndAutoComplete)
		return FALSE;

	if (! ((nChar == VK_UP || nChar == VK_DOWN || nChar == VK_NEXT || nChar == VK_PRIOR) && (GetKeyState(VK_MENU) >= 0)))
		return FALSE;

	HWND hWnd = ::GetWindow(m_hWndAutoComplete, GW_CHILD);
	hWnd = ::GetWindow(hWnd, GW_HWNDLAST);

	MSG msg;
	msg.message = WM_KEYDOWN;
	msg.hwnd = hWnd;
	msg.wParam = (WPARAM)nChar;
	msg.lParam = lParam;

	if (::IsDialogMessage(m_hWndAutoComplete, &msg))
		return TRUE;

	return FALSE;
}


HRESULT CXTPControlComboBoxAutoCompleteWnd::ShellAutoComplete(HWND hEdit, DWORD dwFlags)
{
	m_hWndEdit = hEdit;
	HRESULT hResult = E_FAIL;

	HMODULE hLib = LoadLibraryA("shlwapi.dll");
	if (hLib)
	{
		typedef HRESULT (STDAPICALLTYPE *SHAUTOCOMPLETE)(HWND hwndEdit, DWORD dwFlags);

		SHAUTOCOMPLETE _SHAutoComplete = NULL;
		_SHAutoComplete = (SHAUTOCOMPLETE)GetProcAddress(hLib, "SHAutoComplete");

		if (_SHAutoComplete)
		{
			hResult = _SHAutoComplete(hEdit, dwFlags);
		}

		FreeLibrary(hLib);
	}

	return hResult;
}

void CXTPControlComboBoxAutoCompleteWnd::CloseWindow()
{
	if (m_hWndAutoComplete)
	{
		ShowWindow(m_hWndAutoComplete, SW_HIDE);
	}
}

void CXTPControlComboBoxAutoCompleteWnd::SetAutoCompeteHandle(HWND hWnd)
{
	if (m_hWndAutoComplete == hWnd)
		return;

	if (hWnd)
	{
		XTPMouseManager()->AddTrustedWindow(hWnd);
	}
	else
	{
		XTPMouseManager()->RemoveTrustedWindow(m_hWndAutoComplete);
	}

	m_hWndAutoComplete = hWnd;
}

void CXTPControlComboBoxAutoCompleteWnd::SetupMessageHook(BOOL bSetup)
{
	if (bSetup && m_hHookMessage == 0)
	{
		ASSERT(m_pWndMonitor == NULL);
		m_hHookMessage = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, AfxGetInstanceHandle(), GetCurrentThreadId());
		XTPKeyboardManager()->HookKeyboard(this);
		m_pWndMonitor = this;
	}

	if (!bSetup && m_hHookMessage && (m_pWndMonitor == this))
	{
		UnhookWindowsHookEx(m_hHookMessage);
		XTPKeyboardManager()->UnhookKeyboard(m_pWndMonitor);
		m_hHookMessage = 0;
		m_pWndMonitor = NULL;
	}

	if (!bSetup)
	{
		SetAutoCompeteHandle(NULL);
	}
}

int CXTPControlComboBoxAutoCompleteWnd::OnHookMessage(HWND /*hWnd*/, UINT nMessage, WPARAM& /*wParam*/, LPARAM& /*lParam*/, LRESULT& /*lResult*/)
{
	if (nMessage == WM_MOUSEWHEEL && m_hWndAutoComplete)
		return FALSE_EXIT;

	return FALSE;
}

LRESULT CALLBACK CXTPControlComboBoxAutoCompleteWnd::CallWndProc(int code, WPARAM wParam, LPARAM lParam)
{
	CWPSTRUCT* pMSG = (CWPSTRUCT*)lParam;

	if (pMSG && m_pWndMonitor && pMSG->message == WM_WINDOWPOSCHANGING)
	{
		const HWND& hWndAutoComplete = m_pWndMonitor->m_hWndAutoComplete;
		WINDOWPOS* pWindowPos = (WINDOWPOS*)pMSG->lParam;

		if ((hWndAutoComplete != 0 && hWndAutoComplete == pWindowPos->hwnd) && (pWindowPos->flags & SWP_HIDEWINDOW))
		{
			m_pWndMonitor->SetAutoCompeteHandle(NULL);
		}
		else if ((hWndAutoComplete == 0) && (pWindowPos->flags & SWP_SHOWWINDOW))
		{
			HWND hWnd = pWindowPos->hwnd;
			TCHAR szClassName[255];
			::GetClassName(hWnd, szClassName, 255);

			if (_tcscmp(szClassName, _T("Auto-Suggest Dropdown")) == 0)
			{
				m_pWndMonitor->SetAutoCompeteHandle(hWnd);
				PostMessage(m_pWndMonitor->m_hWndEdit, WM_XTP_SHELLAUTOCOMPLETESTART, 0, 0);
			}
		}
	}

	return CallNextHookEx(m_hHookMessage, code, wParam, lParam);
}



// CXTPControlComboBox

IMPLEMENT_XTP_CONTROL(CXTPControlComboBox, CXTPControlPopup)

CXTPControlComboBox::CXTPControlComboBox()
{
	EnableAutomation();

	m_controlType = xtpControlComboBox;
	m_bDropDown = FALSE;
	m_comboStyle = xtpComboNormal;
	m_nWidth = 100;
	m_nLabelWidth = 0;
	m_nThumbWidth = 0;
	m_pEdit = NULL;
	m_strEditText = _T("");
	m_nCurSel = m_nLastSel = -1;
	m_bDelayReposition = FALSE;
	m_bDelayDestroy = FALSE;
	m_bIgnoreAutoComplete = m_bAutoComplete = FALSE;
	m_bFocused = FALSE;
	m_nDropDownItemCount = 12;
	m_bIgnoreSelection = FALSE;

	m_pCommandBar = new CXTPControlComboBoxList();
	((CXTPControlComboBoxList*)m_pCommandBar)->CreateListBox();

	m_dwShellAutoCompleteFlags = 0;

	m_pAutoCompleteWnd = NULL;
	m_nEditIconId = 0;
	m_bSelEndOk = FALSE;
	m_bEditChanged = TRUE;

	m_dwEditStyle = 0;
}

CXTPControlComboBox::~CXTPControlComboBox()
{
	if (m_pEdit)
	{
		delete m_pEdit;
	}
	SAFE_DELETE(m_pAutoCompleteWnd);
}

CString CXTPControlComboBox::GetEditText() const
{
	CString strEditText = _GetEditText();

	return strEditText == GetEditHint() ? _T("") : strEditText;
}

CString CXTPControlComboBox::_GetEditText() const
{
	if (m_pEdit && m_pEdit->GetSafeHwnd() && m_bEditChanged)
	{
		m_pEdit->GetWindowTextEx(m_strEditText);
		m_bEditChanged = FALSE;
	}

	return m_strEditText;
}

void CXTPControlComboBox::SetEditText(const CString& strText)
{
	_SetEditText(strText.IsEmpty() ? GetEditHint() : strText);
}

void CXTPControlComboBox::_SetEditText(const CString& strText)
{
	if (m_pEdit && m_pEdit->GetSafeHwnd() && _GetEditText() != strText)
	{
		m_pEdit->SetWindowTextEx(strText);
	}
	m_strEditText = strText;
	m_bEditChanged = FALSE;
}

void CXTPControlComboBox::SetEditHint(LPCTSTR lpszEditHint)
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

BOOL CXTPControlComboBox::IsImageVisible() const
{
	if (GetParent()->GetType() == xtpBarTypePopup)
		return GetImage(0) != NULL;

	BOOL bImageVisible = (CXTPControl::GetStyle() == xtpButtonIcon || CXTPControl::GetStyle() == xtpButtonIconAndCaption)
		&& (GetImage(0) != NULL);

	return bImageVisible;
}

BOOL CXTPControlComboBox::IsCaptionVisible() const
{
	if (GetParent()->GetType() == xtpBarTypePopup)
		return TRUE;

	if (GetCaption().IsEmpty())
		return FALSE;

	XTPButtonStyle buttonStyle = CXTPControl::GetStyle();

	if ((buttonStyle == xtpButtonCaption) || (buttonStyle == xtpButtonIconAndCaption) || (buttonStyle == xtpButtonIconAndCaptionBelow))
		return TRUE;

	return m_comboStyle == xtpComboLabel;
}

CString CXTPControlComboBox::GetEditHint() const
{
	return !m_strEditHint.IsEmpty() ? m_strEditHint : m_pAction ? m_pAction->GetEditHint() : _T("");
}

void CXTPControlComboBox::UpdatePopupSelection()
{
	CEdit* pEdit = GetEditCtrl();
	CXTPControlComboBoxPopupBar* pPopupBar = GetComboBoxPopupBar();

	if (!m_bIgnoreSelection && pEdit && pEdit->GetSafeHwnd() && pPopupBar)
	{
		CString strWindowText = _GetEditText();

		int nIndex = pPopupBar->FindStringExact(0, strWindowText);
		if (nIndex != LB_ERR)
			pPopupBar->SetCurSel(nIndex);
		else
		{
			nIndex = pPopupBar->FindString(0, strWindowText);
			pPopupBar->SetTopIndex(nIndex);
			pPopupBar->SetCurSel(nIndex);
			pPopupBar->SetCurSel(-1);
		}
	}
}

void CXTPControlComboBox::OnEditChanged()
{
	CEdit* pEdit = GetEditCtrl();

	CXTPControlComboBoxPopupBar* pPopupBar = GetComboBoxPopupBar();

	if (!m_bIgnoreSelection && m_bAutoComplete && !m_bIgnoreAutoComplete && pEdit && pEdit->GetSafeHwnd() && pPopupBar)
	{
		CString strWindowText = _GetEditText();
		int nLength = strWindowText.GetLength();

		if (nLength > 0)
		{
			// Currently selected range
			int nStart = 0, nEnd = 0;
			pEdit->GetSel(nStart, nEnd);

			if (nStart == nEnd && nLength == nEnd)
			{
				int nIndex = pPopupBar->FindString(0, strWindowText);
				if (nIndex != LB_ERR)
				{
					CString strWindowTextNew;
					pPopupBar->GetText(nIndex, strWindowTextNew);

					_SetEditText(strWindowTextNew);
					pEdit->SetSel(strWindowTextNew.GetLength(), nLength);
				}

			}
		}
		m_bIgnoreAutoComplete = TRUE;
	}

	UpdatePopupSelection();

	NotifySite(CBN_EDITCHANGE);
}



void CXTPControlComboBox::OnSelChanged()
{
	m_bIgnoreSelection = TRUE;
	_SetEditText(GetListBoxText());
	m_bIgnoreSelection = FALSE;

	RedrawParent();

	NotifySite(CBN_SELCHANGE);
}

BOOL CXTPControlComboBox::OnHookMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/)
{
	if (!IsFocused())
		return FALSE;

	if (GetDroppedState())
		return FALSE;

	if (m_pAutoCompleteWnd)
		return FALSE;

	if (!IsValidList())
		return FALSE;

	((CXTPControlComboBoxPopupBar*)m_pCommandBar)->ProcessHookKeyDown(this, zDelta > 0 ? VK_UP : VK_DOWN, 0);
	return TRUE;
}

BOOL CXTPControlComboBox::OnHookKeyDown(UINT nChar, LPARAM lParam)
{
	ASSERT(IsFocused());

	if (!IsFocused())
		return FALSE;

	m_bIgnoreAutoComplete = (nChar == VK_DELETE) || (nChar == VK_BACK);

	if (m_pEdit && m_pEdit->m_bImeMode)
		return FALSE_EXIT;

	if (nChar == VK_TAB && ((m_pAutoCompleteWnd == NULL) || ((m_dwShellAutoCompleteFlags & SHACF_USETAB) == 0)))
		return FALSE;

	if (nChar == VK_RETURN)
		return FALSE;

	if (nChar == VK_ESCAPE)
	{
		m_pCommandBar->OnTrackLost();

		SetCurSel(m_nLastSel);
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

	if ((nChar == VK_F4 && GetKeyState(VK_MENU) >= 0) || ((nChar == VK_UP || nChar == VK_DOWN) && GetKeyState(VK_MENU) < 0))
	{
		OnClick(FALSE);
		return TRUE;
	}

	if (m_pAutoCompleteWnd && m_pAutoCompleteWnd->IsDialogCode(nChar, lParam))
		return TRUE;

	if ((!m_pEdit || (nChar == VK_UP || nChar == VK_DOWN || nChar == VK_NEXT || nChar == VK_PRIOR) && (GetKeyState(VK_MENU) >= 0)) && IsValidList())
	{
		((CXTPControlComboBoxPopupBar*)m_pCommandBar)->ProcessHookKeyDown(this, nChar, lParam);
		return TRUE;
	}

	if (m_pEdit && m_pEdit->GetSafeHwnd() && m_pEdit->IsDialogCode(nChar, lParam))
	{
		return TRUE;
	}

	return FALSE_EXIT;
}

BOOL CXTPControlComboBox::GetDropDownListStyle() const
{
	return m_pEdit != NULL;
}

CXTPControlComboBoxEditCtrl* CXTPControlComboBox::CreateEditControl()
{
	return new CXTPControlComboBoxEditCtrl();
}

void CXTPControlComboBox::SetDropDownListStyle(BOOL bSet)
{
	m_bDropDown = bSet;

	if (bSet && m_pEdit == NULL)
	{
		m_pEdit = CreateEditControl();
		m_pEdit->m_pControl = this;
	}
	else if (!bSet && m_pEdit != NULL)
	{
		delete m_pEdit;
		m_pEdit = NULL;
	}
}

void CXTPControlComboBox::EnableShellAutoComplete(DWORD dwFlags /*= SHACF_FILESYSTEM | SHACF_URLALL*/)
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

void CXTPControlComboBox::EnableAutoComplete(BOOL bAutoComplete /*= TRUE*/)
{
	m_bAutoComplete = bAutoComplete;
}


void CXTPControlComboBox::SetRect(CRect rcControl)
{
	CString strEditText = GetEditText();

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

		rcControl.DeflateRect(m_nLabelWidth + 1 + 3, 3, m_nThumbWidth + 3, 3);
		DeflateEditRect(rcControl);
		m_pEdit->MoveWindow(rcControl);
		ShowHideEditControl();
	}
	m_bDelayReposition = FALSE;
}

void CXTPControlComboBox::ShowHideEditControl()
{
	if (m_pEdit && m_pEdit->GetSafeHwnd())
	{
		m_pEdit->SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
			| (m_pParent && IsVisible() && m_pParent->GetSafeHwnd() ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
	}
}

void CXTPControlComboBox::OnRemoved()
{
	ShowHideEditControl();

	CXTPControlPopup::OnRemoved();
}

void CXTPControlComboBox::SetEnabled(BOOL bEnabled)
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

void CXTPControlComboBox::SetParent(CXTPCommandBar* pParent)
{
	if (pParent != m_pParent && pParent)
	{
		CWnd* pSite = pParent->GetSite();
		if (pSite)
			SetWindowLongPtr(m_pCommandBar->GetSafeHwnd(), GWLP_HWNDPARENT, (LONG_PTR)pSite->GetSafeHwnd());
	}
	if (!pParent && m_pEdit && m_pEdit->GetSafeHwnd())
	{
		m_pEdit->DestroyWindow();

	}
	CXTPControlPopup::SetParent(pParent);
}

BOOL  CXTPControlComboBox::OnSetSelected(int bSelected)
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

BOOL CXTPControlComboBox::HasFocus() const
{
	return IsFocused() || (m_pCommandBar->IsTrackingMode()) || m_pParent->GetCommandBars()->GetMouseManager()->IsMouseLocked();
}

BOOL CXTPControlComboBox::IsFocusable() const
{
	return TRUE;
}

void CXTPControlComboBox::OnClick(BOOL bKeyboard , CPoint pt)
{
	if (m_pEdit && m_pEdit->m_bImeMode)
		return;

	if (IsCustomizeMode())
	{
		m_pParent->SetPopuped(-1);
		m_pParent->SetSelected(-1);
		CustomizeStartDrag(pt);
	}
	else if (bKeyboard && GetEnabled())
	{
		if (IsFocused())
		{
			OnExecute();
		}
		else
		{
			SetFocused(TRUE);
		}
	}
	else if (m_bPopup && IsFocused())
	{
		m_pParent->SetPopuped(-1);

	}
	else CXTPControlPopup::OnClick(bKeyboard, pt);
}

void CXTPControlComboBox::OnUnderlineActivate()
{
	m_pParent->HideKeyboardTips();
	SetFocused(TRUE);
}

void CXTPControlComboBox::OnMouseHover()
{
}

void CXTPControlComboBox::SetHideFlags(DWORD dwFlags)
{
	if (m_dwHideFlags != dwFlags)
	{
		m_dwHideFlags = dwFlags;

		ShowHideEditControl();
		DelayLayoutParent();
	}
}

void CXTPControlComboBox::OnActionChanged(int nProperty)
{
	if (nProperty == 2)    // Visible
	{
		ShowHideEditControl();
	}

	if (nProperty == 0 && m_pEdit && m_pEdit->GetSafeHwnd())    // Enabled
	{
		m_pEdit->EnableWindow(GetEnabled());
		m_pEdit->UpdateCharFormat();
	}

	if (nProperty == 4) // EditHint
	{
		if (_GetEditText().IsEmpty() && !GetEditHint().IsEmpty())
		{
			_SetEditText(GetEditHint());
		}
	}
}

void CXTPControlComboBox::OnActionChanging(int nProperty)
{
	if (nProperty == 4 && !GetEditHint().IsEmpty() && _GetEditText() == GetEditHint())
	{
		_SetEditText(_T(""));
	}
}

void CXTPControlComboBox::OnCalcDynamicSize(DWORD dwMode)
{
	if (dwMode & LM_VERTDOCK) SetHideFlags(m_dwHideFlags | xtpHideDockingPosition);
	else SetHideFlags(m_dwHideFlags & ~xtpHideDockingPosition);
}

DWORD CXTPControlComboBox::GetEditStyle() const
{
	return m_dwEditStyle;
}

void CXTPControlComboBox::SetEditStyle(DWORD dwStyle)
{
	m_dwEditStyle = dwStyle;

	if (m_pEdit && m_pEdit->GetSafeHwnd())
	{
		m_bDelayDestroy = TRUE;
		DelayLayoutParent();
	}
}


void CXTPControlComboBox::Copy(CXTPControl* pControl, BOOL bRecursive)
{
	CXTPControlComboBox* pControlCombo = DYNAMIC_DOWNCAST(CXTPControlComboBox, pControl);
	ASSERT(pControlCombo);
	if (!pControlCombo)
	{
		CXTPControlPopup::Copy(pControl, bRecursive);
		return;
	}

	m_dwEditStyle = pControlCombo->m_dwEditStyle;
	m_comboStyle = pControlCombo->m_comboStyle;

	SetEditText(pControlCombo->GetEditText());
	SetEditHint(pControlCombo->m_strEditHint);
	SetDropDownListStyle(pControlCombo->m_pEdit != NULL);
	m_bAutoComplete = pControlCombo->m_bAutoComplete;
	EnableShellAutoComplete(pControlCombo->m_dwShellAutoCompleteFlags);
	m_nEditIconId = pControlCombo->m_nEditIconId;
	m_nDropDownItemCount = pControlCombo->m_nDropDownItemCount;

	CXTPControlPopup::Copy(pControl, bRecursive);
}

BOOL CXTPControlComboBox::Compare(CXTPControl* pOther)
{
	if (!CXTPControlPopup::Compare(pOther))
		return FALSE;

	if (!pOther->IsKindOf(RUNTIME_CLASS(CXTPControlComboBox)))
		return FALSE;

	return TRUE;
}

CString CXTPControlComboBox::GetListBoxText() const
{
	CXTPControlComboBoxPopupBar* pPopupBar = GetComboBoxPopupBar();

	if (!pPopupBar)
		return _T("");

	int nSel = pPopupBar->GetCurSel();
	CString str;
	if (nSel >= 0)
		pPopupBar->GetText(nSel, str);
	return str;
}

BOOL CXTPControlComboBox::IsValidList() const
{
	return GetComboBoxPopupBar() != NULL;
}

CXTPControlComboBoxPopupBar* CXTPControlComboBox::GetComboBoxPopupBar() const
{
	return DYNAMIC_DOWNCAST(CXTPControlComboBoxPopupBar, m_pCommandBar);
}

int CXTPControlComboBox::GetCurSel() const
{
	CXTPControlComboBoxPopupBar* pPopupBar = GetComboBoxPopupBar();
	return pPopupBar ? pPopupBar->GetCurSel() : m_nCurSel;
}

void CXTPControlComboBox::SetCurSel(int nIndex)
{
	if ((GetCurSel() != nIndex) || (m_nCurSel != nIndex))
	{
		m_nCurSel = nIndex;

		CXTPControlComboBoxPopupBar* pPopupBar = GetComboBoxPopupBar();
		if (pPopupBar)
		{
			pPopupBar->SetCurSel(nIndex);
		}

		OnSelChanged();
	}
}

BOOL CXTPControlComboBox::IsFocused() const
{
	return m_bFocused;
}

void CXTPControlComboBox::SetFocused(BOOL bFocused)
{
	if (m_bFocused == bFocused)
		return;

	m_bFocused = bFocused;

	if (bFocused)
	{
		m_nLastSel = GetCurSel();
		m_bSelEndOk = FALSE;

		if (m_pEdit->GetSafeHwnd() && ::GetFocus() != m_pEdit->GetSafeHwnd())
			m_pEdit->SetFocus();

		m_pParent->SetTrackingMode(TRUE, FALSE);
		m_pParent->SetSelected(m_nIndex);

		XTPMouseManager()->LockMouseMove();

		NotifySite(CBN_SETFOCUS);
	}
	else
	{
		if (!m_bSelEndOk && !GetDroppedState() && !m_pEdit)
		{
			SetCurSel(m_nLastSel);
			NotifySite(CBN_SELENDCANCEL);
		}


		if (m_pEdit->GetSafeHwnd() && ::GetFocus() == m_pEdit->GetSafeHwnd())
			::SetFocus(m_pParent->GetTrackFocus());

		XTPMouseManager()->UnlockMouseMove();

		NotifySite(CBN_KILLFOCUS);
	}

	RedrawParent();
}


void CXTPControlComboBox::OnSetFocus(CWnd* pOldWnd)
{
	if (!GetEditHint().IsEmpty() && m_pEdit->GetSafeHwnd() && GetEditHint() == _GetEditText())
	{
		_SetEditText(_T(""));
	}

	m_strLastText = GetEditText();


	m_bIgnoreAutoComplete = TRUE;
	UpdatePopupSelection();

	if (m_pParent->GetTrackFocus() == 0)
		m_pParent->SetTrackFocus(pOldWnd ? pOldWnd->GetSafeHwnd() : m_pParent->GetSite()->GetSafeHwnd());

	if (!m_bPopup)
		m_pParent->SetPopuped(-1);

	SetFocused(TRUE);
	if (m_pAutoCompleteWnd) m_pAutoCompleteWnd->SetupMessageHook(TRUE);
}

void CXTPControlComboBox::OnKillFocus()
{
	if (!GetEditHint().IsEmpty() && m_pEdit->GetSafeHwnd() && _GetEditText().IsEmpty())
	{
		_SetEditText(GetEditHint());
	}

	SetFocused(FALSE);

	if (GetDroppedState())
		m_pParent->SetPopuped(-1);

	if (m_pAutoCompleteWnd) m_pAutoCompleteWnd->SetupMessageHook(FALSE);
}

BOOL CXTPControlComboBox::GetDroppedState() const
{
	return m_bPopup;
	//return m_pCommandBar->IsTrackingMode() && ((CXTPPopupBar*)m_pCommandBar)->GetControlPopup() == (CXTPControlPopup*)this;
}

BOOL CXTPControlComboBox::OnSetPopup(BOOL bPopup)
{
	if (bPopup)
	{
		m_bSelEndOk = FALSE;
		m_nLastSel = GetCurSel();
	}
	else if (!IsFocused() && m_pEdit == NULL && !m_bSelEndOk)
	{
		SetCurSel(m_nLastSel);
	}

	if (!bPopup && !m_bSelEndOk)
	{
		NotifySite(CBN_SELENDCANCEL);
	}


	if (m_pAutoCompleteWnd && bPopup) m_pAutoCompleteWnd->CloseWindow();

	m_bPopup = bPopup;
	if (bPopup && GetEditCtrl()) GetEditCtrl()->SetFocus();

	return CXTPControlPopup::OnSetPopup(bPopup);
}

void CXTPControlComboBox::OnExecute()
{
	if (GetCurSel() != -1 && (GetDroppedState() || (!m_pEdit && IsFocused())))
		NotifySite(CBN_SELENDOK);

	m_bSelEndOk = TRUE;

	m_nLastSel = GetCurSel();

	CXTPControlPopup::OnExecute();
}

CHARFORMAT2 CXTPControlComboBox::GetDefaultCharFormat()
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

void CXTPControlComboBox::ModifyListBoxStyle(DWORD dwRemove, DWORD dwAdd)
{
	if (IsValidList())
	{
		CXTPControlComboBoxList* pComboBoxList = ((CXTPControlComboBoxList*)m_pCommandBar);

		pComboBoxList->m_dwStyle |= dwAdd;
		pComboBoxList->m_dwStyle &= ~dwRemove;

		pComboBoxList->DestroyWindow();
		pComboBoxList->CreateListBox();
	}
}


//////////////////////////////////////////////////////////////////////////
// CXTPControlComboBoxPopup


IMPLEMENT_XTP_COMMANDBAR(CXTPControlComboBoxPopupBar, CXTPPopupBar)

CXTPControlComboBoxPopupBar::CXTPControlComboBoxPopupBar()
{
	m_bComboBar = TRUE;
	m_bGrabFocus = FALSE;
}

BOOL CXTPControlComboBoxPopupBar::ProcessHookKeyDown(CXTPControlComboBox* pComboBox, UINT nChar, LPARAM lParam)
{
	int nSel = GetCurSel();

	if (pComboBox->GetEditCtrl())
	{
		if ((GetKeyState(VK_MENU) >= 0) && (nChar == VK_UP || nChar == VK_DOWN || nChar == VK_PRIOR || nChar == VK_NEXT))
		{
			CXTPCommandBar::OnHookKeyDown(nChar, lParam);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if ((nChar == VK_F4 && GetKeyState(VK_MENU) >= 0) || ((nChar == VK_UP || nChar == VK_DOWN) && GetKeyState(VK_MENU) < 0))
		{
			return FALSE;
		}
		else
		{
			CXTPCommandBar::OnHookKeyDown(nChar, lParam);
		}
	}

	if (nSel != GetCurSel())
	{
		pComboBox->OnSelChanged();
		if (pComboBox->GetEditCtrl()->GetSafeHwnd()) pComboBox->GetEditCtrl()->SetSel(0, -1);
	}
	return TRUE;
}

BOOL CXTPControlComboBoxPopupBar::OnHookKeyDown(UINT nChar, LPARAM lParam)
{
	ASSERT(m_pControlPopup);
	CXTPControlComboBox* pComboBox = ((CXTPControlComboBox*)m_pControlPopup);
	if (!pComboBox)
		return CXTPCommandBar::OnHookKeyDown(nChar, lParam);

	if (nChar == VK_ESCAPE)
	{
		if (pComboBox->IsFocused())
			return FALSE;

		return CXTPCommandBar::OnHookKeyDown(nChar, lParam);
	}

	if (nChar == VK_TAB) return FALSE;

	return ProcessHookKeyDown(pComboBox, nChar, lParam);
}

//////////////////////////////////////////////////////////////////////////
// CXTPControlComboBoxList

CXTPControlComboBoxList::CXTPControlComboBoxList()
{
	m_barPosition = xtpBarListBox;
	m_nMRUWidth = 0;
	m_bGrabFocus = FALSE;
	m_dwStyle = 0;
}

BEGIN_MESSAGE_MAP(CXTPControlComboBoxList, CXTPPopupBar)
	ON_WM_LBUTTONUP()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


IMPLEMENT_XTP_COMMANDBAR(CXTPControlComboBoxList, CXTPControlComboBoxPopupBar)

void CXTPControlComboBoxList::CreateListBox()
{
	DWORD dwStyle = m_dwStyle & ~CBRS_GRIPPER;

	CreateEx(WS_EX_CLIENTEDGE | WS_EX_TOOLWINDOW, _T("LISTBOX"), _T(""),
		WS_POPUP | WS_VSCROLL | WS_CLIPCHILDREN | dwStyle, CRect(0, 0, 0, 0), 0, 0);
}

void CXTPControlComboBoxList::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/)
{
	// do nothing.
}

int CXTPControlComboBoxList::OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult)
{
	if (nMessage == WM_MOUSEWHEEL)
	{
		CWnd::DefWindowProc(WM_MOUSEWHEEL, wParam, lParam);
		return TRUE;
	}
	return CXTPCommandBar::OnHookMessage(hWnd, nMessage, wParam, lParam, lResult);
}


void CXTPControlComboBoxList::OnNcPaint()
{
	CXTPPaintManager* pPaintManager = GetPaintManager();

	Default();
	CWindowDC dc(this);
	CXTPWindowRect rc(this);
	rc.OffsetRect(-rc.TopLeft());
	dc.Draw3dRect(rc, pPaintManager->GetXtremeColor(XPCOLOR_MENUBAR_BORDER), pPaintManager->GetXtremeColor(XPCOLOR_MENUBAR_BORDER));

	rc.DeflateRect(1, 1);
	dc.Draw3dRect(rc, GetXtremeColor(COLOR_WINDOW), GetXtremeColor(COLOR_WINDOW));

}

BOOL CXTPControlComboBoxList::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void CXTPControlComboBoxList::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	CXTPControlComboBox* pComboBox = (CXTPControlComboBox*)m_pControlPopup;

	pComboBox->OnSelChanged();
	pComboBox->OnExecute();
}

BOOL CXTPControlComboBoxList::SetTrackingMode(int bMode, BOOL bSelectFirst, BOOL bKeyboard)
{
	if (!CXTPCommandBar::SetTrackingMode(bMode, bSelectFirst, bKeyboard))
		return FALSE;

	CXTPControlComboBox* pComboBox = ((CXTPControlComboBox*)m_pControlPopup);

	if (!bMode)
	{
		pComboBox->NotifySite(CBN_CLOSEUP);

		XTPMouseManager()->UnlockMouseMove();
		ShowWindow(SW_HIDE);

		if (GetParentCommandBar())
			GetParentCommandBar()->SetPopuped(-1);

		m_pControlPopup = 0;

	}
	else
	{
		XTPMouseManager()->LockMouseMove();

		pComboBox->UpdatePopupSelection();

		pComboBox->NotifySite(CBN_DROPDOWN);
	}


	return TRUE;
}

CListBox* CXTPControlComboBoxList::GetListBoxCtrl() const
{
	return (CListBox*)this;
}

BOOL CXTPControlComboBoxList::ProcessHookKeyDown(CXTPControlComboBox* pComboBox, UINT nChar, LPARAM lParam)
{
	int nSel = GetCurSel();

	if (pComboBox->GetEditCtrl())
	{
		if ((GetKeyState(VK_MENU) >= 0) && (nChar == VK_UP || nChar == VK_DOWN || nChar == VK_PRIOR || nChar == VK_NEXT))
		{
			CWnd::DefWindowProc(WM_KEYDOWN, (WPARAM)nChar, lParam);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if ((nChar == VK_F4 && GetKeyState(VK_MENU) >= 0) || ((nChar == VK_UP || nChar == VK_DOWN) && GetKeyState(VK_MENU) < 0))
		{
			return FALSE;
		}
		else
		{
			MSG msg;
			msg.message = WM_KEYDOWN;
			msg.hwnd = m_hWnd;
			msg.wParam = (WPARAM)nChar;
			msg.lParam = lParam;

			::IsDialogMessage(m_hWnd, &msg);
			AfxGetThread()->PumpMessage();
		}
	}

	if (nSel != GetCurSel())
	{
		pComboBox->OnSelChanged();
		if (pComboBox->GetEditCtrl()->GetSafeHwnd()) pComboBox->GetEditCtrl()->SetSel(0, -1);
	}
	return TRUE;
}

BOOL CXTPControlComboBoxList::OnHookKeyDown(UINT nChar, LPARAM lParam)
{
	ASSERT(m_pControlPopup);
	CXTPControlComboBox* pComboBox = ((CXTPControlComboBox*)m_pControlPopup);
	if (!pComboBox)
		return CXTPCommandBar::OnHookKeyDown(nChar, lParam);

	if (nChar == VK_ESCAPE)
	{
		if (pComboBox->IsFocused())
			return FALSE;

		return CXTPCommandBar::OnHookKeyDown(nChar, lParam);
	}

	if (nChar == VK_TAB) return FALSE;

	if (nChar == VK_RETURN)
	{
		m_pControlPopup->OnExecute();
		return TRUE;
	}

	return ProcessHookKeyDown(pComboBox, nChar, lParam);
}

void CXTPControlComboBoxList::OnMouseMove(UINT nFlags, CPoint point)
{
	BOOL bOutside;
	UINT nItem = GetListBoxCtrl()->ItemFromPoint(point, bOutside);

	if (!bOutside && GetCurSel() != (int)nItem)
		SetCurSel(nItem);

	CXTPPopupBar::OnMouseMove(nFlags, point);
}



BOOL CXTPControlComboBoxList::Popup(CXTPControlPopup* pControlPopup, BOOL /*bSelectFirst*/)
{
	m_pControlPopup = pControlPopup;

	SetFont(GetPaintManager()->GetIconFont());

	CWnd* pWndOwner = GetOwnerSite();
	if (pWndOwner) pWndOwner->SendMessage(WM_XTP_INITCOMMANDSPOPUP, 0, (LPARAM)this);

	CXTPCommandBars* pCommandBars = GetCommandBars();

	if (pCommandBars && pCommandBars->IsLayoutRTL())
		ModifyStyleEx(WS_EX_LAYOUTRTL, WS_EX_RIGHT | WS_EX_LEFTSCROLLBAR | WS_EX_RTLREADING);
	else ModifyStyleEx(WS_EX_RIGHT | WS_EX_LEFTSCROLLBAR | WS_EX_RTLREADING, 0);

	SetTrackingMode(TRUE, FALSE);

	UpdateFlags();

	CRect rcControl = pControlPopup->GetRect();

	int nItemHeight = GetListBoxCtrl()->GetItemHeight(0);
	int nHeight = min (((CXTPControlComboBox*)m_pControlPopup)->m_nDropDownItemCount,
		max(1, GetListBoxCtrl()->GetCount())) * nItemHeight + 4;
	int nWidth = m_nMRUWidth <= 0 ? rcControl.Width() : m_nMRUWidth;
	UpdateLocation(CSize(nWidth, nHeight));

	EnableWindow();

	return TRUE;
}

void CXTPControlComboBoxList::DrawCommandBar(CDC* pDC, CRect /*rcClipBox*/)
{
	pDC->FillSolidRect(CXTPClientRect(this), GetXtremeColor(COLOR_WINDOW));
	CWnd::DefWindowProc(WM_PAINT, (WPARAM)pDC->m_hDC, 0);
}

//////////////////////////////////////////////////////////////////////////
// CXTPControlComboBoxEditCtrl

BEGIN_MESSAGE_MAP(CXTPControlComboBoxEditCtrl, CXTPEdit)
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(XTP_TTM_WINDOWFROMPOINT, OnWindowFromPoint)
	ON_MESSAGE_VOID(WM_XTP_SHELLAUTOCOMPLETESTART, OnShellAutoCompleteStart)
	ON_CONTROL_REFLECT(EN_CHANGE, OnEditChanged)
END_MESSAGE_MAP()

void CXTPControlComboBoxEditCtrl::OnEditChanged()
{
	m_pControl->m_bEditChanged = TRUE;

	if (::GetFocus() == m_hWnd && !m_bIgonoreEditChanged)
		m_pControl->OnEditChanged();

	SetMargins(0, 0);
	UpdateCharFormat();
}

void CXTPControlComboBoxEditCtrl::UpdateCharFormat()
{
	CHARFORMAT2 cf = m_pControl->GetDefaultCharFormat();
	cf.cbSize = GetRichEditContext().m_bRichEdit2 ? sizeof(CHARFORMAT2) : sizeof(CHARFORMAT);

	::SendMessage(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
	::SendMessage(m_hWnd, EM_SETBKGNDCOLOR, FALSE, cf.crBackColor);
}

void CXTPControlComboBoxEditCtrl::OnShellAutoCompleteStart()
{
	m_pControl->GetCommandBar()->OnTrackLost();
}

void CXTPControlComboBoxEditCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
}

void CXTPControlComboBoxEditCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pControl->IsCustomizeMode())
	{
		ClientToScreen(&point);
		m_pControl->GetParent()->ScreenToClient(&point);

		m_pControl->GetParent()->OnRButtonDown(nFlags, point);
		return;
	}

	m_pControl->GetCommandBar()->OnTrackLost();
	m_pControl->SetFocused(TRUE);

	if (!ShowContextMenu(m_pControl, point))
		CXTPEdit::OnRButtonDown(nFlags, point);
}

void CXTPControlComboBoxEditCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pControl->IsCustomizeMode())
	{
		if (m_pControl->IsCustomizeMovable())
			m_pControl->OnClick();
	}
	else CEdit::OnLButtonDown(nFlags, point);
}

void CXTPControlComboBoxEditCtrl::OnDestroy()
{
	GetWindowTextEx(m_pControl->m_strEditText);
	CEdit::OnDestroy();
}

BOOL CXTPControlComboBoxEditCtrl::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	CXTPCommandBar* pCommandBar = m_pControl->GetParent();

	if (pCommandBar)
	{
		pCommandBar->FilterToolTipMessage(message, wParam, lParam);
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

LRESULT CXTPControlComboBoxEditCtrl::OnWindowFromPoint(WPARAM, LPARAM)
{
	return 1;
}

void CXTPControlComboBoxEditCtrl::OnSetFocus(CWnd* pOldWnd)
{
	if (!m_pControl->IsCustomizeMode())
	{
		m_pControl->OnSetFocus(pOldWnd);
		CEdit::OnSetFocus(pOldWnd);

		SetSel(0, 0);
		::PostMessage(m_hWnd, EM_SETSEL, 0, -1);
	}
}

void CXTPControlComboBoxEditCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CXTPEdit::OnKillFocus(pNewWnd);
	m_pControl->OnKillFocus();
}

void CXTPControlComboBoxEditCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CEdit::OnMouseMove(nFlags, point);

	MapWindowPoints(m_pControl->GetParent(), &point, 1);
	m_pControl->GetParent()->OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////
// CXTPControlComboBoxList

void CXTPControlComboBoxList::Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive)
{
	CXTPPopupBar::Copy(pCommandBar, bRecursive);

	if (!m_hWnd)
		CreateListBox();

	CListBox* pListBox = (CListBox*)(CXTPControlComboBoxList*)pCommandBar;
	DWORD dwCount = pListBox->m_hWnd ? pListBox->GetCount() : 0;

	for (UINT i = 0; i < dwCount; i++)
	{
		CString str;
		pListBox->GetText(i, str);
		GetListBoxCtrl()->AddString(str);
	}

}

void CXTPControlComboBoxList::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPPopupBar::DoPropExchange(pPX);

	CListBox* pThis = (CListBox*)this;
	CString str;

	CXTPPropExchangeSection secItems(pPX->GetSection(_T("Items")));

	if (pPX->IsStoring())
	{
		if (m_hWnd)
		{
			DWORD dwCount = pThis->GetCount();
			secItems->WriteCount(dwCount);

			for (int i = 0; i < (int)dwCount; i++)
			{
				pThis->GetText(i, str);

				CString strSection;
				strSection.Format(_T("Item%i"), i);
				PX_String(&secItems, strSection, str, _T(""));

			}
		}
		else
		{
			pPX->WriteCount(0);
		}
	}
	else
	{
		if (!m_hWnd)
			CreateListBox();

		DWORD nNewCount = secItems->ReadCount();
		for (DWORD i = 0; i < nNewCount; i++)
		{
			CString strSection;
			strSection.Format(_T("Item%i"), i);

			PX_String(&secItems, strSection, str, _T(""));
			pThis->AddString(str);
		}
	}
}


int CXTPControlComboBox::GetDropDownWidth() const
{
	return m_pCommandBar->GetWidth();
}

int CXTPControlComboBox::GetCustomizeMinWidth() const
{
	return m_nLabelWidth + m_nThumbWidth + 5;

}

void CXTPControlComboBox::DeflateEditRect(CRect& rcControl)
{
	if (m_nEditIconId > 0)
	{
		CXTPImageManagerIcon* pIcon = m_pParent->GetImageManager()->GetImage(m_nEditIconId, 0);
		if (pIcon)
			rcControl.left += pIcon->GetWidth() + 2;
	}
}

void CXTPControlComboBox::DrawEditText(CDC* pDC, CRect rcText)
{
	if (!GetEditCtrl() && GetEnabled() && IsFocused())
	{
		CRect rcFocus(rcText.left - 1, rcText.top + 1, rcText.right - 1, rcText.bottom - 1);
		pDC->FillSolidRect(rcFocus, GetXtremeColor(COLOR_HIGHLIGHT));
	}

	if (m_nEditIconId > 0)
	{
		CXTPImageManagerIcon* pIcon = m_pParent->GetImageManager()->GetImage(m_nEditIconId, 0);
		if (pIcon)
		{
			pIcon->Draw(pDC, CPoint(rcText.left, rcText.CenterPoint().y - pIcon->GetHeight() / 2), GetEnabled() ? pIcon->GetIcon() : pIcon->GetDisabledIcon());
			rcText.left += pIcon->GetWidth() + 2;
		}
	}

	if (GetEditCtrl())
		return;

	CString strListBoxText = GetListBoxText();

	if (!strListBoxText.IsEmpty())
	{
		pDC->SetTextColor(GetXtremeColor(!GetEnabled() ? COLOR_GRAYTEXT: IsFocused() ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
		pDC->DrawText(strListBoxText, rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	}
	else if (!GetEditHint().IsEmpty() && GetEnabled() && !IsFocused())
	{
		pDC->SetTextColor(GetXtremeColor(COLOR_GRAYTEXT));
		pDC->DrawText(GetEditHint(), rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	}
}

