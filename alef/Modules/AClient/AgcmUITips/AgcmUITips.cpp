#include "AgcmUITips.h"

#define	AGCM_TIP_NAME_TEXTURE		"Texture"
#define AGCM_TIP_TEXT				"Text"
#define AGCM_TIP_TEXT_HOUR			"Hour"
#define AGCM_TIP_TEXT_TYPE			"Type"
#define AGCM_TIP_LOADING_WARNING	"WARN_MESSAGE_%d"

const UINT32 AGCM_TIP_TEXT_WARN_DELAY	=	3600000;
const UINT32 AGCM_TIP_TEXT_DELAY		=	120000;
const UINT32 AGCM_TIP_TEXT_TIME			=	10000;

#pragma warning ( disable : 4355 )

AgcmUITips::AgcmUITips() :	m_csTipWIndow1024(this),
							m_csTipWIndow1280(this),
							m_csTipWIndow1600(this)
{
	SetModuleName("AgcmUITips")					;
	EnableIdle(TRUE)							;

	m_pAgcmUIManager2		=		NULL		;
	m_pAgcmResourceLoader	=		NULL		;

	m_pstTipImages			=		NULL		;
	m_lNumTipImages			=		0			;

	m_pstTipTexts			=		NULL		;
	m_lNumTipTexts			=		0			;
	m_lNumWarnTexts			=		0			;
	m_lNumSiegewarTexts		=		0			;

	m_szTexturePath[0]		=		0			;

	m_ulTextOpenTime		=		0			;
	m_ulGameStartTime		=		0			;
	m_lWarnTime				=		0			;

	m_lTipText				=		0			;
	m_bShowTipText			=		FALSE		;

	m_bEnableTipText		=		TRUE		;

	m_lLoadingWarning		=		1			;
	m_szLoadingWarning		=		NULL		;
}

AgcmUITips::~AgcmUITips()
{

}

BOOL	AgcmUITips::OnInit()
{	
	return TRUE;
}

BOOL	AgcmUITips::OnAddModule()
{
	// Get Module
	m_pAgcmUIManager2		=	(AgcmUIManager2		*)GetModule("AgcmUIManager2")		;
	m_pAgcmLogin			=	(AgcmLogin			*)GetModule("AgcmLogin");
	m_pAgcmResourceLoader	=	(AgcmResourceLoader	*)GetModule("AgcmResourceLoader")	;

	ASSERT( NULL != m_pAgcmUIManager2		);
	ASSERT( NULL != m_pAgcmResourceLoader		);

	m_pstUDTip	= m_pAgcmUIManager2->AddUserData("Tip_Text", &m_lTipText, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDTip)
		return FALSE;

	m_pstUDLoadingWarning	= m_pAgcmUIManager2->AddUserData("Loading_Warning", &m_szLoadingWarning, sizeof(CHAR), 1, AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pstUDLoadingWarning)
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, "Tip_Next", CBNextTip, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, "Tip_Prev", CBPrevTip, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddCustomControl("Tip1024", &m_csTipWIndow1024))
		return FALSE;

	if (!m_pAgcmUIManager2->AddCustomControl("Tip1280", &m_csTipWIndow1280))
		return FALSE;

	if (!m_pAgcmUIManager2->AddCustomControl("Tip1600", &m_csTipWIndow1600))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "Tip_Text", 0, CBDisplayTip, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	m_lEventOpenTipText		= m_pAgcmUIManager2->AddEvent("Tip_OpenText");
	if (m_lEventOpenTipText < 0)
		return FALSE;

	m_lEventCloseTipText	= m_pAgcmUIManager2->AddEvent("Tip_CloseText");
	if (m_lEventCloseTipText < 0)
		return FALSE;

	return TRUE;
}

