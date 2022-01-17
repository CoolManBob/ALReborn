#include "AgcmIDBoard.h"
#include "agcmuioption.h"


AgcmIDBoard::AgcmIDBoard( eBoardType eType )
: AgcmBaseBoard( eType )
{
	SetPosition( NULL );
	SetApBase( NULL );
	SetIDType( TB_OTHERS );
	SetEnabled( TRUE );
	SetFontType( 0 );
	SetColor( 0 );
	SetEnableParty( 0 );
	SetParty( FALSE );
	SetEnableGuild( 0 );
	SetGuild( FALSE );

	SetText( "" );
	SetGuildText( "" );
	SetNickNameText( "" );
	SetTitleNameText( "" );
	SetCharismaText( "" );
	SetBattleText( "" );

	SetColorGuild( 0 );
	SetColorCharisma( 0 );
	SetFlag( TB_FLAG_NONE );

	m_nPrevFlag = 0;

	m_fOffsetXLeft = 0.0f;
	m_fOffsetXRight = 0.0f;
}

AgcmIDBoard::~AgcmIDBoard(void)
{
}

BOOL AgcmIDBoard::Update( IN DWORD tickDiff , IN RwMatrix* CamMat )
{

	if( GetBoardType() == AGCM_BOARD_ITEMID )
	{
		SetDraw( FALSE );

		if( GetEnabled() == FALSE )			return FALSE;
		
		if( GetClump() && ms_pMng->GetLastTick() - GetClump()->stUserData.calcDistanceTick < 2 )
		{
			SetDraw( TRUE );
			ms_pMng->Trans3DTo2D( &m_CameraPos , &m_ScreenPos , &m_Recipz , (const RwV3d*)m_pPosition ,
						 m_Height , m_pClump , m_Depth );
		}
	}

	return TRUE;
}

void AgcmIDBoard::Render()
{
	if( !ms_pMng->GetAgcmCharacter()->GetSelfCharacter() )
		return;

	// 그리기가 꺼져 있으면 그냥 종료
	if( GetVisible() == FALSE ) return;
	if( GetEnabled() == FALSE ) return;

	switch( m_Type )
	{
	case AGCM_BOARD_IDBOARD:     DrawChar();	SetVisible( FALSE );	break;
	case AGCM_BOARD_ITEMID:      DrawItem();							break;
	}

}

void AgcmIDBoard::_DrawName( RwReal fPosX, RwReal fPosY, INT nAlpha , BOOL bDraw )
{
	if( GetEnabled()	== FALSE )	return;

	INT				nResult			=	0;
	DWORD			dwColor			=	GetColor();
	BOOL			bSecurityName	=	FALSE;
	BOOL			bParty			=	GetParty() && GetEnableParty() == 1 ? TRUE : FALSE;
	AgpdCharacter*	pSelfCharacter	=	ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	AgpdCharacter*	pCharacter		=	(AgpdCharacter*)m_pBase;
	
	if( pSelfCharacter != pCharacter )
	{
		// 배틀 그라운드에서 종족별로 이름 색깔을 바꿔준다
		if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pSelfCharacter ) && ms_pMng->GetAgpmCharacter()->IsPC( pCharacter ) )
		{
			INT32	nType		= ms_pMng->GetAgpmFactors()->GetRace( &pCharacter->m_csFactor );
			INT32	nSelfType	= ms_pMng->GetAgpmFactors()->GetRace( &pSelfCharacter->m_csFactor );
			DWORD	dwTemp		= 0;

			switch( nType )
			{
			case AURACE_TYPE_HUMAN:			dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_HUMAN_NAME_COLOR);			break;
			case AURACE_TYPE_ORC:			dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_ORC_NAME_COLOR);			break;
			case AURACE_TYPE_MOONELF:		dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_MOONELF_NAME_COLOR);		break;
			case AURACE_TYPE_DRAGONSCION:	dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_DRAGONSCION_NAME_COLOR);   break;
			}	

			// 배틀그라운드는 무조건 이름이 ***** 로 나오게 한다
			// 같은 종족은 이름이 나온다
			if( nType != nSelfType )
				bDraw	=	FALSE;
		}
	}
	else
	{
		// 캐릭터의 악당레벨 및 선공여부에 따른 ID Color 후처리
		dwColor = _GetIDColorByPVP( pCharacter );
	}

	INT nTargetResult	=	ms_pMng->GetApmMap()->CheckRegionPerculiarity( pCharacter->m_nBindingRegionIndex	, APMMAP_PECULIARITY_GUILD_PVP );
	INT nSelfResult		=	ms_pMng->GetApmMap()->CheckRegionPerculiarity( pSelfCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_GUILD_PVP );

	if( nTargetResult	==	APMMAP_PECULIARITY_RETURN_DISABLE_USE &&
		nSelfResult		==	APMMAP_PECULIARITY_RETURN_DISABLE_USE	)
	{
		dwColor		=	0xFFFFFFFF;
	}

	if( GetParty() && GetEnableParty() == 1 )
	{
		ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX, fPosY, m_ScreenPos.z , m_Recipz , 
			bDraw ? GetText() :GetBattleText() , GetFontType() , 
			nAlpha, dwColor, true, true, 0xbf8b3f3f );
	}
	else
	{
		ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX, fPosY, m_ScreenPos.z , m_Recipz , 
			bDraw ? GetText() : GetBattleText()   , GetFontType() , 
						nAlpha, dwColor, true );
	}

}

