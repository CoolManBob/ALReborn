/******************************************************************************
Module:  AgsmCharManager.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 23
******************************************************************************/

#include "AgsmCharManager.h"
#include "AgsmTitle.h"
#include "AgsmGameholic.h"
#include "AuGameEnv.h"

#include "AgsmSkill.h"
#include "AgsmBillInfo.h"
#include "AgsmBilling.h"//JK_빌링
#include "AgsmPrivateTrade.h" //JK_거래중금지


#ifdef _AREA_KOREA_
#ifdef _HANGAME_
#include "AuHanIPCheckForServer.h"
#include "../Server/HanGameForServer/HangameTPack/AuHangameTPack.h"
#endif
#endif

AgsmCharManager::AgsmCharManager()
{
	SetModuleName("AgsmCharManager");

	SetModuleType(APMODULE_TYPE_SERVER);

	SetPacketType(AGSMCHARMANAGER_PACKET_TYPE);

	// flag size is sizeof(CHAR)
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8, 1,			// Operation
							AUTYPE_CHAR, 12,		// Account Name
							AUTYPE_INT32, 1,		// character template id
							AUTYPE_CHAR, AGPACHARACTER_MAX_ID_STRING + 1,		// Character Name
							AUTYPE_INT32, 1,		// Character id (처리할 캐릭터 아뒤)
							AUTYPE_POS, 1,			// character position
							AUTYPE_INT32, 1,		// auth key
							/*
#ifdef	__ATTACH_LOGINSERVER__
							AUTYPE_CHAR, AGSM_MAX_SERVER_NAME + 1,	// server group name
#endif	//__ATTACH_LOGINSERVER__
							*/
							AUTYPE_END, 0
							);
	
	m_pagsmItem = NULL;

	m_pagsmBillInfo = NULL;
#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	m_csHanIPCheck = NULL;
	m_csHangameTPack = NULL;
#endif
#ifdef _WEBZEN_BILLING_
	m_pagsmBilling = NULL;
#endif
#endif
}

AgsmCharManager::~AgsmCharManager()
{
}

