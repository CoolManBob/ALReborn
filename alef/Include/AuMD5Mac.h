// AuMD5Mac.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.17.

//
// Based AuMD5Encrypt
//

#ifndef _AUMD5MAC_H_
#define _AUMD5MAC_H_

#include "ApBase.h"
#include "AuCryptAlgorithmBase.h"

#include <wincrypt.h>

class MD5MAC_CTX : public BASE_CTX
{
public:
	BYTE m_HashByte[AUCRYPT_KEY_LENGTH_32BYTE];

	void init()
	{
		eAlgorithm = AUCRYPT_ALGORITHM_MD5MAC;
		memset(m_HashByte, 0, sizeof(m_HashByte));
	}

	MD5MAC_CTX() { init(); }
	~MD5MAC_CTX() {;}
};

class AuMD5Mac : public AuCryptAlgorithmBase
{
private:
	HCRYPTPROV m_hCryptProv; 

	void	Startup();
	void	Cleanup();

public:
	AuMD5Mac();
	virtual ~AuMD5Mac();

	void	Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone = FALSE);
	DWORD	Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);
	void	Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);

	DWORD	GetOutputSize(DWORD lInputSize);
};

#endif//_AUMD5MAC_H_