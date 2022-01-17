#include "AcUIRaider.h"
#include "AgcmMinimap.h"

AcUIRaider::AcUIRaider( VOID )
:	m_pRaiderCamera( NULL ),
	m_pRaiderTexture( NULL ),
	m_pMaskTex(NULL)
{
}

AcUIRaider::~AcUIRaider( VOID )
{

}

BOOL	AcUIRaider::OnInit( VOID )
{
	//_RenderCameraCreate();
	_PointTextureLoad();
	return TRUE;
}

BOOL	AcUIRaider::OnLButtonDown( RsMouseStatus *ms )
{
	this->pParent->OnLButtonDown( ms );
	return FALSE;
}

BOOL	AcUIRaider::OnLButtonUp( RsMouseStatus *ms )
{
	this->pParent->OnLButtonUp( ms );
	return FALSE;
}

BOOL	AcUIRaider::OnMouseMove( RsMouseStatus *ms )
{
	this->pParent->OnMouseMove( ms );
	return FALSE;
}

BOOL	AcUIRaider::OnRButtonUp( RsMouseStatus *ms )
{
	return TRUE;
}

VOID	AcUIRaider::RaiderPositionUpdate( VOID )
{
	if( NULL == this->m_pAgcmMinimap ) return;
	if( NULL == this->m_pAgcmMinimap->m_pcsSelfCharacter ) return;

	// 위치계산..
	m_nXPos	=	__GetRaiderPos( this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.x );
	m_nZPos	=	__GetRaiderPos( this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.z );    
}

BOOL		AcUIRaider::OnIdle( UINT32 ulClockCount )
{

	//RwRGBA	ClearColor;

	//ClearColor.alpha	=	0;
	//ClearColor.blue		=	255;
	//ClearColor.green	=	255;
	//ClearColor.red		=	255;


	//RwCameraClear( m_pRaiderCamera , &ClearColor, rwCAMERACLEARZ | rwCAMERACLEARSTENCIL | rwCAMERACLEARIMAGE );


	//_RenderRaider();
	

	return TRUE;
}

BOOL	AcUIRaider::_RenderCameraCreate( VOID )
{
	RwFrame*		pCameraFrame			=	NULL;
	RwRaster*		pCameraTextureRaster	=	NULL;
	RwRaster*		pCameraZBuffer			=	NULL;

	m_pRaiderCamera		=	RwCameraCreate();

	if( !m_pRaiderCamera )
		return FALSE;

	pCameraFrame		=	RwCameraGetFrame( g_pEngine->m_pCamera );
	if( !pCameraFrame )
		return FALSE;

	RwCameraSetFrame( m_pRaiderCamera , pCameraFrame );

	pCameraTextureRaster		=	RwRasterCreate( ACUIMINIMAP_TEXTURE_SIZE , ACUIMINIMAP_TEXTURE_SIZE , 0 , rwRASTERTYPECAMERATEXTURE );
	pCameraZBuffer				=	RwRasterCreate( ACUIMINIMAP_TEXTURE_SIZE , ACUIMINIMAP_TEXTURE_SIZE , 0 , rwRASTERTYPEZBUFFER );


	if( !pCameraTextureRaster && !pCameraZBuffer )
	{
		RwCameraSetFrame( m_pRaiderCamera , NULL );
		RwFrameDestroy( pCameraFrame );
		RwCameraDestroy( m_pRaiderCamera );
		return FALSE;
	}

	RwCameraSetRaster( m_pRaiderCamera , pCameraTextureRaster );
	RwCameraSetZRaster( m_pRaiderCamera , pCameraZBuffer );

	return TRUE;

}

VOID	AcUIRaider::OnClose( VOID )
{
	if( m_pRaiderTexture )
		RwTextureDestroy( m_pRaiderTexture );

	if( m_pRaiderCamera )
		RwCameraDestroy( m_pRaiderCamera );

}

VOID	AcUIRaider::OnWindowRender( VOID )
{

	_RenderCharacter();
	//_RenderMap();

}

