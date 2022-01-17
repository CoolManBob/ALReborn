#include "AcUIRecruit.h"
#include "AgcmRecruit.h"

#include "AuStrTable.h"

/*****************************************************************
*   Function  : AcUIRecruit
*   Comment   : constructor of AcUIRecruit
*   Date&Time : 2003-04-24 오후 4:22:34
*   Code By   : Seong Yon-jun@NHN Studio 
*****************************************************************/
AcUIRecruit::AcUIRecruit()
{
		m_pAgcmRecruit		=		NULL		;
		
		m_bBBSMode			=		0			;
		m_lSelectedClass	=		0x00000000	;
		m_lMinLevel			=		0			;
		m_lMaxLevel			=		0			;
}

/*****************************************************************
*   Function  : ~AcUIRecruit
*   Comment   : destructor of AcUIRectruit
*   Date&Time : 2003-04-24 오후 4:22:37
*   Code By   : Seong Yon-jun@NHN Studio 
*****************************************************************/
AcUIRecruit::~AcUIRecruit()
{

}

/*****************************************************************
*   Function  : SetBbsMode
*   Comment   : SetBbsMode
*   Date&Time : 2003-04-24 오후 4:22:48
*   Code By   : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcUIRecruit::SetBbsMode( BOOL	bMode	)
{
	// Mercenary
	if ( 0 == bMode )
	{
		m_clBbsMercenary.ShowWindow( TRUE );
		m_clBbsParty.ShowWindow( FALSE );
		m_clTabMercenary.SetButtonMode( 2 );
		m_clTabParty.SetButtonMode( 0 );
	}
	// Party
	else
	{
		m_clBbsMercenary.ShowWindow( FALSE );
		m_clBbsParty.ShowWindow( TRUE );
		m_clTabMercenary.SetButtonMode( 0 );
		m_clTabParty.SetButtonMode( 2 );
	}
	m_bBBSMode = bMode	;
}		

/*****************************************************************
*   Function : 
*   Comment  : 
*   Date&Time : 2003-04-24, 오후 4:27
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AcUIRecruit::OnCommand		( INT32	nID , PVOID pParam	)
{
	INT32* pUIID = (INT32*)pParam;
	//stAgcUIBbsCommandMessageInfo* pCMInfo = (stAgcUIBbsCommandMessageInfo*)pParam;

	// Control 중심의 Command Message
	switch( *pUIID )
	{
		case ACUIRECRUIT_CTRID_CLOSE:
			if ( UICM_BUTTON_MESSAGE_CLICK == nID )
			{	
				EndDialog( 0 );
				return TRUE;
			}
			break;

		case ACUIRECRUIT_CTRID_FIND1:
			if ( UICM_BUTTON_MESSAGE_CLICK == nID )
			{
				if ( NULL != m_pAgcmRecruit )
				{
					GetClassSelectInfo();

					if ( 0 == m_bBBSMode )	m_pAgcmRecruit->UILFPSearch( false, 1, m_lSelectedClass );
					else					m_pAgcmRecruit->UILFMSearch( false, 1, m_lSelectedClass );
				}
				return TRUE;
			}
			break;

		case ACUIRECRUIT_CTRID_FIND2:
			if ( UICM_BUTTON_MESSAGE_CLICK == nID )
			{
				if ( NULL != m_pAgcmRecruit )
				{
					GetClassSelectInfo();

					if ( 0 == m_bBBSMode )	m_pAgcmRecruit->UILFPSearch( true, 1, m_lSelectedClass );
					else					m_pAgcmRecruit->UILFMSearch( true, 1, m_lSelectedClass );
				}
				return TRUE;
			}
			break;
			
		case ACUIRECRUIT_CTRID_TAB1:
			if ( UICM_BUTTON_MESSAGE_PUSHDOWN == nID )
			{
				SetBbsMode( 0 );
				return TRUE;
			}
			else if ( UICM_BUTTON_MESSAGE_PUSHUP == nID )
			{
				SetBbsMode( 0 );
				return TRUE;
			}
			break;

		case ACUIRECRUIT_CTRID_TAB2:
			if ( UICM_BUTTON_MESSAGE_PUSHDOWN == nID )
			{
				SetBbsMode( 1 );
				return TRUE;
			}
			else if ( UICM_BUTTON_MESSAGE_PUSHUP == nID )
			{		
				SetBbsMode( 1 );
				return TRUE;
			}
			break;
			
		case ACUIRECRUIT_CTRID_BBS1:		// Mercenary
			
			if ( UICM_BBS_CHANGE_PAGE == nID )
			{
				if ( NULL != m_pAgcmRecruit )
						m_pAgcmRecruit->UILFPPageChange( m_clBbsMercenary.m_stCommandMessageInfo.lSelectedIndex );
				return TRUE;
			}
			else if ( UICM_BBS_CLICK_CONTENT == nID )
			{
				return TRUE;
			}
			
			break;

		case ACUIRECRUIT_CTRID_BBS2:

			if ( UICM_BBS_CHANGE_PAGE == nID )
			{
				if ( NULL != m_pAgcmRecruit )
						m_pAgcmRecruit->UILFMPageChange( m_clBbsParty.m_stCommandMessageInfo.lSelectedIndex );
				return TRUE;
			}
			else if ( UICM_BBS_CLICK_CONTENT == nID )
			{
				return TRUE;
			}

			break;
	}

	return TRUE;
}

/*****************************************************************
*   Function : GetClassSelectInfo
*   Comment  : Check Box 로 부터 Select상황을 알아낸다
*   Date&Time : 2003-04-28, 오후 4:24
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcUIRecruit::GetClassSelectInfo()	
{
	m_lSelectedClass = 0x00000000;

	if ( 2 == m_clClassSelect[0].GetButtonMode() )// 전사
		m_lSelectedClass = m_lSelectedClass | AGPMRECRUIT_CLASS_FIGHTER;	
	
	if ( 2 == m_clClassSelect[1].GetButtonMode() )					// 궁수
		m_lSelectedClass = m_lSelectedClass | AGPMRECRUIT_CLASS_RANGER;
	
	if ( 2 == m_clClassSelect[2].GetButtonMode() )					// 수도승
		m_lSelectedClass = m_lSelectedClass | AGPMRECRUIT_CLASS_MONK;
	
	if ( 2 == m_clClassSelect[3].GetButtonMode() )					// 마법사 
		m_lSelectedClass = m_lSelectedClass | AGPMRECRUIT_CLASS_MAGE;

/*	if ( FALSE != m_clClassCheckbox[0].GetCheckedValue() )					// 전사 
		m_lSelectedClass = m_lSelectedClass | AGPMRECRUIT_CLASS_FIGHTER;	
	
	if ( FALSE != m_clClassCheckbox[1].GetCheckedValue() )					// 궁수
		m_lSelectedClass = m_lSelectedClass | AGPMRECRUIT_CLASS_RANGER;
	
	if ( FALSE != m_clClassCheckbox[2].GetCheckedValue() )					// 수도승
		m_lSelectedClass = m_lSelectedClass | AGPMRECRUIT_CLASS_MONK;
	
	if ( FALSE != m_clClassCheckbox[3].GetCheckedValue() )					// 마법사 
		m_lSelectedClass = m_lSelectedClass | AGPMRECRUIT_CLASS_MAGE;		*/
}

