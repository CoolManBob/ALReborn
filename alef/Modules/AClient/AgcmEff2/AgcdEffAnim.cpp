#include "AgcdEffAnim.h"
#include "AgcuEffPath.h"
#include "AgcuEffUtil.h"
#include "AgcuBillBoard.h"
#include "AgcdEffCtrl.h"
#include "AgcdEffGlobal.h"
#include "AgcdEffCtrl.h"

#include "ApMemoryTracker.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG( AgcdEffAnim_Colr );
EFFMEMORYLOG( AgcdEffAnim_TuTv );
EFFMEMORYLOG( AgcdEffAnim_Missile );
EFFMEMORYLOG( AgcdEffAnim_Linear );
EFFMEMORYLOG( AgcdEffAnim_Rev );
EFFMEMORYLOG( AgcdEffAnim_Rot );
EFFMEMORYLOG( AgcdEffAnim_RpSpline );
EFFMEMORYLOG( AgcdEffAnim_RtAnim );
EFFMEMORYLOG( AgcdEffAnim_Scale );
EFFMEMORYLOG( AgcdEffAnim_ParticlePosScale );

StUVRect::StUVRect( RwReal fL, RwReal fT, RwReal fR, RwReal fB ) : m_fLeft( fL ), m_fTop( fT ), m_fRight( fR ), m_fBottom( fB )
{
};

const char* EFFANIMTYPE_NAME[AgcdEffAnim::E_EFFANIM_NUM] = 
{
	"color"		,	//"E_EFFANIM_COLOR"			,
	"tutv"		,	//"E_EFFANIM_TUTV"			,
	"missile"	,	//"E_EFFANIM_MISSILE"		,
	"linear"	,	//"E_EFFANIM_LINEAR"		,
	"revolution",	//"E_EFFANIM_REVOLUTION"	,
	"rotation"	,	//"E_EFFANIM_ROTATION"		,
	"rpspline"	,	//"E_EFFANIM_RPSPLINE"		,
	"rtanim"	,	//"E_EFFANIM_RTANIM"		,
	"scale"		,	//"E_EFFANIM_SCALE"			,
	"NotUsed"	,	//"E_EFFANIM_POSSCALE		,
	"postfx"	,	//"E_EFFANIM_POSTFX"		,
	"camera"	,	//"E_EFFANIM_CAMERA"		,

};

//--------------------- AgcdEffAnim -----------------
AgcdEffAnim::AgcdEffAnim ( E_EFFANIMTYPE eEffAnimType, RwUInt32 dwBitFlags ) :
 m_eEffAnimType( eEffAnimType ),
 m_dwBitFlags( dwBitFlags ),
 m_dwLifeTime( 0 ),
 m_eLoopOpt( e_TblDir_none )
{
}

AgcdEffAnim::~AgcdEffAnim()
{
}

RwInt32 AgcdEffAnim::tToFile(FILE* fp)
{
	ASSERT(fp);
	return fwrite( (LPCVOID)(&m_eEffAnimType), 1, sizeof(m_eEffAnimType) + sizeof(m_dwBitFlags) + sizeof(m_dwLifeTime) + sizeof(m_eLoopOpt), fp );
}

RwInt32 AgcdEffAnim::tFromFile(FILE* fp)
{
	ASSERT(fp);
	return fread( (LPVOID)(&m_dwBitFlags), 1, sizeof(m_dwBitFlags) + sizeof(m_dwLifeTime) + sizeof(m_eLoopOpt), fp );
}

//--------------------- AgcdEffAnim_Colr -----------------
AgcdEffAnim_Colr::AgcdEffAnim_Colr(RwUInt32 dwBitFlags, RwInt32 nSize) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_COLOR, dwBitFlags ),
 m_stTblColr( nSize )
{
	EFFMEMORYLOG_CON;
}

AgcdEffAnim_Colr::~AgcdEffAnim_Colr()
{
	EFFMEMORYLOG_DES;
}

BOOL AgcdEffAnim_Colr::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	if ( !pOut )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Colr::bGetTVal failed : pOut == NULL" );
		return FALSE;
	}

	return m_stTblColr.bGetValIntp( static_cast<RwRGBA*>(pOut), dwCurrTime );
}

BOOL AgcdEffAnim_Colr::bSetTVal( RwInt32 nIndex, void* pIn, RwUInt32 dwTime )
{
	if( !pIn )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Colr::bSetTVal failed : pIn == NULL" );
		return FALSE;
	}

	return m_stTblColr.bSetVal( nIndex, *( static_cast<RwRGBA*>(pIn) ), dwTime );
}

BOOL AgcdEffAnim_Colr::bInsTVal( void* pIn, RwUInt32 dwTime )
{
	if( !pIn )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Colr::bInsTVal failed : pIn == NULL" );
		return FALSE;
	}

#ifdef USE_MFC
	BOOL br = m_stTblColr.bInsert( dwTime, *( static_cast<RwRGBA*>(pIn) ) );
	if( br )
		m_stTblColr.bGetLastTime( &m_dwLifeTime );

	return br;
#else
	return m_stTblColr.bInsert( dwTime, *( static_cast<RwRGBA*>(pIn) ) );
#endif//USE_MFC
}

BOOL AgcdEffAnim_Colr::bDelTVal( RwUInt32 dwTime )
{
	return m_stTblColr.bDelete( dwTime );
}
	
RwInt32 AgcdEffAnim_Colr::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32 ulFlagReserved )
{
	ASSERT( pEffCtrlBase );
	RwUInt32	ulKeyTime	= 0LU;
	RwInt32		ir	= ::CalcKeyTime( &ulKeyTime, m_eLoopOpt, dwAccumulateTime, m_dwLifeTime );

	ASSERT( !T_ISMINUS4( ir ) );
	if( ir )
	{
		if( T_ISMINUS4( ir ) )
		{
			Eff2Ut_ERR( "CalcKeyTime failed @ AgcdEffAnim_Colr::bUpdateVal" );
			return -1;
		}

		return ir;
	}

	pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_COLR );
	m_stTblColr.bGetValIntp( pEffCtrlBase->GetPtrRGBA(), ulKeyTime );
	pEffCtrlBase->ApplyRGBScale();

	if( pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT )
	{
		AgcdEffCtrl_Obj* pEffCtrlObj = static_cast<AgcdEffCtrl_Obj*>(pEffCtrlBase);
			
		if( pEffCtrlObj && 
			pEffCtrlObj->IsAddedToWorld() &&
			pEffCtrlObj->GetBlendType() == AgcdEffRenderBase::EFFBLEND_NONE )
		{
			if( pEffCtrlBase->GetPtrRGBA()->alpha == 255 )
			{
				if( pEffCtrlObj->GetCurrBlendType() != AgcdEffRenderBase::EFFBLEND_NONE )
				{
					pEffCtrlObj->SetCurrBlendType( AgcdEffRenderBase::EFFBLEND_NONE );
				}
			}
			else if( pEffCtrlObj->GetCurrBlendType() == AgcdEffRenderBase::EFFBLEND_NONE )
			{
				pEffCtrlObj->SetCurrBlendType( AgcdEffRenderBase::EFFBLEND_ADD_SRCA_INVSRCA );
			}
		}
	}

	return 0;
}

RwInt32 AgcdEffAnim_Colr::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	ir += m_stTblColr.bToFile(fp);
	return ir;
};

RwInt32 AgcdEffAnim_Colr::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);
	ir += m_stTblColr.bFromFile(fp);
	return ir;
};

//--------------------- AgcdEffAnim_Colr -----------------
AgcdEffAnim_TuTv::AgcdEffAnim_TuTv(RwUInt32 dwBitFlags/*=0x0*/, RwInt32 nSize/*=0*/) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_TUTV, dwBitFlags ),
 m_stTblRect( nSize )
{
	EFFMEMORYLOG_CON;
}

AgcdEffAnim_TuTv::~AgcdEffAnim_TuTv()
{
	EFFMEMORYLOG_DES;
}

BOOL AgcdEffAnim_TuTv::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	if ( !pOut )
		return FALSE;

	return m_stTblRect.bGetVal( static_cast<LPSTUVRECT>(pOut), dwCurrTime );
}

BOOL AgcdEffAnim_TuTv::bSetTVal( RwInt32 nIndex, void* pIn, RwUInt32 dwCurrTime )
{
	if( !pIn )
		return FALSE;

	return m_stTblRect.bSetVal( nIndex, *( static_cast<LPSTUVRECT>(pIn) ), dwCurrTime );
}

BOOL AgcdEffAnim_TuTv::bInsTVal( void* pIn, RwUInt32 dwTime )
{
	if( !pIn )
		return FALSE;
	
#ifdef USE_MFC
	BOOL br = m_stTblRect.bInsert( dwTime, *( static_cast<LPSTUVRECT>(pIn) ) );
	if( br ){
		m_stTblRect.bGetLastTime(&m_dwLifeTime);
	}
	return br;
#else
	return m_stTblRect.bInsert( dwTime, *( static_cast<LPSTUVRECT>(pIn) ) );
#endif//USE_MFC
}

BOOL AgcdEffAnim_TuTv::bDelTVal( RwUInt32 dwTime )
{
	return m_stTblRect.bDelete( dwTime );
}

RwInt32 AgcdEffAnim_TuTv::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32 ulFlagReserved )
{
	RwUInt32	ulKeyTime	= 0LU;
	RwInt32		ir	= ::CalcKeyTime( &ulKeyTime, m_eLoopOpt, dwAccumulateTime, m_dwLifeTime );

	if( ir )
	{
		if( T_ISMINUS4( ir ) )
			return -1;

		return ir;
	}

	pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_UVRECT );
	m_stTblRect.bGetVal( pEffCtrlBase->GetPtrUVRect(), ulKeyTime );

	return 0;
}

