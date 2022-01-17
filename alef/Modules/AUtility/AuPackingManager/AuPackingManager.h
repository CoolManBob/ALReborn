#ifndef __PACKING_LIB_H__
#define __PACKING_LIB_H__

#include <String.h>
#include <stdio.h>

#include "ApAdmin.h"
#include "LinkedList.h"
#include "PatchCompress.h"
#include "AuMD5Encrypt.h"

#define Packing_Operation_Add					1
#define Packing_Operation_Remove				2
#define Packing_lBlockSize						4096*4

#define Packing_Patch_FileName					"Patch"
#define Packing_Data_FileName					"Data.Dat"
#define Packing_Data_Backup_FileName			"Data.Bak"
#define Packing_Reference_FileName				"Reference.Dat"
#define Packing_Reference_Backup_FileName		"Reference.Bak"

typedef enum
{
	g_dErrorRequestOldVersion = 1,
	g_dErrorRequestNoneVersion,
	g_dErrorNoPatchFile,
	g_dErrorInvalidPatchCode

} ePatcherErrorCode;

typedef enum
{
	AuPacking_Status_Build = 1,
	AuPacking_Status_Editing,
	AuPacking_Status_Complete,
};

class CListBoxData
{
public:
	int				m_lFileSize;
	unsigned long	m_lCheckSum;
};

class CJZPFileInfo
{
public:
	int				m_iVersion;
	int				m_iFileNameSize;
	char*			m_pstrFileName;
	int				m_iFileSize;
	int				m_iRawFileSize;
	int				m_iBlockIndex;
	unsigned long	m_iCRCCheckSum;

	CJZPFileInfo()
	{
		m_pstrFileName = NULL;
	}

	CJZPFileInfo( int iVersion, int iFileNameSize, char* pstrFileName, int iFileSize, int iRawFileSize, unsigned long iCRCCheckSum )
	{
		m_iVersion = iVersion;
		m_iFileNameSize = iFileNameSize;
		m_pstrFileName = pstrFileName;
		m_iFileSize = iFileSize;
		m_iRawFileSize = iRawFileSize;
		m_iBlockIndex = 0;
		m_iCRCCheckSum = iCRCCheckSum;
	}

	~CJZPFileInfo()
	{
		if( m_pstrFileName != NULL )
		{
			delete [] m_pstrFileName;
			m_pstrFileName = NULL;
		}
	}
};

class CPackingDataNode
{
public:
	int		m_lStartPos;
	int		m_lSize;

	//CPackingDataNode *m_pcsNextNode;

	CPackingDataNode()
	{
		//m_pcsNextNode = NULL;
	}
};

class CPackingFile
{
public:
	bool				m_bPacking;				//패킹되는 파일인가?
	int					m_lOperation;
	size_t				m_lFileNameSize;
	char*				m_pstrFileName;
	int					m_lFileSize;

//	int					m_lChildDataNodeCount;

	CPackingDataNode*	m_pcsDataNode;

	unsigned long		m_lFileCheckSum;

	CPackingFile()
	{
		m_bPacking = true;
		m_lFileSize = 0;
//		m_lChildDataNodeCount = 0;

		m_pcsDataNode = NULL;
	}

	~CPackingFile()
	{
		Reset();
	}

	void Reset();
};

class CPackingFolder
{
public:
	ApAdmin		m_csFilesInfo;

	ApMutualEx	m_Mutex;

//	char		m_lEditing;			//File Writing에 대한정보.
	bool		m_bExportToRef;		//Reference file로 Export할것인가?

	int			m_lEmptyBlockCount;
	int			m_lBlockSize;
	char*		m_pstrBlock;

	size_t		m_lFolderNameSize;
	char*		m_pstrFolderName;

	FILE*		m_fpFile;

	//@{ 2006/04/16 burumal
	long		m_lDataFileSize;
	//@}

	CPackingFolder();
	~CPackingFolder();
};

class CPatchFolderInfo
{
public:
	bool					m_bPacking;
	char*					m_pstrFolderName;

	CDList<CPackingFile*>	m_clFileList;

	void SetPacking( bool bPacking );

	CPatchFolderInfo()
	{
		m_bPacking = true;			//디폴트 True다
		m_pstrFolderName = NULL;
	}

	~CPatchFolderInfo()
	{
		if( m_pstrFolderName != NULL )
		{
			delete [] m_pstrFolderName;
			m_pstrFolderName = NULL;
		}
	}
};

class CPatchVersionInfo
{
public:
	int				m_lVersion;

	CDList<CPatchFolderInfo *> m_clFolderList;

	CPatchVersionInfo()
	{
		m_lVersion = 0;
	}
};

