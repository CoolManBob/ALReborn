// XTPTaskDialogClient.cpp: implementation of the CXTPTaskDialogClient class.
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
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPResourceManager.h"

#include "Controls/XTThemeManager.h"
#include "Controls/XTButtonTheme.h"
#include "Controls/XTButton.h"

#include "XTPTaskDialogAPI.h"
#include "XTPTaskDialogClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// dialog units used for dynamic layout.
//---------------------------------------------------------------------------
enum XTPDluMetrics
{
	xtpDluMinClient        = 180, // minimum client width.
	xtpDluMaxClient        = 265, // maximum client width when using command links.
	xtpDluMaxVerify        = 146, // maximum width for verify and expando buttons.
	xtpDluPadding          = 2,   // padding used for button and content.
	xtpDluBtnSpacing       = 3,   // spacing between common buttons.
	xtpDluMargin           = 5,   // non-client margin.
};


#ifndef LAYOUT_BITMAPORIENTATIONPRESERVED
#define LAYOUT_BITMAPORIENTATIONPRESERVED 0x00000008
#endif

//===========================================================================
// CXTPTaskDialogClient
//===========================================================================
CXTPTaskDialogClient::CXTPTaskDialogClient()
	: m_nSection(xtpNone)
	, m_nMaxClientWidth(0)
	, m_nMinClientWidth(0)
	, m_nPadding(2)
	, m_bExpanded(FALSE)
	, m_bVerification(FALSE)
	, m_bUseSysIcons(FALSE)
	, m_bThemeReady(FALSE)
	, m_hIconFooter(NULL)
	, m_hIconMain(NULL)
	, m_crContext(COLORREF_NULL)
	, m_crContextText(COLORREF_NULL)
	, m_crFooter(COLORREF_NULL)
	, m_crFooterText(COLORREF_NULL)
	, m_crMainText(COLORREF_NULL)
	, m_cr3DLight(COLORREF_NULL)
	, m_cr3DShadow(COLORREF_NULL)
	, m_cr3DHighLight(COLORREF_NULL)
	, m_ptMargin(0,0)
	, m_ptPadding(0,0)
	, m_ptBtnSpacing(0,0)
	, m_ptBorder(0,0)
	, m_pBtnVerify(NULL)
	, m_pBtnExpando(NULL)
	, m_pConfig(NULL)
{
	m_rcMainInstruction.SetRectEmpty();
	m_rcContent.SetRectEmpty();
	m_rcCommandButtons.SetRectEmpty();
	m_rcFooter.SetRectEmpty();
	m_rcExpandedInformation.SetRectEmpty();
	m_rcLinkButtons.SetRectEmpty();
	m_rcRadioButtons.SetRectEmpty();

	m_sizeCommandButton = CSize(0, 0);
	m_sizeIconFooter = CSize(0, 0);
	m_sizeIconMain = CSize(0, 0);

	m_nSelRadioButtonID = 0;

	m_pIconLinkGlyph = NULL;
	m_bMessageBoxStyle = FALSE;
}

void CXTPTaskDialogClient::DeleteAllButtons()
{
	SAFE_DELETE(m_pBtnVerify);
	SAFE_DELETE(m_pBtnExpando);

	CMDTARGET_RELEASE(m_pIconLinkGlyph);

	int i;
	for (i = 0; i < m_arrLinkButtons.GetSize(); ++i)
	{
		CButton* pButton = m_arrLinkButtons[i];
		SAFE_DELETE(pButton);
	}
	m_arrLinkButtons.RemoveAll();

	for (i = 0; i < m_arrRadioButtons.GetSize(); ++i)
	{
		CButton* pButton = m_arrRadioButtons[i];
		SAFE_DELETE(pButton);
	}
	m_arrRadioButtons.RemoveAll();

	for (i = 0; i < m_arrCommandButtons.GetSize(); ++i)
	{
		CButton* pButton = m_arrCommandButtons[i];
		SAFE_DELETE(pButton);
	}
	m_arrCommandButtons.RemoveAll();

	SAFE_DELETE_HICON(m_hIconFooter);
	SAFE_DELETE_HICON(m_hIconMain);

	CMDTARGET_RELEASE(m_pIconLinkGlyph);

	m_mapWndSize.RemoveAll();
}

CXTPTaskDialogClient::~CXTPTaskDialogClient()
{
	DeleteAllButtons();
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CXTPTaskDialogClient, CWnd)
	//{{AFX_MSG_MAP(CXTPTaskDialogClient)
	ON_WM_SETTINGCHANGE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
// CXTPTaskDialogClient message handlers
//---------------------------------------------------------------------------

void CXTPTaskDialogClient::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDialog::OnSettingChange(uFlags, lpszSection);
	RefreshMetrics();
}

void CXTPTaskDialogClient::OnSysColorChange()
{
	CDialog::OnSysColorChange();
	RefreshColors();
}

BOOL CXTPTaskDialogClient::OnEraseBkgnd(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

void CXTPTaskDialogClient::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CXTPBufferDC memDC(dc);

	CXTPClientRect rcClient(this);

	if (!memDC.GetSafeHdc())
		return;

	DrawBackground(&memDC, rcClient);
	memDC.SetBkMode(TRANSPARENT);


	DrawMainInstruction(&memDC);
	DrawContent(&memDC);
	DrawExpandedInfo(&memDC);
	DrawFootNote(&memDC);
}

HBRUSH CXTPTaskDialogClient::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	int i;

	for (i = 0; i < m_arrRadioButtons.GetSize(); i++)
	{
		if (m_arrRadioButtons[i] == pWnd)
		{
			pDC->SetBkColor(m_crContext);
			return GetSysColorBrush(m_bThemeReady ? COLOR_WINDOW : COLOR_3DFACE);
		}
	}

	for (i = 0; i < m_arrLinkButtons.GetSize(); i++)
	{
		if (m_arrLinkButtons[i] == pWnd)
		{
			pDC->SetBkColor(m_crContext);
			return GetSysColorBrush(m_bThemeReady ? COLOR_WINDOW : COLOR_3DFACE);
		}
	}


	return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

}

HRESULT CXTPTaskDialogClient::SendNotify(UINT uNotify, WPARAM wParam /*=0*/, LPARAM lParam /*=0*/)
{
	if (m_pConfig->pfCallback)
	{
		return m_pConfig->pfCallback(m_hWnd, uNotify,
			wParam, lParam, m_pConfig->lpCallbackData);
	}
	return E_FAIL;
}

BOOL CXTPTaskDialogClient::CanCancel() const
{
	if (GetDlgItem(IDCANCEL))
		return TRUE;

	return ((m_pConfig->dwFlags & TDF_ALLOW_DIALOG_CANCELLATION) != 0);
}

