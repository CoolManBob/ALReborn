#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_TerrainBoard.h"

#include "AgcdEffTerrainBoard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CXTPGI_TerrainBoard::CXTPGI_TerrainBoard(CString strCaption, AgcdEffTerrainBoard* pTerrainBoard)
: CXTPGI_EffRenderBase(strCaption, pTerrainBoard)
 ,m_pTerrainBoard(pTerrainBoard)
{
}