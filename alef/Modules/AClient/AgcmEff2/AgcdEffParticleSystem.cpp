// AgcdEffParticleSystem.cpp: implementation of the AgcdEffParticleSystem class.
//
///////////////////////////////////////////////////////////////////////////////
#include "AgcdEffParticleSystem.h"
#include "AgcdEffCtrl.h"
#include "AgcuEffTable.h"
#include "AgcuBillBoard.h"
#include "AgcdEffAnim.h"
#include "AgcdEffGlobal.h"

#include "ApMemoryTracker.h"

#include "AcuSinTbl.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

#include "AgcdGeoTriAreaTbl.h"

EFFMEMORYLOG(StParticle);
EFFMEMORYLOG(AgcdEffParticleSystem);
EFFMEMORYLOG(AgcdEffParticleSys_SimpleBlackHole);

STENVRNPARAM	g_EnvrnParam;

RwInt32 StParticle::CNT = 0;
RwInt32 StParticle::MAX_CNT = 0;

StParticle::StParticle( const RwV3d&				v3dInitPos
			 , const RwV3d&				v3dInitVel
			 , const RwV3d&				v3dScale
			 ,		 RwRGBA				rgba
			 , const STUVRECT&			stUVRect
			 ,		 RwReal				fOmega
			 ,		 RwUInt32			dwLife
			 ,		 RwUInt32			dwStartTime
			 , const stCofEnvrnParam&	stCofEnvrn )
: m_stUV( stUVRect )
, m_fOmega( fOmega )
, m_dwLife( dwLife )
, m_dwStartTime( dwStartTime )
, m_stCofEnvrn( stCofEnvrn )
{
	EFFMEMORYLOG_CON;

	++CNT;
	MAX_CNT = T_MAX(MAX_CNT, CNT);

	m_colr				= rgba;
	m_v3dInitPos		= v3dInitPos;
	m_v3dInitVelocity	= v3dInitVel;
	m_v3dScale			= v3dScale;
}

StParticle::~StParticle()
{
	EFFMEMORYLOG_DES;
	--CNT;
}

RwInt32	AgcdEffParticleSystem::EMITER::bGetAngle( RwReal* pfYaw, RwReal* pfPitch, RwReal fPastTime )const
{
	//yaw
	if( m_fMaxYaw == m_fMinYaw )
		*pfYaw = m_fMinYaw;
	else
	{
		*pfYaw = m_fOmegaYaw_World * fPastTime;
		NSAcuMath::T_CLAMP( *pfYaw, m_fMinYaw, m_fMaxYaw );
	}

	//pitch
	if( m_fMaxPitch == m_fMinPitch )
		*pfPitch = m_fMinPitch;
	else
	{
		*pfPitch = m_fOmegaPitch_Local * fPastTime;
		NSAcuMath::T_CLAMP( *pfPitch, m_fMinPitch, m_fMaxPitch );
	}

	return 0;
}

