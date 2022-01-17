#include "stdafx.h"
#include <math.h>
#include "../resource.h"
#include "AgcmItemDlg.h"
#include "ItemDlg.h"
#include "AgcmFileListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemDlg dialog

CItemDlg::CItemDlg(AgpdItemTemplate *pcsAgpdItemTemplae, AgcdItemTemplate *pcsAgcdItemTemplate, CWnd* pParent )
	: CDialog(CItemDlg::IDD, pParent)
	, m_strFirstColor(_T("000000"))
	, m_strSecondColor(_T("000000"))
	, m_uGap(1000)
{
	//{{AFX_DATA_INIT(CItemDlg)
	m_bWithFace			= pcsAgcdItemTemplate->m_bWithFace;
	m_bWithHair			= pcsAgcdItemTemplate->m_bWithHair;
	m_bEquipTwoHands	= pcsAgcdItemTemplate->m_bEquipTwoHands;
	m_strBaseClump		= pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName ? _T(pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName) : _T("");
	m_strFieldClump		= pcsAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName ? _T(pcsAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName) : _T("");
	m_strSecondClump	= pcsAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName ? _T(pcsAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName) : _T("");
	m_strUIImage		= pcsAgcdItemTemplate->m_pcsPreData->m_pszTextureName ? _T(pcsAgcdItemTemplate->m_pcsPreData->m_pszTextureName) : _T("");
	m_strTemplateName	= _T(pcsAgpdItemTemplae->m_szName);
	m_unStackableNum	= pcsAgpdItemTemplae->m_lMaxStackableCount;
	m_strPickClump		= pcsAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName ? _T(pcsAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName) : _T("");
	m_strSmallUIImage	= pcsAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName ? _T(pcsAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName): _T("");
	m_fBSphereCenterX	= pcsAgcdItemTemplate->m_stBSphere.center.x;
	m_fBSphereCenterY	= pcsAgcdItemTemplate->m_stBSphere.center.y;
	m_fBSphereCenterZ	= pcsAgcdItemTemplate->m_stBSphere.center.z;
	m_fBSphereRadius	= pcsAgcdItemTemplate->m_stBSphere.radius;
	//}}AFX_DATA_INIT

	m_pcsAgpdItemTemplate = pcsAgpdItemTemplae;
	m_pcsAgcdItemTemplate = pcsAgcdItemTemplate;

	m_bWeaponTypeSelectable = false;
}

void CItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ITEM_SPIRIT, m_csItemSpiritStone);
	DDX_Control(pDX, IDC_COMBO_ITEM_USABLE_TYPE, m_csItemUsable);
	DDX_Control(pDX, IDC_COMBO_ITEM_OTHER_TYPE, m_csItemOther);
	DDX_Control(pDX, IDC_COMBO_ITEM_WTYPE, m_csItemWeaponType);
	DDX_Control(pDX, IDC_COMBO_ITEM_TYPE, m_csItemType);
	DDX_Control(pDX, IDC_COMBO_ITEM_PART, m_csItemPart);
	DDX_Control(pDX, IDC_COMBO_ITEM_KIND, m_csItemKind);
	DDX_Check(pDX, IDC_CHECK_EQUIP_TWO_HANDS, m_bEquipTwoHands);
	DDX_Check(pDX, IDC_ITEM_WITH_FACE, m_bWithFace);
	DDX_Check(pDX, IDC_ITEM_WITH_HAIR, m_bWithHair);
	DDX_Check(pDX, IDC_AVATAR_ITEM, m_bAvatarItem);
	DDX_Text(pDX, IDC_EDIT_BASE_CLUMP, m_strBaseClump);
	DDX_Text(pDX, IDC_EDIT_FIELD_CLUMP, m_strFieldClump);
	DDX_Text(pDX, IDC_EDIT_SECOND_CLUMP, m_strSecondClump);
	DDX_Text(pDX, IDC_EDIT_UI_IMAGE, m_strUIImage);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_NAME, m_strTemplateName);
	DDX_Text(pDX, IDC_EDIT_STACKABLE_NUM, m_unStackableNum);
	DDX_Text(pDX, IDC_EDIT_PICK_CLUMP, m_strPickClump);
	DDX_Text(pDX, IDC_EDIT_SMALL_UI_IMAGE, m_strSmallUIImage);
	DDX_Text(pDX, IDC_EDIT_BSPHERE_CENTER_X, m_fBSphereCenterX);
	DDX_Text(pDX, IDC_EDIT_BSPHERE_CENTER_Y, m_fBSphereCenterY);
	DDX_Text(pDX, IDC_EDIT_BSPHERE_CENTER_Z, m_fBSphereCenterZ);
	DDX_Text(pDX, IDC_EDIT_BSPHERE_RADIUS, m_fBSphereRadius);
	DDX_Control(pDX, IDC_LIGHT_SYSTEM, m_ctlLightSystem);
	DDX_Text(pDX, IDC_EDIT_LIGHT_FIRST_COLOR, m_strFirstColor);
	DDX_Text(pDX, IDC_EDIT_LIGHT_SECOND_COLOR, m_strSecondColor);
	DDX_Text(pDX, IDC_EDIT_LIGHT_GAP, m_uGap);
}

