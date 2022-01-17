/*====================================================================

	AgsmRelay2Log.cpp

====================================================================*/

#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"
#include <time.h>

CHAR g_szACNone[5] = _T("NONE");

//
//	==========		Log		==========
//
BOOL AgsmRelay2::CBOperationLog(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgpdLog *pAgpdLog = (AgpdLog *) pData;
	AgsmRelay2 *pThis =	(AgsmRelay2 *) pClass;

	if(!pAgpdLog || !pThis)
		return FALSE;

	AgsdRelay2Log *pAgsdRelay2 = NULL;
	INT16 nParam;

	switch (pAgpdLog->m_cLogType)
		{
		case AGPDLOG_CATEGORY_PLAY:
			pAgsdRelay2 = new AgsdRelay2Log_Play(pAgpdLog);
			nParam = AGSMRELAY_PARAM_LOG_PLAY;
			break;

		case AGPDLOG_CATEGORY_ITEM:
			pAgsdRelay2 = new AgsdRelay2Log_Item(pAgpdLog);
			nParam = AGSMRELAY_PARAM_LOG_ITEM;
			break;				
		
		case AGPDLOG_CATEGORY_CASHITEM:
			pAgsdRelay2 = new AgsdRelay2Log_Item(pAgpdLog);
			nParam = AGSMRELAY_PARAM_LOG_CASHITEM;
			break;
		
		case AGPDLOG_CATEGORY_ETC:
			pAgsdRelay2 = new AgsdRelay2Log_ETC(pAgpdLog);
			nParam = AGSMRELAY_PARAM_LOG_ETC;
			break;		
		
		case AGPDLOG_CATEGORY_GHELD:
			pAgsdRelay2 = new AgsdRelay2Log_Gheld(pAgpdLog);
			nParam = AGSMRELAY_PARAM_LOG_GHELD;
			break;

		case AGPDLOG_CATEGORY_PCROOM:
			pAgsdRelay2 = new AgsdRelay2Log_PCRoom(pAgpdLog);
			nParam = AGSMRELAY_PARAM_LOG_PCROOM;
			break;
		
		default:
			return FALSE;
			break;
		}

	// wdate to string
	AuTimeStamp::ConvertTimeStampToOracleTime(pAgpdLog->m_lTimeStamp, pAgsdRelay2->m_szDate, sizeof(pAgsdRelay2->m_szDate));
	
	// gheld inven
	_i64toa(pAgpdLog->m_llGheldInven, pAgsdRelay2->m_szGheldInven, 10);
	
	// gheld bank
	_i64toa(pAgpdLog->m_llGheldBank, pAgsdRelay2->m_szGheldBank, 10);

	// exp
	_i64toa(pAgpdLog->m_llCharExp, pAgsdRelay2->m_szExp, 10);

	BOOL bFile = FALSE;
	
	if (pThis->m_pAgpmConfig && pThis->m_pAgpmConfig->IsFileLog())
		bFile = TRUE;
	
	if (bFile)
		{
		BOOL bResult = pAgsdRelay2->WriteToFile();
		pAgsdRelay2->Release();
		return bResult;
		}
	else
		return CBOperation(pAgsdRelay2, pThis, (PVOID) nParam);
	//return pThis->OperationLogStatic((AgsdDBParam *)pAgsdRelay2, nParam);
	}




BOOL AgsmRelay2::OperationLogStatic(AgsdDBParam *pAgsdRelay2, INT16 nParam)
	{
	INT16 nQueryIndex = pAgsdRelay2->m_eOperation + nParam;
	INT16 nPoolIndex = 0;

	CHAR *psz = m_pAgsmDatabasePool->m_pAgsmDatabaseConfig->GetQuery(nQueryIndex, &nPoolIndex);

	AgsdQueryStatic* pQuery = new AgsdQueryStatic;
	pAgsdRelay2->SetStaticQuery(pQuery->Get(), psz);
	pAgsdRelay2->Release();

	// 
	pQuery->m_nIndex = 0;

	return m_pAgsmDatabasePool->Execute(pQuery, nPoolIndex);
	}




