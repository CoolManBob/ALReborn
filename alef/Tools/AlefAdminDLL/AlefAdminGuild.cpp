// AlefAdminGuild.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminGuild.h"
#include ".\alefadminguild.h"
#include "AlefAdminManager.h"
#include "AgcmGuild.h"


#define ALEFADMINGUILD_DATA_NONE					"-----------"

//#define ALEFADMINGUILD_NOTICE_DEFAULT				"이곳에 공지사항이 옵니다"
//
//#define ALEFADMINGUILD_COLUMN_1						"Data Name"
//#define ALEFADMINGUILD_COLUMN_2						"Data"
//
//#define ALEFADMINGUILD_CATEGORY_BASIC				"Basic"
//#define ALEFADMINGUILD_FIELD_GUILD_ID				"Guild ID"
//#define ALEFADMINGUILD_FIELD_MASTER_ID				"Master ID"
//#define ALEFADMINGUILD_FIELD_TID					"TID"
//#define ALEFADMINGUILD_FIELD_RANK					"Rank"
//#define ALEFADMINGUILD_FIELD_CREATE_DATE			"Create Date"
//#define ALEFADMINGUILD_FIELD_PASSWORD				"Password"
//#define ALEFADMINGUILD_FIELD_MEMBER					"Member"
//
//#define ALEFADMINGUILD_CATEGORY_BATTLE				"Battle"
//#define ALEFADMINGUILD_FIELD_STATUS					"Status"
//#define ALEFADMINGUILD_FIELD_RECORDS				"Record(s)"
//#define ALEFADMINGUILD_FIELD_ENEMY_GUILD_ID			"Enemy Guild ID"
//#define ALEFADMINGUILD_FIELD_ACCEPT_TIME			"Accept Time"
//#define ALEFADMINGUILD_FIELD_START_TIME				"Start Time"
//#define ALEFADMINGUILD_FIELD_DURATION				"Duration"
//#define ALEFADMINGUILD_FIELD_SCORE					"Score"
//#define ALEFADMINGUILD_FIELD_ENEMY_SCORE			"Enemy Score"

//#define ALEFADMINGUILD_NAME_MASTER					"마스터"
//#define ALEFADMINGUILD_NAME_MEMBER					"길드원"
//#define ALEFADMINGUILD_NAME_JOIN_REQUEST			"가입 신청중"
//#define ALEFADMINGUILD_NAME_LEAVE_REQUEST			"탈퇴 신청중"
//
//#define ALEFADMINGUILD_RESULT_LEAVE_MEMBER_SUCCESS	"길드 멤버를 탈퇴 시켰습니다."
//#define ALEFADMINGUILD_RESULT_LEAVE_MEMBER_FAILED	"길드 멤버를 탈퇴 시키지 못했습니다."
//#define ALEFADMINGUILD_RESULT_DESTROY_SUCCESS		"길드를 해체 시켰습니다."
//#define ALEFADMINGUILD_RESULT_DESTROY_FAILED		"길드를 해체 시키지 못했습니다."
//#define ALEFADMINGUILD_RESULT_CHANGE_MASTER_SUCCESS	"길드 마스터를 변경하였습니다."
//#define ALEFADMINGUILD_RESULT_CHANGE_MASTER_FAILED	"길드 마스터를 변경하지 못했습니다."
//#define ALEFADMINGUILD_RESULT_CHANGE_NOTICE_SUCCESS	"길드 공지를 변경하였습니다."
//#define ALEFADMINGUILD_RESULT_CHANGE_NOTICE_FAILED	"길드 공지를 변경하지 못했습니다."


// AlefAdminGuild

IMPLEMENT_DYNCREATE(AlefAdminGuild, CFormView)

AlefAdminGuild::AlefAdminGuild()
	: CFormView(AlefAdminGuild::IDD)
{
	//m_pGuildGrid = NULL;

	m_lLastSearchType = 0;
	memset(m_szLastSearchField, 0, sizeof(m_szLastSearchField));

	memset(m_szSearchedGuildID, 0, sizeof(m_szSearchedGuildID));
	memset(m_szSelectedMemberID, 0, sizeof(m_szSelectedMemberID));
	m_lSelectedMemberRank = 0;

	m_lTotalGuildCount = 0;
	m_lTotalMemberCount = 0;

    //xtAfxData.bXPMode = TRUE;
}

AlefAdminGuild::~AlefAdminGuild()
{
	//if(m_pGuildGrid)
	//{
	//	m_pGuildGrid->DestroyWindow();
	//	delete m_pGuildGrid;
	//}
}

void AlefAdminGuild::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_GUILD_SEARCH_TYPE, m_csSearchType);
	DDX_Control(pDX, IDC_B_GUILD_SEARCH, m_csSearchBtn);
	DDX_Control(pDX, IDC_B_GUILD_INFO_REFRESH, m_csRefreshBtn);
	DDX_Control(pDX, IDC_LV_MEMBER, m_csMemberLV);
	DDX_Control(pDX, IDC_LV_GUILD, m_csGuildDataLV);
	DDX_Control(pDX, IDC_B_GUILD_DESTROY, m_csGuildDestroyBtn);
	DDX_Control(pDX, IDC_B_MEMBER_FORCED_LEAVE, m_csMemberForcedLeaveBtn);
	DDX_Control(pDX, IDC_B_CHANGE_MASTER, m_csChangeMasterBtn);
	DDX_Control(pDX, IDC_B_CHANGE_NOTICE, m_csChangeNoticeBtn);
}

