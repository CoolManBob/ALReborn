#if !defined(AFX_DDSCONVERTDLG_H__B61B7152_347C_4DB7_9C2D_51D898AE4045__INCLUDED_)
#define AFX_DDSCONVERTDLG_H__B61B7152_347C_4DB7_9C2D_51D898AE4045__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DDSConvertDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDDSConvertDlg dialog

class CDDSConvertDlg : public CDialog
{
// Construction
public:
	CDDSConvertDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDDSConvertDlg)
	enum { IDD = IDD_DDSCONVERT };
	CListBox	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDDSConvertDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDDSConvertDlg)
	afx_msg void OnConvert();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DDSCONVERTDLG_H__B61B7152_347C_4DB7_9C2D_51D898AE4045__INCLUDED_)
