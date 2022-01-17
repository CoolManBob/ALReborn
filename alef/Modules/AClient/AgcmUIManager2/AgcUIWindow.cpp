// AgcUIWindow.cpp: implementation of the AgcUIWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcUIWindow.h"
#include "AgcmUIManager2.h"
#include "AgcmUIOption.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcUIWindow::AgcUIWindow( AgcmUIManager2 *pcsAgcmUIManager2, AgcdUI *pcsUI )
{
	m_pcsAgcmUIManager2 = pcsAgcmUIManager2;
	m_pcsUI = pcsUI;

	SetRenderDefaultImage( FALSE );
}

void AgcUIWindow::OnMoveWindow( void )
{
	AuNode< AgcdUI* >* pcsNode;
	AgcdUI* pcsUI;
	
	//@{ 2006/09/12 burumal
#ifndef USE_MFC
	if( g_pEngine && GetUIDepth( m_pcsUI ) <= 1 )
	{
		AgcmUIOption* pAgcmUIOption = ( AgcmUIOption* ) g_pEngine->GetModule( "AgcmUIOption" );
		if( pAgcmUIOption )
		{
			#define nMINIMUM_UI_BASE_MARGIN	10

			if( x > ( ( INT32 )pAgcmUIOption->m_iScreenWidth - nMINIMUM_UI_BASE_MARGIN ) )
			{
				this->x = ( ( INT32 )pAgcmUIOption->m_iScreenWidth - nMINIMUM_UI_BASE_MARGIN );
			}

			if( ( this->w >= nMINIMUM_UI_BASE_MARGIN ) && x < -this->w + nMINIMUM_UI_BASE_MARGIN )
			{
				this->x = -this->w + nMINIMUM_UI_BASE_MARGIN;
			}

			if( y > ( ( INT32 )pAgcmUIOption->m_iScreenHeight - nMINIMUM_UI_BASE_MARGIN ) )
			{
				this->y = ( ( INT32 )pAgcmUIOption->m_iScreenHeight - nMINIMUM_UI_BASE_MARGIN );
			}

			if( y < 0 )
			{
				this->y = 0;
			}
		}
	}
#endif
	//@}

	if( m_pcsUI->m_pcsUIAttach )
	{
		INT32 lX = x;
		INT32 lY = y;

		switch( m_pcsUI->m_eAttachType )
		{
		case AGCDUI_ATTACH_TYPE_LEFT :		lX = x + w;												break;
		case AGCDUI_ATTACH_TYPE_RIGHT :		lX = x - m_pcsUI->m_pcsUIAttach->m_pcsUIWindow->w;		break;
		case AGCDUI_ATTACH_TYPE_TOP :		lY = y + h;												break;
		case AGCDUI_ATTACH_TYPE_BOTTOM :	lY = y - m_pcsUI->m_pcsUIAttach->m_pcsUIWindow->h;		break;
		}

		if( lX != m_pcsUI->m_pcsUIAttach->m_pcsUIWindow->x || lY != m_pcsUI->m_pcsUIAttach->m_pcsUIWindow->y )
		{
			m_pcsUI->m_pcsUIAttach->m_pcsUIWindow->MoveWindow( lX, lY, m_pcsUI->m_pcsUIAttach->m_pcsUIWindow->w, m_pcsUI->m_pcsUIAttach->m_pcsUIWindow->h );
			return;
		}
	}

	pcsNode = m_pcsUI->m_listAttached.GetHeadNode();
	while( pcsNode )
	{
		pcsUI = pcsNode->GetData();

		switch( pcsUI->m_eAttachType )
		{
		case AGCDUI_ATTACH_TYPE_LEFT :
			pcsUI->m_pcsUIWindow->MoveWindow(	m_pcsUI->m_pcsUIWindow->x - pcsUI->m_pcsUIWindow->w,
												m_pcsUI->m_pcsUIWindow->y, 
												pcsUI->m_pcsUIWindow->w, 
												pcsUI->m_pcsUIWindow->h );
			break;

		case AGCDUI_ATTACH_TYPE_RIGHT :
			pcsUI->m_pcsUIWindow->MoveWindow(	m_pcsUI->m_pcsUIWindow->x + m_pcsUI->m_pcsUIWindow->w,
												m_pcsUI->m_pcsUIWindow->y,
												pcsUI->m_pcsUIWindow->w, 
												pcsUI->m_pcsUIWindow->h );
			break;

		case AGCDUI_ATTACH_TYPE_TOP :
			pcsUI->m_pcsUIWindow->MoveWindow(	m_pcsUI->m_pcsUIWindow->x, 
												m_pcsUI->m_pcsUIWindow->y - pcsUI->m_pcsUIWindow->h,
												pcsUI->m_pcsUIWindow->w, 
												pcsUI->m_pcsUIWindow->h );
			break;

		case AGCDUI_ATTACH_TYPE_BOTTOM :
			pcsUI->m_pcsUIWindow->MoveWindow(	m_pcsUI->m_pcsUIWindow->x, 
												m_pcsUI->m_pcsUIWindow->y + m_pcsUI->m_pcsUIWindow->h,
												pcsUI->m_pcsUIWindow->w, 
												pcsUI->m_pcsUIWindow->h );
			break;
		}

		pcsNode = pcsNode->GetNextNode();
	}

	if( m_pcsUI->m_pcsTooltipAttach )
	{
		switch( m_pcsUI->m_eAttachType )
		{
		case AGCDUI_ATTACH_TYPE_LEFT :
			m_pcsUI->m_pcsTooltipAttach->MoveWindow(	m_pcsUI->m_pcsUIWindow->x - m_pcsUI->m_pcsTooltipAttach->w, 
														m_pcsUI->m_pcsUIWindow->y,
														pcsUI->m_pcsUIWindow->w, 
														pcsUI->m_pcsUIWindow->h );
			break;

		case AGCDUI_ATTACH_TYPE_RIGHT :
			m_pcsUI->m_pcsTooltipAttach->MoveWindow(	m_pcsUI->m_pcsUIWindow->x + m_pcsUI->m_pcsUIWindow->w, 
														m_pcsUI->m_pcsUIWindow->y,
														pcsUI->m_pcsUIWindow->w, 
														pcsUI->m_pcsUIWindow->h );
			break;

		case AGCDUI_ATTACH_TYPE_TOP :
			m_pcsUI->m_pcsTooltipAttach->MoveWindow(	m_pcsUI->m_pcsUIWindow->x, 
														m_pcsUI->m_pcsUIWindow->y - m_pcsUI->m_pcsTooltipAttach->h,
														pcsUI->m_pcsUIWindow->w, 
														pcsUI->m_pcsUIWindow->h );
			break;

		case AGCDUI_ATTACH_TYPE_BOTTOM :
			m_pcsUI->m_pcsTooltipAttach->MoveWindow(	m_pcsUI->m_pcsUIWindow->x, 
														m_pcsUI->m_pcsUIWindow->y + m_pcsUI->m_pcsUIWindow->h,
														pcsUI->m_pcsUIWindow->w, 
														pcsUI->m_pcsUIWindow->h );
			break;
		}
	}

	m_pcsUI->m_alX[ m_pcsAgcmUIManager2->GetUIMode() ] = x;
	m_pcsUI->m_alY[ m_pcsAgcmUIManager2->GetUIMode() ] = y;
}