void AgcmIDBoard::_DrawTitleName( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	if( GetTitle()	== FALSE )	return;

	AgpdCharacter*	pSelfCharacter	=	ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	AgpdCharacter*	pCharacter		=	(AgpdCharacter*)m_pBase;
	
	if( pSelfCharacter != pCharacter )
	{
		// 배틀 그라운드에서 종족별로 이름 색깔을 바꿔준다
		if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pSelfCharacter ) && ms_pMng->GetAgpmCharacter()->IsPC( pCharacter ) )
		{
			return;
		}
	}

	DWORD dwColor =	GetColorTitleName();
	char* pTitleName = GetTitleNameText();
	if( !pTitleName || strlen( pTitleName ) <= 0 ) return;

	//fPosX = GetOffsetTitle().x;
	float fStartX = m_ScreenPos.x + GetOffsetTitle().x;
	fPosY += GetOffset().y - 6.0f;

	//ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX, fPosY, m_ScreenPos.z, m_Recipz, pTitleName, GetFontType(), nAlpha, dwColor, true );
	ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fStartX, fPosY, m_ScreenPos.z, m_Recipz, pTitleName, GetFontType(), nAlpha, dwColor, true );
}

void AgcmIDBoard::_DrawNameEx( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	
	if( !ms_pMng->GetAgpmCharacter()->IsPC( (AgpdCharacter*)GetApBase() ) )		return;
	if( !strlen( ((AgpdCharacter*)GetApBase())->m_szID ) )						return;

	AgpdCharacter* pcsSelfCharacter = ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	if( !pcsSelfCharacter )		return;

	if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pcsSelfCharacter ) )    return;

	int  nAddPos = 0;
	char szAllName[128] = { 0, };

	if( GetNickNameText()[0] )
	{
		if( nAddPos )
		{
			strcpy( szAllName + nAddPos, "   " );
			nAddPos += strlen( "   " );
		}

		strcpy( szAllName + nAddPos , GetNickNameText() );
	}

	if( szAllName[0] )
	{
		fPosY += ( GetOffset().y - 34.f );
		float fStartX = (float)ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType() , szAllName, strlen( szAllName ) ) * 0.5f;
		ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( m_ScreenPos.x - fStartX , fPosY , m_ScreenPos.z , m_Recipz , szAllName, GetFontType() , nAlpha , GetColorNickName() , true );
	}
}

void AgcmIDBoard::_DrawGoMark( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	if( !ms_pMng->GetImgGoMark() )                                                      return;
	if( !ms_pMng->GetAgpmCharacter()->IsStatusGo( (AgpdCharacter*)GetApBase() ) )		return;

	RwV2d vImageSize	= ms_pMng->GetTextureSize( ms_pMng->GetImgGoMark() );
	RwV2d vOffset		= { -46 * .5f, GetOffset().y - ( 10.f * 0.5f ) - 11.f };	//Real Image (49, 19 )
	//RwV2d vOffset		= { 0.f, 0.f };
	g_pEngine->DrawIm2DInWorld( ms_pMng->GetImgGoMark() , &m_CameraPos, &m_ScreenPos, m_Recipz , &vOffset, vImageSize.x, vImageSize.y );
}

