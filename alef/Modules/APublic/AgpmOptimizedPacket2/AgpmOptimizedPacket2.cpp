#include "AgpmOptimizedPacket2.h"
#include "AgppTitle.h"

#define AGPMOPTIMIZEDPACKET2_MAX_EVENT_PACKET		5
#define AGPMOPTIMIZEDPACKET2_BUFFER_SIZE			20480

#define _SEND_MODIFIED_FACTOR_PACKET

AgpmOptimizedPacket2::AgpmOptimizedPacket2()
{
	SetModuleName("AgpmOptimizedPacket2");

	SetPacketType(AGPMOPTIMIZEDCHARMOVE_PACKET_TYPE);
	SetPacketType(AGPMOPTIMIZEDCHARACTION_PACKET_TYPE);
	SetPacketType(AGPMOPTIMIZEDVIEW_PACKET_TYPE);

	m_pcsAgpmFactors			= NULL;
	m_pcsAgpmCharacter			= NULL;
	m_pcsAgpmGrid				= NULL;
	m_pcsAgpmItem				= NULL;
	m_pcsAgpmItemConvert		= NULL;
	m_pcsAgpmSkill				= NULL;
	m_pcsApmEventManager		= NULL;
	m_pcsAgpmGuild				= NULL;

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,										// operation
							AUTYPE_PACKET,			1,										// character packet
							AUTYPE_PACKET,			AGPMITEM_PART_NUM,						// item packet
							AUTYPE_MEMORY_BLOCK,	1,										// buffed skill packet (structure memory copy)
							AUTYPE_MEMORY_BLOCK,	1,										// event packet
							AUTYPE_CHAR,			AGPMGUILD_MAX_GUILD_ID_LENGTH + 1,		// Guild ID

							AUTYPE_MEMORY_BLOCK,	1,										// character all item packet
							AUTYPE_MEMORY_BLOCK,	1,										// character all item convert packet
							AUTYPE_MEMORY_BLOCK,	1,										// character all skill packet
							AUTYPE_INT32,			1,										// character id
							AUTYPE_INT32,			1,										// GuildMarkTID			//	2005.10.24. By SungHoon
							AUTYPE_INT32,			1,										// GuildMarkColor		//	2005.10.24. By SungHoon
							AUTYPE_INT32,			1,										// IsWinner	
							AUTYPE_MEMORY_BLOCK,	1,										// title
							AUTYPE_END,				0
							);

	m_csPacketCharView.SetFlagLength(sizeof(INT32));
	m_csPacketCharView.SetFieldType(
							AUTYPE_INT32,			1,												// character id
							AUTYPE_INT32,			1,												// character tid
							AUTYPE_MEMORY_BLOCK,	1,												// character name
							AUTYPE_INT8,			1,												// current action status
							AUTYPE_INT8,			1,												// IsNewCharacter
							AUTYPE_PACKET,			1,												// factor packet
							AUTYPE_INT8,			1,												// move flag
							AUTYPE_INT8,			1,												// move direction
							AUTYPE_POS,				1,												// character current pos
							AUTYPE_POS,				1,												// destination pos
							AUTYPE_INT32,			1,												// follow target id
							AUTYPE_UINT16,			1,												// follow distance
							AUTYPE_INT16,			1,												// turn x
							AUTYPE_INT16,			1,												// turn y
							AUTYPE_UINT64,			1,												// character special status
							AUTYPE_INT8,			1,												// is transform status, is ridable
							AUTYPE_INT8,			1,												// face index
							AUTYPE_INT8,			1,												// hair index
							AUTYPE_INT8,			1,												// view options
							AUTYPE_UINT16,			1,												// bit flag option
							AUTYPE_INT8,			1,												// character criminal status							
							AUTYPE_INT32,			1,												// wanted criminal flag
							AUTYPE_CHAR,			AGPACHARACTER_MAX_CHARACTER_NICKNAME,			// nick name
							AUTYPE_INT32,			1,												// npc 미니맵에서 보여줄지의 여부.
							AUTYPE_INT32,			1,												// npc 타겟창, 네임보드 보여줄지의 여부..
							AUTYPE_END,				0
							);

	m_csPacketItemView.SetFlagLength(sizeof(INT32));
	m_csPacketItemView.SetFieldType(
							AUTYPE_INT32,			1,				// item id
							AUTYPE_INT32,			1,				// item template id
							AUTYPE_INT16,			1,				// stack count

							// convert packet
							AUTYPE_INT8,			1,				// # of physical convert
							AUTYPE_INT8,			1,				// # of socket
							AUTYPE_INT8,			1,				// # of converted socket

							AUTYPE_INT32,			1,				// converted socket item template id 1
							AUTYPE_INT32,			1,				// converted socket item template id 2
							AUTYPE_INT32,			1,				// converted socket item template id 3
							AUTYPE_INT32,			1,				// converted socket item template id 4
							AUTYPE_INT32,			1,				// converted socket item template id 5
							AUTYPE_INT32,			1,				// converted socket item template id 6
							AUTYPE_INT32,			1,				// converted socket item template id 7
							AUTYPE_INT32,			1,				// converted socket item template id 8
							
							AUTYPE_INT32,			1,				// item status flag

							AUTYPE_UINT16,			1,				// option tid 1
							AUTYPE_UINT16,			1,				// option tid 2
							AUTYPE_UINT16,			1,				// option tid 3
							AUTYPE_UINT16,			1,				// option tid 4
							AUTYPE_UINT16,			1,				// option tid 5

							AUTYPE_END,				0
							);

	m_csPacketCharMove.SetFlagLength(sizeof(INT8));
	m_csPacketCharMove.SetFieldType(
							AUTYPE_INT32,		1,			// character id
							//AUTYPE_POS,			1,			// current position
							AUTYPE_POS,		1,			// current position
							AUTYPE_POS,			1,			// destination position
							AUTYPE_INT32,		1,			// follow target
							AUTYPE_UINT16,		1,			// follow distance
							AUTYPE_INT8,		1,			// move flag
							AUTYPE_INT8,		1,			// move direction
							AUTYPE_INT8,		1,			// next action type
							AUTYPE_INT32,		1,			// next action skill id
							AUTYPE_END,			0
							);

	m_csPacketCharAction.SetFlagLength(sizeof(INT16));
	m_csPacketCharAction.SetFieldType(
							AUTYPE_INT32,		1,			// action character id
							AUTYPE_INT32,		1,			// target character id
							AUTYPE_INT8,		1,			// attack result
							AUTYPE_PACKET,		1,			// factor packet
							AUTYPE_INT32,		1,			// target hp
							//AUTYPE_POS,			1,			// action position
							AUTYPE_INT8,		1,			// combo info
							AUTYPE_INT8,		1,			// force attack
							AUTYPE_UINT32,		1,			// Additional Effect
							AUTYPE_UINT8,		1,			// Hit Index
							AUTYPE_END,			0
							);
}

AgpmOptimizedPacket2::~AgpmOptimizedPacket2()
{
}

BOOL AgpmOptimizedPacket2::OnInit()
{
	m_pcsAgpmTitle = (AgpmTitle *) GetModule("AgpmTitle");

	return TRUE;
}

BOOL AgpmOptimizedPacket2::OnAddModule()
{
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmGrid			= (AgpmGrid *)			GetModule("AgpmGrid");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmItemConvert	= (AgpmItemConvert *)	GetModule("AgpmItemConvert");
	m_pcsAgpmSkill			= (AgpmSkill *)			GetModule("AgpmSkill");
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pcsAgpmGuild			= (AgpmGuild *)			GetModule("AgpmGuild");
	m_pcsAgpmUIStatus		= (AgpmUIStatus *)		GetModule("AgpmUIStatus");
	m_pcsAgpmBillInfo		= (AgpmBillInfo *)		GetModule("AgpmBillInfo");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmGrid ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmItemConvert ||
		!m_pcsAgpmSkill ||
		!m_pcsApmEventManager ||
		!m_pcsAgpmGuild ||
		!m_pcsAgpmUIStatus ||
		!m_pcsAgpmBillInfo)
		return FALSE;

	return TRUE;
}

BOOL AgpmOptimizedPacket2::OnDestroy()
{
	return TRUE;
}

BOOL AgpmOptimizedPacket2::SetMaxMemoryPoolCount(INT32 lMaxCount)
{
	m_csOptimizedPacketMemoryPool.Initialize(AGPMOPTIMIZEDPACKET2_BUFFER_SIZE, lMaxCount, _T("AGPMOPTIMIZEDPACKET2_BUFFER_SIZE"));

	return TRUE;
}

BOOL AgpmOptimizedPacket2::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	switch (ulType) {
	case AGPMOPTIMIZEDVIEW_PACKET_TYPE:
		return OnReceiveCharView(ulType, pvPacket, nSize, ulNID, pstCheckArg);
		break;

	case AGPMOPTIMIZEDCHARMOVE_PACKET_TYPE:
		return OnReceiveCharMove(ulType, pvPacket, nSize, ulNID, pstCheckArg);
		break;

	case AGPMOPTIMIZEDCHARACTION_PACKET_TYPE:
		return OnReceiveCharAction(ulType, pvPacket, nSize, ulNID, pstCheckArg);
		break;
	}

	return TRUE;
}

