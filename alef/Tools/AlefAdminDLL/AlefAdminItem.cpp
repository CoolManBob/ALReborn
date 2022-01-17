// AlefAdminItem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminItem.h"
#include ".\alefadminitem.h"

#include "AlefAdminManager.h"
#include "AuTimeStamp.h"
#include "AgcmUIGuild.h"
#include "AgpmWorld.h"
#include "AgcmLogin.h"
#include "AgpdMailBox.h"
#include "AgcmUIProduct.h"


// AlefAdminItem

IMPLEMENT_DYNCREATE(AlefAdminItem, CFormView)

enum eAlefAdminItemIndex
{
	ALEFADMINITEM_INDEX_EQUIP				= 0,
	ALEFADMINITEM_INDEX_INVEN_START			= 1,
	ALEFADMINITEM_INDEX_INVEN_END			= 4,
	ALEFADMINITEM_INDEX_INVEN_PET_START		= 5,
	ALEFADMINITEM_INDEX_INVEN_PET_END		= 8,
	ALEFADMINITEM_INDEX_SALESBOX			= 9,
	ALEFADMINITEM_INDEX_CASH_ITEM			= 10,
	ALEFADMINITEM_INDEX_MAILBOX				= 11,
	ALEFADMINITEM_INDEX_BANK_START			= 12,
	//ALEFADMINITEM_INDEX_BANK_END			= 100,		// 통합서버 때문에 index가 100도 넘어버렸음... - arycoat. 2010.2.17
};

#define ALEFADMINITEM_MAX_DELETE_REASON			2
#define ALEFADMINITEM_MAX_CREATE_REASON			2

// 2007.03.20. steeple
struct FindItem
{
public:
	FindItem(const UINT64 ullItemSeq) : m_ullItemSeq(ullItemSeq) {;}

	bool operator() (const stAgpdAdminItem* pstItem)
	{
		if(!pstItem)
			return false;

		return pstItem->m_ullDBID == m_ullItemSeq;
	}

	bool operator== (const stAgpdAdminItem* pstItem)
	{
		if(!pstItem)
			return false;

		return pstItem->m_ullDBID == m_ullItemSeq;
	}

private:
	UINT64 m_ullItemSeq;

	FindItem() {;}
};

//TCHAR* g_aszDeleteReason[ALEFADMINITEM_MAX_DELETE_REASON] = 
//{
//	_T("--삭제이유--"),
//	_T("환불")
//};

//TCHAR* g_aszCreateReason[ALEFADMINITEM_MAX_CREATE_REASON] =
//{
//	_T("--생성이유--"),
//	_T("보상")
//};


AlefAdminItem::AlefAdminItem()
	: CFormView(AlefAdminItem::IDD)
{
	//m_pHashMapItemTemplate = NULL;
	memset(&m_stSelectedItem, 0, sizeof(m_stSelectedItem));

	m_pcsAgpmItem = NULL;
	m_pcsAgpmSkill = NULL;
	m_pcsAgpmEventSkillMaster = NULL;
}

AlefAdminItem::~AlefAdminItem()
{
	OnClearInventory( Inventory_MailBox );
	OnClearInventory( Inventory_Bank );
	OnClearInventory( Inventory_Sales );
	OnClearInventory( Inventory_Pet );
	OnClearInventory( Inventory_Cash );
	OnClearInventory( Inventory_Equip );
	OnClearInventory( Inventory_Normal );
}

void AlefAdminItem::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_POS, m_csItemPosCB);
	DDX_Control(pDX, IDC_B_REFRESH, m_csRefreshBtn);
	DDX_Control(pDX, IDC_LC_ITEM, m_csItemList);
	DDX_Control(pDX, IDC_B_REFRESH_CONVERT, m_csConvertRefreshBtn);
	DDX_Control(pDX, IDC_B_DELETE, m_csItemDeleteBtn);
	DDX_Control(pDX, IDC_CB_CONVERT_PHYSICAL, m_csConvertPhysicalCB);
	DDX_Control(pDX, IDC_CB_CONVERT_SOCKET, m_csConvertSocketCB);
	DDX_Control(pDX, IDC_CB_CONVERT_SPIRIT_STONE, m_csConvertSpiritStoneCB);
	DDX_Control(pDX, IDC_CB_CONVERT_STONE, m_csConvertStoneCB);
	DDX_Control(pDX, IDC_B_CONVERT, m_csConvertBtn);
	DDX_Control(pDX, IDC_B_CREATE, m_csCreateBtn);
	DDX_Control(pDX, IDC_L_ITEM_CONVERT, m_csConvertList);
	DDX_Control(pDX, IDC_L_OPTION_ADDED, m_csOptionAddedList);
	DDX_Control(pDX, IDC_L_OPTION_ALL, m_csOptionAllList);
	DDX_Control(pDX, IDC_B_OPTION_ADD, m_csOptionAddBtn);
	DDX_Control(pDX, IDC_B_OPTION_REMOVE, m_csOptionRemoveBtn);
	DDX_Control(pDX, IDC_B_OPT_SEARCH, m_csOptionSearchBtn);
	DDX_Control(pDX, IDC_CB_ITEM_LIST, m_csItemListCB);
	DDX_Control(pDX, IDC_CHK_USING, m_csChkUsingBtn);
	DDX_Control(pDX, IDC_CB_E_DATE_YEAR, m_csExpireDateYearCB);
	DDX_Control(pDX, IDC_CB_E_DATE_MONTH, m_csExpireDateMonthCB);
	DDX_Control(pDX, IDC_CB_E_DATE_DAY, m_csExpireDateDayCB);
	DDX_Control(pDX, IDC_CB_E_DATE_HOUR, m_csExpireDateHourCB);
	DDX_Control(pDX, IDC_CB_E_DATE_MIN, m_csExpireDateMinCB);
	DDX_Control(pDX, IDC_E_R_TIME_DAY, m_csRemainTimeDayEdit);
	DDX_Control(pDX, IDC_E_R_TIME_HOUR, m_csRemainTimeHourEdit);
	DDX_Control(pDX, IDC_E_R_TIME_MIN, m_csRemainTimeMinEdit);
	DDX_Control(pDX, IDC_E_R_TIME_SEC, m_csRemainTimeSecEdit);
	DDX_Control(pDX, IDC_S_REMAIN_TIME, m_csRemainTimeStatic);
	DDX_Control(pDX, IDC_B_ITEM_SEARCH, m_csItemSearchBtn);
	DDX_Control(pDX, IDC_CB_EXPIRE_DATE_DEFAULT, m_csExpireDateDefaultCB);
	DDX_Control(pDX, IDC_CB_DELETE_REASON, m_csDeleteReasonCB);
	DDX_Control(pDX, IDC_CB_CREATE_REASON, m_csCreateReasonCB);
	DDX_Control(pDX, IDC_B_ITEM_UPDATE, m_csItemUpdateBtn);
	DDX_Control(pDX, IDC_CB_SKILLPLUS, m_csSkillPlusCB);
	DDX_Control(pDX, IDC_L_SKILL_ADDED, m_csSkillAddedList);
	DDX_Control(pDX, IDC_L_SKILL_ALL, m_csSkillAllList);
	DDX_Control(pDX, IDC_B_SKILL_ADD, m_csSkillAddBtn);
	DDX_Control(pDX, IDC_B_SKILL_REMOVE, m_csSkillRemoveBtn);
	DDX_Control(pDX, IDC_B_SKILL_SEARCH, m_csSkillSearchBtn);
}

BEGIN_MESSAGE_MAP(AlefAdminItem, CFormView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_CB_POS, OnCbnSelchangeCbPos)
	ON_WM_ACTIVATEAPP()
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_B_CREATE, OnBnClickedBCreate)
	ON_NOTIFY(NM_CLICK, IDC_LC_ITEM, OnNMClickLcItem)
	ON_CBN_SELCHANGE(IDC_CB_CONVERT_SOCKET, OnCbnSelchangeCbConvertSocket)
	ON_CBN_SELCHANGE(IDC_CB_CONVERT_PHYSICAL, OnCbnSelchangeCbConvertPhysical)
	ON_BN_CLICKED(IDC_B_CONVERT, OnBnClickedBConvert)
	ON_CBN_SELCHANGE(IDC_CB_CONVERT_SPIRIT_STONE, OnCbnSelchangeCbConvertSpiritStone)
	ON_CBN_SELCHANGE(IDC_CB_CONVERT_STONE, OnCbnSelchangeCbConvertStone)
	ON_BN_CLICKED(IDC_B_REFRESH_CONVERT, OnBnClickedBRefreshConvert)
	ON_BN_CLICKED(IDC_B_DELETE, OnBnClickedBDelete)
	ON_BN_CLICKED(IDC_B_REFRESH, OnBnClickedBRefresh)
	ON_NOTIFY(NM_CLICK, IDC_L_ITEM_CONVERT, OnNMClickLItemConvert)
	ON_LBN_SELCHANGE(IDC_L_OPTION_ADDED, OnLbnSelchangeLOptionAdded)
	ON_LBN_SELCHANGE(IDC_L_OPTION_ALL, OnLbnSelchangeLOptionAll)
	ON_BN_CLICKED(IDC_B_OPTION_ADD, OnBnClickedBOptionAdd)
	ON_BN_CLICKED(IDC_B_OPTION_REMOVE, OnBnClickedBOptionRemove)
	ON_BN_CLICKED(IDC_B_OPT_SEARCH, OnBnClickedOptionSearch)
	ON_CBN_SELCHANGE(IDC_CB_ITEM_LIST, OnCbnSelchangeCbItemList)
	ON_BN_CLICKED(IDC_B_ITEM_SEARCH, OnBnClickedBItemSearch)
	ON_BN_CLICKED(IDC_CHK_USING, OnBnClickedChkUsing)
	ON_CBN_SELCHANGE(IDC_CB_EXPIRE_DATE_DEFAULT, OnCbnSelchangeCbExpireDateDefault)
	ON_BN_CLICKED(IDC_B_ITEM_UPDATE, OnBnClickedBItemUpdate)
	ON_BN_CLICKED(IDC_B_SKILL_ADD, OnBnClickedBSkillAdd)
	ON_BN_CLICKED(IDC_B_SKILL_REMOVE, OnBnClickedBSkillRemove)
	ON_BN_CLICKED(IDC_B_SKILL_SEARCH, OnBnClickedSkillSearch)
END_MESSAGE_MAP()


// AlefAdminItem 진단입니다.

#ifdef _DEBUG
void AlefAdminItem::AssertValid() const
{
	CFormView::AssertValid();
}

void AlefAdminItem::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// AlefAdminItem 메시지 처리기입니다.

void AlefAdminItem::OnInitialUpdate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFormView::OnInitialUpdate();

	m_pcsAgpmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	m_pcsAgpmSkill = ( AgpmSkill* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmSkill" );
	m_pcsAgpmEventSkillMaster = ( AgpmEventSkillMaster* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmEventSkillMaster" );
	if(!m_pcsAgpmItem || !m_pcsAgpmSkill)
	{
		ASSERT(!"Some module is NULL");
	}

	// ItemTemplate 의 HashMap 은 Manager 에 있다.
	//m_pHashMapItemTemplate = &AlefAdminManager::Instance()->m_hashMapItemTemplate;

	InitList();
	InitConvertList();

	InitGrid();
	InitControls();

	InitItemList(0);	// Equip 으로 초기화
}

BOOL AlefAdminItem::PreTranslateMessage(MSG* msg)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CWnd::PreTranslateMessage(msg);
}

BOOL AlefAdminItem::InitList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csItemList.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_POS), LVCFMT_LEFT, 60);
	m_csItemList.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_NAME), LVCFMT_LEFT, 180);
	m_csItemList.InsertColumn(2, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_COUNT), LVCFMT_LEFT, 40);
	m_csItemList.InsertColumn(3, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_CONVERT), LVCFMT_LEFT, 40);
	m_csItemList.InsertColumn(4, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_SOCKET), LVCFMT_LEFT, 40);
	m_csItemList.InsertColumn(5, _T("TID"), LVCFMT_LEFT, 50);
	m_csItemList.InsertColumn(6, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_RANK), LVCFMT_LEFT, 40);
	m_csItemList.InsertColumn(7, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_USE), LVCFMT_LEFT, 60);
	m_csItemList.InsertColumn(8, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_REMAIN_TIME), LVCFMT_LEFT, 60);
	m_csItemList.InsertColumn(9, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_EXPIRE_DATE), LVCFMT_LEFT, 105);
	m_csItemList.InsertColumn(10, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_USE_COUNT), LVCFMT_LEFT, 60);

	HWND hWndHeader = m_csItemList.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeader.SubclassWindow(hWndHeader);

	m_csItemList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);

	return TRUE;
}

BOOL AlefAdminItem::InitGrid()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return TRUE;
}

BOOL AlefAdminItem::InitControls()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;

	m_csRefreshBtn.SetXButtonStyle(dwStyle);
	m_csConvertRefreshBtn.SetXButtonStyle(dwStyle);
	m_csItemDeleteBtn.SetXButtonStyle(dwStyle);
	m_csConvertBtn.SetXButtonStyle(dwStyle);
	m_csCreateBtn.SetXButtonStyle(dwStyle);
	m_csOptionAddBtn.SetXButtonStyle(dwStyle);
	m_csOptionRemoveBtn.SetXButtonStyle(dwStyle);
	m_csOptionSearchBtn.SetXButtonStyle(dwStyle);
	m_csItemSearchBtn.SetXButtonStyle(dwStyle);
	m_csItemUpdateBtn.SetXButtonStyle(dwStyle);
	m_csSkillAddBtn.SetXButtonStyle(dwStyle);
	m_csSkillRemoveBtn.SetXButtonStyle(dwStyle);
	m_csSkillSearchBtn.SetXButtonStyle(dwStyle);

	InitPosCB();

	InitConvertPhysicalCB();
	InitConvertSocketCB();
	InitConvertStoneCB();

	InitDeleteReasonCB();
	InitCreateReasonCB();

	InitItemListCB();
	InitCashItemCreateOption();

	InitOptionAllList();
	InitSkillAllList();

	return TRUE;
}

BOOL AlefAdminItem::InitPosCB()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csItemPosCB.ResetContent();

	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_EQUIP));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_INVENTORY) + _T("1"));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_INVENTORY) + _T("2"));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_INVENTORY) + _T("3"));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_INVENTORY) + _T("4"));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_INVENTORY_PET) + _T("1"));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_INVENTORY_PET) + _T("2"));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_INVENTORY_PET) + _T("3"));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_INVENTORY_PET) + _T("4"));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_AUCTION));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_CASH_INVEN));
	m_csItemPosCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_MAILBOX));

	// 2005.09.14. steeple
	// 창고에다가 서버이름을 붙여야 하므로 이렇게 바뀜.
	AgpmWorld* pcsAgpmWorld = ( AgpmWorld* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmWorld" );
	if(!pcsAgpmWorld)
		return TRUE;
	
	AgcmLogin* pcmLogin = ( AgcmLogin* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgcmLogin" );
	AgpdWorld* pcsWorld = pcsAgpmWorld->GetWorld(pcmLogin->m_szWorldName);
	if(!pcsWorld)
		return TRUE;

	INT32 lWorldGroupIndex = pcsWorld->m_nWorldGroupIndex;
	CString szTmp = _T("");

	for(INT32 lIndex = 0; lIndex < pcsAgpmWorld->GetWorldCount(); lIndex++)
	{
		pcsWorld = pcsAgpmWorld->GetWorld(lIndex);
		if(!pcsWorld)
			continue;

		if(lWorldGroupIndex != pcsWorld->m_nWorldGroupIndex)
			continue;

		for(INT32 i = 0; i < AGPMITEM_BANK_MAX_LAYER; ++i)
		{
			szTmp.Format("%s%d@%s", AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BANK), i+1, pcsWorld->m_szName);
			m_csItemPosCB.AddString(szTmp);
		}
	}

	m_csItemPosCB.SetCurSel(0);

	return TRUE;
}

BOOL AlefAdminItem::InitConvertList(INT32 lMaxSocket)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csConvertList.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_SOCKET), LVCFMT_LEFT, 60);
	m_csConvertList.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_SOCKETED_ITEM), LVCFMT_LEFT, 200);

	HWND hWndHeader = m_csConvertList.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeader2.SubclassWindow(hWndHeader);

	m_csConvertList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);

	//CHAR szTmp[32];
	//for(INT32 i = 0; i < lMaxSocket; i++)
	//{
	//	sprintf(szTmp, "%d", i + 1);
	//	m_csConvertList.InsertItem(i, szTmp);
	//}

	return TRUE;
}

BOOL AlefAdminItem::InitConvertPhysicalCB()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csConvertPhysicalCB.ResetContent();
	m_csConvertPhysicalCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_PHYSICAL_CONVERT));
	CHAR szTmp[16];
	for(INT32 i = 0; i < AGPDITEMCONVERT_MAX_ITEM_RANK; i++)
	{
		sprintf(szTmp, "%d", i + 1);
		m_csConvertPhysicalCB.AddString(szTmp);
	}
	m_csConvertPhysicalCB.SetCurSel(0);

	return TRUE;
}

