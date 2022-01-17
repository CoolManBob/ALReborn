#ifndef __AGCM_SOUND_H__
#define __AGCM_SOUND_H__

#include "AgcModule.h"
#include "DefineSoundMacro.h"
#include "ContainerUtil.h"
#include <map>

#define __block_copy_obejct__( type, message ) \
	type( type const & other ) { throw message; } \
	void operator=( type const & other ) { throw message; }


#ifdef _DEBUG
	#pragma comment( lib, "SoundLibraryD.lib"  )
#else
	#pragma comment( lib, "SoundLibrary.lib" )
#endif


#define	AGCMSOUND_SOUND_FILENAME_LENGTH								128
#define	AGCMSOUND_SOUND_DEVICENAME_LENGTH							64
#define AGCMSOUND_SOUND_DEVICECOUNT_MAX								10

#define	AGCMSOUND_SOUND_FILEPATH									"Sound\\"
#define	AGCMSOUND_SOUND_FILEPATH_BGM								"Sound\\BGM\\"
#define	AGCMSOUND_SOUND_FILEPATH_EFFECT								"Sound\\Effect\\"
#define	AGCMSOUND_SOUND_FILEPATH_STREAM								"Sound\\Stream\\"
#define	AGCMSOUND_SOUND_FILEPATH_UI									"Sound\\UI\\"

#define	AGCMSOUND_3D_SOUND_OVER_RANGE								0xFFFFFFFF
#define	AGCMSOUND_3D_SOUND_NO_SLOT									0xFFFFFFFE

#define SOUND_PACK_LENGTH_PATH										32
#define SOUND_PACK_LENGTH_PACKFILENAME								7
#define SOUND_PACK_LENGTH_PREFIX									4
#define SOUND_PACK_LENGTH_FILENAME									12

enum eSoundType
{
	SoundType_UnKnown = 0,
	SoundType_BGM,
	SoundType_Effect,
	SoundType_Environment,
	SoundType_UI,
};

enum eSoundSpeakerMode
{
	SoundSpeakerMode_Default = 0,
	SoundSpeakerMode_HeadPhone,
	SoundSpeakerMode_Surround,
	SoundSpeakerMode_Channel4,
	SoundSpeakerMode_Channel51,
	SoundSpeakerMode_Channel71,
};

enum enumSoundJobQueueType
{
	SOUND_JOB_SAMPLE_END = 0,
	SOUND_JOB_3D_END,
	SOUND_JOB_STREAM_END,
	SOUND_JOB_REMOVED
};

enum eSoundEffectRoomType
{
	SoundEffectRoomType_Generic = 0,
	SoundEffectRoomType_PaddedCell,
	SoundEffectRoomType_Room,
	SoundEffectRoomType_RoomBath,
	SoundEffectRoomType_RoomLiving,
	SoundEffectRoomType_RoomStone,
	SoundEffectRoomType_Auditorium,
	SoundEffectRoomType_ConcertHall,
	SoundEffectRoomType_Cave,
	SoundEffectRoomType_Arena,
	SoundEffectRoomType_Hangar,
	SoundEffectRoomType_CarpetedHallway,
	SoundEffectRoomType_Hallway,
	SoundEffectRoomType_StoneCorridor,
	SoundEffectRoomType_Alley,
	SoundEffectRoomType_Forest,
	SoundEffectRoomType_City,
	SoundEffectRoomType_Mountains,
	SoundEffectRoomType_Quarry,
	SoundEffectRoomType_Plain,
	SoundEffectRoomType_ParkingLot,
	SoundEffectRoomType_SewerPipe,
	SoundEffectRoomType_UnderWater,
	SoundEffectRoomType_Drugged,
	SoundEffectRoomType_Dizzy,
	SoundEffectRoomType_Psychotic,
	SoundEffectRoomType_MaxCount,
};

struct stSoundPresetEntry
{
	eSoundEffectRoomType											m_eRoomType;
	FMOD_REVERB_PROPERTIES											m_Property;

	stSoundPresetEntry( void )
	{
		m_eRoomType = SoundEffectRoomType_Generic;
	}
};

struct stSoundDeviceEntry
{
	FMOD_GUID														m_stGUID;
	int																m_nIndex;
	char															m_strDeviceName[ AGCMSOUND_SOUND_DEVICENAME_LENGTH ];

	stSoundDeviceEntry( void )
	{
		memset( &m_stGUID, 0, sizeof( FMOD_GUID ) );
		m_nIndex = -1;
		memset( m_strDeviceName, 0, sizeof( char ) * AGCMSOUND_SOUND_DEVICENAME_LENGTH );
	}
};

struct SoundPackFileEntry;

class AgcmSound : public AgcModule
{
private :
	BOOL															m_bIsReadFromPack;
	BOOL															m_bUseRoom;

	float															m_fMasterVolumeBGM;
	float															m_fMasterVolumeBGMPrev;
	BOOL															m_bIsMuteBGM;

	float															m_fMasterVolumeStream;
	float															m_fMasterVolumeStreamPrev;
	BOOL															m_bIsMuteStream;

	float															m_fMasterVolumeSample;
	float															m_fMasterVolumeSamplePrev;
	BOOL															m_bIsMuteSample;

	float															m_fMasterVolume3DSample;
	float															m_fMasterVolume3DSamplePrev;
	BOOL															m_bIsMute3DSample;

	eSoundEffectRoomType											m_eSoundEffectType;

	ContainerMap< eSoundEffectRoomType, stSoundPresetEntry >		m_mapPreset;
	ContainerMap< std::string, stSoundDeviceEntry >					m_mapDevice;

