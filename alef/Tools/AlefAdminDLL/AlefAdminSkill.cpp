// AlefAdminSkill.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminSkill.h"
#include ".\alefadminskill.h"
#include "AlefAdminManager.h"

#include "AgcmUILogin.h"
#include "AgcmUIProduct.h"

template<class T>
struct ShowDataFunc
{
	void operator() (T t) const
	{
		AlefAdminManager::Instance()->m_pMainDlg->m_pSkillView->ShowData(t);
	}
};

const int ALEFADMINSKILL_INDEX_OWN_PRODUCT			= 4;
const int ALEFADMINSKILL_INDEX_OWN_PRODUCT_COMPOSE	= ALEFADMINSKILL_INDEX_OWN_PRODUCT + 1;
const int ALEFADMINSKILL_INDEX_ALL_PRODUCT			= 14;
const int ALEFADMINSKILL_INDEX_ALL_PRODUCT_COMPOSE	= ALEFADMINSKILL_INDEX_ALL_PRODUCT + 1;


// AlefAdminSkill

IMPLEMENT_DYNCREATE(AlefAdminSkill, CFormView)

AlefAdminSkill::AlefAdminSkill()
	: CFormView(AlefAdminSkill::IDD)
{
	m_lSkillPoint = 0;
	m_pcsAgpmSkill = NULL;
	m_pcsAgpmEventSkillMaster = NULL;
}

AlefAdminSkill::~AlefAdminSkill()
{
	ClearSkillList();
}

void AlefAdminSkill::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_SKILL_TYPE, m_csSkillTypeCB);
	DDX_Control(pDX, IDC_LV_OWN_SKILL, m_csOwnSkillLV);
	DDX_Control(pDX, IDC_CB_SKILL_CATEGORY, m_csSkillCategory);
	DDX_Control(pDX, IDC_LV_ALL_SKILL, m_csSkillLV);
	DDX_Control(pDX, IDC_B_INIT_SKILL_ALL, m_csInitAllBtn);
	DDX_Control(pDX, IDC_B_INIT_SKILL_SPECIFIC, m_csInitSpecificBtn);
	DDX_Control(pDX, IDC_B_REFRESH, m_csRefreshBtn);
	DDX_Control(pDX, IDC_B_LEARN, m_csLearnBtn);
	DDX_Control(pDX, IDC_CK_OWN_SKILL_ALL, m_csOwnSkillAllCheckBox);
	DDX_Control(pDX, IDC_CK_ALL_SKILL_ALL, m_csAllSkillAllCheckBox);
	DDX_Control(pDX, IDC_B_LEVEL_DOWN, m_csLevelDownBtn);
	DDX_Control(pDX, IDC_B_LEVEL_UP, m_csLevelUpBtn);
	DDX_Control(pDX, IDC_B_PRODUCT_EXP_CHANGE, m_csProductExpChangeBtn);
}

BEGIN_MESSAGE_MAP(AlefAdminSkill, CFormView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_B_REFRESH, OnBnClickedBRefresh)
	ON_CBN_SELCHANGE(IDC_CB_SKILL_TYPE, OnCbnSelchangeCbSkillType)
	ON_CBN_SELCHANGE(IDC_CB_SKILL_CATEGORY, OnCbnSelchangeCbSkillCategory)
	ON_BN_CLICKED(IDC_B_LEARN, OnBnClickedBLearn)
	ON_NOTIFY(NM_CLICK, IDC_LV_ALL_SKILL, OnNMClickLvAllSkill)
	ON_NOTIFY(NM_CLICK, IDC_LV_OWN_SKILL, OnNMClickLvOwnSkill)
	ON_BN_CLICKED(IDC_CK_OWN_SKILL_ALL, OnBnClickedCkOwnSkillAll)
	ON_BN_CLICKED(IDC_CK_ALL_SKILL_ALL, OnBnClickedCkAllSkillAll)
	ON_BN_CLICKED(IDC_B_INIT_SKILL_ALL, OnBnClickedBInitSkillAll)
	ON_BN_CLICKED(IDC_B_INIT_SKILL_SPECIFIC, OnBnClickedBInitSkillSpecific)
	ON_BN_CLICKED(IDC_B_LEVEL_UP, OnBnClickedBLevelUp)
	ON_BN_CLICKED(IDC_B_LEVEL_DOWN, OnBnClickedBLevelDown)
	ON_BN_CLICKED(IDC_B_PRODUCT_EXP_CHANGE, OnBnClickedBProductExpChange)
END_MESSAGE_MAP()


// AlefAdminSkill 진단입니다.

#ifdef _DEBUG
void AlefAdminSkill::AssertValid() const
{
	CFormView::AssertValid();
}

void AlefAdminSkill::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// AlefAdminSkill 메시지 처리기입니다.

void AlefAdminSkill::OnInitialUpdate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFormView::OnInitialUpdate();

	m_pcsAgpmSkill = ( AgpmSkill* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmSkill" );
	m_pcsAgpmEventSkillMaster = ( AgpmEventSkillMaster* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmEventSkillMaster" );
	m_pcsAgpmProduct = ( AgpmProduct* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmProduct" );

	ASSERT(m_pcsAgpmSkill);
	ASSERT(m_pcsAgpmEventSkillMaster);
	ASSERT(m_pcsAgpmProduct);

	m_pcsAgpmProduct->SetCallbackComposeInfo(CBComposeInfo, this);

	SetSkillPoint(0);

	// Buttons
	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;
	m_csInitAllBtn.SetXButtonStyle(dwStyle);
	m_csInitSpecificBtn.SetXButtonStyle(dwStyle);
	m_csRefreshBtn.SetXButtonStyle(dwStyle);
	m_csLearnBtn.SetXButtonStyle(dwStyle);
	m_csLevelUpBtn.SetXButtonStyle(dwStyle);
	m_csLevelDownBtn.SetXButtonStyle(dwStyle);
	m_csProductExpChangeBtn.SetXButtonStyle(dwStyle);

	InitComboBox();
	InitListView();

	m_csSkillTypeCB.SetCurSel(0);
	m_csSkillCategory.SetCurSel(0);

	ShowData();
	ShowAllSkill();
}

BOOL AlefAdminSkill::InitComboBox()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSkillTypeCB.ResetContent();
	m_csSkillCategory.ResetContent();

	m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_0));
	m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_1));
	m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_2));
	m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_3));
	m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_4));
	m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_PRODUCT));
	m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_PRODUCT_COMPOSE));

	// 휴먼(나이트, 아쳐, 메이지), 오크(버서커, 헌터, 소서러), 문엘프(레인저, 엘리멘탈리스트, 스워시버클러), 드래곤시온(시온, 슬레이어, 오비터, 서머너)
	CString szText = _T("");
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_HUMAN),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_KNIGHT));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_HUMAN),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_ARCHER));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_HUMAN),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_MAGE));
	m_csSkillCategory.AddString(szText);

	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_ORC),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_BERSERKER));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_ORC),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_HUNTER));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_ORC),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SORCERER));
	m_csSkillCategory.AddString(szText);

	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_MOONELF),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SWASHBUCKLER));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_MOONELF),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_RANGER));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_MOONELF),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_ELEMENTALIST));
	m_csSkillCategory.AddString(szText);

	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_DRAGONSCION),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SLAYER));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_DRAGONSCION),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_OBITER));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_DRAGONSCION),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SUMMERNER));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_DRAGONSCION),
							AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SCION));
	m_csSkillCategory.AddString(szText);
	szText.Format("%s %s", AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_RACE_DRAGONSCION),
							AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_3));
	m_csSkillCategory.AddString(szText);

	m_csSkillCategory.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_PRODUCT));
	m_csSkillCategory.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_PRODUCT_COMPOSE));

	return TRUE;
}

BOOL AlefAdminSkill::InitListView()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csOwnSkillLV.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_NAME), LVCFMT_LEFT, 135);
	m_csOwnSkillLV.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LEVEL), LVCFMT_LEFT, 45);
	m_csOwnSkillLV.InsertColumn(2, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DESC), LVCFMT_LEFT, 300);

	HWND hWndHeader = m_csOwnSkillLV.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeaderOwnSkill.SubclassWindow(hWndHeader);

	m_csOwnSkillLV.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_CHECKBOXES);

	m_csSkillLV.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_NAME), LVCFMT_LEFT, 135);
	m_csSkillLV.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DESC), LVCFMT_LEFT, 300);

	HWND hWndHeader2 = m_csSkillLV.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeaderAllSkill.SubclassWindow(hWndHeader2);

	m_csSkillLV.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_CHECKBOXES);

	return TRUE;
}

// 접속중인 놈의 데이터를 받은 경우
BOOL AlefAdminSkill::OnReceive(AgpdSkill* pcsSkill)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsSkill || !pcsSkill->m_pcsBase)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsSkill->m_pcsBase;
	if(!pcsCharacter)
		return FALSE;

	// 검색한 놈이 아니면 나간다.
	if(strcmp(pcsCharacter->m_szID, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) != 0)
		return TRUE;

	m_csSkillTypeCB.ResetContent();
	AgpmFactors* ppmFactors = ( AgpmFactors* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmFactors" );
	AuRaceType eRaceType = ( AuRaceType )ppmFactors->GetRace( &pcsCharacter->m_csFactor );

	if( eRaceType != AURACE_TYPE_DRAGONSCION )
	{
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_0));
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_1));
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_2));
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_3));
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_4));
	}
	else
	{
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SCION));
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SLAYER));
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_OBITER));
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SUMMERNER));
		m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_MASTERY_3));
	}

	m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_PRODUCT));
	m_csSkillTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_PRODUCT_COMPOSE));

	CString szTmp = _T("");
	szTmp.Format("%s : %s", (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHARNAME),
							pcsCharacter->m_szID);
	SetDlgItemText(IDC_S_SKILL_CHARNAME, (LPCTSTR)szTmp);

	stAgpdAdminSkill stSkill;
	memset(&stSkill, 0, sizeof(stSkill));

	strcpy(stSkill.m_szCharName, pcsCharacter->m_szID);
	stSkill.m_lTID = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_lID;

	AgpmSkill* ppmSkill = ( AgpmSkill* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmSkill" );

	if(IsProductSkill(stSkill.m_lTID))
		stSkill.m_lLevel = ppmSkill->GetSkillExp(pcsSkill);
	else
		stSkill.m_lLevel = ppmSkill->GetSkillLevel(pcsSkill);

	stSkill.m_pcsTemplate = pcsSkill->m_pcsTemplate;

	AddSkillList(&stSkill);
	ShowData(&stSkill);

	return TRUE;
}

