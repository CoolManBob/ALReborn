// XTPGI_Board.cpp: implementation of the CXTPGI_Board class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_Board.h"

#include "AgcdEffBoard.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CXTPGI_Board::CXTPGI_Board(CString strCaption, AgcdEffBoard* pBoard)
 : CXTPGI_EffRenderBase(strCaption, pBoard)
 ,m_pBoard(pBoard)
{
}