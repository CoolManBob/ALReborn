// XTPSyntaxEditLineMarksManager.cpp : implementation file
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
#include "Common/XTPSmartPtrInternalT.h"

// syntax editor includes
#include "XTPSyntaxEditLineMarksManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditLineMarksManager

IMPLEMENT_DYNAMIC(CXTPSyntaxEditLineMarksManager, CCmdTarget)

////////////////////////////////////////////////////////////////////////////
XTP_EDIT_LMPARAM::~XTP_EDIT_LMPARAM()
{
}

XTP_EDIT_LMPARAM::XTP_EDIT_LMPARAM()
{
	Clear();
}

XTP_EDIT_LMPARAM::XTP_EDIT_LMPARAM(const XTP_EDIT_LMPARAM& rSrc)
{
	*this = rSrc;
}

XTP_EDIT_LMPARAM::XTP_EDIT_LMPARAM(DWORD dwVal)
{
	m_eType = xtpEditLMPT_DWORD;
	m_dwValue = dwVal;
}

XTP_EDIT_LMPARAM::XTP_EDIT_LMPARAM(double dblValue)
{
	m_eType = xtpEditLMPT_double;
	m_dblValue = dblValue;
}

XTP_EDIT_LMPARAM::XTP_EDIT_LMPARAM(void* pPtr)
{
	m_eType = xtpEditLMPT_Ptr;

	*this = pPtr;
}

const XTP_EDIT_LMPARAM& XTP_EDIT_LMPARAM::operator=(const XTP_EDIT_LMPARAM& rSrc)
{
	m_eType = rSrc.m_eType;

	if (m_eType == xtpEditLMPT_Unknown)
	{
		Clear();
	}
	else if (m_eType == xtpEditLMPT_DWORD)
	{
		m_dwValue = rSrc.m_dwValue;
	}
	else if (m_eType == xtpEditLMPT_double)
	{
		m_dblValue = rSrc.m_dblValue;
	}
	else if (m_eType == xtpEditLMPT_Ptr)
	{
		m_Ptr = rSrc.m_Ptr;
	}
	else
	{
		ASSERT(FALSE);
	}

	return *this;
}

const XTP_EDIT_LMPARAM& XTP_EDIT_LMPARAM::operator=(DWORD dwValue)
{
	m_Ptr = NULL;

	m_eType = xtpEditLMPT_DWORD;
	m_dwValue = dwValue;
	return *this;
}

const XTP_EDIT_LMPARAM& XTP_EDIT_LMPARAM::operator=(double dblValue)
{
	m_Ptr = NULL;

	m_eType = xtpEditLMPT_double;
	m_dblValue = dblValue;
	return *this;
}

void XTP_EDIT_LMPARAM::SetPtr(void* pPtr, CXTPSyntaxEditVoidObj::TPFDeleter pfDeleter)
{
	ASSERT(!pPtr || pPtr != m_Ptr);

	m_eType = xtpEditLMPT_Ptr;

	CXTPSyntaxEditVoidObj* pVoidObj = NULL;
	if (pPtr)
	{
		pVoidObj = new CXTPSyntaxEditVoidObj(pPtr, pfDeleter);
	}
	m_Ptr = pVoidObj;
}

const XTP_EDIT_LMPARAM& XTP_EDIT_LMPARAM::operator=(void* pPtr)
{
	SetPtr(pPtr, NULL);
	return *this;
}

XTP_EDIT_LMPARAM::operator DWORD() const
{
	if (m_eType == xtpEditLMPT_DWORD)
	{
		return m_dwValue;
	}
	ASSERT(FALSE);
	return 0;
}

XTP_EDIT_LMPARAM::operator double() const
{
	if (m_eType == xtpEditLMPT_double)
	{
		return m_dblValue;
	}
	else if (m_eType == xtpEditLMPT_DWORD)
	{
		return m_dwValue;
	}
	ASSERT(FALSE);
	return 0;
}

XTP_EDIT_LMPARAM::operator void*() const
{
	return GetPtr();
}

void* XTP_EDIT_LMPARAM::GetPtr() const
{
	if (m_eType == xtpEditLMPT_Ptr)
	{
		return m_Ptr ? m_Ptr->GetPtr() : NULL;
	}
	return NULL;
}

BOOL XTP_EDIT_LMPARAM::IsValid() const
{
	return  m_eType == xtpEditLMPT_DWORD ||
			m_eType == xtpEditLMPT_double ||
			m_eType == xtpEditLMPT_Ptr;
}

