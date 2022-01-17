#ifndef	__AGSDZONING_H__
#define	__AGSDZONING_H__

#include "ApBase.h"

typedef enum _ZONING_STATUS {
	AGSMZONING_NOT_ZONING					= 0,
	AGSMZONING_ZONING_START_ZONING,					// 조닝을 시작한다.
	AGSMZONING_ZONING_PROCESS_ZONING,				// 조닝을 정상적으로 시작해서 처리중이다.
	AGSMZONING_ZONING_PASSCONTROL,					// character control을 넘긴다.
	AGSMZONING_ZONING_SUCCESS_PASSCONTROL,			// character control이 넘어갔다.
	AGSMZONING_ZONING_END_ZONING
} ZONING_STATUS;

typedef struct _stZONING_STATUS {
	INT32		lServerID;
	INT16		fZoningStatus;
	BOOL		bConnectZoneServer;
	BOOL		bTeleport;
} stZoningStatus;


class AgsdZoning {
public:
	BOOL				m_bZoning;				// 조닝 하고 있는지 여부
//	stZoningStatus		m_stZoningStatus[5];	// 조닝 상태....
	ApSafeArray<stZoningStatus, 5>	m_stZoningStatus;

	BOOL				m_bDisconnectZoning;	// 조닝 과정에서 disconnect 가 발생하는 경우 이값을 TRUE로 설정해준다.
												// 이값이 TRUE이면 이 서버에서 character 데이타를 계속 유지시킨다.
};

#endif	//__AGSDZONING_H__