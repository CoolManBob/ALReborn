// PatchDataViwerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "PatchDataViwer.h"
#include "PatchDataViwerDlg.h"
#include ".\patchdataviwerdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPatchDataViwerDlg 대화 상자



CPatchDataViwerDlg::CPatchDataViwerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchDataViwerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPatchDataViwerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Check_PackingExtractor_SubDir, m_cPackingExtractSubDir);
	DDX_Control(pDX, IDC_Check_Compare_SubDir, m_cCompareSubDir);
	DDX_Control(pDX, IDC_Static_JZPExtractor_SourcePath, m_cJZPExtractorScrPath);
	DDX_Control(pDX, IDC_Edit_JZPExtractor_DestPath, m_cJZPExtractorDestPath);
	DDX_Control(pDX, IDC_Static_PackingExtractor_ScrPath, m_cDatExtractorScrPath);
	DDX_Control(pDX, IDC_Edit_PackingExtractor_DestPath, m_cDatExtractorDestPath);
	DDX_Control(pDX, IDC_Static_Compare_SrcPath, m_cCompareScrPath);
	DDX_Control(pDX, IDC_Static_Compare_DestPath, m_cCompareDestPath);
}

BEGIN_MESSAGE_MAP(CPatchDataViwerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_Button_JZPExtractor_Scan_SrcFile, OnBnClickedButtonJzpextractorScanSrcfile)
	ON_BN_CLICKED(IDC_Button_JZPExtractor_Scan_DestPath, OnBnClickedButtonJzpextractorScanDestpath)
	ON_BN_CLICKED(IDC_Button_JZPExtractor, OnBnClickedButtonJzpextractor)
	ON_BN_CLICKED(IDC_Button_PackingExtractor_Scan_SrcPath, OnBnClickedButtonPackingextractorScanSrcpath)
	ON_BN_CLICKED(IDC_Button_PackingExtractor_Scan_DestPath, OnBnClickedButtonPackingextractorScanDestpath)
	ON_BN_CLICKED(IDC_Button_PackingExtractor, OnBnClickedButtonPackingextractor)
	ON_BN_CLICKED(IDC_Button_Compare_Scan_SrcPath, OnBnClickedButtonCompareScanSrcpath)
	ON_BN_CLICKED(IDC_Button_Compare_Scan_DestPath, OnBnClickedButtonCompareScanDestpath)
	ON_BN_CLICKED(IDC_Button_Compare, OnBnClickedButtonCompare)
	ON_BN_CLICKED(IDC_Button_Exit, OnBnClickedButtonExit)
END_MESSAGE_MAP()


// CPatchDataViwerDlg 메시지 처리기

BOOL CPatchDataViwerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	memset( m_strJZPSrcPath, 0, sizeof(m_strJZPSrcPath) );
	memset( m_strJZPDestPath, 0, sizeof(m_strJZPDestPath) );

	memset( m_strPackingSrcPath, 0, sizeof(m_strPackingSrcPath) );
	memset( m_strPackingDestPath, 0, sizeof(m_strPackingDestPath) );

	memset( m_strCompareSrcPath, 0, sizeof(m_strCompareSrcPath) );
	memset( m_strCompareDestPath, 0, sizeof(m_strCompareDestPath) );
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CPatchDataViwerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CPatchDataViwerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CPatchDataViwerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CPatchDataViwerDlg::ScanFolder( char *pstrFolder )
{
	// TODO: Add your control notification handler code here
	bool			bResult;

	bResult = false;

	if( pstrFolder )
	{
		LPITEMIDLIST pidlRoot = NULL;
		LPITEMIDLIST pidlSelected = NULL;
		BROWSEINFO bi = {0};
		LPMALLOC pMalloc = NULL;
		char		pszDisplayName[256];

		SHGetMalloc(&pMalloc);

		bi.hwndOwner = this->m_hWnd;
		bi.pidlRoot = pidlRoot;
		bi.pszDisplayName = pszDisplayName;
		bi.lpszTitle = "Choose a Folder";
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
			SHGetPathFromIDList(pidlSelected, pstrFolder );
			bResult = true;
		}
	}

    return bResult;
}

