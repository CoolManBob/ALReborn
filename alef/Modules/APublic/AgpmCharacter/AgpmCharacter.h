#ifndef		_AGPMCHARACTER_H_
#define		_AGPMCHARACTER_H_

#include "ApBase.h"
#include "ApString.h"
#include "ApModuleStream.h"
#include "ApMutualEx.h"
#include "AuPacket.h"
#include "ApmMap.h"
#include "AgpmFactors.h"
#include "AgpaCharacter.h"
#include "AgpaCharacterTemplate.h"
#include "AuExcelTxtLib.h"
#include "AgpdCharacterImportData.h"
#include "AgpmConfig.h"

#include <map>
#include <vector>

#if defined (_AREA_GLOBAL_) || defined(_AREA_KOREA_)
#include "AuGameGuard.h"
#endif

using namespace std;

#define AGPMCHARACTER_MAX_MURDERER_POINT			99999

#define	AGPMCHARACTER_PROCESS_REMOVE_INTERVAL		3000
#define	AGPMCHARACTER_PRESERVE_CHARACTER_DATA		5000

#define AGPMCHARACTER_MOVE_FLAG_DIRECTION			0x01
#define AGPMCHARACTER_MOVE_FLAG_PATHFINDING			0x02
#define AGPMCHARACTER_MOVE_FLAG_STOP				0x04
#define AGPMCHARACTER_MOVE_FLAG_SYNC				0x08
#define AGPMCHARACTER_MOVE_FLAG_FAST				0x10
#define AGPMCHARACTER_MOVE_FLAG_HORIZONTAL			0x20
#define	AGPMCHARACTER_MOVE_FLAG_FOLLOW				0x40

#define AGPMCHAR_FLAG_TRANSFORM		0x01
#define AGPMCHAR_FLAG_RIDABLE		0x02
#define AGPMCHAR_FLAG_EVOLUTION		0x04

#define CHARISMA_TYPE_MAX			13

typedef enum AgpmCharacterPacketOperation {
	AGPMCHAR_PACKET_OPERATION_ADD	= 0,
	AGPMCHAR_PACKET_OPERATION_UPDATE,
	AGPMCHAR_PACKET_OPERATION_REMOVE,
	AGPMCHAR_PACKET_OPERATION_REMOVE_FOR_VIEW,
	AGPMCHAR_PACKET_OPERATION_SELECT,
	AGPMCHAR_PACKET_OPERATION_LEVEL_UP,
	AGPMCHAR_PACKET_OPERATION_ADD_ATTACKER,
	AGPMCHAR_PACKET_OPERATION_MOVE_BANKMONEY,
	AGPMCHAR_PACKET_OPERATION_DISCONNECT_BY_ANOTHER_USER,
	AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_TOWN,
	AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_NOW,
	AGPMCHAR_PACKET_OPERATION_TRANSFORM,
	AGPMCHAR_PACKET_OPERATION_RESTORE_TRANSFORM,
	AGPMCHAR_PACKET_OPERATION_CANCEL_TRANSFORM,
	AGPMCHAR_PACKET_OPERATION_RIDABLE,
	AGPMCHAR_PACKET_OPERATION_RESTORE_RIDABLE,
	AGPMCHAR_PACKET_OPERATION_SOCIAL_ANIMATION,
	AGPMCHAR_PACKET_OPERATION_OPTION_UPDATE,
	AGPMCHAR_PACKET_OPERATION_BLOCK_BY_PENALTY,
	AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_SIEGE_INNER,
	AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_SIEGE_OUTER,
	AGPMCHAR_PACKET_OPERATION_ONLINE_TIME,
	AGPMCHAR_PACKET_OPERATION_NPROTECT_AUTH,
	AGPMCHAR_PACKET_OPERATION_UPDATE_SKILLINIT_STRING,
	AGPMCHAR_PACKET_OPERATION_RESURRECTION_BY_OTHER,
	AGPMCHAR_PACKET_OPERATION_EVENT_EFFECT_ID,
	AGPMCHAR_PACKET_OPERATION_EVOLUTION,
	AGPMCHAR_PACKET_OPERATION_RESTORE_EVOLUTION,
	AGPMCHAR_PACKET_OPERATION_REQUEST_PINCHWANTED_CHARACTER,
	AGPMCHAR_PACKET_OPERATUIN_ANSWER_PINCHWANTED_CHARACTER,
} AgpmCharacterPacketOperation;

typedef enum AgpmCharacterDataType {
	AGPMCHAR_DATA_TYPE_CHAR			= 0,
	AGPMCHAR_DATA_TYPE_TEMPLATE,
	AGPMCHAR_DATA_TYPE_STATIC,
	AGPMCHAR_DATA_TYPE_CHAR_KIND,
	AGPMCHAR_DATA_TYPE_STATIC_ONE,
} AgpmCharacterDataType;

typedef enum AgpmCharacterProcessType {
	AGPMCHAR_PROCESS_ADD_CHAR,
	AGPMCHAR_PROCESS_REMOVE_CHAR,
	AGPMCHAR_PROCESS_UPDATE_CHAR
} AgpmCharacterProcessType;

typedef enum _eAgpmCharacterTransformType {
	AGPMCHAR_TRANSFORM_TYPE_APPEAR_ONLY		= 0,
	AGPMCHAR_TRANSFORM_TYPE_STATUS_ONLY,
	AGPMCHAR_TRANSFORM_TYPE_APPEAR_STATUS_ALL,
} eAgpmCharacterTransformType;

typedef enum _eAgpmCharacterPenalty {
	AGPMCHAR_PENALTY_NONE = -1,
	AGPMCHAR_PENALTY_PRVTRADE = 0,
	AGPMCHAR_PENALTY_NPCTRADE,
	AGPMCHAR_PENALTY_CONVERT,
	AGPMCHAR_PENALTY_AUCTION,
	AGPMCHAR_PENALTY_CHARCUST,
	AGPMCHAR_PENALTY_EXP_LOSE,
	AGPMCHAR_PENALTY_FIRST_ATTACK,
	AGPMCHAR_PENALTY_MAX,
} eAgpmCharacterPenalty;

typedef enum _eAgpmCharacterResurrect {
	AGPMCHAR_RESURRECT_NOW = 0,
	AGPMCHAR_RESURRECT_TOWN,
	AGPMCHAR_RESURRECT_SIEGE_INNER,
	AGPMCHAR_RESURRECT_SIEGE_OUTER,
	AGPMCHAR_RESURRECT_MAX,
} eAgpmCharacterResurrect;

