/*========================================================================

	AgsaAuctionRelay.cpp

========================================================================*/

#include "AgsaAuctionRelay.h"

/************************************************************/
/*		The Implementation of AgsaAuctionRelay class		*/
/************************************************************/
//
AgsaAuctionRelay::AgsaAuctionRelay()
	{
	}

AgsaAuctionRelay::~AgsaAuctionRelay()
	{
	}

//	Add
//============================================
//
AgsdAuctionRelayCharacter* AgsaAuctionRelay::Add(CHAR* pszCharacter, AgsdAuctionRelayCharacter* pcsAuctionRelayCharacter)
	{
	if (!pcsAuctionRelayCharacter || !AddObject((PVOID) &pcsAuctionRelayCharacter, pszCharacter))
		return NULL;

	return pcsAuctionRelayCharacter;
	}

//	Remove
//============================================
//
BOOL AgsaAuctionRelay::Remove(CHAR *pszCharacter)
	{
	if (!pszCharacter || '\0' == pszCharacter)
		return FALSE;

	return RemoveObject(pszCharacter);
	}

//	Get
//============================================
//
AgsdAuctionRelayCharacter* AgsaAuctionRelay::Get(CHAR *pszCharacter)
	{
	if (!pszCharacter || '\0' == pszCharacter)
		return NULL;

	AgsdAuctionRelayCharacter **ppcsAuctionRelayCharacter =
		(AgsdAuctionRelayCharacter **) GetObject(pszCharacter);

	if (!ppcsAuctionRelayCharacter)
		return NULL;

	return *ppcsAuctionRelayCharacter;
	}

