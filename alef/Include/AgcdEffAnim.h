// AgcuEffAnim.h: interface for the AgcuEffAnim class.
//

// missile, RtAnim, RpSpline 을 제외한 나머지는 키프레임 방식이다.
// bUpdateVal : 누적시간과 해당 이펙트베이스의 인스턴스를 인풋으로 넣어주면 그 시간대의 값을 이펙트에 적용.

// AgcdEffAnim_Colr					RwRGB 테이블
// AgcdEffAnim_TuTv					UVRect 테이블
// AgcdEffAnim_Missile				발사체
// AgcdEffAnim_Linear				짖선 운동
// AgcdEffAnim_Rev					공전 운동
// AgcdEffAnim_Rot					자전 운동
// AgcdEffAnim_RpSpline				스플라인
// AgcdEffAnim_RtAnim				에니메이션을 가진 오브젝트의 경우만
// AgcdEffAnim_Scale				크기 변화
// AgcdEffAnim_ParticlePosScale		파티클에서만 쓰이며 입자 하나마다의 위치제어를 위함

///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCUEFFANIM_H__183F7F16_2AC5_485B_AB55_A39E85BA8351__INCLUDED_)
#define AFX_AGCUEFFANIM_H__183F7F16_2AC5_485B_AB55_A39E85BA8351__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffAnimUtil.h"
#include "ApMemory.h"

#include "AgcuEff2ApMemoryLog.h"
#include "singleton.h"

#define FLAG_EFFANIM_MISSILE_ZAXIS_90		0x00010000	//오브젝트이면 월드의 Z축으로 90도를 돌려야 하네..
#define FLAG_EFFANIM_REV_ROTATE				0x00010000	//공전시 회전적용

struct StUVRect
{
	RwReal	m_fLeft,
			m_fTop,
			m_fRight,
			m_fBottom;

	explicit StUVRect( RwReal fL=0.f, RwReal fT=0.f, RwReal fR=1.f, RwReal fB=1.f );
};
typedef StUVRect STUVRECT, *LPSTUVRECT;

class AgcdEffCtrl_Base;
class AgcuEffAnimCreater;
class AgcuEffIniMng;
//--------------------- AgcdEffAnim -----------------
class AgcdEffAnim
{
	friend class AgcuEffAnimCreater;
	
public:
	enum E_EFFANIMTYPE
	{
		E_EFFANIM_COLOR				= 0,//colr
		E_EFFANIM_TUTV,	//uv
		E_EFFANIM_MISSILE,	//translate
		E_EFFANIM_LINEAR,	//translate
		E_EFFANIM_REVOLUTION,	//translate
		E_EFFANIM_ROTATION,	//rotation
		E_EFFANIM_RPSPLINE,	//translation + rotation
		E_EFFANIM_RTANIM,	//translation + rotation
		E_EFFANIM_SCALE,	//scale
		E_EFFANIM_PARTICLEPOSSCALE	,	//for each particle
		E_EFFANIM_POSTFX			,
		E_EFFANIM_CAMERA			,
		E_EFFANIM_NUM				,
	};


	AgcdEffAnim( E_EFFANIMTYPE eEffAnimType, RwUInt32 dwBitFlags );
	virtual ~AgcdEffAnim();

	virtual	BOOL	bGetTVal		( PVOID pOut, RwUInt32 dwCurrTime ) const = 0;
	virtual	BOOL	bSetTVal		( RwInt32 nIndex, void* pIn, RwUInt32 dwTime )		{	return TRUE;	}
	virtual BOOL	bInsTVal		( void* pIn, RwUInt32 dwTime )						{	return TRUE;	}
	virtual BOOL	bDelTVal		( RwUInt32 dwTime )									{	return TRUE;	}
	virtual void	bSetLife		( RwUInt32 dwLife )									{ m_dwLifeTime	= dwLife; };
	virtual RwInt32 bUpdateVal		( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0) = 0;

