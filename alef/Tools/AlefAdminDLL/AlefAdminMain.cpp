// AlefAdminMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminMain.h"
#include ".\alefadminmain.h"

#include "AlefAdminManager.h"
#include "AlefAdminAbout.h"

#include "AgcmTargeting.h"
#include "AgcmChatting2.h"


#define MAX_EDIT_MESSAGE_LENGTH			1024 * 1024 * 3			// 3 Mb. 


// AlefAdminMain 대화 상자입니다.

//IMPLEMENT_DYNAMIC(AlefAdminMain, CDialog)
IMPLEMENT_DYNAMIC(AlefAdminMain, CXTResizeDialog)
AlefAdminMain::AlefAdminMain(CWnd* pParent /*=NULL*/)
	//: CDialog(AlefAdminMain::IDD, pParent)
	: CXTResizeDialog(AlefAdminMain::IDD, pParent)
{
	m_pCharView = NULL;
	m_pItemView = NULL;
	m_pSkillView = NULL;
	m_pNoticeView = NULL;
	m_pGuildView = NULL;
	m_pTitleView = NULL;

	m_pfCBSearch = NULL;
	memset(&m_stLastSearch, 0, sizeof(m_stLastSearch));

	memset(m_szCharName, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));

	m_bSOS = FALSE;

    //xtAfxData.bXPMode = TRUE;

    //// Enable/Disable Menu Shadows
    //xtAfxData.bMenuShadows = TRUE;
}

AlefAdminMain::~AlefAdminMain()
{
	// Destroy Window 가 밖에서 불리면서
	// m_pCharView, m_pItemView 가 각각 PostNcDestroy 가 불리면서 자동 delete 된다.
	// 고로 따로 작업안해주어도 된다.
}

void AlefAdminMain::DoDataExchange(CDataExchange* pDX)
{
	//CDialog::DoDataExchange(pDX);
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_csMainTab);
	DDX_Control(pDX, IDC_B_SEARCH, m_csSearchBtn);
	DDX_Control(pDX, IDC_CB_SEARCH_TYPE, m_csSearchTypeCB);
	DDX_Control(pDX, IDC_LC_SEARCH_RESULT, m_csSearchResult);
	DDX_Control(pDX, IDC_TAB_MESSAGE, m_csMessageTab);
	DDX_Control(pDX, IDC_E_SEARCH_FIELD, m_csSearchEdit);
	DDX_Control(pDX, IDC_CH_SOS, m_csSOSBtn);
}


//BEGIN_MESSAGE_MAP(AlefAdminMain, CDialog)
BEGIN_MESSAGE_MAP(AlefAdminMain, CXTResizeDialog)
	ON_BN_CLICKED(IDC_B_SEARCH, OnBnClickedBSearch)
	ON_NOTIFY(NM_DBLCLK, IDC_LC_SEARCH_RESULT, OnNMDblclkLcSearchResult)
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_OPTION, OnOption)
	ON_COMMAND(IDM_ABOUT, OnAbout)
	ON_BN_CLICKED(IDC_CH_SOS, OnBnClickedChSos)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// AlefAdminMain 메시지 처리기입니다.

BOOL AlefAdminMain::SetCBSearch(ADMIN_CB pfCBSearch)
{
	m_pfCBSearch = pfCBSearch;
	return TRUE;
}

BOOL AlefAdminMain::Create()
{
	return TRUE;
}

BOOL AlefAdminMain::OpenDlg()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(::IsWindow(m_hWnd))
		SetFocus();
	else
	{
		InitCommonControls();

		// 모달리스 다이얼로그 생성
		//CDialog::Create(IDD, m_pParentWnd);
		CXTResizeDialog::Create(IDD, m_pParentWnd);
	}

	ShowCursor(TRUE);
	ShowWindow(SW_SHOW);

	return TRUE;
}

