// AgcmAdminDlgMain.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 15.

#include "AgcmAdminDlgInclude.h"
#include "resource.h"

#define INFO_TEXT_SIZE	512
#define HELP_CANNOT_PROCESS_STR		"현재 진행중인 진정이 있습니다"

AgcmAdminDlgMain::AgcmAdminDlgMain()
{
	m_iResourceID = IDD_MAIN;
	m_pfDlgProc = MainDlgProc;

	m_pfCBSearch = NULL;
	m_pfCBHelpProcess = NULL;
	m_pfCBHelpRequest = NULL;

	SetProcessHelp(NULL);

	memset(&m_stCharData, 0, sizeof(m_stCharData));
	m_szInfoText = new char[INFO_TEXT_SIZE+1];
	
	InitializeCriticalSection(&m_csHelpLock);
}

AgcmAdminDlgMain::~AgcmAdminDlgMain()
{
	delete [] m_szInfoText;

	ClearObjectList();
	ClearHelpList();

	DeleteCriticalSection(&m_csHelpLock);
}

BOOL AgcmAdminDlgMain::SetCBSearch(ADMIN_CB pfCBSearch)
{
	m_pfCBSearch = pfCBSearch;
	return TRUE;
}

BOOL AgcmAdminDlgMain::SetCBHelpProcess(ADMIN_CB pfCBHelpProcess)
{
	m_pfCBHelpProcess = pfCBHelpProcess;
	return TRUE;
}

BOOL AgcmAdminDlgMain::SetCBHelpRequest(ADMIN_CB pfCBHelpRequest)
{
	m_pfCBHelpRequest = pfCBHelpRequest;
	return TRUE;
}

void AgcmAdminDlgMain::HelpLock()
{
	EnterCriticalSection(&m_csHelpLock);
}

void AgcmAdminDlgMain::HelpUnlock()
{
	LeaveCriticalSection(&m_csHelpLock);
}








/////////////////////////////////////////////////////////////////////////////
// Object List
BOOL AgcmAdminDlgMain::AddObject(stAgpdAdminPickingData* pstPickingData)
{
	if(pstPickingData)
	{
		// 먼저 들어가 있으면 지운다.
		DeleteObject(pstPickingData);
		
		stAgpdAdminPickingData* pstNewPickingData = new stAgpdAdminPickingData;
		memcpy(pstNewPickingData, pstPickingData, sizeof(stAgpdAdminPickingData));

		m_listObject.AddTail(pstNewPickingData);

		// 훗훗
		ClearObjectListBox();
		ShowObjectList();
	}

	return TRUE;
}

BOOL AgcmAdminDlgMain::GetSelectedObjectID(HWND hDlg, LPCTSTR szName)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hObjectList = GetDlgItem(hDlg, IDC_LS_MAIN_OBJECT);
	if(hObjectList == NULL) return FALSE;

	INT16 lIndex = (INT16)SendMessage(hObjectList, LB_GETCURSEL, 0, 0);
	if(lIndex == LB_ERR) return FALSE;

	if(SendMessage(hObjectList, LB_GETTEXT, lIndex, (LPARAM)szName) == LB_ERR)
		return FALSE;

	return TRUE;
}

BOOL AgcmAdminDlgMain::DeleteObject(stAgpdAdminPickingData* pstPickingData)
{
	if(!pstPickingData)
		return FALSE;

	if(m_listObject.IsEmpty())
		return FALSE;

	AuNode<stAgpdAdminPickingData*>* pcNode = m_listObject.GetHeadNode();
	AuNode<stAgpdAdminPickingData*>* pcNode2 = NULL;
	while(pcNode)
	{
		if(pcNode->GetData())
		{
			if(pcNode->GetData()->m_lCID == pstPickingData->m_lCID ||
				strcmp(pcNode->GetData()->m_szName, pstPickingData->m_szName) == 0)
			{
				pcNode2 = pcNode->GetNextNode();
				delete pcNode->GetData();
				m_listObject.RemoveNode(pcNode);
				pcNode = pcNode2;

				continue;
			}
		}

		pcNode = pcNode->GetNextNode();
	}

	return TRUE;
}

