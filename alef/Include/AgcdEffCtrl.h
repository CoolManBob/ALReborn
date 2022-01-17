// AgcdEffCtrl.h: interface for the AgcdEffCtrl class.
//

// * AgcdEffCtrl_Set
//	-> AgcmEff2::UseEffSet
//		-> constructor -> vInit	-> vInitEffBaseCtrl
//								-> vInitEffBaseDpnd
//		-> bSetMissileInfo
//		-> bSetMFrmTarget  
//		-> bSetSoundType3DToSample
//		-> bSetPtrClumpParent
//		-> bSetAsyncData or bSetDirAndOffset
//		-> bSetInfo
//		-> bSetPtrClumpEmiter or bSetPtrAtomicEmiter

///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCDEFFCTRL_H__8CFB4A85_2AE1_4739_93E0_E5727A04C9B8__INCLUDED_)
#define AFX_AGCDEFFCTRL_H__8CFB4A85_2AE1_4739_93E0_E5727A04C9B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffBase.h"
#include "AgcdEffSet.h"
#include "rwcore.h"
#include "AgcdEffParticleSystem.h"
#include "AgcdEffSound.h"
#include "AgcdEffPublicStructs.h"
#include "AgcuEff2ApMemoryLog.h"

#include "ContainerUtil.h"

#include <list>
#include <algorithm>
using namespace std;


typedef const RwFrame* (*FPTR_GetFrame)(RwUInt32 dwTime);		//해당 시간의 프레임을 가져오는 함수포인터.

typedef std::list<RwFrame*>		RwFramsList;
typedef RwFramsList::iterator	RwFramsListItr;

#define	FLAG_EFFCTRLSET_SCALE				0x00010000
#define	FLAG_EFFCTRLSET_MAINCHARAC			0x00020000	//이이펙트는 자신에게 터지는 이펙트 이다.
#define	FLAG_EFFCTRLSET_STATIC				0x00040000	//정적인 이펙트는 옥트리로...
#define	FLAG_EFFCTRLSET_UPDATEWITHCLUMP		0x00080000	//Clump 업데이트시 업뎃 콜백이 불린다.
#define	FLAG_EFFCTRLSET_UPDATEWITHATOMIC	0x00100000	//Atomic 업데이트시 업뎃 콜백이 불린다.
#define FLAG_EFFCTRLSET_BEUPDATED			0x00200000
#define	FLAG_EFFCTRLSET_WILLBEASYNCPOS		0x00400000	//m_dwDelay 가 설정되어 E_EFFCTRL_STATE_BEGIN시 싱크를 맞춰준다.
#define	FLAG_EFFCTRLSET_CLUMPEMITER			0x00800000
#define	FLAG_EFFCTRLSET_ATOMICEMITER		0x01000000


//-------------------------- AgcdEffCtrl -------------------------
class AgcdEffCtrl  
{
public:
	AgcdEffCtrl( RwUInt32 dwID, E_EFFCTRL_STATE eState );
	virtual ~AgcdEffCtrl(){};

protected:
	virtual void		SetState	( E_EFFCTRL_STATE eState )	{ m_eState = eState; };

public:
	//access
	E_EFFCTRL_STATE		GetState	( VOID )const				{	return m_eState;	};
	RwUInt32			GetID		( VOID )const				{	return m_dwID;		};
	void				FlagOn		( RwUInt32 ulFlag )			{	DEF_FLAG_ON( m_ulFlag, ulFlag );	};
	void				FlagOff		( RwUInt32 ulFlag )			{	DEF_FLAG_OFF( m_ulFlag, ulFlag );	};
	RwUInt32			FlagChk		( RwUInt32 ulFlag ) const	{	return DEF_FLAG_CHK( m_ulFlag, ulFlag );	};

protected:
	RwInt32				tReturnErr	( VOID )					{ SetState(E_EFFCTRL_STATE_END); return -1; }

protected:
	//do not use!
	AgcdEffCtrl(){};
	AgcdEffCtrl(const AgcdEffCtrl& cpy){ cpy; }
	AgcdEffCtrl& operator = (const AgcdEffCtrl& cpy) { cpy; return *this; }

protected:
	RwUInt32			m_dwID;		//effSet's id, effBase's index
	RwUInt32			m_ulFlag;
	E_EFFCTRL_STATE		m_eState;
};


//-----------------------------------------------------------------------
// EftCtrlMap 의 원소를 지워주는 객체입니다.

template< typename CtrlMap >
struct EffCtrlMapEraser
{
	CtrlMap & ctrlSet_;
	typename CtrlMap::iterator iter;

	EffCtrlMapEraser( CtrlMap & ctrlSet ) 
		: ctrlSet_(ctrlSet), iter(ctrlSet.begin()), reserved_(false) {}

	void reserve() {
		reserved_ = true;
	}

	bool next( typename CtrlMap::iterator & iter ) {

		if( iter != ctrlSet_.end() )
		{
			if( reserved_ )
			{
				ctrlSet_.erase( iter++ );
				reserved_ = false;
			}
			else
			{
				iter++;
			}
		}

		return iter != ctrlSet_.end();
	}

private:
	// 복사생성금지
	EffCtrlMapEraser( EffCtrlMapEraser const & other );
	void operator=( EffCtrlMapEraser const & other );

