// AgsmRelay.cpp: implementation of the AgsmRelay class.
//
//////////////////////////////////////////////////////////////////////
/*	캐릭터 table 구조
	CREATE TABLE alef_character

-	id				NUMBER NOT NULL,		-- table id(sequence)
-	name			VARCHAR2(32),			-- character name
-	template_id		VARCHAR2(32) NOT NULL,	-- template name 
-	account_id		VARCHAR2(32) NOT NULL,	-- account id
	title			VARCHAR2(32),			-- title 
	pos_x			NUMBER(20, 10),			-- position 
	pos_y			NUMBER(20, 10),
	pos_z			NUMBER(20, 10),
	criminal_status	NUMBER(3),				-- criminal status
	murder_point	NUMBER(5),				-- murderer point
	inven_money		NUMBER(20),				-- inventory money
	bank_money		NUMBER(20),				-- bank money
	rcf_time		NUMBER,					-- remained criminal flag time
	rmf_time		NUMBER,					-- remained murder flag time
	hp				NUMBER(10),
	mp				NUMBER(10),
	sp				NUMBER(10),
	exp				NUMBER(10),
	lv				NUMBER(3),				-- level
-	server_group	VARCHAR2(64),			-- server group
	CONSTRAINT alef_char_name_unique UNIQUE (name) 		-- CONSTRAINT alef_char_lv_check CHECK lv BETWEEN 1 AND 99
*/

/* 아이템 table 구조
CREATE TABLE alef_item
	(
	id			NUMBER NOT NULL,
	owner_id	VARCHAR2(32) NOT NULL,	-- owner(character)
-	template_id	VARCHAR2(32) NOT NULL,	-- template
	stack_count	NUMBER(10),				-- stack count
	status		NUMBER(5),				-- status
	pos_x		NUMBER(20, 10),			-- position
	pos_y		NUMBER(20, 10),
	pos_z		NUMBER(20, 10),
	grid_pos_t	NUMBER(5),				-- grid position	tab
	grid_pos_r	NUMBER(5),				--			row
	grid_pos_c	NUMBER(5),				--			column
	need_level	NUMBER(3),
	own_time	DATE DEFAULT (SYSDATE),	-- own time
	CONSTRAINT alef_item_id_unique UNIQUE (id)
	);
*/	

/* 스킬 table 구조
CREATE TABLE alef_skill_mast
	(
	owner_id	VARCHAR(32) NOT NULL,	-- owner(character) id
	mastery		NUMBER(3) NOT NULL,	-- mastery index
	total_sp	NUMBER(3) NOT NULL,	-- total skill point
	skill_seq	VARCHAR2(256),		-- skill seq. tree node
	inst_spec	NUMBER(3),		-- instant specialization
	inst_spec_time	NUMBER,			-- remained instant specialization Time
	const_spec	NUMBER(3)		-- constant specialization
	); 
*/

#include "AgsmRelay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmRelay::AgsmRelay()
{
	SetModuleName("AgsmRelay");
	SetPacketType(AGSMRELAY_PACKET_TYPE);
	
	InitPacket();
	InitCharPacket();
	InitItemPacket();
	InitItemConvertHistoryPacket();
	InitSkillPacket();
	InitUIStatusPacket();
	InitGuildMasterPacket();
	InitGuildMemberPacket();
	InitAccountWorldPacket();
	InitSearchCharacterPacket();
}

AgsmRelay::~AgsmRelay()
{

}

void AgsmRelay::InitPacket()
{
	// DB연산을 여기서 다 진행하게 된다. 그래서 추가될 Packet들을 위해 FlagLength를 sizeof(INT16)으로 설정한다.
	m_csPacket.SetFlagLength(sizeof(INT16));

	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation
							AUTYPE_INT8,			1, // Status
							AUTYPE_INT8,			1, // DB Operation
							AUTYPE_PACKET,			1, // Character Packet
							AUTYPE_PACKET,			1, // Item Packet
							AUTYPE_PACKET,			1, // Item Convert History Packet
							AUTYPE_PACKET,			1, // Skill Packet
							AUTYPE_PACKET,			1, // UI Status Packet
							AUTYPE_PACKET,			1, // Guild Master Packet
							AUTYPE_PACKET,			1, // Guild Member Packet
							AUTYPE_PACKET,			1, // Account World Packet
							AUTYPE_PACKET,			1, // Search Character Packet
							AUTYPE_END,            0);
}

