// AuCryptManager.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.07.

#include "AuCryptManager.h"
#include <algorithm>

// 기본값은 BlowFish 로 세팅
#ifdef _USE_GPK_
	const eAuCryptAlgorithm AUCRTYP_DEFAULT_ALGORITHM_PUBLIC	= AUCRYPT_ALGORITHM_DYNCODE;
	const eAuCryptAlgorithm AUCRTYP_DEFAULT_ALGORITHM_PRIVATE	= AUCRYPT_ALGORITHM_DYNCODE;	
#else
	const eAuCryptAlgorithm AUCRTYP_DEFAULT_ALGORITHM_PUBLIC	= AUCRYPT_ALGORITHM_BLOWFISH;
	const eAuCryptAlgorithm AUCRTYP_DEFAULT_ALGORITHM_PRIVATE	= AUCRYPT_ALGORITHM_BLOWFISH;
#endif

AuCryptManager::AuCryptManager() : m_ePublicAlgorithm(AUCRTYP_DEFAULT_ALGORITHM_PUBLIC),
									m_ePrivateAlgorithm(AUCRTYP_DEFAULT_ALGORITHM_PRIVATE),
									m_bUseCrypt(TRUE),
									m_eCryptCheckLevel(AUCRYPT_CHECK_LEVEL_HIGH),
									m_lPublicKeySize(AUCRYPT_KEY_LENGTH_32BYTE),
									m_lPrivateKeySize(AUCRYPT_KEY_LENGTH_32BYTE)
{
	// 각 알고리즘의 Instance 를 생성한다.
	m_vcAlgorithm.push_back(new AuBlowFish());
	m_vcAlgorithm.push_back(new AuMD5Mac());
	m_vcAlgorithm.push_back(new AuRijnDael());
	m_vcAlgorithm.push_back(new AuSeed());

#ifdef _USE_GPK_
	// DynCode 알고리즘은 가장 마지막에 추가한다.
	m_vcAlgorithm.push_back(new AuDynCode());
#endif
}

AuCryptManager::~AuCryptManager()
{
	// Vector 를 비운다.
	std::for_each(m_vcAlgorithm.begin(), m_vcAlgorithm.end(), DeletePtr());
	m_vcAlgorithm.clear();
}

// 기본적으로 생성자에서 할당이 되는데, 임의로 바꾸고 싶으면 이렇게 한다.
void AuCryptManager::SetAlgorithm(eAuCryptAlgorithm ePublic, eAuCryptAlgorithm ePrivate)
{
	SetPublicAlgorithm(ePublic);
	SetPrivateAlgorithm(ePrivate);
}

LPCTSTR AuCryptManager::GetPublicAlgorithmString()
{
	return GetAlgorithmString(m_ePublicAlgorithm);
}

LPCTSTR AuCryptManager::GetPrivateAlgorithmString()
{
	return GetAlgorithmString(m_ePrivateAlgorithm);
}

LPCTSTR AuCryptManager::GetAlgorithmString(eAuCryptAlgorithm eAlgorithm)
{
	switch(eAlgorithm)
	{
		case AUCRYPT_ALGORITHM_BLOWFISH:		return AUCRYPT_ALGORITHM_NAME_BLOWFISH;
		case AUCRYPT_ALGORITHM_MD5MAC:			return AUCRYPT_ALGORITHM_NAME_MD5MAC;
		case AUCRYPT_ALGORITHM_RIJNDAEL:		return AUCRYPT_ALGORITHM_NAME_RIJNDAEL;
		case AUCRYPT_ALGORITHM_SEED:			return AUCRYPT_ALGORITHM_NAME_SEED;
		case AUCRYPT_ALGORITHM_DYNCODE:			return AUCRYPT_ALGORITHM_NAME_DYNCODE;
		default:								return NULL;
	}

	return NULL;
}

// round key 를 같은 것을 써야 하는 지 리턴.
BOOL AuCryptManager::IsPublicClone()
{
	// 사용하는 알고리즘으 계속 추가해준다.
	if(m_ePublicAlgorithm == AUCRYPT_ALGORITHM_MD5MAC)
		return TRUE;

	return FALSE;
}

