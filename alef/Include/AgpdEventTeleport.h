#ifndef	_AGPDEVENTTELEPORT_H_
#define _AGPDEVENTTELEPORT_H_

#include "ApBase.h"

#define	AGPD_TELEPORT_MAX_POINT_ID				300
#define	AGPD_TELEPORT_MAX_POINT_LIST			100
#define AGPD_TELEPORT_MAX_POINT_OBJECT			30
#define	AGPD_TELEPORT_MAX_POINT_DESCRIPTION		128
#define	AGPD_TELEPORT_MAX_POINT_NAME			32

#define	AGPD_TELEPORT_MAX_GROUP					20
#define	AGPD_TELEPORT_MAX_GROUP_DESCRIPTION		128
#define AGPD_TELEPORT_MAX_GROUP_POINT			30
#define	AGPD_TELEPORT_MAX_GROUP_NAME			32

typedef enum
{
	AGPDTELEPORT_TARGET_TYPE_POS			= 1,
	//AGPDTELEPORT_TARGET_TYPE_INDEX,
	AGPDTELEPORT_TARGET_TYPE_BASE,
	AGPDTELEPORT_TARGET_TYPE_NONE,
} AgpdTeleportTarget;

typedef enum
{
	AGPDTELEPORT_REGION_TYPE_NORMAL			= 0,
	AGPDTELEPORT_REGION_TYPE_PVP,
} AgpdTeleportRegionType;

typedef enum
{
	AGPDTELEPORT_SPECIAL_TYPE_NORMAL		= 0,
	AGPDTELEPORT_SPECIAL_TYPE_RETURN_ONLY,
	AGPDTELEPORT_SPECIAL_TYPE_SIEGEWAR,
	AGPDTELEPORT_SPECIAL_TYPE_CASTLE_TO_DUNGEON,
	AGPDTELEPORT_SPECIAL_TYPE_DUNGEON_TO_LANSPHERE,
	AGPDTELEPORT_SPECIAL_TYPE_LANSPHERE,
	APPDTELEPORT_SPECIAL_TYPE_MAX
} AgpdTeleportSpecialType;

class AgpdTeleportGroup;
typedef struct AgpdTeleportAttach	AgpdTeleportAttach;

class AgpdTeleportPoint : public ApBase
{
public:
	enum eTeleportUseType
	{
		eTeleportUseHuman		= 0x01,
		eTeleportUseOrc			= 0x02,
		eTeleportUseMoonElf		= 0x04,
		eTeleportUseDragonScion	= 0x08,
	};
public:
	CHAR					m_szPointName[AGPD_TELEPORT_MAX_POINT_NAME + 1];

	AgpdTeleportTarget		m_eType;

	union
	{
		AuPOS				m_stPos;

		struct
		{
			ApBaseType		m_eBaseType;
			INT32			m_lID;
		} m_stBase;
	} m_uniTarget;

	CHAR					m_szDescription[AGPD_TELEPORT_MAX_POINT_DESCRIPTION + 1];

	FLOAT					m_fRadiusMin;
	FLOAT					m_fRadiusMax;

	BOOL					m_bAttachEvent;			// Event에 의해서 Attach된 것이다. (TRUE일 경우 Event를 지우는 방법으로 지워야 한다.)
	AgpdTeleportAttach *	m_pstAttach;

	AuList <AgpdTeleportGroup *>	m_listGroup;
	AuList <AgpdTeleportGroup *>	m_listTargetGroup;

	AgpdTeleportRegionType	m_eRegionType;
	AgpdTeleportSpecialType	m_eSpecialType;

	BOOL					m_bActive;		// 아크로드 시스템에서 텔레포트 활성화 여부 확인

	BYTE					m_byUseType;	//가능한종족을 알려준다..

	//m_byUseType == 0이면 모든 종족이 사용이 가능하다
	BOOL	IsUseHuman()			{	return 	m_byUseType ? m_byUseType & eTeleportUseHuman : TRUE;	}
	BOOL	IsUseOrc()				{	return 	m_byUseType ? m_byUseType & eTeleportUseOrc : TRUE;		}
	BOOL	IsUseMoonElf()			{	return 	m_byUseType ? m_byUseType & eTeleportUseMoonElf : TRUE;	}
	BOOL	IsUseDragonScion()		{	return 	m_byUseType ? m_byUseType & eTeleportUseDragonScion : TRUE;	}
	BOOL	IsUseType( eTeleportUseType eType )	{	return m_byUseType ? m_byUseType & eType : TRUE;	}

	void	SetUseType( eTeleportUseType eType )	{	m_byUseType |= eType;	}
};

class AgpdTeleportGroup : public ApBase
{
public:
	CHAR		m_szGroupName[AGPD_TELEPORT_MAX_GROUP_NAME + 1];
	CHAR		m_szDescription[AGPD_TELEPORT_MAX_GROUP_DESCRIPTION + 1];

	AuList <AgpdTeleportPoint *>	m_listPoint;
};

struct AgpdTeleportAttach
{
	INT32					m_lPointID;
	AgpdTeleportPoint *		m_pcsPoint;

	AgpdTeleportAttach() : m_lPointID(0), m_pcsPoint(NULL) {}
};

struct AgpdTeleportAttachToChar
{
	AgpdTeleportPoint *		m_pcsLastUseTeleportPoint;
};


#endif //_AGPDEVENTTELEPORT_H_