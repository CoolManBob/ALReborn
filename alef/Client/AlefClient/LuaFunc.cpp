#include "LuaFunc.h"
#include "MyEngine.h"

LuaGlue	LG_MessageBox( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	const char *pStr = pLua->GetStringArgument( 1 , "*empty*" );

	//::MessageBox( NULL , pStr , "Lua Console Text" , MB_OK );
	g_pEngine->LuaErrorMessage( pStr );

	return 0;
}

LuaGlue	LG_AddCharacter( lua_State *L )
{
	static	INT32	_sClientEventCharacterIndex = 0x0FF00001;
	AuLua * pLua = AuLua::GetPointer();

	// 케릭터 추가 작업..

	INT32	nTID		= ( INT32 ) pLua->GetNumberArgument( 1 , 1 /*남자궁수*/);
	const char *pName	= pLua->GetStringArgument( 2 , "Agent Smith" );

	AgpdCharacter * pCharacter = NULL;

	INT32	nRetryMax = 1000;
	while( !pCharacter && nRetryMax-- )
	{
		pCharacter = g_pcsAgpmCharacter->AddCharacter( _sClientEventCharacterIndex , nTID , ( char * ) pName );
		
		if( _sClientEventCharacterIndex >= 0x0FFFFFFF )	_sClientEventCharacterIndex = 0x0FF00001;
		else											_sClientEventCharacterIndex++;
	}

	if( NULL == pCharacter )
	{
		// 0이면 에러를 리턴함.
		pLua->PushNumber( 0 );
		return 1;
	}

	pCharacter->m_unCurrentStatus	= AGPDCHAR_STATUS_IN_GAME_WORLD;

	// 기본 이동 속도 지정.
	g_pcsAgpmFactors->SetValue(&pCharacter->m_csFactor, 6000, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

	// 위치 조절
	{
		AgpdCharacter * pSelfCharacter = g_pcsAgcmCharacter->GetSelfCharacter();
		if( pSelfCharacter )
		{
			AuPOS pos = pSelfCharacter->m_stPos;
			//pos.x += 400.0f; 

			g_pcsAgpmCharacter->UpdatePosition	( pCharacter , &pos ); 
		}
	}
		
	// Face 바꾸기

	// Hair 바꾸기

	// 탈것

	// Equip

	g_pcsAgpmCharacter->AddCharacterToMap(pCharacter);
	g_pcsAgpmCharacter->UpdateInit( pCharacter );

	pLua->PushNumber( (double) pCharacter->m_lID );
	return 1;
}

LuaGlue	LG_UpdateCustomizeIndex( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID	= ( INT32 ) pLua->GetNumberArgument( 1 , -1 );
	INT32	nFace	= ( INT32 ) pLua->GetNumberArgument( 2 , -1 );
	INT32	nHair	= ( INT32 ) pLua->GetNumberArgument( 3 , -1 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter	||
		nFace == -1			||
		nHair == -1			)
	{
		g_pEngine->LuaErrorMessage( "Parameter Error" );
		LUA_RETURN( FALSE )
	}

	if( !g_pcsAgpmCharacter->UpdateCustomizeIndex( pCharacter , nFace , nHair ) )
	{
		g_pEngine->LuaErrorMessage( "Parameter Error" );
		LUA_RETURN( FALSE )
	}

	LUA_RETURN( TRUE )
}

LuaGlue	LG_StartAnimation( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );
	INT32	nAnimation	= ( INT32 ) pLua->GetNumberArgument( 2 , 0 );
	INT32	nType		= ( INT32 ) pLua->GetNumberArgument( 3 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter || nAnimation >= AGCMCHAR_MAX_ANIM_TYPE )
	{
		return 0;
	}

	g_pcsAgcmCharacter->StopAnimation( pCharacter );
	g_pcsAgcmCharacter->StartAnimation( pCharacter , ( AgcmCharacterAnimType ) nAnimation , 0 , AGCMCHAR_START_ANIM_CB_POINT_DEFAULT , nType );

	return 0;
}


