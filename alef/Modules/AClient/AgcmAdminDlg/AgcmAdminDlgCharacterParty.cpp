// AgcmAdminDlgCharacterParty.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 05.

#include "AgcmAdminDlgInclude.h"
#include "resource.h"

AgcmAdminDlgCharacterParty::AgcmAdminDlgCharacterParty()
{
	m_iResourceID = IDD_CHAR_PARTY;
	m_pfDlgProc = CharacterPartyDlgProc;

	m_pfSearchParty = NULL;
	m_pfReset = NULL;
}

AgcmAdminDlgCharacterParty::~AgcmAdminDlgCharacterParty()
{
	ClearMemberList();
}

BOOL AgcmAdminDlgCharacterParty::SetCBSearchParty(ADMIN_CB pfCallback)
{
	m_pfSearchParty = pfCallback;
	return TRUE;
}

BOOL AgcmAdminDlgCharacterParty::SetCBReset(ADMIN_CB pfCallback)
{
	m_pfReset = pfCallback;
	return TRUE;
}

BOOL AgcmAdminDlgCharacterParty::SetMember(stAgpdAdminCharPartyMember* pstMember)
{
	if(pstMember == NULL)
		return FALSE;

	stAgpdAdminCharPartyMember* pstMemberInList = GetMember(pstMember->m_lCID);
	if(pstMemberInList)	// 이미 데이터가 있으면, 업데이트 해준다.
	{
		memcpy(pstMemberInList, pstMember, sizeof(stAgpdAdminCharPartyMember));
	}
	else	// 새로 넣어준다.
	{
		pstMemberInList = new stAgpdAdminCharPartyMember;
		memcpy(pstMemberInList, pstMember, sizeof(stAgpdAdminCharPartyMember));
		m_listMember.AddTail(pstMemberInList);
	}

	return ShowData();
}

BOOL AgcmAdminDlgCharacterParty::SetMember(stAgpdAdminCharDataSub* pstMemberSub)
{
	if(pstMemberSub == NULL)
		return FALSE;

	stAgpdAdminCharPartyMember* pstMemberInList = GetMember(pstMemberSub->m_lCID);
	if(pstMemberInList)	// 이미 데이터가 있으면, 업데이트 해준다.
	{
		// Sub Data 를 세팅한다....
		strcpy(pstMemberInList->m_szIP, pstMemberSub->m_szIP);
	}

	return ShowData();
}

BOOL AgcmAdminDlgCharacterParty::SetMember(PVOID pList)
{
	AuList<stAgpdAdminCharPartyMember*>* pcsList = (AuList<stAgpdAdminCharPartyMember*>*)pList;
	if(pcsList == NULL)
		return FALSE;

	ClearMemberList();
	ClearMemberListView();

	AuNode<stAgpdAdminCharPartyMember*>* pcNode = pcsList->GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData() == NULL)
			break;

		stAgpdAdminCharPartyMember* pstMember = new stAgpdAdminCharPartyMember;
		memcpy(pstMember, pcNode->GetData(), sizeof(stAgpdAdminCharPartyMember));
		m_listMember.AddTail(pstMember);
		
		pcNode = pcNode->GetNextNode();
	}

	return TRUE;
}

stAgpdAdminCharPartyMember* AgcmAdminDlgCharacterParty::GetMember(INT32 lCID)
{
	if(m_listMember.GetCount() <= 0)
		return NULL;

	AuNode<stAgpdAdminCharPartyMember*>* pcNode = m_listMember.GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData()->m_lCID == lCID)
			return pcNode->GetData();

		pcNode = pcNode->GetNextNode();
	}

	return NULL;
}

BOOL AgcmAdminDlgCharacterParty::ShowData(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	return TRUE;
}

BOOL AgcmAdminDlgCharacterParty::ShowBasicData(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hBasicInfo = GetDlgItem(hDlg, IDC_LV_CHAR_PARTY_BASIC);
	if(!hBasicInfo)
		return FALSE;

	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->ShowBasicInfo(hBasicInfo);;
}

BOOL AgcmAdminDlgCharacterParty::ClearMemberList()
{
	if(m_listMember.IsEmpty() == FALSE)
	{
		AuNode<stAgpdAdminCharPartyMember*>* pcNode = NULL;
		AuNode<stAgpdAdminCharPartyMember*>* pcNode2 = NULL;

		pcNode = m_listMember.GetHeadNode();
		while(pcNode)
		{
			if(pcNode->GetData())
			{
				delete pcNode->GetData();

				pcNode2 = pcNode->GetNextNode();
				m_listMember.RemoveNode(pcNode);
				pcNode = pcNode2;
			}
			else
				break;
		}
	}

	return TRUE;
}

