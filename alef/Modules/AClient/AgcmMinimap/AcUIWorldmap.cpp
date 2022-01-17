// AcUIWorldmap.cpp: implementation of the AcUIWorldmap class.
//
//////////////////////////////////////////////////////////////////////
#include "AgcmMinimap.h"
#include "AcUIWorldmap.h"
#include "AgcmChatting2.h"
#include "ApmMap.h"
#include "AgcmUILogin.h"
#include "AgpmSiegeWar.h"
#include "AuStrTable.h"
#include "AgcmSiegeWar.h"

#include "AuRegionNameConvert.h"

#define WORLDMAP_WIDTH 800
#define WORLDMAP_HEIGHT 600

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
INT32	__GetWMPosX( AcUIWorldmap::stWorldMap	* pstWorldMap , FLOAT fX )
{
	FLOAT	dX = fX - pstWorldMap->xStart;
	INT32	nX = ( INT32 ) ( dX / pstWorldMap->fStepSize );

	if( nX < 10		) nX = 10		;
	if( nX >= WORLDMAP_WIDTH-10) nX = WORLDMAP_WIDTH - 10	;
	return nX;
}

INT32	__GetWMPosY( AcUIWorldmap::stWorldMap	* pstWorldMap , FLOAT fZ )
{
	FLOAT	dZ = fZ - pstWorldMap->zStart;
	INT32	nZ = ( INT32 ) ( dZ / pstWorldMap->fStepSize );

	if( nZ < 10		) nZ = 10		;
	if( nZ >= WORLDMAP_HEIGHT-10) nZ = WORLDMAP_HEIGHT - 10	;
	return nZ;
}

FLOAT	__GetWMPosXINV( AcUIWorldmap::stWorldMap	* pstWorldMap , INT32 nX )
{
	return ( FLOAT ) nX * pstWorldMap->fStepSize + pstWorldMap->xStart;
}

FLOAT	__GetWMPosZINV( AcUIWorldmap::stWorldMap	* pstWorldMap , INT32 nZ )
{
	return ( FLOAT ) nZ * pstWorldMap->fStepSize + pstWorldMap->zStart;
}

AcUIWorldmap::AcUIWorldmap()
{
	m_pAgcmMinimap			= NULL	;
	m_pAgpmCharacter		= NULL	;
	m_nCurrentWorldMapIndex	= 0	;

	m_bThereIsNoWorldMapTemplate	= FALSE;

	m_bIsOpenWorldMap		= FALSE;

	m_fAlpha = 1.0f;
	m_fMinimumAlphaLimit = 0.3f;

	m_nEventGetSlideBar = -1;

	m_nEventGetBtnGuildMarkHuman = -1;
	m_nEventGetBtnGuildMarkHuman2 = -1;
	m_nEventGetBtnGuildMarkHuman3 = -1;

	m_nEventGetBtnGuildMarkOrc = -1;
	m_nEventGetBtnGuildMarkOrc2 = -1;
	m_nEventGetBtnGuildMarkOrc3 = -1;

	m_nEventGetBtnGuildMarkMoonElf = -1;
	m_nEventGetBtnGuildMarkMoonElf2 = -1;
	m_nEventGetBtnGuildMarkMoonElf3 = -1;

	m_nEventGetBtnGuildMarkDragonSion = -1;
	m_nEventGetBtnGuildMarkDragonSion2 = -1;
	m_nEventGetBtnGuildMarkDragonSion3 = -1;

	m_nEventGetBtnGuildMarkArchlord = -1;
	m_nEventGetBtnGuildMarkArchlord2 = -1;
	m_nEventGetBtnGuildMarkArchlord3 = -1;

	m_pScrollAlpha = NULL;

	m_pCastleGuildMarkHuman = NULL;
	m_pCastleGuildMarkHuman2 = NULL;
	m_pCastleGuildMarkHuman3 = NULL;

	m_pCastleGuildMarkOrc = NULL;
	m_pCastleGuildMarkOrc2 = NULL;
	m_pCastleGuildMarkOrc3 = NULL;

	m_pCastleGuildMarkMoonElf = NULL;
	m_pCastleGuildMarkMoonElf2 = NULL;
	m_pCastleGuildMarkMoonElf3 = NULL;

	m_pCastleGuildMarkDragonSion = NULL;
	m_pCastleGuildMarkDragonSion2 = NULL;
	m_pCastleGuildMarkDragonSion3 = NULL;

	m_pCastleGuildMarkArchlord = NULL;
	m_pCastleGuildMarkArchlord2 = NULL;
	m_pCastleGuildMarkArchlord3 = NULL;

	m_pcdUIUserData = NULL;
	m_bViewCastleInfo = FALSE;

	memset( m_strDisplayText, 0, sizeof( CHAR ) * 32 );
}

AcUIWorldmap::~AcUIWorldmap()
{

}