BOOL AgcUIWindow::OnLButtonDown( RsMouseStatus *ms	)
{
	// popup UI°¡ ÀÖÀ¸¸é ´ÝÀÚ.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup && m_pcsUI != m_pcsAgcmUIManager2->m_pcsUIOpenedPopup )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}

	if( m_bActiveStatus && m_pcsUI->m_bTransparent ) return FALSE;
	return AcUIBase::OnLButtonDown( ms );
}

BOOL AgcUIWindow::OnRButtonDown( RsMouseStatus *ms	)
{
	// popup UI°¡ ÀÖÀ¸¸é ´ÝÀÚ.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup && m_pcsUI != m_pcsAgcmUIManager2->m_pcsUIOpenedPopup )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}

	if( m_bActiveStatus && m_pcsUI->m_bTransparent ) return FALSE;

	m_bRButtonDown = TRUE;
	AcUIBase::OnRButtonDown( ms );
	return TRUE;
}

BOOL AgcUIWindow::OnRButtonUp( RsMouseStatus *ms )
{
	if( m_bActiveStatus && m_pcsUI->m_bTransparent ) return FALSE;

	m_bRButtonDown = FALSE;
	m_pcsAgcmUIManager2->m_csManagerWindow.OnRButtonUp( ms );
	AcUIBase::OnRButtonUp( ms );
	return FALSE;
}

