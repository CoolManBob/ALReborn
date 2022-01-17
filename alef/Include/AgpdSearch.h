// AgpdSearch.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006. 06. 27.

#ifndef _AGPDSEARCH_H_
#define _AGPDSEARCH_H_

#include "AgpdCharacter.h"

struct AgpdSearch
{
	CHAR m_szName[AGPDCHARACTER_NAME_LENGTH+1];

	INT32 m_lTID;

	INT32 m_lMinLevel;	
	INT32 m_lMaxLevel;

	AuPOS m_stPos;
};

#endif//_AGPDSEARCH_H_