/*****************************************************************************
* File : AgcmFactorsDlg.h
*
* Desc :
*
* 060403 Bob Jung.
*****************************************************************************/

#ifndef __AGCM_FACTORS_DLG_H__
#define __AGCM_FACTORS_DLG_H__

#include "ApModule.h"
#include "AgpmFactors.h"

class AFX_EXT_CLASS AgcmFactorsDlg : public ApModule
{
public:
	AgcmFactorsDlg();
	virtual ~AgcmFactorsDlg();

	static AgcmFactorsDlg *GetInstance();

	BOOL			OnInit();
	BOOL			OnAddModule();
	BOOL			OnIdle(UINT32 ulClockCount);
	BOOL			OnDestroy();

	AgpmFactors		*GetAgpmFactors()				{return m_pcsAgpmFactors;}

protected:
	AgpmFactors	*m_pcsAgpmFactors;

public:
	BOOL			OpenFactorsDlg(	AgpdFactor *pcsFactor,
									eAgpdFactorsType eFactorsType = AGPD_FACTORS_TYPE_NONE,
									INT32 lSubType = -1										);
};

#endif // __AGCM_FACTORS_DLG_H__

/******************************************************************************
******************************************************************************/