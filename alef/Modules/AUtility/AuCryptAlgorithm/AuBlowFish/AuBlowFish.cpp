// AuBlowFish.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.07.

#include "AuBlowFish.h"
#include "AuBlowFish.h2"	// holds the random digit tables

#define S(x,i) (ctx.SBox[i][x.w.byte##i])
#define bf_F(x) (((S(x,0) + S(x,1)) ^ S(x,2)) + S(x,3))
#define ROUND(a,b,n) (a.dword ^= bf_F(b) ^ ctx.P[n])

AuBlowFish::AuBlowFish()
{
}

AuBlowFish::~AuBlowFish()
{
}

// the low level (private) encryption function
void AuBlowFish::Blowfish_encipher(BLOWFISH_CTX& ctx, DWORD *xl, DWORD *xr)
{
	union aword  Xl, Xr ;

	Xl.dword = *xl ;
	Xr.dword = *xr ;

	Xl.dword ^= ctx.P[0];
	ROUND (Xr, Xl, 1) ;  ROUND (Xl, Xr, 2) ;
	ROUND (Xr, Xl, 3) ;  ROUND (Xl, Xr, 4) ;
	ROUND (Xr, Xl, 5) ;  ROUND (Xl, Xr, 6) ;
	ROUND (Xr, Xl, 7) ;  ROUND (Xl, Xr, 8) ;
	ROUND (Xr, Xl, 9) ;  ROUND (Xl, Xr, 10) ;
	ROUND (Xr, Xl, 11) ; ROUND (Xl, Xr, 12) ;
	ROUND (Xr, Xl, 13) ; ROUND (Xl, Xr, 14) ;
	ROUND (Xr, Xl, 15) ; ROUND (Xl, Xr, 16) ;
	Xr.dword ^= ctx.P[17] ;

	*xr = Xl.dword ;
	*xl = Xr.dword ;
}

// the low level (private) decryption function
void AuBlowFish::Blowfish_decipher(BLOWFISH_CTX& ctx, DWORD *xl, DWORD *xr)
{
   union aword  Xl ;
   union aword  Xr ;

   Xl.dword = *xl ;
   Xr.dword = *xr ;

   Xl.dword ^= ctx.P[17] ;
   ROUND (Xr, Xl, 16) ;  ROUND (Xl, Xr, 15) ;
   ROUND (Xr, Xl, 14) ;  ROUND (Xl, Xr, 13) ;
   ROUND (Xr, Xl, 12) ;  ROUND (Xl, Xr, 11) ;
   ROUND (Xr, Xl, 10) ;  ROUND (Xl, Xr, 9) ;
   ROUND (Xr, Xl, 8) ;   ROUND (Xl, Xr, 7) ;
   ROUND (Xr, Xl, 6) ;   ROUND (Xl, Xr, 5) ;
   ROUND (Xr, Xl, 4) ;   ROUND (Xl, Xr, 3) ;
   ROUND (Xr, Xl, 2) ;   ROUND (Xl, Xr, 1) ;
   Xr.dword ^= ctx.P[0];

   *xl = Xr.dword;
   *xr = Xl.dword;
}


// constructs the enctryption sieve
void AuBlowFish::Initialize(void* pctx, BYTE* key, DWORD keybytes, BOOL bClone)
{
	if(!pctx || !key || keybytes < 1)
		return;

	BLOWFISH_CTX& ctx = *static_cast<BLOWFISH_CTX*>(pctx);

	int  		i, j ;
	DWORD  		data, datal, datar ;
	union aword temp ;

	// first fill arrays from data tables
	for (i = 0 ; i < 18 ; i++)
		ctx.P[i] = bf_P [i] ;

	for (i = 0 ; i < 4 ; i++)
	{
	 	for (j = 0 ; j < 256 ; j++)
	 		ctx.SBox[i][j] = bf_S [i][j] ;
	}


	j = 0 ;
	for (i = 0 ; i < NPASS + 2 ; ++i)
	{
		temp.dword = 0 ;
		temp.w.byte0 = key[j];
		temp.w.byte1 = key[(j+1) % keybytes] ;
		temp.w.byte2 = key[(j+2) % keybytes] ;
		temp.w.byte3 = key[(j+3) % keybytes] ;
		data = temp.dword ;
		ctx.P[i] ^= data ;
		j = (j + 4) % keybytes ;
	}

	datal = 0 ;
	datar = 0 ;

	for (i = 0 ; i < NPASS + 2 ; i += 2)
	{
		Blowfish_encipher (ctx, &datal, &datar) ;
		ctx.P[i] = datal ;
		ctx.P[i + 1] = datar ;
	}

	for (i = 0 ; i < 4 ; ++i)
	{
		for (j = 0 ; j < 256 ; j += 2)
		{
		  Blowfish_encipher (ctx, &datal, &datar) ;
		  ctx.SBox[i][j] = datal ;
		  ctx.SBox[i][j + 1] = datar ;
		}
	}
}

