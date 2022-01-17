#if !defined(AFX_EFFECTLISTMODALDLG_H__25FCE484_A8DC_4CF3_AE2B_525EF7499FE4__INCLUDED_)
#define AFX_EFFECTLISTMODALDLG_H__25FCE484_A8DC_4CF3_AE2B_525EF7499FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffectListModalDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEffectListModalDlg dialog

class CEffectListModalDlg : public CDialog
{
// Construction
public:
	CEffectListModalDlg(UINT32 *pulEID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEffectListModalDlg)
	enum { IDD = IDD_EFFECT_LIST_MODAL };
	CListCtrl	m_csEffectList;
	//}}AFX_DATA

protected:
	UINT32 *m_pulEID;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectListModalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEffectListModalDlg)
	afx_msg void OnDblclkEffectList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTLISTMODALDLG_H__25FCE484_A8DC_4CF3_AE2B_525EF7499FE4__INCLUDED_)
