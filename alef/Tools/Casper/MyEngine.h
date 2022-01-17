// MyEngine.h: interface for the MyEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYENGINE_H__771281B5_C1D8_49D7_B37D_3DDD8F62B6B3__INCLUDED_)
#define AFX_MYENGINE_H__771281B5_C1D8_49D7_B37D_3DDD8F62B6B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AgcEngine.h>

#include "AgpmTimer.h"
#include "AgpmFactors.h"

#include "AgpmGrid.h"
#include "AgcModule.h"

#include "ApmMap.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgcmConnectManager.h"
#include "AgpmCasper.h"
#include "AgpmGuild.h"
#include "AgcmCasper.h"
#include "AgpmOptimizedPacket2.h"


#include "MainWindow.h"
#include <vector>

#define GET_CONTROL_STYLE(control)	(((MyEngine *) g_pEngine)->GetControlData(control))
#define SAVE_CONTROL(control)		(((MyEngine *) g_pEngine)->SaveControlData(control))
#define LOAD_CONTROL(control)		(((MyEngine *) g_pEngine)->LoadControlData(control))

extern MainWindow			g_MainWindow			;

extern AgpmTimer			* g_pcsAgpmTimer		;
extern AgpmGrid				* g_pcsAgpmGrid			;

extern AgpmFactors			* g_pcsAgpmFactors		;
extern AgpmCharacter		* g_pcsAgpmCharacter	;
extern AgpmItem				* g_pcsAgpmItem			;

extern AgpmUnion			* g_pcsAgpmUnion		;
extern AgpmParty			* g_pcsAgpmParty		;

extern ApmEventManager		* g_pcsApmEventManager	;

extern AgpmSkill			* g_pcsAgpmSkill		;

extern AgpmGuild			* g_pcsAgpmGuild		;

extern AgpmCasper			* g_pcsAgpmCasper		;
extern AgcmCasper			* g_pcsAgcmCasper		;

extern AgpmItemConvert		* g_pcsAgpmItemConvert	;

extern AgpmOptimizedPacket2	* g_pcsAgpmOptimizedPacket2	;


extern std::vector<CString> g_vtCharName;

const INT32 TIMER_SEQUENCE_LOGIN = 1;
const INT32 TIMER_TEST_ACTION = 2;

extern INT32 TIMER_SEQUENCE_LOGIN_ELAPSE;
extern INT32 TIMER_TEST_ACTION_ELAPSE;

class MyEngine : public AgcEngine  
{
private:

public:
	MyEngine();
	virtual ~MyEngine();

	// Message
	BOOL				OnInit					();
	BOOL				OnAttachPlugins			();	// 플러그인 연결 은 여기서
	RpWorld *			OnCreateWorld			(); // 월드를 생성해서 리턴해준다.
	RwCamera *			OnCreateCamera			( RpWorld * pWorld );	// 메인 카메라를 Create해준다.
	BOOL				OnCreateLight			();	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
	AgcWindow *			OnSetMainWindow			();	// 디폴트 윈도우를 설정해주는 메시지.. 윈도우를 초기화해서 포인터 넘기면 됀다.

	BOOL				OnRegisterModule		();

	VOID				OnWorldRender			(); // World Render 부분에서 AgcmRender를 사용한다.

	VOID				OnTerminate				();

	VOID				OnEndIdle				();
	
	virtual BOOL				OnCameraResize			( RwRect * pRect );

	BOOL				SetSequenceLogin();
	BOOL				SetConcurrencyLogin();
	
	BOOL				OnRenderPreCameraUpdate();

public:
	BOOL				m_bSequenceMode;
	
	static BOOL CB_EncryptCodeSuccess( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CB_LoginSucceeded(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CB_GetUnionFinished( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CB_GetCharacterFinished( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CB_SettingCharacterFinished( PVOID pData, PVOID pClass, PVOID pCustData );
};

extern MyEngine			g_MyEngine			;

#endif // !defined(AFX_MYENGINE_H__771281B5_C1D8_49D7_B37D_3DDD8F62B6B3__INCLUDED_)
