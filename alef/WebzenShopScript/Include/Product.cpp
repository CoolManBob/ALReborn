#include "Product.h"

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	Product::Product() : packageId(0), id(0), itemId(0), price(0), priceId(0) {}

	//-----------------------------------------------------------------------
	//

	Product::Product( int packageId, int priceId, CShopProduct & scriptData )
		: packageId( packageId )
		, id(scriptData.ProductSeq)
		, itemId(0)
		, price(0)
		, priceId(priceId)
		, name(scriptData.ProductName)
		, desc(scriptData.Description)
	{
		CShopList * shopList = Script::GetList();

		if( !shopList )
			return;

		CShopProductLinkList * productLinkList = shopList->GetProductLinkListPtr();

		if( !productLinkList )
			return;

		// 아크로드에서는 첫번째 아이템 정보만 사용합니다.
		productLinkList->SetProductFirst( id, priceId );
		CShopProductLink productLink;
		if( productLinkList->GetProductNext( productLink ) )
		{
			itemId = productLink.InGameProductID;
			price = productLink.Price;
			isMain = productLink.vOrder == 1 ? true : false;
		}
	}

	//-----------------------------------------------------------------------
}