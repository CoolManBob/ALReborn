// XTPCalendarResource.cpp: implementation of the CXTPCalendarResource class.
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
//#include "Resource.h"

//#include "Common/XTPResourceManager.h"
#include "Common/XTPPropExchange.h"

#include "XTPTopLevelWndMsgNotifier.h"
#include "XTPCalendarUtils.h"
#include "XTPCalendarNotifications.h"

#include "XTPCalendarDayView.h"
#include "XTPCalendarWeekView.h"
#include "XTPCalendarMonthView.h"

#include "XTPCalendarControl.h"

#include "XTPCalendarData.h"
#include "XTPCalendarMemoryDataProvider.h"
#include "XTPCalendarDatabaseDataProvider.h"
#include "XTPCalendarCustomDataProvider.h"
#include "XTPCalendarCustomProperties.h"

#include "XTPCalendarRemindersManager.h"

#include "XTPCalendarOccurSeriesChooseDlg.h"

#include "XTPCalendarResource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTP_SCHEDULES_DATA_VER 2

//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CXTPCalendarResource, CCmdTarget)
IMPLEMENT_DYNCREATE(CXTPCalendarResources, CCmdTarget)

IMPLEMENT_DYNAMIC(CXTPCalendarSchedule, CCmdTarget)
IMPLEMENT_DYNAMIC(CXTPCalendarSchedules, CCmdTarget)

CXTPCalendarSchedule::CXTPCalendarSchedule(UINT uScheduleID, LPCTSTR pcszScheduleName)
{
	m_uScheduleID = uScheduleID;

	ASSERT(pcszScheduleName);
	if (pcszScheduleName)
	{
		m_strScheduleName = pcszScheduleName;
	}

	m_pCustomProperties = new CXTPCalendarCustomProperties();

}

CXTPCalendarSchedule::~CXTPCalendarSchedule()
{
	CMDTARGET_RELEASE(m_pCustomProperties)
}

///////////////////////////////////////////////////////////////////////////////
CXTPCalendarSchedules::CXTPCalendarSchedules()
{
}

CXTPCalendarSchedules::~CXTPCalendarSchedules()
{
}

BOOL CXTPCalendarSchedules::AddNewSchedule(LPCTSTR pcszScheduleName)
{
	ASSERT(pcszScheduleName);
	if (!pcszScheduleName)
	{
		return FALSE;
	}

	UINT uNewID = _GetNextID();
	CXTPCalendarSchedule* pNewSch = new CXTPCalendarSchedule(uNewID, pcszScheduleName);
	if (pNewSch)
	{
		Add(pNewSch, FALSE);
	}

	return pNewSch != NULL;
}

BOOL CXTPCalendarSchedules::RemoveSchedule(UINT uScheduleID)
{
	int nFIdx = FindIndex(uScheduleID);

	if (nFIdx >= 0)
	{
		RemoveAt(nFIdx);
	}
	return nFIdx >= 0;
}

LPCTSTR CXTPCalendarSchedules::GetScheduleName(UINT uScheduleID) const
{
	int nFIdx = FindIndex(uScheduleID);

	if (nFIdx >= 0)
	{
		CXTPCalendarSchedule* pSch = GetAt(nFIdx, FALSE);
		if (pSch)
		{
			return pSch->GetName();
		}
	}
	return NULL;
}

void CXTPCalendarSchedules::SetScheduleName(UINT uScheduleID, LPCTSTR pcszNewName)
{
	int nFIdx = FindIndex(uScheduleID);

	if (nFIdx >= 0)
	{
		CXTPCalendarSchedule* pSch = GetAt(nFIdx, FALSE);
		if (pSch)
		{
			pSch->SetName(pcszNewName);
		}
	}
}

int CXTPCalendarSchedules::FindIndex(UINT uScheduleID) const
{
	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarSchedule* pSch = GetAt(i, FALSE);
		if (pSch && pSch->GetID() == uScheduleID)
		{
			return i;
		}
	}
	return -1;
}

