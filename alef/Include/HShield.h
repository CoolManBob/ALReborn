/*++ BUILD Version: 3,3,11,1  

Copyright (c) 2003-2005  AhnLab, Inc.

Module Name:

    HShield.h

Abstract:

    This header file defines the data types and constants, and exposes APIs 
	that are defined by HackShield Library.

Tag Information:
	
	This header file is auto-generated at 2005-03-26 14:10:41.

--*/
           
#ifndef _HSHIELD_H_INC
#define _HSHIELD_H_INC

// HShield Library Version Info
#define	HSVERSION(w,x,y,z)	(w) <<24|(x&0xFF)<<16|(y&0xFF)<<8|(z&0xFF)

#define HSHIELD_PACKAGE_VER	HSVERSION(3,3,11,1)
#define HSHIELD_PACKAGE_CRC	_T("lRkEssr9QE6fJgOfAVgcWA==")


// Init Option
#define AHNHS_CHKOPT_SPEEDHACK					0x2
#define AHNHS_CHKOPT_READWRITEPROCESSMEMORY 	0x4

#define AHNHS_CHKOPT_KDTARCER					0x8
#define AHNHS_CHKOPT_OPENPROCESS				0x10
#define AHNHS_CHKOPT_AUTOMOUSE					0x20
#define AHNHS_CHKOPT_MESSAGEHOOK                0x40

#define AHNHS_CHKOPT_PROCESSSCAN                0x80

#define AHNHS_CHKOPT_ALL AHNHS_CHKOPT_SPEEDHACK \
						| AHNHS_CHKOPT_READWRITEPROCESSMEMORY \
						| AHNHS_CHKOPT_KDTARCER \
						| AHNHS_CHKOPT_OPENPROCESS \
						| AHNHS_CHKOPT_AUTOMOUSE \
						| AHNHS_CHKOPT_MESSAGEHOOK \
						| AHNHS_CHKOPT_PROCESSSCAN \
						| AHNHS_CHKOPT_HSMODULE_CHANGE \
						| AHNHS_USE_LOG_FILE

#define AHNHS_USE_LOG_FILE                      0x100
#define AHNHS_CHECK_UPDATE_STATE                0x200
#define AHNHS_ALLOW_SVCHOST_OPENPROCESS         0x400
#define AHNHS_ALLOW_LSASS_OPENPROCESS           0x800
#define AHNHS_ALLOW_CSRSS_OPENPROCESS           0x1000
#define AHNHS_DONOT_TERMINATE_PROCESS           0x2000
#define AHNHS_DISPLAY_HACKSHIELD_LOGO           0x4000
#define AHNHS_CHKOPT_HSMODULE_CHANGE			0x8000


// SpeedHack Sensing Ratio
#define	AHNHS_SPEEDHACK_SENSING_RATIO_HIGHEST	0x1
#define	AHNHS_SPEEDHACK_SENSING_RATIO_HIGH		0x2
#define	AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL	0x4
#define	AHNHS_SPEEDHACK_SENSING_RATIO_LOW		0x8
#define	AHNHS_SPEEDHACK_SENSING_RATIO_LOWEST	0x10
#define	AHNHS_SPEEDHACK_SENSING_RATIO_GAME		0x20


typedef int (__stdcall* PFN_AhnHS_Callback)(
	long lCode,
	long lParamSize,
	void* pParam 
);


#if defined(__cplusplus)
extern "C"
{
#endif

int __stdcall _AhnHS_Initialize(
	const char* szFileName,
	PFN_AhnHS_Callback pfn_Callback, //Callback Function Pointer
	int nGameCode,                   //Game Code
	const char* szLicenseKey,        //License Code
	DWORD dwOption,                  //Init Option
	UINT unSHackSensingRatio		 //SpeedHack Sensing Ratio
);


int __stdcall _AhnHS_StartService();

int __stdcall _AhnHS_StopService();

int __stdcall _AhnHS_PauseService( DWORD dwPauseOption );

int __stdcall _AhnHS_ResumeService( DWORD dwResumeOption );

int __stdcall _AhnHS_Uninitialize();

/*
// 3.2.0.1 멀티프로세스 지원되면서 삭제된 API
BOOL __stdcall _AhnHS_CreateProcess( LPCSTR lpApplicationName,
								    LPSTR lpCommandLine,
									LPSECURITY_ATTRIBUTES lpProcessAttributes,
									LPSECURITY_ATTRIBUTES lpThreadAttributes,
									BOOL bInheritHandles,
									DWORD dwCreationFlags,
									LPVOID lpEnvironment,
									LPCSTR lpCurrentDirectory,
									LPSTARTUPINFOA lpStartupInfo,
									LPPROCESS_INFORMATION lpProcessInformation );
*/


#if defined(__cplusplus)
}
#endif


//ERROR CODE
#define HS_ERR_OK                               0x000				// 함수 호출 성공
#define HS_ERR_UNKNOWN                          0x001				// 알 수 없는 오류가 발생했습니다.
#define HS_ERR_INVALID_PARAM                    0x002				// 올바르지 않은 인자입니다.
#define HS_ERR_NOT_INITIALIZED                  0x003				// 핵쉴드 모듈이 초기화되지 않은 상태입니다.
#define HS_ERR_COMPATIBILITY_MODE_RUNNING       0x004				// 현재 프로세스가 호환성 모드로 실행되었습니다.