	bool reserved_;
};


//-------------------------- AgcdEffCtrl_Base -------------------------
class AgcdEffCtrl_Set;
class AgcdEffCtrl_Base : public AgcdEffCtrl
{
public:
	enum
	{
		E_UPDATEFLAG_BILLBOARD	= 0x00000001,
		E_UPDATEFLAG_BILLBOARDY	= 0x00000002,
		E_UPDATEFLAG_SCALE		= 0x00000004,
		E_UPDATEFLAG_ROT		= 0x00000008,
		E_UPDATEFLAG_TRANS		= 0x00000010,
		E_UPDATEFLAG_RTANIM		= 0x00000020,
		E_UPDATEFLAG_COLR		= 0x00000040,
		E_UPDATEFLAG_UVRECT		= 0x00000080,
	};

public:
	explicit AgcdEffCtrl_Base(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex);
	virtual ~AgcdEffCtrl_Base(){};

	virtual RpClump*					GetPtrClump(void)	{	return NULL;	}
	virtual	RwFrame*					GetPtrFrm(void)		{	return NULL;	}
	virtual	RwInt32						Update(RwUInt32 dwAccumulateTime);
	virtual	RwInt32						Render(void)		{	return 0;		}
	virtual void						PostUpdateFrm(RwUInt32 dwCurrTime);

	//access
	const AgcdEffBase*					GetCPtrEffBase			( VOID ) const			{ return m_lpEffBase; };
	AgcdEffBase*						GetPtrEffBase			( VOID )				{ return m_lpEffBase; };
	AgcdEffRenderBase::E_EFFBLENDTYPE	GetBlendType			( VOID ) const;
	AgcdEffBase::E_EFFBASETYPE			GetBaseType				( VOID ) const;

	const AgcdEffCtrl_Set*				GetCPtrEffCtrl_Set		( VOID ) const			{	return m_pEffCtrl_Set;	}
	AgcdEffCtrl_Set*					GetPtrEffCtrl_Set		( VOID )				{	return m_pEffCtrl_Set;	}

	RwRGBA*								GetPtrRGBA				( VOID )				{ return &m_rgba; };
	STUVRECT*							GetPtrUVRect			( VOID )				{ return &m_stUVRect; };
	RwReal								GetRotAngle				( VOID ) const			{ return m_fAccumulatedAngle; };

	void								SetMissileDir			( const RwV3d& v3dDir )	{ m_v3dDirForMissile = v3dDir; };
	void								AddRotAngle				( RwReal fAngle )		{ m_fAccumulatedAngle += fAngle; };

	void								SetStateContinue		( VOID )				{ m_eState = E_EFFCTRL_STATE_CONTINUE; };

	void								ApplyRGBScale			( VOID );
	void								ApplyRGBScale			( RwRGBA* pRgba ) const;
		
	void								UpdateFlagOn			( RwUInt32 ulFlag )		{ DEF_FLAG_ON( m_dwUpdateFlag, ulFlag ); };

protected:
	void								tBeginUpdate			( VOID );
	void								tPreUpdateFrm			( VOID );
	RwInt32								tLifeCheck				( RwUInt32* pdwCurrTime, RwUInt32 dwAccumulateTime );

protected:
	//do not use!
	AgcdEffCtrl_Base(){};
	AgcdEffCtrl_Base(const AgcdEffCtrl_Base& cpy) : AgcdEffCtrl(cpy) { cpy; }
	AgcdEffCtrl_Base& operator = (const AgcdEffCtrl_Base& cpy) { cpy; return *this; }

protected:
	AgcdEffCtrl_Set*		m_pEffCtrl_Set;
	AgcdEffBase*			m_lpEffBase;
	AgcdEffAnim_RtAnim*		m_lpEffAnim_RtAnim;
	AgcdEffAnim_Scale*		m_lpEffAnim_Scale;

	RwV3d					m_v3dDirForMissile;
	RwUInt32				m_dwUpdateFlag;

	RwRGBA					m_rgba;
	STUVRECT				m_stUVRect;
	RwReal					m_fAccumulatedAngle;	//rotation이 각속도를 가지고 있으므로.
};
typedef AgcdEffCtrl_Base	EFFCTRL_BASE, *PEFFCTRL_BASE, *LPEFFCTRL_BASE;
typedef std::list< AgcdEffCtrl_Base* >			LPEffectCtrlBaseList;
typedef	LPEffectCtrlBaseList::iterator			LPEffectCtrlBaseListItr;
typedef LPEffectCtrlBaseList::const_iterator	LPEffectCtrlBaseListCItr;



//-------------------------- AgcdEffCtrl_Set -------------------------


class AgcdEffCtrl_Sound;
class AgcdEffCtrl_Set : public AgcdEffCtrl, public ApMemory<AgcdEffCtrl_Set, 5000>
{
	EFFMEMORYLOG_SMV;