BOOL AlefAdminItem::InitConvertSocketCB()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csConvertSocketCB.ResetContent();
	m_csConvertSocketCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_SOCKET));
	CHAR szTmp[16];
	for(INT32 i = 0; i < AGPDITEMCONVERT_MAX_WEAPON_SOCKET; i++)
	{
		sprintf(szTmp, "%d", i + 1);
		m_csConvertSocketCB.AddString(szTmp);
	}
	m_csConvertSocketCB.SetCurSel(0);

	return TRUE;
}

BOOL AlefAdminItem::InitConvertStoneCB()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csConvertSpiritStoneCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_SPIRIT_STONE));
	m_csConvertStoneCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_RUNE));

	// ItemTemplate 을 돌면서 정령석, 기원석을 넣는다.

	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );

	for(AgpaItemTemplate::iterator it = ppmItem->csTemplateAdmin.begin(); it != ppmItem->csTemplateAdmin.end(); ++it)
	{
		AgpdItemTemplate* pcsItemTemplate = it->second;

		if(pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
		{
			char tempString[70];

			if(((AgpdItemTemplateUsable*)pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
			{
				// Spirit Stone 일 때는 기본속성 5가지만 바를 수 있다. 2005.03.09 현재
				if(((AgpdItemTemplateUsableSpiritStone*)pcsItemTemplate)->m_eSpiritStoneType == AGPMITEM_USABLE_SS_TYPE_MAGIC ||
					((AgpdItemTemplateUsableSpiritStone*)pcsItemTemplate)->m_eSpiritStoneType == AGPMITEM_USABLE_SS_TYPE_WATER ||
					((AgpdItemTemplateUsableSpiritStone*)pcsItemTemplate)->m_eSpiritStoneType == AGPMITEM_USABLE_SS_TYPE_FIRE ||
					((AgpdItemTemplateUsableSpiritStone*)pcsItemTemplate)->m_eSpiritStoneType == AGPMITEM_USABLE_SS_TYPE_EARTH ||
					((AgpdItemTemplateUsableSpiritStone*)pcsItemTemplate)->m_eSpiritStoneType == AGPMITEM_USABLE_SS_TYPE_AIR)
				{
					sprintf(tempString, "%s, %d", pcsItemTemplate->m_szName, pcsItemTemplate->m_lID);
					m_csConvertSpiritStoneCB.AddString(tempString);
				}
			}
			else if(((AgpdItemTemplateUsable*)pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_RUNE)
			{
				sprintf(tempString, "%s, %d", pcsItemTemplate->m_szName, pcsItemTemplate->m_lID);
				m_csConvertStoneCB.AddString(tempString);
			}
		}
	}

	m_csConvertSpiritStoneCB.SetCurSel(0);
	m_csConvertStoneCB.SetCurSel(0);

	return TRUE;
}

BOOL AlefAdminItem::InitDeleteReasonCB()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csDeleteReasonCB.ResetContent();

	//for(INT32 i = 0; i < ALEFADMINITEM_MAX_DELETE_REASON; i++)
	//	m_csDeleteReasonCB.AddString(g_aszDeleteReason[i]);

	m_csDeleteReasonCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DELETE_REASON));
	m_csDeleteReasonCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DELETE_REPAY));

	m_csDeleteReasonCB.SetCurSel(0);

	return TRUE;
}

BOOL AlefAdminItem::InitCreateReasonCB()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csCreateReasonCB.ResetContent();

	//for(INT32 i = 0; i < ALEFADMINITEM_MAX_CREATE_REASON; i++)
	//	m_csCreateReasonCB.AddString(g_aszCreateReason[i]);

	m_csCreateReasonCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CREATE_REASON));
	m_csCreateReasonCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CREATE_REWARD));

	m_csCreateReasonCB.SetCurSel(0);

	return TRUE;
}

// 2006.01.02. steeple
BOOL AlefAdminItem::InitItemListCB()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csItemListCB.ResetContent();

	AgpmItem* pcsAgpmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	if(!pcsAgpmItem)
		return FALSE;

	m_csItemListCB.AddString(_T(""));	// 먼저 TID 0 에 해당하는 빈칸 하나 넣자.
	
	TCHAR szTmp[AGPMITEM_MAX_ITEM_NAME + 32 + 1];	// 이름 + TID
	memset(szTmp, 0, sizeof(szTmp));

	for(AgpaItemTemplate::iterator it = pcsAgpmItem->csTemplateAdmin.begin(); it != pcsAgpmItem->csTemplateAdmin.end(); ++it)
	{
		AgpdItemTemplate* pcsItemTemplate = it->second;

		wsprintf(szTmp, "%s, %d", pcsItemTemplate->m_szName, pcsItemTemplate->m_lID);
		m_csItemListCB.AddString(szTmp);
	}

	m_csItemListCB.SetCurSel(0);

	return TRUE;
}

// 2006.01.02. steeple
BOOL AlefAdminItem::InitCashItemCreateOption()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csChkUsingBtn.SetCheck(BST_UNCHECKED);

	InitExpireDate();

	SetEnableRemainTime(FALSE);
	SetEnableExpireDate(FALSE);

	return TRUE;
}

// 2006.01.02. steeple
BOOL AlefAdminItem::InitExpireDate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csExpireDateDefaultCB.SetCurSel(0);

	TCHAR szTmp[32];
	memset(szTmp, 0, sizeof(szTmp));

	for(INT32 i = 2006; i < 2031; i++)
	{
		wsprintf(szTmp, "%d", i);
		m_csExpireDateYearCB.AddString(szTmp);
	}
	m_csExpireDateYearCB.SetCurSel(0);

	for(INT32 i = 1; i < 13; i++)
	{
		wsprintf(szTmp, "%02d", i);
		m_csExpireDateMonthCB.AddString(szTmp);
	}
	m_csExpireDateMonthCB.SetCurSel(0);

	for(INT32 i = 1; i < 32; i++)
	{
		wsprintf(szTmp, "%02d", i);
		m_csExpireDateDayCB.AddString(szTmp);
	}
	m_csExpireDateDayCB.SetCurSel(0);

	for(INT32 i = 0; i < 24; i++)
	{
		wsprintf(szTmp, "%02d", i);
		m_csExpireDateHourCB.AddString(szTmp);
	}
	m_csExpireDateHourCB.SetCurSel(0);

	for(INT32 i = 0; i < 60; i++)
	{
		wsprintf(szTmp, "%02d", i);
		m_csExpireDateMinCB.AddString(szTmp);
	}
	m_csExpireDateMinCB.SetCurSel(0);

	return TRUE;
}

// 2005.01.03. steeple
BOOL AlefAdminItem::SetCashItemControls(AgpdItemTemplate* pcsItemTemplate)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsItemTemplate)
		return FALSE;

	// 생성 이유 초기화
	m_csCreateReasonCB.SetCurSel(0);

	int iCheck = 0;
	switch(pcsItemTemplate->m_eCashItemType)
	{
		case AGPMITEM_CASH_ITEM_TYPE_NONE:
		default:
			SetEnableRemainTime(FALSE);
			SetEnableExpireDate(FALSE);
			break;

		case AGPMITEM_CASH_ITEM_TYPE_PLAY_TIME:
			SetEnableRemainTime(TRUE);
			ClearRemainTime();
			SetDefaultRemainTime(pcsItemTemplate->m_lRemainTime);
			SetEnableExpireDate(FALSE);
			break;

		case AGPMITEM_CASH_ITEM_TYPE_REAL_TIME:
			SetEnableRemainTime(FALSE);
			SetEnableExpireDate(TRUE);
			break;

		case AGPMITEM_CASH_ITEM_TYPE_ONE_ATTACK:
			SetEnableRemainTime(FALSE);
			SetEnableExpireDate(FALSE);
			break;
	}

	return TRUE;
}

// 2006.01.02. steeple
BOOL AlefAdminItem::SetEnableRemainTime(BOOL bEnable)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csRemainTimeStatic.EnableWindow(bEnable);
	m_csRemainTimeDayEdit.EnableWindow(bEnable);
	m_csRemainTimeHourEdit.EnableWindow(bEnable);
	m_csRemainTimeMinEdit.EnableWindow(bEnable);
	m_csRemainTimeSecEdit.EnableWindow(bEnable);

	return TRUE;
}

// 2006.01.02. steeple
BOOL AlefAdminItem::SetEnableExpireDate(BOOL bEnable)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csExpireDateDefaultCB.EnableWindow(bEnable);
	m_csExpireDateYearCB.EnableWindow(bEnable);
	m_csExpireDateMonthCB.EnableWindow(bEnable);
	m_csExpireDateDayCB.EnableWindow(bEnable);
	m_csExpireDateHourCB.EnableWindow(bEnable);
	m_csExpireDateMinCB.EnableWindow(bEnable);

	return TRUE;
}

// 2006.01.05. steeple
// lRemainTime 은 millisecond
BOOL AlefAdminItem::SetDefaultRemainTime(INT64 lRemainTime)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(lRemainTime < 1)
		return FALSE;

	INT32 ONE_DAY		= 24*60*60*1000;
	INT32 ONE_HOUR		= 60*60*1000;
	INT32 ONE_MINUTE	= 60*1000;

	if(lRemainTime >= ONE_DAY)
	{
		SetDlgItemInt(IDC_E_R_TIME_DAY, lRemainTime / ONE_DAY);
		SetDefaultRemainTime(lRemainTime % ONE_DAY);
	}
	else if(lRemainTime >= ONE_HOUR)
	{
		SetDlgItemInt(IDC_E_R_TIME_HOUR, lRemainTime / ONE_HOUR);
		SetDefaultRemainTime(lRemainTime % ONE_HOUR);
	}
	else if(lRemainTime >= ONE_MINUTE)
	{
		SetDlgItemInt(IDC_E_R_TIME_MIN, lRemainTime / ONE_MINUTE);
		SetDefaultRemainTime(lRemainTime % ONE_MINUTE);
	}
	else
	{
		SetDlgItemInt(IDC_E_R_TIME_SEC, lRemainTime / 1000);
		SetDefaultRemainTime(0);
	}

	return TRUE;
}

BOOL AlefAdminItem::ClearRemainTime()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	SetDlgItemInt(IDC_E_R_TIME_DAY, 0);
	SetDlgItemInt(IDC_E_R_TIME_HOUR, 0);
	SetDlgItemInt(IDC_E_R_TIME_MIN, 0);
	SetDlgItemInt(IDC_E_R_TIME_SEC, 0);

	return TRUE;
}

// 2005.04.11. steeple
BOOL AlefAdminItem::InitOptionAllList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CHAR szTmp[128];
	memset(szTmp, 0, sizeof(szTmp));

	// 모든 옵션 템플릿 데이터를 보여준다.
	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );

	for(AgpaItemOptionTemplate::iterator it = ppmItem->csOptionTemplateAdmin.begin(); it != ppmItem->csOptionTemplateAdmin.end(); ++it)
	{
		AgpdItemOptionTemplate* pcsOptionTemplate = it->second;

		sprintf(szTmp, "%d. %s", pcsOptionTemplate->m_lID, pcsOptionTemplate->m_szDescription);
		m_csOptionAllList.AddString(szTmp);
	}

	return TRUE;
}

BOOL AlefAdminItem::InitSkillAllList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CHAR szTmp[128];
	memset(szTmp, 0, sizeof(szTmp));

	INT32 lCount = 0;

	for(INT32 eRaceType = AURACE_TYPE_HUMAN; eRaceType < AURACE_TYPE_MAX; eRaceType++)
	{
		for(INT32 eCharClass = AUCHARCLASS_TYPE_KNIGHT; eCharClass <= AUCHARCLASS_TYPE_MAX; eCharClass++)
		{
			// 드래곤시온 이외의 종족들은 이렇게..
			if(eRaceType != AURACE_TYPE_DRAGONSCION && eCharClass != AUCHARCLASS_TYPE_MAX)
			{
				for(INT32 lMasteryIndex = 0; lMasteryIndex < AGPMEVENT_SKILL_MAX_MASTERY - 1; lMasteryIndex++)
				{
					INT32* pcsSkillMasteryList = m_pcsAgpmEventSkillMaster->GetMastery((AuRaceType)eRaceType, (AuCharClassType)eCharClass, lMasteryIndex);

					if(!pcsSkillMasteryList)
						continue;

					// 여기서 전투, 강화, 전용, 패시브 스킬을 받아온다.
					for(int i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; i++)
					{
						if(pcsSkillMasteryList[i] == 0)
							break;

						AgpdSkillTemplate* pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsSkillMasteryList[i]);
						if(!pcsSkillTemplate)
							continue;

						sprintf(szTmp, "%d. %s", pcsSkillTemplate->m_lID, pcsSkillTemplate->m_szName);
						m_csSkillAllList.AddString(szTmp);

						lCount++;
					}

					// 추가로 연계스킬을 받아온다.
					INT32 nCharacterTID = -1;
					switch( eRaceType )
					{
					case AURACE_TYPE_HUMAN :
						{
							switch( eCharClass )
							{
							case AUCHARCLASS_TYPE_KNIGHT :	nCharacterTID = 96;		break;
							case AUCHARCLASS_TYPE_RANGER :	nCharacterTID = 1;		break;
							case AUCHARCLASS_TYPE_MAGE : 	nCharacterTID = 6;		break;
							}
						}
						break;

					case AURACE_TYPE_ORC :
						{
							switch( eCharClass )
							{
							case AUCHARCLASS_TYPE_KNIGHT :	nCharacterTID = 4;		break;
							case AUCHARCLASS_TYPE_RANGER :	nCharacterTID = 8;		break;
							case AUCHARCLASS_TYPE_MAGE : 	nCharacterTID = 3;		break;
							}
						}
						break;

					case AURACE_TYPE_MOONELF :
						{
							switch( eCharClass )
							{
							case AUCHARCLASS_TYPE_KNIGHT :	nCharacterTID = 377;	break;
							case AUCHARCLASS_TYPE_RANGER :	nCharacterTID = 460;	break;
							case AUCHARCLASS_TYPE_MAGE : 	nCharacterTID = 9;		break;
							}
						}
						break;
					}

					HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( nCharacterTID );
					if( pVector )
					{
						HighLevelSkillIter iter = pVector->begin();
						while( iter != pVector->end() )
						{
							AgpdEventSkillHighLevel stHighLevel = *iter;
							AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate( stHighLevel.m_lSkillTID );
							if(pcsSkillTemplate)
							{
								sprintf(szTmp, "%d. %s", pcsSkillTemplate->m_lID, pcsSkillTemplate->m_szName);
								m_csSkillAllList.AddString(szTmp);

								lCount++;
							}

							iter++;
						}
					} // 연계스킬
				}
			} // 드래곤 시온 이외의 종족.
			else // 드래곤 시온.
			{
				// 드래곤시온은 스킬마스터리에 없다.. 다르게 받아와야 함..
				INT32 nCharacterTID = -1;
				switch( eCharClass )
				{
				case AUCHARCLASS_TYPE_KNIGHT :	nCharacterTID = DRAGONSCION_SLAYER;					break;
				case AUCHARCLASS_TYPE_RANGER :	nCharacterTID = DRAGONSCION_OBITER;					break;
				case AUCHARCLASS_TYPE_MAGE :	nCharacterTID = DRAGONSCION_SUMMERNER;				break;
				case AUCHARCLASS_TYPE_SCION :	nCharacterTID = DRAGONSCION_SCION;					break;
				case AUCHARCLASS_TYPE_MAX :		nCharacterTID = AGPMEVENT_HIGHLEVEL_PASSIVE_SCION;	break;
				}

				if( nCharacterTID >= 0 )
				{
					HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( nCharacterTID );
					if( pVector )
					{
						INT nCount = 0;
						HighLevelSkillIter iter = pVector->begin();

						while( iter != pVector->end() )
						{
							AgpdEventSkillHighLevel stHighLevel = *iter;
							AgpdSkillTemplate* ppdSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate( stHighLevel.m_lSkillTID );
							if( ppdSkillTemplate )
							{
								sprintf(szTmp, "%d. %s", ppdSkillTemplate->m_lID, ppdSkillTemplate->m_szName);
								m_csSkillAllList.AddString(szTmp);

								nCount++;
							}

							iter++;
						}
					}
				}
			}
		}
	}
	return TRUE;
}

AgpdItemTemplate* AlefAdminItem::GetItemTemplate(INT32 lTID)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!AlefAdminManager::Instance()->IsInitialized())
		return NULL;

	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	return ppmItem->GetItemTemplate(lTID);
}

AgpdItemTemplate* AlefAdminItem::GetItemTemplate(CHAR* szItemName)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szItemName)
		return NULL;

	if(!AlefAdminManager::Instance()->IsInitialized())
		return NULL;

	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	return ppmItem->GetItemTemplate(szItemName);
}

