// AgcmAdminDlgXT_IniManager.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 01. 03.

// Admin Configuration 저장/로드 를 관리한다.
// AuIniManager 의 특성상 한번에 다 읽고, 한번에 다 써야 한다. -0-

class AgcmAdminDlgXT_IniManager
{
private:
	CString m_szPath;

public:
	AgcmAdminDlgXT_IniManager();
	virtual ~AgcmAdminDlgXT_IniManager();

	BOOL SetPath(CHAR* szPath);

	BOOL Load();	// 한번 로드할 때 전체를 다 로드함.
	BOOL LoadMovePlace(AuIniManager* pcsIniManager, INT32 lSection);

	BOOL Save();	// 한번 세이브 할때 전체를 다 세이브 함.
	BOOL SaveMovePlace(AuIniManager* pcsIniManager);
};