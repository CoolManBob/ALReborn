#include "ApBase.h"
#include "AgcEngine.h"
#include "AgcmUIChatting.h"
#include "AgcmUIControl.h"

AgcmUIChatting::AgcmUIChatting()
{
	SetModuleName( "AgcmUIChatting" )	;
	
	m_pAgcmChatting		=	NULL		;
	m_pAgcmUIManager2	=	NULL		;
	
	m_bAddChatWindow	=	FALSE		;

	m_pcsUserDataMaxScrollPos		= NULL;
	m_pcsUserDataCurrentScrollPos	= NULL;

	m_lMaxScrollPos		= 0;
	m_lCurrentScrollPos	= 0;
}

AgcmUIChatting::~AgcmUIChatting()
{

}

BOOL	AgcmUIChatting::OnAddModule()
{
	m_pAgcmChatting		= (AgcmChatting*)GetModule( "AgcmChatting" )		;
	m_pAgcmUIManager2	= (AgcmUIManager2*)GetModule( "AgcmUIManager2" )	;

	if ( NULL == m_pAgcmChatting || NULL == m_pAgcmUIManager2 )
		return FALSE;

	m_pAgcmUIManager2->SetCallbackOpenMainUI( CBOepnMainUI, this )			;
	m_pAgcmUIManager2->SetCallbackCloseAllUI( CBCloseAllUIWindow, this )	;

	if (!m_pAgcmChatting->SetCallbackInputMessage(CBInputMessage, this))
		return FALSE;

	// add user data
	m_pcsUserDataMaxScrollPos		= m_pAgcmUIManager2->AddUserData("채팅_최대스크롤위치",
																   &m_lMaxScrollPos,
																   sizeof(INT32),
																   1,
																   AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataMaxScrollPos)
		return FALSE;

	m_pcsUserDataCurrentScrollPos	= m_pAgcmUIManager2->AddUserData("채팅_현재스크롤위치",
																   &m_lCurrentScrollPos,
																   sizeof(INT32),
																   1,
																   AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataCurrentScrollPos)
		return FALSE;

	
	// add display
	if (!m_pAgcmUIManager2->AddDisplay(this, "채팅_최대스크롤위치", 0, CBDisplayMaxScrollPos, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, "채팅_현재스크롤위치", 0, CBDisplayCurrentScrollPos, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	
	// Add Function
	if ( !m_pAgcmUIManager2->AddFunction( this, "Chat_NextTap", CBNextChatTab, 0 ))
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction( this, "Chat_ScrollUp", CBScrollUp, 0 ))
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction( this, "Chat_ScrollDown", CBScrollDown, 0 ))
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction( this, "Chat_ScrollHome", CBScrollHome, 0 ))
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction( this, "Chat_ScrollEnd", CBScrollEnd, 0 ))
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction( this, "Chat_ScrollMove", CBScrollMove, 0 ))
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction( this, "ChatMode_All", CBChatModeAll, 0 ))
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction( this, "ChatMode_Party", CBChatModeParty, 0 ))
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction( this, "ChatMode_Guild", CBChatModeGuild, 0 ))
		return FALSE;

	return TRUE;
}

