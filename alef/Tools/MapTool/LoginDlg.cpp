// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "LoginDlg.h"
#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog
char	g_szTIDKind[][20] = { "OrcFriarMale", "OrcWarriorMale", "HumanMageMale", "HumanMonkMale", "HumanRangerMale", "HumanKnightMale",
							  "GiantFireBat", "GiantOgre", "GiantSilverMoth", "Goblin", "Gorgon", "Lamia", "Troll",
							  "HumanRangerFemale", "HumanKnightMale2", "HumanKnightFemale", "HumanMageFemale", "HumanMonkFemale",
							  "OrcHunterFemale", "OrcWarriorFemale",  };


INT32	g_nPrevSelectCharacter	= 0;

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	//}}AFX_DATA_INIT
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Control(pDX, IDC_COMBO1, m_comboCharacter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

BOOL CLoginDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	INT32						lIndex		= 0;
	AgpdCharacterTemplate	*	pTemplate	;
	char						str[ 256 ]	;
	for (	pTemplate	= g_pcsAgpmCharacter->GetTemplateSequence(&lIndex);
			pTemplate	;
			pTemplate	= g_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		wsprintf( str , "%04d, %s" , pTemplate->m_lID , pTemplate->m_szTName );
		m_comboCharacter.AddString( str );
	}
	
	m_comboCharacter.SetCurSel( g_nPrevSelectCharacter );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoginDlg::OnOK() 
{
	//char szGetComboTID[20];
	
	m_nID = 0 ;
	//TID입력 검색 
	//GetDlgItemText( IDC_COMBO1, szGetComboTID, 30 );

	INT32	nSel = m_comboCharacter.GetCurSel();
	CString	strText;
	m_comboCharacter.GetLBText( nSel , strText );

	m_nID = atoi( ( LPCTSTR ) strText );
		
//	for ( int i = 0 ; i < 20 ; i++ )
//			if ( strcmp( g_szTIDKind[i] , szGetComboTID ) == 0 )
//			{
//				m_nID = i+1;
//				if ( strstr( g_szTIDKind[i] , "Orc" ) || strstr( g_szTIDKind[i] , "Human" ) )
//					m_bPlayer = TRUE;
//				else
//					m_bPlayer = FALSE;
//			}
	
	if ( m_nID == 0 ) 
	{
		MessageBox( "TID가 올바르지 않습니다.", "AlefClient", MB_OK );
		OnCancel();
	}

	// ID 저장해둠.
	g_nPrevSelectCharacter = m_comboCharacter.GetCurSel()	;
	
	CDialog::OnOK();
}