// 스킬 트리 스트링을 받았다.
BOOL AlefAdminSkill::OnReceiveSkillString(stAgpdAdminSkillString* pstSkillString)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 검색한 놈이 아니면 나간다.
	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pstSkillString->m_szCharName) == FALSE)
		return FALSE;

	CString szTmp = _T("");
	szTmp.Format("%s : %s", (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHARNAME),
							pstSkillString->m_szCharName);
	SetDlgItemText(IDC_S_SKILL_CHARNAME, (LPCTSTR)szTmp);

	INT32 lDecodedLength = 0;
	INT32 lBufferLength = _tcslen(pstSkillString->m_szTreeNode);

	while(lDecodedLength < lBufferLength)
	{
		INT32 lSkillTID = AP_INVALID_SKILLID;
		INT32 lSkillLevel = 1;

		int j;
		for(j = lDecodedLength; j < lBufferLength; ++j)
		{
			if(pstSkillString->m_szTreeNode[j] == ':')
				break;
		}

		if(j == lBufferLength)
			return FALSE;

		sscanf(pstSkillString->m_szTreeNode + lDecodedLength, "%d,%d:", &lSkillTID, &lSkillLevel);
		lDecodedLength	= j + 1;

		stAgpdAdminSkill stSkill;
		memset(&stSkill, 0, sizeof(stSkill));

		strcpy(stSkill.m_szCharName, pstSkillString->m_szCharName);
		stSkill.m_lTID = lSkillTID;
		stSkill.m_lLevel = lSkillLevel;
		stSkill.m_pcsTemplate = (PVOID)GetSkillTemplate(lSkillTID);

		AddSkillList(&stSkill);
	}

	// Product Compose 도 저장하자.
	string szProduct = pstSkillString->m_szProduct;
	int iCount = std::count(szProduct.begin(), szProduct.end(), ':');
	int iIndex = 0, iIndex2 = 0;
	for(int i = 0; i < iCount; ++i)
	{
		iIndex2 = szProduct.find(':', iIndex);
		string szTmp = szProduct.substr(iIndex, iIndex2);
		if(szTmp.empty())
			break;

		m_vcProduct.push_back(atoi(szTmp.c_str()));
		iIndex = iIndex2 + 1;
	}

	// 화면을 다시 뿌린다.
	OnCbnSelchangeCbSkillType();
}

BOOL AlefAdminSkill::AddSkillList(stAgpdAdminSkill* pstSkill)
{
	if(!pstSkill)
		return FALSE;

	stAgpdAdminSkill* pstSkillData = GetSkill(pstSkill->m_lTID);

	if(pstSkillData)
		memcpy(pstSkillData, pstSkill, sizeof(stAgpdAdminSkill));
	else
	{
		pstSkillData = new stAgpdAdminSkill;
		memcpy(pstSkillData, pstSkill, sizeof(stAgpdAdminSkill));
		m_listSkill.push_back(pstSkillData);
	}

	return TRUE;
}

BOOL AlefAdminSkill::ClearSkillList()
{
	if(m_listSkill.empty())
		return TRUE;

	std::for_each(m_listSkill.begin(), m_listSkill.end(), DeletePtr());
	m_listSkill.clear();

	m_vcProduct.clear();
	return TRUE;
}

stAgpdAdminSkill* AlefAdminSkill::GetSkill(INT32 lTID)
{
	if(!lTID)
		return NULL;

	if(m_listSkill.empty())
		return NULL;

	SkillIter iterData = m_listSkill.begin();
	while(iterData != m_listSkill.end())
	{
		if(*iterData && (*iterData)->m_lTID == lTID)
			return *iterData;

		iterData++;
	}

	return NULL;
}

stAgpdAdminSkill* AlefAdminSkill::GetSkill(CHAR* szSkillName)
{
	if(!szSkillName)
		return NULL;

	if(m_listSkill.empty())
		return NULL;

	SkillIter iterData = m_listSkill.begin();
	while(iterData != m_listSkill.end())
	{
		if(*iterData && (*iterData)->m_pcsTemplate)
		{
			AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)(*iterData)->m_pcsTemplate;
			if(strcmp(pcsSkillTemplate->m_szName, szSkillName) == 0)
				return *iterData;
		}

		iterData++;
	}

	return NULL;
}

AgpdSkillTemplate* AlefAdminSkill::GetSkillTemplate(INT32 lTID)
{
	if(!AlefAdminManager::Instance()->IsInitialized())
		return NULL;

	return m_pcsAgpmSkill->GetSkillTemplate(lTID);
}

AgpdSkillTemplate* AlefAdminSkill::GetSkillTemplate(CHAR* szSkillName)
{
	if(!szSkillName)
		return NULL;

	if(!AlefAdminManager::Instance()->IsInitialized())
		return NULL;

	return m_pcsAgpmSkill->GetSkillTemplate(szSkillName);
}

BOOL AlefAdminSkill::ShowData(stAgpdAdminSkill* pstSkill)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstSkill)
		return FALSE;

	int iCurSel = m_csSkillTypeCB.GetCurSel();
	if(IsOwnIndexCompose())
		return ShowProductCompose();

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pstSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )m_pcsAgpmSkill->GetModule( "AgpmCharacter" );
	AgpmFactors* ppmFactor = ( AgpmFactors* )m_pcsAgpmSkill->GetModule( "AgpmFactors" );
	if( !ppmCharacter || !ppmFactor ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pstSkill->m_szCharName );
	if( !ppdCharacter ) return FALSE;

	AuRaceType eRaceType = ( AuRaceType )ppmFactor->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = ( AuCharClassType )ppmFactor->GetClass( &ppdCharacter->m_csFactor );
	if( eRaceType != AURACE_TYPE_DRAGONSCION )
	{
		// 전투, 강화, 전용, 패시브스킬의 경우..
		if( iCurSel < AGPMEVENT_SKILL_MAX_MASTERY - 1 )
		{
			// 드래곤시온이 아닌 종족은 스킬마스터리 인덱스가 일치하여야 한다.
			INT32 lMastery = m_pcsAgpmEventSkillMaster->GetMasteryIndex(pcsSkillTemplate);
			if( lMastery != iCurSel || IsHighLevelSkill( eRaceType, eClassType, pcsSkillTemplate->m_lID ) ) return TRUE;
		}
		// 연계스킬의 경우..
		else if( iCurSel < AGPMEVENT_SKILL_MAX_MASTERY )
		{
			if( !IsHighLevelSkill( eRaceType, eClassType, pcsSkillTemplate->m_lID ) )
			{
				return TRUE;
			}
		}
	}
	else
	{
		// 드래곤시온의 경우 해당 스킬이 현재 선택한 클래스에 맞는 스킬인가를 검사한다.
		if( !IsCurrentClassSkillForDragonScion( iCurSel, pcsSkillTemplate->m_lID ) )
		{
			return TRUE;
		}
	}

	BOOL bUpdate = FALSE;
	CHAR szTmp[255];
	memset(szTmp, 0, sizeof(szTmp));

	// List 를 돌면서 있으면 업뎃, 없으면 추가한다.
	int iTotalRows = m_csOwnSkillLV.GetItemCount();
	for(int i = 0; i < iTotalRows; i++)
	{
		m_csOwnSkillLV.GetItemText(i, 0, szTmp, 254);
		if(strcmp(szTmp, pcsSkillTemplate->m_szName) == 0)
		{
			// Sub 내용을 업뎃해준다.
			if(IsProductSkill(pcsSkillTemplate))
				sprintf(szTmp, "%d", GetLevelOfExp(*pstSkill));
			else
				sprintf(szTmp, "%d", pstSkill->m_lLevel);

			m_csOwnSkillLV.SetItemText(i, 1, szTmp);

			bUpdate = TRUE;
			break;
		}
	}

	if(!bUpdate)
	{
		// 새로 추가해준다.
		m_csOwnSkillLV.InsertItem(iTotalRows, pcsSkillTemplate->m_szName);

		if(IsProductSkill(pcsSkillTemplate))
			sprintf(szTmp, "%d", GetLevelOfExp(*pstSkill));
		else
			sprintf(szTmp, "%d", pstSkill->m_lLevel);

		m_csOwnSkillLV.SetItemText(iTotalRows, 1, szTmp);

		AgpdSkillTooltipTemplate* pcsTooltip = m_pcsAgpmSkill->GetTooltipTemplate(pcsSkillTemplate->m_szName);
		if(pcsTooltip)
		{
			CString szText = _T("");
			szText = pcsTooltip->m_szSkillDescription;
			szText.Replace("\\n", " ");
			szText.Remove('\"');

			m_csOwnSkillLV.SetItemText(iTotalRows, 2, szText);
		}
	}

	return TRUE;
}

