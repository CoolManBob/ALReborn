// XTPCalendarMemoryDataProvider.cpp: implementation of the CXTPCalendarMemoryDataProvider class.
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
#include "XTPCalendarMemoryDataProvider.h"
#include "XTPCalendarEvents.h"
#include "XTPCalendarEvent.h"
#include "XTPCalendarRecurrencePattern.h"
#include "XTPCalendarNotifications.h"
#include "XTPCalendarControl.h"
#include "XTPCalendarResource.h"
#include "Common/XTPPropExchange.h"

#define _XML_FILE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTP_MEM_DP_DATA_VER 1

//===========================================================================
//    Should be a prime number:
// 37, 53, 67, 79, 101, 127, 199, 503, 1021, 1511, 2003, 3001
//===========================================================================
#define XTP_MEMDP_TMP_HASH_TABLE_SIZE 67

//{{AFX_CODEJOCK_PRIVATE
class CXTPCalendarEventHelper : public CXTPCalendarEvent
{
public:
	virtual CXTPCalendarRecurrencePattern* GetRPatternRef()
	{
		return CXTPCalendarEvent::GetRPatternRef();
	}
};
//}}AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarMemoryDataProvider implementation
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CXTPCalendarMemoryDataProvider, CXTPCalendarData)

//////////////////////////////////////////////////////////////////////
CXTPCalendarMemoryDataProvider::CXTPCalendarMemoryDataProvider() :
		m_EventsStorage(XTP_EVENTS_STORAGE_HASH_TABLE_SIZE)
{
	m_typeProvider = xtpCalendarDataProviderMemory;
}

CXTPCalendarMemoryDataProvider::~CXTPCalendarMemoryDataProvider()
{
}

CString CXTPCalendarMemoryDataProvider::GetXMLEncoding()
{
	CString strConnStr_lower = m_strConnectionString;
	strConnStr_lower.MakeLower();

	int nIndex = strConnStr_lower.Find(_T("encoding="));
	if (nIndex == -1)
		return _T("UTF-8");

	CString strEncoding = m_strConnectionString.Mid(nIndex + 9);

	nIndex = strEncoding.Find(_T(';'));
	if (nIndex != -1)
		strEncoding = strEncoding.Left(nIndex);

	return strEncoding;
}

BOOL CXTPCalendarMemoryDataProvider::IsProviderXML(BOOL bCheckXMLsupport, BOOL* pbXMLsupportError)
{
	if (pbXMLsupportError)
	{
		*pbXMLsupportError = FALSE;
	}

	CString strConnStr_lower = m_strConnectionString;
	strConnStr_lower.MakeLower();

	if (strConnStr_lower.IsEmpty())
		return FALSE;

	if (strConnStr_lower.Find(_T("provider=xml")) == -1)
		return FALSE;

	if (bCheckXMLsupport)
	{
		CXTPDOMDocumentPtr xmlDocPtr;
		BOOL bCreated = SUCCEEDED(xmlDocPtr.CreateInstance(CLSID_XTPDOMDocument));

		if (pbXMLsupportError)
		{
			*pbXMLsupportError = !bCreated;
		}
		return bCreated;

	}

	return TRUE;
}


