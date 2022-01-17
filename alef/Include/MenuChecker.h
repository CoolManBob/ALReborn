// MenuChecker.h
// Menu Item Enable / Disable Class
// 2003.08.18 steeple

class CMenuChecker
{
private:
	// Menu
	CMenu* m_pcsMenu;

	// File Section
	BOOL m_bOpenRepository;
	BOOL m_bSetWorkingFolder;
	BOOL m_bAddFile;
	BOOL m_bAddFolder;
	BOOL m_bDelete;

	// Resource Keeper Section
	BOOL m_bGetLatestVersion;
	BOOL m_bCheckOut;
	BOOL m_bCheckIn;
	BOOL m_bUndoCheckOut;
	BOOL m_bRefresh;
	BOOL m_bShowHistory;

	// Admin Section
	BOOL m_bCreateRepository;
	BOOL m_bDeleteRepository;
	BOOL m_bUserManager;

public:
	CMenuChecker() {;}
	virtual ~CMenuChecker() {;}

	void Init();
	void SetMenu(CMenu* pcsMenu);

	// File Section
	void SetOpenRepository(BOOL bFlag) { m_bOpenRepository = bFlag; }
	void SetWorkingFolder(BOOL bFlag) { m_bOpenRepository = bFlag; }
	void SetAddFile(BOOL bFlag) { m_bAddFile = bFlag; }
	void SetAddFolder(BOOL bFlag) { m_bAddFolder = bFlag; }
	void SetDelete(BOOL bFlag) { m_bDelete = bFlag; }

	// Resource Keeper Section
	void SetGetLatestVersion(BOOL bFlag) { m_bGetLatestVersion = bFlag; }
	void SetCheckOut(BOOL bFlag) { m_bCheckOut = bFlag; }
	void SetCheckIn(BOOL bFlag) { m_bCheckIn = bFlag; }
	void SetUndoCheckOut(BOOL bFlag) { m_bUndoCheckOut = bFlag; }
	void SetRefresh(BOOL bFlag) { m_bRefresh = bFlag; }
	void SetShowHistory(BOOL bFlag) { m_bShowHistory = bFlag; }

	// Admin Section
	void SetCreateRepository(BOOL bFlag) { m_bCreateRepository = bFlag; }
	void SetDeleteRepository(BOOL bFlag) { m_bDeleteRepository = bFlag; }
	void SetUserManager(BOOL bFlag) { m_bUserManager = bFlag; }

	// File Section
	BOOL GetOpenRepository() { return m_bOpenRepository; }
	BOOL GetWorkingFolder() { return m_bOpenRepository; }
	BOOL GetAddFile() { return m_bAddFile; }
	BOOL GetAddFolder() { return m_bAddFolder; }
	BOOL GetDelete() { return m_bDelete; }

	// Resource Keeper Section
	BOOL GetLatestVersion() { return m_bGetLatestVersion; }
	BOOL GetCheckOut() { return m_bCheckOut; }
	BOOL GetCheckIn() { return m_bCheckIn; }
	BOOL GetUndoCheckOut() { return m_bUndoCheckOut; }
	BOOL GetRefresh() { return m_bRefresh; }
	BOOL GetShowHistory() { return m_bShowHistory; }

	// Admin Section
	BOOL GetCreateRepository() { return m_bCreateRepository; }
	BOOL GetDeleteRepository() { return m_bDeleteRepository; }
	BOOL GetUserManager() { return m_bUserManager; }

	// Process
	void ReloadAll();
	void ReloadFileSection();
	void ReloadResourceKeeperSection();
	void ReloadAdminSection();

	void SetAdmin(BOOL bFlag = TRUE);
};