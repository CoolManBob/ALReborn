#ifndef __AGCMNATUREEFFECT_H__
#define __AGCMNATUREEFFECT_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmNatureEffectD" )
#else
#pragma comment ( lib , "AgcmNatureEffect" )
#endif
#endif

#include "AgcModule.h"
#include "rwcore.h"
#include "rpworld.h"

#include "AgcmRender.h"
#include "AgcmMap.h"
#include "AgpmNatureEffect.h"

#include <vector>

#define		AGCMNATUREEFFECT_INI_NAME_TEXTURE_NAME				"TEXTURE_NAME"
#define		AGCMNATUREEFFECT_INI_NAME_NEFFECT_ID				"NEFFECT_ID"
#define		AGCMNATUREEFFECT_INI_NAME_TEXTURE_ID				"TEXTURE_ID"

#define	NATURE_EFFECT_NUM			30
#define	NATURE_EFFECT_TEXTURE_NUM	10
// 나뭇잎 같은건 이펙트에서 처리하자^^;;

#define	NE_ROOT_REGION_SIZE			1600.0f
#define	NE_ROOT_REGION_SIZE_HALF	800.0f

// 1600(w) X 1600(h) X 25(grids) 만큼 계산해서 이펙트가 내린다 

typedef enum
{
	NATURE_EFFECT_NONE = -1,
	NATURE_EFFECT_SNOW,
	NATURE_EFFECT_RAIN,
	NATURE_EFFECT_RAIN_WITH_THUNDER,
	NATURE_EFFECT_RAIN_WITH_FOG,
	NATURE_EFFECT_SANDSTORM,
	NATURE_EFFECT_FIREWORKS_DISPLAY,
	NATURE_EFFECT_TYPE_NUM
} enumNatureEffectType;

struct	NatureEffectInfo
{
	enumNatureEffectType	eType		;
	INT32					texID		;
	FLOAT					fSizeMin	;
	FLOAT					fSizeMax	;
	FLOAT					fDensityMin	;
	FLOAT					fDensityMax	;
	FLOAT					fSpeedMin	;
	FLOAT					fSpeedMax	;
	FLOAT					fSwingMin	;
	FLOAT					fSwingMax	;
	UINT8					cRedMin		;
	UINT8					cRedMax		;
	UINT8					cGreenMin	;
	UINT8					cGreenMax	;
	UINT8					cBlueMin	;
	UINT8					cBlueMax	;
	UINT8					cAlphaMin	;
	UINT8					cAlphaMax	;

	INT32					nSkySet		;

	NatureEffectInfo():eType( NATURE_EFFECT_NONE ), nSkySet( 0 ),texID( 0 ){}
};

enum AGCMNATUREEFFECT_CALLBACK
{
	AGCMNATUREEFFECT_START		,	//* pData는 이펙트 셋.
	AGCMNATUREEFFECT_END		,
};


struct	NatureEffect:ApMemory<NatureEffect,50000>
{
	RwV3d					vPos;

	FLOAT					fWidth;			// 1 - 64까지의 실수를 같는다..AcuParticleDraw 설정에 맟춰서..
	FLOAT					fHeight;		// 1 - 64까지의 실수를 같는다..AcuParticleDraw 설정에 맟춰서..
	DWORD					dwColor;

	INT32					iWaitCount;		// 발동 대기 시간(-일 경우), +일 경우 상태 변화 용
	FLOAT					fDownSpeed;
	FLOAT					fSpeed;

	FLOAT					fMoveAngle;
	FLOAT					fParticleRotAngle;
		
	NatureEffect*			next;
};

struct	NatureEffectGroup
{
	INT32					effID;
	INT32					effectNum;

	NatureEffect*			pList;
	NatureEffectGroup*		next;
};

struct	NatureEffectRoot
{
	INT32					xindex;
	INT32					zindex;

	BOOL					bCameraIn;		// 이전 프레임에서 프러스텀 안에 들어왔는지..
	RwSphere				bs;
	RwBBox					region;
	FLOAT					fCenterHeight;
	BOOL					bForceUpdate;	// 처음 생성되어 적어도 한개의 객체가 소멸될때까지 강제적으로 업데이트
	
