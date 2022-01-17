// Patcher2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Patcher2.h"
#include "Patcher2Dlg.h"
#include ".\patcher2dlg.h"
#include "ServerSettingDlg.h"
#include <ImageHlp.h>

#include "FolderPointingDlg.h"
#include "FullScanOptionDlg.h"


#include <fstream>
using namespace std; 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BOOLEAN( x ) ( ( ( x ) != 0 ) ? true : false )

static BOOL	_CreateDirectory( char *pFormat , ... )
{
	char	strtmp[8192];
	va_list vParams;

	va_start	( vParams,		pFormat				);
	vsprintf	( strtmp	,	pFormat,	vParams	);
	va_end		( vParams							);

	return CreateDirectory( strtmp , NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatcher2Dlg dialog

CPatcher2Dlg::CPatcher2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatcher2Dlg::IDD, pParent)
	, m_bSyncExistingFile		( FALSE	)
	, m_bCompareChecksum		( TRUE	)
	, m_bExecutableAvailable	( FALSE	)
	, m_PatchType				( PATCH_FAST )
	, m_DirectoryHandler		( m_cFileInfoList )
	, m_nMaxYear				( 0 )
	, m_nMaxMonth				( 0 )
	, m_nMaxDay					( 0 )
	, m_nMinYear				( 0 )
	, m_nMinMonth				( 0 )
	, m_nMinDay					( 0 )
	, m_nLastPatchYear			( 0 )
	, m_nLastPatchMonth			( 0 )
	, m_nLastPatchDay			( 0 )
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bBuildPatch	= false;

	m_bThreadRun	= FALSE;
	m_eOperation	= OPERATION_COUNT;

	m_nSkilFileCount	=	0;

	ZeroMemory( m_strFileName , sizeof(m_strFileName) );
	
	switch( g_eServiceArea )
	{
	case AP_SERVICE_AREA_KOREA		: m_strRegion = "korea";	break;
	case AP_SERVICE_AREA_CHINA		: m_strRegion = "china";	break;
	case AP_SERVICE_AREA_WESTERN	: m_strRegion = "western";	break;
	case AP_SERVICE_AREA_JAPAN		: m_strRegion = "japan";	break;
	//case AP_SERVICE_AREA_TAIWAN		: m_strRegion = "taiwan";	break;
	default							: m_strRegion = "unknown";	break;
	}
	
}

void CPatcher2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatcher2Dlg)
	DDX_Control(pDX, IDC_PatchFileProgress, m_cPatchFileProgress);
	DDX_Control(pDX, IDC_PatchedFileList, m_cPatchedFilesList);
	DDX_Control(pDX, IDC_PatchVersion_Static, m_cPatchVersionStatic);
	DDX_Control(pDX, IDC_RemoveFileList, m_cRemoveFileList);
	DDX_Control(pDX, IDC_FileInfo_List, m_cFileInfoList);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BuildJZPCheckButton, m_cCheckBuildJZP);
	DDX_Control(pDX, IDC_BuildPackingCheckButton, m_cCheckBuildPackingData);
	DDX_Control(pDX, IDC_CopyRawFiles_Button, m_csCheckCopyRawFiles);
	DDX_Check(pDX, IDC_SynExistingFiles, m_bSyncExistingFile);
	DDX_Check(pDX, IDC_COMPARE_CHECKSUM, m_bCompareChecksum);
	DDX_Control(pDX, IDC_BuildPatch, m_btnBuildPatch);
	DDX_Control(pDX, IDC_UploadJZP, m_btnUploadJZP);

	DDX_Control(pDX, IDC_PatchDone, m_btnPatchDone);
	DDX_Control(pDX, IDC_BUTTON3, m_btnPackageData);
	DDX_Control(pDX, IDC_ProgressStatus_LB, m_lbProgress);
	DDX_Control(pDX, IDC_Comment_EDIT, m_editComment);
	DDX_Radio(pDX, IDC_FAST_RADIO, m_PatchType);
	DDX_Control(pDX, IDC_Log_Button, m_btnLog);
	DDX_Control(pDX, IDC_ADDFILE_STATIC, m_staticPatchFile);
	DDX_Control(pDX, IDC_DELFILE_STATIC, m_staticDelFile);
	DDX_Control(pDX, IDC_LIST_COMBO, m_comboList);
	DDX_Control(pDX, IDC_Exit_Button, m_btnExit);
	DDX_Control(pDX, IDC_REALTIME_SCAN, m_btnSensorOn);
	DDX_Control(pDX, IDC_REALTIME_STOP, m_btnSensorOff);
	DDX_Control(pDX, IDC_SKIPFILE_STATIC, m_staticSkipFile);
} 

BEGIN_MESSAGE_MAP(CPatcher2Dlg, CDialog)
	//{{AFX_MSG_MAP(CPatcher2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE( WM_USER+83 , OnWriteLogFile )
	ON_MESSAGE( WM_USER+84 , OnInitProgress )
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_SCAN, OnRemoveScanFolderButton)
	ON_BN_CLICKED(IDC_ScanFolder_Button, OnScanFolderButton)
	ON_BN_CLICKED(IDC_Exit_Button, OnExitButton)
	ON_BN_CLICKED(IDC_DisplayPrevVersionButton, OnDisplayPrevVersionButton)
	ON_BN_CLICKED(IDC_DisplayNextVersionButton, OnDisplayNextVersionButton)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PatchFileRemoveButton, OnBnClickedPatchfileremovebutton)
	ON_BN_CLICKED(IDC_PatchFileRemoveResetButton, OnBnClickedPatchfileremoveresetbutton)
	ON_BN_CLICKED(IDC_RemoveResetButton, OnBnClickedRemoveresetbutton)
	ON_BN_CLICKED(IDC_ResetButton, OnBnClickedResetbutton)
	ON_BN_CLICKED(IDC_RebuildButton, OnBnClickedRebuildbutton)
	ON_BN_CLICKED(IDC_SynExistingFiles, OnBnClickedSynexistingfiles)
	ON_BN_CLICKED(IDC_UploadJZP, OnBnClickedUploadjzp)
	ON_BN_CLICKED(IDC_BuildPatch, OnBnClickedBuildpatch)
	ON_BN_CLICKED(IDC_PatchDone, OnBnClickedPatchdone)
	ON_BN_CLICKED(IDC_UploadRes, OnBnClickedUploadres)
	ON_BN_CLICKED(IDC_UploadAllJZPs, OnBnClickedUploadalljzps)
	ON_BN_CLICKED(IDC_RebuildDone, OnBnClickedRebuilddone)
	ON_BN_CLICKED(IDC_PATCH_ALL, &CPatcher2Dlg::OnBnClickedPatchAll)
	ON_BN_CLICKED(IDC_BUTTON3, &CPatcher2Dlg::OnBnClickedMakePackage)
	ON_BN_CLICKED(IDC_BuildJZPCheckButton, &CPatcher2Dlg::OnBnClickedBuildjzpcheckbutton)
	ON_BN_CLICKED(IDC_BuildPackingCheckButton, &CPatcher2Dlg::OnBnClickedBuildpackingcheckbutton)
	ON_BN_CLICKED(IDC_CopyRawFiles_Button, &CPatcher2Dlg::OnBnClickedCopyrawfilesButton)
	ON_BN_CLICKED(IDC_COMPARE_CHECKSUM, &CPatcher2Dlg::OnBnClickedCompareChecksum)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CPatcher2Dlg::OnBnClickedClearButton)
	ON_BN_CLICKED(IDC_SETTING_BUTTON, &CPatcher2Dlg::OnBnClickedSettingButton)
	ON_CBN_DROPDOWN(IDC_LIST_COMBO, &CPatcher2Dlg::OnCbnDropdownListCombo)
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CPatcher2Dlg::OnBnClickedConnectButton)
	ON_BN_CLICKED(IDC_FAST_RADIO, &CPatcher2Dlg::OnBnClickedFastRadio)
	ON_BN_CLICKED(IDC_NORMAL_RADIO, &CPatcher2Dlg::OnBnClickedNormalRadio)
	ON_BN_CLICKED(IDC_PatchFileRemoveButton2, &CPatcher2Dlg::OnBnClickedPatchfileremovebutton2)
	ON_BN_CLICKED(IDC_REALTIME_STOP, &CPatcher2Dlg::OnBnClickedRealtimeStop)
	ON_BN_CLICKED(IDC_REALTIME_SCAN, &CPatcher2Dlg::OnBnClickedRealtimeScan)
	ON_BN_CLICKED(IDC_SCANFOLDER, &CPatcher2Dlg::OnBnClickedScanfolder)
	ON_BN_CLICKED(IDC_DeleteButton, &CPatcher2Dlg::OnBnClickedDeletebutton)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PatchFileProgress, &CPatcher2Dlg::OnNMCustomdrawPatchfileprogress)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatcher2Dlg message handlers
void CPatcher2Dlg::setResDir( char *pstrResDir )
{
	char			strCurrentDirectory[ MAX_PATH ];
	char			strFullDir[ MAX_PATH ];

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );
	sprintf_s( strFullDir, MAX_PATH , "%s\\%s", strCurrentDirectory, pstrResDir );

	if( SetCurrentDirectory( strFullDir ) )
	{
	}
	else
	{
		CreateDirectory( pstrResDir, NULL );
	}

	SetCurrentDirectory( strCurrentDirectory );

	sprintf_s( m_strResDir, MAX_PATH , "%s", pstrResDir );

}

void CPatcher2Dlg::setRawFileDir( char *pstrRawFileDir )
{
	char			strCurrentDirectory[ MAX_PATH ];
	char			strFullDir[ MAX_PATH ];

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );
	sprintf_s( strFullDir, MAX_PATH , "%s\\%s", strCurrentDirectory, pstrRawFileDir );

	if( SetCurrentDirectory( strFullDir ) )
	{
	}
	else
	{
		CreateDirectory( pstrRawFileDir, NULL );
	}

	SetCurrentDirectory( strCurrentDirectory );

	sprintf_s( m_strRawFileDir, MAX_PATH , "%s", pstrRawFileDir );
}

void CPatcher2Dlg::setZipDir( char *pstrZipDir )
{
	char			strCurrentDirectory[ MAX_PATH ];
	char			strFullDir[ MAX_PATH ];

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );
	sprintf_s( strFullDir, MAX_PATH , "%s\\%s", strCurrentDirectory, pstrZipDir );

	if( SetCurrentDirectory( strFullDir ) )
	{
	}
	else
	{
		CreateDirectory( pstrZipDir, NULL );
	}

	SetCurrentDirectory( strCurrentDirectory );

	sprintf_s( m_strZipDir, MAX_PATH , "%s", pstrZipDir );
}

void CPatcher2Dlg::setResortJZPDir( char *pstrResortJZPDir )
{
	char			strCurrentDirectory[ MAX_PATH ];
	char			strFullDir[ MAX_PATH ];

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );
	sprintf_s( strFullDir, MAX_PATH , "%s\\%s", strCurrentDirectory, pstrResortJZPDir );

	if( SetCurrentDirectory( strFullDir ) )
	{
	}
	else
	{
		CreateDirectory( pstrResortJZPDir, NULL );
	}

	SetCurrentDirectory( strCurrentDirectory );

	sprintf_s( m_strResortJZPDir, MAX_PATH , "%s", pstrResortJZPDir );
}

void CPatcher2Dlg::setPatchResBackupDir( char *pstrPatchResBackupDir )
{
	char			strCurrentDirectory[ MAX_PATH ];
	char			strFullDir[ MAX_PATH ];

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );
	sprintf_s( strFullDir, MAX_PATH , "%s\\%s", strCurrentDirectory, pstrPatchResBackupDir );

	if( SetCurrentDirectory( strFullDir ) )
	{
	}
	else
	{
		CreateDirectory( pstrPatchResBackupDir, NULL );
	}

	SetCurrentDirectory( strCurrentDirectory );

	sprintf_s( m_strPatchResBackupDir, MAX_PATH , "%s", pstrPatchResBackupDir );
}

void CPatcher2Dlg::setRebuildDir( char *pstrRebuildDir )
{
	char			strCurrentDirectory[ MAX_PATH ];
	char			strFullDir[ MAX_PATH ];

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );
	sprintf_s( strFullDir, MAX_PATH , "%s\\%s", strCurrentDirectory, pstrRebuildDir );

	if( SetCurrentDirectory( strFullDir ) )
	{
	}
	else
	{
		CreateDirectory( pstrRebuildDir, NULL );
	}

	SetCurrentDirectory( strCurrentDirectory );

	sprintf_s( m_strRebuildDir, MAX_PATH , "%s", pstrRebuildDir );
}

void CPatcher2Dlg::setDebugInfoDir( char *pstrDebugDir )
{
	char			strCurrentDirectory[ MAX_PATH ];
	char			strFullDir[ MAX_PATH ];

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );
	sprintf_s( strFullDir, MAX_PATH , "%s\\%s", strCurrentDirectory, pstrDebugDir );

	if( SetCurrentDirectory( strFullDir ) )
	{
	}
	else
	{
		CreateDirectory( pstrDebugDir, NULL );
	}

	SetCurrentDirectory( strCurrentDirectory );

	sprintf_s( m_strDebugDir, MAX_PATH , "%s", pstrDebugDir );
}

void CPatcher2Dlg::setPackingDir( char *pstrPackingDir )
{
	char			strCurrentDirectory[ MAX_PATH ];
	char			strFullDir[ MAX_PATH ];

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );
	sprintf_s( strFullDir, MAX_PATH , "%s\\%s", strCurrentDirectory, pstrPackingDir );

	if( SetCurrentDirectory( strFullDir ) )
	{
	}
	else
	{
		CreateDirectory( pstrPackingDir, NULL );
	}

	SetCurrentDirectory( strCurrentDirectory );

	sprintf_s( m_strPackingDir, MAX_PATH , "%s", pstrPackingDir );
}

bool CPatcher2Dlg::ProcessSortFiles()
{
	bool			bResult;

	bResult = false;

	ApAdmin			*pcsFolderInfo;

	pcsFolderInfo = m_csAuPackingManager.GetFolderInfo();

	if( pcsFolderInfo )
	{
		CPackingFolder			**ppcsPackingFolder;
		int						lFolderIndex;

		lFolderIndex = 0;

		for( ppcsPackingFolder = (CPackingFolder **)pcsFolderInfo->GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder **)pcsFolderInfo->GetObjectSequence( &lFolderIndex ) )
		{
			if( ppcsPackingFolder && (*ppcsPackingFolder) )
			{
				CPackingFile			**ppcsPackingFile;
				CPackingFile			*pcsPackingFile;
				int						lFileIndex;

				lFileIndex = 0;

				for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex) )
				{
					if( ppcsPackingFile && (*ppcsPackingFile) )
					{
						pcsPackingFile = (*ppcsPackingFile);
					}
				}
			}
		}
	}

	//Res File을 빌드한다.

	//어차피 작업 다 끝나면 종료해야한다.
	OnOK();

	return bResult;
}