BOOL AlefAdminSkill::ShowData()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(IsOwnIndexCompose())
		return ShowProductCompose();

	// list 를 돌면서 ShowData 해준다.
	std::for_each(m_listSkill.begin(), m_listSkill.end(), ShowDataFunc<stAgpdAdminSkill*>());
	return TRUE;
}

BOOL AlefAdminSkill::ShowProductCompose()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(IsOwnIndexCompose() == FALSE)
		return FALSE;

	BOOL bUpdate = FALSE;
	CHAR szTmp[255];
	memset(szTmp, 0, sizeof(szTmp));

	ProductVector vcExist;

	// List 를 돌면서 있으면 업뎃, 없으면 추가한다.
	int iTotalRows = m_csOwnSkillLV.GetItemCount();
	for(int i = 0; i < iTotalRows; i++)
	{
		m_csOwnSkillLV.GetItemText(i, 0, szTmp, 254);
		AgpdComposeTemplate* pComposeTemplate = m_pcsAgpmProduct->GetComposeTemplate(atoi(szTmp));
		if(!pComposeTemplate)
			continue;

		vcExist.push_back(pComposeTemplate->m_lComposeID);
	}

	// 캐릭터가 가지고 있는 놈과 비교해서 새로운 놈들만 뽑아낸다.
	ProductVector vcNew;
	std::set_difference(m_vcProduct.begin(), m_vcProduct.end(), vcExist.begin(), vcExist.end(), back_inserter(vcNew));

	ProductIter iterNew = vcNew.begin();
	while(iterNew != vcNew.end())
	{
		AgpdComposeTemplate* pComposeTemplate = m_pcsAgpmProduct->GetComposeTemplate(*iterNew);
		if(!pComposeTemplate)
		{
			++iterNew;
			continue;
		}

		// 새로 추가해준다.
		m_csOwnSkillLV.InsertItem(iTotalRows, pComposeTemplate->m_szCompose);

		// Desc 항목에 TID 를 넣는다. 나중에 꺼내올 때를 위해서.
		sprintf(szTmp, "%d", *iterNew);
		m_csOwnSkillLV.SetItemText(iTotalRows, 2, szTmp);

		++iTotalRows;
		++iterNew;
	}

}

BOOL AlefAdminSkill::ClearSkillListCtrl()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csOwnSkillLV.DeleteAllItems();
	SetDlgItemText(IDC_E_PRODUCT_LEVEL, _T(""));
	return TRUE;
}

BOOL AlefAdminSkill::ClearSkillAllListCtrl()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSkillLV.DeleteAllItems();
	return TRUE;
}

BOOL AlefAdminSkill::ShowAllSkill()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int lIndex = m_csSkillCategory.GetCurSel();
	if(lIndex == CB_ERR)
		return FALSE;

	AuRaceType eRaceType = AURACE_TYPE_HUMAN;
	AuCharClassType eCharClass = AUCHARCLASS_TYPE_KNIGHT;
	switch(lIndex)
	{
		case 0:		eRaceType = AURACE_TYPE_HUMAN;			eCharClass = AUCHARCLASS_TYPE_KNIGHT;		break;
		case 1:		eRaceType = AURACE_TYPE_HUMAN;			eCharClass = AUCHARCLASS_TYPE_RANGER;		break;
		case 2:		eRaceType = AURACE_TYPE_HUMAN;			eCharClass = AUCHARCLASS_TYPE_MAGE;			break;

		case 3:		eRaceType = AURACE_TYPE_ORC;			eCharClass = AUCHARCLASS_TYPE_KNIGHT;		break;
		case 4:		eRaceType = AURACE_TYPE_ORC;			eCharClass = AUCHARCLASS_TYPE_RANGER;		break;
		case 5:		eRaceType = AURACE_TYPE_ORC;			eCharClass = AUCHARCLASS_TYPE_MAGE;			break;

		case 6:		eRaceType = AURACE_TYPE_MOONELF;		eCharClass = AUCHARCLASS_TYPE_KNIGHT;		break;
		case 7:		eRaceType = AURACE_TYPE_MOONELF;		eCharClass = AUCHARCLASS_TYPE_RANGER;		break;
		case 8:		eRaceType = AURACE_TYPE_MOONELF;		eCharClass = AUCHARCLASS_TYPE_MAGE;			break;

		case 9:		eRaceType = AURACE_TYPE_DRAGONSCION;	eCharClass = AUCHARCLASS_TYPE_KNIGHT;		break;
		case 10:	eRaceType = AURACE_TYPE_DRAGONSCION;	eCharClass = AUCHARCLASS_TYPE_RANGER;		break;
		case 11:	eRaceType = AURACE_TYPE_DRAGONSCION;	eCharClass = AUCHARCLASS_TYPE_MAGE;			break;
		case 12:	eRaceType = AURACE_TYPE_DRAGONSCION;	eCharClass = AUCHARCLASS_TYPE_SCION;		break;
		case 13:	eRaceType = AURACE_TYPE_DRAGONSCION;	eCharClass = AUCHARCLASS_TYPE_MAX;			break;		// 드래곤시온 패시브스킬을 받기위해서.. 임시로..;;

		case 14:	break;
		case 15:	break;

		default:	return FALSE;
	}

	INT32 lCount = 0;
	CString szText = _T("");

	if(lIndex < ALEFADMINSKILL_INDEX_ALL_PRODUCT)		// 생산이 아님
	{
		// 드래곤시온 이외의 종족들은 이렇게..
		if( eRaceType != AURACE_TYPE_DRAGONSCION )
		{
			// 생산은 여기서 뿌리지 않는다.
			for(INT32 lMasteryIndex = 0; lMasteryIndex < AGPMEVENT_SKILL_MAX_MASTERY - 1; lMasteryIndex++)
			{
				INT32* pcsSkillMasteryList = m_pcsAgpmEventSkillMaster->GetMastery(eRaceType, eCharClass, lMasteryIndex);
				if(!pcsSkillMasteryList)
					continue;

				// 여기서 전투, 강화, 전용, 패시브 스킬을 받아온다.
				for(int i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; i++)
				{
					if(pcsSkillMasteryList[i] == 0)
						break;

					AgpdSkillTemplate* pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsSkillMasteryList[i]);
					if(!pcsSkillTemplate )
						continue;

					m_csSkillLV.InsertItem(lCount, pcsSkillTemplate->m_szName);

					AgpdSkillTooltipTemplate* pcsTooltip = m_pcsAgpmSkill->GetTooltipTemplate(pcsSkillTemplate->m_szName);
					if(pcsTooltip)
					{
						szText = pcsTooltip->m_szSkillDescription;
						szText.Replace("\\n", " ");
						szText.Remove('\"');
						m_csSkillLV.SetItemText(lCount, 1, szText);
					}

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
						if( pcsSkillTemplate )
						{
							m_csSkillLV.InsertItem( lCount, pcsSkillTemplate->m_szName );

							AgpdSkillTooltipTemplate* pcsTooltip = m_pcsAgpmSkill->GetTooltipTemplate( pcsSkillTemplate->m_szName );
							if(pcsTooltip)
							{
								szText = pcsTooltip->m_szSkillDescription;
								szText.Replace("\\n", " ");
								szText.Remove('\"');
								m_csSkillLV.SetItemText( lCount, 1, szText );
							}

							lCount++;
						}

						iter++;
					}
				}
			}
		}
		else
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
							m_csSkillLV.InsertItem( nCount, ppdSkillTemplate->m_szName );

							AgpdSkillTooltipTemplate* ppdSkillTemplateTooltip = m_pcsAgpmSkill->GetTooltipTemplate( ppdSkillTemplate->m_szName );
							if( ppdSkillTemplateTooltip )
							{
								szText = ppdSkillTemplateTooltip->m_szSkillDescription;
								szText.Replace("\\n", " ");
								szText.Remove('\"');
								m_csSkillLV.SetItemText( nCount, 1, szText );
							}

							nCount++;
						}

						iter++;
					}
				}
			}
		}
	}
	else if(lIndex == ALEFADMINSKILL_INDEX_ALL_PRODUCT)
	{
		// 생산
		INT32 lSequence = 0;
		for(AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplateSequence(&lSequence);
			pcsSkillTemplate;
			pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplateSequence(&lSequence))
		{
			if(!(pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT))
				continue;

			m_csSkillLV.InsertItem(lCount, pcsSkillTemplate->m_szName);

			AgpdSkillTooltipTemplate* pcsTooltip = m_pcsAgpmSkill->GetTooltipTemplate(pcsSkillTemplate->m_szName);
			if(pcsTooltip)
			{
				szText = pcsTooltip->m_szSkillDescription;
				szText.Replace("\\n", " ");
				szText.Remove('\"');
				m_csSkillLV.SetItemText(lCount, 1, szText);
			}

			lCount++;
		}
	}
	else if(lIndex == ALEFADMINSKILL_INDEX_ALL_PRODUCT_COMPOSE)
	{
		// 생산물
		for(INT32 lCategory = AGPMPRODUCT_CATEGORY_HUNT; lCategory < AGPMPRODUCT_CATEGORY_MAX; ++lCategory)
		{
			INT32 lTIDList[AGCMUIPRODUCT_MAX_LIST];
			INT32 lNum = m_pcsAgpmProduct->GetComposeList((eAGPMPRODUCT_CATEGORY)lCategory, lTIDList, AGCMUIPRODUCT_MAX_LIST);
			for(int i = 0; i < lNum; ++i)
			{
				AgpdComposeTemplate* pcsComposeTemplate = m_pcsAgpmProduct->GetComposeTemplate(lTIDList[i]);
				if(!pcsComposeTemplate)
					continue;

				m_csSkillLV.InsertItem(lCount, pcsComposeTemplate->m_szCompose);

				szText.Format("%d", pcsComposeTemplate->m_lComposeID);
				m_csSkillLV.SetItemText(lCount, 1, (LPCTSTR)szText);

				++lCount;
			}
		}
	}

	return TRUE;
}

