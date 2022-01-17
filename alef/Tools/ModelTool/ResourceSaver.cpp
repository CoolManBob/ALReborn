#include "stdafx.h"
#include "ResourceSaver.h"
#include "AgcEngineChild.h"

//---------------------- Global Data -----------------------
char* szClumpDir[eSaveClumpMax] = {
	"%s\\CHARACTER\\%s",
	"%s\\OBJECT\\%s",
	"%s\\EFFECT\\CLUMP\\%s"
};

char* szTextureDir[6] = {
	"%s\\TEXTURE\\CHARACTER\\%s.bmp",
	"%s\\TEXTURE\\CHARACTER\\%s.tif",
	"%s\\TEXTURE\\OBJECT\\%s.bmp",
	"%s\\TEXTURE\\OBJECT\\%s.tif",
	"%s\\TEXTURE\\EFFECT\\%s.bmp",
	"%s\\TEXTURE\\EFFECT\\%s.tif"
};

//---------------------- CResourceSaver -----------------------
CResourceSaver::CResourceSaver() : 
 m_pEngine(NULL),
 m_pPublicCharacterMng(NULL),
 m_pClientCharacterMng(NULL),
 m_pLodMng(NULL),
 m_pPublicSkillMng(NULL),
 m_pClientSkillMng(NULL),
 m_pClientEffectMng(NULL)
{
	::GetCurrentDirectory( 1024, m_szDir );
}

CResourceSaver::~CResourceSaver()
{
}

BOOL	CResourceSaver::Initialize( AgcEngineChild* pEngine )
{
	ASSERT( pEngine );

	m_pEngine				= pEngine;
	m_pPublicCharacterMng	= pEngine->GetAgpmCharacterModule();
	m_pClientCharacterMng	= pEngine->GetAgcmCharacterModule();
	m_pPublicObjectMng		= pEngine->GetApmObjectModule();
	m_pClientObjectMng		= pEngine->GetAgcmObjectModule();
	m_pPublicItemMng		= pEngine->GetAgpmItemModule();
	m_pClientItemMng		= pEngine->GetAgcmItemModule();
	m_pLodMng				= pEngine->GetAgcmPreLODManagerModule();
	m_pPublicSkillMng		= pEngine->GetAgpmSkillModule();
	m_pClientSkillMng		= pEngine->GetAgcmSkillModule();
	m_pClientEffectMng		= pEngine->GetAgcmEventEffectModule();
	m_pClientAnimationMng	= m_pClientCharacterMng->GetAgcaAnimation2();

	return TRUE;	
}

void	CResourceSaver::Destory()
{

}