// round key 를 같은 것을 써야 하는 지 리턴.
BOOL AuCryptManager::IsPrivateClone()
{
	// 사용하는 알고리즘으 계속 추가해준다.
	if(m_ePrivateAlgorithm == AUCRYPT_ALGORITHM_MD5MAC)
		return TRUE;

	return FALSE;
}

// Source 를 그대로 Copy 하는 것인지 확인.
BOOL AuCryptManager::IsPublicCopy()
{
	if(m_ePublicAlgorithm == AUCRYPT_ALGORITHM_DYNCODE)
		return TRUE;

	return FALSE;
}

// Source 를 그대로 Copy 하는 것인지 확인.
BOOL AuCryptManager::IsPrivateCopy()
{
	if(m_ePrivateAlgorithm == AUCRYPT_ALGORITHM_DYNCODE)
		return TRUE;

	return FALSE;
}

// Decrypt 할 때 버퍼가 필요한 지 여부
BOOL AuCryptManager::UseDecryptBuffer(eAuCryptType eCryptType)
{
	eAuCryptAlgorithm eAlgorithm = eCryptType == AUCRYPT_TYPE_PUBLIC ? m_ePublicAlgorithm : m_ePrivateAlgorithm;

	switch(eAlgorithm)
	{
		case AUCRYPT_ALGORITHM_SEED:
			return TRUE;

		default:
			return FALSE;
	}

	return FALSE;
}

// pKey 에 Random 키를 넣어준다.
// 버퍼 크기가 제대로 넘어오지 않으면 문제가 생길 수 있다.
void AuCryptManager::MakeRandomKey(BYTE* pKey, INT32 lSize)
{
	if(!pKey || lSize < 1)
		return;

	// lSize 가 Public, Private 크기랑 모두 다르다면 그냥 return
	if(lSize != m_lPublicKeySize && lSize != m_lPrivateKeySize)
	{
		ASSERT(lSize != m_lPublicKeySize && lSize != m_lPrivateKeySize);
		return;
	}

	srand(timeGetTime());
	for(int i = 0; i < lSize; i++)
		pKey[i] = (BYTE)(rand() % 256);

	return;
}

BOOL AuCryptManager::InitializeActor(AuCryptActor& csCryptActor, BYTE* pKey, INT32 lKeySize, BOOL bClone)
{
	if(!pKey || lKeySize < 1)
		return FALSE;

	switch(csCryptActor.GetCryptType())
	{
		case AUCRYPT_TYPE_PUBLIC:
			return InitializeActorPublic(csCryptActor, pKey, lKeySize, bClone);

		case AUCRYPT_TYPE_PRIVATE:
			return InitializeActorPrivate(csCryptActor, pKey, lKeySize, bClone);

		default:
			return TRUE;
	}

	return TRUE;
}

BOOL AuCryptManager::InitializeActorPublic(AuCryptActor& csCryptActor, BYTE* pKey, INT32 lKeySize, BOOL bClone)
{
	if(!pKey || lKeySize < 1)
		return FALSE;

	if(m_vcAlgorithm[m_ePublicAlgorithm] == NULL)
		return FALSE;

	if(csCryptActor.GetPublicKeyRef().GetKeyPtr() == NULL)
		InitializeCTX(csCryptActor.GetPublicKeyRef());

	m_vcAlgorithm[m_ePublicAlgorithm]->Initialize(csCryptActor.GetPublicKeyRef().GetKeyPtr(), pKey, lKeySize, bClone);
	return TRUE;
}

BOOL AuCryptManager::InitializeActorPrivate(AuCryptActor& csCryptActor, BYTE* pKey, INT32 lKeySize, BOOL bClone)
{
	if(!pKey || lKeySize < 1)
		return FALSE;

	if(m_vcAlgorithm[m_ePrivateAlgorithm] == NULL)
		return FALSE;

	if(csCryptActor.GetPrivateKeyRef().GetKeyPtr() == NULL)
		InitializeCTX(csCryptActor.GetPrivateKeyRef(), AUCRYPT_TYPE_PRIVATE);

	m_vcAlgorithm[m_ePrivateAlgorithm]->Initialize(csCryptActor.GetPrivateKeyRef().GetKeyPtr(), pKey, lKeySize, bClone);
	return TRUE;
}

