// XTPTaskDialog.cpp: implementation of the CXTPTaskDialog class.
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

#include "Common/XTPSystemHelpers.h"
#include "Common/XTPVc80Helpers.h"
#include "Common/XTPVc50Helpers.h"
#include "Common/XTPPropExchange.h"

#include "XTPTaskDialogAPI.h"
#include "XTPTaskDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef CT2CW
#define CT2CW(x) (LPCWSTR)T2CW(x)
#endif

#ifndef CW2CT
#define CW2CT(x) (LPCTSTR)W2CT(x)
#endif

//===========================================================================
// CXTPTaskDialog
//===========================================================================

CXTPTaskDialog::CXTPTaskDialog(CWnd* pWndParent)
	: CXTPTaskDialogFrame(&m_config)
{
	m_pParentWnd = pWndParent;

	m_hwndTaskDialog = NULL;
	::ZeroMemory(&m_config, sizeof(TASKDIALOGCONFIG));
	m_config.cbSize = sizeof(TASKDIALOGCONFIG);
	m_config.hInstance = AfxGetInstanceHandle();
	m_config.hwndParent = pWndParent->GetSafeHwnd();

	m_config.pfCallback = (PFTASKDIALOGCALLBACK)&TaskDialogCallbackProc;
	m_config.lpCallbackData = (LONG_PTR)this;

	m_nSelButtonID      = 0;

	ModifyTaskStyle(0, TDF_POSITION_RELATIVE_TO_WINDOW);
}

CXTPTaskDialog::~CXTPTaskDialog()
{
	ResetContent();
}

#ifndef UNALIGNED
#define UNALIGNED
#endif

