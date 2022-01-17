// AgpmGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 15.
//
//
// Lock 관하여.
// 모든 Lock 은 Character Lock 을 먼저 한 후 -> Guild Lock 을 하게끔 처리한다.

#ifndef _AGPMGUILD_H_
#define _AGPMGUILD_H_

#include "ApModule.h"
#include "AuPacket.h"

#include "AgpmCharacter.h"
#include "AgpdGuild.h"
#include "AgpaGuild.h"
#include "AgpmItem.h"
#include <list>

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgpmGuildD.lib")
#else
	#pragma comment(lib, "AgpmGuild.lib")
#endif
#endif

#define AGPMGUILD_LOG_FILE_NAME				"AgsmGuild_Log.log"

#define	AGPMGUILD_MARK_BOTTOM				"Bottom"
#define	AGPMGUILD_MARK_PATTERN				"Pattern"
#define	AGPMGUILD_MARK_SYMBOL				"Symbol"
#define	AGPMGUILD_MARK_COLOR				"Color"

#define	AGPMGUILD_ID_MARK_BOTTOM			1
#define	AGPMGUILD_ID_MARK_PATTERN			2
#define	AGPMGUILD_ID_MARK_SYMBOL			3
#define	AGPMGUILD_ID_MARK_COLOR				4

//. 2005. 10. 20. Nonstopdj
//. Guild Mark Combination limit count
#define AGPMGUILD_ID_MARK_LIMIT				3

#define	AGPMGUILD_GUILDMARK_MAX_COLOR						24

#define AGPMGUILD_GUILDMARK_ENABLE_MEMBER_COUNT				10
#define AGPMGUILD_GUILDMEMBER_INCREASE_ENABLE_MEMBER_COUNT	20

typedef enum _eAgpmGuildPacketType
{
	AGPMGUILD_PACKET_CREATE = 0,
	AGPMGUILD_PACKET_JOIN_REQUEST,
	AGPMGUILD_PACKET_JOIN_REJECT,
	AGPMGUILD_PACKET_JOIN,
	AGPMGUILD_PACKET_LEAVE,
	AGPMGUILD_PACKET_FORCED_LEAVE,
	AGPMGUILD_PACKET_DESTROY,
	AGPMGUILD_PACKET_UPDATE_MAX_MEMBER_COUNT,
	AGPMGUILD_PACKET_CHAR_DATA,
	AGPMGUILD_PACKET_UPDATE_NOTICE,
	AGPMGUILD_PACKET_SYSTEM_MESSAGE,

	AGPMGUILD_PACKET_UPDATE_STATUS,
	AGPMGUILD_PACKET_UPDATE_RECORD,

	AGPMGUILD_PACKET_BATTLE_INFO,
	AGPMGUILD_PACKET_BATTLE_PERSON,		//Battle참여하는 인원들..
	AGPMGUILD_PACKET_BATTLE_REQUEST,
	AGPMGUILD_PACKET_BATTLE_ACCEPT,
	AGPMGUILD_PACKET_BATTLE_REJECT,
	AGPMGUILD_PACKET_BATTLE_CANCEL_REQUEST,
	AGPMGUILD_PACKET_BATTLE_CANCEL_ACCEPT,
	AGPMGUILD_PACKET_BATTLE_CANCEL_REJECT,
	AGPMGUILD_PACKET_BATTLE_START,
	AGPMGUILD_PACKET_BATTLE_UPDATE_TIME,
	AGPMGUILD_PACKET_BATTLE_UPDATE_SCORE,
	AGPMGUILD_PACKET_BATTLE_WITHDRAW,
	AGPMGUILD_PACKET_BATTLE_RESULT,
	AGPMGUILD_PACKET_BATTLE_MEMBER,				//Battle Member들의 결과
	AGPMGUILD_PACKET_BATTLE_MEMBER_LIST,		//Battle Member List들의 결과
	AGPMGUILD_PACKET_GUILDLIST,

	AGPMGUILD_PACKET_LEAVE_ALLOW,
	AGPMGUILD_PACKET_RENAME_GUILDID,
	AGPMGUILD_PACKET_RENAME_CHARACTERID,

	AGPMGUILD_PACKET_BUY_GUILDMARK,				//	2005.10.19. By SungHoon
	AGPMGUILD_PACKET_BUY_GUILDMARK_FORCE,		//	2005.10.20. By SungHoon

	AGPMGUILD_PACKET_JOINT_REQUEST,
	AGPMGUILD_PACKET_JOINT_REJECT,
	AGPMGUILD_PACKET_JOINT,
	AGPMGUILD_PACKET_JOINT_LEAVE,
	AGPMGUILD_PACKET_HOSTILE_REQUEST,
	AGPMGUILD_PACKET_HOSTILE_REJECT,
	AGPMGUILD_PACKET_HOSTILE,
	AGPMGUILD_PACKET_HOSTILE_LEAVE_REQUEST,
	AGPMGUILD_PACKET_HOSTILE_LEAVE_REJECT,
	AGPMGUILD_PACKET_HOSTILE_LEAVE,

	AGPMGUILD_PACKET_JOINT_DETAIL,
	AGPMGUILD_PACKET_HOSTILE_DETAIL,

	AGPMGUILD_PACKET_BATTLE_ROUND,
	AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP,
	AGPMGUILD_PACKET_CLASS_SOCIETY_SYSTEM,
	AGPMGUILD_PACKET_MAX,
} eAgpmGuildPacketType;

typedef enum _eAgpmGuildCallback
{
	AGPMGUILD_CB_CREATE = 0,
	AGPMGUILD_CB_CREATE_CHECK,
	AGPMGUILD_CB_CREATE_ENABLE_CHECK,
	AGPMGUILD_CB_CREATE_FAIL,
	AGPMGUILD_CB_JOIN_REQUEST,
	AGPMGUILD_CB_JOIN_ENABLE_CHECK,
	AGPMGUILD_CB_JOIN_FAIL,
	AGPMGUILD_CB_JOIN_REJECT,
	AGPMGUILD_CB_JOIN,
	AGPMGUILD_CB_LEAVE,
	AGPMGUILD_CB_LEAVE_FAIL,
	AGPMGUILD_CB_FORCED_LEAVE,
	AGPMGUILD_CB_FORCED_LEAVE_FAIL,
	AGPMGUILD_CB_DESTROY_PREPROCESS,
	AGPMGUILD_CB_DESTROY,
	AGPMGUILD_CB_DESTROY_FAIL,
	AGPMGUILD_CB_CHECK_DESTROY,
	AGPMGUILD_CB_CHECK_PASSWORD,
	AGPMGUILD_CB_UPDATE_MAX_MEMBER_COUNT,
	AGPMGUILD_CB_CHAR_DATA,
	AGPMGUILD_CB_UPDATE_MEMBER_STATUS,
	AGPMGUILD_CB_UPDATE_NOTICE,
	AGPMGUILD_CB_SYSTEM_MESSAGE,

	AGPMGUILD_CB_UPDATE_STATUS,
	AGPMGUILD_CB_UPDATE_RECORD,

	AGPMGUILD_CB_BATTLE_INFO,
	AGPMGUILD_CB_BATTLE_PERSON,
	AGPMGUILD_CB_BATTLE_REQUEST,
	AGPMGUILD_CB_BATTLE_ACCEPT,
	AGPMGUILD_CB_BATTLE_REJECT,
	AGPMGUILD_CB_BATTLE_CANCEL_REQUEST,
	AGPMGUILD_CB_BATTLE_CANCEL_ACCEPT,
	AGPMGUILD_CB_BATTLE_CANCEL_REJECT,
	AGPMGUILD_CB_BATTLE_START,
	AGPMGUILD_CB_BATTLE_UPDATE_TIME,
	AGPMGUILD_CB_BATTLE_UPDATE_SCORE,
	AGPMGUILD_CB_BATTLE_WITHDRAW,
	AGPMGUILD_CB_BATTLE_RESULT,
	AGPMGUILD_CB_BATTLE_MEMBER,
	AGPMGUILD_CB_BATTLE_MEMBER_LIST,
	AGPMGUILD_CB_BATTLE_ROUND,

	AGPMGUILD_CB_GUILDLIST,		//	2005.07.08 By SungHoon
	AGPMGUILD_CB_JOIN_REQUEST_SELF,		//	2005.07.28. By SungHoon
	AGPMGUILD_CB_LEAVE_REQUEST_SELF,		//	2005.07.28. By SungHoon
	AGPMGUILD_CB_DESTROY_GUILD_CHECK_TIME,	//	2005.08.17. By SungHoon
	AGPMGUILD_CB_RENAME_GUILDID,	//	2005.08.24. By SungHoon
	AGPMGUILD_CB_RENAME_CHARID,		//	2005.09.05. By SungHoon
	AGPMGUILD_CB_GUILDMARKTEMPLATE_LOAD,		//	2005.10.07. By SungHoon
	AGPMGUILD_CB_BUY_GUILDMARK,		//	2005.10.19. By SungHoon
	AGPMGUILD_CB_GET_GUILDMARK_TEXTURE,		//	2005.10.19. By SungHoon
	AGPMGUILD_CB_GET_GUILDMARK_SMALL_TEXTURE,		//	2005.10.19. By SungHoon
	AGPMGUILD_CB_BUY_GUILDMARK_FORCE,		//	2005.10.20. By SungHoon

	AGPMGUILD_CB_JOINT_REQUEST,
	AGPMGUILD_CB_JOINT_REJECT,
	AGPMGUILD_CB_JOINT,
	AGPMGUILD_CB_JOINT_LEAVE,
	AGPMGUILD_CB_HOSTILE_REQUEST,
	AGPMGUILD_CB_HOSTILE_REJECT,
	AGPMGUILD_CB_HOSTILE,
	AGPMGUILD_CB_HOSTILE_LEAVE_REQUEST,
	AGPMGUILD_CB_HOSTILE_LEAVE_REJECT,
	AGPMGUILD_CB_HOSTILE_LEAVE,

	AGPMGUILD_CB_JOINT_DETAIL,
	AGPMGUILD_CB_HOSTILE_DETAIL,

	AGPMGUILD_CB_GET_JOINT_MARK,
	AGPMGUILD_CB_GET_JOINT_COLOR,

	AGPMGUILD_CB_WORLD_CHAMPIONSHIP_REQUEST,
	AGPMGUILD_CB_WORLD_CHAMPIONSHIP_ENTER,
	AGPMGUILD_CB_WORLD_CHAMPIONSHIP_RESULT,

	AGPMGUILD_CB_CLASSSOCIETY_SYSTEM_MESSAGE,
	AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_REQUEST,
	AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_ASK,
	AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_ANSWER,

	AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_REQUEST,
	AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_ASK,
	AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_ANSWER,
	AGPMGUILD_CB_CLASSSOCIETY_MEMBERRANK_UPDATE,

	AGPMGUILD_CB_MAX,
} eAgpmGuildCallback;

