#ifndef _AGPM_BATTLE_GROUND_H
#define _AGPM_BATTLE_GROUND_H

#include "ApModule.h"

enum eRegionIndex
{
	REGIONINDEX_BATTLEGROUND_DRAGONSCION_FILED	= 175,
	REGIONINDEX_BATTLEGROUND_MOONELF_FILED		= 125,
	REGIONINDEX_BATTLEGROUND_HUMAN_FILED		= 124,
	REGIONINDEX_BATTLEGROUND_ORC_FILED			= 123,
	REGIONINDEX_BATTLEGROUND_FILED				= 114,
	REGIONINDEX_BATTLEGROUND_DRAGONSCION_CAMP	= 176,
	REGIONINDEX_BATTLEGROUND_MOONELF_CAMP		= 113,
	REGIONINDEX_BATTLEGROUND_ORC_CAMP			= 112,
	REGIONINDEX_BATTLEGROUND_HUMAN_CAMP			= 111,
	REGIONINDEX_BATTLEGROUND_CENTER_OF_CHAOS	= 110,
};

enum eBattleGroundSystemMessage
{

};

class AgpdCharacter;
class AgpmCharacter;

class AgpmBattleGround : public ApModule
{
private:
	AgpmCharacter* m_pagpmCharacter;

public:
	AgpmBattleGround();
	virtual ~AgpmBattleGround();
	
	BOOL	OnInit();

	//set callback
	BOOL SetCallbackSystemMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass);

public:
	BOOL IsAttackable(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL IsInBattleGround(AgpdCharacter* pcsCharacter);
	BOOL IsBattleGround(INT16 nBindingRegionIndex);
};

#endif // _AGPM_BATTLE_GROUND_H
