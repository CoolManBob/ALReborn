#include "AgcSenderWindow.h"
#include "AgcmTargeting.h"

AgcSenderWindow::AgcSenderWindow()
{
	x		=	0			;
	y		=	0			;
	w		=	1600		;
	h		=	1200		;
}
AgcSenderWindow::~AgcSenderWindow()
{

}

BOOL AgcSenderWindow::OnInit			(					)	
{
	return TRUE;
}

BOOL AgcSenderWindow::OnLButtonDown	( RsMouseStatus *ms	)
{
	RwV2d pos = ms->pos; 
	m_pcmTargeting->OnLButtonDown(&pos);

	return TRUE;
}

BOOL AgcSenderWindow::OnMouseMove( RsMouseStatus *ms )
{
	RwV2d pos = ms->pos; 
	m_pcmTargeting->OnMouseMove(&pos);

	return FALSE;
}

BOOL AgcSenderWindow::OnLButtonDblClk( RsMouseStatus *ms	)
{
	RwV2d pos = ms->pos;
	m_pcmTargeting-> OnLButtonDblClk(&pos);

	return TRUE;
}

//@{ Jaewon 20051201
// For the right button double click moving
BOOL AgcSenderWindow::OnRButtonDblClk( RsMouseStatus *ms	)
{
	RwV2d pos = ms->pos;
	m_pcmTargeting-> OnRButtonDblClk(&pos);

	return TRUE;
}
//@} Jaewon

BOOL AgcSenderWindow::OnRButtonDown	( RsMouseStatus *ms	)
{
	RwV2d pos = ms->pos;
	m_pcmTargeting->OnRButtonDown(&pos);

	return TRUE;
}

BOOL AgcSenderWindow::OnLButtonUp	( RsMouseStatus *ms	)
{
	RwV2d pos = ms->pos;
	m_pcmTargeting->OnLButtonUp(&pos);

	return TRUE;
}

BOOL AgcSenderWindow::OnKeyDown		( RsKeyStatus *ks	)
{
 //   switch( ks->keyCharCode )
 //   {
	//case rsENTER:
	//	if (m_pcmTargeting->m_bCurrentLoginMode)
	//	{
	//		if (m_pcmTargeting->m_pAgcmUILogin->GetLoginMainSelectCharacter() &&
	//			!m_pcmTargeting->m_pAgcmUILogin->m_bIsOpenDeleteConfirmUI)
	//			m_pcmTargeting->m_pAgcmUILogin->CharacterDoubleClick( m_pcmTargeting->m_pAgcmUILogin->GetLoginMainSelectCharacter() );

	//		return TRUE;
	//	}

	//	break;

	//case rsRIGHT:
	//	if (m_pcmTargeting->m_bCurrentLoginMode)
	//	{
	//		m_pcmTargeting->m_pAgcmUILogin->NextCharacterClick();

	//		return TRUE;
	//	}

	//	break;

	//case rsLEFT:
	//	if (m_pcmTargeting->m_bCurrentLoginMode)
	//	{
	//		m_pcmTargeting->m_pAgcmUILogin->PrevCharacterClick();

	//		return TRUE;
	//	}

	//case rsBACKSP:
	//	if (m_pcmTargeting->m_bCurrentLoginMode)
	//	{
	//		m_pcmTargeting->m_pAgcmUILogin->CancelCurrentProcess();

	//		return TRUE;
	//	}

	//	break;
	//}

	return FALSE;
}
