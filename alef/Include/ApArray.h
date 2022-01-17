#if !defined(__APARRAY_H__)
#define __APARRAY_H__

#include "ApBase.h"

/******************************************************************************
Module:  ApList.h
Notices: Copyright (c) 2002 netong
Purpose: 
Last Update: 2003. 11. 17
			 ApArray를 장시간 사용하였을때 GetSequence 사용시 비효율적인 요인 발생
			 배열 인덱스에 기초를둔 double-linked list를 사용하여 노드관리를 하면
			 최적화 가능 -> 적용
******************************************************************************/

// Node 관리를 위한 구조체
struct stDLISTDATA;
struct stNodeData
{
	stDLISTDATA	*pPrev;
	stDLISTDATA	*pNext;

	BOOL		bIsSetData;		// 데이터가 pData에 설정되어 있으면 TRUE, 그렇지 않으면 FALSE
	INT32		lCurrentIndex;
};

struct stDLISTDATA : public stNodeData
{
	BYTE		*pData;
};

const INT32 ARRAY_OFFSET_COUNT = 2;	// Head와 Tail Node를 추가해야 하므로 2개가 더 붙는다.

/******************************************************************************
필독 :	VC++ 6.0 컴파일러가 SGI STL에서 pair형태의 iterator를 지원하지 못하기 때문에
		return값이 0이면 데이터가 없는걸로 판별.
		
		그래서 ApArray의 인덱스를 0 base가 아닌 1 base 배열 인덱스를 리턴
		(내부적으로는 0 base를 사용, 외부에서는 1 base로 사용)
******************************************************************************/

// ApArray class
///////////////////////////////////////////////////////////////////////////////
template <INT32 TUseAutoLock> 
class ApArray {
public:
	BYTE		*m_pHeadArray;			// array pointer
	stDLISTDATA	*m_pstHead;				// 
	stDLISTDATA	*m_pstTail;				// 

	stDLISTDATA	*m_pstHeadEmpty;		// 

	INT32		m_lArraySize;			// array size
	INT32		m_lDataSize;			// data size
	INT32		m_lAllocDataSize;		// 실제 메모리에 할당된 Node를 포함한 data size

	INT32		m_lDataCount;			// # of inserted data

	ApMutualEx	m_Mutex;

private:
	BOOL		InitializeNode();		// double linked-list node 설정
	stDLISTDATA* operator [] (INT32 lIndex);

public:
	ApArray();
	~ApArray();

	BOOL Initialize(INT32 lArraySize, INT32 lDataSize);
	INT32 Add(PVOID pData);
	BOOL Delete(INT32 lIndex);
	BOOL DeletePrev(INT32 lIndex);

	PVOID GetData(INT32 lIndex);
	PVOID GetSequence(INT32* plIndex);

	BOOL Reset();

	BOOL SetNotUseLockManager();
};

// ApArray member functions
///////////////////////////////////////////////////////////////////////////////

template <INT32 TUseAutoLock>
inline ApArray<TUseAutoLock>::ApArray()
{
	m_pHeadArray		= NULL	;
	m_pstHeadEmpty		= NULL	;

	m_lArraySize		= 0		;
	m_lDataSize			= 0		;
	m_lDataCount		= 0		;
	m_lAllocDataSize	= 0		;

	m_pstHead			= NULL	;
	m_pstTail			= NULL	;
}

#pragma warning(disable:4786)
template <INT32 TUseAutoLock>
inline ApArray<TUseAutoLock>::~ApArray()
{
	if (m_pHeadArray)
	{
		free(m_pHeadArray);
	}
}
#pragma warning(default:4786)

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::Initialize(INT32 lArraySize, INT32 lDataSize)
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	m_lAllocDataSize = lDataSize + sizeof(stNodeData);	// double linked-list를 위해

	if (m_pHeadArray)
	{
		free(m_pHeadArray);
		m_pHeadArray	= NULL;
	}

	ASSERT( NULL == m_pHeadArray );
	//m_pHeadArray = (BYTE *) GlobalAlloc(GMEM_FIXED, (lArraySize + ARRAY_OFFSET_COUNT) * m_lAllocDataSize);
	m_pHeadArray = (BYTE *) malloc((lArraySize + ARRAY_OFFSET_COUNT) * m_lAllocDataSize);
	if (m_pHeadArray == NULL) return FALSE;	// memory allocation error...

	m_pstHeadEmpty	= (stDLISTDATA*)m_pHeadArray;
	m_lArraySize	= lArraySize;
	m_lDataSize		= lDataSize;

	InitializeNode();

	return TRUE;
}

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::InitializeNode()
{
	// 초기화가 되지 않은 상태이므로 FALSE를 리턴
	if (0 == m_lArraySize || 0 == m_lDataSize) return FALSE;

	ZeroMemory(m_pHeadArray, (m_lArraySize + ARRAY_OFFSET_COUNT) * m_lAllocDataSize);

	stDLISTDATA *pstCurrent, *pstNext;
	pstCurrent	= (stDLISTDATA*)m_pHeadArray;
	pstNext		= (stDLISTDATA*)((BYTE*)m_pHeadArray + m_lAllocDataSize);
	m_pstHead   = pstCurrent;

	// double linked list형태로 node 설정
	for(INT32 i = 0; i < (m_lArraySize + ARRAY_OFFSET_COUNT - 1); ++i) 
	{
		pstCurrent->lCurrentIndex = i;
		pstCurrent->pNext = pstNext;
		pstNext->pPrev = pstCurrent;

		pstCurrent = pstNext;
		pstNext = (stDLISTDATA*)((BYTE*)pstNext + m_lAllocDataSize);
	}

	m_pstTail = pstCurrent;
	m_pstTail->pNext = NULL;

	// Head는 GetSequence를 수행할때 시작점으로 사용되므로 따로 관리
	m_pstHeadEmpty = (stDLISTDATA*)((BYTE*)m_pHeadArray + m_lAllocDataSize);

	return TRUE;
}