VOID AgcdEffParticleSystem::EMITER::bUpdateSideVector(void)
{

	if( m_vDir.x == 0.f &&
		m_vDir.z == 0.f )
	{
		if( m_vDir.y == 0.f )
			return;

		RwV3dNormalize( &m_vDir, &m_vDir );

		m_vSide = *PTRAXISWZ;
	}
	else
	{
		RwV3dCrossProduct ( &m_vSide, PTRAXISWY, &m_vDir );
		
		RwV3dNormalize( &m_vDir, &m_vDir );
		RwV3dNormalize( &m_vSide, &m_vSide );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////
AgcdEffParticleSystem::AgcdEffParticleSystem(AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType)
: AgcdEffRenderBase( AgcdEffBase::E_EFFBASE_PSYS, eBlendType )
, m_nCapacity(0)
, m_dwShootDelay(0LU)
, m_pClump(NULL) 
{
	EFFMEMORYLOG_CON;

	Eff2Ut_ZEROBLOCK( m_szClumpFName );
	Eff2Ut_ZEROBLOCK( m_stEmiter );
	Eff2Ut_ZEROBLOCK( m_stParticleProp );
}

AgcdEffParticleSystem::~AgcdEffParticleSystem()
{
	EFFMEMORYLOG_DES;
}

///////////////////////////////////////////////////////////////////////////////
// bShootPaticle
// 안보이면 파티클 생성 안한다.
// dwAccumulateTime : AgcdEffCtrl_Set::m_stTimeLoop.m_dwCurrTime
//					, 이펙트 셋 생성후로 누적된 시간을 받는다.
// err : -1, good : 0
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffParticleSystem::bShootPaticle
( RwUInt32 dwAccumulateTime, AgcdEffCtrl_ParticleSysTem* pEffCtrl_PSyst )const
{
	RwUInt32	capacity	= m_nCapacity;
	RwUInt32	shootdelay	= m_dwShootDelay ? m_dwShootDelay : 30;
	if( pEffCtrl_PSyst->GetCPtrEffCtrl_Set()->GetParticleNumScale() != 1.f )
	{
		ASSERT(pEffCtrl_PSyst->GetCPtrEffCtrl_Set()->GetParticleNumScale() > 0.f);
		capacity	= (RwUInt32)( static_cast<RwReal>(m_nCapacity)*pEffCtrl_PSyst->GetCPtrEffCtrl_Set()->GetParticleNumScale() );
		shootdelay	= (RwUInt32)( static_cast<RwReal>(m_dwShootDelay)/pEffCtrl_PSyst->GetCPtrEffCtrl_Set()->GetParticleNumScale() );
		shootdelay	= MAX(shootdelay, 30LU);
	}

	if( dwAccumulateTime < pEffCtrl_PSyst->m_dwLastShootTime )
		pEffCtrl_PSyst->m_dwLastShootTime = dwAccumulateTime;

	RwUInt32 dwPastTime = dwAccumulateTime - pEffCtrl_PSyst->m_dwLastShootTime;
	if(  (pEffCtrl_PSyst->GetState() == E_EFFCTRL_STATE_CONTINUE)//연장시 추가 파티클생성안함
	  || (DEF_FLAG_CHK( bGetFlag(), FLAG_EFFBASEPSYS_CAPACITYLIMIT) && (pEffCtrl_PSyst->m_listLpParticle.size() >= capacity))//갯수제한플래그
	  || (!dwPastTime)//넘짧은시간.
	  || (	 (m_eLoopOpt == e_TblDir_none) 
		  && (dwAccumulateTime - m_dwDelay + (m_stParticleProp.m_dwParticleLife + m_stParticleProp.m_dwParticleLifeOffset) > m_dwLife)
		 )
	  )
	  return 0;
	
	if( shootdelay < dwPastTime )
	{
		dwPastTime = shootdelay;
		pEffCtrl_PSyst->m_dwLastShootTime	= dwAccumulateTime - dwPastTime;
	}

	RwReal		fPastTime		= static_cast<RwReal>(dwPastTime) * 0.001f;

	LPPARTICLE	pNewParticle	= NULL;

	RwReal		fYaw			= 0.f;
	RwReal		fPitch			= 0.f;
	RwV3d		v3dDir			= { 0.f, 0.f, 0.f };
	RwInt32		nOneShootNum	= 0;
	RwV3d		v3dParticlePos	= { 0.f, 0.f, 0.f };
	RwV3d		v3dEmiterPos	= { 0.f, 0.f, 0.f };
	RwV3d		v3dGunOffset	= { 0.f, 0.f, 0.f };
	//RwV3d		vTemp;

	while( dwPastTime >= shootdelay )
	{
		v3dEmiterPos.x =
		v3dEmiterPos.y =
		v3dEmiterPos.z = 0.f;
		dwPastTime	-= shootdelay;
		pEffCtrl_PSyst->m_dwLastShootTime	+= shootdelay;
		fPastTime	= static_cast<RwReal>(pEffCtrl_PSyst->m_dwLastShootTime)*0.001f;

		if( T_ISMINUS4( vEmiterFrmUpdate( pEffCtrl_PSyst->m_dwLastShootTime - m_dwDelay, pEffCtrl_PSyst ) ) )
			return pEffCtrl_PSyst->tReturnErr();

		m_stEmiter.bGetAngle( &fYaw, &fPitch, fPastTime );

		vEmiterGetDir( &v3dDir, pEffCtrl_PSyst->m_pFrm, fYaw, fPitch, pEffCtrl_PSyst->GetPtrEffCtrl_Set()->GetRotation() );

		RwV3dNormalize( &v3dDir, &v3dDir );

		if( !DEF_FLAG_CHK( bGetFlag(), FLAG_EFFBASEPSYS_CIRCLEEMITER ) )
		{
			RwReal		fGunLength = m_stEmiter.m_fGunLength
					+ ( Eff2Ut_GETRNDFLOAT1(dwAccumulateTime)*2.f - 1.f ) * m_stEmiter.m_fGunLengthOffset;
			RwV3dScale( &v3dGunOffset, &v3dDir, fGunLength );
		}

		if( pEffCtrl_PSyst->GetPtrEffCtrl_Set()->FlagChk(FLAG_EFFCTRLSET_CLUMPEMITER) )
		{
			PROFILE( "CLUMPEMITER" );
			ASSERT( pEffCtrl_PSyst->GetPtrEffCtrl_Set()->GetPtrClumpEmiter() );
			RpClump* clump = pEffCtrl_PSyst->GetPtrEffCtrl_Set()->GetPtrClumpEmiter();

			RwReal	pos = RpClumpGetSurfaceArea(clump);
			if( pos >= 0.f )
			{			
				RwReal randpos = Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime());
				NSAcuMath::T_CLAMP( randpos, 0.f, 1.f );
				pos *= randpos;
				
				if( T_ISMINUS4(RpClumpGet3DPos( &v3dEmiterPos, clump, pos )) )
					return pEffCtrl_PSyst->tReturnErr();

				if( !bFlagChk( FLAG_EFFBASEPSYS_CHILDDEPENDANCY ) )
				{
					RwV3dTransformVector( &v3dEmiterPos
										, &v3dEmiterPos
										, RwFrameGetLTM( RpClumpGetFrame (clump) )
										);
				}
			}
		}
		else if( pEffCtrl_PSyst->GetPtrEffCtrl_Set()->FlagChk(FLAG_EFFCTRLSET_ATOMICEMITER) )
		{
			PROFILE( "ATOMICEMITER" );
			ASSERT( pEffCtrl_PSyst->GetPtrEffCtrl_Set()->GetPtrAtomicEmiter() );
			RpAtomic*	acomic = pEffCtrl_PSyst->GetPtrEffCtrl_Set()->GetPtrAtomicEmiter();
			RwReal	pos = RpAtomicGetSurfaceArea(acomic);//Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime());
			if( pos >= 0.f )
			{
				RwReal randpos = Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime());
				NSAcuMath::T_CLAMP( randpos, 0.f, 1.f );
				pos *= randpos;
				if( T_ISMINUS4(RpAtomicGet3DPos( &v3dEmiterPos, acomic, pos )) )
					return pEffCtrl_PSyst->tReturnErr();
				
				if( !bFlagChk( FLAG_EFFBASEPSYS_CHILDDEPENDANCY ) )
				{
					RwV3dTransformVector( &v3dEmiterPos
										, &v3dEmiterPos
										, RwFrameGetLTM( RpAtomicGetFrame (acomic) )
										);
				}
			}
		}

		if( !bFlagChk( FLAG_EFFBASEPSYS_CHILDDEPENDANCY ) )
		{
			//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
			RwV3dAdd( &v3dEmiterPos, &v3dEmiterPos, RwMatrixGetPos(RwFrameGetLTM(pEffCtrl_PSyst->m_pFrm))); 
			//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
		}

		RwV3dAdd( &v3dParticlePos, &v3dEmiterPos, &v3dGunOffset );

		
		nOneShootNum	= m_stEmiter.m_nNumOfOneShoot
						+ static_cast<RwInt32>
						  ( Eff2Ut_GETRNDFLOAT1(dwAccumulateTime) * m_stEmiter.m_nNumOfOneShootOffset );

		//기존 이펙트모듈에서의 반지름효과
		vShakeEmiter_forOldPSyst( &v3dParticlePos );
	

		if( T_ISMINUS4( vShootGroup( v3dParticlePos, v3dDir, nOneShootNum, pEffCtrl_PSyst->m_dwLastShootTime, pEffCtrl_PSyst ) ) )
			return pEffCtrl_PSyst->tReturnErr();
		
		if(  DEF_FLAG_CHK(bGetFlag(), FLAG_EFFBASEPSYS_CAPACITYLIMIT)
		  && pEffCtrl_PSyst->m_listLpParticle.size() >= capacity
		  )
			return 0;
	}

	return 0;
}

