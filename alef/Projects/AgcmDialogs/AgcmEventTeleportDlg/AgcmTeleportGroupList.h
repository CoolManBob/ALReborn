#if !defined(AFX_AGCMTELEPORTGROUPLIST_H__73DCA077_37A0_4066_AA7D_C58CFD05CD5F__INCLUDED_)
#define AFX_AGCMTELEPORTGROUPLIST_H__73DCA077_37A0_4066_AA7D_C58CFD05CD5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmTeleportGroupList.h : header file
//

#include "../Resource.h"

#include "AgpmEventTeleport.h"

#include "AgcmTeleportGroup.h"

// TODO : 이거 H에만 있고 rc파일엔 없는 리소스로 만들어진 Dialog..
// 그냥 싹 지우면 돼는건가
#define IDD_TELEPORT_GROUP 0

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportGroupList dialog

class AgcmTeleportGroupList : public CDialog
{
private:
	AgpmEventTeleport		*m_pcsAgpmEventTeleport;

	CHAR					**m_pszSelectedGroupName;

// Construction
public:
	AgcmTeleportGroupList(CWnd* pParent = NULL);   // standard constructor

	BOOL					InitData(AgpmEventTeleport *pcsAgpmEventTeleport, CHAR **pszSelectedGroupName);

// Dialog Data
	//{{AFX_DATA(AgcmTeleportGroupList)
	enum { IDD = IDD_TELEPORT_GROUP };
	CListBox	m_csListGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmTeleportGroupList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	BOOL					OnInitDialog();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmTeleportGroupList)
	afx_msg void OnButtonCreateGroup();
	afx_msg void OnButtonDeleteGroup();
	afx_msg void OnButtonEditGroup();
	afx_msg void OnSelect();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMTELEPORTGROUPLIST_H__73DCA077_37A0_4066_AA7D_C58CFD05CD5F__INCLUDED_)
