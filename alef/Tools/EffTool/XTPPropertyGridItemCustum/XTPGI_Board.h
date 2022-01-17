// XTPGI_Board.h: interface for the CXTPGI_Board class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_BOARD_H__154BABD1_74DD_4784_8A20_C47EA70BD140__INCLUDED_)
#define AFX_XTPGI_BOARD_H__154BABD1_74DD_4784_8A20_C47EA70BD140__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_EffBase.h"

class AgcdEffBoard;
class CXTPGI_Board : public CXTPGI_EffRenderBase  
{
public:
	CXTPGI_Board(CString strCaption, AgcdEffBoard* pBoard);
	virtual ~CXTPGI_Board()	{	}

private:
	AgcdEffBoard* m_pBoard;	
};

#endif // !defined(AFX_XTPGI_BOARD_H__154BABD1_74DD_4784_8A20_C47EA70BD140__INCLUDED_)
