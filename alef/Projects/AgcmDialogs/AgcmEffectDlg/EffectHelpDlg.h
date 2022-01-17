#pragma once

#include "../resource.h"

class CEffectHelpDlg : public CDialog
{
public:
	CEffectHelpDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CEffectHelpDlg)
	enum { IDD = IDD_EFFECT_HELP };
	CString	m_strHelp;
	//}}AFX_DATA

	VOID	InputString(CHAR *szString);

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//{{AFX_MSG(CEffectHelpDlg)
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};