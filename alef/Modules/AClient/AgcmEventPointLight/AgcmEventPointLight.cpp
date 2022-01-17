#include "agcmeventpointlight.h"
#include "AgcmUIConsole.h"
#include "AgcmEventNature.h"
#include "ApModuleStream.h"


AgcmEventPointLight::AgcmEventPointLight(void) :
	m_bEnablePointLight( TRUE )
{
	SetModuleName("AgcmEventPointLight");
	m_nPointLightAttachIndex	= 0;

	m_pcsApmEventManager		= NULL;
	m_pcsAgpmEventPointLight	= NULL;

	m_pcsAgcmRender				= NULL;
	m_pcsAgcmDynamicLightmap	= NULL;

	m_bReturnLoginLobby			= FALSE;
}

AgcmEventPointLight::~AgcmEventPointLight(void)
{
}

AgcdPointLight *	AgcmEventPointLight::GetPointLightClientData( AgpdPointLight	* pstPLight )
{
	return (AgcdPointLight *) m_pcsAgpmEventPointLight->GetAttachedModuleData(
		m_nPointLightAttachIndex, pstPLight);
}

BOOL	AgcmEventPointLight::OnAddModule	()
{
	m_pcsApmEventManager		= ( ApmEventManager		* ) GetModule( "ApmEventManager"		);
	m_pcsAgpmEventPointLight	= ( AgpmEventPointLight	* ) GetModule( "AgpmEventPointLight"	);
	m_pcsAgcmRender				= ( AgcmRender			* ) GetModule( "AgcmRender"				);
	m_pcsAgcmDynamicLightmap	= ( AgcmDynamicLightmap	* ) GetModule( "AgcmDynamicLightmap"	);

	ASSERT( NULL != m_pcsApmEventManager		);
	ASSERT( NULL != m_pcsAgpmEventPointLight	);
	ASSERT( NULL != m_pcsAgcmRender				);
	ASSERT( NULL != m_pcsAgcmDynamicLightmap	);

	if( NULL == m_pcsApmEventManager		) return FALSE;
	if( NULL == m_pcsAgpmEventPointLight	) return FALSE;
	if( NULL == m_pcsAgcmRender				) return FALSE;
	if( NULL == m_pcsAgcmDynamicLightmap	) return FALSE;

	AgcmEventNature * pcsEventNature = ( AgcmEventNature * ) GetModule( "AgcmEventNature" );
	if (pcsEventNature && !pcsEventNature->SetCallbackSkyChange( CBSkyChange, this ))
		return FALSE;

	{
		m_nPointLightAttachIndex = m_pcsAgpmEventPointLight->AttachPointLightData( 
			this , 
			sizeof AgcdPointLight,
			AgcmEventPointLight::OnPointLightCreate , 
			AgcmEventPointLight::OnPointLightDestroy );

		if (!m_pcsAgpmEventPointLight->AddStreamCallback(
			AGPMPOINTLIGHT_DATA, 
			AgcmEventPointLight::PointLightStreamReadCB, 
			AgcmEventPointLight::PointLightStreamWriteCB, 
			this) )
		{
			TRACE( "m_pcsAgpmEventPointLight->AddStreamCallback 등록실패.\n" );
			return FALSE;
		}

		// 마고자 (2005-05-19 오후 5:23:01) : //
		// 콜백등록..
		m_pcsAgpmEventPointLight->SetCallback_AddLight		( CBOnPointLightAddObject		, this );
		m_pcsAgpmEventPointLight->SetCallback_RemoveLight	( CBOnPointLightRemoveObject	, this );
	}

	return TRUE;
}

void	AgcmEventPointLight::TogglePointLight()
{
	EnablePointLight( !m_bEnablePointLight );
}

BOOL	AgcmEventPointLight::OnInit()
{
	AS_REGISTER_TYPE_BEGIN(AgcmEventPointLight, AgcmEventPointLight);
		AS_REGISTER_METHOD0(void, TogglePointLight);
	AS_REGISTER_TYPE_END;

	return TRUE;
}