RwInt32 AgcdEffAnim_TuTv::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	ir += m_stTblRect.bToFile(fp);
	return ir;
};

RwInt32 AgcdEffAnim_TuTv::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);
	ir += m_stTblRect.bFromFile(fp);
	return ir;
};

//--------------------- AgcdEffAnim_Missile -----------------
AgcdEffAnim_Missile::AgcdEffAnim_Missile( RwUInt32 dwBitFlags, RwReal fSpeed, RwReal fAccel , RwReal fRotate , RwReal fRadius , RwReal fZigzagLength , RwReal fMinSpeed , RwReal fMaxSpeed ) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_MISSILE, dwBitFlags ),
 m_fSpeed0( fSpeed ),
 m_fAccel( fAccel ),
 m_fRotate( fRotate ),
 m_fRadius( fRadius ),
 m_fZigzagLength( fZigzagLength ),
 m_fMinSpeed( fMinSpeed ),
 m_fMaxSpeed( fMaxSpeed ),
 m_fPrevTime( 0.0f ),
 m_fPrevCof( 0.0f )
{
	EFFMEMORYLOG_CON;

	m_fRotate		=	0;
	m_fRadius		=	0;
	m_fZigzagLength	=	0;
}

AgcdEffAnim_Missile::~AgcdEffAnim_Missile()
{
	EFFMEMORYLOG_DES;
}

BOOL AgcdEffAnim_Missile::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	if ( !pOut )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Missile::bGetTVal failed : pOut == NULL" );
		return FALSE;
	}

	RwReal	fCurrTime = static_cast<RwReal>(dwCurrTime) * 0.001f;

	//dt : delta time
	//DIR : direction vector
	//accel = const
	//speed = accel * dt + speed0
	//pos	= 1/2 * accel * dt^2 * DIR + speed0 * dt * DIR + pos0
	//pos	= (1/2 * accel * dt^2 + speed0 * dt) * DIR + pos0;
	//return value : (1/2 * accel * dt^2 + speed0 * dt)
	(*static_cast<RwReal*>(pOut)) = 0.5f * m_fAccel * fCurrTime * fCurrTime + m_fSpeed0 * fCurrTime;
								    
	return TRUE;
}

RwInt32	Eff2Ut_Bezier3( RwV3d* pOut, const RwV3d* pv3d0, const RwV3d* pv3d1, const RwV3d* pv3d2, float fu )
{
	ASSERT( fu >= 0.f && fu <= 1.f );

	RwReal	fOneMinusU	= 1.f - fu;
	RwV3d	vTemp	= { 0.f, 0.f, 0.f };
	
	RwV3dScale( pOut, pv3d0, fOneMinusU*fOneMinusU );
	RwV3dScale( &vTemp, pv3d1, 2.f*fu*fOneMinusU );
	RwV3dAdd( pOut, pOut, &vTemp );
	RwV3dScale( &vTemp, pv3d2, fu*fu );
	RwV3dAdd( pOut, pOut, &vTemp );
	
	return 0;
}

