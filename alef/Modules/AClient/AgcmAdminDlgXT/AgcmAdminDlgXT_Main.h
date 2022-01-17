#if !defined(AFX_AGCMADMINDLGXT_MAIN_H__7F98C558_5636_4CF1_97DD_43E9ABD96503__INCLUDED_)
#define AFX_AGCMADMINDLGXT_MAIN_H__7F98C558_5636_4CF1_97DD_43E9ABD96503__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_Main.h : header file
//

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Main dialog

class AgcmAdminDlgXT_Main : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_Main(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_Main)
	enum { IDD = IDD_MAIN };
	CStatic	m_csCurrentUserCount;
	CXTFlatComboBox	m_csMovePlaceCB;
	CXTButton	m_csMoveBtn;
	CListBox m_csObjectListBox;
	CString m_szInfoText;
	CListCtrl m_csHelpListView;
	CString	m_szMovePC;
	CString	m_szMoveTargetPC;
	CString	m_szMoveX;
	CString	m_szMoveY;
	CString	m_szMoveZ;
	CString	m_szMovePlace;
	CString	m_szAdminCount;
	CString	m_szHelpCount;
	CString	m_szCurrentUserCount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_Main)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;
	
	ADMIN_CB m_pfCBSearch;
	ADMIN_CB m_pfCBHelpProcess;

	list<stAgpdAdminPickingData*> m_listObject;

	stAgpdAdminCharData m_stCharData;

	stAgpdAdminHelp m_stSelectedHelp;	// 한번 클릭으로 셀렉트 한 놈.
	stAgpdAdminHelp m_stProcessHelp;	// 두번 클릭으로 프로세싱 하는 놈.
	list<stAgpdAdminHelp*> m_listHelp;
	
	CCriticalSection m_csLock;
	CCriticalSection m_csHelpLock;

	ADMIN_CB m_pfCBHelpRequest;	// Debug 용

public:
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();

	// Callback Function Regist
	BOOL SetCBSearch(ADMIN_CB pfCBSearch);
	BOOL SetCBHelpProcess(ADMIN_CB pfCBHelpProcess);
	BOOL SetCBHelpRequest(ADMIN_CB pfCBHelpRequest);

	// Operation
	void Lock();
	void Unlock();
	void HelpLock();
	void HelpUnlock();

	BOOL AddObject(stAgpdAdminPickingData* pstPickingData);
	BOOL GetSelectedObjectID(LPCTSTR szName);
	BOOL GetSelectedObjectID(CString& szName);
	stAgpdAdminPickingData* GetObject(LPCTSTR szName);
	BOOL RemoveObject(LPCTSTR szName);
	BOOL RemoveObjectInListBox(LPCTSTR szName);
	BOOL ClearObjectList();
	BOOL ClearObjectListBox();
	BOOL ShowObjectList();

	BOOL SetInfoCharData(stAgpdAdminCharData* pstCharData);
	BOOL SetInfoCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub);
	BOOL ClearInfoText();
	BOOL ShowInfoText();

	// Move
	BOOL SetPosition(float fX, float fY, float fZ);
	BOOL AddMovePlace(CString& szMovePlace);
	BOOL ClearMovePlace();

	// Help
	BOOL AddHelp(stAgpdAdminHelp* pstHelp);
	BOOL ProcessHelp(stAgpdAdminHelp* pstHelp);
	BOOL RemoveHelp(stAgpdAdminHelp* pstHelp);

	BOOL SetSelectedHelp(stAgpdAdminHelp* pstHelp);	// ListView 에서 선택한 Help 를 세팅한다.
	stAgpdAdminHelp* GetSelectedHelp();	// ListView 에서 선택한 Help
	BOOL SetProcessHelp(stAgpdAdminHelp* pstHelp);	// 자신이 처리할 Help 를 세팅한다.
	stAgpdAdminHelp* GetProcessHelp();	// 자신이 처리중인 Help

	stAgpdAdminHelp* GetHelp(INT32 lHelpID);	// From List
	BOOL RequestHelp();	// For Debug - 2003.10.29.

	BOOL SendProcessHelp(stAgpdAdminHelp& stHelp);	// 레퍼런스 타입으로 변경

	// Help List
	BOOL ShowHelpList();
	BOOL ClearHelpList();
	BOOL ClearHelpListView();
	
	// Status Text
	BOOL SetCurrentUserCount(INT32 lCurrentUser);
	INT32 GetCurrentUserCount();

	BOOL UpdateCurrentUserCount();
	BOOL UpdateAdminCount();
	BOOL UpdateHelpCount();

protected:
	BOOL OnInitHelpListView();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_Main)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSelchangeObjectList();
	afx_msg void OnToolSearch();
	afx_msg void OnToolCharacter();
	afx_msg void OnToolHelp();
	afx_msg void OnToolMove();
	afx_msg void OnClickLvMainHelp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkLvMainHelp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBMainMoveGo();
	afx_msg void OnObjectClear();
	afx_msg void OnDblclkLsMainObject();
	afx_msg void OnObjectDel();
	afx_msg void OnObjectSelect();
	afx_msg void OnHelpSelect();
	afx_msg void OnSelchangeCbMainMovePlace();
	virtual void OnOK() {;}
	virtual void OnCancel() {;}
	afx_msg void OnToolItem();
	afx_msg void OnToolAdminList();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg void OnToolNotice();
	//}}AFX_MSG
	// For XT Menu Bar

//	CXTPMenuBar m_csMenuBar;

//	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

//	afx_msg LRESULT OnKickIdle(WPARAM wp, LPARAM lCount);
//	afx_msg void OnEmptyCommand();
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_MAIN_H__7F98C558_5636_4CF1_97DD_43E9ABD96503__INCLUDED_)
