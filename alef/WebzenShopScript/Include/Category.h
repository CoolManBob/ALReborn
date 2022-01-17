#pragma once
#include "Script.h"
#include "Package.h"
#include <string>
#include <map>

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	class Category
	{
	public:
		int														id;			// 이 카테고리의 아이디
		std::string												name;		// 이름

		bool													isOpen;
		bool													isEvent;
		int														order;		// 이 카테고리의 노출 순서


		typedef std::vector< Package >							Packages;
		Packages												packages;	// 이 카테고리에 속한 패키지 목록

		bool IsPCRoomCategory() const;

		Category();
		Category( CShopCategory & scriptData );
		bool operator<( Category const & other ) const;
	};

	//-----------------------------------------------------------------------
}