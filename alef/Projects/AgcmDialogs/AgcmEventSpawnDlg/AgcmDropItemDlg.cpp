// AgcmDropItemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeventspawndlg.h"
#include "AgcmDropItemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmDropItemDlg dialog


AgcmDropItemDlg::AgcmDropItemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmDropItemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmDropItemDlg)
	//}}AFX_DATA_INIT
}


void AgcmDropItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmDropItemDlg)
	DDX_Control(pDX, IDC_DropItemSelectList, m_cDropItemSelectList);
	DDX_Control(pDX, IDC_DropItemSelectEdit, m_cDropItemSelectEdit);
	DDX_Control(pDX, IDC_EquipItemCount9Edit, m_cEquipItemCount9Edit);
	DDX_Control(pDX, IDC_EquipItemCount8Edit, m_cEquipItemCount8Edit);
	DDX_Control(pDX, IDC_EquipItemCount7Edit, m_cEquipItemCount7Edit);
	DDX_Control(pDX, IDC_EquipItemCount6Edit, m_cEquipItemCount6Edit);
	DDX_Control(pDX, IDC_EquipItemCount5Edit, m_cEquipItemCount5Edit);
	DDX_Control(pDX, IDC_EquipItemCount4Edit, m_cEquipItemCount4Edit);
	DDX_Control(pDX, IDC_EquipItemCount3Edit, m_cEquipItemCount3Edit);
	DDX_Control(pDX, IDC_EquipItemCount2Edit, m_cEquipItemCount2Edit);
	DDX_Control(pDX, IDC_EquipItemCount1Edit, m_cEquipItemCount1Edit);
	DDX_Control(pDX, IDC_EquipItemCount10Edit, m_cEquipItemCount10Edit);
	DDX_Control(pDX, IDC_EquipItemCount0Edit, m_cEquipItemCount0Edit);
	DDX_Control(pDX, IDC_RuneOptDefensePoint, m_cRuneOptDefensePoint);
	DDX_Control(pDX, IDC_RuneOptAttackPoint, m_cRuneOptAttackPoint);
	DDX_Control(pDX, IDC_RuneOptAttackSpeed, m_cRuneOptAttackSpeed);
	DDX_Control(pDX, IDC_RuneOptDamageEdit, m_cRuneOptDamage);
	DDX_Control(pDX, IDC_RuneOptDefenseRate, m_cRuneOptDefenseRate);
	DDX_Control(pDX, IDC_RuneOptHP, m_cRuneOptHP);
	DDX_Control(pDX, IDC_RuneOptHPRegen, m_cRuneOptHPRegen);
	DDX_Control(pDX, IDC_RuneOptMaxAttackPoint, m_cRuneOptMaxAttackPoint);
	DDX_Control(pDX, IDC_RuneOptMaxAttackSpeed, m_cRuneOptMaxAttackSpeed);
	DDX_Control(pDX, IDC_RuneOptMaxDamageEdit, m_cRuneOptMaxDamage);
	DDX_Control(pDX, IDC_RuneOptMaxDefensePoint, m_cRuneOptMaxDefensePoint);
	DDX_Control(pDX, IDC_RuneOptMaxDefenseRate, m_cRuneOptMaxDefenseRate);
	DDX_Control(pDX, IDC_RuneOptMaxHP, m_cRuneOptMaxHP);
	DDX_Control(pDX, IDC_RuneOptMaxHPRegen, m_cRuneOptMaxHPRegen);
	DDX_Control(pDX, IDC_RuneOptMaxMP, m_cRuneOptMaxMP);
	DDX_Control(pDX, IDC_RuneOptMaxMPRegen, m_cRuneOptMaxMPRegen);
	DDX_Control(pDX, IDC_RuneOptMaxSP, m_cRuneOptMaxSP);
	DDX_Control(pDX, IDC_RuneOptMaxSPRegen, m_cRuneOptMaxSPRegen);
	DDX_Control(pDX, IDC_RuneOptMinAttackSpeed, m_cRuneOptMinAttackSpeed);
	DDX_Control(pDX, IDC_RuneOptMinAttackPoint, m_cRuneOptMinAttackPoint);
	DDX_Control(pDX, IDC_RuneOptMinDamageEdit, m_cRuneOptMinDamageEdit);
	DDX_Control(pDX, IDC_RuneOptMinDefensePoint, m_cRuneOptMinDefensePoint);
	DDX_Control(pDX, IDC_RuneOptMinDefenseRate, m_cRuneOptMinDefenseRate);
	DDX_Control(pDX, IDC_RuneOptMinHP, m_cRuneOptMinHP);
	DDX_Control(pDX, IDC_RuneOptMinHPRegen, m_cRuneOptMinHPRegen);
	DDX_Control(pDX, IDC_RuneOptMinMP, m_cRuneOptMinMP);
	DDX_Control(pDX, IDC_RuneOptMinMPRegen, m_cRuneOptMinMPRegen);
	DDX_Control(pDX, IDC_RuneOptMinSP, m_cRuneOptMinSP);
	DDX_Control(pDX, IDC_RuneOptMinSPRegen, m_cRuneOptMinSPRegen);
	DDX_Control(pDX, IDC_RuneOptMP, m_cRuneOptMP);
	DDX_Control(pDX, IDC_RuneOptMPRegen, m_cRuneOptMPRegen);
	DDX_Control(pDX, IDC_RuneOptSP, m_cRuneOptSP);
	DDX_Control(pDX, IDC_RuneOptSPRegen, m_cRuneOptSPRegen);
	DDX_Control(pDX, IDC_DropItemList, m_cDropItemList);
	DDX_Control(pDX, IDC_RuneCount9Edit, m_cRuneCount9Edit);
	DDX_Control(pDX, IDC_RuneCount8Edit, m_cRuneCount8Edit);
	DDX_Control(pDX, IDC_RuneCount7Edit, m_cRuneCount7Edit);
	DDX_Control(pDX, IDC_RuneCount6Edit, m_cRuneCount6Edit);
	DDX_Control(pDX, IDC_RuneCount5Edit, m_cRuneCount5Edit);
	DDX_Control(pDX, IDC_RuneCount4Edit, m_cRuneCount4Edit);
	DDX_Control(pDX, IDC_RuneCount3Edit, m_cRuneCount3Edit);
	DDX_Control(pDX, IDC_RuneCount2Edit, m_cRuneCount2Edit);
	DDX_Control(pDX, IDC_RuneCount1Edit, m_cRuneCount1Edit);
	DDX_Control(pDX, IDC_RuneCount10Edit, m_cRuneCount10Edit);
	DDX_Control(pDX, IDC_RuneCount0Edit, m_cRuneCount0Edit);
	DDX_Control(pDX, IDC_ItemCount9Edit, m_cItemCount9Edit);
	DDX_Control(pDX, IDC_ItemCount8Edit, m_cItemCount8Edit);
	DDX_Control(pDX, IDC_ItemCount7Edit, m_cItemCount7Edit);
	DDX_Control(pDX, IDC_ItemCount6Edit, m_cItemCount6Edit);
	DDX_Control(pDX, IDC_ItemCount5Edit, m_cItemCount5Edit);
	DDX_Control(pDX, IDC_ItemCount4Edit, m_cItemCount4Edit);
	DDX_Control(pDX, IDC_ItemCount3Edit, m_cItemCount3Edit);
	DDX_Control(pDX, IDC_ItemCount2Edit, m_cItemCount2Edit);
	DDX_Control(pDX, IDC_ItemCount1Edit, m_cItemCount1Edit);
	DDX_Control(pDX, IDC_ItemCount10Edit, m_cItemCount10Edit);
	DDX_Control(pDX, IDC_ItemCount0Edit, m_cItemCount0Edit);
	DDX_Control(pDX, IDC_SpecialRadio, m_cSpecialRadio);
	DDX_Control(pDX, IDC_RareRadio, m_cRareRadio);
	DDX_Control(pDX, IDC_FreeForAllRadio, m_cFreeForAllRadio);
	DDX_Control(pDX, IDC_CommonRadio, m_cCommonRadio);
	DDX_Control(pDX, IDC_JackpotPieceEdit, m_cJackpotPieceEdit);
	DDX_Control(pDX, IDC_JackpotRateEdit, m_cJackpotRateEdit);
	DDX_Control(pDX, IDC_MaxQuantityEdit, m_cMaxQuantityEdit);
	DDX_Control(pDX, IDC_MinQuantityEdit, m_cMinQuantityEdit);
	DDX_Control(pDX, IDC_EquipRateEdit, m_cEquipRateEdit);
	DDX_Control(pDX, IDC_DropRateEdit, m_cDropRateEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmDropItemDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmDropItemDlg)
	ON_BN_CLICKED(IDC_DropItemDeleteButton, OnDropItemDeleteButton)
	ON_BN_CLICKED(IDC_ItemDropAddUpdateButton, OnItemDropAddUpdateButton)
	ON_BN_CLICKED(IDC_FreeForAllRadio, OnFreeForAllRadio)
	ON_BN_CLICKED(IDC_CommonRadio, OnCommonRadio)
	ON_BN_CLICKED(IDC_RareRadio, OnRareRadio)
	ON_BN_CLICKED(IDC_SpecialRadio, OnSpecialRadio)
	ON_BN_CLICKED(IDC_ItemDropCloseButton, OnItemDropCloseButton)
	ON_NOTIFY(NM_CLICK, IDC_DropItemList, OnClickDropItemList)
	ON_EN_CHANGE(IDC_DropItemSelectEdit, OnChangeDropItemSelectEdit)
	ON_LBN_SELCHANGE(IDC_DropItemSelectList, OnSelchangeDropItemSelectList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmDropItemDlg message handlers
void AgcmDropItemDlg::DisableAllRuneEditBox()
{
	CWnd			*pcWnd;

	//추가 Damage
	pcWnd = GetDlgItem( IDC_RuneOptDamageEdit );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinDamageEdit );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxDamageEdit );
	pcWnd->EnableWindow( FALSE );

	//추가 AttackPoint
	pcWnd = GetDlgItem( IDC_RuneOptAttackPoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinAttackPoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxAttackPoint );
	pcWnd->EnableWindow( FALSE );

	//추가 Defense Rate
	pcWnd = GetDlgItem( IDC_RuneOptDefenseRate );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinDefenseRate );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxDefenseRate );
	pcWnd->EnableWindow( FALSE );

	//추가 Defense Point
	pcWnd = GetDlgItem( IDC_RuneOptDefensePoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinDefensePoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxDefensePoint );
	pcWnd->EnableWindow( FALSE );

	//추가 HP
	pcWnd = GetDlgItem( IDC_RuneOptHP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinHP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxHP );
	pcWnd->EnableWindow( FALSE );

	//추가 MP
	pcWnd = GetDlgItem( IDC_RuneOptMP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinMP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxMP );
	pcWnd->EnableWindow( FALSE );

	//추가 SP
	pcWnd = GetDlgItem( IDC_RuneOptSP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinSP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxSP );
	pcWnd->EnableWindow( FALSE );

	//추가 HP Regen
	pcWnd = GetDlgItem( IDC_RuneOptHPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinHPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxHPRegen );
	pcWnd->EnableWindow( FALSE );

	//추가 MP Regen
	pcWnd = GetDlgItem( IDC_RuneOptMPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinMPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxMPRegen );
	pcWnd->EnableWindow( FALSE );

	//추가 SP Regen
	pcWnd = GetDlgItem( IDC_RuneOptSPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinSPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxSPRegen );
	pcWnd->EnableWindow( FALSE );

	//추가 Attack Speed
	pcWnd = GetDlgItem( IDC_RuneOptAttackSpeed );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinAttackSpeed );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxAttackSpeed );
	pcWnd->EnableWindow( FALSE );
}