VOID AgcmIDBoard::_DrawFlag( VOID )
{
	if( !ms_pMng ) return;

	AgpmGuild* ppmGuild = ( AgpmGuild* )ms_pMng->GetModule( "AgpmGuild" );
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )GetApBase();
	if( !ppdCharacter || !ppmGuild ) return;

	EventStatusFlag	unBitFlag;
	memset(&unBitFlag, 0, sizeof(unBitFlag));

	unBitFlag.BitFlag = ppdCharacter->m_unEventStatusFlag;
	m_nPrevFlag = unBitFlag.NationFlag;

	void* pTexture = ms_pMng->GetFlagTexture( unBitFlag.NationFlag );
	if( !pTexture ) return;

	// 길드가 있나..
	float fGuildMarkOffset = 0.0f;
	BOOL bHaveGuild = ppmGuild->GetGuild( ppdCharacter ) ? TRUE : FALSE;
	if( bHaveGuild && ppmGuild )
	{
		if( ppmGuild->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_BOTTOM, ppdCharacter ) )
		{
			for( int nIndex = AGPMGUILD_ID_MARK_BOTTOM; nIndex <= AGPMGUILD_ID_MARK_SYMBOL; nIndex++ )
			{
				fGuildMarkOffset = 0.0f;
				RwTexture* pTexture = ( RwTexture* )ppmGuild->GetGuildMarkTexture( nIndex, ppdCharacter, true );
				if( pTexture )
				{
					fGuildMarkOffset = ms_pMng->GetTextureSize( pTexture ).y;
					if( fGuildMarkOffset > 0.0f )
					{
						bHaveGuild = TRUE;
					}
				}
			}
		}
		else
		{
			// 길드마크가 없더라도 아크로드라면 아크로드마크만큼 오프셋을 지정
			if( ms_pMng->GetAgpmCharacter()->IsArchlord( ppdCharacter ) )
			{
				RwTexture* pTexture = ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark;
				if( pTexture )
				{
					fGuildMarkOffset = ms_pMng->GetTextureSize( pTexture ).y;
				}
			}
		}
	}

	BOOL bHaveCharisma = FALSE;
	if( ( INT )ms_pMng->GetTextureVec().size() > GetCharismaStep() && GetCharismaStep() >= 0 )
	{
		RwTexture* pTextureCharisma = ms_pMng->GetTextureVec()[ GetCharismaStep() ];
		if( pTextureCharisma )
		{
			bHaveCharisma = TRUE;
		}
	}

	RwV2d vImageSize = ms_pMng->GetTextureSize( ( RwTexture* )pTexture );

	float fOffsetY = ms_pMng->GetFlagOffsetY( bHaveGuild, bHaveCharisma );
	RwV2d vOffset = { GetOffsetXLeft() + (-vImageSize.x) - 4.0f, GetOffset().y - ( fGuildMarkOffset + 1.0f ) + fOffsetY };

	g_pEngine->DrawIm2DInWorld( ( RwTexture* )pTexture , &m_CameraPos, &m_ScreenPos, m_Recipz , &vOffset, vImageSize.x, vImageSize.y );
}

VOID AgcmIDBoard::_DrawGuildCompetition( VOID )
{
	if( !ms_pMng ) return;

	// 배틀그라운드에서는 안찍습네다.
	AgpmBattleGround* ppmBattleGround = ( AgpmBattleGround* )g_pEngine->GetModule( "AgpmBattleGround" );
	if( !ppmBattleGround ) return;
	if( ppmBattleGround->IsInBattleGround( ( AgpdCharacter* )GetApBase() ) ) return;

	AgpdGuildADChar* ppdAttachedGuild = ms_pMng->GetAgpmGuild()->GetADCharacter( ( AgpdCharacter* )GetApBase() );

	// 길드대항전 관련 이미지 찍기
	if( ppdAttachedGuild )
	{
		int nImgIndex = -1;
		switch( ppdAttachedGuild->m_lBRRanking )		//  TextBoard.ini 파일에 정의된 인덱스로 매칭
		{
		case WINNER_GUILD_1STPLACE :	nImgIndex = 1;	break;
		case WINNER_GUILD_2NDPLACE :	nImgIndex = 4;	break;
		}

		if( nImgIndex >= 0 )
		{
			RwTexture* pTexture = ms_pMng->GetImgEtc( nImgIndex );
			if( pTexture )
			{
				RwV2d vImageSize	= ms_pMng->GetTextureSize( pTexture );
				RwV2d vOffset		= { -vImageSize.x * .5f, GetOffset().y - vImageSize.y - 8.0f - 20.f };
				g_pEngine->DrawIm2DInWorld( pTexture , &GetCameraPos(), &GetScreenPos() , GetRecipz() , &vOffset, vImageSize.x, vImageSize.y );
			}
		}
	}
}

