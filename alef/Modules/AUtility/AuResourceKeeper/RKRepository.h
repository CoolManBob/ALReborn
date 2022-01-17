// RKRepository.h
// Repository 를 생성, 삭제 관리를 한다.
// 2003.08.05 steeple

class CRKParser;
class CRKWriter;

typedef struct stRepositoryData
{
	CString szName;
	CString szPath;
	CString szWorkingFolder;
} stRepositoryData;

class CRKRepository
{
private:
	stRepositoryData m_stData[MAX_REPOSITORY];

	CString m_szInfoFileName;

	CRKParser m_csInfoParser;
	CRKParser m_csRepositoryParser;
	CRKWriter m_csWriter;

	INT16 m_nDefaultIndex;
	INT16 m_nNowIndex;
	INT16 m_nSize;

public:
	CRKRepository();
	~CRKRepository();

	void Init();

	BOOL LoadInfoFile();
	void SetInfoFileName(CString& szFileName) { m_szInfoFileName = szFileName; }
	
	void SetDefaultIndex(INT16 nIndex);	// Zero-Based
	void SetNowIndex(INT16 nIndex);	// Zero-Based
	void SetSize(INT16 nSize);

	INT16 GetDefaultIndex() { return m_nDefaultIndex; }	// Zero-Based
	INT16 GetNowIndex() { return m_nNowIndex; }	// Zero-Based
	INT16 GetSize() { return m_nSize; }

	INT16 GetRepositoryList(CList<stRepositoryData, stRepositoryData>& csList);
	stRepositoryData* GetRepositoryData(INT16 nIndex);	// Zero-Based
	stRepositoryData* GetRepositoryData(CString& szName);

	BOOL NewRepository();
	BOOL AddRepository(CString& szName, CString& szPath, CString& szWorkingFolder);
	BOOL RemoveRepository(CString& szName);

	// 아래 함수들은 이미 있는 내용을 수정하는 것이다.
	BOOL SetDefaultRepository(CString& szName);
	BOOL SetRemotePath(CString& szName, CString& szPath);
	BOOL SetWorkingFolder(CString& szName, CString& szWorkingFolder);

protected:
	BOOL AddRepositoryArray(CString& szName, CString& szPath, CString& szWorkingFolder);
	BOOL RemoveRepositoryArray(CString& szName);
};