// 2005.06.09. steeple
// Item Rank 를 Template 과 Option 의 합으로 얻는 방법이다.
//
// 원래 랭크는 아이템 자체의 Factor 에서 얻으면 되는데
// 접속해 있지 않은 캐릭의 아이템은 그런 방법을 쓸 수가 없어서
// 이렇게 녹아다로 처리한다. -_-;;
INT32 AlefAdminItem::GetItemRank(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return 0;

	if(!AlefAdminManager::Instance()->IsInitialized())
		return 0;

	AgpmFactors* pcsAgpmFactors = ( AgpmFactors* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmFactors" );
	if(!pcsAgpmFactors)
		return FALSE;

	AgpmItem* pcsAgpmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	if(!pcsAgpmItem)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = GetItemTemplate(pstItem->m_lTID);
	if(!pcsItemTemplate)
		return 0;

	INT32 lItemRank = 0;
	pcsAgpmFactors->GetValue(&pcsItemTemplate->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_PHYSICAL_RANK);

	AgpdItemOptionTemplate* pcsOptionTemplate = NULL;

	// Option 에 Rank 에 변화를 주는 거를 찾는다.
	for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		if(pstItem->m_alOptionTID[i] == 0)
			break;

		pcsOptionTemplate = pcsAgpmItem->GetItemOptionTemplate(pstItem->m_alOptionTID[i]);
		if(pcsOptionTemplate)
		{
			INT32 lOptionRank = 0;

			if (pcsOptionTemplate->m_lPointType == 0)
				pcsAgpmFactors->GetValue(&pcsOptionTemplate->m_csFactor, &lOptionRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_PHYSICAL_RANK);
			else
				pcsAgpmFactors->GetValue(&pcsOptionTemplate->m_csFactorPercent, &lOptionRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_PHYSICAL_RANK);

			lItemRank += lOptionRank;
		}
	}

	return lItemRank;
}

CHAR* AlefAdminItem::GetItemRankName(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return NULL;

	return GetItemRankName(GetItemRank(pstItem));
}

CHAR* AlefAdminItem::GetItemRankName(INT32 lItemRank)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	AgpmItemConvert* ppmItemConvert = ( AgpmItemConvert* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItemConvert" );
	return ppmItemConvert->m_astTablePhysical[lItemRank].szRank;
}

BOOL AlefAdminItem::OnReceive(AgpdItem* pcsItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsItem || !pcsItem->m_pcsCharacter)
		return FALSE;
    
	// 날아온 아이템이 검색한 놈의 것인지 확인
	if(pcsItem->m_eStatus != AGPDITEM_STATUS_BANK &&
		strcmp(pcsItem->m_pcsCharacter->m_szID, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) != 0)
		return FALSE;

	// 이곳에서 Bank Item 의 Validation 체크는 하지 않는다.

	stAgpdAdminItem stItem;
	memset(&stItem, 0, sizeof(stItem));

	if(!SetItemData(pcsItem, &stItem))
		return FALSE;

	return OnReceive(&stItem);
}

BOOL AlefAdminItem::OnReceive(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	// 날아온 아이템이 검색한 놈의 것인지 확인
	if(pstItem->m_lPos != AGPDITEM_STATUS_BANK &&
		strcmp(pstItem->m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) != 0)
		return FALSE;

	// Bank Item 이라면 Account 로 검사
	if(pstItem->m_lPos == AGPDITEM_STATUS_BANK &&
		strcmp(pstItem->m_szAccountName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName()) != 0)
		return FALSE;

	// 2005.05.26. steeple. Template 들을 다시 세팅해준다.
	SetItemTemplate(pstItem);

	// 날아온 아이템의 위치에 따라 다르게 처리해준다.
	switch(pstItem->m_lPos)
	{
	case AGPDITEM_STATUS_INVENTORY :		OnAddItemToInventory( Inventory_Normal, pstItem );		break;
	case AGPDITEM_STATUS_EQUIP :			OnAddItemToInventory( Inventory_Equip, pstItem );		break;
	case AGPDITEM_STATUS_SUB_INVENTORY :	OnAddItemToInventory( Inventory_Pet, pstItem );			break;
	case AGPDITEM_STATUS_BANK :				OnAddItemToInventory( Inventory_Bank, pstItem );		break;
	case AGPDITEM_STATUS_SALESBOX_GRID :
	case AGPDITEM_STATUS_SALESBOX_BACKOUT :	OnAddItemToInventory( Inventory_Sales, pstItem );		break;
	case AGPDITEM_STATUS_CASH_INVENTORY :	OnAddItemToInventory( Inventory_Cash, pstItem );		break;
	case AGPDITEM_STATUS_MAILBOX :			OnAddItemToInventory( Inventory_MailBox, pstItem );		break;
	}

	return ShowData( pstItem );
}

BOOL AlefAdminItem::SetItemData(AgpdItem* pcsItem, stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsItem || !pcsItem->m_pcsCharacter || !pstItem)
		return FALSE;

	AgpmItem* pcsAgpmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	if(!pcsAgpmItem)
		return FALSE;

	AgpmItemConvert* pcsAgpmItemConvert = ( AgpmItemConvert* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItemConvert" );
	if(!pcsAgpmItemConvert)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = (AgpdItemTemplate*)pcsItem->m_pcsItemTemplate;
	if(!pcsItemTemplate)
		return FALSE;

	// 먼저 초기화
	memset(pstItem, 0, sizeof(stAgpdAdminItem));

	strcpy(pstItem->m_szCharName, pcsItem->m_pcsCharacter->m_szID);

	pstItem->m_lID = pcsItem->m_lID;
	pstItem->m_lTID = pcsItemTemplate->m_lID;
	//pstItem->m_pcsTemplate = (PVOID)pcsItemTemplate;

	pstItem->m_lPos = (INT32)pcsItem->m_eStatus;
	pstItem->m_lLayer = (INT32)pcsItem->m_anGridPos[AGPDITEM_GRID_POS_TAB];
	pstItem->m_lRow = (INT32)pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW];
	pstItem->m_lCol = (INT32)pcsItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN];
	pstItem->m_nPart = (INT16)((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nPart;
	pstItem->m_lCount = (INT32)pcsItem->m_nCount;

	pstItem->m_lPhysicalConvertLevel = pcsAgpmItemConvert->GetNumPhysicalConvert(pcsItem);
	pstItem->m_lNumConvertedSocket = pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem);
	pstItem->m_lNumSocket = pcsAgpmItemConvert->GetNumSocket(pcsItem);

	AgpdItemConvertADItem* pcsAttachedConvert = pcsAgpmItemConvert->GetADItem(pcsItem);
	if(pcsAttachedConvert)
	{
		for(int i = 0; i < pcsAttachedConvert->m_lNumSocket; i++)
		{
			if(pcsAttachedConvert->m_lNumConvert > i && pcsAttachedConvert->m_stSocketAttr[i].lTID)
			{
				pstItem->m_stSocketAttr[i].m_bIsSpiritStone = pcsAttachedConvert->m_stSocketAttr[i].bIsSpiritStone;
				pstItem->m_stSocketAttr[i].m_lTID = pcsAttachedConvert->m_stSocketAttr[i].lTID;
				//pstItem->m_stSocketAttr[i].m_pcsItemTemplate = (PVOID)pcsAgpmItem->GetItemTemplate(pcsAttachedConvert->m_stSocketAttr[i].lTID);
			}
		}
	}

	for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		if(pcsItem->m_aunOptionTID[i] == 0)
			break;

		pstItem->m_alOptionTID[i] = pcsItem->m_aunOptionTID[i];
	}

	pstItem->m_lDurability = pcsAgpmItem->GetItemDurabilityCurrent(pcsItem);

	pstItem->m_nInUseItem = pcsItem->m_nInUseItem;
	pstItem->m_lRemainTime = pcsItem->m_lRemainTime;
	pstItem->m_lExpireTime = pcsItem->m_lExpireTime;
	pstItem->m_lCashItemUseCount = pcsItem->m_lCashItemUseCount;

	return TRUE;
}

BOOL AlefAdminItem::SetItemTemplate(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	AgpmItem* pcsAgpmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	if(!pcsAgpmItem)
		return FALSE;

	//pstItem->m_pcsTemplate = (PVOID)pcsAgpmItem->GetItemTemplate(pstItem->m_lTID);

	//for(INT32 i = 0; i < pstItem->m_lNumConvertedSocket; i++)
	//	pstItem->m_stSocketAttr[i].m_pcsItemTemplate = (PVOID)pcsAgpmItem->GetItemTemplate(pstItem->m_stSocketAttr[i].m_lTID);

	return TRUE;
}

BOOL AlefAdminItem::SelectItem(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(pstItem)
		memcpy(&m_stSelectedItem, pstItem, sizeof(m_stSelectedItem));
	else
		memset(&m_stSelectedItem, 0, sizeof(m_stSelectedItem));

	// 삭제 이유 초기화
	m_csDeleteReasonCB.SetCurSel(0);

	return TRUE;
}

BOOL AlefAdminItem::IsSameWithSelectedItem(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem || !pstItem->m_lTID)
		return FALSE;

	if(m_stSelectedItem.m_lTID == 0)
		return FALSE;

	//if(strcmp(pstItem->m_szCharName, m_stSelectedItem.m_szCharName) == 0 &&
	//	pstItem->m_lTID == m_stSelectedItem.m_lTID &&
	//	pstItem->m_lPos == m_stSelectedItem.m_lPos &&
	//	pstItem->m_lLayer == m_stSelectedItem.m_lLayer &&
	//	pstItem->m_lRow == m_stSelectedItem.m_lRow &&
	//	pstItem->m_lCol == m_stSelectedItem.m_lCol)
	//	return TRUE;

	// 걍 깔끔하게 DB ID 로 비교 한다. 2005.06.08. steeple
	if(pstItem->m_ullDBID == m_stSelectedItem.m_ullDBID)
		return TRUE;

	return FALSE;
}

INT32 AlefAdminItem::GetListCtrlIndex(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return -1;

	AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(pstItem->m_lTID);
	if(!pcsItemTemplate)
		return -1;

	INT32 lIndex = -1;

	// 위치에 따라 다르게 한다.
	if(pstItem->m_lPos == AGPDITEM_STATUS_EQUIP)
	{
		//switch(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nPart)
		//{
		//	case AGPMITEM_PART_BODY:				lIndex = 3;		break;
		//	case AGPMITEM_PART_HEAD:				lIndex = 2;		break;
		//	case AGPMITEM_PART_ARMS:				lIndex = 6;		break;	// 이건 어케되는 거냐!!!
		//	case AGPMITEM_PART_HANDS:				lIndex = 6;		break;
		//	case AGPMITEM_PART_LEGS:				lIndex = 4;		break;
		//	case AGPMITEM_PART_FOOT:				lIndex = 5;		break;
		//	case AGPMITEM_PART_CLOAK:				lIndex = -1;	break;	// 아직 망토는 미구현
		//	case AGPMITEM_PART_HAND_LEFT:			lIndex = 1;		break;
		//	case AGPMITEM_PART_HAND_RIGHT:			lIndex = 0;		break;
		//	case AGPMITEM_PART_ACCESSORY_RING1:		lIndex = 8;		break;
		//	case AGPMITEM_PART_ACCESSORY_RING2:		lIndex = 9;		break;
		//	case AGPMITEM_PART_ACCESSORY_NECKLACE:	lIndex = 7;		break;
		//}

		if(pstItem->m_lRow == 0)
		{
			switch(pstItem->m_lCol)
			{
					case 0:		lIndex = 7;			break;	// 목걸이
					case 1:		lIndex = 2;			break;	// 머리
					case 2:		lIndex = 0;			break;	// 오른손
					case 3:		lIndex = 8;			break;	// 반지1 (위)
			}
		}
		else if(pstItem->m_lRow == 1)
		{
			switch(pstItem->m_lCol)
			{
					case 0:		lIndex = 6;			break;	// 팔
					case 1:		lIndex = 3;			break;	// 갑빠
					case 2:		lIndex = 1;			break;	// 왼손
					case 3:		lIndex = 9;			break;	// 반지2 (아래)
			}
		}
		else if(pstItem->m_lRow == 2)
		{
			switch(pstItem->m_lCol)
			{
					case 0:		lIndex = 5;			break;	// 바지
					case 1:		lIndex = 4;			break;	// 발
					case 2:		lIndex = 10;		break;	// 탈것무기
					case 3:		lIndex = 11;		break;	// 탈것
			}
		}
	}
	else if(pstItem->m_lPos == AGPDITEM_STATUS_INVENTORY)
	{
		lIndex = (pstItem->m_lRow * AGPMITEM_INVENTORY_COLUMN) + pstItem->m_lCol + pstItem->m_lRow;
	}
	else if(pstItem->m_lPos == AGPDITEM_STATUS_SUB_INVENTORY)
	{
		lIndex = (pstItem->m_lRow * AGPMITEM_SUB_COLUMN) + pstItem->m_lCol + pstItem->m_lRow;
	}
	else if(pstItem->m_lPos == AGPDITEM_STATUS_BANK)
	{
		lIndex = (pstItem->m_lRow * AGPMITEM_BANK_COLUMN) + pstItem->m_lCol + pstItem->m_lRow;
	}
	else if(pstItem->m_lPos == AGPDITEM_STATUS_SALESBOX_GRID || pstItem->m_lPos == AGPDITEM_STATUS_SALESBOX_BACKOUT)
	{
		lIndex = (pstItem->m_lRow * AGPMITEM_SALES_COLUMN) + pstItem->m_lCol + pstItem->m_lRow;
	}
	else if(pstItem->m_lPos == AGPDITEM_STATUS_CASH_INVENTORY)
	{
		lIndex = pstItem->m_lRow;
	}
	else if(pstItem->m_lPos == AGPDITEM_STATUS_MAILBOX)
	{
		lIndex = pstItem->m_lRow;
	}

	return lIndex;
}

BOOL AlefAdminItem::GetItemPosByIndex(stAgpdAdminItem* pstItem, int iLVIndex)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem || iLVIndex < 0)
		return FALSE;

	// 현재 보여주는 위치를 얻는다.
	int iCurSel = m_csItemPosCB.GetCurSel();
	if(iCurSel == CB_ERR || iCurSel < 0)
		return FALSE;

	CString szTmp = _T("");

	// Item 의 위치저장
	if(iCurSel == ALEFADMINITEM_INDEX_EQUIP)
	{
		//pstItem->m_lPos = AGPDITEM_STATUS_EQUIP;
		//
		//szTmp = m_csItemList.GetItemText(iLVIndex, 1);		// ItemName
		//if(szTmp.GetLength() == 0)
		//	return FALSE;

		//pstItem->m_pcsTemplate = GetItemTemplate((LPSTR)(LPCTSTR)szTmp);
		GetEquipPosByIndex(pstItem, iLVIndex);
	}
	else if(iCurSel >= ALEFADMINITEM_INDEX_INVEN_START && iCurSel <= ALEFADMINITEM_INDEX_INVEN_END)
	{
		pstItem->m_lPos = AGPDITEM_STATUS_INVENTORY;

		szTmp = m_csItemList.GetItemText(iLVIndex, 0);		// Grid Position (Row, Col)
		if(szTmp.GetLength() == 0)
			return FALSE;

		int iLength = szTmp.GetLength();
		int iComma = szTmp.Find(',');
		
		pstItem->m_lLayer = iCurSel - ALEFADMINITEM_INDEX_INVEN_START;
		pstItem->m_lRow = atoi((LPCTSTR)szTmp.Left(iComma));
		pstItem->m_lCol = atoi((LPCTSTR)szTmp.Mid(iComma + 2, iLength - iComma - 2));
	}
	else if(iCurSel >= ALEFADMINITEM_INDEX_INVEN_PET_START && iCurSel <= ALEFADMINITEM_INDEX_INVEN_PET_END)
	{
		pstItem->m_lPos = AGPDITEM_STATUS_SUB_INVENTORY;

		szTmp = m_csItemList.GetItemText(iLVIndex, 0);		// Grid Position (Row, Col)
		if(szTmp.GetLength() == 0)
			return FALSE;

		int iLength = szTmp.GetLength();
		int iComma = szTmp.Find(',');
		
		pstItem->m_lLayer = iCurSel - ALEFADMINITEM_INDEX_INVEN_PET_START;
		pstItem->m_lRow = atoi((LPCTSTR)szTmp.Left(iComma));
		pstItem->m_lCol = atoi((LPCTSTR)szTmp.Mid(iComma + 2, iLength - iComma - 2));
	}
	else if(iCurSel >= ALEFADMINITEM_INDEX_BANK_START /*&& iCurSel <= ALEFADMINITEM_INDEX_BANK_END*/)
	{
		pstItem->m_lPos = AGPDITEM_STATUS_BANK;

		// 2006.03.28. steeple
		// 뱅크의 Layer 와 서버이름을 가져올 수 있는 준비를 한다.
		CString szItemPos = _T("");
		int iTypeIndex = m_csItemPosCB.GetCurSel();
		m_csItemPosCB.GetLBText(iTypeIndex, szItemPos);

		INT32 lBridgeIndex = szItemPos.Find('@');
		if(lBridgeIndex < 0)
			return FALSE;

		szTmp = m_csItemList.GetItemText(iLVIndex, 0);		// Grid Position (Row, Col)
		if(szTmp.GetLength() == 0)
			return FALSE;

		// 여기서는 row, col 을 구한다.
		int iLength = szTmp.GetLength();
		int iComma = szTmp.Find(',');

		pstItem->m_lLayer = 0;		// 일단 Layer 는 0 으로 해놓고
		pstItem->m_lRow = atoi((LPCTSTR)szTmp.Left(iComma));
		pstItem->m_lCol = atoi((LPCTSTR)szTmp.Mid(iComma + 2, iLength - iComma - 2));

		// 여기서 Layer 와 서버이름을 다시 구한다.
		CString szLeft = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BANK);
		INT32 lCurLayer = atoi(szItemPos.Mid(szLeft.GetLength(), 1));		// 창고1@크라우. 이런식에서 1 을 가져오는 것임.
		CString szServerName = szItemPos.Mid(lBridgeIndex + 1);

		lCurLayer--;	// UI 는 1Base 이고, 데이터는 0Base 라서 1 빼준다.

		// 세팅
		pstItem->m_lLayer = lCurLayer;
		strcpy(pstItem->m_szServerName, (LPCTSTR)szServerName);
	}
	else if(iCurSel == ALEFADMINITEM_INDEX_SALESBOX)
	{
		pstItem->m_lPos = AGPDITEM_STATUS_SALESBOX_GRID;

		szTmp = m_csItemList.GetItemText(iLVIndex, 0);		// Grid Position (Row, Col)
		if(szTmp.GetLength() == 0)
			return FALSE;

		int iLength = szTmp.GetLength();
		int iComma = szTmp.Find(',');
		
		pstItem->m_lLayer = 0;
		pstItem->m_lRow = atoi((LPCTSTR)szTmp.Left(iComma));
		pstItem->m_lCol = atoi((LPCTSTR)szTmp.Mid(iComma + 2, iLength - iComma - 2));
	}
	else if(iCurSel == ALEFADMINITEM_INDEX_CASH_ITEM)
	{
		pstItem->m_lPos = AGPDITEM_STATUS_CASH_INVENTORY;

		pstItem->m_lLayer = 0;
		pstItem->m_lRow = iLVIndex;
		pstItem->m_lCol = 0;
	}
	else if(iCurSel == ALEFADMINITEM_INDEX_MAILBOX)
	{
		pstItem->m_lPos = AGPDITEM_STATUS_MAILBOX;

		pstItem->m_lLayer = 0;
		pstItem->m_lRow = iLVIndex;
		pstItem->m_lCol = 0;
	}

	return TRUE;
}