	friend class AgcdEffSetMng;

	struct stEffCtrlBaseEntry
	{
		AgcdEffCtrl_Base*							m_pEffCtrlBase;
		INT32										m_nEffectID;
		INT32										m_nCreateID;

		stEffCtrlBaseEntry( void )
		{
			m_pEffCtrlBase = NULL;
			m_nEffectID = -1;
			m_nCreateID = -1;
		};

	};

	struct	stAsyncData
	{
		stAsyncData()
		{
			Clear();
		};

		void Clear()
		{
			pFrmParent	= NULL;
			pFrmTarget	= NULL;
			ulUseFlag	= 0;
			vOffset.x = vOffset.y = vOffset.z = 0.f;
		};

		RwFrame*	pFrmParent;
		RwFrame*	pFrmTarget;
		RwV3d		vOffset;
		UINT32		ulUseFlag;
	};

public:
	typedef 	map< INT32 , stEffCtrlBaseEntry >				mapCtrlBaseEntry;
	typedef		map< INT32 , stEffCtrlBaseEntry >::iterator		mapCtrlBaseEntryIter;

	explicit AgcdEffCtrl_Set(RwInt32 dwID);
	virtual ~AgcdEffCtrl_Set();

	void					Init_Set(RwUInt32 ulFlag, AgcdEffSet* pEffSet, STTAILINFO& tailInfo, RwFrame* pFrmParent, RwRGBA* pRGBScale, RwReal fParticleNumScale, RwReal fTimeScale , RwV3d vBaseDir );

	virtual void			SetState					( E_EFFCTRL_STATE eState );
	//access
	RpClump*				GetPtrClumpEmiter			( VOID )		{ return m_pClumpEmiter; };
	RpAtomic*				GetPtrAtomicEmiter			( VOID )		{ return m_pAtomicEmiter; };
	RpClump*				GetPtrClumpParent			( VOID )		{ return m_pClumpParent; };
	RwFrame*				GetFrame					( VOID )		{ return m_pFrm; };
	RwMatrix*				GetLTM						( VOID )		{ return m_pFrm ? RwFrameGetLTM( m_pFrm ) : NULL; };
	RwSphere*				GetPtrSphere				( VOID )		{ return &m_rwSphere; };
	LPEFFSET_CBINFO			GetPtrCBInfo				( VOID )		{ return &m_stCBInfo; };
	MISSILETARGETINFO*		GetPtrMissileTargetInfo		( VOID )		{ return &m_stMissileTargetInfo; };

	const RwSphere*			GetPtrSphere				( VOID ) const	{ return &m_rwSphere; };
	const AgcdEffSet*		GetPtrEffSet				( VOID ) const	{ return m_lpEffSet; };
	RwReal					GetScale					( VOID ) const	{ return m_fScale; };
	RwRGBA					GetRGBScale					( VOID ) const	{ return m_rgbaSclae; };
	RwReal					GetParticleNumScale			( VOID ) const	{ return m_fParticleNumScale; };
	const RwV3d*			GetPos						( VOID ) const	{ return m_pFrm ? RwMatrixGetPos( RwFrameGetLTM( m_pFrm ) ) : NULL; };
	const RwMatrix*			GetLTM						( VOID ) const	{ return m_pFrm ? RwFrameGetLTM( m_pFrm ) : NULL; };

	RwUInt32				GetDelay					( VOID ) const	{ return m_dwDelay; };
	RwUInt32				GetAccumulateTime			( VOID ) const	{ return m_stTimeLoop.bGetCurrTime(); };
	RwUInt32				GetContinuation				( VOID ) const	{ return m_dwContinuation; };
	RwUInt32				GetLife						( VOID ) const	{ return m_dwLife; };
	E_LOOPOPT				GetLoopOpt					( VOID ) const	{ return m_stTimeLoop.bGetLoopDir(); };

	//for map module
	AgcdEffCtrl_Base*		Get1stSound					( VOID );

	//PreRemoveFrame
	RwInt32					AddPreRemoveFrm				( RwFrame* pFrm );
	RwInt32					DelPreRemoveFrm				( RwFrame* pFrm );
	RwInt32					PreRemoveFrm				( RwFramsList& lstPreRmFrm );
	
	//setup
	void					SetPtrClumpEmiter			( RpClump* pEmiter);
	void					SetPtrAtomicEmiter			( RpAtomic* pEmiter);
	void					SetPtrClumpParent			( RpClump* pClump) { m_pClumpParent = pClump; };
	void					SetID						( RwUInt32 dwID);
	void					SetInfo						( RwUInt32 dwLife, RwUInt32 dwDelay, RwReal fScale, ApBase* pBase, RwInt32 nOwnerCID, RwInt32 nTargetCID, RwInt32 nCustData, AgcmEffectNoticeEffectProcessCB fptrNoticeEffectProcessCB, PVOID	pNoticeCBClass, RwInt32 nCustID);
	void					SetMissileInfo				( RwFrame* pFrmMissileTarget, const RwV3d* pMissileTargetCenter);
	RwInt32					SetMFrmTarget				( RwFrame* pFrm);
	RwInt32					SetSoundType3DToSample		( BOOL bMainCharac);
	void					SetEffCtrl3DSound			( AgcdEffCtrl_Sound* pEffCtrlSound) { m_pEffCtrl3DSound = pEffCtrlSound; };
	void					SetEffCtrlSoundNoFrustumChk	( AgcdEffCtrl_Sound* pEffCtrlSound) { m_pEffCtrlSound_NoFrustumChk = pEffCtrlSound; };

