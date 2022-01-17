// BillingClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "BillingClient.h"
#include "BillingClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CBillingClientDlg 대화 상자


CBillingClientDlg::CBillingClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBillingClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBillingClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBillingClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
#ifdef VERSION_VS60
	ON_BN_CLICKED(IDC_BUT_CONNECT, OnBnClickedButConnect)
	ON_BN_CLICKED(IDC_BUT_LOGIN, OnBnClickedButLogin)
	ON_BN_CLICKED(IDC_BUT_LOGOUT, OnBnClickedButLogout)
	ON_BN_CLICKED(IDC_BUT_INQUIRE, OnBnClickedButInquire)	
	ON_BN_CLICKED(IDC_BUT_INQUIRE_MULTI, OnBnClickedButInquireMulti)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUT_INQUIRE_PCPOINT, OnBnClickedButInquirePcpoint)
	//ON_BN_CLICKED(IDC_BUT_INQUIRE_DEDUCTTYPE, OnBnClickedButInquireDeducttype)
#else
	ON_BN_CLICKED(IDC_BUT_CONNECT, &CBillingClientDlg::OnBnClickedButConnect)
	ON_BN_CLICKED(IDC_BUT_LOGIN, &CBillingClientDlg::OnBnClickedButLogin)
	ON_BN_CLICKED(IDC_BUT_LOGOUT, &CBillingClientDlg::OnBnClickedButLogout)
	ON_BN_CLICKED(IDC_BUT_INQUIRE, &CBillingClientDlg::OnBnClickedButInquire)
	ON_BN_CLICKED(IDC_BUT_INQUIRE_MULTI, &CBillingClientDlg::OnBnClickedButInquireMulti)
	ON_BN_CLICKED(IDCANCEL, &CBillingClientDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUT_INQUIRE_PCPOINT, &CBillingClientDlg::OnBnClickedButInquirePcpoint)
	//ON_BN_CLICKED(IDC_BUT_INQUIRE_DEDUCTTYPE, &CBillingClientDlg::OnBnClickedButInquireDeducttype)
#endif	
	ON_BN_CLICKED(IDC_BUTTON2, &CBillingClientDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CBillingClientDlg 메시지 처리기

BOOL CBillingClientDlg::OnInitDialog()
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
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	
	//SetDlgItemText(IDC_EDIT_IP, "10.101.203.202");
	SetDlgItemText(IDC_EDIT_IP, _T("218.234.76.11"));
	SetDlgItemText(IDC_EDIT_PORT, _T("45610"));
	SetDlgItemText(IDC_EDIT_LOGIN_COUNT, _T("1"));
	SetDlgItemText(IDC_EDIT_LOGIN_PC, _T("0"));
	SetDlgItemText(IDC_EDIT_LOGIN_GAMECODE, _T("417"));

	m_Client.CreateSession(CBillingClientDlg::ErrorHandler); //클라이언트 초기화

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CBillingClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CBillingClientDlg::OnPaint()
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
HCURSOR CBillingClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBillingClientDlg::OnBnClickedButConnect()
{	
	CString strIP, strPort;
	
	GetDlgItemText(IDC_EDIT_IP, strIP);
	GetDlgItemText(IDC_EDIT_PORT, strPort);
	
	m_Client.Connect(strIP.GetBuffer(0), _tstoi(strPort.GetBuffer(0)));
}

void CBillingClientDlg::OnBnClickedButLogin()
{
	//char* strAccountID, DWORD dwUserGUID, DWORD dwIPAddress, DWORD dwRoomGUID, DWORD dwGameCode, DWORD dwServerType
	//접속이 성공한후 로그인 해야한다. 로그인 여부는 OnConnected 메소드를 통해 확인한다.
	CString str, strCount, strPC, strGameCode;
	GetDlgItemText(IDC_EDIT_LOGIN, str);	

	GetDlgItemText(IDC_EDIT_LOGIN_COUNT, strCount);
	GetDlgItemText(IDC_EDIT_LOGIN_PC, strPC);
	GetDlgItemText(IDC_EDIT_LOGIN_GAMECODE, strGameCode);	

	int nID = _tstoi(str.GetBuffer(0));
	int nCount = _tstoi(strCount.GetBuffer(0));
	int nPC = _tstoi(strPC.GetBuffer(0));
	int nGameCode = _tstoi(strGameCode.GetBuffer(0));

	for(int i = 0 ; i < nCount ; i++)
	{
		m_Client.UserLogin(nID + i, inet_addr("127.0.0.1"), nPC, nGameCode, 1);
	}	
}

void CBillingClientDlg::OnBnClickedButLogout()
{	
	CString str;
	GetDlgItemText(IDC_EDIT_BILLINGID, str);
	
	m_Client.UserLogout( _tstoi(str.GetBuffer(0)));
}

void CBillingClientDlg::OnBnClickedButInquire()
{
	CString str;
	GetDlgItemText(IDC_EDIT_INQUIRE, str);

	m_Client.InquireUser( _tstoi(str.GetBuffer(0)));
	/*

	for(int i = 0 ; i < 100000 ; i++)	
	{
		Sleep(1000 * 60);
		m_Client.InquireUser( _tstoi(str.GetBuffer(0)));
	}
	*/
}

void CBillingClientDlg::OnBnClickedButInquireMulti()
{
	CString str, str1;
	GetDlgItemText(IDC_EDIT_MULTI, str);
	GetDlgItemText(IDC_EDIT_MULTIACCOUNT, str1);
	
	m_Client.InquireMultiUser( _tstoi(str1.GetBuffer(0)), _tstoi(str.GetBuffer(0)));
}

void CBillingClientDlg::OnBnClickedButInquirePcpoint()
{
	CString str1, str2, str3;
	GetDlgItemText(IDC_EDIT_PCPOINT_ACCOUNT, str1);
	GetDlgItemText(IDC_EDIT_PCPOINT_PCACCOUNT, str2);
	GetDlgItemText(IDC_EDIT_PCPOINT_GAMECODE, str3);

	m_Client.InquirePCRoomPoint(_tstoi(str1.GetBuffer(0)), _tstoi(str2.GetBuffer(0)), _tstoi(str3.GetBuffer(0)));
}
//
//void CBillingClientDlg::OnBnClickedButInquireDeducttype()
//{
//	CString str, str1;
//
//	GetDlgItemText(IDC_EDIT_ACCOUNT, str);
//	GetDlgItemText(IDC_EDIT_LOGIN_GAMECODE2, str1);	
//
//	m_Client.InquireDeductType(_tstoi(str.GetBuffer(0)), _tstoi(str1.GetBuffer(0)));
//}


void CBillingClientDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Client.Close();
	OnCancel();
}

