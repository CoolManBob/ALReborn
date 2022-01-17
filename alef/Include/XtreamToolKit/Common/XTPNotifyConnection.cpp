// XTPNotifyConnection.cpp: implementation of the CXTPNotifyConnection and CXTPNotifySink classes.
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

#include "stdafx.h"
#include "XTPNotifyConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable: 4571) // warning C4571: catch(...) blocks compiled with /EHs do not catch or re-throw Structured Exceptions

/////////////////////////////////////////////////////////////////////////////
static LPCTSTR XTP_NOTIFICATION_SINK_MT_MSGWND_NAME  = _T("XTPNotificationSinkMT_MsgWnd");


/////////////////////////////////////////////////////////////////////////////
//class CEmptySyncObject : public CSyncObject

CXTPNotifyConnection::CEmptySyncObject::CEmptySyncObject() :
		CSyncObject(_T("XTPEmptySyncObject"))
{
}

////////////////////////////////////////////////////////////////////////////

CXTPNotifyConnection::CXTPNotifyConnection()
{
	m_nSendQueueCacheSize = 0;
}

CXTPNotifyConnection::~CXTPNotifyConnection()
{
	RemoveAll();
}

void CXTPNotifyConnection::RemoveAll()
{
	CSingleLock singleLock(GetDataLock(), TRUE);

	int nCount = (int)m_arrConnections.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CONNECTION_DESCRIPTOR* pCurElem = m_arrConnections[i];
		ASSERT(pCurElem);
		if (pCurElem)
		{
			delete pCurElem;
		}
		m_arrConnections[i] = NULL;
	}

	m_arrConnections.RemoveAll();
}

XTP_CONNECTION_ID CXTPNotifyConnection::Advise(XTP_NOTIFY_CODE Event, CXTPNotifySink* pSink)
{
	ASSERT(pSink);

	CSingleLock singleLock(GetDataLock(), TRUE);

	CONNECTION_DESCRIPTOR* pNewElem = new CONNECTION_DESCRIPTOR;

	if (!pNewElem)
	{
		return 0;
	}

	pNewElem->dwConnectionID = (XTP_CONNECTION_ID)pNewElem;
	pNewElem->dwNotifyCode = Event;
	pNewElem->pSink = pSink;

	m_arrConnections.Add(pNewElem);

	return pNewElem->dwConnectionID;
}