BEGIN_MESSAGE_MAP(CItemDlg, CDialog)
	//{{AFX_MSG_MAP(CItemDlg)
	ON_BN_CLICKED(IDC_BUTTON_SET_BASE_CLUMP, OnButtonSetBaseClump)
	ON_BN_CLICKED(IDC_BUTTON_SET_FIELD_CLUMP, OnButtonSetFieldClump)
	ON_BN_CLICKED(IDC_BUTTON_SET_SECOND_CLUMP, OnButtonSetSecondClump)
	ON_BN_CLICKED(IDC_BUTTON_SET_SMALL_UI_IMAGE, OnButtonSetSmallUiImage)
	ON_BN_CLICKED(IDC_BUTTON_SET_UI_IMAGE, OnButtonSetUiImage)
	ON_CBN_SELCHANGE(IDC_COMBO_ITEM_TYPE, OnSelchangeComboItemType)
	ON_CBN_SELCHANGE(IDC_COMBO_ITEM_PART, OnSelchangeComboItemPart)
	ON_CBN_SELCHANGE(IDC_COMBO_ITEM_KIND, OnSelchangeComboItemKind)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_BASE_CLUMP, OnButtonRemoveBaseClump)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_FIELD_CLUMP, OnButtonRemoveFieldClump)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_SECOND_CLUMP, OnButtonRemoveSecondClump)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_SMALL_UI_IMAGE, OnButtonRemoveSmallUiImage)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_UI_IMAGE, OnButtonRemoveUiImage)
	ON_CBN_SELCHANGE(IDC_COMBO_ITEM_USABLE_TYPE, OnSelchangeComboItemUsableType)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_PICK_CLUMP, OnButtonRemovePickClump)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE_BSPHERE, OnButtonGenerateBsphere)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemDlg message handlers

void CItemDlg::OnButtonSetBaseClump() 
{
	if(AgcmItemDlg::GetInstance())
	{
		AgcmFileListDlg dlg;
		CHAR* pszTemp = dlg.OpenFileList( AgcmItemDlg::GetInstance()->GetClumpPathName1(), AgcmItemDlg::GetInstance()->GetClumpPathName2(), NULL );
		if( pszTemp )
			m_strBaseClump = pszTemp;

		UpdateData(FALSE);
	}
}

