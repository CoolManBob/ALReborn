#include "AgcdEffSet.h"
#include "AgcdEffSound.h"
#include "AgcdEffGlobal.h"
#include "AgcuEffPath.h"

#include <algorithm>

#include "ApMemoryTracker.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffSet);
EFFMEMORYLOG(AGCDEFFSET_CBARG);
EFFMEMORYLOG(AgcdPreEffSet);

AgcdEffSet::AgcdEffSet(RwUInt32 dwEffSetID) :
 m_dwEffSetID(dwEffSetID),
 m_dwEffSetLife(0),
 m_eLoopOpt( e_TblDir_none ),
 m_dwEffSetBitFlags(0x0U),
 m_nFileOffset(0),
 m_nFileSize(0),
 m_nRefCnt(0),
 m_nAcumCnt(0),
 m_dwLastShootTime(0LU),
 m_vecLPEffectEx(0),
 m_vecLPEffectBase(0),
 m_vecBaseDependancy(0),
 m_enumLoadStatus(AGCDEFFSETRESOURCELOADSTATUS_LOADING),
 m_nCurrentNumOfLoadingBase(0),
 m_bForceImmediate(TRUE)
{
	EFFMEMORYLOG_CON;

	Eff2Ut_ZEROBLOCK( m_BSphere );
	Eff2Ut_ZEROBLOCK( m_BBox );
	Eff2Ut_ZEROBLOCK( m_szTitle );
}

AgcdEffSet::~AgcdEffSet()
{
	EFFMEMORYLOG_DES;

	vClear();
}

void AgcdEffSet::vClear()
{
	for( LPEffectBaseVecItr Itr = m_vecLPEffectBase.begin(); Itr != m_vecLPEffectBase.end(); ++Itr )
		AgcuEffBaseCreater::bDestroy( (*Itr) );
	m_vecLPEffectBase.clear();

	for( LPEffectExVecItr Itr = m_vecLPEffectEx.begin(); Itr != m_vecLPEffectEx.end(); ++Itr )
		DEF_SAFEDELETE( (*Itr) );
	m_vecLPEffectEx.clear();

	m_vecBaseDependancy.clear();
}
	
RwInt32 AgcdEffSet::bGetSoundLendth(RwInt32* pnLength)
{
	if( !pnLength )
		return 0;

	for( LPEffectBaseVecItr	Itr = m_vecLPEffectBase.begin(); Itr != m_vecLPEffectBase.end(); ++Itr )
	{
		if( *Itr && (*Itr)->bGetBaseType() == AgcdEffBase::E_EFFBASE_SOUND )
		{
			AgcdEffSound* pEffSound = static_cast<AgcdEffSound*>(*Itr);
			char szSound[MAX_PATH]	= "";

			if( AgcdEffSound::EFFECT_STREAM_SOUND == pEffSound->bGetSoundType() )
				strcpy( szSound, AgcuEffPath::GetPath_SoundStream() );
			else
				strcpy( szSound, AgcuEffPath::GetPath_Sound() );

			strncat( szSound, pEffSound->bGetSoundFName(), EFF2_FILE_NAME_MAX );
			ASSERT( sizeof(szSound) > strlen(szSound) );

			*pnLength = AgcdEffGlobal::bGetInst().bGetPtrAgcmSound()->GetSampleSoundLength( pEffSound->bGetLoopCnt() < 1 ? SOUND_TYPE_3D_SOUND_LOOPED : SOUND_TYPE_3D_SOUND, szSound );

			return 1;
		}		
	}

	return 0;
}
	
RwInt32 AgcdEffSet::bAddRef()
{
	ASSERT( !T_ISMINUS4( m_nRefCnt ) && "m_nRefCnt < 0" );
	if( m_nRefCnt < 1 )
	{
		m_nRefCnt = 0;

		vAllocMem();

		if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_USEPACK) )
		{
			PROFILE("bAddRef - GetFPFEffSet()");

			if( T_ISMINUS4( bFromFileVariableData( AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->GetFPEffSet(), m_bForceImmediate ) ) )
			{
				Eff2Ut_ERR( "bFromFileVariableData() failed!" );
				vClear();
				return -1;
			}
		}
		else
		{
			if( !AgcdEffGlobal::bGetInst().bGetPtrEffIniMng()->Read_txt( m_dwEffSetID, this, TRUE ) )
			{
				Eff2Ut_ERR( "AgcdEffGlobal::bGetInst().bGetPtrEffIniMng()->bRead_txt() failed!" );
				vClear();
				return -1;
			}
		
			m_enumLoadStatus = 	AGCDEFFSETRESOURCELOADSTATUS_LOADED;		
		}
	}

	++m_nRefCnt;
	++m_nAcumCnt;

	m_dwLastShootTime	= AgcdEffGlobal::bGetInst().bGetCurrTime();

