// AgsmEventNature.h: interface for the AgsmEventNature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMEVENTNATURE_H__876E8F7C_9F2C_459C_834F_41EA53636FEA__INCLUDED_)
#define AFX_AGSMEVENTNATURE_H__876E8F7C_9F2C_459C_834F_41EA53636FEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgsEngine.h"
#include "AgpmEventNature.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmEventNatureD" )
#else
#pragma comment ( lib , "AgsmEventNature" )
#endif
#endif

class AgsmEventNature : public AgsModule  
{
private:
	AgpmEventNature *	m_pcsAgpmEventNature;
	AgpmTimer		*	m_pcsAgpmTimer;

	UINT32				m_ulNextWeatherClock;
	AgpdNatureWeatherType m_eOriginalWeatherType;
	AgpdNatureWeatherType m_eCurrentWeatherType;

public:
	AgsmEventNature();
	virtual ~AgsmEventNature();

	// Virtual Function ต้
	BOOL			OnAddModule();
	BOOL			OnIdle2(UINT32 ulClockCount);

	BOOL			SetWeatherWithDuration(AgpdNatureWeatherType eWeatherType, UINT32 ulDuration);

	static BOOL		CBNature(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL			SendNature(AgpdNatureWeatherType eWeatherType, UINT32 ulNID = 0);
};

#endif // !defined(AFX_AGSMEVENTNATURE_H__876E8F7C_9F2C_459C_834F_41EA53636FEA__INCLUDED_)
