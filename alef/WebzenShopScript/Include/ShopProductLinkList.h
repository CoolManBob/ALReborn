
/**************************************************************************************************

전체 상품 링크 목록 객체

iterator를 이용하여 순차적으로 상품 링크 객체를 가져올 수 있다.
상품 번호와 가격 번호의 조합으로 상품 링크 객체를 가져올 수 있다.

(패키지에 설정된 가격 번호가 한 개인 경우 가격 번호 0으로 해도 된다.)
(패키지에 설정된 가격 번호가 여러 개인 경우 가격 번호를 정확히 기입해야 한다.)

상품 번호와 가격 번호에 해당하는 링크 객체는 여러 개 이다.
여러 링크 객체를 이용하여 하나의 상품을 표현 해야 한다.
상품 링크 객체에는 상품의 속성과 그 속성에 해당하는 Value 값을 링크 해 놓은 객체 이다.


(ex) A 패키지에 속한 상품번호 100번 가격 번호 10번인 상품을 표현 하는 경우.

1. ProductList 에서 100번 Product 가져오기 : 이름, 기본 상품 정보 표현 
2. ProductLinkList 에서 상품번호 100번 가격 번호 10번인 링크 들을 읽는다.
   5번 속성은 값이 123 이다 -> 5번 속성에 대한 정보는 속성 목록에서 읽어온다.
   6번 속성은 값이 234 이다 -> 6번 속성에 대한 정보는 속성 목록에서 읽어온다.

**************************************************************************************************/

#pragma once

#include "ShopProductLink.h"
#include <map>

class CShopProductLinkList
{
public:
	CShopProductLinkList(void);
	~CShopProductLinkList(void);

	void Clear();	

	virtual void Append(CShopProductLink link);

	// Interface --------------------------------------------------
	int GetSize();													// 상품 링크 개수
																	
	void SetFirst();												// 목록에서 첫 번째 상품 링크 객체를 가리키게 한다.
	bool GetNext(CShopProductLink& link);							// 현재 상품 링크 객체를 넘기고 다음 상품 링크 객체를 가리키게 한다.

	void SetProductFirst(int nProductSeq, int nPriceSeq = 0);		// 상품 번호와 가격 번호에 해당하는 첫 번째 링크 객체를 가리키게 한다.
	bool GetProductNext(CShopProductLink& link);					// 현재 상품 링크 객체를 넘기고 다음 상품 링크 객체를 가리키게 한다.
	// ------------------------------------------------------------

protected:
	int PriceSeqKey;
	std::multimap<int, CShopProductLink> m_ProductLinks;	
	std::multimap<int, CShopProductLink>::iterator m_AllProductLinkIter;
	std::multimap<int, CShopProductLink>::iterator m_CurrentProductIter;
	std::pair<std::multimap<int, CShopProductLink>::iterator, std::multimap<int, CShopProductLink>::iterator> m_ProductRange;
};
