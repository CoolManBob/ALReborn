// AuSeed.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.27.

#include "AuSeed.h"
#include "AuSeed.h2"

#define SEED_BLOCK_LENGTH		32	// 한번에 암호화-복호화 하는 양.


#define BlockCopy(pbDst, pbSrc) {					\
	((DWORD *)(pbDst))[0] = ((DWORD *)(pbSrc))[0];	\
	((DWORD *)(pbDst))[1] = ((DWORD *)(pbSrc))[1];	\
	((DWORD *)(pbDst))[2] = ((DWORD *)(pbSrc))[2];	\
	((DWORD *)(pbDst))[3] = ((DWORD *)(pbSrc))[3];	\
}

#define BlockXor(pbDst, phSrc1, phSrc2) {			\
	((DWORD *)(pbDst))[0] = ((DWORD *)(phSrc1))[0]	\
						  ^ ((DWORD *)(phSrc2))[0];	\
	((DWORD *)(pbDst))[1] = ((DWORD *)(phSrc1))[1]	\
						  ^ ((DWORD *)(phSrc2))[1];	\
	((DWORD *)(pbDst))[2] = ((DWORD *)(phSrc1))[2]	\
						  ^ ((DWORD *)(phSrc2))[2];	\
	((DWORD *)(pbDst))[3] = ((DWORD *)(phSrc1))[3]	\
						  ^ ((DWORD *)(phSrc2))[3];	\
}

//	Constants for key schedule: KC0 = golden ratio; KCi = ROTL(KCi-1, 1)
#define KC0 	0x9e3779b9
#define KC1 	0x3c6ef373
#define KC2 	0x78dde6e6
#define KC3 	0xf1bbcdcc
#define KC4 	0xe3779b99
#define KC5 	0xc6ef3733
#define KC6 	0x8dde6e67
#define KC7 	0x1bbcdccf
#define KC8 	0x3779b99e
#define KC9 	0x6ef3733c
#define KC10	0xdde6e678
#define KC11	0xbbcdccf1
#define KC12	0x779b99e3
#define KC13	0xef3733c6
#define KC14	0xde6e678d
#define KC15	0xbcdccf1b

#define EncRoundKeyUpdate0(K, A, B, C, D, KC) {		\
	T0 = A; 										\
	A = (A>>8) ^ (B<<24);							\
	B = (B>>8) ^ (T0<<24);							\
	T0 = A + C - KC;								\
	T1 = B + KC - D;								\
	(K)[0] = SEED_SL[0][(T0    )&0xFF] ^ SEED_SL[1][(T0>> 8)&0xFF]	\
		   ^ SEED_SL[2][(T0>>16)&0xFF] ^ SEED_SL[3][(T0>>24)&0xFF];	\
	(K)[1] = SEED_SL[0][(T1    )&0xFF] ^ SEED_SL[1][(T1>> 8)&0xFF]	\
		   ^ SEED_SL[2][(T1>>16)&0xFF] ^ SEED_SL[3][(T1>>24)&0xFF];	\
}

#define EncRoundKeyUpdate1(K, A, B, C, D, KC) {		\
	T0 = C; 										\
	C = (C<<8) ^ (D>>24);							\
	D = (D<<8) ^ (T0>>24);							\
	T0 = A + C - KC;								\
	T1 = B + KC - D;								\
	(K)[0] = SEED_SL[0][(T0    )&0xFF] ^ SEED_SL[1][(T0>> 8)&0xFF]	\
		   ^ SEED_SL[2][(T0>>16)&0xFF] ^ SEED_SL[3][(T0>>24)&0xFF];	\
	(K)[1] = SEED_SL[0][(T1    )&0xFF] ^ SEED_SL[1][(T1>> 8)&0xFF]	\
		   ^ SEED_SL[2][(T1>>16)&0xFF] ^ SEED_SL[3][(T1>>24)&0xFF];	\
}

