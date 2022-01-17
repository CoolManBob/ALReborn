// AgpmEventNature.h: interface for the AgpmEventNature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPMEVENTNATURE_H__C1DA7534_57D8_4F8D_AE5A_2477C295C735__INCLUDED_)
#define AFX_AGPMEVENTNATURE_H__C1DA7534_57D8_4F8D_AE5A_2477C295C735__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApModule.h"
#include "AuPacket.h"
#include "AuList.h"

#include "ApmEventManager.h"
#include "AgpmTimer.h"

#include "AgpdEventNature.h"


//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventNatureD" )
#else
#pragma comment ( lib , "AgpmEventNature" )
#endif
#endif
//@} Jaewon


//#define	AGPMEVENT_NATURE_PACKET_TYPE		(APMEVENT_FLAG_PACKET_TYPE | APDEVENT_FUNCTION_NATURE)
#define AGPMNATURE_SKYSET_FILENAME						"skyset.ini"

//#define AGPMNATURE_STREAM_NAME_NATURE					"Nature"

//#define AGPMNATURE_STREAM_NAME_WEATHER					"Weather"
//#define AGPMNATURE_STREAM_NAME_WEATHER_MIN_DELAY		"WeatherMinDelay"
//#define AGPMNATURE_STREAM_NAME_WEATHER_MAX_DELAY		"WeatherMaxDelay"
//#define AGPMNATURE_STREAM_NAME_TIME						"Time"
//#define AGPMNATURE_STREAM_NAME_TIME_RATIO				"TimeRatio"

#define AGPMNATURE_INI_NAME_START						"NatureStart"
#define AGPMNATURE_INI_NAME_END							"NatureEnd"
#define AGPMNATURE_INI_NAME_SKYTEMPLATEID				"SkyTemplateID"

// 최초의 년도.
//#define	AGPMNATURE_INITIAL_YEAR							2003
// 1 년은 360일. .1달 30일 고정.

typedef enum AgpmEventNatureCallbackPoint			// Callback ID
{
	AGPMEVENT_TELEPORT_CB_ID_NATURE			= 0	,
	AGPMEVENT_SKY_CB_ADD						,
	AGPMEVENT_SKY_CB_REMOVE							
} AgpmEventNatureCallbackPoint;

class AgpmEventNature : public ApModule  
{
public:

	// Constant!!
//	static const UINT64	c_YearMiliSecond	;	// 1000 * 60 * 60 * 24 * 360	;	// 밀리초 * 초 * 분 * 시 * 날
//	static const UINT32	c_DayMiliSecond		;	// 밀리초 * 초 * 분 * 시
//	static const UINT32	c_HourMiliSecond	;	// 밀리초 * 초 * 분
//	static const UINT32	c_MinuteMiliSecond	;	// 밀리초 * 초
//	static const UINT32	c_MiliSecond		;	// 밀리초
	
protected:
	ApmEventManager	*	m_pcsApmEventManager	;
	AgpmTimer		*	m_pcsAgpmTimer			;
	AgpmCharacter	*	m_pcsAgpmCharacter		;
//	UINT64				m_ullInitialTime		;
//	UINT32				m_ulInitialTickCount	;	// 최초의 tick count를 저장함..
	
//	BOOL				m_bStopTimer			;	// 시계야 멈춰라!..

public:
	AgpdNature			m_csNature				;
	AuPacket			m_csPacket				;

	AgpmEventNature();
	virtual ~AgpmEventNature();

	// Virtual Function 들
	BOOL			OnAddModule		();
	BOOL			OnInit			();
	BOOL			OnDestroy		();
	BOOL			OnReceive		(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL			OnIdle			(UINT32 ulClockCount);

	// Setting Function 들
	BOOL			SetWeather		(AgpdNatureWeatherType eWeather);

	// Callback Function 들
	BOOL			SetCallbackNature(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// Stream Function 들
//	BOOL			StreamWrite		(CHAR *szFile);
//	BOOL			StreamRead		(CHAR *szFile);

	///////////////////////////////////////////////////////////////////////////
	// Modify(BOB, 061003)
	UINT8	SetSpeedRate(UINT8 speed);
	UINT8	GetSpeedRate();

	UINT64	ConvertRealtoGameTime(UINT32 time);
	UINT32	ConvertGametoRealTime(UINT64 time);

	BOOL	GetTimer();
	void	StartTimer();
	void	StopTimer();
	void	ToggleTimer();

	void	SetTime(INT32 hour, INT32 minute = 0);
	void	SetTime(INT32 hour, INT32 minute, INT32 second, UINT64 *pullTime);

	BOOL	IsPM(UINT64 time);

	UINT64	GetGameTime();
	UINT32	GetYear(UINT64 time);
	UINT32	GetMonth(UINT64 time);
	UINT32	GetDay(UINT64 time);
	UINT32	GetHour(UINT64 time);	
	UINT32	GetMinute(UINT64 time);
	UINT32	GetSecond(UINT64 time);
	UINT32	GetMS(UINT64 time);

	UINT32	GetDayTimeDWORD(UINT64 time);
	UINT32	GetHourTimeDWORD(UINT64 time);

	UINT32	GetDayMiliSecond();
	///////////////////////////////////////////////////////////////////////////

public:
	// SkySetting 에 관한것..
	AuList< AgpdSkySet * >	m_listSkySet;

	INT16			AttachSkySetData	(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor = NULL, ApModuleDefaultCallBack pfDestructor = NULL);

	// Callback Functions...
	BOOL			SetCallback_AddSky				(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallback_RemoveSky			(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL		SkySetReadCB	(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL		SkySetWriteCB	(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	
	// SkySetting 얻어내는것..
	AgpdSkySet *	GetSkySet		( INT32	nIndex	);
	AuList< AgpdSkySet * > * GetSkySetList	() { return & m_listSkySet; }

	// Stream Function..
	BOOL			SkySet_StreamWrite		(CHAR *szFile, BOOL bEncryption);
	BOOL			SkySet_StreamRead		(CHAR *szFile, BOOL bDecryption);

	
	// 메모리 할당 펑션..
	AgpdSkySet *	CreateSkySet	();// 빈녀석 생성..
	BOOL			DeleteSkySet	( AgpdSkySet * pSkySet	);	// 스카이를 제거하는 함수가 아니고 할당된 메모리를 지우는 녀석이다.

	BOOL			AddDefaultSkySet	();	// 디폴트를 넣는 것으로..AgcmEventNature 를 add한당므에 호출해준다.
	BOOL			AddSkySet		( INT32 nIndex , char * pStrName		);
	BOOL			AddSkySet		( AgpdSkySet * pSkySet	);
		// 인덱스는 0부터..
	BOOL			RemoveSkySet	( INT32 nIndex			);
	BOOL			RemoveSkySet	( AgpdSkySet * pSkySet	);
	void			RemoveAllSkySet	();
	
	INT32			GetEmptySkySetIndex		();
	
public:
	// Event Streaming
	static BOOL		CBEventNatureConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventNatureDestructor	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventNatureStreamWrite(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventNatureStreamRead	(PVOID pData, PVOID pClass, PVOID pCustData);
};



#endif // !defined(AFX_AGPMEVENTNATURE_H__C1DA7534_57D8_4F8D_AE5A_2477C295C735__INCLUDED_)