BOOL AgsmCharManager::OnAddModule()
{
	m_papmMap				= (ApmMap *)				GetModule("ApmMap");
	m_pagpmFactors			= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pagpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pagpmItem				= (AgpmItem *)				GetModule("AgpmItem");
	m_pagpmEventBinding		= (AgpmEventBinding *)		GetModule("AgpmEventBinding");
	m_pagpmStartupEncryption= (AgpmStartupEncryption *) GetModule("AgpmStartupEncryption");
	m_pagpmLog				= (AgpmLog*)				GetModule("AgpmLog");
	m_pagpmAdmin			= (AgpmAdmin*)				GetModule("AgpmAdmin");
	m_papmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pagpmBillInfo			= (AgpmBillInfo*)			GetModule("AgpmBillInfo");

	//m_pagsmDBStream			= (AgsmDBStream *)			GetModule("AgsmDBStream");
	m_pagsmAOIFilter		= (AgsmAOIFilter *)			GetModule("AgsmAOIFilter");
	//m_pagsmServerManager	= (AgsmServerManager *)		GetModule("AgsmServerManager");
	m_pAgsmServerManager	= (AgsmServerManager *)		GetModule("AgsmServerManager2");
	m_pagsmCharacter		= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pagsmSystemInfo		= (AgsmSystemInfo *)		GetModule("AgsmSystemInfo");
	m_pagsmFactors			= (AgsmFactors *)			GetModule("AgsmFactors");
	m_pagsmAccountManager	= (AgsmAccountManager *)	GetModule("AgsmAccountManager");
	m_pagsmZoning			= (AgsmZoning *)			GetModule("AgsmZoning");
	m_pagsmItem				= (AgsmItem *)				GetModule("AgsmItem");
	m_pAgpmConfig			= (AgpmConfig *)			GetModule("AgpmConfig");
	// JNY TODO : Relay 서버 개발을 위해 m_pagsmDBStream를 체크하는 부분을 
	// 잠시 삭제합니다. 
	// 2004.2.16
	// 다시 복구 하거나 DB모듈 변경에 의한 변경작업을 해야합니다.

	if (!m_papmMap || 
		!m_pagpmFactors ||
		!m_pagpmCharacter ||
		!m_pagpmItem ||
		!m_pagpmAdmin ||
		!m_pagpmBillInfo ||
		!m_pagsmAOIFilter ||
		!m_pAgsmServerManager || 
		!m_pagsmCharacter ||
		!m_pagsmFactors ||
		!m_pagsmAccountManager ||
		!m_pagsmSystemInfo ||
		/*!m_pagsmDBStream ||*/
		!m_pagsmZoning ||
		!m_pagsmItem ||
		!m_pagpmEventBinding ||
		!m_papmEventManager)
		return FALSE;

	//if (!m_pagsmDBStream->SetCallbackCharacterResult(CBDBOperationResult, this))
	//	return FALSE;

	//if (!m_pagsmFactors->SetCallbackFactorDB(CBDBFactor, this))
	//	return FALSE;

//	if (!m_papmMap->SetCallbackRemoveChar(CBRemoveCharacter, this))
//		return FALSE;

//	if (!m_pagpmCharacter->SetCallbackDeleteChar(CBDeleteCharacter, this))
//		return FALSE;

	if (!m_pagpmCharacter->SetCallbackRemoveCharacterFromMap(CBRemoveCharFromMap, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackGetNewCID(CBGetNewCID, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackRemoveID(CBRemoveCharacterID, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackGetNewCID(CBGetNewCID, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackCompleteRecvCharDataFromLoginServer(CBCompleteRecvCharFromLoginServer, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackSendAuthKey(CBSendAuthKey, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackCheckPCRoomType(CBCheckPCRoomType, this))
		return FALSE;

	m_csCertificatedAccount.InitializeObject( sizeof(INT32), AGPACHARACTER_CHARACTER_DATA_COUNT );

	return TRUE;
}

BOOL AgsmCharManager::OnInit()
{
	m_pagsmSkill			= (AgsmSkill*)GetModule("AgsmSkill");
	m_pagsmTitle			= (AgsmTitle*)GetModule("AgsmTitle");
	m_pagsmBillInfo			= (AgsmBillInfo*)GetModule("AgsmBillInfo");

	m_pcsAgsmPrivateTrade = (AgsmPrivateTrade*)GetModule("AgsmPrivateTrade");//JK_거래중금지

	if (!m_pcsAgsmPrivateTrade) return FALSE;//JK_거래중금지


	if(!m_pagsmBillInfo)
		return FALSE;

#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	m_csHanIPCheck			= new AuHanIPCheckForServer;
	if(!m_csHanIPCheck)
		return FALSE;

	m_csHangameTPack		= new AuHangameTPack;
	if(!m_csHangameTPack)
		return FALSE;
#endif
#ifdef _WEBZEN_BILLING_
	m_pagsmBilling			= (AgsmBilling*)GetModule("AgsmBilling");//JK_빌링
#endif
#endif	

	return TRUE;
}

BOOL AgsmCharManager::OnDestroy()
{
#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	if(m_csHanIPCheck)
	{
		delete m_csHanIPCheck;
		m_csHanIPCheck = NULL;
	}

	if(m_csHangameTPack)
	{
		m_csHangameTPack->Destroy();
		delete m_csHangameTPack;
		m_csHangameTPack = NULL;
	}
#endif
#endif

	return TRUE;
}

BOOL AgsmCharManager::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmCharManager::OnValid(CHAR* szData, INT16 nSize)
{
	return TRUE;
}

BOOL AgsmCharManager::AddCertificatedAccount( char *pstrAccountID, INT32 lCID )
{
	BOOL			bResult;

	bResult = FALSE;

	if( m_csCertificatedAccount.AddObject( &lCID, pstrAccountID ) )
	{
		bResult = TRUE;
	}

	return bResult;
}

BOOL AgsmCharManager::RemoveCertificatedAccount( char *pstrAccountID )
{
	BOOL			bResult;

	bResult = FALSE;

	if( m_csCertificatedAccount.RemoveObject( pstrAccountID ) )
	{
		bResult = TRUE;
	}

	return bResult;
}

INT32 AgsmCharManager::GetCertificatedAccountCID( char *pstrAccountID )
{
	void			*pvData;
	INT32			*plCID;

	plCID = NULL;

	pvData = m_csCertificatedAccount.GetObject( pstrAccountID );

	if( pvData != NULL )
	{
		plCID = (INT32 *)(pvData);

		return *plCID;
	}
	else
	{
		return 0;
	}
}

/*
AgpdCharacter* AgsmCharManager::AddCharacter(CHAR *szAccountID, CHAR *szName, INT32 lTID, UINT32 ulNID)
{
	AgsdAccount *pcsAccount = m_pagsmAccountManager->GetAccount(szAccountID);

	return AddCharacter(pcsAccount, szName, lTID, ulNID);
}
*/

AgpdCharacter* AgsmCharManager::AddCharacter(/*AgsdAccount *pcsAccount, */CHAR *szName, INT32 lTID, UINT32 ulNID, BOOL bLoadChar)
{
	//STOPWATCH2(GetModuleName(), _T("AddCharacter"));

	//if (!szName || !strlen(szName))
	//	return NULL;

	//if (pcsAccount && pcsAccount->m_nNumChar == AGSMACCOUNT_MAX_ACCOUNT_CHARACTER)
	//	return NULL;

//	AgpdCharacter *pcsCharacter = CreateCharacterData();
	AgpdCharacter *pcsCharacter = m_pagpmCharacter->AddCharacter(m_csGenerateCID.GetID(), lTID, szName);
	if (!pcsCharacter)
		return NULL;

	AgsdCharacter *pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);

	pcsCharacter->m_unCurrentStatus = AGPDCHAR_STATUS_LOGOUT;

	pcsAgsdCharacter->m_dpnidCharacter = ulNID;

	/*
	if (pcsAccount)
	{
		strncpy(pcsAgsdCharacter->m_szAccountID, pcsAccount->m_szName, AGPACHARACTER_MAX_ID_STRING);

		pcsAccount->m_pcsCharacter[pcsAccount->m_nNumChar] = pcsCharacter;
		pcsAccount->m_lCID[pcsAccount->m_nNumChar++] = pcsCharacter->m_lID;
	}
	*/

	// DB에서 읽어온 경우는 DB에 있는 펙터등등의 데이터를 다 세팅한다. 
	// 새로 생성된 경우일때만 템플릿 펙터를 복사하고 처리를 한다.
	if (!bLoadChar)
	{
		m_pagpmFactors->CopyFactor(&pcsCharacter->m_csFactor, &((AgpdCharacterTemplate *) pcsCharacter->m_pcsCharacterTemplate)->m_csFactor, TRUE);

		/*
		INT32 lLevel = m_pagpmFactors->GetLevel(&pcsCharacter->m_csFactor);
		if (lLevel > 0 && lLevel <= AGPMCHAR_MAX_LEVEL)
			m_pagpmFactors->CopyFactor(&pcsCharacter->m_csFactor, &pcsCharacter->m_pcsCharacterTemplate->m_csLevelFactor[lLevel], TRUE, FALSE);
		*/

		AgpdFactorCharPointMax *pcsFactorCharPointMax = (AgpdFactorCharPointMax *) m_pagpmFactors->SetFactor(&pcsCharacter->m_csFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
		if (pcsFactorCharPointMax)
		{
			m_pagpmFactors->SetMaxExp(&pcsCharacter->m_csFactor, m_pagpmCharacter->GetLevelUpExp(pcsCharacter));

			AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->SetFactor(&pcsCharacter->m_csFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT);
			if (pcsFactorCharPoint)
			{
				INT32	lMaxHP	= 0;
				INT32	lMaxMP	= 0;
				INT32	lMaxSP	= 0;

				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

				m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
				m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
				m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
			}
		}

		m_pagpmFactors->SetOwnerFactor(&pcsCharacter->m_csFactor, pcsCharacter->m_lID, (PVOID) pcsCharacter);
		
		//m_pagpmCharacter->UpdateInit(pcsCharacter);
	}

	//return m_csCharAdmin.AddCharacter(pcsCharacter);
	return pcsCharacter;
}

/*
BOOL AgsmCharManager::RemoveCharacter(CHAR *szAccountID, INT32 lCID)
{
	AgsdAccount *pcsAccount = m_pagsmAccountManager->GetAccount(szAccountID);

	return RemoveCharacter(pcsAccount, lCID);
}
*/

//BOOL AgsmCharManager::RemoveCharacter(/*AgsdAccount *pcsAccount,*/ INT32 lCID)
//{
//	if (lCID == AP_INVALID_CID)
//		return FALSE;

	/*
	if (pcsAccount)
	{
		if (pcsAccount->m_lSelectCharacter == lCID)
		{
			pcsAccount->m_lSelectCharacter = AP_INVALID_CID;
		}

		for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
		{
			if (pcsAccount->m_lCID[i] == lCID)
			{
				CopyMemory(pcsAccount->m_lCID + i, pcsAccount->m_lCID + i + 1, 
					sizeof(INT32) * (pcsAccount->m_nNumChar - i - 1));
				CopyMemory(pcsAccount->m_pcsCharacter + i, pcsAccount->m_pcsCharacter + i + 1, 
					sizeof(AgpdCharacter *) * (pcsAccount->m_nNumChar - i - 1));

				--pcsAccount->m_nNumChar;

				break;
			}
		}
	}
	*/

//	if (!m_pagpmCharacter->RemoveCharacter(lCID))
//		return FALSE;
//
//	return TRUE;
//}


BOOL AgsmCharManager::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || !nSize)
		return FALSE;

	INT8		cOperation = -1;
	CHAR		*szAccountName = NULL;
	CHAR		*szCharname = NULL;
	//INT32		lAccountID = 0;
	INT32		lTID = 0;

	INT32		lCID = 0;
	AuPOS		stPosition;

	CHAR		*szCharName = NULL;

	INT32		lAuthKey	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&szAccountName,
						&lTID,
						&szCharName,
						&lCID,
						&stPosition,
						&lAuthKey);

	switch (cOperation)
	{
	case AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT:
		{
			//STOPWATCH2(GetModuleName(), _T("AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT"));

			if(m_pagpmStartupEncryption)
			{
				if(!m_pagpmStartupEncryption->CheckCompleteStartupEncryption(ulNID))
				{
					DestroyClient(ulNID);
					return FALSE;
				}
			}

			// login server로 부터 데이타를 모두 받았다.
			// 데이타를 모두 받았으니 이제 GameWorld에 추가한다.

			CHAR	szBuffer[256];
			ZeroMemory(szBuffer, sizeof(CHAR) * 256);

//			sprintf(szBuffer, "AgsmCharManager::OnReceive() try AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT");
//			ASSERT(strlen(szBuffer) < 256);
//			WriteLog(AS_LOG_RELEASE, szBuffer);

			if (!szCharName || !strlen(szCharName))
			{
				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() 에서 character name error : null string");
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				DestroyClient(ulNID);

				return FALSE;
			}

			// Mutex Lock을 하고 넘겨준다. 주의 해야 한다.
			AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(m_pagsmCharacter->GetRealCharName(szCharName));
			if (!pcsCharacter)
			{
				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() 에서 GetCharacterLock(%s) failed!!!", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				// 이거 모얌.. ㅜㅜ 2005.04.03. steeple
				//m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);

				DestroyClient(ulNID);

				return FALSE;
			}

			AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter)
			{
				//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
				//	AuLogFile("RemoveNPC.log", "Removed by AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT\n");

				pcsCharacter->m_Mutex.Release();

				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() 에서 EnterGameWorld(%s) failed(8)", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);

				DestroyClient(ulNID);

				return FALSE;
			}

			INT32	lNumChar	= 0;

			AgsdAccount	*pcsAccount	= m_pagsmAccountManager->GetAccount(pcsAgsdCharacter->m_szAccountID);
			if (pcsAccount)
			{
				lNumChar	= pcsAccount->m_nNumChar;

				m_pagsmAccountManager->RemoveAccount(pcsAgsdCharacter->m_szAccountID, TRUE);
			}

			/*
			if (lNumChar > 0)
			{
				pcsCharacter->m_Mutex.Release();
				m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);

				DestroyClient(ulNID);
				return FALSE;
			}
			*/

			if (m_pagsmAccountManager->AddAccount(pcsAgsdCharacter->m_szAccountID, ulNID))
			{
				m_pagsmAccountManager->AddCharacterToAccount(pcsAgsdCharacter->m_szAccountID, pcsCharacter->m_lID, pcsCharacter->m_szID, FALSE);
			}
			else
			{
				//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
				//	AuLogFile("RemoveNPC.log", "Removed by AddAccount()\n");

				pcsCharacter->m_Mutex.Release();
				m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
				DestroyClient(ulNID);
				return FALSE;
			}

			/*
			m_pagsmAccountManager->AddAccount(pcsAgsdCharacter->m_szAccountID, ulNID);
			m_pagsmAccountManager->AddCharacterToAccount(pcsAgsdCharacter->m_szAccountID, pcsCharacter->m_lID, pcsCharacter->m_szID);
			*/

			// 이미 월드에 추가된 넘인지 본다. 추가된 넘이라면 잘못된 넘이다...
			if (pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD)
			{
				pcsCharacter->m_Mutex.Release();

				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() 에서 CurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD (%s)", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				DestroyClient(ulNID);

				return FALSE;
			}

			// 로그인 서버로부터 받아놓은 인증키와 클라이언트에서 보내온 인증키를 비교한다.
			// 다르다면 짤라버린다.
			if (!pcsAgsdCharacter->m_bIsValidAuthKey ||
				lAuthKey != pcsAgsdCharacter->m_lAuthKey)
			{
				pcsCharacter->m_Mutex.Release();

				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() 에서 (%s)의 인증키가 잘못되었다.", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				DestroyClient(ulNID);

				return FALSE;
			}
			
			// 인증키를 이용해 한번 접속 했으니 인증키를 Invalid 시켜 버린다.
			pcsAgsdCharacter->m_bIsValidAuthKey	= FALSE;
			
			// 월드명이 다른 서버에 로그인 했는지 체크.
			AgsdServer *pcsServer = m_pAgsmServerManager->GetThisServer();
			if (!pcsServer)
			{
				DestroyClient(ulNID);
				return FALSE;
			}

			BOOL bFoundWorld = FALSE;
			if (0 == strcmp(pcsServer->m_szWorld, pcsAgsdCharacter->m_szServerName))
				bFoundWorld = TRUE;

			if (!bFoundWorld)
			{
				for (INT32 i = 0; i < pcsServer->m_nSubWorld; i++)
				{
					if (0 == strcmp(pcsServer->m_SubWorld[i].m_szName, pcsAgsdCharacter->m_szServerName))
					{
						bFoundWorld = TRUE;
						break;
					}
				}
			}

			if (!bFoundWorld)
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "Acc[%s]Char[%s]World[%s] entered another world[%s]\n",
																	pcsAgsdCharacter->m_szAccountID,
																	pcsCharacter->m_szID,
																	pcsAgsdCharacter->m_szServerName,
																	pcsServer->m_szWorld);
				AuLogFile_s("LOG\\DIFF_WORLD.log", strCharBuff);

				DestroyClient(ulNID);
				return FALSE;
			}

			pcsAgsdCharacter->m_dpnidCharacter = ulNID;

			// 네트웍 관련 설정을 해준다.
			GetConnectionInfo(pcsAgsdCharacter->m_dpnidCharacter, &pcsAgsdCharacter->m_ulRoundTripLatencyMS, NULL);
			SetIDToPlayerContext(pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);
			SetAccountNameToPlayerContext(pcsAgsdCharacter->m_szAccountID, pcsAgsdCharacter->m_dpnidCharacter);	// 2006.04.28. steeple
			SetCheckValidation(ulNID);

			m_pagsmCharacter->ResetWaitOperation(pcsCharacter, AGSMCHARACTER_WAIT_OPERATION_ENTER_GAME_WORLD);

			ActiveSendBuffer(ulNID);

			if (m_pAgpmConfig)
				m_pAgpmConfig->SendConfigPacket(ulNID);

			// 2007.09.17. steeple
			// EventEffectID 보낸다.
			m_pagsmCharacter->SendPacketEventEffectID(pcsCharacter->m_lID, m_pagpmCharacter->GetEventEffectID(), ulNID);

			SendCharacterLoadingPosition(pcsCharacter, ulNID);

			EnumCallback(AGSMCHARMM_CB_CONNECTED_CHAR, pcsCharacter, UintToPtr(ulNID));

			// AuTimeStamp 동기화 시켜준다.

			// GetCharacterLock()을 호출해서 가져왔기 때문에 Release()를 해줘야 한다.
			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGSMCHARMANAGER_PACKET_OPERATION_LOADING_COMPLETE:
		{
			if (!pstCheckArg->bReceivedFromServer &&
				pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
				pstCheckArg->lSocketOwnerID != lCID)
				return FALSE;

			//STOPWATCH2(GetModuleName(), _T("AGSMCHARMANAGER_PACKET_OPERATION_LOADING_COMPLETE"));

			CHAR	szBuffer[256];
			ZeroMemory(szBuffer, sizeof(CHAR) * 256);

//			sprintf(szBuffer, "AgsmCharManager::OnReceive() try AGSMCHARMANAGER_PACKET_OPERATION_LOADING_COMPLETE");
//			ASSERT(strlen(szBuffer) < 256);
//			WriteLog(AS_LOG_RELEASE, szBuffer);

			AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
			if (!pcsCharacter)
			{
				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() 에서 GetCharacterLock(%d) failed!!!", lCID);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				return FALSE;
			}

			if (!EnterGameWorld(pcsCharacter))
			{
				pcsCharacter->m_Mutex.Release();

				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() 에서 EnterGameWorld(%d) failed!!!", lCID);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				return FALSE;
			}

			pcsCharacter->m_Mutex.Release();
		}
 		break;

	case AGSMCHARMANAGER_PACKET_OPERATION_DELETE_COMPLETE:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			//STOPWATCH2(GetModuleName(), _T("AGSMCHARMANAGER_PACKET_OPERATION_DELETE_COMPLETE"));

			// 2004.07.21. steeple
			// LoginServer 에서 캐릭터 삭제를 성공했을 때!!! 불린다.
			EnumCallback(AGSMCHARMM_CB_DELETE_COMPLETE, m_pagsmCharacter->GetRealCharName(szCharName), NULL);
		}
		break;

	case AGSMCHARMANAGER_PACKET_OPERATION_AUTH_KEY:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			//STOPWATCH2(GetModuleName(), _T("AGSMCHARMANAGER_PACKET_OPERATION_AUTH_KEY"));

			AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(m_pagsmCharacter->GetRealCharName(szCharName));
			if (!pcsCharacter)
			{
				CHAR	szBuffer[256];
				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() 에서 GetCharacterLock(%s) failed!!!", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				return FALSE;
			}

			AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);
			pcsAgsdCharacter->m_lAuthKey		= lAuthKey;
			pcsAgsdCharacter->m_bIsValidAuthKey	= TRUE;

			pcsCharacter->m_Mutex.Release();
		}
		break;

	default:
		{
			OutputDebugString("AgsmCharManager::OnReceive() Unknown Received Operation !!!\n");
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL AgsmCharManager::OnDisconnect(INT32 lCID, UINT32 ulNID)
{
	//STOPWATCH2(GetModuleName(), _T("OnDisconnect"));

	if (lCID != AP_INVALID_CID)		// 캐릭터가 존재하는 경우.. 즉 이 캐릭터가 연결 종료되었다.
	{
		AgpdCharacter	*pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
		if (!pcsCharacter)
		{
//			TRACEFILE("AgsmCharManager::OnDisconnect() 존재하지 않는 CID가 넘어왔다.");
			return FALSE;
		}

		m_pcsAgsmPrivateTrade->ProcessTradeCancelUnExpected(lCID);//JK_거래중금지

		AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);
		AgsdZoning		*pcsAgsdZoning		= m_pagsmZoning->GetADCharacter(pcsCharacter);

		pcsAgsdCharacter->m_bIsNotLogout	= FALSE;

		if (pcsAgsdZoning->m_bDisconnectZoning)	// 조닝땜시 접속이 종료된 경우이다. 이 경우는 걍 아이템만 없애고 캐릭터 데이타는 유지시킨다.
		{
			pcsAgsdZoning->m_bDisconnectZoning = FALSE;

			m_pagpmCharacter->StopCharacter(pcsCharacter, &pcsCharacter->m_stPos);

			m_pagsmCharacter->RemoveItemOnly(pcsCharacter);

			pcsCharacter->m_Mutex.Release();

			return TRUE;

			//m_pagsmCharacter->SendPacketCharRemoveSync(pcsCharacter);
		}
		else	// 정상적으로 접속이 종료되었다. 캐릭터 데이타를 삭제한다.
		{
			//m_papmMap->DeleteChar(pcsCharacter->m_stPos, pcsCharacter->m_lID);

			EnumCallback(AGSMCHARMM_CB_DISCONNECT_CHARACTER, pcsCharacter, NULL);

			// Logout Log 를 남긴다. - 2004.05.02. steeple
			// 비정상 종료시에도 남기기 위해 CBRemoveCharcter로 이동.
			// WriteLogoutLog(pcsCharacter);

			// Level Up Log 를 남긴다. - 2004.05.18. steeple
			// 테이블 통합으로 제거됨. - 2005.05.20. laki
			// m_pagsmCharacter->WriteLevelUpLog(pcsCharacter, 0, TRUE);

			//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
			//	AuLogFile("RemoveNPC.log", "Removed by OnDisconnect()\n");

			pcsCharacter->m_Mutex.Release();

			AgsdAccount	*pcsAccount	= m_pagsmAccountManager->GetAccount(pcsAgsdCharacter->m_szAccountID);
			if (pcsAccount && pcsAccount->m_nNumChar <= 1)
			{
				m_pagsmAccountManager->RemoveAccount(pcsAgsdCharacter->m_szAccountID);
			}

			if (!m_pagpmCharacter->RemoveCharacter(lCID))
			{
				OutputDebugString("AgsmCharManager::OnDisconnect() Error (1) !!!\n");
				return FALSE;
			}

		}

	}

	return TRUE;
}

