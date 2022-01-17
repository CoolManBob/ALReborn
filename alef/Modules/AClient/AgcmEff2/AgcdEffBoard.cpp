#include "AgcdEffBoard.h"
#include "ApMemoryTracker.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG( AgcdEffBoard );

AgcdEffBoard::AgcdEffBoard( AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType ) : AgcdEffRenderBase( AgcdEffBase::E_EFFBASE_BOARD, eBlendType )
{
	EFFMEMORYLOG_CON;
}

AgcdEffBoard::~AgcdEffBoard()
{
	EFFMEMORYLOG_DES;
}

RwInt32 AgcdEffBoard::bToFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tToFile(fp);
	ir += AgcdEffRenderBase::tToFile(fp);
	ir += AgcdEffBase::tToFileVariableData(fp);

	return ir;
};

RwInt32 AgcdEffBoard::bFromFile(FILE* fp)
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
INT32 AgcdEffBoard::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_BOARD );
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