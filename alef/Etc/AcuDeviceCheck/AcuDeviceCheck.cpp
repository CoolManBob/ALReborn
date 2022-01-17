#include "AcuDeviceCheck.h"

DeviceList*			AcuDeviceCheck::m_listFogBug = NULL;
unsigned int		AcuDeviceCheck::m_iVendorID = 0;
unsigned int		AcuDeviceCheck::m_iDeviceID = 0;
char				AcuDeviceCheck::m_strDeviceName[30];

WORD				AcuDeviceCheck::m_iProduct = 0;
WORD				AcuDeviceCheck::m_iVersion = 0;
WORD				AcuDeviceCheck::m_iSubVersion = 0;
WORD				AcuDeviceCheck::m_iBuild = 0;

AcuDeviceCheck::AcuDeviceCheck()
{
	m_listFogBug = NULL;

	m_iVendorID = 0;
	m_iDeviceID = 0;
	memset(m_strDeviceName,'\0',30);

	m_iProduct = 0;
	m_iVersion = 0;
	m_iSubVersion = 0;
	m_iBuild = 0;
}

AcuDeviceCheck::~AcuDeviceCheck()
{
	Release();
}

void AcuDeviceCheck::Release()
{
	DeviceList*		cur_dev = m_listFogBug;
	DeviceList*		remove_dev;

	while(cur_dev)
	{
		remove_dev = cur_dev;
		cur_dev = cur_dev->next;

		delete remove_dev;
	}

	m_listFogBug = NULL;
}

void AcuDeviceCheck::InitListFogBug()
{
	FILE*	fp = fopen(".\\ini\\FogBug.lst","r");
	if(!fp)
	{
		ASSERT(!"FogBug File ReadError");
		return;
	}

	char	strDevice[30];
	int vendorID;
	int deviceID;
	DeviceList*		nw_device;
	
	while(1)
	{
		fscanf(fp,"%d,%d,",&vendorID,&deviceID);
		fgets(strDevice,30,fp);

		if(!strcmp(strDevice,"End"))
		{
			break;
		}

		nw_device = new DeviceList;
		nw_device->device_id = deviceID;
		nw_device->vendor_id = vendorID;
		strcpy(nw_device->DeviceName,strDevice);

		nw_device->next = m_listFogBug;
		m_listFogBug = nw_device;
	}

	fclose(fp);
}

BOOL AcuDeviceCheck::IsMyCardFogBug()
{
	DeviceList*		cur_dev = m_listFogBug;
	while(cur_dev)
	{
		if(cur_dev->vendor_id == m_iVendorID && cur_dev->device_id == m_iDeviceID)		// Bug List에서 발견!
		{
			return TRUE;
		}

		cur_dev = cur_dev->next;
	}

	return FALSE;
}

void AcuDeviceCheck::InitMyDevice()
{
	LPDIRECT3DDEVICE9	pDev = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
	if(!pDev)	return;

	LPDIRECT3D9 pD3D9;
	if(D3D_OK != pDev->GetDirect3D(&pD3D9))
		return;

	D3DADAPTER_IDENTIFIER9		pIdentifier;

	pD3D9->GetAdapterIdentifier(D3DADAPTER_DEFAULT,0,&pIdentifier);

	m_iVendorID = pIdentifier.VendorId;
	m_iDeviceID = pIdentifier.DeviceId;
	strcpy(m_strDeviceName,pIdentifier.Description);

	m_iProduct = HIWORD(pIdentifier.DriverVersion.HighPart);
	m_iVersion = LOWORD(pIdentifier.DriverVersion.HighPart);
	m_iSubVersion = HIWORD(pIdentifier.DriverVersion.LowPart);
	m_iBuild = LOWORD(pIdentifier.DriverVersion.LowPart);
}