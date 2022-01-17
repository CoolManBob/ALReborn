// RKWriter.h
// .rk , .history file writer
// 2003.07.29 steeple

class CRKWriter
{
private:
	CStdioFile m_csFile;
	BOOL m_bOpened;

	CString m_szFileName;

public:
	CRKWriter();
	~CRKWriter();

	void Init();

	BOOL Open();
	BOOL Close();

	BOOL WriteEntry(CString& szType, CString& szFileName, INT32 lFileSize, CString& szDate, INT32 lVer);	// 등록할 때 만든다.
	BOOL WriteOwner(CString& szFileName, INT32 lSize, CString& szDate, CString& szOwner, INT32 lVer = 1);	// Check-Out, Check-In 하면 불러준다.
	BOOL WriteHistory(CString szFileName, CString szDate, CString szOwner, INT32 lVer);	// Check-In 하면 불러준다.
	BOOL RemoveEntry(CString& szType, CString& szFileName);	// 파일 삭제시.. 

	BOOL WriteRepository(CString& szName, CString& szPath, CString& szWorkingFolder, INT8 cDefault);	// Repository 등록 시 로컬 Info.rk 파일 쓸 때 사용.
	BOOL RemoveRepository(CString& szName);	// szName 이 있는 줄을 지운다.
	BOOL WriteRemotePath(CString& szName, CString& szPath);	// szName Repository 의 Path 를 변경한다.
	BOOL WriteWorkingFolder(CString& szName, CString& szWorkingFolder);	// szName Repository 의 WokingFolder 를 변경한다.
	BOOL WriteRepositoryDefault(CString& szName, BOOL bDefault);	// Default 세팅을 해준다.

	BOOL WriteUser(CString& szName, CString& szPassword, BOOL bAdmin, BOOL bAdd = TRUE);	// Repository 에 User 를 Add / Remove 해준다.
	BOOL WriteAdmin(CString& szName, BOOL bAdmin);	// User 의 Admin 을 설정한다.

	void SetFileName(CString& szFileName) { m_szFileName = szFileName; }
};
