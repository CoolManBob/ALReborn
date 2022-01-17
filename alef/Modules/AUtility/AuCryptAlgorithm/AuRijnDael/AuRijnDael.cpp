// AuRijnDael.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.21.

#include "AuRijnDael.h"
#include "AuRijnDael.h2"	// holds the random digit tables

#define KEY_BIT			128
#define BLOCK_SIZE		16

#define FULL_UNROLL

#define RIJNDAEL_SWAP(x) (_lrotl(x, 8) & 0x00ff00ff | _lrotr(x, 8) & 0xff00ff00)
#define GETU32(p) RIJNDAEL_SWAP(*((u32 *)(p)))
#define PUTU32(ct, st) { *((u32 *)(ct)) = RIJNDAEL_SWAP((st)); }

AuRijnDael::AuRijnDael()
{
}

AuRijnDael::~AuRijnDael()
{
}

void AuRijnDael::RijnDaelKeySetupEnc(u32* rk, const u8* cipherKey)
{
	int i;
	u32 temp;

	rk[0] = GETU32(cipherKey     );
	rk[1] = GETU32(cipherKey +  4);
	rk[2] = GETU32(cipherKey +  8);
	rk[3] = GETU32(cipherKey + 12);

	for (i = 0; i < 10; i++) {
		temp  = rk[3];
		rk[4] = rk[0] ^
			(Te4[(temp >> 16) & 0xff] & 0xff000000) ^
			(Te4[(temp >>  8) & 0xff] & 0x00ff0000) ^
			(Te4[(temp      ) & 0xff] & 0x0000ff00) ^
			(Te4[(temp >> 24)       ] & 0x000000ff) ^
			rcon[i];
		rk[5] = rk[1] ^ rk[4];
		rk[6] = rk[2] ^ rk[5];
		rk[7] = rk[3] ^ rk[6];
		rk += 4;
	}
}

void AuRijnDael::RijnDaelKeySetupDec(u32* rk, const u8* cipherKey)
{
	int Nr = 10, i, j;
	u32 temp;

	/* expand the cipher key: */
	RijnDaelKeySetupEnc(rk, cipherKey);

	/* invert the order of the round keys: */
	for (i = 0, j = 4*Nr; i < j; i += 4, j -= 4) {
		temp = rk[i    ]; rk[i    ] = rk[j    ]; rk[j    ] = temp;
		temp = rk[i + 1]; rk[i + 1] = rk[j + 1]; rk[j + 1] = temp;
		temp = rk[i + 2]; rk[i + 2] = rk[j + 2]; rk[j + 2] = temp;
		temp = rk[i + 3]; rk[i + 3] = rk[j + 3]; rk[j + 3] = temp;
	}

	/* apply the inverse MixColumn transform to all round keys but the
	 * first and the last: */
	for (i = 1; i < Nr; i++) {
		rk += 4;
		rk[0] =
			Td0[Te4[(rk[0] >> 24)       ] & 0xff] ^
			Td1[Te4[(rk[0] >> 16) & 0xff] & 0xff] ^
			Td2[Te4[(rk[0] >>  8) & 0xff] & 0xff] ^
			Td3[Te4[(rk[0]      ) & 0xff] & 0xff];
		rk[1] =
			Td0[Te4[(rk[1] >> 24)       ] & 0xff] ^
			Td1[Te4[(rk[1] >> 16) & 0xff] & 0xff] ^
			Td2[Te4[(rk[1] >>  8) & 0xff] & 0xff] ^
			Td3[Te4[(rk[1]      ) & 0xff] & 0xff];
		rk[2] =
			Td0[Te4[(rk[2] >> 24)       ] & 0xff] ^
			Td1[Te4[(rk[2] >> 16) & 0xff] & 0xff] ^
			Td2[Te4[(rk[2] >>  8) & 0xff] & 0xff] ^
			Td3[Te4[(rk[2]      ) & 0xff] & 0xff];
		rk[3] =
			Td0[Te4[(rk[3] >> 24)       ] & 0xff] ^
			Td1[Te4[(rk[3] >> 16) & 0xff] & 0xff] ^
			Td2[Te4[(rk[3] >>  8) & 0xff] & 0xff] ^
			Td3[Te4[(rk[3]      ) & 0xff] & 0xff];
	}
}

