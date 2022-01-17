#ifndef __AGCA_EVENT_EFFECT_SOUND_H__
#define __AGCA_EVENT_EFFECT_SOUND_H__

#include "ApAdmin.h"

#define AGCD_EES_STR_MAX	32

struct AgcdEventEffectSound
{
	CHAR	m_szSoundName[AGCD_EES_STR_MAX];
};

#define AGCA_EES_STR_MAX	256

class AgcaEventEffectSound : public ApAdmin
{
public:
	AgcaEventEffectSound();
	virtual	~AgcaEventEffectSound();

public:
	BOOL					Initialize(INT32 lMaxDataNum);

	AgcdEventEffectSound*	AddSound(CHAR *szName);
	AgcdEventEffectSound*	GetSound(CHAR *szName, BOOL bAdd = FALSE);

	BOOL					RemoveSound(CHAR *szName);
	BOOL					RemoveAll();
};

#endif // __AGCA_EVENT_EFFECT_SOUND_H__