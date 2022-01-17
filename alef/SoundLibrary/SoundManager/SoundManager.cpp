#include "SoundManager.h"
#include "SoundResource/SoundResource.h"
#include "SoundInstance/SoundInstance.h"
#include "SoundInstance/Sound2D.h"
#include "SoundInstance/Sound3D.h"
#include "Util/SoundUtil.h"

#pragma comment( lib, "fmodex_vc.lib" )

//-----------------------------------------------------------------------
//

void SoundResourceEntry::del()
{
	if( resource_ )
	{
		delete resource_;
		resource_ = 0;
		refCount_ = 0;
	}
}

//-----------------------------------------------------------------------
//

SoundManager & SoundManager::Inst( void )
{
	static SoundManager inst;
	return inst;
}

//-----------------------------------------------------------------------
//

SoundManager::SoundManager( void )
{
	m_pSystem = NULL;
	m_pListenerNode = NULL;

	memset( m_strCurrBGM, 0, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME );
	currBGMIdx_ = 0;
	m_vPrevListenerPos = vZeroVector;

	m_pfnSoundLoad = &SoundManager::DefaultLoad;

	Initialize();
}

SoundManager::~SoundManager()
{
	//UnInitialize();
}

bool SoundManager::Initialize( void )
{
	if( m_pSystem != NULL ) return false;

	//-----------------------------------------------------------------------
	//
	// fmod 초기화

#define _check_error_( cmd, msg ) \
	if( (cmd) != FMOD_OK ) \
	{ \
	MessageBox( 0, msg, "fmod error", 0 ); \
	return false; \
	}

	FMOD_RESULT result;

	//-----------------------------------------------------------------------
	//
	// system object 생성 및 초기화

	_check_error_( FMOD::System_Create(&m_pSystem), "fail to create fmod system" );

	//-----------------------------------------------------------------------
	//
	// 버전 체크

	unsigned int version;
	_check_error_( m_pSystem->getVersion( &version ), "fail to check version" );

	if( version < FMOD_VERSION )
	{
		char str[1024];
		sprintf_s( str, sizeof(str),  "Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION );
		MessageBox( 0, str, "fmod error", 0 );
		return false;
	}

	//-----------------------------------------------------------------------
	//
	// 스피커 모드 설정

	FMOD_CAPS caps;
	FMOD_SPEAKERMODE speakermode;

	_check_error_( m_pSystem->getDriverCaps( 0, &caps, 0, 0, &speakermode ), "failed - getDriverCaps" );

	_check_error_( m_pSystem->setSpeakerMode( speakermode ), "failed - setSpeakerMode" );

	if( caps & FMOD_CAPS_HARDWARE_EMULATED )				// You might want to warn the user about this.
	{
		// At 48khz, the latency between issuing an fmod command and hearing it will now be about 213ms.
		_check_error_( m_pSystem->setDSPBufferSize( 1024, 10 ), "failed - setDSPBufferSize" );
	}

	//-----------------------------------------------------------------------
	//
	// Sigmatel 인지 확인 

	char driverName[1024];

	_check_error_( m_pSystem->getDriverInfo( 0, driverName, sizeof(driverName), 0 ), "failed - getDriverInfo" );

	if (strstr(driverName, "SigmaTel"))						// Sigmatel sound devices crackle for some reason if the format is pcm 16bit. pcm floating point output seems to solve it.
	{
		_check_error_( m_pSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR), "failed - setSoftwareFormat" );
	}

	//-----------------------------------------------------------------------
	//
	// 초기화

	result = m_pSystem->init( MAX_SOUND_CHANNELS, FMOD_INIT_NORMAL, 0 );

	if( result == FMOD_ERR_OUTPUT_CREATEBUFFER ) // 지원하지 않은 사운드 모드로 설정된 경우
	{
		_check_error_( m_pSystem->setSpeakerMode( FMOD_SPEAKERMODE_STEREO ), "failed - setting speaker mode to stereo" );

		_check_error_( m_pSystem->init( MAX_SOUND_CHANNELS, FMOD_INIT_NORMAL, 0 ), "failed - system ini to stereo" );
	}

	// 3D 사운드를 위한 거리비율 설정 ( 도플러효과비율, 1미터를 표현하기 위한 거리값, 롤오프비율값 )
	_check_error_( m_pSystem->set3DSettings( 0.1f, SCALE_FACTOR_P_TO_R, 0.1f ), "failed - system setting" );

	//-----------------------------------------------------------------------

#undef _check_error_

	return true;
}

