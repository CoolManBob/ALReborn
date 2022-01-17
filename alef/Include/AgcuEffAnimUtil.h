#if !defined(AFX_AGCUEFFANIMUTIL_H__2562EB9D_E064_485A_A5C3_6E32B72ED9F3__INCLUDED_)
#define AFX_AGCUEFFANIMUTIL_H__2562EB9D_E064_485A_A5C3_6E32B72ED9F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffUtil.h"
#include "rwcore.h"
#include <algorithm>
#include <vector>
using std::vector;

enum eTblDir
{
	e_TblDir_none		= 0,	//반복 없음.
	e_TblDir_infinity,			//무한..
	e_TblDir_oned,				//한방향반복..
	e_TblDir_posi,				//양방향 반복중 + 방향
	e_TblDir_nega,				//양방향 반복중 - 방향
	e_TblDir_end,				//끝!
	e_TblDir_num,
};
typedef eTblDir	E_LOOPOPT;

typedef struct tagStAngle
{
	RwReal	m_fYaw,		//y axis
			m_fPitch,	//x axis
			m_fRoll;	//z axis

	tagStAngle() : m_fYaw(0.f), m_fPitch(0.f), m_fRoll(0.f)		{		};
}STANGLE, *PSTANGLE, *LPSTANGLE;

RwInt32 CalcKeyTime( RwUInt32* pdwKeyTime, E_LOOPOPT eLoopOpt, RwUInt32  dwAccumulateTime, RwUInt32  dwLastTime);

typedef struct stTimeTableLoop
{
public:
	explicit stTimeTableLoop(eTblDir eLoopOpt=e_TblDir_none) : m_eLoopDir( eLoopOpt ), m_dwCurrTime( 0 )		{		};

	eTblDir		bGetLoopDir			( VOID )	const			{	return m_eLoopDir;		}
	RwUInt32	bGetCurrTime		( VOID )	const			{	return m_dwCurrTime;	}
	void		bSetCurrTime		( RwUInt32 dwCurrTime)		{	m_dwCurrTime = dwCurrTime; }

	void		bSetLoopDir			( eTblDir eLooDir )			{	m_eLoopDir = eLooDir;	}
	RwInt32		bAddTime			( RwUInt32 dwDiffTick, RwUInt32 dwLastTime );

private:
	eTblDir		m_eLoopDir;
	RwUInt32	m_dwCurrTime;	//accumulated time
	
}stTimeTableLOOP, *PstTimeTableLOOP, *LPstTimeTableLOOP;

///////////////////////////////////////////////////////////////////////////////
// * TimeTable
template <class T>
struct	stTimeTable
{
	struct stTimeVal
	{
		RwUInt32	m_dwTime;
		T			m_tVal;

		stTimeVal(RwUInt32 dwTime=0) : m_dwTime(dwTime)					{	};
		stTimeVal(RwUInt32 dwTime, const T& tVal) : m_dwTime(dwTime)	{ m_tVal = tVal;/*RwV3d or RwRGBA..*/};
		stTimeVal(const stTimeVal& cpy) : m_dwTime( cpy.m_dwTime ), m_tVal( cpy.m_tVal ){};
		stTimeVal& operator = ( const stTimeVal& cpy ){
			if( this == &cpy )	return *this;
			m_dwTime	= cpy.m_dwTime;
			m_tVal		= cpy.m_tVal;
			return *this;
		};

		bool operator < ( const stTimeVal& cmp )const	{	return (m_dwTime <  cmp.m_dwTime);	}
		bool operator ==( const stTimeVal& cmp )const	{	return (m_dwTime == cmp.m_dwTime);	}
	};
	typedef std::vector<stTimeVal>					STLVEC_TIMEVAL;
	typedef typename STLVEC_TIMEVAL::iterator		STLVEC_ITR;
	typedef typename STLVEC_TIMEVAL::const_iterator	STLVEC_CITR;

public:
	stTimeTable( RwInt32 nNum = 0 ) : m_stlvecTimeVal( nNum )		{		};

public:
	BOOL				bInsert				( RwUInt32 dwTime, const T& tVal );
	BOOL				bDelete				( RwUInt32 dwTime );

	BOOL				bGetVal				( T* pOut, RwUInt32 dwCurrTime ) const;
	BOOL				bGetValIntp			( T* pOut, RwUInt32 dwCurrTime ) const;
	BOOL				bGetLastTime		( RwUInt32* pdwTime ) const;
	BOOL				bGetLowerBound		( RwUInt32* pDwTime1, T* tval1, RwUInt32* pDwTime2, T* tval2, RwUInt32 dwCurrTime ) const;

