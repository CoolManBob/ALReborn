#if !defined(AFX_AGCMADMINDLGXT_ADMINLIST_H__D6DC6AD5_BDD5_4B29_A7F5_20B4B0B56633__INCLUDED_)
#define AFX_AGCMADMINDLGXT_ADMINLIST_H__D6DC6AD5_BDD5_4B29_A7F5_20B4B0B56633__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_AdminList.h : header file
//

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_AdminList dialog

class AgcmAdminDlgXT_AdminList : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_AdminList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_AdminList)
	enum { IDD = IDD_ADMIN_LIST };
	CXTButton	m_csSelectBtn;
	CListCtrl	m_csAdminLV;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_AdminList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;

	list<stAgpdAdminInfo*> m_listAdmin;
	
	CCriticalSection m_csLock;

public:
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();

	void Lock();
	void Unlock();

	BOOL AddAdmin(stAgpdAdminInfo* pstAdminInfo);

	stAgpdAdminInfo* GetAdmin(CHAR* szName);
	stAgpdAdminInfo* GetAdmin(INT32 lCID);
	INT32 GetAdminCount();

	BOOL RemoveAdmin(stAgpdAdminInfo* pstAdminInfo);
	BOOL RemoveAdmin(CHAR* szName);
	BOOL RemoveAdmin(INT32 lCID);
	
	BOOL ClearAdminListData();

	BOOL ShowAdminList();
	BOOL ClearAdminList();

// Implementation
protected:
	BOOL InitAdminListView();
	
	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_AdminList)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnOK() {;}
	virtual void OnCancel() {;}
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_ADMINLIST_H__D6DC6AD5_BDD5_4B29_A7F5_20B4B0B56633__INCLUDED_)