BOOL AlefAdminSkill::SetAllSkillByTID(INT32 lTID)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	switch(lTID)
	{
		// Human - Knight, Ranger, Mage
		case 96:	m_csSkillCategory.SetCurSel(0);		OnCbnSelchangeCbSkillCategory();	break;
		case 1:		m_csSkillCategory.SetCurSel(1);		OnCbnSelchangeCbSkillCategory();	break;
		case 6:		m_csSkillCategory.SetCurSel(2);		OnCbnSelchangeCbSkillCategory();	break;

		// Orc - Knight, Ranger, Mage
		case 4:		m_csSkillCategory.SetCurSel(3);		OnCbnSelchangeCbSkillCategory();	break;
		case 8:		m_csSkillCategory.SetCurSel(4);		OnCbnSelchangeCbSkillCategory();	break;
		case 3:		m_csSkillCategory.SetCurSel(5);		OnCbnSelchangeCbSkillCategory();	break;

		// MoonElf - Knight, Ranger, Mage
		case 377:	m_csSkillCategory.SetCurSel(6);		OnCbnSelchangeCbSkillCategory();	break;
		case 460:	m_csSkillCategory.SetCurSel(7);		OnCbnSelchangeCbSkillCategory();	break;
		case 9:		m_csSkillCategory.SetCurSel(8);		OnCbnSelchangeCbSkillCategory();	break;

		// DragonScion - Knight, Ranger, Mage, Scion
		case 1722:	m_csSkillCategory.SetCurSel(9);		OnCbnSelchangeCbSkillCategory();	break;
		case 1723:	m_csSkillCategory.SetCurSel(10);	OnCbnSelchangeCbSkillCategory();	break;
		case 1732:	m_csSkillCategory.SetCurSel(11);	OnCbnSelchangeCbSkillCategory();	break;
		case 1724:	m_csSkillCategory.SetCurSel(12);	OnCbnSelchangeCbSkillCategory();	break;

		default:	m_csSkillCategory.SetCurSel(0);		OnCbnSelchangeCbSkillCategory();	break;
	}

	return TRUE;
}

BOOL AlefAdminSkill::SetSkillPoint(INT32 lSkillPoint)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_lSkillPoint = lSkillPoint;
	if(m_lSkillPoint == 0)
		SetDlgItemText(IDC_E_REMAIN_POINT, "");
	else
		SetDlgItemInt(IDC_E_REMAIN_POINT, m_lSkillPoint);
	return TRUE;
}

BOOL AlefAdminSkill::CheckLearnableSkill(AgpdSkillTemplate* pcsSkillTemplate)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsSkillTemplate)
		return FALSE;

	// 현재 관리하는 놈이 없으면 나간다.
	stAgpdAdminCharData* pstCharData = &AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData;
	if(!pstCharData || strlen(pstCharData->m_stBasic.m_szCharName) == 0)
		return FALSE;

	BOOL bAleadyLearn = FALSE;
	if(GetSkill(pcsSkillTemplate->m_lID))
		bAleadyLearn = TRUE;

	// 생산스킬이고 이미 안배웠으면 return TRUE;
	if(pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT && !bAleadyLearn)
		return TRUE;

	INT32 lBaseRace	= pstCharData->m_stStat.m_lRace;
	INT32 lBaseClass = pstCharData->m_stStat.m_lClass;

	switch((AuRaceType)lBaseRace)
	{
		case AURACE_TYPE_HUMAN:
		{
			switch((AuCharClassType)lBaseClass)
			{
				case AUCHARCLASS_TYPE_KNIGHT:
					if(pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_KNIGHT][1] == 0)
						return FALSE;
					break;

				case AUCHARCLASS_TYPE_RANGER:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_ARCHER][1] == 0)
						return FALSE;
					break;

				case AUCHARCLASS_TYPE_MAGE:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_WIZARD][1] == 0)
						return FALSE;
					break;

				default:
					return FALSE;
					break;
			}

			break;
		}

		case AURACE_TYPE_ORC:
		{
			switch((AuCharClassType)lBaseClass)
			{
				case AUCHARCLASS_TYPE_KNIGHT:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_BERSERKER][1] == 0)
						return FALSE;
					break;

				case AUCHARCLASS_TYPE_RANGER:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_HUNTER][1] == 0)
						return FALSE;
					break;

				case AUCHARCLASS_TYPE_MAGE:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_SORCERER][1] == 0)
						return FALSE;
					break;

				default:
					return FALSE;
					break;
			}

			break;
		}

		case AURACE_TYPE_MOONELF:
		{
			switch((AuCharClassType)lBaseClass)
			{
				case AUCHARCLASS_TYPE_KNIGHT:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_SWASHBUCKLER][1] == 0)
						return FALSE;
					break;

				case AUCHARCLASS_TYPE_RANGER:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_RANGER][1] == 0)
						return FALSE;
					break;

				case AUCHARCLASS_TYPE_MAGE:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_ELEMENTALER][1] == 0)
						return FALSE;
					break;

				default:
					return FALSE;
					break;
			}

			break;
		}

		case AURACE_TYPE_DRAGONSCION:
		{
			switch((AuCharClassType)lBaseClass)
			{
				case AUCHARCLASS_TYPE_KNIGHT:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_SLAYER][1] == 0)
						return FALSE;
					break;

				case AUCHARCLASS_TYPE_RANGER:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_OBITER][1] == 0)
						return FALSE;
					break;

				case AUCHARCLASS_TYPE_MAGE:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_SUMMONER][1] == 0)
						return FALSE;
					break;

				case AUCHARCLASS_TYPE_SCION:
					if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_SCION][1] == 0)
						return FALSE;
					break;

				default:
					return FALSE;
					break;
			}

			break;
		}

		default:
			return FALSE;
			break;
	}

	return TRUE;
}

INT32 AlefAdminSkill::GetSelectedOwnSkill(vector<INT32>& vcSelectedSkill)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	INT32 lCount = m_csOwnSkillLV.GetItemCount();
	if(lCount == 0)
		return 0;

	CString szName = _T("");
	for(int i = 0; i < lCount; i++)
	{
		if(m_csOwnSkillLV.GetCheck(i))
		{
			szName = m_csOwnSkillLV.GetItemText(i, 0);

			if(IsOwnIndexCompose() == FALSE)
			{
				AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate((CHAR*)(LPCTSTR)szName);
				if(pcsSkillTemplate)
					vcSelectedSkill.push_back(pcsSkillTemplate->m_lID);
			}
			else
			{
				szName = m_csOwnSkillLV.GetItemText(i, 2);
				AgpdComposeTemplate* pcsComposeTemplate = m_pcsAgpmProduct->GetComposeTemplate(atoi((LPCTSTR)szName));
				if(pcsComposeTemplate)
					vcSelectedSkill.push_back(pcsComposeTemplate->m_lComposeID);
			}
		}
	}

	return vcSelectedSkill.size();
}

INT32 AlefAdminSkill::GetSelectedAllSkill(vector<INT32>& vcSelectedSkill)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	INT32 lCount = m_csSkillLV.GetItemCount();
	if(lCount == 0)
		return 0;

	CString szName = _T("");
	for(int i = 0; i < lCount; i++)
	{
		if(m_csSkillLV.GetCheck(i))
		{
			szName = m_csSkillLV.GetItemText(i, 0);

			if(IsAllIndexCompose() == FALSE)
			{
				AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate((CHAR*)(LPCTSTR)szName);
				if(pcsSkillTemplate)
					vcSelectedSkill.push_back(pcsSkillTemplate->m_lID);
			}
			else
			{
				szName = m_csSkillLV.GetItemText(i, 1);
				AgpdComposeTemplate* pcsComposeTemplate = m_pcsAgpmProduct->GetComposeTemplate(atoi((LPCTSTR)szName));
				if(pcsComposeTemplate)
					vcSelectedSkill.push_back(pcsComposeTemplate->m_lComposeID);
			}
		}
	}

	return vcSelectedSkill.size();
}

INT32 AlefAdminSkill::MakeLearnString(vector<INT32>& vcSelectedSkill, stAgpdAdminSkillString& stSkillString)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	vector<INT32>::iterator iter = vcSelectedSkill.begin();
	while(iter != vcSelectedSkill.end())
	{
		// 이미 배운것은 빼준다.
		if(GetSkill(*iter))
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		// 배울수 있는 건지 확인한다.
		AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(*iter);
		if(CheckLearnableSkill(pcsSkillTemplate) == FALSE)
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		++iter;
	}

	if(vcSelectedSkill.empty())
		return 0;

	// 남은 스킬 포인트와 비교한다.
	if(vcSelectedSkill.size() > m_lSkillPoint)
	{
		::AfxMessageBox(IDS_SKILL_NOT_ENOUGH_POINT);
		return 0;
	}

	CHAR szTmp[64];
	memset(szTmp, 0, sizeof(szTmp));

	// 오프라인일 경우 이미 가지고 있는 스킬을 붙여준다.
	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		SkillIter sIter = m_listSkill.begin();
		while(sIter != m_listSkill.end())
		{
			sprintf(szTmp, "%d,%d:", (*sIter)->m_lTID, (*sIter)->m_lLevel);
			_tcscat(stSkillString.m_szTreeNode, szTmp);
			++sIter;
		}
	}

	// 문장을 만들어준다.
	iter = vcSelectedSkill.begin();
	while(iter != vcSelectedSkill.end())
	{
		if(IsProductSkill(*iter))
			sprintf(szTmp, "%d,0:", *iter);
		else
			sprintf(szTmp, "%d,1:", *iter);

		_tcscat(stSkillString.m_szTreeNode, szTmp);
		++iter;
	}

	return _tcslen(stSkillString.m_szTreeNode);
}

