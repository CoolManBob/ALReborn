// AgcuCamPathWork.cpp
// -----------------------------------------------------------------------------
//                             _____                 _____        _   _     _   _   _            _                         
//     /\                     / ____|               |  __ \      | | | |   | | | | | |          | |                        
//    /  \    __ _  ___ _   _| |      __ _ _ __ ___ | |__) | __ _| |_| |__ | | | | | | ___  _ __| | __     ___ _ __  _ __  
//   / /\ \  / _` |/ __| | | | |     / _` | '_ ` _ \|  ___/ / _` | __| '_ \| | | | | |/ _ \| '__| |/ /    / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| |_| | |____| (_| | | | | | | |    | (_| | |_| | | |  V _ V  | (_) | |  |   <  _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|\_____|\__,_|_| |_| |_|_|     \__,_|\__|_| |_|\__/ \__/ \___/|_|  |_|\_\(_) \___| .__/| .__/ 
//            __/ |                                                                                           | |   | |    
//           |___/                                                                                            |_|   |_|    
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 01/19/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcuEffUtil.h"
#include "AgcuCamPathWork.h"
#include "AcuFrameMemory.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

// =============================================================================
// AgcuPathWork
// =============================================================================
// -----------------------------------------------------------------------------
AgcuPathWork::AgcuPathWork(eWorkType eType, RwUInt32 ulLife)
	: m_eType(eType)
	, m_ulLife(ulLife)
	, m_ulAccum(0LU)
{
	m_unionBaseFlag.st.m_1ulState	= 0;
	m_unionBaseFlag.st.m_1ulForward	= 0;
	m_unionBaseFlag.st.m_1ulLoop	= 0;
}

AgcuPathWork::~AgcuPathWork()
{
}

// -----------------------------------------------------------------------------
//AgcuPathWork::~AgcuPathWork()
//{
//}
// -----------------------------------------------------------------------------
void AgcuPathWork::bReset(void)
{
	m_unionBaseFlag.st.m_1ulState	= 0;
	m_ulAccum	= 0LU;
}
// -----------------------------------------------------------------------------
void AgcuPathWork::bForward(void)
{
	m_unionBaseFlag.st.m_1ulForward = 1;
	// 2004/02/14 마고자
	// bBackword 도 이걸 0으로 .. bForward도 이걸 0 으로..
	// 어느 쪽으로 해도 카메라 워크가 거꾸로 되던데요 ;
	// 이거 왜바꾸셨어요? -_- 일단 자리에 안계셔서 제 맘대로 바꾸고 체크인합니다. 
	// 로그 확인해보세요.
	// ->kday
	// ^^;;
}
// -----------------------------------------------------------------------------
void AgcuPathWork::bBackward(void)
{
	m_unionBaseFlag.st.m_1ulForward = 0;
}
// -----------------------------------------------------------------------------
void AgcuPathWork::bLoop(void)
{
	m_unionBaseFlag.st.m_1ulLoop = 1;
}
// -----------------------------------------------------------------------------
void AgcuPathWork::bNoLoop(void)
{
	m_unionBaseFlag.st.m_1ulLoop = 0;
}
// -----------------------------------------------------------------------------
void AgcuPathWork::tTimeUpdate(RwUInt32 ulDifTick)
{
	if( ulDifTick)
	{
		static RwUInt32 PostTick = ulDifTick;

		const RwUInt32 EPSILON = 10;
		if( ulDifTick >  PostTick + EPSILON )
			ulDifTick = PostTick + EPSILON;
		PostTick = ulDifTick;
		

		m_ulAccum	+= ulDifTick;
		if( m_ulAccum >= m_ulLife )
			if( m_unionBaseFlag.st.m_1ulLoop )
				m_ulAccum -= m_ulLife;
			else
			{
				m_ulAccum	= m_ulLife;
				m_unionBaseFlag.st.m_1ulState	= 1;//end
			}
			
		//char buff[MAX_PATH] = "";
		//itoa(ulDifTick,buff,10);
		//strcat(buff, "--");
		//if( ulDifTick > MAX_PATH-strlen(buff)-1 )
		//{
		//	ulDifTick = MAX_PATH-strlen(buff)-1;
		//}
		//memset(&buff[strlen(buff)], '*', ulDifTick);
		//buff[strlen(buff)-1] = '\n';
		//Eff2Ut_ToConsol(buff);
	}
}