BOOL AgsmCharManager::IsValidLogin(CHAR *szAccountName)
{
	if (!szAccountName || !szAccountName[0])
		return FALSE;

	return TRUE;
}

//		CreateCharacter
//	Functions
//		- 새로운 캐릭을 만든다.
//			(더이상 만들 수 있는지 검사한다. 그담에 만들수 있다면 클라이언트가 보내준 데이타로 캐릭을 만든다)
//	Arguments
//		  (캐릭터를 만들때 템플릿 데이타에서 변경되는 사항들을 다 인자로 넘겨준다)
//		- szAccountName : 계정 이름
//		- szCharName : 만드는 새로운 캐릭터 이름
//		- lTID : 캐릭터 템플릿 아뒤
//		- ulNID : 이 사용자 network id
//	Return value
//		- AgpdCharacter * : 생성한 캐릭터 포인터
///////////////////////////////////////////////////////////////////////////////
AgpdCharacter* AgsmCharManager::CreateCharacter(CHAR *szAccountName, CHAR *szCharName, INT32 lTID, UINT32 ulNID, BOOL bIsPC, CHAR *pszServerName)
{
	//STOPWATCH2(GetModuleName(), _T("CreateCharacter"));

	if (!lTID)
		return FALSE;

	// template 데이타를 기반으로 해서 새로운 캐릭터를 생성한다.
	// 기본은 template 데이타를 사용하되 사용자가 변경한 데이타는 인자로 넘겨받아 구걸 적용시킨다.

	AgpdCharacterTemplate *pcsCharTemplate = m_pagpmCharacter->GetCharacterTemplate(lTID);
	if (!pcsCharTemplate)
		return FALSE;

	AgpdCharacter	*pcsCharacter = AddCharacter(/*pcsAccount, */szCharName, lTID, ulNID);
	if (!pcsCharacter)
		return FALSE;

	INT32	lRace	= m_pagpmFactors->GetRace(&pcsCharacter->m_csFactor);

//	AuPOS	stFirstPos;
//
//	switch (lRace) {
//	case AURACE_TYPE_ORC:
//		{
//			stFirstPos.x = 94021.922f;
//			stFirstPos.y = 4133.300f;
//			stFirstPos.z = 244826.938f;
//			break;
//		}
//
//	case AURACE_TYPE_HUMAN:
//	default:
//		{
//			stFirstPos.x = -353348;
//			stFirstPos.y = 6946;
//			stFirstPos.z = 119705;
//			break;
//		}
//	}
//
//	if (!m_papmEventManager->GetRandomPos(&stFirstPos, &pcsCharacter->m_stPos, 100, 600, TRUE))
//		pcsCharacter->m_stPos	= stFirstPos;

//	if (!m_pagpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos))
//		return FALSE;

	// character point를 세팅한다.
	m_pagsmFactors->SetCharPoint(&pcsCharacter->m_csFactor, FALSE, FALSE);

	if (bIsPC)
	{
		m_pagpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos);

		AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);

		AgsdServer *pcsThisServer = m_pAgsmServerManager->GetThisServer();
		if (!pcsThisServer)
		{
			//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
			//	AuLogFile("RemoveNPC.log", "Removed by AgsmCharManager::CreateCharacter()\n");

			m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
			return FALSE;
		}
		// 2005.07.21	laki
		//pcsAgsdCharacter->m_ulServerID = pcsThisServer->m_lID;
		pcsAgsdCharacter->m_ulServerID = pcsThisServer->m_lServerID;

		ZeroMemory(pcsAgsdCharacter->m_szServerName, sizeof(pcsAgsdCharacter->m_szServerName));
		
		if (NULL == pszServerName)
			strncpy(pcsAgsdCharacter->m_szServerName, pcsThisServer->m_szWorld, AGSM_MAX_SERVER_NAME);
		else
			strncpy(pcsAgsdCharacter->m_szServerName, pszServerName, AGSM_MAX_SERVER_NAME);

		//m_pagsmCharacter->ReCalcCharacterFactors(pcsCharacter, FALSE);

		/*
#ifdef	__DB_OPERATION__
		// DB에 저장한다.
		stAgsmCharacterDB	stCharacterData;

		m_pagsmDBStream->InitChacterData(&stCharacterData);

		// 저장할 캐릭터 데이타를 세팅한다.
		strcpy(stCharacterData.szName, pcsCharacter->m_szID);
		strcpy(stCharacterData.szTName, ((AgpdCharacterTemplate *) pcsCharacter->m_pcsCharacterTemplate)->m_szTName);
		strcpy(stCharacterData.szAccountName, pcsAgsdCharacter->m_szAccountID);
		strcpy(stCharacterData.szServerName, ((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_szGroupName);

		stCharacterData.fPosX = pcsCharacter->m_stPos.x;
		stCharacterData.fPosY = pcsCharacter->m_stPos.y;
		stCharacterData.fPosZ = pcsCharacter->m_stPos.z;

		// 위에서 세팅한 데이타로 쿼리를 날린다.
		if (!m_pagsmDBStream->AddCharacterDB(&stCharacterData, pcsCharacter->m_lID))
		{
			m_pagpmCharacter->RemoveCharacter(pcsCharacter);
			return FALSE;
		}

#else
		*/

		/*
		SendPacketCreateCharacter(pcsCharacter, szAccountName);
		*/

		m_pagpmCharacter->UpdateInit(pcsCharacter);

		/*
		if (bIsPC)
			EnumCallback(AGSMCHARMM_CB_INSERT_CHARACTER_TO_DB, pcsCharacter, NULL);
		*/

		EnumCallbackCreateCharacter(pcsCharacter);

