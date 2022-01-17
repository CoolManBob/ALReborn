#include "AgpmEpicZone.h"
#include "AgpmCharacter.h"

AgpmEpicZone::AgpmEpicZone()
	: m_pcsAgpmCharacter(NULL)
{
	SetModuleName("AgpmEpicZone");
}

AgpmEpicZone::~AgpmEpicZone()
{

}

BOOL AgpmEpicZone::OnInit()
{
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");

	return TRUE;
}

BOOL AgpmEpicZone::IsEpicZone(INT16 nBindingRegionindex)
{
	switch(nBindingRegionindex)
	{
		case REGIONINDEX_EPICZONE_HELL_OF_FLAME:
		case REGIONINDEX_EPICZONE_VEIN_OF_LIFE:
		case REGIONINDEX_EPICZONE_MOUNTAINS_BLADESTORM:
		case REGIONINDEX_EPICZONE_HEAVEN_HELL:
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmEpicZone::IsInEpicZone(AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	return IsEpicZone(pcsCharacter->m_nBindingRegionIndex);
}