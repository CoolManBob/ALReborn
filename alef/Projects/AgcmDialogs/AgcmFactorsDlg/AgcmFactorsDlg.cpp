// AgcmFactorsDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "../resource.h"

#include "AgcmFactorsDlg.h"
#include "FactorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AgcmFactorsDlg			*g_pcsAgcmFactorsDlgInst	= NULL;

/*****************************************************************************
* Purpose :
*
* 060903 Bob Jung.
*****************************************************************************/
AgcmFactorsDlg *AgcmFactorsDlg::GetInstance()
{
	return g_pcsAgcmFactorsDlgInst;
}

/*****************************************************************************
* Purpose :
*
* 060403 Bob Jung.
*****************************************************************************/
AgcmFactorsDlg::AgcmFactorsDlg()
{
	SetModuleName("AgcmFactorsDlg");

	g_pcsAgcmFactorsDlgInst	= this;
	m_pcsAgpmFactors		= NULL;
}

/*****************************************************************************
* Purpose :
*
* 060403 Bob Jung.
*****************************************************************************/
AgcmFactorsDlg::~AgcmFactorsDlg()
{
}

/*****************************************************************************
* Purpose :
*
* 060403 Bob Jung.
*****************************************************************************/
BOOL AgcmFactorsDlg::OnInit()
{
	return TRUE;
}

/*****************************************************************************
* Purpose :
*
* 060403 Bob Jung.
*****************************************************************************/
BOOL AgcmFactorsDlg::OnAddModule()
{
	m_pcsAgpmFactors	= (AgpmFactors *)(GetModule("AgpmFactors"));
	if(!m_pcsAgpmFactors)
	{
		OutputDebugString("AgcmFactorsDlg::OnAddModule - <AgpmFactors 모듈을 가져올 수 없당>\n");
		return FALSE;
	}

	return TRUE;
}

/*****************************************************************************
* Purpose :
*
* 060403 Bob Jung.
*****************************************************************************/
BOOL AgcmFactorsDlg::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

/*****************************************************************************
* Purpose :
*
* 060403 Bob Jung.
*****************************************************************************/
BOOL AgcmFactorsDlg::OnDestroy()
{
	return TRUE;
}

/*****************************************************************************
* Purpose :
*
* 060903 Bob Jung.
*****************************************************************************/
BOOL AgcmFactorsDlg::OpenFactorsDlg(AgpdFactor *pcsFactor, eAgpdFactorsType eFactorsType, INT32 lSubType)
{
	//CFactorDlg dlg(pcsFactor, eFactorsType, lSubType);
	//dlg.DoModal();

	return TRUE;
}

/******************************************************************************
******************************************************************************/