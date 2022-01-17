// XTSortClass.cpp : implementation of the CXTSortClass class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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

#include "stdafx.h"
#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTSortClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

struct CXTSortClass::SORT_ITEM
{
	SORT_ITEM(const DWORD_PTR dwData, const CString &strItemText)
	{
		m_dwData = dwData;
		m_strItemText = strItemText;
	}
	DWORD_PTR   m_dwData;
	CString m_strItemText;
};

//////////////////////////////////////////////////////////////////////
// CXTSortClass
//
CXTSortClass::CXTSortClass(CListCtrl* pListCtrl, const int nCol)
{
	ASSERT(pListCtrl);
	m_pListCtrl = pListCtrl;
	int nCount = m_pListCtrl->GetItemCount();

	// Replace Item data with pointer to XT_SORT_ITEM structure
	int i;
	for (i = 0; i < nCount; i++)
	{
		DWORD_PTR dwData = m_pListCtrl->GetItemData(i); // save current data to restore it later
		CString strItemText = m_pListCtrl->GetItemText(i, nCol);
		m_pListCtrl->SetItemData(i, (DWORD_PTR)new SORT_ITEM(dwData, strItemText));
	}
}

CXTSortClass::~CXTSortClass()
{
	ASSERT(m_pListCtrl);
	int nCount = m_pListCtrl->GetItemCount();

	int i;
	for (i = 0; i < nCount; i++)
	{
		SORT_ITEM* pItem = (SORT_ITEM*)m_pListCtrl->GetItemData(i);
		ASSERT(pItem);
		m_pListCtrl->SetItemData(i, pItem->m_dwData);
		delete pItem;
	}
}

void CXTSortClass::Sort(bool bAsc, XTSortType eType)
{
	long lParamSort = eType;

	// if lParamSort positive - ascending sort order, negative - descending
	if (!bAsc)
	{
		lParamSort *= -1;
	}

	m_pListCtrl->SortItems(Compare, lParamSort);
}

int CALLBACK CXTSortClass::Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	SORT_ITEM* item1 = (SORT_ITEM*)lParam1;
	SORT_ITEM* item2 = (SORT_ITEM*)lParam2;

	ASSERT(item1 && item2);

	// Restore data type and sort order from lParamSort
	// if lParamSort positive - ascending sort order, negative - descending
	short sOrder = lParamSort < 0 ? (short) -1 : (short) 1;

	XTSortType eType = (XTSortType) (lParamSort * sOrder); // get rid of sign

	// declare typed buffers
	COleDateTime t1, t2;

	switch (eType)
	{
	case xtSortInt:
		{
			CString cs1 = item1->m_strItemText;
			CString cs2 = item2->m_strItemText;
			RemoveNonNumeric(cs1);
			RemoveNonNumeric(cs2);
			return (_ttol(cs1) - _ttol(cs2)) * sOrder;
		}

	case xtSortDecimal:
#ifdef _UNICODE
		{
			char szText1[ 256 ];
			char szText2[ 256 ];

			::WideCharToMultiByte(CP_ACP, 0, item1->m_strItemText, -1, szText1,
				255, NULL, NULL);

			::WideCharToMultiByte(CP_ACP, 0, item2->m_strItemText, -1, szText2,
				255, NULL, NULL);

			return (atof(szText1) < atof(szText2) ? -1 : 1) * sOrder;
		}
#else
		return (atof (item1->m_strItemText) < atof(item2->m_strItemText) ? -1 : 1) * sOrder;
#endif

	case xtSortDateTime:
		if (t1.ParseDateTime(item1->m_strItemText) && t2.ParseDateTime(item2->m_strItemText))
		{
			return (t1 < t2 ? -1 : 1) * sOrder;
		}
		return 0;

	case xtSortString:
		return item1->m_strItemText.CompareNoCase(item2->m_strItemText) * sOrder;

	default:
		ASSERT("Error: attempt to sort a column without type.");
		return 0;
	}
}

int CXTSortClass::RemoveNonNumeric(CString &csSource)
{
	TCHAR ch;
	CString temp;
	int nLen = csSource.GetLength();
	int i = 0;

	while (i < nLen)
	{
		ch = csSource.GetAt(i);
		if (_istdigit(ch))
			temp += ch;
		++i;
	}
	csSource = temp;
	return csSource.GetLength();
}
