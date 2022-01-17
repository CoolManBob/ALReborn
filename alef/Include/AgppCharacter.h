#ifndef AGPPCHARACTER_H
#define AGPPCHARACTER_H

//////////////////////////////////////////////////////////////////////////////
//

#define MAX_PINCH_WANTED_CHARACTER		5

struct PACKET_CHARACTER : public PACKET_HEADER
{
	CHAR	Flag1;
	CHAR	Flag2;
	CHAR	Flag3;
	CHAR	Flag4;

	INT8	pcOperation;

	PACKET_CHARACTER()
		: pcOperation(0), Flag1(1), Flag2(0), Flag3(0), Flag4(0)
	{
		cType			= AGPMCHARACTER_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_CHARACTER);
	}
};

struct PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST : public PACKET_CHARACTER
{
	INT32	lUserID;
	INT32   lRequestCharacterID;

	PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST()
		: lUserID(0), lRequestCharacterID(0)
	{
		pcOperation		= AGPMCHAR_PACKET_OPERATION_REQUEST_PINCHWANTED_CHARACTER;
		unPacketLength  = (UINT16)sizeof(PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST);
	}
};

struct PACKET_CHARACTER_PINCHWANTED_CHAR_ANSWER : public PACKET_CHARACTER
{
	INT32   lPinchCharacterID;
	INT32	lPinchWantedCharacter[MAX_PINCH_WANTED_CHARACTER];

	PACKET_CHARACTER_PINCHWANTED_CHAR_ANSWER()
		: lPinchCharacterID(0)
	{
		pcOperation		= AGPMCHAR_PACKET_OPERATUIN_ANSWER_PINCHWANTED_CHARACTER;
		unPacketLength	= (UINT16)sizeof(PACKET_CHARACTER_PINCHWANTED_CHAR_ANSWER);
		ZeroMemory(lPinchWantedCharacter, sizeof(lPinchWantedCharacter));
	}
};

#endif