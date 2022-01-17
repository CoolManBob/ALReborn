// XTPNotifyConnection.h: interface for CXTPNotifyConnection and
// CXTPNotifySik classes.
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

//{{AFX_CODEJOCK_PRIVATE
#ifndef _XTPNOTIFYCONNECTION_H__
#define _XTPNOTIFYCONNECTION_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4097)

#include <afxmt.h>
#include "XTPVC80Helpers.h"

/////////////////////////////////////////////////////////////////////////////
class CXTPNotifySink;

typedef DWORD_PTR XTP_CONNECTION_ID;
typedef DWORD XTP_NOTIFY_CODE;


//-------------------------------------------------------------------------
// Summary: Notify flags options
//-------------------------------------------------------------------------
enum XTPNotifyFlags
{
	xtpNotifyPostMessage                = 0x80000000, // Event will be posted
	xtpNotifyGuarantyPost               = 0x40000000, // Used together with nofPostMessage. Event will be guaranty posted. (Wait untill PostMessage returns TRUE)
	xtpNotifyDirectCallForOneThread     = 0x20000000  // Event handler will be called directly if sender and receiver are in one thread.
};

//===========================================================================
// Summary:
//     This class is used as implementation of Connection interface in
//     a XTPNotification mechanism. This mechanism consists of
//     Connection object(s) and Sink object(s). The first are used to
//     send notification messages and the second are used to receive
//     these notification. Each notification has unique number
//     (or EventCode or NotificationCode) for a given connection
//     object or for the whole system. This depends on implementation.
//     Using system unique NotificationCodes is a preferred way to avoid
//     potential errors with intersected NotificationCodes.
// See Also: CXTPNotifySink overview,
//           CXTPNotifyConnection overview,
//===========================================================================
class _XTP_EXT_CLASS CXTPNotifyConnection : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// See Also: ~CXTPNotifyConnection()
	//-----------------------------------------------------------------------
	CXTPNotifyConnection();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also: CXTPNotifyConnection()
	//-----------------------------------------------------------------------
	virtual ~CXTPNotifyConnection();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Establishes a connection between the connection object and
	//     a sink object.
	// Parameters:
	//     dwNotifyCode - EventCode of a Notification.
	//     pSink     - Pointer to the Sink object which will receive
	//                 events with specified code.
	// Remarks:
	//     Call this method to establish a connection with the event
	//     source and the sink object. pSink->OnEvent(...) method will be
	//     called if event with specified EventCode will occur.
	//     Use Unadvise() method with returned connection ID to terminate
	//     the connection call.
	// Returns:
	//     The unique connection ID used for Unadvise() method.
	// See Also: CXTPNotifySink overview, Unadvise() method.
	//-----------------------------------------------------------------------
	XTP_CONNECTION_ID Advise(XTP_NOTIFY_CODE dwNotifyCode, CXTPNotifySink* pSink);

	//-----------------------------------------------------------------------
	// Summary:
	//     Terminate a connection between the connection object and
	//     a sink object.
	// Parameters:
	//     ConnectionID - The unique connection ID previously returned
	//                    in Advise() call.
	// Remarks:
	//     Call this method to Terminate a connection with the event
	//     source and the sink object.
	// See Also: Advise() method.
	//-----------------------------------------------------------------------
	void Unadvise(XTP_CONNECTION_ID ConnectionID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Remove All connections information.
	// Remarks:
	//     Call this method to remove all connections added using
	//     Advise method.
	// See Also: Advise method
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Send event with specified EventCode.
	// Parameters:
	//          dwNotifyCode   - EventCode of a Notification.
	//          wParam      - First parameter specific for this Notification.
	//          lParam      - Second parameter specific for this Notification.
	//          dwFlags     - Additional flags. See XTPNotifyFlags.
	// Remarks:
	//     Call this method to send event with specified EventCode to
	//     sinks which where advised to this EventCode using Advise
	//     method.
	// Returns:
	//     TRUE if event was sent to any client (if any client exists),
	//     FALSE - otherwise.
	// See Also: Advise(), XTPNotifyFlags.
	//-----------------------------------------------------------------------
	BOOL SendEvent(XTP_NOTIFY_CODE dwNotifyCode, WPARAM wParam , LPARAM lParam, DWORD dwFlags = 0);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Find connection information in the m_arConnections array.
	// Parameters:
	//     ConnectionID - The unique connection ID.
	// Remarks:
	//     Call this method to find connection description in the
	//     m_arConnections array using specified connection ID.
	// Returns:
	//     The zero-based index of connection description in the
	//     m_arConnections array that matches the requested ConnectionID;
	//     -1 if the it is not found.
	// See Also: Advise method, CONNECTION_DESCRIPTOR
	//-----------------------------------------------------------------------
	int FindConnection(XTP_CONNECTION_ID ConnectionID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get synchronization object to lock internal class data.
	// Remarks:
	//     This method is used as prototype for derived classes to
	//     support multi threaded clients calls. In base
	//     implementation pointer to an empty locker object is returned.
	// Returns:
	//     Pointer to the locker object.
	// See Also: CCriticalSection, CSingleLock, CSyncObject.
	//-----------------------------------------------------------------------
	virtual CSyncObject* GetDataLock();

	//===========================================================================
	// Summary:
	//     This struct is used in the implementation of the
	//     class CXTPNotifyConnection to store connection between
	//     a Connection object, Notification code and the sink object.
	// See Also: CXTPNotifyConnection overview,
	//           CXTPNotifySink overview,
	//===========================================================================
	struct CONNECTION_DESCRIPTOR
	{
		XTP_NOTIFY_CODE    dwNotifyCode;   // The Notification code.
		CXTPNotifySink*    pSink;          // The pointer to the sink object. (with InternalAddRef)
		XTP_CONNECTION_ID  dwConnectionID; // The unique connection ID used for Unadvise() method.
	};

	//===========================================================================
	// Summary:
	//     This struct is derived from CSyncObject and should be used
	//     instead of CCriticalSection to improve performance and
	//     to provide compatibility. It just provide CSyncObject
	//     interface with empty Lock and Unlock methods.
	// See Also: CCriticalSection, CSingleLock, CSyncObject.
	//===========================================================================
	class CEmptySyncObject : public CSyncObject
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Default object constructor.
		//-----------------------------------------------------------------------
		CEmptySyncObject();

		//-----------------------------------------------------------------------
		// Summary:
		//     Set object state as locked.
		// Parameters:
		//     dwTimeout - Specifies the amount of time to wait for the
		//                 synchronization object to be available (signaled)
		// Remarks:
		//     Empty Lock method.
		// Returns:
		//     TRUE
		// See Also: CCriticalSection::Lock, CSingleLock, CSyncObject.
		//-----------------------------------------------------------------------
		virtual BOOL Lock(DWORD dwTimeout = INFINITE);

		//-----------------------------------------------------------------------
		// Summary:
		//     Set object state as unlocked.
		// Parameters:
		//     lCount     - Number of accesses to release
		//     lPrevCount - Points to a variable to receive the previous count of the synchronization object
		// Remarks:
		//     Empty Unlock method.
		// Returns:
		//     TRUE
		// See Also: CCriticalSection::Unlock, CSingleLock, CSyncObject.
		//-----------------------------------------------------------------------
		virtual BOOL Unlock();
		virtual BOOL Unlock(LONG lCount, LPLONG lPrevCount = NULL); //<combine CXTPNotifyConnection::CEmptySyncObject::Unlock>
	};

protected:
	CArray<CONNECTION_DESCRIPTOR*, CONNECTION_DESCRIPTOR*> m_arrConnections; // store connections between a Connection object, Notification code and the sink object.
	CArray<CONNECTION_DESCRIPTOR, CONNECTION_DESCRIPTOR&> m_arrSendQueueCache; // used in SendEvent method for safety reason
	int m_nSendQueueCacheSize; // used in SendEvent method together with m_arrSendQueueCache.
	CEmptySyncObject m_emptyLocker; // Pseudo-locker object
};