	//access functions
	RwUInt32		bGetLife		( VOID ) const					{	return m_dwLifeTime;	}
	E_EFFANIMTYPE	bGetAnimType	( VOID ) const					{	return m_eEffAnimType;	}
	E_LOOPOPT		bGetLoopOpt		( VOID ) const					{	return m_eLoopOpt;		}
	void			bSetLoopOpt		( E_LOOPOPT eLoopOpt )			{	m_eLoopOpt = eLoopOpt;	}
	RwUInt32		bGetFlag		( VOID ) const					{	return m_dwBitFlags;	}

	virtual RwInt32	bToFile			( FILE* fp )										{return 1;};
	virtual RwInt32	bFromFile		( FILE* fp )										{return 1;};

	//util functions
	void			bAddFlag		( RwUInt32 dwFlag )		{ m_dwBitFlags	|= dwFlag; };
	void			bDelFlag		( RwUInt32 dwFlag )		{ m_dwBitFlags	|= ~dwFlag; };
	RwUInt32		bCheckFlag		( RwUInt32 dwFlag )		{ return (m_dwBitFlags & dwFlag); };

protected:
	RwInt32			tToFile			( FILE* fp );
	RwInt32			tFromFile		( FILE* fp );

protected:
	//do not use
	AgcdEffAnim( const AgcdEffAnim& cpy ):m_eEffAnimType(cpy.m_eEffAnimType){cpy;};
	AgcdEffAnim& operator = ( const AgcdEffAnim& cpy ){cpy; return *this;};

public:
	const E_EFFANIMTYPE		m_eEffAnimType;
	RwUInt32				m_dwBitFlags;			//각종 옵션 및 스위치.
	RwUInt32				m_dwLifeTime;			//테이블이면 마지막 시간값, 스플라인이면 라이프, RtAnimation 이면.. 없음.
	E_LOOPOPT				m_eLoopOpt;				//loop option

};
typedef AgcdEffAnim	EFFANIM, *LPEFFANIM, *PEFFANIM;

extern const char* EFFANIMTYPE_NAME[AgcdEffAnim::E_EFFANIM_NUM];

//--------------------- AgcdEffAnim_Colr -----------------
class AgcdEffAnim_Colr : public AgcdEffAnim, public ApMemory<AgcdEffAnim_Colr, 2000>
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

public:
	AgcdEffAnim_Colr( RwUInt32 dwBitFlags=0x0, RwInt32 nSize=0);
	virtual ~AgcdEffAnim_Colr();
	
	virtual	BOOL		bGetTVal	( void* pOut, RwUInt32 dwCurrTime ) const;
	virtual	BOOL		bSetTVal	( RwInt32 nIndex, void* pIn, RwUInt32 dwTime );
	virtual BOOL		bInsTVal	( void* pIn, RwUInt32 dwTime );
	virtual BOOL		bDelTVal	( RwUInt32 dwTime );
	virtual RwInt32		bUpdateVal	( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0 );

	RwInt32				bToFile		( FILE* fp );
	RwInt32				bFromFile	( FILE* fp );

private:
	//do not use
	AgcdEffAnim_Colr( const AgcdEffAnim_Colr& cpy ) : AgcdEffAnim( cpy )	{cpy;};
	AgcdEffAnim_Colr& operator = ( const AgcdEffAnim_Colr& cpy ){ cpy; return *this; };

public:
	stTimeTable<RwRGBA>	m_stTblColr;
};
typedef AgcdEffAnim_Colr EFFANIM_COLR, *LPEFFANIM_COLR, *PEFFANIM_COLR;

//--------------------- AgcdEffAnim_TuTv -----------------
class AgcdEffAnim_TuTv : public AgcdEffAnim, public ApMemory<AgcdEffAnim_TuTv, 2000>
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

public:
	AgcdEffAnim_TuTv( RwUInt32 dwBitFlags=0x0, RwInt32 nSize=0);
	virtual ~AgcdEffAnim_TuTv();
	
	virtual	BOOL		bGetTVal	( PVOID pOut, RwUInt32 dwCurrTime ) const;
	virtual	BOOL		bSetTVal	( RwInt32 nIndex, void* pIn, RwUInt32 dwTime );
	virtual BOOL		bInsTVal	( void* pIn, RwUInt32 dwTime );
	virtual BOOL		bDelTVal	( RwUInt32 dwTime );
	virtual RwInt32		bUpdateVal	( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0 );
		
	//file in out
	RwInt32				bToFile		( FILE* fp );
	RwInt32				bFromFile	( FILE* fp );