//-----------------------------------------------------------------------
//

void SoundManager::UnInitialize( void )
{
	_DeleteAllSoundInstance();
	_DeleteAllSoundResource();

	m_listReservedDeleteData.clear();

	m_pListenerNode = NULL;
	memset( m_strCurrBGM, 0, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME );

	if( m_pSystem != NULL )
	{
		m_pSystem->release();
		m_pSystem = NULL;
	}
}

//-----------------------------------------------------------------------
//

BOOL SoundManager::DefaultLoad( char* pFileName, SOUND_TYPE eType )
{
	return Inst().LoadFile( pFileName, eType );
}

//-----------------------------------------------------------------------
//

BOOL SoundManager::LoadFile( char* pFileName, SOUND_TYPE eType )
{
	SoundResourceEntry * resourceEntry = FindResource( eType, pFileName );

	if( resourceEntry )
	{
		resourceEntry->incRef();
	}
	else
	{
		SoundResource* pResource = new SoundResource;
		if( pResource->Create( m_pSystem, pFileName, eType, 1.0f, 0.0f, 6000.0f ) == false )
		{
			delete pResource;
			return FALSE;
		}

		m_mapSoundResource[eType][pFileName].set( pResource );

#ifdef _DEBUG
		_NoticeSoundResource();
#endif
	}

	return TRUE;
}

//-----------------------------------------------------------------------
//

BOOL SoundManager::LoadStream( char* pPathFileName, char* pFileName, int nFileOffset, int nFileSize, SOUND_TYPE eType )
{
	SoundResourceEntry * resourceEntry = FindResource( eType, pFileName );

	if( resourceEntry )
	{
		resourceEntry->incRef();
	}
	else
	{
		SoundResource* pResource = new SoundResource;
		if( pResource->Create( m_pSystem, pPathFileName, pFileName, nFileOffset, nFileSize, eType, 1.0f, 0.0f, 6000.0f ) == false )
		{
			delete pResource;
			return FALSE;
		}

		m_mapSoundResource[eType][pFileName].set( pResource );

#ifdef _DEBUG
		_NoticeSoundResource();
#endif
	}

	return TRUE;
}

//-----------------------------------------------------------------------
//

BOOL SoundManager::LoadBuffer( char* pBuffer, char* pFileName, int nFileSize, SOUND_TYPE eType )
{
	SoundResourceEntry * resourceEntry = FindResource( eType, pFileName );

	if( resourceEntry )
	{
		resourceEntry->incRef();
	}
	else
	{
		SoundResource* pResource = new SoundResource;
		if( pResource->Create( m_pSystem, pBuffer, pFileName, nFileSize, eType, 1.0f, 0.0f, 6000.0f ) == false )
		{
			delete pResource;
			return FALSE;
		}

		m_mapSoundResource[eType][pFileName].set( pResource );

#ifdef _DEBUG
		_NoticeSoundResource();
#endif
	}

	return TRUE;
}

//-----------------------------------------------------------------------
//

BOOL SoundManager::UnLoad( SOUND_TYPE type, char* pFileName )
{
	SoundResources & resources = m_mapSoundResource[type];

	SoundResources::iterator iter = resources.find( pFileName );

	if( iter == resources.end() )
		return FALSE;

	SoundResourceEntry & resourceEntry = iter->second;

	resourceEntry.decRef();

	if( resourceEntry.get() == 0 )
		resources.erase( iter );

#ifdef _DEBUG
	_NoticeSoundResource();
#endif

	return TRUE;
}

//-----------------------------------------------------------------------
//

BOOL SoundManager::UnLoad( SoundResource* pResource )
{
	if( !pResource ) return FALSE;

	return UnLoad( pResource->GetType(), pResource->GetFileName() );
}

//-----------------------------------------------------------------------
//

SoundResourceEntry* SoundManager::FindResource( SOUND_TYPE type, const char* pFileName )
{
	if( !pFileName ) return NULL;

	SoundResources & resources = m_mapSoundResource[type];

	SoundResources::iterator iter = resources.find( pFileName );

	return iter == resources.end() ? 0 : &(iter->second);
}

