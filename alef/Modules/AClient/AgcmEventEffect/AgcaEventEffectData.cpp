#include "AgcaEventEffectData.h"

AgcaEventEffectData::AgcaEventEffectData()
{
	m_lMax		= 0;
	m_lCount	= 0;
}

AgcaEventEffectData::~AgcaEventEffectData()
{
}

BOOL AgcaEventEffectData::Initialize(INT32 lMaxDataNum)
{
	return InitializeObject(sizeof(AgcdUseEffectSetData), m_lMax = lMaxDataNum);
}

AgcdUseEffectSetData *AgcaEventEffectData::CreateData()
{
	if(m_lCount >= m_lMax)
		return NULL;

	AgcdUseEffectSetData stData;
	memset(&stData, 0, sizeof(AgcdUseEffectSetData));

	stData.m_fScale		= 1.0f;
	stData.m_lCustData	= -1;

	return (AgcdUseEffectSetData *)(AddObject((PVOID)(&stData), m_lCount++));
}

BOOL AgcaEventEffectData::RemoveAll()
{
	m_lCount	= 0;
	m_lMax		= 0;

	return RemoveObjectAll();
}