void  CXTPNotifyConnection::Unadvise(XTP_CONNECTION_ID ConnectionID)
{
	CSingleLock singleLock(GetDataLock(), TRUE);

	try
	{
		int nCount = (int)m_arrConnections.GetSize();
		int nFIndex = FindConnection(ConnectionID);

		if (nFIndex >= 0 && nFIndex < nCount)
		{
			CONNECTION_DESCRIPTOR* pElem = m_arrConnections[nFIndex];

			ASSERT(pElem);
			if (pElem)
			{
				delete pElem;
			}
			m_arrConnections.RemoveAt(nFIndex);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	catch(...)
	{
		ASSERT(FALSE);
		TRACE(_T("EXCEPTION! CXTPNotifyConnection::Unadvise(ConnectionID = %d)\n"), ConnectionID);
	}
}

BOOL CXTPNotifyConnection::SendEvent(XTP_NOTIFY_CODE Event,
								WPARAM wParam , LPARAM lParam, DWORD dwFlags)
{
	CSingleLock singleLock(GetDataLock(), TRUE);
	InternalAddRef();

	int nCount = (int)m_arrConnections.GetSize();

	if (m_arrSendQueueCache.GetSize() < nCount + m_nSendQueueCacheSize)
	{
		m_arrSendQueueCache.SetSize(nCount + m_nSendQueueCacheSize);
	}
	//******************************************************************************
	int nFirstLocalClientIndex = m_nSendQueueCacheSize;

	int i;
	for (i = 0; i < nCount; i++)
	{
		CONNECTION_DESCRIPTOR* pElem = m_arrConnections[i];
		ASSERT(pElem);
		if (pElem && pElem->dwNotifyCode == Event)
		{
			m_arrSendQueueCache.SetAt(m_nSendQueueCacheSize, *pElem);
			m_nSendQueueCacheSize++;
		}
	}
	int nLastLocalClientIndex = m_nSendQueueCacheSize-1;

	singleLock.Unlock();
	//************************************************************************
	for (i = nFirstLocalClientIndex; i <= nLastLocalClientIndex; i++)
	{
		singleLock.Lock();

		if (i >= m_arrSendQueueCache.GetSize())
		{
			ASSERT(FALSE);
			return FALSE;
		}

		CONNECTION_DESCRIPTOR& rElem = m_arrSendQueueCache.ElementAt(i);
		int nFIndex = FindConnection(rElem.dwConnectionID);
		if (nFIndex < 0)
		{
			// Unadvise was called inside OnEvent(...) Call
			continue;
		}
		try
		{
			CXTPNotifySink* ptrSink = rElem.pSink;

			singleLock.Unlock();

			if (ptrSink)
			{
				ptrSink->OnEvent(Event, wParam, lParam, dwFlags);
			}
			else
			{
				ASSERT(FALSE);
				TRACE(_T("CXTPNotifyConnection::SendEvent(Event = %d, wParam = %d, lParam = %d, dwFlags = %x) pSink = %x\n"),
						Event, wParam, lParam, dwFlags, rElem.pSink);
			}
		}
		catch(...)
		{
			ASSERT(FALSE);
			TRACE(_T("EXCEPTION! CXTPNotifyConnection::SendEvent(Event = %d, wParam = %d, lParam = %d, dwFlags = %x)\n"),
					Event, wParam, lParam, dwFlags);
		}
	}

	//==============================================================================
	singleLock.Lock();

	m_nSendQueueCacheSize = nFirstLocalClientIndex;

	singleLock.Unlock();

	InternalRelease();
	return (nFirstLocalClientIndex <= nLastLocalClientIndex);
}

int CXTPNotifyConnection::FindConnection(XTP_CONNECTION_ID ConnectionID)
{
	CSingleLock singleLock(GetDataLock(), TRUE);

	int nCount = (int)m_arrConnections.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CONNECTION_DESCRIPTOR* pCurElem = m_arrConnections[i];
		ASSERT(pCurElem);
		if (pCurElem)
		{
			if (pCurElem->dwConnectionID == ConnectionID)
			{
				return i;
			}
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////
//class CXTPNotifyConnectionMT : public CXTPNotifyConnection

CXTPNotifyConnectionMT::CXTPNotifyConnectionMT()
{
}

CXTPNotifyConnectionMT::~CXTPNotifyConnectionMT()
{
}


////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CXTPNotifySinkImplMTMsgWnd, CWnd)
	ON_REGISTERED_MESSAGE(xtp_wm_NotificationSinkMTOnEvent, OnInterThreadEvent)
END_MESSAGE_MAP()

CXTPNotifySinkImplMTMsgWnd::CXTPNotifySinkImplMTMsgWnd()
{
}

CXTPNotifySinkImplMTMsgWnd::~CXTPNotifySinkImplMTMsgWnd()
{
}

BOOL CXTPNotifySinkImplMTMsgWnd::CreateWnd()
{
	LPCTSTR pcszSimpleWndClass = AfxRegisterWndClass(0);
	CRect rcEmpty(0, 0, 0, 0);

	BOOL bCreated = CreateEx(0, pcszSimpleWndClass,
		XTP_NOTIFICATION_SINK_MT_MSGWND_NAME,
		WS_POPUP, rcEmpty, NULL, 0);
	ASSERT(bCreated);
	return bCreated;
}

//////////////////////////////////////////////////////////////////////////
// CXTPNotifySink

CXTPNotifySink::CXTPNotifySink()
{
}

CXTPNotifySink::~CXTPNotifySink()
{
	UnadviseAll();
}

XTP_CONNECTION_ID CXTPNotifySink::Advise(CXTPNotifyConnection* pConnection,
								XTP_NOTIFY_CODE dwNotifyCode)
{
	ASSERT(pConnection);
	if (!pConnection)
	{
		return 0;
	}

	XTP_CONNECTION_ID dwConnectionID = pConnection->Advise(dwNotifyCode, this);
	ASSERT(dwConnectionID);

	if (dwConnectionID)
	{
		ADVISE_DESCRIPTOR advDataNew;
		ASSERT(!m_mapAdviseData.Lookup(dwConnectionID, advDataNew));

		advDataNew.dwConnectionID = dwConnectionID;
		advDataNew.pConnection = pConnection;
		advDataNew.dwNotifyCode = dwNotifyCode;
		pConnection->InternalAddRef();

		m_mapAdviseData.SetAt(dwConnectionID, advDataNew);

	}
	return dwConnectionID;
}

void CXTPNotifySink::UnadviseAll()
{
	XTP_CONNECTION_ID ConnectionID = 0;
	ADVISE_DESCRIPTOR advData;
	POSITION pos = m_mapAdviseData.GetStartPosition();
	while (pos)
	{
		m_mapAdviseData.GetNextAssoc(pos, ConnectionID, advData);
		if (advData.pConnection)
		{
			advData.pConnection->Unadvise(advData.dwConnectionID);
			advData.pConnection->InternalRelease();
		}
	}
	m_mapAdviseData.RemoveAll();
}

void CXTPNotifySink::Unadvise(XTP_CONNECTION_ID ConnectionID)
{
	ADVISE_DESCRIPTOR advData;
	if (m_mapAdviseData.Lookup(ConnectionID, advData))
	{
		if (advData.pConnection)
		{
			advData.pConnection->Unadvise(advData.dwConnectionID);
			advData.pConnection->InternalRelease();
		}
		m_mapAdviseData.RemoveKey(ConnectionID);
	}
	else
	{
		ASSERT(FALSE);
	}
}