BEGIN_MESSAGE_MAP(AlefAdminGuild, CFormView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_B_GUILD_SEARCH, OnBnClickedBGuildSearch)
	ON_BN_CLICKED(IDC_B_GUILD_INFO_REFRESH, OnBnClickedBGuildInfoRefresh)
	ON_NOTIFY(NM_CLICK, IDC_LV_GUILD, OnNMClickLvGuild)
	ON_NOTIFY(NM_CLICK, IDC_LV_MEMBER, OnNMClickLvMember)
	ON_BN_CLICKED(IDC_B_GUILD_DESTROY, OnBnClickedBGuildDestroy)
	ON_BN_CLICKED(IDC_B_MEMBER_FORCED_LEAVE, OnBnClickedBMemberForcedLeave)
	ON_BN_CLICKED(IDC_B_CHANGE_MASTER, OnBnClickedBChangeMaster)
	ON_BN_CLICKED(IDC_B_CHANGE_NOTICE, OnBnClickedBChangeNotice)
END_MESSAGE_MAP()


// AlefAdminGuild 진단입니다.

#ifdef _DEBUG
void AlefAdminGuild::AssertValid() const
{
	CFormView::AssertValid();
}

void AlefAdminGuild::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// AlefAdminGuild 메시지 처리기입니다.

int AlefAdminGuild::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_pGuildGrid = new CXTPPropertyGrid;
	return 0;
}

BOOL AlefAdminGuild::OnEraseBkgnd(CDC* pDC)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UNREFERENCED_PARAMETER(pDC);	
	return TRUE;
	//return CFormView::OnEraseBkgnd(pDC);
}

void AlefAdminGuild::OnPaint()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CFormView::OnPaint()을(를) 호출하지 마십시오.

	CRect r;
	GetClientRect(&r);
	CXTMemDC memDC(&dc, r);
	
	//if(m_pGuildGrid)
	//	m_pGuildGrid->Invalidate();
	
	CFormView::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
}

void AlefAdminGuild::OnInitialUpdate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFormView::OnInitialUpdate();

	m_csGuildDataLV.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_COLUMN), LVCFMT_LEFT, 100);
	m_csGuildDataLV.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DATA), LVCFMT_LEFT, 200);

	InitComboBox();
	InitGrid();
	InitListView();

	InitGridData();

	// 얘만 여기서 불러준다.
	HWND hWndHeader = m_csGuildDataLV.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeader.SubclassWindow(hWndHeader);

	m_csGuildDataLV.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);



	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;

	m_csSearchBtn.SetXButtonStyle(dwStyle);
	m_csRefreshBtn.SetXButtonStyle(dwStyle);
	m_csGuildDestroyBtn.SetXButtonStyle(dwStyle);
	m_csMemberForcedLeaveBtn.SetXButtonStyle(dwStyle);
	m_csChangeMasterBtn.SetXButtonStyle(dwStyle);
	m_csChangeNoticeBtn.SetXButtonStyle(dwStyle);

	SetDlgItemText(IDC_E_GUILD_NOTICE, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_THIS_PLACE_NOTICE));

	// Callback 등록
	AgpmGuild* ppmGuild = ( AgpmGuild* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmGuild" );
	ppmGuild->SetCallbackGuildCreate(CBGuildCreate, this);
	ppmGuild->SetCallbackGuildJoin(CBGuildJoin, this);

	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )AlefAdminManager::Instance()->GetAdminData()->GetAgpmAdmin();
	ppmAdmin->SetCallbackGuild(CBReceiveGuildOperation, this);

	OnBnClickedBGuildInfoRefresh();
}

BOOL AlefAdminGuild::InitComboBox()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSearchType.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ID));
	m_csSearchType.SetCurSel(0);

	return TRUE;
}

BOOL AlefAdminGuild::InitGrid()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 2005.04.28. steeple 변경.
	// PropertyGrid 가 보이지 않는 이유로 인해서 그냥 ListView 로 바꿈

	// 안에 뿌려지는 내용만 넣어준다.
	// 컬럼이나 헤더 같은 건 OnInitialUpdate 에서 해준다.

	INT32 lIndex = 0;

	//m_csGuildDataLV.InsertItem(lIndex, ALEFADMINGUILD_CATEGORY_BASIC);
	//m_csGuildDataLV.SetItemText(lIndex++, 1, AELFADMINGUILD_DATA_NONE);

	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ID));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_MASTER_ID));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, _T("TID"));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_RANK));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_CREATION_DATE));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_PASSWORD));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_MEMBER_COUNT));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));

	// 한칸 띄어주고
	m_csGuildDataLV.InsertItem(lIndex, _T(""));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));

	//m_csGuildDataLV.InsertItem(lIndex, ALEFADMINGUILD_CATEGORY_BATTLE);
	//m_csGuildDataLV.SetItemText(lIndex++, 1, AELFADMINGUILD_DATA_NONE);

	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_STATUS));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_RECORD));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ENEMY_GUILD_ID));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_ACCEPT_TIME));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_START_TIME));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_DURATION));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_SCORE));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));
	m_csGuildDataLV.InsertItem(lIndex, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_ENEMY_SCORE));
	m_csGuildDataLV.SetItemText(lIndex++, 1, _T(""));

	return TRUE;

	//CRect rc;
	//m_csGuildDataPlace.GetWindowRect(&rc);
	//ScreenToClient(&rc);

	//if(m_pGuildGrid->Create(rc, this, IDC_PROPERTY_GRID_GUILD))
	//{
	//	m_pGuildGrid->SetFont(GetFont());
	//	
	//	// Create Basic Category
	//	CXTPPropertyGridItem* pBasic = m_pGuildGrid->AddCategory(_T("Basic"));
	//	pBasic->AddChildItem(new CXTPSetValueGridItem(_T("Guild Name"), _T("")));
	//	pBasic->AddChildItem(new CXTPSetValueGridItem(_T("Master Name"), _T("")));
	//	pBasic->AddChildItem(new CXTPSetValueGridItem(_T("TID"), _T("")));
	//	pBasic->AddChildItem(new CXTPSetValueGridItem(_T("Rank"), _T("")));
	//	pBasic->AddChildItem(new CXTPSetValueGridItem(_T("Create Date"), _T("")));
	//	pBasic->AddChildItem(new CXTPSetValueGridItem(_T("Password"), _T("")));
	//	pBasic->AddChildItem(new CXTPSetValueGridItem(_T("Member"), _T("")));

	//	pBasic->Expand();

	//	// Create Battle Category
	//	CXTPPropertyGridItem* pBattle = m_pGuildGrid->AddCategory(_T("Battle"));
	//	pBattle->AddChildItem(new CXTPSetValueGridItem(_T("Status"), _T("")));
	//	pBattle->AddChildItem(new CXTPSetValueGridItem(_T("Record(s)"), _T("")));
	//	pBattle->AddChildItem(new CXTPSetValueGridItem(_T("Enemy Guild Name"), _T("")));
	//	pBattle->AddChildItem(new CXTPSetValueGridItem(_T("Accept Time"), _T("")));
	//	pBattle->AddChildItem(new CXTPSetValueGridItem(_T("Start Time"), _T("")));
	//	pBattle->AddChildItem(new CXTPSetValueGridItem(_T("Duration"), _T("")));
	//	pBattle->AddChildItem(new CXTPSetValueGridItem(_T("Score"), _T("")));
	//	pBattle->AddChildItem(new CXTPSetValueGridItem(_T("Enemy Score"), _T("")));

	//	pBattle->Expand();

	//	// 색을 바꿔준다.
	//	m_pGuildGrid->SetCustomColors(RGB(200, 200, 200), 0, RGB(187, 216, 178), RGB(253, 255, 253), 0);

	//	// Description 을 끈다.
	//	m_pGuildGrid->ShowHelp(FALSE);

	//	m_pGuildGrid->ShowWindow(SW_SHOW);
	//	m_pGuildGrid->SetForegroundWindow();
	//}

	//return TRUE;
}

