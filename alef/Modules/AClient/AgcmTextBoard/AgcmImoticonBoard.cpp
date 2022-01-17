#include "AgcmImoticonBoard.h"

AgcmImoticonBoard::AgcmImoticonBoard( eBoardType eType )
: AgcmBaseBoard( eType )
{
	SetCharacter( NULL );	SetImageID( 0 );	SetAniType( 0 );	SetAniCount( 0 );	SetAniEndCount( 0 );	

	ZeroMemory( m_AniParam	,	sizeof(m_AniParam)		);
	ZeroMemory( m_AniVal	,	sizeof(m_AniVal)		);
}

AgcmImoticonBoard::~AgcmImoticonBoard(void)
{
}

BOOL AgcmImoticonBoard::Update( IN DWORD tickDiff , IN RwMatrix* CamMat )
{
	
	SetAniCount( GetAniCount() + tickDiff );
	
	if( GetAniCount() > GetAniEndCount() )
	{
		return FALSE;
	}

	RwV3d wPos;

	SetDraw( FALSE );

	if( GetClump() && ms_pMng->GetLastTick() - GetClump()->stUserData.calcDistanceTick < 2 )
	{
		AgcdCharacter* pcsCharacter = ms_pMng->GetAgcmCharacter()->GetCharacterData( m_KeyObj );
		if( pcsCharacter )
		{
			SetDraw( TRUE );

			if( pcsCharacter->m_pPickAtomic != pcsCharacter->m_pPickAtomic->next )
			{
				wPos.x     = pcsCharacter->m_pPickAtomic->boundingSphere.center.x;
				wPos.y     = pcsCharacter->m_pPickAtomic->boundingSphere.center.y;
				wPos.z     = pcsCharacter->m_pPickAtomic->boundingSphere.center.z;

				RwFrame* pFrame = RpClumpGetFrame( pcsCharacter->m_pClump );
				if( pFrame )    RwV3dTransformPoint( &wPos, &wPos, RwFrameGetMatrix(pFrame) );

				ms_pMng->Trans3DTo2D( &m_CameraPos , &m_ScreenPos , &m_Recipz , &wPos );
			}

			else
			{
				ms_pMng->Trans3DTo2D( &m_CameraPos , &m_ScreenPos , &m_Recipz , (const RwV3d*)&m_KeyObj->m_stPos , m_Height );
			}
		}

	}

	return TRUE;
}

