#pragma once
#include "ShopListManager.h"

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	class Script : public CShopListManager
	{
	public:
		static Script &											Instance();
		static CShopList *										GetList();
		static bool												SetVersion( unsigned short year, unsigned short yearId );

		static bool												DeleteFiles( void );

		bool													LoadScript();

		virtual													~Script();

		CListVersionInfo										versionInfo;

	private:
		static bool												DeleteFolder( char* pFolder );

		Script() {}
		Script( Script const & other );
		void operator=( Script const & other );
	};

	//-----------------------------------------------------------------------
}