UINT CXTPCalendarSchedules::_GetNextID() const
{
	UINT uMaxID = 0;

	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarSchedule* pSch = GetAt(i, FALSE);
		if (pSch && uMaxID < pSch->GetID())
		{
			uMaxID = pSch->GetID();
		}
	}
	return uMaxID + 1;
}

void CXTPCalendarSchedules::DoPropExchange(CXTPPropExchange* pPX)
{
	if (!pPX)
	{
		ASSERT(FALSE);
		return;
	}

	if (pPX->IsLoading())
	{
		_Load(pPX);
	}
	else
	{
		_Save(pPX);
	}
}

void CXTPCalendarSchedules::_Save(CXTPPropExchange* pPX)
{
	if (!pPX || !pPX->IsStoring())
	{
		ASSERT(FALSE);
		return;
	}

	CMDTARGET_ADDREF(pPX);
	CXTPPropExchangeSection secSchedules(pPX); //->GetSection(_T("EventsSchedules")));
	secSchedules->EmptySection();

	long nVersion = XTP_SCHEDULES_DATA_VER;
	PX_Long(&secSchedules, _T("Version"), nVersion, XTP_SCHEDULES_DATA_VER);

	int nCount = GetCount();
	CXTPPropExchangeEnumeratorPtr pEnumerator(secSchedules->GetEnumerator(_T("Schedule")));
	POSITION posStorage = pEnumerator->GetPosition(nCount);

	int nSavedCount = 0;
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarSchedule* pSch = GetAt(i, FALSE);
		ASSERT(pSch);
		if (!pSch)
		{
			continue;
		}
		ULONG ulID = pSch->GetID();
		CString strName = pSch->GetName();

		CXTPPropExchangeSection secSch(pEnumerator->GetNext(posStorage));
		PX_ULong(&secSch, _T("ID"), ulID);
		PX_String(&secSch, _T("Name"), strName);

		if (pSch->GetCustomProperties())
			pSch->GetCustomProperties()->DoPropExchange(&secSch);

		nSavedCount++;
	}
	ASSERT(nSavedCount == nCount);
}

void CXTPCalendarSchedules::_Load(CXTPPropExchange* pPX)
{
	if (!pPX || !pPX->IsLoading())
	{
		ASSERT(FALSE);
		return;
	}
	RemoveAll();

	CMDTARGET_ADDREF(pPX);
	CXTPPropExchangeSection secSchedules(pPX); //->GetSection(_T("EventsSchedules")));

	long nVersion = 0;
	PX_Long(&secSchedules, _T("Version"), nVersion, XTP_SCHEDULES_DATA_VER);

	if (nVersion != XTP_SCHEDULES_DATA_VER)
	{
		TRACE(_T("ERROR! XTPCalendarSchedules: Unsupported data file version. (%d) \n"), nVersion);
		return;
	}

	CXTPPropExchangeEnumeratorPtr pEnumerator(secSchedules->GetEnumerator(_T("Schedule")));
	POSITION posStorage = pEnumerator->GetPosition();

	while (posStorage)
	{
		ULONG ulID;
		CString strName;

		CXTPPropExchangeSection secSch(pEnumerator->GetNext(posStorage));
		PX_ULong(&secSch, _T("ID"), ulID, 0);
		PX_String(&secSch, _T("Name"), strName);

		CXTPCalendarSchedule* pNewSch = new CXTPCalendarSchedule(ulID, strName);
		if (!pNewSch)
		{
			return;
		}

		if (nVersion >= 2)
		{
			XTP_SAFE_CALL2(pNewSch, GetCustomProperties(), DoPropExchange(&secSch));
		}

		Add(pNewSch, FALSE);
	}
}

///////////////////////////////////////////////////////////////////////////////
CXTPCalendarResource::CXTPCalendarResource(CXTPCalendarControl* pCalendarCtrl)
	: m_pCalendarCtrl(pCalendarCtrl)
{
	m_pDataProvider = NULL;
	m_bCloseDataProviderWhenDestroy = TRUE;

}