BOOL AlefAdminMain::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	//CDialog::OnInitDialog();
	CXTResizeDialog::OnInitDialog();

	// Layered Window
	//SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	// Make this window 70% alpha
	//SetLayeredWindowAttributes(0, (255 * 70) / 100, LWA_ALPHA);

	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;
	m_csSearchBtn.SetXButtonStyle(dwStyle);

	InitSearchTypeCB();
	InitSearchResultList();

	InitMainTab();
	CreateViews();
	AddTabViews();

	InitMessageTab();
	CreateMessageViews();
	AddMessageTabViews();

	m_csSearchEdit.SetFocus();

	SetResize(IDC_CB_SEARCH_TYPE,	SZ_TOP_LEFT,	SZ_TOP_LEFT);
	SetResize(IDC_E_SEARCH_FIELD,	SZ_TOP_LEFT,	SZ_TOP_LEFT);
	SetResize(IDC_B_SEARCH,			SZ_TOP_LEFT,	SZ_TOP_LEFT);
	SetResize(IDC_CH_SOS,			SZ_TOP_LEFT,	SZ_TOP_LEFT);
	SetResize(IDC_S_CURRENT_USER,	SZ_TOP_LEFT,	SZ_TOP_LEFT);
	SetResize(IDC_LC_SEARCH_RESULT,	SZ_TOP_LEFT,	SZ_TOP_RIGHT);
	SetResize(IDC_TAB_MAIN,			SZ_TOP_LEFT,	SZ_BOTTOM_RIGHT);
	SetResize(IDC_TAB_MESSAGE,		SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);

	// Timer 발동
	SetTimer(0, 500, NULL);	// 0.5 초에 한번씩

	// Callback Setting
	AgcmTargeting* pcmTargeting = ( AgcmTargeting* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgcmTargeting" );
	pcmTargeting->SetCallbackLButtonDownPickSomething(CBOnLButtonDownPicking, this);

	AgcmChatting2* pcmChatting = ( AgcmChatting2* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgcmChatting2" );
	pcmChatting->SetCallbackInputMessage(CBOnRecvChatting, this);

	// Default Own Skill List 를 읽어놓는다.
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmEventSkillMaster" );
	ppmEventSkillMaster->StreamReadDefaultSkill("Ini/Skill_DefaultOwn.txt", TRUE);

	return FALSE;
}

BOOL AlefAdminMain::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!AlefAdminManager::Instance()->IsInitialized())
		return FALSE;

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F7)
	{
		OnClose();
		return TRUE;
	}

	//if(CDialog::IsDialogMessage(pMsg))
	if(CXTResizeDialog::IsDialogMessage(pMsg))
		return TRUE;

	//return CDialog::PreTranslateMessage(pMsg);
	return CXTResizeDialog::PreTranslateMessage(pMsg);
}

BOOL AlefAdminMain::IsDialogMessage(LPMSG lpMsg)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Main Dialog 확인
	if(IsWindow(m_hWnd) && IsWindowVisible() && PreTranslateMessage(lpMsg))
		return TRUE;

	// Main Dialog 확인
	//if(IsWindow(m_hWnd) && IsWindowVisible() && CDialog::IsDialogMessage(lpMsg))
	//	return TRUE;

	// Tab Control 인지 확인
	//if(IsWindow(m_csMainTab.m_hWnd) && m_csMainTab.IsWindowVisible() && m_csMainTab.IsDialogMessage(lpMsg))
	//	return TRUE;

	// 각 View 의 메세지인지 확인한다.
	//if(m_pCharView && ::IsWindow(m_pCharView->m_hWnd) && m_pCharView->IsWindowVisible() && m_pCharView->PreTranslateMessage(lpMsg))
	//	return TRUE;

	//if(m_pItemView && ::IsWindow(m_pItemView->m_hWnd) && m_pItemView->IsWindowVisible() && m_pItemView->IsDialogMessage(lpMsg))
	//	return TRUE;

	return FALSE;
}

BOOL AlefAdminMain::InitSearchTypeCB()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSearchTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SEARCH_CHARNAME));
	m_csSearchTypeCB.AddString(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SEARCH_ACCNAME));
	m_csSearchTypeCB.SetCurSel(0);

	return TRUE;
}