// -----------------------------------------------------------------------------
RwInt32 AgcuPathWork::bOnIdleSetKeyTime(  RwMatrix& matOut
										 , RwUInt32 ulSetTick
										 /*ulSetTick < life*/)
{
	m_ulAccum = ulSetTick;

	if( m_ulAccum > m_ulLife )
		m_ulAccum = m_ulLife;
	
	return bOnIdle(matOut, 0LU);
};

// -----------------------------------------------------------------------------
RwUInt32 AgcuPathWork::tKeyTime(void)
{
	return (m_unionBaseFlag.st.m_1ulForward ? m_ulAccum : (m_ulLife - m_ulAccum));
}

// =============================================================================
// AgcuPathSingleSpline
// =============================================================================
// -----------------------------------------------------------------------------
AgcuPathSingleSpline::AgcuPathSingleSpline(RwUInt32 ulLife)
	: AgcuPathWork(AgcuPathWork::eWT_SingleSpline, ulLife)
	, m_pSpline(NULL)
	, m_fInvsLife(0.f)
{
	m_unionFlag.st.m_4ulType	= 0;
	m_unionFlag.st.m_1ulAcel	= 0;
	ASSERT( ulLife );
	if( ulLife )
		m_fInvsLife	= 1.f / static_cast<RwReal>(ulLife);
}
// -----------------------------------------------------------------------------
AgcuPathSingleSpline::~AgcuPathSingleSpline()
{
	Eff2Ut_SAFE_DESTROY_SPLINE(m_pSpline);
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathSingleSpline::bInitSpline(const SETSPLINE& setSpline)
{
	ASSERT( setSpline.numCtrl >= 4 );
	Eff2Ut_SAFE_DESTROY_SPLINE(m_pSpline);

	AcuFrameMemoryLocal<RwV3d> ctrlPoints(setSpline.numCtrl);
	//RwV3d*	ctrlPoints = (RwV3d*)AcuFrameMemory::AllocFrameMemory(sizeof(RwV3d)*setSpline.numCtrl);
	RwV3d* point=ctrlPoints;
	RwMatrix* matrix=setSpline.ctrlMatrices;
	for( int i=0; 
		 i<setSpline.numCtrl; 
		 ++i, ++point, ++matrix )
	{
		*point = matrix->pos;
	}

	m_pSpline =
		RpSplineCreate( setSpline.numCtrl
		, setSpline.closed ? rpSPLINETYPECLOSEDLOOPBSPLINE : rpSPLINETYPEOPENLOOPBSPLINE
		, ctrlPoints );

	ASSERT( m_pSpline );
	if( !m_pSpline )
	{//err
		m_unionBaseFlag.st.m_1ulState	= 1;
		return -1;
	}

	m_unionBaseFlag.st.m_1ulState	= 0;
	return 0;
}
// -----------------------------------------------------------------------------
void AgcuPathSingleSpline::bSetFixedLookat(const RwV3d* lookat)
{
	m_pFixedLookat	= lookat;
	m_unionFlag.st.m_4ulType	= 3;//
}
// -----------------------------------------------------------------------------
void AgcuPathSingleSpline::bSetActType(RwUInt32 ulType)
{
	ASSERT( ulType >= 0 && ulType < 16 );
	ASSERT( ulType < 4 );//current vaild type
	m_unionFlag.st.m_4ulType = ulType;
}
// -----------------------------------------------------------------------------
void AgcuPathSingleSpline::bSetAccel(RwUInt32 ulAccel)
{
	ASSERT( ulAccel == 0 || ulAccel == 1 );
	m_unionFlag.st.m_1ulAcel = ulAccel;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathSingleSpline::bInitSpline(AgcmResourceLoader* pAgcmResourceLoader
										  ,LPSTR szSpline/*fullpath*/
										  , const RwMatrix* pmatTrans)
{
	ASSERT( pAgcmResourceLoader && szSpline );
	Eff2Ut_SAFE_DESTROY_SPLINE(m_pSpline);

	m_pSpline = pAgcmResourceLoader->LoadRpSpline(szSpline);

	ASSERT( m_pSpline );
	if( !m_pSpline )
	{//err
		m_unionBaseFlag.st.m_1ulState	= 1;
		return -1;
	}

	m_unionBaseFlag.st.m_1ulState	= 0;
	return 0;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathSingleSpline::bOnIdle(RwMatrix& matOut, RwUInt32 ulDifTick)
{
	PROFILE("AgcuPathSingleSpline::bOnIdle");

	ASSERT( m_pSpline && m_fInvsLife != 0.f );
	if( !m_pSpline			||
		m_fInvsLife == 0.f	)
		return -1;//err

	AgcuPathWork::tTimeUpdate(ulDifTick);
	if( m_unionBaseFlag.st.m_1ulState == 1/*end*/)
		return m_unionBaseFlag.st.m_1ulState;


	switch( m_unionFlag.st.m_4ulType )
	{
	case 0://flight
	{
		return vGetMatrix_flight( 
			&matOut
			, static_cast<RwReal>(AgcuPathWork::tKeyTime()) * m_fInvsLife );
	}break;

	case 1://side vector is parallel with xz_plane
	{
		return vGetMatrix_sideParallelXzplane( 
			&matOut
			, static_cast<RwReal>(AgcuPathWork::tKeyTime()) * m_fInvsLife );
	}break;

	case 2://up vector is world's up vector (0,1,0)
	{
		return vGetMatrix_upWorldUp( 
			&matOut
			, static_cast<RwReal>(AgcuPathWork::tKeyTime()) * m_fInvsLife );
	}break;

	case 3://fixed lookat with side vector parallel to xz_plane
	{
		return vGetMatrix_fixedLookat( 
			&matOut
			, static_cast<RwReal>(AgcuPathWork::tKeyTime()) * m_fInvsLife );
	}break;

	default :
		ASSERT( !"unknown type" );
		break;
	}

	return m_unionBaseFlag.st.m_1ulState;
}

RwInt32		AgcuPathSingleSpline::bOnIdleOffset(RwMatrix& matOut, RwUInt32 ulOffset)
{
	PROFILE("AgcuPathSingleSpline::bOnIdle");

	ASSERT( m_pSpline && m_fInvsLife != 0.f );
	if( !m_pSpline			||
		m_fInvsLife == 0.f	)
		return -1;//err

	switch( m_unionFlag.st.m_4ulType )
	{
	case 0://flight
	{
		return vGetMatrix_flight( 
			&matOut
			, static_cast<RwReal>(ulOffset) * m_fInvsLife );
	}break;

	case 1://side vector is parallel with xz_plane
	{
		return vGetMatrix_sideParallelXzplane( 
			&matOut
			, static_cast<RwReal>(ulOffset) * m_fInvsLife );
	}break;

	case 2://up vector is world's up vector (0,1,0)
	{
		return vGetMatrix_upWorldUp( 
			&matOut
			, static_cast<RwReal>(ulOffset) * m_fInvsLife );
	}break;

	case 3://fixed lookat with side vector parallel to xz_plane
	{
		return vGetMatrix_fixedLookat( 
			&matOut
			, static_cast<RwReal>(ulOffset) * m_fInvsLife );
	}break;

	default :
		ASSERT( !"unknown type" );
		break;
	}

	return m_unionBaseFlag.st.m_1ulState;
}

// -----------------------------------------------------------------------------
RwInt32 AgcuPathSingleSpline::vGetMatrix_flight(RwMatrix* pmat, RwReal where)
{
	if( -1.f == RpSplineFindMatrix( m_pSpline
		, m_unionFlag.st.m_1ulAcel ? rpSPLINEPATHNICEENDS : rpSPLINEPATHSMOOTH
		, where
		, NULL
		, pmat ) )
		return -1;//err

	return 0;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathSingleSpline::vGetMatrix_sideParallelXzplane(RwMatrix* pmat, RwReal where)
{
	if( -1.f == RpSplineFindMatrix( m_pSpline
		, m_unionFlag.st.m_1ulAcel ? rpSPLINEPATHNICEENDS : rpSPLINEPATHSMOOTH
		, where
		, NULL
		, pmat ) )
		return -1;//err

	if( fabsf(pmat->right.y) > 0.0001f )
	{
		const RwV3d	wy = { 0.f, 1.f, 0.f };
		if( fabsf(RwV3dDotProduct( &pmat->at, &wy )) < 1.f )
		{
			RwV3dCrossProduct( &pmat->right, &wy, &pmat->at );
			RwV3dNormalize( &pmat->right, &pmat->right );
			RwV3dCrossProduct( &pmat->up, &pmat->at, &pmat->right );
			RwV3dNormalize( &pmat->up, &pmat->up );
		}
	}

	return 0;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathSingleSpline::vGetMatrix_upWorldUp(RwMatrix* pmat, RwReal where)
{
	if( -1.f == RpSplineFindMatrix( m_pSpline
		, m_unionFlag.st.m_1ulAcel ? rpSPLINEPATHNICEENDS : rpSPLINEPATHSMOOTH
		, where
		, NULL
		, pmat ) )
	{
		m_unionBaseFlag.st.m_1ulState = 1;
		return -1;//err
	}

	const RwV3d	wy = { 0.f, 1.f, 0.f };
	if( fabsf(RwV3dDotProduct( &pmat->at, &wy ))
		<
		fabsf(RwV3dDotProduct( &pmat->right, &wy ))
		)
	{
		pmat->up = wy;
		RwV3dCrossProduct( &pmat->right, &wy, &pmat->at );
		RwV3dNormalize( &pmat->right, &pmat->right );
		RwV3dCrossProduct( &pmat->at, &pmat->right, &wy );
		RwV3dNormalize( &pmat->at, &pmat->at );
	}
	else
	{
		pmat->up = wy;
		RwV3dCrossProduct( &pmat->at, &pmat->right, &wy );
		RwV3dNormalize( &pmat->at, &pmat->at );
		RwV3dCrossProduct( &pmat->right, &wy, &pmat->at );
		RwV3dNormalize( &pmat->right, &pmat->right );
	}

	return 0;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathSingleSpline::vGetMatrix_fixedLookat(RwMatrix* pmat, RwReal where)
{
	ASSERT( m_pFixedLookat && m_pSpline );

	RwMatrixSetIdentity(pmat);
	
	if( !RpSplineFindPosition (m_pSpline
		, m_unionFlag.st.m_1ulAcel ? rpSPLINEPATHNICEENDS : rpSPLINEPATHSMOOTH
		, where
		, &pmat->pos
		, NULL ) )
		return -1;

	RwV3d	atSubEye;
	RwV3dSub(&atSubEye, m_pFixedLookat, &pmat->pos);

	
	const RwV3d	wy = {0.f, 1.f, 0.f};
	RwV3dNormalize( &pmat->at, &atSubEye );
	if( fabsf(RwV3dDotProduct(&pmat->at, &wy)) < 0.9999f )
	{
		RwV3dCrossProduct( &pmat->right, &wy, &pmat->at );
		RwV3dNormalize( &pmat->right, &pmat->right );
		RwV3dCrossProduct( &pmat->up, &pmat->at, &pmat->right );
		RwV3dNormalize( &pmat->up, &pmat->up );
	}
	else
	{
		// TODO : correct this code
		RwV3d	vat = pmat->at;
		if( -1.f == RpSplineFindMatrix( m_pSpline
			, m_unionFlag.st.m_1ulAcel ? rpSPLINEPATHNICEENDS : rpSPLINEPATHSMOOTH
			, where
			, NULL
			, pmat ) )
		{
			return -1;
		}

		RwV3d	vrot;
		RwReal	angle;
		if( AngleFrom2Vec( &vrot, &angle, pmat->at, vat ) )
		{
			RwMatrix tmp;
			RwMatrixRotate ( &tmp, &vrot, DEF_R2D(angle), rwCOMBINEREPLACE );

			RwV3d	vpos = { -pmat->pos.x, -pmat->pos.y, -pmat->pos.z };
			RwMatrixTranslate ( pmat, &vpos, rwCOMBINEPOSTCONCAT );
			RwMatrixTransform ( pmat, &tmp, rwCOMBINEPOSTCONCAT  );
			RwV3dNegate( &vpos, &vpos );
			RwMatrixTranslate ( pmat, &vpos, rwCOMBINEPOSTCONCAT );
		}
		else if( angle == DEF_PI )
		{
			RwV3dNegate( &pmat->right, &pmat->right );
			RwV3dNegate( &pmat->up, &pmat->up );
			RwV3dNegate( &pmat->at, &pmat->at );
		}

	}
	return 0;
}

// =============================================================================
// AgcuPathDoubleSpline
// =============================================================================
// -----------------------------------------------------------------------------
AgcuPathDoubleSpline::AgcuPathDoubleSpline(RwUInt32 ulLife)
	: AgcuPathWork(AgcuPathWork::eWT_DoubleSpline, ulLife)
	, m_pSplineEye(NULL)
	, m_pSplineLookat(NULL)
	, m_fInvsLife(0.f)
{
	m_unionFlag.st.m_4ulType	= 0;
	m_unionFlag.st.m_1ulAcel	= 0;

	ASSERT( ulLife );
	if( ulLife )
		m_fInvsLife	= 1.f / static_cast<RwReal>(ulLife);
}
// -----------------------------------------------------------------------------
AgcuPathDoubleSpline::~AgcuPathDoubleSpline()
{
	Eff2Ut_SAFE_DESTROY_SPLINE(m_pSplineEye);
	Eff2Ut_SAFE_DESTROY_SPLINE(m_pSplineLookat);
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathDoubleSpline::bInitSpline(const SETSPLINE& setSpline)
{
	ASSERT( setSpline.numCtrl >= 4 );

	AcuFrameMemoryLocal<RwV3d>	ctrlPointsEye(setSpline.numCtrl);
	//RwV3d*	ctrlPointsEye = (RwV3d*)AcuFrameMemory::AllocFrameMemory(sizeof(RwV3d)*setSpline.numCtrl);
	RwV3d*	pointEye = ctrlPointsEye;
	AcuFrameMemoryLocal<RwV3d>	ctrlPointsAt(setSpline.numCtrl);
	//RwV3d*	ctrlPointsAt = (RwV3d*)AcuFrameMemoryLocal<>::AllocFrameMemory(sizeof(RwV3d)*setSpline.numCtrl);
	RwV3d*	pointAt = ctrlPointsAt;
	const RwMatrix* matrix=setSpline.ctrlMatrices;
	RwV3d	voffset = { 0.f, 0.f, 0.f };
    for( int i=0; 
		 i<setSpline.numCtrl; 
		 ++i, ++pointEye, ++pointAt, ++matrix )
	{
		*pointEye = matrix->pos;

		RwV3dScale( &voffset, &matrix->at, 5000.f );
		RwV3dAdd(pointAt, &matrix->pos, &voffset);
	}

	{
		Eff2Ut_SAFE_DESTROY_SPLINE(m_pSplineEye);

		m_pSplineEye = RpSplineCreate( setSpline.numCtrl
			, setSpline.closed ? rpSPLINETYPECLOSEDLOOPBSPLINE : rpSPLINETYPEOPENLOOPBSPLINE
			, ctrlPointsEye );
		ASSERT( m_pSplineEye );
		if( !m_pSplineEye )
		{//err
			m_unionBaseFlag.st.m_1ulState = 1;
			return -1;
		}
	}

	{
		Eff2Ut_SAFE_DESTROY_SPLINE(m_pSplineLookat);

		m_pSplineLookat = RpSplineCreate( setSpline.numCtrl
			, setSpline.closed ? rpSPLINETYPECLOSEDLOOPBSPLINE : rpSPLINETYPEOPENLOOPBSPLINE
			, ctrlPointsAt );
		ASSERT( m_pSplineLookat );
		if( !m_pSplineLookat )
		{//err
			m_unionBaseFlag.st.m_1ulState = 1;
			return -1;
		}
	}
    
	m_unionBaseFlag.st.m_1ulState	= 0;
	return 0;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathDoubleSpline::bInitSpline(
	AgcmResourceLoader* pAgcmResourceLoader
	, LPSTR			szSplineEye/*fullpath*/
	, LPSTR			szSplineAt/*fullpath*/)
{
	ASSERT( pAgcmResourceLoader && szSplineEye && szSplineAt );

	Eff2Ut_SAFE_DESTROY_SPLINE(m_pSplineEye);
	Eff2Ut_SAFE_DESTROY_SPLINE(m_pSplineLookat);
	m_pSplineEye = pAgcmResourceLoader->LoadRpSpline(szSplineEye);
	m_pSplineLookat = pAgcmResourceLoader->LoadRpSpline(szSplineAt);

	ASSERT( m_pSplineEye && m_pSplineLookat );
	if( !m_pSplineEye || !m_pSplineLookat )
	{//err
		m_unionBaseFlag.st.m_1ulState	= 1;
		return -1;
	}

	m_unionBaseFlag.st.m_1ulState	= 0;
	return 0;
}
// -----------------------------------------------------------------------------
void AgcuPathDoubleSpline::bSetActType(RwUInt32 ulType)
{
	ASSERT( ulType >= 0 && ulType < 16 );
	ASSERT( ulType < 2 );//current vaild type
	m_unionFlag.st.m_4ulType = ulType;
}
// -----------------------------------------------------------------------------
void AgcuPathDoubleSpline::bSetAccel(RwUInt32 ulAccel)
{
	ASSERT( ulAccel == 0 || ulAccel == 1 );
	m_unionFlag.st.m_1ulAcel = ulAccel;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathDoubleSpline::bOnIdle(RwMatrix& matOut, RwUInt32 ulDifTick)
{
	ASSERT( m_pSplineEye && m_pSplineLookat && m_fInvsLife != 0.f );
	if( !m_pSplineEye		||
		!m_pSplineLookat	||
		m_fInvsLife == 0.f	)
	{
		m_unionBaseFlag.st.m_1ulState = 1;
		return -1;//err
	}

	AgcuPathWork::tTimeUpdate(ulDifTick);
	if( m_unionBaseFlag.st.m_1ulState == 1/*end*/)
		return m_unionBaseFlag.st.m_1ulState;

	
	RwInt32 ir =vGetMatrix( &matOut
		, static_cast<RwReal>(AgcuPathWork::tKeyTime()) * m_fInvsLife );
	if( T_ISMINUS4( ir ) )
	{
		m_unionBaseFlag.st.m_1ulState = 1;
		return -1;
	}

	return m_unionBaseFlag.st.m_1ulState;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathDoubleSpline::vGetMatrix(RwMatrix* pmat, RwReal where)
{
	ASSERT( pmat );

	switch(this->m_unionFlag.st.m_4ulType)
	{
	case 0: // 0 : side vector is parallel with xz_plane
		{
			if( T_ISMINUS4( vGetEye(pmat->pos, where ) ) )
				return -1;
			RwV3d	lookat = {0.f, 0.f, 0.f};
			if( T_ISMINUS4( vGetLookat(lookat, where ) ) )
				return -1;

			const RwV3d	wy = {0.f, 1.f, 0.f};
			RwV3dSub( &pmat->at, &lookat, &pmat->pos );
			RwV3dNormalize( &pmat->at, &pmat->at );

			if( fabsf(RwV3dDotProduct(&pmat->at, &wy)) < 0.9999f )
			{
				RwV3dCrossProduct( &pmat->right, &wy, &pmat->at );
				RwV3dNormalize( &pmat->right, &pmat->right );
				RwV3dCrossProduct( &pmat->up, &pmat->at, &pmat->right );
				RwV3dNormalize( &pmat->up, &pmat->up );
			}
			else
			{
				// TODO : correct this code
				RwV3d	vat = pmat->at;
				if( -1.f == RpSplineFindMatrix( m_pSplineEye
					, m_unionFlag.st.m_1ulAcel ? rpSPLINEPATHNICEENDS : rpSPLINEPATHSMOOTH
					, where
					, NULL
					, pmat ) )
				{
					return -1;
				}

				RwV3d	vrot;
				RwReal	angle;
				if( AngleFrom2Vec(&vrot, &angle, pmat->at, vat ) )
				{
					RwMatrix tmp;
					RwMatrixRotate ( &tmp, &vrot, DEF_R2D(angle), rwCOMBINEREPLACE );

					RwV3d	vpos = { -pmat->pos.x, -pmat->pos.y, -pmat->pos.z };
					RwMatrixTranslate ( pmat, &vpos, rwCOMBINEPOSTCONCAT );
					RwMatrixTransform ( pmat, &tmp, rwCOMBINEPOSTCONCAT  );
					RwV3dNegate( &vpos, &vpos );
					RwMatrixTranslate ( pmat, &vpos, rwCOMBINEPOSTCONCAT );
				}
				else if( angle == DEF_PI )
				{
					RwV3dNegate( &pmat->right, &pmat->right );
					RwV3dNegate( &pmat->up, &pmat->up );
					RwV3dNegate( &pmat->at, &pmat->at );
				}

			}
		}break;
	case 1: // 1 : getMatrix from eyeSpline and rotat to look at lookatSpline 
		{
			RwV3d	vat = pmat->at;
			if( -1.f == RpSplineFindMatrix( m_pSplineEye
				, m_unionFlag.st.m_1ulAcel ? rpSPLINEPATHNICEENDS : rpSPLINEPATHSMOOTH
				, where
				, NULL
				, pmat ) )
			{
				return -1;
			}

			RwV3d	vrot;
			RwReal	angle;
			if( AngleFrom2Vec(&vrot, &angle, pmat->at, vat ) )
			{
				RwMatrix tmp;
				RwMatrixRotate ( &tmp, &vrot, DEF_R2D(angle), rwCOMBINEREPLACE );

				RwV3d	vpos = { -pmat->pos.x, -pmat->pos.y, -pmat->pos.z };
				RwMatrixTranslate ( pmat, &vpos, rwCOMBINEPOSTCONCAT );
				RwMatrixTransform ( pmat, &tmp, rwCOMBINEPOSTCONCAT  );
				RwV3dNegate( &vpos, &vpos );
				RwMatrixTranslate ( pmat, &vpos, rwCOMBINEPOSTCONCAT );
			}
			else if( angle == DEF_PI )
			{
				RwV3dNegate( &pmat->right, &pmat->right );
				RwV3dNegate( &pmat->up, &pmat->up );
				RwV3dNegate( &pmat->at, &pmat->at );
			}
		}break;
	}
	
	return 0;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathDoubleSpline::vGetEye(RwV3d& vEye, RwReal where)
{
	ASSERT( m_pSplineEye );
	if( !RpSplineFindPosition (m_pSplineEye
		, m_unionFlag.st.m_1ulAcel ? rpSPLINEPATHNICEENDS : rpSPLINEPATHSMOOTH
		, where
		, &vEye
		, NULL ) )
		return -1;

	return 0;
}
// -----------------------------------------------------------------------------
RwInt32 AgcuPathDoubleSpline::vGetLookat(RwV3d& vLookat, RwReal where)
{
	ASSERT( m_pSplineLookat );
	if( !RpSplineFindPosition( m_pSplineLookat
		, m_unionFlag.st.m_1ulAcel ? rpSPLINEPATHNICEENDS : rpSPLINEPATHSMOOTH
		, where
		, &vLookat
		, NULL ) )
		return -1;

	return 0;
}

// -----------------------------------------------------------------------------
// AgcuCamPathWork.cpp - End of file
// -----------------------------------------------------------------------------

AgcuPathWork * AgcuPathWork::Load( BYTE * pByte )
{
	ASSERT( NULL != pByte );

	// 버젼 정보 기록..
	// sab 파일은 이걸로 끝.. 
	// 새로운 정보는 다른 포맷으로 생성하는게 좋을테니
	// 버젼 정보는 생략한다.
	// fwrite( ( void * ) &version , sizeof version , 1 , pFile );

	INT32			nType				;
	INT32			nSSType				;
	BOOL			bAccel				;
	BOOL			bClosed				;
	BOOL			bLoop				;

	UINT32			uDuration			;	// 전체길이
	INT32			nCtrlMatrixCount	;
	RwMatrix		pCtrlMatrix	[ CW_MAX_SPLINE_NODE ];

	// 옵션.
	// fread( ( void * ) &m_nType		, sizeof m_nType	, 1 , pFile );
	nType	= *( ( INT32 * ) pByte );
	pByte	+= sizeof nType;
	//fread( ( void * ) &m_nSSType	, sizeof m_nSSType	, 1 , pFile );
	nSSType	= *( ( INT32 * ) pByte );
	pByte	+= sizeof nSSType;
	//fread( ( void * ) &m_bAccel		, sizeof m_bAccel	, 1 , pFile );
	bAccel	= *( ( BOOL * ) pByte );
	pByte	+= sizeof bAccel;
	//fread( ( void * ) &m_bClosed	, sizeof m_bClosed	, 1 , pFile );
	bClosed	= *( ( BOOL * ) pByte );
	pByte	+= sizeof bClosed;
	//fread( ( void * ) &m_bLoop		, sizeof m_bLoop	, 1 , pFile );
	bLoop	= *( ( BOOL * ) pByte );
	pByte	+= sizeof bLoop;

	// 길이
	//fread( ( void * ) &m_uDuration , sizeof m_uDuration , 1 , pFile );
	uDuration	= *( ( UINT32 * ) pByte );
	pByte		+= sizeof uDuration;

	// 노드 갯수..
	//fread( ( void * ) &m_nCtrlMatrixCount , sizeof m_nCtrlMatrixCount , 1 , pFile );
	nCtrlMatrixCount	= *( ( INT32 * ) pByte );
	pByte				+= sizeof nCtrlMatrixCount;

	ASSERT( nCtrlMatrixCount > 0	);
	ASSERT( nCtrlMatrixCount < 100	);

	if( nCtrlMatrixCount <= 0	||
		nCtrlMatrixCount > 100	)
		return NULL;

	// 매트릭스 카피..
    // fread( ( void * ) m_pCtrlMatrix , sizeof RwMatrix , m_nCtrlMatrixCount , pFile );
	memcpy( ( void * ) pCtrlMatrix ,pByte , nCtrlMatrixCount * sizeof RwMatrix );

	// 로딩 완료..

	// 스플라인 생성..
	AgcuPathWork * pSplinePathWork = NULL;
	if( nCtrlMatrixCount >= 4 )
	{
		switch( nType )
		{
		case 	SINGLESPLINE:
			{
				switch( nSSType )
				{
				default:
				case	SST_NORMAL		:
					{
						AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( uDuration );

						if( bLoop )	pSpline->bLoop	();
						else		pSpline->bNoLoop();

						pSpline->bReset();

						pSpline->bForward();

						pSpline->bSetActType(1);		//flight

						if( bAccel )	pSpline->bSetAccel(1);			//accel
						else			pSpline->bSetAccel(0);			//accel

						stSetSpline	stSet;

						if( bClosed )	stSet.closed		= true;
						else			stSet.closed		= false;

						stSet.ctrlMatrices	= pCtrlMatrix;
						stSet.numCtrl		= nCtrlMatrixCount;

						pSpline->bInitSpline(stSet);

						pSplinePathWork = pSpline;
					}
					break;
				case	SST_STARE	:
					{ 
						AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( uDuration );

						// 첫번째 녀석은 위치로 사용..
						if( nCtrlMatrixCount >= 5 )
						{
							if( bLoop )	pSpline->bLoop();
							else		pSpline->bNoLoop();

							pSpline->bReset();

							pSpline->bForward();
							//pSpline->bBackward();

							pSpline->bSetActType(1);		//flight

							if( bAccel )	pSpline->bSetAccel(1);			//accel
							else			pSpline->bSetAccel(0);			//accel

							stSetSpline	stSet;

							if( bClosed )	stSet.closed		= true;
							else			stSet.closed		= false;

							stSet.ctrlMatrices	= pCtrlMatrix + 1;
							stSet.numCtrl		= nCtrlMatrixCount - 1;

							pSpline->bInitSpline(stSet);
							pSpline->bSetFixedLookat( &pCtrlMatrix[ 0 ].pos);

							pSplinePathWork = pSpline;
						}
					}
					break;
				case SST_PARALLEL:
					{
						// ...
					}
					break;
				case SST_FLIGHT:
					{
						// ...
					}
					break;
				}
			}
			break;
		case	DOUBLESPLINE:
			{
				AgcuPathDoubleSpline * pSpline = new AgcuPathDoubleSpline( uDuration );

				if( bLoop )	pSpline->bLoop();
				else			pSpline->bNoLoop();

				pSpline->bReset();

				pSpline->bForward();
				//pSpline->bBackward();

				pSpline->bSetActType(0);	//side vector is parallel with xz_plane

				if( bAccel )	pSpline->bSetAccel(1);			//accel
				else			pSpline->bSetAccel(0);			//accel

				stSetSpline	stSet;

				if( bClosed )	stSet.closed	= true;
				else			stSet.closed	= false;

				stSet.ctrlMatrices	= pCtrlMatrix;
				stSet.numCtrl		= nCtrlMatrixCount;

				pSpline->bInitSpline(stSet);

				pSplinePathWork = pSpline;
			}
			break;
		}
	}

	return pSplinePathWork;
}


AgcuPathWork * AgcuPathWork::CreateSplinePath(LPCSTR fname/*.sab*/)
{
	AgcuPathWork*	pr	= NULL;
	AuAutoFile	fp(fname, "rb");
	if( fp && !fseek(fp, 0, SEEK_END) )
	{
		int	buffsize	= ftell(fp);
		AcuFrameMemoryLocal<BYTE>	buff(buffsize);
		if( buff && !fseek(fp, 0, SEEK_SET) )
			if(buffsize == fread(buff, sizeof(BYTE), buffsize, fp))
				pr	= AgcuPathWork::Load(buff);
	}

	return pr;
}