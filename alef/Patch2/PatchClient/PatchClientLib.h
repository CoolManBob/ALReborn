#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <afxinet.h>

#include "LinkedList.h"

#include "PatchPacketTypeCS.h"
#include "PatchPacketTypeSC.h"
#include "PatchClientRegistry.h"
#include "PatchCompress.h"
#include "AuMD5Encrypt.h"
#include "CustomProgressBar.h"
#include "AuPackingManager.h"

#include "AuPatchCheckCode.h"
#include "ClientNetworkLib.h"

#include "AgcmUIOptionDefine.h"

#include <list>
#include <map>

#define	PATCHCLIENT_GAMECLIENT_FILE	"alefclient.exe"

#define PATCHCLIENT_PING_DELAY			2000
#define PATCHCLIENT_PING_MAX_WAIT_TIME	20000
#define PATCHCLIENT_PING_WAIT_SEQ		(PATCHCLIENT_PING_MAX_WAIT_TIME - PATCHCLIENT_PING_DELAY)

const unsigned int BATCH_FILE_CONTENT_SIZE = 512;

enum OSType
{
	OS_NONE = 0,
	W95,
	W95SP1,
	W95OSR2,
	W98,
	W98SP1,
	W98SE,
	WME,
	WNT351,
	WNT4,
	W2K,
	WXP,
	W2003SERVER,
	WCE
};

struct	DriverInfo
{
	int		index;
	char	download_url[100];
	
	// version 체크용
	int		iProduct;
	int		iVersion;
	int		iSubVersion;
	int		iBuild;

	DriverInfo*	next;
};

class AgpdPatchFileInfo
{
public:
	int				m_iFileNameSize;
	char			*m_pstrFileName;
	unsigned int	m_iFileSize;
	unsigned long	m_ulCheckSum;

	AgpdPatchFileInfo()
	{
		m_pstrFileName = NULL;
	}
	~AgpdPatchFileInfo()
	{
		if( m_pstrFileName )
		{
			delete [] m_pstrFileName;
			m_pstrFileName = NULL;
		}
	}
};

class CPatchClientLib;

class AgpdPatchAreaInfo
{
public:
	typedef enum
	{
		TYPE_NORMAL	= 0,
		TYPE_NEW
	} Type;

	static CPatchClientLib *	m_pcsPatchClientLib;

	char *			m_pszName;
	Type			m_eType;
	int				m_lCategory;
	char *			m_pszPatchServer;
	int				m_lPatchPort;
	char *			m_pszLoginServer;
	int				m_lLoginPort;
	DWORD			m_adwPingSendTime[PATCHCLIENT_PING_WAIT_SEQ];
	int				m_lPingSeq;
	int				m_lPingTime;

	WSABUF			m_cWSABuf;

	CWinSockLib		m_csSocket;

	AgpdPatchAreaInfo()
	{
		m_pszName			= NULL;
		m_eType				= TYPE_NORMAL;
		m_lCategory			= 0;
		m_pszPatchServer	= NULL;
		m_lPatchPort		= 0;
		m_pszLoginServer	= NULL;
		m_lLoginPort		= 0;
		m_lPingTime			= -1;
		m_lPingSeq			= 0;
		memset(&m_adwPingSendTime, 0, sizeof(m_adwPingSendTime));

		m_cWSABuf.len = 4096;
		m_cWSABuf.buf = new char[m_cWSABuf.len];
	}

	~AgpdPatchAreaInfo()
	{
		Clear();

		delete [] m_cWSABuf.buf;
	}

	void Clear();
	bool SetPatchInfo(char *pszName, char *pszPatchServer, int lPatchPort, char *pszLoginServer, int lLoginPort, int lType = TYPE_NORMAL, int lCategory = 0);
	bool SendPingPacket();
	bool ParsePacket();
	bool CalcPingTime(int lSeq);
};

class CPatchClientLib : public CWinSockLib
{
	// 폴더 단위로 지우는 파일들을 처리 하기 위해
	// 맵 안에 리스트를 사용했다. key값으로는 경로가 쓰이고 리스트에는 파일명만 들어간다
	typedef std::list< string >						RemoveFileList;
	typedef std::list< string >::iterator			RemoveFileListIter;

	typedef std::map< string , RemoveFileList >		RemoveFolderMap;

	class CPatchFileInfo
	{
	public: 
		bool	m_bPacking;
		int		m_nOperation;
		int		m_nNameLength;
		char*	m_pName;
		int		m_nSize;
		unsigned long	m_lFilePos;

		CPatchFileInfo()
		{
			Clear();
		}

		~CPatchFileInfo()
		{
			if ( m_pName )
				delete m_pName;
		}

		void Clear()
		{
			m_bPacking		= false;
			m_nOperation	= 0;
			m_nNameLength	= 0;
			m_pName			= NULL;
			m_nSize			= 0;
			m_lFilePos		= 0;
		}
	};

	//지울파일을 리스트에 가지고 있다가 패치가 완료되면 지운다.
	CDList<char *>		m_clDeleteFile;

