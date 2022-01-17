// AgcmAdminDlgCharacterSkill.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 05.

#include "AgcmAdminDlgInclude.h"
#include "resource.h"

AgcmAdminDlgCharacterSkill::AgcmAdminDlgCharacterSkill()
{
	m_iResourceID = IDD_CHAR_SKILL;
	m_pfDlgProc = CharacterSkillDlgProc;

	m_pfSearchSkill = NULL;
	m_pfRollback = NULL;
	m_pfSave = NULL;	// 아직 쓸지 안 쓸지 미정
}

AgcmAdminDlgCharacterSkill::~AgcmAdminDlgCharacterSkill()
{
	ClearMasteryList();
}

BOOL AgcmAdminDlgCharacterSkill::SetCBSearchSkill(ADMIN_CB pfCallback)
{
	m_pfSearchSkill = pfCallback;
	return TRUE;
}

BOOL AgcmAdminDlgCharacterSkill::SetCBRollback(ADMIN_CB pfCallback)
{
	m_pfRollback = pfCallback;
	return TRUE;
}

BOOL AgcmAdminDlgCharacterSkill::SetCBSave(ADMIN_CB pfCallback)
{
	m_pfSave = pfCallback;
	return TRUE;
}

BOOL AgcmAdminDlgCharacterSkill::SetMasteryList(BOOL bResult, PVOID pList)
{
	AuList<stAgpdAdminSkillMastery*>* pSkillList = NULL;

	BOOL bSearchCharacter = FALSE;
	// 검색한 사람 것을 받았는 지 확인한다.
	if(bResult)
	{
		pSkillList = (AuList<stAgpdAdminSkillMastery*>*)pList;
		if(pSkillList)
		{
			if(pSkillList->GetHeadNode() && pSkillList->GetHeadNode()->GetData())
			{
				if(AgcmAdminDlgManager::Instance()->GetCharacterDlg()->IsSearchCharacter(pSkillList->GetHeadNode()->GetData()->m_szCharName))
				{
					bSearchCharacter = TRUE;
				}
			}
		}
	}

	if(!bSearchCharacter)
		return FALSE;
	
	// List 를 통째로 받는 것이므로, 이전에 있는 건 지운다.
	ClearMasteryList();
	ClearMasteryListView();

	if(!bResult)
		return FALSE;

	if(pSkillList == NULL)
		AuList<stAgpdAdminSkillMastery*>* pSkillList = (AuList<stAgpdAdminSkillMastery*>*)pList;

	if(!pSkillList)
		return FALSE;

	AuNode<stAgpdAdminSkillMastery*>* pcNode = pSkillList->GetHeadNode();
	while(pcNode)
	{
		stAgpdAdminSkillMastery* pMastery = new stAgpdAdminSkillMastery;
		memcpy(pMastery, pcNode->GetData(), sizeof(stAgpdAdminSkillMastery));
		m_listMastery.AddTail(pMastery);

		pcNode = pcNode->GetNextNode();
	}

	return ShowData();
}

