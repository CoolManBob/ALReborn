#include "AgcmSound.h"
#include "AgcmCharacter.h"
#include "AgcuEffPath.h"
#include "AgcmUIOption.h"
#include "../SoundManager/SoundManager.h"
#include "../SoundResource/SoundResource.h"
#include "../SoundInstance/Sound3D.h"
#include "../SoundInstance/Sound2D.h"


#define SOUND_3D_DISTANCE_MIN										0.0f
#define SOUND_3D_DISTANCE_MAX										8000.0f
//#define SOUND_3D_DISTANCE_LIMIT										36000000.0f

#define SOUND_FADE_DURATION											5.0f

//-----------------------------------------------------------------------
//

struct SoundPackFileEntry
{
	int																fileSize_;
	int																fileOffset_;
	unsigned int													soundLength_;
	char															packFileName_[ 256 ];

	SoundPackFileEntry( void )
		: fileSize_(0)
		, fileOffset_(0)
		, soundLength_(0)
	{
		memset( packFileName_, 0, sizeof(packFileName_) );
	}
};

//-----------------------------------------------------------------------
//

AgcmSound::AgcmSound( void )
{
	SetModuleName( "AgcmSound" );
	EnableIdle( TRUE );

	m_bIsReadFromPack = TRUE;
	m_bUseRoom = FALSE;

	m_fMasterVolumeBGM = 1.0f;
	m_fMasterVolumeBGMPrev = 1.0f;
	
	m_fMasterVolumeStream = 1.0f;
	m_fMasterVolumeStreamPrev = 1.0f;

	m_fMasterVolumeSample = 1.0f;
	m_fMasterVolumeSamplePrev = 1.0f;

	m_fMasterVolume3DSample = 1.0f;
	m_fMasterVolume3DSamplePrev = 1.0f;

	m_eSoundEffectType = SoundEffectRoomType_Generic;

	m_nCurrentDeviceIndex = 0;

	m_nCurrentBGM = 0;

	m_bIsMuteBGM = FALSE;
	m_bIsMuteStream = FALSE;
	m_bIsMuteSample = FALSE;
	m_bIsMute3DSample = FALSE;

	m_nPrevTime = 0;
}

AgcmSound::~AgcmSound( void )
{
	for( PackFiles::iterator iter = packFiles_.begin(); iter != packFiles_.end(); ++iter )
	{
		if(iter->second)
			delete iter->second;
	}
}

