#ifndef __HanAuthForSvr__
#define __HanAuthForSvr__

#ifdef HANAUTHFORSVR_EXPORTS
#define HANAUTH_API __declspec(dllexport)
#else
	#define HANAUTH_API __declspec(dllimport)
	#ifdef _DEBUG
		#ifdef _WIN64
			#pragma comment(lib, "HanAuthForSvrD_x64.lib") 
		#else
			#pragma comment(lib, "HanAuthForSvrD.lib") 
		#endif
	#else
		#ifdef _WIN64
			#pragma comment(lib, "HanAuthForSvr_x64.lib") 
		#else
			#pragma comment(lib, "HanAuthForSvr.lib") 
		#endif
	#endif
#endif

//
// Service Codes
//
#define SERVICE_NATION  0x000000FF
#define SERVICE_KOR		0x00000001
#define SERVICE_USA		0x00000002
#define SERVICE_JPN		0x00000003
#define SERVICE_CHN		0x00000004

#define SERVICE_TYPE    0x00000F00
#define SERVICE_ALPHA	0x00000100
#define SERVICE_REAL	0x00000200
#define SERVICE_BETA	0x00000300

//
// Error Codes:
//   0 is OK, posivites are responses from server,
//   negatives are errors on local system.
//
#define HAN_AUTHSVR_OK						0
#define HAN_AUTHSVR_PARAM_INVALID			1	// parameter of auth is invalid.
#define HAN_AUTHSVR_IP_INVALID				2	// this ip address is invalid.
#define HAN_AUTHSVR_MEMBERID_INVALID		3	// memeberID is invalid.
#define HAN_AUTHSVR_PASSWORD_INCORRECT		4	// password incorrect
#define HAN_AUTHSVR_PASSWORD_MISMATCHOVER	5	// password mismatch ( over 3 times )
#define HAN_AUTHSVR_MEMBERID_NOTEXIST		6	// memberID is not HangameID
#define HAN_AUTHSVR_SYSTEM_ERROR			7	// system error
#define HAN_AUTHSVR_COOKIE_SETTINGERR		8	// cookie setting error
#define HAN_AUTHSVR_COOKIE_NOTEXIST			9	// cookie is not found
#define HAN_AUTHSVR_COOKIE_TIMEOUT		   10	// cookie timeout error

#define HAN_AUTHSVR_ARGUMENT_INVALID	   -1
#define HAN_AUTHSVR_INITED_ALREADY		   -2
#define HAN_AUTHSVR_INITED_NOT			   -3
#define HAN_AUTHSVR_INITED_FAIL			   -4
#define HAN_AUTHSVR_CONNPOOL_CREATEFAIL	   -5
#define HAN_AUTHSVR_GETCONNECTION_FAIL	   -6
#define HAN_AUTHSVR_OPENCONNECTION_FAIL	   -7
#define HAN_AUTHSVR_SENDCONNECTION_FAIL	   -8
#define HAN_AUTHSVR_RECVCONNECTION_FAIL	   -9
#define HAN_AUTHSVR_RETURNVALUE_INVALID	  -10
#define HAN_AUTHSVR_AUTHSTRING_INVALID	  -11
#define HAN_AUTHSVR_CONNPOOL_INVALID_SIZE -12
#define HAN_AUTHSVR_DLL_UNEXPECTED		 -100	// unknown error

//
// HanAuthGet*() Returns:
//
#define HAN_AUTHSVR_USER_UNKNOWN		   -1
#define HAN_AUTHSVR_USER_IS_MALE		    0
#define HAN_AUTHSVR_USER_IS_FEMALE		    1

//
// Length Limits for API Parameters:
//
#define LIMIT_GAMEID						20
#define LIMIT_MEMBERID						50
#define LIMIT_AUTHSTRING					(4 << 10)			// that's 4KB

#define SIZE_OUTBUFFER						512					// should provide at least this size for param szOutBuff.
#define SIZE_AUTHSTRING						LIMIT_AUTHSTRING	// deprecated

//
// Initial values
//
#define INVALID_SERVICE_CODE				0xFFFFFFFF
#define INVALID_CONN_COUNT					0

//
// API Declarations:
//

// Initialize
HANAUTH_API int __stdcall HanAuthInit(/*IN*/ char* szGameId,
									  /*IN*/ int nServiceCode = INVALID_SERVICE_CODE,
									  /*IN*/ unsigned int uConnCnt = INVALID_CONN_COUNT);

// Don't Check Client IP 
HANAUTH_API int __stdcall HanAuthForSvr(/*IN*/ char* szMemberId, 
										/*IN*/ char* szAuthString, 
										/*OUT*/ char* szOutBuff,
										/*IN*/ unsigned int size_buf,
										/*IN*/ char* szClientIP = NULL);

// Get User Age
//  return :
//				HAN_AUTHSVR_USER_UNKNOWN	: HanAuthForSvr return isn't HAN_AUTHSVR_OK
//				0<=							: user age
HANAUTH_API int __stdcall HanAuthGetUserAge(/*IN*/ char* szOutBuff);

// Get User Sex
// Use only Kor
//  return :
//				HAN_AUTHSVR_USER_UNKNOWN	: HanAuthForSvr return isn't HAN_AUTHSVR_OK
//				HAN_AUTHSVR_USER_IS_MALE	: user is male
//				HAN_AUTHSVR_USER_IS_FEMALE	: user is female
HANAUTH_API int __stdcall HanAuthGetUserSex(/*IN*/ char* szOutBuff);

// Get User Info
// ex.        0 | User ID | Age | Value1 | Value2
// nIndex     0    1         2      3         4
//  return :
//				HAN_AUTHSVR_USER_UNKNOWN	: HanAuthForSvr return isn't HAN_AUTHSVR_OK
//				HAN_AUTHSVR_OK				: return Value
HANAUTH_API int __stdcall HanAuthGetUserInfo(/*IN*/ char* szOutBuff,
											/*IN*/ unsigned int nIndex,
											/*OUT*/ char* szValue,
											/*IN*/ unsigned int size_value);



#endif // __HanAuthForSvr__