/********************************************************/
/*		The Implementation of AgsdRelay2Log class		*/
/********************************************************/
//
//
//	==========		Log Play		==========
//
AgsdRelay2Log_Play::AgsdRelay2Log_Play(AgpdLog* pLog)
	{
	ASSERT(NULL != pLog);
	m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	//memcpy(&m_csLog, pLog, sizeof(AgpdLog_Play));
	m_csLog = *static_cast<AgpdLog_Play*>(pLog);
	}


BOOL AgsdRelay2Log_Play::SetParamInsert(AuStatement* pStatement)
	{
	/*
	INTSERT INTO ALT_PLAY (
		ACTCODE, WDATE, IPADDR, ACCOUNTID, CHARID, CHARTID, CHARLV, CHAREXP, GHELDINVEN, GHELDBANK,
		PARTYTIME, SOLOTIME, KILLPC, KILLMON, DEADBYPC, DEADBYMON)
	VALUES (
		:1, TO_DATE(:2, 'YYYY/MM/DD HH:MI:SS'), :3, :4, :5, :6, :7, :8, :9, :10,
		:11, :12, :13, :14, :15, :16)
	*/
	INT16 i = 0;
	pStatement->SetParam(i++, m_csLog.m_szAct[m_csLog.m_cFlag], sizeof(CHAR) * (AGPDLOG_MAX_ACTCODE + 1));
	pStatement->SetParam(i++, m_szDate, sizeof(m_szDate));
	pStatement->SetParam(i++, m_csLog.m_szIP, sizeof(m_csLog.m_szIP));
	pStatement->SetParam(i++, m_csLog.m_szAccountID, sizeof(m_csLog.m_szAccountID));
	pStatement->SetParam(i++, m_csLog.m_szCharID, sizeof(m_csLog.m_szCharID));
	pStatement->SetParam(i++, &m_csLog.m_lCharTID);
	pStatement->SetParam(i++, &m_csLog.m_lCharLevel);
	pStatement->SetParam(i++, m_szExp, sizeof(m_szExp));
	pStatement->SetParam(i++, m_szGheldInven, sizeof(m_szGheldInven));
	pStatement->SetParam(i++, m_szGheldBank, sizeof(m_szGheldBank));
	
	pStatement->SetParam(i++, &m_csLog.m_lCharLevelFrom);
	pStatement->SetParam(i++, &m_csLog.m_lPartyTime);
	pStatement->SetParam(i++, &m_csLog.m_lSoloTime);
	pStatement->SetParam(i++, &m_csLog.m_lKillPC);
	pStatement->SetParam(i++, &m_csLog.m_lKillMonster);
	pStatement->SetParam(i++, &m_csLog.m_lDeadByPC);
	pStatement->SetParam(i++, &m_csLog.m_lDeadByMonster);
	
	return TRUE;
	}


void AgsdRelay2Log_Play::SetStaticQuery(CHAR *pszDest, CHAR *pszFmt)
	{
	if (!pszDest || !pszFmt)
		return;

	//sprintf(pszDest, pszFmt,
	return;
	}


BOOL AgsdRelay2Log_Play::WriteToFile()
	{
	/*CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\PLAYLOG_%04d%02d%02d-%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), 
		"%s\t%s\t%s\t%s\t%s\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
			m_csLog.m_szAct[m_csLog.m_cFlag],
			m_szDate,
			m_csLog.m_szIP,
			m_csLog.m_szAccountID,
			m_csLog.m_szCharID,
			m_csLog.m_lCharTID,
			m_csLog.m_lCharLevel,
			m_szExp,
			m_szGheldInven,
			m_szGheldBank,
		
			m_csLog.m_lCharLevelFrom,
			m_csLog.m_lPartyTime,
			m_csLog.m_lSoloTime,
			m_csLog.m_lKillPC,
			m_csLog.m_lKillMonster,
			m_csLog.m_lDeadByPC,
			m_csLog.m_lDeadByMonster);
	AuLogFile_s(szFile, strCharBuff);*/
	
	return TRUE;
	}