BOOL AgcmSound::OnAddModule( void )
{
	_GetDeviceInformation();

	FMOD_REVERB_PROPERTIES stProperty01 = FMOD_PRESET_GENERIC;
	FMOD_REVERB_PROPERTIES stProperty02 = FMOD_PRESET_PADDEDCELL;
	FMOD_REVERB_PROPERTIES stProperty03 = FMOD_PRESET_ROOM;
	FMOD_REVERB_PROPERTIES stProperty04 = FMOD_PRESET_BATHROOM;
	FMOD_REVERB_PROPERTIES stProperty05 = FMOD_PRESET_LIVINGROOM;
	FMOD_REVERB_PROPERTIES stProperty06 = FMOD_PRESET_STONEROOM;
	FMOD_REVERB_PROPERTIES stProperty07 = FMOD_PRESET_AUDITORIUM;
	FMOD_REVERB_PROPERTIES stProperty08 = FMOD_PRESET_CONCERTHALL;
	FMOD_REVERB_PROPERTIES stProperty09 = FMOD_PRESET_CAVE;
	FMOD_REVERB_PROPERTIES stProperty10 = FMOD_PRESET_ARENA;
	FMOD_REVERB_PROPERTIES stProperty11 = FMOD_PRESET_HANGAR;
	FMOD_REVERB_PROPERTIES stProperty12 = FMOD_PRESET_CARPETTEDHALLWAY;
	FMOD_REVERB_PROPERTIES stProperty13 = FMOD_PRESET_HALLWAY;
	FMOD_REVERB_PROPERTIES stProperty14 = FMOD_PRESET_STONECORRIDOR;
	FMOD_REVERB_PROPERTIES stProperty15 = FMOD_PRESET_ALLEY;
	FMOD_REVERB_PROPERTIES stProperty16 = FMOD_PRESET_FOREST;
	FMOD_REVERB_PROPERTIES stProperty17 = FMOD_PRESET_CITY;
	FMOD_REVERB_PROPERTIES stProperty18 = FMOD_PRESET_MOUNTAINS;
	FMOD_REVERB_PROPERTIES stProperty19 = FMOD_PRESET_QUARRY;
	FMOD_REVERB_PROPERTIES stProperty20 = FMOD_PRESET_PLAIN;
	FMOD_REVERB_PROPERTIES stProperty21 = FMOD_PRESET_PARKINGLOT;
	FMOD_REVERB_PROPERTIES stProperty22 = FMOD_PRESET_SEWERPIPE;
	FMOD_REVERB_PROPERTIES stProperty23 = FMOD_PRESET_UNDERWATER;
	FMOD_REVERB_PROPERTIES stProperty24 = FMOD_PRESET_DRUGGED;
	FMOD_REVERB_PROPERTIES stProperty25 = FMOD_PRESET_DIZZY;
	FMOD_REVERB_PROPERTIES stProperty26 = FMOD_PRESET_PSYCHOTIC;

	_AddPreset( SoundEffectRoomType_Generic,			&stProperty01 );
	_AddPreset( SoundEffectRoomType_PaddedCell, 		&stProperty02 );
	_AddPreset( SoundEffectRoomType_Room,				&stProperty03 );
	_AddPreset( SoundEffectRoomType_RoomBath,			&stProperty04 );
	_AddPreset( SoundEffectRoomType_RoomLiving, 		&stProperty05 );
	_AddPreset( SoundEffectRoomType_RoomStone,			&stProperty06 );
	_AddPreset( SoundEffectRoomType_Auditorium, 		&stProperty07 );
	_AddPreset( SoundEffectRoomType_ConcertHall,		&stProperty08 );
	_AddPreset( SoundEffectRoomType_Cave,				&stProperty09 );
	_AddPreset( SoundEffectRoomType_Arena,				&stProperty10 );
	_AddPreset( SoundEffectRoomType_Hangar,				&stProperty11 );
	_AddPreset( SoundEffectRoomType_CarpetedHallway,	&stProperty12 );
	_AddPreset( SoundEffectRoomType_Hallway,			&stProperty13 );
	_AddPreset( SoundEffectRoomType_StoneCorridor,		&stProperty14 );
	_AddPreset( SoundEffectRoomType_Alley,				&stProperty15 );
	_AddPreset( SoundEffectRoomType_Forest,				&stProperty16 );
	_AddPreset( SoundEffectRoomType_City,				&stProperty17 );
	_AddPreset( SoundEffectRoomType_Mountains,			&stProperty18 );
	_AddPreset( SoundEffectRoomType_Quarry,				&stProperty19 );
	_AddPreset( SoundEffectRoomType_Plain,				&stProperty20 );
	_AddPreset( SoundEffectRoomType_ParkingLot,			&stProperty21 );
	_AddPreset( SoundEffectRoomType_SewerPipe,			&stProperty22 );
	_AddPreset( SoundEffectRoomType_UnderWater,			&stProperty23 );
	_AddPreset( SoundEffectRoomType_Drugged,			&stProperty24 );
	_AddPreset( SoundEffectRoomType_Dizzy,				&stProperty25 );
	_AddPreset( SoundEffectRoomType_Psychotic,			&stProperty26 );

	_LoadPackingFile( "Sound\\SoundPak.info" );
	return TRUE;
}

BOOL AgcmSound::OnInit( void )
{
	SoundManager::Inst().SetLoader( &AgcmSound::LoadSound );

	return TRUE;
}

BOOL AgcmSound::OnDestroy( void )
{
	SoundManager::Inst().UnInitialize();
	return TRUE;
}

BOOL AgcmSound::OnIdle( unsigned int nTime )
{
	SoundNode*	pNode		=	NULL;
	AgcmRender*	pcmRender	=	static_cast< AgcmRender* >( g_pEngine->GetModule("AgcmRender") );
	if( !pcmRender )
		return FALSE;

	RwCamera*	pCamera		=	pcmRender->GetCamera();
	pNode	=	RwCameraGetFrame( pCamera );
	
	SetBGMVolume( m_fMasterVolumeBGM );
	SetStreamVolume( m_fMasterVolumeStream );
	SetSampleVolume( m_fMasterVolumeSample );
	Set3DSampleVolume( m_fMasterVolume3DSample );

	if( m_nPrevTime == 0 )
	{
		m_nPrevTime = nTime;
	}

	float fSPF = ( float )( nTime - m_nPrevTime ) * 0.001f;
	SoundManager::Inst().Update( pNode, fSPF );

	m_nPrevTime = nTime;
	return TRUE;
}

BOOL AgcmSound::OnUpdate3DSoundPosition( int nIndex, const SoundVector* pVector, int nWeight )
{
	SoundInstance* pInstance = SoundManager::Inst().FindInstance( nIndex );

	if( !pInstance )
		return FALSE;

	if( pInstance->GetType() != SOUND_TYPE_3D_SOUND && pInstance->GetType() != SOUND_TYPE_3D_SOUND_LOOPED )
		return FALSE;

	Sound3D* p3DSound = reinterpret_cast< Sound3D* >(pInstance);
	p3DSound->UpdatePosition( ( SoundVector* )pVector );
	return TRUE;
}

void AgcmSound::CloseAll3DSample( void )
{
	SoundManager::Inst().Delete3D();
}

void AgcmSound::CloseAllSample( void )
{
	SoundManager::Inst().Delete2D();
}