void AgcmIDBoard::_DrawGuild( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	INT		nResult		=	0;

	if( GetEnableGuild() == FALSE )		return;

	if( ms_pMng->GetAgcmUIcontrol() == NULL )			return;
	if( GetGuild()	== FALSE || GetEnableGuild()	!= 1 )	return;

	AgpdCharacter*	pcsSelfCharacter	= ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	if( pcsSelfCharacter == NULL )	return;

	if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pcsSelfCharacter ) )	return;

	ms_pMng->CheckPeculiarity( pcsSelfCharacter , nResult );

	AgpdSiegeWar*	pcsSiegeWar		= ms_pMng->GetAgpmSiegeWar()->GetArchlordCastle();
	if( pcsSiegeWar == NULL )		return;

	AgpdGuildADChar* pcsAttachedSelfGuild	= ms_pMng->GetAgpmGuild()->GetADCharacter( pcsSelfCharacter );
	AgpdGuildADChar* pcsAttachedGuild		= ms_pMng->GetAgpmGuild()->GetADCharacter( (AgpdCharacter*)GetApBase() );

	fPosX += GetOffsetGuild().x;
	fPosY += GetOffsetGuild().y;
	
	ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX , fPosY , GetScreenPos().z , GetRecipz() , GetGuildText() , GetFontType() , nAlpha , GetColorGuild() , true );

	AgpdGuild* pcsSelfGuild	= ms_pMng->GetAgpmGuild()->GetGuild( pcsAttachedSelfGuild->m_szGuildID );

	if( pcsSelfGuild && ms_pMng->GetAgpmGuild()->IsJointGuild( pcsSelfGuild, pcsAttachedGuild->m_szGuildID ) )
	{
		JointVector& vecJoint = *pcsSelfGuild->m_csRelation.m_pJointVector;
		int nJointMax = vecJoint.size();
		int nJointIndex = 1;
		for( JointIter Itr = vecJoint.begin(); Itr != vecJoint.end(); ++Itr, ++nJointIndex )
		{
			if( pcsSiegeWar->m_strOwnerGuildName.Compare( Itr->m_szGuildID ) == COMPARE_EQUAL && ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark )
				ms_pMng->DrawGuildImage( this , fPosX,  ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark, 0xffffffff, .5f, nJointMax, nJointIndex );
			else
			{
				int nJointGuildMarkTID		= ms_pMng->GetAgpmGuild()->GetJointGuildMarkID( Itr->m_szGuildID );
				int nJointGuildMarkColor	= ms_pMng->GetAgpmGuild()->GetJointGuildMarkColor( Itr->m_szGuildID );
				if( nJointGuildMarkTID )
				{
					for( int nIndex = AGPMGUILD_ID_MARK_BOTTOM; nIndex <= AGPMGUILD_ID_MARK_SYMBOL; ++nIndex )
					{
						int nColor = ms_pMng->GetAgpmGuild()->GetGuildMarkColor( nIndex , nJointGuildMarkColor );
						RwTexture* pTexture = (RwTexture*)( ms_pMng->GetAgpmGuild()->GetGuildMarkTexture( nIndex, nJointGuildMarkTID, true) );
						ms_pMng->DrawGuildImage( this , fPosX, pTexture, nColor, .5f, nJointMax, nJointIndex );
					}

					ms_pMng->DrawGuildImage( this, fPosX, ms_pMng->GetImgEtc(0) , 0xffffffff, .5f, nJointMax, nJointIndex );
				}
			}
		}
	}
	else
	{
		if( pcsSiegeWar->m_strOwnerGuildName.Compare(pcsAttachedGuild->m_szGuildID) == COMPARE_EQUAL && ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark )
			ms_pMng->DrawGuildImage( this ,  fPosX, ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark );
		else
		{
			if( ms_pMng->GetAgpmGuild()->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_BOTTOM, (AgpdCharacter*)GetApBase() ) )
			{
				for( int nIndex = AGPMGUILD_ID_MARK_BOTTOM; nIndex <= AGPMGUILD_ID_MARK_SYMBOL; nIndex++ )
				{
					int nColor = ms_pMng->GetAgpmGuild()->GetGuildMarkColor( nIndex, (AgpdCharacter*)GetApBase() );
					RwTexture* pTexture = (RwTexture*)ms_pMng->GetAgpmGuild()->GetGuildMarkTexture( nIndex, (AgpdCharacter*)GetApBase(), true );
					ms_pMng->DrawGuildImage( this, fPosX, pTexture, nColor );
				}

				ms_pMng->DrawGuildImage( this, fPosX, ms_pMng->GetImgEtc(0) );
			}
		}
	}

	//. 공성/수성에 참가한 상태인가?
	int iJoinSiegeWar = ms_pMng->GetAgcmSiegeWar()->GetSiegeWarGuildType( (AgpdCharacter*)GetApBase() );
	if( iJoinSiegeWar > -1 )
	{
		//길드마크를 그렸었으면..
		RwTexture* pTexture = (RwTexture*)ms_pMng->GetAgpmGuild()->GetGuildMarkTexture( AGPMGUILD_ID_MARK_BOTTOM, (AgpdCharacter*)GetApBase(), true );
		ms_pMng->DrawGuildImage( this, fPosX, ms_pMng->GetImgEtc( iJoinSiegeWar+1 ) , 0xffffffff, 1.f, 0, 0, pTexture ? TRUE : FALSE );
	}
}


