#include "stdafx.h"
#include "AuThread.h"

AuThread::AuThread( UINT nStackSize /* = 0  */, UINT nPriority /* = THREAD_PRIORITY_NORMAL */ )
{
	m_hThread		=	NULL;
	m_nStackSize	=	nStackSize;
	m_nPriority		=	nPriority;
}

AuThread::~AuThread( VOID )
{
	Destroy();
}


DWORD WINAPI AuThread::_THREAD_ENTRY_POINT( IN VOID* pData )
{
	AuThread*		pThis	= static_cast< AuThread* >(pData);

	pThis->Run();

	pThis->Destroy();

	return 0;
}

BOOL	AuThread::Start()
{
	// 이미 한번 실행 한적이 있으면 그냥 끝낸다
	if( m_hThread )		return FALSE;

	
	// Thread 생성
	m_hThread		=	CreateThread( 0 , m_nStackSize , AuThread::_THREAD_ENTRY_POINT , 
										this , 0 , &m_dwThreadID );

	return TRUE;
}

VOID	AuThread::Destroy( VOID )
{
	if( m_hThread != NULL )	
	{
		::CloseHandle( m_hThread );
		m_hThread		=	NULL;
	}
}

BOOL	AuThread::WaitForTermination( IN DWORD dwMilliSeconds /* = INFINITE */ )
{
	if( !m_hThread )
		return FALSE;

	if( WAIT_OBJECT_0	!= ::WaitForSingleObject( m_hThread , dwMilliSeconds ) )
	{
		::TerminateThread( m_hThread , 0 );
	}

	return TRUE;
}

VOID	AuThread::SetPriority( IN UINT nPriority )
{

	if( m_hThread	== NULL )	return;

	m_nPriority		= nPriority;
	::SetThreadPriority( m_hThread , m_nPriority );

}

UINT	AuThread::GetPriority( VOID ) const
{
	if( m_hThread	== NULL )	return 0;

	return ::GetThreadPriority( m_hThread );	
}