#include "AgcTipWindow.h"
#include "AgcmUITips.h"

#include "RwCore.h"

#define	AGC_TIP_DURATION	60000

AgcTipWindow::AgcTipWindow(AgcmUITips *pcsAgcmUITips)
{
	m_pcsAgcmUITips		= pcsAgcmUITips;
	m_ulPrevClockCount	= 0;
	m_pstTipTexture		= NULL;
}

AgcTipWindow::~AgcTipWindow()
{
}

BOOL	AgcTipWindow::OnInit()
{
	return TRUE;
}

VOID	AgcTipWindow::OnWindowRender()
{
	
	UINT32	ulClockCount	= m_pcsAgcmUITips->GetClockCount();

	if (!m_csTextureList.GetImage_ID(GetRenderTexture()))
	{
		m_ulPrevClockCount	= 0;
		m_pstTipTexture		= NULL;
	}

	if (ulClockCount - m_ulPrevClockCount >= AGC_TIP_DURATION)
	{		
		INT32	lTextureID;

		m_csTextureList.DestroyTextureList();

		m_pstTipTexture	= m_pcsAgcmUITips->GetRandomTipImage();
		if (m_pstTipTexture)
		{
			lTextureID	= m_csTextureList.AddTexture(m_pstTipTexture);
			SetRednerTexture(lTextureID);
		}

		m_ulPrevClockCount		= ulClockCount;
	}
	

	return AcUIBase::OnWindowRender();
}

VOID	AgcTipWindow::OnClose()
{
	m_ulPrevClockCount	= 0;
}