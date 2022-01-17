/******************************************************************************
Module:  AgpaParty.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 02. 07
******************************************************************************/

#include "AgpaParty.h"

AgpaParty::AgpaParty()
{
}

AgpaParty::~AgpaParty()
{
}

//		AddParty
//	Functions
//		- add party data
//	Arguments
//		- pcsParty
//	Return value
//		- AgpdParty * : Ãß°¡µÈ party data pointer
///////////////////////////////////////////////////////////////////////////////
AgpdParty* AgpaParty::AddParty(AgpdParty* pcsParty)
{
	if (!AddObject((PVOID) &pcsParty, pcsParty->m_lID))
		return NULL;

	return pcsParty;
}

//		RemoveParty
//	Functions
//		- remove party data
//	Arguments
//		- lPID : party id
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL AgpaParty::RemoveParty(INT32 lPID)
{
	return RemoveObject(lPID);
}

//		GetParty
//	Functions
//		- get party data
//	Arguments
//		- lPID : party id
//	Return value
//		- AgpdParty * : party data pointer
//		- NULL : fail operation
///////////////////////////////////////////////////////////////////////////////
AgpdParty* AgpaParty::GetParty(INT32 lPID)
{
	AgpdParty **ppcsParty = (AgpdParty **) GetObject(lPID);

	if (!ppcsParty)
		return NULL;	

	return *ppcsParty;
}
