#if !defined(AFX_CLIPPLANESETDLG_H__4AF63C5C_3307_4624_9D58_653EA88AED6D__INCLUDED_)
#define AFX_CLIPPLANESETDLG_H__4AF63C5C_3307_4624_9D58_653EA88AED6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClipPlaneSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClipPlaneSetDlg dialog

class CClipPlaneSetDlg : public CDialog
{
// Construction
public:
	CClipPlaneSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CClipPlaneSetDlg)
	enum { IDD = IDD_FARCLIPPLANE };
	CSliderCtrl	m_ctlFogDistance;
	CSliderCtrl	m_ctlFogFarClip;
	BOOL	m_bUseFog				;
	BOOL	m_bUseSky				;
	int		m_nLoadRange_Data		;
	int		m_nLoadRange_Detail		;
	int		m_nLoadRange_Rough		;
	float	m_fFogDistance;
	float	m_fFogFarClip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClipPlaneSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClipPlaneSetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUsefog();
	afx_msg void OnUsesky();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIPPLANESETDLG_H__4AF63C5C_3307_4624_9D58_653EA88AED6D__INCLUDED_)
