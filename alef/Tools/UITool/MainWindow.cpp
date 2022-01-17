// MainWindow.cpp: implementation of the MainWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"

#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rpcollis.h"
#include "rtpick.h"
#include "rtcharse.h"

#include <skeleton.h>
#include <menu.h>
//#include "events.h"
#include <camera.h>
#include <MagDebug.h>
#include "MyEngine.h"
#include "MainWindow.h"
#include <win.h>

#include "MyEngine.h"

#include "MainFrm.h"

#include "UITCustomControlDialog.h"

#define INITIAL_CONTROL_X	20
#define INITIAL_CONTROL_Y	20
#define CONTROL_SPAWN_DIFF	20

#define SELECTED_BOX_SIZE	5
#define SELECTED_BOX_COLOR	0x00777777
#define GRID_ITEM_COLOR		0x00000055
#define AREA_LINE_COLOR		0x00000000
#define HANDLER_COLOR		0x00000080

CHAR *g_aszUIMode[AGCDUI_MAX_MODE] =
{
	"1024x768",
	"1280x1024",
	"1600x1200"
};

CHAR *g_szArgumentNULL = "<NULL>";
CHAR *g_aszUIActionType[AGCDUI_MAX_ACTION] = 
{
	"None",
	"UI",
	"Control",
	"Function",
	"Message",
	"Sound"
};

CHAR *g_aszUIActionUIType[AGCDUI_MAX_ACTION_UI] =
{
	"None",
	"Open",
	"Close",
	"Toggle",
	"Move",
	"Close Event UI",
	"Attach Left",
	"Attach Right",
	"Attach Top",
	"Attach Bottom",
	"Detach"
};

CHAR *g_aszUIActionMessageType[AGCDUI_MAX_ACTION_MESSAGE] =
{
	"OK Dialog",
	"OK Cancel Dialog",
	"Input Dialog",
	"Chatting Window"
};

CHAR *	g_aszMessages[AcUIBase::TYPE_COUNT][20] =
{
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_NONE
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_BASE
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "Input End", "Input Tab", "Input Shift-Tab", "Activate", "Deactivate", NULL},	// TYPE_EDIT
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "Click", "PushDown", "PushUp", "Drag Drop", NULL},		// TYPE_BUTTON
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "Check", "Uncheck", NULL},	// TYPE_CHECKBOX
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "SelectChange", NULL},	// TYPE_COMBO
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_NUMBER
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_TOOLTIP
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "Drag Start", "Drag End", "Drag Drop", "LClick", "RClick", "GridItem Set Focus", "GridItem Kill Focus", NULL},	// TYPE_GRID
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_BAR
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "Scroll", NULL},	// TYPE_LIST
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_LIST_ITEM
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "Drag Start", "Drag End", "Drag Drop", "LClick", "RClick", "GridItem Set Focus", "GridItem Kill Focus", NULL},	// TYPE_SKILL_TREE
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "Drag Start", "Drag End", NULL},	// TYPE_GRID_ITEM
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "Change Value", "UpButton", "DownButton", NULL},	// TYPE_SCROLL
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", "Base10", "Button Move", NULL},	// TYPE_SCROLL_BUTTON
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_CUSTOM
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_TREE
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_TREE_ITEM
	{"Base Set Focus", "Base Kill Focus", "Base LClick", "Base RClick", "Base LClickDbl", "Base RClickDbl", "Base Close", "Base Init", "Base DragDrop", NULL},	// TYPE_CLOCK
};

CHAR *	g_aszKeyStatus[AGCDUI_MAX_HOTKEY_TYPE] =
{
	"Normal", "Shift", "Ctrl", "Alt"
};

CHAR *	g_aszKeyTable[256] =
{
    NULL, "ESC", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "BACKSP", "TAB",
    "q", "w", "e", "r", "t", "y", "u", "i",
    "o", "p", "[", "]", "ENTER", NULL, "a", "s",
    "d", "f", "g", "h", "j", "k", "l", ";",
    "\"", "`", NULL, "#", "z", "x", "c", "v",
    "b", "n", "m", ",", ".", "/", NULL, "TIMES",
    NULL, " ", "CAPSLK", "F1", "F2", "F3", "F4", "F5",
    "F6", "F7", "F8", "F9", "F10", "NUMLOCK", NULL, "HOME",
    "PADUP", "PADPGUP", "MINUS", "PADLEFT", "PAD5", "PADRIGHT", "PLUS", "PADEND",
    "PADDOWN", "PADPGDN", "PADINS", "PADDEL", NULL, NULL, "\\", "F11",
    "F12", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, "PADENTER", NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, "DIVIDE", NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, "NUMLOCK", NULL, "HOME",
    "UP", "PGUP", NULL, "LEFT", NULL, "RIGHT", NULL, "END",
    "DOWN", "PGDN", "INS", "DEL", NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "<LClick>", "<RClick>", "<MClick>", NULL, NULL, NULL, NULL, NULL	// 248부터
};

CHAR *	g_aszTextHAlign[ACUIBASE_MAX_HALIGN] =
{
	"Left",
	"Center",
	"Right"
};