//
//	==========		Log Item		==========
//
AgsdRelay2Log_Item::AgsdRelay2Log_Item(AgpdLog* pLog)
	{
	ASSERT(NULL != pLog);
	m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	//memcpy(&m_csLog, pLog, sizeof(AgpdLog_Item));
	m_csLog = *static_cast<AgpdLog_Item*>(pLog);

	ZeroMemory(m_szItemSeq, sizeof(m_szItemSeq));
	_i64toa(m_csLog.m_ullItemDBID, m_szItemSeq, 10);
	
	ZeroMemory(m_szExpireDate, sizeof(m_szExpireDate));
	AuTimeStamp::ConvertTimeStampToOracleTime(m_csLog.m_lExpireDate, m_szExpireDate, sizeof(m_szExpireDate));
	}


BOOL AgsdRelay2Log_Item::SetParamInsert(AuStatement* pStatement)
	{
	/*
	INTSERT INTO ALT_ITEM (
		ACTCODE, WDATE, IPADDR, ACCOUNTID, CHARID, CHARTID, CHARLV, CHAREXP, GHELDINVEN, GHELDBANK,
		ITEMSEQ, ITEMTID, ITEMQTY, ITEMCONVHIST, ITEMOPTION, GHELD, CHARID2,
		INUSE, USECOUNT, REMAINTIME, EXPIREDATE, DESCR)
	VALUES (
		:1, TO_DATE(:2, 'YYYY/MM/DD HH:MI:SS'), :3, :4, :5, :6, :7, :8, :9, :10,
		:11, :12, :13, :14, :15, :16, :17, :18, :19, :20, TO_DATE(:21, 'YYYY/MM/DD HH:MI:SS')), :21
	*/	
	INT16 i = 0;
	pStatement->SetParam(i++, m_csLog.m_szAct[m_csLog.m_cFlag], sizeof(CHAR) * (AGPDLOG_MAX_ACTCODE + 1));
	pStatement->SetParam(i++, m_szDate, sizeof(m_szDate));
	pStatement->SetParam(i++, m_csLog.m_szIP, sizeof(m_csLog.m_szIP));
	pStatement->SetParam(i++, m_csLog.m_szAccountID, sizeof(m_csLog.m_szAccountID));
	pStatement->SetParam(i++, m_csLog.m_szCharID, sizeof(m_csLog.m_szCharID));
	pStatement->SetParam(i++, &m_csLog.m_lCharTID);
	pStatement->SetParam(i++, &m_csLog.m_lCharLevel);
	pStatement->SetParam(i++, m_szExp, sizeof(m_szExp));
	pStatement->SetParam(i++, m_szGheldInven, sizeof(m_szGheldInven));
	pStatement->SetParam(i++, m_szGheldBank, sizeof(m_szGheldBank));

	pStatement->SetParam(i++, m_szItemSeq, sizeof(m_szItemSeq));
	pStatement->SetParam(i++, &m_csLog.m_lItemTID);
	pStatement->SetParam(i++, &m_csLog.m_lItemQty);
	pStatement->SetParam(i++, m_csLog.m_szConvert, sizeof(m_csLog.m_szConvert));
	pStatement->SetParam(i++, m_csLog.m_szOption, sizeof(m_csLog.m_szOption));
	pStatement->SetParam(i++, &m_csLog.m_lGheld);
	pStatement->SetParam(i++, m_csLog.m_szCharID2, sizeof(m_csLog.m_szCharID2));
	pStatement->SetParam(i++, &m_csLog.m_lInUse);
	pStatement->SetParam(i++, &m_csLog.m_lUseCount);
	pStatement->SetParam(i++, (UINT32*)&m_csLog.m_lRemainTime);
	pStatement->SetParam(i++, m_szExpireDate, sizeof(m_szExpireDate));
	pStatement->SetParam(i++, m_csLog.m_szDescription, sizeof(m_csLog.m_szDescription));
	
	return TRUE;
	}


void AgsdRelay2Log_Item::SetStaticQuery(CHAR *pszDest, CHAR *pszFmt)
	{
	if (!pszDest || !pszFmt)
		return;

	//sprintf(pszDest, pszFmt,
	return;
	}