BOOL AlefAdminItem::GetEquipPosByIndex(stAgpdAdminItem* pstItem, int iLVIndex)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem || iLVIndex < 0)
		return FALSE;

	pstItem->m_lPos = AGPDITEM_STATUS_EQUIP;
	switch(iLVIndex)
	{
		case 0:		pstItem->m_lRow = 0;	pstItem->m_lCol = 2;	break;	// 오른손
		case 1:		pstItem->m_lRow = 1;	pstItem->m_lCol = 2;	break;	// 왼손
		case 2:		pstItem->m_lRow = 0;	pstItem->m_lCol = 1;	break;	// 머리
		case 3:		pstItem->m_lRow = 1;	pstItem->m_lCol = 1;	break;	// 갑빠
		case 4:		pstItem->m_lRow = 2;	pstItem->m_lCol = 1;	break;	// 발
		case 5:		pstItem->m_lRow = 2;	pstItem->m_lCol = 0;	break;	// 바지
		case 6:		pstItem->m_lRow = 1;	pstItem->m_lCol = 0;	break;	// 팔
		case 7:		pstItem->m_lRow = 0;	pstItem->m_lCol = 0;	break;	// 목걸이
		case 8:		pstItem->m_lRow = 0;	pstItem->m_lCol = 3;	break;	// 반지1
		case 9:		pstItem->m_lRow = 1;	pstItem->m_lCol = 3;	break;	// 반지2
		case 10:	pstItem->m_lRow = 2;	pstItem->m_lCol = 2;	break;	// 탈것무기
		case 11:	pstItem->m_lRow = 2;	pstItem->m_lCol = 3;	break;	// 탈것
	}

	return TRUE;
}

BOOL AlefAdminItem::ShowData(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	// 얘는 그냥 찍어준다. 많이 불려도 그냥 그러려니 하게나.
	CHAR szTmp[255] = { 0, };

	// 자꾸 버퍼 깨져날아가서 걍 코드에 박아둔다.
	//CHAR* pTextNick = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHARNAME);
	CHAR* pTextNick = "Character Name";

	CHAR* pTextNickString = AlefAdminManager::Instance()->m_pMainDlg->m_szCharName;
	pTextNickString[ AGPACHARACTER_MAX_ID_STRING ] = '\0';

	//CHAR* pTextAccount = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ACCNAME);
	CHAR* pTextAccount = "Account";

	sprintf_s( szTmp, sizeof( CHAR ) * 255, "%s : %s,  %s : %s", pTextNick, pTextNickString, pTextAccount, pstItem->m_szAccountName );
		
	szTmp[ 255 - 1 ] = '\0';
	SetDlgItemText(IDC_S_ITEM_OWNER, szTmp);

	// 콤보박스에서의 Type 과 pstItem 의 Pos 가 맞을때만한다.
	int iTypeIndex = m_csItemPosCB.GetCurSel();
	if(iTypeIndex == CB_ERR)
		return FALSE;

	BOOL bShowData = FALSE;
	if(iTypeIndex == ALEFADMINITEM_INDEX_EQUIP && pstItem->m_lPos == AGPDITEM_STATUS_EQUIP)
		bShowData = TRUE;
	else if(iTypeIndex >= ALEFADMINITEM_INDEX_INVEN_START && iTypeIndex <= ALEFADMINITEM_INDEX_INVEN_END &&	// 콤보박스 인덱스
			pstItem->m_lPos == AGPDITEM_STATUS_INVENTORY &&	// 인벤토리
			iTypeIndex == pstItem->m_lLayer + ALEFADMINITEM_INDEX_INVEN_START)			// ComboBox 와 인벤토리 Layer 비교
		bShowData = TRUE;
	else if(iTypeIndex >= ALEFADMINITEM_INDEX_INVEN_PET_START && iTypeIndex <= ALEFADMINITEM_INDEX_INVEN_PET_END &&	// 콤보박스 인덱스
			pstItem->m_lPos == AGPDITEM_STATUS_SUB_INVENTORY &&	// 인벤토리
			iTypeIndex == pstItem->m_lLayer + ALEFADMINITEM_INDEX_INVEN_PET_START)			// ComboBox 와 인벤토리 Layer 비교
		bShowData = TRUE;
	else if(iTypeIndex >= ALEFADMINITEM_INDEX_BANK_START /*&& iTypeIndex <= ALEFADMINITEM_INDEX_BANK_END*/ &&	// 콤보박스 인덱스
			pstItem->m_lPos == AGPDITEM_STATUS_BANK)		// 뱅크
	{
		// 2005.09.15.
		// 서버통합 후 뱅크 체크를 다양화 해야 한다.
		CString szBank = _T("");
		m_csItemPosCB.GetLBText(iTypeIndex, szBank);

		INT32 lBridgeIndex = szBank.Find('@');
		if(lBridgeIndex < 0)
			return FALSE;

		CString szLeft = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BANK);

		INT32 lCurLayer = atoi(szBank.Mid(szLeft.GetLength(), 1));		// 창고1@크라우. 이런식에서 1 을 가져오는 것임.
		CString szServerName = szBank.Mid(lBridgeIndex + 1);

		lCurLayer--;	// UI 는 1Base 이고, 데이터는 0Base 라서 1 빼준다.

		// Layer 가 같고 ServerName 이 같을 때만 해준다.
		if(lCurLayer == pstItem->m_lLayer &&
			szServerName.Compare(pstItem->m_szServerName) == 0)
			bShowData = TRUE;
	}
	else if(iTypeIndex == ALEFADMINITEM_INDEX_SALESBOX &&
		(pstItem->m_lPos == AGPDITEM_STATUS_SALESBOX_GRID || pstItem->m_lPos == AGPDITEM_STATUS_SALESBOX_BACKOUT))
		bShowData = TRUE;
	else if(iTypeIndex == ALEFADMINITEM_INDEX_CASH_ITEM && pstItem->m_lPos == AGPDITEM_STATUS_CASH_INVENTORY)
		bShowData = TRUE;
	else if(iTypeIndex == ALEFADMINITEM_INDEX_MAILBOX && pstItem->m_lPos == AGPDITEM_STATUS_MAILBOX)
		bShowData = TRUE;

	if(!bShowData)
		return TRUE;	// 걍 나가기만 하면 된다.

	if(pstItem->m_lPos == AGPDITEM_STATUS_EQUIP ||
		pstItem->m_lPos == AGPDITEM_STATUS_INVENTORY ||
		pstItem->m_lPos == AGPDITEM_STATUS_BANK ||
		pstItem->m_lPos == AGPDITEM_STATUS_SALESBOX_GRID ||
		pstItem->m_lPos == AGPDITEM_STATUS_SALESBOX_BACKOUT ||
		pstItem->m_lPos == AGPDITEM_STATUS_CASH_INVENTORY ||
		pstItem->m_lPos == AGPDITEM_STATUS_SUB_INVENTORY ||
		pstItem->m_lPos == AGPDITEM_STATUS_MAILBOX)
		ShowUpdateByPosition(pstItem);	// 이미 Grid 위치가 쓰여있구 거기다가 쓴다.
	else
	{
		ShowUpdateByItemDBID(pstItem);	// 아직 미구현 2005.02.22.
	}

	// 새로 받은 놈과 미리 예전에 선택해 놓은 놈과 같다면!!!
	if(IsSameWithSelectedItem(pstItem))
	{
		// 상세정보를 다시 뿌린다.
		SelectItem(pstItem);
		ShowDurability(pstItem);
		ShowSkillPlus(pstItem);
		ShowConvertData(pstItem);
		ShowConvertComboBox(pstItem);
		ShowAddedOptionData(pstItem);
	}

	return TRUE;
}

BOOL AlefAdminItem::ShowData(INT32 lTypeIndex)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	InitItemList( lTypeIndex );

	switch( lTypeIndex )
	{
	case ALEFADMINITEM_INDEX_SALESBOX :		return OnShowInventory( Inventory_Sales );		break;
	case ALEFADMINITEM_INDEX_CASH_ITEM :	return OnShowInventory( Inventory_Cash );		break;
	case ALEFADMINITEM_INDEX_MAILBOX :		return OnShowInventory( Inventory_MailBox );	break;
	default :
		{
			if( lTypeIndex <= ALEFADMINITEM_INDEX_INVEN_END )
			{
				return OnShowInventory( Inventory_Normal );
			}
			else if( lTypeIndex <= ALEFADMINITEM_INDEX_INVEN_PET_END )
			{
				return OnShowInventory( Inventory_Pet );
			}
			else /*if( lTypeIndex <= ALEFADMINITEM_INDEX_BANK_END )*/
			{
				return OnShowInventory( Inventory_Bank );
			}
		}
		break;
	}

	return TRUE;
}

// 이미 위치가 써있는 놈을 업데이트 할때 사용
BOOL AlefAdminItem::ShowUpdateByPosition(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	// 업데이트 해준다.
	CHAR szTmp[255] = { 0, };

	INT32 lIndex = GetListCtrlIndex(pstItem);
	if(lIndex >= 0)
	{
		AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(pstItem->m_lTID);
		if(!pcsItemTemplate)
			return FALSE;

		if(pstItem->m_lPos != AGPDITEM_STATUS_SALESBOX_BACKOUT)
			m_csItemList.SetItemText(lIndex, 1, pcsItemTemplate->m_szName);
		else
		{
			memset(szTmp, 0, sizeof(CHAR) * 255);
			sprintf(szTmp, "%s (%s)", pcsItemTemplate->m_szName, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_SOLD_OUT));
			m_csItemList.SetItemText(lIndex, 1, szTmp);
		}

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d", pstItem->m_lCount > 0 ? pstItem->m_lCount : 1);
		m_csItemList.SetItemText(lIndex, 2, szTmp);

		if(pstItem->m_lPhysicalConvertLevel > 0)
		{
			memset(szTmp, 0, sizeof(CHAR) * 255);
			sprintf(szTmp, "+%d", pstItem->m_lPhysicalConvertLevel);
		}
		else
		{
			memset(szTmp, 0, sizeof(CHAR) * 255);
			sprintf(szTmp, "0");
		}

		m_csItemList.SetItemText(lIndex, 3, szTmp);

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d/%d", pstItem->m_lNumConvertedSocket, pstItem->m_lNumSocket);
		m_csItemList.SetItemText(lIndex, 4, szTmp);

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d", pstItem->m_lTID);
		m_csItemList.SetItemText(lIndex, 5, szTmp);

		CHAR* szRankName = GetItemRankName(pstItem);
		if(szRankName)
		{
			m_csItemList.SetItemText(lIndex, 6, szRankName);
		}

		if(pstItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE)
		{
			m_csItemList.SetItemText(lIndex, 7, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_USE));
		}
		else if(pstItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE)
		{
			m_csItemList.SetItemText(lIndex, 7, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_PAUSED));
		}

		if(pstItem->m_lRemainTime != 0)		// milli second
		{
			INT64 lRemainTime = pstItem->m_lRemainTime;

			INT32 ONE_DAY		= 24*60*60*1000;
			INT32 ONE_HOUR		= 60*60*1000;
			INT32 ONE_MINUTE	= 60*1000;

			if (lRemainTime >= ONE_DAY)	// 1일
			{
				memset(szTmp, 0, sizeof(CHAR) * 255);
				sprintf(szTmp, "%d%s %d%s",
					(INT32)((double)lRemainTime/(double)ONE_DAY),
					AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TIME_DAY),
					(INT32)((double)(lRemainTime%ONE_DAY)/(double)ONE_HOUR),
					AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_HOUR));
			}
			else if (lRemainTime >= ONE_HOUR)	// 1시간
			{
				memset(szTmp, 0, sizeof(CHAR) * 255);
				sprintf(szTmp, "%d%s %d%s",
					(INT32)((double)lRemainTime/(double)ONE_HOUR),
					AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_HOUR),
					(INT32)((double)(lRemainTime%ONE_HOUR)/(double)ONE_MINUTE),
					AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE));
			}
			else if (lRemainTime >= ONE_MINUTE)	// 1분
			{
				memset(szTmp, 0, sizeof(CHAR) * 255);
				sprintf(szTmp, "%d%s",
					(INT32)((double)lRemainTime/(double)ONE_MINUTE),
					AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE));
			}
			else if (lRemainTime < ONE_MINUTE)		// 1분 미만
			{
				memset(szTmp, 0, sizeof(CHAR) * 255);
				sprintf(szTmp, "%d%s",
					(INT32)((double)lRemainTime/(double)1000),
					AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_SECOND));
			}

			m_csItemList.SetItemText(lIndex, 8, szTmp);
		}
		else
		{
			m_csItemList.SetItemText(lIndex, 8, _T(" - "));
		}

		if(pstItem->m_lExpireTime != 0)
		{
			TCHAR szDateString[AUTIMESTAMP_SIZE_ORACLE_TIME_STRING+1];
			AuTimeStamp::FormatTimeString(pstItem->m_lExpireTime, "%y/%m/%d %H:%M", szDateString, AUTIMESTAMP_SIZE_ORACLE_TIME_STRING+1);

			m_csItemList.SetItemText(lIndex, 9, szDateString);
		}
		else
		{
			m_csItemList.SetItemText(lIndex, 9, _T(" - "));
		}

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d", pstItem->m_lCashItemUseCount);
		m_csItemList.SetItemText(lIndex, 10, szTmp);
	}
	else
	{
		// 위치가 없으면 낭패지 뭐 -0-;;
	}

	return TRUE;
}

BOOL AlefAdminItem::ShowUpdateByItemDBID(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return TRUE;
}

