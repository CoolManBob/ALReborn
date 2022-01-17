// AgsmMakeSQL.cpp: implementation of the AgsmMakeSQL class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmMakeSQL.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmMakeSQL::AgsmMakeSQL()
{
	SetModuleName("AgsmMakeSQL");
}

AgsmMakeSQL::~AgsmMakeSQL()
{

}

BOOL AgsmMakeSQL::OnAddModule()
{
	m_pAgsmLog = (AgsmLog*)GetModule("AgsmLog");
	m_pAgsmAdmin = (AgsmAdmin*)GetModule("AgsmAdmin");
	m_pAgsmDBPool	= (AgsmDBPool*)GetModule("AgsmDBPool");
	m_pAgsmRelay	= (AgsmRelay*)GetModule("AgsmRelay");

	if (!m_pAgsmLog || !m_pAgsmAdmin || !m_pAgsmDBPool || !m_pAgsmRelay) 
		return FALSE;

	if(!m_pAgsmLog->SetCallbackWriteLog(CBLog, this))	// 2004.04.29. steeple
		return FALSE;

	if(!m_pAgsmAdmin->SetCallbackUpdateCharacterBan(CBCharacterBan, this))	// 2004.03.30. steeple
		return FALSE;

	/*
	if (!m_pAgsmRelay->SetCallbackCharacter(CBCharacter, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackItem(CBItem, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackItemConvertHistory(CBItemConvertHistory, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackSkill(CBSkill, this))
		return FALSE;
	*/

	if (!m_pAgsmRelay->SetCallbackCharacter2(CBCharacter2, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackItem2(CBItem2, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackItemConvertHistory2(CBItemConvertHistory2, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackSkill2(CBSkill2, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackUIStatus2(CBUIStatus2, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackGuildMaster2(CBGuildMaster2, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackGuildMember2(CBGuildMember2, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackAccountWorld(CBAccountWorld2, this))
		return FALSE;
	if (!m_pAgsmRelay->SetCallbackSearchCharacter(CBSearchCharacter, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmMakeSQL::OnInit()
{
	return TRUE;
}

BOOL AgsmMakeSQL::OnDestroy()
{
	return TRUE;
}

/*
BOOL AgsmMakeSQL::CBCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	INT32 lDBOperation	= (INT32)pCustData;
	AgsmMakeSQL* pThis	= (AgsmMakeSQL*)pClass;
	AgsdRelayCharacter* pAgsdRelayCharacter	= (AgsdRelayCharacter*)pData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_INSERT:	pThis->CharacterInsert(pAgsdRelayCharacter);	break;
	case AGSMRELAY_DB_OPERATION_UPDATE:	pThis->CharacterUpdate(pAgsdRelayCharacter);	break;
	case AGSMRELAY_DB_OPERATION_DELETE:	pThis->CharacterDelete(pAgsdRelayCharacter);	break;
	}
	
	return TRUE;
}

BOOL AgsmMakeSQL::CBItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	INT32 lDBOperation	= (INT32)pCustData;
	AgsmMakeSQL* pThis	= (AgsmMakeSQL*)pClass;
	AgsdRelayItem* pAgsdRelayItem	= (AgsdRelayItem*)pData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_INSERT:	pThis->ItemInsert(pAgsdRelayItem);	break;
	case AGSMRELAY_DB_OPERATION_UPDATE:	pThis->ItemUpdate(pAgsdRelayItem);	break;
	case AGSMRELAY_DB_OPERATION_DELETE:	pThis->ItemDelete(pAgsdRelayItem);	break;
	}

	return TRUE;
}

BOOL AgsmMakeSQL::CBItemConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmMakeSQL					*pThis						= (AgsmMakeSQL *)					pClass;
	AgsdRelayItemConvertHistory	*pstRelayItemConvertHistory	= (AgsdRelayItemConvertHistory *)	pData;
	INT32						lDBOperation				= (INT32)							pCustData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_INSERT:	pThis->ItemConvertHistoryInsert(pstRelayItemConvertHistory);	break;
	case AGSMRELAY_DB_OPERATION_DELETE:	pThis->ItemConvertHistoryDelete(pstRelayItemConvertHistory);	break;
	}

	return TRUE;
}

BOOL AgsmMakeSQL::CBSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	INT32 lDBOperation	= (INT32)pCustData;
	AgsmMakeSQL* pThis	= (AgsmMakeSQL*)pClass;
	AgsdRelaySkill* pAgsdRelaySkill	= (AgsdRelaySkill*)pData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_INSERT:	pThis->SkillInsert(pAgsdRelaySkill);	break;
	case AGSMRELAY_DB_OPERATION_UPDATE:	pThis->SkillUpdate(pAgsdRelaySkill);	break;
	case AGSMRELAY_DB_OPERATION_DELETE:	pThis->SkillDelete(pAgsdRelaySkill);	break;
	}

	return TRUE;
}
*/

BOOL AgsmMakeSQL::CBCharacter2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	INT32 lDBOperation	= (INT32)pCustData;
	AgsmMakeSQL* pThis	= (AgsmMakeSQL*)pClass;
	AgsdRelayCharacter2* pAgsdRelayCharacter	= (AgsdRelayCharacter2*)pData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_INSERT:	pThis->CharacterInsert3(pAgsdRelayCharacter);	break;
	case AGSMRELAY_DB_OPERATION_UPDATE:	pThis->CharacterUpdate3(pAgsdRelayCharacter);	break;
	case AGSMRELAY_DB_OPERATION_DELETE:	pThis->CharacterDelete3(pAgsdRelayCharacter);	break;
	}
	
	return TRUE;
}

BOOL AgsmMakeSQL::CBItem2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	INT32 lDBOperation	= (INT32)pCustData;
	AgsmMakeSQL* pThis	= (AgsmMakeSQL*)pClass;
	AgsdRelayItem2* pAgsdRelayItem	= (AgsdRelayItem2*)pData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_INSERT:	pThis->ItemInsert3(pAgsdRelayItem);	break;
	case AGSMRELAY_DB_OPERATION_UPDATE:	pThis->ItemUpdate3(pAgsdRelayItem);	break;
	case AGSMRELAY_DB_OPERATION_DELETE:	pThis->ItemDelete3(pAgsdRelayItem);	break;
	}

	return TRUE;
}

BOOL AgsmMakeSQL::CBItemConvertHistory2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmMakeSQL						*pThis						= (AgsmMakeSQL *)					pClass;
	AgsdRelayItemConvertHistory2	*pstRelayItemConvertHistory	= (AgsdRelayItemConvertHistory2 *)	pData;
	INT32							lDBOperation				= (INT32)							pCustData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_UPDATE: pThis->ItemConvertHistoryUpdate(pstRelayItemConvertHistory);	break;
	}

	return TRUE;
}

BOOL AgsmMakeSQL::CBSkill2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	INT32 lDBOperation	= (INT32)pCustData;
	AgsmMakeSQL* pThis	= (AgsmMakeSQL*)pClass;
	AgsdRelaySkill2* pAgsdRelaySkill	= (AgsdRelaySkill2*)pData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_INSERT:	pThis->SkillInsert3(pAgsdRelaySkill);	break;
	case AGSMRELAY_DB_OPERATION_UPDATE:	pThis->SkillUpdate3(pAgsdRelaySkill);	break;
	case AGSMRELAY_DB_OPERATION_DELETE:	pThis->SkillDelete3(pAgsdRelaySkill);	break;
	}

	return TRUE;
}

BOOL AgsmMakeSQL::CBUIStatus2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	INT32 lDBOperation	= (INT32)pCustData;
	AgsmMakeSQL* pThis	= (AgsmMakeSQL*)pClass;
	AgsdRelayUIStatus2* pAgsdRelayUIStatus	= (AgsdRelayUIStatus2*)pData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_UPDATE:	pThis->UIStatusUpdate3(pAgsdRelayUIStatus);	break;
	}

	return TRUE;
}