LRESULT CXTPTaskDialogClient::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED)
			{
				int nID = (int)LOWORD(wParam);
				HWND hWnd = (HWND)lParam;

				if (hWnd && m_arrRadioButtons.GetSize() > 0)
				{
					for (int i = 0; i < m_arrRadioButtons.GetSize(); i++)
					{
						if (m_arrRadioButtons[i]->GetSafeHwnd() == hWnd )
						{
							if (m_arrRadioButtons[i]->GetCheck())
							{
								m_nSelRadioButtonID = nID;
								SendNotify(TDN_RADIO_BUTTON_CLICKED, (WPARAM)m_nSelRadioButtonID);
							}
							return TRUE;
						}
					}
				}


				if (hWnd && (hWnd == m_wndFooter.GetSafeHwnd()
					|| hWnd == m_wndContent.GetSafeHwnd()
					|| hWnd == m_wndExpandedInformation.GetSafeHwnd()))
				{
					CXTPTaskDialogLinkCtrl* pWnd = (CXTPTaskDialogLinkCtrl*)CWnd::FromHandle(hWnd);
					CXTPTaskDialogLinkCtrl::LINKITEM* pItem = pWnd->GetFocusedLink();
					if (pItem)
					{
						USES_CONVERSION;
						PCWSTR pszURL = T2CW(pItem->strUrl);
						SendNotify(TDN_HYPERLINK_CLICKED, (WPARAM)0, (LPARAM)pszURL);

						return 1;
					}
				}

				switch (nID)
				{
				case XTP_IDC_VERIFY:
					if (m_pBtnVerify && m_pBtnVerify->GetSafeHwnd() == hWnd)
					{
						m_bVerification = !m_bVerification;
						SendNotify(TDN_VERIFICATION_CLICKED, m_bVerification);
						return 0;
					}

				case XTP_IDC_EXPANDO:
					if (m_pBtnExpando && m_pBtnExpando->GetSafeHwnd() == hWnd)
					{
						m_bExpanded = !m_bExpanded;
						SendNotify(TDN_EXPANDO_BUTTON_CLICKED, m_bExpanded);
						OnExpandExpandoClicked();
						return 0;
					}

				case IDCANCEL:
					if (!CanCancel())
						return FALSE;
					// Fall thru

				default:
					if (SendNotify(TDN_BUTTON_CLICKED, (WPARAM)nID) == 0)
					{
						if (nID == IDCANCEL)
						{
							OnCancel();
						}
						else if (nID == IDOK)
						{
							OnOK();
						}
						else
						{
							EndDialog(nID);
						}
					}
					return 0;
				}
			}
			break;
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


//---------------------------------------------------------------------------
// Initialization
//---------------------------------------------------------------------------

void CXTPTaskDialogClient::RefreshMetrics()
{
	CXTPWinThemeWrapper themeApi;
	m_bThemeReady = m_bMessageBoxStyle || themeApi.IsAppThemeReady();

	m_ptBorder.x      = ::GetSystemMetrics(SM_CXEDGE) + ::GetSystemMetrics(SM_CXBORDER);
	m_ptBorder.y      = ::GetSystemMetrics(SM_CYEDGE) + ::GetSystemMetrics(SM_CYBORDER);
	m_sizeIconFooter.cx     = ::GetSystemMetrics(SM_CXSMICON);
	m_sizeIconFooter.cy     = ::GetSystemMetrics(SM_CYSMICON);
	m_sizeIconMain.cx = ::GetSystemMetrics(SM_CXICON);
	m_sizeIconMain.cy = ::GetSystemMetrics(SM_CYICON);

	m_ptMargin     = XTPDlu2Pix(xtpDluMargin, xtpDluMargin);
	m_ptPadding    = XTPDlu2Pix(xtpDluPadding, xtpDluPadding);

	m_ptBtnSpacing = CPoint(XTPDlu2Pix(xtpDluBtnSpacing, xtpDluBtnSpacing).x, 0);

	if (!m_bThemeReady)
		m_ptBtnSpacing = CPoint(m_ptMargin.x, 2);

	m_nMinClientWidth = XTPDlu2Pix(xtpDluMinClient, 0).x - m_ptMargin.x * 2;


	CRect rcScreen = XTPMultiMonitor()->GetWorkArea(m_pConfig->hwndParent);
	m_nMaxClientWidth = rcScreen.Width() - ((m_ptBorder.x*2) + (m_ptMargin.x*2));

	if (m_nMinClientWidth > m_nMaxClientWidth)
		m_nMinClientWidth = m_nMaxClientWidth;
}

void CXTPTaskDialogClient::RefreshColors()
{
	COLORREF crWindow = GetXtremeColor(COLOR_WINDOW);
	COLORREF crWindowText = GetXtremeColor(COLOR_WINDOWTEXT);
	COLORREF crBtnFace = GetXtremeColor(COLOR_BTNFACE);
	COLORREF crBtnText = GetXtremeColor(COLOR_BTNTEXT);
	m_cr3DShadow = GetXtremeColor(COLOR_3DSHADOW);
	m_cr3DHighLight = GetXtremeColor(COLOR_3DHILIGHT);
	m_cr3DLight  = XTPColorManager()->LightColor(crBtnFace, m_cr3DShadow, 750);

	m_crContext = m_bThemeReady? crWindow : crBtnFace;
	m_crContextText = m_bThemeReady? crWindowText : crBtnText;
	m_crMainText = m_bThemeReady ? RGB(0x00, 0x33, 0x99) : m_crContextText;
	m_crFooter = crBtnFace;
	m_crFooterText = crBtnText;

	m_wndContent.SetBackColor(m_crContext);
	m_wndContent.SetTextColor(m_crContextText);
	m_wndExpandedInformation.SetBackColor(IsExpandFooterArea() ? m_crFooter : m_crContext);
	m_wndExpandedInformation.SetTextColor(IsExpandFooterArea() ? m_crFooterText : m_crContextText);
	m_wndFooter.SetBackColor(m_crFooter);
	m_wndFooter.SetTextColor(m_crFooterText);
}

BOOL CXTPTaskDialogClient::LoadItemString(CString& strItem, PCWSTR pszBuffer)
{
	strItem.Empty();

	if (pszBuffer && IS_INTRESOURCE(pszBuffer))
	{
		UINT uID = (UINT)(UINT_PTR)pszBuffer;

		if (::FindResource(m_pConfig->hInstance, MAKEINTRESOURCE((uID>>4)+1), RT_STRING))
			XTPLoadStringInst(m_pConfig->hInstance, uID, &strItem);
		else
			XTPResourceManager()->LoadString(&strItem, uID);

		ASSERT(!strItem.IsEmpty());
	}
	else
	{
		strItem = pszBuffer;
	}

	return (!strItem.IsEmpty());
}

void CXTPTaskDialogClient::LoadStrings()
{
	LoadItemString(m_strMainInstruction, m_pConfig->pszMainInstruction);
	LoadItemString(m_strContent, m_pConfig->pszContent);
	LoadItemString(m_strFooter, m_pConfig->pszFooter);
	LoadItemString(m_strVerificationText, m_pConfig->pszVerificationText);
	LoadItemString(m_strExpandedInformation, m_pConfig->pszExpandedInformation);

	if (!m_strExpandedInformation.IsEmpty())
	{
		LoadItemString(m_strExpandedControlText, m_pConfig->pszExpandedControlText);
		LoadItemString(m_strCollapsedControlText, m_pConfig->pszCollapsedControlText);

		if (m_strExpandedControlText.IsEmpty() && m_strCollapsedControlText.IsEmpty())
		{
			m_strExpandedControlText.LoadString(XTP_IDS_TASK_HIDE_DETAILS);
			m_strCollapsedControlText.LoadString(XTP_IDS_TASK_SEE_DETAILS);
		}
		else if (m_strExpandedControlText.IsEmpty())
		{
			m_strExpandedControlText = m_strCollapsedControlText;
		}
		else if (m_strCollapsedControlText.IsEmpty())
		{
			m_strCollapsedControlText = m_strExpandedControlText;
		}
	}
}