	RwInt32					SetLife						( RwUInt32 ulLife );
	RwInt32					SetScale					( RwReal fScale );

	RwInt32					SetDirAndOffset				( RwFrame* pFrmParent, RwFrame* pFrmTarget, const RwV3d& offset, RwUInt32 ulFlag );
	void					SetAsyncData				( RwFrame* pFrmParent, RwFrame* pFrmTarget, const RwV3d& offset, RwUInt32 ulFlag );
	
	//idle
	RwInt32					TimeUpdate					( RwUInt32 dwDifTick );
	RwInt32					Update						( VOID );
	RwInt32					Render						( VOID );
	RwInt32					RemFromRenderOrOctree		( VOID );

	BOOL					CB_EFFECT_DISTCORRECT		( PVOID pDistFloat, PVOID pNull1, PVOID pNull2 );
	
	//tone
	void					ToneDown					( VOID );
	void					ToneRestore					( VOID );

	void					SetRotation					( const RtQuat* pRot )	{	m_quatRotation = *pRot;	}
	RtQuat*					GetRotation					( VOID )				{	return &m_quatRotation;	}

	void					End							( BOOL bCallCB = TRUE );	//외부에서 이펙트를 끄고 싶을때 사용.

	static	BOOL			CB_Update					( PVOID pNull1, PVOID pThis, PVOID pNull2 );
	static	BOOL			CB_Render					( PVOID pNull1, PVOID pThis, PVOID pNull2 );

private:
	RwInt32					Async						( VOID );
	RwInt32					Init						( STTAILINFO& tailInfo , RwV3d vBaseDir );
	RwInt32					InitEffBaseCtrl				( STTAILINFO& tailInfo , RwV3d vBaseDir );
	RwInt32					InitEffBaseDpnd				( VOID );
	void					Clear						( VOID );

	RwInt32					UpdateRwSphere				( VOID );
	RwReal					UpdateBBoxHeight			( VOID );
	RwInt32					UpdateSphereCenter			( VOID );

private:
	//do not use!
	AgcdEffCtrl_Set				(const AgcdEffCtrl_Set& cpy) : AgcdEffCtrl(cpy) { cpy;					}
	AgcdEffCtrl_Set& operator = (const AgcdEffCtrl_Set& cpy)					{ cpy; return *this;	}

private:
	union
	{
		RpClump*		m_pClumpEmiter;
		RpAtomic*		m_pAtomicEmiter;
	};

	RpClump*			m_pClumpParent;
	AgcdEffSet*			m_lpEffSet;
	RwFrame*			m_pFrm;

	stTimeTableLOOP		m_stTimeLoop;

	MISSILETARGETINFO	m_stMissileTargetInfo;		//missile target Info
	stAsyncData			m_stAsyncData;				//async data
	
	RwUInt32			m_dwLife;
	RwUInt32			m_dwCreatedTime;
	RwUInt32			m_dwDelay;
	RwUInt32			m_dwContinuation;	//missile_end -> 시간연장.

	RwReal				m_fScale;
	RwReal				m_fParticleNumScale;
	RwReal				m_fTimeScale;
	RwRGBA				m_rgbaSclae;

	EFFSET_CBINFO		m_stCBInfo;

	RwSphere			m_rwSphere;

	RwBool				m_bAddedToRenderOrOctree;
	RwInt32				m_nZIndexByCamera;

	RtQuat				m_quatRotation;

	AgcdEffCtrl_Sound*	m_pEffCtrl3DSound;//3디사운드는 매 프레임마다 위치 업데잇
	AgcdEffCtrl_Sound*	m_pEffCtrlSound_NoFrustumChk;

	mapCtrlBaseEntry	m_MapEffCtrlBase;

	RwFramsList			m_listPreRemoveFrm;
};
typedef AgcdEffCtrl_Set EFFCTRL_SET, *PEFFCTRL_SET, *LPEFFCTRL_SET;
typedef std::list<LPEFFCTRL_SET>			LPEffectCtrlSetList;
typedef LPEffectCtrlSetList::iterator		LPEffectCtrlSetListItr;
typedef LPEffectCtrlSetList::const_iterator	LPEffectCtrlSetListCItr;

