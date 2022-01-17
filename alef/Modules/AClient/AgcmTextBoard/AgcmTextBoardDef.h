#pragma once

#include "ApMemory.h"
#include "ApAutoLockCharacter.h"

#include "AgcModule.h"
#include "AgcmMap.h"
#include "AgcmRender.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgcmFont.h"
#include "ApmMap.h"

#include "AgpmItem.h"
#include "AgcmItem.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include "ApmEventManager.h"
#include "AgpmEventTeleport.h"
#include "AgcmSkill.h"

#include "AgpmGuild.h"
#include "AgpmParty.h"
#include "AgpmPvP.h"
#include "AgpmBillInfo.h"
#include "AgcmGuild.h"
#include "AgcmSiegeWar.h"
#include "AgpmBattleGround.h"
#include "AgcmUIControl.h"
#include "AgcmUIConsole.h"

#include <list>
#include <vector>
#include <algorithm>
#include <string>

// 2D Pixel Offset
#define		TB_HP_OFFSET				26
#define		TB_ID_HEIGHT_OFFSET			20
#define		TB_TEXT_HEIGHT_OFFSET		30
#define		TB_IMOTICON_HEIGHT_OFFSET	35

#define		TB_IMAGE_COUNT				9
#define		TB_IMAGE_TAIL_COUNT			3

#define		TB_ID_MAX_CHAR				50
#define		TB_TEXT_MAX_CHAR			50
#define		TB_TEXT_MAX_CHARLINE		3

#define		TB_DAMAGE_MAX_NUM			8

#define		TB_INI_NAME_TEXTURES		"Textures"
#define		TB_INI_NAME_FILE_NAME		"FN"

#define		AGCMTEXTBOARD_MAX_BOARD		1000

#define		TBID_TEXTURE_ADDINTIONAL_SIZE	5
#define		CHARISMA_TYPE_MAX				13

#define		TBID_TAG_SIZE			13
#define		TB_ID_DRAW_SIZE			12
#define		TB_IMOTICON_DRAW_SIZE	8

#define		DAMAGE_HEIGHT		38.4f	// 32.0 * 1.2
#define		DAMAGE_SMALL_HEIGHT	31.2f	// 26 * 1.2

#define		AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_SPEED			(0.00008f)
#define		AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_ACCELERATION	(0.0005f)

#ifndef		RW_TEXTURE_DESTROY
#define     RW_TEXTURE_DESTROY(p) {	if(p)	{	RwTextureDestroy(p); (p) = NULL;		}	}
#endif

#ifndef		SAFE_DELETE
#define     SAFE_DELETE(p)        { if(p)   {   delete p; p=NULL;                       }   }
#endif

// type
enum eBoardType
{
	AGCM_BOARD_TEXT						,

	AGCM_BOARD_IDBOARD					,
	AGCM_BOARD_ITEMID					,

	AGCM_BOARD_DAMAGE					,
	AGCM_BOARD_CRITICALDAMAGE			,

	AGCM_BOARD_HPBAR                    ,
	AGCM_BOARD_HPMPBAR                  ,

	AGCM_BOARD_IMOTICON					,

	AGCM_BOARD_COUNT                    ,
};

enum eImoticonType
{
	IMOTICON_MISS		 = 530  ,
	IMOTICON_CRITICAL			,
	IMOTICON_BLOCK				,
	IMOTICON_LEVELUP            ,
	IMOTICON_EVADE				,
	IMOTICON_DODGE				,
	IMOTICON_REFLEX				,
	IMOTICON_ATTACK             ,
	IMOTICON_CHARISMA1			,
	IMOTICON_CHARISMA2          ,
	IMOTICON_CHARISMA3          ,

	IMOTICON_COUNT              ,
};

enum	TEXTBOARD_IDTYPE
{
	TB_MAINCHARACTER	= 0x01,
	TB_MONSTER			= 0x02,
	TB_NPC				= 0x04,
	TB_OTHERS			= 0x08
};

enum	TEXTBOARD_IDTAG
{
	TB_TAG_PCBANG	= 0x0000,
	TB_TAG_PK,
	TB_TAG_FIRSTATTACKER,
	TB_TAG_MURDER1,
	TB_TAG_MURDER2,
	TB_TAG_MURDER3,
	TB_TAG_EVENTSTATUS_EXPEDITION,
	TB_MAX_TAG
};

enum	TEXTBOARD_IDFLAG
{
	TB_FLAG_NONE			= 0x0000,
	TB_FLAG_PCBANG			= (1 << TB_TAG_PCBANG),
	TB_FLAG_PK				= (1 << TB_TAG_PK),
	TB_FLAG_FIRSTATTACKER	= (1 << TB_TAG_FIRSTATTACKER),
	TB_FLAG_MURDER1			= (1 << TB_TAG_MURDER1),
	TB_FLAG_MURDER2			= (1 << TB_TAG_MURDER2),
	TB_FLAG_MURDER3			= (1 << TB_TAG_MURDER3),
	TB_FLAG_EVENTSTATUS_EXPEDITION	= (1 << TB_TAG_EVENTSTATUS_EXPEDITION),
};

enum eBattleGroundNameColor
{
	BATTLEGROUND_HUMAN_NAME_COLOR		,
	BATTLEGROUND_MOONELF_NAME_COLOR		,
	BATTLEGROUND_ORC_NAME_COLOR			,
	BATTLEGROUND_DRAGONSCION_NAME_COLOR	,

	BATTLEGROUND_COLOR_COUNT
};