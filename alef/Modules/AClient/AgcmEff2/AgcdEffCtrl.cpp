#include "AgcdEffCtrl.h"
#include "AgcdEffGlobal.h"

#include "AgcdEffBoard.h"
#include "AgcdEffLight.h"
#include "AgcdEffMFrm.h"
#include "AgcdEffObj.h"
#include "AgcdEffSound.h"
#include "AgcdEffTail.h"
#include "AgcdEffParticleSystem.h"
#include "AgcuEffPath.h"
#include "AgcuBezierForTail.h"
#include "AgcuBillBoard.h"

#include "AgcuEffD3dVtx.h"
#include "AgcuEffVBMng.h"
#include "AgcuEffTable.h"

#include "AcuMathFunc.h"
#include "AcuSinTbl.h"
USING_ACUMATH;

#include "AcuObjecWire.h"
#include "AcuIMDraw.h"

#include "AgpmSiegeWar.h"

#include "AgcmEventEffect.h"
#include "cslog.h"


#ifdef _CALCBOUNDINFO

class CALCBOUNDINFO
{
private:
	static BOOL		m_begin;
	static RwBBox	m_box;
	static RwSphere	m_sphere;
public:
	static	void BEGIN()	{
		m_begin	= TRUE;
		m_box.inf.x = m_box.inf.y = m_box.inf.z = 9999.f;
		m_box.sup.x = m_box.sup.y = m_box.sup.z = -9999.f;
		m_sphere.center.x = m_sphere.center.y = m_sphere.center.z = 0.f;
		m_sphere.radius	= 0.f;
	}

	static	void END(RwBBox* pBox=NULL, RwSphere* pSphere=NULL)	{
		m_begin	= FALSE;
		if( !pBox || !pSphere )
			return;
		*pBox = m_box;
		*pSphere = m_sphere;
	}

	static	RwBBox* GetBox() { return &m_box; }
	static	RwSphere* GetSphere() { return &m_sphere; }
	static	BOOL Begin() { return m_begin; }

	static	VOID RENDER(const RwSphere* pSphere=NULL, const RwBBox* pBox=NULL, const RwFrame* pFrm=NULL)	{
		Eff2Ut_RenderBSphere( pSphere ? *pSphere : m_sphere, 0xffff0000, pFrm );
		Eff2Ut_RenderBBox( pBox ? *pBox : m_box, pFrm );
	}
	static	VOID RENDER_SPHERE(const RwSphere* pSphere=NULL, const RwFrame* pFrm=NULL)	{
		if( pSphere )		Eff2Ut_RenderBSphere( *pSphere, 0xffff0000, pFrm );
	}
	static	VOID RENDER_BOX(const RwBBox* pBox=NULL, const RwFrame* pFrm=NULL)	{
		if( pBox )		Eff2Ut_RenderBBox( *pBox, pFrm );
	}
};

BOOL		CALCBOUNDINFO::m_begin	= FALSE;
RwBBox		CALCBOUNDINFO::m_box	= {{-9999.0f,-9999.0f,-9999.0f},{9999.0f,9999.0f,9999.0}};
RwSphere	CALCBOUNDINFO::m_sphere	= {{0.f,0.f,0.f},0.f};

RwBBox*		CALCBOUNDINFO_BOX		= CALCBOUNDINFO::GetBox();
RwSphere*	CALCBOUNDINFO_SPHERE	= CALCBOUNDINFO::GetSphere();
#endif // _CALCBOUNDINFO


EFFMEMORYLOG( AgcdEffCtrl_Set				 );
EFFMEMORYLOG( AgcdEffCtrl_Board				 );
EFFMEMORYLOG( AgcdEffCtrl_TerrainBoard		 );
EFFMEMORYLOG( AgcdEffCtrl_ParticleSysTem	 );
EFFMEMORYLOG( AgcdEffCtrl_ParticleSyst_SBH	 );
EFFMEMORYLOG( AgcdEffCtrl_Tail				 );
EFFMEMORYLOG( AgcdEffCtrl_Obj				 );
EFFMEMORYLOG( AgcdEffCtrl_Light				 );
EFFMEMORYLOG( AgcdEffCtrl_MFrm				 );
EFFMEMORYLOG( AgcdEffCtrl_Sound				 );
EFFMEMORYLOG( AgcdEffCtrl_PostFX			 );


//-------------------------- AgcdEffCtrl -------------------------
AgcdEffCtrl::AgcdEffCtrl( RwUInt32 dwID, E_EFFCTRL_STATE eState ) : m_dwID( dwID ), m_eState( eState ), m_ulFlag( 0 )
{
}


//-------------------------- AgcdEffCtrl_Base -------------------------
AgcdEffCtrl_Base::AgcdEffCtrl_Base(AgcdEffCtrl_Set* lpEffCtrl_Set, AgcdEffBase* lpEffBase, RwUInt32 dwIndex) : AgcdEffCtrl( dwIndex, E_EFFCTRL_STATE_WAIT ), 
 m_pEffCtrl_Set( lpEffCtrl_Set ),
 m_lpEffBase( lpEffBase ),
 m_dwUpdateFlag( 0 ),
 m_lpEffAnim_RtAnim( NULL ),
 m_lpEffAnim_Scale( NULL ),
 m_fAccumulatedAngle( 0.f )
{
	m_rgba.alpha =  m_rgba.red =  m_rgba.green = m_rgba.blue = 255;

	ASSERT( m_pEffCtrl_Set );
	ASSERT( m_lpEffBase );
	if( !lpEffBase )
	{
		Eff2Ut_ERR( "m_lpEffBase == NULL @ AgcdEffCtrl_Base::CONSTRUCTOR" );
		SetState( E_EFFCTRL_STATE_END );
	}

	else
	{
		//AgcdEffAnim_Scale
		EFFBASE::LPEffAniVecItr	it_curr = m_lpEffBase->bGetRefAnimList().begin();
		EFFBASE::LPEffAniVecItr	it_last = m_lpEffBase->bGetRefAnimList().end();
		LPEFFANIM	pEffAnim	= NULL;
		for( ; it_curr != it_last; ++it_curr )
		{
			pEffAnim	= (*it_curr);
			ASSERT( pEffAnim );
			if( !pEffAnim )
			{
				Eff2Ut_ERR( "pEffAnim == NULL @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
				SetState(E_EFFCTRL_STATE_END);
				break;
			}

			if( pEffAnim->bGetAnimType() == AgcdEffAnim::E_EFFANIM_SCALE )
			{
				m_lpEffAnim_Scale	= static_cast<AgcdEffAnim_Scale*>( pEffAnim );
				break;
			}
		}
	}
}


void AgcdEffCtrl_Base::tBeginUpdate()
{
	m_dwUpdateFlag	= 0;
}

void AgcdEffCtrl_Base::tPreUpdateFrm()
{
	RwFrame*	pFrm	= GetPtrFrm();
	if( !pFrm )		return;

	RwMatrix*	pModelling = RwFrameGetMatrix(pFrm);

	if( m_lpEffBase->bIsRenderBase() )
	{
		if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASE_BILLBOARD ) )
		{
			DEF_FLAG_ON( m_dwUpdateFlag, E_UPDATEFLAG_BILLBOARD );
			RwMatrixTransform( pModelling, BMATRIX::bGetInst().bGetPtrMatB(), rwCOMBINEREPLACE );
		}
		else if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASE_BILLBOARDY ) )
		{
			DEF_FLAG_ON( m_dwUpdateFlag, E_UPDATEFLAG_BILLBOARDY );
			RwMatrixTransform( pModelling, BMATRIX::bGetInst().bGetPtrMatBY(), rwCOMBINEREPLACE );
		}
		else
		{
			RwMatrixSetIdentity( pModelling );
		}

		RwMatrixTransform( pModelling, static_cast<AgcdEffRenderBase*>(m_lpEffBase)->bGetCPtrMat_Trans(), rwCOMBINEPOSTCONCAT );
	}
	else
	{
		RwMatrixSetIdentity( pModelling );
	}

	RwFrameUpdateObjects( pFrm );
}

void AgcdEffCtrl_Base::PostUpdateFrm(RwUInt32 dwCurrTime)
{
	RwFrame*	pFrame = GetPtrFrm();
	ASSERT(pFrame);
	if( !pFrame )		return;

	RwMatrix*	pModelling = RwFrameGetMatrix(pFrame);
	ASSERT(pModelling);
	if( !pModelling )	return;

	BOOL		bFrameDirty = FALSE;
	if( m_lpEffBase->bIsRenderBase() )
	{
		const RwMatrix* pMatTemp = static_cast<AgcdEffRenderBase*>(m_lpEffBase)->bGetCPtrMat_Rot();
		RwMatrixTransform( pModelling, pMatTemp, rwCOMBINEPRECONCAT );
		bFrameDirty = TRUE;
	}

	if( DEF_FLAG_CHK( m_dwUpdateFlag, E_UPDATEFLAG_BILLBOARD ) || DEF_FLAG_CHK( m_dwUpdateFlag, FLAG_EFFBASE_BILLBOARDY ) )
	{
		ASSERT( "kday" && m_lpEffBase->bIsRenderBase() );			
		RwV3d		v3dPos		= pModelling->pos;
		RwFrame*	pFrmParent	= RwFrameGetParent( pFrame );

		if( pFrmParent )
		{
			RwMatrix	matInvParent;
			RwMatrix	matScale;
			RwMatrix*	pLTM = RwFrameGetLTM(pFrmParent);
			Eff2Ut_RwMatrixGetScale( *pLTM, &matScale );

			RwMatrixInvert( &matInvParent, pLTM );
			RwMatrixTransform( pModelling, &matScale, rwCOMBINEPOSTCONCAT );
			RwMatrixTransform( pModelling, &matInvParent, rwCOMBINEPOSTCONCAT );
			bFrameDirty = TRUE;
		}

		pModelling->pos	= v3dPos;
	}

	if( m_lpEffAnim_Scale )
		m_lpEffAnim_Scale->bUpdateVal( dwCurrTime, this );

	if( bFrameDirty )
		RwFrameUpdateObjects( pFrame );
}

RwInt32 AgcdEffCtrl_Base::tLifeCheck( RwUInt32* pdwCurrTime, RwUInt32 dwAccumulateTime )
{
	ASSERT( pdwCurrTime );

	switch( GetState() )
	{
	case E_EFFCTRL_STATE_WAIT:
		if( m_lpEffBase->bGetDelay() > dwAccumulateTime )
			return 0;
		else
			SetState(E_EFFCTRL_STATE_BEGIN);
		break;
	case E_EFFCTRL_STATE_BEGIN:
		SetState( E_EFFCTRL_STATE_GOINGON );
		break;
	case E_EFFCTRL_STATE_END:
		return 1;
	default:
		break;
	}

	RwInt32 nTime = CalcKeyTime( pdwCurrTime, m_lpEffBase->bGetLoopOpt(), dwAccumulateTime - m_lpEffBase->bGetDelay(), m_lpEffBase->bGetLife() + m_pEffCtrl_Set->GetContinuation());
	if( nTime )
	{
		if( T_ISMINUS4( nTime ) )
		{
			Eff2Ut_ERR( "CalcKeyTime is failed @ AgcdEffCtrl_Base::tLifeCheck" );
			return tReturnErr();
		}

		SetState( E_EFFCTRL_STATE_END );
	}

	return nTime;
}
//-----------------------------------------------------------------------------
// bUpdate
// -1 : err, 0 : continue, 1 : end
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Base::Update(RwUInt32 dwAccumulateTime)
{
	PROFILE("AgcdEffCtrl_Base::bUpdate");

	if( GetCPtrEffBase()->bFlagChk(FLAG_EFFBASE_MISSILE) && E_EFFCTRL_STATE_CONTINUE == m_eState )		return 0;

	ASSERT( m_lpEffBase );
	if( !m_lpEffBase )
	{
		Eff2Ut_ERR( "m_lpEffBase == NULL @ AgcdEffCtrl_Base::bUpdate" );
		return tReturnErr();
	}

	tBeginUpdate();

	RwUInt32	dwCurrTime	= 0;
	RwInt32		ir			= tLifeCheck( &dwCurrTime, dwAccumulateTime );
	if( ir )
	{
		SetState( E_EFFCTRL_STATE_END );
		return ir;
	}

	if( GetState() == E_EFFCTRL_STATE_WAIT )
		return 0;

	
	if( m_lpEffAnim_RtAnim )
		m_lpEffAnim_RtAnim->bUpdateVal( dwCurrTime, this );

	tPreUpdateFrm();

	EFFBASE::LPEffAniVecItr	it_curr = m_lpEffBase->bGetRefAnimList().begin();
	EFFBASE::LPEffAniVecItr	it_last	= m_lpEffBase->bGetRefAnimList().end();
	LPEFFANIM	pEffAnim	= NULL;
	for( ; it_curr != it_last; ++it_curr )
	{
		pEffAnim	= (*it_curr);
		ASSERT( pEffAnim );
		if( !pEffAnim )
		{
			Eff2Ut_ERR( "pEffAnim == NULL @ AgcdEffCtrl_Base::bUpdate" );
			return tReturnErr();
		}

		if( pEffAnim->bGetAnimType() == AgcdEffAnim::E_EFFANIM_RTANIM	||
			pEffAnim->bGetAnimType() == AgcdEffAnim::E_EFFANIM_SCALE	)
			continue;

		ir = pEffAnim->bUpdateVal( dwCurrTime, this );
		if( ir )
		{
			ASSERT( !T_ISMINUS4(ir) );
			if( T_ISMINUS4(ir) )
			{
				Eff2Ut_ERR( "pEffAnim->bUpdateVal failed @ AgcdEffCtrl_Base::bUpdate" );
				Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - base index : %d", m_dwID) );
				return tReturnErr();
			}
			else if( pEffAnim->bGetAnimType() == AgcdEffAnim::E_EFFANIM_MISSILE )
			{
				SetState( E_EFFCTRL_STATE_END );
				m_pEffCtrl_Set->SetState( E_EFFCTRL_STATE_MISSILEEND );
			}
		}
	}

	PostUpdateFrm(dwCurrTime);

	return 0;
}

AgcdEffRenderBase::E_EFFBLENDTYPE AgcdEffCtrl_Base::GetBlendType(void)const
{
	ASSERT( "kday" && m_lpEffBase->bIsRenderBase() );
	return static_cast<AgcdEffRenderBase*>(m_lpEffBase)->bGetBlendType();
}

AgcdEffBase::E_EFFBASETYPE AgcdEffCtrl_Base::GetBaseType(void)const
{
	ASSERT( "kday" && m_lpEffBase );
	return m_lpEffBase->bGetBaseType();
};

void AgcdEffCtrl_Base::ApplyRGBScale(void)
{
	ApplyRGBScale( &m_rgba );
}

void AgcdEffCtrl_Base::ApplyRGBScale(RwRGBA* pRgba)const
{
	RwRGBA	rgbscale = m_pEffCtrl_Set->GetRGBScale();
	if( *(RwInt32*)(&rgbscale) == 0xffffffff )
		return;

	RwV4d	colrscl	=
	{ 
		static_cast<RwReal>(rgbscale.red)		* 0.00392157f, // 1/255 == 0.00392157f
		static_cast<RwReal>(rgbscale.green)		* 0.00392157f,
		static_cast<RwReal>(rgbscale.blue)		* 0.00392157f,
		static_cast<RwReal>(rgbscale.alpha)		* 0.00392157f
	};
	
	RwUInt32	ulR	= static_cast<RwUInt32>( static_cast<RwReal>(pRgba->red) * colrscl.x );
	RwUInt32	ulG	= static_cast<RwUInt32>( static_cast<RwReal>(pRgba->green) * colrscl.y );
	RwUInt32	ulB	= static_cast<RwUInt32>( static_cast<RwReal>(pRgba->blue) * colrscl.z );
	RwUInt32	ulA	= static_cast<RwUInt32>( static_cast<RwReal>(pRgba->alpha) * colrscl.w );

	pRgba->red		= ulR > 255UL ? 255 : static_cast<RwUInt8>( ulR );
	pRgba->green	= ulG > 255UL ? 255 : static_cast<RwUInt8>( ulG );
	pRgba->blue		= ulB > 255UL ? 255 : static_cast<RwUInt8>( ulB );
	pRgba->alpha	= ulA > 255UL ? 255 : static_cast<RwUInt8>( ulA );
}

#define LOG_EFFSET_ADD(a)
#define LOG_EFFSET_DEL(a)
#define LOG_EFFSET_RECB(a)
#define LOG_EFFSET_REM(a)
#define LOG_EFFSET_NRECB(a)

//-------------------------- AgcdEffCtrl_Set -------------------------
AgcdEffCtrl_Set::AgcdEffCtrl_Set(RwInt32 dwID) : AgcdEffCtrl( dwID, E_EFFCTRL_STATE_WAIT )
, m_pClumpEmiter(NULL)
, m_pClumpParent(NULL)
, m_lpEffSet(NULL)
, m_pFrm( NULL )
, m_dwDelay( 0 )
, m_dwLife( 0 )
, m_dwContinuation( 0 )
, m_fParticleNumScale ( 0.0f )
, m_fTimeScale ( 0.0f )
, m_bAddedToRenderOrOctree( FALSE )
, m_nZIndexByCamera( 0 )
, m_pEffCtrl3DSound( NULL )
, m_pEffCtrlSound_NoFrustumChk( NULL )
{
	EFFMEMORYLOG_CON;
}

AgcdEffCtrl_Set::~AgcdEffCtrl_Set()
{
	EFFMEMORYLOG_DES;

	Clear();
	LOG_EFFSET_DEL(this);
}

void AgcdEffCtrl_Set::Init_Set(RwUInt32 ulFlag, AgcdEffSet* pEffSet, STTAILINFO& tailInfo, RwFrame* pFrmParent, RwRGBA* pRGBScale, RwReal fParticleNumScale, RwReal fTimeScale , RwV3d vBaseDir )
{
	if( !pEffSet ) return;

	m_lpEffSet		=	pEffSet;

	if( m_dwLife == AGCMEFFCTRL_IMMEDIATE_FINISH_LIFE_VALUE )
	{
		// 이펙트가 로딩이 돼기도 전에 종료가 요청이 됄경우
		// 지속이펙트의경우 메모리에서 사라지지 않는 버그가 있다.
		m_stTimeLoop	=  stTimeTableLOOP(pEffSet->bGetLoopOpt());
		SetState(E_EFFCTRL_STATE_END);
		return;
	}

	m_stTimeLoop		= stTimeTableLOOP( pEffSet->bGetLoopOpt() );
	m_dwLife			= pEffSet->bGetLife();
	m_dwCreatedTime		= AgcdEffGlobal::bGetInst().bGetCurrTime();
	m_fParticleNumScale	= fParticleNumScale;
	m_fTimeScale		= fTimeScale;

	if( pRGBScale )
		m_rgbaSclae = *pRGBScale;
	else
		m_rgbaSclae.red = m_rgbaSclae.green = m_rgbaSclae.blue = m_rgbaSclae.alpha = 255;

	RtQuatInit( &m_quatRotation, 0, 0, 0, 1 );

#ifdef _CALCBOUNDINFO
	if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_CALCBOUND ) )
		CALCBOUNDINFO::BEGIN();
#endif

	if( !pEffSet )
	{
		SetState( E_EFFCTRL_STATE_END );
		return;
	}


	m_pFrm	= RwFrameCreate();
	if( !m_pFrm )
	{
		SetState(E_EFFCTRL_STATE_END);
		return;
	}

	RwMatrix*	pModelling = RwFrameGetMatrix( m_pFrm );
	RwMatrixSetIdentity( pModelling );
	if( pFrmParent )
	{
		if( DEF_FLAG_CHK( ulFlag, stEffUseInfo::E_FLAG_LINKTOPARENT) )
		{
			RwFrameAddChild( pFrmParent, m_pFrm );
		}
		else
		{
			if( pFrmParent->root )
			{
				RwMatrixTranslate( pModelling, RwMatrixGetPos( RwFrameGetLTM( pFrmParent ) ), rwCOMBINEPOSTCONCAT );
				RwFrameUpdateObjects(m_pFrm);
			}
		}
	}
	else
		RwFrameUpdateObjects( m_pFrm );


	PROFILE("vInit");
	if( T_ISMINUS4( Init(tailInfo , vBaseDir) ) )
	{
		SetState(E_EFFCTRL_STATE_END);
	}

	else
		m_rwSphere	= m_lpEffSet->bGetBSphere();

}
		
RwInt32 AgcdEffCtrl_Set::UpdateRwSphere()
{
	
	RwBBox	bbox = m_lpEffSet->bGetBBox();
	RwV3dTransformPoint( &bbox.inf, &bbox.inf, GetLTM() );
	RwV3dTransformPoint( &bbox.sup, &bbox.sup, GetLTM() );

	Eff2Ut_CalcSphere( &m_rwSphere, &bbox );
	return 0;
}
			
RwReal AgcdEffCtrl_Set::UpdateBBoxHeight()
{
	RwReal	fheight	= ( m_lpEffSet->bGetBBox().sup.y - m_lpEffSet->bGetBBox().inf.y ) * 0.5f;
	fheight *= RwV3dLength( &GetLTM()->up );
	return fheight;
}

RwInt32 AgcdEffCtrl_Set::UpdateSphereCenter()
{
	if( FlagChk( FLAG_EFFCTRLSET_STATIC ) )	return 0;

	RwV3dTransformPoint( &m_rwSphere.center, &m_lpEffSet->bGetBSphere().center, GetLTM() );
	return 0;
}