VOID	AcUIRaider::_RenderRaider( VOID )
{
	if (this->m_pAgcmMinimap->m_pcsSelfCharacter == NULL)
		return;

	INT32		nDivisionIndex			=	0;
	INT32		nPartIndex				=	0;
	
	FLOAT		fX						=	this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.x; 
	FLOAT		fZ						=	this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.z;
	FLOAT		fLocalX					=	0;
	FLOAT		fLocalZ					=	0;

	fX									-=	MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH * this->w / ( 2.0f * 2.0f * ACUIMINIMAP_TEXTURE_SIZE );
	fZ									-=	MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH * this->h / ( 2.0f * 2.0f * ACUIMINIMAP_TEXTURE_SIZE );

	nDivisionIndex						=	GetDivisionIndexF( fX , fZ );

	INT32		lFirstSectorIndexX		=	ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndex ) );
	INT32		lFirstSectorIndexZ		=	ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndex ) );

	fLocalX								=	this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.x - GetSectorStartX( lFirstSectorIndexX );
	fLocalZ								=	this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.z - GetSectorStartZ( lFirstSectorIndexZ );

	INT32		nX						=	( INT32 ) ( fLocalX / ( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 4.0f ) );
	INT32		nZ						=	( INT32 ) ( fLocalZ / ( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 4.0f ) );

	if( nX > 3 ) nX = 3;
	if( nZ > 3 ) nZ = 3;

	nPartIndex = nX + nZ * 4;
	

	lFirstSectorIndexX					=	ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndex ) );
	lFirstSectorIndexZ					=	ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndex ) );

	INT32		nDVX					=	GetDivisionXIndex( nDivisionIndex );
	INT32		nDVZ					=	GetDivisionZIndex( nDivisionIndex );
	INT32		nDivisionRight			=	MakeDivisionIndex( nDVX + 1	, nDVZ		);
	INT32		nDivisionBottom			=	MakeDivisionIndex( nDVX		, nDVZ + 1	);
	INT32		nDivisionRightBottom	=	MakeDivisionIndex( nDVX + 1	, nDVZ + 1	);
	INT32		nDivisionTemp			=	0;
	INT32		nOffset					=	0;
	INT32		nHalf					=	ACUIMINIMAP_TEXTURE_SIZE >> 1;
	
	BOOL		bFirst					=	TRUE;
	BOOL		bInvalidMapPosition		=	TRUE;

	RwTexture	*pMinimapTexture		=	NULL;
	RwTexture	*pCurrentTexture		=	NULL;
	RwTexture	*pRaiderTexture			=	NULL;

	m_fWidth							=	MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;
	fLocalX								=	GetSectorStartX( lFirstSectorIndexX );
	fLocalZ								=	GetSectorStartZ( lFirstSectorIndexZ );

	for( INT j = 0 ; j < 5 ; ++ j )
	{
		for( INT i = 0 ; i < 6 ; ++ i )
		{

			nDivisionTemp	= MakeDivisionIndex( nDVX + i / 2 , nDVZ + j / 2 );
			nOffset			= i % 2 + ( j % 2 ) * 2;

			pCurrentTexture = _TargetRenderTexture( nDivisionTemp , nOffset );

			if( pCurrentTexture )
			{
				if( bFirst  )
				{
					bFirst = FALSE;
					pMinimapTexture = pCurrentTexture;
					m_nDivisionIndex	=	nDivisionTemp;
					m_nPartIndex		=	nPartIndex;
				}
				else
				{
					if( pMinimapTexture != pCurrentTexture )
						bInvalidMapPosition = FALSE;
				}
			}

		}
	}
}

