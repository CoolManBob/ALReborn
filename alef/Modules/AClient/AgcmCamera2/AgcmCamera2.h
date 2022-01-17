// AgcmCamera2.h: interface for the AgcmCamera2 class.
//

// 마우스 인풋에 따른 혹은 주캐릭터가 움직임으로써 발생한 카메라 정보 없데이트는
// OnIdle 에서 한다.

// ** camera work base functions
// rotation, zoom 은 일정 계수로 조절하였으며 뎀핑 필터의 영향을 받을 수 있다.
// 아래 카메라 동작을 위한 함수들은 대부분 카메라 웍에 필요한 변위(각, 위치)를 저장하고 업데이트플래그를 셋팅함으로써 실제 매트릭스 업데이트는 OnIdle 에서 한다.
// bRotWY(RwReal deg);				input : mouse L_Button + mouse move : x변위
// bRotCXRwReal deg);				input : mouse L_Button + mouse move : y변위
// bZoom(RwReal ds);				input : wheel
// bTurnWY_180(void);				input : mouse middlebutton ( 180 도를 정해진 시간동안 회전하며. 사인함수의 일정구간이 속도가 되도록 구현 )
// bOnMoveTarget(void);				desc  : 캐릭이 이동한 만큼 카메라이동.

// ** SetCharacter and Move, Stop callback functions
// CBOnTargetMove( PVOID pData, PVOID pClass, PVOID pCustData );
// CBOnTargetStop( PVOID pData, PVOID pClass, PVOID pCustData );
// CBOnTargetSet ( PVOID pData, PVOID pClass, PVOID pCustData );

// ** util classes
// AgcuCamConstrain		: 카메라 구속조건 구현
// AgcuCamDampingForce	: 카메라 동작을 감쇠식으로 smoothing
// AgcuCamSpringForce	: 캐릭이동시 카메라 가 바라보는 점과 캐릭간의 스프링연결(사용안함)
//						  카메라가 구속조건에서 벗어났을때 스프링식을 이용한 복원(회전스프링은 사용안함 : constrain of pitch)
// AgcuCamPathWork		: 기존의 아크로드 시작시 스플라인을 이용한 카메라웍에서 사용하였으나 현재사용안함.
// AgcuCamMode			: 1인칭, 3인칭(쿼터, 탑, 프리)모드로 전환, 각모드별 셋팅은 생성자에서.

// ** 필요 데이터 수정 및 테스트를 위한 파일 : cam2opt.txt( 툴리포즈에 파일을 추가함 )
// AgcmCamera2::OnIdle 에서 [shift + 'R'] 키조합으로 AgcmCamera2::OptionFromFile 호출
//////////////////////////////////////////////////////////////////////

#ifndef _AGCMCAMERA2_H_
#define _AGCMCAMERA2_H_

#include "AgcModule.h"

#include "AgcmMap.h"
#include "AgcmCharacter.h"
#include "AgcuCamPathWork.h"

#include <stack>

typedef const RwV3d*	LPCRWV3;

class AgcuPathWork;

class AgcmCamera2 : public AgcModule  
{
public:
	//기능사용 플래그
	enum e_opt_flag	{
		e_flag_use_springforce			= 0x00000001,	//사용안함
		e_flag_use_dampingforce			= 0x00000002,	//회전 및 줌동작에서 감쇠효과
		e_flag_use_constrain_zoom		= 0x00000004,	//줌구속 ( 지형과 컬리전 & 라이더블 오브젝트가 카메라 거리 구속조건 )
		e_flag_use_constrain_pitch		= 0x00000008,	//사용안함 ( 원의도 : 길드워에서 카메라조작 없이 캐릭터를 움직일때 카메라와 캐릭사이에 지형이 가로막을 경우 카메라를 회전시켜 들어올림 )

		e_flag_use_LimitPitch			= 0x00000010,	//각도 제한 플래그
		e_flag_use_LimitZoom			= 0x00000020,	//줌제한 플레그

		e_flag_use_reStoreSpring_zoom	= 0x00000040,	//줌구속조건에서 벗어났을때.. 스프링 복원
		e_flag_use_reStoreSpring_pitch	= 0x00000080,	//사용안함
	};

	//한프레임에 업데이트 되어야할 플래그들.
	enum e_upt_mask {
		e_mask_yaw				= 0x00000001,
		e_mask_pitch			= 0x00000002,
		e_mask_zoom				= 0x00000004,
		e_mask_move				= 0x00000008,

		e_mask_wired			= 0x00000010,	//사용안함
		e_mask_compressed		= 0x00000020,	//사용안함

		e_mask_releaseSphiral	= 0x00000040,	//사용안함
		e_mask_releaseLinear	= 0x00000080,	//사용안함

		e_mask_turn180			= 0x00000100,
		e_mask_moveTarget		= 0x00000200,

		e_mask_alphaTarget		= 0x00000400,
		e_mask_transparentTarget= 0x00000800,	//transparent main character
	};