void AgcdEffCtrl_Set::SetState( E_EFFCTRL_STATE eState )
{
	CCSLog	stLog( 5 );
	switch( ( m_eState = eState ) )
	{
	case E_EFFCTRL_STATE_WAIT:
		break;
	case E_EFFCTRL_STATE_BEGIN:
		{

			if( m_bAddedToRenderOrOctree )
				break;

			m_bAddedToRenderOrOctree = TRUE;
			
			UpdateRwSphere();

			//각 이펙트 베이스에 필요한 초기값셋팅.
			// ex. 파티클 에미터의 마지막 위치값을 셋팅해야 하지 안을까?
			//
			// 2005/1/24 01:05 (이종석)
			// 여기에 Missile인지 체크를 안하고 그냥 bSetMissileInfo를 해서 TargetFrame이 없기 대문에 바로 Effect를 END 해버린다.
			// 그래서, 여기서는 꼭 Missile인지 체크해야 할 것으로 보임
			// 그리고, End로 바뀐 Effect의 경우에 AgcdEffCtrl_Sound 가 delete 되지 않는다. (확인 바람)
			if ( m_lpEffSet->bGetFlag() & FLAG_EFFSET_MISSILE )
				SetMissileInfo( m_stMissileTargetInfo.m_pFrmTarget, &m_stMissileTargetInfo.m_v3dCenter );

			if( FlagChk(FLAG_EFFCTRLSET_WILLBEASYNCPOS) )
			{
				if( T_ISMINUS4( Async() ) )
				{
					SetState( E_EFFCTRL_STATE_END );
					return;
				}
			}

			Update();

			//add to render-module or octree
			if( FlagChk( FLAG_EFFCTRLSET_STATIC ) )
			{
				ASSERT( AgcdEffGlobal::bGetInst().bGetPtrAgcmOcTree() );
				
				OcCustomDataList	param;
				param.BS = m_rwSphere;
				param.pData1 = NULL;
				param.pData2 = NULL;
				param.iAppearanceDistance = 2;
				param.pClass = this;
				param.pRenderCB = AgcdEffCtrl_Set::CB_Render;
				param.pUpdateCB = AgcdEffCtrl_Set::CB_Update;				
				param.pDistCorrectCB = NULL;

				param.piCameraZIndex = &m_nZIndexByCamera;

				param.TopVerts[0].x = param.BS.center.x - param.BS.radius;
				param.TopVerts[0].y = param.BS.center.y + param.BS.radius;
				param.TopVerts[0].z = param.BS.center.z - param.BS.radius;

				param.TopVerts[1].x = param.BS.center.x + param.BS.radius;
				param.TopVerts[1].y = param.BS.center.y + param.BS.radius;
				param.TopVerts[1].z = param.BS.center.z - param.BS.radius;

				param.TopVerts[2].x = param.BS.center.x + param.BS.radius;
				param.TopVerts[2].y = param.BS.center.y + param.BS.radius;
				param.TopVerts[2].z = param.BS.center.z + param.BS.radius;

				param.TopVerts[3].x = param.BS.center.x - param.BS.radius;
				param.TopVerts[3].y = param.BS.center.y + param.BS.radius;
				param.TopVerts[3].z = param.BS.center.z + param.BS.radius;

				AgcdEffGlobal::bGetInst().bGetPtrAgcmOcTree()->AddCustomRenderDataToOcTree( param.BS.center.x, param.BS.center.y, param.BS.center.z, &param );
			}
			else if( FlagChk( FLAG_EFFCTRLSET_UPDATEWITHCLUMP ) )
			{
				if( m_pClumpParent && m_pClumpParent->stType.pObject && ( m_pClumpParent->ulFlag & RWFLAG_RENDER_ADD ) )
				{
					if( m_pClumpParent->stType.eType & ACUOBJECT_TYPE_CHARACTER )
					{
						AgpdCharacter* pdCharacter = (AgpdCharacter*)m_pClumpParent->stType.pObject;
						if( pdCharacter )
						{
							static AgpmSiegeWar* _pcsAgpmSiegeWar = (AgpmSiegeWar*)AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->GetModule("AgpmSiegeWar");
							if(_pcsAgpmSiegeWar && pdCharacter->m_pcsCharacterTemplate )
							{
								AgpdSiegeWarADCharTemplate*	pdSiegeWarADCharTemplate = _pcsAgpmSiegeWar->GetAttachTemplateData( pdCharacter->m_pcsCharacterTemplate );
								if( pdSiegeWarADCharTemplate && pdSiegeWarADCharTemplate->m_eSiegeWarMonsterType == AGPD_SIEGE_MONSTER_CATAPULT )
									m_pClumpParent->ulFlag = m_pClumpParent->ulFlag | RWFLAG_DONOT_CULL;		//. 이 Clump는 Octree테스트를 강제로 통과함.
							}

							if( AgcdEffGlobal::bGetInst().bGetPtrFrmMainCharac() == m_stMissileTargetInfo.m_pFrmTarget && m_lpEffSet->bGetFlag() & FLAG_EFFSET_MISSILE )
							{
								m_pClumpParent->ulFlag = m_pClumpParent->ulFlag | RWFLAG_DONOT_CULL;		//. 이 Clump는 Octree테스트를 강제로 통과함.
							}
						}
					}
				}

				AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->AddUpdateInfotoClump( m_pClumpParent, this, AgcdEffCtrl_Set::CB_Update, NULL, NULL, NULL, AgcdEffCtrl_Set::CB_Render );
			}
			else
			{
				AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->AddCustomRenderObject( this, &m_rwSphere, UpdateBBoxHeight(), AgcdEffCtrl_Set::CB_Update, AgcdEffCtrl_Set::CB_Render, NULL, NULL );
			}

			if( !m_stCBInfo.m_pfNoticeEffectProcessCB )		break;

			m_stCBInfo.m_stNoticeEffProcessData.lEffectProcessType = AGCMEFF2_PROCESS_TYPE_EFFECT_START;
			if( m_stCBInfo.m_pfNoticeEffectProcessCB )
				m_stCBInfo.m_pfNoticeEffectProcessCB( m_stCBInfo.m_stNoticeEffProcessData, m_stCBInfo.m_pNoticeCBClass );
		}
		break;

	case E_EFFCTRL_STATE_GOINGON:
		break;

	case E_EFFCTRL_STATE_END:
		{
			CCSLog	stLog( 6 );

			if( m_pFrm )
			{	
				if ( RwFrameGetParent(m_pFrm) )
					RwFrameRemoveChild( m_pFrm );
				
				RwFrameDestroy( m_pFrm );
				m_pFrm = NULL;
			}

			if( FlagChk(FLAG_EFFCTRLSET_WILLBEASYNCPOS) )
				m_stAsyncData.Clear();

			AgcdEffCtrl_MFrm*	pEffCtrlMFrm = NULL;
			AgcdEffCtrl_Sound*	pEffCtrlSound = NULL;

			if(m_pClumpParent)
			{
				CCSLog	stLog( 7 );

				mapCtrlBaseEntryIter	Iter	=	m_MapEffCtrlBase.begin();
				for( ; Iter !=  m_MapEffCtrlBase.end() ; ++Iter )
				{
					stEffCtrlBaseEntry*	pEntry	=	&Iter->second;
					if( pEntry )
					{
						if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_MOVINGFRAME )
						{
							static_cast< AgcdEffCtrl_MFrm* >( pEntry->m_pEffCtrlBase )->ReleaseTargetFrm();
						}
						else if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_SOUND )
						{
							AgcdEffCtrl_Sound* pEffCtrlSound = static_cast< AgcdEffCtrl_Sound* >( pEntry->m_pEffCtrlBase );
							RwUInt32 nSoundIndex = pEffCtrlSound->GetSoundIndex();
							//if( nSoundIndex != 0 && nSoundIndex != AGCMSOUND_3D_SOUND_OVER_RANGE && nSoundIndex != AGCMSOUNT_3D_SOUND_NO_SLOT )
							//{
								AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->PreStop3DSound( nSoundIndex );
							//}
						}
					}
				}

				//m_pClumpParent = NULL;
			}
			else
			{
				CCSLog	stLog( 8 );

				mapCtrlBaseEntryIter	Iter	=	m_MapEffCtrlBase.begin();
				for( ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
				{
					stEffCtrlBaseEntry*	pEntry	=	&Iter->second;
					if( pEntry )
					{
						if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_MOVINGFRAME )
						{
							static_cast< AgcdEffCtrl_MFrm* >( pEntry->m_pEffCtrlBase )->ReleaseTargetFrm();
						}
					}
				}
			}
			
			if( m_stCBInfo.m_bCallCB )
			{
				CCSLog	stLog( 9 );
				LOG_EFFSET_RECB(this);
				AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->EnumCallback( AGCMEFF2_CB_ID_REMOVEEFFECT, this, m_stCBInfo.m_pBase );
#ifdef _DEBUG
				if( !m_stCBInfo.m_pBase )
				{
					LOG_EFFSET_NRECB(this);
				}
#endif //_DEBUG
			}
#ifdef _DEBUG
			else
			{
				LOG_EFFSET_NRECB(this);
			}
#endif //_DEBUG

			if( m_stCBInfo.m_pfNoticeEffectProcessCB )
			{
				CCSLog	stLog( 10 );
				m_stCBInfo.m_stNoticeEffProcessData.lEffectProcessType = AGCMEFF2_PROCESS_TYPE_EFFECT_END;
				m_stCBInfo.m_pfNoticeEffectProcessCB( m_stCBInfo.m_stNoticeEffProcessData, m_stCBInfo.m_pNoticeCBClass );
			}
		}
		break;
	case E_EFFCTRL_STATE_CONTINUE:
		{
			//파티클의 에미터의 파티클생성 중지.
			//테일에서의 새로운 점 생성 중지.
			SetLife( AgcdEffGlobal::bGetInst().bGetCurrTime() - m_dwCreatedTime + m_dwContinuation );
			m_stTimeLoop.bSetLoopDir( e_TblDir_none );

			mapCtrlBaseEntryIter		Iter	=	m_MapEffCtrlBase.begin();
			for(  ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
			{
				stEffCtrlBaseEntry*	pEntry	=	&Iter->second;
				if( pEntry )
				{
					pEntry->m_pEffCtrlBase->SetStateContinue();
				}
			}
		}
		break;

	case E_EFFCTRL_STATE_MISSILEEND:
		{
			if( m_stCBInfo.m_pfNoticeEffectProcessCB )
			{
				m_stCBInfo.m_stNoticeEffProcessData.lEffectProcessType = AGCMEFF2_PROCESS_TYPE_EFFECT_MISSLEEND;
				m_stCBInfo.m_pfNoticeEffectProcessCB( m_stCBInfo.m_stNoticeEffProcessData, m_stCBInfo.m_pNoticeCBClass );
			}

			mapCtrlBaseEntryIter		Iter	=	m_MapEffCtrlBase.begin();
			for(  ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
			{
				stEffCtrlBaseEntry* pEntry = &Iter->second;
				if( pEntry )
				{
					switch( pEntry->m_pEffCtrlBase->GetBaseType() )
					{
					case AgcdEffBase::E_EFFBASE_PSYS:
						m_dwContinuation = T_MAX( m_dwContinuation, static_cast<const AgcdEffParticleSystem*>(pEntry->m_pEffCtrlBase->GetCPtrEffBase())->bGetCRefPProp().m_dwParticleLife );
						break;
					case AgcdEffBase::E_EFFBASE_PSYS_SIMPLEBLACKHOLE:
						m_dwContinuation = T_MAX( m_dwContinuation, static_cast<const AgcdEffParticleSys_SimpleBlackHole*>(pEntry->m_pEffCtrlBase->GetCPtrEffBase())->bGetParticleLife() );
						break;
					case AgcdEffBase::E_EFFBASE_OBJECT:
						if( pEntry->m_pEffCtrlBase->FlagChk( FLAG_EFFBASEOBJ_CHILDOFMISSILE ) )
							m_dwContinuation = T_MAX( m_dwContinuation, pEntry->m_pEffCtrlBase->GetCPtrEffBase()->bGetLife());
						break;
					}
				}
			}

			SetState( m_dwContinuation ? E_EFFCTRL_STATE_CONTINUE : E_EFFCTRL_STATE_END );
		}
		break;
	}
}

// 외부에서 이펙트를 끄고 싶을때 사용.
void AgcdEffCtrl_Set::End(BOOL bCallCB)
{ 
	CCSLog	stLog( 4 );
	m_stCBInfo.m_bCallCB = bCallCB;
	SetState(E_EFFCTRL_STATE_END);
}

void AgcdEffCtrl_Set::SetPtrClumpEmiter(RpClump* pEmiter)
{
	if( pEmiter )
	{
		FlagOn( FLAG_EFFCTRLSET_CLUMPEMITER );
		m_pClumpEmiter	= pEmiter;
	}
}

void AgcdEffCtrl_Set::SetPtrAtomicEmiter(RpAtomic* pEmiter)
{
	if(pEmiter)
	{
		FlagOn( FLAG_EFFCTRLSET_ATOMICEMITER );
		m_pAtomicEmiter	= pEmiter;
	}
}

void AgcdEffCtrl_Set::SetID(RwUInt32 dwID)
{ 
	m_dwID = dwID; 
	LOG_EFFSET_ADD(this);
};

void AgcdEffCtrl_Set::SetInfo( RwUInt32 dwLife, RwUInt32 dwDelay, RwReal fScale, ApBase* pBase, RwInt32 nOwnerCID, RwInt32 nTargetCID, RwInt32 nCustData, 
								AgcmEffectNoticeEffectProcessCB fptrNoticeEffectProcessCB, PVOID pNoticeCBClass, RwInt32 nCustID )
{
	m_dwLife	= (T_ISMINUS4(dwLife)) ? m_dwLife : dwLife;
	m_dwDelay	= dwDelay;
	m_fScale	= fScale;

	RwV3d	vscale = { fScale, fScale, fScale };
	RwFrameScale( m_pFrm, &vscale, rwCOMBINEPRECONCAT );

	m_stCBInfo.m_pBase								= pBase;
	m_stCBInfo.m_stNoticeEffProcessData.lOwnerCID	= nOwnerCID;
	m_stCBInfo.m_stNoticeEffProcessData.lTargetCID	= nTargetCID;
	m_stCBInfo.m_stNoticeEffProcessData.lCustData	= nCustData;
	if( DEF_FLAG_CHK( GetPtrEffSet()->bGetFlag(), FLAG_EFFSET_MISSILE ) )
		m_stCBInfo.m_stNoticeEffProcessData.bMissile	= TRUE;
	m_stCBInfo.m_pfNoticeEffectProcessCB			= fptrNoticeEffectProcessCB;
	m_stCBInfo.m_pNoticeCBClass						= pNoticeCBClass;
	m_stCBInfo.m_stNoticeEffProcessData.lCustID		= nCustID;

	if( !m_dwDelay )
		SetState(E_EFFCTRL_STATE_BEGIN); 
}

void AgcdEffCtrl_Set::SetMissileInfo(RwFrame* pFrmMissileTarget, const RwV3d* pMissileTargetCenter)
{
	if( !pFrmMissileTarget )
	{
		tReturnErr();
		return;
	}
	
	m_stMissileTargetInfo.m_pFrmTarget	= pFrmMissileTarget;
	m_stMissileTargetInfo.m_v3dCenter	= *pMissileTargetCenter;
	RwV3dAdd( &m_stMissileTargetInfo.m_v3dTarget, RwMatrixGetPos(RwFrameGetLTM(pFrmMissileTarget)), pMissileTargetCenter );
	m_stMissileTargetInfo.m_v3dGP0		= *RwMatrixGetPos( RwFrameGetLTM( m_pFrm ) );

	switch( m_lpEffSet->bGetMissileInfo().m_eMissileType )
	{
	case e_missile_bezier3:
	case e_missile_bezier3_rot:
		{
			RwV3d	fromto;
			RwV3dSub( &fromto, &m_stMissileTargetInfo.m_v3dTarget, &m_stMissileTargetInfo.m_v3dGP0 );
			RwV3dNormalize( &fromto, &fromto );
			RwV3d	side;
			RwV3dCrossProduct( &side, AXISWORLD::bGetInst().bGetPtrWorldY(), &fromto );
			RwV3dNormalize( &side, &side );
			RwV3d	up;
			RwV3dCrossProduct( &up, &fromto, &side );
			RwV3dNormalize( &up, &up );
			
			RwV3dScale( &side, &side, m_lpEffSet->bGetMissileInfo().m_v3dOffset.x );
			RwV3dScale( &up, &up, m_lpEffSet->bGetMissileInfo().m_v3dOffset.y );
			RwV3dScale( &fromto, &fromto, m_lpEffSet->bGetMissileInfo().m_v3dOffset.z );
			RwV3dAdd( &m_stMissileTargetInfo.m_v3dGP1, &m_stMissileTargetInfo.m_v3dGP0, &side );
			RwV3dAdd( &m_stMissileTargetInfo.m_v3dGP1, &m_stMissileTargetInfo.m_v3dGP1, &up );
			RwV3dAdd( &m_stMissileTargetInfo.m_v3dGP1, &m_stMissileTargetInfo.m_v3dGP1, &fromto );
		}
		break;
	}
}

RwInt32 AgcdEffCtrl_Set::SetMFrmTarget(RwFrame* pFrm)
{

	mapCtrlBaseEntryIter	Iter	=	m_MapEffCtrlBase.begin();
	for(  ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
	{
		stEffCtrlBaseEntry* pEntry = &Iter->second;
		if( pEntry )
		{
			if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_MOVINGFRAME )
			{
				return static_cast< AgcdEffCtrl_MFrm* >( pEntry->m_pEffCtrlBase )->SetTargetFrm( pFrm );
			}
		}
	}


	Eff2Ut_ERR( "### can't find movingframe ###\n" );

	return -1;
}

RwInt32 AgcdEffCtrl_Set::SetSoundType3DToSample(BOOL bMainCharac)
{
	if( !bMainCharac )		return 0;

	mapCtrlBaseEntryIter	Iter	=	m_MapEffCtrlBase.begin();
	for(  ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
	{
		stEffCtrlBaseEntry* pEntry = &Iter->second;
		if( pEntry )
		{
			AgcdEffCtrl_Sound* pSoundBase = static_cast< AgcdEffCtrl_Sound* >( pEntry->m_pEffCtrlBase );

			if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_MOVINGFRAME )
			{
				if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_SOUND && 
					pEntry->m_pEffCtrlBase->GetPtrEffBase()->bFlagChk( FLAG_EFFBASESOUND_3DTOSAMEPLE ) &&
					pSoundBase->GetSoundType() == AgcdEffSound::EFFECT_SOUND_3DSOUND )
				{
					pSoundBase->SetSoundType( AgcdEffSound::EFFECT_SAMPLE_SOUND );
					SetEffCtrl3DSound( NULL );
				}
			}
		}
	}

	return 0;
}

RwInt32 AgcdEffCtrl_Set::SetLife( RwUInt32 ulLife )
{
	if( T_ISMINUS4( ulLife ) )		return 0;

	if( !ulLife )
	{
		m_stTimeLoop.bSetLoopDir( e_TblDir_infinity );
	}
	else
	{
		m_stTimeLoop.bSetLoopDir( e_TblDir_none );
		m_dwLife	= ulLife;
	}

	return 0;
}

RwInt32 AgcdEffCtrl_Set::SetScale( RwReal fScale )
{
	if( fScale != 1.f )
	{
		RwV3d vScale = { fScale, fScale, fScale };
		RwFrameScale ( m_pFrm, &vScale, rwCOMBINEPRECONCAT );
		FlagOn( FLAG_EFFCTRLSET_SCALE );
	}

	return 0;
}

RwInt32 AgcdEffCtrl_Set::SetDirAndOffset( RwFrame* pFrmParent, RwFrame* pFrmTarget, const RwV3d& vOffset, RwUInt32 ulFlag )
{
	if( !pFrmParent )		return 0;

	RwV3d	offset		= { 0.f, 0.f, 0.f };
	BOOL	bFrameDirty = FALSE;
	
	RwMatrix*	pModelling = RwFrameGetMatrix(m_pFrm);

	if( vOffset.x || vOffset.y || vOffset.z )
	{
		if( DEF_FLAG_CHK( ulFlag, stEffUseInfo::E_FLAG_LINKTOPARENT ) )
		{
			RwMatrixTranslate(pModelling, &vOffset, rwCOMBINEPOSTCONCAT );
			bFrameDirty = TRUE;
		}
		else
		{
			RwMatrix	mat;
			Eff2Ut_RwMatrixGetNoScale( *RwFrameGetLTM(pFrmParent), &mat );
			RwV3dTransformVector(&offset, &vOffset, &mat);
		
			RwMatrixTranslate ( pModelling, &offset, rwCOMBINEPOSTCONCAT );
			bFrameDirty = TRUE;
		}
	}

	if( !pFrmTarget )
	{
		if( bFrameDirty )	
			RwFrameUpdateObjects( m_pFrm );

		return 0;
	}

	if( DEF_FLAG_CHK( ulFlag, stEffUseInfo::E_FLAG_LINKTOPARENT ) && DEF_FLAG_CHK( ulFlag, stEffUseInfo::E_FLAG_NOSCALE ) )
	{
		RwMatrix matInvScale;
		Eff2Ut_RwMatrixGetInvScale(*RwFrameGetLTM(pFrmParent), &matInvScale);
		RwMatrixTransform (pModelling, &matInvScale, rwCOMBINEPOSTCONCAT);
	}
	
	if( !DEF_FLAG_CHK( ulFlag, stEffUseInfo::E_FLAG_DIR_TAR_TO_PAR ) && !DEF_FLAG_CHK( ulFlag, stEffUseInfo::E_FLAG_DIR_PAR_TO_TAR ) )
		
	{
		if( bFrameDirty )
			RwFrameUpdateObjects(m_pFrm);

		return 0;
	}

	RwV3d*	pFrom	= NULL;
	RwV3d*	pTo		= NULL;
	
	if( DEF_FLAG_CHK( ulFlag, stEffUseInfo::E_FLAG_DIR_TAR_TO_PAR ) )
	{
		pFrom	= RwMatrixGetPos( RwFrameGetLTM(pFrmTarget) );
		pTo		= RwMatrixGetPos( RwFrameGetLTM(pFrmParent) );
	}
	else
	{
		pFrom	= RwMatrixGetPos( RwFrameGetLTM(pFrmParent) );
		pTo		= RwMatrixGetPos( RwFrameGetLTM(pFrmTarget) );
	}
	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();

	RwV3d	FromTo	= { 
		  pTo->x - pFrom->x + offset.x
		, DEF_FLAG_CHK( ulFlag, stEffUseInfo::E_FLAG_DIR_IGN_HEIGHT ) ? 0.f : pTo->y - pFrom->y + offset.y
		, pTo->z - pFrom->z + offset.z };


	RwReal	sqLen  = Eff2Ut_RwV3dSqLength(FromTo);
	if( sqLen < 0.0001f )
		return 0;

	if( !DEF_FLAG_CHK( ulFlag, stEffUseInfo::E_FLAG_LINKTOPARENT ) )
	{
		//rot world-y-axis
		RwV3d	proj = FromTo;
		proj.y	= 0.f;
		
		RwV3dNormalize( &proj, &proj );
		RwReal	fDot	= RwV3dDotProduct( PTRAXISWZ, &proj );
		T_CLAMP( fDot, -1.f, 1.f );
		RwReal	fDeg	= DEF_R2D( acosf( fDot ) );
		if( FromTo.x < 0.f )
			fDeg = -fDeg;
		
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
		RwMatrixRotate( pModelling , PTRAXISWY, fDeg, rwCOMBINEPRECONCAT );
		bFrameDirty = TRUE;

		//rot local-x-axis
		if( FromTo.y != 0.f )
		{
			RwV3dNormalize( &FromTo, &FromTo );
			fDot	= RwV3dDotProduct( &FromTo, &proj );
			T_CLAMP( fDot, -1.f, 1.f );
			fDeg	= DEF_R2D( acosf( fDot ) );
			if( FromTo.y > 0.f )
				fDeg = -fDeg;

			if(bFrameDirty)
				RwFrameUpdateObjects(m_pFrm);	// 아래 GetLTM 하기 전에 dirtyflag set

			RwV3d	vaxis	= RwFrameGetLTM(m_pFrm)->right;
			RwV3dNormalize( &vaxis, &vaxis );
			
			RwMatrixRotate( pModelling, &vaxis, fDeg, rwCOMBINEPRECONCAT );
		}
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();

		
	}
	else
	{
		RwV3d	vParentDir	= *RwMatrixGetAt( RwFrameGetLTM(pFrmParent) );
		RwV3dNormalize( &FromTo, &FromTo );
		RwV3d	wy	= {0.f, 1.f, 0.f};
		RwV3dAssign( &pModelling->at, &FromTo );
		RwV3dCrossProduct( &pModelling->right, &wy, &FromTo );
		RwV3dCrossProduct( &pModelling->up, &pModelling->at, &pModelling->right );

		RwMatrix	stInvRot;
		RwMatrix	stRot;

		Eff2Ut_RwMatrixGetRot(*RwFrameGetLTM(pFrmParent), &stRot);
		RwMatrixInvert(&stInvRot, &stRot);
		RwMatrixTransform(pModelling, &stInvRot, rwCOMBINEPOSTCONCAT);
		bFrameDirty = TRUE;
	}

	if( bFrameDirty )
		RwFrameUpdateObjects( m_pFrm );

	return 0;
}
	
void AgcdEffCtrl_Set::SetAsyncData( RwFrame* pFrmParent, RwFrame* pFrmTarget, const RwV3d& offset, RwUInt32 ulFlag )
{
	m_stAsyncData.pFrmParent = pFrmParent;
	m_stAsyncData.pFrmTarget = pFrmTarget;
	m_stAsyncData.vOffset = offset;
	m_stAsyncData.ulUseFlag = ulFlag;
}

RwInt32	AgcdEffCtrl_Set::Async(void)
{
	if( !m_stAsyncData.pFrmParent )		return -1;

	RwFrameTranslate( m_pFrm, RwMatrixGetPos( RwFrameGetLTM( m_stAsyncData.pFrmParent ) ), rwCOMBINEREPLACE );
	return SetDirAndOffset( m_stAsyncData.pFrmParent, m_stAsyncData.pFrmTarget, m_stAsyncData.vOffset, m_stAsyncData.ulUseFlag );
}

