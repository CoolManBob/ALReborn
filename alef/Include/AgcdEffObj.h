#if !defined(AFX_AGCDEFFOBJ_H__BB3CA726_6653_4F5D_AD54_E65E8958428C__INCLUDED_)
#define AFX_AGCDEFFOBJ_H__BB3CA726_6653_4F5D_AD54_E65E8958428C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffBase.h"
#include "AgcuEff2ApMemoryLog.h"

class AgcdEffObj : public AgcdEffRenderBase, public ApMemory<AgcdEffObj, 400>
{
	EFFMEMORYLOG_SMV;

public:
	explicit AgcdEffObj(AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType=AgcdEffRenderBase::EFFBLEND_ADD_ONE_ONE);
	virtual ~AgcdEffObj();

	RpClump*		bLoadClump			( VOID );
	RpClump*		vLoadClump			( const RwChar* szClump );

	const RwChar*	bGetClumpName		( VOID ) const				{ return m_szClumpFName;	};
	const RpClump*	bGetPtrClump		( VOID ) const				{ return m_pClump;			};
	const RwRGBA	bGetPreLit			( VOID ) const				{ return m_rgbaPreLit;		};
	RwChar*			bGetClumpName		( VOID )					{ return m_szClumpFName;	};
	RpClump*		bGetPtrClump		( VOID )					{ return m_pClump;			};
	
	void			bSetPreLit			( RwRGBA rgbaPreLit )		{ m_rgbaPreLit = rgbaPreLit; };
	RwInt32			bSetClumpName		( const RwChar* szClump );
	RwInt32			bFindNodeIndex		( RwInt32 nodeID );
	
	RwInt32			bToFile				( FILE* fp );
	RwInt32			bFromFile			( FILE* fp );

#ifdef USE_MFC
	virtual 
	INT32			bForTool_Clone		( AgcdEffBase* pEffBase );
#endif//USE_MFC

private:
	//do not use!
	AgcdEffObj( const AgcdEffObj& cpy ) : AgcdEffRenderBase(cpy) {cpy;}
	AgcdEffObj& operator = ( const AgcdEffObj& cpy ) { cpy; return *this; }

public:
	RwChar		m_szClumpFName[EFF2_FILE_NAME_MAX];
	RpClump*	m_pClump;
	RwRGBA		m_rgbaPreLit;
};

#endif