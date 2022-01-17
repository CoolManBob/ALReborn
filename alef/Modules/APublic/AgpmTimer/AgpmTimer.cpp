#include "AgpmTimer.h"

AgpmTimer::AgpmTimer()
{
	SetModuleName("AgpmTimer");
	EnableIdle2(TRUE);

	SetPacketType(AGPMTIMER_PACKET_TYPE);
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_UINT8,	1,				// Time Ratio
							AUTYPE_UINT64,	1,				// Time
							AUTYPE_END,		0	);

	m_ullInitialTime		= 0;
	m_ulInitialTickCount	= 0;
	m_bStopTimer			= FALSE;
	m_ullTime				= 0;
	m_ucTimeRatio			= 1;
	m_ucCurHour				= 0;
	m_ucSetCallbackHour		= 0;

	// 임시...
	m_ullInitialTime		= 1;
}

AgpmTimer::~AgpmTimer()
{
}

BOOL AgpmTimer::OnAddModule()
{
	return TRUE;
}

BOOL AgpmTimer::OnIdle2(UINT32 ulClockCount)
{
	PROFILE("AgpmTimer::OnIdle2");

// 마고자 (2004-06-10 오후 12:09:13) : 좀 이상한코드라서..
// 어차피 클라이언트엔 의미없는 코드라 삭제.
//	if(m_ullInitialTime)
	{
		if(!m_ulInitialTickCount) // 이니셜 타임이 설정된 상태에서만 .. 틱을 체크함..
		{
			m_ulInitialTickCount = ulClockCount;
		}
		else if(!m_bStopTimer)
		{
			m_ullTime	= m_ullInitialTime + ((ulClockCount - m_ulInitialTickCount) * m_ucTimeRatio);
			m_ucCurHour	= (UINT8)(GetHour(m_ullTime));

			if(m_ucCurHour != m_ucSetCallbackHour)
			{
				m_ucSetCallbackHour	= m_ucCurHour;
				EnumCallback(AGPM_TIMER_CB_ID_CHECK_HOUR, (PVOID)(m_ucSetCallbackHour), NULL);
			}
		}
		else
		{
			// 마고자 (2004-06-10 오후 1:05:45) : 멈추어있어도 업데이트는 됀다..
			m_ullTime	= m_ullInitialTime;
		}
	}

	return TRUE;
}

BOOL AgpmTimer::SetCallbackCheckHour(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_TIMER_CB_ID_CHECK_HOUR, pfCallback, pClass);
}

BOOL AgpmTimer::SetCallbackReceiveTime(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_TIMER_CB_ID_RECEIVE_TIME, pfCallback, pClass);
}

UINT32 AgpmTimer::GetYear(UINT64 time)
{
	return (UINT32)(time / AGPD_TIMER_YEAR_MILI_SECOND + AGPD_TIMER_INITIAL_YEAR);
}

UINT32 AgpmTimer::GetMonth(UINT64 time)
{
	return GetDay(time) / 30 + 1;
}

UINT32 AgpmTimer::GetDay(UINT64 time)
{
	return (UINT32)(	(time / AGPD_TIMER_DAY_MILI_SECOND) % 360	) + 1;
}

UINT32 AgpmTimer::GetHour(UINT64 time)
{
	return (UINT32)(	(time / AGPD_TIMER_HOUR_MILI_SECOND) % 24	);
}

UINT32 AgpmTimer::GetMinute(UINT64 time)
{
	return (UINT32)(	(time / AGPD_TIMER_MINUTE_MILI_SECOND) % 60	);
}

UINT32 AgpmTimer::GetSecond(UINT64 time)
{
	return (UINT32)(	(time / AGPD_TIMER_MILI_SECOND) % 60	);
}

UINT32 AgpmTimer::GetMS(UINT64 time)
{
	return (UINT32)(time % AGPD_TIMER_MILI_SECOND);
}

BOOL AgpmTimer::IsPM(UINT64 time)
{
	if(GetHour(time) <= 12)
		return FALSE;
	else
		return TRUE;
}

UINT32 AgpmTimer::GetDayTimeDWORD(UINT64 time)
{
	return (UINT32)(time % AGPD_TIMER_DAY_MILI_SECOND);
}

UINT32 AgpmTimer::GetHourTimeDWORD(UINT64 time)
{
	return (UINT32)(time % AGPD_TIMER_HOUR_MILI_SECOND);
}

UINT8 AgpmTimer::SetSpeedRate(UINT8 speed)
{
	UINT8 backup	= m_ucTimeRatio;
	m_ucTimeRatio	= speed;
	return backup;
}

UINT64 AgpmTimer::ConvertRealtoGameTime(UINT32 time)
{
	return (UINT64)(time / 1000 * m_ucTimeRatio);
}

UINT32 AgpmTimer::ConvertGametoRealTime(UINT64 time)
{
	return (UINT32)(time * 1000 / m_ucTimeRatio);
}

void AgpmTimer::SetTime(INT32 hour , INT32 minute)
{
	m_ullInitialTime = (hour * 60 + minute) * 60 * 1000;
	 
	m_ullTime	= m_ullInitialTime;
}

void AgpmTimer::SetTime(INT32 hour , INT32 minute, INT32 second, UINT64 *pullTime)
{
	m_ullInitialTime = (hour * 60 * 60 + minute * 60 + second) * 1000;
	m_ullTime	= m_ullInitialTime;

	if(!pullTime)
		return ;

	*pullTime = m_ullInitialTime;
}

BOOL AgpmTimer::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	UINT8		ucTimeRatio	= 0;
	UINT64		ullTime		= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&ucTimeRatio,
						&ullTime);

	if(ucTimeRatio)
	{
		m_ucTimeRatio			= ucTimeRatio;
	}

	if(ullTime)
	{
		m_ullInitialTime		= ullTime;
		m_ulInitialTickCount	= 0;

		m_ullTime				= ullTime;
	}

	EnumCallback(AGPM_TIMER_CB_ID_RECEIVE_TIME, NULL, NULL);

	return TRUE;
}

PVOID	AgpmTimer::MakePacketTime(INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMTIMER_PACKET_TYPE,
								&m_ucTimeRatio,
								&m_ullTime);
}

/******************************************************************************
******************************************************************************/