RwInt32 AgcdEffCtrl_Set::InitEffBaseCtrl(STTAILINFO& tailInfo , RwV3d vBaseDir )
{
	if( !m_lpEffSet )		return tReturnErr();

	ASSERT( !T_ISMINUS4( m_lpEffSet->bGetVarSizeInfo().m_nNumOfBase ) );

	LPEFFBASE	lpEffBase	= NULL;
	RwFrame		*pFrmParent	= NULL;

	//EffBaseCtrl
	for( RwInt32 i = 0; i<m_lpEffSet->bGetVarSizeInfo().m_nNumOfBase; ++i )
	{
		lpEffBase = m_lpEffSet->bGetPtrEffBase( i );
		ASSERT( lpEffBase );
		if( !lpEffBase )
		{
			Eff2Ut_ERR( "m_lpEffSet->bGetPtrEffBase failed @ AgcdEffCtrl_Set::vInitEffBaseCtrl" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - EffSet_id : %d, this_id : %d, err_baseIndex : %d", m_lpEffSet->bGetID(), m_dwID, i ) );
			tReturnErr();
		}

		if( ( DEF_FLAG_CHK( lpEffBase->bGetFlag(), FLAG_EFFBASE_BASEDEPENDANCY ) && 
			DEF_FLAG_CHK( lpEffBase->bGetFlag(), FLAG_EFFBASE_BASEDPND_CHILD ) )	||	//BASE간 DEPENDANCY가 있고 CHILD인경우.
			DEF_FLAG_CHK( lpEffBase->bGetFlag(), FLAG_EFFBASE_MISSILE )			)		//missile 이면 Child_Frame이 되면 안된다.
			pFrmParent	= NULL;
		else if( lpEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_TAIL )	//tail Target_Frame이 필요하다.
		{
			pFrmParent	= NULL;

			LPEFFCTRL_BASE pEffCtrlBase = AgcuEffBaseCtrlCreater::CreateCtrl( this, lpEffBase, i, pFrmParent );

			ASSERT( pEffCtrlBase );
			if( !pEffCtrlBase )
			{
				Eff2Ut_ERR( "AgcuEffBaseCtrlCreater::bCreate failed @ AgcdEffCtrl_Set::vInitEffBaseCtrl" );
				Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - EffSet_id : %d, this_id : %d, err_baseIndex : %d", m_lpEffSet->bGetID(), m_dwID, i ) );
				tReturnErr();
			}

			stEffCtrlBaseEntry NewEntry;

			NewEntry.m_pEffCtrlBase = pEffCtrlBase;
			NewEntry.m_nEffectID = 0;
			//NewEntry.m_nCreateID = m_MapEffCtrlBase.size();
			NewEntry.m_nCreateID = m_MapEffCtrlBase.empty() ? 0 : m_MapEffCtrlBase.rbegin()->second.m_nCreateID + 1;

			m_MapEffCtrlBase.insert( make_pair( NewEntry.m_nCreateID , NewEntry ) );

			if( !DEF_FLAG_CHK( lpEffBase->bGetFlag(), FLAG_EFFBASE_BASEDEPENDANCY | FLAG_EFFBASE_BASEDPND_CHILD ) )
			{
				AgcdEffCtrl_Tail* pEffCtrl_Tail = static_cast<AgcdEffCtrl_Tail*>(pEffCtrlBase);
				if(pEffCtrl_Tail)
				{
					if( tailInfo.m_eType == stTailInfo::e_nodebase )
						pEffCtrl_Tail->SetTargetInfo( tailInfo.m_stNodeInfo.m_pFrmNode1, tailInfo.m_stNodeInfo.m_pFrmNode2);
					else
						pEffCtrl_Tail->SetTargetInfo( tailInfo.m_stHeightInfo.m_pFrmTailTarget, tailInfo.m_stHeightInfo.m_fHeight1, tailInfo.m_stHeightInfo.m_fHeight2 );
				}
			}

			continue;
		}
		else if( lpEffBase->bGetBaseType() != EFFBASE::E_EFFBASE_MOVINGFRAME )
		{
			pFrmParent	= m_pFrm;
		}
	
		LPEFFCTRL_BASE pEffCtrlBase = AgcuEffBaseCtrlCreater::CreateCtrl( this, lpEffBase, i, pFrmParent );
		if( !pEffCtrlBase )
		{
			Eff2Ut_ERR( "AgcuEffBaseCtrlCreater::bCreate failed @ AgcdEffCtrl_Set::vInitEffBaseCtrl" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - EffSet_id : %d, this_id : %d, err_baseIndex : %d", m_lpEffSet->bGetID(), m_dwID, i ) );
			tReturnErr();
		}

		if( pEffCtrlBase->GetBaseType()	== EFFBASE::E_EFFBASE_PSYS )
		{
			AgcdEffCtrl_ParticleSysTem*	pEffCtrl_Particle	=	static_cast< AgcdEffCtrl_ParticleSysTem* >(pEffCtrlBase);
			if( pEffCtrl_Particle )
			{
				pEffCtrl_Particle->SetBaseDir( vBaseDir );
			}
		}

		//.  EffctrlBase 추가.
		//m_listLpEffBaseCtrl.push_back(pEffCtrlBase);

		stEffCtrlBaseEntry NewEntry;

		NewEntry.m_pEffCtrlBase = pEffCtrlBase;
		NewEntry.m_nEffectID = 0;
		NewEntry.m_nCreateID = m_MapEffCtrlBase.empty() ? 0 : m_MapEffCtrlBase.rbegin()->second.m_nCreateID + 1;

		m_MapEffCtrlBase.insert( make_pair(NewEntry.m_nCreateID, NewEntry) );

		if( DEF_FLAG_CHK( lpEffBase->bGetFlag(), FLAG_EFFBASE_MISSILE ) )
		{
			if( pEffCtrlBase->GetPtrFrm() )
			{
				RwFrame*	pFrame = pEffCtrlBase->GetPtrFrm();
				RwMatrix*	pModelling  = RwFrameGetMatrix( pFrame );
				RwMatrixTranslate( pModelling, RwMatrixGetPos( RwFrameGetLTM( m_pFrm ) ), rwCOMBINEPOSTCONCAT );
				RwFrameUpdateObjects( pFrame );
			}
			else
				return tReturnErr();

		}
	}

	return 0;
}
	
RwInt32 AgcdEffCtrl_Set::InitEffBaseDpnd(void)
{
	INT32 nEntryIndex = 0;

	AgcdEffCtrl_Obj*	pEffCtrl_Obj	= NULL;

	//dependancy
	for( RwInt32 i=0; i<m_lpEffSet->bGetVarSizeInfo().m_nNumOfBaseDependancy; ++i )
	{
		const EFFSET::stBaseDependancy& baseDpnd	= m_lpEffSet->bGetCRefBaseDpnd(i);

		RwInt32	j	= 0;
		
		LPEFFCTRL_BASE			pEffCtrlBaseParent	=	NULL;
		LPEFFCTRL_BASE			pEffCtrlBaseChild	=	NULL;
		stEffCtrlBaseEntry*		pEntry				=	NULL;
		mapCtrlBaseEntryIter	Iter				=	m_MapEffCtrlBase.begin();

		nEntryIndex = baseDpnd.GetParentIndex();
		for( INT i = 0 ; Iter != m_MapEffCtrlBase.end() ; ++Iter , ++i )
		{
			if( i == nEntryIndex )
			{
				pEntry	=	&Iter->second;
				break;
			}
		}

		if( pEntry )
			pEffCtrlBaseParent		=	pEntry->m_pEffCtrlBase;


		Iter			=	m_MapEffCtrlBase.begin();
		nEntryIndex		=	baseDpnd.GetChildIndex();
		pEntry			=	NULL;
		for( INT i = 0 ; Iter != m_MapEffCtrlBase.end() ; ++Iter , ++i )
		{
			if( i == nEntryIndex )
			{
				pEntry	=	&Iter->second;
				break;
			}
		}

		if( pEntry )
			pEffCtrlBaseChild		=	pEntry->m_pEffCtrlBase;
		

		//confirm
		if( !pEffCtrlBaseParent || !pEffCtrlBaseChild )
			return tReturnErr();

		//get RwFrame
		RwFrame*	pFrmParent	= pEffCtrlBaseParent->GetPtrFrm();
		RwFrame*	pFrmChild	= pEffCtrlBaseChild->GetPtrFrm();
		
		//if the child's type is tail
		if( pEffCtrlBaseChild->GetBaseType() == EFFBASE::E_EFFBASE_TAIL )
		{
			
			AgcdEffCtrl_Tail*	pEBCtrl_Tail
				= static_cast<AgcdEffCtrl_Tail*>( pEffCtrlBaseChild );
			const AgcdEffTail* pEBTail
				= static_cast<const AgcdEffTail*>( pEffCtrlBaseChild->GetCPtrEffBase() );

			switch( pEffCtrlBaseParent->GetBaseType() )
			{
			case EFFBASE::E_EFFBASE_BOARD:
				{
					RwReal	fh1=0.f, fh2=0.f;
					pEBTail->bGetHeight(&fh1, &fh2);
					pEBCtrl_Tail->SetTargetInfo( pFrmParent, fh1, fh2 );
				}break;
			case EFFBASE::E_EFFBASE_PSYS:
				{
					RwReal	fh1=0.f, fh2=0.f;
					pEBTail->bGetHeight(&fh1, &fh2);
					pEBCtrl_Tail->SetTargetInfo( pFrmParent, fh1, fh2 );
				}break;
			case EFFBASE::E_EFFBASE_PSYS_SIMPLEBLACKHOLE:
				{
					RwReal	fh1=0.f, fh2=0.f;
					pEBTail->bGetHeight(&fh1, &fh2);
					pEBCtrl_Tail->SetTargetInfo( pFrmParent, fh1, fh2 );
				}break;
			case EFFBASE::E_EFFBASE_TAIL:
				{
					Eff2Ut_ERR( "Dependancy is not available, tail's parent is tail!" );
					return tReturnErr();
				}break;
			case EFFBASE::E_EFFBASE_OBJECT:
				{
					AgcdEffCtrl_Obj*	pEBCtrl_Obj	= 
						static_cast<AgcdEffCtrl_Obj*>( pEffCtrlBaseParent );
					//rework
					if( baseDpnd.FlagChk(FLAG_EFFBASE_DEPENDANCY_HASNODE ) )
					{
						RpHAnimHierarchy *pHierarchy	= Eff2Ut_GetHierarchyFromRwFrame( pEBCtrl_Obj->GetPtrFrmClump() );
						if( !pHierarchy )
						{
							Eff2Ut_ERR( "Eff2Ut_GetHierarchyFromRwFrame failed @ AgcdEffCtrl_Set::vInitEffBaseDpnd" );
							Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - EffSet_id : %d, this_id : %d, err_DependancyIndex : %d", m_lpEffSet->bGetID(), m_dwID, i ) );
							return tReturnErr();
						}
						RwInt32	nNodeID	= baseDpnd.GetParentNodeID();
						ASSERT( nNodeID );
						RwInt32 nNodeIndex	= RpHAnimIDGetIndex( pHierarchy, nNodeID );
						if( T_ISMINUS4( nNodeIndex ) )
						{
							Eff2Ut_ERR( "RpHAnimIDGetIndex failed @ AgcdEffCtrl_Set::vInitEffBaseDpnd" );
							Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - EffSet_id : %d, this_id : %d, err_DependancyIndex : %d", m_lpEffSet->bGetID(), m_dwID, i ) );
							return tReturnErr();
						}
						if( pHierarchy->pNodeInfo[ nNodeIndex ].pFrame )
						{
							RwReal	fh1=0.f, fh2=0.f;
							pEBTail->bGetHeight(&fh1, &fh2);
							pEBCtrl_Tail->SetTargetInfo( pHierarchy->pNodeInfo[ nNodeIndex ].pFrame, fh1, fh2 );
						}
						else
						{
							Eff2Ut_ERR( "pHierarchy->pNodeInfo[ nNodeIndex ].pFrame == NULL @ AgcdEffCtrl_Set::vInitEffBaseDpnd" );
							return tReturnErr();
						}
					}
					else
					{
						RwReal	fh1=0.f, fh2=0.f;
						pEBTail->bGetHeight(&fh1, &fh2);
						pEBCtrl_Tail->SetTargetInfo( pEBCtrl_Obj->GetPtrFrm()/*pEBCtrl_Obj->bGetPtrFrmClump()*/, fh1, fh2 );
					}
				}break;
			case EFFBASE::E_EFFBASE_LIGHT:
				{
					RwReal	fh1=0.f, fh2=0.f;
					pEBTail->bGetHeight(&fh1, &fh2);
					pEBCtrl_Tail->SetTargetInfo( pFrmParent, fh1, fh2 );
				}break;
			case EFFBASE::E_EFFBASE_SOUND:
				{
					Eff2Ut_ERR( "wrong dependancy link! @ AgcdEffCtrl_Set::vInitEffBaseDpnd" );
					Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - EffSet_id : %d, this_id : %d, err_DependancyIndex : %d", m_lpEffSet->bGetID(), m_dwID, i ) );
					return tReturnErr();
				}break;
			case EFFBASE::E_EFFBASE_MOVINGFRAME:
				{
					Eff2Ut_ERR( "wrong dependancy link! @ AgcdEffCtrl_Set::vInitEffBaseDpnd" );
					Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - EffSet_id : %d, this_id : %d, err_DependancyIndex : %d", m_lpEffSet->bGetID(), m_dwID, i ) );
					return tReturnErr();
				}break;
			}
		}
		//if the child's type is NOT tail
		else
		{
			//pre_confirm
			if( !pFrmParent || !pFrmChild )
			{
				Eff2Ut_ERR( "!pFrmParent || !pFrmChild @ AgcdEffCtrl_Set::vInitEffBaseDpnd" );
				Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - EffSet_id : %d, this_id : %d, err_DependancyIndex : %d", m_lpEffSet->bGetID(), m_dwID, i ) );
				return tReturnErr();
			}

			//child must link to some clum's node
			if( baseDpnd.FlagChk(FLAG_EFFBASE_DEPENDANCY_HASNODE) )
			{
				//dynamic cast
				AgcdEffCtrl_Obj*	pEBCtrl_Obj	= 
					static_cast<AgcdEffCtrl_Obj*>( pEffCtrlBaseParent );
				//confirm
				if( !(pEBCtrl_Obj->GetPtrFrm()) || !(pEBCtrl_Obj->GetPtrClump()) )
					return tReturnErr();

				//find right parent frame
				RwInt32	nNodeID	= baseDpnd.GetParentNodeID();
				RwInt32 nNodeIndex	= static_cast<AgcdEffObj*>(pEBCtrl_Obj->GetPtrEffBase())->bFindNodeIndex( nNodeID );
				if( T_ISMINUS4( nNodeIndex ) )
					return tReturnErr();

				RpHAnimHierarchy *pHierarchy	= Eff2Ut_GetHierarchyFromRwFrame( pEBCtrl_Obj->GetPtrFrmClump() );
				if( pHierarchy && !pHierarchy->pNodeInfo[ nNodeIndex ].pFrame )
				{
					RpHAnimHierarchyAttach( pHierarchy );
					if( !pHierarchy->pNodeInfo[ nNodeIndex ].pFrame )
						return tReturnErr();
				}

				RwFrameAddChild ( pHierarchy->pNodeInfo[ nNodeIndex ].pFrame, pFrmChild );

				pEffCtrlBaseChild->GetPtrEffBase()->bFlagOn( FLAG_EFFBASE_BASEDPND_TOOBJ );
				if( T_ISMINUS4( pEBCtrl_Obj->AddPreRemoveFrm( pFrmChild ) ) )
				{
					return tReturnErr();
				}
				if( T_ISMINUS4( AddPreRemoveFrm( pFrmChild ) ) )
				{
					return tReturnErr();
				}
			}
			//general case
			else
			{
				if( pEffCtrlBaseParent->GetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT )
				{
					AgcdEffCtrl_Obj*	pEBCtrl_Obj	= 
						static_cast<AgcdEffCtrl_Obj*>( pEffCtrlBaseParent );

					//. 2006. 3. 8. Nonstopdj
					//. clone에 실패한 clump일 경우 문제발생.
					if(pEBCtrl_Obj && NULL == pEBCtrl_Obj->GetPtrFrmClump())
						return tReturnErr();

					if( T_ISMINUS4( pEBCtrl_Obj->AddPreRemoveFrm( pFrmChild ) ) )
					{
						//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
						return tReturnErr();
					}
					if( T_ISMINUS4( AddPreRemoveFrm( pFrmChild ) ) )
					{
						//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
						return tReturnErr();
					}
					
					RwFrameAddChild ( pEBCtrl_Obj->GetPtrFrmClump(), pFrmChild );
					pEffCtrlBaseChild->GetPtrEffBase()->bFlagOn( FLAG_EFFBASE_BASEDPND_TOOBJ );
				}
				else
				{
					RwFrameAddChild ( pFrmParent, pFrmChild );
				}
				//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
			}
			// 일단 link는 걸고
			// 데이타 업데이트시 고려하여 적용 할것.
			if( baseDpnd.FlagChk(FLAG_EFFBASE_DEPENDANCY_ONLYPOS) )
			{
				pEffCtrlBaseChild->GetPtrEffBase()->bFlagOn( FLAG_EFFBASE_DEPEND_ONLYPOS );
			}
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// vInit
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Set::Init(STTAILINFO& tailInfo , RwV3d vBaseDir )
{
	
	if( T_ISMINUS4( InitEffBaseCtrl(tailInfo , vBaseDir) ) )
		return tReturnErr();

	if( T_ISMINUS4( InitEffBaseDpnd() ) )
		return tReturnErr();

	return 0;
}

//-----------------------------------------------------------------------------
// vClear
//-----------------------------------------------------------------------------
void AgcdEffCtrl_Set::Clear()
{
	RemFromRenderOrOctree();

	mapCtrlBaseEntryIter	Iter		=	m_MapEffCtrlBase.begin();
	for( ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
	{
		stEffCtrlBaseEntry*		pEntry		=	&Iter->second;
		if( pEntry )
		{
			//if( pEntry->m_pEffCtrlBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_MOVINGFRAME )
			AgcuEffBaseCtrlCreater::DestroyCtrl( pEntry->m_pEffCtrlBase );
		}
	}

	m_MapEffCtrlBase.clear();


	if( !m_listPreRemoveFrm.empty() )
	{
		Eff2Ut_ERR( " m_listPreRemoveFrm.empty() == FALSE @ AgcdEffCtrl_Set::vClear \n " );
	}

	ASSERT( m_lpEffSet );
	if( m_lpEffSet )
	{
		m_lpEffSet->bRelease();
		m_lpEffSet = NULL;
	}

	Eff2Ut_SAFE_DESTROY_FRAME( m_pFrm );
}

//-----------------------------------------------------------------------------
// bTimeUpdate
// -1 : err, 0 : continue, 1 : end
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Set::TimeUpdate( RwUInt32 dwDifTick )
{
	INT32 nEntryCount = m_MapEffCtrlBase.size();
	if( nEntryCount <= 0 )
	{
		SetState(E_EFFCTRL_STATE_END);
	}

	if( GetState() == E_EFFCTRL_STATE_END )
		return 1;
	
	dwDifTick = static_cast<RwUInt32>(m_fTimeScale * ( static_cast<RwReal>(dwDifTick) ) );

	switch( GetState() )
	{
	case E_EFFCTRL_STATE_WAIT :
		{
			RwUInt32 uGlobalCurrentTime = AgcdEffGlobal::bGetInst().bGetCurrTime();
			if( uGlobalCurrentTime-m_dwCreatedTime > m_dwDelay )
			{			
				m_stTimeLoop.bSetCurrTime(0LU);
				SetState(E_EFFCTRL_STATE_BEGIN);

				if( T_ISMINUS4( m_stTimeLoop.bAddTime( uGlobalCurrentTime - ( m_dwCreatedTime + m_dwDelay ), m_dwLife ) ) )
				{
					if( DEF_FLAG_CHK( m_lpEffSet->bGetFlag(), FLAG_EFFSET_MISSILE ) )
					{
						SetState(E_EFFCTRL_STATE_GOINGON);
						m_stTimeLoop.bSetLoopDir( e_TblDir_infinity );
						return 0;
					}

					SetState(E_EFFCTRL_STATE_END);
					return 1;
				}
				
				SetState(E_EFFCTRL_STATE_GOINGON);
			}

			return 0;
		}
		break;
	}

	if( T_ISMINUS4( m_stTimeLoop.bAddTime( dwDifTick, m_dwLife ) ) )
	{
		if( DEF_FLAG_CHK( m_lpEffSet->bGetFlag(), FLAG_EFFSET_MISSILE ) &&
			m_eState != E_EFFCTRL_STATE_CONTINUE )
		{
			SetState(E_EFFCTRL_STATE_GOINGON);
			m_stTimeLoop.bSetLoopDir( e_TblDir_infinity );
		}
		else
		{
			SetState(E_EFFCTRL_STATE_END);
			return 1;
		}
	}

	if( m_pEffCtrl3DSound )
	{
		m_pEffCtrl3DSound->Update3DSoundPos();
	}

	FlagOff( FLAG_EFFCTRLSET_BEUPDATED );

	if( m_pEffCtrlSound_NoFrustumChk )
	{
		m_pEffCtrlSound_NoFrustumChk->Update( m_stTimeLoop.bGetCurrTime() );
	}

	return 0;
}

//-----------------------------------------------------------------------------
// bUpdate
// -1 : err, 0 : continue, 1 : end
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Set::Update(void)
{
	PROFILE("AgcdEffCtrl_Set::bUpdate");

	if( GetState() == E_EFFCTRL_STATE_END )
		return 1;

	if ( m_pClumpParent && m_lpEffSet )
		AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect()->CheckExclusiveEffect(m_pClumpParent, m_lpEffSet->bGetID());

	mapCtrlBaseEntryIter	Iter		=	m_MapEffCtrlBase.begin();
	for( ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
	{
		
		stEffCtrlBaseEntry*		pEntry	=	&Iter->second;
		if( pEntry )
		{
			pEntry->m_pEffCtrlBase->Update( m_stTimeLoop.bGetCurrTime() );
		}

	}

	if( this->GetState()		==	E_EFFCTRL_STATE_END )
		return 1;

	UpdateSphereCenter();

	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Set::Render(void)
{	
#ifdef _CALCBOUNDINFO	
	if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_CALCBOUND ) )
	{
		static BOOL	bF5	= FALSE;
		if( KEYDOWN_ONECE(VK_F5, bF5) )
			CALCBOUNDINFO::BEGIN();
		static BOOL	bF4	= FALSE;
		if( KEYDOWN_ONECE(VK_F4, bF4) )
			CALCBOUNDINFO::END();
	}
#endif

	PROFILE("AgcdEffCtrl_Set::bRender");

	if ( m_pClumpParent && m_lpEffSet )
	{
		if ( AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect()->IsExclusiveEffect(m_pClumpParent, m_lpEffSet->bGetID()) )
			return 0;
	}

	if( m_pClumpParent && (m_pClumpParent->stType.eType & ACUOBJECT_TYPE_INVISIBLE) )
	{
		if ( (m_pClumpParent->stType.eType & ACUOBJECT_TYPE_FORCED_RENDER_EFFECT) == 0 )
			return 0;
	}

	if( GetState() == E_EFFCTRL_STATE_END )
		return 1;

#ifdef USE_MFC
	#ifdef _DEBUG
		if( m_pFrm )
		{
			if( AgcdEffGlobal::bGetInst().bGetCBFrmRender() )
				AgcdEffGlobal::bGetInst().bGetCBFrmRender()(m_pFrm);
		}
	#endif
#else
	if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_SHOWFRM) )
	{
		AXISVIEW::RenderFrame(m_pFrm, 0xffffff00);
	}
#endif//USE_MFC

#ifdef _DEBUG


	if( AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->m_skipEffSetID == m_lpEffSet->bGetID() )
		return 0;
	RwBool	theDbgEffSet = m_lpEffSet->bGetID() == AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->m_chkEffSetID;
	RwInt32	dbgBaseIndex = -1;
	if(theDbgEffSet)
		AXISVIEW::RenderFrame(m_pFrm, 0xffff0000);
#endif //_DEBUG


	INT32 nResult = 0;
	mapCtrlBaseEntryIter	Iter	=	m_MapEffCtrlBase.begin();
	for( ; Iter != m_MapEffCtrlBase.end() ;  )
	{
		stEffCtrlBaseEntry*	pEntry	=	&Iter->second;
		if( pEntry )
		{
			if( pEntry->m_pEffCtrlBase->GetState() == E_EFFCTRL_STATE_WAIT ||	pEntry->m_pEffCtrlBase->GetState() == E_EFFCTRL_STATE_END ) 
			{
				++Iter;
				continue;
			}

			if( GetState() == E_EFFCTRL_STATE_END ) 
				return 1;

#ifdef _DEBUG
			++dbgBaseIndex;
#endif //_DEBUG

#ifdef USE_MFC
			if( AgcdEffGlobal::bGetInst().bGetShowEffBase() && pEntry->m_pEffCtrlBase->GetCPtrEffBase() != AgcdEffGlobal::bGetInst().bGetShowEffBase() ) continue;
#endif //USE_MFC

			if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_SHOWWIRE ) )
			{
				if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT && !pEntry->m_pEffCtrlBase->GetPtrEffBase()->bFlagChk( FLAG_EFFBASEOBJ_DUMMY )	)
				{
					AcuObjecWire::bGetInst().bOnOffClump(true);
					AcuObjecWire::bGetInst().bRenderClumpOnce( static_cast<AgcdEffCtrl_Obj*>(pEntry->m_pEffCtrlBase)->GetPtrClump() );
				}
			}

			nResult = pEntry->m_pEffCtrlBase->Render();
			if( nResult )
			{
				if( T_ISMINUS4( nResult ) )		
					return tReturnErr();

				else						
				{
					AgcuEffBaseCtrlCreater::DestroyCtrl( pEntry->m_pEffCtrlBase );
					pEntry->m_pEffCtrlBase = NULL;

					m_MapEffCtrlBase.erase( Iter++ );
					continue;
				}
			}

#ifdef _DEBUG
			if( theDbgEffSet )
			{
				if( dbgBaseIndex == AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->m_dbgBaseIndex )
				{
					AXISVIEW::RenderFrame( pEntry->m_pEffCtrlBase->GetPtrFrm(), 0xffff0000 );

					RwReal lenx = RwV3dLength( RwMatrixGetRight( RwFrameGetLTM(pEntry->m_pEffCtrlBase->GetPtrFrm()) ) );
					RwReal leny = RwV3dLength( RwMatrixGetUp( RwFrameGetLTM(pEntry->m_pEffCtrlBase->GetPtrFrm()) ) );
					RwReal lenz = RwV3dLength( RwMatrixGetAt( RwFrameGetLTM(pEntry->m_pEffCtrlBase->GetPtrFrm()) ) );
				}
			}

			if( AgcdEffGlobal::bGetInst().bGetCBFrmRender() )
			{
				if( pEntry->m_pEffCtrlBase->GetPtrClump() )
				{
					AgcdEffGlobal::bGetInst().bGetCBFrmRender()(RpClumpGetFrame(pEntry->m_pEffCtrlBase->GetPtrClump()));
				}
				else if( pEntry->m_pEffCtrlBase->GetPtrFrm() )
				{
					AgcdEffGlobal::bGetInst().bGetCBFrmRender()(pEntry->m_pEffCtrlBase->GetPtrFrm());
				}
			}
#endif//_DEBUG
		}

		++Iter;
	}

	
#ifdef _CALCBOUNDINFO
	if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_CALCBOUND ) )
	{
		Eff2Ut_CalcSphere(CALCBOUNDINFO::GetSphere(), CALCBOUNDINFO::GetBox());
		CALCBOUNDINFO::RENDER();
	}
	else
	//@{ kday 20051108
	// ;)
#endif //_CALCBOUNDINFO
	{
#ifdef _CALCBOUNDINFO
		if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_SHOWBOX ) )
			CALCBOUNDINFO::RENDER_BOX( &GetPtrEffSet()->bGetBBox(), m_pFrm );
