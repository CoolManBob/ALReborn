// EditConsoleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "EditConsoleDlg.h"
#include "Mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditConsoleDlg dialog

extern CMainFrame *		g_pMainFrame;

CStringList CEditConsoleDlg::m_sCommandHistory;

CEditConsoleDlg::CEditConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditConsoleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditConsoleDlg)
	m_strCommand = _T("");
	//}}AFX_DATA_INIT

	m_nCommandType	= 0;//NOCOMMAND;
}


void CEditConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditConsoleDlg)
	DDX_Text(pDX, IDC_COMMAND, m_strCommand);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditConsoleDlg, CDialog)
	//{{AFX_MSG_MAP(CEditConsoleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditConsoleDlg message handlers

void CEditConsoleDlg::OnOK() 
{
	UpdateData( TRUE );

	m_strCommand.MakeLower();

	m_Argument.SetParam( (LPSTR)(LPCTSTR) m_strCommand );
	if( m_Argument.GetArgCount() > 0 )
	{
		// 커맨드 체크..
		char	strCommand[ 256 ];
		m_Argument.GetParam( 0 , strCommand );
		m_nCommandType = GetCommandIndex( strCommand );

		// 마고자 (2004-06-10 오전 11:29:42) : 커맨드가 있는것만 캐싱.
		m_sCommandHistory.AddTail( m_strCommand );
		if( m_sCommandHistory.GetCount() > 10 )
			m_sCommandHistory.RemoveHead();
	}
	else
	{
		m_nCommandType = 0 ; //_NOCOMMAND		;
	}
	
	CDialog::OnOK();
}

BOOL CEditConsoleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect	rect;
	GetWindowRect( rect );
	
	CRect	rectNew;
	rectNew.left	= m_Pos.x;
	rectNew.top		= m_Pos.y - rect.Height	();
	rectNew.right	= m_Pos.x + rect.Width	();
	rectNew.bottom	= m_Pos.y;

	MoveWindow( rectNew );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int	CEditConsoleDlg::GetCommandIndex( CString str )
{
	AuNode< CConsoleCommand > * pNode = g_pMainFrame->m_listCommand.GetHeadNode();
	CConsoleCommand * pCommand;
	while( pNode )
	{
		pCommand = &pNode->GetData();

		// 중복하는것만 검사..

		if	( !pCommand->strAlias[ 0 ].IsEmpty() && !strncmp( str , pCommand->strAlias[ 0 ] , 256 ) ) break;
		if	( !pCommand->strAlias[ 1 ].IsEmpty() && !strncmp( str , pCommand->strAlias[ 1 ] , 256 ) ) break;
		if	( !pCommand->strAlias[ 2 ].IsEmpty() && !strncmp( str , pCommand->strAlias[ 2 ] , 256 ) ) break;

		g_pMainFrame->m_listCommand.GetNext( pNode );
	}

	if( NULL == pNode )
	{
		return 0 ; //_NOCOMMAND;
	}
	else
	{
		return pCommand->nCommandIndex;
	}

}

BOOL CEditConsoleDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if( pMsg->message == WM_KEYDOWN )
	{
		switch( pMsg->wParam )
		{
		case VK_UP		:
			{
				UpdateData( TRUE );
			
				POSITION pos = m_sCommandHistory.Find( m_strCommand );

				if( NULL != pos )
				{
					m_sCommandHistory.GetPrev( pos );
				}

				if( NULL == pos )
				{
					if( m_sCommandHistory.GetCount() )
						m_strCommand = m_sCommandHistory.GetTail();
				}
				else
				{
					m_strCommand = m_sCommandHistory.GetAt( pos );
				}

				UpdateData( FALSE );
			}
			break;
		case VK_DOWN	:
			{
				UpdateData( TRUE );
			
				POSITION pos = m_sCommandHistory.Find( m_strCommand );

				if( NULL != pos )
				{
					m_sCommandHistory.GetNext( pos );
				}

				if( NULL == pos )
				{
					if( m_sCommandHistory.GetCount() )
						m_strCommand = m_sCommandHistory.GetHead();
				}
				else
				{
					m_strCommand = m_sCommandHistory.GetAt( pos );
				}

				UpdateData( FALSE );
			}
			break;
		default:
			break;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