typedef enum AgpmCharacterCallbackPoint			// Callback ID in Character Module 
{
	AGPMCHAR_CB_ID_INIT			= 0,
	AGPMCHAR_CB_ID_NEW_CREATED_CHAR,
	AGPMCHAR_CB_ID_REMOVE,
	AGPMCHAR_CB_ID_REMOVE_ID,
	AGPMCHAR_CB_ID_RESET_MONSTER,
	AGPMCHAR_CB_ID_DELETE,
	AGPMCHAR_CB_ID_MOVE,
	AGPMCHAR_CB_ID_STOP,
	AGPMCHAR_CB_ID_UPDATE_MONEY,
	AGPMCHAR_CB_ID_UPDATE_FACTOR,
	AGPMCHAR_CB_ID_UPDATE_STATUS,
	AGPMCHAR_CB_ID_UPDATE_ACTION_STATUS,
	AGPMCHAR_CB_ID_UPDATE_ACTION_STATUS_CHECK,
	AGPMCHAR_CB_ID_UPDATE_SPECIAL_STATUS,
	AGPMCHAR_CB_ID_UPDATE_CRIMINAL_STATUS,
	AGPMCHAR_CB_ID_UPDATE_POSITION_CHECK,
	AGPMCHAR_CB_ID_UPDATE_POSITION,
	AGPMCHAR_CB_ID_UPDATE_CHAR,
	AGPMCHAR_CB_ID_CHECK_IS_ACTION_MOVE,
	AGPMCHAR_CB_ID_ACTION_ATTACK,
	AGPMCHAR_CB_ID_ACTION_ATTACK_MISS,
	AGPMCHAR_CB_ID_ACTION_SKILL,
	AGPMCHAR_CB_ID_ACTION_PICKUP_ITEM,
	AGPMCHAR_CB_ID_ACTION_EVENT_TELEPORT,
	AGPMCHAR_CB_ID_ACTION_EVENT_NPC_TRADE,
	AGPMCHAR_CB_ID_PINCHWANTED_ANSWER,
	//AGPMCHAR_CB_ID_ACTION_EVENT_MASTERY_SPECIALIZE,		//	사용 안하고 있음
	AGPMCHAR_CB_ID_ACTION_EVENT_BANK,
	AGPMCHAR_CB_ID_ACTION_EVENT_ITEMCONVERT,
	AGPMHCAR_CB_ID_ACTION_EVENT_GUILD,
	AGPMCHAR_CB_ID_ACTION_EVENT_PRODUCT,
	AGPMCHAR_CB_ID_ACTION_EVENT_NPC_DIALOG,
	AGPMCHAR_CB_ID_ACTION_EVENT_SKILL_MASTER,
	AGPMCHAR_CB_ID_ACTION_EVENT_REFINERY,
	AGPMCHAR_CB_ID_ACTION_EVENT_QUEST,
	AGPMCHAR_CB_ID_ACTION_PRODUCT_SKILL,
	AGPMCHAR_CB_ID_ACTION_EVENT_CHARCUSTOMIZE,
	AGPMCHAR_CB_ID_ACTION_EVENT_ITEMREPAIR,
	AGPMCHAR_CB_ID_ACTION_EVENT_REMISSION,
	AGPMCHAR_CB_ID_ACTION_EVENT_WANTEDCRIMINAL,
	AGPMCHAR_CB_ID_ACTION_EVENT_SIEGE_WAR,
	AGPMCHAR_CB_ID_ACTION_USE_SIEGEWAR_ATTACK_OBJECT,
	AGPMCHAR_CB_ID_ACTION_CARVE_A_SEAL,
	AGPMCHAR_CB_ID_RECALC_FACTOR,				// factor를 다쉬 다 계산할 필요가 생겼을때....
	AGPMCHAR_CB_ID_UPDATE_HP,
	AGPMCHAR_CB_ID_UPDATE_MAX_HP,
	AGPMCHAR_CB_ID_UPDATE_LEVEL,
	AGPMCHAR_CB_ID_CHECK_REMOVE_CHAR,
	ACPMCHAR_CB_ID_UI_SET_MONEY,
	AGPMCHAR_CB_ID_UPDATE_BANK_MONEY,
	AGPMCHAR_CB_ID_MOVE_BANK_MONEY,
	AGPMCHAR_CB_ID_TRANSFORM_APPEAR,
	AGPMCHAR_CB_ID_TRANSFORM_STATUS,
	AGPMCHAR_CB_ID_RESTORE_TRANSFORM,
	AGPMCHAR_CB_ID_CANCEL_TRANSFORM,
	AGPMCHAR_CB_ID_UPDATE_MURDERER_POINT,
	AGPMCHAR_CB_ID_UPDATE_MUKZA_POINT,
	AGPMCHAR_CB_ID_UPDATE_SKILL_POINT,
	AGPMCHAR_CB_ID_ADD_ATTACKER_TO_LIST,
	AGPMCHAR_CB_ID_RECEIVE_ACTION,
	AGPMCHAR_CB_ID_CHECK_NOW_UPDATE_ACTION_FACTOR,
	AGPMCHAR_CB_ID_CHECK_VALID_NORMAL_ATTACK,
	AGPMCHAR_CB_ID_CHECK_ACTION_REQUIREMENT,
	AGPMCHAR_CB_ID_CHECK_ACTION_ATTACK_TARGET,
	AGPMCHAR_CB_ID_PAY_ACTION_COST,
	AGPMCHAR_CB_ID_RECEIVE_CHARACTER_DATA,
	AGPMCHAR_CB_ID_POST_ADD,						// Character Add다 처리한 후 - Login에서 사용 98pastel
	AGPMCHAR_CB_ID_DISCONNECT_BY_ANOTHER_USER,		// 누군가에 의해 끊긴 경우.
	AGPMCHAR_CB_ID_MONSTER_AI_PATHFIND,				// 몬스터 길찾기 AI를 위해 Callback
	AGPMCHAR_CB_ID_STREAM_READ_IMPORT_DATA,
	AGPMCHAR_CB_ID_STREAM_READ_IMPORT_DATA_ERROR_REPORT,
	AGPMCHAR_CB_ID_REQUEST_RESURRECTION,
	AGPMCHAR_CB_ID_GET_NEW_CID,
	AGPMCHAR_CB_ID_IS_PLAYER_CHARACTER,
	AGPMCHAR_CB_ID_ADD_CHARACTER_TO_MAP,
	AGPMCHAR_CB_ID_REMOVE_CHARACTER_FROM_MAP,
	AGPMCHAR_CB_ID_SET_COMBAT_MODE,
	AGPMCHAR_CB_ID_RESET_COMBAT_MODE,
	AGPMCHAR_CB_ID_CHECK_PROCESS_IDLE,
	AGPMCHAR_CB_ID_BINDING_REGION_CHANGE,
	AGPMCHAR_CB_ID_SOCIAL_ANIMATION,
	AGPMCHAR_CB_ID_RELEASE_ACTION_MOVE_LOCK,
	AGPMCHAR_CB_ID_UPDATE_CUSTOMIZE,
	AGPMCHAR_CB_ID_UPDATE_OPTION_FLAG,		//	2005.05.31. By SungHoon 거부 목록이 변할 경우
	AGPMCHAR_CB_ID_UPDATE_LEVEL_POST,		//	2005.06.14. By SungHoon 레벨 업 후 불릴 CallBack
	AGPMCHAR_CB_ID_UPDATE_BANK_SIZE,
	AGPMCHAR_CB_ID_GET_ITEM_LANCER,
	AGPMCHAR_CB_ID_REMOVE_RIDE,
	//@{ Jaewon 20050902
	// For fade-in/out
	AGPMCHAR_CB_ID_CHECK_REMOVE_CHAR2,
	AGPMCHAR_CB_ID_ALREADY_EXIST_CHAR,
	AGPMCHAR_CB_ID_UPDATE_EVENTSTATUSFLAG,
	//@} Jaewon
	AGPMCHAR_CB_ID_INIT_TEMPLATE_DEFAULT_VALUE,
	AGPMCHAR_CB_ID_BLOCK_BY_PENALTY,
	AGPMCHAR_CB_ID_UPDATE_CASH,
	AGPMCHAR_CB_ID_READ_ALL_CHARACTER_TEMPLATE,
	AGPMCHAR_CB_ID_CHECK_ALL_BLOCK_STATUS,
	AGPMCHAR_CB_ID_ACTION_EVENT_TAX,
	ACPMCHAR_CB_ID_PAY_TAX,
	ACPMCHAR_CB_ID_GET_TAX_RATIO,
	AGPMHCAR_CB_ID_ACTION_EVENT_GUILD_WAREHOUSE,
	AGPMCHAR_CB_ID_ADD_STATIC_CHARACTER,
	AGPMCHAR_CB_ID_REMOVE_PROTECTED_NPC,
	AGPMCHAR_CB_ID_USE_EFFECT,
	AGPMCHAR_CB_ID_IS_STATIC_CHARACTER,
	AGPMCHAR_CB_ID_IS_ARHCLORD,
	AGPMCHAR_CB_ID_ACTION_EVENT_ARCHLORD,
	AGPMCHAR_CB_ID_UPDATE_CHARISMA_POINT,
	AGPMCHAR_CB_ID_GET_BONUS_DROP_RATE,
	AGPMCHAR_CB_ID_GET_BONUS_DROP_RATE2,
	AGPMCHAR_CB_ID_GET_BONUS_MONEY_RATE,
	AGPMCHAR_CB_ID_GET_BONUS_CHARISMA_RATE,
	AGPMCHAR_CB_ID_ACTION_EVENT_GAMBLE,
	AGPMCHAR_CB_ID_UPDATE_NICKNAME,
	AGPMCHAR_CB_ID_ONLINE_TIME,
	AGPMCHAR_CB_ID_GAMEGUARD_AUTH,
	AGPMCHAR_CB_ID_RESURRECTION_BY_OTHER,
	AGPMCHAR_CB_ID_EVENT_EFFECT_ID,
	AGPMCHAR_CB_ID_ACTION_EVENT_GACHA,
	AGPMCHAR_CB_ID_PRE_UPDATE_POSITION,
	AGPMCHAR_CB_ID_EVOLUTION,
	AGPMCHAR_CB_ID_RESTORE_EVOLUTION,
	AGPMCHAR_CB_ID_PINCHWANTED_REQUEST,
	AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE,
} AgpmCharacterCallbackPoint;

/*
enum eCheckEnableActionState
{
	AGPMCHAR_CHECK_ENABLE_ACTION_OK = 0,
	AGPMCHAR_CHECK_DISABLE_ACTION_SLEEP,		// Sleep 상태라 이동할 수 없는 상태
	AGPMCHAR_CHECK_DISABLE_ACTION_STUN,			// Stun 상태라 이동할 수 없는 상태
	AGPMCHAR_CHECK_DISABLE_ACTION_HOLD,			// Hold 상태라 이동할 수 없는 상태
	AGPMCHAR_CHECK_DISABLE_ACTION_BLOCK,		// Block 상태라 이동할 수 없는 상태
	AGPMCHAR_CHECK_DISABLE_ACTION_CONFUSION,	// Confusion 상태라 이동이 랜덤
	AGPMCHAR_CHECK_DISABLE_ACTION_FEAR,			// Fear 상태라 이동이 랜덤
	AGPMCHAR_CHECK_DISABLE_ACTION_BERSERK,		// Berserk 상태라 이동이 랜덤
	AGPMCHAR_CHECK_DISABLE_ACTION_SHRINK,		// Shrink 상태라 이동이 랜덤
	AGPMCHAR_CHECK_DISABLE_ACTION_ETC,			// 기타다른 이유로 이동할 수 없는 상태
	AGPMCHAR_CHECK_ENABLE_ACTION_MAx
};
*/

#define AGPMCHAR_INI_NAME_NAME				"Name"
#define AGPMCHAR_INI_NAME_BTYPE				"BlockingType"
#define	AGPMCHAR_INI_NAME_CHAR_TYPE			"CharacterType"

#define AGPMCHAR_INI_NAME_BOX_INF			"BoxInf"
#define AGPMCHAR_INI_NAME_BOX_SUP			"BoxSup"
#define AGPMCHAR_INI_NAME_SPHERE_CENTER		"SphereCenter"
#define AGPMCHAR_INI_NAME_SPHERE_RADIUS		"SphereRadius"
#define AGPMCHAR_INI_NAME_CYLINDER_CENTER	"CylinderCenter"
#define AGPMCHAR_INI_NAME_CYLINDER_HEIGHT	"CylinderHeight"
#define AGPMCHAR_INI_NAME_CYLINDER_RADIUS	"CylinderRadius"

#define AGPMCHAR_INI_NAME_BOX				"Box"
#define AGPMCHAR_INI_NAME_SPHERE			"Sphere"
#define AGPMCHAR_INI_NAME_CYLINDER			"Cylinder"
#define AGPMCHAR_INI_NAME_NONE				"None"

#define AGPMCHAR_INI_NAME_TID				"TID"
#define AGPMCHAR_INI_NAME_SCALE				"Scale"
#define AGPMCHAR_INI_NAME_POSITION			"Position"
#define AGPMCHAR_INI_NAME_AXIS				"Axis"
#define AGPMCHAR_INI_NAME_DEGREE			"Degree"
#define	AGPMCHAR_INI_NAME_DIRECTION			"Direction"
#define AGPMCHAR_INI_NAME_MINIMAP			"Minimap"
#define AGPMCHAR_INI_NAME_NAMEBOARD			"NameBoard"

// factor character type ini
#define	AGPMCHAR_INI_NAME_UNDEAD			"Undead"

#define	AGPMCHAR_INI_NAME_FACE_NUM			"FaceNum"
#define	AGPMCHAR_INI_NAME_HAIR_NUM			"HairNum"

//@{ 2006/09/08 burumal
#define AGPMCHAR_INI_NAME_SELF_DESTRUCTION_ATTACK_TYPE	"SELF_DEST"
//@}

