
/**************************************************************************************************

전체 속성 목록 객체

iterator를 이용하여 순차적으로 속성 객체를 가져올 수 있다.
유니크한 속성 번호로 속성 객체를 가져올 수 있다.

**************************************************************************************************/

#pragma once

#include "ShopProperty.h"

class CShopPropertyList
{
public:
	CShopPropertyList(void);
	~CShopPropertyList(void);

	void Clear();	

	virtual void Append(CShopProperty Property);

	// Interface --------------------------------------------------
	int GetSize();													// 속성 개수

	void SetFirst();												// 목록에서 첫 번째 속성 객체를 가리키게 한다.
	bool GetNext(CShopProperty& Property);							// 현재 카테고리 객체를 넘기고 다음 카테고리 객체를 가리키게 한다.

	bool GetValueByKey(int nPropertySeq, CShopProperty& Property);	// 속성 번호로 속성 객체를 가져온다.
	bool GetValueByIndex(int nIndex, CShopProperty& Property);		// 인덱스 번호로 속성 객체를 가져온다. 
	// ------------------------------------------------------------

protected:	
	std::map<int, CShopProperty> m_Propertys;						// 속성 객체 맵
	std::map<int, CShopProperty>::iterator m_PropertyIter;			// 속성 iterator
	std::vector<int> m_PropertyIndex;								// 속성 번호 인덱스 목록
};