//===========================================================================
// Summary:
//     This class is used as thread-safety implementation of Connection
//     interface in a XTPNotification mechanism. This mechanism consists of
//     Connection object(s) and Sink object(s). The first are used to
//     send notification messages and the second are used to receive
//     these notification. Each notification has unique number
//     (or EventCode or NotificationCode) for a given connection
//     object or for the whole system. This depends on implementation.
//     Using system unique NotificationCodes is a preferred way to avoid
//     potential errors with intersected NotificationCodes.
// See Also: CXTPNotifySink, CXTPNotifyConnection
//===========================================================================
class _XTP_EXT_CLASS CXTPNotifyConnectionMT : public CXTPNotifyConnection
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPNotifyConnectionMT();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPNotifyConnectionMT();

	//-----------------------------------------------------------------------
	// Summary:
	//     Post event with specified EventCode.
	// Parameters:
	//          dwNotifyCode  - EventCode of a Notification.
	//          wParam  - First parameter specific for this Notification.
	//          lParam  - Second parameter specific for this Notification.
	//          dwFlags     - Additional flags. See XTPNotifyFlags.
	// Remarks:
	//          Call this method to post event with specified EventCode to
	//          sinks which where advised to this EventCode using Advise
	//          method. Event will be posted only to the sinks which
	//          implementations support Post event possibility, like standard
	//          multithreaded sinks.
	//          The sink implementation is responsible for the Post mechanism.
	//          For posted event the appropriate bit of dwFlags is set
	//          automatically and SendEvent is called.
	// Returns:
	//          TRUE if event was sent to any client (if any client exists),
	//          FALSE - otherwise.
	// See Also: Advise(), SendEvent(), XTPNotifyFlags.
	//-----------------------------------------------------------------------
	BOOL PostEvent(XTP_NOTIFY_CODE dwNotifyCode, WPARAM wParam , LPARAM lParam,
					DWORD dwFlags = 0);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Get synchronization object to lock internal class data.
	// Remarks:
	//     This method is used to support multi threaded clients calls..
	// Returns:
	//     Pointer to the locker object (CCriticalSection).
	// See Also: CCriticalSection, CSingleLock, CSyncObject.
	//-----------------------------------------------------------------------
	virtual CSyncObject* GetDataLock();

	CCriticalSection m_DataLockerCS; // Data locker object
};


