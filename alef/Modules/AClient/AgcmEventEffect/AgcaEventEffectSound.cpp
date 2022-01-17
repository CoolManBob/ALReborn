#include "AgcaEventEffectSound.h"

#include "ApMemoryTracker.h"

AgcaEventEffectSound::AgcaEventEffectSound()
{
}

AgcaEventEffectSound::~AgcaEventEffectSound()
{
}

BOOL AgcaEventEffectSound::Initialize(INT32 lMaxDataNum)
{
	return InitializeObject(sizeof(AgcdEventEffectSound), lMaxDataNum);
}

AgcdEventEffectSound *AgcaEventEffectSound::AddSound(CHAR *szName)
{
	AgcdEventEffectSound	stSound;
	memset( &stSound, 0, sizeof( AgcdEventEffectSound ) );
	strcpy( stSound.m_szSoundName, szName );

	return (AgcdEventEffectSound *)(AddObject((PVOID)(&stSound), szName));
}

AgcdEventEffectSound *AgcaEventEffectSound::GetSound(CHAR *szName, BOOL bAdd)
{
	AgcdEventEffectSound* pstSound = (AgcdEventEffectSound *)(GetObject(szName));
	if( !pstSound && bAdd )
		pstSound = AddSound(szName);

	return pstSound;
}

BOOL AgcaEventEffectSound::RemoveSound(CHAR *szName)
{
	return RemoveObject(szName);
}

BOOL AgcaEventEffectSound::RemoveAll()
{
	return RemoveObjectAll();
}