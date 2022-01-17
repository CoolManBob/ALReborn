// AgcUIEffect.cpp: implementation of the AgcUIEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcUIEffect.h"
#include "AgcmUIManager2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcUIEffect::AgcUIEffect(AgcmUIManager2 *pcsAgcmUIManager2)
{
	m_pcsAgcmUIManager2 =	pcsAgcmUIManager2	;
	m_Property.bTopmost =	TRUE				;
	m_Property.bUseInput=	FALSE				;

	m_bFade				=	FALSE				;
	m_pstTextureFade	=	NULL				;

	m_szSubText[ACUI_EFFECT_SUBTEXT_LENGTH - 1]	=	0	;
	m_bExistSubText		=	FALSE				;

	m_szAboveText[ACUI_EFFECT_SUBTEXT_LENGTH - 1]	=	0	;
	m_bExistAboveText	=	FALSE				;

	m_lSubTextFont		=	0					;
	m_dwSubTextColor	=	0					;
	m_fSubTextScale		=	1.0f				;

	m_lAboveTextFont	=	0					;
	m_dwAboveTextColor	=	0					;
	m_fAboveTextScale	=	1.0f				;
}

AgcUIEffect::~AgcUIEffect()
{

}

BOOL			AgcUIEffect::OnInit			()
{
	if (m_pstTextureFade)
	{
		RwTextureDestroy(m_pstTextureFade);
		m_pstTextureFade = NULL;
	}

	RwRaster *				pstRaster;

	pstRaster			=	RwRasterCreate(1, 1, 32, rwRASTERTYPETEXTURE);
	if (!pstRaster)
		return FALSE;

	RwRasterLock(pstRaster, 0, rwRASTERLOCKWRITE);

	((DWORD *) pstRaster->cpPixels)[0] = 0xffffffff;

	m_pstTextureFade	=	RwTextureCreate(pstRaster);

	return TRUE;
}

VOID			AgcUIEffect::OnClose		()
{
	if (m_pstTextureFade)
	{
		RwTextureDestroy(m_pstTextureFade);
		m_pstTextureFade = NULL;
	}
}