BOOL AlefAdminMain::InitSearchResultList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSearchResult.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SEARCH_CHARNAME), LVCFMT_LEFT, 100);
	m_csSearchResult.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SEARCH_ACCNAME), LVCFMT_LEFT, 100);
	m_csSearchResult.InsertColumn(2, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LEVEL), LVCFMT_LEFT, 50);
	m_csSearchResult.InsertColumn(3, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RACE), LVCFMT_LEFT, 60);
	m_csSearchResult.InsertColumn(4, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CLASS), LVCFMT_LEFT, 60);
	m_csSearchResult.InsertColumn(5, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LOGIN_STATUS), LVCFMT_LEFT, 100);
	m_csSearchResult.InsertColumn(6, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ID), LVCFMT_LEFT, 100);
	m_csSearchResult.InsertColumn(7, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SERVER), LVCFMT_LEFT, 100);

	HWND hWndHeader = m_csSearchResult.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeader.SubclassWindow(hWndHeader);

	m_csSearchResult.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);

	return TRUE;
}

BOOL AlefAdminMain::InitMainTab()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csMainTab.SendInitialUpdate(TRUE);
	m_csMainTab.Invalidate(FALSE);

	m_csMainTab.ModifyStyle(0L, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	m_csMainTab.m_bXPBorder = true;

	m_csMainTab.SetFont(GetFont());

	//XTThemeManager()->SetTheme(xtThemeOfficeXP);

	return TRUE;
}

BOOL AlefAdminMain::CreateViews()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CRect rectDummy(0, 0, 0, 0);

	// Character View
	CCreateContext cc;
	ZeroMemory(&cc, sizeof(cc));
	m_pCharView = (AlefAdminCharacter*)RUNTIME_CLASS(AlefAdminCharacter)->CreateObject();
	if(!m_pCharView || !((CWnd *)m_pCharView)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csMainTab, IDD_CHARACTER, &cc))
	{
		//TRACE0("Failed to create Character View.\n");
		return FALSE;
	}

	// Item View
	CCreateContext cc2;
	ZeroMemory(&cc2, sizeof(cc2));
	m_pItemView = (AlefAdminItem*)RUNTIME_CLASS(AlefAdminItem)->CreateObject();
	if(!m_pItemView || !((CWnd *)m_pItemView)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csMainTab, IDD_ITEM, &cc2))
	{
		//TRACE0("Failed to create Item View.\n");
		return FALSE;
	}

	// Skill View
	CCreateContext cc3;
	ZeroMemory(&cc3, sizeof(cc3));
	m_pSkillView = (AlefAdminSkill*)RUNTIME_CLASS(AlefAdminSkill)->CreateObject();
	if(!m_pSkillView || !((CWnd *)m_pSkillView)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csMainTab, IDD_SKILL, &cc3))
	{
		//TRACE0("Failed to create Skill View.\n");
		return FALSE;
	}

	// Notice View
	CCreateContext cc4;
	ZeroMemory(&cc4, sizeof(cc4));
	m_pNoticeView = (AlefAdminNotice*)RUNTIME_CLASS(AlefAdminNotice)->CreateObject();
	if(!m_pNoticeView || !((CWnd *)m_pNoticeView)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csMainTab, IDD_NOTICE, &cc4))
	{
		return FALSE;
	}

	// Guild View
	CCreateContext cc5;
	ZeroMemory(&cc5, sizeof(cc5));
	m_pGuildView = (AlefAdminGuild*)RUNTIME_CLASS(AlefAdminGuild)->CreateObject();
	if(!m_pGuildView || !((CWnd *)m_pGuildView)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csMainTab, IDD_GUILD, &cc5))
	{
		return FALSE;
	}

	CCreateContext cc6;
	ZeroMemory( &cc6 , sizeof(cc6) );
	m_pUserView	=	(AlefAdminUser*)RUNTIME_CLASS( AlefAdminUser )->CreateObject();
	if(!m_pUserView || !((CWnd*)m_pUserView)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csMainTab, IDD_USER , &cc6))
	{
		return FALSE;
	}

	CCreateContext cc7;
	ZeroMemory( &cc7 , sizeof(cc7) );
	m_pTitleView	=	(AlefAdminTitle*)RUNTIME_CLASS( AlefAdminTitle )->CreateObject();
	if(!m_pTitleView || !((CWnd*)m_pTitleView)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csMainTab, IDD_USER , &cc7))
	{
		return FALSE;
	}

	m_pCharView->OnInitialUpdate();
	m_pItemView->OnInitialUpdate();
	m_pSkillView->OnInitialUpdate();
	m_pNoticeView->OnInitialUpdate();
	m_pGuildView->OnInitialUpdate();
	m_pUserView->OnInitialUpdate();
	m_pTitleView->OnInitialUpdate();

	return TRUE;
}

