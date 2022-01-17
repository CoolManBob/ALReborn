#include "AgcuEffUtil.h"
#include "AgcdEffSound.h"
#include "AgcuEffPath.h"
#include "AgcdEffGlobal.h"

#include "ApMemoryTracker.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffSound);

AgcdEffSound::AgcdEffSound(eEffSoundType eSoundType ) : AgcdEffBase( AgcdEffBase::E_EFFBASE_SOUND ), 
 m_eSoundType( eSoundType ),
 m_dwLoopCnt( 1 ),
 m_fVolume( 1.1f ),
 m_ulSoundLen( 0LU )
{
	EFFMEMORYLOG_CON;

	Eff2Ut_ZEROBLOCK( m_szSoundFName );
	Eff2Ut_ZEROBLOCK( m_szMonoFName );

#ifdef USE_MFC
	bFlagOn(FLAG_EFFBASESOUND_STOPATEFFECTEND);
#endif //USE_MFC
}

AgcdEffSound::~AgcdEffSound()
{
	EFFMEMORYLOG_DES;
}

RwInt32 AgcdEffSound::bSetSoundName(const RwChar* szSound)
{
	if( !szSound )
	{
		Eff2Ut_ERR( "AgcdEffSound::bSetSoundName failed : szSound == NULL" );
		return -1;
	}

	ASSERT( sizeof(m_szSoundFName) > strlen(szSound) );
	strcpy( m_szSoundFName, szSound );
	return 0;
}

RwInt32 AgcdEffSound::bSetMonoFName(const RwChar* szMono)
{
	if( !szMono )
	{
		Eff2Ut_ERR( "AgcdEffSound::bSetMonoFName failed : szMono == NULL" );
		return -1;
	}

	ASSERT( sizeof(m_szMonoFName) > strlen(szMono) );
	strcpy( m_szMonoFName, szMono );
	return 0;
}

RwInt32 AgcdEffSound::bStopSound(eEffSoundType soundType, RwUInt32 dwIndex)
{
	BOOL br	= false;
	switch( soundType )
	{
	case EFFECT_SOUND_NONE:
		return true;
	case EFFECT_SOUND_3DSOUND:
		//if( dwIndex == AGCMSOUND_3D_SOUND_NO_SLOT || dwIndex == AGCMSOUND_3D_SOUND_OVER_RANGE )
		//{
		//	br = true;
		//	break;
		//}
		br	= AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->Stop3DSound( dwIndex );
		break;

	//case EFFECT_STREAM_SOUND:
	//	br	= AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->StopStream( dwIndex, true );
	//	break;

	case EFFECT_SAMPLE_SOUND:
		br	= AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->StopSampleSound( dwIndex );
		break;

	case EFFECT_BGM_SOUND:
		br	= AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->StopBGM( true );
		break;
	}

	return br ? 0 : -1;
}

