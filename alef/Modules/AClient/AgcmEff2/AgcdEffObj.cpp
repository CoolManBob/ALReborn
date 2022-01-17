#include "AgcuEffUtil.h"
#include "AgcdEffObj.h"
#include "AgcuEffPath.h"
#include "AgcdEffGlobal.h"

#include "AcuObject.h"

#include "ApMemoryTracker.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffObj);

AgcdEffObj::AgcdEffObj( AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType) : AgcdEffRenderBase( AgcdEffBase::E_EFFBASE_OBJECT, eBlendType ),
 m_pClump(NULL)
{
	EFFMEMORYLOG_CON;

	Eff2Ut_ZEROBLOCK(m_szClumpFName);
	Eff2Ut_ZEROBLOCK(m_rgbaPreLit);
}

AgcdEffObj::~AgcdEffObj()
{
	EFFMEMORYLOG_DES;

	Eff2Ut_SAFE_DESTROY_CLUMP(m_pClump);
}

RwInt32 AgcdEffObj::bSetClumpName(const RwChar* szClump)
{
	if( !szClump )
	{
		Eff2Ut_ERR( "AgcdEffObj::bSetClumpName failed : szClump == NULL" );
		return -1;
	}

	if( !strlen( szClump ) )
	{
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
		m_pClump		= RpClumpCreate();
		if( !m_pClump )
		{
			//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
			Eff2Ut_ERR( "AgcdEffObj::bSetClumpName failed : szClump == \"\"" );
			return -1;
		}
		
		RwFrame* pFrm	= RwFrameCreate();
		RwFrameSetIdentity( pFrm );
		ASSERT( pFrm );
		if( !RpClumpSetFrame( m_pClump, pFrm ) )
		{
			Eff2Ut_ERR( "RpClumpSetFrame failed @ AgcdEffObj::bSetClumpName" );
			return -1;
		}
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
	}
	else
	{
		vLoadClump( szClump );
		if( !m_pClump )
		{
			Eff2Ut_ERR( "AgcdEffObj::bSetClumpName failed : vLoadClump failed" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg( "PS - szClum : %s", szClump ) );
			return -2;
		}

		strcpy( m_szClumpFName, szClump );
		ASSERT( sizeof(m_szClumpFName) > strlen(szClump) );
	}

	return 0;
}

RpClump* AgcdEffObj::bLoadClump(void)
{
	if( !strlen( m_szClumpFName ) )
	{
		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->LockFrame();
		RpClump*	pClump = RpClumpCreate();
		if( !pClump )
		{
			//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();
			Eff2Ut_ERR( "AgcdEffObj::bLoadClump failed : szClump == \"\"" );
			return NULL;
		}

		RwFrame* pFrm	= RwFrameCreate();
		RwFrameSetIdentity( pFrm );
		ASSERT( pFrm );
		if( !RpClumpSetFrame( pClump, pFrm ) )
		{
			Eff2Ut_ERR( "RpClumpSetFrame failed @ AgcdEffObj::bLoadClump" );
		}

		//. AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UnLockFrame();

		return pClump;
	}
	else
	{
		RwChar	szFullPath[MAX_PATH]	= "";
		strcpy( szFullPath, AgcuEffPath::GetPath_Clump() );
		strcat( szFullPath, m_szClumpFName );
		ASSERT( sizeof(szFullPath) > strlen(szFullPath) );

		RwChar	szTexPath[MAX_PATH]		= "";
		strncpy(szTexPath, AgcuEffPath::GetPath_Tex(), MAX_PATH-1);

		RpClump* pClump = AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->LoadClump( szFullPath, NULL, NULL, rwFILTERLINEAR, szTexPath );
		if( pClump )
		{
			AcuObject::SetClumpPreLitLim( pClump, &m_rgbaPreLit );
		}
		else
		{
			Eff2Ut_ERR("LoadClump failed @ AgcdEffObj::bLoadClump" );
			Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - %s", m_szClumpFName) );
		}

		return pClump;
	}
}