RwInt32 AgcdEffParticleSystem::vEmiterFrmUpdate(RwUInt32 dwTheTime, AgcdEffCtrl_ParticleSysTem* pEffCtrl_PSyst )const
{
	pEffCtrl_PSyst->tPreUpdateFrm();

	INT	ir	= 0;
	if( pEffCtrl_PSyst->m_lpEffAnim_Missile )
	{		
		ir = 
		pEffCtrl_PSyst->m_lpEffAnim_Missile->bUpdateVal( dwTheTime, pEffCtrl_PSyst );

		ASSERT( !T_ISMINUS4(ir) );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "pEffCtrl_PSyst->m_lpEffAnim_Missile->bUpdateVal failed @ AgcdEffParticleSystem::vEmiterUpdate" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - base index : %d", pEffCtrl_PSyst->m_dwID) );
			return pEffCtrl_PSyst->tReturnErr();
		}
	}
	
	if( pEffCtrl_PSyst->m_lpEffAnim_Linear )
	{		
		ir = 
		pEffCtrl_PSyst->m_lpEffAnim_Linear->bUpdateVal( dwTheTime, pEffCtrl_PSyst );

		ASSERT( !T_ISMINUS4(ir) );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "pEffCtrl_PSyst->m_lpEffAnim_Linear->bUpdateVal failed @ AgcdEffParticleSystem::vEmiterUpdate" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - base index : %d", pEffCtrl_PSyst->m_dwID) );
			return pEffCtrl_PSyst->tReturnErr();
		}
	}
	
	if( pEffCtrl_PSyst->m_lpEffAnim_Rev )
	{		
		ir = 
		pEffCtrl_PSyst->m_lpEffAnim_Rev->bUpdateVal( dwTheTime, pEffCtrl_PSyst );

		ASSERT( !T_ISMINUS4(ir) );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "pEffCtrl_PSyst->m_lpEffAnim_Rev->bUpdateVal failed @ AgcdEffParticleSystem::vEmiterUpdate" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - base index : %d", pEffCtrl_PSyst->m_dwID) );
			return pEffCtrl_PSyst->tReturnErr();
		}
	}
	
	if( !DEF_FLAG_CHK(m_stEmiter.m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_NODEPND) )
	{
		if( pEffCtrl_PSyst->m_lpEffAnim_Rot )
		{		
			ir = 
			pEffCtrl_PSyst->m_lpEffAnim_Rot->bUpdateVal( dwTheTime, pEffCtrl_PSyst );

			ASSERT( !T_ISMINUS4(ir) );
			if( T_ISMINUS4(ir) )
			{
				Eff2Ut_ERR( "pEffCtrl_PSyst->m_lpEffAnim_Rot->bUpdateVal failed @ AgcdEffParticleSystem::vEmiterUpdate" );
				Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - base index : %d", pEffCtrl_PSyst->m_dwID) );
				return pEffCtrl_PSyst->tReturnErr();
			}
		}
	}
	
	if( pEffCtrl_PSyst->m_lpEffAnim_RpSpline )
	{		
		ir = 
		pEffCtrl_PSyst->m_lpEffAnim_RpSpline->bUpdateVal( dwTheTime, pEffCtrl_PSyst );

		ASSERT( !T_ISMINUS4(ir) );
		if( T_ISMINUS4(ir) )
		{
			Eff2Ut_ERR( "pEffCtrl_PSyst->m_lpEffAnim_RpSpline->bUpdateVal failed @ AgcdEffParticleSystem::vEmiterUpdate" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - base index : %d", pEffCtrl_PSyst->m_dwID) );
			return pEffCtrl_PSyst->tReturnErr();
		}
	}
                                      
	pEffCtrl_PSyst->PostUpdateFrm(dwTheTime);

	return 0;
}

