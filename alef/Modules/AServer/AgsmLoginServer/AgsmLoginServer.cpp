/*=============================================================================

	AgsmLogin.cpp

=============================================================================*/


#include "AgsmLoginServer.h"


/************************************************************/
/*		The Implementation of Login Queue Manager class		*/
/************************************************************/
//
AgsmLoginServer::AgsmLoginServer()
	{
	SetModuleName(_T("AgsmLoginServer"));
	SetModuleType(APMODULE_TYPE_SERVER);
	//SetPacketType(AGSMLOGIN_PACKET_TYPE);//JK_중복로그인

	m_pcQueue = new CQueue<PVOID>;
	m_ulMaxQueueCount = 10000000;
	}


AgsmLoginServer::~AgsmLoginServer()
	{
	if (NULL != m_pcQueue)
		{
		delete m_pcQueue;
		m_pcQueue	= NULL;
		}
	}




//	ApModule inherited
//=============================================
//
BOOL AgsmLoginServer::OnAddModule()
	{
	m_pAgpmLogin = (AgpmLogin *) GetModule(_T("AgpmLogin"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgsmCharacter = (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	
	if (!m_pAgpmLogin || !m_pAgpmCharacter || !m_pAgsmCharacter)
		{
		return FALSE;
		}

	return TRUE;
	}




//	Packet send
//===========================================
//
//	Ghost/Duplicated Account를 지워달라고 요청한다.
BOOL AgsmLoginServer::SendRemoveDuplicateAccount(TCHAR *pszAccountID, INT32 lCID, UINT32 ulServerNID)
	{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMLOGIN_REMOVE_DUPLICATED_ACCOUNT;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMLOGIN_PACKET_TYPE,
													&cOperation,		// lOperation
													NULL,				// EncryptCode
													pszAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													&lCID,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulServerNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}
	
	return bResult;
	}


BOOL AgsmLoginServer::SendRenameCharacter(TCHAR *pszNewChar, TCHAR *pszOldChar, UINT32 ulServerNID)
	{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMLOGIN_RENAME_CHARACTER;
	INT16	nPacketLength;

	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGSMLOGIN_PACKET_TYPE,
																		NULL,
																		pszNewChar,		// char id
																		NULL,
																		NULL,
																		NULL,
																		NULL,
																		NULL,
																		NULL,
																		pszOldChar		// old char id
																		);

	if (NULL == pvDetailCharInfo)
		return FALSE;

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMLOGIN_PACKET_TYPE,
														&cOperation,		// lOperation
														NULL,				// EncryptCode
														NULL,				// AccountID
														NULL,				// AccountID Len
														NULL,				// AccountPassword
														NULL,				// AccountPassword Len
														NULL,				// lCID
														NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
														pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
														NULL,				// pvDetailServerInfo; 서버군 정보.
														NULL,				// lResult대신 NID보냈음.
														NULL,
														NULL,
														NULL,
														NULL,				// isLimited
														NULL				// isProtected
														);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulServerNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}




//	Queue
//===================================================
//
BOOL AgsmLoginServer::CheckQueueCount()
	{
	if (NULL == m_pcQueue)
		return FALSE;

	if ((m_ulMaxQueueCount/(UINT16)2) >= m_pcQueue->getCount())
		return TRUE;

	return FALSE;
	}


BOOL AgsmLoginServer::CheckAndPushToQueue(PVOID pvQuery, TCHAR *pszFile, INT32 lLine)
	{
	if (CheckQueueCount())
		return PushToQueue(pvQuery, pszFile, lLine);
		
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginServer::CheckAndPushToQueue() - CheckQueueCount Error [%s][%d]\n", pszFile, lLine);
	AuLogFile_s("LOG\\AgsmLoginServer.log", strCharBuff);
	
	delete pvQuery;

	return FALSE;
	}


BOOL AgsmLoginServer::PushToQueue(PVOID pvQuery, TCHAR *pszFile, INT32 lLine)
	{
	if (NULL != m_pcQueue)
		return m_pcQueue->push(pvQuery, pszFile, lLine);

	return FALSE;
	}


PVOID AgsmLoginServer::PopFromQueue()
	{
	PVOID pvQuery = NULL;

	if (NULL != m_pcQueue)
		pvQuery = m_pcQueue->pop();

	return pvQuery;
	}


