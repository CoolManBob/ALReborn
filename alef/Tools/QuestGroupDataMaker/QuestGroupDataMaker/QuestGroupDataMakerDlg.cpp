// QuestGroupDataMakerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "QuestGroupDataMaker.h"
#include "QuestGroupDataMakerDlg.h"
#include ".\questgroupdatamakerdlg.h"

#include "AuExcelTxtLib.h"
#include "AgpmQuest.h"
#include "ApBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHECK_LOG_FILE	"ValidationCheckReport.log"
#define LOAD_LOG_FILE	"LoadingReport.log"

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQuestGroupDataMakerDlg 대화 상자



CQuestGroupDataMakerDlg::CQuestGroupDataMakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuestGroupDataMakerDlg::IDD, pParent)
	, m_strPathTemplateData(_T(""))
	, m_strPathNPCData(_T(""))
	, m_strPathObjectData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZeroMemory(m_vtQusetNode, sizeof(m_vtQusetNode));
}

void CQuestGroupDataMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strPathTemplateData);
	DDX_Text(pDX, IDC_EDIT2, m_strPathNPCData);
	DDX_Text(pDX, IDC_EDIT3, m_strPathObjectData);
}

BEGIN_MESSAGE_MAP(CQuestGroupDataMakerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOAD_TEMPLATE_DATA, OnBnClickedLoadTemplateData)
	ON_BN_CLICKED(IDC_LOAD_NPC_DATA, OnBnClickedLoadNpcData)
	ON_BN_CLICKED(IDC_MAKE_QUEST_GROUP_DATA, OnBnClickedMakeQuestGroupData)
	ON_BN_CLICKED(IDC_VALIDATION_CHECK, OnBnClickedValidationCheck)
	ON_BN_CLICKED(IDC_CLEAR_DATA, OnBnClickedClearData)
	ON_BN_CLICKED(IDC_LOAD_OBJECT_DATA, OnBnClickedLoadObjectData)
END_MESSAGE_MAP()


// CQuestGroupDataMakerDlg 메시지 처리기

