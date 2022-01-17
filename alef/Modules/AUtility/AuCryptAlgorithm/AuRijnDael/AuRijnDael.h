// AuRijnDael.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.07.

//
// Wrapper for RijnDael algorithm
//

// 예의상 주석문도 같이 퍼왔음. 하하하핫.

/*
 * rijndael-alg-fst.c
 *
 * @version 3.0 (December 2000)
 *
 * Optimised ANSI C code for the Rijndael cipher (now AES)
 *
 * @author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 * @author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 * @author Paulo Barreto <paulo.barreto@terra.com.br>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _AURIJNDAEL_H_
#define _AURIJNDAEL_H_

#include "AuCryptAlgorithmBase.h"

#define ENC_KEY_LENGTH			44
#define IV_LENGTH				16

typedef	unsigned char			u8;
typedef	unsigned int			u32;

class RIJNDAEL_CTX : public BASE_CTX
{
public:
	u32	m_EncryptKey[ENC_KEY_LENGTH];
	u32	m_DecryptKey[ENC_KEY_LENGTH];
	u8	m_RandomIV[IV_LENGTH];

	void init()
	{
		eAlgorithm = AUCRYPT_ALGORITHM_RIJNDAEL;
		memset(m_EncryptKey, 0, sizeof(m_EncryptKey));
		memset(m_DecryptKey, 0, sizeof(m_DecryptKey));
		memset(m_RandomIV, 0, sizeof(m_RandomIV));
	}

	RIJNDAEL_CTX() { init(); }
	~RIJNDAEL_CTX() {;}
};

class AuRijnDael : public AuCryptAlgorithmBase
{
private:
	void	RijnDaelKeySetupEnc(u32* rk, const u8* cipherKey);
	void	RijnDaelKeySetupDec(u32* rk, const u8* cipherKey);
	void	RijnDaelEncrypt(const u32* rk, const u8* pt, u8* ct);
	void	RijnDaelDecrypt(const u32* rk, const u8* ct, u8* pt);

public:
	AuRijnDael();
	virtual ~AuRijnDael();

	void	Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone = FALSE);
	DWORD	Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);
	void	Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);

	DWORD	GetOutputSize(DWORD lInputSize);
};

#endif//_AURIJNDAEL_H_