BOOL AgsmMakeSQL::CBAccountWorld2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	INT32 lDBOperation	= (INT32)pCustData;
	AgsmMakeSQL* pThis	= (AgsmMakeSQL*)pClass;
	AgsdRelayAccountWorld* pAgsdRelayAccountWorld	= (AgsdRelayAccountWorld*)pData;

	switch(lDBOperation)
	{
	case AGSMRELAY_DB_OPERATION_UPDATE:	pThis->AccountWorldUpdate(pAgsdRelayAccountWorld);	break;
	}

	return TRUE;
}

/*
BOOL AgsmMakeSQL::ItemInsert(AgsdRelayItem *pAgsdRelayItem)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "insert into item (id, owner_id, template_id, stack_count, status, pos_x, pos_y, pos_z, "
								"grid_pos_t, grid_pos_r, grid_pos_c) "
								"values (%I64d, '%s', %d, %d, %d, %f, %f, %f, %d, %d, %d)",
								pAgsdRelayItem->lDBID,
								pAgsdRelayItem->pszCharName,
								pAgsdRelayItem->lTemplateID,
								pAgsdRelayItem->lStackCount,
								pAgsdRelayItem->lStatus,
								pAgsdRelayItem->stPosition.x,
								pAgsdRelayItem->stPosition.y,
								pAgsdRelayItem->stPosition.z,
								pAgsdRelayItem->lGridTab,
								pAgsdRelayItem->lGridRow,
								pAgsdRelayItem->lGridColumn);
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);	
}

BOOL AgsmMakeSQL::ItemUpdate(AgsdRelayItem *pAgsdRelayItem)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "update item set "
								"owner_id='%s', "
								"template_id=%d, "
								"stack_count=%d, "
								"status=%d, "
								"pos_x=%f, "
								"pos_y=%f, "
								"pos_z=%f, "
								"grid_pos_t=%d, "
								"grid_pos_r=%d, "
								"grid_pos_c=%d "
								"where id=%I64d",
								pAgsdRelayItem->pszCharName,
								pAgsdRelayItem->lTemplateID,
								pAgsdRelayItem->lStackCount,
								pAgsdRelayItem->lStatus,
								pAgsdRelayItem->stPosition.x,
								pAgsdRelayItem->stPosition.y,
								pAgsdRelayItem->stPosition.z,
								pAgsdRelayItem->lGridTab,
								pAgsdRelayItem->lGridRow,
								pAgsdRelayItem->lGridColumn,
								pAgsdRelayItem->lDBID);

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::ItemDelete(AgsdRelayItem *pAgsdRelayItem)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "delete from item where id=%I64d", pAgsdRelayItem->lDBID);
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::ItemConvertHistoryInsert(AgsdRelayItemConvertHistory *pAgsdRelayItemConvertHistory)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "insert into item_convert_history (id, item_id, type, attr, value) values (seq_item_conv_id.nextval, %I64d, %d, %d, %d)",
								pAgsdRelayItemConvertHistory->ullDBID,
								pAgsdRelayItemConvertHistory->cType,
								pAgsdRelayItemConvertHistory->lAttribute,
								pAgsdRelayItemConvertHistory->lValue);
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::ItemConvertHistoryDelete(AgsdRelayItemConvertHistory *pAgsdRelayItemConvertHistory)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "delete from item_convert_history where id=%I64d and type=%d and attr=%d",
								pAgsdRelayItemConvertHistory->ullDBID,
								pAgsdRelayItemConvertHistory->cType,
								pAgsdRelayItemConvertHistory->lAttribute);
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}
*/