//{{AFX_CODEJOCK_PRIVATE
static LPCTSTR XTP_NOTIFICATION_SINK_MT_ON_EVENT_MSG = _T("XTPNotificationSinkMTOnEvent");
const UINT xtp_wm_NotificationSinkMTOnEvent = RegisterWindowMessage(XTP_NOTIFICATION_SINK_MT_ON_EVENT_MSG);
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     This class is used as implementation of Sink interface in
//     a XTPNotification mechanism. This mechanism consists of
//     Connection object(s) and Sink object(s). The first are used to
//     send notification messages and the second are used to receive
//     these notification. Each notification has unique number
//     (or dwNotifyCode or NotificationCode) for a given connection
//     object or for the whole system. This depends on implementation.
//     Using system unique NotificationCodes is a preferred way to avoid
//     potential errors with intersected NotificationCodes.
// See Also: CXTPNotifySink overview, DECLARE_XTPSINK macro,
//           CXTPNotifyConnection overview,
//===========================================================================
class _XTP_EXT_CLASS CXTPNotifySink
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// See Also: ~CXTPNotifySink()
	//-----------------------------------------------------------------------
	CXTPNotifySink();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also: CXTPNotifySink()
	//-----------------------------------------------------------------------
	virtual ~CXTPNotifySink();

