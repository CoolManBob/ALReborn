// AgcmItemDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
//#include <afxdllx.h>
#include "../resource.h"

#include "AgcmItemDlg.h"
#include "ItemDlg.h"
#include "ItemListDlg.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE AgcmItemDlgDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("AGCMITEMDLG.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(AgcmItemDlgDLL, hInstance))
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

		new CDynLinkLibrary(AgcmItemDlgDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("AGCMITEMDLG.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(AgcmItemDlgDLL);
	}
	return 1;   // ok
}
*/
static AgcmItemDlg *g_pcsInstAgcmItemDlg = NULL;

AgcmItemDlg *AgcmItemDlg::GetInstance()
{
	return g_pcsInstAgcmItemDlg;
}

AgcmItemDlg::AgcmItemDlg()
{
	SetModuleName("AgcmItemDlg");

	g_pcsInstAgcmItemDlg	= this;
}

AgcmItemDlg::~AgcmItemDlg()
{
}

BOOL AgcmItemDlg::OnAddModule()
{
	m_pcsAgpmItem		= (AgpmItem *)(GetModule("AgpmItem"));
	if(!m_pcsAgpmItem)
		return FALSE;

	return TRUE;
}

VOID AgcmItemDlg::SetClumpPathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2)
{
	sprintf(m_szFindClumpPathName1, "%s*.%s", szPath, szExt1);
	sprintf(m_szFindClumpPathName2, "%s*.%s", szPath, szExt2);
}

VOID AgcmItemDlg::SetClumpFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2)
{
	strcpy(m_szFindClumpPathName1, szFindPathName1);
	strcpy(m_szFindClumpPathName2, szFindPathName2);
}

VOID AgcmItemDlg::SetTexturePathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2, CHAR *szExt3)
{
	sprintf(m_szFindTexturePathName1, "%s*.%s", szPath, szExt1);
	sprintf(m_szFindTexturePathName2, "%s*.%s", szPath, szExt2);
	sprintf(m_szFindTexturePathName3, "%s*.%s", szPath, szExt3);
}

VOID AgcmItemDlg::SetTextureFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2, CHAR *szFindPathName3)
{
	strcpy(m_szFindTexturePathName1, szFindPathName1);
	strcpy(m_szFindTexturePathName2, szFindPathName2);
	strcpy(m_szFindTexturePathName3, szFindPathName3);
}

CHAR *AgcmItemDlg::GetClumpPathName1()
{
	return m_szFindClumpPathName1;
}

CHAR *AgcmItemDlg::GetClumpPathName2()
{
	return m_szFindClumpPathName2;
}

CHAR *AgcmItemDlg::GetTexturePathName1()
{
	return m_szFindTexturePathName1;
}

CHAR *AgcmItemDlg::GetTexturePathName2()
{
	return m_szFindTexturePathName2;
}

CHAR *AgcmItemDlg::GetTexturePathName3()
{
	return m_szFindTexturePathName3;
}

BOOL AgcmItemDlg::OpenItemDlg(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate, bool bWeaponTypeSelectable /*= true*/)
{
	if((!pcsAgpdItemTemplate) || (!pcsAgcdItemTemplate))
		return FALSE;

	CItemDlg dlg(pcsAgpdItemTemplate, pcsAgcdItemTemplate);
	dlg.SetWeaponTypeSelectable( bWeaponTypeSelectable );
	return dlg.DoModal() == IDOK ? TRUE : FALSE;
}

AgpdItemTemplate *AgcmItemDlg::OpenItemList()
{
	INT32 lTID;

	CItemListDlg dlg(&lTID);
	if(dlg.DoModal() == IDOK)
	{
		return m_pcsAgpmItem->GetItemTemplate(lTID);
	}

	return NULL;
}

BOOL AgcmItemDlg::SetCallbackGenerateBSphere(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEMDLG_CB_ID_SET_BSPHERE, pfCallback, pClass);
}

BOOL AgcmItemDlg::EnumCallbackSetItemData(INT32 lCallbackPoint, PVOID pvData, PVOID pvCustData)
{
	return EnumCallback(lCallbackPoint, pvData, pvCustData);
}