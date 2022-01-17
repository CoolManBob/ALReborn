#include "AgcmHPBar.h"

AgcmHPBar::AgcmHPBar( eBoardType eType)
: AgcmIDBoard( eType )
{
	SetDrawHPMP( FALSE );
	SetDecreaseHP( FALSE );
	SetDecreaseMP( FALSE );
	SetRemove( FALSE );
	SetRemoveCount( 0 );
}

AgcmHPBar::~AgcmHPBar(void)
{
}

void AgcmHPBar::SetData( IN AgcmIDBoard* pIDBoard )
{
	SetPosition( pIDBoard->GetPosition() );
	SetCameraPos( pIDBoard->GetCameraPos() );
	SetScreenPos( pIDBoard->GetScreenPos() );
	SetRecipz( pIDBoard->GetRecipz() );
	SetDraw( pIDBoard->GetDraw() );
	SetIDType( TB_MAINCHARACTER );
	SetApBase( pIDBoard->GetApBase() );
	SetClump( pIDBoard->GetClump() );
	SetEnabled( pIDBoard->GetEnabled() );
	SetFontType( pIDBoard->GetFontType() );
	SetDepth( pIDBoard->GetDepth() );
	SetHeight( pIDBoard->GetHeight() );
	SetOffset( pIDBoard->GetOffset() );
	SetColor( pIDBoard->GetColor() );
	SetEnableParty( pIDBoard->GetEnableParty() );
	SetParty( pIDBoard->GetParty() );
	SetEnableGuild( pIDBoard->GetEnableGuild() );
	SetGuild( pIDBoard->GetGuild() );
	SetColorGuild( pIDBoard->GetColorGuild() );
	SetOffsetGuild( pIDBoard->GetOffsetGuild() );
	SetOffsetTitle( pIDBoard->GetOffsetTitle() );
	SetCharismaStep( pIDBoard->GetCharismaStep() );
	
	SetText			( pIDBoard->GetText() );
	SetGuildText	( pIDBoard->GetGuildText() );
	SetCharismaText	( pIDBoard->GetCharismaText() );
	SetNickNameText	( pIDBoard->GetNickNameText() );
	SetTitleNameText( pIDBoard->GetTitleNameText() );

	ReCalulateOffsetX();
}

BOOL AgcmHPBar::Update( IN DWORD tickDiff , IN RwMatrix* CamMat )
{

	switch( GetBoardType() )
	{
	case AGCM_BOARD_HPBAR:				_HPBarUpdate	( tickDiff , CamMat );		break;
	case AGCM_BOARD_HPMPBAR:			_HPMPBarUpdate	( tickDiff , CamMat );		break;

	}

	return TRUE;
}	

void AgcmHPBar::Render()
{
	switch( GetBoardType() )
	{
	case AGCM_BOARD_HPBAR:			_DrawHPBar();	break;
	case AGCM_BOARD_HPMPBAR:		_DrawHPMPBar();	break;
	}

}

