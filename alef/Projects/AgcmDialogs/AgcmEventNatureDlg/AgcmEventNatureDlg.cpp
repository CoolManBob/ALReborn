// AgcmEventNatureDlg.cpp: implementation of the AgcmEventNatureDlg class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "../resource.h"
#include "AgcEngine.h"
#include "AgcmEventNatureDlg.h"
#include "EventNatureSettingDlg.h"
#include "TemplateSelectDlg.h"
#include "ClipPlaneSetDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#pragma message ( "Define State ::: _DEBUG" )
#else
#pragma message ( "Define State ::: Release Build" )
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmEventNatureDlg::AgcmEventNatureDlg()
{
	SetModuleName( "AgcmEventNautreDlg" );
	pResampleTextureFunction	= NULL;
	p__SetTimeFunction			= NULL;
	p__SetFogFunction			= NULL;
	p__MoveSky					= NULL;
	//p__SetSkyRadius				= NULL;
	p__OpenEffectList			= NULL;
	p__SetSkyTemplateID			= NULL;
	p__GetSoundLength			= NULL;

	m_pcsApmEventManager	= NULL;
	m_pcsApmEventManagerDlg	= NULL;
	m_pcsAgpmEventNature	= NULL;
	m_pcsAgcmEventNature	= NULL;
	m_pcsAgcmMap			= NULL;

	strcpy( m_strCurrentDirectory , "" );
}

AgcmEventNatureDlg::~AgcmEventNatureDlg()
{

}

BOOL	AgcmEventNatureDlg::OnAddModule()
{
	VERIFY( m_pcsApmEventManager	= (ApmEventManager *)		GetModule("ApmEventManager"		) );
	VERIFY( m_pcsApmEventManagerDlg	= (ApmEventManagerDlg *)	GetModule("ApmEventManagerDlg"	) );

	VERIFY( m_pcsAgpmEventNature	= (AgpmEventNature *)		GetModule("AgpmEventNature"	)	);
	VERIFY( m_pcsAgcmEventNature	= (AgcmEventNature *)		GetModule("AgcmEventNature"	)	);

	VERIFY( m_pcsAgcmMap			= (AgcmMap *)				GetModule("AgcmMap"	)			);

	if( NULL == m_pcsApmEventManager	|| NULL == m_pcsApmEventManagerDlg ||
		NULL == m_pcsAgpmEventNature	|| NULL == m_pcsAgcmEventNature	||
		NULL == m_pcsAgcmMap			)
	{
		TRACE( "AgcmEventNatureDlg 모듈 추가 실패!n" );
		return FALSE;
	}

	// Dialog 등록..
	if (!m_pcsApmEventManagerDlg->AddEventDialog( this , APDEVENT_FUNCTION_NATURE ) )
	{
		ASSERT( !"AgcmEventNatureDlg Dialog 추가실패!" );
		return FALSE;
	}

	return TRUE;
}

BOOL	AgcmEventNatureDlg::Open(ApdEvent *pstEvent)
{
	if (!pstEvent)
		return FALSE;

//	if (!pstEvent || !pstEvent->m_pvData)
//		return FALSE;

	// Dialog 띄우기..

	
	BOOL	bRet	= FALSE;

	CEventNatureSettingDlg	dlg;

	try
	{
		if( IDOK == dlg.DoModal( this , pstEvent ) )
			bRet	= TRUE	;
		else
			bRet	= FALSE	;
	}
	catch( ... )
	{
		if( dlg.IsWindowEnabled() ) dlg.DestroyWindow();
		::MessageBox( NULL , "Event Nature Dialog가 정상종료돼지 못했습니다." , "AgcmEventNature" , MB_ICONERROR );
	}

	return bRet;
}

BOOL	AgcmEventNatureDlg::OpenEditTemplateDialog( char * pToolDirectory )
{
	ASSERT( NULL != pToolDirectory			);
	ASSERT( NULL != m_pcsAgcmEventNature	);
	ASSERT( NULL != m_pcsAgpmEventNature	);
	
	ASSERT( NULL != pResampleTextureFunction	);
	ASSERT( NULL != p__SetTimeFunction			);
	ASSERT( NULL != p__SetFogFunction			);
	ASSERT( NULL != p__MoveSky					);
	//ASSERT( NULL != p__SetSkyRadius				);
	ASSERT( NULL != p__GetSoundLength				);
	
	strncpy( m_strCurrentDirectory , pToolDirectory , 1024 );

	CTemplateSelectDlg	dlg;
	if( IDOK == dlg.DoModal( this ) )
	{
		// 텍스쳐 저장..
		if( m_pcsAgcmEventNature->SaveTextureScriptFile() )
		{
			// Success
		}
		else
		{
			TRACE( "텍스쳐 스크립트파일 저장실패\n" );
			ASSERT( !"텍스쳐 스크립트파일 저장실패\n" );
			return FALSE;
		}

		// 스카이 템플릿 저장.
		char strTargetFilename[ 1024 ];
		wsprintf( strTargetFilename , "%s\\Ini\\%s" , pToolDirectory , AGPMNATURE_SKYSET_FILENAME );
		if( m_pcsAgpmEventNature->SkySet_StreamWrite( strTargetFilename , FALSE ) )
		{
			// Success
		}
		else
		{
			TRACE( "텍스쳐 스크립트파일 저장실패\n" );
			ASSERT( !"텍스쳐 스크립트파일 저장실패\n" );
			return FALSE;
		}
	}

	return TRUE;
}

//void	AgcmEventNatureDlg::SetRwEngineInstace( void * pRwEngineInstance )
//{
//	//RwEngineInstance	= pRwEngineInstance;
//}

BOOL	AgcmEventNatureDlg::OpenClipPlaneDlg		()
{
	ASSERT( NULL != m_pcsAgcmEventNature	);
	ASSERT( NULL != m_pcsAgpmEventNature	);
	
	ASSERT( NULL != pResampleTextureFunction	);
	ASSERT( NULL != p__SetTimeFunction			);
	ASSERT( NULL != p__SetFogFunction			);
	ASSERT( NULL != p__MoveSky					);

	CClipPlaneSetDlg	dlg;
	dlg.m_fFogDistance	= m_pcsAgcmEventNature->m_fFogDistance	;
	dlg.m_fFogFarClip	= m_pcsAgcmEventNature->m_fFogFarClip	;
	if( IDOK == dlg.DoModal( this ) )
	{
	}

	return TRUE;
}