#endif //_CALCBOUNDINFO

		if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_SHOWSPHERE ) )
			AcuObjecWire::bGetInst().bRenderSphere(m_rwSphere);
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Set::RemFromRenderOrOctree()
{
	CCSLog	stLog( 300 );

	if( m_bAddedToRenderOrOctree )
	{
		if( FlagChk( FLAG_EFFCTRLSET_STATIC ) )
		{
			CCSLog	stLog( 301 );
			AgcdEffGlobal::bGetInst().bGetPtrAgcmOcTree()->RemoveCustomRenderDataFromOcTree(
				m_rwSphere.center.x
				, m_rwSphere.center.y
				, m_rwSphere.center.z
				, this
				, NULL
				, NULL
				);	
		}
		else if( FlagChk( FLAG_EFFCTRLSET_UPDATEWITHCLUMP ) )
		{
			CCSLog	stLog( 302 );
			AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->RemoveUpdateInfoFromClump3( m_pClumpParent , this );
		}
		else
		{
			CCSLog	stLog( 303 );
			AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->RemoveCustomRenderObject(
				this
				, NULL
				, NULL
				);
		}

		m_bAddedToRenderOrOctree	= FALSE;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// CB_Update
//-----------------------------------------------------------------------------
BOOL AgcdEffCtrl_Set::CB_Update( PVOID pNull1, PVOID pThis, PVOID pNull2 )
{
	PROFILE("AgcdEffCtrl_Set::CB_Update");


	AgcdEffCtrl_Set*	pCtrlSet	=	static_cast< AgcdEffCtrl_Set* >(pThis);
	AgcmEff2*			pcmEffect	=	AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2();

	if( pCtrlSet->GetState()	==	E_EFFCTRL_STATE_END )
		return FALSE;

	if( !pcmEffect ) 
		return FALSE;

	if( !pcmEffect->FindEffectSet( pCtrlSet ) )	
		return FALSE;

	if(pCtrlSet->m_lpEffSet->m_enumLoadStatus == AGCDEFFSETRESOURCELOADSTATUS_LOADING)
		return TRUE;

	if( pCtrlSet->Update() )
	{
		pCtrlSet->SetState(E_EFFCTRL_STATE_END);
		return FALSE;
	}

	pCtrlSet->FlagOn( FLAG_EFFCTRLSET_BEUPDATED );

	return TRUE;
}

//-----------------------------------------------------------------------------
// CB_Render
//-----------------------------------------------------------------------------
BOOL AgcdEffCtrl_Set::CB_Render( PVOID pNull1, PVOID pThis, PVOID pNull2 )
{
	PROFILE("AgcdEffCtrl_Set::CB_Render");

	AgcdEffCtrl_Set*	pCtrlSet	=	static_cast< AgcdEffCtrl_Set* >(pThis);
	AgcmEff2*			pcmEffect	=	AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2();

	if(pCtrlSet->GetState() == E_EFFCTRL_STATE_END )
		return FALSE;

	if( !pcmEffect ) 
		return FALSE;

	if( !pcmEffect->FindEffectSet( pCtrlSet ) )	
		return FALSE;

	if( pCtrlSet->m_lpEffSet->m_enumLoadStatus == AGCDEFFSETRESOURCELOADSTATUS_LOADING)
		return TRUE;

	if( T_ISMINUS4( pCtrlSet->Render() ) )
	{
		pCtrlSet->SetState(E_EFFCTRL_STATE_END);
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// bGet1stSound
//-----------------------------------------------------------------------------
AgcdEffCtrl_Base* AgcdEffCtrl_Set::Get1stSound(void)
{

	mapCtrlBaseEntryIter	Iter		=	m_MapEffCtrlBase.begin();
	for( ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
	{
		stEffCtrlBaseEntry*		pEntry		=	&Iter->second;
		if( pEntry )
		{
			if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_SOUND )
				return pEntry->m_pEffCtrlBase;
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Set::AddPreRemoveFrm( RwFrame* pFrm )
{
	if( !pFrm )
	{
		Eff2Ut_ERR("AgcdEffCtrl_Set::bAddPreRemoveFrm failed!");
		return tReturnErr();
	}

	RwFramsListItr	it_f
		= std::find( m_listPreRemoveFrm.begin(), m_listPreRemoveFrm.end(), pFrm );
	if( it_f != m_listPreRemoveFrm.end() )
	{
		Eff2Ut_ERR("AgcdEffCtrl_Set::bAddPreRemoveFrm failed!");
		return tReturnErr();
	}

	m_listPreRemoveFrm.push_back( pFrm );
	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Set::DelPreRemoveFrm( RwFrame* pFrm )
{
	if( !pFrm )
	{
		Eff2Ut_ERR("AgcdEffCtrl_Set::bDelPreRemoveFrm failed!");
		return tReturnErr();
	}

	RwFramsListItr	it_f
		= find( m_listPreRemoveFrm.begin(), m_listPreRemoveFrm.end(), pFrm );
	if( it_f == m_listPreRemoveFrm.end() )
	{
		return 0;
	}

	m_listPreRemoveFrm.erase( it_f );
	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Set::PreRemoveFrm( RwFramsList& lstPreRmFrm )
{
	if( lstPreRmFrm.empty() )
	{
		return 0;
	}

	RwFramsListItr	it_f = m_listPreRemoveFrm.end();
	RwFramsListItr	it_curr
		= lstPreRmFrm.begin();
	RwFrame*	pFrm	= NULL;

	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
	for( ; it_curr != lstPreRmFrm.end(); ++it_curr )
	{
		pFrm	= (*it_curr);
		it_f = find( m_listPreRemoveFrm.begin(), m_listPreRemoveFrm.end(), pFrm );
		if( it_f != m_listPreRemoveFrm.end() )
		{
			RwFrameRemoveChild( pFrm );
			m_listPreRemoveFrm.erase( it_f );
		}
	}
	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();

	lstPreRmFrm.clear();
	return 0;
}

//tone
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void AgcdEffCtrl_Set::ToneDown()
{

	mapCtrlBaseEntryIter	Iter		=	m_MapEffCtrlBase.begin();
	for( ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
	{
		stEffCtrlBaseEntry*		pEntry		=	&Iter->second;
		if( pEntry )
		{
			if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT )
				static_cast< AgcdEffCtrl_Obj* >(pEntry->m_pEffCtrlBase)->ToneDown();
		}
	}

}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void AgcdEffCtrl_Set::ToneRestore()
{

	mapCtrlBaseEntryIter	Iter		=	m_MapEffCtrlBase.begin();
	for( ; Iter != m_MapEffCtrlBase.end() ; ++Iter )
	{
		stEffCtrlBaseEntry*		pEntry		=	&Iter->second;
		if( pEntry )
		{
			if( pEntry->m_pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT )
				static_cast< AgcdEffCtrl_Obj* >(pEntry->m_pEffCtrlBase)->ToneRestore();
		}
	}

}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_Board
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Board::AgcdEffCtrl_Board( AgcdEffCtrl_Set* lpEffCtrl_Set
									, AgcdEffBase* lpEffBase
									, RwUInt32 dwIndex
									, RwFrame* pFrmParent)
: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
, m_pFrm( NULL )
{
	EFFMEMORYLOG_CON;

	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
	m_pFrm	= RwFrameCreate();
	if( !m_pFrm )
	{
		Eff2Ut_ERR( "RwFrameCreate failed" );
		SetState(E_EFFCTRL_STATE_END);
	}
	else
	{
		RwMatrixSetIdentity ( &m_pFrm->modelling );
		if( pFrmParent )
		{
			RwFrameAddChild( pFrmParent, m_pFrm );		// 내부에서 RwFrameUpdateObjects( m_pFrm ); 해준다..
			//lpEffCtrl_Set->bAddFrmLinkAdd( pFrmParent, m_pFrm, false );
		}
		else
			RwFrameUpdateObjects( m_pFrm );
	}
	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
}

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Board::~AgcdEffCtrl_Board()
{
	EFFMEMORYLOG_DES;

	if( DEF_FLAG_CHK( GetPtrEffBase()->bGetFlag(), FLAG_EFFBASE_BASEDPND_TOOBJ ) )
	{
		m_pEffCtrl_Set->DelPreRemoveFrm( GetPtrFrm() );
	}

	Eff2Ut_SAFE_DESTROY_FRAME( m_pFrm );
}

//-----------------------------------------------------------------------------
// bUpdate
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Board::Update(RwUInt32 dwAccumulateTime)
{
	PROFILE("AgcdEffCtrl_Board::bUpdate");

	RwInt32	ir	= AgcdEffCtrl_Base::Update(dwAccumulateTime);
	ASSERT( !T_ISMINUS4( ir ) );
	if( ir )
	{
		if( T_ISMINUS4( ir ) )
		{
			Eff2Ut_ERR( "AgcdEffCtrl_Base::bUpdate(dwAccumulateTime) failed @ AgcdEffCtrl_Board::bUpdate" );
			return tReturnErr();
		}

		return ir;
	}

	ASSERT( !DEF_FLAG_CHK( m_dwUpdateFlag, E_UPDATEFLAG_RTANIM ) );

	return 0;
}

//-----------------------------------------------------------------------------
// bRender
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Board::Render(void)
{
	PROFILE("AgcdEffCtrl_Board::bRender");

	LPD3DVTX_PCT_RECT	pVtxRect = NULL;
	// lock	
	if( SUCCEEDED(g_agcuEffVBMng.LockVB_Board((VOID**)&pVtxRect)) )
	{
		DWORD	colr	= ( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_TONEDOWN) )
						? DEF_ARGB32(m_rgba.alpha, m_rgba.red >> 1, m_rgba.green >> 1, m_rgba.blue >> 1)
						: DEF_ARGB32(m_rgba.alpha, m_rgba.red, m_rgba.green, m_rgba.blue)
						;
		new(pVtxRect)	D3dVtx_pct_Rect( colr, m_stUVRect );

		g_agcuEffVBMng.UnlockVB_Board();
	}

	if( !m_lpEffBase->bIsRenderBase() )
	{
		Eff2Ut_ERR( "m_lpEffBase isn't AgcdEffRenderBase @ AgcdEffCtrl_Board::bRender" );
		return tReturnErr();
	}

	AgcdEffRenderBase* pEffRenderBase = static_cast<AgcdEffRenderBase*>(m_lpEffBase);

	// SetRenderState
	pEffRenderBase->bSet_Renderstate();

	// SetTexture
	RwD3D9SetTexture( pEffRenderBase->bGetPtrRwTex(), 0LU );

	// Render
	RwMatrix*	pFrmLTM = RwFrameGetLTM ( m_pFrm );

	g_agcuEffVBMng.RenderBoard( pFrmLTM );
	


#ifdef _CALCBOUNDINFO
	if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_CALCBOUND) )
	if( CALCBOUNDINFO::Begin() )
	{
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
		AgcuEffVtx_CalcBBox( CALCBOUNDINFO::GetBox(), &pVtxRect->m_vtx0, 4, RwFrameGetLTM ( m_pFrm ) );
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
	}
#endif

	// renderstate restore
	pEffRenderBase->bRestore_Renderstate();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_TerrainBoard
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_TerrainBoard::AgcdEffCtrl_TerrainBoard( AgcdEffCtrl_Set* lpEffCtrl_Set
													  , AgcdEffBase* lpEffBase
													  , RwUInt32 dwIndex
													  , RwFrame* pFrmParent )
: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
, m_pFrm( NULL )
, m_fRadius( 100.f )
{
	EFFMEMORYLOG_CON;

	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
	m_pFrm	= RwFrameCreate();
	ASSERT( m_pFrm );
	if( !m_pFrm )
	{
		Eff2Ut_ERR( "RwFrameCreate failed" );
		SetState(E_EFFCTRL_STATE_END);
	}
	else
	{
		RwMatrixSetIdentity ( &m_pFrm->modelling );
		if( pFrmParent )
		{
			RwFrameAddChild( pFrmParent, m_pFrm );		// 내부에서 RwFrameUpdateObjects( m_pFrm ); 해준다..
			//lpEffCtrl_Set->bAddFrmLinkAdd( pFrmParent, m_pFrm, false );
		}
		else
			RwFrameUpdateObjects( m_pFrm );
	}
	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
};

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_TerrainBoard::~AgcdEffCtrl_TerrainBoard()
{
	EFFMEMORYLOG_DES;

	if( DEF_FLAG_CHK( GetPtrEffBase()->bGetFlag(), FLAG_EFFBASE_BASEDPND_TOOBJ ) )
	{
		m_pEffCtrl_Set->DelPreRemoveFrm( GetPtrFrm() );
	}
	Eff2Ut_SAFE_DESTROY_FRAME( m_pFrm );
}
//-----------------------------------------------------------------------------
// bUpdate
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_TerrainBoard::Update(RwUInt32 dwAccumulateTime)
{
	PROFILE("AgcdEffCtrl_TerrainBoard::bUpdate");
	if( GetCPtrEffBase()->bFlagChk(FLAG_EFFBASE_MISSILE) && 
		m_eState == E_EFFCTRL_STATE_CONTINUE)
		return 0;

	tBeginUpdate();

	ASSERT( m_lpEffBase );
	if( !m_lpEffBase )
	{
		Eff2Ut_ERR( "m_lpEffBase == NULL @ AgcdEffCtrl_TerrainBoard::bUpdate" );
		return tReturnErr();
	}

	RwUInt32	dwCurrTime	= 0LU;
	RwInt32		ir			= tLifeCheck( &dwCurrTime, dwAccumulateTime );
	if( ir )
	{
		SetState( E_EFFCTRL_STATE_END );
		return ir;
	}
	if( GetState() == E_EFFCTRL_STATE_WAIT )
		return 0;

	EFFBASE::LPEffAniVecItr	it_curr
		= m_lpEffBase->bGetRefAnimList().begin();
	EFFBASE::LPEffAniVecItr	it_last
		= m_lpEffBase->bGetRefAnimList().end();
	LPEFFANIM	pEffAnim	= NULL;

	if( m_lpEffAnim_Scale )
	{
		RwV3d	vscale = {0.f, 0.f, 0.f};
		RwUInt32	ulKeyTime	= 0LU;
		::CalcKeyTime( &ulKeyTime, m_lpEffAnim_Scale->bGetLoopOpt(), dwCurrTime, m_lpEffAnim_Scale->bGetLife() );
		m_lpEffAnim_Scale->bGetTVal( static_cast<LPVOID>(&vscale), ulKeyTime );
		m_fRadius = vscale.x * 100.f;
	}
	for( ; it_curr != it_last; ++it_curr )
	{
		pEffAnim	= (*it_curr);
		ASSERT( pEffAnim );
		if( !pEffAnim )
		{
			Eff2Ut_ERR( "pEffAnim == NULL @ AgcdEffCtrl_TerrainBoard::bUpdate" );
			return tReturnErr();
		}

		if( pEffAnim->bGetAnimType() == AgcdEffAnim::E_EFFANIM_SCALE )
			continue;

		ir = pEffAnim->bUpdateVal( dwCurrTime, this );
		if( ir )
		{
			ASSERT( !T_ISMINUS4(ir) );
			if( T_ISMINUS4(ir) )
			{
				Eff2Ut_ERR( "pEffAnim->bUpdateVal failed @ AgcdEffCtrl_TerrainBoard::bUpdate" );
				Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - base index : %d", m_dwID) );
				return tReturnErr();
			}
			else
			{
				if( pEffAnim->bGetAnimType() == AgcdEffAnim::E_EFFANIM_SCALE )
				{
					SetState( E_EFFCTRL_STATE_END );
					m_pEffCtrl_Set->SetState( E_EFFCTRL_STATE_MISSILEEND );
				}
			}
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// bRender
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_TerrainBoard::Render(void)
{
	PROFILE("AgcdEffCtrl_TerrainBoard::bRender");

	ASSERT( "kday" && m_lpEffBase->bIsRenderBase() );

	AgcdEffRenderBase* pEffRenderBase
		= static_cast<AgcdEffRenderBase*>(m_lpEffBase);
	// SetRenderState
	pEffRenderBase->bSet_Renderstate();
	// SetTexture
	RwD3D9SetTexture( pEffRenderBase->bGetPtrRwTex(), 0LU );
	// Render
	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
	const RwMatrix*	pFrmLTM = RwFrameGetLTM ( m_pFrm );
	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();

	//colr, m_rgba
	//angle, m_fAccumulatedAngle
	//scale, m_fRadius
	//pos, ltm->pos
	g_agcuEffVBMng.RenderTerrainBoard( 
		pFrmLTM->pos
		, m_fRadius
		, m_fAccumulatedAngle
		, m_rgba);

#ifdef _CALCBOUNDINFO
	if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_CALCBOUND) )
	if( CALCBOUNDINFO::Begin() )
	{
		D3DVTX_PCT*	pvtx	= NULL;
		g_agcuEffVBMng.LockVB_TerrainBaord((LPVOID*)&pvtx);
		AgcuEffVtx_CalcBBox( CALCBOUNDINFO::GetBox()
			, pvtx
			, g_agcuEffVBMng.GetNumVtx_TerrainBoard()
			, NULL );
		g_agcuEffVBMng.UnlockVB_TerrainBoard();
	}
#endif

	// renderstate restore
	pEffRenderBase->bRestore_Renderstate();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_ParticleSysTem
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_ParticleSysTem::AgcdEffCtrl_ParticleSysTem( AgcdEffCtrl_Set* lpEffCtrl_Set
													  , AgcdEffBase* lpEffBase
													  , RwUInt32 dwIndex
													  , RwFrame* pFrmParent )
: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
, m_pFrm( NULL )
, m_pParent( pFrmParent )
, m_lpEffAnim_Colr( NULL )
, m_lpEffAnim_TuTv( NULL )
, m_lpEffAnim_Scale( NULL )
, m_lpEffAnim_Missile( NULL )
, m_lpEffAnim_Linear( NULL )
, m_lpEffAnim_Rev( NULL )
, m_lpEffAnim_Rot( NULL )
, m_lpEffAnim_RpSpline( NULL )
, m_lpEffAnim_ParticlePosScale( NULL )
, m_dwLastAccumulateTime( 0LU )
, m_dwLastShootTime( 0LU )
{	
	EFFMEMORYLOG_CON;

	m_pFrm	= RwFrameCreate();
	ASSERT( m_pFrm );
	if( !m_pFrm )
	{
		Eff2Ut_ERR( "RwFrameCreate failed" );
		SetState(E_EFFCTRL_STATE_END);
	}
	else
	{
		RwMatrixSetIdentity ( &m_pFrm->modelling );
		if( pFrmParent )
		{
			RwFrameAddChild( pFrmParent, m_pFrm ); // 내부에서 RwFrameUpdateObjects( m_pFrm );	해준다.
		}
		else
			RwFrameUpdateObjects( m_pFrm );
	}
}

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_ParticleSysTem::~AgcdEffCtrl_ParticleSysTem()
{
	EFFMEMORYLOG_DES;

	_ClearPaticle();
	if( DEF_FLAG_CHK( GetPtrEffBase()->bGetFlag(), FLAG_EFFBASE_BASEDPND_TOOBJ ) )
	{		
		m_pEffCtrl_Set->DelPreRemoveFrm( GetPtrFrm() );
	}

	Eff2Ut_SAFE_DESTROY_FRAME( m_pFrm );
}

//-----------------------------------------------------------------------------
// vClearPaticle
//-----------------------------------------------------------------------------
void AgcdEffCtrl_ParticleSysTem::_ClearPaticle(void)
{
	LPParticleListItr	it_curr = m_listLpParticle.begin();
	for( ; it_curr != m_listLpParticle.end(); ++it_curr )
	{
		DEF_SAFEDELETE( *it_curr );
	}

	m_listLpParticle.clear();
}

//-----------------------------------------------------------------------------
// vClearPaticle
// -1 : fail, 0 : success
//-----------------------------------------------------------------------------
RwInt32	AgcdEffCtrl_ParticleSysTem::_PushBack_Particle( LPPARTICLE pParticle, RwInt32 dwCapacity )
{
	ASSERT( pParticle );
	if( !pParticle )
	{
		Eff2Ut_ERR( "pParticle == NULL @ AgcdEffCtrl_ParticleSysTem::vPushBack_Particle" );
		return tReturnErr();
	}

	if( static_cast<RwInt32>(m_listLpParticle.size()) >= dwCapacity )
	{
		LPParticleListItr	it_f	= m_listLpParticle.begin();
		if( it_f != m_listLpParticle.end() )
		{
			DEF_SAFEDELETE(*it_f);
			m_listLpParticle.erase(it_f);
		}
		else
		{
			Eff2Ut_ERR( "m_listLpParticle.size() >= dwCapacity but begin == end" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - dwCapacity : %d, m_listLpParticle.size() : %d"
						   , dwCapacity
						   , m_listLpParticle.size() ) );
			return tReturnErr();
		}
	}

	m_listLpParticle.push_back( pParticle );

	return 0;
}

//-----------------------------------------------------------------------------
// vParticleUpdate
// -1 : err, 0 : continue, 1 : end
// dwAccumulateTime : AgcdEffCtrl_Set::m_stTimeLoop.m_dwCurrTime
//					, 이펙트 셋 생성후로 누적된 시간을 받는다.
//-----------------------------------------------------------------------------
RwInt32	AgcdEffCtrl_ParticleSysTem::_ParticleUpdate(LPPARTICLE pParticle
						, RwV3d* pv3dPos
						, RwReal* pfAngle
						, RwUInt32 dwAccumulateTime
						, const STENVRNPARAM& crefStEnvrnParam)
{

	ASSERT( pParticle	&&
			pv3dPos		&&
			pfAngle		);

	//이런경우는 없을텐데..
	if( !pParticle )
		return -1;

	RwUInt32	dwPastTime	= dwAccumulateTime - pParticle->m_dwStartTime;	//milli second

	//life check
	if( pParticle->m_dwLife 
	 && dwPastTime >= pParticle->m_dwLife )
		return 1;	//end

	RwReal		fPastTime	= static_cast<RwReal>(dwPastTime) * 0.001f;							//second

	RwV3d		v3dVel		= {0.f, 0.f, 0.f};
	RwV3d		v3dGrav		= {0.f, 0.f, 0.f};
	RwV3d		v3dWind		= {0.f, 0.f, 0.f};
	RwV3d		v3dGAddWind	= {0.f, 0.f, 0.f};

	//pos
	RwV3dScale( &v3dGrav, crefStEnvrnParam.bGetCPtrGrav(), pParticle->m_stCofEnvrn.m_fCofGrav);
	RwV3dScale( &v3dWind, crefStEnvrnParam.bGetCPtrWind(), pParticle->m_stCofEnvrn.m_fCofAirResistance );
	RwV3dAdd( &v3dGAddWind, &v3dGrav, &v3dWind );
	RwV3dScale( &v3dGAddWind, &v3dGAddWind, 0.5f * fPastTime * fPastTime );

	
	if( this->m_lpEffAnim_ParticlePosScale )
	{
		RwReal		scale	= 1.f;
		RwUInt32	keyTime	= 0LU;
		RwInt32		ir		= 
			::CalcKeyTime( &keyTime, m_lpEffAnim_ParticlePosScale->bGetLoopOpt(), dwPastTime, m_lpEffAnim_ParticlePosScale->bGetLife() );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "::CalcKeyTime failed @ AgcdEffCtrl_ParticleSysTem::vParticleUpdate" );
			return tReturnErr();
		}
		m_lpEffAnim_ParticlePosScale->bGetTVal( &scale, keyTime );

		RwV3d disp = {0.f, 0.f, 0.f};
		RwV3dScale( &disp, &pParticle->m_v3dInitVelocity, scale*m_pEffCtrl_Set->GetScale() );

		RwV3dAdd( pv3dPos, &pParticle->m_v3dInitPos, &disp );
	}
	else
	{
		RwV3dScale( &v3dVel, &pParticle->m_v3dInitVelocity, fPastTime*m_pEffCtrl_Set->GetScale() );
		RwV3dAdd( pv3dPos, &pParticle->m_v3dInitPos, &v3dVel );
	}

	RwV3dAdd( pv3dPos, pv3dPos, &v3dGAddWind );

	//rot
	*pfAngle	= pParticle->m_fOmega * fPastTime;

	//colr
	if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEPSYS_RANDCOLR ) )
	{
		if( pParticle->m_dwLife )
		pParticle->m_colr.alpha	
			= static_cast<RwUInt8>( 255.f - 255.f * fPastTime / (pParticle->m_dwLife * 0.001f) );
		else
		pParticle->m_colr.alpha	
			= static_cast<RwUInt8>( 255.f - 255.f * fPastTime );
	}
	else
	{
		if( m_lpEffAnim_Colr )
		{
			RwUInt32	keyTime	= 0LU;
			RwInt32		ir		= 
				::CalcKeyTime( &keyTime, m_lpEffAnim_Colr->bGetLoopOpt(), dwPastTime, m_lpEffAnim_Colr->bGetLife() );
			if( T_ISMINUS4(ir) )
			{
				Eff2Ut_ERR( "::CalcKeyTime failed @ AgcdEffCtrl_ParticleSysTem::vParticleUpdate" );
				return tReturnErr();
			}
			m_lpEffAnim_Colr->bGetTVal( &pParticle->m_colr, keyTime );
			ApplyRGBScale(&pParticle->m_colr);
		}
	}

	//uv
	if( m_lpEffAnim_TuTv )
	{
		RwUInt32	keyTime	= 0LU;
		RwInt32		ir		= 
			::CalcKeyTime( &keyTime, m_lpEffAnim_TuTv->bGetLoopOpt(), dwPastTime, m_lpEffAnim_TuTv->bGetLife() );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "::CalcKeyTime failed @ AgcdEffCtrl_ParticleSysTem::vParticleUpdate" );
			return tReturnErr();
		}
		m_lpEffAnim_TuTv->bGetTVal( &pParticle->m_stUV, keyTime );
	}

	//scale
	if( m_lpEffAnim_Scale )
	{
		RwUInt32	keyTime	= 0LU;
		RwInt32		ir		= 
			::CalcKeyTime( &keyTime, m_lpEffAnim_Scale->bGetLoopOpt(), dwPastTime, m_lpEffAnim_Scale->bGetLife() );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "::CalcKeyTime failed @ AgcdEffCtrl_ParticleSysTem::vParticleUpdate" );
			return tReturnErr();
		}
		m_lpEffAnim_Scale->bGetTVal( &pParticle->m_v3dScale, keyTime );
	}

	return 0;
}

//-----------------------------------------------------------------------------
// vClearPaticle
// -1 : err, 0 : continue, 1 : end
//-----------------------------------------------------------------------------
RwInt32	AgcdEffCtrl_ParticleSysTem::Update(RwUInt32 dwAccumulateTime)
{
	PROFILE("AgcdEffCtrl_ParticleSysTem::bUpdate");

#ifdef _DEBUG
	NS_EFF2PARTICLEPROFILE::stProfiler( "bUpdate" );
#endif //_DEBUG

	if( m_dwLastAccumulateTime == dwAccumulateTime )
		return 0;
	m_dwLastAccumulateTime	= dwAccumulateTime;
	
	ASSERT( m_lpEffBase );
	if( !m_lpEffBase )
	{
		Eff2Ut_ERR( "m_lpEffBase == NULL @ AgcdEffCtrl_ParticleSysTem::bUpdate" );
		return tReturnErr();
	}

	AgcdEffParticleSystem*	pEffBase_PSyst
		= static_cast<AgcdEffParticleSystem*>(m_lpEffBase);
	RwUInt32	dwCurrTime	= 0LU;
	RwInt32		ir			= tLifeCheck( &dwCurrTime, dwAccumulateTime );
	if( ir )
	{
		if( T_ISMINUS4( ir ) )
		{
			Eff2Ut_ERR( "tLifeCheck is failed @ AgcdEffCtrl_ParticleSysTem::bUpdate" );
			return tReturnErr();
		}

		if( m_listLpParticle.empty() )
			return 1;
		else
			return 0;//particle이 남아 있으므로 랜더함수 호출.
	}	
	if( GetState() == E_EFFCTRL_STATE_WAIT )
		return 0;

	//tPreUpdateFrm();

	m_lpEffAnim_Colr	= NULL;
	m_lpEffAnim_TuTv	= NULL;
	m_lpEffAnim_Scale	= NULL;
	
	m_lpEffAnim_Missile	= NULL;
	m_lpEffAnim_Linear	= NULL;
	m_lpEffAnim_Rev		= NULL;
	m_lpEffAnim_Rot		= NULL;
	m_lpEffAnim_RpSpline= NULL;

	
	EFFBASE::LPEffAniVecItr	it_curr = m_lpEffBase->bGetRefAnimList().begin();
	EFFBASE::LPEffAniVecItr	it_last = m_lpEffBase->bGetRefAnimList().end();
	LPEFFANIM	pEffAnim	= NULL;

	for( ; it_curr != it_last; ++it_curr )
	{
		pEffAnim	= (*it_curr);
		ASSERT( pEffAnim );

		switch( pEffAnim->bGetAnimType() )
		{
		case AgcdEffAnim::E_EFFANIM_COLOR:
			m_lpEffAnim_Colr
				= static_cast<AgcdEffAnim_Colr*>(pEffAnim);
			ASSERT(m_lpEffAnim_Colr);
			break;

		case AgcdEffAnim::E_EFFANIM_TUTV:
			m_lpEffAnim_TuTv
				= static_cast<AgcdEffAnim_TuTv*>(pEffAnim);
			ASSERT(m_lpEffAnim_TuTv);
			break;

		case AgcdEffAnim::E_EFFANIM_MISSILE:
			m_lpEffAnim_Missile
				= static_cast<AgcdEffAnim_Missile*>(pEffAnim);
			ASSERT(m_lpEffAnim_Missile);
			break;

		case AgcdEffAnim::E_EFFANIM_LINEAR:
			m_lpEffAnim_Linear
				= static_cast<AgcdEffAnim_Linear*>(pEffAnim);
			ASSERT(m_lpEffAnim_Linear);
			break;

		case AgcdEffAnim::E_EFFANIM_REVOLUTION:
			m_lpEffAnim_Rev
				= static_cast<AgcdEffAnim_Rev*>(pEffAnim);
			ASSERT(m_lpEffAnim_Rev);
			break;

		case AgcdEffAnim::E_EFFANIM_ROTATION:
			m_lpEffAnim_Rot
				= static_cast<AgcdEffAnim_Rot*>(pEffAnim);
			ASSERT(m_lpEffAnim_Rot);
			break;

		case AgcdEffAnim::E_EFFANIM_RPSPLINE:
			m_lpEffAnim_RpSpline
				= static_cast<AgcdEffAnim_RpSpline*>(pEffAnim);
			ASSERT(m_lpEffAnim_RpSpline);
			break;

		case AgcdEffAnim::E_EFFANIM_RTANIM:
			Eff2Ut_ERR( "E_EFFANIM_RTANIM == pEffAnim->bGetAnimType() @ AgcdEffCtrl_ParticleSysTem::bUpdate" );
			break;

		case AgcdEffAnim::E_EFFANIM_SCALE:
			m_lpEffAnim_Scale
				= static_cast<AgcdEffAnim_Scale*>(pEffAnim);
			ASSERT(m_lpEffAnim_Scale);
			break;

		case AgcdEffAnim::E_EFFANIM_PARTICLEPOSSCALE:
			{
				ASSERT( "kday" && !m_lpEffAnim_ParticlePosScale);
				m_lpEffAnim_ParticlePosScale = static_cast<AgcdEffAnim_ParticlePosScale*>(pEffAnim);
				continue;
			}break;

		}
	}//for

	if( T_ISMINUS4( pEffBase_PSyst->bShootPaticle( dwAccumulateTime, this ) ) )
	{
		Eff2Ut_ERR( "pEffBase_PSyst->bShootPaticle failed @ AgcdEffCtrl_ParticleSysTem::bUpdate" );
		return tReturnErr();
	}

	m_v3dBeforePos	= RwFrameGetMatrix(m_pFrm)->pos;

	return 0;
}

