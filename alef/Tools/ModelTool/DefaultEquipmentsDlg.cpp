// DefaultEquipmentsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "DefaultEquipmentsDlg.h"
#include "ItemTemplateListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefaultEquipmentsDlg dialog


CDefaultEquipmentsDlg::CDefaultEquipmentsDlg(AgpdItemADCharTemplate *pcsAgpdItemADCharTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(CDefaultEquipmentsDlg::IDD, pParent)
{
	CHAR aszTemplateName[AGPMITEM_PART_NUM][AMT_MAX_STR];
	memset(aszTemplateName, 0, sizeof(CHAR) * AGPMITEM_PART_NUM * AMT_MAX_STR);

	if( CModelToolApp::GetInstance() )
	{
		AgpmItem* pcsAgpmItem = CModelToolApp::GetInstance()->GetEngine()->GetAgpmItemModule();
		if( pcsAgpmItem )
		{
			for(INT32 lPartID = AGPMITEM_PART_BODY; lPartID < AGPMITEM_PART_NUM; ++lPartID)
			{
				if( pcsAgpdItemADCharTemplate->m_lDefaultEquipITID[lPartID] )
				{
					AgpdItemTemplate* pcsAgpdItemTemplate = pcsAgpmItem->GetItemTemplate( pcsAgpdItemADCharTemplate->m_lDefaultEquipITID[lPartID]) ;
					if( !pcsAgpdItemTemplate )	continue;

					m_astrEquipment[lPartID] = pcsAgpdItemTemplate->m_szName;
				}
			}
		}
	}

	m_pcsAgpdItemADCharTemplate = pcsAgpdItemADCharTemplate;
	memcpy( &m_csTempData, pcsAgpdItemADCharTemplate, sizeof(AgpdItemADCharTemplate) );
}


void CDefaultEquipmentsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefaultEquipmentsDlg)
	DDX_Text(pDX, IDC_EDIT_DE_AMULET, m_astrEquipment[AGPMITEM_PART_ACCESSORY_NECKLACE] );	//m_strAmulet
	DDX_Text(pDX, IDC_EDIT_DE_ARMS, m_astrEquipment[AGPMITEM_PART_ARMS] );
	DDX_Text(pDX, IDC_EDIT_DE_BODY, m_astrEquipment[AGPMITEM_PART_BODY] );
	DDX_Text(pDX, IDC_EDIT_DE_CLOAK, m_astrEquipment[AGPMITEM_PART_ARMS2] );
	DDX_Text(pDX, IDC_EDIT_DE_FOOT, m_astrEquipment[AGPMITEM_PART_FOOT] );
	DDX_Text(pDX, IDC_EDIT_DE_HANDS, m_astrEquipment[AGPMITEM_PART_HANDS] );
	DDX_Text(pDX, IDC_EDIT_DE_HEAD, m_astrEquipment[AGPMITEM_PART_HEAD] );
	DDX_Text(pDX, IDC_EDIT_DE_LEGS, m_astrEquipment[AGPMITEM_PART_LEGS] );
	DDX_Text(pDX, IDC_EDIT_DE_RING1, m_astrEquipment[AGPMITEM_PART_ACCESSORY_RING1] );
	DDX_Text(pDX, IDC_EDIT_DE_RING2, m_astrEquipment[AGPMITEM_PART_ACCESSORY_RING2] );
	DDX_Text(pDX, IDC_EDIT_DE_SHIELD, m_astrEquipment[AGPMITEM_PART_HAND_LEFT] );	//m_strShield
	DDX_Text(pDX, IDC_EDIT_DE_WEAPON, m_astrEquipment[AGPMITEM_PART_HAND_RIGHT] );	//m_strWeapon

	DDX_Text(pDX, IDC_EDIT_DE_AMULET2, m_astrEquipment[AGPMITEM_PART_V_ACCESSORY_NECKLACE] );	//m_strAmulet
	DDX_Text(pDX, IDC_EDIT_DE_ARMS2, m_astrEquipment[AGPMITEM_PART_V_ARMS] );
	DDX_Text(pDX, IDC_EDIT_DE_BODY2, m_astrEquipment[AGPMITEM_PART_V_BODY] );
	DDX_Text(pDX, IDC_EDIT_DE_CLOAK2, m_astrEquipment[AGPMITEM_PART_V_ARMS2] );
	DDX_Text(pDX, IDC_EDIT_DE_FOOT2, m_astrEquipment[AGPMITEM_PART_V_FOOT] );
	DDX_Text(pDX, IDC_EDIT_DE_HANDS2, m_astrEquipment[AGPMITEM_PART_V_HANDS] );
	DDX_Text(pDX, IDC_EDIT_DE_HEAD2, m_astrEquipment[AGPMITEM_PART_V_HEAD] );
	DDX_Text(pDX, IDC_EDIT_DE_LEGS2, m_astrEquipment[AGPMITEM_PART_V_LEGS] );
	DDX_Text(pDX, IDC_EDIT_DE_RING3, m_astrEquipment[AGPMITEM_PART_V_ACCESSORY_RING1] );
	DDX_Text(pDX, IDC_EDIT_DE_RING4, m_astrEquipment[AGPMITEM_PART_V_ACCESSORY_RING2] );
	DDX_Text(pDX, IDC_EDIT_DE_SHIELD2, m_astrEquipment[AGPMITEM_PART_V_HAND_LEFT] );	//m_strShield
	DDX_Text(pDX, IDC_EDIT_DE_WEAPON2, m_astrEquipment[AGPMITEM_PART_V_HAND_RIGHT] );	//m_strWeapon
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefaultEquipmentsDlg, CDialog)
	//{{AFX_MSG_MAP(CDefaultEquipmentsDlg)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_AMULET, OnButtonDeSetAmulet)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_ARMS, OnButtonDeSetArms)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_BODY, OnButtonDeSetBody)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_CLOAK, OnButtonDeSetCloak)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_FOOT, OnButtonDeSetFoot)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_HANDS, OnButtonDeSetHands)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_HEAD, OnButtonDeSetHead)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_LEGS, OnButtonDeSetLegs)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_RING1, OnButtonDeSetRing1)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_RING2, OnButtonDeSetRing2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_SHIELD, OnButtonDeSetShield)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_WEAPON, OnButtonDeSetWeapon)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_AMULET, OnButtonDeDelAmulet)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_ARMS, OnButtonDeDelArms)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_BODY, OnButtonDeDelBody)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_CLOAK, OnButtonDeDelCloak)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_FOOT, OnButtonDeDelFoot)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_HANDS, OnButtonDeDelHands)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_HEAD, OnButtonDeDelHead)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_LEGS, OnButtonDeDelLegs)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_RING1, OnButtonDeDelRing1)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_RING2, OnButtonDeDelRing2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_SHIELD, OnButtonDeDelShield)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_WEAPON, OnButtonDeDelWeapon)

	ON_BN_CLICKED(IDC_BUTTON_DE_SET_AMULET2, OnButtonDeSetAmulet2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_ARMS2, OnButtonDeSetArms2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_BODY2, OnButtonDeSetBody2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_CLOAK2, OnButtonDeSetCloak2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_FOOT2, OnButtonDeSetFoot2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_HANDS2, OnButtonDeSetHands2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_HEAD2, OnButtonDeSetHead2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_LEGS2, OnButtonDeSetLegs2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_RING3, OnButtonDeSetRing3)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_RING4, OnButtonDeSetRing4)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_SHIELD2, OnButtonDeSetShield2)
	ON_BN_CLICKED(IDC_BUTTON_DE_SET_WEAPON2, OnButtonDeSetWeapon2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_AMULET2, OnButtonDeDelAmulet2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_ARMS2, OnButtonDeDelArms2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_BODY2, OnButtonDeDelBody2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_CLOAK2, OnButtonDeDelCloak2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_FOOT2, OnButtonDeDelFoot2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_HANDS2, OnButtonDeDelHands2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_HEAD2, OnButtonDeDelHead2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_LEGS2, OnButtonDeDelLegs2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_RING3, OnButtonDeDelRing3)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_RING4, OnButtonDeDelRing4)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_SHIELD2, OnButtonDeDelShield2)
	ON_BN_CLICKED(IDC_BUTTON_DE_DEL_WEAPON2, OnButtonDeDelWeapon2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefaultEquipmentsDlg message handlers

void	CDefaultEquipmentsDlg::UpdateEquip( INT32 lEqipID )
{
	CItemTemplateListDlg dlg( &m_csTempData.m_lDefaultEquipITID[ lEqipID ] );
	dlg.SetListFilter( lEqipID );
	if( IDOK != dlg.DoModal() )	return;

	AgpmItem* pcsAgpmItem = CModelToolApp::GetInstance()->GetEngine()->GetAgpmItemModule();
	if( !pcsAgpmItem )			return;

	AgpdItemTemplate *pcsAgpdItemTemplate = pcsAgpmItem->GetItemTemplate( m_csTempData.m_lDefaultEquipITID[lEqipID] );
	if( !pcsAgpdItemTemplate )	return;

	m_astrEquipment[lEqipID] = pcsAgpdItemTemplate->m_szName;
	UpdateData(FALSE);
}

void	CDefaultEquipmentsDlg::DeleteEquip( INT32 lEquipID )
{
	m_csTempData.m_lDefaultEquipITID[lEquipID] = 0;
	m_astrEquipment[lEquipID] = "";
	UpdateData(FALSE);
}

void CDefaultEquipmentsDlg::OnOK() 
{
	// TODO: Add extra validation here
	memcpy(m_pcsAgpdItemADCharTemplate, &m_csTempData, sizeof(AgpdItemADCharTemplate));
	CDialog::OnOK();
}