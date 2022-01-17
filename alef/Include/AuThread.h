#pragma once

// Thread 생성 Class
// Thread를 만들 class에서 상속 받은 후에.. Run을 재정의 하면 된다
// 그 후 Start 함수를 호출하면 끝
// MK	2008.8.14

#include <Windows.h>

class AuThread
{
public:
	AuThread( UINT nStackSize = 0 , UINT nPriority = THREAD_PRIORITY_NORMAL );
	AuThread( AuThread& rhs );			//	복사생성자는 선언만 해놓고 구현을 하지 않는다(복사생성자못쓰게)
	virtual ~AuThread( VOID );

	// Thread 시작
	BOOL				Start( VOID );		//

	VOID				Destroy( VOID );	//

	// default 값이 무한정 기다리기
	// 기다렸다가 Thread 죽이기
	BOOL				WaitForTermination( IN DWORD dwMilliSeconds = INFINITE );	//

	// Thread 우선 순위 변경 및 확인
	VOID				SetPriority( IN UINT nPriority	);							//
	UINT				GetPriority( VOID				)	const;					//


	// 이 Class를 상속 받는 모든 Class는 Run을 재정의 해야한다
	virtual VOID		Run( VOID )			= 0;

private:
	static DWORD WINAPI _THREAD_ENTRY_POINT( IN VOID* pData );						//

protected:
	DWORD				m_dwThreadID;			//	Thread ID
	HANDLE				m_hThread;				//	Thread Handle
	UINT				m_nStackSize;			//	Thread Stack Size
	UINT				m_nPriority;			//	Thread Priority

};