bool CPatcher2Dlg::ProcessRebuild( CDList<CPatchVersionInfo *> *pcsPatchVersionInfo )
{
	CListNode<CPatchVersionInfo *>		*pcsPatchVersionNode;
	CListNode<CPatchFolderInfo *>		*pcsPatchFolderNode;
	CListNode<CPackingFile *>			*pcsPatchFileNode;
	CDList<CPatchVersionInfo *>			csRebuildPatchVersionInfo;

	//	CPackingFolder						**ppcsPackingFolder;
	CPatchVersionInfo					*pcsRebuildPatchVersionInfo;

	ApAdmin								csFolderInfo;
	char								strBuffer[ MAX_PATH ];
	char								strSourceFolder[ MAX_PATH ];
	bool								bResult;

	bResult = false;

	csFolderInfo.InitializeObject( sizeof(CPackingFolder *), 1000 );

	//현재폴더를 얻어낸다.
	GetCurrentDirectory( sizeof(m_strSourceFolder), m_strSourceFolder );

	sprintf_s( strSourceFolder, MAX_PATH , "%s\\%s", m_strSourceFolder, m_strRawFileDir );

	//Ignore File을 읽어들인다.
	m_csAuPackingManager.LoadIgnoreFileInfo( strSourceFolder, "IgnoreFileInfo.txt" );

	//역순으로 정렬한다.
	for( pcsPatchVersionNode=pcsPatchVersionInfo->GetEndNode(); pcsPatchVersionNode; pcsPatchVersionNode=pcsPatchVersionNode->m_pcPrevNode )
	{
		pcsRebuildPatchVersionInfo = new CPatchVersionInfo;

		//버전, 코드를 얻어낸다.
		pcsRebuildPatchVersionInfo->m_lVersion = pcsPatchVersionNode->m_tData->m_lVersion;

		for( pcsPatchFolderNode=pcsPatchVersionNode->m_tData->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
		{
			int					lFolderNameLen;

			//공통처리
			lFolderNameLen = strlen(pcsPatchFolderNode->m_tData->m_pstrFolderName)+1;

			//ApAdmin Packing Folder
			CPackingFolder		*pcsPackingFolder;
			CPackingFolder		**ppcsPackingFolder;

			ppcsPackingFolder = (CPackingFolder **)csFolderInfo.GetObject( _strlwr(pcsPatchFolderNode->m_tData->m_pstrFolderName ));

			if( NULL == ppcsPackingFolder )
			{
				pcsPackingFolder = new CPackingFolder;

				pcsPackingFolder->m_lFolderNameSize = lFolderNameLen;
				pcsPackingFolder->m_pstrFolderName = new char[lFolderNameLen];
				memset( pcsPackingFolder->m_pstrFolderName, 0, lFolderNameLen );
				strcat( pcsPackingFolder->m_pstrFolderName, pcsPatchFolderNode->m_tData->m_pstrFolderName );

				csFolderInfo.AddObject( (VOID **)&pcsPackingFolder, _strlwr(pcsPackingFolder->m_pstrFolderName) );
			}
			else
			{
				pcsPackingFolder = *ppcsPackingFolder;
			}

			//Packing List
			CPatchFolderInfo	*pcsPatchFolderInfo;
			bool				bFileInFolder;

			pcsPatchFolderInfo = new CPatchFolderInfo;

#ifndef _TEST_SERVER_
			pcsPatchFolderInfo->m_bPacking = pcsPatchFolderNode->m_tData->m_bPacking;
#else
			pcsPatchFolderInfo->m_bPacking = false;
#endif

			//폴더이름 복사.
			pcsPatchFolderInfo->m_pstrFolderName = new char[lFolderNameLen];
			memset( pcsPatchFolderInfo->m_pstrFolderName, 0, lFolderNameLen );
			strcat( pcsPatchFolderInfo->m_pstrFolderName, pcsPatchFolderNode->m_tData->m_pstrFolderName );

			pcsRebuildPatchVersionInfo->m_clFolderList.Add( pcsPatchFolderInfo );

			bFileInFolder = false;

			for( pcsPatchFileNode = pcsPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pcsPatchFileNode; pcsPatchFileNode=pcsPatchFileNode->m_pcNextNode )
			{
				//들어갔는가?
				if( pcsPackingFolder->m_csFilesInfo.AddObject( (void **)&pcsPatchFileNode->m_tData, _strlwr(pcsPatchFileNode->m_tData->m_pstrFileName) ) )
				{
					CPackingFile			*pcsPackingFile;
					char					*pstrFolderName, *pstrFileName;
					bool					bPacking;

					pcsPackingFile = new CPackingFile;

					pstrFolderName = pcsPatchFolderNode->m_tData->m_pstrFolderName;
					pstrFileName = pcsPatchFileNode->m_tData->m_pstrFileName;

					//Packing여부는 Ignore File정보에 따라 결정하자~
					if( !stricmp( "root", pstrFolderName ) )
					{
						sprintf_s( strBuffer, MAX_PATH , "%s\\%s", strSourceFolder, pstrFileName );
					}
					else
					{
						sprintf_s( strBuffer, MAX_PATH , "%s\\%s\\%s", strSourceFolder, pstrFolderName, pstrFileName );
					}

					//
					if( m_csAuPackingManager.IsIgnoreFileName( _strlwr(strBuffer) ) )
					{
						bPacking = false;                        
					}
					else
					{
						bPacking = true;
					}

#ifndef _TEST_SERVER_
					pcsPackingFile->m_bPacking = bPacking;
#else
					pcsPackingFile->m_bPacking = false;
#endif

					pcsPackingFile->m_lFileCheckSum = pcsPatchFileNode->m_tData->m_lFileCheckSum;
					pcsPackingFile->m_lFileNameSize = pcsPatchFileNode->m_tData->m_lFileNameSize;
					pcsPackingFile->m_lOperation = pcsPatchFileNode->m_tData->m_lOperation;

					//Size, StartPos는 필요없다.
					//pcsPackingFile->m_pcsDataNode->m_lSize;
					//pcsPackingFile->m_pcsDataNode->m_lStartPos;

					pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
					memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );
					strcat( pcsPackingFile->m_pstrFileName, pcsPatchFileNode->m_tData->m_pstrFileName );

					//리스트에추가
					pcsPatchFolderInfo->m_clFileList.Add( pcsPackingFile );
					bFileInFolder = true;
				}
			}

			//추가된 파일이 하나도 없으면 
			if( bFileInFolder == false )
			{
				pcsRebuildPatchVersionInfo->m_clFolderList.Remove(pcsPatchFolderInfo);
			}			
		}

		//파일이 있으면 JZP파일을 만든다. 아니면 말고~
		if( pcsRebuildPatchVersionInfo->m_clFolderList.getCount() > 0 )
		{
			int			lVer;

			lVer = pcsRebuildPatchVersionInfo->m_lVersion;

			m_csAuPackingManager.BuildJZPFile( lVer, m_strRebuildDir, pcsRebuildPatchVersionInfo, this->GetSafeHwnd(), m_cPatchFileProgress.GetSafeHwnd() );

			csRebuildPatchVersionInfo.AddHead(pcsRebuildPatchVersionInfo);
		}
		else
		{
			delete pcsRebuildPatchVersionInfo;
		}
	}

	//Res파일을 빌드한다.
	char			strRebuildResFileName[ MAX_PATH ];

	sprintf_s( strRebuildResFileName, MAX_PATH , "%s\\Patch_%04d_Rebuild.Res", m_strPatchResBackupDir, m_csAuPackingManager.m_lCurrentVersion );
	bResult = BuildResFile( &csRebuildPatchVersionInfo, strRebuildResFileName, m_strRawFileDir, m_strRebuildDir );

	return bResult;
}

BOOL CPatcher2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	setZipDir( "ZipDirs" );
	setRawFileDir( "RawFiles" );
	setResDir( "PatchServerRes" );
	setResortJZPDir( "PatchResortRes" );
	setPatchResBackupDir( "PatchResBackup" );
	setRebuildDir( "RebuildResult" );
	setDebugInfoDir( "DebugInfo" );
	setPackingDir( "Packing" );

	if (!ReadConfig())
	{
		AfxMessageBox("Error Reading Configuration File !!!", MB_OK);
	}

	m_csAuPackingManager.LoadResFile( "Patch.Res" );

	
    m_cCheckBuildJZP.SetCheck( BST_CHECKED );
    m_cCheckBuildPackingData.SetCheck( BST_CHECKED );
    m_csCheckCopyRawFiles.SetCheck( BST_CHECKED );
	m_btnLog.SetCheck( BST_CHECKED );

	m_lCurrentDisplayVersion = m_csAuPackingManager.GetLastVersion();
	++m_lCurrentDisplayVersion;

	DisplayPatchVersion( m_lCurrentDisplayVersion );
	UpdateListBox( m_lCurrentDisplayVersion );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPatcher2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CPatcher2Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CPatcher2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool CPatcher2Dlg::BuildResFile( CDList<CPatchVersionInfo *> *pcsPatchVersionInfo, char *pstrResFileName, char *pstrRawDir, char *pstrResDir )
{

	FILE			*file;

	bool			bResult;

	bResult = false;

	if( pstrResFileName )
	{
		file = fopen( pstrResFileName, "wb" );

		if( file )
		{
			CListNode<CPatchVersionInfo *>		*pcsPatchVersionNode;
			CListNode<CPatchFolderInfo *>		*pcsPatchFolderNode;
			CListNode<CPackingFile *>			*pcsPatchFileNode;

			bool			bBuildJZP;
			bool			bBuildDat;
			bool			bCopyRawFiles;
			int				lMaxPatchCount;
			int				lMaxPatchFolderCount;
			int				lMaxPatchFileCount;
			CHAR			szTemp[ 256 ];

			bBuildJZP = m_csAuPackingManager.IsCheckedBuildJZP();
			bBuildDat = m_csAuPackingManager.IsCheckedBuildDat();
			bCopyRawFiles = m_csAuPackingManager.IsCheckedCopyRawFiles();

			fwrite( &bBuildJZP, 1, sizeof(bool), file );
			fwrite( &bBuildDat, 1, sizeof(bool), file );
			fwrite( &bCopyRawFiles, 1, sizeof(bool), file );

			//총 버전수~
			lMaxPatchCount = pcsPatchVersionInfo->getCount();

			fwrite( &lMaxPatchCount, 1, sizeof(int), file );

			for( pcsPatchVersionNode=pcsPatchVersionInfo->GetStartNode(); pcsPatchVersionNode; pcsPatchVersionNode=pcsPatchVersionNode->m_pcNextNode )
			{
				//버전 정보를 저장한다.
				fwrite( &pcsPatchVersionNode->m_tData->m_lVersion, 1, sizeof(int), file );

				//폴더수를 저장한다.
				lMaxPatchFolderCount = pcsPatchVersionNode->m_tData->m_clFolderList.getCount();

				fwrite( &lMaxPatchFolderCount, 1, sizeof(int), file );

				for( pcsPatchFolderNode=pcsPatchVersionNode->m_tData->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
				{
					int				lFolderNameLen;
					int				lFileCount;

					lFolderNameLen = strlen( pcsPatchFolderNode->m_tData->m_pstrFolderName) + 1;

					//Folder Name Length
					fwrite( &lFolderNameLen, 1, sizeof(int), file );
					//Folder Name
					fwrite( pcsPatchFolderNode->m_tData->m_pstrFolderName, 1, lFolderNameLen, file );

					lMaxPatchFileCount = pcsPatchFolderNode->m_tData->m_clFileList.getCount();

					fwrite( &lMaxPatchFileCount, 1, sizeof(int), file );

					lFileCount = 0;
					for( pcsPatchFileNode = pcsPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pcsPatchFileNode; pcsPatchFileNode=pcsPatchFileNode->m_pcNextNode )
					{
						++lFileCount;
						//Operation
						fwrite( &pcsPatchFileNode->m_tData->m_lOperation, 1, sizeof(int), file );

						//FileNameSize
						fwrite( &pcsPatchFileNode->m_tData->m_lFileNameSize, 1, sizeof(int), file );

						//FileName
						fwrite( pcsPatchFileNode->m_tData->m_pstrFileName, 1, pcsPatchFileNode->m_tData->m_lFileNameSize, file );

						//FileSize
						fwrite( &pcsPatchFileNode->m_tData->m_lFileSize, 1, sizeof(int), file );

						//CheckSum
						fwrite( &pcsPatchFileNode->m_tData->m_lFileCheckSum, 1, sizeof(int), file );
					}
				}
				sprintf_s( szTemp , 256 , "[완료] : Res Version 정보 저장 %d / %d", pcsPatchVersionNode->m_tData->m_lVersion , lMaxPatchCount );
				AddListString( szTemp );
			}

			//JZP File정보를 저장한다.
			int				iJZPFiles;

			iJZPFiles = pcsPatchVersionInfo->getCount();

			//JZPFile의수를 저장한다.
			fwrite( &iJZPFiles, 1, sizeof( iJZPFiles ), file );

			// 패치 옵션
			switch( m_PatchType )
			{
			case PATCH_FAST:		// 빠르게 Res 파일을 묶는다			
				_FastMakeJZPInfo	( pstrResFileName , pstrRawDir , pstrResDir , file );	
				break;

			case PATCH_NORMAL:		// 처음부터 Res 파일을 묶는다( 초 느리다 )
				_NormalMakeJZPInfo	( pcsPatchVersionInfo , pstrResFileName , pstrRawDir , pstrResDir , file );
				break;
			}			
		}
	}

	fclose( file );
	return true;
}

VOID CPatcher2Dlg::_FastMakeJZPInfo( char *pstrResFileName, char *pstrRawDir, char *pstrResDir , FILE* pFile )
{
	CJZPFileInfo*	pJZPFileInfo	=	NULL;
	BOOL			bAddPatch		=	FALSE;
	CHAR			szTemp[	MAX_PATH	];
	ZeroMemory( szTemp , MAX_PATH );

	for( INT i = 1 ; i <= m_csAuPackingManager.GetLastVersion() ; ++i )
	{
		pJZPFileInfo		=	m_csAuPackingManager.GetJZPInfo( i );

		// 리빌드 같은경우 for문을 끝까지 다 돌지만...
		// 패치를 새로 만드는 경우에는 파일에서 얻어 올 수 없으므로
		// 밑에서 작업을 해줘야 한다 ㅎㅎ
		if( !pJZPFileInfo )
		{
			if( m_csAuPackingManager.GetLastVersion() == i )
			{
				bAddPatch	=	TRUE;
				break;
			}

			continue;
		}

		// Version 정보
		fwrite( &pJZPFileInfo->m_iVersion, 1, sizeof( INT ) , pFile );

		//pFileNameSize를 저장한다. NULL문자 포함
		fwrite( &pJZPFileInfo->m_iFileNameSize, 1, sizeof( INT ) , pFile  );

		//pFileName을 저장한다.
		fwrite( pJZPFileInfo->m_pstrFileName , 1 , pJZPFileInfo->m_iFileNameSize , pFile );

		//pFileSize를 저장한다.
		fwrite( &pJZPFileInfo->m_iFileSize , 1 , sizeof( INT ) , pFile );

		//압축을 풀었을때의 사이즈를 저장한다.
		fwrite( &pJZPFileInfo->m_iRawFileSize , 1 , sizeof( INT ) , pFile );

		//CRC CheckSum을 저장한다.
		fwrite( &pJZPFileInfo->m_iCRCCheckSum , 1 , sizeof( unsigned long ) , pFile );

		sprintf_s( szTemp , 256 , "[완료] : JZP 정보 저장 %d / %d", pJZPFileInfo->m_iVersion , m_csAuPackingManager.GetLastVersion() );
		AddListString( szTemp );
	}

	// 리빌드는 여기까지다 ^_^
	if( !bAddPatch )	return;

	// 패치를 새로 만드는 경우 아래까지 실행된다
	// 소스 정리해야 된다 ㅡ ㅡ안습
	CPatchVersionInfo*		pPatchVersionInfo	=	m_csAuPackingManager.GetPatchInfo( m_csAuPackingManager.GetLastVersion() );
	ASSERT( pPatchVersionInfo );

	CListNode<CPatchFolderInfo *>		*pPatchFolderNode	=	NULL;
	CListNode<CPackingFile *>			*pPatchFileNode		=	NULL;

	for( pPatchFolderNode = pPatchVersionInfo->m_clFolderList.GetStartNode() ; pPatchFolderNode ; pPatchFolderNode = pPatchFolderNode->m_pcNextNode )
	{
		char			strJZPPath[ MAX_PATH ];
		char			strJZPFileName[ MAX_PATH ];
		int				iFileNameSize;
		int				iFileSize;
		int				iRawFileSize; //해당 Jzp파일의 압축을 풀었을때 사이즈.
		int				iVersion;
		unsigned long	iHeaderSum,iCheckSum;

		iVersion = pPatchVersionInfo->m_lVersion;
		sprintf_s( strJZPFileName	,  MAX_PATH  , "Patch%04d.JZP"	, iVersion );
		sprintf_s( strJZPPath		,  MAX_PATH  , "%s\\%s"		, pstrResDir , strJZPFileName );
		iFileNameSize = strlen( strJZPFileName ) + 1;

		//FileSize를 구한다.
		FILE		*fpJZP;
		fpJZP	= fopen( strJZPPath, "rb" );

		if( fpJZP != NULL )
		{
			fseek( fpJZP, 0, SEEK_END );
			iFileSize = ftell( fpJZP );
			fseek( fpJZP, 0, SEEK_SET );
			
			//CRC CheckSum을 구한다.
			MapFileAndCheckSum( strJZPPath, &iHeaderSum, &iCheckSum );

			//iRawFileSize를 구한다.
			iRawFileSize = 0;

			for( pPatchFileNode = pPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pPatchFileNode; pPatchFileNode=pPatchFileNode->m_pcNextNode )
			{
				FILE			*fpRawFile;
				char			strRawFile[MAX_PATH];

				if( !strcmp( "root", pPatchFolderNode->m_tData->m_pstrFolderName ) )
				{
					sprintf_s( strRawFile,  MAX_PATH  , "%s\\%s", pstrRawDir, pPatchFileNode->m_tData->m_pstrFileName );
				}
				else
				{
					sprintf_s( strRawFile,  MAX_PATH  , "%s\\%s%s", pstrRawDir, pPatchFolderNode->m_tData->m_pstrFolderName, pPatchFileNode->m_tData->m_pstrFileName );
				}

				fpRawFile = fopen( strRawFile, "rb" );

				if( fpRawFile != NULL )
				{
					fseek( fpRawFile, 0, SEEK_END );
					iRawFileSize += ftell( fpRawFile );

					fclose( fpRawFile );
				}
			}

			fwrite( &iVersion, 1, sizeof( iVersion ), pFile );
			fwrite( &iFileNameSize, 1, sizeof( iFileNameSize ), pFile );
			fwrite( strJZPFileName, 1, iFileNameSize, pFile );
			fwrite( &iFileSize, 1, sizeof(iFileSize), pFile );
			fwrite( &iRawFileSize, 1, sizeof(iRawFileSize), pFile );
			fwrite( &iCheckSum, 1, sizeof(unsigned long), pFile );

			fclose( fpJZP );
			sprintf_s( szTemp ,  MAX_PATH  , "[완료] : JZP 정보 저장 %d / %d", iVersion , m_csAuPackingManager.GetLastVersion() );
			AddListString( szTemp );

			break;
		}
		else
		{
			break;
		}
	
	}

}

VOID CPatcher2Dlg::_NormalMakeJZPInfo( CDList<CPatchVersionInfo *> *pcsPatchVersionInfo , char *pstrResFileName, char *pstrRawDir, char *pstrResDir , FILE* pFile )
{
	CListNode<CPatchVersionInfo *>		*pcsPatchVersionNode	=	NULL;
	CListNode<CPatchFolderInfo *>		*pcsPatchFolderNode		=	NULL;
	CListNode<CPackingFile *>			*pcsPatchFileNode		=	NULL;
	CHAR								szTemp[  MAX_PATH  ];
	ZeroMemory( szTemp ,  MAX_PATH  );

	//JZPFile의 정보를 저장한다.
	for( pcsPatchVersionNode = pcsPatchVersionInfo->GetStartNode(); pcsPatchVersionNode; pcsPatchVersionNode=pcsPatchVersionNode->m_pcNextNode )
	{
		for( pcsPatchFolderNode = pcsPatchVersionNode->m_tData->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
		{
			char			strJZPPath[ MAX_PATH ];
			char			strJZPFileName[ MAX_PATH ];
			int				iFileNameSize;
			int				iFileSize;
			int				iRawFileSize; //해당 Jzp파일의 압축을 풀었을때 사이즈.
			int				iVersion;
			unsigned long	iHeaderSum,iCheckSum;

			iVersion = pcsPatchVersionNode->m_tData->m_lVersion;
			sprintf_s( strJZPFileName	,  MAX_PATH  , "Patch%04d.JZP"	, iVersion );
			sprintf_s( strJZPPath		,  MAX_PATH  , "%s\\%s"		, pstrResDir , strJZPFileName );
			iFileNameSize = strlen( strJZPFileName ) + 1;

			//FileSize를 구한다.
			FILE		*fpJZP;
			fpJZP	= fopen( strJZPPath, "rb" );

			if( fpJZP != NULL )
			{
				fseek( fpJZP, 0, SEEK_END );
				iFileSize = ftell( fpJZP );
				fseek( fpJZP, 0, SEEK_SET );

				//CRC CheckSum을 구한다.
				MapFileAndCheckSum( strJZPPath, &iHeaderSum, &iCheckSum );

				//iRawFileSize를 구한다.
				iRawFileSize = 0;

				for( pcsPatchFileNode = pcsPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pcsPatchFileNode; pcsPatchFileNode=pcsPatchFileNode->m_pcNextNode )
				{
					FILE			*fpRawFile;
					char			strRawFile[ MAX_PATH ];

					if( !strcmp( "root", pcsPatchFolderNode->m_tData->m_pstrFolderName ) )
					{
						sprintf_s( strRawFile,  MAX_PATH  , "%s\\%s", pstrRawDir, pcsPatchFileNode->m_tData->m_pstrFileName );
					}
					else
					{
						sprintf_s( strRawFile,  MAX_PATH  , "%s\\%s%s", pstrRawDir, pcsPatchFolderNode->m_tData->m_pstrFolderName, pcsPatchFileNode->m_tData->m_pstrFileName );
					}

					fpRawFile = fopen( strRawFile, "rb" );

					if( fpRawFile != NULL )
					{
						fseek( fpRawFile, 0, SEEK_END );
						iRawFileSize += ftell( fpRawFile );

						fclose( fpRawFile );
					}
				}

				fwrite( &iVersion, 1, sizeof( iVersion ), pFile );
				fwrite( &iFileNameSize, 1, sizeof( iFileNameSize ), pFile );
				fwrite( strJZPFileName, 1, iFileNameSize, pFile );
				fwrite( &iFileSize, 1, sizeof(iFileSize), pFile );
				fwrite( &iRawFileSize, 1, sizeof(iRawFileSize), pFile );
				fwrite( &iCheckSum, 1, sizeof(unsigned long), pFile );

				fclose( fpJZP );
				sprintf_s( szTemp ,  MAX_PATH  , "[완료] : JZP 정보 저장 %d / %d", iVersion , m_csAuPackingManager.GetLastVersion() );
				AddListString( szTemp );

				break;
			}
			else
			{
				break;
			}
		}


	}
}


void CPatcher2Dlg::DisplayPatchedFiles()
{
	CPackingFolder		**ppcsPackingFolder;
	CPackingFile		**ppcsPackingFile;
	ApAdmin				*pcsFolderInfo;
	char				strFilePath[ MAX_PATH ];
	INT					lFolderIndex;
	INT					lFileIndex;

	lFolderIndex = 0;

	pcsFolderInfo = m_csAuPackingManager.GetFolderInfo();

	for( ppcsPackingFolder = (CPackingFolder **)pcsFolderInfo->GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder **)pcsFolderInfo->GetObjectSequence( &lFolderIndex ))
	{
		if( ppcsPackingFolder && (*ppcsPackingFolder) )
		{
			lFileIndex = 0;

			for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex); ppcsPackingFile;  ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex) )
			{
				if( ppcsPackingFile && (*ppcsPackingFile) )
				{
					sprintf_s( strFilePath, MAX_PATH , "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName );

					if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Add )
					{
						m_cPatchedFilesList.AddString( _strlwr(strFilePath) );
					}
					else if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
					{
						//지우기면 아무일도 없다.
						;
					}
				}
			}
		}        
	}
}

