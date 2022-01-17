// XTPTopLevelWndMsgNotifier.h: interface for the CXTPTopLevelWndMsgNotifier class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(_XTP_TOP_LEVEL_WND_MSG_NOTIFIER_H__)
#define _XTP_TOP_LEVEL_WND_MSG_NOTIFIER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CXTPTopLevelWndMsgNotifier window

//{{AFX_CODEJOCK_PRIVATE
class CXTPTopLevelWndMsgNotifier;
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------
// Summary:
//      This global function is used to get library static common top-level
//      window object to catch windows messages, which system sends to
//      top-level windows (like WM_TIMECHANGE, WM_SYSCOLORCHANGE).
// Remarks:
//      Using CXTPTopLevelWndMsgNotifier you can advise your non-top-level
//      (child) windows to receive these messages.
// Returns:
//      A pointer to CXTPTopLevelWndMsgNotifier object.
//-----------------------------------------------------------------------
CXTPTopLevelWndMsgNotifier* XTPGetTopLevelWndMsgNotifier();

//===========================================================================
// Summary:
//      This class is used to catch windows messages, which system sends to
//      top-level windows and to send them to subscribers windows.
//      The following messages are supported:
//          <b>WM_TIMECHANGE</b>
//          <b>WM_SYSCOLORCHANGE</b>
// See Also:
//      WM_TIMECHANGE, WM_SYSCOLORCHANGE.
//===========================================================================
class _XTP_EXT_CLASS CXTPTopLevelWndMsgNotifier : public CWnd
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPTopLevelWndMsgNotifier();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPTopLevelWndMsgNotifier();

	//-----------------------------------------------------------------------
	// Summary:
	//     This enum contains additional parameter value(s).
	//-----------------------------------------------------------------------
	enum EnumParams
	{
		msgAll = 0      // This value means all messages or any message.
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Advice CWnd object to receive specified top-level windows message(s).
	// Parameters:
	//      pWnd    - [in] A pointer to client window.
	//      uWndMsg - [in] Windows message ID.
	// Remarks:
	//      If <b>uWndMsg</b> is <b>msgAll</b> than all supported messages
	//      will be sent to client window.
	// Se Also:
	//      Unadvise.
	//-----------------------------------------------------------------------
	void Advise(CWnd* pWnd, UINT uWndMsg = msgAll);

	//-----------------------------------------------------------------------
	// Summary:
	//     Unadvise CWnd object from receive specified top-level windows message(s).
	// Parameters:
	//      pWnd    - [in] A pointer to client window.
	//      uWndMsg - [in] Windows message ID.
	// Remarks:
	//      If <b>uWndMsg</b> is <b>msgAll</b> unadvised from all supported
	//      messages.
	// Se Also:
	//      Advise.
	//-----------------------------------------------------------------------
	void Unadvise(CWnd* pWnd, UINT uWndMsg = msgAll);

protected:
	//{{AFX_CODEJOCK_PRIVATE
	friend CXTPTopLevelWndMsgNotifier* XTPGetTopLevelWndMsgNotifier();
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to Create a CWnd object.
	// Remarks:
	//      A simple top-level window will be created.
	// Returns:
	//      TRUE is successful< FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL CreateWnd();

	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimeChange();
	afx_msg void OnSysColorChange();
	afx_msg void OnTimer(UINT_PTR uTimerID);
	//}}AFX_CODEJOCK_PRIVATE
private:

	struct CLIENT_INFO {
		CWnd* m_pWnd;
		UINT m_uWndMsg;
	};

	class CClientsArray : public CArray<CLIENT_INFO, CLIENT_INFO&>
	{
	public:
		CClientsArray();
		virtual ~CClientsArray();

		int FindExact(CWnd* pWnd, UINT uWndMsg = CXTPTopLevelWndMsgNotifier::msgAll);
		int FindClient(CWnd* pWnd, UINT uWndMsg = CXTPTopLevelWndMsgNotifier::msgAll);
		void Remove(CWnd* pWnd, UINT uWndMsg = CXTPTopLevelWndMsgNotifier::msgAll);
	};

	CClientsArray   m_arClients;
	SYSTEMTIME      m_timeLastDate;

	void SendToClients(UINT uWndMsg, WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_XTP_TOP_LEVEL_WND_MSG_NOTIFIER_H__)
