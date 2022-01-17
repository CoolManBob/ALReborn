#include "EventProperty.h"

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	void EventProperty::SetEventProperty( int imageTag )
	{
		switch( imageTag )
		{
		case 680:
			SetEvent( eEvent::NEW );
			break;

		case 681:
			SetEvent( eEvent::HOT );
			break;

		case 682:
			SetEvent( eEvent::SALE );
			break;

		case 683:
			SetEvent( eEvent::EVENT );
			break;

		case 781:
			SetEvent( eEvent::COMMON );
			break;

		case 782:
			SetEvent( eEvent::PACKAGE );
			break;
		}
	}

	//-----------------------------------------------------------------------
}