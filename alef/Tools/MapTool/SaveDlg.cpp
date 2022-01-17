// SaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "SaveDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveDlg dialog


CSaveDlg::CSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveDlg)
	m_bMapInfo			= TRUE;
	m_bObjectList		= TRUE;
	m_bObjectTemplate	= TRUE;
	m_bTileList			= TRUE;
	m_bTileVertexColor	= FALSE;
	m_bDungeonData		= FALSE;
	//}}AFX_DATA_INIT
}


void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveDlg)
	DDX_Check(pDX, IDC_MAPINFO, m_bMapInfo);
	DDX_Check(pDX, IDC_OBJECTLIST, m_bObjectList);
	DDX_Check(pDX, IDC_OBJECTTEMPLATE, m_bObjectTemplate);
	DDX_Check(pDX, IDC_TILELIST, m_bTileList);
	DDX_Check(pDX, IDC_TILEVERTEXCOLOR, m_bTileVertexColor);
	DDX_Check(pDX, IDC_DUNGEON, m_bDungeonData );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveDlg, CDialog)
	//{{AFX_MSG_MAP(CSaveDlg)
	ON_BN_CLICKED(IDC_NOPE, OnNo)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveDlg message handlers

void CSaveDlg::OnNo() 
{
	// TODO: Add your control notification handler code here
	EndDialog( IDNO );
}

BOOL CSaveDlg::AddPluginSaveData( BOOL bSaveDefault , char * pStr , CUITileList_PluginBase * pPlugin)
{
	ASSERT( NULL != pPlugin );

	PluginSave	savedata;
	savedata.bSave		= bSaveDefault	;
	savedata.strName	= pStr			;
	savedata.pPlugin	= pPlugin		;
	savedata.pButton	= new CButton	;

	m_listPlugin.AddTail( savedata );
	return TRUE;
}

BOOL CSaveDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect	rect;
	GetDlgItem( IDC_PLUGIN_SAVE )->GetWindowRect( rect );
	ScreenToClient( rect );

	// 플러그인 체크박스 추가..
	AuNode< PluginSave > * pNode = m_listPlugin.GetHeadNode();
	PluginSave * pSave;

	int nCount = 0 ;
	while( pNode )
	{
		pSave = & pNode->GetData();

		pSave->pButton->Create(
			pSave->strName				,
			WS_VISIBLE | BS_CHECKBOX	,
			CRect( rect.left , rect.top + 20 * nCount , 200 , rect.top + 20 * ( 1 + nCount ) ),
			this						,
			24521 + nCount				);

		pSave->pButton->SetCheck( pSave->bSave );
		m_listPlugin.GetNext( pNode );

		nCount++;
	}
	
	// 마고자 (2003-10-28 오후 3:54:30) : 강제로 템플릿 세이브 폴스시킴.
	m_bObjectTemplate = FALSE;
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSaveDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// 플러그인 체크박스 추가..
	AuNode< PluginSave > * pNode = m_listPlugin.GetHeadNode();
	PluginSave * pSave;

	int nCount = 0 ;
	while( pNode )
	{
		pSave = & pNode->GetData();

		pSave->bSave = pSave->pButton->GetCheck();
		pSave->pButton->DestroyWindow();
		delete pSave->pButton;
		m_listPlugin.GetNext( pNode );
	}
}

LRESULT CSaveDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( message )
	{
	case WM_COMMAND:
		{
			DWORD	wNotifyCode	= HIWORD(wParam); // notification code 
			DWORD	wID			= LOWORD(wParam); // item, control, or accelerator identifier 
			HWND	hwndCtl		= (HWND) lParam	; // handle of control 

			AuNode< PluginSave > * pNode = m_listPlugin.GetHeadNode();
			PluginSave * pSave;

			BOOL	bRet;
			while( pNode )
			{
				pSave = & pNode->GetData();

				if( hwndCtl == pSave->pButton->GetSafeHwnd() )
				{
					bRet = pSave->pButton->GetCheck();
					pSave->pButton->SetCheck( !bRet );
					break;
				}
				m_listPlugin.GetNext( pNode );
			}
		}
		break;
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}
