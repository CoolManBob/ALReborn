#ifndef __PACKET_TYPE_SC__
#define __PACKET_TYPE_SC__

#include "ApPacket.h"

#define PATCH_DOWNLOAD_SERVER_LENGTH	256

#pragma pack(1)

//받아야하는 패치 파일의수, 버전를 받는다.
struct CPacketTypeSC_0x01 : public PATCH_PACKET_HEADER
{
public:

	int				m_iRecvFiles;
	int				m_iVersion;
	int				m_iPatchCheckCode;
	__int64			m_iTotalSize;

	char			m_strServerIP[PATCH_DOWNLOAD_SERVER_LENGTH];
	unsigned short	m_iServerPort;

	void Init()
	{
		m_iSize = sizeof(CPacketTypeSC_0x01);
		m_iCommand = 0x01;
		m_iPatchCheckCode = 0;
	}

	CPacketTypeSC_0x01()
	{
		Init();
	}
};

//실제로 파일을 추가한다.
struct CPacketTypeSC_0x02 : public PATCH_PACKET_HEADER
{
public:

	void Init()
	{
		m_iSize = sizeof(CPacketTypeSC_0x02);
		m_iCommand = 0x02;
	}

	CPacketTypeSC_0x02()
	{
		Init();
	}
};

//실제로 파일을 지운다.
struct CPacketTypeSC_0x03 : public PATCH_PACKET_HEADER
{
public:

	int				m_PatchVersion;

	void Init()
	{
		m_iSize			= sizeof(CPacketTypeSC_0x03);
		m_iCommand		= 0x3;
		m_PatchVersion	=	0;
	}

	CPacketTypeSC_0x03()
	{
		Init();
	}
};

//파일 조각을 보냄.
struct CPacketTypeSC_0x12 : public PATCH_PACKET_HEADER
{
public:
	bool			m_bComplete;

	void Init()
	{
		m_iSize = sizeof(CPacketTypeSC_0x12);
		m_iCommand = 0x12;
		m_bComplete = false;
	}

	CPacketTypeSC_0x12()
	{
		Init();
	}
};

//클라이언트 실행파일의 CRC 를 보냄
struct CPacketTypeSC_0x05 : public PATCH_PACKET_HEADER
{
public:
	unsigned long		m_ulCRC;

	void Init()
	{
		m_iSize = sizeof(CPacketTypeSC_0x05);
		m_iCommand = 0x05;
		m_ulCRC = 0;
	}

	CPacketTypeSC_0x05()
	{
		Init();
	}
};

//ErrorCode
struct CPacketTypeSC_0xEE : public PATCH_PACKET_HEADER
{
public:
	int				m_iErrorCode;

	void Init()
	{
		m_iSize = sizeof(CPacketTypeSC_0xEE);
		m_iCommand = 0xEE;
		m_iErrorCode = 0;
	}

	CPacketTypeSC_0xEE()
	{
		Init();
	}
};

struct CPacketTypeSC_0xff : public PATCH_PACKET_HEADER
{
public:
	unsigned int	m_iSeq;

	void Init()
	{
		m_iSize = sizeof(CPacketTypeSC_0xff);
		m_iCommand = 0xff;
		m_iSeq = 0;
	}

	CPacketTypeSC_0xff()
	{
		Init();
	}
};

#pragma pack()

#endif
