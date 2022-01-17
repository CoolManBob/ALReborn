// AgcmAdminDlgCharacterData.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 15.

#include "AgcmAdminDlgInclude.h"
#include "resource.h"

AgcmAdminDlgCharacterData::AgcmAdminDlgCharacterData()
{
	m_iResourceID = IDD_CHAR_DATA;
	m_pfDlgProc = CharacterDataDlgProc;

	m_pfEditCharacter = NULL;

	memset(&m_stCharData, 0, sizeof(m_stCharData));
}

AgcmAdminDlgCharacterData::~AgcmAdminDlgCharacterData()
{
}

BOOL AgcmAdminDlgCharacterData::SetCBEditCharacter(ADMIN_CB pfCallback)
{
	m_pfEditCharacter = pfCallback;
	return TRUE;
}

BOOL AgcmAdminDlgCharacterData::SetCharData(BOOL bResult, stAgpdAdminCharData* pstCharData)
{
	// 검색한 것을 받았는지 검사한다.
	if(AgcmAdminDlgManager::Instance()->GetCharacterDlg()->IsSearchCharacter(pstCharData) == FALSE)
		return FALSE;

	if(pstCharData)
		memcpy(&m_stCharData, pstCharData, sizeof(m_stCharData));
	else
		memset(&m_stCharData, 0, sizeof(m_stCharData));

	ShowData();

	return TRUE;
}

BOOL AgcmAdminDlgCharacterData::SetCharDataSub(BOOL bResult, stAgpdAdminCharDataSub* pstCharDataSub)
{
	if(pstCharDataSub)
	{
		// 검색한 것을 받았는 지 확인한다.
		if(AgcmAdminDlgManager::Instance()->Instance()->GetCharacterDlg()->GetCharData()->m_stBasic.m_lCID == pstCharDataSub->m_lCID)
			memcpy(&m_stCharData.m_stSub, pstCharDataSub, sizeof(m_stCharData.m_stSub));
	}
	else
		memset(&m_stCharData.m_stSub, 0, sizeof(m_stCharData.m_stSub));

	ShowData();

	return TRUE;
}

BOOL AgcmAdminDlgCharacterData::ShowData(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;
	
	if(m_stCharData.m_stBasic.m_lCID == 0 ||
		strlen(m_stCharData.m_stBasic.m_szCharName) == 0)
	{
		ClearAllInfoField();
		return TRUE;
	}

	// 각항목에 데이터를 세팅해준다.
	CHAR szTmp[255];

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_CHARNAME, m_stCharData.m_stBasic.m_szCharName);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_CHARID, m_stCharData.m_stBasic.m_lCID, FALSE);
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_ACCNAME, m_stCharData.m_stSub.m_szAccName);
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_NAME, m_stCharData.m_stSub.m_szName);

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_RACE, m_stCharData.m_stStatus.m_szRace);
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_CLASS, m_stCharData.m_stStatus.m_szClass);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_LEV, m_stCharData.m_stStatus.m_lLevel, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_EXP, m_stCharData.m_stPoint.m_lEXP, FALSE);

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_UT, "");
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_CREDATE, 0, FALSE);

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LOGIN, m_stCharData.m_stSub.m_szLogin);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_PLAYTIME, m_stCharData.m_stSub.m_lPlayTime, FALSE);

	sprintf(szTmp, "%.0f, %.0f, %.0f", m_stCharData.m_stBasic.m_stPos.x,
															m_stCharData.m_stBasic.m_stPos.y,
															m_stCharData.m_stBasic.m_stPos.z);
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_XYZ, szTmp);
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_IP, m_stCharData.m_stSub.m_szIP);

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LOGOUT, m_stCharData.m_stSub.m_szLastLogOut);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_LAST_PLAYTIME, m_stCharData.m_stSub.m_lTotalPlayTime, FALSE);
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LAST_XYZ, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LAST_IP, m_stCharData.m_stSub.m_szLastIP);

	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_STR, m_stCharData.m_stStatus.m_lSTR, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_DEX, m_stCharData.m_stStatus.m_lDEX, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_CON, m_stCharData.m_stStatus.m_lCON, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_WIS, m_stCharData.m_stStatus.m_lWIS, FALSE);

	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_INT, m_stCharData.m_stStatus.m_lINT, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_CHA, m_stCharData.m_stStatus.m_lCHA, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_HP, m_stCharData.m_stPoint.m_lHP, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_MAXHP, m_stCharData.m_stPoint.m_lMaxHP, FALSE);

	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_MP, m_stCharData.m_stPoint.m_lMP, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_MAXMP, m_stCharData.m_stPoint.m_lMaxMP, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_SP, m_stCharData.m_stPoint.m_lSP, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_MAXSP, m_stCharData.m_stPoint.m_lMaxSP, FALSE);

	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_AC, 0, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_ATK, 0, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_MAGICATK, 0, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_MINDMG, 0, FALSE);

	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_MAXDMG, 0, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_MURDER, m_stCharData.m_stStatus.m_lMurderer, FALSE);
	
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_FIRE, m_stCharData.m_stAttribute.m_lFire, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_WATER, m_stCharData.m_stAttribute.m_lWater, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_EARTH, m_stCharData.m_stAttribute.m_lEarth, FALSE);
	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_WIND, m_stCharData.m_stAttribute.m_lAir, FALSE);

	SetDlgItemInt(m_hDlg, IDC_E_CHARINFO_MAGIC, m_stCharData.m_stAttribute.m_lMagic, FALSE);

	// Money
	sprintf(szTmp, "%I64d", m_stCharData.m_stMoney.m_lInventoryMoney);
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_INVMONEY, szTmp);

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_STOMONEY, "");

	return TRUE;
}