INT32 AlefAdminSkill::MakeLevelString(vector<INT32>& vcSelectedSkill, stAgpdAdminSkillString& stSkillString, BOOL bUp)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 업그레이드 할 수 있는 스킬인지 점검.
	// 아닌 스킬들은 제외시킨다.
	vector<INT32>::iterator iter = vcSelectedSkill.begin();
	while(iter != vcSelectedSkill.end())
	{
		AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(*iter);
		if(!pcsSkillTemplate)
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		if(!CheckEnableInitialization(pcsSkillTemplate))
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		AgpdEventSkillAttachTemplateData* pcsAttachTemplateData = m_pcsAgpmEventSkillMaster->GetSkillAttachTemplateData(pcsSkillTemplate);
		stAgpdAdminSkill* pstSkill = GetSkill(*iter);
		if(!pcsAttachTemplateData || !pstSkill)
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		// 최고치까지 배웟는 지 확인.
		if(bUp && pcsAttachTemplateData->lMaxUpgradeLevel <= pstSkill->m_lLevel)
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		// 캐릭터가 배울 수 있는 레벨에 도달했는 지 체크
		if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData.m_stStatus.m_lLevel <
			pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][pstSkill->m_lLevel + 1])
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		// 레벨 감소하는 것에서 현재 레벨이 1이라면 뺀다. 얘는 Initialize 로 따로 처리해야 한다.
		if(!bUp && pstSkill->m_lLevel == 1)
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		++iter;
	}

	if(vcSelectedSkill.empty())
		return 0;

	// 레벨을 올려주는 상황이라면 스킬포인트 점검
	if(bUp && vcSelectedSkill.size() > m_lSkillPoint)
	{
		CString szMsg = _T("");
		szMsg.LoadString(IDS_SKILL_NOT_ENOUGH_POINT);
		::AfxMessageBox((LPCTSTR)szMsg);
		return 0;
	}

	CHAR szTmp[64];
	memset(szTmp, 0, sizeof(szTmp));

	// 오프라인일 경우 가지고 있는 스킬의 레벨을 조정해준다.
	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		SkillIter sIter = m_listSkill.begin();
		while(sIter != m_listSkill.end())
		{
			// 변경이 가해져야 할 애들을 걸러낸다.
			vector<INT32>::iterator Iter = std::find(vcSelectedSkill.begin(), vcSelectedSkill.end(), (*sIter)->m_lTID);
			if(Iter != vcSelectedSkill.end())
				sprintf(szTmp, "%d,%d:", (*sIter)->m_lTID, bUp ? (*sIter)->m_lLevel + 1 : (*sIter)->m_lLevel - 1);
			else
				sprintf(szTmp, "%d,%d:", (*sIter)->m_lTID, (*sIter)->m_lLevel);

			_tcscat(stSkillString.m_szTreeNode, szTmp);
			++sIter;
		}
	}
	// 온라인일 경우에는 변경이 가해질 애들만 스트링으로 만든다.
	else
	{
		vector<INT32>::iterator iter = vcSelectedSkill.begin();
		while(iter != vcSelectedSkill.end())
		{
			sprintf(szTmp, "%d,1:", *iter);		// 어차피 여기 레벨값은 무시 되기 때문에 1로 Dummy 를 주었다.
			_tcscat(stSkillString.m_szTreeNode, szTmp);

			++iter;
		}
	}

	return _tcslen(stSkillString.m_szTreeNode);
}

INT32 AlefAdminSkill::MakeInitAllString(stAgpdAdminSkillString& stSkillString)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CHAR szTmp[64];
	memset(szTmp, 0, sizeof(szTmp));

	// 현재 가지고 있는 스킬에서 기본적으로 가져야 하는 스킬만 넣는다.
	// 온라인이던, 오프라인이던 상관없이 아래 작업은 한다.
	SkillIter sIter = m_listSkill.begin();
	while(sIter != m_listSkill.end())
	{
		AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate((*sIter)->m_lTID);
		if(IsDefaultSkill(pcsSkillTemplate, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData.m_stBasic.m_lTID))
		{
			sprintf(szTmp, "%d,1:", pcsSkillTemplate->m_lID);
			_tcscat(stSkillString.m_szTreeNode, szTmp);
		}

		++sIter;
	}

	// 오프라인일 경우에는 Product Compose 내용도 넣어준다.
	//
	//

	return _tcslen(stSkillString.m_szTreeNode);
}

INT32 AlefAdminSkill::MakeInitSpecificString(vector<INT32>& vcSelectedSkill, stAgpdAdminSkillString& stSkillString)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 아닌 스킬들은 제외시킨다.
	vector<INT32>::iterator iter = vcSelectedSkill.begin();
	while(iter != vcSelectedSkill.end())
	{
		AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(*iter);
		if(!pcsSkillTemplate)
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		if(!CheckEnableInitialization(pcsSkillTemplate))
		{
			iter = vcSelectedSkill.erase(iter);
			continue;
		}

		++iter;
	}

	if(vcSelectedSkill.empty())
		return 0;

	CHAR szTmp[64];
	memset(szTmp, 0, sizeof(szTmp));

	// 오프라인일 경우 이미 가지고 있는 스킬과 비교해서 전체 스트링을 만들어주고
	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		SkillIter sIter = m_listSkill.begin();
		while(sIter != m_listSkill.end())
		{
			vector<INT32>::iterator Iter = std::find(vcSelectedSkill.begin(), vcSelectedSkill.end(), (*sIter)->m_lTID);
			if(Iter == vcSelectedSkill.end())	// 초기화 할 목록에 없는 것만 String 만든다.
			{
				sprintf(szTmp, "%d,%d:", (*sIter)->m_lTID, (*sIter)->m_lLevel);
				_tcscat(stSkillString.m_szTreeNode, szTmp);
			}
			++sIter;
		}

		// Product Compose 도 넣어준다.
		//
		//
	}
	// 온라인일 경우에는 초기화 되는 애들만 만들어준다.
	else
	{
		vector<INT32>::iterator iter = vcSelectedSkill.begin();
		while(iter != vcSelectedSkill.end())
		{
			sprintf(szTmp, "%d,1:", *iter);
			_tcscat(stSkillString.m_szTreeNode, szTmp);

			++iter;
		}
	}

	return _tcslen(stSkillString.m_szTreeNode);
}

INT32 AlefAdminSkill::MakeCurrentString(stAgpdAdminSkillString& stSkillString)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CHAR szTmp[64];
	memset(szTmp, 0, sizeof(szTmp));

	SkillIter sIter = m_listSkill.begin();
	while(sIter != m_listSkill.end())
	{
		sprintf(szTmp, "%d,%d:", (*sIter)->m_lTID, (*sIter)->m_lLevel);
		_tcscat(stSkillString.m_szTreeNode, szTmp);

		++sIter;
	}

	return _tcslen(stSkillString.m_szTreeNode);
}

INT32 AlefAdminSkill::MakeLearnComposeString(vector<INT32>& vcSelected, stAgpdAdminSkillString& stSkillString)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	ProductVector vcNew;
	std::set_difference(vcSelected.begin(), vcSelected.end(), m_vcProduct.begin(), m_vcProduct.end(), back_inserter(vcNew));

	CHAR szTmp[64];
	memset(szTmp, 0, sizeof(szTmp));

	// 오프라인일 경우 이미 가지고 있는 애들과 비교해서 전체 스트링을 만들어주고
	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		ProductIter iter = m_vcProduct.begin();
		while(iter != m_vcProduct.end())
		{
			sprintf(szTmp, "%d:", *iter);
			_tcscat(stSkillString.m_szProduct, szTmp);

			++iter;
		}

		iter = vcNew.begin();
		while(iter != vcNew.end())
		{
			sprintf(szTmp, "%d:", *iter);
			_tcscat(stSkillString.m_szProduct, szTmp);

			++iter;
		}
	}
	// 온라인일 경우에는 배우는 애들만 만들어준다.
	else
	{
		ProductIter iter = vcNew.begin();
		while(iter != vcNew.end())
		{
			sprintf(szTmp, "%d:", *iter);
			_tcscat(stSkillString.m_szProduct, szTmp);

			++iter;
		}
	}

	return _tcslen(stSkillString.m_szProduct);
}

INT32 AlefAdminSkill::MakeInitAllComposeString(stAgpdAdminSkillString& stSkillString)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 꺙 깔끔하게 bzero
	memset(stSkillString.m_szProduct, 0, sizeof(stSkillString));
	return 0;
}

INT32 AlefAdminSkill::MakeInitSpecificComposeString(vector<INT32>& vcSelected, stAgpdAdminSkillString& stSkillString)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CHAR szTmp[64];
	memset(szTmp, 0, sizeof(szTmp));

	// 오프라인일 경우 vcNew 만 보내면 된다.
	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		ProductVector vcNew;
		std::set_difference(m_vcProduct.begin(), m_vcProduct.end(), vcSelected.begin(), vcSelected.end(), back_inserter(vcNew));

		ProductIter iter = vcNew.begin();
		while(iter != vcNew.end())
		{
			sprintf(szTmp, "%d:", *iter);
			_tcscat(stSkillString.m_szProduct, szTmp);

			++iter;
		}
	}
	// 온라인일 경우에는 작업할 수 없다.
	else
	{
	}

	return _tcslen(stSkillString.m_szProduct);
}

