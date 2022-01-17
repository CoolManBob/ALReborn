// XTPTaskDialogFrame.cpp: implementation of the CXTPTaskDialogFrame class.
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

#include "Common/XTPImageManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPVC80Helpers.h"

#include "Controls/XTThemeManager.h"
#include "Controls/XTButtonTheme.h"
#include "Controls/XTButton.h"

#include "XTPTaskDialogAPI.h"
#include "XTPTaskDialogFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPTaskDialogFrame

CXTPTaskDialogFrame::CXTPTaskDialogFrame(TASKDIALOGCONFIG* pConfig)
{
	m_pConfig = pConfig;

	ASSERT(m_pConfig != NULL);

	m_lpDlgTemplate = NULL;
	m_dwTickCount = 0;
}

CXTPTaskDialogFrame::~CXTPTaskDialogFrame()
{
	DestroyWindow();

	free(m_lpDlgTemplate);
}

BOOL CXTPTaskDialogFrame::OnInitDialog()
{
	BOOL bResult = CXTPTaskDialogClient::OnInitDialog();

	int nDefaultButton = m_pConfig->nDefaultButton;

	if (nDefaultButton == 0)
	{
		if (m_arrLinkButtons.GetSize() > 0)
		{
			nDefaultButton = m_arrLinkButtons[0]->GetDlgCtrlID();
		}
		else if (m_arrCommandButtons.GetSize() > 0)
		{
			nDefaultButton = m_arrCommandButtons[0]->GetDlgCtrlID();
		}
	}

	CWnd* pWnd = nDefaultButton > 0 ? GetDlgItem(nDefaultButton) : NULL;

	if (pWnd)
	{
		SetDefID(nDefaultButton);
		PostMessage(WM_NEXTDLGCTL, (WPARAM)pWnd->GetSafeHwnd(), TRUE);
	}

	m_dwTickCount = GetTickCount();

	if (m_pConfig->dwFlags & TDF_CALLBACK_TIMER)
	{
		SetTimer(1, 200, NULL);
	}

	return bResult;
}

INT_PTR CXTPTaskDialogFrame::DoModal()
{
	if (m_lpDlgTemplate == NULL)
	{
		const int nDlgTemplateSize = sizeof(DLGTEMPLATE) + sizeof(DWORD) * 8;
		m_lpDlgTemplate =  (DLGTEMPLATE*)malloc(nDlgTemplateSize);
		ZeroMemory(m_lpDlgTemplate, nDlgTemplateSize);
	}

	m_lpDlgTemplate->style = GetFrameStyle();

	if (m_pConfig->dwFlags & TDF_RTL_LAYOUT)
	{
		m_lpDlgTemplate->dwExtendedStyle = WS_EX_LAYOUTRTL;
	}

	if (m_lpDialogTemplate == 0)
	{
		InitModalIndirect(m_lpDlgTemplate);
	}

	return CDialog::DoModal();
}

BEGIN_MESSAGE_MAP(CXTPTaskDialogFrame, CXTPTaskDialogClient)
	//{{AFX_MSG_MAP(CXTPTaskDialogFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(TDM_SET_ELEMENT_TEXT, OnDialogSetElementText)
	ON_MESSAGE(TDM_CLICK_BUTTON, OnDialogClickButton)
	ON_MESSAGE(TDM_CLICK_RADIO_BUTTON, OnDialogClickRadioButton)

	ON_MESSAGE(TDM_ENABLE_BUTTON, OnDialogEnableButton)
	ON_MESSAGE(TDM_ENABLE_RADIO_BUTTON, OnDialogEnableRadioButton)
	ON_MESSAGE(TDM_CLICK_VERIFICATION, OnDialogClickVerificationButton)
	ON_MESSAGE(TDM_UPDATE_ICON, OnDialogUpdateIcon)
	ON_MESSAGE(TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE, OnDialogSetButtonElevationState)

	ON_MESSAGE(TDM_SET_MARQUEE_PROGRESS_BAR, OnDialogSetMarqueeProgressBar)
	ON_MESSAGE(TDM_SET_PROGRESS_BAR_STATE, OnDialogSetProgressBarState)
	ON_MESSAGE(TDM_SET_PROGRESS_BAR_RANGE, OnDialogSetProgressBarRange)
	ON_MESSAGE(TDM_SET_PROGRESS_BAR_POS, OnDialogSetProgressBarPos)
	ON_MESSAGE(TDM_SET_PROGRESS_BAR_MARQUEE, OnDialogStartProgressBarMarquee)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPTaskDialogFrame message handlers

