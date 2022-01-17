#ifndef  __WBANetwork_Common_H
#define  __WBANetwork_Common_H

#pragma warning(disable:4127)

#include <winsock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <process.h> 
#include <assert.h>
#include <stdarg.h>
#include <conio.h>

#pragma warning(push)
#pragma warning(disable:4702)

#include <iostream>
#include <tchar.h>
#include <strsafe.h>

#pragma warning(pop)

namespace WBANetwork
{
	extern	void	CallbackErrorHandler( DWORD lastError, TCHAR* desc );
}


#endif