void XTP_EDIT_LMPARAM::Clear()
{
	m_eType = xtpEditLMPT_Unknown;

	m_dwValue = 0;
	m_dblValue = 0;
	m_Ptr = NULL;
}

////////////////////////////////////////////////////////////////////////////
//struct XTP_EDIT_LMDATA

XTP_EDIT_LMDATA::~XTP_EDIT_LMDATA()
{
}

XTP_EDIT_LMDATA::XTP_EDIT_LMDATA()
{
	m_nRow = 0;
}

XTP_EDIT_LMDATA::XTP_EDIT_LMDATA(const XTP_EDIT_LMDATA& rSrc)
{
	*this = rSrc;
}

const XTP_EDIT_LMDATA& XTP_EDIT_LMDATA::operator=(const XTP_EDIT_LMDATA& rSrc)
{
	m_nRow = rSrc.m_nRow;
	m_Param = rSrc.m_Param;

	return *this;
}

/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CXTPSyntaxEditLineMarksManager::CLineMarksList::~CLineMarksList()
{
	RemoveAll();
}

int CXTPSyntaxEditLineMarksManager::CLineMarksList::FindIndex(const int nKey)
{
	const int nCount = (int)m_array.GetSize();

	// binary search
	int nLeftIndex = 0, nRightIndex = nCount - 1;
	while (nRightIndex >= nLeftIndex)
	{
		XTP_EDIT_LMDATA* pLeftData = m_array.GetAt(nLeftIndex);
		XTP_EDIT_LMDATA* pRightData = m_array.GetAt(nRightIndex);
		if (!pLeftData || !pRightData)
			break;

		if (nKey == pLeftData->m_nRow)
			return nLeftIndex;

		if (nKey == pRightData->m_nRow)
			return nRightIndex;

		int nMediumIndex = (nRightIndex + nLeftIndex) / 2;

		if (nMediumIndex == nRightIndex || nMediumIndex == nLeftIndex)
			break;

		XTP_EDIT_LMDATA* pMediumData = m_array.GetAt(nMediumIndex);
		if (!pMediumData)
			break;

		if (nKey == pMediumData->m_nRow)
		{
			return nMediumIndex;
		}
		else if (nKey > pMediumData->m_nRow)
		{
			nLeftIndex = nMediumIndex;
		}
		else // if (nKey < pMediumData->m_nRow)
		{
			nRightIndex = nMediumIndex;
		}
	}
	return -1;
}

int CXTPSyntaxEditLineMarksManager::CLineMarksList::FindLowerIndex(const int nKey)
{
	const int nCount = (int)m_array.GetSize();

	if (nCount == 0)
		return -1;

	// binary search
	int nLeftIndex = 0, nRightIndex = nCount - 1;
	while (nRightIndex >= nLeftIndex)
	{
		int nMediumIndex = (nRightIndex + nLeftIndex) / 2;

		XTP_EDIT_LMDATA* pMediumData = m_array.GetAt(nMediumIndex);
		if (!pMediumData)
			break;

		XTP_EDIT_LMDATA* pMediumNextData = nMediumIndex + 1 < nCount ?
			m_array.GetAt(nMediumIndex + 1) : NULL;

		if (nKey >= pMediumData->m_nRow &&
			(!pMediumNextData || (pMediumNextData && nKey < pMediumNextData->m_nRow)))
		{
			return nMediumIndex;
		}
		else if (nKey > pMediumData->m_nRow)
		{
			if (nLeftIndex < nMediumIndex)
				nLeftIndex = nMediumIndex;
			else if (nLeftIndex + 1 == nRightIndex)
				nLeftIndex = nRightIndex;
			else
				break;
		}
		else
		{
			if (nRightIndex > nMediumIndex)
				nRightIndex = nMediumIndex;
			else if (nRightIndex - 1 == nLeftIndex)
				nRightIndex = nLeftIndex;
			else
				break;
		}
	}

	return -1;
}

