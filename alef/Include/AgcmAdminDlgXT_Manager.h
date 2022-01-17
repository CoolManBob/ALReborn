// AgcmAdminDlgXT_Manager.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 12. 11.

// Singleton 으로 만든다.
class AgcmAdminDlgXT_Manager
{
private:
	static AgcmAdminDlgXT_Manager* _Instance;	// Singleton 객체

	PVOID m_pcsCBClass;	// Callback Class

	// Dialogs...
	AgcmAdminDlgXT_Main* m_pcsMainDlg;
	AgcmAdminDlgXT_Search* m_pcsSearchDlg;
	AgcmAdminDlgXT_Character* m_pcsCharacterDlg;
	AgcmAdminDlgXT_CharItem* m_pcsCharItemDlg;
	AgcmAdminDlgXT_Help* m_pcsHelpDlg;
	AgcmAdminDlgXT_Move* m_pcsMoveDlg;
	AgcmAdminDlgXT_Ban* m_pcsBanDlg;
	AgcmAdminDlgXT_Item* m_pcsItemDlg;
	AgcmAdminDlgXT_AdminList* m_pcsAdminListDlg;
	AgcmAdminDlgXT_Notice* m_pcsNoticeDlg;
	
	// 2004.12.01. Renewal
	AgcmAdminDlgXT_Game* m_pcsGameDlg;
	
	// Message Queue
	AgcmAdminDlgXT_MessageQueue* m_pcsMessageQueue;
	
	// Resource Loader
	AgcmAdminDlgXT_ResourceLoader* m_pcsResourceLoader;

	// IniManager
	AgcmAdminDlgXT_IniManager* m_pcsIniManager;

	// Self Admin Info
	stAgpdAdminInfo m_stSelfAdminInfo;

protected:
	AgcmAdminDlgXT_Manager();

public:
	virtual ~AgcmAdminDlgXT_Manager();

	// Instance
	static AgcmAdminDlgXT_Manager* Instance();

	// Set
	BOOL SetCBClass(PVOID pClass);
	BOOL SetSelfAdminInfo(stAgpdAdminInfo* pstAdminInfo);

	// Get
	PVOID GetCBClass();
	stAgpdAdminInfo* GetSelfAdminInfo();

	AgcmAdminDlgXT_Main* GetMainDlg();
	AgcmAdminDlgXT_Search* GetSearchDlg();
	AgcmAdminDlgXT_Character* GetCharDlg();
	AgcmAdminDlgXT_CharItem* GetCharItemDlg();
	AgcmAdminDlgXT_Help* GetHelpDlg();
	AgcmAdminDlgXT_Move* GetMoveDlg();
	AgcmAdminDlgXT_Ban* GetBanDlg();
	AgcmAdminDlgXT_Item* GetItemDlg();
	AgcmAdminDlgXT_AdminList* GetAdminListDlg();
	AgcmAdminDlgXT_Notice* GetNoticeDlg();

	// 2004.12.01. Renewal
	AgcmAdminDlgXT_Game* GetGameDlg();

	AgcmAdminDlgXT_MessageQueue* GetMessageQueue();

	AgcmAdminDlgXT_ResourceLoader* GetResourceLoader();
	
	AgcmAdminDlgXT_IniManager* GetIniManager();
	
	// Dialog
	BOOL IsDialogMessage(LPMSG lpMsg);	// Window 메시지가 ADMIN Dialog Message 인지 검사한다.
	BOOL OpenMainDlg();
	BOOL OpenSearchDlg();
	BOOL OpenCharacterDlg();
	BOOL OpenCharItemDlg();
	BOOL OpenHelpDlg();
	BOOL OpenMoveDlg();
	BOOL OpenBanDlg();
	BOOL OpenItemDlg();
	BOOL OpenAdminListDlg();
	BOOL OpenNoticeDlg();

	// 2004.12.01. Renewal
	BOOL IsDLGMSG(LPMSG lpMsg);
	BOOL OpenGameDlg();
	
	BOOL OpenAllWindows(INT nShowCmd = SW_HIDE);

	// Operation
	void GetDateTimeByTimeStamp(INT32 lTimeStamp, char* szDate, INT32 lSize = 128);
};