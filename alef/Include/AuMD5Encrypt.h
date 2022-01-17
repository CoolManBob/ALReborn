#ifndef __AUMD5ENCRYPT_H__
#define __AUMD5ENCRYPT_H__

#include <stdio.h>

#include <Windows.h>
#include <wincrypt.h>
#define KEYLENGTH  CRYPT_CREATE_SALT

#define ENCRYPT_ALGORITHM CALG_RC4 


#define MD5_HASH_KEY_STRING						"Protect_No_666_HevensDoor"



#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuMD5EnCryptD" )
#else
#pragma comment ( lib , "AuMD5EnCrypt" )
#endif
#endif

class AuMD5Encrypt
{
private:
	HCRYPTPROV		m_hCryptProv; 

	bool Initialize();
	bool CleanUp();

public:
	AuMD5Encrypt();
	~AuMD5Encrypt();
	bool EncryptString( char *pstrHashString, char *pstrData, unsigned long istrSize );
	bool DecryptString( char *pstrHashString, char *pstrData, unsigned long istrSize );
	char *GetRandomHashString( int iStringSize );
};

#endif