#ifndef _APPP_CASHMALL_H
#define _APPP_CASHMALL_H

#include "WebzenBilling_Kor.h" 
//#include "AgsmBilling.h" 

#pragma pack(1)

struct PACKET_CASHMALL : public PACKET_HEADER
{
	CHAR	Flag1;
	CHAR	Flag2;

	INT8	pcOperation;
	INT32	lCID;

	PACKET_CASHMALL()
		: pcOperation(0), Flag1(1), Flag2(0), lCID(AP_INVALID_CID)
	{
		cType			= AGPMCASHMALL_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_CASHMALL);
	}
};

// C -> S : 샵 아이템 구매요청
struct PACKET_CASHMALL_BUY_CSREQ : public PACKET_CASHMALL
{
	INT32 m_nPackageSeq;
	INT32 m_nDisPlaySeq;
	INT32 m_nPriceSeq;

	PACKET_CASHMALL_BUY_CSREQ(INT32 CID, INT32 PackageSeq, INT32 DisplaySeq, INT32 PriceSeq)
	{
		pcOperation		= AGPMCASH_OPERATION_REQUEST_BUY_ITEM_WEBZEN;
		unPacketLength	= (UINT16)sizeof(PACKET_CASHMALL_BUY_CSREQ);

		lCID = CID;
		m_nPackageSeq = PackageSeq;
		m_nDisPlaySeq = DisplaySeq;
		m_nPriceSeq   = PriceSeq;
	}
};
// S -> C : 샵 아이템 구매 결과
struct PACKET_CASHMALL_BUY_SCACK : public PACKET_CASHMALL
{
	INT32 m_nResultCode;
	INT32 m_nLeftProductCount;

	PACKET_CASHMALL_BUY_SCACK(INT32 nResultCode, INT32 nLeftProductCount)
	{
		pcOperation		= AGPMCASH_OPERATION_RESPONSE_BUY_RESULT_WEBZEN;
		unPacketLength	= (UINT16)sizeof(PACKET_CASHMALL_BUY_SCACK);

		m_nResultCode = nResultCode;
		m_nLeftProductCount = nLeftProductCount;
	}

};

// C -> S : 유료보관함 아이템 사용요청 (UseStorage)
struct PACKET_CASHMALL_USESTORAGE_CSREQ : public PACKET_CASHMALL
{
	INT32 m_nStorageSeq;
	INT32 m_nStorageItemSeq;
	

	PACKET_CASHMALL_USESTORAGE_CSREQ(INT32 CID, INT32 nStorageSeq, INT32 nStorageItemSeq)
	{
		pcOperation		= AGPMCASH_OPERATION_USESTORAGE_CSREQ;
		unPacketLength	= (UINT16)sizeof(PACKET_CASHMALL_USESTORAGE_CSREQ);

		lCID = CID;
		m_nStorageSeq = nStorageSeq;
		m_nStorageItemSeq = nStorageItemSeq;
		
	}
};

// S -> C : 샵 아이템 구매 결과
struct PACKET_CASHMALL_USESTORAGE_SCACK : public PACKET_CASHMALL
{
	INT32 m_nResultCode;
	

	PACKET_CASHMALL_USESTORAGE_SCACK(INT32 nResultCode)
	{
		pcOperation		= AGPMCASH_OPERATION_USESTORAGE_SCACK;
		unPacketLength	= (UINT16)sizeof(PACKET_CASHMALL_USESTORAGE_SCACK);

		m_nResultCode = nResultCode;
		
	}

};

// C -> S : 유료보관함 리스트 요청 사용요청 (InquireStorageListPageGiftMessage)
struct PACKET_CASHMALL_INQUIRESTORAGELIST_CSREQ : public PACKET_CASHMALL
{
	INT32 m_nNowPage; //현재 조회할 페이지 번호


	PACKET_CASHMALL_INQUIRESTORAGELIST_CSREQ(INT32 CID, INT32 nNowPage)
	{
		pcOperation		= AGPMCASH_OPERATION_INQUIRESTOAGELIST_CSREQ;
		unPacketLength	= (UINT16)sizeof(PACKET_CASHMALL_INQUIRESTORAGELIST_CSREQ);

		lCID = CID;
		m_nNowPage = nNowPage;
		

	}
};

// C -> S : 유료보관함 리스트 요청 사용요청 (InquireStorageListPageGiftMessage)
struct PACKET_CASHMALL_INQUIRESTORAGELIST_SCACK : public PACKET_CASHMALL
{
	INT32 m_nResultCode;

	INT32 m_nNowPage; //현재 조회할 페이지 번호
	INT32 m_nListCount; //현재 페이지의 상품갯수
	INT32 m_nTotalCount; //전체 보관함 상품갯수
	INT32 m_nTotalPage;		// 보관한 전체 페이지
	STStorageNoGiftMessage m_pStorageList[STORAGELISTCOUNT_PER_PAGE];


	PACKET_CASHMALL_INQUIRESTORAGELIST_SCACK(INT32 nResultCode, INT32 nNowPage, INT32 nTotalPage, INT32 nListCount, INT32 nTotalCount, STStorageNoGiftMessage* pStorageList)
	{
		pcOperation		= AGPMCASH_OPERATION_INQUIRESTOAGELIST_SCACK;
		unPacketLength	= (UINT16)sizeof(PACKET_CASHMALL_INQUIRESTORAGELIST_SCACK);

		m_nResultCode = nResultCode;
		m_nNowPage = nNowPage;
		m_nTotalPage = nTotalPage;
		m_nListCount = nListCount;
		m_nTotalCount = nTotalCount;
		memcpy(m_pStorageList, pStorageList, sizeof(m_pStorageList));

	}
};

#pragma pack()

#endif //_APPP_CASHMALL_H