void AuRijnDael::RijnDaelEncrypt(const u32* rk, const u8* pt, u8* ct)
{
	u32 s0, s1, s2, s3, t0, t1, t2, t3;
	int Nr = 10;
#ifndef FULL_UNROLL
	int r;
#endif /* ?FULL_UNROLL */

	/*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	s0 = GETU32(pt     ) ^ rk[0];
	s1 = GETU32(pt +  4) ^ rk[1];
	s2 = GETU32(pt +  8) ^ rk[2];
	s3 = GETU32(pt + 12) ^ rk[3];

#ifdef FULL_UNROLL
	/* round 1: */
	t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >>  8) & 0xff] ^ Te3[s3 & 0xff] ^ rk[ 4];
	t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >>  8) & 0xff] ^ Te3[s0 & 0xff] ^ rk[ 5];
	t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >>  8) & 0xff] ^ Te3[s1 & 0xff] ^ rk[ 6];
	t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >>  8) & 0xff] ^ Te3[s2 & 0xff] ^ rk[ 7];
	/* round 2: */
	s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >>  8) & 0xff] ^ Te3[t3 & 0xff] ^ rk[ 8];
	s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >>  8) & 0xff] ^ Te3[t0 & 0xff] ^ rk[ 9];
	s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >>  8) & 0xff] ^ Te3[t1 & 0xff] ^ rk[10];
	s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >>  8) & 0xff] ^ Te3[t2 & 0xff] ^ rk[11];
	/* round 3: */
	t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >>  8) & 0xff] ^ Te3[s3 & 0xff] ^ rk[12];
	t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >>  8) & 0xff] ^ Te3[s0 & 0xff] ^ rk[13];
	t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >>  8) & 0xff] ^ Te3[s1 & 0xff] ^ rk[14];
	t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >>  8) & 0xff] ^ Te3[s2 & 0xff] ^ rk[15];
	/* round 4: */
	s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >>  8) & 0xff] ^ Te3[t3 & 0xff] ^ rk[16];
	s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >>  8) & 0xff] ^ Te3[t0 & 0xff] ^ rk[17];
	s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >>  8) & 0xff] ^ Te3[t1 & 0xff] ^ rk[18];
	s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >>  8) & 0xff] ^ Te3[t2 & 0xff] ^ rk[19];
	/* round 5: */
	t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >>  8) & 0xff] ^ Te3[s3 & 0xff] ^ rk[20];
	t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >>  8) & 0xff] ^ Te3[s0 & 0xff] ^ rk[21];
	t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >>  8) & 0xff] ^ Te3[s1 & 0xff] ^ rk[22];
	t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >>  8) & 0xff] ^ Te3[s2 & 0xff] ^ rk[23];
	/* round 6: */
	s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >>  8) & 0xff] ^ Te3[t3 & 0xff] ^ rk[24];
	s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >>  8) & 0xff] ^ Te3[t0 & 0xff] ^ rk[25];
	s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >>  8) & 0xff] ^ Te3[t1 & 0xff] ^ rk[26];
	s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >>  8) & 0xff] ^ Te3[t2 & 0xff] ^ rk[27];
	/* round 7: */
	t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >>  8) & 0xff] ^ Te3[s3 & 0xff] ^ rk[28];
	t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >>  8) & 0xff] ^ Te3[s0 & 0xff] ^ rk[29];
	t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >>  8) & 0xff] ^ Te3[s1 & 0xff] ^ rk[30];
	t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >>  8) & 0xff] ^ Te3[s2 & 0xff] ^ rk[31];
	/* round 8: */
	s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >>  8) & 0xff] ^ Te3[t3 & 0xff] ^ rk[32];
	s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >>  8) & 0xff] ^ Te3[t0 & 0xff] ^ rk[33];
	s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >>  8) & 0xff] ^ Te3[t1 & 0xff] ^ rk[34];
	s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >>  8) & 0xff] ^ Te3[t2 & 0xff] ^ rk[35];
	/* round 9: */
	t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >>  8) & 0xff] ^ Te3[s3 & 0xff] ^ rk[36];
	t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >>  8) & 0xff] ^ Te3[s0 & 0xff] ^ rk[37];
	t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >>  8) & 0xff] ^ Te3[s1 & 0xff] ^ rk[38];
	t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >>  8) & 0xff] ^ Te3[s2 & 0xff] ^ rk[39];
	rk += Nr << 2;
