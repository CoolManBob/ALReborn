#ifndef __AGPM_TIMER_H__
#define __AGPM_TIMER_H__

#include "ApModule.h"
#include "ApDefine.h"
#include "AuPacket.h"
#include "AgpdTimer.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmTimerD" )
#else
#pragma comment ( lib , "AgpmTimer" )
#endif // _DEBUG
#endif

typedef enum
{
	AGPM_TIMER_CB_ID_CHECK_HOUR			= 0,
	AGPM_TIMER_CB_ID_RECEIVE_TIME,
	AGPM_TIMER_CB_ID_NUM
} AgpmTimerCallbackPoint;

#pragma pack(1)
class AgpmTimer : public ApModule
{
protected:
	UINT64		m_ullInitialTime;
	UINT32		m_ulInitialTickCount;

	BOOL		m_bStopTimer;

	UINT64		m_ullTime;
	UINT8		m_ucTimeRatio;

	UINT8		m_ucCurHour;			// 현재 시간
	UINT8		m_ucSetCallbackHour;	// 시간대별 콜백을 위한 변

public:
	AuPacket	m_csPacket;

	AgpmTimer();
	virtual ~AgpmTimer();

protected:
	BOOL	OnAddModule();
	BOOL	OnIdle2(UINT32 ulClockCount);
	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

public:
	UINT8	SetSpeedRate(UINT8 speed);
	UINT8	GetSpeedRate()	{return m_ucTimeRatio;}

	UINT64	ConvertRealtoGameTime(UINT32 time);
	UINT32	ConvertGametoRealTime(UINT64 time);

	BOOL	TimerIsStop()	{return	m_bStopTimer;}
	void	StartTimer()	{m_bStopTimer = FALSE;}
	void	StopTimer()		{m_bStopTimer = TRUE;}
	void	ToggleTimer()	{m_bStopTimer = !m_bStopTimer;}

	void	SetTime(INT32 hour, INT32 minute = 0);
	void	SetTime(INT32 hour, INT32 minute, INT32 second, UINT64 *pullTime);
	
	UINT64	GetGameTime()	{return m_ullTime;}
	UINT8	GetCurYear()	{return GetYear(m_ullTime);}
	UINT8	GetCurMonth()	{return GetMonth(m_ullTime);}
	UINT8	GetCurDay()	{return GetDay(m_ullTime);}
	UINT8	GetCurHour()	{return GetHour(m_ullTime);}
	UINT8	GetCurMinute()	{return GetMinute(m_ullTime);}
	UINT8	GetCurSecond()	{return GetSecond(m_ullTime);}

	BOOL	IsPM(UINT64 time);

	UINT32	GetYear(UINT64 time);
	UINT32	GetMonth(UINT64 time);
	UINT32	GetDay(UINT64 time);
	UINT32	GetHour(UINT64 time);	
	UINT32	GetMinute(UINT64 time);
	UINT32	GetSecond(UINT64 time);
	UINT32	GetMS(UINT64 time);

	UINT32	GetDayTimeDWORD(UINT64 time);
	UINT32	GetHourTimeDWORD(UINT64 time);

	UINT32	GetDayMiliSecond()	{return AGPD_TIMER_DAY_MILI_SECOND;}

	// Callback
	BOOL	SetCallbackCheckHour(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReceiveTime(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	PVOID	MakePacketTime(INT16 *pnPacketLength);
};

#pragma pack()
#endif // __AGPM_TIMER_H__