void CPatcher2Dlg::UpdateListBox( int lVersion )
{
	CPatchVersionInfo			*pcsPatchVersionInfo;

	m_cFileInfoList.ResetContent();
	m_cPatchedFilesList.ResetContent();
	m_cRemoveFileList.ResetContent();

	pcsPatchVersionInfo = m_csAuPackingManager.GetPatchInfo(lVersion);

	if( pcsPatchVersionInfo )
	{
		CListNode<CPatchFolderInfo *>		*pcsPatchFolderNode;
		CListNode<CPackingFile *>			*pcsPatchFileNode;

		char				strFullPath[ MAX_PATH ];

		for( pcsPatchFolderNode=pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
		{
			for( pcsPatchFileNode = pcsPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pcsPatchFileNode; pcsPatchFileNode=pcsPatchFileNode->m_pcNextNode )
			{
				sprintf_s( strFullPath,  MAX_PATH , "%s\\%s", pcsPatchFolderNode->m_tData->m_pstrFolderName, pcsPatchFileNode->m_tData->m_pstrFileName );

				if( pcsPatchFileNode->m_tData->m_lOperation == Packing_Operation_Add )
				{
                    m_cFileInfoList.AddString( strFullPath );
				}
				else if( pcsPatchFileNode->m_tData->m_lOperation == Packing_Operation_Remove )
				{
					m_cRemoveFileList.AddString( strFullPath );
				}
			}
		}
	}
	else
	{
		//Res파일을 읽었으니 지금까지 패치된 파일리스트에 넣는다.
		DisplayPatchedFiles();
	}
}

void CPatcher2Dlg::DisplayPatchVersion( int lVersion )
{
	char			strVersion[ MAX_PATH ];

	sprintf_s( strVersion,  MAX_PATH  ,  "Patch Version : %d", lVersion );
	m_cPatchVersionStatic.SetWindowText( strVersion );
}

void CPatcher2Dlg::OnRemoveScanFolderButton()
{
	UpdateData();

	LPITEMIDLIST pidlRoot = NULL;
    LPITEMIDLIST pidlSelected = NULL;
    BROWSEINFO bi = {0};
    LPMALLOC pMalloc = NULL;
	char		pszDisplayName[256];

    SHGetMalloc(&pMalloc);

    bi.hwndOwner = this->m_hWnd;
    bi.pidlRoot = pidlRoot;
    bi.pszDisplayName = pszDisplayName;
    bi.lpszTitle = "Choose a Remove Folder";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;

    pidlSelected = SHBrowseForFolder(&bi);

    if(pidlRoot)
    {
        pMalloc->Free(pidlRoot);
    }

    pMalloc->Release();

	if( pidlSelected != NULL )
	{
		SHGetPathFromIDList(pidlSelected, m_strSourceFolder );

		//맨 윗닷은 무조건 root로 저장된다.
		m_csAuPackingManager.AddFolder( "root" );

		m_bAddList = true;
	}

	//파일 갯수를 얻어내고 세팅한다.
	m_lMaxProcessFileCount = 0;
	m_lProcessFileCount = 0;
	//GetMaxProcessFileCount( m_strSourceFolder );
	//m_cPatchFileProgress.SetRange( 0, m_lMaxProcessFileCount );


	//Remove List를 날린다.
	m_cRemoveFileList.ResetContent();

	CPackingFolder		**ppcsPackingFolder;
	CPackingFile		**ppcsPackingFile;
	ApAdmin				*pcsFolderInfo;
	char				strFilePath[ MAX_PATH ];
	int					lFolderIndex;
	int					lFileIndex;

	lFolderIndex = 0;

	pcsFolderInfo = m_csAuPackingManager.GetFolderInfo();

	for( ppcsPackingFolder = (CPackingFolder **)pcsFolderInfo->GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder **)pcsFolderInfo->GetObjectSequence( &lFolderIndex ))
	{
		if( ppcsPackingFolder && (*ppcsPackingFolder) )
		{
			lFileIndex = 0;

			if (!strncmp((*ppcsPackingFolder)->m_pstrFolderName, "root", 4) ||
				!strncmp((*ppcsPackingFolder)->m_pstrFolderName, "ini", 3))
				continue;

			for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex); ppcsPackingFile;  ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex) )
			{
				if( ppcsPackingFile && (*ppcsPackingFile) && (*ppcsPackingFile)->m_lOperation == Packing_Operation_Add )
				{
					sprintf_s( strFilePath, MAX_PATH , "%s\\%s\\%s", m_strSourceFolder, (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName );

					WIN32_FIND_DATA FindFileData;
					HANDLE	hFind	= FindFirstFile(strFilePath, &FindFileData);
					if (hFind == INVALID_HANDLE_VALUE)
					{
						CHAR	szRemoveFileName[ MAX_PATH ];
						ZeroMemory(szRemoveFileName, sizeof(szRemoveFileName));

						sprintf_s(szRemoveFileName, MAX_PATH , "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName);

						//같은 이름이 없으면 추가한다.
						if( LB_ERR == m_cRemoveFileList.FindStringExact( -1, szRemoveFileName ) )
						{
							m_cRemoveFileList.AddString( szRemoveFileName );
						}
					}
					else
					{
						FindClose(hFind);
					}
				}
			}
		}        
	}


	CHAR szTemp[ 20 ];
	ZeroMemory( szTemp , sizeof(szTemp) );

	sprintf_s( szTemp , "%d" , m_cRemoveFileList.GetCount() );
	m_staticDelFile.SetWindowText( szTemp );
}

void CPatcher2Dlg::OnScanFolderButton() 
{
	UpdateData();
	

	CFullScanOptionDlg		OptionDlg;
	CString					lastDate;

	lastDate.Format( "%d월 %d일" , m_csConfig.m_csLastPatchDate / 100 , m_csConfig.m_csLastPatchDate % 100 );

	

	OptionDlg.m_strLastPatch	=	lastDate;

	if( OptionDlg.DoModal() == IDOK )
	{

		m_nMaxYear	=	OptionDlg.m_nMaxYear;
		m_nMaxMonth	=	OptionDlg.m_nMaxMonth;
		m_nMaxDay	=	OptionDlg.m_nMaxDay;

		m_nMinYear	=	OptionDlg.m_nMinYear;
		m_nMinMonth	=	OptionDlg.m_nMinMonth;
		m_nMinDay	=	OptionDlg.m_nMinDay;

		m_nFullScanType		=	OptionDlg.GetType();
	}

	else
	{
		return;
	}


    LPITEMIDLIST	pidlRoot		= NULL;
    LPITEMIDLIST	pidlSelected	= NULL;
    BROWSEINFO		bi				= {0};
    LPMALLOC		pMalloc			= NULL;
	CHAR			pszDisplayName[256];

    SHGetMalloc(&pMalloc);

    bi.hwndOwner		= this->m_hWnd;
    bi.pidlRoot			= pidlRoot;
    bi.pszDisplayName	= pszDisplayName;
    bi.lpszTitle		= "Choose a Patch Folder";
    bi.ulFlags			= 0;
    bi.lpfn				= NULL;
    bi.lParam			= 0;

    pidlSelected = SHBrowseForFolder(&bi);

	// NULL이면 Scan Folder에서 취소를 누른것이기 때문에
	// 바로 종료 시킨다
	if( pidlSelected == NULL )		return;
	
    if(pidlRoot)
    {
        pMalloc->Free(pidlRoot);
    }

    pMalloc->Release();

	if( pidlSelected != NULL )
	{
		SHGetPathFromIDList(pidlSelected, m_strSourceFolder );

		//맨 윗닷은 무조건 root로 저장된다.
		m_csAuPackingManager.AddFolder( "root" );

		m_bAddList = true;
	}

	//파일 갯수를 얻어내고 세팅한다.
	m_lMaxProcessFileCount = 0;
	m_lProcessFileCount = 0;
	m_cPatchFileProgress.SetRange( 0, m_lMaxProcessFileCount );

	m_eOperation		=	OPERATION_FULLSCAN;
	Start();

}

// 수동으로 폴더를 사용자가 선택해야 한다
// 폴더 선택은 상위 폴더를 해야 한다
VOID CPatcher2Dlg::OneFileAdd( LPCTSTR pFullPathFileName, bool bCompareCheckSum /* = false */ )
{

}

// 자동으로 폴더를 선택한다
// 추가하려는 폴더가 CurrentPath의 아래에 있어야 한다
VOID CPatcher2Dlg::AutoOneFileAdd( LPCTSTR pFullPathFileName, LPCTSTR pstrCurrentPath, CListBox *pcListBox, bool bCompareCheckSum /* = false */ )
{
	
	INT32				nLength			=	strlen( pFullPathFileName );
	CPackingFile*		pcsPackingFile	=	NULL;
	CListBoxData*		pListBoxData	=	new	CListBoxData;
	bool				bSameCheckSum	=	false;
	bool				bAddToListBox	=	true;
	CString				strFilePath		=	pFullPathFileName;
	CString				strFileName;
	CString				PatchPath;
	DWORD				dwHeaderSum	,	dwCheckSum;

	// 이 루프에서 파일의 이름과 경로를 따로 빼낸다
	for( INT i = nLength ; i > 0 ; --i )
	{
		if( pFullPathFileName[ i ]	==	'\\' )
		{
			strFileName		=	&pFullPathFileName[i+1];			//	File 이름만 넣는다
			strFilePath.Delete( i , strFileName.GetLength()+1 );	//	File의 경로만 넣는다

			PatchPath		=	strFilePath;
			PatchPath.Delete( PatchPath.Find( pstrCurrentPath ) , strlen(pstrCurrentPath) );
			break;
		}
	}

	if( PatchPath	==	"" )
		PatchPath	=	"root";

	else if( PatchPath[0] == '\\' )
	{
		PatchPath.Delete( 0 , 1 );
	}

	// 현재 파일이 존재하는지 확인한다
	FILE*	pFile		=	fopen( pFullPathFileName , "rb" );
	DWORD	dwFileSize	=	0;
	if( pFile != NULL )
	{
		fseek( pFile , 0 , SEEK_END );
		dwFileSize	=	ftell( pFile );

		pListBoxData->m_lFileSize		=	dwFileSize;

		if( dwFileSize	==	0 )
		{
			bAddToListBox	=	false;
		}

		fclose( pFile );
	}


	// CheckSum 비교가 체크박스에 체크 되었다면 업데이트 할 파일의 체크섬 비교를 한다
	if( bCompareCheckSum	==	true )
	{
		
		MapFileAndCheckSum( (char*)pFullPathFileName , &dwHeaderSum , &dwCheckSum );
		pListBoxData->m_lCheckSum		=	dwCheckSum;

		pcsPackingFile	=	m_csAuPackingManager.GetFile( (char*)PatchPath.GetString(), (char*)strFileName.GetString() );

		// 전에 있던 파일과 비교한다
		if( pcsPackingFile )
		{
			if( dwCheckSum == pcsPackingFile->m_lFileCheckSum )
			{
				bSameCheckSum	=	true;
			}

			else
			{
				
			}
		}

		// 이 옵션이 true라면 존재하는 파일만 update 받는다
		else if( m_bSyncExistingFile )
		{
			bSameCheckSum		=	true;
		}
	}

	// 추가할 수 있다면
	// ListBox에 추가한다
	if( bAddToListBox == true )
	{

		CHAR	szFullPath[ MAX_PATH ];
		ZeroMemory( szFullPath , MAX_PATH );

		sprintf_s( szFullPath , MAX_PATH , "%s\\%s" , PatchPath , strFileName );

		if( bSameCheckSum == false )
		{
			INT nIndex	=	pcListBox->AddString( szFullPath );
			pcListBox->SetItemDataPtr( nIndex , pListBoxData );
		}
		
		// 추가가 안된다면 그냥 지워버린다
		else
		{
			++m_nSkilFileCount;
			delete	pListBoxData;
		}

		// 만약 Remove 리스트에 추가된 파일이라면 지운다 ㅎ
		INT	nIndex	=	m_cRemoveFileList.FindStringExact( LB_ERR , szFullPath );
		if( nIndex	!=	LB_ERR )
		{
			m_cRemoveFileList.DeleteString( nIndex );
		}

	}
}