BOOL AgpmOptimizedPacket2::OnReceiveCharView(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation					= AGPM_OPTIMIZEDPACKET2_NONE;
	PVOID	pvPacketChar				= NULL;
	PVOID	pvPacketItem[AGPMITEM_PART_NUM];
	CHAR	*pszBuffedSkill				= NULL;
	CHAR	*pszEventPacket				= NULL;
	CHAR	*pszGuildName				= NULL;

	UINT16	unBuffedSkillLength			= 0;
	UINT16	unEventPacketLength			= 0;

	ZeroMemory(pvPacketItem, sizeof(PVOID) * AGPMITEM_PART_NUM);

	CHAR	*pszCharAllItem				= NULL;
	CHAR	*pszCharAllItemConvert		= NULL;
	CHAR	*pszCharAllSkill			= NULL;

	UINT16	unCharAllItemLength			= 0;
	UINT16	unCharAllItemConvertLength	= 0;
	UINT16	unCharAllSkillLength		= 0;
	UINT16	unCharTitleLength			= 0;

	INT32	lCharID						= AP_INVALID_CID;
	INT32	lGuildMarkTID				= -1;
	INT32	lGuildMarkColor				= -1;
	BOOL	IsWinner					= FALSE;

	PVOID	pvPacketTitle				= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
							&cOperation,
							&pvPacketChar,
							&pvPacketItem,
							&pszBuffedSkill, &unBuffedSkillLength,
							&pszEventPacket, &unEventPacketLength,
							&pszGuildName,
							&pszCharAllItem, &unCharAllItemLength,
							&pszCharAllItemConvert, &unCharAllItemConvertLength,
							&pszCharAllSkill, &unCharAllSkillLength,
							&lCharID,
							&lGuildMarkTID,
							&lGuildMarkColor,
							&IsWinner,
							&pvPacketTitle, &unCharTitleLength);

	switch (cOperation) {
	case AGPM_OPTIMIZEDPACKET2_ADD_CHARACTER_VIEW:
		{
			AgpdCharacter	*pcsCharacter	= ProcessCharacterPacket(pvPacketChar, pstCheckArg);
			if (!pcsCharacter || !pcsCharacter->m_Mutex.WLock())
				return FALSE;

			ProcessItemPacket(pcsCharacter, pvPacketItem, pstCheckArg);

			ProcessBuffedSkillPacket(pcsCharacter, pszBuffedSkill, unBuffedSkillLength, pstCheckArg);

			ProcessEventPacket(pcsCharacter, pszEventPacket, unEventPacketLength, pstCheckArg, ulNID);

			// 2005.01.31. steeple
			// TT........... 캐릭 길드 정보만 처리할 때는 나머지는 -1 로... TT
			m_pcsAgpmGuild->OnOperationCharacterGuildData(pcsCharacter->m_lID, pszGuildName, pcsCharacter->m_szID, -1, -1, -1, -1, -1,lGuildMarkTID, lGuildMarkColor, IsWinner);

			if(m_pcsAgpmTitle)
			{
				if(pvPacketTitle && unCharTitleLength != 0)
					m_pcsAgpmTitle->OnOperationTitleUseNear(pcsCharacter, (PACKET_AGPPTITLE*) pvPacketTitle);
			}

			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGPM_OPTIMIZEDPACKET2_ADD_CHARACTER:
		{
			ProcessCharAllItemPacket(pszCharAllItem, unCharAllItemLength, pstCheckArg, ulNID);
			ProcessCharAllItemConvertPacket(pszCharAllItemConvert, unCharAllItemConvertLength, pstCheckArg, ulNID);
			ProcessCharAllSkillPacket(pszCharAllSkill, unCharAllSkillLength, pstCheckArg, ulNID);
		}
		break;

	case AGPM_OPTIMIZEDPACKET2_RELEASE_MOVE_ACTION:
		{
			AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCharID);
			if (!pcsCharacter)
				return FALSE;

			EnumCallback(AGPM_OPTIMIZEDPACKET2_CB_MOVE_ACTION_RELEASE, pcsCharacter, NULL);

			pcsCharacter->m_Mutex.Release();
		}
		break;
	}

	return TRUE;
}

BOOL AgpmOptimizedPacket2::OnReceiveCharMove(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT32	lCID				= AP_INVALID_CID;
	AuPOS	stOriginPos;
	//AuPOSBaseMeter	stOriginMeterPos;
	AuPOS	stDestPos;
	INT32	lFollowTargetID		= AP_INVALID_CID;
	UINT16	unFollowDistance	= 0;
	INT8	cMoveFlag			= 0;
	INT8	cMoveDirection		= 0;
	INT8	cActionType			= AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_NONE;
	INT32	lSkillID			= AP_INVALID_SKILLID;

	ZeroMemory(&stOriginPos, sizeof(AuPOS));
	//ZeroMemory(&stOriginMeterPos, sizeof(AuPOSBaseMeter));
	ZeroMemory(&stDestPos, sizeof(AuPOS));

	m_csPacketCharMove.GetField(TRUE, pvPacket, nSize,
								&lCID,
								&stOriginPos,
								//&stOriginMeterPos,
								&stDestPos,
								&lFollowTargetID,
								&unFollowDistance,
								&cMoveFlag,
								&cMoveDirection,
								&cActionType,
								&lSkillID);

	AgpdCharacter	*pcsCharacter		= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacter	*pcsMoveTargetChar	= m_pcsAgpmCharacter->GetCharacterLock(lFollowTargetID);

	/*
	stOriginPos.x	= (float) stOriginMeterPos.x * 100;
	stOriginPos.y	= (float) stOriginMeterPos.y * 100;
	stOriginPos.z	= (float) stOriginMeterPos.z * 100;
	*/

	// Before Process Moving Packet Check Moving disturbing
	if(m_pcsAgpmCharacter->CheckEnableActionCharacter(pcsCharacter, AGPDCHAR_DISTURB_ACTION_MOVE) == FALSE)
	{
		return FALSE;
	}

	m_pcsAgpmCharacter->ProcessMovePacket(pcsCharacter, pcsMoveTargetChar, (INT32) unFollowDistance,
			&stOriginPos, &stDestPos, pcsCharacter->m_fTurnX, pcsCharacter->m_fTurnY, cMoveFlag, cMoveDirection, pstCheckArg->bReceivedFromServer);

	if (cActionType == AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_ATTACK)
		EnumCallback(AGPM_OPTIMIZEDPACKET2_CB_MOVE_ACTION_ATTACK, pcsCharacter, NULL);
	else if (cActionType == AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_SKILL)
		EnumCallback(AGPM_OPTIMIZEDPACKET2_CB_MOVE_ACTION_SKILL, pcsCharacter, &lSkillID);

	if (pcsMoveTargetChar)
		pcsMoveTargetChar->m_Mutex.Release();

	pcsCharacter->m_Mutex.Release();
	
	return TRUE;
}

BOOL AgpmOptimizedPacket2::OnReceiveCharAction(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT32	lCID			= AP_INVALID_CID;
	INT32	lTargetID		= AP_INVALID_CID;
	INT8	cAttackResult	= (-1);
	PVOID	pvPacketFactor	= NULL;
	INT32	lHP				= 0;
	//AuPOS	stActionPos		= {0,0,0};
	INT8	cComboInfo		= (-1);
	INT8	cForceAttack	= (-1);
	UINT32	ulAdditionalEffect = 0;
	UINT8	cHitIndex		= 0;
	
	m_csPacketCharAction.GetField(TRUE, pvPacket, nSize, 
								  &lCID,
								  &lTargetID,
								  &cAttackResult,
								  &pvPacketFactor,
								  &lHP,
								  //&stActionPos,
								  &cComboInfo,
								  &cForceAttack,
								  &ulAdditionalEffect,
								  &cHitIndex);

	AgpdCharacter	*pcsCharacter		= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacter	*pcsTargetCharacter	=	NULL;
	if( lTargetID	!= AP_INVALID_CID )
		pcsTargetCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lTargetID);

#ifdef _SEND_MODIFIED_FACTOR_PACKET
	CHAR	*szOriginalPacketFactor	= (CHAR *) pvPacketFactor;

	if (pvPacketFactor)
	{
		szOriginalPacketFactor	= (CHAR *) malloc(sizeof(CHAR) * (*((UINT16 *) pvPacketFactor) + 8 + 2));

		*((UINT16 *) szOriginalPacketFactor)		= *((UINT16 *) pvPacketFactor) + 8;

		*((UINT16 *) szOriginalPacketFactor + 1)	= 1;

		*((UINT16 *) szOriginalPacketFactor + 2)	= *((UINT16 *) pvPacketFactor) + 4;

		*((UINT16 *) szOriginalPacketFactor + 3)	= 8;

		CopyMemory((CHAR *) szOriginalPacketFactor + 8, pvPacketFactor, sizeof(CHAR) * (*((UINT16 *) pvPacketFactor) + 2));
	}

	m_pcsAgpmCharacter->ProcessActionPacket(pcsCharacter, pcsTargetCharacter, (PVOID) szOriginalPacketFactor,
											cAttackResult, cForceAttack, cComboInfo, AP_INVALID_SKILLID,
											ulAdditionalEffect, cHitIndex, pstCheckArg->bReceivedFromServer , AGPDCHAR_ACTION_TYPE_ATTACK);

	if (szOriginalPacketFactor)
		free(szOriginalPacketFactor);

#else
	m_pcsAgpmCharacter->ProcessActionPacket(pcsCharacter, pcsTargetCharacter, pvPacketFactor,
											cAttackResult, cForceAttack, cComboInfo, AP_INVALID_SKILLID,
											ulAdditionalEffect, cHitIndex, pstCheckArg->bReceivedFromServer , AGPDCHAR_ACTION_TYPE_ATTACK);

#endif	//_SEND_MODIFIED_FACTOR_PACKET

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

