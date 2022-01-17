#if !defined(AFX_SETCHARLINKANIMDLG_H__AE0D414C_F5DF_4E01_A810_DDE5EEC585C8__INCLUDED_)
#define AFX_SETCHARLINKANIMDLG_H__AE0D414C_F5DF_4E01_A810_DDE5EEC585C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetCharLinkAnimDlg.h : header file
//

#include "../resource.h"

/////////////////////////////////////////////////////////////////////////////
// SetCharLinkAnimDlg dialog

class SetCharLinkAnimDlg : public CDialog
{
// Construction
public:
	SetCharLinkAnimDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SetCharLinkAnimDlg)
	enum { IDD = IDD_SET_CHAR_LINK_ANIM };
	CListBox	m_csLinkAnimList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SetCharLinkAnimDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SetCharLinkAnimDlg)
	afx_msg void OnButtonLaAdd();
	afx_msg void OnButtonLaRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETCHARLINKANIMDLG_H__AE0D414C_F5DF_4E01_A810_DDE5EEC585C8__INCLUDED_)