CString CXTPTaskDialogFrame::GetFrameTitle() const
{
	CString strWindowTitle;

	if (m_pConfig->pszWindowTitle != NULL)
	{
		if (IS_INTRESOURCE(m_pConfig->pszWindowTitle))
		{
			XTPLoadStringInst(m_pConfig->hInstance,
				(UINT)(UINT_PTR)m_pConfig->pszWindowTitle, &strWindowTitle);
		}
		else
		{
			strWindowTitle = m_pConfig->pszWindowTitle;
		}
	}

	if (strWindowTitle.IsEmpty())
	{
		TCHAR szModuleName[_MAX_PATH];
		::GetModuleFileName(AfxGetInstanceHandle(), szModuleName, _MAX_PATH);

		TCHAR szFileName[_MAX_FNAME], szExt[_MAX_EXT];
		SPLITPATH_S(szModuleName, NULL, NULL, szFileName, szExt);

		strWindowTitle.Format(_T("%s%s"), szFileName, szExt);
	}

	return strWindowTitle;
}

DWORD CXTPTaskDialogFrame::GetFrameStyle() const
{
	DWORD dwStyle = WS_POPUP | WS_BORDER | WS_CAPTION | DS_ABSALIGN | DS_NOIDLEMSG | DS_3DLOOK | DS_MODALFRAME;

	dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	if (m_pConfig->dwFlags & TDF_CAN_BE_MINIMIZED)
	{
		dwStyle |= WS_MINIMIZEBOX;
	}

	if ((m_pConfig->dwFlags & TDF_ALLOW_DIALOG_CANCELLATION) || (m_pConfig->dwCommonButtons & TDCBF_CANCEL_BUTTON))
	{
		dwStyle |= WS_SYSMENU;
	}
	return dwStyle;
}

int CXTPTaskDialogFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPTaskDialogClient::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(GetFrameTitle());

	// inform callback Task Dialog was created.
	SendNotify(TDN_CREATED);

	if (m_pConfig->pszMainIcon == TD_WARNING_ICON)
		MessageBeep(MB_ICONEXCLAMATION);
	if (m_pConfig->pszMainIcon == TD_ERROR_ICON)
		MessageBeep(MB_ICONERROR);
	if (m_pConfig->pszMainIcon == TD_INFORMATION_ICON)
		MessageBeep(MB_ICONINFORMATION);


	if (!CreateClient())
	{
		TRACE0("Failed to create Task Dialog UI window.\n");
		return FALSE;
	}

	// inform callback Task Dialog was constructed.
	SendNotify(TDN_DIALOG_CONSTRUCTED);

	return 0;
}

void CXTPTaskDialogFrame::OnDestroy()
{
	CXTPTaskDialogClient::OnDestroy();

	// inform callback Task Dialog was destroyed.
	SendNotify(TDN_DESTROYED);

}


BOOL CXTPTaskDialogFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		switch (pMsg->wParam)
		{
		case VK_F1:
			{
				// inform callback F1 help key was pressed.
				SendNotify(TDN_HELP);
				return TRUE;
			}
		}
	}

	return CXTPTaskDialogClient::PreTranslateMessage(pMsg);
}

void CXTPTaskDialogFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		DWORD dwTickCount = GetTickCount() - m_dwTickCount;

		if (SendNotify(TDN_TIMER, dwTickCount) == 1)
		{
			m_dwTickCount = GetTickCount();
		}
	}

	CXTPTaskDialogClient::OnTimer(nIDEvent);
}