//-----------------------------------------------------------------------
//

SoundInstance* SoundManager::FindInstance( const int nIndex )
{
	SoundInstances::iterator iter = m_mapSoundInstances.find( nIndex );

	return iter == m_mapSoundInstances.end() ? 0 : iter->second;
}

//-----------------------------------------------------------------------
//

int SoundManager::CreateInstance( char* pFileName, SOUND_TYPE soundType, SoundNode* pTargetNode, int* pChannelIndex , BOOL bOnlyPos )
{
	// 리소스를 로딩하거나 참조 카운트 증가
	m_pfnSoundLoad( pFileName, soundType );

	// 파일명에 해당하는 음원을 찾지 못하면 패스
	SoundResourceEntry * resourceEntry = FindResource( soundType, pFileName );

	if( !resourceEntry )
		return 0;

	SoundResource * pResource = resourceEntry->get();

	if( pResource == NULL )	
		return 0;

	SoundInstance* pNewInstance = NULL;

	switch( soundType )
	{
		// 2D 사운드인 경우
	case SOUND_TYPE_2D_SOUND :
	case SOUND_TYPE_2D_SOUND_LOOPED :
		{
			pNewInstance = new Sound2D();
			pNewInstance->Create( pResource );
		}
		break;

		// 3D 사운드인 경우
	case SOUND_TYPE_3D_SOUND :
	case SOUND_TYPE_3D_SOUND_LOOPED :
		{
			Sound3D * sound3dInst = new Sound3D( pResource );

			if( bOnlyPos )
			{
				SoundVector pos = SoundUtil::GetWorldPositionFromNode( pTargetNode );
				sound3dInst->SetPos( pos );
				sound3dInst->SetPrevPos( pos );
			}
			else
			{
				sound3dInst->SetSceneNode( pTargetNode );
			}

			pNewInstance = sound3dInst;

			if( pChannelIndex )
			{
				*pChannelIndex = pNewInstance->GetIndex();
			}
		}
		break;

	default :	
		return 0;
	}

	//#ifdef _DEBUG
	//	char strDebug[ 256 ] = { 0, };
	//	sprintf_s( strDebug, "[ Sound ] Create Sound Instance, FileName = %s, Type = %d, TotalCount = %d\n",
	//		pNewInstance->GetFileName(), pNewInstance->GetType(), m_mapSoundInstance.GetSize() + 1 );
	//	OutputDebugString( strDebug );
	//#endif

	SoundInstances::iterator iter = m_mapSoundInstances.find( pNewInstance->GetIndex() );

	if( iter != m_mapSoundInstances.end() ) // 인덱스가 중복됨 생성 실패
	{
		delete pNewInstance;
		return 0;
	}

	m_mapSoundInstances.insert( std::make_pair( pNewInstance->GetIndex(), pNewInstance ) );

#ifdef _DEBUG
	_NoticeSoundResource();
#endif

	return pNewInstance->GetIndex();
}

//-----------------------------------------------------------------------
//

bool SoundManager::DeleteInstance( const int nChannelIndex )
{
	SoundInstances::iterator iter = m_mapSoundInstances.find( nChannelIndex );

	return DeleteInstance( iter );
}

//-----------------------------------------------------------------------
//

bool SoundManager::DeleteInstance( SoundManager::SoundInstances::iterator iter )
{
	if( iter == m_mapSoundInstances.end() )
		return false;

	SoundInstance * inst = iter->second;

	if( inst )
	{
		SoundResources & resources = m_mapSoundResource[ inst->GetType() ];

		SoundResources::iterator resourceIter = resources.find( inst->GetFileName() );

		inst->Stop();

		if( resourceIter != resources.end() )
			UnLoad( resourceIter->second.get() );

		inst->Destroy();

		delete inst;
	}

	m_mapSoundInstances.erase( iter );

#ifdef _DEBUG
	_NoticeSoundResource();
#endif
	return true;
}

//-----------------------------------------------------------------------
//

