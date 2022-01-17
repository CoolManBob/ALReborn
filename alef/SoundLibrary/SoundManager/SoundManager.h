#ifndef __CLASS_SOUNDMANAGER_H__
#define __CLASS_SOUNDMANAGER_H__

#include "DefineSoundMacro.h"
#include <string>
#include <map>
#include <list>
#include <set>

class SoundResource;
class SoundInstance;

//-----------------------------------------------------------------------
//

struct SoundResourceEntry
{
	SoundResourceEntry() : refCount_(0), resource_(0) {}

	void set( SoundResource * resource ) { 
		if( resource_ != resource ) 
		{
			del();
			resource_ = resource;
			refCount_ = 1;
		}
	}

	SoundResource * get() { return resource_; }

	void incRef() { ++refCount_; }
	void decRef() { --refCount_; if( refCount_ < 1 ) del(); }

private:
	void del();

	int refCount_;
	SoundResource * resource_;
};

//-----------------------------------------------------------------------
//

class istring // 대소문자 구별없이 비교하는 문자열 형식
{
public:
	istring() {}
	istring( char const * s ) : str_( s ? s : "" ) {}
	istring( istring const & other ) : str_(other.str_) {}

	istring & operator=( istring const &other ) {
		if( this != &other )
			str_ = other.str_;
		return *this;
	}

	bool operator<( istring const & other ) const{
		if( str_.empty() )
			return true;

		if( other->empty() )
			return false;

		return _stricmp( str_.c_str(), other->c_str() ) < 0;
	}

	bool operator==( istring const & other ) const{
		if( str_.empty() && other->empty() )
			return true;

		if( str_.empty() || other->empty() )
			return false;

		return _stricmp( str_.c_str(), other->c_str() ) == 0;
	}

	inline std::string * operator->() {return &str_;}
	inline std::string const * operator->() const {return &str_;}

private:
	std::string str_;
};

//-----------------------------------------------------------------------
//

typedef BOOL (*SoundLoadFunc)( char* pFileName, SOUND_TYPE eType );

//-----------------------------------------------------------------------
//

class SoundManager
{
private :
	FMOD::System*											m_pSystem;

	typedef std::map< istring, SoundResourceEntry >			SoundResources;
	typedef std::map< SOUND_TYPE, SoundResources >			SoundResourceMap;
	typedef std::map< int, SoundInstance* >					SoundInstances;
	typedef std::set< int >									SoundInstanceIndices;
	typedef std::set< int >								ReservedDeleteData;

	ReservedDeleteData										m_listReservedDeleteData;
	SoundResourceMap										m_mapSoundResource;
	SoundInstances											m_mapSoundInstances;
	SoundInstanceIndices									m_mapEnvironmentSounds;

	char													m_strCurrBGM[ SOUND_STRING_LENGTH_FILENAME ];
	int														currBGMIdx_;

	SoundNode*												m_pListenerNode;
	SoundVector												m_vPrevListenerPos;

	SoundLoadFunc											m_pfnSoundLoad;

public :
	virtual ~SoundManager( void );

	static SoundManager &		Inst						( void );

	void						_DeleteAllSoundInstance		( void );
	void						_DeleteAllSoundResource		( void );

	void						SetLoader					( SoundLoadFunc func );

	void						Update						( SoundNode* pListenerNode, float fTimeElapsed );
	void						UpdateListenerPostion		( SoundNode* pListenerNode, float fTimeElapsed );

public :
	int 						Play						( char* pFileName, SOUND_TYPE soundType, SoundNode* pSceneNode, int loopCnt, float volume, float minDist, float maxDist, BOOL onlyPos = FALSE );

	void 						Stop						( int nChannelIndex );

	void 						FadeIn						( int nChannelIndex, const float fDuration = 5.0f, const float fTargetVolume = 1.0f );
	void 						FadeOut						( int nChannelIndex, const float fDuration = 5.0f, const float fTargetVolume = 0.0f );

	void 						StopAllSound				( void );

	int 						ChangeBGM					( char* pFileName, float volume, const float fDuration = 5.0f );

	int							AddEnvironmentSound			( char* pFileName, float volume, const float fDuration = 5.0f );
	void						RemoveEnvironmentSound		( int nChannelIndex, const float fDuration = 5.0f );
	int							FindEnvironmentSound		( char* pFileName );
	void						SetEnvSoundVolume			( float fVolume );
	void						ClearEnvSound				( void );

	void 						ReleaseSceneNode			( SoundNode* pSceneNode );
	void 						ReleaseSceneNodeByIndex		( int nIndex );
	void						ReleaseAllSceneNode			( void );

	void						SetSoundPreset				( const FMOD_REVERB_PROPERTIES* pProperty );
	void						SetSpeakerMode				( FMOD_SPEAKERMODE eMode );

	BOOL						IsPlaying					( int nIndex );

	int							GetDeviceCount				( void );
	BOOL						GetDeviceInfo				( int nIndex, char* pDeviceName, int nBufferSize, FMOD_GUID* pDeviceID );
	BOOL						UseDevice					( int nIndex );

	float						GetVolume					( int nChannelIndex, SoundNode* sceneNode );
	void						SetVolume					( int nChannelIndex, const float fVolume = 1.0f );

	void						SetLoopCount				( int instIdx, SoundNode* pSceneNode = NULL, const int nLoopCount = 1 );

	SoundNode*					GetCurrentListenerNode		( void ) { return m_pListenerNode; }
	SoundVector					GetCurrentListenerPos		( void );

	SoundInstance*				FindInstance				( const int nChannelIndex );
	void						ReserveDelete				( const int nChannelIndex );
	bool						DeleteReserved				( void );
	bool						DeleteInstance				( const int nChannelIndex );
	SoundResourceEntry*			FindResource				( SOUND_TYPE type, const char* pFileName );

	BOOL 						LoadFile					( char* pFileName, SOUND_TYPE eType );
	BOOL						LoadStream					( char* pPathFileName, char* pFileName, int nFileOffset, int nFileSize, SOUND_TYPE eType );
	BOOL						LoadBuffer					( char* pBuffer, char* pFileName, int nFileSize, SOUND_TYPE eType );

	void						Delete3D					();
	void						Delete2D					();

	void						Change3DVolume				( float volumeDelta );
	void						Change2DVolume				( float volumeDelta );

	void 						UnInitialize				( void );

private :

	SoundManager( void );

	void						_NoticeSoundResource		( void );

	BOOL 						UnLoad						( SOUND_TYPE type, char* pFileName );
	BOOL 						UnLoad						( SoundResource* pResource );

	int							CreateInstance				( char* pFileName, SOUND_TYPE soundType, SoundNode* pTargetNode = NULL, int* pChannelIndex = NULL , BOOL bOnlyPos = TRUE );
	bool						DeleteInstance				( SoundInstances::iterator iter );

	bool 						Initialize					( void );

	static BOOL					DefaultLoad					( char * pFileName, SOUND_TYPE eType );
};

//-----------------------------------------------------------------------

#endif