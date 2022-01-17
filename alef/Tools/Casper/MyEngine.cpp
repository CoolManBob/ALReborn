// MyEngine.cpp: implementation of the MyEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rtcharse.h"
#include "rpcollis.h"
#include "rpusrdat.h"
#include "rpspline.h"
#include "rtanim.h"
#include "rplodatm.h"
#include "rpmatfx.h"
#include "rpmorph.h"

#include "AcuRpMtexture.h"

#include <skeleton.h>
#include <menu.h>
#include <camera.h>

#include "MyEngine.h"

#include "time.h"

// Image Loader
#include "rtbmp.h"
#include "rtpng.h"

#include "MainWindow.h"
#include <win.h>

#include "MainFrm.h"
//#include "ApmProfiler.h"

#include "MainFrm.h"

#include "AcuFrameMemory.h"

#include "ApMemoryTracker.h"

INT32 TIMER_SEQUENCE_LOGIN_ELAPSE = 1000;
INT32 TIMER_TEST_ACTION_ELAPSE = 4000;

//typedef char strCharName[AGPACHARACTER_MAX_ID_STRING + 1];
std::vector<CString> g_vtCharName;

MainWindow			g_MainWindow		;
MyEngine			g_MyEngine			;

AgpmTimer			* g_pcsAgpmTimer			;
AgpmGrid			* g_pcsAgpmGrid			;

AgpmFactors			* g_pcsAgpmFactors			;

ApmMap				* g_pcsApmMap				;
AgpmCharacter		* g_pcsAgpmCharacter		;
AgpmItem			* g_pcsAgpmItem				;
AgpmUnion			* g_pcsAgpmUnion			;
AgpmParty			* g_pcsAgpmParty			;
ApmEventManager		* g_pcsApmEventManager		;
AgpmSkill			* g_pcsAgpmSkill			;
AgcmConnectManager	* g_pcsAgcmConnectManager	;

AgpmCasper			* g_pcsAgpmCasper		;

AgpmItemConvert		* g_pcsAgpmItemConvert	;

AgpmGuild			* g_pcsAgpmGuild		;

//AgpmOptimizedPacket	* g_pcsAgpmOptimizedPacket	;
AgpmOptimizedPacket2	* g_pcsAgpmOptimizedPacket2	;

AgcmCasper			* g_pcsAgcmCasper		;

AcuFrameMemory		g_csAcuFrameMemory		;
//AgcmLoginGameServer	* g_pcsAgcmLoginGameServer;

// SetCallbackPostRender
BOOL	__AgcmRender_PostRenderCallback (PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( g_pEngine && g_pEngine->m_pCurrentFullUIModule )
		g_MainWindow.WindowRender();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyEngine::MyEngine()
{
	m_szImagePath = ".\\Texture\\;.\\Texture\\UI\\";
	m_bSequenceMode = TRUE;
}

MyEngine::~MyEngine()
{

}

BOOL		MyEngine::OnAttachPlugins	()	// 플러그인 연결 은 여기서
{
    /* 
     * Attach world plug-in...
     */
    if (!RpWorldPluginAttach())
    {
        return FALSE;
    }

	if (!RpMatFXPluginAttach())
	{
		return FALSE;
	}

	if (!RpHAnimPluginAttach())
	{
		return FALSE;
	}

	if (!RpSkinPluginAttach())
	{
		return FALSE;
	}

    if (!RpCollisionPluginAttach() )
    {
        return FALSE;
    }

	if (!RpSplinePluginAttach() )
	{
		return FALSE;
	}

	if(!RpMorphPluginAttach())
	{
		return FALSE;
	}

	if( !RpMTexturePluginAttach() )
	{
		return FALSE;
	}

	if( !RpUserDataPluginAttach() )
	{
		return FALSE;
	}

	if( !RpLODAtomicPluginAttach() )
	{
		return FALSE;
	}

	if (!RtAnimInitialize())
	{
		return FALSE;
	}

	return TRUE;
}

RpMaterial *Material(RpMaterial *material, void *data)
{
	return material;
}

BOOL MyEngine::OnRenderPreCameraUpdate()
{
//	* g_pcsAgcmShadow.ShadowTextureRender();
	return TRUE;
}

RpWorld *	MyEngine::OnCreateWorld		() // 월드를 생성해서 리턴해준다.
{
	return CreateWorld();
}

RwCamera *	MyEngine::OnCreateCamera	( RpWorld * pWorld )	// 메인 카메라를 Create해준다.
{
	m_fFarClipPlane = 50000.0f;
	m_fNearClipPlane = 100.0f;

	return CreateCamera( pWorld );
}

BOOL		MyEngine::OnCreateLight		()	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
{
	ASSERT( m_pWorld );
	//return AgcEngine::OnCreateLight();
	m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);
	
	float	ambiantvalue		= 0.3f;//0.4f;
	float	directionalvalue	= 0.7f;

    if (m_pLightAmbient)
    {
		RwRGBAReal color = {ambiantvalue, ambiantvalue, ambiantvalue, 1.0f};
		RpLightSetColor(m_pLightAmbient, &color);
		RpWorldAddLight(m_pWorld, m_pLightAmbient);
    }

    m_pLightDirect = RpLightCreate(rpLIGHTDIRECTIONAL);

    if (m_pLightDirect)
    {
        RwFrame *f = RwFrameCreate();
        RpLightSetFrame( m_pLightDirect , f);

		RwV3d v = { 0.0f , 0.0f , 0.0f };
		RwFrameTranslate	( f , & v ,	rwCOMBINEREPLACE );

		RwMatrix	* pMatrix	= RwFrameGetLTM	( f			);
		RwV3d		* pAt		= RwMatrixGetAt		( pMatrix	);
		pAt->x	= 0.556f;
		pAt->y	= -0.658f;
		pAt->z	= 0.056f;

		RwRGBAReal color = {(240.0f/255.0f), (202.0f/255.0f), (116.0f/255.0f), 1.0f};
		RpLightSetColor(m_pLightDirect, &color);

        RpWorld * pWorld = RpWorldAddLight(m_pWorld, m_pLightDirect);
    }

	return TRUE;
}