//-------------------------- AgcdEffCtrl_Board -------------------------
class AgcdEffCtrl_Board : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_Board, 1000>
{
	EFFMEMORYLOG_SMV;

public:
	explicit AgcdEffCtrl_Board(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	virtual ~AgcdEffCtrl_Board();

	virtual	RwInt32			Update		( RwUInt32 dwAccumulateTime );
	virtual	RwInt32			Render		( VOID );
	virtual	RwFrame*		GetPtrFrm	( VOID )		{ return m_pFrm; };

private:
	//do not use!
	AgcdEffCtrl_Board(const AgcdEffCtrl_Board& cpy) : AgcdEffCtrl_Base(cpy) { cpy; }
	AgcdEffCtrl_Board& operator = (const AgcdEffCtrl_Board& cpy) { cpy; return *this; }

private:
	RwFrame*	m_pFrm;
};

//-------------------------- AgcdEffCtrl_TerrainBoard -------------------------
class AgcdEffCtrl_TerrainBoard : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_TerrainBoard, 100>
{
	EFFMEMORYLOG_SMV;

public:
	explicit AgcdEffCtrl_TerrainBoard(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	virtual ~AgcdEffCtrl_TerrainBoard();

	virtual	RwInt32			Update		( RwUInt32 dwAccumulateTime );
	virtual	RwInt32			Render		( VOID );
	virtual	RwFrame*		GetPtrFrm	( VOID )	{ return m_pFrm; };

private:
	//do not use!
	AgcdEffCtrl_TerrainBoard(const AgcdEffCtrl_TerrainBoard& cpy) : AgcdEffCtrl_Base(cpy) { cpy; }
	AgcdEffCtrl_TerrainBoard& operator = (const AgcdEffCtrl_TerrainBoard& cpy) { cpy; return *this; }

private:
	RwFrame*				m_pFrm;
	RwReal					m_fRadius;
};

//-------------------------- AgcdEffCtrl_ParticleSysTem -------------------------
class AgcdEffCtrl_ParticleSysTem : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_ParticleSysTem, 900>
{
	EFFMEMORYLOG_SMV;

	friend class AgcdEffParticleSystem;
	
public:
	explicit AgcdEffCtrl_ParticleSysTem(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	virtual ~AgcdEffCtrl_ParticleSysTem();

	virtual void				SetState			( E_EFFCTRL_STATE eState );
	virtual void				PostUpdateFrm		( RwUInt32 dwCurrTime );
	virtual	RwInt32				Update				( RwUInt32 dwAccumulateTime );
	virtual	RwInt32				Render				( VOID );
	virtual	RwFrame*			GetPtrFrm			( VOID )	{ return m_pFrm; };

	RwV3d&						GetBaseDir			( VOID )			{	return m_vBaseDir;		}
	VOID						SetBaseDir			( RwV3d& vDir )		{	m_vBaseDir	=	vDir;	}

private:
	void						_ClearPaticle		( VOID );
	RwInt32						_PushBack_Particle	( LPPARTICLE pParticle, RwInt32 dwCapacity );
	RwInt32						_ParticleUpdate		( LPPARTICLE pParticle, RwV3d* pv3dPos, RwReal* pfAngle, RwUInt32 dwAccumulateTime, const STENVRNPARAM& crefStEnvrnParam);

	RwInt32						_RenderFP			( VOID );	//through FixedPipeline
	RwInt32						_RenderVS			( VOID );	//through VertexShader

private:
	//do not use!
	AgcdEffCtrl_ParticleSysTem(const AgcdEffCtrl_ParticleSysTem& cpy) : AgcdEffCtrl_Base(cpy) { cpy; }
	AgcdEffCtrl_ParticleSysTem& operator = (const AgcdEffCtrl_ParticleSysTem& cpy) { cpy; return *this; }

private:
	RwFrame*						m_pParent;
	RwFrame*						m_pFrm;
	LPParticleList					m_listLpParticle;
	RwUInt32						m_dwLastAccumulateTime;
	RwUInt32						m_dwLastShootTime;
	
	RwV3d							m_v3dBeforePos;

	RwV3d							m_vBaseDir;

	//particle
	AgcdEffAnim_Colr*				m_lpEffAnim_Colr;
	AgcdEffAnim_TuTv*				m_lpEffAnim_TuTv;
	AgcdEffAnim_Scale*				m_lpEffAnim_Scale;

	//emiter
	AgcdEffAnim_Missile*			m_lpEffAnim_Missile;
	AgcdEffAnim_Linear*				m_lpEffAnim_Linear;
	AgcdEffAnim_Rev*				m_lpEffAnim_Rev;
	AgcdEffAnim_Rot*				m_lpEffAnim_Rot;
	AgcdEffAnim_RpSpline*			m_lpEffAnim_RpSpline;
	AgcdEffAnim_ParticlePosScale*	m_lpEffAnim_ParticlePosScale;
};

//-------------------------- AgcdEffCtrl_ParticleSyst_SBH -------------------------
class AgcdEffCtrl_ParticleSyst_SBH : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_ParticleSyst_SBH, 500>
{
	EFFMEMORYLOG_SMV;

