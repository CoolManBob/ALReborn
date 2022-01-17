// AgpaAI.h: interface for the AgpaAI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPAAI_H__C17D5BCC_1AF8_47D2_9DFE_3EB82551A3BC__INCLUDED_)
#define AFX_AGPAAI_H__C17D5BCC_1AF8_47D2_9DFE_3EB82551A3BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApAdmin.h"
#include "AgpdAI.h"

class AgpaAI : public ApAdmin  
{
public:
	AgpaAI();
	virtual ~AgpaAI();

	AgpdAITemplate *	AddAITemplate(AgpdAITemplate *pstTemplate);
	AgpdAITemplate *	GetAITemplate(INT32 lTID);
	BOOL				RemoveAITemplate(INT32 lTID);
};

#endif // !defined(AFX_AGPAAI_H__C17D5BCC_1AF8_47D2_9DFE_3EB82551A3BC__INCLUDED_)
