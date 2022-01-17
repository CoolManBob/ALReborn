// XTPCustomHeap.h: *** template definition.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(_XTPCUSTOMHEAP_H__)
#define _XTPCUSTOMHEAP_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE

#include "XTPVC80Helpers.h"


//{{AFX_CODEJOCK_PRIVATE
#define XTP_EXPORT_PARAMS_NO

typedef enum _XTP_HEAP_INFORMATION_CLASS
{
	xtpHeapCompatibilityInformation
} 
XTP_HEAP_INFORMATION_CLASS;
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//      This function enables features for a specified heap.
// Parameters:
//      hHeapHandle   - [in] Handle to the heap where information is to be set.
//                    This handle is returned by either the HeapCreate or
//                    GetProcessHeap function.
//
//      pHeapInformation - [in] Heap information buffer. The format of this data
//                         depends on the HeapCompatibilityInformation class.
//
//      nHeapInformationLength - [in] Size of the pHeapInformation buffer, in bytes.
// Remarks:
//      This function is a wrapper to HeapSetInformation windows API function.
//      If HeapSetInformation is not supported by windows, this function exits
//      without any actions.
// Returns:
//      If the function succeeds, the return value is nonzero
//      If the function fails, the return value is 0 (zero). To get extended
//      error information, call GetLastError.
//      If HeapSetInformation is not supported by windows return value is also TRUE.
// See Also:
//      HeapSetInformation, XTPHeapSetLowFragmentation
//===========================================================================
AFX_INLINE BOOL XTPHeapSetCompatibilityInformation(HANDLE hHeapHandle, PVOID pHeapInformation, ULONG_PTR nHeapInformationLength)
{
	typedef BOOL (WINAPI *PFNHEAPSETINFORMATION)(HANDLE HeapHandle, XTP_HEAP_INFORMATION_CLASS eHICls, PVOID HeapInformation, ULONG_PTR HeapInformationLength);

	HMODULE hKernel = ::GetModuleHandle(_T("KERNEL32.DLL"));
	ASSERT(hKernel);
	if (hKernel)
	{
		PFNHEAPSETINFORMATION pfHeapSetInformation = (PFNHEAPSETINFORMATION)::GetProcAddress(hKernel, "HeapSetInformation");
		if (pfHeapSetInformation)
		{
			return pfHeapSetInformation(hHeapHandle, xtpHeapCompatibilityInformation,
											 pHeapInformation, nHeapInformationLength);
		}
	}
	return TRUE; // not supported;
}

//===========================================================================
// Summary:
//      Call this member to enable "Low-fragmentation Heap" (LFH) feature for
//      a given heap.
// Parameters:
//      hHeapHandle   - [in] Handle to the heap where information is to be set.
//                    This handle is returned by either the HeapCreate or
//                    GetProcessHeap function.
// Remarks:
//      This function is a wrapper to HeapSetInformation windows API function.
//      If HeapSetInformation is not supported by windows, this function exits
//      without any actions.
// Returns:
//      If the function succeeds, the return value is nonzero
//      If the function fails, the return value is 0 (zero). To get extended
//      error information, call GetLastError.
//      If HeapSetInformation is not supported by windows return value is also TRUE.
// See Also:
//      HeapSetInformation, XTPHeapSetCompatibilityInformation
//===========================================================================
AFX_INLINE BOOL XTPHeapSetLowFragmentation(HANDLE hHeapHandle)
{
	// LFH mode does not enabled under debugger,
	// only under 'clear' run.
	ULONG uHI = 2;
	return XTPHeapSetCompatibilityInformation(hHeapHandle, &uHI, sizeof(uHI));
}

