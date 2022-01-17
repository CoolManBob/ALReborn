#pragma once
#include "SDMessage.h"
#include "packet.h"

class CSDPacket :
	public CPacket
{
public:
	CSDPacket(void);
	~CSDPacket(void);

	void PutSDMessage(CSDMessage & msg );
	static void	PutSDMessage(CSDMessage & msg, unsigned char * buf);

	CSDMessage * GetSDMessage();
	bool		 GetSDMessage(CSDMessage & msg);

	static CSDMessage * GetSDMessage( const char * packetAddr );	
	static bool			GetSDMessage( const char * packetAddr, CSDMessage & msg );
};