	NatureEffectGroup*		listgroup;
	
	NatureEffectRoot*		next;
};

class AgcmNatureEffect : public AgcModule
{
	AgcmRender*				m_pcsAgcmRender			;
	AgcmMap*				m_pcsAgcmMap			;
	AgpmNatureEffect*		m_pcsAgpmNatureEffect	;
	BOOL					m_bStopping				;	// 이펙트 종료 플래그.
	UINT32					m_uStopSignaledTime		;	// 스톱 명령 떨어진 시간..

	void	SetStopFlag() { m_bStopping = TRUE; m_uStopSignaledTime = GetTickCount(); }
public:
	static AgcmNatureEffect	* m_pThis;
	BOOL	GetStopFlag() { return m_bStopping; }
	UINT32	GetLooseTime() { if( GetStopFlag() ) return GetTickCount() - m_uStopSignaledTime; else return 0; }

	// Functions
	AgcmNatureEffect();
	~AgcmNatureEffect();

	BOOL	SetCallbackEffectStart	(ApModuleDefaultCallBack pfCallback, PVOID pClass){	return SetCallback( AGCMNATUREEFFECT_START		, pfCallback, pClass);}
	BOOL	SetCallbackEffectEnd	(ApModuleDefaultCallBack pfCallback, PVOID pClass){	return SetCallback( AGCMNATUREEFFECT_END		, pfCallback, pClass);}

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);
	void	OnLuaInitialize( AuLua * pLua );

	void	UpdateNatureEffects(UINT32	tickdiff);

	BOOL	StartNatureEffect(INT32		effID , BOOL bForce = FALSE );
	BOOL	EndNatureEffect(INT32		effID , BOOL bForce = FALSE );
	BOOL	StopNatureEffect( BOOL bForce = FALSE );

	inline	void	SetWindEnable(BOOL	bVal) { m_stWind.bWind = bVal; }
	inline	BOOL	GetWindEnable(){ return m_stWind.bWind; }
	inline	void	SetWindVector(RwV3d	vec) { m_stWind.vecWind = vec; }
	inline	RwV3d *	GetWindVector() { return &m_vecWind; }

	BOOL	LoadNatureEffectInfoFromINI(char*	szFileName , BOOL bDecryption);
	BOOL	SaveNatureEffectInfoToINI(char*	szFileName , BOOL bDecryption);

protected:
	void	ReleaseNatureEffectAll();

public:
	void	InitRoots();					// render의 MainFrame으로 roots초기화
	void	RefreshRoots();					// roots를 MainFrame의 pos에 따라 갱신

	static	BOOL	CB_NATUREEFFECT_POSTRENDER ( PVOID pData, PVOID pClass, PVOID pCustData );

    // Datas
	UINT32						m_ulLastTick;

	struct	Wind
	{
		BOOL	bWind; // 바람 있음 없음.
		BOOL	bDirection; // 방향 있음 없음
		RwV3d	vecWind;	// 바람 빠르기.. 방향이 없을땐 x값만 사용함.

		Wind():bWind(FALSE), bDirection( FALSE )
		{
			vecWind.x = 0.2f; vecWind.y = 0.0f; vecWind.z = 0.0f; 
		}
	};

	Wind						m_stWind;
	RwV3d						m_vecWind;

	INT32						m_iBeforeRootIndexX;
	INT32						m_iBeforeRootIndexZ;
	
	NatureEffectRoot*			m_listRoot;

	vector< NatureEffectInfo >	m_vectorstNEInfo;
	vector< RwTexture*		>	m_vectorTexture	;

	// 마고자 (2006-02-24 오전 9:46:32) : 
	// 콘솔용 펑션
	void			SetNatureEffect	( int nEff );
	void			SetWind			( FLOAT fXValue);
	void			WindIdleProcess	( UINT32 ulClockCount );

	static	BOOL	CBNatureEffectChanged ( PVOID pData, PVOID pClass, PVOID pCustData );
};

#endif