BOOL AlefAdminItem::InitItemList(INT32 lTypeIndex)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(lTypeIndex < 0)
		return FALSE;

	// 화면을 비운다.
	ClearItemListCtrl();

	int iIndex = 0;
	if(lTypeIndex == ALEFADMINITEM_INDEX_EQUIP)	// Equip
	{
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_RIGHT_HAND));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_LEFT_HAND));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_HEAD));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_CHEST));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_PANTS));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_FEET));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_ARM));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_NECKLACE));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_RING) + _T("1"));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_RING) + _T("2"));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_STANDARD));
		m_csItemList.InsertItem(iIndex++, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EQUIP_RIDE));
	}
	else if(lTypeIndex >= ALEFADMINITEM_INDEX_INVEN_START && lTypeIndex <= ALEFADMINITEM_INDEX_INVEN_END)	// Inven
	{
		CHAR szTmp[32];
		memset(szTmp, 0, sizeof(CHAR) * 32);

		for(int i = 0; i < AGPMITEM_INVENTORY_ROW; i++)
		{
			for(int j = 0; j < AGPMITEM_INVENTORY_COLUMN; j++)
			{
				sprintf(szTmp, "%d, %d", i, j);
				m_csItemList.InsertItem(iIndex++, szTmp);
			}

			m_csItemList.InsertItem(iIndex++, _T(""));
		}
	}
	else if(lTypeIndex >= ALEFADMINITEM_INDEX_INVEN_PET_START && lTypeIndex <= ALEFADMINITEM_INDEX_INVEN_PET_END)
	{	
		CHAR szTmp[32];
		memset(szTmp, 0, sizeof(CHAR) * 32);

		for(int i = 0; i < AGPMITEM_SUB_ROW; i++)
		{
			for(int j = 0; j < AGPMITEM_SUB_COLUMN; j++)
			{
				sprintf(szTmp, "%d, %d", i, j);
				m_csItemList.InsertItem(iIndex++, szTmp);
			}

			m_csItemList.InsertItem(iIndex++, _T(""));
		}
	}
	else if(lTypeIndex >= ALEFADMINITEM_INDEX_BANK_START /*&& lTypeIndex <= ALEFADMINITEM_INDEX_BANK_END*/)	// Bank
	{
		CHAR szTmp[32];
		memset(szTmp, 0, sizeof(CHAR) * 32);

		for(int i = 0; i < AGPMITEM_BANK_ROW; i++)
		{
			for(int j = 0; j < AGPMITEM_BANK_COLUMN; j++)
			{
				sprintf(szTmp, "%d, %d", i, j);
				m_csItemList.InsertItem(iIndex++, szTmp);
			}

			m_csItemList.InsertItem(iIndex++, _T(""));
		}
	}
	else if(lTypeIndex == ALEFADMINITEM_INDEX_SALESBOX)	// Sales Box
	{
		CHAR szTmp[32];
		memset(szTmp, 0, sizeof(CHAR) * 32);

		for(int i = 0; i < AGPMITEM_SALES_ROW; i++)
		{
			for(int j = 0; j < AGPMITEM_SALES_COLUMN; j++)
			{
				sprintf(szTmp, "%d, %d", i, j);
				m_csItemList.InsertItem(iIndex++, szTmp);
			}

			m_csItemList.InsertItem(iIndex++, _T(""));
		}
	}
	else if(lTypeIndex == ALEFADMINITEM_INDEX_CASH_ITEM)	// Cash Inventory
	{
		CHAR szTmp[32];
		memset(szTmp, 0, sizeof(szTmp));

		for(int i = 0; i < AGPMITEM_CASH_INVENTORY_ROW; i++)
		{
			sprintf(szTmp, "%d", i);
			m_csItemList.InsertItem(iIndex++, szTmp);
		}
	}
	else if(lTypeIndex == ALEFADMINITEM_INDEX_MAILBOX)
	{
		CHAR szTmp[32];
		memset(szTmp, 0, sizeof(szTmp));

		for(int i = 0; i < AGPMMAILBOX_MAX_MAILS_IN_EFFECT; i++)
		{
			sprintf(szTmp, "%d", i);
			m_csItemList.InsertItem(iIndex++, szTmp);
		}
	}

	return TRUE;
}

BOOL AlefAdminItem::ClearItemListCtrl()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csItemList.DeleteAllItems();
	return TRUE;
}

BOOL AlefAdminItem::ShowDurability(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	SetDlgItemInt(IDC_E_DURABILITY, pstItem->m_lDurability);
	SetDlgItemInt(IDC_E_MAX_DURABILITY, pstItem->m_lMaxDurability);

	return TRUE;
}

BOOL AlefAdminItem::ClearDurability()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	SetDlgItemText(IDC_E_DURABILITY, _T(""));
	SetDlgItemText(IDC_E_MAX_DURABILITY, _T(""));
	return TRUE;
}

BOOL AlefAdminItem::ShowSkillPlus(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = NULL;
	for(int i = 0; i < AGPMITEM_MAX_SKILL_PLUS_EFFECT; ++i)
	{
		if(!pstItem->m_alSkillPlusTID[i])
			break;

		pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(pstItem->m_alSkillPlusTID[i]);
		if(!pcsSkillTemplate)
			break;

		CString szTmp(pcsSkillTemplate->m_szName);
		szTmp += _T(" +Lv1");
		m_csSkillPlusCB.AddString(szTmp);
	}

	m_csSkillPlusCB.SetCurSel(0);
	return TRUE;
}

BOOL AlefAdminItem::ClearSkillPlus()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSkillPlusCB.ResetContent();
	return TRUE;
}

BOOL AlefAdminItem::ShowConvertData(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(pstItem->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	INT32 lMaxSocket = GetMaxSocket(pstItem);
	if(lMaxSocket == 0)
		return FALSE;

	CHAR szTmp[255];
	memset(szTmp, 0, sizeof(CHAR) * 255);

	// 뿌린다.
	for(int i = 0; i < lMaxSocket; i++)
	{
		// 소켓 Index 를 먼저 쓰고
		sprintf(szTmp, "%d", i + 1);
		m_csConvertList.InsertItem(i, szTmp);

		if(pstItem->m_lNumSocket > i)
		{
			AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(pstItem->m_stSocketAttr[i].m_lTID);
			if(pstItem->m_lNumConvertedSocket > i && pcsItemTemplate)
			{
				m_csConvertList.SetItemText(i, 1, pcsItemTemplate->m_szName);
			}
			else
			{
				m_csConvertList.SetItemText(i, 1, _T(" - "));
			}
		}
		else
		{
			m_csConvertList.SetItemText(i, 1, _T(" X "));
		}
	}

	return TRUE;
}

BOOL AlefAdminItem::ShowConvertComboBox(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(pstItem->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	CHAR szTmp[8];
	memset(szTmp, 0, sizeof(CHAR) * 8);

	// 일반 강화 수치 세팅해주고
	InitConvertPhysicalCB();
	if(IsPhysicalConvertable(pstItem))
		m_csConvertPhysicalCB.SetCurSel(pstItem->m_lPhysicalConvertLevel);
	else
		m_csConvertPhysicalCB.SetCurSel(0);

	// 뚫은 소켓 세팅한 다음
	InitConvertSocketCB();
	m_csConvertSocketCB.SetCurSel(pstItem->m_lNumSocket);

	// 아이템 바른거 찾아준다.
	OnCbnSelchangeCbConvertSocket();

	return TRUE;
}

BOOL AlefAdminItem::ClearConvertData()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csConvertList.DeleteAllItems();

	InitConvertPhysicalCB();
	InitConvertSocketCB();
	m_csConvertSpiritStoneCB.SetCurSel(0);
	m_csConvertStoneCB.SetCurSel(0);

	return TRUE;
}

INT32 AlefAdminItem::GetMaxSocket(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return 0;

	AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(pstItem->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	INT32 lMaxSocket = 0;
	if(pcsItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP)
	{
		switch(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind)
		{
			case AGPMITEM_EQUIP_KIND_WEAPON:
				lMaxSocket = AGPDITEMCONVERT_MAX_WEAPON_SOCKET;
				break;

			case AGPMITEM_EQUIP_KIND_ARMOUR:
				lMaxSocket = AGPDITEMCONVERT_MAX_ARMOUR_SOCKET;
				break;

			case AGPMITEM_EQUIP_KIND_SHIELD:
			case AGPMITEM_EQUIP_KIND_RING:
			case AGPMITEM_EQUIP_KIND_NECKLACE:
				lMaxSocket = AGPDITEMCONVERT_MAX_ETC_SOCKET;
				break;
		}
	}

	return lMaxSocket;
}

BOOL AlefAdminItem::IsPhysicalConvertable(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(pstItem->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	if(pcsItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP)
		return FALSE;

	if(!(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON ||
		 (((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR &&
		  ((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nPart == AGPMITEM_PART_BODY)))
		return FALSE;

	if(pstItem->m_lPhysicalConvertLevel >= AGPDITEMCONVERT_MAX_ITEM_RANK)
		return FALSE;

	// 에고아이템인지도 확인해야 하나 아직은 그런게 없으므로 GG
	//if (IsEgoItem(pcsItem))
	//	return AGPDITEMCONVERT_RESULT_IS_EGO_ITEM;

	return TRUE;
}

// 2005.04.11.
BOOL AlefAdminItem::ShowAddedOptionData(stAgpdAdminItem* pstItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItem)
		return FALSE;

	CHAR szTmp[128];
	memset(szTmp, 0, sizeof(szTmp));

	AgpdItemOptionTemplate* pcsOptionTemplate = NULL;

	for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		if(pstItem->m_alOptionTID[i] <= 0)
			break;

		AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
		pcsOptionTemplate = ppmItem->GetItemOptionTemplate(pstItem->m_alOptionTID[i]);
		if(!pcsOptionTemplate)
			break;

		sprintf(szTmp, "%d. %s", pstItem->m_alOptionTID[i], pcsOptionTemplate->m_szDescription);
		m_csOptionAddedList.AddString(szTmp);
	}

	return TRUE;
}

BOOL AlefAdminItem::ClearAddedOptionData()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csOptionAddedList.ResetContent();
	return TRUE;
}

BOOL AlefAdminItem::ClearAddedSkillData()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSkillAddedList.ResetContent();
	return TRUE;
}

BOOL AlefAdminItem::ShowOptionDesc(INT32* pclOptionTID, INT32 lCount)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pclOptionTID || lCount == 0)
		return FALSE;

	CString szDesc = _T("");
	CString szTmp = _T("");
	AgpdItemOptionTemplate* pcsOptionTemplate = NULL;

	for(INT32 i = 0; i < lCount && i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
		pcsOptionTemplate = ppmItem->GetItemOptionTemplate(pclOptionTID[i]);
		if(!pcsOptionTemplate)
			continue;

		if(i != 0)
			szDesc += " | ";

		szTmp.Format("%s", pcsOptionTemplate->m_szDescription);
		szDesc += szTmp;
	}

	SetDlgItemText(IDC_E_OPTION_DESC, (LPCTSTR)szDesc);

	return TRUE;
}

BOOL AlefAdminItem::InitSelectedOption()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csOptionAllList.SetSel(-1, FALSE);
	SetDlgItemText(IDC_E_OPTION_DESC, _T(""));
	return TRUE;
}

BOOL AlefAdminItem::InitSelectedSkill()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSkillAllList.SetSel(-1, FALSE);
	return TRUE;
}

BOOL AlefAdminItem::GetSelctedItemPosition(stAgpdAdminItemOperation* pstItemOperation)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItemOperation)
		return FALSE;

	if(m_stSelectedItem.m_lTID == 0)
		return FALSE;

	pstItemOperation->m_lPos = m_stSelectedItem.m_lPos;
	pstItemOperation->m_lLayer = m_stSelectedItem.m_lLayer;
	pstItemOperation->m_lRow = m_stSelectedItem.m_lRow;
	pstItemOperation->m_lColumn = m_stSelectedItem.m_lCol;
	pstItemOperation->m_nPart = m_stSelectedItem.m_nPart;

	return TRUE;
}

INT32 AlefAdminItem::GetNowSetPhysicalConvertLevel()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	INT32 lCurSel = m_csConvertPhysicalCB.GetCurSel();
	if(lCurSel == CB_ERR || lCurSel > AGPDITEMCONVERT_MAX_ITEM_RANK)
		return 0;

	return lCurSel;
}

INT32 AlefAdminItem::GetNowSetSocketNum()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	INT32 lCurSel = m_csConvertSocketCB.GetCurSel();
	if(lCurSel == CB_ERR)
		return 0;

	return lCurSel;
}

INT32 AlefAdminItem::GetNowSetOption(stAgpdAdminItemOperation* pstItemOperation)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItemOperation)
		return 0;

	INT32 lCount = m_csOptionAddedList.GetCount();
	if(lCount <= 0 || lCount > AGPDITEM_OPTION_MAX_NUM)
	{
		pstItemOperation->m_alOptionTID[0] = -1;
		return 0;
	}

	CString szTmp = _T("");
	INT32 lIndex = 0;
	INT32 lOptionIndex = 0;
	for(INT32 i = 0; i < lCount && i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		m_csOptionAddedList.GetText(i, szTmp);
		lIndex = szTmp.Find(".");

		if(lIndex < 0 || lIndex >= szTmp.GetLength())
			continue;

		pstItemOperation->m_alOptionTID[lOptionIndex] = atoi((LPCTSTR)szTmp.Left(lIndex + 1));
		lOptionIndex++;
	}

	return lCount;
}

INT32 AlefAdminItem::GetNowSetSkill(stAgpdAdminItemOperation* pstItemOperation)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItemOperation)
		return 0;

	INT32 lCount = m_csSkillAddedList.GetCount();
	if(lCount <= 0 || lCount > AGPMITEM_MAX_SKILL_PLUS_EFFECT)
	{
		pstItemOperation->m_alSkillPlusTID[0] = -1;
		return 0;
	}

	CString szTmp = _T("");
	INT32 lIndex = 0;
	INT32 lOptionIndex = 0;
	for(INT32 i = 0; i < lCount && i < AGPMITEM_MAX_SKILL_PLUS_EFFECT; i++)
	{
		m_csSkillAddedList.GetText(i, szTmp);
		lIndex = szTmp.Find(".");

		if(lIndex < 0 || lIndex >= szTmp.GetLength())
			continue;

		pstItemOperation->m_alSkillPlusTID[lOptionIndex] = atoi((LPCTSTR)szTmp.Left(lIndex + 1));
		lOptionIndex++;
	}

	return lCount;
}

// 2006.01.03. steeple
BOOL AlefAdminItem::GetCashUsingAndTimeDate(stAgpdAdminItemOperation* pstItemOperation)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItemOperation)
		return FALSE;

	// 일단 초기화
	pstItemOperation->m_nInUseItem = 0;
	pstItemOperation->m_lRemainTime = 0;
	pstItemOperation->m_lExpireTime = 0;

	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	AgpdItemTemplate* pcsItemTemplate = ppmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) == FALSE)
		return FALSE;

	//if(m_csChkUsingBtn.GetCheck() == BST_CHECKED)
	//	pstItemOperation->m_nInUseItem = 1;
	//else
	//	return TRUE;

	if(pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_PLAY_TIME)
	{
		// 남은 시간을 구한다.
		INT64 lDay = GetDlgItemInt(IDC_E_R_TIME_DAY);
		INT32 lHour = GetDlgItemInt(IDC_E_R_TIME_HOUR);
		INT32 lMin = GetDlgItemInt(IDC_E_R_TIME_MIN);
		INT32 lSec = GetDlgItemInt(IDC_E_R_TIME_SEC);

		if(lDay < 0 || lHour < 0 || lMin < 0 || lSec < 0)
			return FALSE;

		pstItemOperation->m_lRemainTime = lDay * 24 * 60 * 60 * 1000 +
										  lHour * 60 * 60 * 1000 +
										  lMin * 60 * 1000 +
										  lSec * 1000;
	}
	else if(pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_REAL_TIME)
	{
		int iUseExpireDateDefault = m_csExpireDateDefaultCB.GetCurSel();
		if(iUseExpireDateDefault == 0)
		{
			// 아이템 종료일을 지정해준다.
			INT32 lYear = GetDlgItemInt(IDC_CB_E_DATE_YEAR);
			INT32 lMonth = GetDlgItemInt(IDC_CB_E_DATE_MONTH);
			INT32 lDay = GetDlgItemInt(IDC_CB_E_DATE_DAY);
			INT32 lHour = GetDlgItemInt(IDC_CB_E_DATE_HOUR);
			INT32 lMin = GetDlgItemInt(IDC_CB_E_DATE_MIN);

			if(lYear < 2006 || lYear > 2030 ||
				lMonth < 1 || lMonth > 12 ||
				lDay < 1 || lDay > 31 ||
				lHour < 0 || lHour > 23 ||
				lMin < 0 || lMin > 59)
				return FALSE;

			TCHAR szOracleTime[AUTIMESTAMP_SIZE_ORACLE_TIME_STRING + 1];
			memset(szOracleTime, 0, sizeof(szOracleTime));
			
			wsprintf(szOracleTime, "%04d/%02d/%02d %02d:%02d:00", lYear, lMonth, lDay, lHour, lMin);
			pstItemOperation->m_lExpireTime = AuTimeStamp::ConvertOracleTimeToTimeStamp(szOracleTime);
		}
		else
			pstItemOperation->m_lExpireTime = 0;	// 새걸루 만들어준다.
	}

	return TRUE;
}

