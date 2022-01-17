#include "AcUIScroll.h"
#include "AcUIEdit.h"

AcUIScroll::AcUIScroll( void )
{
	m_nType = TYPE_SCROLL;

	m_fScrollValue = 0.0f;
	m_fScrollUnit =	0.1f;

	m_lMinPosition = 0;
	m_lMaxPosition = 0;

	m_pcsButtonScroll =	NULL;
	m_pcsButtonUp =	NULL;
	m_pcsButtonDown =	NULL;

	m_bMoveByUnit =	FALSE;
	m_bVScroll = FALSE;
}

AcUIScroll::~AcUIScroll( void )
{
}

BOOL AcUIScroll::OnInit( void )
{
	AcUIBase::OnInit();
	return TRUE;
}

void AcUIScroll::SetScrollButtonInfo( BOOL bVScroll, INT32 lMin, INT32 lMax, FLOAT fUnit, BOOL bMoveByUnit )
{
	m_bVScroll = bVScroll;

	m_lMinPosition = lMin;
	m_lMaxPosition = lMax;
	m_fScrollUnit = fUnit;
	m_bMoveByUnit = bMoveByUnit;

	if( m_pcsButtonScroll )
	{
		m_pcsButtonScroll->m_bVScroll = bVScroll;
		m_pcsButtonScroll->m_lMinPosition = lMin;
		m_pcsButtonScroll->m_lMaxPosition = lMax;
		m_pcsButtonScroll->m_fScrollUnit = fUnit;
		m_pcsButtonScroll->m_bMoveByUnit = bMoveByUnit;
	}
}

void AcUIScroll::SetScrollButton( AcUIScrollButton *pcsButtonScroll )
{
	m_pcsButtonScroll = pcsButtonScroll;
	if( m_pcsButtonScroll )
	{
		m_pcsButtonScroll->m_bVScroll = m_bVScroll;
		m_pcsButtonScroll->m_lMinPosition = m_lMinPosition;
		m_pcsButtonScroll->m_lMaxPosition = m_lMaxPosition;
		m_pcsButtonScroll->m_fScrollUnit = m_fScrollUnit;
		m_pcsButtonScroll->m_bMoveByUnit = m_bMoveByUnit;

		if( m_bVScroll )
		{
			m_pcsButtonScroll->MoveWindow( m_pcsButtonScroll->x, m_lMinPosition, m_pcsButtonScroll->w, m_pcsButtonScroll->h );
		}
		else
		{
			m_pcsButtonScroll->MoveWindow( m_lMinPosition, m_pcsButtonScroll->y, m_pcsButtonScroll->w, m_pcsButtonScroll->h );
		}
	}
}
void AcUIScroll::SetScrollUpButton( AcUIButton *pcsButtonUp )
{
	m_pcsButtonUp = pcsButtonUp;
}

void AcUIScroll::SetScrollDownButton( AcUIButton *pcsButtonDown )
{
	m_pcsButtonDown	= pcsButtonDown;
}

float AcUIScroll::GetScrollValue( void )
{
	return m_fScrollValue;
}

void AcUIScroll::CalcScrollValue( void )
{
	if( !m_pcsButtonScroll ) return;
	if( m_bVScroll )	// V Scroll인 경우 
	{
		if( m_lMinPosition == m_lMaxPosition )
		{
			SetScrollValue( 0.0f );
		}
		else
		{
			FLOAT fValue = ( m_pcsButtonScroll->y - m_lMinPosition ) / ( FLOAT )( m_lMaxPosition - m_lMinPosition );
			if( m_bMoveByUnit && m_fScrollUnit != 0.0f )
			{
				fValue = m_fScrollUnit * ( INT32 )( fValue / m_fScrollUnit + m_fScrollUnit / 2.0f );
			}

			SetScrollValue( fValue );
		}
	}
	else				// H Scroll인 경우 
	{
		if( m_lMinPosition == m_lMaxPosition )
		{
			SetScrollValue( 0.0f );
		}
		else 
		{
			SetScrollValue( ( m_pcsButtonScroll->x - m_lMinPosition ) / ( FLOAT )( m_lMaxPosition - m_lMinPosition ) );
		}
	}
}

BOOL AcUIScroll::SetScrollValue( float fScrollValue, BOOL bSendMessage )
{
	// Valid Check 
	if( 0.0f > fScrollValue ) fScrollValue = 0.0f;
	if( 1.0f < fScrollValue ) fScrollValue = 1.0f;
	if( m_fScrollValue == fScrollValue ) return TRUE;

	// Input Value
	m_fScrollValue = fScrollValue;
	if( !m_pcsButtonScroll ) return TRUE;

	// Move Scroll Button
	INT32 lMovePosition = ( INT32 )( ( float )( m_lMaxPosition - m_lMinPosition ) * fScrollValue + ( float )m_lMinPosition );
	if( m_bVScroll )		// V Scroll
	{
		m_pcsButtonScroll->MoveWindow( m_pcsButtonScroll->x, lMovePosition, m_pcsButtonScroll->w, m_pcsButtonScroll->h );
	}
	else 
	{
		m_pcsButtonScroll->MoveWindow( lMovePosition, m_pcsButtonScroll->y, m_pcsButtonScroll->w, m_pcsButtonScroll->h );
	}

	if( pParent && bSendMessage )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_SCROLL_CHANGE_SCROLLVALUE, &m_lControlID );
	}

	return TRUE;
}

