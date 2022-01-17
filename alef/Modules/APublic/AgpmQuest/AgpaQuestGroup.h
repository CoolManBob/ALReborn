// AgpaQuestGroup.h: interface for the AgpaQuestGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPAQUESTGROUP_H__57435F0D_C3AD_4F66_B0F7_808E937E5FEF__INCLUDED_)
#define AFX_AGPAQUESTGROUP_H__57435F0D_C3AD_4F66_B0F7_808E937E5FEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApAdmin.h"
#include "AgpdQuestTemplate.h"

class AgpaQuestGroup : public ApAdmin
{
public:
	AgpaQuestGroup();
	virtual ~AgpaQuestGroup();

	AgpdQuestGroup*	Add(AgpdQuestGroup *pcsQuest);
	AgpdQuestGroup*	Get(INT32 lQuestGroupID);
	BOOL		Remove(INT32 lQuestGroupID);

	BOOL IsExistEstimate(INT32 lQuestGroupID, INT32 lQuestTID);
	BOOL IsExistGrant(INT32 lQuestGroupID, INT32 lQuestTID);
};

#endif // !defined(AFX_AGPAQUESTGROUP_H__57435F0D_C3AD_4F66_B0F7_808E937E5FEF__INCLUDED_)
