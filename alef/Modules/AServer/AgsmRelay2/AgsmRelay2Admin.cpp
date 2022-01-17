/*===============================================================

	AgsmRelay2Admin.cpp


	2005.05.02. steeple

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"
#include "AgspBankMoneyEdit.h"
#include "AgspCharismaPointEdit.h"


//
//	==========		Admin		==========
//
void AgsmRelay2::InitPacketAdmin()
{
	m_csPacketAdmin.SetFlagLength(sizeof(INT16));
	m_csPacketAdmin.SetFieldType(AUTYPE_INT16,			1,							// eAgsmRelay2Operation
								AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,	// Admin Char Name
								AUTYPE_INT16,			1,							// Operation
								AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,	// Search Target Name (AccountName or CharName)
								AUTYPE_MEMORY_BLOCK,	1,							// Search Data
								AUTYPE_MEMORY_BLOCK,	1,							// Char Basic Data
								AUTYPE_MEMORY_BLOCK,	1,							// Char Status Data
								AUTYPE_MEMORY_BLOCK,	1,							// Char Stat Data
								AUTYPE_MEMORY_BLOCK,	1,							// Char Money Data
								AUTYPE_MEMORY_BLOCK,	1,							// Char Item Data
								AUTYPE_MEMORY_BLOCK,	1,							// Char Title Data
								AUTYPE_CHAR,			_MAX_TREENODE_LENGTH + 1,		// Skill tree node
								AUTYPE_CHAR,			_MAX_TREENODE_LENGTH + 1,		// product(compose)
								AUTYPE_END,				0
								);
}

BOOL AgsmRelay2::OnParamAdmin(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
{
	if(!pvPacket)
		return FALSE;

	AgsdRelay2Admin* pcsRelayAdmin = new AgsdRelay2Admin;

	CHAR* pszAdminCharName = NULL;
	INT16 nAdminOperation = -1;
	CHAR* pszTargetName = NULL;

	PVOID pvSearchData = NULL;
	INT16 nSearchDataLength = -1;
	PVOID pvBasicData = NULL;
	INT16 nBasicDataLength = -1;
	PVOID pvStatusData = NULL;
	INT16 nStatusDataLength = -1;
	PVOID pvStatData = NULL;
	INT16 nStatDataLength = -1;
	PVOID pvMoneyData = NULL;
	INT16 nMoneyDataLength = -1;
	PVOID pvItemData = NULL;
	INT16 nItemDataLength = -1;
	PVOID pvTitleData = NULL;
	INT16 nTitleDataLength;

	CHAR* pszTreeNode = NULL;
	CHAR* pszProduct = NULL;

	m_csPacketAdmin.GetField(FALSE, pvPacket, 0,
							&pcsRelayAdmin->m_eOperation,	
							&pszAdminCharName,
							&nAdminOperation,
							&pszTargetName,
							&pvSearchData, &nSearchDataLength,
							&pvBasicData, &nBasicDataLength,
							&pvStatusData, &nStatusDataLength,
							&pvStatData, &nStatDataLength,
							&pvMoneyData, &nMoneyDataLength,
							&pvItemData, &nItemDataLength,
							&pvTitleData, &nTitleDataLength,
							&pszTreeNode,
							&pszProduct
							);

	if (!pszAdminCharName || strlen(pszAdminCharName) > _MAX_CHARNAME_LENGTH)
		{
		pcsRelayAdmin->Release();
		return FALSE;
		}

	pcsRelayAdmin->m_ulNID = ulNID;
	strncpy(pcsRelayAdmin->m_szAdminCharName, pszAdminCharName, _MAX_CHARNAME_LENGTH);
	pcsRelayAdmin->m_nAdminOperation = nAdminOperation;
	if(pszTargetName && strlen(pszTargetName) <= _MAX_CHARNAME_LENGTH)
		strncpy(pcsRelayAdmin->m_szTargetName, pszTargetName, _MAX_CHARNAME_LENGTH);

	if(pvSearchData)
		memcpy(&pcsRelayAdmin->m_stSearchResult, pvSearchData, sizeof(pcsRelayAdmin->m_stSearchResult));
	if(pvBasicData)
		memcpy(&pcsRelayAdmin->m_stBasic, pvBasicData, sizeof(pcsRelayAdmin->m_stBasic));
	if(pvStatusData)
		memcpy(&pcsRelayAdmin->m_stStatus, pvStatusData, sizeof(pcsRelayAdmin->m_stStatus));
	if(pvStatData)
		memcpy(&pcsRelayAdmin->m_stStat, pvStatData, sizeof(pcsRelayAdmin->m_stStat));
	if(pvMoneyData)
		memcpy(&pcsRelayAdmin->m_stMoney, pvMoneyData, sizeof(pcsRelayAdmin->m_stMoney));
	if(pvItemData)
		memcpy(&pcsRelayAdmin->m_stItem, pvItemData, sizeof(pcsRelayAdmin->m_stItem));
	if(pvTitleData)
		memcpy(&pcsRelayAdmin->m_stTitle, pvTitleData, sizeof(pcsRelayAdmin->m_stTitle));

	if(pszTreeNode)
		_tcsncpy(pcsRelayAdmin->m_szTreeNode, pszTreeNode, _MAX_TREENODE_LENGTH);
	if(pszProduct)
		_tcsncpy(pcsRelayAdmin->m_szProduct, pszProduct, _MAX_TREENODE_LENGTH);


	return EnumCallback(AGSMRELAY_PARAM_ADMIN, (PVOID)pcsRelayAdmin, (PVOID)nAdminOperation);
	//return EnumCallback(AGSMRELAY_PARAM_ADMIN, (PVOID)pcsRelayAdmin, (PVOID)nParam);
}

BOOL AgsmRelay2::CBOperationAdmin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgsdDBParam* pAgsdRelay2 = static_cast<AgsdDBParam*>(pData);
	INT16 nParam = (INT16)pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationAdmin,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
}

BOOL AgsmRelay2::CBFinishOperationAdmin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pCustData)
		return FALSE;

	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgsdDBParam* pAgsdRelay2 = static_cast<AgsdDBParam*>(pCustData);
	AuRowset* pRowset = static_cast<AuRowset*>(pData);

	if(NULL != pRowset && AGSMDATABASE_OPERATION_SELECT == pAgsdRelay2->m_eOperation)
		pThis->OnSelectResultAdmin(pRowset, pAgsdRelay2);

	pAgsdRelay2->Release();

	return TRUE;
}

BOOL AgsmRelay2::OnSelectResultAdmin(AuRowset *pRowset, AgsdDBParam *pAgsdRelay)
{
	if(!pAgsdRelay)
		return FALSE;

	if(!m_pAgpmAdmin || !m_pAgsmAdmin)
		return FALSE;

	AgsdRelay2Admin* pAgsdRelay2Admin = reinterpret_cast<AgsdRelay2Admin*>(pAgsdRelay);
	if(!pAgsdRelay2Admin)
		return FALSE;

	CHAR szTmp[255] = {0,};

	// 기본적인 데이터를 세팅하고
	AgsdRelay2Admin csAdmin;
	if(pAgsdRelay2Admin->m_szAdminCharName)
		strncpy(csAdmin.m_szAdminCharName, pAgsdRelay2Admin->m_szAdminCharName, _MAX_CHARNAME_LENGTH);
	if(pAgsdRelay2Admin->m_szTargetName)
		strncpy(csAdmin.m_szTargetName, pAgsdRelay2Admin->m_szTargetName, _MAX_CHARNAME_LENGTH);

	csAdmin.m_nAdminOperation = pAgsdRelay2Admin->m_nAdminOperation;

	// Admin Operation 에 따라 구별해야 한다.
	switch(pAgsdRelay2Admin->m_nAdminOperation)
	{
		case AGSMRELAY_PARAM_ADMIN_ACCOUNT:
		{
			for(UINT32 i = 0; i < pRowset->GetRowCount(); i++)
			{
				strncpy(csAdmin.m_stSearchResult.m_szCharName, pRowset->Get(i, 0) ? (CHAR*)pRowset->Get(i, 0) : _T(""), AGPACHARACTER_MAX_ID_STRING);
				strncpy(csAdmin.m_stSearchResult.m_szAccName, pRowset->Get(i, 1) ? (CHAR*)pRowset->Get(i, 1) : _T(""), AGPACHARACTER_MAX_ID_STRING);

				csAdmin.m_stSearchResult.m_lTID = atoi(pRowset->Get(i, 2) ? (CHAR*)pRowset->Get(i, 2) : _T(""));
				csAdmin.m_stSearchResult.m_lLevel = atoi(pRowset->Get(i, 3) ? (CHAR*)pRowset->Get(i, 3) : _T(""));

				strncpy(csAdmin.m_stSearchResult.m_szCreDate, pRowset->Get(i, 4) ? (CHAR*)pRowset->Get(i, 4) : _T(""), AGPMADMIN_MAX_CREDATE_STRING);
				strncpy(csAdmin.m_stSearchResult.m_szIP, pRowset->Get(i, 5) ? (CHAR*)pRowset->Get(i, 5) : _T(""), AGPMADMIN_MAX_IP_STRING);
				strncpy(csAdmin.m_stSearchResult.m_szGuildID, pRowset->Get(i, 7) ? (CHAR*)pRowset->Get(i, 7) : _T(""), AGPACHARACTER_MAX_ID_STRING);
				csAdmin.m_stSearchResult.m_lStatus = 0;

				strncpy(csAdmin.m_stSearchResult.m_szServerName, pRowset->Get(i, 6) ? (CHAR*)pRowset->Get(i, 6) : _T(""), AGPMADMIN_MAX_SERVERNAME_LENGTH);

				// 보낸다.
				SendSelectResultAdmin(pAgsdRelay->m_ulNID, &csAdmin);
			}
			break;
		}

		case AGSMRELAY_PARAM_ADMIN_CHAR:
		{
			if(pRowset->GetRowCount() != 1)
				break;

			UINT32 i = 0;
            strncpy(csAdmin.m_stBasic.m_szCharName, pRowset->Get(i, 0) ? (CHAR*)pRowset->Get(i, 0) : _T(""), AGPACHARACTER_MAX_ID_STRING);
			
			strncpy(csAdmin.m_stSearchResult.m_szCharName, pRowset->Get(i, 0) ? (CHAR*)pRowset->Get(i, 0) : _T(""), AGPACHARACTER_MAX_ID_STRING);
			strncpy(csAdmin.m_stSearchResult.m_szAccName, pRowset->Get(i, 1) ? (CHAR*)pRowset->Get(i, 1) : _T(""), AGPACHARACTER_MAX_ID_STRING);
			strncpy(csAdmin.m_stSearchResult.m_szCreDate, pRowset->Get(i, 16) ? (CHAR*)pRowset->Get(i, 16) : _T(""), AGPMADMIN_MAX_CREDATE_STRING);
			strncpy(csAdmin.m_stSearchResult.m_szIP, pRowset->Get(i, 17) ? (CHAR*)pRowset->Get(i, 17) : _T(""), AGPMADMIN_MAX_IP_STRING);
			strncpy(csAdmin.m_stSearchResult.m_szGuildID, pRowset->Get(i, 21) ? (CHAR*)pRowset->Get(i, 21) : _T(""), AGPACHARACTER_MAX_ID_STRING);
			csAdmin.m_stSearchResult.m_lLevel = atoi(pRowset->Get(i, 6) ? (CHAR*)pRowset->Get(i, 6) : _T(""));

			csAdmin.m_stBasic.m_lTID = atoi(pRowset->Get(i, 2) ? (CHAR*)pRowset->Get(i, 2) : _T(""));

			csAdmin.m_stStatus.m_lHP = atoi(pRowset->Get(i, 3) ? (CHAR*)pRowset->Get(i, 3) : _T(""));
			csAdmin.m_stStatus.m_lMP = atoi(pRowset->Get(i, 4) ? (CHAR*)pRowset->Get(i, 4) : _T(""));
			csAdmin.m_stStatus.m_llExp = (INT64) _atoi64(pRowset->Get(i, 5) ? (CHAR*)pRowset->Get(i, 5) : _T(""));
			csAdmin.m_stStatus.m_lLevel = atoi(pRowset->Get(i, 6) ? (CHAR*)pRowset->Get(i, 6) : _T(""));

			if(pRowset->Get(i, 7))
				m_pAgsmCharacter->DecodingPosition((CHAR*)pRowset->Get(i, 7), &csAdmin.m_stStatus.m_stPos);

			csAdmin.m_stStatus.m_lSkillPoint = atoi(pRowset->Get(i, 8) ? (CHAR*)pRowset->Get(i, 8) : _T(""));

			// 2006.02.07. steeple
			csAdmin.m_stStatus.m_lCriminalPoint = atoi(pRowset->Get(i, 9) ? (CHAR*)pRowset->Get(i, 9) : _T(""));
			csAdmin.m_stStatus.m_lMurdererPoint = atoi(pRowset->Get(i, 10) ? (CHAR*)pRowset->Get(i, 10) : _T(""));
			csAdmin.m_stStatus.m_lRemainedCriminalTime = (UINT32)atol(pRowset->Get(i, 11) ? (CHAR*)pRowset->Get(i, 11) : _T(""));
			csAdmin.m_stStatus.m_lRemainedMurdererTime = (UINT32)atol(pRowset->Get(i, 12) ? (CHAR*)pRowset->Get(i, 12) : _T(""));

            strncpy(csAdmin.m_stMoney.m_szCharName, pRowset->Get(i, 0) ? (CHAR*)pRowset->Get(i, 0) : _T(""), AGPACHARACTER_MAX_ID_STRING);
			csAdmin.m_stMoney.m_llInvenMoney = _atoi64(pRowset->Get(i, 13) ? (CHAR*)pRowset->Get(i, 13) : _T(""));
			csAdmin.m_stMoney.m_llBankMoney = _atoi64(pRowset->Get(i, 14) ? (CHAR*)pRowset->Get(i, 14) : _T(""));

			csAdmin.m_stStat.m_lCHA = atoi(pRowset->Get(i, 15) ? (CHAR*)pRowset->Get(i, 15) : _T(""));

			strncpy(csAdmin.m_stBasic.m_szServerName, pRowset->Get(i, 18) ? (CHAR*)pRowset->Get(i, 18) : _T(""), AGPMADMIN_MAX_SERVERNAME_LENGTH);
			strncpy(csAdmin.m_stSearchResult.m_szServerName, pRowset->Get(i, 18) ? (CHAR*)pRowset->Get(i, 18) : _T(""), AGPMADMIN_MAX_SERVERNAME_LENGTH);

			_tcsncpy(csAdmin.m_szTreeNode, pRowset->Get(i, 19) ? (CHAR*)pRowset->Get(i, 19) : _T(""), _MAX_TREENODE_LENGTH);
			_tcsncpy(csAdmin.m_szProduct, pRowset->Get(i, 20) ? (CHAR*)pRowset->Get(i, 20) : _T(""), _MAX_TREENODE_LENGTH);

			// 보낸다.
			SendSelectResultAdmin(pAgsdRelay->m_ulNID, &csAdmin);

			// 2005.06.08. steeple
			// Char Item 의 Bank 쪽 쿼리는 이곳에서 해준다.
			// 왜냐면.... 이것저것 따로 만들기 귀찮아서 -_-;;
			// 그리고 Character 만 검색시에는 Account 를 모를 때가 있기 때문에 이곳에서 Account 를 받아왔으니 여기서 날려주는 게 편리.

			AgsdRelay2Admin* pAgsdRelay2AdminForItemBank = new AgsdRelay2Admin;
			if(pAgsdRelay2Admin->m_szAdminCharName)
				strncpy(pAgsdRelay2AdminForItemBank->m_szAdminCharName, pAgsdRelay2Admin->m_szAdminCharName, _MAX_CHARNAME_LENGTH);

			// TargetName 에다가는 계정을 넣어준다.
			strncpy(pAgsdRelay2AdminForItemBank->m_szTargetName, csAdmin.m_stSearchResult.m_szAccName, _MAX_CHARNAME_LENGTH);

			pAgsdRelay2AdminForItemBank->m_eOperation = AGSMDATABASE_OPERATION_SELECT;
			pAgsdRelay2AdminForItemBank->m_nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_BANK;
			pAgsdRelay2AdminForItemBank->m_ulNID = pAgsdRelay->m_ulNID;

			EnumCallback(AGSMRELAY_PARAM_ADMIN, (PVOID)pAgsdRelay2AdminForItemBank, (PVOID)pAgsdRelay2AdminForItemBank->m_nAdminOperation);

#ifdef _DEBUG
			printf("[Admin] Admin:%s, Account:%s :: AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_BANK Query 요청\n",
												pAgsdRelay2AdminForItemBank->m_szAdminCharName,
												pAgsdRelay2AdminForItemBank->m_szTargetName);
#endif

			//
			//
			// 2006.01.04. steeple 
			// 캐쉬 아이템 검색한다. 후후후
			//

			AgsdRelay2Admin* pAgsdRelay2AdminForItemCash = new AgsdRelay2Admin;
			if(pAgsdRelay2Admin->m_szAdminCharName)
				strncpy(pAgsdRelay2AdminForItemCash->m_szAdminCharName, pAgsdRelay2Admin->m_szAdminCharName, _MAX_CHARNAME_LENGTH);

			// 여기는 캐릭터 이름.
			strncpy(pAgsdRelay2AdminForItemCash->m_szTargetName, csAdmin.m_stSearchResult.m_szCharName, _MAX_CHARNAME_LENGTH);

			pAgsdRelay2AdminForItemCash->m_eOperation = AGSMDATABASE_OPERATION_SELECT;
			pAgsdRelay2AdminForItemCash->m_nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_CASH;
			pAgsdRelay2AdminForItemCash->m_ulNID = pAgsdRelay->m_ulNID;

			EnumCallback(AGSMRELAY_PARAM_ADMIN, (PVOID)pAgsdRelay2AdminForItemCash, (PVOID)pAgsdRelay2AdminForItemCash->m_nAdminOperation);

#ifdef _DEBUG
			printf("[Admin] Admin:%s, Account:%s :: AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_CASH Query 요청\n",
												pAgsdRelay2AdminForItemCash->m_szAdminCharName,
												pAgsdRelay2AdminForItemCash->m_szTargetName);
#endif

			break;
		}

		case AGSMRELAY_PARAM_ADMIN_CHAR_ITEM:
		case AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_BANK:
		case AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_CASH:
		{
			for(UINT32 i = 0; i < pRowset->GetRowCount(); i++)
			{
				memset(&csAdmin.m_stItem, 0, sizeof(csAdmin.m_stItem));

				csAdmin.m_stItem.m_ullDBID = (UINT64)_atoi64(pRowset->Get(i, 0) ? (CHAR*)pRowset->Get(i, 0) : _T(""));

				if(pRowset->Get(i, 1))
					strncpy(csAdmin.m_stItem.m_szAccountName, (CHAR*)pRowset->Get(i, 1), AGPACHARACTER_MAX_ID_STRING);
				if(pRowset->Get(i, 2))
					strncpy(csAdmin.m_stItem.m_szCharName, (CHAR*)pRowset->Get(i, 2), AGPACHARACTER_MAX_ID_STRING);

				csAdmin.m_stItem.m_lTID = atoi(pRowset->Get(i, 3) ? (CHAR*)pRowset->Get(i, 3) : _T(""));
				csAdmin.m_stItem.m_lCount = atoi(pRowset->Get(i, 4) ? (CHAR*)pRowset->Get(i, 4) : _T(""));
				csAdmin.m_stItem.m_lPos = atoi(pRowset->Get(i, 5) ? (CHAR*)pRowset->Get(i, 5) : _T(""));

				if(pRowset->Get(i, 6))
					m_pAgpmAdmin->DecodingItemPos(&csAdmin.m_stItem, (CHAR*)pRowset->Get(i, 6), (INT32)_tcslen((CHAR*)pRowset->Get(i, 6)));
				if(pRowset->Get(i, 7))
					m_pAgpmAdmin->DecodingItemConvertHistory(&csAdmin.m_stItem, (CHAR*)pRowset->Get(i, 7), (INT32)_tcslen((CHAR*)pRowset->Get(i, 7)));
				if(pRowset->Get(i, 8))
					m_pAgpmAdmin->DecodingItemOption(&csAdmin.m_stItem, (CHAR*)pRowset->Get(i, 8), (INT32)_tcslen((CHAR*)pRowset->Get(i, 8)));
				if(pRowset->Get(i, 9))
					m_pAgpmAdmin->DecodingItemSkillPlus(&csAdmin.m_stItem, (CHAR*)pRowset->Get(i, 9), (INT32)_tcslen((CHAR*)pRowset->Get(i, 9)));

				csAdmin.m_stItem.m_lDurability = atoi(pRowset->Get(i, 10) ? (CHAR*)pRowset->Get(i, 10) : _T(""));
				csAdmin.m_stItem.m_lMaxDurability = atoi(pRowset->Get(i, 11) ? (CHAR*)pRowset->Get(i, 11) : _T(""));

				strncpy(csAdmin.m_stItem.m_szServerName, pRowset->Get(i, 12) ? (CHAR*)pRowset->Get(i, 12) : _T(""), AGPMADMIN_MAX_SERVERNAME_LENGTH);

				csAdmin.m_stItem.m_nInUseItem = (INT8)atoi(pRowset->Get(i, 13) ? (CHAR*)pRowset->Get(i, 13) : _T(""));
				csAdmin.m_stItem.m_lRemainTime = _atoi64(pRowset->Get(i, 14) ? (CHAR*)pRowset->Get(i, 14) : _T(""));
				
				if(pRowset->Get(i, 15))
					strcpy(szTmp, (CHAR*)pRowset->Get(i, 15));
				csAdmin.m_stItem.m_lExpireTime = AuTimeStamp::ConvertOracleTimeToTimeStamp(szTmp);

				csAdmin.m_stItem.m_lCashItemUseCount = atoi(pRowset->Get(i, 16) ? (CHAR*)pRowset->Get(i, 16) : _T(""));

				csAdmin.m_stItem.m_llStaminaRemainTime = _atoi64(pRowset->Get(i, 17) ? (CHAR*)pRowset->Get(i, 17) : _T(""));

				// 보내시기~
				SendSelectResultAdmin(pAgsdRelay->m_ulNID, &csAdmin);
			}
			break;
		}
		case AGSMRELAY_PARAM_ADMIN_CHAR_TITLE:
		{
			for(UINT32 i = 0; i < pRowset->GetRowCount(); i++)
			{
				memset(&csAdmin.m_stTitle, 0, sizeof(csAdmin.m_stTitle));

				strncpy(csAdmin.m_stTitle.m_szCharName, (CHAR*)pRowset->Get(i, 0), AGPACHARACTER_MAX_ID_STRING);

				csAdmin.m_stTitle.lTitleTid = atoi(pRowset->Get(i, 1) ? (CHAR*)pRowset->Get(i, 1) : _T(""));
				csAdmin.m_stTitle.bIsTitle = atoi(pRowset->Get(i, 2) ? (CHAR*)pRowset->Get(i, 2) : _T(""));
				csAdmin.m_stTitle.bUseTitle = atoi(pRowset->Get(i, 3) ? (CHAR*)pRowset->Get(i, 3) : _T(""));

				for(int j = 0; j < 5; j++)
					csAdmin.m_stTitle.CheckValue[j] = atoi(pRowset->Get(i, 4+j) ? (CHAR*)pRowset->Get(i, 4+j) : _T(""));

				SendSelectResultAdmin(pAgsdRelay->m_ulNID, &csAdmin);
			}
			break;
		}
	}

	return TRUE;
}

BOOL AgsmRelay2::SendSelectResultAdmin(UINT32 ulNID, AgsdRelay2Admin* pcsAdmin)
{
	if(!ulNID || !pcsAdmin)
		return FALSE;

	AgsdServer2* pcsGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(ulNID);
	if(!pcsGameServer)
		return FALSE;

	INT32 ulNewNID = pcsGameServer->m_dpnidServer;

	INT16 nPacketLength	= 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;

	INT16 nSearchDataLength = sizeof(pcsAdmin->m_stSearchResult);
	INT16 nBasicDataLength = sizeof(pcsAdmin->m_stBasic);
	INT16 nStatusDataLength = sizeof(pcsAdmin->m_stStatus);
	INT16 nStatDataLength = sizeof(pcsAdmin->m_stStat);
	INT16 nMoneyDataLength = sizeof(pcsAdmin->m_stMoney);
	INT16 nItemDataLength = sizeof(pcsAdmin->m_stItem);
	INT16 nTitleDataLength = sizeof(pcsAdmin->m_stTitle);

	PVOID pvPacketAdmin = m_csPacketAdmin.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														pcsAdmin->m_szAdminCharName,
														&pcsAdmin->m_nAdminOperation,
														pcsAdmin->m_szTargetName,
														&pcsAdmin->m_stSearchResult, &nSearchDataLength,
														&pcsAdmin->m_stBasic, &nBasicDataLength,
														&pcsAdmin->m_stStatus, &nStatusDataLength,
														&pcsAdmin->m_stStat, &nStatDataLength,
														&pcsAdmin->m_stMoney, &nMoneyDataLength,
														&pcsAdmin->m_stItem, &nItemDataLength,
														&pcsAdmin->m_stTitle, &nTitleDataLength,
														pcsAdmin->m_szTreeNode,
														pcsAdmin->m_szProduct
														);
	if(!pvPacketAdmin)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketAdmin, AGSMRELAY_PARAM_ADMIN, ulNewNID);
	m_csPacketAdmin.FreePacket(pvPacketAdmin);

	return bResult;
}

BOOL AgsmRelay2::CBOperationResultAdmin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pData)
		return FALSE;

	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgsdRelay2Admin* pAgsdRelay2 = static_cast<AgsdRelay2Admin*>(pData);

	// 2006.02.07. steeple
	// 게임서버에서 하는 콜백이다. 고로 게임서버가 아니면 나가야 한다. 이거 때메 하루 날렸3 ㅜㅜ
	if(pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
		return TRUE;

	// Auto release AgsdRelay2
	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	if(AGSMDATABASE_OPERATION_SELECT != pAgsdRelay2->m_eOperation)
		return FALSE;

	// Admin Operation 에 따라서 구별해야 한다.
	switch(pAgsdRelay2->m_nAdminOperation)
	{
		case AGSMRELAY_PARAM_ADMIN_ACCOUNT:
			pThis->m_pAgsmAdmin->ProcessSearchAccountDB(pAgsdRelay2->m_szAdminCharName, pAgsdRelay2->m_szTargetName,
												&pAgsdRelay2->m_stSearchResult);
			break;

		case AGSMRELAY_PARAM_ADMIN_CHAR:
			pThis->m_pAgsmAdmin->ProcessSearchCharacterDB(pAgsdRelay2->m_szAdminCharName, pAgsdRelay2->m_stBasic.m_szCharName,
												&pAgsdRelay2->m_stSearchResult,
												&pAgsdRelay2->m_stBasic, &pAgsdRelay2->m_stStatus, &pAgsdRelay2->m_stStat,
												&pAgsdRelay2->m_stMoney, pAgsdRelay2->m_szTreeNode, pAgsdRelay2->m_szProduct);
			break;

		case AGSMRELAY_PARAM_ADMIN_CHAR_ITEM:
		case AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_BANK:
		case AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_CASH:
			pThis->m_pAgsmAdmin->ProcessSearchCharItemDB(pAgsdRelay2->m_szAdminCharName, pAgsdRelay2->m_szTargetName,
												&pAgsdRelay2->m_stItem);
			break;
		case AGSMRELAY_PARAM_ADMIN_CHAR_TITLE:
			pThis->m_pAgsmAdmin->ProcessSearchCharTitleDB(pAgsdRelay2->m_szAdminCharName, pAgsdRelay2->m_szTargetName,
												&pAgsdRelay2->m_stTitle);
			break;
	}
	
	return TRUE;
}





BOOL AgsmRelay2::CBSearchAccount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szAdminCharName = static_cast<CHAR*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	CHAR* szAccountName = static_cast<CHAR*>(pCustData);

	// Validation Check
	if(strlen(szAdminCharName) > _MAX_CHARNAME_LENGTH || strlen(szAccountName) > _MAX_CHARNAME_LENGTH)
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	INT16 nAdminOperation = AGSMRELAY_PARAM_ADMIN_ACCOUNT;
	PVOID pvPacketAdmin = pThis->m_csPacketAdmin.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														szAdminCharName,
														&nAdminOperation,
														szAccountName,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL);
	if(!pvPacketAdmin)
		return FALSE;

	BOOL bResult = pThis->MakeAndSendRelayPacket(pvPacketAdmin, AGSMRELAY_PARAM_ADMIN);
	pThis->m_csPacketAdmin.FreePacket(pvPacketAdmin);
	
	return bResult;
}

BOOL AgsmRelay2::CBSearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szAdminCharName = static_cast<CHAR*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	CHAR* szCharName = static_cast<CHAR*>(pCustData);

	// Validation Check
	if(strlen(szCharName) > _MAX_CHARNAME_LENGTH || strlen(szCharName) > _MAX_CHARNAME_LENGTH)
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	INT16 nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR;
	PVOID pvPacketAdmin = pThis->m_csPacketAdmin.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														szAdminCharName,
														&nAdminOperation,
														szCharName,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL);
	if(!pvPacketAdmin)
		return FALSE;

	BOOL bResult = pThis->MakeAndSendRelayPacket(pvPacketAdmin, AGSMRELAY_PARAM_ADMIN);
	pThis->m_csPacketAdmin.FreePacket(pvPacketAdmin);

	// Item 도 요청한다.
	nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR_ITEM;
	PVOID pvPacketAdmin2 = pThis->m_csPacketAdmin.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														szAdminCharName,
														&nAdminOperation,
														szCharName,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL);
	if(!pvPacketAdmin2)
		return FALSE;

	bResult = pThis->MakeAndSendRelayPacket(pvPacketAdmin2, AGSMRELAY_PARAM_ADMIN);
	pThis->m_csPacketAdmin.FreePacket(pvPacketAdmin2);

	// Bank Item 은 Character Search Result 를 받는 부분에서 쿼리한다.

	// Title 요청.
	nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR_TITLE;
	PVOID pvPacketAdmin3 = pThis->m_csPacketAdmin.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														szAdminCharName,
														&nAdminOperation,
														szCharName,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL);

	if(!pvPacketAdmin3)
		return FALSE;

	bResult = pThis->MakeAndSendRelayPacket(pvPacketAdmin3, AGSMRELAY_PARAM_ADMIN);
	pThis->m_csPacketAdmin.FreePacket(pvPacketAdmin3);
	
	return bResult;
}

// 2005.09.14. steeple
// 서버 통합으로 인해서 한 서버에서 뱅크를 3개까지 가질 수 있다.
// 그런데 로그인 해있는 캐릭은 자기가 접속한 서버의 것만 가지고 있어서
// 나머지 것을 알 수가 없다. 고로 이렇게 뱅크만 가져오는 함수를 추가했다.
BOOL AgsmRelay2::CBSearchAccountBank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szAdminCharName = static_cast<CHAR*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	CHAR* szAccountName = static_cast<CHAR*>(pCustData);

	// Validation Check
	if(strlen(szAccountName) >= _MAX_CHARNAME_LENGTH || strlen(szAccountName) >= _MAX_CHARNAME_LENGTH)
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	INT16 nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_BANK;
	PVOID pvPacketAdmin = pThis->m_csPacketAdmin.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														szAdminCharName,
														&nAdminOperation,
														szAccountName,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL);
	if(!pvPacketAdmin)
		return FALSE;

	BOOL bResult = pThis->MakeAndSendRelayPacket(pvPacketAdmin, AGSMRELAY_PARAM_ADMIN);
	pThis->m_csPacketAdmin.FreePacket(pvPacketAdmin);

	return bResult;
}

// 2005.05.31. steeple
BOOL AgsmRelay2::CBAdminUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgpdAdminCharEdit* pstCharEdit = static_cast<stAgpdAdminCharEdit*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdCharacter* pcsAdminCharacter = static_cast<AgpdCharacter*>(pCustData);

	stAgpdAdminSearchResult stSearchResult;
	stAgpdAdminCharDataStatus stStatus;
	stAgpdAdminCharDataMoney stMoney;

	INT16 nSearchResultSize = sizeof(stSearchResult);
	INT16 nStatusSize = sizeof(stStatus);
	INT16 nMoneySize = sizeof(stMoney);

	memset(&stSearchResult, 0, nSearchResultSize);
	memset(&stStatus, 0, nStatusSize);
	memset(&stMoney, 0, nMoneySize);

	// Position
	strncpy(stSearchResult.m_szAccName, pstCharEdit->m_szNewValueChar, AGPACHARACTER_MAX_ID_STRING);

	stStatus.m_lLevel = pstCharEdit->m_lNewValueINT;
	stStatus.m_llExp = pstCharEdit->m_llNewValueExp;

	// InvenMoney
	_i64toa(pstCharEdit->m_llNewValueINT64, stMoney.m_szCharName, 10);

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	INT16 nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR;
	PVOID pvPacketAdmin = pThis->m_csPacketAdmin.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														pcsAdminCharacter->m_szID,
														&nAdminOperation,
														pstCharEdit->m_szCharName,
														&stSearchResult, &nSearchResultSize,
														NULL,
														&stStatus, &nStatusSize,
														NULL,
														&stMoney, &nMoneySize,
														NULL,
														NULL,
														NULL,
														NULL);
	if(!pvPacketAdmin)
		return FALSE;

	BOOL bResult = pThis->MakeAndSendRelayPacket(pvPacketAdmin, AGSMRELAY_PARAM_ADMIN);
	pThis->m_csPacketAdmin.FreePacket(pvPacketAdmin);

	return bResult;
}

// 2006.02.06. steeple
BOOL AgsmRelay2::CBAdminUpdateCharacter2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgpdAdminCharEdit* pstCharEdit = static_cast<stAgpdAdminCharEdit*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdCharacter* pcsAdminCharacter = static_cast<AgpdCharacter*>(pCustData);

	stAgpdAdminCharDataStatus stStatus;
	INT16 nStatusSize = sizeof(stStatus);
	memset(&stStatus, 0, nStatusSize);

	stStatus.m_lCriminalPoint = pstCharEdit->m_lNewValueINT;
	stStatus.m_lMurdererPoint = (INT32)pstCharEdit->m_lNewValueFloat;
	stStatus.m_lRemainedCriminalTime = (UINT32)pstCharEdit->m_llNewValueINT64 * 60;		// 분에서 초로
	stStatus.m_lRemainedMurdererTime = (UINT32)pstCharEdit->m_lNewValueCharLength * 60;	// 분에서 초로

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	INT16 nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR2;
	PVOID pvPacketAdmin = pThis->m_csPacketAdmin.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														pcsAdminCharacter->m_szID,
														&nAdminOperation,
														pstCharEdit->m_szCharName,
														NULL,
														NULL,
														&stStatus, &nStatusSize,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL);
	if(!pvPacketAdmin)
		return FALSE;

	BOOL bResult = pThis->MakeAndSendRelayPacket(pvPacketAdmin, AGSMRELAY_PARAM_ADMIN);
	pThis->m_csPacketAdmin.FreePacket(pvPacketAdmin);

	return bResult;
}

BOOL AgsmRelay2::CBAdminUpdateBankMoney(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgpdAdminCharEdit* pstCharEdit = static_cast<stAgpdAdminCharEdit*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdCharacter* pcsAdminCharacter = static_cast<AgpdCharacter*>(pCustData);


	return pThis->SendBankMoneyEdit(pstCharEdit->m_szCharName, pstCharEdit->m_szNewValueChar, pstCharEdit->m_llNewValueINT64);
}

BOOL AgsmRelay2::CBAdminUpdateCharismaPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgpdAdminCharEdit* pstCharEdit = static_cast<stAgpdAdminCharEdit*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdCharacter* pcsAdminCharacter = static_cast<AgpdCharacter*>(pCustData);


	return pThis->SendCharismaPointEdit(pstCharEdit->m_szCharName, pstCharEdit->m_lNewValueINT);
}

BOOL AgsmRelay2::SendBankMoneyEdit(CHAR *pszAccountID, CHAR *pszServerName, INT64 llBankMoney)
{
	if (!pszAccountID || !pszServerName || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	PACKET_AGSMBANKMONEYEDIT_RELAY pRelayPacket(pszAccountID, pszServerName, llBankMoney);

	AgsdServer* pRelayServer = m_pAgsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

BOOL AgsmRelay2::SendCharismaPointEdit(CHAR *pszCharID, INT32 lCharismaPoint)
{
	if(!pszCharID || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	PACKET_AGSMCHARISMAPOINTEDIT_RELAY pRelayPacket(pszCharID, lCharismaPoint);

	AgsdServer* pRelayServer = m_pAgsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

// 2005.05.18. steeple
BOOL AgsmRelay2::CBAdminItemInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdItem* pAgpdItem = static_cast<AgpdItem*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	stAgpdAdminItemOperation* pstItemOperation = static_cast<stAgpdAdminItemOperation*>(pCustData);

	if (NULL == pAgpdItem || NULL == pThis->m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	AgsdItem* pcsAgsdItem = pThis->m_pAgsmItem->GetADItem(pAgpdItem);
	if(!pcsAgsdItem)
		return FALSE;

	if(IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType) == FALSE)
	{
		if(AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus)
			return FALSE;
	}
	else
	{
		if(AGPDITEM_STATUS_CASH_INVENTORY != pAgpdItem->m_eStatus)
			return FALSE;
	}

	INT16	nPacketLength = 0;
	INT16	nOperation = AGSMDATABASE_OPERATION_INSERT;
	PVOID	pvPacketItem = pThis->MakeItemPacket(pAgpdItem, pcsAgsdItem->m_ullDBIID, nOperation, &nPacketLength, NULL, NULL, NULL, pstItemOperation);
	if (!pvPacketItem)
		return FALSE;

	BOOL bResult = FALSE;
	if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
		bResult = pThis->MakeAndSendRelayPacket(pvPacketItem, AGSMRELAY_PARAM_CASHITEM);
	else
		bResult = pThis->MakeAndSendRelayPacket(pvPacketItem, AGSMRELAY_PARAM_ITEM);
	pThis->m_csPacketItem.FreePacket(pvPacketItem);
	
	return bResult;
}

// 2005.05.18. steeple
BOOL AgsmRelay2::CBAdminItemDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgpdAdminItemOperation* pstItemOperation = static_cast<stAgpdAdminItemOperation*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdCharacter* pcsAdminCharacter = static_cast<AgpdCharacter*>(pCustData);

	UINT64 ullDBID = pstItemOperation->m_ullDBID;

	CHAR szDeleteReason[AGPMITEM_MAX_DELETE_REASON + 1];
	memset(szDeleteReason, 0, sizeof(szDeleteReason));
	sprintf(szDeleteReason, "Deleted By Admin : %s", pcsAdminCharacter->m_szID);

	AgpdItemTemplate* pcsItemTemplate = pThis->m_pAgpmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	return pThis->SendItemDelete(ullDBID, szDeleteReason, IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType));
}

// 2005.05.18. steeple
BOOL AgsmRelay2::CBAdminItemConvertUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdItem* pAgpdItem = static_cast<AgpdItem*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);

	return pThis->SendItemConvertUpdate(pAgpdItem);
}

// 2005.05.31. steeple
BOOL AgsmRelay2::CBAdminItemConvertUpdate2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szConvertHistory = static_cast<CHAR*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	UINT64* pullDBID = static_cast<UINT64*>(pCustData);

	if(!pThis->m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	INT16 nConvertHistoryLength = (INT16)_tcslen(szConvertHistory);

	PVOID pvPacketItemConvertUpdate	= pThis->m_csPacketItemConvert.MakePacket(FALSE, &nPacketLength, 0,
																	&nOperation,
																	pullDBID,
																	szConvertHistory,
																	&nConvertHistoryLength);

	if(!pvPacketItemConvertUpdate)
		return FALSE;

	BOOL bResult = pThis->MakeAndSendRelayPacket(pvPacketItemConvertUpdate, AGSMRELAY_PARAM_ITEM_CONVERT_HISTORY);
	pThis->m_csPacketItemConvert.FreePacket(pvPacketItemConvertUpdate);

	return bResult;
}

// 2006.02.08. steeple
BOOL AgsmRelay2::CBAdminItemUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{	
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgpdAdminItemOperation* pstItemOperation = static_cast<stAgpdAdminItemOperation*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdCharacter* pcsAdminCharacter = static_cast<AgpdCharacter*>(pCustData);

	// 일단 아이템의 위치부터 결정한다.
	INT16 nAdminOperation = -1;
	switch(pstItemOperation->m_stOriginalItem.m_lPos)
	{
		case AGPDITEM_STATUS_INVENTORY:
		case AGPDITEM_STATUS_EQUIP:
			nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR_ITEM;
			break;

		case AGPDITEM_STATUS_CASH_INVENTORY:
			nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_BANK;
			break;

		case AGPDITEM_STATUS_BANK:
			nAdminOperation = AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_CASH;
			break;

		// 위의 경우가 아니라면 업데이트 하지 않는다.
		default:
			return FALSE;
	}

	stAgpdAdminItem stItem;
	INT16 nSize = sizeof(stItem);
	memset(&stItem, 0, nSize);

	stItem.m_ullDBID = pstItemOperation->m_ullDBID;

	// 현재 업데이트 하는 항목은 Durability 이다.
	stItem.m_lDurability = pstItemOperation->m_lDurability;
	stItem.m_lMaxDurability = pstItemOperation->m_lMaxDurability;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketAdmin = pThis->m_csPacketAdmin.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														pcsAdminCharacter->m_szID,
														&nAdminOperation,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														&stItem, &nSize,
														NULL,
														NULL,
														NULL);
	if(!pvPacketAdmin)
		return FALSE;

	BOOL bResult = pThis->MakeAndSendRelayPacket(pvPacketAdmin, AGSMRELAY_PARAM_ADMIN);
	pThis->m_csPacketAdmin.FreePacket(pvPacketAdmin);

	return bResult;
}

BOOL AgsmRelay2::CBAdminSkillUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgpdAdminSkillString* pstSkillString = static_cast<stAgpdAdminSkillString*>(pData);
	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdCharacter* pcsAdminCharacter = static_cast<AgpdCharacter*>(pCustData);

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketSkill = pThis->m_csPacketSkill.MakePacket(FALSE, &nPacketLength, 0,
															&nOperation,
															pstSkillString->m_szCharName,
															pstSkillString->m_szTreeNode,
															pstSkillString->m_szProduct
															);

	if(!pvPacketSkill)
		return FALSE;

	BOOL bResult = pThis->MakeAndSendRelayPacket(pvPacketSkill, AGSMRELAY_PARAM_SKILL);	
	pThis->m_csPacketSkill.FreePacket(pvPacketSkill);

	return bResult;
}







BOOL AgsmRelay2::SetCallbackAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRELAY_PARAM_ADMIN, pfCallback, pClass);
}






/********************************************************/
/*		The Implementation of AgsdRelay2Admin class		*/
/********************************************************/
//
AgsdRelay2Admin::AgsdRelay2Admin()
{
	ZeroMemory(m_szAdminCharName, sizeof(m_szAdminCharName));
	m_nAdminOperation = 0;
	ZeroMemory(m_szTargetName, sizeof(m_szTargetName));

	ZeroMemory(&m_stSearchResult, sizeof(m_stSearchResult));
	ZeroMemory(&m_stBasic, sizeof(m_stBasic));
	ZeroMemory(&m_stStatus, sizeof(m_stStatus));
	ZeroMemory(&m_stStat, sizeof(m_stStat));
	ZeroMemory(&m_stMoney, sizeof(m_stMoney));
	ZeroMemory(&m_stItem, sizeof(m_stItem));
	ZeroMemory(&m_stTitle, sizeof(m_stTitle));

	ZeroMemory(m_szDBID, sizeof(m_szDBID));
	ZeroMemory(m_szExp, sizeof(m_szExp));
	ZeroMemory(m_szTreeNode, sizeof(m_szTreeNode));
	ZeroMemory(m_szProduct, sizeof(m_szProduct));
}