BOOL AlefAdminMain::AddTabViews()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!m_csMainTab.AddView(_T("Character"), m_pCharView, 0, 0))
	{
		//TRACE0("Failed to create Character View.\n");
		return FALSE;
	}

	if(!m_csMainTab.AddView(_T("Item"), m_pItemView, 1, 1))
	{
		//TRACE0("Failed to create Item View.\n");
		return FALSE;
	}

	if(!m_csMainTab.AddView(_T("Skill"), m_pSkillView, 2, 2))
	{
		//TRACE0("Failed to create Skill View.\n");
		return FALSE;
	}

	if(!m_csMainTab.AddView(_T("Notice"), m_pNoticeView, 3, 3))
	{
		return FALSE;
	}

	if(!m_csMainTab.AddView(_T("Guild"), m_pGuildView, 4, 4))
	{
		return FALSE;
	}

	if(!m_csMainTab.AddView(_T("User"), m_pUserView, 5, 5))
	{
		return FALSE;
	}

	if(!m_csMainTab.AddView(_T("Title"), m_pTitleView, 6, 6))
	{
		return FALSE;
	}

	m_csMainTab.SetActiveView(0);

	return TRUE;
}

BOOL AlefAdminMain::InitMessageTab()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csMessageTab.SendInitialUpdate(TRUE);
	m_csMessageTab.Invalidate(FALSE);

	m_csMessageTab.ModifyStyle(0L, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	m_csMessageTab.ModifyTabStyle(0L, TCS_BOTTOM);

	m_csMessageTab.m_bXPBorder = true;

	m_csMessageTab.SetFont(GetFont());

	return TRUE;
}

BOOL AlefAdminMain::CreateMessageViews()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CRect rectDummy(0, 0, 0, 0);

	DWORD dwExStyle = WS_EX_WINDOWEDGE;
	DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE;

	// Chat
	if(!m_csMessageChat.Create(dwStyle, rectDummy, &m_csMessageTab, IDC_EDIT_MESSAGE_CHAT))
	{
		return -1;
	}
	m_csMessageChat.ModifyStyleEx(0L, dwExStyle);
	m_csMessageChat.SetFont(GetFont());

	// System
	if(!m_csMessageSystem.Create(dwStyle, rectDummy, &m_csMessageTab, IDC_EDIT_MESSAGE_SYSTEM))
	{
		return -1;
	}
	m_csMessageSystem.ModifyStyleEx(0L, dwExStyle);
	m_csMessageSystem.SetFont(GetFont());

	// Admin
	if(!m_csMessageAdmin.Create(dwStyle, rectDummy, &m_csMessageTab, IDC_EDIT_MESSAGE_ADMIN))
	{
		return -1;
	}
	m_csMessageAdmin.ModifyStyleEx(0L, dwExStyle);
	m_csMessageAdmin.SetFont(GetFont());

	// Action
	if(!m_csMessageAction.Create(dwStyle, rectDummy, &m_csMessageTab, IDC_EDIT_MESSAGE_ACTION))
	{
		return -1;
	}
	m_csMessageAction.ModifyStyleEx(0L, dwExStyle);
	m_csMessageAction.SetFont(GetFont());

	return TRUE;
}

BOOL AlefAdminMain::AddMessageTabViews()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!m_csMessageTab.AddControl(_T("Chat"), &m_csMessageChat, 0, 0))
	{
		return FALSE;
	}

	if(!m_csMessageTab.AddControl(_T("System"), &m_csMessageSystem, 1, 1))
	{
		return FALSE;
	}

	if(!m_csMessageTab.AddControl(_T("Admin"), &m_csMessageAdmin, 2, 2))
	{
		return FALSE;
	}

	if(!m_csMessageTab.AddControl(_T("Action"), &m_csMessageAction, 3, 3))
	{
		return FALSE;
	}

	m_csMessageTab.SetActiveView(2);	// Admin Message 를 처음 보여주자.

	return TRUE;
}

