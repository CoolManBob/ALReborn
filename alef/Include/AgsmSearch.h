// AgsmSearch.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.06.27.

#ifndef _AGSMSEARCH_H_
#define _AGSMSEARCH_H_

#include "AgsEngine.h"

#include "AgpmCharacter.h"
#include "AgpmSearch.h"

class AgsmSearch : public AgsModule
{
private:
	AgpmCharacter* m_pcsAgpmCharacter;
	AgpmSearch* m_pcsAgpmSearch;

public:
	AgsmSearch();
	virtual ~AgsmSearch();

	BOOL OnAddModule();

	BOOL SetCharacterData(AgpdSearch& stSearch, AgpdCharacter* pcsCharacter);

	BOOL SendResult(AgpdSearch& stSearch, UINT32 ulNID, INT8 cOperation = AGPMSEARCH_PACKET_RESULT);

	static BOOL CBSearchName(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSearchTID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSearchLevel(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSearchTIDLevel(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif//_AGSMSEARCH_H_