// AgcuEffTable.h: interface for the AgcuEffTable class.
//
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCUEFFTABLE_H__756B60C7_CDE5_4B6C_81D0_6A2AA196E85C__INCLUDED_)
#define AFX_AGCUEFFTABLE_H__756B60C7_CDE5_4B6C_81D0_6A2AA196E85C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "rwcore.h"
#include "ApDefine.h"	//for GetTickCount, srand, rand, sin
#include "MagDebug.h"	//for ASSERT
#include "d3dx9math.h"

#define		INVS_RAND_MAX		0.000030518509476f		// 1.f/RAND_MAX

template< class T >
struct Eff2Ut_StRandTable
{
public:
	Eff2Ut_StRandTable(unsigned int dwNumMinusOne=0x3ff/*0x3ff=1023*/, float fScale=1.f)
	: m_pVal(NULL)
	, m_dwNumMinusOne(dwNumMinusOne)
	, m_dwNum(dwNumMinusOne+1)
	, m_fScale(fScale)
	, m_fInvsScale(1.f/fScale)
	, m_nCnt(0)
	{
		//varify
		ASSERT( vAvailable() && "@ tagEff2Ut_StRandTable::tagEff2Ut_StRandTable()" );
		if( vAvailable() ){
			m_pVal	= new T[m_dwNum];
			ASSERT( m_pVal && "@ tagEff2Ut_StRandTable::tagEff2Ut_StRandTable()" );
		}

			if( m_pVal ){
				srand(GetTickCount());
				for( unsigned int i=0; i<m_dwNum; ++i )
					m_pVal[i]	= (T) ( (float)( rand() ) * ( fScale * INVS_RAND_MAX ) );
			}
	};
	~Eff2Ut_StRandTable()
	{
		delete [] m_pVal;
	};

	T bGetTableVal(unsigned int dwTime)					{	return m_pVal[ (dwTime + (++m_nCnt)) & m_dwNumMinusOne ];	};
	T bGetTableVal( unsigned int dwTime, float fScale )	{	return ( m_pVal[ (dwTime + (++m_nCnt)) & m_dwNumMinusOne ] * fScale * m_fInvsScale );	}
	T bGetTableVal(void)								{	return m_pVal[ (++m_nCnt) & m_dwNumMinusOne ];	}
	T bGetTableVal(float fScale)						{	return ( m_pVal[ (++m_nCnt) & m_dwNumMinusOne ] * fScale * m_fInvsScale );	}

private:
	bool	vAvailable()
	{
		unsigned int dwMask	= 0xffffffff;
		for(int i=0; i<31; ++i)
		{
			if( m_dwNumMinusOne == (dwMask >> i) )
				return true;
		}

		return false;
	};

private:
	Eff2Ut_StRandTable( const Eff2Ut_StRandTable& cpy ){};				//copy constructor 금지..
	Eff2Ut_StRandTable& operator = (const Eff2Ut_StRandTable& cpy)	{};	//대입 연산 금지..

private:
	T*					m_pVal;
	const unsigned int	m_dwNumMinusOne;
	const unsigned int	m_dwNum;
	const float			m_fScale;
	const float			m_fInvsScale;

	int					m_nCnt;
};

///////////////////////////////////////////////////////////////////////////////
// 전역변수 extern 선언.
extern Eff2Ut_StRandTable<unsigned char>	g_RndTblBYTE;
extern Eff2Ut_StRandTable<float>			g_RndTblFloat_1;

///////////////////////////////////////////////////////////////////////////////
// 외부에서 사용할 매크로
#define Eff2Ut_GETRNDRGB( dwTime )			( g_RndTblBYTE.bGetTableVal( dwTime ) )
#define Eff2Ut_GETRNDFLOAT1( dwTime )		( g_RndTblFloat_1.bGetTableVal( dwTime ) )
#define	Eff2Ut_GETRNDFLOAT Eff2Ut_GETRNDFLOAT1

#endif