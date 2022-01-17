/**
* @brief HanAuthForSvr.h
* This file declares Publishing Authentication Server Module api
* 
* Copyright (c) 2008 NHN Corporation. All Rights Reserved.
*
* @author		: nhn
* @date			: 2008-08-08
*/


#ifndef __HanAuthForSvr__
#define __HanAuthForSvr__

#if defined(_WIN32) || defined(_WIN64)
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
#else
	#define HANAUTH_API
#endif

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif

// pre define value
//////////////////////////////////////////////////////////////////////////
#define SERVICE_NATION			0x000000FF
#define SERVICE_KOR				0x00000001
#define SERVICE_USA				0x00000002
#define SERVICE_JPN				0x00000003
#define SERVICE_CHN				0x00000004

#define SERVICE_TYPE			0x00000F00
#define SERVICE_ALPHA			0x00000100
#define SERVICE_REAL			0x00000200
#define SERVICE_BETA			0x00000300

#define LIMIT_GAMEID			20
#define LIMIT_MEMBERID			50
#define LIMIT_AUTHSTRING		(4 << 10)			// that's 4KB

#define SIZE_OUTBUFFER			512					// should provide at least this size for param szOutBuff.
#define SIZE_AUTHSTRING			LIMIT_AUTHSTRING	// deprecated

#define INVALID_SERVICE_CODE	0xFFFFFFFF
#define INVALID_CONN_COUNT		0

//////////////////////////////////////////////////////////////////////////


// ASM api global return code
//////////////////////////////////////////////////////////////////////////
// Error Codes:
//   0 is OK, posivites are responses from server,
//   negatives are errors on local system.
//////////////////////////////////////////////////////////////////////////
#define HAN_AUTHSVR_OK						0	// OK
#define HAN_AUTHSVR_PARAM_INVALID			1	// transmission parameter error
#define HAN_AUTHSVR_IP_INVALID				2	// IP address error
#define HAN_AUTHSVR_MEMBERID_INVALID		3	// user ID error
#define HAN_AUTHSVR_PASSWORD_INCORRECT		4	// password error
#define HAN_AUTHSVR_PASSWORD_MISMATCHOVER	5	// maximum number of password errors exceeded
#define HAN_AUTHSVR_MEMBERID_NOTEXIST		6	// nonexistent user
#define HAN_AUTHSVR_SYSTEM_ERROR			7	// system error
#define HAN_AUTHSVR_COOKIE_SETTINGERR		8	// AuthString (cookie) setup error
#define HAN_AUTHSVR_COOKIE_NOTEXIST			9	// nonexistent AuthString (cookie)
#define HAN_AUTHSVR_COOKIE_TIMEOUT		   10	// valid time of AuthString (cookie) expired
#define HAN_AUTHSVR_NOT_PREMIUM_MEMBER	   100	// This user is not a premium member

#define HAN_AUTHSVR_ARGUMENT_INVALID	   -1	// transmission parameter error
#define HAN_AUTHSVR_INITED_ALREADY		   -2	// ASM already initialized
#define HAN_AUTHSVR_INITED_NOT			   -3	// ASM not initialized
#define HAN_AUTHSVR_INITED_FAIL			   -4	// ASM initialization failure
#define HAN_AUTHSVR_CONNPOOL_CREATEFAIL	   -5	// connection pool generation failure
#define HAN_AUTHSVR_GETCONNECTION_FAIL	   -6	// connection obtaining failure
#define HAN_AUTHSVR_OPENCONNECTION_FAIL	   -7	// connection opening failure
#define HAN_AUTHSVR_SENDCONNECTION_FAIL	   -8	// transmission failure
#define HAN_AUTHSVR_RECVCONNECTION_FAIL	   -9	// reception failure
#define HAN_AUTHSVR_RETURNVALUE_INVALID	  -10	// invalid return value
#define HAN_AUTHSVR_AUTHSTRING_INVALID	  -11	// invalid AuthString
#define HAN_AUTHSVR_CONNPOOL_INVALID_SIZE -12	// invalid connection pool size
#define HAN_AUTHSVR_DIFFERENT_NATION	  -13	// invalid nation for operating this function
#define HAN_AUTHSVR_DLL_UNEXPECTED		 -100	// unknown error

