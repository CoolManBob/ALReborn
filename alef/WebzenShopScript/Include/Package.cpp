#include "Package.h"
#include <assert.h>

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	Package::Package()
		: categoryId(0)
		, id(0)
		, price(0)
		, priceId(0)
		, mainProductId(0)
		, mainItemId(0)
	{}

	//-----------------------------------------------------------------------
	//

	Package::Package( int categoryId, int order, CShopPackage & scriptData )
		: categoryId( categoryId )
		, id( scriptData.PackageProductSeq )
		, price( scriptData.Price )
		, priceId( 0 )
		, mainProductId(0)
		, mainItemId(0)
		, name( scriptData.PackageProductName )
		, desc( scriptData.Description )
		, order(order)
	{
		// 이벤트 속성 셋팅
		SetEventProperty( scriptData.ImageTagType );

		// 가격 코드 읽기 - 아크로드에서는 첫번째 정보만 사용합니다.
		scriptData.GetPriceSeqFirst( priceId );

		// 프로덕트 갯수 예약
		products.reserve( scriptData.GetProductCount() );

		CShopList * shopList = Script::GetList();

		if( !shopList )
			return;

		CShopProductList * productList = shopList->GetProductListPtr();

		if( !productList )
			return;

		// 패키지에 포함된 프로덕트들의 정보를 읽어옵니다.
		int productId = 0;
		for( scriptData.SetProductSeqFirst(); scriptData.GetProductSeqNext( productId ); )
		{
			CShopProduct product;
			if( productList->GetValueByKey( productId, product ) )
				products.push_back( Product( id, priceId, product ) );
		}

		int packageTID = 0;

		packageTID = atoi( scriptData.InGamePackageID );

		setmain( packageTID );
	}

	//-----------------------------------------------------------------------
	//

	void Package::setmain(int packageTID)
	{
		mainItemId = 0;
		mainProductId = 0;

		if( packageTID )
		{
			mainItemId = packageTID;
			return;
		}

		// 패키지의 메인 아이템 설정
		if( !products.empty() )
		{
			for( size_t i=0; i<products.size(); ++i )
			{
				Product & product = products[i];

				if( mainItemId == 0 )
				{
					mainProductId = products[i].id;
					mainItemId = products[i].itemId;
				}
				else if( product.itemId != 0 )
				{
					if( product.isMain )
					{
						mainProductId = products[i].id;
						mainItemId = products[i].itemId;

						break;
					}
				}
			}			
		}
	}

	//-----------------------------------------------------------------------
}