typedef enum _eAgpmGuildSystemCode
{
	AGPMGUILD_SYSTEM_CODE_EXIST_GUILD_ID = 0,				// 길드 생성 시 이미 같은 이름이 있음
	AGPMGUILD_SYSTEM_CODE_NEED_MORE_LEVEL,					// Level 이 필요치에 못 미침.
	AGPMGUILD_SYSTEM_CODE_NEED_MORE_MONEY,					// Money 가 더 필요함
	AGPMGUILD_SYSTEM_CODE_NEED_ITEM,						// 아이템이 필요함.
	AGPMGUILD_SYSTEM_CODE_USE_SPECIAL_CHAR,					// 길드 이름에 특수 문자를 넣었음
	AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER,					// 길드 가입시 이미 길드에 가입되어 있음.
	AGPMGUILD_SYSTEM_CODE_MAX_MEMBER,						// 길드 멤버가 최고치까지 가입되어 있음.
	AGPMGUILD_SYSTEM_CODE_JOIN_FAIL,						// 그냥 가입 실패. -0-;;
	AGPMGUILD_SYSTEM_CODE_LEAVE_FAIL,						// 탈퇴 실패 -0-;;
	AGPMGUILD_SYSTEM_CODE_NOT_MASTER,						// Master 가 아님
	AGPMGUILD_SYSTEM_CODE_INVALID_PASSWORD,					// Password 가 틀림

	AGPMGUILD_SYSTEM_CODE_GUILD_CREATE_COMPLETE,			// 길드 만들기 성공
	AGPMGUILD_SYSTEM_CODE_GUILD_DESTROY,
	AGPMGUILD_SYSTEM_CODE_GUILD_JOIN,
	AGPMGUILD_SYSTEM_CODE_GUILD_JOIN_REJECT,
	AGPMGUILD_SYSTEM_CODE_GUILD_LEAVE,
	AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE,
	AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER2,					// 마스터가 초대할 때 초대 받는 이가 멤버이면 마스터에게 이걸 보낸다.
	AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE2,				// 강퇴당한 캐릭터에게 간다.

	AGPMGUILD_SYSTEM_CODE_BATTLE_BOTH_MEMBER_COUNT,			// 멤버수가 동일한 Type인데 다르넹..
	AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_ENOUGH_MEMBER_COUNT,	// 멤버수가 모자란다.
	AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_ENOUGH_MEMBER_COUNT2,	// 상대길드의 멤버수가 모자란다.
	AGPMGUILD_SYSTME_CODE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL,	// 멤버의 레벨합이 모자란다.
	AGPMGUILD_SYSTME_CODE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL2,	// 상대길드의 멤버 레벨합이 모자란다.
	AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS,		// 길드전 신청할 수 없는 상태이다.
	AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS2,		// 상대길드가 길드전 받을 수 없는 상태이다.
	AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE,		// 상대길마가 오프라인.
	AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_EXIST_GUILD,			// 그런 길드는 없습니다.
	AGPMGUILD_SYSTEM_CODE_BATTLE_REQUEST,					// 길드전 신청 성공.
	AGPMGUILD_SYSTEM_CODE_BATTLE_REJECT,					// 길드 배틀을 거절했음.
	AGPMGUILD_SYSTEM_CODE_BATTLE_REJECT_BY_OTHER,			// 상대방이 길드 배틀을 거절했음.
	AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_PASSED_TIME,		// 취소할 수 있는 시간이 지나버렸음.
	AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_REJECT,				// 배틀취소를 거절했음.
	AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_REJECT_BY_OTHER,	// 상대방이 배틀취소를 거절했음.
	AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_ACCEPT,				// 길드전이 취소되었음.
	AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_WITHDRAW_UNTIL_START,	// 시작 전까지는 GG 칠 수 없음.
	AGPMGUILD_SYSTEM_CODE_BATTLE_WITHDRAW,					// 우리 길드가 GG 쳤음.
	AGPMGUILD_SYSTEM_CODE_BATTLE_WITHDRAW_BY_WIN,			// 상대 길드가 GG 쳐서 이겼음.
	AGPMGUILD_SYSTEM_CODE_BATTLE_WIN,						// 이겼다~
	AGPMGUILD_SYSTEM_CODE_BATTLE_DRAW,						// 비겼다~
	AGPMGUILD_SYSTEM_CODE_BATTLE_LOSE,						// 졌다~
	AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE,				// 길드전 중에는 할 수 없음.
	AGPMGUILD_SYSTEM_CODE_GUILD_JOIN_REFUSE,				// 길드 초대 거부 상태이다.
	AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE,					// 길드전 거부 상태이다.
	AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_GHELD_FOR_INCREASE_MAX,	//	길드 최대인원증가에 필요한 겔드가 부족하다.
	AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_SKULL_FOR_INCREASE_MAX,	//	길드 최대인원증가에 필요한 해골이 부족하다.
	AGPMGUILD_SYSTEM_CODE_DESTROY_FAIL_TOO_EARLY,	//	길드 최대인원증가에 필요한 해골이 부족하다.
	AGPMGUILD_SYSTEM_CODE_NO_EXIST_SEARCH_GUILD,			//	찾으려는 길드가 존재하지 않습니다.
	AGPMGUILD_SYSTEM_CODE_JOIN_REQUEST_SELF,				//	가입신청을 했다.
	AGPMGUILD_SYSTEM_CODE_JOIN_REQUEST_REJECT,				//	가입신청을 취소했다.
	AGPMGUILD_SYSTEM_CODE_LEAVE_REQUEST,				//	탈퇴신청을 했다.

	AGPMGUILD_SYSTEM_CODE_RENAME_USE_SPECIAL_CHAR,		//	길드 이름 바꾸기 특수 문자
	AGPMGUILD_SYSTEM_CODE_RENAME_EXIST_GUILD_ID,		//	길드 이름 바꾸기 존재하는 길드
	AGPMGUILD_SYSTEM_CODE_RENAME_NOT_MASTER,			//	길드 이름 바꾸기 길마가 아님
	AGPMGUILD_SYSTEM_CODE_RENAME_IMPOSIBLE_GUILD_ID,	//	길드 이름 바꾸기 불가능한 아이디

	AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NO_EXIST_SKULL,		//	해골이 없다.
	AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NOT_ENOUGHT_SKULL,	//	해골이 부족하다.
	AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NOT_ENOUGHT_GHELD,	//	겔드가 부족하다.
	AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NO_EXIST_BOTTOM,	//	바탕을 선택하지 않았다.
	AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_DUPLICATE,			//	중복되는 길드마크가 있다.

	AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE,					// 연대할 수 없는 길드임.
	AGPMGUILD_SYSTEM_CODE_JOINT_NOT_LEADER,					// 연대장이 아님.
	AGPMGUILD_SYSTEM_CODE_JOINT_ALREADY_JOINT,				// 이미 연대 길드임.
	AGPMGUILD_SYSTEM_CODE_JOINT_MAX,						// 맥스치에 찼음.
	AGPMGUILD_SYSTEM_CODE_JOINT_MASTER_OFFLINE,				// 상대 길마가 오프라인.
	AGPMGUILD_SYSTEM_CODE_JOINT_NOT_ENOUGH_MEMBER,			// 멤버수 부족.
	AGPMGUILD_SYSTEM_CODE_JOINT_OHTER_NOT_ENOUGH_MEMBER,	// 멤버수 부족.
	AGPMGUILD_SYSTEM_CODE_JOINT_WAIT,						// 연대신청하였음.
	AGPMGUILD_SYSTEM_CODE_JOINT_REJECT,						// 연대거절.
	AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE,					// 연대실패.
	AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS,					// 연대성공.
	AGPMGUILD_SYSTEM_CODE_JOINT_LEAVE_SUCCESS,				// 연대탈퇴.
	AGPMGUILD_SYSTEM_CODE_JOINT_LEAVE_OTHER_GUILD,			// 다른길드가 연대탈퇴.
	AGPMGUILD_SYSTEM_CODE_JOINT_DESTROY,					// 연대해체.
	AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS2,					// 

	AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE,					// 적대할 수 없는 길드임.
	AGPMGUILD_SYSTEM_CODE_HOSTILE_ALREADY_HOSTILE,			// 이미 적대 길드임.
	AGPMGUILD_SYSTEM_CODE_HOSTILE_MAX,						// 맥스치에 찼음.
	AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE,			// 상대 길마가 오프라인.
	AGPMGUILD_SYSTEM_CODE_HOSTILE_NOT_ENOUGH_MEMBER,		// 멤버수 부족.
	AGPMGUILD_SYSTEM_CODE_HOSTILE_OTHER_NOT_ENOUGH_MEMBER,	// 멤버수 부족.
	AGPMGUILD_SYSTEM_CODE_HOSTILE_WAIT,						// 적대신청하였음.
	AGPMGUILD_SYSTEM_CODE_HOSTILE_REJECT,					// 적대거절.
	AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE,					// 적대실패.
	AGPMGUILD_SYSTEM_CODE_HOSTILE_SUCCESS,					// 적대성공

	AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_DISABLE,
	AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_WAIT,
	AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_REJECT,
	AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_FAILURE,
	AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_SUCCESS,

	AGPMGUILD_SYSTEM_CODE_ARCHLORD_DEFENSE_SUCCESS,			// 아크로드 방어에 성공
	AGPMGUILD_SYSTEM_CODE_ARCHLORD_DEFENSE_FAILURE,			// 아크로드 방어에 실패
	AGPMGUILD_SYSTEM_CODE_ARCHLORD_ATTACK_SUCCESS,			// 아크로드 탈환에 성공
	AGPMGUILD_SYSTEM_CODE_ARCHLORD_ATTACK_FAILURE,			// 아크로드 탈환에 실패

	AGPMGUILD_SYSTEM_CODE_REFUSE_RELATION,					// 연대/적대 거절
	
	AGPMGUILD_SYSTEM_CODE_WAREHOUSE_NOT_ENOUGH_MEMBER,		// 멤버수가 부족하여, 길드창고를 사용할수 없습니다.
	AGPMGUILD_SYSTEM_CODE_GUILD_DESTROY_NOT_EMPTY_WAREHOUSE,	// 길드창고가 비어있지않아 길드를 해체할수 없습니다.
} eAgpmGuildSystemCode;

