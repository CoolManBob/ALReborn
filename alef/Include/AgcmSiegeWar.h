#pragma once

#include "AgpmCharacter.h"
#include "AgpmSiegeWar.h"
#include "AgpmGuild.h"

#include "AgcmCharacter.h"
#include "AgcmMinimap.h"

typedef enum  AgcmSiegeWarCBID {
	AGCMSIEGEWAR_CB_OPEN_CHECK_RESULT	= 0,
};

class AgcmSiegeWar : public AgcModule {
public:
	AgcmSiegeWar();
	virtual ~AgcmSiegeWar();

	BOOL	OnAddModule();

	struct	SiegeWarDisplayInfo
	{
		INT32	nCID			;
		INT32	nMinimapIndex	;

		SiegeWarDisplayInfo():nCID( -1 ),nMinimapIndex( -1 ){}
	};

	vector< SiegeWarDisplayInfo >	m_vecSiegeWarDisplayInfo;
	SiegeWarDisplayInfo *	GetSiegeWarDisplayInfo( INT32 nCID );
	BOOL					AddSiegeWarDisplayInfo( INT32 nCID , INT32 nMinimapIndex );
	BOOL					RemoveSiegeWarDisplayInfo( INT32 nCID );

private:
	AgpmCharacter	*m_pcsAgpmCharacter	;
	AgpmSiegeWar	*m_pcsAgpmSiegeWar	;
	AgpmGuild		*m_pcsAgpmGuild		;
	AgcmCharacter	*m_pcsAgcmCharacter	;
	AgcmMinimap		*m_pcsAgcmMinimap	;

	RwTexture		*m_pCatapultStatusTexture[2];
	RwTexture		*m_pAtkResTowerStatusTexture[2];

	RwTexture		*m_pCastleOwnerTexture[AGPMSIEGEWAR_MAX_CASTLE];

public:
	BOOL	SendNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullNextSiegeWarTime);

	BOOL	SendDefenseApplication(AgpdSiegeWar *pcsSiegeWar);
	BOOL	SendCancelDefenseApplication(AgpdSiegeWar *pcsSiegeWar);

	BOOL	SendAttackApplication(AgpdSiegeWar *pcsSiegeWar);
	BOOL	SendCancelAttackApplication(AgpdSiegeWar *pcsSiegeWar);

	BOOL	SendRequestAttackApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 nPage);
	BOOL	SendRequestDefenseApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 nPage);

	BOOL	SendSelectDefenseGuild(AgpdSiegeWar *pcsSiegeWar, CHAR *szGuildName);

	BOOL	SendRequestDefenseGuildList(AgpdSiegeWar *pcsSiegeWar);

	BOOL	SendRequestOpenAttackObject(AgpdCharacter *pcsTarget);
	BOOL	SendRequestUseAttackObject(AgpdCharacter *pcsTarget);
	BOOL	SendRequestRepairAttackObject(AgpdCharacter *pcsTarget);

	BOOL	SendAddItemToAttackObject(AgpdCharacter *pcsTarget, AgpdItem *pcsItem);
	BOOL	SendRemoveItemToAttackObject(AgpdCharacter *pcsTarget, AgpdItem *pcsItem);

	BOOL	SendCarveASeal(AgpdCharacter *pcsTarget);

	BOOL	ClickCharacter(AgpdCharacter *pcsTarget);

	BOOL	SetCatapultUseTexture(RwTexture *pTexture);
	BOOL	SetCatapultRepairTexture(RwTexture *pTexture);

	BOOL	SetAtkResTowerUseTexture(RwTexture *pTexture);
	BOOL	SetAtkResTowerRepairTexture(RwTexture *pTexture);

	BOOL	SetCastleOwnerTexture(RwTexture *pTexture, INT32 lIndex);

	RwTexture*	GetCatapultUseTexture();
	RwTexture*	GetCatapultRepairTexture();

	RwTexture*	GetAtkResTowerUseTexture();
	RwTexture*	GetAtkResTowerRepairTexture();

	RwTexture*	GetCastleOwnerTextureTexture(INT32 lIndex);

	INT32	GetSiegeWarGuildType(AgpdCharacter *pcsCharacter);

	static BOOL		CBInitCharacter		(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUpdateCharStatus	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRemoveCharacter	(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBRemoveNPCGuildCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SetCallbackOpenCheckResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};