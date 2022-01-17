#include "stdafx.h"

#include "../resource.h"

#include "agcmai2dlg.h"
#include "AgcmAI2Dialog.h"

#include "AgcmAI2EditTableDlg.h"

AgcmAI2EditTableDlg		g_csAgcmAI2EdtTableDlg;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AgcmAI2Dialog::AgcmAI2Dialog(CWnd* pParent /*=NULL*/) : CDialog(AgcmAI2Dialog::IDD, pParent)
{
}


void AgcmAI2Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAI2Dialog)
	DDX_Control(pDX, IDC_Check_FixedNPC, m_cFixedNPCCheckButton);
	DDX_Control(pDX, IDC_Check_NPC, m_cNPCCheckButton);
	DDX_Control(pDX, IDC_AI2_AggressiveEdit, m_cAggressiveEdit);
	DDX_Control(pDX, IDC_AI2_UseSkillList, m_cUseSkillList);
	DDX_Control(pDX, IDC_AI2_UseItemList, m_cUseItemList);
	DDX_Control(pDX, IDC_AI2_AdjustWarriorEdit, m_cAdjustWarriorEdit);
	DDX_Control(pDX, IDC_AI2_AdjustTryDebuffEdit, m_cAdjustTryDebuffEdit);
	DDX_Control(pDX, IDC_AI2_AdjustTryAttackEdit, m_cAdjustTryAttackEdit);
	DDX_Control(pDX, IDC_AI2_AdjustMonkEdit, m_cAdjustMonkEdit);
	DDX_Control(pDX, IDC_AI2_AdjustMageEdit, m_cAdjustMageEdit);
	DDX_Control(pDX, IDC_AI2_AdjustHealEdit, m_cAdjustHealEdit);
	DDX_Control(pDX, IDC_AI2_AdjustDemageEdit, m_cAdjustDamageEdit);
	DDX_Control(pDX, IDC_AI2_AdjustDebuffEdit, m_cAdjustDebuffEdit);
	DDX_Control(pDX, IDC_AI2_AdjustBuffEdit, m_cAdjustBuffEdit);
	DDX_Control(pDX, IDC_AI2_AdjustArcherEdit, m_cAdjustArcherEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAI2Dialog, CDialog)
	//{{AFX_MSG_MAP(AgcmAI2Dialog)
	ON_BN_CLICKED(IDC_AI2_AddUseItemButton, OnAI2AddUseItemButton)
	ON_BN_CLICKED(IDC_AI2_UpdateUseItemButton, OnAI2UpdateUseItemButton)
	ON_BN_CLICKED(IDC_AI2_DeleteUseItemButton, OnAI2DeleteUseItemButton)
	ON_BN_CLICKED(IDC_AI2_AddUseSkillButton, OnAI2AddUseSkillButton)
	ON_BN_CLICKED(IDC_AI2_UpdateUseSkillButton, OnAI2UpdateUseSkillButton)
	ON_BN_CLICKED(IDC_AI2_DeleteUseSkillButton, OnAI2DeleteUseSkillButton)
	ON_BN_CLICKED(IDC_AI2_AdjustOKButton, OnAI2AdjustOKButton)
	ON_BN_CLICKED(IDC_AI2_AdjustCancelButton, OnAI2AdjustCancelButton)
	ON_BN_CLICKED(IDC_Check_NPC, OnCheckNPC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAI2Dialog message handlers
void AgcmAI2Dialog::ClearControl()
{
	m_cUseItemList.ResetContent();
	m_cUseSkillList.ResetContent();

	m_cAdjustWarriorEdit.SetWindowText( "" );
	m_cAdjustArcherEdit.SetWindowText( "" );
	m_cAdjustMageEdit.SetWindowText( "" );
	m_cAdjustMonkEdit.SetWindowText( "" );
	
	m_cAdjustDamageEdit.SetWindowText( "" );
	m_cAdjustTryAttackEdit.SetWindowText( "" );
	m_cAdjustDebuffEdit.SetWindowText( "" );
	m_cAdjustTryDebuffEdit.SetWindowText( "" );
	m_cAdjustHealEdit.SetWindowText( "" );
	m_cAdjustBuffEdit.SetWindowText( "" );
}

void AgcmAI2Dialog::ActiveMobAI()
{
	CWnd			*pcsWnd;

	pcsWnd = GetDlgItem(IDC_AI2_AdjustWarriorEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AdjustArcherEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AdjustMageEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AdjustMonkEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AggressiveEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AdjustDemageEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AdjustTryAttackEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AdjustDebuffEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AdjustTryDebuffEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AdjustHealEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AdjustBuffEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_UseItemList);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AddUseItemButton);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_UpdateUseItemButton);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_DeleteUseItemButton);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_UseSkillList);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_AddUseSkillButton);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_UpdateUseSkillButton);
	if( pcsWnd )
		pcsWnd->EnableWindow();

	pcsWnd = GetDlgItem(IDC_AI2_DeleteUseSkillButton);
	if( pcsWnd )
		pcsWnd->EnableWindow();
}