typedef enum _eAgpmGuildModuleDataType
{
	AGPMGUILD_DATA_TYPE_GUILD = 0,
	AGPMGUILD_DATA_TYPE_MEMBER,
	AGPMGUILD_DATA_TYPE_BATTLE_POINT,
	AGPMGUILD_DATA_TYPE_NUM,
	AGPMGUILD_DATA_TYPE_GUILD_MARK_TEMPLATE,
} eAgpmGuildModuleDataType;


#define AGPMGUILD_CREATE_REQUIRE_LEVEL					20
//#define AGPMGUILD_CREATE_REQUIRE_MONEY					3500
//#define AGPMGUILD_CREATE_REQUIRE_MONEY					10000	돈과 아이템은 MaxMember 설정에서 읽어서 처리
#define AGPMGUILD_BATTLE_POINT_LEVEL_GAP_ZERO			123456

// commented by ST
//#define AGPMGUILD_SYSTEM_MESSAGE_NEED_MORE_LEVEL					"캐릭터 레벨이 20이 되면 길드를 만들 수 있습니다"
//#define AGPMGUILD_SYSTEM_MESSAGE_NEED_MORE_MONEY					"겔드가 부족합니다."
//#define AGPMGUILD_SYSTEM_MESSAGE_ALREADY_MEMBER						"길드에 가입한 상태입니다. 길드 탈퇴 후 다시 오십시오"
//#define AGPMGUILD_SYSTEM_MESSAGE_MAX_MEMBER							"길드원이 다 찼습니다"
//#define AGPMGUILD_SYSTEM_MESSAGE_INVALID_RACE						"종족이 다릅니다"
//#define AGPMGUILD_SYSTEM_MESSAGE_EXIST_GUILD_ID						"같은 길드명이 이미 있습니다. 다시 작성해주십시오"
//#define AGPMGUILD_SYSTEM_MESSAGE_USE_SPECIAL_CHAR					"특수 문자는 사용할 수 없습니다"
//#define AGPMGUILD_SYSTEM_MESSAGE_JOIN_FAIL							"가입에 실패하였습니다"
//#define AGPMGUILD_SYSTEM_MESSAGE_JOIN_REJECT						"상대방이 가입을 거절하였습니다"
//#define AGPMGUILD_SYSTEM_MESSAGE_INVALID_PASSWORD					"비밀번호가 맞지 않습니다"
//
//#define AGPMGUILD_SYSTEM_MESSAGE_GUILD_CREATE_COMPLETE				"길드를 생성하였습니다"
//#define AGPMGUILD_SYSTEM_MESSAGE_GUILD_DESTROY						"길드가 해체되었습니다"
//#define AGPMGUILD_SYSTEM_MESSAGE_GUILD_LEVEL_UP						"레벨 20이 되면 길드를 만들 수 있습니다."

#define	AGPMGUILD_MAX_AGPD_REQUIRE_MAX_MEMBER_INCREASE				6
#define	AGPMGUILD_MAX_AGPD_GUILDMARK_TEMPLATE						5
#define AGPMGUILD_GUILD_DESTROY_TIME								60 * 60 * 24		//	하루

////GuildMark 부분 모듈화
//class CGuildMark
//{
//public:
//	CGuildMark();
//	~CGuildMark();
//
//	AgpdGuildMarkTemplate* CreateGuildMarkTemplate();							//	2005.10.05. By SungHoon
//	VOID DestroyGuildMarkTemplate(AgpdGuildMarkTemplate *pGuildMarkTemplate);	//	2006.10.05. By SungHoon
//
//	AgpdGuildMarkTemplate* AddGuildMarkTemplate(CHAR *szName, INT32 lTypeCode, INT32 lTID, INT32 lIndex, INT32 lGheld, INT32 lSkullTID, INT32 lSkullCount);	//	2005.10.05. By SungHoon
//
//	AgpdGuildMarkTemplate *GetGuildMarkTemplate(INT32 lGuildTemplateID);		//	2005.10.05. By SungHoon
//	BOOL ReadGuildMarkTemplate( CHAR *szFileName, BOOL bEncryption);			//	2005.10.06. By SungHoon
//	BOOL ReReadGuildMarkTemplate( CHAR *szFileName, BOOL bEncryption);			//	2005.10.06. By SungHoon
//
//	VOID DeleteAllGuildMark();													//	2005.10.07. By SungHoon
//
//	//	2005.10.05. By SungHoon
//	INT16 AttachGuildMarkTemplateData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
//
//	INT32 GetMaxGuildMarkGridItem();
//	INT32 GetMaxGuildMarkColorGridItem();
//
//	INT32 SetGuildMarkID(INT32 lTypeCode, INT32 lID, INT32 lSettingID);
//	INT32 GetGuildMarkID(INT32 lTypeCode, INT32 lID);
//
//	INT32 SetGuildMarkColor(INT32 lTypeCode, INT32 lID, INT32 lSettingID);
//	INT32 GetGuildMarkColor(INT32 lTypeCode, INT32 lID);
//	INT32 GetGuildMarkColorID(INT32 lTypeCode, INT32 lID);
//
//	AgpdGuildMarkTemplate* GetGuildMarkTemplate(INT32 lTypeCode, INT32 lGuildMarkTemplateID);
//
//	VOID *GetGuildMarkTexture(INT32 lTypeCode, INT32 lGuildMarkTemplateID, BOOL bSmallTexture );
//	VOID *GetGuildMarkTexture(INT32 lTypeCode, AgpdCharacter *pcsCharacter, BOOL bSmallTexture );
//	AgpdGuildMarkTemplate* GetGuildMarkTemplate(INT32 lTypeCode, AgpdCharacter *pcsCharacter);
//	INT32 GetGuildMarkColor(INT32 lTypeCode, AgpdCharacter *pcsCharacter);
//
//	INT32 GetGuildMarkID(AgpdCharacter *pcsCharacter);
//	INT32 GetGuildMarkColor(AgpdCharacter *pcsCharacter);
//
//	INT32 GetJointGuildMarkID(CHAR* szGuildID);
//	INT32 GetJointGuildMarkColor(CHAR* szGuildID);
//
//	BOOL IsEnableBuyGuildMark(INT32 lGuildMarkTID, AgpdCharacter *pcsCharacter);
//	BOOL IsDuplicateGuildMark(INT32 lGuildMarkTID);
//
//private:
//	ApAdmin	m_csGuildMarkTemplateList;
//
//	INT32	m_lGuildMarkBottomCount;
//	INT32	m_lGuildMarkPatternCount;
//	INT32	m_lGuildMarkSymbolCount;
//	INT32	m_lGuildMarkColorCount;
//
//};

