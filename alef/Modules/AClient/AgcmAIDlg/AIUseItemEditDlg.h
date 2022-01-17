#if !defined(AFX_AIUSEITEMEDITDLG_H__2DC345B1_852D_4F82_9688_5D3D105B7F02__INCLUDED_)
#define AFX_AIUSEITEMEDITDLG_H__2DC345B1_852D_4F82_9688_5D3D105B7F02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AIUseItemEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAIUseItemEditDlg dialog

class CAIUseItemEditDlg : public CDialog
{
// Construction
public:
	CAIUseItemEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAIUseItemEditDlg)
	enum { IDD = IDD_DIALOG_AI_USE_ITEM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAIUseItemEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAIUseItemEditDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AIUSEITEMEDITDLG_H__2DC345B1_852D_4F82_9688_5D3D105B7F02__INCLUDED_)