LuaGlue	LG_Struck( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );
	INT32	nCID2		= ( INT32 ) pLua->GetNumberArgument( 2 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter  )
	{
		return 0;
	}

	AgcdCharacter * pstAgcdCharacter = g_pcsAgcmCharacter->GetCharacterData(pCharacter);
	pstAgcdCharacter->m_lAttackerID = nCID2;

	pstAgcdCharacter->m_bForceAnimation = TRUE;
	g_pcsAgcmCharacter->StartAnimation(pCharacter, AGCMCHAR_ANIM_TYPE_STRUCK);
	pstAgcdCharacter->m_bForceAnimation = FALSE;

	return 0;
}

LuaGlue	LG_SocialAnimation( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );
	INT32	nAnimation	= ( INT32 ) pLua->GetNumberArgument( 2 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter || nAnimation >= AGPDCHAR_SOCIAL_TYPE_MAX )
	{
		return 0;
	}

	g_pcsAgpmCharacter->OnOperationSocialAnimation( nCID , ( UINT8 ) nAnimation );
	return 0;
}

LuaGlue	LG_SetAttackStatus( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter )return 0;

	AgcdCharacter * pcdCharacter = g_pcsAgcmCharacter->GetCharacterData( pCharacter );
	g_pcsAgcmCharacter->SetAttackStatus( pcdCharacter );

	return 0;
}
LuaGlue	LG_SetStruckAnimation( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter )return 0;

	pCharacter->m_alTargetCID[0] = g_pcsAgcmCharacter->GetSelfCharacter()->m_lID;
	AgcdCharacter * pcdCharacter = g_pcsAgcmCharacter->GetCharacterData( pCharacter );
	g_pcsAgcmCharacter->SetStruckAnimation( pCharacter , pcdCharacter );

	return 0;
}

LuaGlue	LG_SetDeadAnimation( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter )return 0;

	AgcdCharacter * pcdCharacter = g_pcsAgcmCharacter->GetCharacterData( pCharacter );
	g_pcsAgcmCharacter->SetDeadAnimation( pCharacter , pcdCharacter );

	return 0;
}

LuaGlue	LG_RemoveCharacter( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter )
	{
		return 0;
	}

	g_pcsAgpmCharacter->RemoveCharacter( pCharacter->m_lID );
	return 0;
}

LuaGlue	LG_GetSelfCharacter( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	lID = 0;
	AgpdCharacter * pSelfCharacter = g_pcsAgcmCharacter->GetSelfCharacter();
	if( pSelfCharacter )
	{
		lID = pSelfCharacter->m_lID;
	}

	LUA_RETURN( lID )
}

LuaGlue	LG_GetCharacter( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	const char *pName = pLua->GetStringArgument( 1 , "*empty*" );

	INT32	lID = 0;
	AgpdCharacter * pCharacter = g_pcsAgpmCharacter->GetCharacter( ( char * ) pName );

	if( pCharacter )
	{
		lID = pCharacter->m_lID;
	}

	LUA_RETURN( lID )
}

LuaGlue	LG_MoveCharacter( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AuPOS	pos;
	pos.x	= ( FLOAT ) pLua->GetNumberArgument( 2 , 0 );
	pos.z	= ( FLOAT ) pLua->GetNumberArgument( 3 , 0 );
	pos.y	= ( FLOAT ) pLua->GetNumberArgument( 4 , SECTOR_MAX_HEIGHT );
	
	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter )
	{
		return 0;
	}

	//AuPOS	pos = pCharacter->m_stPos;
	//static int _sCount = 0;
	//switch( _sCount++ % 4  )
	//{
	//case 0:	pos.x += 400.0f; break;
	//case 1:	pos.z += 400.0f; break;
	//case 2:	pos.x -= 400.0f; break;
	//case 3:	pos.z -= 400.0f; break;
	//}

	g_pcsAgpmCharacter->MoveCharacter( pCharacter , &pos ,  MD_NODIRECTION, FALSE, TRUE  );
	return 0;
}