void AgcmIDBoard::_DrawMonsterAbility( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	if( !ms_pMng->GetAgpmBattleGround()->IsInBattleGround( ms_pMng->GetAgcmCharacter()->GetSelfCharacter() ) )	return;	//BattleGround가 아니면 Would you please 꺼져줄래?

	AgpdCharacter*	pdCharacter = static_cast< AgpdCharacter* >(GetApBase());
	if( !pdCharacter )											   return;
	if( !ms_pMng->GetAgpmCharacter()->IsMonster( pdCharacter ) )   return;

	RwTexture* pTexture = ms_pMng->GetAgcmSkill()->GetBuffSkillSmallTexture( pdCharacter, 0, TRUE );
	if( !pTexture )			return;

	RwV2d vImageSize	= ms_pMng->GetTextureSize( pTexture );
	RwV2d vOffset		= { GetOffset().x +(-vImageSize.x) - 2.f - 4.f, GetOffset().y - 8.f };
	g_pEngine->DrawIm2DInWorld( pTexture, &m_CameraPos, &m_ScreenPos, m_Recipz, &vOffset, vImageSize.x, vImageSize.y );
}

void AgcmIDBoard::_DrawCharisma( RwReal fPosX, RwReal fPosY, INT nAlpha )
{

	AgpdCharacter*	pdCharacter = static_cast< AgpdCharacter* >(GetApBase());
	if( !pdCharacter )										 return;
	if( !ms_pMng->GetAgpmCharacter()->IsPC( pdCharacter ) )	 return;

	if( (INT)ms_pMng->GetTextureVec().size() <= GetCharismaStep() || GetCharismaStep() < 0 )   return;

	RwTexture* pTexture = ms_pMng->GetTextureVec()[ GetCharismaStep() ];
	if( !pTexture )		return;

	// 길드가 있나..
	float fGuildMarkOffset = 0.0f;
	AgpmGuild* ppmGuild = ( AgpmGuild* )ms_pMng->GetModule( "AgpmGuild" );
	BOOL bHaveGuild = ppmGuild->GetGuild( pdCharacter ) ? TRUE : FALSE;
	if( bHaveGuild && ppmGuild )
	{
		AgpdGuildMarkTemplate* pMarkTemplateBottom = ppmGuild->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_BOTTOM, pdCharacter );
		AgpdGuildMarkTemplate* pMarkTemplatePattern = ppmGuild->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_PATTERN, pdCharacter );
		AgpdGuildMarkTemplate* pMarkTemplateSimbol = ppmGuild->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_SYMBOL, pdCharacter );

		if( pMarkTemplateBottom || pMarkTemplatePattern || pMarkTemplateSimbol )
		{
			for( int nIndex = AGPMGUILD_ID_MARK_BOTTOM; nIndex <= AGPMGUILD_ID_MARK_SYMBOL; nIndex++ )
			{	
				RwTexture* pTexture = ( RwTexture* )ppmGuild->GetGuildMarkTexture( nIndex, pdCharacter, true );
				if( pTexture )
				{
					float fTextureHeight = ms_pMng->GetTextureSize( pTexture ).y;
					fGuildMarkOffset = fGuildMarkOffset < fTextureHeight ? fTextureHeight : fGuildMarkOffset;
				}
			}
		}
		else
		{
			// 길드마크가 없더라도 아크로드라면 아크로드마크만큼 오프셋을 지정
			if( ms_pMng->GetAgpmCharacter()->IsArchlord( pdCharacter ) )
			{
				RwTexture* pTexture = ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark;
				if( pTexture )
				{
					fGuildMarkOffset = ms_pMng->GetTextureSize( pTexture ).y;
				}
			}
		}
	}

	RwV2d vImageSize	= ms_pMng->GetTextureSize( pTexture );
	//RwV2d vPos		= { pInfo->screen_pos.x - (vImageSize.x * .5f), fPosY + ( pInfo->screen_pos.y - 20.f ) };
	RwV2d vOffset		= { GetOffsetXLeft() + (-vImageSize.x) - 4.0f, GetOffset().y - ( fGuildMarkOffset + 1.0f ) };

	g_pEngine->DrawIm2DInWorld( pTexture, &m_CameraPos, &m_ScreenPos, m_Recipz , &vOffset, vImageSize.x, vImageSize.y );
}

