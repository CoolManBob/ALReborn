// AgsmStartupEncryption.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.11.

#ifndef _AGSMSTARTUPENCRYPTION_H_
#define _AGSMSTARTUPENCRYPTION_H_

#include "AgsEngine.h"
#include "AgpmStartupEncryption.h"
#include "AgsmAOIFilter.h"

#include "AgpmConfig.h"

class AgsmStartupEncryption : public AgsModule
{
private:
	AgpmStartupEncryption* m_pcsAgpmStartupEncryption;

	AgsmAOIFilter* m_pcsAgsmAOIFilter;

	AgpmConfig* m_pcsAgpmConfig;

	BYTE* m_pPublicKey;
	BYTE* m_pEncryptedPublicKey;

	INT32 m_lPublicKeySize;
	INT32 m_lEncryptedKeySize;

	AuCryptActor m_csStartupActor;

public:
	AgsmStartupEncryption();
	virtual ~AgsmStartupEncryption();

	BOOL OnAddModule();

	BOOL SetAlgorithm();
	BOOL InitializePublicKey();
	BOOL InitializePublicKey(BYTE* pKey, INT32 lSize);
	
	BOOL InitializePublicKeyAtDynCode();				// DynCode 는 완전 다르게 작업한다.
	BOOL InitializePrivateKeyAtDynCode(UINT32 ulNID);	// DynCode 는 완전 다르게 작업한다.

	BOOL SendAlgorithmType(UINT32 ulNID);
	BOOL SendEncryptedPublicKey(UINT32 ulNID);
	BOOL SendComplete(UINT32 ulNID);

	BOOL SendDynCodePublic(UINT32 ulNID);
	BOOL SendDynCodePrivate(UINT32 ulNID);

	static BOOL CBRequestPublic(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBMakePrivate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCheckComplete(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif//_AGSMSTARTUPENCRYPTION_H_