void CBillingClientDlg::ErrorHandler( DWORD lastError, TCHAR* desc )
{
	CTime t = CTime::GetCurrentTime();   	 
	
	CString str;
	str.Format(_T("[%s] %s (%d)\r\n"), t.Format(_T("%Y-%m-%d %H:%M:%S")), desc, lastError);

	TCHAR szModule[MAX_PATH] = {0};
	TCHAR szFilePath[MAX_PATH] = {0};

	::GetModuleFileName( NULL, szModule, MAX_PATH );

	*(_tcsrchr( szModule, '\\' ) ) = 0;

	StringCchPrintf( szFilePath, STRSAFE_MAX_CCH, _T("%s\\BillingNetWork.log"), szModule );

	HANDLE	hFile;	
	DWORD		dwBytesWritten, dwPos;  

	hFile = CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  //File 생성 
	if (hFile == INVALID_HANDLE_VALUE)
	{ 
		return;
	}  	
		
	dwPos = SetFilePointer(hFile, 0, NULL, FILE_END); 

	LockFile(hFile, dwPos, 0, dwPos + str.GetLength(), 0); 
	WriteFile(hFile, str.GetBuffer(0), str.GetLength(), &dwBytesWritten, NULL); 
	str.ReleaseBuffer();
	UnlockFile(hFile, dwPos, 0, dwPos + str.GetLength(), 0);     
	CloseHandle(hFile); 
}

void CBillingClientDlg::OnBnClickedButton2()
{
	//m_Client.Close();
	//m_Client.SetKill();

	CString str, strCount, strPC, strGameCode;
	GetDlgItemText(IDC_EDIT_LOGIN, str);	

	GetDlgItemText(IDC_EDIT_LOGIN_COUNT, strCount);
	GetDlgItemText(IDC_EDIT_LOGIN_PC, strPC);
	GetDlgItemText(IDC_EDIT_LOGIN_GAMECODE, strGameCode);	

	int nID = _tstoi(str.GetBuffer(0));
	int nCount = _tstoi(strCount.GetBuffer(0));
	int nPC = _tstoi(strPC.GetBuffer(0));
	int nGameCode = _tstoi(strGameCode.GetBuffer(0));

	m_Client.InquirePersonDeduct(nID, nGameCode);
}
