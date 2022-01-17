#pragma once

#undef APMEMORY_CHECKER

#define _CRT_ALLOCATION_DEFINED
#define _STD_USING

#include "ApMutualEx.h"
#include <windows.h>

const unsigned int FRONT_GUARD_BYTE	= 0xAABBCCDD;
const unsigned int REAR_GUARD_BYTE	= 0xDDCCBBAA;

enum eAllocationType
{
	EAT_NONE = 0,
	EAT_MALLOC,				// malloc
	EAT_NEW,				// new(size)
	EAT_NEW_DEBUG,			// new(size, sourceFile, sourceLine)
	EAT_NEW_ARRAY,			// new [] (size)
	EAT_NEW_ARRAY_DEBUG,	// new [] (size, sourceFile, sourceLine)
};

enum eDeallocationType
{
	EDT_NONE = 0,
	EDT_FREE,				// free
	EDT_DELETE,				// delete
	EDT_DELETE_ARRAY,		// delete []
};

struct ApMemDebugInfo
{
	size_t				dataSize_;			// 실제로 할당 요청 사이즈
	size_t				allocSize_;			// 메모리에 할당 된 사이즈
	char*				sourceFile_;		// 파일 이름
	char*				sourceFunc_;		// 함수 이름
	int					sourceLine_;		// line 번호
	LONG				sequence_;			// 할당된 순서
	eAllocationType		allocType_;			// 할당 타입
	eDeallocationType	deallocType_;		// 할당 해제 타입
	ApMemDebugInfo		*next_;				// next node
	ApMemDebugInfo		*prev_;				// previous node
	unsigned int		frontGuardByte_;	// front guard byte

	// followed by:
	// unsigned char rawData[size_]
	// unsigned int rearguardbyte
};

class ApMemoryChecker
{
public:
	ApMemoryChecker(void);
	~ApMemoryChecker(void);

	static ApMemoryChecker& getInstance();

	void dumpAll();
	void dumpCrashedMemory();

	void setBasePtr(const void *pointer);
	bool isValidBasePtr(const void *pointer);

	void setCountLimit(const size_t limitCount);
	bool isValidCountLimit(const size_t count);

	void* debug_alloc(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			     eAllocationType allocType, const size_t size);

	void debug_free(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			      eDeallocationType deallocType, const void *pointer);

	// MBCS
	void* memmove_Checker(void *dest, const void *src, size_t count);
	void* memcpy_Checker(void *dest, const void *src, size_t count);
	void* memset_Checker(void *dest, int c, size_t count);

#ifdef _UNICODE
	wchar_t* wmemmove_Checker(wchar_t *dest, const wchar_t *src, size_t count);
	wchar_t* wmemcpy_Checker(wchar_t *dest, const wchar_t *src, size_t count);
	wchar_t* wmemset_Checker(wchar_t *dest, wchar_t c, size_t count);
#endif

// c++ style
void	setNewInfo(const char *file, const unsigned int line, const char *func);

private:
	unsigned int* getRearGuardByte(const ApMemDebugInfo *pDebugInfo);
	ApMemDebugInfo* getDebugInfo(const void *pRawData);
	bool isValidPointer(const void *pointer);
	bool isValidFree(eAllocationType allocType, eDeallocationType deallocType);
	bool isValidGuardByte(const ApMemDebugInfo *pDebugInfo);

private:
	HANDLE heap_;

	void *basePtr_;
	size_t limitCount_;

	LONG sequence_;				// 할당 번호(malloc, new 통합)
	LONG allocCount_;			// 메모리 할당을 호출한 횟수
	LONG freeCount_;			// 메모리 해제를 이용한 횟수 (pooling 제외)
	LONG currentAllocCount_;	// 현재 할당된 메모리 갯수

	size_t currentAllocatedSize_;	// 현재 할당된 메모리의 Byte 크기(debug 정보 포함 사이즈)
	size_t currentDataSize_;		// 할당된 데이터의 Byte 크기(순수 데이터 사이즈)

	ApMemDebugInfo *first_;
	ApMemDebugInfo *last_;

	ApCriticalSection lock_;
};

void* malloc_Checker(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			     const size_t size);

void free_Checker(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			      const void *pointer);

void* realloc_Checker(void *memblock, size_t size);
void* calloc_Checker(size_t num, size_t size);

//// memory 관련 함수
//void* memmove_Checker(void *dest, const void *src, size_t count);
////wchar_t* wmemmove_Checker(wchar_t *dest, const wchar_t *src, size_t count);
//
//void* memcpy_Checker(void *dest, const void *src, size_t count);
////wchar_t* wmemcpy_Checker(wchar_t *dest, const wchar_t *src, size_t count);
//
//void* memset_Checker(void *dest, int c, size_t count);
//
////wchar_t* wmemset_Checker(wchar_t *dest, wchar_t c, size_t count);
//
//// c++ style
//void	setNewInfo(const char *file, const unsigned int line, const char *func);

// preprocessor definitions 에서 APMEMORY_CHECKER를 선언해주어야 한다.
#ifdef APMEMORY_CHECKER

void*	operator new(size_t size);
void*	operator new[](size_t size);
void*	operator new(size_t size, const char *sourceFile, int sourceLine);
void*	operator new[](size_t size, const char *sourceFile, int sourceLine);
void	operator delete(void *pointer);
void	operator delete[](void *pointer);



//#define	new						(setNewInfo  (__FILE__,__LINE__,__FUNCTION__), false) ? NULL : new
//#define	delete					(setNewInfo  (__FILE__,__LINE__,__FUNCTION__),false) ? m_setOwner("",0,"") : delete

#define	malloc(_SIZE)			malloc_Checker(__FILE__,__LINE__,__FUNCTION__, _SIZE)
#define	calloc(_NUM, _SIZE)		calloc_Checker(_NUM, _SIZE)
#define	realloc(_PTR, _SIZE)	realloc_Checker(_PTR, _SIZE)
#define	free(_PTR)				free_Checker(__FILE__,__LINE__,__FUNCTION__, _PTR)

// MBCS
#define memmove(_DEST_PTR, _SRC_PTR, _COUNT)	ApMemoryChecker::getInstance().memmove_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
#define memcpy(_DEST_PTR, _SRC_PTR, _COUNT)		ApMemoryChecker::getInstance().memcpy_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
#define memset(_DEST_PTR, _CHAR, _COUNT)		ApMemoryChecker::getInstance().memset_Checker(_DEST_PTR, _CHAR, _COUNT)

#ifdef _UNICODE
#define wmemmove(_DEST_PTR, _SRC_PTR, _COUNT)	ApMemoryChecker::getInstance().wmemmove_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
#define wmemcpy(_DEST_PTR, _SRC_PTR, _COUNT)	ApMemoryChecker::getInstance().wmemcpy_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
#define wmemset(_DEST_PTR, _CHAR, _COUNT)		ApMemoryChecker::getInstance().wmemset_Checker(_DEST_PTR, _CHAR, _COUNT)
#endif

//#define MoveMemory(_DEST_PTR, _SRC_PTR, _COUNT) memmove_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
//#define CopyMemory(_DEST_PTR, _SRC_PTR, _COUNT) memcpy_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
//#define FillMemory(_DEST_PTR, _COUNT, _CHAR)	memset_Checker(_DEST_PTR, _CHAR, _COUNT)
//#define ZeroMemory(_DEST_PTR, _LENGTH)			memset_Checker(_DEST_PTR, 0, _LENGTH)

#endif