#define NEXT_GUILDMAX_MEMBER( ___pcsGuild___ ) (___pcsGuild___->m_lMaxMemberCount > 0 ) ? ___pcsGuild___->m_lMaxMemberCount + ( 10 - (___pcsGuild___->m_lMaxMemberCount % 10) ) : 10;

#pragma pack(1)

struct PACKET_GUILD
{

};

struct PACKET_GUILD_BATTLE : public PACKET_GUILD
{
	INT16*	pnPacketLength;
	INT32*	plCID;
	INT8*	pcOperation;
	CHAR*	szGuildID;
	CHAR*	szMasterID;
	INT8*	pcStatus;
	INT32*	plWin;
	INT32*	plDraw;
	INT32*	plLose;
	INT32*	pGuildPoint;
	CHAR*	szEnemyGuildID;
	CHAR*	szEnemyGuildMasterID;
	UINT32*	pulAcceptTime;
	UINT32*	pulReadyTime;
	UINT32*	pulStartTime;
	INT32*	pType;
	UINT32*	pulDuration;
	UINT32*	pPerson;
	UINT32*	pulCurrentTime;
	INT32*	plMyScore;
	INT32*	plEnemyScore;
	INT32*	plMyUpScore;
	INT32*	plEnemyUpScore;
	INT8*	pcBattleResult;
	UINT32*	ulRound;

	PACKET_GUILD_BATTLE()
		:	pnPacketLength(NULL), plCID(NULL), pcOperation(NULL), szGuildID(NULL), szMasterID(NULL), pcStatus(NULL), 
		plWin(NULL), plDraw(NULL), plLose(NULL), pGuildPoint(NULL), szEnemyGuildID(NULL), szEnemyGuildMasterID(NULL),
		pulAcceptTime(NULL), pulReadyTime(NULL), pulStartTime(NULL), pType(NULL), pulDuration(NULL), pPerson(NULL), pulCurrentTime(NULL),
		plMyScore(NULL), plEnemyScore(NULL), plMyUpScore(NULL), plEnemyUpScore(NULL), pcBattleResult(NULL), ulRound(NULL)
	{
	};
	
	PACKET_GUILD_BATTLE(INT16* PacketLength, INT32* lCID, INT8* Operation, CHAR* GuildID, CHAR* MasterID,
			INT8* cStatus, INT32* lWin, INT32* lDraw, INT32* lLose, INT32* GuildPoint, CHAR* EnemyGuildID, CHAR* EnemyGuildMasterID,
			UINT32* AcceptTime, UINT32* ReadyTime, UINT32* StartTime, 
			INT32* Type, UINT32* Duration, UINT32* Person, 
			UINT32* CurrentTime, INT32* MyScore, INT32* EnemyScore, INT32* MyUpScore, INT32* EnemyUpScore, INT8* BattleResult, UINT32* Round)
		:	pnPacketLength(NULL), plCID(NULL), pcOperation(NULL), szGuildID(NULL), szMasterID(NULL), pcStatus(NULL), 
		plWin(NULL), plDraw(NULL), plLose(NULL), pGuildPoint(NULL), szEnemyGuildID(NULL), szEnemyGuildMasterID(NULL),
		pulAcceptTime(NULL), pulReadyTime(NULL), pulStartTime(NULL), pType(NULL), pulDuration(NULL), pPerson(NULL), pulCurrentTime(NULL),
		plMyScore(NULL), plEnemyScore(NULL), plMyUpScore(NULL), plEnemyUpScore(NULL), pcBattleResult(NULL), ulRound(NULL)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;	
		pcOperation		= Operation;
		szGuildID		= GuildID;	
		szMasterID		= MasterID;
		pcStatus		= cStatus;
		plWin			= lWin;
		plDraw			= lDraw;
		plLose			= lLose;
		pGuildPoint		= GuildPoint;
		szEnemyGuildID	= EnemyGuildID;
		szEnemyGuildMasterID = EnemyGuildMasterID;
		pulAcceptTime	= AcceptTime;
		pulReadyTime	= ReadyTime;
		pulStartTime	= StartTime;
		pType			= Type;
		pulDuration		= Duration;
		pPerson			= Person;
		pulCurrentTime	= CurrentTime;
		plMyScore		= MyScore;
		plEnemyScore	= EnemyScore;
		plMyUpScore		= MyUpScore;
		plEnemyUpScore	= EnemyUpScore;
		pcBattleResult	= BattleResult;
		ulRound			= Round;
	};
};

struct PACKET_GUILD_BATTLE_INFO : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;
	
	PACKET_GUILD_BATTLE_INFO(INT16* PacketLength, INT32* lCID, CHAR* GuildID, CHAR* MasterID,
		INT8* cStatus, INT32* lWin, INT32* lDraw, INT32* lLose, INT32* GuildPoint, CHAR* EnemyGuildID, CHAR* EnemyGuildMasterID,
		UINT32* AcceptTime, UINT32* ReadyTime, UINT32* StartTime, 
		INT32* Type, UINT32* Duration, UINT32* Person, 
		UINT32* CurrentTime, INT32* MyScore, INT32* EnemyScore, INT32* MyUpScore, INT32* EnemyUpScore, INT8* BattleResult)
		: cOperation(AGPMGUILD_PACKET_BATTLE_INFO)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;	
		pcOperation		= &cOperation;
		
		szGuildID		= GuildID;	
		szMasterID		= MasterID;
		pcStatus		= cStatus;
		plWin			= lWin;
		plDraw			= lDraw;
		plLose			= lLose;
		pGuildPoint		= GuildPoint;
		szEnemyGuildID	= EnemyGuildID;
		szEnemyGuildMasterID = EnemyGuildMasterID;
		pulAcceptTime	= AcceptTime;
		pulReadyTime	= ReadyTime;
		pulStartTime	= StartTime;
		pType			= Type;
		pulDuration		= Duration;
		pPerson			= Person;
		pulCurrentTime	= CurrentTime;
		plMyScore		= MyScore;
		plEnemyScore	= EnemyScore;
		plMyUpScore		= MyUpScore;
		plEnemyUpScore	= EnemyUpScore;
		pcBattleResult	= BattleResult;
	};
};

struct PACKET_GUILD_BATTLE_RESULT : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;

	PACKET_GUILD_BATTLE_RESULT(INT16* PacketLength, INT32* lCID,
		CHAR* pszID, CHAR* pszEnemyGuildID, INT32* lMyScore, INT32* lEnemyScore, INT8* BattleResult)
		: cOperation(AGPMGUILD_PACKET_BATTLE_RESULT)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= &cOperation;

		szGuildID		= pszID;
		szEnemyGuildID	= pszEnemyGuildID;
		plMyScore		= lMyScore;
		plEnemyScore	= lEnemyScore;
		pcBattleResult	= BattleResult;
	}

};

struct PACKET_GUILD_BATTLE_REQUEST : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;
	
	PACKET_GUILD_BATTLE_REQUEST(INT16* PacketLength, INT32* lCID,
		CHAR* pszID, CHAR* pszMasterID, CHAR* pszEnemyGuildID, INT32* Type, UINT32* lDuration, UINT32* Person)
		: cOperation(AGPMGUILD_PACKET_BATTLE_REQUEST)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= &cOperation;
		
		szGuildID		= pszID;
		szMasterID		= pszMasterID;
		szEnemyGuildID	= pszEnemyGuildID;
		pType			= Type;
		pulDuration		= lDuration;
		pPerson			= Person;
	}
};

struct PACKET_GUILD_BATTLE_ACCEPT : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;
	
	PACKET_GUILD_BATTLE_ACCEPT(INT16* PacketLength, INT32* lCID, 
		CHAR* GuildID, CHAR* MasterID, CHAR* EnemyGuildID, UINT32* AcceptTime, UINT32* ReadyTime, UINT32* StartTime, 
		INT32* Type, UINT32* Duration, UINT32* Person)
		: cOperation(AGPMGUILD_PACKET_BATTLE_ACCEPT)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= &cOperation;
		
		szGuildID		= GuildID;
		szMasterID		= MasterID;
		szEnemyGuildID	= EnemyGuildID;
		pulAcceptTime	= AcceptTime;
		pulReadyTime	= ReadyTime;
		pulStartTime	= StartTime;
		pType			= Type;
		pulDuration		= Duration;
		pPerson			= Person;
	}
};