BOOL AlefAdminGuild::InitListView()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csMemberLV.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHARNAME), LVCFMT_LEFT, 150);
	m_csMemberLV.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LEVEL), LVCFMT_LEFT, 45);
	m_csMemberLV.InsertColumn(2, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_RANK), LVCFMT_LEFT, 60);

	HWND hWndHeader = m_csMemberLV.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeader2.SubclassWindow(hWndHeader);

	m_csMemberLV.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);

	return TRUE;
}

BOOL AlefAdminGuild::IsSearchGuild(CHAR* szGuildID, CHAR* szMasterID)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	switch(m_lLastSearchType)
	{
		case 0:			// 길드 이름 검색
			if(!szGuildID)
				return FALSE;
			
			return strcmp(m_szLastSearchField, szGuildID) == 0 ? TRUE : FALSE;
			break;

		case 1:			// 길마 이름 검색
			if(!szMasterID)
				return FALSE;

			return strcmp(m_szLastSearchField, szMasterID) == 0 ? TRUE : FALSE;
			break;

	}

	return FALSE;
}

BOOL AlefAdminGuild::InitGridData()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// 다 지워주고 새로 세팅한다.
	m_csGuildDataLV.DeleteAllItems();
	InitGrid();

	return TRUE;

	//CXTPPropertyGridItem* pCategory = NULL;
	//CXTPSetValueGridItem* pSubItem = NULL;
	//CXTPPropertyGridItems* pSubItemList = NULL;
	//INT32 lIndex = 0, lCount = 0;

	//pCategory = m_pGuildGrid->FindItem(_T("Basic"));
	//if(pCategory)
	//{
	//	pSubItemList = pCategory->GetChilds();
	//	if(pSubItemList)
	//	{
	//		lCount = pSubItemList->GetCount();
	//		for(lIndex = 0; lIndex < lCount; lIndex++)
	//			((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
	//	}
	//}

	//pCategory = m_pGuildGrid->FindItem(_T("Battle"));
	//if(pCategory)
	//{
	//	pSubItemList = pCategory->GetChilds();
	//	if(pSubItemList)
	//	{
	//		lCount = pSubItemList->GetCount();
	//		for(lIndex = 0; lIndex < lCount; lIndex++)
	//			((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
	//	}
	//}

	//return TRUE;
}

