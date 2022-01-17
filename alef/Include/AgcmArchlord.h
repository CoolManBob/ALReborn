#pragma once

#include "AgcModule.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

class AgpmArchlord;

class AgcmArchlord : public AgcModule 
{
private:
	AgpmArchlord *m_pcsAgpmArchlord;
	AgpmCharacter *m_pcsAgpmCharacter;
	AgcmCharacter *m_pcsAgcmCharacter;
	AgcmEventManager *m_pcsAgcmEventManager;

public:
	AgcmArchlord();
	virtual ~AgcmArchlord();

//	virtual BOOL OnInit();
	virtual BOOL OnAddModule();
//	virtual BOOL OnIdle(UINT32 ulClockCount);	
//	virtual BOOL OnDestroy();

	static BOOL CBOperationSetArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBOperationCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL OperationSetArchlord(char* szID);
	BOOL SendRequestEvent(AgpdCharacter *pcsCharacter, ApdEvent *pApdEvent);
};