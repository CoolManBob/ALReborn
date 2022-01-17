#include "AgcmTextBoard.h"
#include "AgcmIDBoard.h"

AgcmTextBoard::AgcmTextBoard( eBoardType eType )
: AgcmBaseBoard( eType )
{
	SetBoardW( 0 );		SetBoardH( 0 );		SetLifeCount( 0 );		SetTextColor( 0 );	SetTailType( 0 );
	SetLineCount( 0 );	SetFontType( 0 );	SetCharacter( NULL );	SetIDBoard( NULL );

	InitText();
}

AgcmTextBoard::~AgcmTextBoard(void)
{
}

void AgcmTextBoard::Render()
{
	// 말풍선 그리기
	RwRGBA  vColor;

	FLOAT   stX , stY;

	FLOAT	fw , fh;			// 공백을 제외한 실제 픽셀 사이즈
	FLOAT	iw , ih;			// Image Width & height(Include 공백 - texture는 2의 배수사이즈이므로)
	FLOAT	fTailx , fTailw , fTailh;
	FLOAT	fTailIw , fTailIh;
	FLOAT	fx1;


	AgcmIDBoard*	pBoard	= NULL;

	if( m_pIDBoard )
	{
		pBoard		= m_pIDBoard;
	}

	else
	{
		pBoard		= (AgcmIDBoard*)ms_pMng->GetMainCharacter()->pHPBar;
	}

	if( GetDraw() )
	{
		if( !ms_pMng->IsRender( (AgpdCharacter*)pBoard->GetApBase() ) )
		{
			return;
		}
		
		vColor.red		= 210;
		vColor.green	= 215;
		vColor.blue		= 148;
		vColor.alpha = (GetLifeCount() < 1000) ? GetLifeCount() * 255 / 1000 : 255;
		ms_Image2D.SetColor( vColor );
		ms_Image2D.SetUV( 0.f , 0.f , 1.f , 1.f , GetRecipz() );
		ms_Image2D.SetCameraTrans( GetCameraPos() , GetRecipz() , GetScreenPos().z );

		stX = GetScreenPos().x + GetOffset().x;
		stY = GetScreenPos().y + GetOffset().y;

		fw = 6.0f;    fh = 6.0f;     iw = 8.0f;    ih = 8.0f;

		// 좌상단 모서리
		ms_Image2D.SetPos( stX - fw , stY - fh , iw , ih );
		ms_Image2D.Render( rwPRIMTYPETRIFAN , (void*)ms_pMng->GetImgBalloon(0) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(0)) : NULL );

		// 우상단 모서리
		ms_Image2D.SetPos( stX + GetBoardW() , stY - fh , iw , ih );
		ms_Image2D.Render( rwPRIMTYPETRIFAN , (void*)ms_pMng->GetImgBalloon(1) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(1)) : NULL );

		// 좌하단 모서리
		ms_Image2D.SetPos( stX - fw , stY + GetBoardH() , iw , ih );
		ms_Image2D.Render( rwPRIMTYPETRIFAN , (void*)ms_pMng->GetImgBalloon(2) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(2)) : NULL );

		// 우하단 모서리
		ms_Image2D.SetPos( stX + GetBoardW() , stY + GetBoardH() , iw , ih );
		ms_Image2D.Render( rwPRIMTYPETRIFAN , (void*)ms_pMng->GetImgBalloon(3) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(3)) : NULL );

		// 상단 Line
		ms_Image2D.SetPos( stX , stY - fh , (FLOAT)GetBoardW() , ih );
		ms_Image2D.Render( rwPRIMTYPETRIFAN , (void*)ms_pMng->GetImgBalloon(4) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(4)) : NULL );

		// 좌측 Line
		ms_Image2D.SetPos( stX - fw , stY , iw , (FLOAT)GetBoardH() );
		ms_Image2D.Render( rwPRIMTYPETRIFAN , (void*)ms_pMng->GetImgBalloon(5) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(5)) : NULL );

		// 우측 Line
		ms_Image2D.SetPos( stX + GetBoardW() , stY , iw , (FLOAT)GetBoardH() );
		ms_Image2D.Render( rwPRIMTYPETRIFAN , (void*)ms_pMng->GetImgBalloon(6) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(6)) : NULL );

		//하단 Line
		if( GetTailType() == 0 )		// Right
		{
			fTailx = -4.0f;
			fTailw = 9.0f;
			fTailh = 14.0f;

			fTailIw = fTailIh = 16.0f;
		}
		else if( GetTailType() == 1)	// bottom
		{
			fTailx = -4.0f;
			fTailw = 9.0f;
			fTailh = 14.0f;

			fTailIw = fTailIh = 16.0f;
		}
		else if( GetTailType() == 2)	// left
		{
			fTailx = -4.0f;
			fTailw = 9.0f;
			fTailh = 14.0f;

			fTailIw = fTailIh = 16.0f;
		}

		fx1 = GetBoardW() * 0.5f + fTailx;

		ms_Image2D.SetPos( stX , stY + GetBoardH(), fx1, ih );
		ms_Image2D.Render( rwPRIMTYPETRIFAN, (void*)ms_pMng->GetImgBalloon(7) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(7)) : NULL );

		ms_Image2D.SetPos( stX + fx1 + fTailw , stY + GetBoardH() , GetBoardW() - fx1 - fTailw  , ih );
		ms_Image2D.Render( rwPRIMTYPETRIFAN, (void*)ms_pMng->GetImgBalloon(7) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(7)) : NULL );

		ms_Image2D.SetPos( stX + fx1 , stY + GetBoardH() , fTailIw  , fTailIh );
		ms_Image2D.Render( rwPRIMTYPETRIFAN, (void*)ms_pMng->GetImgBalloonTail(GetTailType()) ? RwTextureGetRaster(ms_pMng->GetImgBalloonTail(GetTailType())) : NULL );

		// Center Box
		ms_Image2D.SetPos( stX , stY , (FLOAT)GetBoardW() , (FLOAT)GetBoardH() );
		ms_Image2D.Render( rwPRIMTYPETRIFAN, (void*)ms_pMng->GetImgBalloon(8) ? RwTextureGetRaster(ms_pMng->GetImgBalloon(8)) : NULL);

		INT		nResult	= ms_pMng->GetApmMap()->CheckRegionPerculiarity( ((AgpdCharacter*)pBoard->GetApBase())->m_nBindingRegionIndex , APMMAP_PECULIARITY_SHOWNAME );

		// ID 그리기
		if ( pBoard )
		{
			ms_pMng->GetAgcmFont()->FontDrawStart( pBoard->GetFontType() );


			if( pBoard->GetEnableParty() == 2 )
			{
				if( APMMAP_PECULIARITY_RETURN_DISABLE_USE == nResult )
				{
					ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( stX-2.0f, stY-10.0f, GetScreenPos().z, GetRecipz(), pBoard->GetBattleText() , pBoard->GetFontType() , vColor.alpha, pBoard->GetColor() , true, true, 0x7f8b3f3f );
				}

				else
				{
					ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( stX-2.0f, stY-10.0f, GetScreenPos().z, GetRecipz(), pBoard->GetText() , pBoard->GetFontType() , vColor.alpha, pBoard->GetColor() , true, true, 0x7f8b3f3f );
				}

				
			}
			else
			{
				if( APMMAP_PECULIARITY_RETURN_DISABLE_USE == nResult )
				{
					ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( stX-2.0f, stY-10.0f, GetScreenPos().z, GetRecipz(), pBoard->GetBattleText() , pBoard->GetFontType() , vColor.alpha, pBoard->GetColor() , true);
				}

				else
				{
					ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( stX-2.0f, stY-10.0f, GetScreenPos().z, GetRecipz(), pBoard->GetText() , pBoard->GetFontType() , vColor.alpha, pBoard->GetColor() , true);
				}
				
			}

			if( !ms_pMng->GetAgpmBattleGround()->IsInBattleGround( (AgpdCharacter*)pBoard->GetApBase() ) && APMMAP_PECULIARITY_RETURN_DISABLE_USE != nResult )
			{
				if( pBoard->GetEnableGuild() == 2)
					ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( stX-2.0f, stY-10.0f+m_pIDBoard->GetOffset().y, pBoard->GetScreenPos().z, pBoard->GetRecipz(), pBoard->GetGuildText(), pBoard->GetFontType(), vColor.alpha, pBoard->GetColorGuild() , true );
			}

			// Text 그리기
			stX =  GetScreenPos().x + GetOffset().x + 5.0f;
			stY =  GetScreenPos().y + GetOffset().y + 8.0f;
			fh  =  (float)ms_pMng->GetAgcmFont()->m_iYL[ GetFontType() ];

			for( INT i=0 ; i < GetLineCount() ; ++i )
			{
				ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( stX, stY, GetScreenPos().z , GetRecipz() , GetText(i) , GetFontType() , vColor.alpha, GetTextColor(), true );
				stY += fh;
			}

			ms_pMng->GetAgcmFont()->FontDrawEnd();
		}
	}

}