void AgcmAI2Dialog::DeactiveMobAI()
{
	CWnd			*pcsWnd;

	pcsWnd = GetDlgItem(IDC_AI2_AdjustWarriorEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AdjustArcherEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AdjustMageEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AdjustMonkEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AggressiveEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AdjustDemageEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AdjustTryAttackEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AdjustDebuffEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AdjustTryDebuffEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AdjustHealEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AdjustBuffEdit);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_UseItemList);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AddUseItemButton);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_UpdateUseItemButton);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_DeleteUseItemButton);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_UseSkillList);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_AddUseSkillButton);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_UpdateUseSkillButton);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);

	pcsWnd = GetDlgItem(IDC_AI2_DeleteUseSkillButton);
	if( pcsWnd )
		pcsWnd->EnableWindow(FALSE);
}

void AgcmAI2Dialog::FillControlByTemplate( AgpdAI2Template *pcsTemplate )
{
	if( !pcsTemplate )	return;
	
	if( pcsTemplate->m_eNPCAI2Type == AGPMAI2_TYPE_FIXED_NPC )
	{
		m_cNPCCheckButton.SetCheck(1);
		m_cFixedNPCCheckButton.SetCheck(1);

		DeactiveMobAI();
	}
	else if( pcsTemplate->m_eNPCAI2Type == AGPMAI2_TYPE_PATROL_NPC )
	{
		m_cNPCCheckButton.SetCheck(1);

		DeactiveMobAI();
	}

	char	strBuffer[80];
	itoa( pcsTemplate->m_csClassAdjust.m_lWarrior, strBuffer, 10 );
	m_cAdjustWarriorEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csClassAdjust.m_lArcher, strBuffer, 10 );
	m_cAdjustArcherEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csClassAdjust.m_lMage, strBuffer, 10 );
	m_cAdjustMageEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csClassAdjust.m_lMonk, strBuffer, 10 );
	m_cAdjustMonkEdit.SetWindowText( strBuffer );

	itoa( pcsTemplate->m_lAgressivePoint, strBuffer, 10 );
	m_cAggressiveEdit.SetWindowText( strBuffer );

	itoa( pcsTemplate->m_csAgroActionAdjust.m_lDamage, strBuffer, 10 );
	m_cAdjustDamageEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lTryAttack, strBuffer, 10 );
	m_cAdjustTryAttackEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lDebuff, strBuffer, 10 );
	m_cAdjustDebuffEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lTryDebuff, strBuffer, 10 );
	m_cAdjustTryDebuffEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lP2PHeal, strBuffer, 10 );
	m_cAdjustHealEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lP2PBuff, strBuffer, 10 );
	m_cAdjustBuffEdit.SetWindowText( strBuffer );

	for( int lCounter=0; lCounter<AGPDAI2_MAX_USABLE_ITEM_COUNT; lCounter++ )
	{
		if( pcsTemplate->m_csUseItem[lCounter].m_lItemID != 0 )
		{
			AgpdItemTemplate* pcsAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate( m_pcsAgpdAI2Template->m_csUseItem[0].m_lItemID );
			if( !pcsAgpdItemTemplate )	continue;

			m_cUseItemList.SetItemData( m_cUseItemList.AddString( pcsAgpdItemTemplate->m_szName ), lCounter );
		}
	}

	for( int lCounter=0; lCounter<AGPDAI2_MAX_USABLE_ITEM_COUNT; lCounter++ )
	{
		if( pcsTemplate->m_csUseSkill[lCounter].m_lSkillID != 0 )
		{
			AgpdSkillTemplate* pcsAgpdSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate( m_pcsAgpdAI2Template->m_csUseSkill[0].m_lSkillID );
			if( !pcsAgpdSkillTemplate  )	continue;

			m_cUseSkillList.SetItemData( m_cUseSkillList.AddString( pcsAgpdSkillTemplate->m_szName ), lCounter );
		}
	}
}