RwInt32 AgcdEffAnim_Missile::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32 ulFlagReserved )
{
	RwUInt32	ulKeyTime	= 0LU;
	
	const STMISSILEINFO* pMissileInfo = &pEffCtrlBase->GetCPtrEffCtrl_Set()->GetPtrEffSet()->bGetMissileInfo();
	MISSILETARGETINFO* pMissileTargetInfo = pEffCtrlBase->GetPtrEffCtrl_Set()->GetPtrMissileTargetInfo();

	RwReal	fPastTime	=	dwAccumulateTime * 0.001f;
	RwReal	fMissileSpd	=	( m_fAccel * fPastTime * 0.5f + m_fSpeed0 );
	RwReal	fCof		=	fMissileSpd * fPastTime;
	RwReal	fTime		=	fPastTime - m_fPrevTime;
	RwReal	fDistance	=	fCof - m_fPrevCof;
	RwReal	fMoveSecond	=	fDistance	*	(1.0f/fTime);

	if( dwAccumulateTime == 0 )
	{
		m_fPrevTime		=	0;
		m_fPrevCof		=	0.f;
	}

	else if( m_fMinSpeed > 0 && fMoveSecond < m_fMinSpeed )
	{
		fCof	-=	fDistance;
		fCof	+=	m_fMinSpeed / (1.0f/fTime);
	}

	else if( m_fMaxSpeed > 0 && fMoveSecond > m_fMaxSpeed )
	{
		fCof	-=	fDistance;
		fCof	+=	m_fMaxSpeed / (1.0f/fTime);
	}

	m_fPrevTime		=	fPastTime;
	m_fPrevCof			=	fCof;
	
	RwFrame*	pFrm	= pEffCtrlBase->GetPtrFrm();
	RwMatrix*	pModelling	= RwFrameGetMatrix( pFrm );
		
	switch( pMissileInfo->m_eMissileType )
	{
	case e_missile_linear:
		{
			RwV3d	vDir;
			RwV3dSub( &vDir, &pMissileTargetInfo->m_v3dTarget, &pMissileTargetInfo->m_v3dGP0 );
			RwV3d	fromto = { vDir.x, vDir.y, vDir.z };
			RwReal	fLen	= RwV3dLength( &vDir );

			if( fLen <= fCof )
			{
				RwFrameTranslate( pFrm, &pMissileTargetInfo->m_v3dTarget, rwCOMBINEPOSTCONCAT );
			}
			else
			{			
				//translate
				RwV3dNormalize( &vDir, &vDir );
				RwV3dScale( &vDir, &vDir, fCof );
				RwV3dAdd( &vDir, &vDir, &pMissileTargetInfo->m_v3dGP0 );
				RwFrameTranslate( pFrm, &vDir, rwCOMBINEPOSTCONCAT );
			}

			//rotate : 오브젝트인경우만..
			if( pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT )
			{
				AgcdEffCtrl_Obj* pEffCtrlObj = static_cast<AgcdEffCtrl_Obj*>(pEffCtrlBase);
				if( pEffCtrlObj )
				{
					RwBool	bRotToUp	= FALSE;
					RwReal	fDeg_ToUp	= 0.f;
					if( ::fabs( fromto.y ) > 0.0001f )
					{
						bRotToUp	= TRUE;
						RwV3d	tempFromTo	= fromto;
						fromto.y	= 0.f;

						RwV3dNormalize( &tempFromTo, &tempFromTo );
						RwV3dNormalize( &fromto, &fromto );
						
						RwReal fDot	= RwV3dDotProduct( &tempFromTo, &fromto );
						Eff2Ut_CLAMP( fDot, -1.f, 1.f );
						fDeg_ToUp	= DEF_R2D( acosf( fDot ) );

						if( tempFromTo.y > 0.f )
							fDeg_ToUp = -fDeg_ToUp;
					}
					else
						RwV3dNormalize( &fromto, &fromto );

					RwReal	fDot = RwV3dDotProduct ( &fromto, AXISWORLD::bGetInst().bGetPtrWorldZ() );

					Eff2Ut_CLAMP( fDot, -1.f, 1.f );
					RwReal	fDeg = DEF_R2D( acosf(fDot) );

					if( fromto.x < 0.f )
						fDeg	= -fDeg;

					pFrm = pEffCtrlObj->GetPtrFrmClump();
					pModelling = RwFrameGetMatrix(pFrm);

					RwMatrixRotate( pModelling, AXISWORLD::bGetInst().bGetPtrWorldY(), fDeg, rwCOMBINEREPLACE );
					if( bRotToUp )
						RwMatrixRotate( pModelling, &RwFrameGetLTM(pEffCtrlObj->GetPtrFrmClump())->right, fDeg_ToUp, rwCOMBINEPOSTCONCAT );
					
					RwFrameUpdateObjects( pFrm );
				}
			}

			if( fLen <= fCof )
				return 1;
		}
		break;

	case e_missile_bezier3:
		{
			RwV3d	v3dTrans	= { 0.f,0.f,0.f };
			RwV3d	vDir;
			RwV3dSub( &vDir, &pMissileTargetInfo->m_v3dTarget, &pMissileTargetInfo->m_v3dGP0 );

			RwReal	fLen	= RwV3dLength( &vDir );
			if( fLen <= fCof )
			{
				RwFrameTranslate( pFrm , &pMissileTargetInfo->m_v3dTarget, rwCOMBINEPOSTCONCAT );
				v3dTrans	= pMissileTargetInfo->m_v3dTarget;
			}
			else
			{
				//translate
				Eff2Ut_Bezier3( &v3dTrans, &pMissileTargetInfo->m_v3dGP0, &pMissileTargetInfo->m_v3dGP1, &pMissileTargetInfo->m_v3dTarget, fCof / fLen );
				RwFrameTranslate( pFrm , &v3dTrans, rwCOMBINEPOSTCONCAT );
			}
			
			//rotate : 오브젝트인경우만..
			if( AgcdEffBase::E_EFFBASE_OBJECT == pEffCtrlBase->GetBaseType() )
			{
				AgcdEffCtrl_Obj* pEffCtrlObj = static_cast<AgcdEffCtrl_Obj*>(pEffCtrlBase);
				if( pEffCtrlObj )
				{
					RwV3d	fromto = { vDir.x, vDir.y, vDir.z };
					RwV3dNormalize( &fromto, &fromto );
					RwReal	fDot = RwV3dDotProduct ( &fromto, AXISWORLD::bGetInst().bGetPtrWorldZ() );
					Eff2Ut_CLAMP( fDot, -1.f, 1.f );
					RwReal	fDeg = DEF_R2D( acosf(fDot) );
					if( vDir.x < 0.f )
						fDeg	= -fDeg;

					RwFrameRotate( pEffCtrlObj->GetPtrFrmClump(), AXISWORLD::bGetInst().bGetPtrWorldY(), fDeg, rwCOMBINEREPLACE );
					pMissileTargetInfo->m_v3dLastPt = v3dTrans;
				}
			}
			
			if( fLen <= fCof )
				return 1;
		}
		break;

	case e_missile_linear_rot:
		{
			RwV3d		vDir;
			RwV3dSub( &vDir, &pMissileTargetInfo->m_v3dTarget, &pMissileTargetInfo->m_v3dGP0 );
			RwV3d		fromto			=	{ vDir.x, vDir.y, vDir.z };
			RwReal		fLen			=	RwV3dLength( &vDir );
			RwV3d		vMoveMissile	=	{ 0 , m_fRadius , 0 };
			RwMatrix	MissileRotate;
			ZeroMemory( &MissileRotate , sizeof(RwMatrix) );

			if( fLen <= fCof )
			{
				RwFrameTranslate( pFrm, &pMissileTargetInfo->m_v3dTarget, rwCOMBINEPOSTCONCAT );
			}
			else
			{			
				RwMatrix*	pMatrix	=	RwFrameGetMatrix( pFrm );
				RwV3d*	pPos	=	RwMatrixGetPos( pMatrix );

				if( RwV3dLength( pPos ) )
				{
					*pPos	=	pMissileTargetInfo->m_v3dLastPt;					
				}

				//translate
				RwV3dNormalize( &vDir, &vDir );
				RwV3dScale( &vDir, &vDir, fCof );
				RwV3dAdd( &vDir, &vDir, &pMissileTargetInfo->m_v3dGP0 );
				RwFrameTranslate( pFrm, &vDir, rwCOMBINEPOSTCONCAT );

				pMissileTargetInfo->m_v3dLastPt	=	*RwMatrixGetPos( pMatrix );
		
				// Missile Rotate
				RwMatrixRotate( &MissileRotate , &vDir , m_fRotate*fPastTime , rwCOMBINEREPLACE );
				RwV3dTransformPoint( &vMoveMissile , &vMoveMissile , &MissileRotate );
				RwFrameTranslate( pFrm , &vMoveMissile , rwCOMBINEPOSTCONCAT );

			}

			//rotate : 오브젝트인경우만..
			if( pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT )
			{
				AgcdEffCtrl_Obj* pEffCtrlObj = static_cast<AgcdEffCtrl_Obj*>(pEffCtrlBase);
				if( pEffCtrlObj )
				{
					RwBool	bRotToUp	= FALSE;
					RwReal	fDeg_ToUp	= 0.f;
					if( ::fabs( fromto.y ) > 0.0001f )
					{
						bRotToUp	= TRUE;
						RwV3d	tempFromTo	= fromto;
						fromto.y	= 0.f;

						RwV3dNormalize( &tempFromTo, &tempFromTo );
						RwV3dNormalize( &fromto, &fromto );

						RwReal fDot	= RwV3dDotProduct( &tempFromTo, &fromto );
						Eff2Ut_CLAMP( fDot, -1.f, 1.f );
						fDeg_ToUp	= DEF_R2D( acosf( fDot ) );

						if( tempFromTo.y > 0.f )
							fDeg_ToUp = -fDeg_ToUp;
					}
					else
						RwV3dNormalize( &fromto, &fromto );

					RwReal	fDot = RwV3dDotProduct ( &fromto, AXISWORLD::bGetInst().bGetPtrWorldZ() );

					Eff2Ut_CLAMP( fDot, -1.f, 1.f );
					RwReal	fDeg = DEF_R2D( acosf(fDot) );

					if( fromto.x < 0.f )
						fDeg	= -fDeg;

					pFrm = pEffCtrlObj->GetPtrFrmClump();
					pModelling = RwFrameGetMatrix(pFrm);

					RwMatrixRotate( pModelling, AXISWORLD::bGetInst().bGetPtrWorldY(), fDeg, rwCOMBINEREPLACE );
					if( bRotToUp )
						RwMatrixRotate( pModelling, &RwFrameGetLTM(pEffCtrlObj->GetPtrFrmClump())->right, fDeg_ToUp, rwCOMBINEPOSTCONCAT );

					RwFrameUpdateObjects( pFrm );
				}
			}

			if( fLen <= fCof )
				return 1;
		}
		break;

	case e_missile_bezier3_rot:
		{
			RwV3d	v3dTrans	= { 0.f,0.f,0.f };
			RwV3d	vDir;
			RwV3dSub( &vDir, &pMissileTargetInfo->m_v3dTarget, &pMissileTargetInfo->m_v3dGP0 );
			RwV3d		vMoveMissile	=	{ 0 , m_fRadius , 0 };
			RwMatrix	MissileRotate;
			ZeroMemory( &MissileRotate , sizeof(RwMatrix) );

			RwReal	fLen	= RwV3dLength( &vDir );
			if( fLen <= fCof )
			{
				RwFrameTranslate( pFrm , &pMissileTargetInfo->m_v3dTarget, rwCOMBINEPOSTCONCAT );
				v3dTrans	= pMissileTargetInfo->m_v3dTarget;
			}
			else
			{
				RwMatrix*	pMatrix	=	RwFrameGetMatrix( pFrm );
				RwV3d*		pPos	=	RwMatrixGetPos( pMatrix );
				if( RwV3dLength( pPos ) )
				{
					*pPos	=	pMissileTargetInfo->m_v3dLastPos;					
				}

				//translate
				Eff2Ut_Bezier3( &v3dTrans, &pMissileTargetInfo->m_v3dGP0, &pMissileTargetInfo->m_v3dGP1, &pMissileTargetInfo->m_v3dTarget, fCof / fLen );
				RwFrameTranslate( pFrm , &v3dTrans, rwCOMBINEPOSTCONCAT );

				pMissileTargetInfo->m_v3dLastPos	=	*RwMatrixGetPos( pMatrix );

				// Missile Rotate
				RwMatrixRotate( &MissileRotate , &vDir , m_fRotate*fPastTime , rwCOMBINEREPLACE );
				RwV3dTransformPoint( &vMoveMissile , &vMoveMissile , &MissileRotate );
				RwFrameTranslate( pFrm , &vMoveMissile , rwCOMBINEPOSTCONCAT );

			}

			//rotate : 오브젝트인경우만..
			if( AgcdEffBase::E_EFFBASE_OBJECT == pEffCtrlBase->GetBaseType() )
			{
				AgcdEffCtrl_Obj* pEffCtrlObj = static_cast<AgcdEffCtrl_Obj*>(pEffCtrlBase);
				if( pEffCtrlObj )
				{
					RwV3d	fromto = { vDir.x, vDir.y, vDir.z };
					RwV3dNormalize( &fromto, &fromto );
					RwReal	fDot = RwV3dDotProduct ( &fromto, AXISWORLD::bGetInst().bGetPtrWorldZ() );
					Eff2Ut_CLAMP( fDot, -1.f, 1.f );
					RwReal	fDeg = DEF_R2D( acosf(fDot) );
					if( vDir.x < 0.f )
						fDeg	= -fDeg;

					RwFrameRotate( pEffCtrlObj->GetPtrFrmClump(), AXISWORLD::bGetInst().bGetPtrWorldY(), fDeg, rwCOMBINEREPLACE );
					pMissileTargetInfo->m_v3dLastPt = v3dTrans;
				}
			}

			if( fLen <= fCof )
				return 1;
		}
		break;

	case e_missile_zigzag:
		//{
		//	RwV3d	vDir;
		//	RwV3dSub( &vDir, &pMissileTargetInfo->m_v3dTarget, &pMissileTargetInfo->m_v3dGP0 );
		//	RwV3d	fromto = { vDir.x, vDir.y, vDir.z };
		//	RwReal	fLen	= RwV3dLength( &vDir );
		//	RwV3d	vMoveMissile	=	{ 0 , 0 , 0 };
		//	RwMatrix MissileRotate;
		//	ZeroMemory( &MissileRotate , sizeof(RwMatrix) );

		//	if( fLen <= fCof )
		//	{
		//		RwFrameTranslate( pFrm, &pMissileTargetInfo->m_v3dTarget, rwCOMBINEPOSTCONCAT );

		//	}
		//	else
		//	{			
		//		RwMatrix*	pMatrix		=	RwFrameGetMatrix( pFrm );
		//		RwV3d*		pPos		=	RwMatrixGetPos( pMatrix );
		//		RwV3d		pOffset		=	{ m_fRadius , 0 , m_fZigzagLength };

		//		if( RwV3dLength( pPos ) )
		//		{
		//			*pPos	=	pMissileTargetInfo->m_v3dLastPt;					
		//		}

		//		//translate
		//		RwV3dNormalize( &vDir, &vDir );

		//		RwV3d

		//		RwV3dScale( &vDir, &vDir, fCof );
		//		RwV3dAdd( &vDir, &vDir, &pMissileTargetInfo->m_v3dGP0 );
		//		RwFrameTranslate( pFrm, &vDir, rwCOMBINEPOSTCONCAT );

		//		pMissileTargetInfo->m_v3dLastPt	=	*RwMatrixGetPos( pMatrix );
		//	
		//		// Missile Rotate
		//		RwMatrixRotate( &MissileRotate , &vDir , m_fRotate*fPastTime , rwCOMBINEREPLACE );

		//		RwV3dTransformPoint( &vMoveMissile , &vMoveMissile , &MissileRotate );
		//		RwFrameTranslate( pFrm , &vMoveMissile , rwCOMBINEPOSTCONCAT );

		//		pMissileTargetInfo->m_matRotate	=	MissileRotate;

		//	}

		//	//rotate : 오브젝트인경우만..
		//	if( pEffCtrlBase->GetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT )
		//	{
		//		AgcdEffCtrl_Obj* pEffCtrlObj = static_cast<AgcdEffCtrl_Obj*>(pEffCtrlBase);
		//		if( pEffCtrlObj )
		//		{
		//			RwBool	bRotToUp	= FALSE;
		//			RwReal	fDeg_ToUp	= 0.f;
		//			if( ::fabs( fromto.y ) > 0.0001f )
		//			{
		//				bRotToUp	= TRUE;
		//				RwV3d	tempFromTo	= fromto;
		//				fromto.y	= 0.f;

		//				RwV3dNormalize( &tempFromTo, &tempFromTo );
		//				RwV3dNormalize( &fromto, &fromto );

		//				RwReal fDot	= RwV3dDotProduct( &tempFromTo, &fromto );
		//				Eff2Ut_CLAMP( fDot, -1.f, 1.f );
		//				fDeg_ToUp	= DEF_R2D( acosf( fDot ) );

		//				if( tempFromTo.y > 0.f )
		//					fDeg_ToUp = -fDeg_ToUp;
		//			}
		//			else
		//				RwV3dNormalize( &fromto, &fromto );

		//			RwReal	fDot = RwV3dDotProduct ( &fromto, AXISWORLD::bGetInst().bGetPtrWorldZ() );

		//			Eff2Ut_CLAMP( fDot, -1.f, 1.f );
		//			RwReal	fDeg = DEF_R2D( acosf(fDot) );

		//			if( fromto.x < 0.f )
		//				fDeg	= -fDeg;

		//			pFrm = pEffCtrlObj->GetPtrFrmClump();
		//			pModelling = RwFrameGetMatrix(pFrm);

		//			RwMatrixRotate( pModelling, AXISWORLD::bGetInst().bGetPtrWorldY(), fDeg, rwCOMBINEREPLACE );
		//			if( bRotToUp )
		//				RwMatrixRotate( pModelling, &RwFrameGetLTM(pEffCtrlObj->GetPtrFrmClump())->right, fDeg_ToUp, rwCOMBINEPOSTCONCAT );

		//			RwFrameUpdateObjects( pFrm );
		//		}
		//	}

		//	if( fLen <= fCof )
		//		return 1;
		//}
		//break;
		break;
	}

	pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_TRANS | AgcdEffCtrl_Base::E_UPDATEFLAG_ROT );

	return 0;
}