void AgsmRelay::InitCharPacket()
{
	/*
	m_csCharPacket.SetFlagLength(sizeof(INT32));
	m_csCharPacket.SetFieldType(
								AUTYPE_CHAR,		AGSMACCOUNT_MAX_ACCOUNT_NAME + 1,		// Account ID
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,		// Character ID
								AUTYPE_INT32,		1,		// Template ID
								AUTYPE_CHAR,		AGPACHARACTER_MAX_CHARACTER_TITLE + 1,	// Title
								AUTYPE_POS,			1,		// Character Position
								AUTYPE_INT8,		1,		// Criminal Status
								AUTYPE_INT32,		1,		// Murderer Point
								AUTYPE_INT64,		1,		// Inventory Money
								AUTYPE_INT64,		1,		// Bank Money
								AUTYPE_UINT32,		1,		// Remained criminal flag time
								AUTYPE_UINT32,		1,		// Remained murder flag time
								AUTYPE_INT32,		1,		// hp
								AUTYPE_INT32,		1,		// mp
								AUTYPE_INT32,		1,		// sp
								AUTYPE_INT32,		1,		// exp
								AUTYPE_INT32,		1,		// lv
								AUTYPE_END,         0);
	*/

	m_csCharPacket2.SetFlagLength(sizeof(INT16));
	m_csCharPacket2.SetFieldType(
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,		// Character ID
								AUTYPE_CHAR,		AGPACHARACTER_MAX_CHARACTER_TITLE + 1,	// Title
								AUTYPE_CHAR,		33,		// Character Position
								AUTYPE_INT8,		1,		// Criminal Status
								AUTYPE_INT32,		1,		// Murderer Point
								AUTYPE_INT64,		1,		// Inventory Money
								//AUTYPE_INT64,		1,		// Bank Money
								AUTYPE_UINT32,		1,		// Remained criminal flag time
								AUTYPE_UINT32,		1,		// Remained murder flag time
								AUTYPE_INT32,		1,		// hp
								AUTYPE_INT32,		1,		// mp
								AUTYPE_INT32,		1,		// sp
								AUTYPE_INT32,		1,		// exp
								AUTYPE_INT32,		1,		// lv
								AUTYPE_INT32,		1,		// skill point
								AUTYPE_END,         0);
}

void AgsmRelay::InitItemPacket()
{
	/*
	m_csItemPacket.SetFlagLength(sizeof(INT16));
	m_csItemPacket.SetFieldType(
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// Owner ID
								AUTYPE_INT32,		1,									// Item template ID
								AUTYPE_INT32,		1,									// Stack count
								AUTYPE_INT16,		1,									// Status
								AUTYPE_POS,			1,									// Item position
								AUTYPE_INT16,		1,									// Grid tab position
								AUTYPE_INT16,		1,									// Grid row position
								AUTYPE_INT16,		1,									// Grid column position
								AUTYPE_INT32,		1,									// Need level
								AUTYPE_UINT64,		1,									// DB ID
								AUTYPE_END,         0);
	*/

	m_csItemPacket2.SetFlagLength(sizeof(INT16));
	m_csItemPacket2.SetFieldType(
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// Owner ID
								AUTYPE_INT32,		1,									// Item template ID
								AUTYPE_INT32,		1,									// Stack count
								AUTYPE_INT16,		1,									// Status
								AUTYPE_CHAR,		33,									// encoded position (grid or field)
								AUTYPE_INT32,		1,									// Need level
								AUTYPE_INT32,		1,									// Durability
								AUTYPE_MEMORY_BLOCK,		1,							// Deleted Reason
								AUTYPE_UINT64,		1,									// DB ID
								AUTYPE_END,         0);
}

void AgsmRelay::InitItemConvertHistoryPacket()
{
	/*
	m_csItemConvertHistoryPacket.SetFlagLength(sizeof(INT8));
	m_csItemConvertHistoryPacket.SetFieldType(
								AUTYPE_UINT64,		1,									// DBID
								AUTYPE_INT8,		1,									// type (rune or spirit stone)
								AUTYPE_INT32,		1,									// attribute
								AUTYPE_INT32,		1,									// value
								AUTYPE_END,			0);
	*/

	m_csItemConvertHistoryPacket2.SetFlagLength(sizeof(INT8));
	m_csItemConvertHistoryPacket2.SetFieldType(
								AUTYPE_UINT64,			1,								// DBID
								AUTYPE_MEMORY_BLOCK,	1,								// encoded all history list
								AUTYPE_END,				0);
}