BOOL AgcmHPBar::_HPBarUpdate( DWORD tickDiff , RwMatrix* CamMat )
{
	if( GetRemove() )
	{
		SetRemoveCount( GetRemoveCount() - tickDiff );
	}

	if( GetRemoveCount() < 0 )
	{
		return FALSE;
	}

	else
	{
		SetDraw( FALSE );
		if( GetClump() && ms_pMng->GetLastTick() - GetClump()->stUserData.calcDistanceTick < 2 )
		{
			AgpdCharacter* pdCharacter   = static_cast< AgpdCharacter* >( GetApBase() );
			if( pdCharacter == NULL ) return FALSE;

			AgcdCharacter* cdCharacter   = ms_pMng->GetAgcmCharacter()->GetCharacterData(pdCharacter);
			if( cdCharacter == NULL ) return FALSE;

			FLOAT fDepth    = cdCharacter->GetDepth();
			ms_pMng->Trans3DTo2D( &m_CameraPos , &m_ScreenPos , &m_Recipz , (const RwV3d*)m_pPosition , m_Height , GetClump() , fDepth );

			SetDraw( TRUE );
			if( pdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
			{
				if( m_PerResultHP != 0.0f )
				{
					m_bDecreaseHP       = TRUE;
					m_PerAfterImageHP   = m_PerResultHP;
					m_SpeedAfterImageHP = AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_SPEED;
					m_PerResultHP       = 0.0f;
					return TRUE;
				}
			}

			if( m_bDecreaseHP )
			{
				m_PerAfterImageHP     -=  m_SpeedAfterImageHP * tickDiff;
				m_SpeedAfterImageHP   *= (1.0f + AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_ACCELERATION * tickDiff );

				if( m_PerResultHP >= m_PerAfterImageHP )
				{
					m_bDecreaseHP    = FALSE;
				}
			}
		}
	}

	return TRUE;

}

BOOL AgcmHPBar::_HPMPBarUpdate( DWORD tickDiff , RwMatrix* CamMat )
{
	SetDrawHPMP( FALSE );
	
	AgpdCharacter *	pdCharacter = (AgpdCharacter *)GetApBase();
	if (!pdCharacter)		return FALSE;

	SetDrawHPMP( TRUE );

	ms_pMng->Trans3DTo2D( &GetCamPosHPMP() , &GetScrPosHPMP() , GetRecipzHPMPPtr() , (const RwV3d*)GetPosition() , 0.f, GetClump() , GetDepth() );

	if ( pdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
	{
		if( GetPerResultHP() != 0.0f )
		{
			SetDecreaseHP( TRUE );
			SetPerAfterImageHP( GetPerResultHP() );
			SetSpeedAfterImageHP( AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_SPEED );
			SetPerResultHP( 0.0f );
		}
	}

	if( GetDecreaseHP() )
	{
		SetPerAfterImageHP( GetPerAfterImageHP() - GetSpeedAfterImageHP() * tickDiff );
		SetSpeedAfterImageHP( GetSpeedAfterImageHP() * (1.0f + AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_ACCELERATION * tickDiff) );

		if( GetPerResultHP() >= GetPerAfterImageHP() )
		{
			SetDecreaseHP( FALSE );
		}
	}

	if( GetDecreaseMP() )
	{
		SetPerAfterImageMP( GetPerAfterImageMP() - GetSpeedAfterImageMP()*tickDiff );
		SetSpeedAfterImageMP( GetSpeedAfterImageMP() * (1.0f + AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_ACCELERATION * tickDiff) );

		if( GetPerResultMP() >= GetPerAfterImageMP() )
		{
			SetDecreaseMP( FALSE );
		}
	}

	return TRUE;
}


void AgcmHPBar::_DrawHPBar()
{
	RwRGBA		vColor	= { 255 , 255 , 255 , 255 };
	FLOAT		stX		=	0;
	FLOAT		stY		=	0;

	ms_Image2D.SetColor( vColor );
	if( GetDraw() )
	{
		ms_Image2D.SetCameraTrans( GetCameraPos() , GetRecipz() , GetScreenPos().z );

		stX = GetScreenPos().x + GetOffset().x;
		stY = GetScreenPos().y + GetOffset().y;

		ms_Image2D.SetPos( stX, stY,120, 7 );
		ms_Image2D.SetUV( 0.f, 0.f, 1.f, 1.f, GetRecipz() );
		ms_Image2D.Render( rwPRIMTYPETRIFAN, RwTextureGetRaster( ms_pMng->GetImgHPBar(0) ) );

		if( GetDecreaseHP() )
		{
			ms_Image2D.SetColor( 128, vColor.red, vColor.green, vColor.blue );
			ms_Image2D.SetPos( stX, stY, (120.0f * GetPerAfterImageHP()), 7 );
			ms_Image2D.Render( rwPRIMTYPETRIFAN, RwTextureGetRaster( ms_pMng->GetImgHPBar(1)) );

			ms_Image2D.SetColor( vColor );
			ms_Image2D.SetPos( stX, stY, (120.0f * GetPerResultHP()), 7 );
			ms_Image2D.Render( rwPRIMTYPETRIFAN, RwTextureGetRaster( ms_pMng->GetImgHPBar(1)) );
		}
		else
		{
			ms_Image2D.SetPos( stX , stY , (120.0f * GetPerResultHP()), 7 );
			ms_Image2D.Render( rwPRIMTYPETRIFAN, RwTextureGetRaster( ms_pMng->GetImgHPBar(1) ) );
		}
	}
}

void AgcmHPBar::_DrawHPMPBar()
{
	RwV2d   Offset = { 0.0f , 0.0f };
	FLOAT	stX		=	0;
	FLOAT	stY		=	0;

	if( IsBadReadPtr( this , sizeof(*this) ) )   return ;
	if( GetVisible() == FALSE )                  return ;

	AgpdCharacter* pdCharacter   = static_cast< AgpdCharacter* >( m_pBase );
	//AgpdCharacter* pdSelfCharacter	= ms_pMng->GetAgcmCharacter()->GetSelfCharacter();

	if( ms_pMng->IsRender( pdCharacter )  == FALSE )
	{
		return;
	}

	//RwTexture* pTexStatus   = ms_pMng->GetStateTexture( pdCharacter );
	//if( pTexStatus )
	//{
	//	Offset.x   = -RwRasterGetWidth( RwTextureGetRaster( pTexStatus ) ) * 0.5f;
	//	Offset.y   = m_OffSet.y  -  RwRasterGetHeight( RwTextureGetRaster( pTexStatus) ) - 8.0f;
	//	ms_pMng->RenderTexture( pTexStatus , m_CameraPos , m_ScreenPos , m_Recipz , &Offset );
	//}

	//RwTexture* pTexCastleOwner  = ms_pMng->GetCastleTexture( pdCharacter->m_ulSpecialStatus );
	//if( pTexCastleOwner )
	//{
	//	Offset.x   = -RwRasterGetWidth( RwTextureGetRaster(pTexCastleOwner) ) * 0.5f;
	//	FLOAT fOffsetY  = pdCharacter->m_szNickName[0] ? 46.0f : 34.0f;
	//	Offset.y        = m_OffSet.y - RwRasterGetHeight( RwTextureGetRaster( pTexCastleOwner ) ) - fOffsetY;
	//	ms_pMng->RenderTexture( pTexCastleOwner , m_CameraPos , m_ScreenPos , m_Recipz , &Offset );
	//}

	// HPMP Bar
	if( ms_pMng->GetEnableMiniHPBar() && ms_pMng->GetDrawHPMP() && GetDrawHPMP() )
	{
		stX		=	GetScrPosHPMP().x	+	GetOffsetHPMP().x;
		stY		=	GetScrPosHPMP().y	+	GetOffsetHPMP().y;

		g_pEngine->DrawIm2D( ms_pMng->GetImgHPBar(0) , stX , stY , 120 , 7, 0.0f , 0.0f , 1.0f ,1.0f );

		if( GetDecreaseHP() )
		{
			g_pEngine->DrawIm2D( ms_pMng->GetImgHPBar(1) , stX , stY , (120.0f * GetPerAfterImageHP()) , 7 , 0.0f , 0.0f , 1.0f , 1.0f , 0xffffffff , 128 );
		}
		g_pEngine->DrawIm2D( ms_pMng->GetImgHPBar(1) , stX , stY , (120.0f * GetPerResultHP()) , 7 , 0.0f , 0.0f , 1.0f , 1.0f );

		stY	+= 7;
		g_pEngine->DrawIm2D( ms_pMng->GetImgHPBar(0) , stX , stY , 120 , 7 , 0.0f , 0.0f , 1.0f , 1.0f );

		if( GetDecreaseMP() )
		{
			g_pEngine->DrawIm2D( ms_pMng->GetImgHPBar(2) , stX , stY , (120.0f * GetPerAfterImageMP()) , 7 , 0.0f , 0.0f , 1.0f , 1.0f , 0xffffffff , 128 );
		}
		g_pEngine->DrawIm2D( ms_pMng->GetImgHPBar(2) , stX , stY , (120.0f * GetPerResultMP()) , 7 , 0.0f , 0.0f , 1.0f , 1.0f );
	}

	// ID
	if( GetEnabled() == FALSE )			return;

	DrawChar();
	//INT		nResult		=	0;
	//BOOL	bDraw		=	TRUE;
	//INT32	nType		= ms_pMng->GetAgpmFactors()->GetRace( &pdCharacter->m_csFactor );
	//INT32	nSelf		= ms_pMng->GetAgpmFactors()->GetRace( &pdSelfCharacter->m_csFactor );

	//ms_pMng->CheckPeculiarity( pdSelfCharacter , nResult );
	//switch( nResult )
	//{
	//case APMMAP_PECULIARITY_RETURN_DISABLE_USE:	
	//	bDraw	=	FALSE;
	//	break;

	//case APMMAP_PECULIARITY_RETURN_ENABLE_USE_UNION:
	//	if( (AgpdCharacter*)m_pBase != ms_pMng->GetAgcmCharacter()->GetSelfCharacter() )
	//	{
	//		if( nType != nSelf )   
	//			bDraw   = FALSE;
	//	}
	//	break;
	//}

	//stX		= GetScreenPos().x	+ GetOffset().x;
	//stY		= GetScreenPos().y	+ GetOffset().y;

	//ms_pMng->GetAgcmFont()->FontDrawStart( GetFontType() );

	//_DrawName		( stX , stY , GetAlpha() , TRUE );

	//if( bDraw )
	//{
	//	_DrawTitleName  ( stX , stY , GetAlpha() );
	//	_DrawGoMark		( stX , stY , GetAlpha() );
	//	_DrawGuild		( stX , stY , GetAlpha() );
	//	_DrawNameEx		( stX , stY , GetAlpha() );
	//	_DrawCharisma	( stX , stY , GetAlpha() );
	//	_DrawMonsterAbility( stX , stY , GetAlpha() );
	//	_DrawFlag();
	//}

	//ms_pMng->GetAgcmFont()->FontDrawEnd();

	//Offset.x	=  GetOffsetXRight() + 2;
	//Offset.y	=  GetOffset().y - 1;

	//for( INT i = 0 ; i < TB_MAX_TAG ; ++i )
	//{
	//	if( ms_pMng->GetImgIDTag(i) && GetFlag() & (1<<i) )
	//	{
	//		ms_pMng->RenderTexture( ms_pMng->GetImgIDTag(i) , GetCameraPos() , GetScreenPos() , GetRecipz() , &Offset );
	//		Offset.x	+=	TBID_TAG_SIZE;
	//	}
	//}
}