BOOL AgcmAdminDlgMain::ClearObjectList()
{
	if(m_listObject.IsEmpty())
		return FALSE;

	AuNode<stAgpdAdminPickingData*>* pcNode = m_listObject.GetHeadNode();
	AuNode<stAgpdAdminPickingData*>* pcNode2 = NULL;
	while(pcNode)
	{
		if(pcNode->GetData())
		{
			pcNode2 = pcNode->GetNextNode();
			delete pcNode->GetData();
			m_listObject.RemoveNode(pcNode);
			pcNode = pcNode2;

			continue;
		}

		pcNode = pcNode->GetNextNode();
	}

	return TRUE;
}

BOOL AgcmAdminDlgMain::ClearObjectListBox(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hObjectList = GetDlgItem(hDlg, IDC_LS_MAIN_OBJECT);
	if(!hObjectList)
		return FALSE;

	SendMessage(hObjectList, LB_RESETCONTENT, 0, 0);

	return TRUE;
}

BOOL AgcmAdminDlgMain::ShowObjectList(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	if(m_listObject.IsEmpty())
	{
		ClearObjectListBox(hDlg);
		return TRUE;
	}

	HWND hObjectList = GetDlgItem(hDlg, IDC_LS_MAIN_OBJECT);
	if(!hObjectList)
		return FALSE;

	AuNode<stAgpdAdminPickingData*>* pcNode = m_listObject.GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData())
			SendMessage(hObjectList, LB_ADDSTRING, 0, (LPARAM)pcNode->GetData()->m_szName);

		pcNode = pcNode->GetNextNode();
	}

	return TRUE;
}










///////////////////////////////////////////////////////////////////////////////////
// Info Text

BOOL AgcmAdminDlgMain::SetInfoCharData(stAgpdAdminCharData* pstCharData)
{
	if(!pstCharData)
		return FALSE;

	// 선택한 Object 인지 확인한다.
	if(!GetSelectedObjectID(m_hDlg, pstCharData->m_stBasic.m_szCharName))
	{
		// 그냥 Skip 한다.
		return TRUE;
	}

	// 여기서는 memcpy 만 하고, 화면에 뿌리지는 않는다.
	// 화면에 뿌리는 시점은 Sub 정보를 받았을 때이다.
	memcpy(&m_stCharData, pstCharData, sizeof(m_stCharData));

	return TRUE;
}

BOOL AgcmAdminDlgMain::SetInfoCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub)
{
	// 받은 데이터가 선택한 Object 인지 확인해서..
	if(pstCharDataSub && m_stCharData.m_stBasic.m_lCID == pstCharDataSub->m_lCID)
		memcpy(&m_stCharData.m_stSub, pstCharDataSub, sizeof(m_stCharData.m_stSub));
	else
		memset(&m_stCharData.m_stSub, 0, sizeof(m_stCharData.m_stSub));

	return ShowInfoText();
}

BOOL AgcmAdminDlgMain::SetInfoText(LPCTSTR szName, LPCTSTR szText)
{
	if(!szName)
		return FALSE;

	if(strlen(szName) == 0)
	{
		SetDlgItemText(m_hDlg, IDC_E_MAIN_INFO, "");
		return TRUE;
	}

	char szSelectedName[255];
	if(GetSelectedObjectID(m_hDlg, szSelectedName) == FALSE)
		return FALSE;

	if(strcmp(szSelectedName, szName) != 0)
		return FALSE;

	strcpy(m_szInfoText, szText);
	SetDlgItemText(m_hDlg, IDC_E_MAIN_INFO, m_szInfoText);
	
	return TRUE;
}