void AgcmSound::SetSpeaker( eSoundSpeakerMode eMode )
{
	FMOD_SPEAKERMODE eFMODMode = FMOD_SPEAKERMODE_STEREO;
	switch( eMode )
	{
	case SoundSpeakerMode_HeadPhone :	eFMODMode = FMOD_SPEAKERMODE_STEREO;	break;
	case SoundSpeakerMode_Surround :	eFMODMode = FMOD_SPEAKERMODE_SURROUND;	break;
	case SoundSpeakerMode_Channel4 :	eFMODMode = FMOD_SPEAKERMODE_QUAD;		break;
	case SoundSpeakerMode_Channel51 :	eFMODMode = FMOD_SPEAKERMODE_5POINT1;	break;
	case SoundSpeakerMode_Channel71 :	eFMODMode = FMOD_SPEAKERMODE_7POINT1;	break;
	}

	SoundManager::Inst().SetSpeakerMode( eFMODMode );
}

void AgcmSound::SetRoom( eSoundEffectRoomType eType )
{
	m_eSoundEffectType = m_bUseRoom ? eType : SoundEffectRoomType_Generic;

	FMOD_REVERB_PROPERTIES* pProperty = _GetPreset( m_eSoundEffectType );

	if( pProperty )	
		SoundManager::Inst().SetSoundPreset( pProperty );
}

stSoundDeviceEntry* AgcmSound::GetDeviceInfo( int nIndex )
{
	stSoundDeviceEntry * result = 0;
	
	if( nIndex > -1 && nIndex < m_mapDevice.GetSize() )
		result = &m_stDevice[ nIndex ];

	if( !result )
		result = &m_stDevice[ 0 ];

	return result;
}

stSoundDeviceEntry* AgcmSound::GetDeviceInfo( char* pDeviceName )
{
	//if( !pDeviceName || strlen( pDeviceName ) <= 0 ) return NULL;
	stSoundDeviceEntry * result = m_mapDevice.Get( pDeviceName ? pDeviceName : "" );

	if( !result && m_mapDevice.GetSize() > 0 )
	{
		result = &m_stDevice[ 0 ];
	}

	return result;
}

void AgcmSound::OnSelectDevice( char* pDeviceName )
{
	stSoundDeviceEntry* pDevice = GetDeviceInfo( pDeviceName );
	if( !pDevice ) return;

	m_nCurrentDeviceIndex = pDevice->m_nIndex;
	SoundManager::Inst().UseDevice( pDevice->m_nIndex );
}

void AgcmSound::OnSelectDevice( int nIndex )
{
	stSoundDeviceEntry* pDevice = GetDeviceInfo( nIndex );
	if( !pDevice ) return;

	m_nCurrentDeviceIndex = pDevice->m_nIndex;
	SoundManager::Inst().UseDevice( pDevice->m_nIndex );
}

char* AgcmSound::GetCurrentDeviceName( void )
{
	stSoundDeviceEntry* pDevice = GetDeviceInfo( m_nCurrentDeviceIndex );
	if( !pDevice ) return NULL;
	return pDevice->m_strDeviceName;
}

void AgcmSound::SetStreamVolume( float fVolume )
{
	m_fMasterVolumeStreamPrev = m_fMasterVolumeStream;
	m_fMasterVolumeStream = fVolume;

	SoundManager::Inst().SetEnvSoundVolume( m_fMasterVolumeStream );
}

void AgcmSound::SetBGMVolume( float fVolume )
{
	if( fVolume < 0 )
		fVolume = 0.f;

	m_fMasterVolumeBGMPrev = m_fMasterVolumeBGM;
	m_fMasterVolumeBGM = fVolume;

//#ifdef _DEBUG
//	char strDebug[ 256 ] = { 0, };
//	sprintf_s( strDebug, sizeof( char ) * 256, "[ AgcmSound::SetBGMVolume ] Volume = %3.3f\n", fVolume );
//	OutputDebugString( strDebug );
//#endif

	SoundInstance* pInstance = SoundManager::Inst().FindInstance( m_nCurrentBGM );
	if( !pInstance ) return;

	pInstance->SetVolume( fVolume );
}

void AgcmSound::SetSampleVolume( float fVolume )
{
	if( m_fMasterVolumeSample != fVolume )
		SoundManager::Inst().Change2DVolume( fVolume - m_fMasterVolumeSample );

	m_fMasterVolumeSamplePrev = m_fMasterVolumeSample;
	m_fMasterVolumeSample = fVolume;
}

void AgcmSound::Set3DSampleVolume( float fVolume )
{
	if( m_fMasterVolume3DSamplePrev != fVolume )
		SoundManager::Inst().Change3DVolume( fVolume - m_fMasterVolume3DSamplePrev );

	m_fMasterVolume3DSamplePrev = m_fMasterVolume3DSample;
	m_fMasterVolume3DSample = fVolume;
}

