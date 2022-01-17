#include "AgcdEffTail.h"
#include "ApMemoryTracker.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffTail);

AgcdEffTail::AgcdEffTail(AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType) : AgcdEffRenderBase( AgcdEffBase::E_EFFBASE_TAIL, eBlendType ), 
 m_nMaxNum(0),
 m_dwTimeGap(0LU),
 m_dwPointLife(0LU),
 m_fInvsPointLife(0.f)
{
	EFFMEMORYLOG_CON;
}

AgcdEffTail::~AgcdEffTail()
{
	EFFMEMORYLOG_DES;
}

RwInt32	AgcdEffTail::bToFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tToFile(fp);
	ir += AgcdEffRenderBase::tToFile(fp);

	ir += fwrite( &m_nMaxNum, 1, sizeof(m_nMaxNum), fp );
	ir += fwrite( &m_dwTimeGap, 1, sizeof(m_dwTimeGap), fp );
	ir += fwrite( &m_dwPointLife, 1, sizeof(m_dwPointLife), fp );
	ir += fwrite( &m_fHeight1, 1, sizeof(m_fHeight1), fp );
	ir += fwrite( &m_fHeight2, 1, sizeof(m_fHeight2), fp );

	ir += AgcdEffBase::tToFileVariableData(fp);
	return ir;
}

RwInt32	AgcdEffTail::bFromFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tFromFile(fp);
	ir += AgcdEffRenderBase::tFromFile(fp);

	ir += fread( &m_nMaxNum, 1, sizeof(m_nMaxNum), fp );
	ir += fread( &m_dwTimeGap, 1, sizeof(m_dwTimeGap), fp );
	ir += fread( &m_dwPointLife, 1, sizeof(m_dwPointLife), fp );
	m_fInvsPointLife = 1.f/m_dwPointLife;
	ir += fread( &m_fHeight1, 1, sizeof(m_fHeight1), fp );
	ir += fread( &m_fHeight2, 1, sizeof(m_fHeight2), fp );

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
INT32 AgcdEffTail::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_TAIL );
	if( T_ISMINUS4( AgcdEffBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}
	if( T_ISMINUS4( AgcdEffRenderBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}

	AgcdEffTail* pEffTail = static_cast<AgcdEffTail*>(pEffBase);
	m_nMaxNum = pEffTail->m_nMaxNum;
	m_dwTimeGap = pEffTail->m_dwTimeGap;
	m_dwPointLife = pEffTail->m_dwPointLife;
	m_fInvsPointLife = pEffTail->m_fInvsPointLife;
	m_fHeight1 = pEffTail->m_fHeight1;
	m_fHeight2 = pEffTail->m_fHeight2;

	return 0;
};
#endif//USE_MFC