private:
	//do not use
	AgcdEffAnim_TuTv( const AgcdEffAnim_TuTv& cpy ) : AgcdEffAnim( cpy ) {cpy;};
	AgcdEffAnim_TuTv& operator = ( const AgcdEffAnim_TuTv& cpy ) {cpy; return *this;};

public:
	stTimeTable<STUVRECT>	m_stTblRect;	// left_top
};
typedef AgcdEffAnim_TuTv EFFANIM_TUTV, *LPEFFANIM_TUTV, *PEFFANIM_TUTV;

//--------------------- AgcdEffAnim_Missile -----------------
class AgcdEffAnim_Missile : public AgcdEffAnim, public ApMemory<AgcdEffAnim_Missile,  300 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

private:
	//do not use
	AgcdEffAnim_Missile( const AgcdEffAnim_Missile& cpy ) : AgcdEffAnim( cpy ) {cpy;};
	AgcdEffAnim_Missile& operator = ( const AgcdEffAnim_Missile& cpy ) {cpy; return *this; };

public:
	AgcdEffAnim_Missile( RwUInt32 dwBitFlags=0x0, RwReal fSpeed=1000.f, RwReal fAccel=0.f , RwReal fRotate=0.f , RwReal fRadius=0.f , RwReal fZigzagLength=0.f , RwReal fMinSpeed=0.f , RwReal fMaxSpeed=0.f );
	virtual ~AgcdEffAnim_Missile();

	virtual	BOOL		bGetTVal		( void* pOut, RwUInt32 dwCurrTime ) const;
	virtual RwInt32		bUpdateVal		( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0);

	RwReal				bGetSpeed		( VOID ) const		{ return m_fSpeed0;		};
	RwReal				bGetAccel		( VOID ) const		{ return m_fAccel;		};

	VOID				bSetSpeed		( RwReal fSpeed )	{ m_fSpeed0 = fSpeed;	};
	VOID				bSetAccel		( RwReal fAccel )	{ m_fAccel	= fAccel;	};

	VOID				SetMinSpeed		( RwReal fMinSpeed )	{	m_fMinSpeed	=	fMinSpeed;	}
	VOID				SetMaxSpeed		( RwReal fMaxSpeed )	{	m_fMaxSpeed	=	fMaxSpeed;	}

	VOID				SetRotate		( RwReal fRotate )	{ m_fRotate	=	fRotate;};
	VOID				SetRadius		( RwReal fRadius )	{ m_fRadius	=	fRadius;};
	VOID				SetZigzagLength ( RwReal fLength )	{ m_fZigzagLength	=	fLength; };

	RwReal				GetMinSpeed		( VOID ) const		{ return m_fMinSpeed;	}
	RwReal				GetMaxSpeed		( VOID ) const		{ return m_fMaxSpeed;	}

	RwReal				GetRotate		( VOID ) const		{ return m_fRotate;		};
	RwReal				GetRadius		( VOID ) const		{ return m_fRadius;		};
	RwReal				GetZigzagLength	( VOID ) const		{ return m_fZigzagLength;};
	
	RwInt32				bToFile			( FILE* fp );
	RwInt32				bFromFile		( FILE* fp );

public:
	RwReal	m_fMinSpeed;	//	최저 속도
	RwReal	m_fMaxSpeed;	//	최고 속도

	RwReal	m_fSpeed0;	//초기 속도.			( cm / sec   )
	RwReal	m_fAccel;	//가속도<속도방향>		( cm / sec^2 )

	RwReal	m_fRotate;			//	초당 몇번 회전하는지
	RwReal	m_fRadius;			//	rotate , zigzag 반지름
	RwReal	m_fZigzagLength;	//	zigzag 되는 길이

	RwReal	m_fPrevCof;			//	전 프레임에 움직인 거리
	RwReal	m_fPrevTime;		//	전 시간 저장


};
typedef AgcdEffAnim_Missile EFFANIM_MISSILE, *LPEFFANIM_MISSILE, *PEFFANIM_MISSILE;