void AgcmIDBoard::DrawItem()
{

	FLOAT stX    = 0;
	FLOAT stY    = 0;

	BOOL bItemNameDraw = TRUE;
	AgcmUIOption* pOption = (AgcmUIOption *)ms_pMng->GetModule("AgcmUIOption");
	if( pOption  )
		bItemNameDraw = pOption->m_bVisibleViewItem;

	if( bItemNameDraw )
	{
		if( GetDraw() )
		{
			stX = GetScreenPos().x + GetOffset().x;
			stY = GetScreenPos().y + GetOffset().y;

			ms_pMng->GetAgcmFont()->FontDrawStart( GetFontType() );
			ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( stX , stY , GetScreenPos().z, m_Recipz , GetText() , GetFontType() , 255 , GetColor() ,true );
			ms_pMng->GetAgcmFont()->FontDrawEnd();
		}
	}

}

void AgcmIDBoard::DrawChar()
{
	// 캐릭터 없으면 패스
	AgpdCharacter* pdCharacter = ( AgpdCharacter* )m_pBase;
	if( !pdCharacter ) return;

	// 찍을 ID 가 없으면 패스
	if( !ms_pMng->GetDrawID() ) return;

	// 그려야 할 캐릭터가 아니면 패스
	if( !ms_pMng->IsRender( pdCharacter ) )	return;

	if( _IsInGuildStadium( pdCharacter ) )
	{
		// 길드경기장에서는 ID, 길드명, 길드마크만 보여야 한단다..
		_Draw_InGuildStadium( pdCharacter );
	}
	else
	{
		// 기본 그리기
		_Draw_Normal( pdCharacter );
	}
}

VOID AgcmIDBoard::ReCalulateOffsetX( VOID )
{
	// IDBoard 의 가운데 위치하는 별칭, 길드명, 타이틀명, 캐릭터명중에서 가장 긴 문자열을 기준으로 왼쪽 OffsetX 값을 구한다.
	float fLengthName = 0.0f;
	int nNameLength = ( int )strlen( m_Text );
	if( nNameLength > 0 && nNameLength <= TB_ID_MAX_CHAR )
	{
		fLengthName = ( float )ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType(), m_Text, nNameLength );
	}

	float fLengthNickName = 0.0f;
	int nNickNameLength = ( int )strlen( m_strNickName );
	if( nNickNameLength > 0 && nNickNameLength <= TB_ID_MAX_CHAR )
	{
		fLengthNickName = ( float )ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType(), m_strNickName, nNickNameLength );
	}

	float fLengthGuildName = 0.0f;
	int nGuildNameLength = ( int )strlen( m_strGuild );
	if( nGuildNameLength > 0 && nGuildNameLength <= TB_ID_MAX_CHAR )
	{
		fLengthGuildName = ( float )ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType(), m_strGuild, nGuildNameLength );
	}

	float fLengthTitleName = 0.0f;
	int nTitleNameLength = ( int )strlen( m_strTitleName );
	if( nTitleNameLength > 0 && nTitleNameLength <= TB_ID_MAX_CHAR )
	{
		fLengthTitleName = ( float )ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType(), m_strTitleName, nTitleNameLength );
	}

	// 계산된 값을 타이틀 오프셋으로 저장
	GetOffsetTitle().x = fLengthTitleName * -0.5f;

	float fLongestLength = fLengthNickName > fLengthGuildName ? fLengthNickName : fLengthGuildName;
	fLongestLength = fLongestLength > fLengthTitleName ? fLongestLength : fLengthTitleName;
	fLongestLength = fLongestLength > fLengthName ? fLongestLength : fLengthName;

	// 가장 긴 문자열의 길이를 기준으로 왼쪽에 찍힐 아이콘들의 OffsetX 값 설정
	m_fOffsetXLeft = fLongestLength * -0.5f;

	// 가장 긴 문자열의 길이를 기준으로 오른쪽에 찍힐 아이콘들의 OffsetX 값 설정
	m_fOffsetXRight = fLongestLength * 0.5f;
}