BOOL CXTPCalendarMemoryDataProvider::Open()
{
	CString strFileName = GetDataSource();

	const int cnErrSize = 4096;
	TCHAR szError[cnErrSize+1];
	ZeroMemory(szError, sizeof(szError));

	CXTPCalendarData::Open();
	RemoveAllEvents();

	if (strFileName.IsEmpty())
	{
		return TRUE;
	}
	BOOL bResult = TRUE;

	try
	{
		BOOL bXMLsupportError = FALSE;
		if (!IsProviderXML(TRUE, &bXMLsupportError))
		{
			if (bXMLsupportError)
			{
				TRACE(_T("Warning: CXTPCalendarMemoryDataProvider::Open() cannot create XML parcer instance.\n"));
				TRACE(_T("\tperhaps AfxOleInit() has not been called, or msxml3.dll not found. \n"));
				TRACE(_T("\tThe binary data format is used. \n."));
			}

			CFile fileData(strFileName, CFile::modeRead);

			if (fileData.GetLength() >= 8)
			{
				CArchive arData(&fileData, CArchive::load);
				CXTPPropExchangeArchive px(arData);
				bResult = _Load(&px);
			}
		}
		else
		{
			bResult = FALSE;
#ifndef _XTP_EXCLUDE_XML
			CXTPPropExchangeXMLNode px(TRUE, 0, _T("Calendar"));
			px.SetEncoding(GetXMLEncoding());
			if (px.LoadFromFile(strFileName))
			{
				bResult = _Load(&px);
			}
#else
			//bResult = FALSE;
#endif
		}
	}
	catch(CFileException* pEfile)
	{
		if (pEfile->GetErrorMessage(szError, cnErrSize))
		{
			TRACE(_T("WARNING! MemoryDataProvider: Load Events data file error: %s \n"), szError);
		}
		pEfile->Delete();
		bResult = FALSE;
	}
	catch(CArchiveException* pEarc)
	{
		if (pEarc->GetErrorMessage(szError, cnErrSize))
		{
			TRACE(_T("WARNING! MemoryDataProvider: Load Events data archive error: %s \n"), szError);
		}
		//pEarc->ReportError();
		pEarc->Delete();
		bResult = FALSE;
	}
	catch(_com_error& e)
	{
		const TCHAR* pErrStr = e.ErrorMessage();

		if (pErrStr)
		{
			TRACE(_T("WARNING! MemoryDataProvider: Store Events data COM error: %s \n"), pErrStr);
		}
		bResult = FALSE;
	}

	if (!bResult)
	{
		CXTPCalendarData::Close();
	}
	return bResult;
}

BOOL CXTPCalendarMemoryDataProvider::Create()
{
	CXTPCalendarData::Create();

	return Save();
}

BOOL CXTPCalendarMemoryDataProvider::Save()
{
	CString strFileName = GetDataSource();

	if (strFileName.IsEmpty())
	{
		return TRUE;
	}

	const int cnErrSize = 4096;
	TCHAR szError[cnErrSize+1];
	ZeroMemory(szError, sizeof(szError));

	try
	{
		BOOL bXMLsupportError = FALSE;
		if (!IsProviderXML(TRUE, &bXMLsupportError))
		{
			if (bXMLsupportError)
			{
				TRACE(_T("Warning: CXTPCalendarMemoryDataProvider::Save() cannot create XML parcer instance.\n"));
				TRACE(_T("\tperhaps AfxOleInit() has not been called, or msxml3.dll not found. \n"));
				TRACE(_T("\tThe binary data format is used. \n."));
			}

			CFile fileData(strFileName, CFile::modeWrite | CFile::modeCreate);
			CArchive arData(&fileData, CArchive::store);

			CXTPPropExchangeArchive px(arData);
			_Save(&px);
		}
		else
		{
#ifndef _XTP_EXCLUDE_XML
			CXTPPropExchangeXMLNode px(FALSE, 0, _T("Calendar"));
			px.SetEncoding(GetXMLEncoding());
			_Save(&px);
			px.SaveToFile(strFileName);
#else
			return FALSE;
#endif
		}

	}
	catch(CFileException* pEfile)
	{
		if (pEfile->GetErrorMessage(szError, cnErrSize))
		{
			TRACE(_T("WARNING! MemoryDataProvider: Store Events data file error: %s \n"), szError);
		}
		pEfile->Delete();
		return FALSE;
	}
	catch(CArchiveException* pEarc)
	{
		if (pEarc->GetErrorMessage(szError, cnErrSize))
		{
			TRACE(_T("WARNING! MemoryDataProvider: Store Events data archive error: %s \n"), szError);
		}
		pEarc->Delete();
		return FALSE;
	}
	catch(_com_error& e)
	{
		const TCHAR* pErrStr = e.ErrorMessage();

		if (pErrStr)
		{
			TRACE(_T("WARNING! MemoryDataProvider: Store Events data COM error: %s \n"), pErrStr);
		}
		return FALSE;
	}

	return TRUE;
}

void CXTPCalendarMemoryDataProvider::Close()
{
	if (!IsOpen())
	{
		CXTPCalendarData::Open();
	}

	RemoveAllEvents();

	CXTPCalendarData::Close();
}

