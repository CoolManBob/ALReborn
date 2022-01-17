// XTPSyntaxEditLexClass.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// common includes
#include "Common/XTPColorManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPVC50Helpers.h"

// syntax editor includes
#include "XTPSyntaxEditDefines.h"
#include "XTPSyntaxEditStruct.h"
#include "XTPSyntaxEditLexPtrs.h"
#include "XTPSyntaxEditLexClassSubObjT.h"
#include "XTPSyntaxEditTextIterator.h"
#include "XTPSyntaxEditSectionManager.h"
#include "XTPSyntaxEditLexCfgFileReader.h"
#include "XTPSyntaxEditLexClassSubObjDef.h"
#include "XTPSyntaxEditLexClass.h"
#include "XTPSyntaxEditLexParser.h"
#include "XTPSyntaxEditTextIterator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define TRACE_ASSERT ASSERT
#define TRACE_ASSERT(x)

//#define TRACE_MEMORY_LEX_AUTOMAT_MEM_MAN

////////////////////////////////////////////////////////////////////////////
const int c_nPrevClassDepth_default = 1;

using namespace XTPSyntaxEditLexAnalyser;

namespace XTPSyntaxEditLexAnalyser
{
	CXTPSyntaxEditLexAutomatMemMan g_LexAutomatMemMan;

	CXTPSyntaxEditLexAutomatMemMan* XTPGetLexAutomatMemMan()
	{
		return &g_LexAutomatMemMan;
	}
	////////////////////////////////////////////////////////////////////////////

	static const LPCTSTR cszSpecs = _T("~`!@#$%^&*()_-+=\\|{}[];:'\",.<>/?");
	//---------------------------------------------------------------------------
	const TCHAR* g_arKnownLexClassAttribs[] =
	{
		XTPLEX_ATTR_TXT_COLORFG,
		XTPLEX_ATTR_TXT_COLORBK,
		XTPLEX_ATTR_TXT_COLORSELFG,
		XTPLEX_ATTR_TXT_COLORSELBK,
		XTPLEX_ATTR_TXT_BOLD,
		XTPLEX_ATTR_TXT_ITALIC,
		XTPLEX_ATTR_TXT_UNDERLINE,
		XTPLEX_ATTR_CASESENSITIVE,
		XTPLEX_ATTR_COLLAPSABLE,
		XTPLEX_ATTR_COLLAPSEDTEXT,
		XTPLEX_ATTR_PARSEONSCREEN,
		XTPLEX_ATTR_RESTARTRUNLOOP,
		XTPLEX_ATTR_ENDCLASSPARENT,
		XTPLEX_ATTR_RECURRENCEDEPTH,
		XTPLEX_ATTR_DISPLAYNAME,

		// Global attributes
		XTPLEX_ATTRG_FIRSTPARSEINSEPARATETHREAD,
		XTPLEX_ATTRG_EDITREPARCEINSEPARATETHREAD,
		XTPLEX_ATTRG_CONFIGCHANGEDREPARCEINSEPARATETHREAD,
		XTPLEX_ATTRG_EDITREPARCETIMEOUT_MS,
		XTPLEX_ATTRG_MAXBACKPARSEOFFSET,
		XTPLEX_ATTRG_ONSCREENSCHCACHELIFETIME_SEC,
		XTPLEX_ATTRG_PARSERTHREADIDLELIFETIME_SEC,
	};

	extern int FindStr(const CStringArray& rarData, LPCTSTR pcszStr, BOOL bCase = FALSE);
	extern int Find_noCase(CStringArray& rarData, LPCTSTR strData);
	//===========================================================================
	BOOL SplitStr(LPCTSTR pcszStr, TCHAR chSplitter, CStringArray& rArProps)
	{
		rArProps.RemoveAll();
		CString str0(pcszStr);
		CString strSplitter(chSplitter);

		CString str;
		do {
			str = str0.SpanExcluding(strSplitter);

			int nLen = str.GetLength();
			int nLenMax = str0.GetLength();

			if (nLen)
			{
				DELETE_S(str0, 0, min(nLen+1, nLenMax));
				rArProps.Add(str);
			}
		}
		while (!str.IsEmpty());

		return TRUE;
	}

	//---------------------------------------------------------------------------
	BOOL PropPathSplit(LPCTSTR pcszPropPath, CStringArray& rArProps)
	{
		return SplitStr(pcszPropPath, _T(':'), rArProps);
	}

	//---------------------------------------------------------------------------
	CString MakeStr(const CStringArray& rArProps, LPCTSTR strSplitter)
	{
		CString strResult;
		int nCount = (int)rArProps.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			if (i)
			{
				strResult += strSplitter;
			}
			CString strI = rArProps[i];
			strResult += strI;
		}
		return strResult;
	}

	//---------------------------------------------------------------------------
	CString PropPathFirstRemove(CString& rStrPropPath)
	{
		CString strProp;

		strProp = rStrPropPath.SpanExcluding(_T(":"));

		int nLen = strProp.GetLength();
		int nLenMax = rStrPropPath.GetLength();

		if (nLen)
		{
			DELETE_S(rStrPropPath, 0, min(nLen+1, nLenMax));
		}

		return strProp;
	}

	//---------------------------------------------------------------------------
	int PropPathCount(LPCTSTR pcszPropPath)
	{
		CString strPP(pcszPropPath);

		int nCount = 1;

		int nLenMax = strPP.GetLength();

		for (int nFIndex = 0; nFIndex >= 0 && nFIndex < nLenMax;)
		{
			nFIndex = FIND_S(strPP, _T(':'), nFIndex);
			if (nFIndex >= 0)
			{
				nFIndex++;
				nCount++;
			}
		}

		return nCount;
	}

	//---------------------------------------------------------------------------
	void AddIfNeed(CXTPSyntaxEditLexVariantPtrArray* pArDest, CXTPSyntaxEditLexVariant* pLVar)
	{
		if (!pArDest || !pLVar)
		{
			ASSERT(FALSE);
			return;
		}

		int nCount = (int)pArDest->GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CXTPSyntaxEditLexVariant* pLV_i = pArDest->GetAt(i, FALSE);
			ASSERT(pLV_i);
			if (pLV_i && *pLV_i == *pLVar)
			{
				return;
			}
		}

		//-----------
		CXTPSyntaxEditLexVariantPtr ptrLV(pLVar, TRUE);
		pArDest->Add(ptrLV);
	}

	//---------------------------------------------------------------------------
	void ConcatenateLVArrays(CXTPSyntaxEditLexVariantPtrArray* pArDest,
							 CXTPSyntaxEditLexVariantPtrArray* pAr2, int nMaxCount = INT_MAX)
	{
		if (!pArDest || !pAr2)
		{
			ASSERT(FALSE);
			return;
		}
		int nCount = min(nMaxCount, (int)pAr2->GetSize());
		for (int i = 0; i < nCount; i++)
		{
			CXTPSyntaxEditLexVariant* pLVar = pAr2->GetAt(i, FALSE);
			AddIfNeed(pArDest, pLVar);
		}
	}

	//---------------------------------------------------------------------------
	int FindStr(CXTPSyntaxEditLexVariantPtrArray* pAr, LPCTSTR pcszStr, BOOL bCase = FALSE, int* pnStrIdx = NULL)
	{
		if (!pAr || !pcszStr)
		{
			ASSERT(FALSE);
			return -1;
		}

		int nCount = (int)pAr->GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CXTPSyntaxEditLexVariant* pLV_i = pAr->GetAt(i, FALSE);
			ASSERT(pLV_i && pLV_i->IsStrType());

			if (pLV_i && pLV_i->IsStrType())
			{
				int nIdx2 = FindStr(pLV_i->m_arStrVals, pcszStr, bCase);
				if (nIdx2 >= 0)
				{
					if (pnStrIdx)
					{
						*pnStrIdx = nIdx2;
					}
					return i;
				}
			}
		}
		return -1;
	}

	AFX_INLINE int StrCmp_sort(const CString& rStr1, const CString& rStr2,
		BOOL bAscending, BOOL bNoCase)
	{
		int nCmpRes = 0;

		if (bNoCase)
		{
			nCmpRes = rStr1.CompareNoCase(rStr2);
		}
		else
		{
			nCmpRes = rStr1.Compare(rStr2);
		}

		if (!bAscending)
		{
			nCmpRes *= -1;
		}
		return nCmpRes;
	}

	void SortArray(CStringArray& rarData, BOOL bAscending, BOOL bNoCase)
	{
		CString strTmp;

		int nCount = (int)rarData.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			for (int j = i + 1; j < nCount; j++)
			{
				int nCmpRes = StrCmp_sort(rarData[i], rarData[j], bAscending, bNoCase);
				if (nCmpRes > 0)
				{
					strTmp = rarData[i];
					rarData[i] = rarData[j];
					rarData[j] = strTmp;
				}
			}
		}
	}

	int _cdecl StrCmp_qsort_AnC(const void* p1, const void* p2)
	{
		return StrCmp_sort(*((CString*)p1), *((CString*)p2), TRUE, TRUE);
	}
	int _cdecl StrCmp_qsort_AC(const void* p1, const void* p2)
	{
		return StrCmp_sort(*((CString*)p1), *((CString*)p2), TRUE, FALSE);
	}
	int _cdecl StrCmp_qsort_DnC(const void* p1, const void* p2)
	{
		return StrCmp_sort(*((CString*)p1), *((CString*)p2), FALSE, TRUE);
	}
	int _cdecl StrCmp_qsort_DC(const void* p1, const void* p2)
	{
		return StrCmp_sort(*((CString*)p1), *((CString*)p2), FALSE, FALSE);
	}

	void QSortArray(CStringArray& rarData, BOOL bAscending, BOOL bNoCase)
	{
		int nCount = (int)rarData.GetSize();
		if (nCount <= 0)
		{
			return;
		}

		//--------------------------------------------------------------------
		typedef int (_cdecl* TPFNStrCmp)(const void* p1, const void* p2);

		TPFNStrCmp pfnStrCmp = NULL;
		if (bAscending && bNoCase)
		{
			pfnStrCmp = StrCmp_qsort_AnC;
		}
		else if (bAscending && !bNoCase)
		{
			pfnStrCmp = StrCmp_qsort_AC;
		}
		else if (!bAscending && bNoCase)
		{
			pfnStrCmp = StrCmp_qsort_DnC;
		}
		else if (!bAscending && !bNoCase)
		{
			pfnStrCmp = StrCmp_qsort_DC;
		}

		//--------------------------------------------------------------------
		void* pArray = (void*)&rarData[0];
		int nDataSize = sizeof(CString*);

		qsort(pArray, nCount, nDataSize, pfnStrCmp);
	}

	BOOL SortTagsInLexVarArray(CXTPSyntaxEditLexVariantPtrArray& rarTags,
								BOOL bAscending, BOOL bNoCase)
	{
		CXTPSyntaxEditLexVariantPtrArray arData;
		CXTPSyntaxEditLexVariant lvStrs;

		int nCount0 = (int)rarTags.GetSize();
		for (int i = 0; i < nCount0; i++)
		{
			CXTPSyntaxEditLexVariant* pLV = rarTags.GetAt(i, FALSE);
			ASSERT(pLV);
			if (pLV)
			{
				if (!(pLV->m_nObjType == xtpEditLVT_valStr || pLV->m_nObjType == xtpEditLVT_valVar))
				{
					TRACE(_T("ERROR! Tag should be string or variable. \n"));
				}

				if (pLV->m_nObjType == xtpEditLVT_valStr)
				{
					lvStrs.m_nObjType = xtpEditLVT_valStr;
					lvStrs.m_arStrVals.Append(pLV->m_arStrVals);
				}
				else
				{
					CXTPSyntaxEditLexVariant* pClone = pLV->Clone();
					if (pClone)
					{
						arData.AddPtr(pClone, FALSE);
					}
					else
					{
						return FALSE;
					}
				}
			}
		}

		//------------------------------------------------------------------------
		if (lvStrs.m_nObjType == xtpEditLVT_valStr)
		{
			QSortArray(lvStrs.m_arStrVals, bAscending, bNoCase);

			CXTPSyntaxEditLexVariantPtr ptrClone = lvStrs.Clone();
			if (ptrClone)
			{
				arData.InsertAt(0, ptrClone);
			}
			else
			{
				return FALSE;
			}
		}

		//====================================================================
		rarTags.RemoveAll();
		rarTags.Append(arData);

		return TRUE;
	}
}

////////////////////////////////////////////////////////////////////////////
//struct LA_CHAR_NODE

LA_CHAR_NODE::LA_CHAR_NODE()
{
	wChar = 0;
	wFlags = 0;
	pNextMap = NULL;

	pNextNode = NULL;
}

LA_CHAR_NODE::~LA_CHAR_NODE()
{
//  if (pNextMap

//      pNextMap->InternalRelease();
//  }
}

void LA_CHAR_NODE::Clear()
{
	wChar = 0;
	wFlags = 0;
	pNextNode = NULL;

	if (pNextMap)
	{
		pNextMap->InternalRelease();
		pNextMap = NULL;
	}
}

const LA_CHAR_NODE& LA_CHAR_NODE::operator=(const LA_CHAR_NODE& rSrc)
{
	if (rSrc.pNextMap)
	{
		rSrc.pNextMap->InternalAddRef();
	}
	if (pNextMap)
	{
		pNextMap->InternalRelease();
	}

	wChar = rSrc.wChar;
	wFlags = rSrc.wFlags;
	pNextMap = rSrc.pNextMap;

	pNextNode = rSrc.pNextNode;

	return *this;
}

////////////////////////////////////////////////////////////////////////////

CXTPSyntaxEditLexAutomatMemMan::CXTPSyntaxEditLexAutomatMemMan()
{
	m_lLockCount = 0;

	m_pFreeMaps = NULL;
	m_pFreeNodes = NULL;
	m_pFreeTables = NULL;

	m_uAllocatedMaps = 0;
	m_uUsedMaps = 0;

	m_uAllocatedNodes = 0;
	m_uUsedNodes = 0;

	m_uAllocatedTables = 0;
	m_uUsedTables = 0;
	m_uAllocatedTablesBytes = 0;
	m_uUsedTablesBytes = 0;
}

CXTPSyntaxEditLexAutomatMemMan::~CXTPSyntaxEditLexAutomatMemMan()
{
	FreeAll();
}

DWORD CXTPSyntaxEditLexAutomatMemMan::Lock()
{
	return ::InterlockedIncrement(&m_lLockCount);
}

DWORD CXTPSyntaxEditLexAutomatMemMan::Unlok()
{
	if (m_lLockCount <=0)
	{
		ASSERT(FALSE);
		return 0;
	}

	LONG lResult = ::InterlockedDecrement(&m_lLockCount);
	if (0 == lResult)
	{
		FreeAll();
		return 0;
	}
	return (DWORD)lResult;
}

