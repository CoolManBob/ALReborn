#if !defined(AFX_AGCDEFFBOARD_H__B77291AC_0149_42FF_9985_E22717906A9E__INCLUDED_)
#define AFX_AGCDEFFBOARD_H__B77291AC_0149_42FF_9985_E22717906A9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "AgcdEffBase.h"
#include "ApMemory.h"
#include "AgcuEff2ApMemoryLog.h"

class AgcdEffBoard  
	:	public AgcdEffRenderBase	, 
		public ApMemory<AgcdEffBoard, 400>
{
	EFFMEMORYLOG_SMV;

public:
	explicit AgcdEffBoard( AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType=AgcdEffRenderBase::EFFBLEND_ADD_ONE_ONE );
	virtual ~AgcdEffBoard();
	
	//file in out
	RwInt32			bToFile		( FILE* fp );
	RwInt32			bFromFile	( FILE* fp );
	
#ifdef USE_MFC
	virtual 
	INT32			bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC

private:
	AgcdEffBoard( const AgcdEffBoard& cpy ) : AgcdEffRenderBase(cpy) {cpy;}
	AgcdEffBoard& operator = ( const AgcdEffBoard& cpy ) { cpy; return *this; }
};

#endif