	//받은 파일의 압축을 풀기위해.
	CCompress			m_cCompress;

	WSABUF				m_cWSABuf;

	bool				m_bPatchClient;

	//Download ServerInfo
	char				m_strDownloadServerIP[PATCH_DOWNLOAD_SERVER_LENGTH];
	unsigned short		m_iDownloadServerPort;

	char				*m_pstrBuffer;
	char				m_strTempDir[256];
	int					m_iPacketSize;
	int					m_iRemainPacketLength;

	bool				m_bURL;
	int					m_iVersion;					//올려야하는 패치 버전(서버로부터 받음)
	int					m_iPatchCheckCode;
	int					m_iRecvPatchFiles;			//받아야하는 패치파일의 수.
	int					m_iCurrentRecvedFiles;		//현재받은 패치파일의 수.

	char				m_strPatchFileName[512];	//패치중인 파일 이름.
	int					m_PatchFileSize;			//패치중인 파일의 크기
	int					m_iRecvedBlockSize;			//얼마나 받았나?

	int					m_iStartTime;
	int					m_iNowTime;

	int					m_iCurrentPatchFiles;
	int					m_iTotalPatchFiles;			//패치될 파일들의 갯수.
	
	FILE				*m_fpRecvFile;

	//. 2006. 3. 20. nonstopdj
	//. 낱개 JZP파일 decompress지원 추가 멤버.
	CString				m_szBatchFileContent;

	//. load patch check file, and get check code.
	AuPatchCheckCode	m_cAuPatchCheckCode;

	// 중국 Area 선택 관련 기능
	char *				m_szAreaDownloadURL;

	char				m_strAlefClientFullPath[255];

	RemoveFolderMap		m_RemoveFolderMap;

public:
	CPatchClientRegistry		m_cPatchReg;

	bool				m_bExit;
	bool				m_bExitPing;

	//Packing
	AuPackingManager	m_csAuPackingManager;
	
	//ProgressBar를 그리기 위해 어쩔수없이..... -_-;
	CCustomProgressCtrl	*m_pcCustomProgressRecvFile;
	CCustomProgressCtrl	*m_pcCustomProgressRecvBlock;
	CStatic				*m_pcStaticStatus;
	//CStatic				*m_pcStaticProgressPercent;
	CStatic				*m_pcStaticDetailInfo;

	CWnd				*m_phParentCWnd;

	int					m_lResult;
	__int64				m_iRequireDiskSpace;		//디스크가 얼마나 필요한가?

	// 중국 Area 선택 관련 기능
	AgpdPatchAreaInfo *	m_pcsAreaInfo;
	HANDLE				m_hEventForNewSocket;
	HANDLE *			m_phEvents;
	int					m_lArea;
	CString				m_strLoginInfo;
	ApCriticalSection	m_csCSArea;

	CPatchClientLib();
	~CPatchClientLib();

	//새로 개발된 PatchClient 코드.
	CDList<AgpdPatchFileInfo *> m_csPatchFileList;

	//레퍼런스파일을 읽는다.
	bool LoadReferenceFile();

	//깨진 Refrence파일이 있는지 확인한다.
	bool HaveCrackedFolder();

	//Cracked Folder ApAdmin을 얻어낸다.
	ApAdmin *GetCrackedFolderInfo();

//	bool GetOriginFileFromHTTP();

	//원래 존재하던 PatchClient 코드.
	bool IsPatchClient();

	bool ExecuteIOLoop( bool bLogin );
	bool ExtractPacket( int iTransferedBytes, AgpdPatchAreaInfo *pcsArea = NULL );
	bool ParsePacket( char *pstrPacket, AgpdPatchAreaInfo *pcsArea = NULL );

	bool SendCurrentVersion( int iVersion, int iPatchCheckCode );
	bool SendRequestFile();
	bool SendFileBlock();
	bool GetPatchInfo( char *pstrPacket );
	bool GetPatchFile( char *pstrPacket );
	bool RemoveFile	( char *pstrPacket );

	bool DeleteTokenFileName( char *pTokenFileName );	// 토큰 문자열 파일 목록을 지운다
	bool DeleteFileList( VOID );						// m_RemoveFolderMap에 들어있는 파일들을 다 지운다

	bool IsPackingFolder		( const string& strFolderName );
	bool PackingDataFileDelete	( const char* strFolderName , RemoveFileList& rFileList );
	bool DataFileDelete			( const char* strFolderName , RemoveFileList& rFileList );

	bool SetTempDir( char *pstrTempDir );

	bool PatchFileToDisk( int iFileNameSize, char *pstrFileName, int iFileSize, char *pstrPatchFile );
	bool RemoveDir( char *pstrDir );
	bool RemoveDirR( LPCTSTR pstrSourceDir, LPCTSTR pstrCurrentDir );
	BOOL CopyFileToPatchFolder( char *pstrSPath, char *pstrDPath, char *pstrFullPath, BOOL bOverwrite );
	bool CopyDir( char *pstrDestDir, char *pstrSourceDir );
	bool CopyDirR( char *pstrDestPath, char *pstrSourcePath, LPCTSTR pstrCurrentPath );