LuaGlue	LG_UpdatePosition( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );
	if( pCharacter )
	{
		pCharacter->m_stPos.x	= ( FLOAT ) pLua->GetNumberArgument( 2 , 0 );
		pCharacter->m_stPos.z	= ( FLOAT ) pLua->GetNumberArgument( 3 , 0 );
		pCharacter->m_stPos.y	= ( FLOAT ) pLua->GetNumberArgument( 4 , SECTOR_MAX_HEIGHT );

		g_pcsAgpmCharacter->UpdatePosition( pCharacter , &pCharacter->m_stPos );
	}

	return 0;
}

LuaGlue	LG_TurnCharacter( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	FLOAT	fDegreeX	= ( FLOAT ) pLua->GetNumberArgument( 2 , 0 );
	FLOAT	fDegreeY	= ( FLOAT ) pLua->GetNumberArgument( 3 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );
	if( pCharacter )
	{
		g_pcsAgpmCharacter->TurnCharacter( pCharacter , fDegreeX , fDegreeY );
	}

	return 0;
}

LuaGlue	LG_GetSelfCharacterPos( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	AgpdCharacter * pSelfCharacter = g_pcsAgcmCharacter->GetSelfCharacter();
	if( pSelfCharacter )
	{
		pLua->PushNumber( pSelfCharacter->m_stPos.x );
		pLua->PushNumber( pSelfCharacter->m_stPos.y );
		pLua->PushNumber( pSelfCharacter->m_stPos.z );
	}
	else
	{
		pLua->PushNumber( 0 );
		pLua->PushNumber( 0 );
		pLua->PushNumber( 0 );
	}
	return 3;
}

LuaGlue	LG_GetCharacterPos( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( pCharacter )
	{
		pLua->PushNumber( pCharacter->m_stPos.x );
		pLua->PushNumber( pCharacter->m_stPos.y );
		pLua->PushNumber( pCharacter->m_stPos.z );
	}
	else
	{
		pLua->PushNumber( 0 );
		pLua->PushNumber( 0 );
		pLua->PushNumber( 0 );
	}
	return 3;
}

LuaGlue	LG_ShowCharacterPos( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( pCharacter )
	{
		char str[ 256 ];
		sprintf( str , "%s (%.0f,%.0f,%.0f)" , pCharacter->m_szID , pCharacter->m_stPos.x , pCharacter->m_stPos.y , pCharacter->m_stPos.z );
		g_pEngine->LuaErrorMessage( str );
	}
	else
	{
		g_pEngine->LuaErrorMessage( "Character Not Found" );
	}

	return 0;
}

LuaGlue	LG_StartTheaterMode( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	BOOL	bRemoveCharacter		= (BOOL) pLua->GetNumberArgument( 1 , 1 );

	AgpdCharacter *			pcsCharacter;
	AgcdCharacter *			pstCharacter;
	INT32					lIndex = 0;

	pcsCharacter = g_pcsAgpmCharacter->GetCharacter(g_pcsAgcmCharacter->m_lSelfCID);
	if (pcsCharacter)
	{
		pstCharacter = g_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

		if( bRemoveCharacter && pstCharacter )
			g_pcsAgcmRender->RemoveClumpFromWorld(pstCharacter->m_pClump);
		g_pEngine->SetRenderMode(TRUE, FALSE);
		if (g_pcsAgcmTargeting) g_pcsAgcmTargeting->m_bShowGlyph = FALSE;
	}

	return 0;
}

