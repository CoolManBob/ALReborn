#ifndef ___AGCM_WATER_DLG2_H
#define ___AGCM_WATER_DLG2_H

#include "ApModule.h"
#include "AgcmWater.h"

class AFX_EXT_CLASS AgcmWaterDlg2 : public ApModule
{
public:
	AgcmWaterDlg2();
	~AgcmWaterDlg2();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	int  OpenWaterDlg2(int status_ID);

	int	 MapBMode(int BlendMode,int BOP);

	AgcmWater*		m_pcmWater;
};

#endif

