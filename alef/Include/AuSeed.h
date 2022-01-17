// AuSeed.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.27.

//
// C++ Wrapper for Seed algorithm
//

// 원래 주석문도 퍼왔었으나, 재수없어서 지워버렸다. 하하하핫.
// 이놈은 뭐이리 달린 게 많냐!!!!!
// Wrapping 하기도 할일이 많구나~~~

#ifndef _AUSEED_H_
#define _AUSEED_H_

#include "AuCryptAlgorithmBase.h"

//#include <stdlib.h>
//#include <string.h>
//#include <memory.h>

// 쓸데없는 거 지우고 Little_Endian
#undef BIG_ENDIAN
#define LITTLE_ENDIAN	1		// For x86 Compatility

//	rotate by using shift operations
#if defined(_MSC_VER)
	#define ROTL_DWORD(x, n) _lrotl((x), (n))
	#define ROTR_DWORD(x, n) _lrotr((x), (n))
#else
	#define ROTL_DWORD(x, n) ( (DWORD)((x) << (n)) | (DWORD)((x) >> (32-(n))) )
	#define ROTR_DWORD(x, n) ( (DWORD)((x) >> (n)) | (DWORD)((x) << (32-(n))) )
#endif

//	reverse the byte order of DWORD(DWORD:4-bytes integer) and WORD.
#define ENDIAN_REVERSE_DWORD(dwS)	( (ROTL_DWORD((dwS),  8) & 0x00ff00ff)	\
									 | (ROTL_DWORD((dwS), 24) & 0xff00ff00) )

//	move DWORD type to BYTE type and BYTE type to DWORD type
#if defined(BIG_ENDIAN)		////	Big-Endian machine
	#define BIG_B2D(B, D)		D = *(DWORD *)(B)
	#define BIG_D2B(D, B)		*(DWORD *)(B) = (DWORD)(D)
	#define LITTLE_B2D(B, D)	D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
#elif defined(LITTLE_ENDIAN)	////	Little-Endian machine
	#define BIG_B2D(B, D)		D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define BIG_D2B(D, B)		*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
	#define LITTLE_B2D(B, D)	D = *(DWORD *)(B)
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = (DWORD)(D)
#else
	#error ERROR : Invalid DataChangeType
#endif

//	현재 아래의 4개 방식을 지원한다.
#define AI_ECB					1
#define AI_CBC					2
#define AI_OFB					3
#define AI_CFB					4
//	현재 아래의 두 padding을 지원한다.
#define AI_NO_PADDING			1	//	Padding 없음(입력이 16바이트의 배수)
#define AI_PKCS_PADDING			2	//	padding되는 바이트 수로 padding

//	SEED에 관련된 상수들
#define SEED_BLOCK_LEN			16		//	in BYTEs
#define SEED_USER_KEY_LEN		16		//	in BYTEs
#define SEED_NO_ROUNDS			16
#define SEED_NO_ROUNDKEY		(2*SEED_NO_ROUNDS)	//	in DWORDs

//	SEED..
class SEED_ALG_INFO : public BASE_CTX
{
public:
	DWORD		ModeID;						//	ECB or CBC
	DWORD		PadType;					//	블록암호의 Padding type
	BYTE		IV[SEED_BLOCK_LEN];			//	Initial Vector
	BYTE		ChainVar[SEED_BLOCK_LEN];	//	Chaining Variable
	BYTE		Buffer[SEED_BLOCK_LEN];		//	Buffer for unfilled block
	DWORD		BufLen; 					//	Buffer의 유효 바이트 수
	DWORD		RoundKey[SEED_NO_ROUNDKEY];	//	라운드 키의 DWORD 수

	void		init()
	{
		eAlgorithm = AUCRYPT_ALGORITHM_SEED;
		ModeID = 0;
		PadType = 0;
		memset(IV, 0, sizeof(IV));
		memset(ChainVar, 0, sizeof(ChainVar));
		memset(Buffer, 0, sizeof(Buffer));
		BufLen = 0;
		memset(RoundKey, 0, sizeof(RoundKey));
	}

	SEED_ALG_INFO() { init(); }
	~SEED_ALG_INFO() {;}
};

typedef SEED_ALG_INFO	SEED_CTX;	// 다른 알고리즘들과의 Naming 때문에 추가.
typedef DWORD			RET_VAL;


//	Error Code - 정리하고, 적당히 출력해야 함.
#define CTR_SUCCESS					0
#define CTR_FATAL_ERROR				0x1001
#define CTR_INVALID_USERKEYLEN		0x1002	//	비밀키의 길이가 부적절함.
#define CTR_PAD_CHECK_ERROR			0x1003	//	
#define CTR_DATA_LEN_ERROR			0x1004	//	평문의 길이가 부적절함.
#define CTR_CIPHER_LEN_ERROR		0x1005	//	암호문이 블록의 배수가 아님.



// 여기서 부터 C++ Wrapper
class AuSeed : public AuCryptAlgorithmBase
{
private:
	void	SEED_SetAlgInfo(DWORD ModeID, DWORD PadType, BYTE* IV, SEED_ALG_INFO* AlgInfo);		// AlgInfo 초기화
	RET_VAL SEED_KeySchedule(BYTE* UserKey, DWORD UserKeyLen, SEED_ALG_INFO *AlgInfo);			// Round Key 생성

	// 암호화 함수들.
	RET_VAL	SEED_EncInit(SEED_ALG_INFO* AlgInfo);
	RET_VAL	SEED_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL	SEED_EncFinal(SEED_ALG_INFO	*AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);
	void	SEED_Encrypt(void* CipherKey, BYTE* Data);

	RET_VAL ECB_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL	CBC_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL	OFB_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL	CFB_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);

	RET_VAL ECB_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL CBC_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL OFB_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL CFB_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);

	// 복호화 함수들.
	RET_VAL	SEED_DecInit(SEED_ALG_INFO* AlgInfo);
	RET_VAL	SEED_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL	SEED_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);
	void	SEED_Decrypt(void* CipherKey, BYTE* Data);

	RET_VAL ECB_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL CBC_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL OFB_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL CFB_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);

	RET_VAL ECB_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL CBC_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL OFB_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL CFB_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);

	// Padding 관련
	RET_VAL PaddSet(BYTE* pbOutBuffer, DWORD dRmdLen, DWORD dBlockLen, DWORD dPaddingType);
	RET_VAL PaddCheck(BYTE* pbOutBuffer, DWORD dBlockLen, DWORD dPaddingType);

public:
	AuSeed();
	virtual ~AuSeed();

	void	Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone = FALSE);
	DWORD	Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);
	void	Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);

	DWORD	GetOutputSize(DWORD lInputSize);
};

#endif	//_AUSEED_H_
