// AgsmEventNature.cpp: implementation of the AgsmEventNature class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmEventNature.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmEventNature::AgsmEventNature()
{
	SetModuleName("AgsmEventNature");

	EnableIdle2(TRUE);

	SetModuleType(APMODULE_TYPE_SERVER);

	m_ulNextWeatherClock = 0;
	m_eOriginalWeatherType = AGPDNATURE_WEATHER_TYPE_NONE;
	m_eCurrentWeatherType = AGPDNATURE_WEATHER_TYPE_NONE;
}

AgsmEventNature::~AgsmEventNature()
{

}

BOOL	AgsmEventNature::OnAddModule()
{
	// 상위 Module들 가져온다.
	m_pcsAgpmEventNature	= (AgpmEventNature	*)	GetModule("AgpmEventNature");
	m_pcsAgpmTimer			= (AgpmTimer		*)	GetModule("AgpmTimer");

	if ((!m_pcsAgpmEventNature) || (!m_pcsAgpmTimer))
		return FALSE;

	// Callback을 Set한다.
	if (!m_pcsAgpmEventNature->SetCallbackNature(CBNature, this))
		return FALSE;

	return TRUE;
}

// 2006.11.08. steeple
// 싹 다 바꼈다.
BOOL	AgsmEventNature::OnIdle2(UINT32 ulClockCount)
{
	if(m_ulNextWeatherClock == 0)
		return TRUE;

	if(ulClockCount > m_ulNextWeatherClock)
	{
		// 원래 것으로 돌려준다.
		SetWeatherWithDuration(m_eOriginalWeatherType, 0);
	}

	return TRUE;


//	UINT64	ullThisTime = m_pcsAgpmTimer->GetGameTime();
//
//	// 현재 시간이 날씨 변경 시간이 되면.
////	if (m_ullNextWeatherTime && m_pcsAgpmEventNature->m_csNature.m_ullTime >= m_ullNextWeatherTime)
//	if (m_ullNextWeatherTime && ullThisTime >= m_ullNextWeatherTime)
//	{
//		// 새로운 날씨를 random하게 계산해서 적용하고, 다음 날씨 변경 시간을 계산한다.
//		AgpdNatureWeatherType eWeather = (AgpdNatureWeatherType) (rand() % (AGPDNATURE_WEATHER_TYPE_MAX - 1) + 1);
//
//		m_pcsAgpmEventNature->SetWeather(eWeather);
//
//		//m_ullNextWeatherTime = m_pcsAgpmTimer->GetGameTime() + m_pcsAgpmEventNature->m_csNature.m_ullWeatherMinDelay +
//		m_ullNextWeatherTime = ullThisTime + m_pcsAgpmEventNature->m_csNature.m_ullWeatherMinDelay +
//							   (rand() % (m_pcsAgpmEventNature->m_csNature.m_ullWeatherMaxDelay - m_pcsAgpmEventNature->m_csNature.m_ullWeatherMinDelay));
//	}
//
//	return TRUE;
}

// 2006.11.08. steeple
// 일정시간동안 날씨를 바꿔주는 함수
BOOL	AgsmEventNature::SetWeatherWithDuration(AgpdNatureWeatherType eWeatherType, UINT32 ulDuration)
{
	// 걍 무조건 바꾼다.
	m_eCurrentWeatherType = eWeatherType;
	
	if(ulDuration == 0)
		m_ulNextWeatherClock = 0;	// 이렇게 되면 영원히 바뀌는 것임.
	else
	{
		UINT32 ulClock = GetClockCount();
		m_ulNextWeatherClock = ulClock + ulDuration;
	}

	SendNature(m_eCurrentWeatherType);

	return TRUE;
}

// 2006.11.08. steeple
// Nature 패킷 부활.
// 시간은 Timer 에서 보내니깐, 여기서는 Weather 만 보낸다.
BOOL	AgsmEventNature::CBNature(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmEventNature *	pThis = (AgsmEventNature *) pClass;
	AgpdNature *		pcsNature = (AgpdNature *) pData;

	return pThis->SendNature(pcsNature->m_eWeather);
}

// 2006.11.08. steeple
BOOL	AgsmEventNature::SendNature(AgpdNatureWeatherType eWeatherType, UINT32 ulNID)
{
	PVOID pvPacket= NULL;
	INT16 nSize = 0;

	pvPacket = m_pcsAgpmEventNature->m_csPacket.MakePacket(TRUE, &nSize, AGPMEVENT_NATURE_PACKET_TYPE, 
					&eWeatherType,
					//NULL,		//&pcsNature->m_ucTimeRatio,
					//NULL,		//&pcsNature->m_ullTime,
					NULL,
					NULL);
	if(!pvPacket || nSize < 1)
		return FALSE;

	BOOL bResult = FALSE;
	if(!ulNID)
		bResult = SendPacketAllUser(pvPacket, nSize);
	else
		bResult = SendPacket(pvPacket, nSize, ulNID);

	m_pcsAgpmEventNature->m_csPacket.FreePacket(pvPacket);

	return bResult;
}