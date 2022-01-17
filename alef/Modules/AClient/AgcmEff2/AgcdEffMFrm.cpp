#include "AgcdEffMFrm.h"
#include "AgcuBillBoard.h"
#include "AgcdEffGlobal.h"
#include "AgcuEffTable.h"

#include "ApMemoryTracker.h"

#include "AcuSinTbl.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffMFrm);

AgcdEffMFrm::AgcdEffMFrm(E_MFRM_TYPE eMFrmType) : AgcdEffBase( AgcdEffBase::E_EFFBASE_MOVINGFRAME ), m_eMFrmType( eMFrmType )
{
	EFFMEMORYLOG_CON;
}

AgcdEffMFrm::~AgcdEffMFrm()
{
	EFFMEMORYLOG_DES;
}

RwInt32 AgcdEffMFrm::bShakeFrm( RwFrame* pFrmShake, RwUInt32 dwPastTime )
{
	if( dwPastTime > m_stShakeFrm.bGetDuration() )
		return 1;

	if( !pFrmShake || !pFrmShake->child )
	{
		Eff2Ut_ERR( "!pFrmShake || !pFrmShake->child @ AgcdEffMFrm::bShakeFrm" );
		return -1;
	}

	RwV3d	shakeAxis	= { 0.f, 0.f, 0.f };
	switch( m_stShakeFrm.bGetOscillationAxis() )
	{
	case EOSCILLATIONAXIS_LOCAL_X	:
		RwV3dNormalize	( &shakeAxis, RwMatrixGetRight ( RwFrameGetLTM ( pFrmShake->child ) ) );
		break;
	case EOSCILLATIONAXIS_LOCAL_Y	:
		RwV3dNormalize	( &shakeAxis, RwMatrixGetUp ( RwFrameGetLTM ( pFrmShake->child ) ) );
		break;
	case EOSCILLATIONAXIS_LOCAL_Z	:
		RwV3dNormalize	( &shakeAxis, RwMatrixGetAt ( RwFrameGetLTM ( pFrmShake->child ) ) );
		break;

	case EOSCILLATIONAXIS_WORLD_X	:
		shakeAxis	= *PTRAXISWX;
		break;
	case EOSCILLATIONAXIS_WORLD_Y	:
		shakeAxis	= *PTRAXISWY;
		break;
	case EOSCILLATIONAXIS_WORLD_Z	:
		shakeAxis	= *PTRAXISWZ;
		break;

	case EOSCILLATIONAXIS_CAMERA_X	:
		shakeAxis	= *RwMatrixGetRight( RwFrameGetLTM( RwCameraGetFrame( AgcdEffGlobal::bGetInst().bGetPtrRwCamera() ) ) );
		break;
	case EOSCILLATIONAXIS_CAMERA_Y	:
		shakeAxis	= *RwMatrixGetUp( RwFrameGetLTM( RwCameraGetFrame( AgcdEffGlobal::bGetInst().bGetPtrRwCamera() ) ) );
		break;
	case EOSCILLATIONAXIS_CAMERA_Z	:
		shakeAxis	= *RwMatrixGetAt( RwFrameGetLTM( RwCameraGetFrame( AgcdEffGlobal::bGetInst().bGetPtrRwCamera() ) ) );
		break;

	case EOSCILLATIONAXIS_RANDOM	:
		shakeAxis.x	= Eff2Ut_GETRNDFLOAT1(dwPastTime);
		shakeAxis.z	= Eff2Ut_GETRNDFLOAT1(dwPastTime);
		RwV3dNormalize( &shakeAxis, &shakeAxis );
		break;
	}

	RwV3dScale( &shakeAxis, &shakeAxis, m_stShakeFrm.bDampping(dwPastTime) * AcuSinTbl::Sin( m_stShakeFrm.bGetSpeed() * (RwReal)dwPastTime ) );
	RwFrameTranslate ( pFrmShake, &shakeAxis, rwCOMBINEREPLACE );

	return 0;
}

RwInt32 AgcdEffMFrm::bToFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tToFile(fp);

	ir += fwrite( &m_eMFrmType, 1, sizeof(m_eMFrmType), fp );
	ir += fwrite( &m_stShakeFrm.m_eAxis, 1, sizeof(m_stShakeFrm.m_eAxis), fp );
	ir += fwrite( &m_stShakeFrm.m_eWhose, 1, sizeof(m_stShakeFrm.m_eWhose), fp );
	ir += fwrite( &m_stShakeFrm.m_fAmplitude, 1, sizeof(m_stShakeFrm.m_fAmplitude), fp );
	ir += fwrite( &m_stShakeFrm.m_dwDuration, 1, sizeof(m_stShakeFrm.m_dwDuration), fp );
	ir += fwrite( &m_stShakeFrm.m_fTotalCycle, 1, sizeof(m_stShakeFrm.m_fTotalCycle), fp );

	ir += AgcdEffBase::tToFileVariableData(fp);
	return ir;
}

RwInt32 AgcdEffMFrm::bFromFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tFromFile(fp);

	ir += fread( &m_eMFrmType, 1, sizeof(m_eMFrmType), fp );
	ir += fread( &m_stShakeFrm.m_eAxis, 1, sizeof(m_stShakeFrm.m_eAxis), fp );
	ir += fread( &m_stShakeFrm.m_eWhose, 1, sizeof(m_stShakeFrm.m_eWhose), fp );
	ir += fread( &m_stShakeFrm.m_fAmplitude, 1, sizeof(m_stShakeFrm.m_fAmplitude), fp );
	ir += fread( &m_stShakeFrm.m_dwDuration, 1, sizeof(m_stShakeFrm.m_dwDuration), fp );
	ir += fread( &m_stShakeFrm.m_fTotalCycle, 1, sizeof(m_stShakeFrm.m_fTotalCycle), fp );

	m_stShakeFrm.bSetDuration(m_stShakeFrm.m_dwDuration);
	m_stShakeFrm.bSetCycle(m_stShakeFrm.m_fTotalCycle);

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
INT32 AgcdEffMFrm::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_MOVINGFRAME );
	if( T_ISMINUS4( AgcdEffBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}

	AgcdEffMFrm* pEffMFrm = static_cast<AgcdEffMFrm*>(pEffBase);
	m_eMFrmType = pEffMFrm->m_eMFrmType;
	m_stShakeFrm = pEffMFrm->m_stShakeFrm;

	return 0;
};
#endif//USE_MFC