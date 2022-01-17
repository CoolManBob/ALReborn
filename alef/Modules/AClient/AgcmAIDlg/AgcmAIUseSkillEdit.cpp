// AgcmAIUseSkillEdit.cpp : implementation file
//

#include "stdafx.h"
#include "agcmaidlg.h"
#include "AgcmAIUseSkillEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAIUseSkillEdit dialog


AgcmAIUseSkillEdit::AgcmAIUseSkillEdit(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAIUseSkillEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAIUseSkillEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AgcmAIUseSkillEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAIUseSkillEdit)
	DDX_Control(pDX, IDC_AI_USE_SKILL_RATE_EDIT, m_cRateEdit);
	DDX_Control(pDX, IDC_AI_USE_SKILL_LIST, m_cSkillList);
	DDX_Control(pDX, IDC_AI_USE_SKILL_HP_CON_EDIT, m_cHPConEdit);
	DDX_Control(pDX, IDC_AI_SKILL_USE_COMBO, m_cSkillListCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAIUseSkillEdit, CDialog)
	//{{AFX_MSG_MAP(AgcmAIUseSkillEdit)
	ON_BN_CLICKED(IDC_AI_USE_SKILL_ADD_BUTTON, OnAiUseSkillAddButton)
	ON_BN_CLICKED(IDC_AI_USE_SKILL_OK_BUTTON, OnAiUseSkillOkButton)
	ON_BN_CLICKED(IDC_AI_USE_SKILL_CANCEL_BUTTON, OnAiUseSkillCancelButton)
	ON_BN_CLICKED(IDC_AI_USE_SKILL_REMOVE_BUTTON, OnAiUseSkillRemoveButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAIUseSkillEdit message handlers

BOOL AgcmAIUseSkillEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_cSkillList.InsertColumn( 0, "TID", LVCFMT_LEFT, 50, 0 );
	m_cSkillList.InsertColumn( 1, "Name", LVCFMT_LEFT, 150, 0 );
	m_cSkillList.InsertColumn( 2, "Rate", LVCFMT_LEFT, 50, 1 );

	AgpaSkillTemplate		*pcsAgpaSkillTemplate;
	AgpdSkillTemplate		**ppcsSkillTemplate;
	AgpdSkillTemplate		*pcsSkillTemplate;
	
	char			strUseSkillData[80];
	int				iIndex;
	int				iCounter;

	pcsAgpaSkillTemplate = &m_pcsAgpmSkill->m_csAdminTemplate;
	iIndex = 0;

	while( 1 ) 
	{
		ppcsSkillTemplate = (AgpdSkillTemplate **)pcsAgpaSkillTemplate->GetObjectSequence( &iIndex );

		if( ppcsSkillTemplate == NULL )
		{
			break;
		}
		else
		{
			m_cSkillListCombo.AddString( (*ppcsSkillTemplate)->m_szName );
		}
	}

	itoa( m_pstTemplate->m_stAI.m_csAIUseSkill.m_lHP, strUseSkillData, 10 );
	m_cHPConEdit.SetWindowText( strUseSkillData );

	for( iCounter=0; iCounter<AGPDAI_MAX_USABLE_SKILL_COUNT; iCounter++ )
	{
		if( (m_pstTemplate->m_stAI.m_csAIUseSkill.m_alTID[iCounter] != 0) && (m_pstTemplate->m_stAI.m_csAIUseSkill.m_alRate[iCounter] != 0) )
		{
			pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate( m_pstTemplate->m_stAI.m_csAIUseSkill.m_alTID[iCounter] );

			if( pcsSkillTemplate != NULL )
			{
				itoa( m_pstTemplate->m_stAI.m_csAIUseSkill.m_alTID[iCounter], strUseSkillData, 10 );
				m_cSkillList.InsertItem( 0, strUseSkillData );

				m_cSkillList.SetItemText( 0, 1, pcsSkillTemplate->m_szName );

				itoa( m_pstTemplate->m_stAI.m_csAIUseSkill.m_alRate[iCounter], strUseSkillData, 10 );
				m_cSkillList.SetItemText( 0, 2, strUseSkillData );
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAIUseSkillEdit::OnAiUseSkillAddButton() 
{
	// TODO: Add your control notification handler code here
	char		strSkillName[255];
	char		strRate[80];
	int			iSelCur;

	iSelCur = m_cSkillListCombo.GetCurSel();

	if( iSelCur != CB_ERR )
	{
		if( m_cSkillListCombo.GetLBText( iSelCur, strSkillName ) != CB_ERR )
		{
			if( strlen( strSkillName ) )
			{
				m_cRateEdit.GetWindowText( strRate, sizeof( strRate) );

				if( strlen(strRate) )
				{
					AgpdSkillTemplate	*pcsSkillTempate;
					char				strTID[80];

					pcsSkillTempate = m_pcsAgpmSkill->GetSkillTemplate( strSkillName );
					sprintf( strTID, "%d", pcsSkillTempate->m_lID );

					m_cSkillList.InsertItem( 0, strTID );
					m_cSkillList.SetItemText( 0, 1, strSkillName );
					m_cSkillList.SetItemText( 0, 2, strRate );
				}
			}
		}
	}
}

void AgcmAIUseSkillEdit::OnAiUseSkillRemoveButton() 
{
	// TODO: Add your control notification handler code here
	int			iSelect;

	iSelect = m_cSkillList.GetSelectionMark();
	m_cSkillList.DeleteItem( iSelect );	
}

void AgcmAIUseSkillEdit::OnAiUseSkillOkButton() 
{
	// TODO: Add your control notification handler code here
	char			strHP[80];

	m_cHPConEdit.GetWindowText( strHP, sizeof(strHP) );

	if( strlen( strHP ) != 0 )
	{
		char		strTID[80];
		char		strRate[80];

		int			lCounter;
		int			lCurrentIndex;
		int			lNextIndex;

		lCurrentIndex = 0;

		m_pstTemplate->m_stAI.m_csAIUseSkill.m_lHP = atoi( strHP );

		while( 1 )
		{
			lNextIndex = m_cSkillList.GetNextItem( lCurrentIndex, LVNI_ALL );

			if( lCurrentIndex == -1 )
				break;

			m_cSkillList.GetItemText( lCurrentIndex, 0, strTID, sizeof(strTID) );
//			m_cSkillList.GetItemText( lCurrentIndex, 1, strName, sizeof(strName) );
			m_cSkillList.GetItemText( lCurrentIndex, 2, strRate, sizeof(strRate) );

			for( lCounter=0; lCounter<AGPDAI_MAX_USABLE_SKILL_COUNT; lCounter++ )
			{
				if( m_pstTemplate->m_stAI.m_csAIUseSkill.m_alTID[lCounter] == 0 )
				{
					break;
				}
			}

			m_pstTemplate->m_stAI.m_csAIUseSkill.m_alTID[lCounter] = atoi( strTID );
			m_pstTemplate->m_stAI.m_csAIUseSkill.m_alRate[lCounter] = atoi(strRate );

			lCurrentIndex = lNextIndex;
		}
	}

	OnOK();	
}

void AgcmAIUseSkillEdit::OnAiUseSkillCancelButton() 
{
	// TODO: Add your control notification handler code here
	OnCancel();	
}