CXTPCalendarEventsPtr CXTPCalendarMemoryDataProvider::DoRetrieveDayEvents(COleDateTime dtDay)
{
	CXTPCalendarEventsPtr ptrEvents = new CXTPCalendarEvents;
	if (!ptrEvents)
	{
		return NULL;
	}
	m_tree.SearchForEvents((DWORD)dtDay, (DWORD)dtDay, ptrEvents);

	_PostProcessOccurrencesFromMaster2(dtDay, dtDay, ptrEvents);

	// call this function here to increase performance (to avoid clone events which will be removed)
	_FilterDayEventsInstancesByEndTime(dtDay, ptrEvents);

	ptrEvents->CloneEvents();

	return ptrEvents;
}

CXTPCalendarEventsPtr CXTPCalendarMemoryDataProvider::RetrieveEvents(COleDateTime dtStartDay, COleDateTime dtEndDay)
{
	CXTPCalendarEventsPtr ptrEvents= new CXTPCalendarEvents;
	if (!ptrEvents)
		return NULL;

	m_tree.SearchForEvents((DWORD)dtStartDay, (DWORD)dtEndDay, ptrEvents);

	_PostProcessOccurrencesFromMaster2(dtStartDay, dtEndDay, ptrEvents);

	// to remove multiday events which end day is dtStartDay.
	_FilterDayEventsInstancesByEndTime(dtStartDay, ptrEvents);

	ptrEvents->CloneEvents();

	return ptrEvents;
}

CXTPCalendarEventsPtr CXTPCalendarMemoryDataProvider::DoGetAllEvents_raw()
{
	CXTPCalendarEventsPtr ptrArray = new CXTPCalendarEvents();
	if (!ptrArray)
		return NULL;

	POSITION pos = m_EventsStorage.GetStartPosition();
	while (pos)
	{
		DWORD dwID = 0;
		CXTPCalendarEvent* pEvent = m_EventsStorage.GetNextElement(pos, dwID);
		if (pEvent)
		{
			ptrArray->Add(pEvent, TRUE);
		}
	}

	ptrArray->CloneEvents();

	return ptrArray;
}

CXTPCalendarEventsPtr CXTPCalendarMemoryDataProvider::DoGetUpcomingEvents(
								COleDateTime dtFrom, COleDateTimeSpan spPeriod)
{
	UNREFERENCED_PARAMETER(dtFrom); UNREFERENCED_PARAMETER(spPeriod);
	return DoGetAllEvents_raw();
}


//////////////////////////////////////////////////////////////////////////
//
// Internal search algorithms and structures
//
//////////////////////////////////////////////////////////////////////////
// DSTNode
//////////////////////////////////////////////////////////////////////////
CXTPCalendarMemoryDataProvider::DSTNode::DSTNode() :
		m_pMapEvents(NULL), pLeft(NULL), pRight(NULL)
{}

CXTPCalendarMemoryDataProvider::DSTNode::~DSTNode()
{
	if (m_pMapEvents)
		delete m_pMapEvents;
	if (pLeft)
		delete pLeft;
	if (pRight)
		delete pRight;
}
void CXTPCalendarMemoryDataProvider::DSTNode::Mark(CXTPCalendarEvent* pEvent)
{
	if (!m_pMapEvents)
	{
		m_pMapEvents = new EventsMap;

		if (!m_pMapEvents) {
			return;
		}
	}
	m_pMapEvents->SetAt(pEvent, TRUE);
}
void CXTPCalendarMemoryDataProvider::DSTNode::Unmark(CXTPCalendarEvent* pEvent)
{
	if (m_pMapEvents)
	{
		m_pMapEvents->RemoveKey(pEvent);
	}
}
BOOL CXTPCalendarMemoryDataProvider::DSTNode::IsMarked(CXTPCalendarEvent* pEvent)
{
	if (m_pMapEvents)
	{
		BOOL bValue;
		return m_pMapEvents->Lookup(pEvent, bValue);
	}
	return FALSE;
}