	typedef void (AgcmCamera2::*mfptr_float)(float);
public:
	enum e_idle_type {
		e_idle_default	= 0,	//rot_cx, rot_wy, zoom, follow charac
		e_idle_login	,		//do nothing

		e_idle_chagemode,		//mode change

		e_idle_path		,		//path work

		e_idle_num		,
	};


	//target info
	enum e_targetState {
		e_target_static	= 0,
		e_target_moving	,

		e_target_num	,
	};
	struct stTargetInfo	{
		e_targetState	m_eState;
		RwFrame*		m_pFrm;
		RwV3d			m_vOffset;
		RwV3d			m_vLookat;
		RwV3d			m_vEye;

		stTargetInfo();

		//get matrix axis
		const RwV3d*	GetPtrRight(void);
		const RwV3d*	GetPtrUp(void);
		const RwV3d*	GetPtrAt(void);
		const RwV3d*	GetPtrPos(void);

		void			CalcLookat(const RwV3d* pVEyeSubAt=NULL);

		void			OnMove(const RwV3d* pvEyeSubAt=NULL);
		void			OnStop(void);
		void			OnSet(RwFrame* pFrmTarget, const RwV3d* pvOffset=NULL, const RwV3d* pvEyeSubAt=NULL);
		void			SetYOffset(RwReal fup);
	};

public:
	//camera
	const RwV3d*	bGetPtrCX(void);
	const RwV3d*	bGetPtrCY(void);
	const RwV3d*	bGetPtrCZ(void);
	const RwV3d*	bGetPtrEye(void);
	const RwV3d*	bGetPtrLookat(void);
	
	//. 2006. 5. 23. nonstopdj 
	const float		bGetMaxDistance()
	{
		return m_fMaxLen;
	}

	const float		bGetMinDistance()
	{
		return m_fMinLen;
	}
	
	//. 2006. 4. 17. Nonstopdj
	//. m_v3dEyeSubAt value init.
	void			InitEyeSubAtValue(void)
	{
		m_v3dEyeSubAt.x		= 281.f;
		m_v3dEyeSubAt.y		= 281.f;
		m_v3dEyeSubAt.z		= 450.f;
	};
	//settind
	void		bSetTargetFrame(RwFrame* pFrmTarget, const RwV3d& vOffset);
	void		bSetIdleType(e_idle_type eIdle);