//--------------------- AgcdEffAnim_Linear -----------------
class AgcdEffAnim_Linear : public AgcdEffAnim, public ApMemory<AgcdEffAnim_Linear,  300 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

public:
	AgcdEffAnim_Linear( RwUInt32 dwBitFlags=0x0, RwInt32 nSize=0 );
	virtual ~AgcdEffAnim_Linear();
	
	virtual	BOOL		bGetTVal		( PVOID pOut, RwUInt32 dwCurrTime ) const;
	virtual	BOOL		bSetTVal		( RwInt32 nIndex, void* pIn, RwUInt32 dwTime );
	virtual BOOL		bInsTVal		( PVOID pIn, RwUInt32 dwTime );
	virtual BOOL		bDelTVal		( RwUInt32 dwTime );
	virtual RwInt32		bUpdateVal		( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0LU);

	RwInt32				bToFile			( FILE* fp );
	RwInt32				bFromFile		( FILE* fp );

private:
	//do not use
	AgcdEffAnim_Linear( const AgcdEffAnim_Linear& cpy ) : AgcdEffAnim( cpy ) { cpy; };
	AgcdEffAnim_Linear& operator = ( const AgcdEffAnim_Linear& cpy ) { cpy; return *this; };

public:
	stTimeTable<RwV3d>	m_stTblPos;	//Position TABLE.. 
};

//--------------------- AgcdEffAnim_Rev -----------------
class AgcdEffAnim_Rev : public AgcdEffAnim, public ApMemory<AgcdEffAnim_Rev,  300 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

public:
	struct StRevolution
	{
		RwReal	m_fHeight;
		RwReal	m_fRadius;
		RwReal	m_fAngle;	// not angular speed( degree / sec ) but the angle ( degree )

		StRevolution( RwReal fHeight = 0.f, RwReal fRadius = 100.f, RwReal fAngle = 0.f	) : m_fHeight( fHeight ), m_fRadius( fRadius ), m_fAngle( fAngle )	{		}
	};
	typedef StRevolution STREVOLUTION, *PSTREVOLUTION, *LPSTREVOLUTION;
	
public:
	AgcdEffAnim_Rev( RwUInt32 dwBitFlags=0x0, RwInt32 nSize=0, const RwV3d* pV3dAxis=NULL );
	virtual ~AgcdEffAnim_Rev();

	virtual	BOOL			bGetTVal		( PVOID pOut, RwUInt32 dwCurrTime ) const;
	virtual	BOOL			bSetTVal		( RwInt32 nIndex, void* pIn, RwUInt32 dwTime );
	virtual BOOL			bInsTVal		( PVOID pIn, RwUInt32 dwTime );
	virtual BOOL			bDelTVal		( RwUInt32 dwTime );
	virtual RwInt32			bUpdateVal		( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0);

	const RwV3d&			bGetRotAxis		( VOID )const { return m_v3dRotAxis; };
	const RwV3d&			bGetRotSide		( VOID )const { return m_v3dRotSide; };
	void					bSetRotAxis		( const RwV3d& v3dRotAxis )	{ m_v3dRotAxis = v3dRotAxis; };
	
	//file in out
	RwInt32					bToFile			( FILE* fp );
	RwInt32					bFromFile		( FILE* fp );

private:
	//do not use
	AgcdEffAnim_Rev( const AgcdEffAnim_Rev& cpy ) : AgcdEffAnim( cpy ) { cpy; };
	AgcdEffAnim_Rev& operator = ( const AgcdEffAnim_Rev& cpy ) { cpy; return *this; };

public:
	RwV3d						m_v3dRotAxis;	// default : < 0, 1, 0 >
	RwV3d						m_v3dRotSide;
	stTimeTable<StRevolution>	m_stTblRev;
};
typedef AgcdEffAnim_Rev EFFANIM_REV, *LPEFFANIM_REV, *PEFFANIM_REV;