void AgcmDropItemDlg::EnableWeaponRuneEditBox()
{
	CWnd			*pcWnd;

	//추가 Damage
	pcWnd = GetDlgItem( IDC_RuneOptDamageEdit );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinDamageEdit );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxDamageEdit );
	pcWnd->EnableWindow();

	//추가 AttackPoint
	pcWnd = GetDlgItem( IDC_RuneOptAttackPoint );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinAttackPoint );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxAttackPoint );
	pcWnd->EnableWindow();

	//추가 Defense Rate
	pcWnd = GetDlgItem( IDC_RuneOptDefenseRate );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinDefenseRate );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxDefenseRate );
	pcWnd->EnableWindow( FALSE );

	//추가 Defense Point
	pcWnd = GetDlgItem( IDC_RuneOptDefensePoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinDefensePoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxDefensePoint );
	pcWnd->EnableWindow( FALSE );

	//추가 HP
	pcWnd = GetDlgItem( IDC_RuneOptHP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinHP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxHP );
	pcWnd->EnableWindow( FALSE );

	//추가 MP
	pcWnd = GetDlgItem( IDC_RuneOptMP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinMP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxMP );
	pcWnd->EnableWindow( FALSE );

	//추가 SP
	pcWnd = GetDlgItem( IDC_RuneOptSP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinSP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxSP );
	pcWnd->EnableWindow( FALSE );

	//추가 HP Regen
	pcWnd = GetDlgItem( IDC_RuneOptHPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinHPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxHPRegen );
	pcWnd->EnableWindow();

	//추가 MP Regen
	pcWnd = GetDlgItem( IDC_RuneOptMPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinMPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxMPRegen );
	pcWnd->EnableWindow();

	//추가 SP Regen
	pcWnd = GetDlgItem( IDC_RuneOptSPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinSPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxSPRegen );
	pcWnd->EnableWindow();

	//추가 Attack Speed
	pcWnd = GetDlgItem( IDC_RuneOptAttackSpeed );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinAttackSpeed );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxAttackSpeed );
	pcWnd->EnableWindow();
}