static void XTPTaskDialogSetConfigText(UNALIGNED PCWSTR& lpszConfig, LPCTSTR strText)
{
	if (lpszConfig)
	{
		if (!IS_INTRESOURCE(lpszConfig))
			delete [] (LPWSTR)lpszConfig;
		lpszConfig = NULL;
	}

	if (!strText)
		return;

	if (IS_INTRESOURCE(strText))
	{
		lpszConfig = (LPWSTR)strText;
		return;
	}

	int nLength = (int)_tcslen(strText);
	if (nLength == 0)
		return;

	lpszConfig = (PCWSTR) new WCHAR[nLength + 1];

#ifdef _UNICODE
	STRCPY_S((LPWSTR)lpszConfig, nLength + 1, strText);
#else
	MultiByteToWideChar(CP_ACP, 0, strText, -1, (LPWSTR)lpszConfig, nLength + 1);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CXTPTaskDialog message handlers

HRESULT CALLBACK CXTPTaskDialog::TaskDialogCallbackProc(HWND hwnd, UINT uNotification, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{   ASSERT(0 != dwRefData);

	CXTPTaskDialog* pThis = (CXTPTaskDialog*)dwRefData;
	if (!pThis)
		return E_FAIL;

	if (uNotification == TDN_DIALOG_CONSTRUCTED)
	{
		ASSERT(0 != hwnd);
		pThis->OnAttach(hwnd);
	}

	if (uNotification == TDN_DESTROYED)
	{
		pThis->OnDialogDestroyed();
		pThis->OnDetach();
		return S_OK;
	}

	return pThis->OnDialogNotify(uNotification, wParam, lParam);
}

HRESULT CXTPTaskDialog::OnDialogNotify(UINT uNotification, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HRESULT hResult = S_OK;

	switch (uNotification)
	{
	case TDN_DIALOG_CONSTRUCTED:
		{
			OnDialogConstructed();
			break;
		}
	case TDN_NAVIGATED:
		{
			OnNavigated();
			break;
		}
	case TDN_BUTTON_CLICKED:
		{
			BOOL bCloseDialog = TRUE;
			OnButtonClicked((int)wParam, bCloseDialog);
			hResult = !bCloseDialog;
			break;
		}
	case TDN_HYPERLINK_CLICKED:
		{
			USES_CONVERSION;

			OnHyperlinkClicked(CW2CT((PCWSTR)lParam));
			break;
		}
	case TDN_TIMER:
		{
			ASSERT(UINT_MAX >= wParam);
			BOOL bReset = FALSE;
			OnTimer((DWORD)wParam, bReset);
			hResult = bReset;
			break;
		}
	case TDN_RADIO_BUTTON_CLICKED:
		{
			OnRadioButtonClicked((int)wParam);
			break;
		}
	case TDN_CREATED:
		{
			break;
		}
	case TDN_VERIFICATION_CLICKED:
		{
			OnVerificationClicked(wParam != 0);
			break;
		}
	case TDN_HELP:
		{
			OnHelp();
			break;
		}
	case TDN_EXPANDO_BUTTON_CLICKED:
		{
			OnExpandoButtonClicked(wParam != 0);
			break;
		}
	default:
		{
			TRACE0("CXTPTaskDialog::TaskDialogCallbackProc -- Unhandled notification encountered.\n");
			return E_NOTIMPL;
		}
	}
	return hResult;
}

BOOL CXTPTaskDialog::OnAttach(HWND hWndNew)
{
	ASSERT(m_hwndTaskDialog == NULL);

	if (hWndNew == NULL)
		return FALSE;

	m_hwndTaskDialog = hWndNew;
	return TRUE;
}

HWND CXTPTaskDialog::OnDetach()
{
	HWND hWnd = m_hwndTaskDialog;
	if (hWnd != NULL)
	{
		m_hwndTaskDialog = NULL;
	}
	return hWnd;
}

void CXTPTaskDialog::ResetContent()
{
	XTPTaskDialogSetConfigText(m_config.pszWindowTitle, NULL);
	XTPTaskDialogSetConfigText(m_config.pszMainInstruction, NULL);
	XTPTaskDialogSetConfigText(m_config.pszContent, NULL);
	XTPTaskDialogSetConfigText(m_config.pszVerificationText, NULL);
	XTPTaskDialogSetConfigText(m_config.pszExpandedInformation, NULL);
	XTPTaskDialogSetConfigText(m_config.pszExpandedControlText, NULL);
	XTPTaskDialogSetConfigText(m_config.pszCollapsedControlText, NULL);
	XTPTaskDialogSetConfigText(m_config.pszFooter, NULL);


	int i;
	for (i = 0; i < m_arrButtons.GetSize(); i++)
	{
		XTPTaskDialogSetConfigText(m_arrButtons[i].pszButtonText, NULL);
	}
	m_arrButtons.RemoveAll();

	for (i = 0; i < m_arrRadioButtons.GetSize(); i++)
	{
		XTPTaskDialogSetConfigText(m_arrRadioButtons[i].pszButtonText, NULL);
	}
	m_arrRadioButtons.RemoveAll();

	m_nSelButtonID      = 0;
}


void CXTPTaskDialog::SetWindowTitle(LPCTSTR pszText)
{
	CString strText = pszText;

	if (pszText && IS_INTRESOURCE(pszText))
	{
		XTPLoadStringInst(m_config.hInstance, (UINT)(UINT_PTR)pszText, &strText);
	}

	if (::IsWindow(m_hwndTaskDialog))
	{
		::SetWindowText(m_hwndTaskDialog, strText);
		return;
	}

	XTPTaskDialogSetConfigText(m_config.pszWindowTitle, strText);
}

BOOL CXTPTaskDialog::SetMainInstruction(LPCTSTR pszText)
{
	XTPTaskDialogSetConfigText(m_config.pszMainInstruction, pszText);

	if (::IsWindow(m_hwndTaskDialog))
	{
		return (::SendMessage(m_hwndTaskDialog, TDM_SET_ELEMENT_TEXT,
			TDE_MAIN_INSTRUCTION, (LPARAM)(LPCWSTR)m_config.pszMainInstruction) != 0);
	}

	return TRUE;
}

BOOL CXTPTaskDialog::SetContent(LPCTSTR pszText)
{
	XTPTaskDialogSetConfigText(m_config.pszContent, pszText);

	if (::IsWindow(m_hwndTaskDialog))
	{
		return (::SendMessage(m_hwndTaskDialog, TDM_SET_ELEMENT_TEXT,
			TDE_CONTENT, (LPARAM)(LPCWSTR)m_config.pszContent) != 0);
	}

	return TRUE;
}

BOOL CXTPTaskDialog::SetExpandedInformation(LPCTSTR pszText)
{
	XTPTaskDialogSetConfigText(m_config.pszExpandedInformation, pszText);

	if (::IsWindow(m_hwndTaskDialog))
	{
		return (::SendMessage(m_hwndTaskDialog, TDM_SET_ELEMENT_TEXT,
			TDE_EXPANDED_INFORMATION, (LPARAM)(LPCWSTR)m_config.pszExpandedInformation) != 0);
	}

	return TRUE;
}

BOOL CXTPTaskDialog::SetFooter(LPCTSTR pszText)
{
	XTPTaskDialogSetConfigText(m_config.pszFooter, pszText);

	if (::IsWindow(m_hwndTaskDialog))
	{
		return (::SendMessage(m_hwndTaskDialog, TDM_SET_ELEMENT_TEXT,
			TDE_FOOTER, (LPARAM)(LPCWSTR)m_config.pszFooter) != 0);
	}

	return TRUE;
}

BOOL CXTPTaskDialog::SetMainIcon(HICON hIcon)
{
	ModifyTaskStyle(TDF_USE_HICON_MAIN, hIcon == 0 ? 0: TDF_USE_HICON_MAIN);

	m_config.hMainIcon = hIcon;

	if (::IsWindow(m_hwndTaskDialog))
	{
		return (::SendMessage(m_hwndTaskDialog, TDM_UPDATE_ICON,
			TDIE_ICON_MAIN, (LPARAM)hIcon) != 0);
	}

	return TRUE;
}

BOOL CXTPTaskDialog::SetMainIcon(LPCWSTR pszResource)
{
	ASSERT(IS_INTRESOURCE(pszResource));

	ModifyTaskStyle(TDF_USE_HICON_MAIN, 0);

	m_config.pszMainIcon = pszResource;

	if (::IsWindow(m_hwndTaskDialog))
	{
		return (::SendMessage(m_hwndTaskDialog, TDM_UPDATE_ICON,
			TDIE_ICON_MAIN, (LPARAM)pszResource) != 0);
	}

	return TRUE;
}

BOOL CXTPTaskDialog::SetFooterIcon(HICON hIcon)
{
	ModifyTaskStyle(TDF_USE_HICON_FOOTER, (hIcon == 0) ? 0 : TDF_USE_HICON_FOOTER);

	m_config.hFooterIcon = hIcon;

	if (::IsWindow(m_hwndTaskDialog))
	{
		return (::SendMessage(m_hwndTaskDialog, TDM_UPDATE_ICON,
			TDIE_ICON_FOOTER, (LPARAM)hIcon) != NULL);
	}

	return TRUE;
}

BOOL CXTPTaskDialog::SetFooterIcon(LPCWSTR pszResource)
{
	ASSERT(IS_INTRESOURCE(pszResource));

	ModifyTaskStyle(TDF_USE_HICON_FOOTER, 0);

	m_config.pszFooterIcon = pszResource;

	if (::IsWindow(m_hwndTaskDialog))
	{
		return (::SendMessage(m_hwndTaskDialog, TDM_UPDATE_ICON,
			TDIE_ICON_FOOTER, (LPARAM)pszResource) != 0);
	}

	return TRUE;
}

void CXTPTaskDialog::SetWidth(int cxWidth, BOOL bPixelToDLU /*=FALSE*/)
{
	if (bPixelToDLU)
	{
		CSize sizeBase = ::GetDialogBaseUnits();
		cxWidth = ::MulDiv(cxWidth, 4, sizeBase.cx);
	}
	m_config.cxWidth = cxWidth;
}

void CXTPTaskDialog::AddButton(LPCTSTR pszText, int nButtonID)
{
	for (int i = 0; i < (int)m_arrButtons.GetSize(); i++)
	{
		if (m_arrButtons[i].nButtonID == nButtonID)
		{
			XTPTaskDialogSetConfigText(m_arrButtons[i].pszButtonText, pszText);
			return;
		}
	}

	TASKDIALOG_BUTTON button;
	ZeroMemory(&button, sizeof(TASKDIALOG_BUTTON));

	XTPTaskDialogSetConfigText(button.pszButtonText, pszText);
	button.nButtonID = nButtonID;
	m_arrButtons.Add(button);
}

void CXTPTaskDialog::AddRadioButton(LPCTSTR pszText, int nButtonID)
{
	for (int i = 0; i < (int)m_arrRadioButtons.GetSize(); i++)
	{
		if (m_arrRadioButtons[i].nButtonID == nButtonID)
		{
			XTPTaskDialogSetConfigText(m_arrRadioButtons[i].pszButtonText, pszText);
			return;
		}
	}

	TASKDIALOG_BUTTON radioButton;
	ZeroMemory(&radioButton, sizeof(TASKDIALOG_BUTTON));

	XTPTaskDialogSetConfigText(radioButton.pszButtonText, pszText);
	radioButton.nButtonID = nButtonID;
	m_arrRadioButtons.Add(radioButton);
}

void CXTPTaskDialog::SetVerificationText(LPCTSTR pszText)
{
	XTPTaskDialogSetConfigText(m_config.pszVerificationText, pszText);
}

void CXTPTaskDialog::SetExpandedControlText(LPCTSTR pszText)
{
	XTPTaskDialogSetConfigText(m_config.pszExpandedControlText, pszText);
}

void CXTPTaskDialog::SetCollapsedControlText(LPCTSTR pszText)
{
	XTPTaskDialogSetConfigText(m_config.pszCollapsedControlText, pszText);
}

INT_PTR CXTPTaskDialog::DoModal(BOOL bUseComCtl32 /*=TRUE*/)
{
	ASSERT(m_hwndTaskDialog == NULL);

	m_config.pButtons      = m_arrButtons.GetData();
	m_config.cButtons      = (UINT)m_arrButtons.GetSize();
	m_config.pRadioButtons = m_arrRadioButtons.GetData();
	m_config.cRadioButtons = (UINT)m_arrRadioButtons.GetSize();

	// You can only use comctl32.dll task dialog when system is vista or newer...

	if (bUseComCtl32 && (XTPSystemVersion()->dwMajorVersion >= 5))
	{
		HRESULT (WINAPI* pfnTaskDialogIndirect)(const TASKDIALOGCONFIG*, int*, int*, BOOL*);
		CXTPModuleHandle modComCtl32(_T("ComCtl32.dll"));
		if (modComCtl32.GetProcAddress((FARPROC*)&pfnTaskDialogIndirect, "TaskDialogIndirect", MAKELONG(0,6)))
		{
			if (FAILED(pfnTaskDialogIndirect(&m_config,
				&m_nSelButtonID, &m_nSelRadioButtonID, &m_bVerification)))
			{
				return 0;
			}
			return m_nSelButtonID;
		}
	}

	m_nSelButtonID = (int)CXTPTaskDialogFrame::DoModal();

	return m_nSelButtonID;
}

BOOL CXTPTaskDialog::ModifyTaskStyle(TASKDIALOG_FLAGS dwRemove, TASKDIALOG_FLAGS dwAdd)
{
	TASKDIALOG_FLAGS dwStyle = m_config.dwFlags;
	TASKDIALOG_FLAGS dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;

	if (dwStyle != dwNewStyle)
	{
		m_config.dwFlags = dwNewStyle;
		return TRUE;
	}

	return FALSE;
}

void CXTPTaskDialog::EnableHyperlinks(BOOL bEnable)
{
	ModifyTaskStyle(TDF_ENABLE_HYPERLINKS, bEnable? TDF_ENABLE_HYPERLINKS: 0);
}

void CXTPTaskDialog::EnableCancellation(BOOL bEnable)
{
	ModifyTaskStyle(TDF_ALLOW_DIALOG_CANCELLATION, bEnable? TDF_ALLOW_DIALOG_CANCELLATION: 0);
}

void CXTPTaskDialog::EnableRelativePosition(BOOL bEnable)
{
	ModifyTaskStyle(TDF_POSITION_RELATIVE_TO_WINDOW, bEnable? TDF_POSITION_RELATIVE_TO_WINDOW: 0);
}

void CXTPTaskDialog::ShowProgressBar(BOOL bShow, BOOL bMarquee /*=FALSE*/)
{
	if (bMarquee)
	{
		ModifyTaskStyle(TDF_SHOW_PROGRESS_BAR | TDF_SHOW_MARQUEE_PROGRESS_BAR,
			bShow? TDF_SHOW_MARQUEE_PROGRESS_BAR: 0);
	}
	else
	{
		ModifyTaskStyle(TDF_SHOW_MARQUEE_PROGRESS_BAR | TDF_SHOW_PROGRESS_BAR,
			bShow? TDF_SHOW_PROGRESS_BAR: 0);
	}
}

void CXTPTaskDialog::EnableRtlLayout(BOOL bEnable)
{
	ModifyTaskStyle(TDF_RTL_LAYOUT, bEnable? TDF_RTL_LAYOUT: 0);
}

void CXTPTaskDialog::ExpandedByDefault(BOOL bExpanded)
{
	ModifyTaskStyle(TDF_EXPANDED_BY_DEFAULT, bExpanded? TDF_EXPANDED_BY_DEFAULT: 0);
}

void CXTPTaskDialog::ExpandFooterArea(BOOL bExpandFooter)
{
	ModifyTaskStyle(TDF_EXPAND_FOOTER_AREA, bExpandFooter? TDF_EXPAND_FOOTER_AREA: 0);
}

void CXTPTaskDialog::SetCommonButtons(BOOL bOk, BOOL bYes, BOOL bNo, BOOL bCancel, BOOL bRetry, BOOL bClose)
{
	m_config.dwCommonButtons = 0;

	if (bOk)     m_config.dwCommonButtons |= TDCBF_OK_BUTTON;
	if (bYes)    m_config.dwCommonButtons |= TDCBF_YES_BUTTON;
	if (bNo)     m_config.dwCommonButtons |= TDCBF_NO_BUTTON;
	if (bCancel) m_config.dwCommonButtons |= TDCBF_CANCEL_BUTTON;
	if (bRetry)  m_config.dwCommonButtons |= TDCBF_RETRY_BUTTON;
	if (bClose)  m_config.dwCommonButtons |= TDCBF_CLOSE_BUTTON;
}

void CXTPTaskDialog::SetCommonButtons(TASKDIALOG_COMMON_BUTTON_FLAGS dwButtons)
{
	m_config.dwCommonButtons = dwButtons;
}

void CXTPTaskDialog::EnableCommandLinks(BOOL bEnable, BOOL bShowIcon)
{
	if (bShowIcon)
	{
		ModifyTaskStyle(TDF_USE_COMMAND_LINKS | TDF_USE_COMMAND_LINKS_NO_ICON,
			bEnable? TDF_USE_COMMAND_LINKS: 0);
	}
	else
	{
		ModifyTaskStyle(TDF_USE_COMMAND_LINKS_NO_ICON | TDF_USE_COMMAND_LINKS,
			bEnable? TDF_USE_COMMAND_LINKS_NO_ICON: 0);
	}
}

void CXTPTaskDialog::SetDefaultButton(int nButtonID)
{
	m_config.nDefaultButton = nButtonID;
}

void CXTPTaskDialog::SetDefaultRadioButton(int nButtonID)
{
	m_config.nDefaultRadioButton = nButtonID;
	NoDefaultRadioButton(nButtonID == 0);
}

void CXTPTaskDialog::NoDefaultRadioButton(BOOL bNoDefault)
{
	ModifyTaskStyle(TDF_NO_DEFAULT_RADIO_BUTTON, bNoDefault? TDF_NO_DEFAULT_RADIO_BUTTON: 0);

	if (bNoDefault)
		m_config.nDefaultRadioButton = 0;
}

void CXTPTaskDialog::SetVerifyCheckState(BOOL bChecked)
{
	ModifyTaskStyle(TDF_VERIFICATION_FLAG_CHECKED, bChecked? TDF_VERIFICATION_FLAG_CHECKED: 0);
}

void CXTPTaskDialog::EnableCallbackTimer(BOOL bEnable)
{
	ModifyTaskStyle(TDF_CALLBACK_TIMER, bEnable? TDF_CALLBACK_TIMER: 0);
}

void CXTPTaskDialog::EnableMinimize(BOOL bEnable)
{
	ModifyTaskStyle(TDF_CAN_BE_MINIMIZED, bEnable? TDF_CAN_BE_MINIMIZED: 0);
}

AFX_INLINE CString GetPropExchangeString(CXTPPropExchange* pPX, LPCTSTR lpsz, LPCTSTR const* rglpsz, int nString)
{
	CString str;

	if (lpsz)
	{
		CXTPPropExchangeSection px(pPX->GetSection(lpsz));
		PX_String(&px, NULL, str);
	}
	else
	{
		PX_String(pPX, NULL, str);
	}
	if (nString > 0 && FIND_S(str, _T('%'), 0) != -1)
	{
		CString strFormat = str;
		AfxFormatStrings(str, strFormat, rglpsz, nString);
	}
	return str;
}

AFX_INLINE int GetPropExchangeInt(CXTPPropExchange* pPX, LPCTSTR lpsz)
{
	CString str;
	PX_String(pPX, lpsz, str);
	return _ttoi(str);
}

AFX_INLINE BOOL GetPropExchangeFlag(CXTPPropExchange* pPX, LPCTSTR lpsz)
{
	CString str;
	PX_String(pPX, lpsz, str);
	return str.CompareNoCase(_T("True")) == 0 || str.CompareNoCase(_T("1")) == 0;
}

AFX_INLINE LPCWSTR GetPropExchangeIcon(CXTPPropExchange* pPX, LPCTSTR lpsz)
{
	CString str;
	CXTPPropExchangeSection px(pPX->GetSection(lpsz));
	PX_String(&px, _T("Image"), str);

	if (str.IsEmpty())
		return NULL;

	if (str.CompareNoCase(_T("*Information")) == 0)
		return TD_INFORMATION_ICON;
	if (str.CompareNoCase(_T("*Error")) == 0)
		return TD_ERROR_ICON;
	if (str.CompareNoCase(_T("*Shield")) == 0)
		return TD_SHIELD_ICON;
	if (str.CompareNoCase(_T("*Warning")) == 0)
		return TD_WARNING_ICON;

	return NULL;
}

BOOL CXTPTaskDialog::CreateFromResource(UINT nIDResource, LPCTSTR lpszDialog)
{
	return CreateFromResourceParam(AfxGetInstanceHandle(), nIDResource, lpszDialog, NULL, 0);
}

BOOL CXTPTaskDialog::CreateFromResource(HINSTANCE hInstance, UINT nIDResource, LPCTSTR lpszDialog)
{
	return CreateFromResourceParam(hInstance, nIDResource, lpszDialog, NULL, 0);
}

BOOL CXTPTaskDialog::CreateFromResourceParam(HINSTANCE hInstance, UINT nIDResource, LPCTSTR lpszDialog, LPCTSTR const* rglpsz, int nString)
{
	CXTPPropExchangeXMLNode px(TRUE, NULL, _T("Dialogs"));

	if (!px.LoadFromResource(hInstance, MAKEINTRESOURCE(nIDResource), RT_HTML))
		return FALSE;

	if (!px.OnBeforeExchange())
		return FALSE;

	px.SetCompactMode(TRUE);

	CString strDialogSection;
	strDialogSection.Format(_T("Dialog[@Name='%s']"), lpszDialog);
	if (!px.IsSectionExists(strDialogSection))
		return FALSE;

	CXTPPropExchangeSection pxDialog(px.GetSection(strDialogSection));
	if (!pxDialog->OnBeforeExchange())
		return FALSE;

	return CreateFromResourceParam((CXTPPropExchangeXMLNode*)&pxDialog, rglpsz, nString);
}

BOOL CXTPTaskDialog::CreateFromResourceParam(CXTPPropExchangeXMLNode* pPX, LPCTSTR const* rglpsz, int nString)
{
	if (!pPX)
		return FALSE;

	EnableHyperlinks(GetPropExchangeFlag(pPX, _T("EnableHyperlinks")));
	EnableCancellation(GetPropExchangeFlag(pPX, _T("AllowCancel")));
	ShowProgressBar(GetPropExchangeFlag(pPX, _T("ProgressBar")), FALSE);
	SetWidth(GetPropExchangeInt(pPX, _T("Width")), TRUE);
	EnableRelativePosition(GetPropExchangeFlag(pPX, _T("RelativeToWindow")));
	EnableRtlLayout(GetPropExchangeFlag(pPX, _T("RTLLayout")));
	NoDefaultRadioButton(GetPropExchangeFlag(pPX, _T("NoDefaultRadioButton")));
	EnableMessageBoxStyle(GetPropExchangeFlag(pPX, _T("MessageBoxStyle")));

	SetWindowTitle(GetPropExchangeString(pPX, _T("WindowTitle"), rglpsz, nString));
	SetMainInstruction(GetPropExchangeString(pPX, _T("MainInstruction"), rglpsz, nString));
	SetContent(GetPropExchangeString(pPX, _T("Content"), rglpsz, nString));
	SetFooter(GetPropExchangeString(pPX, _T("Footer"), rglpsz, nString));

	SetMainIcon(GetPropExchangeIcon(pPX, _T("MainInstruction")));
	SetFooterIcon(GetPropExchangeIcon(pPX, _T("Footer")));

	CXTPPropExchangeSection pxButtons(pPX->GetSection(_T("Buttons")));
	CXTPPropExchangeEnumeratorPtr enumButton(pxButtons->GetEnumerator(NULL));

	POSITION pos = enumButton->GetPosition(0);
	while (pos)
	{
		CXTPPropExchangeSection secButton(enumButton->GetNext(pos));

		CString strSection = ((CXTPPropExchangeXMLNode*)&secButton)->GetSectionName();

		int nID = GetPropExchangeInt(&secButton, _T("ID"));

		if (strSection.CompareNoCase(_T("Button")) == 0 && (nID != 0))
		{
			AddButton(GetPropExchangeString(&secButton, NULL, NULL, 0), nID);
		}
		else if (strSection.CompareNoCase(_T("CommandLink")) == 0 && (nID != 0))
		{
			CString strText = GetPropExchangeString(&secButton, _T("Text"), rglpsz, nString);
			CString strExplanation = GetPropExchangeString(&secButton, _T("Explanation"), rglpsz, nString);

			if (!strExplanation.IsEmpty()) strText += _T("\n") + strExplanation;

			AddButton(strText, nID);

			EnableCommandLinks(TRUE, !GetPropExchangeFlag(pPX, _T("NoCommandLinkGlyph")));
		}
		else if (strSection.CompareNoCase(_T("Ok")) == 0)
		{
			m_config.dwCommonButtons |= TDCBF_OK_BUTTON;
		}
		else if (strSection.CompareNoCase(_T("Yes")) == 0)
		{
			m_config.dwCommonButtons |= TDCBF_YES_BUTTON;
		}
		else if (strSection.CompareNoCase(_T("No")) == 0)
		{
			m_config.dwCommonButtons |= TDCBF_NO_BUTTON;
		}
		else if (strSection.CompareNoCase(_T("Cancel")) == 0)
		{
			m_config.dwCommonButtons |= TDCBF_CANCEL_BUTTON;
		}
		else if (strSection.CompareNoCase(_T("Retry")) == 0)
		{
			m_config.dwCommonButtons |= TDCBF_RETRY_BUTTON;
		}
		else if (strSection.CompareNoCase(_T("Close")) == 0)
		{
			m_config.dwCommonButtons |= TDCBF_CLOSE_BUTTON;
		}
	}
	SetDefaultButton(GetPropExchangeInt(&pxButtons, _T("Default")));


	CXTPPropExchangeSection pxRadioButtons(pPX->GetSection(_T("RadioButtons")));
	CXTPPropExchangeEnumeratorPtr enumRadioButton(pxRadioButtons->GetEnumerator(_T("RadioButton")));

	pos = enumRadioButton->GetPosition(0);
	while (pos)
	{
		CXTPPropExchangeSection secButton(enumRadioButton->GetNext(pos));

		int nID = GetPropExchangeInt(&secButton, _T("ID"));

		if (nID != 0)
		{
			AddRadioButton(GetPropExchangeString(&secButton, NULL, NULL, 0), nID);
		}
	}
	SetDefaultRadioButton(GetPropExchangeInt(&pxRadioButtons, _T("Default")));

	if (pPX->IsSectionExists(_T("ExpandedInformation")))
	{
		CXTPPropExchangeSection pxExpandedInformation(pPX->GetSection(_T("ExpandedInformation")));

		SetExpandedInformation(GetPropExchangeString(&pxExpandedInformation, NULL, rglpsz, nString));
		ExpandFooterArea(GetPropExchangeFlag(&pxExpandedInformation, _T("Footer")));
		ExpandedByDefault(GetPropExchangeFlag(&pxExpandedInformation, _T("Expanded")));

		SetExpandedControlText(GetPropExchangeString(pPX, _T("ExpandedText"), rglpsz, nString));
		SetCollapsedControlText(GetPropExchangeString(pPX, _T("CollapsedText"), rglpsz, nString));
	}

	if (pPX->IsSectionExists(_T("Verification")))
	{
		CXTPPropExchangeSection pxVerification(pPX->GetSection(_T("Verification")));

		SetVerificationText(GetPropExchangeString(&pxVerification, NULL, rglpsz, nString));
		SetVerifyCheckState(GetPropExchangeFlag(&pxVerification, _T("Checked")));
	}

	return TRUE;
}