#define SeedRound(A,B,C,D, K) {									\
	DWORD T0, T1;												\
	T0 = C ^ (K)[0];											\
	T1 = D ^ (K)[1];											\
	T1 ^= T0;													\
	T1 = SEED_SL[0][(T1    )&0xFF] ^ SEED_SL[1][(T1>> 8)&0xFF]	\
	   ^ SEED_SL[2][(T1>>16)&0xFF] ^ SEED_SL[3][(T1>>24)&0xFF];	\
	T0 += T1;													\
	T0 = SEED_SL[0][(T0    )&0xFF] ^ SEED_SL[1][(T0>> 8)&0xFF]	\
	   ^ SEED_SL[2][(T0>>16)&0xFF] ^ SEED_SL[3][(T0>>24)&0xFF];	\
	T1 += T0;													\
	T1 = SEED_SL[0][(T1    )&0xFF] ^ SEED_SL[1][(T1>> 8)&0xFF]	\
	   ^ SEED_SL[2][(T1>>16)&0xFF] ^ SEED_SL[3][(T1>>24)&0xFF];	\
	T0 += T1;													\
	A ^= T0; B ^= T1;											\
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// 여기서 부터 AuSeed Member Function 구현.
//
AuSeed::AuSeed()
{
}

AuSeed::~AuSeed()
{
}

void AuSeed::Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone)
{
	if(!pctx || !key || lKeySize < 1)
		return;

	//// 키 사이즈는 32바이트!!!
	//if(lKeySize != AUCRYPT_KEY_LENGTH_32BYTE)
	//	return;

	SEED_CTX& ctx = *static_cast<SEED_CTX*>(pctx);

	// 현재 User Key Size 는 32 Bytes. Encrypt 하면 48 Bytes 가 된다.
	// 그런데 AuSeed 에서 사용하는 UserKey 는 16 Bytes.
	// 그래서 앞의 16 Bytes 를 Round Key 용으로, 뒤의 16 Bytes 를 IV 용으로 세팅한다.
	// Encrypted 된 나머지 16Byte 는(?) 버린다. -_-;;

	BYTE UserKey[SEED_BLOCK_LEN];
	memcpy(UserKey, key, SEED_BLOCK_LEN);

	BYTE IV[SEED_BLOCK_LEN];
	memcpy(IV, key + SEED_BLOCK_LEN, SEED_BLOCK_LEN);

	// ctx 초기화
	// DES 방식은 CBC 이고, Padding 을 지원한다. 방식은 성능을 고려해서 적절한 것으로 바꾸자.
	SEED_SetAlgInfo(AI_CBC, AI_PKCS_PADDING, IV, &ctx);
	SEED_KeySchedule(UserKey, SEED_BLOCK_LEN, &ctx);
}

DWORD AuSeed::Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return 0;

	SEED_CTX& ctx = *static_cast<SEED_CTX*>(pctx);
	RET_VAL ret;

	ret = SEED_EncInit(&ctx);
	if(ret != CTR_SUCCESS)
	{
		ASSERT(!"AuSeed::Encypt(...) EncInit Failed");
		return 0;
	}

	BYTE Buffer[SEED_BLOCK_LENGTH + 32];
	DWORD dwOutputSize = 0, dwEncrypted = 0;
	INT32 lIndex = 0, lBlockSize = 0, lCount = 0;

	while(true)
	{
		lBlockSize = ((lCount + 1) * SEED_BLOCK_LENGTH) < (INT32)lSize ? SEED_BLOCK_LENGTH : (INT32)lSize - (lCount * SEED_BLOCK_LENGTH);
		if(lBlockSize <= 0)
			break;

		memset(Buffer, 0, sizeof(Buffer));
		dwEncrypted = 0;

		lIndex = lCount * SEED_BLOCK_LENGTH;
		ret = SEED_EncUpdate(&ctx, pInput + lIndex, lBlockSize, Buffer, &dwEncrypted);
		if(ret != CTR_SUCCESS)
		{
			ASSERT(!"AuSeed:Encyprt(...) EncUpdate Failed");
			return 0;
		}

		if(dwEncrypted > 0)
		{
			memcpy(pOutput + dwOutputSize, Buffer, dwEncrypted);
			dwOutputSize += dwEncrypted;
		}

		++lCount;
	}

	memset(Buffer, 0, sizeof(Buffer));
	dwEncrypted = 0;

	ret = SEED_EncFinal(&ctx, Buffer, &dwEncrypted);
	if(ret != CTR_SUCCESS)
	{
		ASSERT(!"AuSeed:Encyprt(...) EncFinal Failed");
		return 0;
	}

	if(dwEncrypted > 0)
	{
		memcpy(pOutput + dwOutputSize, Buffer, dwEncrypted);
		dwOutputSize += dwEncrypted;
	}

	return dwOutputSize;
}