void AgcmAI2Dialog::GetTemplateDataFromContrl( AgpdAI2Template *pcsTemplate )
{
	if( !pcsTemplate )	return;

	if( m_cNPCCheckButton.GetCheck() == 1 )
	{
		pcsTemplate->m_eNPCAI2Type = AGPMAI2_TYPE_PATROL_NPC;

		if( m_cFixedNPCCheckButton.GetCheck() == 1 )
		{
			pcsTemplate->m_eNPCAI2Type = AGPMAI2_TYPE_FIXED_NPC;
		}
	}
	else if( m_cNPCCheckButton.GetCheck() == 0 )
	{
		pcsTemplate->m_eNPCAI2Type = AGPMAI2_TYPE_MOB;
	}

	char	strBuffer[80];
	itoa( pcsTemplate->m_csClassAdjust.m_lWarrior, strBuffer, 10 );
	m_cAdjustWarriorEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csClassAdjust.m_lArcher, strBuffer, 10 );
	m_cAdjustArcherEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csClassAdjust.m_lMage, strBuffer, 10 );
	m_cAdjustMageEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csClassAdjust.m_lMonk, strBuffer, 10 );
	m_cAdjustMonkEdit.SetWindowText( strBuffer );

	itoa( pcsTemplate->m_lAgressivePoint, strBuffer, 10 );
	m_cAggressiveEdit.SetWindowText( strBuffer );
	
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lDamage, strBuffer, 10 );
	m_cAdjustDamageEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lTryAttack, strBuffer, 10 );
	m_cAdjustTryAttackEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lDebuff, strBuffer, 10 );
	m_cAdjustDebuffEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lTryDebuff, strBuffer, 10 );
	m_cAdjustTryDebuffEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lP2PHeal, strBuffer, 10 );
	m_cAdjustHealEdit.SetWindowText( strBuffer );
	itoa( pcsTemplate->m_csAgroActionAdjust.m_lP2PBuff, strBuffer, 10 );
	m_cAdjustBuffEdit.SetWindowText( strBuffer );

	for( INT32 lCounter=0; lCounter<AGPDAI2_MAX_USABLE_ITEM_COUNT; lCounter++ )
	{
		if( pcsTemplate->m_csUseItem[lCounter].m_lItemID != 0 )
		{
			AgpdItemTemplate* pcsAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate( pcsTemplate->m_csUseItem[lCounter].m_lItemID );
			if( !pcsAgpdItemTemplate )	continue;

			m_cUseItemList.SetItemData( m_cUseItemList.AddString( pcsAgpdItemTemplate->m_szName ), lCounter );
		}
	}

	for( INT32 lCounter=0; lCounter<AGPDAI2_MAX_USABLE_SKILL_COUNT; lCounter++ )
	{
		if( pcsTemplate->m_csUseSkill[lCounter].m_lSkillID != 0 )
		{
			AgpdSkillTemplate* pcsAgpdSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate( pcsTemplate->m_csUseSkill[lCounter].m_lSkillID );
			if( !pcsAgpdSkillTemplate )	continue;

			m_cUseSkillList.SetItemData( m_cUseSkillList.AddString( pcsAgpdSkillTemplate->m_szName ), lCounter );
		}
	}
}