	BOOL				bSetVal				( const T& tIn , RwUInt32 dwTime );
	BOOL				bSetVal				( RwInt32 nIndex, const T& tIn , RwUInt32 dwTime );

	BOOL				bSetSize			( RwInt32 nSize );
	
	BOOL				bCpyTable			( PVOID pTable );

	RwInt32				bToFile				( FILE* fp );
	RwInt32				bFromFile			( FILE* fp );

	RwInt32				bForTool_GetNum		( VOID )const	{	return (RwInt32)m_stlvecTimeVal.size();	}
	STLVEC_TIMEVAL&		bForTool_GetTbl		( VOID )		{	return m_stlvecTimeVal;	}

private:
	bool vIsExist( const stTimeVal& chk );

private:
	//do not use
	stTimeTable( const stTimeTable& cpy ) : m_stlvecTimeVal( cpy.m_stlvecTimeVal )	{		};
	stTimeTable& operator= ( const stTimeTable& cpy )
	{
		if( &cpy == this )	return *this;

		m_stlvecTimeVal	= cpy.m_stlvecTimeVal;
		return *this;
	};

public:
	STLVEC_TIMEVAL	m_stlvecTimeVal;
};


template<class T>
bool stTimeTable<T>::vIsExist( const stTimeVal& chk )
{
	return std::binary_search( m_stlvecTimeVal.begin(), m_stlvecTimeVal.end(), chk );
};

template<class T>
BOOL stTimeTable<T>::bInsert( RwUInt32 dwTime, const T& tVal )
{
	stTimeVal	ins( dwTime, tVal );
	if( vIsExist(ins) )		return FALSE;

	STLVEC_ITR	pos	= std::lower_bound( m_stlvecTimeVal.begin(), m_stlvecTimeVal.end(), ins );
	m_stlvecTimeVal.insert( pos, ins );
	
	return TRUE;
}

template<class T>
BOOL stTimeTable<T>::bDelete( RwUInt32 dwTime )
{
	stTimeVal	del( dwTime );

	if( !vIsExist(del) )//없는데..
		return FALSE;

	STLVEC_ITR	find = std::find( m_stlvecTimeVal.begin(), m_stlvecTimeVal.end(), del );

	ASSERT( find != m_stlvecTimeVal.end() && "어라...." );
	if( find == m_stlvecTimeVal.end() )
		return FALSE;

	m_stlvecTimeVal.erase( find );

	return TRUE;
}

template<class T>
BOOL stTimeTable<T>::bGetVal( T* pOut, RwUInt32 dwCurrTime ) const
{
	ASSERT( pOut && "stTimeTable::bGetVal()" );

	if( m_stlvecTimeVal.empty() )	return FALSE;
	
	STLVEC_CITR	it_curr = m_stlvecTimeVal.begin();
	STLVEC_CITR	it_last = m_stlvecTimeVal.end();
	if( (*(it_curr)).m_dwTime < dwCurrTime )
	{
		for( ++it_curr; it_curr != it_last; ++it_curr )
		{
			if((*it_curr).m_dwTime > dwCurrTime)
			{
				--it_curr;
				break;
			}
		}
	}

	if( it_curr == it_last )
		--it_curr;

	*pOut	= (* it_curr ).m_tVal;
	
	return TRUE;
};

//-----------------------------------------------------------------------------
// bGetValIntp
//-----------------------------------------------------------------------------
template<class T>
BOOL stTimeTable<T>::bGetValIntp( T* pOut, RwUInt32 dwCurrTime ) const
{
	ASSERT( pOut && "stTimeTable::bGetVal()" );
	
	if( m_stlvecTimeVal.empty() )
		return FALSE;

	STLVEC_CITR	it_curr = m_stlvecTimeVal.begin();
	STLVEC_CITR	it_last = m_stlvecTimeVal.end();
	for( ; it_curr != it_last; ++it_curr )
	{
		if((*it_curr).m_dwTime > dwCurrTime)
		{
			break;
		}
	}

	if( it_curr == m_stlvecTimeVal.begin() )
		*pOut	= (* it_curr ).m_tVal;
	else if( it_curr == it_last )
		*pOut	= (* (--it_curr) ).m_tVal;
	else
		LinearIntp( pOut, &(*(it_curr-1)).m_tVal, &(*it_curr).m_tVal, (RwReal)( dwCurrTime - (*(it_curr-1)).m_dwTime ) / (RwReal)( (*it_curr).m_dwTime - (*(it_curr-1)).m_dwTime ) );

	return TRUE;
};

