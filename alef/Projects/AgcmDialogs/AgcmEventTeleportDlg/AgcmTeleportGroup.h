#if !defined(AFX_AGCMTELEPORTGROUP_H__33424E45_A0C1_4DF3_B4A4_76B0FB9AA482__INCLUDED_)
#define AFX_AGCMTELEPORTGROUP_H__33424E45_A0C1_4DF3_B4A4_76B0FB9AA482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmTeleportGroup.h : header file
//

#include "../Resource.h"
#include "AgpmEventTeleport.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportGroup dialog

class AgcmTeleportGroup : public CDialog
{
private:
	AgpmEventTeleport		*m_pcsAgpmEventTeleport;
	AgpdTeleportGroup		*m_pcsGroup;

// Construction
public:
	AgcmTeleportGroup(CWnd* pParent = NULL);   // standard constructor

	BOOL					OnInitDialog();

	BOOL					InitDialogData();
	BOOL					InitData(AgpmEventTeleport *pcsAgpmEventTeleport, AgpdTeleportGroup *pcsGroup);

// Dialog Data
	//{{AFX_DATA(AgcmTeleportGroup)
	enum { IDD = IDD_TELEPORT_EDIT_GROUP };
	CListBox	m_csListWholePoint;
	CListBox	m_csListGroupPoint;
	CString		m_strDescription;
	CString		m_strGroupName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmTeleportGroup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmTeleportGroup)
	afx_msg void OnButtonCreatePoint();
	afx_msg void OnButtonDeletePoint();
	afx_msg void OnButtonEditPoint();
	afx_msg void OnButtonMovetoGroup();
	afx_msg void OnButtonMovetoPoint();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMTELEPORTGROUP_H__33424E45_A0C1_4DF3_B4A4_76B0FB9AA482__INCLUDED_)