void AgcmAI2Dialog::InitData( AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmAI2 *pcsAgpmAI2, AgpdAI2Template *pcsTemplate )
{
	m_pcsAgpmItem			= pcsAgpmItem;
	m_pcsAgpmSkill			= pcsAgpmSkill;
	m_pcsAgpmAI2			= pcsAgpmAI2;
	m_pcsAgpdAI2Template	= pcsTemplate;
}

BOOL AgcmAI2Dialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ClearControl();
	FillControlByTemplate( m_pcsAgpdAI2Template );
	
	return TRUE; 
}

void AgcmAI2Dialog::OnAI2AddUseItemButton() 
{
	// TODO: Add your control notification handler code here
	INT32 lEmptyIndex;
	BOOL	bFull = TRUE;
	for( lEmptyIndex=0; lEmptyIndex<AGPDAI2_MAX_USABLE_ITEM_COUNT; lEmptyIndex++ )
	{
		if( m_pcsAgpdAI2Template->m_csUseItem[lEmptyIndex].m_lItemID == 0 )
		{
			bFull = FALSE;
			break;
		}
	}

	if( bFull )
	{
		MessageBox( "더 이상 추가할 수 없습니다." );
		return;
	}

	g_csAgcmAI2EdtTableDlg.InitData( m_pcsAgpmItem, m_pcsAgpmSkill, m_pcsAgpmAI2, m_pcsAgpdAI2Template, AGCMAI2DIALOG_EDITDLG_ADDITEM, (void *)&m_pcsAgpdAI2Template->m_csUseItem[lEmptyIndex] );

	if( IDOK == g_csAgcmAI2EdtTableDlg.DoModal() )
	{
		m_cUseItemList.ResetContent();

		for( INT32 lCounter=0; lCounter<AGPDAI2_MAX_USABLE_ITEM_COUNT; lCounter++ )
		{
			if( !m_pcsAgpdAI2Template->m_csUseItem[lCounter].m_lItemID  )	continue;
			AgpdItemTemplate* pcsAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate( m_pcsAgpdAI2Template->m_csUseItem[0].m_lItemID );
			if( !pcsAgpdItemTemplate )	continue;

			m_cUseItemList.SetItemData( m_cUseItemList.AddString( pcsAgpdItemTemplate->m_szName ), lCounter );
		}
	}
}

void AgcmAI2Dialog::OnAI2UpdateUseItemButton() 
{
	// TODO: Add your control notification handler code here
	INT32 lCurSel = m_cUseItemList.GetCurSel();
	if( lCurSel != LB_ERR )
	{
		INT32 lData = m_cUseItemList.GetItemData( lCurSel );
		g_csAgcmAI2EdtTableDlg.InitData( m_pcsAgpmItem, m_pcsAgpmSkill, m_pcsAgpmAI2, m_pcsAgpdAI2Template, AGCMAI2DIALOG_EDITDLG_UPDATEITEM, (void *)&m_pcsAgpdAI2Template->m_csUseItem[lData] );
		g_csAgcmAI2EdtTableDlg.DoModal();
	}
}

void AgcmAI2Dialog::OnAI2DeleteUseItemButton() 
{
	// TODO: Add your control notification handler code here
	INT32 lCurSel = m_cUseItemList.GetCurSel();
	if( lCurSel != LB_ERR )
		m_cUseItemList.DeleteString( lCurSel );
}

