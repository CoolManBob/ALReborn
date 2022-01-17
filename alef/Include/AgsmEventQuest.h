// AgsmEventQuest.h: interface for the AgsmEventQuest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMEVENTQUEST_H__CCF71688_4639_4164_923F_2219DCE4E7BF__INCLUDED_)
#define AFX_AGSMEVENTQUEST_H__CCF71688_4639_4164_923F_2219DCE4E7BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgpmEventQuest.h"
#include "AgsmCharacter.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgsmEventQuestD.lib")
#else
	#pragma comment(lib, "AgsmEventQuest.lib")
#endif
#endif

class AgsmEventQuest : public AgsModule
{
public:
	AgpmEventQuest* m_pcsAgpmEventQuest;
	AgsmCharacter* m_pcsAgsmCharacter;
	
public:
	AgsmEventQuest();
	virtual ~AgsmEventQuest();

	BOOL OnAddModule();
	static BOOL CBProductEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif // !defined(AFX_AGSMEVENTQUEST_H__CCF71688_4639_4164_923F_2219DCE4E7BF__INCLUDED_)
