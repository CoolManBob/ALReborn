#ifndef __AGCA_EVENT_EFFECT_DATA_H__
#define __AGCA_EVENT_EFFECT_DATA_H__

#include "ApAdmin.h"
#include "AgcdEffect.h"

class AgcaEventEffectData : public ApAdmin
{
public:
	AgcaEventEffectData();
	virtual ~AgcaEventEffectData();

protected:
	INT32					m_lMax;
	INT32					m_lCount;

public:
	BOOL					Initialize(INT32 lMaxDataNum);

	AgcdUseEffectSetData	*CreateData();
	BOOL					RemoveAll();

};

#endif // __AGCA_EVENT_EFFECT_DATA_H__