void SoundManager::Update( SoundNode* pListenerNode, float fTimeElapsed )
{
	for( SoundInstances::iterator iter = m_mapSoundInstances.begin(); iter != m_mapSoundInstances.end(); )
	{
		SoundInstance * inst = iter->second;

		if( inst )
		{
			SoundInstances::iterator curIter = iter;
			++iter;

			inst->Update( fTimeElapsed, pListenerNode );
		}
		else
		{
			m_mapSoundInstances.erase( iter++ );
		}
	}

	UpdateListenerPostion( pListenerNode, fTimeElapsed );

	// 사운드 재생을 진행시킨다.
	if( m_pSystem != NULL )
	{
		m_pSystem->update();
	}

	for( SoundInstances::iterator iter = m_mapSoundInstances.begin(); iter != m_mapSoundInstances.end(); )
	{
		SoundInstance * inst = iter->second;

		if( inst )
		{
			SoundInstances::iterator curIter = iter;
			++iter;

			if( !inst->PostUpdate() ) // 종료된 사운드인스턴스 제거
			{
				//DeleteInstance( curIter );
				ReserveDelete(curIter->first);
			}
		}
		else
		{
			m_mapSoundInstances.erase( iter++ );
		}
	}

	static float fDeleteTimer = 0.0f;
	fDeleteTimer += fTimeElapsed;

	if( fDeleteTimer > 10*60 )
	{
		// 10분마다 예약된 사운드 리소스 제거
		fDeleteTimer = 0.0f;
		DeleteReserved();
	}
}

//-----------------------------------------------------------------------
//

void SoundManager::UpdateListenerPostion( SoundNode* pListenerNode, float fTimeElapsed )
{
	if( pListenerNode == NULL ) return;

	FMOD_VECTOR vListenerPos;
	FMOD_VECTOR vListenerForward;
	FMOD_VECTOR vListenerUp;
	FMOD_VECTOR vListenerVelocity;

	SoundVector vVelocity = vZeroVector;
	//SoundVector vForward = vZeroVector;
	SoundVector vUp = vZeroVector;

	SoundVector vPos = SoundUtil::GetWorldPositionFromNode( pListenerNode );

	if( fTimeElapsed > 0 )
		vVelocity = SoundUtil::CalcVectorVelocity( &vPos, &m_vPrevListenerPos, fTimeElapsed );

	SoundVector vAt = SoundUtil::GetAtFromNode( pListenerNode );
	//vForward = SoundUtil::CalcVectorScale( vAt, -1.0f );
	//vForward = SoundUtil::NormalizeVector( vForward );
	//RwV3dNormalize( vForward, vAt );
	//vForward = SoundUtil::NormalizeVector( vAt );


	vUp = SoundUtil::GetUpFromNode( pListenerNode );
	//vUp = SoundUtil::NormalizeVector( vUp );

	VECTOR3_SET_FMODVECTOR( vListenerPos, vPos );
	VECTOR3_SET_FMODVECTOR( vListenerForward, vAt );
	VECTOR3_SET_FMODVECTOR( vListenerUp, vUp );
	VECTOR3_SET_FMODVECTOR( vListenerVelocity, vVelocity );

	m_pSystem->set3DListenerAttributes( 0, &vListenerPos, &vListenerVelocity, &vListenerForward, &vListenerUp );
	m_vPrevListenerPos = vPos;
}

//-----------------------------------------------------------------------
//

int SoundManager::Play( char* pFileName, SOUND_TYPE soundType, SoundNode* pSceneNode, int loopCnt, float volume, float minDist, float maxDist, BOOL onlyPos )
{
	// 3D 사운드 인데 위치값이 없으면 실패
	if( soundType == SOUND_TYPE_3D_SOUND || soundType == SOUND_TYPE_3D_SOUND_LOOPED )
	{
		if( pSceneNode == 0 )
			return 0;
	}

	// 인스턴스 생성
	int instIdx = CreateInstance( pFileName, soundType, pSceneNode, 0, onlyPos );

	if( instIdx == 0 )
		return 0;

	SoundInstance * inst = FindInstance( instIdx );

	if( !inst )
		return 0;

	inst->SetVolume( volume );
	inst->SetActiveDistanceMinMax( minDist, maxDist );
	inst->SetLoopCount( loopCnt < 1 ? -1 : loopCnt );

	if( soundType == SOUND_TYPE_3D_SOUND || soundType == SOUND_TYPE_3D_SOUND_LOOPED )
	{
		Sound3D * inst3D = reinterpret_cast< Sound3D* >(inst);

		if( onlyPos )
		{
			SoundVector pos = SoundUtil::GetWorldPositionFromNode( pSceneNode );
			inst3D->SetPos( pos );
			inst3D->SetPrevPos( pos );
		}
		else
		{
			inst3D->SetSceneNode( pSceneNode );
		}

		inst3D->Update(0,0);
	}

	// 플레이
	inst->Play();

	// 플레이한 녀석의 인스턴스 번호를 리턴해준다.
	return inst->GetIndex();
}
//-----------------------------------------------------------------------
//

