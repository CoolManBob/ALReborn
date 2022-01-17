#ifndef __AGSM_RECRUIT_H__
#define __AGSM_RECRUIT_H__

#include "ApBase.h"
#include "AgsEngine.h"
#include "ApAdmin.h"
#include "AgpmRecruit.h"
#include "ApmMap.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgpmParty.h"
#include "AgpdItem.h"
#include "AgpmItem.h"
//#include "AgsmServerManager.h"
#include "AgsmServerManager2.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmRecruitD" )
#else
#pragma comment ( lib , "AgsmRecruit" )
#endif
#endif

class AgsmRecruit : public AgsModule
{
	ApmMap			*m_papmMap;
	AgpmCharacter	*m_pagpmCharacter;
	AgpmParty		*m_pagpmParty;
	AgpmFactors		*m_pagpmFactors;
	AgpmItem		*m_pagpmItem;
	AgsmServerManager *m_pAgsmServerManager;

	AuPacket		m_csPacket;     //Recruit 패킷.
	AuPacket		m_csLFPInfo;	//LFP
	AuPacket		m_csLFMInfo;	//LFM

public:
	AgsmRecruit();
	~AgsmRecruit();

	BOOL ProcessRegisterLFP( INT32 lLFPID );
	BOOL ProcessRegisterLFM( INT32 lLFMID, void *pvLFMInfo );

	//Registration
	BOOL SendRegisterLFP( INT32 lLFPID, char *pstrLFPID, INT32 lLevel, INT32 lClass, INT32 lResult );
	BOOL SendRegisterLFM( INT32 lLFMID, char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lLevel, INT32 lClass, char *pstrCharName, INT32 lLeaderID, INT32 lResult );

	//Update
	BOOL SendUpdateLFPInfo( INT32 lLFPID, char *pstrLFPID, INT32 lLevel, INT32 lClass, INT32 lResult );
//	BOOL SendUpdateLFMInfo( INT32 lLFMID, char *pstrLFMID, INT32 lLevel );
	//Cancel
	BOOL SendCancelLFP( INT32 lLFPID );
	BOOL SendCancelLFM( INT32 lLFMID );

	//어디에나 있는 OnAddModule이다. 특별할거 없다. 특별하면 안되나? 아햏햏~
	BOOL OnAddModule();
	//패킷을 받고 파싱하자~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
};

#endif
