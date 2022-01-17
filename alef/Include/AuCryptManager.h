// AuCryptManager.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.07.

//
// 암호화 작업을 한다.
// 여러 암호화 알고리즘을 선택적으로 사용할 수 있게 한다.
// 

#ifndef _AUCRYPT_MANAGER_H_
#define _AUCRYPT_MANAGER_H_

#include "AuCrypt.h"
#include "AuCryptAlgorithmBase.h"

#include <vector>

typedef enum _eAuCryptCheckLevel
{
	AUCRYPT_CHECK_LEVEL_NONE = 0,
	AUCRYPT_CHECK_LEVEL_LOW,
	AUCRYPT_CHECK_LEVEL_MIDDLE,
	AUCRYPT_CHECK_LEVEL_HIGH,
	AUCRYPT_CHECK_LEVEL_ULTRA,
	AUCRYPT_CHECK_LEVEL_SUPER,
	AUCRYPT_CHECK_LEVEL_EXTREME,
	AUCRYPT_CHECK_LEVEL_MAX,
} eAuCryptCheckLevel;

typedef enum _eAuCryptCheckLimit
{
	AUCRYPT_CHECK_LIMIT_NONE		= 0,
	AUCRYPT_CHECK_LIMIT_LOW			= 50,
	AUCRYPT_CHECK_LIMIT_MIDDLE		= 25,
	AUCRYPT_CHECK_LIMIT_HIGH		= 10,
	AUCRYPT_CHECK_LIMIT_ULTRA		= 5,
	AUCRYPT_CHECK_LIMIT_SUPER		= 3,
	AUCRYPT_CHECK_LIMIT_EXTREME		= 1,
	AUCRYPT_CHECK_LIMIT_MAX,
} eAuCryptCheckLimit;

#define AUCRYPT_MANAGER_INVALID_PACKET_LOG_FILENAME		"CryptInvalid.log"

class AuCryptManager
{
public:
	typedef std::vector<AuCryptAlgorithmBase*>				AlgorithmVector;
	typedef std::vector<AuCryptAlgorithmBase*>::iterator	AlgorithmIterator;

private:
	BOOL m_bUseCrypt;
	eAuCryptCheckLevel m_eCryptCheckLevel;

	AlgorithmVector m_vcAlgorithm;

	eAuCryptAlgorithm m_ePublicAlgorithm;
	eAuCryptAlgorithm m_ePrivateAlgorithm;

	INT32 m_lPublicKeySize;
	INT32 m_lPrivateKeySize;

public:
	AuCryptManager();
	virtual ~AuCryptManager();

	void	SetUseCrypt(BOOL bUseCrypt = TRUE) { m_bUseCrypt = bUseCrypt; }
	BOOL	IsUseCrypt() { return m_bUseCrypt; }

	void	SetCryptCheckLevel(eAuCryptCheckLevel eCheckLevel) { m_eCryptCheckLevel = eCheckLevel; }
	eAuCryptCheckLevel GetCryptCheckLevel() { return m_eCryptCheckLevel; }

	void	SetAlgorithm(eAuCryptAlgorithm ePublic, eAuCryptAlgorithm ePrivate);
	void	SetPublicAlgorithm(eAuCryptAlgorithm eAlgorithm) { m_ePublicAlgorithm = eAlgorithm; }
	void	SetPrivateAlgorithm(eAuCryptAlgorithm eAlgorithm) { m_ePrivateAlgorithm = eAlgorithm; }
	eAuCryptAlgorithm GetPublicAlgorithm() { return m_ePublicAlgorithm; }
	eAuCryptAlgorithm GetPrivateAlgorithm() { return m_ePrivateAlgorithm; }
	LPCTSTR	GetPublicAlgorithmString();
	LPCTSTR GetPrivateAlgorithmString();
	LPCTSTR	GetAlgorithmString(eAuCryptAlgorithm eAlgorithm);
	
	void	SetPublicKeySize(INT32 lSize) { m_lPublicKeySize = lSize; }
	void	SetPrivateKeySize(INT32 lSize) { m_lPrivateKeySize = lSize; }
	INT32	GetPublicKeySize() { return m_lPublicKeySize; }
	INT32	GetPrivateKeySize() { return m_lPrivateKeySize; }

	BOOL	IsPublicClone();
	BOOL	IsPrivateClone();
	BOOL	IsPublicCopy();
	BOOL	IsPrivateCopy();

	BOOL	UseDecryptBuffer(eAuCryptType eCryptType);

	void	MakeRandomKey(BYTE* pKey, INT32 lSize);

	BOOL	InitializeActor(AuCryptActor& csCryptActor, BYTE* pKey, INT32 lKeySize, BOOL bClone = FALSE);
	BOOL	InitializeActorPublic(AuCryptActor& csCryptActor, BYTE* pKey, INT32 lKeySize, BOOL bClone = FALSE);
	BOOL	InitializeActorPrivate(AuCryptActor& csCryptActor, BYTE* pKey, INT32 lKeySize, BOOL bClone = FALSE);
	BOOL	InitializeActor(AuCryptActor& csCryptActor, AuCryptActor& csSourceActor);
	BOOL	InitializeActorPublic(AuCryptActor& csCryptActor, AuCryptActor& csSourceActor);
	BOOL	InitializeActorPrivate(AuCryptActor& csCryptActor, AuCryptActor& csSourceActor);

	BOOL	InitializeCTX(AuCryptCell& csKeyInfo, eAuCryptType eCryptType = AUCRYPT_TYPE_PUBLIC);

	UINT32	Encrypt(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize);
	void	Decrypt(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize);
	UINT32	Encrypt(AuCryptActor& csCryptActor, eAuCryptType eCryptType, BYTE* pInput, BYTE* pOutput, INT32 lInputSize);
	void	Decrypt(AuCryptActor& csCryptActor, eAuCryptType eCryptType, BYTE* pInput, BYTE* pOutput, INT32 lInputSize);

	UINT32	EncryptByPublic(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize);
	void	DecryptByPublic(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize);
	UINT32	EncryptByPrivate(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize);
	void	DecryptByPrivate(AuCryptActor& csCryptActor, BYTE* pInput, BYTE* pOutput, INT32 lInputSize);

	INT32	GetOutputSize(eAuCryptType eCryptType, INT32 lInputSize);

	// Dynamic Code 관련
	BOOL	IsUseDynCode();
	BOOL	InitServer();
	INT32	GetClientDynCode(AuCryptActor& csCryptActor, eAuCryptType eCryptType, const BYTE** ppCode);
	INT32	GetClientDynCodePublic(AuCryptActor& csCryptActor, const BYTE** ppCode);
	INT32	GetClientDynCodePrivate(AuCryptActor& csCryptActor, const BYTE** ppCode);
};

#endif //_AUCRYPT_MANAGER_H_