BOOL	CResourceSaver::Create( int nID, eResourceSaver eMode )
{
	ASSERT( m_pEngine );

	char szSaveDir[1024];
	switch( eMode )
	{
	case eResourceSaverCharacter:
		{
			AgpdCharacterTemplate* pPublicTemplate = m_pPublicCharacterMng->GetCharacterTemplate( nID );
			if( !pPublicTemplate )		return FALSE;

			AgcdCharacterTemplate* pClientTemplate = m_pClientCharacterMng->GetTemplateData( pPublicTemplate );
			if( !pClientTemplate  )		return FALSE;

			CreateRoot( pPublicTemplate->m_szTName, szSaveDir );

			//Ini저장
			SaveIni( nID, m_szDir, szSaveDir, eResourceSaverCharacter );

			//Clump저장(Texture)
			SaveClumpFile( pClientTemplate->m_pcsPreData->m_pszDFFName, m_szDir, szSaveDir, eSaveClumpCharacter );
			SaveClumpFile( pClientTemplate->m_pcsPreData->m_pszDADFFName, m_szDir, szSaveDir, eSaveClumpCharacter );
			SaveClumpFile( pClientTemplate->m_pcsPreData->m_pszPickDFFName, m_szDir, szSaveDir, eSaveClumpCharacter );
			SaveLod( (ApBase*)pPublicTemplate, m_szDir, szSaveDir, eSaveClumpCharacter );
			SaveAnimation( pClientTemplate, m_szDir, szSaveDir );
			SaveSkill( pPublicTemplate, pClientTemplate, m_szDir, szSaveDir );
			SaveEffect( (ApBase*)pPublicTemplate, m_szDir, szSaveDir );

			//Character들이 들고있는 기본 Item정보 추가
			AgpdItemADCharTemplate* pPublicItemADCharTemplate = m_pPublicItemMng->GetADCharacterTemplate( pPublicTemplate );
			if( !pPublicItemADCharTemplate )	return FALSE;
			
			for( int lPartID = AGPMITEM_PART_BODY; lPartID < AGPMITEM_PART_NUM; ++lPartID )
			{
				int nTID = pPublicItemADCharTemplate->m_lDefaultEquipITID[lPartID];
				if( nTID )
					Create( nTID, eResourceSaverItem );
			}

			MessageBox( pPublicTemplate->m_szTName );
		}
		break;
	case eResourceSaverObject:
		{
			ApdObjectTemplate* pPublicTemplate = m_pPublicObjectMng->GetObjectTemplate( nID );
			if( !pPublicTemplate )		return FALSE;

			AgcdObjectTemplate* pClientTemplate = m_pClientObjectMng->GetTemplateData( pPublicTemplate );
			if( !pClientTemplate )		return FALSE;

			CreateRoot( pPublicTemplate->m_szName, szSaveDir );

			//Ini저장
			SaveIni( nID, m_szDir, szSaveDir, eResourceSaverObject );

			SaveClumpFile( pClientTemplate->m_szCollisionDFFName, m_szDir, szSaveDir, eSaveClumpObject );
			SaveClumpFile( pClientTemplate->m_szPickDFFName, m_szDir, szSaveDir, eSaveClumpObject );

			SaveObjectGroup( pClientTemplate->m_stGroup.m_pstList, m_szDir, szSaveDir );
			//SaveClumpFile( pClientTemplate->m_stGroup.m_pstList->m_csData.m_pszDFFName, m_szDir, szSaveDir, eSaveClumpObject );
			SaveLod( (ApBase*)pPublicTemplate, m_szDir, szSaveDir, eSaveClumpObject );
			SaveEffect( (ApBase*)pPublicTemplate, m_szDir, szSaveDir  );

			MessageBox( pPublicTemplate->m_szName );
		}
		break;
	case eResourceSaverItem:
		{
			AgpdItemTemplate* pPublicTemplate = m_pPublicItemMng->GetItemTemplate( nID );
			if( !pPublicTemplate )		return FALSE;
				
			AgcdItemTemplate* pClientTemplate = m_pClientItemMng->GetTemplateData( pPublicTemplate );
			if( !pClientTemplate )		return FALSE;

			CreateRoot( pPublicTemplate->m_szName, szSaveDir );

			SaveClumpFile( pClientTemplate->m_pcsPreData->m_pszBaseDFFName, m_szDir, szSaveDir, eSaveClumpCharacter );
			SaveClumpFile( pClientTemplate->m_pcsPreData->m_pszSecondDFFName, m_szDir, szSaveDir, eSaveClumpCharacter );
			SaveClumpFile( pClientTemplate->m_pcsPreData->m_pszFieldDFFName, m_szDir, szSaveDir, eSaveClumpCharacter );
			SaveClumpFile( pClientTemplate->m_pcsPreData->m_pszPickDFFName, m_szDir, szSaveDir, eSaveClumpCharacter );

			SaveFile( pClientTemplate->m_pcsPreData->m_pszTextureName, m_szDir, szSaveDir, "%s\\TEXTURE\\ITEM\\%s" );
			SaveFile( pClientTemplate->m_pcsPreData->m_pszSmallTextureName, m_szDir, szSaveDir, "%s\\TEXTURE\\ITEM\\%s" );
			SaveFile( pClientTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName, m_szDir, szSaveDir, "%s\\TEXTURE\\ITEM\\%s" );
			SaveFile( pClientTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName, m_szDir, szSaveDir, "%s\\TEXTURE\\ITEM\\%s" );

			SaveLod( (ApBase*)pPublicTemplate, m_szDir, szSaveDir, eSaveClumpObject );
			SaveEffect( (ApBase*)pPublicTemplate, m_szDir, szSaveDir  );

			MessageBox( pPublicTemplate->m_szName );
		}
		break;
	}

	return TRUE;
}