void AgcmDropItemDlg::EnableShieldRuneEditBox()
{
	CWnd			*pcWnd;

	//추가 Damage
	pcWnd = GetDlgItem( IDC_RuneOptDamageEdit );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinDamageEdit );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxDamageEdit );
	pcWnd->EnableWindow( FALSE );

	//추가 AttackPoint
	pcWnd = GetDlgItem( IDC_RuneOptAttackPoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinAttackPoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxAttackPoint );
	pcWnd->EnableWindow( FALSE );

	//추가 Defense Rate
	pcWnd = GetDlgItem( IDC_RuneOptDefenseRate );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinDefenseRate );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxDefenseRate );
	pcWnd->EnableWindow();

	//추가 Defense Point
	pcWnd = GetDlgItem( IDC_RuneOptDefensePoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinDefensePoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxDefensePoint );
	pcWnd->EnableWindow( FALSE );

	//추가 HP
	pcWnd = GetDlgItem( IDC_RuneOptHP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinHP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxHP );
	pcWnd->EnableWindow( FALSE );

	//추가 MP
	pcWnd = GetDlgItem( IDC_RuneOptMP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinMP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxMP );
	pcWnd->EnableWindow( FALSE );

	//추가 SP
	pcWnd = GetDlgItem( IDC_RuneOptSP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinSP );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxSP );
	pcWnd->EnableWindow( FALSE );

	//추가 HP Regen
	pcWnd = GetDlgItem( IDC_RuneOptHPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinHPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxHPRegen );
	pcWnd->EnableWindow();

	//추가 MP Regen
	pcWnd = GetDlgItem( IDC_RuneOptMPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinMPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxMPRegen );
	pcWnd->EnableWindow();

	//추가 SP Regen
	pcWnd = GetDlgItem( IDC_RuneOptSPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinSPRegen );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxSPRegen );
	pcWnd->EnableWindow();

	//추가 Attack Speed
	pcWnd = GetDlgItem( IDC_RuneOptAttackSpeed );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinAttackSpeed );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxAttackSpeed );
	pcWnd->EnableWindow( FALSE );

}

void AgcmDropItemDlg::EnableArmourRuneEditBox()
{
	CWnd			*pcWnd;

	//추가 Damage
	pcWnd = GetDlgItem( IDC_RuneOptDamageEdit );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinDamageEdit );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxDamageEdit );
	pcWnd->EnableWindow( FALSE );

	//추가 AttackPoint
	pcWnd = GetDlgItem( IDC_RuneOptAttackPoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinAttackPoint );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxAttackPoint );
	pcWnd->EnableWindow( FALSE );

	//추가 Defense Rate
	pcWnd = GetDlgItem( IDC_RuneOptDefenseRate );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinDefenseRate );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxDefenseRate );
	pcWnd->EnableWindow( FALSE );

	//추가 Defense Point
	pcWnd = GetDlgItem( IDC_RuneOptDefensePoint );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinDefensePoint );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxDefensePoint );
	pcWnd->EnableWindow();

	//추가 HP
	pcWnd = GetDlgItem( IDC_RuneOptHP );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinHP );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxHP );
	pcWnd->EnableWindow();

	//추가 MP
	pcWnd = GetDlgItem( IDC_RuneOptMP );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinMP );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxMP );
	pcWnd->EnableWindow();

	//추가 SP
	pcWnd = GetDlgItem( IDC_RuneOptSP );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMinSP );
	pcWnd->EnableWindow();

	pcWnd = GetDlgItem( IDC_RuneOptMaxSP );
	pcWnd->EnableWindow();

	//추가 HP Regen
	pcWnd = GetDlgItem( IDC_RuneOptHPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinHPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxHPRegen );
	pcWnd->EnableWindow( FALSE );

	//추가 MP Regen
	pcWnd = GetDlgItem( IDC_RuneOptMPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinMPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxMPRegen );
	pcWnd->EnableWindow( FALSE );

	//추가 SP Regen
	pcWnd = GetDlgItem( IDC_RuneOptSPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinSPRegen );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxSPRegen );
	pcWnd->EnableWindow( FALSE );

	//추가 Attack Speed
	pcWnd = GetDlgItem( IDC_RuneOptAttackSpeed );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMinAttackSpeed );
	pcWnd->EnableWindow( FALSE );

	pcWnd = GetDlgItem( IDC_RuneOptMaxAttackSpeed );
	pcWnd->EnableWindow( FALSE );

}

void AgcmDropItemDlg::SetDlgData( AgpmItem *pcsAgpmItem, AgpdDropItemTemplate *pcsAgpdDropItemTemplate )
{
	m_pcsAgpmItem = pcsAgpmItem;
//	m_pcsAgpdDropItemTemplate = pcsAgpdDropItemTemplate;
}

void AgcmDropItemDlg::SetDropItemTemplate( AgpdDropItemTemplate *pcsAgpdDropItemTemplate )
{
//	m_pcsAgpdDropItemTemplate = pcsAgpdDropItemTemplate;
}

//void AgcmDropItemDlg::UpdateDropItemListRow( int iRow, int lItemTID, int iDropRate, int iEquipRate, int iMinQuantity, int iMaxQuantity, int iJackpotRate, int iJackpotPiece )
void AgcmDropItemDlg::UpdateDropItemListRow( int iRow, AgpdDropItemInfo *pcsDropItemInfo )
{
	AgpdItemTemplate		*pcsItemTemplate;

	char			strBuffer[80];

	pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( pcsDropItemInfo->m_lItemTID );

	if( pcsItemTemplate != NULL )
	{
		m_cDropItemList.SetItemData( iRow, pcsItemTemplate->m_lID );

		//ItemName
		m_cDropItemList.SetItemText( iRow, 0, pcsItemTemplate->m_szName );
		m_cDropItemList.SetItemData( iRow, pcsItemTemplate->m_lID );

		//DropRate
		sprintf( strBuffer, "%d", pcsDropItemInfo->m_lDropRate );
		m_cDropItemList.SetItemText( iRow, 1, strBuffer );

		//EquipDropRate
		sprintf( strBuffer, "%d", pcsDropItemInfo->m_lERate );
		m_cDropItemList.SetItemText( iRow, 2, strBuffer );

		//Min~Max Quantity
		sprintf( strBuffer, "%d ~ %d", pcsDropItemInfo->m_lMinQuantity, pcsDropItemInfo->m_lMaxQuantity );
		m_cDropItemList.SetItemText( iRow, 3, strBuffer );

		//JackpotRate
		sprintf( strBuffer, "%d", pcsDropItemInfo->m_lJackpotRate );
		m_cDropItemList.SetItemText( iRow, 4, strBuffer );

		//JackpotPiece
		sprintf( strBuffer, "%d", pcsDropItemInfo->m_lJackpotPiece );
		m_cDropItemList.SetItemText( iRow, 5, strBuffer );
	}
}

