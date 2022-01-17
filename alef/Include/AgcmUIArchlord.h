#pragma once

#include "AgcModule.h"

class AgcmUIManager2;
class AgpmArchlord;
class AgcmArchlord;
class AgcmChatting2;

class AgcmUIArchlord : public AgcModule
{
private:
	AgcmUIManager2*										m_pcsAgcmUIManager2;
	AgpmArchlord*										m_pcsAgpmArchlord;
	AgcmArchlord*										m_pcsAgcmArchlord;
	AgcmChatting2*										m_pcsAgcmChatting2;

public:
	AgcmUIArchlord( void );
	virtual ~AgcmUIArchlord( void );

	// virtual~
public :
	virtual BOOL 				OnAddModule				( void );
	virtual BOOL 				OnInit					( void ) { return TRUE; }
	virtual BOOL 				OnDestroy				( void ) { return TRUE; }
	virtual BOOL 				OnIdle					( UINT32 ulClockCount ) { return TRUE; }

	// Method
public :
	BOOL 						SetTextEffect			( CHAR *pszString );
	BOOL 						AddSystemMessage		( CHAR* szMsg );

	// CallBack
public :
	static BOOL 				CBGuardInfo				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBSetArchlord			( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBCancelArchlord		( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBSetGuard				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBCancelGuard			( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBCurrentStep			( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBMessageId				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBGrant					( PVOID pData, PVOID pClass, PVOID pCustData );
};