#ifndef USE_MFC
	AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->PopFromEffSetPool(this);
#endif //USE_MFC

	return m_nRefCnt;
}
	
RwInt32 AgcdEffSet::bRelease(void)
{
	if( AGCDEFFSETRESOURCELOADSTATUS_LOADED != m_enumLoadStatus )		return m_nRefCnt;

	if( m_nRefCnt < 0 )
	{
		#ifndef USE_MFC
		AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->PushToEffSetPool(this);
		#endif //USE_MFC

		--m_nRefCnt;
		ASSERT( !T_ISMINUS4( m_nRefCnt ) && "m_nRefCnt < 0" );

		if( m_nRefCnt == 0 )
			vClear();
	}

	return m_nRefCnt;
}

RwInt32 AgcdEffSet::bInsEffTex( const RwChar* szTex, const RwChar* szMask )
{
	m_vecLPEffectEx.push_back( new EFFTEX( szTex, szMask, m_vecLPEffectEx.size() ) );
	return 0;
}
	
RwInt32 AgcdEffSet::bInsEffBase( AgcdEffBase::E_EFFBASETYPE eEffBaseType )
{
	m_vecLPEffectBase.push_back( AgcuEffBaseCreater::bCreate( eEffBaseType ) );
	return 0;
}
	
RwInt32 AgcdEffSet::bInsDependancy( const stBaseDependancy& dependancy )
{
	m_vecBaseDependancy.push_back( dependancy );
	return 0;
}
	
RwInt32 AgcdEffSet::bSetEffTexName( RwInt32 nIndex, const RwChar* szTex, const RwChar* szMask )
{
	LPEFFTEX pEffTex = m_vecLPEffectEx[nIndex];
	if( !pEffTex )
	{
		Eff2Ut_ERR( "pEffTex == NULL @ AgcdEffSet::bSetEffTexName" );
		Eff2Ut_ERR( Eff2Ut_FmtMsg("ps - m_vecLPEffectEx.size() : %d, nIndex : %d", m_vecLPEffectEx.size(), nIndex) );
		return -1;
	}

	//툴에서 사용하며... 지정하고 싶은 파일이름만 입력할 수 있도록.
	if( szTex )		pEffTex->bSetTexName( szTex );
	if( szMask )	pEffTex->bSetMaskName( szMask );

	return 0;
}
	
RwInt32 AgcdEffSet::bSetDependancy( RwInt32 nIndex, const stBaseDependancy& BaseDpndncy )
{
	ASSERT( !T_ISMINUS4( nIndex ) && " @ AgcdEffSet::bSetDependancy()" );
	ASSERT( (RwInt32)m_vecBaseDependancy.size() > nIndex && " @ AgcdEffSet::bSetDependancy()" );

	m_vecBaseDependancy[nIndex]	= BaseDpndncy;

	return 0;
}

RwInt32 AgcdEffSet::bFromFile(FILE* fp)
{
	ASSERT(fp);

	int nreadbyte	= sizeof(m_dwEffSetID)
					+ sizeof(m_szTitle)
					+ sizeof(m_dwEffSetLife)
					+ sizeof(m_eLoopOpt)
					+ sizeof(m_dwEffSetBitFlags)
					+ sizeof(m_stMissileInfo)
					+ sizeof(m_BSphere)
					+ sizeof(m_BBox)
					+ sizeof(m_stVarSizeInfo)
					+ sizeof(m_nFileOffset)
					+ sizeof(m_nFileSize);

	if( nreadbyte != fread( (LPVOID)(&m_dwEffSetID), 1, nreadbyte, fp) )
	{
		ASSERT( !"fread failed" );
		return -1;
	}
	return nreadbyte;
}
	