BOOL AgcmAdminDlgCharacterSkill::ShowData(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hMastery = GetDlgItem(hDlg, IDC_LV_CHAR_SKILL_LIST);
	if(!hMastery)
		return FALSE;

	if(m_listMastery.GetCount() <= 0)
		return FALSE;

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	int iRows = 0;
	INT16 lSpecialization = 0;
	AuNode<stAgpdAdminSkillMastery*>* pcNode = m_listMastery.GetHeadNode();
	while(pcNode)
	{
		lvItem.iItem = iRows;

		// Rows
		lvItem.iSubItem = 0;
		wsprintf(szTmp, "%d", iRows+1);
		lvItem.pszText = szTmp;
		ListView_InsertItem(hMastery, &lvItem);

		// Mastery Name
		lvItem.iSubItem = 1;
		lvItem.pszText = pcNode->GetData()->m_szMasteryName;
		ListView_SetItem(hMastery, &lvItem);

		// UsedSP
		lvItem.iSubItem = 2;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lUsedSP);
		lvItem.pszText = szTmp;
		ListView_SetItem(hMastery, &lvItem);

		// AquSkill
		lvItem.iSubItem = 3;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lAquSkill);
		lvItem.pszText = szTmp;
		ListView_SetItem(hMastery, &lvItem);
		
		// Specialization
		strcpy(szTmp, "");
		lSpecialization = pcNode->GetData()->m_lSpecialization;
		if(lSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_DECREASE_COST)
			strcat(szTmp, "1");
		if(lSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_DMG)
			strcat(szTmp, " 2");
		if(lSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_DURATION)
			strcat(szTmp, " 3");
		if(lSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_TARGET_AREA)
			strcat(szTmp, " 4");
		if(lSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_TARGET_DISTANCE)
			strcat(szTmp, " 5");

		lvItem.iSubItem = 4;
		lvItem.pszText = szTmp;
		ListView_SetItem(hMastery, &lvItem);

		// LastUpdate - 사용하지 않는다.
		lvItem.iSubItem = 5;
		lvItem.pszText = "";
		ListView_SetItem(hMastery, &lvItem);
					
		iRows++;
		pcNode = pcNode->GetNextNode();
	}

	return TRUE;
}

BOOL AgcmAdminDlgCharacterSkill::ShowBasicData(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hBasicInfo = GetDlgItem(hDlg, IDC_LV_CHAR_SKILL_BASIC);
	if(!hBasicInfo)
		return FALSE;

	// 일단 비운다.
	ListView_DeleteAllItems(hBasicInfo);
	
	stAgpdAdminCharData* pstCharData = AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetCharData();
	if(strlen(pstCharData->m_stBasic.m_szCharName) <= 0)
		return FALSE;

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	// CharName
	lvItem.iItem = 0;
	lvItem.iSubItem = 0;
	lvItem.pszText = pstCharData->m_stBasic.m_szCharName;
	ListView_InsertItem(hBasicInfo, &lvItem);

	// Level
	lvItem.iSubItem = 1;
	wsprintf(szTmp, "%d", pstCharData->m_stStatus.m_lLevel);
	lvItem.pszText = szTmp;
	ListView_SetItem(hBasicInfo, &lvItem);

	// Class
	lvItem.iSubItem = 2;
	lvItem.pszText = pstCharData->m_stStatus.m_szClass;
	ListView_SetItem(hBasicInfo, &lvItem);

	// Race
	lvItem.iSubItem = 3;
	lvItem.pszText = pstCharData->m_stStatus.m_szRace;
	ListView_SetItem(hBasicInfo, &lvItem);

	// Exp
	lvItem.iSubItem = 4;
	if(pstCharData->m_stPoint.m_lMaxEXP != 0)
		sprintf(szTmp, "%.2f%%", (float)(pstCharData->m_stPoint.m_lEXP / (pstCharData->m_stPoint.m_lMaxEXP + 0.0f) * 100.0f));
	else
		sprintf(szTmp, "");
	lvItem.pszText = szTmp;
	ListView_SetItem(hBasicInfo, &lvItem);

	// TotalSP
	lvItem.iSubItem = 5;
	if(pstCharData->m_stPoint.m_lTotalSP != 0)
		sprintf(szTmp, "%d", pstCharData->m_stPoint.m_lTotalSP);
	else
		sprintf(szTmp, "");
	lvItem.pszText = szTmp;
	ListView_SetItem(hBasicInfo, &lvItem);
	
	// RemSP
	lvItem.iSubItem = 6;
	if(pstCharData->m_stPoint.m_lRemainSP != 0)
		sprintf(szTmp, "%d", pstCharData->m_stPoint.m_lRemainSP);
	else
		sprintf(szTmp, "");
	lvItem.pszText = szTmp;
	ListView_SetItem(hBasicInfo, &lvItem);

	// CreDate
	lvItem.iSubItem = 7;
	lvItem.pszText = "몰리";
	ListView_SetItem(hBasicInfo, &lvItem);

	// IP
	lvItem.iSubItem = 8;
	lvItem.pszText = pstCharData->m_stSub.m_szIP;
	ListView_SetItem(hBasicInfo, &lvItem);

	return TRUE;
}

