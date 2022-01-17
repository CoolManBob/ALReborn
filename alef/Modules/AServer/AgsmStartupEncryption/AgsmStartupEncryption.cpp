// AgsmStartupEncryption.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.11.

#include "AgsmStartupEncryption.h"

#ifdef USE_GPK
	#ifdef _DEBUG
		#pragma comment(lib,"AuDynCodeServerD.lib")
	#else
		#pragma comment(lib,"AuDynCodeServer.lib")
	#endif
#endif

AgsmStartupEncryption::AgsmStartupEncryption()
{
	SetModuleName("AgsmStartupEncryption");
	SetModuleType(APMODULE_TYPE_SERVER);

	EnableIdle(FALSE);
	EnableIdle2(FALSE);

	m_pcsAgpmStartupEncryption = NULL;
	
	m_pcsAgsmAOIFilter = NULL;

	m_pcsAgpmConfig = NULL;

	m_pPublicKey = NULL;
	m_pEncryptedPublicKey = NULL;

	m_lPublicKeySize = 0;
	m_lEncryptedKeySize = 0;

	m_csStartupActor.SetCryptType(AUCRYPT_TYPE_PUBLIC);	// Startup 은 Public 만 쓴다.
}

AgsmStartupEncryption::~AgsmStartupEncryption()
{
	if(m_pPublicKey)
		delete [] m_pPublicKey;
	if(m_pEncryptedPublicKey)
		delete [] m_pEncryptedPublicKey;
}

BOOL AgsmStartupEncryption::OnAddModule()
{
	m_pcsAgpmStartupEncryption = (AgpmStartupEncryption*)GetModule("AgpmStartupEncryption");
	m_pcsAgsmAOIFilter = (AgsmAOIFilter*)GetModule("AgsmAOIFilter");

	m_pcsAgpmConfig = (AgpmConfig*)GetModule("AgpmConfig");

	if(!m_pcsAgpmStartupEncryption || !m_pcsAgsmAOIFilter)
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackRequestPublic(CBRequestPublic, this))
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackMakePrivate(CBMakePrivate, this))
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackCheckComplete(CBCheckComplete, this))
		return FALSE;

	// 서버의 Main Key 를 담을 버퍼 생성.
	m_lPublicKeySize = AuPacket::GetCryptManager().GetPublicKeySize();
	m_lEncryptedKeySize = AuPacket::GetCryptManager().GetOutputSize(AUCRYPT_TYPE_PUBLIC, m_lPublicKeySize);

	if(m_lPublicKeySize < 1 || m_lEncryptedKeySize < 1)
	{
		ASSERT(!"Key Size 초기화 에러");
		return FALSE;
	}

	m_pPublicKey = new BYTE[m_lPublicKeySize];
	m_pEncryptedPublicKey = new BYTE[m_lEncryptedKeySize];

	memset(m_pPublicKey, 0, m_lPublicKeySize);
	memset(m_pEncryptedPublicKey, 0, m_lEncryptedKeySize);

	return TRUE;
}

// 2006.06.14. steeple
// AgpmConfig 로 부터 읽어들인다.
BOOL AgsmStartupEncryption::SetAlgorithm()
{
	if(!m_pcsAgpmConfig)
		return FALSE;

	LPCTSTR szPublicAlgorithm = m_pcsAgpmConfig->GetEncPublic();
	if(szPublicAlgorithm && _tcslen(szPublicAlgorithm) > 0)
	{
		if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_BLOWFISH, _tcslen(AUCRYPT_ALGORITHM_NAME_BLOWFISH)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_BLOWFISH);
		else if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_MD5MAC, _tcslen(AUCRYPT_ALGORITHM_NAME_MD5MAC)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_MD5MAC);
		else if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_RIJNDAEL, _tcslen(AUCRYPT_ALGORITHM_NAME_RIJNDAEL)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_RIJNDAEL);
		else if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_SEED, _tcslen(AUCRYPT_ALGORITHM_NAME_SEED)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_SEED);
		else if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_DYNCODE, _tcslen(AUCRYPT_ALGORITHM_NAME_DYNCODE)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_DYNCODE);
	}

	LPCTSTR szPrivateAlgorithm = m_pcsAgpmConfig->GetEncPrivate();
	if(szPrivateAlgorithm && _tcslen(szPrivateAlgorithm) > 0)
	{
		if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_BLOWFISH, _tcslen(AUCRYPT_ALGORITHM_NAME_BLOWFISH)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_BLOWFISH);
		else if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_MD5MAC, _tcslen(AUCRYPT_ALGORITHM_NAME_MD5MAC)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_MD5MAC);
		else if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_RIJNDAEL, _tcslen(AUCRYPT_ALGORITHM_NAME_RIJNDAEL)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_RIJNDAEL);
		else if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_SEED, _tcslen(AUCRYPT_ALGORITHM_NAME_SEED)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_SEED);
		else if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_DYNCODE, _tcslen(AUCRYPT_ALGORITHM_NAME_DYNCODE)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_DYNCODE);
	}

	// Print Selected Algorithm
	szPublicAlgorithm = AuPacket::GetCryptManager().GetPublicAlgorithmString();
	printf("\nPublic  Encryption Algorithm : %s\n", szPublicAlgorithm ? szPublicAlgorithm : _T("None"));
	szPrivateAlgorithm = AuPacket::GetCryptManager().GetPrivateAlgorithmString();
	printf("Private Encryption Algorithm : %s\n\n", szPrivateAlgorithm ? szPrivateAlgorithm : _T("None"));

	return TRUE;
}