void AgsmRelay::InitSkillPacket()
{
	/*
	m_csSkillPacket.SetFlagLength(sizeof(INT8));
	m_csSkillPacket.SetFieldType(
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// Owner ID
								AUTYPE_INT32,		1,									// Mastery index
								AUTYPE_INT32,		1,									// Total skill point
								AUTYPE_CHAR,		128 + 1,							// Skill tree node
								AUTYPE_INT8,		1,									// Instant specialization
								AUTYPE_UINT32,		1,									// Remained instant specialization time
								AUTYPE_INT8,		1,									// Const specialization
								AUTYPE_END,			0);
	*/

	m_csSkillPacket2.SetFlagLength(sizeof(INT8));
	m_csSkillPacket2.SetFieldType(
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// Owner ID
//								AUTYPE_INT32,		1,									// Mastery index
//								AUTYPE_INT32,		1,									// Total skill point
								AUTYPE_CHAR,		128 + 1,							// Skill tree node
								AUTYPE_CHAR,		32 + 1,								// Skill Specialize Data
								AUTYPE_END,			0);
}

void AgsmRelay::InitUIStatusPacket()
{
	m_csUIStatusPacket2.SetFlagLength(sizeof(INT8));
	m_csUIStatusPacket2.SetFieldType(
								AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Owner ID
								AUTYPE_MEMORY_BLOCK,	1,									// encoded QBelt string
								AUTYPE_END,				0);
}

void AgsmRelay::InitGuildMasterPacket()
{
	m_csGuildMasterPacket2.SetFlagLength(sizeof(INT8));
	m_csGuildMasterPacket2.SetFieldType(
								AUTYPE_CHAR,		AGPMGUILD_MAX_GUILD_ID_LENGTH + 1,	// GuildID
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// Master ID
								AUTYPE_INT32,		1,	// Guild TID
								AUTYPE_INT32,		1,	// Guild Rank
								AUTYPE_INT32,		1,	// Creation Date
								AUTYPE_INT32,		1,	// Max Member Count
								AUTYPE_INT32,		1,	// Union ID
								AUTYPE_CHAR,		AGPMGUILD_MAX_PASSWORD_LENGTH + 1,	// Password
								AUTYPE_END,			0);
}

void AgsmRelay::InitGuildMemberPacket()
{
	m_csGuildMemberPacket2.SetFlagLength(sizeof(INT8));
	m_csGuildMemberPacket2.SetFieldType(
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// CharID
								AUTYPE_CHAR,		AGPMGUILD_MAX_GUILD_ID_LENGTH + 1,	// GuildID
								AUTYPE_INT32,		1,	// Member Rank
								AUTYPE_INT32,		1,	// Join Date
								AUTYPE_INT32,		1,	// Level
								AUTYPE_INT32,		1,	// TID
								AUTYPE_END,			0);
}

void AgsmRelay::InitAccountWorldPacket()
{
	m_csAccountWorldPacket.SetFlagLength(sizeof(INT8));
	m_csAccountWorldPacket.SetFieldType(
								AUTYPE_CHAR,		AGSMACCOUNT_MAX_ACCOUNT_NAME + 1,
								AUTYPE_INT64,		1,
								AUTYPE_END,			0);
}

void AgsmRelay::InitSearchCharacterPacket()
{
	m_csSearchCharacterPacket.SetFlagLength(sizeof(INT16));
	m_csSearchCharacterPacket.SetFieldType(
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// Admin Name
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// Char Name
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// Acc Name
								AUTYPE_INT32,		1,	// Level
								AUTYPE_INT32,		1,	// Race
								AUTYPE_INT32,		1,	// Class
								AUTYPE_INT32,		1,	// Status
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,	// Creation Date
								AUTYPE_END,			0);
}