/*****************************************************************
*   Function : ChildControlInit
*   Comment  : Child Control Init
*   Date&Time : 2003-04-28, 오후 4:56
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AcUIRecruit::ChildControlInit()	
{
	// Tab 버튼 초기화
	AddChild( &m_clTabMercenary, ACUIRECRUIT_CTRID_TAB1 );
	m_clTabMercenary.MoveWindow( AGCUIRECRUIT_TAB1_X, AGCUIRECRUIT_TAB1_Y, 
		AGCUIRECRUIT_TAB_WIDTH, AGCUIRECRUIT_TAB_HEIGHT );
	m_clTabMercenary.SetButtonMode( 2 );

	// Tab 버튼 초기화 
	AddChild( &m_clTabParty, ACUIRECRUIT_CTRID_TAB2 );
	m_clTabParty.MoveWindow( AGCUIRECRUIT_TAB2_X, AGCUIRECRUIT_TAB2_Y, 
		AGCUIRECRUIT_TAB_WIDTH, AGCUIRECRUIT_TAB_HEIGHT );
	m_clTabParty.SetButtonMode( 0 );

	AddChild( &m_clButtonFind, ACUIRECRUIT_CTRID_FIND1 );
	m_clButtonFind.MoveWindow( AGCUIRECRUIT_FIND_BUTTON_X, AGCUIRECRUIT_FIND_BUTTON_Y,
		AGCUIRECRUIT_BUTTON_WIDTH, AGCUIRECRUIT_BUTTON_HEIGHT );
	
	AddChild( &m_clButtonMyLevelFind, ACUIRECRUIT_CTRID_FIND2 );
	m_clButtonMyLevelFind.MoveWindow( AGCUIRECRUIT_MY_LEVEL_FIND_BUTTON_X, AGCUIRECRUIT_MY_LEVEL_FIND_BUTTON_Y,
		AGCUIRECRUIT_BUTTON_WIDTH, AGCUIRECRUIT_BUTTON_HEIGHT );

	// BBS 초기화 
	AddChild( &m_clBbsMercenary, ACUIRECRUIT_CTRID_BBS1 );
	m_clBbsMercenary.MoveWindow( AGCUIRECRUIT_BBS_X, AGCUIRECRUIT_BBS_Y, 
		AGCUIRECRUIT_BBS_WIDTH, AGCUIRECRUIT_BBS_HEIGHT );
	m_clBbsMercenary.ShowWindow( TRUE );
	
	AddChild( &m_clBbsParty, ACUIRECRUIT_CTRID_BBS2 );
	m_clBbsParty.MoveWindow( AGCUIRECRUIT_BBS_X, AGCUIRECRUIT_BBS_Y, 
		AGCUIRECRUIT_BBS_WIDTH, AGCUIRECRUIT_BBS_HEIGHT );
	m_clBbsParty.ShowWindow( FALSE );

	//게시판 속성 입력 
	stAcUIBbsInfo stBbsInfo;
	stBbsInfo.m_cColumnNum = 3;
	stBbsInfo.m_cRowNum = 7;
	stBbsInfo.m_nCellHeight = 17;
	stBbsInfo.m_nColumnGap = 10;
	stBbsInfo.m_nGapColumnCell = 20;
	stBbsInfo.m_nNowPageNum = 1;
	stBbsInfo.m_nAllPageNum = 1;
	stBbsInfo.m_v2dCellStart.x = 15.0f;
	stBbsInfo.m_v2dCellStart.y = 8.0f;
	m_clBbsMercenary.InitInfoInput( &stBbsInfo );
		
	//게시판 컬럼 정보 입력 
	m_clBbsMercenary.SetColumnInfo( 0, ClientStr().GetStr(STI_NAME), 145, HANFONT_CENTERARRANGE );
	m_clBbsMercenary.SetColumnInfo( 1, ClientStr().GetStr(STI_LEVEL), 60, HANFONT_CENTERARRANGE );
	m_clBbsMercenary.SetColumnInfo( 2, ClientStr().GetStr(STI_CLASS), 80 );
	stBbsInfo.m_cColumnNum = 3;
	m_clBbsParty.InitInfoInput( &stBbsInfo );
	m_clBbsParty.SetColumnInfo( 0, ClientStr().GetStr(STI_LEADER_NAME), 65, HANFONT_RIGHTARRANGE );
	m_clBbsParty.SetColumnInfo( 1, ClientStr().GetStr(STI_NEED_NUMBER), 65, HANFONT_RIGHTARRANGE );
	m_clBbsParty.SetColumnInfo( 2, ClientStr().GetStr(STI_NEED_LEVEL), 65 , HANFONT_CENTERARRANGE );
	//m_clBbsParty.SetColumnInfo( 3, "필요작업", 65 , HANFONT_CENTERARRANGE );

	// Close Button 초기화 
	AddChild( &m_clButtonClose, ACUIRECRUIT_CTRID_CLOSE );
	m_clButtonClose.MoveWindow( AGCUIRECRUIT_CLOSE_BUTTON_X, AGCUIRECRUIT_CLOSE_BUTTON_Y,
		AGCUIRECRUIT_CLOSE_BUTTON_WIDTH, AGCUIRECRUIT_CLOSE_BUTTON_HEIGHT );

	// Check Box 초기화 
	for ( int i = 0 ; i < ACUIRECRUIT_FIND_CLASS_NUM ; ++i )
	{
		AddChild( &m_clClassSelect[i] );
		m_clClassSelect[i].MoveWindow( AGCUIRECRUIT_SELECTBUTTON_START_X, 
			AGCUIRECRUIT_SELECTBUTTON_START_Y + i * AGCUIRECRUIT_SELECTBUTTON_GAP_Y,
			AGCUIRECRUIT_SELECTBUTTON_WIDTH, AGCUIRECRUIT_SELECTBUTTON_HEIGHT );
	}

	// Edit 초기화
//	m_clMinLevel.MoveWindow( AGCUIRECRUIT_EDITMINLEVEL_X, AGCUIRECRUIT_EDITMINLEVEL_Y, 
//		AGCUIRECRUIT_EDIT_WIDTH, AGCUIRECRUIT_EDIT_HEIGHT );
//	AddChild( &m_clMinLevel );

//	m_clMaxLevel.MoveWindow( AGCUIRECRUIT_EDITMAXLEVEL_X, AGCUIRECRUIT_EDITMAXLEVEL_Y, 
//		AGCUIRECRUIT_EDIT_WIDTH, AGCUIRECRUIT_EDIT_HEIGHT );
//	AddChild( &m_clMaxLevel );

	return TRUE;
}