void CXTPCalendarMemoryDataProvider::DSTNode::AppendAll(CMapIDtoEvent* pMapEvents)
{
	if (!pMapEvents || !m_pMapEvents)
		return;

	// enumerate all elements of the map
	POSITION pos = m_pMapEvents->GetStartPosition();
	BOOL bMark;
	CXTPCalendarEvent* pEvent = NULL;
	while (pos != NULL)
	{
		m_pMapEvents->GetNextAssoc(pos, pEvent, bMark);
		if (bMark && pEvent)
		{
			pMapEvents->SetAt(pEvent->GetEventID(),  pEvent);
		}
	}

}
//////////////////////////////////////////////////////////////////////////
// DST
//////////////////////////////////////////////////////////////////////////
const int xtpBitsScan = sizeof(DWORD) * 8 - 1;

CXTPCalendarMemoryDataProvider::DST::DST() :
	tmp_mapEvents(64)
{
	pHead = new DSTNode();

	tmp_mapEvents.InitHashTable(XTP_MEMDP_TMP_HASH_TABLE_SIZE, FALSE);
}

CXTPCalendarMemoryDataProvider::DST::~DST()
{
	if (pHead)
		delete pHead;
}

void CXTPCalendarMemoryDataProvider::DST::Insert(CXTPCalendarEvent* pEvent)
{
	MarkRange(pEvent, dstScanMark);
}

void CXTPCalendarMemoryDataProvider::DST::Remove(CXTPCalendarEvent* pEvent)
{
	MarkRange(pEvent, dstScanUnmark);
}

void CXTPCalendarMemoryDataProvider::DST::Clear()
{
	if (pHead)
		delete pHead;

	pHead = new DSTNode();
}

void CXTPCalendarMemoryDataProvider::DST::MarkRange(CXTPCalendarEvent* pEvent, const DSTScanType eScanType)
{
	DWORD dwStartDay = (DWORD)pEvent->GetStartTime();
	DWORD dwEndDay = (DWORD)pEvent->GetEndTime();

	ScanRange(pHead, xtpBitsScan - 1, 0, (DWORD)(1 << xtpBitsScan),
			  dwStartDay, dwEndDay,
			  pEvent, NULL, eScanType);
}

void CXTPCalendarMemoryDataProvider::DST::SearchForEvents(DWORD dwStart, DWORD dwEnd, CXTPCalendarEvents* pEvents)
{
	if (!pEvents)
		return;

	tmp_mapEvents.RemoveAll();

	ScanRange(pHead, xtpBitsScan - 1, 0, (DWORD)(1 << xtpBitsScan),
			  dwStart, dwEnd,
			  NULL, &tmp_mapEvents, dstScanFind);


	// move events from temp map to array
	int nCount = (int)tmp_mapEvents.GetCount();
	if (nCount)
	{
		int nIndex = pEvents->GetCount();
		pEvents->SetSize(nIndex + nCount); // to avoid multi-reallocks

		DWORD dwKey;
		CXTPCalendarEvent* pEvent;
		POSITION pos = tmp_mapEvents.GetStartPosition();
		while(pos)
		{
			tmp_mapEvents.GetNextAssoc(pos, dwKey, pEvent);
			ASSERT(pEvent);
			if (pEvent)
				pEvents->SetAt(nIndex, pEvent, TRUE);

			nIndex++;
		}
		ASSERT(nIndex == pEvents->GetCount());

		tmp_mapEvents.RemoveAll();
	}
}

void CXTPCalendarMemoryDataProvider::DST::ScanRange(
	DSTNode* const pNode, const int nDepth,
	const DWORD dwMinValue, const DWORD dwMaxValue,
	const DWORD dwStart, const DWORD dwEnd,
	CXTPCalendarEvent* const pEvent, CMapIDtoEvent* pMapEvents,
	const DSTScanType eScanType)
{
	DWORD dwMediumValue = dwMinValue | (nDepth >= 0 ? 1 << nDepth : 0);

	switch(eScanType)
	{
	case dstScanMark:
	case dstScanUnmark:
		// check if range contained in the min-max range
		if (dwStart <= dwMinValue && dwEnd >= dwMaxValue - 1)
		{
			switch(eScanType)
			{
			case dstScanMark:
				// Add this event to this entire range node.
				pNode->Mark(pEvent);
				return;
			case dstScanUnmark:
				// Delete this event from this entire range node.
				pNode->Unmark(pEvent);
				return;
			}
		}
		break;
	case dstScanFind:
		if (dwEnd >= dwMinValue && dwStart < dwMaxValue)
		{
			pNode->AppendAll(pMapEvents);
		}
		break;
	}

	// check for end recursion
	if (nDepth < 0)
		return;

	// run left search
	if (dwStart <= dwMediumValue)
	{
		if (!pNode->pLeft && eScanType == dstScanMark)
			pNode->pLeft = new DSTNode();

		if (pNode->pLeft)
		{
			ScanRange(pNode->pLeft, nDepth - 1,
					  dwMinValue, dwMediumValue,
					  dwStart, dwEnd,
					  pEvent, pMapEvents,
					  eScanType);
		}
	}

	// run right search
	if (dwEnd >= dwMediumValue)
	{
		if (!pNode->pRight && eScanType == dstScanMark)
			pNode->pRight = new DSTNode;

		if (pNode->pRight)
		{
			ScanRange(pNode->pRight, nDepth - 1,
					  dwMediumValue, dwMaxValue,
					  dwStart, dwEnd,
					  pEvent, pMapEvents,
					  eScanType);
		}
	}

}

