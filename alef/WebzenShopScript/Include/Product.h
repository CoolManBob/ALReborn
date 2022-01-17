#pragma once
#include "script.h"
#include "EventProperty.h"
#include <string>

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	class Product : public EventProperty
	{
	public:
		Product();
		Product( int packageId, int priceId, CShopProduct & scriptData );
		virtual ~Product()										{}

		int														packageId;	// 빌링 시스템에서 이 상품이 속한 패키지 아이디
		int														id;			// 빌링 시스템에서 이 상품을 지칭하는 아이디
		int														itemId;		// 실제 아크로드 아이템과 매칭되는 아이디
		int														price;		// 가격
		int														priceId;	// 가격종류
		bool													isMain;		// 패키지의 메인 아이템인지
		std::string												name;
		std::string												desc;
	};

	//-----------------------------------------------------------------------
}