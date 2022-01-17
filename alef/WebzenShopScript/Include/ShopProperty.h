
/**************************************************************************************************

속성 객체

하나의 속성 정보를 가지고 있다.

**************************************************************************************************/

#pragma once

#include "include.h"
#include <map>

class CShopProperty
{
public:
	CShopProperty();
	virtual ~CShopProperty();

	bool SetProperty(tstring strdata);

public:	
	int		PropertySeq;										//  1. 속성 번호
	TCHAR	PropertyName[SHOPLIST_LENGTH_PROPERTYNAME];			//  2. 속성 명
	int		PropertyType;										//  3. 속성 유형 (141:아이템 속성, 142:가격 속성)
	int		MustFlag;											//  4. 필수 여부 (145:필수, 146:선택)
	int		UnitType;											//  5. 단위 코드
	TCHAR	UnitName[SHOPLIST_LENGTH_PROPERTYUNITNAME];			//  6. 속성 단위 명
};