BOOL AgcmAdminDlgCharacterSkill::ClearMasteryList()
{
	if(m_listMastery.IsEmpty() == FALSE)
	{
		AuNode<stAgpdAdminSkillMastery*>* pcNode = NULL;
		AuNode<stAgpdAdminSkillMastery*>* pcNode2 = NULL;

		pcNode = m_listMastery.GetHeadNode();
		while(pcNode)
		{
			if(pcNode->GetData())
			{
				delete pcNode->GetData();

				pcNode2 = pcNode->GetNextNode();
				m_listMastery.RemoveNode(pcNode);
				pcNode = pcNode2;
			}
			else
				break;
		}
	}

	return TRUE;
}

BOOL AgcmAdminDlgCharacterSkill::ClearMasteryListView(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hMastery = GetDlgItem(hDlg, IDC_LV_CHAR_SKILL_LIST);
	return ListView_DeleteAllItems(hMastery);
}

LRESULT CALLBACK AgcmAdminDlgCharacterSkill::CharacterSkillDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_NOTIFY:
		{
			switch(((LPNMHDR)lParam)->code)
			{
				// Property Sheet
				case PSN_SETACTIVE:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->OnInitDialog(hDlg, wParam, lParam);

				case PSN_APPLY:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->CloseDlg(GetParent(hDlg));
			}

			LPNMHDR hdr;
			LPNMLISTVIEW nlv;
			hdr = (LPNMHDR)lParam;
			nlv = (LPNMLISTVIEW)lParam;
			HWND hMastery = GetDlgItem(hDlg, IDC_LV_CHAR_SKILL_LIST);
			
			if(hdr != NULL && hdr->hwndFrom == hMastery)
			{
				switch(hdr->code)
				{
					case NM_DBLCLK:
						return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->OnMasteryLVDblClk(hMastery, hdr, nlv);
				}
			}
			
			return TRUE;
		}
			
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_CB_CHAR_SKILL_TYPE:
					if(HIWORD(wParam) == CBN_SELCHANGE)
						return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->OnMasteryChangeCB(hDlg);

					break;

				case IDC_BT_CHAR_SKILL_REFRESH:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->OnRefreshBtn(hDlg);

				case IDC_BT_CHAR_SKILL_ROLLBACK:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->OnRollbackBtn(hDlg);

				case IDC_BT_CHAR_SKILL_SAVE:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->OnSaveBtn(hDlg);
			}
			return FALSE;

		// Property Page 용 Dialog 에는 WM_CLOSE 가 없다.
	}

	return FALSE;
}

