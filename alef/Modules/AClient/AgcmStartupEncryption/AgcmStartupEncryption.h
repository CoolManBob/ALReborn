// AghmStartupEncryption.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.11.

#ifndef _AGCMSTARTUPENCRYPTION_H_
#define _AGCMSTARTUPENCRYPTION_H_

#include "AgpmStartupEncryption.h"

#include "AgcModule.h"
#include "AgcmLogin.h"
#include "AgcmCharacter.h"
#include "AgcmReturnToLogin.h"
#include "AgcmConnectManager.h"

#include "AuRandomNumber.h"

//const int AGCMSTARTUPENCRYPTION_MAX_RANDOM_LENGTH		= 8;	// 8 의 배수 단위로 하자.

class AgcmStartupEncryption : public AgcModule
{
private:
	AgpmStartupEncryption*	m_pcsAgpmStartupEncryption;
	AgcmLogin*				m_pcsAgcmLogin;
	AgcmReturnToLogin*		m_pcsAgcmReturnToLogin;
	AgcmConnectManager*		m_pcsAgcmConnectManager;
	AgcmCharacter*			m_pcsAgcmCharacter;

	MTRand			m_csRandom;

public:
	AgcmStartupEncryption();
	virtual ~AgcmStartupEncryption();

	BOOL OnAddModule();

	BOOL SendRequestPublic(INT16 nNID);
	BOOL SendEncryptedRandom(BYTE* pKey, INT32 nSize, INT16 nNID);

	static BOOL CBAlgorithmType(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBPublic(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBComplete(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBDynCodePublic(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDynCodePrivate(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBConnectLoginServer(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBConnectGameServer(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReconnectLoginServer(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif//_AGCMSTARTUPENCRYPTION_H_