BOOL AgcmAdminDlgMain::ClearInfoText()
{
	strcpy(m_szInfoText, "");
	return TRUE;
}

BOOL AgcmAdminDlgMain::ShowInfoText(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	if(m_stCharData.m_stBasic.m_lCID == 0 ||
		strlen(m_stCharData.m_stBasic.m_szCharName) == 0)
	{
		SetDlgItemText(hDlg, IDC_E_MAIN_INFO, "");
		return TRUE;
	}

	strcpy(m_szInfoText, "");
	char szTmp[64];

	// 선택한 것이 없을 때는 지운다.
	if(GetSelectedObjectID(hDlg, szTmp) == FALSE)
	{
		SetDlgItemText(hDlg, IDC_E_MAIN_INFO, "");
		return TRUE;
	}
	
	// CharNo
	wsprintf(szTmp, "CharNo:%d ", m_stCharData.m_stBasic.m_lCID);
	strcat(m_szInfoText, szTmp);

	// Acc
	wsprintf(szTmp, " AccID:%s ", m_stCharData.m_stSub.m_szAccName);
	strcat(m_szInfoText, szTmp);

	// Name
	wsprintf(szTmp, " Name:%s ", m_stCharData.m_stSub.m_szName);
	strcat(m_szInfoText, szTmp);

	// Race
	wsprintf(szTmp, "Race:%s ", m_stCharData.m_stStatus.m_szRace);
	strcat(m_szInfoText, szTmp);

	// Class
	wsprintf(szTmp, "Class:%s ", m_stCharData.m_stStatus.m_szClass);
	strcat(m_szInfoText, szTmp);

	// UT
	wsprintf(szTmp, "UT:%s ", "UT");
	strcat(m_szInfoText, szTmp);

	// CreDate
	wsprintf(szTmp, "CreDate:%s ", "만든날짜");
	strcat(m_szInfoText, szTmp);

	// Login
	wsprintf(szTmp, "Log-in:%s ", m_stCharData.m_stSub.m_szLogin);
	strcat(m_szInfoText, szTmp);

	// Playtime
	wsprintf(szTmp, "PlayTime:%d ", m_stCharData.m_stSub.m_lPlayTime);
	strcat(m_szInfoText, szTmp);

	// Status
	wsprintf(szTmp, "Status:%d ", m_stCharData.m_stBasic.m_lCID);	// 아직 Status 필드가 없다??
	strcat(m_szInfoText, szTmp);

	// XYZ
	sprintf(szTmp, "XYZ:%.0f, %.0f, %.0f ",
						m_stCharData.m_stBasic.m_stPos.x,
						m_stCharData.m_stBasic.m_stPos.y,
						m_stCharData.m_stBasic.m_stPos.z);	// 음.. 
	strcat(m_szInfoText, szTmp);

	// Heading
	wsprintf(szTmp, "Heading:%d ", m_stCharData.m_stStatus.m_lMovement);
	strcat(m_szInfoText, szTmp);

	// IP
	wsprintf(szTmp, "IP:%s ", m_stCharData.m_stSub.m_szIP);
	strcat(m_szInfoText, szTmp);

	// InvGhelld
	wsprintf(szTmp, "InvGhelld:0 ");		// 돈 체크해주어야 한다.
	strcat(m_szInfoText, szTmp);

	// Bank Ghelld
	wsprintf(szTmp, "BankGhelld:0 ");
	strcat(m_szInfoText, szTmp);

	// EXP
	wsprintf(szTmp, "EXP:%d ", m_stCharData.m_stPoint.m_lEXP);
	strcat(m_szInfoText, szTmp);

	// LEV
	wsprintf(szTmp, "LEV:%d ", m_stCharData.m_stStatus.m_lLevel);
	strcat(m_szInfoText, szTmp);

	// HP
	wsprintf(szTmp, "HP:%d(%d) ", m_stCharData.m_stPoint.m_lHP, m_stCharData.m_stPoint.m_lMaxHP);
	strcat(m_szInfoText, szTmp);

	// MP
	wsprintf(szTmp, "MP:%d(%d) ", m_stCharData.m_stPoint.m_lMP, m_stCharData.m_stPoint.m_lMaxMP);
	strcat(m_szInfoText, szTmp);

	// SP
	wsprintf(szTmp, "SP:%d(%d) ", m_stCharData.m_stPoint.m_lSP, m_stCharData.m_stPoint.m_lMaxSP);
	strcat(m_szInfoText, szTmp);

	// STR
	wsprintf(szTmp, "STR:%d ", m_stCharData.m_stStatus.m_lSTR);
	strcat(m_szInfoText, szTmp);

	// DEX
	wsprintf(szTmp, "DEX:%d ", m_stCharData.m_stStatus.m_lDEX);
	strcat(m_szInfoText, szTmp);

	// CON
	wsprintf(szTmp, "CON:%d ", m_stCharData.m_stStatus.m_lCON);
	strcat(m_szInfoText, szTmp);

	// WIS
	wsprintf(szTmp, "WIS:%d ", m_stCharData.m_stStatus.m_lWIS);
	strcat(m_szInfoText, szTmp);

	// INT
	wsprintf(szTmp, "INT:%d ", m_stCharData.m_stStatus.m_lINT);
	strcat(m_szInfoText, szTmp);

	// AC
	wsprintf(szTmp, "AC:%d ", m_stCharData.m_stPoint.m_lAP);	// AC 가 왜 없지. -_-;
	strcat(m_szInfoText, szTmp);

	// AP
	wsprintf(szTmp, "AP:%d ", m_stCharData.m_stPoint.m_lAP);	// 후후후
	strcat(m_szInfoText, szTmp);

	// Magic AP
	wsprintf(szTmp, "Magic AP:%d ", m_stCharData.m_stPoint.m_lMaxMAP);
	strcat(m_szInfoText, szTmp);

	// Min Dmg
	wsprintf(szTmp, "MinDMG:0 ");	// 음...
	strcat(m_szInfoText, szTmp);

	// Max Dmg
	wsprintf(szTmp, "MaxDMG:0 ");	// 으음...
	strcat(m_szInfoText, szTmp);

	// Murderer Point
	wsprintf(szTmp, "MurP:%d ", m_stCharData.m_stStatus.m_lMurderer);
	strcat(m_szInfoText, szTmp);

	// Fire
	wsprintf(szTmp, "Fire:%d ", m_stCharData.m_stAttribute.m_lFire);
	strcat(m_szInfoText, szTmp);

	// Water
	wsprintf(szTmp, "Water:%d ", m_stCharData.m_stAttribute.m_lWater);
	strcat(m_szInfoText, szTmp);

	// Earth
	wsprintf(szTmp, "Earth:%d ", m_stCharData.m_stAttribute.m_lEarth);
	strcat(m_szInfoText, szTmp);

	// Wind
	wsprintf(szTmp, "Wind:%d ", m_stCharData.m_stAttribute.m_lAir);
	strcat(m_szInfoText, szTmp);

	// Magic
	wsprintf(szTmp, "Magic:%d ", m_stCharData.m_stAttribute.m_lMagic);
	strcat(m_szInfoText, szTmp);

	
	////////////////////////////////////////////////////////////////////////////////////
	// 휴.. 완료.
	SetDlgItemText(m_hDlg, IDC_E_MAIN_INFO, m_szInfoText);

	return TRUE;
}