void CPatcher2Dlg::FindSubDirFiles( LPCTSTR pstrSourcePath, LPCTSTR pstrCurrentPath, CListBox *pcListBox, bool bCompareCheckSum )
{
	HANDLE				hHandle;
	WIN32_FIND_DATA		csFindData;
	char				strSearchFolder[ MAX_PATH ];

	bool				bResult;

	bResult = false;

	sprintf_s( strSearchFolder, MAX_PATH , "%s\\*.*", pstrCurrentPath );

	hHandle = FindFirstFile( strSearchFolder, &csFindData );

	while ( 1 )
	{
		if( FindNextFile( hHandle, &csFindData ) == FALSE )
			break;

		//폴더인가?
		if( csFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			//if( !strcmpi( ".", csFindData.cFileName ) || !strcmpi( "..", csFindData.cFileName ) )
			//아예 '.'으로 시작되는 폴더를 다 막자. ".", ",,", ".svn" 등이 다 막힌다.
			if( csFindData.cFileName[0] == '.' )
			{
				
				continue;
			}
			else
			{
				char				strBuffer[ MAX_PATH ];
				char				strFullPath[ MAX_PATH ];

				sprintf_s( strFullPath, MAX_PATH , "%s\\%s", pstrCurrentPath, csFindData.cFileName );

				if( m_csAuPackingManager.GetFolderName( strFullPath, pstrSourcePath, strBuffer, sizeof(strBuffer) ) )
				{
					m_csAuPackingManager.AddFolder( strBuffer );

					FindSubDirFiles( pstrSourcePath, strFullPath, pcListBox, bCompareCheckSum );
				}
			}
		}
		else
		{

			if( CheckLimitTime( csFindData.ftLastWriteTime ) )
				continue;

			CString			cExcludeFileName;

			cExcludeFileName = csFindData.cFileName;

			if( !cExcludeFileName.CompareNoCase( "COption.ini" ) || !cExcludeFileName.CompareNoCase( "vssver.scc" ) )
			{
				////////////////////////////////////////////////
				/*//이 경우는 아예 빼는경우임. 메시지 출력이라도 해줄까? */
				//////////////////////////////////////////////////
			}
			else if( !cExcludeFileName.CompareNoCase( "AlefClient.pdb" ) )
			{
				/////////////////////////////////
			}
			else
			{
				CString			strString;
				CListBoxData	*pcsListBoxData;

				FILE			*file;
				bool			bAddtoListBox;
				bool			bSameCheckSum;
				char			*pstrFileName;
				char			strFullPath[255];
				int				iRootSize;
				int				iFileNameLength;
				long			lFileSize;

				bAddtoListBox	= true;
				bSameCheckSum	= false;

				pcsListBoxData	= new CListBoxData;
				pstrFileName	= csFindData.cFileName;

				//pstrFileName앞에서 pstr에 해당하는 내용을 지운다.
				iRootSize		= strlen( pstrSourcePath ) + 1; // "\\", 즉 역슬래쉬를 지운다.
				iFileNameLength = strlen( pstrFileName );

				//파일에 실제 내용이 있는지 확인한다.
				sprintf( strFullPath, "%s\\%s", pstrCurrentPath, pstrFileName );

				if( !cExcludeFileName.CompareNoCase( "AlefClient.exe" ) )
				{
					char			strTargetPath[255];

					sprintf( strTargetPath, "%s\\%s\\%d\\%s", m_strDebugDir, m_strRegion.c_str() , m_csAuPackingManager.m_lCurrentVersion, cExcludeFileName );

					_CreateDirectory( "%s", m_strDebugDir );
					_CreateDirectory( "%s\\%s", m_strDebugDir, m_strRegion.c_str() );
					_CreateDirectory( "%s\\%s\\%d", m_strDebugDir, m_strRegion.c_str() , m_csAuPackingManager.m_lCurrentVersion );
	
					CopyFile(strFullPath, strTargetPath, FALSE);

					m_bExecutableAvailable = TRUE;
				}

				file = fopen( strFullPath, "rb" );

				if( file != NULL )
				{
					fseek( file, 0, SEEK_END );
					lFileSize = ftell( file );

					pcsListBoxData->m_lFileSize = lFileSize;

					if( lFileSize == 0 )
					{
						bAddtoListBox = false;
					}

					fclose( file );
				}

				char				strFolderName[ MAX_PATH ];
				char				strRelativePath[ MAX_PATH ];
				int					lFolderNameLen;

				m_csAuPackingManager.GetFolderName( strFullPath, pstrSourcePath, strRelativePath, sizeof(strFolderName) );

				_splitpath( strRelativePath, NULL, strFolderName, NULL, NULL );

				// strFolderName 마지막에 붙는 \를 삭제한다.
				lFolderNameLen = strlen(strFolderName);

				if( lFolderNameLen == 0 )
				{
					sprintf_s( strFolderName, MAX_PATH ,  "%s", "root" );
				}
				else
				{
					memset( &strFolderName[lFolderNameLen-1], 0, 1 );
				}

				unsigned long	iHeaderSum,iCheckSum;

				MapFileAndCheckSum( strFullPath, &iHeaderSum, &iCheckSum );
				pcsListBoxData->m_lCheckSum = iCheckSum;

				//CheckSum을 확인한다.
				if( bCompareCheckSum == true )
				{
					CPackingFile			*pcsPackingFile;

					pcsPackingFile = m_csAuPackingManager.GetFile( strFolderName, pstrFileName );

					if( pcsPackingFile )
					{
						if( iCheckSum == pcsPackingFile->m_lFileCheckSum )
						{
							bSameCheckSum = true;
						}
					}
					else if( m_bSyncExistingFile )
					{
						// 이 옵션은 현재 패치되어있는 파일만 패치하는 것이므로, bSameCheckSum을 true로 만들어서 List에서 뺀다.
						bSameCheckSum = true;
					}
				}

				//AddList에 넣을수 있다면 넣는다.
				if( bAddtoListBox == true )
				{
					int				lIndex;
					char			strBuffer[ MAX_PATH ];

					sprintf_s( strBuffer, MAX_PATH , "%s\\%s", strFolderName, pstrFileName );

					//CheckSum이 같다면 Add하지 않는다. 지우는 파일도 아니다.
					if( bSameCheckSum == false )
					{
						lIndex = pcListBox->AddString( strBuffer );
						pcListBox->SetItemDataPtr( lIndex, (void *)pcsListBoxData );
//						m_csAuPackingManager.AddFile( strFolderName, pstrFileName, pcsListBoxData->m_lFileSize, pcsListBoxData->m_lCheckSum );
					}
					else
					{
						delete pcsListBoxData;
					}

					//만일 RemoveBox에 Add시킨 파일이 있다면 지운다.
					lIndex = m_cRemoveFileList.FindStringExact( LB_ERR, strBuffer );

					if( lIndex != LB_ERR )
					{
						m_cRemoveFileList.DeleteString( lIndex );
					}
				}
				else
				{
					delete pcsListBoxData;
				}

				//다 읽었으니 프로그레스바를 증가시킨다.
				m_lProcessFileCount++;
				m_cPatchFileProgress.SetPos( m_lProcessFileCount );
			}
		}
	}

	//finder.Close();
	if( hHandle != INVALID_HANDLE_VALUE )	FindClose( hHandle );
}

void CPatcher2Dlg::GetMaxProcessFileCount( char *pstrFolderName )
{
    if( pstrFolderName )
	{
		CFileFind finder;

		CString strWildcard(pstrFolderName);
		strWildcard += _T("\\*.*");

		BOOL bWorking = finder.FindFile(strWildcard);

		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			if (finder.IsDots())
				continue;

			if (finder.IsDirectory())
			{
				GetMaxProcessFileCount( finder.GetFilePath().GetBuffer(0) );
			}
			else
			{
				CString			cExcludeFileName;

				cExcludeFileName = finder.GetFileName();

				if( !cExcludeFileName.CompareNoCase( "COption.ini" ) || !cExcludeFileName.CompareNoCase( "vssver.scc" ) )
				{
					//이 경우는 아예 빼는경우임. 메시지 출력이라도 해줄까? 
				}
				else if( !cExcludeFileName.CompareNoCase( "AlefClient.pdb" ) )
				{
				}
				else
				{
					m_lMaxProcessFileCount++;
				}
			}
		}

		finder.Close();
	}
}

BOOL CPatcher2Dlg::CopyFileToPatchFolder( char *pstrSPath, char *pstrDPath, char *pstrFullPath, BOOL bOverwrite )
{
	BOOL		bResult;
	char		strSFileName[256];

	memset( strSFileName, 0, 256 );
	sprintf( strSFileName, "%s\\%s", pstrSPath, pstrDPath );

	//복사될 위치에 같은 이름의 파일이있다면 그 파일의 속성을 세팅한다.
	SetFileAttributes( strSFileName, FILE_ATTRIBUTE_NORMAL );

	bResult = CopyFile( pstrFullPath, strSFileName, bOverwrite );

	if( bResult == FALSE )
	{
		int		iErrorCode;

		iErrorCode = GetLastError();
		
		//ErrorCode가 0x03이면 폴더가 없는경우!! 폴더를 만들어준다.
		if( iErrorCode == 0x03 )
		{
			bool			bGetDirName;
			int				iCurrentPos;
			int				iDPathSize;
			char			strDir[256];

			iCurrentPos = 0;
			bGetDirName = false;
			iDPathSize = strlen( pstrDPath );
			memset( strDir, 0, 256 );

			for( int i=0; i<iDPathSize; i++ )
			{
				strDir[iCurrentPos] = pstrDPath[i];
				iCurrentPos++;

				if( pstrDPath[i] == '\\' )
				{
					CreateDirectory( strDir, NULL );
					
					//파일을 복사한다.
					bResult = CopyFile( pstrFullPath, strSFileName, bOverwrite );

					if( bResult == TRUE )
					{
						break;
					}
					else
					{
						if( GetLastError() == 0x03 )
						{
							continue;
						}
						else
						{
							bResult = FALSE;
							break;
						}
					}
				}
			}			
		}
		else
		{
			if( iErrorCode == 80 )
			{
				MessageBox( pstrFullPath, strSFileName );
			}

			bResult = FALSE;
		}
	}

	//PatchFiles Folder로 복사한다. Remove파일선택과 현재 올라간 패치 파일들을 보기위해 
	//PatchFiles로 복사를한다.
	return bResult;
}