// get output length, which must be even MOD 8
DWORD AuBlowFish::GetOutputSize(DWORD lInputSize)
{
	DWORD 	lVal ;

	lVal = lInputSize % 8 ;	// find out if uneven number of bytes atthe end
	if (lVal != 0)
		return lInputSize + 8 - lVal ;
	else
		return lInputSize ;
}

// Encode pIntput into pOutput.  Input length in lSize.  Returned value
// is length of output which will be even MOD 8 bytes.  Inputbuffer and
// output buffer can be the same, but be sure buffer length is even MOD8.
DWORD AuBlowFish::Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return 0;

	BLOWFISH_CTX& ctx = *static_cast<BLOWFISH_CTX*>(pctx);

	DWORD 	lCount, lOutSize, lGoodBytes ;
	BYTE	*pi, *po ;
	int		i, j ;
	int		SameDest = (pInput == pOutput ? 1 : 0) ;

	lOutSize = GetOutputSize (lSize) ;
	for (lCount = 0 ; lCount < lOutSize ; lCount += 8)
	{
		if (SameDest)	// if encoded data is being written into inputbuffer
		{
		 	if (lCount < lSize - 7)	// if not dealing with unevenbytes at end
		 	{
		 	 	Blowfish_encipher (ctx, (DWORD *) pInput, (DWORD *) (pInput + 4)) ;
		 	}
		 	else		// pad end of data with null bytes tocomplete encryption
		 	{
				po = pInput + lSize ;	// point at bytepast theend of actual data
				j = (int) (lOutSize - lSize) ;	// number ofbytes to set to null
				for (i = 0 ; i < j ; i++)
					*po++ = 0 ;
		 	 	Blowfish_encipher (ctx, (DWORD *) pInput, (DWORD *) (pInput + 4)) ;
		 	}
		 	pInput += 8 ;
		}
		else 			// output buffer not equal to inputbuffer, so must copy
		{               // input to output buffer prior to encrypting
		 	if (lCount < lSize - 7)	// if not dealing with unevenbytes at end
		 	{
		 		pi = pInput ;
		 		po = pOutput ;
		 		for (i = 0 ; i < 8 ; i++)
					// copy bytes to output
		 			*po++ = *pi++ ;

		 	 	Blowfish_encipher (ctx, (DWORD *) pOutput, (DWORD *) (pOutput + 4)) ;	// nowencrypt them
		 	}
		 	else		// pad end of data with null bytes tocomplete encryption
		 	{
		 		lGoodBytes = lSize - lCount ;	// number ofremaining data bytes
		 		po = pOutput ;
		 		for (i = 0 ; i < (int) lGoodBytes ; i++)
		 			*po++ = *pInput++ ;
		 		for (j = i ; j < 8 ; j++)
		 			*po++ = 0 ;
		 	 	Blowfish_encipher (ctx, (DWORD *) pOutput, (DWORD *) (pOutput + 4)) ;
		 	}
		 	pInput += 8 ;
		 	pOutput += 8 ;
		}
	}
	return lOutSize ;
 }

// Decode pIntput into pOutput.  Input length in lSize.  Inputbuffer and
// output buffer can be the same, but be sure buffer length is even MOD8.
void AuBlowFish::Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return;

	BLOWFISH_CTX& ctx = *static_cast<BLOWFISH_CTX*>(pctx);

	DWORD 	lCount ;
	BYTE	*pi, *po ;
	int		i ;
	int		SameDest = (pInput == pOutput ? 1 : 0) ;

	for (lCount = 0 ; lCount < lSize ; lCount += 8)
	{
		if (SameDest)	// if encoded data is being written into inputbuffer
		{
	 	 	Blowfish_decipher (ctx, (DWORD *) pInput, (DWORD *) (pInput + 4)) ;
		 	pInput += 8 ;
		}
		else 			// output buffer not equal to inputbuffer
		{               // so copy input to output before decoding
	 		pi = pInput ;
	 		po = pOutput ;
	 		for (i = 0 ; i < 8 ; i++)
	 			*po++ = *pi++ ;
	 	 	Blowfish_decipher (ctx, (DWORD *) pOutput, (DWORD *) (pOutput + 4)) ;
		 	pInput += 8 ;
		 	pOutput += 8 ;
		}
	}
}

