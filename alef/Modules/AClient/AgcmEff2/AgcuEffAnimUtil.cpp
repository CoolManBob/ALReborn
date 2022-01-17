#include "AgcuEffAnimUtil.h"
#include "ApMemoryTracker.h"

RwInt32 stTimeTableLoop::bAddTime( RwUInt32 dwDiffTick, RwUInt32 dwLastTime )
{
	if( m_eLoopDir == e_TblDir_end )
		return 0;

	if( m_eLoopDir == e_TblDir_nega )
	{
		if( m_dwCurrTime < dwDiffTick )
		{
			m_dwCurrTime	= dwDiffTick - m_dwCurrTime;
			m_eLoopDir		= e_TblDir_posi;
		}
		else
			m_dwCurrTime	-= dwDiffTick;

		return 0;
	}

	m_dwCurrTime += dwDiffTick;

	if( m_dwCurrTime > dwLastTime )
	{
		switch( m_eLoopDir )
		{					
		case e_TblDir_none:
			m_eLoopDir = e_TblDir_end;
			return -1;	//the end!, 한번만 음수를 리턴한다. 그때 지워야 할거면 지울것!
		case e_TblDir_oned:
			m_dwCurrTime	-= dwLastTime;
			break;
		case e_TblDir_posi:
			m_dwCurrTime	= ( dwLastTime << 1 ) - m_dwCurrTime;//dwLife * 2 - m_dwCurrTime;
			m_eLoopDir		= e_TblDir_nega;
			break;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// CalcKeyTime
// 누적시간으로부터 keyTime을 구한다.
// -1 : err, 0 : continue, 1 : end
///////////////////////////////////////////////////////////////////////////////
RwInt32 CalcKeyTime( RwUInt32* pdwKeyTime, E_LOOPOPT eLoopOpt, RwUInt32  dwAccumulateTime, RwUInt32  dwLastTime)
{
//	e_TblDir_none		= 0,//반복 없음.
//	e_TblDir_infinity	,	//무한..
//	e_TblDir_oned		,	//한방향반복..
//	e_TblDir_posi		,	//양방향 반복중 + 방향
//	e_TblDir_nega		,	//양방향 반복중 - 방향

	ASSERT( pdwKeyTime );

	switch( eLoopOpt )
	{
	case e_TblDir_none:
		{
			if( dwAccumulateTime > dwLastTime )
			{
				*pdwKeyTime = dwLastTime;
				return 1;
			}
			*pdwKeyTime = dwAccumulateTime;
		}
		break;
	case e_TblDir_infinity:
		*pdwKeyTime = dwAccumulateTime;
		break;
	case e_TblDir_oned:
		{
			if( !dwLastTime )
			{
				*pdwKeyTime	= dwLastTime;
				break;
			}

			if( dwAccumulateTime > dwLastTime )
				*pdwKeyTime = dwAccumulateTime % dwLastTime;
			else
				*pdwKeyTime = dwAccumulateTime;			
		}
		break;
	case e_TblDir_posi:
		{
			if( dwAccumulateTime > dwLastTime )
			{
				if( !dwLastTime )
				{
					*pdwKeyTime	= dwLastTime;
					break;
				}
				RwUInt32	devid	= dwAccumulateTime/dwLastTime;
				*pdwKeyTime			= dwAccumulateTime - devid * dwLastTime;
				if( DEF_ISODD( devid ) )
					*pdwKeyTime	= dwLastTime - *pdwKeyTime;
			}
			else
			{
				*pdwKeyTime = dwAccumulateTime;
			}
		}
		break;
	default:
		Eff2Ut_ERR("CalcKeyTime failed");
		return -1;
	}

	return 0;
}