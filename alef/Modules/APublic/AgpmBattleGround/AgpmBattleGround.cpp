#include "AgpmBattleGround.h"
#include "AgppBattleGround.h"
#include "AgpmCharacter.h"

//************************************
// Method:    AgpmBattleGround
// FullName:  AgpmBattleGround::AgpmBattleGround
// Access:    public 
// Returns:   
// Qualifier:
//************************************
AgpmBattleGround::AgpmBattleGround()
	: m_pagpmCharacter(NULL)
{
	SetModuleName("AgpmBattleGround");
}

AgpmBattleGround::~AgpmBattleGround()
{

}

BOOL	AgpmBattleGround::OnInit()
{
	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	
	return TRUE;
}

BOOL AgpmBattleGround::SetCallbackSystemMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return TRUE;
}

//************************************
// Method:    IsInBattleGround
// FullName:  AgpmBattleGround::IsInBattleGround
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: AgpdCharacter * pcsCharacter
//************************************
BOOL AgpmBattleGround::IsInBattleGround(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;
		
	return IsBattleGround(pcsCharacter->m_nBindingRegionIndex);
}

BOOL AgpmBattleGround::IsBattleGround(INT16 nBindingRegionIndex)
{
	switch(nBindingRegionIndex)
	{
		case REGIONINDEX_BATTLEGROUND_MOONELF_FILED:
		case REGIONINDEX_BATTLEGROUND_HUMAN_FILED:
		case REGIONINDEX_BATTLEGROUND_ORC_FILED:
		case REGIONINDEX_BATTLEGROUND_DRAGONSCION_FILED:
		case REGIONINDEX_BATTLEGROUND_FILED:
		case REGIONINDEX_BATTLEGROUND_DRAGONSCION_CAMP:
		case REGIONINDEX_BATTLEGROUND_MOONELF_CAMP:
		case REGIONINDEX_BATTLEGROUND_ORC_CAMP:
		case REGIONINDEX_BATTLEGROUND_HUMAN_CAMP:
		case REGIONINDEX_BATTLEGROUND_CENTER_OF_CHAOS:
			return TRUE;
	}

	return FALSE;
}

//************************************
// Method:    IsAttackable
// FullName:  AgpmBattleGround::IsAttackable
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: AgpdCharacter * pcsCharacter
// Parameter: AgpdCharacter * pcsTarget
//************************************
BOOL AgpmBattleGround::IsAttackable(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget) 
		return FALSE;

	if( !IsInBattleGround(pcsCharacter) || !IsInBattleGround(pcsTarget))
		return FALSE;

	// 같은 종족이면 공격 불가
	if( m_pagpmCharacter->IsSameRace(pcsCharacter, pcsTarget) == TRUE )
		return FALSE;

	return TRUE;
}
