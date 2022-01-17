// AuDynCode.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.06.08.

//
// Wrapper for DynCode of China
//

#ifdef _AREA_CHINA_

#ifndef _AUDYNCODE_H_
#define _AUDYNCODE_H_

#include "AuCryptAlgorithmBase.h"

#include "SDCltDynCode.h"
#include "SDSvrDynCode.h"

class DYNCODE_CTX : public BASE_CTX
{
public:
	int nCodeIndex;
	SGDP::ISDCltDynCode* m_pClientDynCode;

	void init();
	DYNCODE_CTX() { m_pClientDynCode = NULL; init(); }
	~DYNCODE_CTX() { init(); }
};

class AuDynCode : public AuCryptAlgorithmBase
{
private:
	SGDP::ISDSvrDynCode* m_pServerDynCode;

	BOOL	bServer;
	char*	GetRootPath();

public:
	AuDynCode();
	virtual ~AuDynCode();

	BOOL	InitServer();
	void	Cleanup();

	void	Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone = FALSE);
	void	Initialize(void* pctx, void* pctxSource);
	DWORD	Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);
	void	Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);

	DWORD	GetOutputSize(DWORD lInputSize);

	// Specific for AuDynCode
	INT32	GetClientDynCode(void* pctx, const BYTE** ppCode);
};

#endif//_AUDYNCODE_H_

#endif//_AREA_CHINA_