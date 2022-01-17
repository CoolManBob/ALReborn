// XTPDatePickerDaysCollection.cpp: implementation of the CXTPDatePickerDaysCollection class.
//
// This file is a part of the XTREME CALENDAR MFC class library.
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
#include "XTPDatePickerDaysCollection.h"
#include "XTPDatePickerItemDay.h"
#include "XTPDatePickerItemMonth.h"
#include "XTPDatePickerControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPDatePickerDaysCollection

CXTPDatePickerDaysCollection::CXTPDatePickerDaysCollection(CXTPDatePickerControl* pControl)
	: m_pControl(pControl)
{
}

CXTPDatePickerDaysCollection::~CXTPDatePickerDaysCollection()
{
}

void CXTPDatePickerDaysCollection::_InsertBlock(int nIndexInsert, long nDateBegin, long nDateEnd)
{
	SELECTED_BLOCK block;
	block.nDateBegin = nDateBegin;
	block.nDateEnd = nDateEnd;
	m_arrSelectedBlocks.InsertAt(nIndexInsert, block);
}

void CXTPDatePickerDaysCollection::AddBlock(long nBegin, long nEnd)
{
	int i = 0;
	int nCount = (int)m_arrSelectedBlocks.GetSize();

	if (nCount > 0)
	{
		long& nDateEnd = m_arrSelectedBlocks[nCount - 1].nDateEnd;

		if (nDateEnd == nBegin)
		{
			nDateEnd = nEnd + 1;
			m_pControl->_RedrawControl(FALSE);
			return;
		}

		if (nDateEnd < nBegin)
		{
			i = nCount;
		}
	}

	for (; i < nCount; i++)
	{
		long& nDateBegin = m_arrSelectedBlocks[i].nDateBegin;
		long& nDateEnd = m_arrSelectedBlocks[i].nDateEnd;

		if ((nDateBegin <= nBegin) && (nDateEnd > nEnd))
		{
			return;
		}

		if (nDateEnd == nBegin)
		{
			nDateEnd = nEnd + 1;
			if (i + 1 < nCount && m_arrSelectedBlocks[i + 1].nDateBegin == nDateEnd)
			{
				nDateEnd = m_arrSelectedBlocks[i + 1].nDateEnd;
				m_arrSelectedBlocks.RemoveAt(i + 1);
			}

			m_pControl->_RedrawControl(FALSE);
			return;
		}
		if (nDateBegin == nEnd + 1)
		{
			nDateBegin = nBegin;
			m_pControl->_RedrawControl(FALSE);
			return;
		}

		if (nDateBegin > nEnd)
			break;
	}

	_InsertBlock(i, nBegin, nEnd + 1);
	m_pControl->_RedrawControl(FALSE);
}

void CXTPDatePickerDaysCollection::Clear()
{
	m_arrSelectedBlocks.RemoveAll();
}

void CXTPDatePickerDaysCollection::Add(const COleDateTime& dtDay)
{
	AddBlock((long)dtDay, (long)dtDay);
}

void CXTPDatePickerDaysCollection::Remove(const COleDateTime& dtDay)
{
	long nDate = (long)dtDay;

	for (int i = 0; i < (int)m_arrSelectedBlocks.GetSize(); i++)
	{
		long nDateBegin = m_arrSelectedBlocks[i].nDateBegin;
		long nDateEnd = m_arrSelectedBlocks[i].nDateEnd;

		if ((nDateBegin <= nDate) && (nDateEnd > nDate))
		{
			if (nDateBegin != nDate)
			{
				_InsertBlock(i, nDateBegin, nDate);
				i++;
			}

			if (nDateEnd - 1 != nDate)
			{
				_InsertBlock(i + 1, nDate + 1, nDateEnd);
			}

			m_arrSelectedBlocks.RemoveAt(i);
			return;
		}
	}
}


void CXTPDatePickerDaysCollection::Select(const COleDateTime& dtDay)
{
	long nDate = (long)dtDay;

	if (m_arrSelectedBlocks.GetSize() == 1 && m_arrSelectedBlocks[0].nDateBegin ==
		m_arrSelectedBlocks[0].nDateEnd - 1 && m_arrSelectedBlocks[0].nDateBegin == nDate)
	{

	}
	else
	{
		Clear();
		Add(dtDay);
	}
}

void CXTPDatePickerDaysCollection::SelectRange(const COleDateTime& dtDayBegin, const COleDateTime& dtDayEnd)
{
	long nBeginDate = (long)dtDayBegin;
	long nEndDate = (long)dtDayEnd;

	if (nBeginDate > nEndDate)
	{
		long nBlockBegin = nBeginDate;
		nBeginDate = nEndDate;
		nEndDate = nBlockBegin;
	}

	if (m_arrSelectedBlocks.GetSize() == 1 && m_arrSelectedBlocks[0].nDateBegin == nBeginDate &&
		m_arrSelectedBlocks[0].nDateEnd == nEndDate + 1)
	{
		return;
	}

	Clear();

	_InsertBlock(0, nBeginDate, nEndDate + 1);
}

BOOL CXTPDatePickerDaysCollection::Contains(const COleDateTime& dtDay) const
{
	long nDay = (long)dtDay;

	int nCount = (int)m_arrSelectedBlocks.GetSize();
	if (nCount == 0)
		return FALSE;

	if (m_arrSelectedBlocks[0].nDateBegin > nDay)
		return FALSE;

	if (m_arrSelectedBlocks[nCount - 1].nDateEnd <= nDay)
		return FALSE;

	for (int i = 0; i < nCount; i++)
	{
		if ((m_arrSelectedBlocks[i].nDateBegin <= nDay) &&
			(m_arrSelectedBlocks[i].nDateEnd > nDay))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CXTPDatePickerDaysCollection::GetMinMaxRange(COleDateTime& refMinRange, COleDateTime& refMaxRange) const
{
	int nSelCount = (int)m_arrSelectedBlocks.GetSize();
	if (nSelCount == 0)
		return FALSE;

	refMinRange = (DATE)(double)m_arrSelectedBlocks[0].nDateBegin;
	refMaxRange = (DATE)(double)m_arrSelectedBlocks[nSelCount - 1].nDateEnd - 1;

	return TRUE;
}

int CXTPDatePickerDaysCollection::GetSelectedDaysCount()
{
	int nCount = 0;
	for (int i = 0; i < (int)m_arrSelectedBlocks.GetSize(); i++)
	{
		nCount += m_arrSelectedBlocks[i].nDateEnd - m_arrSelectedBlocks[i].nDateBegin;
	}
	return nCount;
}