//#endif	//__DB_OPERATION__
	}
	else
	{
		m_pagpmCharacter->UpdateInit(pcsCharacter);

		EnumCallbackCreateCharacter(pcsCharacter);
	}

	//m_pagpmCharacter->UpdateInit(pcsCharacter);

	//EnumCallback(AGSMCHARMM_CB_CREATECHAR, pcsCharacter, NULL);

	//pcsCharacter->m_Mutex.Release();

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_bIsCreatedChar	= TRUE;

	return pcsCharacter;
}

/*
//		DeleteCharacter
//	Functions
//		- 기존에 있는 캐릭을 지운다.
//			지울 캐릭터가 있는지 검사한다.
//			지운후에 DB에 반영한다.
//	Arguments
//		- szAccountName : 계정 이름
//		- lCID : 지울 캐릭터
//	Return value
//		- INT16 : 처리결과
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmCharManager::DeleteCharacter(CHAR *szAccountName, INT32 lCID)
{
	return DeleteCharacter(szAccountName, m_pagpmCharacter->GetCharacter(lCID));
}

INT16 AgsmCharManager::DeleteCharacter(CHAR *szAccountName, AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	return TRUE;
}
*/

//		PreSetting
//	Functions
//		- EnterGameWorld 들어가기 전에 해야할 설정이 있음 여기서 한다.
//	Arguments
//		- none
//	Return value
//		- INT16 : 처리결과
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmCharManager::PreSetting()
{
	return TRUE;
}