void AgcmSound::MuteSound( eSoundType eType )
{
	switch( eType )
	{
	case SoundType_BGM :
		{
			SetBGMVolume( 0.0f );
			m_bIsMuteBGM = TRUE;
		}
		break;

	case SoundType_Environment :
		{
			SetStreamVolume( 0.0f );
			m_bIsMuteStream = TRUE;
		}
		break;

	case SoundType_Effect :
		{
			SetSampleVolume( 0.0f );
			m_bIsMuteSample = TRUE;

			Set3DSampleVolume( 0.0f );
			m_bIsMute3DSample = TRUE;
		}
		break;
	}

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, sizeof( char ) * 256, "[ Sound ] Mute Sound : %s\n", eType == SoundType_BGM ? "BGM" : eType == SoundType_Environment ? "Environment" : "Effect" );
	OutputDebugString( strDebug );
#endif
}

void AgcmSound::UnMuteSound( eSoundType eType )
{
	switch( eType )
	{
	case SoundType_BGM :
		{
			SetBGMVolume( m_fMasterVolumeBGMPrev );			
			m_bIsMuteBGM = FALSE;
		}
		break;

	case SoundType_Effect :
		{
			SetSampleVolume( m_fMasterVolumeSamplePrev );
			m_bIsMuteSample = FALSE;

			Set3DSampleVolume( m_fMasterVolume3DSamplePrev );
			m_bIsMute3DSample = FALSE;
		}
		break;

	case SoundType_Environment :
		{
			SetStreamVolume( m_fMasterVolumeStreamPrev );
			m_bIsMuteStream = FALSE;
		}
		break;
	}

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, sizeof( char ) * 256, "[ Sound ] UnMute Sound : %s\n", eType == SoundType_BGM ? "BGM" : eType == SoundType_Environment ? "Environment" : "Effect" );
	OutputDebugString( strDebug );
#endif
}

BOOL AgcmSound::PreStop3DSound( void* pClump )
{
	RpClump* pObjectClump = ( RpClump* )pClump;
	if( !pObjectClump ) return FALSE;

	SoundNode* pNode = RpClumpGetFrame( pObjectClump );
	if( !pNode ) return FALSE;

	SoundManager::Inst().ReleaseSceneNode( pNode );
	return TRUE;
}

BOOL AgcmSound::PreStop3DSound( int nChannelIndex )
{
	SoundManager::Inst().ReleaseSceneNodeByIndex( nChannelIndex );
	return TRUE;
}

unsigned int AgcmSound::PlayBGM( char* pFile, int nLoopCount, BOOL bUseFade, float fVolume, int nPriority )
{
	if( m_bIsMuteBGM ) return 0;

	m_nCurrentBGM = SoundManager::Inst().ChangeBGM( pFile, m_fMasterVolumeBGM, bUseFade ? SOUND_FADE_DURATION : 0.0f );

	if( m_bIsMuteBGM )
	{
		MuteSound( SoundType_BGM );
	}
	else
	{
		UnMuteSound( SoundType_BGM );
	}

	return m_nCurrentBGM;
}

BOOL AgcmSound::StopBGM( BOOL bUseFadeOut, INT32 nPriority )
{
	m_nCurrentBGM = SoundManager::Inst().ChangeBGM( NULL, m_fMasterVolumeBGM, bUseFadeOut ? SOUND_FADE_DURATION : 0.0f );
	return TRUE;
}

unsigned int AgcmSound::PlayStream( char* pFile, int nLoopCount, float fVolume, BOOL bUseFade )
{
	if( m_bIsMuteStream )
	{
		MuteSound( SoundType_Environment );
		return 0;
	}

	// 만약 플레이리스트에 있는 것이 또 들어오면 무시한다.
	int nFindEnvCount = SoundManager::Inst().FindEnvironmentSound( pFile );
	if( nFindEnvCount > 0 ) return nFindEnvCount;

//#ifdef _DEBUG
//	char strDebug[ 256 ] = { 0, };
//	sprintf_s( strDebug, "[ Sound ] Play Stream, FileName = %s\n", pFile );
//	OutputDebugString( strDebug );
//#endif

	// 볼륨은 매개변수로 들어온 볼륨은 무시하고 마스터 볼륨을 넣어준다.
	int nIndex = SoundManager::Inst().AddEnvironmentSound( pFile, m_fMasterVolumeStream, bUseFade ? SOUND_FADE_DURATION : 0.0f );

	// 루프카운트를 설정해주고.. 새거 플레이.. 환경사운드는 반복사운드니까 루프카운트는 무한으로 해준다.
	SoundManager::Inst().SetLoopCount( nIndex, 0, -1 );

	// 새거가 몇번인지 리턴
	return nIndex;
}

BOOL AgcmSound::StopStream( unsigned int nIndex, BOOL bUseFadeOut )
{
//#ifdef _DEBUG
//	char strDebug[ 256 ] = { 0, };
//	CSoundInstance* pInstance = SoundManager::Inst().FindInstance( nIndex );
//	sprintf_s( strDebug, "[ Sound ] Stop Stream, FileName = %s\n", pInstance ? pInstance->GetFileName() : "UnKnown" );
//	OutputDebugString( strDebug );
//#endif

	SoundManager::Inst().RemoveEnvironmentSound( nIndex, bUseFadeOut ? 5.0f : 0.0f );
	return TRUE;
}