BOOL CResourceSaver::CreateRoot( char* szName, char* szSaveDir )
{
	//각각의 폴더생성
	sprintf( szSaveDir, "%s\\%s", m_szDir, szName );
	CreateDirectory( szSaveDir, NULL );
	CreateFolder( szSaveDir );

	return TRUE;
}

BOOL CResourceSaver::CreateFolder( char* szSaveDir )
{
	char szFolder[1024];
	sprintf( szFolder, "%s\\CHARACTER", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\CHARACTER\\ANIMATION", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\OBJECT", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\OBJECT\\ANIMATION", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\EFFECT", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\EFFECT\\ANIMATION", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\EFFECT\\CLUMP", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\EFFECT\\INI", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\TEXTURE", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\TEXTURE\\CHARACTER", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\TEXTURE\\OBJECT", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\TEXTURE\\EFFECT", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\TEXTURE\\ITEM", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\SOUND", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\SOUND\\EFFECT", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\INI", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\INI\\CharacterTemplate", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	sprintf( szFolder, "%s\\INI\\ObjectTemplate", szSaveDir );
	if( !CreateDirectory( szFolder, NULL ) )
		return FALSE;

	return TRUE;
}

void CResourceSaver::SaveObjectGroup( AgcdObjectTemplateGroupList* pStart, char* szSrcDir, char* szDstDir )
{
	if( !pStart )	return;

	char szSrc[1024], szDst[1024];
	AgcdObjectTemplateGroupList* pstList = pStart;
	while( pstList )
	{
		const AgcdObjectTemplateGroupData& cGroupData = pstList->m_csData;

		SaveClumpFile( cGroupData.m_pszDFFName, szSrcDir, szDstDir, eSaveClumpObject );
		if( cGroupData.m_pstClump && cGroupData.m_pstClump->szName && cGroupData.m_pstClump->szName[0] )
			SaveClumpFile( cGroupData.m_pstClump->szName, szSrcDir, szDstDir, eSaveClumpObject );

		if( cGroupData.m_pcsAnimation &&
			cGroupData.m_pcsAnimation->m_pcsHead &&
			cGroupData.m_pcsAnimation->m_pcsHead->m_pszRtAnimName )
		{
			sprintf( szSrc, "%s\\OBJECT\\ANIMATION\\%s", szSrcDir, cGroupData.m_pcsAnimation->m_pcsHead->m_pszRtAnimName );
			sprintf( szDst, "%s\\OBJECT\\ANIMATION\\%s", szDstDir, cGroupData.m_pcsAnimation->m_pcsHead->m_pszRtAnimName );
			CopyFile( szSrc, szDst, NULL );
		}

		pstList = pstList->m_pstNext;
	}
}

void CResourceSaver::SaveIni( int nID, char* szSrcDir, char* szDstDir, eResourceSaver eMode )
{
	//ini저장
	char szSrcFolder[1024], szSrc[1024], szDst[1024];
	switch( eMode )
	{
	case eResourceSaverCharacter:
		sprintf( szSrcFolder, "%s\\INI\\CharacterTemplate\\*.*", szSrcDir );
		sprintf( szSrc, "%s\\INI\\CharacterTemplate\\", szSrcDir );
		sprintf( szDst, "%s\\INI\\CharacterTemplate\\", szDstDir );
		break;
	case eResourceSaverItem:
		return;
	case eResourceSaverObject:
		sprintf( szSrcFolder, "%s\\INI\\ObjectTemplate\\*.*", szSrcDir, nID );
		sprintf( szSrc, "%s\\INI\\ObjectTemplate\\", szSrcDir );
		sprintf( szDst, "%s\\INI\\ObjectTemplate\\", szDstDir );
		break;
	}

	char szFileName[16];
	sprintf( szFileName, "%06d", nID );

	WIN32_FIND_DATA	wfd;
	HANDLE	hList = FindFirstFile( szSrcFolder, &wfd );
	while( 1 )
	{
		if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
		}
		else
		{
			if( !strncmp( szFileName, wfd.cFileName, 6 ) )
			{
				strcat( szSrc, wfd.cFileName );
				strcat( szDst, wfd.cFileName );
				CopyFile( szSrc, szDst, NULL );
				return;
			}
		}

		if( !FindNextFile( hList, &wfd ) )
			break;
	}
}