BOOL AlefAdminMain::SearchCharacter(LPCTSTR szCharName)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szCharName || strlen(szCharName) <= 0)
		return FALSE;

	ZeroMemory(&m_stLastSearch, sizeof(m_stLastSearch));
	strcpy(m_stLastSearch.m_szSearchName, szCharName);

	// Main Dialog 에 CharName 을 세팅해준다.
	SetCharName(m_stLastSearch.m_szSearchName);

	ResetAllCharData();

	AlefAdminManager::Instance()->GetAdminData()->SendSearchCharacter(&m_stLastSearch);

	return TRUE;
}

BOOL AlefAdminMain::OnReceiveSearchResult(stAgpdAdminSearchResult* pstResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstResult)
		return FALSE;

	// 권한 체크
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		return FALSE;

	// 검색을 해서 얻은 결과인지 확인한다.
	BOOL bValid = FALSE;
	if(m_stLastSearch.m_iField == 0 && strcmp(m_stLastSearch.m_szSearchName, pstResult->m_szCharName) == 0)
		bValid = TRUE;
	else if(m_stLastSearch.m_iField == 1 && strcmp(m_stLastSearch.m_szSearchName, pstResult->m_szAccName) == 0)
		bValid = TRUE;

	if(!bValid)
		return FALSE;

	// 현재 갯수를 구하고
	int iIndex = m_csSearchResult.GetItemCount();
	BOOL bFound = FALSE;

	CHAR szTmp[255];
	memset(szTmp, 0, sizeof(CHAR) * 255);

	// 이미 있는지 찾는다.
	if(iIndex > 0)
	{
		for(int i = 0; i < iIndex; i++)
		{
			m_csSearchResult.GetItemText(i, 0, szTmp, 254);
			if(strcmp(szTmp, pstResult->m_szCharName) == 0)
			{
				bFound = TRUE;
				iIndex = i;
				break;
			}
		}
	}

	// 이미 있다면 업뎃
	if(bFound)
	{
		// 레벨
		sprintf(szTmp, "%d", pstResult->m_lLevel);
		m_csSearchResult.SetItemText(iIndex, 2, szTmp);

		// 접속상태
		if(pstResult->m_lStatus == 1)
			m_csSearchResult.SetItemText(iIndex, 5, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ONLINE));
		else
			m_csSearchResult.SetItemText(iIndex, 5, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_OFFLINE));

		// 길드
		sprintf(szTmp, "%s", pstResult->m_szGuildID);
		m_csSearchResult.SetItemText(iIndex, 6, szTmp);

		// 서버 2005.09.14.
		sprintf(szTmp, "%s", pstResult->m_szServerName);
		m_csSearchResult.SetItemText(iIndex, 7, szTmp);
	}
	else
	{
		// 없다면 마지막에 넣는다.
				
		// 캐릭이름
		m_csSearchResult.InsertItem(iIndex, pstResult->m_szCharName);

		// 계정이름
		m_csSearchResult.SetItemText(iIndex, 1, pstResult->m_szAccName);

		// 레벨
		sprintf(szTmp, "%d", pstResult->m_lLevel);
		m_csSearchResult.SetItemText(iIndex, 2, szTmp);

		AgpmCharacter* ppmCharacter = ( AgpmCharacter* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmCharacter" );
		AgpdCharacterTemplate* pcsCharacterTemplate = ppmCharacter->GetCharacterTemplate(pstResult->m_lTID);
		if(pcsCharacterTemplate)
		{
			INT32 lRace = 0, lClass = 0;

			AgpmFactors* ppmFactor = ( AgpmFactors* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmFactors" );
			ppmFactor->GetValue(&pcsCharacterTemplate->m_csFactor, &lRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
			ppmFactor->GetValue(&pcsCharacterTemplate->m_csFactor, &lClass,	AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

			// 종족
			if(ppmFactor->GetCharacterRaceName(lRace))
				m_csSearchResult.SetItemText(iIndex, 3, ppmFactor->GetCharacterRaceName(lRace));

			// 직업
			if(ppmFactor->GetCharacterClassName(lRace, lClass))
			m_csSearchResult.SetItemText(iIndex, 4, ppmFactor->GetCharacterClassName(lRace, lClass));
		}

		// 접속상태
		if(pstResult->m_lStatus == 1)
			m_csSearchResult.SetItemText(iIndex, 5, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ONLINE));
		else
			m_csSearchResult.SetItemText(iIndex, 5, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_OFFLINE));

		// 길드
		sprintf(szTmp, "%s", pstResult->m_szGuildID);
		m_csSearchResult.SetItemText(iIndex, 6, szTmp);

		// 서버 2005.09.14.
		sprintf(szTmp, "%s", pstResult->m_szServerName);
		m_csSearchResult.SetItemText(iIndex, 7, szTmp);
	}

	// CharView 에다가도 GuildID 넣어줘본다.
	if(strcmp(pstResult->m_szCharName, m_pCharView->m_stCharData.m_stBasic.m_szCharName) == 0)
		m_pCharView->SetGuildID(pstResult->m_szGuildID);

	// CharView 에다가도 ServerName 세팅해준다.
	if(strcmp(pstResult->m_szCharName, m_pCharView->m_stCharData.m_stBasic.m_szCharName) == 0)
		m_pCharView->SetServerName(pstResult->m_szServerName);

	return TRUE;
}

BOOL AlefAdminMain::ClearSearchResultList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSearchResult.DeleteAllItems();
	return TRUE;
}

