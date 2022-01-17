#if !defined(AFX_EFFECTEDITDLG_H__4853429D_F53B_44CF_8ACD_11EC44875AA1__INCLUDED_)
#define AFX_EFFECTEDITDLG_H__4853429D_F53B_44CF_8ACD_11EC44875AA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffectEditDlg.h : header file
//
#include "PortionStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CEffectEditDlg dialog

class CEffectEditDlg : public CDialog
{
// Construction
public:
	CEffectEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEffectEditDlg)
	enum { IDD = IDD_EFFECT_EDIT_DIALOG };
	CPortionStatic	m_ctlPortion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEffectEditDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTEDITDLG_H__4853429D_F53B_44CF_8ACD_11EC44875AA1__INCLUDED_)
