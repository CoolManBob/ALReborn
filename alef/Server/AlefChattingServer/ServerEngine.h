#if !defined(__SERVERENGINE_H__)
#define __SERVERENGINE_H__

#include "AgsEngine.h"

#include "AgpmAreaChatting.h"
#include "AgsmAreaChatting.h"

class ServerEngine : public AgsEngine
{
public:
	ServerEngine();
	~ServerEngine();

	BOOL	OnRegisterModule();
	BOOL	OnTerminate();
};

#endif	//__SERVERENGINE_H__