struct PACKET_GUILD_BATTLE_REJECT : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;
	
	PACKET_GUILD_BATTLE_REJECT(INT16* PacketLength, INT32* lCID, CHAR* GuildID, CHAR* MasterID, CHAR* EnemyGuildID)
		: cOperation(AGPMGUILD_PACKET_BATTLE_REJECT)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= &cOperation;
		
		szGuildID		= GuildID;
		szMasterID		= MasterID;
		szEnemyGuildID	= EnemyGuildID;
	}
};

struct PACKET_GUILD_BATTLE_CANCEL : public PACKET_GUILD_BATTLE
{
	PACKET_GUILD_BATTLE_CANCEL(INT16* PacketLength, INT32* lCID, INT8* Operation, CHAR* GuildID, CHAR* MasterID, CHAR* EnemyGuildID)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= Operation;
		
		szGuildID		= GuildID;
		szMasterID		= MasterID;
		szEnemyGuildID	= EnemyGuildID;
	}
};

struct PACKET_GUILD_BATTLE_START : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;
	
	PACKET_GUILD_BATTLE_START(INT16* PacketLength, INT32* lCID, CHAR* GuildID, CHAR* EnemyGuildID, CHAR* EnemyGuildMasterID, UINT32* StartTime, UINT32* Duration, UINT32* CurrentTime)
		: cOperation(AGPMGUILD_PACKET_BATTLE_START)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= &cOperation;
		
		szGuildID		= GuildID;
		szEnemyGuildID	= EnemyGuildID;
		szEnemyGuildMasterID = EnemyGuildMasterID;
		pulStartTime	= StartTime;
		pulDuration		= Duration;
		pulCurrentTime	= CurrentTime;
	}

};

struct PACKET_GUILD_BATTLE_UPDATE_TIME : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;

	PACKET_GUILD_BATTLE_UPDATE_TIME(INT16* PacketLength, INT32* lCID, CHAR* GuildID, CHAR* EnemyGuildID, UINT32* CurrentTime)
		: cOperation(AGPMGUILD_PACKET_BATTLE_UPDATE_TIME)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= &cOperation;

		szGuildID		= GuildID;
		szEnemyGuildID	= EnemyGuildID;
		pulCurrentTime	= CurrentTime;
	}
};

struct PACKET_GUILD_BATTLE_UPDATE_SCORE : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;

	PACKET_GUILD_BATTLE_UPDATE_SCORE(INT16* PacketLength, INT32* lCID, CHAR* GuildID, CHAR* EnemyGuildID, 
		INT32* MyScore, INT32* EnemyScore, INT32* MyUpScore, INT32* EnemyUpScore)
		: cOperation(AGPMGUILD_PACKET_BATTLE_UPDATE_SCORE)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= &cOperation;

		szGuildID		= GuildID;
		szEnemyGuildID	= EnemyGuildID;
		plMyScore		= MyScore;
		plEnemyScore	= EnemyScore;
		plMyUpScore		= MyUpScore;
		plEnemyUpScore	= EnemyUpScore;
	}
};

struct PACKET_GUILD_BATTLE_WITHDRAW : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;

	PACKET_GUILD_BATTLE_WITHDRAW(INT16* PacketLength, INT32* lCID, CHAR* GuildID, CHAR* MasterID, CHAR* EnemyGuildID)
		: cOperation(AGPMGUILD_PACKET_BATTLE_WITHDRAW)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= &cOperation;

		szGuildID		= GuildID;
		szMasterID		= MasterID;
		szEnemyGuildID	= EnemyGuildID;
	}
};

struct PACKET_GUILD_BATTLE_ROUND : public PACKET_GUILD_BATTLE
{
	INT8 cOperation;
	
	PACKET_GUILD_BATTLE_ROUND(INT16* PacketLength, INT32* lCID, CHAR* GuildID, UINT32* Round)
		: cOperation(AGPMGUILD_PACKET_BATTLE_ROUND)
	{
		pnPacketLength	= PacketLength;
		plCID			= lCID;
		pcOperation		= &cOperation;
		
		szGuildID		= GuildID;
		ulRound			= Round;
	}
};

#pragma pack()

class AgpmGuild : public ApModule
{
private:
	AgpmCharacter* m_pagpmCharacter;
	AgpmItem *m_pagpmItem;
	AgpmGrid *m_pagpmGrid;

public:
	AgpaGuild m_csGuildAdmin;
	AgpaGuild m_csRemoveAdmin;

private:
	UINT32 m_ulLastRemoveClockCount;

	ApAdmin m_csBattlePointAdmin;
	INT32 m_lBattlePointMinLevelGap;
	INT32 m_lBattlePointMaxLevelGap;

	INT16 m_nIndexADCharacter;

public:
	AuPacket m_csPacket;
	AuPacket m_csMemberPacket;
	AuPacket m_csBattlePacket;
	AuPacket m_csBattlePersonPacket;
	AuPacket m_csBattleMemberPacket;
	AuPacket m_csBattleMemberListPacket;
	AuPacket m_csGuildListPacket;
	AuPacket m_csGuildListItemPacket;
	
public:
	AgpmGuild();
	virtual ~AgpmGuild();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle2(UINT32 ulClockCount);
	BOOL OnDestroy();

	BOOL SetMaxGuild(INT lCount);

	// Attached Data
	AgpdGuildADChar* GetADCharacter(AgpdCharacter* pData);
	static BOOL ConAgpdGuildADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgpdGuildADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	// Attach Data
	INT16 AttachGuildData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT16 AttachMemberData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	// On Receive
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL OnOperationCreate(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, INT32 lGuildTID, INT32 lGuildRank,
								INT32 lCreationDate, INT32 lMaxMemberCount, INT32 lUnionID, CHAR* szPassword,
								CHAR* szNotice, INT16 nNoticeLength,
								INT8 cStatus, INT32 lWin, INT32 lDraw, INT32 lLose,
								INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL IsWinner);
	BOOL OnOperationCreateCheck(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, INT32 lGuildTID, INT32 lGuildRank,
								INT32 lCreationDate, INT32 lMaxMemberCount, INT32 lUnionID, CHAR* szPassword,
								CHAR* szNotice, INT16 nNoticeLength);
	BOOL OnOperationJoinRequest(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID);
	BOOL OnOperationJoinReject(INT32 lCID, CHAR* szGuildID, CHAR* szCharID);
	BOOL OnOperationJoin(INT32 lCID, CHAR* szGuildID, CHAR* szMemberID, INT32 lMemberRank, INT32 lJoinDate,
								INT32 lLevel, INT32 lTID, INT8 cStatus);
	BOOL OnOperationLeave(INT32 lCID, CHAR* szGuildID, CHAR* szMemberID);
	BOOL OnOperationForcedLeave(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szMemberID);
	BOOL OnOperationLeaveAllow(INT32 lCID, CHAR* szGuildID, CHAR* szCharID, BOOL bNotifyMaster, BOOL bAdmin = FALSE);		//	2005.08.01. By SungHoon
	BOOL OnOperationRenameGuildID(INT32 lCID, CHAR* szGuildID, CHAR* szCharID);		//	2005.08.24. By SungHoon

	BOOL OnOperationDestroy(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szPassword, BOOL bAdmin = FALSE);
	BOOL OnOperationUpdateMaxMemberCount(INT32, CHAR* szGuildID, INT32 lNewMaxMemberCount);
	BOOL OnOperationCharacterGuildData(INT32 lCID, CHAR* szGuildID, CHAR* szMemberID, INT32 lMemberRank, INT32 lJoinDate,
								INT32 lLevel, INT32 lTID, INT8 cStatus, INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL IsWinner);
	BOOL OnOperationUpdateNotice(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szNotice, INT16 nNoticeLength, BOOL bAdmin = FALSE);
	BOOL OnOperationSystemMessage(INT32 lCode, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2, AgpdCharacter* pcsCharacter = NULL);

	BOOL OnOperationUpdateStatus(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, INT8 cStatus);
	BOOL OnOperationBattleRecord(INT32 lCID, CHAR* szGuildID, INT32 lWin, INT32 lDraw, INT32 lLose, INT32 ulGuildPoint);