	//ProgressBar를 그리기 위해 어쩔수없이..... -_-;
	void SetDialogInfo( CWnd *phParentCWnd, CCustomProgressCtrl *pcCustomProgressRecvFile, CCustomProgressCtrl *pcCustomProgressRecvBlock, CStatic *pcStaticStatus, CStatic *pcProgressPercent, CStatic *pcStaticDetailInfo );

	bool GetTotalPatchFiles( char *pstrSourcePath );
	bool GetTotalPatchFilesR( char *pstrSourcePath, LPCTSTR pstrCurrentPath );

	//@{ 2006/04/18 burumal
	bool GetPatchFiles( char *pstrSourcePath, char* pstrPatchFile );
	//@}
	
	bool SearchJZPFile( char *pstrSourcePath );
	bool SearchJZPFileR( char *pstrSourcePath, LPCTSTR pstrCurrentPath );
	bool DecompressJZP( char *pstrTempFolderName, char *pstrJzpFileName );
	bool DecompressJZPEx(char* pstrTempFolderName, char* pstrJzpFileName);		

#ifdef USE_ZPACK
	bool PackingToZpack(char* pstrTempFolderName, char* pstrJzpFileName);
	bool CompressPatchFiles();
#endif

	//For InternetLib
	bool GetInternetFile(char *pstrURL );
	bool GetURL( char *pstrPacket );

	//For Static Display
	bool InvalidateStatic( CStatic *pcStatic, wchar_t *pstrText, bool bInvalidateRect = true);

	//For GetFileName
	char *GetFileName( char *pstrFilePath );

	//. 2006. 3. 20. nonstopdj
	//. Make batch file, force exit program, Excute batch file
	void ForceExitProgram();

	//. In ones JZP file process.
	void InOnesJZPProcess(char* pszFileName, char* pszFullPathFileName, bool* pbSelfPatch , bool bIsMNP = false);

	//. In ones JZP file decompress.
	bool DecompressSingleJZP(char* pszFileName);

	bool DownloadFile(char* pszURL, char* pszFile, char* pszHeader = NULL, int lTotalRecvByte = 0, AgpdPatchFileInfo *pcsPatchFileInfo = NULL);

	//@{ 2006/04/12 burumal
	CPackingFile* SearchBakPackingEmptyList(char* pFileName, long lNeeds, ApAdmin* pAdminBakEmpty, long* pOffset);
	//@}

	//@{ 2006/04/14 burumal
	bool AttachAddedPackingData(FILE* fpReference, char* pstrFolderName, char* pstrFileName, int lFileSize, char* pDataBuffer);	
	//@}

	//@{ 2006/04/13 burumal
	bool OverwritePackingData(FILE* fpReference, FILE* fpBakDat, char* pFolderName, CPackingFile* pOverwriteInfo, 
		char* pFileName, char* pstrBuffer, int lFileSize);
	//@}

	//@{ 2006/04/16 burumal
	bool WriteNormalPackingRefInfo(FILE* fpReference, ApAdmin* pAdminNormal);	
	//@}

	//@{ 2006/10/17 burumal
	void StartupSelfPatchProcess();
	//@}

	// 중국 Area 선택 관련 기능
	bool SetConfigFile();
	bool ReadConfig(char *pszFileName);
	bool DownloadAreaConfig();
	bool ReadAreaConfig();
	AgpdPatchAreaInfo *GetArea(char *pszName);
	bool SetArea(char *pszName);
	int GetAreaCount(int lCategory);
	int GetNewAreaCount();
	AgpdPatchAreaInfo *GetNextArea(int lCategory, int *plIndex);
	AgpdPatchAreaInfo *GetNextNewArea(int *plIndex);

	bool StartGame( char* pPatchCodeString );
	void ScanCrackedFiles( char *pstrSourceFolder ); //패치중 깨진 파일들을 복구한다. 하는일이라봐야 Remove, Rename뿐이지만...
	BOOL CheckDXVersion();
	int CheckGraphicDriver();

	OSType GetMyWindowVer();
	void ClearLists(DriverInfo*	pList);

	void	SendPingPacket();
	bool	CheckPingTime(bool bLoop = true);
	INT32	GetPingTime(char *pszArea);
	void	ClearAllArea();

	void	LockArea()	{ m_csCSArea.Lock();	}
	void	UnlockArea()	{ m_csCSArea.Unlock();	}

	char const * GetDownLoadURL() { return m_strDownloadServerIP; }

public :
	BOOL	SendRequestClientFileCRC( void );
	void	RefreshProgressBar1( std::wstring strFileNameW );
private:
	string m_exeName;

#ifdef USE_ZPACK
	typedef std::multimap< std::wstring, std::wstring > PatchFiles;
	PatchFiles patchFiles_;
	int progressCount_;
	int progressMax_;
	int lastPatchVersion_;
#endif
};