BOOL			AgcUIEffect::OnIdle			( UINT32 ulClockCount	)
{
	PROFILE("AgcUIEffect::OnIdle");
	static	UINT32	_uPrevClockCount = ulClockCount;
	UINT32	uDiff	= ulClockCount - _uPrevClockCount;
	_uPrevClockCount	= ulClockCount;

	FLOAT	fScale	;

	if (m_bDrawStaticString)
	{
		INT32	lAlpha = 255;
		if( 0x7FFFFFFF != m_ulTextEndTime )
		{
			if( m_ulTextStartTime == 0 )
			{
				// 이펙트 텍스트는 최초에 디스플레이돼는 타임 기준으로 듀레이션을 정한다.
				m_ulTextStartTime	= ulClockCount + AGCUIEFFECT_FADE_DELAY		;
				m_ulTextEndTime		= m_ulTextEndTime + m_ulTextStartTime		;
			}
			if( uDiff > AGCUIEFFECT_FADE_DELAY )
			{
				// FadeCount 보다 큰 갭이 생길경우 유져가 보지 못할 가능성이 높으므로
				// 해당 Diff 만큼 이펙트를 뒤로 밀어준다.
				m_ulTextStartTime	+= uDiff;
				m_ulTextEndTime		+= uDiff;
			}

			if (m_ulTextStartTime > ulClockCount)
				lAlpha =	255 - 255 * (m_ulTextStartTime - ulClockCount) / AGCUIEFFECT_FADE_DELAY	;
			else if (m_ulTextEndTime < ulClockCount)
			{
				if (m_ulTextEndTime + AGCUIEFFECT_FADE_DELAY < ulClockCount)
				{
					m_bDrawStaticString	=	FALSE;
					lAlpha	= 0;
				}
				else
					lAlpha	= 255 * (m_ulTextEndTime + AGCUIEFFECT_FADE_DELAY - ulClockCount) / AGCUIEFFECT_FADE_DELAY;
			}
		}

		lAlpha = max( 0, min( lAlpha, 255 ) );
		m_lColor			=	((lAlpha << 24) & 0xff000000) | (m_lColor & 0x00ffffff);
		m_dwSubTextColor	=	((lAlpha << 24) & 0xff000000) | (m_dwSubTextColor & 0x00ffffff);
	}

	if (m_csTextureList.GetCount() > 0)
	{
		RwTexture *	pstTexture		=	m_csTextureList.GetImage_ID(m_lImageID);

		if (pstTexture)
		{
			m_stRenderInfo.cRenderAlpha	=	255	;

			if (m_ulImageFadeInEndTime > ulClockCount)
			{
				m_stRenderInfo.cRenderAlpha		=	255 - 255 * (m_ulImageFadeInEndTime - ulClockCount) / m_lImageDurationFadeIn;
			}

			if (m_ulImageScalingEndTime > ulClockCount)
			{
				fScale			=	1.0f + (m_fImageScale - 1.0f) * (m_ulImageScalingEndTime - ulClockCount) / m_lImageDurationScaling;

				SetRenderScale(fScale);

				m_lImageOffsetX	=	( INT32 ) (w - (RwRasterGetWidth(RwTextureGetRaster(pstTexture)) - m_lImageMarginRight) * fScale) / 2;
				m_lImageOffsetY	=	( INT32 ) (h - (RwRasterGetHeight(RwTextureGetRaster(pstTexture)) - m_lImageMarginBottom) * fScale) / 2;
			}
			else
			{
				SetRenderScale(1.0001f);

				m_lImageOffsetX	=	( INT32 ) (w - (RwRasterGetWidth(RwTextureGetRaster(pstTexture)) - m_lImageMarginRight)) / 2;
				m_lImageOffsetY	=	( INT32 ) (h - (RwRasterGetHeight(RwTextureGetRaster(pstTexture)) - m_lImageMarginBottom)) / 2;
			}

			if (m_ulImageEndTime + AGCUIEFFECT_FADE_DELAY < ulClockCount)
			{
				SetRenderScale(1.0001f);
				m_csTextureList.DestroyTextureList();
				m_stRenderInfo.cRenderAlpha		=	0		;
			}
			else if (m_ulImageEndTime < ulClockCount)
			{
				SetRenderScale(1.0001f);
				m_stRenderInfo.cRenderAlpha		=	255 * (m_ulImageEndTime + AGCUIEFFECT_FADE_DELAY - ulClockCount) / AGCUIEFFECT_FADE_DELAY	;
			}
		}
	}


	if (m_bFade && m_pstTextureFade)
	{
		if (m_lDurationFadeOut != -1 && m_ulFadeOutStartTime + m_lDurationFadeOut > ulClockCount)
		{
			m_lFadeAlpha	=	255 - 255 * (m_ulFadeOutStartTime + m_lDurationFadeOut - ulClockCount) / m_lDurationFadeOut;
		}
		else if (m_lDurationFadeIn != -1 && m_ulFadeInStartTime + m_lDurationFadeIn > ulClockCount)
		{
			m_lFadeAlpha	=	255 * (m_ulFadeInStartTime + m_lDurationFadeIn - ulClockCount) / m_lDurationFadeIn;
		}
		else if (m_lDurationFadeIn == -1)
		{
			m_lFadeAlpha	=	255		;
		}
		else
		{
			m_lFadeAlpha	=	0		;
			m_bFade			=	FALSE	;
		}
	}

	SetFocus();

	return TRUE;
}