AgpdCharacter* AgpmOptimizedPacket2::ProcessCharacterPacket(PVOID pvPacketChar, DispatchArg *pstCheckArg)
{
	if (!pvPacketChar || !pstCheckArg)
		return NULL;

	INT32	lCID				= AP_INVALID_CID;
	INT32	lTID				= AP_INVALID_CID;
	CHAR	*szNameBuffer		= NULL;
	UINT16	unNameLength		= 0;
	INT8	cActionStatus		= 0;
	INT8	cIsNewCharacter		= 0;
	PVOID	pvPacketFactor		= NULL;
	INT8	cMoveFlag			= 0;
	INT8	cMoveDirection		= 0;
	AuPOS	stCurrentPos;
	AuPOS	stDestPos;
	INT32	lFollowTargetID		= AP_INVALID_CID;
	UINT16	unFollowDistance	= 0;
	INT16	nTurnX				= 0;
	INT16	nTurnY				= 0;
	UINT64	ulSpecialStatus		= AGPDCHAR_SPECIAL_STATUS_NONE;
	INT8	cIsTransform		= (-1);
	INT8	cFaceIndex			= (-1);
	INT8	cHairIndex			= (-1);
	INT8	cViewOption			= (-1);
	UINT16	unBitFlagOption		= 0;
	INT8	cCriminalStatus		= 0;
	INT32	lIsWantedCriminal	= -1;
	CHAR	*pszNickName		= NULL;
	BOOL	bNPCDisplayMap		= TRUE;
	BOOL	bNPCDisplayForNameBoard = TRUE;

	char	AuthBuffer[128] = {0, };
	UINT	unAuthLength = 0;

	ZeroMemory(&stCurrentPos, sizeof(AuPOS));
	ZeroMemory(&stDestPos, sizeof(AuPOS));

	m_csPacketCharView.GetField(FALSE, pvPacketChar, 0,
								&lCID,									// character id
								&lTID,									// character tid
								&szNameBuffer, &unNameLength,			// character name
								&cActionStatus,							// current action status
								&cIsNewCharacter,						// IsNewCharacter
								&pvPacketFactor,						// factor packet
								&cMoveFlag,								// move flag
								&cMoveDirection,						// move direction
								&stCurrentPos,							// character current pos
								&stDestPos,								// destination pos
								&lFollowTargetID,						// follow target id
								&unFollowDistance,						// follow distance
								&nTurnX,								// turn x
								&nTurnY,								// turn y
								&ulSpecialStatus,						// character special status
								&cIsTransform,							// is transform status, is ridable
								&cFaceIndex,							// face index
								&cHairIndex,							// hair index
								&cViewOption,							// view options
								&unBitFlagOption,						// bit flag option
								&cCriminalStatus,						// character criminal status			
								&lIsWantedCriminal,						// wanted criminal flag
								&pszNickName,							// nick
								&bNPCDisplayMap,
								&bNPCDisplayForNameBoard,
								&AuthBuffer, &unAuthLength				// gameguard
								);

	CHAR	szName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	ZeroMemory(szName, sizeof(CHAR) * (AGPDCHARACTER_MAX_ID_LENGTH + 1));

	if (unNameLength > AGPDCHARACTER_MAX_ID_LENGTH)
		unNameLength	= AGPDCHARACTER_MAX_ID_LENGTH;

	CopyMemory(szName, szNameBuffer, unNameLength);

	INT32	lFollowDistance	= (INT32)	unFollowDistance;
	FLOAT	fTurnX			= (FLOAT)	nTurnX;
	FLOAT	fTurnY			= (FLOAT)	nTurnY;

	PVOID	pvPacketMove	= m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, NULL, 0,
								&stCurrentPos,
								(stDestPos.x == 0.0f && stDestPos.z == 0.0f) ? &stCurrentPos : &stDestPos,
								&lFollowTargetID,
								&lFollowDistance,
								&fTurnX,
								&fTurnY,
								&cMoveFlag,
								&cMoveDirection);

	m_pcsAgpmCharacter->OnOperationAdd(pstCheckArg,
										lCID,
										lTID,
										szName,
										(INT8) AGPDCHAR_STATUS_IN_GAME_WORLD,
										cActionStatus,
										cCriminalStatus,
										pvPacketMove,
										NULL,
										pvPacketFactor,
										0,
										0,
										0,
										cIsNewCharacter,
										0,
										ulSpecialStatus,
										cIsTransform,
										NULL,
										cFaceIndex,
										cHairIndex,
										0,
										unBitFlagOption,
										-1,											// criminal
										-1,											// murderer
										lIsWantedCriminal,							// wanted criminal flag
										pszNickName,
										bNPCDisplayMap,
										bNPCDisplayForNameBoard,
										0
										);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacketMove);

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacter(lCID);
	if (!pcsCharacter)
		return NULL;

	m_pcsAgpmUIStatus->OnOperationUpdateViewHelmetOption(pcsCharacter, cViewOption);

	SetCharBitFlag(pcsCharacter, unBitFlagOption);

	return pcsCharacter;
}

BOOL AgpmOptimizedPacket2::ProcessItemPacket(AgpdCharacter *pcsCharacter, PVOID *ppvPacketItem, DispatchArg *pstCheckArg)
{
	if (!pcsCharacter || !ppvPacketItem || !pstCheckArg)
		return FALSE;

	for (int i = 0; i < AGPMITEM_PART_NUM; ++i)
	{
		if (!ppvPacketItem[i])
			break;

		INT32	lItemID				= AP_INVALID_IID;
		INT32	lItemTID			= AP_INVALID_IID;
		INT16	nStackCount			= 0;

		INT8	cNumPhysicalConvert	= 0;
		INT8	cNumSocket			= 0;
		INT8	cNumConvertedSocket	= 0;

		INT32	lConvertedItemTID[8];
		ZeroMemory(lConvertedItemTID, sizeof(INT32) * 8);

		INT32	lStatusFlag			= (-1);

		UINT16	unOptionTID[5];
		ZeroMemory(unOptionTID, sizeof(unOptionTID));

		m_csPacketItemView.GetField(FALSE, ppvPacketItem[i], 0,
									&lItemID,
									&lItemTID,
									&nStackCount,
									&cNumPhysicalConvert,
									&cNumSocket,
									&cNumConvertedSocket,
									&lConvertedItemTID[0],
									&lConvertedItemTID[1],
									&lConvertedItemTID[2],
									&lConvertedItemTID[3],
									&lConvertedItemTID[4],
									&lConvertedItemTID[5],
									&lConvertedItemTID[6],
									&lConvertedItemTID[7],
									&lStatusFlag,
									&unOptionTID[0],
									&unOptionTID[1],
									&unOptionTID[2],
									&unOptionTID[3],
									&unOptionTID[4]);

		PVOID	pvOptionPacket	= NULL;

		if (unOptionTID[0] != 0)
		{
			INT16	nPacketLength	= 0;
			pvOptionPacket	= m_pcsAgpmItem->m_csPacketOption.MakePacket(FALSE, &nPacketLength, 0,
															&unOptionTID[0],
															(unOptionTID[1] != 0) ? &unOptionTID[1] : NULL,
															(unOptionTID[2] != 0) ? &unOptionTID[2] : NULL,
															(unOptionTID[3] != 0) ? &unOptionTID[3] : NULL,
															(unOptionTID[4] != 0) ? &unOptionTID[4] : NULL);
		}

		m_pcsAgpmItem->OnOperationAdd(pstCheckArg->bReceivedFromServer,
										AGPDITEM_STATUS_EQUIP,
										lItemID,
										lItemTID,
										pcsCharacter,
										(INT32) nStackCount,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										0,
										lStatusFlag,
										pvOptionPacket,
										NULL,	//	Skill Plus Information
										NULL,	//	CashInformation
										NULL);

		if (pvOptionPacket)
			m_pcsAgpmItem->m_csPacketOption.FreePacket(pvOptionPacket);

		AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(lItemID);
		if (!pcsItem)
			continue;

		PVOID	pvPacketTID	= NULL;

		if (cNumConvertedSocket > 0)
		{
			pvPacketTID	= m_pcsAgpmItemConvert->m_csPacketTID.MakePacket(FALSE, NULL, 0,
													&lConvertedItemTID[0],
													&lConvertedItemTID[1],
													&lConvertedItemTID[2],
													&lConvertedItemTID[3],
													&lConvertedItemTID[4],
													&lConvertedItemTID[5],
													&lConvertedItemTID[6],
													&lConvertedItemTID[7]);
		}

		m_pcsAgpmItemConvert->OnOperationAdd(pcsCharacter, pcsItem, cNumPhysicalConvert, cNumSocket, cNumConvertedSocket, pvPacketTID);

		if (pvPacketTID)
			m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacketTID);
	}

	return TRUE;
}

BOOL AgpmOptimizedPacket2::ProcessBuffedSkillPacket(AgpdCharacter *pcsCharacter, CHAR *pszBuffedSkill, UINT16 unDataSize, DispatchArg *pstCheckArg)
{
	if (!pszBuffedSkill || unDataSize < 1 || !pstCheckArg || !pcsCharacter)
		return FALSE;

	AgpmOptimizedPacket2BuffedList	*pstBuffedList	= (AgpmOptimizedPacket2BuffedList *)	pszBuffedSkill;

	INT32	lDataCount	= unDataSize / sizeof(AgpmOptimizedPacket2BuffedList);

	for (int i = 0; i < lDataCount; ++i)
	{
		m_pcsAgpmSkill->OnReceiveAddBuffedList((ApBase *) pcsCharacter, pstBuffedList[i].lSkillTID, 0, pstBuffedList[i].lCasterTID, pstBuffedList[i].cChargeLevel , pstBuffedList[i].ulExpiredTime );
	}

	return TRUE;
}

BOOL AgpmOptimizedPacket2::ProcessEventPacket(AgpdCharacter *pcsCharacter, CHAR *szEventPacket, UINT16 unEventPacketSize, DispatchArg *pstCheckArg, UINT32 ulNID)
{
	if (!pcsCharacter || !szEventPacket || unEventPacketSize < 1 || !pstCheckArg)
		return FALSE;

	INT32	lProcessedIndex	= 0;

	while (lProcessedIndex < unEventPacketSize)
	{
		UINT16 unPacketLength = ((PPACKET_HEADER) ((CHAR *) szEventPacket + lProcessedIndex))->unPacketLength;

		if (unPacketLength < 1)
			return FALSE;

		m_pcsApmEventManager->OnReceive(AGPMEVENT_MANAGER,
								 (PVOID) ((CHAR *) szEventPacket + lProcessedIndex),
								 (INT16) unPacketLength,
								 ulNID,
								 pstCheckArg);

		lProcessedIndex	+= unPacketLength;
	}

	return TRUE;
}

