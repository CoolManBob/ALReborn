#ifndef AGPPAGPPADMIN_H
#define AGPPAGPPADMIN_H

////////////////////////////////////////////////////////////////////////////////////////
// define Admin Packet
struct PACKET_AGPMADMIN_ADMIN : public PACKET_HEADER
{
	CHAR	Flag1;
	CHAR	Flag2;
	
	INT8	pcOperation;

	PACKET_AGPMADMIN_ADMIN()
		: pcOperation(0), Flag1(3), Flag2(0)
	{
		cType			= AGPMADMIN_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPMADMIN_ADMIN);
	}
};

struct PACKET_AGPMADMIN_USER_POSITON : public PACKET_AGPMADMIN_ADMIN
{
	INT32 lCID;
	INT32 lRegionIndexCount;
	INT32 arRegionIndex[AGPMADMIN_MAX_REGION_INDEX];

	PACKET_AGPMADMIN_USER_POSITON(INT32 RegionIndexCount, INT32 *RegionIndex, INT32 _lCID) 
	{
		lRegionIndexCount = RegionIndexCount;
		pcOperation = AGPMADMIN_PACKET_USER_POSITION_ANSWER;
		unPacketLength = (UINT16)sizeof(PACKET_AGPMADMIN_USER_POSITON);
		lCID = _lCID;

		memcpy(arRegionIndex, RegionIndex, (sizeof(INT32)*RegionIndexCount));
	}
};

struct PACKET_AGPMADMIN_USER_POSITION_REQUEST : public PACKET_AGPMADMIN_ADMIN
{
	INT32 lCID;

	PACKET_AGPMADMIN_USER_POSITION_REQUEST(INT32 _lCID) 
	{
		lCID = _lCID;
		pcOperation		= AGPMADMIN_PACKET_USER_POSITION_REQUEST;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPMADMIN_USER_POSITION_REQUEST);
	}
};

#endif // AGPPADMIN_H