BOOL AuCryptManager::InitializeActor(AuCryptActor& csCryptActor, AuCryptActor& csSourceActor)
{
	switch(csCryptActor.GetCryptType())
	{
		case AUCRYPT_TYPE_PUBLIC:
			return InitializeActorPublic(csCryptActor, csSourceActor);

		case AUCRYPT_TYPE_PRIVATE:
			return InitializeActorPrivate(csCryptActor, csSourceActor);

		default:
			return TRUE;
	}

	return TRUE;
}

BOOL AuCryptManager::InitializeActorPublic(AuCryptActor& csCryptActor, AuCryptActor& csSourceActor)
{
	if(m_vcAlgorithm[m_ePublicAlgorithm] == NULL)
		return FALSE;

	if(csCryptActor.GetPublicKeyRef().GetKeyPtr() == NULL)
		InitializeCTX(csCryptActor.GetPublicKeyRef());

	m_vcAlgorithm[m_ePublicAlgorithm]->Initialize(csCryptActor.GetPublicKeyRef().GetKeyPtr(),
													csSourceActor.GetPublicKeyRef().GetKeyPtr());
	return TRUE;
}

BOOL AuCryptManager::InitializeActorPrivate(AuCryptActor& csCryptActor, AuCryptActor& csSourceActor)
{
	if(m_vcAlgorithm[m_ePrivateAlgorithm] == NULL)
		return FALSE;

	if(csCryptActor.GetPrivateKeyRef().GetKeyPtr() == NULL)
		InitializeCTX(csCryptActor.GetPrivateKeyRef(), AUCRYPT_TYPE_PRIVATE);

	m_vcAlgorithm[m_ePrivateAlgorithm]->Initialize(csCryptActor.GetPrivateKeyRef().GetKeyPtr(), 
													csSourceActor.GetPublicKeyRef().GetKeyPtr());
	return TRUE;
}

BOOL AuCryptManager::InitializeCTX(AuCryptCell& csKeyInfo, eAuCryptType eCryptType)
{
	eAuCryptAlgorithm eAlgorithm;
	if(eCryptType == AUCRYPT_TYPE_PUBLIC)
		eAlgorithm = m_ePublicAlgorithm;
	else if(eCryptType == AUCRYPT_TYPE_PRIVATE)
		eAlgorithm = m_ePrivateAlgorithm;
	else
		return FALSE;

	switch(eAlgorithm)
	{
		case AUCRYPT_ALGORITHM_BLOWFISH:
			csKeyInfo.SetKey(new BLOWFISH_CTX);
			csKeyInfo.SetKeyInfoSize(sizeof(BLOWFISH_CTX));
			break;

		case AUCRYPT_ALGORITHM_MD5MAC:
			csKeyInfo.SetKey(new MD5MAC_CTX);
			csKeyInfo.SetKeyInfoSize(sizeof(MD5MAC_CTX));
			break;

		case AUCRYPT_ALGORITHM_RIJNDAEL:
			csKeyInfo.SetKey(new RIJNDAEL_CTX);
			csKeyInfo.SetKeyInfoSize(sizeof(RIJNDAEL_CTX));
			break;

		case AUCRYPT_ALGORITHM_SEED:
			csKeyInfo.SetKey(new SEED_CTX);
			csKeyInfo.SetKeyInfoSize(sizeof(SEED_CTX));
			break;

#ifdef _USE_GPK_
		case AUCRYPT_ALGORITHM_DYNCODE:
			csKeyInfo.SetKey(new DYNCODE_CTX);
			csKeyInfo.SetKeyInfoSize(sizeof(DYNCODE_CTX));
			break;
#endif

		default:
			return FALSE;
	}

	return TRUE;
}

// 리턴값은 pOutput 의 size
UINT32 AuCryptManager::Encrypt(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize)
{
	switch(csCryptActor.GetCryptType())
	{
		case AUCRYPT_TYPE_PUBLIC:
			return EncryptByPublic(csCryptActor, pInput, pOutput, lInputSize);
			break;

		case AUCRYPT_TYPE_PRIVATE:
			return EncryptByPrivate(csCryptActor, pInput, pOutput, lInputSize);
			break;

		case AUCRYPT_TYPE_NONE:
		default:
			return 0;
	}
}