void AgcdEffCtrl_ParticleSysTem::PostUpdateFrm(RwUInt32 dwCurrTime)
{
	ASSERT( GetPtrFrm() );

	BOOL		bFrameDirty = FALSE;
	RwFrame*	pFrame = GetPtrFrm();
	RwMatrix*	pModelling = RwFrameGetMatrix( pFrame );

	if( m_lpEffBase->bIsRenderBase() )
	{
		const RwMatrix *pMatTemp = static_cast<AgcdEffRenderBase*>( m_lpEffBase )->bGetCPtrMat_Rot();
		RwMatrixTransform( pModelling, pMatTemp, rwCOMBINEPRECONCAT );
		bFrameDirty = TRUE;
	}

	if( DEF_FLAG_CHK( m_dwUpdateFlag, E_UPDATEFLAG_BILLBOARD ) ||
		DEF_FLAG_CHK( m_dwUpdateFlag, FLAG_EFFBASE_BILLBOARDY ) )
	{			
		RwV3d		v3dPos		= pModelling->pos;
		RwFrame*	pFrmParent	= RwFrameGetParent( GetPtrFrm() );

		if( pFrmParent )
		{
			RwMatrix	matInvParent;
			RwMatrixInvert( &matInvParent, RwFrameGetLTM( pFrmParent ) );
			RwMatrixTransform( pModelling,  &matInvParent, rwCOMBINEPOSTCONCAT );
			bFrameDirty = TRUE;
		}
		pModelling->pos	= v3dPos;
	}

	if(bFrameDirty) RwFrameUpdateObjects(pFrame);

}


//-----------------------------------------------------------------------------
// bRender
//-----------------------------------------------------------------------------
#ifdef _DEBUG
#include "AcuIMDraw.h"
#endif // _DEBUG

RwInt32	AgcdEffCtrl_ParticleSysTem::_RenderVS(void)
{
	PROFILE("AgcdEffCtrl_ParticleSysTem::vRenderVS");

	
#ifdef USE_MFC
	NS_EFF2PARTICLEPROFILE::stProfiler tmp( "bRender" );
#endif //USE_MFC

	LPD3DVTX_PCT_RECT	pVtxRect	= NULL;
	D3DXVECTOR4*		pVtxExtra	= NULL;
	// lock	
	RwMatrix*	pmat	= NULL;
	if( SUCCEEDED(g_agcuEffVBMng.LockVB_Particle((VOID**)&pVtxRect, static_cast<DWORD>(m_listLpParticle.size())*4LU)) )
	{		
		if( SUCCEEDED( g_agcuEffVBMng.LockVB_ParticleExtra( &pVtxExtra, static_cast<DWORD>(m_listLpParticle.size())*4LU ) ) )
		{
			LPPARTICLE	pParticle	= NULL;
			RwV3d		v3dPos		= {0.f,0.f,0.f};
			RwReal		fAngle		= 0.f;
			INT			nChk		= 0;

			RwMatrix*	pLTM = RwFrameGetLTM( m_pFrm );
			D3DXMATRIX	mat(1.f,0.f,0.f,0.f,
							0.f,1.f,0.f,0.f,
							0.f,0.f,1.f,0.f,
							0.f,0.f,0.f,1.f);
			
			{
	#ifdef USE_MFC
			NS_EFF2PARTICLEPROFILE::stProfiler tmp( "before loop" );
	#endif //USE_MFC
			if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEPSYS_PARTICLEBILLBOARDY ) )
			{
				mat	= *BMATRIX::bGetInst().bGetPtrD3DMatBY();
			}
			else if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEPSYS_PARTICLEBILLBOARD ) )
			{
				mat	= *BMATRIX::bGetInst().bGetPtrD3DMatB();
			}
			else if( GetCPtrEffBase()->bFlagChk( FLAG_EFFBASEPSYS_CHILDDEPENDANCY ) )
			{
				*(D3DXVECTOR3*)(&mat._11)	= *(D3DXVECTOR3*)(&pLTM->right);
				*(D3DXVECTOR3*)(&mat._21)	= *(D3DXVECTOR3*)(&pLTM->up);
				*(D3DXVECTOR3*)(&mat._31)	= *(D3DXVECTOR3*)(&pLTM->at);
			}

			if( GetCPtrEffBase()->bFlagChk( FLAG_EFFBASEPSYS_CHILDDEPENDANCY ) )
			{
				if( GetPtrEffCtrl_Set()->FlagChk(FLAG_EFFCTRLSET_ATOMICEMITER) )
				{
					pmat	= RwFrameGetLTM( RpAtomicGetFrame( GetPtrEffCtrl_Set()->GetPtrAtomicEmiter() ) );
				}
				else if( GetPtrEffCtrl_Set()->FlagChk(FLAG_EFFCTRLSET_CLUMPEMITER) )
				{
					pmat	= RwFrameGetLTM( RpClumpGetFrame( GetPtrEffCtrl_Set()->GetPtrClumpEmiter() ) );
				}
				else
					pmat	= pLTM;

				if( pmat )
				{
					RwMatrix	invmat;
					RwMatrixInvert( &invmat, pmat );
					RwMatrixTransform( (RwMatrix*)&mat, &invmat, rwCOMBINEPOSTCONCAT );
				}
			}
			}


			LPParticleListItr	it_curr = m_listLpParticle.begin();
						
			RwReal	halfWidhtCof	= DEFAULT_BOARDSIZE * m_pEffCtrl_Set->GetScale() * 0.5f;
			RwReal	halfHeightCof	= DEFAULT_BOARDSIZE * m_pEffCtrl_Set->GetScale() * 0.5f;

			RwD3D9SetVertexShaderConstant( 4, (float*)&mat._11, 1 );
			RwD3D9SetVertexShaderConstant( 5, (float*)&mat._21, 1 );
			const RwV4d	const1 = { halfWidhtCof, halfHeightCof, 1.f, 0.f };
			RwD3D9SetVertexShaderConstant( 6, (float*)&const1, 1 );

			while( it_curr != m_listLpParticle.end() )
			{
				pParticle	= (*it_curr);
				{
	#ifdef USE_MFC
				NS_EFF2PARTICLEPROFILE::stProfiler tmp( "vParticleUpdate" );
	#endif //USE_MFC
				nChk = _ParticleUpdate( pParticle, &v3dPos, &fAngle, m_dwLastAccumulateTime, g_EnvrnParam );
				}
				
				{
	#ifdef USE_MFC
				NS_EFF2PARTICLEPROFILE::stProfiler tmp( "if( nChk )" );
	#endif //USE_MFC
				if( nChk )
				{
					if( T_ISMINUS4( nChk ) )
					{
						g_agcuEffVBMng.UnlockVB_Particle();
						return tReturnErr();
					}

					{
	#ifdef USE_MFC
					NS_EFF2PARTICLEPROFILE::stProfiler tmp( "DEF_SAFEDELETE" );
	#endif //USE_MFC
					DEF_SAFEDELETE( pParticle );
					}
					{
	#ifdef USE_MFC
					NS_EFF2PARTICLEPROFILE::stProfiler tmp( "m_listLpParticle.erase" );
	#endif //USE_MFC
					it_curr = m_listLpParticle.erase( it_curr );
					continue;
					}
				}
				}

				
				{
	#ifdef USE_MFC
				NS_EFF2PARTICLEPROFILE::stProfiler tmp( "vertex transform" );
	#endif //USE_MFC

				DWORD	colr = AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_TONEDOWN) 
					? DEF_ARGB32(pParticle->m_colr.alpha, pParticle->m_colr.red >> 1 , pParticle->m_colr.green >> 1 , pParticle->m_colr.blue >> 1)
					: DEF_ARGB32(pParticle->m_colr.alpha, pParticle->m_colr.red, pParticle->m_colr.green, pParticle->m_colr.blue);
				RwReal	fsin = NSAcuMath::AcuSinTbl::Sin( fAngle );
				RwReal	fcos = NSAcuMath::AcuSinTbl::Cos( fAngle );
				
	#ifdef _CALCBOUNDINFO
				{
					if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_CALCBOUND) )
					if( CALCBOUNDINFO::Begin() )
					{

						RwReal	halfWidth	= halfWidhtCof * pParticle->m_v3dScale.x;
						RwReal	halfHeight	= halfHeightCof * pParticle->m_v3dScale.y;

						D3DXVECTOR3	vx	= halfWidth  * (fcos * (*(D3DXVECTOR3*)(&mat._11)) - fsin * (*(D3DXVECTOR3*)(&mat._21)));
						D3DXVECTOR3	vy	= halfHeight * (fsin * (*(D3DXVECTOR3*)(&mat._11)) + fcos * (*(D3DXVECTOR3*)(&mat._21)));

						RwV3d	vtmp;
						if( pmat )
						{
							ASSERT( ( GetCPtrEffBase()->bFlagChk( FLAG_EFFBASEPSYS_CHILDDEPENDANCY ) ) );
							RwV3dTransformPoint( &vtmp, &v3dPos, pmat );
						}

						pVtxRect->m_vtx0.m_v3Pos	= *((D3DXVECTOR3*)&vtmp) - vx + vy;
						pVtxRect->m_vtx1.m_v3Pos	= *((D3DXVECTOR3*)&vtmp) - vx - vy;
						pVtxRect->m_vtx2.m_v3Pos	= *((D3DXVECTOR3*)&vtmp) + vx + vy;
						pVtxRect->m_vtx3.m_v3Pos	= *((D3DXVECTOR3*)&vtmp) + vx - vy;

						AgcuEffVtx_CalcBBox( CALCBOUNDINFO::GetBox(), &pVtxRect->m_vtx0, 4, NULL );
					}
				}

	#endif

				pVtxRect->m_vtx0.m_dwColr = 
				pVtxRect->m_vtx1.m_dwColr = 
				pVtxRect->m_vtx2.m_dwColr = 
				pVtxRect->m_vtx3.m_dwColr = colr;

				pVtxRect->m_vtx0.m_v2Tutv.x = pParticle->m_stUV.m_fLeft;	pVtxRect->m_vtx0.m_v2Tutv.y = pParticle->m_stUV.m_fTop;
				pVtxRect->m_vtx1.m_v2Tutv.x = pParticle->m_stUV.m_fLeft;	pVtxRect->m_vtx1.m_v2Tutv.y = pParticle->m_stUV.m_fBottom;
				pVtxRect->m_vtx2.m_v2Tutv.x = pParticle->m_stUV.m_fRight;	pVtxRect->m_vtx2.m_v2Tutv.y = pParticle->m_stUV.m_fTop;
				pVtxRect->m_vtx3.m_v2Tutv.x = pParticle->m_stUV.m_fRight;	pVtxRect->m_vtx3.m_v2Tutv.y = pParticle->m_stUV.m_fBottom;

				pVtxRect->m_vtx0.m_v3Pos	= 
				pVtxRect->m_vtx1.m_v3Pos	= 
				pVtxRect->m_vtx2.m_v3Pos	= 
				pVtxRect->m_vtx3.m_v3Pos	= *((D3DXVECTOR3*)&v3dPos);

				pVtxExtra->x =  fsin;
				pVtxExtra->y =  fcos;
				pVtxExtra->z = -pParticle->m_v3dScale.x;
				pVtxExtra->w =  pParticle->m_v3dScale.y;
				++pVtxExtra;

				pVtxExtra->x =  fsin;
				pVtxExtra->y =  fcos;
				pVtxExtra->z = -pParticle->m_v3dScale.x;
				pVtxExtra->w = -pParticle->m_v3dScale.y;
				++pVtxExtra;

				pVtxExtra->x =  fsin;
				pVtxExtra->y =  fcos;
				pVtxExtra->z =  pParticle->m_v3dScale.x;
				pVtxExtra->w =  pParticle->m_v3dScale.y;
				++pVtxExtra;

				pVtxExtra->x =  fsin;
				pVtxExtra->y =  fcos;
				pVtxExtra->z =  pParticle->m_v3dScale.x;
				pVtxExtra->w = -pParticle->m_v3dScale.y;
				++pVtxExtra;

				//<----
				}

				++pVtxRect;
				++it_curr;
			}//for
			g_agcuEffVBMng.UnlockVB_ParticleExtra();
		}//SUCCEEDED( g_agcuEffVBMng.bLockVB_ParticleExtra( (VOID**)&pVtxExtra ) )

		// unlock
		g_agcuEffVBMng.UnlockVB_Particle();
	}//SUCCEEDED(g_agcuEffVBMng.bLockVB_Particle((VOID**)&pVtxRect))


	{
#ifdef USE_MFC
	NS_EFF2PARTICLEPROFILE::stProfiler tmp( "bRenderParticle" );
#endif //USE_MFC

	AgcdEffRenderBase* pEffRenderBase
		= static_cast<AgcdEffRenderBase*>(m_lpEffBase);
	// SetRenderState
	pEffRenderBase->bSet_Renderstate();

	// SetTexture
	RwD3D9SetTexture( pEffRenderBase->bGetPtrRwTex(), 0LU );
	// Render
	RwMatrix mat;
	RwMatrixSetIdentity( &mat );

	g_agcuEffVBMng.RenderParticle( m_listLpParticle.size(), pmat ? pmat : &mat, TRUE );

	// renderstate restore
	pEffRenderBase->bRestore_Renderstate();
	}
	return 0;
}

RwInt32	AgcdEffCtrl_ParticleSysTem::_RenderFP(void)
{
	PROFILE("AgcdEffCtrl_ParticleSysTem::vRenderFP");

	
#ifdef USE_MFC
	NS_EFF2PARTICLEPROFILE::stProfiler tmp( "bRender" );
#endif //USE_MFC

	LPD3DVTX_PCT_RECT	pVtxRect	= NULL;
	RwMatrix*			pmat		= NULL;
	// lock	
	if( SUCCEEDED(g_agcuEffVBMng.LockVB_Particle((VOID**)&pVtxRect, static_cast<DWORD>(m_listLpParticle.size())*4LU)) )
	{		
			LPPARTICLE	pParticle	= NULL;
			RwV3d		v3dPos		= {0.f,0.f,0.f};
			RwReal		fAngle		= 0.f;
			INT			nChk		= 0;

			RwMatrix*	pLTM = RwFrameGetLTM( m_pFrm );
			
			D3DXMATRIX	mat(1.f,0.f,0.f,0.f,
							0.f,1.f,0.f,0.f,
							0.f,0.f,1.f,0.f,
							0.f,0.f,0.f,1.f);

			{
	#ifdef USE_MFC
			NS_EFF2PARTICLEPROFILE::stProfiler tmp( "before loop" );
	#endif //USE_MFC
			if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEPSYS_PARTICLEBILLBOARDY ) )
			{
				mat	= *BMATRIX::bGetInst().bGetPtrD3DMatBY();
			}
			else if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEPSYS_PARTICLEBILLBOARD ) )
			{
				mat	= *BMATRIX::bGetInst().bGetPtrD3DMatB();
			}
			else if( GetCPtrEffBase()->bFlagChk( FLAG_EFFBASEPSYS_CHILDDEPENDANCY ) )
			{
				*(D3DXVECTOR3*)(&mat._11)	= *(D3DXVECTOR3*)(&pLTM->right);
				*(D3DXVECTOR3*)(&mat._21)	= *(D3DXVECTOR3*)(&pLTM->up);
				*(D3DXVECTOR3*)(&mat._31)	= *(D3DXVECTOR3*)(&pLTM->at);
			}

			if( GetCPtrEffBase()->bFlagChk( FLAG_EFFBASEPSYS_CHILDDEPENDANCY ) )
			{
				if( GetPtrEffCtrl_Set()->FlagChk(FLAG_EFFCTRLSET_ATOMICEMITER) )
				{
					// Debug logging
					RwFrame *pFrame = RpAtomicGetFrame( GetPtrEffCtrl_Set()->GetPtrAtomicEmiter() );
					if(pFrame == NULL)
					{
						RpAtomic *pAtomic = GetPtrEffCtrl_Set()->GetPtrAtomicEmiter();
						MD_SetErrorMessage("(RpAtomicGetFrame( bGetPtrEffCtrl_Set()->bGetPtrAtomicEmiter() ) == NULL) : %d, %d, %s, repEntry=0x%08x, next=0x%08x\n", 
							pAtomic->id, pAtomic->geometry?pAtomic->geometry->numTriangles:0, 
							pAtomic->clump?pAtomic->clump->szName:"No Clump",
							pAtomic->repEntry, pAtomic->next);

						// Prevent the crash.
						g_agcuEffVBMng.UnlockVB_Particle();
						return tReturnErr();
					}
					pmat	= RwFrameGetLTM( pFrame );
				}
				else if( GetPtrEffCtrl_Set()->FlagChk(FLAG_EFFCTRLSET_CLUMPEMITER) )
				{
					pmat	= RwFrameGetLTM( RpClumpGetFrame( GetPtrEffCtrl_Set()->GetPtrClumpEmiter() ) );
				}
				else
					pmat	= pLTM;
			}
			}

			RwV3d	vScale	= 	{	RwV3dLength( &pLTM->right ), RwV3dLength ( &pLTM->up ), RwV3dLength ( &pLTM->at )	};
			float fScale = ( vScale.x + vScale.y + vScale.z ) / 3.f;
			RwReal	halfWidhtCof	= DEFAULT_BOARDSIZE * m_pEffCtrl_Set->GetScale() /*/ fScale*/ * 0.5f;
			RwReal	halfHeightCof	= DEFAULT_BOARDSIZE * m_pEffCtrl_Set->GetScale() /*/ fScale*/ * 0.5f;
			
			LPParticleListItr	it_curr = m_listLpParticle.begin();
			while( it_curr != m_listLpParticle.end() )
			{
				pParticle	= (*it_curr);
				{
	#ifdef USE_MFC
				NS_EFF2PARTICLEPROFILE::stProfiler tmp( "vParticleUpdate" );
	#endif //USE_MFC
				nChk = _ParticleUpdate( pParticle, &v3dPos, &fAngle, m_dwLastAccumulateTime, g_EnvrnParam );
				}				
				
				{
	#ifdef USE_MFC
				NS_EFF2PARTICLEPROFILE::stProfiler tmp( "if( nChk )" );
	#endif //USE_MFC
				if( nChk )
				{
					if( T_ISMINUS4( nChk ) )
					{
						g_agcuEffVBMng.UnlockVB_Particle();
						return tReturnErr();
					}

					{
	#ifdef USE_MFC
					NS_EFF2PARTICLEPROFILE::stProfiler tmp( "DEF_SAFEDELETE" );
	#endif //USE_MFC
					DEF_SAFEDELETE( pParticle );
					}
					{
	#ifdef USE_MFC
					NS_EFF2PARTICLEPROFILE::stProfiler tmp( "m_listLpParticle.erase" );
	#endif //USE_MFC
					it_curr = m_listLpParticle.erase( it_curr );
					continue;
					}
				}
				if(pmat)RwV3dTransformPoint(&v3dPos, &v3dPos, pmat);

				}
				
				{
	#ifdef USE_MFC
				NS_EFF2PARTICLEPROFILE::stProfiler tmp( "vertex transform" );
	#endif //USE_MFC

				DWORD		colr = AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_TONEDOWN) 
					? DEF_ARGB32(pParticle->m_colr.alpha, pParticle->m_colr.red >> 1 , pParticle->m_colr.green >> 1 , pParticle->m_colr.blue >> 1)
					: DEF_ARGB32(pParticle->m_colr.alpha, pParticle->m_colr.red, pParticle->m_colr.green, pParticle->m_colr.blue);

				pVtxRect->m_vtx0.m_dwColr = 
				pVtxRect->m_vtx1.m_dwColr = 
				pVtxRect->m_vtx2.m_dwColr = 
				pVtxRect->m_vtx3.m_dwColr = colr;

				pVtxRect->m_vtx0.m_v2Tutv.x = pParticle->m_stUV.m_fLeft;	pVtxRect->m_vtx0.m_v2Tutv.y = pParticle->m_stUV.m_fTop;
				pVtxRect->m_vtx1.m_v2Tutv.x = pParticle->m_stUV.m_fLeft;	pVtxRect->m_vtx1.m_v2Tutv.y = pParticle->m_stUV.m_fBottom;
				pVtxRect->m_vtx2.m_v2Tutv.x = pParticle->m_stUV.m_fRight;	pVtxRect->m_vtx2.m_v2Tutv.y = pParticle->m_stUV.m_fTop;
				pVtxRect->m_vtx3.m_v2Tutv.x = pParticle->m_stUV.m_fRight;	pVtxRect->m_vtx3.m_v2Tutv.y = pParticle->m_stUV.m_fBottom;

				
				RwReal	halfWidth	= halfWidhtCof * pParticle->m_v3dScale.x;
				RwReal	halfHeight	= halfHeightCof * pParticle->m_v3dScale.y;

				RwReal	fsin		= NSAcuMath::AcuSinTbl::Sin( fAngle );
				RwReal	fcos		= NSAcuMath::AcuSinTbl::Cos( fAngle );
				
				D3DXVECTOR3	vx	= halfWidth  * (fcos * (*(D3DXVECTOR3*)(&mat._11)) - fsin * (*(D3DXVECTOR3*)(&mat._21)));
				D3DXVECTOR3	vy	= halfHeight * (fsin * (*(D3DXVECTOR3*)(&mat._11)) + fcos * (*(D3DXVECTOR3*)(&mat._21)));

				pVtxRect->m_vtx0.m_v3Pos	= *((D3DXVECTOR3*)&v3dPos) - vx + vy;
				pVtxRect->m_vtx1.m_v3Pos	= *((D3DXVECTOR3*)&v3dPos) - vx - vy;
				pVtxRect->m_vtx2.m_v3Pos	= *((D3DXVECTOR3*)&v3dPos) + vx + vy;
				pVtxRect->m_vtx3.m_v3Pos	= *((D3DXVECTOR3*)&v3dPos) + vx - vy;
				
	#ifdef _CALCBOUNDINFO
				{
					if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_CALCBOUND) )
					if( CALCBOUNDINFO::Begin() )
					{
					
						RwV3d	vtmp = v3dPos;
						//if( pmat )
						//{
						//	ASSERT( ( bGetCPtrEffBase()->bFlagChk( FLAG_EFFBASEPSYS_CHILDDEPENDANCY ) ) );
						//	RwV3dTransformPoint( &vtmp, &v3dPos, pmat );
						//}

						D3DVTX_PCT	ptmp[4];

						ptmp[0].m_v3Pos	= *((D3DXVECTOR3*)&vtmp) - vx + vy;
						ptmp[1].m_v3Pos	= *((D3DXVECTOR3*)&vtmp) - vx - vy;
						ptmp[2].m_v3Pos	= *((D3DXVECTOR3*)&vtmp) + vx + vy;
						ptmp[3].m_v3Pos	= *((D3DXVECTOR3*)&vtmp) + vx - vy;

						AgcuEffVtx_CalcBBox( CALCBOUNDINFO::GetBox(), &pVtxRect->m_vtx0, 4, NULL );
					}
				}
	#endif //_CALCBOUNDINFO
				}
				
				++pVtxRect;
				++it_curr;
			}//for

		// unlock
		g_agcuEffVBMng.UnlockVB_Particle();
	}//SUCCEEDED(g_agcuEffVBMng.bLockVB_Particle((VOID**)&pVtxRect))

	{
#ifdef USE_MFC
	NS_EFF2PARTICLEPROFILE::stProfiler tmp( "bRenderParticle" );
#endif //USE_MFC

	AgcdEffRenderBase* pEffRenderBase
		= static_cast<AgcdEffRenderBase*>(m_lpEffBase);

	// SetRenderState
	pEffRenderBase->bSet_Renderstate();

	// SetTexture
	RwD3D9SetTexture( pEffRenderBase->bGetPtrRwTex(), 0LU );

	// Render
	RwMatrix mat;
	RwMatrixSetIdentity( &mat );
	g_agcuEffVBMng.RenderParticle( m_listLpParticle.size(), /*pmat ? pmat : */&mat, FALSE );

	// renderstate restore
	pEffRenderBase->bRestore_Renderstate();
	}
	return 0;
}

RwInt32	AgcdEffCtrl_ParticleSysTem::Render(void)
{
	PROFILE("AgcdEffCtrl_ParticleSysTem::bRender");

	return (this->_RenderFP());
}


//-----------------------------------------------------------------------------
// SetState
//-----------------------------------------------------------------------------
void AgcdEffCtrl_ParticleSysTem::SetState( E_EFFCTRL_STATE eState )
{
	if( eState == E_EFFCTRL_STATE_BEGIN )
	{
		m_dwLastShootTime		 =
		m_dwLastAccumulateTime = 
			m_pEffCtrl_Set->GetAccumulateTime();
	}
	
	AgcdEffCtrl::SetState( eState );
}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_ParticleSyst_SBH
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_ParticleSyst_SBH::AgcdEffCtrl_ParticleSyst_SBH( 
														AgcdEffCtrl_Set* lpEffCtrl_Set
													  , AgcdEffBase* lpEffBase
													  , RwUInt32 dwIndex
													  , RwFrame* pFrmParent )
: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
, m_pFrm( NULL )
, m_lpEffAnim_Colr( NULL )
, m_lpEffAnim_TuTv( NULL )
, m_lpEffAnim_Scale( NULL )
, m_lpEffAnim_ParticlePosScale( NULL )
, m_dwLastAccumulateTime( 0LU )
, m_dwLastShootTime( 0LU )
{
	EFFMEMORYLOG_CON;

	m_pFrm	= RwFrameCreate();
	ASSERT( m_pFrm );
	if( !m_pFrm )
	{
		Eff2Ut_ERR( "RwFrameCreate failed" );
		SetState(E_EFFCTRL_STATE_END);
	}
	else
	{
		RwMatrixSetIdentity ( &m_pFrm->modelling );
		if( pFrmParent )
		{
			RwFrameAddChild( pFrmParent, m_pFrm );
		}
		else
			RwFrameUpdateObjects(m_pFrm);

	}
}

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_ParticleSyst_SBH::~AgcdEffCtrl_ParticleSyst_SBH()
{
	EFFMEMORYLOG_DES;

	_ClearPaticle();
	if( DEF_FLAG_CHK( GetPtrEffBase()->bGetFlag(), FLAG_EFFBASE_BASEDPND_TOOBJ ) )
	{
		m_pEffCtrl_Set->DelPreRemoveFrm( GetPtrFrm() );
	}
	Eff2Ut_SAFE_DESTROY_FRAME( m_pFrm );
}

//-----------------------------------------------------------------------------
// vClearPaticle
//-----------------------------------------------------------------------------
void AgcdEffCtrl_ParticleSyst_SBH::_ClearPaticle()
{
	LPParticleListItr	it_curr = m_listLpParticle.begin();
	for( ; it_curr != m_listLpParticle.end(); ++it_curr )
	{
		DEF_SAFEDELETE( *it_curr );
	}

	m_listLpParticle.clear();
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_ParticleSyst_SBH::_PushBack_Particle(  LPPARTICLE pParticle
														 , RwInt32 nCapacity )
{
	ASSERT( pParticle );

	if( static_cast<RwInt32>(m_listLpParticle.size()) >= nCapacity )
	{
		LPParticleListItr	it_f	= m_listLpParticle.begin();
		if( it_f != m_listLpParticle.end() )
		{
			DEF_SAFEDELETE(*it_f);
			m_listLpParticle.erase(it_f);
		}
		else
		{
			Eff2Ut_ERR( "m_listLpParticle.size() >= dwCapacity but m_listLpParticle is empty()" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - dwCapacity : %d, m_listLpParticle.size() : %d"
						   , nCapacity
						   , m_listLpParticle.size() ) );
			return tReturnErr();
		}
	}

	m_listLpParticle.push_back( pParticle );
	return 0;
}