CHAR *	g_aszTextVAlign[ACUIBASE_MAX_VALIGN] =
{
	"Top",
	"Center",
	"Bottom"
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() 
{
	memset( m_strBuffer, 0, sizeof(m_strBuffer) );
	memset( m_strBufferComposing, 0, sizeof(m_strBufferComposing) );

	m_lControlX = INITIAL_CONTROL_X;
	m_lControlY = INITIAL_CONTROL_Y;

	m_pcsUI = NULL;
	m_pcsControl = NULL;

	m_pcsSelectedWindow = NULL;
	m_eMode	= UIT_MODE_EDIT;

	m_bDrawingArea	= FALSE;
	m_bDrawGrid		= FALSE;

	m_pdlgWinProperty = NULL;
}

MainWindow::~MainWindow()
{
	delete m_pdlgWinProperty;
}

// 초기화 , 데이타 수집 & 그래픽 데이타 로딩.	WindowUI 의경우 Open돼는 시점, Full UI의 경우 Setting돼는 시점.
BOOL MainWindow::OnInit			(					)
{
	m_pdlgWinProperty = new UITWindowProperties("Properties");
	m_pdlgWinProperty->Create();
	m_pdlgWinProperty->ShowWindow(SW_SHOW);

	m_pdlgWinProperty->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	srand(time(NULL));

	VERIFY(g_pcsAgcmUIControl	->UIIniRead				(".\\INI\\UIDataList.ini", FALSE));
//	VERIFY(g_pcsAgpmEventTeleport	->StreamReadGroup	(".\\Ini\\TeleportGroup.ini", FALSE));
//	VERIFY(g_pcsAgpmEventTeleport	->StreamReadPoint	(".\\Ini\\TeleportPoint.ini", FALSE));
//	VERIFY(g_pcsApmObject		->StreamReadTemplate		(".\\Ini\\ObjectTemplate.ini"));
//	VERIFY(g_pcsApmObject		->StreamRead				(".\\Ini\\ObjStatic.ini"));
//	VERIFY(g_pcsAgpmFactors		->CharacterTypeStreamRead(".\\Ini\\CharType.ini", FALSE));
//	VERIFY(g_pcsAgpmSkill		->StreamReadTemplate		(".\\Ini\\SkillTemplate.ini"));
//	VERIFY(g_pcsAgpmCharacter	->StreamReadTemplate		(".\\Ini\\CharacterTemplate.ini"));
//	VERIFY(g_pcsAgpmItem			->StreamReadTemplate		(".\\Ini\\ItemTemplate.ini"));
//	VERIFY(g_pcsAgpmSkill		->ReadSkillSpecTxt		(".\\Ini\\Skill_Spec.txt", FALSE));
//	VERIFY(g_pcsAgpmSkill		->ReadSkillConstTxt		(".\\Ini\\Skill_Const.txt", FALSE));
//	VERIFY(g_pcsAgpmSkill		->ReadSkillMasteryTxt	(".\\Ini\\Skill_Mastery.txt", FALSE));
//	VERIFY(g_pcsAgpmSkill		->StreamReadSpecialize	(".\\Ini\\SkillSpecialization.ini", FALSE));
//	VERIFY(g_pcsAgpmSkill		->ReadSkillTooltipTxt	(".\\Ini\\SkillTooltip.txt", FALSE));
	VERIFY(g_pcsAgcmLensFlare	->StreamReadTemplate		("./Ini/LensFlare.ini", FALSE));
	VERIFY(g_pcsAgcmTextBoardMng	->StreamReadTemplate		("./Ini/TextBoard.ini", FALSE));
	VERIFY(g_pcsAgcmEventNature	->LoadTextureScriptFile	("./Ini/alefskysetting.ini"));
	VERIFY(g_pcsAgpmEventNature	->SkySet_StreamRead		( "./Ini/SkySet.ini" , FALSE) );
	VERIFY(g_pcsAgcmTargeting	->StreamReadTemplate		("./Ini/Targeting.ini", FALSE));
	VERIFY(g_pcsAgcmUIManager2	->StreamReadUIMessage	("./Ini/UIMessage.txt", FALSE));

	return TRUE;
}

BOOL MainWindow::OnLButtonDown	( RsMouseStatus *ms	)
{
	if (m_eMode == UIT_MODE_AREA)
	{
		m_stAreaStart = ms->pos;
		m_stAreaEnd = ms->pos;

		m_bDrawingArea = TRUE;
		m_bDrawArea = TRUE;

		return TRUE;
	}

	return FALSE;
}

BOOL MainWindow::OnLButtonUp	( RsMouseStatus *ms	)
{
	if (m_eMode == UIT_MODE_AREA && m_bDrawingArea)
	{
		if (ms->pos.x < m_stAreaStart.x)
		{
			m_stAreaEnd.x = m_stAreaStart.x;
			m_stAreaStart.x = ms->pos.x;
		}
		else
		{
			m_stAreaEnd.x = ms->pos.x;
		}

		if (ms->pos.y < m_stAreaStart.y)
		{
			m_stAreaEnd.y = m_stAreaStart.y;
			m_stAreaStart.y = ms->pos.y;
		}
		else
		{
			m_stAreaEnd.y = ms->pos.y;
		}

		m_bDrawingArea = FALSE;

		return TRUE;
	}

	return FALSE;
}

BOOL MainWindow::OnRButtonDown	( RsMouseStatus *ms	)
{
	return FALSE;
}

BOOL MainWindow::OnMouseMove	( RsMouseStatus *ms	)
{
	m_nXPos	= ( INT32 ) ms->pos.x;
	m_nYPos	= ( INT32 ) ms->pos.y;

	if (m_eMode == UIT_MODE_AREA && m_bDrawingArea)
	{
		m_stAreaEnd = ms->pos;

		return TRUE;
	}

	return FALSE;
}

BOOL MainWindow::OnIdle			(UINT32 ulClockCount)
{
	return FALSE;
}

void MainWindow::OnClose		(					)	// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close돼는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
{
	m_pdlgWinProperty->m_dlgWinProperty.m_csTextures.DestroyTextureList();
}


BOOL MainWindow::OnKeyDown		( RsKeyStatus *ks	)
{
    switch( ks->keyCharCode )
    {
	case rsF1:
        {
			g_pcsAgcmUIManager2->m_csEffectWindow.SetEffectText("Success", 3, 200, 0, 0xff, 1.0f, 2000, TRUE);
			g_pcsAgcmUIManager2->m_csEffectWindow.SetEffectSubText("Test", 4, 300, 0, 0xff, 1.0f);
        }
		return TRUE;

	case rsF2:
		{
			g_pcsAgcmUIManager2->m_csEffectWindow.SetEffectImage("Success", 0xffffff, 8.0f, 300, 300, 2000, 60, 194);
		}
		return TRUE;

	case rsF3:
		{
			g_pcsAgcmUIManager2->m_csEffectWindow.SetEffectImage("Failed", 0xffffff, 5.0f, 500, 1000, 2000, 60, 240);
		}
		return TRUE;

	case rsF4:
		{
			g_pcsAgcmUIManager2->m_csEffectWindow.SetEffectFade(0x0000ff, 100, 100);
		}
		return TRUE;

	case rsF8 :
		{			
		}
		return TRUE;

	default:
		break;
	}
	return FALSE;
}

BOOL MainWindow::OnKeyUp		( RsKeyStatus *ks	)
{
 //   switch( ks->keyCharCode )
    {
	}

	return FALSE;
}

BOOL MainWindow::OnRButtonUp	( RsMouseStatus *ms	)
{
	return FALSE;
}

BOOL MainWindow::OnChar			( char *pChar	, UINT lParam )
{
	if( strlen( m_strBuffer ) >= 250 )
	{
		strncpy( m_strBuffer , "" , 256 );
	}

	switch( *pChar )
	{
	case '\r'	:	strncpy( m_strBuffer , "" , 256 );	break;
	case '\b'	:
		{
			if( m_strBuffer[ strlen( m_strBuffer ) - 1 ] < 0 )
				m_strBuffer[ strlen( m_strBuffer ) - 2 ] = 0x00;
			else
				m_strBuffer[ strlen( m_strBuffer ) - 1 ] = 0x00;
		}
		break;
	default:
		{
			strcat( m_strBuffer , pChar );
			
			TRACE( "까꿍! (%s)\n" , pChar );
		}
		break;
	}

	return TRUE;
}

BOOL MainWindow::PreTranslateInputMessage	( RsEvent event , PVOID param	)
{
	if (m_eMode == UIT_MODE_RUN)
	{
		return FALSE;
	}
	else if (m_eMode == UIT_MODE_EDIT)
	{
		switch (event)
		{
		case rsKEYDOWN			:
			OnKeyDown( (RsKeyStatus *) param );
			return TRUE;

		case rsKEYUP			:
			OnKeyUp( (RsKeyStatus *) param );
			return TRUE;

		case rsCHAR				:
			OnChar( (char *) param , 0 );
			return TRUE;

		case rsIMECOMPOSING		:
			OnIMEComposing( (char *) param , 0 );
			return TRUE;

		case rsLEFTBUTTONDBLCLK	:
			ScreenToClient( *(RsMouseStatus *) param );
			OnLButtonDblClk( (RsMouseStatus *) param );
			return TRUE;

		case rsMOUSEMOVE		:
			ScreenToClient( *(RsMouseStatus *) param );
			OnMouseMove( (RsMouseStatus *) param );
			return TRUE;
		}
	}
	else if (m_eMode == UIT_MODE_AREA)
	{
		switch (event)
		{
		case rsLEFTBUTTONDOWN		:
			ScreenToClient( *(RsMouseStatus *) param );
			OnLButtonDown( (RsMouseStatus *) param );
			return TRUE;

		case rsLEFTBUTTONUP		:
			ScreenToClient( *(RsMouseStatus *) param );
			OnLButtonUp( (RsMouseStatus *) param );
			return TRUE;

		case rsMOUSEMOVE		:
			ScreenToClient( *(RsMouseStatus *) param );
			OnMouseMove( (RsMouseStatus *) param );
			return TRUE;

		case rsLEFTBUTTONDBLCLK	:
			ScreenToClient( *(RsMouseStatus *) param );
			OnLButtonDblClk( (RsMouseStatus *) param );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL MainWindow::OnIMEComposing	( char *composing	, UINT lParam )
{
	strncpy_s( m_strBufferComposing, 256, composing, _TRUNCATE );
	return TRUE;
}


void MainWindow::OnWindowRender	()
{
	AgcWindow::OnWindowRender();
}

void MainWindow::OnRender		( RwRaster *raster	)	// 렌더는 여기서;
{
	g_MainWindow.DrawSelected();
	g_MainWindow.DrawHandler();
	g_MainWindow.DrawArea();
}

void MainWindow::RefreshProperty()
{
	if (!m_pcsSelectedWindow || m_pcsSelectedWindow == (AcUIBase *) this)
		return;

	AcUIBase *		pcsWindow = NULL;
	CHAR *			szName = NULL;
	AgcdUIControl *	pcsControl = g_pcsAgcmUIManager2->GetControl(m_pcsUI, m_pcsSelectedWindow);

	if (m_pcsUI && m_pcsUI->m_pcsUIWindow && m_pcsSelectedWindow == m_pcsUI->m_pcsUIWindow)
	{
		pcsWindow = m_pcsUI->m_pcsUIWindow;
	}
	else if (pcsControl)
	{
		pcsWindow = pcsControl->m_pcsBase;
	}
	else
		pcsWindow = NULL;

	m_pdlgWinProperty->UpdateDataToDialog(pcsWindow, m_pcsUI, pcsControl);
}

void MainWindow::UpdateProperty	()
{
	if (!m_pcsSelectedWindow || m_pcsSelectedWindow == (AcUIBase *) this)
		return;

	m_pdlgWinProperty->UpdateDataFromDialog();

	RefreshProperty();
}

void MainWindow::NewControl		(INT32 lType)
{
	if (m_eMode != UIT_MODE_EDIT)
	{
		AfxMessageBox("Change Edit Mode First !!!");
		return;
	}

	if (!m_pcsUI)
		return;

	AgcdUIControl *		pcsControl;
	AgcdUICControl *	pcsCControl = NULL;

	if (lType == AcUIBase::TYPE_CUSTOM)
	{
		UITCustomControlDialog	dlgCustom;

		if (dlgCustom.DoModal() != IDOK)
			return;

		pcsCControl = dlgCustom.m_pcsCControl;
		if (!pcsCControl)
			return;
	}

	if (m_pcsControl)// && m_pcsControl->m_lType == AcUIBase::TYPE_LIST_ITEM)
	{
		if (AfxMessageBox("Do you want embedded control?", MB_YESNO) == IDYES)
			pcsControl = g_pcsAgcmUIManager2->AddControl(m_pcsUI, lType, 0, 0, m_pcsControl->m_pcsBase, NULL, pcsCControl);
		else
			pcsControl = g_pcsAgcmUIManager2->AddControl(m_pcsUI, lType, m_lControlX, m_lControlY, NULL, NULL, pcsCControl);
	}
	else
		pcsControl = g_pcsAgcmUIManager2->AddControl(m_pcsUI, lType, m_lControlX, m_lControlY, NULL, NULL, pcsCControl);

	if (!pcsControl)
	{
		AfxMessageBox("Error Add Control !!!");
		return;
	}

	SAVE_CONTROL(pcsControl);

	sprintf(pcsControl->m_szName, "CONTROL_%d", rand());

	pcsControl->m_pcsBase->m_Property.bMovable = TRUE;
	pcsControl->m_pcsBase->SetActive(FALSE);
	pcsControl->m_pcsBase->SetDrawOutline(TRUE);

	switch (lType)
	{
	case AcUIBase::TYPE_BASE:
		{
			pcsControl->m_pcsBase->SetRenderDefaultImage(FALSE);
			g_pcsAgcmUIManager2->SetControlDisplayMap(pcsControl, "Caption");

			break;
		}

	case AcUIBase::TYPE_GRID:
	case AcUIBase::TYPE_SKILL_TREE:
		{
			((AcUIGrid *) pcsControl->m_pcsBase)->SetDrawAreas(m_bDrawGrid);

			break;
		}

	case AcUIBase::TYPE_TREE:
	case AcUIBase::TYPE_LIST:
		{
			AgcdUIControl *	pcsControlItem = g_pcsAgcmUIManager2->AddControl(m_pcsUI, lType == AcUIBase::TYPE_TREE ? AcUIBase::TYPE_TREE_ITEM : AcUIBase::TYPE_LIST_ITEM, 0, 0, pcsControl->m_pcsBase);
			if (!pcsControlItem)
			{
				AfxMessageBox("Error Add List Control !!!");
				return;
			}

			SAVE_CONTROL(pcsControlItem);

			sprintf(pcsControlItem->m_szName, "CONTROL_%d", rand());

			pcsControl->m_uoData.m_stList.m_pcsListItemFormat = pcsControlItem;

			pcsControlItem->m_pcsBase->m_Property.bMovable = TRUE;
			pcsControlItem->m_pcsBase->SetActive(FALSE);
			pcsControlItem->m_pcsBase->SetDrawOutline(TRUE);

			pcsControl->m_pcsBase->MoveWindow(pcsControl->m_pcsBase->x,
											  pcsControl->m_pcsBase->y,
											  pcsControl->m_pcsBase->w,
											  pcsControl->m_pcsBase->h * 3);

			pcsControlItem->m_pcsBase->MoveWindow(pcsControlItem->m_pcsBase->x,
												  pcsControlItem->m_pcsBase->y,
												  pcsControlItem->m_pcsBase->w,
												  pcsControlItem->m_pcsBase->h * 2);


			break;
		}

	case AcUIBase::TYPE_SCROLL:
		{
			AgcdUIControl * pcsControlItem = g_pcsAgcmUIManager2->AddControl(m_pcsUI, AcUIBase::TYPE_SCROLL_BUTTON, 0, 0, pcsControl->m_pcsBase);
			if (!pcsControlItem)
			{
				AfxMessageBox("Error Add Scroll Control !!!");
				return;
			}

			SAVE_CONTROL(pcsControlItem);

			sprintf(pcsControlItem->m_szName, "CONTROL_%d", rand());

			pcsControlItem->m_pcsBase->m_Property.bMovable = TRUE;
			pcsControlItem->m_pcsBase->SetActive(FALSE);
			pcsControlItem->m_pcsBase->SetDrawOutline(TRUE);

			((AcUIScroll *) pcsControl->m_pcsBase)->SetScrollButton((AcUIScrollButton *) pcsControlItem->m_pcsBase);

			pcsControl->m_pcsBase->MoveWindow(pcsControl->m_pcsBase->x,
											  pcsControl->m_pcsBase->y,
											  pcsControl->m_pcsBase->w * 2,
											  pcsControl->m_pcsBase->h * 2);

			pcsControlItem->m_pcsBase->MoveWindow(pcsControlItem->m_pcsBase->x,
												  pcsControlItem->m_pcsBase->y,
												  30,
												  30);

			break;
		}

	case AcUIBase::TYPE_COMBO:
		{
			AgcdUIControl *	pcsControlButton = g_pcsAgcmUIManager2->AddControl(m_pcsUI, AcUIBase::TYPE_BUTTON, 0, 0, pcsControl->m_pcsBase);
			if (!pcsControlButton)
			{
				AfxMessageBox("Error Add Combo Control !!!");
				return;
			}

			SAVE_CONTROL(pcsControlButton);

			sprintf(pcsControlButton->m_szName, "CONTROL_%d", rand());

			((AcUICombo *)pcsControl->m_pcsBase)->SetComboButton((AcUIButton *) pcsControlButton->m_pcsBase);

			pcsControlButton->m_pcsBase->m_Property.bMovable = TRUE;
			pcsControlButton->m_pcsBase->SetActive(FALSE);
			pcsControlButton->m_pcsBase->SetDrawOutline(TRUE);

			pcsControl->m_pcsBase->MoveWindow(pcsControl->m_pcsBase->x,
											  pcsControl->m_pcsBase->y,
											  pcsControl->m_pcsBase->w * 2,
											  pcsControl->m_pcsBase->h);


			break;
		}
	}

	AddControlPosition();
}

void MainWindow::NewUI()
{
	AgcdUI *	pcsUI;

	pcsUI = g_pcsAgcmUIManager2->CreateUI();
	sprintf(pcsUI->m_szUIName, "UI_%d", rand());
	g_pcsAgcmUIManager2->AddUI(pcsUI);

	OpenUI(pcsUI);

	ChangeMode(m_eMode);
}

void MainWindow::OpenUI(AgcdUI *pcsUI)
{
	if (m_eMode != UIT_MODE_EDIT)
		return;

	INT32		lIndex = 0;
	AgcdUI *	pcsUITemp;

	for (pcsUITemp = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex); pcsUITemp; pcsUITemp = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex))
	{
		if (pcsUITemp->m_eStatus == AGCDUI_STATUS_OPENED)
			g_pcsAgcmUIManager2->CloseUI(pcsUITemp);
	}

	WindowListUpdate();
	g_pcsAgcmUIManager2->OpenUI(pcsUI, FALSE, TRUE);
	WindowListUpdate();

	m_pcsUI = pcsUI;
	m_pcsControl = NULL;

	m_lControlX = INITIAL_CONTROL_X;
	m_lControlY = INITIAL_CONTROL_Y;

	if (pcsUI)
		m_pcsSelectedWindow = pcsUI->m_pcsUIWindow;
	else
		m_pcsSelectedWindow = NULL;

	RefreshProperty();
}

void MainWindow::DeleteControl	(AgcdUIControl *pcsControl)
{
	if (!m_pcsUI || !pcsControl)
		return;

	m_pcsSelectedWindow = m_pcsUI->m_pcsUIWindow;
	m_pcsControl = NULL;

	RefreshProperty();

	g_pcsAgcmUIManager2->RemoveControl(pcsControl);
}

void MainWindow::DeleteUI		(AgcdUI *pcsUI)
{
	if (!m_pcsUI)
		return;

	m_pcsSelectedWindow = NULL;
	m_pcsUI = NULL;
	m_pcsControl = NULL;

	RefreshProperty();

	g_pcsAgcmUIManager2->RemoveUI(pcsUI);
	g_pcsAgcmUIManager2->DestroyUI(pcsUI);

	g_pMainFrame->UpdateUIList();
}

void MainWindow::AddControlPosition	()
{
	if (!m_pcsUI)
		return;

	m_lControlX += CONTROL_SPAWN_DIFF;
	m_lControlY += CONTROL_SPAWN_DIFF;

	if (m_lControlX >= m_pcsUI->m_pcsUIWindow->w - 100)
		m_lControlX = INITIAL_CONTROL_X;

	if (m_lControlY >= m_pcsUI->m_pcsUIWindow->h - 100)
		m_lControlY = INITIAL_CONTROL_Y;
}

void MainWindow::DrawSelected		()
{
	if (m_eMode == UIT_MODE_RUN || !m_pcsUI)
		return;

	AcUIBase *	pcsWindow = (AcUIBase *) g_MyEngine.GetFocusedWindow();

	if (!pcsWindow || pcsWindow == (AcUIBase *) this)
		pcsWindow = m_pcsSelectedWindow;

	if (pcsWindow != m_pcsSelectedWindow)
	{
		while (pcsWindow && m_pcsUI && pcsWindow != m_pcsUI->m_pcsUIWindow)
		{
			m_pcsControl = g_pcsAgcmUIManager2->GetControl(m_pcsUI, pcsWindow);
			if (m_pcsControl)
				break;

			pcsWindow = (AcUIBase *) pcsWindow->pParent;
		}

		m_pcsSelectedWindow = pcsWindow;
		RefreshProperty();
	}

	if (!pcsWindow)
		pcsWindow = m_pcsUI->m_pcsUIWindow;

	if (pcsWindow != m_pcsSelectedWindow)
	{
		m_pcsSelectedWindow = pcsWindow;
		RefreshProperty();
	}

	m_pcsControl = g_pcsAgcmUIManager2->GetControl(m_pcsUI, pcsWindow);

	INT32	lStartX = 0;
	INT32	lStartY = 0;
	INT32	lEndX = pcsWindow->w;
	INT32	lEndY = pcsWindow->h;
	INT32	lMidX;
	INT32	lMidY;

	pcsWindow->ClientToScreen(&lStartX, &lStartY);
	pcsWindow->ClientToScreen(&lEndX, &lEndY);

	lMidX = (lStartX + lEndX) / 2;
	lMidY = (lStartY + lEndY) / 2;

	g_MyEngine.DrawIm2D(NULL, (FLOAT) lStartX - SELECTED_BOX_SIZE, (FLOAT) lStartY - SELECTED_BOX_SIZE, SELECTED_BOX_SIZE * 2, SELECTED_BOX_SIZE * 2,
			0.0f, 0.0f, 1.0f, 1.0f, SELECTED_BOX_COLOR, 255);

	g_MyEngine.DrawIm2D(NULL, (FLOAT) lStartX - SELECTED_BOX_SIZE, (FLOAT) lEndY - SELECTED_BOX_SIZE, SELECTED_BOX_SIZE * 2, SELECTED_BOX_SIZE * 2,
			0.0f, 0.0f, 1.0f, 1.0f, SELECTED_BOX_COLOR, 255);

	g_MyEngine.DrawIm2D(NULL, (FLOAT) lEndX - SELECTED_BOX_SIZE, (FLOAT) lStartY - SELECTED_BOX_SIZE, SELECTED_BOX_SIZE * 2, SELECTED_BOX_SIZE * 2,
			0.0f, 0.0f, 1.0f, 1.0f, SELECTED_BOX_COLOR, 255);

	g_MyEngine.DrawIm2D(NULL, (FLOAT) lEndX - SELECTED_BOX_SIZE, (FLOAT) lEndY - SELECTED_BOX_SIZE, SELECTED_BOX_SIZE * 2, SELECTED_BOX_SIZE * 2,
			0.0f, 0.0f, 1.0f, 1.0f, SELECTED_BOX_COLOR, 255);

	g_MyEngine.DrawIm2D(NULL, (FLOAT) lStartX - SELECTED_BOX_SIZE, (FLOAT) lMidY - SELECTED_BOX_SIZE, SELECTED_BOX_SIZE * 2, SELECTED_BOX_SIZE * 2,
			0.0f, 0.0f, 1.0f, 1.0f, SELECTED_BOX_COLOR, 255);

	g_MyEngine.DrawIm2D(NULL, (FLOAT) lEndX - SELECTED_BOX_SIZE, (FLOAT) lMidY - SELECTED_BOX_SIZE, SELECTED_BOX_SIZE * 2, SELECTED_BOX_SIZE * 2,
			0.0f, 0.0f, 1.0f, 1.0f, SELECTED_BOX_COLOR, 255);

	g_MyEngine.DrawIm2D(NULL, (FLOAT) lMidX - SELECTED_BOX_SIZE, (FLOAT) lStartY - SELECTED_BOX_SIZE, SELECTED_BOX_SIZE * 2, SELECTED_BOX_SIZE * 2,
			0.0f, 0.0f, 1.0f, 1.0f, SELECTED_BOX_COLOR, 255);

	g_MyEngine.DrawIm2D(NULL, (FLOAT) lMidX - SELECTED_BOX_SIZE, (FLOAT) lEndY - SELECTED_BOX_SIZE, SELECTED_BOX_SIZE * 2, SELECTED_BOX_SIZE * 2,
			0.0f, 0.0f, 1.0f, 1.0f, SELECTED_BOX_COLOR, 255);
}

void MainWindow::DrawArea			()
{
	if (m_eMode != UIT_MODE_AREA || !m_bDrawArea)
		return;

	if (m_stAreaEnd.x > m_stAreaStart.x)
	{
		g_MyEngine.DrawIm2D(NULL, m_stAreaStart.x, m_stAreaStart.y, m_stAreaEnd.x - m_stAreaStart.x, 1, 0.0f, 0.0f, 1.0f, 1.0f, AREA_LINE_COLOR, 255);
		g_MyEngine.DrawIm2D(NULL, m_stAreaStart.x, m_stAreaEnd.y, m_stAreaEnd.x - m_stAreaStart.x, 1, 0.0f, 0.0f, 1.0f, 1.0f, AREA_LINE_COLOR, 255);
	}
	else
	{
		g_MyEngine.DrawIm2D(NULL, m_stAreaEnd.x, m_stAreaStart.y, m_stAreaStart.x - m_stAreaEnd.x, 1, 0.0f, 0.0f, 1.0f, 1.0f, AREA_LINE_COLOR, 255);
		g_MyEngine.DrawIm2D(NULL, m_stAreaEnd.x, m_stAreaEnd.y, m_stAreaStart.x - m_stAreaEnd.x, 1, 0.0f, 0.0f, 1.0f, 1.0f, AREA_LINE_COLOR, 255);
	}

	if (m_stAreaEnd.y > m_stAreaStart.y)
	{
		g_MyEngine.DrawIm2D(NULL, m_stAreaStart.x, m_stAreaStart.y, 1, m_stAreaEnd.y - m_stAreaStart.y, 0.0f, 0.0f, 1.0f, 1.0f, AREA_LINE_COLOR, 255);
		g_MyEngine.DrawIm2D(NULL, m_stAreaEnd.x, m_stAreaStart.y, 1, m_stAreaEnd.y - m_stAreaStart.y, 0.0f, 0.0f, 1.0f, 1.0f, AREA_LINE_COLOR, 255);
	}
	else
	{
		g_MyEngine.DrawIm2D(NULL, m_stAreaStart.x, m_stAreaEnd.y, 1, m_stAreaStart.y - m_stAreaEnd.y, 0.0f, 0.0f, 1.0f, 1.0f, AREA_LINE_COLOR, 255);
		g_MyEngine.DrawIm2D(NULL, m_stAreaEnd.x, m_stAreaEnd.y, 1, m_stAreaStart.y - m_stAreaEnd.y, 0.0f, 0.0f, 1.0f, 1.0f, AREA_LINE_COLOR, 255);
	}
}

void MainWindow::DrawHandler		()
{
	if (m_eMode != UIT_MODE_AREA)
		return;

	if (!m_pcsUI || !m_pcsUI->m_pcsUIWindow)
		return;

	RwRect *	pstRect = &m_pcsUI->m_pcsUIWindow->m_rectHandlerArea;
	INT32		lX = pstRect->x;
	INT32		lY = pstRect->y;

	m_pcsUI->m_pcsUIWindow->ClientToScreen(&lX, &lY);

	g_MyEngine.DrawIm2D(NULL, (FLOAT) lX, (FLOAT) lY, (FLOAT) pstRect->w, (FLOAT) pstRect->h, 0.0f, 0.0f, 1.0f, 1.0f, HANDLER_COLOR, 50);
}

void MainWindow::ChangeMode			(UITMode eMode, BOOL bSave)
{
	m_eMode = eMode;

//	g_pcsAgcmUIManager2->CountUserData();

	if (m_eMode == UIT_MODE_EDIT || m_eMode == UIT_MODE_RUN || m_eMode == UIT_MODE_AREA)
	{
		INT32			lIndex1;
		INT32			lIndex2;
		AgcdUI *		pcsUI;
		AgcdUIControl *	pcsControl;

		lIndex1 = 0;
		for (pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex1); pcsUI; pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex1))
		{
			pcsUI->m_pcsUIWindow->UpdateChildWindow();

			if (m_eMode == UIT_MODE_EDIT)
			{
				//pcsUI->m_pcsUIWindow->SetStatus(-1, FALSE);
				pcsUI->m_pcsUIWindow->SetDrawOutline(TRUE);
				pcsUI->m_pcsUIWindow->SetActive(FALSE);
			}
			else if (m_eMode == UIT_MODE_RUN)
			{
				//pcsUI->m_pcsUIWindow->SetStatus(-1, FALSE);
				pcsUI->m_pcsUIWindow->SetDrawOutline(FALSE);
				pcsUI->m_pcsUIWindow->SetActive(TRUE);

				if (pcsUI->m_eType != AGCDUI_TYPE_NORMAL)
					g_pcsAgcmUIManager2->SortPopupUI(pcsUI);

				lIndex2 = 0;
				for (pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(pcsUI, &lIndex2); pcsControl; pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(pcsUI, &lIndex2))
				{
					LOAD_CONTROL(pcsControl);
				}
			}
			else if (m_eMode == UIT_MODE_AREA)
			{
				//pcsUI->m_pcsUIWindow->SetStatus(-1, FALSE);
				pcsUI->m_pcsUIWindow->SetDrawOutline(TRUE);
				pcsUI->m_pcsUIWindow->SetActive(FALSE);
			}

			lIndex2 = 0;
			for (pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(pcsUI, &lIndex2); pcsControl; pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(pcsUI, &lIndex2))
			{
				if (pcsControl->m_bDynamic)
					continue;

				if (m_eMode == UIT_MODE_EDIT)
				{
					if (bSave)
						SAVE_CONTROL(pcsControl);

					pcsControl->m_pcsBase->m_Property.bMovable = TRUE;
					pcsControl->m_pcsBase->m_Property.bVisible = TRUE;
					pcsControl->m_pcsBase->m_Property.bUseInput = TRUE;

					pcsControl->m_pcsBase->SetActive( FALSE );
					//pcsControl->m_pcsBase->SetStatus(-1, FALSE);
					pcsControl->m_pcsBase->SetDrawOutline( TRUE );

					switch (pcsControl->m_lType)
					{
					case AcUIBase::TYPE_GRID:
					case AcUIBase::TYPE_SKILL_TREE:
						{
							((AcUIGrid *) pcsControl->m_pcsBase)->SetDrawAreas(m_bDrawGrid);
							break;
						}

					case AcUIBase::TYPE_TREE:
					case AcUIBase::TYPE_LIST:
						{
							if (pcsControl->m_uoData.m_stList.m_pcsListItemFormat)
							{
								AcUIListItem *	pcsFormatItem = (AcUIListItem *) pcsControl->m_uoData.m_stList.m_pcsListItemFormat->m_pcsBase;

								if (!pcsFormatItem->pParent)
									pcsControl->m_pcsBase->AddChild(pcsFormatItem, pcsFormatItem->GetControlID(), FALSE);
							}

							((AcUIList *) pcsControl->m_pcsBase)->SetListItemWindowVisibleRow(0);
							((AcUIList *) pcsControl->m_pcsBase)->SetListItemWindowStartRow(0, TRUE);

							break;
						}
					}
				}
				else if (m_eMode == UIT_MODE_RUN)
				{
					pcsControl->m_pcsBase->SetActive( TRUE );
					//pcsControl->m_pcsBase->SetStatus(-1, FALSE);
					pcsControl->m_pcsBase->SetDrawOutline( FALSE );

					switch (pcsControl->m_lType)
					{
					case AcUIBase::TYPE_GRID:
					case AcUIBase::TYPE_SKILL_TREE:
						{
							((AcUIGrid *) pcsControl->m_pcsBase)->SetDrawAreas(m_bDrawGrid);
							break;
						}

					case AcUIBase::TYPE_TREE:
					case AcUIBase::TYPE_LIST:
						{
							AgcdUIControl *	pcsFormatControl = pcsControl->m_uoData.m_stList.m_pcsListItemFormat;

							if (pcsFormatControl)
							{
								AcUIListItem *	pcsFormatItem = (AcUIListItem *) pcsFormatControl->m_pcsBase;
								if( pcsFormatItem )
								{
									if (pcsFormatItem->pParent)
										pcsFormatItem->pParent->DeleteChild(pcsFormatItem, FALSE, FALSE);

									if (pcsControl->m_pstUserData)
									{
										((AcUIList *) pcsControl->m_pcsBase)->SetListItemWindowVisibleRow(GET_CONTROL_STYLE(pcsControl)->m_lListItemVisibleRow);
										((AcUIList *) pcsControl->m_pcsBase)->SetListItemWindowMoveInfo(pcsFormatItem->w, pcsFormatItem->h, pcsFormatItem->x, pcsFormatItem->y);
										((AcUIList *) pcsControl->m_pcsBase)->SetListItemWindowTotalNum(pcsControl->m_pstUserData->m_stUserData.m_lCount);
										((AcUIList *) pcsControl->m_pcsBase)->SetListItemWindowStartRow(0);
									}
								}
							}

							break;
						}
					}
				}
				else if (m_eMode == UIT_MODE_AREA)
				{
					pcsControl->m_pcsBase->m_Property.bMovable = FALSE;
					pcsControl->m_pcsBase->SetActive( FALSE );
					//pcsControl->m_pcsBase->SetStatus(-1, FALSE);
					pcsControl->m_pcsBase->SetDrawOutline( TRUE );

					switch (pcsControl->m_lType)
					{
					case AcUIBase::TYPE_GRID:
					case AcUIBase::TYPE_SKILL_TREE:
						{
							((AcUIGrid *) pcsControl->m_pcsBase)->SetDrawAreas(m_bDrawGrid);
							break;
						}

					case AcUIBase::TYPE_TREE:
					case AcUIBase::TYPE_LIST:
						{
							if (pcsControl->m_uoData.m_stList.m_pcsListItemFormat)
							{
								pcsControl->m_uoData.m_stList.m_pcsListItemFormat->m_pcsBase->m_Property.bVisible = FALSE;
							}
							break;
						}
					}
				}

				g_pcsAgcmUIManager2->RefreshControl(pcsControl, TRUE);
			}

			pcsUI->m_pcsUIWindow->UpdateChildWindow();
		}
	}

	switch (m_eMode)
	{
	case UIT_MODE_RUN:
		{
			AcUIToolTip::m_bViewToolTip = TRUE;
			m_pdlgWinProperty->ShowWindow(SW_HIDE);
			g_pcsAgcmUIManager2->OpenMainUI();
			break;
		}
	case UIT_MODE_EDIT:
		{
			OpenUI(m_pcsUI);

			m_pdlgWinProperty->ShowWindow(SW_SHOW);

			break;
		}
	case UIT_MODE_AREA:
		{
			m_pdlgWinProperty->ShowWindow(SW_SHOW);
			m_bDrawArea = FALSE;
			break;
		}
	}

	g_pcsAgcmUIManager2->CountUserData();
}

