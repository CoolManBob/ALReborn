#pragma once
#include "sdkconfig.h"
#include "sdtalk\SDPacket.h"

class PTSDK_STUFF CPacketProcessor
{
public:
	virtual void DoPacket(CSDPacket & packet) = 0;
};
