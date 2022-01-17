// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "ProgressDlg.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	RGB_PROGRESSBACKGROUND	RGB( 72 , 53, 102 )
#define	RGB_PROGRESSBAR			RGB( 255 , 21, 43 )

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

int	CProgressDlg::m_nRefCount = 0;

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	m_strPercent = _T("");
	m_strMessage = _T("");
	//}}AFX_DATA_INIT

	srand( (unsigned)time( NULL ) );

	vector< COLORREF >	colorref;
	
	/*
	colorref.push_back( RGB( 255 , 0 , 0	)  );
	colorref.push_back( RGB( 255 , 0 , 222	)  );
	colorref.push_back( RGB( 114 , 0 , 255	)  );
	colorref.push_back( RGB( 0 , 42 , 255	)  );
	colorref.push_back( RGB( 0 , 252 , 255	)  );
	colorref.push_back( RGB( 0 , 255 , 48	)  );
	colorref.push_back( RGB( 138 , 255 , 0	)  );
	colorref.push_back( RGB( 255 , 255 , 0	)  );
	colorref.push_back( RGB( 255 , 144 , 0	)  );
	colorref.push_back( RGB( 53 , 101 , 49	)  );
	colorref.push_back( RGB( 95 , 129 , 205	)  );
	colorref.push_back( RGB( 194 , 119 , 195 ) );
	colorref.push_back( RGB( 195 , 119 , 142 ) );
	colorref.push_back( RGB( 195 , 194 , 119 ) );
	colorref.push_back( RGB( 51 , 51 , 46	 ) );
	colorref.push_back( RGB( 185 , 185 , 175 ) );

	m_rgbBackground = colorref[ rand() % colorref.size() ];
	*/

	m_rgbBackground = RGB( ( rand() % 240 ) , ( rand() % 240 ) , ( rand() % 240 ) );

	CStringList	strList;

	/*
	// 금연
	strList.AddTail( "흡연! 폐를 혹사하는 행위" );
	strList.AddTail( "디스가 한갑이면 소주가 두병이다." );
	strList.AddTail( "맵툴 금연 운동 본부장 마고자" );

	// 업데이트 공지
	strList.AddTail( "2005/04/04 오브젝트 이동 개선" );
	strList.AddTail( "2005/04/04 오브젝트 선택하고 T눌러보기!" );
	strList.AddTail( "2005/04/06 세이브 체크를 작업단위로변경" );
	strList.AddTail( "2005/04/06 던젼툴 저장관련 갱신" );
	strList.AddTail( "2005/04/06 던젼윈도,마우스오른쪽 더블클릭!" );
	strList.AddTail( "2005/04/07 오브젝트 복사 C누른채 드래그." );
	strList.AddTail( "2005/04/07 오브젝트 선택후 t누르면 45도회전" );
	strList.AddTail( "2005/04/11 오브젝트 그룹회전이동스케일링" );
	strList.AddTail( "2005/04/11 오브젝트 그룹 저장&찍기 기능추가" );
	strList.AddTail( "2005/04/11 여기 칼라가 무지 다양해졌음!" );
	strList.AddTail( "2005/04/12 오브젝트이동에 관한 대량업데잇" );
	strList.AddTail( "2005/04/12 오브젝트템플릿 로딩속도개선" );
	strList.AddTail( "2005/04/13 오브젝트 선택 왕 버그 수정 -,.-" );
	strList.AddTail( "2005/04/15 세이브 데이타 정리" );
	strList.AddTail( "2005/05/16 옥트리 익스포트 수정" );
	strList.AddTail( "2005/05/16 오브젝트 정보표시 옵션추가" );

	// 팁
	strList.AddTail( "이것은 새로운 맵툴 팁 시스템" );
	strList.AddTail( "F5 , F6 으로 카메라 위치저장/로드" );
	strList.AddTail( "F7로 이벤트 오브젝트찾기,Ctrl F7로설정" );
	strList.AddTail( "Alt+S 로 쎄이브" );
	strList.AddTail( "세이브는 언제나 SubData와 동시에돼용" );
	strList.AddTail( "미니맵은 별도의 Minimap 폴더에 저장" );
	strList.AddTail( "커맨드 cms로 카메라 이동 속도조절" );
	strList.AddTail( "커맨드 rs로 케릭터 달리기 속도 조절" );
	strList.AddTail( "타일탭에서 I로 타일 피킹" );
	strList.AddTail( "백업은 생활화!" );
	strList.AddTail( "오브젝트+옥트리데이타는 항상 묶어서관리" );
	strList.AddTail( "오브젝트 그룹 찍기는 마우스오른쪽더블클릭" );
	strList.AddTail( "오브젝트 Shift Drag 는 상하이동" );
	strList.AddTail( "오브젝트 Control Drag 는 스케일링" );
	strList.AddTail( "오브젝트 Shift+Control Drag 는 회전" );
	strList.AddTail( "오브젝트는 디비젼 경계를 넘어가지 못해요" );
	strList.AddTail( "오브젝트 그룹선택시 ctrl로 산뒤에것도 선택" );
	strList.AddTail( "1버튼으로 지형메뉴" );
	strList.AddTail( "2버튼으로 타일메뉴" );
	strList.AddTail( "3버튼으로 오브젝트메뉴" );
	strList.AddTail( "던젼윈도우에서 오른번튼 두번으로 카메라이동" );
	strList.AddTail( "오브젝트 창에서 CTRL + A 로 전체선택" );
	strList.AddTail( "ALT + Drag로 오브젝트 그룹선택" );
	strList.AddTail( "ALT + Shift Drag로 오브젝트 그룹선택추가" );
	strList.AddTail( "ALT + Ctrl Drag로 오브젝트 그룹선택삭제" );
	strList.AddTail( "옵션메뉴에 오브젝트 정보 표시 추가" );

	// 잡담
	strList.AddTail( "배아제 밥사줘요~" );
	strList.AddTail( "유랑의 마고자" );
	strList.AddTail( "맵툴을 믿지 마세요." );
	strList.AddTail( "아크로드 파이팅" );
	strList.AddTail( "멋쟁이 마고자" );
	strList.AddTail( "모두의 맵툴" );
	strList.AddTail( "Maptool May Cry" );
	strList.AddTail( "현대 인류의 적 , 뱃살" );
	strList.AddTail( "맵툴은 이미 죽어 있다." );
	strList.AddTail( "맵툴의 순정" );
	strList.AddTail( "맵툴 기어 솔리드" );
	strList.AddTail( "귀염둥이 만돌 아저씨" );
	strList.AddTail( "왜 맵툴가는 곳에 담배 있는가?" );
	strList.AddTail( "맵투릭스" );
	strList.AddTail( "식사는 제때 제때 꼬박 꼬박" );
	strList.AddTail( "나는 맵툴이 무슨짓을 한지 알고있다." );
	strList.AddTail( "독도는 우리땅" );
	strList.AddTail( "그대 그리고 맵툴" );
	strList.AddTail( "맵툴 너만 있으면 돼" );
	strList.AddTail( "The Legend of Maptool" );
	strList.AddTail( "마고자님이 보고 계셔" );
	strList.AddTail( "죽느냐 사느냐 그것이 문제로다." );
	strList.AddTail( "버그있는 툴도 툴이다." );
	strList.AddTail( "맵툴이 기가 막혀" );
	strList.AddTail( "다 죽여 버리겠다!" );
	strList.AddTail( "맵툴은 그때 그때 달라요." );
	strList.AddTail( "혼자가 아닌 맵툴" );
	strList.AddTail( "고마해라. 마이주그따아이가" );
	strList.AddTail( "맵툴과 잠 못 이루는 밤" );
	strList.AddTail( "The Next Maptool" );
	*/

	strList.AddTail( ".............................." );
	strList.AddTail( ".............................." );
	strList.AddTail( ".............................." );
	strList.AddTail( ".............................." );
	strList.AddTail( ".....................!........" );
	strList.AddTail( ".............?................" );
	strList.AddTail( "........!!...................." );
	strList.AddTail( ".........................$$..." );
	strList.AddTail( ".......................(--  )." );
	strList.AddTail( ".................2%..........." );
	strList.AddTail( "..(+- -)......................" );
	strList.AddTail( "...기름값 연일 최고치 갱신중.." );
	strList.AddTail( ".........................#...." );

	int		nRand = rand() % strList.GetCount();
	POSITION	pos = strList.GetHeadPosition();
	while( nRand-- ) strList.GetNext( pos );
	m_strMaptoolMessage	= strList.GetAt( pos );

	m_nPopupLevel = m_nRefCount ++;
}