	friend class AgcdEffParticleSys_SimpleBlackHole;

public:
	explicit AgcdEffCtrl_ParticleSyst_SBH(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	virtual ~AgcdEffCtrl_ParticleSyst_SBH();

	//state change
	virtual void		SetState			( E_EFFCTRL_STATE eState );
	virtual void		PostUpdateFrm		( RwUInt32 dwCurrTime );
	virtual RwInt32		Update				( RwUInt32 dwAccumulateTime );	
	virtual RwInt32		Render				( VOID );
	virtual	RwFrame*	GetPtrFrm			( VOID )	{ return m_pFrm; };
	
private:
	void				_ClearPaticle		( VOID );
	RwInt32				_PushBack_Particle	( LPPARTICLE pParticle, RwInt32 nCapacity );
	RwInt32				_ParticleUpdate		( LPPARTICLE pParticle, RwV3d* pv3dPos, RwReal* pfAngle, RwUInt32 dwAccumulateTime, const STENVRNPARAM& crefStEnvrnParam );

private:
	//do not use!
	AgcdEffCtrl_ParticleSyst_SBH(const AgcdEffCtrl_ParticleSyst_SBH& cpy) : AgcdEffCtrl_Base(cpy) { cpy; }
	AgcdEffCtrl_ParticleSyst_SBH& operator = (const AgcdEffCtrl_ParticleSyst_SBH& cpy) { cpy; return *this; }

private:
	RwFrame*						m_pFrm;
	LPParticleList					m_listLpParticle;
	RwUInt32						m_dwLastAccumulateTime;
	RwUInt32						m_dwLastShootTime;

	//particle
	AgcdEffAnim_Colr*				m_lpEffAnim_Colr;
	AgcdEffAnim_TuTv*				m_lpEffAnim_TuTv;
	AgcdEffAnim_Scale*				m_lpEffAnim_Scale;
	AgcdEffAnim_ParticlePosScale*	m_lpEffAnim_ParticlePosScale;
};

//-------------------------- AgcdEffCtrl_Tail -------------------------
class AgcdEffCtrl_Tail : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_Tail,1600>
{
	EFFMEMORYLOG_SMV;

public:
	enum	eTailType
	{
		e_TailTypeBEZ	= 0,	//bezier
		e_TailTypeCMR	,		//catmule-rom
		e_TailTypeLine	,		//하나의 판

		e_TailTypeNum	,
	};

	struct TAILPOINT
	{
		RwUInt32	m_dwTime;
		RwV3d		m_v3dP1,
					m_v3dP2;
	};
	typedef std::list<TAILPOINT>							TailPointList;
	typedef TailPointList::iterator							TailPointListItr;
	typedef TailPointList::const_iterator					TailPointListCItr;
	typedef std::list<TAILPOINT>::reverse_iterator			TailPointListRItr;
	typedef std::list<TAILPOINT>::const_reverse_iterator	TailPointListCRItr;

public:
	explicit AgcdEffCtrl_Tail( AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmTarget = NULL, FPTR_GetFrame fptrGetFrame = NULL);
	virtual ~AgcdEffCtrl_Tail();

	virtual void			SetState		( E_EFFCTRL_STATE eState );
	virtual	RwInt32			Update			( RwUInt32 dwAccumulateTime );
	virtual	RwInt32			Render			( VOID );

	//setup
	RwInt32					SetTargetInfo	( RwFrame* pFrmNode1, RwFrame* pFrmNode2 );
	RwInt32					SetTargetInfo	( RwFrame* pFrmTarget, RwReal fHeight1, RwReal fHeight2);
	RwInt32					SetTargetInfo	( RwReal fHeight1, RwReal fHeight2);
	void					SetFPTR_GetFrame( FPTR_GetFrame fptrGetFrame){m_fptrGetFrame=fptrGetFrame;};

	const TailPointList&	GetCRefListTailPoint( VOID ) const	{	return m_stllistTPoint;	};

private:
	RwInt32					_CurrPointUpdate	( VOID );
	RwInt32					_FirstPointUpdate	( RwUInt32 dwAccumulateTime );
	RwInt32					_TailUpdate			( RwUInt32 dwAccumulateTime );

private:
	//do not use!
	AgcdEffCtrl_Tail(const AgcdEffCtrl_Tail& cpy) : AgcdEffCtrl_Base(cpy) { cpy; }
	AgcdEffCtrl_Tail& operator = (const AgcdEffCtrl_Tail& cpy) { cpy; return *this; }

private:
	eTailType			m_eType;

	TailPointList		m_stllistTPoint;
	TAILPOINT			m_stCurrTPoint;
	TAILPOINT			m_stStartPoint;
	RwUInt32			m_dwLastAccumulateTime;
	FPTR_GetFrame		m_fptrGetFrame;

	stTailInfo			m_stTailInfo;

