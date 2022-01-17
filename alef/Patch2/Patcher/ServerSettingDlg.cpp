// ServerSettingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "ServerSettingDlg.h"
#include "ServerEditDlg.h"

#include <fstream>

using namespace std;


// CServerSettingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerSettingDlg, CDialog)

CServerSettingDlg::CServerSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerSettingDlg::IDD, pParent)
{

}

CServerSettingDlg::~CServerSettingDlg()
{
}

void CServerSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CServerSettingDlg, CDialog)
	ON_BN_CLICKED(ID_OK_BUTTON, &CServerSettingDlg::OnBnClickedOkButton)
	ON_COMMAND(ID_MENU_ADDSERVER, &CServerSettingDlg::OnMenuAddserver)
	ON_COMMAND(ID_MENU_EDITSERVER, &CServerSettingDlg::OnMenuEditserver)
	ON_COMMAND(ID_MENU_DELETESERVER, &CServerSettingDlg::OnMenuDeleteserver)
END_MESSAGE_MAP()

// CServerSettingDlg 메시지 처리기입니다.

BOOL CServerSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ListCtrl.InsertColumn(0, "Server Name", LVCFMT_CENTER, 180);
	m_ListCtrl.InsertColumn(1, "IP Address", LVCFMT_CENTER, 230);
	
	RefreshList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

VOID CServerSettingDlg::RefreshList( VOID )
{
	m_ListCtrl.DeleteAllItems();

	fstream		FileStream;
	CHAR		szTemp[	256 ];

	FileStream.open( SERVER_FILE_NAME , ios_base::in );
	if( !FileStream )	return;

	for( INT i = 0 ;  ; ++i )
	{
		// Server Name
		FileStream.getline( szTemp , 256 );
		if( szTemp[0] == 0 )	break;

		m_ListCtrl.InsertItem( i, szTemp );

		// IP Address
		FileStream.getline( szTemp , 256 );
		m_ListCtrl.SetItemText( i, 1, szTemp );
		
		// 공백
		FileStream.getline( szTemp , 256 );

		ZeroMemory( szTemp , 256 );
	}

	if( FileStream )	FileStream.close();
}


void CServerSettingDlg::OnBnClickedOkButton()
{
	fstream			FileStream;

	FileStream.open( SERVER_FILE_NAME , ios_base::out );

	if( !FileStream ) return;

	INT ItemCnt = m_ListCtrl.GetItemCount();

	for( INT i = 0 ; i < ItemCnt ; ++i )
	{
		FileStream << m_ListCtrl.GetItemText( i , 0 ) << endl;
		FileStream << m_ListCtrl.GetItemText( i , 1 ) << endl;
		FileStream << endl;
	}

	FileStream.close();

	EndDialog( 1 );
}

void CServerSettingDlg::OnMenuAddserver()
{
	CServerEditDlg	dlg;

	if( dlg.DoModal() == IDOK )
	{
		INT Index	= m_ListCtrl.GetItemCount();

		m_ListCtrl.InsertItem( Index , dlg.GetName() );
		m_ListCtrl.SetItemText( Index , 1 , dlg.GetIP() );
	}

}

void CServerSettingDlg::OnMenuEditserver()
{
	POSITION		Pos;
	int				nSelItem;
	CServerEditDlg	dlg;

	Pos			= m_ListCtrl.GetFirstSelectedItemPosition();
	nSelItem	= m_ListCtrl.GetNextSelectedItem( Pos );
	
	dlg.SetName( m_ListCtrl.GetItemText( nSelItem , 0 ) );
	dlg.SetIP( m_ListCtrl.GetItemText( nSelItem , 1 ) );


	if( dlg.DoModal() == IDOK )
	{
		m_ListCtrl.SetItemText( nSelItem , 0 , dlg.GetName() );
		m_ListCtrl.SetItemText( nSelItem , 1 , dlg.GetIP() );
	}

}

void CServerSettingDlg::OnMenuDeleteserver()
{
	// 리스트컨트롤에 선택된 아이템을 삭제

	POSITION	Pos;
	int			nSelItem;

	Pos			= m_ListCtrl.GetFirstSelectedItemPosition();
	nSelItem	= m_ListCtrl.GetNextSelectedItem( Pos );
		
	m_ListCtrl.DeleteItem( nSelItem );

}