RwInt32 AgcdEffAnim_Missile::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	fwrite(&m_fSpeed0, sizeof(m_fSpeed0) + sizeof(m_fAccel), 1, fp );
	ir += sizeof(m_fSpeed0) + sizeof(m_fAccel);
	return ir;
};

RwInt32 AgcdEffAnim_Missile::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);
	fread(&m_fSpeed0, sizeof(m_fSpeed0) + sizeof(m_fAccel), 1, fp );
	ir += sizeof(m_fSpeed0) + sizeof(m_fAccel);
	return ir;
};

//--------------------- AgcdEffAnim_Linear -----------------
AgcdEffAnim_Linear::AgcdEffAnim_Linear( RwUInt32 dwBitFlags, RwInt32 nSize ) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_LINEAR, dwBitFlags ),
 m_stTblPos( nSize )
{
	EFFMEMORYLOG_CON;
}

AgcdEffAnim_Linear::~AgcdEffAnim_Linear()
{
	EFFMEMORYLOG_DES;
}

BOOL AgcdEffAnim_Linear::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	if ( !pOut )
		return FALSE;

	return m_stTblPos.bGetValIntp( static_cast<RwV3d*>(pOut), dwCurrTime );
}

BOOL AgcdEffAnim_Linear::bSetTVal( RwInt32 nIndex, void* pIn, RwUInt32 dwCurrTime )
{
	if( !pIn )
		return FALSE;

	return m_stTblPos.bSetVal( nIndex, *( static_cast<RwV3d*>(pIn) ), dwCurrTime );
}

BOOL AgcdEffAnim_Linear::bInsTVal( void* pIn, RwUInt32 dwTime )
{
	if( !pIn )
		return FALSE;
	
#ifdef USE_MFC
	BOOL br = m_stTblPos.bInsert( dwTime, *( static_cast<RwV3d*>(pIn) ) );
	if( br ){
		m_stTblPos.bGetLastTime(&m_dwLifeTime);
	}
	return br;
#else
	return m_stTblPos.bInsert( dwTime, *( static_cast<RwV3d*>(pIn) ) );
#endif//USE_MFC
}

BOOL AgcdEffAnim_Linear::bDelTVal( RwUInt32 dwTime )
{
	return m_stTblPos.bDelete( dwTime );
}

RwInt32 AgcdEffAnim_Linear::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32 ulFlagReserved )
{
	RwUInt32	ulKeyTime	= 0LU;
	RwInt32		ir	= ::CalcKeyTime( &ulKeyTime, m_eLoopOpt, dwAccumulateTime, m_dwLifeTime );

	if( ir )
	{
		if( T_ISMINUS4( ir ) )
			return -1;

		return ir;
	}
	
	RwV3d	v3dTrans;
	m_stTblPos.bGetValIntp( &v3dTrans, ulKeyTime );

	if( pEffCtrlBase->GetCPtrEffCtrl_Set()->FlagChk( FLAG_EFFCTRLSET_SCALE ) )
	{
		RwV3dScale( &v3dTrans, &v3dTrans, pEffCtrlBase->GetCPtrEffCtrl_Set()->GetScale() );
	}
	
	pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_TRANS );

	//이건 3차클베끝나고 데이터수정을 하자.
	if( (AgcdEffBase::E_EFFBASE_OBJECT == pEffCtrlBase->GetBaseType()) && 
		!DEF_FLAG_CHK(pEffCtrlBase->GetCPtrEffBase()->bGetFlag(), FLAG_EFFBASEOBJ_DUMMY) )
	{
		RwV3d	vdiff;
		RwV3dSub( &vdiff, &v3dTrans, RwMatrixGetPos( RwFrameGetMatrix( RpClumpGetFrame( pEffCtrlBase->GetPtrClump() ) ) ) );
		RwFrameTranslate( RpClumpGetFrame( pEffCtrlBase->GetPtrClump() ), &vdiff, rwCOMBINEPOSTCONCAT );
	}
	else
		RwFrameTranslate( pEffCtrlBase->GetPtrFrm(), &v3dTrans, rwCOMBINEPOSTCONCAT );

	return 0;
}

RwInt32 AgcdEffAnim_Linear::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	ir += m_stTblPos.bToFile(fp);
	return ir;
};

RwInt32 AgcdEffAnim_Linear::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);
	ir += m_stTblPos.bFromFile(fp);
	return ir;
};


//--------------------- AgcdEffAnim_Rev -----------------
AgcdEffAnim_Rev::AgcdEffAnim_Rev(RwUInt32 dwBitFlags, RwInt32 nSize, const RwV3d* pV3dAxis) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_REVOLUTION, dwBitFlags ),
 m_stTblRev( nSize )
{
	EFFMEMORYLOG_CON;

	if( pV3dAxis )
	{
		RwReal	fLen = RwV3dNormalize( &m_v3dRotAxis, pV3dAxis );
		if( fLen == 0.f )
			m_eLoopOpt	= e_TblDir_end;

		if( m_v3dRotAxis.x == 0.f && m_v3dRotAxis.z == 0.f )
		{
			m_v3dRotSide	= AXISWX;
		}
		else
		{
			RwV3dCrossProduct( &m_v3dRotSide, &AXISWY, &m_v3dRotAxis );
			fLen = RwV3dNormalize( &m_v3dRotSide, &m_v3dRotSide );
			ASSERT( fLen != 0.f );
			if( fLen == 0.f )
			{
				Eff2Ut_ERR( "RwV3dNormalize failed @ AgcdEffAnim_Rev::CONSTRUCTER" );
				m_eLoopOpt	= e_TblDir_end;
			}
		}
	}
	else
	{
		m_v3dRotAxis	= AXISWY;
		m_v3dRotSide	= AXISWX;
	}
}

AgcdEffAnim_Rev::~AgcdEffAnim_Rev()
{
	EFFMEMORYLOG_DES;
}

BOOL AgcdEffAnim_Rev::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	if ( !pOut )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Rev::bGetTVal failed : pOut == NULL" );
		return FALSE;
	}

	STREVOLUTION	r1, r2;
	RwUInt32		t1, t2;
	BOOL br	= m_stTblRev.bGetLowerBound( &t1, &r1, &t2, &r2, dwCurrTime );
	if( t1 == t2 )
	{
		*( static_cast<LPSTREVOLUTION>(pOut) ) = r1;
	}
	else
	{
		LPSTREVOLUTION	pr	= static_cast<LPSTREVOLUTION>(pOut);
		RwReal	cof	= static_cast<RwReal>(dwCurrTime - t1) / static_cast<RwReal>(t2-t1);
		LinearIntp( &pr->m_fAngle	, &r1.m_fAngle , &r2.m_fAngle , cof );
		LinearIntp( &pr->m_fHeight	, &r1.m_fHeight, &r2.m_fHeight, cof );
		LinearIntp( &pr->m_fRadius	, &r1.m_fRadius, &r2.m_fRadius, cof );
	}

	return br;
}

BOOL AgcdEffAnim_Rev::bSetTVal( RwInt32 nIndex, void* pIn, RwUInt32 dwCurrTime )
{
	if( !pIn )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Rev::bSetTVal failed : pIn == NULL" );
		return FALSE;
	}

	return m_stTblRev.bSetVal( nIndex, *( static_cast<LPSTREVOLUTION>(pIn) ), dwCurrTime );
}

BOOL AgcdEffAnim_Rev::bInsTVal( void* pIn, RwUInt32 dwTime )
{
	if( !pIn )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Rev::bInsTVal failed : pIn == NULL" );
		return FALSE;
	}
	
