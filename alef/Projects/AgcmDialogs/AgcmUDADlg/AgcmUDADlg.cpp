// AgcmUDADlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "../resource.h"
//#include <afxdllx.h>
#include "AgcmUDADlg.h"
#include "MainUDADlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AgcmUDADlg	*g_pcsAgcmUDADlgInstance = NULL;
static CMainUDADlg	*g_pcsMainUDADlgInstance = NULL;

AgcmUDADlg::AgcmUDADlg()
{
	SetModuleName("AgcmUDADlg");

	g_pcsAgcmUDADlgInstance = this;

	m_pcsAgcmRender			= NULL;
	m_fnCBChangeAtomic		= NULL;
}

AgcmUDADlg::~AgcmUDADlg()
{
	CloseMainUDADlg();
}

AgcmUDADlg *AgcmUDADlg::GetInstance()
{
	return g_pcsAgcmUDADlgInstance;
}

BOOL AgcmUDADlg::OnAddModule()
{
	m_pcsAgcmRender = (AgcmRender *)(GetModule("AgcmRender"));
	if(!m_pcsAgcmRender)
		return FALSE;

	return TRUE;
}

BOOL AgcmUDADlg::OpenMainUDADlg(BOOL bEnable)
{
	if(g_pcsMainUDADlgInstance)
		return TRUE; // 이미 열려 있당...

	g_pcsMainUDADlgInstance = new CMainUDADlg();
	g_pcsMainUDADlgInstance->Create();
	g_pcsMainUDADlgInstance->ShowWindow(SW_SHOW);
	g_pcsMainUDADlgInstance->EnableControl(bEnable);

	return TRUE;
}

VOID AgcmUDADlg::CloseMainUDADlg()
{
	if(g_pcsMainUDADlgInstance)
	{
		g_pcsMainUDADlgInstance->ShowWindow(SW_HIDE);
		g_pcsMainUDADlgInstance->DestroyWindow();

		delete g_pcsMainUDADlgInstance;
		g_pcsMainUDADlgInstance = NULL;
	}
}

VOID AgcmUDADlg::SetRenderType(INT32 *plRenderType, RpAtomic *pstAtomic)
{
	if(g_pcsMainUDADlgInstance)
	{
		g_pcsMainUDADlgInstance->SetRenderTypePointer(plRenderType, pstAtomic);
	}
}

VOID AgcmUDADlg::EnableMainUDADlgControl(BOOL bEnable)
{
	if(g_pcsMainUDADlgInstance)
	{
		g_pcsMainUDADlgInstance->EnableControl(bEnable);
	}
}

BOOL AgcmUDADlg::IsOpenMainUDADlg()
{
	return (g_pcsMainUDADlgInstance) ? (TRUE) : (FALSE);
}
