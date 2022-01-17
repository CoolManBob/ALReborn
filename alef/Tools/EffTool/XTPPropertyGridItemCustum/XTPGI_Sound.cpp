// XTPGI_Sound.cpp: implementation of the CXTPGI_Sound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_Sound.h"

#include "AgcuEffPath.h"
#include "AgcdEffSound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPGI_Sound::CXTPGI_Sound(CString strCaption, AgcdEffSound* pSound)
	: CXTPGI_EffBase(strCaption, pSound)
	, m_pSound(pSound)

	, m_pItemType(NULL)
	, m_pItemSoundFile(NULL)
	, m_pItemMonoFile(NULL)
	, m_pItemLoopCnt(NULL)
	, m_pItemVolume(NULL)
{
	ASSERT(m_pSound);
}
CXTPGI_Sound::~CXTPGI_Sound()
{
}
void CXTPGI_Sound::OnAddChildItem()
{
	CXTPGI_EffBase::OnAddChildItem();

	ASSERT(m_pSound);

	static LPCTSTR	strType[AgcdEffSound::EFFECT_SOUND_NUM] = { "NONE", "3D", "STREAM", "SAMPLE", "BGM", };
	m_pItemType = (CXTPGI_Enum<AgcdEffSound::eEffSoundType>*)AddChildItem(new CXTPGI_Enum<AgcdEffSound::eEffSoundType>(_T("type"), AgcdEffSound::EFFECT_SOUND_NUM, m_pSound->m_eSoundType, &m_pSound->m_eSoundType, strType) );

	LPCTSTR	FILTER = "WAV파일(*.wav, *.WAV) |*.wav;*.WAV|MP3파일(*.mp3, *.MP3)|*.mp3;*.MP3||";
	m_pItemSoundFile = (CXTPGI_FileName*)AddChildItem(new CXTPGI_FileName(_T("sound"), EFF2_FILE_NAME_MAX, m_pSound->m_szSoundFName, m_pSound->m_szSoundFName, AgcuEffPath::bGetPath_Sound(), FILTER ));
	m_pItemSoundFile = (CXTPGI_FileName*)AddChildItem(new CXTPGI_FileName(_T("mono"), EFF2_FILE_NAME_MAX, m_pSound->m_szMonoFName, m_pSound->m_szMonoFName, AgcuEffPath::bGetPath_SoundMono(), FILTER ));
	m_pItemLoopCnt = (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("loop"), &m_pSound->m_dwLoopCnt, m_pSound->m_dwLoopCnt));
	m_pItemVolume = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("volume"), &m_pSound->m_fVolume, m_pSound->m_fVolume));
	
	//flags
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pSound->m_dwBitFlags, FLAG_EFFBASESOUND_STOPATEFFECTEND	,_T("stop at effect end" )));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pSound->m_dwBitFlags, FLAG_EFFBASESOUND_3DTOSAMEPLE		,_T("3d sound to sample" )));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pSound->m_dwBitFlags, FLAG_EFFBASESOUND_NOFRUSTUMCHK		,_T("ambience sound")));
};
