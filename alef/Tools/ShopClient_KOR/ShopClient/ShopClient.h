#ifndef _SHOPCLIENT_H
#define _SHOPCLIENT_H

#include "WebzenBilling_Kor.h"

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD_x64_KOR.lib") 
	#else
		#pragma comment(lib, "WebzenBilling_x64_KOR.lib") 
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD_KOR.lib") 
	#else
		#pragma comment(lib, "WebzenBilling_KOR.lib") 
	#endif
#endif

#endif //_SHOPCLIENT_H