LuaGlue	LG_EndTheaterMode( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	AgpdCharacter *			pcsCharacter;
	AgcdCharacter *			pstCharacter;
	INT32					lIndex = 0;

	pcsCharacter = g_pcsAgpmCharacter->GetCharacter(g_pcsAgcmCharacter->m_lSelfCID);
	if (pcsCharacter)
	{
		pstCharacter = g_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

		if( pstCharacter )
		{
			g_pcsAgcmRender->RemoveClumpFromWorld(pstCharacter->m_pClump);
			g_pcsAgcmRender->AddClumpToWorld(pstCharacter->m_pClump);
			g_pcsAgcmRender->AddUpdateInfotoClump(pstCharacter->m_pClump, g_pcsAgcmCharacter, AgcmCharacter::CBUpdateCharacter,NULL,pcsCharacter,NULL);
			if (g_pcsAgcmShadow)
				g_pcsAgcmShadow->AddShadow(pstCharacter->m_pClump);
		}
		g_pEngine->SetRenderMode(TRUE, TRUE);
		if (g_pcsAgcmTargeting)
			g_pcsAgcmTargeting->m_bShowGlyph = TRUE;
	}

	return 0;
}



LuaGlue	LG_GotoSiegeZone( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	char strMessage[ 256 ];
	sprintf( strMessage , "/move %f,%f" , -352800.0f , -47752.0f );

	if( g_pcsAgcmChatting2 )
	{
		g_pcsAgcmChatting2->SendChatMessage( AGPDCHATTING_TYPE_NORMAL , strMessage , strlen( strMessage ) , NULL );
	}
	return 0;
}

// Lua Functions

LuaGlue	LG_StartCharPolyMorphByTID( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	// 스카이셋을 지정.

	INT32	nCID	= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );
	INT32	nTID	= ( INT32 ) pLua->GetNumberArgument( 2 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( nCID == 0 || nTID == 0 || NULL == pCharacter )
	{
		g_pEngine->LuaErrorMessage( "Parameter Error" );
	}

	//g_pcsAgcmCharacter->QueueCharPolyMorphByTID( pCharacter , nTID );
	g_pcsAgcmCharacter->StartCharPolyMorphByTID( pCharacter , nTID );

	LUA_RETURN( TRUE );
}

LuaGlue	LG_EndCharPolyMorph( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	// 스카이셋을 지정.

	INT32	nCID	= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AgpdCharacter * pCharacter;
	pCharacter = g_pcsAgpmCharacter->GetCharacter( nCID );

	if( nCID == 0 || NULL == pCharacter )
	{
		g_pEngine->LuaErrorMessage( "Parameter Error" );
	}

	g_pcsAgcmCharacter->EndCharPolyMorph( pCharacter );

	LUA_RETURN( TRUE );
}

LuaGlue	LG_AutoHunt( lua_State *L )
{
	//AuLua * pLua = AuLua::GetPointer();
	//g_MyEngine.m_pMainWindow->AutoHunt();
	return 0;
}

LuaGlue	LG_GetCharacterByTID( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nTID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );

	AgpdCharacter * pcsCharacter;
	AgpdCharacter * pCharacterFound = NULL;

	INT32	lIndex = 0;
	INT32	nFoundCount = 0;

	for (pcsCharacter = g_pcsAgpmCharacter->GetCharSequence(&lIndex);
		pcsCharacter;
		pcsCharacter = g_pcsAgpmCharacter->GetCharSequence(&lIndex))
	{
		// 케릭터들 돌면서 블러킹 정보 출력.
		AgpdCharacterTemplate	*pcsTemplate;
		pcsTemplate = pcsCharacter->m_pcsCharacterTemplate;
		if( pcsTemplate->m_lID == nTID )
		{
			pCharacterFound = pcsCharacter;

			nFoundCount ++;

			if( nFoundCount != 1 )
			{
				char	str[256];
				sprintf( str , "%d , %s , %d\n" , nTID , pcsTemplate->m_szTName , nFoundCount );
				OutputDebugString( str );
			}
		}
	}

	if( pCharacterFound )
	{
		LUA_RETURN( pCharacterFound->m_lID )
	}
	else
	{
		LUA_RETURN( 0 )
	}
}

