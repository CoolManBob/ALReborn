#ifndef __CLASS_WEBZEN_SHOP_LIST_H__
#define __CLASS_WEBZEN_SHOP_LIST_H__

#include <Winsock2.h>
#include "Windows.h"
#include "Script.h"
#include "Shop.h"
#include "ProductDic.h"

/*

struct stWebzenShopProduct
{
	int															m_nCategoryID;
	int															m_nPackageSeq;

	int															m_nPID;
	int															m_nTID;

	std::string													m_strProductName;
	std::string													m_strProductDesc;
	int															m_nPrice;

	BOOL														m_bIsEvent;
	BOOL														m_bIsHot;
	BOOL														m_bIsNew;

	int															m_nPriceSeq;

	stWebzenShopProduct( void )
	{
		m_nCategoryID = 0;
		m_nPackageSeq = 0;

		m_nPID = 0;
		m_nTID = 0;

		m_strProductName = "";
		m_strProductDesc = "";
		m_nPrice = 0;

		m_bIsEvent = FALSE;
		m_bIsHot = FALSE;
		m_bIsNew = FALSE;

		m_nPriceSeq = 0;
	}
};

struct stWebzenShopCategory
{
	int															m_nCategoryID;
	std::string													m_strCategoryName;
	
	ContainerMap< int, stWebzenShopProduct >					m_mapProduct;

	stWebzenShopCategory( void )
	{
		m_nCategoryID = 0;
		m_strCategoryName = "";
	}
};

class CWebzenShopList : public CShopListManager, public CWebzenUtility
{
private :
	DownloaderType												m_eDownLoadType;
	CListVersionInfo											m_VersionInfo;

	CShopList*													m_pShopList;
	ContainerMap< int, stWebzenShopCategory >					m_mapCategory;

	BOOL														m_bIsPCBang;

public :
	CWebzenShopList( void );
	virtual ~CWebzenShopList( void );

public :
	BOOL						OnShopUIInitialize				( void );

	BOOL						OnShopListInitialize			( void );
	BOOL						OnShopListLoadFromFile			( void );

public :
	void						SetShopListVersionInfo			( unsigned short nYear, unsigned short nYearID, BOOL bIsPCBang );

public :
	int							GetCategoryCount				( void );
	char*						GetCategoryName					( int nCategoryIndex );

	int							GetPackageSequence				( int nCategoryIndex, int nProductIndex );
	int							GetPackageSequence				( int nPID );

	int							GetProductCount					( int nCategoryIndex );
	int							GetProductPID					( int nCategoryIndex, int nProductIndex );
	int							GetCategoryID					( int nCategoryIndex, int nProductIndex );

	int							GetCategoryIndex				( int nPID );
	int							GetProductIndex					( int nCategoryIndex, int nPID );

	int							GetProductTID					( int nCategoryIndex, int nProductIndex );
	int							GetProductTID					( int nPID );

	char*						GetProductName					( int nCategoryIndex, int nProductIndex );
	char*						GetProductName					( int nPID );

	char*						GetProductDesc					( int nCategoryIndex, int nProductIndex );
	char*						GetProductDesc					( int nPID );

	int							GetProductPrice					( int nCategoryIndex, int nProductIndex );
	int							GetProductPrice					( int nPID );

	int							GetProductPriceSequence			( int nCategoryIndex, int nProductIndex );
	int							GetProductPriceSequence			( int nPID );

	BOOL						GetProductIsEvent				( int nCategoryIndex, int nProductIndex );
	BOOL						GetProductIsEvent				( int nPID );

	BOOL						GetProductIsHot					( int nCategoryIndex, int nProductIndex );
	BOOL						GetProductIsHot					( int nPID );

	BOOL						GetProductIsNew					( int nCategoryIndex, int nProductIndex );
	BOOL						GetProductIsNew					( int nPID );

	BOOL						IsPCBang						( void ) { return m_bIsPCBang; }

	bool						isEnableCategory				( CShopCategory& pCategory );
	bool						isPCBangCategory				( stWebzenShopCategory& category );
	bool						isPCBangCategory				( int categoryIdx );

private :
	BOOL						_ParseShopData					( void );
	BOOL						_ParseShopCategory				( CShopCategory* pCategory );
	BOOL						_ParseShopPackage				( CShopCategory* pCategory, CShopPackage* pPackage );

	BOOL						_DeleteShopDataFile				( void );

	BOOL						_AddCategory					( int nCategoryID, char* pCategoryName );
	BOOL						_AddProduct						( int nCategoryID, int nPackageSeq, int nPID, int nTID, char* pProductName, char* pProductDesc, int nPrice, int nPriceSeq, BOOL bIsEvent = FALSE, BOOL bIsHot = FALSE, BOOL bIsNew = FALSE );

	stWebzenShopCategory*		_GetCategory					( int nCategoryID );
	stWebzenShopCategory*		_GetCategoryByIndex				( int nCategoryIndex );
	stWebzenShopCategory*		_GetCategory					( char* pCategoryName );

	stWebzenShopProduct*		_GetCategoryProdduct			( int nCategoryID, int nPID );
	stWebzenShopProduct*		_GetCategoryProdductByIndex		( int nCategoryIndex, int nProductIndex );
	stWebzenShopProduct*		_GetCategoryProdduct			( char* pProductName );
	stWebzenShopProduct*		_GetCategoryProdduct			( int nPID );
};
*/

#endif