
/**************************************************************************************************

상품 링크 객체

하나의 상품 링크 정보를 가지고 있다.
상품에 대한 하나의 속성과 그 속성의 Value와 가격을 링크 한 객체 이다.

상품 번호가 동일한 여러 링크 객체를 조합하여 한 가지 상품을 표현해야 한다.

**************************************************************************************************/

#pragma once

#include "include.h"

class CShopProductLink
{
public:
	CShopProductLink();
	virtual ~CShopProductLink();

	bool SetProductLink(tstring strData);

	int		ProductSeq;								// 1. 상품 순번
	TCHAR	Value[SHOPLIST_LENGTH_PROPERTYVALUE];	// 2. 속성 값
	int 	Price;									// 3. 단위 상품 가격
	int 	PriceSeq;								// 4. 단위 상품 가격 순번
	int 	vOrder;									// 5. 메인 속성 구분 (1: 메인, 9: 서브)
	int 	InGameProductID;						// 6. 아이템 코드 (없는경우 0)
	int 	PropertySeq;							// 7. 속성 코드
	int		PropertyType;							// 8. 속성 유형 (141 : 아이템 속성, 142 : 가격 속성)
};
