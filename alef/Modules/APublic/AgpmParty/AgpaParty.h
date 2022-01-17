/******************************************************************************
Module:  AgpaParty.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 26
******************************************************************************/

#if !defined(__AGPAPARTY_H__)
#define __AGPAPARTY_H__

#include "ApBase.h"
#include "ApAdmin.h"
#include "AgpdParty.h"
#include "AgpdCharacter.h"

//	AgpaParty class
//		- party data를 관리한다.
///////////////////////////////////////////////////////////////////////////////
class AgpaParty : public ApAdmin {
public:
	AgpaParty();
	~AgpaParty();

	AgpdParty* AddParty(AgpdParty* pcsParty);
	BOOL RemoveParty(INT32 lPID);
	AgpdParty* GetParty(INT32 lPID);
};

#endif //__AGPAPARTY_H__