void AuCryptManager::Decrypt(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize)
{
	switch(csCryptActor.GetCryptType())
	{
		case AUCRYPT_TYPE_PUBLIC:
			DecryptByPublic(csCryptActor, pInput, pOutput, lInputSize);
			break;

		case AUCRYPT_TYPE_PRIVATE:
			DecryptByPrivate(csCryptActor, pInput, pOutput, lInputSize);
			break;

		case AUCRYPT_TYPE_NONE:
		default:
			break;
	}
}

// 편의성을 위해 제공하는 API
UINT32 AuCryptManager::Encrypt(AuCryptActor& csCryptActor, eAuCryptType eCryptType, BYTE* pInput, BYTE* pOutput, INT32 lInputSize)
{
	switch(eCryptType)
	{
		case AUCRYPT_TYPE_PUBLIC:
			return EncryptByPublic(csCryptActor, pInput, pOutput, lInputSize);
			break;

		case AUCRYPT_TYPE_PRIVATE:
			return EncryptByPrivate(csCryptActor, pInput, pOutput, lInputSize);
			break;

		case AUCRYPT_TYPE_NONE:
		default:
			return 0;
	}
}

// 편의성을 위해 제공하는 API
void AuCryptManager::Decrypt(AuCryptActor& csCryptActor, eAuCryptType eCryptType, BYTE* pInput, BYTE* pOutput, INT32 lInputSize)
{
	switch(eCryptType)
	{
		case AUCRYPT_TYPE_PUBLIC:
			DecryptByPublic(csCryptActor, pInput, pOutput, lInputSize);
			break;

		case AUCRYPT_TYPE_PRIVATE:
			DecryptByPrivate(csCryptActor, pInput, pOutput, lInputSize);
			break;

		case AUCRYPT_TYPE_NONE:
		default:
			break;
	}
}

// csCryptActor 의 Type 에 상관없이 강제로 진행
UINT32 AuCryptManager::EncryptByPublic(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize)
{
	// 일단 input 이 없으면 에러
	if(!pInput)
		return 0;

	if(m_vcAlgorithm[m_ePublicAlgorithm] == NULL)
		return 0;

	//if(csCryptActor.GetCryptType() == AUCRYPT_TYPE_NONE)
	//	return 0;

	return m_vcAlgorithm[m_ePublicAlgorithm]->Encrypt(csCryptActor.GetPublicKeyRef().GetKeyPtr(), pInput, pOutput, lInputSize);
}

// csCryptActor 의 Type 에 상관없이 강제로 진행
void AuCryptManager::DecryptByPublic(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize)
{
	// 일단 input 이 없으면 에러
	if(!pInput)
		return;

	if(m_vcAlgorithm[m_ePublicAlgorithm] == NULL)
		return;

	//if(csCryptActor.GetCryptType() == AUCRYPT_TYPE_NONE)
	//	return;

	m_vcAlgorithm[m_ePublicAlgorithm]->Decrypt(csCryptActor.GetPublicKeyRef().GetKeyPtr(), pInput, pOutput, lInputSize);
}

// csCryptActor 의 Type 에 상관없이 강제로 진행
UINT32 AuCryptManager::EncryptByPrivate(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize)
{
	// 일단 input 이 없으면 에러
	if(!pInput)
		return 0;

	if(m_vcAlgorithm[m_ePrivateAlgorithm] == NULL)
		return 0;

	//if(csCryptActor.GetCryptType() == AUCRYPT_TYPE_NONE)
	//	return 0;

	return m_vcAlgorithm[m_ePrivateAlgorithm]->Encrypt(csCryptActor.GetPrivateKeyRef().GetKeyPtr(), pInput, pOutput, lInputSize);
}

