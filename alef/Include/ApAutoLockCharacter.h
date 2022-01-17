#ifndef	_APAUTOLOCKCHARACTER_H_
#define _APAUTOLOCKCHARACTER_H_

#include "AgpdCharacter.h"
#include "AgpmCharacter.h"

class ApAutoLockCharacter
{
public:
	ApAutoLockCharacter(AgpmCharacter *pcsAgpmCharacter, INT32 lCID) : m_pcsAgpdCharacter( NULL )
	{
		m_pcsAgpdCharacter = pcsAgpmCharacter->GetCharacterLock( lCID );
	}

	ApAutoLockCharacter(AgpmCharacter *pcsAgpmCharacter, CHAR *szGameID) : m_pcsAgpdCharacter( NULL )
	{
		m_pcsAgpdCharacter = pcsAgpmCharacter->GetCharacterLock( szGameID );
	}
	~ApAutoLockCharacter()
	{
		if (m_pcsAgpdCharacter) m_pcsAgpdCharacter->m_Mutex.Release();
	}
	inline AgpdCharacter *GetCharacterLock()
	{
		return m_pcsAgpdCharacter;
	}
private:
	AgpdCharacter	*m_pcsAgpdCharacter;
};

#endif