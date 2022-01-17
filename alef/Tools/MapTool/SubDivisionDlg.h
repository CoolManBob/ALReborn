#if !defined(AFX_SUBDIVISIONDLG_H__88F4FA18_765F_49FF_A59C_7C7F2BBC04A8__INCLUDED_)
#define AFX_SUBDIVISIONDLG_H__88F4FA18_765F_49FF_A59C_7C7F2BBC04A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubDivisionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSubDivisionDlg dialog

class CSubDivisionDlg : public CDialog
{
// Construction
public:

	enum DETAIL
	{
		SUBDIVISIONDETAIL_ONE	,
		SUBDIVISIONDETAIL_x05	,
		SUBDIVISIONDETAIL_x1	,
		SUBDIVISIONDETAIL_x2	,
		SUBDIVISIONDETAIL_x4	,
		SUBDIVISIONDETAIL_x8	,
		SUBDIVISIONDETAIL_COUNT
	};
	CSubDivisionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSubDivisionDlg)
	enum { IDD = IDD_SUBDIVISION };
	CSliderCtrl	m_ctlSlider	;
	CString		m_strDetail	;
	BOOL	m_bTiled;
	BOOL	m_bAlphaTiled;
	//}}AFX_DATA

public:
	int			m_nDetail	;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubDivisionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSubDivisionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureDetailslider(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBDIVISIONDLG_H__88F4FA18_765F_49FF_A59C_7C7F2BBC04A8__INCLUDED_)