public:
	// -----------------------------------------------------------------------------------------
	// Summary:
	//     Event with specified dwNotifyCode was occurred.
	// Parameters:
	//     dwNotifyCode :  EventCode of a Notification.
	//     wParam :   First parameter specific for this Notification.
	//     lParam :   Second parameter specific for this Notification.
	//     dwFlags  :   Additional flags. See XTPNotifyFlags.
	// Remarks:
	//     This method is called by Connection object to notify advice sink
	//     that event with specified EventCode was occurred. It should be
	//     override in your derived class to receive notification(s).
	// See Also:
	//           DECLARE_XTPSINK macro, DECLARE_XTPSINK_MT macro,
	//           CXTPNotifyConnection,
	//           CXTPNotifyConnection::Advise method
	//-----------------------------------------------------------------------
	virtual void OnEvent(XTP_NOTIFY_CODE dwNotifyCode, WPARAM wParam,
						 LPARAM lParam, DWORD dwFlags) = 0;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Terminate all connections between the connection object(s) and
	//     this sink object.
	// Remarks:
	//     Call this method to Terminate all connections with the event
	//     source(s) and this sink object.
	// See Also: CXTPNotifySink, UnadviseAll method, Advise method,
	//           CXTPNotifyConnection,
	//           CXTPNotifyConnection::Unadvise method
	//-----------------------------------------------------------------------
	void UnadviseAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Terminate a connection between the connection object and
	//     this sink object.
	// Parameters:
	//     id - The unique connection ID previously returned
	//          in Advise() call.
	// Remarks:
	//     Call this method to Terminate a connection with the event
	//     source and this sink object.
	// See Also: CXTPNotifySink, UnadviseAll method, Advise method,
	//           CXTPNotifyConnection,
	//           CXTPNotifyConnection::Unadvise method
	//-----------------------------------------------------------------------
	void Unadvise(XTP_CONNECTION_ID id);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Establishes a connection between the connection object and
	//     a sink object.
	// Parameters:
	//     pConnection  - Pointer to the Connection object.
	//     dwNotifyCode - Notify Code of a Notification.
	// Remarks:
	//     Call this method to establish a connection with the event
	//     source and the sink object. OnEvent(...) method will be
	//     called if event with specified dwNotifyCode will occur.
	//     Use Unadvise() method with returned connection ID to terminate
	//     the connection call.
	// Returns:
	//     The unique connection ID used for Unadvise() method.
	// See Also: CXTPNotifySink, Unadvise method, GetParam method,
	//           CXTPNotifyConnection,
	//           CXTPNotifyConnection::Advise method
	//-----------------------------------------------------------------------
	XTP_CONNECTION_ID Advise(CXTPNotifyConnection* pConnection, XTP_NOTIFY_CODE dwNotifyCode);

protected:
	//===========================================================================
	// Summary:
	//     This struct is used in to store connection between a Connection
	//     object(s), Notification code and the sink object.
	// See Also: CXTPNotifySink overview,
	//===========================================================================
	struct ADVISE_DESCRIPTOR
	{
		XTP_NOTIFY_CODE        dwNotifyCode;   // The Notification code.
		CXTPNotifyConnection*  pConnection;    // The pointer to the connection object. (with InternalAddRef)
		XTP_CONNECTION_ID      dwConnectionID; // Original Connection ID returned by pConnection->Advise() method.
	};

	CMap<XTP_CONNECTION_ID, XTP_CONNECTION_ID&, ADVISE_DESCRIPTOR, ADVISE_DESCRIPTOR&> m_mapAdviseData; // store connections between a Connection objects, Notification code and this sink object.
};


//////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPNotifyConnection::CEmptySyncObject::Lock(DWORD /*dwTimeout*/) {
	return TRUE;
}
AFX_INLINE BOOL CXTPNotifyConnection::CEmptySyncObject::Unlock() {
	return TRUE;
}
AFX_INLINE BOOL CXTPNotifyConnection::CEmptySyncObject::Unlock(LONG /*lCount*/, LPLONG /*lPrevCount = NULL*/) {
	return TRUE;
}
AFX_INLINE CSyncObject* CXTPNotifyConnection::GetDataLock() {
	return &m_emptyLocker;
}
AFX_INLINE CSyncObject* CXTPNotifyConnectionMT::GetDataLock() {
	return &m_DataLockerCS;
}
AFX_INLINE BOOL CXTPNotifyConnectionMT::PostEvent(XTP_NOTIFY_CODE dwNotifyCode, WPARAM wParam , LPARAM lParam, DWORD dwFlags) {
	return SendEvent(dwNotifyCode, wParam, lParam, dwFlags | xtpNotifyPostMessage);
}

