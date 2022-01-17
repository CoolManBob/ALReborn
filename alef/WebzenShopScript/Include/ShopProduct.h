
/**************************************************************************************************

상품 객체

하나의 상품 정보를 가지고 있다.

**************************************************************************************************/

#pragma once

#include "include.h"
#include <map>

class CShopProduct
{
public:
	CShopProduct();
	virtual ~CShopProduct();

	bool SetProduct(tstring strdata);

public:	
	int		ProductSeq;											//  1. 상품 번호
	TCHAR	ProductName[SHOPLIST_LENGTH_PRODUCTNAME];			//  2. 상품 명
	int		ProductType;										//  3. 상품 유형 코드
	int		DeleteFlag;											//  4. 삭제 여부 (143: 삭제, 144: 활성)
	int		StorageGroup;										//  5. 보관함 그룹 유형
	int		ShareFlag;											//  6. ServerType(서버 유형) 별 보관함 노출 공유 항목 여부
	TCHAR	Description[SHOPLIST_LENGTH_PRODUCTDESCRIPTION];	//  7. 상품 설명
};
