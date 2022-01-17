#ifndef	_AGPDEVENTNATURE_H_
#define _AGPDEVENTNATURE_H_

#include "ApBase.h"

typedef enum
{
	AGPDNATURE_TYPE_TIME	= 1,
	AGPDNATURE_TYPE_WEATHER,
	AGPDNATURE_TYPE_MAX,
} AgpdNatureType;

typedef enum
{
	AGPDNATURE_WEATHER_TYPE_NONE = 0,
	AGPDNATURE_WEATHER_TYPE_CALM,
	AGPDNATURE_WEATHER_TYPE_GLOOMY,
	AGPDNATURE_WEATHER_TYPE_RAINY,
	AGPDNATURE_WEATHER_TYPE_POURING,
	AGPDNATURE_WEATHER_TYPE_SNOWY,
	AGPDNATURE_WEATHER_TYPE_MAX
} AgpdNatureWeatherType;

typedef enum
{
	AGPDNATURE_SKY_DATA_OBJECT	= 0
} AgpdNatureDataType;

typedef enum
{
	AGPMEVENTNATURE_SKY_DATA_STREAM	= 0
} AgpmEventNatureStream;

class AgpdNature : public ApBase
{
public:
//	UINT64					m_ullTime			;	// 현재시간..
//	UINT8					m_ucTimeRatio		;	// 시간 배속 비율..
	AgpdNatureWeatherType	m_eWeather			;
	UINT64					m_ullWeatherMinDelay;
	UINT64					m_ullWeatherMaxDelay;
	
	AgpdNature()
	{
		m_eWeather				= AGPDNATURE_WEATHER_TYPE_NONE;
		m_ullWeatherMinDelay	= 0;
		m_ullWeatherMaxDelay	= 0;
	}
};

#define	AGPDSKYSET_MAX_NAME	50

// 스카이 정보에 대한 스트럭쳐..
class AgpdSkySet : public ApBase
{
public:
	INT32					m_nIndex						;	// 현재 셋의 인덱스.
	CHAR					m_strName[ AGPDSKYSET_MAX_NAME + 1];	// 현재 셋의 이름.
};

#endif //_AGPDEVENTNATURE_H_