CXTPCalendarResource::~CXTPCalendarResource()
{
	if (m_pDataProvider)
	{
		if (m_bCloseDataProviderWhenDestroy && m_pDataProvider->IsOpen())
		{
			m_pDataProvider->Close();
		}
		CMDTARGET_RELEASE(m_pDataProvider);
	}
}

void CXTPCalendarResource::SetDataProvider(CXTPCalendarData* pDataProvider,
										   BOOL bCloseDataProviderWhenDestroy)
{
	CMDTARGET_ADDREF(pDataProvider);
	CMDTARGET_RELEASE(m_pDataProvider);

	m_pDataProvider = pDataProvider;
	m_bCloseDataProviderWhenDestroy = bCloseDataProviderWhenDestroy;

}

CXTPCalendarEventsPtr CXTPCalendarResource::RetrieveDayEvents(COleDateTime dtDay)
{
	// get current day date
	dtDay = CXTPCalendarUtils::ResetTime(dtDay);

	//
	// retrieve all events for this day from the corresponding data provider
	//
	CXTPCalendarEventsPtr ptrEvents = m_pDataProvider ? m_pDataProvider->RetrieveDayEvents(dtDay) : new CXTPCalendarEvents();

	//
	// filter all events by ScheduleID collection
	//
	const int nScheduleIDCount = (int)m_arScheduleIDs.GetSize();

	// do not filter by schedule ID if no schedules set
	if (nScheduleIDCount)
	{
		const int nEventCount = ptrEvents ? ptrEvents->GetCount() : 0;
		for (int nEvent = nEventCount - 1; nEvent >= 0; nEvent--)
		{
			BOOL bRemove = TRUE;

			// get next event from the collection
			CXTPCalendarEvent* pEvent = ptrEvents->GetAt(nEvent, FALSE);
			ASSERT(pEvent);

			if (pEvent)
			{
				UINT uScheduleIDEvent = pEvent->GetScheduleID();
				bRemove = !ExistsScheduleID(uScheduleIDEvent, TRUE);
			}

			if (bRemove)
			{
				ptrEvents->RemoveAt(nEvent);
			}
		}

	}

	return ptrEvents;
}

