#if !defined(AFX_EVENTNATURESETTINGDLG_H__DA40C80E_A6D9_4D3D_B57F_2120BE48C10E__INCLUDED_)
#define AFX_EVENTNATURESETTINGDLG_H__DA40C80E_A6D9_4D3D_B57F_2120BE48C10E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventNatureSettingDlg.h : header file
//

#include "AgcmEventNatureDlg.h"

class CEventNatureSettingDlg : public CDialog
{
// Construction
public:
	CEventNatureSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventNatureSettingDlg)
	enum { IDD = IDD_EVENT_NATURE_DLG };
	CComboBox	m_ctlWeatherCombo;
	float	m_fRange		;
	//}}AFX_DATA

protected:
	// Data members..
	AgcmEventNatureDlg	*	m_csAgcmEventNatureDlg	;
	ApdEvent			*	m_pstEvent				;

public:
	// Operations..
	UINT	DoModal( AgcmEventNatureDlg * pParent , ApdEvent * pstEvent );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventNatureSettingDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventNatureSettingDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTNATURESETTINGDLG_H__DA40C80E_A6D9_4D3D_B57F_2120BE48C10E__INCLUDED_)
