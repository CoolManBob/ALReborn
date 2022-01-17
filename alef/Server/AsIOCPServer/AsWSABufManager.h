// AsWSABufManager.h: interface for the AsWSABufManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASWSABUFMANAGER_H__2DB4BB11_A873_4D29_A35B_0F27729612A9__INCLUDED_)
#define AFX_ASWSABUFMANAGER_H__2DB4BB11_A873_4D29_A35B_0F27729612A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApDefine.h"
#include "ApBase.h"

#if defined _M_X64
#define MAX_WSABUF_COUNT 1500
#else
#define MAX_WSABUF_COUNT 500
#endif

enum Enum_ADD_PACKET_RESULT
{
	ADD_PACKET_RESULT_NONE = 0,
	ADD_PACKET_RESULT_SUCCESS,		// 성공적으로 패킷 추가
	ADD_PACKET_RESULT_DROP,			// 패킷 무시
	ADD_PACKET_RESULT_DESTROY,		// 더이상 패킷을 보낼수 없는 상태, 종료시켜야됨
	ADD_PACKET_RESULT_MAX
};

class AsWSABufManager  
{
private:
	WSABUF			m_WsaBuffer[MAX_WSABUF_COUNT];		// 패킷들의 시작주소
	PACKET_PRIORITY	m_Priority[MAX_WSABUF_COUNT];		// 패킷 우선순위
	INT32			m_lPriorityCount[PACKET_PRIORITY_MAX];	// 우선순위에 해당하는 패킷의 수량
	INT32			m_lCurrentCount;					// 현재 누적되어 있는 패킷 수량
	INT32			m_lTotalBufSize;					// 누적되어 있는 패킷의 전체 크기

public:
	BOOL bSetTimerEvent;

private:
	BOOL RemoveWSABuffer(INT32 lIndex);
	PACKET_PRIORITY GetLastPriority(PACKET_PRIORITY ePriority);

public:
	AsWSABufManager();
	virtual ~AsWSABufManager();

	void Initialize();

	inline INT32 GetTotalBufferSize()	{return m_lTotalBufSize;}
	inline INT32 GetCurrentCount()		{return m_lCurrentCount;}

	Enum_ADD_PACKET_RESULT AddPacket(CHAR* pData, INT32 lLength, PACKET_PRIORITY ePriority);
	CHAR* GetStartBuffer();
	void BufferClear();
};

#endif // !defined(AFX_ASWSABUFMANAGER_H__2DB4BB11_A873_4D29_A35B_0F27729612A9__INCLUDED_)