INT32 AlefAdminSkill::MakeCurrentProductComposeString(stAgpdAdminSkillString& stSkillString)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CHAR szTmp[64];
	memset(szTmp, 0, sizeof(szTmp));

	ProductIter iter = m_vcProduct.begin();
	while(iter != m_vcProduct.end())
	{
		sprintf(szTmp, "%d:", *iter);
		_tcscat(stSkillString.m_szProduct, szTmp);

		++iter;
	}

	return _tcslen(stSkillString.m_szProduct);
}

BOOL AlefAdminSkill::CheckEnableInitialization(AgpdSkillTemplate* pcsSkillTemplate)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsSkillTemplate)
		return FALSE;

	// 스킬이면 일단 TRUE
	if(pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
		return TRUE;

	//if(m_pcsAgpmEventSkillMaster->GetBuyCost(pcsSkillTemplate) == 0)
	//	return FALSE;
	AgpdEventSkillAttachTemplateData* pcsAttachTemplateData	= m_pcsAgpmEventSkillMaster->GetSkillAttachTemplateData(pcsSkillTemplate);
	if(pcsAttachTemplateData->m_stSkillCost[1].lCostMoney < 1)
		return FALSE;

	if(m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkillTemplate) == FALSE)
		return FALSE;

	// 기본으로 가지고 있는 스킬인지 확인.
	if(IsDefaultSkill(pcsSkillTemplate, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData.m_stBasic.m_lTID))
		return FALSE;

	return TRUE;
}

BOOL AlefAdminSkill::IsDefaultSkill(AgpdSkillTemplate* pcsSkillTemplate, INT32 lCharTID)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsSkillTemplate)
		return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmCharacter" );

	AgpdCharacterTemplate* pcsCharacterTemplate = ppmCharacter->GetCharacterTemplate(lCharTID);
	if(!pcsCharacterTemplate)
		return FALSE;

	AgpdEventSkillAttachCharTemplateData* pcsCharTemplateAttachData	= m_pcsAgpmEventSkillMaster->GetSkillAttachCharTemplateData(pcsCharacterTemplate);
	if(!pcsCharTemplateAttachData)
		return FALSE;

	for(int i = 0; i < pcsCharTemplateAttachData->m_lNumDefaultSkill; ++i)
	{
		if(pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate[i] == pcsSkillTemplate)
			return TRUE;
	}

	return FALSE;
}

BOOL AlefAdminSkill::IsOwnIndexProduct()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iCurSel = m_csSkillTypeCB.GetCurSel();
	return iCurSel == ALEFADMINSKILL_INDEX_OWN_PRODUCT;
}

BOOL AlefAdminSkill::IsOwnIndexCompose()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iCurSel = m_csSkillTypeCB.GetCurSel();
	return iCurSel == ALEFADMINSKILL_INDEX_OWN_PRODUCT_COMPOSE;
}

BOOL AlefAdminSkill::IsAllIndexCompose()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iCurSel = m_csSkillCategory.GetCurSel();
	return iCurSel == ALEFADMINSKILL_INDEX_ALL_PRODUCT_COMPOSE;
}

BOOL AlefAdminSkill::IsProductSkill(INT32 lTID)
{
	return IsProductSkill(GetSkillTemplate(lTID));
}

BOOL AlefAdminSkill::IsProductSkill(AgpdSkillTemplate* pcsSkillTemplate)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsSkillTemplate)
		return FALSE;

	return (pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT);
}

// 생산스킬의 경험치를 바탕으로 현재 레벨 수치를 얻는다.
INT32 AlefAdminSkill::GetLevelOfExp(stAgpdAdminSkill& stSkill)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!stSkill.m_pcsTemplate)
		return 0;

	AgpdProductSkillFactor *pAgpdProductSkillFactor =
			m_pcsAgpmProduct->GetProductSkillFactor(((AgpdSkillTemplate*)stSkill.m_pcsTemplate)->m_szName);
	// non-growable skill
	if (!pAgpdProductSkillFactor)
		return 0;

	return m_pcsAgpmProduct->GetLevelOfExp(pAgpdProductSkillFactor, stSkill.m_lLevel);
}

// 생산스킬의 경험치를 바탕으로 현재 레벨에서 남는 Exp 값을 구한다.
INT32 AlefAdminSkill::GetExpOfCurrLevel(stAgpdAdminSkill& stSkill)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!stSkill.m_pcsTemplate)
		return 0;

	AgpdProductSkillFactor *pAgpdProductSkillFactor =
			m_pcsAgpmProduct->GetProductSkillFactor(((AgpdSkillTemplate*)stSkill.m_pcsTemplate)->m_szName);
	// non-growable skill
	if (!pAgpdProductSkillFactor)
		return 0;

	INT32 lLevel = GetLevelOfExp(stSkill);
	INT32 lExp = (stSkill.m_lLevel - pAgpdProductSkillFactor->m_lFactors[lLevel-1][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP]);
	return ((lExp > 0) ? lExp : 0);
}

INT32 AlefAdminSkill::GetMaxExpOfCurrLevel(stAgpdAdminSkill& stSkill, INT32 lLevel)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!stSkill.m_pcsTemplate)
		return 0;

	AgpdProductSkillFactor *pAgpdProductSkillFactor =
			m_pcsAgpmProduct->GetProductSkillFactor(((AgpdSkillTemplate*)stSkill.m_pcsTemplate)->m_szName);
	// non-growable skill
	if (!pAgpdProductSkillFactor)
		return 0;

	if(lLevel == 0)
		lLevel = GetLevelOfExp(stSkill);

	return pAgpdProductSkillFactor->m_lFactors[lLevel][AGPMPRODUCT_SKILL_FACTOR_LEVELUP_EXP];
}

INT32 AlefAdminSkill::GetAccExpOfCurrLevel(stAgpdAdminSkill& stSkill, INT32 lLevel)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!stSkill.m_pcsTemplate)
		return 0;

	AgpdProductSkillFactor *pAgpdProductSkillFactor =
			m_pcsAgpmProduct->GetProductSkillFactor(((AgpdSkillTemplate*)stSkill.m_pcsTemplate)->m_szName);
	// non-growable skill
	if (!pAgpdProductSkillFactor)
		return 0;

	if(lLevel == 0)
		lLevel = GetLevelOfExp(stSkill);

	return pAgpdProductSkillFactor->m_lFactors[lLevel][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP];
}





BOOL AlefAdminSkill::AddAdminMessage(CHAR* szSkillName, UINT ulResourceID, INT32 lValue1)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szSkillName || !ulResourceID)
		return FALSE;

	// 메시지 출력
	CString szMessage = _T("");
	CString szFormat = _T("");

	szFormat.LoadString(ulResourceID);
	if(ulResourceID != IDS_SKILL_UPGRADE_SUCCESS)
		szMessage.Format((LPCTSTR)szFormat, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName, szSkillName);
	else
		szMessage.Format((LPCTSTR)szFormat, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName, szSkillName, lValue1);

	AlefAdminManager::Instance()->m_pMainDlg->ProcessAdminMessage((LPCTSTR)szMessage);

	return TRUE;
}

void AlefAdminSkill::MessageRefreshOffLine()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
		::AfxMessageBox(IDS_EDIT_DB_REFRESH);
}

BOOL AlefAdminSkill::IsHighLevelSkill( AuRaceType eRaceType, AuCharClassType eClassType, INT32 nSkillTID )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	INT32 nCharacterTID = -1;
	switch( eRaceType )
	{
	case AURACE_TYPE_HUMAN :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	nCharacterTID = 96;		break;
			case AUCHARCLASS_TYPE_RANGER :	nCharacterTID = 1;		break;
			case AUCHARCLASS_TYPE_MAGE : 	nCharacterTID = 6;		break;
			}
		}
		break;

	case AURACE_TYPE_ORC :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	nCharacterTID = 4;		break;
			case AUCHARCLASS_TYPE_RANGER :	nCharacterTID = 8;		break;
			case AUCHARCLASS_TYPE_MAGE : 	nCharacterTID = 3;		break;
			}
		}
		break;

	case AURACE_TYPE_MOONELF :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	nCharacterTID = 377;	break;
			case AUCHARCLASS_TYPE_RANGER :	nCharacterTID = 460;	break;
			case AUCHARCLASS_TYPE_MAGE : 	nCharacterTID = 9;		break;
			}
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	nCharacterTID = DRAGONSCION_SLAYER;					break;
			case AUCHARCLASS_TYPE_RANGER :	nCharacterTID = DRAGONSCION_OBITER;					break;
			case AUCHARCLASS_TYPE_MAGE : 	nCharacterTID = DRAGONSCION_SUMMERNER;				break;
			case AUCHARCLASS_TYPE_SCION : 	nCharacterTID = DRAGONSCION_SCION;					break;
			case AUCHARCLASS_TYPE_MAX : 	nCharacterTID = AGPMEVENT_HIGHLEVEL_PASSIVE_SCION;	break;
			}
		}
		break;
	}

	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( nCharacterTID );
	if( !pVector ) return TRUE;

	HighLevelSkillIter iter = pVector->begin();
	while( iter != pVector->end() )
	{
		AgpdEventSkillHighLevel stHighLevel = *iter;
		AgpdSkillTemplate* ppdSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate( stHighLevel.m_lSkillTID );

		if( ppdSkillTemplate && ppdSkillTemplate->m_lID == nSkillTID )
		{
			return TRUE;
		}

		iter++;
	}

	return FALSE;
}