BOOL AgsmMakeSQL::ItemInsert3(AgsdRelayItem2 *pAgsdRelayItem)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "insert into charitem (itemseq, charid, itemtid, stackcount, status, pos, durability) "
								"values (%I64d, '%s', %d, %d, %d, '%s', %d)",
								pAgsdRelayItem->lDBID,
								pAgsdRelayItem->pszCharName,
								pAgsdRelayItem->lTemplateID,
								pAgsdRelayItem->lStackCount,
								pAgsdRelayItem->lStatus,
								pAgsdRelayItem->pszPosition,
								pAgsdRelayItem->lDurability);
	
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);	
}

BOOL AgsmMakeSQL::ItemUpdate3(AgsdRelayItem2 *pAgsdRelayItem)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "update charitem set "
								"charid='%s',"
								"stackcount=%d,"
								"status=%d,"
								"pos='%s',"
								"durability=%d "
								"where itemseq=%I64d",
								pAgsdRelayItem->pszCharName,
								pAgsdRelayItem->lStackCount,
								pAgsdRelayItem->lStatus,
								pAgsdRelayItem->pszPosition,
								pAgsdRelayItem->lDurability,
								pAgsdRelayItem->lDBID);

	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::ItemDelete3(AgsdRelayItem2 *pAgsdRelayItem)
{
	SQLBuffer* pSQLBuffer	= new SQLBuffer;

	CHAR	szDeleteReason[AGPMITEM_MAX_DELETE_REASON + 1];
	ZeroMemory(szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));

	// 2004.05.25. steeple.
	if(pAgsdRelayItem->pDeleteReasonString && pAgsdRelayItem->nDeleteReasonLength > 0)
	{
		if (AGPMITEM_MAX_DELETE_REASON > pAgsdRelayItem->nDeleteReasonLength)
			CopyMemory(szDeleteReason, (CHAR *) pAgsdRelayItem->pDeleteReasonString, sizeof(CHAR) * pAgsdRelayItem->nDeleteReasonLength);
		else
			CopyMemory(szDeleteReason, (CHAR *) pAgsdRelayItem->pDeleteReasonString, sizeof(CHAR) * AGPMITEM_MAX_DELETE_REASON);
	}

	sprintf(pSQLBuffer->Buffer, "CALL SPDELETEITEM (%I64d,'%s')", pAgsdRelayItem->lDBID, szDeleteReason);
	
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::ItemConvertHistoryUpdate(AgsdRelayItemConvertHistory2 *pAgsdRelayItemConvertHistory)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	CHAR	szConvertHistory[129];
	ZeroMemory(szConvertHistory, sizeof(CHAR) * 129);

	// 2004.05.25. steeple
	if(pAgsdRelayItemConvertHistory->pConvertHistoryString && pAgsdRelayItemConvertHistory->nStringLength > 0)
	{
		if(128 > pAgsdRelayItemConvertHistory->nStringLength)
			strncpy(szConvertHistory, (CHAR *) pAgsdRelayItemConvertHistory->pConvertHistoryString, sizeof(CHAR) * pAgsdRelayItemConvertHistory->nStringLength);
		else
			strncpy(szConvertHistory, (CHAR *) pAgsdRelayItemConvertHistory->pConvertHistoryString, sizeof(CHAR) * 128);
	}

	sprintf(pSQLBuffer->Buffer, "update charitem set convhist='%s' where itemseq=%I64d",
								szConvertHistory,
								pAgsdRelayItemConvertHistory->ullDBID);
	
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