LuaGlue	LG_SetTargetStruckEffect( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nTargetCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );
	INT32	nAttackerCID	= ( INT32 ) pLua->GetNumberArgument( 2 , 0 );

	AgpdCharacter * pCharacter	= g_pcsAgpmCharacter->GetCharacter( nTargetCID		);
	AgpdCharacter * pAttacker	= g_pcsAgpmCharacter->GetCharacter( nAttackerCID	);

	if( nTargetCID == 0		|| NULL == pCharacter		||
		NULL == pAttacker	|| pAttacker == pCharacter	)
	{
		g_pEngine->LuaErrorMessage( "Parameter Error" );
	}
	else
	{
		INT32	lDamageNormal	= 100;
		INT32	lTotalDamage	= 100;

		AgpdFactor csFactor;
		g_pcsAgpmFactors->InitFactor(&csFactor);

		// 여기서 위에서 나온 데미지를 실제 캐릭터의 status에 반영한다.
		AgpdFactor			*pcsUpdateFactorResult = (AgpdFactor *) g_pcsAgpmFactors->SetFactor(&csFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
		if (!pcsUpdateFactorResult)
		{
			g_pcsAgpmFactors->DestroyFactor(&csFactor);

			LUA_RETURN( FALSE )
		}

		g_pcsAgpmFactors->SetValue(pcsUpdateFactorResult, (-lDamageNormal), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);
		g_pcsAgpmFactors->SetValue(pcsUpdateFactorResult, (-lTotalDamage), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		
		g_pcsAgpmCharacter->ActionCharacter(
			AGPDCHAR_ACTION_TYPE_ATTACK ,
			pAttacker					,
			pCharacter					,
			NULL						,	// Target Position
			-1							,	// bForce
			5							,	// Action Result Type
			pcsUpdateFactorResult		,	// Factor Damage
			1							,	// Combo Info
			0							,	// Additional Info
			0								// Hit Index
		);

		g_pcsAgcmCharacter->EnumCallback(AGCMCHAR_CB_ID_ATTACK_SUCESS, pAttacker, pCharacter);
	}
	
	LUA_RETURN( TRUE )
}

LuaGlue	LG_UseGG	( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	const char *pName = pLua->GetStringArgument( 1 , "-1" );

	INT32 nIndex = atoi( pName );
	bool bNoArgument = !strcmp( pName , "-1" ) ? true : false;

	/*
	struct	MapPosition
	{
		float	x;
		float	z;
		string	name;
	};

	MapPosition	aMapPos[] = {
		{ 759174.0f , -768531.0f , "Programmer Zone" } ,	// Programmer Zone
		{ -352800.0f , -47752.0f , "Siege Area" } , // 공성지역
		{ 760400.0f , 769200.0f , "Teranoa" } ,	// 테라노아
		{ 0.0f , 0.0f , "Empty" } 
	};
	*/

	TeleportInfo * pPoint = NULL;
	if( nIndex == 0 )
	{
		if( !strcmp( pName , "0" ) )
			pPoint = g_cMyEngine.GetTeleportPoint( nIndex );
		else
			pPoint = g_cMyEngine.GetTeleportPoint( pName );
	}
	else
	{
		pPoint = g_cMyEngine.GetTeleportPoint( nIndex );
	}

	if( !pPoint )
	{
		char strMessage[ 256 ];
		int	nDisplay = 0;

		map< INT32 , TeleportInfo >::iterator	iter;

		// 문자열 검색..
		for( iter = g_cMyEngine.m_mapTeleportPoint.begin() ;
			iter != g_cMyEngine.m_mapTeleportPoint.end();
			iter++ )
		{
			INT32			nIndex	= iter->first	;
			TeleportInfo	&stInfo	= iter->second	;

			if( ( !bNoArgument && strstr( stInfo.strName.c_str() , pName ) ) ||
				bNoArgument )
			{
				sprintf( strMessage , "%d to '%s'" , nIndex , stInfo.strName.c_str() );
				g_pEngine->LuaErrorMessage( strMessage );
				nDisplay++;
			}
		}

		// 처음 2바이트 검색
		if( !nDisplay )
			for( iter = g_cMyEngine.m_mapTeleportPoint.begin() ;
				iter != g_cMyEngine.m_mapTeleportPoint.end();
				iter++ )
			{
				INT32			nIndex	= iter->first	;
				TeleportInfo	&stInfo	= iter->second	;

				if( ( !bNoArgument && !strncmp( stInfo.strName.c_str() , pName , 2 ) ) ||
					bNoArgument )
				{
					sprintf( strMessage , "%d to '%s'" , nIndex , stInfo.strName.c_str() );
					g_pEngine->LuaErrorMessage( strMessage );
					nDisplay++;
				}
			}

		if( !nDisplay )
		{
			g_pEngine->LuaErrorMessage( "표시할 포인트가 없습니다" );
		}
	}
	else
	{
		char strMessage[ 256 ];

		sprintf( strMessage , "Move to '%s'" , pPoint->strName.c_str() );
		g_pEngine->LuaErrorMessage( strMessage );

		sprintf( strMessage , "/move %f,%f" , pPoint->pos.x , pPoint->pos.z );

		if( g_pcsAgcmChatting2 )
		{
			g_pcsAgcmChatting2->SendChatMessage( AGPDCHATTING_TYPE_NORMAL , strMessage , strlen( strMessage ) , NULL );
		}
	}

	return 0;
}

LuaGlue	LG_SaveGG	( lua_State *L )
{
	g_cMyEngine.SaveTelportPoint();
	g_pEngine->LuaErrorMessage( "GG Point Saved" );

	return 0;
}

LuaGlue	LG_LoadGG	( lua_State *L )
{
	g_cMyEngine.LoadTelportPoint();
	g_pEngine->LuaErrorMessage( "GG Point Loaded" );

	return 0;
}

LuaGlue	LG_AddGG	( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	const char	strEmpty[] = "<empty>";
	const char *pName = pLua->GetStringArgument( 1 , strEmpty );

	INT32	nX	= ( INT32 ) pLua->GetNumberArgument( 2 , 0 );
	INT32	nZ	= ( INT32 ) pLua->GetNumberArgument( 3 , 0 );

	string	strName = pName;

	if( !strcmp( strEmpty , pName ) )
	{
		// 이름이 없으면 리젼 네임 사용함.
		AgpdCharacter * pSelfCharacter = g_pcsAgcmCharacter->GetSelfCharacter();
		if( pSelfCharacter )
		{
			ApmMap::RegionTemplate * pTemplate = g_pcsApmMap->GetTemplate( pSelfCharacter->m_nBindingRegionIndex );

			if( pTemplate )
			{
				strName	= pTemplate->pStrName;
			}
		}
	}

	// 이름이 있으면 이름쓰고 없으면 리젼네임 사용함.
	if( nX == 0 && nZ == 0 )
	{
		// 좌표가 0 이면 셀프케릭터 좌표를 사용함.
		AgpdCharacter * pSelfCharacter = g_pcsAgcmCharacter->GetSelfCharacter();
		if( pSelfCharacter )
		{
			AuPOS pos = pSelfCharacter->m_stPos;
			nX = ( INT32 ) pos.x;
			nZ = ( INT32 ) pos.z;
		}
	}

	g_cMyEngine.AddTeleportPoint( g_cMyEngine.GetTeleportEmptyIndex() , TeleportInfo( strName.c_str() , (FLOAT) nX , (FLOAT) nZ ) );

	return 0;
}

LuaGlue	LG_DeleteGG	( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	const char *pName = pLua->GetStringArgument( 1 , "" );

	INT32 nIndex = atoi( pName );

	BOOL bRet = FALSE;
	if( nIndex == 0 )
	{
		bRet = g_cMyEngine.DeleteTeleportPoint( pName );
	}
	else
	{
		bRet = g_cMyEngine.DeleteTeleportPoint( nIndex );
	}

	if( !bRet )
	{
		g_pEngine->LuaErrorMessage( "지울 포인트가 없습니다." );
	}
	else
	{
		g_pEngine->LuaErrorMessage( "포인트를 삭제하였습니다." );
	}

	return 0;
}
