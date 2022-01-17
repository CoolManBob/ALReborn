#ifndef		_ACUISCROLL_H_
#define		_ACUISCROLL_H_

#include "AcUIBase.h"
#include "AcUIButton.h"

enum
{
	UICM_SCROLLBUTTON_MOVE							= UICM_BASE_MAX_MESSAGE	,
	UICM_SCROLLBUTTON_MAX_MESSAGE
};

class AcUIScrollButton	: public AcUIButton	// 가운데 스크롤 핸들러 버튼 
{
public:
	BOOL											m_bVScroll;

	INT32											m_lMinPosition;
	INT32 											m_lMaxPosition;
	FLOAT											m_fScrollUnit;		// Up Down Button Change Unit
	BOOL											m_bMoveByUnit;

	AcUIScrollButton( void ) : m_lMinPosition( 0 ), m_lMaxPosition( 0 )
	{
		m_Property.bMovable = TRUE;
		m_nType = TYPE_SCROLL_BUTTON;

		x = 0;
		y = 0;
	}

public:
	virtual	BOOL			OnMouseMove				( RsMouseStatus *ms	);
};

/* AcUIScrollButton */
/**************************************************************************************************/
/**************************************************************************************************/
/* AcUIScroll */

// Command Message
enum 
{
	UICM_SCROLL_CHANGE_SCROLLVALUE					= UICM_BASE_MAX_MESSAGE,
	UICM_SCROLL_UPBUTTON_DOWN, 
	UICM_SCROLL_DOWNBUTTON_DOWN,
	UICM_SCROLL_MAX_MESSAGE
};

class AcUIScroll : public AcUIBase 
{
public:
	AcUIScroll( void );
	virtual ~AcUIScroll( void );

public:
	BOOL											m_bVScroll;

	INT32											m_lMinPosition;
	INT32 											m_lMaxPosition;
	FLOAT											m_fScrollUnit;		// Up Down Button Change Unit
	BOOL											m_bMoveByUnit;

private:
	FLOAT											m_fScrollValue;		// Scroll Value		0	~	1

	AcUIScrollButton*								m_pcsButtonScroll;
	AcUIButton*										m_pcsButtonUp;
	AcUIButton*										m_pcsButtonDown;

public:
	void					SetScrollButtonInfo		( BOOL bVScroll, INT32 lMin, INT32 lMax, FLOAT fUnit, BOOL bMoveByUnit = FALSE );
	void					SetScrollButton			( AcUIScrollButton *pcsButtonScroll );
	void					SetScrollUpButton		( AcUIButton *pcsButtonUp );
	void					SetScrollDownButton		( AcUIButton *pcsButtonDown );
	BOOL					SetScrollValue			( float fScrollValue, BOOL bSendMessage = TRUE );	// Scroll Value 를 바꾸고 ScrollButton의 위치를 바꾼다 
	FLOAT					GetScrollValue			( void );											// Scroll Value 를 얻는다 
	void					CalcScrollValue			( void );											// Scroll Value 를 계산한다 

	AcUIButton*				GetScrollUpButton		( void ) { return m_pcsButtonUp; }
	AcUIButton*				GetScrollDownButton		( void ) { return m_pcsButtonDown; }
	
	virtual BOOL			OnInit					( void );
	virtual	BOOL			OnCommand				( INT32	nID , PVOID pParam );
	virtual	void			OnMoveWindow			( void );
};

#endif		// _ACUISCROLL_H_

