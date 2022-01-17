#include "AgcdEffTerrainBoard.h"

#include "ApMemoryTracker.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffTerrainBoard);

AgcdEffTerrainBoard::AgcdEffTerrainBoard( AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType ) : AgcdEffRenderBase( AgcdEffBase::E_EFFBASE_TERRAINBOARD, eBlendType )
{
	EFFMEMORYLOG_CON;
}

AgcdEffTerrainBoard::~AgcdEffTerrainBoard()
{
	EFFMEMORYLOG_DES;
}

RwInt32 AgcdEffTerrainBoard::bToFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tToFile(fp);
	ir += AgcdEffRenderBase::tToFile(fp);
	ir += AgcdEffBase::tToFileVariableData(fp);

	return ir;
};

RwInt32 AgcdEffTerrainBoard::bFromFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tFromFile(fp);
	ir += AgcdEffRenderBase::tFromFile(fp);
	RwInt32 ir2 = AgcdEffBase::tFromFileVariableData(fp);
	if( T_ISMINUS4( ir2 ) )
	{
		ASSERT( !"AgcdEffBase::tToFileVariableData failed" );
		return -1;
	}

	return (ir+ir2);
};

#ifdef USE_MFC
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
INT32 AgcdEffTerrainBoard::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_TERRAINBOARD );
	if( T_ISMINUS4( AgcdEffBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}
	if( T_ISMINUS4( AgcdEffRenderBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}

	return 0;
};
#endif//USE_MFC