BOOL	AgcmEventPointLight::OnPointLightCreate		(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventPointLight	*pThis			= (AgcmEventPointLight *)(pClass);
	AgpdPointLight		*pstAgpdPLight	= (AgpdPointLight *)(pData);
	AgcdPointLight		*pstAgcdPLight	= pThis->GetPointLightClientData(pstAgpdPLight);

	memset( pstAgcdPLight , 0 , sizeof(AgcdPointLight) );

	// 마고자 (2005-05-20 오전 10:35:43) : 
	// 라이트 초기값 설정..
	pstAgcdPLight->nEnableFlag	= AGCPLF_DEFAULT;
	pstAgcdPLight->pLight		= NULL			;
	pstAgcdPLight->uRed			= 255			;
	pstAgcdPLight->uGreen		= 255			;
	pstAgcdPLight->uBlue		= 255			;
	pstAgcdPLight->fRadius		= 2000.0f		;

	return TRUE;
}

BOOL	AgcmEventPointLight::OnPointLightDestroy	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventPointLight	*pThis			= (AgcmEventPointLight *)(pClass);
	AgpdPointLight		*pstAgpdPLight	= (AgpdPointLight *)(pData);
	AgcdPointLight		*pstAgcdPLight	= pThis->GetPointLightClientData(pstAgpdPLight);

	if( pstAgcdPLight->pLight )
	{
		// 마고자 (2005-05-19 오후 2:55:47) : 
		// 라이트 제거 안됐네.. 처리..
	}

	return TRUE;
}

#define	AGCMPL_RGB		"Color"
#define	AGCMPL_RADIUS	"Radius"
#define	AGCMPL_FLAG		"Flag"

BOOL	AgcmEventPointLight::PointLightStreamWriteCB	( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	AgcmEventPointLight	*pThis			= (AgcmEventPointLight *)(pClass);
	AgpdPointLight		*pstAgpdPLight	= (AgpdPointLight *)(pData);
	AgcdPointLight		*pstAgcdPLight	= pThis->GetPointLightClientData(pstAgpdPLight);
	
	if (!pcsStream->WriteValue( AGCMPL_RGB , ( INT32 ) RGB( pstAgcdPLight->uRed , pstAgcdPLight->uGreen , pstAgcdPLight->uBlue ) ) )
		return FALSE;
	if (!pcsStream->WriteValue( AGCMPL_RADIUS , pstAgcdPLight->fRadius ) )
		return FALSE;
	if (!pcsStream->WriteValue( AGCMPL_FLAG , pstAgcdPLight->nEnableFlag ) )
		return FALSE;

	return TRUE;
}

BOOL	AgcmEventPointLight::PointLightStreamReadCB	( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	// 마고자 (2005-05-19 오후 5:10:18) : 
	// 데이타 읽는 과정..

	AgcmEventPointLight	*pThis			= (AgcmEventPointLight *)(pClass);
	AgpdPointLight		*pstAgpdPLight	= (AgpdPointLight *)(pData);
	AgcdPointLight		*pstAgcdPLight	= pThis->GetPointLightClientData(pstAgpdPLight);

	const CHAR *szValueName;

	while(pcsStream->ReadNextValue())
	{
		szValueName = pcsStream->GetValueName();
		if (!strncmp( szValueName , AGCMPL_RGB , 10 ) )
		{
			INT32	nRGB;
			pcsStream->GetValue( &nRGB );
			pstAgcdPLight->uRed		= GetRValue( nRGB );
			pstAgcdPLight->uGreen	= GetGValue( nRGB );
			pstAgcdPLight->uBlue	= GetBValue( nRGB );
		}
		else
		if (!strncmp( szValueName , AGCMPL_RADIUS , 10 ) )
		{
			pcsStream->GetValue( &pstAgcdPLight->fRadius );
		}
		else
		if (!strncmp( szValueName , AGCMPL_FLAG , 10 ) )
		{
			pcsStream->GetValue( &pstAgcdPLight->nEnableFlag );
		}
	}

	return TRUE;
}

BOOL	AgcmEventPointLight::CBOnPointLightAddObject		( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventPointLight	*pThis			= ( AgcmEventPointLight * )(pClass);
	ApdObject			*pcsApdObject	= ( ApdObject			* ) pData	;

	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsApdObject );

	// 이벤트 스트럭쳐도 얻어냄..
	ApdEvent *	pstEvent	;
	pstEvent	= pThis->m_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_POINTLIGHT );

	if( pstEvent )
	{
		AgpdPointLight		*pstAgpdPLight	= (AgpdPointLight *)(pstEvent->m_pvData);
		AgcdPointLight		*pstAgcdPLight	= pThis->GetPointLightClientData(pstAgpdPLight);

		if( pThis->AddLightToArray( pstAgcdPLight , pcsApdObject ) && pThis->m_bEnablePointLight )
		{
			pThis->AddLight( pstAgcdPLight , &pcsApdObject->m_stPosition );
		}
	}

	return TRUE;
}

BOOL	AgcmEventPointLight::CBOnPointLightRemoveObject		( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventPointLight	*pThis			= ( AgcmEventPointLight * )(pClass);
	ApdObject			*pcsApdObject	= ( ApdObject			* ) pData	;

	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsApdObject );

	// 이벤트 스트럭쳐도 얻어냄..
	ApdEvent *	pstEvent	;
	pstEvent	= pThis->m_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_POINTLIGHT );

	if( pstEvent )
	{
		AgpdPointLight		*pstAgpdPLight	= (AgpdPointLight *)(pstEvent->m_pvData);
		AgcdPointLight		*pstAgcdPLight	= pThis->GetPointLightClientData(pstAgpdPLight);

		if( pThis->RemoveLightToArray( pstAgcdPLight ) && pThis->m_bEnablePointLight )
		{
			pThis->RemoveLight( pstAgcdPLight );
		}
	}

	return TRUE;
}

BOOL		AgcmEventPointLight::AddLightToArray	( AgcdPointLight * pcsAgcdPLight , ApdObject * pcsApdObject )
{
	vector <LightObjectInfo>::iterator Iter;

	for ( Iter = m_vectorPointLight.begin( ) ; Iter != m_vectorPointLight.end( ) ; Iter++ )
	{
		if( (*Iter).pstAgcdPLight == pcsAgcdPLight )
		{
			// 중복!
			ASSERT( !"AgcmEventPointLight::AddLightToArray" );
			return FALSE;
		}
	}

	LightObjectInfo	stInfo;
	stInfo.pcsApdObject		= pcsApdObject	;
	stInfo.pstAgcdPLight	= pcsAgcdPLight	;
		
	m_vectorPointLight.push_back( stInfo );
	return TRUE;
}

BOOL		AgcmEventPointLight::RemoveLightToArray	( AgcdPointLight * pcsAgcdPLight )
{
	vector <LightObjectInfo>::iterator Iter;

	for ( Iter = m_vectorPointLight.begin( ) ; Iter != m_vectorPointLight.end( ) ; Iter++ )
	{
		if( (*Iter).pstAgcdPLight == pcsAgcdPLight )
		{
			m_vectorPointLight.erase( Iter );
			return TRUE;
		}
	}

	// 없다!
	ASSERT( !"AgcmEventPointLight::RemoveLightToArray" );
	return FALSE;
}

BOOL		AgcmEventPointLight::AddLight		( AgcdPointLight * pcsAgcdPLight , AuPOS * pPos )
{
	if( NULL == pcsAgcdPLight->pLight )
	{
		// 라이트 하나 추가
		RpLight	* pLight = RpLightCreate( rpLIGHTPOINT );
		RwRGBAReal	rgb;

		rgb.red		= ( FLOAT ) pcsAgcdPLight->uRed		/ 255.0f;
		rgb.green	= ( FLOAT ) pcsAgcdPLight->uGreen	/ 255.0f;
		rgb.blue	= ( FLOAT ) pcsAgcdPLight->uBlue	/ 255.0f;
		rgb.alpha	= 1.0f;

		RpLightSetColor		( pLight , &rgb						);
		RpLightSetRadius	( pLight , pcsAgcdPLight->fRadius	);

		RwFrame	* pFrame = RwFrameCreate();
		RpLightSetFrame( pLight , pFrame );

		RwV3d	pos;
		pos.x	= pPos->x;
		pos.z	= pPos->z;
		pos.y	= pPos->y;

		RwFrameTranslate( pFrame , &pos , rwCOMBINEREPLACE );

		// m_listLight.push_back( pLight );

		// 마고자 (2005-05-20 오전 10:17:29) : 
		// 맵툴빌드에서만 라이트가 이동 가능하다.
		#ifdef USE_MFC
		BOOL	bMovable = TRUE;
		#else
		BOOL	bMovable = FALSE;
		#endif

		if( pcsAgcdPLight->nEnableFlag & AGCPLF_AFFECT_OBJECT )
		{
			m_pcsAgcmRender->AddLightToRenderWorld( pLight , bMovable ) ;
		}
		if( pcsAgcdPLight->nEnableFlag & AGCPLF_AFFECT_GEOMETRY )
		{
			m_pcsAgcmDynamicLightmap->addLight( pLight , bMovable );
		}

		// 변수 설정
		pcsAgcdPLight->pLight = pLight;
	}
	else
	{
		// 이미 추가돼있네? -,.-
	}
			
	return TRUE;
}

BOOL		AgcmEventPointLight::RemoveLight	( AgcdPointLight * pcsAgcdPLight )
{
	if( NULL != pcsAgcdPLight->pLight )
	{
		RwFrame	* pFrame;

		pFrame = RpLightGetFrame( pcsAgcdPLight->pLight );

		// 프레임제거..
		RpLightSetFrame( pcsAgcdPLight->pLight , NULL );
		RwFrameDestroy( pFrame );

		// 마고자 (2005-05-20 오전 10:17:29) : 
		// 맵툴빌드에서만 라이트가 이동 가능하다.
		#ifdef USE_MFC
		BOOL	bMovable = TRUE;
		#else
		BOOL	bMovable = FALSE;
		#endif

		if( pcsAgcdPLight->nEnableFlag & AGCPLF_AFFECT_OBJECT )
		{
			m_pcsAgcmRender->RemoveLightFromRenderWorld( pcsAgcdPLight->pLight , bMovable );
		}
		if( pcsAgcdPLight->nEnableFlag & AGCPLF_AFFECT_GEOMETRY )
		{
			m_pcsAgcmDynamicLightmap->removeLight( pcsAgcdPLight->pLight );
		}

		RpLightDestroy( pcsAgcdPLight->pLight );
		// 변수 설정
		pcsAgcdPLight->pLight = NULL;
	}
	else
	{
		// 이미 없는데? -_-;;;
	}

	return TRUE;
}

void	AgcmEventPointLight::EnablePointLight( BOOL bEnable )
{
	if( m_bEnablePointLight == bEnable ) return;

	// 
	vector <LightObjectInfo>::iterator Iter;

	if( m_bEnablePointLight )
	{
		// 올라간거 다죽임..

		for ( Iter = m_vectorPointLight.begin( ) ; Iter != m_vectorPointLight.end( ) ; Iter++ )
		{
			RemoveLight( (*Iter).pstAgcdPLight );
		}
	}
	else
	{
		for ( Iter = m_vectorPointLight.begin( ) ; Iter != m_vectorPointLight.end( ) ; Iter++ )
		{
			AddLight( (*Iter).pstAgcdPLight ,  &( (*Iter).pcsApdObject->m_stPosition ) );
		}
	}

	m_bEnablePointLight = bEnable;
}


BOOL	AgcmEventPointLight::CBSkyChange ( PVOID	pData, PVOID pClass, PVOID pCustData )
{
	AgcmEventPointLight	* pThis		= (AgcmEventPointLight	*) pClass	;
	ASkySetting			* pSkySet	= (ASkySetting			*) pData	;

	ASSERT( NULL != pThis	);
	ASSERT( NULL != pSkySet	);

	// 스카이설정으로 렌즈 플레어 온오프
	BOOL	bEnablePointLight = TRUE;
	if( pSkySet->nWeatherEffect1 & ASkySetting::DISABLE_POINT_LIGHT )
	{
		bEnablePointLight = FALSE	;
	}
	else
	{
		bEnablePointLight = TRUE	;
	}

	pThis->EnablePointLight( bEnablePointLight );

	return	TRUE;
}