BOOL AgsdRelay2Log_Item::WriteToFile()
	{
	/*CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\ITEMLOG_%04d%02d%02d-%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff),
			"%s\t%s\t%s\t%s\t%s\t%d\t%d\t%s\t%s\t%s%s\t%d\t%d\t%s\t%s\t%d\t%s\t%d\t%d\t%ld\t%s\t%s\n",
			m_csLog.m_szAct[m_csLog.m_cFlag],
			m_szDate,
			m_csLog.m_szIP,
			m_csLog.m_szAccountID,
			m_csLog.m_szCharID,
			m_csLog.m_lCharTID,
			m_csLog.m_lCharLevel,
			m_szExp,
			m_szGheldInven,
			m_szGheldBank,
		
			m_szItemSeq,
			m_csLog.m_lItemTID,
			m_csLog.m_lItemQty,
			m_csLog.m_szConvert,
			m_csLog.m_szOption,
			m_csLog.m_lGheld,
			m_csLog.m_szCharID2,
			m_csLog.m_lInUse,
			m_csLog.m_lUseCount,
			m_csLog.m_lRemainTime,
			m_szExpireDate,
			m_csLog.m_szDescription);
	AuLogFile_s(szFile, strCharBuff);*/
	
	return TRUE;
	}




//
//	==========		Log ETC		==========
//
AgsdRelay2Log_ETC::AgsdRelay2Log_ETC(AgpdLog* pLog)
	{
	ASSERT(NULL != pLog);
	m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	//memcpy(&m_csLog, pLog, sizeof(AgpdLog_ETC));
	m_csLog = *static_cast<AgpdLog_ETC*>(pLog);
	}


BOOL AgsdRelay2Log_ETC::SetParamInsert(AuStatement* pStatement)
	{
	/*
	INTSERT INTO ALT_PLAY (
		ACTCODE, WDATE, IPADDR, ACCOUNTID, CHARID, CHARTID, CHARLV, CHAREXP, GHELDINVEN, GHELDBANK,
		NUMID, STRID, DESCR, GHELD, CHARID2)
	VALUES (
		:1, TO_DATE(:2, 'YYYY/MM/DD HH:MI:SS'), :3, :4, :5, :6, :7, :8, :9, :10,
		:11, :12, :13, :14, :15)
	*/
		
	INT16 i = 0;
	pStatement->SetParam(i++, m_csLog.m_szAct[m_csLog.m_cFlag], sizeof(CHAR) * (AGPDLOG_MAX_ACTCODE + 1));
	pStatement->SetParam(i++, m_szDate, sizeof(m_szDate));
	pStatement->SetParam(i++, m_csLog.m_szIP, sizeof(m_csLog.m_szIP));
	pStatement->SetParam(i++, m_csLog.m_szAccountID, sizeof(m_csLog.m_szAccountID));
	pStatement->SetParam(i++, m_csLog.m_szCharID, sizeof(m_csLog.m_szCharID));
	pStatement->SetParam(i++, &m_csLog.m_lCharTID);
	pStatement->SetParam(i++, &m_csLog.m_lCharLevel);
	pStatement->SetParam(i++, m_szExp, sizeof(m_szExp));
	pStatement->SetParam(i++, m_szGheldInven, sizeof(m_szGheldInven));
	pStatement->SetParam(i++, m_szGheldBank, sizeof(m_szGheldBank));

	pStatement->SetParam(i++, &m_csLog.m_lNumID);
	pStatement->SetParam(i++, m_csLog.m_szStrID, sizeof(m_csLog.m_szStrID));
	pStatement->SetParam(i++, m_csLog.m_szDescription, sizeof(m_csLog.m_szDescription));
	pStatement->SetParam(i++, &m_csLog.m_lGheld);
	pStatement->SetParam(i++, m_csLog.m_szCharID2, sizeof(m_csLog.m_szCharID2));
	
	return TRUE;
	}


void AgsdRelay2Log_ETC::SetStaticQuery(CHAR *pszDest, CHAR *pszFmt)
	{
	if (!pszDest || !pszFmt)
		return;

	//sprintf(pszDest, pszFmt,
	return;
	}