BOOL AgcmSound::ClearStream( void )
{
//#ifdef _DEBUG
//	OutputDebugString( "[ Sound ] ____ Clear Environment Streaming Sounds. ____\n" );
//#endif

	// 모든 스트리밍 사운드 종료
	SoundManager::Inst().ClearEnvSound();
	return TRUE;
}

unsigned int AgcmSound::Play3DSound( char* pFile, int nLoopCount, float fVolume , BOOL bSetClump, void* pClumpFrame, float fPlaySpeed , BOOL bOnlyPos )
{
	if( m_bIsMute3DSample ) 
		return 0;

	SoundNode* pNode =  NULL;

	// bSetClump가 TRUE면 Clump
	if( bSetClump )
	{
		if( pClumpFrame )
			pNode	=	RpClumpGetFrame( ( RpClump* )pClumpFrame );

		else
		{
			AgcmRender*	pcmRender	=	static_cast< AgcmRender* >( g_pEngine->GetModule("AgcmRender") );
			if( pcmRender )
			{
				RwCamera*	pCamera	=	pcmRender->GetCamera();
				if( pCamera )
				{
					pNode = RwCameraGetFrame( pCamera );
				}
			}
		}
	}

	// FALSE는 Frame
	else
	{
		pNode	=	(RwFrame*)pClumpFrame;
	}

	// 이렇게 해도 Node가 없으면 그냥 버린다
	if( !pNode )
		return 0;

	// 플레이하려면 적절한 거리내에 있어야 한다.
	RwMatrix* pMatrix = RwFrameGetMatrix( pNode );
	if( !pMatrix ) return 0;

	SoundVector* pPos = RwMatrixGetPos( pMatrix );
	if( !pPos ) return 0;

	if( nLoopCount > 0 && !IsIn3DSoundRange( pPos ) )
	{
		//#ifdef _DEBUG
		//		SoundVector vPos = SoundManager::Inst().GetCurrentListenerPos();
		//		float fDistance = SoundUtil::CalcScalarDistance( vPos, *pPos );
		//		
		//		char strDebug[ 256 ] = { 0, };
		//		sprintf_s( strDebug, sizeof( char ) * 256, "[ Sound ] Cancel Play 3D Sound, FileName = %s, Distance = %f\n", pFile, fDistance );
		//		OutputDebugString( strDebug );
		//#endif

		return 0;
	}

	SOUND_TYPE soundType = nLoopCount < 1 ? SOUND_TYPE_3D_SOUND_LOOPED : SOUND_TYPE_3D_SOUND;

	int nIndex = 0;
	nIndex = SoundManager::Inst().Play( pFile, soundType, pNode, nLoopCount, m_fMasterVolume3DSample * fVolume, SOUND_3D_DISTANCE_MIN, SOUND_3D_DISTANCE_MAX, bOnlyPos );


	//#ifdef _DEBUG
	//	char strDebug[ 256 ] = { 0, };
	//	sprintf_s( strDebug, sizeof( char ) * 256, "[ Sound ] Play 3DSound : %s, Volume = %3.3f\n", pFile, m_fMasterVolume3DSample );
	//	OutputDebugString( strDebug );
	//#endif

	return nIndex;
}

BOOL AgcmSound::Stop3DSound( unsigned int nIndex )
{
	SoundManager::Inst().Stop( nIndex );
	return TRUE;
}

unsigned int AgcmSound::PlaySampleSound( char* pFile, int nLoopCount, float fVolume, float fPlaySpeed )
{
	if( m_bIsMuteSample ) return 0;

	return SoundManager::Inst().Play( pFile, SOUND_TYPE_2D_SOUND, NULL, nLoopCount, m_fMasterVolumeSample * fVolume, SOUND_3D_DISTANCE_MIN, SOUND_3D_DISTANCE_MAX );
	
//#ifdef _DEBUG
//	char strDebug[ 256 ] = { 0, };
//	sprintf_s( strDebug, sizeof( char ) * 256, "[ Sound ] Play SampleSound : %s, Volume = %3.3f\n", pFile, m_fMasterVolumeSample );
//	OutputDebugString( strDebug );
//#endif
}

BOOL AgcmSound::StopSampleSound( unsigned int nIndex )
{
	SoundManager::Inst().Stop( nIndex );
	return TRUE;
}

void AgcmSound::JobQueueAdd( enumSoundJobQueueType eJobType, unsigned int nKey )
{
}

void AgcmSound::JobQueueRemove( unsigned int nKey )
{
}

unsigned int AgcmSound::GetStreamSoundLength( const char* pFile )
{
	if( !pFile || strlen( pFile ) <= 0 ) return 0;

	SoundResourceEntry * resourceEntry = SoundManager::Inst().FindResource( SOUND_TYPE_2D_SOUND_LOOPED, pFile );
	if( !resourceEntry || !resourceEntry->get() ) return 0;

	// 들고 있는 시간은 초단위 시간이다 * 1000 해서 ms 단위로 내보낸다.
	return ( unsigned int )( resourceEntry->get()->GetLength() * 1000.0f );
}