BOOL AgcmAdminDlgCharacterParty::ClearMemberListView(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hList = GetDlgItem(hDlg, IDC_LV_CHAR_PARTY_LIST);
	if(!hList)
		return FALSE;

	return ListView_DeleteAllItems(hList);
}

LRESULT CALLBACK AgcmAdminDlgCharacterParty::CharacterPartyDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_NOTIFY:
			switch(((LPNMHDR)lParam)->code)
			{
				// Property Sheet
				case PSN_SETACTIVE:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->OnInitDialog(hDlg, wParam, lParam);

				case PSN_APPLY:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->CloseDlg(GetParent(hDlg));
			}
			return TRUE;
			
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_BT_CHAR_PARTY_REFRESH:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->OnRefreshBtn(hDlg);

				case IDC_BT_CHAR_PARTY_RESET:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->OnResetBtn(hDlg);
			}
			return FALSE;

		// Property Page 용 Dialog 에는 WM_CLOSE 가 없다.
	}

	return FALSE;
}

LRESULT AgcmAdminDlgCharacterParty::OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if(GetInitialized())
	{
		ShowBasicData(hDlg);
		ShowData(hDlg);

		return TRUE;
	}

	// Property Sheet 에서 열어주므로, m_hDlg 는 NULL 이다.
	// 그래서 이렇게 초기화 할때 세팅해준다.
	m_hDlg = hDlg;

	InitBasicInfoLV(hDlg);
	InitPartyLV(hDlg);

	SetInitialized(TRUE);

	ShowBasicData(hDlg);
	ShowData(hDlg);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterParty::InitBasicInfoLV(HWND hDlg)
{
	HWND hBasicInfo = GetDlgItem(hDlg, IDC_LV_CHAR_PARTY_BASIC);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hBasicInfo, 0, &lvCol);

	lvCol.cx = 70;
	lvCol.pszText = "CharNo";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hBasicInfo, 1, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "AccName";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hBasicInfo, 2, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "Name";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hBasicInfo, 3, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Race";
	lvCol.iSubItem = 4;
	ListView_InsertColumn(hBasicInfo, 4, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Class";
	lvCol.iSubItem = 5;
	ListView_InsertColumn(hBasicInfo, 5, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Lev";
	lvCol.iSubItem = 6;
	ListView_InsertColumn(hBasicInfo, 6, &lvCol);

	lvCol.cx = 100;
	lvCol.pszText = "UT";
	lvCol.iSubItem = 7;
	ListView_InsertColumn(hBasicInfo, 7, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "CreDate";
	lvCol.iSubItem = 8;
	ListView_InsertColumn(hBasicInfo, 8, &lvCol);

	lvCol.cx = 120;
	lvCol.pszText = "IP";
	lvCol.iSubItem = 9;
	ListView_InsertColumn(hBasicInfo, 9, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "Status";
	lvCol.iSubItem = 10;
	ListView_InsertColumn(hBasicInfo, 10, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hBasicInfo, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterParty::InitPartyLV(HWND hDlg)
{
	HWND hParty = GetDlgItem(hDlg, IDC_LV_CHAR_PARTY_LIST);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hParty, 0, &lvCol);

	lvCol.cx = 70;
	lvCol.pszText = "CharNo";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hParty, 1, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "ID";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hParty, 2, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "Name";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hParty, 3, &lvCol);

	lvCol.cx = 40;
	lvCol.pszText = "Race";
	lvCol.iSubItem = 4;
	ListView_InsertColumn(hParty, 4, &lvCol);

	lvCol.cx = 40;
	lvCol.pszText = "Class";
	lvCol.iSubItem = 5;
	ListView_InsertColumn(hParty, 5, &lvCol);

	lvCol.cx = 30;
	lvCol.pszText = "Lev";
	lvCol.iSubItem = 6;
	ListView_InsertColumn(hParty, 6, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "UT";
	lvCol.iSubItem = 7;
	ListView_InsertColumn(hParty, 7, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "CreDate";
	lvCol.iSubItem = 8;
	ListView_InsertColumn(hParty, 8, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "IP";
	lvCol.iSubItem = 9;
	ListView_InsertColumn(hParty, 9, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hParty, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterParty::OnRefreshBtn(HWND hDlg)
{
	if(!m_pcsCBClass || !m_pfSearchParty)
		return FALSE;

	stAgpdAdminCharData* pstCharData = AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetCharData();
	stAgpdAdminCharParty stParty;
	memset(&stParty, 0, sizeof(stParty));

	stParty.m_lCID = pstCharData->m_stBasic.m_lCID;
	strcpy(stParty.m_szCharName, pstCharData->m_stBasic.m_szCharName);
	
	m_pfSearchParty(&stParty, m_pcsCBClass, NULL);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterParty::OnResetBtn(HWND hDlg)
{
	return TRUE;
}