bool CPatchDataViwerDlg::DepressJZP( char *pstrJZPFileName, char *pstrDestPath )
{
	FILE			*file;
	bool			bResult;
	bool			bPacking;
	int				lVersion;
	int				lMaxFolderCount;
	int				lFolderCount;
	int				lMaxFileCount;
	int				lFileCount;
	int				lTotalFileCount;

	int				lFolderNameLen;
	char			*pstrFolderName;
	char			strCurrentDirectory[255];

	bResult = false;

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );

	SetCurrentDirectory( pstrDestPath );

	if( pstrJZPFileName )
	{
		file = fopen( pstrJZPFileName, "rb" );

		if( file )
		{
			//버전
			fread( &lVersion, 1, sizeof(int), file );

			//전체 파일갯수를 읽어낸다.
			fread( &lTotalFileCount, 1, sizeof(int), file );

			//폴더 갯수
			fread( &lMaxFolderCount, 1, sizeof(int), file );

			for( lFolderCount=0; lFolderCount<lMaxFolderCount; lFolderCount++ )
			{
				//폴더이름 길이로드
				fread( &lFolderNameLen, 1, sizeof(int), file );

				//폴더이름 로드
				pstrFolderName = new char[lFolderNameLen];
				memset( pstrFolderName, 0, lFolderNameLen );
				fread( pstrFolderName, 1, lFolderNameLen, file );

				//편집중이라고 세팅한다.
				m_csAuPackingManager.MarkPacking( pstrFolderName );

				//파일갯수기록
				fread( &lMaxFileCount, 1, sizeof(int), file );

				bool bPackingFolder;
				//이 폴더에 패킹을 하는 파일이 존재하는가?
				fread( &bPackingFolder, 1, sizeof(bool), file );

				for( lFileCount=0; lFileCount<lMaxFileCount ;lFileCount++ )
				{
					int				lOperation;
					int				lFileNameLength;
					int				lFileSize;
					char			*pstrFileName;
					char			*pstrBuffer;

					pstrFileName = NULL;
					pstrBuffer = NULL;

					fread( &bPacking, 1, sizeof(bool), file );

					//Operation
					fread( &lOperation, 1, sizeof(int), file );

					//FileNameLength
					fread( &lFileNameLength, 1, sizeof(int), file );

					//FileName
					pstrFileName = new char[lFileNameLength];
					memset( pstrFileName, 0, lFileNameLength );
					fread(pstrFileName, 1, lFileNameLength, file );

					FILE *fp	= fopen("JZPFiles.txt", "at");
					fprintf(fp, pstrFileName);
					fclose(fp);

					if( lOperation == Packing_Operation_Add )
					{
						//FileSize
						fread( &lFileSize, 1, sizeof(int), file );

						pstrBuffer = new char[lFileSize];

						//FileData
						fread( pstrBuffer, 1, lFileSize, file );

						//Packing이면 Data.Dat에 추가한다.
						if( bPacking == true )
						{
							if( !strcmpi( "root", pstrFolderName ) )
							{
								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrDestPath, pstrFileName );
							}
							else
							{
								char			strDestDir[255];

								sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );
								m_csAuPackingManager.CreateFolder( pstrFolderName );

								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrDestPath, strDestDir );
							}
						}
						//Packing이 아니면 해당 폴더에 그냥 파일을 생성한다.
						else
						{
							if( !strcmpi( "root", pstrFolderName ) )
							{
								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrDestPath, pstrFileName );
							}
							else
							{
								char			strDestDir[255];

								sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );
								m_csAuPackingManager.CreateFolder( pstrFolderName );

								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrDestPath, strDestDir );
							}
						}
					}
					//지워지는 파일.
					else
					{
					}

					if( pstrFileName != NULL )
						delete [] pstrFileName;
					if( pstrBuffer != NULL )
						delete [] pstrBuffer;
				}
			}
		}

		bResult = true;
		fclose( file );
	}

	SetCurrentDirectory( strCurrentDirectory );

	return bResult;
}

bool CPatchDataViwerDlg::DepressDat( char *pstrSrcPath, char *pstrDestPath )
{
	ApAdmin			*pcsAdminFolderInfo;
	bool			bResult;
	char			strCurrentDirectory[255];

	bResult = false;

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );

	SetCurrentDirectory( pstrSrcPath );	

	m_csAuPackingManager.LoadReferenceFile( pstrSrcPath, true, true ); //Read only, Load SubDir
	//m_csAuPackingManager.setLoadFromPackingData(true);
	//m_csAuPackingManager.m_bLoadFromRawFile = false;

	m_csAuPackingManager.SetFilePointer( m_cPackingExtractSubDir.GetCheck() );

	pcsAdminFolderInfo = m_csAuPackingManager.GetFolderInfo();

	if( pcsAdminFolderInfo )
	{
		CPackingFolder	**ppcsPackingFolder;
		int				lFolderIndex;

		lFolderIndex = 0;

		for( ppcsPackingFolder = (CPackingFolder **)pcsAdminFolderInfo->GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder **)pcsAdminFolderInfo->GetObjectSequence( &lFolderIndex ) )
		{
			if( ppcsPackingFolder && (*ppcsPackingFolder) )
			{
				CPackingFile	**ppcsPackingFile;
				int				lFileIndex;

				lFileIndex = 0;

				for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex) )
				{
                    if( ppcsPackingFile && (*ppcsPackingFile) )
					{
						ApdFile			csApdFile;

						if( m_csAuPackingManager.OpenFile( (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName, &csApdFile ) )
						{
							int				lFileSize;

							lFileSize = m_csAuPackingManager.GetFileSize( &csApdFile );

							if( lFileSize )
							{
								FILE			*file;
								char			*pstrBuffer;
								char			strDestPath[255];

								pstrBuffer = new char[lFileSize];

                                m_csAuPackingManager.ReadFile( pstrBuffer, lFileSize, &csApdFile );

								//쓰기전에는 목적지 디렉토리로세팅한다.
								SetCurrentDirectory( pstrDestPath );	

								//읽어들인 블럭을 하드디스크에 저장한다.
								if( !stricmp( "root", (*ppcsPackingFolder)->m_pstrFolderName ) )
								{
									sprintf( strDestPath, "%s", (*ppcsPackingFile)->m_pstrFileName );
								}
								else
								{
									m_csAuPackingManager.CreateFolder( (*ppcsPackingFolder)->m_pstrFolderName );

									sprintf( strDestPath, "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName );
								}

								file = fopen( strDestPath, "wb" );

								if( file )
								{
                                    fwrite( pstrBuffer, 1, lFileSize, file );

									fclose( file );
								}

								delete [] pstrBuffer;

								//다 썼으니 소스 디렉토리로 돌린다.
								SetCurrentDirectory( pstrSrcPath );	
							}

							m_csAuPackingManager.CloseFile( &csApdFile );
						}
					}
				}
			}
		}

		bResult = true;
	}

	SetCurrentDirectory( strCurrentDirectory );

	return bResult;
}