BOOL AgcUIWindow::OnLButtonDblClk( RsMouseStatus *ms )
{
	// popup UI°¡ ÀÖÀ¸¸é ´ÝÀÚ.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup && m_pcsUI != m_pcsAgcmUIManager2->m_pcsUIOpenedPopup )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}

	if( m_bActiveStatus && m_pcsUI->m_bTransparent ) return FALSE;
	return TRUE;
}

BOOL AgcUIWindow::OnRButtonDblClk( RsMouseStatus *ms )
{
	// popup UI°¡ ÀÖÀ¸¸é ´ÝÀÚ.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup && m_pcsUI != m_pcsAgcmUIManager2->m_pcsUIOpenedPopup )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}

	if( m_bActiveStatus && m_pcsUI->m_bTransparent ) return FALSE;
	return TRUE;
}

BOOL AgcUIWindow::OnMouseMove( RsMouseStatus *ms )
{
	if( m_bRButtonDown && !m_pcsAgcmUIManager2->m_csManagerWindow.m_bRButtonClicked )
	{
		m_bRButtonDown = FALSE;
	}

	AcUIBase::OnMouseMove( ms );
	if( m_pcsAgcmUIManager2->m_csManagerWindow.m_bCameraMoveMode && !m_bRButtonDown ) return FALSE;
	return FALSE;
}

BOOL AgcUIWindow::OnKeyDown( RsKeyStatus *ks )
{
	switch( ks->keyCharCode )
	{
	case rsESC :		return m_pcsAgcmUIManager2->m_csManagerWindow.OnKeyDown( ks );			break;
	}

	return FALSE;
}

BOOL AgcUIWindow::OnKeyUp( RsKeyStatus *ks )
{
	switch( ks->keyCharCode )
	{
	case rsESC :		return m_pcsAgcmUIManager2->m_csManagerWindow.OnKeyUp( ks );			break;
	}

	return FALSE;
}