#else  /* !FULL_UNROLL */
	/*
	 * Nr - 1 full rounds:
	 */
	r = Nr >> 1;
	for (;;) {
		t0 =
		    Te0[(s0 >> 24)       ] ^
		    Te1[(s1 >> 16) & 0xff] ^
		    Te2[(s2 >>  8) & 0xff] ^
		    Te3[(s3      ) & 0xff] ^
		    rk[4];
		t1 =
		    Te0[(s1 >> 24)       ] ^
		    Te1[(s2 >> 16) & 0xff] ^
		    Te2[(s3 >>  8) & 0xff] ^
		    Te3[(s0      ) & 0xff] ^
		    rk[5];
		t2 =
		    Te0[(s2 >> 24)       ] ^
		    Te1[(s3 >> 16) & 0xff] ^
		    Te2[(s0 >>  8) & 0xff] ^
		    Te3[(s1      ) & 0xff] ^
		    rk[6];
		t3 =
		    Te0[(s3 >> 24)       ] ^
		    Te1[(s0 >> 16) & 0xff] ^
		    Te2[(s1 >>  8) & 0xff] ^
		    Te3[(s2      ) & 0xff] ^
		    rk[7];

		rk += 8;
		if (--r == 0) {
		    break;
		}

		s0 =
		    Te0[(t0 >> 24)       ] ^
		    Te1[(t1 >> 16) & 0xff] ^
		    Te2[(t2 >>  8) & 0xff] ^
		    Te3[(t3      ) & 0xff] ^
		    rk[0];
		s1 =
		    Te0[(t1 >> 24)       ] ^
		    Te1[(t2 >> 16) & 0xff] ^
		    Te2[(t3 >>  8) & 0xff] ^
		    Te3[(t0      ) & 0xff] ^
		    rk[1];
		s2 =
		    Te0[(t2 >> 24)       ] ^
		    Te1[(t3 >> 16) & 0xff] ^
		    Te2[(t0 >>  8) & 0xff] ^
		    Te3[(t1      ) & 0xff] ^
		    rk[2];
		s3 =
		    Te0[(t3 >> 24)       ] ^
		    Te1[(t0 >> 16) & 0xff] ^
		    Te2[(t1 >>  8) & 0xff] ^
		    Te3[(t2      ) & 0xff] ^
		    rk[3];
	}
#endif /* ?FULL_UNROLL */

	/*
	 * apply last round and
	 * map cipher state to byte array block:
	 */
	s0 =
		(Te4[(t0 >> 24)       ] & 0xff000000) ^
		(Te4[(t1 >> 16) & 0xff] & 0x00ff0000) ^
		(Te4[(t2 >>  8) & 0xff] & 0x0000ff00) ^
		(Te4[(t3      ) & 0xff] & 0x000000ff) ^
		rk[0];
	PUTU32(ct     , s0);
	s1 =
		(Te4[(t1 >> 24)       ] & 0xff000000) ^
		(Te4[(t2 >> 16) & 0xff] & 0x00ff0000) ^
		(Te4[(t3 >>  8) & 0xff] & 0x0000ff00) ^
		(Te4[(t0      ) & 0xff] & 0x000000ff) ^
		rk[1];
	PUTU32(ct +  4, s1);
	s2 =
		(Te4[(t2 >> 24)       ] & 0xff000000) ^
		(Te4[(t3 >> 16) & 0xff] & 0x00ff0000) ^
		(Te4[(t0 >>  8) & 0xff] & 0x0000ff00) ^
		(Te4[(t1      ) & 0xff] & 0x000000ff) ^
		rk[2];
	PUTU32(ct +  8, s2);
	s3 =
		(Te4[(t3 >> 24)       ] & 0xff000000) ^
		(Te4[(t0 >> 16) & 0xff] & 0x00ff0000) ^
		(Te4[(t1 >>  8) & 0xff] & 0x0000ff00) ^
		(Te4[(t2      ) & 0xff] & 0x000000ff) ^
		rk[3];
	PUTU32(ct + 12, s3);
}

