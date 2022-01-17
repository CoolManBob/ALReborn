
/**************************************************************************************************

 전체 카테고리 목록 객체

iterator를 이용하여 순차적으로 카테고리 객체를 가져올 수 있다.
유니크한 카테고리 번호로 카테고리 객체를 가져올 수 있다.

**************************************************************************************************/

#pragma once

#include "ShopCategory.h"
#include "ShopPackageLink.h"
#include <map>

class CShopCategoryList
{
public:
	CShopCategoryList(void);
	~CShopCategoryList(void);

	void ClearCategory();
	void ClearPackageLink();
	void AppendPackageLink(CShopPackageLink& link);
	void AppendCategory(CShopCategory& category);
	void SortPackageLinkList();

	// Interface --------------------------------------------------
	int  GetSize();													// 카테고리 개수

	void SetFirst();												// 목록에서 첫 번째 카테고리 객체를 가리키게 한다.
	bool GetNext(CShopCategory& category);							// 현재 카테고리 객체를 넘기고 다음 카테고리 객체를 가리키게 한다.

	bool GetValueByKey(int nCategorySeq, CShopCategory& category);	// 카테고리 번호로 카테고리 객체를 가져온다.
	bool GetValueByIndex(int nIndex, CShopCategory& category);		// 인덱스 번호로 카테고리 객체를 가져온다. 
	// ------------------------------------------------------------

protected:
	std::map<int, CShopCategory> m_Categorys;						// 카테고리 객체 맵
	std::map<int, CShopCategory>::iterator m_CategoryIter;			// 카테고리 iterator
	std::vector<int> m_CategoryIndex;								// 카테고리 번호 인덱스 목록

	std::vector<CShopPackageLink> m_PackageLinks;					// 패키지 링크 목록
};
