#include "PatchServerOverlapped.h"

void CAcceptOverlappedEx::init()
{
	recycleOverlapped();

	m_pstrAddrInfo = new char[256]; //256바이트면 주소저장하는데는 충분하겠지? 호홋~

	m_hSocket = 0;

	//Overlapping관련 Event생성.
	m_cOverlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
}

void CAcceptOverlappedEx::recycleOverlapped()
{
	m_cOverlapped.Internal = 0;
	m_cOverlapped.InternalHigh = 0;
	m_cOverlapped.Offset = 0;
	m_cOverlapped.OffsetHigh = 0;
}

void CAcceptOverlappedEx::cleanup()
{
	delete [] m_pstrAddrInfo;
	CloseHandle( m_cOverlapped.hEvent );
}

bool COverlappedEx::init( bool bEvent, int iOperation )
{
	m_iOperation = iOperation;

	if( bEvent == true )
	{
		m_cOverlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	}
	else
	{
		m_cOverlapped.hEvent = 0;
	}

	recycleOverlapped();

	return true;
}

bool COverlappedEx::setOperation( int iOperation )
{
	m_iOperation = iOperation;

	return true;
}

bool COverlappedEx::cleanup()
{
	if( m_cOverlapped.hEvent != 0 )
		CloseHandle( m_cOverlapped.hEvent );

	return true;
}

void COverlappedEx::recycleOverlapped()
{
	m_cOverlapped.Internal = 0;
	m_cOverlapped.InternalHigh = 0;
	m_cOverlapped.Offset = 0;
	m_cOverlapped.OffsetHigh = 0;
}

COverlappedRecv::COverlappedRecv()
{
	init( false, IOCP_OVERLAP_RECV, 0, 0 );
}

COverlappedRecv::COverlappedRecv( bool bEvent, int iWSABufSize , int iInternalBufferSize )
{
	init( bEvent, IOCP_OVERLAP_RECV, iWSABufSize, iInternalBufferSize );
}

COverlappedRecv::~COverlappedRecv()
{
	cleanup();
}

bool COverlappedRecv::init( bool bEvent, int iOperation, int iWSABufSize , int iInternalBufferSize  )
{
	((COverlappedEx *)(this))->init( bEvent, iOperation );

	if( iWSABufSize != 0 )
	{
		m_cWSABuf.buf = new char[iWSABufSize];
		m_cWSABuf.len = iWSABufSize;
	}
	else //if( iWSABufSize == 0 )
	{
		m_cWSABuf.buf = NULL;
		m_cWSABuf.len = 0;
	}

	if( iInternalBufferSize != 0 )
	{
		m_pstrBuffer = new char[iInternalBufferSize];
		m_iRemainPacketLength = 0;
	}
	else //if( iWSABufSize == 0 )
	{
		m_pstrBuffer = NULL;
		m_iRemainPacketLength = 0;
	}

	return true;
}

bool COverlappedRecv::cleanup()
{
	((COverlappedEx *)(this))->cleanup();

	if( m_cWSABuf.buf != NULL )
	{
		delete [] m_cWSABuf.buf;
	}

	if( m_pstrBuffer != NULL )
	{
		delete [] m_pstrBuffer;
	}

	return true;
}

COverlappedSend::COverlappedSend()
{
	init( false, IOCP_OVERLAP_SEND, 0 );
}

COverlappedSend::COverlappedSend( bool bEvent, int iWSABufSize )
{
	init( bEvent, IOCP_OVERLAP_SEND, iWSABufSize );
}

COverlappedSend::~COverlappedSend()
{
	cleanup();
}

bool COverlappedSend::init( bool bEvent, int iOperation, int iWSABufSize  )
{
	((COverlappedEx *)(this))->init( bEvent, iOperation );

	if( iWSABufSize != 0 )
	{
		m_cWSABuf.buf = new char[iWSABufSize];
		m_cWSABuf.len = iWSABufSize;
	}
	else //if( iWSABufSize == 0 )
	{
		m_cWSABuf.buf = NULL;
		m_cWSABuf.len = 0;
	}

	return true;
}

bool COverlappedSend::cleanup()
{
	((COverlappedEx *)(this))->cleanup();

	if( m_cWSABuf.buf != NULL )
	{
		delete [] m_cWSABuf.buf;
		m_cWSABuf.len = 0;
	}

	return true;
}

COverlappedSendPatchFile::COverlappedSendPatchFile()
{
	init( false, IOCP_OVERLAP_PATCH_FILE, 0 );
}

COverlappedSendPatchFile::COverlappedSendPatchFile( bool bEvent, int iWSABufSize )
{
	init( bEvent, IOCP_OVERLAP_PATCH_FILE, iWSABufSize );
}

COverlappedSendPatchFile::~COverlappedSendPatchFile()
{
	cleanup();
}

bool COverlappedSendPatchFile::init( bool bEvent, int iOperation, int iWSABufSize  )
{
	((COverlappedEx *)(this))->init( bEvent, iOperation );

	if( iWSABufSize != 0 )
	{
		m_cWSABuf.buf = new char[iWSABufSize];
		m_cWSABuf.len = iWSABufSize;
	}
	else //if( iWSABufSize == 0 )
	{
		m_cWSABuf.buf = NULL;
		m_cWSABuf.len = 0;
	}

	return true;
}

bool COverlappedSendPatchFile::cleanup()
{
	((COverlappedEx *)(this))->cleanup();

	if( m_cWSABuf.buf != NULL )
	{
		delete [] m_cWSABuf.buf;
		m_cWSABuf.len = 0;
	}

	return true;
}