//////////////////////////////////////////////////////////////////////////////
// Help
BOOL AgcmAdminDlgMain::AddHelp(stAgpdAdminHelp* pstHelp)
{
	if(GetHelp(pstHelp->m_lCount))
		return FALSE;

	stAgpdAdminHelp* pstNewHelp = new stAgpdAdminHelp;
	memcpy(pstNewHelp, pstHelp, sizeof(stAgpdAdminHelp));
	m_listHelp.AddTail(pstNewHelp);

	// 화면을 비우고
	ClearHelpListView();

	return ShowHelpList();
}

BOOL AgcmAdminDlgMain::ProcessHelp(stAgpdAdminHelp* pstHelp)
{
	// Remove Help
	RemoveHelp(pstHelp);

	// 이게 필요한 것인지 아닌지 잘 모르겠다.....
	// 확인 안해도 될듯 한데.. Help 다이얼로그에서만 처리해도 될듯 한데.. -0-
	// 넘어온 pstHelp 가 자기가 처리할 것인지 확인한다.
	if(pstHelp->m_lAdminCID == AgcmAdminDlgManager::Instance()->GetSelfAdminInfo()->m_lCID)
	{
		SetProcessHelp(pstHelp);
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmAdminDlgMain::RemoveHelp(stAgpdAdminHelp* pstHelp)
{
	if(m_listHelp.IsEmpty())
		return FALSE;

	AuNode<stAgpdAdminHelp*>* pcNode = m_listHelp.GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData() && pcNode->GetData()->m_lCount == pstHelp->m_lCount)
		{
			delete pcNode->GetData();	// 이걸 해야 되는 지 말아야 되는지 고민중... -0-
			m_listHelp.RemoveNode(pcNode);
			break;
		}
		pcNode = pcNode->GetNextNode();
	}

	// 화면을 비운다.
	ClearHelpListView();

	return ShowHelpList();
}

