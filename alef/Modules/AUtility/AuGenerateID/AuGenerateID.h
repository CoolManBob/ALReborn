// AuGenerateID.h: interface for the AuGenerateID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUGENERATEID_H__13BF55DD_B44E_494E_B41D_098F6E02773D__INCLUDED_)
#define AFX_AUGENERATEID_H__13BF55DD_B44E_494E_B41D_098F6E02773D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "ApMutualEx.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuGenerateIDD" )
#else
#pragma comment ( lib , "AuGenerateID" )
#endif
#endif

#pragma pack (1)
class AuFIFOQueue
{
private:
	INT32				m_lQueueSize;
	INT32				m_lDataSize;

	INT32				m_lHead;
	INT32				m_lTail;

	PVOID				m_pvQueue;

	INT32				m_lCurrentDataCount;

public:
	AuFIFOQueue();
	~AuFIFOQueue();

	BOOL				Initialize(INT32 lQueueSize, INT32 lDataSize);

	BOOL				AddData(PVOID pvData);
	BOOL				GetHeadData(PVOID pvData);
};

class AuRemovePool
{
	PVOID				m_pvPool;

	INT32				m_lPoolSize;
	INT32				m_lDataSize;
	INT32				m_lCurrentDataCount;

	AuRemovePool*		m_pNext;

public:
	AuRemovePool();
	~AuRemovePool();

	BOOL				Initialize(INT32 lPoolSize, INT32 lDataSize);

	BOOL				AddData(PVOID pvData);
	BOOL				IsFull();

	BOOL				SetNextPool(AuRemovePool *pNext);
	AuRemovePool*		GetNextPool();

	PVOID				GetData(INT32 lIndex);

	BOOL				Reset();
};

class AuGenerateID  
{
private:
	UINT32				m_ulServerFlag;
	UINT32				m_ulServerClearFlag;

	INT32				m_lID;
	ApCriticalSection	m_MutexID;

	INT32				m_lMinID;
	INT32				m_lMaxID;

	AuFIFOQueue			m_csRemoveIDQueue;

	BOOL				m_bUsePool;

	AuRemovePool		m_csRemovePool[2];
	AuRemovePool*		m_pCurrentPool;

public:
	AuGenerateID();
	virtual ~AuGenerateID();

	BOOL	Initialize(UINT32 ulStartValue = 1, UINT32 ulServerFlag = 0, INT16 nSizeServerFlag = 0, INT32 lRemoveIDQueueSize = 0, BOOL bUsePool = FALSE);
	INT32	GetID();
	INT32	GetCurrentID();

	BOOL	AddRemoveID(INT32 lRemoveID);
};

//////////////////////////////////////////////////////////////////////////
//
class AuGenerateID64  
{
private:
	INT32				m_lServerIndex;
	UINT16				m_llID;
	ApCriticalSection	m_MutexID;

public:
	AuGenerateID64();
	virtual ~AuGenerateID64();

	BOOL	Initialize(INT32 ServerIndex);
	INT64	GetID();
};
#pragma pack ()
#endif // !defined(AFX_AUGENERATEID_H__13BF55DD_B44E_494E_B41D_098F6E02773D__INCLUDED_)