////////////////////////////////////////////////////////////////////////////
void CXTPCalendarMemoryDataProvider::Serialize(CArchive& ar)
{
	CXTPPropExchangeArchive px(ar);
	DoPropExchange(&px);
}
void CXTPCalendarMemoryDataProvider::DoPropExchange(CXTPPropExchange* pPX)
{
	if (pPX->IsLoading())
	{
		VERIFY(_Load(pPX));
	}
	else
	{
		VERIFY(_Save(pPX));
	}
}

BOOL CXTPCalendarMemoryDataProvider::_Load(CXTPPropExchange* pPX)
{
	//------------------------------------
	// - File data ver
	//
	// ..... - Events one by one
	//
	// a) MasterEvent
	// b) RecurrencePttern
	// c) DWORD - Exceptions count
	// d) ..... - RecurrencePttern_Exceptions one by one
	//
	// ..... - Events one by one
	//

	CXTPAutoResetValue<BOOL> autoReset(m_bDisableNotificationsSending, FALSE);
	m_bDisableNotificationsSending = TRUE;

	long nSchema = 0;

	PX_Long(pPX, _T("Version"), (long&)nSchema);
	pPX->ExchangeLocale();

	if (nSchema != XTP_MEM_DP_DATA_VER)
	{
		//AfxMessageBox(_T("Unsupported data file version."));
		TRACE(_T("ERROR! MemoryDataProvider: Unsupported data file version. (%d) \n"), nSchema);
		return FALSE;
	}

	CXTPPropExchangeSection secOptions(pPX->GetSection(_T("Options")));
	if (m_pCalendarOptions)
	{
		m_pCalendarOptions->DoPropExchange(&secOptions);
	}

	CXTPPropExchangeSection sec(pPX->GetSection(_T("Events")));
	CXTPPropExchangeEnumeratorPtr pEnumerator(sec->GetEnumerator(_T("Event")));
	POSITION pos = pEnumerator->GetPosition();

	DWORD dwMinEventID = (DWORD)-1;

	while (pos)
	{
		CXTPCalendarEventPtr ptrEvent = CreateNewEvent();
		if (!ptrEvent)
		{
			return FALSE;
		}
		CXTPPropExchangeSection secEvent(pEnumerator->GetNext(pos));
		ptrEvent->DoPropExchange(&secEvent);

		dwMinEventID = min(dwMinEventID, ptrEvent->GetEventID());

		int nRecState = ptrEvent->GetRecurrenceState();
		ASSERT(nRecState == xtpCalendarRecurrenceMaster || nRecState == xtpCalendarRecurrenceNotRecurring);

		if (nRecState == xtpCalendarRecurrenceMaster)
		{
			CXTPCalendarRecurrencePatternPtr ptrPattern = ptrEvent->GetRecurrencePattern();
			if (!ptrPattern || !ptrPattern)
			{
				ASSERT(FALSE);
				continue;
			}

			CXTPPropExchangeSection secPattern(secEvent->GetSection(_T("Pattern")));
			ptrPattern->DoPropExchange(&secPattern);

			DWORD dwPatternID = ptrPattern->GetPatternID();
			dwMinEventID = min(dwMinEventID, dwPatternID);

			CXTPPropExchangeEnumeratorPtr pEnumeratorExc(secPattern->GetEnumerator(_T("Exception")));
			POSITION posExc = pEnumeratorExc->GetPosition();

			while (posExc)
			{
				CXTPCalendarEventPtr ptrExcept = CreateNewEvent();
				if (!ptrExcept)
				{
					return TRUE;
				}

				CXTPPropExchangeSection secExceptEvent(pEnumeratorExc->GetNext(posExc));
				ptrExcept->DoPropExchange(&secExceptEvent);
				dwMinEventID = min(dwMinEventID, ptrExcept->GetEventID());

				nRecState = ptrExcept->GetRecurrenceState();
				ASSERT(nRecState == xtpCalendarRecurrenceException);

				ptrPattern->SetException(ptrExcept);
			}

			//=======================================================
			ptrEvent->UpdateRecurrence(ptrPattern);
		}

		VERIFY(AddEvent(ptrEvent));
	}

	//-----------------------------------------
	ms_dwNextFreeTempID = dwMinEventID;
	GetNextFreeTempID();

	//=========================================
	if (m_pSchedules)
	{
		CXTPPropExchangeSection secSchedules(pPX->GetSection(_T("EventsSchedules")));
		m_pSchedules->DoPropExchange(&secSchedules);
	}

	return TRUE;
}

