#ifndef __CLASS_RENDER_DEVICE_H__
#define __CLASS_RENDER_DEVICE_H__




#include "Windows.h"
#include "ContainerUtil.h"


#define	STRING_LENGTH_IMAGE_PATH						1024


struct stRenderDeivceSettings
{
	HWND												m_hWnd;

	int													m_nScreenWidth;
	int													m_nScreenHeight;
	int													m_nScreenDepth;
	int													m_nRefreshRate;
	int													m_nMultiSamplingLevel;
	int													m_nMaxOpenFiles;
	
	float												m_fAspectRatio;

	DWORD												m_dwDefaultBGColor;

	BOOL												m_bIsFullScreen;
	BOOL												m_bIsVSync;

	char												m_strImagePath[ STRING_LENGTH_IMAGE_PATH ];

	stRenderDeivceSettings( void )
	{
		m_hWnd = NULL;

		m_nScreenWidth = 0;
		m_nScreenHeight = 0;
		m_nScreenDepth = 0;
		m_nRefreshRate = 0;
		m_nMultiSamplingLevel = 0;
		m_nMaxOpenFiles = 0;

		m_fAspectRatio = 0.0f;

		m_dwDefaultBGColor = ( 255 << 24 ) | ( 255 << 16 ) | ( 255 << 8 ) | 255;

		m_bIsFullScreen = FALSE;
		m_bIsVSync = FALSE;

		memset( m_strImagePath, 0, sizeof( char ) * STRING_LENGTH_IMAGE_PATH );
	}
};

class CRenderDevice
{
public :
	stRenderDeivceSettings								m_DefaultSettings;

public :
	CRenderDevice( void );
	virtual ~CRenderDevice( void );

public :
	virtual BOOL			OnCreate					( stRenderDeivceSettings* pDeviceSettings );
	virtual BOOL			OnDestroy					( void );
	virtual BOOL			OnSceneStart				( void );
	virtual BOOL			OnSceneEnd					( void );
	virtual BOOL			OnSetDefaultBlendMode		( void );

public :
	HWND					GetWindowHandle				( void ) { return m_DefaultSettings.m_hWnd; }
	void					SetWindowHandle				( HWND hWnd ) { m_DefaultSettings.m_hWnd = hWnd; }

public :
	static CRenderDevice*	GetRenderDevice				( char* pRenderDeviceName = NULL );
};




#endif