unsigned int AgcmSound::GetSampleSoundLength( SOUND_TYPE soundType, const char* pFile )
{
	if( !pFile || strlen( pFile ) <= 0 ) return 0;

	SoundResourceEntry * resourceEntry = SoundManager::Inst().FindResource( soundType, pFile );
	if( !resourceEntry || !resourceEntry->get() ) return 0;

	// 들고 있는 시간은 초단위 시간이다 * 1000 해서 ms 단위로 내보낸다.
	return ( unsigned int )( resourceEntry->get()->GetLength() * 1000.0f );
}

const char* AgcmSound::GetCurrBGMFileName( void )
{
	SoundInstance* pInstance = SoundManager::Inst().FindInstance( m_nCurrentBGM );
	if( !pInstance ) return NULL;

	return pInstance->GetFileName();
}

BOOL AgcmSound::IsIn3DSoundRange( RwV3d* pPos )
{
	if( !pPos ) return FALSE;

	AgcmRender* pcmRender = ( AgcmRender* )g_pEngine->GetModule( "AgcmRender" );
	if( !pcmRender ) return FALSE;


	RwCamera*	pCamera	=	pcmRender->GetCamera();
	RwFrame*	pFrame	=	NULL;

	if( pCamera )
		pFrame = RwCameraGetFrame( pCamera );

	if( !pFrame )
		return FALSE;

	RwMatrix* pLTM = RwFrameGetLTM( pFrame );
	
	RwV3d vTransformedPos = { 0.0f, 0.0f, 0.0f };
	RwV3dSub( &vTransformedPos, &pLTM->pos, pPos );
	RwReal fDist = RwV3dLength( &vTransformedPos );

	return fDist > SOUND_3D_DISTANCE_MAX ? FALSE : TRUE;
}

BOOL AgcmSound::IsSoundMuted( eSoundType eType )
{
	switch( eType )
	{
	case SoundType_BGM :			return m_bIsMuteBGM;		break;
	case SoundType_Effect :			return m_bIsMute3DSample;	break;
	case SoundType_Environment :	return m_bIsMuteStream;		break;
	}

	return FALSE;
}

BOOL AgcmSound::LoadSound( char * pFilename, SOUND_TYPE eType )
{
	static AgcmSound * _this = (AgcmSound*)(g_pEngine->GetModule("AgcmSound"));

	if(!_this)
		return FALSE;

	// 매개변수로 들어온 볼륨값은 무시한다.. 옵션에서 지정된 마스터 볼륨으로..
	if( !SoundManager::Inst().LoadFile( pFilename, eType ) )
	{
		// 3D Effect 사운드는 패킹파일로 묶여있는 경우도 있으니..
		if( !_this->_LoadPackingSound( pFilename, eType ) )
		{
			// 패킹파일에서 읽지 못한 경우 AuPackingManager 를 동원해서 읽어본다.
			AuPackingManager* pPackingManager = AuPackingManager::GetSingletonPtr();
			if( !pPackingManager ) return 0;

			ApdFile pdFile;
			if( !pPackingManager->OpenFile( pFilename, &pdFile ) )
			{
				return FALSE;
			}

			int nFileSize = pPackingManager->GetFileSize( &pdFile );
			if( nFileSize <= 0 )
			{
				pPackingManager->CloseFile( &pdFile );
				return FALSE;
			}

			char* pBuffer = new char[ nFileSize ];
			int nReadSize = pPackingManager->ReadFile( pBuffer, nFileSize, &pdFile );
			pPackingManager->CloseFile( &pdFile );

			if( nReadSize != nFileSize )
			{
				delete[] pBuffer;
				pBuffer = NULL;
				return FALSE;
			}

			BOOL bLoadResult = SoundManager::Inst().LoadBuffer( pBuffer, pFilename, nFileSize, eType );

			delete[] pBuffer;
			pBuffer = NULL;

			if( !bLoadResult ) 
				return FALSE;
		}
	}

	return TRUE;
}

template< typename Container, typename Key, typename Entry >
inline bool insert_entry( Container & container, Key & key, Entry * entry )
{
	if( !entry )
		return false;

	typename Container::iterator iter = container.find( key );

	if( iter != container.end() )
	{
		delete entry;
		return false;
	}

	container.insert( std::make_pair( key, entry ) );

	return true;
};

BOOL AgcmSound::_LoadPackingFile( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	DWORD dwStartTime = ::timeGetTime();

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, sizeof( char ) * 256, "---- Start Load Sound Packing File Information : %s ----\n", pFileName );
	OutputDebugString( strDebug );