	int																m_nCurrentDeviceIndex;
	int																m_nCurrentBGM;

	typedef std::map< std::string, SoundPackFileEntry* >			PackFiles;

	PackFiles														packFiles_;

	unsigned int													m_nPrevTime;

public :
	// AgcmUIOption 에서 UserData 로 직접 끌어다 쓰는 코드가 있어서 이전 코드와의 호환을 위해 만듬.. ㅡ.ㅡ;;
	stSoundDeviceEntry												m_stDevice[ AGCMSOUND_SOUND_DEVICECOUNT_MAX ];

public :
	AgcmSound( void );
	~AgcmSound( void );

public :
	virtual BOOL				OnAddModule							( void );
	virtual BOOL				OnInit								( void );
	virtual BOOL				OnDestroy							( void );	// 처리순서때문에.. 플래그값 변경이랑 볼륨값 변경의 작업순서가 반대가 된다..
	virtual BOOL				OnIdle								( unsigned int nTime );

public :
	BOOL						OnUpdate3DSoundPosition				( int nIndex, const SoundVector* pVector, int nWeight );
	
	void						OnSelectDevice						( char* pDeviceName );
	void						OnSelectDevice						( int nIndex );

public :
	void						CloseAll3DSample					( void );
	void						CloseAllSample						( void );

	void						SetSpeaker							( eSoundSpeakerMode eMode );
	void						SetRoom								( eSoundEffectRoomType eType );
	void						SetUseRoom							( BOOL bIs ) { m_bUseRoom = bIs; }

	int							GetDeviceCount						( void ) { return m_mapDevice.GetSize(); }
	stSoundDeviceEntry*			GetDeviceInfo						( int nIndex );
	stSoundDeviceEntry*			GetDeviceInfo						( char* pDeviceName );

	char*						GetCurrentDeviceName				( void );
	int							GetCurrentDeviceIndex				( void ) { return m_nCurrentDeviceIndex; }

	float						GetBGMVolume						( void ) { return m_fMasterVolumeBGM; }
	float						GetStreamVolume						( void ) { return m_fMasterVolumeStream; }
	float						GetSampleVolume						( void ) { return m_fMasterVolumeSample; }
	float						Get3DSampleVolume					( void ) { return m_fMasterVolume3DSample; }

	void						SetStreamVolume						( float fVolume );
	void						SetStreamVolumePrev					( float fVolume ) { m_fMasterVolumeStreamPrev = fVolume; }

	void						SetBGMVolume						( float fVolume );
	void						SetBGMVolumePrev					( float fVolume ) { m_fMasterVolumeBGMPrev = fVolume; }

	void						SetSampleVolume						( float fVolume );
	void						SetSampleVolumePrev					( float fVolume ) { m_fMasterVolumeSamplePrev = fVolume; }

	void 						Set3DSampleVolume					( float fVolume );
	void 						Set3DSampleVolumePrev				( float fVolume ) { m_fMasterVolume3DSamplePrev = fVolume; }

	void						MuteSound							( eSoundType eType );
	void						UnMuteSound							( eSoundType eType );

	void						SetReadFromPack						( BOOL bIs ) { m_bIsReadFromPack = bIs; }

	BOOL 						PreStop3DSound						( void* pClump );
	BOOL 						PreStop3DSound						( int nChannelIndex );

	unsigned int				PlayBGM								( char* pFile, int nLoopCount = -1, BOOL bUseFade = TRUE, float fVolume = 1.0f, int nPriority = 0 );
	BOOL						StopBGM								( BOOL bUseFadeOut, INT32 nPriority = 0 );

	unsigned int				PlayStream							( char* pFile, int nLoopCount = -1, float fVolume = 1.0f, BOOL bUseFade = TRUE );
	BOOL						StopStream							( unsigned int nIndex, BOOL bUseFadeOut = TRUE );
	BOOL						ClearStream							( void );

	unsigned int				Play3DSound							( char* pFile, int nLoopCount = 1, float fVolume = 1.0f , BOOL bSetClump = FALSE, void* pClumpFrame = NULL, float fPlaySpeed = 1.0f , BOOL bOnlyPos = FALSE );
	BOOL						Stop3DSound							( unsigned int nIndex );

	unsigned int				PlaySampleSound						( char* pFile, int nLoopCount = 1, float fVolume = 1.0f, float fPlaySpeed = 1.0f );
	BOOL						StopSampleSound						( unsigned int nIndex );

	void 						JobQueueAdd							( enumSoundJobQueueType eJobType, unsigned int nKey );
	void 						JobQueueRemove						( unsigned int nKey );

	unsigned int 				GetStreamSoundLength				( const char* pFile );
	unsigned int 				GetSampleSoundLength				( SOUND_TYPE soundType, const char* pFile );

	const char*					GetCurrBGMFileName					( void );

	BOOL 						IsIn3DSoundRange					( RwV3d* pPos );
	BOOL						IsSoundMuted						( eSoundType eType );

	int							GetCurrentBGMChannelIdx				()						{ return m_nCurrentBGM; }

private :
	static BOOL					LoadSound							( char * pFilename, SOUND_TYPE eType );

	BOOL 						_LoadPackingFile					( char* pFileName );
	BOOL 						_LoadPackingSound					( char* pFileName, SOUND_TYPE eType );

	void 						_GetDeviceInformation				( void );

	FMOD_REVERB_PROPERTIES*		_GetPreset							( eSoundEffectRoomType eType );
	void						_AddPreset							( eSoundEffectRoomType eType, FMOD_REVERB_PROPERTIES* pProperty );
};

#endif