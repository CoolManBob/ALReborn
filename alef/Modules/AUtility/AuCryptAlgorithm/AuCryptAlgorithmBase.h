// AuCryptAlgorithmBase.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.11.

// 각 알고리즘의 Encrypt / Decrypt 를 동적 실행 시켜줌.
// Bridge 임.

#ifndef _AUCRYPTALGORITHMBASE_H_
#define _AUCRYPTALGORITHMBASE_H_

#include "ApBase.h"

typedef enum _eAuCryptType
{
	AUCRYPT_TYPE_NONE		= 0,
	AUCRYPT_TYPE_PUBLIC		= 1,
	AUCRYPT_TYPE_PRIVATE	= 2,
	AUCRYPT_TYPE_BOTH		= AUCRYPT_TYPE_PUBLIC | AUCRYPT_TYPE_PRIVATE,
} eAuCryptType;

typedef enum _eAuCryptKeyLength
{
	AUCRYPT_KEY_LENGTH_NONE			= 0,
	AUCRYPT_KEY_LENGTH_1BYTE		= 1,
	AUCRYPT_KEY_LENGTH_2BYTE		= 2,
	AUCRYPT_KEY_LENGTH_4BYTE		= 4,
	AUCRYPT_KEY_LENGTH_8BYTE		= 8,
	AUCRYPT_KEY_LENGTH_16BYTE		= 16,
	AUCRYPT_KEY_LENGTH_32BYTE		= 32,
	AUCRYPT_KEY_LENGTH_64BYTE		= 64,
} eAuCryptKeyLength;

typedef enum _eAuCryptAlgorithm
{
	AUCRYPT_ALGORITHM_NONE		= -1,
	AUCRYPT_ALGORITHM_BLOWFISH,
	AUCRYPT_ALGORITHM_MD5MAC,
	AUCRYPT_ALGORITHM_RIJNDAEL,
	AUCRYPT_ALGORITHM_SEED,
	AUCRYPT_ALGORITHM_DYNCODE,		// DynCode 는 가장 마지막 알고리즘이 되어야 한다.
	AUCRYPT_ALGORITHM_MAX,
} eAuCryptAlgorithm;

//const CHAR* AUCRYPT_ALGORITHM_NAME_BLOWFISH		= "BlowFish";
//const CHAR* AUCRYPT_ALGORITHM_NAME_MD5MAC		= "MD5Mac";
//const CHAR* AUCRYPT_ALGORITHM_NAME_RIJNDAEL		= "RijnDael";
//const CHAR* AUCRYPT_ALGORITHM_NAME_SEED			= "Seed";
//const CHAR* AUCRYPT_ALGORITHM_NAME_DYNCODE		= "DynCode";

#define AUCRYPT_ALGORITHM_NAME_BLOWFISH		"BlowFish"
#define AUCRYPT_ALGORITHM_NAME_MD5MAC		"MD5Mac"
#define AUCRYPT_ALGORITHM_NAME_RIJNDAEL		"RijnDael"
#define AUCRYPT_ALGORITHM_NAME_SEED			"Seed"
#define AUCRYPT_ALGORITHM_NAME_DYNCODE		"DynCode"

class BASE_CTX
{
public:
	eAuCryptAlgorithm eAlgorithm;
	virtual void init() = 0;
	BASE_CTX() { eAlgorithm = AUCRYPT_ALGORITHM_NONE; }
	virtual ~BASE_CTX() {;}
};

class AuCryptAlgorithmBase
{
public:
	AuCryptAlgorithmBase() {;}
	virtual ~AuCryptAlgorithmBase() {;}

	virtual void	Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone = FALSE) = 0;
	virtual	void	Initialize(void* pctx, void* pctxSource) {;}
	virtual DWORD	Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize) = 0;
	virtual void	Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize) = 0;

	virtual DWORD	GetOutputSize(DWORD lInputSize) = 0;
};

#endif //_AUCRYPTALGORITHMBASE_H_