BOOL	AgcmUITips::OnIdle(UINT32 ulClockCount)
{
	if (m_pAgcmLogin && m_pAgcmLogin->GetLoginMode() == AGCMLOGIN_MODE_POST_LOGIN && m_lNumTipTexts)
	{
		if (!m_ulGameStartTime)
			m_ulGameStartTime	= ulClockCount;

		// Delay가 지나면.. 다음 Tip
		if (!m_ulTextOpenTime || (!m_bShowTipText && ulClockCount >= m_ulTextOpenTime + AGCM_TIP_TEXT_DELAY))
		{
			if (m_bEnableTipText && !m_bShowTipText)
			{
				m_lTipText			= m_csRand.randInt(m_lNumTipTexts - 1);
				m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDTip);
				m_pAgcmUIManager2->ThrowEvent(m_lEventOpenTipText);
			}

			m_bShowTipText		= TRUE;
			m_ulTextOpenTime	= ulClockCount;
		}
	}

	if (m_bShowTipText && ulClockCount >= m_ulTextOpenTime + AGCM_TIP_TEXT_TIME)
	{
		m_bShowTipText		= FALSE;
		m_ulTextOpenTime	= ulClockCount;
		m_pAgcmUIManager2->ThrowEvent(m_lEventCloseTipText);
	}

	if (m_ulGameStartTime && (UINT32) m_lWarnTime < ((ulClockCount - m_ulGameStartTime) / AGCM_TIP_TEXT_WARN_DELAY))
	{
		INT32				lIndex;

		m_lWarnTime	= (ulClockCount - m_ulGameStartTime) / AGCM_TIP_TEXT_WARN_DELAY;

		for (lIndex = m_lNumTipTexts + m_lNumSiegewarTexts; lIndex < m_lNumTipTexts + m_lNumSiegewarTexts + m_lNumWarnTexts; ++lIndex)
		{
			if (m_pstTipTexts[lIndex].m_lHours <= m_lWarnTime)
				m_lTipText	= lIndex;
			else
				break;
		}

		if (m_lTipText >= m_lNumTipTexts)
		{
			m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDTip);
			m_pAgcmUIManager2->ThrowEvent(m_lEventOpenTipText);

			if (!m_bShowTipText)
				m_pAgcmUIManager2->ThrowEvent(m_lEventOpenTipText);

			m_bShowTipText		= TRUE;
			m_ulTextOpenTime	= ulClockCount;
		}
	}

	return TRUE;
}

BOOL	AgcmUITips::OnDestroy()
{
	DestroyTipImages();
	DestroyTipTexts();

	return TRUE;
}

