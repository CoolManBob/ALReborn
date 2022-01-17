#include "AcuFrameMemory.h"

INT32	AcuFrameMemory::m_iByteAllocated = 0;
UINT8*	AcuFrameMemory::m_pFrame = NULL;

//#ifndef _DEBUG
//UINT8*	AcuFrameMemory::m_pMemoryBlock = NULL;
//#else
//UINT8	AcuFrameMemory::m_pMemoryBlock[DEBUG_FRAME_MEMORY_SIZE];
//#endif

//. 2006. 6. 26. Nonstopdj
//. Heap을 이용하지 않는다.
UINT8	AcuFrameMemory::m_pMemoryBlock[DEBUG_FRAME_MEMORY_SIZE];
UINT32	AcuFrameMemory::m_iCurAllocated = 0;

#ifdef _DEBUG
UINT32	g_uMaxUsageBuffer = 0;
#endif // _DEBUG

static	INT32	g_iMainThreadID = 0;

AcuFrameMemory::AcuFrameMemory()
{}

AcuFrameMemory::~AcuFrameMemory()
{}

BOOL	AcuFrameMemory::Create()
{
	m_iByteAllocated = 0;
	m_pFrame = NULL;

//#ifndef _DEBUG
//	m_pMemoryBlock = NULL;
//#endif

	g_iMainThreadID = GetCurrentThreadId();

	return TRUE;
}

void	AcuFrameMemory::Release()
{
//#ifndef _DEBUG
//	delete []m_pMemoryBlock; 
//	m_pMemoryBlock = m_pFrame = NULL;
//#else
//	m_pFrame = NULL;
//#endif

	m_pFrame = NULL;
	m_iByteAllocated = 0;
	m_iCurAllocated = 0;
}

void	AcuFrameMemory::Clear()
{
//#ifndef _DEBUG
//	m_pFrame = m_pMemoryBlock;					// 시작 위치로
//#else
//	m_pFrame = &m_pMemoryBlock[0];
//#endif
	m_pFrame = &m_pMemoryBlock[0];
	m_iCurAllocated = 0;
}

BOOL	AcuFrameMemory::InitManager(INT32	nSize)
{
//#ifndef _DEBUG
//	m_pMemoryBlock	= new UINT8[nSize];
//	m_pFrame = m_pMemoryBlock;
//	m_iByteAllocated = nSize;
//#else
//	memset(m_pMemoryBlock, 0, sizeof(m_pMemoryBlock));
//	m_pFrame = &m_pMemoryBlock[0];
//	m_iByteAllocated = DEBUG_FRAME_MEMORY_SIZE;
//#endif

	memset(m_pMemoryBlock, 0, sizeof(m_pMemoryBlock));
	m_pFrame = &m_pMemoryBlock[0];
	m_iByteAllocated = DEBUG_FRAME_MEMORY_SIZE;

	return TRUE;
}

void*	AcuFrameMemory::AllocFrameMemory(INT32 nBytes)
{
	#ifdef _DEBUG
	if( g_iMainThreadID != GetCurrentThreadId())
		MD_SetErrorMessage( "AcuFrameMemory::AllocFrameMemory, called by other thread." );
	#endif

	UINT8*		pMem;
	pMem = m_pFrame;

	//. 2006. 6. 26. nonstopdj
	//. size검사를 먼저한다.
	ASSERT( (INT32 ) m_iCurAllocated + nBytes < m_iByteAllocated );

	m_iCurAllocated += nBytes;
	
	//. 2006. 1. 31. Nonstopdj
	//. error report에 남기기.
	if( (INT32) m_iCurAllocated >= m_iByteAllocated )
	{
		MD_SetErrorMessage( "AcuFrameMemory::AllocFrameMemory, Allocate size overflow." );
		return NULL;
	}

	#ifdef _DEBUG
	{
		// 최대 요구량 로그로 남김.
		if( g_uMaxUsageBuffer < m_iCurAllocated ) g_uMaxUsageBuffer = m_iCurAllocated;
	}
	#endif // _DEBUG

	
	m_pFrame += nBytes;

	return (void*)pMem;
}

void	AcuFrameMemory::DeallocFrameMemory(INT32	nBytes)
{
	#ifdef _DEBUG
	if( g_iMainThreadID != GetCurrentThreadId())
		MD_SetErrorMessage( "AcuFrameMemory::DeallocFrameMemory, called by other thread." );
	#endif

	m_pFrame -= nBytes;
	m_iCurAllocated -= nBytes;
}