// Encrypt 랑 함수만 다르지 구조는 같다.
void AuSeed::Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return;

	SEED_CTX& ctx = *static_cast<SEED_CTX*>(pctx);
	RET_VAL ret;

	ret = SEED_DecInit(&ctx);
	if(ret != CTR_SUCCESS)
	{
		ASSERT(!"AuSeed::Decrypt(...) DecInit Failed");
		return;
	}

	BYTE Buffer[SEED_BLOCK_LENGTH + 32];
	DWORD dwOutputSize = 0, dwDecrypted = 0;
	INT32 lIndex = 0, lBlockSize = 0, lCount = 0;

	while(true)
	{
		lBlockSize = ((lCount + 1) * SEED_BLOCK_LENGTH) < (INT32)lSize ? SEED_BLOCK_LENGTH : (INT32)lSize - (lCount * SEED_BLOCK_LENGTH);
		if(lBlockSize <= 0)
			break;

		memset(Buffer, 0, sizeof(Buffer));
		dwDecrypted = 0;

		lIndex = lCount * SEED_BLOCK_LENGTH;
		ret = SEED_DecUpdate(&ctx, pInput + lIndex, lBlockSize, Buffer, &dwDecrypted);
		if(ret != CTR_SUCCESS)
		{
			ASSERT(!"AuSeed:Decrypt(...) DecUpdate Failed");
			return;
		}

		if(dwDecrypted > 0)
		{
			memcpy(pOutput + dwOutputSize, Buffer, dwDecrypted);
			dwOutputSize += dwDecrypted;
		}

		++lCount;
	}

	memset(Buffer, 0, sizeof(Buffer));
	dwDecrypted = 0;

	ret = SEED_DecFinal(&ctx, Buffer, &dwDecrypted);
	if(ret != CTR_SUCCESS)
	{
		ASSERT(!"AuSeed:Decrypt(...) DecFinal Failed");
		return;
	}

	if(dwDecrypted > 0)
	{
		memcpy(pOutput + dwOutputSize, Buffer, dwDecrypted);
		dwOutputSize += dwDecrypted;
	}
}

// 아웃풋 사이즈의 길이는 SEED_BLOCK_LEN 의 배수이다.
DWORD AuSeed::GetOutputSize(DWORD lInputSize)
{
	if(lInputSize % SEED_BLOCK_LEN == 0)
		return lInputSize + SEED_BLOCK_LEN;		// SEED_BLOCK_LEN 만큼 더해준다.
	else
		return ((DWORD)(lInputSize / SEED_BLOCK_LEN) + 1) * SEED_BLOCK_LEN;
}

void AuSeed::SEED_SetAlgInfo(DWORD ModeID, DWORD PadType, BYTE* IV, SEED_ALG_INFO* AlgInfo)
{
	AlgInfo->ModeID = ModeID;
	AlgInfo->PadType = PadType;

	if( IV!=NULL )
		memcpy(AlgInfo->IV, IV, SEED_BLOCK_LEN);
	else
		memset(AlgInfo->IV, 0, SEED_BLOCK_LEN);
}