//		EnterGameWorld
//	Functions
//		- pCharacter 데이타로 실제 게임에 들어간다.
//	Arguments
//		- pCharacter : 게임에 들어갈 캐릭터 정보
//	Return value
//		- INT16 : 처리결과
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmCharManager::EnterGameWorld(AgpdCharacter *pcsCharacter, BOOL bIsCheckValidate)
{
	//STOPWATCH2(GetModuleName(), _T("EnterGameWorld"));

	// 캐릭의 현재 상태 m_nCurrentStatus를 AGPDCHAR_STATUS_NORMAL로 바꾼다.
	if (!pcsCharacter || pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD)
		return FALSE;

	FLOAT	fOriginalPosY	= 0.0f;

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);

	if (bIsCheckValidate)
	{
		if (!pcsAgsdCharacter->m_bIsRecvAllDataFromLoginSvr ||
			pcsAgsdCharacter->m_dpnidCharacter == 0)
			return FALSE;

		pcsAgsdCharacter->m_strIPAddress.MemSetAll();
		strncpy(&pcsAgsdCharacter->m_strIPAddress[0], GetPlayerIPAddress(pcsAgsdCharacter->m_dpnidCharacter), 15);

		// 현재 저장되고 있지 않은 Skill 관련 데이타들을 위해 이렇게 세팅한다.
		INT32	lLevel	= 0;
		m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

		EnumCallback(AGSMCHARMM_CB_SET_CHAR_LEVEL, pcsCharacter, &lLevel);
		// 현재 저장되고 있지 않은 Skill 관련 데이타들을 위해 이렇게 세팅한다.

		if (m_pagpmCharacter->IsPC(pcsCharacter))
			m_pagsmFactors->ResetCharMaxPoint(&pcsCharacter->m_csFactor);

		fOriginalPosY	= pcsCharacter->m_stPos.y;
		pcsCharacter->m_stPos.y	= 100000;
		
		if (m_pagpmCharacter->IsPC(pcsCharacter))
		{
			if (pcsCharacter->m_stPos.x == 0.0f &&
				pcsCharacter->m_stPos.z == 0.0f)
			{
				m_pagpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos);
			}
			else
			{
				EnumCallback(AGSMCHARMM_CB_CHECK_LOGIN_POSITION, pcsCharacter, NULL);

				// 레벨 제한 걸린 지역이면 이전 마을로 보내버린다. 2008.03.25. steeple
				m_pagsmCharacter->CheckLoginPosition(pcsCharacter);
			}
		}

		// 2005.01.21. steeple
		m_pagpmCharacter->UpdateRegion(pcsCharacter);

		/*
		INT16	unCurrentStatus	= pcsCharacter->m_unCurrentStatus;
		pcsCharacter->m_unCurrentStatus	= AGPDCHAR_STATUS_IN_GAME_WORLD;
		*/

		EnumCallback(AGSMCHARMM_CB_SET_CHARACTER_GAME_DATA, pcsCharacter, NULL);

		// character, item, etc.. 등등의 데이타를 일단 몽땅 보내준다.
		if (!m_pagsmCharacter->SendCharacterAllInfo(pcsCharacter, pcsAgsdCharacter->m_dpnidCharacter))
		{
			return FALSE;
		}

#ifdef _AREA_KOREA_
#ifdef _WEBZEN_BILLING_

		if(pcsAgsdCharacter->m_dpnidCharacter > 0 && m_pagpmCharacter->IsPC(pcsCharacter) && _tcslen(pcsCharacter->m_szID) > 0)
		{
			m_pagsmBilling->CheckIn(pcsCharacter);
		}

#endif
#ifdef _HAN_GAME_
		// PC Room 체크 2007.11.06. steeple
		if(pcsAgsdCharacter->m_dpnidCharacter > 0 && m_pagpmCharacter->IsPC(pcsCharacter) && _tcslen(pcsCharacter->m_szID) > 0)
		{
			CheckPCRoom(pcsCharacter);
		}
#endif
#endif

		//Title 정보 따로 전해준다.
		m_pagsmTitle->TitleListSend(pcsCharacter);
		m_pagsmTitle->TitleQuestListSend(pcsCharacter);

		// 마을에 있는 경우 해당 마을의 NPC를 모두 전송한다. 2005.06.27 by kelovon
		m_pagsmCharacter->SendRegionNPCToChar(m_pagpmCharacter->GetRealRegionIndex(pcsCharacter), pcsCharacter);

		/*
		pcsCharacter->m_unCurrentStatus	= unCurrentStatus;
		*/

		//m_pagsmCharacter->CheckRecvCharList(pcsCharacter->m_lID);

		// 이 character로 게임이 진행될거라는 패킷을 날려준다.
		if (!SendCharacterSettingOK(pcsCharacter, pcsAgsdCharacter->m_dpnidCharacter))
		{
			return FALSE;
		}
	}

	m_pagsmCharacter->ReCalcCharacterFactors(pcsCharacter, TRUE);

	if (bIsCheckValidate)
		AdjustCharMaxPoint(pcsCharacter);

	AgpdCharacter *pcsAddChar = m_pagsmCharacter->EnterGameWorld(pcsCharacter);
	if (!pcsAddChar)
		return FALSE;

	AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pagpmFactors->SetFactor(&pcsCharacter->m_csFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (pcsResultFactor)
	{
		m_pagpmFactors->CopyFactor(pcsResultFactor, &pcsCharacter->m_csFactor, TRUE, FALSE, AGPD_FACTORS_TYPE_CHAR_POINT);

		PVOID	pvPacketFactor = m_pagpmFactors->MakePacketFactors(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (pvPacketFactor)
		{
			m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_3);
		}

		m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);
	}

	/*
	// 캐릭터를 AgpmCharacter에 추가한다. 걍 Instance 자체를 추가해버린다. 다시 읽지 않는다.
	AgpdCharacter *pcsAddChar = m_pagpmCharacter->AddCharacter(pCharacter);
	if (!pcsAddChar)
		return FALSE;

	//m_pagsmFactors->LoadFactor(pcsAddChar->m_szID, &pcsAddChar->m_csFactor);
	
	pcsAddChar->m_unCurrentStatus = AGPDCHAR_STATUS_NORMAL;

	EnumCallback(AGSMCHARMM_CB_ENTERGAMEWORLD, pcsAddChar, NULL);

	m_pagpmCharacter->UpdateInit(pcsAddChar);
	*/

	//m_pagpmCharacter->UpdateStatus(pcsAddChar, AGPDCHAR_STATUS_NORMAL);

	EnumCallback(AGSMCHARMM_CB_ENTERGAMEWORLD, pcsAddChar, NULL);

	// 2006.01.23. steeple
	// EnterGameWorld Callback 후에 부르는 걸로 변경.
	// 그래야지만, Cash Item Skill 이 제대로 동작한다.
	//
	// 2005.12.14. steeple
	// AgsmItem 에서 필요한 EnterGameWorld 를 해준다. AgsmItem 이 상위 모듈이라서 이렇게 직접 불러줌.
	m_pagsmItem->EnterGameWorld(pcsAddChar);

	//////////////////////////////////////////////////////////////////////////
	// save skill - arycoat 2008.7.
	m_pagsmSkill->RecastSaveSkill(pcsCharacter);

	// 여기서 다시 PassiveCastSkill을 해준다.
	m_pagsmSkill->ReCastAllPassiveSkill((ApBase*)pcsCharacter);

	m_pagsmTitle->UsingTitleSendToClient(pcsCharacter);

	//JK_패시브스킬 표시 안되는 문제