#endif

	FILE* pFile = fopen( pFileName, "rb" );
	if( !pFile ) return FALSE;

	// 파일사이즈 얻어오기
	fseek( pFile, 0, SEEK_END );
	long nFileSize = ftell( pFile );

	// 파일포인터 원위치
	fseek( pFile, 0, SEEK_SET );

	if( nFileSize < 1 )
	{
		fclose(pFile);
		return FALSE;
	}

	// 버퍼로 로딩
	char* pBuffer = new char[ nFileSize ];
	memset( pBuffer, 0, sizeof( char ) * nFileSize );
	fread( pBuffer, nFileSize, 1, pFile );

	// 다 읽었으니 파일 닫아버리고..
	fclose( pFile );

	// 최초 4바이트는 폴더갯수이다.
	char* filePos = pBuffer;
	int nPathCount = *( int* )filePos;
	filePos += sizeof( int );

	char orgPath[ 256 ] = { 0, };

	::GetCurrentDirectory( sizeof( orgPath ), orgPath );

	for( int nCountPath = 0 ; nCountPath < nPathCount ; nCountPath++ )
	{
		char packPath[256]={0,};

		memcpy_s( packPath, sizeof( packPath ), filePos, SOUND_PACK_LENGTH_PATH );
		filePos += SOUND_PACK_LENGTH_PATH;

		int nCategoryCount = *( int* )filePos;
		filePos += sizeof( int );

		char curPath[ 256 ] = { 0, };
		sprintf_s( curPath, sizeof(curPath), "%s\\%s%s", orgPath, AGCMSOUND_SOUND_FILEPATH, packPath );		
		::SetCurrentDirectory( curPath );

		for( int nCountCategory = 0 ; nCountCategory < nCategoryCount ; nCountCategory++ )
		{
			filePos += sizeof( BYTE );

			char packFileName[128] = {0,};

			memcpy_s( packFileName, sizeof( packFileName ), filePos, SOUND_PACK_LENGTH_PACKFILENAME );

			filePos += SOUND_PACK_LENGTH_PACKFILENAME;

			filePos += SOUND_PACK_LENGTH_PREFIX;

			int nFileCount = *( int* )filePos;
			filePos += sizeof( int );

			// 원래는 MaxFileCount 가 있지만.. 필요없어졌다..
			filePos += sizeof( int );

			char tmp[ 512 ] = { 0, };
			sprintf_s( tmp, sizeof(tmp), "%s\\%s", curPath, packFileName );

			HANDLE packFileHandle = ::CreateFile( tmp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL );
			if( !packFileHandle || packFileHandle == INVALID_HANDLE_VALUE )
			{
#ifdef _DEBUG
				char strDebug[ 256 ] = { 0, };
				sprintf_s( strDebug, sizeof( char ) * 256, "[ SoundPack ] Cannot Open File : %d\n", packFileHandle );
				OutputDebugString( strDebug );
#endif
				continue;
			}

			CloseHandle( packFileHandle );

			char soundFileName[128];
			char localPath[256];

			for( int nCountFile = 0 ; nCountFile < nFileCount ; nCountFile++ )
			{
				SoundPackFileEntry * newFile = new SoundPackFileEntry;

				int nFileIndex = *( int* )filePos;
				filePos += sizeof( int );

				memset(soundFileName, 0, sizeof(soundFileName));

				memcpy_s( soundFileName, sizeof(soundFileName), filePos, SOUND_PACK_LENGTH_FILENAME );
				filePos += SOUND_PACK_LENGTH_FILENAME;

				newFile->fileOffset_ = *( int* )filePos;
				filePos += sizeof( int );

				newFile->fileSize_ = *( int* )filePos;
				filePos += sizeof( int );

				newFile->soundLength_ = *( int* )filePos;
				filePos += sizeof( unsigned int );

				sprintf_s( newFile->packFileName_, sizeof(newFile->packFileName_), "%s%s\\%s", AGCMSOUND_SOUND_FILEPATH, packPath, packFileName );

				sprintf_s( localPath, sizeof(localPath), "%s%s\\%s", AGCMSOUND_SOUND_FILEPATH, packPath, soundFileName );

				std::transform( localPath, localPath + strlen(localPath), localPath, ::tolower );

				insert_entry( packFiles_, localPath, newFile );
			}
		}
	}

	// 로딩 끝났으니 현재 폴더 원상복구
	::SetCurrentDirectory( orgPath );

	DWORD dwEndTime = ::timeGetTime();
	DWORD dwDuration = dwEndTime - dwStartTime;

#ifdef _DEBUG
	memset( strDebug, 0, sizeof( char ) * 256 );
	sprintf_s( strDebug, sizeof( char ) * 256, "---- Load Sound Packing File Information Complete. ( Loading Time : %d ms )----\n", dwDuration );
	OutputDebugString( strDebug );
#endif

	delete[] pBuffer;

	return TRUE;
}

