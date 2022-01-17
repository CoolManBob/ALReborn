// AgcdEffSetMng.cpp: implementation of the AgcdEffSetMng class.
//
///////////////////////////////////////////////////////////////////////////////
#include "AgcdEffSetMng.h"
#include "AgcuEffUtil.h"
#include "AgcdEffGlobal.h"
#include "AgcuEffPath.h"

#include "ApMemoryTracker.h"

#include "AgcdEffCtrl.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

///////////////////////////////////////////////////////////////////////////////
// Destruction
///////////////////////////////////////////////////////////////////////////////
AgcdEffSetMng::AgcdEffSetMng() : m_POOLNUM_EFFSET(50)
{
}

///////////////////////////////////////////////////////////////////////////////
// Destruction
///////////////////////////////////////////////////////////////////////////////
AgcdEffSetMng::~AgcdEffSetMng()
{
	_ClearEffSet();
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
void AgcdEffSetMng::_ClearEffSet()
{
	LPEffectSetMapItr	it_curr
		= m_mapEffectSet.begin();
	for( ; it_curr != m_mapEffectSet.end(); ++it_curr )
	{
		LPEFFSET pDel	= (*it_curr).second;

		DEF_SAFEDELETE( pDel );
	}

	m_mapEffectSet.clear();
	//@{ kday 20051207
	// ;)
	m_PoolOfEffSet.clear();
	//@} kday
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSetMng::_InsertEffSet( LPEFFSET lpNewEffSet )
{
	if( FindEffSet(lpNewEffSet->bGetID()) )
	{
		Eff2Ut_ERR(" this effset is already exist ! " );
		return -1;
	}
	
	m_mapEffectSet.insert( 
		LPEffectSetMap::value_type( lpNewEffSet->bGetID(), lpNewEffSet ) );
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffSetMng::_DeleteEffSet(RwUInt32 dwID)
{
	LPEffectSetMapItr	it_del	= m_mapEffectSet.find( dwID );
	if( it_del == m_mapEffectSet.end() )
		return -1;
	
	LPEFFSET pDel	= (*it_del).second;

	DEF_SAFEDELETE(pDel);
	m_mapEffectSet.erase( it_del );

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffSetMng::_DeleteEffSet(LPEFFSET lpNewEffSet)
{
	return _DeleteEffSet(lpNewEffSet->bGetID());
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
LPEFFSET AgcdEffSetMng::FindEffSet(RwUInt32 dwID)
{
	LPEffectSetMapItr	it_f	= m_mapEffectSet.find( dwID );
	if( it_f == m_mapEffectSet.end() )
	{
		return NULL;
	}
	
	return (*it_f).second;
}

///////////////////////////////////////////////////////////////////////////////
// bLoadEffSet
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffSetMng::_LoadEffSet(const RwChar* szFName, void* pAgcdEffSetMng)
{
	LPEFFSET pNewEffSet	= new AgcdEffSet;

	if( !AgcdEffGlobal::bGetInst().bGetPtrEffIniMng()->Read_txt( szFName, pNewEffSet, FALSE ) )
	{
		DEF_SAFEDELETE( pNewEffSet );
		Eff2Ut_ERR( "AgcdEffGlobal::bGetInst().bGetPtrEffIniMng()->bRead_txt failed @ LoadEffSet" );
		return -1;
	}

	RwInt32	ir	=
	static_cast<AgcdEffSetMng*>(pAgcdEffSetMng)->_InsertEffSet( pNewEffSet );
	if( T_ISMINUS4( ir ) )
	{
		Eff2Ut_ERR( "vIns failed @ AgcdEffSetMng::bLoadEffSet" );
		DEF_SAFEDELETE( pNewEffSet );
		return -1;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// bLoadAll
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffSetMng::LoadAll(void)
{
	_ClearEffSet();

	RwInt32 ir	=
		::Eff2Ut_ForAllFile( AgcuEffPath::GetPath_Ini(), ".ini", AgcdEffSetMng::_LoadEffSet, (void*)this );
	if( T_ISMINUS4( ir ) )
	{
		Eff2Ut_ERR( "::Eff2Ut_ForAllFile failed @ AgcdEffSetMng::bLoadAll" );
		return -1;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////
// bMakeEffFile
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffSetMng::MakeEffFile(LPCSTR packingpath, AcCallbackData1 pfOutputCallback)
{
	//if( packingpath )
	//{
	//	this->vClear();
	//	
	//	RwInt32 chk =  ::Eff2Ut_ForAllFile( packingpath, ".ini", AgcdEffSetMng::vLoadEffSet, (void*)this );
	//	if( T_ISMINUS4( chk ) )
	//	{
	//		Eff2Ut_ERR( "::Eff2Ut_ForAllFile failed @ AgcdEffSetMng::bLoadAll" );
	//		return -1;
	//	}
	//}


	RwInt32 ir = 0;
	char	buff[MAX_PATH] = "";

	//variable data
	LPCSTR	EFFFILE_VAR = "efffile_var.eff";
	strcpy( buff, packingpath ? packingpath : AgcuEffPath::GetPath_Ini() );
	strcat( buff, EFFFILE_VAR );
	AuAutoFile	fp( buff, "wb" );
	ASSERT( fp );
	if( !fp )
		return -1;
	LPEffectSetMapItr	it_curr = m_mapEffectSet.begin(),
						it_last = m_mapEffectSet.end();
	LPEFFSET pEffSet = NULL;
	//@{ kday 20050711
	// ;)
	INT nNumEffSet = m_mapEffectSet.size();

	//@} kday
	for( ; it_curr != it_last; ++it_curr )
	{
		pEffSet = (*it_curr).second;		
		if( T_ISMINUS4(pEffSet->bAddRef()) )
		{
			ASSERT( !"pEffSet->bAddRef failed" );

			if( pfOutputCallback )
			{
				pfOutputCallback( (PVOID)Eff2Ut_FmtMsg("[effid : %d] AddRef failed @ AgcdEffSetMng::bMakeEffFile", pEffSet->bGetID()) );
			}
			pEffSet->m_nFileSize	= -1;
			//return -1;			
			//@{ kday 20050711
			// ;)
			--nNumEffSet;
			//@} kday
			continue;
		}
		
		pEffSet->m_nFileOffset = ir;
		pEffSet->m_nFileSize = pEffSet->bToFileVariableData(fp);
		ir += pEffSet->m_nFileSize;

		pEffSet->bRelease();
	}

	if( pfOutputCallback &&
		static_cast<INT>(m_mapEffectSet.size()) != nNumEffSet )
	{
		pfOutputCallback( (PVOID)Eff2Ut_FmtMsg(
			"%d / %d(failed / total) AddRef failed @ AgcdEffSetMng::bMakeEffFile"
			, nNumEffSet
			, m_mapEffectSet.size() 
			) 
			);
	}
//	return 1;

	//headerfile
	LPCSTR	EFFFILE_SET = "efffile_set.eff";
	memset(buff, 0, sizeof(buff));
	strcpy( buff, packingpath ? packingpath : AgcuEffPath::GetPath_Ini() );
	strcat( buff, EFFFILE_SET );

	AuAutoFile fpSet( buff, "wb" );
	ASSERT(fpSet);
	if( !fpSet )
		return -1;
	//@{ kday 20050711
	// ;)
	//INT nNumEffSet = m_mapEffectSet.size();
	//@} kday
	fwrite( &nNumEffSet, sizeof(nNumEffSet), 1, fpSet );
	for( it_curr=m_mapEffectSet.begin(); it_curr != it_last; ++it_curr )
	{
		pEffSet = (*it_curr).second;
		//@{ kday 20050711
		// ;)
		if( T_ISMINUS4( pEffSet->m_nFileSize ) )
			continue;
		//@} kday
		ir = pEffSet->bToFile(fpSet);

		ASSERT( "kday" && ir );
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// bPushToEffSetPool
///////////////////////////////////////////////////////////////////////////////
class fctrOlderEffSet
{
public:
	bool operator () (LPEFFSET pEffSet1, LPEFFSET pEffSet2)
	{
		return( pEffSet1->bGetLastShootTime() < pEffSet2->bGetLastShootTime() );
	}

};
VOID AgcdEffSetMng::PushToEffSetPool(LPEFFSET lpEffSet)
{
	if( lpEffSet->bGetRefCnt() > 1 )
		return;

	ASSERT( lpEffSet->bGetRefCnt() == 1 );

	if( (RwInt32) m_PoolOfEffSet.size() < this->m_POOLNUM_EFFSET )
	{
		ASSERT( m_PoolOfEffSet.end() == 
			std::find( m_PoolOfEffSet.begin(), m_PoolOfEffSet.end(), lpEffSet ) );

		lpEffSet->bAddRef();
		m_PoolOfEffSet.push_back(lpEffSet);
	}

	else
	{
		typedef LPEffectList::iterator	ITR;
		ITR	it_f
			= std::min_element( 
				m_PoolOfEffSet.begin()
				, m_PoolOfEffSet.end()
				, fctrOlderEffSet()
				);

		if( (*it_f)->bGetLastShootTime() < lpEffSet->bGetLastShootTime() )
		{
			(*it_f)->bRelease();

			lpEffSet->bAddRef();

			(*it_f)	= lpEffSet;
		}
	}
};
///////////////////////////////////////////////////////////////////////////////
// bPopFromEffSetPool
///////////////////////////////////////////////////////////////////////////////
VOID AgcdEffSetMng::PopFromEffSetPool(LPEFFSET lpEffSet)
{
	ASSERT( lpEffSet->bGetRefCnt() >= 1 );
	if( lpEffSet->bGetRefCnt() == 2 )
	{		
		typedef LPEffectList::iterator	ITR;
		ITR	it_f
			= std::find(
				m_PoolOfEffSet.begin()
				, m_PoolOfEffSet.end()
				, lpEffSet
				);

		if( it_f != m_PoolOfEffSet.end() )
		{
			(*it_f)->bRelease();
			m_PoolOfEffSet.erase(it_f);
		}
	}

#ifdef _DEBUG
	else
	{
		typedef LPEffectList::iterator	ITR;
		ITR	it_f = std::find(
					  m_PoolOfEffSet.begin()
					, m_PoolOfEffSet.end()
					, lpEffSet
					);
		ASSERT( it_f == m_PoolOfEffSet.end() );
	}
#endif //_DEBUG
};

///////////////////////////////////////////////////////////////////////////////
// bLoadEffFile
///////////////////////////////////////////////////////////////////////////////
RwInt32	AgcdEffSetMng::LoadEffFile(void)
{
	_ClearEffSet();

	//headerfile
	LPCSTR	EFFFILE_SET = "efffile_set.eff";
	char	buff[MAX_PATH] = "";
	strcpy( buff, AgcuEffPath::GetPath_Ini() );
	strcat( buff, EFFFILE_SET );
	AuAutoFile fp( buff, "rb" );
	ASSERT(fp && "efffile_set.eff");
	if( !fp )
		return -1;
	INT nNumEffSet = 0;//m_mapEffectSet.size();
	fread( &nNumEffSet, sizeof(nNumEffSet), 1, fp );
	for( int i=0; i<nNumEffSet; ++i )
	{
		LPEFFSET pEffSet = new EFFSET;
		if( T_ISMINUS4(pEffSet->bFromFile(fp)) )
		{
			ASSERT( !"pEffSet->bFromFile failed" );
			continue;
			//return -1;
		}
		else
			pEffSet->bAddRef();
			
		m_mapEffectSet.insert( 
			LPEffectSetMap::value_type( pEffSet->bGetID(), pEffSet ) );
	}

	ASSERT( "kday" && nNumEffSet == static_cast<int>(m_mapEffectSet.size()) );
	return 0;
}
///////////////////////////////////////////////////////////////////////////////
// bShowState
///////////////////////////////////////////////////////////////////////////////
RwInt32 AgcdEffSetMng::ShowState(RwInt32 nCtrlCnt)
{	
	static char buff[MAX_PATH]	= "";
	static BOOL b1st = TRUE;
	if( b1st )
	{
		b1st = FALSE;
		SYSTEMTIME st;	
		memset(&st, 0, sizeof(st));		
		GetLocalTime(&st);
		sprintf(buff,"effCurrState_%04d%02d%02d.txt",
			st.wYear,st.wMonth,st.wDay);
	}

	AuAutoFile fp( buff, "a" );
	if( !fp )
		return -1;

	Eff2Ut_TimeStampToFile(fp);
	RwInt32	ir = 0;
	RwInt32 nTotalAcum = 0;
	LPEffectSetMapItr	it_curr = m_mapEffectSet.begin(),
						it_last = m_mapEffectSet.end();
	
	for( ; it_curr != it_last; ++it_curr )
	{
		LPEFFSET pEffSet = (*it_curr).second;
		if( !pEffSet )
		{
			fprintf( fp, "err\r\n" );
			continue;
		}

		fprintf( fp, "id : %8u, life : %8d, count : %4d, accum : %d\r\n", pEffSet->bGetID(), pEffSet->bGetLife(), pEffSet->bGetRefCnt(), pEffSet->bGetAcumCnt() );
		ir += pEffSet->bGetRefCnt();
		nTotalAcum += pEffSet->bGetAcumCnt();
	}
	fprintf( fp, "total count : %d\r\ntotal accum : %d\r\nctrl count : %d\r\n", ir, nTotalAcum, nCtrlCnt );
	
#if defined(EFF2_APMEMORY_LOG)
	fprintf( fp, "[%s]\r\ncount : %d, max : %d, new : %d, del : %d\r\n"
		, AgcdEffSet::stApMemoryLog.m_szClassName
		, AgcdEffSet::stApMemoryLog.m_ulCnt
		, AgcdEffSet::stApMemoryLog.m_ulMax
		, AgcdEffSet::stApMemoryLog.m_ulTotalNew
		, AgcdEffSet::stApMemoryLog.m_ulTotalDel );
	fprintf( fp, "[%s]\r\ncount : %d, max : %d, new : %d, del : %d\r\n"
		, AgcdEffCtrl_Set::stApMemoryLog.m_szClassName
		, AgcdEffCtrl_Set::stApMemoryLog.m_ulCnt
		, AgcdEffCtrl_Set::stApMemoryLog.m_ulMax
		, AgcdEffCtrl_Set::stApMemoryLog.m_ulTotalNew
		, AgcdEffCtrl_Set::stApMemoryLog.m_ulTotalDel );
#endif //EFF2_APMEMORY_LOG

	return ir;
}