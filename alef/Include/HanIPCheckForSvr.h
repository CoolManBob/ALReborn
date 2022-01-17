#ifndef __HAN_IPCHECKFORSVR_H__
#define __HAN_IPCHECKFORSVR_H__

#ifdef HANIPCHECKFORSVR_EXPORTS
	#define HANIPCHECK_API __declspec(dllexport)
#else
	#define HANIPCHECK_API __declspec(dllimport)
	#ifdef _DEBUG
		#ifdef _WIN64
			#pragma comment(lib, "HanIPCheckForSvrD_x64.lib") 
		#else
			#pragma comment(lib, "HanIPCheckForSvrD.lib") 
		#endif
	#else
		#ifdef _WIN64
			#pragma comment(lib, "HanIPCheckForSvr_x64.lib") 
		#else
			#pragma comment(lib, "HanIPCheckForSvr.lib") 
		#endif
	#endif
#endif

#pragma comment(lib, "ws2_32.lib")

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif

// Constants
#define SERVICE_NATION										0x000000FF
#define SERVICE_KOR											0x00000001
#define SERVICE_USA											0x00000002
#define SERVICE_JPN											0x00000003
#define SERVICE_CHN											0x00000004

#define SERVICE_TYPE										0x00000F00
#define SERVICE_ALPHA										0x00000100
#define SERVICE_REAL										0x00000200
#define SERVICE_BETA										0x00000300

#define SERVICE_SITE										0x0000F000
#define SERVICE_HANGAME										0x00001000
#define SERVICE_ASOBLOG										0x00002000

/*
Return value Info:
0 : Ok
- : fault from  function / this system 
-1 : general fault.
+ : fault from server
*/
#define HAN_IPCHECK_OK										0
#define HAN_IPCHECK_ARGUMENT_INVALID						-1
#define HAN_IPCHECK_INITED_NOT								-2
#define HAN_IPCHECK_INITED_FAIL								-3
#define HAN_IPCHECK_INITED_CONNPOOL							-4
#define HAN_IPCHECK_GETCONN_FAIL							-5
#define HAN_IPCHECK_SENDREQ_FAIL							-6
#define HAN_IPCHECK_RECVRESULT_FAIL							-7
#define HAN_IPCHECK_RECVRESULT_INVALID						-8
#define HAN_IPCHECK_DBGW_RPC_CLIENT_CREATEION_FAILED		-9
#define HAN_IPCHECK_DBGW_RPC_CONNECTOR_CREATEION_FAILED		-10
#define HAN_IPCHECK_DBGW_CONNECTION_FAILED					-11
#define HAN_IPCHECK_DBGW_EXEC_FAILED						-12
#define HAN_IPCHECK_DBGW_SELECTRESULT_FAILED				-13
#define HAN_IPCHECK_DBGW_SELECTRESULT_INVALID				-14
#define HAN_IPCHECK_DBGW_SELECTRESULT_COLUMN_INVALID		-15
#define HAN_IPCHECK_DBGW_LOAD_LOCAL_DIRECTORY_FILE_FAILED	-16
#define HAN_IPCHECK_DBGW_FUNCTIONNAME_INVALID				-17
#define HAN_IPCHECK_DBGW_SELECTRESULT_OVERCOUNT				-18
#define HAN_IPCHECK_DLL_UNEXPECTED							-100

#define HAN_IPCHECK_ERRSTR_INVALID_IP						"ERROR001"
#define HAN_IPCHECK_ERRSTR_DISABLED_IP						"ERROR002"
#define HAN_IPCHECK_ERRSTR_NOT_REGISTERED_IP				"ERROR003"

HANIPCHECK_API int __stdcall HanIPCheckInit(IN const char* szGameId);

HANIPCHECK_API int __stdcall HanIPCheckRequest(IN OUT char* szRstStrBuff, IN int nRstStrBuffSize, IN const char* szIP);

HANIPCHECK_API int __stdcall HanIPCheckGetLastError();

HANIPCHECK_API const char* __stdcall HanIPCheckErrorString(IN int nErrorCode);

HANIPCHECK_API void __stdcall HanIPCheckTerm();

#endif // __HAN_IPCHECKFORSVR_H__