void AgcmAI2Dialog::OnAI2AddUseSkillButton() 
{
	BOOL bFull = TRUE;
	INT32 lEmptyIndex;
	for( lEmptyIndex=0; lEmptyIndex<AGPDAI2_MAX_USABLE_SKILL_COUNT; lEmptyIndex++ )
	{
		if( m_pcsAgpdAI2Template->m_csUseSkill[lEmptyIndex].m_lSkillID == 0 )
		{
			bFull = FALSE;
			break;
		}
	}

	if( bFull )
	{
		MessageBox( "더 이상 추가할 수 없습니다." );
		return;
	}

	// TODO: Add your control notification handler code here
	g_csAgcmAI2EdtTableDlg.InitData( m_pcsAgpmItem, m_pcsAgpmSkill, m_pcsAgpmAI2, m_pcsAgpdAI2Template, AGCMAI2DIALOG_EDITDLG_ADDSKILL,(void *)&m_pcsAgpdAI2Template->m_csUseSkill[lEmptyIndex] );

	if( IDOK == g_csAgcmAI2EdtTableDlg.DoModal() )
	{
		for( INT32 lCounter=0; lCounter<AGPDAI2_MAX_USABLE_SKILL_COUNT; lCounter++ )
		{
			if( !m_pcsAgpdAI2Template->m_csUseSkill[lCounter].m_lSkillID )	continue;

			AgpdSkillTemplate* pcsAgpdSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate( m_pcsAgpdAI2Template->m_csUseSkill[0].m_lSkillID );
			if( !pcsAgpdSkillTemplate != NULL )		continue;

			m_cUseSkillList.SetItemData( m_cUseSkillList.AddString( pcsAgpdSkillTemplate->m_szName ), lCounter );
		}
	}	
}

void AgcmAI2Dialog::OnAI2UpdateUseSkillButton() 
{
	// TODO: Add your control notification handler code here
	INT32 lCurSel = m_cUseSkillList.GetCurSel();
	if( lCurSel != LB_ERR )
	{
		INT32 lData = m_cUseSkillList.GetItemData( lCurSel );
		g_csAgcmAI2EdtTableDlg.InitData( m_pcsAgpmItem, m_pcsAgpmSkill, m_pcsAgpmAI2, m_pcsAgpdAI2Template, AGCMAI2DIALOG_EDITDLG_UPDATESKILL, (void *)&m_pcsAgpdAI2Template->m_csUseSkill[lData] );
		g_csAgcmAI2EdtTableDlg.DoModal();
	}
}

void AgcmAI2Dialog::OnAI2DeleteUseSkillButton() 
{
	// TODO: Add your control notification handler code here
	INT32 lCurSel = m_cUseSkillList.GetCurSel();
	if( lCurSel != LB_ERR )
		m_cUseSkillList.DeleteString( lCurSel );
}

void AgcmAI2Dialog::OnAI2AdjustOKButton() 
{
	// TODO: Add your control notification handler code here
	GetTemplateDataFromContrl( m_pcsAgpdAI2Template );

	OnOK();
}

void AgcmAI2Dialog::OnAI2AdjustCancelButton() 
{
	// TODO: Add your control notification handler code here

	OnCancel();
}

BOOL AgcmAI2Dialog::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP )
	{
		if( pMsg->wParam == VK_RETURN )
		{
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void AgcmAI2Dialog::OnCheckNPC() 
{
	// TODO: Add your control notification handler code here
	int					lState;
	CWnd				*pcsWnd;

	lState = m_cNPCCheckButton.GetCheck();
	pcsWnd = GetDlgItem(IDC_Check_FixedNPC);

	if( pcsWnd )
	{
		//선택 안됨.
		if( lState == 0 )
		{
			ActiveMobAI();
			pcsWnd->EnableWindow(FALSE);
		}
		//선택된 상태.
		else
		{
			DeactiveMobAI();
			pcsWnd->EnableWindow(TRUE);
		}	
	}
}
