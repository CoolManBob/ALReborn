#if !defined(AFX_AGCMADMINDLGXT_HELP_H__76A8CDCA_6F8A_4543_9AB5_708FBBD01FFF__INCLUDED_)
#define AFX_AGCMADMINDLGXT_HELP_H__76A8CDCA_6F8A_4543_9AB5_708FBBD01FFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_Help.h : header file
//

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Help dialog

class AgcmAdminDlgXT_Help : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_Help(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_Help)
	enum { IDD = IDD_HELP };
	CXTButton	m_csGoBtn;
	CXTButton	m_csChatSendBtn;
	CXTButton	m_csMemoSaveBtn;
	CXTButton	m_csCompleteBtn;
	CXTButton	m_csDeferBtn;
	CXTButton	m_csDeleteBtn;
	CXTButton	m_csDeliverBtn;
	CXTButton	m_csRejectBtn;
	CStatic	m_csCharInfoPlace;
	CListCtrl	m_csHelpListView;
	CString	m_szHelpMsg;
	CString	m_szHelpMemo;
	CString	m_szHelpChat;
	CString	m_szHelpChatSend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_Help)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;

	// Callback Function Pointer
	ADMIN_CB m_pfCBHelpComplete;
	ADMIN_CB m_pfCBHelpDefer;
	ADMIN_CB m_pfCBHelpMemo;
	ADMIN_CB m_pfCBCharInfo;
	ADMIN_CB m_pfCBChatting;

	stAgpdAdminHelp m_stProcessHelp;
	stAgpdAdminCharData m_stCharData;

	list<stAgpdAdminHelp*> m_listHelp;
	INT16 m_nSelectedIndex;
	
	// Grid
	CXTPPropertyGrid* m_pcsCharGrid;
	
	CCriticalSection m_csLock;

public:
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();
	
	void Lock();
	void Unlock();
	
	// Callback Function Regist
	BOOL SetCallbackHelpComplete(ADMIN_CB pfCallback);
	BOOL SetCallbackHelpDefer(ADMIN_CB pfCallback);
	BOOL SetCallbackHelpMemo(ADMIN_CB pfCallback);
	BOOL SetCallbackCharInfo(ADMIN_CB pfCallback);
	BOOL SetCallbackChatting(ADMIN_CB pfCallback);

	// Operation
	BOOL AddHelp(stAgpdAdminHelp* pstHelp);
	BOOL RemoveHelp(stAgpdAdminHelp* pstHelp);

	BOOL SetProcessHelp(stAgpdAdminHelp* pstHelp);	// Admin 이 지금부터 처리할 Help 이다.
	stAgpdAdminHelp* GetProcessHelp();	// Admin 이 지금 처리중인 Help

	BOOL SetCharInfo(stAgpdAdminCharData* pstCharData);
	BOOL SetCharSub(stAgpdAdminCharDataSub* pstCharDataSub);
	stAgpdAdminCharData* GetCharInfo();
	BOOL CheckReceivedCharInfo(stAgpdAdminCharData* pstCharData);	// 받은 CharData 와 요청한 것과 같은지 검사한다.

	BOOL ReceiveChatMsg(stAgpdAdminChatData* pstAdminChatData);	// 귓속말 받음.
	
	stAgpdAdminHelp* GetHelpInList(INT32 lCount);
	BOOL GetSubjectText(CHAR* szText);
	BOOL GetMemoText(CHAR* szText);

	BOOL ReceiveHelpCompleteResult(stAgpdAdminHelp* pstHelp);	// 처리 결과가 날라온다.
	BOOL ReceiveHelpCompleteResultMessage(stAgpdAdminHelp* pstHelp);	// Message Queue 에서 부른다.
	BOOL ReceiveHelpDefer(stAgpdAdminHelp* pstHelp);	// 보류 신청 결과가 날라온다.
	BOOL ReceiveHelpDeferMessage(stAgpdAdminHelp* pstHelp);	// Message Queue 에서 부른다.
	BOOL ReceiveHelpMemoResult(stAgpdAdminHelp* pstHelp);	// Memo 저장 결과가 날라온다.
	BOOL ReceiveHelpMemoResultMessage(stAgpdAdminHelp* pstHelp);	// Message Queue 에서 부른다.

	BOOL ShowHelpList();
	BOOL ShowCharGrid();
	BOOL ShowSubject();
	BOOL ShowMemo();

	BOOL ClearHelpList();
	BOOL ClearHelpListView();
	BOOL ClearCharGrid();
	BOOL ClearSubject();
	BOOL ClearMemo();

	// Chatting
	BOOL AddChatMsg(CHAR* szMsg);
	BOOL AddChatMsg(CString& szMsg);
	BOOL ClearChatMsg();
	
protected:
	BOOL OnInitHelpListView();
	BOOL OnInitCharGrid();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_Help)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBHelpComplete();
	afx_msg void OnBHelpDefer();
	afx_msg void OnBHelpDelete();
	afx_msg void OnBHelpDeliver();
	afx_msg void OnBHelpReject();
	afx_msg void OnClickLvHelpList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBHelpMemoSave();
	virtual void OnOK() {;}
	virtual void OnCancel() {;}
	afx_msg void OnBChatSend();
	afx_msg void OnBHelpGo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_HELP_H__76A8CDCA_6F8A_4543_9AB5_708FBBD01FFF__INCLUDED_)