BOOL AgsmRelay::OnAddModule()
{
	// Relay Server를 이용하는 서버에서 필요한 모듈들
	m_pAgpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pAgsmCharacter		= (AgsmCharacter*)GetModule("AgsmCharacter");

	m_pAgpmItem				= (AgpmItem*)GetModule("AgpmItem");
	m_pAgsmItem				= (AgsmItem*)GetModule("AgsmItem");
	m_pAgsmItemConvert		= (AgsmItemConvert*)GetModule("AgsmItemConvert");

	m_pAgsmServerManager	= (AgsmServerManager*)GetModule("AgsmServerManager2");
	m_pAgpmFactors			= (AgpmFactors*)GetModule("AgpmFactors");

	m_pAgpmSkill			= (AgpmSkill*)GetModule("AgpmSkill");
	m_pAgsmSkill			= (AgsmSkill*)GetModule("AgsmSkill");
	m_pAgsmEventSkillMaster	= (AgsmEventSkillMaster*)GetModule("AgsmEventSkillMaster");

	m_pAgsmUIStatus			= (AgsmUIStatus *) GetModule("AgsmUIStatus");

	m_pAgsmGuild = (AgsmGuild*)GetModule("AgsmGuild");

	m_pAgsmAdmin = (AgsmAdmin*)GetModule("AgsmAdmin");

	if (m_pAgpmCharacter)
	{
		if (!m_pAgpmCharacter->SetCallbackMoveBankMoney(CBAccountWorldUpdate, this))
			return FALSE;
	}

	if (m_pAgsmCharacter)
	{
		if (!m_pAgsmCharacter->SetCallbackCharacterUpdate(CBCharacterUpdate, this))
			return FALSE;
	}
	
	if (m_pAgsmItem)
	{
		if (!m_pAgsmItem->SetCallbackItemInsert(CBItemInsert, this))
			return FALSE;

		if (!m_pAgsmItem->SetCallbackItemUpdate(CBItemUpdate, this))
			return FALSE;

		if (!m_pAgsmItem->SetCallbackItemDelete(CBItemDelete, this))
			return FALSE;

//		if (!m_pAgsmItem->SetCallbackItemConvertHistoryInsert(CBItemConvertHistoryInsert, this))
//			return FALSE;
//		if (!m_pAgsmItem->SetCallbackItemConvertHistoryDelete(CBItemConvertHistoryDelete, this))
//			return FALSE;
	}

	if (m_pAgsmItemConvert)
	{
		if (!m_pAgsmItemConvert->SetCallbackUpdateConvertHistoryToDB(CBItemConvertHistoryInsert, this))
			return FALSE;
	}

	if (m_pAgsmSkill)
	{
		if (!m_pAgsmSkill->SetCallbackUpdateSkillMasteryToDB(CBSkillUpdate, this))
			return FALSE;
	}

	if (m_pAgsmEventSkillMaster)
	{
		if (!m_pAgsmEventSkillMaster->SetCallbackUpdateDB(CBSkillUpdate, this))
			return FALSE;
	}

	if (m_pAgsmUIStatus)
	{
		if (!m_pAgsmUIStatus->SetCallbackUpdateDataToDB(CBUIStatusUpdate, this))
			return FALSE;
	}

	if(m_pAgsmGuild)
	{
		if(!m_pAgsmGuild->SetCallbackDBGuildInsert(CBGuildMasterInsert, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBGuildUpdate(CBGuildMasterUpdate, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBGuildDelete(CBGuildMasterDelete, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBGuildSelect(CBGuildMasterSelect, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBMemberInsert(CBGuildMemberInsert, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBMemberUpdate(CBGuildMemberUpdate, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBMemberDelete(CBGuildMemberDelete, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBMemberSelect(CBGuildMemberSelect, this))
			return FALSE;
	}

	if(m_pAgsmAdmin)
	{
		if(!m_pAgsmAdmin->SetCallbackSearchAccount(CBSearchCharacterByAccountSelect, this))
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmRelay::OnDestroy()
{
	return TRUE;
}

BOOL AgsmRelay::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	INT8	cOperation					= -1;
	INT8	cStatus						= -1;
	INT8	cDBOperation				= -1;
	PVOID	pvPacketCharacter			= NULL;
	PVOID	pvPacketItem				= NULL; 
	PVOID	pvPacketItemConvertHistory	= NULL;
	PVOID	pvPacketSkill				= NULL;
	PVOID	pvPacketUIStatus			= NULL;
	PVOID	pvPacketGuildMaster = NULL;
	PVOID	pvPacketGuildMember = NULL;
	PVOID	pvPacketAccountWorld		= NULL;
	PVOID	pvPacketSearchCharacter			= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cOperation,
						&cStatus,
						&cDBOperation,
						&pvPacketCharacter,
						&pvPacketItem,
						&pvPacketItemConvertHistory,
						&pvPacketSkill,
						&pvPacketUIStatus,
						&pvPacketGuildMaster,
						&pvPacketGuildMember,
						&pvPacketAccountWorld,
						&pvPacketSearchCharacter
						);

	switch(cOperation)
	{
	/*
	case AGSMRELAY_OPERATION_CHARACTER:
		{
			OperationCharacter(cDBOperation, pvPacketCharacter, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION_ITEM:
		{
			OperationItem(cDBOperation, pvPacketItem, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION_ITEM_CONVERT_HISTORY:
		{
			OperationItemConvertHistory(cDBOperation, pvPacketItemConvertHistory, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION_SKILL:
		{
			OperationSkill(cDBOperation, pvPacketSkill, ulNID);
		}
		break;
	*/

	case AGSMRELAY_OPERATION2_CHARACTER:
		{
			OperationCharacter2(cDBOperation, pvPacketCharacter, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION2_ITEM:
		{
			OperationItem2(cDBOperation, pvPacketItem, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION2_ITEM_CONVERT_HISTORY:
		{
			OperationItemConvertHistory2(cDBOperation, pvPacketItemConvertHistory, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION2_SKILL:
		{
			OperationSkill2(cDBOperation, pvPacketSkill, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION2_UI_STATUS:
		{
			OperationUIStatus2(cDBOperation, pvPacketUIStatus, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION2_GUILD_MASTER:
		{
			OperationGuildMaster2(cDBOperation, pvPacketGuildMaster, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION2_GUILD_MEMBER:
		{
			OperationGuildMember2(cDBOperation, pvPacketGuildMember, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION2_ACCOUNT_WORLD:
		{
			OperationAccountWorld(cDBOperation, pvPacketAccountWorld, ulNID);
		}
		break;

	case AGSMRELAY_OPERATION2_SEARCH_CHARACTER:
		{
			OperationSearchCharacter(cDBOperation, pvPacketSearchCharacter, ulNID);
		}
		break;

	default:
		{
			OutputDebugString("AgsmRelay::OnReceive() Error !!!\n");
		}
	}

	return TRUE;
}

/*
BOOL AgsmRelay::SetCallbackCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION_CHARACTER, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION_ITEM, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackItemConvertHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION_ITEM_CONVERT_HISTORY, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION_SKILL, pfCallback, pClass);
}
*/

BOOL AgsmRelay::SetCallbackCharacter2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION2_CHARACTER, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackItem2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION2_ITEM, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackItemConvertHistory2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION2_ITEM_CONVERT_HISTORY, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackSkill2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION2_SKILL, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackUIStatus2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION2_UI_STATUS, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackGuildMaster2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION2_GUILD_MASTER, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackAccountWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION2_ACCOUNT_WORLD, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackGuildMember2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION2_GUILD_MEMBER, pfCallback, pClass);
}

BOOL AgsmRelay::SetCallbackSearchCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_OPERATION2_SEARCH_CHARACTER, pfCallback, pClass);
}

PVOID AgsmRelay::MakeRelayPacket(BOOL bPacket, INT16* pnPacketLength, INT8 cOperation, INT8 cStatus, INT8 cDBOperation,
						PVOID pvPacketChar, PVOID pvPacketItem, PVOID pvPacketItemConvertHistory, PVOID pvPacketSkill, PVOID pvPacketUIStatus,
						PVOID pvPacketGuildMaster, PVOID pvPacketGuildMember, PVOID pvPacketAccountWorld,
						PVOID pvPacketSearchCharacter)
{
	return m_csPacket.MakePacket(bPacket, pnPacketLength, AGSMRELAY_PACKET_TYPE, 
								&cOperation, 
								&cStatus, 
								&cDBOperation,
								pvPacketChar,
								pvPacketItem,
								pvPacketItemConvertHistory,
								pvPacketSkill,
								pvPacketUIStatus,
								pvPacketGuildMaster,
								pvPacketGuildMember,
								pvPacketAccountWorld,
								pvPacketSearchCharacter);
}