BOOL	AgcmUIChatting::OnInit()
{
	m_pcsUI = m_pAgcmUIManager2->CreateUI();
	if (!m_pcsUI)
		return FALSE;

	strcpy(m_pcsUI->m_szUIName, "Chat");
	m_pcsUI->m_bMainUI = TRUE;

	if (!m_pAgcmUIManager2->AddUI(m_pcsUI))
		return FALSE;

	if (!m_pcsUI->m_pcsUIWindow->AddChild((AgcWindow*)&(m_pAgcmChatting->m_clChatWindow)))
		return FALSE;

	m_pAgcmChatting->m_clChatWindow.MoveWindow( AGCMUICHATTING_CHAT_WINDOW_X, 0, 
		ACUICHATTING_CHAT_WINDOW_WIDTH, ACUICHATTING_CHAT_WINDOW_HEIGHT );
	m_pAgcmChatting->m_clChatWindow.m_Property.bMovable		=	FALSE	;
	m_pAgcmChatting->m_clChatWindow.m_clProperty.bShrink	=	FALSE	;

	m_pcsUI->m_pcsUIWindow->SetHandler( AGCMUICHATTING_CHATTING_WINDOW_HANDLER_X + AGCMUICHATTING_CHAT_WINDOW_X,
										AGCMUICHATTING_CHATTING_WINDOW_HANDLER_Y,
										AGCMUICHATTING_CHATTING_WINDOW_HANDLER_W + AGCMUICHATTING_CHAT_WINDOW_X,
										AGCMUICHATTING_CHATTING_WINDOW_HANDLER_H);
	m_pcsUI->m_pcsUIWindow->m_Property.bMovable		=	FALSE	;
	m_pcsUI->m_pcsUIWindow->m_Property.bUseInput	=	FALSE	;
	m_pcsUI->m_pcsUIWindow->m_clProperty.bShrink	=	FALSE	;

	AgcdUIMode	eUIMode	= m_pAgcmUIManager2->GetUIMode();

	INT32	lYPosition	= 0;

	switch (eUIMode) {
	case AGCDUI_MODE_1024_768:
		{
			lYPosition	= AGCMUICHATTING_CHAT_WINDOW_Y;
		}
		break;

	case AGCDUI_MODE_1280_1024:
		{
			lYPosition	= AGCMUICHATTING_CHAT_WINDOW_Y + 1024 - 768;
		}
		break;

	case AGCDUI_MODE_1600_1200:
		{
			lYPosition	= AGCMUICHATTING_CHAT_WINDOW_Y + 1200 - 768;
		}
		break;
	}

	m_pcsUI->m_pcsUIWindow->MoveWindow( AGCMUICHATTING_CHAT_WINDOW_X, lYPosition, 
		ACUICHATTING_CHAT_WINDOW_WIDTH + AGCMUICHATTING_CHAT_WINDOW_X, ACUICHATTING_CHAT_WINDOW_HEIGHT  );

	return TRUE;
}

BOOL	AgcmUIChatting::OnDestroy()
{
	if (m_pcsUI->m_pcsUIWindow)
		m_pcsUI->m_pcsUIWindow->DeleteChild(&m_pAgcmChatting->m_clChatWindow);

	return TRUE;
}

BOOL	AgcmUIChatting::UIIniRead( CHAR* szName,BOOL bDecryption )
{
	AuIniManager	clIniManager;
	clIniManager.SetPath( szName );

	if ( FALSE == clIniManager.ReadFile(0, bDecryption) )
	{
		ASSERT( 0 && "AgcmUIChatting - UI Ini Read실패" );
		return FALSE;
	}

	INT32 lNumSection = clIniManager.GetNumSection();
	
	if ( lNumSection < 1 )
	{
		ASSERT( 0 && "AgcmUIChatting - UI Ini Read실패" );
		return FALSE;
	}	

	INT32 lNumKey = clIniManager.GetNumKeys( 0 );	//첫번째 Section의 Key Num 
	
	CHAR* szKeyName;
	CHAR* szKeyValue;

	for ( INT32 i = 0 ; i < lNumKey ; ++i )
	{
		szKeyName		=	clIniManager.GetKeyName( 0, i )		;
		szKeyValue		=	clIniManager.GetValue( 0, i )		;

		if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_MAIN))
		{
			m_pAgcmChatting->m_clChatWindow.AddImage( szKeyValue );				// 먼저 Add
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB0_0))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[0].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_NORMAL );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB0_1))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[0].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB0_2))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[0].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_CLICK );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB0_3))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[0].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_DISABLE );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB1_0))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[1].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_NORMAL );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB1_1))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[1].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB1_2))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[1].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_CLICK );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB1_3))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[1].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_DISABLE );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB2_0))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[2].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_NORMAL );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB2_1))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[2].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB2_2))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[2].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_CLICK );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB2_3))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[2].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_DISABLE );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB3_0))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[3].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_NORMAL );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB3_1))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[3].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB3_2))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[3].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_CLICK );
		}
		else if (!strcmp(szKeyName, AGCMUICHATTING_KEY_NAME_CHAT_TAB3_3))
		{
			m_pAgcmChatting->m_clChatWindow.m_clTabButton[3].AddButtonImage( szKeyValue, ACUIBUTTON_MODE_DISABLE );
		}
	}

	return TRUE;
}

