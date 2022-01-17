#include "AgcmDmgBoard.h"
#include "AgcmHPBar.h"

AgcmDmgBoard::AgcmDmgBoard( eBoardType eType )
: AgcmBaseBoard( eType )
{
	ZeroMemory( m_Text  , TB_DAMAGE_MAX_NUM   );
	memset    ( &m_Color , 255 , sizeof(m_Color) );

	SetNumCount( 0 );	SetCharacter( NULL );	SetAniCount( 0 );	SetAniEndCount( 0 );
}

AgcmDmgBoard::~AgcmDmgBoard(void)
{
}

void AgcmDmgBoard::Render()
{
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void *) FALSE );
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void *) FALSE );		//. ztest disable

	
	switch( GetBoardType() )
	{
	case AGCM_BOARD_DAMAGE:				_DrawDamage();				break;
	case AGCM_BOARD_CRITICALDAMAGE:		_DrawCriticalDamage();		break;
	}


	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE , (VOID*) TRUE );
}

BOOL AgcmDmgBoard::Update( IN DWORD tickDiff , IN RwMatrix* CamMat )
{

	RwV3d	wPos;
	float	upoffset  = 0;

	SetAniCount( GetAniCount() + tickDiff );
	
	if( GetAniCount() < 0 ) return FALSE;
	else if( GetAniCount() > GetAniEndCount() )
	{
		return FALSE;
	}
	else
	{
		SetDraw( FALSE );

		if( GetClump() && ms_pMng->GetLastTick() - GetClump()->stUserData.calcDistanceTick < 2 )
		{

			// 일반 데미지 일때만.. 값이 위로 올라가게 한다
			// 크리데미지는 안 올라간다
			if( GetBoardType() == AGCM_BOARD_DAMAGE ) 
			{
				upoffset  = (FLOAT)GetAniCount() / (FLOAT)GetAniEndCount() * 40.0f;
			}

			AgcdCharacter* pcsCharacter  = ms_pMng->GetAgcmCharacter()->GetCharacterData( GetCharacter() );
			
			if( pcsCharacter )
			{
				SetDraw( TRUE );
				if( pcsCharacter->m_pPickAtomic != pcsCharacter->m_pPickAtomic->next )
				{
					wPos.x   = pcsCharacter->m_pPickAtomic->boundingSphere.center.x;
					wPos.y   = pcsCharacter->m_pPickAtomic->boundingSphere.center.y + upoffset;
					wPos.y   = pcsCharacter->m_pPickAtomic->boundingSphere.center.z;

					RwFrame* pFrame   = RpClumpGetFrame( pcsCharacter->m_pClump );
					if( pFrame )   RwV3dTransformPoint( &wPos , &wPos , RwFrameGetMatrix(pFrame) );

					ms_pMng->Trans3DTo2D( &m_CameraPos , &m_ScreenPos , &m_Recipz , &wPos );
				}
				else
				{
					ms_pMng->Trans3DTo2D( &m_CameraPos, &m_ScreenPos, &m_Recipz, (const RwV3d*)&GetCharacter()->m_stPos, m_Height + 15.0f + upoffset );
				}
			}

		}
	}
	
	return TRUE;
}


void AgcmDmgBoard::_DrawDamage( VOID )
{
	FLOAT	stX  = 0;
	FLOAT	stY  = 0;
	FLOAT	Per  = 0;
	RwRGBA	vColor;

	Per = (FLOAT) GetAniCount() / (FLOAT) GetAniEndCount();
	vColor.alpha = Per < 0.5f ? 255 : 255 - (INT32) (((Per-0.5f)*2.0f) * 255);

	ms_Image2D.SetColor( vColor.alpha , GetColor().red , GetColor().green , GetColor().blue );
	ms_Image2D.SetCameraTrans( GetCameraPos() , GetRecipz() , GetScreenPos().z );

	stX		= GetScreenPos().x + GetOffset().x;
	stY		= GetScreenPos().y + GetOffset().y;

	for( INT i = 0 ; i < GetNumCount() ; ++i )
	{
		ms_Image2D.SetUV( ms_pMng->GetDamageUVW( m_Text[i] ).x , 0.0f , 
			ms_pMng->GetDamageUVW( m_Text[i] ).y , 1.0f , GetRecipz() );

		if( ms_pMng->GetMainCharacter() && ms_pMng->GetMainCharacter()->pHPBar && GetClump() == ms_pMng->GetMainCharacter()->pHPBar->GetClump() )
		{
			ms_Image2D.SetPos( stX , stY , ms_pMng->GetDamageSizeSmall( m_Text[i] ) , DAMAGE_SMALL_HEIGHT );
			stX		-= ms_pMng->GetDamageSizeSmall( m_Text[i] );
		}
		else
		{
			ms_Image2D.SetPos( stX , stY , ms_pMng->GetDamageUVW( m_Text[i] ).z , DAMAGE_HEIGHT );
			stX		-= ms_pMng->GetDamageUVW( m_Text[i] ).z;
		}

		RwTexture* pTexture = ms_pMng->GetImgDamage();
		if( pTexture )
		{
			ms_Image2D.Render( rwPRIMTYPETRIFAN , RwTextureGetRaster( pTexture ) );
		}
	}
}

void AgcmDmgBoard::_DrawCriticalDamage( VOID )
{
	FLOAT	fCriticalSize		= 0;
	FLOAT	fCriticalWidth		= 0;
	FLOAT	fCriticalHeight		= 0;
	FLOAT	Per					= 0;
	FLOAT   stX					= 0;
	FLOAT	stY					= 0;
	RwRGBA	vColor				= { 0 , 0 , 0 , 0 };

	Per = ( FLOAT )GetAniCount() / ( FLOAT )GetAniEndCount();

	if( Per < 0.15f )
	{
		vColor.alpha	=	0.5f + Per * 853.3f;
		fCriticalSize	=	1.3f - Per * 2.0f;
	}
	else if( Per > 0.85f)
	{
		vColor.alpha	= 255	- ( Per - 0.85f ) * 1000.0f;
		fCriticalSize	= 1.0f	+ (	Per - 0.85f ) * ( Per - 0.85f ) * 1.5f;
	}
	else
	{
		vColor.alpha	= 255;
		fCriticalSize	= 1.0f;
	}

	fCriticalHeight = DAMAGE_HEIGHT * fCriticalSize;

	ms_Image2D.SetColor( vColor.alpha , GetColor().red , GetColor().green , GetColor().blue );
	ms_Image2D.SetCameraTrans( GetCameraPos() , GetRecipz() , GetScreenPos().z );

	stX		= GetScreenPos().x	+	GetOffset().x	*	fCriticalSize;
	stY		= GetScreenPos().y	+	GetOffset().y	*	fCriticalSize * 0.5f;

	for( INT i = 0 ; i < GetNumCount() ; ++i )
	{
		ms_Image2D.SetUV( ms_pMng->GetDamageUVW( m_Text[i] ).x , 0.0f , 
							ms_pMng->GetDamageUVW( m_Text[i] ).y , 1.0f , GetRecipz() );

		fCriticalWidth		= ms_pMng->GetDamageUVW( m_Text[i] ).z	*	fCriticalSize;
		ms_Image2D.SetPos( stX , stY , fCriticalWidth , fCriticalHeight );

		stX		-=	fCriticalWidth;
		ms_Image2D.Render( rwPRIMTYPETRIFAN , RwTextureGetRaster( ms_pMng->GetImgDamage() ) );
	}

}