BOOL AlefAdminMain::SetCharName(LPCTSTR szCharName)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(szCharName && strlen(szCharName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(m_szCharName, szCharName);

	return TRUE;
}

BOOL AlefAdminMain::ResetAllCharData()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_pCharView->InitCharData();
	m_pCharView->InitCharGridData();
	m_pCharView->m_pCharQuest->ClearContent();

	m_pItemView->OnClearInventory( Inventory_MailBox );
	m_pItemView->OnClearInventory( Inventory_Bank );
	m_pItemView->OnClearInventory( Inventory_Sales );
	m_pItemView->OnClearInventory( Inventory_Pet );
	m_pItemView->OnClearInventory( Inventory_Cash );
	m_pItemView->OnClearInventory( Inventory_Equip );
	m_pItemView->OnClearInventory( Inventory_Normal );
	m_pItemView->OnCbnSelchangeCbPos();

	//m_pItemView->SelectItem(NULL);	// 이건 내비둔다.
	m_pItemView->ClearConvertData();

	m_pSkillView->ClearSkillList();
	m_pSkillView->ClearSkillListCtrl();
	m_pSkillView->SetSkillPoint(0);

	m_pTitleView->ClearTitleContent();
	m_pTitleView->ClearTitleQuestContent();

	return TRUE;
}

BOOL AlefAdminMain::IsSearchCharacter(CHAR* szCharName)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szCharName)
		return FALSE;

	return strcmp(m_szCharName, szCharName) == 0 ? TRUE : FALSE;
}

BOOL AlefAdminMain::ProcessChatMessage(LPCTSTR szMessage)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return TRUE;
}

BOOL AlefAdminMain::ProcessSystemMessage(LPCTSTR szMessage)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return TRUE;
}

BOOL AlefAdminMain::ProcessAdminMessage(LPCTSTR szMessage)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szMessage)
		return FALSE;

	CString szMsg = _T("");
	m_csMessageAdmin.GetWindowText(szMsg);

	if(szMsg.GetLength() > MAX_EDIT_MESSAGE_LENGTH)
		szMsg = _T("");

	szMsg += szMessage;
	szMsg += "\r\n";

	m_csMessageAdmin.SetWindowText((LPCTSTR)szMsg);

	// Focus 를 맨 밑으로
	int iLineCount = m_csMessageAdmin.GetLineCount();
	if(iLineCount > 0)
	{
		m_csMessageAdmin.LineScroll(iLineCount);
	}

	return TRUE;
}