BOOL AlefAdminItem::OnReceiveResult(stAgpdAdminItemOperation* pstItemOperation)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstItemOperation)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	CString szMsg = _T("");
	CString szFormat = _T("");

	BOOL bRefresh = TRUE;

	switch(pstItemOperation->m_cResult)
	{
		case AGPMADMIN_ITEM_RESULT_SUCCESS_CREATE:
			szFormat.LoadString(IDS_ITEM_CREATE_SUCCESS);
			szMsg.Format((LPCTSTR)szFormat, pcsItemTemplate ? pcsItemTemplate->m_szName : _T("Invalid Template"), pstItemOperation->m_lCount);
			break;

		case AGPMADMIN_ITEM_RESULT_FAILED_INVEN_FULL:
			szFormat.LoadString(IDS_ITEM_CREATE_FAILURE);
			szMsg.Format((LPCTSTR)szFormat, pcsItemTemplate ? pcsItemTemplate->m_szName : _T("Invalid Template"));
			break;

		case AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_PHY:
			szFormat.LoadString(IDS_ITEM_CONVERT_SUCCESS_PHY);
			szMsg.Format((LPCTSTR)szFormat, pcsItemTemplate ? pcsItemTemplate->m_szName : _T("Invalid Template"), pstItemOperation->m_cPhysicalConvertLevel);
			break;

		case AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_SOCKET:
			szFormat.LoadString(IDS_ITEM_CONVERT_SUCCESS_SOCKET);
			szMsg.Format((LPCTSTR)szFormat, pcsItemTemplate ? pcsItemTemplate->m_szName : _T("Invalid Template"), pstItemOperation->m_cSocket);
			break;

		case AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_ADDITEM:
		{
			AgpdItemTemplate* pcsStoneTemplate = GetItemTemplate(pstItemOperation->m_lUsableItemTID);

			szFormat.LoadString(IDS_ITEM_CONVERT_SUCCESS_STONE);
			szMsg.Format((LPCTSTR)szFormat, pcsItemTemplate ? pcsItemTemplate->m_szName : _T("Invalid Template"),
											pcsStoneTemplate ? pcsStoneTemplate->m_szName : _T("Invalid Template"));
			break;
		}

		case AGPMADMIN_ITEM_RESULT_FAILED_CONVERT:
			szFormat.LoadString(IDS_ITEM_CONVERT_FAILURE);
			szMsg.Format((LPCTSTR)szFormat, pcsItemTemplate ? pcsItemTemplate->m_szName : _T("Invalid Template"));
			break;

		case AGPMADMIN_ITEM_RESULT_SUCCESS_DELETE:
			szFormat.LoadString(IDS_ITEM_DELETE_SUCCESS);
			szMsg.Format((LPCTSTR)szFormat, pcsItemTemplate ? pcsItemTemplate->m_szName : _T("Invalid Template"));
			break;

		case AGPMADMIN_ITEM_RESULT_FAILED_DELETE:
			szFormat.LoadString(IDS_ITEM_DELETE_FAILURE);
			szMsg = szFormat;
			break;

		case AGPMADMIN_ITEM_RESULT_SUCCESS_OPTINO_ADD:
			szFormat.LoadString(IDS_ITEM_OPTION_ADD_SUCCESS);
			szMsg = szFormat;
			break;

		case AGPMADMIN_ITEM_RESULT_SUCCESS_OPTION_REMOVE:
			szFormat.LoadString(IDS_ITEM_OPTION_REMOVE_SUCCESS);
			szMsg = szFormat;
			break;

		case AGPMADMIN_ITEM_RESULT_FAILED_OPTION_ADD:
		case AGPMADMIN_ITEM_RESULT_FAILED_OPTION_REMOVE:
			bRefresh = FALSE;
			break;
	}

	if(szMsg.GetLength() > 0)
		AlefAdminManager::Instance()->m_pMainDlg->ProcessAdminMessage((LPCTSTR)szMsg);

	// Refresh 해준다.
	if(bRefresh)
		AlefAdminManager::Instance()->m_pMainDlg->SearchCharacter(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData.m_stBasic.m_szCharName);

	return TRUE;
}







int AlefAdminItem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL AlefAdminItem::OnEraseBkgnd(CDC* pDC)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UNREFERENCED_PARAMETER(pDC);	
	return TRUE;
	//return CFormView::OnEraseBkgnd(pDC);
}

void AlefAdminItem::OnPaint()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CFormView::OnPaint()을(를) 호출하지 마십시오.

	CRect r;
	GetClientRect(&r);
	CXTMemDC memDC(&dc, r);
	
	CFormView::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
}

void AlefAdminItem::OnCbnSelchangeCbPos()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iIndex = m_csItemPosCB.GetCurSel();
	if(iIndex == CB_ERR)
		return;

	SelectItem(NULL);
	ClearDurability();
	ClearSkillPlus();
	ClearConvertData();
	ClearAddedOptionData();
	ClearAddedSkillData();

	InitItemList(iIndex);

	if(iIndex == ALEFADMINITEM_INDEX_CASH_ITEM &&
		AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_4)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	ShowData(iIndex);
}

void AlefAdminItem::OnBnClickedBCreate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 권한체크 2005.09.15.
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	INT32 lTID = GetDlgItemInt(IDC_E_CREATE_TID);
	if(lTID == 0)
		return;

	INT32 lCount = GetDlgItemInt(IDC_E_CREATE_COUNT);
	if(lCount == 0) lCount = 1;

	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	AgpdItemTemplate* pcsItemTemplate = ppmItem->GetItemTemplate(lTID);
	if(!pcsItemTemplate)
		return;

	CString szMsg = _T("");
	CString szFormat = _T("");

	int iCreateReason = m_csCreateReasonCB.GetCurSel();
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && iCreateReason == 0)
	{
		szMsg.LoadString(IDS_ITEM_INSERT_CREATE_REASON);
		::AfxMessageBox((LPCTSTR)szMsg);
		return;
	}

	// 유료아이템 생성은 레벨5 부터 가능.
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_5)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	stAgpdAdminItemOperation stItemOperation;
	memset(&stItemOperation, 0, sizeof(stItemOperation));

	if(strlen(AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) > 0)
	{
		szFormat.LoadString(IDS_ITEM_CREATE_OWNER);
        szMsg.Format((LPCTSTR)szFormat, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName, pcsItemTemplate->m_szName);

		INT32 lResult = ::AfxMessageBox((LPCTSTR)szMsg, MB_YESNOCANCEL);
		if(lResult == IDYES)
		{
			strcpy(stItemOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);
			strcpy(stItemOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData.m_stSub.m_szAccName);
		}
		else if(lResult == IDNO)
		{
			strcpy(stItemOperation.m_szCharName, AlefAdminManager::Instance()->m_stSelfAdminInfo.m_szAdminName);
			strcpy(stItemOperation.m_szAccName, AlefAdminManager::Instance()->m_stSelfAdminInfo.m_szAccName);
		}
		else
			return;

		szFormat.LoadString(IDS_ITEM_CREATE_ACCNAME_CONFIRM);
		szMsg.Format((LPCTSTR)szFormat, stItemOperation.m_szCharName, stItemOperation.m_szAccName);
		if(::AfxMessageBox((LPCTSTR)szMsg, MB_YESNO) == IDNO)
			return;
	}

	stItemOperation.m_cOperation = AGPMADMIN_ITEM_CREATE;
	stItemOperation.m_cReason = (INT8)iCreateReason;
	stItemOperation.m_lTID = lTID;
	stItemOperation.m_lCount = lCount;

	stItemOperation.m_cPhysicalConvertLevel = GetNowSetPhysicalConvertLevel();
	stItemOperation.m_cSocket = GetNowSetSocketNum();
	GetNowSetOption(&stItemOperation);
	GetNowSetSkill(&stItemOperation);

	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) == FALSE)
	{
		// 캐쉬아이템이 아닐 경우는 그냥 인벤토리를 돌고
		if(!_FindEmptySlot( Inventory_Normal, &stItemOperation))
		{
			szFormat.LoadString(IDS_ITEM_CREATE_INVEN_FULL);
			szMsg.Format((LPCTSTR)szFormat);
			::AfxMessageBox((LPCTSTR)szMsg);
			return;
		}
	}
	else
	{
		// 캐쉬 아이템일 경우는 로니인벤토리를 돈다.
		if(!_FindEmptySlot( Inventory_Cash, &stItemOperation))
		{
			szFormat.LoadString(IDS_ITEM_CREATE_INVEN_FULL);
			szMsg.Format((LPCTSTR)szFormat);
			::AfxMessageBox((LPCTSTR)szMsg);
			return;
		}

		// 사용중, 남은시간, 종료일 정보를 얻는다.
		GetCashUsingAndTimeDate(&stItemOperation);
	}

	AlefAdminManager::Instance()->GetAdminData()->SendItemOperation(&stItemOperation);

	InitSelectedOption();
	InitSelectedSkill();

	// 리프레쉬 하라는 메시지 창을 띄어준다.
	szFormat.LoadString(IDS_ITEM_REFRESH);
	szMsg.Format((LPCTSTR)szFormat);
	::AfxMessageBox((LPCTSTR)szMsg);
}

void AlefAdminItem::OnNMClickLcItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 먼저 비우시기
	ClearDurability();
	ClearSkillPlus();
	ClearConvertData();
	ClearAddedOptionData();
	ClearAddedSkillData();

	// 임시 Item structure
	stAgpdAdminItem stItem;
	memset(&stItem, 0, sizeof(stItem));

	// 클릭한 곳의 아이템 정보를 얻어온다.
	if(!GetItemPosByIndex(&stItem, nlv->iItem))
	{
		SelectItem(NULL);
		return;
	}

	// 위에서 얻은 것을 바탕으로 list 에서 얻는다.
	stAgpdAdminItem* pstItem = NULL;
	switch(stItem.m_lPos)
	{
	case AGPDITEM_STATUS_INVENTORY :		pstItem = GetItem( Inventory_Normal, &stItem );		break;
	case AGPDITEM_STATUS_EQUIP :			pstItem = GetItem( Inventory_Equip, &stItem );		break;
	case AGPDITEM_STATUS_BANK :				pstItem = GetItem( Inventory_Bank, &stItem );		break;
	case AGPDITEM_STATUS_SALESBOX_GRID :	pstItem = GetItem( Inventory_Sales, &stItem );		break;
	case AGPDITEM_STATUS_SUB_INVENTORY :	pstItem = GetItem( Inventory_Pet, &stItem );		break;
	case AGPDITEM_STATUS_CASH_INVENTORY :	pstItem = GetItem( Inventory_Cash, &stItem );		break;
	case AGPDITEM_STATUS_MAILBOX :			pstItem = GetItem( Inventory_MailBox, &stItem );	break;
	}

	SelectItem(pstItem);

	// list 에서 얻었으면 Convert 정보를 뿌린다.
	if(pstItem)
	{
		ShowDurability(pstItem);
		ShowSkillPlus(pstItem);
		ShowConvertData(pstItem);
		ShowConvertComboBox(pstItem);
		ShowAddedOptionData(pstItem);

		CString szMsg = _T("");
		szMsg.Format("Item Sequence ID (DB ID) : %I64d", pstItem->m_ullDBID);
		AlefAdminManager::Instance()->m_pMainDlg->ProcessActionMessage((LPCTSTR)szMsg);
	}

	*pResult = 0;
}

void AlefAdminItem::OnCbnSelchangeCbConvertPhysical()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csConvertSocketCB.SetCurSel(0);
	m_csConvertSpiritStoneCB.SetCurSel(0);
	m_csConvertStoneCB.SetCurSel(0);
}

void AlefAdminItem::OnCbnSelchangeCbConvertSocket()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 선택된 아이템이 없으면 그냥 나간다.
	if(m_stSelectedItem.m_lTID == 0)
		return;

	int iCurSel = m_csConvertSocketCB.GetCurSel();
	if(iCurSel == CB_ERR || iCurSel <= 0)
		return;

	// Physical 은 처음값으로 돌린다.
	m_csConvertPhysicalCB.SetCurSel(m_stSelectedItem.m_lPhysicalConvertLevel);

	iCurSel--;	// 하나 빼줘야 인덱스가 맞는다.

	// 얘네들을 초기화 해주고~
	m_csConvertSpiritStoneCB.SetCurSel(0);
	m_csConvertStoneCB.SetCurSel(0);
	if(iCurSel >= m_stSelectedItem.m_lNumConvertedSocket)
		return;

	AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(m_stSelectedItem.m_stSocketAttr[iCurSel].m_lTID);
	if(!pcsItemTemplate)
		return;

	CString szTmp = _T("");
	if(m_stSelectedItem.m_stSocketAttr[iCurSel].m_bIsSpiritStone)
	{
		for(int i = 0; i < m_csConvertSpiritStoneCB.GetCount(); i++)
		{
			m_csConvertSpiritStoneCB.GetLBText(i, szTmp);
			if(szTmp.Compare(pcsItemTemplate->m_szName) == 0)
			{
				m_csConvertSpiritStoneCB.SetCurSel(i);
				break;
			}
		}
	}
	else
	{
		for(int i = 0; i < m_csConvertStoneCB.GetCount(); i++)
		{
			m_csConvertStoneCB.GetLBText(i, szTmp);
			if(szTmp.Compare(pcsItemTemplate->m_szName) == 0)
			{
				m_csConvertStoneCB.SetCurSel(i);
				break;
			}
		}
	}
}


void AlefAdminItem::OnBnClickedBConvert()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 권한체크 2005.09.15.
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(m_stSelectedItem.m_lTID == 0)
		return;

	if(m_stSelectedItem.m_lPos != AGPDITEM_STATUS_INVENTORY)
	{
		AfxMessageBox(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SET_ON_INVENTORY));
		return;
	}

	CString szFormat = _T("");
	szFormat.LoadString(IDS_ITEM_CONVERT_CONFIRM);
	if(::AfxMessageBox((LPCTSTR)szFormat, MB_YESNO) != IDYES)
		return;

	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		szFormat.LoadString(IDS_ITEM_CONVERT_WARNING);
		if(::AfxMessageBox((LPCTSTR)szFormat, MB_YESNO) != IDYES)
			return;
	}

	AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(m_stSelectedItem.m_lTID);
	if(!pcsItemTemplate)
		return;

	// 유료아이템 생성은 레벨5 부터 가능.
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_5)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	stAgpdAdminItemOperation stItemOperation;
	memset(&stItemOperation, 0, sizeof(stItemOperation));

	strcpy(stItemOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);
	strcpy(stItemOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName());

	stItemOperation.m_cOperation = AGPMADMIN_ITEM_CONVERT;
	stItemOperation.m_ullDBID = m_stSelectedItem.m_ullDBID;
	stItemOperation.m_lID = m_stSelectedItem.m_lID;
	stItemOperation.m_lTID = m_stSelectedItem.m_lTID;

	GetSelctedItemPosition(&stItemOperation);

	// 걍 원래 아이템 정보를 통째로 복사한다. 2005.05.30.
	memcpy(&stItemOperation.m_stOriginalItem, &m_stSelectedItem, sizeof(m_stSelectedItem));

	szFormat.LoadString(IDS_ITEM_REFRESH);
	::AfxMessageBox((LPCTSTR)szFormat);
	
	// Physical 강화부터 체크
	int iCurSel = m_csConvertPhysicalCB.GetCurSel();
	if(iCurSel >= 0 && iCurSel != m_stSelectedItem.m_lPhysicalConvertLevel)
	{
		// 강화시켜준다. Level 다운도 가능하다.
		stItemOperation.m_cPhysicalConvertLevel = (INT8)iCurSel;
		AlefAdminManager::Instance()->GetAdminData()->SendItemOperation(&stItemOperation);
		return;
	}

	// 2005.06.20. steeple
	// Physical 이 아니라면 원래 값대로 맞춰준다.
	stItemOperation.m_cPhysicalConvertLevel = m_stSelectedItem.m_lPhysicalConvertLevel;

	iCurSel = m_csConvertSocketCB.GetCurSel();
	if(iCurSel >= 0)
	{
		int iSpiritCurSel = m_csConvertSpiritStoneCB.GetCurSel();
		int iStoneCurSel = m_csConvertStoneCB.GetCurSel();

		if(iCurSel < m_stSelectedItem.m_lNumSocket &&
			iSpiritCurSel < 1 && iStoneCurSel < 1)			// 소켓 줄일 때는 무조건 Init 이다.
		{
			szFormat.LoadString(IDS_ITEM_INIT_SOCKET);
			if(::AfxMessageBox((LPCTSTR)szFormat, MB_YESNO) == IDNO)
				return;
		}

		// 2005.07.06. steeple
		// Socket 개수 체크 약간 변경
		if(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
		{
			if(iCurSel > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
				iCurSel = -1;
		}
		else if(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR)
		{
			if(iCurSel > AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
				iCurSel = -1;
		}
		else if(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_RING ||
			((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_NECKLACE ||
			((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD)
		{
			if(iCurSel > AGPDITEMCONVERT_MAX_ETC_SOCKET)
				iCurSel = -1;
		}
		else
		{
			// 개조할 수 없는 아이템
			iCurSel = -1;
		}

		if(iCurSel < 0)
		{
			szFormat.LoadString(IDS_ITEM_CONVERT_FAILURE);
			::AfxMessageBox((LPCTSTR)szFormat);
			return;
		}

		stItemOperation.m_cSocket = (INT8)iCurSel;

		// 정령석, 기원석이 있는 지 보구 있으면 세팅
		if(iSpiritCurSel > 0)
		{
			CString szTmp = _T("");
			m_csConvertSpiritStoneCB.GetLBText(iSpiritCurSel, szTmp);

			int iCommaIndex = szTmp.Find(_T(","));
			CString szTID = szTmp.Mid(iCommaIndex + 1);

			AgpdItemTemplate* pcsSpiritStoneTemplate = GetItemTemplate(atoi((LPCTSTR)szTID));
			if(pcsSpiritStoneTemplate)
			{
				stItemOperation.m_cSocket = m_stSelectedItem.m_lNumSocket;
				stItemOperation.m_lUsableItemTID = pcsSpiritStoneTemplate->m_lID;
			}
		}
		else if(iStoneCurSel > 0)
		{
			CString szTmp = _T("");
			m_csConvertStoneCB.GetLBText(iStoneCurSel, szTmp);

			int iCommaIndex = szTmp.Find(_T(","));
			CString szTID = szTmp.Mid(iCommaIndex + 1);

			AgpdItemTemplate* pcsStoneTemplate = GetItemTemplate(atoi((LPCTSTR)szTID));
			if(pcsStoneTemplate)
			{
				stItemOperation.m_cSocket = m_stSelectedItem.m_lNumSocket;
				stItemOperation.m_lUsableItemTID = pcsStoneTemplate->m_lID;
			}
		}

		// 참고로 정령석 바르는 것은 넘겨준 소켓 인덱스와는 상관없이 현재 바를 수 있는 제일 먼저번에다가 바른다.
		AlefAdminManager::Instance()->GetAdminData()->SendItemOperation(&stItemOperation);
		return;
	}
}

void AlefAdminItem::OnCbnSelchangeCbConvertSpiritStone()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 정령석을 선택하려 하므로 기원석의 선택은 없애준다.
	m_csConvertStoneCB.SetCurSel(0);
}

void AlefAdminItem::OnCbnSelchangeCbConvertStone()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 기원석을 선택하려 하므로 정령석의 선택은 없애준다.
	m_csConvertSpiritStoneCB.SetCurSel(0);
}

void AlefAdminItem::OnBnClickedBRefreshConvert()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!m_stSelectedItem.m_lTID)
		return;

	// 검색을 다시 날리면 되려나.
	AlefAdminManager::Instance()->m_pMainDlg->SearchCharacter(m_stSelectedItem.m_szCharName);
}

void AlefAdminItem::OnBnClickedBDelete()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 권한체크 2005.09.15.
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(!m_stSelectedItem.m_lTID)
		return;

	AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(m_stSelectedItem.m_lTID);
	if(!pcsItemTemplate)
		return;

	CString szFormat = _T("");
	CString szMsg = _T("");

	szFormat.LoadString(IDS_ITEM_DELETE_CONFIRM);
	szMsg.Format((LPCTSTR)szFormat);
	if(::AfxMessageBox((LPCTSTR)szMsg, MB_YESNO) == IDNO)
		return;

	// 유료아이템 생성은 레벨5 부터 가능.
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_5)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	int iDeleteReason = m_csDeleteReasonCB.GetCurSel();
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && iDeleteReason == 0)
	{
		szMsg.LoadString(IDS_ITEM_INSERT_DELETE_REASON);
		::AfxMessageBox((LPCTSTR)szMsg);
		return;
	}

	// 창고 아이템이라면 확인창을 띄워준다. 2006.03.28. steeple
	if(m_stSelectedItem.m_lPos == AGPDITEM_STATUS_BANK)
	{
		::AfxMessageBox(IDS_BANK_ITEM_DELETE_CONFIRM);
		szMsg.Format(IDS_BANK_ITEM_DELETE_CONFIRM2, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData.m_stSub.m_szAccName);
		if(::AfxMessageBox((LPCTSTR)szMsg, MB_YESNO) == IDNO)
			return;
	}
	
	stAgpdAdminItemOperation stItemOperation;
	memset(&stItemOperation, 0, sizeof(stItemOperation));

	strcpy(stItemOperation.m_szCharName, m_stSelectedItem.m_szCharName);
	strcpy(stItemOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName());

	stItemOperation.m_cOperation = AGPMADMIN_ITEM_DELETE;
	stItemOperation.m_cReason = (INT8)iDeleteReason;
	stItemOperation.m_ullDBID = m_stSelectedItem.m_ullDBID;
	stItemOperation.m_lID = m_stSelectedItem.m_lID;
	stItemOperation.m_lTID = pcsItemTemplate->m_lID;

	stItemOperation.m_lPos = m_stSelectedItem.m_lPos;
	stItemOperation.m_lLayer = m_stSelectedItem.m_lLayer;
	stItemOperation.m_lRow = m_stSelectedItem.m_lRow;
	stItemOperation.m_lColumn = m_stSelectedItem.m_lCol;
	stItemOperation.m_nPart = m_stSelectedItem.m_nPart;

	AlefAdminManager::Instance()->GetAdminData()->SendItemOperation(&stItemOperation);

	// 리프레쉬 하라는 메시지 창을 띄어준다.
	szFormat.LoadString(IDS_ITEM_REFRESH);
	szMsg.Format((LPCTSTR)szFormat);
	::AfxMessageBox((LPCTSTR)szMsg);
}

