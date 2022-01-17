// XTPTaskDialogClient.h: interface for the CXTPTaskDialogClient class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPTASKCLIENTWND_H__)
#define __XTPTASKCLIENTWND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "XTPTaskDialogControls.h"

class CXTPTaskDialogFrame;
class CXTPImageManagerIcon;

enum XTPTaskSection
{
	xtpNone,
	xtpMainInstruction,
	xtpContent,
	xtpCommand,
	xtpFooter,
	xtpExpandedInformation,
};

#define XTP_IDC_VERIFY                      10
#define XTP_IDC_EXPANDO                     11
#define XTP_CLASSNAME_TASKDIALOGUIWND       _T("Codejock.TaskDialogUIWnd")

//===========================================================================
// CXTPTaskDialogClient window
//===========================================================================
class _XTP_EXT_CLASS CXTPTaskDialogClient : public CDialog
{
public:
	CXTPTaskDialogClient();
	virtual ~CXTPTaskDialogClient();

public:
	HRESULT SendNotify(UINT uNotify, WPARAM wParam = 0, LPARAM lParam = 0);

public:
	virtual BOOL CreateClient();

	// flag checks
	BOOL IsLinkButtonsUsed() const;
	BOOL IsLinkButtonsIconVisible() const;
	BOOL HasLinkButtons() const;
	BOOL IsExpandedByDefault() const;
	BOOL IsExpandFooterArea() const;
	BOOL HasFixedWidth() const;
	BOOL IsPositionRelativeToWindow() const;
	BOOL IsProgressBarVisible() const;
	BOOL IsHyperlinksEnabled() const;
	BOOL IsFooterIconHandleUsed() const;
	BOOL IsMainIconHandleUsed() const;
	BOOL CanCancel() const;

protected:

	int                                     m_nIdealWidth;
	int                                     m_nSection;
	int                                     m_nMaxClientWidth;
	int                                     m_nMinClientWidth;
	int                                     m_nPadding;
	BOOL                                    m_bExpanded;
	BOOL                                    m_bVerification;
	BOOL                                    m_bUseSysIcons;
	BOOL                                    m_bThemeReady;
	HICON                                   m_hIconFooter;
	HICON                                   m_hIconMain;

	CXTPImageManagerIcon*                   m_pIconLinkGlyph;

	COLORREF                                m_crContext;
	COLORREF                                m_crContextText;
	COLORREF                                m_crFooter;
	COLORREF                                m_crFooterText;
	COLORREF                                m_crMainText;
	COLORREF                                m_cr3DLight;
	COLORREF                                m_cr3DShadow;
	COLORREF                                m_cr3DHighLight;
	CFont                                   m_font;
	CFont                                   m_fontLarge;
	CPoint                                  m_ptMargin;
	CPoint                                  m_ptPadding;
	CPoint                                  m_ptBtnSpacing;
	CPoint                                  m_ptBorder;
	CSize                                   m_sizeIconFooter;
	CSize                                   m_sizeIconMain;
	CSize                                   m_sizeCommandButton;
	CRect                                   m_rcMainInstruction;
	CRect                                   m_rcContent;
	CRect                                   m_rcCommandButtons;
	CRect                                   m_rcExpandedInformation;
	CRect                                   m_rcFooter;
	CRect                                   m_rcRadioButtons;
	CRect                                   m_rcLinkButtons;
	CString                                 m_strMainInstruction;
	CString                                 m_strContent;
	CString                                 m_strFooter;
	CString                                 m_strVerificationText;
	CString                                 m_strExpandedInformation;
	CString                                 m_strExpandedControlText;
	CString                                 m_strCollapsedControlText;
	TASKDIALOGCONFIG*                       m_pConfig;

	CButton*                                m_pBtnVerify;
	CXTButton*                              m_pBtnExpando;
	CXTPTaskDialogLinkCtrl                  m_wndContent;
	CXTPTaskDialogLinkCtrl                  m_wndExpandedInformation;
	CXTPTaskDialogLinkCtrl                  m_wndFooter;
	CXTPTaskDialogProgressCtrl              m_wndProgressBar;
	CArray<CButton*,CButton*>               m_arrCommandButtons;
	CArray<CButton*,CButton*>               m_arrLinkButtons;
	CArray<CButton*,CButton*>               m_arrRadioButtons;

	int                                     m_nSelRadioButtonID; // ID of the radio button that was selected when the task dialog was closed.
	CMap<CWnd*,CWnd*,SIZE,SIZE&> m_mapWndSize;
	BOOL                                    m_bMessageBoxStyle;

protected:

	BOOL IsWindow(CWnd* pWnd) const;

	// initialization
	void CreateIcons();
	void LoadStrings();
	void CreateFonts();

	BOOL LoadItemString(CString& strItem, PCWSTR pszBuffer);

	BOOL CreateCommandButtons();
	BOOL CreateLinkButtons();
	BOOL CreateRadioButtons();
	BOOL CreateVerifyExpandButtons();
	BOOL CreateProgressBar();

	CButton* AddCommandButton(int nButtonID, int nIDLabel, CSize sizeButton);
	CButton* AddCommandButton(int nButtonID, LPCTSTR lpszLabel, CSize sizeButton);
	CButton* CreateButton(int nButtonID, LPCTSTR lpszLabel, CSize sizeButton, DWORD dwButtonStyle = BS_PUSHBUTTON | BS_MULTILINE, CXTButtonTheme* pTheme = NULL);