//@{ 2006/08/17 burumal
#define AGPMCHAR_INI_NAME_SWCO_BOX			"SIEGEWAR_COLL_BOX"
#define AGPMCHAR_INI_NAME_SWCO_SPHERE		"SIEGEWAR_COLL_SPHERE"
#define AGPMCHAR_INI_NAME_SWCO_OFFSET		"SIEGEWAR_COLL_OFFSET"
//@}

#define	AGPMCHAR_EXCEL_COLUMN_STR			"STR"
#define	AGPMCHAR_EXCEL_COLUMN_DEX			"Dex"
#define	AGPMCHAR_EXCEL_COLUMN_CON			"CON"
#define	AGPMCHAR_EXCEL_COLUMN_WIS			"WIS"
#define	AGPMCHAR_EXCEL_COLUMN_INT			"INT"
#define	AGPMCHAR_EXCEL_COLUMN_CHA			"CHA"
#define	AGPMCHAR_EXCEL_COLUMN_MAX_HP		"Max HP"
#define	AGPMCHAR_EXCEL_COLUMN_MAX_MP		"Max MP"
#define	AGPMCHAR_EXCEL_COLUMN_MAX_SP		"Max SP"
#define AGPMCHAR_EXCEL_COLUMN_MAX_AR		"Max AR"
#define AGPMCHAR_EXCEL_COLUMN_MAX_DR		"Max DR"
#define	AGPMCHAR_EXCEL_COLUMN_MAX_MAP		"Max ATK"
#define	AGPMCHAR_EXCEL_COLUMN_DEF_PHY		"AC"
#define	AGPMCHAR_EXCEL_COLUMN_DEF_FIRE		"FIRE"
#define	AGPMCHAR_EXCEL_COLUMN_DEF_WATER		"WATER"
#define	AGPMCHAR_EXCEL_COLUMN_DEF_EARTH		"EARTH"
#define	AGPMCHAR_EXCEL_COLUMN_DEF_AIR		"AIR"
#define	AGPMCHAR_EXCEL_COLUMN_DEF_MAGIC		"MAGIC"
#define	AGPMCHAR_EXCEL_COLUMN_MIN_DMG_PHY	"MIN DMG"
#define	AGPMCHAR_EXCEL_COLUMN_MAX_DMG_PHY	"MAX DMG"

const INT32	AGPMCHARACTER_MURDERER_PRESERVE_TIME	= 7200;		// 2 hours
const INT32	AGPMCHARACTER_CRIMINAL_PRESERVE_TIME	= 20 * 60;	// 다시 20분 (2007.02.02. steeple)
const INT32 AGPMCHARACTER_MAX_CHARISMA_POINT		= 1000000;

const INT32 AGPMCHARACTER_MINIMUM_GET_HEROIC_POINT_LEVEL = 90;

const INT32 g_alPCTID[]	= 
{
	96, 1, 6, 4, 8, 3, 9, 460, 377, 0, 0, 0, 1722, 1723, 1724, 1732
};
const INT32 AGPMCHARACTER_PC_TID_COUNT = sizeof(g_alPCTID) / sizeof(INT32);

struct AuBoxCollision
{
	AuPOS	v[ 4 ];
	AuPOS	p[ 4 ];
	
	AuBoxCollision()
	{
		v[0].x = -1.0f;
		v[0].y = 0.0f;
		v[0].z = 0.0f;
		v[1].x = 0.0f;
		v[1].y = 0.0f;
		v[1].z = -1.0f;
		v[2].x = 1.0f;
		v[2].y = 0.0f;
		v[2].z = 0.0f;
		v[3].x = 0.0f;
		v[3].y = 0.0f;
		v[3].z = 1.0f;
	}
	
	void	SetSize( float fWidth , float fHeight )
	{
		p[0].x = -fWidth/2;
		p[0].y = 0.0f;
		p[0].z = fHeight/2;
		p[1].x = -fWidth/2;
		p[1].y = 0.0f;
		p[1].z = -fHeight/2;
		p[2].x = fWidth/2;
		p[2].y = 0.0f;
		p[2].z = -fHeight/2;
		p[3].x = fWidth/2;
		p[3].y = 0.0f;
		p[3].z = fHeight/2;
	}

	void	SetOffset( float fOffsetX , float fOffsetZ )
	{
		p[0].x += fOffsetX;
		p[0].z += fOffsetZ;
		p[1].x += fOffsetX;
		p[1].z += fOffsetZ;
		p[2].x += fOffsetX;
		p[2].z += fOffsetZ;
		p[3].x += fOffsetX;
		p[3].z += fOffsetZ;
	}

	void	Transform( AuPOS pos , float fRotate )
	{
		fRotate = -fRotate;
		// 로테이트
		float fCos = cos( fRotate / 180.0f * 3.1415927f);
		float fSin = sin( fRotate / 180.0f * 3.1415927f);

		AuPOS ptTmp;

		#define ROTATE( pt ) {	ptTmp.x = fCos * ( pt ).x - fSin * ( pt ).z; \
								ptTmp.z = fSin * ( pt ).x + fCos * ( pt ).z; \
								pt = ptTmp; }

		ROTATE( v[0] )
		ROTATE( v[1] )
		ROTATE( v[2] )
		ROTATE( v[3] )
		ROTATE( p[0] )
		ROTATE( p[1] )
		ROTATE( p[2] )
		ROTATE( p[3] )

		// 트렌슬레이트
		p[0].x += pos.x;
		p[0].z += pos.z;
		p[1].x += pos.x;
		p[1].z += pos.z;
		p[2].x += pos.x;
		p[2].z += pos.z;
		p[3].x += pos.x;
		p[3].z += pos.z;
	}

	BOOL CollBox( AuPOS *pStart , AuPOS *pVelocity , float fRadius , AuPOS * pvCollPoint );
};

struct _GG_AUTH_DATA;

// 종족을 좀 편하게 인덱싱 하기 위해서 만든 공용체.
// by 마고자
union AuRace
{
	struct	stRace
	{
		INT16	nRace	;
		INT16	nClass	;
	};
	INT32	nIndex;
	stRace	detail;

	AuRace():nIndex ( 0 ){}
	AuRace( INT16 nRace , INT16 nClass )
	{
		detail.nRace	= nRace		;
		detail.nClass	= nClass	;
	}

	bool	operator<( const AuRace &rt ) const
	{
		return this->nIndex < rt.nIndex;
	}
} ;

class AgpmCharacter : public ApModule 
{
	//@{ kday 20051027
	// ;)
	struct numOfCharacByType
	{
		INT32 pc_			;
		INT32 npc_			;
		INT32 monster_		;
		INT32 creature_		;
		INT32 guard_		;
		INT32 gm_			;
		INT32 attackable_	;
		INT32 targetable_	;
		INT32 movablenpc_	;
		ApString<256>	showBuff_;

		numOfCharacByType()
			: pc_		(0)
			, npc_		(0)
			, monster_	(0)
			, creature_	(0)
			, guard_	(0)
			, gm_		(0)
			, attackable_(0)
			, targetable_(0)
			, movablenpc_(0){};


		const LPCTSTR getBuff(void)
		{
			showBuff_.Format(
				_T(
				"pc_ : %d -- \n"
				"npc_ : %d -- \n"
				"monster_ : %d -- \n"
				"creature_ : %d -- \n"
				"guard_	 : %d -- \n"
				"gm_ : %d -- \n"
				"attackable_ : %d -- \n"
				"targetable_ : %d -- \n"
				"movablenpc_ : %d -- \n")
				, pc_		
				, npc_		
				, monster_	
				, creature_	
				, guard_	
				, gm_		
				, attackable_
				, targetable_
				, movablenpc_);
			return showBuff_.GetBuffer();
		}
		void reset(void)
		{
			pc_			= 0;
			npc_		= 0;
			monster_	= 0;
			creature_	= 0;
			guard_		= 0;
			gm_			= 0;
			attackable_	= 0;
			targetable_	= 0;
			movablenpc_	= 0;

		}
		void add(UINT32 type)
		{
			pc_			 += (AGPMCHAR_TYPE_PC			& type) >> 0x0;	//0x00000001
			npc_		 += (AGPMCHAR_TYPE_NPC			& type) >> 0x1;	//0x00000002
			monster_	 += (AGPMCHAR_TYPE_MONSTER		& type) >> 0x2;	//0x00000004
			creature_	 += (AGPMCHAR_TYPE_CREATURE		& type) >> 0x3;	//0x00000008
			guard_		 += (AGPMCHAR_TYPE_GUARD		& type) >> 0x4;	//0x00000010
			gm_			 += (AGPMCHAR_TYPE_GM			& type) >> 0x5;	//0x00000020
			attackable_	 += (AGPMCHAR_TYPE_ATTACKABLE	& type) >> 0x10;//0x00010000
			targetable_	 += (AGPMCHAR_TYPE_TARGETABLE	& type) >> 0x11;//0x00020000
			movablenpc_	 += (AGPMCHAR_TYPE_MOVABLENPC	& type) >> 0x12;//0x00040000
		}
		void remove(UINT32 type)
		{
			pc_			 -= (AGPMCHAR_TYPE_PC			& type) >> 0x0;	//0x00000001
			npc_		 -= (AGPMCHAR_TYPE_NPC			& type) >> 0x1;	//0x00000002
			monster_	 -= (AGPMCHAR_TYPE_MONSTER		& type) >> 0x2;	//0x00000004
			creature_	 -= (AGPMCHAR_TYPE_CREATURE		& type) >> 0x3;	//0x00000008
			guard_		 -= (AGPMCHAR_TYPE_GUARD		& type) >> 0x4;	//0x00000010
			gm_			 -= (AGPMCHAR_TYPE_GM			& type) >> 0x5;	//0x00000020
			attackable_	 -= (AGPMCHAR_TYPE_ATTACKABLE	& type) >> 0x10;//0x00010000
			targetable_	 -= (AGPMCHAR_TYPE_TARGETABLE	& type) >> 0x11;//0x00020000
			movablenpc_	 -= (AGPMCHAR_TYPE_MOVABLENPC	& type) >> 0x12;//0x00040000
		}
	}m_numOfCharacByType;
public:
	typedef struct numOfCharacByType	STNUMOFCHARBYTYPE;
	STNUMOFCHARBYTYPE&	GetNumOfCharByType(void){ return m_numOfCharacByType; };
	//@} kday

private:
	// 사용하는 Parent Module 들
	AgpmFactors*			m_pcsAgpmFactors;

