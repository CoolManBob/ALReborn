#pragma once

// 마고자 (2005-05-19 오전 11:32:44) : 
// 포인트라이트 정보 붙이기 인터페이스.

#include "ApmEventManager.h"

#define AGPMEVENTPOINTLIGHT_INI_NAME_START			"PointLightStart"
#define AGPMEVENTPOINTLIGHT_INI_NAME_END			"PointLightEnd"

typedef enum
{
	AGPMPOINTLIGHT_DATA		= 0,
} AgpmPointLightData;

// 스카이 정보에 대한 스트럭쳐..
class AgpdPointLight : public ApBase
{
public:
	// nothing..

	/*
	UINT8	uRed		;
	UINT8	uGreen		;
	UINT8	uBlue		;
	FLOAT	fRadius		;
	UINT32	uEnableFlag	;

	// 0번째 비트는 0시
	// 1번째 비트는 1시
	// ...
	// 23번째 비트는 23시
	*/
};

enum AgpmEventPointLightCallbackPoint			// Callback ID
{
	AGPMEVENTPOINTLIGHT_ADDOBJECT		= 0	,			
	AGPMEVENTPOINTLIGHT_REMOVEOBJECT	= 1
};

class AgpmEventPointLight : public ApModule 
{
public:
	ApmEventManager*		m_pcsApmEventManager;

public:
	AgpmEventPointLight(void);
	~AgpmEventPointLight(void);

public:
	// 라이트가 로딩될때..
	BOOL	SetCallback_AddLight(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
		return SetCallback(AGPMEVENTPOINTLIGHT_ADDOBJECT, pfCallback, pClass);
	}
	// 라이트가 제거될때..
	BOOL	SetCallback_RemoveLight(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
		return SetCallback(AGPMEVENTPOINTLIGHT_REMOVEOBJECT, pfCallback, pClass);
	}

public:
	virtual BOOL	OnAddModule	();

	// Callback 등록과 Data Attach API
	INT16	AttachPointLightData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
	{
		return SetAttachedModuleData( pClass , AGPMPOINTLIGHT_DATA , nDataSize , pfConstructor , pfDestructor );
	}

// Event Streaming
	static BOOL		CBEventPointLightConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventPointLightDestructor	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventPointLightStreamWrite(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventPointLightStreamRead	(PVOID pData, PVOID pClass, PVOID pCustData);

	// Add & Remove
	static BOOL	CBOnAddObject		( PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBOnRemoveObject	( PVOID pData, PVOID pClass, PVOID pCustData);
};
