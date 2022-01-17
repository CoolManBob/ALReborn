#if !defined(AFX_AGCDEFFPUBLICSTRUCTS_H__21A01375_4005_40A9_8BC0_C5D4A16343A2__INCLUDED_)
#define AFX_AGCDEFFPUBLICSTRUCTS_H__21A01375_4005_40A9_8BC0_C5D4A16343A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "rwcore.h"
#include "rphanim.h"
#include "AcDefine.h"
 
enum	E_EFFCTRL_STATE
{
	E_EFFCTRL_STATE_WAIT		= 0,
	E_EFFCTRL_STATE_BEGIN,
	E_EFFCTRL_STATE_GOINGON,
	E_EFFCTRL_STATE_END,
	E_EFFCTRL_STATE_CONTINUE,
	E_EFFCTRL_STATE_MISSILEEND,
	E_EFFCTRL_STATE_NUM			,
};

enum AgcmEff2CallbackPoint
{
	AGCMEFF2_CB_ID_REMOVEEFFECT = 0
};

struct stEffSetCallBackInfo
{
	BOOL								m_bCallCB;
	ApBase*								m_pBase;
	STEFF_NOTICE_PROCESSDATA			m_stNoticeEffProcessData;
						
	AgcmEffectNoticeEffectProcessCB		m_pfNoticeEffectProcessCB;	// Effect Start & End & Missile End 등의 Process상황을 보고 받을 CB Function
	PVOID								m_pNoticeCBClass;			// Notice CB을 가지고 있는 Class

	stEffSetCallBackInfo();
};
typedef stEffSetCallBackInfo EFFSET_CBINFO, *PEFFSET_CBINFO, *LPEFFSET_CBINFO;

struct stTialInfoNodeBase
{
	RwFrame*	m_pFrmNode1;
	RwFrame*	m_pFrmNode2;
};

struct stTailInfoHeightBase
{
	RwFrame*	m_pFrmTailTarget;
	RwReal		m_fHeight1;
	RwReal		m_fHeight2;
};

struct stTailInfo
{
	enum eTailInfoType
	{
		e_nodebase		= 0,
		e_heightbase,
	};

	stTailInfo();
	void CloneTailInfo(stTailInfo& Info)
	{
		m_eType = Info.m_eType;
		m_stNodeInfo.m_pFrmNode1 = Info.m_stNodeInfo.m_pFrmNode1;
		m_stNodeInfo.m_pFrmNode2 = Info.m_stNodeInfo.m_pFrmNode2;
		m_stHeightInfo.m_pFrmTailTarget = Info.m_stHeightInfo.m_pFrmTailTarget;
		m_stHeightInfo.m_fHeight1		= Info.m_stHeightInfo.m_fHeight1;
		m_stHeightInfo.m_fHeight2		= Info.m_stHeightInfo.m_fHeight2;
	}

	eTailInfoType			m_eType;
	stTialInfoNodeBase		m_stNodeInfo;
	stTailInfoHeightBase	m_stHeightInfo;
};
typedef stTailInfo STTAILINFO, *PSTTAILINFO, *LPSTTAILINFO;

struct stMissileTargetInfo
{
	stMissileTargetInfo();
	
	void CloneMissileTargetInfo(stMissileTargetInfo& Info)
	{
		m_pFrmTarget	= Info.m_pFrmTarget;
		m_v3dCenter		= Info.m_v3dCenter;	
		m_v3dTarget		= Info.m_v3dTarget;
		m_v3dGP0		= Info.m_v3dGP0;	
		m_v3dGP1		= Info.m_v3dGP1;
		m_v3dLastPt		= Info.m_v3dLastPt;

		m_v3dLastPos	= Info.m_v3dLastPos;

		m_nRadius		= Info.m_nRadius;
		m_nZigzagType	= Info.m_nZigzagType;
		m_nRotateSpeed	= Info.m_nRotateSpeed;
	}

	RwFrame*	m_pFrmTarget;
	RwV3d		m_v3dCenter;	//relative to ltm.pos : rwsphere.center - ltm.pos
	RwV3d		m_v3dTarget;

	// for linear_rot , bezier3_rot 
	RwV3d		m_v3dLastPos;	// Rotate Data
	RwInt32		m_nRotateSpeed;	// Rotate Speed

	// for linear_rot , bezier3_rot , zigzag
	RwInt32		m_nRadius;	

	// for zigzag	0 = left right , 1 = up down
	RwInt32		m_nZigzagType;

	//for bezier3
	RwV3d		m_v3dGP0;	//GP : GuidPoint
	RwV3d		m_v3dGP1;
	RwV3d		m_v3dLastPt;

};
typedef stMissileTargetInfo	MISSILETARGETINFO, *PMISSILETARGETINFO, *LPMISSILETARGETINFO;


