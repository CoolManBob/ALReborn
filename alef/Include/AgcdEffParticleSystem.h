#if !defined(AFX_AGCDEFFPARTICLESYSTEM_H__05F24328_2629_468D_B43A_D70528F6CD11__INCLUDED_)
#define AFX_AGCDEFFPARTICLESYSTEM_H__05F24328_2629_468D_B43A_D70528F6CD11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AgcdEffBase.h"
#include "AgcuEff2ApMemoryLog.h"

enum ePGroup
{
	ePGroup_Point		= 0,
	ePGroup_Box,
	ePGroup_Cylinder,
	ePGroup_Sphere,
	ePGroup_Num,
};

struct stPGroupBox
{
	RwReal	m_fHWidth,
			m_fHHeight,
			m_fHDepth;
};

struct stPGroupCylinder
{
	RwReal	m_fRadius,
			m_fHHeight;
};

struct stPGroupSphere
{
	RwReal	m_fRadius;
};

struct stEnvrnParam
{
public:
	stEnvrnParam()
	{
		m_v3dGrav.x	=  0.f,
		m_v3dGrav.y	= -980.f,
		m_v3dGrav.z	=  0.f;
		
		m_v3dWind.x	=  1.f,
		m_v3dWind.y	=  0.f,
		m_v3dWind.z	=  0.f;
	};

	const RwV3d& bGetCRefGrav()const { return m_v3dGrav; };
	const RwV3d& bGetCRefWind()const { return m_v3dWind; };
	const RwV3d* bGetCPtrGrav()const { return &m_v3dGrav; };
	const RwV3d* bGetCPtrWind()const { return &m_v3dWind; };

private:
	RwV3d		m_v3dGrav;
	RwV3d		m_v3dWind;
};
typedef stEnvrnParam STENVRNPARAM, *PSTENVRNPARAM, *LPSTENVRNPARAM;
extern STENVRNPARAM	g_EnvrnParam;

struct stCofEnvrnParam
{
	explicit stCofEnvrnParam( RwReal fCofGrav=1.f, RwReal fCofAR=1.f ) : m_fCofGrav( fCofGrav ), m_fCofAirResistance( fCofAR )	{		};

	RwReal		m_fCofGrav;
	RwReal		m_fCofAirResistance;
};

struct StParticle : public ApMemory<StParticle, 5000 >
{
	EFFMEMORYLOG_SMV;

	StParticle( const RwV3d& v3dInitPos, const RwV3d& v3dInitVel, const RwV3d& v3dScale, RwRGBA rgba, const STUVRECT& stUVRect, 
				RwReal fOmega, RwUInt32 dwLife, RwUInt32 dwStartTime, const stCofEnvrnParam& stCofEnvrn );
	~StParticle();

	static	RwInt32	CNT;
	static	RwInt32	MAX_CNT;
	RwV3d			m_v3dInitPos;
	RwV3d			m_v3dInitVelocity;
	RwV3d			m_v3dScale;

	RwRGBA			m_colr;
	STUVRECT		m_stUV;
	RwReal			m_fOmega;			//angular speed in PSyst, angle in PSystSBH;
	RwUInt32		m_dwLife;
	RwUInt32		m_dwStartTime;
	stCofEnvrnParam	m_stCofEnvrn;
};
typedef StParticle PARTICLE, *PPARTICLE, *LPPARTICLE;
typedef std::list<LPPARTICLE>			LPParticleList;
typedef LPParticleList::iterator		LPParticleListItr;
typedef LPParticleList::const_iterator	LPParticleListCItr;

class AgcdEffCtrl_ParticleSysTem;
class AgcdEffParticleSystem : public AgcdEffRenderBase, public ApMemory<AgcdEffParticleSystem, 200>
{
	EFFMEMORYLOG_SMV;

public:
	//emiter
	struct EMITER
	{
		RwReal		m_fPower;					// 초기 속도.. ( cm / sec )
		RwReal		m_fPowerOffset;

		RwReal		m_fGunLength;				// 포신 길이.. ( cm )
		RwReal		m_fGunLengthOffset;			

		RwUInt32	m_nNumOfOneShoot;			// 한번에 발사할 입자 갯수.
		RwUInt32	m_nNumOfOneShootOffset;
		
		RwReal		m_fOmegaYaw_World,
					m_fOmegaPitch_Local;		// 에미터의 각속도. ( deg / sec )

		RwReal		m_fMinYaw	, m_fMaxYaw,	// currYawAngle		= currYawAngle	 * m_fMaxYaw   / 360.f;
					m_fMinPitch	, m_fMaxPitch;	// currPitchAngle	= currPitchAngle * m_fMaxPitch / 360.f;

		RwV3d		m_vDir;						// 초기 포신 방향
		RwV3d		m_vSide;					// 포신의 side_vector
		RwReal		m_fConAngle;				// 포신의 cone angle