BOOL AgcmAdminDlgMain::SetProcessHelp(stAgpdAdminHelp* pstHelp)
{
	if(pstHelp)
		memcpy(&m_stProcessHelp, pstHelp, sizeof(m_stProcessHelp));
	else
		memset(&m_stProcessHelp, 0, sizeof(m_stProcessHelp));

	return TRUE;
}

stAgpdAdminHelp* AgcmAdminDlgMain::GetProcessHelp()
{
	return &m_stProcessHelp;
}

stAgpdAdminHelp* AgcmAdminDlgMain::GetHelp(INT32 lHelpID)
{
	if(lHelpID < 0)
		return NULL;

	if(m_listHelp.IsEmpty())
		return NULL;

	AuNode<stAgpdAdminHelp*>* pcNode = m_listHelp.GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData() && pcNode->GetData()->m_lCount == lHelpID)
			return pcNode->GetData();

		pcNode = pcNode->GetNextNode();
	}

	return NULL;
}

BOOL AgcmAdminDlgMain::RequestHelp()
{
	char szSubject[255];
	GetDlgItemText(m_hDlg, IDC_E_MAIN_INFO, szSubject, 254);

	stAgpdAdminHelp stHelp;
	memset(&stHelp, 0, sizeof(stHelp));

	strcpy(stHelp.m_szSubject, szSubject);

	if(strlen(stHelp.m_szSubject) == 0)
	{
		MessageBox(m_hDlg, "진정 내용을 입력하세요.", "정보", MB_OK);
		return TRUE;
	}

	m_pfCBHelpRequest(&stHelp, m_pcsCBClass, NULL);

	return TRUE;
}

