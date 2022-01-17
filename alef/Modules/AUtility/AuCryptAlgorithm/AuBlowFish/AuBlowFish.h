// AuBlowFish.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.07.

//
// Wrapper for blowfish algorithm
//

#ifndef _AUBLOWFISH_H_
#define _AUBLOWIFSH_H_

#include "AuCryptAlgorithmBase.h"

#define MAXKEYBYTES 	56		// 448 bits max
#define NPASS           16		// SBox passes

#define PSIZE			18
#define SBoxCount		4
#define SBoxSize		256

class BLOWFISH_CTX : public BASE_CTX
{
public:
	DWORD P[PSIZE];
	DWORD SBox[SBoxCount][SBoxSize];

	void init()
	{
		eAlgorithm = AUCRYPT_ALGORITHM_BLOWFISH;
		memset(P, 0, sizeof(P));
		memset(SBox, 0, sizeof(SBox));
	}

	BLOWFISH_CTX() { init(); }
	~BLOWFISH_CTX() {;}
};

class AuBlowFish : public AuCryptAlgorithmBase
{
private:
	void	Blowfish_encipher(BLOWFISH_CTX& ctx, DWORD* xl, DWORD* xr);
	void	Blowfish_decipher(BLOWFISH_CTX& ctx, DWORD* xl, DWORD* xr);

public:
	AuBlowFish();
	virtual ~AuBlowFish();

	void	Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone = FALSE);
	DWORD	Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);
	void	Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);

	DWORD	GetOutputSize(DWORD lInputSize);
};

union aword
{
	DWORD dword;
	BYTE byte[4];
	struct
	{
		unsigned int byte3:8;
		unsigned int byte2:8;
		unsigned int byte1:8;
		unsigned int byte0:8;
	} w;
};

#endif//_AUBLOWIFSH_H_