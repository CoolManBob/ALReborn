#pragma once

#include "AcUIButton.h"

typedef BOOL (*fnEventCallback)( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton );


class AcUIEventButton
	:	public AcUIButton
{
public:
	AcUIEventButton								( VOID );
	virtual ~AcUIEventButton					( VOID );

	VOID			SetCallbackClickEvent		( PVOID pClass , fnEventCallback	EventCallback );

	VOID			SetCallbackSetFocusEvent	( PVOID pClass , fnEventCallback	EventCallback );
	VOID			SetCallbackKillFocusEvent	( PVOID pClass , fnEventCallback	EventCallback );

	virtual BOOL	OnLButtonDown				( RsMouseStatus *ms	);	
	virtual BOOL	OnLButtonUp					( RsMouseStatus *ms );

	virtual VOID	OnSetFocus					( VOID );
	virtual VOID	OnKillFocus					( VOID );

	virtual VOID	SetFocus					( VOID );
	virtual VOID	KillFocus					( VOID );

protected:
	fnEventCallback				m_fnClickEvent;
	PVOID						m_pClass;

	fnEventCallback				m_fnFocusEvent;
	PVOID						m_pFocusClass;
	PVOID						m_pFocusData1;
	PVOID						m_pFocusData2;

	fnEventCallback				m_fnKillFocusEvent;
	PVOID						m_pKillFocusClass;
	PVOID						m_pKillFocusData1;
	PVOID						m_pKillFocusData2;

	static	AcUIEventButton*	m_pLastClickDown;
};