BOOL AlefAdminSkill::IsCurrentClassSkillForDragonScion( INT32 nSelect, INT32 nSkillTID )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	INT32 nCharacterTID = -1;
	switch( nSelect )
	{
	case 0 :	nCharacterTID = DRAGONSCION_SCION;					break;
	case 1 :	nCharacterTID = DRAGONSCION_SLAYER;					break;
	case 2 : 	nCharacterTID = DRAGONSCION_OBITER;					break;
	case 3 : 	nCharacterTID = DRAGONSCION_SUMMERNER;				break;
	case 4 : 	nCharacterTID = AGPMEVENT_HIGHLEVEL_PASSIVE_SCION;	break;
	}

	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( nCharacterTID );
	if( !pVector ) return TRUE;

	HighLevelSkillIter iter = pVector->begin();
	while( iter != pVector->end() )
	{
		AgpdEventSkillHighLevel stHighLevel = *iter;
		AgpdSkillTemplate* ppdSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate( stHighLevel.m_lSkillTID );

		if( ppdSkillTemplate && ppdSkillTemplate->m_lID == nSkillTID )
		{
			return TRUE;
		}

		iter++;
	}

	return FALSE;
}

BOOL AlefAdminSkill::CBComposeInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdComposeCharacterAD* pcsComposeAD = static_cast<AgpdComposeCharacterAD*>(pData);
	AlefAdminSkill* pThis = static_cast<AlefAdminSkill*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pCustData);

	// 검색한 캐릭터가 맞다면
	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pcsCharacter->m_szID))
	{
		for(int i = 0; i < pcsComposeAD->m_lCount; ++i)
		{
			// 패킷이 여러번 날아올 수 있으므로, 없는 것만 넣는다.
			ProductIter iter = std::find(pThis->m_vcProduct.begin(), pThis->m_vcProduct.end(), pcsComposeAD->m_OwnComposes[i]);
			if(iter == pThis->m_vcProduct.end())
				pThis->m_vcProduct.push_back(pcsComposeAD->m_OwnComposes[i]);
		}

		pThis->ShowProductCompose();
	}

	return TRUE;
}








int AlefAdminSkill::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL AlefAdminSkill::OnEraseBkgnd(CDC* pDC)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UNREFERENCED_PARAMETER(pDC);	
	return TRUE;
	//return CFormView::OnEraseBkgnd(pDC);
}

void AlefAdminSkill::OnPaint()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CPaintDC dc(this); // device context for painting
	// 그리기 메시지에 대해서는 CFormView::OnPaint()을(를) 호출하지 마십시오.

	CRect r;
	GetClientRect(&r);
	CXTMemDC memDC(&dc, r);

	// 왠지 모르게 이놈만 새로 안그려지길래 무효화를 강제로 시킨다.
	//m_csSkillList.Invalidate();
	
	CFormView::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
}

// 2006.05.12. 리뉴얼
void AlefAdminSkill::OnBnClickedBLearn()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 현재 검색한 캐릭터가 없으면 GG
	if(strlen(AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) == 0)
		return;

	// 레벨 체크
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(::AfxMessageBox(IDS_SKILL_CONFIRM_5, MB_YESNO) == IDNO)
		return;

	vector<INT32> vcSelectedSkill;
	if(GetSelectedAllSkill(vcSelectedSkill) == 0)
		return;

	stAgpdAdminSkillString stSkillString;
	memset(&stSkillString, 0, sizeof(stSkillString));

	stAgpdAdminCharEdit stCharEdit;
	memset(&stCharEdit, 0, sizeof(stCharEdit));

	// 생산물이 아니면
	if(IsAllIndexCompose() == FALSE)
	{
		if(MakeLearnString(vcSelectedSkill, stSkillString) == 0)
			return;

		MakeCurrentProductComposeString(stSkillString);
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_SKILL_LEARN;
	}
	else
	{
		if(MakeLearnComposeString(vcSelectedSkill, stSkillString) == 0)
			return;

		MakeCurrentString(stSkillString);
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_COMPOSE_LEARN;
	}

	stCharEdit.m_lCID = 1;
	strcpy(stCharEdit.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);

	// 스킬 데이터에다가도 캐릭터 이름 세팅
	_tcsncpy(stSkillString.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName, AGPDCHARACTER_NAME_LENGTH);

	AlefAdminManager::Instance()->GetAdminData()->SendCharacterModification(&stCharEdit, &stSkillString);
	MessageRefreshOffLine();
}

void AlefAdminSkill::OnBnClickedBRefresh()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	AlefAdminManager::Instance()->m_pMainDlg->SearchCharacter(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData.m_stBasic.m_szCharName);
}

void AlefAdminSkill::OnCbnSelchangeCbSkillType()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int lIndex = m_csSkillTypeCB.GetCurSel();
	if(lIndex == CB_ERR)
		return;

	ClearSkillListCtrl();
	ShowData();
}

void AlefAdminSkill::OnCbnSelchangeCbSkillCategory()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int lIndex = m_csSkillCategory.GetCurSel();
	if(lIndex == CB_ERR)
		return;

	ClearSkillAllListCtrl();
	ShowAllSkill();
}

void AlefAdminSkill::OnNMClickLvAllSkill(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	BOOL bCheck = m_csSkillLV.GetCheck(nlv->iItem);

	// 체크가 되어 있다면 풀어주고, 안되어 있다면 체크해준다.
	if(bCheck)
		m_csSkillLV.SetCheck(nlv->iItem, 0);
	else
		m_csSkillLV.SetCheck(nlv->iItem, 1);

	*pResult = 0;
}

void AlefAdminSkill::OnNMClickLvOwnSkill(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	BOOL bCheck = m_csOwnSkillLV.GetCheck(nlv->iItem);

	// 체크가 되어 있다면 풀어주고, 안되어 있다면 체크해준다.
	if(bCheck)
		m_csOwnSkillLV.SetCheck(nlv->iItem, 0);
	else
		m_csOwnSkillLV.SetCheck(nlv->iItem, 1);

	// 생산스킬이라면 Exp 뿌려준다.
	CString szName = m_csOwnSkillLV.GetItemText(nlv->iItem, 0);
	AgpdSkillTemplate* pcsSkillTemplate = GetSkillTemplate((CHAR*)(LPCTSTR)szName);
	if(IsProductSkill(pcsSkillTemplate))
	{
		stAgpdAdminSkill* pstSkill = GetSkill(pcsSkillTemplate->m_lID);
		if(pstSkill)
		{
			INT32 lCurrExp = GetExpOfCurrLevel(*pstSkill);
			INT32 lCurrMax = GetMaxExpOfCurrLevel(*pstSkill);

			szName.Format("%d / %d (%3.1f %%)", lCurrExp, lCurrMax, (double)lCurrExp / (double)lCurrMax * 100.0);
			SetDlgItemText(IDC_E_PRODUCT_LEVEL, (LPCTSTR)szName);
		}
	}
	else
		SetDlgItemText(IDC_E_PRODUCT_LEVEL, _T(""));

	*pResult = 0;
}

void AlefAdminSkill::OnBnClickedCkOwnSkillAll()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iCheck = m_csOwnSkillAllCheckBox.GetCheck();
	for(int i = 0; i < m_csOwnSkillLV.GetItemCount(); i++)
		m_csOwnSkillLV.SetCheck(i, iCheck == BST_CHECKED ? 1 : 0);
}

void AlefAdminSkill::OnBnClickedCkAllSkillAll()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iCheck = m_csAllSkillAllCheckBox.GetCheck();
	for(int i = 0; i < m_csSkillLV.GetItemCount(); i++)
		m_csSkillLV.SetCheck(i, iCheck == BST_CHECKED ? 1 : 0);
}

void AlefAdminSkill::OnBnClickedBInitSkillAll()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 현재 검색한 캐릭터가 없으면 GG
	if(strlen(AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) == 0)
		return;

	// 레벨 체크
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(::AfxMessageBox(IDS_SKILL_CONFIRM_1, MB_YESNO) == IDNO)
		return;

	stAgpdAdminSkillString stSkillString;
	memset(&stSkillString, 0, sizeof(stSkillString));

	stAgpdAdminCharEdit stCharEdit;
	memset(&stCharEdit, 0, sizeof(stCharEdit));

	if(IsOwnIndexCompose() == FALSE)
	{
		if(MakeInitAllString(stSkillString) == 0)
			return;

		MakeCurrentProductComposeString(stSkillString);

		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_SKILL_INIT_ALL;
	}
	else
	{
		// 캐릭터가 온라인이면 불가능한 작업이다.
		if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline())
		{
			::AfxMessageBox(IDS_SKILL_DISABLE_BY_ONLINE);
			return;
		}

		MakeInitAllComposeString(stSkillString);
		MakeCurrentString(stSkillString);

		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_COMPOSE_INIT_ALL;
	}

	stCharEdit.m_lCID = 1;
	strcpy(stCharEdit.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);

	// 스킬 데이터에도 캐릭터 이름 세팅
	_tcsncpy(stSkillString.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName, AGPDCHARACTER_NAME_LENGTH);

	AlefAdminManager::Instance()->GetAdminData()->SendCharacterModification(&stCharEdit, &stSkillString);
	MessageRefreshOffLine();
}