BOOL AgsdRelay2Log_ETC::WriteToFile()
	{
	/*CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\ETCLOG_%04d%02d%02d-%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), 
			"%s\t%s\t%s\t%s\t%s\t%d\t%d\t%s\t%s\t%s\t%d\t%s\t%s\t%d\t%s\n",
			m_csLog.m_szAct[m_csLog.m_cFlag],
			m_szDate,
			m_csLog.m_szIP,
			m_csLog.m_szAccountID,
			m_csLog.m_szCharID,
			m_csLog.m_lCharTID,
			m_csLog.m_lCharLevel,
			m_szExp,
			m_szGheldInven,
			m_szGheldBank,

			m_csLog.m_lNumID,
			m_csLog.m_szStrID,
			m_csLog.m_szDescription,
			m_csLog.m_lGheld,
			m_csLog.m_szCharID2);
	AuLogFile_s(szFile, strCharBuff);*/
	
	return TRUE;
	}




//
//	==========		Log Gheld		==========
//
AgsdRelay2Log_Gheld::AgsdRelay2Log_Gheld(AgpdLog* pLog)
	{
	ASSERT(NULL != pLog);
	m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	//memcpy(&m_csLog, pLog, sizeof(AgpdLog_Gheld));
	m_csLog = *static_cast<AgpdLog_Gheld*>(pLog);
	}


BOOL AgsdRelay2Log_Gheld::SetParamInsert(AuStatement* pStatement)
	{
	/*
	INTSERT INTO ALT_PLAY (
		ACTCODE, WDATE, IPADDR, ACCOUNTID, CHARID, CHARTID, CHARLV, CHAREXP, GHELDINVEN, GHELDBANK,
		GHELD)
	VALUES (
		:1, TO_DATE(:2, 'YYYY/MM/DD HH:MI:SS'), :3, :4, :5, :6, :7, :8, :9, :10,
		:11, :12)
	*/
		
	INT16 i = 0;
	pStatement->SetParam(i++, m_csLog.m_szAct[m_csLog.m_cFlag], sizeof(CHAR) * (AGPDLOG_MAX_ACTCODE + 1));
	pStatement->SetParam(i++, m_szDate, sizeof(m_szDate));
	pStatement->SetParam(i++, m_csLog.m_szIP, sizeof(m_csLog.m_szIP));
	pStatement->SetParam(i++, m_csLog.m_szAccountID, sizeof(m_csLog.m_szAccountID));
	pStatement->SetParam(i++, m_csLog.m_szCharID, sizeof(m_csLog.m_szCharID));
	pStatement->SetParam(i++, &m_csLog.m_lCharTID);
	pStatement->SetParam(i++, &m_csLog.m_lCharLevel);
	pStatement->SetParam(i++, m_szExp, sizeof(m_szExp));
	pStatement->SetParam(i++, m_szGheldInven, sizeof(m_szGheldInven));
	pStatement->SetParam(i++, m_szGheldBank, sizeof(m_szGheldBank));

	pStatement->SetParam(i++, &m_csLog.m_lGheldChange);
	pStatement->SetParam(i++, m_csLog.m_szCharID2, sizeof(m_csLog.m_szCharID2));
	
	return TRUE;
	}


void AgsdRelay2Log_Gheld::SetStaticQuery(CHAR *pszDest, CHAR *pszFmt)
	{
	if (!pszDest || !pszFmt)
		return;

	//sprintf(pszDest, pszFmt,
	return;
	}


BOOL AgsdRelay2Log_Gheld::WriteToFile()
	{
	/*CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\GHELDLOG_%04d%02d%02d-%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), 
			"%s\t%s\t%s\t%s\t%s\t%d\t%d\t%s\t%s\t%s\t%d\t%s\n",
			m_csLog.m_szAct[m_csLog.m_cFlag],
			m_szDate,
			m_csLog.m_szIP,
			m_csLog.m_szAccountID,
			m_csLog.m_szCharID,
			m_csLog.m_lCharTID,
			m_csLog.m_lCharLevel,
			m_szExp,
			m_szGheldInven,
			m_szGheldBank,

			m_csLog.m_lGheldChange,
			m_csLog.m_szCharID2);
	AuLogFile_s(szFile, strCharBuff);*/
	
	return TRUE;
	}



