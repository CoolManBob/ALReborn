#ifndef _ACU_DEVICE_CHECK_H_
#define _ACU_DEVICE_CHECK_H_

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuDeviceCheckD" )
#else
#pragma comment ( lib , "AcuDeviceCheck" )
#endif
#endif

#include "RwCore.h"
#include "RpWorld.h"

#include "ApBase.h"

#include <d3d9.h>

typedef struct tag_DeviceList
{
	char				DeviceName[30];
	unsigned int		vendor_id;
	unsigned int		device_id;
	tag_DeviceList*		next;
}DeviceList;

class AcuDeviceCheck
{
public:
	AcuDeviceCheck();
	~AcuDeviceCheck();

	static void				Release();

	static DeviceList*		m_listFogBug;			// Fog Bug로 인해 캐릭터가 찢어지는 카드들 list

	static unsigned int		m_iVendorID;
	static unsigned int		m_iDeviceID;
	static char				m_strDeviceName[30];

	static WORD				m_iProduct;
	static WORD				m_iVersion;
	static WORD				m_iSubVersion;
	static WORD				m_iBuild;

	static void				InitMyDevice();

	static void				InitListFogBug();
	static BOOL				IsMyCardFogBug();
};

#endif