//	m_pagsmSkill->SendPassiveSkillAll(pcsCharacter, m_pagsmCharacter->GetCharDPNID(pcsCharacter));


	// 게임 가드 추가
	//if ( AP_SERVICE_AREA_JAPAN == g_eServiceArea
	//	 && m_pagpmCharacter->IsPC(pcsAddChar)
	//	 && strlen(pcsAddChar->m_szID) )
	//{
	//	m_pagsmCharacter->GGSendInitialPacket( pcsAddChar );
	//}

	// 중국 중독 방지시스템 추가
#ifdef _AREA_CHINA_
	g_agsmGameholic.Online( pcsAgsdCharacter->m_szAccountID,
							  pcsAddChar->m_szID,
							  pcsAgsdCharacter->m_strIPAddress.begin() );
#endif

	if(m_pAgpmConfig->IsTestServer() &&
	   m_pagpmCharacter->IsPC(pcsAddChar) && _tcslen(pcsAddChar->m_szID) > 0)
	{
		INT32 lLevel = m_pagpmCharacter->GetLevel(pcsAddChar);
		INT64 lExp = m_pagpmCharacter->GetExp(pcsAddChar);

		if(lLevel == 1 && lExp == 0)
		{
			m_pagsmCharacter->SetCharacterLevel(pcsAddChar, m_pAgpmConfig->GetStartLevel(), FALSE);
			m_pagpmCharacter->AddMoney(pcsAddChar, m_pAgpmConfig->GetStartGheld());
			m_pagpmCharacter->UpdateMoney(pcsAddChar);
		}
	}

	// Go (Game Operator) 체크 2007.07.29. steeple
	if(pcsAddChar->m_pcsCharacterTemplate->m_lID == 886)
	{
		m_pagpmCharacter->UpdateSetSpecialStatus(pcsAddChar, AGPDCHAR_SPECIAL_STATUS_GO);
	}

	if (bIsCheckValidate && pcsAgsdCharacter->m_dpnidCharacter != 0)
	{
		pcsCharacter->m_stPos.y	= fOriginalPosY;

		// Relay, Backup Server로 pcsCharacter의 public data를 넘겨준다.
		//////////////////////////////////////////////////////////////////////////////////////
		AgsdServer	*pcsRelayServer	= m_pAgsmServerManager->GetRelayServer();
		if (pcsRelayServer)
		{
			//m_pagsmCharacter->SendPacketAllDBData(pcsCharacter, pcsRelayServer->m_dpnidServer);

			//////////////////////////////////////////////////////////////////////////
			//
			// 아래 코드 주석 처리. 2004.07.20. steeple
			//
			// Admin 일 경우에는 Relay, Backup Server 서버에 AddCharacter 해야 한다. - 2004.03.04 steeple
			//if(m_pagpmAdmin->IsAdminCharacter(pcsCharacter))
			//{
			//	m_pagsmCharacter->SendCharacterAllInfo(pcsCharacter, pcsRelayServer->m_dpnidServer);
			//}
		}

		/*
		//아래코드들은 로그인 서버로 넘겨야한다.
		//ControlServer정보를 보낸다.
		m_pagsmSystemInfo->SendServerInfo( AGSMSERVER_TYPE_MASTER_SERVER, pcsCharacter->m_lID, m_pAgsmServerManager->GetThisServer() );
		//Deal Server정보를 보낸다.
		m_pagsmSystemInfo->SendServerInfo( AGSMSERVER_TYPE_DEAL_SERVER, pcsCharacter->m_lID, m_pAgsmServerManager->GetDealServer() );
		//Recruit Server정보를 보낸다.
		m_pagsmSystemInfo->SendServerInfo( AGSMSERVER_TYPE_RECRUIT_SERVER, pcsCharacter->m_lID, m_pAgsmServerManager->GetRecruitServer() );
		*/

		// Login Log 를 남긴다. - 2004.05.02. steeple
		WriteLoginLog(pcsCharacter);
	}

	return TRUE;
}

// 2007.11.06. steeple
// PC Room Module 을 초기화 한다.
BOOL AgsmCharManager::InitPCRoomModule()
{
#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	GetGameEnv().InitEnvironment();

	BOOL bTest = FALSE;
	if(GetGameEnv().IsAlpha())
		bTest = TRUE;

	// s피시방 비활성화 - arycoat 2010.11.02
	//int iResult = m_csHanIPCheck.AuHanIPCheckInit(bTest);

	// iResult 값 분석해본다.

	if( m_csHangameTPack )
	{
		AgsdServer2* pGameServer = m_pAgsmServerManager->GetThisServer();
		if(!pGameServer)
			return FALSE;
				
		if(!m_csHangameTPack->Create(pGameServer->m_lServerID))
			return FALSE;
	}
#endif
#endif //_AREA_KOREA_
	return TRUE;
}

// 2007.11.06. steeple
// PC Room IP 체크한다.
BOOL AgsmCharManager::CheckPCRoom(AgpdCharacter* pcsCharacter)
{
#ifdef _AREA_KOREA_
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	AgpdBillInfo* pcsAttachBillInfo = m_pagpmBillInfo->GetADCharacter(pcsCharacter);
	if(!pcsAttachBillInfo)
		return FALSE;

#ifdef _HANGAME_
	int iResult = -1;
//	if(GetGameEnv().IsAlpha())
//		iResult = m_csHanIPCheck.AuHanIPCheckRequest(_T("121.172.155.159"), pcsAgsdCharacter->m_szCRMCode, pcsAgsdCharacter->m_szGrade);
//	else	// s피시방 비활성화 - arycoat 2010.11.02
//	iResult = m_csHanIPCheck.AuHanIPCheckRequest(&pcsAgsdCharacter->m_strIPAddress[0], pcsAgsdCharacter->m_szCRMCode, pcsAgsdCharacter->m_szGrade);

	printf("CharName: %s, CRM : %s, Grade : %s\n", pcsCharacter->m_szID, pcsAgsdCharacter->m_szCRMCode, pcsAgsdCharacter->m_szGrade);
	if(iResult == 0)
	{
		pcsAttachBillInfo->m_ulPCRoomType |= AGPDPCROOMTYPE_HANGAME_S;
	}

	if( m_csHangameTPack )
	{
		BOOL bTPack = m_csHangameTPack->CheckIn(pcsAgsdCharacter->m_szAccountID, &pcsAgsdCharacter->m_strIPAddress[0]);
		if(bTPack)
		{
			pcsAttachBillInfo->m_ulPCRoomType |= AGPDPCROOMTYPE_HANGAME_TPACK;
		}
	}

	if( pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_S || pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK )
	{
		m_pagpmBillInfo->UpdateIsPCRoom(pcsCharacter, TRUE);
	}
	m_pagsmBillInfo->SendCharacterBillingInfo(pcsCharacter);
#endif
#endif
	return TRUE;
}