void CXTPSyntaxEditLexAutomatMemMan::FreeAll()
{
	ASSERT(m_lLockCount == 0);

	POSITION pos;

	//- 1 -----------------------------------------------
	pos = m_allocatedNodes.GetHeadPosition();
	while (pos)
	{
		delete[] m_allocatedNodes.GetNext(pos);
	}
	m_allocatedNodes.RemoveAll();
	//- 2 -----------------------------------------------
	pos = m_allocatedMaps.GetHeadPosition();
	while (pos)
	{
		delete[] m_allocatedMaps.GetNext(pos);
	}
	m_allocatedMaps.RemoveAll();
	//- 3 -----------------------------------------------
	pos = m_allocatedTables.GetHeadPosition();
	while (pos)
	{
		delete m_allocatedTables.GetNext(pos);
	}
	m_allocatedTables.RemoveAll();

	m_pFreeMaps = NULL;
	m_pFreeNodes = NULL;
	m_pFreeTables = NULL;

	m_uAllocatedMaps = 0;
	m_uUsedMaps = 0;

	m_uAllocatedNodes = 0;
	m_uUsedNodes = 0;

	m_uAllocatedTables = 0;
	m_uUsedTables = 0;
	m_uAllocatedTablesBytes = 0;
	m_uUsedTablesBytes = 0;
}

CXTPSyntaxEditLexAutomatWordsMap* CXTPSyntaxEditLexAutomatMemMan::NewMap(UINT uHashTableSize)
{
	if (m_pFreeMaps)
	{
		m_uUsedMaps += 1;

		CXTPSyntaxEditLexAutomatWordsMap* pMap = m_pFreeMaps;
		m_pFreeMaps = m_pFreeMaps->pNextFreeObj;
		pMap->pNextFreeObj = NULL;

		pMap->InitMap(uHashTableSize);
		return pMap;
	}

	int nObjCount = 1024/sizeof(CXTPSyntaxEditLexAutomatWordsMap);
	m_pFreeMaps = new CXTPSyntaxEditLexAutomatWordsMap[nObjCount];

	if (!m_pFreeMaps)
	{
		return NULL;
	}
	m_allocatedMaps.AddTail(m_pFreeMaps);

	m_uAllocatedMaps += nObjCount;

	for (int i = 1; i < nObjCount; i++)
	{
		m_pFreeMaps[i-1].pNextFreeObj = &m_pFreeMaps[i];
	}

	return NewMap(uHashTableSize);
}

LA_CHAR_NODE* CXTPSyntaxEditLexAutomatMemMan::NewNode()
{
	if (m_pFreeNodes)
	{
		m_uUsedNodes += 1;

		LA_CHAR_NODE* pObj = m_pFreeNodes;
		m_pFreeNodes = m_pFreeNodes->pNextNode;
		pObj->pNextNode = NULL;

		return pObj;
	}

	int nObjCount = 1024/sizeof(LA_CHAR_NODE);
	m_pFreeNodes = new LA_CHAR_NODE[nObjCount];

	if (!m_pFreeNodes)
	{
		return NULL;
	}
	m_allocatedNodes.AddTail(m_pFreeNodes);
	m_uAllocatedNodes += nObjCount;

	for (int i = 1; i < nObjCount; i++)
	{
		m_pFreeNodes[i-1].pNextNode = &m_pFreeNodes[i];
	}

	return NewNode();
}

PLA_CHAR_NODE* CXTPSyntaxEditLexAutomatMemMan::NewHashTable(UINT uSize)
{
	ASSERT(uSize >= 1);
	if (m_pFreeTables)
	{
		// try find object with the same size
		LA_HASH_TABLE** ppHTabPrev = &m_pFreeTables;
		LA_HASH_TABLE* pHTab = m_pFreeTables;
		while (pHTab)
		{
			if (pHTab->uSize == uSize)
			{
				*ppHTabPrev = pHTab->pNextFreeObj;
				pHTab->pNextFreeObj = NULL;

				m_uUsedTables += 1;
				m_uUsedTablesBytes += uSize * sizeof(PLA_CHAR_NODE) + LA_HASH_TABLE::GetHeaderSizeB();

				return pHTab->GetData();
			}
			ppHTabPrev = &(pHTab->pNextFreeObj);
			ASSERT(pHTab != pHTab->pNextFreeObj);
			pHTab = pHTab->pNextFreeObj;
		}

		// try find bigger object and cut it
		ppHTabPrev = &m_pFreeTables;
		pHTab = m_pFreeTables;
		UINT uHeaderSize = LA_HASH_TABLE::GetHeaderSize();
		while (pHTab)
		{
			if (pHTab->uSize >= uSize)
			{
				if (pHTab->uSize - uSize > uHeaderSize)
				{
					LA_HASH_TABLE* pHTabNext = (LA_HASH_TABLE*) (pHTab->GetData()+ uSize);
					pHTabNext->uSize = pHTab->uSize - uSize - uHeaderSize;
					pHTab->uSize = uSize;

					ASSERT(pHTabNext->uSize > 0);
					ASSERT(pHTab != pHTab->pNextFreeObj);
					pHTabNext->pNextFreeObj = pHTab->pNextFreeObj;

					*ppHTabPrev = pHTabNext;
					pHTab->pNextFreeObj = NULL;

					m_uAllocatedTables += 1;
				}
				else
				{
					ASSERT(pHTab != pHTab->pNextFreeObj);
					*ppHTabPrev = pHTab->pNextFreeObj;
					pHTab->pNextFreeObj = NULL;
				}

				m_uUsedTables += 1;
				m_uUsedTablesBytes += uSize * sizeof(PLA_CHAR_NODE) + LA_HASH_TABLE::GetHeaderSizeB();

				return pHTab->GetData();
			}
			ppHTabPrev = &pHTab->pNextFreeObj;
			ASSERT(pHTab != pHTab->pNextFreeObj);
			pHTab = pHTab->pNextFreeObj;
		}
	}

	//=== Allocate new block ===
	UINT uDataSize = 1024 * uSize;
	LA_HASH_TABLE*  pNewTable = (LA_HASH_TABLE*) new PLA_CHAR_NODE[uDataSize];
	if (!pNewTable)
	{
		return NULL;
	}
	memset(pNewTable, 0, uDataSize * sizeof(PLA_CHAR_NODE));

	pNewTable->uSize = uDataSize - LA_HASH_TABLE::GetHeaderSize();
	pNewTable->pNextFreeObj = m_pFreeTables;
	m_pFreeTables = pNewTable;

	ASSERT(m_pFreeTables != m_pFreeTables->pNextFreeObj);

	m_allocatedTables.AddTail((PLA_CHAR_NODE*)pNewTable);

	m_uAllocatedTables += 1;
	m_uAllocatedTablesBytes += uDataSize * sizeof(PLA_CHAR_NODE);

	return NewHashTable(uSize);
}

void CXTPSyntaxEditLexAutomatMemMan::FreeObject(CXTPSyntaxEditLexAutomatWordsMap* pObj)
{
	pObj->Clear();

	pObj->pNextFreeObj = m_pFreeMaps;
	m_pFreeMaps = pObj;

	m_uUsedMaps --;
}

void CXTPSyntaxEditLexAutomatMemMan::FreeObject(LA_CHAR_NODE* pObj)
{
	pObj->Clear();

	pObj->pNextNode = m_pFreeNodes;
	m_pFreeNodes = pObj;

	m_uUsedNodes--;
}

void CXTPSyntaxEditLexAutomatMemMan::FreeObject(PLA_CHAR_NODE* pObj)
{
	LA_HASH_TABLE* pHTab = LA_HASH_TABLE::GetHeader(pObj);
	pHTab->Clear();

	pHTab->pNextFreeObj = m_pFreeTables;
	ASSERT(pHTab != pHTab->pNextFreeObj);
	m_pFreeTables = pHTab;


	m_uUsedTables--;
	m_uUsedTablesBytes -= pHTab->uSize * sizeof(PLA_CHAR_NODE) + LA_HASH_TABLE::GetHeaderSizeB();
}

#ifdef _DEBUG
void CXTPSyntaxEditLexAutomatMemMan::Dump(CDumpContext& dc) const
{
	UNREFERENCED_PARAMETER(dc);

#ifdef TRACE_MEMORY_LEX_AUTOMAT_MEM_MAN

	UINT uAllocatedTotal = (m_uAllocatedMaps*sizeof(CXTPSyntaxEditLexAutomatWordsMap) +
		m_uAllocatedNodes*sizeof(LA_CHAR_NODE) +
		m_uAllocatedTablesBytes) / 1024;
	UINT uUsedTotal = (m_uUsedMaps*sizeof(CXTPSyntaxEditLexAutomatWordsMap) +
		m_uUsedNodes*sizeof(LA_CHAR_NODE) +
		m_uUsedTablesBytes) / 1024;

	dc  << _T("*** Lex-Automat Mem Man ***   \n ")
		<< _T("\n")
		<< _T("AllocatedMaps=") << (int)m_uAllocatedMaps << _T(" (") << (int)(m_uAllocatedMaps * sizeof(CXTPSyntaxEditLexAutomatWordsMap) / 1024) << _T(" KB)  \n")
		<< _T("UsedMaps=") << (int)m_uUsedMaps << _T(" (") << (int)(m_uUsedMaps * sizeof(CXTPSyntaxEditLexAutomatWordsMap) / 1024) << _T(" KB) \n")
		<< _T("\n")
		<< _T("AllocatedNodes=") << (int)m_uAllocatedNodes << _T(" (") << (int)(m_uAllocatedNodes*sizeof(LA_CHAR_NODE) / 1024) << _T(" KB) \n")
		<< _T(" UsedNodes=") << (int)m_uUsedNodes << _T(" (") << (int)(m_uUsedNodes*sizeof(LA_CHAR_NODE) / 1024) << _T(" KB) \n")
		<< _T("\n")
		<< _T("AllocatedTables=") << (int)m_uAllocatedTables << _T(" (") << (int)(m_uAllocatedTablesBytes / 1024) << _T(" KB) \n")
		<< _T("UsedTables=") << (int)m_uUsedTables << _T(" (") << (int)(m_uUsedTablesBytes / 1024) << _T(" KB) \n")
		<< _T("\n ")
		<< _T("= AllocatedTotal= ") << (int)uAllocatedTotal << _T(" KB, UsedTotal= ") << (int)uUsedTotal << _T(" KB, [Free= ") << ((int)uAllocatedTotal - (int)uUsedTotal) << _T(" KB] \n\n ");
#endif
}
#endif

////////////////////////////////////////////////////////////////////////////
//class CXTPSyntaxEditLexAutomatWordsMap : public CCmdTarget

CXTPSyntaxEditLexAutomatWordsMap::CXTPSyntaxEditLexAutomatWordsMap(UINT uHashTableSize)
{
	m_pHashTable = NULL;
	ASSERT(uHashTableSize >= 1);
	m_uHashTableSize = max(1, uHashTableSize);

	pNextFreeObj = NULL;
}

CXTPSyntaxEditLexAutomatWordsMap::~CXTPSyntaxEditLexAutomatWordsMap()
{
	//RemoveAll();
}

void CXTPSyntaxEditLexAutomatWordsMap::InitMap(UINT uHashTableSize)
{
	if (m_pHashTable)
	{
		XTPGetLexAutomatMemMan()->FreeObject(m_pHashTable);
		m_pHashTable = NULL;
	}

	ASSERT(uHashTableSize >= 1);
	m_uHashTableSize = uHashTableSize;
}

UINT CXTPSyntaxEditLexAutomatWordsMap::GetHashTableSize()
{
	return m_uHashTableSize;
}

void CXTPSyntaxEditLexAutomatWordsMap::OnFinalRelease()
{
	RemoveAll() ;

	XTPGetLexAutomatMemMan()->FreeObject(this);
}


void CXTPSyntaxEditLexAutomatWordsMap::RemoveAll()
{
	if (m_pHashTable)
	{
		for (UINT i = 0; i < m_uHashTableSize; i++)
		{
			LA_CHAR_NODE* pNode = m_pHashTable[i];
			while (pNode)
			{
				LA_CHAR_NODE* pNode2 = pNode;

				pNode = pNode->pNextNode;

				XTPGetLexAutomatMemMan()->FreeObject(pNode2);
			}
		}

		XTPGetLexAutomatMemMan()->FreeObject(m_pHashTable);
		m_pHashTable = NULL;
	}
}

UINT CXTPSyntaxEditLexAutomatWordsMap::PrimeAdjustU_50(UINT uNumber)
{
	ASSERT(uNumber <= 50);

	static UINT s_arPrimes_50[] = {1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53};
	static CUIntArray s_arMap_50;

	if (s_arMap_50.GetSize() == 0)
	{
		int nPrimeIdx = 0;
		for (UINT i = 0; i < 50; i++)
		{
			if (i > s_arPrimes_50[nPrimeIdx] && nPrimeIdx < _countof(s_arPrimes_50))
			{
				nPrimeIdx++;
				ASSERT(i <= s_arPrimes_50[nPrimeIdx]);
			}
			s_arMap_50.SetAtGrow(i, s_arPrimes_50[nPrimeIdx]);
		}
	}

	if (uNumber < 50)
	{
		UINT uPrameNum = s_arMap_50[uNumber];
		return uPrameNum;
	}
	return uNumber;
}

UINT CXTPSyntaxEditLexAutomatWordsMap::DivideHTSize(UINT uHashTableSize, UINT uMin)
{
	UINT uDiv = 2;
	if (uHashTableSize <= 10)
	{
		uDiv = 2;
	}
	else if (uHashTableSize <= 20)

	{
		uDiv = 3;
	}
	else
	{
		uDiv = 4;
	}

	UINT uNewSize = max(uHashTableSize/uDiv, uMin);
	uNewSize = PrimeAdjustU_50(uNewSize);
	return uNewSize;
}

WORD CXTPSyntaxEditLexAutomatWordsMap::_GetChar(LPCTSTR pcszStr, int& rnCharLen) const
{
	WORD wChar = 0;

	LPCTSTR p2 = _tcsinc(pcszStr);
	rnCharLen = int(p2 - pcszStr);

	if (rnCharLen == 1)
	{
		wChar = *((BYTE*)pcszStr);
	}
	else if (rnCharLen == 2)

	{
		wChar = *((WORD*)pcszStr);
	}
	else
	{
		ASSERT(FALSE);
		rnCharLen = 0;
	}

	return wChar;
}

WORD CXTPSyntaxEditLexAutomatWordsMap::_ChangeCase(WORD wChar) const
{
	WORD dwChar0[2] = {wChar, 0 };

	if (_istlower(wChar))
	{
		STRUPR_S((LPTSTR)&dwChar0, 2);
	}
	else if (_istupper(wChar))
	{
		TCSLWR_S((LPTSTR)&dwChar0, 2);
	}
	return dwChar0[0];
}

WORD CXTPSyntaxEditLexAutomatWordsMap::_MakeLower(WORD wChar) const
{
	if (_istupper(wChar))
	{
		WORD dwChar0[2] = {wChar, 0 };
		TCSLWR_S((LPTSTR)&dwChar0, 2);
		return dwChar0[0];
	}
	return wChar;
}