BOOL AgcmTextBoard::Update( IN DWORD tickDiff , IN RwMatrix* CamMat )
{

	if( m_pIDBoard   == NULL ) return FALSE;

	SetLifeCount( GetLifeCount() - tickDiff );

	if( GetLifeCount() < 0 )
	{
		if( m_pIDBoard->GetIDType() & TB_MONSTER )
		{
			if( ms_pMng->GetDrawNameMonster() ) m_pIDBoard->SetEnabled( TRUE );
		}
		else if( m_pIDBoard->GetIDType() & TB_OTHERS )
		{
			if( ms_pMng->GetDrawNameOther() ) m_pIDBoard->SetEnabled( TRUE );
		}
		else if( m_pIDBoard->GetIDType() & TB_MAINCHARACTER )
		{
			if( ms_pMng->GetDrawNameMine() ) m_pIDBoard->SetEnabled( TRUE );
		}

		if( ms_pMng->GetDrawNameParty() && m_pIDBoard->GetEnableParty() == 2 ) m_pIDBoard->SetEnableParty( 1 );
		if( ms_pMng->GetDrawNameGuild() && m_pIDBoard->GetEnableGuild() == 2 ) m_pIDBoard->SetEnableGuild( 1 );

		return FALSE;
	}
			
	else
	{
		// 1차이는 reflection그려줄때 tick을 1 줄이므로 허용
		if( GetClump() && ms_pMng->GetLastTick() - GetClump()->stUserData.calcDistanceTick < 2 )
		{
			ms_pMng->Trans3DTo2D( &m_CameraPos , &m_ScreenPos , &m_Recipz , (const RwV3d*)&m_pCharacter->m_stPos , m_Height + 30.0f );
			SetDraw( TRUE );
		}
		else
		{
			SetDraw( FALSE );
		}
	}



	return TRUE;
}

void AgcmTextBoard::InitText()
{
	for( INT i = 0 ; i < TB_TEXT_MAX_CHARLINE ; ++i )
	{
		ZeroMemory( m_Text[ i ] , TB_TEXT_MAX_CHAR+2 );
	}
}