//===========================================================================
// Summary:
//      This template class used as a base class for allocators which can use
//      a custom (separate) heap. Each allocator use own heap or standard heap.
//      To use custom heap set ms_bUseCustomHeap member to TRUE.
// Parameters:
//      _TData - This class must contain following static members for allocator:
//
//          struct allocatorClassData
//          {
//              static HANDLE       ms_hCustomHeap;     // handle to the custom heap
//              static LONG         ms_dwRefs;          // allocated blocks count;
//              static BOOL         ms_bLFHEnabled;     // report is LFH enabled for custom heap;
//              static BOOL         ms_bUseCustomHeap;  // Define does allocator use custom heap or default heap;
//          };
// See Also:
//      XTP_DECLARE_HEAP_ALLOCATOR, XTP_IMPLEMENT_HEAP_ALLOCATOR,
//      CXTPHeapObjectT
//===========================================================================
template<class _TData>
class CXTPHeapAllocatorT : public _TData
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	typedef _TData TData;
	//}}AFX_CODEJOCK_PRIVATE


	//*** Allocator Interface ***


	//-----------------------------------------------------------------------
	// Summary:
	//      Allocate memory block of nBytes size.
	// Returns:
	//      A pointer to allocated block or NULL.
	// See Also:
	//      Free_mem
	//-----------------------------------------------------------------------
	static void* AFX_CDECL Alloc_mem(size_t nBytes)
	{
		InterlockedIncrement(&ms_dwRefs); //ms_dwRefs++;

		if (ms_bUseCustomHeap)
		{
			CreateHeapIfNeed();

			return ::HeapAlloc(ms_hCustomHeap, 0, nBytes);
		}
		else
		{
		#ifdef _DEBUG
			return DEBUG_NEW char[nBytes];
		#else
			return new char[nBytes];
		#endif

		}
	};

	//-----------------------------------------------------------------------
	// Summary:
	//      Free memory block previously allocated by Alloc_mem.
	// Parameters:
	//      p - [in] Pointer to a memory block.
	// See Also:
	//      Alloc_mem
	//-----------------------------------------------------------------------
	static void AFX_CDECL Free_mem(void* p)
	{
		LONG nRefs = 0;
		if (ms_dwRefs)
			nRefs = InterlockedDecrement(&ms_dwRefs); //ms_dwRefs--;

		if (ms_bUseCustomHeap)
		{
			VERIFY(::HeapFree(ms_hCustomHeap, 0, p));

			if (nRefs == 0)
				ClearHeap();
		}
		else
		{
			delete p;
		}
	};


	//*** Implementation ***

	//{{AFX_CODEJOCK_PRIVATE
	CXTPHeapAllocatorT()
	{
	};

	virtual ~CXTPHeapAllocatorT()
	{
		// When other static objects use this allocator,
		// they may be destroyed later (and free memory later)
		// Each static object, which use this allocator,
		// should check ms_dwRefs and destroy heap if no more refs.
		if (ms_dwRefs == 0)
			ClearHeap();
	};

	static void AFX_CDECL CreateHeapIfNeed()
	{
		if (!ms_hCustomHeap)
		{
			ms_hCustomHeap = ::HeapCreate(0, 0, 0);
			ASSERT(ms_hCustomHeap);
			ms_bLFHEnabled = XTPHeapSetLowFragmentation(ms_hCustomHeap);
		}
	}

	static void AFX_CDECL ClearHeap()
	{
		ASSERT(ms_dwRefs == 0);
		if (ms_hCustomHeap)
			VERIFY(::HeapDestroy(ms_hCustomHeap));
		ms_hCustomHeap = NULL;
	}
	//}}AFX_CODEJOCK_PRIVATE
};


//{{AFX_CODEJOCK_PRIVATE
#define XTP_DECLARE_HEAP_ALLOCATOR_(allocatorClass, EXPORT_PARAMS) \
	struct EXPORT_PARAMS allocatorClass##Data     \
	{                                              \
		static HANDLE       ms_hCustomHeap;        \
		static LONG         ms_dwRefs;             \
		static BOOL         ms_bLFHEnabled;        \
		static BOOL         ms_bUseCustomHeap;     \
	};                                             \
	class  EXPORT_PARAMS allocatorClass : public CXTPHeapAllocatorT<allocatorClass##Data> {};
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//      This macros used to declare allocator class derived from CXTPHeapAllocatorT
//      which can use default or custom (separate) heap.
//      Such allocator can be used as parameter for CXTPHeapObjectT template or
//      for other cases.
// Parameters:
//      allocatorClass  - [in] Name of the allocator class.
// Remarks:
//      Used together with XTP_IMPLEMENT_HEAP_ALLOCATOR macro.
// Example:
// <code>
//
//  // probably in header (*.h) file:
//  XTP_DECLARE_HEAP_ALLOCATOR(CXTPReportRowAllocator)
//
//  // in implementation (*.cpp) file:
//  XTP_IMPLEMENT_HEAP_ALLOCATOR(CXTPReportRowAllocator)
//
// </code>
// See Also:
//      XTP_IMPLEMENT_HEAP_ALLOCATOR, CXTPHeapAllocatorT, CXTPHeapObjectT
//===========================================================================
#define XTP_DECLARE_HEAP_ALLOCATOR(allocatorClass) XTP_DECLARE_HEAP_ALLOCATOR_(allocatorClass, XTP_EXPORT_PARAMS_NO)


//===========================================================================
// Summary:
//      This macros used to implement allocator class previously declared by
//      XTP_DECLARE_HEAP_ALLOCATOR macro.
//      Such allocator can be used as parameter for CXTPHeapObjectT template or
//      for other cases.
// Parameters:
//      allocatorClass  - [in] Name of the allocator class.
//      bUseCustomHeap  - [in] Set as TRUE to enable custom heap by default for this allocator and FALSE to disable.
// Remarks:
//      Used together with XTP_DECLARE_HEAP_ALLOCATOR macro.
// Example:
// <code>
//
//  // probably in header (*.h) file:
//  XTP_DECLARE_HEAP_ALLOCATOR(CXTPReportRowAllocator)
//
//  // in implementation (*.cpp) file:
//  XTP_IMPLEMENT_HEAP_ALLOCATOR(CXTPReportRowAllocator)
//
// </code>
// See Also:
//      XTP_DECLARE_HEAP_ALLOCATOR, CXTPHeapAllocatorT, CXTPHeapObjectT
//===========================================================================
#define XTP_IMPLEMENT_HEAP_ALLOCATOR(allocatorClass, bUseCustomHeap)        \
	HANDLE    allocatorClass##Data::ms_hCustomHeap = NULL;  \
	LONG      allocatorClass##Data::ms_dwRefs = 0;          \
	BOOL      allocatorClass##Data::ms_bLFHEnabled = FALSE; \
	BOOL      allocatorClass##Data::ms_bUseCustomHeap = bUseCustomHeap;  \
	allocatorClass g_obj##allocatorClass;


