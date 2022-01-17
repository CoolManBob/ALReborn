// AgcmAdminDlgMain.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 15.

class __declspec(dllexport) AgcmAdminDlgMain : public AgcmAdminDlgBase
{
protected:
	// Callback Function Pointer
	ADMIN_CB m_pfCBSearch;
	ADMIN_CB m_pfCBHelpProcess;

	// Object List
	AuList<stAgpdAdminPickingData*> m_listObject;

	stAgpdAdminCharData m_stCharData;
	char* m_szInfoText;

	stAgpdAdminHelp m_stProcessHelp;
	AuList<stAgpdAdminHelp*> m_listHelp;
	
	CRITICAL_SECTION m_csHelpLock;

	ADMIN_CB m_pfCBHelpRequest;	// Debug 용

public:
	AgcmAdminDlgMain();
	virtual ~AgcmAdminDlgMain();

	// Callback Function Regist
	BOOL SetCBSearch(ADMIN_CB pfCBSearch);
	BOOL SetCBHelpProcess(ADMIN_CB pfCBHelpProcess);
	BOOL SetCBHelpRequest(ADMIN_CB pfCBHelpRequest);

	// Operation
	void HelpLock();
	void HelpUnlock();
	
	BOOL AddObject(stAgpdAdminPickingData* pstPickingData);
	BOOL GetSelectedObjectID(HWND hDlg, LPCTSTR szName);
	BOOL DeleteObject(stAgpdAdminPickingData* pstPickingData);
	BOOL ClearObjectList();
	BOOL ClearObjectListBox(HWND hDlg = NULL);
	BOOL ShowObjectList(HWND hDlg = NULL);

	BOOL SetInfoCharData(stAgpdAdminCharData* pstCharData);
	BOOL SetInfoCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub);
	BOOL SetInfoText(LPCTSTR szName, LPCTSTR pszText);
	BOOL ClearInfoText();
	BOOL ShowInfoText(HWND hDlg = NULL);

	// Help
	BOOL AddHelp(stAgpdAdminHelp* pstHelp);
	BOOL ProcessHelp(stAgpdAdminHelp* pstHelp);
	BOOL RemoveHelp(stAgpdAdminHelp* pstHelp);
	BOOL SetProcessHelp(stAgpdAdminHelp* pstHelp);	// 자신이 처리할 Help 를 세팅한다.
	stAgpdAdminHelp* GetProcessHelp();	// 자신이 처리중인 Help
	stAgpdAdminHelp* GetHelp(INT32 lHelpID);	// From List
	BOOL RequestHelp();	// For Debug - 2003.10.29.

	// List
	BOOL ShowHelpList(HWND hDlg = NULL);
	BOOL ClearHelpList();
	BOOL ClearHelpListView(HWND hDlg = NULL);

	// Dialog Proc
	static LRESULT CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

protected:
	// Initialize
	LRESULT OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);
	LRESULT OnInitHelpListView(HWND hDlg);

	// Menu
	LRESULT OnCloseMenu(HWND hDlg);
	LRESULT OnSearchMenu(HWND hDlg);
	LRESULT OnCharacterMenu(HWND hDlg);
	LRESULT OnHelpMenu(HWND hDlg);

	// Button
	LRESULT OnDelBtn(HWND hDlg);
	LRESULT OnClearBtn(HWND hDlg);
	LRESULT OnGoBtn(HWND hDlg);
	LRESULT OnModifyBtn(HWND hDlg);
	LRESULT OnHelpBtn(HWND hDlg);

	// Notify
	LRESULT OnObjectListClk(HWND hDlg);
	LRESULT OnObjectListDblClk(HWND hDlg);

	// Help
	LRESULT OnHelpListDblClk(HWND hHelpList, LPNMHDR hdr, LPNMLISTVIEW nlv);
};