BOOL AgcUIWindow::OnCommand( INT32 nID, PVOID pParam )
{
	PROFILE( "AgcUIWindow::OnCommand()" );

	INT32 lIndex = *( INT32* )pParam;
	AcUIBase* pcsBase = *( AcUIBase** )pParam;

	// popup UI°¡ ÀÖÀ¸¸é ´ÝÀÚ.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup && m_pcsUI != m_pcsAgcmUIManager2->m_pcsUIOpenedPopup &&
		( nID == UICM_BASE_MESSAGE_LCLICK || nID == UICM_BASE_MESSAGE_RCLICK ||
		  nID == UICM_BASE_MESSAGE_LDBLCLICK || nID == UICM_BASE_MESSAGE_RDBLCLICK ) )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}

	AgcdUIControl* pcsControl = m_pcsAgcmUIManager2->GetControl( m_pcsUI, lIndex );
	if( !pcsControl || pcsControl->m_lType == AcUIBase::TYPE_NONE ) return FALSE;
	if( nID < 0 || nID >= AgcmUIManager2::m_alControlMessages[ pcsControl->m_pcsBase->m_nType ] ) return FALSE;

	AgpdGridItem* pcsGridItem = NULL;
	if( pcsBase->m_nType == AcUIBase::TYPE_GRID_ITEM )
	{
		pcsGridItem = ( ( AcUIGridItem* )pcsBase )->m_ppdGridItem;
	}

	switch( nID )
	{
	case UICM_GRIDITEM_SET_FOCUS :
		{
			if( pcsBase->m_nType == AcUIBase::TYPE_GRID_ITEM && pcsGridItem )
			{
				if( pcsControl )
				{
					m_pcsAgcmUIManager2->OpenTooltip( pcsGridItem->m_szTooltip, pcsBase, pcsControl->m_pcsParentUI->m_pcsUIWindow );
				}
				else
				{
					m_pcsAgcmUIManager2->OpenTooltip( pcsGridItem->m_szTooltip, pcsBase );
				}
			}
		}
		break;

	case UICM_GRIDITEM_KILL_FOCUS :
		{
			if( pcsGridItem )
			{
				m_pcsAgcmUIManager2->CloseTooltip();
			}
		}
		break;

	case UICM_BASE_MESSAGE_SET_FOCUS :
		{
			if( pcsBase->m_szTooltip )
			{
				if( pcsControl )
				{
					m_pcsAgcmUIManager2->OpenTooltip( pcsBase->m_szTooltip, pcsBase, pcsControl->m_pcsParentUI->m_pcsUIWindow );
					m_pcsAgcmUIManager2->OpenTooltip( pcsControl->m_szTooltip, pcsControl->m_pcsBase, pcsControl->m_pcsParentUI->m_pcsUIWindow );
				}
				else
				{
					m_pcsAgcmUIManager2->OpenTooltip( pcsBase->m_szTooltip, pcsBase );
					m_pcsAgcmUIManager2->OpenTooltip( pcsControl->m_szTooltip, pcsControl->m_pcsBase );
				}
			}
		}
		break;

	case UICM_BASE_MESSAGE_KILL_FOCUS :
		{
			if( pcsBase->m_szTooltip )
			{
				m_pcsAgcmUIManager2->CloseTooltip();
			}
		}
		break;

	case UICM_BUTTON_MESSAGE_PUSHDOWN :
		{
			if( pcsControl->m_lType == AcUIBase::TYPE_BUTTON && pcsControl->m_uoData.m_stButton.m_pstCheckUserData )
			{
				pcsControl->m_uoData.m_stButton.m_pstCheckUserData->m_lSelectedIndex = pcsControl->m_lUserDataIndex;
			}
		}
		break;

	case UICM_COMBO_MESSAGE_SELECT_CHANGED :
		{
			if( pcsControl->m_pstUserData && pcsControl->m_lType == AcUIBase::TYPE_COMBO )
			{
				pcsControl->m_pstUserData->m_lSelectedIndex = ( ( AcUICombo* )pcsControl->m_pcsBase )->GetSelectedIndex();
			}
		}
		break;
	}

	AgcdUIMessageMap* pstMessageMap = pcsControl->m_pstMessageMaps + nID;	
	return m_pcsAgcmUIManager2->ProcessAction( &pstMessageMap->m_listAction, pcsControl, NULL, pcsControl->m_lUserDataIndex );
}

BOOL AgcUIWindow::PreTranslateInputMessage( RsEvent event, PVOID param )
{
	switch( event )
	{
	case rsMOUSEMOVE :
		{
			if( m_Property.bUseInput )
			{
				m_pcsAgcmUIManager2->SetMouseOnUI( m_pcsUI );
			}
		}
		break;
	}

	return AcUIBase::PreTranslateInputMessage( event, param );
}

//@{ 2006/09/14 burumal
INT32 AgcUIWindow::GetUIDepth( AgcdUI* pUI )
{
	INT32 nDepth = 0;
	if( !pUI ) return nDepth;

	GetUIDepthRecurse( pUI, &nDepth );
	return nDepth;
}

void AgcUIWindow::GetUIDepthRecurse( AgcdUI* pCurUI, INT32* pDepth )
{
	ASSERT( pCurUI );
	( *pDepth )++;

	if( pCurUI->m_pcsParentUI )
	{
		GetUIDepthRecurse( pCurUI->m_pcsParentUI, pDepth );
	}
}
//@}