BOOL AlefAdminGuild::ShowGuildData(AgpdGuild* pcsGuild)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsGuild)
		return FALSE;

	AgpmGuild* ppmGuild = ( AgpmGuild* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmGuild" );

	INT32 lIndex = -1;
	CHAR szTmp[255];
	memset(szTmp, 0, sizeof(szTmp));
	
	// 위에서 부터 차례로 세팅해준다.
	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ID));
	if(lIndex >= 0)
	{
		sprintf(szTmp, "%s", pcsGuild->m_szID);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_MASTER_ID));
	if(lIndex >= 0)
	{
		sprintf(szTmp, "%s", pcsGuild->m_szMasterID);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(_T("TID"));
	if(lIndex >= 0)
	{
		sprintf(szTmp, "%d", pcsGuild->m_lTID);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_RANK));
	if(lIndex >= 0)
	{
		sprintf(szTmp, "%d", pcsGuild->m_lRank);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_CREATION_DATE));
	if(lIndex >= 0)
	{
		AlefAdminManager::Instance()->ConvertTimeStampToString(pcsGuild->m_lCreationDate, szTmp);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_PASSWORD));
	if(lIndex >= 0)
	{
		sprintf(szTmp, "%s", pcsGuild->m_szPassword);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_MEMBER_COUNT));
	if(lIndex >= 0)
	{
		sprintf(szTmp, "%d / %d", ppmGuild->GetOnlineMemberCount(pcsGuild), ppmGuild->GetMemberCount(pcsGuild));
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}



	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_STATUS));
	if(lIndex >= 0)
	{
		switch(pcsGuild->m_cStatus)
		{
			case AGPMGUILD_STATUS_NONE:
				strcpy(szTmp, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_STATUS_NONE));
				break;
			case AGPMGUILD_STATUS_BATTLE_READY:
				strcpy(szTmp, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_STATUS_READY));
				break;
			case AGPMGUILD_STATUS_BATTLE:
				strcpy(szTmp, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_STATUS_BATTLE));
				break;
		}
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_RECORD));
	if(lIndex >= 0)
	{
		sprintf(szTmp, "W:%d / D:%d / L:%d", pcsGuild->m_lWin, pcsGuild->m_lDraw, pcsGuild->m_lLose);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ENEMY_GUILD_ID));
	if(lIndex >= 0)
	{
		sprintf(szTmp, "%s", pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_ACCEPT_TIME));
	if(lIndex >= 0)
	{
		AlefAdminManager::Instance()->ConvertTimeStampToString(pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime, szTmp);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_START_TIME));
	if(lIndex >= 0)
	{
		AlefAdminManager::Instance()->ConvertTimeStampToString(pcsGuild->m_csCurrentBattleInfo.m_ulStartTime, szTmp);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_DURATION));
	if(lIndex >= 0)
	{
		if(pcsGuild->m_csCurrentBattleInfo.m_ulDuration == 0)
			strcpy(szTmp, _T(""));
		else
			sprintf(szTmp, "%d Min(s)", pcsGuild->m_csCurrentBattleInfo.m_ulDuration / 60);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_SCORE));
	if(lIndex >= 0)
	{
		if(pcsGuild->m_csCurrentBattleInfo.m_lMyScore == 0)
			strcpy(szTmp, _T(""));
		else
			sprintf(szTmp, "%d", pcsGuild->m_csCurrentBattleInfo.m_lMyScore);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_BATTLE_ENEMY_SCORE));
	if(lIndex >= 0)
	{
		if(pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore == 0)
			strcpy(szTmp, _T(""));
		else
			sprintf(szTmp, "%d", pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	// 공지사항도 세팅해준다.
	CString szNotice = pcsGuild->m_szNotice;
	szNotice.Replace(_T("___"), _T("\r\n"));
	SetDlgItemText(IDC_E_GUILD_NOTICE, (LPCTSTR)szNotice);

	return TRUE;

	//CXTPPropertyGridItem* pCategory = NULL;
	//CXTPSetValueGridItem* pSubItem = NULL;
	//CXTPPropertyGridItems* pSubItemList = NULL;
	//CString szValue;
	//CHAR szTmp[255];
	//
	//pCategory = m_pGuildGrid->FindItem(_T("Basic"));
	//if(pCategory)	// Basic Category
	//{
	//	pSubItemList = pCategory->GetChilds();
	//	if(pSubItemList)
	//	{
	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Guild Name"));
	//		if(pSubItem)
	//		{
	//			szValue = pcsGuild->m_szID;
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Master Name"));
	//		if(pSubItem)
	//		{
	//			szValue = pcsGuild->m_szMasterID;
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("TID"));
	//		if(pSubItem)
	//		{
	//			szValue.Format("%d", pcsGuild->m_lTID);
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Rank"));
	//		if(pSubItem)
	//		{
	//			szValue.Format("%d", pcsGuild->m_lRank);
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Create Date"));
	//		if(pSubItem)
	//		{
	//			//szValue.Format("%d", pcsGuild->m_lTID);
	//			//pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Password"));
	//		if(pSubItem)
	//		{
	//			szValue = pcsGuild->m_szPassword;
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Member"));
	//		if(pSubItem)
	//		{
	//			szValue.Format("%d / %d", AlefAdminManager::Instance()->GetAdminModule()->m_pcsAgpmGuild->GetOnlineMemberCount(pcsGuild),
	//									AlefAdminManager::Instance()->GetAdminModule()->m_pcsAgpmGuild->GetMemberCount(pcsGuild));
	//			pSubItem->SetValue(szValue);
	//		}
	//	}
	//}

	//pCategory = m_pGuildGrid->FindItem(_T("Battle"));
	//if(pCategory)	// Basic Category
	//{
	//	pSubItemList = pCategory->GetChilds();
	//	if(pSubItemList)
	//	{
	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Status"));
	//		if(pSubItem)
	//		{
	//			switch(pcsGuild->m_cStatus)
	//			{
	//				case AGPMGUILD_STATUS_NONE:			szValue = _T("Normal");			break;
	//				case AGPMGUILD_STATUS_BATTLE_READY:	szValue = _T("Battle Ready");	break;
	//				case AGPMGUILD_STATUS_BATTLE:		szValue = _T("Now Battle");		break;
	//			}
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Record(s)"));
	//		if(pSubItem)
	//		{
	//			szValue.Format("W:%d / D:%d / L:%d", pcsGuild->m_lWin, pcsGuild->m_lDraw, pcsGuild->m_lLose);
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Enemy Guild Name"));
	//		if(pSubItem)
	//		{
	//			szValue = pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID;
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Accept Time"));
	//		if(pSubItem)
	//		{
	//			if(pcsGuild->m_cStatus == AGPMGUILD_STATUS_NONE)
	//				szValue = _T("");
	//			else
	//			{
	//				memset(szTmp, 0, sizeof(szTmp));
	//				AlefAdminManager::Instance()->ConvertTimeStampToString(pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime, szTmp);
	//				szValue = szTmp;
	//			}

	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Start Time"));
	//		if(pSubItem)
	//		{
	//			if(pcsGuild->m_cStatus == AGPMGUILD_STATUS_NONE)
	//				szValue = _T("");
	//			else
	//			{
	//				memset(szTmp, 0, sizeof(szTmp));
	//				AlefAdminManager::Instance()->ConvertTimeStampToString(pcsGuild->m_csCurrentBattleInfo.m_ulStartTime, szTmp);
	//				szValue = szTmp;
	//			}

	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Duration"));
	//		if(pSubItem)
	//		{
	//			if(pcsGuild->m_cStatus == AGPMGUILD_STATUS_NONE)
	//				szValue = _T("");
	//			else
	//				szValue.Format("%d Min", pcsGuild->m_csCurrentBattleInfo.m_ulDuration / 60);

	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Score"));
	//		if(pSubItem)
	//		{
	//			if(pcsGuild->m_cStatus == AGPMGUILD_STATUS_NONE)
	//				szValue = _T("");
	//			else
	//				szValue.Format("%d", pcsGuild->m_csCurrentBattleInfo.m_lMyScore);

	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Enemy Score"));
	//		if(pSubItem)
	//		{
	//			if(pcsGuild->m_cStatus == AGPMGUILD_STATUS_NONE)
	//				szValue = _T("");
	//			else
	//				szValue.Format("%d", pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore);

	//			pSubItem->SetValue(szValue);
	//		}
	//	}
	//}

	//// 공지사항도 써준다.
	//SetDlgItemText(IDC_E_GUILD_NOTICE, pcsGuild->m_szNotice);

	//return TRUE;
}