AgcWindow *	MyEngine::OnSetMainWindow	()	// 디폴트 윈도우를 설정해주는 메시지.. 윈도우를 초기화해서 포인터 넘기면 됀다.
{
	return &g_MainWindow;
}

BOOL MyEngine::OnRegisterModule()
{
	SetOneTimePopCount(10000);
//----> kday 20040917
//	SetMaxPacket(200000);
//<----

	g_AuCircularBuffer.Init(10000000);

	AgcEngine::SetRenderMode(FALSE, FALSE);

	REGISTER_MODULE( g_pcsAgpmTimer				, AgpmTimer				)
	REGISTER_MODULE( g_pcsAgpmFactors			, AgpmFactors			)
	REGISTER_MODULE( g_pcsApmMap				, ApmMap				)
	REGISTER_MODULE( g_pcsAgpmCharacter			, AgpmCharacter			)
	REGISTER_MODULE( g_pcsAgcmConnectManager	, AgcmConnectManager	)
	REGISTER_MODULE( g_pcsAgpmGrid				, AgpmGrid				)
	REGISTER_MODULE( g_pcsAgpmItem				, AgpmItem				)
	REGISTER_MODULE( g_pcsAgpmUnion				, AgpmUnion				)
	REGISTER_MODULE( g_pcsAgpmParty				, AgpmParty				)
	REGISTER_MODULE( g_pcsApmEventManager		, ApmEventManager		)
	REGISTER_MODULE( g_pcsAgpmSkill				, AgpmSkill				)
	REGISTER_MODULE( g_pcsAgpmCasper			, AgpmCasper			)
	REGISTER_MODULE( g_pcsAgpmItemConvert		, AgpmItemConvert		)
	REGISTER_MODULE( g_pcsAgpmGuild				, AgpmGuild				)
	//REGISTER_MODULE( g_pcsAgpmOptimizedPacket	, AgpmOptimizedPacket	)
	REGISTER_MODULE( g_pcsAgpmOptimizedPacket2	, AgpmOptimizedPacket2	)
	REGISTER_MODULE( g_pcsAgcmCasper			, AgcmCasper			)

	// module initialize
	if (g_pcsApmMap)
	{
		g_pcsApmMap->		SetLoadingMode		( TRUE , FALSE	);
		g_pcsApmMap->		SetAutoLoadData		( TRUE			);

		if( !g_pcsApmMap->Init( NULL ) )
			return FALSE;
	}

	if (g_pcsAgpmCharacter)
	{
		g_pcsAgpmCharacter->	SetMaxCharacterTemplate	( 1000							);
		g_pcsAgpmCharacter->	SetMaxCharacter			( 1000							);
		g_pcsAgpmCharacter->	SetMaxCharacterRemove	( 2000							);
	}

	if (g_pcsAgpmItem)
	{
		g_pcsAgpmItem->		SetMaxItem				( 1000							);
		g_pcsAgpmItem->		SetMaxItemTemplate		( 1000							);
	}

	return TRUE;
}

BOOL MyEngine::OnInit()
{
	return AgcEngine::OnInit();
}

VOID MyEngine::OnWorldRender()
{
}

BOOL MyEngine::OnCameraResize( RwRect * pRect )
{
	return AgcEngine::OnCameraResize( pRect );
}

