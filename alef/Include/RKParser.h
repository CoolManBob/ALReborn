// RKParser.h
// .rk File Parser
// 2003.07.29 steeple

// 현재 Line 의 Token 들을 구조체로 뽑아내고 싶을때 쓴다.
typedef struct stRKEntryInfo
{
	CString szType;
	CString szFileName;
	CString szSize;
	CString szDate;
	CString szVer;
	CString szOwner;
} stRKEntryInfo;

typedef struct stRKHistoryInfo
{
	CString szOwner;
	CString szDate;
	CString szVer;
} stRKHistoryInfo;


class CRKParser
{
private:
	CStdioFile m_csFile;
	BOOL m_bOpened;

	CString m_szFileName;
	CString m_szNowLine;
	CString m_szToken[MAX_TOKEN];

	INT8 m_cNumToken;
	INT8 m_cNowToken;
	INT32 m_lNowLine;
	INT32 m_lTotalLine;

public:
	CRKParser();
	~CRKParser();

	void Init();

	BOOL Open();
	BOOL Close();

	BOOL NewLine();
	BOOL FindFileInfo(CString& szFileName);	// 그곳의 Offset 까지 간다.

	void SetFileName(CString& szFileName) { m_szFileName = szFileName; }

	CString* GetToken(INT8 cIndex);
	CString* GetPassword();
	void GetEntryTokenStruct(struct stRKEntryInfo& stRKEntryInfo);
	void GetHistoryTokenStruct(struct stRKHistoryInfo& stRKHistoryInfo);

	UINT8 GetNumToken() { return m_cNumToken; }
	UINT8 GetNowToken() { return m_cNowToken; }
	UINT32 GetNowLine() { return m_lNowLine; }
	UINT32 GetTotalLine() { return m_lTotalLine; }

private:
	BOOL ReadToken();
	void ClearParseData();
};
