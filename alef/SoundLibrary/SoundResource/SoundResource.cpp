#include "SoundResource.h"

//-----------------------------------------------------------------------
//

SoundResource::SoundResource( void ) : m_pSystem( NULL ), m_pSound( NULL )
{
	memset( filename_, 0, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME );
}

//-----------------------------------------------------------------------
//

SoundResource::~SoundResource( void )
{
	Destroy();
}

//-----------------------------------------------------------------------
//

bool SoundResource::Create( FMOD::System* pSystem, char* pFileName, SOUND_TYPE eType, const float fDefaultVolume, const float fDefaultDistanceMin, const float fDefaultDistanceMax )
{
	if( !pSystem || !pFileName ) return false;
	m_pSystem = pSystem;

	FMOD_RESULT Result;
	FMOD_MODE nMode = 0;

	switch( eType )
	{
	case SOUND_TYPE_2D_SOUND :
		{
			nMode = FMOD_DEFAULT;
			Result = m_pSystem->createStream( pFileName, nMode, 0, &m_pSound );
		}
		break;

	case SOUND_TYPE_2D_SOUND_LOOPED :
		{
			nMode = FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE;
			Result = m_pSystem->createStream( pFileName, nMode, 0, &m_pSound );
		}
		break;

	case SOUND_TYPE_3D_SOUND :
		{
			nMode = FMOD_3D | FMOD_HARDWARE | FMOD_3D_LINEARROLLOFF;
			Result = m_pSystem->createSound( pFileName, nMode, 0, &m_pSound );
		}
		break;

	case SOUND_TYPE_3D_SOUND_LOOPED :
		{
			nMode = FMOD_LOOP_NORMAL | FMOD_3D | FMOD_HARDWARE | FMOD_3D_LINEARROLLOFF;
			Result = m_pSystem->createSound( pFileName, nMode, 0, &m_pSound );
		}
		break;

	default :
		{
			return false;
		}
		break;
	}

	if( Result != FMOD_OK )	return false;

	memset( filename_, 0, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME );
	strcpy_s( filename_, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME, pFileName );

	type_ = eType;

	defaultVolume_ = fDefaultVolume;
	defaultDistanceMin_ = fDefaultDistanceMin;
	defaultDistanceMax_ = fDefaultDistanceMax;
	return true;
}

//-----------------------------------------------------------------------
//

bool SoundResource::Create( FMOD::System* pSystem, char* pPathFileName, char* pFileName, int nFileOffset, int nFileSize, SOUND_TYPE eType, const float fDefaultVolume, const float fDefaultDistanceMin, const float fDefaultDistanceMax )
{
	if( !pSystem || !pPathFileName || !pFileName ) return false;
	m_pSystem = pSystem;

	FMOD_CREATESOUNDEXINFO SoundInfo;
	memset( &SoundInfo, 0, sizeof( FMOD_CREATESOUNDEXINFO ) );

	SoundInfo.cbsize = sizeof( FMOD_CREATESOUNDEXINFO );
	SoundInfo.length = nFileSize;
	SoundInfo.fileoffset = nFileOffset;

	FMOD_RESULT Result;
	FMOD_MODE nMode = 0;

	switch( eType )
	{
	case SOUND_TYPE_2D_SOUND :
		nMode = FMOD_2D | FMOD_HARDWARE | FMOD_CREATESTREAM;
		Result = m_pSystem->createStream( pPathFileName, nMode, &SoundInfo, &m_pSound );
		break;

	case SOUND_TYPE_2D_SOUND_LOOPED :
		nMode = FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE | FMOD_CREATESTREAM;
		Result = m_pSystem->createStream( pPathFileName, nMode, &SoundInfo, &m_pSound );
		break;

	case SOUND_TYPE_3D_SOUND :
		nMode = FMOD_3D | FMOD_HARDWARE | FMOD_3D_LINEARROLLOFF;
		Result = m_pSystem->createSound( pPathFileName, nMode, &SoundInfo, &m_pSound );
		break;

	case SOUND_TYPE_3D_SOUND_LOOPED :
		nMode = FMOD_LOOP_NORMAL | FMOD_3D | FMOD_HARDWARE | FMOD_3D_LINEARROLLOFF;
		Result = m_pSystem->createSound( pPathFileName, nMode, &SoundInfo, &m_pSound );
		break;

	default :
		return false;
	}	

	if( Result != FMOD_OK )	return false;

	memset( filename_, 0, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME );
	strcpy_s( filename_, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME, pFileName );

	type_ = eType;

	defaultVolume_ = fDefaultVolume;
	defaultDistanceMin_ = fDefaultDistanceMin;
	defaultDistanceMax_ = fDefaultDistanceMax;
	return true;
}