RwInt32 AgcdEffParticleSystem::vEmiterGetDir(RwV3d* pDir, RwFrame* pFrmEmiter, RwReal fYaw, RwReal fPitch, RtQuat *pQuat)const
{
	ASSERT( pDir && pFrmEmiter );

	if( DEF_FLAG_CHK(m_stEmiter.m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_SHOOTNOMOVE) )
	{
		pDir->x = 
		pDir->y = 
		pDir->z = 0.f;
	}
	else if( DEF_FLAG_CHK(m_stEmiter.m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_SHOOTRAND) )
	{		
		RwReal Yaw	= Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*360.f;
		RwReal Pitch= Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*360.f;
		pDir->y		= AcuSinTbl::Sin( Pitch );
		pDir->x		= AcuSinTbl::Cos( Pitch )*AcuSinTbl::Cos( Yaw );
		pDir->z		= AcuSinTbl::Cos( Pitch )*AcuSinTbl::Sin( Yaw );

		RwV3dNormalize ( pDir, pDir );
	}
	else if( DEF_FLAG_CHK(m_stEmiter.m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_SHOOTCONE) )
	{
		RwReal	fPitchDist	= m_stEmiter.m_fMaxPitch - m_stEmiter.m_fMinPitch;
		RwReal Yaw	= Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*360.f;
		RwReal Pitch= (Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*2.f-1.f)*m_stEmiter.m_fConAngle;
		pDir->y		= AcuSinTbl::Cos( Pitch );
		pDir->x		= AcuSinTbl::Sin( Pitch )*AcuSinTbl::Cos( Yaw );
		pDir->z		= AcuSinTbl::Sin( Pitch )*AcuSinTbl::Sin( Yaw );

		RwMatrix	matTemp;
		RwMatrix	matRet;
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
		Eff2Ut_RwMatrixGetNoScale( *RwFrameGetLTM(pFrmEmiter), &matTemp );

		//. 2006. 6. 7. nonstopdj
		if(pQuat)
		{
			RwMatrix matRot;
			RtQuatConvertToMatrix( pQuat, &matRot);
			RwMatrixMultiply(&matRet, &matTemp, &matRot);
		}
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
		RwV3d		v3dDir;
		RwV3dTransformVector ( &v3dDir, &m_stEmiter.m_vDir, &matRet );

		RwV3d	WYToDir	= { v3dDir.x-PTRAXISWY->x
			, v3dDir.y-PTRAXISWY->y
			, v3dDir.z-PTRAXISWY->z };
		RwV3dAdd( pDir, pDir, &WYToDir );

		RwV3dNormalize ( pDir, pDir );

	}
	else
	{
		RwMatrix	mat;
		RwMatrix	matRet;
		RwMatrixRotate ( &mat, PTRAXISWY, fYaw, rwCOMBINEREPLACE );
		RwV3d		v3dSide;
		RwV3dTransformVector(&v3dSide, &m_stEmiter.m_vSide, &mat);
		RwMatrixRotate ( &mat, &v3dSide, fPitch, rwCOMBINEPOSTCONCAT );
		if( !DEF_FLAG_CHK(m_stEmiter.m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_NODEPND)
		 &&	!bFlagChk(FLAG_EFFBASEPSYS_CHILDDEPENDANCY) )
		{
			//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
			//RwMatrixTransform ( &mat, RwFrameGetLTM(pFrmEmiter), rwCOMBINEPOSTCONCAT );
			RwMatrix	matTemp;
			Eff2Ut_RwMatrixGetRot( *RwFrameGetLTM(pFrmEmiter), &matTemp );
			Eff2Ut_RwMatrixGetNoScale( matTemp, &matTemp );

			RwMatrixTransform( &mat, &matTemp, rwCOMBINEPRECONCAT );
			//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();

			//. 2006. 6. 7. nonstopdj
			if(pQuat)
			{
				RwMatrix matRot;
				RtQuatConvertToMatrix( pQuat, &matRot);
				RwMatrixMultiply(&matRet, &mat, &matRot);
			}
		}
		RwV3dTransformVector( pDir, &m_stEmiter.m_vDir, &matRet );
	}

	return 0;
}

//---------------------------------------------------------------------------->
// util func
VOID AgcdEffParticleSystem::vCalcOffset_Point(RwV3d* pV3dOffset) const
{
	pV3dOffset->x = 
	pV3dOffset->y = 
	pV3dOffset->z = 0.f;
}

VOID AgcdEffParticleSystem::vCalcOffset_Box(RwV3d* pV3dOffset) const
{
	pV3dOffset->x
		= (Eff2Ut_GETRNDFLOAT1( AgcdEffGlobal::bGetInst().bGetCurrTime() )*2.f -1) * m_stEmiter.m_pgroupBox.m_fHWidth;
	pV3dOffset->y
		= (Eff2Ut_GETRNDFLOAT1( AgcdEffGlobal::bGetInst().bGetCurrTime() )*2.f -1) * m_stEmiter.m_pgroupBox.m_fHHeight;
	pV3dOffset->z
		= (Eff2Ut_GETRNDFLOAT1( AgcdEffGlobal::bGetInst().bGetCurrTime() )*2.f -1) * m_stEmiter.m_pgroupBox.m_fHDepth;
}

VOID AgcdEffParticleSystem::vCalcOffset_Cylinder(RwV3d* pV3dOffset) const
{
	pV3dOffset->x
		= AcuSinTbl::Cos(Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*360.f)
		* m_stEmiter.m_pgroupCylinder.m_fRadius;
	pV3dOffset->y
		= (Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*2.f - 1.f) * m_stEmiter.m_pgroupCylinder.m_fHHeight;
	pV3dOffset->z
		= AcuSinTbl::Sin(Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*360.f) 
		* m_stEmiter.m_pgroupCylinder.m_fRadius;
}

VOID AgcdEffParticleSystem::vCalcOffset_Sphere(RwV3d* pV3dOffset) const
{
	RwReal	Alpha	= Eff2Ut_GETRNDFLOAT1(Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*360.f);
	RwReal	Beta	= Eff2Ut_GETRNDFLOAT1(Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*360.f);

	pV3dOffset->x	= AcuSinTbl::Cos(Alpha) * AcuSinTbl::Cos(Beta) * m_stEmiter.m_pgroupSphere.m_fRadius;
	pV3dOffset->y	= AcuSinTbl::Sin(Alpha) * m_stEmiter.m_pgroupSphere.m_fRadius;
	pV3dOffset->z	= AcuSinTbl::Cos(Alpha) * AcuSinTbl::Sin(Beta) * m_stEmiter.m_pgroupSphere.m_fRadius;
}