BOOL CPatcher2Dlg::Copy2BackupAndCompress( CPatchVersionInfo *pcsPatchVersionInfo )
{
	CListNode<CPatchFolderInfo *>		*pcsPatchFolderNode;
	CListNode<CPackingFile *>			*pcsPatchFileNode;

	BOOL			bResult;
	char			strCurrentDir	[ MAX_PATH ]; //현재 디렉토리를 저장한다.
	char			strZipDir		[ MAX_PATH ];
	char			strPatchDir		[ MAX_PATH ];   //패치를 위해 "현재디렉토리+version"이 저장되는 디렉토리다.
	char			strSourcePath	[ MAX_PATH ];
	char			strDestPath		[ MAX_PATH ];   //어디로 복사할것인가?
	char			strFullFileName	[ MAX_PATH ];
	char			strBuffer		[ MAX_PATH ];
	int				iRegistrationFiles;
	int				iMaxRegistrationFiles;

	bResult = TRUE;
	iRegistrationFiles = 0;
	iMaxRegistrationFiles = 0;

	//현재디렉토리를 기억한다.
	ZeroMemory( strCurrentDir , MAX_PATH );
	GetCurrentDirectory(  MAX_PATH , strCurrentDir );

	//현재디렉토리+패치버전을 기본디렉토리로 정한다.
	sprintf_s( strDestPath, MAX_PATH ,  "%s\\%d", m_strZipDir, m_csAuPackingManager.m_lCurrentVersion );
	CreateDirectory( strDestPath, NULL );

	sprintf_s( strZipDir, MAX_PATH ,  "%s\\%s\\%d", strCurrentDir, m_strZipDir, m_csAuPackingManager.m_lCurrentVersion );

	//복사하고 압축할 파일의 갯수를 구한다.
	for( pcsPatchFolderNode=pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
	{
		iMaxRegistrationFiles += pcsPatchFolderNode->m_tData->m_clFileList.getCount();
	}

	m_cPatchFileProgress.SetRange( 0, iMaxRegistrationFiles );
	
	if( m_btnLog.GetCheck()	==	BST_CHECKED )		
	{
		_WriteLog( m_strFileName , "----------------------------------------------------------------" );
		_WriteLog( m_strFileName , "총 파일 갯수 : %d" , iMaxRegistrationFiles );
		_WriteLog( m_strFileName , "----------------------------------------------------------------" );
	}

	//실제 파일을 압축,복사한다.
	for( pcsPatchFolderNode=pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
	{
		for( pcsPatchFileNode = pcsPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pcsPatchFileNode; pcsPatchFileNode=pcsPatchFileNode->m_pcNextNode )
		{
			//진행 상황 메시지출력
			sprintf_s( strBuffer, MAX_PATH ,  "Processing %s\\%s file", pcsPatchFolderNode->m_tData->m_pstrFolderName, pcsPatchFileNode->m_tData->m_pstrFileName );
			AddListString( strBuffer );

			//Root는 무시한다.
			if( !strcmpi("root", pcsPatchFolderNode->m_tData->m_pstrFolderName) )
			{
				sprintf_s( strFullFileName, MAX_PATH ,  "%s", pcsPatchFileNode->m_tData->m_pstrFileName );
			}
			else
			{
				sprintf_s( strFullFileName, MAX_PATH , "%s\\%s", pcsPatchFolderNode->m_tData->m_pstrFolderName, pcsPatchFileNode->m_tData->m_pstrFileName );
			}

			if( pcsPatchFileNode->m_tData->m_lOperation == Packing_Operation_Remove )
			{
				char			strFilePath[ MAX_PATH ];
				char			strCurrentDir[ MAX_PATH ];

				GetCurrentDirectory(  MAX_PATH , strCurrentDir );

				sprintf_s( strFilePath,  MAX_PATH , "%s\\%s\\%s", strCurrentDir, m_strRawFileDir, strFullFileName );

				//지우기전에 파일 속성을 노멀로한다. 그래야 지워진다.
				SetFileAttributes( strFilePath, FILE_ATTRIBUTE_NORMAL );
				DeleteFile( strFilePath );

				iRegistrationFiles++;
				m_cPatchFileProgress.SetPos( iRegistrationFiles );				
			}
			else if( pcsPatchFileNode->m_tData->m_lOperation == Packing_Operation_Add )
			{
				SetCurrentDirectory( strZipDir );

				//복사해 올 파일의 경로를 만들어낸다.
				sprintf_s( strSourcePath, MAX_PATH , "%s\\%s", m_strSourceFolder, strFullFileName );

				m_cCompress.compressFile( strSourcePath, strZipDir, strFullFileName );

				//PatchFiles 폴더로 파일들을 올린다.(최종패치 파일 확인)
				sprintf_s( strPatchDir, MAX_PATH , "%s\\%s", strCurrentDir, m_strRawFileDir );
				SetCurrentDirectory( strPatchDir );

				sprintf_s( strDestPath, MAX_PATH , "%s\\%s", m_strSourceFolder, strFullFileName );

				//같은 파일이 있으면 OverWrite한다!!
				if( CopyFileToPatchFolder( strPatchDir, strFullFileName, strDestPath, FALSE ) == FALSE )
				{
					bResult = FALSE;
				}

				iRegistrationFiles++;
				m_cPatchFileProgress.SetPos( iRegistrationFiles );				

				//모든작업이 다 끝났으니.... 원래 디렉토리로 돌린다.
				SetCurrentDirectory( strCurrentDir );
			}
		}
	}

	return bResult;
}

BOOL CPatcher2Dlg::_BuildPatch( VOID )
{

	::DeleteFile( m_strFileName );

	bool	bAddFileList		= false;
	bool	bRemoveFileList		= false;
	bool	bModifyIdentyFile	= false;

	if( m_lCurrentDisplayVersion != (m_csAuPackingManager.GetLastVersion()+1) )
	{
		MessageBox( "Version information is incorrect." );
		return false;
	}

	if( m_btnLog.GetCheck()	==	BST_CHECKED )
	{
		CString		strComment;

		m_editComment.GetWindowTextA( strComment );

		CreateDirectory( "Log", NULL );
		sprintf_s( m_strFileName , "Log\\Log_%d" , m_lCurrentDisplayVersion );
		::DeleteFile( m_strFileName );

		_WriteLog( m_strFileName , "----------------------------------------------------------------" );
		_WriteLog( m_strFileName , "패치 버전 : %d" , m_lCurrentDisplayVersion );
		_WriteLog( m_strFileName , "----------------------------------------------------------------" );
		_WriteLog( m_strFileName , " " );

		_WriteLog( m_strFileName , "---------------------------Comment------------------------------" );
		_WriteLog( m_strFileName , (char*)strComment.GetString() );
		_WriteLog( m_strFileName , "----------------------------------------------------------------" );
		_WriteLog( m_strFileName , " " );

		CString		TempString;

		_WriteLog( m_strFileName , "--------------------------File List-----------------------------" );

		for( INT i = 0 ; i < m_cFileInfoList.GetCount() ; ++i )
		{
			m_cFileInfoList.GetText( i , TempString );

			_WriteLog( m_strFileName , (CHAR*)TempString.GetString() );
			
		}

		_WriteLog( m_strFileName , "----------------------------------------------------------------" );
	}

	if( m_cFileInfoList.GetCount() > 0 )
	{
		bAddFileList = true;
	}

	if( m_cRemoveFileList.GetCount() )
	{
		//Remove File List에 있는 파일이 AddList에 있다면 패치를 할 수 없다.
		int				lMaxRemoveFileCount;
		char			strBuffer[ MAX_PATH ];

		lMaxRemoveFileCount = m_cRemoveFileList.GetCount();

		for( int lCounter=0; lCounter<lMaxRemoveFileCount; lCounter++ )
		{
			m_cRemoveFileList.GetText( lCounter, strBuffer );

			if( LB_ERR != m_cFileInfoList.FindStringExact( -1, strBuffer ) )
			{
				//만약 같은게 있다면?
				bModifyIdentyFile = true;
				break;
			}
		}

		bRemoveFileList = true;
	}

	if( bModifyIdentyFile )
	{
		MessageBox( "Same file exist at Add/Remove List." );
		return false;
	}

	if( bAddFileList || bRemoveFileList )
	{
		CPatchVersionInfo				*pcsPatchVersionInfo;
		CListNode<CPatchFolderInfo *>	*pcsFolderNode;

		CPatchFolderInfo		*pcsPatchFolderInfo;
		CListBoxData			*pcsListBoxData;

		char			strFileNameInListBox[255];
		char			strDrive[255];
		char			strDir[255];
		char			strFileName[255];
		char			strFileExt[255];
		char			strFileFullName[255];
		int				lCounter;
		int				lFolderNameLen;

		bool			bFolderFound;

		pcsPatchVersionInfo = new CPatchVersionInfo;

		m_csAuPackingManager.m_lCurrentVersion++;
		m_lCurrentDisplayVersion = m_csAuPackingManager.m_lCurrentVersion;

		//진행 상황 메시지출력
		AddListString( "------------------- Initializing -------------------" );

		//압축하지 않을 정보를 읽어들인다.
		if( bAddFileList )
		{
			m_csAuPackingManager.LoadIgnoreFileInfo( m_strSourceFolder, "IgnoreFileInfo.txt" );
		}

		//버전을 넣어준다.
		pcsPatchVersionInfo->m_lVersion = m_csAuPackingManager.m_lCurrentVersion;

		//추가할 파일에 대해서 연산을 한다.-------------------------------------------------------------------------------
		for( lCounter=0; lCounter< m_cFileInfoList.GetCount(); lCounter++ )
		{
			bFolderFound = false;

			m_cFileInfoList.GetText( lCounter, strFileNameInListBox );
			pcsListBoxData = (CListBoxData *)m_cFileInfoList.GetItemDataPtr( lCounter );

			_splitpath( strFileNameInListBox, strDrive, strDir, strFileName, strFileExt );
			sprintf( strFileFullName, "%s%s", strFileName, strFileExt );

			//strDir가장 뒤에 붙은 "\"를 없앤다.
			lFolderNameLen = strlen( strDir );
			if( lFolderNameLen > 0 )
			{
				if( strDir[lFolderNameLen-1] == '\\' )
				{
					memset( &strDir[lFolderNameLen-1], 0, 1 );
				}
			}
			else
			{
				sprintf( strDir, "%s", "root" );
			}

			//폴더네임, 파일이름을 뽑아냈으니 버전에 따른 폴더와 파일을 세팅해준다.
			//1. 우선 폴더가 있는지 본다.
			for( pcsFolderNode = pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsFolderNode; pcsFolderNode = pcsFolderNode->m_pcNextNode )
			{
				if( !strcmpi( pcsFolderNode->m_tData->m_pstrFolderName, strDir ) )
				{
					pcsPatchFolderInfo = pcsFolderNode->m_tData;
					bFolderFound = true;
					break;
				}
			}

			//없으면 추가한다.
			if( bFolderFound == false )
			{
				pcsPatchFolderInfo = new CPatchFolderInfo;

				lFolderNameLen = strlen( strDir ) + 1;

				pcsPatchFolderInfo->m_pstrFolderName = new char[lFolderNameLen];
				memset( pcsPatchFolderInfo->m_pstrFolderName, 0, lFolderNameLen );

				strcat( pcsPatchFolderInfo->m_pstrFolderName, strDir );

				pcsPatchVersionInfo->m_clFolderList.Add( pcsPatchFolderInfo );
			}

			if( pcsPatchFolderInfo )
			{
				CPackingFile		*pcsPackingFile;

				pcsPackingFile = new CPackingFile;

				//패킹할 파일인지 그냥 넘어갈 파일인지 구분한다.
				char			strBuffer[255];

				if( !strcmp( "root", strDir ) )
				{
					sprintf( strBuffer, "%s\\%s", m_strSourceFolder, strFileFullName );
				}
				else
				{
					sprintf( strBuffer, "%s\\%s", m_strSourceFolder, strFileNameInListBox );
				}

				//수정 2005.1.19 패킹안하기위해 주석처리.
				if( m_csAuPackingManager.IsIgnoreFileName( strBuffer ) )
				{
					pcsPackingFile->m_bPacking = false;
				}
				else
				{
					pcsPackingFile->m_bPacking = true;
				}

#ifdef _TEST_SERVER_
				pcsPackingFile->m_bPacking = false;
#endif

				//Operation
				pcsPackingFile->m_lOperation = Packing_Operation_Add;

				//FileNameSize
				pcsPackingFile->m_lFileNameSize = strlen( strFileFullName ) + 1;

				//FileName
				pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
				memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );
				strcat( pcsPackingFile->m_pstrFileName, strFileFullName );

				//파일크기
				pcsPackingFile->m_lFileSize = pcsListBoxData->m_lFileSize;
				//체크섬
				pcsPackingFile->m_lFileCheckSum = pcsListBoxData->m_lCheckSum;

				//List에 추가한다.
				pcsPatchFolderInfo->m_clFileList.Add( pcsPackingFile );

				//Tree에도 추가한다.
				m_csAuPackingManager.AddFile( strDir, pcsPackingFile, pcsListBoxData->m_lFileSize, pcsListBoxData->m_lCheckSum );

				//Tree의 Folder를 얻어와서 이놈은 새로추가된 폴더라고 세팅해준다.
				CPackingFolder			*pcsTempPackingFolder;
				pcsTempPackingFolder = m_csAuPackingManager.GetFolder( strDir );

				if( pcsTempPackingFolder )
				{
					pcsTempPackingFolder->m_bExportToRef = true;
				}
			}

			//다 썼으니 지운다.
			delete pcsListBoxData;
		}

		//지울 파일에 대해서 연산을 한다.---------------------------------------------------------------------------------
		for( lCounter=0; lCounter< m_cRemoveFileList.GetCount(); lCounter++ )
		{
			m_cRemoveFileList.GetText( lCounter, strFileNameInListBox );

			_splitpath( strFileNameInListBox, strDrive, strDir, strFileName, strFileExt );
			sprintf( strFileFullName, "%s%s", strFileName, strFileExt );

			strDir[strlen( strDir )-1] = 0;

			bFolderFound = false;

			//1. 우선 폴더가 있는지 본다.
			for( pcsFolderNode = pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsFolderNode; pcsFolderNode = pcsFolderNode->m_pcNextNode )
			{
				if( !strcmpi( pcsFolderNode->m_tData->m_pstrFolderName, strDir ) )
				{
					pcsPatchFolderInfo = pcsFolderNode->m_tData;
					bFolderFound = true;
					break;
				}
			}

			//없으면 추가한다.
			if( bFolderFound == false )
			{
				pcsPatchFolderInfo = new CPatchFolderInfo;

				lFolderNameLen = strlen( strDir ) + 1;

				pcsPatchFolderInfo->m_pstrFolderName = new char[lFolderNameLen];
				memset( pcsPatchFolderInfo->m_pstrFolderName, 0, lFolderNameLen );

				strcat( pcsPatchFolderInfo->m_pstrFolderName, strDir );

				pcsPatchVersionInfo->m_clFolderList.Add( pcsPatchFolderInfo );
			}

			if( pcsPatchFolderInfo )
			{
				CPackingFile		*pcsPackingFile;

				pcsPackingFile = new CPackingFile;

				//패킹할 파일인지 그냥 넘어갈 파일인지 구분한다.
				char			strBuffer[255];
				ZeroMemory( strBuffer , 255 );

				CPackingFile*	pFile = m_csAuPackingManager.GetFile( strDir , strFileFullName );

				pcsPackingFile->m_bPacking	=	pFile->m_bPacking;

#ifdef _TEST_SERVER_
				pcsPackingFile->m_bPacking = false;
#endif

				//Operation
				pcsPackingFile->m_lOperation = Packing_Operation_Remove;

				//FileNameSize
				pcsPackingFile->m_lFileNameSize = strlen( strFileFullName ) + 1;

				//FileName
				pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
				memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );
				strcat( pcsPackingFile->m_pstrFileName, strFileFullName );

				//파일크기
				pcsPackingFile->m_lFileSize = 0;
				//체크섬
				pcsPackingFile->m_lFileCheckSum = 0;

				//List에 추가한다.
				pcsPatchFolderInfo->m_clFileList.Add( pcsPackingFile );

				//Tree의 Folder를 얻어와서 이놈은 새로추가된 폴더라고 세팅해준다.
				CPackingFolder			*pcsTempPackingFolder;
				pcsTempPackingFolder = m_csAuPackingManager.GetFolder( strDir );

				if( pcsTempPackingFolder )
				{
					pcsTempPackingFolder->m_bExportToRef = true;
				}
			}
		}

		//백업폴더로 파일들을 복사하고 패치를 위해 압축한다.
		if( m_csCheckCopyRawFiles.GetCheck() == BST_CHECKED )
		{
			Copy2BackupAndCompress(pcsPatchVersionInfo );
		}

		//패치버전을 추가한다.
		m_csAuPackingManager.AddPatch( pcsPatchVersionInfo );

		//진행 상황 메시지출력
		AddListString( "------------------- Building JZP File -------------------" );

		//JZP파일을 빌드한다.
		if( m_cCheckBuildJZP.GetCheck() == BST_CHECKED )
		{
			char				strJZPFileName[255];

			sprintf( strJZPFileName, "Patch%04d.JZP", m_csAuPackingManager.m_lCurrentVersion );

			m_csAuPackingManager.BuildJZPFile( m_csAuPackingManager.m_lCurrentVersion, m_strResDir, pcsPatchVersionInfo, this->GetSafeHwnd(), m_cPatchFileProgress.GetSafeHwnd() );
		}

		//진행 상황 메시지출력
		AddListString( "------------------- Building RES File -------------------" );

		char			strDestResFilePath[255];
		sprintf( strDestResFilePath, "%s\\Patch_%04d.Res", m_strPatchResBackupDir, m_lCurrentDisplayVersion );

		BuildResFile( m_csAuPackingManager.GetPatchList(), strDestResFilePath, m_strRawFileDir, m_strResDir );

		AddListString( "------------------- Build Done -------------------" );

		m_bBuildPatch = true;
	}
	else
	{
		MessageBox( "No files to patch." );
		return FALSE;
	}

	return TRUE;

}

void CPatcher2Dlg::OnExitButton() 
{
	// TODO: Add your control notification handler code here
	OnOK();	
}

void CPatcher2Dlg::OnDisplayPrevVersionButton() 
{
	// TODO: Add your control notification handler code here
	m_lCurrentDisplayVersion--;
	
	if( m_lCurrentDisplayVersion < 1 )
	{
		m_lCurrentDisplayVersion = 1;
	}
	else
	{
		DisplayPatchVersion( m_lCurrentDisplayVersion );
		UpdateListBox( m_lCurrentDisplayVersion );
	}
}

void CPatcher2Dlg::OnDisplayNextVersionButton() 
{
	// TODO: Add your control notification handler code here
	m_lCurrentDisplayVersion++;

	if( m_lCurrentDisplayVersion > m_csAuPackingManager.m_lCurrentVersion + 1 )
	{
		m_lCurrentDisplayVersion = m_csAuPackingManager.m_lCurrentVersion + 1;
	}
	else
	{
		DisplayPatchVersion( m_lCurrentDisplayVersion );
		UpdateListBox( m_lCurrentDisplayVersion );
	}	
}

void CPatcher2Dlg::OnBnClickedPatchfileremovebutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int				lSelectedCount;

	lSelectedCount = m_cPatchedFilesList.GetSelCount();

	if( lSelectedCount != LB_ERR )
	{
		int				*paIndex;
		char			strBuffer[255];

		paIndex = new int[lSelectedCount];

		m_cPatchedFilesList.GetSelItems( lSelectedCount, paIndex );

		//Remove List를 날린다.
		m_cRemoveFileList.ResetContent();

		for( int lCounter=0; lCounter<lSelectedCount; lCounter++ )
		{
			m_cPatchedFilesList.GetText( paIndex[lCounter], strBuffer );

			//같은 이름이 없으면 추가한다.
			if( LB_ERR == m_cRemoveFileList.FindStringExact( -1, strBuffer ) )
			{
                m_cRemoveFileList.AddString( strBuffer );
			}
		}

		delete [] paIndex;
	}

	CHAR szTemp[20];

	sprintf_s( szTemp , "%d" , m_cRemoveFileList.GetCount() );
	m_staticDelFile.SetWindowText( szTemp );
}

void CPatcher2Dlg::OnBnClickedPatchfileremoveresetbutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_cPatchedFilesList.SetSel( -1, FALSE );
}

void CPatcher2Dlg::OnBnClickedRemoveresetbutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_cRemoveFileList.ResetContent();
}

void CPatcher2Dlg::OnBnClickedResetbutton()
{

	INT	nCount	=	m_cFileInfoList.GetCount();

	for( INT i = 0 ; i < nCount ; ++i )
	{
		delete m_cFileInfoList.GetItemDataPtr( i );
	}

	m_cFileInfoList.ResetContent();

		CHAR	szTemp[ MAX_PATH ];
	ZeroMemory( szTemp , MAX_PATH );

	sprintf_s( szTemp , "%d" , m_cFileInfoList.GetCount() );
	m_staticPatchFile.SetWindowText( szTemp );


}

void CPatcher2Dlg::OnBnClickedSynexistingfiles()
{
	UpdateData();

	if (m_bSyncExistingFile)
	{
		AfxMessageBox("This option is to scan folders for patches only with already patched files.", MB_OK);
	}
}

void CPatcher2Dlg::OnBnClickedUploadjzp()
{

	m_eOperation		= OPERATION_UPLOAD_JZP;
	Start();	
}

void CPatcher2Dlg::OnBnClickedUploadres()
{
	// Res 파일 업로드 기능을 꺼놓는다 ㅎㅎ
	// VPN 때문에 안된다
	
	/*m_cProgressStatusStatic.SetWindowText( "Uploading Res..." );

	char			strResFile[255];

	sprintf( strResFile, "%s\\Patch_%04d.Res", m_strPatchResBackupDir, m_lCurrentDisplayVersion );

	if (AfxMessageBox("Upload for Test?", MB_YESNO) == IDYES)
		UploadRes(&m_csConfig.m_csRes4Test, strResFile);

	if (AfxMessageBox("Upload to Real Server?", MB_YESNO) == IDYES)
		UploadRes(&m_csConfig.m_csRes4Real, strResFile);

	m_cProgressStatusStatic.SetWindowText( "Res Upload Done..." );*/
}

void CPatcher2Dlg::OnBnClickedBuildpatch()
{
	m_eOperation		=	OPERATION_BUILD_PATCH;
	Start();
}

void CPatcher2Dlg::OnBnClickedPatchdone()
{
	m_eOperation		=	OPERATION_PATCH_DONE;
	Start();
}

void CPatcher2Dlg::OnBnClickedRebuildbutton()
{
	m_eOperation		=	OPERATION_REBUILD;
	Start();
}

void CPatcher2Dlg::OnBnClickedUploadalljzps()
{
	AddListString( "------------------- UpLoading JZP -------------------" );

	UploadJZP(&m_csConfig.m_csJZP, m_strRebuildDir, 0, m_lCurrentDisplayVersion);

	AddListString( "------------------- JZP Upload Done -------------------" );
}

VOID CPatcher2Dlg::AddListString( const CString& str )
{
	m_lbProgress.AddString( str );								// String을 추가하고
	m_lbProgress.SetCaretIndex( m_lbProgress.GetCount()+1 );	// 캐럿을 가장 밑으로 내린다
}

void CPatcher2Dlg::OnBnClickedRebuilddone()
{
	CHAR			strCurrentDirectory	[ MAX_PATH ];
	CHAR			strFullDir			[ MAX_PATH ];
	CHAR			strSourceFile		[ MAX_PATH ];
	CHAR			strTargetFile		[ MAX_PATH ];
	INT				nVersion;

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );

	sprintf_s( strFullDir, MAX_PATH , "%s\\%s\\Rebuild_%04d", strCurrentDirectory, m_strResDir, m_csAuPackingManager.m_lCurrentVersion );

	if( SetCurrentDirectory( strFullDir ) )
	{
	}
	else
	{
		if (!CreateDirectory( strFullDir, NULL ))
		{
			AfxMessageBox("Error Creating Directory !!!", MB_OK);
			return;
		}
	}

	SetCurrentDirectory( strCurrentDirectory );

	for (nVersion = 0; nVersion <= m_csAuPackingManager.m_lCurrentVersion; ++nVersion)
	{
		sprintf_s(strSourceFile, MAX_PATH , "%s\\Patch%04d.JZP", m_strResDir, nVersion);
		sprintf_s(strTargetFile, MAX_PATH , "%s\\Patch%04d.JZP", strFullDir, nVersion);

		MoveFile(strSourceFile, strTargetFile);
	}

	for (nVersion = 0; nVersion <= m_csAuPackingManager.m_lCurrentVersion; ++nVersion)
	{
		sprintf_s(strSourceFile, MAX_PATH , "%s\\Patch%04d.JZP", m_strRebuildDir, nVersion);
		sprintf_s(strTargetFile, MAX_PATH , "%s\\Patch%04d.JZP", m_strResDir, nVersion);

		MoveFile(strSourceFile, strTargetFile);
	}

	sprintf_s( strFullDir, MAX_PATH , "%s\\Patch_%04d_Rebuild.Res", m_strPatchResBackupDir, m_csAuPackingManager.m_lCurrentVersion );
	CopyFile( strFullDir, "Patch.Res", FALSE );
}