BOOL AgpmOptimizedPacket2::ProcessCharAllItemPacket(CHAR *szCharAllItem, UINT16 unCharAllItemLength, DispatchArg *pstCheckArg, UINT32 ulNID)
{
	if (!szCharAllItem || unCharAllItemLength < 1 || !pstCheckArg)
		return FALSE;

	INT32	lProcessedIndex	= 0;

	while (lProcessedIndex < unCharAllItemLength)
	{
		UINT16 unPacketLength = ((PPACKET_HEADER) ((CHAR *) szCharAllItem + lProcessedIndex))->unPacketLength;

		if (unPacketLength < 1)
			return FALSE;

		m_pcsAgpmItem->OnReceive(AGPMITEM_PACKET_TYPE,
								 (PVOID) ((CHAR *) szCharAllItem + lProcessedIndex),
								 (INT16) unPacketLength,
								 ulNID,
								 pstCheckArg);

		lProcessedIndex	+= unPacketLength;
	}

	return TRUE;
}

BOOL AgpmOptimizedPacket2::ProcessCharAllItemConvertPacket(CHAR *szCharAllItemConvert, UINT16 unCharAllItemConvertLength, DispatchArg *pstCheckArg, UINT32 ulNID)
{
	if (!szCharAllItemConvert || unCharAllItemConvertLength < 1 || !pstCheckArg)
		return FALSE;

	INT32	lProcessedIndex	= 0;

	while (lProcessedIndex < unCharAllItemConvertLength)
	{
		UINT16 unPacketLength = ((PPACKET_HEADER) ((CHAR *) szCharAllItemConvert + lProcessedIndex))->unPacketLength;

		if (unPacketLength < 1)
			return FALSE;

		m_pcsAgpmItemConvert->OnReceive(AGPMITEMCONVERT_PACKET_TYPE,
								 (PVOID) ((CHAR *) szCharAllItemConvert + lProcessedIndex),
								 (INT16) unPacketLength,
								 ulNID,
								 pstCheckArg);

		lProcessedIndex	+= unPacketLength;
	}

	return TRUE;
}

BOOL AgpmOptimizedPacket2::ProcessCharAllSkillPacket(CHAR *szCharAllSkill, UINT16 unCharAllSkillLength, DispatchArg *pstCheckArg, UINT32 ulNID)
{
	if (!szCharAllSkill || unCharAllSkillLength < 1 || !pstCheckArg)
		return FALSE;

	INT32	lProcessedIndex	= 0;

	while (lProcessedIndex < unCharAllSkillLength)
	{
		UINT16 unPacketLength = ((PPACKET_HEADER) ((CHAR *) szCharAllSkill + lProcessedIndex))->unPacketLength;

		if (unPacketLength < 1)
			return FALSE;

		m_pcsAgpmSkill->OnReceive(AGPMSKILL_PACKET_TYPE,
								 (PVOID) ((CHAR *) szCharAllSkill + lProcessedIndex),
								 (INT16) unPacketLength,
								 ulNID,
								 pstCheckArg);

		lProcessedIndex	+= unPacketLength;
	}

	return TRUE;
}