/*
BOOL AgsmMakeSQL::CharacterInsert(AgsdRelayCharacter* pAgsdRelayCharacter)
{
	return TRUE;
}

BOOL AgsmMakeSQL::CharacterUpdate(AgsdRelayCharacter* pAgsdRelayCharacter)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "update character set "
								"title='%s', "
								"pos_x=%f, "
								"pos_y=%f, "
								"pos_z=%f, "
								"criminal_status=%d, "
								"murder_point=%d, "
								"inven_money=%I64d, "
								"bank_money=%I64d, "
								"rcf_time=%ld, "
								"rmf_time=%ld, "
								"hp=%d, "
								"mp=%d, "
								"sp=%d, "
								"exp=%d, "
								"lv=%d "
								"where name='%s'",
								pAgsdRelayCharacter->pszTitle,
								pAgsdRelayCharacter->stPosition.x,
								pAgsdRelayCharacter->stPosition.y,
								pAgsdRelayCharacter->stPosition.z,
								pAgsdRelayCharacter->lCriminalStatus,
								pAgsdRelayCharacter->lMurderPoint,
								pAgsdRelayCharacter->lInventoryMoney,
								pAgsdRelayCharacter->lBankMoney,
								pAgsdRelayCharacter->ulRemainCriminalTime,
								pAgsdRelayCharacter->ulRemainMurderTime,
								pAgsdRelayCharacter->lHP,
								pAgsdRelayCharacter->lMP,
								pAgsdRelayCharacter->lSP,
								pAgsdRelayCharacter->lExp,
								pAgsdRelayCharacter->lLevel,
								pAgsdRelayCharacter->pszName);

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::CharacterDelete(AgsdRelayCharacter* pAgsdRelayCharacter)
{
	return TRUE;
}

BOOL AgsmMakeSQL::CharacterSelect(AgsdRelayCharacter* pAgsdRelayCharacter)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "select * from character where name = '%s'", pAgsdRelayCharacter->pszName);

	pSQLBuffer->m_pClass = this;
	pSQLBuffer->m_pfCallback = CBCharacterSelect;
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}
*/