BOOL CXTPCalendarMemoryDataProvider::_Save(CXTPPropExchange* pPX)
{
	//------------------------------------
	// - File data ver
	//
	// ..... - Events one by one
	//
	// a) MasterEvent
	// b) RecurrencePttern
	// c) DWORD - Exceptions count
	// d) ..... - RecurrencePttern_Exceptions one by one
	//
	// ..... - Events one by one
	//

	long nSchema = XTP_MEM_DP_DATA_VER;
	PX_Long(pPX, _T("Version"), (long&)nSchema);
	pPX->ExchangeLocale();

	CXTPPropExchangeSection secOptions(pPX->GetSection(_T("Options")));
	secOptions->EmptySection();
	if (m_pCalendarOptions)
	{
		m_pCalendarOptions->DoPropExchange(&secOptions);
	}

	CXTPPropExchangeSection sec(pPX->GetSection(_T("Events")));
	sec->EmptySection();

	CXTPPropExchangeEnumeratorPtr pEnumerator(sec->GetEnumerator(_T("Event")));
	POSITION posEnum = pEnumerator->GetPosition(m_EventsStorage.GetCount());

	POSITION pos = m_EventsStorage.GetStartPosition();
	while (pos)
	{
		DWORD dwId = 0;
		CXTPCalendarEvent* pEvent = m_EventsStorage.GetNextElement(pos, dwId);

		int nRecState = pEvent->GetRecurrenceState();
		ASSERT(nRecState == xtpCalendarRecurrenceMaster || nRecState == xtpCalendarRecurrenceNotRecurring);

		CXTPCalendarRecurrencePatternPtr ptrPattern;
		if (nRecState == xtpCalendarRecurrenceMaster)
		{
			ptrPattern = pEvent->GetRecurrencePattern();
			ASSERT(ptrPattern);
			if (ptrPattern == NULL)
				continue;
		}

		CXTPPropExchangeSection secEvent(pEnumerator->GetNext(posEnum));
		pEvent->DoPropExchange(&secEvent);

		if (nRecState == xtpCalendarRecurrenceMaster)
		{
			CXTPPropExchangeSection secPattern(secEvent->GetSection(_T("Pattern")));
			ptrPattern->DoPropExchange(&secPattern);

			CXTPCalendarEventsPtr ptrExceptions = ptrPattern->GetExceptions();

			DWORD dwExcCount = ptrExceptions ? ptrExceptions->GetCount() : 0;

			CXTPPropExchangeEnumeratorPtr pEnumeratorExc(secPattern->GetEnumerator(_T("Exception")));
			POSITION posExc = pEnumeratorExc->GetPosition(dwExcCount);

			for (int n = 0; n < (int)dwExcCount; n++)
			{
				CXTPCalendarEvent* pExcept = ptrExceptions->GetAt(n, FALSE);
				ASSERT(pExcept->GetRecurrenceState() == xtpCalendarRecurrenceException);

				CXTPPropExchangeSection secExceptEvent(pEnumeratorExc->GetNext(posExc));
				pExcept->DoPropExchange(&secExceptEvent);
			}
		}
	}

	//=========================================
	if (m_pSchedules)
	{
		CXTPPropExchangeSection secSchedules(pPX->GetSection(_T("EventsSchedules")));
		m_pSchedules->DoPropExchange(&secSchedules);
	}

	return TRUE;
}

