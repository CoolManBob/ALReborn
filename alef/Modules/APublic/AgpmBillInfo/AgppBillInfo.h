#ifndef _APPP_BILLINFO_H
#define _APPP_BILLINFO_H

#pragma pack(1)


struct PACKET_BILLINGINFO : public PACKET_HEADER
{
	CHAR	Flag1;

	INT8	pcOperation;
	INT32	lCID;

	PACKET_BILLINGINFO()
		: pcOperation(0), Flag1(1), lCID(AP_INVALID_CID)
	{
		cType			= AGPMBILLINFO_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_BILLINGINFO);
	}
};

// S -> C
struct PACKET_BILLINGINFO_CASHINFO : public PACKET_BILLINGINFO
{
	INT64 m_pWCoin;
	INT64 m_pPCoin;

	PACKET_BILLINGINFO_CASHINFO(INT32 CID, INT64 WCoin, INT64 PCoin)
	{
		pcOperation		= AGPMBILL_OPERATION_CASHINFO;
		unPacketLength	= (UINT16)sizeof(PACKET_BILLINGINFO_CASHINFO);

		lCID = CID;
		m_pWCoin = WCoin;
		m_pPCoin = PCoin;
	}
};

// LS -> GS
struct PACKET_BILLINGINFO_GUID : public PACKET_BILLINGINFO
{
	DWORD m_dwAccountGUID;
	DWORD m_dwPCRoomGuid;//JK_웹젠인증
	DWORD m_dwClientCnt; //JK_복수클라이언트 
	DWORD m_dwAge;		 //JK_심야샷다운

	PACKET_BILLINGINFO_GUID(INT32 CID, DWORD AccountGUID)
	{
		pcOperation		= AGPMBILL_OPERATION_GUID;
		unPacketLength	= (UINT16)sizeof(PACKET_BILLINGINFO_GUID);

		lCID = CID;
		m_dwAccountGUID = AccountGUID;
	}

};
//JK_웹젠빌링 : 버전정보 요청
// C -> S
struct PACKET_BILLINGINFO_VERSIONINFO_CSREQ : public PACKET_BILLINGINFO
{

	PACKET_BILLINGINFO_VERSIONINFO_CSREQ(INT32 CID)
	{
		pcOperation		= AGPMBILL_OPERATION_VERSIONINFO_CSREQ;
		unPacketLength	= (UINT16)sizeof(PACKET_BILLINGINFO_VERSIONINFO_CSREQ);

		lCID = CID;
	}
};
// S -> C  : 버전정보 응답
struct PACKET_BILLINGINFO_VERSIONINFO_SCACK : public PACKET_BILLINGINFO
{
	int m_nYear;
	int m_nYearIdentity;

	PACKET_BILLINGINFO_VERSIONINFO_SCACK(int nYear, int nYearIdentity)
	{
		pcOperation		= AGPMBILL_OPERATION_VERSIONINFO_SCACK;
		unPacketLength	= (UINT16)sizeof(PACKET_BILLINGINFO_VERSIONINFO_SCACK);
		
		m_nYear = nYear;
		m_nYearIdentity = nYearIdentity;
	}
};

#pragma pack()

#endif //_APPP_BILLINFO_H