// AgcUIEffect.h: interface for the AgcUIEffect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCUIEFFECT_H__622605AB_570B_440D_9438_5976C6E237E9__INCLUDED_)
#define AFX_AGCUIEFFECT_H__622605AB_570B_440D_9438_5976C6E237E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcEngine.h"
#include "AgcdUIManager2.h"

#define ACUI_EFFECT_ABOVETEXT_LENGTH	64
#define ACUI_EFFECT_SUBTEXT_LENGTH		64

#define	AGCUIEFFECT_FADE_DELAY			500

class AgcmUIManager2;

class AgcUIEffect : public AcUIBase
{
private:
	AgcmUIManager2 *	m_pcsAgcmUIManager2		;

	UINT32				m_ulTextStartTime		;
	UINT32				m_ulTextEndTime			;

	FLOAT				m_fImageScale			;
	UINT32				m_ulImageFadeInEndTime	;	// Image가 FadeIn이 다 된 시간
	UINT32				m_ulImageScalingEndTime	;
	UINT32				m_ulImageEndTime		;
	INT32				m_lImageDurationFadeIn	;
	INT32				m_lImageDurationScaling	;
	INT32				m_lImageDurationFixed	;
	INT32				m_lImageID				;
	INT32				m_lImageMarginBottom	;
	INT32				m_lImageMarginRight		;

	BOOL				m_bFade					;
	DWORD				m_dwFadeColor			;
	INT32				m_lFadeAlpha			;
	UINT32				m_ulFadeOutStartTime	;
	UINT32				m_ulFadeInStartTime		;
	INT32				m_lDurationFadeOut		;
	INT32				m_lDurationFadeIn		;
	RwTexture *			m_pstTextureFade		;

	BOOL				m_bExistSubText			;
	BOOL				m_bExistAboveText		;

	CHAR				m_szAboveText[ACUI_EFFECT_ABOVETEXT_LENGTH]			;
	INT32				m_lAboveTextFont;
	INT32				m_lAboveTextOffsetY;
	INT32				m_lAboveTextOffsetX;
	BOOL				m_bAboveTextImage;
	DWORD				m_dwAboveTextColor;
	FLOAT				m_fAboveTextScale;

	CHAR				m_szSubText[ACUI_EFFECT_SUBTEXT_LENGTH]			;
	INT32				m_lSubTextFont;
	INT32				m_lSubTextOffsetY;
	INT32				m_lSubTextOffsetX;
	BOOL				m_bSubTextImage;
	DWORD				m_dwSubTextColor;
	FLOAT				m_fSubTextScale;

public:
	AgcUIEffect(AgcmUIManager2 *pcsAgcmUIManager2);
	virtual ~AgcUIEffect();

	virtual BOOL	OnInit			();
	virtual VOID	OnClose			();
	virtual BOOL	OnIdle			( UINT32 ulClockCount	);
	virtual VOID	OnWindowRender	();

	//BOOL			UseEffect		( UINT32 ulEffectID		);
	BOOL			SetEffectText	( CHAR *szText, INT32 lFont, INT32 lOffsetY, BOOL bImageFont, DWORD dwColor, FLOAT fScale, INT32 lDuration, BOOL bExistSubText = FALSE, BOOL bExistAboveText = FALSE	);
	BOOL			SetEffectSubText( CHAR *szText, INT32 lFont, INT32 lOffsetY, BOOL bImageFont, DWORD dwColor, FLOAT fScale	);
	BOOL			SetEffectAboveText( CHAR *szText, INT32 lFont, INT32 lOffsetY, BOOL bImageFont, DWORD dwColor, FLOAT fScale	);
	BOOL			SetEffectImage	( CHAR *szImage, DWORD dwColor, FLOAT fScale, INT32 lDurationFadeIn, INT32 lDurationScaling, INT32 lDurationFixed, INT32 lMarginBottom = 0, INT32 lMarginRight = 0	);
	BOOL			SetEffectFade	( DWORD dwColor, INT32 lDurationOut, INT32 lDurationIn			);
					// lDurationOut이나 lDurationIn 이 -1이면 해당 Fade는 하지 않는다.

	// 2005.03.13. steeple
	BOOL			SetEffectSubText( CHAR *szText, INT32 lFont, INT32 lOffsetX, INT32 lOffsetY, BOOL bImageFont, DWORD dwColor, FLOAT fScale, INT32 lDuration, BOOL bExistSubText = FALSE, UINT32 ulFadeDelay = AGCUIEFFECT_FADE_DELAY);
};

#endif // !defined(AFX_AGCUIEFFECT_H__622605AB_570B_440D_9438_5976C6E237E9__INCLUDED_)
