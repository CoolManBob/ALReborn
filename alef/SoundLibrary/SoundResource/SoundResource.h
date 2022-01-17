#ifndef __CLASS_SOUND_RESOURCE_H__
#define __CLASS_SOUND_RESOURCE_H__

#include "DefineSoundMacro.h"

//-----------------------------------------------------------------------
//

class SoundResource
{
public :

	FMOD::System*											m_pSystem;
	FMOD::Sound*											m_pSound;

	char													filename_[ SOUND_STRING_LENGTH_FILENAME ];
	SOUND_TYPE												type_;
	float													defaultVolume_;
	float													defaultDistanceMin_;
	float													defaultDistanceMax_;

public :
	SoundResource( void );
	virtual ~SoundResource( void );

	bool						Create						( FMOD::System* pSystem, char* pFileName, SOUND_TYPE eType, const float fDefaultVolume, const float fDefaultDistanceMin, const float fDefaultDistanceMax );
	bool						Create						( FMOD::System* pSystem, char* pPathFileName, char* pFileName, int nFileOffset, int nFileSize, SOUND_TYPE eType, const float fDefaultVolume, const float fDefaultDistanceMin, const float fDefaultDistanceMax );
	bool						Create						( FMOD::System* pSystem, char* pBuffer, char* pFileName, int nFileSize, SOUND_TYPE eType, const float fDefaultVolume, const float fDefaultDistanceMin, const float fDefaultDistanceMax );
	void						Destroy						( void );

	FMOD::System*				GetSystem					( void ) { return m_pSystem; }
	FMOD::Sound*				GetSound					( void ) { return m_pSound; }
	char*						GetFileName					( void ) { return filename_; }
	SOUND_TYPE					GetType						( void ) { return type_; }

	bool						PlaySound					( FMOD::Channel *& channel );

	float						GetDefaultVolume 			() const { return defaultVolume_; }
	float						GetDefaultDistMin			() const { return defaultDistanceMin_; }
	float						GetDefaultDistMax			() const { return defaultDistanceMax_; }

	float						GetLength					( void );
	bool						Is3DSound					( void );
};

//-----------------------------------------------------------------------

#endif