BOOL CXTPTaskDialogClient::CreateCommandButtons()
{
	if ((m_pConfig->dwCommonButtons & TDCBF_OK_BUTTON) || (!m_pConfig->pButtons && !m_pConfig->dwCommonButtons))
	{
		if (AddCommandButton(IDOK, XTP_IDS_TASK_OK, m_sizeCommandButton) == NULL)
		{
			TRACE0("Failed to create OK button.\n");
			return FALSE;
		}
	}

	if (m_pConfig->dwCommonButtons & TDCBF_YES_BUTTON)
	{
		if (AddCommandButton(IDYES, XTP_IDS_TASK_YES, m_sizeCommandButton) == NULL)
		{
			TRACE0("Failed to create YES button.\n");
			return FALSE;
		}
	}

	if (m_pConfig->dwCommonButtons & TDCBF_NO_BUTTON)
	{
		if (AddCommandButton(IDNO, XTP_IDS_TASK_NO, m_sizeCommandButton) == NULL)
		{
			TRACE0("Failed to create NO button.\n");
			return FALSE;
		}
	}

	if (m_pConfig->dwCommonButtons & TDCBF_RETRY_BUTTON)
	{
		if (AddCommandButton(IDRETRY, XTP_IDS_TASK_RETRY, m_sizeCommandButton) == NULL)
		{
			TRACE0("Failed to create RETRY button.\n");
			return FALSE;
		}
	}

	if (m_pConfig->dwCommonButtons & TDCBF_CANCEL_BUTTON)
	{
		if (AddCommandButton(IDCANCEL, XTP_IDS_TASK_CANCEL, m_sizeCommandButton) == NULL)
		{
			TRACE0("Failed to create CANCEL button.\n");
			return FALSE;
		}
	}

	if (m_pConfig->dwCommonButtons & TDCBF_CLOSE_BUTTON)
	{
		if (AddCommandButton(IDCLOSE, XTP_IDS_TASK_CLOSE, m_sizeCommandButton) == NULL)
		{
			TRACE0("Failed to create CLOSE button.\n");
			return FALSE;
		}
	}


	return TRUE;
}

