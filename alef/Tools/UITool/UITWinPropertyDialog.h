#if !defined(AFX_UITWINPROPERTYDIALOG_H__27CE5F54_6B83_448F_8DB5_0BC65864C2FC__INCLUDED_)
#define AFX_UITWINPROPERTYDIALOG_H__27CE5F54_6B83_448F_8DB5_0BC65864C2FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWinPropertyDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITWinPropertyDialog dialog

class UITWinPropertyDialog : public CPropertyPage
{
	DECLARE_DYNCREATE(UITWinPropertyDialog)

private:
	CHAR *			m_szName;
	AcUIBase *		m_pcsBase;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

public:
	AcuTextureList	m_csTextures;

// Construction
public:
	BOOL UpdateDataToDialog(CHAR *szName, AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();
	BOOL RefreshTextureList();

	BOOL OnSetActive();

	UITWinPropertyDialog();   // standard constructor
	~UITWinPropertyDialog();

// Dialog Data
	//{{AFX_DATA(UITWinPropertyDialog)
	enum { IDD = IDD_UIT_WP_WINDOW_DIALOG };
	CListCtrl	m_csTextureList;
	CString	m_strName;
	long	m_lX;
	long	m_lY;
	long	m_ulW;
	UINT	m_ulH;
	BYTE	m_cRed;
	BYTE	m_cGreen;
	BYTE	m_cBlue;
	BYTE	m_cAlpha;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITWinPropertyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITWinPropertyDialog)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnUITAddTexture();
	afx_msg void OnUITEditTexture();
	afx_msg void OnUITDeleteTexture();
	virtual BOOL OnInitDialog();
	afx_msg void OnUITSetTexture();
	afx_msg void OnUITDelete();
	afx_msg void OnUITSizeAsImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWINPROPERTYDIALOG_H__27CE5F54_6B83_448F_8DB5_0BC65864C2FC__INCLUDED_)