PVOID AgpmOptimizedPacket2::MakePacketCharView(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, BOOL bIsNewChar)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8	cOperation			= AGPM_OPTIMIZEDPACKET2_ADD_CHARACTER_VIEW;

	INT8	cActionStatus		= (INT8) pcsCharacter->m_unActionStatus;
	PVOID	pvPacketFactor		= m_pcsAgpmFactors->MakePacketFactorsCharView(&pcsCharacter->m_csFactor);

	INT8	cMoveFlag = (pcsCharacter->m_eMoveDirection != MD_NODIRECTION ? AGPMCHARACTER_MOVE_FLAG_DIRECTION : 0) |
						(pcsCharacter->m_bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
			 			(pcsCharacter->m_bMove ? 0 : AGPMCHARACTER_MOVE_FLAG_SYNC) |
						(pcsCharacter->m_bSync ? AGPMCHARACTER_MOVE_FLAG_SYNC : 0) |
						(pcsCharacter->m_bMoveFast ? AGPMCHARACTER_MOVE_FLAG_FAST : 0) |
						(pcsCharacter->m_bMoveFollow ? AGPMCHARACTER_MOVE_FLAG_FOLLOW : 0) |
						(pcsCharacter->m_bHorizontal ? AGPMCHARACTER_MOVE_FLAG_HORIZONTAL : 0);

	INT8	cMoveDirection = (INT8)pcsCharacter->m_eMoveDirection;

	UINT16	unFollowDistance	= (UINT16)	pcsCharacter->m_lFollowDistance;

	INT16	nTurnX				= (INT16)	pcsCharacter->m_fTurnX;
	INT16	nTurnY				= (INT16)	pcsCharacter->m_fTurnY;

	UINT16	unIDLength			= 0;
	if (pcsCharacter->m_szID && pcsCharacter->m_szID[0])
		unIDLength	= (UINT16)strlen(pcsCharacter->m_szID);

	INT8	cIsTransform		= 0;
	if (pcsCharacter->m_bIsTrasform)
		cIsTransform |= AGPMCHAR_FLAG_TRANSFORM;
	if (pcsCharacter->m_bRidable)
		cIsTransform |= AGPMCHAR_FLAG_RIDABLE;
	if (pcsCharacter->m_bIsEvolution)
		cIsTransform |= AGPMCHAR_FLAG_EVOLUTION;

	INT8	cFaceIndex			= (INT8)	pcsCharacter->m_lFaceIndex;
	INT8	cHairIndex			= (INT8)	pcsCharacter->m_lHairIndex;

	//UINT16	unBitFlagOption		= GetCharBitFlag(pcsCharacter);

	AgpdUIStatusADChar	*pcsAttachData		= m_pcsAgpmUIStatus->GetADCharacter(pcsCharacter);

	PVOID	pvPacketChar		= m_csPacketCharView.MakePacket(FALSE, pnPacketLength, 0,
										&pcsCharacter->m_lID,																				// character id
										&pcsCharacter->m_lTID1,																				// character tid
										pcsCharacter->m_szID, &unIDLength,																	// character name
										&cActionStatus,																						// current action status
										(bIsNewChar) ? &bIsNewChar : NULL,																	// IsNewCharacter
										pvPacketFactor,																						// factor packet
										&cMoveFlag,																							// move flag
										&cMoveDirection,																					// move direction
										&pcsCharacter->m_stPos,																				// character current pos
										(pcsCharacter->m_bMove) ? &pcsCharacter->m_stDestinationPos : NULL,									// destination pos
										(pcsCharacter->m_lFollowTargetID != AP_INVALID_CID) ? &pcsCharacter->m_lFollowTargetID : NULL,		// follow target id
										(pcsCharacter->m_lFollowTargetID != AP_INVALID_CID) ? &unFollowDistance : NULL,						// follow distance
										(pcsCharacter->m_bMove) ? NULL : &nTurnX,															// turn x
										(pcsCharacter->m_bMove) ? NULL : &nTurnY,															// turn y
										&pcsCharacter->m_ulSpecialStatus,																	// character special status
										&cIsTransform,																						// is transform status, is ridable
										&cFaceIndex,																						// face index
										&cHairIndex,																						// hair index
										&pcsAttachData->m_cOptionViewHelmet,																// view options
										&pcsCharacter->m_unEventStatusFlag,																	// bit flag option
										&pcsCharacter->m_unCriminalStatus,																	// character criminal status			
										&pcsCharacter->m_bIsWantedCriminal,																	// wanted criminal flag
										pcsCharacter->m_szNickName,
										&pcsCharacter->m_bNPCDisplayForMap,
										&pcsCharacter->m_bNPCDisplayForNameBoard
										);

	ApSafeArray<PVOID, AGPMITEM_PART_NUM>	pvPacketItem;
	pvPacketItem.MemSetAll();

	INT32	lIndexItemPacket	= 0;
	INT32	i = 0;

	// 캐릭터가 입고 있는 equip item 들에 대한 view info를 보내준다.
	for (i = 0; i < AGPMITEM_PART_NUM; ++i)
	{
		AgpdItem	*pcsItem	= m_pcsAgpmItem->GetEquipSlotItem(pcsCharacter, (AgpmItemPart) i);
		if (!pcsItem || !pcsItem->m_pcsItemTemplate)
			continue;

		// 2007.12.14. steeple
		// 서버가 간간히 죽어버리는 현상을 막기 위해서, 아이템을 한번 더 구해본다. 
		pcsItem = m_pcsAgpmItem->GetItem(pcsItem->m_lID);
		if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "MakePacketCharView(...), pcsItem is null, Part:%d, Character:%s\n",
						i, pcsCharacter->m_szID);
			AuLogFile_s("LOG\\ItemError.log", strCharBuff);

			continue;
		}

		INT16	nStackCount		= (INT16)	pcsItem->m_nCount;

		AgpdItemConvertADItem	*pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pcsItem);

		INT8	cNumPhysicalConvert			= (INT8)	pcsAttachData->m_lPhysicalConvertLevel;
		INT8	cNumSocket					= (INT8)	pcsAttachData->m_lNumSocket;
		INT8	cNumConvertedSocket			= (INT8)	pcsAttachData->m_lNumConvert;

		ApSafeArray<INT32, 8>				alConvertedItemTemplateID;
		alConvertedItemTemplateID.MemSetAll();

		for (int j = 0; j < cNumConvertedSocket; ++j)
		{
			alConvertedItemTemplateID[j]	= pcsAttachData->m_stSocketAttr[j].lTID;
		}

		pvPacketItem[lIndexItemPacket++]	= m_csPacketItemView.MakePacket(FALSE, pnPacketLength, 0,
												&pcsItem->m_lID,
												&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID,
												&nStackCount,

												(cNumPhysicalConvert > 0) ? &cNumPhysicalConvert : NULL,
												(cNumSocket > 0) ? &cNumSocket : NULL,
												(cNumConvertedSocket > 0) ? &cNumConvertedSocket : NULL,

												(alConvertedItemTemplateID[0] != AP_INVALID_IID) ? &alConvertedItemTemplateID[0] : NULL,
												(alConvertedItemTemplateID[1] != AP_INVALID_IID) ? &alConvertedItemTemplateID[1] : NULL,
												(alConvertedItemTemplateID[2] != AP_INVALID_IID) ? &alConvertedItemTemplateID[2] : NULL,
												(alConvertedItemTemplateID[3] != AP_INVALID_IID) ? &alConvertedItemTemplateID[3] : NULL,
												(alConvertedItemTemplateID[4] != AP_INVALID_IID) ? &alConvertedItemTemplateID[4] : NULL,
												(alConvertedItemTemplateID[5] != AP_INVALID_IID) ? &alConvertedItemTemplateID[5] : NULL,
												(alConvertedItemTemplateID[6] != AP_INVALID_IID) ? &alConvertedItemTemplateID[6] : NULL,
												(alConvertedItemTemplateID[7] != AP_INVALID_IID) ? &alConvertedItemTemplateID[7] : NULL,
												
												&pcsItem->m_lStatusFlag,

												pcsItem->m_aunOptionTID[0] ? &pcsItem->m_aunOptionTID[0] : NULL,
												pcsItem->m_aunOptionTID[1] ? &pcsItem->m_aunOptionTID[1] : NULL,
												pcsItem->m_aunOptionTID[2] ? &pcsItem->m_aunOptionTID[2] : NULL,
												pcsItem->m_aunOptionTID[3] ? &pcsItem->m_aunOptionTID[3] : NULL,
												pcsItem->m_aunOptionTID[4] ? &pcsItem->m_aunOptionTID[4] : NULL);
	}

	ApSafeArray<AgpmOptimizedPacket2BuffedList, AGPMSKILL_MAX_SKILL_BUFF>	astBuffedList;
	astBuffedList.MemSetAll();

	AgpdSkillAttachData	*pcsAttachSkillData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);

	for (i = 0; i < AGPMSKILL_MAX_SKILL_BUFF; ++i)
	{
		if (pcsAttachSkillData->m_astBuffSkillList[i].lSkillTID == AP_INVALID_SKILLID)
			break;

		astBuffedList[i].lSkillTID		= pcsAttachSkillData->m_astBuffSkillList[i].lSkillTID;
		astBuffedList[i].lCasterTID		= pcsAttachSkillData->m_astBuffSkillList[i].lCasterTID;
		astBuffedList[i].cChargeLevel	= (INT8) pcsAttachSkillData->m_astBuffSkillList[i].lChargeLevel;
		astBuffedList[i].ulExpiredTime	= pcsAttachSkillData->m_astBuffSkillList[i].ulEndTimeMSec;
	}

	UINT16	unBuffedSkillLength	= (UINT16)sizeof(AgpmOptimizedPacket2BuffedList) * i;
	PVOID	pvPacketBuffedSkill	= &astBuffedList[0];

	CHAR	*szEventPacketBuffer	= (CHAR *) m_csOptimizedPacketMemoryPool.Alloc();
	//ZeroMemory(szEventPacketBuffer, sizeof(CHAR) * AGPMOPTIMIZEDPACKET2_BUFFER_SIZE);

	INT32	lEventPacketBufferLength		= 0;

	ApdEventAttachData	*pcsEventAttachData	= m_pcsApmEventManager->GetEventData((ApBase *) pcsCharacter);
	if (szEventPacketBuffer && pcsEventAttachData)
	{
		for (i = 0; i < pcsEventAttachData->m_unFunction; ++i)
		{
			INT32	lEventIndex	= (INT32) pcsEventAttachData->m_astEvent[i].m_eFunction;

			if (lEventIndex <= APDEVENT_FUNCTION_NONE ||
				lEventIndex >= APDEVENT_MAX_FUNCTION)
				continue;

			if (lEventIndex == APDEVENT_FUNCTION_TELEPORT ||
				lEventIndex == APDEVENT_FUNCTION_NPCTRADE ||
				lEventIndex == APDEVENT_FUNCTION_BANK ||
				lEventIndex == APDEVENT_FUNCTION_NPCDAILOG ||
				lEventIndex == APDEVENT_FUNCTION_ITEMCONVERT ||
				lEventIndex == APDEVENT_FUNCTION_GUILD ||
				lEventIndex == APDEVENT_FUNCTION_PRODUCT ||
				lEventIndex == APDEVENT_FUNCTION_SKILLMASTER ||
				lEventIndex == APDEVENT_FUNCTION_REFINERY ||
				lEventIndex == APDEVENT_FUNCTION_QUEST ||
				lEventIndex == APDEVENT_FUNCTION_AUCTION ||
				lEventIndex == APDEVENT_FUNCTION_CHAR_CUSTOMIZE ||
				lEventIndex == APDEVENT_FUNCTION_ITEM_REPAIR ||
				lEventIndex == APDEVENT_FUNCTION_REMISSION ||
				lEventIndex == APDEVENT_FUNCTION_WANTEDCRIMINAL ||
				lEventIndex == APDEVENT_FUNCTION_SIEGEWAR_NPC ||
				lEventIndex == APDEVENT_FUNCTION_TAX ||
				lEventIndex == APDEVENT_FUNCTION_GUILD_WAREHOUSE ||
				lEventIndex == APDEVENT_FUNCTION_ARCHLORD ||
				lEventIndex == APDEVENT_FUNCTION_GAMBLE ||
				lEventIndex == APDEVENT_FUNCTION_WORLD_CHAMPIONSHIP
				)
			{
				INT16	nPacketLength	= 0;
				PVOID	pvPacketEvent	= m_pcsApmEventManager->MakePacketEventData(&pcsEventAttachData->m_astEvent[i], (ApdEventFunction) lEventIndex, &nPacketLength);

				if (lEventPacketBufferLength + nPacketLength < AGPMOPTIMIZEDPACKET2_BUFFER_SIZE)
				{
					CopyMemory(szEventPacketBuffer + lEventPacketBufferLength,
							pvPacketEvent,
							sizeof(CHAR) * nPacketLength);

					lEventPacketBufferLength	+= nPacketLength;
				}

				m_csPacket.FreePacket(pvPacketEvent);
			}
		}
	}

	CHAR	*pszGuildName		= NULL;
	INT32	lGuildMarkTID		= 0;
	INT32	lGuildMarkColor		= 0xFFFFFFFF;
	INT32	lBRRanking			= 0;

	if (m_pcsAgpmCharacter->IsPC(pcsCharacter))
	{
		AgpdGuildADChar	*pcsAttachedGuild	= m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
		if (pcsAttachedGuild)
		{
			AgpdGuild *pcsGuild = m_pcsAgpmGuild->GetGuildLock(pcsAttachedGuild->m_szGuildID);
			if(pcsGuild)
			{
				if (m_pcsAgpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID))
				{
					pszGuildName	= pcsAttachedGuild->m_szGuildID;
					lGuildMarkTID	= pcsGuild->m_lGuildMarkTID;
					lGuildMarkColor	= pcsGuild->m_lGuildMarkColor;
					lBRRanking		= pcsGuild->m_lBRRanking;
				}
				pcsGuild->m_Mutex.Release();
			}
		}
	}
	else if (m_pcsAgpmCharacter->IsMonster(pcsCharacter))
	{
		// kermi 2006.8.10
		// 공성전에서 사용될 NPC Guild는 PC가 아니지만 길드 이름이 필요하다.
		AgpdGuildADChar	*pcsAttachedGuild	= m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
		if (pcsAttachedGuild && (NULL != pcsAttachedGuild->m_szGuildID[0]))
		{
			pszGuildName	= pcsAttachedGuild->m_szGuildID;
			lGuildMarkTID	= pcsAttachedGuild->m_lGuildMarkTID;
			lGuildMarkColor	= pcsAttachedGuild->m_lGuildMarkColor;			
		}
	}

	PACKET_AGPPTITLE_USE_NEAR pvPacketTitle;
	INT16 nSize = sizeof(PACKET_AGPPTITLE_USE_NEAR);
	if(pcsCharacter->m_csTitle != NULL)
	{
		INT32 lTitleID = pcsCharacter->m_csTitle->GetUseTitle();
		pvPacketTitle.nTitleID = lTitleID;
		pvPacketTitle.bUse = (lTitleID > 0 ) ? TRUE : FALSE;
	}

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDVIEW_PACKET_TYPE,
												&cOperation,
												pvPacketChar,
												(pvPacketItem[0]) ? &pvPacketItem[0] : NULL,
												pvPacketBuffedSkill, &unBuffedSkillLength,
												szEventPacketBuffer, &lEventPacketBufferLength,
												pszGuildName,
												NULL,
												NULL,
												NULL,
												NULL,
												&lGuildMarkTID,
												&lGuildMarkColor,
												&lBRRanking,
												&pvPacketTitle, &nSize
												);

	m_csPacketCharView.FreePacket(pvPacketChar);
	for (i = 0; i < AGPMITEM_PART_NUM; ++i)
		m_csPacketItemView.FreePacket(pvPacketItem[i]);

	m_csOptimizedPacketMemoryPool.Free(szEventPacketBuffer);
	
	m_csPacket.SetCID(pvPacket, *pnPacketLength, pcsCharacter->m_lID);

