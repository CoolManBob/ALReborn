
/**************************************************************************************************

전체 상품 목록 객체

iterator를 이용하여 순차적으로 상품 객체를 가져올 수 있다.
유니크한 상품 번호로 상품 객체를 가져올 수 있다.

**************************************************************************************************/

#pragma once

#include "ShopProduct.h"

class CShopProductList
{
public:
	CShopProductList(void);
	~CShopProductList(void);

	void Clear();	

	virtual void Append(CShopProduct product);

	// Interface --------------------------------------------------
	int GetSize();													// 상품 개수
																	
	void SetFirst();												// 목록에서 첫 번째 상품 객체를 가리키게 한다.
	bool GetNext(CShopProduct& product);							// 현재 상품 객체를 넘기고 다음 상품 객체를 가리키게 한다.

	bool GetValueByKey(int nProductSeq, CShopProduct& Product);		// 상품 번호로 상품 객체를 가져온다.
	bool GetValueByIndex(int nIndex, CShopProduct& Product);		// 인덱스 번호로 상품 객체를 가져온다. 
	// ------------------------------------------------------------

protected:	
	std::map<int, CShopProduct> m_Products;							// 상품 객체 맵
	std::map<int, CShopProduct>::iterator m_ProductIter;			// 상품 iterator
	std::vector<int> m_ProductIndex;								// 상품 번호 인덱스 목록
};