void CItemDlg::OnButtonSetFieldClump() 
{
	if( AgcmItemDlg::GetInstance() )
	{
		AgcmFileListDlg dlg;
		CHAR* pszTemp = dlg.OpenFileList( AgcmItemDlg::GetInstance()->GetClumpPathName1(), AgcmItemDlg::GetInstance()->GetClumpPathName2(), NULL );
		if( pszTemp )
			m_strFieldClump = pszTemp;

		UpdateData(FALSE);
	}
}

void CItemDlg::OnButtonSetSecondClump() 
{
	if(AgcmItemDlg::GetInstance())
	{
		AgcmFileListDlg dlg;
		CHAR* pszTemp = dlg.OpenFileList( AgcmItemDlg::GetInstance()->GetClumpPathName1(), AgcmItemDlg::GetInstance()->GetClumpPathName2(), NULL );
		if( pszTemp )
			m_strSecondClump = pszTemp;

		UpdateData(FALSE);
	}
}

void CItemDlg::OnButtonSetSmallUiImage() 
{
	if(AgcmItemDlg::GetInstance())
	{
		AgcmFileListDlg dlg;
		CHAR* pszTemp = dlg.OpenFileList(	AgcmItemDlg::GetInstance()->GetTexturePathName1(), AgcmItemDlg::GetInstance()->GetTexturePathName2(), AgcmItemDlg::GetInstance()->GetTexturePathName3() );
		if( pszTemp )
			m_strSmallUIImage = pszTemp;

		UpdateData(FALSE);
	}
}

void CItemDlg::OnButtonSetUiImage() 
{
	if(AgcmItemDlg::GetInstance())
	{
		AgcmFileListDlg dlg;
		CHAR* pszTemp = dlg.OpenFileList( AgcmItemDlg::GetInstance()->GetTexturePathName1(), AgcmItemDlg::GetInstance()->GetTexturePathName2(), AgcmItemDlg::GetInstance()->GetTexturePathName3() );
		if( pszTemp )
			m_strUIImage = pszTemp;

		UpdateData(FALSE);
	}
}