void AgcmDropItemDlg::EditDropItemListRow( int iRow )
{
	char			strBuffer[80];

	memset( strBuffer, 0, sizeof(strBuffer) );

	m_cDropItemList.GetItemText( iRow, 0, strBuffer, sizeof(strBuffer) );

	if( !strcmp( strBuffer, "N/A" ) )
	{
		m_cDropItemList.SetItemText( iRow, 0, "편집중" );
		m_cDropItemList.SetItemText( iRow, 1, "편집중" );
		m_cDropItemList.SetItemText( iRow, 2, "편집중" );
		m_cDropItemList.SetItemText( iRow, 3, "편집중" );
		m_cDropItemList.SetItemText( iRow, 4, "편집중" );
		m_cDropItemList.SetItemText( iRow, 5, "편집중" );
	}
	else
	{
		char			strOutputBuffer[80];

		sprintf( strOutputBuffer, "%s(*)", strBuffer );

		m_cDropItemList.SetItemText( iRow, 0, strOutputBuffer );
	}
}

void AgcmDropItemDlg::CleanDropItemList()
{
	INT32				lCounter;

	for( lCounter=0; lCounter<AGPDDROPITEM_ITEM_COUNT; lCounter++ )
	{
		CleanDropItemListRow( lCounter );
	}
}

void AgcmDropItemDlg::CleanDropItemListRow( int iRow )
{
	m_cDropItemList.SetItemText( iRow, 0, "N/A" );
	m_cDropItemList.SetItemText( iRow, 1, "N/A" );
	m_cDropItemList.SetItemText( iRow, 2, "N/A" );
	m_cDropItemList.SetItemText( iRow, 3, "N/A" );
	m_cDropItemList.SetItemText( iRow, 4, "N/A" );
	m_cDropItemList.SetItemText( iRow, 5, "N/A" );
}

/*eAgpmDropItemBag AgcmDropItemDlg::GetCurrentBag()
{
	eAgpmDropItemBag eBag;

	//가방정보
	if( m_cFreeForAllRadio.GetState() == 0x00000001 )
	{
		eBag = AGPMDROPITEM_FREEFORALL;
	}
	else if( m_cCommonRadio.GetState() == 0x00000001 )
	{
		eBag = AGPMDROPITEM_COMMON;
	}
	else if( m_cRareRadio.GetState() == 0x00000001 )
	{
		eBag = AGPMDROPITEM_RARE;
	}
	else if( m_cSpecialRadio.GetState() == 0x00000001 )
	{
		eBag = AGPMDROPITEM_SPECIAL;
	}

	return eBag;
}*/

/*AgpdDropItemBag	*AgcmDropItemDlg::GetCurrentBag( eAgpmDropItemBag eBag )
{
	AgpdDropItemBag			*pcsCurrentEditBag;

	pcsCurrentEditBag = NULL;

	//가방정보
	if( eBag == AGPMDROPITEM_FREEFORALL )
	{
		pcsCurrentEditBag = &m_pcsAgpdDropItemTemplate->m_csFreeForAll;
	}
	else if( eBag == AGPMDROPITEM_COMMON )
	{
		pcsCurrentEditBag = &m_pcsAgpdDropItemTemplate->m_csCommon;
	}
	else if( eBag == AGPMDROPITEM_RARE )
	{
		pcsCurrentEditBag = &m_pcsAgpdDropItemTemplate->m_csRare;
	}
	else if( eBag == AGPMDROPITEM_SPECIAL )
	{
		pcsCurrentEditBag = &m_pcsAgpdDropItemTemplate->m_csSpecial;
	}

	return pcsCurrentEditBag;
}*/