void AgcmImoticonBoard::Render()
{

	FLOAT				stX		=	0;
	FLOAT				stY		=	0;
	RwRGBA				vColor	=	{ 0 , 0 , 0 , 0 };

	RwIm2DVertex		BoxFan[ 4 ];

	switch( GetBoardType() )
	{
		// 확대 되며 서서히 올라가며 사라짐
	case IMOTICON_MISS:
	case IMOTICON_EVADE:
	case IMOTICON_DODGE:
	case IMOTICON_REFLEX:
		{
			vColor.red = vColor.green = vColor.blue = 255;

			FLOAT		width_scaleval	= 0.0f;
			FLOAT		height_scaleval	= 0.0f;

			FLOAT		width			= 64.0f;
			FLOAT		height			= 64.0f;

			ms_Image2D.SetCameraTrans( GetCameraPos() , GetRecipz() , GetScreenPos().z );
			ms_Image2D.SetUV( 0.0f , 0.0f , 1.0f , 1.0f , GetRecipz() );

			FLOAT		f1	= TB_IMOTICON_HEIGHT_OFFSET	+	GetAniCount()	*	( 14.0f / (FLOAT)GetAniEndCount() );

			if( GetAniCount() > GetAniParam( 0 ) )
			{
				FLOAT fPer		= GetPer();
				vColor.alpha	= 255 - fPer * 255;
				ms_Image2D.SetColor( vColor );	

				width_scaleval	= GetAniVal(0)	* fPer;
				height_scaleval = GetAniVal(1)	* fPer;

				FLOAT s1	= width	 * fPer * 0.3f;
				FLOAT s2	= height * fPer * 0.3f;

				stX		=	GetScreenPos().x	+	GetOffset().x	+	s1;
				stY		=	GetScreenPos().y	+	GetOffset().y	-	f1	+	s2;

				ms_Image2D.SetPos( stX , stY , width - s1 * 2.0f , height - s2 * 2.0f );
				ms_Image2D.Render( rwPRIMTYPETRIFAN ,ms_pMng->GetImgImoticon( GetImageID() ) ? RwTextureGetRaster( ms_pMng->GetImgImoticon( GetImageID() ) ) : NULL );
			
			}
			else
				vColor.alpha = 255;
			
			ms_Image2D.SetColor( vColor );

			stX = GetScreenPos().x + GetOffset().x - width_scaleval * 0.5f;
			stY = GetScreenPos().y + GetOffset().y - f1 - height_scaleval *	0.5f;

			ms_Image2D.SetPos( stX , stY , width + width_scaleval , height + height_scaleval );
			ms_Image2D.Render( rwPRIMTYPETRIFAN ,ms_pMng->GetImgImoticon( GetImageID() ) ? RwTextureGetRaster( ms_pMng->GetImgImoticon( GetImageID() ) ) : NULL );
		}
		break;
	case IMOTICON_CRITICAL:
	case IMOTICON_ATTACK: 
	case IMOTICON_CHARISMA1:
	case IMOTICON_CHARISMA2:
	case IMOTICON_CHARISMA3:
		{
			for( INT i = 0; i < 4 ; ++i)
			{
				RwIm2DVertexSetCameraX( &BoxFan[i] , GetCameraPos().x );
				RwIm2DVertexSetCameraY( &BoxFan[i] , GetCameraPos().y );
				RwIm2DVertexSetCameraZ( &BoxFan[i] , GetCameraPos().z );

				RwIm2DVertexSetRecipCameraZ( &BoxFan[i] , GetRecipz() );
				RwIm2DVertexSetScreenZ( &BoxFan[i] , GetScreenPos().z );
			}

			FLOAT		width = 64.0f;
			FLOAT		height = 64.0f;

			vColor.red = vColor.green = vColor.blue = 255;
			if( GetAniCount() > GetAniParam(0) )
			{
				vColor.alpha = 255 - GetPer() * 255;

				for( INT i=0; i < 4 ; ++i )	
				{
					RwIm2DVertexSetIntRGBA(&BoxFan[i], vColor.red, vColor.green, vColor.blue, vColor.alpha);
				}

				if( GetAniParam(1) == 0 )			// 안쪼갰으면 쪼개기!
				{
					FLOAT	hwidth	= width * 0.5f;
					INT		dir		= rand() % 2;
					INT		temp	= rand() % (int)(width * 0.25f);

					if(dir == 0) // 왼쪽사선 
					{
						SetAniVal( 0 , hwidth+temp );
						SetAniVal( 1 , hwidth-temp );
						SetAniParam( 1 , 1 );
					}
					else if(dir == 1) // 오른쪽사선
					{
						SetAniVal( 0 , hwidth - temp );
						SetAniVal( 1 , hwidth + temp );
						SetAniParam( 1 , 2 );
					}

					SetAniVal( 2 , GetAniVal( 0 ) / width );
					SetAniVal( 3 , GetAniVal( 1 ) / width );
				}

				// 왼쪽 조각
				RwIm2DVertexSetU(&BoxFan[0], 0.0f, GetRecipz() );
				RwIm2DVertexSetV(&BoxFan[0], 0.0f, GetRecipz() );

				RwIm2DVertexSetU(&BoxFan[1], GetAniVal(2) , GetRecipz() );
				RwIm2DVertexSetV(&BoxFan[1], 0.0f, GetRecipz() );

				RwIm2DVertexSetU(&BoxFan[2], GetAniVal(3) , GetRecipz() );
				RwIm2DVertexSetV(&BoxFan[2], 1.0f, GetRecipz() );

				RwIm2DVertexSetU(&BoxFan[3], 0.0f, GetRecipz() );
				RwIm2DVertexSetV(&BoxFan[3], 1.0f, GetRecipz() );

				stX = GetScreenPos().x + GetOffset().x - 4.0f * GetPer();
				stY = GetScreenPos().y + GetOffset().y - TB_IMOTICON_HEIGHT_OFFSET - 3.0f * GetPer();

				RwIm2DVertexSetScreenX( &BoxFan[0] , stX);
				RwIm2DVertexSetScreenY( &BoxFan[0] , stY); 

				RwIm2DVertexSetScreenX(&BoxFan[1] , stX + GetAniVal(0) );
				RwIm2DVertexSetScreenY(&BoxFan[1] , stY ); 

				RwIm2DVertexSetScreenX(&BoxFan[2] , stX + GetAniVal(1) );
				RwIm2DVertexSetScreenY(&BoxFan[2] , stY + height); 

				RwIm2DVertexSetScreenX(&BoxFan[3] , stX );
				RwIm2DVertexSetScreenY(&BoxFan[3] , stY + height); 

				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, ms_pMng->GetImgImoticon( GetImageID() ) ? RwTextureGetRaster( ms_pMng->GetImgImoticon( GetImageID() ) ) : NULL );
				RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, BoxFan, 4);

				// 오른쪽 조각
				RwIm2DVertexSetU(&BoxFan[0], GetAniVal(2) , GetRecipz() );
				RwIm2DVertexSetV(&BoxFan[0], 0.0f , GetRecipz() );

				RwIm2DVertexSetU(&BoxFan[1], 1.0f, GetRecipz() );
				RwIm2DVertexSetV(&BoxFan[1], 0.0f, GetRecipz() );

				RwIm2DVertexSetU(&BoxFan[2], 1.0f, GetRecipz() );
				RwIm2DVertexSetV(&BoxFan[2], 1.0f, GetRecipz() );

				RwIm2DVertexSetU(&BoxFan[3], GetAniVal(3), GetRecipz() );
				RwIm2DVertexSetV(&BoxFan[3], 1.0f, GetRecipz() );

				stX = GetScreenPos().x + GetOffset().x + 8.0f * GetPer();
				stY = GetScreenPos().y + GetOffset().y - TB_IMOTICON_HEIGHT_OFFSET + 6.0f * GetPer();

				RwIm2DVertexSetScreenX(&BoxFan[0] , stX + GetAniVal(0) );
				RwIm2DVertexSetScreenY(&BoxFan[0] , stY); 

				RwIm2DVertexSetScreenX(&BoxFan[1] , stX + width );
				RwIm2DVertexSetScreenY(&BoxFan[1] , stY ); 

				RwIm2DVertexSetScreenX(&BoxFan[2] , stX + width );
				RwIm2DVertexSetScreenY(&BoxFan[2] , stY + height); 

				RwIm2DVertexSetScreenX(&BoxFan[3] , stX + GetAniVal(1) );
				RwIm2DVertexSetScreenY(&BoxFan[3] , stY + height ); 

				RwRenderStateSet(rwRENDERSTATETEXTURERASTER ,ms_pMng->GetImgImoticon( GetImageID() ) ? RwTextureGetRaster( ms_pMng->GetImgImoticon( GetImageID() ) ) : NULL );
				RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, BoxFan, 4);
			}
			else
			{
				vColor.alpha = 255;
				ms_Image2D.SetColor( vColor );
				ms_Image2D.SetUV( 0.f, 0.f, 1.f, 1.f, GetRecipz() );

				stX = GetScreenPos().x + GetOffset().x;
				stY = GetScreenPos().y + GetOffset().y - TB_IMOTICON_HEIGHT_OFFSET;

				ms_Image2D.SetPos( stX , stY , width, height );
				ms_Image2D.Render( rwPRIMTYPETRIFAN , ms_pMng->GetImgImoticon( GetImageID() ) ? RwTextureGetRaster( ms_pMng->GetImgImoticon( GetImageID() ) ) : NULL );
			}
		}
		break;
	case IMOTICON_BLOCK:
		{
			ms_Image2D.SetCameraTrans( GetCameraPos() , GetRecipz() , GetScreenPos().z );

			FLOAT	width	= 64.0f;
			FLOAT	height	= 64.0f;

			vColor.red = vColor.green = vColor.blue = 255;

			if( GetAniCount() > GetAniParam(0) )
			{
				vColor.alpha = 255 - GetPer() * 255;
				ms_Image2D.SetColor( vColor );

				if( GetPer() < 0.5f)		// 전체가 진동 
				{
					if( GetAniParam(1) == 0 && GetAniVal(0) > GetAniVal(2) )
					{
						SetAniParam( 1 , 1 );
						SetAniVal( 2 , GetAniVal(2) * -1.4f );
						SetAniVal( 1 , GetAniVal(1) * -1.5f );

					}
					else if( GetAniParam(1) == 1 && GetAniVal(0) < GetAniVal(2) )
					{
						SetAniParam( 1 , 0 );
						SetAniVal( 2 , GetAniVal(2) * -1.4f );
						SetAniVal( 1 , GetAniVal(1) * -1.5f );
					}

					if( GetAniParam(1) == 0 )
					{
						SetAniVal( 0 , GetAniVal(0) + ( GetAniVal(1)+0.3f ));
					}
					else if( GetAniParam(1) == 1)
					{
						SetAniVal( 0 , GetAniVal(0) + ( GetAniVal(1)-0.3f ));
					}

					ms_Image2D.SetUV( 0.0f, 0.0f, 1.0f, 1.0f, GetRecipz() );

					stX = GetScreenPos().x + GetOffset().x + GetAniVal(0);
					stY = GetScreenPos().y + GetOffset().y - TB_IMOTICON_HEIGHT_OFFSET;

					ms_Image2D.SetPos( stX , stY , width, height );
					ms_Image2D.Render( rwPRIMTYPETRIFAN , ms_pMng->GetImgImoticon( GetImageID() ) ? RwTextureGetRaster( ms_pMng->GetImgImoticon( GetImageID() ) ) : NULL );
				}
				else				// slice 진동
				{
					FLOAT 	height_slice = 4.0f;

					if(	GetAniParam(1) < 2)
					{
						SetAniParam( 1 , 2 );
						SetAniVal( 2 , 0.0f );
					}

					SetAniVal(2 , GetAniVal(2) + 0.02f );
					if( GetAniParam(1) == 2)
					{
						if( GetAniVal(3) > 5.0f)
						{
							SetAniParam(1 , 3 );
							SetAniVal( 3 , 5.0f );
						}
						else
						{	
							SetAniVal( 3 , GetAniVal(3)+ ( 0.5f+GetAniVal(2) ) );
						}
					}
					else if( GetAniParam(1) == 3)
					{
						if( GetAniVal(3) < -5.0f)
						{
							SetAniParam( 1 , 2 );
							SetAniVal( 3 , -5.0f );
						}
						else
						{
							
							SetAniVal( 3 , GetAniVal(3) - (0.5f+GetAniVal(2)) );
						}
					}

					FLOAT v1	= 0;
					FLOAT v2	= 0;

					for( INT i = 0 ; i < (INT)height ; i+=(INT)(height_slice*2) )
					{
						v1 = (FLOAT)i/height;
						v2 = (FLOAT)(i+height_slice)/height;

						ms_Image2D.SetUV( 0.f, v1, 1.f, v2, GetRecipz() );

						stX = GetScreenPos().x + GetOffset().x + GetAniVal(3);
						stY = GetScreenPos().y + GetOffset().y - TB_IMOTICON_HEIGHT_OFFSET + i;

						ms_Image2D.SetPos( stX , stY , width, height_slice );
						ms_Image2D.Render( rwPRIMTYPETRIFAN , RwTextureGetRaster( ms_pMng->GetImgImoticon( GetImageID() )	) );
					}

					for( INT i = (INT)height_slice ; i <(INT)height ; i+=(INT)(height_slice*2) )
					{
						v1 = (FLOAT)i/height;
						v2 = (FLOAT)(i+height_slice)/height;

						ms_Image2D.SetUV( 0.f, v1, 1.f, v2, GetRecipz() );

						stX = GetScreenPos().x + GetOffset().x - GetAniVal(3);
						stY = GetScreenPos().y + GetOffset().y - TB_IMOTICON_HEIGHT_OFFSET + i;

						ms_Image2D.SetPos( stX , stY , width, height_slice );
						ms_Image2D.Render( rwPRIMTYPETRIFAN , ms_pMng->GetImgImoticon( GetImageID() ) ? RwTextureGetRaster( ms_pMng->GetImgImoticon( GetImageID() ) ) : NULL );
					}
				}
			}
			else
			{
				vColor.alpha = 255;
				ms_Image2D.SetColor( vColor );
				ms_Image2D.SetUV( 0.0f, 0.0f, 1.0f, 1.0f, GetRecipz() );

				stX = GetScreenPos().x + GetOffset().x;
				stY = GetScreenPos().y + GetOffset().y - TB_IMOTICON_HEIGHT_OFFSET;

				ms_Image2D.SetPos( stX , stY , width, height );
				ms_Image2D.Render( rwPRIMTYPETRIFAN , RwTextureGetRaster( ms_pMng->GetImgImoticon( GetImageID() ) ) );
			}
		}	
		break;
	}
	

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) TRUE);
}