BOOL AgsmMakeSQL::CharacterInsert3(AgsdRelayCharacter2* pAgsdRelayCharacter)
{
	return TRUE;
}

BOOL AgsmMakeSQL::CharacterUpdate3(AgsdRelayCharacter2* pAgsdRelayCharacter)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "update chardetail set "
								"title='%s',"
								"pos='%s',"
								"criminalstatus=%d,"
								"murderpoint=%d,"
								"rcftime=%ld,"
								"rmftime=%ld,"
								"inven_money=%I64d,"
								"hp=%d,"
								"mp=%d,"
								"sp=%d,"
								"exp=%d,"
								"lv=%d,"
								"skillpoint=%d "
								"where charid='%s'",
								pAgsdRelayCharacter->pszTitle,
								pAgsdRelayCharacter->pszPosition,
								pAgsdRelayCharacter->lCriminalStatus,
								pAgsdRelayCharacter->lMurderPoint,
								pAgsdRelayCharacter->ulRemainCriminalTime,
								pAgsdRelayCharacter->ulRemainMurderTime,
								pAgsdRelayCharacter->lInventoryMoney,
								pAgsdRelayCharacter->lHP,
								pAgsdRelayCharacter->lMP,
								pAgsdRelayCharacter->lSP,
								pAgsdRelayCharacter->lExp,
								pAgsdRelayCharacter->lLevel,
								pAgsdRelayCharacter->lSkillPoint,
								pAgsdRelayCharacter->pszName);

	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::CharacterDelete3(AgsdRelayCharacter2* pAgsdRelayCharacter)
{
	return TRUE;
}

BOOL AgsmMakeSQL::CharacterSelect3(AgsdRelayCharacter2* pAgsdRelayCharacter)
{
	return TRUE;
}

BOOL AgsmMakeSQL::AccountWorldUpdate(AgsdRelayAccountWorld* pstAccountWorld)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "update accountworld set "
								"bankmoney=%I64d "
								"where accountid='%s'",
								pstAccountWorld->m_llBankMoney,
								pstAccountWorld->m_szAccountID);

	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

/*
// 2004.03.30. steeple
BOOL AgsmMakeSQL::CharacterBanUpdate(stAgpdAdminBan* pstBan)
{
	if(!pstBan)
		return FALSE;

	// 캐릭터 쪽
	SQLBuffer* pSQLBuffer = new SQLBuffer;
	sprintf(pSQLBuffer->Buffer, "UPDATE character SET "
								"chatban_starttime=%d, "
								"chatban_keeptime=%d, "
								"charban_starttime=%d, "
								"charban_keeptime=%d "
								"WHERE name='%s'",
								pstBan->m_lChatBanStartTime,
								pstBan->m_lChatBanKeepTime,
								pstBan->m_lCharBanStartTime,
								pstBan->m_lCharBanKeepTime,
								pstBan->m_szCharName
								);
	m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);

	// 계정 쪽
	// ...
	// ...

	return TRUE;
}
*/