// HanAuthGet*() Returns:
#define HAN_AUTHSVR_USER_UNKNOWN		   -1	// unknown error
#define HAN_AUTHSVR_USER_IS_MALE		    0	// male user
#define HAN_AUTHSVR_USER_IS_FEMALE		    1	// female user

//////////////////////////////////////////////////////////////////////////


/**
* @fn		HanAuthInit()
* @brief	initialize module
* 
* @param	szGameId			Unique ID of a game
*			nServiceCode		Service code
*			uConnCnt			Number of connection pools linked to the authentication environment
*/
HANAUTH_API int __stdcall HanAuthInit(IN char* szGameId,
									  IN int nServiceCode = INVALID_SERVICE_CODE,
									  IN unsigned int uConnCnt = INVALID_CONN_COUNT);

/**
* @fn		HanAuthForSvr()
* @brief	Transmits AuthString and user ID to the Hangame Auth. Server to request a validation on authentication.
*			gets authentication results and short user information returned when authentication is completed.
* 
* @param	szMemberId			Hangame ID of user
*			szAuthString		AuthString delivered from the Game Client
*			szOutBuff			Buffer for authentication results to be returned
*			size_buf			Size of buffer for authentication results to be returned
*			szClientIP			Enters IP of the Game Client for the game that is required to check IP
*/
HANAUTH_API int __stdcall HanAuthForSvr(IN char* szMemberId, 
										IN char* szAuthString, 
										OUT char* szOutBuff,
										IN unsigned int size_buf,
										IN char* szClientIP = NULL);

/**
* @fn		HanAuthGetUserAge()
* @brief	A method that reads the age from the user information returned
*			from the "Hangame Auth. Server" along with authentication results
* 
* @param	szOutBuff			szOutBuff value received from the HanAuthForSvr() method, i.e., address of buffer that gets authentication results returned
* @return
*			HAN_AUTHSVR_USER_UNKNOWN	Return information format error
*			0							If the age is actually 0 or if the age information is a text, not a number
*			1<=							user age
*/
HANAUTH_API int __stdcall HanAuthGetUserAge(IN char* szOutBuff);

/**
* @fn		HanAuthGetUserSex()
* @brief	A method that reads the gender of users
*			from the user information returned from the "Hangame Auth. Server" along with authentication results
* 
* @param	szOutBuff			szOutBuff value received from the HanAuthForSvr() method, i.e., address of buffer that gets authentication results returned
* @return
*			HAN_AUTHSVR_USER_UNKNOWN	Return information format error
*			HAN_AUTHSVR_USER_IS_MALE	Male user
*			HAN_AUTHSVR_USER_IS_FEMALE	Female user
*/
HANAUTH_API int __stdcall HanAuthGetUserSex(IN char* szOutBuff);

/**
* @fn		HanAuthGetUserInfo()
* @brief	A method that reads any information from the user information returned
*			from the "Hangame Auth. Server" along with authentication results
* 
* @param	szOutBuff			szOutBuff value received from the HanAuthForSvr() method, i.e., address of buffer that gets authentication results returned
*			nIndex				Index of a desired value (e.g. If the return value is ¡®0|ID|AGE|KEY1|KEY2,¡¯ Index[0] is ¡®0¡¯, Index[1] ¡®ID¡¯ and Index[2] ¡®AGE.¡¯
*			szValue				Buffer for retrieved value to be saved
*			size_value			Size of buffer for retrieved value to be saved
* @return
*			HAN_AUTHSVR_USER_UNKNOWN	Return information format error
*			HAN_AUTHSVR_OK				Execution success
*/
HANAUTH_API int __stdcall HanAuthGetUserInfo(IN char* szOutBuff,
											IN unsigned int nIndex,
											OUT char* szValue,
											IN unsigned int size_value);

/**
* @fn		HanAuthGetPremiumAuthResult()
* @brief	This function verifies whether the user is a premium user
* 
* @param	szMemberID					Address of a string that contains the user id
*			nIdType						Type of user id. 0 is a normal user id and 1 is an ACC ID. ACC ID is used in Japan
* @return
*			HAN_AUTHSVR_OK				This user is a premium user
*			HAN_AUTHSVR_NOT_PREMIUM_MEMBER	This user is not a premium user
*/
HANAUTH_API int __stdcall HanAuthGetPremiumAuthResult(IN  char*  szMemberID,  IN  int  nIdType );


#endif // __HanAuthForSvr__