void SoundManager::Stop( int nChannelIndex )
{
	DeleteInstance( nChannelIndex );
}

//-----------------------------------------------------------------------
//

void SoundManager::FadeIn( int nChannelIndex, const float fDuration, const float fTargetVolume )
{
	SoundInstance* pInstance = FindInstance( nChannelIndex );
	if( !pInstance ) return;

	pInstance->FadeIn( fDuration, fTargetVolume );
}

//-----------------------------------------------------------------------
//

void SoundManager::FadeOut( int nChannelIndex, const float fDuration, const float fTargetVolume )
{
	SoundInstance* pInstance = FindInstance( nChannelIndex );
	if( !pInstance ) return;

	pInstance->FadeOut( fDuration, fTargetVolume );
}

//-----------------------------------------------------------------------
//

void SoundManager::StopAllSound( void )
{
	_DeleteAllSoundInstance();
}

//-----------------------------------------------------------------------
//

int SoundManager::ChangeBGM( char* pFileName, float volume, const float fDuration )
{
	// 제대로 된 파일명이 들어오지 않는다면 기존에 플레이중인것을 끈다는 의미로 본다.
	if( !pFileName || strlen( pFileName ) <= 0 )
	{
		// 현재 플레이 중인 녀석은 FadeOut 시킨다. FadeOut 이 끝나면 종료된다.
		FadeOut( currBGMIdx_, fDuration );
		return 0;
	}

	// 현재 bgm과 같은 bgm 이면 그냥 리턴
	if( strcmp( m_strCurrBGM, pFileName ) == 0 )
	{
		SoundInstance * inst = FindInstance( currBGMIdx_ );
		if( inst && !strcmp( inst->GetFileName(), m_strCurrBGM ) )
		{
			return currBGMIdx_;
		}
	}

	// 현재 플레이 중인 녀석은 FadeOut 시킨다.
	FadeOut( currBGMIdx_, fDuration );

	// 재생중인 파일명을 지운다.
	memset( m_strCurrBGM, 0, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME );

	// 새로 들어온 BGM 을 플레이하고.. 플레이 실패하면 0 리턴
	currBGMIdx_ = Play( pFileName, SOUND_TYPE_2D_SOUND_LOOPED, NULL, 0, volume, 0, 10000.0f );

	if( currBGMIdx_ == 0 )
		return 0;

	// 사운드 속성 설정
	SetLoopCount( currBGMIdx_, NULL, -1 );
	SetVolume( currBGMIdx_, volume );

	strcpy_s( m_strCurrBGM, sizeof( char ) * SOUND_STRING_LENGTH_FILENAME, pFileName );

	// 지정된 길이로 FadeIn 걸어준다.
	FadeIn( currBGMIdx_, fDuration, volume );

	return currBGMIdx_;
}

//-----------------------------------------------------------------------
//

int SoundManager::AddEnvironmentSound( char* pFileName, float volume, const float fDuration )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return 0;

	// 플레이.. 실패하면 0 리턴..
	int channelIndex = Play( pFileName, SOUND_TYPE_2D_SOUND_LOOPED, NULL, 0, volume, 0, 10000.0f );
	if( channelIndex == 0 )
		return 0;


	// FadeIn 걸어주고
	FadeIn( channelIndex, fDuration, volume );

	// 환경 사운드 목록에도 추가
	m_mapEnvironmentSounds.insert( channelIndex );

	return channelIndex;
}

//-----------------------------------------------------------------------
//

void SoundManager::RemoveEnvironmentSound( int nChannelIndex, const float fDuration )
{
	SoundInstanceIndices::iterator iter  = m_mapEnvironmentSounds.find( nChannelIndex );

	if( iter == m_mapEnvironmentSounds.end() )
		return;

	FadeOut( *iter, fDuration );

	m_mapEnvironmentSounds.erase( iter );
}

