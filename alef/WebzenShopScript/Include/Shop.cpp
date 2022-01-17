#include "Shop.h"
#include <algorithm>

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	Shop & Shop::Instance()
	{
		static Shop inst;
		return inst;
	}
	
	//-----------------------------------------------------------------------
	//

	void Shop::SetShopVersion( unsigned short year, unsigned short yearId )
	{
		// 샵정보 초기화
		Instance().categorys.clear();

		Instance().ParseShopData();
	}

	//-----------------------------------------------------------------------
	//

	bool Shop::ParseShopData()
	{
		CShopList * shopList = Script::GetList();

		if( !shopList ) 
			return false;

		CShopCategoryList* categoryList = shopList->GetCategoryListPtr();

		if( !categoryList ) 
			return false;

		//categoryList->SortPackageLinkList(); //최초 자동정렬 되어있음

		CShopCategory rootCategory;
		if( categoryList->GetValueByIndex( 0, rootCategory ) )
		{
			// 원래는 첫번째 카테고리를 읽어들였으나.. 다음과 같은 사유로 첫번째 카테고리는 무시하게 되었음.
			// 1. 웹젠 샵 데이터 상에서 카테고리는 상위 카테고리 / 하위 카테고리로 나뉘어지며 CShopCategoryList 로부터 직접 얻어지는 CShopCategory 는
			//    상위 카테고리이다.
			// 2. 상위 카테고리는 구현상 상품을 등록할 수 없도록 되어 있기 때문에 반드시 하위 카테고리를 하나 이상 가져야 하며 상품은 하위 카테고리에
			//    등록된다.
			// 3. 따라서 이 규칙에 맞추기 위하여 상위 카테고리 1개를 등록하고 실제로 샵 목록에 표시될 카테고리들은 모두 이 상위 카테고리의 하위 카테고리로
			//    등록하도록 한다.
			// 4. 따라서 코드 구현상 첫번째로 검색되는 카테고리는 카테고리 목록에 추가하지 않고 무시하며 이 카테고리로 부터 얻어지는 하위 카테고리들만
			//    추가하도록 한다.


			// 세컨드 카테고리 정보로 순회
			int categoryIdx = 0;
			for( rootCategory.SetChildCategorySeqFirst(); rootCategory.GetChildCategorySeqNext( categoryIdx ); )
			{
				CShopCategory category;
				if( categoryList->GetValueByKey( categoryIdx, category ) )
				{
					categorys[ category.ProductDisplaySeq ] = Category( category );
				}
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------
	//

	Category const * Shop::GetCategory( int categoryId )
	{
		Categorys const & categorys = GetCategorys();

		Categorys::const_iterator iter = categorys.find( categoryId );

		return iter == categorys.end() ? 0 : &(iter->second);
	}

	//-----------------------------------------------------------------------
}