void AcUIWorldmap::OnAddEvent( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return;

	if( m_nEventGetSlideBar < 0 )
	{
		m_nEventGetSlideBar = pcmUIManager->AddEvent( "WorldMap_GetSlideBar" );
		if( m_nEventGetSlideBar < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkHuman < 0 )
	{
		m_nEventGetBtnGuildMarkHuman = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleHuman" );
		if( m_nEventGetBtnGuildMarkHuman < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkHuman2 < 0 )
	{
		m_nEventGetBtnGuildMarkHuman2 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleHuman2" );
		if( m_nEventGetBtnGuildMarkHuman2 < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkHuman3 < 0 )
	{
		m_nEventGetBtnGuildMarkHuman3 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleHuman3" );
		if( m_nEventGetBtnGuildMarkHuman3 < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkOrc < 0 )
	{
		m_nEventGetBtnGuildMarkOrc = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleOrc" );
		if( m_nEventGetBtnGuildMarkOrc < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkOrc2 < 0 )
	{
		m_nEventGetBtnGuildMarkOrc2 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleOrc2" );
		if( m_nEventGetBtnGuildMarkOrc2 < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkOrc3 < 0 )
	{
		m_nEventGetBtnGuildMarkOrc3 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleOrc3" );
		if( m_nEventGetBtnGuildMarkOrc3 < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkMoonElf < 0 )
	{
		m_nEventGetBtnGuildMarkMoonElf = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleMoonElf" );
		if( m_nEventGetBtnGuildMarkMoonElf < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkMoonElf2 < 0 )
	{
		m_nEventGetBtnGuildMarkMoonElf2 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleMoonElf2" );
		if( m_nEventGetBtnGuildMarkMoonElf2 < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkMoonElf3 < 0 )
	{
		m_nEventGetBtnGuildMarkMoonElf3 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleMoonElf3" );
		if( m_nEventGetBtnGuildMarkMoonElf3 < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkDragonSion < 0 )
	{
		m_nEventGetBtnGuildMarkDragonSion = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleDragonSion" );
		if( m_nEventGetBtnGuildMarkDragonSion < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkDragonSion2 < 0 )
	{
		m_nEventGetBtnGuildMarkDragonSion2 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleDragonSion2" );
		if( m_nEventGetBtnGuildMarkDragonSion2 < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkDragonSion3 < 0 )
	{
		m_nEventGetBtnGuildMarkDragonSion3 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleDragonSion3" );
		if( m_nEventGetBtnGuildMarkDragonSion3 < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkArchlord < 0 )
	{
		m_nEventGetBtnGuildMarkArchlord = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleArchlord" );
		if( m_nEventGetBtnGuildMarkArchlord < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkArchlord2 < 0 )
	{
		m_nEventGetBtnGuildMarkArchlord2 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleArchlord2" );
		if( m_nEventGetBtnGuildMarkArchlord2 < 0 ) return;
	}

	if( m_nEventGetBtnGuildMarkArchlord3 < 0 )
	{
		m_nEventGetBtnGuildMarkArchlord3 = pcmUIManager->AddEvent( "WorldMap_GetBtnCastleArchlord3" );
		if( m_nEventGetBtnGuildMarkArchlord3 < 0 ) return;
	}
}

void AcUIWorldmap::OnAddFunction( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return;

	pcmUIManager->AddFunction( this, "WorldMap_GetSlideBar", CB_GetSlideBar, 0 );

	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleHuman", CB_GetBtnCastleHuman, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleHuman2", CB_GetBtnCastleHuman2, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleHuman3", CB_GetBtnCastleHuman3, 0 );

	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleOrc", CB_GetBtnCastleOrc, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleOrc2", CB_GetBtnCastleOrc2, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleOrc3", CB_GetBtnCastleOrc3, 0 );

	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleMoonElf", CB_GetBtnCastleMoonElf, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleMoonElf2", CB_GetBtnCastleMoonElf2, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleMoonElf3", CB_GetBtnCastleMoonElf3, 0 );

	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleDragonSion", CB_GetBtnCastleDragonSion, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleDragonSion2", CB_GetBtnCastleDragonSion2, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleDragonSion3", CB_GetBtnCastleDragonSion3, 0 );

	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleArchlord", CB_GetBtnCastleArchlord, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleArchlord2", CB_GetBtnCastleArchlord2, 0 );
	pcmUIManager->AddFunction( this, "WorldMap_GetBtnCastleArchlord3", CB_GetBtnCastleArchlord3, 0 );
}

void AcUIWorldmap::OnAddDisplay( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return;

	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerGuildHuman",			0,	CB_OnDisplayOwnerGuildHuman, AGCDUI_USERDATA_TYPE_STRING );
	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerNameHuman",			0,	CB_OnDisplayOwnerNameHuman, AGCDUI_USERDATA_TYPE_STRING );

	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerGuildOrc",			0,	CB_OnDisplayOwnerGuildOrc, AGCDUI_USERDATA_TYPE_STRING );
	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerNameOrc",				0,	CB_OnDisplayOwnerNameOrc, AGCDUI_USERDATA_TYPE_STRING );

	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerGuildMoonElf",		0,	CB_OnDisplayOwnerGuildMoonElf, AGCDUI_USERDATA_TYPE_STRING );
	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerNameMoonElf",			0,	CB_OnDisplayOwnerNameMoonElf, AGCDUI_USERDATA_TYPE_STRING );

	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerGuildDragonScion",	0,	CB_OnDisplayOwnerGuildDragonScion, AGCDUI_USERDATA_TYPE_STRING );
	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerNameDragonScion",		0,	CB_OnDisplayOwnerNameDragonScion, AGCDUI_USERDATA_TYPE_STRING );

	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerGuildArchlord",		0,	CB_OnDisplayOwnerGuildArchlord, AGCDUI_USERDATA_TYPE_STRING );
	pcmUIManager->AddDisplay( this, "WorldMap_DisplayOwnerNameArchlord",		0,	CB_OnDisplayOwnerNameArchlord, AGCDUI_USERDATA_TYPE_STRING );
}

void AcUIWorldmap::OnAddUserData( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return;

	m_pcdUIUserData = pcmUIManager->AddUserData( "WorldMap_UserData", m_strDisplayText, sizeof( CHAR ), 32, AGCDUI_USERDATA_TYPE_STRING );
}

void AcUIWorldmap::OnGetControls( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return;

	pcmUIManager->ThrowEvent( m_nEventGetSlideBar );

	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkHuman );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkHuman2 );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkHuman3 );

	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkOrc );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkOrc2 );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkOrc3 );

	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkMoonElf );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkMoonElf2 );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkMoonElf3 );

	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkDragonSion );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkDragonSion2 );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkDragonSion3 );

	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkArchlord );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkArchlord2 );
	pcmUIManager->ThrowEvent( m_nEventGetBtnGuildMarkArchlord3 );
}

void AcUIWorldmap::OnRefreshUserData( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return;

	pcmUIManager->RefreshUserData( m_pcdUIUserData, TRUE );
}

void AcUIWorldmap::OnUpdateWorldMapAlpha( void )
{
	// 스크롤바에 설정된 스크롤버튼의 위치값을 가져온다.
	if( m_pScrollAlpha )
	{
		m_pScrollAlpha->CalcScrollValue();
		m_fAlpha = m_pScrollAlpha->GetScrollValue();

		if( m_fAlpha > 1.0f )
		{
			m_fAlpha = 1.0f;
		}
		else if( m_fAlpha < m_fMinimumAlphaLimit )
		{
			m_fAlpha = m_fMinimumAlphaLimit;
		}

		m_pfAlpha = &m_fAlpha;
	}
	else
	{
		m_fAlpha = 1.0f;
		m_pfAlpha = &m_fAlpha;
	}
}

