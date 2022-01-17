// XTPReportRecords.cpp : implementation of the CXTPReportRecords class.
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
#include "Resource.h"

#include "Common/XTPPropExchange.h"

#include "XTPReportRecordItem.h"
#include "XTPReportRecord.h"
#include "XTPReportRecords.h"
#include "XTPReportInplaceControls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXTPReportRecords, CCmdTarget)

CXTPReportRecords::CXTPReportRecords(BOOL bArray /*= FALSE*/)
{
	m_pOwnerRecord = NULL;

	m_pVirtualRecord = NULL;
	m_nVirtualRecordsCount = 0;
	m_bArray = bArray;

	m_bCaseSensitive = TRUE;


}

CXTPReportRecords::CXTPReportRecords(CXTPReportRecord* pOwnerRecord)
{
	m_pOwnerRecord = pOwnerRecord;

	m_pVirtualRecord = NULL;
	m_nVirtualRecordsCount = 0;
	m_bArray = FALSE;

	m_bCaseSensitive = TRUE;

}

void CXTPReportRecords::SetVirtualMode(CXTPReportRecord* pVirtualRecord, int nCount)
{
	// release old virtual record
	if (m_pVirtualRecord)
	{
		m_pVirtualRecord->InternalRelease();
	}

	// reset virtual mode
	if (!pVirtualRecord || nCount <= 0)
	{
		if (pVirtualRecord)
		{
			pVirtualRecord->InternalRelease();
		}
		m_pVirtualRecord = NULL;
		m_nVirtualRecordsCount = 0;
	}
	else // set new virtual record
	{
		m_pVirtualRecord = pVirtualRecord;
		m_nVirtualRecordsCount = nCount;
		if (m_pVirtualRecord)
		{
			m_pVirtualRecord->m_pRecords = this;
		}
	}
}

CXTPReportRecords::~CXTPReportRecords()
{
	RemoveAll();

	if (m_pVirtualRecord)
	{
		m_pVirtualRecord->InternalRelease();
	}
}

void CXTPReportRecords::RemoveAll()
{
	if (!m_bArray)
	{
		// array cleanup
		for (int nRecord = (int)m_arrRecords.GetSize() - 1; nRecord >= 0; nRecord--)
		{
			CXTPReportRecord* pRecord = m_arrRecords.GetAt(nRecord);
			pRecord->InternalRelease();
		}
	}
	m_arrRecords.RemoveAll();
}

void CXTPReportRecords::UpdateIndexes(int nStart /*= 0*/)
{
	for (int i = nStart; i < GetCount(); i++)
		GetAt(i)->m_nIndex = i;
}

CXTPReportRecord* CXTPReportRecords::Add(CXTPReportRecord* pRecord)
{
	int nIndex = (int)m_arrRecords.Add(pRecord);

	if (m_bArray)
	{
		ASSERT(pRecord->m_pRecords);
	}
	else
	{
		pRecord->m_nIndex = nIndex;
		pRecord->m_pRecords = this;
	}

	return pRecord;
}

void CXTPReportRecords::RemoveAt(int nIndex)
{
	ASSERT(!m_bArray);

	if (nIndex < (int)m_arrRecords.GetSize())
	{
		m_arrRecords[nIndex]->InternalRelease();
		m_arrRecords.RemoveAt(nIndex);

		UpdateIndexes(nIndex);
	}
}

int CXTPReportRecords::RemoveRecord(CXTPReportRecord* pRecord)
{
	ASSERT(!m_bArray);

	for (int i = 0; i < (int)m_arrRecords.GetSize(); i++)
	{
		CXTPReportRecord* pRec = m_arrRecords.GetAt(i);
		if (pRec == pRecord)
		{
			pRecord->InternalRelease();
			m_arrRecords.RemoveAt(i);

			UpdateIndexes(i);

			return i;
		}
	}

	return - 1;
}

void CXTPReportRecords::InsertAt(int nIndex, CXTPReportRecord* pRecord)
{
	ASSERT(!m_bArray);

	m_arrRecords.InsertAt(nIndex, pRecord);
	pRecord->m_pRecords = this;
	UpdateIndexes(nIndex);
}

int CXTPReportRecords::GetCount() const
{
	if (m_pVirtualRecord != NULL)
		return m_nVirtualRecordsCount;

	return (int)m_arrRecords.GetSize();
}
CXTPReportRecord* CXTPReportRecords::GetAt(int nIndex) const
{
	if (m_pVirtualRecord)
	{
		m_pVirtualRecord->m_nIndex = nIndex;
		return m_pVirtualRecord;
	}

	return nIndex >= 0 && nIndex < GetCount() ? m_arrRecords.GetAt(nIndex) : NULL;
}

void CXTPReportRecords::DoPropExchange(CXTPPropExchange* pPX)
{
	pPX->ExchangeSchemaSafe();

	_DoPropExchange(pPX);
}

void CXTPReportRecords::_DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPPropExchangeEnumeratorPtr pEnumRecords(pPX->GetEnumerator(_T("Record")));

	if (pPX->IsStoring())
	{
		int nCount = (int)GetCount();
		POSITION pos = pEnumRecords->GetPosition((DWORD)nCount);

		for (int i = 0; i < nCount; i++)
		{
			CXTPReportRecord* pRecord = GetAt(i);
			ASSERT(pRecord);

			CXTPPropExchangeSection sec(pEnumRecords->GetNext(pos));
			PX_Object(&sec, pRecord, RUNTIME_CLASS(CXTPReportRecord));
		}
	}
	else
	{
		RemoveAll();

		POSITION pos = pEnumRecords->GetPosition();

		while (pos)
		{
			CXTPReportRecord* pRecord = NULL;

			CXTPPropExchangeSection sec(pEnumRecords->GetNext(pos));
			PX_Object(&sec, pRecord, RUNTIME_CLASS(CXTPReportRecord));

			if (!pRecord)
				AfxThrowArchiveException(CArchiveException::badClass);

			Add(pRecord);
		}
	}
}

void CXTPReportRecords::Move(int nIndex, CXTPReportRecords* pRecords)
{
	ASSERT(pRecords->m_bArray == TRUE);

	if (nIndex > GetCount())
		nIndex = GetCount();

	int nRecordsCount = (int)pRecords->GetCount(), i;

	for (i = 0; i < nRecordsCount; i++)
	{
		CXTPReportRecord* pRecord = pRecords->GetAt(i);
		int nRecordIndex = pRecord->GetIndex();

		if (pRecord->m_pRecords != this)
			continue;

		ASSERT(pRecord && GetAt(nRecordIndex) == pRecord);

		m_arrRecords.RemoveAt(nRecordIndex);

		if (nRecordIndex < nIndex)
		{
			nIndex--;
		}

		for (int j = i + 1; j < nRecordsCount; j++)
		{
			pRecord = pRecords->GetAt(j);
			if (pRecord->m_pRecords != this)
				continue;

			if (pRecord->GetIndex() > nRecordIndex)
			{
				pRecord->m_nIndex--;
			}
		}
	}

	for (i = 0; i < nRecordsCount; i++)
	{
		CXTPReportRecord* pRecord = pRecords->GetAt(i);
		if (pRecord->m_pRecords != this)
			continue;

		m_arrRecords.InsertAt(nIndex, pRecord);
		nIndex++;
	}

	UpdateIndexes();
}

int CXTPReportRecords::Compare(const CString& str1, const CString& str2) const
{
	if (!IsCaseSensitive())
		return str1.CompareNoCase(str2);

	return str1.Compare(str2);
}


