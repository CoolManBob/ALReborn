// AgcmUIDebugInfo.h: interface for the AgcmUIDebugInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMUIDEBUGINFO_H__6D994291_6B9B_4413_8708_76CE782997AD__INCLUDED_)
#define AFX_AGCMUIDEBUGINFO_H__6D994291_6B9B_4413_8708_76CE782997AD__INCLUDED_

#include "AgcmUIManager2.h"
#include "AgcDebugWindow.h"
#include "AgcmFont.h"
#include "ApmMap.h"
#include "AgcmMap.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmItem.h"
#include "AgcmItem.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgcmGrass.h"
#include "AgcmRender.h"
#include "AgcmShadow2.h"
#include "AgcmSound.h"
#include "AgcmResourceLoader.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER < 1300
#ifdef	_DEBUG
	#pragma comment (lib , "AgcmUIDebugInfoD")
#else
	#pragma comment (lib , "AgcmUIDebugInfo")
#endif
#endif

typedef enum
{
	AGCM_UIDEBUG_MODE_PROFILE	= 0,
	AGCM_UIDEBUG_MODE_MEMORY,
	AGCM_UIDEBUG_MODE_OBJECT,
	AGCM_UIDEBUG_MODE_GRAPHIC,
	AGCM_UIDEBUG_MODE_SOUND,
	AGCM_UIDEBUG_MODE_NETWORK,
	AGCM_UIDEBUG_MAX_MODE
} AgcmUIDebugMode;

class AgcmUIDebugInfo : public AgcModule  
{
private:
	AgcmUIManager2 *	m_pcsAgcmUIManager2;
	AgcDebugWindow		m_csDebugWindow;

	BOOL				m_bEnableDebugInfo;
	AgcmUIDebugMode		m_eDebugMode;

public:
	ApmMap *			m_pcsApmMap;
	AgcmMap *			m_pcsAgcmMap;
	AgpmCharacter *		m_pcsAgpmCharacter;
	AgcmCharacter *		m_pcsAgcmCharacter;
	AgpmItem *			m_pcsAgpmItem;
	AgcmItem *			m_pcsAgcmItem;
	ApmObject *			m_pcsApmObject;
	AgcmObject *		m_pcsAgcmObject;
	AgcmGrass *			m_pcsAgcmGrass;
	AgcmShadow2 *		m_pcsAgcmShadow2;
	AgcmRender *		m_pcsAgcmRender;
	AgcmSound *			m_pcsAgcmSound;
	AgcmFont *			m_pcsAgcmFont;
	AgcmResourceLoader *m_pcsAgcmResourceLoader;
	AgcmEff2*			m_pcsAgcmEff2;

public:
	AgcmUIDebugInfo();
	virtual ~AgcmUIDebugInfo();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnIdle(UINT32 unClockCount);
	BOOL	OnDestroy();

	BOOL	EnableDebugInfo(BOOL bEnable = TRUE);
	inline	VOID	ToggleDebugInfo()	{ EnableDebugInfo(!m_bEnableDebugInfo);	}

	VOID	SetDebugMode(AgcmUIDebugMode eMode);
	inline	AgcmUIDebugMode		GetCurrentDebugMode()	{ return m_eDebugMode;	}

	VOID	ChangeTextureFilterMode();
};

#endif // !defined(AFX_AGCMUIDEBUGINFO_H__6D994291_6B9B_4413_8708_76CE782997AD__INCLUDED_)