VOID	AcUIRaider::_RenderCharacter( VOID )
{
	AgpdCharacter			*pcsCharacter	=	NULL;
	INT32					lIndex			=	0;
	INT32					nX				=	0;
	INT32					nZ				=	0;
	INT32					nOffsetX		=	0;
	INT32					nOffsetZ		=	0;
	INT32					nCenterX		=	(this->w >> 1);
	INT32					nCenterZ		=	(this->h >> 1);
	//RwRaster*				pRaster			=	RwCameraGetRaster( m_pRaiderCamera );
	INT32					nTextureType	=	0;
	FLOAT					fModulateX		=	1;
	FLOAT					fModulateZ		=	1;

	for (pcsCharacter = m_pAgpmCharacter->GetCharSequence(&lIndex);
		pcsCharacter;
		pcsCharacter = m_pAgpmCharacter->GetCharSequence(&lIndex))
	{
		INT32		nWidth		= ACUIRAIDER_POINT_SIZE;
		INT32		nWidthHalf	= nWidth >> 1;

		nX = __GetRaiderPos( pcsCharacter->m_stPos.x ) - m_nXPos + nCenterX;
		nZ = __GetRaiderPos( pcsCharacter->m_stPos.z ) - m_nZPos + nCenterZ;

		if( nX < nWidthHalf				) continue;
		if( nX >= this->w - nWidthHalf	) continue;
		if( nZ < nWidthHalf				) continue;
		if( nZ >= this->h - nWidthHalf	) continue;

		nOffsetX	= nX - nWidthHalf;
		nOffsetZ	= nZ - nWidthHalf;

		nTextureType	=	_CharacterType( pcsCharacter );

		// 아무것도 해당이 안되면 찍지 말자
		if( nTextureType	==	POINT_TEX_COUNT )
			continue;

		this->ClientToScreen( &nOffsetX , &nOffsetZ );

		g_pEngine->DrawIm2DPixel(
			m_pArrPointTexture[ nTextureType ] ,
			nOffsetX 								, 
			nOffsetZ		 						, 
			ACUIRAIDER_POINT_SIZE					,
			ACUIRAIDER_POINT_SIZE					,
			0										,
			0										,
			ACUIRAIDER_POINT_SIZE					,
			ACUIRAIDER_POINT_SIZE					,
			m_lColor			,
			255 );
	
	}

}

VOID	AcUIRaider::_RenderMap( VOID )
{
	INT32 nStartX	=	0;
	INT32 nStartZ	=	0;

	this->ClientToScreen(&nStartX, &nStartZ);

	if( !m_pRaiderTexture )
	{

		m_pRaiderTexture	=	RwTextureCreate( RwCameraGetRaster(m_pRaiderCamera) );
		RwTextureSetFilterMode( m_pRaiderTexture , rwFILTERLINEAR );
		m_pMaskTex			=	RwTextureRead( "raider_base3.png" , NULL );

	}

	g_pEngine->DrawIm2DPixel( m_pRaiderTexture , nStartX-32 , nStartZ-32 , ACUIMINIMAP_TEXTURE_SIZE , ACUIMINIMAP_TEXTURE_SIZE , 0 , 0 , ACUIMINIMAP_TEXTURE_SIZE , ACUIMINIMAP_TEXTURE_SIZE  );
	//g_pEngine->DrawIm2DPixelMask( m_pRaiderTexture , m_pMaskTex , nStartX-32 , nStartZ-32 , ACUIRAIDER_TEXTURE_SIZE , ACUIRAIDER_TEXTURE_SIZE , 0 , 0 , ACUIRAIDER_TEXTURE_SIZE , ACUIRAIDER_TEXTURE_SIZE );
}

