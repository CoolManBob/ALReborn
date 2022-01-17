#pragma once

#include "DefineSoundMacro.h"

class SoundResource;

//-----------------------------------------------------------------------
//

class SoundInstance
{
protected :
	SoundResource *											resource_;
	FMOD::Channel*											channel_;
	bool													isPlay_;
	int														inst_idx_;

	bool													isFadeIn_;
	float													fadeInTargetVolume_;
	float													timeFadeInDuration_;

	bool													isFadeOut_;
	float													fadeOutTargetVolume_;
	float													timeFadeOutDuration_;

public :
	SoundInstance( void );
	virtual ~SoundInstance( void );

	bool						Create						( SoundResource* pResource );
	void						Destroy						( void );

	virtual bool				Update						( float fTimeElapsed, SoundNode* pListenerNode = NULL );
	virtual bool				PostUpdate					( void );

	FMOD::Channel*				GetChannel					( void ) { return channel_; }

	void 						Pause						( void );
	void 						Mute						( void );
	void 						Play						( void );
	void 						Stop						( void );
	void 						FadeIn						( const float fDuration = 5.0f, const float fTargetVolume = 1.0f );
	void 						FadeOut						( const float fDuration = 5.0f, const float fTargetVolume = 0.0f );

	bool						IsPlaying					( void );

	float						GetVolume					( void );
	void						SetVolume					( const float fVolume );
	int							GetLoopCount				( void );
	void						SetLoopCount				( const int nLoopCount );

	virtual float				GetActiveDistanceMin		( void ) { return 0; }
	virtual float				GetActiveDistanceMax		( void ) { return 0; }
	virtual void				SetActiveDistanceMinMax		( const float fDistanceMin, const float fDistanceMax ) {}

	int							GetIndex					( void ) { return inst_idx_; }

	bool						Is3DSound					();
	const char *				GetFileName					( void );
	SOUND_TYPE					GetType						();
};

//-----------------------------------------------------------------------