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

extern AgcEngine *	g_pEngine;

#define INITIAL_CONTROL_X	20
#define INITIAL_CONTROL_Y	20
#define CONTROL_SPAWN_DIFF	20

#define SELECTED_BOX_SIZE	5
#define SELECTED_BOX_COLOR	0x00777777
#define GRID_ITEM_COLOR		0x00000055
#define AREA_LINE_COLOR		0x00000000
#define HANDLER_COLOR		0x00000080



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() 
{
}

MainWindow::~MainWindow()
{
}

// 초기화 , 데이타 수집 & 그래픽 데이타 로딩.	WindowUI 의경우 Open돼는 시점, Full UI의 경우 Setting돼는 시점.
BOOL MainWindow::OnInit			(					)
{
	srand(time(NULL));

	//VERIFY(g_csAgcmUIControl	.UIIniRead				(".\\INI\\UIDataList.ini"));
	//VERIFY(g_pcsAgpmEventTeleport	.StreamReadGroup	(".\\Ini\\TeleportGroup.ini", TRUE));
	//VERIFY(g_pcsAgpmEventTeleport	.StreamReadPoint	(".\\Ini\\TeleportPoint.ini", TRUE));
	//VERIFY(g_pcsApmObject		.StreamReadTemplate		(".\\Ini\\ObjectTemplate.ini", TRUE));
	//VERIFY(g_pcsApmObject		.StreamRead				(".\\Ini\\ObjStatic.ini", TRUE));
	VERIFY(g_pcsAgpmFactors		->CharacterTypeStreamRead(".\\Ini\\CharType.ini", TRUE));
	//VERIFY(g_pcsAgpmSkill		->StreamReadTemplate		(".\\Ini\\SkillTemplate.ini", TRUE));
	VERIFY(g_pcsAgpmCharacter	->StreamReadTemplate		(".\\Ini\\CharacterTemplate.ini", NULL, TRUE));
	VERIFY(g_pcsAgpmItem		->StreamReadTemplate		(".\\Ini\\ItemTemplate.ini", NULL, TRUE));
	//VERIFY(g_pcsAgpmSkill		->ReadSkillSpecTxt		(".\\Ini\\Skill_Spec.txt", TRUE));
	//VERIFY(g_pcsAgpmSkill		->ReadSkillConstTxt		(".\\Ini\\Skill_Const.txt", TRUE));
	//VERIFY(g_pcsAgpmSkill		->ReadSkillMasteryTxt	(".\\Ini\\Skill_Mastery.txt", TRUE));
	//VERIFY(g_pcsAgpmSkill		->StreamReadSpecialize	(".\\Ini\\SkillSpecialization.ini", TRUE));
	//VERIFY(g_pcsAgpmSkill		->ReadSkillTooltipTxt	(".\\Ini\\SkillTooltip.txt", TRUE));
	//VERIFY(g_pcsAgcmLensFlare	.StreamReadTemplate		("./Ini/LensFlare.ini", TRUE));
	//VERIFY(g_csAgcmTextBoard	.StreamReadTemplate		("./Ini/TextBoard.ini"));
	//VERIFY(g_csAgcmEventNature	.LoadTextureScriptFile	("./Ini/alefskysetting.ini"));
	//VERIFY(g_pcsAgpmEventNature	.SkySet_StreamRead		( "./Ini/SkySet.ini", TRUE ) );
	//VERIFY(g_csAgcmTargeting	.StreamReadTemplate		("./Ini/Targeting.ini"));

	return TRUE;
}

BOOL MainWindow::OnLButtonDown	( RsMouseStatus *ms	)
{
	return FALSE;
}

BOOL MainWindow::OnLButtonUp	( RsMouseStatus *ms	)
{
	return FALSE;
}

BOOL MainWindow::OnRButtonDown	( RsMouseStatus *ms	)
{
	return FALSE;
}

BOOL MainWindow::OnMouseMove	( RsMouseStatus *ms	)
{
	return FALSE;
}

BOOL MainWindow::OnIdle			(UINT32 ulClockCount)
{
	return FALSE;
}

void MainWindow::OnClose		(					)	// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close돼는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
{
}


BOOL MainWindow::OnKeyDown		( RsKeyStatus *ks	)
{
    switch( ks->keyCharCode )
    {
	case rsF1:
        {
        }
		return TRUE;

	case rsF2:
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
	return TRUE;
}

BOOL MainWindow::PreTranslateInputMessage	( RsEvent event , PVOID param	)
{
	return FALSE;
}

BOOL MainWindow::OnIMEComposing	( char *composing	, UINT lParam )
{
	return TRUE;
}


void MainWindow::OnWindowRender	()
{
	AgcWindow::OnWindowRender();
}
