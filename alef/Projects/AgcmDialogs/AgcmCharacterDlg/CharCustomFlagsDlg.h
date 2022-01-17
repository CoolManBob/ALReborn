#pragma once

#include "..\resource.h"

class CCharCustomFlagsDlg : public CDialog
{
public:
	CCharCustomFlagsDlg(UINT32 *pulCustomFlags, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CCharCustomFlagsDlg)
	enum { IDD = IDD_CHAR_CUSTOM_FLAGS };
	BOOL	m_bPlayMovingSound;
	//}}AFX_DATA

protected:
	UINT32		*m_pulCustomFlags;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//{{AFX_MSG(CCharCustomFlagsDlg)
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};