//-----------------------------------------------------------------------
//

int SoundManager::FindEnvironmentSound( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return 0;

	for( SoundInstanceIndices::iterator iter = m_mapEnvironmentSounds.begin(); iter != m_mapEnvironmentSounds.end(); ++iter )
	{
		SoundInstance * inst = FindInstance( *iter );
		if( inst )
		{
			if( strcmp( inst->GetFileName(), pFileName ) == 0 )
				return inst->GetIndex();
		}
	}

	return 0;
}

//-----------------------------------------------------------------------
//

void SoundManager::SetEnvSoundVolume( float fVolume )
{
	for( SoundInstanceIndices::iterator iter = m_mapEnvironmentSounds.begin(); iter != m_mapEnvironmentSounds.end(); ++iter )
		SetVolume( *iter, fVolume );
}

//-----------------------------------------------------------------------
//

void SoundManager::ClearEnvSound( void )
{
	for( SoundInstanceIndices::iterator iter = m_mapEnvironmentSounds.begin(); iter != m_mapEnvironmentSounds.end(); ++iter )
		Stop( *iter );

	m_mapEnvironmentSounds.clear();
}

//-----------------------------------------------------------------------
//

void SoundManager::ReleaseSceneNode( SoundNode* pSceneNode )
{
	for( SoundInstances::iterator iter = m_mapSoundInstances.begin(); iter != m_mapSoundInstances.end(); )
	{
		SoundInstance * inst = iter->second;

		if( inst )
		{
			SoundInstances::iterator curIter = iter;
			++iter;

			if( inst->Is3DSound() )
			{
				Sound3D * inst3d = reinterpret_cast<Sound3D*>(inst);

				if( inst3d->GetSceneNode() == pSceneNode )
					DeleteInstance( curIter );
			}
		}
		else
		{
			m_mapSoundInstances.erase( iter++ );
		}
	}
}

//-----------------------------------------------------------------------
//

void SoundManager::ReleaseSceneNodeByIndex( int nIndex )
{
	DeleteInstance( nIndex );
}

//-----------------------------------------------------------------------
//

void SoundManager::ReleaseAllSceneNode( void )
{
	for( SoundInstances::iterator iter = m_mapSoundInstances.begin(); iter != m_mapSoundInstances.end(); )
	{
		SoundInstance * inst = iter->second;

		if( inst )
		{
			SoundInstances::iterator curIter = iter;

			++iter;

			if( inst->Is3DSound() )
			{
				DeleteInstance( curIter );
			}
		}
		else
		{
			m_mapSoundInstances.erase( iter++ );
		}
	}
}

//-----------------------------------------------------------------------
//

void SoundManager::SetSoundPreset( const FMOD_REVERB_PROPERTIES* pProperty )
{
	if( !pProperty ) return;
	if( !m_pSystem ) return;

	// 그냥 일단 블럭
	// 	if( SoundUtil::GetOSVersionType() < 7 ) // 윈도우 버전이 비스타 이상인 경우에는 소리울림 현상으로 일단 ReverbProperties를 사용하지 않는다..
	// 		m_pSystem->setReverbProperties( pProperty );
}

//-----------------------------------------------------------------------
//

void SoundManager::SetSpeakerMode( FMOD_SPEAKERMODE eMode )
{
	if( !m_pSystem ) return;
	m_pSystem->setSpeakerMode( eMode );
}

//-----------------------------------------------------------------------
//

BOOL SoundManager::IsPlaying( int nIndex )
{
	SoundInstance* pInstance = FindInstance( nIndex );
	if( !pInstance ) return FALSE;
	return pInstance->IsPlaying() ? TRUE : FALSE;
}

//-----------------------------------------------------------------------
//

int SoundManager::GetDeviceCount( void )
{
	if( !m_pSystem ) return 0;

	int nDeviceCount = 0;
	m_pSystem->getNumDrivers( &nDeviceCount );

	return nDeviceCount;
}

//-----------------------------------------------------------------------
//