RpClump* AgcdEffObj::vLoadClump(const RwChar* szClump)
{	
	Eff2Ut_SAFE_DESTROY_CLUMP( m_pClump );

	RwChar	szFullPath[MAX_PATH]	= "";
	strcpy( szFullPath, AgcuEffPath::GetPath_Clump() );
	strcat( szFullPath, szClump );
	ASSERT( sizeof(szFullPath) > strlen(szFullPath) );

	RwChar	szTexPath[MAX_PATH]		= "";
	strncpy(szTexPath, AgcuEffPath::GetPath_Tex(), MAX_PATH-1);

	m_pClump = AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->LoadClump( szFullPath, NULL, NULL, rwFILTERLINEAR, szTexPath );
	if( !m_pClump )
	{
		Eff2Ut_ERR("LoadClump failed @ AgcdEffObj::vLoadClump" );
		Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - %s", szClump) );
		return NULL;
	}

	AcuObject::SetClumpPreLitLim( m_pClump, &m_rgbaPreLit );
	return m_pClump;
}

RwInt32	AgcdEffObj::bFindNodeIndex( RwInt32 nodeID )
{
	ASSERT( m_pClump );

	if( bFlagChk( FLAG_EFFBASEOBJ_DUMMY ) )		return -1;

	RpHAnimHierarchy* pHierarchy = Eff2Ut_GetHierarchyFromRwFrame( RpClumpGetFrame( m_pClump ) );
	return pHierarchy ? RpHAnimIDGetIndex( pHierarchy, nodeID ) : -1;
};

RwInt32	AgcdEffObj::bToFile(FILE* fp)
{
	RwInt32 nSize = AgcdEffBase::tToFile(fp);
	nSize += AgcdEffRenderBase::tToFile(fp);

	nSize += fwrite( &m_szClumpFName, 1, sizeof(m_szClumpFName), fp );
	nSize += fwrite( &m_rgbaPreLit, 1, sizeof(m_rgbaPreLit), fp );

	nSize += AgcdEffBase::tToFileVariableData(fp);
	return nSize;
}

RwInt32	AgcdEffObj::bFromFile(FILE* fp)
{
	RwInt32 nSize = AgcdEffBase::tFromFile(fp);
	nSize += AgcdEffRenderBase::tFromFile(fp);

	nSize += fread( &m_szClumpFName, 1, sizeof(m_szClumpFName), fp );
	if( DEF_FLAG_CHK( bGetFlag(), FLAG_EFFBASEOBJ_DUMMY ) )
	{
		bSetClumpName( "" );
	}
	else
	{
		//. Main Thread일때만 직접 읽음.
		if(AgcdEffBase::m_bForceImmediate)
		{
			if( !vLoadClump(m_szClumpFName) )
			{
				ASSERT( !"vLoadClump failed" );
				return -1;
			}
		}
	}
	nSize += fread( &m_rgbaPreLit, 1, sizeof(m_rgbaPreLit), fp );

	RwInt32 nSize2 = AgcdEffBase::tFromFileVariableData(fp);
	if( T_ISMINUS4(nSize2) )
	{
		ASSERT( !"AgcdEffBase::tFromFileVariableData failed" );
		return -1;
	}

	return nSize+nSize2;
}

#ifdef USE_MFC
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
INT32 AgcdEffObj::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT );
	if( T_ISMINUS4( AgcdEffBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}
	if( T_ISMINUS4( AgcdEffRenderBase::bForTool_Clone( pEffBase ) ) )
	{
		return -1;
	}

	AgcdEffObj* pEffObj = static_cast<AgcdEffObj*>(pEffBase);
	bSetClumpName( pEffObj->bGetClumpName() );
	bSetPreLit( pEffObj->bGetPreLit() );

	ASSERT( "kday" && m_pClump );
	return 0;
};
#endif//USE_MFC