//===========================================================================
// Summary:
//      This template class used as a helper class to override new/delete
//      operators and use custom heap allocators inside them.
// Parameters:
//      _TObject    - A base class;
//      _TAllocator - An allocator class name;
//
// Example:
// <code>
//
//  // *** Allocator must be declared (and implemented)
//  // probably in header (*.h) file:
//  XTP_DECLARE_HEAP_ALLOCATOR(CMyCustomHeapAllocator)
//
//  // in implementation (*.cpp) file:
//  XTP_IMPLEMENT_HEAP_ALLOCATOR(CMyCustomHeapAllocator, TRUE)
//
//  // To enable custom heap allocations use second parameter in
//  // XTP_IMPLEMENT_HEAP_ALLOCATOR macro or set corresponding flag
//  // on initialization, before any allocations:
//  //
//  CMyCustomHeapAllocator::ms_bUseCustomHeap = TRUE;
//
//  //*** One way to use:
//  class CMyClass : public CXTPHeapObjectT<CMyClassBase, CMyCustomHeapAllocator>
//  {
//      // ...
//  };
//  CMyClass* pMyClassObj = new CMyClass();
//
//
//  //*** Other way to use:
//  class CMyClass : public CMyClassBase
//  {
//      // ...
//  };
//
//  class CMyClass_heap : public CXTPHeapObjectT<CMyClass, CMyCustomHeapAllocator>
//  {
//  };
//  CMyClass* pMyClassObj = new CMyClass_heap();
// </code>
//
// See Also:
//      XTP_DECLARE_HEAP_ALLOCATOR, XTP_IMPLEMENT_HEAP_ALLOCATOR,
//===========================================================================
template<class _TObject, class _TAllocator>
class CXTPHeapObjectT : public _TObject
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	typedef _TObject TObject;
	typedef _TAllocator TAllocator;
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//      Allocate memory block of nSize bytes.
	// Returns:
	//      A pointer to allocated block or NULL.
	// See Also:
	//      operator delete
	//-----------------------------------------------------------------------
	void* PASCAL operator new(size_t nSize)
	{
		if (TAllocator::ms_bUseCustomHeap)
			return  TAllocator::Alloc_mem(nSize);
		else
		{
			InterlockedIncrement(&TAllocator::ms_dwRefs); //TAllocator::ms_dwRefs++;
			return ::operator new(nSize);
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Free memory block previously allocated by operator new.
	// Parameters:
	//      p - [in] Pointer to a memory block.
	// See Also:
	//      operator new
	//-----------------------------------------------------------------------
	void PASCAL operator delete(void* p)
	{
		if (TAllocator::ms_bUseCustomHeap)
			TAllocator::Free_mem(p);
		else
		{
			InterlockedDecrement(&TAllocator::ms_dwRefs); //TAllocator::ms_dwRefs--;
			::operator delete(p);
		}
	}

//{{AFX_CODEJOCK_PRIVATE

	//void* PASCAL operator new(size_t, void* p) {return p;} // default is fine as is

	#if _MSC_VER >= 1200
		void PASCAL operator delete(void* p, void* pPlace)
		{
			if (TAllocator::ms_bUseCustomHeap)
				operator delete(p);
			else
			{
				InterlockedDecrement(&TAllocator::ms_dwRefs); //TAllocator::ms_dwRefs--;
				::operator delete(p, pPlace);
			}
		}
	#endif

#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
		// for file name/line number tracking using DEBUG_NEW
		void* PASCAL operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
		{
			if (TAllocator::ms_bUseCustomHeap)
				return operator new(nSize);
			else
			{
				InterlockedIncrement(&TAllocator::ms_dwRefs); //TAllocator::ms_dwRefs++;
				return ::operator new(nSize, lpszFileName, nLine);
			}
		}

	#if _MSC_VER >= 1200
		void PASCAL operator delete(void *p, LPCSTR lpszFileName, int nLine)
		{
			if (TAllocator::ms_bUseCustomHeap)
				operator delete(p);
			else
			{
				InterlockedDecrement(&TAllocator::ms_dwRefs); //TAllocator::ms_dwRefs--;
				::operator delete(p, lpszFileName, nLine);
			}
		}
	#endif
#endif
//}}AFX_CODEJOCK_PRIVATE
};

//===========================================================================
// Summary:
//      This class used to store strings. It automatically allocate/deallocate
//      memory for string data.
// Remarks:
//      Override _AllocStringData, _FreeStringData to change default memory allocation.
// See Also:
//      CXTPHeapStringT, XTP_DECLARE_HEAP_ALLOCATOR, CXTPHeapAllocatorT
//===========================================================================
class _XTP_EXT_CLASS CXTPHeapString
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	// See Also:
	//      ~CXTPHeapString
	//-----------------------------------------------------------------------
	CXTPHeapString()
	{
		m_nStrLen = 0;
		m_pcszString = NULL;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Object constructor.
	// Parameters:
	//      pcszString - [in] Pointer to a source string.
	// See Also:
	//      ~CXTPHeapString
	//-----------------------------------------------------------------------
	CXTPHeapString(LPCTSTR pcszString)
	{
		m_nStrLen = 0;
		m_pcszString = NULL;

		SetString(pcszString);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Default object destructor.
	// See Also:
	//      CXTPHeapString
	//-----------------------------------------------------------------------
	virtual ~CXTPHeapString()
	{
		_FreeStringData();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this function to determine is string empty.
	// Returns:
	//      TRUE if stored string is empty, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsEmpty() const
	{
		return !m_pcszString || m_nStrLen == 0;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Assign new string value.
	// Parameters:
	//      pcszString - [in] Pointer to a source string.
	// Returns:
	//      Stored string value.
	//-----------------------------------------------------------------------
	LPCTSTR operator=(LPCTSTR pcszString)
	{
		SetString(pcszString);
		return m_pcszString ? m_pcszString : _T("");
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Determine are strings equal.
	// Parameters:
	//      pcszString - [in] Pointer to a string to compare.
	// Returns:
	//      TRUE if strings are equal, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL operator==(LPCTSTR pcszString) const
	{
		return 0 == _tcscmp((LPCTSTR)*this, pcszString);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Determine are strings different.
	// Parameters:
	//      pcszString - [in] Pointer to a string to compare.
	// Returns:
	//      TRUE if strings are different, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL operator!=(LPCTSTR pcszString) const
	{
		return 0 != _tcscmp((LPCTSTR)*this, pcszString);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      operator LPCTSTR.
	// Returns:
	//      Pointer to a stored string.
	//-----------------------------------------------------------------------
	operator LPCTSTR() const
	{
		return m_pcszString ? m_pcszString : _T("");
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      operator CString.
	// Returns:
	//      A CString object which contains stored string.
	//-----------------------------------------------------------------------
	operator CString() const
	{
		return CString(m_pcszString ? m_pcszString : _T(""));
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Allocates a BSTR from stored string data..
	// Returns:
	//      The newly allocated string.
	//-----------------------------------------------------------------------
	virtual BSTR AllocSysString() const
	{
		CString str(m_pcszString ? m_pcszString : _T(""));
		return str.AllocSysString();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this method to set new string value.
	// Parameters:
	//      pcszString - [in] Pointer to a source string.
	//-----------------------------------------------------------------------
	virtual void SetString(LPCTSTR pcszString)
	{
		if (!pcszString || *pcszString == 0)
		{
			_FreeStringData();
			return;
		}

		int nNewStrLen = (int)_tcslen(pcszString);

		if (nNewStrLen <= m_nStrLen && m_pcszString)
		{
			STRCPY_S(m_pcszString, m_nStrLen + 1, pcszString);
		}
		else
		{
			_FreeStringData();

			_AllocStringData(nNewStrLen);

			if (m_pcszString)
				STRCPY_S(m_pcszString, m_nStrLen + 1, pcszString);
		}
	}

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      This method is used to allocate string data buffer.
	// Parameters:
	//      nNewStrLen - [in] New string length without null character.
	//-----------------------------------------------------------------------
	virtual void _AllocStringData(int nNewStrLen)
	{
		ASSERT(m_pcszString == NULL);

		m_nStrLen = nNewStrLen;
		m_pcszString = new TCHAR[nNewStrLen + 1];
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      This method is used to free string data buffer previously allocated
	//      in _AllocStringData.
	//-----------------------------------------------------------------------
	virtual void _FreeStringData()
	{
		if (m_pcszString)
			delete m_pcszString;

		m_pcszString = NULL;
		m_nStrLen = 0;
	}

	int m_nStrLen;       // Stored string length (without null character).
	LPTSTR m_pcszString; // Pointer to a stored string (or NULL).
};

//===========================================================================
// Summary:
//      This template used to store strings using different allocators.
// Remarks:
//      String data stored in default or separate (custom) heap.
//      Specially useful for VC 6.0 instead of CString, because
//      CString Release implementation allocates data using some cache
//      and data is not deallocated when CString destroyed.
// See Also:
//      CXTPHeapString, XTP_DECLARE_HEAP_ALLOCATOR, CXTPHeapAllocatorT
//===========================================================================
template<class _TAllocator>
class CXTPHeapStringT : public CXTPHeapString
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	typedef _TAllocator TAllocator;
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	// Parameters:
	//      pcszString - [in] Pointer to a source string.
	// See Also:
	//      ~CXTPHeapStringT
	//-----------------------------------------------------------------------
	CXTPHeapStringT(LPCTSTR pcszString = _T(""))
	{
		SetString(pcszString);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Default object destructor.
	// See Also:
	//      CXTPHeapString
	//-----------------------------------------------------------------------
	virtual ~CXTPHeapStringT()
	{
		_FreeStringData();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Assign new string value.
	// Parameters:
	//      pcszString - [in] Pointer to a source string.
	// Returns:
	//      Stored string value.
	//-----------------------------------------------------------------------
	LPCTSTR operator=(LPCTSTR pcszString)
	{
		SetString(pcszString);
		return m_pcszString ? m_pcszString : _T("");
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Determine are strings equal.
	// Parameters:
	//      pcszString - [in] Pointer to a string to compare.
	// Returns:
	//      TRUE if strings are equal, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL operator==(LPCTSTR pcszString) const
	{
		return 0 == _tcscmp((LPCTSTR)*this, pcszString);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Determine are strings different.
	// Parameters:
	//      pcszString - [in] Pointer to a string to compare.
	// Returns:
	//      TRUE if strings are different, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL operator!=(LPCTSTR pcszString) const
	{
		return 0 != _tcscmp((LPCTSTR)*this, pcszString);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      operator LPCTSTR.
	// Returns:
	//      Pointer to a stored string.
	//-----------------------------------------------------------------------
	operator LPCTSTR() const
	{
		return m_pcszString ? m_pcszString : _T("");
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      operator CString.
	// Returns:
	//      A CString object which contains stored string.
	//-----------------------------------------------------------------------
	operator CString() const
	{
		return CString(m_pcszString ? m_pcszString : _T(""));
	}

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//      This method is used to allocate string data buffer.
	// Parameters:
	//      nNewStrLen - [in] New string length without null character.
	//-----------------------------------------------------------------------
	virtual void _AllocStringData(int nNewStrLen)
	{
		ASSERT(m_pcszString == NULL);

		m_nStrLen = nNewStrLen;
		m_pcszString = (LPTSTR)TAllocator::Alloc_mem((nNewStrLen + 1) * sizeof(TCHAR));
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      This method is used to free string data buffer previously allocated
	//      in _AllocStringData.
	//-----------------------------------------------------------------------
	virtual void _FreeStringData()
	{
		if (m_pcszString)
			TAllocator::Free_mem(m_pcszString);

		m_pcszString = NULL;
		m_nStrLen = 0;
	}
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_CODEJOCK_PRIVATE
struct XTP_BATCHALLOC_OBJ_HEADER;

struct XTP_BATCHALLOC_BLOCK_HEADER
{
	LONG m_dwRefs;

#ifdef _DEBUG
	DWORD m_dwObjCount;
	DWORD m_dwObjSize; // for debug
#endif
	XTP_BATCHALLOC_OBJ_HEADER* pFreeList;

	XTP_BATCHALLOC_BLOCK_HEADER* pPrev;
	XTP_BATCHALLOC_BLOCK_HEADER* pNext;
};

struct XTP_BATCHALLOC_OBJ_HEADER
{
	XTP_BATCHALLOC_BLOCK_HEADER* pBlockHeader;

	XTP_BATCHALLOC_OBJ_HEADER* pNextFree;

	void* GetData()
	{
		BYTE* pData = (BYTE*)this;
		pData += sizeof(XTP_BATCHALLOC_OBJ_HEADER);
		return pData;
	}

	static XTP_BATCHALLOC_OBJ_HEADER* GetHeader(void* pData)
	{
		BYTE* pBlockData = (BYTE*)pData;
		pBlockData -= sizeof(XTP_BATCHALLOC_OBJ_HEADER);
		return (XTP_BATCHALLOC_OBJ_HEADER*)pBlockData;
	}
};

template<class _TAllocator, class _TBatchAllocData>
class CXTPBatchAllocManagerT : public _TBatchAllocData
{
public:
	//{{ AFX_CODEJOCK_PRIVATE
	typedef _TBatchAllocData TBatchAllocData;
	typedef _TAllocator TAllocator;
	//}} AFX_CODEJOCK_PRIVATE

	virtual ~CXTPBatchAllocManagerT()
	{
		// all data must be deallocated before
		ASSERT(m_pBusyBlocks == NULL);

		FreeExtraData();

		ASSERT(m_pFreeBlocks == NULL);

		// This static object destructor may be called later than allocator destructor.
		if (TAllocator::ms_dwRefs == 0)
			TAllocator::ClearHeap();
	}

	AFX_INLINE static int AFX_CDECL _Round4(int nSize)
	{
		int nDiv = nSize / 4;
		int nMod = nSize % 4;
		int nSizeQ4 = (nDiv + (nMod ? 1 : 0)) * 4;
		return nSizeQ4;
	}

	static void* AFX_CDECL AllocData(size_t nSize)
	{
		if (!m_pFreeBlocks)
		{
			int nObjSize = _Round4((int)nSize + sizeof(XTP_BATCHALLOC_OBJ_HEADER));
			int nBlockDataSize = sizeof(XTP_BATCHALLOC_BLOCK_HEADER) +
					m_nBlockSize * nObjSize;

			BYTE* pBlockData = (BYTE*)_TAllocator::Alloc_mem(nBlockDataSize);

			if (!pBlockData)
				return NULL;

			XTP_BATCHALLOC_BLOCK_HEADER* pBlockHdr = (XTP_BATCHALLOC_BLOCK_HEADER*)pBlockData;
			ZeroMemory(pBlockHdr, sizeof(XTP_BATCHALLOC_BLOCK_HEADER));
			pBlockHdr->m_dwRefs = 0;
#ifdef _DEBUG
			pBlockHdr->m_dwObjCount = m_nBlockSize;
			pBlockHdr->m_dwObjSize = (DWORD)nSize;
#endif
			//---------
			m_pFreeBlocks = pBlockHdr;

			XTP_BATCHALLOC_OBJ_HEADER* pObjHdr = NULL;
			pBlockData = pBlockData + sizeof(XTP_BATCHALLOC_BLOCK_HEADER);
			pBlockHdr->pFreeList = (XTP_BATCHALLOC_OBJ_HEADER*)pBlockData;

			for (int i = 0; i < m_nBlockSize; i++)
			{
				pObjHdr = (XTP_BATCHALLOC_OBJ_HEADER*)pBlockData;
				//ZeroMemory(pObjHdr, sizeof(XTP_BATCHALLOC_OBJ_HEADER));

				pBlockData += nObjSize;

				pObjHdr->pBlockHeader = pBlockHdr;
				pObjHdr->pNextFree = (XTP_BATCHALLOC_OBJ_HEADER*)pBlockData;
			}
			pObjHdr->pNextFree = NULL;
		}

		ASSERT(m_pFreeBlocks && m_pFreeBlocks->pFreeList);
		if (!m_pFreeBlocks || !m_pFreeBlocks->pFreeList)
			return NULL;

		ASSERT(m_pFreeBlocks->m_dwObjSize == (DWORD)nSize);

		void* pData = m_pFreeBlocks->pFreeList->GetData();

		m_pFreeBlocks->m_dwRefs++;
		m_dwAllocatedObjects++;

		XTP_BATCHALLOC_OBJ_HEADER* pNextFree = m_pFreeBlocks->pFreeList->pNextFree;
		m_pFreeBlocks->pFreeList->pNextFree = NULL;
		m_pFreeBlocks->pFreeList = pNextFree;

		// no more free objects, move to busy blocks
		if (!pNextFree)
		{
			XTP_BATCHALLOC_BLOCK_HEADER* pNewBusyBlock = m_pFreeBlocks;
			m_pFreeBlocks = m_pFreeBlocks->pPrev ? m_pFreeBlocks->pPrev : m_pFreeBlocks->pNext;
			if (m_pFreeBlocks)
				m_pFreeBlocks->pPrev = pNewBusyBlock->pPrev;

			pNewBusyBlock->pNext = m_pBusyBlocks;
			pNewBusyBlock->pPrev = m_pBusyBlocks ? m_pBusyBlocks->pPrev : NULL;
			if (m_pBusyBlocks)
				m_pBusyBlocks->pPrev = pNewBusyBlock;
			m_pBusyBlocks = pNewBusyBlock;
		}

		return pData;
	}

	static void AFX_CDECL FreeData(void* pObj)
	{
		if (!pObj)
			return;

		XTP_BATCHALLOC_OBJ_HEADER* pObjHdr = XTP_BATCHALLOC_OBJ_HEADER::GetHeader(pObj);
		pObjHdr->pNextFree = pObjHdr->pBlockHeader->pFreeList;
		pObjHdr->pBlockHeader->pFreeList = pObjHdr;

		pObjHdr->pBlockHeader->m_dwRefs--;
		m_dwAllocatedObjects--;

		// was busy block, move to free list
		if (pObjHdr->pNextFree == NULL)
		{
			XTP_BATCHALLOC_BLOCK_HEADER* pNewFreeBlock = pObjHdr->pBlockHeader;
			if (pNewFreeBlock->pPrev)
				pNewFreeBlock->pPrev->pNext = pNewFreeBlock->pNext;

			if (pNewFreeBlock->pNext)
				pNewFreeBlock->pNext->pPrev = pNewFreeBlock->pPrev;

			if (m_pBusyBlocks == pNewFreeBlock)
				m_pBusyBlocks = pNewFreeBlock->pPrev ? pNewFreeBlock->pPrev : pNewFreeBlock->pNext;

			pNewFreeBlock->pNext = m_pFreeBlocks;
			pNewFreeBlock->pPrev = m_pFreeBlocks ? m_pFreeBlocks->pPrev : NULL;
			if (m_pFreeBlocks)
				m_pFreeBlocks->pPrev = pNewFreeBlock;
			m_pFreeBlocks = pNewFreeBlock;
		}

		// block is totally free, destroy block
		if (pObjHdr->pBlockHeader->m_dwRefs == 0 && m_bDestroyEmptyBlocksOnFree)
		{
			XTP_BATCHALLOC_BLOCK_HEADER* pEmptyBlock = pObjHdr->pBlockHeader;

			XTP_BATCHALLOC_BLOCK_HEADER* pIsNextFree = pEmptyBlock->pPrev ? pEmptyBlock->pPrev : pEmptyBlock->pNext;
			BOOL bLast = (m_pFreeBlocks == pEmptyBlock) && pIsNextFree;

			// do not destroy last free block
			if (!bLast || m_bDestroyLastEmptyBlockOnFree)
			{
				if (pEmptyBlock->pPrev)
					pEmptyBlock->pPrev->pNext = pEmptyBlock->pNext;
				if (pEmptyBlock->pNext)
					pEmptyBlock->pNext->pPrev = pEmptyBlock->pPrev;

				if (m_pFreeBlocks == pEmptyBlock)
					m_pFreeBlocks = pEmptyBlock->pPrev ? pEmptyBlock->pPrev : pEmptyBlock->pNext;

				_TAllocator::Free_mem(pEmptyBlock);
			}
		}

//      if (m_dwAllocatedObjects == 0)
//          FreeExtraData();
	}

public:
	static void AFX_CDECL FreeExtraData()
	{
		XTP_BATCHALLOC_BLOCK_HEADER* pBlock = m_pFreeBlocks;
		while (pBlock && pBlock->pPrev)
		{
			pBlock = pBlock->pPrev;
		}

		while (pBlock)
		{
			// block is totally free, destroy block
			if (pBlock->m_dwRefs == 0)
			{
				XTP_BATCHALLOC_BLOCK_HEADER* pEmptyBlock = pBlock;

				if (pEmptyBlock->pPrev)
					pEmptyBlock->pPrev->pNext = pEmptyBlock->pNext;

				if (pEmptyBlock->pNext)
					pEmptyBlock->pNext->pPrev = pEmptyBlock->pPrev;

				if (m_pFreeBlocks == pEmptyBlock)
					m_pFreeBlocks = pEmptyBlock->pPrev ? pEmptyBlock->pPrev : pEmptyBlock->pNext;

				pBlock = pBlock->pNext;

				TAllocator::Free_mem(pEmptyBlock);
			}
			else
			{
				pBlock = pBlock->pNext;
			}
		}
	}
};
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//      This template class used as a helper class to override new/delete
//      operators and use batch allocation inside them.
//      Batch allocation means that memory allocated not for one object only,
//      but for many objects at one time (for 1024 objects by default).
//      Next allocations take memory from this big block. New blocks allocated
//      when necessary. This increase performance and reduce heap fragmentation.
//      Batch allocation mechanism responsible for allocation/deallocation
//      blocks of memory from heap and internally organize free/busy lists of
//      memory pieces. When object deleted, its memory stored in free list and
//      used for new objects.
//      When all memory pieces from block free, it may be deallocated from
//      heap automatically (this depends on options in _TBatchAllocData)
//      or by FreeExtraData call,
//
// Parameters:
//      _TObject         - A base class;
//      _TAllocator      - An allocator class name; by default _TObject::TAllocator is used.
//      _TBatchAllocData - This class must contain following static members for allocator:
//
//                          struct BatchAllocData
//                          {
//                              static BOOL m_bEnableBatchAllocation;   // Define is Batch Allocation enabled;
//                              static LONG m_dwAllocatedObjects;       // allocated blocks count;
//
//                              static BOOL m_bDestroyEmptyBlocksOnFree;    // if TRUE - completely free blocks will be deallocated on free objects, otherwise they will stay in free list.
//                              static BOOL m_bDestroyLastEmptyBlockOnFree; // if TRUE - last completely free block will be deallocated on free objects, otherwise it will stay in free list.
//
//                              static int  m_nBlockSize;               // Count of objects in block.
//                          protected:
//                              static XTP_BATCHALLOC_BLOCK_HEADER* m_pFreeBlocks;  // List of blocks which have free pieces.
//                              static XTP_BATCHALLOC_BLOCK_HEADER* m_pBusyBlocks;  // List of blocks which have not free pieces.
//                          };
//
// Example:
// <code>
//
//  // *** Batch data must be declared (and implemented)
//  // probably in header (*.h) file:
//  XTP_DECLARE_BATCH_ALLOC_OBJ_DATA(CBatchReportRecord_Data);
//  class CBatchReportRecord : public CXTPBatchAllocObjT<CXTPReportRecord, CBatchReportRecord_Data>
//  {
//      // ...
//  };
//
//  // in implementation (*.cpp) file:
//  XTP_IMPLEMENT_BATCH_ALLOC_OBJ_DATA(CBatchReportRecord_Data, CBatchReportRecord, TRUE);
//
//  // To enable Batch allocations use second parameter in
//  // XTP_IMPLEMENT_BATCH_ALLOC_OBJ_DATA macro or set corresponding flag
//  // on initialization, before any allocations:
//  //
//  CMyCustomHeapAllocator::ms_bUseCustomHeap = TRUE;
//
//  //*** How to use:
//  CBatchReportRecord* pMyClassObj = new CBatchReportRecord();
//
//</code>
//
// See Also:
//      XTP_DECLARE_BATCH_ALLOC_OBJ_DATA, XTP_IMPLEMENT_BATCH_ALLOC_OBJ_DATA,
//===========================================================================
template<class _TObject, class _TBatchAllocData, class _TAllocator = _TObject::TAllocator >
class CXTPBatchAllocObjT : public _TObject
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	typedef _TObject TObject;
	typedef _TBatchAllocData TBatchAllocData;
	typedef _TAllocator TAllocator;

	typedef CXTPBatchAllocManagerT<_TAllocator, _TBatchAllocData > TBlockMan;
	//}}AFX_CODEJOCK_PRIVATE

public:

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function check all blocks and deallocate which are
	//      completely free.
	// See Also:
	//      _TBatchAllocData
	//-----------------------------------------------------------------------
	static void AFX_CDECL FreeExtraData() {
		TBlockMan::FreeExtraData();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Allocate memory block of nSize bytes.
	// Returns:
	//      A pointer to allocated block or NULL.
	// See Also:
	//      operator delete
	//-----------------------------------------------------------------------
	void* PASCAL operator new(size_t nSize)
	{
		if (TBlockMan::m_bBatchAllocationEnabled)
			return TBlockMan::AllocData(nSize);
		else
			return _TObject::operator new(nSize);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Free memory block previously allocated by operator new.
	// Parameters:
	//      p - [in] Pointer to a memory block.
	// See Also:
	//      operator new
	//-----------------------------------------------------------------------
	void PASCAL operator delete(void* p)
	{
		if (TBlockMan::m_bBatchAllocationEnabled)
			TBlockMan::FreeData(p);
		else
			_TObject::operator delete(p);
	}


//{{AFX_CODEJOCK_PRIVATE
	//void* PASCAL operator new(size_t, void* p) {return p;} // default is fine as is

	#if _MSC_VER >= 1200
		void PASCAL operator delete(void* p, void* pPlace)
		{
			if (TBlockMan::m_bBatchAllocationEnabled)
				TBlockMan::FreeData(p);
			else
				_TObject::operator delete(p, pPlace);
		}
	#endif

#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
		// for file name/line number tracking using DEBUG_NEW
		void* PASCAL operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
		{
			if (TBlockMan::m_bBatchAllocationEnabled)
				return operator new(nSize);
			else
				return _TObject::operator new(nSize, lpszFileName, nLine);
		}

	#if _MSC_VER >= 1200
		void PASCAL operator delete(void *p, LPCSTR lpszFileName, int nLine)
		{
			if (TBlockMan::m_bBatchAllocationEnabled)
				operator delete(p);
			else
				_TObject::operator delete(p, lpszFileName, nLine);
		}
	#endif
#endif
//}}AFX_CODEJOCK_PRIVATE
};

//{{AFX_CODEJOCK_PRIVATE
#define XTP_DECLARE_BATCH_ALLOC_OBJ_DATA_(dataClass, EXPORT_PARAMS) \
	struct EXPORT_PARAMS dataClass                  \
	{                                               \
		static BOOL m_bBatchAllocationEnabled;      \
		static LONG m_dwAllocatedObjects;           \
		static BOOL m_bDestroyEmptyBlocksOnFree;    \
		static BOOL m_bDestroyLastEmptyBlockOnFree; \
		static int  m_nBlockSize;                   \
													\
		static BOOL AFX_CDECL IsDataEmpty() { return !m_pFreeBlocks && !m_pBusyBlocks; };   \
	protected:                                      \
		static XTP_BATCHALLOC_BLOCK_HEADER* m_pFreeBlocks;  \
		static XTP_BATCHALLOC_BLOCK_HEADER* m_pBusyBlocks;  \
	};
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//      This macros used to declare Batch allocation data which used with
//      CXTPBatchAllocObjT template.
// Parameters:
//      dataClass   - [in] Name of the Batch allocation data class.
// Remarks:
//      Used together with XTP_IMPLEMENT_BATCH_ALLOC_OBJ_DATA macro.
// Example:
// <code>
//
//  XTP_DECLARE_BATCH_ALLOC_OBJ_DATA(CBatchReportRecord_Data);
//  class CBatchReportRecord : public CXTPBatchAllocObjT<CXTPReportRecord, CBatchReportRecord_Data>
//  {
//      // ...
//  };
//
//  // in implementation (*.cpp) file:
//  XTP_IMPLEMENT_BATCH_ALLOC_OBJ_DATA(CBatchReportRecord_Data, CBatchReportRecord, TRUE);
//
// </code>
// See Also:
//      XTP_IMPLEMENT_HEAP_ALLOCATOR, CXTPHeapAllocatorT, CXTPBatchAllocObjT
//===========================================================================
#define XTP_DECLARE_BATCH_ALLOC_OBJ_DATA(dataClass) XTP_DECLARE_BATCH_ALLOC_OBJ_DATA_(dataClass, XTP_EXPORT_PARAMS_NO)

//===========================================================================
// Summary:
//      This macros used to declare Batch allocation data which used with
//      CXTPBatchAllocObjT template.
// Parameters:
//      dataClass           - [in] Name of the Batch allocation data class.
//      objClass            - [in] Name of the object class for Batch allocation.
//      batchAllocEnabled   - [in] Set as TRUE to enable Batch allocation by default for this object and FALSE to disable.
// Remarks:
//      Used together with XTP_DECLARE_BATCH_ALLOC_OBJ_DATA macro.
// Example:
// <code>
//
//  XTP_DECLARE_BATCH_ALLOC_OBJ_DATA(CBatchReportRecord_Data);
//  class CBatchReportRecord : public CXTPBatchAllocObjT<CXTPReportRecord, CBatchReportRecord_Data>
//  {
//      // ...
//  };
//
//  // in implementation (*.cpp) file:
//  XTP_IMPLEMENT_BATCH_ALLOC_OBJ_DATA(CBatchReportRecord_Data, CBatchReportRecord, TRUE);
//
// </code>
// See Also:
//      XTP_DECLARE_BATCH_ALLOC_OBJ_DATA, CXTPHeapAllocatorT, CXTPBatchAllocObjT
//===========================================================================
#define XTP_IMPLEMENT_BATCH_ALLOC_OBJ_DATA(dataClass, objClass, batchAllocEnabled)  \
	BOOL dataClass::m_bBatchAllocationEnabled       = batchAllocEnabled; \
	LONG dataClass::m_dwAllocatedObjects            = 0;        \
	int  dataClass::m_nBlockSize                    = 1024;     \
	BOOL dataClass::m_bDestroyEmptyBlocksOnFree     = FALSE;    \
	BOOL dataClass::m_bDestroyLastEmptyBlockOnFree  = FALSE;    \
	XTP_BATCHALLOC_BLOCK_HEADER* dataClass::m_pFreeBlocks       = NULL; \
	XTP_BATCHALLOC_BLOCK_HEADER* dataClass::m_pBusyBlocks       = NULL; \
	objClass::TBlockMan gs_##objClass##_BlocksManager;

#endif // !defined(_XTPCUSTOMHEAP_H__)