void AgcmIDBoard::_Draw_Normal( AgpdCharacter* ppdCharacter )
{
	if( !ppdCharacter ) return;
	if( !ms_pMng ) return;

	AgpdCharacter* ppdSelfCharacter = ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	if( !ppdSelfCharacter ) return;

	RwV2d   Offset = { 0.0f , 0.0f };
	FLOAT   stX , stY;
	
	RwTexture* pTexStatus = ms_pMng->GetStateTexture( ppdCharacter );
	if( pTexStatus )
	{
		Offset.x = -RwRasterGetWidth( RwTextureGetRaster( pTexStatus ) ) * 0.5f;
		Offset.y = GetOffset().y - RwRasterGetHeight( RwTextureGetRaster( pTexStatus ) ) - 8.0f;
		ms_pMng->RenderTexture( pTexStatus, m_CameraPos , m_ScreenPos , m_Recipz , &Offset );
	}

	RwTexture* pTexCastleOwner = ms_pMng->GetCastleTexture( ppdCharacter->m_ulSpecialStatus );
	if( pTexCastleOwner )
	{
		Offset.x = ( float )-RwRasterGetWidth( RwTextureGetRaster( pTexCastleOwner ) ) * 0.5f;
		float fOffsetY = ppdCharacter->m_szNickName[0] ? 46.0f : 34.0f;
		Offset.y = GetOffset().y - RwRasterGetHeight( RwTextureGetRaster( pTexCastleOwner ) ) - fOffsetY;
		ms_pMng->RenderTexture( pTexCastleOwner, m_CameraPos , m_ScreenPos , m_Recipz , &Offset );
	}

	INT		nResult		=	0;
	BOOL	bDraw		=	TRUE;
	INT32	nType		= ms_pMng->GetAgpmFactors()->GetRace( &ppdCharacter->m_csFactor );
	INT32	nSelf		= ms_pMng->GetAgpmFactors()->GetRace( &ppdSelfCharacter->m_csFactor );
	
	ms_pMng->CheckPeculiarity( ppdSelfCharacter , nResult );
	switch( nResult )
	{
	case APMMAP_PECULIARITY_RETURN_DISABLE_USE:	
		bDraw	=	FALSE;
		break;

	case APMMAP_PECULIARITY_RETURN_ENABLE_USE_UNION:
		if( (AgpdCharacter*)m_pBase != ppdSelfCharacter )
		{
			if( nType != nSelf )   
				bDraw   = FALSE;
		}
		break;
	}

	stX = ( FLOAT ) ( (INT) ( GetScreenPos().x + GetOffset().x ) );
	stY = ( FLOAT ) ( (INT) ( GetScreenPos().y + GetOffset().y ) );

	ms_pMng->GetAgcmFont()->FontDrawStart( GetFontType() );

	_DrawName          			( stX, stY, GetAlpha() , bDraw );
	_DrawTitleName     			( stX, stY, GetAlpha() );
	_DrawGoMark        			( stX, stY, GetAlpha() );

	if( bDraw )
	{
		_DrawGuild				( stX, stY, GetAlpha() );
		_DrawGuildCompetition	();
		_DrawNameEx        		( stX, stY, GetAlpha() );
		_DrawCharisma      		( stX, stY, GetAlpha() );
		_DrawMonsterAbility		( stX, stY, GetAlpha() );
		_DrawFlag();
	}

	ms_pMng->GetAgcmFont()->FontDrawEnd();

	Offset.x  =  GetOffsetXRight() + 2;
	Offset.y  =  GetOffset().y - 1;

	for( INT i = 0 ; i < TB_MAX_TAG ; ++i )
	{
		if( ms_pMng->GetImgIDTag(i) && (GetFlag() & (1<<i)) )
		{
			ms_pMng->RenderTexture( ms_pMng->GetImgIDTag(i) , m_CameraPos , m_ScreenPos , m_Recipz , &Offset );
			Offset.x  += TBID_TAG_SIZE;
		}
	}
}