/*void AgcmDropItemDlg::SetDropDataFromDropTemplate( eAgpmDropItemBag eBag, int iRow )
{
	AgpdDropItemBag			*pcsCurrentEditBag;

	pcsCurrentEditBag = GetCurrentBag( eBag );

	if( pcsCurrentEditBag != NULL )
	{
		char			strBuffer[80];
		AgpdItemTemplate	*pcsItemTemplate;

		pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lItemTID );

		UpdateDropItemListRow( iRow, 
								pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lItemTID, 
								pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lDropRate,
								pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lERate,
								pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lMinQuantity,
								pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lMaxQuantity,
								pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lJackpotRate,
								pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lJackpotPiece );
		UpdateDropItemListRow( iRow, &pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow] );

		if( pcsItemTemplate != NULL )
		{
			UpdateRuneOptionEditBox( pcsItemTemplate );
		}
		else
		{
			DisableAllRuneEditBox();
		}

		//Item기본 드랍 정보
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lDropRate, strBuffer, 10 );
		m_cDropRateEdit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lERate, strBuffer, 10 );
		m_cEquipRateEdit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lMinQuantity, strBuffer, 10 );
		m_cMinQuantityEdit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lMaxQuantity, strBuffer, 10 );
		m_cMaxQuantityEdit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lJackpotRate, strBuffer, 10 );
		m_cJackpotRateEdit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lJackpotPiece, strBuffer, 10 );
		m_cJackpotPieceEdit.SetWindowText( strBuffer );

		//Rune옵션정보
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptDamage, strBuffer, 10 );
		m_cRuneOptDamage.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinDamage, strBuffer, 10 );
		m_cRuneOptMinDamageEdit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxDamage, strBuffer, 10 );
		m_cRuneOptMaxDamage.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptAttackPoint, strBuffer, 10 );
		m_cRuneOptAttackPoint.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinAttackPoint, strBuffer, 10 );
		m_cRuneOptMinAttackPoint.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxAttackPoint, strBuffer, 10 );
		m_cRuneOptMaxAttackPoint.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptDefenseRate, strBuffer, 10 );
		m_cRuneOptDefenseRate.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinDefenseRate, strBuffer, 10 );
		m_cRuneOptMinDefenseRate.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxDefenseRate, strBuffer, 10 );
		m_cRuneOptMaxDefenseRate.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptDefensePoint, strBuffer, 10 );
		m_cRuneOptDefensePoint.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinDefensePoint, strBuffer, 10 );
		m_cRuneOptMinDefensePoint.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxDefensePoint, strBuffer, 10 );
		m_cRuneOptMaxDefensePoint.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptHP, strBuffer, 10 );
		m_cRuneOptHP.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinHP, strBuffer, 10 );
		m_cRuneOptMinHP.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxHP, strBuffer, 10 );
		m_cRuneOptMaxHP.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMP, strBuffer, 10 );
		m_cRuneOptMP.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinMP, strBuffer, 10 );
		m_cRuneOptMinMP.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxMP, strBuffer, 10 );
		m_cRuneOptMaxMP.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptSP, strBuffer, 10 );
		m_cRuneOptSP.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinSP, strBuffer, 10 );
		m_cRuneOptMinSP.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxSP, strBuffer, 10 );
		m_cRuneOptMaxSP.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptHPRegen, strBuffer, 10 );
		m_cRuneOptHPRegen.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinHPRegen, strBuffer, 10 );
		m_cRuneOptMinHPRegen.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxHPRegen, strBuffer, 10 );
		m_cRuneOptMaxHPRegen.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMPRegen, strBuffer, 10 );
		m_cRuneOptMPRegen.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinMPRegen, strBuffer, 10 );
		m_cRuneOptMinMPRegen.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxMPRegen, strBuffer, 10 );
		m_cRuneOptMaxMPRegen.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptSPRegen, strBuffer, 10 );
		m_cRuneOptSPRegen.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinSPRegen, strBuffer, 10 );
		m_cRuneOptMinSPRegen.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxSPRegen, strBuffer, 10 );
		m_cRuneOptMaxSPRegen.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptAttackSpeed, strBuffer, 10 );
		m_cRuneOptAttackSpeed.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMinAttackSpeed, strBuffer, 10 );
		m_cRuneOptMinAttackSpeed.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_lRuneOptMaxAttackSpeed, strBuffer, 10 );
		m_cRuneOptMaxAttackSpeed.SetWindowText( strBuffer );

		//Item Drop Count
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[0], strBuffer, 10 );
		m_cItemCount0Edit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_alDropItemCountRate[1], strBuffer, 10 );
		m_cItemCount1Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[2], strBuffer, 10 );
		m_cItemCount2Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[3], strBuffer, 10 );
		m_cItemCount3Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[4], strBuffer, 10 );
		m_cItemCount4Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[5], strBuffer, 10 );
		m_cItemCount5Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[6], strBuffer, 10 );
		m_cItemCount6Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[7], strBuffer, 10 );
		m_cItemCount7Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[8], strBuffer, 10 );
		m_cItemCount8Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[9], strBuffer, 10 );
		m_cItemCount9Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropItemCountRate[10], strBuffer, 10 );
		m_cItemCount10Edit.SetWindowText( strBuffer );

		//Equip시 Item Drop Count
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[0], strBuffer, 10 );
		m_cEquipItemCount0Edit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[1], strBuffer, 10 );
		m_cEquipItemCount1Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[2], strBuffer, 10 );
		m_cEquipItemCount2Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[3], strBuffer, 10 );
		m_cEquipItemCount3Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[4], strBuffer, 10 );
		m_cEquipItemCount4Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[5], strBuffer, 10 );
		m_cEquipItemCount5Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[6], strBuffer, 10 );
		m_cEquipItemCount6Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[7], strBuffer, 10 );
		m_cEquipItemCount7Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[8], strBuffer, 10 );
		m_cEquipItemCount8Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[9], strBuffer, 10 );
		m_cEquipItemCount9Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_alDropEquipItemCountRate[10], strBuffer, 10 );
		m_cEquipItemCount10Edit.SetWindowText( strBuffer );

		//Rune Drop Count
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[0], strBuffer, 10 );
		m_cRuneCount0Edit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[1], strBuffer, 10 );
		m_cRuneCount1Edit.SetWindowText( strBuffer );

		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[2], strBuffer, 10 );
		m_cRuneCount2Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[3], strBuffer, 10 );
		m_cRuneCount3Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[4], strBuffer, 10 );
		m_cRuneCount4Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[5], strBuffer, 10 );
		m_cRuneCount5Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[6], strBuffer, 10 );
		m_cRuneCount6Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[7], strBuffer, 10 );
		m_cRuneCount7Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[8], strBuffer, 10 );
		m_cRuneCount8Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[9], strBuffer, 10 );
		m_cRuneCount9Edit.SetWindowText( strBuffer );
		
		itoa( pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[10], strBuffer, 10 );
		m_cRuneCount10Edit.SetWindowText( strBuffer );
	}
}*/