#ifdef USE_MFC
	BOOL br = m_stTblRev.bInsert( dwTime, *( static_cast<LPSTREVOLUTION>(pIn) ) );
	if( br ){
		m_stTblRev.bGetLastTime(&m_dwLifeTime);
	}
	return br;
#else
	return m_stTblRev.bInsert( dwTime, *( static_cast<LPSTREVOLUTION>(pIn) ) );
#endif//USE_MFC
}

BOOL AgcdEffAnim_Rev::bDelTVal( RwUInt32 dwTime )
{
	return m_stTblRev.bDelete( dwTime );
}

RwInt32 AgcdEffAnim_Rev::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32 ulFlagReserved )
{
	ASSERT( pEffCtrlBase );
	RwUInt32	ulKeyTime	= 0LU;
	RwInt32		ir	= ::CalcKeyTime( &ulKeyTime, m_eLoopOpt, dwAccumulateTime, m_dwLifeTime );
	ASSERT( !T_ISMINUS4( ir ) );
	if( ir ){
		if( T_ISMINUS4( ir ) ){
			Eff2Ut_ERR( "CalcKeyTime failed @ AgcdEffAnim_Rev::bUpdateVal" );
			return -1;
		}
		return ir;
	}

	STREVOLUTION	stRev;
	bGetTVal( static_cast<LPVOID>( &stRev ), ulKeyTime );
	if( pEffCtrlBase->GetCPtrEffCtrl_Set()->FlagChk( FLAG_EFFCTRLSET_SCALE ) )
	{
		stRev.m_fHeight	*= pEffCtrlBase->GetCPtrEffCtrl_Set()->GetScale();
		stRev.m_fRadius	*= pEffCtrlBase->GetCPtrEffCtrl_Set()->GetScale();
	}

	RwV3d	v3dTrans;
	RwV3dScale( &v3dTrans, &m_v3dRotSide, stRev.m_fRadius );
	RwMatrix	matRot;
	RwMatrixRotate( &matRot, &m_v3dRotAxis, stRev.m_fAngle*dwAccumulateTime*0.001f, rwCOMBINEREPLACE );
	RwV3dTransformVector( &v3dTrans, &v3dTrans, &matRot );

	RwV3d	v3dY	= { 0.f, 1.f, 0.f };
	RwV3dScale( &v3dY, &m_v3dRotAxis, stRev.m_fHeight );

	RwV3dAdd( &v3dTrans, &v3dTrans, &v3dY );
		
	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();

	RwFrame*	pFrame = pEffCtrlBase->GetPtrFrm();
	RwMatrix*	pModelling = RwFrameGetMatrix( pFrame );

	pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_TRANS );
	RwMatrixTranslate ( pModelling , &v3dTrans, rwCOMBINEPOSTCONCAT );

	if( bCheckFlag(FLAG_EFFANIM_REV_ROTATE) && 
		!pEffCtrlBase->GetCPtrEffBase()->bFlagChk( FLAG_EFFBASE_BILLBOARD | FLAG_EFFBASE_BILLBOARDY ) )
	{
		pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_ROT );
		RwMatrixTransform( pModelling , &matRot, rwCOMBINEPRECONCAT );
	}

	RwFrameUpdateObjects( pFrame );

	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();

	return 0;
}

RwInt32 AgcdEffAnim_Rev::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	fwrite(&m_v3dRotAxis, sizeof(m_v3dRotAxis), 1, fp);
	fwrite(&m_v3dRotSide, sizeof(m_v3dRotSide), 1, fp);
	ir += sizeof(m_v3dRotAxis) + sizeof(m_v3dRotSide);
	ir += m_stTblRev.bToFile(fp);
	return ir;
};

RwInt32 AgcdEffAnim_Rev::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);
	fread(&m_v3dRotAxis, sizeof(m_v3dRotAxis), 1, fp);
	fread(&m_v3dRotSide, sizeof(m_v3dRotSide), 1, fp);
	ir += sizeof(m_v3dRotAxis) + sizeof(m_v3dRotSide);
	ir += m_stTblRev.bFromFile(fp);
	return ir;
};

//--------------------- AgcdEffAnim_Rot -----------------
AgcdEffAnim_Rot::AgcdEffAnim_Rot(RwUInt32 dwBitFlags, RwInt32 nSize, const RwV3d* pV3dRotAxis ) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_ROTATION, dwBitFlags ),
 m_stTblDeg( nSize )
{
	EFFMEMORYLOG_CON;

	m_v3dRotAxis	= pV3dRotAxis ? *pV3dRotAxis : AXISWZ;
}

AgcdEffAnim_Rot::~AgcdEffAnim_Rot()
{
	EFFMEMORYLOG_DES;
}

BOOL AgcdEffAnim_Rot::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	if ( !pOut )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Rot::bGetTVal failed : pOut == NULL" );
		return FALSE;
	}

	return m_stTblDeg.bGetValIntp( static_cast<RwReal*>(pOut), dwCurrTime );
}

BOOL AgcdEffAnim_Rot::bSetTVal( RwInt32 nIndex, void* pIn, RwUInt32 dwCurrTime )
{
	if( !pIn )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Rot::bSetTVal failed : pIn == NULL" );
		return FALSE;
	}

	return m_stTblDeg.bSetVal( nIndex, *( static_cast<RwReal*>(pIn) ), dwCurrTime );
}

BOOL AgcdEffAnim_Rot::bInsTVal( void* pIn, RwUInt32 dwTime )
{
	if( !pIn )
	{
		Eff2Ut_ERR( "AgcdEffAnim_Rot::bInsTVal failed : pIn == NULL" );
		return FALSE;
	}
	
#ifdef USE_MFC
	BOOL br = m_stTblDeg.bInsert( dwTime, *( static_cast<RwReal*>(pIn) ) );
	if( br )
		m_stTblDeg.bGetLastTime(&m_dwLifeTime);

	return br;
#else
	return m_stTblDeg.bInsert( dwTime, *( static_cast<RwReal*>(pIn) ) );
#endif//USE_MFC
}

BOOL AgcdEffAnim_Rot::bDelTVal( RwUInt32 dwTime )
{
	return m_stTblDeg.bDelete( dwTime );
}

RwInt32 AgcdEffAnim_Rot::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32 ulFlagReserved )
{
	ASSERT( pEffCtrlBase );
	RwUInt32	ulKeyTime	= 0LU;
	RwInt32		ir	= ::CalcKeyTime( &ulKeyTime, m_eLoopOpt, dwAccumulateTime, m_dwLifeTime );
	ASSERT( !T_ISMINUS4( ir ) );
	if( ir ){
		if( T_ISMINUS4( ir ) ){
			Eff2Ut_ERR( "CalcKeyTime failed @ AgcdEffAnim_Rot::bUpdateVal" );
			return -1;
		}
		return ir;
	}

	RwReal	fDeg = 0.f;
	m_stTblDeg.bGetValIntp( &fDeg, ulKeyTime );

	//0.03030303f = 1.f/33.f
	//0.00091827 = 0.03030303f*0.03030303f
	//0.91827 = 0.00091827 * 1000
	//이전 데이터 유지를 위해서.
	AgcdEffGlobal	&EffGlobal = AgcdEffGlobal::bGetInst();
	RwReal			fDiffTimes = EffGlobal.bGetDiffTimeS();
	fDeg	= fDeg * fDiffTimes * 0.91827f;
	pEffCtrlBase->AddRotAngle( fDeg );

	pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_ROT );

	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();

	//---->점더 생각해보자.
	//clump를 건드리지 않고 하는 방법은 없을까?
	//시간으로 제어하는 방법으로 바꾸어야 하는데.
	if( (AgcdEffBase::E_EFFBASE_OBJECT == pEffCtrlBase->GetBaseType()) &&
		!DEF_FLAG_CHK(pEffCtrlBase->GetCPtrEffBase()->bGetFlag(), FLAG_EFFBASEOBJ_DUMMY) )
	{
		//@{ 2006/05/02 burumal
		//RwFrame*	pFrmClump	= RpClumpGetFrame( pEffCtrlBase->bGetPtrClump() );		
		RwFrame*	pFrmClump	= pEffCtrlBase->GetPtrClump() ? RpClumpGetFrame(pEffCtrlBase->GetPtrClump()) : NULL;
		ASSERT(pFrmClump);
		if ( !pFrmClump )
		{
			MD_SetErrorMessage( "AgcdEffAnim_Rot::bUpdateVal pEffCtrlBase->bGetPtrClump() is invalid!! \n" );
			return -1;
		}
		//@}
		
		RwMatrix*	pModelling	= RwFrameGetMatrix( pFrmClump );
		RwV3d		v3dPos;
		RwV3dNegate ( &v3dPos, RwMatrixGetPos( pModelling ) );
		RwMatrixTranslate ( pModelling , &v3dPos, rwCOMBINEPOSTCONCAT );
		RwMatrixRotate ( pModelling , &m_v3dRotAxis	, fDeg , rwCOMBINEPOSTCONCAT );
		RwV3dNegate ( &v3dPos, &v3dPos );
		RwMatrixTranslate ( pModelling , &v3dPos, rwCOMBINEPOSTCONCAT );
		
		RwFrameUpdateObjects( pFrmClump );
	}
	else
	{
		//@{ 2006/05/12 burumal
		if ( !pEffCtrlBase->GetPtrFrm() )
		{
			MD_SetErrorMessage( "AgcdEffAnim_Rot::bUpdateVal pEffCtrlBase->bGetPtrClump() is invalid!! \n" );
			return -1;
		}
		//@}

		RwFrameRotate( pEffCtrlBase->GetPtrFrm()
			, &m_v3dRotAxis
			, pEffCtrlBase->GetRotAngle()
			, rwCOMBINEPRECONCAT
			);
	}

	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
	//<----

	return 0;
}