void CPatcher2Dlg::UploadJZP(CPatcherFTPInfo *pcsFTPInfo, char *strResDir, int nStartVersion, int nEndVersion)
{
	//JZP파일을 Upload한다.
	int		nVersion;

	for( nVersion = nStartVersion; nVersion <= nEndVersion; ++nVersion )
	{
		CHAR				strJZPFileName[ MAX_PATH ];

		sprintf_s( strJZPFileName, MAX_PATH ,  "Patch%04d.JZP", nVersion );

		vector<string>::iterator	csIter;

		//JZP파일을 FTP로 복사한다.
		for (csIter = pcsFTPInfo->m_aszServers.begin(); csIter != pcsFTPInfo->m_aszServers.end(); ++csIter)
		{
			m_cPatcherFTP.UploadFile(
				(*csIter).c_str()
				, pcsFTPInfo->m_strID.c_str()
				, pcsFTPInfo->m_strPW.c_str()
				, strResDir
				, strJZPFileName
				, pcsFTPInfo->m_nPort
				, pcsFTPInfo->m_bPassiveMode
				);
		}
	}
}

void CPatcher2Dlg::UploadRes(CPatcherFTPInfo *pcsFTPInfo, char *strResSourccePath)
{
	char			strResTargetPath[ MAX_PATH ];
	vector<string>::iterator	csIter;

	sprintf_s( strResTargetPath, MAX_PATH ,  "%s\\Patch.Res", m_strPatchResBackupDir);

	CopyFile(strResSourccePath, strResTargetPath, FALSE);

	for (csIter = pcsFTPInfo->m_aszServers.begin(); csIter != pcsFTPInfo->m_aszServers.end(); ++csIter)
	{
		m_cPatcherFTP.UploadFile( 
			(*csIter).c_str()
			, pcsFTPInfo->m_strID.c_str()
			, pcsFTPInfo->m_strPW.c_str()
			, m_strPatchResBackupDir
			, "Patch.Res"
			, pcsFTPInfo->m_nPort
			, pcsFTPInfo->m_bPassiveMode
			);
	}

	DeleteFile(strResTargetPath);
}

bool CPatcher2Dlg::SaveConfig( VOID )
{
	AuIniManagerA	csINI;
	int				nSection;

	csINI.SetMode(AUINIMANAGER_MODE_NORMAL);
	csINI.SetPath(PATCHER2_CONFIG_FILE);
	if (!csINI.ReadFile())
		return false;

	nSection = csINI.FindSection( PATCHER2_CONFIG_PATCH );
	if( nSection >= 0 )
	{
		if( !SaveConfigPatch( &csINI , nSection , &m_csConfig.m_csLastPatchDate) )
			return false;
	}

	csINI.WriteFile();

	return true;
}

bool CPatcher2Dlg::ReadConfig()
{
	AuIniManagerA	csINI;
	int				nSection;

	csINI.SetMode(AUINIMANAGER_MODE_NAME_OVERWRITE);
	csINI.SetPath(PATCHER2_CONFIG_FILE);
	if (!csINI.ReadFile())
		return false;

	nSection = csINI.FindSection(PATCHER2_CONFIG_JZP);
	if (nSection >= 0)
	{
		if (!ReadConfigFTP(&csINI, nSection, &m_csConfig.m_csJZP))
			return false;
	}

	nSection = csINI.FindSection(PATCHER2_CONFIG_RES4REAL);
	if (nSection >= 0)
	{
		if (!ReadConfigFTP(&csINI, nSection, &m_csConfig.m_csRes4Real))
			return false;
	}

	nSection = csINI.FindSection(PATCHER2_CONFIG_RES4TEST);
	if (nSection >= 0)
	{
		if (!ReadConfigFTP(&csINI, nSection, &m_csConfig.m_csRes4Test))
			return false;
	}

	nSection = csINI.FindSection( PATCHER2_CONFIG_PATCH );
	if( nSection >= 0 )
	{
		if( !ReadConfigPatch( &csINI , nSection , &m_csConfig.m_csLastPatchDate) )
			return false;
	}


	return true;
}

bool CPatcher2Dlg::ReadConfigFTP(AuIniManagerA *pcsINI, int nSection, CPatcherFTPInfo *pcsInfo)
{
	int				nKey;
	char *			szKeyName;

	pcsInfo->m_bEnabled	= true;

	for (nKey = 0; nKey < pcsINI->GetNumKeys(nSection); ++nKey)
	{
		szKeyName	= (char*)pcsINI->GetKeyName(nSection, nKey);
		if (!stricmp(szKeyName, PATCHER2_CONFIG_SERVER))
		{
			pcsInfo->m_aszServers.push_back(pcsINI->GetValue(nSection, nKey));
		}
		else if (!stricmp(szKeyName, PATCHER2_CONFIG_PORT))
		{
			pcsInfo->m_nPort	= atoi(pcsINI->GetValue(nSection, nKey));
		}
		else if (!stricmp(szKeyName, PATCHER2_CONFIG_ID))
		{
			pcsInfo->m_strID	= pcsINI->GetValue(nSection, nKey);
		}
		else if (!stricmp(szKeyName, PATCHER2_CONFIG_PW))
		{
			pcsInfo->m_strPW	= pcsINI->GetValue(nSection, nKey);
		}
		else if (!stricmp(szKeyName, PATCHER2_CONFIG_PASSIVE))
		{
			pcsInfo->m_bPassiveMode	= BOOLEAN( atoi(pcsINI->GetValue(nSection, nKey)) );
		}
	}

	return true;
}

bool	CPatcher2Dlg::ReadConfigPatch( AuIniManagerA *pcsINI, int nSection, INT *pcsInfo )
{
	INT			nKey			=	0;
	char*		szKeyName		=	NULL;

	for (nKey = 0; nKey < pcsINI->GetNumKeys(nSection); ++nKey)
	{
		szKeyName	= (char*)pcsINI->GetKeyName(nSection, nKey);

		if (!stricmp(szKeyName, PATCHER2_CONFIG_PATCH_LASTYEAR))
		{
			m_nLastPatchYear	=	atoi(pcsINI->GetValue(nSection, nKey));
			*pcsInfo			=	m_nLastPatchMonth * 10000;
		}

		else if (!stricmp(szKeyName, PATCHER2_CONFIG_PATCH_LASTMONTH))
		{
			m_nLastPatchMonth	=	atoi(pcsINI->GetValue(nSection, nKey));
			*pcsInfo			+=	m_nLastPatchMonth * 100;
		}

		else if (!stricmp(szKeyName, PATCHER2_CONFIG_PATCH_LASTDAY))
		{
			m_nLastPatchDay	=	atoi(pcsINI->GetValue(nSection, nKey));
			*pcsInfo			+=	m_nLastPatchDay;
		}
	}

	return true;
}

bool	CPatcher2Dlg::SaveConfigPatch( AuIniManagerA *pcsINI, int nSection, INT *pcsInfo )
{
	INT						nKey			=	0;
	char*					szKeyName		=	NULL;
	char*					szSectionName	=	NULL;
	INT						data			=	0;

	SYSTEMTIME	sysTime;
	GetSystemTime( &sysTime );

	
	szSectionName	=	pcsINI->GetSectionName( nSection );

	for (nKey = 0; nKey < pcsINI->GetNumKeys(nSection); ++nKey)
	{
		szKeyName	= (char*)pcsINI->GetKeyName(nSection, nKey);
		if (!stricmp(szKeyName, PATCHER2_CONFIG_PATCH_LASTYEAR))
		{
			data = sysTime.wYear;
			pcsINI->SetValueI( szSectionName , szKeyName , data );
		}

		else if (!stricmp(szKeyName, PATCHER2_CONFIG_PATCH_LASTMONTH))
		{
			data = sysTime.wMonth;
			pcsINI->SetValueI( szSectionName , szKeyName , data );
		}

		else if (!stricmp(szKeyName, PATCHER2_CONFIG_PATCH_LASTDAY))
		{
			data = sysTime.wDay;
			pcsINI->SetValueI( szSectionName , szKeyName , data );
		}
	}


	return true;
}

VOID CPatcher2Dlg::_PatchAll( VOID )
{
	
	m_btnBuildPatch.EnableWindow( FALSE );
	m_btnUploadJZP.EnableWindow( FALSE );
	m_btnPatchDone.EnableWindow( FALSE );

	if( _BuildPatch() )
	{
		AddListString( "------------------- Uploading JZP -------------------" );
		UploadJZP(&m_csConfig.m_csJZP, m_strResDir, m_lCurrentDisplayVersion, m_lCurrentDisplayVersion);
		AddListString( "------------------- JZP Upload Done -------------------" );

		if( m_bBuildPatch )
		{
			//만든 Res 파일을 현재 폴더로 Copy한다.
			char strDestResFilePath[ MAX_PATH ];
			sprintf_s( strDestResFilePath, MAX_PATH , "%s\\Patch_%04d.Res", m_strPatchResBackupDir, m_lCurrentDisplayVersion );
			CopyFile( strDestResFilePath, "Patch.Res", FALSE );

			//다 끝났으니 올라간 버전을 디스플레이해준다.
			++m_lCurrentDisplayVersion;
			UpdateListBox( m_lCurrentDisplayVersion );
			DisplayPatchVersion( m_lCurrentDisplayVersion );

			SaveConfig();

			char szText[ MAX_PATH ];
			sprintf_s( szText, MAX_PATH , "%d : Version Patch를 완료했습니다.", m_lCurrentDisplayVersion-1 );
			AfxMessageBox( szText, MB_OK );
		}
		else
			AfxMessageBox( "Building Patch first!!!", MB_OK );
	}


	m_btnBuildPatch.EnableWindow( TRUE );
	m_btnUploadJZP.EnableWindow( TRUE );
	m_btnPatchDone.EnableWindow( TRUE );
}

VOID CPatcher2Dlg::_UploadJZP( VOID )
{
	AddListString( "------------------- Uploading JZP -------------------" );

	UploadJZP(&m_csConfig.m_csJZP, m_strResDir, m_lCurrentDisplayVersion, m_lCurrentDisplayVersion);

	AddListString( "------------------- JZP Upload Done -------------------" );
}

VOID CPatcher2Dlg::_PatchDone( VOID )
{
	if (!m_bBuildPatch)
	{
		AfxMessageBox("패치 작업을 먼저 하세요", MB_OK);
		return;
	}

	//만든 Res 파일을 현재 폴더로 Copy한다.
	char strDestResFilePath[ MAX_PATH ];
	sprintf_s( strDestResFilePath, MAX_PATH , "%s\\Patch_%04d.Res", m_strPatchResBackupDir, m_lCurrentDisplayVersion );
	CopyFile( strDestResFilePath, "Patch.Res", FALSE );

	m_csAuPackingManager.LoadResFile( "Patch.res" );

	//다 끝났으니 올라간 버전을 디스플레이해준다.
	++m_lCurrentDisplayVersion;
	UpdateListBox( m_lCurrentDisplayVersion );
	DisplayPatchVersion( m_lCurrentDisplayVersion );

	SaveConfig();

}

VOID CPatcher2Dlg::_ReBuild( VOID )
{
	int			lResult;

	lResult = MessageBox( "Rebuild may take very long time. OK?", "확인", MB_YESNO );

	if( lResult == IDYES )
	{
		if( ProcessRebuild( m_csAuPackingManager.GetPatchList() ) )
		{
			MessageBox( "Rebuild Complete!" );
		}
		else
		{
			MessageBox( "Error Rebuilding!" );
		}
	}

}

VOID CPatcher2Dlg::_FullScan( VOID )
{
	// 일단 초기화.
	m_bExecutableAvailable = FALSE;
	FindSubDirFiles( m_strSourceFolder, m_strSourceFolder, &m_cFileInfoList, BOOLEAN( m_bCompareChecksum ) );

	if( m_bExecutableAvailable )
	{
		// 실행화일이 업데이트 돼었다.
		// 서버 버젼 에 데이타를 올림.

		char			strTargetPath[255];
		sprintf( strTargetPath, "%s\\%s\\%d", m_strDebugDir, m_strRegion.c_str() , m_csAuPackingManager.m_lCurrentVersion );
		// svn add strTargetPath
		// svn commit strTargetPath
		// 로그 확인.

		// 폴더 확인하고 코드 삽입.
	}

	CHAR	szTemp[ 20 ];
	ZeroMemory( szTemp , sizeof(szTemp) );

	sprintf_s( szTemp , "%d" , m_cFileInfoList.GetCount() );
	m_staticPatchFile.SetWindowText( szTemp );

	
	

	AfxMessageBox( "Complete" );

}

