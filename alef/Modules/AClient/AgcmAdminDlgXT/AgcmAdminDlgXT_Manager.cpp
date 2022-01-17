// AgcmAdminDlgXT_Manager.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 12. 11.

#include "StdAfx.h"

///////////////////////////////////////////////////////
// AgcmAdminDlgXT_Manager Class....

AgcmAdminDlgXT_Manager::AgcmAdminDlgXT_Manager()
{
	SetSelfAdminInfo(NULL);

	// Dialog 초기화
	m_pcsMainDlg = new AgcmAdminDlgXT_Main;
	m_pcsSearchDlg = new AgcmAdminDlgXT_Search;
	m_pcsCharacterDlg = new AgcmAdminDlgXT_Character;
	m_pcsCharItemDlg = new AgcmAdminDlgXT_CharItem;
	m_pcsHelpDlg = new AgcmAdminDlgXT_Help;
	m_pcsMoveDlg = new AgcmAdminDlgXT_Move;
	m_pcsBanDlg = new AgcmAdminDlgXT_Ban;
	m_pcsItemDlg = new AgcmAdminDlgXT_Item;
	m_pcsAdminListDlg = new AgcmAdminDlgXT_AdminList;
	m_pcsNoticeDlg = new AgcmAdminDlgXT_Notice;
	
	m_pcsGameDlg = new AgcmAdminDlgXT_Game;

	m_pcsMessageQueue = new AgcmAdminDlgXT_MessageQueue;
	
	m_pcsResourceLoader = new AgcmAdminDlgXT_ResourceLoader;

	m_pcsIniManager = new AgcmAdminDlgXT_IniManager;
}

AgcmAdminDlgXT_Manager::~AgcmAdminDlgXT_Manager()
{

	if(m_pcsNoticeDlg) m_pcsNoticeDlg->DestroyWindow();
	if(m_pcsAdminListDlg) m_pcsAdminListDlg->DestroyWindow();
	if(m_pcsItemDlg) m_pcsItemDlg->DestroyWindow();
	if(m_pcsBanDlg) m_pcsBanDlg->DestroyWindow();
	if(m_pcsCharItemDlg) m_pcsCharItemDlg->DestroyWindow();
	if(m_pcsCharacterDlg) m_pcsCharacterDlg->DestroyWindow();
	if(m_pcsHelpDlg) m_pcsHelpDlg->DestroyWindow();
	if(m_pcsMoveDlg) m_pcsMoveDlg->DestroyWindow();
	if(m_pcsSearchDlg) m_pcsSearchDlg->DestroyWindow();
	if(m_pcsMainDlg) m_pcsMainDlg->DestroyWindow();

	if(m_pcsGameDlg) m_pcsGameDlg->DestroyWindow();

	Sleep(10);

	if(m_pcsNoticeDlg) delete m_pcsNoticeDlg;
	if(m_pcsAdminListDlg) delete m_pcsAdminListDlg;
	if(m_pcsItemDlg) delete m_pcsItemDlg;
	if(m_pcsBanDlg) delete m_pcsBanDlg;
	if(m_pcsCharItemDlg) delete m_pcsCharItemDlg;
	if(m_pcsCharacterDlg) delete m_pcsCharacterDlg;
	if(m_pcsHelpDlg) delete m_pcsHelpDlg;
	if(m_pcsMoveDlg) delete m_pcsMoveDlg;
	if(m_pcsSearchDlg) delete m_pcsSearchDlg;
	if(m_pcsMainDlg) delete m_pcsMainDlg;

	if(m_pcsGameDlg) delete m_pcsGameDlg;

	if(m_pcsMessageQueue) delete m_pcsMessageQueue;

	if(m_pcsResourceLoader) delete m_pcsResourceLoader;

	if(m_pcsIniManager) delete m_pcsIniManager;
}

AgcmAdminDlgXT_Manager* AgcmAdminDlgXT_Manager::_Instance = 0;
AgcmAdminDlgXT_Manager* AgcmAdminDlgXT_Manager::Instance()
{
	if(!_Instance)
		_Instance = new AgcmAdminDlgXT_Manager();

	return _Instance;
}

BOOL AgcmAdminDlgXT_Manager::SetCBClass(PVOID pClass)
{
	m_pcsCBClass = pClass;

	return TRUE;
}

