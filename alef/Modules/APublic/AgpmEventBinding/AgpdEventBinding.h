//	AgpdEventBinding.h
//////////////////////////////////////////////////////////////////////

#ifndef	__AGPDEVENTBINDING_H__
#define	__AGPDEVENTBINDING_H__

#include "ApDefine.h"
#include "ApBase.h"
#include "AgpmFactors.h"

const int	AGPDBINDING_MAX_NAME							= 64;
const int	AGPDBINDING_MAX_TOWN_NAME						= 64;

#define	AGPDBINDING_STREAM_BINDING_NAME						"BindingName"
#define	AGPDBINDING_STREAM_TOWN_NAME						"TownName"
#define	AGPDBINDING_STREAM_RADIUS							"Radius"
#define	AGPDBINDING_STREAM_BINDING_TYPE						"BindingType"
#define AGPDBINDING_STREAM_NAME_EVENT_END					"BindingEnd"
#define	AGPDBINDING_STREAM_CHARACTER_RACE_TYPE				"CharRaceType"
#define	AGPDBINDING_STREAM_CHARACTER_CLASS_TYPE				"CharClassType"

typedef enum _AgpdBindingType {
	AGPDBINDING_TYPE_NONE									= 0,
	AGPDBINDING_TYPE_RESURRECTION							= 1,
	AGPDBINDING_TYPE_NEW_CHARACTER							= 2,
	AGPDBINDING_TYPE_SIEGEWAR_OFFENSE						= 3,
	AGPDBINDING_TYPE_SIEGEWAR_DEFENSE_INNER					= 4,
	AGPDBINDING_TYPE_SIEGEWAR_ARCHLORD						= 5,
	AGPDBINDING_TYPE_SIEGEWAR_ARCHLORD_ATTACKER				= 6,
	AGPDBINDING_TYPE_SIEGEWAR_DEFENSE_OUTTER				= 7,
	AGPDBINDING_TYPE_MAX									= 8
} AgpdBindingType;


class AgpdBinding : public ApBase {
public:
	CHAR					m_szBindingName[AGPDBINDING_MAX_NAME + 1];

	CHAR					m_szTownName[AGPDBINDING_MAX_TOWN_NAME + 1];

	AuPOS					m_stBasePos;
	UINT32					m_ulRadius;

	AgpdFactorCharType		m_csFactorCharType;

	AgpdBindingType			m_eBindingType;
};

typedef struct _AgpdBindingAttach {
	INT32					m_lBindingID;
	AgpdBinding				*m_pcsBinding;
} AgpdBindingAttach, *pstAgpdBindingAttach;


#endif	//__AGPDEVENTBINDING_H__