BOOL AlefAdminGuild::ShowMemberData(AgpdGuild* pcsGuild, CHAR* szMemberID)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsGuild || !szMemberID)
		return FALSE;

	AgpmGuild* ppmGuild = ( AgpmGuild* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmGuild" );
	AgpdGuildMember* pcsMember = ppmGuild->GetMember(pcsGuild, szMemberID);
	if(!pcsMember)
		return FALSE;

	// 이미 있는 지 뒤져본다.
	INT32 lIndex = 0;
	BOOL bFound = FALSE;
	INT32 lCount = m_csMemberLV.GetItemCount();
	CString szValue = _T("");

	for(lIndex; lIndex < lCount; lIndex++)
	{
		szValue = m_csMemberLV.GetItemText(lIndex, 0);
		if(szValue.Compare(szMemberID) == 0)
		{
			bFound = TRUE;
			break;
		}
	}

	if(!bFound)		// 찾지 못했다면 Insert 해주고
		m_csMemberLV.InsertItem(lIndex, szMemberID);

	szValue.Format("%d", pcsMember->m_lLevel);
	m_csMemberLV.SetItemText(lIndex, 1, szValue);

	switch(pcsMember->m_lRank)
	{
		case AGPMGUILD_MEMBER_RANK_NORMAL:
			szValue = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_MEMBER);
			break;
		case AGPMGUILD_MEMBER_RANK_MASTER:
			szValue = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_MASTER);
			break;
		case AGPMGUILD_MEMBER_RANK_SUBMASTER:
			szValue = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_SUBMASTER);
			break;
		case AGPMGUILD_MEMBER_RANK_JOIN_REQUEST:
			szValue = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_JOIN_REQUEST);
			break;
		case AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST:
			szValue = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_LEAVE_REQUEST);
			break;
		default:
			szValue = _T("");
			break;
	}
	m_csMemberLV.SetItemText(lIndex, 2, szValue);

	// 얘도 한번씩 불러준다.
	UpdateMemberInfo(pcsGuild);

	return TRUE;
}

BOOL AlefAdminGuild::UpdateMemberInfo(AgpdGuild* pcsGuild)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsGuild)
		return FALSE;

	AgpmGuild* ppmGuild = ( AgpmGuild* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmGuild" );
	INT32 lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_MEMBER_COUNT));
	if(lIndex >= 0)
	{
		CHAR szTmp[255];
		sprintf(szTmp, "%d / %d", ppmGuild->GetOnlineMemberCount(pcsGuild),	ppmGuild->GetMemberCount(pcsGuild));
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	return TRUE;

	//CXTPPropertyGridItem* pCategory = NULL;
	//CXTPSetValueGridItem* pSubItem = NULL;
	//CXTPPropertyGridItems* pSubItemList = NULL;
	//CString szValue;
	//
	//pCategory = m_pGuildGrid->FindItem(_T("Basic"));
	//if(pCategory)	// Basic Category
	//{
	//	pSubItemList = pCategory->GetChilds();
	//	if(pSubItemList)
	//	{
	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Member"));
	//		if(pSubItem)
	//		{
	//			szValue.Format("%d / %d", AlefAdminManager::Instance()->GetAdminModule()->m_pcsAgpmGuild->GetOnlineMemberCount(pcsGuild),
	//									AlefAdminManager::Instance()->GetAdminModule()->m_pcsAgpmGuild->GetMemberCount(pcsGuild));
	//			pSubItem->SetValue(szValue);
	//		}
	//	}
	//}

	//return TRUE;
}

BOOL AlefAdminGuild::UpdatePassword(CHAR* szPassword)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szPassword)
		return FALSE;

	INT32 lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_PASSWORD));
	if(lIndex >= 0)
	{
		CHAR szTmp[255];
		sprintf(szTmp, "%s", szPassword);
		m_csGuildDataLV.SetItemText(lIndex, 1, szTmp);
	}

	return TRUE;

	//CXTPPropertyGridItem* pCategory = NULL;
	//CXTPSetValueGridItem* pSubItem = NULL;
	//CXTPPropertyGridItems* pSubItemList = NULL;
	//CString szValue;
	//
	//pCategory = m_pGuildGrid->FindItem(_T("Basic"));
	//if(pCategory)	// Basic Category
	//{
	//	pSubItemList = pCategory->GetChilds();
	//	if(pSubItemList)
	//	{
	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Password"));
	//		if(pSubItem)
	//		{
	//			szValue = szPassword;
	//			pSubItem->SetValue(szValue);
	//		}
	//	}
	//}

	//return TRUE;
}