BOOL AgsmMakeSQL::CharacterBanUpdate3(stAgpdAdminBan* pstBan)
{
	if(!pstBan)
		return FALSE;

	// 캐릭터 쪽
	SQLBuffer* pSQLBuffer = new SQLBuffer;
	sprintf(pSQLBuffer->Buffer, "UPDATE chardetail SET "
								"chatblockstart=%d, "
								"chatblockkeep=%d, "
								"connblockstart=%d, "
								"connblockkeep=%d "
								"WHERE charid='%s'",
								pstBan->m_lChatBanStartTime,
								pstBan->m_lChatBanKeepTime,
								pstBan->m_lCharBanStartTime,
								pstBan->m_lCharBanKeepTime,
								pstBan->m_szCharName
								);
	
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);

	// 계정 쪽 - ArchWeb 에 있는 amt_master 를 사용한다.
	char* szCode = "";
	char* szBlockDate = "";	// 현재(2004.05.18. 클베 때) 사용하지 않는다.
	char* szBlock = "Y";
	if(pstBan->m_lAccBanStartTime && pstBan->m_lAccBanKeepTime)
	{
		szCode = "TSXX";	// 현재(2004.05.18. 클베 때) 는 자격박탈만 있다.
		szBlock = "N";
	}

	SQLBuffer* pSQLBuffer2 = new SQLBuffer;
	sprintf(pSQLBuffer2->Buffer, "UPDATE archlord.amt_master SET "
								"penalty_gbn='%s', "
								"tester_yn='%s' "
								"WHERE accountid='%s'",
								szCode,
								szBlock,
								pstBan->m_szAccName
								);

	ASSERT(strlen(pSQLBuffer2->Buffer) < sizeof(pSQLBuffer2->Buffer));
	
	m_pAgsmDBPool->ExecuteSQL(pSQLBuffer2);

	return TRUE;
}

/*
BOOL AgsmMakeSQL::CBCharacterSelect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	auRowset *pRowset = (auRowset*)pData;
	AgsmMakeSQL *pThis = (AgsmMakeSQL*)pClass;

	AgsdRelayCharacter csRelayCharacter;
	
	//csRelayCharacter.lDBID					= *((INT64 *) pRowset->Get(0, 0));
	csRelayCharacter.pszName				= (CHAR*)pRowset->Get(0, 1);
	csRelayCharacter.lTemplateID			= *(INT32*)pRowset->Get(0, 2);
	csRelayCharacter.pszAccountID			= (CHAR*)pRowset->Get(0, 3);
	csRelayCharacter.pszTitle				= (CHAR*)pRowset->Get(0, 4);
	csRelayCharacter.stPosition.x			= *(FLOAT*)(pRowset->Get(0, 5));
	csRelayCharacter.stPosition.y			= *(FLOAT*)(pRowset->Get(0, 6));
	csRelayCharacter.stPosition.z			= *(FLOAT*)(pRowset->Get(0, 7));
	csRelayCharacter.lCriminalStatus		= *(INT8*)(pRowset->Get(0, 8));
	csRelayCharacter.lMurderPoint			= *(INT32*)(pRowset->Get(0, 9));
	csRelayCharacter.lInventoryMoney		= *(INT64*)(pRowset->Get(0, 10));
	csRelayCharacter.lBankMoney				= *(INT64*)(pRowset->Get(0, 11));
	csRelayCharacter.ulRemainCriminalTime	= *(UINT32*)(pRowset->Get(0, 12));
	csRelayCharacter.ulRemainMurderTime		= *(UINT32*)(pRowset->Get(0, 13));
	csRelayCharacter.lHP					= *(INT32*)(pRowset->Get(0, 14));
	csRelayCharacter.lMP					= *(INT32*)(pRowset->Get(0, 15));
	csRelayCharacter.lSP					= *(INT32*)(pRowset->Get(0, 16));
	csRelayCharacter.lExp					= *(INT32*)(pRowset->Get(0, 17));
	csRelayCharacter.lLevel					= *(INT32*)(pRowset->Get(0, 18));

	return pThis->m_pAgsmRelay->SendResultCharacter(&csRelayCharacter);
}

BOOL AgsmMakeSQL::CBItemSelect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	auRowset *pRowset = (auRowset*)pData;
	AgsmMakeSQL *pThis = (AgsmMakeSQL*)pClass;

	return TRUE;
}

BOOL AgsmMakeSQL::CBSkillSelect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	auRowset *pRowset = (auRowset*)pData;
	AgsmMakeSQL *pThis = (AgsmMakeSQL*)pClass;

	return TRUE;
}
*/