/*****************************************************************
*   Function : CBOepnMainUI
*   Comment  : Main UI가 Open 되었을때
*   Date&Time : 2003-10-28, 오전 11:23
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL	AgcmUIChatting::CBOepnMainUI( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIChatting* pThis = (AgcmUIChatting*)pClass			;

	// Chatting Window 를 UIManager2 에다가 붙인다 
	if ( pThis->m_bAddChatWindow == FALSE )
	{
		if (!pThis->m_pAgcmUIManager2->OpenUI(pThis->m_pcsUI))
			pThis->m_bAddChatWindow		=	TRUE;
	}

	return TRUE;
}

/*****************************************************************
*   Function : CBCloseAllUIWindow
*   Comment  : 모든 UI가 닫힐때는 Chatting Window도 닫아주자
*   Date&Time : 2004-01-08, 오후 12:55
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL	AgcmUIChatting::CBCloseAllUIWindow( PVOID pData, PVOID pClass, PVOID pCustData			)
{
	AgcmUIChatting* pThis = (AgcmUIChatting*)pClass			;

	if ( pThis->m_bAddChatWindow )
	{
		pThis->m_pAgcmUIManager2->CloseUI(pThis->m_pcsUI);
	}

	return TRUE;
}

BOOL	AgcmUIChatting::CBNextChatTab( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting* pThis = (AgcmUIChatting*)pClass			;

	pThis->m_pAgcmChatting->m_clChatWindow.SetNextChatMode();

	return TRUE;
}

BOOL	AgcmUIChatting::CBInputMessage( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting	*pThis			= (AgcmUIChatting *)	pClass;

	pThis->RefreshScrollPos();

	return TRUE;
}

BOOL	AgcmUIChatting::CBScrollUp( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting *	pThis	= (AgcmUIChatting *)	pClass	;
	INT32				lControlID	= ACUICHATTING_CTRID_SCROLL		;

	//for (lIndex = 0; lIndex < ACUICHATTING_MAX_VISUAL_CHATTING_LINE; ++lIndex)
		pThis->m_pAgcmChatting->m_clChatWindow.SendMessage(AgcWindow::MESSAGE_COMMAND, (PVOID) UICM_ACUICHATSCROLL_UP_LINE, &lControlID);

	pThis->RefreshScrollPos();

	return TRUE;
}

BOOL	AgcmUIChatting::CBScrollDown( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting *	pThis = (AgcmUIChatting*)pClass			;
	INT32				lControlID	= ACUICHATTING_CTRID_SCROLL		;

	//for (lIndex = 0; lIndex < ACUICHATTING_MAX_VISUAL_CHATTING_LINE; ++lIndex)
		pThis->m_pAgcmChatting->m_clChatWindow.SendMessage(AgcWindow::MESSAGE_COMMAND, (PVOID) UICM_ACUICHATSCROLL_DOWN_LINE, &lControlID);

	pThis->RefreshScrollPos();

	return TRUE;
}

BOOL	AgcmUIChatting::CBScrollHome( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting *	pThis = (AgcmUIChatting*)pClass			;
	INT32				lControlID	= ACUICHATTING_CTRID_SCROLL		;

	pThis->m_pAgcmChatting->m_clChatWindow.m_clChatContent[pThis->m_pAgcmChatting->m_clChatWindow.m_lChatMode].m_clScroll.m_fScrollValue = 0.0f;
	pThis->m_pAgcmChatting->m_clChatWindow.SendMessage(AgcWindow::MESSAGE_COMMAND, (PVOID) UICM_ACUICHATSCROLL_CHANGE, (PVOID)&lControlID);

	pThis->RefreshScrollPos();

	return TRUE;
}

BOOL	AgcmUIChatting::CBScrollEnd( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting *	pThis = (AgcmUIChatting*)pClass			;
	INT32				lControlID	= ACUICHATTING_CTRID_SCROLL		;

	pThis->m_pAgcmChatting->m_clChatWindow.m_clChatContent[pThis->m_pAgcmChatting->m_clChatWindow.m_lChatMode].m_clScroll.m_fScrollValue = 1.0f;
	pThis->m_pAgcmChatting->m_clChatWindow.SendMessage(AgcWindow::MESSAGE_COMMAND, (PVOID) UICM_ACUICHATSCROLL_CHANGE, (PVOID)&lControlID);

	pThis->RefreshScrollPos();

	return TRUE;
}

BOOL	AgcmUIChatting::CBScrollMove( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIChatting *	pThis = (AgcmUIChatting*)pClass			;
	INT32				lControlID	= ACUICHATTING_CTRID_SCROLL		;

	AcUIScroll *		pcsScrollControl	= (AcUIScroll *) pcsSourceControl->m_pcsBase;

	pThis->m_pAgcmChatting->m_clChatWindow.m_clChatContent[pThis->m_pAgcmChatting->m_clChatWindow.m_lChatMode].m_clScroll.m_fScrollValue = pcsScrollControl->GetScrollValue();
	pThis->m_pAgcmChatting->m_clChatWindow.SendMessage(AgcWindow::MESSAGE_COMMAND, (PVOID) UICM_ACUICHATSCROLL_CHANGE, (PVOID)&lControlID);

	pThis->RefreshScrollPos();

	return TRUE;
}

BOOL	AgcmUIChatting::CBChatModeAll( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting*		pThis			= (AgcmUIChatting *)		pClass;

	INT32				lControlID		= ACUICHATTING_CTRID_TAB_BUTTON_0;

	pThis->m_pAgcmChatting->m_clChatWindow.SendMessage(AgcWindow::MESSAGE_COMMAND, (PVOID) UICM_BUTTON_MESSAGE_CLICK, (PVOID) &lControlID);

	pThis->RefreshScrollPos();

	return TRUE;
}

BOOL	AgcmUIChatting::CBChatModeParty( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting*		pThis			= (AgcmUIChatting *)		pClass;

	INT32				lControlID		= ACUICHATTING_CTRID_TAB_BUTTON_1;

	pThis->m_pAgcmChatting->m_clChatWindow.SendMessage(AgcWindow::MESSAGE_COMMAND, (PVOID) UICM_BUTTON_MESSAGE_CLICK, (PVOID) &lControlID);

	pThis->RefreshScrollPos();

	return TRUE;
}

BOOL	AgcmUIChatting::CBChatModeGuild( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting*		pThis			= (AgcmUIChatting *)		pClass;

	INT32				lControlID		= ACUICHATTING_CTRID_TAB_BUTTON_2;

	pThis->m_pAgcmChatting->m_clChatWindow.SendMessage(AgcWindow::MESSAGE_COMMAND, (PVOID) UICM_BUTTON_MESSAGE_CLICK, (PVOID) &lControlID);

	pThis->RefreshScrollPos();

	return TRUE;
}

BOOL	AgcmUIChatting::CBDisplayMaxScrollPos(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !plValue)
		return FALSE;

	AgcmUIChatting		*pThis			= (AgcmUIChatting *)		pClass;

	*plValue	= pThis->m_lMaxScrollPos;

	return TRUE;
}

BOOL	AgcmUIChatting::CBDisplayCurrentScrollPos(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !plValue)
		return FALSE;

	AgcmUIChatting		*pThis			= (AgcmUIChatting *)		pClass;

	*plValue	= pThis->m_lCurrentScrollPos;

	return TRUE;
}

BOOL	AgcmUIChatting::RefreshScrollPos()
{
	m_lMaxScrollPos		= m_pAgcmChatting->m_clChatWindow.m_clChatContent[m_pAgcmChatting->m_clChatWindow.m_lChatMode].GetNumContent();

	if (m_lMaxScrollPos >= ACUICHATTING_MAX_VISUAL_CHATTING_LINE)
	{
		m_lCurrentScrollPos	= m_lMaxScrollPos - (m_pAgcmChatting->m_clChatWindow.m_clChatContent[m_pAgcmChatting->m_clChatWindow.m_lChatMode].m_lChatNowLine
																- m_pAgcmChatting->m_clChatWindow.m_clChatContent[m_pAgcmChatting->m_clChatWindow.m_lChatMode].m_lChatWriteLine);

		if (m_lCurrentScrollPos < 0)
			m_lCurrentScrollPos += ACUICHATTING_MAX_CHATTING_LINE;
		else if (m_lCurrentScrollPos >= ACUICHATTING_MAX_CHATTING_LINE)
			m_lCurrentScrollPos -= ACUICHATTING_MAX_CHATTING_LINE;

		m_lMaxScrollPos		-= (ACUICHATTING_MAX_VISUAL_CHATTING_LINE - 1);
		m_lCurrentScrollPos	-= (ACUICHATTING_MAX_VISUAL_CHATTING_LINE - 1);
	}
	else
	{
		m_lCurrentScrollPos	= m_lMaxScrollPos;
	}

	m_pAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataCurrentScrollPos);

	return TRUE;
}