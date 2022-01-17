
/**************************************************************************************************

스크립트 목록 최 상위 객체

카테고리 목록, 패키지 목록, 상품 목록, 속성 목록, 상품 링크 목록을 가지고 있다.

**************************************************************************************************/

#pragma once

#include "ShopPackage.h"
#include "ShopProduct.h"

#include "ShopCategoryList.h"
#include "ShopPackageList.h"
#include "ShopProductLinkList.h"
#include "ShopProductList.h"
#include "ShopPropertyList.h"

class CShopList  
{
public:
	CShopList();
	virtual ~CShopList();

	WZResult LoadPackageLink(const TCHAR* szFilePath);
	WZResult LoadProductLink(const TCHAR* szFilePath);
	WZResult LoadCategroy(const TCHAR* szFilePath);
	WZResult LoadPackage (const TCHAR* szFilePath);
	WZResult LoadProduct (const TCHAR* szFilePath);	
	WZResult LoadProperty(const TCHAR* szFilePath);	

	// Interface --------------------------------------------------
	CShopCategoryList*		GetCategoryListPtr()	{return m_CategoryListPtr;};	// 카테고리 목록 가져온다.
	CShopPackageList*		GetPackageListPtr()		{return m_PackageListPtr;};		// 패키지 목록 가져온다.
	CShopProductList*		GetProductListPtr()		{return m_ProductListPtr;};		// 상품 목록 가져온다.
	CShopPropertyList*		GetPropertyListPtr()	{return m_PropertyListPtr;};	// 속성 목록 가져온다.
	CShopProductLinkList*	GetProductLinkListPtr() {return m_ProductListLinkPtr;};	// 상품 링크 목록 가져온다.
	// ------------------------------------------------------------

private:	
	CShopCategoryList*		m_CategoryListPtr;
	CShopPackageList*		m_PackageListPtr;
	CShopProductList*		m_ProductListPtr;
	CShopPropertyList*		m_PropertyListPtr;
	CShopProductLinkList*	m_ProductListLinkPtr;
};