void  AcUIWorldmap::OnUpdateCastleInfo( AuCastleOwnerType eCastleType, AuCastleInfoType eInfoType, void* pData )
{
	switch( eInfoType )
	{
	case CastleInfo_OwnerGuildName :
		{
			CHAR* pDisplay = ( CHAR* )pData;
			if( !pDisplay ) return;

			CHAR* pName = GetCastleOwnerGuildName( eCastleType );
			if( m_bViewCastleInfo )
			{
				if( pName && strlen( pName ) > 0 )
				{
					sprintf( pDisplay, "[%s]", pName );
				}
				else
				{
					CHAR* pText = ClientStr().GetStr( STI_STATICTEXT_NO_TEXT );
					sprintf( pDisplay, "[%s]", pText );
				}
			}
			else
			{
				sprintf( pDisplay, "%s", "" );
			}
		}
		break;

	case CastleInfo_GuildMasterName :
		{
			CHAR* pDisplay = ( CHAR* )pData;
			if( !pDisplay ) return;

			CHAR* pGuildMasterName = GetCastleOwnerGuildMasterName( eCastleType );
			if( m_bViewCastleInfo )
			{
				if( pGuildMasterName && strlen( pGuildMasterName ) > 0 )
				{
					sprintf( pDisplay, "%s", pGuildMasterName );
				}
				else
				{
					CHAR* pText = ClientStr().GetStr( STI_STATICTEXT_NO_TEXT );
					sprintf( pDisplay, "%s", pText );
				}
			}
			else
			{
				sprintf( pDisplay, "%s", "" );
			}
		}
		break;
	}

	OnUpdateGuildMarkTexture( eCastleType );
}

