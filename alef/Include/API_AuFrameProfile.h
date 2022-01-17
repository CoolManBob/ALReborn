#pragma once

#include <windows.h>
#include <TCHAR.h>

#ifdef ALEF_DLLEXPORT
#define ALEF_DLL __declspec(dllexport)
#elif ALEF_DLLIMPORT
#define ALEF_DLL __declspec(dllimport)
#else
#error ALEF_DLL define error
#endif

// FrameCallstack은 스레드별로 생성되며 각 스레드가 실행되면서 거쳐간 함수의 호출정보를
// 누적한다. 이때 자동적으로 수집되는 정보는 함수의 시작부분과 종료부분이며
// 함수의 중간부분은 별도의 코드를 삽입하여 정보를 수집한다.
// 현재 총 100만번의 함수 호출 정보를 누적할수 있으며 이는 시작과 종료 정보를 포함한
// 수치이기 때문에 산술적으로 총 50만번의 함수를 호출할수 있다.

// FrameCallStack을 초기화 한다. 
// Callstack정보를 모으기 원하는 작업의 시작지점에서 초기화를 진행한다.
// 게임루프의 경우 한번의 게임루프 후에는 반드시 초기화를 해주어야 해당 게임루프의 Callstack정보를 
// 모을수 있다. 만약 초기화를 해주지 않으면 Callstack 정보를 계속 누적시킨다.
ALEF_DLL void FrameProfile_Init();

// FrameCallStack 기능의 사용여부를 결정한다. TRUE면 CallStack정보를 수집하기 시작하고 FALSE이면 무시한다.
// 만약 FrameCallStack을 계속 사용한다면 해당 스레드에서 단 한번만 호출하면 된다.
// 원하는 구간의 CallStack 정보를 모으고 싶을때에도 사용 가능하다.
ALEF_DLL void FrameProfile_SetEnable( BOOL bEnable );

ALEF_DLL void FrameProfile_SetThreadName( LPTSTR pThreadName );

// SEH를 이용하여 예외 처리를 할때 사용된다. 해당 에러코드와 GetExceptionInformation을 이용한
// 예외 발생 주소정보를 저장한다. 
// 주의 : GetExceptionInformation과 GetExceptionCode는 예외 필터에서만 사용 가능하다
ALEF_DLL void FrameProfile_AddSEHInfo( DWORD dwErrorCode, DWORD_PTR dwExceptionAddress );

// 누적된 FrameCallstack 정보를 파일에 저장한다. 파일에 저장할때의 이름 규칙은 다음과 같다.
// 일반적인 경우			: FrameCallstack [ThreadID] [Count]
// Exception이 발생한 경우	: Exception_FrameCallStack [ThreadID] [Count]
ALEF_DLL void FrameProfile_WriteFile();

ALEF_DLL void FrameProfile_WriteFileAll();

// WriteFrameCallStackLog를 이용하여 작성된 파일을 읽어 들인다.
// 이때 Callstack 정보와 ExceptionCode 정보를 읽어온다.
ALEF_DLL void FrameProfile_ReadFromFile( LPCTSTR pFileName );

// FrameCallstack이 가지고 있는 정보를 콘솔 화면으로 출력한다.
// 현재 수집된 정보를 이용하든 ReadFrameCallStackLog를 이용하든 사용가능
ALEF_DLL void FrameProfile_DisplayToConsole();

// AddSEHInfo를 쉽게 쓰기 위해서 제공하는 함수
// 사용예)
// __except( ExceptionFilterFrameCallstack ( GetExceptionCode(), GetExceptionInformation() )
ALEF_DLL int FrameProfile_ExceptionFilter( DWORD dwErrorCode, LPEXCEPTION_POINTERS pException );

// ManualProfile은 FrameCallstack의 함수의 시작과 종료 지점의 정보만을 남길수 있는 단점을
// 보완하는 방법으로 함수의 중간중간에 프로그래머가 원하는 위치에 명시적으로 로그를 넣을수 있다.
// 하지만 naked 함수의 특성상 함수의 호출을 다음과 같은 함수를 만들어 여기서 리턴되는
// FARPROC(ManualProfile 함수 포인터)를 이용하여 함수를 호출한다.
//
// FARPROC GetManualProfile()
// {
//		return ::GetProcAddress( ::GetModuleHandle( _T("AuFrameProfile") ), _T("ManualProfile") );
// }
//
//	ex)
// FARPROC fpManual = GetManualProfile();
// fpManual();	// 호출 완료
//