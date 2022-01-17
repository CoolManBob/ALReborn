/*====================================================================

	AgpmLogin.cpp
	
====================================================================*/


#include "AgpmLogin.h"


/****************************************************/
/*		The Implementation of Extra Data class		*/
/****************************************************/
//
BOOL AgpdLoginCompenItemList::Parse(CHAR *pszItemDetails)
	{
	if (!pszItemDetails || 0 >= _tcslen(pszItemDetails))
		return FALSE;
	
	m_lCount = 0;
	TCHAR szItem[51];
	ZeroMemory(szItem, sizeof(szItem));
	TCHAR *pszItem = szItem;

	while (TRUE)
		{
		if (m_lCount >= AGPMLOGINDB_DB_MAX_COMPEN_MASTER)
			break;

		if (_T('\0') == *pszItemDetails)
			{
			if (_tcslen(szItem) > 0)
				{
				if (2 == _stscanf(szItem, _T("%d:%d"), &(m_Items[m_lCount].m_lItemTID), &(m_Items[m_lCount].m_lItemQty)))
					m_lCount++;
				ZeroMemory(szItem, sizeof(szItem));
				}
			break;
			}

		if (_T(',') == *pszItemDetails)
			{
			if (2 == _stscanf(szItem, _T("%d:%d"), &(m_Items[m_lCount].m_lItemTID), &(m_Items[m_lCount].m_lItemQty)))
				m_lCount++;
			ZeroMemory(szItem, sizeof(szItem));
			pszItem = szItem;
			pszItemDetails++;
			}
		
		*pszItem++ = *pszItemDetails++;
		}		
	
	return TRUE;
	}




/****************************************************/
/*		The Implementation of AgpmLogin class		*/
/****************************************************/
//
AgpmLogin::AgpmLogin()
	{
	SetModuleName("AgpmLogin");
	//SetPacketType(AGPMLOGIN_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));

	m_csPacket.SetFieldType(AUTYPE_INT8,			1,								// Operation
							AUTYPE_CHAR,			ENCRYPT_STRING_SIZE,			// EncryptCode
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING+1,	// Account ID
							AUTYPE_INT8,			1,								// Account ID Length
							AUTYPE_CHAR,			AGPACHARACTER_MAX_PW_STRING+1,	// Password
							AUTYPE_INT8,			1,								// Password Length
							AUTYPE_INT32,			1,								// CID
							AUTYPE_CHAR,			AGPMLOGIN_SERVERGROUPNAMESIZE,	// Server Address Packet
							AUTYPE_PACKET,			1,								// Character Info. Packet
							AUTYPE_PACKET,			1,								// Server Info. Packet
							AUTYPE_INT32,			1,								// Result
							AUTYPE_PACKET,			1,								// Version Info. Packet
							AUTYPE_CHAR,			AGPMLOGIN_J_AUTHSTRING+1,		// gamestring
							AUTYPE_CHAR,			AGPMLOGIN_EKEY_CHALLENGE,		// challenge number for ekey
							AUTYPE_INT32,			1,								// isLimited
							AUTYPE_INT32,			1,								// isProtected
							AUTYPE_END,				0
							);

	m_csPacketServerAddr.SetFlagLength(sizeof(INT8));
	m_csPacketServerAddr.SetFieldType(AUTYPE_CHAR,		AGPMLOGIN_IPADDRSIZE, // GameServerAddr
								AUTYPE_CHAR,		AGPMLOGIN_IPADDRSIZE, // AuctionServerAddr
								AUTYPE_CHAR,		AGPMLOGIN_IPADDRSIZE, // RecruitServerAddr
								AUTYPE_END,			0
								);

	m_csPacketCharInfo.SetFlagLength(sizeof(INT16));
	m_csPacketCharInfo.SetFieldType(AUTYPE_INT32,	1,  //TID
									AUTYPE_CHAR,	AGPACHARACTER_MAX_ID_STRING + 1, //CharName
									AUTYPE_INT32,	1,	//MaxRegisterChars
									AUTYPE_INT32,	1,	//SlotIndex
									AUTYPE_INT32,	1,	//Union Info
									AUTYPE_INT32,	1,	//Race Info
									AUTYPE_INT32,	1,	//hair index
									AUTYPE_INT32,	1,	//face index
									AUTYPE_CHAR,	AGPACHARACTER_MAX_ID_STRING + 1, // New Char ID
									AUTYPE_END,		0
									);

	m_csPacketServerInfo.SetFlagLength(sizeof(INT8));
	m_csPacketServerInfo.SetFieldType(AUTYPE_CHAR,	AGPMLOGIN_IPADDRSIZE,  //IPAddress
									  AUTYPE_END,	0
									  );

	m_csPacketVersionInfo.SetFlagLength(sizeof(INT8));
	m_csPacketVersionInfo.SetFieldType(AUTYPE_INT32,		1,		// major version
									   AUTYPE_INT32,		1,		// minor version
									   AUTYPE_END,			0
									   );

	m_csPacketCompenInfo.SetFlagLength(sizeof(INT8));
	m_csPacketCompenInfo.SetFieldType(AUTYPE_INT32,			1,		// compen id
									  AUTYPE_INT32,			1,		// compen type,
									  AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// char id
									  AUTYPE_CHAR,			100,	// item details
									  AUTYPE_MEMORY_BLOCK,	1,		// description
									  AUTYPE_END,			0
									  );

	InitFuncPtr();	// 함수 포인터 초기화
	}


AgpmLogin::~AgpmLogin()
	{
	}




//	ApModule inherited
//=====================================
//
BOOL AgpmLogin::OnAddModule()
	{
	
	return TRUE;
	}




//	Check
//=============================================
//
BOOL AgpmLogin::IsDuplicatedCharacterOfMigration(CHAR *pszChar)
	{
	CHAR *psz = pszChar;
	if (NULL == psz)
		return FALSE;

	if (AGPMLOGIN_MIN_CHARID_SIZE > _tcslen(pszChar))
		return TRUE;

	while (_T('\0') != *psz)
		{
		if (_T('@') == *psz)
			return TRUE;

		psz = GetNextChar(psz);
		}
	
	return FALSE;
	}

void AgpmLogin::InitFuncPtr( void )
{
	if (AP_SERVICE_AREA_KOREA == g_eServiceArea )
	{
		GetNextCharPtr = &AgpmLogin::GetNextCharKr;
	}

	if (AP_SERVICE_AREA_CHINA == g_eServiceArea )
	{
		GetNextCharPtr = &AgpmLogin::GetNextCharCn;
	}

	if (AP_SERVICE_AREA_WESTERN == g_eServiceArea )
	{
		GetNextCharPtr = &AgpmLogin::GetNextCharEn;
	}

	if (AP_SERVICE_AREA_JAPAN == g_eServiceArea )
	{
		GetNextCharPtr = &AgpmLogin::GetNextCharJp;
	}
}

char* AgpmLogin::GetNextCharKr(unsigned char* str)
{
	return ( *str > 0x80 ) ? (char*)(str + 2) : (char*)(str + 1);
}

char* AgpmLogin::GetNextCharEn(unsigned char* str)
{
	return (char*)(str + 1);
}

char* AgpmLogin::GetNextCharCn(unsigned char* str)
{
	return ( *str > 0x80 ) ? (char*)(str + 2) : (char*)(str + 1);
}

char* AgpmLogin::GetNextCharJp(unsigned char* str)
{
	if ( ((0x80 < *str) && (*str <= 0x9F)) || (*str >= 0xE0) )
		return (char*)(str + 2);
	return (char*)(str + 1);
}