void CResourceSaver::SaveAnimation( AgcdCharacterTemplate* pClientTemplate, char* szSrcDir, char* szDstDir )
{
	char szSrc[1024], szDst[1024];
	INT32			lMaxAnimType2		= m_pClientCharacterMng->GetAnimType2Num(pClientTemplate);
	for ( INT32 lType = 0; lType < AGCMCHAR_MAX_ANIM_TYPE; ++lType )
	{
		for(INT32 lType2 = 0; lType2 < lMaxAnimType2; ++lType2)
		{
			if( !pClientTemplate->m_pacsAnimationData[lType][lType2] || !pClientTemplate->m_pacsAnimationData[lType][lType2]->m_pcsAnimation )
				continue;

			AgcdAnimData2* pcsCurrentAnimData = pClientTemplate->m_pacsAnimationData[lType][lType2]->m_pcsAnimation->m_pcsHead;
			while (pcsCurrentAnimData)
			{
				sprintf( szSrc, "%s\\CHARACTER\\ANIMATION\\%s", szSrcDir, pcsCurrentAnimData->m_pszRtAnimName );
				sprintf( szDst, "%s\\CHARACTER\\ANIMATION\\%s", szDstDir, pcsCurrentAnimData->m_pszRtAnimName );
				CopyFile( szSrc, szDst, NULL );

				SaveAniSound( pcsCurrentAnimData, szSrcDir, szDstDir );

				pcsCurrentAnimData	= pcsCurrentAnimData->m_pcsNext;
			}
		}
	}
}


