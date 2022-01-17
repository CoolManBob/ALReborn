#pragma once
#include "Script.h"
#include "Category.h"
#include <string>
#include <map>

namespace Webzen
{
	//-----------------------------------------------------------------------
	// 한 카테고리는 여러개의 패키지를 리스트로 보유
	// 한 패키지는 여러개의 프로덕트를 리스트로 보유
	// 한프로덕트는 한개의 아크로드 아이템과 매칭

	class Shop
	{
	public:
		typedef std::map< int, Category >						Categorys;

		static Shop &											Instance();
		static void												SetShopVersion( unsigned short year, unsigned short yearId );

		static Categorys const &								GetCategorys() { return Instance().categorys; }
		static Category const *									GetCategory( int categoryId );

	private:
		bool													ParseShopData();

		Categorys												categorys;	// 카테고리 목록


		// 복사방지
		Shop() {}
		Shop( Shop const & other );
		void operator=( Shop const & other );
	};

	//-----------------------------------------------------------------------
}