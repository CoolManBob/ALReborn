// AgcmUIDebugInfo.cpp: implementation of the AgcmUIDebugInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmUIDebugInfo.h"

#define AGCMUIDEBUG_WINDOW_WIDTH	1024
#define AGCMUIDEBUG_WINDOW_HEIGHT	768

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmUIDebugInfo::AgcmUIDebugInfo()
{
	SetModuleName("AgcmUIDebugInfo");
	EnableIdle(TRUE);

	m_bEnableDebugInfo = FALSE;
	m_eDebugMode = AGCM_UIDEBUG_MODE_PROFILE;

	m_csDebugWindow.SetUIWindow( this );
}

AgcmUIDebugInfo::~AgcmUIDebugInfo()
{

}

BOOL	AgcmUIDebugInfo::OnAddModule()
{
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");
	m_pcsApmMap				= (ApmMap *)			GetModule("ApmMap");
	m_pcsAgcmMap			= (AgcmMap *)			GetModule("AgcmMap");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgcmItem			= (AgcmItem *)			GetModule("AgcmItem");
	m_pcsApmObject			= (ApmObject *)			GetModule("ApmObject");
	m_pcsAgcmObject			= (AgcmObject *)		GetModule("AgcmObject");
	m_pcsAgcmGrass			= (AgcmGrass *)			GetModule("AgcmGrass");
	m_pcsAgcmShadow2		= (AgcmShadow2 *)		GetModule("AgcmShadow2");
	m_pcsAgcmRender			= (AgcmRender *)		GetModule("AgcmRender");
	m_pcsAgcmSound			= (AgcmSound *)			GetModule("AgcmSound");
	m_pcsAgcmFont			= (AgcmFont *)			GetModule("AgcmFont");
	m_pcsAgcmResourceLoader	= (AgcmResourceLoader *)GetModule("AgcmResourceLoader");
	m_pcsAgcmEff2			= (AgcmEff2 * )			GetModule("AgcmEff2");

	return TRUE;
}

BOOL	AgcmUIDebugInfo::OnInit()
{
	return TRUE;
}

BOOL	AgcmUIDebugInfo::OnIdle(UINT32 ulClockCount)
{
#ifdef _PROFILE_
	AuProfileManager::Increment_Frame_Counter();
#endif

	return TRUE;
}

BOOL	AgcmUIDebugInfo::OnDestroy()
{
	EnableDebugInfo(FALSE);

	return TRUE;
}

BOOL	AgcmUIDebugInfo::EnableDebugInfo(BOOL bEnable)
{
	if (m_bEnableDebugInfo == bEnable)
		return FALSE;

	m_bEnableDebugInfo = bEnable;

	if (m_pcsAgcmUIManager2)
	{
		if (m_bEnableDebugInfo)
		{
			m_pcsAgcmUIManager2->AddWindow(&m_csDebugWindow);

			m_csDebugWindow.MoveWindow((m_csDebugWindow.pParent->w - AGCMUIDEBUG_WINDOW_WIDTH) / 2,
									   (m_csDebugWindow.pParent->h - AGCMUIDEBUG_WINDOW_HEIGHT) / 2,
									   AGCMUIDEBUG_WINDOW_WIDTH,
									   AGCMUIDEBUG_WINDOW_HEIGHT);

		}
		else
			m_pcsAgcmUIManager2->RemoveWindow(&m_csDebugWindow);

	}

	return TRUE;
}

VOID	AgcmUIDebugInfo::SetDebugMode(AgcmUIDebugMode eMode)
{
	m_eDebugMode = eMode;
}