CProgressDlg::~CProgressDlg()
{
	m_nRefCount --;
}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Text(pDX, IDC_PERCENT, m_strPercent);
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_PAINT()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

BOOL	CProgressDlg::StartProgress( CString message , int target , CWnd * pParent )
{
	m_strMessage	= message	;
	m_nTarget		= target	;
	m_nCurrent		= 0			;

	return Create( IDD , pParent );
}

int		CProgressDlg::SetProgress( int progress )
{
	if( ( int ) ( 100.0f * ( float ) progress / ( float ) m_nTarget ) == ( int ) ( 100.0f * ( float ) m_nCurrent / ( float ) m_nTarget ) )
	{
		// 화면 업데이트 안함;;
		// m_nCurrent = progress;
		return m_nCurrent;
	}

	// 마고자 (2004-06-28 오후 8:28:39) : 칼라업데잇
	Render();

	m_nCurrent = progress;

	m_strPercent.Format( "%d%%" , ( int ) ( 100.0f * ( float ) progress / ( float ) m_nTarget ) );

	return m_nCurrent;
}

void		CProgressDlg::EndProgress()
{
	DestroyWindow();
}

BOOL CProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 마고자 (2004-06-28 오후 8:42:17) : 스크롤 렉트 얻기..
	GetDlgItem( IDC_PROGRESS )->GetWindowRect( m_rectProgress );
	ScreenToClient( m_rectProgress );
	GetDlgItem( IDC_PROGRESS )->DestroyWindow();

	CRect	rect;
	GetClientRect( rect );

	CDC * pDC = GetDC();
	m_bmpDC.DeleteObject();
	m_memDC.DeleteDC();
	m_bmpDC.CreateCompatibleBitmap( pDC , rect.Width() , rect.Height() );
	m_memDC.CreateCompatibleDC( pDC );
	m_memDC.SelectObject( m_bmpDC );
	ReleaseDC( pDC );

	SetWindowText( m_strMessage );
	
	GetWindowRect( rect );
	CRect	rectNew = rect;

	rectNew.SetRect(
		0,
		0,
		rect.Width(),
		rect.Height() );

	rectNew.left	+= 100 * ( m_nRefCount );
	rectNew.top		+= rect.Height() * ( m_nRefCount );
	rectNew.right	+= 100 * ( m_nRefCount );
	rectNew.bottom	+= rect.Height() * ( m_nRefCount );
	
	MoveWindow( rectNew );
	//SetWindowPos( &wndTopMost , rect.left , rect.top + m_nRefCount * rect.Height(), rect.Width() , rect.Height() , SWP_DRAWFRAME | SWP_NOMOVE );

	ShowWindow( SW_SHOW );
	Render();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL	CProgressDlg::SetMessage		( CString str )
{
	SetWindowText( str );
	return TRUE;
}

