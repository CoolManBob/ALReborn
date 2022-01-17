#pragma once

#include "..\resource.h"

class CCharacterTemplateListDlg : public CDialog
{
public:
	CCharacterTemplateListDlg(INT32	*plIndex, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CCharacterTemplateListDlg)
	enum { IDD = IDD_CHAR_TEMPLATE_LIST };
	CListBox	m_csCharTemplateList;
	//}}AFX_DATA

protected:
	INT32*		m_plIndex;


protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//{{AFX_MSG(CCharacterTemplateListDlg)
	afx_msg void OnDblclkListCharTemplate();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