INT32 AlefAdminGuild::GetCurrentGuildID(CString& szGuildID)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	INT32 lIndex = GetGuildDataIndex(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ID));
	if(lIndex >= 0)
	{
		szGuildID = m_csGuildDataLV.GetItemText(lIndex, 1);
		return szGuildID.GetLength();
	}

    return 0;

	//CXTPPropertyGridItem* pCategory = NULL;
	//CXTPSetValueGridItem* pSubItem = NULL;
	//CXTPPropertyGridItems* pSubItemList = NULL;
	//
	//pCategory = m_pGuildGrid->FindItem(_T("Basic"));
	//if(pCategory)	// Basic Category
	//{
	//	pSubItemList = pCategory->GetChilds();
	//	if(pSubItemList)
	//	{
	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Guild Name"));
	//		if(pSubItem)
	//		{
	//			szGuildID = pSubItem->GetValue();
	//		}
	//	}
	//}

	//return szGuildID.GetLength();
}

INT32 AlefAdminGuild::GetGuildDataIndex(const CHAR* szField)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szField)
		return -1;

	CString szTmp = _T("");
	INT32 lIndex = 0;
	for(lIndex; lIndex < m_csGuildDataLV.GetItemCount(); lIndex++)
	{
		szTmp = m_csGuildDataLV.GetItemText(lIndex, 0);
		if(szTmp.Compare(szField) == 0)
			return lIndex;
	}

	return -1;
}

BOOL AlefAdminGuild::CBGuildCreate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = static_cast<CHAR*>(pData);
	AlefAdminGuild* pThis = static_cast<AlefAdminGuild*>(pClass);
	CHAR* szMasterID = static_cast<CHAR*>(pCustData);

	// 검색한 길드인지 확인
	if(!pThis->IsSearchGuild(szGuildID, szMasterID))
		return TRUE;

	// 길드를 얻구
	AgpmGuild* ppmGuild = ( AgpmGuild* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmGuild" );
	AgpdGuild* pcsGuild = ppmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	strncpy(pThis->m_szSearchedGuildID, pcsGuild->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	memset(pThis->m_szSelectedMemberID, 0, sizeof(pThis->m_szSelectedMemberID));
	pThis->m_lSelectedMemberRank = 0;

	// 그려준다~
	pThis->ShowGuildData(pcsGuild);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AlefAdminGuild::CBGuildJoin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = static_cast<CHAR*>(pData);
	AlefAdminGuild* pThis = static_cast<AlefAdminGuild*>(pClass);
	CHAR* szMemberID = static_cast<CHAR*>(pCustData);

	// 검색한 길드인지 확인
	if(!pThis->IsSearchGuild(szGuildID))
		return TRUE;

	// 길드를 얻구
	AgpmGuild* ppmGuild = ( AgpmGuild* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmGuild" );
	AgpdGuild* pcsGuild = ppmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	// 그려준다.
	pThis->ShowMemberData(pcsGuild, szMemberID);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AlefAdminGuild::CBReceiveGuildOperation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass)
		return FALSE;

	stAgpdAdminCharEdit* pstGuild = static_cast<stAgpdAdminCharEdit*>(pData);
	AlefAdminGuild* pThis = static_cast<AlefAdminGuild*>(pClass);

	switch(pstGuild->m_lCID)
	{
		case AGPMADMIN_GUILD_SEARCH_PASSWORD:
			if(pThis->IsSearchGuild(pstGuild->m_szCharName))
				pThis->UpdatePassword(pstGuild->m_szNewValueChar);

			break;

		case AGPMADMIN_GUILD_TOTAL_INFO:
			pThis->m_lTotalGuildCount = pstGuild->m_lNewValueINT;
			pThis->m_lTotalMemberCount = (INT32)pstGuild->m_llNewValueINT64;

			pThis->SetDlgItemInt(IDC_S_GUILD_COUNT, pThis->m_lTotalGuildCount);
			pThis->SetDlgItemInt(IDC_S_GUILD_MEMBER_COUNT, pThis->m_lTotalMemberCount);
			break;

		case AGPMADMIN_GUILD_RESULT:
		{
			CString szMsg = _T("");

			switch(pstGuild->m_lNewValueINT)
			{
				case AGPMADMIN_GUILD_RESULT_SUCCESS_LEAVE_MEMBER:
					szMsg = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_LEAVE_MEMBER_SUCCESS);

					// 2006.09.06. steeple
					// 새로 검색해준다.
					pThis->m_csSearchType.SetCurSel(pThis->m_lLastSearchType);
					pThis->SetDlgItemText(IDC_E_GUILD_SEARCH_FIELD, pThis->m_szLastSearchField);

					pThis->OnBnClickedBGuildSearch();
					break;

				case AGPMADMIN_GUILD_RESULT_SUCCESS_DESTROY_GUILD:
					szMsg = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_LEAVE_MEMBER_FAILED);
					break;

				case AGPMADMIN_GUILD_RESULT_SUCCESS_CHANGE_MASTER:
					szMsg = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_DESTROY_SUCCESS);
					break;

				case AGPMADMIN_GUILD_RESULT_SUCCESS_CHANGE_NOTICE:
					szMsg = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_DESTROY_FAILED);
					break;

				case AGPMADMIN_GUILD_RESULT_FAILED_LEAVE_MEMBER:
					szMsg = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_CHANGE_MASTER_SUCCESS);
					break;

				case AGPMADMIN_GUILD_RESULT_FAILED_DESTROY_GUILD:
					szMsg = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_CHANGE_MASTER_FAILED);
					break;

				case AGPMADMIN_GUILD_RESULT_FAILED_CHANGE_MASTER:
					szMsg = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_CHANGE_NOTICE_SUCCESS);
					break;

				case AGPMADMIN_GUILD_RESULT_FAILED_CHANGE_NOTICE:
					szMsg = AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_CHANGE_NOTICE_FAILED);
					break;
			}

			if(szMsg.IsEmpty() == FALSE)
				AlefAdminManager::Instance()->m_pMainDlg->ProcessAdminMessage((LPCTSTR)szMsg);

			break;
		}
	}

	return TRUE;
}

