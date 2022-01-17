// AuDynCode.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.06.08.

#ifdef _AREA_CHINA_

#ifdef _DEBUG
	#ifdef _DLL
		#pragma comment(lib, "SDCltDynCodeMDd.lib")
	#else
		#pragma comment(lib, "SDCltDynCodeMTd.lib")
	#endif
#else
	#ifdef _DLL
		#pragma comment(lib, "SDCltDynCodeMD.lib")
	#else
		#pragma comment(lib, "SDCltDynCodeMT.lib")
	#endif
#endif

#include "AuDynCode.h"
#include <windows.h>
#include "SDCltDynCode.h"
#include "SDSvrDynCode.h"

const CHAR* CLIENTBIN_PATH = "\\DynCodeBin\\Client32";

using namespace SGDP;

void DYNCODE_CTX::init()
{
	eAlgorithm = AUCRYPT_ALGORITHM_DYNCODE;
	nCodeIndex = 0;
	if(m_pClientDynCode)
		m_pClientDynCode->Release();
	m_pClientDynCode = NULL;
}

AuDynCode::AuDynCode()
{
	m_pServerDynCode = NULL;
	bServer = FALSE;
}

AuDynCode::~AuDynCode()
{
	Cleanup();
}

// 원래 DynCode 에 있던 거 가져옴.
// Static 이어도 괜찮은 건. 한번하고 나면 그담부턴 read 전용이다.
char* AuDynCode::GetRootPath()
{
    static char szPath[MAX_PATH];
    static bool bFirstTime = true;

    if(bFirstTime)
    {
        bFirstTime = false;
        GetModuleFileName(NULL, szPath, sizeof(szPath));
        char *p = strrchr(szPath, '\\');
        *p = '\0';
    }

    return szPath;
}

BOOL AuDynCode::InitServer()
{
	return TRUE;
}

void AuDynCode::Cleanup()
{
}

void AuDynCode::Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone)
{
	if(!pctx)
		return;

	DYNCODE_CTX& ctx = *static_cast<DYNCODE_CTX*>(pctx);

	const BYTE* pCode = NULL;
	{
		// Client 인 경우는 넘어온 key, lKeySize 가 서버에서 온 pCode 이다.
		if(ctx.m_pClientDynCode == NULL)
			ctx.m_pClientDynCode = SDCreateCltDynCode();

		if(ctx.m_pClientDynCode == NULL)
			return;

		ctx.m_pClientDynCode->SetDynCode(key, lKeySize);
	}
}

void AuDynCode::Initialize(void* pctx, void* pctxSource)
{
	if(!pctx || !pctxSource)
		return;

	DYNCODE_CTX& ctx = *static_cast<DYNCODE_CTX*>(pctx);
	DYNCODE_CTX& ctxSource = *static_cast<DYNCODE_CTX*>(pctxSource);

	if(bServer)
	{
		// 걍 memcpy 면 될듯?
		memcpy(&ctx, &ctxSource, sizeof(ctx));
	}
}

// Encrypt 후 pOutput 은 pInput 과 같고, lSize 도 그대로이다.
DWORD AuDynCode::Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || lSize < 1)
		return 0;

	DYNCODE_CTX& ctx = *static_cast<DYNCODE_CTX*>(pctx);
	{
		if(!ctx.m_pClientDynCode)
			return 0;

		if(ctx.m_pClientDynCode->Encode(pInput, lSize))
		{
			memcpy(pOutput, pInput, lSize);
			return lSize;
		}
		else
			return 0;
	}
}

// Decrypt 후 pOutput 은 pInput 과 같고, lSize 도 그대로이다.
void AuDynCode::Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || lSize < 1)
		return;

	DYNCODE_CTX& ctx = *static_cast<DYNCODE_CTX*>(pctx);
	{
		if(!ctx.m_pClientDynCode)
			return;

		if(ctx.m_pClientDynCode->Decode(pInput, lSize))
			memcpy(pOutput, pInput, lSize);
	}
}

// Output 이 같다.
DWORD AuDynCode::GetOutputSize(DWORD lInputSize)
{
	return lInputSize;
}

// AuCryptAlgorithmBase 에서 상속받은 게 아닌 DynCode 에 특화된 함수
INT32 AuDynCode::GetClientDynCode(void* pctx, const BYTE** ppCode)
{
	if(!pctx)
		return 0;

	DYNCODE_CTX& ctx = *static_cast<DYNCODE_CTX*>(pctx);

	return 0;
}

#endif//_AREA_CHINA_