const LA_CHAR_NODE* CXTPSyntaxEditLexAutomatWordsMap::Lookup(WORD wChar) const
{
	if (m_pHashTable)
	{
		UINT uIndex = wChar % m_uHashTableSize;
		LA_CHAR_NODE* pNode = m_pHashTable[uIndex];
		while (pNode)
		{
			if (pNode->wChar == wChar)
			{
				return pNode;
			}
			pNode = pNode->pNextNode;
		}
	}
	return NULL;
}

const LA_CHAR_NODE* CXTPSyntaxEditLexAutomatWordsMap::SetAt(const LA_CHAR_NODE& newNode)
{
	if (!m_pHashTable)
	{
		ASSERT(m_uHashTableSize >= 1);
		m_pHashTable = XTPGetLexAutomatMemMan()->NewHashTable(m_uHashTableSize);
		if (!m_pHashTable)
		{
			return NULL;
		}
	}

	LA_CHAR_NODE* pNode2 = (LA_CHAR_NODE*)Lookup(newNode.wChar);
	if (!pNode2)
	{
		UINT uIndex = newNode.wChar % m_uHashTableSize;
		LA_CHAR_NODE* pNode0 = m_pHashTable[uIndex];

		pNode2 = XTPGetLexAutomatMemMan()->NewNode();
		*pNode2 = newNode;

		pNode2->pNextNode = pNode0;

		m_pHashTable[uIndex] = pNode2;
	}
	else
	{
		ASSERT(pNode2->wChar == newNode.wChar);
		*pNode2 = newNode;
	}

	return pNode2;
}

void CXTPSyntaxEditLexAutomatWordsMap::AddWord(CString strWord, UINT uHashTableSize )
{
	int nCharLen = 0;
	WORD wChar1 = _GetChar(strWord, nCharLen);
	DELETE_S(strWord, 0, nCharLen);

	BOOL bWordEnded = strWord.GetLength() <= 0;

	LA_CHAR_NODE* pWordData = (LA_CHAR_NODE*)Lookup(wChar1);

	if (!pWordData)
	{
		LA_CHAR_NODE wordData2;
		wordData2.wChar = wChar1;

		pWordData = (LA_CHAR_NODE*)SetAt(wordData2);
		if (!pWordData)
		{
			return;
		}
	}

	if (!bWordEnded && !pWordData->pNextMap)
	{
		pWordData->pNextMap = XTPGetLexAutomatMemMan()->NewMap(uHashTableSize); //new CXTPSyntaxEditLexAutomatWordsMap;
		if (!pWordData->pNextMap)
		{
			return;
		}
	}

	if (bWordEnded)
	{
		pWordData->wFlags |= LA_CHAR_NODE::nfSubWordEnd;
	}
	else
	{
		pWordData->pNextMap->AddWord(strWord, DivideHTSize(uHashTableSize, 1));
	}
}

BOOL CXTPSyntaxEditLexAutomatWordsMap::_FindWord(LPCTSTR pcszBuffer, CString& rstrWord, int nBufSize,
										WORD wEndFlags, BOOL bConvertToLowerCase,
										BOOL bTryToChangeCaseDyn, BOOL bChangeCase) const
{
	ASSERT((bConvertToLowerCase && bTryToChangeCaseDyn) == FALSE);

	int nCharLen = 0;
	WORD wChar1 = _GetChar(pcszBuffer, nCharLen);

	if (bConvertToLowerCase)
	{
		wChar1 = _MakeLower(wChar1);
	}
	else if (bChangeCase)
	{
		wChar1 = _ChangeCase(wChar1);

		if (wChar1 == 0)
		{
			return FALSE;
		}
	}

	LA_CHAR_NODE* pWordData = (LA_CHAR_NODE*)Lookup(wChar1);
	if (!pWordData)
	{
		return FALSE;
	}

	DWORD dwChar2 = wChar1;
	rstrWord += (LPCTSTR)&dwChar2;

	if (!pWordData->pNextMap || (pWordData->wFlags & wEndFlags) )
	{
		return TRUE;
	}

	if (nBufSize < nCharLen)
	{
		return FALSE;
	}

	pcszBuffer += nCharLen;
	nBufSize -= nCharLen;

	BOOL bRes = pWordData->pNextMap->_FindWord(pcszBuffer, rstrWord, nBufSize,
					wEndFlags, bConvertToLowerCase, bTryToChangeCaseDyn, FALSE);

	if (!bRes && bTryToChangeCaseDyn)
	{
		bRes = pWordData->pNextMap->_FindWord(pcszBuffer, rstrWord, nBufSize,
					wEndFlags, bConvertToLowerCase, bTryToChangeCaseDyn, TRUE);
	}

	if (!bRes && (pWordData->wFlags & LA_CHAR_NODE::nfSubWordEnd) )
	{
		return TRUE;
	}

	return bRes;
}

BOOL CXTPSyntaxEditLexAutomatWordsMap::FindWord(BOOL bMinWord, LPCTSTR pcszBuffer, CString& rstrWord, int nBufSize,
										 BOOL bConvertToLowerCase, BOOL bIgnoreCaseDyn) const
{
	BOOL bRes = _FindWord(pcszBuffer, rstrWord, nBufSize,
					(WORD)(bMinWord ? LA_CHAR_NODE::nfSubWordEnd : 0),
					bConvertToLowerCase, bIgnoreCaseDyn, FALSE);

	if (!bRes && bIgnoreCaseDyn)
	{
		bRes = _FindWord(pcszBuffer, rstrWord, nBufSize,
					(WORD)(bMinWord ? LA_CHAR_NODE::nfSubWordEnd : 0),
					bConvertToLowerCase, bIgnoreCaseDyn, TRUE);
	}

	return bRes;
}

////////////////////////////////////////////////////////////////////////////
CXTPSyntaxEditLexTagsAutomat::CXTPSyntaxEditLexTagsAutomat()
{
}

CXTPSyntaxEditLexTagsAutomat::~CXTPSyntaxEditLexTagsAutomat()
{
}

void CXTPSyntaxEditLexTagsAutomat::CopyFrom(const CXTPSyntaxEditLexTagsAutomat& rSrc)
{
	m_ptrWordsMap = rSrc.m_ptrWordsMap;
	m_ptrWordsMap_not = rSrc.m_ptrWordsMap_not;
}

void CXTPSyntaxEditLexTagsAutomat::RemoveAll()
{
	m_ptrWordsMap = NULL;
	m_ptrWordsMap_not = NULL;
}

UINT CXTPSyntaxEditLexTagsAutomat::_CalcHashTableSize(int nCount)
{
	if (!nCount)
	{
		return 1;
	}
	UINT uHashTableSize = 29;
	if (nCount <= 10)
	{         uHashTableSize = 3;
	} else if (nCount <= 30) {  uHashTableSize = 7;
	} else if (nCount <= 60) {  uHashTableSize = 11;
	} else if (nCount <= 100) { uHashTableSize = 17;
	} else if (nCount <= 200) { uHashTableSize = 23; }

	return uHashTableSize;
}

void CXTPSyntaxEditLexTagsAutomat::_AddWords(CXTPSyntaxEditLexAutomatWordsMapPtr& rPtrMap, CStringArray& arTags,
								  BOOL bConvertToLowerCase)
{
	int nCount = (int)arTags.GetSize();
	if (!nCount)
	{
		return;
	}

	if (!rPtrMap)
	{
		UINT uHashTableSize0 = _CalcHashTableSize(nCount);
		rPtrMap = XTPGetLexAutomatMemMan()->NewMap(uHashTableSize0);
		if (!rPtrMap)
		{
			return;
		}
	}
	UINT uHashTableSize = rPtrMap->GetHashTableSize();

	for (int i = 0; i < nCount; i++)
	{
		CString strTag = arTags[i];
		if (bConvertToLowerCase)
		{
			strTag.MakeLower();
		}
		rPtrMap->AddWord(strTag, CXTPSyntaxEditLexAutomatWordsMap::DivideHTSize(uHashTableSize, 3));
	}
}

void CXTPSyntaxEditLexTagsAutomat::AddTagsList(CXTPSyntaxEditLexVariantPtrArray* pArTags, BOOL bConvertToLowerCase)
{
	if (!pArTags)
	{
		ASSERT(FALSE);
		return;
	}

	CStringArray arTags, arTags_not;

	int nCount = (int)pArTags->GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexVariant* pLV = pArTags->GetAt(i, FALSE);
		if (!pLV)
		{
			ASSERT(FALSE);
			continue;
		}
		ASSERT(pLV->m_nObjType == xtpEditLVT_valStr || pLV->m_nObjType == xtpEditLVT_valVar);

		if (pLV->m_nObjType == xtpEditLVT_valStr)
		{
			arTags.Append(pLV->m_arStrVals);
		}
		else if (pLV->m_nObjType == xtpEditLVT_valVar)
		{
			CStringArray arVarData;
			pLV->m_Variable.GetVariableData(pLV->m_Variable.m_nVarID, arVarData);

			if (pLV->m_Variable.m_nVarFlags & CXTPSyntaxEditLexVariable::xtpEditVarfNot)
			{
				arTags_not.Append(arVarData);
			}
			else
			{
				arTags.Append(arVarData);
			}
		}
	}

	_AddWords(m_ptrWordsMap, arTags, bConvertToLowerCase);
	_AddWords(m_ptrWordsMap_not, arTags_not, bConvertToLowerCase);
}

BOOL CXTPSyntaxEditLexTagsAutomat::_FindWord(BOOL bMinWord, LPCTSTR pcszBuffer, CString& rstrWord, int nBufSize,
									 BOOL bConvertToLowerCase, BOOL bIgnoreCaseDyn)

