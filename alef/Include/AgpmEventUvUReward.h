//	AgpmEventUvUReward.h
//		- Union vs Union 의 전투에서 얻는 보상과 관련된 모듈
///////////////////////////////////////////////////////////////

#ifndef	__AGPMEVENTUVUREWARD_H__
#define	__AGPMEVENTUVUREWARD_H__

#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"

#include "ApmEventManager.h"

#include "AgpdEventUvUReward.h"

#include "AuPacket.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventUvURewardD" )
#else
#pragma comment ( lib , "AgpmEventUvUReward" )
#endif


#define	AGPMEVENT_UVU_REWARD_STREAM_START		"UvURewardStart"
#define	AGPMEVENT_UVU_REWARD_STREAM_TYPE		"UvURewardType"
#define	AGPMEVENT_UVU_REWARD_STREAM_END			"UvURewardEnd"


// callback id
typedef enum	_eAgpmUvURewardCBID {
	AGPMEVENT_UVU_REWARD_CB_EXP					= 0,
	AGPMEVENT_UVU_REWARD_CB_UNION_RANK,
} eAgpmUvURewardCBID;


class AgpmEventUvUReward : public ApModule {
private:
	AgpmFactors				*m_pcsAgpmFactors;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItem				*m_pcsAgpmItem;

	ApmEventManager			*m_pcsApmEventManager;

public:
	AuPacket				m_csPacket;

public:
	AgpmEventUvUReward();
	~AgpmEventUvUReward();

	// Virtual Function 들
	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();

	static BOOL				ConAgpdUvUReward(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				DesAgpdUvUReward(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);

	PVOID					MakePacket(ApdEvent *pcsEvent, INT32 lCID, INT32 lIID, INT16 *pnPacketLength);

	BOOL					IsValidSkull(AgpdCharacter *pcsCharacter, AgpdItem *pcsSkull);

	BOOL					SetCallbackRewardExp(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRewardUnionRank(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGPMEVENTUVUREWARD_H__