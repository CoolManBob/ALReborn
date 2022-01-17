#include "AgpmEventPointLight.h"
#include "ApModuleStream.h"

AgpmEventPointLight::AgpmEventPointLight(void)
{
	SetModuleName("AgpmEventPointLight");
	m_pcsApmEventManager	= NULL;

	SetModuleData(sizeof(AgpdPointLight), AGPMPOINTLIGHT_DATA);
}

AgpmEventPointLight::~AgpmEventPointLight(void)
{
}

BOOL	AgpmEventPointLight::OnAddModule	()
{
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");

	if (!m_pcsApmEventManager)
		return FALSE;

	if (!m_pcsApmEventManager->RegisterEvent(
		APDEVENT_FUNCTION_POINTLIGHT,
		CBEventPointLightConstructor,
		CBEventPointLightDestructor	,
		NULL,
		CBEventPointLightStreamWrite,
		CBEventPointLightStreamRead	,
		this)
	)
		return FALSE;

	ApmObject *	pcsApmObject	= ( ApmObject * ) GetModule( "ApmObject" );
	ASSERT( NULL != pcsApmObject && "ApmObject 모듈을먼저 삽입하세요." );
	if( pcsApmObject )
	{
		// Add Object Callback 등록.
		pcsApmObject->SetCallbackAddObject		( CBOnAddObject		, this );
		pcsApmObject->SetCallbackRemoveObject	( CBOnRemoveObject	, this );
	}

	return TRUE;
}

BOOL	AgpmEventPointLight::CBEventPointLightConstructor	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventPointLight *	pThis = (AgpmEventPointLight *) pClass;
	ApdEvent *				pstDstEvent = (ApdEvent *) pData	;
	ApdEvent *				pstSrcEvent = (ApdEvent *) pCustData;

	pstDstEvent->m_pvData = pThis->CreateModuleData( AGPMPOINTLIGHT_DATA );
	if (!pstDstEvent->m_pvData)
		return FALSE;

	if (pstSrcEvent)
	{
		memcpy(pstDstEvent->m_pvData, pstSrcEvent->m_pvData, pThis->GetDataSize(AGPMPOINTLIGHT_DATA));
	}

	return TRUE;
}

BOOL	AgpmEventPointLight::CBEventPointLightDestructor	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventPointLight *	pThis		= (AgpmEventPointLight *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;

	pThis->DestroyModuleData(pstEvent->m_pvData, AGPMPOINTLIGHT_DATA);

	return TRUE;
}

BOOL	AgpmEventPointLight::CBEventPointLightStreamWrite	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventPointLight *	pThis		= (AgpmEventPointLight *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	AgpdPointLight *		pstPLight	= (AgpdPointLight *) pstEvent->m_pvData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;

	if (!pstStream->WriteValue(AGPMEVENTPOINTLIGHT_INI_NAME_START, 0))
		return FALSE;

	// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
	if (!pstStream->EnumWriteCallback(AGPMPOINTLIGHT_DATA, pstPLight, pThis))
		return FALSE;

	if (!pstStream->WriteValue(AGPMEVENTPOINTLIGHT_INI_NAME_END, 0))
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventPointLight::CBEventPointLightStreamRead	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventPointLight *	pThis		= (AgpmEventPointLight *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	AgpdPointLight *		pstPLight	= (AgpdPointLight *) pstEvent->m_pvData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;
	const CHAR *			szValueName;

	if (!pstStream->ReadNextValue())
		return TRUE;

	szValueName = pstStream->GetValueName();
	if (strcmp(szValueName, AGPMEVENTPOINTLIGHT_INI_NAME_START))
		return TRUE;

	// 다른 데이타가 없으므로 바로 콜백 호출.
	if (!pstStream->EnumReadCallback(AGPMPOINTLIGHT_DATA, pstPLight, pThis))
		return FALSE;

	szValueName = pstStream->GetValueName();
	if (strcmp(szValueName, AGPMEVENTPOINTLIGHT_INI_NAME_END))
	{
		// do nothing..
		TRACE( "AgpmEventPointLight::CBEventPointLightStreamRead 끝신호 감지 실패?..\n" );
	}

	return TRUE;
}

BOOL	AgpmEventPointLight::CBOnAddObject		( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventPointLight	*	pThis			= ( AgpmEventPointLight	* ) pClass	;
	ApdObject			*	pcsApdObject	= ( ApdObject			* ) pData	;

	ASSERT( NULL != pThis->m_pcsApmEventManager	);

	// 이벤트 스트럭쳐도 얻어냄..
	ApdEvent *	pstEvent	;
	pstEvent	= pThis->m_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_POINTLIGHT );

	if( pstEvent )
	{
		// 포인트 라이트 정보가 이쓴 경우.. 콜백 호출함..
		pThis->EnumCallback( AGPMEVENTPOINTLIGHT_ADDOBJECT , pcsApdObject , NULL );
	}

	return TRUE;
}

BOOL	AgpmEventPointLight::CBOnRemoveObject	( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventPointLight	*	pThis			= ( AgpmEventPointLight	* ) pClass	;
	ApdObject			*	pcsApdObject	= ( ApdObject			* ) pData	;

	ASSERT( NULL != pThis->m_pcsApmEventManager	);

	// 이벤트 스트럭쳐도 얻어냄..
	ApdEvent *	pstEvent	;
	pstEvent	= pThis->m_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_POINTLIGHT );

	if( pstEvent )
	{
		// 포인트 라이트 정보가 이쓴 경우.. 콜백 호출함..
		pThis->EnumCallback( AGPMEVENTPOINTLIGHT_REMOVEOBJECT , pcsApdObject , NULL );
	}

	return TRUE;
}