void CXTPCalendarMemoryDataProvider::DoRemoveAllEvents()
{
	m_EventsStorage.RemoveAll();
	m_mapPatterns.RemoveAll();

	m_tree.Clear();
}

CXTPCalendarEventPtr CXTPCalendarMemoryDataProvider::DoRead_Event(DWORD dwEventID)
{
	CXTPCalendarEvent* pEvent = m_EventsStorage.Get(dwEventID);
	if (!pEvent)
	{
		return NULL;
	}

	return pEvent->CloneEvent();
}

CXTPCalendarRecurrencePatternPtr CXTPCalendarMemoryDataProvider::DoRead_RPattern(DWORD dwPatternID)
{
	CXTPCalendarRecurrencePattern* pPattern = m_mapPatterns.Get(dwPatternID);
	if (!pPattern)
	{
		return NULL;
	}

	CXTPCalendarRecurrencePatternPtr ptrPattern2 = pPattern->ClonePattern();
	if (ptrPattern2)
	{
		VERIFY( pPattern->GetOccReminders()->Save(ptrPattern2->GetCustomProperties(), NULL) );
	}
	return ptrPattern2;
}

BOOL CXTPCalendarMemoryDataProvider::DoCreate_Event(CXTPCalendarEvent* pEvent, DWORD& rdwNewEventID)
{
	if (!pEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	DWORD dwEventID = pEvent->GetEventID();

	int nRState = pEvent->GetRecurrenceState();
	if (nRState == xtpCalendarRecurrenceException)
	{
		rdwNewEventID = _UniqueID_Event(dwEventID);
		return TRUE;
	}

	ASSERT(nRState == xtpCalendarRecurrenceMaster || nRState == xtpCalendarRecurrenceNotRecurring);

	CXTPCalendarEventPtr ptrEvent2 = pEvent->CloneEvent();
	if (!ptrEvent2)
	{
		return FALSE;
	}

	rdwNewEventID = _UniqueID_Event(dwEventID);

	ptrEvent2->SetEventID(rdwNewEventID);
	pEvent->SetEventID(rdwNewEventID);

	dwEventID = rdwNewEventID;

	// add to storage
	m_EventsStorage.Add(dwEventID, ptrEvent2);

	// add to search tree
	m_tree.Insert(ptrEvent2);

	//- Process recurrence event -------------------------
	if (nRState == xtpCalendarRecurrenceMaster)
	{
		CXTPCalendarEventHelper* pEvent2Ex = (CXTPCalendarEventHelper*)(CXTPCalendarEvent*)ptrEvent2;
		CXTPCalendarRecurrencePattern* pPatternRef = pEvent2Ex->GetRPatternRef();
		ASSERT(pPatternRef);
		if (pPatternRef)
		{
			DWORD dwPatternID = ptrEvent2->GetRecurrencePatternID();
			DWORD dwPatternIDnew = _UniqueID_Patern(dwPatternID);

			ptrEvent2->SetRecurrencePatternID(dwPatternIDnew);
			pEvent->SetRecurrencePatternID(dwPatternIDnew);

			m_mapPatterns.Add(dwPatternID, pPatternRef);
		}
	}
	//----------------------------------------------------
	return TRUE;
}

BOOL CXTPCalendarMemoryDataProvider::DoUpdate_Event(CXTPCalendarEvent* pEvent)
{
	DWORD dwEventID = pEvent->GetEventID();

	ASSERT(dwEventID != XTP_CALENDAR_UNKNOWN_EVENT_ID);

	//- new recurrence state ---------------------------
	int nRState = pEvent->GetRecurrenceState();

	if (nRState == xtpCalendarRecurrenceException ||
		nRState == xtpCalendarRecurrenceOccurrence)
	{
		ASSERT(nRState == xtpCalendarRecurrenceException);
		return TRUE;
	}

	//-------------------------------------------------------
	CXTPCalendarEventPtr ptrMainEvent(m_EventsStorage.Get(dwEventID), TRUE);
	if (!ptrMainEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// update event in search tree
	m_tree.Remove(ptrMainEvent);

	VERIFY( m_EventsStorage.Remove(dwEventID) );
	if (ptrMainEvent->GetRecurrenceState() == xtpCalendarRecurrenceMaster)
	{
		DWORD dwPatternID = ptrMainEvent->GetRecurrencePatternID();
		m_mapPatterns.Remove(dwPatternID);
	}
	//******************************
	DWORD dwNewID;
	BOOL bRes = DoCreate_Event(pEvent, dwNewID);
	ASSERT(dwNewID == dwEventID);

	return bRes;
}

BOOL CXTPCalendarMemoryDataProvider::DoDelete_Event(CXTPCalendarEvent* pEvent)
{
	DWORD dwEventID = pEvent->GetEventID();
	ASSERT(dwEventID != XTP_CALENDAR_UNKNOWN_EVENT_ID);

	//- new recurrence state ---------------------------
	int nRState = pEvent->GetRecurrenceState();

	if (nRState == xtpCalendarRecurrenceException ||
		nRState == xtpCalendarRecurrenceOccurrence)
	{
		ASSERT(nRState == xtpCalendarRecurrenceException);
		return TRUE;
	}

	//-------------------------------------------------------
	CXTPCalendarEventPtr ptrEvent(m_EventsStorage.Get(dwEventID), TRUE);
	if (!ptrEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// remove event from storage
	BOOL bRes = m_EventsStorage.Remove(dwEventID);

	if (!bRes)
	{
		return FALSE;
	}
	// remove event from search tree
	m_tree.Remove(ptrEvent);

	return TRUE;
}

BOOL CXTPCalendarMemoryDataProvider::DoCreate_RPattern(CXTPCalendarRecurrencePattern* pPattern, DWORD& rdwNewPatternID)
{
	if (!pPattern)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	rdwNewPatternID = pPattern->GetPatternID();

	CXTPCalendarRecurrencePattern* pPatternRef = m_mapPatterns.Get(rdwNewPatternID);
	if (pPatternRef)
	{
		VERIFY( pPatternRef->Update(pPattern) );
	}

	return TRUE;
}

BOOL CXTPCalendarMemoryDataProvider::DoUpdate_RPattern(CXTPCalendarRecurrencePattern* pPattern)
{
	if (!pPattern)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	DWORD dwPatternID = pPattern->GetPatternID();

	ASSERT(m_mapPatterns.IsExist(dwPatternID));

	CXTPCalendarRecurrencePattern* pPatternRef = m_mapPatterns.Get(dwPatternID);
	ASSERT(pPatternRef);
	if (pPatternRef)
	{
		VERIFY( pPatternRef->Update(pPattern) );
	}

	return TRUE;
}

BOOL CXTPCalendarMemoryDataProvider::DoDelete_RPattern(CXTPCalendarRecurrencePattern* pPattern)
{
	if (!pPattern)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	DWORD dwPatternID = pPattern->GetPatternID();

	return m_mapPatterns.Remove(dwPatternID);
}

DWORD CXTPCalendarMemoryDataProvider::_UniqueID_Event(DWORD dwID)
{
	if (dwID == XTP_CALENDAR_UNKNOWN_EVENT_ID)
	{
		dwID = GetNextFreeTempID();
	}
	for (int i = 0; i < 1000 && m_EventsStorage.IsExist(dwID); i++)
	{
		dwID = GetNextFreeTempID();
	}
	ASSERT(m_EventsStorage.IsExist(dwID) == FALSE);
	return dwID;
}

DWORD CXTPCalendarMemoryDataProvider::_UniqueID_Patern(DWORD dwID)
{
	if (dwID == XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID)
	{
		dwID = GetNextFreeTempID();
	}
	for (int i = 0; i < 1000 && m_mapPatterns.IsExist(dwID); i++)
	{
		dwID = GetNextFreeTempID();
	}
	ASSERT(m_mapPatterns.IsExist(dwID) == FALSE);
	return dwID;
}
