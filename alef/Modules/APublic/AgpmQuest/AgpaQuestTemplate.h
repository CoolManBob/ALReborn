// AgpaQuest.h: interface for the AgpaQuest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPAQUEST_H__468D7ACC_A816_4595_A203_79CF7649A78D__INCLUDED_)
#define AFX_AGPAQUEST_H__468D7ACC_A816_4595_A203_79CF7649A78D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApAdmin.h"
#include "AgpdQuestTemplate.h"

class AgpaQuestTemplate : public ApAdmin
{
public:
	AgpaQuestTemplate();
	virtual ~AgpaQuestTemplate();

	AgpdQuestTemplate*	Add(AgpdQuestTemplate *pcsQuest);
	AgpdQuestTemplate*	Get(INT32 lQuestID);
	BOOL		Remove(INT32 lQuestID);
};

#endif // !defined(AFX_AGPAQUEST_H__468D7ACC_A816_4595_A203_79CF7649A78D__INCLUDED_)