RwUInt32 AgcdEffSound::bPlaySound(eEffSoundType soundType, RwReal fVolume, RwUInt32 dwLoopCnt, BOOL bClump , PVOID pClumpParent )
{
	RwUInt32	dwIndex	= 0LU;
	char szSound[MAX_PATH]	= "";

	switch( soundType )
	{
	case EFFECT_SOUND_NONE:
		Eff2Ut_TOFILE( "AgcdEffSound::bPlaySound_Failed.txt", Eff2Ut_FmtMsg("EFFECT_SOUND_NONE\n"));
		break;

	case EFFECT_SOUND_3DSOUND:
		{
			strcpy( szSound, AgcuEffPath::GetPath_SoundMono() );
			strncat( szSound, m_szMonoFName, EFF2_FILE_NAME_MAX );
			ASSERT( sizeof(szSound) > strlen(szSound) );
			
			dwIndex	= AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->Play3DSound( szSound, dwLoopCnt, fVolume , bClump , pClumpParent , 1.0f , TRUE );

			if( !dwIndex )
			{
				Eff2Ut_TOFILE( "AgcdEffSound::bPlaySound_Failed.txt", Eff2Ut_FmtMsg("szSound : %s Play3DSound failed\n", szSound ) );
			}

			m_ulSoundLen = AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->GetSampleSoundLength( dwLoopCnt < 1 ? SOUND_TYPE_3D_SOUND_LOOPED : SOUND_TYPE_3D_SOUND, szSound );
		}
		break;

	//case EFFECT_STREAM_SOUND:
	//	{
	//		if( strlen( m_szSoundFName ) )
	//		{
	//			strcpy( szSound, AgcuEffPath::GetPath_SoundStream() );
	//			strncat( szSound, m_szSoundFName, EFF2_FILE_NAME_MAX );
	//			ASSERT( sizeof(szSound) > strlen(szSound) );
	//		}
	//		else
	//		{
	//			strcpy( szSound, AgcuEffPath::GetPath_SoundStream() );
	//			strncat( szSound, m_szMonoFName, EFF2_FILE_NAME_MAX );
	//			ASSERT( sizeof(szSound) > strlen(szSound) );
	//		}
	//		dwIndex	= AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->PlayStream( szSound, dwLoopCnt, fVolume );
	//		if( !dwIndex )
	//		{
	//			Eff2Ut_TOFILE( "AgcdEffSound::bPlaySound_Failed.txt", Eff2Ut_FmtMsg("szSound : %s PlayStream failed\n", szSound) );
	//		}

	//		m_ulSoundLen = AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->GetStreamSoundLength( szSound );
	//	}
	//	break;

	case EFFECT_SAMPLE_SOUND:
		if( strlen( m_szSoundFName ) )
		{
			strcpy( szSound, AgcuEffPath::GetPath_Sound() );
			strncat( szSound, m_szSoundFName, EFF2_FILE_NAME_MAX );
			ASSERT( sizeof(szSound) > strlen(szSound) );
			dwIndex	= AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->PlaySampleSound( szSound, dwLoopCnt, fVolume );
			if( !dwIndex )
			{
				Eff2Ut_TOFILE( "AgcdEffSound::bPlaySound_Failed.txt", Eff2Ut_FmtMsg("szSound : %s PlaySampleSound failed\n", szSound) );
			}

			m_ulSoundLen = AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->GetSampleSoundLength( dwLoopCnt < 1 ? SOUND_TYPE_2D_SOUND_LOOPED : SOUND_TYPE_2D_SOUND_LOOPED, szSound );
		}
		break;
	case EFFECT_BGM_SOUND:		
		Eff2Ut_TOFILE( "AgcdEffSound::bPlaySound_Failed.txt", Eff2Ut_FmtMsg("szSound : %s  type = EFFECT_BGM_SOUND\n", szSound) );
		break;
	}

	return dwIndex;
}

RwInt32 AgcdEffSound::bPosUpdateFor3DSound(RwUInt32 dwIndex, const RwV3d* pV3dPos, INT32	iWeight)
{
	PROFILE("bPosUpdateFor3DSound");

	AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->OnUpdate3DSoundPosition(dwIndex,pV3dPos,iWeight);
	return 0;
}

RwInt32 AgcdEffSound::bToFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tToFile(fp);
	ir += fwrite( &m_eSoundType, 1, sizeof(m_eSoundType)+sizeof(m_szSoundFName)+sizeof(m_szMonoFName)+sizeof(m_dwLoopCnt)+sizeof(m_fVolume), fp );
	ir += AgcdEffBase::tToFileVariableData(fp);
	return ir;
}

RwInt32 AgcdEffSound::bFromFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tFromFile(fp);

	ir += fread( &m_eSoundType, 1, sizeof(m_eSoundType)+sizeof(m_szSoundFName)+sizeof(m_szMonoFName)+sizeof(m_dwLoopCnt)+sizeof(m_fVolume), fp );
	RwInt32 ir2 = AgcdEffBase::tFromFileVariableData(fp);
	if( T_ISMINUS4( ir2 ) )
	{
		ASSERT( !"AgcdEffBase::tToFileVariableData failed" );
		return -1;
	}

	return (ir+ir2);
}

#ifdef USE_MFC
INT32 AgcdEffSound::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_SOUND );
	if( T_ISMINUS4( AgcdEffBase::bForTool_Clone( pEffBase ) ) )
		return -1;

	AgcdEffSound* pEffSound = static_cast<AgcdEffSound*>(pEffBase);
	m_eSoundType = pEffSound->m_eSoundType;
	strcpy( m_szSoundFName, pEffSound->m_szSoundFName );
	strcpy( m_szMonoFName, pEffSound->m_szMonoFName );
	m_dwLoopCnt		= pEffSound->m_dwLoopCnt;
	m_fVolume		= pEffSound->m_fVolume;
	m_ulSoundLen	= pEffSound->m_ulSoundLen;

	return 0;
};
#endif//USE_MFC