RwInt32 AgcdEffAnim_Rot::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	fwrite(&m_v3dRotAxis, sizeof(m_v3dRotAxis), 1, fp);
	ir += sizeof(m_v3dRotAxis);
	ir += m_stTblDeg.bToFile(fp);
	return ir;
};

RwInt32 AgcdEffAnim_Rot::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);
	fread(&m_v3dRotAxis, sizeof(m_v3dRotAxis), 1, fp);
	ir += sizeof(m_v3dRotAxis);
	ir += m_stTblDeg.bFromFile(fp);
	return ir;
};

//--------------------- AgcdEffAnim_RpSpline -----------------
AgcdEffAnim_RpSpline::AgcdEffAnim_RpSpline(RwUInt32 dwBitFlags) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_RPSPLINE, dwBitFlags ),
 m_pRpSpline( NULL )
{
	EFFMEMORYLOG_CON;

	Eff2Ut_ZEROBLOCK( m_szSplineFName );
}

AgcdEffAnim_RpSpline::~AgcdEffAnim_RpSpline()
{
	EFFMEMORYLOG_DES;

	Eff2Ut_SAFE_DESTROY_SPLINE( m_pRpSpline );
}

RwInt32 AgcdEffAnim_RpSpline::bSetSplineFName(const RwChar* szSplineFName)
{
	if( szSplineFName )
	{
		RwChar	szFullPath[MAX_PATH]	= "";

		strcpy( m_szSplineFName, szSplineFName );
		Eff2Ut_SAFE_DESTROY_SPLINE( m_pRpSpline );
		
		strcpy( szFullPath, AgcuEffPath::GetPath_Anim() );
		strcat( szFullPath, m_szSplineFName );

		m_pRpSpline	= AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->LoadRpSpline(szFullPath);
		if( !m_pRpSpline )
			return -2;
	}
	else
		return -1;

	return 0;
}

BOOL AgcdEffAnim_RpSpline::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	if ( !pOut )
		return FALSE;

	if( dwCurrTime > AgcdEffAnim::m_dwLifeTime )
		return FALSE;

	if( m_pRpSpline )
	{
		RwReal fr = RpSplineFindMatrix( m_pRpSpline, rpSPLINEPATHSMOOTH, dwCurrTime * m_fIvsLife, NULL, static_cast<RwMatrix*>(pOut) );
		if( fr == -1.f )
			return FALSE;
	}
	else
		return FALSE;

	return TRUE;
}

RwInt32 AgcdEffAnim_RpSpline::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32 ulFlagReserved )
{
	RwUInt32	ulKeyTime	= 0LU;
	RwInt32		ir	= ::CalcKeyTime( &ulKeyTime, m_eLoopOpt, dwAccumulateTime, m_dwLifeTime );

	if( ir )
	{
		if( T_ISMINUS4( ir ) )
			return -1;

		return ir;
	}

	RwMatrix	matSpline;
	bGetTVal( static_cast<LPVOID>(&matSpline), ulKeyTime );
	
	RwMatrix	matRot, matTrans;
	RwMatrixSetIdentity( &matRot );
	RwMatrixSetIdentity( &matTrans );
	Eff2Ut_RwMatrixGetRot( matSpline, &matRot );
	Eff2Ut_RwMatrixGetTrans( matSpline, &matTrans );

	if( pEffCtrlBase->GetCPtrEffCtrl_Set()->FlagChk( FLAG_EFFCTRLSET_SCALE ) )
		RwV3dScale( RwMatrixGetPos( &matTrans ), RwMatrixGetPos( &matTrans ), pEffCtrlBase->GetCPtrEffCtrl_Set()->GetScale() );

	if( pEffCtrlBase->GetCPtrEffBase()->bFlagChk( FLAG_EFFBASE_BILLBOARD | FLAG_EFFBASE_BILLBOARDY ) )
	{
		pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_TRANS );
		RwFrameTransform( pEffCtrlBase->GetPtrFrm(), &matTrans, rwCOMBINEPOSTCONCAT );
	}
	else
	{
		pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_ROT | AgcdEffCtrl_Base::E_UPDATEFLAG_TRANS );

		RwFrame*	pFrame = pEffCtrlBase->GetPtrFrm();
		RwMatrix*	pModelling = RwFrameGetMatrix( pFrame );

		RwMatrixTransform( pModelling , &matRot, rwCOMBINEPRECONCAT );
		RwMatrixTransform( pModelling , &matTrans, rwCOMBINEPOSTCONCAT );

		RwFrameUpdateObjects( pFrame );
	}

	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();

	return 0;
}

RwInt32 AgcdEffAnim_RpSpline::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	fwrite(&m_szSplineFName, sizeof(m_szSplineFName), 1, fp);
	ir += sizeof(m_szSplineFName);
	return ir;
};

RwInt32 AgcdEffAnim_RpSpline::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);

	if( m_dwLifeTime )
		m_fIvsLife = 1.f/(RwReal)m_dwLifeTime;

	fread(&m_szSplineFName, sizeof(m_szSplineFName), 1, fp);
	ir += sizeof(m_szSplineFName);

	if( T_ISMINUS4(bSetSplineFName(m_szSplineFName)) )
	{
		return -1;
	}

	return ir;
};

//--------------------- AgcdEffAnim_RtAnim -----------------
AgcdEffAnim_RtAnim::AgcdEffAnim_RtAnim(RwUInt32 dwBitFlags) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_RTANIM, dwBitFlags ),
 m_pRtAnim( NULL )
{
	EFFMEMORYLOG_CON;

	Eff2Ut_ZEROBLOCK( m_szAnimFName );
}

AgcdEffAnim_RtAnim::~AgcdEffAnim_RtAnim()
{
	EFFMEMORYLOG_DES;

	Eff2Ut_SAFE_DESTROY_RTANIM( m_pRtAnim );
}

RwInt32 AgcdEffAnim_RtAnim::bSetAnimFName( const RwChar* szAnimFName )
{
	if( szAnimFName )
	{
		strcpy( m_szAnimFName, szAnimFName );
		Eff2Ut_SAFE_DESTROY_RTANIM( m_pRtAnim );
		
		RwChar	szFullPath[MAX_PATH]	= "";
		strcpy( szFullPath, AgcuEffPath::GetPath_Anim() );
		strcat( szFullPath, szAnimFName );
		m_pRtAnim = AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->LoadRtAnim(szFullPath);

		if( !m_pRtAnim )
			return -2;
	}
	else
		return -1;

	return 0;
}

BOOL AgcdEffAnim_RtAnim::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	return TRUE;
}

RwInt32 AgcdEffAnim_RtAnim::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32 ulFlagReserved )
{
	RwUInt32	ulKeyTime	= 0LU;
	RwInt32		ir	= ::CalcKeyTime( &ulKeyTime, m_eLoopOpt, dwAccumulateTime, m_dwLifeTime );

	if( ir )
	{
		if( T_ISMINUS4( ir ) )
			return -1;

		return ir;
	}

	if( AgcdEffBase::E_EFFBASE_OBJECT != pEffCtrlBase->GetBaseType() )
		return -1;

	RpHAnimHierarchy *pHierarchy = RpHAnimGetHierarchy(RpClumpGetFrame(pEffCtrlBase->GetPtrClump()));
	if( !pHierarchy )
	{
		pHierarchy = Eff2Ut_GetHierarchyFromRwFrame( RpClumpGetFrame(pEffCtrlBase->GetPtrClump()) );
		if( !pHierarchy )
			return -1;

	}

	RwReal	fCurrTime	= dwAccumulateTime*0.001f;//dwKeyTime * 0.001f;
	if( !RpHAnimHierarchySetCurrentAnimTime( pHierarchy, fCurrTime ) )
		return -1;

	//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
	if( !RpHAnimUpdateHierarchyMatrices(pHierarchy) )
		return -1;

	pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_RTANIM );

	return 0;
}

RwInt32 AgcdEffAnim_RtAnim::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	fwrite(&m_szAnimFName, sizeof(m_szAnimFName), 1, fp);
	ir += sizeof(m_szAnimFName);
	return ir;
};

RwInt32 AgcdEffAnim_RtAnim::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);
	fread(&m_szAnimFName, sizeof(m_szAnimFName), 1, fp);
	ir += sizeof(m_szAnimFName);
	if( T_ISMINUS4(bSetAnimFName(m_szAnimFName)) )
	{
		return -1;
	};

	return ir;
};

//--------------------- AgcdEffAnim_Scale -----------------
AgcdEffAnim_Scale::AgcdEffAnim_Scale(RwUInt32 dwBitFlags, RwInt32 nSize) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_SCALE, dwBitFlags ),
 m_stTblScale( nSize )
{
	EFFMEMORYLOG_CON;
}

AgcdEffAnim_Scale::~AgcdEffAnim_Scale()
{
	EFFMEMORYLOG_DES;
}

BOOL AgcdEffAnim_Scale::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	if ( !pOut )
		return FALSE;

	return m_stTblScale.bGetValIntp( static_cast<RwV3d*>(pOut), dwCurrTime );
}

BOOL AgcdEffAnim_Scale::bSetTVal( RwInt32 nIndex, void* pIn, RwUInt32 dwCurrTime )
{
	if( !pIn )
		return FALSE;

	return m_stTblScale.bSetVal( nIndex, *( static_cast<RwV3d*>(pIn) ), dwCurrTime );
}

BOOL AgcdEffAnim_Scale::bInsTVal( void* pIn, RwUInt32 dwTime )
{
	if( !pIn )
		return FALSE;
	
#ifdef USE_MFC
	BOOL br = m_stTblScale.bInsert( dwTime, *( static_cast<RwV3d*>(pIn) ) );
	if( br )
		m_stTblScale.bGetLastTime(&m_dwLifeTime);
	return br;
#else
	return m_stTblScale.bInsert( dwTime, *( static_cast<RwV3d*>(pIn) ) );
#endif//USE_MFC
}