//--------------------- AgcdEffAnim_Rot -----------------
class AgcdEffAnim_Rot : public AgcdEffAnim, public ApMemory<AgcdEffAnim_Rot,  1000 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

private:
	//do not use
	AgcdEffAnim_Rot( const AgcdEffAnim_Rot& cpy ) : AgcdEffAnim( cpy ) {cpy;};
	AgcdEffAnim_Rot& operator = ( const AgcdEffAnim_Rot& cpy ){cpy; return *this;};
public:
	AgcdEffAnim_Rot( RwUInt32 dwBitFlags=0x0, RwInt32 nSize=0, const RwV3d* pV3dRotAxis=NULL );
	virtual ~AgcdEffAnim_Rot();

	//pure virtual functions
	virtual	BOOL		bGetTVal		( void* pOut, RwUInt32 dwCurrTime ) const;
	virtual	BOOL		bSetTVal		( RwInt32 nIndex, void* pIn, RwUInt32 dwTime );
	virtual BOOL		bInsTVal		( void* pIn, RwUInt32 dwTime );
	virtual BOOL		bDelTVal		( RwUInt32 dwTime );
	virtual RwInt32		bUpdateVal		( RwUInt32	dwAccumulateTime, AgcdEffCtrl_Base*	pEffBase, RwUInt32 ulFlagReserved = 0);

	const RwV3d&		bGetRotAxis		( VOID )const					{ return m_v3dRotAxis; };
	void				bSetRotAxis		( const RwV3d& v3dRotAxis )		{ m_v3dRotAxis = v3dRotAxis; };
	
	//file in out
	RwInt32				bToFile			( FILE* fp );
	RwInt32				bFromFile		( FILE* fp );

public:
	RwV3d				m_v3dRotAxis;
	stTimeTable<RwReal>	m_stTblDeg;		//각 또는 각속도..
};
typedef AgcdEffAnim_Rot EFFANIM_ROT, *LPEFFANIM_ROT, *PEFFANIM_ROT;

//--------------------- AgcdEffAnim_RpSpline -----------------
class AgcdEffAnim_RpSpline : public AgcdEffAnim, public ApMemory<AgcdEffAnim_RpSpline,  300 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

private:
	//do not use
	AgcdEffAnim_RpSpline( const AgcdEffAnim_RpSpline& cpy ) : AgcdEffAnim( cpy ) {cpy;};
	AgcdEffAnim_RpSpline& operator = ( const AgcdEffAnim_RpSpline& cpy ){cpy; return *this;};
public:
	AgcdEffAnim_RpSpline( RwUInt32 dwBitFlags=0x0 );
	virtual ~AgcdEffAnim_RpSpline();

	virtual	BOOL		bGetTVal		( PVOID pOut, RwUInt32 dwCurrTime ) const;
	virtual RwInt32		bUpdateVal		( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0 );

	//access functions
	const RwChar*		bGetSplineFName	( VOID )const			{ return m_szSplineFName; };
	RwChar*				bGetSplineFName	( VOID )				{ return m_szSplineFName; };
	RwUInt32			bGetLife		( VOID )const			{ return AgcdEffAnim::m_dwLifeTime; };
	RwInt32				bSetSplineFName	(const RwChar* szSplineFName);
	void				bSetLife		(RwUInt32 dwLife)
	{
		AgcdEffAnim::m_dwLifeTime = dwLife;
		if( dwLife )
			m_fIvsLife = 1.f / (RwReal)dwLife;
	};
	
	//file in out
	RwInt32				bToFile			( FILE* fp );
	RwInt32				bFromFile		( FILE* fp );

public:
	RwChar		m_szSplineFName[EFF2_FILE_NAME_MAX];
	RwReal		m_fIvsLife;
	RpSpline*	m_pRpSpline;
};
typedef AgcdEffAnim_RpSpline EFFANIM_RPSPLINE, *LPEFFANIM_RPSPLINE, *PEFFANIM_RPSPLINE;

//--------------------- AgcdEffAnim_RtAnim -----------------
class AgcdEffAnim_RtAnim : public AgcdEffAnim, public ApMemory<AgcdEffAnim_RtAnim, 300 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