// 랜덤 키를 생성해서 시작한다.
BOOL AgsmStartupEncryption::InitializePublicKey()
{
	AuPacket::GetCryptManager().MakeRandomKey(m_pPublicKey, m_lPublicKeySize);
	return InitializePublicKey(m_pPublicKey, m_lPublicKeySize);
}

BOOL AgsmStartupEncryption::InitializePublicKey(BYTE* pKey, INT32 lSize)
{
	if(!pKey || lSize < 1 || lSize > m_lPublicKeySize)
		return FALSE;

	// 이미 세팅되서 넘어온 게 아니라면 카피
	if(m_pPublicKey != pKey)
		memcpy(m_pPublicKey, pKey, lSize);

#ifdef USE_GPK
	// DynCode 를 사용중일 때는 이런식으로 다른 함수를 부른다.
	if(AuPacket::GetCryptManager().IsUseDynCode())
	{
		return InitializePublicKeyAtDynCode();
	}
#endif//USE_GPK

	AuPacket::GetCryptManager().InitializeActor(m_csStartupActor, m_pPublicKey, m_lPublicKeySize);

	m_lEncryptedKeySize = AuPacket::GetCryptManager().Encrypt(m_csStartupActor, m_pPublicKey, m_pEncryptedPublicKey, m_lPublicKeySize);

	// 테스트 용으로 한번 복호화 해봤다.
	//pEngine->m_csCryptManager.Decrypt(m_csStartupActor, m_pEncryptedPublicKey, m_pEncryptedPublicKey, lSize);

	// 멀티캐스트용 CryptActor 를 암호화된 Public Key 로 초기화한다.
	// 모든 소켓의 Public 도 암호화된 Public Key 로 초기화한다.

	// 클론을 사용하는 알고리즘이라면 뒤에 TRUE
	if(AuPacket::GetCryptManager().IsPublicClone())
		AuPacket::GetCryptManager().InitializeActorPublic(m_pcsAgsmAOIFilter->GetCryptActorRef(), m_pEncryptedPublicKey, m_lEncryptedKeySize, TRUE);
	else
		AuPacket::GetCryptManager().InitializeActorPublic(m_pcsAgsmAOIFilter->GetCryptActorRef(), m_pEncryptedPublicKey, m_lEncryptedKeySize);

	return TRUE;
}

#ifdef USE_GPK
// 2006.06.10. steeple
// DynCode Initialize
// DynCode 는 LIbrary 에서 Key 를 뽑아내므로, 서버 시작시에 입력받은 key 는 무시된다.
// 또한, Key 를 뽑아오는 데 필요한 index 를 DYNCODE_CTX 에 저장하고,
// Public Crypt Actor 와 AOIFilter 에 있는 Public Crypt Actor 의 Index 를 맞춰주는 작업을 한다.
BOOL AgsmStartupEncryption::InitializePublicKeyAtDynCode()
{
	// Init Server
	if(!AuPacket::GetCryptManager().InitServer())
		return FALSE;

	// Key 와 KeySize 는 무시된다.
	AuPacket::GetCryptManager().InitializeActorPublic(m_csStartupActor, m_pPublicKey, m_lPublicKeySize);

	// AOI Filter 쪽에는 Copy 해준다.
	AuPacket::GetCryptManager().InitializeActorPublic(m_pcsAgsmAOIFilter->GetCryptActorRef(), m_csStartupActor);

	return TRUE;
}