BOOL MainWindow::ChangeName			(const CHAR *szName)
{
	if (m_pcsUI && m_pcsSelectedWindow == m_pcsUI->m_pcsUIWindow)
		return g_pcsAgcmUIManager2->ChangeUIName(m_pcsUI, (CHAR *) szName);

	return TRUE;
}

void MainWindow::SetGridView(BOOL bViewGrid)
{
	INT32			lIndex1;
	INT32			lIndex2;
	AgcdUI *		pcsUI;
	AgcdUIControl *	pcsControl;

	m_bDrawGrid = bViewGrid;

	lIndex1 = 0;
	for (pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex1); pcsUI; pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex1))
	{
		lIndex2 = 0;
		for (pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(pcsUI, &lIndex2); pcsControl; pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(pcsUI, &lIndex2))
		{
			if (pcsControl->m_lType == AcUIBase::TYPE_GRID || pcsControl->m_lType == AcUIBase::TYPE_SKILL_TREE)
			{
				((AcUIGrid *) pcsControl->m_pcsBase)->SetDrawAreas(bViewGrid);
			}
		}
	}
}

void MainWindow::OnMoveWindow()
{
	if (g_pMainFrame && g_pMainFrame->m_eView != UIT_VIEW_WINDOW)
	{
		this->w	=	g_acsViewSize[g_pMainFrame->m_eView].cx;
		this->h	=	g_acsViewSize[g_pMainFrame->m_eView].cy;
		
		if (g_pcsAgcmUIManager2)
		{
			g_pcsAgcmUIManager2->m_csManagerWindow.MoveWindow(
				g_pcsAgcmUIManager2->m_csManagerWindow.x,
				g_pcsAgcmUIManager2->m_csManagerWindow.y,
				g_acsViewSize[g_pMainFrame->m_eView].cx,
				g_acsViewSize[g_pMainFrame->m_eView].cy);
		}
	}
}












