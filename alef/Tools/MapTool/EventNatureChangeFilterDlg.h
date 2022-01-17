#if !defined(AFX_EVENTNATURECHANGEFILTERDLG_H__B11159A6_098B_45B9_AE0C_FE5C01C3DB49__INCLUDED_)
#define AFX_EVENTNATURECHANGEFILTERDLG_H__B11159A6_098B_45B9_AE0C_FE5C01C3DB49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventNatureChangeFilterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventNatureChangeFilterDlg dialog

class CEventNatureChangeFilterDlg : public CDialog
{
// Construction
public:
	CEventNatureChangeFilterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventNatureChangeFilterDlg)
	enum { IDD = IDD_EVENTNATURE_FILTER };
	BOOL	m_bAmbience		;
	BOOL	m_bByTime		;
	BOOL	m_bCircumstance	;
	BOOL	m_bCloud		;
	BOOL	m_bBgm			;
	BOOL	m_bFog			;
	BOOL	m_bLightColor	;
	BOOL	m_bLightPosition;
	BOOL	m_bSkyColor		;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventNatureChangeFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventNatureChangeFilterDlg)
	afx_msg void OnDonotchangeall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTNATURECHANGEFILTERDLG_H__B11159A6_098B_45B9_AE0C_FE5C01C3DB49__INCLUDED_)
