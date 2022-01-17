#ifndef _AGCMADMIN_H_
#define _AGCMADMIN_H_


#include "AgcModule.h"
#include "AgcmCharacter.h"
#include "AlefAdminAPI.h"
#include "agpmadmin.h"


class AgcmAdmin : public AgcModule
{
public:
	AgcmAdmin();
	virtual ~AgcmAdmin();

	// Virtual Functions
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	static BOOL CBIsDialogMessage(PVOID pMSG, PVOID pClass);

private :
	HMODULE											m_hDLL;
	AgpmAdmin*										m_pagpmAdmin;
	AgcmCharacter*									m_pagcmCharacter;

public :
	BOOL SendAdminClientRequest();
	BOOL				StartAdminClient			( void );

	CHAR*				GetUIMessageString			( CHAR* pKeyString );
	ADMIN_CB			GetDLLFunction				( CHAR* pFunctionName );

	HMODULE				GetDLLModuleHandle			( void ) { return m_hDLL; }
};

#endif