//-----------------------------------------------------------------------------
// vParticleUpdate
// -1 : err, 0 : continue, 1 : end
// dwAccumulateTime : AgcdEffCtrl_Set::m_stTimeLoop.m_dwCurrTime
//					, 이펙트 셋 생성후로 누적된 시간을 받는다.
//-----------------------------------------------------------------------------
RwInt32	AgcdEffCtrl_ParticleSyst_SBH::_ParticleUpdate(LPPARTICLE pParticle
						, RwV3d* pv3dPos
						, RwReal* pfAngle
						, RwUInt32 dwAccumulateTime
						, const STENVRNPARAM& crefStEnvrnParam)
{
	ASSERT( pParticle	&&
			pv3dPos		&&
			pfAngle		);
	RwUInt32	dwPastTime	= dwAccumulateTime - pParticle->m_dwStartTime;	//milli second

	//life check
	if( pParticle->m_dwLife )
	if( dwPastTime >= pParticle->m_dwLife )
	{
		return 1;	//end
	}

	RwReal fPastTime = dwPastTime * 0.001f;//second


	if( this->m_lpEffAnim_ParticlePosScale )
	{
		RwReal		scale	= 1.f;
		RwUInt32	keyTime	= 0LU;
		RwInt32		ir		= 
			::CalcKeyTime( &keyTime, m_lpEffAnim_ParticlePosScale->bGetLoopOpt(), dwPastTime, m_lpEffAnim_ParticlePosScale->bGetLife() );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "::CalcKeyTime failed @ AgcdEffCtrl_ParticleSysTem::vParticleUpdate" );
			return tReturnErr();
		}
		m_lpEffAnim_ParticlePosScale->bGetTVal( &scale, keyTime );

		RwV3d disp = {0.f, 0.f, 0.f};
		RwV3dScale( &disp, &pParticle->m_v3dInitVelocity, scale );

		RwV3dAdd( pv3dPos, &pParticle->m_v3dInitPos, &disp );
	}
	else
	{
		RwV3d v3dVel = {0.f, 0.f, 0.f};
		RwV3dScale( &v3dVel, &pParticle->m_v3dInitVelocity, fPastTime*m_pEffCtrl_Set->GetScale() );
		RwV3dAdd( pv3dPos, &pParticle->m_v3dInitPos, &v3dVel );
	}

	//rot
	//no rotate but assign saved angle;
	*pfAngle = pParticle->m_fOmega;

	//colr
	if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEPSYS_RANDCOLR ) )
	{
		if( pParticle->m_dwLife )
		pParticle->m_colr.alpha	
			= static_cast<RwUInt8>( 255.f - 255.f * fPastTime / (pParticle->m_dwLife * 0.001f) );
		else
		pParticle->m_colr.alpha	
			= static_cast<RwUInt8>( 255.f - 255.f * fPastTime );
	}
	else
	{
		if( m_lpEffAnim_Colr )
		{
			RwUInt32	keyTime	= 0LU;
			RwInt32		ir		= 
				::CalcKeyTime( &keyTime, m_lpEffAnim_Colr->bGetLoopOpt(), dwPastTime, m_lpEffAnim_Colr->bGetLife() );
			if( T_ISMINUS4(ir) )
			{
				Eff2Ut_ERR( "::CalcKeyTime failed @ AgcdEffCtrl_ParticleSysTem::vParticleUpdate" );
				return tReturnErr();
			}

			m_lpEffAnim_Colr->bGetTVal( &pParticle->m_colr, keyTime );
			ApplyRGBScale(&pParticle->m_colr);
		}
	}

	//uv
	if( m_lpEffAnim_TuTv )
	{
		RwUInt32	keyTime	= 0LU;
		RwInt32		ir		= 
			::CalcKeyTime( &keyTime, m_lpEffAnim_TuTv->bGetLoopOpt(), dwPastTime, m_lpEffAnim_TuTv->bGetLife() );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "::CalcKeyTime failed @ AgcdEffCtrl_ParticleSysTem::vParticleUpdate" );
			return tReturnErr();
		}
		m_lpEffAnim_TuTv->bGetTVal( &pParticle->m_stUV, keyTime );
	}

	//scale
	if( m_lpEffAnim_Scale )
	{
		RwUInt32	keyTime	= 0LU;
		RwInt32		ir		= 
			::CalcKeyTime( &keyTime, m_lpEffAnim_Scale->bGetLoopOpt(), dwPastTime, m_lpEffAnim_Scale->bGetLife() );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "::CalcKeyTime failed @ AgcdEffCtrl_ParticleSysTem::vParticleUpdate" );
			return tReturnErr();
		}
		m_lpEffAnim_Scale->bGetTVal( &pParticle->m_v3dScale, keyTime );
	}

	return 0;
}

//-----------------------------------------------------------------------------
// bUpdate
// -1 : err, 0 : continue, 1 : end
//-----------------------------------------------------------------------------
RwInt32	AgcdEffCtrl_ParticleSyst_SBH::Update(RwUInt32 dwAccumulateTime)
{
	PROFILE("AgcdEffCtrl_ParticleSyst_SBH::bUpdate");

	tBeginUpdate();

	if( !m_lpEffBase )
		return tReturnErr();

	AgcdEffParticleSys_SimpleBlackHole*	pEffBase_PSystSBH		= static_cast<AgcdEffParticleSys_SimpleBlackHole*>(m_lpEffBase);

	m_dwLastAccumulateTime	= dwAccumulateTime;

	RwUInt32	dwCurrTime	= 0LU;
	RwInt32		ir			= tLifeCheck( &dwCurrTime, dwAccumulateTime );
	if( ir )
	{
		if( T_ISMINUS4( ir ) )
		{
			return tReturnErr();
		}

		if( m_listLpParticle.empty() )
			return 1;
		
		else
			return 0;//particle이 남아 있으므로 랜더함수 호출.
	}
	if( GetState() == E_EFFCTRL_STATE_WAIT )
		return 0;

	tPreUpdateFrm();

	m_lpEffAnim_Colr	= NULL;
	m_lpEffAnim_TuTv	= NULL;
	m_lpEffAnim_Scale	= NULL;

	EFFBASE::LPEffAniVecItr	it_curr		= m_lpEffBase->bGetRefAnimList().begin();
	EFFBASE::LPEffAniVecItr	it_last		= m_lpEffBase->bGetRefAnimList().end();
	LPEFFANIM				pEffAnim	= NULL;

	for( ; it_curr != it_last; ++it_curr )
	{
		pEffAnim	= (*it_curr);
		if( !pEffAnim )
		{
			return tReturnErr();
		}

		switch( pEffAnim->bGetAnimType() )
		{
		case AgcdEffAnim::E_EFFANIM_COLOR:
			{
				m_lpEffAnim_Colr = static_cast<AgcdEffAnim_Colr*>(pEffAnim);
				continue;
			}break;

		case AgcdEffAnim::E_EFFANIM_TUTV:
			{
				m_lpEffAnim_TuTv = static_cast<AgcdEffAnim_TuTv*>(pEffAnim);
				continue;
			}break;

		case AgcdEffAnim::E_EFFANIM_SCALE:
			{
				m_lpEffAnim_Scale = static_cast<AgcdEffAnim_Scale*>(pEffAnim);
				continue;
			}break;

		case AgcdEffAnim::E_EFFANIM_PARTICLEPOSSCALE:
			{
				m_lpEffAnim_ParticlePosScale = static_cast<AgcdEffAnim_ParticlePosScale*>(pEffAnim);
				continue;
			}break;
		}

		ir = pEffAnim->bUpdateVal( dwCurrTime, this );
		if( T_ISMINUS4(ir) )
			return tReturnErr();

	}

	PostUpdateFrm(dwCurrTime);

	if( T_ISMINUS4( pEffBase_PSystSBH->bShootPaticle( dwAccumulateTime, this ) ) )
		return tReturnErr();

	return 0;
}

void AgcdEffCtrl_ParticleSyst_SBH::PostUpdateFrm(RwUInt32 dwCurrTime)
{
	BOOL	bFrameDirty = FALSE;
	RwFrame*	pFrame = GetPtrFrm();
	ASSERT(pFrame);
	RwMatrix*	pModelling = RwFrameGetMatrix(pFrame);

	if( m_lpEffBase->bIsRenderBase() )
	{
		RwFrame *pFrameTemp = GetPtrFrm();
		const RwMatrix *pMatTemp = static_cast<AgcdEffRenderBase*>( m_lpEffBase )->bGetCPtrMat_Rot();
		RwMatrixTransform( pModelling, pMatTemp, rwCOMBINEPRECONCAT );
		bFrameDirty = TRUE;
	}

	if( DEF_FLAG_CHK( m_dwUpdateFlag, E_UPDATEFLAG_BILLBOARD ) ||
		DEF_FLAG_CHK( m_dwUpdateFlag, FLAG_EFFBASE_BILLBOARDY ) )
	{
		ASSERT( "kday" && m_lpEffBase->bIsRenderBase() );			
		RwV3d		v3dPos		= pModelling->pos;
		RwFrame*	pFrmParent	= RwFrameGetParent( pFrame );
		if( pFrmParent )
		{
			RwMatrix	matInvParent;
			RwMatrixInvert( &matInvParent, RwFrameGetLTM( pFrmParent ) );
			RwMatrixTransform( pModelling , &matInvParent, rwCOMBINEPOSTCONCAT );
			bFrameDirty = TRUE;
		}
		pModelling->pos	= v3dPos;
	}

	if(bFrameDirty) RwFrameUpdateObjects( RwFrameGetRoot( pFrame ) );

}

//-----------------------------------------------------------------------------
// bUpdate
// -1 : err, 0 : continue, 1 : end
//-----------------------------------------------------------------------------
RwInt32	AgcdEffCtrl_ParticleSyst_SBH::Render(void)
{
	PROFILE("AgcdEffCtrl_ParticleSyst_SBH::bRender");

	LPD3DVTX_PCT_RECT	pVtxRect = NULL;
	// lock	
	RwMatrix*	pLTM = RwFrameGetLTM( GetPtrFrm() );
	RwV3d	vdpnd	= { 0.f, 0.f, 0.f };
	if( GetCPtrEffBase()->bFlagChk( FLAG_EFFBASEPSYSSBH_CHILDDEPENDANCY ) )
		vdpnd	= *RwMatrixGetPos( pLTM );

	if( SUCCEEDED(g_agcuEffVBMng.LockVB_Particle((VOID**)&pVtxRect)) )
	{
		LPPARTICLE	pParticle	= NULL;
		RwV3d		v3dPos		= {0.f,0.f,0.f};
		RwReal		fAngle		= 0.f;
		INT			nChk		= 0;

		LPParticleListItr	it_curr = m_listLpParticle.begin();
		while( it_curr != m_listLpParticle.end() )
		{
			pParticle	= (*it_curr);
			nChk = _ParticleUpdate( pParticle, &v3dPos, &fAngle, m_dwLastAccumulateTime, g_EnvrnParam );
			if( nChk )
			{
				if( T_ISMINUS4( nChk ) )
				{
					g_agcuEffVBMng.UnlockVB_Particle();
					return tReturnErr();
				}

				DEF_SAFEDELETE( pParticle );
				it_curr = m_listLpParticle.erase( it_curr );
				continue;
			}

			DWORD		colr = AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_TONEDOWN) 
				? DEF_ARGB32(pParticle->m_colr.alpha, pParticle->m_colr.red >> 1 , pParticle->m_colr.green >> 1 , pParticle->m_colr.blue >> 1 )
				: DEF_ARGB32(pParticle->m_colr.alpha, pParticle->m_colr.red, pParticle->m_colr.green, pParticle->m_colr.blue);

			new(pVtxRect) D3DVTX_PCT_RECT( DEFAULT_BOARDSIZE * pParticle->m_v3dScale.x * m_pEffCtrl_Set->GetScale()
				, DEFAULT_BOARDSIZE * pParticle->m_v3dScale.y * m_pEffCtrl_Set->GetScale()
				, colr//DEF_ARGB32(pParticle->m_colr.alpha, pParticle->m_colr.red, pParticle->m_colr.green, pParticle->m_colr.blue)
				, pParticle->m_stUV
				, fAngle );

			D3DXVECTOR3 v3dCenter( v3dPos.x, v3dPos.y, v3dPos.z);
			
			if( GetCPtrEffBase()->bFlagChk( FLAG_EFFBASEPSYSSBH_OLDTYPE ) )
			{
				if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASE_BILLBOARDY ) )
					pVtxRect->TransForm( BMATRIX::bGetInst().bGetPtrD3DMatBY() );
				else if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASE_BILLBOARD ) )
					pVtxRect->TransForm( BMATRIX::bGetInst().bGetPtrD3DMatB() );

				v3dCenter.x += vdpnd.x;
				v3dCenter.y += vdpnd.y;
				v3dCenter.z += vdpnd.z;
			}

			pVtxRect->Translate( v3dCenter );

			
#ifdef _CALCBOUNDINFO
			if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_CALCBOUND) )
			if( CALCBOUNDINFO::Begin() )
				AgcuEffVtx_CalcBBox( CALCBOUNDINFO::GetBox(), &pVtxRect->m_vtx0, 4, NULL );
#endif

			++pVtxRect;
			++it_curr;
		}

		// unlock
		g_agcuEffVBMng.UnlockVB_Particle();
	}


	AgcdEffRenderBase* pEffRenderBase
		= static_cast<AgcdEffRenderBase*>(m_lpEffBase);
	// SetRenderState
	pEffRenderBase->bSet_Renderstate();

	// SetTexture
	RwD3D9SetTexture( pEffRenderBase->bGetPtrRwTex(), 0LU );
	// Render
	if( GetCPtrEffBase()->bFlagChk( FLAG_EFFBASEPSYSSBH_OLDTYPE ) )
	{
		RwMatrix	mat;
		RwMatrixSetIdentity( &mat );
		g_agcuEffVBMng.RenderParticle( m_listLpParticle.size(), &mat, FALSE );
	}
	else
	{
		g_agcuEffVBMng.RenderParticle( m_listLpParticle.size(), RwFrameGetLTM ( m_pFrm ), FALSE );
	}

	// renderstate restore
	pEffRenderBase->bRestore_Renderstate();

	return 0;
}

//-----------------------------------------------------------------------------
// SetState
//-----------------------------------------------------------------------------
void AgcdEffCtrl_ParticleSyst_SBH::SetState( E_EFFCTRL_STATE eState )
{
	if( eState == E_EFFCTRL_STATE_BEGIN )
	{
		m_dwLastShootTime		 =
		m_dwLastAccumulateTime = 
			m_pEffCtrl_Set->GetAccumulateTime();
	}
	
	AgcdEffCtrl::SetState( eState );
}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_Tail
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Tail::AgcdEffCtrl_Tail( AgcdEffCtrl_Set* lpEffCtrl_Set
								  , AgcdEffBase*	lpEffBase
								  , RwUInt32		dwIndex
								  , RwFrame*		pFrmTarget
								  , FPTR_GetFrame	fptrGetFrame )