VOID AgcdEffParticleSystem::vShakeEmiter_forOldPSyst( RwV3d* pV3dPos )const
{	
	if( DEF_FLAG_CHK( bGetFlag(), FLAG_EFFBASEPSYS_CIRCLEEMITER ) )
	{
		RwReal Angle	= Eff2Ut_GETRNDFLOAT1(Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*360.f);
		if( DEF_FLAG_CHK( bGetFlag(), FLAG_EFFBASEPSYS_FILLCIRCLE ) )
		{
			RwReal	fRadius	= Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*m_stEmiter.m_fGunLength;

			pV3dPos->x	+= fRadius * AcuSinTbl::Cos(Angle);
			pV3dPos->z	+= fRadius * AcuSinTbl::Sin(Angle);
		}
		else
		{
			pV3dPos->x	+= m_stEmiter.m_fGunLength * AcuSinTbl::Cos(Angle);
			pV3dPos->z	+= m_stEmiter.m_fGunLength * AcuSinTbl::Sin(Angle);
		}
	}
}

//<----------------------------------------------------------------------------
RwInt32 AgcdEffParticleSystem::vShootGroup(RwV3d& v3dCenter
										, RwV3d& v3dDir
										, INT nNum
										, RwUInt32 dwShootTime
										, AgcdEffCtrl_ParticleSysTem* pEffCtrl_PSyst )const
{
	PSYS_fptrCalcOffset	fptr	= NULL;
	switch( m_stEmiter.m_ePGroup )
	{
	case ePGroup_Point:		fptr = &AgcdEffParticleSystem::vCalcOffset_Point;		break;
	case ePGroup_Box:		fptr = &AgcdEffParticleSystem::vCalcOffset_Box;			break;
	case ePGroup_Cylinder:	fptr = &AgcdEffParticleSystem::vCalcOffset_Cylinder;	break;
	case ePGroup_Sphere:	fptr = &AgcdEffParticleSystem::vCalcOffset_Sphere;		break;
	default	:
		return pEffCtrl_PSyst->tReturnErr();
	}

	LPPARTICLE	pNewParticle	= NULL;
	RwV3d		v3dScale		= { 1.f, 1.f, 1.f };
	RwV3d		v3dNewPos		= { 0.f, 0.f, 0.f };
	RwV3d		v3dOffset		= { 0.f, 0.f, 0.f };
	RwRGBA		rgba			= { 255,255,255,255 };
	RwUInt32	dwLife			= 0LU;
	RwReal		fOmega			= 0.f;

	stCofEnvrnParam stCofEnvrn;
	STUVRECT		stUVRect;

	RwReal		fPower	= m_stEmiter.m_fPower 
						+ (Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*2.f - 1.f) * m_stEmiter.m_fPowerOffset;

	//RwV3dNormalize( &v3dDir, &v3dDir );

	RwV3d		v3dVel		    = { 0.f, 0.f, 0.f };
	RwV3dScale( &v3dVel, &v3dDir, fPower );

	int	beforemax	= StParticle::MAX_CNT;

	RwUInt32	capacity	= m_nCapacity;
	RwUInt32	shootdelay	= m_dwDelay;
	if( pEffCtrl_PSyst->GetCPtrEffCtrl_Set()->GetParticleNumScale() != 1.f )
	{
		ASSERT(pEffCtrl_PSyst->GetCPtrEffCtrl_Set()->GetParticleNumScale() > 0.f);
		capacity	= floatToInt( static_cast<RwReal>(m_nCapacity)*pEffCtrl_PSyst->GetCPtrEffCtrl_Set()->GetParticleNumScale() );
	}

	for( int i=0; i<nNum; ++i )
	{

		if( DEF_FLAG_CHK( bGetFlag(), FLAG_EFFBASEPSYS_CAPACITYLIMIT ) )
		if( pEffCtrl_PSyst->m_listLpParticle.size() >= capacity )
			return 0;
		//pos
		(this->*fptr)( &v3dOffset );

		// Monster is the direction specified by the check
		// Direction of the Target
		if( DEF_FLAG_CHK( bGetFlag(), FLAG_EFFBASEPSYS_TARGETDIR) )
		{
			RwMatrix	Mat;
			RwMatrixSetIdentity( &Mat );

			if( pEffCtrl_PSyst )
			{
				RwV3dNormalize( &pEffCtrl_PSyst->m_vBaseDir , &pEffCtrl_PSyst->m_vBaseDir );

				if( RwV3dLength( &pEffCtrl_PSyst->m_vBaseDir ) )
				{
					Mat.at =	pEffCtrl_PSyst->m_vBaseDir;
				}

				RwV3dTransformPoint( &v3dOffset , &v3dOffset , &Mat );
			}
		}

		v3dNewPos.x	= v3dCenter.x + v3dOffset.x;
		v3dNewPos.y	= v3dCenter.y + v3dOffset.y;
		v3dNewPos.z	= v3dCenter.z + v3dOffset.z;
		//rgba
		if( DEF_FLAG_CHK( m_dwBitFlags , FLAG_EFFBASEPSYS_RANDCOLR ) )
		{
			rgba.red	= Eff2Ut_GETRNDRGB(AgcdEffGlobal::bGetInst().bGetCurrTime());
			rgba.green	= Eff2Ut_GETRNDRGB(AgcdEffGlobal::bGetInst().bGetCurrTime());
			rgba.blue	= Eff2Ut_GETRNDRGB(AgcdEffGlobal::bGetInst().bGetCurrTime());
			rgba.alpha	= 255;
		}

		//omega
		fOmega	
			= m_stParticleProp.m_fPAngularspeed
			+ ( Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*2.f - 1.f ) 
			* m_stParticleProp.m_fPAngularspeedOffset;

		//life
		dwLife	= m_stParticleProp.m_dwParticleLife
				+ static_cast<RwUInt32>
				  ( Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime()) * m_stParticleProp.m_dwParticleLifeOffset );
		
		//envirenment param
		stCofEnvrn.m_fCofGrav
			= m_stParticleProp.m_stCofEnvrn.m_fCofGrav 
			+ ( Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*2.f - 1.f ) 
			* m_stParticleProp.m_stCofEnvrnOffset.m_fCofGrav;
		stCofEnvrn.m_fCofAirResistance
			= m_stParticleProp.m_stCofEnvrn.m_fCofAirResistance
			+ ( Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*2.f - 1.f ) 
			* m_stParticleProp.m_stCofEnvrnOffset.m_fCofAirResistance;


		pNewParticle	= new PARTICLE( v3dNewPos
			, v3dVel
			, v3dScale
			, rgba
			, stUVRect
			, fOmega
			, dwLife
			, dwShootTime
			, stCofEnvrn );

		ASSERT( pNewParticle );
		if( !pNewParticle )
		{
			Eff2Ut_ERR( "pNewParticle == NULL @ AgcdEffParticleSystem::bShootPaticle" );
			return pEffCtrl_PSyst->tReturnErr();
		}


		if( T_ISMINUS4( pEffCtrl_PSyst->_PushBack_Particle( pNewParticle, capacity ) ) )
		{
			Eff2Ut_ERR( "pEffCtrl_PSyst->vPushBack_Particle failed @ AgcdEffParticleSystem::bShootPaticle" );
			DEF_SAFEDELETE( pNewParticle );
			return pEffCtrl_PSyst->tReturnErr();
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffParticleSystem::bToFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tToFile(fp);
	ir += AgcdEffRenderBase::tToFile(fp);

	ir += fwrite(&m_nCapacity, 1, sizeof(m_nCapacity), fp);
	ir += fwrite(&m_dwShootDelay, 1, sizeof(m_dwShootDelay), fp);
	ir += fwrite(&m_szClumpFName, 1, sizeof(m_szClumpFName), fp);
	ir += fwrite(&m_stEmiter, 1, sizeof(m_stEmiter), fp);
	ir += fwrite(&m_stParticleProp, 1, sizeof(m_stParticleProp), fp);

	ir += AgcdEffBase::tToFileVariableData(fp);
	return ir;
}
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffParticleSystem::bFromFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tFromFile(fp);
	ir += AgcdEffRenderBase::tFromFile(fp);

	ir += fread(&m_nCapacity, 1, sizeof(m_nCapacity), fp);
	ir += fread(&m_dwShootDelay, 1, sizeof(m_dwShootDelay), fp);
	ir += fread(&m_szClumpFName, 1, sizeof(m_szClumpFName), fp);
	ir += fread(&m_stEmiter, 1, sizeof(m_stEmiter), fp);
	ir += fread(&m_stParticleProp, 1, sizeof(m_stParticleProp), fp);

	if(m_dwShootDelay == 0)
		m_dwShootDelay = 1;

	RwInt32 ir2 = AgcdEffBase::tFromFileVariableData(fp);
	if( T_ISMINUS4(ir2) )
	{
		ASSERT( !"AgcdEffBase::tFromFileVariableData failed" );
		return -1;
	}

	return ir+ir2;
}