// 2006.06.10. steeple
// 클라이언트가 접속을 하면 불리게 되는 함수.
// Client Socket 의 Crypt Actor 에 Public, Private 모두 세팅해준다.
// 그런 후 완료했다고 패킷 보낸다.
BOOL AgsmStartupEncryption::InitializePrivateKeyAtDynCode(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	AgsEngine* pEngine = static_cast<AgsEngine*>(GetModuleManager());
	if(!pEngine)
		return FALSE;

	AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[ulNID];
	if(!pSocket)
		return FALSE;

	// 해당 소켓의 Public 초기화. Member Actor 를 카피한다.
	AuPacket::GetCryptManager().InitializeActorPublic(pSocket->GetCryptActorRef(), m_csStartupActor);

	// Private 는 실 초기화를 한다. Key 와 KeySize 는 무시된다.
	AuPacket::GetCryptManager().InitializeActorPrivate(pSocket->GetCryptActorRef(), m_pPublicKey, m_lPublicKeySize);

	SendDynCodePublic(ulNID);
	SendDynCodePrivate(ulNID);

	// 해당 클라이언트에게 암호화 초기화를 완료 했다고 알린다.
	BOOL bResult = SendComplete(ulNID);

	if(bResult)
	{
		// private 로 설정.
		pSocket->GetCryptActorRef().SetCryptType(AUCRYPT_TYPE_PRIVATE);
	}

	return bResult;
}
#endif//USE_GPK