	AgcdEffAnim_Colr*	m_pEffAnim_Colr;
	AgcdEffAnim_TuTv*	m_pEffAnim_TuTv;
};

//-------------------------- AgcdEffCtrl_Obj -------------------------
class AgcdEffCtrl_Obj : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_Obj, 4000>
{
	EFFMEMORYLOG_SMV;

public:
	explicit AgcdEffCtrl_Obj(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	virtual ~AgcdEffCtrl_Obj();

	virtual void			SetState		( E_EFFCTRL_STATE eState );
	virtual	RwInt32			Update			( RwUInt32 dwAccumulateTime);
	virtual	RwFrame*		GetPtrFrm		( VOID )	{ return m_pFrm; };
	virtual	RwInt32			Render			( VOID );

	RwInt32					AddPreRemoveFrm( RwFrame* pFrm );

	//setup
	void					SetCurrBlendType( AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType ) { m_eCurrBlendType = eBlendType; };

	//access
	RpClump*				GetPtrClump		( VOID )		{	return m_pClump;	};	
	RwFrame*				GetPtrFrmClump	( VOID )		{	return m_pClump ? RpClumpGetFrame( m_pClump ) : NULL;	};
	BOOL					IsAddedToWorld	( VOID ) const	{	return m_bAddedWorld; };

	VOID					ToneDown		( VOID );	
	VOID					ToneRestore		( VOID );

	AgcdEffRenderBase::E_EFFBLENDTYPE GetCurrBlendType()const	{ return m_eCurrBlendType; };

protected:
	BOOL					_ExpandForMissileChild( VOID );

private:
	//do not use!
	AgcdEffCtrl_Obj(const AgcdEffCtrl_Obj& cpy) : AgcdEffCtrl_Base(cpy) { cpy; }
	AgcdEffCtrl_Obj& operator = (const AgcdEffCtrl_Obj& cpy) { cpy; return *this; }

private:
	RwFrame*			m_pFrm;
	RpClump*			m_pClump;
	RtAnimAnimation*	m_pRtAnim;
	
	RwFramsList			m_listPreRemoveFrm;
	BOOL				m_bAddedWorld;

	AgcdEffRenderBase::E_EFFBLENDTYPE	m_eCurrBlendType;
};

//-------------------------- AgcdEffCtrl_Light -------------------------
class AgcdEffCtrl_Light : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_Light, 100>
{
	EFFMEMORYLOG_SMV;

public:
	explicit AgcdEffCtrl_Light(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	virtual ~AgcdEffCtrl_Light();

	virtual void		SetState		( E_EFFCTRL_STATE eState );
	virtual	RwFrame*	GetPtrFrm		( VOID )	{ return ( m_pLight ? RpLightGetFrame(m_pLight) : NULL); };//m_pFrm; };//
	//idle
	virtual	RwInt32		Update			( RwUInt32 dwAccumulateTime );

	RwInt32				SetLightRadius	( RwReal fRadius );

private:
	//do not use!
	AgcdEffCtrl_Light(const AgcdEffCtrl_Light& cpy) : AgcdEffCtrl_Base(cpy) { cpy; }
	AgcdEffCtrl_Light& operator = (const AgcdEffCtrl_Light& cpy) { cpy; return *this; }

private:
	RpLight*	m_pLight;
	BOOL		m_bAddedToWorld;
};

//-------------------------- AgcdEffCtrl_MFrm -------------------------
class AgcdEffCtrl_MFrm : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_MFrm, 100>
{
	EFFMEMORYLOG_SMV;

public:
	explicit AgcdEffCtrl_MFrm(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmChild=NULL);
	virtual ~AgcdEffCtrl_MFrm();

	virtual	RwFrame*	GetPtrFrm			( VOID )						{ return m_pFrm; };
	virtual	RwInt32		Update				( RwUInt32 dwAccumulateTime);

	RwInt32				SetTargetFrm		( RwFrame* pFrm );
	RwInt32				ReleaseTargetFrm	( void );

private:
	//do not use!
	AgcdEffCtrl_MFrm(const AgcdEffCtrl_MFrm& cpy) : AgcdEffCtrl_Base(cpy) { cpy; }
	AgcdEffCtrl_MFrm& operator = (const AgcdEffCtrl_MFrm& cpy) { cpy; return *this; }

private:
	RwFrame*		m_pFrm;
};

//-------------------------- AgcdEffCtrl_Sound -------------------------
class AgcdEffCtrl_Sound : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_Sound, 500>
{
	EFFMEMORYLOG_SMV;

public:
	explicit AgcdEffCtrl_Sound( AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex );
	virtual ~AgcdEffCtrl_Sound();

	void							StopSound			( VOID );
	RwInt32							PlaySound			( RwUInt32 dwAccumulateTime );

	//access
	RwUInt32						GetLoopCnt			( VOID )const { return m_dwLoopCnt; };
	RwUInt32						GetSoundIndex		( VOID )const {return m_dwSoundIndex; };
	RwUInt32						GetPlayedTime		( VOID )const {return m_dwPlayedTime; };
	AgcdEffSound::eEffSoundType		GetSoundType		( VOID )const { return m_eSoundType; };

	//setup
	void							SetVolume			( RwReal fVolume )							{ m_fVolume = fVolume; };
	void							SetSoundType		( AgcdEffSound::eEffSoundType eSoundType )	{ m_eSoundType = eSoundType; };
	void							SetLoopCnt			( RwUInt32 dwLoopCnt )						{ m_dwLoopCnt = dwLoopCnt; };