VOID CPatcher2Dlg::_MakePackage( VOID )
{

	bool	bAddFileList		= false;
	bool	bRemoveFileList		= false;
	bool	bModifyIdentyFile	= false;

	if( m_lCurrentDisplayVersion != (m_csAuPackingManager.GetLastVersion()+1) )
	{
		MessageBox( "Version information is incorrect." );
		return;
	}

	if( m_cFileInfoList.GetCount() > 0 )
	{
		bAddFileList = true;
	}

	if( m_cRemoveFileList.GetCount() )
	{
		//Remove File List에 있는 파일이 AddList에 있다면 패치를 할 수 없다.
		int				lMaxRemoveFileCount;
		char			strBuffer[ MAX_PATH ];

		lMaxRemoveFileCount = m_cRemoveFileList.GetCount();

		for( int lCounter=0; lCounter<lMaxRemoveFileCount; lCounter++ )
		{
			m_cRemoveFileList.GetText( lCounter, strBuffer );

			if( LB_ERR != m_cFileInfoList.FindStringExact( -1, strBuffer ) )
			{
				//만약 같은게 있다면?
				bModifyIdentyFile = true;
				break;
			}
		}

		bRemoveFileList = true;
	}

	if( bModifyIdentyFile )
	{
		MessageBox( "Same file exist at Add/Remove List." );
		return;
	}

	if( bAddFileList || bRemoveFileList )
	{
		CPatchVersionInfo				*pcsPatchVersionInfo;
		CListNode<CPatchFolderInfo *>	*pcsFolderNode;

		CPatchFolderInfo		*pcsPatchFolderInfo;
		CListBoxData			*pcsListBoxData;

		char			strFileNameInListBox[ MAX_PATH ];
		char			strDrive			[ MAX_PATH ];
		char			strDir				[ MAX_PATH ];
		char			strFileName			[ MAX_PATH ];
		char			strFileExt			[ MAX_PATH ];
		char			strFileFullName		[ MAX_PATH ];
		int				lCounter;
		int				lFolderNameLen;

		bool			bFolderFound;

		pcsPatchVersionInfo = new CPatchVersionInfo;

		m_csAuPackingManager.m_lCurrentVersion++;
		m_lCurrentDisplayVersion = m_csAuPackingManager.m_lCurrentVersion;

		//진행 상황 메시지출력
		AddListString( "------------------- Initializing -------------------" );

		//압축하지 않을 정보를 읽어들인다.
		if( bAddFileList )
		{
			m_csAuPackingManager.LoadIgnoreFileInfo( m_strSourceFolder, "IgnoreFileInfo.txt" );
		}

		//버전을 넣어준다.
		pcsPatchVersionInfo->m_lVersion = m_csAuPackingManager.m_lCurrentVersion;

		//추가할 파일에 대해서 연산을 한다.-------------------------------------------------------------------------------
		for( lCounter=0; lCounter< m_cFileInfoList.GetCount(); lCounter++ )
		{
			bFolderFound = false;

			m_cFileInfoList.GetText( lCounter, strFileNameInListBox );
			pcsListBoxData = (CListBoxData *)m_cFileInfoList.GetItemDataPtr( lCounter );

			_splitpath( strFileNameInListBox, strDrive, strDir, strFileName, strFileExt );
			sprintf_s( strFileFullName, MAX_PATH , "%s%s", strFileName, strFileExt );

			//strDir가장 뒤에 붙은 "\"를 없앤다.
			lFolderNameLen = strlen( strDir );
			if( lFolderNameLen > 0 )
			{
				if( strDir[lFolderNameLen-1] == '\\' )
				{
					memset( &strDir[lFolderNameLen-1], 0, 1 );
				}
			}
			else
			{
				sprintf_s( strDir, MAX_PATH , "%s", "root" );
			}

			//폴더네임, 파일이름을 뽑아냈으니 버전에 따른 폴더와 파일을 세팅해준다.
			//1. 우선 폴더가 있는지 본다.
			for( pcsFolderNode = pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsFolderNode; pcsFolderNode = pcsFolderNode->m_pcNextNode )
			{
				if( !strcmpi( pcsFolderNode->m_tData->m_pstrFolderName, strDir ) )
				{
					pcsPatchFolderInfo = pcsFolderNode->m_tData;
					bFolderFound = true;
					break;
				}
			}

			//없으면 추가한다.
			if( bFolderFound == false )
			{
				pcsPatchFolderInfo = new CPatchFolderInfo;

				lFolderNameLen = strlen( strDir ) + 1;

				pcsPatchFolderInfo->m_pstrFolderName = new char[lFolderNameLen];
				memset( pcsPatchFolderInfo->m_pstrFolderName, 0, lFolderNameLen );

				strcat( pcsPatchFolderInfo->m_pstrFolderName, strDir );

				pcsPatchVersionInfo->m_clFolderList.Add( pcsPatchFolderInfo );
			}

			if( pcsPatchFolderInfo )
			{
				CPackingFile		*pcsPackingFile;

				pcsPackingFile = new CPackingFile;

				//패킹할 파일인지 그냥 넘어갈 파일인지 구분한다.
				char			strBuffer[255];

				if( !strcmp( "root", strDir ) )
				{
					sprintf_s( strBuffer, MAX_PATH , "%s\\%s", m_strSourceFolder, strFileFullName );
				}
				else
				{
					sprintf_s( strBuffer, MAX_PATH , "%s\\%s", m_strSourceFolder, strFileNameInListBox );
				}

				//수정 2005.1.19 패킹안하기위해 주석처리.
				if( m_csAuPackingManager.IsIgnoreFileName( strBuffer ) )
				{
					pcsPackingFile->m_bPacking = false;
				}
				else
				{
					pcsPackingFile->m_bPacking = true;
				}
				//				//수정 2005.1.19 패킹안하기위한 처리.
				//				pcsPackingFile->m_bPacking = false;

				//Operation
				pcsPackingFile->m_lOperation = Packing_Operation_Add;

				//FileNameSize
				pcsPackingFile->m_lFileNameSize = strlen( strFileFullName ) + 1;

				//FileName
				pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
				memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );
				strcat( pcsPackingFile->m_pstrFileName, strFileFullName );

				//파일크기
				pcsPackingFile->m_lFileSize = pcsListBoxData->m_lFileSize;
				//체크섬
				pcsPackingFile->m_lFileCheckSum = pcsListBoxData->m_lCheckSum;

				//List에 추가한다.
				pcsPatchFolderInfo->m_clFileList.Add( pcsPackingFile );

				//Tree에도 추가한다.
				m_csAuPackingManager.AddFile( strDir, pcsPackingFile, pcsListBoxData->m_lFileSize, pcsListBoxData->m_lCheckSum );

				//Tree의 Folder를 얻어와서 이놈은 새로추가된 폴더라고 세팅해준다.
				CPackingFolder			*pcsTempPackingFolder;
				pcsTempPackingFolder = m_csAuPackingManager.GetFolder( strDir );

				if( pcsTempPackingFolder )
				{
					pcsTempPackingFolder->m_bExportToRef = true;
				}
			}

			//다 썼으니 지운다.
			delete pcsListBoxData;
		}

		//지울 파일에 대해서 연산을 한다.---------------------------------------------------------------------------------
		for( lCounter=0; lCounter< m_cRemoveFileList.GetCount(); lCounter++ )
		{
			m_cRemoveFileList.GetText( lCounter, strFileNameInListBox );

			_splitpath( strFileNameInListBox, strDrive, strDir, strFileName, strFileExt );
			sprintf_s( strFileFullName, MAX_PATH , "%s%s", strFileName, strFileExt );

			strDir[strlen( strDir )-1] = 0;

			bFolderFound = false;

			//1. 우선 폴더가 있는지 본다.
			for( pcsFolderNode = pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsFolderNode; pcsFolderNode = pcsFolderNode->m_pcNextNode )
			{
				if( !strcmpi( pcsFolderNode->m_tData->m_pstrFolderName, strDir ) )
				{
					pcsPatchFolderInfo = pcsFolderNode->m_tData;
					bFolderFound = true;
					break;
				}
			}

			//없으면 추가한다.
			if( bFolderFound == false )
			{
				pcsPatchFolderInfo = new CPatchFolderInfo;

				lFolderNameLen = strlen( strDir ) + 1;

				pcsPatchFolderInfo->m_pstrFolderName = new char[lFolderNameLen];
				memset( pcsPatchFolderInfo->m_pstrFolderName, 0, lFolderNameLen );

				strcat( pcsPatchFolderInfo->m_pstrFolderName, strDir );

				pcsPatchVersionInfo->m_clFolderList.Add( pcsPatchFolderInfo );
			}

			if( pcsPatchFolderInfo )
			{
				CPackingFile		*pcsPackingFile;

				pcsPackingFile = new CPackingFile;

				if( m_csAuPackingManager.IsIgnoreFileName( strDir ) )
				{
					pcsPackingFile->m_bPacking = false;
				}
				else
				{
					pcsPackingFile->m_bPacking = true;
				}

				//Operation
				pcsPackingFile->m_lOperation = Packing_Operation_Remove;

				//FileNameSize
				pcsPackingFile->m_lFileNameSize = strlen( strFileFullName ) + 1;

				//FileName
				pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
				memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );
				strcat( pcsPackingFile->m_pstrFileName, strFileFullName );

				//파일크기
				pcsPackingFile->m_lFileSize = 0;
				//체크섬
				pcsPackingFile->m_lFileCheckSum = 0;

				//List에 추가한다.
				pcsPatchFolderInfo->m_clFileList.Add( pcsPackingFile );

				//Tree의 Folder를 얻어와서 이놈은 새로추가된 폴더라고 세팅해준다.
				CPackingFolder			*pcsTempPackingFolder;
				pcsTempPackingFolder = m_csAuPackingManager.GetFolder( strDir );

				if( pcsTempPackingFolder )
				{
					pcsTempPackingFolder->m_bExportToRef = true;
				}
			}
		}

		if( MakeCompressedFiles( pcsPatchVersionInfo ) )
		{
			AddListString( "------------------- Make Package Done -------------------" );
		}
		else
		{
			AddListString( "------------------- Make Package Failed -------------------" );
		}
	}
	else
	{
		MessageBox( "No files to Make Package." );
		return;
	}
}

void CPatcher2Dlg::OnBnClickedPatchAll()
{
	m_eOperation		=	OPERATION_PATCH_ALL;
	Start();
}

void CPatcher2Dlg::OnBnClickedMakePackage()
{

	m_eOperation		=	OPERATION_MAKE_PACKAGE_DATA;
	Start();

}

BOOL CPatcher2Dlg::MakeCompressedFiles( CPatchVersionInfo *pcsPatchVersionInfo )
{
	if( !pcsPatchVersionInfo ) return FALSE;

	char strRootPath[ MAX_PATH ] = ".\\ZipDirs";
	char strTargetPath[ MAX_PATH ] = { 0, };

	char strVersionNumber[ 3 ] = { 0, };
	sprintf_s( strVersionNumber, MAX_PATH ,  "%d", pcsPatchVersionInfo->m_lVersion );

	_MakeFolder( strRootPath, strVersionNumber );
	sprintf_s( strTargetPath, MAX_PATH , "%s\\%s", strRootPath, strVersionNumber );

	CListNode< CPatchFolderInfo* >* pSourceFolder = pcsPatchVersionInfo->m_clFolderList.GetStartNode();
	while( pSourceFolder )
	{
		// 일단 대상 경로를 만들고..
		_MakeFolder( strTargetPath, pSourceFolder->m_tData->m_pstrFolderName );

		// 만약에 root 폴더거나 패킹을 사용하지 않는 폴더라면
		if( strcmp( pSourceFolder->m_tData->m_pstrFolderName, "root" ) == 0 || !pSourceFolder->m_tData->m_bPacking )
		{
			// 해당 경로의 파일들을 복사한다.
			_CopyFolderFiles( strTargetPath, pSourceFolder->m_tData );
		}
		// 압축해야 하는 거면..
		else
		{
			// 해당 경로의 파일들을 압축한다.
			_CompressFolderFiles( strTargetPath, pSourceFolder->m_tData );
		}

		// 다음 폴더로..
		pSourceFolder = pSourceFolder->m_pcNextNode;
	}

	return TRUE;
}

void CPatcher2Dlg::_MakeFolder( char* pstrTargetPath, char* pFolderName )
{
	if( !pstrTargetPath || !pFolderName ) return;
	if( strlen( pstrTargetPath ) <= 0 || strlen( pFolderName ) <= 0 ) return;
	if( strcmp( pFolderName, "root" ) == 0 ) return;

	char strCreatePath[ MAX_PATH ];
	sprintf_s( strCreatePath, MAX_PATH , "%s\\%s", pstrTargetPath, pFolderName );

	if( !CreateDirectory( strCreatePath, NULL ) )
	{
		// 중간폴더가 없어서 생성이 안되는 경우도 있으니 생성 실패시 생성해야 할 폴더에서 한단계 위 폴더를 생성해본다.
		char strUpperPath[ MAX_PATH ] = { 0, };
		_CopyPathUntilUpperPath( strUpperPath, strCreatePath );

		if( CreateDirectory( strUpperPath, NULL ) )
		{
			// 상위 폴더 생성에 성공하면 원래 만들려던 폴더를 다시 만들기 시도..
			_MakeFolder( pstrTargetPath, pFolderName );
		}
	}	
}

void CPatcher2Dlg::_CopyPathUntilUpperPath( char* pstrUpperPath, char* pCurrentPath )
{
	if( !pstrUpperPath || !pCurrentPath ) return;

	int nPathNameLength = strlen( pCurrentPath );
	if( nPathNameLength <= 0 ) return;

	strcpy( pstrUpperPath, pCurrentPath );

	for( int nCount = nPathNameLength ; nCount >= 0 ; nCount-- )
	{
		if( pstrUpperPath[ nCount ] == '\\' )
		{
			pstrUpperPath[ nCount ] = '\0';
			return;
		}
	}
}

void CPatcher2Dlg::_CompressFolderFiles( char* pstrTargetPath, CPatchFolderInfo* pFolder )
{
	if( !pFolder ) return;

	CHAR	strBuffer		[ MAX_PATH ] = { 0, };
	CHAR	strDataFileName[ MAX_PATH ] = { 0, };
	CHAR	strRefFileName	[ MAX_PATH ] = { 0, };
	INT		nFileCount = 0;

	FILE*	pfpData = NULL;
	FILE*	pfpReference = NULL;

	CListNode< CPackingFile* >* pSourceFile = pFolder->m_clFileList.GetStartNode();
	if( pSourceFile && pFolder->m_bPacking )
	{
		// 파일이 존재하고 패킹해야 한다면.. Reference.dat 와 Data.dat 파일을 생성한다.
		sprintf_s( strDataFileName, MAX_PATH , "%s\\%s\\%s", pstrTargetPath, pFolder->m_pstrFolderName, Packing_Data_FileName );
		pfpData = fopen( strDataFileName, "wb" );
		if( !pfpData )
		{
			char strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, MAX_PATH , "File Open Error!....FileName : %s", strDataFileName );
			AfxMessageBox( strDebug, MB_OK );
			return;
		}

		sprintf_s( strRefFileName, MAX_PATH , "%s\\%s\\%s", pstrTargetPath, pFolder->m_pstrFolderName, Packing_Reference_FileName );
		pfpReference = fopen( strRefFileName, "wb" );
		if( !pfpReference )
		{
			char strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug , MAX_PATH , "File Open Error!....FileName : %s", strRefFileName );
			AfxMessageBox( strDebug, MB_OK );
			return;
		}
		else
		{
			_ReadyReferenceFile( pfpReference, pFolder );
		}
	}
	
	while( pSourceFile )
	{
		//진행 상황 메시지출력
		sprintf_s( strBuffer, MAX_PATH , "Compress %s\\%s file....", pFolder->m_pstrFolderName, pSourceFile->m_tData->m_pstrFileName );
		AddListString( strBuffer );

		char strCurrentPath			[ MAX_PATH ];
		char strOriginalPathFileName[ MAX_PATH ];
		char strIgnoreTest			[ MAX_PATH ];

		ZeroMemory( strCurrentPath			, MAX_PATH );
		ZeroMemory( strOriginalPathFileName , MAX_PATH );
		ZeroMemory( strIgnoreTest			, MAX_PATH );

		GetCurrentDirectory( sizeof( char ) * MAX_PATH, strCurrentPath );

		if( strcmp( pFolder->m_pstrFolderName, "root" ) == 0 )
		{
			sprintf_s( strOriginalPathFileName, MAX_PATH , "%s\\RawFiles\\%s", strCurrentPath, pSourceFile->m_tData->m_pstrFileName );
			sprintf_s( strIgnoreTest, MAX_PATH , "%s", pSourceFile->m_tData->m_pstrFileName );
		}
		else
		{
			sprintf_s( strOriginalPathFileName, MAX_PATH , "%s\\RawFiles\\%s\\%s", strCurrentPath, pFolder->m_pstrFolderName, pSourceFile->m_tData->m_pstrFileName );
			sprintf_s( strIgnoreTest, MAX_PATH , "%s\\%s", pFolder->m_pstrFolderName, pSourceFile->m_tData->m_pstrFileName );
		}

		if( m_csAuPackingManager.IsIgnoreFileName( strOriginalPathFileName ) )
		{
			// 압축제외대상인 파일이라면 걍 복사
			_CopyFile( strOriginalPathFileName, pstrTargetPath, pFolder->m_pstrFolderName, pSourceFile->m_tData->m_pstrFileName );
		}
		else
		{
			// 압축된 각 파일들을 Data.dat 와 Reference.dat 파일에 뭉친다.
			nFileCount++;
			_PackingFile( pfpData, pfpReference, nFileCount, pSourceFile->m_tData, strOriginalPathFileName );
		}

		pSourceFile = pSourceFile->m_pcNextNode;
	}

	// 다 끝났으면 파일닫기..
	if( pfpData )
	{
		fclose( pfpData );
	}

	if( pfpReference )
	{
		fclose( pfpReference );
	}

	// 다 했는데 파일카운트가 0 이면 쓸데없이 파일만 만든셈.. 지워주자
	if( nFileCount <= 0 )
	{
		::DeleteFile( strDataFileName );
		::DeleteFile( strRefFileName );
	}
	else
	{
		// 다했으면 Reference.dat 파일을 암호화한다.
		_EncryptReferenceFile( strRefFileName );
	}
}

void CPatcher2Dlg::_CopyFolderFiles( char* pstrTargetPath, CPatchFolderInfo* pFolder )
{
	if( !pFolder ) return;

	char strBuffer[ MAX_PATH ];

	CListNode< CPackingFile* >* pSourceFile = pFolder->m_clFileList.GetStartNode();
	while( pSourceFile )
	{
		//진행 상황 메시지출력
		sprintf_s( strBuffer, MAX_PATH , "Copy %s\\%s file....", pFolder->m_pstrFolderName, pSourceFile->m_tData->m_pstrFileName );
		AddListString( strBuffer );

		OutputDebugString( strBuffer );
		OutputDebugString( "\n" );

		char strOriginalPathFileName[ MAX_PATH ];
		if( strcmp( pFolder->m_pstrFolderName, "root" ) == 0 )
		{
			sprintf_s( strOriginalPathFileName, MAX_PATH , "RawFiles\\%s", pSourceFile->m_tData->m_pstrFileName );
		}
		else
		{
			sprintf_s( strOriginalPathFileName, MAX_PATH , "RawFiles\\%s\\%s", pFolder->m_pstrFolderName, pSourceFile->m_tData->m_pstrFileName );
		}

		char strTargetPathFileName[ MAX_PATH ];
		if( strcmp( pFolder->m_pstrFolderName, "root" ) == 0 )
		{
			sprintf_s( strTargetPathFileName, MAX_PATH , "%s\\%s", pstrTargetPath, pSourceFile->m_tData->m_pstrFileName );
		}
		else
		{
			sprintf_s( strTargetPathFileName, MAX_PATH , "%s\\%s\\%s", pstrTargetPath, pFolder->m_pstrFolderName, pSourceFile->m_tData->m_pstrFileName );
		}

		CopyFile( strOriginalPathFileName, strTargetPathFileName, FALSE );
		pSourceFile = pSourceFile->m_pcNextNode;
	}
}

void CPatcher2Dlg::_ReadyReferenceFile( FILE* pRefFile, CPatchFolderInfo* pFolder )
{
	if( !pRefFile || !pFolder ) return;

	// 파일의 맨 앞으로 이동해서..
	fseek( pRefFile, 0, SEEK_SET );

	// 우선 이 폴더에 파일이 0개 있다고 써주고..
	int nFileCount = 0;
	fwrite( &nFileCount, sizeof( int ), 1, pRefFile );

	// 이 폴더의 폴더명길이, 폴더명을 써준다.
	int nFolderNameLength = strlen( pFolder->m_pstrFolderName ) + 1;
	fwrite( &nFolderNameLength, sizeof( int ), 1, pRefFile );
	fwrite( pFolder->m_pstrFolderName, sizeof( char ), nFolderNameLength, pRefFile );

	char strDebug[ MAX_PATH ] = { 0, };
	sprintf_s( strDebug, MAX_PATH , "Build Package Folder = %s\n", pFolder->m_pstrFolderName );
	OutputDebugString( strDebug );
}

