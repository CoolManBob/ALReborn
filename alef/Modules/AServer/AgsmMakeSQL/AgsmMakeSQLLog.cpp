// AgsmMakeSQLLog.cpp
// 2004.04.29. steeple
// Log Query 를 남긴다.

#include "AgsmMakeSQL.h"

BOOL AgsmMakeSQL::Log_Loginout(AgpdLog_Loginout* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "INSERT INTO connlog(accountid, charid, flag, playtime) "
						"VALUES('%s', %d, %d, %d)",
						pcsAgpdLog->m_szAccName,
						pcsAgpdLog->m_szCharName,
						pcsAgpdLog->m_cFlag,
						pcsAgpdLog->m_lPlayTime);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::Log_Combat(AgpdLog_Combat* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;
	
	sprintf(pSQLBuffer->Buffer, "INSERT INTO combatlog(charid, chartid, charlv, target, targettid, targetlv, flag, exp, party, attack, skill, combattime, "
						"hp1, hp2, hp3, mp1, mp2, mp3, sp1, sp2, sp3) "
						"VALUES('%s', %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
						pcsAgpdLog->m_szCharName,
						pcsAgpdLog->m_lCharTID,
						pcsAgpdLog->m_lCharLevel,
						pcsAgpdLog->m_szTargetName,
						pcsAgpdLog->m_lTargetTID,
						pcsAgpdLog->m_lTargetLevel,
						(INT8)pcsAgpdLog->m_bWin,
						pcsAgpdLog->m_lExp,
						(INT8)pcsAgpdLog->m_bParty,
						pcsAgpdLog->m_lAttackCount,
						pcsAgpdLog->m_lSkillUseCount,
						pcsAgpdLog->m_lCombatTime,
						pcsAgpdLog->m_nArrHPPotion[0],
						pcsAgpdLog->m_nArrHPPotion[1],
						pcsAgpdLog->m_nArrHPPotion[2],
						pcsAgpdLog->m_nArrMPPotion[0],
						pcsAgpdLog->m_nArrMPPotion[1],
						pcsAgpdLog->m_nArrMPPotion[2],
						pcsAgpdLog->m_nArrSPPotion[0],
						pcsAgpdLog->m_nArrSPPotion[1],
						pcsAgpdLog->m_nArrSPPotion[2]);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::Log_SkillPoint(AgpdLog_SkillPoint* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;
	
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "INSERT INTO skilluplog(charid, chartid, charlv, unusedpoint, mastery1, mastery2, mastery3, mastery4, mastery5, mastery6) "
						"VALUES('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",
						pcsAgpdLog->m_szCharName, 
						pcsAgpdLog->m_lCharTID,
						pcsAgpdLog->m_lCharLevel,
						pcsAgpdLog->m_lRemainPoint,
						pcsAgpdLog->m_arrMasteryPoint[0],
						pcsAgpdLog->m_arrMasteryPoint[1],
						pcsAgpdLog->m_arrMasteryPoint[2],
						pcsAgpdLog->m_arrMasteryPoint[3],
						pcsAgpdLog->m_arrMasteryPoint[4],
						pcsAgpdLog->m_arrMasteryPoint[5]);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::Log_SkillUse(AgpdLog_SkillUse* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "INSERT INTO skilluselog(charid, chartid, charlv, skilltid, count) "
						"VALUES('%s', %d, %d, %d, %d)",
						pcsAgpdLog->m_szCharName,
						pcsAgpdLog->m_lCharTID,
						pcsAgpdLog->m_lCharLevel,
						pcsAgpdLog->m_lSkillTID,
						pcsAgpdLog->m_lCount);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::Log_ItemPickUse(AgpdLog_ItemPickUse* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "INSERT INTO itemuselog(charid, chartid, charlv, itemseq, itemtid, flag) "
						"VALUES('%s', %d, %d, %I64d, %d, %d)",
						pcsAgpdLog->m_szCharName,
						pcsAgpdLog->m_lCharTID,
						pcsAgpdLog->m_lCharLevel,
						pcsAgpdLog->m_lItemDBID,
						pcsAgpdLog->m_lItemTID,
						pcsAgpdLog->m_cFlag);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::Log_ItemTrade(AgpdLog_ItemTrade* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "INSERT INTO itemtradelog(sellcharid, buycharid, itemseq, itemtid, itemcount) "
						"VALUES('%s', '%s', %I64d, %d, %d)",
						pcsAgpdLog->m_szCharName,
						pcsAgpdLog->m_szBuyerName,
						pcsAgpdLog->m_lItemDBID,
						pcsAgpdLog->m_lItemTID,
						pcsAgpdLog->m_lCount);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::Log_ItemConvert(AgpdLog_ItemConvert* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "INSERT INTO itemconvertlog(charid, chartid, charlv, itemseq, itemtid, convertlevel, converttid, result) "
						"VALUES('%s', %d, %d, %I64d, %d, %d, %d, %d)",
						pcsAgpdLog->m_szCharName,
						pcsAgpdLog->m_lCharTID,
						pcsAgpdLog->m_lCharLevel,
						pcsAgpdLog->m_lItemDBID,
						pcsAgpdLog->m_lItemTID,
						pcsAgpdLog->m_cConvertLevel,
						pcsAgpdLog->m_lConvertTID,
						pcsAgpdLog->m_cResult);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::Log_ItemRepair(AgpdLog_ItemRepair* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "INSERT INTO itemrepairlog(charid, chartid, charlv, itemseq, itemtid, money) "
						"VALUES('%s', %d, %d, %I64d, %d, %d)",
						pcsAgpdLog->m_szCharName,
						pcsAgpdLog->m_lCharTID,
						pcsAgpdLog->m_lCharLevel,
						pcsAgpdLog->m_lItemDBID,
						pcsAgpdLog->m_lItemTID,
						pcsAgpdLog->m_lMoney);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::Log_LevelUp(AgpdLog_LevelUp* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	//sprintf(pSQLBuffer->Buffer, "INSERT INTO leveluplog(charid, chartid, lvfrom, lvto, partytime, solotime, huntcount, deathcount) "
	//					"VALUES('%s', %d, %d, %d, %d, %d, %d, %d)",
	//					pcsAgpdLog->m_szCharName,
	//					pcsAgpdLog->m_lCharTID,
	//					pcsAgpdLog->m_lLevelFrom,
	//					pcsAgpdLog->m_lLevelTo,
	//					pcsAgpdLog->m_lPartyTime,
	//					pcsAgpdLog->m_lSoloTime,
	//					pcsAgpdLog->m_lHuntCount,
	//					pcsAgpdLog->m_lDeathCount);

	sprintf(pSQLBuffer->Buffer,
					"MERGE INTO leveluplog t1 USING "
					"(SELECT '%s' AS charid, %d AS chartid, %d AS lvfrom, %d AS lvto, %d AS partytime, %d AS solotime, %d AS huntcount, %d AS deathcount "
					"FROM dual) t2 "
					"ON (t1.charid=t2.charid AND t1.lvfrom=t2.lvfrom) "
					"WHEN MATCHED THEN "
					"UPDATE SET t1.partytime = t2.partytime + t1.partytime, t1.solotime = t2.solotime + t1.solotime, "
					"t1.huntcount = t2.huntcount + t1.huntcount, t1.deathcount = t2.deathcount + t1.deathcount, t1.lvto = t2.lvto "
					"WHEN NOT MATCHED THEN "
					"INSERT (t1.charid, t1.chartid, t1.lvfrom, t1.lvto, t1.partytime, t1.solotime, t1.huntcount, t1.deathcount) "
					"VALUES (t2.charid, t2.chartid, t2.lvfrom, t2.lvto, t2.partytime, t2.solotime, t2.huntcount, t2.deathcount)",
					pcsAgpdLog->m_szCharName,
					pcsAgpdLog->m_lCharTID,
					pcsAgpdLog->m_lLevelFrom,
					pcsAgpdLog->m_lLevelTo,
					pcsAgpdLog->m_lPartyTime,
					pcsAgpdLog->m_lSoloTime,
					pcsAgpdLog->m_lHuntCount,
					pcsAgpdLog->m_lDeathCount);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	OutputDebugString("\n");
	OutputDebugString(pSQLBuffer->Buffer);
	OutputDebugString("\n");

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::Log_ItemOwnerChange(AgpdLog_ItemOwnerChange* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer,
					"INSERT INTO itemownerchangelog(newcharid, prevcharid, itemseq, itemtid, itemcount) "
					"VALUES('%s', '%s', %I64d, %d, %d)",
					pcsAgpdLog->m_szCharName,
					pcsAgpdLog->m_szPrevOwner,
					pcsAgpdLog->m_llItemDBID,
					pcsAgpdLog->m_lItemTID,
					pcsAgpdLog->m_lCount
					);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::CBLog(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdLog* pcsAgpdLog = (AgpdLog*)pData;
	AgsmMakeSQL* pThis = (AgsmMakeSQL*)pClass;

	if(!pcsAgpdLog || !pThis)
		return FALSE;

	switch(pcsAgpdLog->m_cLogType)
	{
		case AGPDLOGTYPE_LOGINOUT:
			pThis->Log_Loginout((AgpdLog_Loginout*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_COMBAT:
			pThis->Log_Combat((AgpdLog_Combat*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_SKILL_POINT:
			pThis->Log_SkillPoint((AgpdLog_SkillPoint*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_SKILL_USE:
			pThis->Log_SkillUse((AgpdLog_SkillUse*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_ITEM_PICKUSE:
			pThis->Log_ItemPickUse((AgpdLog_ItemPickUse*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_ITEM_TRADE:
			pThis->Log_ItemTrade((AgpdLog_ItemTrade*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_ITEM_CONVERT:
			pThis->Log_ItemConvert((AgpdLog_ItemConvert*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_ITEM_REPAIR:
			pThis->Log_ItemRepair((AgpdLog_ItemRepair*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_LEVEL_UP:
			pThis->Log_LevelUp((AgpdLog_LevelUp*)pcsAgpdLog);
			break;
			
		case AGPDLOGTYPE_ITEM_OWNERCHANGE:
			pThis->Log_ItemOwnerChange((AgpdLog_ItemOwnerChange*)pcsAgpdLog);
			break;
	}

	return TRUE;
}