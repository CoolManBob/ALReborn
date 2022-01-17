#pragma once

#include "AgcModule.h"
#include "AgpmEventGacha.h"
#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

class AgcmEventGacha : public AgcModule
{
public:
	enum	eCallbackPoint
	{
		CB_OPEN_GACHAUI	,
		CB_GACHA_ROLL	
	};

	BOOL	m_bGachaWindowOpened;

	BOOL	IsGachaWindowOpen() { return m_bGachaWindowOpened; };
	void	SetGachaWindowOpen( BOOL bOpen ) { m_bGachaWindowOpened = bOpen; }

	AgcmEventGacha();
	virtual ~AgcmEventGacha();

	// ... ApModule inherited
	virtual BOOL	OnAddModule();
	virtual void	OnLuaInitialize	( AuLua * pLua			);

	BOOL	SetCallbackOpenGachaUI	(ApModuleDefaultCallBack pfCallback, PVOID pClass)	{ return SetCallback( CB_OPEN_GACHAUI	, pfCallback , pClass ); }
	BOOL	SetCallbackGachaRoll	(ApModuleDefaultCallBack pfCallback, PVOID pClass)	{ return SetCallback( CB_GACHA_ROLL	, pfCallback , pClass ); }

	static BOOL CBEventRequest	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventGrant	(PVOID pData, PVOID pClass, PVOID pCustData);	
	static BOOL CBEventResult	(PVOID pData, PVOID pClass, PVOID pCustData);	
};