BOOL CXTPTaskDialogClient::CreateLinkButtons()
{
	if (m_pConfig->pButtons == NULL || m_pConfig->cButtons == 0)
		return TRUE;

	for (UINT i = 0; i < m_pConfig->cButtons; ++i)
	{
		CString strButtonText;
		LoadItemString(strButtonText, m_pConfig->pButtons[i].pszButtonText);

		if (IsLinkButtonsUsed())
		{
			CXTPTaskButtonTheme* pTheme = new CXTPTaskButtonTheme(m_bThemeReady);
			pTheme->SetTitleFont(&m_fontLarge);

			CButton* pButton = CreateButton(m_pConfig->pButtons[i].nButtonID,
				strButtonText, CSize(0, 0), BS_PUSHBUTTON | BS_MULTILINE | WS_TABSTOP, pTheme);

			if (pButton == NULL)
			{
				TRACE0("Error creating command link button.\n");
				return FALSE;
			}

			if (m_pIconLinkGlyph)
			{
				CXTButton* pXTButton = DYNAMIC_DOWNCAST(CXTButton, pButton);
				if (pXTButton == NULL)
				{
					TRACE0("Error creating command link button.\n");
					SAFE_DELETE(pButton);
					return FALSE;
				}

				CSize sizeGlyph = m_pIconLinkGlyph->GetExtent();

				m_pIconLinkGlyph->InternalAddRef(); // Lets share one icon!
				pXTButton->SetIcon(sizeGlyph, m_pIconLinkGlyph);
				pXTButton->SetImageGap(sizeGlyph.cx / 2);
			}
			m_arrLinkButtons.Add(pButton);
		}
		else
		{
			CSize sizeButton = CalcButtonSize(strButtonText);
			sizeButton.cx = max(m_sizeCommandButton.cx, sizeButton.cx);

			if (AddCommandButton(m_pConfig->pButtons[i].nButtonID, strButtonText, sizeButton) == NULL)
			{
				TRACE0("Failed to create OK button.\n");
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CXTPTaskDialogClient::CreateProgressBar()
{
	if (!IsProgressBarVisible())
		return TRUE;

	if (!m_wndProgressBar.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 0))
		return FALSE;

	return TRUE;
}

BOOL CXTPTaskDialogClient::CreateRadioButtons()
{
	if (m_pConfig->pRadioButtons == NULL)
		return TRUE;

	for (UINT i = 0; i < m_pConfig->cRadioButtons; ++i)
	{
		CString strButton;
		LoadItemString(strButton, m_pConfig->pRadioButtons[i].pszButtonText);

		int nButtonID = m_pConfig->pRadioButtons[i].nButtonID;

		CPoint point(10,2);
		CButton* pButton = CreateButton(nButtonID, strButton,
			CalcButtonSize(strButton, &point), BS_AUTORADIOBUTTON | BS_TEXT | BS_MULTILINE);

		if (pButton == NULL)
		{
			TRACE0("Error creating radio button.\n");
			return FALSE;
		}

		if ((m_pConfig->dwFlags & TDF_NO_DEFAULT_RADIO_BUTTON) == 0)
		{
			if (m_pConfig->nDefaultRadioButton == nButtonID)
			{
				pButton->SetCheck(TRUE);
				m_nSelRadioButtonID = nButtonID;
				pButton->ModifyStyle(0, WS_TABSTOP);
			}
			if (m_pConfig->nDefaultRadioButton == 0 && i == 0)
			{
				pButton->SetCheck(TRUE);
				m_nSelRadioButtonID = nButtonID;
				pButton->ModifyStyle(0, WS_TABSTOP);
			}
		}
		else if (i == 0)
		{
			pButton->ModifyStyle(0, WS_TABSTOP);
		}

		m_arrRadioButtons.Add(pButton);
	}

	return TRUE;
}

BOOL CXTPTaskDialogClient::CreateVerifyExpandButtons()
{
	// create expando button.
	if (!m_strExpandedInformation.IsEmpty())
	{
		CString strButtonText = IsExpandedByDefault()? m_strExpandedControlText: m_strCollapsedControlText;

		CString strButtonSize = m_strExpandedControlText.GetLength() > m_strCollapsedControlText.GetLength()
			? m_strExpandedControlText : m_strCollapsedControlText;

		CSize sizeButton = CalcButtonSize(strButtonSize, NULL, XTPDlu2Pix(xtpDluMaxVerify,0).x);

		CButton* pButton = CreateButton(XTP_IDC_EXPANDO, strButtonText, sizeButton,
			BS_PUSHBUTTON | BS_MULTILINE | WS_TABSTOP, new CXTPTaskButtonThemeExpando);

		if (pButton == NULL)
		{
			TRACE0("Error creating Expando button.\n");
			return FALSE;
		}

		m_pBtnExpando = DYNAMIC_DOWNCAST(CXTButton, pButton);
		if (m_pBtnExpando == NULL)
		{
			TRACE0("Error creating Expando button.\n");
			SAFE_DELETE(pButton);
			return FALSE;
		}

		m_pBtnExpando->SetXButtonStyle(BS_XT_WINXP_COMPAT);

		m_bExpanded = IsExpandedByDefault();
	}

	// create verification text check box.
	if (!m_strVerificationText.IsEmpty())
	{
		CString strButtonText = m_strVerificationText;
		CPoint point(10, 2);
		CSize sizeButton = CalcButtonSize(strButtonText, &point, XTPDlu2Pix(xtpDluMaxVerify,0).x);

		CButton* pButton = CreateButton(XTP_IDC_VERIFY, strButtonText, sizeButton,
			BS_AUTOCHECKBOX | BS_MULTILINE | BS_TOP | BS_LEFT | WS_TABSTOP);

		if (pButton == NULL)
		{
			TRACE0("Error creating Verification check box.\n");
			return FALSE;
		}

		m_bVerification = ((m_pConfig->dwFlags & TDF_VERIFICATION_FLAG_CHECKED) != 0);
		m_pBtnVerify = pButton;
		m_pBtnVerify->SetCheck(m_bVerification);
	}

	return TRUE;
}

BOOL CXTPTaskDialogClient::CreateClient()
{
	DeleteAllButtons();

	RefreshMetrics();
	RefreshColors();

	LoadStrings();
	CreateIcons();
	CreateFonts();

	m_sizeCommandButton = CalcCommandButtonSize();

	if (!CreateLinkButtons() ||         // create command link buttons.
		!CreateCommandButtons())        // create common buttons OK, Cancel, etc.
	{
		return FALSE;
	}

	// If hyperlinks are enabled, creating the links now with a zero size
	// will extract all hyperlink information from the string so the layout
	// can be determined correctly. The links will be resized when RecalcLayout
	// is called.

	if (IsHyperlinksEnabled())
	{
		CXTPEmptyRect rect;
		m_wndContent.Create(rect, m_strContent, &m_font, this);
		m_wndExpandedInformation.Create(rect, m_strExpandedInformation, &m_font, this);
		m_wndFooter.Create(rect, m_strFooter, &m_font, this);
	}

	if (!CreateProgressBar() ||
		!CreateRadioButtons() ||        // create radio buttons.
		!CreateVerifyExpandButtons())   // create verify and expando buttons.
	{
		return FALSE;
	}


	RecalcLayout(TRUE);

	return TRUE;
}

void CXTPTaskDialogClient::UpdateZOrder()
{
	ZOrder(&m_wndContent);

	if (!IsExpandFooterArea())
		ZOrder(&m_wndExpandedInformation);

	ZOrder(&m_wndProgressBar);

	int i;

	for (i = 0; i < (int)m_arrRadioButtons.GetSize(); i++)
	{
		ZOrder(m_arrRadioButtons[i]);
	}

	for (i = 0; i < (int)m_arrLinkButtons.GetSize(); i++)
	{
		ZOrder(m_arrLinkButtons[i]);
	}

	ZOrder(m_pBtnExpando);
	ZOrder(m_pBtnVerify);

	for (i = 0; i < (int)m_arrCommandButtons.GetSize(); i++)
	{
		ZOrder(m_arrCommandButtons[i]);
	}

	ZOrder(&m_wndFooter);

	if (IsExpandFooterArea())
		ZOrder(&m_wndExpandedInformation);

}


void CXTPTaskDialogClient::ZOrder(CWnd* pWnd)
{
	if (!pWnd || !pWnd->GetSafeHwnd() )
		return;

	pWnd->SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
}

HICON CXTPTaskDialogClient::CreateIcon(HICON hIcon, PCWSTR lpwzIcon, CSize sizeIcon, BOOL bUseHandle)
{
	HICON hNewIcon = NULL;

	if (bUseHandle && hIcon != NULL)
	{
		hNewIcon = ::CopyIcon(hIcon);
	}
	else if (!bUseHandle)
	{
		if (m_pConfig->hInstance == NULL || lpwzIcon == NULL || !IS_INTRESOURCE(lpwzIcon))
		{
			return FALSE;
		}

		if (lpwzIcon == TD_ERROR_ICON ||
			lpwzIcon == TD_WARNING_ICON ||
			lpwzIcon == TD_INFORMATION_ICON ||
			lpwzIcon == TD_SHIELD_ICON)
		{
			DWORD dwID = 0;

			if (m_bUseSysIcons)
			{
				switch ((ULONG_PTR)lpwzIcon)
				{
					case (ULONG_PTR)TD_ERROR_ICON:       dwID = 0x0067; break;
					case (ULONG_PTR)TD_WARNING_ICON:     dwID = 0x0065; break;
					case (ULONG_PTR)TD_INFORMATION_ICON: dwID = 0x0068; break;
					case (ULONG_PTR)TD_SHIELD_ICON:      dwID = 0x006a; break; // vista and later.
				}

				hNewIcon = XTPResourceManager()->CreateIconFromResource(
					CXTPModuleHandle(_T("user32.dll")), MAKEINTRESOURCE(dwID), sizeIcon);
			}

			if (hNewIcon == NULL)
			{
				switch ((ULONG_PTR)lpwzIcon)
				{
					case (ULONG_PTR)TD_ERROR_ICON:       dwID = XTP_IDI_TASKERROR;  break;
					case (ULONG_PTR)TD_WARNING_ICON:     dwID = XTP_IDI_TASKWARN;   break;
					case (ULONG_PTR)TD_INFORMATION_ICON: dwID = XTP_IDI_TASKINFO;   break;
					case (ULONG_PTR)TD_SHIELD_ICON:      dwID = XTP_IDI_TASKSHIELD; break;
				}

				hNewIcon = XTPResourceManager()->LoadIcon(MAKEINTRESOURCE(dwID), sizeIcon);
			}
		}
		else
		{
			hNewIcon = XTPResourceManager()->CreateIconFromResource(
				m_pConfig->hInstance, (LPTSTR)lpwzIcon, sizeIcon);
		}
	}

	ASSERT(hNewIcon != NULL);
	return hNewIcon;
}

void CXTPTaskDialogClient::CreateIcons()
{
	// create small icon.
	SAFE_DELETE_HICON(m_hIconFooter);
	m_hIconFooter = CreateIcon(m_pConfig->hFooterIcon, m_pConfig->pszFooterIcon, m_sizeIconFooter, IsFooterIconHandleUsed());

	// create large icon.
	SAFE_DELETE_HICON(m_hIconMain);
	m_hIconMain = CreateIcon(m_pConfig->hMainIcon, m_pConfig->pszMainIcon, m_sizeIconMain, IsMainIconHandleUsed());

	if (IsLinkButtonsUsed() && IsLinkButtonsIconVisible() && m_pIconLinkGlyph == 0)
	{
		m_pIconLinkGlyph = new CXTPImageManagerIcon(0, 20, 20);

		BOOL bRTLLayout = (m_pConfig->dwFlags & TDF_RTL_LAYOUT);

		CXTPImageManagerIconHandle hIconGlyph;
		CXTPImageManagerIconHandle hIconGlyphHot;

		HBITMAP hBitmap = (HBITMAP)CXTPImageManagerIcon::LoadAlphaBitmap(XTP_IDB_GLYPH);
		if (bRTLLayout)
		{
			hIconGlyph = CXTPImageManagerIcon::InvertAlphaBitmap(hBitmap);
			DeleteObject(hBitmap);
		}
		else
		{
			hIconGlyph = hBitmap;
		}

		hBitmap = (HBITMAP)CXTPImageManagerIcon::LoadAlphaBitmap(XTP_IDB_GLYPH_HOT);

		if (bRTLLayout)
		{
			hIconGlyphHot = CXTPImageManagerIcon::InvertAlphaBitmap(hBitmap);
			DeleteObject(hBitmap);
		}
		else
		{
			hIconGlyphHot = hBitmap;
		}

		m_pIconLinkGlyph->SetIcon(hIconGlyph);
		m_pIconLinkGlyph->SetHotIcon(hIconGlyphHot);
	}
}

void CXTPTaskDialogClient::CreateFonts()
{
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	m_font.DeleteObject();
	m_fontLarge.DeleteObject();

	if (m_bThemeReady)
	{
		CWindowDC dc(NULL);

		if (_tcscmp(ncm.lfMenuFont.lfFaceName, _T("Segoe UI")) == 0)
			ncm.lfMenuFont.lfQuality = 5;

		ncm.lfMenuFont.lfWeight = FW_NORMAL;
		ncm.lfMenuFont.lfItalic = 0;
		m_font.CreateFontIndirect(&ncm.lfMenuFont);

		ncm.lfMenuFont.lfHeight = ::MulDiv(-12, ::GetDeviceCaps(dc.m_hDC, LOGPIXELSY), 72);

		if (_tcscmp(ncm.lfMenuFont.lfFaceName, _T("Segoe UI")) != 0)
			ncm.lfMenuFont.lfWeight = FW_BOLD;
		m_fontLarge.CreateFontIndirect(&ncm.lfMenuFont);
	}
	else
	{
		m_font.CreateFontIndirect(&ncm.lfMenuFont);
		m_fontLarge.CreateFontIndirect(&ncm.lfCaptionFont);
	}
}

//---------------------------------------------------------------------------
// Button
//---------------------------------------------------------------------------

CButton *CXTPTaskDialogClient::CreateButton(int nButtonID, LPCTSTR lpszLabel, CSize sizeButton, DWORD dwButtonStyle /*=BS_PUSHBUTTON | BS_MULTILINE*/, CXTButtonTheme* pTheme /*=NULL*/)
{
	CButton* pButton = pTheme? new CXTButton: new CButton;

	if (pTheme)
		dwButtonStyle |= BS_LEFT;

	CRect rcButton = XTPSize2Rect(sizeButton);
	if (pButton->Create(lpszLabel, WS_CHILD | WS_VISIBLE |
		dwButtonStyle, rcButton, this, nButtonID))
	{
		m_mapWndSize[pButton] = sizeButton;

		if (pTheme)
			((CXTButton*)pButton)->SetTheme(pTheme);

		pButton->SetFont(&m_font);
		return pButton;
	}

	SAFE_DELETE(pButton);
	return NULL;
}

CButton *CXTPTaskDialogClient::AddCommandButton(int nButtonID, int nIDLabel, CSize sizeButton)
{
	CString strButton;
	XTPResourceManager()->LoadString(&strButton, nIDLabel);
	ASSERT(!strButton.IsEmpty());

	return AddCommandButton(nButtonID, strButton, sizeButton);
}

CButton *CXTPTaskDialogClient::AddCommandButton(int nButtonID, LPCTSTR lpszLabel, CSize sizeButton)
{
	CButton* pButton = CreateButton(nButtonID, lpszLabel, sizeButton, BS_PUSHBUTTON | BS_MULTILINE | WS_TABSTOP);
	if (pButton)
	{
		m_arrCommandButtons.Add(pButton);
		return pButton;
	}

	return NULL;
}

CSize CXTPTaskDialogClient::CalcButtonSize(CString strButton, CPoint* pMarginButton /*=NULL*/, int nMaxWidth /*=0*/)
{
	CPoint ptMarginButton;
	ptMarginButton.x = m_ptBtnSpacing.x + m_ptMargin.x;
	ptMarginButton.y = m_ptPadding.y;

	if (pMarginButton == NULL)
		pMarginButton = &ptMarginButton;

	CXTPEmptySize sizeCaptText;
	CString strCaptText = XTPExtractSubString(strButton, 0);
	if (!strCaptText.IsEmpty())
	{
		sizeCaptText.cx = nMaxWidth;

		XTPDrawHelpers()->StripMnemonics(strCaptText);

		if (nMaxWidth == 0)
			XTPCalcIdealTextSize(strCaptText, sizeCaptText, m_font, 330, 7);
		else
			XTPCalcTextSize(strCaptText, sizeCaptText, m_font);
	}

	CXTPEmptySize sizeNoteText;
	CString strNoteText = XTPExtractSubString(strButton, 1);
	if (!strNoteText.IsEmpty())
	{
		sizeNoteText.cx = nMaxWidth;

		if (nMaxWidth == 0)
			XTPCalcIdealTextSize(strNoteText, sizeNoteText, m_font, 330, 7);
		else
			XTPCalcTextSize(strNoteText, sizeNoteText, m_font);
	}

	CXTPEmptySize sizeButton;
	sizeButton.cx = max(sizeCaptText.cx, sizeNoteText.cx) + (pMarginButton->x * 2);
	sizeButton.cy = sizeCaptText.cy + sizeNoteText.cy + (pMarginButton->y * 2);

	return sizeButton;
}

CSize CXTPTaskDialogClient::CalcCommandButtonSize()
{
	CXTPEmptySize sizeCommon;

	for (int nIDString = XTP_IDS_TASK_OK; nIDString <= XTP_IDS_TASK_CLOSE; ++nIDString)
	{
		CString strButton;
		XTPResourceManager()->LoadString(&strButton, nIDString);
		ASSERT(!strButton.IsEmpty());

		CSize sizeButton = CalcButtonSize(strButton);
		sizeCommon.cx = max(sizeCommon.cx, sizeButton.cx);
		sizeCommon.cy = max(sizeCommon.cy, sizeButton.cy);
	}

	return sizeCommon;
}

int CXTPTaskDialogClient::CalcLinkButtonIdealWidth(CString strButton, int cxWidth)
{
	if (strButton.IsEmpty() || !HasLinkButtons())
		return 0;

	cxWidth -= ((m_ptMargin.x*2) + m_nPadding);

	if (m_pIconLinkGlyph)
		cxWidth -= m_pIconLinkGlyph->GetExtent().cx - m_nPadding;

	CString strCaptText = XTPExtractSubString(strButton, 0);

	int nCaptionWidth = 0;
	CSize sizeText(cxWidth, 0);
	if (XTPCalcIdealTextSize(strCaptText, sizeText, m_fontLarge, 120, 30))
	{
		nCaptionWidth = sizeText.cx;
	}

	CString strNoteText = XTPExtractSubString(strButton, 1);
	int nNoteWidth = CalcIdealTextWidth(strNoteText, cxWidth, FALSE);

	cxWidth = max(nCaptionWidth, nNoteWidth);
	if (m_pIconLinkGlyph)
		cxWidth += m_pIconLinkGlyph->GetExtent().cx;

	return cxWidth + m_ptMargin.x * 2 + m_nPadding;
}

CSize CXTPTaskDialogClient::CalcLinkButtonSize(CString strButton, int cxMaxWidth)
{
	if (strButton.IsEmpty() || !HasLinkButtons())
		return CXTPEmptySize();

	cxMaxWidth -= ((m_ptMargin.x*2) + m_nPadding);

	if (m_pIconLinkGlyph)
		cxMaxWidth -= m_pIconLinkGlyph->GetExtent().cx;

	// get the initial text size.
	CXTPEmptySize sizeButton;

	BOOL bWordWrap = FALSE;

	// extract the title string and determine text size.
	CString strCaptText = XTPExtractSubString(strButton, 0);
	CSize sizeCaptText(cxMaxWidth, 0);
	if (XTPCalcTextSize(strCaptText, sizeCaptText, m_fontLarge, &bWordWrap))
	{
		// if the string spans multiple rows, set the max width.
		if (bWordWrap)
			sizeCaptText.cx = cxMaxWidth;
	}
	else
	{
		sizeCaptText.cx = 0;
	}

	// extract the note string and determine text size.
	CString strNoteText = XTPExtractSubString(strButton, 1);
	CSize sizeNoteText(cxMaxWidth, 0);
	if (XTPCalcTextSize(strNoteText, sizeNoteText, m_font, &bWordWrap))
	{
		// if the string spans multiple rows, set the max width.
		if (bWordWrap)
			sizeNoteText.cx = cxMaxWidth;
	}
	else
	{
		sizeNoteText.cx = 0;
	}

	// add padding around text.
	sizeButton.cx = max(sizeCaptText.cx, sizeNoteText.cx) + m_ptMargin.x * 2 + m_nPadding;
	sizeButton.cy = sizeCaptText.cy + sizeNoteText.cy + m_ptMargin.y * 2 /*+ m_nPadding*/;

	return sizeButton;
}

BOOL CXTPTaskDialogClient::MoveButton(CButton* pButton, int x, int y, int cx, int cy, BOOL bRedraw /*=FALSE*/)
{
	// position the button.
	if (IsWindow(pButton) && pButton->SetWindowPos(NULL, x, y, cx, cy, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE))
	{
		// invalidate.
		if (bRedraw)
			pButton->RedrawWindow();

		return TRUE;
	}

	return FALSE;
}

void CXTPTaskDialogClient::SetCommandButtonPos()
{
	int x = m_rcCommandButtons.left, y = m_rcCommandButtons.top +  m_ptMargin.y - 1;

	int i, nCount = (int)m_arrCommandButtons.GetSize();
	for (i = 0; i < nCount; i++)
	{
		CButton* pButton = m_arrCommandButtons[i];

		CXTPEmptySize sizeButton;
		if (m_mapWndSize.Lookup(pButton, sizeButton))
		{
			// no more room, adjust height.
			if (x !=  m_rcCommandButtons.left && x + sizeButton.cx > m_rcCommandButtons.right)
			{
				x = m_rcCommandButtons.left;
				y += sizeButton.cy + m_ptBtnSpacing.y;
			}

			MoveButton(pButton, x, y, sizeButton.cx, sizeButton.cy);

			x += sizeButton.cx + m_ptBtnSpacing.x;
		}
	}
}

void CXTPTaskDialogClient::SetLinkButtonPos()
{
	int nTop = m_rcLinkButtons.top + m_ptMargin.x + m_ptPadding.y / 2 + 2;

	int nCount = (int)m_arrLinkButtons.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		CSize sizeButton;
		if (m_mapWndSize.Lookup(m_arrLinkButtons[i], sizeButton))
		{
			MoveButton(m_arrLinkButtons[i], m_rcLinkButtons.left, nTop, m_rcLinkButtons.Width(), sizeButton.cy);
			nTop += sizeButton.cy;
		}
	}
}

void CXTPTaskDialogClient::SetVerifyButtonPos()
{
	CXTPEmptySize sizeVerify;
	if (IsWindow(m_pBtnVerify) && m_mapWndSize.Lookup(m_pBtnVerify, sizeVerify))
	{
		CXTPEmptyRect rcButton;
		rcButton.left = m_ptMargin.x + 3;
		rcButton.right = rcButton.left + sizeVerify.cx;

		// vertically center.
		rcButton.top = m_rcCommandButtons.top + (m_rcCommandButtons.Height() - sizeVerify.cy)/2;

		CXTPEmptySize sizeExpando;
		if (IsWindow(m_pBtnExpando) && m_mapWndSize.Lookup(m_pBtnExpando, sizeExpando))
		{
			rcButton.top = m_rcCommandButtons.top + m_ptMargin.y + sizeExpando.cy;
		}

		rcButton.bottom = rcButton.top + sizeVerify.cy;
		MoveButton(m_pBtnVerify, rcButton);
	}
}

void CXTPTaskDialogClient::SetExpandoButtonPos()
{
	CXTPEmptySize sizeExpando;
	if (IsWindow(m_pBtnExpando) && m_mapWndSize.Lookup(m_pBtnExpando, sizeExpando))
	{
		CXTPEmptyRect rcButton;
		rcButton.left = m_ptMargin.x + 3;
		rcButton.right = rcButton.left + sizeExpando.cx;

		// vertically center.
		rcButton.top = m_rcCommandButtons.top + (m_rcCommandButtons.Height()-sizeExpando.cy)/2;

		CXTPEmptySize sizeVerify;
		if (IsWindow(m_pBtnVerify) && m_mapWndSize.Lookup(m_pBtnVerify, sizeVerify))
		{
			rcButton.top = m_rcCommandButtons.top + m_ptMargin.y;
		}

		rcButton.bottom = rcButton.top + sizeExpando.cy;
		MoveButton(m_pBtnExpando, rcButton);
	}
}

void CXTPTaskDialogClient::SetLinkCtrlPos()
{
	if (IsHyperlinksEnabled())
	{
		if (m_wndContent) m_wndContent.MoveWindow(m_rcContent);
		if (m_wndExpandedInformation) m_wndExpandedInformation.MoveWindow(m_rcExpandedInformation);
		if (m_wndFooter) m_wndFooter.MoveWindow(m_rcFooter);

		if (m_wndExpandedInformation) m_wndExpandedInformation.ShowWindow(m_bExpanded ? SW_SHOWNOACTIVATE : SW_HIDE);
	}
}

//---------------------------------------------------------------------------
// Layout
//---------------------------------------------------------------------------

int CXTPTaskDialogClient::CalcIdealTextWidth(const CString& strItemText, int nMinWidth, BOOL bLargeFont)
{
	if (strItemText.IsEmpty())
		return nMinWidth;

	CSize sizeText(nMinWidth, 0);
	if (XTPCalcIdealTextSize(strItemText, sizeText,
		bLargeFont? m_fontLarge: m_font,
		bLargeFont? 275: 330,
		bLargeFont? 14: 7))
	{
		nMinWidth = max(nMinWidth, sizeText.cx);
	}
	return nMinWidth;
}
CRect CXTPTaskDialogClient::CalcTextRect(const CString& strItemText, int nWidth, BOOL bLargeFont)
{
	CSize sizeText(nWidth, 0);
	CRect rcItemText(0, 0, 0, 0);

	if (XTPCalcTextSize(strItemText, sizeText, bLargeFont? m_fontLarge: m_font))
	{
		sizeText.cx = nWidth;
		rcItemText = XTPSize2Rect(sizeText);
	}

	return rcItemText;
}

CRect CXTPTaskDialogClient::CalcCommandRect(int nWidth)
{
	CRect rcCommand(0, 0, nWidth, 0);

	if (m_pBtnExpando == 0 && m_pBtnVerify == 0 && m_arrCommandButtons.GetSize() == 0)
		return rcCommand;

	int nMinHeight = 0;
	int nLeftMargin = 0;

	CXTPEmptySize sizeButton;
	if (m_pBtnVerify && m_mapWndSize.Lookup(m_pBtnVerify, sizeButton))
	{
		nMinHeight = sizeButton.cy;
		nLeftMargin = sizeButton.cx;
	}

	if (m_pBtnExpando && m_mapWndSize.Lookup(m_pBtnExpando, sizeButton))
	{
		nMinHeight += sizeButton.cy;
		nLeftMargin = max(nLeftMargin, sizeButton.cx);
	}

	if (nLeftMargin)
		nWidth -= nLeftMargin + m_ptBtnSpacing.x;

	int i, nCount = (int)m_arrCommandButtons.GetSize();
	int x = 0;
	int nMaxWidth = 0;

	for (i = 0; i < nCount; i++)
	{
		CButton* pButton = m_arrCommandButtons[i];

		if (m_mapWndSize.Lookup(pButton, sizeButton))
		{
			// no more room, adjust height.
			if (x != 0 && x + sizeButton.cx > nWidth)
			{
				nMaxWidth = max(nMaxWidth, x - m_ptBtnSpacing.x);
				x = 0;
				rcCommand.bottom += m_ptBtnSpacing.y;
			}

			// first button in row, adjust height.
			if (x == 0)
				rcCommand.bottom += sizeButton.cy;

			x += sizeButton.cx + m_ptBtnSpacing.x;
		}
	}
	nMaxWidth = max(nMaxWidth, x - m_ptBtnSpacing.x);

	rcCommand.left = nWidth - nMaxWidth;
	if (nLeftMargin)
		rcCommand.left += nLeftMargin + m_ptBtnSpacing.x;

	if (nMinHeight > rcCommand.Height())
		rcCommand.bottom = rcCommand.top + nMinHeight;

	rcCommand.bottom += m_ptMargin.y * 2 - 2;

	return rcCommand;
}

int CXTPTaskDialogClient::CalcClientWidth()
{
	int nIdealWidth = m_nMinClientWidth;

	if (HasFixedWidth())
	{
		CSize sizeBase = ::GetDialogBaseUnits();
		int cxWidth = ::MulDiv(m_pConfig->cxWidth, sizeBase.cx, 4);

		nIdealWidth = max(cxWidth - m_ptMargin.x * 2, nIdealWidth);
		return nIdealWidth;
	}

	int nCommandButtonWidth = -m_ptBtnSpacing.x + m_ptMargin.x * 2, nCount, i;

	nCount = (int)m_arrCommandButtons.GetSize();
	for (i = 0; i < nCount; ++i)
	{
		CSize size;
		if (m_mapWndSize.Lookup(m_arrCommandButtons[i], size))
			nCommandButtonWidth += size.cx + m_ptBtnSpacing.x;
	}

	if (!HasLinkButtons())
	{
		nIdealWidth = max(nIdealWidth, nCommandButtonWidth);
	}

	if (m_pBtnVerify->GetSafeHwnd())
	{
		CSize size;
		if (m_mapWndSize.Lookup(m_pBtnVerify, size))
		{
			nIdealWidth = max(nIdealWidth, size.cx + nCommandButtonWidth + m_ptMargin.x);
		}
	}

	if (m_pBtnExpando->GetSafeHwnd())
	{
		CSize size;
		if (m_mapWndSize.Lookup(m_pBtnExpando , size))
		{
			nIdealWidth = max(nIdealWidth, size.cx + nCommandButtonWidth + m_ptMargin.x);
		}
	}

	int nMainIconMargin = (m_hIconMain? (m_sizeIconMain.cx + m_ptMargin.x): 0);

	nCount = (int)m_arrRadioButtons.GetSize();
	for (i = 0; i < nCount; ++i)
	{
		CSize size;
		if (m_mapWndSize.Lookup(m_arrRadioButtons[i], size))
			nIdealWidth = max(nIdealWidth, size.cx + nMainIconMargin);
	}



	CXTPEmptySize size;
	if (m_mapWndSize.Lookup(m_pBtnVerify, size))
		nIdealWidth = max(nIdealWidth, size.cx);

	if (m_mapWndSize.Lookup(m_pBtnExpando, size))
		nIdealWidth = max(nIdealWidth, size.cx);


	int nWidth = CalcIdealTextWidth(m_strMainInstruction, nIdealWidth - nMainIconMargin, TRUE);
	nIdealWidth = max(nIdealWidth, nWidth);

	nWidth = CalcIdealTextWidth(m_strContent, nIdealWidth - nMainIconMargin, FALSE);
	nIdealWidth = max(nIdealWidth, nWidth);

	if (HasLinkButtons())
	{
		for (i = 0; i < (int)m_pConfig->cButtons; ++i)
		{
			CString strButton;
			LoadItemString(strButton, m_pConfig->pButtons[i].pszButtonText);

			nWidth = CalcLinkButtonIdealWidth(strButton, nIdealWidth - nMainIconMargin);
			nIdealWidth = max(nIdealWidth, nWidth + nMainIconMargin);
		}
	}


	nWidth = CalcIdealTextWidth(m_strExpandedInformation, nIdealWidth, FALSE);
	nIdealWidth = max(nIdealWidth, nWidth);

	nWidth = CalcIdealTextWidth(m_strFooter, nIdealWidth, FALSE);
	nIdealWidth = max(nIdealWidth, nWidth);

	nIdealWidth = min(nIdealWidth, m_nMaxClientWidth);

	return nIdealWidth;
}


CSize CXTPTaskDialogClient::CalcDynamicLayout(int nIdealWidth)
{
	int nMainIconMargin = m_hIconMain ? (m_sizeIconMain.cx + m_ptMargin.x) : 0;
	int nFooterIconMargin = m_hIconFooter ? (m_sizeIconFooter.cx + m_ptMargin.x) : 0;

	m_rcMainInstruction = CalcTextRect(m_strMainInstruction, nIdealWidth - nMainIconMargin, TRUE);
	if (!m_rcMainInstruction.IsRectEmpty())
		m_rcMainInstruction.OffsetRect(m_ptMargin.x + nMainIconMargin, m_ptMargin.y);

	m_rcContent = CalcTextRect(m_strContent, nIdealWidth - nMainIconMargin, FALSE);
	m_rcContent.OffsetRect(m_ptMargin.x + nMainIconMargin,
		m_rcMainInstruction.bottom + m_ptMargin.y + 1);

	int nLinkTop = m_rcContent.bottom + m_ptPadding.y;

	if (nLinkTop < m_ptMargin.y * 4 - 3)
		nLinkTop = m_ptMargin.y * 4 - 3;

	if (!IsExpandFooterArea())
	{
		if (!m_strExpandedInformation.IsEmpty())
		{
			if (m_bExpanded)
			{
				m_rcExpandedInformation = CalcTextRect(m_strExpandedInformation, nIdealWidth - nMainIconMargin, FALSE);
				m_rcExpandedInformation.OffsetRect(m_ptMargin.x + nMainIconMargin, nLinkTop + m_ptPadding.y);
			}
			else
			{
				m_rcExpandedInformation.SetRect(m_rcContent.left, nLinkTop, m_rcContent.right, nLinkTop + m_ptPadding.y);
			}

			nLinkTop += m_rcExpandedInformation.Height() + m_ptPadding.y;
		}
		else
		{
			m_rcExpandedInformation.SetRect(m_rcContent.left, nLinkTop, m_rcContent.right, nLinkTop);
		}
	}

	if (m_wndProgressBar.GetSafeHwnd())
	{
		CRect rcProgressBar(m_rcContent.left, nLinkTop + m_ptMargin.y, nIdealWidth + m_ptMargin.x, nLinkTop + m_ptMargin.y * 5/2);
		m_wndProgressBar.MoveWindow(rcProgressBar);

		nLinkTop += rcProgressBar.Height() + m_ptMargin.y;
	}

	if (m_arrRadioButtons.GetSize() > 0)
	{
		nLinkTop +=  m_ptMargin.y;
		m_rcRadioButtons = CRect(m_rcContent.left + m_ptMargin.x, nLinkTop, nIdealWidth, nLinkTop);

		for (int i = 0; i < m_arrRadioButtons.GetSize(); i++)
		{
			CButton* pButton = m_arrRadioButtons[i];

			CSize sizeButton;
			if (m_mapWndSize.Lookup(pButton, sizeButton))
			{
				pButton->MoveWindow(m_rcRadioButtons.left, nLinkTop, sizeButton.cx, sizeButton.cy);

				nLinkTop += sizeButton.cy + 3;
			}
		}
		m_rcRadioButtons.bottom = nLinkTop;
	}
	else
	{
		m_rcRadioButtons = CRect(m_rcContent.left + 1, nLinkTop, m_rcContent.right, nLinkTop);
	}

	m_rcLinkButtons = CRect(m_rcContent.left + 1, nLinkTop + 1, m_ptMargin.y + nIdealWidth - 1, nLinkTop + 1);

	for (int i = 0; i < (int)m_arrLinkButtons.GetSize(); ++i)
	{
		CString strButton;
		LoadItemString(strButton, m_pConfig->pButtons[i].pszButtonText);
		CButton* pButton = m_arrLinkButtons[i];

		CSize sizeButton = CalcLinkButtonSize(strButton, m_rcLinkButtons.Width());
		sizeButton.cx = m_rcLinkButtons.Width();

		m_mapWndSize[pButton] = sizeButton;

		m_rcLinkButtons.bottom += sizeButton.cy;
	}
	if (m_arrLinkButtons.GetSize() > 0) m_rcLinkButtons.bottom += m_ptMargin.y;

	m_rcCommandButtons = CalcCommandRect(nIdealWidth);
	m_rcCommandButtons.OffsetRect(m_ptMargin.x, m_rcLinkButtons.bottom  + m_ptMargin.y + m_ptPadding.y);

	int nBottom = m_rcCommandButtons.bottom;
	if (!m_strFooter.IsEmpty())
	{
		m_rcFooter = CalcTextRect(m_strFooter, nIdealWidth - nFooterIconMargin, FALSE);
		m_rcFooter.OffsetRect(m_ptMargin.x + nFooterIconMargin - 1, nBottom + m_ptMargin.y + 1);

		nBottom += m_rcFooter.Height() + 2 * m_ptMargin.y + 1;
	}

	if (IsExpandFooterArea())
	{
		if (!m_strExpandedInformation.IsEmpty() && m_bExpanded)
		{
			m_rcExpandedInformation = CalcTextRect(m_strExpandedInformation, nIdealWidth, FALSE);
			m_rcExpandedInformation.OffsetRect(m_ptMargin.x, nBottom + m_ptMargin.y);
			nBottom += m_rcExpandedInformation.Height() + 2 * m_ptMargin.y;
		}
		else
		{
			m_rcExpandedInformation.SetRect(m_rcFooter.left, nBottom, m_rcFooter.right, nBottom);
		}

	}

	return CSize(nIdealWidth + m_ptMargin.x * 2, nBottom);
}

void CXTPTaskDialogClient::RecalcLayout(BOOL bMoveWindow)
{
	UpdateZOrder();

	CSize szClient = CalcDynamicLayout(CalcClientWidth());

	SetCommandButtonPos();
	SetLinkButtonPos();
	SetVerifyButtonPos();
	SetExpandoButtonPos();
	SetLinkCtrlPos();

	ResizeParentToFit(szClient, bMoveWindow);
	RedrawWindow(0, 0, RDW_ALLCHILDREN | RDW_INVALIDATE);
}

BOOL CXTPTaskDialogClient::ResizeParentToFit(CSize szClient, BOOL bMoveWindow)
{
	CXTPWindowRect rc(this);

	CRect rcParent = IsPositionRelativeToWindow()?
		CXTPWindowRect(m_pConfig->hwndParent): XTPMultiMonitor()->GetWorkArea(m_pConfig->hwndParent);

	CPoint ptPos((rcParent.left + rcParent.right - szClient.cx) / 2 - 3,
		(rcParent.top + rcParent.bottom - szClient.cy) / 2 - 12);

	CRect rcWindow(ptPos, szClient);

	AdjustWindowRectEx(rcWindow, GetStyle(), FALSE, GetExStyle());
	//rcWindow.InflateRect(m_ptMargin.x, m_ptMargin.y);

	if (bMoveWindow)
		MoveWindow(rcWindow);
	else
		SetWindowPos(0, 0, 0, rcWindow.Width(), rcWindow.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	return TRUE;
}

//---------------------------------------------------------------------------
// Rendering
//---------------------------------------------------------------------------

void CXTPTaskDialogClient::DrawBackground(CDC* pDC, CRect rect)
{
	pDC->FillSolidRect(&rect, m_crFooter);

	if (m_bThemeReady)
	{
		pDC->FillSolidRect(rect.left, rect.top, rect.Width(), m_rcCommandButtons.top, m_crContext);

		if (m_rcCommandButtons.top != m_rcCommandButtons.bottom)
		{
			pDC->FillSolidRect(rect.left, m_rcCommandButtons.top,  rect.Width(), 1, m_cr3DLight);
		}

		if (!m_rcFooter.IsRectEmpty())
		{
			pDC->FillSolidRect(rect.left, m_rcCommandButtons.bottom,  rect.Width(), 1, m_cr3DLight);
			pDC->FillSolidRect(rect.left, m_rcCommandButtons.bottom + 1,  rect.Width(), 1, m_cr3DHighLight);
		}

		if (m_bExpanded && !m_strExpandedInformation.IsEmpty() && IsExpandFooterArea() && !m_rcExpandedInformation.IsRectEmpty())
		{
			pDC->FillSolidRect(rect.left, m_rcExpandedInformation.top - m_ptMargin.y,  rect.Width(), 1, m_cr3DLight);
			pDC->FillSolidRect(rect.left, m_rcExpandedInformation.top - m_ptMargin.y + 1,  rect.Width(), 1, m_cr3DHighLight);
		}
	}
	else
	{
		if (!m_rcFooter.IsRectEmpty())
		{
			pDC->FillSolidRect(rect.left, m_rcCommandButtons.bottom + 1, rect.Width(), 1, m_cr3DShadow);
		}
		if (m_bExpanded && !m_strExpandedInformation.IsEmpty() && IsExpandFooterArea() && !m_rcExpandedInformation.IsRectEmpty())
		{
			pDC->FillSolidRect(rect.left, m_rcExpandedInformation.top - m_ptMargin.y,  rect.Width(), 1, m_cr3DShadow);
		}
	}
}

void CXTPTaskDialogClient::DrawIcon(CDC* pDC, HICON hIcon, CPoint pt, CSize sz)
{
	DWORD dwLayout = XTPDrawHelpers()->IsContextRTL(pDC);
	if (dwLayout & LAYOUT_RTL)
		XTPDrawHelpers()->SetContextRTL(pDC, dwLayout | LAYOUT_BITMAPORIENTATIONPRESERVED);

	::DrawIconEx(*pDC, pt.x, pt.y, hIcon, sz.cx, sz.cy, 0, NULL, DI_NORMAL);

	if (dwLayout & LAYOUT_RTL)
		XTPDrawHelpers()->SetContextRTL(pDC, dwLayout);

}

void CXTPTaskDialogClient::DrawMainInstruction(CDC* pDC)
{
	if (m_hIconMain)
	{
		DrawIcon(pDC, m_hIconMain, m_ptMargin, m_sizeIconMain);
	}

	if (!m_rcMainInstruction.IsRectEmpty() && !m_strMainInstruction.IsEmpty())
	{
		pDC->SetTextColor(m_crMainText);

		CXTPFontDC fontDC(pDC, &m_fontLarge);
		pDC->DrawText(m_strMainInstruction, m_rcMainInstruction,
			DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_TOP);
	}
}

void CXTPTaskDialogClient::DrawContent(CDC* pDC)
{
	if (m_rcContent.IsRectEmpty())
		return;

	if (!m_strContent.IsEmpty() &&  !::IsWindow(m_wndContent.m_hWnd))
	{
		pDC->SetTextColor(m_crContextText);

		CXTPFontDC fontDC(pDC, &m_font);
		pDC->DrawText(m_strContent, m_rcContent, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_TOP);
	}
}

void CXTPTaskDialogClient::DrawExpandedInfo(CDC* pDC)
{
	if (m_rcExpandedInformation.IsRectEmpty() || m_strExpandedInformation.IsEmpty() || !m_bExpanded)
		return;

	if (!::IsWindow(m_wndExpandedInformation.m_hWnd))
	{
		pDC->SetTextColor(IsExpandFooterArea() ? m_crFooterText : m_crContextText);

		CXTPFontDC fontDC(pDC, &m_font);
		pDC->DrawText(m_strExpandedInformation, m_rcExpandedInformation, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_TOP);
	}
}

void CXTPTaskDialogClient::DrawFootNote(CDC* pDC)
{
	if (m_rcFooter.IsRectEmpty())
		return;

	if (m_hIconFooter)
	{
		DrawIcon(pDC, m_hIconFooter, CPoint(m_ptMargin.x + 2, m_rcFooter.top), m_sizeIconFooter);
	}

	if (!::IsWindow(m_wndFooter.m_hWnd))
	{
		pDC->SetTextColor(m_crFooterText);

		CXTPFontDC fontDC(pDC, &m_font);
		pDC->DrawText(m_strFooter, m_rcFooter, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_TOP);
	}
}

void CXTPTaskDialogClient::OnExpandExpandoClicked()
{
	if (!m_pBtnExpando)
		return;

	CString strButtonText = m_bExpanded ? m_strExpandedControlText: m_strCollapsedControlText;
	m_pBtnExpando->SetWindowText(strButtonText);
	m_pBtnExpando->SetChecked(m_bExpanded);

	RecalcLayout();
}