//-----------------------------------------------------------------------
//

bool SoundResource::Create( FMOD::System* pSystem, char* pBuffer, char* pFileName, int nFileSize, SOUND_TYPE eType, const float fDefaultVolume, const float fDefaultDistanceMin, const float fDefaultDistanceMax )
{
	if( !pSystem || !pBuffer || !pFileName ) return false;
	m_pSystem = pSystem;

	FMOD_CREATESOUNDEXINFO SoundInfo;
	memset( &SoundInfo, 0, sizeof( FMOD_CREATESOUNDEXINFO ) );

	SoundInfo.cbsize = sizeof( FMOD_CREATESOUNDEXINFO );
	SoundInfo.length = nFileSize;

	FMOD_RESULT Result;
	FMOD_MODE nMode = 0;

	switch( eType )
	{
	case SOUND_TYPE_2D_SOUND : 			nMode = FMOD_OPENMEMORY | FMOD_CREATESAMPLE;																		break;
	case SOUND_TYPE_2D_SOUND_LOOPED :	nMode = FMOD_OPENMEMORY | FMOD_CREATESAMPLE | FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE;							break;
	case SOUND_TYPE_3D_SOUND :			nMode = FMOD_OPENMEMORY | FMOD_CREATESAMPLE | FMOD_3D | FMOD_HARDWARE | FMOD_3D_LINEARROLLOFF;						break;
	case SOUND_TYPE_3D_SOUND_LOOPED :	nMode = FMOD_OPENMEMORY | FMOD_CREATESAMPLE | FMOD_LOOP_NORMAL | FMOD_3D | FMOD_HARDWARE | FMOD_3D_LINEARROLLOFF;	break;
	default :							return false;																										break;
	}

	Result = m_pSystem->createSound( pBuffer, nMode, &SoundInfo, &m_pSound );
	if( Result != FMOD_OK )	return false;

	memset( filename_, 0, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME );
	strcpy_s( filename_, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME, pFileName );

	type_ = eType;

	defaultVolume_ = fDefaultVolume;
	defaultDistanceMin_ = fDefaultDistanceMin;
	defaultDistanceMax_ = fDefaultDistanceMax;
	return true;
}

//-----------------------------------------------------------------------
//

void SoundResource::Destroy()
{
	if( m_pSound != NULL )
	{
		m_pSound->release();
		m_pSound = NULL;
	}

	m_pSystem = NULL;
	memset( filename_, 0, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME );
}

//-----------------------------------------------------------------------
//

float SoundResource::GetLength( void )
{
	unsigned int nLength = 0;

	if( m_pSound != NULL )
	{
		m_pSound->getLength( &nLength, FMOD_TIMEUNIT_MS );
	}

	return ( float )nLength / 1000.0f;
}

//-----------------------------------------------------------------------
//

bool SoundResource::Is3DSound( void )
{
	switch( type_ )
	{
	case SOUND_TYPE_3D_SOUND :
	case SOUND_TYPE_3D_SOUND_LOOPED :
		{
			return true;
		}
		break;
	}

	return false;
}

//-----------------------------------------------------------------------
//

bool SoundResource::PlaySound( FMOD::Channel *& channel )
{
	FMOD_RESULT result = FMOD_ERR_INVALID_ADDRESS;
	if( channel == 0 )
		result = m_pSystem->playSound( FMOD_CHANNEL_FREE, m_pSound, true, &channel );
	return result != FMOD_OK ? false : true;
}

//-----------------------------------------------------------------------