BOOL AgsmCharManager::CheckOutPCRoom(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	if( m_csHangameTPack )
		m_csHangameTPack->CheckOut(pcsAgsdCharacter->m_szAccountID);
#endif
#endif // _AREA_KOREA_

	return TRUE;
}

BOOL AgsmCharManager::SetCallbackCreateChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_CREATECHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::EnumCallbackCreateCharacter(AgpdCharacter * pcsAgpdCharacter)
{
	return EnumCallback(AGSMCHARMM_CB_CREATECHAR, pcsAgpdCharacter, NULL);
}

BOOL AgsmCharManager::SetCallbackDeleteChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_DELETECHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackLoadChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_LOADCHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackUnLoadChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_UNLOADCHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackEnterGameWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_ENTERGAMEWORLD, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackConnectedChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_CONNECTED_CHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackInsertCharacterToDB(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_INSERT_CHARACTER_TO_DB, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackAddCharResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_ADDCHAR_RESULT, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackSetCharLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_SET_CHAR_LEVEL, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackDisconnectCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_DISCONNECT_CHARACTER, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackDeleteComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_DELETE_COMPLETE, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackSetCharacterGameData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_SET_CHARACTER_GAME_DATA, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackCheckLoginPosition(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_CHECK_LOGIN_POSITION, pfCallback, pClass);
}

BOOL AgsmCharManager::InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag, INT32 lRemoveIDQueueSize)
{
	//m_csGenerateAID.Initialize(ulStartValue, ulServerFlag, nSizeServerFlag);
	m_csGenerateCID.Initialize(ulStartValue, ulServerFlag, nSizeServerFlag, lRemoveIDQueueSize, TRUE);

	return TRUE;
}


BOOL AgsmCharManager::CBRemoveCharFromMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;
		
	AgsmCharManager		*pThis				= (AgsmCharManager *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	if (pThis->m_pagpmCharacter->IsPC(pcsCharacter))
	{	
		AgsdCharacter *pAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if (NULL != pAgsdCharacter && FALSE == pAgsdCharacter->m_bIsNotLogout)
			pThis->WriteLogoutBankLog(pcsCharacter);
	}
	
	return TRUE;
}

//		CBRemoveCharacter
//	Functions
//		- player character가 맵에서 나올경우 DB에 데이타를 몽땅 저장한다.
//	Arguments
//		- pData : pstAgsmDBOperationResult
//		- pClass : this module pointer
//	Return value
//		- BOOL : 처리결과
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharManager::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;
		
	AgsmCharManager		*pThis				= (AgsmCharManager *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	
	if (pThis->m_pagpmCharacter->IsPC(pcsCharacter) && _tcslen(pcsCharacter->m_szID) > 0)
	{
#ifdef _WEBZEN_BILLING_
		if( pThis->m_pagsmBilling )
			pThis->m_pagsmBilling->CheckOut(pcsCharacter);
#endif
#ifdef _HAN_GAME_
		pThis->CheckOutPCRoom(pcsCharacter);
#endif
		pThis->WriteLogoutLog(pcsCharacter);
	}
	
	return TRUE;
}

BOOL AgsmCharManager::CBRemoveCharacterID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmCharManager		*pThis				= (AgsmCharManager *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	pThis->m_csGenerateCID.AddRemoveID(pcsCharacter->m_lID);

	return TRUE;
}

BOOL AgsmCharManager::CBDeleteCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	return TRUE;
}

BOOL AgsmCharManager::CBCompleteRecvCharFromLoginServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmCharManager	*pThis			= (AgsmCharManager *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)		pData;

	return (BOOL) pThis->EnterGameWorld(pcsCharacter);
}

BOOL AgsmCharManager::CBSendAuthKey(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCharManager	*pThis				= (AgsmCharManager *)	pClass;
	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)		pData;
	UINT32			ulNID				= *(UINT32 *)			pCustData;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_bIsValidAuthKey)
		return pThis->SendAuthKeyPacket(pcsCharacter->m_szID, pcsAgsdCharacter->m_lAuthKey, ulNID);

	return TRUE;
}

BOOL AgsmCharManager::CBGetNewCID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmCharManager	*pThis		= (AgsmCharManager *)	pClass;
	INT32			*plCID		= (INT32 *)				pData;

	*plCID						= pThis->m_csGenerateCID.GetID();

	return TRUE;
}

//		CBDisconnectDB
//	Functions
//		- db와의 연결이 끊긴다. (즉, 서버가 죽는다.) player character 전부를 DB에 저장하기 위해 Remove 시킨다.
//			(Remove callback을 받아서 DB 저장한다.)
//	Arguments
//		- pData : NULL
//		- pClass : this module pointer
//		- pCustData : NULL
//	Return value
//		- BOOL : 처리결과
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharManager::CBDisconnectDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmCharManager	*pThis = (AgsmCharManager *) pClass;

	INT32 lIndex = 0;

	AgpdCharacter *pcsCharacter = pThis->m_pagpmCharacter->GetCharSequence(&lIndex);

	while (pcsCharacter)
	{
		AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

		if (pcsAgsdCharacter->m_dpnidCharacter != 0)
		{
			pThis->m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
		}

		pcsCharacter = pThis->m_pagpmCharacter->GetCharSequence(&lIndex);
	}

	return TRUE;
}

BOOL AgsmCharManager::SendCharacterSettingOK(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter)
		return FALSE;

	BOOL	bRetval = TRUE;

#ifdef	__ATTACH_LOGINSERVER__

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGSMCHARMANAGER_PACKET_OPERATION_SETTING_CHARACTER_OK;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMCHARMANAGER_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											NULL,
											&pcsCharacter->m_lID,
											NULL,
											NULL);

	if (!pvPacket)
		return FALSE;

	bRetval	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

#endif	//__ATTACH_LOGINSERVER__

	return bRetval;
}

BOOL AgsmCharManager::SendCharacterLoadingPosition(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGSMCHARMANAGER_PACKET_OPERATION_CHARACTER_POSITION;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMCHARMANAGER_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											NULL,
											&pcsCharacter->m_lID,
											&pcsCharacter->m_stPos,
											NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmCharManager::AdjustCharMaxPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacterTemplate	*pcsTemplate	= pcsCharacter->m_pcsCharacterTemplate;
	if (!pcsTemplate)
		return FALSE;

	INT32	lCharacterLevel	= m_pagpmCharacter->GetLevel(pcsCharacter);

	if (lCharacterLevel < 1 || lCharacterLevel > AGPMCHAR_MAX_LEVEL)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("AdjustCharMaxPoint"));

	INT32	lCurrentMaxHP	= 0;
	INT32	lCurrentMaxMP	= 0;
	INT32	lCurrentMaxSP	= 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	INT64	llCurrentMaxExp	= m_pagpmFactors->GetMaxExp(&pcsCharacter->m_csFactor);

	// adjust current point with max point
	INT32	lCurrentHP		= 0;
	INT32	lCurrentMP		= 0;
	INT32	lCurrentSP		= 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	INT64	llCurrentExp	= m_pagpmFactors->GetExp(&pcsCharacter->m_csFactor);

	if (lCurrentHP > lCurrentMaxHP)
	{
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		
		if (pcsCharacter->m_csFactor.m_pvFactor[AGPD_FACTORS_TYPE_RESULT])
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	}
	if (lCurrentMP > lCurrentMaxMP)
	{
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		
		if (pcsCharacter->m_csFactor.m_pvFactor[AGPD_FACTORS_TYPE_RESULT])
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	}
	if (lCurrentSP > lCurrentMaxSP)
	{
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
		
		if (pcsCharacter->m_csFactor.m_pvFactor[AGPD_FACTORS_TYPE_RESULT])
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	}
	if (llCurrentExp > llCurrentMaxExp)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] CurrentExp(%I64d) > MaxExp(%I64d)\n", pcsCharacter->m_szID, llCurrentExp, llCurrentMaxExp);
		AuLogFile_s("LOG\\AdjustCharPoint.txt", strCharBuff);

		m_pagpmFactors->SetExp(&pcsCharacter->m_csFactor, llCurrentMaxExp);
		
		if (pcsCharacter->m_csFactor.m_pvFactor[AGPD_FACTORS_TYPE_RESULT])
			m_pagpmFactors->SetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), llCurrentMaxExp);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Send Delete Complete
