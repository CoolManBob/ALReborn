// XTPCalendarEventLabel.cpp: implementation of the CXTPCalendarEventLabel and
// CXTPCalendarEventLabels classes.
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
#include "resource.h"

#include "XTPCalendarEventLabel.h"
#include "XTPCalendarUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXTPCalendarEventLabel, CCmdTarget)
IMPLEMENT_DYNAMIC(CXTPCalendarEventLabels, CCmdTarget)
////////////////////////////////////////////////////////////////////////////
CXTPCalendarEventLabel::CXTPCalendarEventLabel(int nID, COLORREF clrColor, LPCTSTR pcszName)
{
	m_nLabelID = nID;
	m_clrColor = clrColor;
	m_strName = pcszName;

}

////////////////////////////////////////////////////////////////////////////
CXTPCalendarEventLabels::CXTPCalendarEventLabels()
{

}

CXTPCalendarEventLabels::~CXTPCalendarEventLabels()
{
	RemoveAll();
}

CXTPCalendarEventLabel* CXTPCalendarEventLabels::Find(int nLabelID) const
{
	int nFIdx = FindIndex(nLabelID);
	if (nFIdx >= 0)
	{
		return GetAt(nFIdx);
	}
	return NULL;
}

int CXTPCalendarEventLabels::FindIndex(int nLabelID) const
{
	int nCount = GetCount();

	if (nLabelID >= 0 && nLabelID < nCount)
	{
		CXTPCalendarEventLabel* pLabel = GetAt(nLabelID);

		if (pLabel && pLabel->m_nLabelID == nLabelID)
		{
			return nLabelID;
		}
	}

	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEventLabel* pLabel = GetAt(i);

		if (pLabel && pLabel->m_nLabelID == nLabelID)
		{
			return i;
		}
	}

	return -1;
}

void CXTPCalendarEventLabels::InitDefaultValues()
{
	RemoveAll();

	//XTP_CALENDAR_NONE_LABEL_ID = 0
	CXTPCalendarEventLabel* pLabel;
	pLabel = new CXTPCalendarEventLabel(0, RGB(255, 255, 255), _T("None"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_NONE);
	InsertAt(0, pLabel);

	pLabel = new CXTPCalendarEventLabel(1, RGB(255, 148, 132), _T("Important"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_IMPORTANT);
	InsertAt(1, pLabel);

	pLabel = new CXTPCalendarEventLabel(2, RGB(132, 156, 231), _T("Business"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_BUSINESS);
	InsertAt(2, pLabel);

	pLabel = new CXTPCalendarEventLabel(3, RGB(165, 222, 99), _T("Personal"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_PERSONAL);
	InsertAt(3, pLabel);

	pLabel = new CXTPCalendarEventLabel(4, RGB(231, 231, 214), _T("Vacation"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_VACATION);
	InsertAt(4, pLabel);

	pLabel = new CXTPCalendarEventLabel(5, RGB(255, 181, 115), _T("Must Attend"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_MUSTATTEND);
	InsertAt(5, pLabel);

	pLabel = new CXTPCalendarEventLabel(6, RGB(132, 239, 247), _T("Travel Required"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_TRAVELREQUIRED);
	InsertAt(6, pLabel);

	pLabel = new CXTPCalendarEventLabel(7, RGB(214, 206, 132), _T("Needs Preparation"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_NEEDSPREPARATION);
	InsertAt(7, pLabel);

	pLabel = new CXTPCalendarEventLabel(8, RGB(198, 165, 247), _T("Birthday"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_BIRTHDAY);
	InsertAt(8, pLabel);

	pLabel = new CXTPCalendarEventLabel(9, RGB(165, 206, 198), _T("Anniversary"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_ANNIVERSARY);
	InsertAt(9, pLabel);

	pLabel = new CXTPCalendarEventLabel(10, RGB(255, 231, 115), _T("Phone Call"));
	CXTPCalendarUtils::LoadString2(pLabel->m_strName, XTP_IDS_CALENDAR_EVENT_LABEL_PHONECALL);
	InsertAt(10, pLabel);
}

/////////////////////////////////////////////////////////////////////////////
