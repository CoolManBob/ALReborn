// AgcmAdminDlgXT_Main.cpp : implementation file
//

#include "stdafx.h"
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MESSAGE_BOX_TITLE	"ArchLord Admin - Main"

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Main dialog

GdiplusStartupInput gidplusStartupInput;
ULONG_PTR gdiplusToken;

AgcmAdminDlgXT_Main::AgcmAdminDlgXT_Main(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAdminDlgXT_Main::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAdminDlgXT_Main)
	m_szInfoText = _T("");
	m_szMovePC = _T("");
	m_szMoveTargetPC = _T("");
	m_szMoveX = _T("");
	m_szMoveY = _T("");
	m_szMoveZ = _T("");
	m_szMovePlace = _T("");
	m_szAdminCount = _T("0");
	m_szHelpCount = _T("0");
	m_szCurrentUserCount = _T("0");
	//}}AFX_DATA_INIT

	m_bInitialized = FALSE;

	m_pfCBSearch = NULL;
	m_pfCBHelpProcess = NULL;
	
	SetSelectedHelp(NULL);
	SetProcessHelp(NULL);
	
	memset(&m_stCharData, 0, sizeof(m_stCharData));
}

void AgcmAdminDlgXT_Main::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAdminDlgXT_Main)
	DDX_Control(pDX, IDC_S_MAIN_CURRENT_USER, m_csCurrentUserCount);
	DDX_Control(pDX, IDC_CB_MAIN_MOVE_PLACE, m_csMovePlaceCB);
	DDX_Control(pDX, IDC_B_MAIN_MOVE_GO, m_csMoveBtn);
	DDX_Control(pDX, IDC_LS_MAIN_OBJECT, m_csObjectListBox);
	DDX_Text(pDX, IDC_E_MAIN_INFO, m_szInfoText);
	DDX_Control(pDX, IDC_LV_MAIN_HELP, m_csHelpListView);
	DDX_Text(pDX, IDC_E_MAIN_MOVE_PC, m_szMovePC);
	DDX_Text(pDX, IDC_E_MAIN_MOVE_TARGET, m_szMoveTargetPC);
	DDX_Text(pDX, IDC_E_MAIN_MOVE_X, m_szMoveX);
	DDX_Text(pDX, IDC_E_MAIN_MOVE_Y, m_szMoveY);
	DDX_Text(pDX, IDC_E_MAIN_MOVE_Z, m_szMoveZ);
	DDX_CBString(pDX, IDC_CB_MAIN_MOVE_PLACE, m_szMovePlace);
	DDX_Text(pDX, IDC_S_MAIN_ADMIN_COUNT, m_szAdminCount);
	DDX_Text(pDX, IDC_S_MAIN_HELP_COUNT, m_szHelpCount);
	DDX_Text(pDX, IDC_S_MAIN_CURRENT_USER, m_szCurrentUserCount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAdminDlgXT_Main, CDialog)
	//{{AFX_MSG_MAP(AgcmAdminDlgXT_Main)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LS_MAIN_OBJECT, OnSelchangeObjectList)
	ON_COMMAND(IDM_TOOL_SEARCH, OnToolSearch)
	ON_COMMAND(IDM_TOOL_CHARACTER, OnToolCharacter)
	ON_COMMAND(IDM_TOOL_HELP, OnToolHelp)
	ON_COMMAND(IDM_TOOL_MOVE, OnToolMove)
	ON_NOTIFY(NM_CLICK, IDC_LV_MAIN_HELP, OnClickLvMainHelp)
	ON_NOTIFY(NM_DBLCLK, IDC_LV_MAIN_HELP, OnDblclkLvMainHelp)
	ON_BN_CLICKED(IDC_B_MAIN_MOVE_GO, OnBMainMoveGo)
	ON_COMMAND(IDM_OBJECT_CLEAR, OnObjectClear)
	ON_LBN_DBLCLK(IDC_LS_MAIN_OBJECT, OnDblclkLsMainObject)
	ON_COMMAND(IDM_OBJECT_DEL, OnObjectDel)
	ON_COMMAND(IDM_OBJECT_SELECT, OnObjectSelect)
	ON_COMMAND(IDM_HELP_SELECT, OnHelpSelect)
	ON_CBN_SELCHANGE(IDC_CB_MAIN_MOVE_PLACE, OnSelchangeCbMainMovePlace)
	ON_COMMAND(IDM_TOOL_ITEM, OnToolItem)
	ON_COMMAND(IDM_TOOL_ADMIN_LIST, OnToolAdminList)
	ON_WM_VKEYTOITEM()
	ON_COMMAND(IDM_TOOL_NOTICE, OnToolNotice)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Main message handlers