void AuRijnDael::RijnDaelDecrypt(const u32* rk, const u8* ct, u8* pt)
{
	u32 s0, s1, s2, s3, t0, t1, t2, t3;
	int Nr = 10;
#ifndef FULL_UNROLL
	int r;
#endif /* ?FULL_UNROLL */

	/*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	s0 = GETU32(ct     ) ^ rk[0];
	s1 = GETU32(ct +  4) ^ rk[1];
	s2 = GETU32(ct +  8) ^ rk[2];
	s3 = GETU32(ct + 12) ^ rk[3];

#ifdef FULL_UNROLL
	/* round 1: */
	t0 = Td0[s0 >> 24] ^ Td1[(s3 >> 16) & 0xff] ^ Td2[(s2 >>  8) & 0xff] ^ Td3[s1 & 0xff] ^ rk[ 4];
	t1 = Td0[s1 >> 24] ^ Td1[(s0 >> 16) & 0xff] ^ Td2[(s3 >>  8) & 0xff] ^ Td3[s2 & 0xff] ^ rk[ 5];
	t2 = Td0[s2 >> 24] ^ Td1[(s1 >> 16) & 0xff] ^ Td2[(s0 >>  8) & 0xff] ^ Td3[s3 & 0xff] ^ rk[ 6];
	t3 = Td0[s3 >> 24] ^ Td1[(s2 >> 16) & 0xff] ^ Td2[(s1 >>  8) & 0xff] ^ Td3[s0 & 0xff] ^ rk[ 7];
	/* round 2: */
	s0 = Td0[t0 >> 24] ^ Td1[(t3 >> 16) & 0xff] ^ Td2[(t2 >>  8) & 0xff] ^ Td3[t1 & 0xff] ^ rk[ 8];
	s1 = Td0[t1 >> 24] ^ Td1[(t0 >> 16) & 0xff] ^ Td2[(t3 >>  8) & 0xff] ^ Td3[t2 & 0xff] ^ rk[ 9];
	s2 = Td0[t2 >> 24] ^ Td1[(t1 >> 16) & 0xff] ^ Td2[(t0 >>  8) & 0xff] ^ Td3[t3 & 0xff] ^ rk[10];
	s3 = Td0[t3 >> 24] ^ Td1[(t2 >> 16) & 0xff] ^ Td2[(t1 >>  8) & 0xff] ^ Td3[t0 & 0xff] ^ rk[11];
	/* round 3: */
	t0 = Td0[s0 >> 24] ^ Td1[(s3 >> 16) & 0xff] ^ Td2[(s2 >>  8) & 0xff] ^ Td3[s1 & 0xff] ^ rk[12];
	t1 = Td0[s1 >> 24] ^ Td1[(s0 >> 16) & 0xff] ^ Td2[(s3 >>  8) & 0xff] ^ Td3[s2 & 0xff] ^ rk[13];
	t2 = Td0[s2 >> 24] ^ Td1[(s1 >> 16) & 0xff] ^ Td2[(s0 >>  8) & 0xff] ^ Td3[s3 & 0xff] ^ rk[14];
	t3 = Td0[s3 >> 24] ^ Td1[(s2 >> 16) & 0xff] ^ Td2[(s1 >>  8) & 0xff] ^ Td3[s0 & 0xff] ^ rk[15];
	/* round 4: */
	s0 = Td0[t0 >> 24] ^ Td1[(t3 >> 16) & 0xff] ^ Td2[(t2 >>  8) & 0xff] ^ Td3[t1 & 0xff] ^ rk[16];
	s1 = Td0[t1 >> 24] ^ Td1[(t0 >> 16) & 0xff] ^ Td2[(t3 >>  8) & 0xff] ^ Td3[t2 & 0xff] ^ rk[17];
	s2 = Td0[t2 >> 24] ^ Td1[(t1 >> 16) & 0xff] ^ Td2[(t0 >>  8) & 0xff] ^ Td3[t3 & 0xff] ^ rk[18];
	s3 = Td0[t3 >> 24] ^ Td1[(t2 >> 16) & 0xff] ^ Td2[(t1 >>  8) & 0xff] ^ Td3[t0 & 0xff] ^ rk[19];
	/* round 5: */
	t0 = Td0[s0 >> 24] ^ Td1[(s3 >> 16) & 0xff] ^ Td2[(s2 >>  8) & 0xff] ^ Td3[s1 & 0xff] ^ rk[20];
	t1 = Td0[s1 >> 24] ^ Td1[(s0 >> 16) & 0xff] ^ Td2[(s3 >>  8) & 0xff] ^ Td3[s2 & 0xff] ^ rk[21];
	t2 = Td0[s2 >> 24] ^ Td1[(s1 >> 16) & 0xff] ^ Td2[(s0 >>  8) & 0xff] ^ Td3[s3 & 0xff] ^ rk[22];
	t3 = Td0[s3 >> 24] ^ Td1[(s2 >> 16) & 0xff] ^ Td2[(s1 >>  8) & 0xff] ^ Td3[s0 & 0xff] ^ rk[23];
	/* round 6: */
	s0 = Td0[t0 >> 24] ^ Td1[(t3 >> 16) & 0xff] ^ Td2[(t2 >>  8) & 0xff] ^ Td3[t1 & 0xff] ^ rk[24];
	s1 = Td0[t1 >> 24] ^ Td1[(t0 >> 16) & 0xff] ^ Td2[(t3 >>  8) & 0xff] ^ Td3[t2 & 0xff] ^ rk[25];
	s2 = Td0[t2 >> 24] ^ Td1[(t1 >> 16) & 0xff] ^ Td2[(t0 >>  8) & 0xff] ^ Td3[t3 & 0xff] ^ rk[26];
	s3 = Td0[t3 >> 24] ^ Td1[(t2 >> 16) & 0xff] ^ Td2[(t1 >>  8) & 0xff] ^ Td3[t0 & 0xff] ^ rk[27];
	/* round 7: */
	t0 = Td0[s0 >> 24] ^ Td1[(s3 >> 16) & 0xff] ^ Td2[(s2 >>  8) & 0xff] ^ Td3[s1 & 0xff] ^ rk[28];
	t1 = Td0[s1 >> 24] ^ Td1[(s0 >> 16) & 0xff] ^ Td2[(s3 >>  8) & 0xff] ^ Td3[s2 & 0xff] ^ rk[29];
	t2 = Td0[s2 >> 24] ^ Td1[(s1 >> 16) & 0xff] ^ Td2[(s0 >>  8) & 0xff] ^ Td3[s3 & 0xff] ^ rk[30];
	t3 = Td0[s3 >> 24] ^ Td1[(s2 >> 16) & 0xff] ^ Td2[(s1 >>  8) & 0xff] ^ Td3[s0 & 0xff] ^ rk[31];
	/* round 8: */
	s0 = Td0[t0 >> 24] ^ Td1[(t3 >> 16) & 0xff] ^ Td2[(t2 >>  8) & 0xff] ^ Td3[t1 & 0xff] ^ rk[32];
	s1 = Td0[t1 >> 24] ^ Td1[(t0 >> 16) & 0xff] ^ Td2[(t3 >>  8) & 0xff] ^ Td3[t2 & 0xff] ^ rk[33];
	s2 = Td0[t2 >> 24] ^ Td1[(t1 >> 16) & 0xff] ^ Td2[(t0 >>  8) & 0xff] ^ Td3[t3 & 0xff] ^ rk[34];
	s3 = Td0[t3 >> 24] ^ Td1[(t2 >> 16) & 0xff] ^ Td2[(t1 >>  8) & 0xff] ^ Td3[t0 & 0xff] ^ rk[35];
	/* round 9: */
	t0 = Td0[s0 >> 24] ^ Td1[(s3 >> 16) & 0xff] ^ Td2[(s2 >>  8) & 0xff] ^ Td3[s1 & 0xff] ^ rk[36];
	t1 = Td0[s1 >> 24] ^ Td1[(s0 >> 16) & 0xff] ^ Td2[(s3 >>  8) & 0xff] ^ Td3[s2 & 0xff] ^ rk[37];
	t2 = Td0[s2 >> 24] ^ Td1[(s1 >> 16) & 0xff] ^ Td2[(s0 >>  8) & 0xff] ^ Td3[s3 & 0xff] ^ rk[38];
	t3 = Td0[s3 >> 24] ^ Td1[(s2 >> 16) & 0xff] ^ Td2[(s1 >>  8) & 0xff] ^ Td3[s0 & 0xff] ^ rk[39];
	rk += Nr << 2;
#else  /* !FULL_UNROLL */
	/*
	 * Nr - 1 full rounds:
	 */
	r = Nr >> 1;
	for (;;) {
		t0 =
		    Td0[(s0 >> 24)       ] ^
		    Td1[(s3 >> 16) & 0xff] ^
		    Td2[(s2 >>  8) & 0xff] ^
		    Td3[(s1      ) & 0xff] ^
		    rk[4];
		t1 =
		    Td0[(s1 >> 24)       ] ^
		    Td1[(s0 >> 16) & 0xff] ^
		    Td2[(s3 >>  8) & 0xff] ^
		    Td3[(s2      ) & 0xff] ^
		    rk[5];
		t2 =
		    Td0[(s2 >> 24)       ] ^
		    Td1[(s1 >> 16) & 0xff] ^
		    Td2[(s0 >>  8) & 0xff] ^
		    Td3[(s3      ) & 0xff] ^
		    rk[6];
		t3 =
		    Td0[(s3 >> 24)       ] ^
		    Td1[(s2 >> 16) & 0xff] ^
		    Td2[(s1 >>  8) & 0xff] ^
		    Td3[(s0      ) & 0xff] ^
		    rk[7];

		rk += 8;
		if (--r == 0) {
		    break;
		}

		s0 =
		    Td0[(t0 >> 24)       ] ^
		    Td1[(t3 >> 16) & 0xff] ^
		    Td2[(t2 >>  8) & 0xff] ^
		    Td3[(t1      ) & 0xff] ^
		    rk[0];
		s1 =
		    Td0[(t1 >> 24)       ] ^
		    Td1[(t0 >> 16) & 0xff] ^
		    Td2[(t3 >>  8) & 0xff] ^
		    Td3[(t2      ) & 0xff] ^
		    rk[1];
		s2 =
		    Td0[(t2 >> 24)       ] ^
		    Td1[(t1 >> 16) & 0xff] ^
		    Td2[(t0 >>  8) & 0xff] ^
		    Td3[(t3      ) & 0xff] ^
		    rk[2];
		s3 =
		    Td0[(t3 >> 24)       ] ^
		    Td1[(t2 >> 16) & 0xff] ^
		    Td2[(t1 >>  8) & 0xff] ^
		    Td3[(t0      ) & 0xff] ^
		    rk[3];
	}
#endif /* ?FULL_UNROLL */

	/*
	 * apply last round and
	 * map cipher state to byte array block:
	 */
	s0 =
		(Td4[(t0 >> 24)       ] & 0xff000000) ^
		(Td4[(t3 >> 16) & 0xff] & 0x00ff0000) ^
		(Td4[(t2 >>  8) & 0xff] & 0x0000ff00) ^
		(Td4[(t1      ) & 0xff] & 0x000000ff) ^
		rk[0];
	PUTU32(pt     , s0);
	s1 =
		(Td4[(t1 >> 24)       ] & 0xff000000) ^
		(Td4[(t0 >> 16) & 0xff] & 0x00ff0000) ^
		(Td4[(t3 >>  8) & 0xff] & 0x0000ff00) ^
		(Td4[(t2      ) & 0xff] & 0x000000ff) ^
		rk[1];
	PUTU32(pt +  4, s1);
	s2 =
		(Td4[(t2 >> 24)       ] & 0xff000000) ^
		(Td4[(t1 >> 16) & 0xff] & 0x00ff0000) ^
		(Td4[(t0 >>  8) & 0xff] & 0x0000ff00) ^
		(Td4[(t3      ) & 0xff] & 0x000000ff) ^
		rk[2];
	PUTU32(pt +  8, s2);
	s3 =
		(Td4[(t3 >> 24)       ] & 0xff000000) ^
		(Td4[(t2 >> 16) & 0xff] & 0x00ff0000) ^
		(Td4[(t1 >>  8) & 0xff] & 0x0000ff00) ^
		(Td4[(t0      ) & 0xff] & 0x000000ff) ^
		rk[3];
	PUTU32(pt + 12, s3);
}