int CXTPSyntaxEditLineMarksManager::CLineMarksList::FindUpperIndex(const int nKey)
{
	const int nCount = (int)m_array.GetSize();

	if (nCount == 0)
		return -1;

	// binary search
	int nLeftIndex = 0, nRightIndex = nCount - 1;
	while (nRightIndex >= nLeftIndex)
	{
		int nMediumIndex = (nRightIndex + nLeftIndex) / 2;

		XTP_EDIT_LMDATA* pMediumData = m_array.GetAt(nMediumIndex);
		if (!pMediumData)
			break;

		XTP_EDIT_LMDATA* pMediumPrevData = nMediumIndex > 0 ?
			m_array.GetAt(nMediumIndex - 1) : NULL;

		if (nKey <= pMediumData->m_nRow &&
			(!pMediumPrevData || (pMediumPrevData && nKey > pMediumPrevData->m_nRow)))
		{
			return nMediumIndex;
		}
		else if (nKey > pMediumData->m_nRow)
		{
			if (nLeftIndex < nMediumIndex)
				nLeftIndex = nMediumIndex;
			else if (nLeftIndex + 1 == nRightIndex)
				nLeftIndex = nRightIndex;
			else
				break;
		}
		else
		{
			if (nRightIndex > nMediumIndex)
				nRightIndex = nMediumIndex;
			else if (nRightIndex - 1 == nLeftIndex)
				nRightIndex = nLeftIndex;
			else
				break;
		}
	}
	return -1;
}

void CXTPSyntaxEditLineMarksManager::CLineMarksList::Add(const XTP_EDIT_LMDATA& lmData)
{
	int nIndex = FindUpperIndex(lmData.m_nRow);
	if (nIndex < 0)
	{
		m_array.Add(new XTP_EDIT_LMDATA(lmData));
	}
	else
	{
		XTP_EDIT_LMDATA* pOldData = m_array.GetAt(nIndex);
		if (pOldData->m_nRow == lmData.m_nRow)
		{
			// just copy data because the key already exists
			*pOldData = lmData;
		}
		else
		{
			// insert new element for the new key
			m_array.InsertAt(nIndex, new XTP_EDIT_LMDATA(lmData));
		}
	}

}

void CXTPSyntaxEditLineMarksManager::CLineMarksList::Remove(const int nKey)
{
	int nIndex = FindIndex(nKey);
	if (nKey >= 0)
	{
		XTP_EDIT_LMDATA* pData = m_array.GetAt(nIndex);
		if (pData)
			delete pData;
		m_array.RemoveAt(nIndex);
	}
}

POSITION CXTPSyntaxEditLineMarksManager::CLineMarksList::FindAt(int nKey)
{
	// find a record with the specified key
	INT_PTR nIndex = FindIndex(nKey);
	return nIndex >= 0 ? (POSITION)(nIndex + 1) : NULL;
}

POSITION CXTPSyntaxEditLineMarksManager::CLineMarksList::FindNext(int nKey)
{
	INT_PTR nIndex = FindUpperIndex(nKey);
	if (nIndex >= 0)
	{
		return (POSITION)(nIndex + 1);
	}
	return NULL;
}

POSITION CXTPSyntaxEditLineMarksManager::CLineMarksList::FindPrev(int nKey)
{
	INT_PTR nIndex = FindLowerIndex(nKey);
	if (nIndex >= 0)
	{
		return (POSITION)(nIndex + 1);
	}
	return NULL;
}

void CXTPSyntaxEditLineMarksManager::CLineMarksList::RefreshLineMarks(
			int nRowFrom, int nRowTo, XTPSyntaxEditLineMarksRefreshType eRefreshType)
{
	int nDiff = abs(nRowTo - nRowFrom);

	if (eRefreshType == xtpEditLMRefresh_Delete)
	{
		nDiff *= -1;
	}

	int nRowU = max(nRowTo, nRowFrom);
	int nRowL = min(nRowTo, nRowFrom);

	const int nCount = (int)m_array.GetSize();
	CXTPSyntaxEditLineMarkPointersArray newArray;

	int nNewIndex = 0;
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		XTP_EDIT_LMDATA* pData = m_array.GetAt(nIndex);
		if (!pData)
			continue;

		if (pData->m_nRow < nRowL)
		{
			// does not affect refresh
			newArray.SetAtGrow(nNewIndex, pData);
		}
		else if (eRefreshType == xtpEditLMRefresh_Delete &&
				pData->m_nRow < nRowU)
		{
			// delete line mark
			--nNewIndex; // By Leva - if we leave NULL values in array the binary search works wrong way
			delete pData;
		}
		else
		{
			// refresh position
			pData->m_nRow += nDiff;
			newArray.SetAtGrow(nNewIndex, pData);
		}
		nNewIndex++;
	}
	// update data in the old array
	m_array.RemoveAll();
	m_array.Append(newArray);
}

