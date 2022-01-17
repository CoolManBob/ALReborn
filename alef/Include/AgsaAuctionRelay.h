/*============================================================================

	AgsaAuctionRelay.h

============================================================================*/

#ifndef _AGSA_AUCTION_RELAY_H_
	#define _AGSA_AUCTION_RELAY_H_

#include "ApBase.h"
#include "AsDefine.h"
#include "ApAdmin.h"
#include "AgsdAuctionRelay.h"

/****************************************************/
/*		The Definition of AgsaAuctionRelayclass		*/
/****************************************************/
//
class AgsaAuctionRelay : public ApAdmin
	{
	public:
		AgsaAuctionRelay();
		virtual ~AgsaAuctionRelay();

		// add/remove
		AgsdAuctionRelayCharacter*	Add(CHAR *pszCharacter, AgsdAuctionRelayCharacter* pcsAuctionRelayCharacter);
		BOOL						Remove(CHAR *pszCharacter);

		// get
		AgsdAuctionRelayCharacter*	Get(CHAR *pszCharacter);
	};

#endif	// _AGSA_SERVER_H_