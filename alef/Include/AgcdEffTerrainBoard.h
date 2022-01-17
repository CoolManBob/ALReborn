#if !defined(AFX_AGCDEFFTERRAINBOARD_H__414F1E80_6616_4FA4_A1E4_0B1C94B94A31__INCLUDED_)
#define AFX_AGCDEFFTERRAINBOARD_H__414F1E80_6616_4FA4_A1E4_0B1C94B94A31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffBase.h"
#include "AgcuEff2ApMemoryLog.h"

class AgcdEffTerrainBoard : public AgcdEffRenderBase, public ApMemory<AgcdEffTerrainBoard, 100>
{
	EFFMEMORYLOG_SMV;

public:
	AgcdEffTerrainBoard( AgcdEffRenderBase::E_EFFBLENDTYPE eBlendType=AgcdEffRenderBase::EFFBLEND_ADD_ONE_ONE);
	virtual ~AgcdEffTerrainBoard();
	
	//file in out
	RwInt32		bToFile				( FILE* fp );
	RwInt32		bFromFile			( FILE* fp );
	
	//for tool
#ifdef USE_MFC
	virtual 
	INT32		bForTool_Clone		( AgcdEffBase* pEffBase );
#endif//USE_MFC
};

#endif