//{{AFX_CODEJOCK_PRIVATE

// to avoid warning C4786 MAPHANDLER is used.
#if (_MSC_VER <= 1200)

template<int HandlerSize>
class XTP_MAPHANDLER_T
{
public:
	BYTE pfHandler[HandlerSize];
};

template<typename T_pfHandler>
class CXTPNotifyMapCodeToHandler : public CMap<XTP_NOTIFY_CODE, XTP_NOTIFY_CODE, XTP_MAPHANDLER_T<sizeof(T_pfHandler)>, XTP_MAPHANDLER_T<sizeof(T_pfHandler)>&>
{
	typedef XTP_MAPHANDLER_T<sizeof(T_pfHandler)> HANDLER;
	typedef CMap<XTP_NOTIFY_CODE, XTP_NOTIFY_CODE, HANDLER, HANDLER&> TBase;
public:
	AFX_INLINE void SetAt(XTP_NOTIFY_CODE dwNotifyCode, T_pfHandler pfHandler)
	{
		HANDLER hnd;
		MEMCPY_S(&hnd.pfHandler, &pfHandler, sizeof(T_pfHandler));
		
		TBase::SetAt(dwNotifyCode, hnd);
	}

	AFX_INLINE BOOL Lookup(XTP_NOTIFY_CODE dwNotifyCode, T_pfHandler& rValue)
	{
		HANDLER hnd;
		if (TBase::Lookup(dwNotifyCode, hnd))
		{
			MEMCPY_S(&rValue, &hnd.pfHandler, sizeof(T_pfHandler));
			return TRUE;
		}
		rValue = 0;
		return FALSE;
	}
};

#endif


template<class ownerClassName, class _CInformator>
class CXTPNotifySinkImpl : public CXTPNotifySink
{
public:
	CXTPNotifySinkImpl() {
		m_mapHandlers.InitHashTable(101, FALSE);
	}
	virtual ~CXTPNotifySinkImpl() {
		UnadviseAll();
	};
public:
	typedef void (ownerClassName::*T_pfHandler) (XTP_NOTIFY_CODE dwNotifyCode, WPARAM wParam , LPARAM lParam);

	XTP_CONNECTION_ID Advise(CXTPNotifyConnection* pConnection, XTP_NOTIFY_CODE dwNotifyCode, T_pfHandler pfHandler)
	{
		m_mapHandlers.SetAt(dwNotifyCode, pfHandler);
		return CXTPNotifySink::Advise(pConnection, dwNotifyCode);
	}

	virtual void OnEvent(XTP_NOTIFY_CODE dwNotifyCode, WPARAM wParam,
						 LPARAM lParam, DWORD /*dwFlags*/)
	{
		ownerClassName* pThis = _CInformator::GetPThis((BYTE*)this);

		if (!pThis) {
			ASSERT(FALSE);
			return;
		}

		T_pfHandler pfHandler = 0;
		if (m_mapHandlers.Lookup(dwNotifyCode, pfHandler) && (pfHandler != NULL))
		{
			(pThis->*(pfHandler)) (dwNotifyCode, wParam, lParam);
			return;
		}

		//WARNING. no handler found. ???
		ASSERT(FALSE);
	}

protected:

#if (_MSC_VER <= 1200) // Using Visual C++ 5.0, 6.0
	CXTPNotifyMapCodeToHandler<T_pfHandler> m_mapHandlers;
#else
	CMap<XTP_NOTIFY_CODE, XTP_NOTIFY_CODE, T_pfHandler, T_pfHandler> m_mapHandlers;
#endif
};


class CXTPNotifySinkImplMTMsgWnd : public CWnd

{
public:
	CXTPNotifySinkImplMTMsgWnd();
	virtual ~CXTPNotifySinkImplMTMsgWnd();

	virtual afx_msg LRESULT OnInterThreadEvent(WPARAM pEventData, LPARAM reserved) = 0;

