#include "AgcmUISkillDragonScion.h"

#include "AgcmCharacter.h"
#include "AgcmUIManager2.h"
#include "AgpmEventSKillMaster.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgcmSkill.h"
#include "AgcmEventSkillMaster.h"
#include "AgcmUICharacter.h"
#include "AgcmUIMain.h"
#include "AgcmUIItem.h"
#include "AgpmSkill.h"


AgcmUIManager2*	AgcmUISkillDragonScion::GetUIManager( VOID )
{
	return m_pcsAgcmUIManager2;
}

AgcmCharacter*	AgcmUISkillDragonScion::GetAgcmCharacter( VOID )
{
	return m_pcsAgcmCharacter;
}

AgpmSkill*	AgcmUISkillDragonScion::GetAgpmSkill( VOID )
{
	return m_pcsAgpmSkill;
}

AgpmGrid*	AgcmUISkillDragonScion::GetAgpmGrid( VOID )
{
	return m_pcsAgpmGrid;
}

AgpmCharacter*	AgcmUISkillDragonScion::GetAgpmCharacter( VOID )
{
	return m_pcsAgpmCharacter;
}