/*void AgcmDropItemDlg::GetDropDataFromDialog( int iRow )
{
	AgpdDropItemBag			*pcsCurrentEditBag;

	pcsCurrentEditBag = GetCurrentBag( GetCurrentBag() );

	if( pcsCurrentEditBag != NULL )
	{
		AgpdDropItemInfo	*pcsDropItemInfo;
		AgpdItemTemplate	*pcsItemTemplate;

		INT32			lItemTID;
		INT32			lSelectCur;
		char			strBuffer[255];

		pcsDropItemInfo = &pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow];

		//Item이름 복사.
		lSelectCur = m_cDropItemSelectList.GetCurSel();

		lItemTID = m_cDropItemSelectList.GetItemData( lSelectCur );

		pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( lItemTID );

		if( pcsItemTemplate != NULL )
		{
			pcsDropItemInfo->m_lItemTID = pcsItemTemplate->m_lID;

			//Item기본 드랍 정보
			m_cDropRateEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lDropRate = atoi( strBuffer );
			m_cEquipRateEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lERate = atoi( strBuffer );
			m_cMinQuantityEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lMinQuantity = atoi( strBuffer );
			m_cMaxQuantityEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lMaxQuantity = atoi( strBuffer );
			m_cJackpotRateEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lJackpotRate = atoi( strBuffer );
			m_cJackpotPieceEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lJackpotPiece = atoi( strBuffer );

			//Rune옵션정보
			m_cRuneOptDamage.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptDamage = atoi( strBuffer );
			m_cRuneOptMinDamageEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinDamage = atoi( strBuffer );
			m_cRuneOptMaxDamage.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxDamage = atoi( strBuffer );
			m_cRuneOptAttackPoint.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptAttackPoint = atoi( strBuffer );
			m_cRuneOptMinAttackPoint.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinAttackPoint = atoi( strBuffer );
			m_cRuneOptMaxAttackPoint.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxAttackPoint = atoi( strBuffer );
			m_cRuneOptDefenseRate.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptDefenseRate = atoi( strBuffer );
			m_cRuneOptMinDefenseRate.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinDefenseRate = atoi( strBuffer );
			m_cRuneOptMaxDefenseRate.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxDefenseRate = atoi( strBuffer );
			m_cRuneOptDefensePoint.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptDefensePoint = atoi( strBuffer );
			m_cRuneOptMinDefensePoint.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinDefensePoint = atoi( strBuffer );
			m_cRuneOptMaxDefensePoint.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxDefensePoint = atoi( strBuffer );
			m_cRuneOptHP.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptHP = atoi( strBuffer );
			m_cRuneOptMinHP.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinHP = atoi( strBuffer );
			m_cRuneOptMaxHP.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxHP = atoi( strBuffer );
			m_cRuneOptMP.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMP = atoi( strBuffer );
			m_cRuneOptMinMP.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinMP = atoi( strBuffer );
			m_cRuneOptMaxMP.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxMP = atoi( strBuffer );
			m_cRuneOptSP.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptSP = atoi( strBuffer );
			m_cRuneOptMinSP.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinSP = atoi( strBuffer );
			m_cRuneOptMaxSP.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxSP = atoi( strBuffer );
			m_cRuneOptHPRegen.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptHPRegen = atoi( strBuffer );
			m_cRuneOptMinHPRegen.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinHPRegen = atoi( strBuffer );
			m_cRuneOptMaxHPRegen.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxHPRegen = atoi( strBuffer );
			m_cRuneOptMPRegen.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMPRegen = atoi( strBuffer );
			m_cRuneOptMinMPRegen.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinMPRegen = atoi( strBuffer );
			m_cRuneOptMaxMPRegen.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxMPRegen = atoi( strBuffer );
			m_cRuneOptSPRegen.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptSPRegen = atoi( strBuffer );
			m_cRuneOptMinSPRegen.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinSPRegen = atoi( strBuffer );
			m_cRuneOptMaxSPRegen.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxSPRegen = atoi( strBuffer );
			m_cRuneOptAttackSpeed.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptAttackSpeed = atoi( strBuffer );
			m_cRuneOptMinAttackSpeed.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMinAttackSpeed = atoi( strBuffer );
			m_cRuneOptMaxAttackSpeed.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsDropItemInfo->m_lRuneOptMaxAttackSpeed = atoi( strBuffer );

			//Item Drop Count 정보
			m_cItemCount0Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[0] = atoi( strBuffer );
			m_cItemCount1Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[1] = atoi( strBuffer );
			m_cItemCount2Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[2] = atoi( strBuffer );
			m_cItemCount3Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[3] = atoi( strBuffer );
			m_cItemCount4Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[4] = atoi( strBuffer );
			m_cItemCount5Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[5] = atoi( strBuffer );
			m_cItemCount6Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[6] = atoi( strBuffer );
			m_cItemCount7Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[7] = atoi( strBuffer );
			m_cItemCount8Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[8] = atoi( strBuffer );
			m_cItemCount9Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[9] = atoi( strBuffer );
			m_cItemCount10Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[10] = atoi( strBuffer );

			//Equip시 Item Drop Count 정보
			m_cEquipItemCount0Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[0] = atoi( strBuffer );
			m_cEquipItemCount1Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[1] = atoi( strBuffer );
			m_cEquipItemCount2Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[2] = atoi( strBuffer );
			m_cEquipItemCount3Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[3] = atoi( strBuffer );
			m_cEquipItemCount4Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[4] = atoi( strBuffer );
			m_cEquipItemCount5Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[5] = atoi( strBuffer );
			m_cEquipItemCount6Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[6] = atoi( strBuffer );
			m_cEquipItemCount7Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[7] = atoi( strBuffer );
			m_cEquipItemCount8Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[8] = atoi( strBuffer );
			m_cEquipItemCount9Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[9] = atoi( strBuffer );
			m_cEquipItemCount10Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropEquipItemCountRate[10] = atoi( strBuffer );

			//Equip시 Item Drop Count 정보
			m_cItemCount0Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[0] = atoi( strBuffer );
			m_cItemCount1Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[1] = atoi( strBuffer );
			m_cItemCount2Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[2] = atoi( strBuffer );
			m_cItemCount3Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[3] = atoi( strBuffer );
			m_cItemCount4Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[4] = atoi( strBuffer );
			m_cItemCount5Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[5] = atoi( strBuffer );
			m_cItemCount6Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[6] = atoi( strBuffer );
			m_cItemCount7Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[7] = atoi( strBuffer );
			m_cItemCount8Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[8] = atoi( strBuffer );
			m_cItemCount9Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[9] = atoi( strBuffer );
			m_cItemCount10Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_alDropItemCountRate[10] = atoi( strBuffer );

			//Rune Drop Count 정보
			m_cRuneCount0Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[0] = atoi( strBuffer );
			m_cRuneCount1Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[1] = atoi( strBuffer );
			m_cRuneCount2Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[2] = atoi( strBuffer );
			m_cRuneCount3Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[3] = atoi( strBuffer );
			m_cRuneCount4Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[4] = atoi( strBuffer );
			m_cRuneCount5Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[5] = atoi( strBuffer );
			m_cRuneCount6Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[6] = atoi( strBuffer );
			m_cRuneCount7Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[7] = atoi( strBuffer );
			m_cRuneCount8Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[8] = atoi( strBuffer );
			m_cRuneCount9Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[9] = atoi( strBuffer );
			m_cRuneCount10Edit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsCurrentEditBag->m_csAgpdDropItemInfo[iRow].m_alDropRuneCountRate[10] = atoi( strBuffer );
		}
	}
}*/