void AlefAdminSkill::OnBnClickedBInitSkillSpecific()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 현재 검색한 캐릭터가 없으면 GG
	if(strlen(AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) == 0)
		return;

	// 레벨 체크
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(::AfxMessageBox(IDS_SKILL_CONFIRM_2, MB_YESNO) == IDNO)
		return;

	vector<INT32> vcSelectedSkill;
	if(GetSelectedOwnSkill(vcSelectedSkill) == 0)
		return;

	stAgpdAdminSkillString stSkillString;
	memset(&stSkillString, 0, sizeof(stSkillString));

	stAgpdAdminCharEdit stCharEdit;
	memset(&stCharEdit, 0, sizeof(stCharEdit));

	if(IsOwnIndexCompose() == FALSE)
	{
		if(MakeInitSpecificString(vcSelectedSkill, stSkillString) == 0)
			return;

		MakeCurrentProductComposeString(stSkillString);
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_SKILL_INIT_SPECIFIC;
	}
	else
	{
		// 캐릭터가 온라인이면 불가능한 작업이다.
		if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline())
		{
			::AfxMessageBox(IDS_SKILL_DISABLE_BY_ONLINE);
			return;
		}

		MakeInitSpecificComposeString(vcSelectedSkill, stSkillString);
		MakeCurrentString(stSkillString);
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_COMPOSE_INIT_SPECIFIC;
	}

	stCharEdit.m_lCID = 1;
	strcpy(stCharEdit.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);

	// 스킬 데이터에도 캐릭터 이름 세팅
	_tcsncpy(stSkillString.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName, AGPDCHARACTER_NAME_LENGTH);

	AlefAdminManager::Instance()->GetAdminData()->SendCharacterModification(&stCharEdit, &stSkillString);
	MessageRefreshOffLine();
}

void AlefAdminSkill::OnBnClickedBLevelUp()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(::AfxMessageBox(IDS_SKILL_CONFIRM_3, MB_YESNO) == IDNO)
		return;

	if(IsOwnIndexProduct())
		ProcessProductLevel(TRUE);
	else
		ProcessLevelUpDown(TRUE);
}

void AlefAdminSkill::OnBnClickedBLevelDown()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(::AfxMessageBox(IDS_SKILL_CONFIRM_4, MB_YESNO) == IDNO)
		return;

	if(IsOwnIndexProduct())
		ProcessProductLevel(FALSE);
	else
		ProcessLevelUpDown(FALSE);
}

BOOL AlefAdminSkill::ProcessLevelUpDown(BOOL bUp)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 현재 검색한 캐릭터가 없으면 GG
	if(strlen(AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) == 0)
		return FALSE;

	// 레벨 체크
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return FALSE;
	}

	// 생산물에는 이런 작업을 할 수 없다.
	if(IsOwnIndexCompose())
	{
		::AfxMessageBox(IDS_SKILL_DISABLE_BY_COMPOSE);
		return FALSE;
	}

	vector<INT32> vcSelectedSkill;
	if(GetSelectedOwnSkill(vcSelectedSkill) == 0)
		return FALSE;

	stAgpdAdminSkillString stSkillString;
	memset(&stSkillString, 0, sizeof(stSkillString));
	if(MakeLevelString(vcSelectedSkill, stSkillString, bUp) == 0)
		return FALSE;

	MakeCurrentProductComposeString(stSkillString);

	stAgpdAdminCharEdit stCharEdit;
	memset(&stCharEdit, 0, sizeof(stCharEdit));

	stCharEdit.m_lCID = 1;
	strcpy(stCharEdit.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);

	stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_SKILL_LEVEL;
	if(bUp)
		stCharEdit.m_lNewValueINT = 1;
	else
		stCharEdit.m_lNewValueINT = -1;

	// 스킬 데이터에도 캐릭터 이름 세팅
	_tcsncpy(stSkillString.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName, AGPDCHARACTER_NAME_LENGTH);

	AlefAdminManager::Instance()->GetAdminData()->SendCharacterModification(&stCharEdit, &stSkillString);
	MessageRefreshOffLine();
}

void AlefAdminSkill::OnBnClickedBProductExpChange()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 현재 검색한 캐릭터가 없으면 GG
	if(strlen(AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) == 0)
		return;

	// 레벨 체크
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(!IsOwnIndexProduct())
		return;

	// 캐릭터가 온라인이면 불가능한 작업이다.
	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline())
	{
		::AfxMessageBox(IDS_SKILL_DISABLE_BY_ONLINE);
		return;
	}

	CString szNewValue = _T("");
	GetDlgItemText(IDC_E_PRODUCT_LEVEL, szNewValue);

	// % 표시가 있으면 뺀다.
	szNewValue.Remove('%%');
	double dNewExpPer = atof((LPCTSTR)szNewValue);
	if(dNewExpPer < 0.0 || dNewExpPer > 100.0)
	{
		::AfxMessageBox(IDS_SKILL_EXP_ERROR);
		return;
	}

	vector<INT32> vcSelectedSkill;
	if(GetSelectedOwnSkill(vcSelectedSkill) == 0)
		return;

	stAgpdAdminSkillString stSkillString;
	memset(&stSkillString, 0, sizeof(stSkillString));

	MakeCurrentString(stSkillString);
	MakeCurrentProductComposeString(stSkillString);

	CString szSkillString = stSkillString.m_szTreeNode;
	CHAR szBefore[64], szAfter[64];
	memset(szBefore, 0, sizeof(szBefore));
	memset(szAfter, 0, sizeof(szAfter));

	vector<INT32>::iterator iter = vcSelectedSkill.begin();
	while(iter != vcSelectedSkill.end())
	{
		stAgpdAdminSkill* pstSkill = GetSkill(*iter);
		if(!pstSkill)
		{
			++iter;
			continue;
		}

		sprintf(szBefore, "%d,%d:", pstSkill->m_lTID, pstSkill->m_lLevel);

		INT32 lCurrLevel = GetLevelOfExp(*pstSkill);

		double dNewExp = (double)GetMaxExpOfCurrLevel(*pstSkill) * dNewExpPer / 100.0;
		if(lCurrLevel > 1)
			dNewExp += (double)GetAccExpOfCurrLevel(*pstSkill, lCurrLevel - 1);
		sprintf(szAfter, "%d,%d:", pstSkill->m_lTID, (INT32)dNewExp);

		szSkillString.Replace(szBefore, szAfter);

		++iter;
	}

	_tcscpy(stSkillString.m_szTreeNode, (LPCTSTR)szSkillString);

	stAgpdAdminCharEdit stCharEdit;
	memset(&stCharEdit, 0, sizeof(stCharEdit));

	stCharEdit.m_lCID = 1;
	strcpy(stCharEdit.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);

	stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_PRODUCT_EXP_CHANGE;

	// 스킬 데이터에도 캐릭터 이름 세팅
	_tcsncpy(stSkillString.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName, AGPDCHARACTER_NAME_LENGTH);

	AlefAdminManager::Instance()->GetAdminData()->SendCharacterModification(&stCharEdit, &stSkillString);
	MessageRefreshOffLine();
}

BOOL AlefAdminSkill::ProcessProductLevel(BOOL bUp)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 현재 검색한 캐릭터가 없으면 GG
	if(strlen(AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) == 0)
		return FALSE;

	// 레벨 체크
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return FALSE;
	}

	if(!IsOwnIndexProduct())
		return FALSE;

	// 캐릭터가 온라인이면 불가능한 작업이다.
	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline())
	{
		::AfxMessageBox(IDS_SKILL_DISABLE_BY_ONLINE);
		return FALSE;
	}

	vector<INT32> vcSelectedSkill;
	if(GetSelectedOwnSkill(vcSelectedSkill) == 0)
		return FALSE;

	stAgpdAdminSkillString stSkillString;
	memset(&stSkillString, 0, sizeof(stSkillString));

	MakeCurrentString(stSkillString);
	MakeCurrentProductComposeString(stSkillString);

	CString szSkillString = stSkillString.m_szTreeNode;
	CHAR szBefore[64], szAfter[64];
	memset(szBefore, 0, sizeof(szBefore));
	memset(szAfter, 0, sizeof(szAfter));

	vector<INT32>::iterator iter = vcSelectedSkill.begin();
	while(iter != vcSelectedSkill.end())
	{
		stAgpdAdminSkill* pstSkill = GetSkill(*iter);
		if(!pstSkill)
		{
			++iter;
			continue;
		}

		INT32 lCurrLevel = GetLevelOfExp(*pstSkill);
		if((bUp && lCurrLevel + 1 >= AGPMPRODUCT_MAX_LEVEL) ||
			(!bUp && lCurrLevel - 1 < 1))
		{
			++iter;
			continue;
		}

		sprintf(szBefore, "%d,%d:", pstSkill->m_lTID, pstSkill->m_lLevel);
		INT32 lNewExp = GetAccExpOfCurrLevel(*pstSkill, bUp ? lCurrLevel + 1 : lCurrLevel - 1) - 
						GetMaxExpOfCurrLevel(*pstSkill, bUp ? lCurrLevel + 1 : lCurrLevel - 1);
		sprintf(szAfter, "%d,%d:", pstSkill->m_lTID, lNewExp);

		szSkillString.Replace(szBefore, szAfter);

		++iter;
	}

	_tcscpy(stSkillString.m_szTreeNode, (LPCTSTR)szSkillString);

	stAgpdAdminCharEdit stCharEdit;
	memset(&stCharEdit, 0, sizeof(stCharEdit));

	stCharEdit.m_lCID = 1;
	strcpy(stCharEdit.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);

	stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_PRODUCT_EXP_CHANGE;

	// 스킬 데이터에도 캐릭터 이름 세팅
	_tcsncpy(stSkillString.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName, AGPDCHARACTER_NAME_LENGTH);

	AlefAdminManager::Instance()->GetAdminData()->SendCharacterModification(&stCharEdit, &stSkillString);
	MessageRefreshOffLine();
	return TRUE;
}