	//idle
	virtual	RwInt32					Update				( RwUInt32 dwAccumulateTime );
	RwInt32							Update3DSoundPos	( VOID );

private:
	//do not use!
	AgcdEffCtrl_Sound(const AgcdEffCtrl_Sound& cpy) : AgcdEffCtrl_Base(cpy) { cpy; }
	AgcdEffCtrl_Sound& operator = (const AgcdEffCtrl_Sound& cpy) { cpy; return *this; }

private:
	AgcdEffSound::eEffSoundType		m_eSoundType;

	RwUInt32						m_dwSoundIndex;
	BOOL							m_bPlayed;
	RwUInt32						m_dwLoopCnt;
	RwReal							m_fVolume;
	RwUInt32						m_dwPlayedTime;
};

//-------------------------- AgcdEffCtrl_PostFX -------------------------
class AgcdEffCtrl_PostFX : public AgcdEffCtrl_Base, public ApMemory<AgcdEffCtrl_PostFX, 10>
{
	EFFMEMORYLOG_SMV;

public:
	AgcdEffCtrl_PostFX(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex);
	virtual ~AgcdEffCtrl_PostFX();

	virtual void		SetState	( E_EFFCTRL_STATE eState );
	virtual	RwInt32		Update		( RwUInt32 dwAccumulateTime );

protected:

	static	AgcdEffCtrl_PostFX*	PROCESSEDPOSTFX;
	static BOOL					OLDFXSTATE;
	static BOOL					BONFX;
	static char					PIPEBU[1024];
};

class AgcdEffCtrl_Camera
	:	public	AgcdEffCtrl_Base	,
		public	ApMemory< AgcdEffCtrl_Camera , 20 >
{
	EFFMEMORYLOG_SMV;

public:
	AgcdEffCtrl_Camera( AgcdEffCtrl_Set*	lpEffCtrl_Set , AgcdEffBase*	lpEffBase , DWORD dwIndex );
	virtual ~AgcdEffCtrl_Camera( VOID );

	VOID				SetCamera	( RwCamera*	pCamera )	{	m_pCamera	=	pCamera;	}
	RwCamera*			GetCamera	( VOID )				{	return m_pCamera;			}


	virtual VOID		SetState	( E_EFFCTRL_STATE eState );
	virtual RwInt32		Update		( RwUInt32 dwAccumulateTime );

protected:
	INT					m_nType;		//	Camera Type
	RwCamera*			m_pCamera;		//	Camera Instance

	RwV3d				m_pCameraPos;	//	Camera Position
	RwV3d				m_pCameraDir;	//	Camera Direction
};



//-------------------------- AgcuEffBaseCtrlCreater -------------------------
class AgcuEffBaseCtrlCreater
{
	typedef LPEFFCTRL_BASE (*fptrCreater)( AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent );

public:
	static LPEFFCTRL_BASE	CreateCtrl( AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL );
	static void				DestroyCtrl( LPEFFCTRL_BASE& prEffCtrl_Base );

private:
	static LPEFFCTRL_BASE	_Create_Board		(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	static LPEFFCTRL_BASE	_Create_PSyst		(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	static LPEFFCTRL_BASE	_Create_PSystSBH	(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	static LPEFFCTRL_BASE	_Create_Tail		(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	static LPEFFCTRL_BASE	_Create_Object		(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	static LPEFFCTRL_BASE	_Create_Light		(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	static LPEFFCTRL_BASE	_Create_Sound		(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	static LPEFFCTRL_BASE	_Create_MFrm		(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmMoving=NULL);
	static LPEFFCTRL_BASE	_Create_TerrainB	(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	static LPEFFCTRL_BASE	_Create_PostFX		(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);
	static LPEFFCTRL_BASE	_Create_Camera		(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex, RwFrame* pFrmParent=NULL);

private:
	static fptrCreater		fPtrCreater[ AgcdEffBase::E_EFFBASE_NUM ];
};


class AgcdPreEffSet : public ApMemory<AgcdPreEffSet, 1000>
{
	EFFMEMORYLOG_SMV;

public:
	AgcdPreEffSet() : m_pEffset(NULL), m_pEffCtrl_Set(NULL)
	{
		EFFMEMORYLOG_CON;
	}

	~AgcdPreEffSet()
	{
		EFFMEMORYLOG_DES;
	}

	void CloneEffUseInfo( stEffUseInfo* Info)
	{
		m_pstEffUseInfo.CloneEffUseInfo(Info);
	}

	void CleanUp()
	{		
		m_pEffset		= NULL;
		m_pEffCtrl_Set	= NULL;
	}

	AgcdEffSet*			m_pEffset;
	AgcdEffCtrl_Set*	m_pEffCtrl_Set;
	stEffUseInfo		m_pstEffUseInfo;
};
typedef list< AgcdPreEffSet* >		PreEffSetList;
typedef PreEffSetList::iterator		PreEffSetListItr;

#define AGCMEFFCTRL_IMMEDIATE_FINISH_LIFE_VALUE	1		// 이펙트가 로딩돼면 바로 종료가 돼도록 라이프를 설정할때 사용함.

#endif