void AlefAdminItem::OnBnClickedBRefresh()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	AlefAdminManager::Instance()->m_pMainDlg->SearchCharacter(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData.m_stBasic.m_szCharName);
}

void AlefAdminItem::OnNMClickLItemConvert(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if(nlv->iItem < 0)
		return;

	CString szTmp = m_csConvertList.GetItemText(nlv->iItem, 0);
	INT32 lSocket = atoi((LPCTSTR)szTmp);
	if(lSocket < 0)
		return;

	m_csConvertSocketCB.SetCurSel(lSocket);
	//OnCbnSelchangeCbCreateSecond();

	*pResult = 0;
}

void AlefAdminItem::OnLbnSelchangeLOptionAdded()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// All List 의 셀렉션을 없애준다.
	m_csOptionAllList.SetSel(-1, FALSE);

	INT32 lCount = m_csOptionAddedList.GetSelCount();
	if(lCount <= 0)
	{
		SetDlgItemText(IDC_E_OPTION_DESC, _T(""));
		return;
	}

	CArray<INT32, INT32> arrListBoxSel;
	arrListBoxSel.SetSize(lCount);
	m_csOptionAddedList.GetSelItems(lCount, arrListBoxSel.GetData());

	INT32 alOptionTID[AGPDITEM_OPTION_MAX_NUM];
	memset(&alOptionTID, 0, sizeof(alOptionTID));

	INT32 lIndex = 0, lRealCount = 0;;
	CString szTmp = _T("");
	for(INT32 i = 0; i < lCount && i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		m_csOptionAddedList.GetText(arrListBoxSel.GetAt(i), szTmp);
		lIndex = szTmp.Find(".");

		if(lIndex < 0 || lIndex >= szTmp.GetLength())
			continue;
        
		alOptionTID[i] = atoi((LPCTSTR)szTmp.Left(lIndex + 1));
		lRealCount++;
	}

	ShowOptionDesc(alOptionTID, lRealCount);
}

void AlefAdminItem::OnLbnSelchangeLOptionAll()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Added List 의 셀렉션을 없애준다.
	m_csOptionAddedList.SetSel(-1, FALSE);

	INT32 lCount = m_csOptionAllList.GetSelCount();
	if(lCount <= 0)
	{
		SetDlgItemText(IDC_E_OPTION_DESC, _T(""));
		return;
	}

	CArray<INT32, INT32> arrListBoxSel;
	arrListBoxSel.SetSize(lCount);
	m_csOptionAllList.GetSelItems(lCount, arrListBoxSel.GetData());

	INT32 alOptionTID[AGPDITEM_OPTION_MAX_NUM];
	memset(&alOptionTID, 0, sizeof(alOptionTID));

	INT32 lIndex = 0, lRealCount = 0;;
	CString szTmp = _T("");
	for(INT32 i = 0; i < lCount && i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		m_csOptionAllList.GetText(arrListBoxSel.GetAt(i), szTmp);
		lIndex = szTmp.Find(".");

		if(lIndex < 0 || lIndex >= szTmp.GetLength())
			continue;
        
		alOptionTID[i] = atoi((LPCTSTR)szTmp.Left(lIndex + 1));
		lRealCount++;
	}

	ShowOptionDesc(alOptionTID, lRealCount);
}

void AlefAdminItem::OnBnClickedBOptionAdd()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 권한체크 2005.09.15.
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	INT32 lCount = m_csOptionAllList.GetSelCount();
	if(lCount <= 0)
		return;

	INT32 lCount2 = m_csOptionAddedList.GetCount();
	if(lCount2 + lCount > AGPDITEM_OPTION_MAX_NUM)
	{
		CHAR szTmp[64];
		sprintf(szTmp, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_MAX_OPTION), AGPDITEM_OPTION_MAX_NUM);
		AfxMessageBox(szTmp);
		return;
	}

	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	AgpdItemTemplate* pcsItemTemplate = NULL;

	// 2006.01.11. steeple
	// 캐릭터가 접속중이 아니라면, 선택한 것을 변경할 수 없고 생성 시에만 쓰인다.
	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		if(m_stSelectedItem.m_lTID != 0)
		{
			::AfxMessageBox(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CANNOT_ADD_OPTION_TO_LOGOUT_USER));
			return;
		}

		pcsItemTemplate = ppmItem->GetItemTemplate(GetDlgItemInt(IDC_E_CREATE_TID));
	}
	else
	{
		// 접속중인데 선택한 아이템이 인벤토리에 있지 않다면
		if(m_stSelectedItem.m_lTID != 0 && m_stSelectedItem.m_lPos != AGPDITEM_STATUS_INVENTORY)
		{
			::AfxMessageBox(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SET_ON_INVENTORY));
			return;
		}

		if(m_stSelectedItem.m_lTID != 0)
			pcsItemTemplate = ppmItem->GetItemTemplate(m_stSelectedItem.m_lTID);
		else
			pcsItemTemplate = ppmItem->GetItemTemplate(GetDlgItemInt(IDC_E_CREATE_TID));
	}

	if(!pcsItemTemplate)
	{
		::AfxMessageBox(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_NOT_SELECTED_ITEM));
		return;
	}

	// 유료아이템 생성은 레벨5 부터 가능.
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_5)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	CArray<INT32, INT32> arrListBoxSel;
	arrListBoxSel.SetSize(lCount);
	m_csOptionAllList.GetSelItems(lCount, arrListBoxSel.GetData());

	// Option Add 날릴 준비~
	stAgpdAdminItemOperation stItemOperation;
	memset(&stItemOperation, 0, sizeof(stItemOperation));

	strcpy(stItemOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);

	stItemOperation.m_cOperation = AGPMADMIN_ITEM_OPTION_ADD;
	stItemOperation.m_lID = m_stSelectedItem.m_lID;
	stItemOperation.m_lTID = pcsItemTemplate->m_lID;

	GetSelctedItemPosition(&stItemOperation);

	INT32 lIndex = 0;
	CString szTmp = _T("");
	for(INT32 i = 0; i < lCount && i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		m_csOptionAllList.GetText(arrListBoxSel.GetAt(i), szTmp);
		lIndex = szTmp.Find(".");

		if(lIndex < 0 || lIndex >= szTmp.GetLength())
			continue;
        
		stItemOperation.m_alOptionTID[i] = atoi((LPCTSTR)szTmp.Left(lIndex + 1));

		// 선택한 아이템이 없을 때에는 텍스트 넣어준다.
		if(m_stSelectedItem.m_lTID == 0)
			m_csOptionAddedList.AddString((LPCTSTR)szTmp);
	}

	// 선택한 아이템이 있는 경우에만 보낸다.
	if(m_stSelectedItem.m_lTID != 0)
	{
		AlefAdminManager::Instance()->GetAdminData()->SendItemOperation(&stItemOperation);
	}

	InitSelectedOption();
}

void AlefAdminItem::OnBnClickedBOptionRemove()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 권한체크 2005.09.15.
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	INT32 lCount = m_csOptionAddedList.GetSelCount();
	if(lCount <= 0)
		return;


	CArray<INT32, INT32> arrListBoxSel;
	arrListBoxSel.SetSize(lCount);
	m_csOptionAddedList.GetSelItems(lCount, arrListBoxSel.GetData());

	// Option Remove 날릴 준비~
	stAgpdAdminItemOperation stItemOperation;
	memset(&stItemOperation, 0, sizeof(stItemOperation));

	strcpy(stItemOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);

	stItemOperation.m_cOperation = AGPMADMIN_ITEM_OPTION_REMOVE;
	stItemOperation.m_lID = m_stSelectedItem.m_lID;
	stItemOperation.m_lTID = m_stSelectedItem.m_lTID;

	GetSelctedItemPosition(&stItemOperation);

	INT32 lIndex = 0;
	CString szTmp = _T("");
	for(INT32 i = lCount - 1; i > -1; i--)
	{
		m_csOptionAddedList.GetText(arrListBoxSel.GetAt(i), szTmp);
		lIndex = szTmp.Find(".");

		if(lIndex < 0 || lIndex >= szTmp.GetLength())
			continue;
	       
		stItemOperation.m_alOptionTID[i] = atoi((LPCTSTR)szTmp.Left(lIndex + 1));

		if(m_stSelectedItem.m_lTID == 0)
			m_csOptionAddedList.DeleteString(arrListBoxSel.GetAt(i));
	}
	
	// 선택한 아이템이 있는 경우에만 보낸다.
	if(m_stSelectedItem.m_lTID == 0)
		return;

	AlefAdminManager::Instance()->GetAdminData()->SendItemOperation(&stItemOperation);
	InitSelectedOption();
}

void AlefAdminItem::OnBnClickedOptionSearch()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString szOID = _T("");
	GetDlgItemText(IDC_OPT_SEARCH_BOX, szOID);

	CString szText = _T("");
	CString szExistOID = _T("");

	int iAllCount = m_csOptionAllList.GetCount();
	int iCommaIndex = 0;

	int iCurSel = 0;
	while(iCurSel < iAllCount)
	{
		m_csOptionAllList.GetText(iCurSel++, szText);

		if(szText.IsEmpty())
			continue;

		iCommaIndex = szText.Find(_T("."));
		szExistOID = szText.Mid(0, iCommaIndex);

		// 찾았3
		if(atoi((LPCTSTR)szOID) == atoi((LPCTSTR)szExistOID))
		{
			m_csOptionAllList.SetCurSel(iCurSel-1);
			m_csOptionAllList.SetSel(iCurSel-1);
			break;
		}
	}
}

void AlefAdminItem::OnBnClickedBSkillAdd()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 권한체크 2005.09.15.
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	INT32 lCount = m_csSkillAllList.GetSelCount();
	if(lCount <= 0)
		return;

	INT32 lCount2 = m_csSkillAddedList.GetCount();
	if(lCount2 + lCount > AGPMITEM_MAX_SKILL_PLUS_EFFECT)
	{
		CHAR szTmp[64];
		sprintf(szTmp, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_MAX_SKILL), AGPMITEM_MAX_SKILL_PLUS_EFFECT);
		AfxMessageBox(szTmp);
		return;
	}

	CArray<INT32, INT32> arrListBoxSel;
	arrListBoxSel.SetSize(lCount);
	m_csSkillAllList.GetSelItems(lCount, arrListBoxSel.GetData());

	INT32 lIndex = 0;
	CString szTmp = _T("");
	for(INT32 i = 0; i < lCount && i < AGPMITEM_MAX_SKILL_PLUS_EFFECT; i++)
	{
		m_csSkillAllList.GetText(arrListBoxSel.GetAt(i), szTmp);
		lIndex = szTmp.Find(".");

		if(lIndex < 0 || lIndex >= szTmp.GetLength())
			continue;

		m_csSkillAddedList.AddString((LPCTSTR)szTmp);
	}

	InitSelectedOption();
}

void AlefAdminItem::OnBnClickedBSkillRemove()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 권한체크 2005.09.15.
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	INT32 lCount = m_csSkillAddedList.GetSelCount();
	if(lCount <= 0)
		return;


	CArray<INT32, INT32> arrListBoxSel;
	arrListBoxSel.SetSize(lCount);
	m_csSkillAddedList.GetSelItems(lCount, arrListBoxSel.GetData());

	INT32 lIndex = 0;
	CString szTmp = _T("");
	for(INT32 i = lCount - 1; i > -1; i--)
	{
		m_csSkillAddedList.GetText(arrListBoxSel.GetAt(i), szTmp);
		lIndex = szTmp.Find(".");

		if(lIndex < 0 || lIndex >= szTmp.GetLength())
			continue;

		m_csSkillAddedList.DeleteString(arrListBoxSel.GetAt(i));
	}

	InitSelectedOption();
}

void AlefAdminItem::OnBnClickedSkillSearch()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString szOID = _T("");
	GetDlgItemText(IDC_SKILL_SEARCH_BOX, szOID);

	CString szText = _T("");
	CString szExistOID = _T("");

	int iAllCount = m_csSkillAllList.GetCount();
	int iCommaIndex = 0;

	int iCurSel = 0;
	while(iCurSel < iAllCount)
	{
		m_csSkillAllList.GetText(iCurSel++, szText);

		if(szText.IsEmpty())
			continue;

		iCommaIndex = szText.Find(_T("."));
		szExistOID = szText.Mid(0, iCommaIndex);

		// 찾았3
		if(atoi((LPCTSTR)szOID) == atoi((LPCTSTR)szExistOID))
		{
			m_csSkillAllList.SetCurSel(iCurSel-1);
			m_csSkillAllList.SetSel(iCurSel-1);
			break;
		}
	}
}