BOOL AgcmAdminDlgMain::ShowHelpList(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hList = GetDlgItem(hDlg, IDC_LV_MAIN_HELP);
	if(!hList)
		return FALSE;

	if(m_listHelp.IsEmpty())
		return FALSE;

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	int iRows = 0;
	AuNode<stAgpdAdminHelp*>* pcNode = m_listHelp.GetTailNode();
	while(pcNode)
	{
		if(pcNode->GetData() == NULL)
			break;

		lvItem.iItem = iRows;

		// Count
		lvItem.iSubItem = 0;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lCount);
		lvItem.pszText = szTmp;
		ListView_InsertItem(hList, &lvItem);

		lvItem.iSubItem = 1;
		lvItem.pszText = pcNode->GetData()->m_szCharName;
		ListView_SetItem(hList, &lvItem);

		lvItem.iSubItem = 2;
		lvItem.pszText = pcNode->GetData()->m_szSubject;
		ListView_SetItem(hList, &lvItem);

		lvItem.iSubItem = 3;
		strcpy(szTmp, "");
		// UNIX TimeStamp 로 부터 날짜를 얻어낸다.
		AgcmAdminDlgManager::Instance()->GetDateTimeByTimeStamp(pcNode->GetData()->m_lDate, szTmp);
		lvItem.pszText = szTmp;
		ListView_SetItem(hList, &lvItem);

		iRows++;
		pcNode = pcNode->GetPrevNode();
	}

	return TRUE;
}

BOOL AgcmAdminDlgMain::ClearHelpList()
{
	if(!m_listHelp.IsEmpty())
	{
		AuNode<stAgpdAdminHelp*>* pcNode = NULL;
		AuNode<stAgpdAdminHelp*>* pcNode2 = NULL;

		pcNode = m_listHelp.GetHeadNode();
		while(pcNode)
		{
			if(pcNode->GetData())
			{
				delete pcNode->GetData();

				pcNode2 = pcNode->GetNextNode();
				m_listHelp.RemoveNode(pcNode);
				pcNode = pcNode2;
			}
			else
				break;
		}
	}

	return TRUE;
}

BOOL AgcmAdminDlgMain::ClearHelpListView(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hList = GetDlgItem(hDlg, IDC_LV_MAIN_HELP);
	if(!hList)
		return FALSE;

	return ListView_DeleteAllItems(hList);
}





// Dialog Procedure
LRESULT CALLBACK AgcmAdminDlgMain::MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
			return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnInitDialog(hDlg, wParam, lParam);

		case WM_ACTIVATE:
			return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnActivate(hDlg, wParam, lParam);

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				// Menu
				case IDM_FILE_CLOSE:
					return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnCloseMenu(hDlg);
				case IDM_TOOL_SEARCH:
					return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnSearchMenu(hDlg);
				case IDM_TOOL_CHARACTER:
					return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnCharacterMenu(hDlg);
				case IDM_TOOL_HELP:
					return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnHelpMenu(hDlg);

				// Object List
				case IDC_LS_MAIN_OBJECT:
					if(HIWORD(wParam) == LBN_SELCHANGE)
						return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnObjectListClk(hDlg);
					else if(HIWORD(wParam) == LBN_DBLCLK)
						return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnObjectListDblClk(hDlg);

					break;

				// Button
				case IDC_BTN_MAIN_DEL:
					return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnDelBtn(hDlg);
				case IDC_BTN_MAIN_CLEAR:
					return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnClearBtn(hDlg);
				case IDC_BTN_MAIN_GO:
					return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnGoBtn(hDlg);
				case IDC_BTN_MAIN_MODIFY:
					return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnModifyBtn(hDlg);
				case IDC_BTN_MAIN_HELP:
					return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnHelpBtn(hDlg);
			}
			return FALSE;

		case WM_NOTIFY:
		{
			LPNMHDR hdr;
			LPNMLISTVIEW nlv;
			hdr = (LPNMHDR)lParam;
			nlv = (LPNMLISTVIEW)lParam;
			HWND hList = GetDlgItem(hDlg, IDC_LV_MAIN_HELP);

			if(hdr != NULL && hdr->hwndFrom == hList)
			{
				switch(hdr->code)
				{
					case NM_DBLCLK:
						return AgcmAdminDlgManager::Instance()->GetMainDlg()->OnHelpListDblClk(hList, hdr, nlv);
				}
			}

			return TRUE;
		}

		case WM_CLOSE:
			return AgcmAdminDlgManager::Instance()->GetMainDlg()->CloseDlg(hDlg);
	}

	return FALSE;
}