void CXTPSyntaxEditLineMarksManager::CLineMarksList::RemoveAll()
{
	const int nCount = (int)m_array.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		XTP_EDIT_LMDATA* pData = m_array.GetAt(i);
		if (pData)
			delete pData;
	}

	m_array.RemoveAll();
}

int CXTPSyntaxEditLineMarksManager::CLineMarksList::GetCount()
{
	return (int)m_array.GetSize();
}

POSITION CXTPSyntaxEditLineMarksManager::CLineMarksList::GetFirstLineMark()
{
	return BEFORE_START_POSITION;
}

XTP_EDIT_LMDATA* CXTPSyntaxEditLineMarksManager::CLineMarksList::GetNextLineMark(POSITION& pos)
{
	// Rule: pos == element index + 1  (because 0=NULL and mean no more elements)

	// check for the beginning of the list
	if (pos == BEFORE_START_POSITION)
	{
		pos = (POSITION)1;
	}

	// get data corresponding to the position
	XTP_EDIT_LMDATA* pData = (INT_PTR)pos > (INT_PTR)m_array.GetSize() ? NULL : m_array.GetAt((INT_PTR)pos - 1);

	// increase position
	pos = (POSITION)((INT_PTR)pos + 1);

	// check for the end of the array
	if ((INT_PTR)pos > (INT_PTR)m_array.GetSize() )
	{
		//pos = (POSITION)m_array.GetSize();
		pos = NULL;
	}

	// return the data for the initial position
	return pData;
}

XTP_EDIT_LMDATA* CXTPSyntaxEditLineMarksManager::CLineMarksList::GetLineMarkAt(const POSITION pos)
{
	// Rule: pos == element index + 1  (because 0=NULL and mean no more elements)

	INT_PTR nIndex = (INT_PTR)pos - 1;
	if (nIndex < 0 || nIndex >= (INT_PTR)m_array.GetSize())
	{
		return NULL;
	}
	return m_array.GetAt(nIndex);
}
/////////////////////////////////////////////////////////////////////////////

CXTPSyntaxEditLineMarksManager::CLineMarksListPtr
CXTPSyntaxEditLineMarksManager::CLineMarksListsMap::GetList(LPCTSTR szMarkType)
{
	CLineMarksListPtr ptrList;
	if (!m_map.Lookup(szMarkType, ptrList))
	{
		return NULL;
	}
	return ptrList;
}

CXTPSyntaxEditLineMarksManager::CLineMarksListPtr
CXTPSyntaxEditLineMarksManager::CLineMarksListsMap::AddList(LPCTSTR szMarkType)
{
	CLineMarksListPtr ptrList(new CLineMarksList());
	m_map.SetAt(szMarkType, ptrList);
	return ptrList;
}

void CXTPSyntaxEditLineMarksManager::CLineMarksListsMap::RefreshLineMarks(
			int nRowFrom, int nRowTo, XTPSyntaxEditLineMarksRefreshType eRefreshType)
{
	POSITION pos = m_map.GetStartPosition();
	CString strKey;
	CLineMarksListPtr ptrList;
	while (pos != NULL)
	{
		m_map.GetNextAssoc(pos, strKey, ptrList);

		if (ptrList)
			ptrList->RefreshLineMarks(nRowFrom, nRowTo, eRefreshType);
	}
}
/////////////////////////////////////////////////////////////////////////////
CXTPSyntaxEditLineMarksManager::CXTPSyntaxEditLineMarksManager()
{
}

CXTPSyntaxEditLineMarksManager::~CXTPSyntaxEditLineMarksManager()
{
}

void CXTPSyntaxEditLineMarksManager::AddRemoveLineMark(int nRow, const XTP_EDIT_LINEMARKTYPE lmType,
													  XTP_EDIT_LMPARAM* pParam)
{
	if (HasRowMark(nRow, lmType))
	{
		DeleteLineMark(nRow, lmType);
	}
	else
	{
		SetLineMark(nRow, lmType, pParam);
	}
}

void CXTPSyntaxEditLineMarksManager::SetLineMark(int nRow, const XTP_EDIT_LINEMARKTYPE lmType,
														  XTP_EDIT_LMPARAM* pParam)
{
	// prepare data structure
	XTP_EDIT_LMDATA lmData;
	lmData.m_nRow = nRow;
	if (pParam)
	{
		lmData.m_Param = *pParam;
	}

	// get corresponding list
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	if (!ptrList)
	{
		// create new list
		ptrList = m_mapLists.AddList(lmType);
		if (!ptrList)
			return; // error happened
	}

	// set value
	ptrList->Add(lmData);
}

