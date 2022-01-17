#include "StdAfx.h"
#include "../resource.h"
#include "AgcmSetFaceDlg.h"
#include "SetFace.h"

CSetFace *			g_pcsMainFaceDlgInstance = NULL;
AgcmSetFaceDlg *	g_pcsAgcmSetFaceDlgInstance = NULL;

AgcmSetFaceDlg::AgcmSetFaceDlg(void)
{
	SetModuleName("AgcmSetFaceDlg");
	g_pcsAgcmSetFaceDlgInstance	= this;
}

AgcmSetFaceDlg::~AgcmSetFaceDlg(void)
{
}

AgcmSetFaceDlg *	AgcmSetFaceDlg::GetInstance()
{
	return g_pcsAgcmSetFaceDlgInstance;
}

BOOL	AgcmSetFaceDlg::OpenMainFaceDlg()
{
	if(g_pcsMainFaceDlgInstance)
		return TRUE; // 이미 열려 있당...

	g_pcsMainFaceDlgInstance = new CSetFace();
	g_pcsMainFaceDlgInstance->Create(CSetFace::IDD);
	g_pcsMainFaceDlgInstance->ShowWindow(SW_SHOW);

	return TRUE;
}

VOID	AgcmSetFaceDlg::CloseMainFaceDlg()
{
	if(g_pcsMainFaceDlgInstance)
	{
		g_pcsMainFaceDlgInstance->ShowWindow(SW_HIDE);
		g_pcsMainFaceDlgInstance->DestroyWindow();

		delete g_pcsMainFaceDlgInstance;
		g_pcsMainFaceDlgInstance = NULL;
	}
}

BOOL	AgcmSetFaceDlg::IsOpenMainFaceDlg()
{
	return (g_pcsMainFaceDlgInstance) ? (TRUE) : (FALSE);
}

BOOL	AgcmSetFaceDlg::SetFaceAtomic(RpAtomic *pstAtomic)
{
	if (!g_pcsMainFaceDlgInstance)
		return FALSE;

	return g_pcsMainFaceDlgInstance->SetFaceAtomic(pstAtomic);
}