	virtual BOOL CreateWnd();

	DECLARE_MESSAGE_MAP()
};

//=== Multithreaded sink implementation =====================================
struct XTP_INTER_THREAD_EVENT_DATA
{
	XTP_NOTIFY_CODE dwNotifyCode;
	WPARAM wParam;
	LPARAM lParam;
	DWORD dwFlags;
};

template<class ownerClassName, class _CInformator >
class CXTPNotifySinkImplMT :
			public CXTPNotifySinkImpl< ownerClassName, _CInformator >,
			protected CXTPNotifySinkImplMTMsgWnd
{
public:
	typedef CXTPNotifySinkImpl<ownerClassName, _CInformator> TBaseSink;

	CXTPNotifySinkImplMT(BOOL bInitInternal = TRUE)
	{
		m_dwTraceFlag0 = 0;
		m_dwNexDataID = 0;
		m_bWndCreated = FALSE;
		m_dwOwnerThreadID = ::GetCurrentThreadId();

		if (bInitInternal) {
			m_bWndCreated = CreateWnd();
		}

		m_PostedEvents.InitHashTable(199, FALSE);
	}

	virtual ~CXTPNotifySinkImplMT()
	{
		UnadviseAll();

		DestroyWindow();
		m_bWndCreated = FALSE;
	}


	virtual void OnEvent(XTP_NOTIFY_CODE dwNotifyCode, WPARAM wParam,
						 LPARAM lParam, DWORD dwFlags)
	{
		ASSERT(m_bWndCreated);

		if (dwFlags & xtpNotifyDirectCallForOneThread)
		{
			if (GetCurrentThreadId() == m_dwOwnerThreadID)
			{
				TBaseSink::OnEvent(dwNotifyCode, wParam, lParam, dwFlags);
				return;
			}
		}

		//====================================================================
		XTP_INTER_THREAD_EVENT_DATA ithData = {dwNotifyCode, wParam, lParam, dwFlags};

		if (dwFlags & xtpNotifyPostMessage)
		{
			CSingleLock singleLock(&m_DataCS, TRUE);

#ifdef _DEBUG
			XTP_INTER_THREAD_EVENT_DATA ithDataTmp;
			ASSERT(m_PostedEvents.Lookup(m_dwNexDataID, ithDataTmp) == FALSE);
#endif

			DWORD dwDataID = m_dwNexDataID;
			m_dwNexDataID++;


			BOOL bTrace1 = FALSE;
			BOOL bPosted = FALSE;
			do
			{
				bPosted = PostMessage(xtp_wm_NotificationSinkMTOnEvent, NULL, dwDataID);
				if (!bPosted && (dwFlags & xtpNotifyGuarantyPost))
				{
					if (!bTrace1) {
						TRACE(_T("WARNING!  CXTPNotifySinkImplMT: PostMessage return FALSE. WAIT and retry. (ThreadID = %x, dataID = %d) \n"),
							 GetCurrentThreadId(),   dwDataID);
						bTrace1 = TRUE;
					}

					singleLock.Unlock();
					Sleep(100);
					singleLock.Lock();
				}
			}
			while (!bPosted && (dwFlags & xtpNotifyGuarantyPost));

			if (bPosted)
			{
				m_PostedEvents[dwDataID] = ithData;

				if (bTrace1) {
					TRACE(_T("WARNING.*  CXTPNotifySinkImplMT: Event is posted! (ThreadID = %x, dataID = %d) \n"),
							GetCurrentThreadId(),   dwDataID);
				}

				m_dwTraceFlag0 = 0;
			}
			else if (!(dwFlags & xtpNotifyGuarantyPost))
			{
				if (m_dwTraceFlag0 < 3)
				{
					TRACE(_T("WARNING!  CXTPNotifySinkImplMT: PostMessage return FALSE - Event is skipped! (ThreadID = %x, dataID = %d) \n"),
							GetCurrentThreadId(),   dwDataID);
					TRACE(_T("          Use xtpNotifyGuarantyPost flag for important events. \n"));
				}
				m_dwTraceFlag0++;
			}
		}
		else
		{
			SendMessage(xtp_wm_NotificationSinkMTOnEvent, (WPARAM)&ithData, 0);
		}
	};

	virtual LRESULT OnInterThreadEvent(WPARAM pEventData, LPARAM dwPostDataID)
	{
		XTP_INTER_THREAD_EVENT_DATA* pIthData = (XTP_INTER_THREAD_EVENT_DATA*)pEventData;

		if (pIthData)
		{
			TBaseSink::OnEvent(pIthData->dwNotifyCode, pIthData->wParam,
								pIthData->lParam, pIthData->dwFlags);
		}
		else
		{
			XTP_INTER_THREAD_EVENT_DATA ithData;

			CSingleLock singleLock(&m_DataCS, TRUE);

			if (m_PostedEvents.Lookup((DWORD)dwPostDataID, ithData))
			{
				m_PostedEvents.RemoveKey((DWORD)dwPostDataID);

				singleLock.Unlock();

				TBaseSink::OnEvent(ithData.dwNotifyCode, ithData.wParam,
									ithData.lParam, ithData.dwFlags);
			}
			else
			{
				ASSERT(FALSE);
			}
		}

		//====================================================================
		return 0;
	};

protected:
	CMap<DWORD, DWORD, XTP_INTER_THREAD_EVENT_DATA, XTP_INTER_THREAD_EVENT_DATA&> m_PostedEvents;
	DWORD               m_dwNexDataID;

	BOOL                m_bWndCreated;
	DWORD               m_dwOwnerThreadID;
	CCriticalSection    m_DataCS;
private:
	DWORD m_dwTraceFlag0;
};
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
#define DECLARE_XTP_SINKEX(ownerClassName, MemberName, _CSinkClass) \
	class C_##MemberName##_Informator \
	{ \
	public: \
		static ownerClassName* GetPThis(BYTE* pSink) { \
			ownerClassName* pThis = \
			((ownerClassName*)((BYTE*)pSink - offsetof(ownerClassName, MemberName))); \
			return pThis; \
		}; \
	}; \
	friend class C_##MemberName##_Informator; \
	typedef _CSinkClass<ownerClassName, C_##MemberName##_Informator> T_##MemberName; \
	T_##MemberName MemberName;