class ApdFile
{
public:
	CPackingFolder	*m_pcsPackingFolder;
	CPackingFile	*m_pcsPackingFile;

	bool		m_bLoadFromPackingFile;
	size_t		m_lCurrentPosition;
	int			m_lFilePosition;
	char		m_strFolderName[255];
	char		m_strFileName[255];

	FILE		*m_phDirectAccess;

	ApdFile()
	{
		m_bLoadFromPackingFile = true;
		m_pcsPackingFolder = NULL;
		m_pcsPackingFile = NULL;

		m_lCurrentPosition = 0;
		m_lFilePosition = 0;
		

		m_phDirectAccess = NULL;
	}
};

class CIgnoreFileInfo
{
public:
    size_t		m_lFileNameSize;
	char		*m_pstrFileName;

	CIgnoreFileInfo()
	{
		m_pstrFileName = NULL;
	}
};

class CCrackedFolderInfo
{
public:
    size_t		m_lFolderNameSize;
	char		*m_pstrFolderName;

	CCrackedFolderInfo()
	{
		m_pstrFolderName = NULL;
	}
};

class AuPackingManager
{
	static		AuPackingManager*	m_pThis;

	//bool		m_bLoadFromPackingData;
	bool		m_bReadOnlyMode;

	int			m_lFileCount;
	int			m_lFileInfoCount;

	ApAdmin		m_csIgnoreFileInfo;

	//Cracked Folder Info.
	ApAdmin		m_csCrackedFolderInfo;

	//현재 접근중인 폴더의 이름.
    //ApAdmin	m_csLoadingFolder;

	ApAdmin		m_csFolderInfo;
	
	//버전에 따른 리스트 정보.
	CDList<CPatchVersionInfo *>	m_cPatchList;

	//JZPFile Info.
	ApAdmin		m_csJZPFileInfo;

	//Lib 4 Compress
	CCompress	m_cCompress;

	//@{ 2006/04/14 burumal
	int			m_nEmptyFileSerial;
	//@}

public:
	//데이터 맵핑.
	int			m_lCurrentVersion;

	//AuMD5Encypt
	AuMD5Encrypt	m_cMD5Encrypt;

	//Option
	bool		m_bBuildJZP;
	bool		m_bBuildDat;
	bool		m_bCopyRawFiles;

	//bool		m_bLoadFromRawFile;

	AuPackingManager();
	~AuPackingManager();
	void Destroy();

	//void setLoadFromPackingData( bool bSet );

	//member access method
	ApAdmin							*GetFolderInfo();
	CDList<CPatchVersionInfo *>		*GetPatchList();

	bool IsCheckedBuildJZP();
	bool IsCheckedBuildDat();
	bool IsCheckedCopyRawFiles();
	void SetBuildJZP( bool bBuildJZP );
	void SetBuildDat( bool bBuildDat );
	void SetCopyRawFiles( bool bCopyRawFiles );

	// Singleton Funcs
	static	AuPackingManager* MakeInstance();
	static	AuPackingManager* GetSingletonPtr();
	static	void DestroyInstance();

	//IgnoreFile
	bool LoadIgnoreFileInfo( char *pstrRootFolder, char *pstrFileName );
	bool AddIgnoreFileName( char *pstrIgnoreFileName );
	bool RemoveIgnoreFileName( char *pstrIgnoreFileName );
	bool IsIgnoreFileName( char *pstrIgnoreFileName );
	ApAdmin *GetIgnoreFileInfo();

	//CrackedFolder
	bool AddCrackedFolderName( char *pstrCrackedFolderName );
	bool RemoveCrackedFolderName( char *pstrCrackedFolderName );
	ApAdmin *GetCrackedFolderInfo();

	//Version
	int GetLastVersion();

	//Operation 4 List
	CPatchVersionInfo *GetPatchInfo( int lVersion );

	//For Res File
	bool ScanFolder( bool bCompareCheckSum );

	//For Data Structure
	bool AddFolder( char *pstrFolderName );
	bool AddFile( char *pstrFolderName, CPackingFile *pcsTempPackingFile, int lFileSize, unsigned long lCheckCode );
	//bool AddPackingFile( char *pstrTempFolderName, char *pstrFolderName, char *pstrFileName );
	//bool RemovePackingFile( char *pstrFolderName, char *pstrFileName );

	//@{ 2006/05/05 burumal
	/*
	CPackingFolder *GetFolder( char *pstrFolderName );
	CPackingFile *GetFile( char *pstrFolerName, char *pstrFileName );
	*/
	CPackingFolder *GetFolder( char *pstrFolderName, bool bAlreadlyLowercase = false );
	CPackingFile *GetFile( char *pstrFolerName, char *pstrFileName, bool bAlreadlyLowercase = false );
	//@}
	
