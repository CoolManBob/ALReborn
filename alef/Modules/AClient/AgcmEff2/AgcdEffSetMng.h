// AgcdEffSetMng.h: interface for the AgcdEffSetMng class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCDEFFSETMNG_H__289CEAF1_C449_4181_8F74_6AFAAFE6BE4E__INCLUDED_)
#define AFX_AGCDEFFSETMNG_H__289CEAF1_C449_4181_8F74_6AFAAFE6BE4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffSet.h"
#include "rwcore.h"
#include "rwplcore.h"
#include "AcDefine.h"
#include <list>
#include <map>
using namespace std;

class AgcdEffSetMng
{	
	//typedef std::map< RwUInt32 , LPEFFSET >		LPEffectSetMap;
	//typedef LPEffectSetMap::iterator			LPEffectSetMapItr;
	//typedef LPEffectSetMap::const_iterator		LPEffectSetMapCItr;

	typedef std::list<LPEFFSET>					LPEffectList;

public:
	AgcdEffSetMng();
	virtual ~AgcdEffSetMng();
	
public:
	LPEFFSET				FindEffSet				( RwUInt32 dwID );
	RwInt32					LoadAll					( VOID );

	RwInt32					MakeEffFile				( LPCSTR packingpath=NULL, AcCallbackData1 pfOutputCallback=NULL );
	RwInt32					LoadEffFile				( VOID );

	VOID					PushToEffSetPool		( LPEFFSET lpEffSet );
	VOID					PopFromEffSetPool		( LPEFFSET lpEffSet );

	RwInt32					ShowState				( RwInt32 nCtrlCnt );

	const LPEffectSetMap&	ForTool_GetEffSetMap	( VOID )							{ 	return m_mapEffectSet; 	}
	RwInt32					ForTool_Ins				( LPEFFSET lpNewEffSet )			{	return _InsertEffSet(lpNewEffSet);	};
	RwInt32					ForTool_Del				( RwUInt32 dwID )					{	return _DeleteEffSet( dwID );		};

private:
	void					_ClearEffSet			( VOID );
	RwInt32					_InsertEffSet			( LPEFFSET lpNewEffSet );
	RwInt32					_DeleteEffSet			( RwUInt32 dwID );
	RwInt32					_DeleteEffSet			( LPEFFSET lpNewEffSet );

	static RwInt32			_LoadEffSet				( const RwChar* szFName, void* pAgcdEffSetMng );

private:
	LPEffectSetMap			m_mapEffectSet;

	LPEffectList			m_PoolOfEffSet;
	const	RwInt32			m_POOLNUM_EFFSET;
};

#endif