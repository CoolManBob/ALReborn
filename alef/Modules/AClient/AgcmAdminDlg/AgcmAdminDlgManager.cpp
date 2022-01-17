// AgcmAdminDlgManager.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 15.

#include "AgcmAdminDlgInclude.h"
#include <time.h>

///////////////////////////////////////////////////////
// AgcmAdminDlgManager Class....
AgcmAdminDlgManager::AgcmAdminDlgManager()
{
	m_hInst = NULL;

	SetSelfAdminInfo(NULL);
}

AgcmAdminDlgManager::~AgcmAdminDlgManager()
{
}

AgcmAdminDlgManager* AgcmAdminDlgManager::_Instance = 0;
AgcmAdminDlgManager* AgcmAdminDlgManager::Instance()
{
	if(!_Instance)
		_Instance = new AgcmAdminDlgManager();

	return _Instance;
}

void AgcmAdminDlgManager::SetInst(HINSTANCE hInst)
{
	m_hInst = hInst;

	// Sub-Dialog 도 세팅해준다.
	m_csMainDlg.SetInst(hInst, NULL);
	m_csSearchDlg.SetInst(hInst, NULL);
	m_csCharacterDlg.SetInst(hInst, NULL);
	m_csHelpDlg.SetInst(hInst, NULL);
	m_csMoveDlg.SetInst(hInst, NULL);
}

BOOL AgcmAdminDlgManager::SetCBClass(PVOID pClass)
{
	m_pcsCBClass = pClass;

	// Sub-Dialog 도 세팅해준다.
	m_csMainDlg.SetCBClass(pClass);
	m_csSearchDlg.SetCBClass(pClass);
	m_csCharacterDlg.SetCBClass(pClass);
	m_csHelpDlg.SetCBClass(pClass);
	m_csMoveDlg.SetCBClass(pClass);

	return TRUE;
}

BOOL AgcmAdminDlgManager::SetSelfAdminInfo(stAgpdAdminInfo* pstAdminInfo)
{
	if(pstAdminInfo)
	{
		memcpy(&m_stSelfAdminInfo, pstAdminInfo, sizeof(m_stSelfAdminInfo));

		// Move Dialog 의 Slef 정보를 세팅한다.
		m_csMoveDlg.SetSelfName(pstAdminInfo->m_szAdminName);
	}
	else
		memset(&m_stSelfAdminInfo, 0, sizeof(m_stSelfAdminInfo));

	return TRUE;
}

PVOID AgcmAdminDlgManager::GetCBClass()
{
	return m_pcsCBClass;
}

AgcmAdminDlgMain* AgcmAdminDlgManager::GetMainDlg()
{
	return &m_csMainDlg;
}

AgcmAdminDlgSearch* AgcmAdminDlgManager::GetSearchDlg()
{
	return &m_csSearchDlg;
}

AgcmAdminDlgCharacter* AgcmAdminDlgManager::GetCharacterDlg()
{
	return &m_csCharacterDlg;
}

AgcmAdminDlgHelp* AgcmAdminDlgManager::GetHelpDlg()
{
	return &m_csHelpDlg;
}

AgcmAdminDlgCharacterMove* AgcmAdminDlgManager::GetMoveDlg()
{
	return &m_csMoveDlg;
}

stAgpdAdminInfo* AgcmAdminDlgManager::GetSelfAdminInfo()
{
	return &m_stSelfAdminInfo;
}

BOOL AgcmAdminDlgManager::IsDialogMessage(LPMSG lpMsg)
{
	if(::IsWindow(m_csMainDlg.GetHDlg()) && ::IsDialogMessage(m_csMainDlg.GetHDlg(), lpMsg))
		return TRUE;

	if(::IsWindow(m_csSearchDlg.GetHDlg()) && ::IsDialogMessage(m_csSearchDlg.GetHDlg(), lpMsg))
		return TRUE;

	if(::IsWindow(m_csCharacterDlg.GetHDlg()) && PropSheet_IsDialogMessage(m_csCharacterDlg.GetHDlg(), lpMsg))
		return TRUE;

	if(::IsWindow(m_csHelpDlg.GetHDlg()) && ::IsDialogMessage(m_csHelpDlg.GetHDlg(), lpMsg))
		return TRUE;

	if(::IsWindow(m_csMoveDlg.GetHDlg()) && ::IsDialogMessage(m_csMoveDlg.GetHDlg(), lpMsg))
		return TRUE;

	//if(PropSheet_IsDialogMessage(m_csCharacterDlg.GetDataPage()->GetHwnd(), lpMsg))
	//	return TRUE;

	//if(PropSheet_IsDialogMessage(m_csCharacterDlg.GetItemPage()->GetHwnd(), lpMsg))
	//	return TRUE;

	//if(::IsDialogMessage(m_csCharacterDlg.GetHwnd(), lpMsg))
	//	return TRUE;

	return FALSE;
}

// Main Dialog 를 연다.
BOOL AgcmAdminDlgManager::OpenMainDlg()
{
	BOOL bResult = m_csMainDlg.OpenDlg();

	if(bResult)
	{
		m_csSearchDlg.SetParentHwnd(m_csMainDlg.GetHDlg());
		m_csCharacterDlg.SetParentHwnd(m_csMainDlg.GetHDlg());
		m_csHelpDlg.SetParentHwnd(m_csMainDlg.GetHDlg());
	}

	return bResult;
}

// Search Dialog 를 연다.
BOOL AgcmAdminDlgManager::OpenSearchDlg()
{
	return m_csSearchDlg.OpenDlg();
}

// Character Dialog 를 연다.
BOOL AgcmAdminDlgManager::OpenCharacterDlg()
{
	return m_csCharacterDlg.OpenDlg();
}

// Help Dialog 를 연다.
BOOL AgcmAdminDlgManager::OpenHelpDlg()
{
	return m_csHelpDlg.OpenDlg();
}

// Move Dialog 를 연다.
BOOL AgcmAdminDlgManager::OpenMoveDlg()
{
	return m_csMoveDlg.OpenDlg();
}

// UNIX TimeStamp 로 부터 날짜를 화면에 뿌릴 용으로 얻어낸다.
void AgcmAdminDlgManager::GetDateTimeByTimeStamp(INT32 lTimeStamp, char* szDate, INT32 lSize)
{
	time_t timeval;
	struct tm* tm_ptr;

	timeval = (time_t)lTimeStamp;
	
	(void)time(NULL);
	tm_ptr = localtime(&timeval);

	strftime(szDate, lSize, "%y-%m-%d/%H:%M", tm_ptr);
}