	ApmMap*					m_pcsApmMap;
	AgpmConfig*				m_pcsAgpmConfig;

public:
	BOOL					OnOperationAdd(DispatchArg *pstCheckArg, INT32 lCID, INT32 lTID, CHAR *szName, INT8 cStatus, INT8 cActionStatus, INT8 cCriminalStatus, PVOID pvMovePacket, PVOID pvActionPacket, PVOID pvFactorPacket, INT64 llMoney, INT64 llBankMoney, INT64 llCash, INT8 cIsNewChar, UINT8 ucRegionIndex, UINT64 ulSpecialStatus, INT8 cIsTransform, CHAR *szSkillInit, INT8 cFaceIndex, INT8 cHairIndex, INT8 cBankSize, UINT16 unEventStatusFlag, INT32 lRemainedCriminalTime, INT32 lRemainedMurdererTime, INT32 lWantedCriminal, CHAR *szNickName , BOOL bNPCDisplayForMap , BOOL bNPCDisplayForNameBoard, UINT32 ulLastKilledTimeInBattleSquare );
	BOOL					OnOperationRemove(INT32 lCID, INT32 lTID, CHAR *szName, INT8 cStatus, PVOID pvMovePacket, PVOID pvActionPacket, PVOID pvFactorPacket, UINT32 ulNID);
	BOOL					OnOperationRemoveForView(INT32 lCID);
	BOOL					OnOperationUpdate(DispatchArg *pstCheckArg, INT32 lCID, INT32 lTID, CHAR *szName, INT8 cStatus, INT8 cActionStatus, INT8 cCriminalStatus, PVOID pvMovePacket, PVOID pvActionPacket, PVOID pvFactorPacket, INT64 llMoney, INT64 llBankMoney, INT64 llCash, INT8 cIsNewChar, UINT8 ucRegionIndex, UINT64 ulSpecialStatus, INT8 cIsTransform, INT8 cFaceIndex, INT8 cHairIndex, INT8 cBankSize, UINT16 unEventStatusFlag, INT32 lRemainedCriminalTime, INT32 lRemainedMurdererTime, INT32 lWantedCriminal, CHAR *szNickName , INT32 lSpecialPart = 0);
	BOOL					OnOperationLevelUp(INT32 lCID, PVOID pvFactorPacket);
	BOOL					OnOperationAddAttacker(INT32 lCID, INT32 lAttackerID);
	BOOL					OnOperationMoveBankMoney(DispatchArg *pstCheckArg, INT32 lCID, INT64 llBankMoney);
	BOOL					OnOperationTransform(INT32 lCID, INT32 lTID);
	BOOL					OnOperationTransform(AgpdCharacter *pcsCharacter, INT32 lTID);
	BOOL					OnOperationRestoreTransform(INT32 lCID);
	BOOL					OnOperationCancelTransform(INT32 lCID);

	BOOL					OnOperationEvolution(INT32 lCID, INT32 lTID);
	BOOL					OnOperationRestoreEvolution(INT32 lCID, INT32 lTID);

	BOOL					OnOperationSocialAnimation(INT32 lCID, UINT8 ucSocialActionIndex);
	BOOL					OnOperationOnlineTime(INT32 lCID, UINT32 ulOnlineTime, INT32 lAddictStatus);
	BOOL					OnOperationGameguardAuth(INT32 lCID, PVOID pggAuthData);
	BOOL					OnOperationResurrectionByOther(INT32 lCID, CHAR* szName, INT32 lFlag);
	BOOL					OnOperationEventEffectID(INT32 lCID, INT32 lEventEffectID);

	BOOL					OnOperationRequestPinchWantedCharacter(PVOID pvPacket);

private:
	UINT32					m_ulPrevProcessMoveFollow;
	UINT32					m_ulProcessMoveFollowInterval;
	INT32					m_lStaticCharacterIndex;
	BOOL					m_bIsAcceptPvP;
	INT32					m_lMaxTID;

	INT32					m_lEventEffectID;

private:
	void	InitFuncPtr();	// 함수 포인터 초기화

	// 언어별 체크 사항

	BOOL	CheckName(CHAR* pszCharName, int charNameLen) { return (this->*CheckNamePtr)(pszCharName, charNameLen); }
	BOOL	(AgpmCharacter::*CheckNamePtr)(CHAR*, int);
	BOOL	CheckNameKr(CHAR* pszCharName, int charNameLen);
	BOOL	CheckNameCn(CHAR* pszCharName, int charNameLen);
	BOOL	CheckNameJp(CHAR* pszCharName, int charNameLen);
	BOOL	CheckNameEn(CHAR* pszCharName, int lCharNameLen);

public:
	map<INT16, vector<AgpdCharacter*> >	m_mapTownToNPCList;	// 마을별 NPC 리스트 관리용, RegionIndex -> AgpdCharacter*, 2005.06.24 by kelovon

public:
	AgpaCharacter			m_csACharacter;
	AgpaCharacterTemplate	m_csACharacterTemplate;

	ApAdmin					m_csAdminCharacterRemove;

	AuPacket				m_csPacket;
	AuPacket				m_csPacketMove;
	AuPacket				m_csPacketAction;


	UINT32					m_ulIntervalClock;
	UINT32					m_ulPrevClockCount;

	UINT32					m_ulPrevRemoveClockCount;

	UINT32					m_ulRegionRefreshInterval;

	CHAR*					m_aszFilterText;
	INT32					m_lNumFilterText;
	
	AgpdCharisma			*m_pAgpdCharisma;
	INT32					m_lTotalCharisma;

	AgpmCharacter();
	virtual ~AgpmCharacter();

	// Virtual Function 들
	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnIdle2(UINT32 ulClockCount);
	BOOL	OnDestroy();

	BOOL	CharacterIdle(AgpdCharacter *pcsCharacter, UINT32 ulClockCount);

	BOOL	ProcessIdleMove(AgpdCharacter *pcsCharacter, UINT32 ulClockCount);

	// Character 생성/파괴 하기
	AgpdCharacter			*CreateCharacter();
	BOOL					DestroyCharacter(AgpdCharacter *pstCharacter);

	// Character Template 생성/파괴 하기
	AgpdCharacterTemplate	*CreateTemplate();
	BOOL					DestroyTemplate(AgpdCharacterTemplate *pstTemplate);

	// character type functions
	UINT32	GetCharacterType(AgpdCharacter *pcsCharacter);

	INT32	GetMaxTID() { return m_lMaxTID; }

	//Set Monster Template
	//BOOL	SetMonster( AgpdCharacter *pcsAgpdCharacter, BOOL bMonster );

	BOOL	SetTypeMonster(AgpdCharacterTemplate *pcsTemplate);
	BOOL	SetTypePC(AgpdCharacterTemplate *pcsTemplate);
	BOOL	SetTypeNPC(AgpdCharacterTemplate *pcsTemplate);
	BOOL	SetTypeGuard(AgpdCharacterTemplate *pcsTemplate);

	BOOL	SetTypeAttackable(AgpdCharacterTemplate *pcsTemplate);
	BOOL	SetTypeTargetable(AgpdCharacterTemplate *pcsTemplate);
	BOOL	SetTypeTrap(AgpdCharacterTemplate *pcsTemplate);

	//. 2006. 6. 21. Nonstopdj
	BOOL	SetTypePolyMorph(AgpdCharacterTemplate *pcsTemplate);
	BOOL	IsPolyMorph(AgpdCharacter *pcsAgpdCharacter);

	BOOL	IsMonster( AgpdCharacter *pcsAgpdCharacter );
	BOOL	IsPC( AgpdCharacter *pcsAgpdCharacter );
	BOOL	IsNPC( AgpdCharacter *pcsAgpdCharacter );
	BOOL	IsCreature(AgpdCharacter *pcsAgpdCharacter);
	BOOL	IsCreature(AgpdCharacterTemplate* pcsCharacterTemplate);
	BOOL	IsGM(AgpdCharacter *pcsAgpdCharacter); //GM인지 확인한다.
	BOOL	IsGuard(AgpdCharacter *pcsAgpdCharacter);
	BOOL	IsPet(AgpdCharacter *pcsAgpdCharacter);
	
	BOOL	IsAttackable(AgpdCharacter *pcsAgpdCharacter);
	BOOL	IsTargetable(AgpdCharacter *pcsAgpdCharacter);
	BOOL	IsMovable	(AgpdCharacter *pcsAgpdCharacter);	// 이동이 가능한 NPC인지 체크.
	BOOL	IsTrap		(AgpdCharacter *pcsAgpdCharacter);	// 트랩인지 확인.

	BOOL	SetStatusTame(AgpdCharacter *pcsCharacter, BOOL bSetStatus);
	BOOL	SetStatusSummoner(AgpdCharacter *pcsCharacter, BOOL bSetStatus);
	BOOL	SetStatusFixed(AgpdCharacter* pcsCharacter, BOOL bSetStatus);