LRESULT CXTPTaskDialogFrame::OnDialogSetElementText(WPARAM wParam, LPARAM lParam)
{
	if (wParam == TDE_MAIN_INSTRUCTION)
	{
		LoadItemString(m_strMainInstruction, (LPCWSTR)lParam);
		RecalcLayout();
		return TRUE;
	}

	if (wParam == TDE_CONTENT)
	{
		LoadItemString(m_strContent, (LPCWSTR)lParam);

		m_wndContent.DestroyWindow();
		if (IsHyperlinksEnabled())
		{
			m_wndContent.Create(CRect(0, 0, 0, 0), m_strContent, &m_font, this);
		}


		RecalcLayout();
		return TRUE;
	}

	if (wParam == TDE_EXPANDED_INFORMATION)
	{
		LoadItemString(m_strExpandedInformation, (LPCWSTR)lParam);
		m_wndExpandedInformation.DestroyWindow();

		RecalcLayout();
		return TRUE;
	}

	if (wParam == TDE_FOOTER)
	{
		LoadItemString(m_strFooter, (LPCWSTR)lParam);

		m_wndFooter.DestroyWindow();
		if (IsHyperlinksEnabled())
		{
			m_wndFooter.Create(CRect(0, 0, 0, 0), m_strFooter, &m_font, this);
		}

		RecalcLayout();
		return TRUE;
	}

	return FALSE;
}

LRESULT CXTPTaskDialogFrame::OnDialogClickButton(WPARAM wParam, LPARAM /*lParam*/)
{
	CWnd* pWnd = GetDlgItem((UINT)wParam);
	if (!pWnd)
		return FALSE;

	WindowProc(WM_COMMAND, MAKEWPARAM((UINT)wParam, BN_CLICKED), (LPARAM)pWnd->GetSafeHwnd());

	return TRUE;
}

LRESULT CXTPTaskDialogFrame::OnDialogClickRadioButton(WPARAM wParam, LPARAM /*lParam*/)
{
	CButton* pWnd = DYNAMIC_DOWNCAST(CButton, GetDlgItem((UINT)wParam));
	if (!pWnd)
		return FALSE;

	if ((pWnd->SendMessage(WM_GETDLGCODE) & DLGC_RADIOBUTTON) == 0)
		return FALSE;

	HWND hWndCtrl = ::GetWindow(m_hWnd, GW_CHILD);

	while (hWndCtrl)
	{
		if (::SendMessage(hWndCtrl, WM_GETDLGCODE, 0, 0L) & DLGC_RADIOBUTTON)
		{
			::SendMessage(hWndCtrl, BM_SETCHECK, hWndCtrl == pWnd->GetSafeHwnd(), 0L);
		}
		hWndCtrl = ::GetWindow(hWndCtrl, GW_HWNDNEXT);
	}

	m_nSelRadioButtonID = (UINT)wParam;
	SendNotify(TDN_RADIO_BUTTON_CLICKED, (WPARAM)m_nSelRadioButtonID);

	return TRUE;
}

LRESULT CXTPTaskDialogFrame::OnDialogEnableButton(WPARAM wParam, LPARAM lParam)
{
	CWnd* pWnd = DYNAMIC_DOWNCAST(CButton, GetDlgItem((UINT)wParam));
	if (!pWnd)
		return FALSE;


	if (!lParam && (::GetFocus() == pWnd->GetSafeHwnd()))
	{
		CWnd* pWndNext = GetNextDlgTabItem(pWnd);
		PostMessage(WM_NEXTDLGCTL, (WPARAM)pWndNext->GetSafeHwnd(), TRUE);
	}

	pWnd->EnableWindow(lParam != 0);

	return TRUE;
}

LRESULT CXTPTaskDialogFrame::OnDialogEnableRadioButton(WPARAM wParam, LPARAM lParam)
{
	CButton* pWnd = DYNAMIC_DOWNCAST(CButton, GetDlgItem((UINT)wParam));
	if (!pWnd)
		return FALSE;

	if ((pWnd->SendMessage(WM_GETDLGCODE) & DLGC_RADIOBUTTON) == 0)
		return FALSE;

	pWnd->EnableWindow(lParam != 0);

	return TRUE;
}