BOOL CQuestGroupDataMakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CQuestGroupDataMakerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CQuestGroupDataMakerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CQuestGroupDataMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CQuestGroupDataMakerDlg::GetFilePath(CString &rResult)
{
	CFileDialog FileDlg(TRUE, _T("*.*"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "All Files(*.*)|*.*");

	if (IDOK == FileDlg.DoModal())
	{
		
		rResult = FileDlg.GetPathName();
		return TRUE;
	}

	return FALSE;
}

void CQuestGroupDataMakerDlg::OnBnClickedLoadTemplateData()
{
	CString strResult;
	if (!GetFilePath(strResult)) return;

	m_strPathTemplateData = strResult;
	UpdateData(FALSE);

	if (m_strPathTemplateData.IsEmpty()) return;

	AuExcelTxtLib csTemplateTxt;
	if (!csTemplateTxt.OpenExcelFile(m_strPathTemplateData.GetBuffer(), TRUE)) return;

	INT32 lMaxRow = csTemplateTxt.GetRow();

	int	AGPDQUEST_FIELD_TID								= 0;
	int	AGPDQUEST_FIELD_SCENARIO_NAME					= 0;
	int	AGPDQUEST_FIELD_QUEST_NAME						= 0;
	int	AGPDQUEST_FIELD_START_AREA						= 0;
	int	AGPDQUEST_FIELD_CATEGORY						= 0;
	int	AGPDQUEST_FIELD_LOCATION_TID					= 0;
	int	AGPDQUEST_FIELD_START_LEVEL						= 0;
	int	AGPDQUEST_FIELD_START_MAX_LEVEL					= 0;	//문엘프퀘스트 관련 수정추가 2005.6.2	AGSDQUEST_EXPAND_BLOCK
	int	AGPDQUEST_FIELD_START_CLASS						= 0;
	int	AGPDQUEST_FIELD_START_GENDER					= 0;
	int	AGPDQUEST_FIELD_START_RACE						= 0;
	int	AGPDQUEST_FIELD_START_PREV_QUEST_TID			= 0;
	int	AGPDQUEST_FIELD_START_PREV_QUEST_NAME			= 0;
	int	AGPDQUEST_FIELD_GIVE_NPC						= 0;
	int	AGPDQUEST_FIELD_CONFIRM_NPC						= 0;
	int	AGPDQUEST_FIELD_QUEST_ITEM_TID					= 0;
	int	AGPDQUEST_FIELD_QUEST_ITEM_NAME					= 0;
	int	AGPDQUEST_FIELD_QUEST_ITEM_COUNT				= 0;
	int	AGPDQUEST_FIELD_ACCEPT_TEXT						= 0;
	int	AGPDQUEST_FIELD_INCOMPLETE_TEXT					= 0;
	int	AGPDQUEST_FIELD_COMPLETE_TEXT					= 0;
	int	AGPDQUEST_FIELD_GOAL_LEVEL						= 0;
	int	AGPDQUEST_FIELD_GOAL_MONEY						= 0;
	int	AGPDQUEST_FIELD_GOAL_ITEM_TID					= 0;
	int	AGPDQUEST_FIELD_GOAL_ITEM_NAME					= 0;
	int	AGPDQUEST_FIELD_GOAL_ITEM_COUNT					= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_TID				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_NAME				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_COUNT				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_NAME			= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_DROP_RATE	= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_COUNT		= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_TID				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_NAME				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_COUNT				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_NAME			= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_DROP_RATE	= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_COUNT		= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT1_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT1_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT1_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT1_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT2_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT2_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT2_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT2_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT3_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT3_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT3_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT3_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT4_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT4_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT4_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT4_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT5_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT5_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT5_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT5_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT1					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR1					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT2					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR2					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT3					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR3					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT4					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR4					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT5					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR5					= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_UPGRADE			= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_SOCKET				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_TID				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_NAME				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_COUNT				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_UPGRADE			= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_SOCKET				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_TID				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_NAME				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_COUNT				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_UPGRADE			= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_SOCKET				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_TID				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_NAME				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_COUNT				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_UPGRADE			= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_SOCKET				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_TID				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_NAME				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_COUNT				= 0;
	int	AGPDQUEST_FIELD_RESULT_MONEY					= 0;
	int	AGPDQUEST_FIELD_RESULT_EXP						= 0;
	int	AGPDQUEST_FIELD_RESULT_NEXT_QUEST_TID			= 0;
	int	AGPDQUEST_FIELD_RESULT_NEXT_QUEST_NAME			= 0;
	int	AGPDQUEST_FIELD_QUEST_TYPE						= 0;
	{
		// 각 필드의 옵셀을 2번째 Row 에서 찾아낸다.

		//FILE	* pFile = fopen( "quest.txt" , "wt" );

		const int	nCategoryRow = 2;
		for (INT32 lCol = 0 ; lCol < csTemplateTxt.GetColumn(); lCol++ )
		{
			char * pDataString = csTemplateTxt.GetData( lCol , nCategoryRow );

			if( NULL == pDataString ) continue;

			//fprintf( pFile , "%s\n" , pDataString );

			if		( !strncmp( pDataString , "Tid"								, 256 ) ) AGPDQUEST_FIELD_TID								= lCol;
			else if	( !strncmp( pDataString , "ScenarioName"					, 256 ) ) AGPDQUEST_FIELD_SCENARIO_NAME					= lCol;
			else if	( !strncmp( pDataString , "QuestName"						, 256 ) ) AGPDQUEST_FIELD_QUEST_NAME						= lCol;
			else if	( !strncmp( pDataString , "Area"							, 256 ) ) AGPDQUEST_FIELD_START_AREA						= lCol;
			else if	( !strncmp( pDataString , "Category"						, 256 ) ) AGPDQUEST_FIELD_CATEGORY						= lCol;
			else if	( !strncmp( pDataString , "Location"						, 256 ) ) AGPDQUEST_FIELD_LOCATION_TID					= lCol;
			else if	( !strncmp( pDataString , "Need_Level_Min"					, 256 ) ) AGPDQUEST_FIELD_START_LEVEL						= lCol;
			else if	( !strncmp( pDataString , "Need_Level_Max"					, 256 ) ) AGPDQUEST_FIELD_START_MAX_LEVEL					= lCol;
			else if	( !strncmp( pDataString , "Need_Class"						, 256 ) ) AGPDQUEST_FIELD_START_CLASS						= lCol;
			else if	( !strncmp( pDataString , "Need_Gender"						, 256 ) ) AGPDQUEST_FIELD_START_GENDER					= lCol;
			else if	( !strncmp( pDataString , "Need_Race"						, 256 ) ) AGPDQUEST_FIELD_START_RACE						= lCol;
			else if	( !strncmp( pDataString , "Need_PreviousQuestTid"			, 256 ) ) AGPDQUEST_FIELD_START_PREV_QUEST_TID			= lCol;
			else if	( !strncmp( pDataString , "Need_PreviousQuestName"			, 256 ) ) AGPDQUEST_FIELD_START_PREV_QUEST_NAME			= lCol;
			else if	( !strncmp( pDataString , "GiveNpcName"						, 256 ) ) AGPDQUEST_FIELD_GIVE_NPC						= lCol;
			else if	( !strncmp( pDataString , "ConfirmNpcName"					, 256 ) ) AGPDQUEST_FIELD_CONFIRM_NPC						= lCol;
			else if	( !strncmp( pDataString , "Quest_TransItemTid"				, 256 ) ) AGPDQUEST_FIELD_QUEST_ITEM_TID					= lCol;
			else if	( !strncmp( pDataString , "Quest_TransItemName"				, 256 ) ) AGPDQUEST_FIELD_QUEST_ITEM_NAME					= lCol;
			else if	( !strncmp( pDataString , "Quest_TransItemCount"			, 256 ) ) AGPDQUEST_FIELD_QUEST_ITEM_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Dialogue_Start"					, 256 ) ) AGPDQUEST_FIELD_ACCEPT_TEXT						= lCol;
			else if	( !strncmp( pDataString , "Dialogue_Progress"				, 256 ) ) AGPDQUEST_FIELD_INCOMPLETE_TEXT					= lCol;
			else if	( !strncmp( pDataString , "Dialogue_Finish"					, 256 ) ) AGPDQUEST_FIELD_COMPLETE_TEXT					= lCol;
			else if	( !strncmp( pDataString , "Complete_Level"					, 256 ) ) AGPDQUEST_FIELD_GOAL_LEVEL						= lCol;
			else if	( !strncmp( pDataString , "Complete_Money"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONEY						= lCol;
			else if	( !strncmp( pDataString , "Complete_ItemTid"				, 256 ) ) AGPDQUEST_FIELD_GOAL_ITEM_TID					= lCol;
			else if	( !strncmp( pDataString , "Complete_ItemName"				, 256 ) ) AGPDQUEST_FIELD_GOAL_ITEM_NAME					= lCol;
			else if	( !strncmp( pDataString , "Complete_ItemCount"				, 256 ) ) AGPDQUEST_FIELD_GOAL_ITEM_COUNT					= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterA_Tid"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_TID				= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterA_Name"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_NAME				= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterA_Count"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_COUNT				= lCol;
			else if	( !strncmp( pDataString , "DropItemA_TID"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "DropItemA_Name"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "DropItemA_Property"				, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_DROP_RATE	= lCol;
			else if	( !strncmp( pDataString , "DropItemA_Count"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_COUNT		= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterB_Tid"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_TID				= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterB_Name"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_NAME				= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterB_Count"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_COUNT				= lCol;
			else if	( !strncmp( pDataString , "DropItemB_TID"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "DropItemB_Name"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "DropItemB_Property"				, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_DROP_RATE	= lCol;
			else if	( !strncmp( pDataString , "DropItemB_Count"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_COUNT		= lCol;
			else if	( !strncmp( pDataString , "ObjectA_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT1_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemA_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT1_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemA_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT1_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemA_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT1_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "ObjectB_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT2_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemB_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT2_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemB_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT2_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemB_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT2_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "ObjectC_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT3_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemC_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT3_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemC_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT3_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemC_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT3_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "ObjectD_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT4_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemD_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT4_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemD_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT4_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemD_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT4_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "ObjectE_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT5_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemE_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT5_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemE_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT5_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemE_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT5_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary1"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT1					= lCol;
			else if	( !strncmp( pDataString , "CountFactor1"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR1					= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary2"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT2					= lCol;
			else if	( !strncmp( pDataString , "CountFactor2"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR2					= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary3"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT3					= lCol;
			else if	( !strncmp( pDataString , "CountFactor3"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR3					= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary4"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT4					= lCol;
			else if	( !strncmp( pDataString , "CountFactor4"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR4					= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary5"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT5					= lCol;
			else if	( !strncmp( pDataString , "CountFactor5"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR5					= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Upgrade_Intensify"	, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_UPGRADE			= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Upgrade_Slot"			, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_SOCKET				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Tid"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_TID				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Name"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_NAME				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Count"				, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Upgrade_Intensify"	, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_UPGRADE			= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Upgrade_Slot"			, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_SOCKET				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Tid"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_TID				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Name"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_NAME				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Count"				, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Upgrade_Intensify"	, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_UPGRADE			= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Upgrade_Slot"			, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_SOCKET				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Tid"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_TID				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Name"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_NAME				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Count"				, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Upgrade_Intensify"	, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_UPGRADE			= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Upgrade_Slot"			, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_SOCKET				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Tid"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_TID				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Name"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_NAME				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Count"				, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Gift_Money"						, 256 ) ) AGPDQUEST_FIELD_RESULT_MONEY					= lCol;
			else if	( !strncmp( pDataString , "Gift_Exp"						, 256 ) ) AGPDQUEST_FIELD_RESULT_EXP						= lCol;
			else if	( !strncmp( pDataString , "Gift_NextQuest_TID"				, 256 ) ) AGPDQUEST_FIELD_RESULT_NEXT_QUEST_TID			= lCol;
			else if	( !strncmp( pDataString , "Gift_NextQuest_Name"				, 256 ) ) AGPDQUEST_FIELD_RESULT_NEXT_QUEST_NAME			= lCol;
			else if	( !strncmp( pDataString , "Self_Quest"						, 256 ) ) AGPDQUEST_FIELD_QUEST_TYPE						= lCol;
		}
		// fclose( pFile );
	}
	
	TemplateData* pTemplateData;
	for (INT32 lRow = 3; lRow < lMaxRow; ++lRow)
	{
		pTemplateData = m_HashMapTemplate.AddTemplate( csTemplateTxt.GetDataToInt(AGPDQUEST_FIELD_TID, lRow),
														csTemplateTxt.GetData(AGPDQUEST_FIELD_GIVE_NPC, lRow),
														csTemplateTxt.GetData(AGPDQUEST_FIELD_CONFIRM_NPC, lRow) );

		if (1 == pTemplateData->lID % 8)
			TRACEFILE(LOAD_LOG_FILE, "[TID : %d] 8의 배수 + 1의 값은 TID로 사용할수 없습니다.", pTemplateData->lID);

		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_QUEST_NAME, lRow, AGPDQUEST_MAX_NAME);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_SCENARIO_NAME, lRow, AGPDQUEST_MAX_NAME);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_START_AREA, lRow, AGPDQUEST_MAX_AREA);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_ACCEPT_TEXT, lRow, AGPDQUEST_MAX_DIALOG_TEXT);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_INCOMPLETE_TEXT, lRow, AGPDQUEST_MAX_DIALOG_TEXT);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_COMPLETE_TEXT, lRow, AGPDQUEST_MAX_DIALOG_TEXT);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_SUMMARY_TEXT1, lRow, AGPDQUEST_MAX_SUMMARY_TEXT);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_SUMMARY_TEXT2, lRow, AGPDQUEST_MAX_SUMMARY_TEXT);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_SUMMARY_TEXT3, lRow, AGPDQUEST_MAX_SUMMARY_TEXT);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_SUMMARY_TEXT4, lRow, AGPDQUEST_MAX_SUMMARY_TEXT);
		CheckStringLength(&csTemplateTxt, pTemplateData->lID, AGPDQUEST_FIELD_SUMMARY_TEXT5, lRow, AGPDQUEST_MAX_SUMMARY_TEXT);

		if (csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT1_OBJECT_ID, lRow))
			pTemplateData->strCheckPointID[0] = csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT1_OBJECT_ID, lRow);

		if (csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT2_OBJECT_ID, lRow))
			pTemplateData->strCheckPointID[1] = csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT2_OBJECT_ID, lRow);

		if (csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT3_OBJECT_ID, lRow))
			pTemplateData->strCheckPointID[2] = csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT3_OBJECT_ID, lRow);

		if (csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT4_OBJECT_ID, lRow))
			pTemplateData->strCheckPointID[3] = csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT4_OBJECT_ID, lRow);

		if (csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT5_OBJECT_ID, lRow))
			pTemplateData->strCheckPointID[4] = csTemplateTxt.GetData(AGPDQUEST_FIELD_CHECKPOINT5_OBJECT_ID, lRow);
	}
}

void CQuestGroupDataMakerDlg::CheckStringLength(AuExcelTxtLib* pExcelTxt, INT32 lTID, INT32 lColumn, INT32 lRow, INT32 lMaxLength)
{
	if (NULL == pExcelTxt->GetData(lColumn, lRow))
		return;

	INT32 lLength = strlen(pExcelTxt->GetData(lColumn, lRow));

	if (lMaxLength < lLength)
	{
		TRACEFILE(LOAD_LOG_FILE, "[TID : %d] 문자 길이 초과 Column No: %d, 문자 길이 : %d(Max : %d)", 
									lTID, lColumn, lLength, lMaxLength);
	}
}

void CQuestGroupDataMakerDlg::OnBnClickedLoadNpcData()
{
	CString strResult;
	if (!GetFilePath(strResult)) return;

	m_strPathNPCData = strResult;
	UpdateData(FALSE);

	if (m_strPathNPCData.IsEmpty()) return;

	AuExcelTxtLib csNpcTxt;
	if (!csNpcTxt.OpenExcelFile(m_strPathNPCData.GetBuffer(), TRUE)) return;

	INT32 lMaxRow = csNpcTxt.GetRow();

	for (INT32 lRow = 3; lRow < lMaxRow; ++lRow)
	{
		ASSERT(NULL == m_HashMapQuestGroup.Find(csNpcTxt.GetData(3, lRow)));
		m_HashMapQuestGroup.AddQuestGroup(csNpcTxt.GetData(3, lRow), csNpcTxt.GetDataToInt(0, lRow), 
											QUESTGROUP_TYPE_NPC);
	}
}

void CQuestGroupDataMakerDlg::OnBnClickedMakeQuestGroupData()
{
	MakeData();
	WriteFile();
}

void CQuestGroupDataMakerDlg::MakeData()
{
	HashMapTemplate::IterTemplate iter = m_HashMapTemplate.m_HashMap.begin();
	TemplateData* pTemplateData;
	QuestGroupData* pQuestGroupData;

	// Quest 전체를 iteration하면서 부여자, 완료자, 체크포인트를 정리한다.
	while (iter != m_HashMapTemplate.m_HashMap.end())
	{
		pTemplateData = iter->second;
		ASSERT(NULL != pTemplateData);

		// Quest 부여자 추가
		if (pTemplateData->strGiveName.length())
		{
			pQuestGroupData = m_HashMapQuestGroup.Find(pTemplateData->strGiveName.c_str());
			ASSERT(NULL != pQuestGroupData);
			pQuestGroupData->listGive.push_back(pTemplateData->lID);
		}

		// Quest 완료자 추가
		if (pTemplateData->strConfirmName.length())
		{
			pQuestGroupData = m_HashMapQuestGroup.Find(pTemplateData->strConfirmName.c_str());
			ASSERT(NULL != pQuestGroupData);
			pQuestGroupData->listComfirm.push_back(pTemplateData->lID);
		}

		for (int i = 0; i < MAX_CHECK_POINT; ++i)
		{
			if (pTemplateData->strCheckPointID[i].empty())
				continue;

			pQuestGroupData = m_HashMapQuestGroup.Find(pTemplateData->strCheckPointID[i].c_str());
			ASSERT(NULL != pQuestGroupData);
			CheckPointElement checkPoint;
			checkPoint.lIndex = i;
			checkPoint.lQuestTID = pTemplateData->lID;
			pQuestGroupData->listCheckPoint.push_back(checkPoint);
		}

		++iter;
	}
}

void CQuestGroupDataMakerDlg::WriteFile()
{
	FILE *file;
	QuestGroupData* pQuestGroupData;
	list<INT>::iterator iterList;

	file = fopen("QuestGroup.ini", "wt");

	ASSERT(file);

	CHashMapQuestGroup::IterQuestGroup iter = m_HashMapQuestGroup.m_HashMap.begin();
	while(iter != m_HashMapQuestGroup.m_HashMap.end())
	{
		pQuestGroupData = iter->second;

		if (pQuestGroupData->listComfirm.empty() && pQuestGroupData->listGive.empty() && pQuestGroupData->listCheckPoint.empty())
		{
			++iter;
			continue;
		}

		fprintf(file, "%d\t", pQuestGroupData->lID);
		fprintf(file, "%s\t", iter->first.c_str());

		list<INT>::iterator iterGive = pQuestGroupData->listGive.begin();
		while (iterGive != pQuestGroupData->listGive.end())
		{
			fprintf(file, "%d:", *iterGive);
			++iterGive;
		}
		fprintf(file, "\t");

		list<INT>::iterator iterConfirm = pQuestGroupData->listComfirm.begin();
		while (iterConfirm != pQuestGroupData->listComfirm.end())
		{
			fprintf(file, "%d:", *iterConfirm);
			++iterConfirm;
		}

		fprintf(file, "\t");
		list<CheckPointElement>::iterator iterCheckPoint = pQuestGroupData->listCheckPoint.begin();
		while (iterCheckPoint != pQuestGroupData->listCheckPoint.end())
		{
			fprintf(file, "%d-%d:", iterCheckPoint->lQuestTID, iterCheckPoint->lIndex);
			++iterCheckPoint;
		}		
		
		fprintf(file, "\n");
		++iter;
	}

	fclose(file);
	MessageBox("Quest Group 파일 생성이 완료 되었습니다.");
}

void CQuestGroupDataMakerDlg::OnBnClickedValidationCheck()
{
	Check_QuestData();
//	Check_NPCInfo();
//	Check_QuestNode();
}

void CQuestGroupDataMakerDlg::Check_QuestData()
{
	HashMapTemplate::IterTemplate iter = m_HashMapTemplate.m_HashMap.begin();
	TemplateData* pTemplateData;
	QuestGroupData* pQuestGroupData;

	// Quest 전체를 iteration하면서 부여자, 완료자, 체크포인트가 유효한지 확인한다.
	while (iter != m_HashMapTemplate.m_HashMap.end())
	{
		pTemplateData = iter->second;
		if (NULL == pTemplateData)
			TRACEFILE(CHECK_LOG_FILE, "[Quest TID : %d] 존재하지 않는 Quest", pTemplateData->lID);

		// Quest 부여자 추가
		if (pTemplateData->strGiveName.length())
		{
			pQuestGroupData = m_HashMapQuestGroup.Find(pTemplateData->strGiveName.c_str());
			if (NULL == pQuestGroupData)
				TRACEFILE(CHECK_LOG_FILE, "[Quest TID : %d] 존재하지 않는 부여자 -> %s", 
							pTemplateData->lID, pTemplateData->strGiveName.c_str());
		}

		// Quest 완료자 추가
		if (pTemplateData->strConfirmName.length())
		{
			pQuestGroupData = m_HashMapQuestGroup.Find(pTemplateData->strConfirmName.c_str());
			if (NULL == pQuestGroupData)
				TRACEFILE(CHECK_LOG_FILE, "[Quest TID : %d] 존재하지 않는 완료자 -> %s", 
							pTemplateData->lID, pTemplateData->strConfirmName.c_str());
		}

		for (int i = 0; i < CHECKPOINT_COUNT; ++i)
		{
			if (pTemplateData->strCheckPointID[i].empty())
				continue;

			pQuestGroupData = m_HashMapQuestGroup.Find(pTemplateData->strCheckPointID[i].c_str());
			if (NULL == pQuestGroupData)
				TRACEFILE(CHECK_LOG_FILE, "[Quest TID : %d] 존재하지 않는 CheckPoint -> %s", 
						pTemplateData->lID, pTemplateData->strCheckPointID[i].c_str());
		}

		++iter;
	}
}

//void CQuestGroupDataMakerDlg::Check_NPCInfo()
//{
	//HashMapTemplate::IterTemplate iter = m_HashMapTemplate.m_HashMap.begin();
	//TemplateData* pTemplateData;
	//NPCData* pNPCData;

	//ZeroMemory(m_vtQusetNode, sizeof(m_vtQusetNode));
	//
	//if (m_bFirst) return;
	//m_bFirst = TRUE;

	//while (iter != m_HashMapTemplate.m_HashMap.end())
	//{
	//	pTemplateData = iter->second;
	//	pNPCData = m_HashMapNPC.Find(pTemplateData->strConfirmName.c_str());
	//	if (pNPCData)
	//	{
	//		m_vtQusetNode[pTemplateData->lID].bUse = TRUE;
	//		m_vtQusetNode[pTemplateData->lID].lConfirmCount++;
	//		pNPCData->listComfirm.push_back(pTemplateData->lID);
	//	}
	//	else
	//		TRACEFILE(CHECK_LOG_FILE, "[Quest TID : %d] 존재하지 않는 NPC : %s", pTemplateData->lID, pTemplateData->strConfirmName.c_str());

	//	pNPCData = m_HashMapNPC.Find(pTemplateData->strGiveName.c_str());
	//	if (pNPCData)
	//	{
	//		m_vtQusetNode[pTemplateData->lID].bUse = TRUE;
	//		m_vtQusetNode[pTemplateData->lID].lGiveCount++;
	//		pNPCData->listGive.push_back(pTemplateData->lID);
	//	}
	//	else
	//		TRACEFILE(CHECK_LOG_FILE, "[Quest TID : %d] 존재하지 않는 NPC : %s", pTemplateData->lID, pTemplateData->strGiveName.c_str());

	//	++iter;
	//}
//}

//void CQuestGroupDataMakerDlg::Check_QuestNode()
//{
//	for (int i = 0; i < MAX_NODE_COUNT; ++i)
//	{
//		if (m_vtQusetNode[i].bUse)
//		{
//			if ((1 != m_vtQusetNode[i].lConfirmCount) || (1 != m_vtQusetNode[i].lGiveCount))
//				TRACEFILE(CHECK_LOG_FILE, "[Quest TID : %d] 퀘스트 노드 설정 오류 Give (%d), Confirm (%d)", i, m_vtQusetNode[i].lGiveCount, m_vtQusetNode[i].lConfirmCount);
//		}
//	}
//}

void CQuestGroupDataMakerDlg::OnBnClickedClearData()
{
	ZeroMemory(m_vtQusetNode, sizeof(m_vtQusetNode));
	m_HashMapQuestGroup.Destory();
	m_HashMapTemplate.Destroy();

	m_strPathNPCData.Empty();
	m_strPathTemplateData.Empty();
	UpdateData(FALSE);
}

void CQuestGroupDataMakerDlg::OnBnClickedLoadObjectData()
{
	// TODO: Add your control notification handler code here
	CString strResult;
	if (!GetFilePath(strResult)) return;

	m_strPathObjectData = strResult;
	UpdateData(FALSE);

	if (m_strPathObjectData.IsEmpty()) return;

	AuExcelTxtLib csObjectText;
	if (!csObjectText.OpenExcelFile(m_strPathObjectData.GetBuffer(), TRUE)) return;

	INT32 lMaxRow = csObjectText.GetRow();

	for (INT32 lRow = 3; lRow < lMaxRow; ++lRow)
	{
		ASSERT(0 != csObjectText.GetDataToInt(0, lRow));
		ASSERT(NULL == m_HashMapQuestGroup.Find(csObjectText.GetData(0, lRow)));
		m_HashMapQuestGroup.AddQuestGroup(csObjectText.GetData(0, lRow), 
											csObjectText.GetDataToInt(0, lRow), QUESTGROUP_TYPE_OBJECT);
	}
}