// csCryptActor 의 Type 에 상관없이 강제로 진행
void AuCryptManager::DecryptByPrivate(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize)
{
	// 일단 input 이 없으면 에러
	if(!pInput)
		return;

	if(m_vcAlgorithm[m_ePrivateAlgorithm] == NULL)
		return;

	//if(csCryptActor.GetCryptType() == AUCRYPT_TYPE_NONE)
	//	return;

	m_vcAlgorithm[m_ePrivateAlgorithm]->Decrypt(csCryptActor.GetPrivateKeyRef().GetKeyPtr(), pInput, pOutput, lInputSize);
}

INT32 AuCryptManager::GetOutputSize(eAuCryptType eCryptType, INT32 lInputSize)
{
	if(eCryptType == AUCRYPT_TYPE_PUBLIC && m_vcAlgorithm[m_ePublicAlgorithm] != NULL)
		return m_vcAlgorithm[m_ePublicAlgorithm]->GetOutputSize(lInputSize);
	else if(eCryptType == AUCRYPT_TYPE_PRIVATE && m_vcAlgorithm[m_ePrivateAlgorithm] != NULL)
		return m_vcAlgorithm[m_ePrivateAlgorithm]->GetOutputSize(lInputSize);

	return 0;
}


// 장퀘용 Dynamic Code... 아 얘때메 구조가 바꼈다.. 짜증난다. -ㅠ-
BOOL AuCryptManager::IsUseDynCode()
{
#ifdef _USE_GPK_
	if(m_ePublicAlgorithm == AUCRYPT_ALGORITHM_DYNCODE && m_ePrivateAlgorithm == AUCRYPT_ALGORITHM_DYNCODE)
		return TRUE;
#else
	return FALSE;
#endif
}

BOOL AuCryptManager::InitServer()
{
#ifdef _USE_GPK_
	if(!IsUseDynCode())
		return FALSE;

	AuDynCode* pcsDynCodePublic = static_cast<AuDynCode*>(m_vcAlgorithm[m_ePublicAlgorithm]);
	if(!pcsDynCodePublic)
		return FALSE;

	AuDynCode* pcsDynCodePrivate = static_cast<AuDynCode*>(m_vcAlgorithm[m_ePrivateAlgorithm]);
	if(!pcsDynCodePrivate)
		return FALSE;

	if(!pcsDynCodePublic->InitServer())
		return FALSE;

	if(pcsDynCodePublic != pcsDynCodePrivate)
	{
		if(!pcsDynCodePrivate->InitServer())
			return FALSE;
	}
	return TRUE;
#else
	return FALSE;
#endif
}

INT32 AuCryptManager::GetClientDynCode(AuCryptActor& csCryptActor, eAuCryptType eCryptType, const BYTE** ppCode)
{
#ifdef _USE_GPK_
	if(!IsUseDynCode())
		return 0;

	if(eCryptType == AUCRYPT_TYPE_PUBLIC)
		return GetClientDynCodePublic(csCryptActor, ppCode);
	else if(eCryptType == AUCRYPT_TYPE_PRIVATE)
		return GetClientDynCodePrivate(csCryptActor, ppCode);
	else
		return 0;
#else
	return 0;
#endif
}

INT32 AuCryptManager::GetClientDynCodePublic(AuCryptActor &csCryptActor, const BYTE **ppCode)
{
#ifdef _USE_GPK_
	if(!IsUseDynCode())
		return 0;

	AuDynCode* pcsDynCode = static_cast<AuDynCode*>(m_vcAlgorithm[m_ePublicAlgorithm]);
	if(!pcsDynCode)
		return 0;

	return pcsDynCode->GetClientDynCode(csCryptActor.GetPublicKeyRef().GetKeyPtr(), ppCode);
#else
	return 0;
#endif
}

INT32 AuCryptManager::GetClientDynCodePrivate(AuCryptActor &csCryptActor, const BYTE **ppCode)
{
#ifdef _USE_GPK_
	if(!IsUseDynCode())
		return 0;

	AuDynCode* pcsDynCode = static_cast<AuDynCode*>(m_vcAlgorithm[m_ePrivateAlgorithm]);
	if(!pcsDynCode)
		return 0;

	return pcsDynCode->GetClientDynCode(csCryptActor.GetPrivateKeyRef().GetKeyPtr(), ppCode);
#else
	return 0;
#endif
}
