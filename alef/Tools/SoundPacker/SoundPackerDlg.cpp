// SoundPackerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SoundPacker.h"
#include "SoundPackerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CSoundPackerDlg dialog

CSoundPackerDlg::CSoundPackerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundPackerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoundPackerDlg)
	m_strResult = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_drvDigital = NULL;
}

void CSoundPackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundPackerDlg)
	DDX_Text(pDX, IDC_EDIT2, m_strResult);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSoundPackerDlg, CDialog)
	//{{AFX_MSG_MAP(CSoundPackerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PACK, OnButtonPack)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_UNPACK, OnButtonUnpack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundPackerDlg message handlers

typedef struct provider_stuff
{
	char* name;
	HPROVIDER id;
} provider_stuff;

BOOL CSoundPackerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	AIL_set_redist_directory( ".\\redist\\" MSS_REDIST_DIR_NAME );
	AIL_startup();

	m_drvDigital = AIL_open_digital_driver( 22050, 16, 2, 0 );

	provider_stuff		pi;	
	HPROENUM			enum3D	= HPROENUM_FIRST;
	m_hProvider = -1;
	//Search Usable Provider
	while (AIL_enumerate_3D_providers(&enum3D, &pi.id, &pi.name))
	{
		if(!strcmp(pi.name,"Miles Fast 2D Positional Audio"))
		{
			AIL_open_3D_provider( pi.id );
			m_hProvider = pi.id;
			break;
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSoundPackerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSoundPackerDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSoundPackerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSoundPackerDlg::OnOK() 
{
	CDialog::OnOK();
}

// 저장 방식 바뀜 2005.2.23 gemani
typedef		struct	tag_SoundFile					
{
	INT32				file_size;
	INT32				file_offset;
	UINT32				sound_length;
	
	char				szFileName[12];
}SoundFile;

typedef		struct	tag_SoundCategory
{
	char				szCategory[4];					// 접두어 2-3글자

	char				cPrefixSize;					// 접두어 길이 2 or 3
	char				szPackFileName[7];				// 접두어 2-3글자 .pk
	HANDLE				hPackFile;
	
	INT32				iSoundFileNum;
	INT32				iSoundFileMaxIndex;				// 동적 할당 초기화용
	SoundFile*			pArraySoundFile;				// 동적 할당

	tag_SoundCategory*	next;

	//PackingTool에서만 쓰임 .. Client주석 처리하자
	INT32				accumulated_file_offset;
}SoundCategory;

typedef		struct	tag_SoundDirectory
{
	char					szDirectory[32];
	
	INT32					iCategoryNum;
	SoundCategory*			listCategory;

	tag_SoundDirectory*		next;
}SoundDirectory;

SoundDirectory*		listSoundDir = NULL;
INT32				dir_count = 0;
INT32				pack_index = 0;

// UI폴더 패킹 안함 
BOOL CSoundPackerDlg::SetDirectoryFiles(char*	szDirectory)
{
	INT32		foffset = 0;

	char		oldDirectory[256];

	GetCurrentDirectory(256,oldDirectory);
	
	BOOL	bRet = SetCurrentDirectory(szDirectory);
	if(!bRet) return FALSE;

	char		nwDirectory[256];
	GetCurrentDirectory(256,nwDirectory);
	_strupr(nwDirectory);

	if(strstr(nwDirectory,"BGM") || strstr(nwDirectory,"STREAM") || strstr(nwDirectory,"REDIST") 
		|| strstr(nwDirectory,"REDIST\\WIN32") || strstr(nwDirectory,"UI") )
	{
		SetCurrentDirectory(oldDirectory);
		return FALSE;
	}

	++dir_count;
	char*		pcFind = strstr(nwDirectory,"\\SOUND\\");
	if(pcFind == NULL)
	{
		char	szMsg[64];
		wsprintf(szMsg,"this Program is always in \\Sound directory .. \n");
		PrintInfo(szMsg);
		Sleep(10000);			// 10초 대기
	}

	SoundDirectory*		nw_dir = new SoundDirectory;
	
	if(strlen(pcFind+7) >= 32)
	{
		char	szMsg[64];
		wsprintf(szMsg,"directory name size(32) overflow .. %s\n",pcFind+6);
		PrintInfo(szMsg);
		Sleep(10000);			// 10초 대기
	}

	strcpy(nw_dir->szDirectory,pcFind+7);
	nw_dir->listCategory = NULL;
	nw_dir->iCategoryNum = 0;
	nw_dir->next = listSoundDir;
	listSoundDir = nw_dir;
	
	SoundDirectory*		cur_dir = nw_dir;

	WIN32_FIND_DATA		find_data;
	HANDLE	handle = FindFirstFile("*.*",&find_data);

	if( INVALID_HANDLE_VALUE == handle)
	{
		DWORD	error = GetLastError();
		return FALSE;
	}

	INT32		prefix_size = 0;
	if(strstr(nwDirectory,"EFFECT\\MONO"))
		prefix_size = 3;
	else
		prefix_size = 2;

	while(1)
	{
		BOOL	bRet = FindNextFile(handle,&find_data);
		if(!bRet)
		{
			DWORD	error = GetLastError();
			break;
		}

		if(!strcmp(".",find_data.cFileName) || !strcmp("..",find_data.cFileName) || !strcmp(".svn",find_data.cFileName))
			continue;

		if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			SetDirectoryFiles(find_data.cFileName);
			continue;
		}

		if(!strstr(find_data.cFileName,".pk") && strlen(find_data.cFileName) != (unsigned int)8+prefix_size)
		{
			char	szMsg[64];
			wsprintf(szMsg,"file name Format: Prefix(%d)+ID(4).ext(3) is invalid .. %s\n",prefix_size,find_data.cFileName);
			PrintInfo(szMsg);
			Sleep(8000);			// 8초 대기

			continue;
		}

		// 기존 카테고리 검색
		BOOL	bFindInCategory = FALSE;
		char	szTemp_Category[10];
		strncpy(szTemp_Category,find_data.cFileName,prefix_size);
		szTemp_Category[prefix_size] = '\0';

		SoundCategory*		cur_category = cur_dir->listCategory;
		while(cur_category)
		{
			if(!strcmp(szTemp_Category,cur_category->szCategory))
			{
				bFindInCategory = TRUE;
				break;
			}

			cur_category = cur_category->next;
		}

		INT32	file_ID = atoi(find_data.cFileName + prefix_size);
		
		HANDLE	hfile = CreateFile(find_data.cFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
		INT32			file_size = GetFileSize(hfile,NULL);
		CloseHandle(hfile);

		SoundFile*		pSetFile;

		if(bFindInCategory)
		{
			pSetFile = &cur_category->pArraySoundFile[file_ID];
		}
		else
		{
			SoundCategory*	nw_category = new SoundCategory;
			nw_category->cPrefixSize = prefix_size;
			nw_category->hPackFile = NULL;
			nw_category->iSoundFileMaxIndex = 0;
			nw_category->iSoundFileNum = 0;
			nw_category->pArraySoundFile = new SoundFile[16000];
			memset(nw_category->pArraySoundFile,0,sizeof(SoundFile)*16000);
			if(strlen(szTemp_Category) > sizeof(nw_category->szCategory))
			{
				char	szMsg[64];
				wsprintf(szMsg,"category name이 너무 길어요.. 수정하고 재작업 하세요! %s\n",szTemp_Category);
				PrintInfo(szMsg);
				Sleep(10000);			// 10초 대기
			}
			strcpy(nw_category->szCategory,szTemp_Category);
			wsprintf(nw_category->szPackFileName,"%s.pk",szTemp_Category);
			nw_category->next = cur_dir->listCategory;
			cur_dir->listCategory = nw_category;

			nw_category->accumulated_file_offset = 0;
			++cur_dir->iCategoryNum;

			cur_category = nw_category;
			pSetFile = &nw_category->pArraySoundFile[file_ID];
		}
		
		_strupr(find_data.cFileName);
		if(strlen(find_data.cFileName) > sizeof(pSetFile->szFileName))
		{
			char	szMsg[64];
			wsprintf(szMsg,"SoundFileName이 너무 길어요.. 수정하고 재작업 하세요! %s\n",find_data.cFileName);
			PrintInfo(szMsg);
			Sleep(10000);			// 10초 대기
		}

		strcpy(pSetFile->szFileName,find_data.cFileName);
		pSetFile->file_offset = cur_category->accumulated_file_offset;
		pSetFile->file_size = file_size;

		// sound length 저장
		HSTREAM hStream = AIL_open_stream( m_drvDigital, find_data.cFileName, 0 );
		if(hStream)
		{
			S32	nDataRate , length ;	
			nDataRate	= hStream->datarate;
			length		= hStream->totallen;
			AIL_close_stream( hStream );
			pSetFile->sound_length = ( INT32 ) ( ( ( FLOAT ) length * 1000.0f ) / ( FLOAT ) nDataRate );
		}
		else
		{
			pSetFile->sound_length = 0;
		}
		
		cur_category->accumulated_file_offset += file_size;

		++cur_category->iSoundFileNum;
		if(file_ID > cur_category->iSoundFileMaxIndex)
			cur_category->iSoundFileMaxIndex = file_ID;
	}

	FindClose(handle);
	
	SetCurrentDirectory(oldDirectory);

	return TRUE;
}

HANDLE	hConsole = NULL;

void CSoundPackerDlg::PrintInfo(char*	szMessage)
{
	if(hConsole)
		::WriteConsole( hConsole, (void*)szMessage, strlen(szMessage), NULL, NULL );
}

void CSoundPackerDlg::OnButtonPack() 
{
	SetCurrentDirectory(".\\Sound");
	
	m_strResult = "Failed";
	UpdateData(FALSE);

	AllocConsole();
	hConsole = GetStdHandle( STD_OUTPUT_HANDLE);
	
	if( hConsole )
	{
		::SetConsoleTitle( "Info" );
	}

	PrintInfo("Packing Start!\n");
	
	PrintInfo("File List Create\n");
							
	WIN32_FIND_DATA		find_data;
	HANDLE	handle =FindFirstFile("*.*",&find_data);

	if( INVALID_HANDLE_VALUE == handle)
	{
		DWORD	error = GetLastError();
		return;
	}

	while(1)
	{
		BOOL	bRet = FindNextFile(handle,&find_data);
		if(!bRet)
		{
			DWORD	error = GetLastError();
			break;
		}

		if(!strcmp(".",find_data.cFileName) || !strcmp("..",find_data.cFileName) || !strcmp(".svn",find_data.cFileName))
			continue;

		if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			SetDirectoryFiles(find_data.cFileName);			
			continue;
		}
	}
	FindClose(handle);

	PrintInfo("File List Create Success\n");
			
	// packing info file save
	HANDLE	fd = CreateFile("SoundPak.info",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD	FP;

	if(fd == INVALID_HANDLE_VALUE)
	{
		DWORD	error = GetLastError();
		return;
	}

	PrintInfo("Packing Info file Save\n");

	FILE*	fp = fopen("FileOffsetLog.txt","w");
	fclose(fp);

	SoundDirectory*		cur_dir = listSoundDir;
	SoundCategory*		cur_category;
	SoundFile*			cur_file;

	WriteFile(fd,&dir_count,sizeof(dir_count),&FP,NULL);

	while(cur_dir)
	{
		WriteFile(fd,cur_dir->szDirectory,sizeof(cur_dir->szDirectory),&FP,NULL);
		WriteFile(fd,&cur_dir->iCategoryNum,sizeof(cur_dir->iCategoryNum),&FP,NULL);

		FILE*	fp = fopen("FileOffsetLog.txt","a+");
		fprintf(fp,"Directory == %s \n",cur_dir->szDirectory);
		fclose(fp);

		cur_category = cur_dir->listCategory;
		while(cur_category)
		{
			FILE*	fp = fopen("FileOffsetLog.txt","a+");
			fprintf(fp,"Category == %s \n",cur_category->szCategory);
			fclose(fp);

			WriteFile(fd,&cur_category->cPrefixSize,sizeof(cur_category->cPrefixSize),&FP,NULL);
			WriteFile(fd,&cur_category->szPackFileName,sizeof(cur_category->szPackFileName),&FP,NULL);
			WriteFile(fd,&cur_category->szCategory,sizeof(cur_category->szCategory),&FP,NULL);
			WriteFile(fd,&cur_category->iSoundFileNum,sizeof(cur_category->iSoundFileNum),&FP,NULL);
			WriteFile(fd,&cur_category->iSoundFileMaxIndex,sizeof(cur_category->iSoundFileMaxIndex),&FP,NULL);
			
			for(int i=0;i<=cur_category->iSoundFileMaxIndex;++i)
			{
				cur_file = &cur_category->pArraySoundFile[i];
				if(strlen(cur_file->szFileName))
				{
					WriteFile(fd,&i,sizeof(i),&FP,NULL);			// file id 저장
					WriteFile(fd,cur_file->szFileName,sizeof(cur_file->szFileName),&FP,NULL);
					WriteFile(fd,&cur_file->file_offset,sizeof(cur_file->file_offset),&FP,NULL);
					WriteFile(fd,&cur_file->file_size,sizeof(cur_file->file_size),&FP,NULL);
					WriteFile(fd,&cur_file->sound_length,sizeof(cur_file->sound_length),&FP,NULL);

					FILE*	fp = fopen("FileOffsetLog.txt","a+");
					fprintf(fp,"%s - offset : %d, size : %d\n",cur_file->szFileName,cur_file->file_offset,cur_file->file_size);
					fclose(fp);
				}
			}

			cur_category = cur_category->next;
		}
		cur_dir = cur_dir->next;
	}

	CloseHandle(fd);

	PrintInfo("Packing Info file Save Success!\n");
	PrintInfo("Sound File Packing Start\n");

	// packing file save
	cur_dir = listSoundDir;

	char		oldDirectory[256];
	GetCurrentDirectory(256,oldDirectory);
	char		szMsg[256];
	
	while(cur_dir)
	{
		wsprintf(szMsg,"Directory Set - %s\n",cur_dir->szDirectory);
		PrintInfo(szMsg);

		SetCurrentDirectory(cur_dir->szDirectory);

		cur_category = cur_dir->listCategory;
		while(cur_category)
		{
			fd = CreateFile(cur_category->szPackFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			if(fd == INVALID_HANDLE_VALUE)
			{
				DWORD	error = GetLastError();
				return;
			}

			wsprintf(szMsg,"Category Set - %s\n",cur_category->szCategory);
			PrintInfo(szMsg);

			for(int i=0;i<=cur_category->iSoundFileMaxIndex;++i)
			{
				cur_file = &cur_category->pArraySoundFile[i];
				if(strlen(cur_file->szFileName))
				{
					HANDLE	hfile_src = CreateFile(cur_file->szFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
					DWORD	FP_src;
					BYTE*	pLoadBuffer = new BYTE[cur_file->file_size];
					ReadFile(hfile_src,pLoadBuffer,cur_file->file_size,&FP_src,NULL);

					SetFilePointer(fd,cur_file->file_offset,NULL,FILE_BEGIN);
					WriteFile(fd,pLoadBuffer,cur_file->file_size,&FP,NULL);
					
					CloseHandle(hfile_src);
					delete	[]pLoadBuffer;

					wsprintf(szMsg,"File Packed- name:%s, size:%d , length:%d\n",cur_file->szFileName,cur_file->file_size,cur_file->sound_length);
					PrintInfo(szMsg);
				}
			}

			CloseHandle(fd);
			
			cur_category = cur_category->next;
		}

		SetCurrentDirectory(oldDirectory);
		cur_dir = cur_dir->next;
	}
	
	PrintInfo("Sound File Packing Success\n");

	fp = fopen("FileIndexEmpty.txt","w");
	fclose(fp);

	// 빈슬롯 체크
	cur_dir = listSoundDir;
	while(cur_dir)
	{
		cur_category = cur_dir->listCategory;
		while(cur_category)
		{
			for(int i=0;i<=cur_category->iSoundFileMaxIndex;++i)
			{
				if(!strlen(cur_category->pArraySoundFile[i].szFileName))
				{
					FILE*	fp = fopen("FileIndexEmpty.txt","a+");
					fprintf(fp,"Category - %s , index - %d\n",cur_category->szCategory,i);
					fclose(fp);
				}
			}

			cur_category = cur_category->next;
		}
		cur_dir = cur_dir->next;
	}

	PrintInfo("Sound List Free Start\n");

	SoundDirectory*		remove_dir;
	SoundCategory*		remove_category;
	
	cur_dir = listSoundDir;
	while(cur_dir)
	{
		cur_category = cur_dir->listCategory;
		while(cur_category)
		{
			remove_category = cur_category;
			cur_category = cur_category->next;

			delete []remove_category->pArraySoundFile;
			delete remove_category;
		}
		
		remove_dir = cur_dir;
		cur_dir = cur_dir->next;
		delete	remove_dir;
	}

	PrintInfo("Sound List Free Success\n");

	Sleep(3000);			// 3초 대기

	::FreeConsole();
	hConsole = NULL;

	m_strResult = "Success!";
	UpdateData(FALSE);
}

void CSoundPackerDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

	if(m_hProvider != -1)
	{
		AIL_close_3D_provider( m_hProvider );
		m_hProvider = -1;
	}

	if (m_drvDigital)
	{
		AIL_close_digital_driver( m_drvDigital );
		m_drvDigital = 0;
	}

	// Now shutdown Miles completely
	AIL_shutdown();

	CDialog::OnClose();
}

void CSoundPackerDlg::OnButtonUnpack() 
{
	//SetCurrentDirectory(".\\Sound");

	char		szMsg[256];

	// TODO: Add your control notification handler code here
	m_strResult = "Failed";
	UpdateData(FALSE);

	AllocConsole();
	hConsole = GetStdHandle( STD_OUTPUT_HANDLE);
	
	if( hConsole )
	{
		::SetConsoleTitle( "Info" );
	}

	PrintInfo("UnPacking Start!\n");
	
	PrintInfo("Packing Data Read\n");

	HANDLE	fd = CreateFile("soundpak.info",GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);

	if(fd == INVALID_HANDLE_VALUE)
	{
		PrintInfo("soundpak.info read error!\n");
		return;
	}

	INT32	iDirectoryCount = 0;
	DWORD	FP;

	ReadFile(fd,&iDirectoryCount,sizeof(iDirectoryCount),&FP,NULL);

	SoundDirectory*		pSoundDirectory = NULL;
	SoundDirectory*		nw_dir;
	SoundCategory*		nw_category;
	SoundFile*			cur_file;
	INT32				file_index;

	char				oldDir[256];
	GetCurrentDirectory(256,oldDir);

	for(int i=0;i<iDirectoryCount;++i)
	{
		nw_dir = new SoundDirectory;

		ReadFile(fd,nw_dir->szDirectory,sizeof(nw_dir->szDirectory),&FP,NULL);
		ReadFile(fd,&nw_dir->iCategoryNum,sizeof(nw_dir->iCategoryNum),&FP,NULL);

		SetCurrentDirectory(oldDir);

		BOOL	bSet = SetCurrentDirectory(nw_dir->szDirectory);
		if(!bSet)
		{
			CreateDirectory(nw_dir->szDirectory,NULL);
			SetCurrentDirectory(nw_dir->szDirectory);
		}

		nw_dir->listCategory = NULL;
		nw_dir->next = pSoundDirectory;
		pSoundDirectory = nw_dir;
		
		for(int j=0;j<nw_dir->iCategoryNum;++j)
		{
			nw_category = new SoundCategory;
		
			ReadFile(fd,&nw_category->cPrefixSize,sizeof(nw_category->cPrefixSize),&FP,NULL);
			ReadFile(fd,&nw_category->szPackFileName,sizeof(nw_category->szPackFileName),&FP,NULL);
			ReadFile(fd,&nw_category->szCategory,sizeof(nw_category->szCategory),&FP,NULL);
			ReadFile(fd,&nw_category->iSoundFileNum,sizeof(nw_category->iSoundFileNum),&FP,NULL);
			ReadFile(fd,&nw_category->iSoundFileMaxIndex,sizeof(nw_category->iSoundFileMaxIndex),&FP,NULL);

			nw_category->hPackFile = CreateFile(nw_category->szPackFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
			nw_category->pArraySoundFile = new SoundFile[nw_category->iSoundFileMaxIndex+1];
			memset(nw_category->pArraySoundFile,0,sizeof(SoundFile) * (nw_category->iSoundFileMaxIndex+1));

			nw_category->next = nw_dir->listCategory;
			nw_dir->listCategory = nw_category;

			for(int k=0;k<nw_category->iSoundFileNum;++k)
			{
				ReadFile(fd,&file_index,sizeof(file_index),&FP,NULL);
				cur_file = &nw_category->pArraySoundFile[file_index];
				ReadFile(fd,cur_file->szFileName,sizeof(cur_file->szFileName),&FP,NULL);
				ReadFile(fd,&cur_file->file_offset,sizeof(cur_file->file_offset),&FP,NULL);
				ReadFile(fd,&cur_file->file_size,sizeof(cur_file->file_size),&FP,NULL);
				ReadFile(fd,&cur_file->sound_length,sizeof(cur_file->sound_length),&FP,NULL);
			}
		}
	}

	CloseHandle(fd);

	// 여기부터 코딩 ^^
	PrintInfo("File Unpack!\n");

	SoundDirectory*		cur_dir = pSoundDirectory;
	
	while(cur_dir)
	{
		SetCurrentDirectory(oldDir);

		SetCurrentDirectory(cur_dir->szDirectory);
		
		SoundCategory*	cur_category = cur_dir->listCategory;

		while(cur_category)
		{
			for(int i=0;i<= cur_category->iSoundFileMaxIndex;++i)
			{
				if(!strlen(cur_category->pArraySoundFile[i].szFileName))	continue;

				cur_file = &cur_category->pArraySoundFile[i];

				HANDLE	hfile_Dest = CreateFile(cur_file->szFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
				DWORD	FP_src;
				DWORD	FP_dst;
				BYTE*	pLoadBuffer = new BYTE[cur_file->file_size];

				SetFilePointer(cur_category->hPackFile,cur_file->file_offset,NULL,FILE_BEGIN);
				ReadFile(cur_category->hPackFile,pLoadBuffer,cur_file->file_size,&FP_src,NULL);

				WriteFile(hfile_Dest,pLoadBuffer,cur_file->file_size,&FP_dst,NULL);
				CloseHandle(hfile_Dest);
				delete []pLoadBuffer;

				wsprintf(szMsg,"File UnPacked- name:%s\n",cur_file->szFileName);
				PrintInfo(szMsg);
			}

			cur_category = cur_category->next;
		}

		cur_dir = cur_dir->next;
	}

	PrintInfo("Sound File UnPacking Success\n");

	PrintInfo("Sound List Free Start\n");

	SoundDirectory*		remove_dir;
	SoundCategory*		cur_category;
	SoundCategory*		remove_category;
	
	cur_dir = pSoundDirectory;
	while(cur_dir)
	{
		cur_category = cur_dir->listCategory;
		while(cur_category)
		{
			remove_category = cur_category;
			cur_category = cur_category->next;

			CloseHandle(remove_category->hPackFile);

			delete []remove_category->pArraySoundFile;
			delete remove_category;
		}

		remove_dir = cur_dir;
		cur_dir = cur_dir->next;

		delete	remove_dir;
	}

	PrintInfo("Sound List Free Success\n");

	::FreeConsole();
	hConsole = NULL;

	m_strResult = "Success!";
	UpdateData(FALSE);
}
