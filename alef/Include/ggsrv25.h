#ifndef _GG_AUTH_SERVER_H_
#define _GG_AUTH_SERVER_H_

#ifdef _AREA_JAPAN_

#ifdef _EXPORT_DLL
	#define GGAUTHS_API    extern "C" __declspec(dllexport)
	#define GGAUTHS_EXPORT __declspec(dllexport)
	#define __CDECL        __cdecl
#else
  	#define GGAUTHS_API extern "C"
	//#define GGAUTHS_API  <== ggsrv_cpp_c.h 에서 활성화. extern "C" 는 c 컴파일러가 인식못함.
	#define GGAUTHS_EXPORT 
	#define __CDECL 
#endif

#ifndef ERROR_SUCCESS
	#define ERROR_SUCCESS							0 //일반적인 성공
#endif
#define	NPGG_INFO_SUCCESS						0 //GetInfo() func success.
#define	NPGG_INFO_ERROR_NOTENOUGHFMEMORY		1 //Need more memory
#define ERROR_GGAUTH_FAIL_MEM_ALLOC				1 //CS인증 모듈 로딩시 메모리 부족
#define ERROR_GGAUTH_FAIL_LOAD_DLL				2 //CS인증 모듈(*.dll, *.so) 로딩 실패
#define ERROR_GGAUTH_FAIL_GET_PROC				3 //CS인증 모듈의 export 함수 설정 실패
#define ERROR_GGAUTH_FAIL_BEFORE_INIT			4 //비정상 동작 <- critical error
#define	ERROR_GGAUTH_FAIL_LOAD_CFG				5 //서버인증 모듈 설정 파일 로딩 실패.
#define ERROR_GGAUTH_INVALID_PARAM				10 // 함수 호출시 잘못된 인자값을 넘겼을 경우
#define ERROR_GGAUTH_NO_REPLY					11 // 응답없음. GetAuthQuery() 가 CheckAuthAnswer()하기전에 두번 불려지면 같은에러 발생
#define ERROR_GGAUTH_INVALID_PROTOCOL_VERSION	12 // CS인증 모듈 프로토콜 번호(ggauth##.dll) 틀림
#define ERROR_GGAUTH_INVALID_REPLY				13 // 응답값 틀림.
#define	ERROR_GGAUTH_INVALID_GAMEGUARD_VER		14 //게임가드 버젼검사. 라이브러리 2.5에서 추가됨(cs인증모듈 50번 이상)
#define ERROR_GGAUTH_SETSTATE_ERROR				20 // SetCSAuthState() 호출 시 m_CSAuthState가 NULL이거나..
													// m_PrtcTemp가 NULL 인 경우 (pProtocolHead 를 찾을 수 없는 경우...)
#define ERROR_GGAUTH_INVALID_GAMEMON_VER		101 //
#define ERROR_GGAUTH_INVALID_GAMEMON_VER_CODE	102 //

#define ERROR_GGAUTH_RETRY_QUERY   200

#define NPGG_CHECKUPDATED_VERIFIED			0 //csa.CheckUpdated() 함수 리턴값. 현재 버전 사용자
#define NPGG_CHECKUPDATED_NOTREADY			1 //csa.CheckUpdated() 함수 리턴값. 버전검사를 하기 전. 버전에 대한 정보가 없음.
#define NPGG_CHECKUPDATED_HIGH				2 //csa.CheckUpdated() 함수 리턴값. 현재 버전보다 높은 사용자
#define NPGG_CHECKUPDATED_LOW				3 //csa.CheckUpdated() 함수 리턴값. 현재 버전보다 낮은 사용자

#define	NPLOG_DEBUG	0x00000001	// 디버깅 메세지 
#define	NPLOG_ERROR	0x00000002  // 에러 메세지 

#define NPLOG_ENABLE_DEBUG 0x00000001 // 디버깅 메세지 생성함
#define NPLOG_ENABLE_ERROR 0x00000002 // 에러 메세지 생성함

// << DATA structure >>
// gameguard auth data
typedef struct _GG_AUTH_DATA
{
	unsigned long dwIndex;
	unsigned long dwValue1;
	unsigned long dwValue2;
	unsigned long dwValue3;
} GG_AUTH_DATA, *PGG_AUTH_DATA;

// gameguard version data
typedef struct _GG_VERSION
{
	unsigned long	dwGGVer;
	unsigned short	wYear;
	unsigned short	wMonth;
	unsigned short	wDay;
	unsigned short	wNum;
} GG_VERSION, *PGG_VERSION;

// Data for Server <-> Server (프로토콜 버전, 게임가드 내부버전, Flag)
typedef struct _GG_CSAUTH_STATE
{
	unsigned long	m_PrtcVersion;
	unsigned long	m_GGVersion;
	unsigned long	m_UserFlag;
} GG_CSAUTH_STATE, *PGG_CSAUTH_STATE;

typedef struct _GG_AUTH_PROTOCOL *PGG_AUTH_PROTOCOL;

GGAUTHS_API unsigned long __CDECL InitGameguardAuth(char* sGGPath, unsigned long dwNumActive, int useTimer, int useLog);
GGAUTHS_API void  __CDECL CleanupGameguardAuth();
GGAUTHS_API unsigned long __CDECL GGAuthUpdateTimer(); //useTimer 가 true 일 경우 호출.
GGAUTHS_API unsigned long __CDECL AddAuthProtocol(char* sDllName);
GGAUTHS_API unsigned long __CDECL SetGGVerLimit(unsigned long nLimitVer);	//버전 상한선을 제한한다.
GGAUTHS_API unsigned long __CDECL SetUpdateCondition(int nTimeLimit, int nCondition); //업데이트 조건을 설정한다.
GGAUTHS_API int ModuleInfo(char* dest, int length);
GGAUTHS_API void NpLog(int mode, char* msg); // referenced by 
/*
{
	if(mode & (NPLOG_DEBUG | NPLOG_ERROR)) // for examples 
#ifdef WIN32
		OutputDebugString(msg);
#else
		printf(msg); 
#endif
};
*/