int		CProgressDlg::SetTarget		( int progress )
{
	m_nTarget	= progress;
	return 1;
}

void CProgressDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	Render();
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CProgressDlg::Render( CDC * pDC )
{
	BOOL	bGetDC = FALSE;
	if( NULL == pDC )
	{
		pDC = GetDC();
		bGetDC	= TRUE;
	}

	// 렌더어어어어어~..

	CRect	rect;
	GetClientRect( rect );
	m_memDC.FillRect( rect , &ALEF_BRUSH_BACKGROUND );

	CString	str;
	str.Format( "(%d/%d) 진행중" , m_nCurrent + 1 , m_nTarget );
	m_memDC.SetBkMode( TRANSPARENT );
	m_memDC.SetTextColor( RGB( 255 , 255 , 168 ) );
	m_memDC.TextOut( 5 , 5 , str );

	INT32	nWidth;
	nWidth = ( INT32 ) ( ( FLOAT ) m_rectProgress.Width() * ( FLOAT ) m_nCurrent / ( FLOAT ) m_nTarget );
	m_memDC.FillSolidRect( m_rectProgress.left - 1 , m_rectProgress.top - 1 , m_rectProgress.Width() + 2 , m_rectProgress.Height() + 2 , RGB( 255 , 255 , 255 ) );
	
	m_memDC.FillSolidRect( m_rectProgress.left , m_rectProgress.top , nWidth , m_rectProgress.Height() , m_rgbBackground );

	CRect rectBox = m_rectProgress;
	m_memDC.SetBkMode( TRANSPARENT );
	m_memDC.SetTextColor( RGB( 0 , 0 , 0 ) );
	m_memDC.DrawText( m_strMaptoolMessage , &rectBox , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	rectBox.MoveToX( m_rectProgress.left - 1 );
	rectBox.MoveToY( m_rectProgress.top - 1 );
	m_memDC.SetTextColor( RGB( 255 , 255 , 255 ) );
	m_memDC.DrawText( m_strMaptoolMessage , &rectBox , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	m_memDC.FillSolidRect( m_rectProgress.left + nWidth , m_rectProgress.top , m_rectProgress.Width() - nWidth , m_rectProgress.Height() , RGB_PROGRESSBACKGROUND );

	pDC->BitBlt( 0 , 0 , rect.Width() , rect.Height() , &m_memDC , 0 , 0 , SRCCOPY );

	if( bGetDC )
	{
		ReleaseDC( pDC );
	}
}

void CProgressDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	
	Render();
}