BOOL AcUIScroll::OnCommand( INT32 nID , PVOID pParam )
{
	// Control 중심의 Command Message
	if( m_pcsButtonScroll && *( INT32* )pParam == m_pcsButtonScroll->GetControlID() )
	{
		if( UICM_SCROLLBUTTON_MOVE == nID )
		{
			CalcScrollValue();
			return TRUE;
		}
		else if( UICM_BASE_MESSAGE_CLOSE == nID )
		{
			SetScrollButton( NULL );
		}
	}
	else if( m_pcsButtonUp && *( INT32* )pParam == m_pcsButtonUp->GetControlID() )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			SetScrollValue( m_fScrollValue - m_fScrollUnit );

			if( pParent )
			{
				pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_SCROLL_UPBUTTON_DOWN, &m_lControlID );
			}
		}
		else if( UICM_BASE_MESSAGE_CLOSE == nID )
		{
			SetScrollUpButton( NULL );
		}
	}
	else if( m_pcsButtonDown && *( INT32* )pParam == m_pcsButtonDown->GetControlID() )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			SetScrollValue( m_fScrollValue + m_fScrollUnit );
			if( pParent )
			{
				pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_SCROLL_DOWNBUTTON_DOWN, &m_lControlID );
			}
		}
		else if( UICM_BASE_MESSAGE_CLOSE == nID )
		{
			SetScrollDownButton( NULL );
		}
	}

	return AcUIBase::OnCommand( nID, pParam );
}

void AcUIScroll::OnMoveWindow( void )
{
	if( pParent && pParent->m_bUIWindow && ( ( AcUIBase* ) pParent )->m_nType == AcUIBase::TYPE_EDIT )
	{
		AcUIEdit* pcsEdit = ( AcUIEdit* )pParent;
		if( pcsEdit->GetVScroll() == this )
		{
			pcsEdit->OnMoveWindow();
		}
	}
}

BOOL AcUIScrollButton::OnMouseMove( RsMouseStatus *ms )
{
	// Window Move중이라면
	if( !m_bActiveStatus )
	{
		return AcUIBase::OnMouseMove( ms );
	}

	if( FALSE != m_bMoveMovableWindow )
	{
		ClientToScreen( *ms );
		
		INT32 lMovePosition	=	0;
		// Scroll Button 답게 Move ~ !
		if( m_bVScroll )	
		{
			lMovePosition = ( INT32 )( ms->pos.y - m_v2dDragPoint.y );
			if( m_bMoveByUnit && m_fScrollUnit )
			{
				FLOAT fUnitGap = ( m_lMaxPosition - m_lMinPosition ) * m_fScrollUnit;

				lMovePosition = lMovePosition - m_lMinPosition + INT32( fUnitGap / 2.0f );
				lMovePosition = INT32( ( ( INT32 )( lMovePosition / fUnitGap ) ) * fUnitGap + m_lMinPosition );
			}

			// Limit Check 
			if( m_lMaxPosition < lMovePosition ) lMovePosition = m_lMaxPosition;
			else if( m_lMinPosition > lMovePosition ) lMovePosition = m_lMinPosition;

			MoveWindow( x, lMovePosition, w, h );
		}
		else
		{
			lMovePosition = ( INT32 )( ms->pos.x - m_v2dDragPoint.x );

			if( m_bMoveByUnit && m_fScrollUnit )
			{
				FLOAT fUnitGap = ( m_lMaxPosition - m_lMinPosition ) * m_fScrollUnit;

				lMovePosition = lMovePosition - m_lMinPosition + INT32( fUnitGap / 2.0f );
				lMovePosition = INT32( ( ( INT32 )( lMovePosition / fUnitGap ) ) * fUnitGap + m_lMinPosition );
			}

			// Limit Check
			if( m_lMaxPosition < lMovePosition ) lMovePosition = m_lMaxPosition;
			else if( m_lMinPosition > lMovePosition ) lMovePosition = m_lMinPosition;

			MoveWindow( lMovePosition, y, w, h );
		}
		
		// Scroll에게 Move여부를 알린다 
		if( pParent )
		{
			pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_SCROLLBUTTON_MOVE, &m_lControlID );
		}
		
		return TRUE;
	}

	return FALSE;
}	
