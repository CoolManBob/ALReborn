#include "AgcuEffUtil.h"
#include "AgcdEffTex.h"
#include "AgcuEffPath.h"
#include "AgcdEffGlobal.h"

#include "ApMemoryTracker.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffTex);
EFFMEMORYLOG(AgcdEffTexInfo);

//---------------------------- AgcdEffTex --------------------------
AgcdEffTex::AgcdEffTex() : m_nIndex(0), m_pTex(NULL), m_nRefCnt(0)
{
	EFFMEMORYLOG_CON;

	Eff2Ut_ZEROBLOCK( m_szTex );
	Eff2Ut_ZEROBLOCK( m_szMask );
}

AgcdEffTex::AgcdEffTex( const RwChar* szTex, const RwChar* szMask, RwInt32 nIndex ) : m_nIndex(nIndex), m_pTex(NULL), m_nRefCnt(0)
{
	EFFMEMORYLOG_CON;

	Eff2Ut_ZEROBLOCK( m_szTex );
	Eff2Ut_ZEROBLOCK( m_szMask );

	bSetTexMaskName( szTex, szMask );
}

AgcdEffTex::~AgcdEffTex()
{
	EFFMEMORYLOG_DES;
	Eff2Ut_SAFE_DESTROY_TEX( m_pTex );
}