template <INT32 TUseAutoLock>
inline stDLISTDATA* ApArray<TUseAutoLock>::operator [] (INT32 lIndex)
{
	if ((m_lArraySize + 1 < lIndex) || (lIndex <= 0))	return NULL;

	--lIndex;
	stDLISTDATA *pstCurrent = (stDLISTDATA*)((BYTE*)m_pHeadArray + (lIndex * m_lAllocDataSize));

	return pstCurrent;

	/*
	if (pstCurrent->bIsSetData) return pstCurrent;
	else return NULL;
	*/
}

template <INT32 TUseAutoLock>
inline INT32 ApArray<TUseAutoLock>::Add(PVOID pData)
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if ((!pData) || (m_lDataCount >= m_lArraySize))
		return (-1);

	// Node 설정
	stDLISTDATA *pstCurrent = m_pstHeadEmpty;

	m_pstHeadEmpty = pstCurrent->pNext;

	// 데이터 복사
	CopyMemory(&pstCurrent->pData, pData, m_lDataSize);
	pstCurrent->bIsSetData = TRUE;

	++m_lDataCount;

	return pstCurrent->lCurrentIndex + 1;
}

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::Delete(INT32 lIndex)
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if ((m_lArraySize + 1 < lIndex) || (lIndex <= 0) || (m_lDataCount <= 0))
		return FALSE;

	stDLISTDATA *pstCurrent = (*this)[lIndex];
	if (!pstCurrent || !pstCurrent->bIsSetData) return FALSE;	// bIsSetData가 세팅되지 않은 경우

	// 다음 노드가 m_pstHeadEmpty라면 노드 설정해주는 작업이 필요하지 않음
	if (pstCurrent->pNext != m_pstHeadEmpty)
	{
		// 삭제된 노드를 HeadEmpty로 올리고 나머지 노드 설정 작업 진행
		stDLISTDATA *pstPrev, *pstNext, *pstHeadEmptyPrev;
		pstPrev = pstCurrent->pPrev;
		pstNext = pstCurrent->pNext;
		pstHeadEmptyPrev = m_pstHeadEmpty->pPrev;

		// Current를 제외한 앞, 뒤의 노드를 연결
		pstPrev->pNext = pstNext;
		pstNext->pPrev = pstPrev;

		// HeadEmpty와 HeadEmpty의 Prev 사이로 Current Node를 옮긴다.
		pstCurrent->pPrev = pstHeadEmptyPrev;
		pstHeadEmptyPrev->pNext = pstCurrent;

		// HeadEmpty의 앞 노드로 연결
		pstCurrent->pNext = m_pstHeadEmpty;
		m_pstHeadEmpty->pPrev = pstCurrent;
	}

	// 데이터 설정 안된걸로 설정
	pstCurrent->bIsSetData = FALSE;

	// 새로운 HeadEmpty로 등극
	m_pstHeadEmpty = pstCurrent;

	--m_lDataCount;

	return TRUE;
}

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::DeletePrev(INT32 lIndex)
{
	// operator []에서는 AutoLock2를 사용하지 않으므로 
	// 여기서 AutoLock2를 사용해준다.
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if (m_lArraySize + 1 < lIndex)
		return FALSE;

	stDLISTDATA *pstCurrent = (*this)[lIndex];
	if (!pstCurrent) return FALSE;
	
	return Delete(pstCurrent->pPrev->lCurrentIndex + 1);
}

template <INT32 TUseAutoLock>
inline PVOID ApArray<TUseAutoLock>::GetData(INT32 lIndex)
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if (m_lArraySize + 1 < lIndex || lIndex < 0)
		return NULL;

	stDLISTDATA *pstCurrent = (*this)[lIndex];

	if (pstCurrent && pstCurrent->bIsSetData) return &pstCurrent->pData;
	else return NULL;
}

template <INT32 TUseAutoLock>
inline PVOID ApArray<TUseAutoLock>::GetSequence(INT32* plIndex)
{
	if (!plIndex || *plIndex < 0 || m_lArraySize + 1 < *plIndex)
		return NULL;

	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if (m_lDataCount <= 0)
		return NULL;

	if (!m_pstHead) return NULL;

	stDLISTDATA *pstCurrent;
	if (0 == *plIndex)
		pstCurrent = m_pstHead->pNext;
	else 
		pstCurrent = (*this)[*plIndex];

	PVOID	pvRetVal	= NULL;

	if (pstCurrent)
	{
		if (pstCurrent->bIsSetData)
		{
			*plIndex = pstCurrent->pNext->lCurrentIndex + 1;

			pvRetVal	= (PVOID)&pstCurrent->pData;
		}
	}

	return pvRetVal;
}

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::Reset()
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if (!InitializeNode()) return FALSE;
	m_lDataCount	= 0;

	return TRUE;
}

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::SetNotUseLockManager()
{
	return m_Mutex.SetNotUseLockManager();
}

#endif	// __APARRAY_H__