#if !defined(AFX_SECTORCREATEDLG_H__305079D8_F34D_4370_BA51_2234D7494A2B__INCLUDED_)
#define AFX_SECTORCREATEDLG_H__305079D8_F34D_4370_BA51_2234D7494A2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SectorCreateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSectorCreateDlg dialog

class CSectorCreateDlg : public CDialog
{
// Construction
public:
	CSectorCreateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSectorCreateDlg)
	enum { IDD = IDD_CREATESECTOR_DIALOG };
	CString	m_strDetail;
	int		m_nDepth;
	float	m_fDefaultHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSectorCreateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSectorCreateDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECTORCREATEDLG_H__305079D8_F34D_4370_BA51_2234D7494A2B__INCLUDED_)