void AlefAdminGuild::OnBnClickedBGuildSearch()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_lLastSearchType = m_csSearchType.GetCurSel();
	if(m_lLastSearchType < 0)
		return;

	memset(m_szLastSearchField, 0, sizeof(m_szLastSearchField));
	GetDlgItemText(IDC_E_GUILD_SEARCH_FIELD, m_szLastSearchField, 254);

	// 기존에 검색해 놓은 길드아이디 저장
	CString szCurrentGuildID = _T("");
	GetCurrentGuildID(szCurrentGuildID);

	// 마지막 검색한 놈은 비우기.
	memset(m_szSearchedGuildID, 0, sizeof(m_szSearchedGuildID));
	memset(m_szSelectedMemberID, 0, sizeof(m_szSelectedMemberID));
	m_lSelectedMemberRank = 0;

	// 화면을 비우고
	InitGridData();
	m_csMemberLV.DeleteAllItems();
	SetDlgItemText(IDC_E_GUILD_NOTICE, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_THIS_PLACE_NOTICE));

	// 자기 길드를 한번 구해본다.
	AgpmGuild* ppmGuild = ( AgpmGuild* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmGuild" );
	AgcmGuild* pcmGuild = ( AgcmGuild* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgcmGuild" );
	CHAR* szGuildID = pcmGuild->GetSelfGuildID();
	if(szGuildID && strlen(szGuildID) > 0 && IsSearchGuild(szGuildID))
	{
		AgpdGuild* pcsGuild = ppmGuild->GetGuildLock(szGuildID);
		if(pcsGuild)
		{
			strncpy(m_szSearchedGuildID, pcsGuild->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);

			// 현재 길드가 있는 것임.
			// 그럼 검색할 필요없이 그냥 뿌리면 되는 것임.
			ShowGuildData(pcsGuild);

			INT32 lIndex = 0;
			for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
									ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
			{
				ShowMemberData(pcsGuild, (*ppcsMember)->m_szID);
			}

			// Password 요청해야 한다.
			stAgpdAdminCharEdit stGuild;
			memset(&stGuild, 0, sizeof(stGuild));

			stGuild.m_lCID = AGPMADMIN_GUILD_SEARCH_PASSWORD;
			strcpy(stGuild.m_szCharName, pcsGuild->m_szID);
			AlefAdminManager::Instance()->GetAdminData()->SendGuildOperation(&stGuild);

			pcsGuild->m_Mutex.Release();

			return;
		}
	}

	// 위에서 return 안했으면 새로 검색이다.
	// 기존에 검색했던 Guild Data 는 싹 지우고 새로운 걸 요청하자.
	if(szCurrentGuildID.GetLength() > 0)
	{
		// 지운다.
		// 자기 길드가 아닐때만 지우자.
		if(strcmp(szCurrentGuildID, szGuildID) != 0)
			ppmGuild->DestroyGuild((CHAR*)(LPCTSTR)szCurrentGuildID);
	}

	// 검색 날린다.
	stAgpdAdminCharEdit stGuild;
	memset(&stGuild, 0, sizeof(stGuild));

	stGuild.m_lCID = AGPMADMIN_GUILD_SEARCH;
	
	stGuild.m_lEditField = (INT16)m_lLastSearchType;
	strncpy(stGuild.m_szCharName, m_szLastSearchField, AGPACHARACTER_MAX_ID_STRING);

	AlefAdminManager::Instance()->GetAdminData()->SendGuildOperation(&stGuild);
}

void AlefAdminGuild::OnBnClickedBGuildInfoRefresh()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	stAgpdAdminCharEdit stGuild;
	memset(&stGuild, 0, sizeof(stGuild));

	stGuild.m_lCID = AGPMADMIN_GUILD_TOTAL_INFO;

	AlefAdminManager::Instance()->GetAdminData()->SendGuildOperation(&stGuild);
}

void AlefAdminGuild::OnNMClickLvGuild(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CString szCaption = m_csGuildDataLV.GetItemText(nlv->iItem, 0);
	CString szDesc = m_csGuildDataLV.GetItemText(nlv->iItem, 1);

	if(szCaption.GetLength() == 0)
		return;

	CString szMsg = _T("");
	szMsg.Format("%s : %s", (LPCTSTR)szCaption, (LPCTSTR)szDesc);

	AlefAdminManager::Instance()->m_pMainDlg->ProcessActionMessage((LPCTSTR)szMsg);

	*pResult = 0;
}

void AlefAdminGuild::OnNMClickLvMember(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CString szCharName = m_csMemberLV.GetItemText(nlv->iItem, 0);
	CString szLevel = m_csMemberLV.GetItemText(nlv->iItem, 1);
	CString szRank = m_csMemberLV.GetItemText(nlv->iItem, 2);

	if(szCharName.GetLength() == 0)
		return;

	strncpy(m_szSelectedMemberID, (LPCTSTR)szCharName, AGPACHARACTER_MAX_ID_STRING);
	if(szRank.Compare(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_MASTER)) == 0)
		m_lSelectedMemberRank = AGPMGUILD_MEMBER_RANK_MASTER;
	else if(szRank.Compare(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_MEMBER)) == 0)
		m_lSelectedMemberRank = AGPMGUILD_MEMBER_RANK_NORMAL;
	else if(szRank.Compare(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_JOIN_REQUEST)) == 0)
		m_lSelectedMemberRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;
	else if(szRank.Compare(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_LEAVE_REQUEST)) == 0)
		m_lSelectedMemberRank = AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST;

	CString szMsg = _T("");
	szMsg.Format("Member : %s, Level : %s, Rank : %s", (LPCTSTR)szCharName, (LPCTSTR)szLevel, (LPCTSTR)szRank);

	AlefAdminManager::Instance()->m_pMainDlg->ProcessActionMessage((LPCTSTR)szMsg);

	*pResult = 0;
}