BOOL AgcmDropItemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
/*	m_lCurrentEditBag = AGPMDROPITEM_FREEFORALL;
	m_lFreeForAllCurrentEditRow = -1;
	m_lFreeForAllCurrentEditRow = -1;
	m_lCommonCurrentEditRow = -1;
	m_lCommonCurrentEditRow = -1;
	m_lRareCurrentEditRow = -1;
	m_lRareCurrentEditRow = -1;
	m_lSpecialCurrentEditRow = -1;
	m_lSpecialCurrentEditRow = -1;*/

	//Radio Button초기화
	m_cFreeForAllRadio.SetCheck( 1 );

	//Drop Item ListCtrl 초기화
	INT32				lCounter;

	m_cDropItemList.InsertColumn( 1, "아이템이름", LVCFMT_LEFT, 200 );
	m_cDropItemList.InsertColumn( 2, "DropRate", LVCFMT_LEFT, 75 );
	m_cDropItemList.InsertColumn( 3, "E.Rate", LVCFMT_LEFT, 75 );
	m_cDropItemList.InsertColumn( 4, "Quantity", LVCFMT_LEFT, 100 );
	m_cDropItemList.InsertColumn( 5, "JackpotRate", LVCFMT_LEFT, 100 );
	m_cDropItemList.InsertColumn( 6, "JackpotPiece", LVCFMT_LEFT, 100 );

	for( lCounter=0; lCounter<AGPDDROPITEM_ITEM_COUNT; lCounter++ )
	{
		m_cDropItemList.InsertItem(lCounter, "N/A" );
		CleanDropItemListRow( lCounter );
//		SetDropDataFromDropTemplate( AGPMDROPITEM_FREEFORALL, lCounter );
	}

	//RuneOption초기화
	DisableAllRuneEditBox();

	//ListBox 초기화
	AgpaItemTemplate		*pcsAgpaItemTemplate;
	AgpdItemTemplate		**ppcsItemTemplate;

	char			strBuffer[255];
	int				iIndex;

	pcsAgpaItemTemplate = m_pcsAgpmItem->GetItemTemplateAdmin();
	iIndex = 0;

	while( 1 ) 
	{
		ppcsItemTemplate = (AgpdItemTemplate **)pcsAgpaItemTemplate->GetObjectSequence( &iIndex );

		if( ppcsItemTemplate == NULL )
		{
			break;
		}
		else
		{
			int				iIndex;

			sprintf( strBuffer, "%s, ID:%d", (*ppcsItemTemplate)->m_szName, (*ppcsItemTemplate)->m_lID );

			iIndex = m_cDropItemSelectList.AddString( strBuffer );
			m_cDropItemSelectList.SetItemData( iIndex, (*ppcsItemTemplate)->m_lID );
		}
	}

	//Control Data초기화.

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmDropItemDlg::OnDropItemDeleteButton() 
{
	// TODO: Add your control notification handler code here
	int			iSelectRow;

	iSelectRow = m_cDropItemList.GetSelectionMark();

	if( iSelectRow != -1 )
	{
		INT32				lResult;

		lResult = MessageBox( "정말 지우시겠습니까?", "확인", MB_YESNO );

		if( lResult == IDYES )
		{
			AgpdDropItemBag			*pcsAgpdDropItemBag;

//			pcsAgpdDropItemBag = GetCurrentBag( GetCurrentBag() );

/*			if( pcsAgpdDropItemBag != NULL )
			{
				pcsAgpdDropItemBag->m_csAgpdDropItemInfo[iSelectRow].Reset();

				//해당 RowData를 초기화한다.
				CleanDropItemListRow( iSelectRow );
			}*/
		}
	}
	else
	{
		MessageBox( "지울 드랍데이터를 선택해주십시요." );
	}
}

void AgcmDropItemDlg::OnItemDropAddUpdateButton() 
{
/*	// TODO: Add your control notification handler code here
	int			iSelectRow;

	iSelectRow = m_cDropItemList.GetSelectionMark();

	if( iSelectRow != -1 )
	{
		AgpdDropItemBag			*pcsAgpdDropItemBag;

		pcsAgpdDropItemBag = GetCurrentBag( GetCurrentBag() );

		if( pcsAgpdDropItemBag != NULL )
		{
			GetDropDataFromDialog( iSelectRow );

			UpdateDropItemListRow( iSelectRow, &pcsAgpdDropItemBag->m_csAgpdDropItemInfo[iSelectRow] );

		}
		//왜 가방 선택 안했어?
		else
		{

		}
	}
	else
	{
		//경고문을 내자~ 왜 편집할놈 Item Index선택 안한겨~
		MessageBox( "추가/변경할 곳을 선택해주십시요." );
	}*/
}


void AgcmDropItemDlg::OnFreeForAllRadio() 
{
	// TODO: Add your control notification handler code here
/*	INT32				lCounter;

	for( lCounter=0; lCounter<AGPDDROPITEM_ITEM_COUNT; lCounter++ )
	{
		CleanDropItemListRow( lCounter );
		SetDropDataFromDropTemplate( AGPMDROPITEM_FREEFORALL, lCounter );
	}

	m_lCurrentEditBag = AGPMDROPITEM_FREEFORALL;

	if( m_lFreeForAllCurrentEditRow != -1 )
		EditDropItemListRow( m_lFreeForAllCurrentEditRow );*/
}

void AgcmDropItemDlg::OnCommonRadio() 
{
/*	// TODO: Add your control notification handler code here
	INT32				lCounter;

	for( lCounter=0; lCounter<AGPDDROPITEM_ITEM_COUNT; lCounter++ )
	{
		CleanDropItemListRow( lCounter );
		SetDropDataFromDropTemplate( AGPMDROPITEM_COMMON, lCounter );
	}

	m_lCurrentEditBag = AGPMDROPITEM_COMMON;

	if( m_lCommonCurrentEditRow != -1 )
		EditDropItemListRow( m_lCommonCurrentEditRow );*/
}

void AgcmDropItemDlg::OnRareRadio() 
{
/*	// TODO: Add your control notification handler code here
	INT32				lCounter;

	for( lCounter=0; lCounter<AGPDDROPITEM_ITEM_COUNT; lCounter++ )
	{
		CleanDropItemListRow( lCounter );
		SetDropDataFromDropTemplate( AGPMDROPITEM_RARE, lCounter );
	}

	m_lCurrentEditBag = AGPMDROPITEM_RARE;

	if( m_lRareCurrentEditRow != -1 )
		EditDropItemListRow( m_lRareCurrentEditRow );*/
}

void AgcmDropItemDlg::OnSpecialRadio() 
{
/*	// TODO: Add your control notification handler code here
	INT32				lCounter;

	for( lCounter=0; lCounter<AGPDDROPITEM_ITEM_COUNT; lCounter++ )
	{
		CleanDropItemListRow( lCounter );
		SetDropDataFromDropTemplate( AGPMDROPITEM_SPECIAL, lCounter );
	}

	m_lCurrentEditBag = AGPMDROPITEM_SPECIAL;

	if( m_lSpecialCurrentEditRow != -1 )
		EditDropItemListRow( m_lSpecialCurrentEditRow );*/
}

