#pragma once

#include "ApMutualEx.h"

const INT32 MAX_TYPE_SIZE		= 1000;			// MemoryManager에서 사용될수 있는 Type의 최대 갯수
const INT32 DEFAULT_ARRAY_SIZE	= 1000;			// 하나의 Type에 할당될 배열 크기
const INT32 MAX_TYPE_NAME_SIZE	= 64;

#define GUARD_BYTE_BEGIN	0xBB
#define GUARD_BYTE_END		0xCC
#define ALLOC_BYTE			0xDD
#define FREE_BYTE			0xEE

#if defined (WIN64)
	#define LAST_NODE			0xFFFFFFFFFFFFFFFF
#else
	#define LAST_NODE			0xFFFFFFFF
#endif

class ApMemoryManager 
{
public:
	// Linked List를 위해서 필요한 변수들
	PVOID	m_pHeadArray[MAX_TYPE_SIZE];		// linked list의 Head가 저장될곳
	PVOID	m_pHeadEmpty[MAX_TYPE_SIZE];		// linked list의 비어있는 Head가 저장될곳
												// (마지막으로 할당된 데이터의 바로 다음 노드)

	// 실제 데이터의 정보를 위해서 필요한 변수들
	INT32	m_lDataSize[MAX_TYPE_SIZE];			// 해당 오브젝트의 데이터 크기 sizeof
	INT32	m_lAllocSize[MAX_TYPE_SIZE];		// 실제 할당된 데이터의 크기
	INT32	m_lArraySize[MAX_TYPE_SIZE];		// 해당 배열의 데이터 갯수

	INT32	m_lAllocCount[MAX_TYPE_SIZE];		// 현재 할당된 데이터의 갯수
	INT32	m_lAllocCountPeak[MAX_TYPE_SIZE];	// 순간 최대 메모리 할당 갯수

	// 초기에 설정된 풀링갯수를 넘어가면 ::operator new를 호출하게 되는데 
	// 그때 alloc된 갯수를 측정하기 위해서 추가
	INT32	m_lAddedAllocCount[MAX_TYPE_SIZE];	// 추가된 할당 갯수
	INT32	m_lFlagAssert[MAX_TYPE_SIZE];		// 풀링 갯수를 넘어갈때 한번만 ASSERT를 띄워주기 위한 플래그용변수

	INT32	m_bEnable[MAX_TYPE_SIZE];			// 메모리 풀링의 Enable/Disable 여부를 결정

#ifdef _CPPRTTI
	CHAR	m_TypeName[MAX_TYPE_SIZE][MAX_TYPE_NAME_SIZE + 1];		// RTTI를 이용한 Class Name
#endif

	INT32	m_lTypeIndex;

	ApCriticalSection	m_cs;	// CriticalSection인데 이름이 mutex라 바꿈

	SYSTEMTIME	m_stStartTime;

private:
	ApMemoryManager();

	VOID	Initialize();
	VOID	InitializeNode(INT32 lTypeIndex);
	BOOL	CheckBeginGuardByte(PVOID pObject);
	BOOL	CheckEndGuardByte(PVOID pObject, INT32 lTypeIndex);
	BOOL	GetAllocated(PVOID pObject);
	VOID	SetAllocation(PVOID pObject, BOOL bAllocation);

public:

	~ApMemoryManager();

	static ApMemoryManager& GetInstance();

	VOID DestroyInstance();
	VOID Destroy();

	INT32 AddType(SIZE_T lSize, SIZE_T lArraySize = DEFAULT_ARRAY_SIZE, const CHAR* pTypeName = NULL);

	PVOID NewObject(INT32 lTypeIndex);
	VOID DeleteObject(INT32 lTypeIndex, PVOID pObject);

	INT32 GetAllocatedCount(INT32 lTypeIndex);
	VOID ReportMemoryInfo();

	BOOL SetEnable(INT32 lTypeIndex, BOOL bEnable);
	BOOL GetEnable(INT32 lTypeIndex);
};