void CResourceSaver::SaveSkill( AgpdCharacterTemplate* pPublicTemplate, AgcdCharacterTemplate* pClientTemplate, char* szSrcDir, char* szDstDir )
{
	AgpdSkillTemplateAttachData* pPublicSkillTemplate	= m_pPublicSkillMng->GetAttachSkillTemplateData( (ApBase*)pPublicTemplate );
	AgcdSkillAttachTemplateData* pClientSkillTemplate	= m_pClientSkillMng->GetAttachTemplateData( (ApBase*)pPublicTemplate );

	char szSrc[1024], szDst[1024];
	INT32		lNumAnimType2   = m_pClientCharacterMng->GetAnimType2Num( pClientTemplate );
	for ( INT32 i = 0; i < GetNumVisualInfo( pClientTemplate ); ++i)
	{
		if( !pPublicSkillTemplate->m_aszUsableSkillTName[i] )
			continue;

		AgcmSkillVisualInfo* pInfo = pClientSkillTemplate->m_pacsSkillVisualInfo[i];
		if( !pInfo )
			continue;

		for ( INT32 lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
		{
			if(	pInfo->m_pacsAnimation[lAnimType2] &&
				pInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation &&
				pInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead &&
				pInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName )
			{
				//Animation 추가
				sprintf( szSrc, "%s\\CHARACTER\\ANIMATION\\%s", szSrcDir, pInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName );
				sprintf( szDst, "%s\\CHARACTER\\ANIMATION\\%s", szDstDir, pInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName );
				CopyFile( szSrc, szDst, NULL );
			}

			if( pInfo->m_pastEffect[lAnimType2] && 
				pInfo->m_pastEffect[lAnimType2]->m_ulConditionFlags )
			{
				AgcdUseEffectSetList* pcsCurrent	= pInfo->m_pastEffect[lAnimType2]->m_pcsHead;
				while (pcsCurrent)
				{
					if( !pcsCurrent->m_csData.m_ulEID && !pcsCurrent->m_csData.m_pszSoundName && !pcsCurrent->m_csData.m_ulConditionFlags )
					{
						pcsCurrent	= pcsCurrent->m_pcsNext;
						continue;
					}

					SaveEffectSound( pcsCurrent->m_csData.m_pszSoundName, szSrcDir, szDstDir );
					SaveEffectFile( pcsCurrent->m_csData.m_ulEID, szSrcDir, szDstDir );
					pcsCurrent	= pcsCurrent->m_pcsNext;
				}
			}
		}
	}
}

void CResourceSaver::SaveLod( ApBase* pBase, char* szSrcDir, char* szDstDir, eSaveClump eMode )
{
	AgcdPreLOD* pLod = m_pLodMng->GetPreLOD( pBase );
	if( pLod )
	{
		AgcdPreLODList* listLod = pLod->m_pstList;
		while( listLod )
		{
			for( int i = 0; i < 5; i++ )
				if( listLod->m_csData.m_aszData[i] && strlen( listLod->m_csData.m_aszData[i] ) )
					SaveClumpFile( listLod->m_csData.m_aszData[i], szSrcDir, szDstDir, eMode );

			listLod = listLod->m_pstNext;
		}
	}
}

void CResourceSaver::SaveEffect( ApBase* pBase, char* szSrcDir, char* szDstDir )
{
	AgcdUseEffectSet* pEffectSet = m_pClientEffectMng->GetUseEffectSet( pBase );
	if( pEffectSet )
	{
		AgcdUseEffectSetList* pcsCurrent = pEffectSet->m_pcsHead;
		while (pcsCurrent)
		{
			SaveEffectFile( pcsCurrent->m_csData.m_ulEID, szSrcDir, szDstDir );
			pcsCurrent	= pcsCurrent->m_pcsNext;
		}
	}
}

void CResourceSaver::SaveFile( char* szName, char* szCurDir, char* szSrcDir, char* szSprintf )
{
	if( !szCurDir || !szSrcDir || !szName || !szSprintf )					return;
	if( !szCurDir[0] || !szSrcDir[0] || !szName[0] || !szSprintf[0] )		return;

	char szSrc[1024], szDst[1024];
	sprintf( szSrc, szSprintf, szCurDir, szName );
	sprintf( szDst, szSprintf, szSrcDir, szName );
	CopyFile( szSrc, szDst, NULL );
}

void CResourceSaver::SaveClumpFile( char* szFilename, char* szCurDir, char* szSaveDir, eSaveClump eMode )
{
	if( !szFilename )				return;
	if(	!strlen( szFilename ) )		return;

	char szSrc[1024], szDst[1024];
	sprintf( szSrc, szClumpDir[eMode], m_szDir, szFilename );
	sprintf( szDst, szClumpDir[eMode], szSaveDir, szFilename );
	CopyFile( szSrc, szDst, TRUE );
	
	StringVec vecString;
	if( GetTextureName( szSrc, vecString ) )
	{
		for( StringVecItr Itr = vecString.begin(); Itr != vecString.end(); ++Itr )
		{
			for( int i = 0; i<6; ++i )
			{
				sprintf( szSrc, szTextureDir[i], m_szDir, (*Itr).c_str() );
				sprintf( szDst, szTextureDir[i], szSaveDir, (*Itr).c_str() );
				CopyFile( szSrc, szDst, TRUE );
			}
		}
	}

	sprintf( szSrc, szClumpDir[eMode], m_szDir, szFilename );
	sprintf( szDst, szClumpDir[eMode], szSaveDir, szFilename );
	if( GetEffectTextureName( szSrc, vecString ) )
	{
		for( StringVecItr Itr = vecString.begin(); Itr != vecString.end(); ++Itr )
		{
			for( int i = 0; i<6; ++i )
			{
				sprintf( szSrc, szTextureDir[i], m_szDir, (*Itr).c_str() );
				sprintf( szDst, szTextureDir[i], szSaveDir, (*Itr).c_str() );
				CopyFile( szSrc, szDst, TRUE );
			}
		}
	}
}

void CResourceSaver::SaveEffectFile( int nID, char* szCurDir, char* szSaveDir )
{
	//Effect저장

	char szSrc[1024], szDst[1024];
	sprintf( szSrc, "%s\\EFFECT\\INI\\NEW\\%d_eff.ini", szCurDir, nID );
	sprintf( szDst, "%s\\EFFECT\\INI\\%d_eff.ini", szSaveDir, nID );
	CopyFile( szSrc, szDst, TRUE );

	//Effect내부의 Texture  저장
	//Texture가 여러장 들어가던뎅 @@;

	char szTemp[512];
	std::ifstream stream( szSrc );
	if( !stream.is_open() )	return;
	
	int nPos;
	while( stream.getline( szTemp, 512 ) )
	{
		std::string strText( szTemp );
		if( strText.empty() )	continue;

		//Texture
		if( strText.find( "efftex" ) != std::string::npos )
		{
			if( ( nPos = strText.find( "=" ) ) != std::string::npos )
			{
				sprintf( szSrc, "%s\\TEXTURE\\EFFECT\\%s", szCurDir, strText.c_str() + nPos + 1 );
				sprintf( szDst, "%s\\TEXTURE\\EFFECT\\%s", szSaveDir, strText.c_str() + nPos + 1 );
				CopyFile( szSrc, szDst, TRUE );
			}
		}

		//Clump
		if( strText.find( "ClumpFile" ) != std::string::npos )
		{
			if( ( nPos = strText.find( "=" ) ) != std::string::npos )
				SaveClumpFile( (char*)strText.c_str() + nPos + 1, szCurDir, szSaveDir, eSaveClumpEffect );
		}

		//Animation(animSpline_file, animRtAnim_file )
		if( strText.find( "anim" ) != std::string::npos && strText.find( "_file" ) != std::string::npos )
		{
			if( ( nPos = strText.find( "=" ) ) != std::string::npos )
			{
				sprintf( szSrc, "%s\\EFFECT\\ANIMATION\\%s", szCurDir, strText.c_str() + nPos + 1 );
				sprintf( szDst, "%s\\EFFECT\\ANIMATION\\%s", szSaveDir, strText.c_str() + nPos + 1 );
				CopyFile( szSrc, szDst, TRUE );
			}
		}

		//Sound
		if( ( nPos = strText.find_first_of( "File=" ) ) >= 0 )
			SaveEffectSound( (char*)strText.c_str() + nPos, szCurDir, szSaveDir );
		if( ( nPos = strText.find_first_of( "MonoFile=" ) ) >= 0 )
			SaveEffectSound( (char*)strText.c_str() + nPos, szCurDir, szSaveDir );
	}
	stream.close();
}

void	CResourceSaver::SaveAniSound( AgcdAnimData2* pAniData, char* szSrcDir, char* szDstDir )
{
	if( !pAniData )	return;

	AEE_CharAnimAttachedData* pAEE = (AEE_CharAnimAttachedData*)m_pClientAnimationMng->GetAttachedData( AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME, pAniData );
	
	for( AEE_CharAnimAttachedData::SoundList::iterator	iter = pAEE->GetList().begin();
		iter != pAEE->GetList().end();
		iter ++ )
	{
		AEE_CharAnimAttachedSoundData	*pcsCurrent	= &*iter;
		SaveEffectSound( pcsCurrent->m_strSoundName.c_str() , szSrcDir, szDstDir );
	}	
}

void	CResourceSaver::SaveEffectSound( const char* szSoundName, const char* szSrcDir, const char* szDstDir )
{
	if( !szSoundName || !szSoundName[0] )	return;

	char szName[16];
	memset( szName, 0, sizeof( char ) * 16 );
	sprintf( szName, "%c%c.pk", szSoundName[0], szSoundName[1] );

	char szSrc[1024], szDst[1024];
	sprintf( szSrc, "%s\\SOUND\\EFFECT\\%s", szSrcDir, szName );
	sprintf( szDst, "%s\\SOUND\\EFFECT\\%s", szDstDir, szName );

	CopyFile( szSrc, szDst, TRUE );
}

int CResourceSaver::GetNumVisualInfo( AgcdCharacterTemplate* pTemplate )
{
	switch( pTemplate->m_lAnimType2 )
	{
	case AGCMCHAR_AT2_WARRIOR:
	case AGCMCHAR_AT2_ARCHER:
	case AGCMCHAR_AT2_WIZARD:
	case AGCMCHAR_AT2_ARCHLORD:
		return AGCDSKILL_MAX_VISUAL_INFO_PC;
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return AGCDSKILL_MAX_VISUAL_INFO_BOSS_MONSTER;
	}

	return 0;
}

static RpAtomic* GetTextureCB( RpAtomic* pRpAtomic, void* pData )
{
	if( !pRpAtomic )		return NULL;

	RpGeometry* pRpGeometry = RpAtomicGetGeometry( pRpAtomic );
	if( !pRpGeometry )		return NULL;
	
	int nGeom = RpGeometryGetNumMaterials( pRpGeometry );
	for( int i=0; i<nGeom; ++i )
	{
		RpMaterial* pMaterial =  RpGeometryGetMaterial( pRpGeometry, i );
		if( !pMaterial ) continue;

		RwTexture* pTexture = RpMaterialGetTexture( pMaterial );
		if( pTexture )
		{
			RwChar* name = RwTextureGetName(pTexture);
			if( name )
			{
				//StringVecItr Itr = ((StringVec*)pData)->begin();
				//for( ; Itr != ((StringVec*)pData)->end(); ++Itr )
				//	if( (*Itr) == name )
				//		continue;

				((StringVec*)pData)->push_back( name );
			}

			RwChar* pMaskName = RwTextureGetMaskName( pTexture );
			if( pMaskName && pMaskName[0] )
				((StringVec*)pData)->push_back( pMaskName );

			RwTexDictionary* pDic = RwTextureGetDictionary(pTexture);
			if( pDic )
			{
				RwChar* pDicName = RwTextureGetName( pDic->texture );
				if( pDicName || pDicName[0] )
					((StringVec*)pData)->push_back( pDicName );
			}
		}
	}

	return NULL;
}

BOOL	CResourceSaver::GetTextureName( char* szClump, StringVec& vecString )
{
	vecString.clear();

	RpClump* pRpClump = RWUTIL().LoadClump( szClump );
	if( !pRpClump )	return FALSE;

	RpClumpForAllAtomics( pRpClump, GetTextureCB, &vecString );
	RpClumpDestroy( pRpClump );
	return TRUE;
}

static void* AddParamTextuerNameCB( DxEffect* pEffect, RwUInt32 eType, RwChar* name, void* data, void* pData )
{
	RwChar guiName[FX_HANDLE_NAME_LENGTH];
	RwChar widget[FX_HANDLE_NAME_LENGTH];

	EffectGUIParams params;
	params.guiName	= guiName;
	params.widget	= widget;

	EffectParameterGetGUI( pEffect, name, &params );

	switch( eType )
	{
		case EFFECT_TWEAKABLE_PARAM_NATYPE:
		case EFFECT_TWEAKABLE_PARAM_FLOAT:
		case EFFECT_TWEAKABLE_PARAM_VECTOR:
		case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
			break;
		case EFFECT_TWEAKABLE_PARAM_TEXTURE:
		{
			RwTexture** pTexture = (RwTexture**)data;
			char* szName = (*pTexture) ? RwTextureGetName( *pTexture ) : NULL;
			if( szName && szName[0] )
			{
				//StringVecItr Itr = ((StringVec*)pData)->begin();
				//for( ; Itr != ((StringVec*)pData)->end(); ++Itr )
				//	if( (*Itr) == name )
				//		continue;

				((StringVec*)pData)->push_back( szName );
			}
			break;
		}
	}

	return pEffect;
}

static RpAtomic* GetEffectTextureNameCB( RpAtomic* pRpAtomic, void* pData )
{
	if( !pRpAtomic )		return NULL;

	RpGeometry* pRpGeometry = RpAtomicGetGeometry( pRpAtomic );
	if( !pRpGeometry )		return NULL;
	
	int nGeom = RpGeometryGetNumMaterials( pRpGeometry );
	for( int i=0; i<nGeom; ++i )
	{
		RpMaterial* pMaterial = RpGeometryGetMaterial( pRpGeometry, i );
		if( !pMaterial )		continue;

		RpMaterialD3DFxExt* pMaterialFX = MATERIAL_GET_FX_DATA( pMaterial );
		if( !pMaterialFX )			continue;
		if( !pMaterialFX->effect )	continue;
		EffectForAllTweakableParameters( pMaterialFX->effect, AddParamTextuerNameCB, pData );
	}

	return pRpAtomic;
}

BOOL	CResourceSaver::GetEffectTextureName( char* szClump, StringVec& vecString )
{
	vecString.clear();

	RpClump* pRpClump = RWUTIL().LoadClump( szClump );
	if( !pRpClump )	return FALSE;

	RpClumpForAllAtomics( pRpClump, GetEffectTextureNameCB, &vecString );
	RpClumpDestroy( pRpClump );
	return TRUE;
}

void	CResourceSaver::MessageBox( char* szName )
{
	if( szName )
	{
		char szNewName[256];
		sprintf( szNewName, "%s가 저장완료 되었습니다", szName );
		::MessageBox( NULL, szNewName, "Save", MB_OK );
	}
}