{
	rstrWord.Empty();

	if (m_ptrWordsMap)
	{
		if (m_ptrWordsMap->FindWord(bMinWord, pcszBuffer, rstrWord, nBufSize, bConvertToLowerCase, bIgnoreCaseDyn))
		{
			return TRUE;
		}
	}

	if (m_ptrWordsMap_not)
	{
		if (!m_ptrWordsMap->FindWord(bMinWord, pcszBuffer, rstrWord, nBufSize, bConvertToLowerCase, bIgnoreCaseDyn))
		{
			WORD dwChar2 = 0;
			TCSNCCPY_S((LPTSTR)&dwChar2, sizeof(TCHAR) + 1, pcszBuffer, 1);
			rstrWord += (LPCTSTR)&dwChar2;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CXTPSyntaxEditLexTagsAutomat::FindMinWord(LPCTSTR pcszBuffer, CString& rstrWord, int nBufSize,
									 BOOL bConvertToLowerCase, BOOL bIgnoreCaseDyn)
{
	return _FindWord(TRUE, pcszBuffer, rstrWord, nBufSize, bConvertToLowerCase, bIgnoreCaseDyn);
}

BOOL CXTPSyntaxEditLexTagsAutomat::FindMaxWord(LPCTSTR pcszBuffer, CString& rstrWord, int nBufSize,
									 BOOL bConvertToLowerCase, BOOL bIgnoreCaseDyn)
{
	return _FindWord(FALSE, pcszBuffer, rstrWord, nBufSize, bConvertToLowerCase, bIgnoreCaseDyn);
}

BOOL CXTPSyntaxEditLexTagsAutomat::IsEmpty()
{
	return m_ptrWordsMap == NULL && m_ptrWordsMap_not == NULL;
}


////////////////////////////////////////////////////////////////////////////
CMapStringToPtr CXTPSyntaxEditLexVariable::s_mapVar2ID;
BOOL CXTPSyntaxEditLexVariable::s_bVarMapInitialized = FALSE;

//////////////////////////////////////////////////////
class CXTPSyntaxEditLexVariable_initializer : public CXTPSyntaxEditLexVariable
{
public:
	CXTPSyntaxEditLexVariable_initializer() {
		InitStandartVarsIfNeed();
	};
} s_LVarIniter;

//////////////////////////////////////////////////////

void CXTPSyntaxEditLexVariable::InitStandartVarsIfNeed()
{
	if (s_bVarMapInitialized)
	{
		return;
	}
	s_bVarMapInitialized = TRUE;

	s_mapVar2ID[_T("@alpha")]   = (void*)xtpEditVarID_alpha;
	s_mapVar2ID[_T("@digit")]   = (void*)xtpEditVarID_digit;
	s_mapVar2ID[_T("@hexdigit")] = (void*)xtpEditVarID_HexDigit;
	s_mapVar2ID[_T("@specs")]   = (void*)xtpEditVarID_specs;
	s_mapVar2ID[_T("@eol")]     = (void*)xtpEditVarID_EOL;
}

CXTPSyntaxEditLexVariable::CXTPSyntaxEditLexVariable()
{
	m_nVarFlags = 0;
	m_nVarID = 0;
}

CXTPSyntaxEditLexVariable::~CXTPSyntaxEditLexVariable()
{
}

const CXTPSyntaxEditLexVariable& CXTPSyntaxEditLexVariable::operator=(const CXTPSyntaxEditLexVariable& rSrc)
{
	m_strVar = rSrc.m_strVar;
	m_nVarFlags = rSrc.m_nVarFlags;
	m_nVarID = rSrc.m_nVarID;
	return *this;
}

BOOL CXTPSyntaxEditLexVariable::SetVariable(LPCTSTR pcszVarName)
{
	m_nVarFlags = 0;
	m_nVarID = xtpEditVarID_Unknown;
	m_strVar = pcszVarName;

	//==================================
	CStringArray arVar;
	if (!SplitStr(pcszVarName, _T(':'), arVar) || arVar.GetSize() < 1)
	{
		//TRACE
		ASSERT(FALSE);
		return FALSE;
	}
	//----------------------------------
	CString strVar = arVar[0];
	//----------------------------------
	int nVarSize = (int)arVar.GetSize();
	if (nVarSize > 1)
	{
		CString strOp = arVar[1];
		if (strOp.CompareNoCase(_T("not")) == 0)
		{
			m_nVarFlags |= xtpEditVarfNot;
		}
	}
	//==================================

	m_nVarID = GetVarID(strVar);
	ASSERT(m_nVarID != xtpEditVarID_Unknown);

	return m_nVarID != xtpEditVarID_Unknown;
}

int CXTPSyntaxEditLexVariable::GetVarID(LPCTSTR pcszVarName)
{
	void* pVarID = 0;
	CString strVar = pcszVarName;
	strVar.MakeLower();

	if (s_mapVar2ID.Lookup(strVar, pVarID))
	{
		return (int)(INT_PTR)pVarID;
	}
	return xtpEditVarID_Unknown;
}

BOOL CXTPSyntaxEditLexVariable::operator == (const CXTPSyntaxEditLexVariable& rSrc) const
{
	return m_nVarID == rSrc.m_nVarID && m_nVarFlags == rSrc.m_nVarFlags;
}

BOOL CXTPSyntaxEditLexVariable::GetVariableData(int nVarID, CStringArray& rarVarData)
{
	CUIntArray arFrom, arTo;
	int i;

	if (nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_alpha)
	{
		arFrom.Add(_T('a'));    arTo.Add(_T('z'));
		arFrom.Add(_T('A'));    arTo.Add(_T('Z'));
	}
	else if (nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_digit)
	{
		arFrom.Add(_T('0'));    arTo.Add(_T('9'));
	}
	else if (nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_HexDigit)
	{
		arFrom.Add(_T('0'));    arTo.Add(_T('9'));
		arFrom.Add(_T('a'));    arTo.Add(_T('f'));
		arFrom.Add(_T('A'));    arTo.Add(_T('F'));
	}

	for (i = 0; i < arFrom.GetSize(); i++)
	{
		for (UINT c = arFrom[i]; c <= arTo[i]; c++)
		{
			CString strChar((TCHAR)c);
			rarVarData.Add(strChar);
		}
	}

	if (arFrom.GetSize())
	{
		return TRUE;
	}

	//------------------------------------------------------------------------
	if (nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_specs)
	{
		LPCTSTR pChar = cszSpecs;
		int nCountC = (int)_tcsclen(cszSpecs);
		for (i = 0; i < nCountC; i++)
		{
			CString strChar((TCHAR)(*pChar));
			rarVarData.Add(strChar);
			pChar = _tcsinc(pChar);
		}
	}
	else if (nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_EOL)
	{
		rarVarData.Add(_T("\r"));
		rarVarData.Add(_T("\n"));
	}
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}
	return TRUE;
}
//===========================================================================
CXTPSyntaxEditLexVariant::~CXTPSyntaxEditLexVariant()
{
}

CXTPSyntaxEditLexVariant::CXTPSyntaxEditLexVariant()
{
	m_nObjType = xtpEditLVT_Unknown;
	m_nValue = 0;
}

CXTPSyntaxEditLexVariant::CXTPSyntaxEditLexVariant(const CXTPSyntaxEditLexVariant& rSrc)
{
	*this = rSrc;
}

CXTPSyntaxEditLexVariant::CXTPSyntaxEditLexVariant(CXTPSyntaxEditLexClass* pClass):
				m_ptrClass(pClass, TRUE)
{
	m_nObjType = xtpEditLVT_classPtr;
	m_nValue = 0;
}

CXTPSyntaxEditLexVariant::CXTPSyntaxEditLexVariant(CXTPSyntaxEditLexVariantPtrArray* pLVArray):
				m_ptrLVArrayPtr(pLVArray)
{
	m_nObjType = xtpEditLVT_LVArrayPtr;
	m_nValue = 0;
}

CXTPSyntaxEditLexVariant::CXTPSyntaxEditLexVariant(const CXTPSyntaxEditLexVariable& rSrcVar)
{
	m_Variable = rSrcVar;
	m_nObjType = xtpEditLVT_valVar;
	m_nValue = 0;
}

CXTPSyntaxEditLexVariant::CXTPSyntaxEditLexVariant(LPCTSTR pcszStr, int eType ) {
	m_nValue = 0;

	ASSERT( eType == xtpEditLVT_valStr || eType == xtpEditLVT_className);

	m_nObjType = eType;
	m_arStrVals.SetAtGrow(0, pcszStr);
}

CXTPSyntaxEditLexVariant::CXTPSyntaxEditLexVariant(const CStringArray* pArStrVals) {
	m_nValue = 0;
	m_nObjType = xtpEditLVT_valStr;
	if (pArStrVals)
	{
		m_arStrVals.Append(*pArStrVals);
	}
}

CXTPSyntaxEditLexVariant::CXTPSyntaxEditLexVariant(int nValue) {
	m_nObjType = xtpEditLVT_valInt;
	m_nValue = nValue;
}


BOOL CXTPSyntaxEditLexVariant::IsStrType() const
{
	if (m_nObjType == xtpEditLVT_valStr || m_nObjType == xtpEditLVT_className)
	{
		return TRUE;
	}
	return FALSE;
}

LPCTSTR CXTPSyntaxEditLexVariant::GetStr() const
{
	if (IsStrType())
	{
		if (m_arStrVals.GetSize() > 0)
		{
			return m_arStrVals[0];
		}
		return _T("");
	}
	ASSERT(FALSE);
	return _T("");
}

const CXTPSyntaxEditLexVariant& CXTPSyntaxEditLexVariant::operator = (int nVal)
{
	m_nValue = nVal;
	m_nObjType = xtpEditLVT_valInt;

	return *this;
}

const CXTPSyntaxEditLexVariant& CXTPSyntaxEditLexVariant::operator = (const CXTPSyntaxEditLexVariable& rSrcVar)
{
	m_nValue = 0;
	m_nObjType = xtpEditLVT_valVar;
	m_Variable = rSrcVar;
	return *this;
}

const CXTPSyntaxEditLexVariant& CXTPSyntaxEditLexVariant::operator = (const CXTPSyntaxEditLexVariant& rSrc)
{
	m_nObjType = rSrc.m_nObjType;

	m_ptrClass = rSrc.m_ptrClass;
	m_ptrLVArrayPtr = rSrc.m_ptrLVArrayPtr;

	m_Variable = rSrc.m_Variable;

	m_nValue = rSrc.m_nValue;

	m_arStrVals.RemoveAll();
	m_arStrVals.Append(rSrc.m_arStrVals);

	return *this;
}

BOOL CXTPSyntaxEditLexVariant::SetVariable(LPCTSTR pcszVarName)
{
	m_nValue = 0;
	m_nObjType = xtpEditLVT_valVar;
	BOOL bRes = m_Variable.SetVariable(pcszVarName);
	return bRes;
}

CXTPSyntaxEditLexVariant* CXTPSyntaxEditLexVariant::Clone() const
{
	CXTPSyntaxEditLexVariant* pClone = new CXTPSyntaxEditLexVariant(*this);

	return pClone;
}

BOOL CXTPSyntaxEditLexVariant::operator == (const CXTPSyntaxEditLexVariant& rSrc) const
{
	if (m_nObjType != rSrc.m_nObjType)
	{
		return FALSE;
	}
	switch(m_nObjType)
	{
	case xtpEditLVT_Unknown:
		return TRUE;
	case xtpEditLVT_className:
	case xtpEditLVT_valStr:
		if (m_arStrVals.GetSize() == rSrc.m_arStrVals.GetSize())
		{
			for (int i = 0; i < m_arStrVals.GetSize(); i++)
			{
				if (m_arStrVals[i] != rSrc.m_arStrVals[i])
				{
					return FALSE;
				}
			}
			return TRUE;
		}
		return FALSE;

	case xtpEditLVT_valInt:
		return m_nValue == rSrc.m_nValue;
	case xtpEditLVT_valVar:
		return m_Variable == rSrc.m_Variable;

	case xtpEditLVT_classPtr:
		return m_ptrClass && rSrc.m_ptrClass &&
			m_ptrClass->GetClassName() == rSrc.m_ptrClass->GetClassName();
	case xtpEditLVT_LVArrayPtr:
		if (m_ptrLVArrayPtr && rSrc.m_ptrLVArrayPtr &&
			m_ptrLVArrayPtr->GetSize() == rSrc.m_ptrLVArrayPtr->GetSize())
		{
			for (int i = 0; i < m_ptrLVArrayPtr->GetSize(); i++)
			{
				if (!(*m_ptrLVArrayPtr->GetAt(i, FALSE) == *rSrc.m_ptrLVArrayPtr->GetAt(i, FALSE)) )
				{
					return FALSE;
				}
			}
			return TRUE;
		}
		return FALSE;
	default:
		ASSERT(FALSE);
	}
	return FALSE;
}

#ifdef _DEBUG
void CXTPSyntaxEditLexVariant::Dump(CDumpContext& dc) const
{
	if (IsStrType())
	{
		int nC = (int)m_arStrVals.GetSize();
		for (int k = 0; k < nC; k++)
		{
			CString strTmp = m_arStrVals[k];

			if (k)
			{
				dc << _T(", ");
			}

			if (m_nObjType == xtpEditLVT_valStr)
			{
				strTmp = XTPSyntaxEditLexConfig()->ESToStr(strTmp, FALSE);
				dc << strTmp;
			}
			else
			{
				dc << strTmp;
			}
		}
	}
	else if (m_nObjType == xtpEditLVT_valInt)
	{
		dc << m_nValue;
	}
	else if (m_nObjType == xtpEditLVT_valVar)
	{
		dc << m_Variable.m_strVar;
	}
	else
	{
		dc << _T(" ?<LV type=") << m_nObjType;
		ASSERT(FALSE);
	}
}
#endif

////////////////////////////////////////////////////////////////////////////
CXTPSyntaxEditLexOnScreenParseCnt::CXTPSyntaxEditLexOnScreenParseCnt()
{
	m_nRowStart = m_nRowEnd = 0;
}

CXTPSyntaxEditLexOnScreenParseCnt::~CXTPSyntaxEditLexOnScreenParseCnt()
{
}

////////////////////////////////////////////////////////////////////////////
CXTPSyntaxEditLexClass_file::CXTPSyntaxEditLexClass_file()
{
	m_bExtInitialized = FALSE;
}

CXTPSyntaxEditLexClass_file::~CXTPSyntaxEditLexClass_file()
{
}

CXTPSyntaxEditLexClass* CXTPSyntaxEditLexClass_file::Clone(CXTPSyntaxEditLexClassSchema* pOwnerSch)
{
	CXTPSyntaxEditLexClass* pNewClass = NULL;
	if (pOwnerSch)
	{
		pNewClass = pOwnerSch->GetNewClass(TRUE);
	}
	else
	{
		pNewClass = new CXTPSyntaxEditLexClass_file();
	}

	if (pNewClass)
	{
		pNewClass->CopyFrom(this);
	}

	return pNewClass;
}

BOOL CXTPSyntaxEditLexClass_file::InternalInitExts(BOOL bReInit)
{
	if (m_bExtInitialized && !bReInit)
	{
		return TRUE;
	}
	m_bExtInitialized = FALSE;

	m_arExt.RemoveAll();

	if (!m_Parent.arClassNames.GetSize())
	{
		TRACE(_T("ERROR! No File extentions string. \n"));
		return FALSE;
	}

	ASSERT(m_Parent.arClassNames.GetSize() == 1);

	CString strExtsSet = m_Parent.arClassNames[0];
	strExtsSet.TrimLeft();
	strExtsSet.TrimRight();
	int nESLen = strExtsSet.GetLength();

	if (nESLen < 2 || strExtsSet[0] != _T('<') ||
		strExtsSet[nESLen-1] != _T('>'))
	{
		TRACE(_T("ERROR! File extentions bad format: '%s'. Right format: <*.ex1|*.ext2|...> \n"), (LPCTSTR)strExtsSet);
		return FALSE;
	}

	DELETE_S(strExtsSet, nESLen-1);
	DELETE_S(strExtsSet, 0);

	SplitStr(strExtsSet, _T('|'), m_arExt);

	int nCount = (int)m_arExt.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CString strExt = m_arExt[i];
		REPLACE_S(strExt, _T("*"), _T(""));
		strExt.TrimLeft();
		strExt.TrimRight();

		m_arExt[i] = strExt;
	}

	//-----------------------------------------------------------------------
	m_bExtInitialized = TRUE;
	return TRUE;
}

BOOL CXTPSyntaxEditLexClass_file::TestExt(LPCTSTR pcszExt) const
{
	ASSERT(m_bExtInitialized);

	int nCount = (int)m_arExt.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CString strExt = m_arExt[i];
		if (_tcsicoll(strExt, pcszExt) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CXTPSyntaxEditLexVariantPtr CXTPSyntaxEditLexClass_file::PropV(LPCTSTR pcszPropName)
{
	CXTPSyntaxEditLexVariantPtr ptrRes = CXTPSyntaxEditLexClass::PropV(pcszPropName);
	if (ptrRes)
	{
		return ptrRes;
	}

	static LPCTSTR pcszIsExtPref = _T("IsExt=");
	static int nIsExtPrefLen = (int)_tcsclen(pcszIsExtPref);

	if (_tcsnicmp(pcszPropName, pcszIsExtPref, nIsExtPrefLen) == 0)
	{
		CString strExt = pcszPropName;
		ASSERT(strExt.GetLength() >= nIsExtPrefLen);

		DELETE_S(strExt, 0, nIsExtPrefLen);

		InternalInitExts(TRUE);

		BOOL bRes = TestExt(strExt);

		CXTPSyntaxEditLexVariant* pV = new CXTPSyntaxEditLexVariant((int)bRes);
		return pV;
	}

	return NULL;
}

int CXTPSyntaxEditLexClass_file::RunParse(CTextIter* pTxtIter,
						CXTPSyntaxEditLexTextSchema* pTxtSch,
						CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
						CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt
						)
{
	if (!pTxtIter || !pTxtSch)
	{
		ASSERT(FALSE);
		//TRACE
		return xtpEditLPR_Error;
	}
	int nParseRes = 0;

	InternalInitExts();

	//===========================================================================
	BOOL bStarted = (rPtrTxtBlock != NULL);
	if (!bStarted )
	{
		CString strFileExt = pTxtIter->GetFileExt();
		bStarted = TestExt(strFileExt);

		if (!bStarted )
		{
			return 0;
		}

		rPtrTxtBlock = pTxtSch->GetNewBlock();
		if (rPtrTxtBlock)
		{
			rPtrTxtBlock->m_ptrLexClass.SetPtr(this, TRUE);

			rPtrTxtBlock->m_PosStartLC = pTxtIter->GetPosLC();
			rPtrTxtBlock->m_nStartTagLen = 0;
		}
		else
		{
			return xtpEditLPR_StartFound | xtpEditLPR_Error;
		}

		nParseRes |= xtpEditLPR_StartFound | xtpEditLPR_Iterated;
	}

	//===========================================================================
	// Run End
	if (pTxtIter->IsEOF() && rPtrTxtBlock)
	{
		CSingleLock singleLock(pTxtSch->GetDataLoker(), TRUE);

		rPtrTxtBlock->m_PosEndLC = pTxtIter->GetPosLC();
		rPtrTxtBlock->m_nEndTagXLCLen = 0;

		nParseRes |= xtpEditLPR_EndFound | xtpEditLPR_Iterated;
		return nParseRes;
	}

	if (nParseRes)
	{
		return nParseRes;
	}


	//===========================================================================
	// *** run childs ***
	int nChildsCount = (int)m_arChildrenClasses.GetSize() + (int)m_arDynChildrenClasses.GetSize();
	if (nChildsCount)
	{
		int nCHres = pTxtSch->RunChildren(pTxtIter, rPtrTxtBlock, this,
											pOnScreenRunCnt);
		nParseRes |= ~(xtpEditLPR_StartFound|xtpEditLPR_EndFound) & nCHres;

		if (nParseRes & (xtpEditLPR_Error|xtpEditLPR_RunBreaked|xtpEditLPR_RunFinished))
		{
			return nParseRes;
		}
		//---------------------------------------------------------------------------
		// Run End
		if (pTxtIter->IsEOF() && rPtrTxtBlock)
		{
			CSingleLock singleLock(pTxtSch->GetDataLoker(), TRUE);

			rPtrTxtBlock->m_PosEndLC = pTxtIter->GetPosLC();
			rPtrTxtBlock->m_nEndTagXLCLen = 0;

			nParseRes |= xtpEditLPR_EndFound;
		}
	}
	return nParseRes;
}


////////////////////////////////////////////////////////////////////////////
CXTPSyntaxEditLexClass::CXTPSyntaxEditLexClass()
{
	SetSubMembers();

	ClearAttributesCache();

	m_Parent.Clear();
	m_Children.Clear();

	m_nActiv_EndTags_Offset = 0;
}

CXTPSyntaxEditLexClass::~CXTPSyntaxEditLexClass()
{
}

void CXTPSyntaxEditLexClass::ClearAttributesCache()
{
	m_bCaseSensitive_Cached = -1; // ask
	m_bCollapsable_Cached = -1; // ask
	m_bRestartRunLoop_Cached = -1; // ask
	m_bEndClassParent_this_Cached = -1; // ask

	m_bTxtAttr_cached = FALSE; // ask
}

CXTPSyntaxEditLexVariantPtr CXTPSyntaxEditLexClass::PropV(LPCTSTR pcszPropName)
{
	if (_tcsicmp(pcszPropName, _T("name")) == 0)
	{
		return new CXTPSyntaxEditLexVariant(m_strClassName, xtpEditLVT_className);
	}

	CXTPSyntaxEditLexVariantPtrArray* ptrData = GetIfMy(pcszPropName);
	if (ptrData && ptrData->GetSize())
	{
		return new CXTPSyntaxEditLexVariant(ptrData);
	}

	return NULL;
}

void CXTPSyntaxEditLexClass::CloseClasses(CXTPSyntaxEditLexClassPtrArray* pArClasses)
{
	if (!pArClasses)
	{
		ASSERT(FALSE);
		return;
	}
	// 1. ***
	int nCount = (int)pArClasses->GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrC = pArClasses->GetAt(i);
		ASSERT(ptrC);
		if (ptrC)
		{
			ptrC->Close();
		}
	}
	pArClasses->RemoveAll();
}

void CXTPSyntaxEditLexClass::Close()
{
	// 1. ***
	CXTPSyntaxEditLexClass::CloseClasses(&m_arChildrenClasses);
	CXTPSyntaxEditLexClass::CloseClasses(&m_arDynChildrenClasses);

	// 2. ***
	m_arChildrenSelfRefClasses.RemoveAll();

	// 3. ***
	TBase::RemoveAll();

	m_mapAttributes.RemoveAll();

	// 4. ***
	m_Parent.ptrDirect = NULL;
}

BOOL CXTPSyntaxEditLexClass::SetProp(const XTP_EDIT_LEXPROPINFO* pPropDesc)
{
	if (!pPropDesc || pPropDesc->arPropName.GetSize() < 1)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	BOOL bProcessed = FALSE;
	BOOL bOK = SetProp_ProcessSpecials(pPropDesc, bProcessed);
	if (!bOK || bProcessed)
	{
		return bOK;
	}

	//-----------------------------------------------------------------------
	bProcessed = FALSE;
	bOK = SetProp_ProcessSpecObjects(pPropDesc, bProcessed);
	if (!bOK || bProcessed)
	{
		return bOK;
	}

	//-----------------------------------------------------------------------
	bProcessed = FALSE;
	bOK = SetProp_ProcessAttributes(pPropDesc, bProcessed);
	if (!bOK || bProcessed)
	{
		return bOK;
	}
	return FALSE;
}

BOOL CXTPSyntaxEditLexClass::ParseValues(const XTP_EDIT_LEXPROPINFO* pPropDesc,
								CXTPSyntaxEditLexVariantPtrArray* pLVArray)
{
	if (!pPropDesc || pPropDesc->arPropName.GetSize() < 1 || !pLVArray)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	pLVArray->RemoveAll();

	int nNCount = (int)pPropDesc->arPropName.GetSize();
	CString strLastName = pPropDesc->arPropName[nNCount-1];
	strLastName.MakeLower();

	CString strFullName = MakeStr(pPropDesc->arPropName, _T(":"));
	strFullName.MakeLower();

	//ref, class, tag, separators
	CXTPSyntaxEditLexVariantPtr ptrLexVar;

	int nVCount = (int)pPropDesc->arPropValue.GetSize();
	for (int i = 0; i < nVCount; i++)
	{
		CString strVal = pPropDesc->arPropValue[i];
		CString strVal2;
		int nVType = xtpEditLVT_Unknown;

		if (IsQuoted(strVal, &strVal2))
		{
			if (strLastName == _T("tag") || strLastName == _T("separators"))
			{
				nVType = xtpEditLVT_valStr;
			}
			else
			{
				TRACE_ASSERT(FALSE);
				//TRACE
				TRACE(_T("ERROR! Quoted values ('%s') are not allowed for <:%s>'. (only for <:tag> and <:separators>) '%s'  \n"),
						(LPCTSTR)strVal, (LPCTSTR)strLastName, (LPCTSTR)strFullName);
			}
		}
		else if (IsVar(strVal, &strVal2))
		{
			if (strLastName == _T("tag") || strLastName == _T("separators"))
			{
				nVType = xtpEditLVT_valVar;
			}
			else
			{
				TRACE_ASSERT(FALSE);
				//TRACE
				TRACE(_T("ERROR! Variable values ('%s') are not allowed for <:%s>'. (only for <:tag> and <:separators>) '%s'  \n"),
					(LPCTSTR)strVal, (LPCTSTR)strLastName, (LPCTSTR)strFullName);
			}
		}
		else
		{
			//class
			if (strLastName == _T("class"))
			{
				nVType = xtpEditLVT_className;
				strVal2 = strVal;
				strVal2.TrimLeft();
				strVal2.TrimRight();
			}
			else
			{
				TRACE_ASSERT(FALSE);
				TRACE(_T("ERROR! Unknown ref-expression '%s' \n"), (LPCTSTR)strVal);
			}
		}
		//----------------------------------
		if (ptrLexVar &&
			(ptrLexVar->m_nObjType != nVType || !ptrLexVar->IsStrType()) )
		{
			ASSERT(ptrLexVar->m_nObjType != xtpEditLVT_Unknown);

			pLVArray->Add(ptrLexVar);
			ptrLexVar = NULL;
		}

		if (nVType != xtpEditLVT_Unknown)
		{
			if (!ptrLexVar)
			{
				ptrLexVar = new CXTPSyntaxEditLexVariant();
			}
			if (!ptrLexVar)
			{
				return FALSE;
			}
			if (nVType == xtpEditLVT_valVar)
			{
				if (!ptrLexVar->SetVariable(strVal2))
				{
					TRACE(_T("ERROR! Unknown Variable '%s' \n"), (LPCTSTR)strVal2);
				}
			}
			else
			{
				ptrLexVar->m_nObjType = nVType;
				ptrLexVar->m_arStrVals.Add(strVal2);
				ASSERT(ptrLexVar->IsStrType());
			}

		}
	}

	//-----------------------------------------
	if (ptrLexVar)
	{
		ASSERT(ptrLexVar->m_nObjType != xtpEditLVT_Unknown);
		pLVArray->Add(ptrLexVar);
		ptrLexVar = NULL;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditLexClass::SetProp_ProcessSpecObjects(const XTP_EDIT_LEXPROPINFO* pPropDesc,
										BOOL& rbProcessed)
{
	rbProcessed = FALSE;

	if (!pPropDesc || pPropDesc->arPropName.GetSize() < 1)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CString strFullName = MakeStr(pPropDesc->arPropName, _T(":"));
	strFullName.MakeLower();

	if (IfMy(strFullName))
	{
		CXTPSyntaxEditLexVariantPtrArray arPtrLVar;

		if (!ParseValues(pPropDesc, &arPtrLVar))
		{
			return FALSE;
		}

		if (!AppendIfMy(strFullName, arPtrLVar))
		{
			ASSERT(FALSE);
			return FALSE;
		}
		rbProcessed = TRUE;
		return TRUE;
	}


	return TRUE;
}

BOOL CXTPSyntaxEditLexClass::IsQuoted(CString strValue, CString* pstrUnQuotedVal)
{
	CString strTmp = strValue;
	strTmp.TrimLeft();
	strTmp.TrimRight();

	int nLen = strTmp.GetLength();
	if (nLen >= 2)
	{
		if (strTmp[0] == _T('\'') && strTmp[nLen-1] == _T('\'') )
		{
			if (pstrUnQuotedVal)
			{
				DELETE_S(strTmp, nLen-1);
				DELETE_S(strTmp, 0);

				*pstrUnQuotedVal = strTmp;
			}
			return TRUE;
		}
	}

	if (pstrUnQuotedVal)
	{
		*pstrUnQuotedVal = _T("");
	}
	return FALSE;
}

BOOL CXTPSyntaxEditLexClass::IsVar(CString strValue, CString* pstrVarVal)
{
	CString strTmp = strValue;
	strTmp.TrimLeft();
	strTmp.TrimRight();

	int nLen = strTmp.GetLength();
	if (nLen >= 2)
	{
		if (strTmp[0] == _T('@'))
		{
			if (pstrVarVal)
			{
				*pstrVarVal = strTmp;
			}
			return TRUE;
		}
	}

	if (pstrVarVal)
	{
		*pstrVarVal = _T("");
	}
	return FALSE;
}

BOOL CXTPSyntaxEditLexClass::SetProp_ProcessSpecials(const XTP_EDIT_LEXPROPINFO* pPropDesc,
										  BOOL& rbProcessed)
{
	rbProcessed = FALSE;

	if (!pPropDesc || pPropDesc->arPropName.GetSize() < 1)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nNCount = (int)pPropDesc->arPropName.GetSize();
	CString strMainName = pPropDesc->arPropName[0];

	if (strMainName.CompareNoCase(_T("parent")) == 0)
	{
		if (nNCount == 1)
		{
			m_Parent.eOpt = xtpEditOptParent_direct;
		}
		else
		{
			ASSERT(nNCount == 2);
			CString strName2 = pPropDesc->arPropName[1];

			if (strName2.CompareNoCase(_T("file")) == 0)
			{
				m_Parent.eOpt = xtpEditOptParent_file;
			}
			else if (strName2.CompareNoCase(_T("dyn")) == 0)
			{
				m_Parent.eOpt = xtpEditOptParent_dyn;
			}
			else //if (strName2.CompareNoCase(_T("Override")) == 0)
			{
				ASSERT(FALSE);
				return FALSE;
			}
		}
		m_Parent.arClassNames.Append(pPropDesc->arPropValue);
		rbProcessed = TRUE;
	}
	else if (strMainName.CompareNoCase(_T("children")) == 0)
	{
		int nValCount = (int)pPropDesc->arPropValue.GetSize();

		if (nValCount == 0)
		{
			m_Children.eOpt = xtpEditOptChildren_No;

			TRACE_ASSERT(m_Children.arClassNames.GetSize() == 0);
			//TRACE(_T(""));

		}
		else
		{
			CString strVal0 = pPropDesc->arPropValue[0];
			strVal0.TrimLeft();
			strVal0.TrimRight();
			if (strVal0 == _T("0"))
			{
				m_Children.eOpt = xtpEditOptChildren_No;
				TRACE_ASSERT(m_Children.arClassNames.GetSize() == 0);
			}
			else
			{

				TRACE_ASSERT(m_Children.arClassNames.GetSize() == 0 &&
							m_Children.eOpt == xtpEditOptChildren_Unknown ||
							m_Children.eOpt == xtpEditOptChildren_List);

				m_Children.eOpt = xtpEditOptChildren_List;
				m_Children.arClassNames.Append(pPropDesc->arPropValue);
			}
		}
		rbProcessed = TRUE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditLexClass::SetProp_ProcessAttributes(const XTP_EDIT_LEXPROPINFO* pPropDesc,
											BOOL& rbProcessed)
{
	rbProcessed = FALSE;

	if (!pPropDesc || pPropDesc->arPropName.GetSize() < 1)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	CString strFullName = MakeStr(pPropDesc->arPropName, _T(":"));
	strFullName.MakeLower();

	BOOL bKnown = FALSE;
	for (int i = 0; i < _countof(g_arKnownLexClassAttribs); i++)
	{
		if (strFullName.CompareNoCase(g_arKnownLexClassAttribs[i]) == 0)
		{
			bKnown = TRUE;
			break;
		}
	}
	if (!bKnown)
	{
		TRACE(_T("WARNING! Unknown attribute '%s'. \n"), (LPCTSTR)strFullName);
		//return TRUE;
	}

	//===========================================================================
	CXTPSyntaxEditLexVariantPtr ptrLvData = new CXTPSyntaxEditLexVariant();
	if (!ptrLvData)
	{
		return FALSE;
	}

	//---------------------------------------------------------------------------
	int nPCount = (int)pPropDesc->arPropValue.GetSize();
	if (nPCount == 1)
	{
		CString strVal = pPropDesc->arPropValue[0];
		CString strVal2;
		if (IsQuoted(strVal, &strVal2))
		{
			*ptrLvData = CXTPSyntaxEditLexVariant(strVal2);
		}
		else if (strFullName == _T("end:class:parent"))
		{
			*ptrLvData = CXTPSyntaxEditLexVariant(&pPropDesc->arPropValue);
		}
		else
		{
			static const int c_nClrPrefLen = (int)_tcsclen(XTPLEX_ATTR_COLORPREFIX);

			TCHAR* pCh = NULL;
			int nValue = _tcstol(strVal, &pCh, 0);

			if (_tcsnicmp(strFullName, XTPLEX_ATTR_COLORPREFIX, c_nClrPrefLen) == 0)
			{
				nValue = XTP_EDIT_RGB_INT2CLR(nValue);
			}
			*ptrLvData = nValue;

		}
		m_mapAttributes[strFullName] = ptrLvData;
	}
	else if (nPCount > 1)
	{
		*ptrLvData = CXTPSyntaxEditLexVariant(&pPropDesc->arPropValue);
		m_mapAttributes[strFullName] = ptrLvData;
	}
	else
	{
		//TRACE
		TRACE(_T("ERROR! There is not values present for attribute '%s'  \n"), (LPCTSTR)strFullName);
	}
	rbProcessed = TRUE;

	//-----------------------------------------------------------------------
	return TRUE;
}

void CXTPSyntaxEditLexClass::SortTags()
{
	BOOL bAsc = FALSE;
	BOOL bNoCase = !IsCaseSensitive();

	SortTagsInLexVarArray(m_previous.m_tag, bAsc, bNoCase);
	SortTagsInLexVarArray(m_previous.m_tag_separators, bAsc, bNoCase);

	SortTagsInLexVarArray(m_start.m_tag, bAsc, bNoCase);

	SortTagsInLexVarArray(m_end.m_tag, bAsc, bNoCase);
	SortTagsInLexVarArray(m_end.m_separators, bAsc, bNoCase);

	SortTagsInLexVarArray(m_token.m_tag, bAsc, bNoCase);
	SortTagsInLexVarArray(m_token.m_start_separators, bAsc, bNoCase);
	SortTagsInLexVarArray(m_token.m_end_separators, bAsc, bNoCase);

	SortTagsInLexVarArray(m_skip.m_tag, bAsc, bNoCase);

	bAsc = TRUE;
	SortTagsInLexVarArray(m_ActiveTags, bAsc, bNoCase);
	//m_ActiveTags.BuildAutomat(TRUE);
}

BOOL CXTPSyntaxEditLexClass::IsCaseSensitive()
{
	if (m_bCaseSensitive_Cached < 0)
	{
		m_bCaseSensitive_Cached = FALSE;

		CXTPSyntaxEditLexVariantPtr ptrCase = GetAttribute(XTPLEX_ATTR_CASESENSITIVE, TRUE);
		if (ptrCase && ptrCase->m_nObjType == xtpEditLVT_valInt)
		{
			m_bCaseSensitive_Cached = ptrCase->m_nValue > 0;
		}
	}
	return m_bCaseSensitive_Cached;
}

BOOL CXTPSyntaxEditLexClass::IsCollapsable()
{
	if (m_bCollapsable_Cached < 0)
	{
		m_bCollapsable_Cached = FALSE;

		CXTPSyntaxEditLexVariantPtr ptrLV = GetAttribute(XTPLEX_ATTR_COLLAPSABLE, FALSE);
		if (ptrLV && ptrLV->m_nObjType == xtpEditLVT_valInt)
		{
			m_bCollapsable_Cached = ptrLV->m_nValue > 0;
		}
	}
	return m_bCollapsable_Cached;
}

BOOL CXTPSyntaxEditLexClass::IsEndClassParent_this()
{
	if (m_bEndClassParent_this_Cached < 0)
	{
		m_bEndClassParent_this_Cached = FALSE;

		CXTPSyntaxEditLexVariantPtr ptrAttrVal = GetAttribute(XTPLEX_ATTR_ENDCLASSPARENT, FALSE);
		ASSERT(!ptrAttrVal || ptrAttrVal && ptrAttrVal->m_nObjType == xtpEditLVT_valStr);

		if (ptrAttrVal && ptrAttrVal->m_nObjType == xtpEditLVT_valStr)
		{
			m_bEndClassParent_this_Cached = Find_noCase(ptrAttrVal->m_arStrVals, _T("this")) >= 0;
		}
	}
	return m_bEndClassParent_this_Cached;
}

BOOL CXTPSyntaxEditLexClass::StrCmpEQ(LPCTSTR pcszStr1, LPCTSTR pcszStr2, int nLen,
							BOOL bCaseSensitive,
							BOOL bParseDirection_Back)
{
	int nRes = 0;

	if (bParseDirection_Back)
	{
		for (int i = 0; i < nLen; i++)
		{
			nRes = StrCmp(pcszStr1, pcszStr2, 1, bCaseSensitive);
			if (nRes)
			{
				break;
			}
			if (*pcszStr1 == 0 || *pcszStr2 == 0)
			{
				break;
			}
			if (i+1 < nLen)
			{
				pcszStr1 = _tcsdec(pcszStr1-2, pcszStr1);
				pcszStr2 = _tcsdec(pcszStr2-2, pcszStr2);
			}
		}
	}
	else
	{
		nRes = StrCmp(pcszStr1, pcszStr2, nLen, bCaseSensitive);
	}

	return nRes == 0;
}

BOOL CXTPSyntaxEditLexClass::StrVarCmpEQ(LPCTSTR pcszStr,
							   const CXTPSyntaxEditLexVariable& lexVar,
							   CString& rstrValue, BOOL bParseDirection_Back)
{
	UNREFERENCED_PARAMETER(bParseDirection_Back);

	BOOL bRes = FALSE;
	TCHAR chChar = *pcszStr;

	if (lexVar.m_nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_alpha)  //a-z, A-Z
	{
		bRes = chChar >= _T('a') && chChar <= _T('z') ||
				chChar >= _T('A') && chChar <= _T('Z');
	}
	else if (lexVar.m_nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_digit) // 0-9
	{
		bRes = chChar >= _T('0') && chChar <= _T('9');
	}
	else if (lexVar.m_nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_HexDigit) // 0-9, a-f, A-F
	{
		bRes = chChar >= _T('0') && chChar <= _T('9') ||
				chChar >= _T('a') && chChar <= _T('f') ||
				chChar >= _T('A') && chChar <= _T('F');
	}
	else if (lexVar.m_nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_specs)
	{
		TCHAR* pF = _tcschr((TCHAR*)cszSpecs, chChar);
		bRes = (pF != NULL);
	}
	// @EOL
	else if (lexVar.m_nVarID == CXTPSyntaxEditLexVariable::xtpEditVarID_EOL)
	{
		bRes = chChar == _T('\r') || chChar == _T('\n');
	}
	else
	{
		ASSERT(FALSE);
		TRACE(_T("ERROR! Unknown lexVariable '%s' \n"), (LPCTSTR)lexVar.m_strVar);
		return FALSE;
	}

	if (lexVar.m_nVarFlags & CXTPSyntaxEditLexVariable::xtpEditVarfNot)
	{
		bRes = !bRes;
	}

	if (bRes)
	{
		rstrValue = chChar;
	}

	return bRes;
}

CXTPSyntaxEditLexClassPtrArray* CXTPSyntaxEditLexClass::GetChildren()
{
	return &m_arChildrenClasses;
}

CXTPSyntaxEditLexClassPtrArray* CXTPSyntaxEditLexClass::GetChildrenDyn()
{
	return &m_arDynChildrenClasses;
}

CXTPSyntaxEditLexClassPtrArray* CXTPSyntaxEditLexClass::GetChildrenSelfRef()
{
	return &m_arChildrenSelfRefClasses;
}

BOOL CXTPSyntaxEditLexClass::AddChild(CXTPSyntaxEditLexClass* pChild, BOOL bDyn)
{
	if (!pChild)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPSyntaxEditLexClassPtr ptrC(pChild, TRUE);
	if (pChild == this)
	{
		int nCount = (int)m_arChildrenSelfRefClasses.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CXTPSyntaxEditLexClassPtr ptrCSelf = m_arChildrenSelfRefClasses[i];
			if (ptrC == ptrCSelf )
			{
				return FALSE;
			}
		}
		m_arChildrenSelfRefClasses.Add(ptrC);
		return TRUE;
	}

	pChild->m_Parent.ptrDirect.SetPtr(this, TRUE);
	if (bDyn)
	{
		m_arDynChildrenClasses.Add(ptrC);
	}
	else
	{
		m_arChildrenClasses.Add(ptrC);
	}
	return TRUE;
}

void CXTPSyntaxEditLexClass::GetParentOpt(int& rnOpt, CStringArray& rArData) const
{
	rnOpt = m_Parent.eOpt;
	rArData.RemoveAll();
	rArData.Append(m_Parent.arClassNames);
}

void CXTPSyntaxEditLexClass::GetChildrenOpt(int& rnOpt, CStringArray& rArData) const
{
	rnOpt = m_Children.eOpt;
	rArData.RemoveAll();
	rArData.Append(m_Children.arClassNames);
}

CXTPSyntaxEditLexClass* CXTPSyntaxEditLexClass::FindParent(LPCTSTR pcszClassName)
{
	if (!m_Parent.ptrDirect)
	{
		return NULL;
	}

	CString strParent = m_Parent.ptrDirect->GetClassName();
	if (strParent.CompareNoCase(pcszClassName) == 0)
	{
		return m_Parent.ptrDirect;
	}
	return m_Parent.ptrDirect->FindParent(pcszClassName);
}

int CXTPSyntaxEditLexClass::RunParse(CTextIter* pTxtIter,
						CXTPSyntaxEditLexTextSchema* pTxtSch,
						CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
						CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt
						)
{
	if (!pTxtIter || !pTxtSch)
	{
		ASSERT(FALSE);
		//TRACE
		return xtpEditLPR_Error | xtpEditLPR_Iterated;
	}

	if (!m_token.IsEmpty() && (!m_start.IsEmpty() || !m_end.IsEmpty()) )
	{
		TRACE(_T("WARNING: Start/End and Token options cannot be used together! '%s' \n"), (LPCTSTR)m_strClassName);
	}

	int nParseRes = 0;

	BOOL bStarted = (rPtrTxtBlock != NULL);
	BOOL bPrevCondition = TRUE;

	//===========================================================================
	if (!bStarted)
	{
		if (!m_previous.IsEmpty())
		{
			bPrevCondition = Run_Previous(pTxtIter, pTxtSch, rPtrTxtBlock,
											pOnScreenRunCnt);
		}

		//===========================================================================
		if (!m_start.IsEmpty())
		{
			if (bPrevCondition && !bStarted )
			{
				int nResStart = Run_Start(pTxtIter, pTxtSch, rPtrTxtBlock,
											pOnScreenRunCnt);
				bStarted = nResStart & xtpEditLPR_StartFound;
				nParseRes |= nResStart;

				return nParseRes;
			}
		}
		else if (bPrevCondition)
		{
			if (!m_token.IsEmpty())
			{
				int nResToken = Run_Token(pTxtIter, pTxtSch, rPtrTxtBlock);
				BOOL bEnded = nResToken & xtpEditLPR_EndFound;

				nParseRes |= nResToken;
				if (nParseRes & xtpEditLPR_Error)
				{
					return nParseRes;
				}
				//---------------------------------------------------------------------------
				if (bEnded)
				{
					return nParseRes;
				}
			}
			else
			{
				bStarted = TRUE;
				nParseRes |= xtpEditLPR_StartFound|xtpEditLPR_Iterated;

				rPtrTxtBlock = pTxtSch->GetNewBlock();
				if (rPtrTxtBlock)
				{
					rPtrTxtBlock->m_ptrLexClass.SetPtr(this, TRUE);
					rPtrTxtBlock->m_PosStartLC = pTxtIter->GetPosLC();
					rPtrTxtBlock->m_nStartTagLen = 0;
				}
				else
				{
					nParseRes |= xtpEditLPR_Error;
				}

				return nParseRes;
			}
		}
	} // if (!bStarted)

	//---------------------------------------------------------------------------
	if (!bStarted)
	{
		return 0;
	}

	//===========================================================================
	if (!m_skip.IsEmpty())
	{
		int nResSkip = Run_Skip(pTxtIter, pTxtSch, rPtrTxtBlock);

		nParseRes |= nResSkip;
		if (nParseRes & xtpEditLPR_Error)
		{
			return nParseRes;
		}
	}

	//===========================================================================
	if (!m_end.IsEmpty())
	{
		int nResEnd = Run_End(pTxtIter, pTxtSch, rPtrTxtBlock, pOnScreenRunCnt);
		BOOL bEnded = nResEnd & xtpEditLPR_EndFound;

		nParseRes |= nResEnd;
		if (nParseRes & xtpEditLPR_Error)
		{
			return nParseRes;
		}

		//---------------------------------------------------------------------------
		if (bEnded)
		{
			return nParseRes;
		}
	}

	//===========================================================================
	// *** run childs ***
	int nChildsCount = (int)m_arChildrenClasses.GetSize() +
						(int)m_arDynChildrenClasses.GetSize() +
						(int)m_arChildrenSelfRefClasses.GetSize();
	if (nChildsCount)
	{
		int nCHres = pTxtSch->RunChildren(pTxtIter, rPtrTxtBlock, this, pOnScreenRunCnt);
		nParseRes |= ~(xtpEditLPR_StartFound|xtpEditLPR_EndFound) & nCHres;

		if (nParseRes & (xtpEditLPR_Error|xtpEditLPR_RunBreaked|xtpEditLPR_RunFinished))
		{
			return nParseRes;
		}

		//---------------------------------------------------------------------------
		int nResEnd = Run_End(pTxtIter, pTxtSch, rPtrTxtBlock, pOnScreenRunCnt);
		nParseRes |= nResEnd;
	}
	return nParseRes;
}

BOOL CXTPSyntaxEditLexClass::Run_Previous(CTextIter* pTxtIter,
								 CXTPSyntaxEditLexTextSchema* pTxtSch,
								 CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
								 CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt)
{
	UNREFERENCED_PARAMETER(rPtrTxtBlock);

	BOOL bRes_Class = TRUE, bRes_Tag = TRUE;

	//=======================================================================
	if (!m_previous.m_class.IsEmpty())
	{
		bRes_Class = Run_PrevClass(NULL, pTxtSch, &m_previous.m_class,
									c_nPrevClassDepth_default, NULL,
									 NULL, pOnScreenRunCnt);
		if (!bRes_Class)
		{
			return FALSE;
		}
	}

	//=======================================================================
	if (!m_previous.m_tag.IsEmpty())
	{
		bRes_Tag = FALSE;

		if (!m_previous.m_tag_separators.IsEmpty())
		{
			int nMaxBackOffset = pTxtIter->GetMaxBackOffset();
			BOOL bContinue = TRUE;
			int nOffset;
			for (nOffset = 0; bContinue && labs(nOffset) < nMaxBackOffset; nOffset--)
			{
				CString strSep;
				bContinue = Run_Tags(pTxtIter, pTxtSch, &m_previous.m_tag_separators,
									 strSep, TRUE);

				if (bContinue)
				{
					pTxtIter->SetTxtOffset(nOffset-1);
				}
			}
			ASSERT(labs(nOffset) < nMaxBackOffset);
		}

		CString strTag;

		bRes_Tag = Run_Tags(pTxtIter, pTxtSch, &m_previous.m_tag, strTag, TRUE);

		pTxtIter->SetTxtOffset(0);
	}

	return bRes_Class && bRes_Tag;
}


BOOL CXTPSyntaxEditLexClass::Run_PrevClass(CXTPSyntaxEditLexTextBlock** ppPrevTB,
								 CXTPSyntaxEditLexTextSchema* pTxtSch,
								 CXTPSyntaxEditLexVariantPtrArray* pLVPrevClasses,
								 int nDepth, XTP_EDIT_LINECOL* pMinPrevPos,
								 CXTPSyntaxEditLexTextBlock* pPrevForParentBlockOnly,
								 CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt)
{
	if (ppPrevTB)
	{
		*ppPrevTB = NULL;
	}

	CXTPSyntaxEditLexTextBlock* pTBprev = NULL;
	if (pOnScreenRunCnt)
	{
		pTBprev = pOnScreenRunCnt->m_ptrTBLast;
	}
	else
	{
		pTBprev = pTxtSch->GetPrevBlock(FALSE);
	}

	if (!pLVPrevClasses || !pTBprev || !pTBprev->m_ptrLexClass)
	{
		return FALSE;
	}
	if (pPrevForParentBlockOnly && pPrevForParentBlockOnly != pTBprev->m_ptrParent)
	{
		return FALSE;
	}

	//=======================================================================
	for (int nD = 0;
		pTBprev && pTBprev->m_ptrLexClass &&
		(!pMinPrevPos || pTBprev->m_PosStartLC >= *pMinPrevPos) &&
		nD < nDepth;
		nD++, pTBprev = pTBprev->m_ptrPrev)
	{
		const CString& strPrevClassName = pTBprev->m_ptrLexClass->m_strClassName;

		int nPCCont1 = (int)pLVPrevClasses->GetSize();
		for (int i = 0; i < nPCCont1; i++)
		{
			CXTPSyntaxEditLexVariant* pVarPrevC = pLVPrevClasses->GetAt(i, FALSE);
			if (!pVarPrevC || !pVarPrevC->IsStrType())
			{
				ASSERT(FALSE);
				continue;
			}
			ASSERT(pVarPrevC->m_nObjType == xtpEditLVT_className);

			int nCCount2 = (int)pVarPrevC->m_arStrVals.GetSize();
			for (int k = 0; k < nCCount2; k++)
			{
				const CString& strCName = pVarPrevC->m_arStrVals[k];
				if (strPrevClassName.CompareNoCase(strCName) == 0)
				{

					if (ppPrevTB)
					{
						*ppPrevTB = pTBprev;
					}
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

int CXTPSyntaxEditLexClass::Run_Start(CTextIter* pTxtIter,
						CXTPSyntaxEditLexTextSchema* pTxtSch,
						CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
						CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt
						)
{
	if (!pTxtIter || !pTxtSch)
	{
		ASSERT(FALSE);
		//TRACE
		return xtpEditLPR_Error;
	}

	BOOL bStarted = (rPtrTxtBlock != NULL);
	ASSERT(!bStarted);

	if (bStarted || m_start.IsEmpty())
	{
		return 0;
	}

	//===========================================================================
	BOOL bRes_Class = TRUE;
	CXTPSyntaxEditLexTextBlock* pPrevClass = NULL;

	if (!m_start.m_class.IsEmpty())
	{
		bRes_Class = Run_PrevClass(&pPrevClass, pTxtSch, &m_start.m_class,
							c_nPrevClassDepth_default, NULL,  NULL, pOnScreenRunCnt);
		if (!bRes_Class)
		{
			return 0;
		}
	}

	//===========================================================================
	BOOL bRes_Tag = TRUE;
	CString strTagVal;
	BOOL bTag_Empty = m_start.m_tag.IsEmpty();

	if (!bTag_Empty)
	{
		bRes_Tag = Run_Tags(pTxtIter, pTxtSch, &m_start.m_tag, strTagVal);
		if (!bRes_Tag)
		{
			return 0;
		}
	}

	bStarted = bRes_Class && bRes_Tag;
	int nRes = 0;
	if (bStarted)
	{
		nRes |= xtpEditLPR_StartFound;
		if (!m_start.m_class.IsEmpty() && pPrevClass && pPrevClass->m_ptrLexClass &&
			m_Parent.ptrDirect )
		{
			CString strC0 = m_Parent.ptrDirect->GetClassName();
			CString strC1 = pPrevClass->m_ptrLexClass->GetClassName();

			if (strC0.CompareNoCase(strC1) != 0)
			{
				nRes |= xtpEditLPR_TBpop1;
			}
		}

		rPtrTxtBlock = pTxtSch->GetNewBlock();
		if (rPtrTxtBlock)
		{
			rPtrTxtBlock->m_ptrLexClass = this;
			rPtrTxtBlock->m_ptrLexClass->InternalAddRef();

			if (bRes_Tag && !bTag_Empty)
			{
				int nLenC = (int)_tcsclen(strTagVal);
				ASSERT(nLenC);

				rPtrTxtBlock->m_PosStartLC = pTxtIter->GetPosLC();
				rPtrTxtBlock->m_nStartTagLen = nLenC;

				pTxtIter->SeekNext(nLenC);
				nRes |= xtpEditLPR_Iterated;
			}
			else
			{
				CSingleLock singleLock(pTxtSch->GetDataLoker(), TRUE);

				ASSERT(pPrevClass);
				if (pPrevClass)
				{
					rPtrTxtBlock->m_PosStartLC = pPrevClass->m_PosStartLC;
					rPtrTxtBlock->m_nStartTagLen = pPrevClass->m_nStartTagLen;
				}
			}
		}

		if (!rPtrTxtBlock)
		{
			return xtpEditLPR_Error | nRes;
		}
	}

	//---------------------------------------------------------------------------
	return nRes;
}

int CXTPSyntaxEditLexClass::Run_Skip(CTextIter* pTxtIter,
							CXTPSyntaxEditLexTextSchema* pTxtSch,
							CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock
							)
{
	if (!pTxtIter || !pTxtSch)
	{
		ASSERT(FALSE);
		//TRACE
		return xtpEditLPR_Error;
	}

	BOOL bStarted = (rPtrTxtBlock != NULL);
	ASSERT(bStarted);

	if (!bStarted || m_skip.IsEmpty())
	{
		return 0;
	}

	//===========================================================================
	CString strTagVal;

	if (!m_skip.m_tag.IsEmpty())
	{
		BOOL bIterated = FALSE;

		while (!pTxtIter->IsEOF() && Run_Tags(pTxtIter, pTxtSch, &m_skip.m_tag, strTagVal))
		{
			int nLen = strTagVal.GetLength();
			pTxtIter->SeekNext(nLen);

			bIterated = TRUE;
		}
		if (bIterated)
		{
			return xtpEditLPR_Iterated;
		}
	}

	return 0;
}

int CXTPSyntaxEditLexClass::Run_End(CTextIter* pTxtIter,
						CXTPSyntaxEditLexTextSchema* pTxtSch,
						CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
						CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt)
{
	if (!pTxtIter || !pTxtSch)
	{
		ASSERT(FALSE);
		//TRACE
		return xtpEditLPR_Error;
	}

	BOOL bEnded = FALSE;

	if (m_end.IsEmpty())
	{
		return 0;
	}

	ASSERT(rPtrTxtBlock);

	//===========================================================================
	BOOL bRes_Class = TRUE;
	CXTPSyntaxEditLexTextBlock* pPrevClass = NULL;

	if (!m_end.m_class.IsEmpty())
	{
		CXTPSyntaxEditLexTextBlock* pTBEndParent = IsEndClassParent_this() ? rPtrTxtBlock : NULL;

		bRes_Class = Run_PrevClass(&pPrevClass, pTxtSch, &m_end.m_class,
						c_nPrevClassDepth_default, &rPtrTxtBlock->m_PosStartLC,
						pTBEndParent, pOnScreenRunCnt);
		if (!bRes_Class)
		{
			return 0;
		}
	}

	//===========================================================================
	BOOL bRes_Tag = TRUE;
	CString strTagVal;
	int nTagLenC = 0;

	if (!m_end.m_tag.IsEmpty())
	{
		bRes_Tag = Run_Tags(pTxtIter, pTxtSch, &m_end.m_tag, strTagVal);
		if (!bRes_Tag)
		{
			return 0;
		}
		nTagLenC = (int)_tcsclen(strTagVal);
	}

	//===========================================================================
	BOOL bRes_Separators = TRUE;
	CString strSepVal;

	if (!m_end.m_separators.IsEmpty())
	{
		pTxtIter->SetTxtOffset(nTagLenC);

		bRes_Separators = Run_Tags(pTxtIter, pTxtSch, &m_end.m_separators, strSepVal);

		pTxtIter->SetTxtOffset(0);

		if (!bRes_Separators)
		{
			return 0;
		}
	}

	//-----------------------------------------------------------------------
	bEnded = bRes_Class && bRes_Tag && bRes_Separators;
	int nRes = 0;

	if (bEnded)
	{
		nRes |= xtpEditLPR_EndFound;
		nRes |= xtpEditLPR_Iterated;

		if (bRes_Tag && !m_end.m_tag.IsEmpty() ||
			bRes_Separators && !m_end.m_separators.IsEmpty())
		{
			CSingleLock singleLock(pTxtSch->GetDataLoker(), TRUE);

			XTP_EDIT_LINECOL runLC = pTxtIter->GetPosLC();

			if (bRes_Tag && !m_end.m_tag.IsEmpty())
			{
				pTxtIter->SeekNext(nTagLenC);
			}

			ASSERT(this == (CXTPSyntaxEditLexClass*)rPtrTxtBlock->m_ptrLexClass);

			rPtrTxtBlock->m_PosEndLC = pTxtIter->GetPosLC();

			const XTP_EDIT_LINECOL beginLC = {1, 0};
			if (rPtrTxtBlock->m_PosEndLC > beginLC)
			{
				pTxtIter->LCPosDec(rPtrTxtBlock->m_PosEndLC);
			}
			else
			{
				TRACE(_T("WARNING (parser): unexpected end position (1,0) \n"));
			}

			if (bRes_Separators && !m_end.m_separators.IsEmpty())
			{
				int nSepLenC = (int)_tcsclen(strSepVal);

				XTP_EDIT_LINECOL endLC2 = rPtrTxtBlock->m_PosEndLC;

				pTxtIter->LCPosAdd(endLC2, nTagLenC+nSepLenC);

				rPtrTxtBlock->m_nEndTagXLCLen = rPtrTxtBlock->m_PosEndLC.GetXLC() -
												endLC2.GetXLC();
			}
			else
			{
				rPtrTxtBlock->m_nEndTagXLCLen = rPtrTxtBlock->m_PosEndLC.GetXLC() -
												runLC.GetXLC();
			}
		}
		else
		{
			CSingleLock singleLock(pTxtSch->GetDataLoker(), TRUE);

			ASSERT(pPrevClass);
			if (pPrevClass)
			{
				rPtrTxtBlock->m_PosEndLC = pPrevClass->m_PosEndLC;
				rPtrTxtBlock->m_nEndTagXLCLen = pPrevClass->m_nEndTagXLCLen;
			}
		}
	}

	//---------------------------------------------------------------------------
	return nRes;
}

int CXTPSyntaxEditLexClass::Run_Token(CTextIter* pTxtIter,
						  CXTPSyntaxEditLexTextSchema* pTxtSch,
						  CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock )
{
	if (!pTxtIter || !pTxtSch)
	{
		ASSERT(FALSE);
		//TRACE
		return xtpEditLPR_Error;
	}

	//===========================================================================
	if (!m_token.m_tag.IsEmpty())
	{
		CString strTokenVal;
		//if (Run_Tags(pTxtIter, pTxtSch, &m_token.m_tag, strTokenVal))
		//BOOL bTagBegins = Run_Tags(pTxtIter, pTxtSch, &m_token.m_tag, strTokenVal);

		BOOL bTagBegins = m_token.m_tag.FindMaxWord(pTxtIter->GetText(1024),
								strTokenVal, 1024, FALSE, !IsCaseSensitive());

#ifdef DBG_AUTOMAT
		BOOL bTagBeginsX = Run_Tags(pTxtIter, pTxtSch, &m_token.m_tag, strTokenVal);
		if (bTagBeginsX  != bTagBegins)
		{
			ASSERT(FALSE);
		}
#endif
		if (bTagBegins)
		{
			CString strSeparator1, strSeparator2;
			if (Run_TokenSeparators(strTokenVal, pTxtIter, pTxtSch,
									strSeparator1, strSeparator2) )
			{
				int nLenC = (int)_tcsclen(strTokenVal);

				rPtrTxtBlock = pTxtSch->GetNewBlock();
				if (rPtrTxtBlock)
				{
					rPtrTxtBlock->m_ptrLexClass.SetPtr(this, TRUE);

					rPtrTxtBlock->m_PosStartLC = pTxtIter->GetPosLC();
					rPtrTxtBlock->m_nStartTagLen = nLenC;
				}

				pTxtIter->SeekNext(nLenC);

				int nRes = xtpEditLPR_StartFound | xtpEditLPR_EndFound | xtpEditLPR_Iterated;

				if (rPtrTxtBlock)
				{
					rPtrTxtBlock->m_PosEndLC = pTxtIter->GetPosLC();
					if (nLenC)
					{
						pTxtIter->LCPosDec(rPtrTxtBlock->m_PosEndLC);
					}
					rPtrTxtBlock->m_nEndTagXLCLen = rPtrTxtBlock->m_PosEndLC.GetXLC() -
										rPtrTxtBlock->m_PosStartLC.GetXLC();
				}
				else
				{
					nRes |= xtpEditLPR_Error;
				}

				return nRes;
			}
		}
	}

	return 0;
}

BOOL CXTPSyntaxEditLexClass::Run_TokenSeparators(CString strToken,
									  CTextIter* pTxtIter,
									  CXTPSyntaxEditLexTextSchema* pTxtSch,
									  CString& rstrSeparator1,
									  CString& rstrSeparator2)
{
	rstrSeparator1.Empty();
	rstrSeparator2.Empty();

	//===========================================================================
	BOOL bTSyes = TRUE;
	BOOL bTEyes = TRUE;

	if (!m_token.m_start_separators.IsEmpty())
	{
		bTSyes = Run_Tags(pTxtIter, pTxtSch, &m_token.m_start_separators, rstrSeparator1, TRUE);
	}

	if (!m_token.m_end_separators.IsEmpty())
	{
		int nLen = strToken.GetLength();
		pTxtIter->SetTxtOffset(nLen);

		bTEyes = Run_Tags(pTxtIter, pTxtSch, &m_token.m_end_separators, rstrSeparator2);

		pTxtIter->SetTxtOffset(0);
	}

	return bTSyes && bTEyes;
}

BOOL CXTPSyntaxEditLexClass::Run_Tags(CTextIter* pIter,
							 CXTPSyntaxEditLexTextSchema* pTxtSch,
							 CXTPSyntaxEditLexVariantPtrArray* pLVTags,
							 CString& rstrTagVal, BOOL bParseDirection_Back
							 )
{
	UNREFERENCED_PARAMETER(pTxtSch);

	BOOL bCaseSensitive = IsCaseSensitive();

	return Run_Tags1(pIter, pLVTags, rstrTagVal, bCaseSensitive, bParseDirection_Back);
}

BOOL CXTPSyntaxEditLexClass::Run_Tags1(CTextIter* pIter,
							CXTPSyntaxEditLexVariantPtrArray* pLVTags,
							CString& rstrTagVal, BOOL bCaseSensitive,
							BOOL bParseDirection_Back
							)
{
	if (!pLVTags)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nCount = (int)pLVTags->GetSize();

	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexVariant* pVTag = pLVTags->GetAt(i, FALSE);

		if (Run_Tags2(pIter, pVTag, rstrTagVal, bCaseSensitive, bParseDirection_Back) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

int CXTPSyntaxEditLexClass::Run_Tags2(CTextIter* pIter,
							CXTPSyntaxEditLexVariant* pVTags,
							CString& rstrTagVal,
							BOOL bCaseSensitive,
							BOOL bParseDirection_Back
						   )
{
	LPCTSTR pText = pIter->GetText();

	if (!pVTags)
	{
		ASSERT(FALSE);
		//TRACE
		return 0;
	}

	if (pVTags->m_nObjType == xtpEditLVT_valStr)
	{
		if (bParseDirection_Back)
		{
			pText = _tcsdec(pText-5, pText);
		}

		for (int k = 0; k < pVTags->m_arStrVals.GetSize(); k++)
		{
			LPCTSTR pcszTag = pVTags->m_arStrVals[k];
			int nLen = (int)_tcsclen(pcszTag);

			if (nLen <= 0)
			{
				ASSERT(FALSE);
				continue;
			}

			if (bParseDirection_Back)
			{
				pcszTag = _tcsninc(pcszTag, nLen-1);
			}

			if (StrCmpEQ(pcszTag, pText, nLen, bCaseSensitive, bParseDirection_Back))
			{
				rstrTagVal = pcszTag;
				return TRUE;
			}
		}
		return FALSE;
	}
	else if (pVTags->m_nObjType == xtpEditLVT_valVar)
	{
		if (bParseDirection_Back)
		{
			pText = _tcsdec(pText-5, pText);
		}
		if (StrVarCmpEQ(pText, pVTags->m_Variable, rstrTagVal, bParseDirection_Back))
		{
			return TRUE;
		}

		return FALSE;
	}
	else
	{
		ASSERT(FALSE);
		//TRACE
	}
	return FALSE;
}

void CXTPSyntaxEditLexClass::CopyFrom(const CXTPSyntaxEditLexClass* pSrc)
{
	if (!pSrc)
	{
		ASSERT(FALSE);
		return;
	}

	m_Parent.eOpt = pSrc->m_Parent.eOpt;
	m_Parent.ptrDirect = pSrc->m_Parent.ptrDirect;

	m_Parent.arClassNames.RemoveAll();
	m_Parent.arClassNames.Append(pSrc->m_Parent.arClassNames);

	m_Children.eOpt = pSrc->m_Children.eOpt;

	m_Children.arClassNames.RemoveAll();
	m_Children.arClassNames.Append(pSrc->m_Children.arClassNames);

	m_strClassName = pSrc->GetClassName();

	TBase::CopyFrom(*pSrc);

	CopyAttributes(pSrc);

	m_arChildrenClasses.RemoveAll();
	m_arDynChildrenClasses.RemoveAll();
	m_arChildrenSelfRefClasses.RemoveAll();
}

const CXTPSyntaxEditLexClass& CXTPSyntaxEditLexClass::operator=(const CXTPSyntaxEditLexClass& rSrc)
{
	CopyFrom(&rSrc);

	return *this;
}

CXTPSyntaxEditLexClass* CXTPSyntaxEditLexClass::Clone(CXTPSyntaxEditLexClassSchema* pOwnerSch)
{
	CXTPSyntaxEditLexClass* pNewClass = NULL;
	if (pOwnerSch)
	{
		pNewClass = pOwnerSch->GetNewClass(FALSE);
	}
	else
	{
		pNewClass = new CXTPSyntaxEditLexClass();
	}

	if (pNewClass)
	{
		pNewClass->CopyFrom(this);
	}

	return pNewClass;
}

void CXTPSyntaxEditLexClass::RemoveAttributes(LPCTSTR pcszAttrPrefix)
{
	ClearAttributesCache();

	if (pcszAttrPrefix)
	{
		int nLenC = (int)_tcsclen(pcszAttrPrefix);
		CString strKey;
		CStringArray arKeysToRemove;

		POSITION pos = m_mapAttributes.GetStartPosition();
		while (pos)
		{
			CXTPSyntaxEditLexVariantPtr ptrVal;
			m_mapAttributes.GetNextAssoc(pos, strKey, ptrVal);
			if (_tcsnicmp(pcszAttrPrefix, strKey, nLenC) == 0)
			{
				arKeysToRemove.Add(strKey);
			}
		}

		//-----------------------------------------
		int nCount = (int)arKeysToRemove.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			strKey = arKeysToRemove[i];
			m_mapAttributes.RemoveKey(strKey);
		}
	}
	else
	{
		m_mapAttributes.RemoveAll();
	}
}

void CXTPSyntaxEditLexClass::CopyAttributes(const CXTPSyntaxEditLexClass* pSrcAttrClass,
								  LPCTSTR pcszAttrPrefix)
{
	RemoveAttributes(pcszAttrPrefix);

	if (!pSrcAttrClass)
	{
		ASSERT(FALSE);
		return;
	}

	int nLenC = pcszAttrPrefix ? (int)(_tcsclen(pcszAttrPrefix)) : 0;
	CString strKey;

	POSITION pos = pSrcAttrClass->m_mapAttributes.GetStartPosition();
	while (pos)
	{
		CXTPSyntaxEditLexVariantPtr ptrVal;
		pSrcAttrClass->m_mapAttributes.GetNextAssoc(pos, strKey, ptrVal);

		if (pcszAttrPrefix)
		{
			if (_tcsnicmp(pcszAttrPrefix, strKey, nLenC) == 0)
			{
				m_mapAttributes[strKey] = ptrVal;
			}
		}
		else
		{
			m_mapAttributes[strKey] = ptrVal;
		}
	}
}

void CXTPSyntaxEditLexClass::SetAttribute(CString strName, const CXTPSyntaxEditLexVariant& rVal)
{
	ClearAttributesCache();

	strName.MakeLower();
	CXTPSyntaxEditLexVariantPtr ptrData = new CXTPSyntaxEditLexVariant(rVal);

	m_mapAttributes[strName] = ptrData;
}

CXTPSyntaxEditLexVariantPtr CXTPSyntaxEditLexClass::GetAttribute(LPCTSTR strName, BOOL bDyn) const
{
	CString strName_lower = strName;
	strName_lower.MakeLower();
	CXTPSyntaxEditLexVariantPtr ptrData;

	if (m_mapAttributes.Lookup(strName_lower, ptrData))
	{
		return ptrData;
	}
	if (bDyn && m_Parent.ptrDirect)
	{
		ptrData = m_Parent.ptrDirect->GetAttribute(strName_lower, TRUE);
		return ptrData;
	}
	return NULL;
}

#ifdef _DEBUG
void CXTPSyntaxEditLexClass::DumpOffset(CDumpContext& dc, LPCTSTR pcszOffset)
{
	if (!pcszOffset)
	{
		pcszOffset = _T("");
	}
	int i;
	dc << _T("\n");

	dc << pcszOffset;
	dc << _T("lexClass:   NAME=") << m_strClassName << _T(" \n");

	//------------------------------------
	dc << pcszOffset;
	if (m_Parent.eOpt == xtpEditOptParent_file)
	{
		dc << _T("parent:file = ");
	}
	else if (m_Parent.eOpt == xtpEditOptParent_direct)
	{
		dc << _T("parent = ");
	}
	else if (m_Parent.eOpt == xtpEditOptParent_dyn)
	{
		dc << _T("parent:dyn = ");
	}
	else
	{
		dc << _T("parent:??? = ");
	}

	for (i = 0; i < m_Parent.arClassNames.GetSize(); i++)
	{
		dc << m_Parent.arClassNames[i] << _T(", ");
	}
	if (m_Parent.ptrDirect)
	{
		dc << _T(" // [parent_by_ptr=[") << m_Parent.ptrDirect->m_strClassName << _T("] ");
	}
	dc << _T(" \n");

	//------------------------------------
	if (m_Children.eOpt == xtpEditOptChildren_No)
	{
		dc << pcszOffset;
		dc << _T("children = 0 \n");
	}
	if (m_Children.eOpt == xtpEditOptChildren_List)
	{
		dc << pcszOffset;
		dc << _T("children = ");
		for (i = 0; i < m_Children.arClassNames.GetSize(); i++)
		{
			dc <<  m_Children.arClassNames[i] << _T(", ");
		}
		dc << _T(" \n");
	}

	//------------------------------------

	m_previous.DumpOffset(dc, pcszOffset);

	m_start.DumpOffset(dc, pcszOffset);
	m_end.DumpOffset(dc, pcszOffset);

	m_token.DumpOffset(dc, pcszOffset);

	m_skip.DumpOffset(dc, pcszOffset);

	m_ActiveTags.DumpOffset(dc, pcszOffset);
}
#endif

void CXTPSyntaxEditLexClass::GetTextAttributes(XTP_EDIT_TEXTBLOCK& rTB)
{
	if (!m_bTxtAttr_cached)
	{
		m_bTxtAttr_cached = TRUE;

		m_txtAttr_cached.clrBlock.crText = COLORREF_NULL;
		m_txtAttr_cached.clrBlock.crBack = COLORREF_NULL;
		m_txtAttr_cached.clrBlock.crHiliteText  = COLORREF_NULL;
		m_txtAttr_cached.clrBlock.crHiliteBack = COLORREF_NULL;

		m_txtAttr_cached.lf.lfWeight = XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION;
		m_txtAttr_cached.lf.lfItalic = XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION;
		m_txtAttr_cached.lf.lfUnderline = XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION;

		//====================================================================
		CXTPSyntaxEditLexVariantPtr ptrLVA;
		ptrLVA = GetAttribute(XTPLEX_ATTR_TXT_COLORFG, TRUE);
		if (ptrLVA && ptrLVA->m_nObjType == xtpEditLVT_valInt)
		{
			m_txtAttr_cached.clrBlock.crText = (COLORREF)ptrLVA->m_nValue;
		}

		ptrLVA = GetAttribute(XTPLEX_ATTR_TXT_COLORBK, TRUE);
		if (ptrLVA && ptrLVA->m_nObjType == xtpEditLVT_valInt)
		{
			m_txtAttr_cached.clrBlock.crBack = (COLORREF)ptrLVA->m_nValue;
		}

		ptrLVA = GetAttribute(XTPLEX_ATTR_TXT_COLORSELFG, TRUE);
		if (ptrLVA && ptrLVA->m_nObjType == xtpEditLVT_valInt)
		{
			m_txtAttr_cached.clrBlock.crHiliteText  = (COLORREF)ptrLVA->m_nValue;
		}

		ptrLVA = GetAttribute(XTPLEX_ATTR_TXT_COLORSELBK, TRUE);
		if (ptrLVA && ptrLVA->m_nObjType == xtpEditLVT_valInt)
		{
			m_txtAttr_cached.clrBlock.crHiliteBack = (COLORREF)ptrLVA->m_nValue;
		}

		//===========================================================================
		ptrLVA = GetAttribute(XTPLEX_ATTR_TXT_BOLD, TRUE);
		if (ptrLVA && ptrLVA->m_nObjType == xtpEditLVT_valInt)
		{
			m_txtAttr_cached.lf.lfWeight =
					ptrLVA->m_nValue ? FW_BOLD : XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION;
		}

		ptrLVA = GetAttribute(XTPLEX_ATTR_TXT_ITALIC, TRUE);
		if (ptrLVA && ptrLVA->m_nObjType == xtpEditLVT_valInt)
		{
			m_txtAttr_cached.lf.lfItalic =
					ptrLVA->m_nValue ? (BYTE)1 : XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION;
		}

		ptrLVA = GetAttribute(XTPLEX_ATTR_TXT_UNDERLINE, TRUE);
		if (ptrLVA && ptrLVA->m_nObjType == xtpEditLVT_valInt)
		{
			m_txtAttr_cached.lf.lfUnderline =
					ptrLVA->m_nValue ? (BYTE)1 : XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION;
		}
	}

	//===========================================================================
	if (m_txtAttr_cached.clrBlock.crText != COLORREF_NULL)
	{
		rTB.clrBlock.crText = m_txtAttr_cached.clrBlock.crText;
	}
	if (m_txtAttr_cached.clrBlock.crBack != COLORREF_NULL)
	{
		rTB.clrBlock.crBack = m_txtAttr_cached.clrBlock.crBack;
	}
	if (m_txtAttr_cached.clrBlock.crHiliteText != COLORREF_NULL)
	{
		rTB.clrBlock.crHiliteText = m_txtAttr_cached.clrBlock.crHiliteText;
	}
	if (m_txtAttr_cached.clrBlock.crHiliteBack != COLORREF_NULL)
	{
		rTB.clrBlock.crHiliteBack = m_txtAttr_cached.clrBlock.crHiliteBack;
	}

	rTB.lf.lfWeight = m_txtAttr_cached.lf.lfWeight;
	rTB.lf.lfItalic = m_txtAttr_cached.lf.lfItalic;
	rTB.lf.lfUnderline = m_txtAttr_cached.lf.lfUnderline;
}

CXTPSyntaxEditLexVariantPtrArray* CXTPSyntaxEditLexClass::BuildActiveTags(int& rnStartCount)
{
	m_ActiveTags.RemoveAll();
	m_nActiv_EndTags_Offset = 0;

	//== start class tags =====
	const TCHAR* s_arActiveTagsObjs1[] = {
			_T("start:Tag"),
			_T("previous:tag"),
			_T("previous:tag:separators"),
			_T("Token:tag")
	};

	//== end class tags =====
	const TCHAR* s_arActiveTagsObjs2[] = {
			_T("skip:Tag"),
			_T("end:Tag"),
			_T("end:separators")
	};

	//== 1. process start tags =====
	int a;
	for (a = 0; a < _countof(s_arActiveTagsObjs1); a++)
	{
		CXTPSyntaxEditLexVariantPtrArray* ptrAT = GetIfMy(s_arActiveTagsObjs1[a]);
		ASSERT(ptrAT);
		if (ptrAT)
		{
			ConcatenateLVArrays(&m_ActiveTags, ptrAT);

			// if start:Tag not empty - skip previous tags.
			// else previous tags are used to determine class start.
			if (a == 0 && ptrAT->GetSize() > 0)
			{
				a += 2;
			}
		}
	}

	//m_ActiveTags.Dump(_T("~a1~ "));

	//== 2. process children and add start tags only =====
	CXTPSyntaxEditLexClassPtrArray* arChildren[2];

	arChildren[0] = GetChildren();
	arChildren[1] = GetChildrenDyn();

	int i;
	for (i = 0; i < _countof(arChildren); i++)
	{
		int nKCount = arChildren[i] ? (int)arChildren[i]->GetSize() : 0;
		int k;
		for (k = 0; k < nKCount; k++)
		{
			CXTPSyntaxEditLexClass* pCh = arChildren[i]->GetAt(k, FALSE);
			ASSERT(pCh);

			int nStartCount = 0;
			CXTPSyntaxEditLexVariantPtrArray* ptrATch = pCh->BuildActiveTags(nStartCount);
			if (ptrATch)
			{
				ConcatenateLVArrays(&m_ActiveTags, ptrATch, nStartCount);
			}
		}
	}

	//== 3. process end tags =====
	m_nActiv_EndTags_Offset = (int)m_ActiveTags.GetSize();

	for (a = 0; a < _countof(s_arActiveTagsObjs2); a++)
	{
		CXTPSyntaxEditLexVariantPtrArray* ptrAT = GetIfMy(s_arActiveTagsObjs2[a]);
		ASSERT(ptrAT);
		if (ptrAT)
		{
			ConcatenateLVArrays(&m_ActiveTags, ptrAT);
		}
	}

	//m_ActiveTags.Dump(_T("~a2~ "));

	//=======================
	rnStartCount = m_nActiv_EndTags_Offset;

	m_ActiveTags.BuildAutomat(TRUE);

	//********************
	m_token.m_tag.BuildAutomat(FALSE);
	//********************
	return &m_ActiveTags;
}