BOOL SoundManager::GetDeviceInfo( int nIndex, char* pDeviceName, int nBufferSize, FMOD_GUID* pDeviceID )
{
	if( !m_pSystem ) return FALSE;
	m_pSystem->getDriverInfo( nIndex, pDeviceName, nBufferSize, pDeviceID );
	return TRUE;
}

//-----------------------------------------------------------------------
//

BOOL SoundManager::UseDevice( int nIndex )
{
	if( !m_pSystem ) return FALSE;
	m_pSystem->setDriver( nIndex );
	return TRUE;
}

//-----------------------------------------------------------------------
//

float SoundManager::GetVolume( int nChannelIndex, SoundNode* pSceneNode )
{
	SoundInstance* pInstance = FindInstance( nChannelIndex );
	if( pInstance != NULL )
	{
		return pInstance->GetVolume();
	}

	return 0.0f;
}

//-----------------------------------------------------------------------
//

void SoundManager::SetVolume( int nChannelIndex, const float fVolume )
{
	SoundInstance* pInstance = FindInstance( nChannelIndex );
	if( pInstance != NULL )
	{
		pInstance->SetVolume( fVolume );
	}
}

//-----------------------------------------------------------------------
//

void SoundManager::SetLoopCount( int instIdx, SoundNode* pSceneNode, const int nLoopCount )
{
	SoundInstance* pInstance = FindInstance( instIdx );
	if( pInstance != NULL )
	{
		pInstance->SetLoopCount( nLoopCount );
	}
}

//-----------------------------------------------------------------------
//

SoundVector SoundManager::GetCurrentListenerPos( void )
{
	SoundVector vPos = { 0.0f, 0.0f, 0.0f };
	if( !m_pListenerNode ) return vPos;

	vPos = SoundUtil::GetWorldPositionFromNode( m_pListenerNode );
	return vPos;
}

//-----------------------------------------------------------------------
//

void SoundManager::_DeleteAllSoundInstance( void )
{
	for( SoundInstances::iterator iter = m_mapSoundInstances.begin(); iter != m_mapSoundInstances.end(); ++iter )
	{
		if( iter->second )
			delete iter->second;
	}

	m_mapSoundInstances.clear();
}

//-----------------------------------------------------------------------
//

void SoundManager::_DeleteAllSoundResource( void )
{
	for( SoundResourceMap::iterator iter = m_mapSoundResource.begin(); iter != m_mapSoundResource.end(); ++iter )
	{
		SoundResources & resources = iter->second;

		for( SoundResources::iterator resource = resources.begin(); resource != resources.end(); ++resource )
		{
			if( resource->second.get() )
				delete resource->second.get();
		}
	}

	m_mapSoundResource.clear();
}

//-----------------------------------------------------------------------
//

void SoundManager::Delete3D()
{
	for( SoundInstances::iterator iter = m_mapSoundInstances.begin(); iter != m_mapSoundInstances.end(); )
	{
		if( iter->second && iter->second->Is3DSound() )
		{
			delete iter->second;
			m_mapSoundInstances.erase( iter++ );
		}
		else
		{
			++iter;
		}
	}
}

//-----------------------------------------------------------------------
//

void SoundManager::Delete2D()
{
	for( SoundInstances::iterator iter = m_mapSoundInstances.begin(); iter != m_mapSoundInstances.end(); )
	{
		if( iter->second && !(iter->second->Is3DSound()) && iter->first != currBGMIdx_ )
		{
			delete iter->second;
			m_mapSoundInstances.erase( iter++ );
		}
		else
		{
			++iter;
		}
	}
}

//-----------------------------------------------------------------------
//

void SoundManager::Change2DVolume( float volumeDelta )
{
	for( SoundInstances::iterator iter = m_mapSoundInstances.begin(); iter != m_mapSoundInstances.end(); ++iter )
	{
		if( iter->second && !(iter->second->Is3DSound()) && iter->first != currBGMIdx_ )
		{
			iter->second->SetVolume( iter->second->GetVolume() + volumeDelta );
		}
	}
}

//-----------------------------------------------------------------------
//

void SoundManager::Change3DVolume( float volumeDelta )
{
	for( SoundInstances::iterator iter = m_mapSoundInstances.begin(); iter != m_mapSoundInstances.end(); ++iter )
	{
		if( iter->second && iter->second->Is3DSound() )
		{
			iter->second->SetVolume( iter->second->GetVolume() + volumeDelta );
		}
	}
}