struct stEffUseInfo
{
	enum {
		E_FLAG_LINKTOPARENT			= 0x00000001	,	//부모 프레임을 가져야 할때
		E_FLAG_NOSCALE				= 0x00000002	,	//부모 프레임을 가지나 스케일 영향은 받고 싶지 안을때..
		E_FLAG_MAINCHARAC			= 0x00000004	,	//자신의 케릭터에서 사용하는 Effect인가 - 3D Sound를 Stereo로 전환할때 필요 
		E_FLAG_DIR_PAR_TO_TAR		= 0x00000008	,	//방향기준을 parent에서 target으로 향한다.
		E_FLAG_DIR_TAR_TO_PAR		= 0x00000010	,	//방향기준을 target에서 parent으로 향한다.
		E_FLAG_DIR_IGN_HEIGHT		= 0x00000020	,	//방향을 정할 때 높이값을 무시한다.

		E_FLAG_EMITER_WITH_CLUMP	= 0x00000040	,
		E_FLAG_EMITER_WITH_ATOMIC	= 0x00000080	,

		E_FLAG_STATICEFFECT			= 0x00010000	,	//정적인 이펙트( 옥트리에 넣을것들 )
	};

	stEffUseInfo();

	void CloneEffUseInfo(stEffUseInfo* Info)
	{
		if( Info )
			*this = *Info;
		/* ??
		m_ulEffID				= Info->m_ulEffID;
		m_v3dCenter				= Info->m_v3dCenter;
		m_fScale				= Info->m_fScale;
		m_fParticleNumScale		= Info->m_fParticleNumScale;
		m_fTimeScale			= Info->m_fTimeScale;
		m_rgbScale				= Info->m_rgbScale;
		m_pFrmParent			= &(*Info->m_pFrmParent);
		m_pFrmTarget			= &(*Info->m_pFrmTarget);
		m_pHierarchy			= &(*Info->m_pHierarchy);
		m_pClumpParent			= &(*Info->m_pClumpParent);
		m_pClumpEmiter			= &(*Info->m_pClumpEmiter);
		m_pAtomicEmiter			= &(*Info->m_pAtomicEmiter);
		m_ulDelay				= Info->m_ulDelay;
		m_ulLife				= Info->m_ulLife;
		m_ulFlags				= Info->m_ulFlags;
		m_pBase					= &(*Info->m_pBase);
		m_lOwnerCID				= Info->m_lOwnerCID;
		m_lTargetCID			= Info->m_lTargetCID;
		m_lCustData				= Info->m_lCustData;
		m_pNoticeCBClass		= Info->m_pNoticeCBClass;
		m_fptrNoticeCB			= Info->m_fptrNoticeCB;
		m_lCustID				= Info->m_lCustID;
		m_quatRotation			= Info->m_quatRotation;
		m_pcsData				= Info->m_pcsData;
		m_bIsAddUpdateList		= Info->m_bIsAddUpdateList;
		m_vBaseDir				= Info->m_vBaseDir;

		m_stTailInfo.CloneTailInfo(Info->m_stTailInfo);
		m_stMissileTargetInfo.CloneMissileTargetInfo(Info->m_stMissileTargetInfo);
		*/
	}

	RwUInt32			m_ulEffID;

	RwV3d				m_v3dCenter;
	RwReal				m_fScale;				//이펙트 스케일
	RwReal				m_fParticleNumScale;	//파티클갯수 배수
	RwReal				m_fTimeScale;			//빠른 캐스트 스킬이펙트
	RwRGBA				m_rgbScale;				//색상 스케일링

	RwFrame*			m_pFrmParent;
	RwFrame*			m_pFrmTarget;
	RpHAnimHierarchy*	m_pHierarchy;
	RpClump*			m_pClumpParent;			//클럼프와 함께 update & render
	RpClump*			m_pClumpEmiter;
	RpAtomic*			m_pAtomicEmiter;

	RwUInt32			m_ulDelay;
	RwUInt32			m_ulLife;

	RwUInt32			m_ulFlags;

	ApBase*				m_pBase;

	INT32				m_lOwnerCID;
	INT32				m_lTargetCID;
	INT32				m_lCustData;
	PVOID				m_pNoticeCBClass;
	AgcmEffectNoticeEffectProcessCB		m_fptrNoticeCB;
	INT32				m_lCustID;

	RtQuat				m_quatRotation;
	void*				m_pcsData;
	BOOL				m_bIsAddUpdateList;

	RwV3d				m_vBaseDir;

	stTailInfo			m_stTailInfo;
	stMissileTargetInfo	m_stMissileTargetInfo;
};
typedef stEffUseInfo	STEFFUSEINFO, *PSTEFFUSEINFO, *LPSTEFFUSEINFO;

#endif