// 넘어오는 Key 의 값이 적어도 12Bytes 이상이어야 한다.
// 현재(2006.04.21) 모든 알고리즘에서 Base Key Size 는 32Bytes 로 고정하였다.
void AuRijnDael::Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone)
{
	if(!pctx || !key || lKeySize < 1)
		return;

	RIJNDAEL_CTX& ctx = *static_cast<RIJNDAEL_CTX*>(pctx);

	// 먼저 IV 생성
	memset(ctx.m_RandomIV, 0, sizeof(ctx.m_RandomIV));
	//srand(timeGetTime());

	//for(int i = 0; i < IV_LENGTH; i++)
	//	ctx.m_RandomIV[i] = (BYTE)(rand()%256);

	// 2006.04.24. steeple
	// IV 를 랜덤으로 생성하면 Client, Server 간에 다르게 된다.
	// 그래서 넘어온 키 값의 8바이트 위치부터 16바이트를 IV 로 세팅한다.
	memcpy(ctx.m_RandomIV, key + 8, sizeof(ctx.m_RandomIV));

	// Encrypt, Decrypt 용 키를 생성해준다.
	RijnDaelKeySetupEnc(ctx.m_EncryptKey, key);
	RijnDaelKeySetupDec(ctx.m_DecryptKey, key);
}