//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Parameters:
//     ownerClassName - Name of the class where DECLARE_XTPSINK macro
//                      is placed.
//     MemberName     - Name of the sink class member.
// Summary:
//     This macro is used as implementation of Sink object in
//     the events receiver class.
// Example: class CMyObjectEventsReceiver : public CXTPCmdTarget
//          {
//          protected:
//              // Declare sink object
//              DECLARE_XTPSINK(CMyObjectEventsReceiver, m_Sink)
//
//              // Declare Event handlers methods
//              void NotificationHandler1(XTP_NOTIFY_CODE Event,
//                                          WPARAM wParam , LPARAM lParam);
//              void NotificationHandler2(XTP_NOTIFY_CODE Event,
//                                          WPARAM wParam , LPARAM lParam);
//              // ...
//          };
// See Also: CXTPNotifySink overview,
//           CXTPNotifyConnection overview,
//===========================================================================
#define DECLARE_XTP_SINK(ownerClassName, MemberName) \
			DECLARE_XTP_SINKEX(ownerClassName, MemberName, CXTPNotifySinkImpl) \
			friend class CXTPNotifySinkImpl<ownerClassName, C_##MemberName##_Informator>;

// <COMBINE DECLARE_XTP_SINK>
#define DECLARE_XTP_SINK_MT(ownerClassName, MemberName) \
			DECLARE_XTP_SINKEX(ownerClassName, MemberName, CXTPNotifySinkImplMT) \
			friend class CXTPNotifySinkImplMT<ownerClassName, C_##MemberName##_Informator>; \
			friend class CXTPNotifySinkImpl<ownerClassName, C_##MemberName##_Informator>;


#endif // !defined(_XTPNOTIFYCONNECTION_H__)