BOOL AgcmSound::_LoadPackingSound( char* pFileName, SOUND_TYPE eType )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return NULL;

	/*
	// 경로명이 포함되어 있을수 있으니.. 떼어낸다.
	char strFileNameOnly[ 256 ] = { 0, };
	int nNameLength = strlen( pFileName );

	int nLastFolderPos = 0;
	for( int nCount = 0 ; nCount < nNameLength ; nCount++ )
	{
		if( pFileName[ nCount ] == '\\' )
		{
			nLastFolderPos = nCount;
		}

		if( pFileName[ nCount ] == '.' )
		{
			strcpy_s( strFileNameOnly, sizeof( strFileNameOnly ), &pFileName[ nLastFolderPos + 1 ] );
			break;
		}
	}

	// 대문자로 모두 통일시킨다.
	nNameLength = strlen( strFileNameOnly );
	for( int nCount = 0 ; nCount < nNameLength ; nCount++ )
	{
		if( strFileNameOnly[ nCount ] != '.' )
		{
			if( ( strFileNameOnly[ nCount ] < 'A' || strFileNameOnly[ nCount ] > 'Z' ) && ( strFileNameOnly[ nCount ] < '0' || strFileNameOnly[ nCount ] > '9' ) )
			{
				strFileNameOnly[ nCount ] -= 32;
			}
		}
	}

	int nPackFileCount = m_mapPackFile.GetSize();
	for( int nCountPackFile = 0 ; nCountPackFile < nPackFileCount ; nCountPackFile++ )
	{
		stSoundPackFile** pPackFile = m_mapPackFile.GetByIndex( nCountPackFile );
		if( pPackFile && *pPackFile )
		{
			int nCategoryCount = (*pPackFile)->m_mapCategory.GetSize();
			for( int nCountCategory = 0 ; nCountCategory < nCategoryCount ; nCountCategory++ )
			{
				stSoundPackCategory** pCategory = (*pPackFile)->m_mapCategory.GetByIndex( nCountCategory );
				if( pCategory && *pCategory )
				{
					stSoundPackFileEntry* pFile = (*pCategory)->m_mapSoundPackFile.Get( strFileNameOnly );
					if( pFile )
					{
						char strPathFileName[ 256 ] = { 0, };
						sprintf_s( strPathFileName, sizeof( char ) * 256, "%s%s\\%s", AGCMSOUND_SOUND_FILEPATH, (*pPackFile)->m_strPath, (*pCategory)->m_strPackFileName );
						return SoundManager::Inst().LoadStream( strPathFileName, pFileName, pFile->m_nFileOffset, pFile->m_nFileSize, eType );
					}
				}
			}
		}
	}
	*/

	if( pFileName[0] == '\\' )
		++pFileName;

	char lowerName[256] = {0,};

	std::transform( pFileName, pFileName + strlen(pFileName), lowerName, ::tolower );

	PackFiles::iterator iter = packFiles_.find( lowerName );

	if( iter != packFiles_.end() && iter->second )
	{
		return SoundManager::Inst().LoadStream( iter->second->packFileName_, pFileName, iter->second->fileOffset_, iter->second->fileSize_, eType );
	}

	return FALSE;
}

void AgcmSound::_GetDeviceInformation( void )
{
	m_mapDevice.Clear();

	int nDeviceCount = SoundManager::Inst().GetDeviceCount();
	for( int nCount = 0 ; nCount < nDeviceCount ; nCount++ )
	{
		stSoundDeviceEntry NewEntry;

		NewEntry.m_nIndex = nCount;
		if( SoundManager::Inst().GetDeviceInfo( nCount, NewEntry.m_strDeviceName, AGCMSOUND_SOUND_DEVICENAME_LENGTH, &NewEntry.m_stGUID ) )
		{
			if( strlen( NewEntry.m_strDeviceName ) >= 0 )
			{
				int nIndex = m_mapDevice.GetSize();
				m_stDevice[ nIndex ].m_nIndex = NewEntry.m_nIndex;

				memcpy_s( &m_stDevice[ nIndex ].m_stGUID, sizeof( FMOD_GUID ), &NewEntry.m_stGUID, sizeof( FMOD_GUID ) );
				strcpy_s( m_stDevice[ nIndex ].m_strDeviceName, sizeof( char ) * AGCMSOUND_SOUND_DEVICENAME_LENGTH, NewEntry.m_strDeviceName );

				m_mapDevice.Add( NewEntry.m_strDeviceName, NewEntry );
			}
		}
	}
}

FMOD_REVERB_PROPERTIES* AgcmSound::_GetPreset( eSoundEffectRoomType eType )
{
	stSoundPresetEntry* pEntry = m_mapPreset.Get( eType );
	if( !pEntry ) return NULL;
	return &pEntry->m_Property;
}

void AgcmSound::_AddPreset( eSoundEffectRoomType eType, FMOD_REVERB_PROPERTIES* pProperty )
{
	if( !pProperty ) return;

	 stSoundPresetEntry* pEntry = m_mapPreset.Get( eType );
	 if( pEntry )
	 {
		 pEntry->m_Property = *pProperty;
	 }
	 else
	 {
		 stSoundPresetEntry NewEntry;

		 NewEntry.m_eRoomType = eType;
		 NewEntry.m_Property = *pProperty;

		 m_mapPreset.Add( eType, NewEntry );
	 }
}