void AcUIWorldmap::OnUpdateGuildMarkTexture( AuCastleOwnerType eCastleType )
{
	if( !m_bViewCastleInfo ) return;

	AcUIButton* pBtn = NULL;
	RwTexture* pTexture = NULL;
	switch( eCastleType )
	{
	case CastleOwner_Human :
		{
			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkBG( eCastleType );
			RenderButton( m_pCastleGuildMarkHuman, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkPattern( eCastleType );
			RenderButton( m_pCastleGuildMarkHuman2, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkSimbol( eCastleType );
			RenderButton( m_pCastleGuildMarkHuman3, pTexture );
		}
		break;

	case CastleOwner_Orc :
		{
			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkBG( eCastleType );
			RenderButton( m_pCastleGuildMarkOrc, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkPattern( eCastleType );
			RenderButton( m_pCastleGuildMarkOrc2, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkSimbol( eCastleType );
			RenderButton( m_pCastleGuildMarkOrc3, pTexture );
		}
		break;

	case CastleOwner_MoonElf :
		{
			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkBG( eCastleType );
			RenderButton( m_pCastleGuildMarkMoonElf, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkPattern( eCastleType );
			RenderButton( m_pCastleGuildMarkMoonElf2, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkSimbol( eCastleType );
			RenderButton( m_pCastleGuildMarkMoonElf3, pTexture );
		}
		break;

	case CastleOwner_DragonScion :
		{
			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkBG( eCastleType );
			RenderButton( m_pCastleGuildMarkDragonSion, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkPattern( eCastleType );
			RenderButton( m_pCastleGuildMarkDragonSion2, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkSimbol( eCastleType );
			RenderButton( m_pCastleGuildMarkDragonSion3, pTexture );
		}
		break;

	case CastleOwner_Archlord :
		{
			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkBG( eCastleType );
			RenderButton( m_pCastleGuildMarkArchlord, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkPattern( eCastleType );
			RenderButton( m_pCastleGuildMarkArchlord2, pTexture );

			pTexture = ( RwTexture* )GetCastleOwnerGuildMarkSimbol( eCastleType );
			RenderButton( m_pCastleGuildMarkArchlord3, pTexture );
		}
		break;

	default :					return;								break;
	}
}

void AcUIWorldmap::OnWindowRender	()
{
	PROFILE("AcUIWorldmap::OnWindowRender");

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )m_pAgcmMinimap->GetModule( "AgcmUIManager2" );
	OnUpdateWorldMapAlpha();

	INT32	nOffsetX, nOffsetZ	;
	INT32	nCenterX, nCenterZ	;
	UINT8	uAlphaWindow = ( UINT8 ) ( ((m_lColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1.0f) );

	ASSERT( NULL != m_pAgcmMinimap );

	stWorldMap	* pstWorldMap	= GetCurrentWorldMap();

	// 마고자 (2005-05-27 오후 3:03:14) : 실수 ;;
	if( NULL == pstWorldMap ) return;

	RwTexture	* pTexture		= m_pAgcmMinimap->GetWorldMapTexture( pstWorldMap->nIndex );

	if( NULL == pTexture || NULL == pstWorldMap )
	{
		TRACE( "월드맵텍스쳐 파일이 안보이나봐요~\n" );
		return;
	}

	INT32	nX = 0 , nZ = 0;
	this->ClientToScreen( &nX , &nZ );

	g_pEngine->DrawIm2DPixel(
			pTexture			, 
			nX					, 
			nZ					, 
			WORLDMAP_WIDTH		,
			WORLDMAP_HEIGHT		,
			0 , 0 , -1 , -1		,
			0xffffffff			,
			uAlphaWindow		 );

	{
		// 타이틀 출력.

		INT32	nPopupX , nPopupY;
		INT32	lFontHeight		= m_pAgcmMinimap->m_pcsAgcmFont->m_astFonts[m_lFontType].lSize;

		m_pAgcmMinimap->m_pcsAgcmFont->FontDrawStart(m_lFontType);

		INT32 nTextWidth = m_pAgcmMinimap->m_pcsAgcmFont->GetTextExtent( 0 , ( char * ) pstWorldMap->strName.c_str(), pstWorldMap->strName.length() );

		// 가운데로 위치시킴..
		nPopupX = ( INT32 ) 396 - (nTextWidth >> 1);
		nPopupY = 44;

		ClientToScreen(&nPopupX, &nPopupY);

		std::string point = RegionLocalName().GetStr( ( char * ) pstWorldMap->strName.c_str() );

		m_pAgcmMinimap->m_pcsAgcmFont->DrawTextIM2D(
			( FLOAT ) nPopupX, ( FLOAT ) nPopupY ,
			point == "" ? (char*) pstWorldMap->strName.c_str() : (char*)point.c_str(),	//  Text
			0 ,			//  font type
			uAlphaWindow,		//  alpha
			0xfffff8e7, //g_TextColor,	// color
			true,		// Draw Shadow
			false,		// Blod
			false,		// Draw Background
			0x77000000	//g_BackColor	// Background color
			);

		m_pAgcmMinimap->m_pcsAgcmFont->FontDrawEnd();
	}
	
	// 커서 출력..

	nX = __GetWMPosX( pstWorldMap , m_pAgcmMinimap->m_stSelfCharacterPos.x );
	nZ = __GetWMPosY( pstWorldMap , m_pAgcmMinimap->m_stSelfCharacterPos.z );

	RwTexture * pCursor = this->m_pAgcmMinimap->GetPcPosTexture();
	FLOAT	fAlpha = (FLOAT)( GetTickCount() % 600 ) / 600.0f * 3.1415727f;
	UINT8	uAlpha = ( UINT8 ) ( 255.0 * sin( fAlpha ) );

	if( uAlpha > uAlphaWindow ) uAlpha = uAlphaWindow;
	{
		// 커서 출력..
		// 방위 알아냄.. 

		// 중앙점..
		nCenterX	= nX;
		nCenterZ	= nZ;

		// 마고자 (2004-04-07 오후 9:44:18) : 
		// 중앙점 8,19
		nOffsetX	= nCenterX - 4	;
		nOffsetZ	= nCenterZ - 4	;
		
		this->ClientToScreen(&nOffsetX, &nOffsetZ);
		this->ClientToScreen(&nCenterX, &nCenterZ);

		g_pEngine->DrawIm2DPixel(
				pCursor				, 
				nOffsetX			, 
				nOffsetZ			, 
				8					,
				8					,
				0 , 0 , -1 , -1		,
				0xffffffff			,
				( UINT8 )uAlpha		);
	}

	// 파티원 출력..
	RwTexture		*pMemberPosTexture	= m_pAgcmMinimap->GetMemberPosTexture();
	AgpdCharacter	*pcsSelfCharacter	= m_pAgcmMinimap->m_pcsSelfCharacter; 

	if( pMemberPosTexture && pcsSelfCharacter )
	{
		AgpdParty	*pcsParty	= this->m_pAgcmMinimap->m_pcsAgpmParty->GetPartyLock(pcsSelfCharacter); 
		
		if (pcsParty && m_pAgpmCharacter) 
		{ 
			for (int i = 0 ; i < pcsParty->m_nCurrentMember; ++i) 
			{ 
				AgpdCharacter	*pcsMember	= m_pAgpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
				
				if (pcsMember == pcsSelfCharacter	||
					pcsMember == NULL				) 
					continue; 

				nX = __GetWMPosX( pstWorldMap , pcsMember->m_stPos.x );
				nZ = __GetWMPosY( pstWorldMap , pcsMember->m_stPos.z );

				////////////

				// 중앙점..
				nCenterX	= nX;
				nCenterZ	= nZ;

				// 마고자 (2004-04-07 오후 9:44:18) : 
				// 중앙점 8,19
				nOffsetX	= nCenterX - 4;// 6은 커서의 중점 옵셋..
				nOffsetZ	= nCenterZ - 4;

				this->ClientToScreen(&nOffsetX, &nOffsetZ);
				this->ClientToScreen(&nCenterX, &nCenterZ);

				g_pEngine->DrawIm2DPixel(
						pMemberPosTexture	, 
						nOffsetX			, 
						nOffsetZ			, 
						8					,
						8					,
						0 , 0 , -1 , -1		,
						0xffffffff			,
						uAlphaWindow		
						);
			} 

			pcsParty->m_Mutex.Release();
		}	
	}


	// 미니맵 포인트 출력
	if( pcsSelfCharacter )
	{	

		INT32	nCenterX, nCenterZ	;
		BOOL	bNeedPopup	= FALSE;
		//INT32	nType;
		char	*pStrText = NULL;
		INT32	nPopupX , nPopupY;
		INT32	nStartY;
		INT32	lDisplayCount	= 0;
		INT32	lFontHeight		= m_pAgcmMinimap->m_pcsAgcmFont->m_astFonts[m_lFontType].lSize;

		AgcmMinimap::MPInfo *	pArray = this->m_pAgcmMinimap->GetMPArray();
		INT32					nCount = this->m_pAgcmMinimap->GetMPCount();

		RwTexture * pTexturePoint;
		INT32		nWidth		;
		INT32		nWidthHalf	;

		nCenterX = __GetWMPosX( pstWorldMap , pcsSelfCharacter->m_stPos.x );
		nCenterZ = __GetWMPosY( pstWorldMap , pcsSelfCharacter->m_stPos.z );
		//nCenterX	= ( this->w >> 1 );
		//nCenterZ	= ( this->h >> 1 );

		for( int i = 0 ; i < nCount ; i ++ )
		{
			if( !pArray[ i ].bDisabled )
			{
				pArray[ i ].bRendered = FALSE;

				switch( pArray[ i ].nType )
				{
				case AgcmMinimap::MPInfo::MPI_QUESTNEW:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetNpcQuestNewTexture();
						nWidth			= 16;
					}
					break;
				case AgcmMinimap::MPInfo::MPI_QUESTINCOMPLETE:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetNpcQuestIncompleteTexture();
						nWidth			= 16;
					}
					break;

				case AgcmMinimap::MPInfo::MPI_SIEGENORMAL:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetTextureSiegeNormal();
						nWidth			= 32;
					}
					break;
				case AgcmMinimap::MPInfo::MPI_SIEGEDESTROIED:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetTextureSiegeDestroied();
						nWidth			= 32;
					}
					break;

				default:
				case AgcmMinimap::MPInfo::MPI_NPC:
				case AgcmMinimap::MPInfo::MPI_NONE:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetNpcNormalTexture();
						nWidth			= 10;
					}
					break;
				}

				nWidthHalf		= nWidth >> 1;

				// protection!
				if( NULL == pTexturePoint ) continue;

				nX = GetMMPosX( pArray[ i ].pos.x );
				nZ = GetMMPosZ( pArray[ i ].pos.z );

				if( nX < nWidthHalf				) continue;
				if( nX >= this->w - nWidthHalf	) continue;
				if( nZ < nWidthHalf				) continue;
				if( nZ >= this->h - nWidthHalf	) continue;

				nOffsetX	= nX - nWidthHalf;
				nOffsetZ	= nZ - nWidthHalf;

				// 팝업 체크..
				if( ( INT32 ) m_prevMouseState.pos.x >= nOffsetX			&&
					( INT32 ) m_prevMouseState.pos.x < nOffsetX + nWidth	&&
					( INT32 ) m_prevMouseState.pos.y >= nOffsetZ			&&
					( INT32 ) m_prevMouseState.pos.y < nOffsetZ + nWidth	)
				{
					++lDisplayCount;
					pArray[ i ].bOnMouse	= TRUE;
				}
				else
				{
					pArray[ i ].bOnMouse	= FALSE;
				}

				this->ClientToScreen(&nOffsetX, &nOffsetZ);

				g_pEngine->DrawIm2DPixel(
						pTexturePoint	, 
						nOffsetX			, 
						nOffsetZ			, 
						nWidth				,
						nWidth				,
						0					,
						0					,
						-1					,
						-1					,
						m_lColor			,
						uAlphaWindow		);

				pArray[ i ].bRendered = TRUE;
			}
		}

		if( g_pEngine->IsCtrlDown() )
		{
			// 컨트롤을 누르고 있으면 모두다 표시함..

			INT32	nTextWidth;

			nCenterX	= ( this->w >> 1 );
			nCenterZ	= ( this->h >> 1 );

			AgcmMinimap::MPInfo *	pHighLight = NULL;

			// 팝업 표시..
			m_pAgcmMinimap->m_pcsAgcmFont->FontDrawStart(m_lFontType);

			for( int i = 0 ; i < nCount ; i ++ )
			{
				if( pArray[ i ].bRendered )
				{
					if (pArray[ i ].bDisabled || !pArray[ i ].strText[0])
						continue;

					if( pArray[ i ].bOnMouse )
					{
						pHighLight = &pArray[ i ] ;
						continue;
					}

					nX = GetMMPosX( pArray[ i ].pos.x );
					nZ = GetMMPosZ( pArray[ i ].pos.z );

					nOffsetX	= nX;
					nOffsetZ	= nZ - 20;

					this->ClientToScreen(&nOffsetX, &nOffsetZ);

					nTextWidth = m_pAgcmMinimap->m_pcsAgcmFont->GetTextExtent( 0 , pArray[ i ].strText, strlen(pArray[ i ].strText) );

					nOffsetX	-= (nTextWidth >> 1);

					m_pAgcmMinimap->m_pcsAgcmFont->DrawTextIM2D(
						( FLOAT ) nOffsetX, ( FLOAT ) nOffsetZ ,
						pArray[ i ].strText,	//  Text
						0 ,			//  font type
						uAlphaWindow	,		//  alpha
						0xfffff8e7, //g_TextColor,	// color
						true,		// Draw Shadow
						false,		// Blod
						true,		// Draw Background
						0x77000000	//g_BackColor	// Background color
						);
				}
			}

			if( pHighLight && !pHighLight->bDisabled && pHighLight->strText[0])
			{
				nX = GetMMPosX( pHighLight->pos.x );
				nZ = GetMMPosZ( pHighLight->pos.z );

				nOffsetX	= nX;
				nOffsetZ	= nZ - 20;

				this->ClientToScreen(&nOffsetX, &nOffsetZ);

				nTextWidth = m_pAgcmMinimap->m_pcsAgcmFont->GetTextExtent( 0 , pHighLight->strText, strlen(pHighLight->strText) );

				nOffsetX	-= (nTextWidth >> 1);

				/*
				m_pAgcmMinimap->m_pcsAgcmFont->DrawTextIM2DScale(
					( FLOAT ) nOffsetX, ( FLOAT ) nOffsetZ ,
					_sfSize , 
					pHighLight->strText,	//  Text
					_sftype ,			//  font type
					uAlphaWindow	,		//  alpha
					0xfffff8e7, //g_TextColor,	// color
					true		// Draw Shadow
					);
				*/
				m_pAgcmMinimap->m_pcsAgcmFont->DrawTextIM2D(
					( FLOAT ) nOffsetX, ( FLOAT ) nOffsetZ ,
					pHighLight->strText,	//  Text
					0 ,			//  font type
					uAlphaWindow	,		//  alpha
					0xfffff8e7, //g_TextColor,	// color
					true,		// Draw Shadow
					false,		// Blod
					true,		// Draw Background
					0xff000000	//g_BackColor	// Background color
					);
			}

			m_pAgcmMinimap->m_pcsAgcmFont->FontDrawEnd();
		}
		else
		if( lDisplayCount)
		{
			INT32	nTextWidth;
			BOOL	bAlignBottom;
			INT32	lMarginYTop;
			INT32	lMarginYBottom;

			lMarginYBottom	= h - ( ( INT32 ) m_prevMouseState.pos.y + lFontHeight * (lDisplayCount + 1));
			lMarginYTop		= ( INT32 ) m_prevMouseState.pos.y - lFontHeight * (lDisplayCount );
			if (lMarginYTop >= 0)
				bAlignBottom	= FALSE;
			else if (lMarginYBottom >= 0)
				bAlignBottom	= TRUE;
			else if (lMarginYTop >= lMarginYBottom)
				bAlignBottom	= FALSE;
			else
				bAlignBottom	= TRUE;

			if (bAlignBottom)
				nStartY = ( INT32 ) m_prevMouseState.pos.y + lFontHeight;
			else
				nStartY = ( INT32 ) m_prevMouseState.pos.y - lFontHeight;

			// 팝업 표시..
			m_pAgcmMinimap->m_pcsAgcmFont->FontDrawStart(m_lFontType);

			for( int i = 0 ; i < nCount ; i ++ )
			{
				if (pArray[ i ].bDisabled || !pArray[ i ].bOnMouse || !pArray[ i ].strText[0])
					continue;

				nTextWidth = m_pAgcmMinimap->m_pcsAgcmFont->GetTextExtent( 0 , pArray[ i ].strText, strlen(pArray[ i ].strText) );

				// 가운데로 위치시킴..
				nPopupX = ( INT32 ) m_prevMouseState.pos.x - (nTextWidth >> 1);
				nPopupY = nStartY;

				ClientToScreen(&nPopupX, &nPopupY);

				m_pAgcmMinimap->m_pcsAgcmFont->DrawTextIM2D(
					( FLOAT ) nPopupX, ( FLOAT ) nPopupY ,
					pArray[ i ].strText,	//  Text
					0 ,			//  font type
					uAlphaWindow	,		//  alpha
					0xfffff8e7, //g_TextColor,	// color
					true,		// Draw Shadow
					false,		// Blod
					true,		// Draw Background
					0x77000000	//g_BackColor	// Background color
					);

				if (bAlignBottom)
					nStartY += lFontHeight;
				else
					nStartY -= lFontHeight;
			}

			m_pAgcmMinimap->m_pcsAgcmFont->FontDrawEnd();
		}
	}

	OnRefreshUserData( pcmUIManager );
}


BOOL	AcUIWorldmap::SetWorldMap( AuPOS * pPos )
{
	if( m_bThereIsNoWorldMapTemplate )
	{
		// 마고자 (2005-05-26 오후 12:12:35) : 
		// 템플릿이 없는 상황
		// 1, 진짜 없거나.
		// 2, 버전업이 안됐거나..

		m_nCurrentWorldMapIndex = 0;
		return TRUE;
	}

	ApmMap	* pcsApmMap = this->m_pAgcmMinimap->m_pcsApmMap;
	ASSERT( NULL != pcsApmMap );
	if( NULL == pcsApmMap ) return FALSE;

	ApmMap::RegionTemplate	* pTemplate;
	pTemplate = pcsApmMap->GetTemplate( pcsApmMap->GetRegion( pPos->x , pPos->z ) );

	m_nCurrentWorldMapIndex = -1;

	if( pTemplate )
	{
		for( int i = 0 ; i < ( int ) m_arrayWorldMap.size() ; i ++ )
		{
			if( m_arrayWorldMap[ i ].nIndex == pTemplate->nWorldMapIndex )
			{
				m_nCurrentWorldMapIndex = i;
				break;
			}
		}
	}

	m_bViewCastleInfo = m_nCurrentWorldMapIndex == 0 ? TRUE : FALSE;

	if( -1 == m_nCurrentWorldMapIndex ) return FALSE; // 매칭돼는 맵이 없다.
	else
		return TRUE;
}

BOOL	AcUIWorldmap::SetWorldMapIndex	( INT32 nWorldMap )
{
	if( m_bThereIsNoWorldMapTemplate )
	{
		// 마고자 (2005-05-26 오후 12:12:35) : 
		// 템플릿이 없는 상황
		// 1, 진짜 없거나.
		// 2, 버전업이 안됐거나..

		m_nCurrentWorldMapIndex = 0;
		return TRUE;
	}

	m_nCurrentWorldMapIndex = -1;

	for( int i = 0 ; i < ( int ) m_arrayWorldMap.size() ; i ++ )
	{
		if( m_arrayWorldMap[ i ].nIndex == nWorldMap )
		{
			m_nCurrentWorldMapIndex = i;
			break;
		}
	}

	m_bViewCastleInfo = m_nCurrentWorldMapIndex == 0 ? TRUE : FALSE;

	if( -1 == m_nCurrentWorldMapIndex ) return FALSE; // 매칭돼는 맵이 없다.
	else
		return TRUE;
}

BOOL	AcUIWorldmap::SetWorldMapByItem	( INT32 nTID )
{
	if( m_bThereIsNoWorldMapTemplate )
	{
		// 마고자 (2005-05-26 오후 12:12:35) : 
		// 템플릿이 없는 상황
		// 1, 진짜 없거나.
		// 2, 버전업이 안됐거나..

		m_nCurrentWorldMapIndex = 0;
		return TRUE;
	}

	m_nCurrentWorldMapIndex = -1;

	for( int i = 0 ; i < ( int ) m_arrayWorldMap.size() ; i ++ )
	{
		if( m_arrayWorldMap[ i ].nMapItemID == nTID )
		{
			m_nCurrentWorldMapIndex = i;
			break;
		}
	}

	m_bViewCastleInfo = m_nCurrentWorldMapIndex == 0 ? TRUE : FALSE;

	if( -1 == m_nCurrentWorldMapIndex ) return FALSE; // 매칭돼는 맵이 없다.
	else
		return TRUE;
}

BOOL AcUIWorldmap::OnInit()
{
	ASSERT( NULL != this->m_pAgcmMinimap );

	return TRUE;
}

BOOL AcUIWorldmap::OnPostInit		()
{
	// 마고자 (2005-07-21 오후 5:45:56) : 
	// UI 작업이 끝나면 이 주석 풀고 작동 시켜야함
	if( -1 == m_nCurrentWorldMapIndex )
	{
		// ASSERT( !"발행하지 않는게 좋음.." );
		// 닫히고 열리는거라면..
		// 맵을 지정하고..

		if( SetWorldMap( &m_pAgcmMinimap->m_stSelfCharacterPos ) )
		{
			AgpmItem	* pAgpmItem = static_cast< AgpmItem * > ( m_pAgcmMinimap->GetModule( "AgpmItem" ) );

			ASSERT( NULL != pAgpmItem );

			// do nothing..

			stWorldMap * pWorldmap = GetCurrentWorldMap();
			ASSERT( NULL != pWorldmap );

			// 아이템 있는지 검사..
			if( pWorldmap->nMapItemID == -1 || 				
				pAgpmItem->GetInventoryItemByTID( m_pAgcmMinimap->m_pcsSelfCharacter , pWorldmap->nMapItemID ) )
			{
				// do nothing 
			}
			else
			{
				// 아이템이 없다.
				// 마고자 (2005-07-21 오후 5:04:44) : 
				// 여기서 return FALSE를 하더라도 UI는 닫히지 않는다.

				m_pAgcmMinimap->m_pcsAgcmUIManager2->ThrowEvent( m_pAgcmMinimap->m_lEventWorldMapNoItem );
				m_pAgcmMinimap->m_pcsAgcmUIManager2->ThrowEvent( m_pAgcmMinimap->m_lEventCloseWorldMapUI );
				return FALSE;
			}
		}
		else
		{
			// 매칭돼는 맵이 없다.. 나오면 안돼는 에러.
			m_pAgcmMinimap->m_pcsAgcmUIManager2->ThrowEvent( m_pAgcmMinimap->m_lEventWorldMapNoItem );
			m_pAgcmMinimap->m_pcsAgcmUIManager2->ThrowEvent( m_pAgcmMinimap->m_lEventCloseWorldMapUI );

			return FALSE;
		}
	}

	return TRUE;
}

BOOL AcUIWorldmap::SetWorldMapInfo( ApmMap::WorldMap	* pWorldMap )
{
	if( NULL != pWorldMap )
	{
		stWorldMap	stWorld;

		stWorld.nIndex		= pWorldMap->nMID;
		stWorld.xStart		= pWorldMap->xStart;
		stWorld.zStart		= pWorldMap->zStart;
		stWorld.xEnd		= pWorldMap->xEnd	;
		stWorld.zEnd		= pWorldMap->zEnd	;

		stWorld.nMapItemID	= pWorldMap->nMapItemID;

		stWorld.fStepSize	= ( stWorld.xEnd - stWorld.xStart ) / ( FLOAT ) WORLDMAP_WIDTH	;
		stWorld.strName		= pWorldMap->strComment				;
		
		// 벡터에 추가.
		m_arrayWorldMap.push_back( stWorld );
	}
	else
	{
		stWorldMap	stWorld;

		stWorld.nIndex		= 1;
		stWorld.xStart		= -695682.50f;
		stWorld.zStart		= -259785.20f;
		stWorld.xEnd		= 579336.88f	;
		stWorld.zEnd		= 696479.38f	;

		stWorld.nMapItemID	= -1;

		stWorld.fStepSize	= ( stWorld.xEnd - stWorld.xStart ) / ( FLOAT ) WORLDMAP_WIDTH	;
		stWorld.strName		= "World Map"	;
		
		// 벡터에 추가.
		m_arrayWorldMap.push_back( stWorld );
	}

	return TRUE;
}	

BOOL AcUIWorldmap::OnLButtonDblClk	( RsMouseStatus *ms	)
{
	stWorldMap	* pstWorldMap	= GetCurrentWorldMap();
	if( NULL == pstWorldMap ) return FALSE;

	FLOAT	fX , fZ;
	fX = __GetWMPosXINV( pstWorldMap , ( INT32 ) ms->pos.x );
	fZ = __GetWMPosZINV( pstWorldMap , ( INT32 ) ms->pos.y );

	char strMessage[ 256 ];
	sprintf( strMessage , "/move %f,%f" , fX , fZ );

	AgcmChatting2 * pcsAgcmChatting2 = ( AgcmChatting2 * ) m_pAgcmMinimap->GetModule( "AgcmChatting2" );

	if( pcsAgcmChatting2 )
	{
		pcsAgcmChatting2->SendChatMessage( AGPDCHATTING_TYPE_NORMAL , strMessage , strlen( strMessage ) , NULL );
	}

	return TRUE;
}

BOOL AcUIWorldmap::OnRButtonUp	( RsMouseStatus *ms	)
{
	return OnLButtonDblClk( ms );
}

BOOL AcUIWorldmap::OnMouseMove( RsMouseStatus *ms	)
{
	m_prevMouseState = *ms;

	return FALSE;
}

INT32 AcUIWorldmap::GetMMPosX( FLOAT fPosX )
{
	stWorldMap	* pstWorldMap	= GetCurrentWorldMap();
	ASSERT( NULL != pstWorldMap );

	return __GetWMPosX( pstWorldMap , fPosX );
}

INT32 AcUIWorldmap::GetMMPosZ( FLOAT fPosZ )
{
	stWorldMap	* pstWorldMap	= GetCurrentWorldMap();
	ASSERT( NULL != pstWorldMap );

	return __GetWMPosY( pstWorldMap , fPosZ );
}

void AcUIWorldmap::SetScrollBar( AcUIScroll* pScroll )
{
	if( !pScroll ) return;

	m_pScrollAlpha = pScroll;
	OnUpdateWorldMapAlpha();
}

void AcUIWorldmap::RenderButton( AcUIButton* pBtn, void* pTexture )
{
	if( pBtn && pTexture )
	{
		INT32 nPosX = pBtn->m_lAbsolute_x;
		INT32 nPosY = pBtn->m_lAbsolute_y;
		INT32 nWidth = pBtn->w;
		INT32 nHeight = pBtn->h;

		g_pEngine->DrawIm2D( ( RwTexture* )pTexture, ( float )nPosX, ( float )nPosY, ( float )nWidth, ( float )nHeight );
	}
}

void* AcUIWorldmap::GetCastleInfomation( AuCastleOwnerType eCastleType )
{
	if( !m_pAgpmCharacter ) return NULL;
	
	AgpmSiegeWar* ppmSiegeWar = ( AgpmSiegeWar* )m_pAgpmCharacter->GetModule( "AgpmSiegeWar" );
	if( !ppmSiegeWar ) return NULL;

	CHAR* pCastleName = NULL;
	switch( eCastleType )
	{
	case CastleOwner_Human :		pCastleName = ClientStr().GetStr( STI_STATICTEXT_CASTLENAME_HUMAN );		break;
	case CastleOwner_Orc :			pCastleName = ClientStr().GetStr( STI_STATICTEXT_CASTLENAME_ORC );			break;
	case CastleOwner_MoonElf :		pCastleName = ClientStr().GetStr( STI_STATICTEXT_CASTLENAME_MOONELF );		break;
	case CastleOwner_DragonScion :	pCastleName = ClientStr().GetStr( STI_STATICTEXT_CASTLENAME_DRAGONSCION );	break;
	case CastleOwner_Archlord :		pCastleName = ClientStr().GetStr( STI_STATICTEXT_CASTLENAME_ARCHLORD );		break;
	default :						return NULL;																break;
	}

	return ppmSiegeWar->GetSiegeWarInfo( pCastleName );
}

void* AcUIWorldmap::GetCastleOwnerGuild( AuCastleOwnerType eCastleType )
{
	AgpmGuild* ppmGuild = ( AgpmGuild* )m_pAgpmCharacter->GetModule( "AgpmGuild" );
	if( !ppmGuild ) return NULL;

	CHAR* pGuildName = GetCastleOwnerGuildName( eCastleType );
	if( pGuildName && strlen( pGuildName ) > 0 )
	{
		return ppmGuild->GetGuild( pGuildName );
	}

	return NULL;
}

CHAR* AcUIWorldmap::GetCastleOwnerGuildName( AuCastleOwnerType eCastleType )
{
	AgpdSiegeWar* ppdSiegeWar = ( AgpdSiegeWar* )GetCastleInfomation( eCastleType );
	if( !ppdSiegeWar ) return NULL;

	return ppdSiegeWar->m_strOwnerGuildName.GetBuffer();
}

CHAR* AcUIWorldmap::GetCastleOwnerGuildMasterName( AuCastleOwnerType eCastleType )
{
	AgpdSiegeWar* ppdSiegeWar = ( AgpdSiegeWar* )GetCastleInfomation( eCastleType );
	if( !ppdSiegeWar ) return NULL;

	return ppdSiegeWar->m_strOwnerGuildMasterName.GetBuffer();
}

void* AcUIWorldmap::GetCastleOwnerGuildMarkBG( AuCastleOwnerType eCastleType )
{
	AgpmGuild* ppmGuild = ( AgpmGuild* )m_pAgpmCharacter->GetModule( "AgpmGuild" );
	if( !ppmGuild ) return NULL;

	AgpdGuild* ppdGuild = ( AgpdGuild* )GetCastleOwnerGuild( eCastleType );
	if( !ppdGuild ) return NULL;

	return ppmGuild->GetGuildMarkTexture( AGPMGUILD_ID_MARK_BOTTOM, ppdGuild->m_lGuildMarkTID, TRUE );
}

void* AcUIWorldmap::GetCastleOwnerGuildMarkPattern( AuCastleOwnerType eCastleType )
{
	AgpmGuild* ppmGuild = ( AgpmGuild* )m_pAgpmCharacter->GetModule( "AgpmGuild" );
	if( !ppmGuild ) return NULL;

	AgpdGuild* ppdGuild = ( AgpdGuild* )GetCastleOwnerGuild( eCastleType );
	if( !ppdGuild ) return NULL;

	return ppmGuild->GetGuildMarkTexture( AGPMGUILD_ID_MARK_PATTERN, ppdGuild->m_lGuildMarkTID, TRUE );
}

void* AcUIWorldmap::GetCastleOwnerGuildMarkSimbol( AuCastleOwnerType eCastleType )
{
	AgpmGuild* ppmGuild = ( AgpmGuild* )m_pAgpmCharacter->GetModule( "AgpmGuild" );
	if( !ppmGuild ) return NULL;

	AgpdGuild* ppdGuild = ( AgpdGuild* )GetCastleOwnerGuild( eCastleType );
	if( !ppdGuild ) return NULL;

	return ppmGuild->GetGuildMarkTexture( AGPMGUILD_ID_MARK_SYMBOL, ppdGuild->m_lGuildMarkTID, TRUE );
}

BOOL AcUIWorldmap::CB_GetSlideBar( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->SetScrollBar( ( AcUIScroll* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleHuman( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkHuman = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleHuman2( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkHuman2 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleHuman3( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkHuman3 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleOrc( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkOrc = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleOrc2( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkOrc2 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleOrc3( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkOrc3 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleMoonElf( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkMoonElf = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleMoonElf2( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkMoonElf2 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleMoonElf3( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkMoonElf3 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleDragonSion( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkDragonSion = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleDragonSion2( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkDragonSion2 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleDragonSion3( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkDragonSion3 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleArchlord( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkArchlord = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleArchlord2( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkArchlord2 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_GetBtnCastleArchlord3( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pData1 ) return FALSE;

	pThis->m_pCastleGuildMarkArchlord3 = ( ( AcUIButton* )( ( AgcdUIControl* )pData1 )->m_pcsBase );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerGuildHuman( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_Human, CastleInfo_OwnerGuildName, pDisplay );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerNameHuman( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_Human, CastleInfo_GuildMasterName, pDisplay );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerGuildOrc( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_Orc, CastleInfo_OwnerGuildName, pDisplay );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerNameOrc( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_Orc, CastleInfo_GuildMasterName, pDisplay );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerGuildMoonElf( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_MoonElf, CastleInfo_OwnerGuildName, pDisplay );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerNameMoonElf( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_MoonElf, CastleInfo_GuildMasterName, pDisplay );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerGuildDragonScion( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_DragonScion, CastleInfo_OwnerGuildName, pDisplay );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerNameDragonScion( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_DragonScion, CastleInfo_GuildMasterName, pDisplay );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerGuildArchlord( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_Archlord, CastleInfo_OwnerGuildName, pDisplay );
	return TRUE;
}

BOOL AcUIWorldmap::CB_OnDisplayOwnerNameArchlord( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	AcUIWorldmap* pThis = ( AcUIWorldmap* )pClass;
	if( !pThis || !pDisplay ) return FALSE;

	pThis->OnUpdateCastleInfo( CastleOwner_Archlord, CastleInfo_GuildMasterName, pDisplay );
	return TRUE;
}

 