LRESULT CXTPTaskDialogFrame::OnDialogClickVerificationButton(WPARAM wParam, LPARAM lParam)
{
	if (!m_pBtnVerify)
		return FALSE;

	m_bVerification = (wParam != 0);
	m_pBtnVerify->SetCheck(m_bVerification);
	SendNotify(TDN_VERIFICATION_CLICKED, m_bVerification);

	if (lParam)
	{
		m_pBtnVerify->SetFocus();
	}

	return TRUE;
}

LRESULT CXTPTaskDialogFrame::OnDialogUpdateIcon(WPARAM wParam, LPARAM lParam)
{
	if (wParam == TDIE_ICON_MAIN)
	{
		SAFE_DELETE_HICON(m_hIconMain);
		m_hIconMain = CreateIcon((HICON)lParam, (PCWSTR)lParam, m_sizeIconMain, IsMainIconHandleUsed());

		RecalcLayout();
		return TRUE;
	}

	if (wParam == TDIE_ICON_FOOTER)
	{
		SAFE_DELETE_HICON(m_hIconFooter);
		m_hIconFooter = CreateIcon((HICON)lParam, (PCWSTR)lParam, m_sizeIconFooter, IsFooterIconHandleUsed());

		RecalcLayout();
		return TRUE;
	}

	return FALSE;
}

#ifndef BCM_FIRST
#define BCM_FIRST               0x1600      // Button control messages
#endif

#ifndef BCM_SETSHIELD
#define BCM_SETSHIELD            (BCM_FIRST + 0x000C)
#endif

LRESULT CXTPTaskDialogFrame::OnDialogSetButtonElevationState(WPARAM wParam, LPARAM lParam)
{
	CButton* pWnd = DYNAMIC_DOWNCAST(CButton, GetDlgItem((UINT)wParam));
	if (!pWnd)
		return FALSE;

	pWnd->SendMessage(BCM_SETSHIELD, 0, lParam);

	return TRUE;
}

#ifndef PBM_SETMARQUEE
#define PBM_SETMARQUEE          (WM_USER+10)
#endif

#ifndef PBS_MARQUEE
#define PBS_MARQUEE             0x08
#endif

#ifndef PBM_SETSTATE
#define PBM_SETSTATE            (WM_USER+16)
#endif

LRESULT CXTPTaskDialogFrame::OnDialogSetMarqueeProgressBar(WPARAM wParam, LPARAM /*lParam*/)
{
	if (!m_wndProgressBar)
		return FALSE;

	m_wndProgressBar.ModifyStyle(PBS_MARQUEE, wParam ? PBS_MARQUEE : 0);

	return TRUE;
}

LRESULT CXTPTaskDialogFrame::OnDialogSetProgressBarState(WPARAM wParam, LPARAM /*lParam*/)
{
	if (!m_wndProgressBar)
		return FALSE;
	m_wndProgressBar.SendMessage(PBM_SETSTATE, wParam);
	return TRUE;
}

LRESULT CXTPTaskDialogFrame::OnDialogSetProgressBarRange(WPARAM /*wParam*/, LPARAM lParam)
{
	if (!m_wndProgressBar)
		return FALSE;

	m_wndProgressBar.SetRange(LOWORD(lParam), HIWORD(lParam));
	return TRUE;
}

LRESULT CXTPTaskDialogFrame::OnDialogSetProgressBarPos(WPARAM wParam, LPARAM /*lParam*/)
{
	if (!m_wndProgressBar)
		return FALSE;

	m_wndProgressBar.SetPos((int)wParam);
	return TRUE;
}

LRESULT CXTPTaskDialogFrame::OnDialogStartProgressBarMarquee(WPARAM wParam, LPARAM lParam)
{
	if (!m_wndProgressBar)
		return FALSE;

	m_wndProgressBar.SendMessage(PBM_SETMARQUEE, wParam, wParam ? lParam : 0);
	return TRUE;
}