void CXTPSyntaxEditLineMarksManager::DeleteLineMark(int nRow, const XTP_EDIT_LINEMARKTYPE lmType)
{
	// get corresponding list
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	if (ptrList)
	{
		// delete line mark
		ptrList->Remove(nRow);

	}
}

BOOL CXTPSyntaxEditLineMarksManager::HasRowMark(int nRow, const XTP_EDIT_LINEMARKTYPE lmType,
												XTP_EDIT_LMPARAM* pParam)
{
	// get corresponding list
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	if (ptrList)
	{
		// Find line data
		POSITION pos = ptrList->FindAt(nRow);
		XTP_EDIT_LMDATA* pData = ptrList->GetLineMarkAt(pos);
		if (pData)
		{
			ASSERT(pData->m_nRow == nRow);
			if (pParam)
			{
				*pParam = pData->m_Param;
			}
			return TRUE;
		}
	}

	return FALSE;
}

POSITION CXTPSyntaxEditLineMarksManager::GetLastLineMark(const XTP_EDIT_LINEMARKTYPE lmType)
{
	int nRow = INT_MAX - 1;
	return FindPrevLineMark(nRow, lmType);
}

POSITION CXTPSyntaxEditLineMarksManager::FindPrevLineMark(int& nRow, const XTP_EDIT_LINEMARKTYPE lmType)
{
	int nPrevRow = -1;
	POSITION posPrev = NULL;

	// get corresponding list
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	if (ptrList)
	{
		// Find line data
		posPrev = ptrList->FindPrev(nRow);
		XTP_EDIT_LMDATA* pData = ptrList->GetLineMarkAt(posPrev);
		if (pData)
		{
			nPrevRow = pData->m_nRow;
		}
	}

	nRow = nPrevRow;
	return posPrev;
}

POSITION CXTPSyntaxEditLineMarksManager::FindNextLineMark(int& nRow, const XTP_EDIT_LINEMARKTYPE lmType)
{
	int nNextRow = INT_MAX;
	POSITION posNext = NULL;

	// get corresponding list
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	if (ptrList)
	{
		// Find line data
		posNext = ptrList->FindNext(nRow);
		XTP_EDIT_LMDATA* pData = ptrList->GetLineMarkAt(posNext);
		if (pData)
		{
			nNextRow = pData->m_nRow;
		}
	}

	if (nNextRow == INT_MAX)
	{
		nNextRow = -1;
	}

	nRow = nNextRow;
	return posNext;
}

POSITION CXTPSyntaxEditLineMarksManager::GetFirstLineMark(const XTP_EDIT_LINEMARKTYPE lmType)
{
	// get corresponding list
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	return ptrList ? ptrList->GetFirstLineMark() : NULL;
}

XTP_EDIT_LMDATA* CXTPSyntaxEditLineMarksManager::GetNextLineMark(POSITION& pos, const XTP_EDIT_LINEMARKTYPE lmType)
{
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	return ptrList ? ptrList->GetNextLineMark(pos) : NULL;
}

void CXTPSyntaxEditLineMarksManager::RefreshLineMarks(int nRowFrom, int nRowTo,
									XTPSyntaxEditLineMarksRefreshType eRefreshType)
{
	m_mapLists.RefreshLineMarks(nRowFrom, nRowTo, eRefreshType);
}

void CXTPSyntaxEditLineMarksManager::RemoveAll(const XTP_EDIT_LINEMARKTYPE lmType)
{
	// get corresponding list
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	if (ptrList)
		ptrList->RemoveAll();
}

XTP_EDIT_LMDATA* CXTPSyntaxEditLineMarksManager::GetLineMarkAt(const POSITION pos, const XTP_EDIT_LINEMARKTYPE lmType)
{
	// get corresponding list
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	return ptrList ? ptrList->GetLineMarkAt(pos) : NULL;
}

int CXTPSyntaxEditLineMarksManager::GetCount(const XTP_EDIT_LINEMARKTYPE lmType)
{
	// get corresponding list
	CLineMarksListPtr ptrList = m_mapLists.GetList(lmType);
	return ptrList ? ptrList->GetCount() : 0;
}