//-----------------------------------------------------------------------
//

void SoundManager::_NoticeSoundResource( void )
{
	//if( !m_pSystem ) return;

	//unsigned int nMemoryBits = FMOD_MEMBITS_ALL;
	//unsigned int nMemoryBitsEvent = FMOD_EVENT_MEMBITS_ALL;
	//unsigned int nMemoryUsedSystem = 0;
	//unsigned int nMemoryUsedSoundTotal = 0;
	//unsigned int nMemoryUsedChannelTotal = 0;

	//FMOD_RESULT Result = m_pSystem->getMemoryInfo( nMemoryBits, nMemoryBitsEvent, &nMemoryUsedSystem, NULL );
	//if( Result != FMOD_OK ) return;

	//int nResourceCount = m_mapSoundResource.GetSize();
	//for( int nCount = 0 ; nCount < nResourceCount ; nCount++ )
	//{
	//	CSoundResource** ppResouce = m_mapSoundResource.GetByIndex( nCount );
	//	if( ppResouce != NULL )
	//	{
	//		CSoundResource* pResource = *ppResouce;
	//		if( pResource && pResource->m_pSound )
	//		{
	//			unsigned int nMemoryUsedSound = 0;
	//			pResource->m_pSound->getMemoryInfo( nMemoryBits, nMemoryBitsEvent, &nMemoryUsedSound, NULL );
	//			nMemoryUsedSoundTotal += nMemoryUsedSound;
	//		}			
	//	}
	//}

	//int nInstanceCount = m_mapSoundInstance.GetSize();
	//for( int nCount = 0 ; nCount < nInstanceCount ; nCount++ )
	//{
	//	CSoundInstance** ppInstance = m_mapSoundInstance.GetByIndex( nCount );
	//	if( ppInstance != NULL )
	//	{
	//		CSoundInstance* pInstance = *ppInstance;
	//		if( pInstance && pInstance->m_pChannel )
	//		{
	//			unsigned int nMemoryUsedSound = 0;
	//			pInstance->m_pChannel->getMemoryInfo( nMemoryBits, nMemoryBitsEvent, &nMemoryUsedSound, NULL );
	//			nMemoryUsedChannelTotal += nMemoryUsedSound;
	//		}			
	//	}
	//}

	//char strDebug[ 256 ] = { 0, };
	//sprintf_s( strDebug, sizeof( char ) * 256, "[ Sound ] MemoryUseSystem = %u Bytes, ResourceCount = %d, MemoryUseSound = %u Bytes, InstanceCount = %d, nMemoryUsedChannel = %u.\n", nMemoryUsedSystem, nResourceCount, nMemoryUsedSoundTotal, nInstanceCount, nMemoryUsedChannelTotal );
	//OutputDebugString( strDebug );
}

void SoundManager::SetLoader( SoundLoadFunc func )
{
	m_pfnSoundLoad = func;
}

void SoundManager::ReserveDelete( const int nChannelIndex )
{
	ReservedDeleteData::iterator itr = m_listReservedDeleteData.begin();

	// 	bool bIsAlreadyExist = false;
	// 
	// 	for(; itr!=m_listReservedDeleteData.end() ;++itr)
	// 	{
	// 		if((*itr) == nChannelIndex)
	// 		{
	// 			bIsAlreadyExist = true;
	// 			break;
	// 		}
	// 	}
	// 
	// 	if(false == bIsAlreadyExist)
	m_listReservedDeleteData.insert(nChannelIndex);
}

bool SoundManager::DeleteReserved( void )
{
	ReservedDeleteData::iterator itr = m_listReservedDeleteData.begin();

#ifdef _DEBUG
	int nSize = m_listReservedDeleteData.size();
	char szBuf[255] = {0,};
	sprintf_s(szBuf, 255, "%d Sound instances have been released.", nSize);
	OutputDebugString(szBuf);
#endif

	for(; itr!=m_listReservedDeleteData.end() ;)
	{
		int nChannelIndex = *itr;
		DeleteInstance(nChannelIndex);
		m_listReservedDeleteData.erase(itr++);
	}

	m_listReservedDeleteData.clear();

	return true;
}

//-----------------------------------------------------------------------