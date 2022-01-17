// AgcmCharacterDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
//#include <afxdllx.h>
#include "../resource.h"

#include "AgcmCharacterDlg.h"
#include "CharacterTemplateListDlg.h"
#include "CharacterDlg.h"

/*#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE AgcmCharacterDlgDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("AGCMCHARACTERDLG.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(AgcmCharacterDlgDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(AgcmCharacterDlgDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("AGCMCHARACTERDLG.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(AgcmCharacterDlgDLL);
	}
	return 1;   // ok
}
*/
static AgcmCharacterDlg *g_pcsInstAgcmCharacterDlg = NULL;

AgcmCharacterDlg *AgcmCharacterDlg::GetInstance()
{
	return g_pcsInstAgcmCharacterDlg;
}

AgcmCharacterDlg::AgcmCharacterDlg()
{
	SetModuleName("AgcmCharacterDlg");

	g_pcsInstAgcmCharacterDlg	= this;
	m_pcsAgpmFactors			= NULL;
}

AgcmCharacterDlg::~AgcmCharacterDlg()
{
}

BOOL AgcmCharacterDlg::OnAddModule()
{
	m_pcsAgpmFactors = (AgpmFactors *)(GetModule("AgpmFactors"));
	if (!m_pcsAgpmFactors)
		return FALSE;

	m_pcsAgpmCharacter = (AgpmCharacter *)(GetModule("AgpmCharacter"));
	if (!m_pcsAgpmCharacter)
		return FALSE;

	m_pcsAgcmCharacter = (AgcmCharacter *)(GetModule("AgcmCharacter"));
	if (!m_pcsAgcmCharacter)
		return FALSE;

	m_pcsAgcmEventEffect = (AgcmEventEffect *)(GetModule("AgcmEventEffect"));
	if (!m_pcsAgcmEventEffect)
		return FALSE;

	return TRUE;
}

VOID AgcmCharacterDlg::SetClumpPathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2)
{
	sprintf(m_szFindPathName1, "%s*.%s", szPath, szExt1);
	sprintf(m_szFindPathName2, "%s*.%s", szPath, szExt2);
}

VOID AgcmCharacterDlg::SetClumpFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2)
{
	strcpy(m_szFindPathName1, szFindPathName1);
	strcpy(m_szFindPathName2, szFindPathName2);
}

CHAR *AgcmCharacterDlg::GetFindPathName1()
{
	return m_szFindPathName1;
}

CHAR *AgcmCharacterDlg::GetFindPathName2()
{
	return m_szFindPathName2;
}

BOOL AgcmCharacterDlg::SetCallbackSetCharacterHeight(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHARACTERDLG_CB_ID_SET_HEIGHT, pfCallback, pClass);
}

//. 2005. 10. 07. Nonstopdj
//. Depth값 반환하는 callback 등록하기
BOOL AgcmCharacterDlg::SetCallbackSetCharacterDepth(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHARACTERDLG_CB_ID_SET_DEPTH, pfCallback, pClass);
}

BOOL AgcmCharacterDlg::SetCallbackSetCharacterHitRange(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHARACTERDLG_CB_ID_SET_HIT_RANGE, pfCallback, pClass);
}

BOOL AgcmCharacterDlg::SetCallbackSetCharacterBSphere(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHARACTERDLG_CB_ID_SET_BSPHERE, pfCallback, pClass);
}

BOOL AgcmCharacterDlg::EnumCallbackSetCharacterData(INT32 lCallbackPoint, PVOID pvData, PVOID pvCustData)
{
	return EnumCallback(lCallbackPoint, pvData, pvCustData);
}
/*
BOOL AgcmCharacterDlg::EnumCallbackSetCharacterHeight(INT32 *plHeight)
{
	return EnumCallback(AGCMCHARACTERDLG_CB_ID_SET_HEIGHT, (PVOID)(plHeight), NULL);
}

BOOL AgcmCharacterDlg::EnumCallbackSetCharacterHitRange(INT32 *plHitRange)
{
	return EnumCallback(AGCMCHARACTERDLG_CB_ID_SET_HIT_RANGE, (PVOID)(plHitRange), NULL);
}*/

BOOL AgcmCharacterDlg::OpenCharacterDlg(AgpdCharacterTemplate *pcsAgpdCharacterTemplate, AgcdCharacterTemplate *pcsAgcdCharacterTemplate)
{
	if((!pcsAgpdCharacterTemplate) || (!pcsAgcdCharacterTemplate))
		return FALSE;
	
	CCharacterDlg dlg(pcsAgpdCharacterTemplate, pcsAgcdCharacterTemplate);
	if(dlg.DoModal() == IDOK)
		return TRUE;

	return FALSE;
}

AgpmFactors *AgcmCharacterDlg::GetAgpmFactors()
{
	return m_pcsAgpmFactors;
}

AgpmCharacter *AgcmCharacterDlg::GetAgpmCharacter()
{
	return m_pcsAgpmCharacter;
}

AgcmCharacter *AgcmCharacterDlg::GetAgcmCharacter()
{
	return m_pcsAgcmCharacter;
}

AgcmEventEffect *AgcmCharacterDlg::GetAgcmEventEffect()
{
	return m_pcsAgcmEventEffect;
}

INT32 AgcmCharacterDlg::OpenCharacterTemplateList()
{
	INT32 lRt;

	CCharacterTemplateListDlg dlg(&lRt);
	if(dlg.DoModal() == IDOK)
	{
		return lRt;
	}

	return -1;
}