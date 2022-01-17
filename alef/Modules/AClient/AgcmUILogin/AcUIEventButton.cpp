#include "AcUIEventButton.h"

AcUIEventButton*	AcUIEventButton::m_pLastClickDown	=	NULL;

AcUIEventButton::AcUIEventButton( VOID )
: m_fnClickEvent(NULL)
, m_pClass(NULL)
, m_fnFocusEvent(NULL)
, m_pFocusClass(NULL)
, m_pFocusData1(NULL)
, m_pFocusData2(NULL)
, m_fnKillFocusEvent(NULL)
, m_pKillFocusClass(NULL)
, m_pKillFocusData1(NULL)
, m_pKillFocusData2(NULL)
{
}

AcUIEventButton::~AcUIEventButton( VOID )
{
}

VOID	AcUIEventButton::SetCallbackClickEvent( PVOID pClass , fnEventCallback EventCallback )
{
	m_pClass		=	pClass;
	m_fnClickEvent	=	EventCallback;
}

BOOL	AcUIEventButton::OnLButtonDown( RsMouseStatus *ms )
{
	AcUIButton::OnLButtonDown( ms );

	m_pLastClickDown	=	this;

	return TRUE;
}

BOOL	AcUIEventButton::OnLButtonUp( RsMouseStatus *ms )
{
	AcUIButton::OnLButtonUp( ms );

	if( m_fnClickEvent && m_pLastClickDown == this )
	{
		m_fnClickEvent( m_pClass , NULL , NULL , this );
	}

	m_pLastClickDown	=	NULL;

	return TRUE;
}

VOID AcUIEventButton::SetCallbackSetFocusEvent( PVOID pClass , fnEventCallback EventCallback )
{
	m_pFocusClass	=	pClass;
	m_fnFocusEvent	=	EventCallback;
}

VOID AcUIEventButton::SetCallbackKillFocusEvent( PVOID pClass , fnEventCallback EventCallback )
{
	m_pKillFocusClass	=	pClass;
	m_fnKillFocusEvent	=	EventCallback;
}

VOID AcUIEventButton::OnSetFocus( VOID )
{
	AcUIButton::OnSetFocus();

	if( m_fnFocusEvent )
		m_fnFocusEvent( m_pFocusClass , m_pFocusData1 , m_pFocusData2 , this );
}

VOID AcUIEventButton::OnKillFocus( VOID )
{
	AcUIButton::OnKillFocus();

	if( m_fnKillFocusEvent )
		m_fnKillFocusEvent( m_pKillFocusClass , m_pKillFocusData1 , m_pKillFocusData2 , this );

}

VOID AcUIEventButton::SetFocus( VOID )
{

}

VOID AcUIEventButton::KillFocus( VOID )
{

}