	bool AddPatch( CPatchVersionInfo *pcsPatchVersionInfo );

	//Export
	bool BuildJZPFile( int lVersion, char *pstrExportFolder, CPatchVersionInfo *pcsPatchVersionInfo, HWND hDisplayWnd, HWND hProgressWnd );
	bool LoadResFile( char *pstrResFileName );

	//For Patch
	bool BuildDatFile( char *pstrPackingDir, char *pstrRawDir, int lVersion, HWND hDisplayWnd, HWND hProgressWnd );
	//bool BuildReferenceFile( char *pstrPackingFolder, char *pstrRawFolder, bool bResort, HWND hDisplayWnd, HWND hProgressWnd );
	bool MarkPacking( char *pstrPackingFolder );
	bool MarkPackingComplete( char *pstrPackingFolder );

	//@{ 2006/04/16 burumal
	//bool LoadReferenceFile(LPCTSTR pstrCurrentPath, bool bReadOnly = true, bool bRecursiveSubDir = true );
	bool LoadReferenceFile(LPCTSTR pstrCurrentPath, bool bReadOnly = true, bool bRecursiveSubDir = true, bool bResizing = false );
	//@}
		// interface of finding reference file

	//@{ 2006/04/16 burumal
	//bool ReadReferenceFile( char * pstFileName );
	bool ReadReferenceFile( char * pstFileName, bool bResizing = false );
	//@}
		// Implementation of reading reference file
		// called in LoadReferenceFile function internally

	void SetFilePointer( bool bLoadSubDir = true );
	void CloseAllFileHandle();
	//bool UpdateDatAndRefFileFromJZP( char *pstrTempFolderName, char *pstrJzpFileName );

	//Misc
	bool GetFolderName( const char *pstrFullPath, const char *pstrSourceFolder, char *pstrBuffer, int lBufferSize );
	bool CreateFolder( char *pstrFolderName );
	CJZPFileInfo *GetJZPInfo( int iVersion );

	//File Operation
    int GetFileSize( CPackingFile *pcsPackingFile );
    int GetFileSize( ApdFile *pcsApdFile );		
	
	bool OpenFile( char *pstrFilePath, ApdFile *pcsApdFile );

	//@{ 2006/05/05 burumal	
	//bool OpenFile( char *pstrFolderName, char *pstrFileName, ApdFile *pcsApdFile );	
	bool OpenFile( char *pstrFolderName, char *pstrFileName, ApdFile *pcsApdFile, char* pstrFullFilePath = NULL );
	//@}
	
	int ReadFile( void *pstrBuffer, int iBufferLen, ApdFile *pcsApdFile );

	bool SetPos( int lPos, ApdFile *pcsApdFile );
	bool CloseFile( ApdFile *pcsApdFile );
	bool Lock( ApdFile *pcsApdFile );
	bool Unlock( ApdFile *pcsApdFile );

	//. 2006. 4. 25. nonstopdj
	//. 파일이 존재하지 않다면 빈파일만들기.
	void CreateEmptyFile( const char* pstrFullPath );

	//Packing2방식
	bool ReadyPacking( char *pstrFolder );
	
	bool CompletePacking( char *pstrFolder, int lCurrentPosition, int lPackedFileCount, ApAdmin *pcsAdminAdd, ApAdmin *pcsAdminRemove );		

	//@{ 2006/04/07 burumal
	//bool BuildPackingFile( char *pstrTempFolderName, char *pstrFolderName, char *pstrFileName, int lCurrentPos, int lFileSize );
	bool BuildPackingFile(char* pstrTempFolderName, char* pstrFolderName, char* pstrFileName, int lCurrentPos, 
		int lFileSize, FILE* fpData, FILE* fpReference, char* pDecompMem);
	//@}

	//@{ 2006/04/12 burumal
	bool ReadBakReference(char* pstrFolder, ApAdmin* pAdminAdd, ApAdmin* pAdminRemove, ApAdmin* pAdminPackingNormal, 
		ApAdmin* pAdminPackingEmpty);
	//@}

	//@{ 2006/04/13 burumal
	CPackingFile* AddEmptyPackingFileInfo(long lStartPos, long lSize, ApAdmin* pAdminPackingEmpty);
	//@}

	//@{ 2006/04/16 burumal
	bool ResizeDatFile(CPackingFolder* pFolder);
	
	bool ResizeFileSize(char* pFilePath, int nFinalSize);
	//@}

	//@{ 2006/04/16 burumal
	bool CompleteCurrentFolderProcess(char* pstrFolder, int lPackedFileCount);
	//@}
};

#endif