RwInt32 AgcdEffTex::bAddRef()
{
	ASSERT( !T_ISMINUS4(m_nRefCnt) && "m_nRefCnt < 0" );

	if( !m_nRefCnt && !m_pTex )
	{
		RwChar	szTexPath[MAX_PATH]	= "";
		strcpy( szTexPath, AgcuEffPath::GetPath_Tex() );
		ASSERT( sizeof(szTexPath) > strlen(szTexPath) );
		m_pTex	= Eff2Ut_FindEffTex( m_szTex, m_szMask, szTexPath, AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader() );
		if( !m_pTex )
		{
			Eff2Ut_ERR( "Eff2Ut_FindEffTex() failed" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - szTex : %s, szMask : %s", m_szTex ? m_szTex : "NULL", m_szMask ? m_szMask : "NULL") );
			return -1;
		}
	}
	++m_nRefCnt;
	return m_nRefCnt;
}

RwInt32 AgcdEffTex::bRelease()
{
	--m_nRefCnt;
	if( !m_nRefCnt )
		Eff2Ut_SAFE_DESTROY_TEX( m_pTex );

	ASSERT( !T_ISMINUS4(m_nRefCnt) && "m_nRefCnt < 0" );
	return m_nRefCnt;
}

RwInt32 AgcdEffTex::bLoadTex( void )
{
	Eff2Ut_SAFE_DESTROY_TEX( m_pTex );

	RwChar	szTexPath[MAX_PATH]	= "";
	strcpy( szTexPath, AgcuEffPath::GetPath_Tex() );
	ASSERT( sizeof(szTexPath) > strlen(szTexPath) );
	m_pTex	= Eff2Ut_FindEffTex( m_szTex, m_szMask, szTexPath, AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader() );
	if( !m_pTex )
	{
		Eff2Ut_ERR( "Eff2Ut_FindEffTex() failed" );
		Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - szTex : %s, szMask : %s", m_szTex, m_szMask) );
		return -1;
	}

	return 0;
}

void AgcdEffTex::bSetTexName( const RwChar* szTex )
{
	if( szTex )
	{
		ASSERT( sizeof(m_szTex) > strlen(szTex) );
		strcpy( m_szTex, szTex );
	}
	else
		Eff2Ut_ZEROBLOCK( m_szTex );

	ASSERT( !T_ISMINUS4(m_nRefCnt) && "m_nRefCnt < 0" );
	if( m_nRefCnt )
	{
		if( bLoadTex() < 0 )
		{
			Eff2Ut_ERR( "bLoadTex() failed @ AgcdEffTex::bSetTexName" );
		}
	}
}

void AgcdEffTex::bSetMaskName( const RwChar* szMask )
{
	if( szMask )
	{
		ASSERT( sizeof(m_szMask) > strlen(szMask) );
		strcpy( m_szMask, szMask );
	}
	else
		Eff2Ut_ZEROBLOCK( m_szMask );

	ASSERT( !T_ISMINUS4(m_nRefCnt) && "m_nRefCnt < 0" );
	if( m_nRefCnt )
	{
		if( bLoadTex() < 0 )
		{
			Eff2Ut_ERR( "bLoadTex() failed @ AgcdEffTex::bSetMaskName" );
		}
	}
}

void AgcdEffTex::bSetTexMaskName( const RwChar* szTex, const RwChar* szMask )
{
	if( szTex )
	{
		ASSERT( sizeof(m_szTex) > strlen(szTex) );
		strcpy( m_szTex, szTex );
	}
	else
		Eff2Ut_ZEROBLOCK( m_szTex );

	if( szMask )
	{
		ASSERT( sizeof(m_szMask) > strlen(szMask) );
		strcpy( m_szMask, szMask );
	}
	else
		Eff2Ut_ZEROBLOCK( m_szMask );

	ASSERT( !T_ISMINUS4(m_nRefCnt) && "m_nRefCnt < 0" );
	if( m_nRefCnt )
	{
		if( T_ISMINUS4( bLoadTex() ) )
		{
			Eff2Ut_ERR( "bLoadTex() failed @ AgcdEffTex::bSetTexMaskName" );
		}
	}
}

bool AgcdEffTex::bCmpFileName(const RwChar* szTex, const RwChar* szMask)
{
	return ( !strcmp(szTex, m_szTex) && !strcmp(szMask, m_szMask) ) ? true : false;
}

RwInt32 AgcdEffTex::bToFile(FILE* fp)
{
	return fwrite( &m_nIndex, 1, sizeof(m_nIndex)+sizeof(m_szTex)+sizeof(m_szMask), fp );
}

RwInt32 AgcdEffTex::bFromFile(FILE* fp)
{
	return fread( &m_nIndex, 1, sizeof(m_nIndex)+sizeof(m_szTex)+sizeof(m_szMask), fp );
}

AgcdEffTex::AgcdEffTex(const AgcdEffTex& cpy): m_nIndex( cpy.m_nIndex ), m_pTex( cpy.m_pTex )

{
	ASSERT( sizeof(m_szTex) > strlen(cpy.m_szTex) );
	ASSERT( sizeof(m_szMask) > strlen(cpy.m_szMask) );
	strcpy( m_szTex, cpy.m_szTex );
	strcpy( m_szMask, cpy.m_szMask );
	if( m_pTex )
		RwTextureAddRef( m_pTex );

}

AgcdEffTex& AgcdEffTex::operator = (const AgcdEffTex& cpy)
{
	if( &cpy == this )
		return *this;

	m_nIndex	= cpy.m_nIndex;
	m_pTex		= cpy.m_pTex;

	ASSERT( sizeof(m_szTex) > strlen(cpy.m_szTex) );
	ASSERT( sizeof(m_szMask) > strlen(cpy.m_szMask) );
	strcpy( m_szTex, cpy.m_szTex );
	strcpy( m_szMask, cpy.m_szMask );
	if( m_pTex )
		RwTextureAddRef( m_pTex );

	return *this;
}

//---------------------------- AgcdEffTexInfo --------------------------
AgcdEffTexInfo::AgcdEffTexInfo() : m_nIndex(0), m_lpAgcdEffTex(NULL)
{
	EFFMEMORYLOG_CON;
}

AgcdEffTexInfo::~AgcdEffTexInfo()
{
	EFFMEMORYLOG_DES;

	if( m_lpAgcdEffTex )
		m_lpAgcdEffTex->bRelease();
}

RwInt32	AgcdEffTexInfo::bSetPtrEffTex( LPEFFTEX lpEffTex )
{ 
	ASSERT( "kday" && lpEffTex );
	if( lpEffTex->bAddRef() < 0 )
	{
		Eff2Ut_ERR( "lpEffTex->bAddRef() failed @ AgcdEffTexInfo::bSetPtrEffTex" );
		return -1;
	}

	m_lpAgcdEffTex = lpEffTex;
	return 0;
}
