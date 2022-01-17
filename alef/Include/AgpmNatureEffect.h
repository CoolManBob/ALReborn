#pragma once

#include "ApBase.h"
#include "ApModule.h"
#include "AuPacket.h"

class AgpmNatureEffect : public ApModule
{
public:
	enum CBList
	{
		SET_NATURE_EFFECT		,	// 능동
		CHANGED_NATURE_EFFECT		// 수동 ( 패킷 리시브같은 경우.. )
			// 공통적으로 pData는 INT32 * 형으로 네이쳐 타입을 가짐.
	};

	enum	NEList
	{
		NE_NOT_DEFINED	= -1,
		NE_SNOW			= 0	,
		NE_RAIN			= 1
	}; // 그냥 적어놓고.. 클라이언트에ㅐ 주도적으로 돼어 있다.. 기본적인것만 등록함..

protected:
	INT32	m_nCurrentNatureEffect;

public:
	//	Packet
	AuPacket		m_csPacket;

public:
	AgpmNatureEffect();
	virtual ~AgpmNatureEffect();

	//	ApModule inherited
	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	// Callback
	BOOL	SetCallbackSetNatureEffect		(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(SET_NATURE_EFFECT			, pfnCallback, pvClass); }
	BOOL	SetCallbackChangedNatureEffect	(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(CHANGED_NATURE_EFFECT		, pfnCallback, pvClass); }

	// Methods
	INT32	SetNatureEffect( INT32 nNatureEffect = NE_NOT_DEFINED );
		// return prev nature effect index

	//	Event Packet
	PVOID	MakePacket( INT32 nNatureEffect, INT16 *pnPacketLength);
};