RET_VAL AuSeed::SEED_KeySchedule(BYTE* UserKey, DWORD UserKeyLen, SEED_ALG_INFO* AlgInfo)
{
	DWORD	A, B, C, D, T0, T1, *K=AlgInfo->RoundKey;

	if( UserKeyLen!=SEED_USER_KEY_LEN )
		return CTR_INVALID_USERKEYLEN;

	BIG_B2D( &(UserKey[0]), A);
	BIG_B2D( &(UserKey[4]), B);
	BIG_B2D( &(UserKey[8]), C);
	BIG_B2D( &(UserKey[12]), D);

	T0 = A + C - KC0;
	T1 = B - D + KC0;
	K[0] = SEED_SL[0][(T0    )&0xFF] ^ SEED_SL[1][(T0>> 8)&0xFF]
		 ^ SEED_SL[2][(T0>>16)&0xFF] ^ SEED_SL[3][(T0>>24)&0xFF];
	K[1] = SEED_SL[0][(T1    )&0xFF] ^ SEED_SL[1][(T1>> 8)&0xFF]
		 ^ SEED_SL[2][(T1>>16)&0xFF] ^ SEED_SL[3][(T1>>24)&0xFF];;

	EncRoundKeyUpdate0(K+ 2, A, B, C, D, KC1 );
	EncRoundKeyUpdate1(K+ 4, A, B, C, D, KC2 );
	EncRoundKeyUpdate0(K+ 6, A, B, C, D, KC3 );
	EncRoundKeyUpdate1(K+ 8, A, B, C, D, KC4 );
	EncRoundKeyUpdate0(K+10, A, B, C, D, KC5 );
	EncRoundKeyUpdate1(K+12, A, B, C, D, KC6 );
	EncRoundKeyUpdate0(K+14, A, B, C, D, KC7 );
	EncRoundKeyUpdate1(K+16, A, B, C, D, KC8 );
	EncRoundKeyUpdate0(K+18, A, B, C, D, KC9 );
	EncRoundKeyUpdate1(K+20, A, B, C, D, KC10);
	EncRoundKeyUpdate0(K+22, A, B, C, D, KC11);
	EncRoundKeyUpdate1(K+24, A, B, C, D, KC12);
	EncRoundKeyUpdate0(K+26, A, B, C, D, KC13);
	EncRoundKeyUpdate1(K+28, A, B, C, D, KC14);
	EncRoundKeyUpdate0(K+30, A, B, C, D, KC15);

	//	Remove sensitive data
	A = B = C = D = T0 = T1 = 0;
	K = NULL;

	//
	return CTR_SUCCESS;
}

RET_VAL	AuSeed::SEED_EncInit(SEED_ALG_INFO* AlgInfo)
{
	AlgInfo->BufLen = 0;
	if( AlgInfo->ModeID!=AI_ECB )
		memcpy(AlgInfo->ChainVar, AlgInfo->IV, SEED_BLOCK_LEN);
	return CTR_SUCCESS;
}

RET_VAL	AuSeed::SEED_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	switch( AlgInfo->ModeID ) {
		case AI_ECB :	return ECB_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen,
											 CipherTxt, CipherTxtLen);
		case AI_CBC :	return CBC_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen,
											 CipherTxt, CipherTxtLen);
		case AI_OFB :	return OFB_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen,
											 CipherTxt, CipherTxtLen);
		case AI_CFB :	return CFB_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen,
											 CipherTxt, CipherTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