// pInput 이 그대로 암호화 되기 때문에, pOutput 에 복사해준다.
DWORD AuRijnDael::Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return 0;

	RIJNDAEL_CTX& ctx = *static_cast<RIJNDAEL_CTX*>(pctx);

	u8 cbc[BLOCK_SIZE];
	u8 *pos = pInput;
	int i, j, blocks;

	memcpy(cbc, ctx.m_RandomIV, BLOCK_SIZE);

	blocks = lSize / BLOCK_SIZE;
	for (i = 0; i < blocks; i++) {
		for (j = 0; j < BLOCK_SIZE; j++)
			cbc[j] ^= pos[j];
		RijnDaelEncrypt((const u32*)ctx.m_EncryptKey, cbc, cbc);
		memcpy(pos, cbc, BLOCK_SIZE);
		pos += BLOCK_SIZE;
	}

	memcpy(pOutput, pInput, lSize);
	return lSize;
}

void AuRijnDael::Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return;

	RIJNDAEL_CTX& ctx = *static_cast<RIJNDAEL_CTX*>(pctx);

	u8 cbc[BLOCK_SIZE], tmp[BLOCK_SIZE];
	u8 *pos = pInput;
	int i, j, blocks;

	memcpy(cbc, ctx.m_RandomIV, BLOCK_SIZE);

	blocks = lSize / BLOCK_SIZE;
	for (i = 0; i < blocks; i++) {
		memcpy(tmp, pos, BLOCK_SIZE);
		RijnDaelDecrypt((const u32*)ctx.m_DecryptKey, pos, pos);
		for (j = 0; j < BLOCK_SIZE; j++)
			pos[j] ^= cbc[j];
		memcpy(cbc, tmp, BLOCK_SIZE);
		pos += BLOCK_SIZE;
	}

	memcpy(pOutput, pInput, lSize);
}

// 크기가 변하지 않는다.
DWORD AuRijnDael::GetOutputSize(DWORD lInputSize)
{
	return lInputSize;
}