private:
	//do not use
	AgcdEffAnim_RtAnim( const AgcdEffAnim_RtAnim& cpy ) : AgcdEffAnim( cpy ) {cpy;};
	AgcdEffAnim_RtAnim& operator = ( const AgcdEffAnim_RtAnim& cpy ) { cpy; return *this; };
public:
	AgcdEffAnim_RtAnim( RwUInt32 dwBitFlags=0x0 );
	virtual ~AgcdEffAnim_RtAnim();
	
	virtual	BOOL				bGetTVal		( void* pOut, RwUInt32 dwCurrTime ) const;
	virtual RwInt32				bUpdateVal		( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0 );
	
	const RwChar*				bGetAnimFName	( VOID ) const		{ return m_szAnimFName; };
	const RtAnimAnimation*		bGetPtrRtAnim	( VOID ) const		{ return m_pRtAnim; };
	RwChar*						bGetAnimFName	( VOID )			{ return m_szAnimFName; };
	RwInt32						bSetAnimFName	( const RwChar* szAnimFName);
	
	//file in out
	RwInt32						bToFile			( FILE* fp );
	RwInt32						bFromFile		( FILE* fp );

public:
	RwChar				m_szAnimFName[EFF2_FILE_NAME_MAX];
	RtAnimAnimation*	m_pRtAnim;
};
typedef AgcdEffAnim_RtAnim EFFANIM_RTANIM, *LPEFFANIM_RTANIM, *PEFFANIM_RTANIM;

//--------------------- AgcdEffAnim_Scale -----------------
class AgcdEffAnim_Scale : public AgcdEffAnim, public ApMemory<AgcdEffAnim_Scale,  1500 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

private:
	//do not use
	AgcdEffAnim_Scale( const AgcdEffAnim_Scale& cpy ) : AgcdEffAnim( cpy ) {cpy;};
	AgcdEffAnim_Scale& operator = ( const AgcdEffAnim_Scale& cpy ){cpy; return *this;};
public:
	AgcdEffAnim_Scale( RwUInt32 dwBitFlags=0x0, RwInt32 nSize=0 );
	virtual ~AgcdEffAnim_Scale();

	virtual	BOOL			bGetTVal		( void* pOut, RwUInt32 dwCurrTime ) const;
	virtual	BOOL			bSetTVal		( RwInt32 nIndex, void* pIn, RwUInt32 dwTime );
	virtual BOOL			bInsTVal		( void* pIn, RwUInt32 dwTime );
	virtual BOOL			bDelTVal		( RwUInt32 dwTime );
	virtual RwInt32			bUpdateVal		( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0 );

	//file in out
	RwInt32					bToFile			( FILE* fp );
	RwInt32					bFromFile		( FILE* fp );

public:
	stTimeTable<RwV3d>	m_stTblScale;
};
typedef AgcdEffAnim_Scale EFFANIM_SCALE, *LPEFFANIM_SCALE, *PEFFANIM_SCALE;

//--------------------- AgcdEffAnim_ParticlePosScale -----------------
class AgcdEffAnim_ParticlePosScale : public AgcdEffAnim, public ApMemory<AgcdEffAnim_ParticlePosScale, 1500 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

private:
	//do not use
	AgcdEffAnim_ParticlePosScale( const AgcdEffAnim_ParticlePosScale& cpy ) : AgcdEffAnim( cpy ) {cpy;};
	AgcdEffAnim_ParticlePosScale& operator = ( const AgcdEffAnim_ParticlePosScale& cpy ){cpy; return *this;};
public:
	AgcdEffAnim_ParticlePosScale( RwUInt32 dwBitFlags=0x0, RwInt32 nSize=0 );
	virtual ~AgcdEffAnim_ParticlePosScale();

	virtual	BOOL			bGetTVal		( void* pOut, RwUInt32 dwCurrTime ) const;
	virtual	BOOL			bSetTVal		( RwInt32 nIndex, void* pIn, RwUInt32 dwTime );
	virtual BOOL			bInsTVal		( void* pIn, RwUInt32 dwTime );
	virtual BOOL			bDelTVal		( RwUInt32 dwTime );
	virtual RwInt32			bUpdateVal		( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0 );
		
	RwInt32					bToFile			( FILE* fp );
	RwInt32					bFromFile		( FILE* fp );