//	PVOID	pvBuffer	= m_csOptimizedPacketMemoryPool.Alloc();
//
//	UINT	nBufferSize	= *pnPacketLength;
//	CopyMemory(pvBuffer, pvPacket, *pnPacketLength);
//
//	m_csPacket.m_MiniLZO.Compress((BYTE *) pvBuffer, &nBufferSize);
//
//	m_csOptimizedPacketMemoryPool.Free(pvBuffer);

	return	pvPacket;
}

PVOID AgpmOptimizedPacket2::MakePacketCharMove(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8 cMoveFlag = (pcsCharacter->m_eMoveDirection != MD_NODIRECTION ? AGPMCHARACTER_MOVE_FLAG_DIRECTION: 0) |
					 (pcsCharacter->m_bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
			 		 (pcsCharacter->m_bMove ? 0 : AGPMCHARACTER_MOVE_FLAG_STOP) |
					 (pcsCharacter->m_bSync ? AGPMCHARACTER_MOVE_FLAG_SYNC : 0) |
					 (pcsCharacter->m_bMoveFast ? AGPMCHARACTER_MOVE_FLAG_FAST : 0) |
					 (pcsCharacter->m_bMoveFollow ? AGPMCHARACTER_MOVE_FLAG_FOLLOW : 0) |
			 		 (pcsCharacter->m_bHorizontal ? AGPMCHARACTER_MOVE_FLAG_HORIZONTAL : 0);

	INT8 cMoveDirection = (INT8)pcsCharacter->m_eMoveDirection;

	UINT16 unFollowDistance	= (UINT16) pcsCharacter->m_lFollowDistance;

	/*
	AuPOSBaseMeter	stOriginPos;
	stOriginPos.x	= (INT16) (pcsCharacter->m_stPos.x / 100);
	stOriginPos.y	= (INT16) (pcsCharacter->m_stPos.y / 100);
	stOriginPos.z	= (INT16) (pcsCharacter->m_stPos.z / 100);
	*/

	PVOID	pvPacket	= m_csPacketCharMove.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDCHARMOVE_PACKET_TYPE,
										&pcsCharacter->m_lID,
										&pcsCharacter->m_stPos,
										//&stOriginPos,
										pcsCharacter->m_bMove ? (pcsCharacter->m_eMoveDirection ? &pcsCharacter->m_stDirection : &pcsCharacter->m_stDestinationPos) : &pcsCharacter->m_stPos,
										(pcsCharacter->m_lFollowTargetID != AP_INVALID_CID) ? &pcsCharacter->m_lFollowTargetID : NULL,
										(pcsCharacter->m_lFollowTargetID != AP_INVALID_CID) ? &unFollowDistance : NULL,
										&cMoveFlag,
										&cMoveDirection,
										NULL,
										NULL);

	m_csPacketCharMove.SetCID(pvPacket, *pnPacketLength, pcsCharacter->m_lID);

	return	pvPacket;
}

PVOID AgpmOptimizedPacket2::MakePacketCharMoveContainAction(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8 cMoveFlag = (pcsCharacter->m_eMoveDirection != MD_NODIRECTION ? AGPMCHARACTER_MOVE_FLAG_DIRECTION : 0) |
					 (pcsCharacter->m_bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
			 		 (pcsCharacter->m_bMove ? 0 : AGPMCHARACTER_MOVE_FLAG_STOP) |
					 (pcsCharacter->m_bSync ? AGPMCHARACTER_MOVE_FLAG_SYNC : 0) |
					 (pcsCharacter->m_bMoveFast ? AGPMCHARACTER_MOVE_FLAG_FAST : 0) |
					 (pcsCharacter->m_bMoveFollow ? AGPMCHARACTER_MOVE_FLAG_FOLLOW : 0) |
			 		 (pcsCharacter->m_bHorizontal ? AGPMCHARACTER_MOVE_FLAG_HORIZONTAL : 0) |
					 (pcsCharacter->m_eMoveDirection ? AGPMCHARACTER_MOVE_FLAG_DIRECTION : 0);

	INT8 cMoveDirection = (INT8)pcsCharacter->m_eMoveDirection;

	// pcsCharacter->m_bMoveDirection -> MOVE_DIRECTION 의 8방향 enum 으로 변경. 3비트 정도 할당해서 보내야함.

	UINT16 unFollowDistance	= (UINT16) pcsCharacter->m_lFollowDistance;

	/*
	AuPOSBaseMeter	stOriginPos;
	stOriginPos.x	= (INT16) (pcsCharacter->m_stPos.x / 100);
	stOriginPos.y	= (INT16) (pcsCharacter->m_stPos.y / 100);
	stOriginPos.z	= (INT16) (pcsCharacter->m_stPos.z / 100);
	*/

	INT8 cNextActionType	= AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_NONE;
	INT32	lSkillID	= AP_INVALID_SKILLID;

	if (pcsCharacter->m_stNextAction.m_eActionType == AGPDCHAR_ACTION_TYPE_ATTACK)
	{
		cNextActionType		= AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_ATTACK;

		pcsCharacter->m_bIsActionMoveLock	= TRUE;
	}
	else if (pcsCharacter->m_stNextAction.m_eActionType == AGPDCHAR_ACTION_TYPE_SKILL)
	{
		cNextActionType		= AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_SKILL;
		lSkillID			= (INT32)pcsCharacter->m_stNextAction.m_lUserData[0];

		pcsCharacter->m_bIsActionMoveLock	= TRUE;
	}

	if (pcsCharacter->m_bIsActionMoveLock)
	{
		pcsCharacter->m_stLockAction	= pcsCharacter->m_stNextAction;
	}

	PVOID	pvPacket	= m_csPacketCharMove.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDCHARMOVE_PACKET_TYPE,
										&pcsCharacter->m_lID,
										&pcsCharacter->m_stPos,
										//&stOriginPos,
										pcsCharacter->m_bMove ? (pcsCharacter->m_eMoveDirection ? &pcsCharacter->m_stDirection : &pcsCharacter->m_stDestinationPos) : &pcsCharacter->m_stPos,
										(pcsCharacter->m_lFollowTargetID != AP_INVALID_CID) ? &pcsCharacter->m_lFollowTargetID : NULL,
										(pcsCharacter->m_lFollowTargetID != AP_INVALID_CID) ? &unFollowDistance : NULL,
										&cMoveFlag,
										&cMoveDirection,
										(cNextActionType == AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_NONE) ? NULL : &cNextActionType,
										(lSkillID == AP_INVALID_SKILLID) ? NULL : &lSkillID);

	m_csPacketCharMove.SetCID(pvPacket, *pnPacketLength, pcsCharacter->m_lID);

	return	pvPacket;
}

PVOID AgpmOptimizedPacket2::MakePacketCharMove(INT32 lCID, INT8 cMoveFlag, INT8 cMoveDirection, AuPOS *pcsDestPos, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	PVOID	pvPacket	= m_csPacketCharMove.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDCHARMOVE_PACKET_TYPE,
										&lCID,
										NULL,
										pcsDestPos,
										NULL,
										NULL,
										&cMoveFlag,
										&cMoveDirection,
										NULL,
										NULL);

	m_csPacketCharMove.SetCID(pvPacket, *pnPacketLength, lCID);

	return	pvPacket;
}

PVOID AgpmOptimizedPacket2::MakePacketReleaseMoveAction(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT32	cOperation	= AGPM_OPTIMIZEDPACKET2_RELEASE_MOVE_ACTION;

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDVIEW_PACKET_TYPE,
												&cOperation,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												&pcsCharacter->m_lID,
												NULL,	/* GuildMarkTID */
												NULL,	/* GuildMarkColor */
												NULL,	/* IsWinner */
												NULL	/* Title */
												);

	return pvPacket;
}

PVOID AgpmOptimizedPacket2::MakePacketCharAction(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetCharacter,
												INT8 cAttackResult, PVOID pvPacketFactor, INT32 lHP,
												INT8 cComboInfo, BOOL bForceAttack, UINT32 ulAdditionalEffect,
												UINT8 cHitIndex, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	PVOID	pvPacket	= m_csPacketCharAction.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDCHARACTION_PACKET_TYPE,
											&pcsCharacter->m_lID,
											&pcsTargetCharacter->m_lID,
											&cAttackResult,
#ifdef _SEND_MODIFIED_FACTOR_PACKET
											(pvPacketFactor) ? ((CHAR *) pvPacketFactor + 8) : NULL,
#else
											pvPacketFactor,
#endif	//_SEND_MODIFIED_FACTOR_PACKET
											(lHP != 0) ? &lHP : NULL,
											//&pcsCharacter->m_stPos,
											(cComboInfo > 0) ? &cComboInfo : NULL,
											(bForceAttack) ? &bForceAttack : NULL,
											(ulAdditionalEffect != 0) ? &ulAdditionalEffect : NULL,
											(cHitIndex != 0) ? &cHitIndex : NULL);

	m_csPacketCharAction.SetCID(pvPacket, *pnPacketLength, pcsCharacter->m_lID);

	return	pvPacket;
}

PVOID AgpmOptimizedPacket2::MakePacketCharAction(INT32 lCID, INT32 lTargetID, INT8 cComboInfo, BOOL bForceAttack,
												 UINT32 ulAdditionalEffect, UINT8 cHitIndex, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	PVOID	pvPacket	=  m_csPacketCharAction.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDCHARACTION_PACKET_TYPE,
											&lCID,
											&lTargetID,
											NULL,
											NULL,
											NULL,
											//&pcsCharacter->m_stPos,
											(cComboInfo > 0) ? &cComboInfo : NULL,
											(bForceAttack) ? &bForceAttack : NULL,
											(ulAdditionalEffect != 0) ? &ulAdditionalEffect : NULL,
											(cHitIndex != 0) ? &cHitIndex : NULL);

	m_csPacketCharAction.SetCID(pvPacket, *pnPacketLength, lCID);

	return	pvPacket;
}