	//call back	
	static	BOOL	CBOnTargetMove( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CBOnTargetStop( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CBOnTargetSet ( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL	CBIsCameraMoving	( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CBTurnCamera	( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL	CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);

//virtual
	BOOL		OnAddModule();
	BOOL		OnInit();
	BOOL		OnDestroy();
	BOOL		OnIdle(UINT32 ulClockCount);
	void		OnLuaInitialize( AuLua * pLua );

	//basic camera work
	void		bRotWY		(RwReal deg);		//WY : world's y axis
	void		bRotCX		(RwReal deg);		//CX : camera's x axis
	void		bZoom		(RwReal ds);
	void		bMove		(const RwV3d& vdelta);
	void		bTurnWY_180 (void);


	void		bOnMoveTarget(void);

	void		bSetCamFrm(const RwV3d& eye, RwReal fYaw, RwReal fPitch);
	void		bSetCamFrm(const RwV3d& lookat, const RwV3d& eye);
	void		bSetCamFrm(RwMatrix& mat);
	
	void		bSetViewWindow(const RwV2d& window);
	void		bGetViewWindow(RwV2d& window);

	void		bTransparentMainCharac();

	void		bLockAct	(e_upt_mask eMask);
	void		bUnlockAct	(e_upt_mask eMask);
	//option
	void		bOptFlagOn(e_opt_flag eopt);
	void		bOptFlagOff(e_opt_flag eopt);

	bool		bIsMoving()
	{
		// 카메라가 현재 움직이고 있는지 리턴.
		return m_stTarget.m_eState == e_target_static ? false : true;
	}
	
private:
	//idle
	void		vOnIdle_default(RwReal fElasped);
	void		vOnIdle_Login(RwReal fElasped);
	void		vOnIdle_modechage(RwReal fElasped);

	//for debugging
	void		vShowInfo(void);

	//option_mask
	bool		vLockFlagChk(e_upt_mask eMask);
	bool		vOptFlagChk(e_opt_flag eopt)const;
	//update_mask
	void		vUdtMaskOn(e_upt_mask eudt);
	void		vUdtMaskOff(e_upt_mask eudt);
	bool		vUdtMaskChk(e_upt_mask eudt)const;

	//available check
	void		vAvailablePitch(RwReal& pitchDelta);
	void		vAvailableZoom(RwReal& zoomDelta);

	//update matrix
	void		vUpdateMatrix();
	void		vForcedZoom(RwReal offset);
	void		vForcedRotCX(RwReal offset);
	void		vAdjLookAt(RwReal currlen);
	void		vAdjAlpha(RwReal currlen);

private:
	//flag
	RwUInt32		m_ulOptFlag;		//option flag
	//module
	AgcmRender*		m_pAgcmRender;
	AgcmMap*		m_pAgcmMap;
	ApmObject*		m_pApmObject;
	AgcmObject*		m_pAgcmObject;
	AgpmCharacter*	m_pAgpmCharacter;
	AgcmCharacter*	m_pAgcmCharacter;
	RpClump*		m_pClumpCharacter;
	//base	info
	RwCamera*		m_pRwCam;
	stTargetInfo	m_stTarget;
	RwV3d			m_v3dLookat;
	RwV3d			m_v3dEyeSubAt;

	//basic camera work
	RwUInt32		m_ulLockMask		;	//해당 플래그들은 잠금상태로( e_upt_mask )
	RwUInt32		m_ulUpdateMask		;	//OnIdle에서 업뎃해야할 플래그들
	RwReal			m_fWYaw				;
	RwReal			m_fCPitch			;
	RwReal			m_fZoom				;
	RwV3d			m_vMove				;
	FLOAT			m_fCharacterHeight	;

	//turn 180
	RwReal			m_fTurnAccumTime;	//sec 누적 회전시간
	RwReal			m_fTurnAccumAngle;	//deg 누적 회전각
	RwReal			m_fTurnTime;		//m_fTurnAngle 회전하는데 걸리는 시간
	RwReal			m_fTurnAngle;		//m_fTurnTime동안 회전할 각도
	RwReal			m_fTurnSpeed;		//m_fTurnSpeed = m_fTurnAngle/m_fTurnTime
	RwReal			m_fTurnCoef;		//m_fTurnCoef = 180.f / (DURING*sinf(OMEGA*DURING) + DURING);

	//constrained
	//distance
	RwReal			m_fDesiredLen;
	RwReal			m_fCurrLen;
	RwReal			m_fLimitedLen;
	//pitch(radian)
	RwReal			m_fDesiredPitch;	//사용안함
	RwReal			m_fCurrPitch;		//사용안함
	RwReal			m_fLimitedPitch;	//사용안함

	//limit
	RwReal			m_fMinLen;
	RwReal			m_fMaxLen;
	RwReal			m_fMinPitch;
	RwReal			m_fMaxPitch;
	
	//idle
	e_idle_type		m_eIdle;
	mfptr_float		m_fptrOnIdle;

public:
	// Lua
	struct	PathWorkInfo
	{
		vector< RwMatrix >	vecCtrlMatrix;
		vector< FLOAT >		vecProjection;

		UINT32			uDuration			;	// 전체길이

		INT32			nType				;
		INT32			nSSType				;
		BOOL			bAccel				;
		BOOL			bClosed				;
		BOOL			bLoop				;

		PathWorkInfo():m_pPathWork( NULL ) , m_pAngleWork( NULL ) { Clear(); }

		BOOL	Save( const char * pFilename );
		BOOL	Load( const char * pFilename );

		BOOL	Play();
		BOOL	Stop();
		BOOL	OnIdle( DWORD uDiffTime , RwMatrix * pMatrix , FLOAT * pfProjection );

		void	Clear();

		INT32	Size();
		BOOL	Push( RwMatrix * pMatrix , FLOAT fProjection );
		BOOL	Pop();

		AgcuPathWork *	m_pPathWork			;	// 스플라인정보
		AgcuPathWork *	m_pAngleWork		;	// 앵글정보

		BOOL	PreparePathWork	();
		BOOL	PrepareAngleWork();

		BOOL	IsPlaying() { if( m_pPathWork ) return TRUE; else return FALSE; }
	};

	PathWorkInfo	m_stPathWork;

	struct	CameraStack
	{
		struct	Info
		{
			RwMatrix	matrix;
			FLOAT		fProjection;
		};

		std::stack< Info >	stack;

		void	Push();
		void	Pop	();
	};

	CameraStack	m_stCameraStack;

	AgcmCamera2();
	virtual ~AgcmCamera2();

	// 콘솔용.
	VOID	Info();
	VOID	ChangeHeight( float fCharacterHeight , float fFaceHeight );

#ifdef _DEBUG
	//for debug
	void OptionFromFile(void);
	void RenderFrustum(RwUInt32 colr=0xff00ffff);
#endif

	//friend class
	friend class AgcuCamConstrain;
	friend class AgcdCamConstrain_Obj;
	friend class AgcdCamConstrain_Terrain;
	friend class AgcuCamDampingForce;
	friend class AgcuCamSpringForce;
	friend class AgcuCamMode;
};

RwBool	IntersectTriangle(const RwV3d *puvRay	//uv	: unit vector
	, const RwV3d *pvCamPos
	, const RwV3d *pv0
	, const RwV3d *pv1
	, const RwV3d *pv2
	, RwReal* t
	, RwReal* u
	, RwReal* v);

VOID CalcPickedPoint(RwV3d& v3dOut
	, const RwV3d& v0
	, const RwV3d& v1
	, const RwV3d& v2
	, RwReal u
	, RwReal v);
#endif // _AGCMCAMERA2_H_