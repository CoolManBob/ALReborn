#pragma once
#include "Script.h"
#include "EventProperty.h"
#include "Product.h"
#include <string>
#include <map>

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	class Package : public EventProperty
	{
	public:
		Package();
		Package( int categoryId, int order, CShopPackage & scriptData );
		virtual ~Package()										{}

		int														categoryId; // 빌링 시스템에서 이 패키지가 속한 카테고리 아이디
		int														id;			// 이 패키지의 아이디		
		int														price;		// 가격
		int														priceId;	// 가격 종류

		int														mainProductId; // 이 패키지를 대표하는 상품 아이디
		int														mainItemId; // 이 패키지를 대표하는 상품의 아이템 아이디
		int														order;		// 노출 순서

		std::string												name;		// 이름
		std::string												desc;		// 설명


		typedef std::vector< Product >							Products;
		Products												products;	// 이 패키지에 속한 프로덕트 목록

	private:
		void													setmain( int packageTID );
	};

	//-----------------------------------------------------------------------
}