BOOL AgpmOptimizedPacket2::AddItemPacketToBuffer(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize, INT32 *plAddBufferSize)
{
	if (!pcsItem || !szBuffer || lBufferSize < 1 || !plAddBufferSize)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItem->MakePacketItem(pcsItem, &nPacketLength);

	if (pvPacket && nPacketLength > 0)
	{
		if (nPacketLength > lBufferSize)
		{
			m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);
			return FALSE;
		}

		CopyMemory(szBuffer, pvPacket, nPacketLength);

		*plAddBufferSize	= nPacketLength;

		m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

		return TRUE;
	}

	return FALSE;
}

BOOL AgpmOptimizedPacket2::AddItemConvertPacketToBuffer(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize, INT32 *plAddBufferSize)
{
	if (!pcsItem || !szBuffer || lBufferSize < 1 || !plAddBufferSize)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItemConvert->MakePacketAdd(pcsItem, &nPacketLength);

	if (pvPacket && nPacketLength > 0)
	{
		if (nPacketLength > lBufferSize)
		{
			m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);
			return FALSE;
		}

		CopyMemory(szBuffer, pvPacket, nPacketLength);

		*plAddBufferSize	= nPacketLength;

		m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

		return TRUE;
	}

	return FALSE;
}

INT32 AgpmOptimizedPacket2::MakeBufferCharAllItem(AgpdCharacter *pcsCharacter, CHAR *szItemBuffer, INT32 *plItemBufferSize, CHAR *szItemConvertBuffer, INT32 *plItemConvertBufferSize)
{
	if (!pcsCharacter || !szItemBuffer || !szItemConvertBuffer || !plItemBufferSize || !plItemConvertBufferSize)
		return 0;

	INT32	lTotalItemPacketSize		= 0;
	INT32	lTotalItemConvertPacketSize	= 0;

	// Make All Item Packet
	AgpdGridItem	*pcsGridItem	= NULL;
	int i = 0;
	for (i = 0; i < AGPMITEM_PART_NUM; ++i)
	{
		pcsGridItem = m_pcsAgpmItem->GetEquipItem(pcsCharacter, i);

		if (pcsGridItem == NULL)
			continue;

		AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		INT32	lAddBufferSize	= 0;
		if (AddItemPacketToBuffer(pcsItem, szItemBuffer + lTotalItemPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemPacketSize, &lAddBufferSize))
			lTotalItemPacketSize	+= lAddBufferSize;

		INT32	lAddConvertBufferSize	= 0;
		if (AddItemConvertPacketToBuffer(pcsItem, szItemConvertBuffer + lTotalItemConvertPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemConvertPacketSize, &lAddConvertBufferSize))
			lTotalItemConvertPacketSize	+= lAddConvertBufferSize;
	}

	AgpdItemADChar	*pcsItemADChar	= m_pcsAgpmItem->GetADCharacter(pcsCharacter);
	
	i = 0;
	for (pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		INT32	lAddBufferSize	= 0;
		if (AddItemPacketToBuffer(pcsItem, szItemBuffer + lTotalItemPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemPacketSize, &lAddBufferSize))
			lTotalItemPacketSize	+= lAddBufferSize;

		INT32	lAddConvertBufferSize	= 0;
		if (AddItemConvertPacketToBuffer(pcsItem, szItemConvertBuffer + lTotalItemConvertPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemConvertPacketSize, &lAddConvertBufferSize))
			lTotalItemConvertPacketSize	+= lAddConvertBufferSize;
	}

	i = 0;
	for (pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csSubInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csSubInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		INT32	lAddBufferSize	= 0;
		if (AddItemPacketToBuffer(pcsItem, szItemBuffer + lTotalItemPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemPacketSize, &lAddBufferSize))
			lTotalItemPacketSize	+= lAddBufferSize;

		INT32	lAddConvertBufferSize	= 0;
		if (AddItemConvertPacketToBuffer(pcsItem, szItemConvertBuffer + lTotalItemConvertPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemConvertPacketSize, &lAddConvertBufferSize))
			lTotalItemConvertPacketSize	+= lAddConvertBufferSize;
	}

	i = 0;
	for (pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csQuestGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csQuestGrid, &i))
	{
		AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		INT32	lAddBufferSize	= 0;
		if (AddItemPacketToBuffer(pcsItem, szItemBuffer + lTotalItemPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemPacketSize, &lAddBufferSize))
			lTotalItemPacketSize	+= lAddBufferSize;

		INT32	lAddConvertBufferSize	= 0;
		if (AddItemConvertPacketToBuffer(pcsItem, szItemConvertBuffer + lTotalItemConvertPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemConvertPacketSize, &lAddConvertBufferSize))
			lTotalItemConvertPacketSize	+= lAddConvertBufferSize;
	}

	i = 0;
	for (pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csSalesBoxGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csSalesBoxGrid, &i))
	{
		AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		INT32	lAddBufferSize	= 0;
		if (AddItemPacketToBuffer(pcsItem, szItemBuffer + lTotalItemPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemPacketSize, &lAddBufferSize))
			lTotalItemPacketSize	+= lAddBufferSize;

		INT32	lAddConvertBufferSize	= 0;
		if (AddItemConvertPacketToBuffer(pcsItem, szItemConvertBuffer + lTotalItemConvertPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemConvertPacketSize, &lAddConvertBufferSize))
			lTotalItemConvertPacketSize	+= lAddConvertBufferSize;
	}

	*plItemBufferSize			= lTotalItemPacketSize;
	*plItemConvertBufferSize	= lTotalItemConvertPacketSize;

	return lTotalItemPacketSize;
}

INT32 AgpmOptimizedPacket2::MakeBufferCharAllBankItem(AgpdCharacter *pcsCharacter, CHAR *szItemBuffer, INT32 *plItemBufferSize, CHAR *szItemConvertBuffer, INT32 *plItemConvertBufferSize)
{
	if (!pcsCharacter || !szItemBuffer || !szItemConvertBuffer || !plItemBufferSize || !plItemConvertBufferSize)
		return 0;

	INT32	lTotalItemPacketSize		= 0;
	INT32	lTotalItemConvertPacketSize	= 0;

	// Make All Item Packet
	AgpdGridItem	*pcsGridItem	= NULL;
	AgpdItemADChar	*pcsItemADChar	= m_pcsAgpmItem->GetADCharacter(pcsCharacter);

	INT32 lTotalItemCount = pcsItemADChar->m_csBankGrid.m_lItemCount;
	INT32 lAddedItemCount = 0;
	
	INT32 i = 0;
	for (pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i))
	{
		AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		INT32	lAddBufferSize	= 0;
		if (AddItemPacketToBuffer(pcsItem, szItemBuffer + lTotalItemPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemPacketSize, &lAddBufferSize))
		{
			lTotalItemPacketSize	+= lAddBufferSize;
			lAddedItemCount++;
		}

		INT32	lAddConvertBufferSize	= 0;
		if (AddItemConvertPacketToBuffer(pcsItem, szItemConvertBuffer + lTotalItemConvertPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemConvertPacketSize, &lAddConvertBufferSize))
			lTotalItemConvertPacketSize	+= lAddConvertBufferSize;
	}	
	
	if (lTotalItemCount != lAddedItemCount)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] (%d of %d) maked. size=(%d, %d) in MakeBufferCharAllBankItem()\n",
								pcsCharacter->m_szID,
								lAddedItemCount,
								lTotalItemCount,
								lTotalItemPacketSize,
								lTotalItemConvertPacketSize);
		AuLogFile_s("LOG\\OptimizedPacketError.txt", strCharBuff);
	}
	
	*plItemBufferSize			= lTotalItemPacketSize;
	*plItemConvertBufferSize	= lTotalItemConvertPacketSize;

	return lTotalItemPacketSize;
}

INT32 AgpmOptimizedPacket2::MakeBufferCharAllCashItem(AgpdCharacter *pcsCharacter,
													  CHAR *szItemBuffer,
													  INT32 *plItemBufferSize,
													  CHAR *szItemConvertBuffer,
													  INT32 *plItemConvertBufferSize,
													  INT32 *pIndex)
{
	if (!pcsCharacter || !szItemBuffer || !szItemConvertBuffer || !plItemBufferSize || !plItemConvertBufferSize)
		return 0;

	INT32	lTotalItemPacketSize		= 0;
	INT32	lTotalItemConvertPacketSize	= 0;

	// Make All Item Packet
	AgpdGridItem	*pcsGridItem	= NULL;
	AgpdItemADChar	*pcsItemADChar	= m_pcsAgpmItem->GetADCharacter(pcsCharacter);
	
	INT32 lTotalItemCount = pcsItemADChar->m_csCashInventoryGrid.m_lItemCount;
	INT32 lAddedItemCount = 0;
	
	INT32 i = *pIndex;
	for (pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		INT32	lAddBufferSize	= 0;
		if (AddItemPacketToBuffer(pcsItem, szItemBuffer + lTotalItemPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemPacketSize, &lAddBufferSize))
		{
			lTotalItemPacketSize	+= lAddBufferSize;
			lAddedItemCount++;
		}

		INT32	lAddConvertBufferSize	= 0;
		if (AddItemConvertPacketToBuffer(pcsItem, szItemConvertBuffer + lTotalItemConvertPacketSize, AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalItemConvertPacketSize, &lAddConvertBufferSize))
			lTotalItemConvertPacketSize	+= lAddConvertBufferSize;
	}

	//if (lTotalItemCount != lAddedItemCount)
	//{
	//	AuLogFile("OptimizedPacketError.txt", "[%s] (%d of %d) maked. size=(%d, %d) in MakeBufferCharAllCashItem()\n",
	//							pcsCharacter->m_szID,
	//							lAddedItemCount,
	//							lTotalItemCount,
	//							lTotalItemPacketSize,
	//							lTotalItemConvertPacketSize
	//							);
	//}

	*plItemBufferSize = lTotalItemPacketSize;
	*plItemConvertBufferSize = lTotalItemConvertPacketSize;
	*pIndex = (lTotalItemCount == lAddedItemCount) ? (CashItemPacketIsOk) : (lAddedItemCount);

	return lTotalItemPacketSize;
}