//
//	==========		Log PCRoom		==========
//
AgsdRelay2Log_PCRoom::AgsdRelay2Log_PCRoom(AgpdLog* pLog)
	{
	ASSERT(NULL != pLog);
	m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	//memcpy(&m_csLog, pLog, sizeof(AgpdLog_PCRoom));
	m_csLog = *static_cast<AgpdLog_PCRoom*>(pLog);
	}


BOOL AgsdRelay2Log_PCRoom::SetParamInsert(AuStatement* pStatement)
	{
	/*
	INSERT INTO webarch.arch_accesslog
		(SERVERID,GAMEID, USERID, CHARID, LOGINDATE, LOGOUTDATE, IP,CRMCODE,LV_LOGIN,LV_LOGOUT,STAYTIME,PLAYTIME,GAMEMONEY1,GAMEMONEY2)
	VALUES(:1,:2,:3,:4,TO_DATE(:5, 'YYYY/MM/DD HH24:MI:SS'),TO_DATE(:6, 'YYYY/MM/DD HH24:MI:SS'),:7,:8,:9,:10,:11,:12,:13,:14)
	*/

	AuTimeStamp::ConvertTimeStampToOracleTime(m_csLog.m_ulLoginTimeStamp, m_szLoginDate, sizeof(m_szLoginDate));
	m_csLog.m_ulPlayTime = (UINT32)(m_csLog.m_ulPlayTime / 60) + 1;
	m_ulStayTime = m_csLog.m_ulPlayTime;
		
	INT16 i = 0;
	pStatement->SetParam(i++, m_csLog.m_szWorld, sizeof(m_csLog.m_szWorld));
	pStatement->SetParam(i++, m_csLog.m_szGameID, sizeof(m_csLog.m_szGameID));
	pStatement->SetParam(i++, m_csLog.m_szAccountID, sizeof(m_csLog.m_szAccountID));
	pStatement->SetParam(i++, m_csLog.m_szCharID, sizeof(m_csLog.m_szCharID));
	pStatement->SetParam(i++, m_szLoginDate, sizeof(m_szLoginDate));
	pStatement->SetParam(i++, m_szDate, sizeof(m_szDate));
	pStatement->SetParam(i++, m_csLog.m_szIP, sizeof(m_csLog.m_szIP));
	pStatement->SetParam(i++, m_csLog.m_szCRMCode, sizeof(m_csLog.m_szCRMCode));
	pStatement->SetParam(i++, &m_csLog.m_lLoginLevel);
	pStatement->SetParam(i++, &m_csLog.m_lCharLevel);
	pStatement->SetParam(i++, &m_csLog.m_ulPlayTime);
	pStatement->SetParam(i++, &m_ulStayTime);
	pStatement->SetParam(i++, m_szGheldInven, sizeof(m_szGheldInven));
	pStatement->SetParam(i++, m_szGheldBank, sizeof(m_szGheldBank));
	pStatement->SetParam(i++, m_csLog.m_szGrade, sizeof(m_csLog.m_szGrade));

	return TRUE;
	}


void AgsdRelay2Log_PCRoom::SetStaticQuery(CHAR *pszDest, CHAR *pszFmt)
	{
	if (!pszDest || !pszFmt)
		return;

	//sprintf(pszDest, pszFmt,
	return;
	}


BOOL AgsdRelay2Log_PCRoom::WriteToFile()
	{
	//CHAR szFile[MAX_PATH];
	//SYSTEMTIME st;
	//GetLocalTime(&st);
	//sprintf(szFile, "LOG\\GHELDLOG_%04d%02d%02d-%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	//AuLogFile(szFile,
	//		"%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\t%s\t%d\t%s\n",
	//		m_csLog.m_szAct[m_csLog.m_cFlag],
	//		m_szDate,
	//		m_csLog.m_szIP,
	//		m_csLog.m_szAccountID,
	//		m_csLog.m_szCharID,
	//		m_csLog.m_lCharTID,
	//		m_csLog.m_lCharLevel,
	//		m_szExp,
	//		m_szGheldInven,
	//		m_szGheldBank,

	//		m_csLog.m_lGheldChange,
	//		m_csLog.m_szCharID2
	//		);
	
	return TRUE;
	}