BOOL AgcdEffAnim_Scale::bDelTVal( RwUInt32 dwTime )
{
	return m_stTblScale.bDelete( dwTime );
}

RwInt32 AgcdEffAnim_Scale::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32	ulFlagReserved )
{
	RwUInt32	ulKeyTime	= 0LU;
	RwInt32		ir	= ::CalcKeyTime( &ulKeyTime, m_eLoopOpt, dwAccumulateTime, m_dwLifeTime );

	if( ir )
	{
		if( T_ISMINUS4( ir ) )
			return -1;

		return ir;
	}

	RwV3d	vscale = { 1.f, 1.f, 1.f };
	m_stTblScale.bGetValIntp( &vscale, ulKeyTime );

	if( AgcdEffBase::E_EFFBASE_LIGHT == pEffCtrlBase->GetBaseType() )
	{
		static_cast<AgcdEffCtrl_Light*>(pEffCtrlBase)->SetLightRadius(vscale.x*LIGHTRADIUSRATION);
	}
	else
	{
		pEffCtrlBase->UpdateFlagOn( AgcdEffCtrl_Base::E_UPDATEFLAG_SCALE );
		RwFrameScale( pEffCtrlBase->GetPtrFrm(), &vscale, rwCOMBINEPRECONCAT );
	}

	return 0;
}

RwInt32 AgcdEffAnim_Scale::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	ir += m_stTblScale.bToFile(fp);
	return ir;
};

RwInt32 AgcdEffAnim_Scale::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);
	ir += m_stTblScale.bFromFile(fp);
	return ir;
};

//--------------------- AgcdEffAnim_ParticlePosScale -----------------
AgcdEffAnim_ParticlePosScale::AgcdEffAnim_ParticlePosScale(RwUInt32 dwBitFlags, RwInt32 nSize) : AgcdEffAnim( AgcdEffAnim::E_EFFANIM_PARTICLEPOSSCALE, dwBitFlags ),
 m_stTblScale( nSize )
{
	EFFMEMORYLOG_CON;
}

AgcdEffAnim_ParticlePosScale::~AgcdEffAnim_ParticlePosScale()
{
	EFFMEMORYLOG_DES;
}

BOOL AgcdEffAnim_ParticlePosScale::bGetTVal( void* pOut, RwUInt32 dwCurrTime ) const
{
	if ( !pOut )
		return FALSE;

	return m_stTblScale.bGetValIntp( static_cast<RwReal*>(pOut), dwCurrTime );
}

BOOL AgcdEffAnim_ParticlePosScale::bSetTVal( RwInt32 nIndex, void* pIn, RwUInt32 dwCurrTime )
{
	if( !pIn )
		return FALSE;

	return m_stTblScale.bSetVal( nIndex, *( static_cast<RwReal*>(pIn) ), dwCurrTime );
}

//-----------------------------------------------------------------------------
BOOL AgcdEffAnim_ParticlePosScale::bInsTVal( void* pIn, RwUInt32 dwTime )
{
	if( !pIn )
		return FALSE;
	
#ifdef USE_MFC
	BOOL br = m_stTblScale.bInsert( dwTime, *( static_cast<RwReal*>(pIn) ) );
	if( br ){
		m_stTblScale.bGetLastTime(&m_dwLifeTime);
	}
	return br;
#else
	return m_stTblScale.bInsert( dwTime, *( static_cast<RwReal*>(pIn) ) );
#endif//USE_MFC
}

BOOL AgcdEffAnim_ParticlePosScale::bDelTVal( RwUInt32 dwTime )
{
	return m_stTblScale.bDelete( dwTime );
}

RwInt32 AgcdEffAnim_ParticlePosScale::bUpdateVal( RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffCtrlBase, RwUInt32	ulFlagReserved//=0LU
									 )
{
	ASSERT( !"doesn't need update" );
	return 0;
}

RwInt32 AgcdEffAnim_ParticlePosScale::bToFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tToFile(fp);
	ir += m_stTblScale.bToFile(fp);
	return ir;
};

RwInt32 AgcdEffAnim_ParticlePosScale::bFromFile(FILE* fp)
{
	RwInt32 ir	= AgcdEffAnim::tFromFile(fp);
	ir += m_stTblScale.bFromFile(fp);
	return ir;
};



//	*******************	AgcdEffAnim_Camera *******************
AgcdEffAnim_Camera::AgcdEffAnim_Camera( DWORD dwBitFlags /* = 0  */, INT32 nSize /* = 0 */ )
	: AgcdEffAnim( AgcdEffAnim::E_EFFANIM_CAMERA , dwBitFlags )	
{
	m_nCameraType	=	0;
	m_fCameraSpeed	=	0.0f;
	m_fRotateCount	=	0.0f;
	m_fCameraRotate	=	0.0f;
	m_fMoveLength	=	0.0f;
}

INT32	AgcdEffAnim_Camera::bUpdateVal(RwUInt32 dwAccumulateTime, AgcdEffCtrl_Base* pEffBase, RwUInt32 ulFlagReserved )
{

	switch( GetCameraType() )
	{

		//	Zoom		
	case 0:
		break;

		//	Rotate
	case 1:

		break;

		//	Origin
	case 2:

		break;
	}


	return 0;
}


//--------------------- StCreateParamEffAnim -----------------
StCreateParamEffAnim::StCreateParamEffAnim() :
 m_eEffAnimType ( AgcdEffAnim::E_EFFANIM_NUM ),
 m_dwBitFlags ( 0 ),
 m_dwLife ( 0 ),
 m_nSize ( 0 ),
 m_fSpeed ( 0.f ),
 m_fAccel ( 0.f ),
 m_fRotate( 0.f ),
 m_fRadius( 0.f ),
 m_fZigzagLength( 0.f ),
 m_pTable ( NULL )
{
	m_v3dRotAxis.x = 0.f;
	m_v3dRotAxis.y = 1.f;
	m_v3dRotAxis.z = 0.f;
	Eff2Ut_ZEROBLOCK( m_szFName );
}

StCreateParamEffAnim::~StCreateParamEffAnim()
{
}

void StCreateParamEffAnim::vZeroMemory(void)
{
	Eff2Ut_ZEROBLOCK( *this );
	m_eEffAnimType = AgcdEffAnim::E_EFFANIM_NUM ;
}