VOID			AgcUIEffect::OnWindowRender	()
{
	INT32	nAbsolute_x	=	0;
	INT32	nAbsolute_y	=	0;

	AcUIBase::OnWindowRender();

	ClientToScreen(&nAbsolute_x, &nAbsolute_y);

	if (m_bDrawStaticString && m_bExistAboveText)
	{
		if (m_bAboveTextImage)
		{
			m_pAgcmFont->DrawTextImgFont( (FLOAT)( nAbsolute_x + m_lAboveTextOffsetX ),
				(FLOAT)( nAbsolute_y + m_lAboveTextOffsetY ),
				m_szAboveText, m_lAboveTextFont,
				( UINT8 ) ( (m_dwAboveTextColor >> 16) & 0xff ), ( UINT8 ) ( (m_dwAboveTextColor >> 8) & 0xff ) , ( UINT8 ) ( (m_dwAboveTextColor >> 0) & 0xff ), ( UINT8 ) ( ((m_dwAboveTextColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1) ) );
		}
		else
		{
			m_pAgcmFont->FontDrawStart(m_lAboveTextFont);

			m_pAgcmFont->DrawTextIM2DScale( (FLOAT)( nAbsolute_x + m_lAboveTextOffsetX ), 
				(FLOAT)( nAbsolute_y + m_lAboveTextOffsetY ), 
				m_fAboveTextScale, 
				m_szAboveText, 
				m_lAboveTextFont, 
				//( UINT8 ) ( ((m_dwAboveTextColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1) ),
				// 2005/01/20 마고자
				// 메인 텍스트의 알파값을 그대로 가져온다.
				// 로딩화면에서 안전지역~ 이라고 뜨는게 메인텍스트와 서브텍스트의 알파값이 달라서 그럼...
				( UINT8 ) ( ((m_lColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1) ),
				m_dwAboveTextColor,
				m_bShadow ? true : false );


			m_pAgcmFont->FontDrawEnd();
		}
	}

	if (m_bDrawStaticString && m_bExistSubText)
	{
		if (m_bSubTextImage)
		{
			m_pAgcmFont->DrawTextImgFont( (FLOAT)( nAbsolute_x + m_lSubTextOffsetX ),
				(FLOAT)( nAbsolute_y + m_lSubTextOffsetY ),
				m_szSubText, m_lSubTextFont,
				( UINT8 ) ( (m_dwSubTextColor >> 16) & 0xff ), ( UINT8 ) ( (m_dwSubTextColor >> 8) & 0xff ) , ( UINT8 ) ( (m_dwSubTextColor >> 0) & 0xff ) , ( UINT8 ) ( ((m_dwSubTextColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1) ) );
		}
		else
		{
			m_pAgcmFont->FontDrawStart(m_lSubTextFont);

			m_pAgcmFont->DrawTextIM2DScale( (FLOAT)( nAbsolute_x + m_lSubTextOffsetX ), 
				(FLOAT)( nAbsolute_y + m_lSubTextOffsetY ), 
				m_fSubTextScale, 
				m_szSubText, 
				m_lSubTextFont, 
				//((m_dwSubTextColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1),
				// 2005/01/20 마고자
				// 메인 텍스트의 알파값을 그대로 가져온다.
				// 로딩화면에서 안전지역~ 이라고 뜨는게 메인텍스트와 서브텍스트의 알파값이 달라서 그럼...
				( UINT8 ) ( ((m_lColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1) ),
				m_dwSubTextColor,
				m_bShadow ? true : false);


			m_pAgcmFont->FontDrawEnd();
		}
	}

	if (m_bFade && m_pstTextureFade)
	{
		g_pEngine->DrawIm2D(m_pstTextureFade, 0, 0, w, h, 0.0f, 0.0f, 1.0f, 1.0f, m_dwFadeColor, m_lFadeAlpha);
	}
}

BOOL			AgcUIEffect::SetEffectText	( CHAR *szText, INT32 lFont, INT32 lOffsetY, BOOL bImageFont, DWORD dwColor, FLOAT fScale, INT32 lDuration, BOOL bExistSubText, BOOL bExistAboveText	)
{
	m_ulTextStartTime	=	0			;
	m_ulTextEndTime		=	lDuration	;
	m_bDrawStaticString	=	TRUE		;

	SetRenderScale(1.0f);

	SetStaticStringExt(szText, fScale, lFont, dwColor & 0x00ffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_TOP, bImageFont);
	m_lStringOffsetY	=	lOffsetY						;

	m_bExistSubText		=	bExistSubText					;
	m_bExistAboveText	=	bExistAboveText					;

	return TRUE;
}

BOOL			AgcUIEffect::SetEffectSubText	( CHAR *szText, INT32 lFont, INT32 lOffsetY, BOOL bImageFont, DWORD dwColor, FLOAT fScale	)
{
	strncpy(m_szSubText, szText, ACUI_EFFECT_SUBTEXT_LENGTH - 1);
	m_lSubTextFont		=	lFont			;
	m_lSubTextOffsetY	=	lOffsetY		;
	m_bSubTextImage		=	bImageFont		;
	m_dwSubTextColor	=	dwColor | 0xff000000	;
	m_fSubTextScale		=	fScale			;

	m_lSubTextOffsetX	=	( INT32 ) ( (w - m_pAgcmFont->GetTextExtent(m_lSubTextFont, m_szSubText, strlen(m_szSubText)) * m_fSubTextScale) / 2.0f ) 	;

	return TRUE;
}

BOOL			AgcUIEffect::SetEffectAboveText	( CHAR *szText, INT32 lFont, INT32 lOffsetY, BOOL bImageFont, DWORD dwColor, FLOAT fScale	)
{
	strncpy(m_szAboveText, szText, ACUI_EFFECT_ABOVETEXT_LENGTH - 1);
	m_lAboveTextFont	=	lFont			;
	m_lAboveTextOffsetY	=	lOffsetY		;
	m_bAboveTextImage	=	bImageFont		;
	m_dwAboveTextColor	=	dwColor | 0xff000000	;
	m_fAboveTextScale	=	fScale			;

	m_lAboveTextOffsetX	=	( INT32 ) ( (w - m_pAgcmFont->GetTextExtent(m_lAboveTextFont, m_szAboveText, strlen(m_szAboveText)) * m_fAboveTextScale) / 2.0f )	;

	return TRUE;
}

BOOL			AgcUIEffect::SetEffectImage	( CHAR *szImage, DWORD dwColor, FLOAT fScale, INT32 lDurationFadeIn, INT32 lDurationScaling, INT32 lDurationFixed, INT32 lMarginBottom, INT32 lMarginRight	)
{
	m_lImageDurationFadeIn	=	lDurationFadeIn;
	m_lImageDurationScaling	=	lDurationScaling;
	m_lImageDurationFixed	=	lDurationFixed;

	m_ulImageFadeInEndTime	=	m_pcsAgcmUIManager2->GetClockCount() + lDurationFadeIn;
	m_ulImageScalingEndTime	=	m_pcsAgcmUIManager2->GetClockCount() + lDurationScaling;
	m_ulImageEndTime		=	m_ulImageScalingEndTime + lDurationFixed;

	m_lImageMarginBottom	=	lMarginBottom;
	m_lImageMarginRight		=	lMarginRight;

	m_lImageID				=	m_csTextureList.AddOnlyThisImage(szImage, TRUE);

	RwTexture *	pstTexture	=	m_csTextureList.GetImage_ID(m_lImageID);
	if (!pstTexture)
		return FALSE;

	m_fImageScale	=	fScale;

	SetRednerTexture(m_lImageID);
	SetRenderScale(fScale);

	return TRUE;
}

BOOL			AgcUIEffect::SetEffectFade	( DWORD dwColor, INT32 lDurationOut, INT32 lDurationIn			)
{
	if (!m_pstTextureFade)
		return FALSE;

	m_bFade					=	TRUE;
	m_ulFadeOutStartTime	=	m_pcsAgcmUIManager2->GetClockCount()	;
	m_ulFadeInStartTime		=	m_ulFadeOutStartTime + (lDurationOut == -1 ? 0 : lDurationOut);
	m_lDurationFadeOut		=	lDurationOut							;
	m_lDurationFadeIn		=	lDurationIn								;
	m_dwFadeColor			=	dwColor									;

	return TRUE;
}

// 2005.03.13. steeple
// X 좌표에도 Offset 을 주기 위해서 SubText 에 세팅한다.
BOOL			AgcUIEffect::SetEffectSubText( CHAR *szText, INT32 lFont, INT32 lOffsetX, INT32 lOffsetY, BOOL bImageFont, DWORD dwColor, FLOAT fScale, INT32 lDuration, BOOL bExistSubText, UINT32 ulFadeDelay)
{
	m_ulTextStartTime	=	0			;
	m_ulTextEndTime		=	lDuration	;
	m_bDrawStaticString	=	TRUE		;

	SetRenderScale(1.0f);

	// Main Text 에다가는 ""
	SetStaticStringExt("", fScale, lFont, dwColor & 0x00ffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_TOP, bImageFont);
	m_lStringOffsetY	=	lOffsetY						;

	m_bExistSubText		=	bExistSubText					;

	strncpy(m_szSubText, szText, ACUI_EFFECT_SUBTEXT_LENGTH - 1);
	m_lSubTextFont		=	lFont			;
	m_lSubTextOffsetX	=	lOffsetX		;
	m_lSubTextOffsetY	=	lOffsetY		;
	m_bSubTextImage		=	bImageFont		;
	m_dwSubTextColor	=	dwColor | 0xff000000	;
	m_fSubTextScale		=	fScale			;

	return TRUE;
}