typedef struct _GG_UPREPORT
{
	unsigned long	dwBefore;   // Before version
	unsigned long	dwNext;		// Now version
	int		nType;		// Update type 1 : gameguard version, 2 : protocol num
} GG_UPREPORT, *PGG_UPREPORT;
GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report); // referenced by 
/*
{
	printf("GGAuth version update [%s] : [%ld] -> [%ld] \n", // for examples
			report->nType==1?"GameGuard Ver":"Protocol Num", 
			report->dwBefore,
			report->dwNext); 
};
*/

// Class CSAuth2
class GGAUTHS_EXPORT CCSAuth2
{
public:
	// Constructor
	CCSAuth2();

	// Destructor
	~CCSAuth2();

protected:
	PGG_AUTH_PROTOCOL m_pProtocol;
	unsigned long m_bPrtcRef;
	unsigned long m_dwUserFlag;
	GG_VERSION m_GGVer;	//게임가드 버젼
	GG_AUTH_DATA m_AuthQueryTmp;	//m_AuthQuery 값을 백업해둔다.

//	bool m_bNewProtocol; // 새로운 프로토콜로 접속하면 true. 새로운 프로토콜이 할성화되면 false, 디폴트 false;
//	bool m_bNewGGVer; // 새로운 게임가드 버전이 접속하면 true. 새로운 프로토콜이 할성화되면 false, 디폴트 false;

//	int m_bActive; //현재 객체가 사용자에게 할당되었는지. Init 호출후 true, Close 호출후 false

public:	
	GG_AUTH_DATA m_AuthQuery;
	GG_AUTH_DATA m_AuthAnswer;

	void  Init();
	unsigned long GetAuthQuery();
	unsigned long CheckAuthAnswer();
	inline void InitCSAuthState(PGG_CSAUTH_STATE m_CSAuthState) { memset(m_CSAuthState, 0, sizeof(GG_CSAUTH_STATE)); };
	unsigned long GetCSAuthState(PGG_CSAUTH_STATE m_CSAuthState);
	unsigned long SetCSAuthState(PGG_CSAUTH_STATE m_CSAuthState);
	unsigned long SetSecretOrder();
	void  Close();
	int	  Info(char* dest, int length); // protocol information
	int	  CheckUpdated(); //현재 사용자의 업데이트 여부를 확인한다.	
};

// C type CSAuth2
typedef void*          LPGGAUTH;

GGAUTHS_API LPGGAUTH __CDECL GGAuthCreateUser();                                              // CCSAuth2()
GGAUTHS_API unsigned long     __CDECL GGAuthDeleteUser(LPGGAUTH pGGAuth);                             // ~CCSAuth2()
GGAUTHS_API unsigned long	 __CDECL GGAuthInitUser(LPGGAUTH pGGAuth);                              // Init()
GGAUTHS_API unsigned long     __CDECL GGAuthCloseUser(LPGGAUTH pGGAuth);                             // Close()
GGAUTHS_API unsigned long    __CDECL GGAuthGetQuery(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);     // GetAuthQuery()
GGAUTHS_API unsigned long    __CDECL GGAuthCheckAnswer(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);  // CheckAuthAnswer()
GGAUTHS_API int      __CDECL GGAuthCheckUpdated(LPGGAUTH pGGAuth);  // CheckAuthAnswer()
GGAUTHS_API int      __CDECL GGAuthUserInfo(LPGGAUTH pGGAuth, char* dest, int length);  // CheckAuthAnswer()
GGAUTHS_API unsigned long	__CDECL GGAuthGetState(LPGGAUTH pGGAuth, PGG_CSAUTH_STATE pAuthState);	// GetCSAuthState()
GGAUTHS_API unsigned long	__CDECL	GGAuthSetState(LPGGAUTH pGGAuth, PGG_CSAUTH_STATE pAuthState);	// SetCSAuthState()
GGAUTHS_API unsigned long	__CDECL	GGAuthSetSecureOrder(LPGGAUTH pGGAuth);	// SetSecureOrder()

#define	NPGG_USER_AUTH_QUERY	0x00000001 //GGAuthGetUserValue() 의 전달 플래그 Query
#define	NPGG_USER_AUTH_ANSWER	0x00000002 //GGAuthGetUserValue() 의 전달 플래그 Answer
#define NPGG_USER_AUTH_INDEX	0x00000010 //GGAuthGetUserValue() 의 전달 플래그 index
#define NPGG_USER_AUTH_VALUE1	0x00000020 //GGAuthGetUserValue() 의 전달 플래그 Value1
#define NPGG_USER_AUTH_VALUE2	0x00000040 //GGAuthGetUserValue() 의 전달 플래그 Value2
#define NPGG_USER_AUTH_VALUE3	0x00000080 //GGAuthGetUserValue() 의 전달 플래그 Value3
GGAUTHS_API unsigned long	 __CDECL GGAuthGetUserValue(LPGGAUTH pGGAuth, int type);

#endif //_AREA_JAPAN_

#endif

#ifdef _AREA_JAPAN_