BOOL CXTPCalendarResource::ExistsScheduleID(UINT uScheduleID,
												BOOL bReturnTrueIfSchedulesSetEmpty)
{
	const int nScheduleIDCount = (int)m_arScheduleIDs.GetSize();

	if (nScheduleIDCount == 0)
	{
		return bReturnTrueIfSchedulesSetEmpty;
	}

	// iterate all ScheduleID's
	for (int i = 0; i < nScheduleIDCount; i++)
	{
		UINT uScheduleID_I = m_arScheduleIDs.GetAt(i);

		if (uScheduleID_I == uScheduleID)
		{
			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//
// Resources collection
//
//

CXTPCalendarResources::CXTPCalendarResources()
{
}

CXTPCalendarResources::~CXTPCalendarResources()
{
}

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarResourcesNf::CXTPCalendarResourcesNf()
{
	m_pConnection = new CXTPNotifyConnection_internal();
}

CXTPCalendarResourcesNf::~CXTPCalendarResourcesNf()
{
	CXTPNotifySink::UnadviseAll();
	CMDTARGET_RELEASE(m_pConnection);
}


void CXTPCalendarResourcesNf::ReBuildInternalData()
{
	UnadviseAll();

	// rebuild optimized data
	m_arResourcesGroupedByDP.RemoveAll();

	int nRCCount = GetCount();
	for (int nRCNr = 0; nRCNr < nRCCount; nRCNr++)
	{
		CXTPCalendarResource* pRCorig = GetAt(nRCNr, FALSE);

		if (!pRCorig || !pRCorig->GetDataProvider() ||
			!pRCorig->GetSchedules())
			{
			ASSERT(FALSE);
			continue;
		}
		CXTPCalendarResource* pRCuniq = FindByDataProvider(&m_arResourcesGroupedByDP, pRCorig->GetDataProvider());

		if (!pRCuniq)
		{
			pRCuniq = new CXTPCalendarResource();
			if (!pRCuniq)
			{
				return;
			}
			pRCuniq->SetDataProvider(pRCorig->GetDataProvider(), FALSE);

			m_arResourcesGroupedByDP.Add(pRCuniq, FALSE);
		}

		int nSchCount = (int)pRCorig->GetSchedules()->GetSize();
		for (int j = 0; j < nSchCount; j++)
		{
			UINT nSchID = pRCorig->GetSchedules()->GetAt(j);
			if (!pRCuniq->ExistsScheduleID(nSchID, FALSE) )
			{
				pRCuniq->GetSchedules()->Add(nSchID);
			}
		}
	}

	//-------------------------
	if (!m_pConnection)
	{
		return;
	}
	// get notify IDs ---------
	CMap<XTP_NOTIFY_CODE, XTP_NOTIFY_CODE, int, int> mapNfIDs;
	int nConnCount = (int)m_pConnection->m_arrConnections.GetSize();
	for (int nConnNr = 0; nConnNr < nConnCount; nConnNr++)
	{
		CXTPNotifyConnection_internal::CONNECTION_DESCRIPTOR* pConnDesc =
										m_pConnection->m_arrConnections[nConnNr];
		ASSERT(pConnDesc);
		if (!pConnDesc)
		{
			continue;
		}
		mapNfIDs[pConnDesc->dwNotifyCode] = 1;
	}

	// advice
	POSITION posNfID = mapNfIDs.GetStartPosition();
	while (posNfID)
	{
		XTP_NOTIFY_CODE nfCode = 0;
		int nTmp;
		mapNfIDs.GetNextAssoc(posNfID, nfCode, nTmp);

		int nDPCount = m_arResourcesGroupedByDP.GetCount();
		for (int i = 0; i < nDPCount; i++)
		{
			CXTPNotifyConnection* pDPconn = XTP_SAFE_GET2(m_arResourcesGroupedByDP.GetAt(i),
											GetDataProvider(), GetConnection(), NULL);
			ASSERT(pDPconn);

			if (pDPconn)
			{
				CXTPNotifySink::Advise(pDPconn, nfCode);
			}
		}
	}
}

CXTPCalendarResource* CXTPCalendarResourcesNf::FindByDataProvider(
										CXTPCalendarResources* pResources,
										CXTPCalendarData* pData)
{
	if (!pResources || !pData)
	{
		ASSERT(FALSE);
		return NULL;
	}

	int nCount = pResources->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarResource* pRC = pResources->GetAt(i);
		ASSERT(pRC);
		if (pRC && pRC->GetDataProvider() == pData)
		{
			return pRC;
		}
	}
	return NULL;
}

void CXTPCalendarResourcesNf::OnEvent(XTP_NOTIFY_CODE dwNotifyCode, WPARAM wParam,
									  LPARAM lParam, DWORD dwFlags)

{
	UNUSED_ALWAYS(dwFlags);

	if (dwNotifyCode == XTP_NC_CALENDAREVENTWASADDED ||
		dwNotifyCode == XTP_NC_CALENDAREVENTWASDELETED ||
		dwNotifyCode == XTP_NC_CALENDAREVENTWASCHANGED)
	{
		CXTPCalendarEvent* pEvent = (CXTPCalendarEvent*)lParam;
		ASSERT(pEvent);

		if (!pEvent)
		{
			return;
		}
		CXTPCalendarResource* pRC = FindByDataProvider(&m_arResourcesGroupedByDP, pEvent->GetDataProvider());

		UINT uSchID = pEvent->GetScheduleID();
		if (!pRC || !pRC->ExistsScheduleID(uSchID, TRUE))
		{
			return;
		}
	}

	ASSERT(GetConnection());

	if (GetConnection())
	{
		GetConnection()->SendEvent(dwNotifyCode, wParam, lParam, dwFlags);
	}
}

/////////////////////////////////////////////////////////////////////////////