LRESULT AgcmAdminDlgCharacterSkill::OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if(GetInitialized())
	{
		// 먼저 화면을 비운다.
		ClearMasteryListView(hDlg);

		OnRefreshBtn(hDlg);

		ShowBasicData(hDlg);
		ShowData(hDlg);
		return TRUE;
	}

	// Property Sheet 에서 열어주므로, m_hDlg 는 NULL 이다.
	// 그래서 이렇게 초기화 할때 세팅해준다.
	m_hDlg = hDlg;

	InitBasicInfoLV(hDlg);
	InitSkillInfoLV(hDlg);
	InitMasteryCB(hDlg);

	SetInitialized(TRUE);
	
	OnRefreshBtn(hDlg);

	ShowBasicData(hDlg);
	ShowData(hDlg);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterSkill::InitBasicInfoLV(HWND hDlg)
{
	HWND hBasicInfo = GetDlgItem(hDlg, IDC_LV_CHAR_SKILL_BASIC);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hBasicInfo, 0, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Level";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hBasicInfo, 1, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "Class";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hBasicInfo, 2, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "Race";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hBasicInfo, 3, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "Exp";
	lvCol.iSubItem = 4;
	ListView_InsertColumn(hBasicInfo, 4, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "TotalSP";
	lvCol.iSubItem = 5;
	ListView_InsertColumn(hBasicInfo, 5, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "RemSP";
	lvCol.iSubItem = 6;
	ListView_InsertColumn(hBasicInfo, 6, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "CreDate";
	lvCol.iSubItem = 7;
	ListView_InsertColumn(hBasicInfo, 7, &lvCol);

	lvCol.cx = 120;
	lvCol.pszText = "IP";
	lvCol.iSubItem = 8;
	ListView_InsertColumn(hBasicInfo, 8, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hBasicInfo, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterSkill::InitSkillInfoLV(HWND hDlg)
{
	HWND hSkillInfo = GetDlgItem(hDlg, IDC_LV_CHAR_SKILL_LIST);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 45;
	lvCol.pszText = "";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hSkillInfo, 0, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "Mastery";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hSkillInfo, 1, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "UsedSP";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hSkillInfo, 2, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "AquSkill";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hSkillInfo, 3, &lvCol);

	lvCol.cx = 120;
	lvCol.pszText = "Specialization";
	lvCol.iSubItem = 4;
	ListView_InsertColumn(hSkillInfo, 4, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "Last Update";
	lvCol.iSubItem = 5;
	ListView_InsertColumn(hSkillInfo, 5, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "";
	lvCol.iSubItem = 6;
	ListView_InsertColumn(hSkillInfo, 6, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hSkillInfo, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
} 

LRESULT AgcmAdminDlgCharacterSkill::InitMasteryCB(HWND hDlg)
{
	HWND hMastery = GetDlgItem(hDlg, IDC_CB_CHAR_SKILL_TYPE);
	if(!hMastery)
		return FALSE;

	char szTmp[255];
	for(int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
	{
		wsprintf(szTmp, "%s%d", MASTERY_STRING, i+1);
		SendMessage(hMastery, CB_ADDSTRING, 0, (LPARAM)szTmp);
	}

	SendMessage(hMastery, CB_SETCURSEL, 0, 0);	// Default All 로 맞춘다.

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterSkill::OnMasteryChangeCB(HWND hDlg)
{
	if(!m_pcsCBClass || !m_pfSearchSkill)
		return FALSE;

	if(hDlg == NULL) hDlg = m_hDlg;
	
	HWND hMastery = GetDlgItem(hDlg, IDC_CB_CHAR_SKILL_TYPE);
	if(!hMastery)
		return FALSE;

	INT16 lMastery = (INT16)SendMessage(hMastery, CB_GETCURSEL, 0, 0);
	if(lMastery == CB_ERR)
		return FALSE;

	stAgpdAdminCharData* pstCharData = AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetCharData();
	stAgpdAdminCharSkill stSkill;

	stSkill.m_lCID = pstCharData->m_stBasic.m_lCID;
	strcpy(stSkill.m_szCharName, pstCharData->m_stBasic.m_szCharName);
	stSkill.m_lType = lMastery;
	
	m_pfSearchSkill(&stSkill, m_pcsCBClass, NULL);
	
	return TRUE;
}

LRESULT AgcmAdminDlgCharacterSkill::OnMasteryLVDblClk(HWND hMastery, LPNMHDR hdr, LPNMLISTVIEW nlv)
{
	if(nlv->iItem < 0)
		return FALSE;

	stAgpdAdminSkillMastery stMastery;
	memset(&stMastery, 0, sizeof(stMastery));

	stMastery.m_lMastery = nlv->iItem;
	stMastery.m_lOwnerCID = AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetCharData()->m_stBasic.m_lCID;
	strcpy(stMastery.m_szCharName, AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetCharData()->m_stBasic.m_szCharName);
	ListView_GetItemText(hMastery, nlv->iItem, 1, stMastery.m_szMasteryName, AGPMSKILL_MAX_MASTERY_NAME);
	
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->SetLastSearchMastery(&stMastery);

	// Skill Sub Page 를 Activate 시킨다.
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->ActivateSkillSubPage();
	
	return TRUE;
}

LRESULT AgcmAdminDlgCharacterSkill::OnRefreshBtn(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;
	
	return OnMasteryChangeCB(hDlg);
}

LRESULT AgcmAdminDlgCharacterSkill::OnRollbackBtn(HWND hDlg)
{
	return TRUE;
}

LRESULT AgcmAdminDlgCharacterSkill::OnSaveBtn(HWND hDlg)
{
	return TRUE;
}