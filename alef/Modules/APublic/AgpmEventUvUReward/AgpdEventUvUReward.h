//	AgpdEventUvUReward.h
///////////////////////////////////////////////////////////////

#ifndef	__AGPDEVENTUVUREWARD_H__
#define	__AGPDEVENTUVUREWARD_H__

#include "ApBase.h"

typedef enum	_eAgpmEventUvURewardType {
	AGPMEVENT_UVU_REWARD_NOT_SET				= 0,
	AGPMEVENT_UVU_REWARD_TYPE_EXP,
	AGPMEVENT_UVU_REWARD_TYPE_UNION_RANK,
	AGPMEVENT_UVU_REWARD_MAX,
} eAgpmEventUvURewardType;


class AgpdUvUReward {
public:
	eAgpmEventUvURewardType		m_eRewardType;
};

#endif	//__AGPDEVENTUVUREWARD_H__