void AgcmDropItemDlg::OnItemDropCloseButton() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void AgcmDropItemDlg::OnClickDropItemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	// TODO: Add your control notification handler code here
	LPNMITEMACTIVATE		nia;

	nia = (LPNMITEMACTIVATE) pNMHDR;

	if( nia->iItem != -1 )
	{
		eAgpmDropItemBag	eBag;
		char				strBuffer[255];
		INT32				lTID;

		eBag = GetCurrentBag();

		memset( strBuffer, 0, sizeof(strBuffer) );

		lTID = m_cDropItemList.GetItemData( nia->iItem );

		if( lTID > 0 )
		{
			INT32			lMaxCount;

			lMaxCount = m_cDropItemSelectList.GetCount();

			for( INT32 lCount=0; lCount<lMaxCount; lCount++ )
			{
				if( lTID == m_cDropItemSelectList.GetItemData( lCount ) )
				{
					m_cDropItemSelectList.SetCurSel( lCount );
					break;
				}
			}
		}

		if( eBag == AGPMDROPITEM_FREEFORALL )
		{
			m_lOldFreeForAllCurrentEditRow = m_lFreeForAllCurrentEditRow;
			m_lFreeForAllCurrentEditRow = nia->iItem;

			if( m_lOldFreeForAllCurrentEditRow != -1 )
			{
				CleanDropItemListRow( m_lOldFreeForAllCurrentEditRow );
				SetDropDataFromDropTemplate( AGPMDROPITEM_FREEFORALL, m_lOldFreeForAllCurrentEditRow );
			}

			EditDropItemListRow( m_lFreeForAllCurrentEditRow );
		}
		else if( eBag == AGPMDROPITEM_COMMON )
		{
			m_lOldCommonCurrentEditRow = m_lCommonCurrentEditRow;
			m_lCommonCurrentEditRow = nia->iItem;

			if( m_lOldCommonCurrentEditRow != -1 )
			{
				CleanDropItemListRow( m_lOldCommonCurrentEditRow );
				SetDropDataFromDropTemplate( AGPMDROPITEM_COMMON, m_lOldCommonCurrentEditRow );
			}

			EditDropItemListRow( m_lCommonCurrentEditRow );
		}
		else if( eBag == AGPMDROPITEM_RARE )
		{
			m_lOldRareCurrentEditRow = m_lRareCurrentEditRow;
			m_lRareCurrentEditRow = nia->iItem;

			if( m_lOldRareCurrentEditRow != -1 )
			{
				CleanDropItemListRow( m_lOldRareCurrentEditRow );
				SetDropDataFromDropTemplate( AGPMDROPITEM_RARE, m_lOldRareCurrentEditRow );
			}

			EditDropItemListRow( m_lRareCurrentEditRow );
		}
		else if( eBag == AGPMDROPITEM_SPECIAL )
		{
			m_lOldSpecialCurrentEditRow = m_lSpecialCurrentEditRow;
			m_lSpecialCurrentEditRow = nia->iItem;

			if( m_lOldSpecialCurrentEditRow != -1 )
			{
				CleanDropItemListRow( m_lOldSpecialCurrentEditRow );
				SetDropDataFromDropTemplate( AGPMDROPITEM_SPECIAL, m_lOldSpecialCurrentEditRow );
			}

			EditDropItemListRow( m_lSpecialCurrentEditRow );
		}

		SetDropDataFromDropTemplate( GetCurrentBag(), nia->iItem );
		EditDropItemListRow( nia->iItem );
	}*/
	
	*pResult = 0;
}

BOOL AgcmDropItemDlg::PreTranslateMessage(MSG* pMsg) 
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

void AgcmDropItemDlg::UpdateRuneOptionEditBox( AgpdItemTemplate *pcsItemTemplate )
{
	//템플릿을 얻어왔으면?
	if( pcsItemTemplate )
	{
		//Equip Item이고
		if( pcsItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP )
		{
			AgpdItemTemplateEquip				*pcsTemplateEquip;
			
			pcsTemplateEquip = (AgpdItemTemplateEquip *) pcsItemTemplate;

			//무기류
			if( pcsTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
			{
				EnableWeaponRuneEditBox();
			}
			//방패류
			else if( pcsTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD )
			{
				EnableShieldRuneEditBox();
			}
			//갑옷류
			else if( pcsTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR )
			{
				EnableArmourRuneEditBox();
			}
			//그외의 경우라면 전부 Disable
			else
			{
				DisableAllRuneEditBox();
			}
		}
	}
}

void AgcmDropItemDlg::OnChangeDropItemSelectEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	char		strBuffer[80];

	int			lTotalCount;
	int			iData;

	lTotalCount = m_cDropItemSelectList.GetCount();

	if( lTotalCount > 0 )
	{
		m_cDropItemSelectEdit.GetWindowText( strBuffer, sizeof(strBuffer) );

		for( int iCount=0; iCount<lTotalCount; iCount++ )
		{
			iData = m_cDropItemSelectList.GetItemData( iCount );

			if( iData == atoi( strBuffer ) )
			{ 
				m_cDropItemSelectList.SetCurSel( iCount );
			}
		}
	}
}

void AgcmDropItemDlg::OnSelchangeDropItemSelectList() 
{
	// TODO: Add your control notification handler code here
	int				iIndex;

	iIndex = m_cDropItemSelectList.GetCurSel();

	if( iIndex != CB_ERR )
	{
		INT32			lItemTID;

		lItemTID = m_cDropItemSelectList.GetItemData( iIndex );

		//뭔가 스트링을 얻어왔다면?
		if( lItemTID > 0 )
		{
			AgpdItemTemplate			*pcsItemTemplate;

			pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( lItemTID );

			//템플릿을 얻어왔으면?
			if( pcsItemTemplate )
			{
				UpdateRuneOptionEditBox( pcsItemTemplate );
			}
		}
	}
}

/*
void AgcmDropItemDlg::OnSelchangeDropItemSelectCombo() 
{
	// TODO: Add your control notification handler code here

	//아이템의 장착 슬롯에 따라 룬 옵션을 세팅해준다.
	int				iIndex;

	iIndex = m_cDropItemSelectList.GetCurSel();

	if( iIndex != CB_ERR )
	{
		char				strBuffer[255];

		memset( strBuffer, 0, sizeof(strBuffer) );

		m_cDropItemSelectList.GetText( iIndex, strBuffer );

		//뭔가 스트링을 얻어왔다면?
		if( strlen( strBuffer) )
		{
			AgpdItemTemplate			*pcsItemTemplate;

			pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( strBuffer );

			//템플릿을 얻어왔으면?
			if( pcsItemTemplate )
			{
				UpdateRuneOptionEditBox( pcsItemTemplate );
			}
		}
	}
}

void AgcmDropItemDlg::OnEditchangeDropItemSelectCombo() 
{
	// TODO: Add your control notification handler code here
	char		strBuffer[80];

	int			lTotalCount;
	int			iData, iTempData;

	lTotalCount = m_cDropItemSelectList.GetCount();

	if( lTotalCount > 0 )
	{
		m_cDropItemSelectEdit.GetWindowText( strBuffer, sizeof(strBuffer) );

		for( int iCount=0; iCount<lTotalCount; iCount++ )
		{
			iData = m_cDropItemSelectList.GetItemData( iCount );

			if( iData == atoi( strBuffer ) )
			{ 
				m_cDropItemSelectList.SetCurSel( iCount );
			}
		}
	}
}
*/

