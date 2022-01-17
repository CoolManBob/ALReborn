#ifndef __AGPMAUCTIONCATEGORY_H__
#define __AGPMAUCTIONCATEGORY_H__

#include "ApBase.h"
#include "ApAdmin.h"
#include "AuExcelTxtLib.h"
#include "ApModule.h"
#include "AgpmItem.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmAuctionCategoryD" )
#else
#pragma comment ( lib , "AgpmAuctionCategory" )
#endif
#endif
//@} Jaewon

#define			AGPA_AUCTION_CATEGORYCOUNT			10000

class AgpdAuctionCategory1Info
{
public:
	INT32				m_lCategoryID;
	char				m_strName[80];
	INT32				m_ChildCount;
	INT32				*m_plChildID;

	AgpdAuctionCategory1Info()
	{
		m_lCategoryID = 0;
		m_ChildCount = 0;
		m_plChildID = NULL;

		memset( m_strName, 0, sizeof(m_strName) );
	}
	~AgpdAuctionCategory1Info()
	{
		if( m_plChildID != NULL )
		{
			delete [] m_plChildID;
		}
	}
};

class AgpdAuctionCategory2Info
{
public:
	INT32				m_lParentCategoryID;
	INT32				m_lCategoryID;
	char				m_strName[80];
	INT32				m_ChildCount;
	INT32				*m_plChildID;

	AgpdAuctionCategory2Info()
	{
		m_lParentCategoryID = 0;
		m_lCategoryID = 0;
		memset( m_strName, 0, sizeof(m_strName) );
		m_ChildCount = 0;
		m_plChildID = NULL;
	}
	~AgpdAuctionCategory2Info()
	{
		if( m_plChildID != NULL )
		{
			delete [] m_plChildID;
		}
	}
};

class AgpmAuctionCategory : public ApModule 
{
	ApAdmin					m_cAdminCategory1;
	ApAdmin					m_cAdminCategory2;
	
	AgpmItem				*m_pcsAgpmItem;

	INT32					m_lCurrentCategory1;
	INT32					m_lCurrentCategory2;

public:
	AgpmAuctionCategory();
	~AgpmAuctionCategory();

	BOOL				OnAddModule();

	ApAdmin	*GetCategory1();
	ApAdmin	*GetCategory2();

	bool LoadCategoryInfo( char *pstrFileName, bool bEncrypt );

	bool BuildCategoryTree();
};

#endif