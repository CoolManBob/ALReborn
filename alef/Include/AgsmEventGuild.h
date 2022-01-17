// AgsmEventGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 13.

#include "AgpmEventGuild.h"

#include "AgsmCharacter.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgsmEventGuildD.lib")
#else
	#pragma comment(lib, "AgsmEventGuild.lib")
#endif
#endif

class AgsmEventGuild : public AgsModule
{
private:
	AgpmEventGuild* m_pagpmEventGuild;
	
	AgsmCharacter* m_pagsmCharacter;
	
public:
	AgsmEventGuild();
	virtual ~AgsmEventGuild();
	
	BOOL OnAddModule();
	
	static BOOL CBGuildEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildEventRequestWarehouse(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildEventRequestWorldChampionship(PVOID pData, PVOID pClass, PVOID pCustData);
};