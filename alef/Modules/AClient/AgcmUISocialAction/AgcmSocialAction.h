#pragma once

#include <vector>

#include "AgpdGrid.h"
#include "AgcModule.h"
#include "AgcmResourceLoader.h"
#include "AgcmUIControl.h"
#include "AgcmUIManager2.h"

#define SOCIALACTION_MAX	 AGPDCHAR_SOCIAL_TYPE_SPECIAL1

struct SocialAction
{
	SocialAction( char* szIcon, char* szCommand, char* szTooltipTitle, char* szTooltipText );

	char	szIcon[128];			//Icon Filename
	char	szCommand[128];			//Chatting Commnae
	char	szTooltipTitle[32];
	char	szTooltipText[512];

	AgpdGridItem*	pGridItem;
	RwTexture*		pTexture;
};
typedef SocialAction* LPSocialAction;
typedef std::vector< SocialAction* >	SocialActionVec;
typedef SocialActionVec::iterator		SocialActionVecItr;

class AgcmSocialActon
{
public:
	AgcmSocialActon();
	~AgcmSocialActon();

	BOOL	Create();
	void	Destory();

	BOOL	Load( char* szFilename, BOOL bDecryption );

	const SocialAction*	GetInfo( UINT32 nNum )	{	return nNum < AGPDCHAR_SOCIAL_TYPE_SPECIAL1 ? m_vecInfo[nNum] : NULL;	}

protected:
	void	SetModule( AgpmGrid* pAgpmGrid, AgcmResourceLoader* pResourceLoader, AgcmUIControl* pUIControl, AgcmUIManager2* pUIManager2 );
	BOOL	OpenTooltip( int nIndex );
	void	CloseTooltip();

protected:
	AgpmGrid*			m_pcsAgpmGrid;
	AgcmResourceLoader*	m_pcsAgcmResourceLoader;
	AgcmUIControl*		m_pcsAgcmUIControl;
	AgcmUIManager2*		m_pcsAgcmUIManager2;

	AgpdGrid			m_cGrid;
	SocialActionVec		m_vecInfo;

	AcUIToolTip			m_cTooltip;
};