template<class T>
BOOL stTimeTable<T>::bGetLowerBound( RwUInt32* pDwTime1, T* tval1, RwUInt32* pDwTime2, T* tval2, RwUInt32 dwCurrTime ) const
{
	ASSERT( tval1 && pDwTime1 && tval2 && pDwTime2 && "stTimeTable::bGetVal()" );
	if( m_stlvecTimeVal.empty()	)	return FALSE;
	
	stTimeVal	timeval(dwCurrTime);

	STLVEC_CITR	it_f = std::lower_bound( m_stlvecTimeVal.begin(), m_stlvecTimeVal.end(), timeval );

	if( it_f == m_stlvecTimeVal.end() )
	{
		//out of range
		--it_f;
		*pDwTime1	= 
		*pDwTime2	= (*it_f).m_dwTime;
		*tval1		= (*it_f).m_tVal;
		return FALSE;
	}
	else if( it_f == m_stlvecTimeVal.begin() )
	{
		*pDwTime1	= 
		*pDwTime2	= (*it_f).m_dwTime;
		*tval1		= (*it_f ).m_tVal;
		return FALSE;
	}
	else
	{
		*pDwTime2	= (*it_f).m_dwTime;
		*tval2		= (*it_f).m_tVal;
		
		--it_f;
		*pDwTime1	= (*(it_f)).m_dwTime;
		*tval1		= (*(it_f)).m_tVal;
	}

	return TRUE;
};

template<class T>
BOOL stTimeTable<T>::bGetLastTime( RwUInt32* pdwTime ) const
{
	if( !m_stlvecTimeVal.empty() )
	{
		*pdwTime = m_stlvecTimeVal.rbegin()->m_dwTime;
		return TRUE;
	}

	return FALSE;
};

template<class T>
BOOL stTimeTable<T>::bSetVal( const T& tVal , RwUInt32 dwTime )
{
	stTimeVal	tmp( dwCurrTime, tVal );
	
	STLVEC_ITR	it_f = std::find( m_stlvecTimeVal.begin(), m_stlvecTimeVal.end(), tmp );
	if( it_f == m_stlvecTimeVal.end() )
		return FALSE;

	(*it_f).m_tVal	= tVal;
	return TRUE;
};

template<class T>
BOOL stTimeTable<T>::bSetVal( RwInt32 nIndex, const T& tVal , RwUInt32 dwTime )
{
	if( T_ISMINUS4( nIndex ) )						return FALSE;
	if( nIndex >= (RwInt32)m_stlvecTimeVal.size() )	return FALSE;

	if( m_stlvecTimeVal[nIndex].m_dwTime == dwTime )
	{
		m_stlvecTimeVal[nIndex].m_tVal	= tVal;
	}
	else
	{
		stTimeVal ins( dwTime, tVal );
		if( vIsExist( ins ) )
			return FALSE;

#ifdef	_EFF_TOOL_
		bDelete( m_stlvecTimeVal[nIndex].m_dwTime );
		bInsert( dwTime, tVal );
#else
		m_stlvecTimeVal[nIndex]	= ins;
#endif
	}

	return TRUE;
};

template<class T>
BOOL stTimeTable<T>::bSetSize( RwInt32 nSize )
{
	if( nSize <= 0 )	return FALSE;

	m_stlvecTimeVal.resize( nSize );
	return TRUE;
};

template<class T>
RwInt32 stTimeTable<T>::bToFile(FILE* fp)
{
	//binary
	ASSERT(fp);
	int nSize = m_stlvecTimeVal.size();
	int ir = fwrite( (LPVOID)(&nSize), sizeof(nSize), 1, fp );
	ASSERT( ir == 1 );
	ir = fwrite( (LPVOID)(&m_stlvecTimeVal[0]), sizeof(stTimeVal), nSize, fp );
	return (ir*sizeof(stTimeVal) + 4);//byte
}

template<class T>
RwInt32 stTimeTable<T>::bFromFile(FILE* fp)
{
	//binary
	ASSERT(fp);
	int nSize;
	int ir = fread( (LPVOID)(&nSize), sizeof(nSize), 1, fp );
	m_stlvecTimeVal.resize(nSize);
	ASSERT( ir == 1 );
	ir = fread( (LPVOID)(&m_stlvecTimeVal[0]), sizeof(stTimeVal), nSize, fp );
	return (ir*sizeof(stTimeVal) + 4);//byte
}

template<class T>
BOOL stTimeTable<T>::bCpyTable( void* pTable )
{
	if( !pTable )		return FALSE;

	memcpy( &m_stlvecTimeVal[0], pTable, sizeof( stTimeVal ) * m_stlvecTimeVal.size() );
	return TRUE;
};

#endif