#pragma once

#include <d3dx9core.h>
#include <AgcEngine.h>

#include "AgcmEff2.h"
#include "MainWindow.h"


#include "Axis.h"
#include "Grid.h"
#include "EffUt_HFMap.h"
#include "ShowFrm.h"
USING_EFFUT;

#include "AgcmFont.h"

extern AgcmEff2*			g_pAgcmEff2;
extern AgcmFont*			g_pAgcmFont;


typedef std::list<RwV3d>	STLLIST_RwV3d;

RwReal GetMapHeight(RwReal x, RwReal y, RwReal z);//이펙트 툴에 AgcmMap이 추가 되기 전까지 이넘을 쓰자.

class CDlgGridToolOption;
class MyEngine : public AgcEngine  
{
	friend class CDlgGridToolOption;
	friend RwReal GetMapHeight(RwReal x, RwReal y, RwReal z);
	
public:
	void	bSetDevice(LPDIRECT3DDEVICE9 lpDev) { m_lpDev=lpDev; };
	void	bLoadTex(LPCSTR fName, INT* pnWidth, INT* pnHeight, INT* pnDepth, LPCTSTR* szFmt, LPCTSTR szDir=NULL);

	HRESULT	bRenderTexPreview(HWND hWnd=NULL);
	HRESULT bRenderTexForUVSelect(HWND hWnd, RwTexture* pTex, float fWidth, float fHeight, INT nRow, INT nCol, INT nSelRow, INT nSelCol);
	
public:
	void	bOnOffAxisCone(BOOL OnOff)	{ m_cAxisWorld.bSetupShowCone(OnOff);}
	void	bOnOffEffFrame(BOOL OnOff);

public:
	BOOL		vPickPlane(RwV3d* pV3dPicked, const RwV3d& center, BOOL bXZ=TRUE);
public:
	VOID		bInitAccessory();
	VOID		bDestroyAcessory();

	CAxis*		bGetPtrFrmAxis() { return &m_cAxisFrm; };

	LPSHOWFRM bPickShowFrm(POINT ptMouse);
	VOID bMoveShowFrm(POINT ptMouse);
	VOID bLBtnUp() { m_lpPickedShowFrm = NULL; };
	VOID bRotShowFrm(int dx, int dy);
	VOID bBackToOrgShowFrm(void);

	VOID bSetFrmHeight(void);

private:
	INT32		vRenderAcessory();
	INT32		vRenderAxisGrid();
	INT32		vRenderHMap();

	BOOL		OnRenderPreCameraUpdate	();

public:
	MyEngine();
	virtual ~MyEngine();

	// Message
	BOOL				OnInit					();
	BOOL				OnAttachPlugins			();	// 플러그인 연결 은 여기서
	RpWorld *			OnCreateWorld			(); // 월드를 생성해서 리턴해준다.
	RwCamera *			OnCreateCamera			( RpWorld * pWorld );	// 메인 카메라를 Create해준다.
	BOOL				OnCreateLight			();	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
	AgcWindow *			OnSetMainWindow			();	// 디폴트 윈도우를 설정해주는 메시지.. 윈도우를 초기화해서 포인터 넘기면 됀다.

	BOOL				OnRegisterModule		();

	VOID				OnWorldRender			(); // World Render 부분에서 AgcmRender를 사용한다.

	VOID				OnTerminate				();

	VOID				OnEndIdle				();

public:
	CGrid				m_cGrid1;
	CGrid				m_cGrid10;
	CAxis				m_cAxisWorld;
	CEffUt_HFMap<VTX_PNDT1>		m_cHeightMap;
	RwTexture*			m_pTexHeightMap;
	CAxis				m_cAxisFrm;
	LPSHOWFRM			m_lpPickedShowFrm;

	LPDIRECT3DDEVICE9	m_lpDev;
	RwTexture*			m_lpTexture;
};

extern MyEngine			g_MyEngine;
extern MainWindow		g_MainWindow;