INT32 AgpmOptimizedPacket2::MakeBufferCharAllSkill(AgpdCharacter *pcsCharacter, CHAR *szBuffer)
{
	if (!pcsCharacter || !szBuffer)
		return 0;

	AgpdSkillAttachData	*pcsSkillAttachData	= (AgpdSkillAttachData *) m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);
	if (!pcsSkillAttachData)
		return 0;

	INT32	lTotalSkillPacketLength	= 0;

	// send skill information
	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		if (!pcsSkillAttachData->m_alSkillID[i])
			break;

		AgpdSkill	*pcsSkill = m_pcsAgpmSkill->GetSkill(pcsSkillAttachData->m_alSkillID[i]);
		if (!pcsSkill)
			continue;

		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= m_pcsAgpmSkill->MakePacketSkill(pcsSkill, &nPacketLength);
		
		if (pvPacket && nPacketLength > 0)
		{
			if (AGPMOPTIMIZEDPACKET2_BUFFER_SIZE - lTotalSkillPacketLength < nPacketLength)
			{
				m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);
				return lTotalSkillPacketLength;
			}

			CopyMemory(szBuffer + lTotalSkillPacketLength, pvPacket, nPacketLength);

			lTotalSkillPacketLength	+= nPacketLength;

			m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);
		}
	}
	
	return lTotalSkillPacketLength;
}

PVOID AgpmOptimizedPacket2::MakePacketCharAllItemSkillExceptBankCash(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	// make all item packet
	INT32	lItemPacketBufferSize			= 0;
	CHAR	*szItemPacketBuffer				= (CHAR *) m_csOptimizedPacketMemoryPool.Alloc();

	INT32	lItemConvertPacketBufferSize	= 0;
	CHAR	*szItemConvertPacketBuffer		= (CHAR *) m_csOptimizedPacketMemoryPool.Alloc();

	MakeBufferCharAllItem(pcsCharacter, szItemPacketBuffer, &lItemPacketBufferSize, szItemConvertPacketBuffer, &lItemConvertPacketBufferSize);

	// make all skill packet
	INT32	lSkillPacketBufferSize	= 0;
	CHAR	*szSkillPacketBuffer		= (CHAR *) m_csOptimizedPacketMemoryPool.Alloc();
	if (szSkillPacketBuffer)
	{
		//ZeroMemory(szItemPacketBuffer, sizeof(CHAR) * AGPMOPTIMIZEDPACKET2_BUFFER_SIZE);

		lSkillPacketBufferSize	= MakeBufferCharAllSkill(pcsCharacter, szSkillPacketBuffer);
	}

	INT8	cOperation	= AGPM_OPTIMIZEDPACKET2_ADD_CHARACTER;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDVIEW_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											szItemPacketBuffer, &lItemPacketBufferSize,
											szItemConvertPacketBuffer, &lItemConvertPacketBufferSize,
											szSkillPacketBuffer, &lSkillPacketBufferSize,
											NULL,
											NULL,	/* GuildMarkTID */
											NULL,	/* GuildMarkColor */
											NULL,	/* IsWinner */
											NULL	/* Title */
											);

	if (0 >= *pnPacketLength || *pnPacketLength >= APPACKET_MAX_PACKET_SIZE)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] packet length is %d in MakePacketCharAllItemSkillExceptBankCash()\n", pcsCharacter->m_szID, *pnPacketLength);
		AuLogFile_s("LOG\\OptimizedPacketError.txt", strCharBuff);
	}

	m_csOptimizedPacketMemoryPool.Free(szItemPacketBuffer);
	m_csOptimizedPacketMemoryPool.Free(szItemConvertPacketBuffer);
	m_csOptimizedPacketMemoryPool.Free(szSkillPacketBuffer);

//	PVOID	pvBuffer	= m_csOptimizedPacketMemoryPool.Alloc();
//
//	UINT	nBufferSize	= *pnPacketLength;
//	CopyMemory(pvBuffer, pvPacket, *pnPacketLength);
//
//	m_csPacket.m_MiniLZO.Compress((BYTE *) pvBuffer, &nBufferSize);
//
//	m_csOptimizedPacketMemoryPool.Free(pvBuffer);


	return	pvPacket;
}

PVOID AgpmOptimizedPacket2::MakePacketCharAllBankItem(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	// make all item packet
	INT32	lItemPacketBufferSize			= 0;
	CHAR	*szItemPacketBuffer				= (CHAR *) m_csOptimizedPacketMemoryPool.Alloc();

	INT32	lItemConvertPacketBufferSize	= 0;
	CHAR	*szItemConvertPacketBuffer		= (CHAR *) m_csOptimizedPacketMemoryPool.Alloc();

	MakeBufferCharAllBankItem(pcsCharacter, szItemPacketBuffer, &lItemPacketBufferSize, szItemConvertPacketBuffer, &lItemConvertPacketBufferSize);

	INT8	cOperation	= AGPM_OPTIMIZEDPACKET2_ADD_CHARACTER;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDVIEW_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											szItemPacketBuffer, &lItemPacketBufferSize,
											szItemConvertPacketBuffer, &lItemConvertPacketBufferSize,
											NULL, NULL,
											NULL,
											NULL,	/* GuildMarkTID */
											NULL,	/* GuildMarkColor */
											NULL,	/* IsWinner */
											NULL	/* Title */
											);

	if (0 >= *pnPacketLength || *pnPacketLength >= APPACKET_MAX_PACKET_SIZE)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] packet length is %d in MakePacketCharAllBankItem()\n", pcsCharacter->m_szID, *pnPacketLength);
		AuLogFile_s("LOG\\OptimizedPacketError.txt", strCharBuff);
	}

	m_csOptimizedPacketMemoryPool.Free(szItemPacketBuffer);
	m_csOptimizedPacketMemoryPool.Free(szItemConvertPacketBuffer);

	return	pvPacket;
}

PVOID AgpmOptimizedPacket2::MakePacketCharAllCashItem(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, INT32* plItemIndex)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	// make all item packet
	INT32	lItemPacketBufferSize			= 0;
	CHAR	*szItemPacketBuffer				= (CHAR *) m_csOptimizedPacketMemoryPool.Alloc();

	INT32	lItemConvertPacketBufferSize	= 0;
	CHAR	*szItemConvertPacketBuffer		= (CHAR *) m_csOptimizedPacketMemoryPool.Alloc();

	MakeBufferCharAllCashItem(pcsCharacter, szItemPacketBuffer, &lItemPacketBufferSize, szItemConvertPacketBuffer, &lItemConvertPacketBufferSize, plItemIndex);

	INT8	cOperation	= AGPM_OPTIMIZEDPACKET2_ADD_CHARACTER;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMOPTIMIZEDVIEW_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											szItemPacketBuffer, &lItemPacketBufferSize,
											szItemConvertPacketBuffer, &lItemConvertPacketBufferSize,
											NULL, NULL,
											NULL,
											NULL,	/* GuildMarkTID */
											NULL,	/* GuildMarkColor */
											NULL,	/* IsWinner */
											NULL	/* Title */
											);

	if (0 >= *pnPacketLength || *pnPacketLength >= APPACKET_MAX_PACKET_SIZE)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] packet length is %d in MakePacketCharAllCashItem()\n", pcsCharacter->m_szID, *pnPacketLength);
		AuLogFile_s("LOG\\OptimizedPacketError.txt", strCharBuff);
	}

	m_csOptimizedPacketMemoryPool.Free(szItemPacketBuffer);
	m_csOptimizedPacketMemoryPool.Free(szItemConvertPacketBuffer);

	return	pvPacket;
}

BOOL AgpmOptimizedPacket2::SetCallbackMoveActionAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_OPTIMIZEDPACKET2_CB_MOVE_ACTION_ATTACK, pfCallback, pClass);
}

BOOL AgpmOptimizedPacket2::SetCallbackMoveActionSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_OPTIMIZEDPACKET2_CB_MOVE_ACTION_SKILL, pfCallback, pClass);
}

BOOL AgpmOptimizedPacket2::SetCallbackMoveActionRelease(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_OPTIMIZEDPACKET2_CB_MOVE_ACTION_RELEASE, pfCallback, pClass);
}

UINT16 AgpmOptimizedPacket2::GetCharBitFlag(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	UINT16	unBitFlag	= pcsCharacter->m_unEventStatusFlag;

	AgpdBillInfo	*pcsAttachBillInfo	= m_pcsAgpmBillInfo->GetADCharacter(pcsCharacter);

	if (pcsAttachBillInfo->m_bIsPCRoom)
		unBitFlag |= AGPM_CHAR_BIT_FLAG_IS_PC_ROOM;

	return unBitFlag;
}

BOOL AgpmOptimizedPacket2::SetCharBitFlag(AgpdCharacter *pcsCharacter, UINT16 unBitFlag)
{
	if (!pcsCharacter)
		return FALSE;

	if (unBitFlag & AGPM_CHAR_BIT_FLAG_IS_PC_ROOM)
	{
		m_pcsAgpmBillInfo->UpdateIsPCRoom(pcsCharacter, TRUE);
	}

	return TRUE;
}