BOOL AgsdRelay2Admin::SetParamSelect(AuStatement* pStatement)
{
	pStatement->SetParam(0, m_szTargetName, sizeof(m_szTargetName));

	return TRUE;
}

BOOL AgsdRelay2Admin::SetParamInsert(AuStatement* pStatement)
{
	return TRUE;
}

BOOL AgsdRelay2Admin::SetParamUpdate(AuStatement* pStatement)
{
	INT16 nIndex = 0;
	switch(m_nAdminOperation)
	{
		case AGSMRELAY_PARAM_ADMIN_CHAR:
			_i64toa(m_stStatus.m_llExp, m_szExp, 10);

			pStatement->SetParam(nIndex++, &m_stStatus.m_lLevel);
			pStatement->SetParam(nIndex++, m_szExp, sizeof(m_szExp));
			pStatement->SetParam(nIndex++, m_stSearchResult.m_szAccName, sizeof(m_stSearchResult.m_szAccName));	// Position
			pStatement->SetParam(nIndex++, m_stMoney.m_szCharName, sizeof(m_stMoney.m_szCharName));				// InvenMoney
			pStatement->SetParam(nIndex++, m_szTargetName, sizeof(m_szTargetName));								// CharName
			break;

		case AGSMRELAY_PARAM_ADMIN_CHAR2:
			pStatement->SetParam(nIndex++, &m_stStatus.m_lCriminalPoint);
			pStatement->SetParam(nIndex++, &m_stStatus.m_lMurdererPoint);
			pStatement->SetParam(nIndex++, &m_stStatus.m_lRemainedCriminalTime);
			pStatement->SetParam(nIndex++, &m_stStatus.m_lRemainedMurdererTime);
			pStatement->SetParam(nIndex++, m_szTargetName, sizeof(m_szTargetName));	// CharName
			break;

		case AGSMRELAY_PARAM_ADMIN_CHAR_ITEM:
		case AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_BANK:
			_i64toa(m_stItem.m_ullDBID, m_szDBID, 10);

			pStatement->SetParam(nIndex++, &m_stItem.m_lDurability);
			pStatement->SetParam(nIndex++, &m_stItem.m_lMaxDurability);
			pStatement->SetParam(nIndex++, m_szDBID, sizeof(m_szDBID));
			break;

		case AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_CASH:
			_i64toa(m_stItem.m_ullDBID, m_szDBID, 10);

			pStatement->SetParam(nIndex++, &m_stItem.m_lDurability);
			pStatement->SetParam(nIndex++, m_szDBID, sizeof(m_szDBID));
			break;

		case AGSMRELAY_PARAM_ADMIN_CHAR_TITLE:
			pStatement->SetParam(nIndex++, m_szTargetName, sizeof(m_szTargetName));
			break;
	}

	return TRUE;
}