		RwUInt32	m_dwFlagOfEmiter;			// emiter's flag
		
		ePGroup		m_ePGroup;
		union {
			stPGroupBox			m_pgroupBox;
			stPGroupCylinder	m_pgroupCylinder;
			stPGroupSphere		m_pgroupSphere;
		};

		RwInt32	bGetAngle( RwReal* pfYaw, RwReal* pfPitch, RwReal fPastTime )const;
		VOID	bUpdateSideVector(void);	//m_vDir를 셋팅한후 사이드 백터를 계산한다.
	};

	//particle property
	struct PARTICLEPROP
	{
		RwReal			m_fPAngularspeed;			// 입자의 각속도.
		RwReal			m_fPAngularspeedOffset;

		RwUInt32		m_dwParticleLife;			// 입자의 유지 시간.
		RwUInt32		m_dwParticleLifeOffset;

		stCofEnvrnParam	m_stCofEnvrn;				// 환경 영향 계수.
		stCofEnvrnParam	m_stCofEnvrnOffset;

		RwUInt32		m_dwFlagOfParticle;			// particle's flag
	};

public:
	RwInt32			m_nCapacity;				// 총 보유 파티클 갯수.
	RwUInt32		m_dwShootDelay;				// 발사 간격..

	BOOL			m_bDirOfTargetUse;

	RwChar			m_szClumpFName[EFF2_FILE_NAME_MAX];
	RpClump			*m_pClump;

	EMITER			m_stEmiter;
	PARTICLEPROP	m_stParticleProp;
	
private:
	//do not use!
	//AgcdEffParticleSystem( const AgcdEffParticleSystem& cpy ) : AgcdEffRenderBase(cpy) {cpy;}
	//AgcdEffParticleSystem& operator = ( const AgcdEffParticleSystem& cpy ) { cpy; return *this; }
	AgcdEffParticleSystem( const AgcdEffParticleSystem& cpy );
	AgcdEffParticleSystem& operator = ( const AgcdEffParticleSystem& cpy );

private:
	RwInt32 vEmiterFrmUpdate(RwUInt32 dwTheTime, AgcdEffCtrl_ParticleSysTem* pEffCtrl_PSyst )const;
	RwInt32 vEmiterGetDir(RwV3d* pDir, RwFrame* pFrmEmiter, RwReal fYaw, RwReal fPitch, RtQuat *pQuat = NULL)const;
	RwInt32 vShootGroup(RwV3d& v3dCenter, RwV3d& v3dDir, INT nNum, RwUInt32 dwShootTime, AgcdEffCtrl_ParticleSysTem* pEffCtrl_PSyst )const;

	VOID	vCalcOffset_Point(RwV3d* pV3dOffset) const;
	VOID	vCalcOffset_Box(RwV3d* pV3dOffset) const;
	VOID	vCalcOffset_Cylinder(RwV3d* pV3dOffset) const;
	VOID	vCalcOffset_Sphere(RwV3d* pV3dOffset) const;

	VOID	vShakeEmiter_forOldPSyst( RwV3d* pV3dPos )const;
	
public:
	explicit AgcdEffParticleSystem(AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType=AgcdEffRenderBase::EFFBLEND_ADD_ONE_ONE);
	virtual ~AgcdEffParticleSystem();

	RwInt32 bShootPaticle( RwUInt32 dwAccumulateTime, AgcdEffCtrl_ParticleSysTem* pEffCtrl_PSyst )const ;

	//setup
	void	bSetCapacity( RwInt32 nCapacity )		{ m_nCapacity	= nCapacity; };
	void	bSetShootDelay( RwUInt32 dwShootDelay )	{ m_dwShootDelay= dwShootDelay ? dwShootDelay : 1; };
	void	bSetClumpFile( const RwChar* szClump )	{ 		
		ASSERT( sizeof(m_szClumpFName) > strlen(szClump) );
		strcpy( m_szClumpFName, szClump ? szClump : "" ); 
	};

	PARTICLEPROP&	bGetRefPProp(void)			{ return m_stParticleProp; };
	EMITER&			bGetRefEmiter(void)			{ return m_stEmiter; };
	
	//access
	RwInt32			bGetCapacity(void)const		{ return m_nCapacity; };
	RwUInt32		bGetShootDelay(void)const	{ return m_dwShootDelay; };
	const RwChar*	bGetClumpFile(void)const	{ return m_szClumpFName; };
	const RpClump*	bGetCPtrClump(void)const	{ return m_pClump; };
	RpClump*		bGetPtrClump(void)			{ return m_pClump; };
	const EMITER&	bGetCRefEmiter(void)const	{ return m_stEmiter; };
	const PARTICLEPROP& bGetCRefPProp(void)const{ return m_stParticleProp; };


	BOOL			GetBaseDirUse( VOID )		{	return m_bDirOfTargetUse;	}
	VOID			SetBaseDirUse( BOOL bUse )	{ m_bDirOfTargetUse	=	bUse;	}
	
