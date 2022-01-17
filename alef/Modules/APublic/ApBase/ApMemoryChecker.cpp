//#include "StdAfx.h"
#include "ApMemoryChecker.h"
#include "NoMemoryChecker.h"

#include <crtdbg.h>
//#include <tchar.h>

static char* szAllocationType[] =
{
	"NONE",
	"MALLOC",
	"NEW",
	"NEW_DEBUG",
	"NEW_ARRAY",
	"NEW_ARRAY_DEBUG"
};

static char* szDeallocationType[] =
{
	"NONE",
	"FREE",
	"DELETE",
	"DELETE_ARRAY"
};

__declspec(thread) const char	*tls_sourceFile;
__declspec(thread) const char	*tls_sourceFunc;
__declspec(thread) unsigned int	tls_sourceLine;

void setNewInfo(const char *file, const unsigned int line, const char *func)
{
	tls_sourceFile = file;
	tls_sourceFunc = func;
	tls_sourceLine = line;
}

#ifdef APMEMORY_CHECKER
void* operator new(size_t size)
{
	return ApMemoryChecker::getInstance().debug_alloc(tls_sourceFile, tls_sourceLine, tls_sourceFunc, EAT_NEW, size);
}

void* operator new(size_t size, const char *sourceFile, int sourceLine)
{
	return ApMemoryChecker::getInstance().debug_alloc(sourceFile, sourceLine, NULL, EAT_NEW_DEBUG, size);
}

void* operator new[](size_t size)
{
	return ApMemoryChecker::getInstance().debug_alloc(tls_sourceFile, tls_sourceLine, tls_sourceFunc, EAT_NEW_ARRAY, size);
}

void* operator new[](size_t size, const char *sourceFile, int sourceLine)
{
	return ApMemoryChecker::getInstance().debug_alloc(sourceFile, sourceLine, NULL, EAT_NEW_ARRAY_DEBUG, size);
}

void operator delete(void *address)
{
	return ApMemoryChecker::getInstance().debug_free(NULL, 0, NULL, EDT_DELETE, address);
}

void operator delete[](void *address)
{
	return ApMemoryChecker::getInstance().debug_free(NULL, 0, NULL, EDT_DELETE_ARRAY, address);
}

void* malloc_Checker(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			     const size_t size)
{
	return ApMemoryChecker::getInstance().debug_alloc(sourceFile, sourceLine, sourceFunc, EAT_MALLOC, size);
}

void free_Checker(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			      const void *address)
{
	return ApMemoryChecker::getInstance().debug_free(sourceFile, sourceLine, sourceFunc, EDT_FREE, address);
}

void* realloc_Checker(void *memblock, size_t size)
{
	// 사용 금지 함수
//	_ASSERT_EXPR(0, _T("don't use realloc"));
	return NULL;
}

void* calloc_Checker(size_t num, size_t size)
{
	// 사용 금지 함수
//	_ASSERT_EXPR(0, _T("don't use calloc"));
	return NULL;
}



