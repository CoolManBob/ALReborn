// AgcdEffSound.h: interface for the AgcdEffSound class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCDEFFSOUND_H__D317680C_1DBE_4E73_A5D6_8ECEEEB50C99__INCLUDED_)
#define AFX_AGCDEFFSOUND_H__D317680C_1DBE_4E73_A5D6_8ECEEEB50C99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffBase.h"
#include "AgcuEff2ApMemoryLog.h"

//#############################################################################
// class		: AgcdEffSound
// desc			: 
// update		: 20040527 by kday
//				- 최초 작업.
//#############################################################################
class AgcdEffSound : public AgcdEffBase, public ApMemory<AgcdEffSound, 100>
{
	EFFMEMORYLOG_SMV;

public:
	enum eEffSoundType
	{
		EFFECT_SOUND_NONE		= 0	,
		EFFECT_SOUND_3DSOUND		,
		EFFECT_STREAM_SOUND			,
		EFFECT_SAMPLE_SOUND			,
		EFFECT_BGM_SOUND			,

		EFFECT_SOUND_NUM			,
	};

#ifdef USE_MFC
public:
#else
private:
#endif//USE_MFC
	eEffSoundType		m_eSoundType;
	RwChar				m_szSoundFName[EFF2_FILE_NAME_MAX];
	RwChar				m_szMonoFName[EFF2_FILE_NAME_MAX];
	RwUInt32			m_dwLoopCnt;
	RwReal				m_fVolume;

	RwUInt32			m_ulSoundLen;

	
private:
	//do not use!
	AgcdEffSound( const AgcdEffSound& cpy ) : AgcdEffBase(cpy) {cpy;};
	AgcdEffSound& operator = ( const AgcdEffSound& cpy ) { cpy; return *this; };
public:
	explicit AgcdEffSound(eEffSoundType eSoundType = EFFECT_SOUND_3DSOUND);
	virtual ~AgcdEffSound();

	RwInt32			bStopSound			( eEffSoundType soundType, RwUInt32 dwIndex );
	RwUInt32		bPlaySound			( eEffSoundType soundType, RwReal fVolume, RwUInt32 dwLoopCnt, BOOL bClump , PVOID pClumpParent=NULL );
	RwInt32			bPosUpdateFor3DSound( RwUInt32 dwIndex, const RwV3d* pV3dPos , INT32		iWeight );

	//access
	eEffSoundType	bGetSoundType		( VOID ) const { return m_eSoundType; };
	const RwChar*	bGetSoundFName		( VOID ) const { return m_szSoundFName; };
	const RwChar*	bGetMonoFName		( VOID ) const { return m_szMonoFName; };
	RwUInt32		bGetLoopCnt			( VOID ) const { return m_dwLoopCnt; };
	RwReal			bGetVolume			( VOID ) const { return m_fVolume; };
	RwUInt32		bGetSoundLength		( VOID ) const { return m_ulSoundLen; };

	//set
	void			bSetSoundType		( eEffSoundType eSoundType )	{ m_eSoundType = eSoundType; };
	void			bSetLoopCnt			( RwUInt32 dwLoopCnt )			{ m_dwLoopCnt = dwLoopCnt; };
	void			bSetVolume			( RwReal fVolume )				{ m_fVolume = fVolume; };
	RwInt32			bSetSoundName		( const RwChar* szSound );
	RwInt32			bSetMonoFName		( const RwChar* szMono );
	
	//file in out
	RwInt32			bToFile				( FILE* fp );
	RwInt32			bFromFile			( FILE* fp );
	
	//for tool
#ifdef USE_MFC
	virtual 
	INT32		bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC
};

#endif