BOOL AgcmAdminDlgXT_Main::Create()
{
	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::OpenDlg(INT nShowCmd)
{
	if(::IsWindow(m_hWnd))
		SetFocus();
	else
	{
		// Initialize GDI+
		GdiplusStartup(&gdiplusToken, &gidplusStartupInput, NULL);
		
		// Config 를 로드한다.
		AgcmAdminDlgXT_Manager::Instance()->GetIniManager()->Load();

		// Message Queue 를 초기화/구동 시킨다.
		AgcmAdminDlgXT_Manager::Instance()->GetMessageQueue()->Init();
		AgcmAdminDlgXT_Manager::Instance()->GetMessageQueue()->Start();

		// 꽁수는 실패로 돌아감. 안해 -_-;;
		// 일단 모든 윈도우를 다 생성한다. - Window Z-Position 때문에... 이런 꽁수를 도입
		//AgcmAdminDlgXT_Manager::Instance()->OpenAllWindows();
		
		CDialog::Create(IDD, m_pParentWnd);
	}

	ShowCursor(TRUE);

	ShowWindow(nShowCmd);

	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::CloseDlg()
{
	if(::IsWindow(m_hWnd))
		ShowWindow(SW_HIDE);
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::IsInitialized()
{
	return m_bInitialized;
}

BOOL AgcmAdminDlgXT_Main::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_bInitialized = TRUE;
	::ShowCursor(TRUE);

	OnInitHelpListView();

	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;
	m_csMoveBtn.SetXButtonStyle(dwStyle);

	ShowObjectList();
	ShowHelpList();

	// MovePlaceCB 를 Move Dialog 의 데이터로 부터 받아서 만든다.
	AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->MakeMainWindowMovePlaceCB();

	UpdateAdminCount();
	UpdateHelpCount();

	return TRUE;
}

void AgcmAdminDlgXT_Main::OnClose()
{
	CloseDlg();
}

void AgcmAdminDlgXT_Main::PostNcDestroy()
{
	ClearHelpList();

	// Shutdown GDI+
	GdiplusShutdown(gdiplusToken);

	CDialog::PostNcDestroy();
}




//////////////////////////////////////////////////////////
// Operation
BOOL AgcmAdminDlgXT_Main::SetCBSearch(ADMIN_CB pfCBSearch)
{
	m_pfCBSearch = pfCBSearch;
	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::SetCBHelpProcess(ADMIN_CB pfCBHelpProcess)
{
	m_pfCBHelpProcess = pfCBHelpProcess;
	return TRUE;
}

// For Debug
BOOL AgcmAdminDlgXT_Main::SetCBHelpRequest(ADMIN_CB pfCBHelpRequest)
{
	m_pfCBHelpRequest = pfCBHelpRequest;
	return TRUE;
}

void AgcmAdminDlgXT_Main::Lock()
{
	m_csLock.Lock();
}

void AgcmAdminDlgXT_Main::Unlock()
{
	m_csLock.Unlock();
}

void AgcmAdminDlgXT_Main::HelpLock()
{
	m_csHelpLock.Lock();
}

void AgcmAdminDlgXT_Main::HelpUnlock()
{
	m_csHelpLock.Unlock();
}

BOOL AgcmAdminDlgXT_Main::AddObject(stAgpdAdminPickingData* pstPickingData)
{
	if(!m_bInitialized)
		return FALSE;

	if(!pstPickingData)
		return FALSE;

	if(strlen(pstPickingData->m_szName) == 0)
		return FALSE;

	// 이미 있는 지 확인해서 있으면 기존 것을 지운다.
	if(GetObject(pstPickingData->m_szName))
	{
		RemoveObject(pstPickingData->m_szName);
		RemoveObjectInListBox(pstPickingData->m_szName);
	}

	// 포인터 새로 생성해서 넣는다.
	stAgpdAdminPickingData* pstNewPickingData = new stAgpdAdminPickingData;
	memcpy(pstNewPickingData, pstPickingData, sizeof(stAgpdAdminPickingData));

	m_listObject.push_back(pstNewPickingData);

	//ClearObjectListBox();
	//return ShowObjectList();

	// 속도 개선 작업의 일환
	m_csObjectListBox.InsertString(0, pstNewPickingData->m_szName);
	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::GetSelectedObjectID(LPCTSTR szName)
{
	if(!m_bInitialized)
		return FALSE;

	//m_csObjectListBox.UpdateData();
	int iIndex = m_csObjectListBox.GetCurSel();
	
	if(iIndex == LB_ERR)
		return FALSE;

	m_csObjectListBox.GetText(iIndex, (char*)szName);
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::GetSelectedObjectID(CString& szName)
{
	if(!m_bInitialized)
		return FALSE;

	//m_csObjectListBox.UpdateData();
	int iIndex = m_csObjectListBox.GetCurSel();

	if(iIndex == LB_ERR)
		return FALSE;

	m_csObjectListBox.GetText(iIndex, szName);

	return TRUE;
}

stAgpdAdminPickingData* AgcmAdminDlgXT_Main::GetObject(LPCTSTR szName)
{
	if(m_listObject.size() == 0 || !szName)
		return NULL;

	list<stAgpdAdminPickingData*>::iterator iterData = m_listObject.begin();
	while(iterData != m_listObject.end())
	{
		if(!*iterData)
		{
			iterData++;
			continue;
		}

		if(strcmp(szName, (*iterData)->m_szName) == 0)
			return *iterData;

		iterData++;
	}

	return NULL;
}

BOOL AgcmAdminDlgXT_Main::RemoveObject(LPCTSTR szName)
{
	if(m_listObject.size() == 0 || !szName)
		return FALSE;

	BOOL bRemove = FALSE;

	list<stAgpdAdminPickingData*>::iterator iterData = m_listObject.begin();
	while(iterData != m_listObject.end())
	{
		if(!*iterData)
		{
			iterData++;
			continue;
		}
		
		if(strcmp(szName, (*iterData)->m_szName) == 0)
		{
			delete *iterData;
			m_listObject.erase(iterData);

			bRemove = TRUE;
			break;
		}

		iterData++;
	}

	return bRemove;
}

BOOL AgcmAdminDlgXT_Main::RemoveObjectInListBox(LPCTSTR szName)
{
	if(!m_bInitialized)
		return FALSE;

	if(!szName)
		return FALSE;

	int iIndex = m_csObjectListBox.FindStringExact(0, szName);
	if(iIndex < 0)
		return FALSE;

	m_csObjectListBox.DeleteString(iIndex);
	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::ClearObjectList()
{
	if(m_listObject.size() == 0)
		return FALSE;

	list<stAgpdAdminPickingData*>::iterator iterData = m_listObject.begin();
	while(iterData != m_listObject.end())
	{
		if(*iterData)
			delete *iterData;

		iterData++;
	}

	m_listObject.clear();

	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::ClearObjectListBox()
{
	if(!m_bInitialized)
		return FALSE;

	//UpdateData();
	m_csObjectListBox.ResetContent();
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::ShowObjectList()
{
	if(!m_bInitialized)
		return FALSE;

	if(m_listObject.size() == 0)
		return ClearObjectListBox();

	list<stAgpdAdminPickingData*>::reverse_iterator iterData = m_listObject.rbegin();
	while(iterData != m_listObject.rend())
	{
		if(*iterData)
			m_csObjectListBox.AddString((*iterData)->m_szName);
		
		iterData++;
	}
	
	return TRUE;
}







///////////////////////////////////////////////////////////////////////////////////
// Info Text

BOOL AgcmAdminDlgXT_Main::SetInfoCharData(stAgpdAdminCharData* pstCharData)
{
	if(!pstCharData)
		return FALSE;

	CString szTmp;
	if(GetSelectedObjectID(szTmp) == FALSE)
		return FALSE;

	// 선택한 Object 인지 확인한다.
	if(szTmp.Compare(pstCharData->m_stBasic.m_szCharName) != 0)
	{
		// 그냥 Skip 한다.
		return TRUE;
	}

	// 여기서는 memcpy 만 하고, 화면에 뿌리지는 않는다.
	// 화면에 뿌리는 시점은 Sub 정보를 받았을 때이다.
	memcpy(&m_stCharData, pstCharData, sizeof(m_stCharData));

	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::SetInfoCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub)
{
	// 받은 데이터가 선택한 Object 인지 확인해서..
	//if(pstCharDataSub && m_stCharData.m_stBasic.m_lCID == pstCharDataSub->m_lCID)
	if(pstCharDataSub && strcmp(m_stCharData.m_stBasic.m_szCharName, pstCharDataSub->m_szName) == 0)
		memcpy(&m_stCharData.m_stSub, pstCharDataSub, sizeof(m_stCharData.m_stSub));
	else
		memset(&m_stCharData.m_stSub, 0, sizeof(m_stCharData.m_stSub));

	return ShowInfoText();
}

BOOL AgcmAdminDlgXT_Main::ClearInfoText()
{
	if(!m_bInitialized)
		return FALSE;

	m_szInfoText.Empty();
	UpdateData(FALSE);
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::ShowInfoText()
{
	if(!m_bInitialized)
		return FALSE;

	if(m_stCharData.m_stBasic.m_lCID == 0 ||
		strlen(m_stCharData.m_stBasic.m_szCharName) == 0)
	{
		m_szInfoText.Empty();
		UpdateData(FALSE);

		return TRUE;
	}

	CString szTmp;
	// 선택한 것이 없을 때는 지운다.
	if(GetSelectedObjectID(szTmp) == FALSE)
	{
		m_szInfoText.Empty();
		UpdateData(FALSE);

		return TRUE;
	}
	
	m_szInfoText.Empty();

	// CharName
	szTmp.Format("CharName : %s\r\n", m_stCharData.m_stBasic.m_szCharName);
	m_szInfoText += szTmp;

	// CharNo
	szTmp.Format("CharNo : %d\r\n", m_stCharData.m_stBasic.m_lCID);
	m_szInfoText += szTmp;

	// Acc
	szTmp.Format("AccName : %s\r\n", m_stCharData.m_stSub.m_szAccName);
	m_szInfoText += szTmp;

	// Name
	szTmp.Format("Name : %s\r\n", m_stCharData.m_stSub.m_szName);
	m_szInfoText += szTmp;

	// Race, Class
	szTmp.Format("Race/Class : %s/%s\r\n", m_stCharData.m_stStatus.m_szRace, m_stCharData.m_stStatus.m_szClass);
	m_szInfoText += szTmp;

	// Status
	//wsprintf(szTmp, "Status:%d ", m_stCharData.m_stBasic.m_lCID);	// 아직 Status 필드가 없다??
	//strcat(m_szInfoText, szTmp);

	// XYZ
	szTmp.Format("X, Y, Z : %.0f, %.0f, %.0f\r\n",
						m_stCharData.m_stBasic.m_stPos.x,
						m_stCharData.m_stBasic.m_stPos.y,
						m_stCharData.m_stBasic.m_stPos.z);	// 음.. 
	m_szInfoText += szTmp;

	// IP
	szTmp.Format("IP : %s\r\n", m_stCharData.m_stSub.m_szIP);
	m_szInfoText += szTmp;

	// Ghelld
	szTmp.Format("Ghelld\r\n");
	m_szInfoText += szTmp;
	szTmp.Format("- Inven : %d\r\n- Bank : %d\r\n", m_stCharData.m_stMoney.m_lInventoryMoney, 0);
	m_szInfoText += szTmp;

	// EXP, Level
	szTmp.Format("EXP/Level : %d, %d\r\n", m_stCharData.m_stPoint.m_lEXP, m_stCharData.m_stStatus.m_lLevel);
	m_szInfoText += szTmp;

	// HP, MaxHP
	szTmp.Format("HP/MaxHP : %d/%d\r\n", m_stCharData.m_stPoint.m_lHP, m_stCharData.m_stPoint.m_lMaxHP);
	m_szInfoText += szTmp;

	// MP, MaxMP
	szTmp.Format("MP/MaxMP : %d/%d\r\n", m_stCharData.m_stPoint.m_lMP, m_stCharData.m_stPoint.m_lMaxMP);
	m_szInfoText += szTmp;

	// SP, MaxSP
	szTmp.Format("SP/MaxSP : %d/%d\r\n", m_stCharData.m_stPoint.m_lSP, m_stCharData.m_stPoint.m_lMaxSP);
	m_szInfoText += szTmp;

	UpdateData(FALSE);
	
	return TRUE;
}











/////////////////////////////////////////////////////////////////////////////
// Move
BOOL AgcmAdminDlgXT_Main::SetPosition(float fX, float fY, float fZ)
{
	if(!m_bInitialized)
		return FALSE;

	m_szMoveX.Format("%.2f", fX);
	m_szMoveY.Format("%.2f", fY);
	m_szMoveZ.Format("%.2f", fZ);

	UpdateData(FALSE);

	return TRUE;
}

// Move Dialog 에서 호출한다.
BOOL AgcmAdminDlgXT_Main::AddMovePlace(CString& szMovePlace)
{
	if(!m_bInitialized)
		return FALSE;

	m_csMovePlaceCB.AddString(szMovePlace);

	return TRUE;
}

// Move Dialog 에서 호출한다.
BOOL AgcmAdminDlgXT_Main::ClearMovePlace()
{
	if(!m_bInitialized)
		return FALSE;

	m_csMovePlaceCB.ResetContent();

	return TRUE;
}










//////////////////////////////////////////////////////////////////////////////
// Help
BOOL AgcmAdminDlgXT_Main::AddHelp(stAgpdAdminHelp* pstHelp)
{
	if(!pstHelp)
		return FALSE;

	if(GetHelp(pstHelp->m_lCount))
		return FALSE;

	stAgpdAdminHelp* pstNewHelp = new stAgpdAdminHelp;
	memcpy(pstNewHelp, pstHelp, sizeof(stAgpdAdminHelp));
	m_listHelp.push_back(pstNewHelp);

	// 화면을 비우고
	ClearHelpListView();

	return ShowHelpList();
}

BOOL AgcmAdminDlgXT_Main::ProcessHelp(stAgpdAdminHelp* pstHelp)
{
	// 2004.02.24. 이제 안씀
	return TRUE;

	//if(!pstHelp)
	//	return FALSE;
	
	// Remove Help
	//RemoveHelp(pstHelp);

	//SetSelectedHelp(NULL);

	// 이게 필요한 것인지 아닌지 잘 모르겠다.....
	// 확인 안해도 될듯 한데.. Help 다이얼로그에서만 처리해도 될듯 한데.. -0-
	// 넘어온 pstHelp 가 자기가 처리할 것인지 확인한다.
	//if(strcmp(pstHelp->m_szAdminName, AgcmAdminDlgXT_Manager::Instance()->GetSelfAdminInfo()->m_szAdminName) == 0)
	//{
	//	SetProcessHelp(pstHelp);
	//	return TRUE;
	//}
	
	//return FALSE;
}

BOOL AgcmAdminDlgXT_Main::RemoveHelp(stAgpdAdminHelp* pstHelp)
{
	if(!pstHelp)
		return FALSE;

	if(m_listHelp.size() == 0)
		return FALSE;

	// 2004.02.24. Selected Help 를 초기화 시킴.
	if(pstHelp->m_lCount == GetSelectedHelp()->m_lCount)
		SetSelectedHelp(NULL);
	
	list<stAgpdAdminHelp*>::iterator iterData = m_listHelp.begin();
	while(iterData != m_listHelp.end())
	{
		if((*iterData) && (*iterData)->m_lCount == pstHelp->m_lCount)
		{
			delete *iterData;
			m_listHelp.erase(iterData);
			break;
		}

		iterData++;
	}

	// 화면을 비우고
	ClearHelpListView();

	return ShowHelpList();
}

BOOL AgcmAdminDlgXT_Main::SetSelectedHelp(stAgpdAdminHelp* pstHelp)
{
	if(pstHelp)
		memcpy(&m_stSelectedHelp, pstHelp, sizeof(m_stSelectedHelp));
	else
		memset(&m_stSelectedHelp, 0, sizeof(m_stSelectedHelp));

	return TRUE;
}

stAgpdAdminHelp* AgcmAdminDlgXT_Main::GetSelectedHelp()
{
	return &m_stSelectedHelp;
}

BOOL AgcmAdminDlgXT_Main::SetProcessHelp(stAgpdAdminHelp* pstHelp)
{
	if(pstHelp)
		memcpy(&m_stProcessHelp, pstHelp, sizeof(m_stProcessHelp));
	else
		memset(&m_stProcessHelp, 0, sizeof(m_stProcessHelp));
	
	return TRUE;
}

stAgpdAdminHelp* AgcmAdminDlgXT_Main::GetProcessHelp()
{
	return &m_stProcessHelp;
}

stAgpdAdminHelp* AgcmAdminDlgXT_Main::GetHelp(INT32 lHelpID)
{
	if(lHelpID < 0)
		return NULL;

	if(m_listHelp.size() == 0)
		return NULL;

	list<stAgpdAdminHelp*>::iterator iterData = m_listHelp.begin();
	while(iterData != m_listHelp.end())
	{
		if((*iterData) && (*iterData)->m_lCount == lHelpID)
			return *iterData;

		iterData++;
	}
	
	return NULL;
}

BOOL AgcmAdminDlgXT_Main::RequestHelp()
{
	if(!m_bInitialized)
		return FALSE;

	UpdateData();
	if(m_szInfoText.IsEmpty())
	{
		MessageBox("진정 내용을 입력하세요.", MESSAGE_BOX_TITLE);
		return TRUE;
	}
	
	stAgpdAdminHelp stHelp;
	memset(&stHelp, 0, sizeof(stHelp));

	strcpy(stHelp.m_szSubject, (LPCTSTR)m_szInfoText);

	m_pfCBHelpRequest(&stHelp, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);

	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::SendProcessHelp(stAgpdAdminHelp& stHelp)
{
	if(!m_pfCBHelpProcess)
		return FALSE;

	// 현재 처리중인 Help 가 있으면 안된다.
	//stAgpdAdminHelp* pstProcessHelp = AgcmAdminDlgManager::Instance()->GetHelpDlg()->GetProcessHelp();
	//if(pstProcessHelp)
	//	return FALSE;

	if(stHelp.m_lCount != 0)
		return FALSE;

	// Callback Execute
	m_pfCBHelpProcess(&stHelp, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);

	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::ShowHelpList()
{
	if(!m_bInitialized)
		return FALSE;

	if(m_listHelp.size() == 0)
	{
		UpdateHelpCount();
		return FALSE;
	}

	//UpdateData();

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	int iRows = 0;
	list<stAgpdAdminHelp*>::reverse_iterator iterData = m_listHelp.rbegin();
	while(iterData != m_listHelp.rend())
	{
		if((*iterData) == NULL)
			break;

		lvItem.iItem = iRows;

		// Count
		lvItem.iSubItem = 0;
		sprintf(szTmp, "%d", (*iterData)->m_lCount);
		lvItem.pszText = szTmp;
		m_csHelpListView.InsertItem(&lvItem);

		lvItem.iSubItem = 1;
		lvItem.pszText = (*iterData)->m_szCharName;
		m_csHelpListView.SetItem(&lvItem);

		lvItem.iSubItem = 2;
		lvItem.pszText = (*iterData)->m_szSubject;
		m_csHelpListView.SetItem(&lvItem);

		lvItem.iSubItem = 3;
		strcpy(szTmp, "");
		// UNIX TimeStamp 로 부터 날짜를 얻어낸다.
		AgcmAdminDlgXT_Manager::Instance()->GetDateTimeByTimeStamp((*iterData)->m_lDate, szTmp);
		lvItem.pszText = szTmp;
		m_csHelpListView.SetItem(&lvItem);

		iterData++; iRows++;
	}

	UpdateData(FALSE);

	UpdateHelpCount();

	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::ClearHelpList()
{
	if(m_listHelp.size() == 0)
		return FALSE;

	list<stAgpdAdminHelp*>::iterator iterData = m_listHelp.begin();
	while(iterData != m_listHelp.end())
	{
		if(*iterData)
			delete *iterData;

		iterData++;
	}

	m_listHelp.clear();

	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::ClearHelpListView()
{
	if(!m_bInitialized)
		return FALSE;
	
	//UpdateData();
	m_csHelpListView.DeleteAllItems();
	return TRUE;
}





//////////////////////////////////////////////////////////////////////////
// Status Bar
BOOL AgcmAdminDlgXT_Main::SetCurrentUserCount(INT32 lCurrentUser)
{
	// 값이 변동이 없으면 Windows Update 를 하지 않는다.
	if(atoi((LPCTSTR)m_szCurrentUserCount) == lCurrentUser)
		return TRUE;

	m_szCurrentUserCount.Format("%d", lCurrentUser);

	return UpdateCurrentUserCount();
}

INT32 AgcmAdminDlgXT_Main::GetCurrentUserCount()
{
	return atoi((LPCTSTR)m_szCurrentUserCount);
}

BOOL AgcmAdminDlgXT_Main::UpdateCurrentUserCount()
{
	if(!m_bInitialized)
		return FALSE;

	// m_szCurrentUserCount 는 SetCurrnetUserCount(...) 에서 세팅해 준다.

	// UpdateData(FALSE 를 전체로 날리면, 전체 데이터가 지워지는 현상이 발생해서,
	// Static 만 다시 그리게 바꿈.

	m_csCurrentUserCount.SetWindowText(m_szCurrentUserCount);
	//UpdateData(FALSE);
	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::UpdateAdminCount()
{
	if(!m_bInitialized)
		return FALSE;

	m_szAdminCount.Format("%d", AgcmAdminDlgXT_Manager::Instance()->GetAdminListDlg()->GetAdminCount());
	UpdateData(FALSE);

	return TRUE;
}

BOOL AgcmAdminDlgXT_Main::UpdateHelpCount()
{
	if(!m_bInitialized)
		return FALSE;

	m_szHelpCount.Format("%d", m_listHelp.size());
	UpdateData(FALSE);

	return TRUE;
}





// Contorl Init
BOOL AgcmAdminDlgXT_Main::OnInitHelpListView()
{
	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 40;
	lvCol.pszText = "No";
	lvCol.iSubItem = 0;
	m_csHelpListView.InsertColumn(0, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 1;
	m_csHelpListView.InsertColumn(1, &lvCol);

	lvCol.cx = 110;
	lvCol.pszText = "Subject";
	lvCol.iSubItem = 2;
	m_csHelpListView.InsertColumn(2, &lvCol);

	lvCol.cx = 100;
	lvCol.pszText = "Date/Time";
	lvCol.iSubItem = 3;
	m_csHelpListView.InsertColumn(3, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	m_csHelpListView.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}







//////////////////////////////////////////////////////////////////////
// Dialog

void AgcmAdminDlgXT_Main::OnSelchangeObjectList()
{
	if(!m_bInitialized)
		return;

	if(!m_pfCBSearch || !AgcmAdminDlgXT_Manager::Instance()->GetCBClass())
		return;

	//UpdateData();

	CString szName;
	if(GetSelectedObjectID(szName) == FALSE)
		return;

	// 이름을 얻어냈으니, 파싱해서 검색을 날린다.
	// ID 로 Object 의 Type 을 알아낸다.
	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	strcpy(stSearch.m_szSearchName, (LPCTSTR)szName);

	// Callback Execute
	m_pfCBSearch(&stSearch, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);

	return;
}

void AgcmAdminDlgXT_Main::OnDblclkLsMainObject() 
{
	// TODO: Add your control notification handler code here
	if(!m_pfCBSearch || !AgcmAdminDlgXT_Manager::Instance()->GetCBClass())
		return;

	//UpdateData();

	CString szName;
	if(GetSelectedObjectID(szName) == FALSE)
		return;

	// 이름을 얻어냈으니, 파싱해서 검색을 날린다.
	// ID 로 Object 의 Type 을 알아낸다.
	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	strcpy(stSearch.m_szSearchName, (LPCTSTR)szName);

	// Character Dialog 의 Last Search 를 세팅한다.
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetLastSearch(&stSearch);

	// Open Char Dialog
	AgcmAdminDlgXT_Manager::Instance()->OpenCharacterDlg();

	// Callback Execute
	m_pfCBSearch(&stSearch, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);
}

void AgcmAdminDlgXT_Main::OnObjectSelect() 
{
	// TODO: Add your command handler code here
	OnDblclkLsMainObject();
}

void AgcmAdminDlgXT_Main::OnObjectDel() 
{
	// TODO: Add your command handler code here
	if(!m_bInitialized)
		return;

	int iIndex = m_csObjectListBox.GetCurSel();
	if(iIndex < 0)
		return;

	CString szName;
	if(GetSelectedObjectID(szName) == FALSE)
		return;

	m_csObjectListBox.DeleteString(iIndex);

	RemoveObject((LPCTSTR)szName);
	//ClearObjectListBox();
	//ShowObjectList();

	m_szInfoText.Empty();
	UpdateData(FALSE);
}

void AgcmAdminDlgXT_Main::OnObjectClear() 
{
	// TODO: Add your command handler code here
	if(!m_bInitialized)
		return;

	ClearObjectList();
	ClearObjectListBox();

	m_szInfoText.Empty();
	UpdateData(FALSE);
}

void AgcmAdminDlgXT_Main::OnToolSearch() 
{
	// TODO: Add your command handler code here
	AgcmAdminDlgXT_Manager::Instance()->OpenSearchDlg();
}

void AgcmAdminDlgXT_Main::OnToolCharacter() 
{
	// TODO: Add your command handler code here
	AgcmAdminDlgXT_Manager::Instance()->OpenCharacterDlg();
}

void AgcmAdminDlgXT_Main::OnToolHelp() 
{
	// TODO: Add your command handler code here
	AgcmAdminDlgXT_Manager::Instance()->OpenHelpDlg();
}

void AgcmAdminDlgXT_Main::OnToolMove() 
{
	// TODO: Add your command handler code here
	AgcmAdminDlgXT_Manager::Instance()->OpenMoveDlg();
}

void AgcmAdminDlgXT_Main::OnHelpSelect() 
{
	// TODO: Add your command handler code here
	if(!m_pfCBHelpProcess || !AgcmAdminDlgXT_Manager::Instance()->GetCBClass())
		return;

	stAgpdAdminHelp stHelp;
	memcpy(&stHelp, GetSelectedHelp(), sizeof(stHelp));

	if(stHelp.m_lCount == 0)
		return;

	m_pfCBHelpProcess(&stHelp, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);
}

void AgcmAdminDlgXT_Main::OnClickLvMainHelp(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	if(!m_bInitialized)
		return;

	//UpdateData();

	LPNMLISTVIEW nlv = (LPNMLISTVIEW)pNMHDR;
	if(nlv->iItem >= 0)
	{
		CString szValue;
		stAgpdAdminHelp stHelp;
		memset(&stHelp, 0, sizeof(stHelp));

		szValue = m_csHelpListView.GetItemText(nlv->iItem, 0);	// Help Count (Index)
		stHelp.m_lCount = atoi((LPCTSTR)szValue);

		szValue = m_csHelpListView.GetItemText(nlv->iItem, 1);	// Char Name
		strcpy(stHelp.m_szCharName, (LPCTSTR)szValue);

		SetSelectedHelp(&stHelp);
	}
	else
		SetSelectedHelp(NULL);
	
	*pResult = 0;
}

void AgcmAdminDlgXT_Main::OnDblclkLvMainHelp(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(!m_bInitialized)
		return;

	if(!m_pfCBHelpProcess)
		return;

	//UpdateData();

	LPNMLISTVIEW nlv = (LPNMLISTVIEW)pNMHDR;
	if(nlv->iItem >= 0)
	{
		CString szValue;
		stAgpdAdminHelp stHelp;
		memset(&stHelp, 0, sizeof(stHelp));

		szValue = m_csHelpListView.GetItemText(nlv->iItem, 0);	// Help Count (Index)
		stHelp.m_lCount = atoi((LPCTSTR)szValue);

		szValue = m_csHelpListView.GetItemText(nlv->iItem, 1);	// Char Name
		strncpy(stHelp.m_szCharName, (LPCTSTR)szValue, AGPACHARACTER_MAX_ID_STRING);

		// 이미 선택한 놈과 같은 건지 비교
		if(GetSelectedHelp()->m_lCount == stHelp.m_lCount)
		{
			m_pfCBHelpProcess(&stHelp, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);

			// Help Window Open
			AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->OpenDlg();
		}
	}
	else
		SetSelectedHelp(NULL);
	
	*pResult = 0;
}

void AgcmAdminDlgXT_Main::OnSelchangeCbMainMovePlace() 
{
	// TODO: Add your control notification handler code here
	if(!m_bInitialized)
		return;

	//UpdateData();

	int iIndex = m_csMovePlaceCB.GetCurSel();
	if(iIndex < 0)
		return;

	m_szMovePC = AgcmAdminDlgXT_Manager::Instance()->GetSelfAdminInfo()->m_szAdminName;

	m_csMovePlaceCB.GetLBText(iIndex, m_szMovePlace);
	if(m_szMovePlace.IsEmpty())
	{
		m_szMoveX = _T("");
		m_szMoveY = _T("");
		m_szMoveZ = _T("");
	}
	else
	{
		AgcdAdminMovePlace* pstMovePlace = AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->GetMovePlace(m_szMovePlace);
		if(pstMovePlace)
		{
			SetPosition(pstMovePlace->m_fX, pstMovePlace->m_fY, pstMovePlace->m_fZ);
		}
	}

	UpdateData(FALSE);
}

void AgcmAdminDlgXT_Main::OnBMainMoveGo() 
{
	// TODO: Add your control notification handler code here
	if(!m_bInitialized)
		return;

	UpdateData();

	// Move Dialog 에 데이터를 세팅하고, Move 시킨다.
	//AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->SetMovePC(m_szMovePC);
	//AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->SetMoveTargetPC(m_szMoveTargetPC);
	//AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->SetPlace(m_szMovePlace);
	//AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->SetPosition(atof((LPCTSTR)m_szMoveX), atof((LPCTSTR)m_szMoveY), atof((LPCTSTR)m_szMoveZ));

	AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->ProcessMove(m_szMovePC, m_szMoveTargetPC,
																							atof((LPCTSTR)m_szMoveX), atof((LPCTSTR)m_szMoveY), atof((LPCTSTR)m_szMoveZ));
}

void AgcmAdminDlgXT_Main::OnToolItem() 
{
	// TODO: Add your command handler code here
	AgcmAdminDlgXT_Manager::Instance()->OpenItemDlg();
}

void AgcmAdminDlgXT_Main::OnToolAdminList() 
{
	// TODO: Add your command handler code here
	AgcmAdminDlgXT_Manager::Instance()->OpenAdminListDlg();
}

int AgcmAdminDlgXT_Main::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex) 
{
	// TODO: Add your message handler code here and/or call default

	if(pListBox == (CListBox*)&m_csObjectListBox)
	{
		if(nKey == VK_DELETE)
		{
			OnObjectDel();
			return -2;
		}
	}
	
	return CDialog::OnVKeyToItem(nKey, pListBox, nIndex);
}

void AgcmAdminDlgXT_Main::OnToolNotice() 
{
	// TODO: Add your command handler code here
	AgcmAdminDlgXT_Manager::Instance()->OpenNoticeDlg();
}