void CItemDlg::OnOK() 
{
	UpdateData(TRUE);

	strcpy(m_pcsAgpdItemTemplate->m_szName, (char*)m_strTemplateName.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName, (char*)m_strBaseClump.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName, (char*)m_strSecondClump.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName, (char*)m_strFieldClump.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName, (char*)m_strPickClump.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdItemTemplate->m_pcsPreData->m_pszTextureName, (char*)m_strUIImage.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName, (char*)m_strSmallUIImage.GetBuffer() );

	m_pcsAgcdItemTemplate->m_bWithHair			= m_bWithHair;
	m_pcsAgcdItemTemplate->m_bWithFace			= m_bWithFace;
	m_pcsAgcdItemTemplate->m_bEquipTwoHands		= m_bEquipTwoHands;
	m_pcsAgpdItemTemplate->m_bStackable			= m_unStackableNum ? TRUE : FALSE;
	m_pcsAgpdItemTemplate->m_lMaxStackableCount	= m_unStackableNum;

	m_pcsAgpdItemTemplate->m_nType = (AgpmItemType)(m_csItemType.GetItemData(m_csItemType.GetCurSel()));
	if (m_pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP)
	{
		AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip = (AgpdItemTemplateEquip *)(m_pcsAgpdItemTemplate);

		pcsAgpdItemTemplateEquip->m_nPart = (AgpmItemPart)(m_csItemPart.GetItemData(m_csItemPart.GetCurSel()));
		if( m_bAvatarItem )
		{
			pcsAgpdItemTemplateEquip->m_nPart = pcsAgpdItemTemplateEquip->GetAvatarPartIndex();
		}

		pcsAgpdItemTemplateEquip->m_nKind = (AgpmItemEquipKind)(m_csItemKind.GetItemData(m_csItemKind.GetCurSel()));

		if (pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
		{
			if ( m_bWeaponTypeSelectable )
			{
				AgpdItemTemplateEquipWeapon *pcsAgpdItemTemplateEquipWeapon = (AgpdItemTemplateEquipWeapon *)(pcsAgpdItemTemplateEquip);
				pcsAgpdItemTemplateEquipWeapon->m_nWeaponType = (AgpmItemEquipWeaponType)(m_csItemWeaponType.GetItemData(m_csItemWeaponType.GetCurSel()));
			}
		}

		pcsAgpdItemTemplateEquip->m_lightInfo.eType = ( AuCharacterLightInfo::TYPE ) m_ctlLightSystem.GetItemData( m_ctlLightSystem.GetCurSel() );
		if( pcsAgpdItemTemplateEquip->m_lightInfo.GetType() )
		{
			DWORD uRGB;
			sscanf( (LPCTSTR) m_strFirstColor , "%X" , &uRGB );
			ReadUINT32ToRGB( uRGB , &pcsAgpdItemTemplateEquip->m_lightInfo.r1 , &pcsAgpdItemTemplateEquip->m_lightInfo.g1 , &pcsAgpdItemTemplateEquip->m_lightInfo.b1 );
			sscanf( (LPCTSTR) m_strSecondColor , "%X" , &uRGB );
			ReadUINT32ToRGB( uRGB , &pcsAgpdItemTemplateEquip->m_lightInfo.r2 , &pcsAgpdItemTemplateEquip->m_lightInfo.g2 , &pcsAgpdItemTemplateEquip->m_lightInfo.b2 );
			
			pcsAgpdItemTemplateEquip->m_lightInfo.uChangeGap = m_uGap;
		}
	}
	else if (m_pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
	{
		AgpdItemTemplateUsable *pstUsable = (AgpdItemTemplateUsable *)(m_pcsAgpdItemTemplate);
		pstUsable->m_nUsableItemType = m_csItemUsable.GetItemData(m_csItemUsable.GetCurSel());
		if (pstUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
		{
			AgpdItemTemplateUsableSpiritStone *pstSS = (AgpdItemTemplateUsableSpiritStone *)(pstUsable);
			pstSS->m_eSpiritStoneType = (AgpmItemUsableSpiritStoneType)(m_csItemSpiritStone.GetItemData(m_csItemSpiritStone.GetCurSel()));
		}
	}
	else if (m_pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_OTHER)
	{
		AgpdItemTemplateOther *pstOther = (AgpdItemTemplateOther *)(m_pcsAgpdItemTemplate);
		pstOther->m_eOtherItemType = (AgpmItemOtherType)(m_csItemOther.GetItemData(m_csItemOther.GetCurSel()));
	}

	m_pcsAgcdItemTemplate->m_stBSphere.center.x	= m_fBSphereCenterX;
	m_pcsAgcdItemTemplate->m_stBSphere.center.y = m_fBSphereCenterY;
	m_pcsAgcdItemTemplate->m_stBSphere.center.z = m_fBSphereCenterZ;
	m_pcsAgcdItemTemplate->m_stBSphere.radius	= m_fBSphereRadius;

	CDialog::OnOK();
}

void CItemDlg::OnSelchangeComboItemType()
{
	// TODO: Add your control notification handler code here
//	if(m_csItemType.GetCurSel() == AGPMITEM_TYPE_EQUIP)
	DWORD dwData = m_csItemType.GetItemData(m_csItemType.GetCurSel());

	m_csItemWeaponType.EnableWindow(FALSE);

	if(dwData == AGPMITEM_TYPE_EQUIP)
	{
		m_csItemPart.EnableWindow(TRUE);
		m_csItemKind.EnableWindow(TRUE);

		if(m_csItemKind.GetItemData(m_csItemKind.GetCurSel()) == AGPMITEM_EQUIP_KIND_WEAPON)
		{
			if ( m_bWeaponTypeSelectable )
				m_csItemWeaponType.EnableWindow(TRUE);
		}

		m_csItemUsable.EnableWindow(FALSE);
		m_csItemOther.EnableWindow(FALSE);
		m_csItemSpiritStone.EnableWindow(FALSE);
	}
	else if(dwData == AGPMITEM_TYPE_USABLE)
	{
		m_csItemUsable.EnableWindow(TRUE);

		if(m_csItemUsable.GetItemData(m_csItemUsable.GetCurSel()) == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
		{
			m_csItemSpiritStone.EnableWindow(TRUE);
		}
		else
		{
			m_csItemSpiritStone.EnableWindow(FALSE);
		}

		m_csItemOther.EnableWindow(FALSE);
		m_csItemPart.EnableWindow(FALSE);
		m_csItemKind.EnableWindow(FALSE);		
	}
	else if(dwData == AGPMITEM_TYPE_OTHER)
	{
		m_csItemOther.EnableWindow(TRUE);

		m_csItemUsable.EnableWindow(FALSE);
		m_csItemSpiritStone.EnableWindow(FALSE);
		m_csItemPart.EnableWindow(FALSE);
		m_csItemKind.EnableWindow(FALSE);
	}
}

static const char* s_szUsableType[AGPMITEM_USABLE_TYPE_NUM] = 
{
	"POTION",
	"BRANK",
	"SPIRIT_STONE",
	"SOUL_CUBE",
	"TELEPORT_SCROLL",
	"ARROW",
	"BOLT",
	"TRANSFORM",
	"CONVERT_CATALYST",
	"RUNE",
	"RECEIPE",
	"SKILL_BOOK",
	"SKILL_SCROLL",
	"SKILLROLLBACK_SCROLL",
	"REVERSE_ORB",
	"RECALL_PARTY",
	"MAP",
	"LOTTERY_BOX",
	"LOTTERY_KEY",
	"ANTI_DROP",
	"AREA_CHATTING",
	"ADD_BANK_SLOT",
	"USE_EFFECT",
	"CHATTING",
	"JUMP",
	"LUCKY_SCROLL",
	"NICK",
	"SKILL_INIT",
	"SOCKET_INIT",
	"ANY_SUNDRY",
	"ANY_BANK",
	"AVATAR",
	"CUSTOMIZE",
	"QUEST",
	"PET_FOOD"
};

BOOL CItemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	INT32	lEquipKindIndex;

	m_csItemType.SetItemData(m_csItemType.InsertString(-1, ITEM_DLG_TYPE_EQUIP),						AGPMITEM_TYPE_EQUIP);
	m_csItemType.SetItemData(m_csItemType.InsertString(-1, ITEM_DLG_TYPE_USABLE),						AGPMITEM_TYPE_USABLE);
	m_csItemType.SetItemData(m_csItemType.InsertString(-1, ITEM_DLG_TYPE_OTHER),						AGPMITEM_TYPE_OTHER);

	for( int i=AGPMITEM_PART_BODY; i<AGPMITEM_PART_V_BODY; ++i )
		m_csItemPart.SetItemData(m_csItemPart.InsertString( -1, g_aszWeaponKindName[i] ), i );

	m_csItemKind.SetItemData(m_csItemKind.InsertString(-1, ITEM_DLG_KIND_ARMOUR),						AGPMITEM_EQUIP_KIND_ARMOUR);
	m_csItemKind.SetItemData(m_csItemKind.InsertString(-1, ITEM_DLG_KIND_WEAPON),						AGPMITEM_EQUIP_KIND_WEAPON);
	m_csItemKind.SetItemData(m_csItemKind.InsertString(-1, ITEM_DLG_KIND_SHIELD),						AGPMITEM_EQUIP_KIND_SHIELD);
	m_csItemKind.SetItemData(m_csItemKind.InsertString(-1, ITEM_DLG_KIND_RING),							AGPMITEM_EQUIP_KIND_RING);
	m_csItemKind.SetItemData(m_csItemKind.InsertString(-1, ITEM_DLG_KIND_NECKLACE),						AGPMITEM_EQUIP_KIND_NECKLACE);
	m_csItemKind.SetItemData(m_csItemKind.InsertString(-1, ITEM_DLG_KIND_RIDE),							AGPMITEM_EQUIP_KIND_RIDE);

	for( int i=0; i<AGPMITEM_USABLE_TYPE_NUM; ++i )
		m_csItemUsable.SetItemData( m_csItemUsable.InsertString( -1, s_szUsableType[i] ), i );

	m_csItemOther.SetItemData(m_csItemOther.InsertString(-1, ITEM_DLG_OTHER_SKULL),						AGPMITEM_OTHER_TYPE_SKULL);
	m_csItemOther.SetItemData(m_csItemOther.InsertString(-1, ITEM_DLG_OTHER_MONEY),						AGPMITEM_OTHER_TYPE_MONEY);

	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_FIRE),				AGPMITEM_USABLE_SS_TYPE_FIRE);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_AIR),				AGPMITEM_USABLE_SS_TYPE_AIR);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_WATER),			AGPMITEM_USABLE_SS_TYPE_WATER);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_EARTH),			AGPMITEM_USABLE_SS_TYPE_EARTH);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_MAGIC),			AGPMITEM_USABLE_SS_TYPE_MAGIC);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_ICE),				AGPMITEM_USABLE_SS_TYPE_ICE);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_LIGHTENING),		AGPMITEM_USABLE_SS_TYPE_LIGHTENING);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_POISON),			AGPMITEM_USABLE_SS_TYPE_POISON);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_CON),				AGPMITEM_USABLE_SS_TYPE_CON);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_WIS),				AGPMITEM_USABLE_SS_TYPE_WIS);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_DEX),				AGPMITEM_USABLE_SS_TYPE_DEX);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_STR),				AGPMITEM_USABLE_SS_TYPE_STR);
	m_csItemSpiritStone.SetItemData(m_csItemSpiritStone.InsertString(-1, ITEM_DLG_SS_INT),				AGPMITEM_USABLE_SS_TYPE_INT);

	m_csItemSpiritStone.SetCurSel(0);
	m_csItemUsable.SetCurSel(0);
	m_csItemOther.SetCurSel(0);
	m_csItemType.SetCurSel(0);
	m_csItemPart.SetCurSel(0);
	m_csItemKind.SetCurSel(0);

	for( int i=AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_SWORD; i<AGPMITEM_EQUIP_WEAPON_TYPE_NUM; ++i )
		m_csItemWeaponType.SetItemData(m_csItemWeaponType.InsertString( -1, g_aszWeaponTypeName[i] ), i );

	if(	AGPMITEM_TYPE_EQUIP <= m_pcsAgpdItemTemplate->m_nType && AGPMITEM_TYPE_NUM > m_pcsAgpdItemTemplate->m_nType )		
		m_csItemType.SetCurSel(m_pcsAgpdItemTemplate->m_nType);

	INT32 lItemType = m_csItemType.GetCurSel();
	if(lItemType == AGPMITEM_TYPE_EQUIP)
	{
		GetDlgItem( IDC_AVATAR_ITEM )->EnableWindow( TRUE );

		AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip = (AgpdItemTemplateEquip *)(m_pcsAgpdItemTemplate);
		if( pcsAgpdItemTemplateEquip->IsAvatarEquip() )
		{
			m_bAvatarItem = TRUE;
			UpdateData( FALSE );
		}

		if(	(pcsAgpdItemTemplateEquip->GetPartIndex() >= AGPMITEM_PART_BODY) &&
			(pcsAgpdItemTemplateEquip->GetPartIndex() < AGPMITEM_PART_NUM)		)
		{
			m_csItemPart.SetCurSel(pcsAgpdItemTemplateEquip->GetPartIndex() - AGPMITEM_PART_BODY);
		}

		lEquipKindIndex	= (INT32)(log10l(pcsAgpdItemTemplateEquip->m_nKind) / log10l((double)(2.0f))); // 2ÀÇ lEquipKindIndex½ÂÀº pcsAgpdItemTemplateEquip->m_nKind;
		m_csItemKind.SetCurSel(lEquipKindIndex);

		if(pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
		{
			AgpdItemTemplateEquipWeapon *pcsAgpdItemTemplateEquipWeapon = (AgpdItemTemplateEquipWeapon *)(pcsAgpdItemTemplateEquip);
			if ( m_bWeaponTypeSelectable )
				m_csItemWeaponType.SetCurSel(pcsAgpdItemTemplateEquipWeapon->m_nWeaponType);
		}
		else
		{
			if ( m_bWeaponTypeSelectable )
				m_csItemWeaponType.EnableWindow(FALSE);
		}

		m_csItemOther.EnableWindow(FALSE);
		m_csItemUsable.EnableWindow(FALSE);
		m_csItemSpiritStone.EnableWindow(FALSE);

		m_ctlLightSystem.SetItemData(m_ctlLightSystem.InsertString(-1, "No Lighting"	),	AuCharacterLightInfo::NO_VARIATION);
		m_ctlLightSystem.SetItemData(m_ctlLightSystem.InsertString(-1, "Ambient Plus"	),	AuCharacterLightInfo::AMBIENT_PLUS);
		m_ctlLightSystem.SetItemData(m_ctlLightSystem.InsertString(-1, "Ambient Minus"	),	AuCharacterLightInfo::AMBIENT_MINUS);

		m_ctlLightSystem.SetCurSel(pcsAgpdItemTemplateEquip->m_lightInfo.GetType() );

		if( pcsAgpdItemTemplateEquip->m_lightInfo.GetType() )
		{
			DWORD	uRGB;

			uRGB = MakeRGBToUINT32( pcsAgpdItemTemplateEquip->m_lightInfo.r1 , pcsAgpdItemTemplateEquip->m_lightInfo.g1 , pcsAgpdItemTemplateEquip->m_lightInfo.b1 );
			m_strFirstColor.Format( "%X" , uRGB );
			uRGB = MakeRGBToUINT32( pcsAgpdItemTemplateEquip->m_lightInfo.r2 , pcsAgpdItemTemplateEquip->m_lightInfo.g2 , pcsAgpdItemTemplateEquip->m_lightInfo.b2 );
			m_strSecondColor.Format( "%X" , uRGB );
			m_uGap = pcsAgpdItemTemplateEquip->m_lightInfo.uChangeGap;

			UpdateData( FALSE );
		}
	}
	else if(lItemType == AGPMITEM_TYPE_USABLE)
	{
		AgpdItemTemplateUsable *pstUsable = (AgpdItemTemplateUsable *)(m_pcsAgpdItemTemplate);

		if((pstUsable->m_nUsableItemType >= AGPMITEM_USABLE_TYPE_POTION) && (pstUsable->m_nUsableItemType < AGPMITEM_USABLE_TYPE_NUM))
		{
			m_csItemUsable.SetCurSel(pstUsable->m_nUsableItemType);

			if(pstUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
			{
				AgpdItemTemplateUsableSpiritStone *pstSStone = (AgpdItemTemplateUsableSpiritStone *)(pstUsable);
				if((pstSStone->m_eSpiritStoneType >= 0) && (pstSStone->m_eSpiritStoneType < AGPMITEM_USABLE_SS_TYPE_NUM))
				{
					m_csItemSpiritStone.SetCurSel((INT32)(pstSStone->m_eSpiritStoneType));
				}
			}
			else
			{
				m_csItemSpiritStone.EnableWindow(FALSE);
			}
		}
		else
		{
			m_csItemSpiritStone.EnableWindow(FALSE);
		}

		m_csItemPart.EnableWindow(FALSE);
		m_csItemKind.EnableWindow(FALSE);

		if ( m_bWeaponTypeSelectable )
			m_csItemWeaponType.EnableWindow(FALSE);

		m_csItemOther.EnableWindow(FALSE);
	}
	else if(lItemType == AGPMITEM_TYPE_OTHER)
	{
		AgpdItemTemplateOther *pstOther = (AgpdItemTemplateOther *)(m_pcsAgpdItemTemplate);

		if((pstOther->m_eOtherItemType >= AGPMITEM_OTHER_TYPE_SKULL) && (pstOther->m_eOtherItemType < AGPMITEM_OTHER_TYPE_NUM))
		{
			m_csItemOther.SetCurSel(pstOther->m_eOtherItemType);
		}

		m_csItemPart.EnableWindow(FALSE);
		m_csItemKind.EnableWindow(FALSE);
		
		if ( m_bWeaponTypeSelectable )
			m_csItemWeaponType.EnableWindow(FALSE);

		m_csItemUsable.EnableWindow(FALSE);
		m_csItemSpiritStone.EnableWindow(FALSE);
	}

	if ( m_bWeaponTypeSelectable == FALSE )
		m_csItemWeaponType.EnableWindow(FALSE);	

	return TRUE;
}

void CItemDlg::OnSelchangeComboItemPart() 
{
}

void CItemDlg::OnSelchangeComboItemKind() 
{
	// TODO: Add your control notification handler code here
	if(m_csItemKind.GetItemData(m_csItemKind.GetCurSel()) == AGPMITEM_EQUIP_KIND_WEAPON)
	{
		if ( m_bWeaponTypeSelectable )
			m_csItemWeaponType.EnableWindow(TRUE);
	}
	else
	{
		if ( m_bWeaponTypeSelectable )
			m_csItemWeaponType.EnableWindow(FALSE);
	}
}

void CItemDlg::OnButtonRemoveBaseClump() 
{
	// TODO: Add your control notification handler code here
	m_strBaseClump	= "";
	UpdateData(FALSE);
}

void CItemDlg::OnButtonRemoveFieldClump() 
{
	// TODO: Add your control notification handler code here
	m_strFieldClump	= "";
	UpdateData(FALSE);
}

void CItemDlg::OnButtonRemoveSecondClump() 
{
	// TODO: Add your control notification handler code here
	m_strSecondClump	= "";
	UpdateData(FALSE);
}

void CItemDlg::OnButtonRemoveSmallUiImage() 
{
	// TODO: Add your control notification handler code here
//	m_strSmallUIImage	= "";
//	UpdateData(FALSE);
}

void CItemDlg::OnButtonRemoveUiImage() 
{
	// TODO: Add your control notification handler code here
	m_strUIImage	= "";
	UpdateData(FALSE);
}

void CItemDlg::OnSelchangeComboItemUsableType() 
{
	// TODO: Add your control notification handler code here
	if(m_csItemUsable.GetItemData(m_csItemUsable.GetCurSel()) == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
	{
		m_csItemSpiritStone.EnableWindow(TRUE);
	}
	else
	{
		m_csItemSpiritStone.EnableWindow(FALSE);
	}	
}

void CItemDlg::OnButtonRemovePickClump() 
{
	// TODO: Add your control notification handler code here
	m_strPickClump	= "";
	UpdateData(FALSE);
}

void CItemDlg::OnButtonGenerateBsphere() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmItemDlg::GetInstance())
		return;

	RwSphere	stBSphere;
	if (!AgcmItemDlg::GetInstance()->EnumCallbackSetItemData(AGCMITEMDLG_CB_ID_SET_BSPHERE, (PVOID)(&stBSphere)))
		return;

	m_fBSphereCenterX	= stBSphere.center.x;
	m_fBSphereCenterY	= stBSphere.center.y;
	m_fBSphereCenterZ	= stBSphere.center.z;
	m_fBSphereRadius	= stBSphere.radius;

	UpdateData(FALSE);
}

