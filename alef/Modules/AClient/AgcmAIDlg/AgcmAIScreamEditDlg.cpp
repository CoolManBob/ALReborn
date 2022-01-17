// AgcmAIScreamEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmaidlg.h"
#include "AgcmAIScreamEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAIScreamEditDlg dialog


AgcmAIScreamEditDlg::AgcmAIScreamEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAIScreamEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAIScreamEditDlg)
	m_cHelpScreamAllCharTypeCheck = FALSE;
	//}}AFX_DATA_INIT
}


void AgcmAIScreamEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAIScreamEditDlg)
	DDX_Control(pDX, IDC_AI_HELP_SCREAM_CHARTYPE_COMBO, m_cHelpScreamCharTypeCombo);
	DDX_Control(pDX, IDC_AI_HELP_SCREAM_CHARTYLE_LIST, m_cHelpScreamCharTypeList);
	DDX_Control(pDX, IDC_AI_HELPSCREAM_EDIT3, m_cHelpScreamEdit3);
	DDX_Control(pDX, IDC_AI_HELPSCREAM_EDIT2, m_cHelpScreamEdit2);
	DDX_Control(pDX, IDC_AI_HELPSCREAM_EDIT1, m_cHelpScreamEdit1);
	DDX_Control(pDX, IDC_AI_FEARSCREAM_EDIT3, m_cFearScreamEdit3);
	DDX_Control(pDX, IDC_AI_FEARSCREAM_EDIT2, m_cFearScreamEdit2);
	DDX_Control(pDX, IDC_AI_FEARSCREAM_EDIT1, m_cFearScreamEdit1);
	DDX_Check(pDX, IDC_AI_HELP_SCREAM_ALL_CHAR_TYPE_CHECK, m_cHelpScreamAllCharTypeCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAIScreamEditDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmAIScreamEditDlg)
	ON_BN_CLICKED(IDC_AI_HELP_SCREAM_CHAR_ADD_BUTTON, OnAiHelpScreamCharAddButton)
	ON_BN_CLICKED(IDC_AI_HELP_SCREAM_CHAR_REMOVE_BUTTON, OnAiHelpScreamCharRemoveButton)
	ON_BN_CLICKED(IDC_AI_HELP_SCREAM_ALL_CHAR_TYPE_CHECK, OnAiHelpScreamAllCharTypeCheck)
	ON_BN_CLICKED(IDC_AI_SCREAM_OK_BUTTON, OnAiScreamOkButton)
	ON_BN_CLICKED(IDC_AI_SCREAM_CANCEL_BUTTON, OnAiScreamCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAIScreamEditDlg message handlers

BOOL AgcmAIScreamEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	AgpaCharacterTemplate	*pcsAgpaCharacterTemplate;
	AgpdCharacterTemplate	**ppcsCharacterTemplate;
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate;

	char			strScreamData[80];
	int				iIndex;
	int				iCharTypeTID;

	pcsAgpaCharacterTemplate = &m_pcsAgpmCharacter->m_csACharacterTemplate;
	iIndex = 0;

	while( 1 ) 
	{
		ppcsCharacterTemplate = (AgpdCharacterTemplate **)pcsAgpaCharacterTemplate->GetObjectSequence( &iIndex );

		if( ppcsCharacterTemplate == NULL )
		{
			break;
		}
		else
		{
			m_cHelpScreamCharTypeCombo.AddString( (*ppcsCharacterTemplate)->m_szTName );
		}
	}

	for( int i=0; i<AGPDAI_MAX_HELP_CHARTYPE_COUNT; i++ )
	{
		iCharTypeTID = m_pstTemplate->m_stAI.m_csAIScream.m_alHelpCharTID[i];

		pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( iCharTypeTID );		
	}

	itoa( m_pstTemplate->m_stAI.m_csAIScream.m_lFearLV1Agro, strScreamData, 10 );
	m_cFearScreamEdit1.SetWindowText( strScreamData );

	itoa( m_pstTemplate->m_stAI.m_csAIScream.m_lFearLV2HP, strScreamData, 10 );
	m_cFearScreamEdit2.SetWindowText( strScreamData );

	itoa( m_pstTemplate->m_stAI.m_csAIScream.m_bFearLV3, strScreamData, 10 );
	m_cFearScreamEdit3.SetWindowText( strScreamData );
	
	itoa( m_pstTemplate->m_stAI.m_csAIScream.m_lHelpLV1CopyAgro, strScreamData, 10 );
	m_cHelpScreamEdit1.SetWindowText( strScreamData );

	itoa( m_pstTemplate->m_stAI.m_csAIScream.m_lHelpLV2Heal, strScreamData, 10 );
	m_cHelpScreamEdit2.SetWindowText( strScreamData );

	itoa( m_pstTemplate->m_stAI.m_csAIScream.m_lHelpLV3, strScreamData, 10 );
	m_cHelpScreamEdit3.SetWindowText( strScreamData );

	m_cHelpScreamAllCharTypeCheck = m_pstTemplate->m_stAI.m_csAIScream.m_bHelpAll;

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAIScreamEditDlg::OnAiHelpScreamCharAddButton() 
{
	// TODO: Add your control notification handler code here
	char		strCharName[255];
	int			iSelCur;

	iSelCur = m_cHelpScreamCharTypeCombo.GetCurSel();

	if( iSelCur != CB_ERR )
	{
		if( m_cHelpScreamCharTypeCombo.GetLBText( iSelCur, strCharName ) != CB_ERR )
		{
			if( strlen( strCharName ) )
			{
				m_cHelpScreamCharTypeList.AddString( strCharName );
			}
		}
	}	
}

void AgcmAIScreamEditDlg::OnAiHelpScreamCharRemoveButton() 
{
	// TODO: Add your control notification handler code here
	int			iSelect;

	iSelect = m_cHelpScreamCharTypeList.GetCurSel();

	if( iSelect != LB_ERR )
	{
		m_cHelpScreamCharTypeList.DeleteString( iSelect );
	}
}

void AgcmAIScreamEditDlg::OnAiHelpScreamAllCharTypeCheck() 
{
	// TODO: Add your control notification handler code here
	m_cHelpScreamAllCharTypeCheck = 1-m_cHelpScreamAllCharTypeCheck;

	if( m_cHelpScreamAllCharTypeCheck )
	{
		m_cHelpScreamCharTypeCombo.EnableWindow( FALSE );
		m_cHelpScreamCharTypeList.EnableWindow( FALSE );
	}
	else
	{
		m_cHelpScreamCharTypeCombo.EnableWindow( TRUE );
		m_cHelpScreamCharTypeList.EnableWindow( TRUE );
	}
}

void AgcmAIScreamEditDlg::OnAiScreamOkButton() 
{
	// TODO: Add your control notification handler code here
	char		strCharTypeName[255];
	int			lCharTypeCount;
	int			lCounter;
	bool		bResult;

	bResult = true;

	//정확히 입력이 되었는지 확인한다.
	lCharTypeCount = m_cHelpScreamCharTypeList.GetCount();

	if( lCharTypeCount == 0 )
		bResult = false;

	if( m_cFearScreamEdit1.GetWindowTextLength() == 0 )
		bResult = false;

	if( m_cFearScreamEdit1.GetWindowTextLength() == 0 )
		bResult = false;

	if( m_cFearScreamEdit1.GetWindowTextLength() == 0 )
		bResult = false;

	if( m_cHelpScreamEdit1.GetWindowTextLength() == 0 )
		bResult = false;

	if( m_cHelpScreamEdit2.GetWindowTextLength() == 0 )
		bResult = false;

	if( m_cHelpScreamEdit3.GetWindowTextLength() == 0 )
		bResult = false;

	//다 입력된게 확인되면?
	if( bResult == true )
	{
		AgpdCharacterTemplate		*pcsAgpdCharacterTempalte;

		char			strScream[80];

		m_pstTemplate->m_stAI.m_csAIScream.m_bUseScream = true;

		m_cFearScreamEdit1.GetWindowText( strScream, sizeof( strScream ) );
		m_pstTemplate->m_stAI.m_csAIScream.m_lFearLV1Agro = atoi( strScream );

		m_cFearScreamEdit2.GetWindowText( strScream, sizeof( strScream ) );
		m_pstTemplate->m_stAI.m_csAIScream.m_lFearLV2HP = atoi( strScream );

		m_cFearScreamEdit3.GetWindowText( strScream, sizeof( strScream ) );
		if( atoi( strScream ) )
		{
			m_pstTemplate->m_stAI.m_csAIScream.m_bFearLV3 = true;
		}
		else
		{
			m_pstTemplate->m_stAI.m_csAIScream.m_bFearLV3 = false;
		}

		m_cHelpScreamEdit1.GetWindowText( strScream, sizeof( strScream ) );
		m_pstTemplate->m_stAI.m_csAIScream.m_lHelpLV1CopyAgro = atoi( strScream );

		m_cHelpScreamEdit2.GetWindowText( strScream, sizeof( strScream ) );
		m_pstTemplate->m_stAI.m_csAIScream.m_lHelpLV2Heal = atoi( strScream );

		m_cHelpScreamEdit3.GetWindowText( strScream, sizeof( strScream ) );
		m_pstTemplate->m_stAI.m_csAIScream.m_lHelpLV3 = atoi( strScream );

		for( lCounter=0; lCounter<lCharTypeCount; lCounter++ )
		{
			m_cHelpScreamCharTypeList.GetText( lCounter, strCharTypeName );

			pcsAgpdCharacterTempalte = m_pcsAgpmCharacter->GetCharacterTemplate( strCharTypeName );
			m_pstTemplate->m_stAI.m_csAIScream.m_alHelpCharTID[lCounter] = pcsAgpdCharacterTempalte->m_lID;
		}
	}
	
	OnOK();
}

void AgcmAIScreamEditDlg::OnAiScreamCancelButton() 
{
	// TODO: Add your control notification handler code here
	OnCancel();	
}