void* ApMemoryChecker::memmove_Checker(void *dest, const void *src, size_t count)
{
	if (FALSE != IsBadCodePtr(static_cast<FARPROC>(dest)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadReadPtr(src, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadWritePtr(dest, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(dest))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(src))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidCountLimit(count))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	return memmove(dest, src, count);
}



void* ApMemoryChecker::memcpy_Checker(void *dest, const void *src, size_t count)
{
	if (FALSE != IsBadCodePtr(static_cast<FARPROC>(dest)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadReadPtr(src, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadWritePtr(dest, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(dest))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(src))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidCountLimit(count))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	return memcpy(dest, src, count);
}



void* ApMemoryChecker::memset_Checker(void *dest, int c, size_t count)
{
	if (FALSE != IsBadCodePtr(static_cast<FARPROC>(dest)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadWritePtr(dest, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(dest))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidCountLimit(count))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	return memset(dest, c, count);
}

#ifdef _UNICODE
wchar_t* ApMemoryChecker::wmemcpy_Checker(wchar_t *dest, const wchar_t *src, size_t count)
{
	if (FALSE != IsBadCodePtr(reinterpret_cast<FARPROC>(dest)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadReadPtr(src, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadWritePtr(dest, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(dest))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(src))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidCountLimit(count))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	return wmemcpy(dest, src, count);
}

wchar_t* ApMemoryChecker::wmemmove_Checker(wchar_t *dest, const wchar_t *src, size_t count)
{
	if (FALSE != IsBadCodePtr(reinterpret_cast<FARPROC>(dest)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadReadPtr(src, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadWritePtr(dest, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(dest))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(src))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidCountLimit(count))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	return wmemmove(dest, src, count);
}

wchar_t* ApMemoryChecker::wmemset_Checker(wchar_t *dest, wchar_t c, size_t count)
{
	if (FALSE != IsBadCodePtr(reinterpret_cast<FARPROC>(dest)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (FALSE != IsBadWritePtr(dest, static_cast<UINT_PTR>(count)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidBasePtr(dest))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	if (false == ApMemoryChecker::getInstance().isValidCountLimit(count))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return NULL;
	}

	return wmemset(dest, c, count);
}
#endif

#endif

ApMemoryChecker::ApMemoryChecker(void) : heap_(NULL), basePtr_(NULL), limitCount_(0), sequence_(0), 
							allocCount_(0), freeCount_(0), currentAllocCount_(0), currentAllocatedSize_(0), 
							currentDataSize_(0), first_(NULL), last_(NULL)
{
	AuAutoLock lock(lock_);

	heap_ = HeapCreate(0, 4096, 0);
//	_ASSERT_EXPR(heap_, _T("failed heap create"));
}

ApMemoryChecker::~ApMemoryChecker(void)
{
	if (NULL != heap_)
	{
		if (FALSE == HeapDestroy(heap_))
		{
//			_ASSERT_EXPR(heap_, _T("crashed heap handle"));
		}

		heap_ = NULL;
	}
	else
	{
//		_ASSERT_EXPR(heap_, _T("crashed heap handle"));
	}
}

ApMemoryChecker& ApMemoryChecker::getInstance()
{
	// Initialize를 이용하여 메모리를 할당한후 사용하여야 된다.
	static ApMemoryChecker Heap;
	return Heap;
}

void* ApMemoryChecker::debug_alloc(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			     eAllocationType allocType, const size_t size)
{
//	_ASSERT(NULL != heap_);

	AuAutoLock lock(lock_);

	size_t allocatedSize = sizeof(ApMemDebugInfo) + size + sizeof(unsigned int);
	void* allocUnit = HeapAlloc(heap_, 0, allocatedSize);
	ApMemDebugInfo *pDbg = static_cast<ApMemDebugInfo*>(allocUnit);

	if (NULL == first_)
		last_ = pDbg;
	else
		first_->prev_ = pDbg;

	pDbg->allocSize_ = allocatedSize;
	pDbg->dataSize_ = size;
	pDbg->allocType_ = allocType;
	pDbg->sourceFile_ = const_cast<char*>(sourceFile);
	pDbg->sourceLine_ = sourceLine;
	pDbg->sourceFunc_ = const_cast<char*>(sourceFunc);
	pDbg->sequence_ = ++sequence_;
	pDbg->deallocType_ = EDT_NONE;
	pDbg->next_ = first_;
	pDbg->prev_ = NULL;
	pDbg->frontGuardByte_ = FRONT_GUARD_BYTE;

	first_ = pDbg;

	unsigned int *pRearGuardByte = getRearGuardByte(pDbg);
	*pRearGuardByte = REAR_GUARD_BYTE;

	++allocCount_;
	++currentAllocCount_;
	currentAllocatedSize_ += allocatedSize;
	currentDataSize_ += size;
	
	return (static_cast<BYTE*>(allocUnit) + sizeof(ApMemDebugInfo));
}

void ApMemoryChecker::debug_free(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
		      eDeallocationType deallocType, const void *address)
{
	AuAutoLock lock(lock_);

	if (NULL == address) 
		return;

	// 포인터의 유효성 검사
	if (false == isValidPointer(address))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return;
	}

	// 메모리 할당 방법에 적합한 메모리 해제 방법인지 확인
	ApMemDebugInfo *pDebugInfo = getDebugInfo(address);
	if (false == isValidFree(pDebugInfo->allocType_, deallocType))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return;
	}

	// guard byte 확인
	if (false == isValidGuardByte(pDebugInfo))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return;
	}

//	_ASSERT_EXPR(((NULL != pDebugInfo->prev_) || (NULL != pDebugInfo->next_)), _T("invalid debuginfo"));

	// list 재구성
	if (pDebugInfo->next_)
	{
		pDebugInfo->next_->prev_ = pDebugInfo->prev_;
	}
	else
	{
//        _ASSERTE(last_ == pDebugInfo);
		last_ = pDebugInfo->prev_;
	}

	if (pDebugInfo->prev_)
	{
		pDebugInfo->prev_->next_ = pDebugInfo->next_;
	}
	else
	{
//		_ASSERTE(first_ == pDebugInfo);
		first_ = pDebugInfo->next_;
	}

	++freeCount_;
	--currentAllocCount_;
	currentAllocatedSize_ -= pDebugInfo->allocSize_;
	currentDataSize_ -= pDebugInfo->dataSize_;

	// 메모리 해제
	if (FALSE == HeapFree(heap_, 0, static_cast<void*>(pDebugInfo)))
	{
		RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
		return;
	}
}

void ApMemoryChecker::setBasePtr(const void *pointer)
{
	AuAutoLock lock(lock_);

	basePtr_ = const_cast<void*>(pointer);
}

bool ApMemoryChecker::isValidBasePtr(const void *pointer)
{
	AuAutoLock lock(lock_);

	// basePtr_가 NULL이면 사용하지 않는다는 뜻
	if (NULL == basePtr_)
		return true;

	if (basePtr_ > pointer)
		return false;

	return true;
}

void ApMemoryChecker::setCountLimit(const size_t limitCount)
{
	AuAutoLock lock(lock_);

	limitCount_ = limitCount;
}

bool ApMemoryChecker::isValidCountLimit(const size_t count)
{
	AuAutoLock lock(lock_);

	// limitCount_가 0이면 사용하지 않는다는 뜻
	if (0 == limitCount_)
		return true;

	if (limitCount_ < count)
		return false;

	return true;
}

unsigned int* ApMemoryChecker::getRearGuardByte(const ApMemDebugInfo *pDebugInfo)
{
	AuAutoLock lock(lock_);

	BYTE *pAllocUnit = reinterpret_cast<BYTE*>(const_cast<ApMemDebugInfo*>(pDebugInfo));
	return reinterpret_cast<unsigned int*>(pAllocUnit + sizeof(ApMemDebugInfo) + pDebugInfo->dataSize_);
}

bool ApMemoryChecker::isValidGuardByte(const ApMemDebugInfo *pDebugInfo)
{
	AuAutoLock lock(lock_);

	if (FRONT_GUARD_BYTE != pDebugInfo->frontGuardByte_)
		return false;

	if (REAR_GUARD_BYTE != *(getRearGuardByte(pDebugInfo)))
		return false;

	return true;
}

bool ApMemoryChecker::isValidFree(eAllocationType allocType, eDeallocationType deallocType)
{
	AuAutoLock lock(lock_);

	switch (allocType)
	{
	case EAT_NONE:
		{
//			_ASSERT_EXPR(0, _T("invalid allocation type : EAT_NONE"));
			return false;
		}

	case EAT_MALLOC:
		{
			if (EDT_FREE == deallocType)
				return true;

			return false;
		}

	case EAT_NEW:
	case EAT_NEW_DEBUG:
		{
			if (EDT_DELETE == deallocType)
				return true;

			return false;
		}

	case EAT_NEW_ARRAY:
	case EAT_NEW_ARRAY_DEBUG:
		{
			if (EDT_DELETE_ARRAY == deallocType)
				return true;

			return false;
		}

	default:
		{
//			_ASSERT_EXPR(0, _T("invalid allocation type : default"));
			return false;
		}
	};

	return false;
}

ApMemDebugInfo* ApMemoryChecker::getDebugInfo(const void *pRawData)
{
	AuAutoLock lock(lock_);

//	_ASSERT_EXPR(pRawData, _T("pRawData is null pointer"));
	return reinterpret_cast<ApMemDebugInfo*>(static_cast<BYTE*>(const_cast<void*>(pRawData)) - 
												sizeof(ApMemDebugInfo));
}

bool ApMemoryChecker::isValidPointer(const void *address)
{
	AuAutoLock lock(lock_);

	if (FALSE != IsBadCodePtr(static_cast<FARPROC>(address)))
	{
		DWORD error = GetLastError();
//		_ASSERT_EXPR(0, _T("invalid IsBadCodePtr"));
		return false;
	}

	ApMemDebugInfo *pDebugInfo = getDebugInfo(address);
//	_ASSERT_EXPR(pDebugInfo, _T("pDebugInfo is null pointer"));

	if (FALSE != IsBadWritePtr(static_cast<void*>(pDebugInfo), pDebugInfo->allocSize_))
	{
//		_ASSERT_EXPR(0, _T("invlid IsBadWritePtr"));
		return false;
	}

	return true;
}

void ApMemoryChecker::dumpAll()
{
	//for (ApMemDebugInfo *iter = first_; NULL != iter; iter = iter->next_)
	//{
	//	printf("%d\n", iter->sequence_);
	//}
}

void ApMemoryChecker::dumpCrashedMemory()
{
	//for (ApMemDebugInfo *iter = first_; NULL != iter; iter = iter->next_)
	//{
	//	if (false == isValidGuardByte(iter))
	//		printf("%d\n", iter->sequence_);
	//}	
}
