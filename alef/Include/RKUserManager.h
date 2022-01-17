// RKUserManger.h
// User Manager Class
// 2003.08.05 steeple

class CPKParser;

typedef struct stUserInfo
{
	CString szUserName;
	CString szPassword;
	BOOL	bAdmin;
} stUserInfo;

class CRKUserManager
{
private:
	CRKParser m_csParser;
	CRKWriter m_csWriter;

	CString m_szUserName;
	CString m_szPassword;
	CString m_szRemotePath;
	BOOL	m_bAdmin;

	INT8 m_cStatus;

public:
	CRKUserManager();
	~CRKUserManager();

	void Init();

	void SetUserName(CString& szUserName) { m_szUserName = szUserName; }
	void SetPassword(CString& szPassword) { m_szPassword = szPassword; }
	void SetRemotePath(CString& szRemotePath) { m_szRemotePath = szRemotePath; }

	CString& GetUserName() { return m_szUserName; }
	CString& GetPassword() { return m_szPassword; }
	INT8 GetStatus() { return m_cStatus; }

	BOOL Login(CString& szUserName, CString& szPassword);
	BOOL Login();

	BOOL Logout();

	// Admin
	BOOL IsAdmin() { return m_bAdmin; }
	BOOL AddUser(CString& szUserName, CString& szPassword, CString& szRemotePath, BOOL bAdmin = FALSE);
	BOOL RemoveUser(CString& szUserName, CString& szRemotePath);
	INT32 GetUserList(CList<stUserInfo, stUserInfo>& csList, CString& szRemotePath);	// 해당 Repository 의 User List 를 얻는다.
	BOOL SetAdmin(CString& szUserName, BOOL bAdmin, CString& szRemotePath);

protected:
	void SetStatus(INT8 cStatus) { m_cStatus = cStatus; }
};