// AgcdEffTail.h: interface for the AgcdEffTail class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCDEFFTAIL_H__C5E091D2_F079_4913_96DD_7B7CDA67FAEC__INCLUDED_)
#define AFX_AGCDEFFTAIL_H__C5E091D2_F079_4913_96DD_7B7CDA67FAEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffBase.h"
#include "AgcuEff2ApMemoryLog.h"

//#############################################################################
// class		: AgcdEffTail
// desc			: 
// update		: 20040527 by kday
//				- 최초 작업.
//#############################################################################
class AgcdEffTail : public AgcdEffRenderBase, public ApMemory<AgcdEffTail, 10>
{
	EFFMEMORYLOG_SMV;

#ifdef USE_MFC
public:
#else
private:
#endif//USE_MFC
	RwInt32		m_nMaxNum;			//최대 사각형 갯수..
	RwUInt32	m_dwTimeGap;		//추가 점이 생길 시간.
	RwUInt32	m_dwPointLife;		//추가된 점의 존속시간.
	RwReal		m_fInvsPointLife;	//1.f/m_dwPointLife

	RwReal		m_fHeight1;			//height base
	RwReal		m_fHeight2;			//node base인 경우는 이값이 무시된다.
private:
	//do not use!
	AgcdEffTail( const AgcdEffTail& cpy ) {cpy;};
	AgcdEffTail& operator = ( const AgcdEffTail& cpy ) { cpy; return *this; };
public:
	explicit AgcdEffTail(AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType=AgcdEffRenderBase::EFFBLEND_ADD_ONE_ONE);
	virtual ~AgcdEffTail( VOID );

	//setup
	void			bSetCapacity		( RwInt32 nMaxNum )			{ m_nMaxNum		= nMaxNum; };
	void			bSetTimeGap			( RwUInt32 dwTimeGap )		{ m_dwTimeGap	= dwTimeGap; };
	void			bSetPointLife		( RwUInt32 dwPointLife )	{ m_dwPointLife	= dwPointLife; m_fInvsPointLife = 1.f / static_cast<RwReal>(m_dwPointLife); };
	void			bSetHeight			( RwReal fh1, RwReal fh2 )	{ m_fHeight1 = fh1; m_fHeight2 = fh2; };

	//access
	RwInt32			bGetCapacity		( VOID ) const		{ return m_nMaxNum; };
	RwUInt32		bGetTimeGap			( VOID ) const		{ return m_dwTimeGap; };
	RwUInt32		bGetPointLife		( VOID ) const		{ return m_dwPointLife; };
	RwReal			bGetInvsPointLife	( VOID ) const		{ return m_fInvsPointLife; };
	VOID			bGetHeight			( RwReal* pfh1, RwReal* pfh2)const { *pfh1 = m_fHeight1, *pfh2 = m_fHeight2; };
	
	//file in out
	RwInt32			bToFile				( FILE* fp );
	RwInt32			bFromFile			( FILE* fp );
	
	//for tool
#ifdef USE_MFC
	virtual 
	INT32		bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC
};

#endif