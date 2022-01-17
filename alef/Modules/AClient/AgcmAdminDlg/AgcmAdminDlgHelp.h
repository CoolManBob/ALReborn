// AgcmAdminDlgHelp.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 19.

// Help Dialog Class
class __declspec(dllexport) AgcmAdminDlgHelp : public AgcmAdminDlgBase
{
protected:
	// Callback Function Pointer
	ADMIN_CB m_pfCBHelpComplete;
	ADMIN_CB m_pfCBCharInfo;
	ADMIN_CB m_pfCBChatting;

	// Member Data
	stAgpdAdminHelp m_stProcessHelp;
	stAgpdAdminCharData m_stCharData;

	AuList<stAgpdAdminHelp*> m_listHelp;
	INT16 m_nSelectedIndex;

	char* m_szChatMsg;

public:
	AgcmAdminDlgHelp();
	virtual ~AgcmAdminDlgHelp();

	// Callback Function Regist
	BOOL SetCallbackHelpComplete(ADMIN_CB pfCallback);
	BOOL SetCallbackCharInfo(ADMIN_CB pfCallback);
	BOOL SetCallbackChatting(ADMIN_CB pfCallback);

	// Operation
	BOOL AddHelp(stAgpdAdminHelp* pstHelp);
	BOOL RemoveHelp(stAgpdAdminHelp* pstHelp);

	BOOL SetProcessHelp(stAgpdAdminHelp* pstHelp);	// Admin 이 지금부터 처리할 Help 이다.
	stAgpdAdminHelp* GetProcessHelp();	// Admin 이 지금 처리중인 Help

	BOOL SetCharInfo(stAgpdAdminCharData* pstCharData);
	stAgpdAdminCharData* GetCharInfo();
	BOOL CheckReceivedCharInfo(stAgpdAdminCharData* pstCharData);	// 받은 CharDat 와 요청한 것과 같은지 검사한다.

	BOOL ReceiveChatMsg(stAgpdAdminChatData* pstAdminChatData);	// 귓속말 받음.
	
	stAgpdAdminHelp* GetHelpInList(INT32 lCount);
	BOOL GetSubjectText(CHAR* szText);
	BOOL GetMemoText(CHAR* szText);

	BOOL ReceiveHelpCompleteResult(stAgpdAdminHelp* pstHelp);	// 처리 결과가 날라온다.

	BOOL ShowHelpList(HWND hDlg = NULL);
	BOOL ShowCharInfo(HWND hDlg = NULL);
	BOOL ShowSubject(HWND hDlg = NULL);
	BOOL ShowMemo(HWND hDlg = NULL);

	BOOL ClearHelpList();
	BOOL ClearHelpListView(HWND hDlg = NULL);
	BOOL ClearCharInfoView(HWND hDlg = NULL);
	BOOL ClearSubject(HWND hDlg = NULL);
	BOOL ClearMemo(HWND hDlg = NULL);

	// Chatting
	BOOL AddChatMsg(CHAR* szMsg);
	BOOL ClearChatMsg(HWND hDlg = NULL);

	// Dialog Proc
	static LRESULT CALLBACK HelpDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

protected:
	// Initialize
	LRESULT OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);

	// Initialize Help List View
	LRESULT OnInitHelpLV(HWND hDlg);

	// Initiallze Char Info List View
	LRESULT OnInitCharInfoLV(HWND hDlg);

	// ListView
	LRESULT OnHelpListViewClk(HWND hHelpList, LPNMHDR hdr, LPNMLISTVIEW nlv);

	// Button
	LRESULT OnRefreshBtn(HWND hDlg);

	LRESULT OnGoBtn(HWND hDlg);
	LRESULT OnModifyBtn(HWND hDlg);
	LRESULT OnGMBtn(HWND hDlg);
	LRESULT OnXYZBtn(HWND hDlg);

	LRESULT OnCompleteBtn(HWND hDlg, INT8 nStatus = AGPMADMIN_HELP_STATUS_COMPLETE);
	LRESULT OnDeferBtn(HWND hDlg);
	LRESULT OnDeliverBtn(HWND hDlg);
	LRESULT OnRejectBtn(HWND hDlg);
	LRESULT OnDeleteBtn(HWND hDlg);

	LRESULT OnSaveBtn(HWND hDlg);
	LRESULT OnCancelBtn(HWND hDlg);

	LRESULT OnChatting(HWND hDlg);
};