public:
	stTimeTable<RwReal>	m_stTblScale;
};
typedef AgcdEffAnim_ParticlePosScale EFFANIM_PARTICLEPOSSCALE, *LPEFFANIM_PARTICLEPOSSCALE, *PEFFANIM_PARTICLEPOSSCALE;

class AgcdEffAnim_PostFX : public AgcdEffAnim , public ApMemory< AgcdEffAnim_PostFX , 1500 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

private:
	AgcdEffAnim_PostFX( const AgcdEffAnim_PostFX& cpy ) : AgcdEffAnim( cpy ) {cpy;};
	AgcdEffAnim_PostFX& operator = ( const AgcdEffAnim_PostFX& cpy ){cpy; return *this;};

public:
	AgcdEffAnim_PostFX( INT32 dwBitFlags = 0 , INT32 nSize = 0) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_POSTFX, dwBitFlags )
	{

	}
	virtual ~AgcdEffAnim_PostFX() {}

	virtual BOOL			bGetTVal		( PVOID pOut, RwUInt32 dwCurrTime ) const { return TRUE; }
	virtual BOOL			bSetTVal		( RwInt32 nIndex, void* pIn, RwUInt32 dwTime ) { return TRUE; }
	virtual BOOL			bInsTVal		( void* pIn, RwUInt32 dwTime )	{ return TRUE; }
	virtual BOOL			bDelTVal		( RwUInt32 dwTime )	{ return TRUE;}
	virtual INT32			bUpdateVal		(RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved = 0 ) { return 0; }

	RwInt32					bToFile			( FILE*	fp ){ return 0; }
	RwInt32					bFromFile		( FILE* fp ){ return 0; }

public:
};

class AgcdEffAnim_Camera 
	:	public AgcdEffAnim		,
		public ApMemory< AgcdEffAnim_Camera , 300 >
{
	EFFMEMORYLOG_SMV;

	friend class AgcuEffAnimCreater;
	friend class AgcuEffIniMng;

private:
	AgcdEffAnim_Camera( const AgcdEffAnim_Camera& cpy ) : AgcdEffAnim(cpy) { }
	AgcdEffAnim_Camera& operator = ( const AgcdEffAnim_Camera& cpy ) { return *this; }

public:
	AgcdEffAnim_Camera( DWORD dwBitFlags = 0 , INT32 nSize = 0 );

	VOID					SetCameraType		( INT nCameraType )	{	m_nCameraType	=	nCameraType;	}


	VOID					SetCameraPos		( RwV3d& vPos	)					{	m_vCameraPos	=	vPos;		}
	VOID					SetCameraDir		( RwV3d& vDir	)					{	m_vCameraDir	=	vDir;		}

	VOID					SetCameraSpeed		( FLOAT	fCameraSpeed	)			{	m_fCameraSpeed	=	fCameraSpeed;	}
	VOID					SetCameraRotate		( FLOAT fDegree			)			{	m_fCameraRotate	=	fDegree;		}
	VOID					SetCameraRotateCount( FLOAT	fCount			)			{	m_fRotateCount	=	fCount;		}
	VOID					SetCameraMoveLength	( FLOAT	fMoveLength		)			{	m_fMoveLength	=	fMoveLength;	}

	INT						GetCameraType		( VOID )							{	return m_nCameraType;	}

	const RwV3d&			GetCameraPos		( VOID ) const						{	return m_vCameraPos;	}
	const RwV3d&			GetCameraDir		( VOID ) const						{	return m_vCameraDir;	}

	FLOAT					GetCameraSpeed		( VOID ) const						{	return m_fCameraSpeed;	}
	FLOAT					GetCameraRotate		( VOID ) const						{	return m_fCameraRotate;	}
	FLOAT					GetCameraRotateCount( VOID ) const						{	return m_fRotateCount;	}
	FLOAT					GetCameraMoveLength	( VOID ) const						{	return m_fMoveLength;	}
	
	RwInt32					bToFile				( FILE*	fp )	{	return 0;	}
	RwInt32					bFromFile			( FILE* fp )	{	return 0;	}

	virtual	BOOL			bGetTVal			( PVOID pOut, RwUInt32 dwCurrTime ) const { return TRUE; }
	virtual INT32			bUpdateVal			( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved /* = 0 */ );



	INT						m_nCameraType;		//	Camera Type
	RwV3d					m_vCameraPos;		//	Start Camera Position	( Off Set )
	RwV3d					m_vCameraDir;		//	Camera Direction

	FLOAT					m_fCameraSpeed;		//	Camera Speed	( Second )

	//	Rotate Camera
	FLOAT					m_fRotateCount;	
	FLOAT					m_fCameraRotate;

	//	Zoom Camera
	FLOAT					m_fMoveLength;

};