BOOL	AgcmUITips::ReadTipImages(CHAR *szFile, BOOL bDecryption)
{
	INT32					lRow;
	INT32					lColumn;
	AuExcelTxtLib			csStream;
	CHAR *					pszData					= NULL;

	if(!csStream.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgcmUITips::ReadTips() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	DestroyTipImages();

	m_lNumTipImages	= csStream.GetRow() - 1;

	m_pstTipImages	= new AgcdUITipImage [m_lNumTipImages];
	if (!m_pstTipImages)
	{
		OutputDebugString("AgcmUITips::ReadTipImages() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	for(lRow = 1; lRow <= m_lNumTipImages; ++lRow)
	{
		for (lColumn = 0; lColumn < csStream.GetColumn(); ++lColumn)
		{
			pszData = csStream.GetData(lColumn, 0);
			if(!pszData)
			{
				continue;
			}

			if (!stricmp(pszData, AGCM_TIP_NAME_TEXTURE))
			{
				pszData = csStream.GetData(lColumn, lRow);

				if (pszData)
				{
					m_pstTipImages[lRow - 1].m_szTextureName = new CHAR [strlen(pszData) + 1];
					strcpy(m_pstTipImages[lRow - 1].m_szTextureName, pszData);
				}
			}
		}
	}

	csStream.CloseFile();

	return TRUE;
}

static INT32 __cdecl TipTextCompare(const void *Tip1, const void *Tip2)
{
	AgcdUITipText *		pcsTip1 = (AgcdUITipText *) Tip1;
	AgcdUITipText *		pcsTip2 = (AgcdUITipText *) Tip2;

	if ((INT32) pcsTip1->m_eType == (INT32) pcsTip2->m_eType)
		return (pcsTip1->m_lHours - pcsTip2->m_lHours);

	return ((INT32) pcsTip1->m_eType == (INT32) pcsTip2->m_eType);
}

BOOL	AgcmUITips::ReadTipTexts(CHAR *szFile, BOOL bDecryption)
{
	INT32					lRow;
	INT32					lColumn;
	AuExcelTxtLib			csStream;
	CHAR *					pszData					= NULL;

	if(!csStream.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgcmUITips::ReadTips() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	DestroyTipTexts();

	m_pstTipTexts	= new AgcdUITipText [csStream.GetRow() - 1];
	if (!m_pstTipTexts)
	{
		OutputDebugString("AgcmUITips::ReadTipTexts() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	for(lRow = 1; lRow < csStream.GetRow(); ++lRow)
	{
		for (lColumn = 0; lColumn < csStream.GetColumn(); ++lColumn)
		{
			pszData = csStream.GetData(lColumn, 0);
			if(!pszData)
			{
				continue;
			}

			if (!stricmp(pszData, AGCM_TIP_TEXT))
			{
				pszData = csStream.GetData(lColumn, lRow);

				if (pszData)
				{
					m_pstTipTexts[lRow - 1].m_szText = new CHAR [strlen(pszData) + 1];
					strcpy(m_pstTipTexts[lRow - 1].m_szText, pszData);
				}
			}
			else if (!stricmp(pszData, AGCM_TIP_TEXT_HOUR))
			{
				m_pstTipTexts[lRow - 1].m_lHours = csStream.GetDataToInt(lColumn, lRow);

				if (m_pstTipTexts[lRow - 1].m_lHours)
					m_pstTipTexts[lRow - 1].m_eType = AGCDUI_TIPTEXT_TYPE_WARN;
			}
			else if (!stricmp(pszData, AGCM_TIP_TEXT_TYPE))
			{
				m_pstTipTexts[lRow - 1].m_eType = (AgcdUITipTextType) csStream.GetDataToInt(lColumn, lRow);

				if (m_pstTipTexts[lRow - 1].m_lHours)
					m_pstTipTexts[lRow - 1].m_eType = AGCDUI_TIPTEXT_TYPE_WARN;
			}
		}

		switch (m_pstTipTexts[lRow - 1].m_eType)
		{
		case AGCDUI_TIPTEXT_TYPE_SIEGEWAR:
			++m_lNumSiegewarTexts;
			break;

		case AGCDUI_TIPTEXT_TYPE_WARN:
			++m_lNumWarnTexts;
			break;

		default:
			++m_lNumTipTexts;
			break;
		}
	}

	qsort(m_pstTipTexts, m_lNumTipTexts + m_lNumSiegewarTexts + m_lNumWarnTexts, sizeof(AgcdUITipText), TipTextCompare);

	csStream.CloseFile();

	return TRUE;
}

VOID	AgcmUITips::DestroyTipImages()
{
	INT32	lIndex;

	if (m_pstTipImages)
	{
		for (lIndex = 0; lIndex < m_lNumTipImages; ++lIndex)
		{
			if (m_pstTipImages[lIndex].m_szTextureName)
			{
				delete [] m_pstTipImages[lIndex].m_szTextureName;
				m_pstTipImages[lIndex].m_szTextureName	= NULL;
			}
		}

		delete [] m_pstTipImages;

		m_pstTipImages	= NULL;
	}

	m_lNumTipImages	= 0;
}

VOID	AgcmUITips::DestroyTipTexts()
{
	INT32	lIndex;

	if (m_pstTipTexts)
	{
		for (lIndex = 0; lIndex < m_lNumTipTexts + m_lNumSiegewarTexts + m_lNumWarnTexts; ++lIndex)
		{
			if (m_pstTipTexts[lIndex].m_szText)
			{
				delete [] m_pstTipTexts[lIndex].m_szText;
				m_pstTipTexts[lIndex].m_szText	= NULL;
			}
		}

		delete [] m_pstTipTexts;

		m_pstTipTexts	= NULL;
	}

	m_lNumTipTexts			= 0;
	m_lNumWarnTexts			= 0;
	m_lNumSiegewarTexts		= 0;
}

RwTexture *		AgcmUITips::GetRandomTipImage()
{
	if (!m_pstTipImages || m_lNumTipImages <= 0)
		return NULL;

	INT32		lNumTry		= 10;
	INT32		lIndex;
	RwTexture *	pstTexture	= NULL;
	CHAR		szLoadingWarningName[32];

	sprintf(szLoadingWarningName, AGCM_TIP_LOADING_WARNING, m_lLoadingWarning);
	if ((m_szLoadingWarning = m_pAgcmUIManager2->GetUIMessage(szLoadingWarningName)))
	{
		m_pstUDLoadingWarning->m_stUserData.m_pvData = &m_szLoadingWarning;
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDLoadingWarning);
		++m_lLoadingWarning;
	}
	else
	{
		m_lLoadingWarning = 1;
		sprintf(szLoadingWarningName, AGCM_TIP_LOADING_WARNING, m_lLoadingWarning);
		if ((m_szLoadingWarning = m_pAgcmUIManager2->GetUIMessage(szLoadingWarningName)))
		{
			m_pstUDLoadingWarning->m_stUserData.m_pvData = &m_szLoadingWarning;
			m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDLoadingWarning);
			++m_lLoadingWarning;
		}
	}

	while (lNumTry-- > 0)
	{
		lIndex		= m_csRand.randInt(m_lNumTipImages - 1);
		if (!m_pstTipImages[lIndex].m_szTextureName)
			continue;

		pstTexture	= m_pAgcmResourceLoader->LoadTexture(m_pstTipImages[lIndex].m_szTextureName, NULL, NULL, NULL, -1, m_szTexturePath);
		if (pstTexture)
			break;
	}
	
	if( pstTexture )
		RwTextureSetAddressing( pstTexture, rwTEXTUREADDRESSCLAMP );

	return pstTexture;
}

VOID	AgcmUITips::SetTexturePath(CHAR *szPath)
{
	strcpy(m_szTexturePath, szPath);
}

VOID	AgcmUITips::EnableTipText(BOOL bEnable)
{
	if (m_bEnableTipText == bEnable)
		return;

	if (TRUE && !bEnable )
		m_pAgcmUIManager2->ThrowEvent(m_lEventCloseTipText);
	else if (m_bShowTipText && bEnable )
		m_pAgcmUIManager2->ThrowEvent(m_lEventOpenTipText);

	m_bEnableTipText = bEnable;
}


BOOL	AgcmUITips::CBNextTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUITips *	pThis = (AgcmUITips *) pClass;

	if (!pThis->m_pstTipTexts || pThis->m_lNumTipTexts <= 0)
		return FALSE;

	if (pThis->m_lTipText >= pThis->m_lNumTipTexts)
		return FALSE;

	++pThis->m_lTipText;
	if (pThis->m_lTipText >= pThis->m_lNumTipTexts)
		pThis->m_lTipText = 0;

	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDTip);

	pThis->m_ulTextOpenTime	= 0;

	return TRUE;
}

BOOL	AgcmUITips::CBPrevTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUITips *	pThis = (AgcmUITips *) pClass;

	if (!pThis->m_pstTipTexts || pThis->m_lNumTipTexts <= 0)
		return FALSE;

	if (pThis->m_lTipText >= pThis->m_lNumTipTexts)
		return FALSE;

	--pThis->m_lTipText;
	if (pThis->m_lTipText < 0)
		pThis->m_lTipText = pThis->m_lNumTipTexts - 1;

	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDTip);

	pThis->m_ulTextOpenTime	= 0;

	return TRUE;
}

BOOL	AgcmUITips::CBDisplayTip(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	AgcmUITips *	pThis = (AgcmUITips *) pClass;
	CHAR *			szText = "";

	if (!pThis->m_pstTipTexts || pThis->m_lTipText <= 0)
		return FALSE;

	szText = pThis->m_pstTipTexts[pThis->m_lTipText].m_szText;
	if (!szText)
		return FALSE;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_EDIT)
	{
		CHAR	szDisplay[512];

		if (pThis->m_lTipText >= pThis->m_lNumTipTexts)
            sprintf(szDisplay, szText, pThis->m_lWarnTime);
		else
			sprintf(szDisplay, szText);

		((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(szDisplay);
	}
	else if (szDisplay)
	{
		if (pThis->m_lTipText >= pThis->m_lNumTipTexts)
            sprintf(szDisplay, szText, pThis->m_lWarnTime);
		else
			sprintf(szDisplay, szText);
	}

	return TRUE;
}