void CPatchDataViwerDlg::OnBnClickedButtonJzpextractorScanSrcfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog				cFileDialog( TRUE, "*.JZP", "*.JZP" );
	CString					cFilePath;

	if( cFileDialog.DoModal() == IDOK )
	{
		cFilePath = cFileDialog.GetPathName();

		sprintf( m_strJZPSrcPath, "%s", cFilePath.GetBuffer() );

		m_cJZPExtractorScrPath.SetWindowText( m_strJZPSrcPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonJzpextractorScanDestpath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ScanFolder( m_strJZPDestPath ) )
	{
		m_cJZPExtractorDestPath.SetWindowText( m_strJZPDestPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonJzpextractor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_cJZPExtractorDestPath.GetWindowTextLength() )
	{
		char			strBuffer[255];
		
		memset( strBuffer, 0, sizeof(strBuffer) );
		m_cJZPExtractorDestPath.GetWindowText(strBuffer, sizeof(strBuffer) );

		sprintf( m_strJZPDestPath, "%s", strBuffer );
	}

	if( (strlen( m_strJZPSrcPath ) == 0) || (strlen(m_strJZPDestPath) == 0 ) )
	{
		MessageBox( "폴더를 모두 설정해주십시요." );
	}
	else
	{
		if( !strcmp( m_strJZPSrcPath, m_strJZPDestPath ) )
		{
			MessageBox( "같은 폴더에 JZP파일을 풀수 없습니다." );
		}
		else
		{
			if( DepressJZP( m_strJZPSrcPath, m_strJZPDestPath ) )
			{
				MessageBox( "JZP파일 풀기 성공" );
			}
			else
			{
				MessageBox( "JZP파일 풀기 실패" );
			}
		}
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonPackingextractorScanSrcpath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ScanFolder( m_strPackingSrcPath ) )
	{
		m_cDatExtractorScrPath.SetWindowText( m_strPackingSrcPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonPackingextractorScanDestpath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ScanFolder( m_strPackingDestPath ) )
	{
		m_cDatExtractorDestPath.SetWindowText( m_strPackingDestPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonPackingextractor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_cDatExtractorDestPath.GetWindowTextLength() )
	{
		char			strBuffer[255];
		
		memset( strBuffer, 0, sizeof(strBuffer) );
		m_cDatExtractorDestPath.GetWindowText(strBuffer, sizeof(strBuffer) );

		sprintf( m_strPackingDestPath, "%s", strBuffer );
	}

	if( (strlen( m_strPackingSrcPath ) == 0) || (strlen(m_strPackingDestPath) == 0 ) )
	{
		MessageBox( "폴더를 모두 설정해주십시요." );
	}
	else
	{
		if( !strcmp( m_strPackingSrcPath, m_strPackingDestPath ) )
		{
			MessageBox( "같은 폴더에 Dat파일을 풀수 없습니다." );
		}
		else
		{
			if( DepressDat( m_strPackingSrcPath, m_strPackingDestPath ) )
			{
				MessageBox( "Dat파일 풀기 성공" );
			}
			else
			{
				MessageBox( "Dat파일 풀기 실패" );
			}
		}
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonCompareScanSrcpath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ScanFolder( m_strCompareSrcPath ) )
	{
		m_cCompareScrPath.SetWindowText( m_strCompareSrcPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonCompareScanDestpath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ScanFolder( m_strCompareDestPath ) )
	{
		m_cCompareDestPath.SetWindowText( m_strCompareDestPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonCompare()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( (strlen( m_strCompareSrcPath ) == 0) || (strlen(m_strCompareDestPath) == 0 ) )
	{
		MessageBox( "폴더를 모두 설정해주십시요." );
	}
	else
	{
		if( !strcmp( m_strCompareSrcPath, m_strCompareDestPath ) )
		{
			MessageBox( "같은 폴더를 비교할 수 없습니다." );
		}
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}