RwInt32 AgcdEffSet::bToFile(FILE* fp)
{
	ASSERT(fp);

	RwInt32 ir = 
	fwrite( (LPCVOID)(&m_dwEffSetID), 1
		, sizeof(m_dwEffSetID)
		+ sizeof(m_szTitle)
		+ sizeof(m_dwEffSetLife)
		+ sizeof(m_eLoopOpt)
		+ sizeof(m_dwEffSetBitFlags)
		+ sizeof(m_stMissileInfo)
		+ sizeof(m_BSphere)
		+ sizeof(m_BBox)
		+ sizeof(m_stVarSizeInfo)
		+ sizeof(m_nFileOffset)
		+ sizeof(m_nFileSize)
		, fp
		);

	ASSERT( "kday" && ir == sizeof(m_dwEffSetID)
		+ sizeof(m_szTitle)
		+ sizeof(m_dwEffSetLife)
		+ sizeof(m_eLoopOpt)
		+ sizeof(m_dwEffSetBitFlags)
		+ sizeof(m_stMissileInfo)
		+ sizeof(m_BSphere)
		+ sizeof(m_BBox)
		+ sizeof(m_stVarSizeInfo)
		+ sizeof(m_nFileOffset)
		+ sizeof(m_nFileSize) );
	return ir;
}

RwInt32 AgcdEffSet::bToFileVariableData(FILE* fp)
{
	ASSERT(fp);

	RwInt32 nSize = 0;
	for( LPEffectExVecItr Itr = m_vecLPEffectEx.begin(); Itr != m_vecLPEffectEx.end(); ++Itr )
		nSize += (*Itr)->bToFile(fp);

	if( m_stVarSizeInfo.m_nNumOfBaseDependancy )
		nSize += fwrite( &m_vecBaseDependancy[0], 1, sizeof(stBaseDependancy) * m_stVarSizeInfo.m_nNumOfBaseDependancy, fp );

	for( LPEffectBaseVecItr Itr = m_vecLPEffectBase.begin(); Itr != m_vecLPEffectBase.end(); ++Itr )
		nSize += (*Itr)->bToFile(fp);

	return nSize;
}

RwInt32 AgcdEffSet::bFromFileVariableData(FILE* fp, const BOOL bForceImmediate)
{
	ASSERT(fp);
	ASSERT(m_nRefCnt == 0);

	RwInt32 nSize = 0;
	RwInt32	nr = 0;
	
	fseek( fp, m_nFileOffset, SEEK_SET );

	//tex
	for( int i=0; i<m_stVarSizeInfo.m_nNumOfTex; ++i )
	{
		LPEFFTEX pTex = new EFFTEX;
		ASSERT( pTex );
			
		nSize += pTex->bFromFile(fp);
		m_vecLPEffectEx.push_back(pTex);
	}

	//depandency
	if( m_stVarSizeInfo.m_nNumOfBaseDependancy )
	{
		m_vecBaseDependancy.resize(m_stVarSizeInfo.m_nNumOfBaseDependancy);
		nSize += fread( &m_vecBaseDependancy[0], 1, sizeof(stBaseDependancy) * m_stVarSizeInfo.m_nNumOfBaseDependancy, fp );
	}
	
	//. bForceImmediate와 eBaseType에 따라 BackThread Loading Process를 탄다.
	for( int i=0; i<m_stVarSizeInfo.m_nNumOfBase; ++i )
	{
		AgcdEffBase::E_EFFBASETYPE	eBaseType;
		fread( &eBaseType, sizeof(eBaseType), 1, fp );
		
		//. eBaseType에따른 EffBase생성. Factory Method.
		PROFILE("AgcuEffBaseCreater::bCreate");
		LPEFFBASE	pBase = AgcuEffBaseCreater::bCreate(eBaseType);
		ASSERT(pBase);
		if( !pBase )		return -1;

		pBase->m_bForceImmediate = bForceImmediate;

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//. 파일로 부터 정보를 읽어드린 후 로딩하는 리소스들
		//. 파일이름이나 기타 정보는 무조건 Main Thread에서 로딩하고.. 일단 Clump까지만 BackThread로 돌림.
		PROFILE("pBase->bFromFile");
		nr = pBase->bFromFile(fp);
		if( T_ISMINUS4(nr) )
		{
			ASSERT( !"pBase->bFromFile failed" );
			AgcuEffBaseCreater::bDestroy(pBase);
			return -1;
		}
		//.
		////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//. Back thread을 사용하고 BaseType은 Clump, Texure일 때
		//. AddLoadEntry에 들어갈 인자구조체 설정, Loading이 끝난 후 delete.
		AGCDEFFSET_CBARG*	pArg = new AGCDEFFSET_CBARG;
		pArg->m_pOwnerSet	= this;
		pArg->m_pLoadBase	= pBase;
		pArg->m_eBaseType	= eBaseType;

		//. AgcmEff2에 등록된 AddLoadEntry CallBack을 이용하여 로딩.
		AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->AddLoadEntry( AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->m_lLoaderLoadID, pArg, NULL, bForceImmediate );
																		
		nSize += nr;
		m_vecLPEffectBase.push_back(pBase);
	}

	return nSize;
}
	