RET_VAL	AuSeed::SEED_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	switch( AlgInfo->ModeID ) {
		case AI_ECB :	return ECB_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		case AI_CBC :	return CBC_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		case AI_OFB :	return OFB_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		case AI_CFB :	return CFB_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

void AuSeed::SEED_Encrypt(void* CipherKey, BYTE* Data)
{
	DWORD	A, B, C, D;
	DWORD*	K = (DWORD*)CipherKey;

	BIG_B2D( &(Data[8]), C);
	BIG_B2D( &(Data[12]), D);
	BIG_B2D( &(Data[0]), A);
	BIG_B2D( &(Data[4]), B);

	//
	SeedRound(A, B, C, D, K   );
	SeedRound(C, D, A, B, K+ 2);
	SeedRound(A, B, C, D, K+ 4);
	SeedRound(C, D, A, B, K+ 6);
	SeedRound(A, B, C, D, K+ 8);
	SeedRound(C, D, A, B, K+10);
	SeedRound(A, B, C, D, K+12);
	SeedRound(C, D, A, B, K+14);
	SeedRound(A, B, C, D, K+16);
	SeedRound(C, D, A, B, K+18);
	SeedRound(A, B, C, D, K+20);
	SeedRound(C, D, A, B, K+22);
	SeedRound(A, B, C, D, K+24);
	SeedRound(C, D, A, B, K+26);
	SeedRound(A, B, C, D, K+28);
	SeedRound(C, D, A, B, K+30);

	//
	BIG_D2B(A, &(Data[8]));
	BIG_D2B(B, &(Data[12]));
	BIG_D2B(C, &(Data[0]));
	BIG_D2B(D, &(Data[4]));

	//	Remove sensitive data
	A = B = C = D = 0;
	K = NULL;
}

RET_VAL AuSeed::ECB_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN, BufLen=AlgInfo->BufLen;

	//
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	BlockCopy(CipherTxt, AlgInfo->Buffer);
	SEED_Encrypt(ScheduledKey, CipherTxt);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) {
		BlockCopy(CipherTxt, PlainTxt);
		SEED_Encrypt(ScheduledKey, CipherTxt);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//	control the case that PlainTxt and CipherTxt are the same buffer
	return CTR_SUCCESS;
}

RET_VAL AuSeed::CBC_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN, BufLen=AlgInfo->BufLen;

	//
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	BlockXor(CipherTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	SEED_Encrypt(ScheduledKey, CipherTxt);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) {
		BlockXor(CipherTxt, CipherTxt-BlockLen, PlainTxt);
		SEED_Encrypt(ScheduledKey, CipherTxt);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}
	BlockCopy(AlgInfo->ChainVar, CipherTxt-BlockLen);

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::OFB_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(CipherTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) {
		SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(CipherTxt, AlgInfo->ChainVar, PlainTxt);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::CFB_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, AlgInfo->Buffer);
	BlockCopy(CipherTxt, AlgInfo->ChainVar);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) {
		SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, PlainTxt);
		BlockCopy(CipherTxt, AlgInfo->ChainVar);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::ECB_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	DWORD		PaddByte;

	//	Padding
	PaddByte = PaddSet(AlgInfo->Buffer, BufLen, BlockLen, AlgInfo->PadType);
	if( PaddByte>BlockLen )		return PaddByte;

	if( PaddByte==0 ) {
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	core part
	BlockCopy(CipherTxt, AlgInfo->Buffer);
	SEED_Encrypt(ScheduledKey, CipherTxt);

	//
	*CipherTxtLen = BlockLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::CBC_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	DWORD		PaddByte;

	//	Padding
	PaddByte = PaddSet(AlgInfo->Buffer, BufLen, BlockLen, AlgInfo->PadType);
	if( PaddByte>BlockLen )		return PaddByte;

	if( PaddByte==0 ) {
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	core part
	BlockXor(CipherTxt, AlgInfo->Buffer, AlgInfo->ChainVar);
	SEED_Encrypt(ScheduledKey, CipherTxt);
	BlockCopy(AlgInfo->ChainVar, CipherTxt);

	//
	*CipherTxtLen = BlockLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::OFB_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;
	DWORD		i;

	//	Check Output Memory Size
	*CipherTxtLen = BlockLen;

	//	core part
	SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	for( i=0; i<BufLen; i++)
		CipherTxt[i] = (BYTE) (AlgInfo->Buffer[i] ^ AlgInfo->ChainVar[i]);

	//
	*CipherTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::CFB_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*CipherTxtLen = BufLen;

	//	core part
	SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, AlgInfo->Buffer);
	memcpy(CipherTxt, AlgInfo->ChainVar, BufLen);

	//
	*CipherTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}

