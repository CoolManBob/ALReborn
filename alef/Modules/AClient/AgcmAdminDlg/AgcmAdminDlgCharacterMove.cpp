// AgcmAdminDlgCharacterMove.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 11. 25.

#include "AgcmAdminDlgInclude.h"
#include "resource.h"

AgcmAdminDlgCharacterMove::AgcmAdminDlgCharacterMove()
{
	m_iResourceID = IDD_CHAR_MOVE;
	m_pfDlgProc = MoveDlgProc;
	
	m_pfCBMove = NULL;

	memset(m_szSelfName, 0, AGPACHARACTER_MAX_ID_STRING+1);
	memset(m_szName, 0, AGPACHARACTER_MAX_ID_STRING+1);

	m_fX = m_fY = m_fZ = 0.0f;
	
	m_bSelfMove = FALSE;
}

AgcmAdminDlgCharacterMove::~AgcmAdminDlgCharacterMove()
{
}

BOOL AgcmAdminDlgCharacterMove::SetCBMove(ADMIN_CB pfCBMove)
{
	m_pfCBMove = pfCBMove;
	return TRUE;
}

BOOL AgcmAdminDlgCharacterMove::SetSelfName(CHAR* szSelfName)
{
	if(!szSelfName || strlen(szSelfName) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	strcpy(m_szSelfName, szSelfName);
	return TRUE;
}

BOOL AgcmAdminDlgCharacterMove::SetName(CHAR* szName)
{
	if(!szName || strlen(szName) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	strcpy(m_szName, szName);
	return TRUE;
}

BOOL AgcmAdminDlgCharacterMove::SetPosition(float fX, float fY, float fZ)
{
	m_fX = fX;
	m_fY = fY;
	m_fZ = fZ;
	
	return TRUE;
}

BOOL AgcmAdminDlgCharacterMove::SetSelfMove(BOOL bFlag)
{
	m_bSelfMove = bFlag;

	if(bFlag)
		CheckDlgButton(m_hDlg, IDC_C_CHAR_MOVE_SELF, BST_CHECKED);
	else
		CheckDlgButton(m_hDlg, IDC_C_CHAR_MOVE_SELF, BST_UNCHECKED);

	return TRUE;
}

BOOL AgcmAdminDlgCharacterMove::ProcessSelfMove(float fX, float fY, float fZ)
{
	if(!m_pfCBMove || !m_pcsCBClass)
		return FALSE;

	stAgpdAdminCharDataPosition stPosition;
	memset(&stPosition, 0, sizeof(stPosition));
	
	strcpy(stPosition.m_szName, m_szSelfName);
	stPosition.m_fX = fX;
	stPosition.m_fY = fY;
	stPosition.m_fZ = fZ;

	m_pfCBMove(&stPosition, m_pcsCBClass, NULL);

	return TRUE;
}

BOOL AgcmAdminDlgCharacterMove::ProcessSelfMove(CHAR* szTargetName)
{
	if(!m_pfCBMove || !m_pcsCBClass)
		return FALSE;

	if(!szTargetName)
		return FALSE;

	stAgpdAdminCharDataPosition stPosition;
	memset(&stPosition, 0, sizeof(stPosition));

	strcpy(stPosition.m_szName, m_szSelfName);
	strcpy(stPosition.m_szTargetName, szTargetName);

	m_pfCBMove(&stPosition, m_pcsCBClass, NULL);

	return TRUE;
}

BOOL AgcmAdminDlgCharacterMove::ShowData(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	// Check Box 에 따라서 Name 세팅 해준다.
	OnSelfCheckBox(hDlg);

	char szTmp[64];
	
	sprintf(szTmp, "%.2f", m_fX);
	SetDlgItemText(hDlg, IDC_E_CHAR_MOVE_X, szTmp);

	sprintf(szTmp, "%.2f", m_fY);
	SetDlgItemText(hDlg, IDC_E_CHAR_MOVE_Y, szTmp);

	sprintf(szTmp, "%.2f", m_fZ);
	SetDlgItemText(hDlg, IDC_E_CHAR_MOVE_Z, szTmp);

	return TRUE;
}

LRESULT CALLBACK AgcmAdminDlgCharacterMove::MoveDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
			return AgcmAdminDlgManager::Instance()->GetMoveDlg()->OnInitDialog(hDlg, wParam, lParam);

		case WM_ACTIVATE:
			return AgcmAdminDlgManager::Instance()->GetMoveDlg()->OnActivate(hDlg, wParam, lParam);

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case 2:	// ESC
					return AgcmAdminDlgManager::Instance()->GetMoveDlg()->CloseDlg(hDlg);
					
				case IDC_C_CHAR_MOVE_SELF:
					return AgcmAdminDlgManager::Instance()->GetMoveDlg()->OnSelfCheckBox(hDlg);

				case IDC_B_CHAR_MOVE_GO:
					return AgcmAdminDlgManager::Instance()->GetMoveDlg()->OnGoBtn(hDlg);
			}
			return FALSE;

		case WM_CLOSE:
			return AgcmAdminDlgManager::Instance()->GetMoveDlg()->CloseDlg(hDlg);
	}
	return FALSE;
}