void AlefAdminItem::OnCbnSelchangeCbItemList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString szText = _T("");
	int iCurSel = m_csItemListCB.GetCurSel();
	m_csItemListCB.GetLBText(iCurSel, szText);

	if(szText.IsEmpty())
		return;

	int iCommaIndex = szText.Find(_T(","));
	CString szTID = szText.Mid(iCommaIndex + 1);

	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	AgpdItemTemplate* pcsItemTemplate = ppmItem->GetItemTemplate(atoi((LPCTSTR)szTID));
	if(!pcsItemTemplate)
	{
		SetDlgItemText(IDC_E_CREATE_TID, _T(""));
		return;
	}

	SetDlgItemInt(IDC_E_CREATE_TID, pcsItemTemplate->m_lID);

	SetCashItemControls(pcsItemTemplate);
}

void AlefAdminItem::OnBnClickedBItemSearch()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString szTID = _T("");
	GetDlgItemText(IDC_E_CREATE_TID, szTID);

	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	AgpdItemTemplate* pcsItemTemplate = ppmItem->GetItemTemplate(atoi((LPCTSTR)szTID));
	if(!pcsItemTemplate)
	{
		m_csItemListCB.SetCurSel(0);
		return;
	}

	CString szText = _T("");
	CString szExistTID = _T("");

	int iAllCount = m_csItemListCB.GetCount();
	int iCommaIndex = 0;

	int iCurSel = 0;
	while(iCurSel < iAllCount)
	{
		m_csItemListCB.GetLBText(iCurSel++, szText);

		if(szText.IsEmpty())
			continue;

		iCommaIndex = szText.Find(_T(","));
		szExistTID = szText.Mid(iCommaIndex + 1);

		// 찾았3
		if(pcsItemTemplate->m_lID == atoi((LPCTSTR)szExistTID))
		{
			m_csItemListCB.SetCurSel(iCurSel-1);
			SetCashItemControls(pcsItemTemplate);

			//이 아이템에 원래 부여된 option 보여줌.
			ClearAddedOptionData();
			ClearAddedSkillData();
			CString szTmp = _T("");
			for(INT32 i = 0;i < AGPDITEM_OPTION_MAX_NUM; i++)
			{
				if(pcsItemTemplate->m_alOptionTID[i] != 0)
				{
					CHAR szTmp[128];
					memset(szTmp, 0, sizeof(szTmp));

					// 모든 옵션 템플릿 데이터를 보여준다.
					AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );

					for(AgpaItemOptionTemplate::iterator it = ppmItem->csOptionTemplateAdmin.begin(); it != ppmItem->csOptionTemplateAdmin.end(); ++it)
					{
						AgpdItemOptionTemplate* pcsOptionTemplate = it->second;

						if(pcsItemTemplate->m_alOptionTID[i] == pcsOptionTemplate->m_lID)
						{
							sprintf(szTmp, "%d. %s", pcsOptionTemplate->m_lID, pcsOptionTemplate->m_szDescription);
							m_csOptionAddedList.AddString(szTmp);
						}
					}
				}
			}
			break;
		}
	}
}

void AlefAdminItem::OnBnClickedChkUsing()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 이거 할 필요가 없어졌다.
	// 사용중이 아닌 상태로 시간 넣을 수 있어야 한다.
	return;

	//int iCheck = m_csChkUsingBtn.GetCheck();
	//if(iCheck == BST_CHECKED)
	//{
	//	INT32 lTID = GetDlgItemInt(IDC_E_CREATE_TID);
	//	AgpdItemTemplate* pcsItemTemplate = AlefAdminManager::Instance()->GetAdminModule()->m_pcsAgpmItem->GetItemTemplate(lTID);
	//	if(!pcsItemTemplate)
	//		return;

	//	SetCashItemControls(pcsItemTemplate);
	//}
	//else
	//{
	//	SetEnableRemainTime(FALSE);
	//	SetEnableExpireDate(FALSE);
	//}
}

void AlefAdminItem::OnCbnSelchangeCbExpireDateDefault()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iCurSel = m_csExpireDateDefaultCB.GetCurSel();

	if(iCurSel == 0)
	{
		m_csExpireDateYearCB.EnableWindow(TRUE);
		m_csExpireDateMonthCB.EnableWindow(TRUE);
		m_csExpireDateDayCB.EnableWindow(TRUE);
		m_csExpireDateHourCB.EnableWindow(TRUE);
		m_csExpireDateMinCB.EnableWindow(TRUE);
	}
	else
	{
		m_csExpireDateYearCB.EnableWindow(FALSE);
		m_csExpireDateMonthCB.EnableWindow(FALSE);
		m_csExpireDateDayCB.EnableWindow(FALSE);
		m_csExpireDateHourCB.EnableWindow(FALSE);
		m_csExpireDateMinCB.EnableWindow(FALSE);
	}
}

void AlefAdminItem::OnBnClickedBItemUpdate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 권한체크 2005.09.15.
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(!m_stSelectedItem.m_lTID)
		return;

	AgpmItem* ppmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	AgpdItemTemplate* pcsItemTemplate = ppmItem->GetItemTemplate(m_stSelectedItem.m_lTID);
	if(!pcsItemTemplate)
		return;

	CString szMsg = _T("");

	szMsg.LoadString(IDS_ITEM_UPDATE_CONFIRM);
	if(::AfxMessageBox((LPCTSTR)szMsg, MB_YESNO) == IDNO)
		return;

	
	if(ppmItem->GetItemDurabilityMax(pcsItemTemplate) <= 0)
		return;

	// 유료아이템 수정은 레벨5 부터 가능.
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_5)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	INT32 lNewDurability = GetDlgItemInt(IDC_E_DURABILITY);
	INT32 lNewMaxDurability = GetDlgItemInt(IDC_E_MAX_DURABILITY);

	// Max Durability 도 수정이 가능하게 되었다. 2006.03.26.
	if(lNewDurability > lNewMaxDurability)
		return;

	stAgpdAdminItemOperation stItemOperation;
	memset(&stItemOperation, 0, sizeof(stItemOperation));

	// 원본 카피
	stItemOperation.m_stOriginalItem = m_stSelectedItem;

	strcpy(stItemOperation.m_szCharName, m_stSelectedItem.m_szCharName);
	strcpy(stItemOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName());

	stItemOperation.m_cOperation = AGPMADMIN_ITEM_UPDATE;
	stItemOperation.m_ullDBID = m_stSelectedItem.m_ullDBID;
	stItemOperation.m_lID = m_stSelectedItem.m_lID;
	stItemOperation.m_lTID = pcsItemTemplate->m_lID;

	stItemOperation.m_nPart = m_stSelectedItem.m_nPart;

	stItemOperation.m_lDurability = lNewDurability;
	stItemOperation.m_lMaxDurability = lNewMaxDurability;

	AlefAdminManager::Instance()->GetAdminData()->SendItemOperation(&stItemOperation);

	// 리프레쉬 하라는 메시지 창을 띄어준다.
	szMsg.LoadString(IDS_ITEM_REFRESH);
	::AfxMessageBox((LPCTSTR)szMsg);
}

BOOL AlefAdminItem::OnAddItemToInventory( eInventoryType eInventory, stAgpdAdminItem* ppdItem )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	ContainerVector< stItemEntry >* pInventory = ( ContainerVector< stItemEntry >* )_SelectInventory( eInventory );
	if( !pInventory ) return FALSE;

	// 캐쉬인벤의 경우 권한 체크를 추가로 해준다.
	if( eInventory == Inventory_Cash )
	{
		if( AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_4 )
		{
			return FALSE;
		}
	}

	stAgpdAdminItem* ppdCurrentItem = GetItem( eInventory, ppdItem );
	if( ppdCurrentItem )
	{
		memcpy( ppdCurrentItem, ppdItem, sizeof( stAgpdAdminItem ) );
	}
	else
	{
		stItemEntry NewItem;
		memcpy( &NewItem.m_Item, ppdItem, sizeof( stAgpdAdminItem ) );
		pInventory->Add( NewItem );
	}

	return TRUE;
}

BOOL AlefAdminItem::OnShowInventory( eInventoryType eInventory )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	ContainerVector< stItemEntry >* pInventory = ( ContainerVector< stItemEntry >* )_SelectInventory( eInventory );
	if( !pInventory ) return FALSE;

	int nInventoryItemCount = pInventory->GetSize();
	for( int nCount = 0 ; nCount < nInventoryItemCount ; nCount++ )
	{
		stItemEntry* pEntry = pInventory->Get( nCount );
		if( pEntry )
		{
			ShowData( &pEntry->m_Item );
		}
	}

	return TRUE;
}

BOOL AlefAdminItem::OnClearInventory( eInventoryType eInventory )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	ContainerVector< stItemEntry >* pInventory = ( ContainerVector< stItemEntry >* )_SelectInventory( eInventory );
	if( !pInventory ) return FALSE;

	pInventory->Clear();
	return TRUE;
}

BOOL AlefAdminItem::OnUpdateItemPosition( eInventoryType eInventory, stAgpdAdminItem* ppdItem )
{
	switch( eInventory )
	{
	case Inventory_Normal :		return _UpdateItemPosition_Normal( ppdItem );	break;
	case Inventory_Equip :		return _UpdateItemPosition_Equip( ppdItem );	break;
	case Inventory_Cash :		return _UpdateItemPosition_Cash( ppdItem );		break;
	case Inventory_Pet :		return _UpdateItemPosition_Pet( ppdItem );		break;
	case Inventory_Bank :		return _UpdateItemPosition_Bank( ppdItem );		break;
	case Inventory_Sales :		return _UpdateItemPosition_Sales( ppdItem );	break;
	case Inventory_MailBox :	return _UpdateItemPosition_MailBox( ppdItem );	break;
	}

	return FALSE;
}

stAgpdAdminItem* AlefAdminItem::GetItem( eInventoryType eInventory, stAgpdAdminItem* ppdItem )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItem ) return NULL;

	ContainerVector< stItemEntry >* pInventory = ( ContainerVector< stItemEntry >* )_SelectInventory( eInventory );
	if( !pInventory ) return NULL;

	int nItemCount = pInventory->GetSize();
	for( int nCount = 0 ; nCount < nItemCount ; nCount++ )
	{
		stItemEntry* pEntry = pInventory->Get( nCount );
		if( pEntry )
		{
			if( _IsSameItem( &pEntry->m_Item, ppdItem ) )
			{
				return &pEntry->m_Item;
			}
		}
	}

	return NULL;
}

stAgpdAdminItem* AlefAdminItem::GetItem( eInventoryType eInventory, INT16 nLayer, INT16 nRow, INT16 nCol )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	ContainerVector< stItemEntry >* pInventory = ( ContainerVector< stItemEntry >* )_SelectInventory( eInventory );
	if( !pInventory ) return NULL;

	int nItemCount = pInventory->GetSize();
	for( int nCount = 0 ; nCount < nItemCount ; nCount++ )
	{
		stItemEntry* pEntry = pInventory->Get( nCount );
		if( pEntry )
		{
			if( pEntry->m_Item.m_lLayer == nLayer &&
				pEntry->m_Item.m_lRow == nRow &&
				pEntry->m_Item.m_lCol == nCol )
			{
				return &pEntry->m_Item;
			}
		}
	}

	return NULL;
}

void* AlefAdminItem::_SelectInventory( eInventoryType eInventory )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	switch( eInventory )
	{
	case Inventory_Normal :		return &m_vecItemInventory_Normal;		break;
	case Inventory_Equip :		return &m_vecItemInventory_Equip;		break;
	case Inventory_Cash :		return &m_vecItemInventory_Cash;		break;
	case Inventory_Pet :		return &m_vecItemInventory_Pet;			break;
	case Inventory_Bank :		return &m_vecItemInventory_Bank;		break;
	case Inventory_Sales :		return &m_vecItemInventory_Sales;		break;
	case Inventory_MailBox :	return &m_vecItemInventory_MailBox;		break;
	};

	return NULL;
}

BOOL AlefAdminItem::_IsSameItem( stAgpdAdminItem* ppdItem1, stAgpdAdminItem* ppdItem2 )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItem1 || !ppdItem2 ) return FALSE;

	if( ppdItem1->m_lLayer == ppdItem2->m_lLayer &&
		ppdItem1->m_lRow == ppdItem2->m_lRow &&
		ppdItem1->m_lCol == ppdItem2->m_lCol &&
		ppdItem1->m_lPos == ppdItem2->m_lPos )
	{
		// 창고에 있는 아이템은 서버명까지 동일한지 검사한다.
		if( ppdItem1->m_lPos == AGPDITEM_STATUS_BANK )
		{
			if( strcmp( ppdItem1->m_szServerName, ppdItem2->m_szServerName ) != 0 )
			{
				return FALSE;
			}
		}

		// 메일박스에 존재하는 물건이라면 하나더..
		if( ppdItem1->m_lPos == AGPDITEM_STATUS_MAILBOX )
		{
			if( ppdItem1->m_ullDBID != ppdItem2->m_ullDBID )
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL AlefAdminItem::_FindEmptySlot( eInventoryType eInventory, stAgpdAdminItemOperation* ppdItemOperation )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItemOperation ) return FALSE;

	// 찾는 방법 인벤토리의 모든 인덱스대로 뒤져서.. 해당 인덱스에 아이템이 존재하느냐 아니냐로 판단..
	// 아이템이 존재하지 않는 최초의 인덱스가 발견되면 TRUE 인 것이다.
	INT16 nMaxLayer = _GetInventoryMaxLayer( eInventory );
	INT16 nMaxRow = _GetInventoryMaxRow( eInventory );
	INT16 nMaxCol = _GetInventoryMaxCol( eInventory );

	for( int nCountLayer = 0 ; nCountLayer < nMaxLayer ; nCountLayer++ )
	{
		for( int nCountRow = 0 ; nCountRow < nMaxRow ; nCountRow++ )
		{
			for( int nCountCol = 0 ; nCountCol < nMaxCol ; nCountCol++ )
			{
				if( !GetItem( eInventory, nCountLayer, nCountRow, nCountCol ) )
				{
					ppdItemOperation->m_lLayer = nCountLayer;
					ppdItemOperation->m_lRow = nCountRow;
					ppdItemOperation->m_lColumn = nCountCol;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL AlefAdminItem::_UpdateItemPosition_Normal( stAgpdAdminItem* ppdItem )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItem ) return FALSE;

	return TRUE;
}

BOOL AlefAdminItem::_UpdateItemPosition_Equip( stAgpdAdminItem* ppdItem )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItem ) return FALSE;

	return TRUE;
}

BOOL AlefAdminItem::_UpdateItemPosition_Cash( stAgpdAdminItem* ppdItem )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItem ) return FALSE;

	return TRUE;
}

BOOL AlefAdminItem::_UpdateItemPosition_Pet( stAgpdAdminItem* ppdItem )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItem ) return FALSE;

	return TRUE;
}

BOOL AlefAdminItem::_UpdateItemPosition_Bank( stAgpdAdminItem* ppdItem )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItem ) return FALSE;

	return TRUE;
}

BOOL AlefAdminItem::_UpdateItemPosition_Sales( stAgpdAdminItem* ppdItem )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItem ) return FALSE;

	return TRUE;
}

BOOL AlefAdminItem::_UpdateItemPosition_MailBox( stAgpdAdminItem* ppdItem )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	if( !ppdItem ) return FALSE;

	return TRUE;
}

INT16 AlefAdminItem::_GetInventoryMaxLayer( eInventoryType eInventory )
{
	switch( eInventory )
	{
	case Inventory_Normal :		return AGPMITEM_MAX_INVENTORY;			break;
	case Inventory_Cash :		return AGPMITEM_CASH_INVENTORY_LAYER;	break;
	case Inventory_Pet :		return AGPMITEM_SUB_LAYER;				break;
	case Inventory_Bank :		return AGPMITEM_BANK_MAX_LAYER;			break;
	case Inventory_Sales :		return AGPMITEM_SALES_LAYER;			break;
	case Inventory_MailBox :	return 1;								break;
	}

	return 0;
}

INT16 AlefAdminItem::_GetInventoryMaxRow( eInventoryType eInventory )
{
	switch( eInventory )
	{
	case Inventory_Normal :		return AGPMITEM_INVENTORY_ROW;			break;
	case Inventory_Cash :		return AGPMITEM_CASH_INVENTORY_ROW;		break;
	case Inventory_Pet :		return AGPMITEM_SUB_ROW;				break;
	case Inventory_Bank :		return AGPMITEM_BANK_ROW;				break;
	case Inventory_Sales :		return AGPMITEM_SALES_ROW;				break;
	case Inventory_MailBox :	return AGPMMAILBOX_MAX_MAILS_IN_EFFECT;	break;
	}

	return 0;
}

INT16 AlefAdminItem::_GetInventoryMaxCol( eInventoryType eInventory )
{
	switch( eInventory )
	{
	case Inventory_Normal :		return AGPMITEM_INVENTORY_COLUMN;		break;
	case Inventory_Cash :		return AGPMITEM_CASH_INVENTORY_COLUMN;	break;
	case Inventory_Pet :		return AGPMITEM_SUB_COLUMN;				break;
	case Inventory_Bank :		return AGPMITEM_BANK_COLUMN;			break;
	case Inventory_Sales :		return AGPMITEM_SALES_COLUMN;			break;
	case Inventory_MailBox :	return 1;								break;
	}

	return 0;
}


