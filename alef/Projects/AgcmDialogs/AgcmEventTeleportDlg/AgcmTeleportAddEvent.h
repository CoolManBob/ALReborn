#if !defined(AFX_AGCMTELEPORTADDEVENT_H__E0F37CDE_418A_490D_8068_31E28A7AEA2C__INCLUDED_)
#define AFX_AGCMTELEPORTADDEVENT_H__E0F37CDE_418A_490D_8068_31E28A7AEA2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmTeleportAddEvent.h : header file
//

#include "../Resource.h"

#include "AgpmEventTeleport.h"

#include "AgcmTeleportGroupList.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportAddEvent dialog

#define IDD_TELEPORT_ADD_EVENT 0

class AgcmTeleportAddEvent : public CDialog
{
private:
	AgpmEventTeleport			*m_pcsAgpmEventTeleport;

	ApdEvent					*m_pcsEvent;

	ApdEvent					m_csTempEvent;

// Construction
public:
	AgcmTeleportAddEvent(CWnd* pParent = NULL);   // standard constructor

	BOOL						InitData(AgpmEventTeleport *pcsAgpmEventTeleport, ApdEvent *pcsEvent);

// Dialog Data
	//{{AFX_DATA(AgcmTeleportAddEvent)
	enum { IDD = IDD_TELEPORT_ADD_EVENT };
	CListBox	m_csListGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmTeleportAddEvent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	BOOL					OnInitDialog();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmTeleportAddEvent)
	afx_msg void OnButtonAddGroup();
	afx_msg void OnButtonRemoveGroup();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMTELEPORTADDEVENT_H__E0F37CDE_418A_490D_8068_31E28A7AEA2C__INCLUDED_)