RwTexture*	AcUIRaider::_TargetRenderTexture( INT32 nDivisionIdnex , INT32 nPartIndex )
{
	RwTexture * pTexture ;
	FLOAT fStartX	= __GetDivisionStartX( nDivisionIdnex , nPartIndex );
	FLOAT fStartZ	= __GetDivisionStartZ( nDivisionIdnex , nPartIndex );

	INT32	nX;
	INT32	nZ;

	nX		=	__GetMMPos( fStartX );
	nZ		=	__GetMMPos( fStartZ );

	nX		=	nX - m_nXPos + this->w / 2 ;
	nZ		=	nZ - m_nZPos + this->h / 2 ;

	INT32	nStartX , nStartZ	;
	INT32	nEndX	, nEndZ		;
	INT32	nUStart = 0, nVStart = 0	;
	INT32	nUEnd = ACUIMINIMAP_TEXTURE_SIZE, nVEnd = ACUIMINIMAP_TEXTURE_SIZE;

	nStartX	= nX		;
	nStartZ	= nZ		;
	nEndX	= nStartX	+ ACUIMINIMAP_TEXTURE_SIZE		;
	nEndZ	= nStartZ	+ ACUIMINIMAP_TEXTURE_SIZE		;

	if( nStartX < 0 )
	{
		nUStart	= (-nStartX);
		nStartX	= 0;
	}

	if( nStartZ < 0 )
	{
		nVStart	= (-nStartZ );
		nStartZ = 0;
	}

	if( nEndX >= this->w )
	{
		nUEnd	-= nEndX - this->w;
		nEndX	= this->w ;
	}
	if( nEndZ >= this->h )
	{
		nVEnd	-= nEndZ - this->h;
		nEndZ	= this->h ;
	}

	pTexture = this->m_pAgcmMinimap->GetMinimapTexture( nDivisionIdnex , nPartIndex );
	RwRaster*	pRaster		=	RwCameraGetRaster( m_pRaiderCamera );

	if( ( nEndX - nStartX ) > 0 && ( nEndZ - nStartZ ) > 0 && pTexture )
	{

		RwCameraBeginUpdate( m_pRaiderCamera );

		g_pEngine->DrawIm2DPixelRaster( pRaster , pTexture					, 
			nStartX						, 
			nStartZ						, 
			nEndX - nStartX				,
			nEndZ - nStartZ				,
			nUStart						,			
			nVStart						,			
			nUEnd						,			
			nVEnd						,
			m_lColor					,
			( UINT8 ) ( ((m_lColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1)));

		RwCameraEndUpdate( m_pRaiderCamera );
		
		return pTexture;
	}
	else
	{
		return NULL;
	}
}

VOID	AcUIRaider::_PointTextureLoad( VOID )
{
	m_pArrPointTexture[ POINT_TEX_RED		]		=	RwTextureRead( "raider_pixel_red.png" , NULL );
	m_pArrPointTexture[ POINT_TEX_GREEN		]		=	RwTextureRead( "raider_pixel_green.png" , NULL );
	m_pArrPointTexture[ POINT_TEX_PURPLE	]		=	RwTextureRead( "raider_pixel_violet.png" , NULL );
	m_pArrPointTexture[ POINT_TEX_WHITE		]		=	RwTextureRead( "raider_pixel_white.png" , NULL );
	m_pArrPointTexture[ POINT_TEX_BLUE		]		=	RwTextureRead( "raider_pixel_blue.png" , NULL );
}

INT32	AcUIRaider::_CharacterType( AgpdCharacter* pAgpdCharacter )
{
	AgpdCharacter*		pSelfCharacter		=	this->m_pAgcmMinimap->m_pcsAgcmCharacter->GetSelfCharacter();

	if( pAgpdCharacter->m_szID[0] == NULL )
		return POINT_TEX_COUNT;

	if( m_pAgcmMinimap->m_bMurderer )
	{
		// 범죄자 빨간색
		if( this->m_pAgpmCharacter->GetMurdererLevel( pAgpdCharacter ) >= AGPMCHAR_MURDERER_LEVEL1_POINT )
			return POINT_TEX_RED;
	}

	if( m_pAgcmMinimap->m_bEnemyGuild )
	{

		// 배틀그라운드에서는 표시하지 않는다
		if( !this->m_pAgcmMinimap->m_pcsAgpmBattleGround->IsInBattleGround( pAgpdCharacter ) )
		{
			// 적대길드 표시
			CHAR* szSelfGuildID	 	= this->m_pAgcmMinimap->m_pcsAgpmGuild->GetJoinedGuildID( pSelfCharacter );
			CHAR* szTargetGuildID	= this->m_pAgcmMinimap->m_pcsAgpmGuild->GetJoinedGuildID( pAgpdCharacter );
			
			if( szSelfGuildID && szTargetGuildID && strlen( szSelfGuildID ) > 1 && strlen(szTargetGuildID) > 1 )
			{
				if( this->m_pAgcmMinimap->m_pcsAgpmGuild->IsHostileGuild( szSelfGuildID , szTargetGuildID ) )
					return POINT_TEX_GREEN;
			}
		}
	}

	if( m_pAgcmMinimap->m_bCriminal )
	{
		// 선공자
		if( pAgpdCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED )
			return POINT_TEX_PURPLE;
	}

	return POINT_TEX_COUNT;
}