VOID MyEngine::OnTerminate()
{
	g_csAcuFrameMemory.Release();

	AgcEngine::OnTerminate();
}

VOID	MyEngine::OnEndIdle()
{
	g_csAcuFrameMemory.Clear();
}

BOOL	MyEngine::SetSequenceLogin()
{
	m_bSequenceMode = TRUE;

	return TRUE;
}

BOOL	MyEngine::SetConcurrencyLogin()
{
	m_bSequenceMode = FALSE;
	return TRUE;
}

//	Callback 
//=============================================
//
BOOL	MyEngine::CB_EncryptCodeSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	CMainFrame *pMainWnd = (CMainFrame*)::AfxGetMainWnd();

	CString strTestID;
	INT16 ulNID = (INT16) pCustData;
	MyEngine* pThis = (MyEngine*)pClass;

	if (pThis->m_bSequenceMode)
		{
		strTestID.Format("%s_%04d", pMainWnd->m_strHigherID, pMainWnd->m_lBeginID + pMainWnd->m_lCurrent);
		}
	else
		{
		strTestID.Format("%s_%04d", pMainWnd->m_strHigherID, pMainWnd->m_lBeginID + ulNID);
		}

	TRACE("-----------------> Encrypt code success...Sending account(id=%s, NID=%d)\n", (LPCTSTR)strTestID, ulNID);
//	g_pcsAgcmLogin.SendAccount( const_cast<CHAR*>(strTestID.operator LPCTSTR()), "1111", ulNID );

	//##### 요거 여기서 하는거 좀 거시기하당...
	pMainWnd->m_lCurrent++;
	
	return TRUE;
	}

BOOL MyEngine::CB_LoginSucceeded(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	CMainFrame *pMainWnd = (CMainFrame*)::AfxGetMainWnd();
	CString strTestID;
	INT16 ulNID = (INT16) pCustData;
	MyEngine* pThis = (MyEngine*) pClass;

	if (pThis->m_bSequenceMode)
		strTestID.Format("%s_%04d", pMainWnd->m_strHigherID, pMainWnd->m_lBeginID + pMainWnd->m_lCurrent - 1);
	else
		strTestID.Format("%s_%04d", pMainWnd->m_strHigherID, pMainWnd->m_lBeginID + ulNID);

	TRACE("-----------------> Login succeeded... Sending GetUnion(id=%s, NID=%d)\n", (LPCTSTR) strTestID, ulNID);

	return TRUE;
	}


BOOL MyEngine::CB_GetUnionFinished(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	INT16 ulNID = (INT16)pCustData;
	CHAR *pszAccountID = (CHAR *)pData;

	TRACE("-----------------> Get union finished... Sending GetChars(id=%s, NID=%d)\n", pszAccountID, ulNID);
	
	return TRUE;
	}


BOOL MyEngine::CB_GetCharacterFinished(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	PVOID pvBuffer;	// 0 : CID, 1 : AccountID
	MyEngine* pThis = (MyEngine*)pClass;

	pvBuffer = pData;

	INT32 lCID = (INT32) ((PVOID *) pvBuffer)[0];
	CHAR *pszAccountID = (CHAR *) ((PVOID *) pvBuffer)[1];
	INT16 ulNID = (INT16) pCustData;

	if (0 == lCID)
		{
		TRACE("-----------------> Get chars finished... 근디 캐릭 없당...-_-; 끊는당\n");
		}
	else
		{
		TRACE("-----------------> Get chars finished... Sending SelectCharacter(cid=%d, NID=%d)\n",
											lCID, ulNID);
		}
	
	return TRUE;
	}


BOOL MyEngine::CB_SettingCharacterFinished(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	MyEngine* pThis = (MyEngine*)pClass;

	TRACE("-----------------> SettingCharacterFinished [%s]\n", pData ? (CHAR *) pData : "null");

	g_vtCharName.push_back((CHAR *) pData);
	g_pMainFrame->UpdateUserList(TRUE);

	CMainFrame *pMainWnd = (CMainFrame*)::AfxGetMainWnd();
	if (pMainWnd->m_lEndID < (pMainWnd->m_lBeginID + pMainWnd->m_lCurrent))
		{
		// 테스트 캐릭터 접속 중단
		g_pMainFrame->SetWindowText("접속 완료");
		}
	else
		{
		if (pThis->m_bSequenceMode)
			{
			// 테스트 캐릭터 접속 계속
			TRACE("다음 계정(%d] 쏩니다...\n", pMainWnd->m_lBeginID + pMainWnd->m_lCurrent);
			// PostMessage는 어떨까낭?
			pMainWnd->SetTimer(TIMER_SEQUENCE_LOGIN, TIMER_SEQUENCE_LOGIN_ELAPSE, NULL);
			}
		}
	return TRUE;
	}