void AgcmIDBoard::_Draw_InGuildStadium( AgpdCharacter* ppdCharacter )
{
	if( !ms_pMng ) return;

	FLOAT   stX , stY;
	
	stX = ( FLOAT ) ( (INT) ( GetScreenPos().x + GetOffset().x ) );
	stY = ( FLOAT ) ( (INT) ( GetScreenPos().y + GetOffset().y ) );

	ms_pMng->GetAgcmFont()->FontDrawStart( GetFontType() );

	_DrawName          		( stX, stY, GetAlpha() , TRUE );
	_DrawGoMark        		( stX, stY, GetAlpha() );
	_DrawGuild         		( stX, stY, GetAlpha() );

	ms_pMng->GetAgcmFont()->FontDrawEnd();
}

BOOL AgcmIDBoard::_IsInGuildStadium( AgpdCharacter* ppdCharacter )
{
	if( !ppdCharacter ) return FALSE;

	// 맵 모듈 없어도 패스
	ApmMap* ppmMap = ( ApmMap* )g_pEngine->GetModule( "ApmMap" );
	if( !ppmMap ) return FALSE;

	// 리전을 알수 없어도 패스
	ApmMap::RegionTemplate* pRegionTemplate = ppmMap->GetTemplate( ppdCharacter->m_nBindingRegionIndex );
	if( !pRegionTemplate ) return FALSE;

	// 길드경기장 리전 TID 리스트.. ㅡ.ㅡ.. 따로 타입이 있겠지 했드니만 없네.. 걍 TID 비교함
	// 나중에 어디 파일로 빼야 함.. ㅡ.ㅡ;; 지금은 당장 패치를 해야 하니..
	static int g_nStadiumTID[] = { 89, 90, 91, 92, 162 };
	static int g_nStadiumCount = 5;

	for( int nCount = 0 ; nCount < g_nStadiumCount ; nCount++ )
	{
		if( pRegionTemplate->nIndex == g_nStadiumTID[ nCount ] )
		{
			return TRUE;
		}
	}

	return FALSE;
}

DWORD AgcmIDBoard::_GetIDColorByPVP( AgpdCharacter* ppdCharacter )
{
	DWORD dwColor = GetColor();
	if( !ppdCharacter ) return dwColor;

	// 배틀그라운드 이외의 경우 현재의 악당레벨 및 선공자상태에 따라 ID 색상을 변경한다.
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )g_pEngine->GetModule( "AgcmResourceLoader" );
	if( !ppmCharacter || !pcmResourceLoader ) return dwColor;


	// 악당레벨이 있을 경우에 대한 색상 지정.. 선공자색상보다 우선순위가 높으므로 나중에 체크한다.
	int nCurrMurdererLevel = ppmCharacter->GetMurdererLevel( ppdCharacter );

	switch( nCurrMurdererLevel )
	{
	case AGPMCHAR_MURDERER_LEVEL1_POINT :	dwColor = pcmResourceLoader->GetColor( "IDColor_MurdererLevel1" );	break;
	case AGPMCHAR_MURDERER_LEVEL2_POINT :	dwColor = pcmResourceLoader->GetColor( "IDColor_MurdererLevel2" );	break;
	case AGPMCHAR_MURDERER_LEVEL3_POINT :	dwColor = pcmResourceLoader->GetColor( "IDColor_MurdererLevel3" );	break;
	}

	// 선공자인 경우의 색상 지정
	BOOL bIsFirstAttacker = ppdCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED ? TRUE : FALSE;
	if( bIsFirstAttacker )
	{
		dwColor = pcmResourceLoader->GetColor( "IDColor_FirstAttacker" );
	}

	return dwColor;
}