// Login Server -> Game Server 로 보낸다.
BOOL AgsmCharManager::SendDeleteCompletePacket(CHAR* szCharName, UINT32 ulNID)
{
	if(!szCharName || !ulNID)
		return FALSE;

	INT16 nPacketLength	= 0;
	INT8 cOperation = AGSMCHARMANAGER_PACKET_OPERATION_DELETE_COMPLETE;
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMCHARMANAGER_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											szCharName,
											NULL,
											NULL,
											NULL);

	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmCharManager::SendAuthKeyPacket(CHAR* szCharName, INT32 lAuthKey, UINT32 ulNID)
{
	if (!szCharName || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGSMCHARMANAGER_PACKET_OPERATION_AUTH_KEY;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMCHARMANAGER_PACKET_TYPE,
													&cOperation,
													NULL,
													NULL,
													szCharName,
													NULL,
													NULL,
													&lAuthKey);

	if (!pvPacket)
		return FALSE;

	BOOL	bResult			= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return	bResult;
}

//////////////////////////////////////////////////////////////////////////
// Log 관련 - 2004.05.02. steeple
BOOL AgsmCharManager::WriteLoginLog(AgpdCharacter *pAgpdCharacter)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pAgpdCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pAgpdCharacter);
	if(!pAgsdCharacter)
		return FALSE;

	INT32 lCharTID = ((AgpdCharacterTemplate*)pAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacter);

	return m_pagpmLog->WriteLog_Login(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID, pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID, lCharTID, lLevel, llExp,
									pAgpdCharacter->m_llMoney, pAgpdCharacter->m_llBankMoney
									);
}

BOOL AgsmCharManager::WriteLogoutBankLog(AgpdCharacter *pAgpdCharacter)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pAgpdCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pAgpdCharacter);
	if(!pAgsdCharacter)
		return FALSE;

	INT32 lCharTID = ((AgpdCharacterTemplate*)pAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacter);
	
	// Bank Log
	AgpdItemADChar* pAgpdItemADChar = m_pagpmItem->GetADCharacter(pAgpdCharacter);
	if (m_pagsmItem && pAgpdItemADChar)
	{
		AgpdGridItem *pAgpdGridItem = NULL;
		INT32 lIndex = 0;
		for (pAgpdGridItem = m_pagpmItem->m_pagpmGrid->GetItemSequence(&pAgpdItemADChar->m_csBankGrid, &lIndex);
			pAgpdGridItem;
			pAgpdGridItem = m_pagpmItem->m_pagpmGrid->GetItemSequence(&pAgpdItemADChar->m_csBankGrid, &lIndex))
		{
			AgpdItem *pAgpdItem = m_pagpmItem->GetItem(pAgpdGridItem);
			if (pAgpdItem && pAgpdItem->m_pcsCharacter)
			{
				m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_BANK_LOGOUT,
										pAgpdItem->m_pcsCharacter->m_lID,
										pAgpdItem,
										pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1
										);
			}
		}
	}
	return TRUE;
}

BOOL AgsmCharManager::WriteLogoutLog(AgpdCharacter *pAgpdCharacter)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pAgpdCharacter)
		return FALSE;

	// 게임 서버만 남긴다. 2008.01.11. steeple
	if(m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pAgpdCharacter);
	if(!pAgsdCharacter)
		return FALSE;

	INT32 lCharTID = ((AgpdCharacterTemplate*)pAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacter);
	
	// Play Log
	INT32 lPartyTime = (INT32) (m_pagsmCharacter->GetPartyPlayTimeMSec(pAgpdCharacter) / 1000 / 60);		// minute
	INT32 lSoloTime = (INT32) (m_pagsmCharacter->GetSoloPlayTimeMSec(pAgpdCharacter) / 1000 / 60);		// minute
	INT32 lKillPC = m_pagsmCharacter->GetKillMonCount(pAgpdCharacter);
	INT32 lKillMon = m_pagsmCharacter->GetKillPCCount(pAgpdCharacter);
	INT32 lDeadByPC = m_pagsmCharacter->GetDeadByMonCount(pAgpdCharacter);
	INT32 lDeadByMon = m_pagsmCharacter->GetDeadByPCCount(pAgpdCharacter);


	m_pagpmLog->WriteLog_Logout(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID, pAgsdCharacter->m_szServerName,
										pAgpdCharacter->m_szID, lCharTID, lLevel, llExp,
										pAgpdCharacter->m_llMoney, pAgpdCharacter->m_llBankMoney,
										lPartyTime, lSoloTime, lKillPC, lKillMon, lDeadByPC, lDeadByMon
										);

#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	// 2007.11.12. steeple
	// PC 방이던 아니던 다 남긴다.
	//if(m_pagpmBillInfo->IsPCBang(pAgpdCharacter))
	//{
		BOOL bTestServer = FALSE;
		if(GetGameEnv().IsAlpha())
			bTestServer = TRUE;

		INT64 llInvenMoney, llBankMoney;
		llInvenMoney = llBankMoney = 0;
		m_pagpmCharacter->GetMoney(pAgpdCharacter, &llInvenMoney);
		llBankMoney = m_pagpmCharacter->GetBankMoney(pAgpdCharacter);
		std::string strGameString = m_csHanIPCheck->GetGameString(bTestServer);
		m_pagpmLog->WriteLog_PCRoom(AuTimeStamp::GetCurrentTimeStamp(), strGameString.c_str(), pAgsdCharacter->m_szServerName,
									pAgsdCharacter->m_szAccountID, pAgsdCharacter->m_szServerName,
									m_pagsmCharacter->GetRealCharName(pAgpdCharacter->m_szID),
									pAgsdCharacter->m_ulConnectedTimeStamp, &pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_lLoginLevel, lLevel, llInvenMoney, llBankMoney,
									pAgsdCharacter->m_szCRMCode, pAgsdCharacter->m_szGrade);									
	//}
#endif
#endif
	return TRUE;
}
/*
BOOL AgsmCharManager::SendDummyPacket(INT32 lSize, INT32 lCount)
{
	PROFILE("AgsmCharManager::SendDummyPacket");

	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacter("치약묵자");

	UINT32	ulNID	= m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	CHAR	*szDummy	= (CHAR *) malloc(sizeof(CHAR) * lSize);

	while (1)
	{
		for (int i = 0; i < lCount; ++i)
		{
			SendPacket(szDummy, lSize, ulNID);
		}

		Sleep(2000);
	}

	return TRUE;
}
*/

BOOL AgsmCharManager::CBCheckPCRoomType(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCharManager *pThis			= (AgsmCharManager *) pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *) pData;
	INT32			*pulPCRoomType	= (INT32 *) pCustData;

	AgpdBillInfo *pcsAttachBillInfo	= pThis->m_pagpmBillInfo->GetADCharacter(pcsCharacter);
	if(pcsAttachBillInfo)
	{
		if(pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_S)
			*pulPCRoomType |= AGPDPCROOMTYPE_HANGAME_S;

		if(pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)
			*pulPCRoomType |= AGPDPCROOMTYPE_HANGAME_TPACK;
	}

	return TRUE;
}