void CPatcher2Dlg::_PackingFile( FILE* pDatFile, FILE* pRefFile, const int nFileCount, CPackingFile* pPackFile, char* pOriginalFile )
{
	if( !pDatFile || !pRefFile || !pPackFile || !pOriginalFile ) return;

	char strDebug[ MAX_PATH ];
	sprintf_s( strDebug, MAX_PATH , "Write %s to Data.dat....", pOriginalFile );

	// ===== 1. Data.dat 파일에 현재 파일내용을 추가 =====
	// 우선 파일의 끝으로 가서 기록이 시작될 위치를 구하고..
	fseek( pDatFile, 0, SEEK_END );
	int nCurrPos = ( int )ftell( pDatFile );

	// 기록할 원본파일을 열어서..
	FILE* pSourceFile = fopen( pOriginalFile, "rb" );
	if( !pSourceFile )
	{
		char strDebug[ 256 ] = { 0, };
		sprintf( strDebug, "File Open Error!....FileName : %s", pOriginalFile );
		AfxMessageBox( strDebug, MB_OK );
		return;
	}

	// 파일의 끝에 도달할때까지 파일내용을 Data.dat 에 복사.. 인데..
	// 4KB 짜리 버퍼를 만들어서 버퍼사이즈 단위로 읽다가 읽은량이 버퍼보다 작으면 종료하게 한다.
	// 이게 디스크접근을 줄여서 하드랙을 좀 줄여주는건가.. ??
	char pBuffer[ 4096 ];
	memset( pBuffer, 0, sizeof( char ) * 4096 );

	int nReadSize = 4096;
	while( nReadSize >= 4096 )
	{
		nReadSize = fread( pBuffer, sizeof( char ), 4096, pSourceFile );
		fwrite( pBuffer, sizeof( char ), nReadSize, pDatFile );
	}

	fclose( pSourceFile );

	strcat( strDebug, "Success!\n" );
	OutputDebugString( strDebug );

	memset( strDebug, 0, sizeof( char ) * 256 );
	sprintf( strDebug, "Write Fileinfo to Reference.dat...." );

	// ===== 2. Reference.dat 파일에 참조정보 기록하기 =====
	// 파일의 맨 앞으로 가서 파일 갯수를 기록한다.
	fseek( pRefFile, 0, SEEK_SET );
	fwrite( &nFileCount, sizeof( int ), 1, pRefFile );

	// 파일의 맨 끝으로 가서 파일명길이, 파일명, 저장시작위치, 파일크기를 기록한다.
	// 파일명 길이를 기존에 것을 쓰지 않고 직접 잰다.. 폴더안에 폴더 같은경우 문제가 있으니..
	// 경로명을 떼고 파일명만 넣어준다.
	char strFileName[ 256 ] = { 0, };
	char strExtName[ 16 ] = { 0, };

	_splitpath( pPackFile->m_pstrFileName, NULL, NULL, strFileName, strExtName ); 
	strcat( strFileName, strExtName );

	int nNameLength = ( int )strlen( strFileName ) + 1;

	fseek( pRefFile, 0, SEEK_END );
	fwrite( &nNameLength, sizeof( int ), 1, pRefFile );
	fwrite( &strFileName, sizeof( char ), nNameLength, pRefFile );
	fwrite( &nCurrPos, sizeof( int ), 1, pRefFile );
	fwrite( &pPackFile->m_lFileSize, sizeof( int ), 1, pRefFile );

	strcat( strDebug, "Success!\n" );
	OutputDebugString( strDebug );
}

void CPatcher2Dlg::_CopyFile( char* pstrOriginalFile, char* pstrOutputDir, char* pTargetPath, char* pTargetName )
{
	char strCurrPath[ 256 ] = { 0, };
	GetCurrentDirectory( sizeof( char ) * 256, strCurrPath );

	char strFullTargetPathFileName[ 256 ] = { 0, };
	sprintf( strFullTargetPathFileName, "%s\\%s\\%s\\%s", strCurrPath, pstrOutputDir, pTargetPath, pTargetName );

	::CopyFile( pstrOriginalFile, strFullTargetPathFileName, FALSE );
}

void CPatcher2Dlg::_EncryptReferenceFile( char* pRefFileName )
{
	if( !pRefFileName ) return;
	if( strlen( pRefFileName ) <= 0 ) return;

	FILE* pfpRefFile = fopen( pRefFileName, "rb" );
	if( !pfpRefFile )
	{
		char strDebug[ 256 ] = { 0, };
		sprintf( strDebug, "File Open Error!....FileName : %s", pRefFileName );
		AfxMessageBox( strDebug, MB_OK );
		return;
	}

	// 파일 사이즈를 얻어와서 그만큼의 버퍼를 잡는다.
	fseek( pfpRefFile, 0, SEEK_END );
	int nFileSize = ( int )ftell( pfpRefFile );

	char* pBuffer = ( char* )new char[ nFileSize ];
	memset( pBuffer, 0, sizeof( char ) * nFileSize );

	fseek( pfpRefFile, 0, SEEK_SET );
	fread( pBuffer, sizeof( char ), nFileSize, pfpRefFile );
	fclose( pfpRefFile );

	// 암호화 시작!
	if( !m_csAuPackingManager.m_cMD5Encrypt.EncryptString( "1111", pBuffer, sizeof( char ) * nFileSize ) )
	{
		char strDebug[ 256 ] = { 0, };
		sprintf( strDebug, "Data Encrypt Error!....FileName : %s", pRefFileName );
		AfxMessageBox( strDebug, MB_OK );

		delete[] pBuffer;
		pBuffer = NULL;
		return;
	}

	// 암호화 성공했으면 기존 파일 지우고 새로 만들기
	::DeleteFile( pRefFileName );

	pfpRefFile = fopen( pRefFileName, "wb" );
	if( !pfpRefFile )
	{
		char strDebug[ 256 ] = { 0, };
		sprintf( strDebug, "File Open Error!....FileName : %s", pRefFileName );
		AfxMessageBox( strDebug, MB_OK );
		return;
	}

	fwrite( pBuffer, sizeof( char ), nFileSize, pfpRefFile );
	fclose( pfpRefFile );

	delete[] pBuffer;
	pBuffer = NULL;
}

void CPatcher2Dlg::OnBnClickedCompareChecksum()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bCompareChecksum = !m_bCompareChecksum;
}

void CPatcher2Dlg::OnBnClickedBuildjzpcheckbutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool bIs = m_cCheckBuildJZP.GetCheck() == BST_CHECKED;
	m_csAuPackingManager.SetBuildJZP( bIs );
}

void CPatcher2Dlg::OnBnClickedBuildpackingcheckbutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool bIs = m_cCheckBuildPackingData.GetCheck() == BST_CHECKED;
	m_csAuPackingManager.SetBuildDat( bIs );
}

void CPatcher2Dlg::OnBnClickedCopyrawfilesButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool bIs = m_csCheckCopyRawFiles.GetCheck() == BST_CHECKED;
	m_csAuPackingManager.SetCopyRawFiles( bIs );
}

void CPatcher2Dlg::Run( VOID )
{
	
	switch( m_eOperation )
	{
	case OPERATION_PATCH_ALL:			
		_PatchAll();
		break;

	case OPERATION_BUILD_PATCH:			
		_BuildPatch();
		break;

	case OPERATION_MAKE_PACKAGE_DATA:
		_MakePackage();
		break;

	case OPERATION_PATCH_DONE:
		_PatchDone();
		break;

	case OPERATION_UPLOAD_JZP:
		_UploadJZP();
		break;

	case OPERATION_REBUILD:
		_ReBuild();
		break;

	case OPERATION_FULLSCAN:
		_FullScan();
		break;
	}	

	m_eOperation		=	OPERATION_COUNT;
}

// List Box의 내용을 모두 지운다
void CPatcher2Dlg::OnBnClickedClearButton()
{
	m_lbProgress.ResetContent();
}

LRESULT CPatcher2Dlg::OnWriteLogFile( WPARAM wParam , LPARAM lParam )
{
	AddListString( (const char*)wParam );
	m_cPatchFileProgress.SetPos( (INT)lParam );

	return 0L;
}

LRESULT CPatcher2Dlg::OnInitProgress( WPARAM wParam , LPARAM lParam )
{
	m_cPatchFileProgress.SendMessage( PBM_SETRANGE	, 0 , lParam	);
	m_cPatchFileProgress.SendMessage( PBM_SETPOS	, 0 , 0			);

	return 0L;
}

void CPatcher2Dlg::OnBnClickedSettingButton()
{
	CServerSettingDlg		dlg;

	dlg.DoModal();
}
void CPatcher2Dlg::OnCbnDropdownListCombo()
{
	m_ServerMap.clear();
	m_comboList.ResetContent();

	fstream		FileStream;
	CHAR		NameTemp[ 256 ];
	CHAR		IPTemp[ 256 ];

	FileStream.open( SERVER_FILE_NAME , ios_base::in );
	if( !FileStream )	return;

	for( INT i = 0 ;  ; ++i )
	{
		// Server Name
		FileStream.getline( NameTemp , 256 );
		if( NameTemp[0] == 0 )	break;

		//m_ListCtrl.InsertItem( i, szTemp );
		m_comboList.AddString( NameTemp );

		// IP Address
		FileStream.getline( IPTemp , 256 );
		//m_ListCtrl.SetItemText( i, 1, szTemp );

		// map에 server 정보를 추가한다
		m_ServerMap.insert( make_pair(NameTemp,IPTemp) );

		// 공백
		FileStream.getline( NameTemp , 256 );

		ZeroMemory( NameTemp , 256 );
	}

	if( FileStream )	FileStream.close();
}

void CPatcher2Dlg::OnBnClickedConnectButton()
{
	CString strData;
	INT CurrentSel	=	m_comboList.GetCurSel();

	// 선택되었다면 Server 이름을 얻어온다
	if( CurrentSel	==	-1 )	return;
	m_comboList.GetLBText( CurrentSel , strData );

	CHAR ServerIP[ 256 ];
	sprintf_s( ServerIP , 256 , "/v:%s" , m_ServerMap[ strData ] );

	::ShellExecute( NULL , "open" , "mstsc.exe" , ServerIP , NULL , SW_SHOW );
}
void CPatcher2Dlg::OnBnClickedFastRadio()
{
	m_PatchType		=	PATCH_FAST;
}

void CPatcher2Dlg::OnBnClickedNormalRadio()
{
	m_PatchType		=	PATCH_NORMAL;
}

void CPatcher2Dlg::OnBnClickedPatchfileremovebutton2()
{
	INT				lSelectedCount;

	lSelectedCount = m_cRemoveFileList.GetCurSel();

	if( lSelectedCount != LB_ERR )
	{
		m_cRemoveFileList.DeleteString( lSelectedCount );
	}

	CHAR szTemp[20];

	sprintf_s( szTemp , "%d" , m_cRemoveFileList.GetCount() );
	m_staticDelFile.SetWindowText( szTemp );
}

void CPatcher2Dlg::OnBnClickedRealtimeScan()
{
	m_DirectoryWatcher.WatchDirectory( m_RealTimeFolder , FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE , &m_DirectoryHandler , TRUE );
	
	m_btnSensorOn.EnableWindow( FALSE );
	m_btnSensorOff.EnableWindow( TRUE );

	ZeroMemory( m_strSourceFolder , 260 );
	strcpy_s( m_strSourceFolder , 260 , m_RealTimeFolder );

	AddListString( "[[[ RealTime Folder Sensor On ]]]" );
}

void CPatcher2Dlg::OnBnClickedRealtimeStop()
{
	m_DirectoryWatcher.UnwatchAllDirectories();

	m_btnSensorOn.EnableWindow( TRUE );
	m_btnSensorOff.EnableWindow( FALSE );

	AddListString( "[[[ RealTime Folder Sensor Off ]]]" );
}

void CPatcher2Dlg::OnBnClickedScanfolder()
{
	CFolderPointingDlg		dlg;

	dlg.m_strRealTimeFolder			=	m_RealTimeFolder;

	if(	dlg.DoModal()	==	IDOK	)
	{
		m_RealTimeFolder		=	dlg.m_strRealTimeFolder;
	}

}

BOOL CPatcher2Dlg::CheckPatchFile( const char* szFileName )
{
	return TRUE;
}

BOOL CPatcher2Dlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message	==	WM_KEYDOWN )
	{
		switch( pMsg->wParam )
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

DWORD	CPatcher2Dlg::GetFileSize( const char* szFileName )
{

	// 파일의 크기를 구해서 반환한다
	FILE*		pFile		=	fopen( szFileName , "rb" );
	DWORD		dwFileSize	=	0;

	if( pFile )
	{
		fseek( pFile , 0 , SEEK_END );
		dwFileSize	=	ftell( pFile );
		fclose( pFile );
	}

	return dwFileSize;

}

DWORD	CPatcher2Dlg::GetCheckSum( const char* szFileName )
{
	DWORD	dwHeaderSum		=	0;
	DWORD	dwCheckSum		=	0;

	MapFileAndCheckSum( (char*)szFileName , &dwHeaderSum , &dwCheckSum );

	return dwCheckSum;
}

BOOL	CPatcher2Dlg::CheckAddFile( const char* szFileName , DWORD dwCheckSum )
{
	if( !szFileName || !dwCheckSum )
		return FALSE;
	
	return TRUE;
}

VOID	CPatcher2Dlg::DragFileAdd( const char* szFolderName )
{
	//맨 윗닷은 무조건 root로 저장된다.
	m_csAuPackingManager.AddFolder( "root" );

	m_bAddList = true;

	FindSubDirFiles( szFolderName , szFolderName , &m_cFileInfoList , m_bCompareChecksum );

	CHAR	szTemp[ MAX_PATH ];
	ZeroMemory( szTemp , MAX_PATH );

	sprintf_s( szTemp , "%d" , m_cFileInfoList.GetCount() );
	m_staticPatchFile.SetWindowText( szTemp );
}

VOID	CPatcher2Dlg::UpdateFileList( VOID )
{
	FileUpdateListIter	Iter	=	m_FileUpdateList.begin();
	for( ; Iter != m_FileUpdateList.end() ; ++Iter )
	{
		RealTimeSensorFileAdd( (*Iter).c_str() );
	}

	m_FileUpdateList.clear();
}


VOID	CPatcher2Dlg::RealTimeSensorFileAdd( const char* szFileName )
{	
	m_csAuPackingManager.AddFolder( "root" );

	m_bAddList	=	true;

	AutoOneFileAdd( szFileName , m_RealTimeFolder , &m_cFileInfoList , m_bCompareChecksum );

	CHAR	szTemp[ MAX_PATH ];
	ZeroMemory( szTemp , MAX_PATH );

	sprintf_s( szTemp , "%d" , m_cFileInfoList.GetCount() );
	m_staticPatchFile.SetWindowText( szTemp );

	sprintf_s( szTemp , "%d" , m_nSkilFileCount );
	m_staticSkipFile.SetWindowText( szTemp );
}
void CPatcher2Dlg::OnBnClickedDeletebutton()
{
	CString str;

	if( m_cFileInfoList.GetCurSel() == LB_ERR )
		return;

	m_cFileInfoList.GetText( m_cFileInfoList.GetCurSel() , str );

	if( AfxMessageBox( str , MB_OKCANCEL ) == IDOK )
	{
		m_cFileInfoList.DeleteString( m_cFileInfoList.GetCurSel() );
	}

	CHAR	szTemp[ MAX_PATH ];
	ZeroMemory( szTemp , MAX_PATH );

	sprintf_s( szTemp , "%d" , m_cFileInfoList.GetCount() );
	m_staticPatchFile.SetWindowText( szTemp );
}


VOID CPatcher2Dlg::_WriteLog( const char* szFileName , const char* pData , ... )
{
	CHAR		szTemp[ 300 ];
	ZeroMemory( szTemp , 300 );

	va_list		vlist;
	va_start( vlist , pData );
	vsprintf( szTemp , pData , vlist );
	va_end( vlist );

	AuLogFile_s( (char*)szFileName , szTemp );
}
void CPatcher2Dlg::OnNMCustomdrawPatchfileprogress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


BOOL	CPatcher2Dlg::CheckLimitTime( FILETIME& rFileTime )
{

	// 마지막 패치를 기준으로 체크
	if( m_nFullScanType	==	0	)
	{
		CTime	FileTime( rFileTime );

		INT		nFileDate	=	FileTime.GetYear()*10000	+ FileTime.GetMonth()*100	+ FileTime.GetDay();
		INT		nLastDate	=	m_nLastPatchYear*10000 + m_nLastPatchMonth*100 + m_nLastPatchDay;

		if( nLastDate <= nFileDate )
		{
			return FALSE;
		}

	}

	// 전체 체크
	else if( m_nFullScanType == 1 )
	{
		return FALSE;
	}

	// 지정된 시간에서 체크
	else if( m_nFullScanType == 2 )
	{
		CTime	FileTime( rFileTime );

		INT		nFileDate	=	FileTime.GetYear()*10000	+ FileTime.GetMonth()*100	+ FileTime.GetDay();
		INT		nMaxDate	=	m_nMaxYear*10000			+ m_nMaxMonth*100			+ m_nMaxDay;
		INT		nMinDate	=	m_nMinYear*10000			+ m_nMinMonth*100			+ m_nMinDay;

		if( nMinDate <= nFileDate && nMaxDate >= nFileDate )
		{
			return FALSE;
		}

	}

	return TRUE;
}