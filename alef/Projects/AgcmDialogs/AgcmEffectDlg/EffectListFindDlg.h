#pragma once

class CEffectListFindDlg : public CDialog
{
public:
	CEffectListFindDlg(CHAR *pszDest, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CEffectListFindDlg)
	enum { IDD = IDD_EFFECT_LIST_FIND };
	CString	m_strFind;
	//}}AFX_DATA

protected:
	CHAR	*m_pszDest;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};