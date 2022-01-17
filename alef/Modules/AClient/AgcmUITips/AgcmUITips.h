#ifndef			_AGCMUITIPS_H_
#define			_AGCMUITIPS_H_

#include "AuRandomNumber.h"
#include "AgcmUIManager2.h"
#include "AgcmLogin.h"
#include "AgcmResourceLoader.h"
#include "AgcTipWindow.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUITipsD" )
#else
#pragma comment ( lib , "AgcmUITips" )
#endif
#endif

#define AGCDUITIPS_TEXT_LENGTH	128

class AgcdUITipImage
{
public:
	CHAR *	m_szTextureName;

	AgcdUITipImage()
	{
		m_szTextureName	= NULL;
	}
};

typedef enum
{
	AGCDUI_TIPTEXT_TYPE_NORMAL = 0,
	AGCDUI_TIPTEXT_TYPE_SIEGEWAR,
	AGCDUI_TIPTEXT_TYPE_WARN,
} AgcdUITipTextType;

class AgcdUITipText
{
public:
	AgcdUITipTextType	m_eType;
	INT32				m_lHours;
	CHAR *				m_szText;

	AgcdUITipText()
	{
		m_eType = AGCDUI_TIPTEXT_TYPE_NORMAL;
		m_lHours = 0;
		m_szText = NULL;
	}
};


class AgcmUITips : public AgcModule
{
public:
	AgcmUITips()	;
	~AgcmUITips()	;

	virtual BOOL	OnAddModule();
	virtual BOOL	OnInit();
	virtual BOOL	OnIdle(UINT32 ulClockCount);
	virtual BOOL	OnDestroy();
	
public:
	AgcmUIManager2		*	m_pAgcmUIManager2		;
	AgcmLogin			*	m_pAgcmLogin			;
	AgcmResourceLoader	*	m_pAgcmResourceLoader	;
	MTRand					m_csRand;

	BOOL					m_bEnableTipText;

	INT32					m_lTipText;
	AgcdUIUserData *		m_pstUDTip;
	AgcdUIUserData *		m_pstUDLoadingWarning;

	AgcTipWindow			m_csTipWIndow1024;
	AgcTipWindow			m_csTipWIndow1280;
	AgcTipWindow			m_csTipWIndow1600;

	CHAR					m_szTexturePath[128]	;

	INT32					m_lNumTipImages;
	AgcdUITipImage *		m_pstTipImages;

	INT32					m_lNumTipTexts;
	INT32					m_lNumWarnTexts;
	INT32					m_lNumSiegewarTexts;
	AgcdUITipText *			m_pstTipTexts;
	BOOL					m_bShowTipText;

	INT32					m_lWarnTime;
	UINT32					m_ulGameStartTime;
	UINT32					m_ulTextOpenTime;

	INT32					m_lEventOpenTipText;
	INT32					m_lEventCloseTipText;

	INT32					m_lLoadingWarning;
	CHAR *					m_szLoadingWarning;

public:
	BOOL			ReadTipImages(CHAR *szFile, BOOL bDecryption = FALSE);
	BOOL			ReadTipTexts(CHAR *szFile, BOOL bDecryption = FALSE);
	VOID			DestroyTipImages();
	VOID			DestroyTipTexts();

	RwTexture *		GetRandomTipImage();
	CHAR *			GetNextTipText();
	CHAR *			GetPrevTipText();

	VOID			SetTexturePath(CHAR *szPath);

	VOID			EnableTipText(BOOL bEnable = TRUE);

	static	BOOL	CBDisplayTip(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBNextTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBPrevTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
};

#endif			// _AGCMUITIPS_H_

