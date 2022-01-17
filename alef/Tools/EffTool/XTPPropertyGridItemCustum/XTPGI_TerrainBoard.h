#ifndef	_H_XTPGI_TERRAINBOARD_20050103
#define _H_XTPGI_TERRAINBOARD_20050103

#include "XTPGI_EffBase.h"

class AgcdEffTerrainBoard;
class CXTPGI_TerrainBoard : public CXTPGI_EffRenderBase  
{
public:
	CXTPGI_TerrainBoard(CString strCaption, AgcdEffTerrainBoard* pTerrainBoard) 
	: CXTPGI_EffRenderBase(strCaption, (AgcdEffRenderBase*)pTerrainBoard)
	 ,m_pTerrainBoard(pTerrainBoard)
	{
	}
	virtual	~CXTPGI_TerrainBoard()	{	}

private:
	AgcdEffTerrainBoard*	m_pTerrainBoard;
};

#endif // _H_XTPGI_TERRAINBOARD_20050103
