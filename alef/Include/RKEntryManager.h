// RKEntryManager.h
// CRKWriter, CRKParser Wrapper Class
// 2003.07.30 steeple

class CRKParser;
class CRKWriter;

class CRKEntryManager
{
private:
	CRKParser m_csRKEntryParser;
	CRKParser m_csRKHistoryParser;

	CRKWriter m_csRKEntryWriter;
	CRKWriter m_csRKHistoryWriter;

	stRKEntryInfo m_stRKEntryInfo;
	stRKHistoryInfo m_stRKHistoryInfo;

	CString m_szRemoteDrive;	// C, D, E 이런 드라이브가 올수도 있고, \\NHN 이런식으로 올 수도 있다.

public:
	CRKEntryManager();
	~CRKEntryManager();

	void Init();

	void ClearEntryInfo();
	void ClearHistoryInfo();

	void SetRemoteDirve(CString& szDrive) { m_szRemoteDrive = szDrive; }
	void SetEntryParserFileName(CString& szFileName);
	void SetHistoryParserFileName(CString& szFileName);
	void SetEntryWriterFileName(CString& szFileName);
	void SetHistoryWriterFileName(CString& szFileName);

	CString& GetRemoteDrive() { return m_szRemoteDrive; }
	void GetFileNameByFilePath(CString& szFilePath, CString& szFileName);

	BOOL GetEntryFileInfo(CString& szFileName, stRKEntryInfo* pstRKEntryInfo = NULL);
	//BOOL GetHistoryFileInfo(CString& szFileName, stRKHistoryInfo& stRKHistoryInfo); // 이건 좀 나중에 생각 더 해서 구현

	stRKEntryInfo* GetEntryData() { return &m_stRKEntryInfo; }
	stRKHistoryInfo* GetHistoryData() { return &m_stRKHistoryInfo; }



	BOOL AddEntry(CString& szType, CString& szFileName, INT32 lFileSize, CString& szDate, INT32 lVer);	// szFileName 에는 PATH 안!!!!! 포함이다.
	BOOL AddEntry(CString& szFileName);	// szFileName 에는 PATH 도 포함이다.

	BOOL CheckLatestVersion(CString& szLocalFileName, CString& szFileName);	// szLocalFileName, szFileName 에는 PATH 도 포함이다.
	BOOL CheckOut(CString& szLocalFileName, CString& szFileName, CString& szOwner);	// szLocalFileName, szFileName 에는 PATH 도 포함이다.
	BOOL CheckIn(CString& szLocalFileName, CString& szFileName, CString& szOwner, BOOL bRealCopy = TRUE);	// szLocalFileName, szFileName 에는 PATH 도 포함이다.
	BOOL UndoCheckOut(CString& szLocalFileName, CString& szFileName, CString& szOwner);	// szLocalFileName, szFileName 에는 PATH 도 포함이다.

	BOOL GetEntryList(CString& szFileName, CList<stRKEntryInfo, stRKEntryInfo>& csList);	// szFileName 에는 PATH 도 포함이다.
	BOOL GetHistoryList(CString& szFileName, CList<stRKHistoryInfo, stRKHistoryInfo>& csList);	// szFileName 에는 PATH 도 포함이다.
	CString GetBackupFilePath(CString& szFileName, INT32 lVer);	// szFileName 에는 PATH 도 포함이다.

	BOOL RemoveEntry(CString& szFileName, CString& szOwner);	// szFileName 에는 PATH 도 포함이다.
	BOOL RemoveDirectoryEntry(CString& szFileName, CString& szOwner);	// szFileName 에는 PATH 도 포함이다.

	// 범용 유틸이 되어버림.. -0-
	void RemoveFile(CString& szFileName);	// 읽기 전용이면 풀고 지운다.
	void RemoveDirectoryNAllFiles(CString& szDirectoryName);	// 디렉토리의 모든 파일을 지운다. 디렉토리도 지운다. 서브도 지운다.
	void RemoveDirectoryFinalSlash(CString& szFileName);	// 디렉토리 이름에 마지막에 \ 가 있으면 지운다.
	void CreateSubDirectory(CString& szPath);	// Path 에 포함되어 있는 모든 Sub Directory 를 만든다.
	void GetDirectoryPath(CString& szFileName, CString& szPath);	// szFileName 에서 디렉토리 Path 를 뽑는다.
	void SetReadOnly(CString& szFileName, BOOL bFlag = TRUE);	// 읽기 전용으로 만든다.
	BOOL SetModifyTime(CString& szOriginFileName, CString& szTargetFileName);	// 2번째 파일의 수정날짜를 1번째 파일의 수정날짜로 세팅해준다.
	BOOL HistoryFile(CString szFileName, CString szDate, CString szOwner, INT32 lVer);	// szFileName 에는 PATH 도 포함이다.

protected:
	BOOL BackupFile(CString& szFileName, INT32 lVer);	// szFileName 에는 PATH 도 포함이다.

	BOOL RemoveBackupEntry(CString& szFileName);	// 파일이 지워졌을 때, 백업 쪽 처리를 해준다. szFileName 은 원본 PATH
	BOOL RemoveBackupDirectoryEntry(CString& szFileName);	// 디렉토리가 지워졌을 때, 백업 쪽 처리를 해준다. szFileName 은 원본 PATH

	BOOL TemporaryCheckOut(CString& szFileName, CString& szOwner);	// szFileName 에는 PATH 도 포함이다.
	BOOL UndoTemporaryCheckOut(CString& szFileName, CString& szOwner);	// szFileName 에는 PATH 도 포함이다.

	void GetBackupFilePathByFileName(CString& szFileName, CString& szNewPath);	// 원본 PATH 에서 Backup File Path 를 얻는다.
};