//--------------------- StCreateParamEffAnim -----------------
struct StCreateParamEffAnim
{
	StCreateParamEffAnim();
	~StCreateParamEffAnim();

	void			vZeroMemory			( VOID );

	RwInt32			bSetForColr			( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, void* pColrTable );
	RwInt32			bSetForTuTv			( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, void* pTuTvTable );
	RwInt32			bSetForMissile		( RwUInt32 dwFlag, RwUInt32 dwLife, RwReal fSpeed0, RwReal fAccel , RwReal fRotate , RwReal fRadius , RwReal fZigzagLength , RwReal fMinSpeed , RwReal fMaxSpeed );
	RwInt32			bSetForLinear		( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, void* pPosTable );
	RwInt32			bSetForRev			( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, const RwV3d& v3dAxis, void* pStRevTable );
	RwInt32			bSetForRot			( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, const RwV3d& v3dAxis, void* pStAngleTable );
	RwInt32			bSetForRpSpline		( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, const RwChar* szSplineFName );
	RwInt32			bSetForRtAnim		( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, const RwChar* szAnimFName );
	RwInt32			bSetForScale		( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, void* pScaleTable );
	RwInt32			bSetForPostFX		( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, VOID* pFXTable );
	RwInt32			bSetForCamera		( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize );

	AgcdEffAnim::E_EFFANIMTYPE	m_eEffAnimType;
	RwUInt32					m_dwBitFlags;
	RwUInt32					m_dwLife;
	E_LOOPOPT					m_eLoopOpt;

	// keyframe table
	RwInt32						m_nSize;

	RwReal						m_fMinSpeed;
	RwReal						m_fMaxSpeed;

	// missile
	RwReal						m_fSpeed;
	RwReal						m_fAccel;

	RwReal						m_fRotate;		
	RwReal						m_fRadius;		
	RwReal						m_fZigzagLength;

	// Rev/Rot
	RwV3d						m_v3dRotAxis;

	// RpSpline/RtAnimation
	RwChar						m_szFName[EFF2_FILE_NAME_MAX];

	// data table
	void*						m_pTable;

};
typedef StCreateParamEffAnim STCREATEPARAM_EFFANIM, *PSTCREATEPARAM_EFFANIM, *LPSTCREATEPARAM_EFFANIM;

//--------------------- AgcuEffAnimCreater -----------------
class AgcuEffAnimCreater
	: public SingleTon< AgcuEffAnimCreater >
{
public:
	static AgcdEffAnim*		bCreate				( LPSTCREATEPARAM_EFFANIM lpParam );
	static void				bDestroy			( AgcdEffAnim*& prEffAnim );

private:
	static AgcdEffAnim*		vCreate_Colr		( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_TuTv		( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_Missile		( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_Linear		( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_Rev			( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_Rot			( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_RpSpline	( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_RtAnim		( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_Scale		( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_PostFX		( LPSTCREATEPARAM_EFFANIM lpParam );
	static AgcdEffAnim*		vCreate_Camera		( LPSTCREATEPARAM_EFFANIM lpParam );

public:
};

#endif