/*
BOOL AgsmMakeSQL::SkillInsert(AgsdRelaySkill* pAgsdRelaySkill)
{
	return TRUE;
}

BOOL AgsmMakeSQL::SkillUpdate(AgsdRelaySkill* pAgsdRelaySkill)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "update skill_mast set "
								"total_sp=%d, "
								"skill_seq='%s', "
								"inst_spec=%d, "
								"inst_spec_time=%ld, "
								"const_spec=%d "
								"where owner_id='%s' and mastery=%d",
								pAgsdRelaySkill->lTotalSkillPoint,
								pAgsdRelaySkill->pszTreeNode,
								pAgsdRelaySkill->lInstSpec,
								pAgsdRelaySkill->ulRemainInstSepc,
								pAgsdRelaySkill->lConstSpec,
								pAgsdRelaySkill->pszCharName,
								pAgsdRelaySkill->lMasteryIndex
								);

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::SkillDelete(AgsdRelaySkill* pAgsdRelaySkill)
{
	return TRUE;
}
*/

BOOL AgsmMakeSQL::SkillInsert3(AgsdRelaySkill2* pAgsdRelaySkill)
{
	return TRUE;
}

BOOL AgsmMakeSQL::SkillUpdate3(AgsdRelaySkill2* pAgsdRelaySkill)
{
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "update charskill set "
//								"totalsp=%d,"
								"skillseq='%s',"
								"spec='%s' "
//								"where charid='%s'/*  and mastery=%d*/",
								"where charid='%s'",
//								pAgsdRelaySkill->lTotalSkillPoint,
								pAgsdRelaySkill->pszTreeNode,
								pAgsdRelaySkill->pszSpecialize,
								pAgsdRelaySkill->pszCharName//,
//								pAgsdRelaySkill->lMasteryIndex
								);

	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::SkillDelete3(AgsdRelaySkill2* pAgsdRelaySkill)
{
	return TRUE;
}

BOOL AgsmMakeSQL::UIStatusUpdate3(AgsdRelayUIStatus2* pAgsdRelayUIStatus)
{
	if (!pAgsdRelayUIStatus)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	CHAR szQBeltString[257];
	memset(szQBeltString, 0, sizeof(CHAR) * 257);

	// 2004.05.25. steeple
	// QBeltString 은 NULL 일 수도 있다.
	if(pAgsdRelayUIStatus->pszQBeltString && pAgsdRelayUIStatus->nStringLength > 0)
	{
		if(257 > pAgsdRelayUIStatus->nStringLength)
			strncpy(szQBeltString, pAgsdRelayUIStatus->pszQBeltString, pAgsdRelayUIStatus->nStringLength);
		else
			strncpy(szQBeltString, pAgsdRelayUIStatus->pszQBeltString, 256);
	}

	sprintf(pSQLBuffer->Buffer, "update chardetail set "
								"quickbelt='%s' "
								"where charid='%s'",
								szQBeltString,
								pAgsdRelayUIStatus->pszCharName
								);

	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

// 2004.03.30. steeple
BOOL AgsmMakeSQL::CBCharacterBan(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminBan* pstBan = (stAgpdAdminBan*)pData;
	AgsmMakeSQL* pThis = (AgsmMakeSQL*)pClass;

	if(!pstBan || !pThis)
		return FALSE;

	return pThis->CharacterBanUpdate3(pstBan);
}