BOOL AgcmAdminDlgXT_Manager::SetSelfAdminInfo(stAgpdAdminInfo* pstAdminInfo)
{
	if(pstAdminInfo)
	{
		memcpy(&m_stSelfAdminInfo, pstAdminInfo, sizeof(m_stSelfAdminInfo));

		// Move Dialog 의 Slef 정보를 세팅한다.
		//m_csMoveDlg.SetSelfName(pstAdminInfo->m_szAdminName);
	}
	else
		memset(&m_stSelfAdminInfo, 0, sizeof(m_stSelfAdminInfo));

	return TRUE;
}

PVOID AgcmAdminDlgXT_Manager::GetCBClass()
{
	return m_pcsCBClass;
}

stAgpdAdminInfo* AgcmAdminDlgXT_Manager::GetSelfAdminInfo()
{
	return &m_stSelfAdminInfo;
}

AgcmAdminDlgXT_Main* AgcmAdminDlgXT_Manager::GetMainDlg()
{
	return m_pcsMainDlg;
}

AgcmAdminDlgXT_Search* AgcmAdminDlgXT_Manager::GetSearchDlg()
{
	return m_pcsSearchDlg;
}

AgcmAdminDlgXT_CharItem* AgcmAdminDlgXT_Manager::GetCharItemDlg()
{
	return m_pcsCharItemDlg;
}

AgcmAdminDlgXT_Character* AgcmAdminDlgXT_Manager::GetCharDlg()
{
	return m_pcsCharacterDlg;
}

AgcmAdminDlgXT_Help* AgcmAdminDlgXT_Manager::GetHelpDlg()
{
	return m_pcsHelpDlg;
}

AgcmAdminDlgXT_Move* AgcmAdminDlgXT_Manager::GetMoveDlg()
{
	return m_pcsMoveDlg;
}

AgcmAdminDlgXT_Ban* AgcmAdminDlgXT_Manager::GetBanDlg()
{
	return m_pcsBanDlg;
}

AgcmAdminDlgXT_Item* AgcmAdminDlgXT_Manager::GetItemDlg()
{
	return m_pcsItemDlg;
}

AgcmAdminDlgXT_AdminList* AgcmAdminDlgXT_Manager::GetAdminListDlg()
{
	return m_pcsAdminListDlg;
}

AgcmAdminDlgXT_Notice* AgcmAdminDlgXT_Manager::GetNoticeDlg()
{
	return m_pcsNoticeDlg;
}

AgcmAdminDlgXT_Game* AgcmAdminDlgXT_Manager::GetGameDlg()
{
	return m_pcsGameDlg;
}

AgcmAdminDlgXT_MessageQueue* AgcmAdminDlgXT_Manager::GetMessageQueue()
{
	return m_pcsMessageQueue;
}

AgcmAdminDlgXT_ResourceLoader* AgcmAdminDlgXT_Manager::GetResourceLoader()
{
	return m_pcsResourceLoader;
}

AgcmAdminDlgXT_IniManager* AgcmAdminDlgXT_Manager::GetIniManager()
{
	return m_pcsIniManager;
}

BOOL AgcmAdminDlgXT_Manager::IsDialogMessage(LPMSG lpMsg)
{
	if(m_pcsMainDlg && m_pcsMainDlg->IsInitialized() && m_pcsMainDlg->IsWindowVisible() && m_pcsMainDlg->IsDialogMessage(lpMsg))
		return TRUE;

	if(m_pcsSearchDlg && m_pcsSearchDlg->IsInitialized() && m_pcsSearchDlg->IsWindowVisible() && m_pcsSearchDlg->IsDialogMessage(lpMsg))
		return TRUE;

	if(m_pcsCharacterDlg && m_pcsCharacterDlg->IsInitialized() && m_pcsCharacterDlg->IsWindowVisible() && m_pcsCharacterDlg->IsDialogMessage(lpMsg))
		return TRUE;

	if(m_pcsCharItemDlg && m_pcsCharItemDlg->IsInitialized() && m_pcsCharItemDlg->IsWindowVisible() && m_pcsCharItemDlg->IsDialogMessage(lpMsg))
		return TRUE;

	if(m_pcsHelpDlg && m_pcsHelpDlg->IsInitialized() && m_pcsHelpDlg->IsWindowVisible() && m_pcsHelpDlg->IsDialogMessage(lpMsg))
		return TRUE;

	if(m_pcsMoveDlg && m_pcsMoveDlg->IsInitialized() && m_pcsMoveDlg->IsWindowVisible() && m_pcsMoveDlg->IsDialogMessage(lpMsg))
		return TRUE;

	if(m_pcsBanDlg && m_pcsBanDlg->IsInitialized() && m_pcsBanDlg->IsWindowVisible() && m_pcsBanDlg->IsDialogMessage(lpMsg))
		return TRUE;

	if(m_pcsItemDlg && m_pcsItemDlg->IsInitialized() && m_pcsItemDlg->IsWindowVisible() && m_pcsItemDlg->IsDialogMessage(lpMsg))
		return TRUE;

	if(m_pcsAdminListDlg && m_pcsAdminListDlg->IsInitialized() && m_pcsAdminListDlg->IsWindowVisible() && m_pcsAdminListDlg->IsDialogMessage(lpMsg))
		return TRUE;

	if(m_pcsNoticeDlg && m_pcsNoticeDlg->IsInitialized() && m_pcsNoticeDlg->IsWindowVisible() && m_pcsNoticeDlg->IsDialogMessage(lpMsg))
		return TRUE;

	return FALSE;
}

