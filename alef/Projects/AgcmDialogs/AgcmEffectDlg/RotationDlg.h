#pragma once

#include "../resource.h"
#include "AuMathEx.h"

class CRotationDlg : public CDialog
{
public:
	CRotationDlg(AgcdUseEffectSetDataRotation **ppstRotation, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRotationDlg)
	enum { IDD = IDD_ROTATION };
	float	m_fAngleX;
	float	m_fAngleY;
	float	m_fAngleZ;
	//}}AFX_DATA

protected:
	AgcdUseEffectSetDataRotation **m_ppstRotation;

	BOOL	m_bFirstChanged;

	BOOL	ApplyData();

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	//{{AFX_MSG(CRotationDlg)
	afx_msg void OnBuutonApply();
	
	DECLARE_MESSAGE_MAP()
};