void AgcdEffSet::vAllocMem(void)
{
	m_vecLPEffectEx.reserve( m_stVarSizeInfo.m_nNumOfTex );
	m_vecLPEffectBase.reserve( m_stVarSizeInfo.m_nNumOfBase );
	m_vecBaseDependancy.reserve( m_stVarSizeInfo.m_nNumOfBaseDependancy );
}
	
#ifdef USE_MFC
BOOL AgcdEffSet::bForTool_IsTexUsed( RwInt32 nTexIndex )
{
	LPEffectBaseVecItr	Itr	= m_vecLPEffectBase.begin();
	LPEffectBaseVecItr	it_last	= m_vecLPEffectBase.end();

	for( ; Itr!=it_last; ++Itr )
	{
		if( (*Itr)->bIsRenderBase() )
		{
			if( nTexIndex == 
				static_cast<AgcdEffRenderBase*>(*Itr)->m_cEffTexInfo.m_nIndex
				)
				return TRUE;
		}
	}

	return FALSE;
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
BOOL AgcdEffSet::bForTool_IsAllTexUsed( RwChar* szOut )
{
	BOOL br	= TRUE;
	LPEffectExVecCItr	Itr	= m_vecLPEffectEx.begin();
	LPEffectExVecCItr	it_last	= m_vecLPEffectEx.end();

	for( ; Itr != it_last; ++Itr )
	{
		if( !bForTool_IsTexUsed( (*Itr)->bGetIndex() ) )
		{
			br	= FALSE;
			strcat(szOut, (*Itr)->bGetTexName());
			strcat(szOut, "\r\n");
		}
	}

	return br;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_FindEffTex( const RwChar* szTex, const RwChar* szMask )
{
	LPEFFTEX pTex = NULL;
	for( size_t i=0; i<m_vecLPEffectEx.size(); ++i )
	{
		pTex	= m_vecLPEffectEx[i];
		if( pTex->bCmpFileName( szTex, szMask ) )
			return static_cast<RwInt32>(i);
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_InsEffEffTex( const RwChar* szTex, const RwChar* szMask )
{
	RwInt32 ir = bForTool_FindEffTex(szTex, szMask);
	if( !T_ISMINUS4(ir) )
	{
		//이미 존재한다...
		return ir;
	}

	if( T_ISMINUS4( bInsEffTex(szTex, szMask) ) )
	{
		Eff2Ut_ERR( "bInsEffTex failed @ AgcdEffSet::bForTool_InsEffEffTex" );
		return -1;
	}

	++m_stVarSizeInfo.m_nNumOfTex;

	return ( m_stVarSizeInfo.m_nNumOfTex - 1 );
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_InsEffBase( AgcdEffBase::E_EFFBASETYPE eEffBaseType )
{
	if( T_ISMINUS4(bInsEffBase(eEffBaseType)) )
	{
		Eff2Ut_ERR( "bInsEffBase failed @ AgcdEffSet::bForTool_InsEffBase" );
		return -1;
	}

	return ++m_stVarSizeInfo.m_nNumOfBase;
}
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_InsEffBase( AgcdEffBase* pEffBase )
{
	m_vecLPEffectBase.push_back( pEffBase );
	return ++m_stVarSizeInfo.m_nNumOfBase;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_InsDependancy( const stBaseDependancy& dependancy )
{
	LPEFFBASE	pParent = bGetPtrEffBase( dependancy.GetParentIndex() );
	LPEFFBASE	pChild  = bGetPtrEffBase( dependancy.GetChildIndex() );
	ASSERT( pParent && pChild );
	ASSERT( !pChild->bFlagChk( FLAG_EFFBASE_BASEDPND_CHILD ) );
	if( T_ISMINUS4(bInsDependancy(dependancy)) )
	{
		Eff2Ut_ERR( "bInsDependancy failed @ AgcdEffSet::bForTool_InsDependancy" );
		return -1;
	}

	pParent->bFlagOn( FLAG_EFFBASE_BASEDEPENDANCY | FLAG_EFFBASE_BASEDPND_PARENT );
	pChild->bFlagOn( FLAG_EFFBASE_BASEDEPENDANCY | FLAG_EFFBASE_BASEDPND_CHILD );

	return ++m_stVarSizeInfo.m_nNumOfBaseDependancy;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_DelEffTex( RwInt32 nIndex )
{
	ASSERT( !T_ISMINUS4( nIndex ) && "nIndex < 0 @ AgcdEffSet::bDelEffTex()" );
	ASSERT( (RwInt32)m_vecLPEffectEx.size() > nIndex && "m_vecLPEffectEx.size() <= nIndex @ AgcdEffSet::bDelEffTex()" );

	LPEffectExVecItr	it_del
		= m_vecLPEffectEx.begin();
	for( RwInt32 delindex=nIndex; it_del != m_vecLPEffectEx.end(); ++it_del, --delindex )
	{
		if( !delindex )
		{
			DEF_SAFEDELETE( *it_del );
			m_vecLPEffectEx.erase( it_del );
			--m_stVarSizeInfo.m_nNumOfTex;
			break;
		}
	}
	int	i = 0;
	for( LPEffectExVecItr Itr=m_vecLPEffectEx.begin();
	     Itr != m_vecLPEffectEx.end(); 
		 ++Itr, ++i )
	{
		(*Itr)->bSetIndex(i);
	}

	LPEffectBaseVecItr	itbase_curr = m_vecLPEffectBase.begin();
	for( ; itbase_curr != m_vecLPEffectBase.end(); ++itbase_curr )
	{
		if((*itbase_curr)->bIsRenderBase())
		{
			AgcdEffRenderBase* pRenderBase = static_cast<AgcdEffRenderBase*>(*itbase_curr);
			if( !pRenderBase 
				|| T_ISMINUS4(pRenderBase->m_cEffTexInfo.m_nIndex) 
				|| (*itbase_curr)->bGetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT
				)
			{
				ASSERT( "kday" && (*itbase_curr)->bGetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT );
				continue;
			}

			ASSERT( pRenderBase->m_cEffTexInfo.m_lpAgcdEffTex );
			if( pRenderBase->bGetTexIndex() == nIndex )
			{
				Eff2Ut_ERR("pRenderBase->m_cEffTexInfo.m_nIndex == nIndex 이런 경우가 있으면 안된다.");
			}

			if( pRenderBase->bGetTexIndex() > nIndex )
			{
				pRenderBase->bSetTexIndex( pRenderBase->bGetTexIndex() - 1 );
			}

			//pRenderBase->bSetPtrEffTex( bGetPtrEffTex(pRenderBase->bGetTexIndex()) );
		}
	}

	return m_stVarSizeInfo.m_nNumOfTex;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_DelAllEffTexNotUsed(void)
{
	RwInt32 delCount = 0;
	LPEffectExVecCItr	Itr	= m_vecLPEffectEx.begin();

	int nPos = 0;
	for( ; Itr != m_vecLPEffectEx.end(); )
	{
		if( !bForTool_IsTexUsed( (*Itr)->bGetIndex() ) )
		{
			bForTool_DelEffTex( (*Itr)->bGetIndex() );
			++delCount;
			Itr	= m_vecLPEffectEx.begin();
			std::advance( Itr, nPos );
			continue;
		}
		++nPos;
		++Itr;
	}

	return delCount;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
BOOL AgcdEffSet::bForTool_ChkDependancy( RwInt32 nBaseIndex )
{
	BaseDependancyVecItr Itr = m_vecBaseDependancy.begin();
	stBaseDependancy*	pDpnd	= NULL;
	for( ; Itr != m_vecBaseDependancy.end(); ++Itr )
	{
		pDpnd	= &(*Itr);
		if( pDpnd->GetParentIndex() == nBaseIndex ||
			pDpnd->GetChildIndex() == nBaseIndex )
			return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
class ftr_jny
{
	int m_nBaseIndex;
public:
	ftr_jny(int nBaseIndex) : m_nBaseIndex(nBaseIndex){};
	bool operator() (const AgcdEffSet::stBaseDependancy& dpnd)
	{
		if( dpnd.GetChildIndex() == m_nBaseIndex ||
			dpnd.GetParentIndex() == m_nBaseIndex )
			return true;
		return false;
	}
};

int AgcdEffSet::bForTool_DelDependancyAll( RwInt32 nBaseIndex )
{
	BaseDependancyVecItr	it_r =
	std::remove_if( m_vecBaseDependancy.begin(), m_vecBaseDependancy.end(), ftr_jny(nBaseIndex) );

	int ir = (int)(m_vecBaseDependancy.end() - it_r);
	m_vecBaseDependancy.erase(it_r, m_vecBaseDependancy.end());

	m_stVarSizeInfo.m_nNumOfBaseDependancy = m_vecBaseDependancy.size();

	return ir;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_FindBaseIndex( LPEFFBASE pBase )
{
	LPEffectBaseVecItr	Itr = m_vecLPEffectBase.begin();

	int nIndex=0;
	for( ; Itr != m_vecLPEffectBase.end(); ++Itr, ++nIndex )
	{
		if( pBase == (*Itr) )
			return nIndex;
	}

	return -1;
}
	
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_DelEffBase( RwInt32 nIndex )
{
	ASSERT( !T_ISMINUS4( nIndex ) && " @ AgcdEffSet::bDelEffBase()" );
	ASSERT( (RwInt32)m_vecLPEffectBase.size() > nIndex && " @ AgcdEffSet::bDelEffBase()" );

	LPEffectBaseVecItr	it_del = m_vecLPEffectBase.begin();
	for( ; it_del != m_vecLPEffectBase.end(); ++it_del, --nIndex )
	{
		if( !nIndex )
		{
			DEF_SAFEDELETE( *it_del );
			m_vecLPEffectBase.erase( it_del );
			return (--m_stVarSizeInfo.m_nNumOfBase);
		}
	}

	return -1;
}
	
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_DelDependancy( RwInt32 nIndex )
{
	ASSERT( !T_ISMINUS4( nIndex ) && " @ AgcdEffSet::bDelDependancy()" );
	ASSERT( (RwInt32)m_vecBaseDependancy.size() > nIndex && " @ AgcdEffSet::bDelDependancy()" );

	BaseDependancyVecItr	it_del = m_vecBaseDependancy.begin();
	for( ; it_del != m_vecBaseDependancy.end(); ++it_del, --nIndex )
	{
		if( !nIndex )
		{
			m_vecBaseDependancy.erase( it_del );
			break;
		}
	}

	m_stVarSizeInfo.m_nNumOfBaseDependancy = m_vecBaseDependancy.size();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_ChangeBaseTexIndex(AgcdEffRenderBase* pRenderBase, RwInt32 nOldIndex, RwInt32 nNewIndex)
{
	ASSERT( pRenderBase && nNewIndex < m_stVarSizeInfo.m_nNumOfTex );


	LPEFFTEX	pTex = bGetPtrEffTex(nNewIndex);
	if( !pTex ){
		Eff2Ut_ERR( "bGetPtrEffTex failed" );
		return -1;
	}
	pRenderBase->bSetTexIndex( nNewIndex );
	pRenderBase->bSetPtrEffTex( pTex );

	pTex = bGetPtrEffTex(nOldIndex);
	if( !pTex )
	{
		Eff2Ut_ERR( "!pTex" );
		return -1;
	}

	RwInt32	refCnt = pTex->bRelease();
	if( T_ISMINUS4(refCnt) )
	{
		Eff2Ut_ERR( "refCnt < 0" );
		return -1;
	}

	if( refCnt == 0 )
	{
		if( T_ISMINUS4( bForTool_DelEffTex(nOldIndex) ) )
		{
			Eff2Ut_ERR( "bForTool_DelEffTex(nOldIndex) failed" );
			return -1;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_FindDependancy(const stBaseDependancy& dependancy )const
{
	BaseDependancyVecCItr it_f =
	std::find( m_vecBaseDependancy.begin(), m_vecBaseDependancy.end(), dependancy );
	if( it_f == m_vecBaseDependancy.end() )
		return -1;

	return static_cast<RwInt32>(std::distance( m_vecBaseDependancy.begin(), it_f ));
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSet::bForTool_FlagChk( RwUInt32 flag )const
{
	return DEF_FLAG_CHK(m_dwEffSetBitFlags, flag);
};

#endif