// Main Dialog 를 연다.
BOOL AgcmAdminDlgXT_Manager::OpenMainDlg()
{
	return m_pcsMainDlg->OpenDlg();
}

// Search Dialog 를 연다.
BOOL AgcmAdminDlgXT_Manager::OpenSearchDlg()
{
	return m_pcsSearchDlg->OpenDlg();
}

// Character Dialog 를 연다.
BOOL AgcmAdminDlgXT_Manager::OpenCharacterDlg()
{
	return m_pcsCharacterDlg->OpenDlg();
}

// Character Item Dialog 를 연다.
BOOL AgcmAdminDlgXT_Manager::OpenCharItemDlg()
{
	return m_pcsCharItemDlg->OpenDlg();
}

// Help Dialog 를 연다.
BOOL AgcmAdminDlgXT_Manager::OpenHelpDlg()
{
	return m_pcsHelpDlg->OpenDlg();
}

// Move Dialog 를 연다.
BOOL AgcmAdminDlgXT_Manager::OpenMoveDlg()
{
	return m_pcsMoveDlg->OpenDlg();
}

BOOL AgcmAdminDlgXT_Manager::OpenBanDlg()
{
	return m_pcsBanDlg->OpenDlg();
}

BOOL AgcmAdminDlgXT_Manager::OpenItemDlg()
{
	return m_pcsItemDlg->OpenDlg();
}

BOOL AgcmAdminDlgXT_Manager::OpenAdminListDlg()
{
	return m_pcsAdminListDlg->OpenDlg();
}

BOOL AgcmAdminDlgXT_Manager::OpenNoticeDlg()
{
	return m_pcsNoticeDlg->OpenDlg();
}

BOOL AgcmAdminDlgXT_Manager::IsDLGMSG(LPMSG lpMsg)
{
	if(m_pcsGameDlg && m_pcsGameDlg->IsInitialized() && m_pcsGameDlg->IsWindowVisible() && m_pcsGameDlg->IsDialogMessage(lpMsg))
		return TRUE;

	return FALSE;
}

BOOL AgcmAdminDlgXT_Manager::OpenGameDlg()
{
	return m_pcsGameDlg->OpenDlg();
}

BOOL AgcmAdminDlgXT_Manager::OpenAllWindows(INT nShowCmd)
{
	//m_pcsMainDlg->OpenDlg(nShowCmd); // Main Window 는 하지 않는다.
	m_pcsSearchDlg->OpenDlg(nShowCmd);
	m_pcsCharacterDlg->OpenDlg(nShowCmd);
	m_pcsCharItemDlg->OpenDlg(nShowCmd);
	m_pcsHelpDlg->OpenDlg(nShowCmd);
	m_pcsMoveDlg->OpenDlg(nShowCmd);
	m_pcsBanDlg->OpenDlg(nShowCmd);
	m_pcsItemDlg->OpenDlg(nShowCmd);
	m_pcsAdminListDlg->OpenDlg(nShowCmd);
	m_pcsNoticeDlg->OpenDlg(nShowCmd);
	
	return TRUE;
}

// UNIX TimeStamp 로 부터 날짜를 화면에 뿌릴 용으로 얻어낸다. - char*
void AgcmAdminDlgXT_Manager::GetDateTimeByTimeStamp(INT32 lTimeStamp, char* szDate, INT32 lSize)
{
	time_t timeval;
	struct tm* tm_ptr;

	timeval = (time_t)lTimeStamp;
	
	(void)time(NULL);
	tm_ptr = localtime(&timeval);

	strftime(szDate, lSize, "%y-%m-%d/%H:%M", tm_ptr);
}