#ifdef USE_MFC
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
INT32 AgcdEffParticleSystem::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_PSYS );
	if( T_ISMINUS4( AgcdEffBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}
	if( T_ISMINUS4( AgcdEffRenderBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}

	AgcdEffParticleSystem* pEffPSys = static_cast<AgcdEffParticleSystem*>(pEffBase);
	m_nCapacity = pEffPSys->m_nCapacity;
	m_dwShootDelay = pEffPSys->m_dwShootDelay;	
	bSetClumpFile( pEffPSys->m_szClumpFName );
	m_stEmiter = pEffPSys->m_stEmiter;
	m_stParticleProp = pEffPSys->m_stParticleProp;

	return 0;
};
#endif//USE_MFC

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////
AgcdEffParticleSys_SimpleBlackHole::AgcdEffParticleSys_SimpleBlackHole
(AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType)
: AgcdEffRenderBase( AgcdEffBase::E_EFFBASE_PSYS_SIMPLEBLACKHOLE, eBlendType )
, m_nCapacity(0)
, m_nNumOfOneShoot(0)
, m_nNumOfOneShootOffset(0)
, m_dwTimeGap(0LU)
, m_dwParticleLife(0LU)
, m_fInitSpeed(0.f)
, m_fInitSpeedOffset(0.f)
, m_fRollMin(0.f)
, m_fRollMax(0.f)
, m_fRollStep(0.f)
, m_fRadius(0.f)
{
	EFFMEMORYLOG_CON;
}

AgcdEffParticleSys_SimpleBlackHole::~AgcdEffParticleSys_SimpleBlackHole()
{
	EFFMEMORYLOG_DES;
}

///////////////////////////////////////////////////////////////////////////////
// vCircleEmiter
///////////////////////////////////////////////////////////////////////////////
VOID AgcdEffParticleSys_SimpleBlackHole::vCircleEmiter( RwV3d* pPos )const
{
	if( !bFlagChk( FLAG_EFFBASEPSYSSBH_CIRCLEEMITER ) )
		return;

	RwReal	Angle	= Eff2Ut_GETRNDFLOAT1(Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*360.f);
	
	RwV3d	offset	= { 0.f,0.f,0.f };
	if( bFlagChk( FLAG_EFFBASEPSYSSBH_FILLCIRCLE ) )
	{
		RwReal	fRadius	= Eff2Ut_GETRNDFLOAT1(AgcdEffGlobal::bGetInst().bGetCurrTime())*m_fRadius;
		offset.x	+= fRadius * AcuSinTbl::Cos(Angle);
		offset.z	+= fRadius * AcuSinTbl::Sin(Angle);
	}
	else
	{
		offset.x	+= m_fRadius * AcuSinTbl::Cos(Angle);
		offset.z	+= m_fRadius * AcuSinTbl::Sin(Angle);
	}

	RwV3dAdd( pPos, pPos, &offset );
}

///////////////////////////////////////////////////////////////////////////////
// bShootPaticle
// 안보이면 파티클 생성 안한다.
// dwAccumulateTime : AgcdEffCtrl_Set::m_stTimeLoop.m_dwCurrTime
//					, 이펙트 셋 생성후로 누적된 시간을 받는다.
// err : -1, good : 0
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffParticleSys_SimpleBlackHole::bShootPaticle
( RwUInt32 dwAccumulateTime, AgcdEffCtrl_ParticleSyst_SBH* pEffCtrl_PSystSBH )const
{
	ASSERT( pEffCtrl_PSystSBH );

	
	if( pEffCtrl_PSystSBH->GetState() == E_EFFCTRL_STATE_CONTINUE )
		return 0;

	if( m_eLoopOpt == e_TblDir_none )
	{
		if( dwAccumulateTime - m_dwDelay + m_dwParticleLife > m_dwLife )
			return 0;
	}
	
	RwUInt32	capacity	= m_nCapacity;
	RwUInt32	shootdelay	= m_dwTimeGap ? m_dwTimeGap : 30;
	if( pEffCtrl_PSystSBH->GetCPtrEffCtrl_Set()->GetParticleNumScale() != 1.f )
	{
		ASSERT(pEffCtrl_PSystSBH->GetCPtrEffCtrl_Set()->GetParticleNumScale() > 0.f);
		capacity	= floatToInt( static_cast<RwReal>(m_nCapacity)*pEffCtrl_PSystSBH->GetCPtrEffCtrl_Set()->GetParticleNumScale() );
		shootdelay	= floatToInt( static_cast<RwReal>(m_dwTimeGap)/pEffCtrl_PSystSBH->GetCPtrEffCtrl_Set()->GetParticleNumScale() );
	}


	RwUInt32	dwPastTime	= dwAccumulateTime - pEffCtrl_PSystSBH->m_dwLastShootTime;
	if( !dwPastTime )
		return 0;

	RwReal	fRollDist	= m_fRollMax - m_fRollMin;

	RwV3d		vOrigin	= { 0.f, 0.f, 0.f };
	RwV3d		vPos	= { 0.f, 0.f, 0.f };
	RwV3d		v3dVel	= { 0.f, 0.f, 0.f };
	RwV3d		v3dScale= { 1.f, 1.f, 1.f };
	RwReal		fSpeed	= 0.f;
	RwReal		fDeg	= 0.f;
	RwReal		fLife	= m_dwParticleLife * 0.001f;
	RwRGBA		rgba	= { 255, 255, 255, 255 };
	STUVRECT	stUVRect;
	stCofEnvrnParam	stCofEnvrn;

	LPPARTICLE	pNewParticle	= NULL;
	if( shootdelay < dwPastTime )
	{
		dwPastTime = shootdelay;
		pEffCtrl_PSystSBH->m_dwLastShootTime	= dwAccumulateTime - dwPastTime;
	}

	int nNum	= 0;
	int beforemax =StParticle::MAX_CNT;

	while( dwPastTime >= shootdelay )
	{
		dwPastTime -= shootdelay;
		pEffCtrl_PSystSBH->m_dwLastShootTime	+= shootdelay;

		RwInt32	nNumOfOneShoot	= m_nNumOfOneShoot 
								+ static_cast<RwInt32>
								  ( Eff2Ut_GETRNDFLOAT1(dwAccumulateTime) * m_nNumOfOneShootOffset );

		nNum+=nNumOfOneShoot;

		for( int i=0; i<nNumOfOneShoot; ++i )
		{
			if( DEF_FLAG_CHK( bGetFlag(), FLAG_EFFBASEPSYSSBH_CAPACITYLIMIT ) )
				if( pEffCtrl_PSystSBH->m_listLpParticle.size() >= capacity )
					return 0;

			if( DEF_FLAG_CHK( m_dwBitFlags , FLAG_EFFBASEPSYSSBH_REGULARANGLE ) )
			{
				if( i==0 )
				{
					if( pEffCtrl_PSystSBH->m_listLpParticle.empty() )
					{
						fDeg	= m_fRollMin;
					}
					else
					{
						fDeg	= pEffCtrl_PSystSBH->m_listLpParticle.back()->m_fOmega + m_fRollStep;
					}
				}
				else
				{
					fDeg	+= m_fRollStep;
				}

				if( fDeg > m_fRollMax )
					fDeg = m_fRollMin;
			}
			else
			{
				fDeg	= m_fRollMin + Eff2Ut_GETRNDFLOAT1(dwAccumulateTime) * fRollDist;
			}

			fSpeed	= m_fInitSpeed + (Eff2Ut_GETRNDFLOAT1(dwAccumulateTime)*2.f - 1.f)*m_fInitSpeedOffset;

			if( bFlagChk( FLAG_EFFBASEPSYSSBH_MINANGLE ) )
			{
				v3dVel.y	= fSpeed;
				fDeg		= 0;
			}
			else
			{
				v3dVel.x	= AcuSinTbl::Sin(180.f-fDeg) * fSpeed;
				v3dVel.y	= AcuSinTbl::Cos(180.f-fDeg) * fSpeed;
			}

			if( DEF_FLAG_CHK( m_dwBitFlags , FLAG_EFFBASEPSYSSBH_RANDCOLR ) )
			{
				rgba.red	= Eff2Ut_GETRNDRGB(dwAccumulateTime);
				rgba.green	= Eff2Ut_GETRNDRGB(dwAccumulateTime);
				rgba.blue	= Eff2Ut_GETRNDRGB(dwAccumulateTime);
				rgba.alpha	= 255;
			}
			
			if( bFlagChk( FLAG_EFFBASEPSYSSBH_OLDTYPE ) )
				RwV3dTransformVector ( &v3dVel, &v3dVel, BMATRIX::bGetInst().bGetPtrMatB() );

			if( bFlagChk( FLAG_EFFBASEPSYSSBH_INVS ) )
			{
				RwV3dScale( &vPos, &v3dVel, -fLife );
			}
			else
			{
				vPos = vOrigin;
			}

			if( !bFlagChk( FLAG_EFFBASEPSYSSBH_CHILDDEPENDANCY ) )
			if( bFlagChk( FLAG_EFFBASEPSYSSBH_OLDTYPE ) )
			{
				//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
				RwV3dAdd( &vPos, &vPos, RwMatrixGetPos( RwFrameGetLTM( pEffCtrl_PSystSBH->GetPtrFrm() ) ) );
				//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
			}

			vCircleEmiter( &vPos );

			pNewParticle	= new PARTICLE	( vPos
											, v3dVel
											, v3dScale
											, rgba
											, stUVRect
											, fDeg		//omega -> angle
											, m_dwParticleLife
											, pEffCtrl_PSystSBH->m_dwLastShootTime
											, stCofEnvrn );

			

			ASSERT( pNewParticle );
			if( !pNewParticle )
			{
				Eff2Ut_ERR( "pNewParticle == NULL @ AgcdEffParticleSys_SimpleBlackHole::bShootPaticle" );
				return pEffCtrl_PSystSBH->tReturnErr();
			}

			if( T_ISMINUS4( pEffCtrl_PSystSBH->_PushBack_Particle( pNewParticle, capacity ) ) )
			{
				Eff2Ut_ERR( "pEffCtrl_PSystSBH->vPushBack_Particle failed @ AgcdEffParticleSys_SimpleBlackHole::bShootPaticle" );
				return pEffCtrl_PSystSBH->tReturnErr();
			}
		}

	}//while(dwPastTime > m_dwTimeGap)

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffParticleSys_SimpleBlackHole::bToFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tToFile(fp);
	ir += AgcdEffRenderBase::tToFile(fp);

	ir += fwrite( &m_nCapacity, 1
				, sizeof(m_nCapacity)
				+ sizeof(m_nNumOfOneShoot)
				+ sizeof(m_nNumOfOneShootOffset)
				+ sizeof(m_dwTimeGap)
				+ sizeof(m_dwParticleLife)
				+ sizeof(m_fInitSpeed)
				+ sizeof(m_fInitSpeedOffset)
				+ sizeof(m_fRollMin)
				+ sizeof(m_fRollMax)
				+ sizeof(m_fRollStep)
				+ sizeof(m_fRadius)
				, fp);

	ir += AgcdEffBase::tToFileVariableData(fp);
	return ir;
}
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffParticleSys_SimpleBlackHole::bFromFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tFromFile(fp);
	ir += AgcdEffRenderBase::tFromFile(fp);

	ir += fread( &m_nCapacity, 1
				, sizeof(m_nCapacity)
				+ sizeof(m_nNumOfOneShoot)
				+ sizeof(m_nNumOfOneShootOffset)
				+ sizeof(m_dwTimeGap)
				+ sizeof(m_dwParticleLife)
				+ sizeof(m_fInitSpeed)
				+ sizeof(m_fInitSpeedOffset)
				+ sizeof(m_fRollMin)
				+ sizeof(m_fRollMax)
				+ sizeof(m_fRollStep)
				+ sizeof(m_fRadius)
				, fp);

	if( m_dwTimeGap == 0 )
		m_dwTimeGap = 1;

	bSetRollMinMax(m_fRollMin, m_fRollMax);

	RwInt32 ir2 = AgcdEffBase::tFromFileVariableData(fp);
	if( T_ISMINUS4(ir2) )
	{
		ASSERT( !"AgcdEffBase::tFromFileVariableData failed" );
		return -1;
	}

	return ir+ir2;
}


#ifdef USE_MFC
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
INT32 AgcdEffParticleSys_SimpleBlackHole::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_PSYS_SIMPLEBLACKHOLE );
	if( T_ISMINUS4( AgcdEffBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}
	if( T_ISMINUS4( AgcdEffRenderBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}

	AgcdEffParticleSys_SimpleBlackHole* pEffPSBH = static_cast<AgcdEffParticleSys_SimpleBlackHole*>(pEffBase);
	m_nCapacity = pEffPSBH->m_nCapacity;
	m_nNumOfOneShoot = pEffPSBH->m_nNumOfOneShoot;
	m_nNumOfOneShootOffset = pEffPSBH->m_nNumOfOneShootOffset;
	m_dwTimeGap = pEffPSBH->m_dwTimeGap;
	m_dwParticleLife = pEffPSBH->m_dwParticleLife;
	m_fInitSpeed = pEffPSBH->m_fInitSpeed;
	m_fInitSpeedOffset = pEffPSBH->m_fInitSpeedOffset;
	m_fRollMin = pEffPSBH->m_fRollMin;
	m_fRollMax = pEffPSBH->m_fRollMax;
	m_fRadius = pEffPSBH->m_fRadius;

	return 0;
};
#endif//USE_MFC