BOOL AlefAdminMain::ProcessActionMessage(LPCTSTR szMessage)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szMessage)
		return FALSE;

	CString szMsg = _T("");
	m_csMessageAction.GetWindowText(szMsg);

	if(szMsg.GetLength() > MAX_EDIT_MESSAGE_LENGTH)
		szMsg = _T("");

	szMsg += szMessage;
	szMsg += "\r\n";

	m_csMessageAction.SetWindowText((LPCTSTR)szMsg);

	// Focus 를 맨 밑으로
	int iLineCount = m_csMessageAction.GetLineCount();
	if(iLineCount > 0)
	{
		m_csMessageAction.LineScroll(iLineCount);
	}

	return TRUE;
}

BOOL AlefAdminMain::SaveWhisperToFile(AgpdChatData* pstChatData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstChatData)
		return FALSE;

	// 귓말인지 체크
	if(pstChatData->eChatType != AGPDCHATTING_TYPE_WHISPER &&
		pstChatData->eChatType != AGPDCHATTING_TYPE_WHISPER2)
		return FALSE;

	UINT32 ulCurrentTimeStamp = AlefAdminManager::Instance()->GetCurrentTimeStamp();

	CHAR szTmp[64];
	memset(szTmp, 0, sizeof(szTmp));
	
	CHAR szDate[128];
	memset(szDate, 0, sizeof(szDate));
	AlefAdminManager::Instance()->ConvertTimeStampToString(ulCurrentTimeStamp, szDate);

	CHAR szFileName[255];
	AlefAdminManager::Instance()->ConvertTimeStampToDateString(ulCurrentTimeStamp, szTmp);
	sprintf(szFileName, "%s_%s.txt", AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_WHISPER), szTmp);

	CHAR szMessage[255 + 128 + 1];
	memset(szMessage, 0, sizeof(szMessage));
	if(strcmp(pstChatData->szSenderName, AlefAdminManager::Instance()->m_stSelfAdminInfo.m_szAdminName) == 0)
	{
		// 귓말 보낸 것임
		sprintf(szMessage, "[Send %s] %s -> %s : ", szDate, pstChatData->szSenderName, pstChatData->szTargetName);
	}
	else
	{
		// 귓말 받음
		sprintf(szMessage, "[Recv %s] %s -> %s : ", szDate, pstChatData->szSenderName, AlefAdminManager::Instance()->m_stSelfAdminInfo.m_szAdminName);
	}

	strncat(szMessage, pstChatData->szMessage, pstChatData->lMessageLength);

	FILE* pfFile = fopen(szFileName, "a+");
	if(pfFile)
	{
		fprintf(pfFile, "%s\r\n", szMessage);
		fclose(pfFile);
	}

	return TRUE;
}

BOOL AlefAdminMain::OnReceiveCurrentUser(INT32 lCurrentUser)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString strFormat = _T("");
	CString szCurrentUser = _T("");
	AgcmLogin* pcmLogin = ( AgcmLogin* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgcmLogin" );

	if(AlefAdminManager::Instance()->GetAdminLevel() > 98)
	{
		strFormat.LoadString( IDS_MAIN_CURRENT_USER );
		szCurrentUser.Format( strFormat, pcmLogin->m_szWorldName, lCurrentUser );
	}
	else
	{
		strFormat.LoadString( IDS_MAIN_SERVER );
		szCurrentUser.Format( strFormat , pcmLogin->m_szWorldName );
	}

	SetDlgItemText(IDC_S_CURRENT_USER, (LPCTSTR)szCurrentUser);
	return TRUE;
}




// Callback

// From AgcmTargetting
// 마우스로 캐릭터를 찍었음.
BOOL AlefAdminMain::CBOnLButtonDownPicking(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass || !pCustData)
		return FALSE;

	INT32 lType = *(INT32*)pData;
	INT32 lCID = *(INT32*)pCustData;
	AlefAdminMain* pThis = (AlefAdminMain*)pClass;

	if(lType != ACUOBJECT_TYPE_CHARACTER)
		return TRUE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmCharacter" );
	AgpdCharacter* pcsCharacter = ppmCharacter->GetCharacter(lCID);
	if(!pcsCharacter)
		return TRUE;

	pThis->m_csSearchTypeCB.SetCurSel(0);		// 검색 타입은 캐릭터로
	pThis->m_csSearchEdit.SetWindowText(pcsCharacter->m_szID);

	CString szTmp = _T("");
	szTmp.Format("Picking Character : %s", pcsCharacter->m_szID);
	pThis->ProcessActionMessage((LPCTSTR)szTmp);

	return TRUE;
}

