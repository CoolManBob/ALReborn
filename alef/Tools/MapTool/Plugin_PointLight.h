#pragma once

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#include "AxisManager.h"
#include "HSVControlStatic.h"

#include <vector>

// CPlugin_PointLight

class CPlugin_PointLight : public CUITileList_PluginBase
{
	DECLARE_DYNAMIC(CPlugin_PointLight)

public:
	CPlugin_PointLight();
	virtual ~CPlugin_PointLight();

protected:
	DECLARE_MESSAGE_MAP()

public:
	// 폴리건 정보..
	RpAtomic *	m_pSphereAtomic			;
	INT32		m_nSelectedPoint		;

	INT32		m_nSelectedAxis		;
	FLOAT		m_fSelectedValue1	;
	FLOAT		m_fSelectedValue2	;

	RpAtomic *	TranslateSphere( RwV3d * pPos );
	void		DrawSphere( RwV3d	* pPos );
	INT32		GetCollisionIndex( RsMouseStatus *ms );

	CAxisManager	m_cAxis;

	// Point Light Object Info
	struct	PLOInfo
	{
		INT32				nOID	;
		RpLight	*			pLight	;
		AgcdPointLight *	pstInfo	;

		PLOInfo() : nOID( -1 ) , pLight( NULL ) , pstInfo( NULL ) {}
	};

	vector< PLOInfo >	m_listLight;
	void		RemoveAllLights();

	// Utility Functions
	INT32		GetLightOffset( RpLight *	pLight	);
	INT32		GetLightOffset( INT32		nOID	);
	void		RemoveLightInfo( RpLight *	pLight	);

	// Selected Light Info Update
	void		RadiusUpdate();
	void		PositionUpdate();
	void		ColorUpdate();

	static BOOL	CBSaveDivision( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL	CBLoadDivision( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL CBPointLightFilter(PVOID pData, PVOID pClass, PVOID pCustData);

public:
	CHSVControlStatic	m_ctlRGBSelectStatic;

public:
	virtual BOOL OnWindowRender			();
	virtual	BOOL Window_OnLButtonDown	( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonUp		( RsMouseStatus *ms );
	virtual BOOL Window_OnMouseMove		( RsMouseStatus *ms );
	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks );

	virtual BOOL OnQuerySaveData		( char * pStr );
	virtual BOOL OnSaveData				();
	virtual BOOL OnLoadData				();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();

	// Add & Remove
	static BOOL	CBOnPointLightAddObject		( PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBOnPointLightRemoveObject	( PVOID pData, PVOID pClass, PVOID pCustData);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

/*

// 마고자 (2005-04-18 오후 2:51:14) : 
Point Light 회의..
1,오브젝트의 여러개 Light 적용 문제
  - 현재 2개 적용 돼고 있고 , 후에 수정될 예정..
2,바닥에 이미지 라이트맵은 영 도움이 안됀다.
  - 버택스 단위로 수정이 돼면 혀결이 될듯..
3,라이트가 약하다.. 
  - 이펙트 쪽 처리때문에 일부러 라이트의 강도를 떨어뜨리고 있는중.
  - 지역용 라이트를 따로 타입을 두어 계산해야할듯
4,마이너스 라이트가 가능한가
  - 일정상 힘들다.. 그리고 셰이더를 이용할 수 밖에 없다.
5, 카피 Move Undo. .. 
  - ㅠㅠ..
6,시간대 조절 on/off
  - 저장 방식이 정해지는 대로 설정할듯
7,라이트를 카메라에 따라 받고 안받고 하는 버그가 있다..
  - 재원아저씨가 해결했다함.
*/