// Init Dialog
LRESULT AgcmAdminDlgMain::OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if(!GetInitialized())
	{
		::InitCommonControls();

		::ShowCursor(TRUE);

		OnInitHelpListView(hDlg);
	}

	// 비우고 출력
	ClearObjectListBox(hDlg);
	ClearInfoText();
	ShowInfoText(hDlg);

	// Data 출력
	ShowData(hDlg);
	ShowHelpList(hDlg);

	::SetForegroundWindow(hDlg);

	return TRUE;
}

// Init Help List View
LRESULT AgcmAdminDlgMain::OnInitHelpListView(HWND hDlg)
{
	HWND hHelpList = GetDlgItem(hDlg, IDC_LV_MAIN_HELP);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 40;
	lvCol.pszText = "No";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hHelpList, 0, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hHelpList, 1, &lvCol);

	lvCol.cx = 110;
	lvCol.pszText = "Subject";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hHelpList, 2, &lvCol);

	lvCol.cx = 90;
	lvCol.pszText = "Date/Time";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hHelpList, 3, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hHelpList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

// Close Dialog - From Menu
LRESULT AgcmAdminDlgMain::OnCloseMenu(HWND hDlg)
{
	return CloseDlg(hDlg);
}

// Open Search Dialog - From Menu
LRESULT AgcmAdminDlgMain::OnSearchMenu(HWND hDlg)
{
	return AgcmAdminDlgManager::Instance()->OpenSearchDlg();
}

// Open Character Dialog - From Menu
LRESULT AgcmAdminDlgMain::OnCharacterMenu(HWND hDlg)
{
	return AgcmAdminDlgManager::Instance()->OpenCharacterDlg();
}

// Open Help Dialog - From Menu
LRESULT AgcmAdminDlgMain::OnHelpMenu(HWND hDlg)
{
	return AgcmAdminDlgManager::Instance()->OpenHelpDlg();
}

// Del Button
LRESULT AgcmAdminDlgMain::OnDelBtn(HWND hDlg)
{
	HWND hObjectList = GetDlgItem(hDlg, IDC_LS_MAIN_OBJECT);
	if(hObjectList == NULL) return FALSE;

	INT16 lIndex = (INT16)SendMessage(hObjectList, LB_GETCURSEL, 0, 0);
	if(lIndex == LB_ERR) return FALSE;

	stAgpdAdminPickingData stPickingData;
	memset(&stPickingData, 0, sizeof(stPickingData));

	GetSelectedObjectID(hDlg, stPickingData.m_szName);

	// List 를 비운다.
	SendMessage(hObjectList, LB_DELETESTRING, lIndex, 0);
	DeleteObject(&stPickingData);

	// Info Text 를 비운다.
	ClearInfoText();
	SetDlgItemText(hDlg, IDC_E_MAIN_INFO, "");

	return TRUE;
}

// Clear Button
LRESULT AgcmAdminDlgMain::OnClearBtn(HWND hDlg)
{
	HWND hObjectList = GetDlgItem(hDlg, IDC_LS_MAIN_OBJECT);
	if(hObjectList == NULL) return FALSE;

	// List 를 비운다.
	SendMessage(hObjectList, LB_RESETCONTENT, 0, 0);
	ClearObjectList();

	// Info Text 를 비운다.
	ClearInfoText();
	SetDlgItemText(hDlg, IDC_E_MAIN_INFO, "");
	
	return TRUE;
}