// From AgcmChatting2
BOOL AlefAdminMain::CBOnRecvChatting(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass)
		return FALSE;

	AgpdChatData* pstChatData = (AgpdChatData*)pData;
	AlefAdminMain* pThis = (AlefAdminMain*)pClass;

	if(pThis->m_cOptionDlg.m_bSaveWhisper)
		pThis->SaveWhisperToFile(pstChatData);

	return TRUE;
}











void AlefAdminMain::OnBnClickedBSearch()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!m_pfCBSearch)
		return;

	// 권한 체크
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		return;

	// 먼저 비운다.
	ClearSearchResultList();
	ResetAllCharData();

	ZeroMemory(&m_stLastSearch, sizeof(m_stLastSearch));

	// Search 항목 얻고
	m_stLastSearch.m_iField = (INT8)m_csSearchTypeCB.GetCurSel();
	GetDlgItemText(IDC_E_SEARCH_FIELD, m_stLastSearch.m_szSearchName, AGPACHARACTER_MAX_ID_STRING);

	AlefAdminManager::Instance()->GetAdminData()->SendSearchCharacter(&m_stLastSearch);
}

void AlefAdminMain::OnNMDblclkLcSearchResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// CharName 으로 다시 검색 패킷을 날린다.
	CString szCharName = _T("");
	szCharName = m_csSearchResult.GetItemText(nlv->iItem, 0);

	SearchCharacter((LPCTSTR)szCharName);

	*pResult = 0;
}

void AlefAdminMain::OnClose()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	AlefAdminManager::Instance()->GetAdminData()->CloseMainDlg();
	ShowWindow(SW_HIDE);
}


void AlefAdminMain::OnOption()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Option Dialog 를 DoModal 로 띄운다.
	if(!m_cOptionDlg.DoModal())
		return;

	LONG lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if(lExStyle & WS_EX_LAYERED)	// 현재 Layered Widow 임
	{
		if(m_cOptionDlg.m_bUseAlpha)
		{
			// 알파값 재조정
			SetLayeredWindowAttributes(0, (255 * (100 - m_cOptionDlg.m_lAlpha)) / 100, LWA_ALPHA);
		}
		else	// 알파를 사용하지 않는다면
		{
			SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle & ~WS_EX_LAYERED);
			RedrawWindow(NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
		}
	}
	else
	{
		if(m_cOptionDlg.m_bUseAlpha)	// 알파를 사용한다.
		{
			SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle | WS_EX_LAYERED);
			SetLayeredWindowAttributes(0, (255 * (100 - m_cOptionDlg.m_lAlpha)) / 100, LWA_ALPHA);
		}
	}
}

void AlefAdminMain::OnBnClickedChSos()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iCheck = m_csSOSBtn.GetCheck();
	if(iCheck == BST_CHECKED)
		m_bSOS = TRUE;
	else if(iCheck == BST_UNCHECKED)
		m_bSOS = FALSE;
}

void AlefAdminMain::OnTimer(UINT nIDEvent)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(nIDEvent == 0)
	{
		// SOS Macro
		if(m_bSOS)
		{
			CHAR szMsg[16];
			sprintf(szMsg, "/sos ");

			AgcmChatting2* pcmChatting = ( AgcmChatting2* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgcmChatting2" );
			pcmChatting->SendChatMessage(AGPDCHATTING_TYPE_NORMAL, szMsg, (INT32)strlen(szMsg),	NULL);
		}
	}

	//CDialog::OnTimer(nIDEvent);
	CXTResizeDialog::OnTimer(nIDEvent);
}

void AlefAdminMain::OnDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	//CDialog::OnDestroy();
	CXTResizeDialog::OnDestroy();

	KillTimer(0);
}

void AlefAdminMain::OnAbout()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	AlefAdminAbout csAbout;
	csAbout.DoModal();
}
