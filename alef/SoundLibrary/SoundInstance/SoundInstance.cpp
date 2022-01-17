#include "SoundInstance.h"
#include "SoundResource/SoundResource.h"

//-----------------------------------------------------------------------
//

SoundInstance::SoundInstance( void )
	: resource_(0)
	, channel_(NULL)
	, isPlay_(false)
	, inst_idx_(0)
	, isFadeIn_(false)
	, isFadeOut_(false)
{}

//-----------------------------------------------------------------------
//

SoundInstance::~SoundInstance( void )
{
	Destroy();
}

//-----------------------------------------------------------------------
//

bool SoundInstance::Create( SoundResource* resource )
{
	if( resource == NULL ) return false;

	Destroy();

	resource_ = resource;

	if( !resource_->PlaySound( channel_ ) )
		return false;

	//channel_->getIndex( &inst_idx_ );
	// 0은 무효값으로 쓴다.
	//inst_idx_ += 1;

	static int _idx = 1;

	inst_idx_ = _idx++;

	if( _idx == INT_MAX  )
		_idx = 1;

	SetVolume( resource_->GetDefaultVolume() );

	if( resource_->Is3DSound() )
	{
		SetActiveDistanceMinMax( resource_->GetDefaultDistMin(), resource_->GetDefaultDistMax() );
	}

	isPlay_ = false;

	return true;
}

//-----------------------------------------------------------------------
//

void SoundInstance::Destroy( void )
{
	resource_ = 0;

	if( channel_ != NULL )
	{
		channel_->stop();
		channel_ = NULL;
	}
}

//-----------------------------------------------------------------------
//

bool SoundInstance::Update( float fTimeElapsed, SoundNode* pListenerNode )
{
	if( isFadeIn_ )
	{
		float fCurrVolume = GetVolume();

		if( fCurrVolume >= fadeInTargetVolume_ || fCurrVolume >= 1 )
		{
			isFadeIn_ = false;
			fCurrVolume = fadeInTargetVolume_;
			timeFadeInDuration_ = 0.0f;

			if( channel_ ) 
				channel_->setVolume( fCurrVolume );
		}
		else
		{
			if( timeFadeInDuration_ <= 0 )
				fCurrVolume = timeFadeInDuration_;
			else
				fCurrVolume += fTimeElapsed / timeFadeInDuration_;

			if( fCurrVolume > timeFadeInDuration_ )
				fCurrVolume = timeFadeInDuration_;

			if( channel_ ) 
				channel_->setVolume( fCurrVolume );
		}
	}

	if( isFadeOut_ )
	{
		float fCurrVolume = GetVolume();

		if( fCurrVolume <= fadeOutTargetVolume_ || fCurrVolume <= 0 )
		{
			isFadeOut_ = false;
			fCurrVolume = 0.0f;
			timeFadeOutDuration_ = 0.0f;
			Stop();
			return false;
		}
		else
		{
			if( timeFadeOutDuration_ <= 0 )
				fCurrVolume = 0;
			else
				fCurrVolume -= fTimeElapsed / ( float )timeFadeOutDuration_;

			if( fCurrVolume < fadeOutTargetVolume_ )
				fCurrVolume = fadeOutTargetVolume_;

			if( channel_ ) 
				channel_->setVolume( fCurrVolume );
		}
	}

	return true;
}

//-----------------------------------------------------------------------
//

bool SoundInstance::PostUpdate( void )
{
	if( channel_ != NULL )
	{
		bool bIsPlay = false;

		channel_->isPlaying( &bIsPlay );

		if( bIsPlay == false )
		{
			Stop();
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------
//

void SoundInstance::Pause( void )
{
	if( channel_ != NULL )
	{
		isPlay_ = false;
		channel_->setPaused( true );
	}
}

//-----------------------------------------------------------------------
//

void SoundInstance::Mute( void )
{
	if( channel_ != NULL )
	{
		channel_->setMute( true );
	}
}

//-----------------------------------------------------------------------
//

void SoundInstance::Play( void )
{
	if( channel_ == NULL ) 
		return;

	if( channel_ && !isPlay_ )
	{
		channel_->setPaused( false );
		isPlay_ = true;
	}
}

//-----------------------------------------------------------------------
//

void SoundInstance::Stop( void )
{
	if( channel_ != NULL )
	{		
		isPlay_ = false;
		isFadeIn_ = false;
		isFadeOut_ = false;
		channel_->stop();
	}
}

//-----------------------------------------------------------------------
//

void SoundInstance::FadeIn( const float fDuration, const float fTargetVolume )
{
	SetVolume( 0.0f );
	isFadeIn_ = true;
	fadeInTargetVolume_ = fTargetVolume;
	timeFadeInDuration_ = fDuration;
}

//-----------------------------------------------------------------------
//

void SoundInstance::FadeOut( const float fDuration, const float fTargetVolume )
{
	isFadeOut_ = true;
	fadeOutTargetVolume_ = fTargetVolume;
	timeFadeOutDuration_ = fDuration;
}

//-----------------------------------------------------------------------
//

bool SoundInstance::IsPlaying( void )
{
	return isPlay_;
}

//-----------------------------------------------------------------------
//

float SoundInstance::GetVolume()
{
	float fVolume = 0.0f;

	if( channel_ != NULL )
	{
		channel_->getVolume( &fVolume );
	}

	return fVolume;
}

//-----------------------------------------------------------------------
//

void SoundInstance::SetVolume( const float fVolume )
{
	if( channel_ != NULL )
	{
		if( !isFadeIn_ && !isFadeOut_ )
			channel_->setVolume( fVolume );
	}
}

//-----------------------------------------------------------------------
//

int SoundInstance::GetLoopCount()
{
	int nLoopCount = 0;

	if( channel_ != NULL )
	{
		channel_->getLoopCount( &nLoopCount );
	}

	return nLoopCount;
}

//-----------------------------------------------------------------------
//

void SoundInstance::SetLoopCount( const int nLoopCount )
{
	if( channel_ != NULL )
	{
		channel_->setLoopCount( nLoopCount );
	}
}

//-----------------------------------------------------------------------
//

bool SoundInstance::Is3DSound()
{
	return resource_ ? resource_->Is3DSound() : false;
}

//-----------------------------------------------------------------------
//

const char * SoundInstance::GetFileName( void )
{
	return resource_ ? resource_->GetFileName() : 0;
}

//-----------------------------------------------------------------------
//

SOUND_TYPE SoundInstance::GetType()
{
	return resource_ ? resource_->GetType() : SOUND_TYPE_INVALID ;
}

//-----------------------------------------------------------------------