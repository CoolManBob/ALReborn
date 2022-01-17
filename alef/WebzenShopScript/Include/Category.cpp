#include "Category.h"

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	Category::Category()
		: id(0)
		, isOpen(false)
		, isEvent(false)
	{}

	//-----------------------------------------------------------------------
	//

	Category::Category( CShopCategory & scriptData )
		: id( scriptData.ProductDisplaySeq )
		, name( scriptData.CategroyName )
		, isOpen( scriptData.OpenFlag == 202 ? false : true )
		, isEvent( scriptData.EventFlag == 199 ? true : false )
	{
		CShopList * shopList = Script::GetList();

		if( !shopList )
			return;

		CShopPackageList * packageList = shopList->GetPackageListPtr();

		if( !packageList )
			return;

		// 패키지 목록 사이즈 예약
		packages.reserve( packageList->GetSize() );

		// 패키지 리스트 순회
		int pakageIdx = 0;
		for( scriptData.SetChildPackagSeqFirst(); scriptData.GetChildPackagSeqNext( pakageIdx ); )
		{
			CShopPackage package;
			if( packageList->GetValueByKey( pakageIdx, package ) )
			{
				packages.push_back( Package( id, order, package ) );
			}
		}
	}

	//-----------------------------------------------------------------------
	//

	bool Category::IsPCRoomCategory() const
	{
		return name.find( "PC" ) != std::string::npos ? true : false;
	}

	//-----------------------------------------------------------------------
	//

	bool Category::operator<( Category const & other ) const
	{
		return order < other.order;
	}

	//-----------------------------------------------------------------------
}