RwInt32 StCreateParamEffAnim::bSetForColr( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, void* pColrTable )
{
	vZeroMemory();

	m_eEffAnimType	= AgcdEffAnim::E_EFFANIM_COLOR;
	m_dwBitFlags	= dwFlag;
	m_dwLife		= dwLife;
	m_eLoopOpt		= eLoopOpt;
	m_nSize			= nSize;
	m_pTable		= pColrTable;

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForTuTv( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, void* pTuTvTable )
{
	vZeroMemory();

	m_eEffAnimType	= AgcdEffAnim::E_EFFANIM_TUTV;
	m_dwBitFlags	= dwFlag;
	m_dwLife		= dwLife;
	m_eLoopOpt		= eLoopOpt;
	m_nSize			= nSize;
	m_pTable		= pTuTvTable;

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForMissile( RwUInt32 dwFlag, RwUInt32 dwLife, RwReal fSpeed0, RwReal fAccel , RwReal fRotate , RwReal fRadius , RwReal fZigZagLength , RwReal fMinSpeed , RwReal fMaxSpeed )
{
	vZeroMemory();

	m_eEffAnimType	= AgcdEffAnim::E_EFFANIM_MISSILE;
	m_dwBitFlags	= dwFlag;
	m_dwLife		= dwLife;
	m_fSpeed		= fSpeed0;
	m_fAccel		= fAccel;
	m_fRotate		= fRotate;
	m_fRadius		= fRadius;
	m_fZigzagLength = fZigZagLength;
	m_fMinSpeed		= fMinSpeed;
	m_fMaxSpeed		= fMaxSpeed;

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForLinear( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, void* pPosTable )
{
	vZeroMemory();

	m_eEffAnimType	= AgcdEffAnim::E_EFFANIM_LINEAR;
	m_dwBitFlags	= dwFlag;
	m_dwLife		= dwLife;
	m_eLoopOpt		= eLoopOpt;
	m_nSize			= nSize;
	m_pTable		= pPosTable;

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForRev( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, const RwV3d& v3dAxis, void* pStRevTable )
{
	vZeroMemory();

	m_eEffAnimType	= AgcdEffAnim::E_EFFANIM_REVOLUTION;
	m_dwBitFlags	= dwFlag;
	m_dwLife		= dwLife;
	m_eLoopOpt		= eLoopOpt;
	m_nSize			= nSize;
	m_v3dRotAxis	= v3dAxis;
	m_pTable		= pStRevTable;

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForRot( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, const RwV3d& v3dAxis, void* pStAngleTable )
{
	vZeroMemory();

	m_eEffAnimType	= AgcdEffAnim::E_EFFANIM_ROTATION;
	m_dwBitFlags	= dwFlag;
	m_dwLife		= dwLife;
	m_eLoopOpt		= eLoopOpt;
	m_nSize			= nSize;
	m_v3dRotAxis	= v3dAxis;
	m_pTable		= pStAngleTable;

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForRpSpline( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, const RwChar* szSplineFName )
{
	vZeroMemory();

	m_eEffAnimType	= AgcdEffAnim::E_EFFANIM_RPSPLINE;
	m_dwBitFlags	= dwFlag;
	m_dwLife		= dwLife;
	m_eLoopOpt		= eLoopOpt;
	strcpy( m_szFName, szSplineFName );

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForRtAnim( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, const RwChar* szAnimFName )
{
	vZeroMemory();

	m_eEffAnimType	= AgcdEffAnim::E_EFFANIM_RTANIM;
	m_dwBitFlags	= dwFlag;
	m_dwLife		= dwLife;
	m_eLoopOpt		= eLoopOpt;
	strcpy( m_szFName, szAnimFName );

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForScale( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, void* pScaleTable )
{
	vZeroMemory();

	m_eEffAnimType	= AgcdEffAnim::E_EFFANIM_SCALE;
	m_dwBitFlags	= dwFlag;
	m_dwLife		= dwLife;
	m_eLoopOpt		= eLoopOpt;
	m_nSize			= nSize;
	m_pTable		= pScaleTable;

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForPostFX( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize, VOID* pFXTable )
{
	vZeroMemory();

	m_eEffAnimType	=	AgcdEffAnim::E_EFFANIM_POSTFX;
	m_dwBitFlags	=	dwFlag;
	m_dwLife		=	dwLife;
	m_eLoopOpt		=	eLoopOpt;
	m_nSize			=	nSize;
	m_pTable		=	pFXTable;

	return 0;
}

RwInt32 StCreateParamEffAnim::bSetForCamera( RwUInt32 dwFlag, RwUInt32 dwLife, E_LOOPOPT eLoopOpt, RwInt32 nSize )
{
	vZeroMemory();

	m_eEffAnimType	=	AgcdEffAnim::E_EFFANIM_CAMERA;
	m_dwBitFlags	=	dwFlag;
	m_dwLife		=	dwLife;
	m_eLoopOpt		=	eLoopOpt;
	m_nSize			=	nSize;

	return 0;
}

//--------------------- AgcuEffAnimCreater -----------------

AgcdEffAnim* AgcuEffAnimCreater::bCreate( LPSTCREATEPARAM_EFFANIM lpParam )
{
	if( !lpParam )
		return NULL;

	if( lpParam->m_eEffAnimType >= AgcdEffAnim::E_EFFANIM_NUM )
		return NULL;

	switch( lpParam->m_eEffAnimType )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR:			return vCreate_Colr( lpParam );	
	case AgcdEffAnim::E_EFFANIM_TUTV:			return vCreate_TuTv( lpParam );	
	case AgcdEffAnim::E_EFFANIM_MISSILE:		return vCreate_Missile( lpParam );
	case AgcdEffAnim::E_EFFANIM_LINEAR:			return vCreate_Linear( lpParam );
	case AgcdEffAnim::E_EFFANIM_REVOLUTION:		return vCreate_Rev( lpParam );	
	case AgcdEffAnim::E_EFFANIM_ROTATION:		return vCreate_Rot( lpParam );	
	case AgcdEffAnim::E_EFFANIM_RPSPLINE:		return vCreate_RpSpline( lpParam );
	case AgcdEffAnim::E_EFFANIM_RTANIM:			return vCreate_RtAnim( lpParam );		
	case AgcdEffAnim::E_EFFANIM_SCALE:			return vCreate_Scale( lpParam );
	case AgcdEffAnim::E_EFFANIM_POSTFX:			return vCreate_PostFX( lpParam );
	case AgcdEffAnim::E_EFFANIM_CAMERA:			return vCreate_Camera( lpParam );

	}

	return NULL;
}

void AgcuEffAnimCreater::bDestroy( AgcdEffAnim*& prEffAnim )
{
	if( prEffAnim )
	{
		if( prEffAnim->m_eEffAnimType < AgcdEffAnim::E_EFFANIM_NUM )
			DEF_SAFEDELETE( prEffAnim );
	}
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_Colr( LPSTCREATEPARAM_EFFANIM lpParam )
{	
	AgcdEffAnim_Colr* pRet = new AgcdEffAnim_Colr( lpParam->m_dwBitFlags, lpParam->m_nSize );
	if( !pRet )
		return NULL;

	if( lpParam->m_pTable )
	{
		if( !pRet->m_stTblColr.bCpyTable( lpParam->m_pTable ) )
		{
			DEF_SAFEDELETE( pRet );
			return NULL;
		}
	}

	pRet->bSetLife( lpParam->m_dwLife );
	pRet->bSetLoopOpt( lpParam->m_eLoopOpt );
	
	return (AgcdEffAnim*)pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_TuTv( LPSTCREATEPARAM_EFFANIM lpParam )
{	
	AgcdEffAnim_TuTv* pRet =  new AgcdEffAnim_TuTv( lpParam->m_dwBitFlags, lpParam->m_nSize );
	if( !pRet )
		return NULL;

	if( lpParam->m_pTable )
	{
		if( !pRet->m_stTblRect.bCpyTable( lpParam->m_pTable ) )
		{
			DEF_SAFEDELETE( pRet );
			return NULL;
		}
	}

	pRet->bSetLife( lpParam->m_dwLife );
	pRet->bSetLoopOpt( lpParam->m_eLoopOpt );

	return pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_Missile( LPSTCREATEPARAM_EFFANIM lpParam )
{	
	AgcdEffAnim_Missile* pRet =  new AgcdEffAnim_Missile( lpParam->m_dwBitFlags, lpParam->m_fSpeed, lpParam->m_fAccel , lpParam->m_fRotate , lpParam->m_fRadius , lpParam->m_fZigzagLength );
	if( !pRet )
		return NULL;

	pRet->bSetLife( lpParam->m_dwLife );
	pRet->bSetLoopOpt( lpParam->m_eLoopOpt );

	return (AgcdEffAnim*)pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_Linear	( LPSTCREATEPARAM_EFFANIM lpParam )
{
	AgcdEffAnim_Linear*	pRet = new AgcdEffAnim_Linear( lpParam->m_dwBitFlags, lpParam->m_nSize );
	if( !pRet )
		return NULL;

	if( lpParam->m_pTable )
	{
		if( !pRet->m_stTblPos.bCpyTable( lpParam->m_pTable ) )
		{
			DEF_SAFEDELETE( pRet );
			return NULL;
		}			
	}

	pRet->bSetLife( lpParam->m_dwLife );
	pRet->bSetLoopOpt( lpParam->m_eLoopOpt );

	return pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_Rev( LPSTCREATEPARAM_EFFANIM lpParam )
{	
	AgcdEffAnim_Rev* pRet = new AgcdEffAnim_Rev( lpParam->m_dwBitFlags, lpParam->m_nSize, &lpParam->m_v3dRotAxis );
	if( !pRet )
		return NULL;

	if( lpParam->m_pTable )
	{
		if( !pRet->m_stTblRev.bCpyTable( lpParam->m_pTable ) )
		{
			DEF_SAFEDELETE( pRet );
			return NULL;
		}
	}

	pRet->bSetLife( lpParam->m_dwLife );
	pRet->bSetLoopOpt( lpParam->m_eLoopOpt );

	return pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_Rot( LPSTCREATEPARAM_EFFANIM lpParam )
{	
	AgcdEffAnim_Rot* pRet = new AgcdEffAnim_Rot( lpParam->m_dwBitFlags, lpParam->m_nSize, &lpParam->m_v3dRotAxis );
	if( pRet )
	{
		if( lpParam->m_pTable )
		{
			if( !pRet->m_stTblDeg.bCpyTable( lpParam->m_pTable ) )
			{
				DEF_SAFEDELETE( pRet );
				return NULL;
			}
		}

		pRet->bSetLife( lpParam->m_dwLife );
		pRet->bSetLoopOpt( lpParam->m_eLoopOpt );
	}

	return pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_RpSpline( LPSTCREATEPARAM_EFFANIM lpParam )
{	
	AgcdEffAnim_RpSpline* pRet = new AgcdEffAnim_RpSpline( lpParam->m_dwBitFlags );
	if( pRet )
	{
		if( pRet->bSetSplineFName( lpParam->m_szFName ) )
		{
			DEF_SAFEDELETE( pRet );
			return NULL;
		}

		pRet->bSetLife( lpParam->m_dwLife );
		pRet->bSetLoopOpt( lpParam->m_eLoopOpt );
	}

	return pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_RtAnim( LPSTCREATEPARAM_EFFANIM lpParam )
{	
	AgcdEffAnim_RtAnim*	pRet = new AgcdEffAnim_RtAnim( lpParam->m_dwBitFlags );
	if( pRet )
	{
		if( pRet->bSetAnimFName( lpParam->m_szFName ) )
		{
			DEF_SAFEDELETE( pRet );
			return NULL;
		}

		pRet->bSetLife( lpParam->m_dwLife );
		pRet->bSetLoopOpt( lpParam->m_eLoopOpt );
	}

	return pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_Scale( LPSTCREATEPARAM_EFFANIM lpParam )
{	
	AgcdEffAnim_Scale* pRet = new AgcdEffAnim_Scale( lpParam->m_dwBitFlags, lpParam->m_nSize );
	if( pRet )
	{
		if( lpParam->m_pTable )
		{
			if( !pRet->m_stTblScale.bCpyTable( lpParam->m_pTable ) )
			{
				DEF_SAFEDELETE( pRet );
				return NULL;
			}
		}

		pRet->bSetLife( lpParam->m_dwLife );
		pRet->bSetLoopOpt( lpParam->m_eLoopOpt );
	}

	return pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_PostFX( LPSTCREATEPARAM_EFFANIM lpParam )
{

	AgcdEffAnim_PostFX*	pRet	=	new AgcdEffAnim_PostFX( lpParam->m_dwBitFlags , lpParam->m_nSize );

	return pRet;
}

AgcdEffAnim* AgcuEffAnimCreater::vCreate_Camera( LPSTCREATEPARAM_EFFANIM lpParam )
{
	AgcdEffAnim_Camera* pRet = new AgcdEffAnim_Camera( lpParam->m_dwBitFlags, lpParam->m_nSize );
	if( pRet )
	{
		pRet->bSetLife( lpParam->m_dwLife );
		pRet->bSetLoopOpt( lpParam->m_eLoopOpt );
	}

	return pRet;
}