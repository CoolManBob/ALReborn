#include "AgcdEffLight.h"
#include "AgcuBillBoard.h"

#include "ApMemoryTracker.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffLight);

AgcdEffLight::AgcdEffLight( E_EFFLIGHTTYPE eLightType ) : AgcdEffBase( AgcdEffBase::E_EFFBASE_LIGHT ),
 m_eEffLightType ( eLightType ),
 m_fConAngle( 0.f )
{
	EFFMEMORYLOG_CON;

	RwMatrixSetIdentity( &m_matTrans );
	RwMatrixSetIdentity( &m_matRot );
	
	Eff2Ut_ZEROBLOCK( m_v3dCenter );
}

AgcdEffLight::~AgcdEffLight()
{
	EFFMEMORYLOG_DES;
}

void AgcdEffLight::bSetPos( const RwV3d& v3Pos )	
{
	m_v3dCenter = v3Pos;
	RwMatrixTranslate( &m_matTrans, &m_v3dCenter, rwCOMBINEREPLACE );
}

void AgcdEffLight::bSetAngle( const STANGLE& stAngle )	
{ 
	m_stAngle = stAngle;
	RwMatrixRotate ( &m_matRot, PTRAXISWY, m_stAngle.m_fYaw, rwCOMBINEREPLACE );
	RwMatrixRotate ( &m_matRot, PTRAXISWX, m_stAngle.m_fPitch, rwCOMBINEPOSTCONCAT );
	RwMatrixRotate ( &m_matRot, PTRAXISWZ, m_stAngle.m_fRoll, rwCOMBINEPOSTCONCAT );
}

RwInt32 AgcdEffLight::bToFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tToFile(fp);

	ir += fwrite( &m_eEffLightType, 1
		, sizeof(m_eEffLightType)
		+ sizeof(m_v3dCenter)
		+ sizeof(m_stAngle)
		+ sizeof(m_fConAngle)
		+ sizeof(m_SurfProp)
		+ sizeof(m_ColrMaterial)
		, fp );

	ir += AgcdEffBase::tToFileVariableData(fp);
	return ir;
}

RwInt32 AgcdEffLight::bFromFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tFromFile(fp);

	ir += fread( &m_eEffLightType, 1
		, sizeof(m_eEffLightType)
		+ sizeof(m_v3dCenter)
		+ sizeof(m_stAngle)
		+ sizeof(m_fConAngle)
		+ sizeof(m_SurfProp)
		+ sizeof(m_ColrMaterial)
		, fp );

	RwInt32 ir2 = AgcdEffBase::tFromFileVariableData(fp);
	if( T_ISMINUS4( ir2 ) )
	{
		ASSERT( !"AgcdEffBase::tToFileVariableData failed" );
		return -1;
	}
	return (ir+ir2);
}

#ifdef USE_MFC
INT32 AgcdEffLight::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_LIGHT );
	if( T_ISMINUS4( AgcdEffBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}

	AgcdEffLight* pEffLight = static_cast<AgcdEffLight*>(pEffBase);
	m_SurfProp = pEffLight->m_SurfProp;
	m_ColrMaterial = pEffLight->m_ColrMaterial;
	m_matTrans = pEffLight->m_matTrans;
	m_matRot = pEffLight->m_matRot;

	return 0;
};
#endif//USE_MFC