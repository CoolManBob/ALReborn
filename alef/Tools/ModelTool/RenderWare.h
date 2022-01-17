#pragma once

class CRenderWare
{
public:
	CRenderWare();
	virtual ~CRenderWare();

	RwBool		Initialize(HWND hWnd, RwUInt32 dwWidth = 640, RwUInt32 dwHeight = 480, RwUInt32 dwBpp = 16, RwBool bFull = FALSE);
	VOID		Destroy();
	
	RwBool		BegineScene(RwBool bIsClear = TRUE);
	void		EndScene();

	VOID		UpdateFPS();
	VOID		ShowFPS();

	VOID		SetCameraSize(RwRect *rect, RwReal viewWindow  = RWUTIL_DEFAULT_VIEWWINDOW, RwReal aspectRatio = RWUTIL_DEFAULT_ASPECTRATIO);
	VOID		SetClearColor(RwUInt8 red, RwUInt8 green, RwUInt8 blue, RwUInt8 alpha = 255);

	RwUInt32	GetFPS()		{	return m_dwFramesPerSecond;	}
	RwReal		GetDelta()		{	return m_fDeltaTime;		}
	RpWorld*	GetWorld()		{	return m_World;				}
	RtCharset*	GetCharset()	{	return m_Charset;			}
	RwCamera*	GetCamera()		{	return m_Camera;			}
	RpLight*	GetAmbient()	{	return m_Ambient;			}
	RpLight*	GetDirection()	{	return m_Direction;			}
	RwRGBA		GetColor()		{	return m_BackgroundColor;	}
	RpLightMap&	GetRpLightMap()	{	return m_mapLight;			}

protected:
	VOID		InitializeDebugMessage();
	VOID		AddVideoModes();
	RwBool		AttachPlugIn();
	RwBool		SetDevice(RwUInt32 dwDeviceIndex);
	RwBool		SetSubSystem(RwUInt32 index);
	RwBool		SetVideoMode(RwUInt32 index);
	RwUInt32	FindVideoModeIndex(RwUInt32 dwWidth, RwUInt32 dwHeight, RwUInt32 dwDepth, RwBool bFullScreen);
	RwBool		RegisterImageLoader();

	RwBool		CreateObject();
	VOID		DeleteObject();

	RwCamera*	CreateCamera();
	VOID		CameraDestroy(RwCamera* pCamera);
	
	
protected:
	HWND			m_hWnd;
	RwBool			m_bFullScreen;
	RwUInt32		m_dwWidth;
	RwUInt32		m_dwHeight;
	RwUInt32		m_dwDepth;
	
	RwUInt32		m_dwNumSubSystems;
	RwUInt32		m_dwCurSysIndex;
	RwUInt32		m_dwNumVideoModes;
	RwUInt32		m_dwCurVideoModeIndex;

	RwUInt32		m_dwFramesPerSecond;
	RwReal			m_fDeltaTime;

	RwRGBA			m_BackgroundColor;
	
	RwSubSystemInfo m_sSubSysInfo[RWUTIL_MAX_SUBSYSTEMS];
	RwVideoMode     m_sVideoModes[RWUTIL_MAX_VIDEOMODE];

	RtCharset*		m_Charset;
	RpWorld*		m_World;
	RwCamera*		m_Camera;
	RpLight*		m_Ambient;
	RpLight*		m_Direction;
	RpLightMap		m_mapLight;		//Light의 정보를 관리하기위해서. 존재.. 
};