// Go Button
LRESULT AgcmAdminDlgMain::OnGoBtn(HWND hDlg)
{
	HWND hGoBtn = GetDlgItem(hDlg, IDC_BTN_MAIN_GO);

	char szName[64];
	memset(szName, 0, 64);

	GetSelectedObjectID(hDlg, szName);

	// 일단 내(자신, Admin)가 간다.
	AgcmAdminDlgManager::Instance()->GetMoveDlg()->SetParentHwnd(hGoBtn);
	if(strlen(szName) == 0)
		AgcmAdminDlgManager::Instance()->GetMoveDlg()->SetSelfMove(TRUE);
	else
	{
		AgcmAdminDlgManager::Instance()->GetMoveDlg()->SetSelfMove(FALSE);
		AgcmAdminDlgManager::Instance()->GetMoveDlg()->SetName(szName);
	}

	AgcmAdminDlgManager::Instance()->OpenMoveDlg();
	
	return TRUE;
}

// Modify Button
LRESULT AgcmAdminDlgMain::OnModifyBtn(HWND hDlg)
{
	return TRUE;
}

// Help Button - For Debug
LRESULT AgcmAdminDlgMain::OnHelpBtn(HWND hDlg)
{
	RequestHelp();

	return TRUE;
}

// Object List Select
LRESULT AgcmAdminDlgMain::OnObjectListClk(HWND hDlg)
{
	char szObject[255];
	if(GetSelectedObjectID(hDlg, szObject) == FALSE)
		return FALSE;

	// 이름을 얻어냈으니, 파싱해서 검색을 날린다.
	// ID 로 Object 의 Type 을 알아낸다.
	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	stSearch.m_iType = 0;
	stSearch.m_iField = 0;
	strcpy(stSearch.m_szSearchName, szObject);

	// Callback Execute
	m_pfCBSearch(&stSearch, m_pcsCBClass, NULL);

	return TRUE;
}

// Object List Double Click
LRESULT AgcmAdminDlgMain::OnObjectListDblClk(HWND hDlg)
{
	if(!m_pfCBSearch)
		return FALSE;

	char szObject[255];
	if(GetSelectedObjectID(hDlg, szObject) == FALSE)
		return FALSE;

	// 이름을 얻어냈으니, 파싱해서 검색을 날린다.
	// ID 로 Object 의 Type 을 알아낸다.
	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	stSearch.m_iType = 0;
	stSearch.m_iField = 0;
	strcpy(stSearch.m_szSearchName, szObject);

	// 콜백 호출 전에, Last Search 를 세팅한다.
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->SetLastSearch(&stSearch);

	// Callback Execute
	m_pfCBSearch(&stSearch, m_pcsCBClass, NULL);

	// Type 을 보고, 띄울 창을 결정한다.
	switch(stSearch.m_iType)
	{
		case 0:	// PC Character
			AgcmAdminDlgManager::Instance()->OpenCharacterDlg();
			break;
	}

	return TRUE;
}

// Help List View Double Click
LRESULT AgcmAdminDlgMain::OnHelpListDblClk(HWND hHelpList, LPNMHDR hdr, LPNMLISTVIEW nlv)
{
	if(!m_pfCBHelpProcess)
		return FALSE;

	// 현재 처리중인 Help 가 있으면 안된다.
	stAgpdAdminHelp* pstHelp = AgcmAdminDlgManager::Instance()->GetHelpDlg()->GetProcessHelp();
	if(!pstHelp)
		return FALSE;

	if(pstHelp->m_lCount != 0)
	{
		MessageBox(m_hDlg, HELP_CANNOT_PROCESS_STR, "Info", MB_OK);
		return FALSE;
	}

	char szBuf[64];
	stAgpdAdminHelp stHelp;
	ZeroMemory(&stHelp, sizeof(stHelp));

	// Count 와 이름을 얻어서 보낸다. Admin ID 세팅은 AgcmAdmin 에서 한다.
	ListView_GetItemText(hHelpList, nlv->iItem, 0, szBuf, 63);
	stHelp.m_lCount = atoi(szBuf);
	ListView_GetItemText(hHelpList, nlv->iItem, 1, stHelp.m_szCharName, AGPACHARACTER_MAX_ID_STRING);

	// Callback Execute
	m_pfCBHelpProcess(&stHelp, m_pcsCBClass, NULL);

	return TRUE;
}