void AgcmAdminDlgCharacterData::ClearAllInfoField()
{
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_CHARNAME, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_CHARID, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_ACCNAME, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_NAME, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_RACE, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_CLASS, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LEV, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_EXP, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_UT, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_CREDATE, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LOGIN, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_PLAYTIME, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_XYZ, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_IP, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LOGOUT, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LAST_PLAYTIME, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LAST_XYZ, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_LAST_IP, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_STR, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_DEX, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_CON, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_WIS, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_INT, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_CHA, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_HP, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_MAXHP, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_MP, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_MAXMP, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_SP, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_MAXSP, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_AC, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_ATK, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_MAGICATK, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_MINDMG, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_MAXDMG, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_MURDER, "");
	
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_FIRE, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_WATER, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_EARTH, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_WIND, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_MAGIC, "");

	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_INVMONEY, "");
	SetDlgItemText(m_hDlg, IDC_E_CHARINFO_STOMONEY, "");
}

// 각 Field 에 의해서 Edit 항목을 Show/Hide 해준다.
void AgcmAdminDlgCharacterData::ShowEditField(UINT iMode, UINT iFieldName, UINT iFieldData)
{
	HWND hEditGroup = GetDlgItem(m_hDlg, IDC_S_EDIT_GROUP);
	HWND hEditField = GetDlgItem(m_hDlg, IDC_BT_CHARINFO_EDIT_FIELD);
	HWND hEditData = GetDlgItem(m_hDlg, IDC_E_CHARINFO_EDIT_DATA);
	HWND hEditEnter = GetDlgItem(m_hDlg, IDC_BT_CHARINFO_EDIT_ENTER);
	HWND hEditCancel = GetDlgItem(m_hDlg, IDC_BT_CHARINFO_EDIT_CANCEL);

	int nCmdShow = SW_SHOW;

	if(iMode == 0)	// Hide
	{
		SetDlgItemText(m_hDlg, IDC_BT_CHARINFO_EDIT_FIELD, "");
		SetDlgItemText(m_hDlg, IDC_E_CHARINFO_EDIT_DATA, "");

		nCmdShow = SW_HIDE;
	}
	else	// Show & Set Data
	{
		char szTmp[255];
		GetDlgItemText(m_hDlg, iFieldName, szTmp, 254);
		SetWindowText(hEditField, szTmp);

		GetDlgItemText(m_hDlg, iFieldData, szTmp, 254);
		SetWindowText(hEditData, szTmp);
	}

	ShowWindow(hEditGroup, nCmdShow);
	ShowWindow(hEditField, nCmdShow);
	ShowWindow(hEditData, nCmdShow);
	ShowWindow(hEditEnter, nCmdShow);
	ShowWindow(hEditCancel, nCmdShow);
}

LRESULT CALLBACK AgcmAdminDlgCharacterData::CharacterDataDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
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
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->OnInitDialog(hDlg, wParam, lParam);

				case PSN_APPLY:
				case PSN_RESET:
					//SetWindowLong(GetParent(hDlg), DWL_MSGRESULT, FALSE);
					//SendMessage(GetParent(hDlg), WM_CLOSE, 0, 0);
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->CloseDlg(GetParent(hDlg));
					//return FALSE;
			}
			return TRUE;
			
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_BT_CHARINFO_EDIT_ENTER:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->OnEditEnterBtn();

				case IDC_BT_CHARINFO_EDIT_CANCEL:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->OnEditCancelBtn();
			}
			return FALSE;

		// Property Page 용 Dialog 에는 WM_CLOSE 가 없다.
	}

	return FALSE;
}

LRESULT AgcmAdminDlgCharacterData::OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	//::SetForegroundWindow(hDlg);

	// Property Sheet 에서 열어주므로, m_hDlg 는 NULL 이다.
	// 그래서 이렇게 초기화 할때 세팅해준다.
	m_hDlg = hDlg;

	ShowData(hDlg);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterData::OnEditEnterBtn()
{
	return TRUE;
}

LRESULT AgcmAdminDlgCharacterData::OnEditCancelBtn()
{
	ShowEditField(0, 0, 0);	// Hide Edit Group
	return TRUE;
}