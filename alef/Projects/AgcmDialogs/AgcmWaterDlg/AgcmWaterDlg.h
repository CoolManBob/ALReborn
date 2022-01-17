#ifndef __AGCM_WATER_DLG_H
#define __AGCM_WATER_DLG_H

#include "ApModule.h"
#include "AgcmWater.h"

class AFX_EXT_CLASS AgcmWaterDlg : public ApModule
{
public:
	AgcmWaterDlg();
	~AgcmWaterDlg();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	int  OpenWaterDlg(int status_ID);

	int	 MapBMode(int BlendMode,int BOP);

	AgcmWater*		m_pcmWater;
};

#endif // #ifndef __AGCM_WATER_DLG_H