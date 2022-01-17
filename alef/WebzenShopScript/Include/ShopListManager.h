
/**************************************************************************************************

샵리스트 관리를 위해 사용되는 Main 객체

서버로 부터 스크립트 파일을 다운로드 하고, 
각 파일을 읽어 객체와 목록을 구성한다.

**************************************************************************************************/

#pragma once

#include "include.h"
#include "ListManager.h"
#include "ShopList.h"

class CShopListManager : public CListManager
{
public:
	CShopListManager();	
	virtual ~CShopListManager();

	CShopList*		GetListPtr() {return m_ShopList;};

private:
	CShopList*		m_ShopList;

	WZResult		LoadScript(bool bDonwLoad);
};