	BOOL OnOperationBattleInfo(INT32 lCID, CHAR* szGuildID, INT8 cStatus, CHAR* szEnemyGuildID, CHAR* szEnemyGuildMasterID,
								INT32 lWin, INT32 lDraw, INT32 lLose,
								UINT32 ulAcceptTime, UINT32 ulReadyTime, UINT32 ulStartTime,
								UINT32 lType, UINT32 ulDuration, UINT32 ulPerson, UINT32 ulCurrentTime,
								INT32 lMyScore, INT32 lEnemyScore, INT32 lMyUpScore, INT32 lEnemyUpScore, INT8 cResult);
	BOOL OnOperationBattlePerson(INT32 lCID, CHAR* szGuildID, INT16 lPersonCount, CHAR* pPersonList );
	BOOL OnOperationBattleRequest(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, UINT32 lType, UINT32 ulDuration, UINT32 ulPerson );
	BOOL OnOperationBattleAccept(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, UINT32 ulAcceptTime, UINT32 ulReadyTime, UINT32 ulStartTime,
								UINT32 lType, UINT32 ulDuration, UINT32 ulPerson );
	BOOL OnOperationBattleReject(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	BOOL OnOperationBattleCancelRequest(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	BOOL OnOperationBattleCancelAccept(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	BOOL OnOperationBattleCancelReject(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	BOOL OnOperationBattleStart(INT32 lCID, CHAR* szGuildID, CHAR* szEnemyGuildID, CHAR* szEnemyGuildMasterID, UINT32 ulStartTime, UINT32 ulDuration, UINT32 ulCurrentTime);
	BOOL OnOperationBattleUpdateTime(INT32 lCID, CHAR* szGuildID, CHAR* szEnemyGuildID, UINT32 ulAcceptTime, UINT32 ulReadyTime, UINT32 ulStartTime, UINT32 ulDuration, UINT32 ulCurrentTime);
	BOOL OnOperationBattleUpdateScore(INT32 lCID, CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lMyScore, INT32 lEnemyScore, INT32 lMyUpScore, INT32 lEnemyUpScore);
	BOOL OnOperationBattleWithdraw(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	BOOL OnOperationBattleResult(INT32 lCID, CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lMyScore, INT32 lEnemyScore, INT8 cBattleResult);
	BOOL OnOperationBattleMember(INT32 lCID, CHAR* szGuildID, CHAR* szCharID, UINT32 ulScore, UINT32 ulKill, UINT32 ulDeath );
	BOOL OnOperationBattleMemberList(INT32 lCID, INT32 lMyCount, AgpdGuildMemberBattleInfo* pMyMemberInfo,
											INT32 lEnemyCount, AgpdGuildMemberBattleInfo* pEnemyMemberInfo );
	BOOL OnOperationBattleRound(INT32 lCID, CHAR* szGuildID, UINT32 ulRound);
		

	BOOL OnOperationRenameCharacterID(CHAR* szGuildID, CHAR* szOldID, CHAR *szNewID);		//	2005.09.05. By SungHoon

	BOOL OnOperationBuyGuildMark(INT lCID, CHAR* szGuildID, INT32 lGuildMark, INT32 lGuildColor);	//	2005.10.19. By SungHoon
	BOOL OnOperationBuyGuildMarkForce(INT lCID, CHAR* szGuildID, INT32 lGuildMark, INT32 lGuildColor);	//	2005.10.19. By SungHoon

	BOOL OnOperationJointRequest(CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szMasterID);
	BOOL OnOperationJointReject(CHAR* szGuildID, CHAR* szJointGuildID);
	BOOL OnOperationJoint(CHAR* szGuildID, CHAR* szJointGuildID, UINT32 ulDate, INT8 cRelation);
	BOOL OnOperationJointLeave(INT32 lCID, CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szNewLeader);
	BOOL OnOperationHostileRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID);
	BOOL OnOperationHostileReject(CHAR* szGuildID, CHAR* szHostileGuildID);
	BOOL OnOperationHostile(CHAR* szGuildID, CHAR* szHostileGuildID, UINT32 ulDate);
	BOOL OnOperationHostileLeaveRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID);
	BOOL OnOperationHostileLeaveReject(CHAR* szGuildID, CHAR* szHostileGuildID);
	BOOL OnOperationHostileLeave(INT32 lCID, CHAR* szGuildID, CHAR* szHostileGuildID);

	BOOL OnOperationJointDetail(PVOID pvGuildListPacket);
	BOOL OnOperationHostileDetail(PVOID pvGuildListPacket);

	// AgpaGuild 를 관리
	BOOL AddGuild(AgpdGuild* pcsGuild);
	BOOL RemoveGuild(AgpdGuild* pcsGuild);
	AgpdGuild* GetGuild(CHAR* szGuildID);
	inline AgpdGuild* GetGuildLock(CHAR* szGuildID);
	AgpdGuild* GetGuildLock(CHAR* szGuildID, AuAutoLock& pLock);
	INT32 GetGuildCount();
	AgpdGuild* GetGuildSequence(INT32* plIndex);

	// Guild Data 관리
	AgpdGuild* CreateGuild(CHAR* szGuildID, INT32 lGuildTID, INT32 lGuildRank, INT32 lMaxMemberCount, INT32 lUnionID);		// 새 길드 생성
	BOOL CheckCreateGuildEnable(CHAR* szGuildID, CHAR* szMasterID);
	BOOL DestroyGuild(CHAR* szGuildID);							// 길드	해체
	BOOL DestroyGuild(AgpdGuild* pcsGuild);						// 길드 해체
	BOOL DestroyGuildFromRemoveAdmin(AgpdGuild* pcsGuild);		// 길드 메모리 해제
	AgpdGuildMember* GetMember(AgpdGuild* pcsGuild, CHAR* szMemberID);
	AgpdGuildMember* GetMaster(AgpdGuild* pcsGuild);
	inline INT32 GetMemberCount(AgpdGuild* pcsGuild);
	INT32 GetAllMemberCount(AgpdGuild* pcsGuild);
	INT32 GetOnlineMemberCount(AgpdGuild* pcsGuild);
	BOOL SetMaster(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMaster);
	BOOL IsMaster(CHAR* szGuildID, CHAR* szMasterID);
	BOOL IsMaster(AgpdGuild* pcsGuild, CHAR* szMasterID);
	BOOL IsSubMaster( CHAR* szGuildID , CHAR* szSubMasterID );
	BOOL IsSubMaster( AgpdGuild* pcsGuild , CHAR* szSubMasterID );
	INT32 GetMemberLevelSum(AgpdGuild* pcsGuild);
	AgpdGuild* GetGuild(AgpdCharacter *pcsCharacter);
	AgpdGuild* GetGuildLock(AgpdCharacter *pcsCharacter);

	BOOL IsExceptionCharacterOfGuildRankSystem(AgpdGuild* pcsGuild, AgpdCharacter* pcsCharacter);

	// Member Data 관리
	AgpdGuildMember* CreateGuildMember(CHAR* szCharID);
	BOOL AddMember(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	BOOL JoinMember(CHAR* szGuildID, CHAR* szCharID, INT32 lMemberRank, INT32 lJoinDate, INT32 lLevel, INT32 lTID, INT8 cStatus);
	BOOL JoinMember(AgpdGuild* pcsGuild, CHAR* szCharID, INT32 lMemberRank, INT32 lJoinDate, INT32 lLevel, INT32 lTID, INT8 cStatus);
	BOOL CheckJoinEnable(CHAR* szGuildID, CHAR* szCharID, BOOL bSelfRequest);
	BOOL RemoveMember(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	BOOL LeaveMember(CHAR* szGuildID, CHAR* szCharID, BOOL bNotifyMaster, BOOL bAdmin = FALSE);
	BOOL ForcedLeaveMember(CHAR* szGuildID, CHAR* szForceID, CHAR* szCharID);	// ForceID 가 CharID 를 짜름

	inline BOOL UpdateMemberStatus(CHAR* szGuildID, CHAR* szMemberID, INT32 lMemberRank, INT32 lJoinDate, INT32 lLevel, INT32 lTID, INT8 cStatus);
	inline BOOL UpdateMemberStatus(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember, INT32 lMemberRank, INT32 lJoinDate, INT32 lLevel, INT32 lTID, INT8 cStatus);
	inline BOOL UpdateMemberStatus(AgpdCharacter* pcsCharacter, CHAR* szGuildID, AgpdGuildMember* pcsMember);

	INT32 GetMemberCIDList(AgpdCharacter* pcsCharacter, list<INT32>* pList);
	INT32 GetMemberCIDList(CHAR* szGuildID, list<INT32>* pList);
	INT32 GetMemberCIDList(AgpdGuild* pcsGuild, list<INT32>* pList);

	BOOL IsAnyGuildMember(AgpdCharacter* pcsCharacter);
	CHAR* GetJoinedGuildID(AgpdCharacter* pcsCharacter);

	// Character Attached Data
	inline BOOL SetCharAD(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL IsWinner, AgpdGuildMember* pcsMember);

	// Find Character Guild ID
	BOOL FindCharGuildID(AgpdCharacter* pcsCharacter);
	AgpdGuild* FindGuildLockFromCharID(CHAR* szCharID);

	// Battle
	BOOL IsBattleDeclareStatus(AgpdGuild* pcsGuild);
	BOOL IsBattleStatus(AgpdGuild* pcsGuild);
	BOOL IsBattleReadyStatus(AgpdGuild* pcsGuild);
	BOOL IsBattleIngStatus(AgpdGuild* pcsGuild);
	BOOL CheckBattleType(AgpdGuild* pGuild, AgpdGuild* pEnemyGuild, eGuildBattleType eType );
	BOOL IsEnemyGuild(CHAR* szGuildID, CHAR* szEnemyGuildID, BOOL bBattleIng = FALSE);
	BOOL IsEnemyGuild(AgpdGuild* pcsGuild, CHAR* szEnemyGuildID, BOOL bBattleIng = FALSE);
	BOOL InitCurrentBattleInfo(AgpdGuild* pcsGuild);

	// Battle Point
	INT32 GetMinimumWinPoint(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild, UINT32 ulDuration);
	INT32 GetBattlePoint(INT32 lLevelGap);
	BOOL SetMaxBattlePointAdmin(INT32 lCount);
	BOOL ReadBattlePointTxt(CHAR* szFile, BOOL bDecryption);

	// Relation Guild
	BOOL AddJointGuild(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate, INT8 cRelation);
	BOOL IsJointGuild(CHAR* szGuildID, CHAR* szJointGuildID);
	BOOL IsJointGuild(AgpdGuild* pcsGuild, CHAR* szGuildID);
	BOOL IsJointLeader(AgpdGuild* pcsGuild, CHAR* szGuildID = NULL);
	BOOL RemoveJointGuild(AgpdGuild* pcsGuild, CHAR* szGuildID);
	BOOL ClearJointGuild(AgpdGuild* pcsGuild);
	AgpdGuildRelationUnit* GetJointGuild(AgpdGuild* pcsGuild, CHAR* szGuildID);
	INT32 GetJointGuildCount(AgpdGuild* pcsGuild);

	BOOL AddHostileGuild(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate);
	BOOL IsHostileGuild(CHAR* szGuildID, CHAR* szHostileGuildID);
	BOOL IsHostileGuild(AgpdGuild* pcsGuild, CHAR* szGuildID);
	BOOL IsHostileGuild(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL RemoveHostileGuild(AgpdGuild* pcsGuild, CHAR* szGuildID);
	AgpdGuildRelationUnit* GetHostileGuild(AgpdGuild* pcsGuild, CHAR* szGuildID);
	INT32 GetHostileGuildCount(AgpdGuild* pcsGuild);

	void				SetLastRelationWaitGuildID(AgpdGuild* pcsGuild, CHAR* szGuildID = NULL);
	inline const CHAR*	GetLastRelationWaitGuildID(AgpdGuild* pcsGuild);
	BOOL				IsLastRelationWaitGuildID(AgpdGuild* pcsGuild, CHAR* szGuildID);


	//////////////////////////////////////////////////////////////////////////
	// Etc
	UINT32 GetCurrentTimeStamp();
	UINT32 GetCurrentTimeDate(CHAR* szTimeBuf);
	BOOL WriteGuildLog(CHAR* szMessage);
	
	BOOL IsSameGuild(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	
	//////////////////////////////////////////////////////////////////////////
	// Packet
	PVOID MakeGuildPacket(BOOL bPacket, INT16* pnPacketLength, UINT8 cType, INT8* pcOperation, INT32* plCID,
						CHAR* szGuildID, CHAR* szMasterID, INT32* plGuildTID, INT32* plGuildRank, INT32* plCreationDate, INT32* plMaxMemberCount,
						INT32* plUnionID, CHAR* szPassword, CHAR* szNotice, INT16* pnNoticeLength, 
						INT8* pcStatus, INT32* plWin, INT32* plDraw, INT32* plLose, INT32* pGuildPoint,
						PVOID pvMemberPacket, PVOID pvBattlePacket, PVOID pvBattlePersonPacket, PVOID pvBattleMemberPacket, PVOID pvBattleMemberListPacket, PVOID pvGuildListPacket,
						INT32* plGuildMarkTID, INT32 *plGuildMarkColor, BOOL *pIsWinner);

	PVOID MakeGuildCreatePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, INT32* plGuildTID,
						INT32* plGuildRank, INT32* plCreationDate, INT32* plMaxMemberCount, INT32* plUnionID, CHAR* szPassword,
						CHAR* szNotice, INT16* pnNoticeLength, 
						INT8* pcStatus, INT32* plWin, INT32* plDraw, INT32* plLose, INT32* pGuildPoint,
						INT32 *plGuildMarkTID, INT32 *plGuildMarkColor, BOOL *pIsWinner);

	PVOID MakeGuildJoinRequestPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID);
	PVOID MakeGuildJoinRejectPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID);
	PVOID MakeGuildJoinPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMemberID, INT32* plMemberRank,
						INT32* plJoinDate, INT32* plLevel, INT32* plTID, INT8* pcStatus);
	PVOID MakeGuildMemberPacket(INT16* pnPacketLength, INT8 *pOperation, INT32* plCID, CHAR* szGuildID, CHAR* szMemberID, INT32 *plMemberRank, INT32* plJoinDate, INT32* plLevel, INT32* plTID, INT8* pcStatus);		//	2005.08.01. By SungHoon

	PVOID MakeGuildLeavePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID);
	PVOID MakeGuildLeaveAllowPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID);
	PVOID MakeGuildForcedLeavePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID);
	PVOID MakeGuildDestroyPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szPassword);
	PVOID MakeGuildUpdateMaxMemberCountPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, INT32* plNewMaxMemberCount);
	PVOID MakeGuildCharDataPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID, INT32* plRank,
						INT32* plJoinDate, INT32* plLevel, INT32* plTID, INT8* pcStatus,INT32* plGuildMarkTID, INT32* plGuildMarkColor, BOOL* pIsWinner);
	PVOID MakeGuildUpdateNoticePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID,
						CHAR* szNotice, INT16* pnNoticeLength);
	PVOID MakeGuildSystemMessagePacket(INT16* pnPacketLength, INT32* plCode, CHAR* szData1, CHAR* szData2, INT32* plData1, INT32* plData2);

	PVOID MakeGuildBattlePacket(PACKET_GUILD_BATTLE& pPacket);
	PVOID MakeGuildBattlePersonPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, INT16* pPersonCount, CHAR* pPersonList, INT16* pSize );
	PVOID MakeGuildBattleRequestPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, INT32* pType, UINT32* pulDuration, UINT32* pPerson);
	PVOID MakeGuildBattleAcceptPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, UINT32* pulAcceptTime, UINT32* pulReadyTime, UINT32* pulStartTime, 
										INT32* pType, UINT32* pulDuration, UINT32* pPerson);
	PVOID MakeGuildBattleRejectPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	PVOID MakeGuildBattleCancelPacket(INT16* pnPacketLength, INT32* plCID, INT8* pcOperation, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	PVOID MakeGuildBattleCancelReqeustPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	PVOID MakeGuildBattleCancelAcceptPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	PVOID MakeGuildBattleCancelRejectPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	PVOID MakeGuildBattleStartPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szEnemyGuildID, CHAR* szEnemyGuildMasterID, UINT32* pulStartTime, UINT32* pulDuration, UINT32* pulCurrentTime);
	PVOID MakeGuildBattleUpdateTimePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szEnemyGuildID, UINT32* pulAcceptTime, UINT32* pulReadyTime, UINT32* pulStartTime, UINT32* pulDuration, UINT32* pulCurrentTime);
	PVOID MakeGuildBattleUpdateScorePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szEnemyGuildID, INT32* plMyScore, INT32* plEnemyScore, INT32* plMyUpScore, INT32* plEnemyUpScore);
	PVOID MakeGuildBattleWithdrawPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	
	PVOID MakeGuildBattleMemberPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID, UINT32 ulScore, UINT32 ulKill, UINT32 ulDeath);
	PVOID MakeGuildBattleMemberListPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szEnemyGuildID);

	PVOID MakeGuildListPacket(INT32 *pCID, INT32 lPage, INT16 *pnPacketLength , CHAR *szGuildID );		//	2005.07.08. By SungHoon
	PVOID MakeGuildListPacket(INT16 *pnPacketLength, AgpdGuild *pcsGuild );		//	2005.07.08. By SungHoon

	PVOID MakeGuildListPacket(INT32 *pCID, CHAR *szGuildID, INT16 *pnPacketLength );	//	2005.07.13. By SungHoon

	PVOID MakeGuildListRequestPacket(INT32 *pCID, INT32 lPage, INT16 *pnPacketLength );		//	2005.07.08. By SungHoon
	PVOID MakeGuildListRequestFindPacket(INT32 *pCID, CHAR *szGuildID, INT16 *pnPacketLength );	//	2005.07.15. By SungHoon
	PVOID MakeGuildRenameGuildIDPacket(INT32 *pCID, CHAR *szGuildID, CHAR *szMemberID, INT16 *pnPacketLength );	//	2005.08.24. By SungHoon
	PVOID MakeGuildRenameCharacterIDPacket(CHAR *szGuildID, CHAR *szOldMemberID, CHAR *szNewMemberID, INT16 *pnPacketLength );		//	2005.09.05. By SungHoon
	PVOID MakeGuildBuyGuildMarkPacket(INT16* pnPacketLength, INT32 lCID, CHAR *szGuildID, INT32 *plGuildMarkTID, INT32 *plGuildMarkColor);	//	2005.10.19. By SungHoon
	PVOID MakeGuildBuyGuildMarkForcePacket(INT16* pnPacketLength, INT32 lCID, CHAR *szGuildID, INT32 *plGuildMarkTID, INT32 *plGuildMarkColor);	//	2005.10.19. By SungHoon

	PVOID MakeGuildJointRequestPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szMasterID);
	PVOID MakeGuildJointRejectPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szJointGuildID);
	PVOID MakeGuildJointPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szJointGuildID, UINT32 ulDate, INT8 cRelation);
	PVOID MakeGuildJointLeavePacket(INT16* pnPacketLength, INT32 lCID, CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szNewLeader);

	PVOID MakeGuildHostileRequestPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID);
	PVOID MakeGuildHostileRejectPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID);
	PVOID MakeGuildHostilePacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID, UINT32 ulDate);
	PVOID MakeGuildHostileLeaveRequestPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID);
	PVOID MakeGuildHostileLeaveRejectPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID);
	PVOID MakeGuildHostileLeavePacket(INT16* pnPacketLength, INT32 lCID, CHAR* szGuildID, CHAR* szHostileGuildID);

	//////////////////////////////////////////////////////////////////////////
	// Callback
	BOOL SetCallbackGuildCreate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildCreateCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildCreateEnableCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildCreateFail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildJoinRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildJoinEnableCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildJoinFail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildJoinReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildJoin(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildLeaveFail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildForcedLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildForcedLeaveFail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildDestroyPreProcess(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildDestroy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildDestroyFail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildCheckDestroy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildUpdateMaxMemberCount(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCheckPassword(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCharData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateMemberStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateNotice(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSystemMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackUpdateStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateRecord(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackBattleInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattlePerson(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleAccept(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleCancelRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleCancelAccept(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleCancelReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleStart(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleUpdateTime(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleUpdateScore(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleWithdraw(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleMember(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleMemberList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBattleRound(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackReceiveGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//	2005.07.08. By SungHoon
	BOOL SetCallbackReceiveLeaveRequestSelf(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//	2005.07.28. By SungHoon
	BOOL SetCallbackReceiveJoinRequestSelf(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//	2005.08.02. By SungHoon

	BOOL SetCallbackDestroyGuildCheckTime(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//	2005.08.17. By SungHoon
	BOOL SetCallbackRenameGuildID(ApModuleDefaultCallBack pfCallback, PVOID pClass);			//	2005.08.24. By SungHoon
	BOOL SetCallbackRenameCharID(ApModuleDefaultCallBack pfCallback, PVOID pClass);			//	2005.09.05. By SungHoon
	BOOL SetCallbackGuildMarkTemplateLoad(ApModuleDefaultCallBack pfCallback, PVOID pClass);			//	2005.10.07. By SungHoon
	BOOL SetCallbackBuyGuildMark(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//	2005.10.19. By SungHoon
	BOOL SetCallbackBuyGuildMarkForce(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//	2005.10.20. By SungHoon
	
	BOOL SetCallbackGetGuildMarkTexture(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//	2005.10.19. By SungHoon
	BOOL SetCallbackGetGuildMarkSmallTexture(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//	2005.10.19. By SungHoon

	BOOL SetCallbackJointRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackJointReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackJoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackJointLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackHostileRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackHostileReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackHostile(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackHostileLeaveRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackHostileLeaveReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackHostileLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackJointDetail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackHostileDetail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGetJointMark(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGetJointColor(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackGuildAppointmentAsk		( ApModuleDefaultCallBack pfCallback , PVOID pClass );	// 부길마임명
	BOOL SetCallbackGuildSuccessionAsk		( ApModuleDefaultCallBack pfCallback , PVOID pClass );	// 길드승계 
	BOOL SetCallbackGuildAppointmentSysMsg	( ApModuleDefaultCallBack pfCallback , PVOID pClass );	// 부길마 임명 메세지 
	BOOL SetCallbackGuildMemberRankUpdate	( ApModuleDefaultCallBack pfCallback , PVOID pClass );	// 길드 멤버 랭크 업데이트


	AgpdRequireItemIncreaseMaxMember *GetRequireIncreaseMaxMember(INT32 lMaxMember);				//	2005.07.08. By SungHoon
	BOOL ReadRequireItemIncreaseMaxMember( CHAR *szFileName, BOOL bEncryption);
	BOOL OnOperationGuildList(INT32 lCID, PVOID pvGuildListPacket, CHAR *szGuildID);			//	2005.07.08. By SungHoon
	
	//////////////////////////////////////////////////////////////////////////
	// Debug
	BOOL PrintGuildInfo();

	BOOL RenameGuildID(AgpdGuild *pcsGuild, CHAR *szGuildID);
	BOOL RenameCharID(AgpdGuild *pcsGuild, CHAR *szOldID, CHAR *szNewID);
	BOOL IsDuplicatedCharacterOfMigration(CHAR *pszChar);

	static BOOL	CBChangeItemOwner(PVOID pData, PVOID pClass, PVOID pCustData);

protected:
	AgpdRequireItemIncreaseMaxMember m_stRequireIncreaseMaxMember[AGPMGUILD_MAX_AGPD_REQUIRE_MAX_MEMBER_INCREASE];		//	2005.07.08. By SungHoon

public:
	AgpdGuildMarkTemplate* CreateGuildMarkTemplate();
	VOID DestroyGuildMarkTemplate(AgpdGuildMarkTemplate *pGuildMarkTemplate);

	AgpdGuildMarkTemplate* AddGuildMarkTemplate(CHAR *szName, INT32 lTypeCode, INT32 lTID, INT32 lIndex, INT32 lGheld, INT32 lSkullTID, INT32 lSkullCount);

	AgpdGuildMarkTemplate *GetGuildMarkTemplate(INT32 lGuildTemplateID);
	BOOL ReadGuildMarkTemplate( CHAR *szFileName, BOOL bEncryption);
	BOOL ReReadGuildMarkTemplate( CHAR *szFileName, BOOL bEncryption);

	VOID DeleteAllGuildMark();

	INT16 AttachGuildMarkTemplateData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	INT32 GetMaxGuildMarkGridItem();
	INT32 GetMaxGuildMarkColorGridItem();

	INT32 SetGuildMarkID(INT32 lTypeCode, INT32 lID, INT32 lSettingID);
	INT32 GetGuildMarkID(INT32 lTypeCode, INT32 lID);

	INT32 SetGuildMarkColor(INT32 lTypeCode, INT32 lID, INT32 lSettingID);
	INT32 GetGuildMarkColor(INT32 lTypeCode, INT32 lID);
	INT32 GetGuildMarkColorID(INT32 lTypeCode, INT32 lID);

	AgpdGuildMarkTemplate* GetGuildMarkTemplate(INT32 lTypeCode, INT32 lGuildMarkTemplateID);

	VOID *GetGuildMarkTexture(INT32 lTypeCode, INT32 lGuildMarkTemplateID, BOOL bSmallTexture );
	VOID *GetGuildMarkTexture(INT32 lTypeCode, AgpdCharacter *pcsCharacter, BOOL bSmallTexture );
	AgpdGuildMarkTemplate* GetGuildMarkTemplate(INT32 lTypeCode, AgpdCharacter *pcsCharacter);
	INT32 GetGuildMarkColor(INT32 lTypeCode, AgpdCharacter *pcsCharacter);

	INT32 GetGuildMarkID(AgpdCharacter *pcsCharacter);
	INT32 GetGuildMarkColor(AgpdCharacter *pcsCharacter);

	INT32 GetJointGuildMarkID(CHAR* szGuildID);
	INT32 GetJointGuildMarkColor(CHAR* szGuildID);

	BOOL IsEnableBuyGuildMark(INT32 lGuildMarkTID, AgpdCharacter *pcsCharacter);
	BOOL IsDuplicateGuildMark(INT32 lGuildMarkTID);

	void OnOperationWorldChampionship(PVOID pvPacket);
	void OnOperationClassSocietySystem(PVOID pvPacket);

public:
	ApAdmin	m_csGuildMarkTemplateList;

private:
	INT32	m_lGuildMarkBottomCount;
	INT32	m_lGuildMarkPatternCount;
	INT32	m_lGuildMarkSymbolCount;
	INT32	m_lGuildMarkColorCount;
};

#endif //_AGPMGUILD_H_