	BOOL	IsStatusTame(AgpdCharacter *pcsCharacter);
	BOOL	IsStatusSummoner(AgpdCharacter *pcsCharacter);
	BOOL	IsStatusFixed(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusWasTamed(AgpdCharacter* pcsCharacter);

	BOOL	IsStatusTransparent(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusInvincible(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusAttrInvincible(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusHalfTransparent(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusFullTransparent(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusNormalATKInvincible(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusSkillATKInvincible(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusDisableSkill(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusDisableNormalATK(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusSleep(AgpdCharacter* pcsCharacter);
	BOOL	IsStatusGo(AgpdCharacter* pcsCharacter);

	BOOL	IsInDungeon( AgpdCharacter* pcsCharacter );
	BOOL	IsInSiege( AgpdCharacter* pcsCharacter );

	BOOL	IsDisableChattingCharacter(AgpdCharacter *pcsCharacter);

	// 마고자 (2005-08-16 오전 11:56:29) : 
	BOOL	IsRideOn	(AgpdCharacter *pcsAgpdCharacter);	// 탈것에 타고 있는지 확인.
	INT32	GetVehicleTID(AgpdCharacter *pcsAgpdCharacter);	// 탈것의  Template ID 를 리턴.

	// Stream Function 들
	BOOL	StreamWriteTemplate(CHAR *szFile, BOOL bEncryption);
	BOOL	StreamReadTemplate(CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE);
	BOOL	StreamWrite(CHAR *szFile);
	BOOL	StreamRead(CHAR *szFile);
	BOOL	StreamWriteCharKind(CHAR *szFile);
	BOOL	StreamReadCharKind(CHAR *szFile);

	BOOL	StreamReadStaticCharacter	(CHAR *szFile , BOOL bCrashSafe = FALSE );
	BOOL	StreamReadOneStaticCharacter	(CHAR *szFile , CHAR *szCharName, BOOL bCrashSafe = FALSE );
		// bCrashSafe 는 템플릿이 없을경우 임시 템플릿으로 대체하는 코드 작동 여부다.
	BOOL	StreamWriteStaticCharacter	(CHAR *szFile);

	BOOL	StreamReadLevelUpExpTxt(CHAR *szFile);
	BOOL	StreamReadDownExpPerLevelTxt(CHAR *szFile); 	//JK_레벨별 경험치패널티
	BOOL	StreamReadCharGrowUpTxt(CHAR *szFile, BOOL bDecryption = FALSE);

	BOOL	StreamReadImportData(CHAR *szFile, CHAR *szDebugString, BOOL bDecryption);

	BOOL	SetStreamFactor(AuExcelLib *pcsExcelTxtLib, AgpdFactor *pcsAgpdFactor, INT16 nRow, INT16 nColumn, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);
	
	BOOL	StreamReadCharismaTitle(CHAR *pszFile, BOOL bDecryption);

	static BOOL		TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL		TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL		CharacterWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL		CharacterReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL		StaticCharacterWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL		StaticCharacterReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL		StaticOneCharacterWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL		StaticOneCharacterReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	// 환경 변수 설정 함수
	BOOL	SetMaxCharacter(INT32 nCount);
	BOOL	SetMaxCharacterTemplate(INT32 nCount);
	BOOL	SetMaxCharacterRemove(INT32 nCount);

	INT32	GetMaxCharacterTemplate( void );

	BOOL	SetIdleIntervalMSec(UINT32 lIntervalClock);

	BOOL	SetProcessMoveFollowInterval(UINT32 ulIntervalClock);

	// character remove pool 관련 함수들
	BOOL	ProcessRemove(UINT32 ulClockCount);

	BOOL	AddRemoveCharacter(AgpdCharacter *pcsCharacter);

	// Character Manipulation 함수들
	AgpdCharacter*	AddCharacter(INT32 lCID, INT32 lTID, CHAR* szGameID);
	AgpdCharacter*	AddCharacter(AgpdCharacter *pcsCharacter);
	AgpdCharacter*	GetCharacter(INT32 lCID);
	AgpdCharacter*	GetCharacter(CHAR*	szGameID);
	AgpdCharacter*	GetCharacterLock(INT32 lCID);
	AgpdCharacter*	GetCharacterLock(CHAR*	szGameID);
	BOOL			GetCharacterLock(INT32 *palCID, INT32 lNumCharacter, AgpdCharacter **ppacsCharacter);
	BOOL			GetCharacterLockByPointer(INT_PTR *paCharPointer, INT32 lNumCharacter);

	BOOL			RemoveCharacter(INT32 lCID, BOOL bDelete = FALSE, BOOL bDestroyModuleData = FALSE);
	BOOL			RemoveCharacter(CHAR*	szGameID, BOOL bDelete = FALSE, BOOL bDestroyModuleData = FALSE);
private:
	BOOL			RemoveCharacter(AgpdCharacter *pcsCharacter, BOOL bDelete, BOOL bDestroyModuleData);
public:
	BOOL			RemoveAllCharacters();
	AgpdCharacter*	RemoveAllCharactersExceptOne(INT32 lCID);
	//BOOL			RemoveCharacter(AgpdCharacter*	pcsCharacter);

	BOOL			ResetMonsterData(AgpdCharacter *pcsCharacter, INT32 lNewTID);

	// 캐릭터 데이타는 두고 맵에 추가, 삭제하는 오퍼레이션만 한다.
	BOOL			RemoveCharacterFromMap(AgpdCharacter* pcsCharacter, BOOL bIsEnumCallback = TRUE);
	BOOL			AddCharacterToMap(AgpdCharacter *pcsCharacter, BOOL bIsEnumCallback = TRUE);

	// 돈 관련
	BOOL			CheckMoneySpace(INT32 lCID, INT64 llMoney );
	BOOL			CheckMoneySpace(AgpdCharacter  *pcsAgpdCharacter, INT64 llMoney );
	BOOL			SetMoney( INT32 lCID, INT64 llMoney );
	BOOL			SetMoney( AgpdCharacter  *pcsAgpdCharacter, INT64 llMoney );
	BOOL			GetMoney( INT32 lCID, INT64 *pllMoney );
	BOOL			GetMoney( AgpdCharacter  *pcsAgpdCharacter, INT64 *pllMoney );
	BOOL			AddMoney( INT32 lCID, INT64 lMoney );
	BOOL			AddMoney( AgpdCharacter  *pcsAgpdCharacter, INT64 lMoney );
	BOOL			SubMoney( INT32 lCID, INT64 lMoney );
	BOOL			SubMoney( AgpdCharacter  *pcsAgpdCharacter, INT64 lMoney );

	BOOL			SetBankMoney(AgpdCharacter *pcsCharacter, INT64 llBankMoney);
	INT64			GetBankMoney(AgpdCharacter *pcsCharacter);
	INT64			AddBankMoney(AgpdCharacter *pcsCharacter, INT64 llBankMoney);
	INT64			SubBankMoney(AgpdCharacter *pcsCharacter, INT64 llBankMoney);

	BOOL			SetCash(AgpdCharacter *pcsCharacter, INT64 llCash);
	INT64			GetCash(AgpdCharacter *pcsCharacter);
	INT64			AddCash(AgpdCharacter *pcsCharacter, INT64 llCash);
	INT64			SubCash(AgpdCharacter *pcsCharacter, INT64 llCash);

	// Character Template Manipulation 함수들
	AgpdCharacterTemplate*	AddCharacterTemplate(INT32 lTID);
	AgpdCharacterTemplate*	GetCharacterTemplate(INT32 ulTID);
	AgpdCharacterTemplate*	GetCharacterTemplate(CHAR *szTName);
	AgpdCharacterTemplate*	GetCharacterTemplateUseCID(INT32 lCID);
	BOOL					RemoveAllCharacterTemplate(); // Add(151003, BOB)
	BOOL					RemoveCharacterTemplate(INT32 lTID); // Add 090802 Bob Jung.
	BOOL					RemoveCharacterTemplate(AgpdCharacterTemplate* pcsTemplate); // Add 090802 Bob Jung.

	BOOL					GetRaceCharacterTemplate(INT32 lRace, INT32 *alTID, INT32 lBufferSize);
	INT32					GetRaceFromTemplate(AgpdCharacterTemplate* pcsTemplate);

	// Sequence 가져오기 (ApAdmin Wrapping)
	AgpdCharacter*			GetCharSequence(INT32 *lIndex);
	AgpdCharacterTemplate*	GetTemplateSequence(INT32 *lIndex);

	// Validation Check 함수들
	BOOL	OnValid(CHAR* szData, INT16 nSize);

	BOOL	IsCharacter(INT32 lCID);
	BOOL	IsCharacter(CHAR* szGameID);

	// Factor 초기화 함수
	BOOL	InitFactor(AgpdCharacter *pCharacter);

	// 통신 관련 함수들
	BOOL	ReceiveAddCharacter();
	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL	ProcessMovePacket(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsMoveTargetChar, INT32 lMoveFollowDistance,
							  AuPOS	*pstSrcPos, AuPOS *pstDstPos, FLOAT fDegreeX, FLOAT fDegreeY,
							  INT8 cMoveFlag, INT8 cMoveDirection, BOOL bReceivedFromServer);
	BOOL	ProcessActionPacket(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetChar, PVOID pvPacketTargetDamageFactor,
							  INT8 cActionResultType, INT8 cForceAttack, INT8 cComboInfo, INT32 lSkillTID,
							  UINT32 ulAdditionalEffect, UINT8 cHitIndex, BOOL bReceivedFromServer, INT8 cAction = AGPDCHAR_ACTION_TYPE_ATTACK );

	// 동작 함수들
	BOOL	TransformCharacter(INT32 lCID, eAgpmCharacterTransformType eType, AgpdCharacterTemplate *pcsTemplate = NULL, AgpdFactor *pcsTargetFactor = NULL, BOOL bIsCopyFactor = FALSE);
	BOOL	TransformCharacter(AgpdCharacter *pcsCharacter, eAgpmCharacterTransformType eType, AgpdCharacterTemplate *pcsTemplate = NULL, AgpdFactor *pcsTargetFactor = NULL, BOOL bIsCopyFactor = FALSE);

	BOOL	RestoreTransformCharacter(INT32 lCID);
	BOOL	RestoreTransformCharacter(AgpdCharacter *pcsCharacter);

	BOOL	EvolutionCharacter(AgpdCharacter *pcsCharacter, AgpdCharacterTemplate *pcsTemplate);
	BOOL	RestoreEvolutionCharacter(AgpdCharacter *pcsCharacter, AgpdCharacterTemplate *pcsTemplate);

	BOOL	TransformCharacterAppear(AgpdCharacter *pcsCharacter, AgpdCharacterTemplate *pcsTemplate);
	BOOL	TransformCharacterStatus(AgpdCharacter *pcsCharacter, AgpdCharacterTemplate *pcsTemplate, AgpdFactor *pcsTargetFactor, BOOL bIsCopyFactor = FALSE);

	BOOL	SyncMoveStartPosition(AgpdCharacter *pcsCharacter, AuPOS *posSrc);
	BOOL	AdjustMoveSpeed(AgpdCharacter *pcsCharacter, AuPOS *pcsSrc);

	BOOL	CheckEnableActionCharacter(AgpdCharacter *pcsCharacter, eDisturbAction eDisturb);
	BOOL	MoveCharacter(INT32 lCID, AuPOS *pstPos, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding = FALSE, BOOL bMoveFast = FALSE, BOOL bHorizontal = FALSE, BOOL bExistNextAction = FALSE, BOOL bStopMove = FALSE);
	BOOL	MoveCharacter(AgpdCharacter *pcsCharacter, AuPOS *pstPos, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding = FALSE, BOOL bMoveFast = FALSE, BOOL bHorizontal = FALSE, BOOL bExistNextAction = FALSE, BOOL bStopMove = FALSE);

	BOOL	MoveCharacterFollow(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetCharacter, INT32 lFollowDistance, BOOL bMoveFirst = FALSE, FLOAT *pfDeltaMove = NULL);
	BOOL	CheckStopFollow(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetCharacter);

	BOOL	NegligiblePosition(AuPOS *posSrc1, AuPOS *posSrc2);

	BOOL	MoveCharacterHorizontal(INT32 lCID, AuPOS *pstBasePos, INT32 lDistance, BOOL bExistNextAction = FALSE);
	BOOL	MoveCharacterHorizontal(AgpdCharacter *pcsCharacter, AuPOS *pstBasePos, INT32 lDistance, BOOL bExistNextAction = FALSE);

	BOOL	StopCharacter(INT32 lCID, AuPOS *pstPos);
	BOOL	StopCharacter(AgpdCharacter *pcsCharacter, AuPOS *pstPos);

	BOOL	TurnCharacter(INT32 lCID, FLOAT fDegreeX, FLOAT fDegreeY);
	BOOL	TurnCharacter(AgpdCharacter *pcsCharacter, FLOAT fDegreeX, FLOAT fDegreeY);

	BOOL	MoveTurnCharacter(INT32 lCID, AuPOS *pstPos, FLOAT fDegreeX, FLOAT fDegreeY, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding = FALSE, BOOL bMoveFast = FALSE, BOOL bExistNextAction = FALSE);
	BOOL	MoveTurnCharacter(AgpdCharacter *pcsCharacter, AuPOS *pstPos, FLOAT fDegreeX, FLOAT fDegreeY, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding = FALSE, BOOL bMoveFast = FALSE, BOOL bExistNextAction = FALSE);
	
	float	GetSelfCharacterTurnAngle( AgpdCharacter *pcsCharacter, AuPOS* pDestination );

	BOOL	MoveCharacterToPositionWithDistance(AgpdCharacter *pcsCharacter, AuPOS stTargetPos, INT32 lDistance, BOOL bMoveFirst = FALSE, FLOAT *pfDeltaMove = NULL);

	// Action 함수들
	BOOL	SyncActionPosition(AgpdCharacterActionType eAction, AgpdCharacter *pcsCharacter, ApBase *pcsTargetBase, AuPOS *pstActionPosition);

	BOOL	ActionCharacter(AgpdCharacterActionType eAction, INT32 lCID, ApBase *pcsTargetBase, AuPOS *pstTargetPos, BOOL bForce = FALSE, INT8 cActionResultType = (-1), AgpdFactor *pcsFactorDamage = NULL, UINT8 cComboInfo = 0, UINT32 ulAdditionalEffect = 0, UINT8 cHitIndex = 0, INT32 lSkillTID = 0);
	BOOL	ActionCharacter(AgpdCharacterActionType eAction, AgpdCharacter *pcsCharacter, ApBase *pcsTargetBase, AuPOS *pstTargetPos, BOOL bForce = FALSE, INT8 cActionResultType = (-1), AgpdFactor *pcsFactorDamage = NULL, UINT8 cComboInfo = 0, UINT32 ulAdditionalEffect = 0, UINT8 cHitIndex = 0, INT32 lSkillTID = 0);

	BOOL	IsSufficientCost(AgpdCharacter *pcsCharacter, AgpdCharacterActionType eAction);
	INT32	GetMeleeActionCostMP(AgpdCharacter *pcsCharacter);

	// Update 함수들
	BOOL	UpdateInit(AgpdCharacter *pcsCharacter);

	BOOL	UpdateMoney(INT32 lCID);
	BOOL	UpdateMoney(AgpdCharacter *pCharacter);

	BOOL	UpdateFactor(INT32 lCID);
	BOOL	UpdateFactor(AgpdCharacter *pCharacter);

	BOOL	UpdateStatus(INT32 lCID, UINT16 nNewStatus);
	BOOL	UpdateStatus(AgpdCharacter *pCharacter, UINT16 nNewStatus);

	BOOL	UpdateActionStatus(INT32 lCID, UINT16 nNewStatus, BOOL bCheck = TRUE);
	BOOL	UpdateActionStatus(AgpdCharacter *pCharacter, UINT16 nNewStatus, BOOL bCheck = TRUE);

	BOOL	UpdateSpecialStatus(INT32 lCID, UINT64 ulStatus, INT32 lReserved = 0);
	BOOL	UpdateSpecialStatus(AgpdCharacter* pCharacter, UINT64 ulStatus, INT32 lReserved = 0);
	BOOL	UpdateSetSpecialStatus(INT32 lCID, UINT64 ulSetStatus, INT32 lReserved = 0);
	BOOL	UpdateSetSpecialStatus(AgpdCharacter* pCharacter, UINT64 ulSetStatus, INT32 lReserved = 0);
	BOOL	UpdateUnsetSpecialStatus(INT32 lCID, UINT64 ulUnsetStatus, INT32 lReserved = 0);
	BOOL	UpdateUnsetSpecialStatus(AgpdCharacter* pCharacter, UINT64 ulUnsetStatus, INT32 lReserved = 0);

	BOOL	SetCharacterLevelLimit		( AgpdCharacter * pcsCharacter , INT32 nLevel );
	BOOL	ReleaseCharacterLevelLimit	( AgpdCharacter * pcsCharacter );
	
	enum	LevelLimitState
	{
		LLS_NONE,		// 블럭 없음.. 노멀 상황
		LLS_LIMITED,	// 레벨 제한걸림.. 걸린 레벨보다 낮음. 경험치&레벨업 정상획득
		LLS_BLOCKED,	// 레벨 제한 걸림.. 경험치는 얻으나 레벨은 못올라감.
		LLS_DOWNED		// 레벨 제한걸림 .. 레벨이 다운되어 있음.
	};

	LevelLimitState	GetCharacterLevelLimit	( AgpdCharacter * pcsCharacter );

	BOOL	UpdateCriminalStatus(INT32 lCID, AgpdCharacterCriminalStatus eNewStatus);
	BOOL	UpdateCriminalStatus(AgpdCharacter *pcsCharacter, AgpdCharacterCriminalStatus eNewStatus);
	
	BOOL	UpdatePosition(INT32 lCID, AuPOS *pstPos, BOOL bCheckBlock = TRUE, BOOL bSync = FALSE);
	BOOL	UpdatePosition(AgpdCharacter *pCharacter, AuPOS *pstPos, BOOL bCheckBlock = TRUE, BOOL bSync = FALSE);

	BOOL	UpdateRegion(AgpdCharacter *pcsCharacter);
	INT32	GetRealRegionIndex(AgpdCharacter *pcsCharacter)
	{
		if (!pcsCharacter)
			return (-1);

		if (pcsCharacter->m_nBindingRegionIndex > 0) return pcsCharacter->m_nBindingRegionIndex;

		return m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);
	}

	BOOL	UpdateLevel(INT32 lCID, INT32 lLevelChange);
	BOOL	UpdateLevel(AgpdCharacter *pCharacter, INT32 lLevelChange);
	BOOL	ChangedLevel(AgpdCharacter *pCharacter);

	BOOL	UpdateSkillPoint(INT32 lCID, INT32 lPoint);
	BOOL	UpdateSkillPoint(AgpdCharacter *pCharacter, INT32 lPoint);

	BOOL	UpdateHeroicPoint(INT32 lCID, INT32 lPoint);
	BOOL	UpdateHeroicPoint(AgpdCharacter *pcsCharacter, INT32 lPoint);
	BOOL	UpdateHeroicPoint(INT32 lCID, INT32 lNewPoint, BOOL bFullUpdate);
	BOOL	UpdateHeroicPoint(AgpdCharacter *pcsCharacter, INT32 lNewPoint, BOOL bFullUpdate);

	BOOL	UpdateCustomizeIndex(AgpdCharacter *pcsCharacter, INT32 lNewFaceIndex, INT32 lNewHairIndex);

	BOOL	UpdateBankSize(AgpdCharacter *pcsCharacter, INT8 cBankSize);

	BOOL	UpdateEventStatusFlag(AgpdCharacter *pcsCharacter, UINT16 unEventStatusFlag);

	// Callback 등록 함수들
	BOOL	SetCallbackInitChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// Add 되고 Init 후 Callback
	BOOL	SetCallbackNewCreatedCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAddCharPost(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Add 처리 모두 한 후 
	BOOL	SetCallbackRemoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Remove 되기 전 Callback
	BOOL	SetCallbackRemoveID(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Remove 되기 전 Callback
	BOOL	SetCallbackResetMonster(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Monster 데이타만 리셋
	BOOL	SetCallbackDeleteChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackMoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);				// Target Position이 정해진 후 Callback
	BOOL	SetCallbackStopChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);				// Character가 이동을 정지한 후 Callback
	BOOL	SetCallbackUpdateMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// 돈이 변한뒤 CallBack
	BOOL	SetCallbackUpdateFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// Factor가 변한 후 Callback
	BOOL	SetCallbackUpdateStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// 상태 변경된 후 Callback
	BOOL	SetCallbackUpdateActionStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// action 상태 변경된 후 Callback
	BOOL	SetCallbackUpdateActionStatusCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// action 상태 변경된 후 Callback
	BOOL	SetCallbackUpdateSpecialStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Special Status 변경된 후 Callback
	BOOL	SetCallbackUpdateCriminalStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// criminal status 변경된 후 Callback
	BOOL	SetCallbackUpdateChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// 매 Clock마다 모든 Character에 대해서 호출
	BOOL	SetCallbackUpdatePositionCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Character의 Position을 Update하기 전에 호출
	BOOL	SetCallbackUpdatePosition(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Character의 Position을 Update한 후에 호출
	BOOL	SetCallbackPreUpdatePosition(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Character의 Position을 Update한 후 리전 업데이트 하기 전에 호출
	BOOL	SetCallbackCheckIsActionMove(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// action move 를 해야하는지 검사한다
	BOOL	SetCallbackActionAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// melee attack action packet을 받은후 패킷을 파싱하고 호출
	BOOL	SetCallbackActionAttackMiss(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// skill action packet을 받은후 패킷을 파싱하고 호출
	BOOL	SetCallbackActionPickupItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventTeleport(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventNPCTrade(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventMasterySpecialize(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventBank(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventItemConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventGuildWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventProduct(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventNPCDialog(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventSkillMaster(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventRefinery(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionProductSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventCharCustomize(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventItemRepair(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventRemission(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventWantedCriminal(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventTax(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventGamble(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionEventGacha(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionUseSiegeWarAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackActionCarveASeal(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//BOOL	SetCallbackReCalcFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// factor result 를 다쉬 계산한다.
	//BOOL	SetCallbackUpdateHP(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// factor에서 HP가 바뀌었을경우
	//BOOL	SetCallbackUpdateMaxHP(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// factor에서 Max HP가 바뀌었을경우
	BOOL	SetCallbackUpdateLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// factor에서 level이 바뀌었을 경우
	BOOL	SetCallbackCheckRemoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// 이 캐릭터 데이타를 정말 없애야 하는지 체크한다.
	//@{ Jaewon 20050902
	// For fade-in/out
	BOOL	SetCallbackCheckRemoveChar2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//@} Jaewon
	BOOL	SetCallbackUISetMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// UI에서 사용 - 서버로부터 Money Update
	BOOL	SetCallbackUpdateBankMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// UI에서 사용 - 서버로부터 Money Update
	BOOL	SetCallbackMoveBankMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// UI에서 사용 - 서버로부터 Money Update
	BOOL	SetCallbackUpdateCash(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackPayTax(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackGetTaxRatio(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackTransformAppear(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackTransformStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRestoreTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCancelTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackEvolution(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRestoreEvolution(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	BOOL	SetCallbackUpdateMurdererPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateMukzaPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateCharismaPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackUpdateSkillPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackAddAttackerToList(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackReceiveAction(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCheckNowUpdateActionFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackCheckValidNormalAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCheckActionRequirement(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCheckActionAttackTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackPayActionCost(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReceiveCharacterData(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackDisconnectFromGameServer( ApModuleDefaultCallBack pfCallback, PVOID pClass	);
	BOOL	SetCallbackDisconnectByAnotherUser( ApModuleDefaultCallBack pfCallback, PVOID pClass	);
	BOOL	SetCallbackMonsterAIPathFind( ApModuleDefaultCallBack pfCallback, PVOID pClass	);

	BOOL	SetCallbackStreamReadImportData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackStreamReadImportDataErrorReport(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackRequestResurrection(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackGetNewCID(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackIsPlayerCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackAddCharacterToMap(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveCharacterFromMap(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackSetCombatMode(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackResetCombatMode(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackCheckProcessIdle(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// 마고자 (2004-09-17 오후 5:19:23) : 리전 체인지 콜벡 지정..
	BOOL	SetCallbackBindingRegionChange(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackSocialAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackReleaseActionMoveLock(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateCustomize(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackUpdateOptionFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//	2005.05.31. By SungHoon(초대 거부 콜백 지정)

	BOOL	SetCallbackUpdateBankSize(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackGetItemLancer(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackRemoveRide(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackAlreadyExistChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackUpdateEventStatusFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackInitTemplateDefaultValue(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackReadAllCharacterTemplate(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackCheckAllBlockStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackAddStaticCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveProtectedNPC(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackUseEffect(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackIsStaticCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackIsArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackGetBonusDropRate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackGetBonusDropRate2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackGetBonusMoneyRate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackGetBonusCharismaRate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	BOOL	SetCallbackUpdateNickName(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackOnlineTime(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackGameguardAuth(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackResurrectionByOther(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackEventEffectID(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackPinchWantedAnswer( ApModuleDefaultCallBack pfCallback , PVOID pClass );
	BOOL	SetCallbackPinchWantedRequest( ApModuleDefaultCallBack pfCallback , PVOID pClass );

	BOOL	SetCallbackCheckPCRoomType( ApModuleDefaultCallBack pfCallback, PVOID pClass );

	PVOID	MakePacketOptionFlag(AgpdCharacter *pcsCharacter, INT32 lOptionFlag, INT16 *pnPacketLength);	//	2005.05.31. By SungHoon(초대 거부 플래그 패킷 생성)

	// SetAttachedModuleData() wrap functions
	INT16	AttachCharacterData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT16	AttachCharacterTemplateData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	//BOOL	ReCalcCharacterFactors(INT32 lCID);
	//BOOL	ReCalcCharacterFactors(AgpdCharacter *pcsCharacter);

	BOOL	ReflectFactorPacket(AgpdCharacter *pcsCharacter, PVOID pvPacketFactor, AgpdFactor *pcsFactorDamageBuffer = NULL);

	BOOL	CheckActionStatus(INT32 lCID, INT16 nActionType);
	BOOL	CheckActionStatus(AgpdCharacter *pcsCharacter, INT16 nActionType);

	BOOL	CheckNormalAttack(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, BOOL bForceAttack);
	BOOL	CheckVaildNormalAttackTarget(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, BOOL bForceAttack);
	BOOL	CheckActionRequirement(AgpdCharacter *pcsCharacter, AgpdCharacterActionResultType *peActionResult, AgpdCharacterActionType eType);

	BOOL	PayActionCost(AgpdCharacter *pcsCharacter, INT16 nActionType);

	INT32	GetUnion(AgpdCharacter *pcsCharacter);
	INT32	GetUnion(INT32 lRace);

	INT32	GetMurdererPoint(AgpdCharacter *pcsCharacter);
	INT32	GetMurdererPoint(INT32 lCID);
	
	INT32	GetCharismaPoint(AgpdCharacter *pcsCharacter);
	
	INT32	GetRemainedCriminalTime(AgpdCharacter *pcsCharacter);
	INT32	GetRemainedMurdererTime(AgpdCharacter *pcsCharacter);
	
	UINT32	SetLastCriminalTime(AgpdCharacter *pcsCharacter, INT32 lRemain);
	UINT32	SetLastMurdererTime(AgpdCharacter *pcsCharacter, INT32 lRemain);

	UINT32	GetLastKilledTimeInBattleSquare(AgpdCharacter *pcsCharacter);
	UINT32	SetLastKilledTimeInBattleSquare(AgpdCharacter *pcsCharacter, UINT32 ulKilledTime);

	INT32	GetMurdererLevel(INT32 lCID);
	INT32	GetMurdererLevel(AgpdCharacter *pcsCharacter);
	INT32	GetMurdererLevelOfPoint(INT32 lPoint);

	INT32	GetMukzaPoint(AgpdCharacter *pcsCharacter);
	INT32	GetMukzaPoint(INT32 lCID);

	INT32	GetMoveSpeed(INT32 lCID);
	INT32	GetMoveSpeed(AgpdCharacter *pcsCharacter);

	INT32	GetFastMoveSpeed(INT32 lCID);
	INT32	GetFastMoveSpeed(AgpdCharacter *pcsCharacter);

	BOOL	IsMurderer(AgpdCharacter *pcsCharacter, INT32 *plPoint = NULL);
	BOOL	IsMurderer(INT32 lCID, INT32 *plPoint = NULL);

	BOOL	IsMukza(AgpdCharacter *pcsCharacter);
	BOOL	IsMukza(INT32 lCID);

	BOOL	UpdateMurdererPoint(AgpdCharacter *pcsCharacter, INT32 lMurdererPoint);
	BOOL	AddMurdererPoint(AgpdCharacter *pcsCharacter, INT32 lAdd);
	BOOL	SubMurdererPoint(AgpdCharacter *pcsCharacter, INT32 lSub);
	BOOL	UpdateMukzaPoint(AgpdCharacter *pcsCharacter, INT32 lMukzaPoint);

	BOOL	UpdateCharismaPoint(AgpdCharacter *pcsCharacter, INT32 lCharismaPoint);
	BOOL	AddCharismaPoint(AgpdCharacter *pcsCharacter, INT32 lAdd);
	BOOL	SubCharismaPoint(AgpdCharacter *pcsCharacter, INT32 lSub);

	INT64	GetLevelUpExp(INT32 lLevel);
	INT64	GetLevelUpExp(AgpdCharacter *pcsCharacter);
	INT64	GetDownExpPerLevel(INT32 lLevel);//JK_레벨별 경험치 패널티

	INT32	GetCharGrowUpTemplate(CHAR *szData, AgpdCharacterTemplate **ppcsTemplate, INT32 lCount);
	BOOL	SetCharGrowUpFactor(AgpdFactor *pcsFactor, CHAR *szColumnName, CHAR *szValue);

	INT64	GetExp(AgpdCharacter *pcsCharacter);
	INT32	GetSkillPoint(AgpdCharacter *pcsCharacter);

	INT32	GetHeroicPoint(AgpdCharacter *pcsCharacter);

	INT32	GetLevel		(AgpdCharacter *pcsCharacter);
	INT32	GetLevelBefore	(AgpdCharacter *pcsCharacter);
	INT32	GetLevelOriginal(AgpdCharacter *pcsCharacter);
	INT32	GetLevelLimited	(AgpdCharacter *pcsCharacter);

	INT32	GetLevelFromTemplate(AgpdCharacterTemplate* pcsTemplate);

	INT32	GetHP(AgpdCharacter* pcsCharacter);
	INT32	GetMaxHP(AgpdCharacter* pcsCharacter);

	// attacker list management functions
	BOOL	AddAttackerToList(AgpdCharacter *pcsCharacter, INT32 lAttackerID);
	BOOL	IsInAttackerList(AgpdCharacter *pcsCharacter, INT32 lAttackerID);
	BOOL	ClearAttackerToList(AgpdCharacter *pcsCharacter);

	UINT32	GetAttackIntervalMSec(AgpdCharacter *pcsCharacter);

	BOOL	IsCombatMode(AgpdCharacter *pcsCharacter);
	BOOL	SetCombatMode(AgpdCharacter *pcsCharacter, UINT32 ulClockCount);
	BOOL	ResetCombatMode(AgpdCharacter *pcsCharacter);

	BOOL	SetActionBlockTime(AgpdCharacter *pcsCharacter, UINT32 ulBlockTime, AgpdCharacterActionBlockType eBlockType = AGPDCHAR_ACTION_BLOCK_TYPE_ALL);
	BOOL	IsActionBlockCondition(AgpdCharacter *pcsCharacter, AgpdCharacterActionBlockType eBlockType = AGPDCHAR_ACTION_BLOCK_TYPE_ALL);
	BOOL	ClearActionBlockTime(AgpdCharacter *pcsCharacter);

	BOOL	PvPModeOn();
	BOOL	PvPModeOff();

	BOOL	SetRegionRefreshInterval(UINT32 ulIntervalMSec);
	UINT32	GetRegionRefreshInterval();
	
	PVOID	MakePacketCharView(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, BOOL bIsNewChar = FALSE);
	PVOID	MakePacketCharLevel(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);		//	2005.06.15 By SungHoon
	PVOID	MakePacketCharAction(INT16 *pnPacketLength, AgpdCharacterActionType eAction = AGPDCHAR_ACTION_TYPE_NONE, INT32 lTargetCID = -1, INT32 lSkillTID = -1, AgpdCharacterActionResultType eActionResult = AGPDCHAR_ACTION_RESULT_TYPE_NONE, PVOID pvFactor = NULL, AuPOS *pstPos = NULL, UINT8 ucCombo = -1, BOOL bForce = FALSE, AgpdCharacterAdditionalEffect eEffect = AGPDCHAR_ADDITIONAL_EFFECT_NONE, UINT8 cHitIndex = -1);

	BOOL	CheckLockAction(AgpdCharacter *pcsCharacter);

	BOOL	IsInPvPDanger(AgpdCharacter *pcsCharacter);

	VOID	MakeGameID( CHAR *szSrcGameID, CHAR *szTargetGameID );		//	2005.05.18. By SungHoon
	//만들수있는 CharName인지 확인한다.
	BOOL	CheckCharName(CHAR *pszCharName, BOOL bCheckMinLength = TRUE);							//	2005.05.27. By SungHoon
	
	BOOL	CheckFilterText(CHAR* pszText);								// 2005.07.01. steeple
	BOOL	StreamReadChatFilterText(CHAR* szFile, BOOL bEncryption);	// 2005.07.01. steeple

	BOOL	IsOptionFlag( AgpdCharacter *pcsCharacter, AgpdCharacterOptionFlag eOptionFlag );		//	2005.05.31. By SungHoon
	BOOL	IsOptionFlag( INT32 lOptionFlag , AgpdCharacterOptionFlag eOptionFlag );				//	2005.06.01. By SungHoon
	INT32	SetOptionFlag( AgpdCharacter *pcsCharacter, AgpdCharacterOptionFlag eOptionFlag );		//	2005.05.31. By SungHoon
	INT32	UnsetOptionFlag( AgpdCharacter *pcsCharacter, AgpdCharacterOptionFlag eOptionFlag );	//	2005.05.31. By SungHoon
	INT32	SetOptionFlag( INT32 lOptionFlag, AgpdCharacterOptionFlag eOptionFlag );		//	2005.05.31. By SungHoon
	INT32	UnsetOptionFlag( INT32 lOptionFlag, AgpdCharacterOptionFlag eOptionFlag );	//	2005.05.31. By SungHoon

	BOOL	SetCallbackUpdateLevelPost(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//	2005.06.14 By SungHoon

	BOOL	AddRegionNPC(AgpdCharacter *pcsAgpdCharacter);	// 2005.06.24 By Kelovon

	BOOL	HasPenalty(AgpdCharacter *pcsCharacter, eAgpmCharacterPenalty ePenalty);
	BOOL	SetCallbackBlockByPenalty(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	AddGameBonusExp(AgpdCharacter* pcsCharacter, INT32 lValue);
	BOOL	SubGameBonusExp(AgpdCharacter* pcsCharacter, INT32 lValue);
	INT32	GetGameBonusExp(AgpdCharacter* pcsCharacter);

	BOOL	AddGameBonusMoney(AgpdCharacter* pcsCharacter, INT32 lValue);
	BOOL	SubGameBonusMoney(AgpdCharacter* pcsCharacter, INT32 lValue);
	INT32	GetGameBonusMoney(AgpdCharacter* pcsCharacter);

	BOOL	AddGameBonusDropRate(AgpdCharacter* pcsCharacter, INT32 lValue);
	BOOL	SubGameBonusDropRate(AgpdCharacter* pcsCharacter, INT32 lValue);
	INT32	GetGameBonusDropRate(AgpdCharacter* pcsCharacter);
	
	BOOL	AddGameBonusDropRate2(AgpdCharacter* pcsCharacter, INT32 lValue);
	BOOL	SubGameBonusDropRate2(AgpdCharacter* pcsCharacter, INT32 lValue);
	INT32	GetGameBonusDropRate2(AgpdCharacter* pcsCharacter);

	BOOL	AddGameBonusCharismaRate(AgpdCharacter* pcsCharacter, INT32 lValue);
	BOOL	SubGameBonusCharismaRate(AgpdCharacter* pcsCharacter, INT32 lValue);
	INT32	GetGameBonusCharismaRate(AgpdCharacter* pcsCharacter);

	AuRace	GetCharacterRace( AgpdCharacter * pcsCharacter );
	CHAR*	GetCharacterRaceString( AgpdCharacter * pcsCharacter );

	void	PrintRegionNPCList();

	INT32	GetRemovePoolCount();

	BOOL	SetReturnToLoginServer(AgpdCharacter *pcsCharacter)	{ if (!pcsCharacter) return FALSE; pcsCharacter->m_unCurrentStatus = AGPDCHAR_STATUS_RETURN_TO_LOGIN_SERVER; return TRUE; };
	BOOL	ResetReturnToLoginServer(AgpdCharacter *pcsCharacter) { if (!pcsCharacter) return FALSE; pcsCharacter->m_unCurrentStatus = AGPDCHAR_STATUS_IN_GAME_WORLD; return TRUE; };
	BOOL	IsReturnToLoginServerStatus(AgpdCharacter *pcsCharacter) { if (!pcsCharacter) return FALSE; if (pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_RETURN_TO_LOGIN_SERVER) return TRUE; return FALSE;};

	BOOL	IsAllBlockStatus(AgpdCharacter *pcsCharacter);

	FLOAT	GetWorldDirection(FLOAT fX, FLOAT fZ);

	BOOL	GetValidDestination( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , ApmMap::BLOCKINGTYPE eType , AgpdCharacter * pSkipCharacter = NULL );
	protected:
		BOOL	CollisionCylinder	( AgpdCharacter * pSelf , AgpdCharacter * pBlock , AuPOS * pStart , AuPOS * pDirection , AuPOS * pValid , FLOAT * pfDistance );
		BOOL	CollisionBox		( AgpdCharacter * pSelf , AgpdCharacter * pBlock , AuPOS * pStart , AuPOS * pDirection , AuPOS * pValid , FLOAT * pfDistance );

	public:

	// Valid가 업데이트 되경우 TRUE 리턴.
	BOOL	IsDisableCharacter(AgpdCharacter *pcsCharacter);
	BOOL	IsUseCharacter(AgpdCharacter *pcsCharacter);
	BOOL	IsRepairCharacter(AgpdCharacter *pcsCharacter);
	
	BOOL	PayTax(AgpdCharacter *pcsCharacter, INT64 llTax);
	INT32	GetTaxRatio(AgpdCharacter *pcsCharacter);

	BOOL	IsArchlord(CHAR* szCharName);
	BOOL	IsArchlord(AgpdCharacter* pcsCharacter);

	ApmMap::BLOCKINGTYPE	GetBlockingType( AgpdCharacter* pcsCharacter );

	// 같은 종족인지 검사 (중국은 다른 종족은 때린다. 패널티두 없다.)
	BOOL IsSameRace(AgpdCharacter *pcsPC1, AgpdCharacter *pcsPC2);
	BOOL IsSameRace(AgpdCharacter *pcsPC, INT32 lRace);
	
	CHAR*	GetCharismaTitle(AgpdCharacter *pAgpdCharacter);
	INT32	GetCharismaStep(AgpdCharacter *pAgpdCharacter);
	
	BOOL	IsInJail(AgpdCharacter* pcsCharacter);
	BOOL	IsInConsulationOffice(AgpdCharacter* pcsCharacter);

	PVOID	MakePacketOnlineTime(AgpdCharacter* pcsCharacter, INT16* pnPacketLength);
	PVOID	MakePacketGameguardAuth( AgpdCharacter* pcsCharacter, PVOID pggData, INT16 ggDataLength, INT16* pPacketLength );
	PVOID	MakePacketResurrectionByOther(INT16* pnPacketLength, INT32 lCID, CHAR* szName, INT32 lFlag);
	PVOID	MakePacketEventEffectID(INT16* pnPacketLength, INT32 lCID, INT32 lEventEffectID);

	void	SetEventEffectID(INT32 lEventEffectID) { m_lEventEffectID = lEventEffectID; }
	INT32	GetEventEffectID() { return m_lEventEffectID; }

	inline	INT32 GetCurrentRegionLevelLimit(AgpdCharacter* pcsCharacter)
	{
		ApmMap::RegionTemplate	* pTemplate = m_pcsApmMap->GetTemplate(pcsCharacter->m_nBindingRegionIndex);
		return pTemplate ? pTemplate->nLevelLimit : 0 ;
	}

	inline	INT32 GetCurrentRegionLevelLimit(AuPOS& stPos)
	{
		UINT16 unRegionIndex = m_pcsApmMap->GetRegion(stPos.x, stPos.z);
		ApmMap::RegionTemplate	* pTemplate = m_pcsApmMap->GetTemplate(unRegionIndex);
		return pTemplate ? pTemplate->nLevelLimit : 0 ;
	}

	BOOL	SetNationFlag(AgpdCharacter* pcsCharacter, INT32 nFlagType);
};

#endif