RET_VAL	AuSeed::SEED_DecInit(SEED_ALG_INFO* AlgInfo)
{
	AlgInfo->BufLen = 0;
	if( AlgInfo->ModeID!=AI_ECB )
		memcpy(AlgInfo->ChainVar, AlgInfo->IV, SEED_BLOCK_LEN);
	return CTR_SUCCESS;
}

RET_VAL	AuSeed::SEED_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	switch( AlgInfo->ModeID ) {
		case AI_ECB :	return ECB_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen,
											 PlainTxt, PlainTxtLen);
		case AI_CBC :	return CBC_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen,
											 PlainTxt, PlainTxtLen);
		case AI_OFB :	return OFB_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen,
											 PlainTxt, PlainTxtLen);
		case AI_CFB :	return CFB_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen,
											 PlainTxt, PlainTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

RET_VAL	AuSeed::SEED_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	switch( AlgInfo->ModeID ) {
		case AI_ECB :	return ECB_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		case AI_CBC :	return CBC_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		case AI_OFB :	return OFB_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		case AI_CFB :	return CFB_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

void AuSeed::SEED_Decrypt(void* CipherKey, BYTE* Data)
{
	DWORD	A, B, C, D;
	DWORD*	K = (DWORD*)CipherKey;

	//
	BIG_B2D( &(Data[8]), C);
	BIG_B2D( &(Data[12]), D);
	BIG_B2D( &(Data[0]), A);
	BIG_B2D( &(Data[4]), B);

	//
	SeedRound(A, B, C, D, K+30);
	SeedRound(C, D, A, B, K+28);
	SeedRound(A, B, C, D, K+26);
	SeedRound(C, D, A, B, K+24);
	SeedRound(A, B, C, D, K+22);
	SeedRound(C, D, A, B, K+20);
	SeedRound(A, B, C, D, K+18);
	SeedRound(C, D, A, B, K+16);
	SeedRound(A, B, C, D, K+14);
	SeedRound(C, D, A, B, K+12);
	SeedRound(A, B, C, D, K+10);
	SeedRound(C, D, A, B, K+ 8);
	SeedRound(A, B, C, D, K+ 6);
	SeedRound(C, D, A, B, K+ 4);
	SeedRound(A, B, C, D, K+ 2);
	SeedRound(C, D, A, B, K+ 0);

	//
	BIG_D2B(A, &(Data[8]));
	BIG_D2B(B, &(Data[12]));
	BIG_D2B(C, &(Data[0]));
	BIG_D2B(D, &(Data[4]));

	//	Remove sensitive data
	A = B = C = D = 0;
	K = NULL;
}

RET_VAL AuSeed::ECB_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( CipherTxt==PlainTxt )	return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	SEED_Decrypt(ScheduledKey, PlainTxt);
	PlainTxt += BlockLen;
	while( CipherTxtLen>BlockLen ) {
		BlockCopy(PlainTxt, CipherTxt);
		SEED_Decrypt(ScheduledKey, PlainTxt);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::CBC_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN, BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( CipherTxt==PlainTxt )	return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	SEED_Decrypt(ScheduledKey, PlainTxt);
	BlockXor(PlainTxt, PlainTxt, AlgInfo->ChainVar);
	PlainTxt += BlockLen;
	if( CipherTxtLen>BlockLen ) {
		BlockCopy(PlainTxt, CipherTxt);
		SEED_Decrypt(ScheduledKey, PlainTxt);
		BlockXor(PlainTxt, PlainTxt, AlgInfo->Buffer);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}
	while( CipherTxtLen>BlockLen ) {
		BlockCopy(PlainTxt, CipherTxt);
		SEED_Decrypt(ScheduledKey, PlainTxt);
		BlockXor(PlainTxt, PlainTxt, CipherTxt-BlockLen);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}
	BlockCopy(AlgInfo->ChainVar, CipherTxt-BlockLen);

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::OFB_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(PlainTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	PlainTxt += BlockLen;
	while( CipherTxtLen>BlockLen ) {
		SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(PlainTxt, AlgInfo->ChainVar, CipherTxt);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::CFB_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(PlainTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	BlockCopy(AlgInfo->ChainVar, AlgInfo->Buffer);
	PlainTxt += BlockLen;
	while( CipherTxtLen>BlockLen ) {
		SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(PlainTxt, AlgInfo->ChainVar, CipherTxt);
		BlockCopy(AlgInfo->ChainVar, CipherTxt);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::ECB_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	RET_VAL		ret;

	//	Check Output Memory Size
	if( BufLen==0 ) {
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}
	*PlainTxtLen = BlockLen;

	if( BufLen!=BlockLen )	return CTR_CIPHER_LEN_ERROR;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	SEED_Decrypt(ScheduledKey, PlainTxt);

	//	Padding Check
	ret = PaddCheck(PlainTxt, BlockLen, AlgInfo->PadType);
	if( ret==(DWORD)-3 )	return CTR_PAD_CHECK_ERROR;
	if( ret==(DWORD)-1 )	return CTR_FATAL_ERROR;

	*PlainTxtLen = BlockLen - ret;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::CBC_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=SEED_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	RET_VAL		ret;

	//	Check Output Memory Size
	if( BufLen==0 ) {
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}
	*PlainTxtLen = BlockLen;

	if( BufLen!=BlockLen )	return CTR_CIPHER_LEN_ERROR;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	SEED_Decrypt(ScheduledKey, PlainTxt);
	BlockXor(PlainTxt, PlainTxt, AlgInfo->ChainVar);
	BlockCopy(AlgInfo->ChainVar, AlgInfo->Buffer);

	//	Padding Check
	ret = PaddCheck(PlainTxt, BlockLen, AlgInfo->PadType);
	if( ret==(DWORD)-3 )	return CTR_PAD_CHECK_ERROR;
	if( ret==(DWORD)-1 )	return CTR_FATAL_ERROR;

	*PlainTxtLen = BlockLen - ret;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::OFB_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		i, BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen;

	//	core part
	SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	for( i=0; i<BufLen; i++)
		PlainTxt[i] = (BYTE) (AlgInfo->Buffer[i] ^ AlgInfo->ChainVar[i]);

	*PlainTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}

RET_VAL AuSeed::CFB_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen)
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen;

	//	core part
	SEED_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, AlgInfo->Buffer);
	memcpy(PlainTxt, AlgInfo->ChainVar, BufLen);

	*PlainTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}





RET_VAL AuSeed::PaddSet(BYTE* pbOutBuffer, DWORD dRmdLen, DWORD dBlockLen, DWORD dPaddingType)
{
	DWORD dPadLen;

	switch( dPaddingType ) {
		case AI_NO_PADDING :
			if( dRmdLen==0 )	return 0;
			else				return CTR_DATA_LEN_ERROR;

		case AI_PKCS_PADDING :
			dPadLen = dBlockLen - dRmdLen;
			memset(pbOutBuffer+dRmdLen, (char)dPadLen, (int)dPadLen);
			return dPadLen;

		default :
			return CTR_FATAL_ERROR;
	}
}

RET_VAL AuSeed::PaddCheck(BYTE* pbOutBuffer, DWORD dBlockLen, DWORD dPaddingType)
{
	DWORD i, dPadLen;

	switch( dPaddingType ) {
		case AI_NO_PADDING :
			return 0;			//	padding된 데이타가 0바이트임.

		case AI_PKCS_PADDING :
			dPadLen = pbOutBuffer[dBlockLen-1];
			if( ((int)dPadLen<=0) || (dPadLen>(int)dBlockLen) )
				return CTR_PAD_CHECK_ERROR;
			for( i=1; i<=dPadLen; i++)
				if( pbOutBuffer[dBlockLen-i] != dPadLen )
					return CTR_PAD_CHECK_ERROR;
			return dPadLen;

		default :
			return CTR_FATAL_ERROR;
	}
}

