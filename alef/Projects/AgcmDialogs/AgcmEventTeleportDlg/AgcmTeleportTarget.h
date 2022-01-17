#if !defined(AFX_AGCMTELEPORTTARGET_H__42966CEF_FBA7_4966_B1FD_7644867F6B5E__INCLUDED_)
#define AFX_AGCMTELEPORTTARGET_H__42966CEF_FBA7_4966_B1FD_7644867F6B5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmTeleportTarget.h : header file
//

#include "../Resource.h"

#include "AgcModule.h"
#include "ApmEventManager.h"
#include "AgpmEventTeleport.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportTarget dialog

class AFX_EXT_CLASS AgcmTeleportTarget : public CDialog
{
public:
	AgcmTeleportTarget(AgpmEventTeleport *pcsAgpmEventTeleport, CWnd* pParent = NULL);   // standard constructor

	BOOL	InitData(ApdEvent *pstEvent, AgpdTeleportPoint *pcsPoint);
	BOOL	InitDialogData();

private:
	AgpmEventTeleport *	m_pcsAgpmEventTeleport;

// Construction
public:
// Dialog Data
	//{{AFX_DATA(AgcmTeleportTarget)
	enum { IDD = IDD_TELEPORT_TARGET };
	CListBox	m_csPointTarget;
	CListBox	m_csPointGroup;
	CListBox	m_csGroupList;
	float		m_fX;
	float		m_fY;
	float		m_fZ;
	UINT		m_ulIndex;
	int			m_nBaseType;
	long		m_lID;
	float		m_fRadiusMin;
	float		m_fRadiusMax;
	BOOL	m_bTypePos;
	BOOL	m_bTypeIndex;
	BOOL	m_bTypeBase;
	CString	m_strDescription;
	CString	m_strPointName;
	BOOL	m_bSiegeWarOnly;
	BOOL	m_bUseTypeHuman;
	BOOL	m_bUseTypeOrc;
	BOOL	m_bUseTypeMoonElf;
	//}}AFX_DATA

	AgpdTeleportPoint *	m_pcsPoint;
	ApdEvent *			m_pstEvent;

public:
	virtual BOOL Create(CWnd* pParent);
	virtual void OnOK();
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(AgcmTeleportTarget)
	afx_msg void OnTeleportTypeBase();
	afx_msg void OnTeleportTypePos();
	afx_msg void OnTeleportAddGroup();
	afx_msg void OnTeleportRemoveGroup();
	afx_msg void OnTeleportAddTarget();
	afx_msg void OnTeleportRemoveTarget();
	afx_msg void OnTeleportGroupAdd();
	afx_msg void OnTeleportGroupEdit();
	afx_msg void OnTeleportGroupDelete();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMTELEPORTTARGET_H__42966CEF_FBA7_4966_B1FD_7644867F6B5E__INCLUDED_)