: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
, m_eType( e_TailTypeCMR )
, m_dwLastAccumulateTime( 0LU )
, m_fptrGetFrame( fptrGetFrame )
, m_pEffAnim_Colr( NULL )
, m_pEffAnim_TuTv( NULL )
{
	EFFMEMORYLOG_CON;

	m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget = pFrmTarget;

	AgcdEffTail*	pEffBase_Tail = static_cast<AgcdEffTail*>( lpEffBase );
	ASSERT( "kday" && lpEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_TAIL );

	EFFBASE::LPEffAniVecItr	it_curr
		= m_lpEffBase->bGetRefAnimList().begin();
	EFFBASE::LPEffAniVecItr	it_last
		= m_lpEffBase->bGetRefAnimList().end();
	for( ; it_curr!=it_last; ++it_curr )
	{
		if( (*it_curr)->bGetAnimType() == AgcdEffAnim::E_EFFANIM_COLOR )
		{
			ASSERT( "kday" && !m_pEffAnim_Colr );
			m_pEffAnim_Colr = static_cast<AgcdEffAnim_Colr*>(*it_curr);
		}
		else if( (*it_curr)->bGetAnimType() == AgcdEffAnim::E_EFFANIM_TUTV )
		{
			ASSERT( "kday" && !m_pEffAnim_TuTv );
			m_pEffAnim_TuTv = static_cast<AgcdEffAnim_TuTv*>(*it_curr);
		}
	}
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Tail::SetTargetInfo(  RwFrame* pFrmNode1
										 , RwFrame* pFrmNode2 )
{
	if( pFrmNode1 && pFrmNode2)
	{
		m_stTailInfo.m_eType = stTailInfo::e_nodebase;
		m_stTailInfo.m_stNodeInfo.m_pFrmNode1 = pFrmNode1;
		m_stTailInfo.m_stNodeInfo.m_pFrmNode2 = pFrmNode2;
	}
	else
	{
		return tReturnErr();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Tail::SetTargetInfo(RwFrame* pFrmTarget, RwReal fHeight1, RwReal fHeight2)
{
	if( pFrmTarget )
	{
		m_stTailInfo.m_eType	= stTailInfo::e_heightbase;
		m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget	= pFrmTarget;
		m_stTailInfo.m_stHeightInfo.m_fHeight1			= fHeight1;
		m_stTailInfo.m_stHeightInfo.m_fHeight2			= fHeight2;
	}
	else
	{
		Eff2Ut_ERR( "pFrmTarget == NULL @ bSetTargetInfo" );
		return tReturnErr();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Tail::SetTargetInfo(RwReal fHeight1, RwReal fHeight2)
{
	m_stTailInfo.m_eType	= stTailInfo::e_heightbase;
	m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget	= NULL;
	m_stTailInfo.m_stHeightInfo.m_fHeight1			= fHeight1;
	m_stTailInfo.m_stHeightInfo.m_fHeight2			= fHeight2;
	return 0;
}

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Tail::~AgcdEffCtrl_Tail()
{
	EFFMEMORYLOG_DES;
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Tail::_CurrPointUpdate()
{
	// 미사일 에니메이션이 끝났을때 디팬던시가 걸린 테일의 현재점 위치 업데이트 중지.
	if( m_pEffCtrl_Set->GetState() == E_EFFCTRL_STATE_END )
		return 0;
	//<----
	if( m_stTailInfo.m_eType == stTailInfo::e_nodebase )
	{
		//체크할것
		if( !m_stTailInfo.m_stNodeInfo.m_pFrmNode1 || 
			!m_stTailInfo.m_stNodeInfo.m_pFrmNode2	 )
			return tReturnErr();

		if( !m_stTailInfo.m_stNodeInfo.m_pFrmNode1 || !m_stTailInfo.m_stNodeInfo.m_pFrmNode2 )
			return tReturnErr();

		m_stCurrTPoint.m_v3dP2 = *RwMatrixGetPos( RwFrameGetLTM( m_stTailInfo.m_stNodeInfo.m_pFrmNode1 ) );
		m_stCurrTPoint.m_v3dP1 = *RwMatrixGetPos( RwFrameGetLTM( m_stTailInfo.m_stNodeInfo.m_pFrmNode2 ) );
	}
	else if( m_stTailInfo.m_eType == stTailInfo::e_heightbase )
	{
		
		if(!m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget)
			return tReturnErr();
	
		RwV3d	vFrmUp;
		if( m_lpEffBase->bFlagChk(FLAG_EFFBASETAIL_Y90) )
		{
			RwV3dNormalize( &vFrmUp, &RwFrameGetLTM(m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget)->right );
		}

		else
		{
			RwV3dNormalize( &vFrmUp, &RwFrameGetLTM(m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget)->up );
			if( m_lpEffBase->bFlagChk(FLAG_EFFBASETAIL_Y45) )
			{
				RwMatrix	mat;
				RwMatrixRotate (&mat, &RwFrameGetLTM(m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget)->at, 45.f, rwCOMBINEREPLACE );
				RwV3dTransformVector(&vFrmUp, &vFrmUp, &mat);
			}
			else if( m_lpEffBase->bFlagChk(FLAG_EFFBASETAIL_Y135) )
			{
				RwMatrix	mat;
				RwMatrixRotate (&mat, &RwFrameGetLTM(m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget)->at, 135.f, rwCOMBINEREPLACE );
				RwV3dTransformVector(&vFrmUp, &vFrmUp, &mat);
			}

		}
		RwV3dScale( &m_stCurrTPoint.m_v3dP1, &vFrmUp, m_stTailInfo.m_stHeightInfo.m_fHeight1 );
		RwV3dScale( &m_stCurrTPoint.m_v3dP2, &vFrmUp, m_stTailInfo.m_stHeightInfo.m_fHeight2 );
		
		RwMatrix*	pLTM = RwFrameGetLTM(m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget);
		RwV3dAdd( &m_stCurrTPoint.m_v3dP1, &pLTM->pos, &m_stCurrTPoint.m_v3dP1 );
		RwV3dAdd( &m_stCurrTPoint.m_v3dP2, &pLTM->pos, &m_stCurrTPoint.m_v3dP2 );
	}

	if( m_stllistTPoint.empty() )
	{
		m_stCurrTPoint.m_dwTime	= GetCPtrEffCtrl_Set()->GetAccumulateTime();
		m_stStartPoint	= m_stCurrTPoint;
		m_stllistTPoint.push_back( m_stCurrTPoint );
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Tail::_FirstPointUpdate(RwUInt32 dwAccumulateTime)
{
	//생각을 해보자.
	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Tail::_TailUpdate(RwUInt32 dwAccumulateTime)
{

	AgcdEffTail*	pEffBase_Tail
		= static_cast<AgcdEffTail*> ( m_lpEffBase );
	//insert
	RwUInt32	dwPastTime	= dwAccumulateTime - m_dwLastAccumulateTime;

	if( dwPastTime >= pEffBase_Tail->bGetTimeGap() )
	{
		dwPastTime -= pEffBase_Tail->bGetTimeGap();

		m_stCurrTPoint.m_dwTime	= dwAccumulateTime;
		if( m_stllistTPoint.empty() )
		{
			m_stStartPoint	= m_stCurrTPoint;
		}
		m_stllistTPoint.push_back( m_stCurrTPoint );
		m_dwLastAccumulateTime	= dwAccumulateTime;
		
		if( 5 < m_stllistTPoint.size() )
		{
			m_stllistTPoint.pop_front();
			m_stStartPoint	= m_stllistTPoint.front();
		}
		if( pEffBase_Tail->bGetCapacity() <= static_cast<RwInt32>(m_stllistTPoint.size()) )
			m_stllistTPoint.pop_front();

	}

	if( m_stllistTPoint.empty() )
		return 0;

	TAILPOINT*	pTailPoint;

	TailPointListItr	it_curr	
		= m_stllistTPoint.begin();

	//life check
	for( ; it_curr != m_stllistTPoint.end();  )
	{
		pTailPoint	= (&(*it_curr));	
		if( dwAccumulateTime - pTailPoint->m_dwTime >= pEffBase_Tail->bGetPointLife() )
		{
			m_stllistTPoint.erase( it_curr++ );
			if( !m_stllistTPoint.empty() )
			{
				m_stStartPoint	= m_stllistTPoint.front();
			}

			continue;
		}
		else
			break;
	}
	if( !m_stllistTPoint.empty() )
	{
		it_curr	= m_stllistTPoint.begin();
		pTailPoint	= &(*it_curr);
		TAILPOINT*	pTailPoint2 = (m_stllistTPoint.size() > 1) ? &(*(++it_curr)) : &m_stCurrTPoint;

		if( dwPastTime >= pEffBase_Tail->bGetTimeGap() )
			dwPastTime %= pEffBase_Tail->bGetTimeGap();
		RwReal	fCof	= static_cast<float>(dwPastTime) 
						/ static_cast<float>( pEffBase_Tail->bGetTimeGap() );
		
		LinearIntp( &pTailPoint->m_v3dP1, &m_stStartPoint.m_v3dP1, &pTailPoint2->m_v3dP1, fCof );
		LinearIntp( &pTailPoint->m_v3dP2, &m_stStartPoint.m_v3dP2, &pTailPoint2->m_v3dP2, fCof );

		return 0;
	}

	return 1;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Tail::Update(RwUInt32 dwAccumulateTime)
{
	PROFILE("AgcdEffCtrl_Tail::bUpdate");

	if( GetState() == E_EFFCTRL_STATE_END )
		return 1;

	if( !m_lpEffBase )
		return tReturnErr();

	RwUInt32	dwCurrTime	= 0LU;
	RwInt32		ir			= tLifeCheck( &dwCurrTime, dwAccumulateTime );

	if( ir )
	{
		if( T_ISMINUS4( ir ) )
			return tReturnErr();
		
		if( m_stllistTPoint.empty() )
			return 1;

		return _TailUpdate( dwAccumulateTime );
	}
	switch( GetState() )
	{
	case E_EFFCTRL_STATE_WAIT:				
		return 0;
	case E_EFFCTRL_STATE_END:				
		return 1;
	}

	_CurrPointUpdate();

	//이넘을 어디서 업데이트 시킬까?
	if( m_pEffAnim_Colr )
		m_pEffAnim_Colr->bGetTVal( static_cast<void*>( &m_rgba ), dwCurrTime );

	if( m_pEffAnim_TuTv )
		m_pEffAnim_TuTv->bGetTVal( static_cast<void*>( &m_stUVRect ), dwCurrTime );

	return _TailUpdate( dwAccumulateTime );
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Tail::Render()
{
	PROFILE("AgcdEffCtrl_Tail::bRender");

	if( GetState() == E_EFFCTRL_STATE_END )
	{
		return 1;
	}

	if( m_stllistTPoint.empty() )
		return 0;

	AgcdEffTail* lpEffBase_Tail = static_cast<AgcdEffTail*>(m_lpEffBase);

	AgcuBezierForTail<D3DVTX_PCT>		bezier;
	AgcuCatmull_Rom_Tail<D3DVTX_PCT>	catmull;

	LPD3DVTX_PCT	pVtx = NULL;

#ifdef _DEBUG
	if(m_stTailInfo.m_eType == stTailInfo::e_heightbase)
	{
		m_eType = e_TailTypeCMR;
	}
	else
	{
		static BOOL bBezier	= TRUE;
		static BOOL bB		= FALSE;
		if( KEYDOWN_ONECE('B', bB) )
			if( KEYDOWN( VK_SHIFT ) && KEYDOWN( VK_CONTROL ) )
				bBezier	= !bBezier;
		if( bBezier )
		{
			m_eType = e_TailTypeBEZ;
		}
		else
		{
			m_eType = e_TailTypeCMR;
		}
	}
#else
	if(m_stTailInfo.m_eType == stTailInfo::e_heightbase)
	{
		m_eType = e_TailTypeCMR;
	}
	else
	{
		m_eType = e_TailTypeBEZ;
	}
#endif //_DEBUG

	//일단.
	lpEffBase_Tail->bSetCapacity( (m_eType == e_TailTypeBEZ) ? 16 : 20 );

	if( SUCCEEDED( 
			g_agcuEffVBMng.LockVB_Tail( (VOID**)&pVtx
				, (lpEffBase_Tail->bGetCapacity() * m_stllistTPoint.size()  + 1)*2
		)))
	{
		DWORD		colr = AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_TONEDOWN) 
			? DEF_ARGB32(m_rgba.alpha, m_rgba.red >> 1 , m_rgba.green >> 1 , m_rgba.blue >> 1)
			: DEF_ARGB32(m_rgba.alpha, m_rgba.red, m_rgba.green, m_rgba.blue);

		switch( m_eType )
		{
		case e_TailTypeBEZ:{
				RwInt32	ir	= 
				bezier.bBezier( pVtx, lpEffBase_Tail->bGetCapacity(), m_stCurrTPoint, m_stllistTPoint , colr );
				if( T_ISMINUS4( ir ) )
				{
					return tReturnErr();
				}
			}break;
		case e_TailTypeCMR:{
				RwInt32	ir	= 
				catmull.bCatmull_Rom( pVtx, m_stllistTPoint, m_stCurrTPoint, lpEffBase_Tail->bGetCapacity() , colr );
				if( T_ISMINUS4( ir ) )
				{
					return tReturnErr();
				}
			}break;
		case e_TailTypeLine:{
			}break;
		}

		g_agcuEffVBMng.UnlockVB_Tail();
	}

	// SetRenderState
	lpEffBase_Tail->bSet_Renderstate();

//테일 가이드 라인 체크
#ifdef _DEBUG
	static BOOL bShowGuidLine	= FALSE;
	static BOOL bG				= FALSE;
	if( KEYDOWN_ONECE( 'G', bG ) )
	{
		if( KEYDOWN( VK_SHIFT ) && KEYDOWN( VK_CONTROL ) )
			bShowGuidLine	= !bShowGuidLine;
	}
	if( bShowGuidLine )
	{
		D3DVTX_PCT	avtxTop[6];
		D3DVTX_PCT	avtxBottom[6];

		TailPointListItr	it_curr
			= m_stllistTPoint.begin();

		TAILPOINT*	pTailPoint	= NULL;

		int i=0;

		for( ; it_curr != m_stllistTPoint.end(); ++i, ++it_curr )
		{
			pTailPoint	= &(*it_curr);
			avtxTop[i].m_v3Pos.x	= pTailPoint->m_v3dP1.x;
			avtxTop[i].m_v3Pos.y	= pTailPoint->m_v3dP1.y;
			avtxTop[i].m_v3Pos.z	= pTailPoint->m_v3dP1.z;
			avtxTop[i].m_dwColr		= 0xffff0000;

			avtxBottom[i].m_v3Pos.x	= pTailPoint->m_v3dP2.x;
			avtxBottom[i].m_v3Pos.y	= pTailPoint->m_v3dP2.y;
			avtxBottom[i].m_v3Pos.z	= pTailPoint->m_v3dP2.z;
			avtxBottom[i].m_dwColr	= 0xff0000ff;
		}
		avtxTop[i].m_v3Pos.x	= m_stCurrTPoint.m_v3dP1.x;
		avtxTop[i].m_v3Pos.y	= m_stCurrTPoint.m_v3dP1.y;
		avtxTop[i].m_v3Pos.z	= m_stCurrTPoint.m_v3dP1.z;
		avtxTop[i].m_dwColr		= 0xffff0000;
		
		avtxBottom[i].m_v3Pos.x	= m_stCurrTPoint.m_v3dP2.x;
		avtxBottom[i].m_v3Pos.y	= m_stCurrTPoint.m_v3dP2.y;
		avtxBottom[i].m_v3Pos.z	= m_stCurrTPoint.m_v3dP2.z;
		avtxBottom[i].m_dwColr	= 0xff0000ff;

		if( i > 1 )
		{
			RwD3D9SetTexture( NULL, 0LU );
			AgcuEffVtx_DrawLine( avtxTop, i );
			AgcuEffVtx_DrawLine( avtxBottom, i );

			//AgcuEffVtx_DrawLine( &g_testLine[0], lpEffBase_Tail->bGetCapacity() );
		}
	}
#endif //_DEBUG

	// SetTexture
	RwD3D9SetTexture( lpEffBase_Tail->bGetPtrRwTex(), 0LU );
	// Render
	RwMatrix	mat;
	RwMatrixSetIdentity( &mat );
	if( m_stllistTPoint.size() == 1 )
	{
		g_agcuEffVBMng.RenderTail( 1, &mat );
	}
	else
	{
		switch( m_eType )
		{
		case e_TailTypeBEZ:{
			g_agcuEffVBMng.RenderTail( lpEffBase_Tail->bGetCapacity()-1, &mat );
			}break;
		case e_TailTypeCMR:{
			g_agcuEffVBMng.RenderTail( (lpEffBase_Tail->bGetCapacity()*m_stllistTPoint.size())-1, &mat );
			}break;
		case e_TailTypeLine:{
			}break;
		}
	}

	// renderstate restore
	lpEffBase_Tail->bRestore_Renderstate();

	return 0;
}

//-----------------------------------------------------------------------------
// SetState
//-----------------------------------------------------------------------------
void AgcdEffCtrl_Tail::SetState( E_EFFCTRL_STATE eState )
{
	if( eState == E_EFFCTRL_STATE_BEGIN )
	{
		m_dwLastAccumulateTime = 
			m_pEffCtrl_Set->GetAccumulateTime() 
			- m_lpEffBase->bGetDelay();
	}
	
	AgcdEffCtrl::SetState( eState );
}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_Obj
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Obj::AgcdEffCtrl_Obj( AgcdEffCtrl_Set* lpEffCtrl_Set
								, AgcdEffBase* lpEffBase
								, RwUInt32 dwIndex
								, RwFrame* pFrmParent )
: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
, m_pFrm( NULL )
, m_pClump( NULL )
, m_pRtAnim( NULL )
, m_bAddedWorld( FALSE )
, m_eCurrBlendType( static_cast<AgcdEffRenderBase*>(lpEffBase)->bGetBlendType() )
{
	EFFMEMORYLOG_CON;
	
	m_pFrm	= RwFrameCreate();
	ASSERT( m_pFrm );
	if( !m_pFrm )
	{
		Eff2Ut_ERR( "RwFrameCreate failed" );
		SetState(E_EFFCTRL_STATE_END);
	}
	else
	{
		RwMatrixSetIdentity ( &m_pFrm->modelling );
		if( pFrmParent )
		{
			RwFrameAddChild( pFrmParent, m_pFrm );
		}
		else
			RwFrameUpdateObjects( m_pFrm );
	}
	
	AgcdEffObj*	lpEffBase_Obj
		= static_cast<AgcdEffObj*>(lpEffBase);
	if( lpEffBase_Obj )
	{
		//ASSERT( lpEffBase_Obj->bGetPtrClump() );
		if( lpEffBase_Obj->bGetPtrClump() )
		{
			{
			PROFILE("RpClumpClone");
			//clump clone
            m_pClump	= RpClumpClone( lpEffBase_Obj->bGetPtrClump() );
			//m_pClump	= lpEffBase_Obj->bLoadClump();
			ASSERT( m_pClump );
			if( !m_pClump )
			{
				Eff2Ut_ERR( "RpClumpClone failed @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
				SetState(E_EFFCTRL_STATE_END);
			}
			}

			//RtAnimation
			EFFBASE::LPEffAniVecItr	it_curr
				= m_lpEffBase->bGetRefAnimList().begin();
			EFFBASE::LPEffAniVecItr	it_last
				= m_lpEffBase->bGetRefAnimList().end();
			for( ; it_curr != it_last; ++it_curr )
			{
				ASSERT( "kday" && *it_curr );
				if( (*it_curr)->bGetAnimType() == AgcdEffAnim::E_EFFANIM_RTANIM )
				{
					m_lpEffAnim_RtAnim = static_cast<AgcdEffAnim_RtAnim*>( *it_curr );
					break;
				}
			}
			if( m_lpEffAnim_RtAnim )
			{
				RwChar	szFullName[MAX_PATH]	= "";
				strcpy( szFullName, AgcuEffPath::GetPath_Anim() );
				strcat( szFullName, m_lpEffAnim_RtAnim->bGetAnimFName() );				
				ASSERT( sizeof(szFullName) > strlen(szFullName) );

				m_pRtAnim = AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->LoadRtAnim(szFullName);
				ASSERT( m_pRtAnim );
				if( m_pRtAnim )
				{
					RpHAnimHierarchy *hierarchy = 
						Eff2Ut_GetHierarchyFromRwFrame( RpClumpGetFrame( m_pClump ) );
					ASSERT( hierarchy );
					if( !hierarchy )
					{
						Eff2Ut_ERR( "hierarch == NULL @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
						SetState(E_EFFCTRL_STATE_END);
					}
					else
					{
						if( RpClumpForAllAtomics(m_pClump, Eff2Ut_SetHierarchyForSkinAtomic, (void *)hierarchy) )
						{
						
							if( RpHAnimHierarchySetFlags( hierarchy
														, (RpHAnimHierarchyFlag)
														  (RpHAnimHierarchyGetFlags(hierarchy)		| 
														   rpHANIMHIERARCHYUPDATELTMS				|
														   rpHANIMHIERARCHYUPDATEMODELLINGMATRICES	) ) )
							{
								if( RpHAnimHierarchySetCurrentAnim( hierarchy, m_pRtAnim ) )
								{
									if( !RpHAnimHierarchyAttach(hierarchy) )
									{
										Eff2Ut_ERR( "RpHAnimHierarchyAttach failed @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
										SetState(E_EFFCTRL_STATE_END);
									}
								}
								else
								{
									Eff2Ut_ERR( "RpHAnimHierarchySetCurrentAnim failed @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
									SetState(E_EFFCTRL_STATE_END);
								}
							}
							else
							{
								Eff2Ut_ERR( "RpHAnimHierarchySetFlags failed @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
								SetState(E_EFFCTRL_STATE_END);
							}
						}
						else
						{
							Eff2Ut_ERR( "RpClumpForAllAtomics failed @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
							SetState(E_EFFCTRL_STATE_END);
						}
					}

				}
				else
				{
					Eff2Ut_ERR( "RtAnimAnimationRead failed @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
					SetState(E_EFFCTRL_STATE_END);
				}
			}

		}
		else
		{
			Eff2Ut_ERR( "lpEffBase_Obj->bGetPtrClump() == NULL and not dummy object @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
			SetState(E_EFFCTRL_STATE_END);
		}

		if( m_pFrm && ( m_pClump != NULL && RpClumpGetFrame( m_pClump ) ))
		{
			RwFrameTransform( m_pFrm, RwFrameGetMatrix( RpClumpGetFrame( m_pClump ) ), rwCOMBINEREPLACE );
			
			RwFrameGetMatrix( RpClumpGetFrame( m_pClump ) )->pos.x = 
			RwFrameGetMatrix( RpClumpGetFrame( m_pClump ) )->pos.y = 
			RwFrameGetMatrix( RpClumpGetFrame( m_pClump ) )->pos.z = 0.f;

			RwFrame*	pClumpFrame = RpClumpGetFrame(m_pClump);
			RwFrameAddChild( m_pFrm, pClumpFrame );

			//only for debugging
#ifdef _DEBUG
			//@{ Jaewon 20050905
			// ;)
			//m_pClump->szName = const_cast<char*>( m_pEffCtrl_Set->bGetPtrEffSet()->bGetTitle() );
			RpClumpSetName(m_pClump, m_pEffCtrl_Set->GetPtrEffSet()->bGetTitle());
			//@} Jaewon
#endif
		}
		else
		{
			Eff2Ut_ERR( "m_pFrm == NULL || RwClumpGetFrame( m_pClump )==NULL @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
			SetState(E_EFFCTRL_STATE_END);
		}
	}
	else
	{
		Eff2Ut_ERR("lpEffBase == NULL @ AgcdEffCtrl_Obj::CONSTRUCTOR" );
		SetState(E_EFFCTRL_STATE_END);
	}	

}

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Obj::~AgcdEffCtrl_Obj()
{
	EFFMEMORYLOG_DES;

	SetState( E_EFFCTRL_STATE_END );
	if( m_bAddedWorld )
	{

	}

	m_pEffCtrl_Set->PreRemoveFrm( m_listPreRemoveFrm );

	if( m_pClump )
	{		
		RwFrame*	pFrame = RpClumpGetFrame(m_pClump);
		ASSERT( pFrame 
			 && RwFrameGetParent(pFrame) 
			 && RwFrameGetParent(pFrame) == m_pFrm 
			  );
		RwFrameRemoveChild( pFrame );
	}

	if( DEF_FLAG_CHK( GetPtrEffBase()->bGetFlag(), FLAG_EFFBASE_BASEDPND_TOOBJ ) )
	{		
		m_pEffCtrl_Set->DelPreRemoveFrm( GetPtrFrm() );
	}
	
	Eff2Ut_SAFE_DESTROY_FRAME( m_pFrm );
	Eff2Ut_SAFE_DESTROY_CLUMP( m_pClump );
	Eff2Ut_SAFE_DESTROY_RTANIM( m_pRtAnim );
}

//-----------------------------------------------------------------------------
// tExpandForMissileChild
//-----------------------------------------------------------------------------
BOOL AgcdEffCtrl_Obj::_ExpandForMissileChild()
{
	if( !GetCPtrEffBase()->bFlagChk( FLAG_EFFBASEOBJ_CHILDOFMISSILE ) )
		return TRUE;

	RwV3d	vposClump	= *RwMatrixGetPos( RwFrameGetLTM( GetPtrFrmClump() ) );
	RwV3d	vposEffSet	= *m_pEffCtrl_Set->GetPos();
	RwV3d	offset	= { vposEffSet.x - vposClump.x
		, vposEffSet.y - vposClump.y
		, vposEffSet.z - vposClump.z };

	RwReal	flen	= RwV3dLength( &offset );
	flen	*= -.01f;

	RwV3d	scale	= { 1.f, flen, 1.f };
	
	RwFrameScale( GetPtrFrm(), &scale, rwCOMBINEPRECONCAT );

	return TRUE;
}

//-----------------------------------------------------------------------------
// bUpdate
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Obj::Update(RwUInt32 dwAccumulateTime)
{
	PROFILE("AgcdEffCtrl_Obj::bUpdate");

	RwInt32	ir	= AgcdEffCtrl_Base::Update(dwAccumulateTime);
	ASSERT( !T_ISMINUS4( ir ) );
	if( ir )
	{
		if( T_ISMINUS4( ir ) )
		{
			Eff2Ut_ERR( "AgcdEffCtrl_Base::bUpdate(dwAccumulateTime) failed @ AgcdEffCtrl_Obj::bUpdate" );
			return tReturnErr();
		}

		return ir;
	}

	_ExpandForMissileChild();

	
#ifdef _CALCBOUNDINFO
	if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_CALCBOUND) )
	if( CALCBOUNDINFO::Begin() &&
		!DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEOBJ_DUMMY ) )
	{
		RwBBox	bbox = {{-9999.f,-9999.f,-9999.f},{9999.f,9999.f,9999.f}};
		Eff2Ut_CalcBBox( m_pClump, (void*)&bbox );

		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
		RwMatrix*	pFrameLTM = RwFrameGetLTM( RpClumpGetFrame(m_pClump) );
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();

		RwV3dTransformPoint( &bbox.inf, &bbox.inf, pFrameLTM );
		RwV3dTransformPoint( &bbox.sup, &bbox.sup, pFrameLTM );
				
		D3DXVec3Minimize ( (D3DXVECTOR3*)&CALCBOUNDINFO::GetBox()->inf
			, (D3DXVECTOR3*)&CALCBOUNDINFO::GetBox()->inf
			, (D3DXVECTOR3*)&bbox.inf );
		D3DXVec3Minimize ( (D3DXVECTOR3*)&CALCBOUNDINFO::GetBox()->inf
			, (D3DXVECTOR3*)&CALCBOUNDINFO::GetBox()->inf
			, (D3DXVECTOR3*)&bbox.sup );

		D3DXVec3Maximize ( (D3DXVECTOR3*)&CALCBOUNDINFO::GetBox()->sup
			, (D3DXVECTOR3*)&CALCBOUNDINFO::GetBox()->sup
			, (D3DXVECTOR3*)&bbox.inf );
		D3DXVec3Maximize ( (D3DXVECTOR3*)&CALCBOUNDINFO::GetBox()->sup
			, (D3DXVECTOR3*)&CALCBOUNDINFO::GetBox()->sup
			, (D3DXVECTOR3*)&bbox.sup );

		Eff2Ut_CalcSphere(CALCBOUNDINFO::GetSphere(), CALCBOUNDINFO::GetBox());
	}
#endif

	return 0;
}

//-----------------------------------------------------------------------------
// bRender
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Obj::Render(void)
{
	ASSERT("kday" && m_lpEffBase->bIsRenderBase());

	if( !DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEOBJ_DUMMY ) )
	{
		//modify clor
		if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_TONEDOWN) )
		{
			RwRGBA	rgba = { m_rgba.red >> 1, m_rgba.green >> 1, m_rgba.blue >> 1, m_rgba.alpha };
			RpClumpForAllAtomics( m_pClump, Eff2Ut_AtomicModifyColor, &rgba);
		}
		else
			RpClumpForAllAtomics( m_pClump, Eff2Ut_AtomicModifyColor, &m_rgba);

		
		// SetRenderState
		static_cast<AgcdEffRenderBase*>(m_lpEffBase)->bSet_Renderstate();

		AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->OriginalClumpRender( m_pClump );

		// renderstate restore
		static_cast<AgcdEffRenderBase*>(m_lpEffBase)->bRestore_Renderstate();
	}


	return 0;
};

void AgcdEffCtrl_Obj::SetState( E_EFFCTRL_STATE eState )
{
	if( GetState() == eState )
		return;

	if( !DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEOBJ_DUMMY ) )
	{
		if( eState == E_EFFCTRL_STATE_BEGIN )
		{
			
			if( AgcdEffGlobal::bGetInst().bGetPtrAgcmUVAnimation() && m_pClump )
			{
				AgcmUVAnimation::CallBack_AddClump( 
					m_pClump
					, AgcdEffGlobal::bGetInst().bGetPtrAgcmUVAnimation()
					, NULL );
			}

			ASSERT( !m_bAddedWorld );
			if( m_bAddedWorld )
			{
				Eff2Ut_ERR( "이미 월드에 추가됨!" );
				AgcdEffCtrl::SetState( E_EFFCTRL_STATE_END );
				return;
			}
			
		}
		else if( eState == E_EFFCTRL_STATE_END )
		{
			
			if( AgcdEffGlobal::bGetInst().bGetPtrAgcmUVAnimation() && m_pClump)
			{
				AgcmUVAnimation::CallBack_DeleteClump( 
					m_pClump
					, AgcdEffGlobal::bGetInst().bGetPtrAgcmUVAnimation()
					, NULL );
			}

			if( m_bAddedWorld )
			{
				RwRGBA	rgba = { m_rgba.red, m_rgba.green, m_rgba.blue, 0 };
				RpClumpForAllAtomics( m_pClump, Eff2Ut_AtomicModifyColor, &rgba);
			}
		}
	}

	AgcdEffCtrl::SetState( eState );
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Obj::AddPreRemoveFrm( RwFrame* pFrm )
{
	RwFramsListItr	it_f
		= std::find( m_listPreRemoveFrm.begin(), m_listPreRemoveFrm.end(), pFrm );
	if( it_f != m_listPreRemoveFrm.end() )
	{
		Eff2Ut_ERR("AgcdEffCtrl_Obj::bAddPreRemoveFrm failed!");
		return tReturnErr();
	}

	m_listPreRemoveFrm.push_back( pFrm );
	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
VOID AgcdEffCtrl_Obj::ToneDown()
{
	if( !DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEOBJ_DUMMY ) )
	{
		RwRGBA	rgba	= { m_rgba.red >> 1, m_rgba.green >> 1, m_rgba.blue >> 1, m_rgba.alpha };
		RpClumpForAllAtomics( m_pClump, Eff2Ut_AtomicModifyColor, &rgba);
	}
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
VOID AgcdEffCtrl_Obj::ToneRestore()
{
	if( !DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASEOBJ_DUMMY ) )
	{
		RpClumpForAllAtomics( m_pClump, Eff2Ut_AtomicModifyColor, &m_rgba);
	}
}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_Light
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Light::AgcdEffCtrl_Light( AgcdEffCtrl_Set* lpEffCtrl_Set
									, AgcdEffBase* lpEffBase
									, RwUInt32 dwIndex
									, RwFrame* pFrmParent )
: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
, m_pLight( NULL )
, m_bAddedToWorld( FALSE )
{
	EFFMEMORYLOG_CON;

	ASSERT( "kday" && lpEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_LIGHT );

	AgcdEffLight*	lpEffBase_Light
		= static_cast<AgcdEffLight*>( lpEffBase );
	if( lpEffBase_Light )
	{
		RpLightType	lightType;
		switch( lpEffBase_Light->bGetLightType() )
		{
		case 0:
			lightType	= (RpLightType)AgcdEffLight::E_EFFLIGHT_POINT;
			break;
		case 1:
			lightType	= (RpLightType)AgcdEffLight::E_EFFLIGHT_SOFTSPOT;
			break;
		case 2:
			lightType	= (RpLightType)AgcdEffLight::E_EFFLIGHT_SPOT;
			break;
		default:
			lightType	= (RpLightType)(lpEffBase_Light->bGetLightType());
		}

		m_pLight	= RpLightCreate( lightType );
		ASSERT( m_pLight );
		if( m_pLight )
		{
			RwFrame*	pFrm	= RwFrameCreate();
			if( pFrm )
			{
				RwMatrixSetIdentity ( &pFrm->modelling );
				RpLightSetFrame ( m_pLight, pFrm );
				if( pFrmParent )
					RwFrameAddChild( pFrmParent, pFrm );
				else
					RwFrameUpdateObjects( pFrm );

				RpLightSetRadius( m_pLight, LIGHTRADIUSRATION );
				RpLightSetConeAngle( m_pLight, DEF_D2R( lpEffBase_Light->bGetConAngle() ) );

				
				const RwReal invs_255	= 1.f/255.f;
				RwRGBAReal		frgba = { m_rgba.red * invs_255
								, m_rgba.green * invs_255
								, m_rgba.blue * invs_255
								, m_rgba.alpha * invs_255 };

				RpLightSetColor( m_pLight, &frgba );
			}
			else
			{
				Eff2Ut_ERR( "RwFrameCreate failed @ @ AgcdEffCtrl_Light::CONSTRUCTOR");
				SetState(E_EFFCTRL_STATE_END);
			}
		}
		else
		{
			Eff2Ut_ERR( "RpLightCreate failed @ @ AgcdEffCtrl_Light::CONSTRUCTOR");
			SetState(E_EFFCTRL_STATE_END);
		}			
	}
	else
	{
		Eff2Ut_ERR( "lpEffBase == NULL @ AgcdEffCtrl_Light::CONSTRUCTOR");
		SetState(E_EFFCTRL_STATE_END);
	}

}

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Light::~AgcdEffCtrl_Light()
{
	EFFMEMORYLOG_DES;

	if( DEF_FLAG_CHK( GetPtrEffBase()->bGetFlag(), FLAG_EFFBASE_BASEDPND_TOOBJ ) )
	{
		m_pEffCtrl_Set->DelPreRemoveFrm( GetPtrFrm() );
	}

	if( m_bAddedToWorld )
	{
		if( AgcdEffGlobal::bGetInst().bGetPtrAgcmDynamicLightmap() )
			AgcdEffGlobal::bGetInst().bGetPtrAgcmDynamicLightmap()->removeLight( m_pLight );
		AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->RemoveLightFromRenderWorld(
			m_pLight
			, m_pEffCtrl_Set->FlagChk(FLAG_EFFCTRLSET_STATIC) ? FALSE : TRUE
			);

		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
		RwMatrix*		pLTM = RwFrameGetLTM( GetPtrFrm());
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
	}

	Eff2Ut_SAFE_DESTROY_LIGHT( m_pLight );
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Light::SetLightRadius(RwReal fRadius)
{
	if( !m_pLight )
	{
		Eff2Ut_ERR( "m_pLight==NULL @ AgcdEffCtrl_Light::bSetLightRadius" );
		return tReturnErr();
	}

	RpLightSetRadius( m_pLight, fRadius );
	return 0;
}
	
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void AgcdEffCtrl_Light::SetState( E_EFFCTRL_STATE eState )
{
	if( eState == E_EFFCTRL_STATE_BEGIN )
	{
		RwMatrix*	pLTM = RwFrameGetLTM( GetPtrFrm() );

		AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->AddLightToRenderWorld(
			m_pLight
			, m_pEffCtrl_Set->FlagChk(FLAG_EFFCTRLSET_STATIC) ? FALSE : TRUE);
		if( AgcdEffGlobal::bGetInst().bGetPtrAgcmDynamicLightmap() )
			AgcdEffGlobal::bGetInst().bGetPtrAgcmDynamicLightmap()->addLight( m_pLight
			, m_pEffCtrl_Set->FlagChk(FLAG_EFFCTRLSET_STATIC) ? FALSE : TRUE );

		m_bAddedToWorld	= TRUE;
	}

	AgcdEffCtrl::SetState( eState );
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Light::Update( RwUInt32 dwAccumulateTime )
{
	PROFILE("AgcdEffCtrl_Light::bUpdate");

	RwInt32	ir	= AgcdEffCtrl_Base::Update(dwAccumulateTime);

	ASSERT( !T_ISMINUS4( ir ) );
	if( ir )
	{
		if( T_ISMINUS4( ir ) )
		{
			Eff2Ut_ERR( "AgcdEffCtrl_Base::bUpdate(dwAccumulateTime) failed @ AgcdEffCtrl_Light::bUpdate" );
			return tReturnErr();
		}

		return ir;
	}

	//mat	= matScale * matRotate * matBillboard * matTranslate;
	RwFrame*			pFrm			= RpLightGetFrame ( m_pLight );
	AgcdEffLight*		pEffBase_Light	= static_cast<AgcdEffLight*>(m_lpEffBase);
	if( !pEffBase_Light )
		return tReturnErr();

	if( DEF_FLAG_CHK( m_dwUpdateFlag, E_UPDATEFLAG_COLR ) )
	{
		const RwReal invs_255	= 1.f/255.f;
		RwRGBAReal		frgba = { m_rgba.red	* invs_255
								, m_rgba.green	* invs_255
								, m_rgba.blue	* invs_255
								, m_rgba.alpha	* invs_255 };

		RpLightSetColor( m_pLight, &frgba );
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_MFrm
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_MFrm::AgcdEffCtrl_MFrm( AgcdEffCtrl_Set* lpEffCtrl_Set
								  , AgcdEffBase* lpEffBase
								  , RwUInt32 dwIndex
								  , RwFrame* pFrmChild)
: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
, m_pFrm( NULL )
{
	EFFMEMORYLOG_CON;
}

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_MFrm::~AgcdEffCtrl_MFrm()
{
	EFFMEMORYLOG_DES;

	ReleaseTargetFrm();
}

//-----------------------------------------------------------------------------
// bSetTargetFrm
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_MFrm::SetTargetFrm( RwFrame* pFrm )
{

	if( !m_pFrm && pFrm )
	{
		m_pFrm = RwFrameCreate();
		if( !m_pFrm )
		{
			Eff2Ut_ERR( "RwFrameCreate failed @ AgcdEffCtrl_MFrm::bSetTargetFrm" );
			return tReturnErr();
		}
		RwMatrixSetIdentity( &m_pFrm->modelling );

		RwFrame* pFrmParent = RwFrameGetParent( pFrm );
		if( pFrmParent )
		{
			RwFrameRemoveChild( pFrm );
			RwFrameAddChild( pFrmParent, m_pFrm );
		}

		RwFrameAddChild( m_pFrm, pFrm );
	}
	else
	{
		Eff2Ut_ERR( "NULL != m_pFrm || NULL == pFrm @ AgcdEffCtrl_MFrm::bSetTargetFrm" );
		return tReturnErr();
	}
	

	return 0;
}

//-----------------------------------------------------------------------------
// bReleaseTargetFrm
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_MFrm::ReleaseTargetFrm(void)
{
	if( !m_pFrm )
		return 0;

	RwFrame*	pFrmChild	= m_pFrm->child;

	if( pFrmChild )
	{
		RwFrame *curFrame = RwFrameGetParent ( pFrmChild );
		if( curFrame && curFrame->child )
			RwFrameRemoveChild( pFrmChild );
	}

	RwFrame*	pFrmParent	= RwFrameGetParent( m_pFrm );
	if( pFrmParent )
	{
		RwFrameRemoveChild( m_pFrm );

		if( pFrmChild )
			RwFrameAddChild( pFrmParent, pFrmChild );
	}

	RwFrameDestroy( m_pFrm );
	m_pFrm	= NULL;

	SetState( E_EFFCTRL_STATE_END );

	return 0;
}

//-----------------------------------------------------------------------------
// bUpdate
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_MFrm::Update(RwUInt32 dwAccumulateTime)
{
	PROFILE("AgcdEffCtrl_MFrm::bUpdate");

	ASSERT( m_lpEffBase );
	if( !m_lpEffBase )
	{
		Eff2Ut_ERR( "m_lpEffBase == NULL @ AgcdEffCtrl_MFrm::bUpdate" );
		return tReturnErr();
	}

	RwUInt32	dwCurrTime	= 0LU;
	RwInt32		ir			= tLifeCheck( &dwCurrTime, dwAccumulateTime );
	if( ir )
	{
		return ir;
	}
	if( GetState() == E_EFFCTRL_STATE_WAIT )
		return 0;
	
	if( !m_pFrm || !m_pFrm->child )	
		return 0;

	AgcdEffMFrm*	lpEffBase_MFrm
		= static_cast<AgcdEffMFrm*>( m_lpEffBase );
	if( lpEffBase_MFrm )
	{
		if( lpEffBase_MFrm->bGetMFrmType() == AgcdEffMFrm::EMFRM_SHAKE )
		{

			if( !GetCPtrEffCtrl_Set()->FlagChk( FLAG_EFFCTRLSET_MAINCHARAC) )
				return 0;

			if( T_ISMINUS4( ir = lpEffBase_MFrm->bShakeFrm( m_pFrm, dwCurrTime ) ) )
			{
				Eff2Ut_ERR( "lpEffBase_MFrm->bShakeFrm failed @ AgcdEffCtrl_MFrm::bUpdate" );
				return tReturnErr();
			}
			if(ir)
			{
				return ir;
			}
		}
		else if( lpEffBase_MFrm->bGetMFrmType() == AgcdEffMFrm::EMFRM_SPLINE )
		{
			ASSERT( lpEffBase_MFrm->bGetRefAnimList().size() == 1 );

			EFFBASE::LPEffAniVecItr	it_begin
				= lpEffBase_MFrm->bGetRefAnimList().begin();

			if( AgcdEffAnim::E_EFFANIM_RPSPLINE == (*it_begin)->bGetAnimType() )
			{
				AgcdEffAnim_RpSpline*	pEffAnim_Spline
					= static_cast<AgcdEffAnim_RpSpline*>( *it_begin );
				
				RwMatrix	mat;
				RwMatrixSetIdentity( &mat );
				RwInt32	ir	= 
				pEffAnim_Spline->bGetTVal( &mat, dwCurrTime );
				if( T_ISMINUS4( ir ) )
				{
					Eff2Ut_ERR( "pEffAnim_Spline->bGetTVal failed @ AgcdEffCtrl_MFrm::bUpdate" );
					return tReturnErr();
				}
				else if( ir )
				{
					return ir;
				}

				//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
				RwFrameTransform( m_pFrm, &mat, rwCOMBINEPOSTCONCAT );
				//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
			}
		}
		else
		{
			Eff2Ut_ERR( "lpEffBase_MFrm->bGetMFrmType() is unknown @ AgcdEffCtrl_MFrm::bUpdate" );
			return tReturnErr();
		}
	}
	else
	{
		Eff2Ut_ERR( "m_lpEffBase == NULL @ AgcdEffCtrl_MFrm::bUpdate" );
		return tReturnErr();
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_Sound
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Sound::AgcdEffCtrl_Sound( AgcdEffCtrl_Set* lpEffCtrl_Set
									, AgcdEffBase* lpEffBase
									, RwUInt32 dwIndex )
: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
, m_dwSoundIndex( 0LU )
, m_bPlayed( FALSE )
, m_dwPlayedTime( 0LU )
{
	EFFMEMORYLOG_CON;

	AgcdEffSound*	pEffBase_Sound
		= static_cast<AgcdEffSound*>(m_lpEffBase);
	if( pEffBase_Sound )
	{
		m_dwLoopCnt		= pEffBase_Sound->bGetLoopCnt();
		m_fVolume		= pEffBase_Sound->bGetVolume();
		m_eSoundType	= pEffBase_Sound->bGetSoundType();

		if( m_eSoundType == AgcdEffSound::EFFECT_SOUND_3DSOUND )
			lpEffCtrl_Set->SetEffCtrl3DSound( this );
		else if( pEffBase_Sound->bFlagChk( FLAG_EFFBASESOUND_NOFRUSTUMCHK ) )
			lpEffCtrl_Set->SetEffCtrlSoundNoFrustumChk( this );

	}
	else
	{
		tReturnErr();
	}
}

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_Sound::~AgcdEffCtrl_Sound()
{
	EFFMEMORYLOG_DES;

	if( !m_bPlayed )
	{
	}else
	if( DEF_FLAG_CHK( m_lpEffBase->bGetFlag(), FLAG_EFFBASESOUND_STOPATEFFECTEND ) )
	{
		AgcdEffSound*	pEffBase_Sound
			= static_cast<AgcdEffSound*>(m_lpEffBase);		
		if( pEffBase_Sound )
		{
			pEffBase_Sound->bStopSound( m_eSoundType, m_dwSoundIndex );
		}
		else
		{
			Eff2Ut_ERR("m_lpEffBase == NULL @ AgcdEffCtrl_Sound::~AgcdEffCtrl_Sound");
		}		
	}
}

//-----------------------------------------------------------------------------
// bStopSound
//-----------------------------------------------------------------------------
void AgcdEffCtrl_Sound::StopSound()
{
	AgcdEffSound*	pEffBase_Sound
		= static_cast<AgcdEffSound*>(m_lpEffBase);
	
	if( pEffBase_Sound )
	{
		pEffBase_Sound->bStopSound( m_eSoundType, m_dwSoundIndex );
		m_bPlayed = FALSE;
	}
	else
	{
		Eff2Ut_ERR("static_cast<AgcdEffSound*> failed @ AgcdEffCtrl_Sound::bStopSound");
	}
}
//-----------------------------------------------------------------------------
// bPlaySound
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Sound::PlaySound(RwUInt32 dwAccumulateTime)
{
	if( GetState() == E_EFFCTRL_STATE_END )
		return 0;

	AgcdEffSound*	lpEffBase_Sound
		= static_cast<AgcdEffSound*>(m_lpEffBase);
	if(!lpEffBase_Sound)
	{
		Eff2Ut_ERR( "m_lpEffBase==NULL @ AgcdEffCtrl_Sound::bPlaySound" );
		return tReturnErr();
	}
	if( !m_bPlayed )
	{
		INT32 prevIndx = m_dwSoundIndex;

		if( m_pEffCtrl_Set->GetPtrClumpParent() )
			m_dwSoundIndex	= lpEffBase_Sound->bPlaySound(m_eSoundType, m_fVolume, m_dwLoopCnt , TRUE , m_pEffCtrl_Set->GetPtrClumpParent() );
		else
			m_dwSoundIndex	= lpEffBase_Sound->bPlaySound( m_eSoundType , m_fVolume , m_dwLoopCnt , FALSE , m_pEffCtrl_Set->GetFrame() );

		if( !m_dwSoundIndex )
		{
			Eff2Ut_TOFILE( "AgcdEffSound::bPlaySound_Failed.txt"
				, Eff2Ut_FmtMsg("effsetid : %d\n"
				, m_pEffCtrl_Set->GetPtrEffSet()->bGetID()
				)
				);

			if( prevIndx )
				lpEffBase_Sound->bStopSound( m_eSoundType, prevIndx );

			return tReturnErr();
		}

		if( m_eSoundType == AgcdEffSound::EFFECT_SOUND_3DSOUND )
		{
			if( m_dwSoundIndex == AGCMSOUND_3D_SOUND_NO_SLOT ||
				m_dwSoundIndex == AGCMSOUND_3D_SOUND_OVER_RANGE )
			{
				if( m_pEffCtrl_Set->GetLoopOpt() == e_TblDir_infinity && m_dwLoopCnt == 0 )
				{
					m_dwPlayedTime = dwAccumulateTime;
					return 0;
				}
				else
				{
					lpEffBase_Sound->bStopSound( m_eSoundType, m_dwSoundIndex );

					return tReturnErr();
				}
			}
			
			lpEffBase_Sound->bPosUpdateFor3DSound( 
				m_dwSoundIndex
				, m_pEffCtrl_Set->GetPos()
				, 100 );
		}

		m_bPlayed	= TRUE;
		m_dwPlayedTime = dwAccumulateTime;

		if( lpEffBase_Sound->bFlagChk( FLAG_EFFBASESOUND_NOFRUSTUMCHK ) )
			m_pEffCtrl_Set->SetEffCtrlSoundNoFrustumChk( NULL );
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Sound::Update(RwUInt32 dwAccumulateTime)
{
	PROFILE("AgcdEffCtrl_Sound::bUpdate");

	ASSERT( "kday" && m_lpEffBase );

	RwUInt32	dwCurrTime	= 0LU;
	RwInt32		ir			= tLifeCheck( &dwCurrTime, dwAccumulateTime );
	if( ir )
	{
		return ir;
	}
	if( GetState() == E_EFFCTRL_STATE_WAIT )
		return 0;

	AgcdEffSound*	lpEffBase_Sound
		= static_cast<AgcdEffSound*>(m_lpEffBase);
	if(!lpEffBase_Sound)
	{
		Eff2Ut_ERR( "m_lpEffBase == NULL @ AgcdEffCtrl_Sound::bUpdate" );
		return tReturnErr();
	}
	if( !m_bPlayed )
	{
		PlaySound(m_pEffCtrl_Set->GetAccumulateTime());
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_Sound::Update3DSoundPos()
{
	ASSERT( "kday" && m_lpEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_SOUND );
	AgcdEffSound*	lpEffBase_Sound
		= static_cast<AgcdEffSound*>(m_lpEffBase);
	if(!lpEffBase_Sound)
	{
		Eff2Ut_ERR( "m_lpEffBase == NULL @ AgcdEffCtrl_Sound::bUpdate3DSoundPos" );
		return tReturnErr();
	}

	if( m_bPlayed )
	{
		if( m_eSoundType == AgcdEffSound::EFFECT_SOUND_3DSOUND )
		{
			/*
			if( m_pEffCtrl_Set->GetLoopOpt() == e_TblDir_infinity
				&& m_dwLoopCnt == 0
				)
			{
				if( AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->IsIn3DSoundRange( (RwV3d*)m_pEffCtrl_Set->GetPos() ) )
				{
					return 
					lpEffBase_Sound->bPosUpdateFor3DSound( 
					m_dwSoundIndex
					, m_pEffCtrl_Set->GetPos()
					, m_pEffCtrl_Set->FlagChk( FLAG_EFFCTRLSET_BEUPDATED ) ? 9 : 4 );
				}
				else
				{
					StopSound();
				}
			}
			else
			{
				return 
				lpEffBase_Sound->bPosUpdateFor3DSound( 
				m_dwSoundIndex
				, m_pEffCtrl_Set->GetPos()
				, m_pEffCtrl_Set->FlagChk( FLAG_EFFCTRLSET_BEUPDATED ) ? 9 : 4 );
			}
			*/
			return 
				lpEffBase_Sound->bPosUpdateFor3DSound( 
				m_dwSoundIndex
				, m_pEffCtrl_Set->GetPos()
				, m_pEffCtrl_Set->FlagChk( FLAG_EFFCTRLSET_BEUPDATED ) ? 9 : 4 );
		}
	}
	else if( m_pEffCtrl_Set->GetLoopOpt() == e_TblDir_infinity 
		&& m_dwLoopCnt == 0 
		&& lpEffBase_Sound->bGetSoundLength() <= m_pEffCtrl_Set->GetAccumulateTime() - m_dwPlayedTime )
	{
		PROFILE("bPlaySound @ AgcdEffCtrl_Sound::bUpdate3DSoundPos");
		PlaySound( m_pEffCtrl_Set->GetAccumulateTime() );
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// AgcdEffCtrl_PostFX
///////////////////////////////////////////////////////////////////////////////
AgcdEffCtrl_PostFX*	AgcdEffCtrl_PostFX::PROCESSEDPOSTFX	= NULL;
BOOL	AgcdEffCtrl_PostFX::OLDFXSTATE = FALSE;
BOOL	AgcdEffCtrl_PostFX::BONFX = FALSE;
char	AgcdEffCtrl_PostFX::PIPEBU[1024] = "";
//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------
AgcdEffCtrl_PostFX::AgcdEffCtrl_PostFX(AgcdEffCtrl_Set* lpEffCtrl_Set
									   , AgcdEffBase* lpEffBase
									   , RwUInt32 dwIndex)
: AgcdEffCtrl_Base(lpEffCtrl_Set, lpEffBase, dwIndex)
{
	EFFMEMORYLOG_CON;
	
}
//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
AgcdEffCtrl_PostFX::~AgcdEffCtrl_PostFX()
{
	EFFMEMORYLOG_DES;

#ifndef USE_MFC
	ASSERT( "kday" && PROCESSEDPOSTFX );
#endif //USE_MFC
	if( PROCESSEDPOSTFX == this )
	{
		SetState( E_EFFCTRL_STATE_END );
	}
}
//-----------------------------------------------------------------------------
// SetState
//-----------------------------------------------------------------------------
void AgcdEffCtrl_PostFX::SetState( E_EFFCTRL_STATE eState )
{
	if( eState == E_EFFCTRL_STATE_END && this == PROCESSEDPOSTFX )
	{
		PROCESSEDPOSTFX	= NULL;
		
		AgcmPostFX*	pAgcmPostFX = AgcdEffGlobal::bGetInst().bGetPtrAgcmPostFX();
		ASSERT( "kday" && pAgcmPostFX );

		BONFX = false;

		if(OLDFXSTATE)
		{
			pAgcmPostFX->setPipeline(PIPEBU, 1023);
		}
		else
		{
			pAgcmPostFX->Off();
		}
	}
	
	AgcdEffCtrl::SetState( eState );
};
//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------
RwInt32 AgcdEffCtrl_PostFX::Update(RwUInt32 dwAccumulateTime)
{
	ASSERT( "kday" && m_lpEffBase );
	ASSERT( "kday" && m_lpEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_POSTFX );

	if( !m_lpEffBase 
	 || m_lpEffBase->bGetBaseType() != AgcdEffBase::E_EFFBASE_POSTFX 
	 || !m_lpEffAnim_Scale )
		return tReturnErr();

	RwUInt32	dwCurrTime	= 0LU;
	RwInt32		ir			= tLifeCheck( &dwCurrTime, dwAccumulateTime );
	if( ir )
	{
		if( PROCESSEDPOSTFX == this )
			PROCESSEDPOSTFX	= NULL;

		return ir;
	}
	if( GetState() == E_EFFCTRL_STATE_WAIT )
		return 0;

	if( !PROCESSEDPOSTFX )
	{
		AgcmPostFX*	pAgcmPostFX = AgcdEffGlobal::bGetInst().bGetPtrAgcmPostFX();
		if( !pAgcmPostFX )
		{	//end
			return tReturnErr();
		}

		if(!BONFX)
		{
			if(OLDFXSTATE = pAgcmPostFX->isOn())
				pAgcmPostFX->getPipeline(PIPEBU, 1023);
			else
				PIPEBU[0] = '\0';
		}
		char buf[1024];
		strcpy(buf, PIPEBU);
		if(strlen(PIPEBU)>0) strcat(buf, "-");

		strcat(buf, "ClampingCircle");
		pAgcmPostFX->setPipeline(buf);
		pAgcmPostFX->On();
		BONFX = true;

		PROCESSEDPOSTFX	= this;

		AgcdEffPostFX*	pPostFX	= static_cast<AgcdEffPostFX*>(m_lpEffBase);
		RwV2d center = pPostFX->bGetSharedParam().center;
		pAgcmPostFX->setCenter( center.x, center.y );
		pAgcmPostFX->setPipeline( pPostFX->bGetTech() );

	}

	if( PROCESSEDPOSTFX != this 
	 || !m_lpEffAnim_Scale
	 )
		return 0;

	RwV3d theval;
	m_lpEffAnim_Scale->bGetTVal( &theval, dwCurrTime );
	
	switch( static_cast<AgcdEffPostFX*>(m_lpEffBase)->bGetSharedParam().type )
	{
	case e_fx_ClampingCircle:
	case e_fx_Wave			:
	case e_fx_Shockwave		:
	case e_fx_Darken		:
	case e_fx_Brighten		:
		{
			AgcdEffGlobal::bGetInst().bGetPtrAgcmPostFX()->setRadius( theval.x );
		}break;
	case e_fx_Ripple		:
		{
			// TODO : "kday" insert available code
		}break;
	case e_fx_Twist			:
		{
			AgcdEffGlobal::bGetInst().bGetPtrAgcmPostFX()->setRadius( theval.x );
		}break;
	default:
		{
			ASSERT( !"kday" && "unknown postfx type" );
			return tReturnErr();
		};
	}

	return 0;
};

AgcdEffCtrl_Camera::AgcdEffCtrl_Camera(AgcdEffCtrl_Set *lpEffCtrl_Set, AgcdEffBase *lpEffBase, DWORD dwIndex)
	: AgcdEffCtrl_Base( lpEffCtrl_Set, lpEffBase, dwIndex )
	,	m_pCamera( NULL )
{


}

AgcdEffCtrl_Camera::~AgcdEffCtrl_Camera( VOID )
{

}

VOID	AgcdEffCtrl_Camera::SetState( E_EFFCTRL_STATE eState )
{	
	if( GetCamera()	&&	eState	==	E_EFFCTRL_STATE_BEGIN )
	{
		RwFrame*	pFrame	=	RwCameraGetFrame( GetCamera() );
	}

	AgcdEffCtrl::SetState( eState );
}

RwInt32	AgcdEffCtrl_Camera::Update( RwUInt32 dwAccumulateTime )
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// AgcuEffBaseCtrlCreater
///////////////////////////////////////////////////////////////////////////////
AgcuEffBaseCtrlCreater::fptrCreater AgcuEffBaseCtrlCreater::fPtrCreater[]	= {

	_Create_Board		,//EFFBASE::E_EFFBASE_BOARD					= 0,
	_Create_PSyst		,//EFFBASE::E_EFFBASE_PSYS					,
	_Create_PSystSBH	,//EFFBASE::E_EFFBASE_PSYS_SIMPLEBLACKHOLE	,
	_Create_Tail		,//EFFBASE::E_EFFBASE_TAIL					,
	_Create_Object		,//EFFBASE::E_EFFBASE_OBJECT				,
	_Create_Light		,//EFFBASE::E_EFFBASE_LIGHT					,
	_Create_Sound		,//EFFBASE::E_EFFBASE_SOUND					,
	_Create_MFrm		,//EFFBASE::E_EFFBASE_MOVINGFRAME			,
	_Create_TerrainB	,//E_EFFBASE_TERRAINBOARD					,
	_Create_PostFX		,//E_EFFBASE_POSTFX							,
	_Create_Camera		,//E_EFFBASE_CAMERA							,

};

//-----------------------------------------------------------------------------
// vCreate_Board
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_Board(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_Board" );
	ASSERT( lpEffBase );
	ASSERT( lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_BOARD );

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_Board( lpEffCtrl_Set, lpEffBase, dwIndex, pFrmParent) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_PSyst
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_PSyst(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_PSyst" );
	ASSERT( lpEffBase );
	ASSERT( lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_PSYS );

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_ParticleSysTem( lpEffCtrl_Set, lpEffBase, dwIndex, pFrmParent) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_PSystSBH
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_PSystSBH(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_PSystSBH" );
	ASSERT( lpEffBase );
	ASSERT( lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_PSYS_SIMPLEBLACKHOLE );

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_ParticleSyst_SBH( lpEffCtrl_Set, lpEffBase, dwIndex, pFrmParent) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_Tail
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_Tail(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_Tail" );
	ASSERT( lpEffBase );
	ASSERT( lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_TAIL );

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_Tail( lpEffCtrl_Set, lpEffBase, dwIndex, pFrmParent) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_Object
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_Object(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_Object" );
	ASSERT( lpEffBase );
	ASSERT( lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_OBJECT );

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_Obj( lpEffCtrl_Set, lpEffBase, dwIndex, pFrmParent) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_Light
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_Light(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_Light" );
	ASSERT( lpEffBase );
	ASSERT( lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_LIGHT );

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_Light( lpEffCtrl_Set, lpEffBase, dwIndex, pFrmParent ) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_Sound
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_Sound(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_Sound" );
	ASSERT( lpEffBase );
	ASSERT( lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_SOUND );

	pFrmParent;

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_Sound( lpEffCtrl_Set, lpEffBase, dwIndex ) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_MFrm
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_MFrm(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmTarget)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_MFrm" );
	ASSERT( lpEffBase );
	ASSERT( lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_MOVINGFRAME );

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_MFrm( lpEffCtrl_Set, lpEffBase, dwIndex, pFrmTarget) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_MFrm
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_TerrainB(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_TerrainB" );
	ASSERT( lpEffBase );
	ASSERT( lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_TERRAINBOARD );

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_TerrainBoard( lpEffCtrl_Set, lpEffBase, dwIndex, pFrmParent) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_PostFX
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_PostFX(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_PostFX" );
	ASSERT( "kday" && lpEffBase );
	ASSERT( "kday" && lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_POSTFX );

	pFrmParent;

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_PostFX( lpEffCtrl_Set, lpEffBase, dwIndex) );

	return pRet;
}

//-----------------------------------------------------------------------------
// vCreate_Camera
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::_Create_Camera(AgcdEffCtrl_Set* lpEffCtrl_Set
													  , AgcdEffBase* lpEffBase
													  , RwUInt32 dwIndex
													  , RwFrame* pFrmParent)
{
	PROFILE( "AgcuEffBaseCtrlCreater::vCreate_Camera" );
	ASSERT( "kday" && lpEffBase );
	ASSERT( "kday" && lpEffBase->bGetBaseType() == EFFBASE::E_EFFBASE_CAMERA );

	pFrmParent;

	LPEFFCTRL_BASE pRet	= 
		static_cast<LPEFFCTRL_BASE>
		( new AgcdEffCtrl_Camera( lpEffCtrl_Set, lpEffBase, dwIndex) );

	return pRet;
}

//-----------------------------------------------------------------------------
// bCreate
//-----------------------------------------------------------------------------
LPEFFCTRL_BASE AgcuEffBaseCtrlCreater::CreateCtrl(AgcdEffCtrl_Set* lpEffCtrl_Set
												   , AgcdEffBase* lpEffBase
												   , RwUInt32 dwIndex
												   , RwFrame* pFrmParent)
{
	ASSERT( lpEffBase );
	if( !lpEffBase )
	{
		Eff2Ut_ERR("lpEffBase == NULL @ AgcuEffBaseCtrlCreater::bCreate");
		return NULL;
	}
	
	LPEFFCTRL_BASE pBase = 
	AgcuEffBaseCtrlCreater::fPtrCreater[ lpEffBase->bGetBaseType() ]( lpEffCtrl_Set
																    , lpEffBase
																	, dwIndex
																	, pFrmParent );

	//. 2006. 3. 9. Nonstopdj
	//. if vLoadClump, RpClumpClone, was return faild.
	if(pBase && pBase->FlagChk(E_EFFCTRL_STATE_END))
	{
		MD_SetErrorMessage("AgcuEffBaseCtrlCreater::bCreate() RpClump Load/clone Failed");
		return NULL;
	}

	return pBase;
}

//-----------------------------------------------------------------------------
// bDestroy
//-----------------------------------------------------------------------------
void AgcuEffBaseCtrlCreater::DestroyCtrl( LPEFFCTRL_BASE& lpEffCtrl_Base )
{
	DEF_SAFEDELETE( lpEffCtrl_Base );
}

//@{ 2006/11/17 burumal
#define fAPPEARANCE_DISTANCE_CORRECTION_VALUE_FOR_EFFECT	(0.0f)

#ifdef _DEBUG
FLOAT g_fEffectDistCorrectionValue = fAPPEARANCE_DISTANCE_CORRECTION_VALUE_FOR_EFFECT;
#endif

BOOL	AgcdEffCtrl_Set::CB_EFFECT_DISTCORRECT( PVOID pDistFloat, PVOID pNull1, PVOID pNull2 )
{	
	pNull1;
	pNull2;

	if ( pDistFloat == NULL )
		return FALSE;

#ifdef _DEBUG
	*((FLOAT*) pDistFloat) = *((FLOAT*) pDistFloat) + g_fEffectDistCorrectionValue;
#else
	*((FLOAT*) pDistFloat) = *((FLOAT*) pDistFloat) + fAPPEARANCE_DISTANCE_CORRECTION_VALUE_FOR_EFFECT;
#endif
	// -값이 되는것은 상관없다

	return TRUE;
}
//@}
