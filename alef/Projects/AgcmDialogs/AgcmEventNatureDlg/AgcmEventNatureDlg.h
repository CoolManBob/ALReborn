// AgcmEventNatureDlg.h: interface for the AgcmEventNatureDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMEVENTNATUREDLG_H__08507056_7286_4007_99C2_24128F5671C1__INCLUDED_)
#define AFX_AGCMEVENTNATUREDLG_H__08507056_7286_4007_99C2_24128F5671C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApModule.h"
//#include "AgcmCharacter.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"
#include "AgcmEventNature.h"
#include "AgcmMap.h"
#include "AgcmUIControl.h"

class AFX_EXT_CLASS AgcmEventNatureDlg : public ApmEventDialog  
{
public:
	ApmEventManager		*	m_pcsApmEventManager	;
	ApmEventManagerDlg	*	m_pcsApmEventManagerDlg	;
	AgpmEventNature		*	m_pcsAgpmEventNature	;
	AgcmEventNature		*	m_pcsAgcmEventNature	;
	AgcmMap				*	m_pcsAgcmMap			;

	char				m_strCurrentDirectory[ 1024 ];

	AgcmEventNatureDlg();
	virtual ~AgcmEventNatureDlg();

public:
	// Virtual Functions...
	BOOL	OnAddModule();
	BOOL	Open(ApdEvent *pstEvent);
	void	Close();
	
	BOOL	OpenEditTemplateDialog	( char * pToolDirectory );
	BOOL	OpenClipPlaneDlg		();
	
//	void	SetRwEngineInstace( void * pRwEngineInstance );
	BOOL	( *pResampleTextureFunction )( char * pSource , char * pDestination , INT32 nWidth , INT32 nHeight );
	BOOL	( * p__SetTimeFunction		)( AgcmEventNature * pEventNature , INT32 hour , INT32 minutes );
	void	( * p__SetFogFunction		)( AgcmEventNature * pEventNature , 
		BOOL		bFogOn			, FLOAT		fFogDistance	, FLOAT		fFogDensity	, RwRGBA *	pstFogColor	);

	void	( * p__MoveSky				)( AgcmEventNature * pEventNature );
	//void	( * p__SetSkyRadius			)( AgcmEventNature * pEventNature ,  FLOAT		fRadius		);
	UINT32	( * p__OpenEffectList		)();
	
	INT32	( * p__SetSkyTemplateID		)( AgcmEventNature * pEventNature , ASkySetting	* pSkySet	, INT32 nFlag );
	UINT32	( * p__GetSoundLength		)( UINT32 lEid );
};

#endif // !defined(AFX_AGCMEVENTNATUREDLG_H__08507056_7286_4007_99C2_24128F5671C1__INCLUDED_)