LRESULT AgcmAdminDlgCharacterMove::OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if(GetInitialized())
	{
		ShowData(hDlg);
		return TRUE;
	}

	SetDlgItemText(hDlg, IDC_E_CHAR_MOVE_NAME, "");
	CheckDlgButton(hDlg, IDC_C_CHAR_MOVE_SELF, BST_UNCHECKED);
	SetDlgItemText(hDlg, IDC_E_CHAR_MOVE_X, "");
	SetDlgItemText(hDlg, IDC_E_CHAR_MOVE_Y, "");
	SetDlgItemText(hDlg, IDC_E_CHAR_MOVE_Z, "");
	
	return TRUE;
}

LRESULT AgcmAdminDlgCharacterMove::OnActivate(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	// 포커스를 받으면...
	if(LOWORD(wParam) != WA_INACTIVE)
	{
		SetWindowPosByParentWnd(hDlg);

		ShowData(hDlg);
	}
	
	return TRUE;
}

LRESULT AgcmAdminDlgCharacterMove::OnSelfCheckBox(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hCheckBox = GetDlgItem(hDlg, IDC_C_CHAR_MOVE_SELF);
	if(!hCheckBox) return FALSE;

	INT16 nCheck = SendMessage(hCheckBox, BM_GETCHECK, 0, 0);

	if(nCheck == BST_CHECKED)
		SetDlgItemText(hDlg, IDC_E_CHAR_MOVE_NAME, m_szSelfName);
	else if(nCheck == BST_UNCHECKED)
		SetDlgItemText(hDlg, IDC_E_CHAR_MOVE_NAME, m_szName);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterMove::OnGoBtn(HWND hDlg)
{
	if(!m_pfCBMove || !m_pcsCBClass)
		return FALSE;

	if(hDlg == NULL) hDlg = m_hDlg;

	char szTmp[64];
	stAgpdAdminCharDataPosition stPosition;
	memset(&stPosition, 0, sizeof(stPosition));

	GetDlgItemText(hDlg, IDC_E_CHAR_MOVE_NAME, szTmp, 63);
	strcpy(stPosition.m_szName, szTmp);

	GetDlgItemText(hDlg, IDC_E_CHAR_MOVE_X, szTmp, 63);
	if(strlen(szTmp) > 0) stPosition.m_fX = (FLOAT)atof(szTmp);

	GetDlgItemText(hDlg, IDC_E_CHAR_MOVE_Y, szTmp, 63);
	if(strlen(szTmp) > 0) stPosition.m_fY = (FLOAT)atof(szTmp);

	GetDlgItemText(hDlg, IDC_E_CHAR_MOVE_Z, szTmp, 63);
	if(strlen(szTmp) > 0) stPosition.m_fZ = (FLOAT)atof(szTmp);

	m_pfCBMove(&stPosition, m_pcsCBClass, NULL);

	return TRUE;
}