// XTPReportHyperlink.cpp : implementation of the CXTPReportHyperlink class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
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
#include "XTPReportHyperlink.h"
#include "Common/XTPPropExchange.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CXTPReportHyperlink, CCmdTarget)
IMPLEMENT_DYNAMIC(CXTPReportHyperlinks, CCmdTarget)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPReportHyperlink::CXTPReportHyperlink(int nHyperTextBegin, int nHyperTextLen)
	: m_nHyperTextBegin(nHyperTextBegin),
	m_nHyperTextLen(nHyperTextLen),
	m_rcHyperSpot(0, 0, 0, 0)
{

}

CXTPReportHyperlink::~CXTPReportHyperlink()
{

}

void CXTPReportHyperlink::DoPropExchange(CXTPPropExchange* pPX)
{
	PX_Int(pPX, _T("HyperTextBegin"), m_nHyperTextBegin);
	PX_Int(pPX, _T("HyperTextLen"), m_nHyperTextLen);
}

CXTPReportHyperlinks::CXTPReportHyperlinks()
{

}

CXTPReportHyperlinks::~CXTPReportHyperlinks()
{
	RemoveAll();
}

void CXTPReportHyperlinks::RemoveAt(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)GetSize())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPReportHyperlink* pItem = GetAt(nIndex);
	if (pItem)
		pItem->InternalRelease();

	TBase::RemoveAt(nIndex);
}

void CXTPReportHyperlinks::RemoveAll()
{
	for (int nItem = (int)GetSize() - 1; nItem >= 0; nItem--)
	{
		CXTPReportHyperlink* pItem = GetAt(nItem);
		if (pItem)
			pItem->InternalRelease();
	}
	TBase::RemoveAll();
}

void CXTPReportHyperlinks::DoPropExchange(CXTPPropExchange* pPX)
{
	int nCount = (int)GetSize();

	CXTPPropExchangeEnumeratorPtr pEnumItems(pPX->GetEnumerator(_T("Hyperlink")));

	if (pPX->IsStoring())
	{
		POSITION posItem = pEnumItems->GetPosition((DWORD)nCount);

		for (int i = 0; i < nCount; i++)
		{
			CXTPReportHyperlink* pItem = GetAt(i);
			ASSERT(pItem);
			if (pItem)
			{
				CXTPPropExchangeSection secItem(pEnumItems->GetNext(posItem));
				pItem->DoPropExchange(&secItem);
			}
		}
	}
	else
	{
		POSITION posItem = pEnumItems->GetPosition();
		while (posItem)
		{
			CXTPReportHyperlink* pItem = new CXTPReportHyperlink();
			if (!pItem)
			{
				return;
			}
			CXTPPropExchangeSection secItem(pEnumItems->GetNext(posItem));
			pItem->DoPropExchange(&secItem);

			Add(pItem);
		}
	}

}

void CXTPReportHyperlinks::CopyFrom(CXTPReportHyperlinks* pSrc)
{
	if (pSrc == this)
		return;

	RemoveAll();

	if (!pSrc)
		return;

	int nCount = pSrc->GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CXTPReportHyperlink* pHlnk = pSrc->GetAt(i);
		if (pHlnk)
		{
			pHlnk->InternalAddRef();
			Add(pHlnk);
		}
	}
}