BOOL AgsmStartupEncryption::SendAlgorithmType(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	// 걍 클라이언트로 현재 알고리즘 타입 보내준다.
	BYTE pData[sizeof(INT32) * 2];
	INT32 lPublicAlgorithm = (INT32)AuPacket::GetCryptManager().GetPublicAlgorithm();
	INT32 lPrivateAlgorithm = (INT32)AuPacket::GetCryptManager().GetPrivateAlgorithm();

	memcpy(pData, &lPublicAlgorithm, sizeof(INT32));
	memcpy(pData + sizeof(INT32), &lPrivateAlgorithm, sizeof(INT32));

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(&nPacketLength,
														AGPMSTARTUPENCRYPTION_PACKET_ALGORITHM_TYPE,
														pData,
														sizeof(INT32) * 2);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmStartupEncryption::SendEncryptedPublicKey(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(&nPacketLength,
														AGPMSTARTUPENCRYPTION_PACKET_PUBLIC,
														m_pEncryptedPublicKey,
														m_lEncryptedKeySize);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmStartupEncryption::SendComplete(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(
													&nPacketLength,
													AGPMSTARTUPENCRYPTION_PACKET_COMPLETE
													);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmStartupEncryption::SendDynCodePublic(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	AgsEngine* pEngine = static_cast<AgsEngine*>(GetModuleManager());
	if(!pEngine)
		return FALSE;

	AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[ulNID];
	if(!pSocket)
		return FALSE;

	const BYTE* pCode = NULL;
	INT32 lCodeSize = 0;

	lCodeSize = AuPacket::GetCryptManager().GetClientDynCodePublic(pSocket->GetCryptActorRef(), &pCode);
	if(lCodeSize < 1)
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(
													&nPacketLength,
													AGPMSTARTUPENCRYPTION_PACKET_DYNCODE_PUBLIC,
													const_cast<BYTE*>(pCode),
													(INT16)lCodeSize);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmStartupEncryption::SendDynCodePrivate(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	AgsEngine* pEngine = static_cast<AgsEngine*>(GetModuleManager());
	if(!pEngine)
		return FALSE;

	AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[ulNID];
	if(!pSocket)
		return FALSE;

	const BYTE* pCode = NULL;
	INT32 lCodeSize = 0;

	lCodeSize = AuPacket::GetCryptManager().GetClientDynCodePrivate(pSocket->GetCryptActorRef(), &pCode);
	if(lCodeSize < 1)
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(
													&nPacketLength,
													AGPMSTARTUPENCRYPTION_PACKET_DYNCODE_PRIVATE,
													const_cast<BYTE*>(pCode),
													(INT16)lCodeSize);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmStartupEncryption::CBRequestPublic(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pCustData)
		return FALSE;

	// 클라이언트에게 Public 키를 보낸다.
	AgsmStartupEncryption* pThis = static_cast<AgsmStartupEncryption*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	//AuLogFile("Enc.log", "sm;1-1 NID:%u\n", ulNID);

	// 2006.06.14. steeple
	// 암호화 알고리즘부터 보내준다.
	pThis->SendAlgorithmType(ulNID);

#ifdef USE_GPK
	// DynCode 를 사용한다면, 여기서 다르게 처리해준다.
	if(AuPacket::GetCryptManager().IsUseDynCode())
	{
		//AuLogFile("Enc.log", "sm;1-2 NID:%u\n", ulNID);

		return pThis->InitializePrivateKeyAtDynCode(ulNID);
	}
#endif
	//AuLogFile("Enc.log", "sm;1-3 NID:%u\n", ulNID);
	return pThis->SendEncryptedPublicKey(ulNID);
}

BOOL AgsmStartupEncryption::CBMakePrivate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	// 클라이언트에서 받은 값을 가지고 개인키를 만든다.
	PVOID* ppvBuffer = static_cast<PVOID*>(pData);
	AgsmStartupEncryption* pThis = static_cast<AgsmStartupEncryption*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	BYTE* pKey = static_cast<BYTE*>(ppvBuffer[0]);
	INT16 nSize = *static_cast<INT16*>(ppvBuffer[1]);

	//AuLogFile("Enc.log", "sm;2-1 NID:%u\n", ulNID);

	AgsEngine* pEngine = static_cast<AgsEngine*>(pThis->GetModuleManager());
	if(!pEngine)
		return FALSE;

	AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[ulNID];
	if(!pSocket)
		return FALSE;

	//AuLogFile("Enc.log", "sm;2-2 NID:%u\n", ulNID);

	// 해당 소켓의 Public 초기화
	// 클론을 사용하는 알고리즘이라면 뒤에 TRUE
	if(AuPacket::GetCryptManager().IsPublicClone())
		AuPacket::GetCryptManager().InitializeActorPublic(pSocket->GetCryptActorRef(), pThis->m_pEncryptedPublicKey, pThis->m_lEncryptedKeySize, TRUE);
	else
		AuPacket::GetCryptManager().InitializeActorPublic(pSocket->GetCryptActorRef(), pThis->m_pEncryptedPublicKey, pThis->m_lEncryptedKeySize);

	INT32 lOutputSize = AuPacket::GetCryptManager().GetOutputSize(AUCRYPT_TYPE_PUBLIC, (INT32)nSize);
	//AuLogFile("Enc.log", "sm;2-3 NID:%u, lOutputSize:%d\n", ulNID, lOutputSize);
	if(lOutputSize < 1)
		return FALSE;

	// 복호화 할 버퍼 생성.
	BYTE* pOutput = new BYTE[lOutputSize];

	AuPacket::GetCryptManager().DecryptByPublic(pSocket->GetCryptActorRef(), pKey, pOutput, (INT32)nSize);

	// 앞으로 해당 클라이언트의 키는 pOutput 값이 된다.
	// 그러므로 해당 소켓에 있는 Actor 에 pOutput 으로 Initialize 해준다.
	if(AuPacket::GetCryptManager().IsPrivateClone())
		AuPacket::GetCryptManager().InitializeActorPrivate(pSocket->GetCryptActorRef(), pOutput, (INT32)nSize, TRUE);
	else
		AuPacket::GetCryptManager().InitializeActorPrivate(pSocket->GetCryptActorRef(), pOutput, (INT32)nSize);

	delete [] pOutput;

	//AuLogFile("Enc.log", "sm;2-4 NID:%u\n", ulNID, lOutputSize);

	// 해당 클라이언트에게 암호화 초기화를 완료 했다고 알린다.
	BOOL bResult = pThis->SendComplete(ulNID);

	if(bResult)
	{
		//AuLogFile("Enc.log", "sm;2-5 NID:%u\n", ulNID, lOutputSize);

		// private 로 설정.
		pSocket->GetCryptActorRef().SetCryptType(AUCRYPT_TYPE_PRIVATE);
	}

	//AuLogFile("Enc.log", "sm;2-6 NID:%u\n", ulNID, lOutputSize);
	return bResult;
}

BOOL AgsmStartupEncryption::CBCheckComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	BOOL* pbResult = static_cast<BOOL*>(pData);
	AgsmStartupEncryption* pThis = static_cast<AgsmStartupEncryption*>(pClass);
	UINT32* pulNID = static_cast<UINT32*>(pCustData);

	AgsEngine* pEngine = static_cast<AgsEngine*>(pThis->GetModuleManager());
	if(!pEngine)
	{
		*pbResult = FALSE;
		return FALSE;
	}

	AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[*pulNID];
	if(!pSocket)
	{
		*pbResult = FALSE;
		return FALSE;
	}

	// 암호화를 해야 하는 상황인데 해당 소켓이 암호화 시작 상태가 아니라면 FALSE 를 대입해준다.
	if(AuPacket::GetCryptManager().IsUseCrypt() &&
		pSocket->GetCryptActorRef().GetCryptType() == AUCRYPT_TYPE_NONE)
	{
		*pbResult = FALSE;
		return TRUE;
	}

	return TRUE;
}