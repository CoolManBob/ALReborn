// AuMD5Mac.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.17.

#include "AuMD5Mac.h"

AuMD5Mac::AuMD5Mac()
{
	Startup();
}

AuMD5Mac::~AuMD5Mac()
{
	Cleanup();
}

void AuMD5Mac::Startup()
{
	CryptSetProvider(MS_DEF_PROV, PROV_RSA_FULL);

	if(!CryptAcquireContext(&m_hCryptProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		ASSERT(!"AuMD5Mac::Startup() CryptAcquireContext Failure!!!");
		return;
	}
}

void AuMD5Mac::Cleanup()
{
	if(m_hCryptProv) 
	   CryptReleaseContext(m_hCryptProv, 0);
}

void AuMD5Mac::Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone)
{
	if(!pctx || !key || lKeySize < 1)
		return;

	MD5MAC_CTX& ctx = *static_cast<MD5MAC_CTX*>(pctx);

	if(lKeySize != sizeof(ctx.m_HashByte))
	{
		ASSERT(!"넘어온 KeySize 가 sizeof(ctx.m_HashByte) 에 미치지 못한다.");
		return;
	}

	if(bClone)
	{
		// 그냥 키를 복사한다.
		memcpy(ctx.m_HashByte, key, lKeySize);
		return;
	}

	// Random Hash String 을 만들어준다.
	memset(ctx.m_HashByte, 0, sizeof(ctx.m_HashByte));
	srand(timeGetTime());

	for(int i = 0; i < AUCRYPT_KEY_LENGTH_32BYTE; i++)
		ctx.m_HashByte[i] = (BYTE)(rand()%256);

	// 입력된 Key 에 상관없이 Hash 값을 키로 한다.
	// 넘어온 key 가 ctx.m_HashByte 크기에 미치지 못하면 문제가 생긴다.
	memcpy(key, ctx.m_HashByte, sizeof(ctx.m_HashByte));
}

// MD5Mac 은 pInput 이 같은 크기로 그냥 바뀌기 때문에, pOutput 에 그냥 카피해준다.
DWORD AuMD5Mac::Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return 0;

	MD5MAC_CTX& ctx = *static_cast<MD5MAC_CTX*>(pctx);

	HCRYPTKEY		hKey; 
	HCRYPTHASH		hHash; 

	//Hash를 만든다.
	if(!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
		return 0;

	//Hash Object에 Hash Data를 넣는다.
	if(!CryptHashData(hHash, ctx.m_HashByte, AUCRYPT_KEY_LENGTH_32BYTE, 0))
		return 0;

	//Key를 뽑아낸다.
	if(!CryptDeriveKey(m_hCryptProv, CALG_RC4, hHash, CRYPT_CREATE_SALT, &hKey))
		return 0;

	//Key를 뽑아냈으니 Hash OBJ를 초기화한다.
	CryptDestroyHash(hHash);
	hHash = 0;

    //Key를 이용해 pstrData를 Encoding한다.
	if(!CryptEncrypt(hKey, 0, FALSE, 0, pInput, &lSize, lSize))
		return 0;

	//Key를 더이상 사용할 일은 없으므로 없앤다.
	CryptDestroyKey(hKey);

	// pOutput 에 넣어준다.
	memcpy(pOutput, pInput, lSize);
	return lSize;
}

void AuMD5Mac::Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return;

	MD5MAC_CTX& ctx = *static_cast<MD5MAC_CTX*>(pctx);

	HCRYPTKEY		hKey; 
	HCRYPTHASH		hHash; 

	//Hash를 만든다.
	if(!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
		return;

	//Hash Object에 Hash Data를 넣는다.
	if(!CryptHashData(hHash, ctx.m_HashByte, AUCRYPT_KEY_LENGTH_32BYTE, 0))
		return;

	//Key를 뽑아낸다.
	if(!CryptDeriveKey(m_hCryptProv, CALG_RC4, hHash, CRYPT_CREATE_SALT, &hKey))
		return;

	//Key를 뽑아냈으니 Hash OBJ를 초기화한다.
	CryptDestroyHash(hHash); 
	hHash = 0;

	//더이상 Encoding할게 없으니 Final인자를 TRUE로....
	if(!CryptDecrypt(hKey, 0, FALSE, 0, pInput, &lSize))
		return;
	
	//Key를 더이상 사용할 일은 없으므로 없앤다.
	CryptDestroyKey(hKey);

	// Output 쪽에 Copy
	memcpy(pOutput, pInput, lSize);
}

// MD5Mac 은 Input, Output Size 가 같다.
DWORD AuMD5Mac::GetOutputSize(DWORD lInputSize)
{
	return lInputSize;
}
