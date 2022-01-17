#include "CRenderDevice.h"
#include "CRenderDevice_Renderware.h"


static CRenderDevice* g_pRenderDevice = NULL;
static stRenderDeivceSettings g_stDeviceSettings;

CRenderDevice* CRenderDevice::GetRenderDevice( char* pRenderDeviceName )
{
	if( !pRenderDeviceName ) return g_pRenderDevice;

	if( !g_pRenderDevice )
	{
		if( strcmp( pRenderDeviceName, "Renderware" ) == 0 )
		{
			g_pRenderDevice = new CRenderDevice_Renderware;
			if( !g_pRenderDevice->OnCreate( &g_stDeviceSettings ) )
			{
#ifdef _DEBUG
				OutputDebugString( "Failed to create render device. ( CRenderDevice::OnCreate() )\n" );
#endif
			}
		}
	}

	return g_pRenderDevice;
}




CRenderDevice::CRenderDevice( void )
{
}

CRenderDevice::~CRenderDevice( void )
{
}

BOOL CRenderDevice::OnCreate( stRenderDeivceSettings* pDeviceSettings )
{
	if( !pDeviceSettings ) return FALSE;
	if( !pDeviceSettings->m_hWnd ) return FALSE;

	memcpy( &m_DefaultSettings, pDeviceSettings, sizeof( stRenderDeivceSettings ) );
	return TRUE;
}

BOOL CRenderDevice::OnDestroy( void )
{
	return TRUE;
}