	HICON CreateIcon(HICON hIcon, PCWSTR lpwzIcon, CSize sizeIcon, BOOL bUseHandle);

	// layout
	CSize CalcCommandButtonSize();
	CSize CalcButtonSize(CString strButton, CPoint* pMarginButton = NULL, int nMaxWidth = 0);
	CSize CalcLinkButtonSize(CString strButton, int nMaxCmdLinkWidth);

	CSize CalcDynamicLayout(int nIdealWidth);


	CRect CalcCommandRect(int nIdealWidth);
	int CalcIdealTextWidth(const CString& strItemText, int nMinWidth, BOOL bLargeFont);
	CRect CalcTextRect(const CString& strItemText, int nWidth, BOOL bLargeFont);
	int CalcLinkButtonIdealWidth(CString strButton, int cxWidth);

	void RecalcLayout(BOOL bMoveWindow = FALSE);
	BOOL ResizeParentToFit(CSize szClient, BOOL bMoveWindow);
	void SetCommandButtonPos();
	void SetLinkButtonPos();
	void SetVerifyButtonPos();
	void SetExpandoButtonPos();
	void SetLinkCtrlPos();

	BOOL MoveButton(CButton* pButton, int x, int y, int cx, int cy, BOOL bRedraw = FALSE);
	BOOL MoveButton(CButton* pButton, const CRect& rect, BOOL bRedraw = FALSE);

	void ZOrder(CWnd* pWnd);
	void UpdateZOrder();

	int CalcClientWidth();

	// Drawing
	void RefreshMetrics();
	void RefreshColors();

	void DeleteAllButtons();

	virtual void DrawMainInstruction(CDC* pDC);
	virtual void DrawContent(CDC* pDC);
	virtual void DrawFootNote(CDC* pDC);
	virtual void DrawBackground(CDC* pDC, CRect rect);
	virtual void DrawExpandedInfo(CDC* pDC);

	void DrawIcon(CDC* pDC, HICON hIcon, CPoint pt, CSize sz);
	void OnExpandExpandoClicked();

	//{{AFX_VIRTUAL(CXTPTaskDialogClient)
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPTaskDialogClient)
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSysColorChange();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

AFX_INLINE BOOL CXTPTaskDialogClient::IsLinkButtonsUsed() const {
	return ((m_pConfig->dwFlags & (TDF_USE_COMMAND_LINKS | TDF_USE_COMMAND_LINKS_NO_ICON)) != 0);
}
AFX_INLINE BOOL CXTPTaskDialogClient::IsProgressBarVisible() const {
	return ((m_pConfig->dwFlags & (TDF_SHOW_PROGRESS_BAR | TDF_SHOW_MARQUEE_PROGRESS_BAR)) != 0);
}
AFX_INLINE BOOL CXTPTaskDialogClient::IsLinkButtonsIconVisible() const {
	return ((m_pConfig->dwFlags & TDF_USE_COMMAND_LINKS_NO_ICON) == 0);
}
AFX_INLINE BOOL CXTPTaskDialogClient::HasLinkButtons() const {
	return ((m_pConfig->pButtons != NULL) && (m_pConfig->cButtons > 0) && IsLinkButtonsUsed());
}
AFX_INLINE BOOL CXTPTaskDialogClient::IsExpandedByDefault() const {
	return ((m_pConfig->dwFlags & TDF_EXPANDED_BY_DEFAULT) != 0);
}
AFX_INLINE BOOL CXTPTaskDialogClient::IsExpandFooterArea() const {
	return ((m_pConfig->dwFlags & TDF_EXPAND_FOOTER_AREA) != 0);
}
AFX_INLINE BOOL CXTPTaskDialogClient::HasFixedWidth() const {
	return (m_pConfig->cxWidth != 0);
}
AFX_INLINE BOOL CXTPTaskDialogClient::IsPositionRelativeToWindow() const {
	return m_pConfig->hwndParent && (m_pConfig->dwFlags & TDF_POSITION_RELATIVE_TO_WINDOW);
}
AFX_INLINE BOOL CXTPTaskDialogClient::IsHyperlinksEnabled() const {
	return ((m_pConfig->dwFlags & TDF_ENABLE_HYPERLINKS) != 0);
}
AFX_INLINE BOOL CXTPTaskDialogClient::IsFooterIconHandleUsed() const {
	return ((m_pConfig->dwFlags & TDF_USE_HICON_FOOTER) != 0);
}
AFX_INLINE BOOL CXTPTaskDialogClient::IsMainIconHandleUsed() const {
	return ((m_pConfig->dwFlags & TDF_USE_HICON_MAIN) != 0);
}
AFX_INLINE BOOL CXTPTaskDialogClient::IsWindow(CWnd* pWnd) const {
	return (pWnd && ::IsWindow(pWnd->GetSafeHwnd()));
}
AFX_INLINE BOOL CXTPTaskDialogClient::MoveButton(CButton* pButton, const CRect& rect, BOOL bRedraw /*=FALSE*/) {
	return MoveButton(pButton, rect.left, rect.top, rect.Width(), rect.Height(), bRedraw);
}


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPTASKCLIENTWND_H__)
