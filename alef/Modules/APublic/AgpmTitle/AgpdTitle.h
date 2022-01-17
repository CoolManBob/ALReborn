#if !defined(__AGPD_TITLE_H__)
#define __AGPD_TITLE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"

#include <vector>

const INT32 AGPDTITLE_TITLE_NUMBER_MAX	=	500;

using namespace std;

class AgpdHasTitle
{
public:
	INT32 lTitleID;
	BOOL bUsingTitle;
	UINT32 lSetTime;

	AgpdHasTitle() : lTitleID(0), lSetTime(0), bUsingTitle(FALSE)
	{
	}
};

//typedef ApVector<AgpdHasTitle, AGPDTITLE_TITLE_NUMBER_MAX> VectorTitle;
typedef vector<AgpdHasTitle> VectorTitle;
typedef VectorTitle::iterator IterTitle;

class AgpdTitle
{
public:
	BOOL			m_bLoadedTitle;
	VectorTitle		m_vtTitle;

public:
	AgpdTitle();

	BOOL IsFullTitle();

	BOOL AddTitle(INT32 lTitleID);

	BOOL RemoveTitle(INT32 lTitleID);

	BOOL IsHaveTitle(INT32 lTitleID);

	BOOL IsUsingTitle(INT32 lTitleID);

	BOOL UpdateSetTime(INT32 lTitleID, UINT32 lSetTime);

	BOOL UpdateUsingTitle(INT32 lTitleID, BOOL bUsingTitle);

	UINT32 GetSetTime(INT32 lTitleID);

	vector<AgpdHasTitle>::iterator GetUsingTitle();

	INT32 GetUseTitle();

	vector<AgpdHasTitle>::iterator FindByTitleID(INT32 lTitleID);

	UINT32 GetLastSetTime();

	INT32 GetHaveTitleNumber();
};

#endif //__AGPD_TITLE_H__