void AlefAdminGuild::OnBnClickedBGuildDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(_mbstrlen(m_szSearchedGuildID) == 0)
		return;

	// 권한체크를 한 번 한 후
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	// 예의상 한 번 물어봐야지
	CString szMsg = _T("");
	szMsg.Format(IDS_GUILD_DESTROY_CONFIRM, m_szSearchedGuildID);
	if(::AfxMessageBox(szMsg, MB_YESNO) == IDNO)
		return;

	stAgpdAdminCharEdit stGuild;
	memset(&stGuild, 0, sizeof(stGuild));

	stGuild.m_lCID = AGPMADMIN_GUILD_DESTROY;
	strncpy(stGuild.m_szNewValueChar, m_szSearchedGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);

	AlefAdminManager::Instance()->GetAdminData()->SendGuildOperation(&stGuild);
}

void AlefAdminGuild::OnBnClickedBMemberForcedLeave()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(_mbstrlen(m_szSelectedMemberID) == 0)
		return;

	// 권한체크를 한 번 한 후
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	CString szMsg = _T("");
	if(m_lSelectedMemberRank != AGPMGUILD_MEMBER_RANK_NORMAL)
	{
		szMsg.LoadString(IDS_GUILD_LEAVE_ONLY_MEMBER);
		::AfxMessageBox(szMsg);
		return;
	}

	szMsg.Format(IDS_GUILD_MEMBER_FORCED_LEAVE, m_szSelectedMemberID, m_szSearchedGuildID);
	if(::AfxMessageBox(szMsg, MB_YESNO) == IDNO)
		return;

	SendForcedMemberLeave(m_szSearchedGuildID, m_szSelectedMemberID);
}

// 길드 이름은 NULL 이어도 가능하다.
BOOL AlefAdminGuild::SendForcedMemberLeave(const CHAR* szGuildID, const CHAR* szMemberID)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szMemberID)
		return FALSE;

	if(strlen(szMemberID) == 0)
		return FALSE;

	stAgpdAdminCharEdit stGuild;
	memset(&stGuild, 0, sizeof(stGuild));

	stGuild.m_lCID = AGPMADMIN_GUILD_LEAVE_MEMBER;
	strncpy(stGuild.m_szCharName, szMemberID, AGPACHARACTER_MAX_ID_STRING);
	
	if(szGuildID)
		strncpy(stGuild.m_szNewValueChar, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);

	AlefAdminManager::Instance()->GetAdminData()->SendGuildOperation(&stGuild);
	
	return TRUE;
}

void AlefAdminGuild::OnBnClickedBChangeMaster()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(strlen(m_szSearchedGuildID) == 0 || strlen(m_szSelectedMemberID) == 0)
		return;

	if(m_lSelectedMemberRank != AGPMGUILD_MEMBER_RANK_NORMAL)
		return;

	// 권한체크를 한 번 한 후
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	CString szMsg = _T("");
	szMsg.Format(IDS_GUILD_CHANGE_MASTER_CONFIRM);
	if(::AfxMessageBox(szMsg, MB_YESNO) == IDNO)
		return;

	szMsg.Format(IDS_GUILD_CHANGE_MASTER, m_szSearchedGuildID, m_szSelectedMemberID);
	if(::AfxMessageBox(szMsg, MB_YESNO) == IDNO)
		return;

	stAgpdAdminCharEdit stGuild;
	memset(&stGuild, 0, sizeof(stGuild));

	stGuild.m_lCID = AGPMADMIN_GUILD_CHANGE_MASTER;
	strncpy(stGuild.m_szCharName, m_szSelectedMemberID, AGPACHARACTER_MAX_ID_STRING);
	strncpy(stGuild.m_szNewValueChar, m_szSearchedGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);

	AlefAdminManager::Instance()->GetAdminData()->SendGuildOperation(&stGuild);
}

void AlefAdminGuild::OnBnClickedBChangeNotice()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(strlen(m_szSearchedGuildID) == 0)
		return;

	// 권한체크를 한 번 한 후
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	CString szMsg = _T("");
	szMsg.Format(IDS_GUILD_CHANGE_NOTICE_CONFIRM);
	if(::AfxMessageBox(szMsg, MB_YESNO) == IDNO)
		return;

	stAgpdAdminCharEdit stGuild;
	memset(&stGuild, 0, sizeof(stGuild));

	stGuild.m_lCID = AGPMADMIN_GUILD_CHANGE_NOTICE;
	strncpy(stGuild.m_szCharName, m_szSearchedGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);

	CString szNotice = _T("");
	GetDlgItemText(IDC_E_GUILD_NOTICE, szNotice);
	szNotice.Replace(_T("\r\n"), _T("___"));
	if(szNotice.GetLength() >= AGPMGUILD_MAX_NOTICE_LENGTH)
	{
		szMsg.LoadString(IDS_GUILD_NOTICE_EXCEED_MAX_LENGTH);
		::AfxMessageBox(szMsg);
		return;
	}

	strncpy(stGuild.m_szNewValueChar, (LPCTSTR)szNotice, AGPMGUILD_MAX_NOTICE_LENGTH);
	stGuild.m_lNewValueCharLength = strlen(stGuild.m_szNewValueChar);

	AlefAdminManager::Instance()->GetAdminData()->SendGuildOperation(&stGuild);
}
