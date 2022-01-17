// DriverDownDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PatchClient.h"
#include "DriverDownDlg.h"

#include "PatchClientDlg.h"
#include "AutoDetectMemoryLeak.h"

#ifdef _DEBUG

#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif



// DriverDownDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(DriverDownDlg, CDialog)
DriverDownDlg::DriverDownDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DriverDownDlg::IDD, pParent)
{
	m_bSelectResult		= FALSE;
	m_strFileName		= "";
	m_strDriverURL		= "";
	m_bDownloadSuccess	= FALSE;
}

DriverDownDlg::~DriverDownDlg()
{
}

void DriverDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchClientOptionDlg)
	DDX_Control(pDX, IDC_BUTTON_DRIVER_OK, m_cKbcOKButton);
	DDX_Control(pDX, IDC_BUTTON_DRIVER_NO, m_cKbcNOButton);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(DriverDownDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DRIVER_OK, OnBnClickedButtonDriverOk)
	ON_BN_CLICKED(IDC_BUTTON_DRIVER_NO, OnBnClickedButtonDriverNo)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHECK_DONOTUSECHECKDRIVER, OnBnClickedCheckDonotusecheckdriver)
END_MESSAGE_MAP()


// DriverDownDlg 메시지 처리기입니다.

BOOL DriverDownDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_cDrawFont.CreateFont( 14, 0, 0, 0, 500, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "돋움" );

	m_cBMPBackground.LoadBitmap( IDB_DRIVERBACK );

	m_cKbcOKButton.SetBmpButtonImage( IDB_Button_Option_Save_Set, RGB(0,0,255) );
	m_cKbcOKButton.SetButtonText( "예" );
	m_cKbcNOButton.SetBmpButtonImage( IDB_Button_Option_Cancel_Set, RGB(0,0,255) );
	m_cKbcNOButton.SetButtonText( "아니오" );

	GetDlgItem( IDC_BUTTON_DRIVER_OK )->SetWindowPos( NULL, 75, 150, 87, 22, SWP_SHOWWINDOW );
	GetDlgItem( IDC_BUTTON_DRIVER_NO )->SetWindowPos( NULL, 228, 150, 87, 22, SWP_SHOWWINDOW );

	m_ctrlProgress.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL	DriverDownDlg::AutoDownloadAndExecDriver()
{
	CInternetSession	session;
	CInternetFile*		file = NULL;
	try
    {
        file = (CInternetFile *)session.OpenURL( m_strDriverURL );
    }
    catch( CInternetException * pEx )
    {
        TCHAR    lpszError[256];
        pEx->GetErrorMessage( lpszError, 256, NULL );
        return false;
    }

	if( !file )			return false;

	bool			bResult = true;
	FILE			*fp;

	int		iBufferSize = 4096;
	int		iReadByte;
	int		iTotalRecvByte = 0;

	
	int iFileSize = (int) file->GetLength();		//파일의 사이즈를 알아낸다.
	CString cFileName = file->GetFileName();		//파일 이름을 얻어낸다.
    file->SetReadBufferSize(iBufferSize);

	fp = fopen( cFileName.GetBuffer(0), "wb" );
	if( !fp )
	{
		file->Close();
		delete file;
		session.Close();
	}

	m_strFileName = cFileName.GetBuffer( 0 );
	m_ctrlProgress.SetRange32( 0, iFileSize );

	char* pstrData = new char[iBufferSize];

	while(1)
	{ 
		iReadByte		= file->Read( pstrData, iBufferSize );
		iTotalRecvByte	+= iReadByte;
		if( iReadByte )
		{
			RECT	rect;
			m_ctrlProgress.GetWindowRect( &rect );
			m_ctrlProgress.SetPos( iTotalRecvByte );
			InvalidateRect( &rect );

			int iWriteBytes = (int)fwrite( pstrData, 1, iReadByte, fp );
			if( iWriteBytes != iReadByte )
			{
//				MessageBox( "하드 디스크가 부족?" );
				bResult = false;
				break;
			}
		}
		else
			break;
	}

	delete [] pstrData;
	fclose( fp );

	cFileName.ReleaseBuffer();
    file->Close();
	delete file;
	session.Close();

	return bResult;
}

void DriverDownDlg::OnBnClickedButtonDriverOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSelectResult = TRUE;

	m_ctrlProgress.ShowWindow(SW_SHOW);

	if( AutoDownloadAndExecDriver() )
		m_bDownloadSuccess = TRUE;
		
	OnOK();
}

void DriverDownDlg::OnBnClickedButtonDriverNo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSelectResult = FALSE;
	OnCancel();
}

void DriverDownDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	CDC		memdc;
	memdc.CreateCompatibleDC( &dc );
	memdc.SelectObject( &m_cBMPBackground );

	BITMAP	cBitmap;
	m_cBMPBackground.GetBitmap( &cBitmap );

	dc.BitBlt( 0, 0, cBitmap.bmWidth, cBitmap.bmHeight, &memdc, 0, 0, SRCCOPY );
	
	//폰트처리
	//Text 관련 세팅.
	dc.SetBkMode(TRANSPARENT);              
	CFont* pOldFont = (CFont*)dc.SelectObject(&m_cDrawFont);
	dc.SetTextColor( RGB(255,255,255) );

	CString	str;
	dc.SetTextColor( RGB(255,0,0) );

	str = "아크로드를 원할히 즐기기 위해서는 반드시 최신";
	dc.TextOut( 32, 54, str, str.GetLength());
	
	str = "그래픽카드 드라이버를 다운로드 받아야 합니다. ";
	dc.TextOut( 32, 74, str, str.GetLength());

	dc.SetTextColor( RGB(255,255,255) );
	str = "예를 누르시면 최신 드라이버를 자동으로 설치해줍니다";
	dc.TextOut( 32, 94, str, str.GetLength());

	str = "최신 그래픽카드 드라이버를 설치하시겠습니까?";
	dc.TextOut( 32, 114, str, str.GetLength());

	str = "다음부터 드라이버 자동업데이트 사용하지 않음";
	dc.TextOut( 55, 190, str, str.GetLength());

	dc.SelectObject(pOldFont);
}

void DriverDownDlg::OnBnClickedCheckDonotusecheckdriver()
{
	BOOL bUse = ! ( (CButton*)GetDlgItem( IDC_CHECK_DONOTUSECHECKDRIVER ) )->GetCheck();
	CPatchClientRegistry cPathchReg;
	cPathchReg.SetUseDriverUpdate( bUse );
}