	//file in out
	RwInt32	bToFile(FILE* fp);
	RwInt32 bFromFile(FILE* fp);
	
	//for tool
#ifdef USE_MFC
	virtual 
	INT32		bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC
};

typedef void(AgcdEffParticleSystem::*PSYS_fptrCalcOffset)(RwV3d* pV3dOffset) const;

//#############################################################################
// class		: AgcdEffParticleSys_BlackHole
// desc			: 
// type			: 
// update		: 20040701 by kday
//				- 최초 작업.
//#############################################################################
class AgcdEffCtrl_ParticleSyst_SBH;
class AgcdEffParticleSys_SimpleBlackHole : public AgcdEffRenderBase, public ApMemory<AgcdEffParticleSys_SimpleBlackHole, 100>
{
	EFFMEMORYLOG_SMV;

#ifdef USE_MFC
public:
#else
private:
#endif//USE_MFC
	RwInt32		m_nCapacity;

	RwInt32		m_nNumOfOneShoot;
	RwInt32		m_nNumOfOneShootOffset;

	RwUInt32	m_dwTimeGap;		// ( milli sec )
	RwUInt32	m_dwParticleLife;	// ( milli sec )

	RwReal		m_fInitSpeed;		// ( cm / sec )
	RwReal		m_fInitSpeedOffset;	// ( cm / sec )

	RwReal		m_fRollMin,			// ( deg )
				m_fRollMax;			// ( deg )

	RwReal		m_fRollStep;		// ( deg )	// (m_fRollMax-m_fRollMin)/Capacity

	RwReal		m_fRadius;

private:
	VOID		vCircleEmiter( RwV3d* pPos )const;
	
private:
	//do not use!
	AgcdEffParticleSys_SimpleBlackHole( const AgcdEffParticleSys_SimpleBlackHole& cpy ) : AgcdEffRenderBase(cpy) {cpy;}
	AgcdEffParticleSys_SimpleBlackHole& operator = ( const AgcdEffParticleSys_SimpleBlackHole& cpy ) { cpy; return *this; }

public:
	explicit AgcdEffParticleSys_SimpleBlackHole(AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType = AgcdEffRenderBase::EFFBLEND_ADD_ONE_ONE);
	virtual ~AgcdEffParticleSys_SimpleBlackHole();

	RwInt32 bShootPaticle( RwUInt32 dwAccumulateTime, AgcdEffCtrl_ParticleSyst_SBH* pEffCtrl_PSystSBH )const ;

	//setup
	void	bSetCapacity( RwInt32 nCapacity )
	{ 
		m_nCapacity	= nCapacity; 
	};
	void	bSetOneShootNum( RwInt32 nOneShootNum, RwInt32 nOffSet )
	{
		m_nNumOfOneShoot		= nOneShootNum;
		m_nNumOfOneShootOffset	= nOffSet;
	};
	void	bSetShootDelay( RwUInt32 dwDelay )
	{
		m_dwTimeGap	= dwDelay ? dwDelay : 1;
	};
	void	bSetPLife( RwUInt32 dwLife )
	{
		m_dwParticleLife	= dwLife;
	};
	void	bSetInitSpeed( RwReal fSpeed, RwReal fOffset )
	{
		m_fInitSpeed		= fSpeed;
		m_fInitSpeedOffset	= fOffset;
	};
	void	bSetRollMinMax( RwReal fRollMin, RwReal fRollMax )
	{
		m_fRollMin	= fRollMin;
		m_fRollMax	= fRollMax;

		m_fRollStep	= (m_fRollMax-m_fRollMin)/(static_cast<RwReal>(m_nCapacity));
	};
	void	bSetRadius( RwReal fRadius ) { m_fRadius = fRadius; };

	//access
	RwInt32		bGetCapacity(void)const			{ return m_nCapacity; };
	RwInt32		bGetOneShootNum(void)const		{ return m_nNumOfOneShoot; };
	RwInt32		bGetOneShootNumOffset(void)const{ return m_nNumOfOneShootOffset; };
	RwUInt32	bGetShootDelay(void)const		{ return m_dwTimeGap; };
	RwUInt32	bGetParticleLife(void)const		{ return m_dwParticleLife; };
	RwReal		bGetInitSpeed(void)const		{ return m_fInitSpeed; };
	RwReal		bGetInitSpeedOffset(void)const	{ return m_fInitSpeedOffset; };
	RwReal		bGetRollMin(void)const			{ return m_fRollMin; };
	RwReal		bGetRollMax(void)const			{ return m_fRollMax; };
	RwReal		bGetRadius(void)const			{ return m_fRadius; };
	
	//file in out
	RwInt32	bToFile(FILE* fp);
	RwInt32 bFromFile(FILE* fp);
	
	//for tool
#ifdef USE_MFC
	virtual 
	INT32		bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC

};

#endif