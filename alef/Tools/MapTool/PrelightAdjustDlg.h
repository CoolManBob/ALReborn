#if !defined(AFX_PRELIGHTADJUSTDLG_H__41F091E9_C901_4BC1_9273_9040AD4DF685__INCLUDED_)
#define AFX_PRELIGHTADJUSTDLG_H__41F091E9_C901_4BC1_9273_9040AD4DF685__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrelightAdjustDlg.h : header file
//

#include "portionStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CPrelightAdjustDlg dialog

class CPrelightAdjustDlg : public CDialog
{
// Construction
public:
	CPrelightAdjustDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrelightAdjustDlg)
	enum { IDD = IDD_PRELIGHT };
	CPortionStatic	m_ctlPortion;
	int		m_nGeometry		;
	int		m_nObject		;
	int		m_nCharacter	;
	CString	m_strGeometry	;
	CString	m_strObject		;
	CString	m_strCharacter	;
	//}}AFX_DATA
protected:

public:
	CRect	m_Rect			;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrelightAdjustDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrelightAdjustDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRELIGHTADJUSTDLG_H__41F091E9_C901_4BC1_9273_9040AD4DF685__INCLUDED_)
