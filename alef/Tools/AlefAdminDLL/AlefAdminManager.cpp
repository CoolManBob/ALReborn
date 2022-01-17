// AlefAdminManager.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 02. 14.

#include "StdAfx.h"
#include "AlefAdminManager.h"

///////////////////////////////////////////////////////
// AlefAdminManager Class....

AlefAdminManager::AlefAdminManager()
{
	memset(&m_stSelfAdminInfo, 0, sizeof(m_stSelfAdminInfo));
	m_pAdminModule = NULL;

	m_pMainDlg = new AlefAdminMain();
}

AlefAdminManager::~AlefAdminManager()
{
	ClearItemTemplateData();

	if(m_pMainDlg) m_pMainDlg->DestroyWindow();

	Sleep(0);

	if(m_pMainDlg) delete m_pMainDlg;
}

AlefAdminManager* AlefAdminManager::_Instance = 0;
//AlefAdminManager* AlefAdminManager::Instance()
//{
//	if(!_Instance)
//		_Instance = new AlefAdminManager();
//
//	return _Instance;
//}

void AlefAdminManager::DeleteInstance()
{
	if(_Instance)
	{
		delete _Instance;
		_Instance = NULL;
	}
}

BOOL AlefAdminManager::SetAdminModule(PVOID pModule)
{
	m_pAdminModule = pModule;
	m_DataManager.SetAdminModule( pModule );

	OnInit();
	return TRUE;
}

BOOL AlefAdminManager::SetAdminModule(AgcmAdmin* pcsAgcmAdmin)
{
	m_pAdminModule = (PVOID)pcsAgcmAdmin;
	m_DataManager.SetAdminModule( pcsAgcmAdmin );

	OnInit();
	return TRUE;
}

void AlefAdminManager::OnInit()
{
	m_csStringManager.Init();
}

//PVOID AlefAdminManager::GetAdminModule()
//{
//	return m_pAdminModule;
//}

AgcmAdmin* AlefAdminManager::GetAdminModule()
{
	return ( AgcmAdmin* )m_DataManager.GetAgcmAdmin();
}

CDataManager* AlefAdminManager::GetAdminData( void )
{
	return &m_DataManager;
}

BOOL AlefAdminManager::IsDialogMessage(LPMSG lpMsg)
{
	if(m_pMainDlg && ::IsWindow(m_pMainDlg->m_hWnd) && m_pMainDlg->IsWindowVisible() && m_pMainDlg->IsDialogMessage(lpMsg))
		return TRUE;

	return FALSE;
}

BOOL AlefAdminManager::SetSelfAdminInfo(stAgpdAdminInfo* pstInfo)
{
	if(!pstInfo)
		return FALSE;

	memcpy(&m_stSelfAdminInfo, pstInfo, sizeof(m_stSelfAdminInfo));
	return TRUE;
}

INT16 AlefAdminManager::GetAdminLevel()
{
	return m_stSelfAdminInfo.m_lAdminLevel;
}

BOOL AlefAdminManager::OpenNotPrivilegeDlg()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString szMsg = _T("");
	szMsg.LoadString(IDS_NOT_PRIVILEGE);

	::AfxMessageBox((LPCTSTR)szMsg);
	return TRUE;
}

BOOL AlefAdminManager::IsInitialized()
{
	if( GetAdminModule() && m_pMainDlg && ::IsWindow(m_pMainDlg->m_hWnd))
		return TRUE;

	return FALSE;
}

UINT32 AlefAdminManager::GetCurrentTimeStamp()
{
	time_t timeval;
	(void)time(&timeval);
	
	return (UINT32)timeval;
}

BOOL AlefAdminManager::ConvertTimeStampToString(UINT32 ulTimeStamp, CHAR* szTmp)
{
	if(!szTmp)
		return FALSE;

	if(ulTimeStamp == 0)
	{
		strcpy(szTmp, "");
		return TRUE;
	}

	time_t timeval;
	struct tm* tm_ptr;

	timeval = (time_t)ulTimeStamp;
	tm_ptr = localtime(&timeval);

	if(!tm_ptr)
		return FALSE;

	strftime(szTmp, 32, "%Y/%m/%d %H:%M:%S", tm_ptr);
	return strlen(szTmp);
}

BOOL AlefAdminManager::ConvertTimeStampToDateString(UINT32 ulTimeStamp, CHAR* szTmp)
{
	if(!szTmp)
		return FALSE;

	if(ulTimeStamp == 0)
	{
		strcpy(szTmp, "");
		return TRUE;
	}

	time_t timeval;
	struct tm* tm_ptr;

	timeval = (time_t)ulTimeStamp;
	tm_ptr = localtime(&timeval);

	if(!tm_ptr)
		return FALSE;

	strftime(szTmp, 32, "%Y%m%d", tm_ptr);
	return strlen(szTmp);
}

BOOL AlefAdminManager::OpenMainDlg()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// AgcmAdmin Module 이 세팅안되어 있으면 실패다.
	if( !GetAdminModule() )
		return FALSE;

	return m_pMainDlg->OpenDlg();
}

BOOL AlefAdminManager::AddItemTemplate(stAgpdAdminItemTemplate* pstItemTemplate)
{
	//if(!pstItemTemplate)
	//	return FALSE;

	//// 이미 있는지 확인해서 있으면 걍 나간다.
	//unordered_map<string, stAgpdAdminItemTemplate*>::const_iterator iterData = m_hashMapItemTemplate.find(pstItemTemplate->m_szItemName);
	//if(iterData != m_hashMapItemTemplate.end())
	//	return FALSE;

	//stAgpdAdminItemTemplate* pstNewItemTemplate = new stAgpdAdminItemTemplate;
	//memcpy(pstNewItemTemplate, pstItemTemplate, sizeof(stAgpdAdminItemTemplate));

	//m_hashMapItemTemplate.insert(
	//	unordered_map<string, stAgpdAdminItemTemplate*>::value_type(
	//	pstNewItemTemplate->m_szItemName, pstNewItemTemplate));
	return TRUE;
}

BOOL AlefAdminManager::ClearItemTemplateData()
{
	//if(m_hashMapItemTemplate.size() == 0)
	//	return FALSE;

	//stAgpdAdminItemTemplate* pstItemTemplate = NULL;
	//unordered_map<string, stAgpdAdminItemTemplate*>::const_iterator iterData = m_hashMapItemTemplate.begin();
	//while(iterData != m_hashMapItemTemplate.end())
	//{
	//	pstItemTemplate = (*iterData).second;
	//	if(pstItemTemplate)
	//		delete pstItemTemplate;

	//	iterData++;
	//}

	//m_hashMapItemTemplate.clear();

	return TRUE;
}

AlefAdminStringManager& AlefAdminManager::GetStringManager()
{
	return m_csStringManager;
}