#define HS_ERR_INVALID_LICENSE                  0x100				// 올바르지 않은 라이센스 키입니다.
#define HS_ERR_INVALID_FILES                    0x101				// 잘못된 파일 설치되었습니다. 프로그램을 재설치하시기 바랍니다.
#define HS_ERR_INIT_DRV_FAILED                  0x102
#define HS_ERR_ANOTHER_SERVICE_RUNNING          0x103				// 다른 게임이나 프로세스에서 핵쉴드를 이미 사용하고 있습니다.
#define HS_ERR_ALREADY_INITIALIZED              0x104				// 이미 핵쉴드 모듈이 초기화되어 있습니다.
#define HS_ERR_SOFTICE_DETECT                   0x105				// 컴퓨터에서 SoftICE 실행이 감지되었습니다. SoftICE 실행을 중지시킨 뒤에 다시 실행시켜주시기바랍니다.
#define HS_ERR_EXECUTABLE_FILE_CRACKED			0x106				// 실행 파일의 코드가 크랙 되었습니다.
#define HS_ERR_NEED_ADMIN_RIGHTS				0x107				// ADMIN 권한이 필요합니다.

#define HS_ERR_START_ENGINE_FAILED              0x200				// 해킹 프로그램 감지 엔진을 시작할 수 없습니다.
#define HS_ERR_ALREADY_SERVICE_RUNNING          0x201				// 이미 핵쉴드 서비스가 실행 중입니다.
#define HS_ERR_DRV_FILE_CREATE_FAILED           0x202				// 핵쉴드 드라이버 파일을 생성할 수 없습니다.
#define HS_ERR_REG_DRV_FILE_FAILED              0x203				// 핵쉴드 드라이버를 등록할 수 없습니다.
#define HS_ERR_START_DRV_FAILED                 0x204				// 핵쉴드 드라이버를 시작할 수 없습니다.

#define HS_ERR_SERVICE_NOT_RUNNING              0x301				// 핵쉴드 서비스가 실행되고 있지 않은 상태입니다.
#define HS_ERR_SERVICE_STILL_RUNNING            0x302				// 핵쉴드 서비스가 아직 실행중인 상태입니다.

#define HS_ERR_NEED_UPDATE                      0x401				// 핵쉴드 모듈의 업데이트가 필요합니다.

//CallBack Code
#define AHNHS_ACTAPC_DETECT_ALREADYHOOKED		0x010101			// 일부 API가 이미 후킹되어 있는 상태입니다. (그러나 실제로는 이를 차단하고 있기 때문에 후킹프로그램은 동작하지 않습니다.)
#define AHNHS_ACTAPC_DETECT_AUTOMOUSE			0x010102			// 오토마우스 행위가 감지되었습니다.	
#define AHNHS_ACTAPC_DETECT_HOOKFUNCTION		0x010301			// 보호 API에 대한 후킹 행위가 감지되었습니다.
#define AHNHS_ACTAPC_DETECT_DRIVERFAILED		0x010302			// 해킹 차단 드라이버가 로드되지 않았습니다.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK			0x010303			// 스피드핵류의 프로그램에 의해 시스템 시간이 변경되었습니다.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK_APP		0x010304			// 스피드핵류의 프로그램에 의해 시스템 시간이 변경되었습니다.
#define AHNHS_ACTAPC_DETECT_MESSAGEHOOK			0x010306			// 메시지 후킹이 시도되었으며 이를 차단하지 못했습니다.
#define AHNHS_ACTAPC_DETECT_KDTRACE				0x010307			// 디버거 트래이싱이 발생했다.(커널 디버거 활성화, 이후 브레이크 포인터 처리)
#define AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED		0x010308			// 설치된 디버거 트래이싱이 변경되었다.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK_RATIO		0x01030B			// 스피드핵 감지 옵션이 'GAME'이 경우 이 콜백으로 최근 5초동안의 시간정보가 전달됩니다.
#define AHNHS_ENGINE_DETECT_GAME_HACK           0x010501			// 게임 해킹툴의 실행이 발견되었습니다.
#define AHNHS_ENGINE_DETECT_GENERAL_HACK        0x010502			// 일반 해킹툴(트로이목마 종류)이 발견되었습니다.
#define AHNHS_ACTAPC_DETECT_MODULE_CHANGE       0x010701			// 핵쉴드 관련모듈이 변경되었습니다.

#pragma pack(1)

typedef struct _ACTAPCPARAM_DETECT_HOOKFUNCTION
{	
	char szFunctionName[128];		// file path
	char szModuleName[128];
	
} ACTAPCPARAM_DETECT_HOOKFUNCTION, *PACTAPCPARAM_DETECT_HOOKFUNCTION;


// 오토마우스 관련 APC Structure
typedef struct
{
	BYTE	byDetectType;			// AutoMouse 관련 API 호출 1, AutoMouse 관련 API 변조 2
	DWORD	dwPID;					// AutoMouse 프로세스 또는 API 가 변조된 프로세스
	CHAR	szProcessName[16+1];	// 프로세스명
	CHAR	szAPIName[128];			// 호출된 API 명 또는 변조된 API 명
}ACTAPCPARAM_DETECT_AUTOMOUSE, *PACTAPCPARAM_DETECT_AUTOMOUSE;


#define	EAGLE_AUTOMOUSE_APCTYPE_API_CALLED			1
#define	EAGLE_AUTOMOUSE_APCTYPE_API_ALTERATION		2
#define	EAGLE_AUTOMOUSE_APCTYPE_SHAREDMEMORY_ALTERATION	3

#pragma pack()


#endif _HSHIELD_H_INC