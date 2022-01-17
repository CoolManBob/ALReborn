// XTPHookManager.h : interface for the CXTPHookManager class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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
#if !defined(__XTPHOOKMANAGER_H__)
#define __XTPHOOKMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPCommandBarsDefines.h"


//===========================================================================
// Summary:
//     HookAble interface.
// See Also: CXTPHookManager, CXTPHookManager::SetHook
//===========================================================================
class _XTP_EXT_CLASS CXTPHookManagerHookAble
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPHookManagerHookAble object
	//-------------------------------------------------------------------------
	CXTPHookManagerHookAble();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPHookManagerHookAble object, handles clean up and deallocation
	//-------------------------------------------------------------------------
	virtual ~CXTPHookManagerHookAble();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by WindowProc, or is called during message reflection.
	// Parameters:
	//     hWnd - Window handle that the message belongs to.
	//     nMessage - Specifies the message to be sent.
	//     wParam - Specifies additional message-dependent information.
	//     lParam - Specifies additional message-dependent information.
	//     lResult - The return value of WindowProc. Depends on the message; may be NULL.
	// Returns:
	//     TRUE if message was processed.
	//-----------------------------------------------------------------------
	virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult) = 0;

public:
	BOOL m_bAutoDestroy;            // TRUE to automatically delete hook
};


//===========================================================================
// Summary:
//     CXTPHookManager is standalone class. It is used to hook a CWnd object
//     in order to intercept and act upon window messages that are received.
// Remarks:
//     To access CXTPHookManager methods use XTPHookManager function
// See Also:
//     XTPHookManager, CXTPHookManagerHookAble
//===========================================================================
class _XTP_EXT_CLASS CXTPHookManager
{
private:
	class CHookSink;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPHookManager object.
	//-----------------------------------------------------------------------
	CXTPHookManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPHookManager object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	~CXTPHookManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will hook a window so that its messages are
	//     intercepted before they are passed on to the specified window.
	// Parameters:
	//     hWnd  - A handle to a window that represents that represents the window to hook.
	//     pHook - Hookable class that will receive messages
	//-----------------------------------------------------------------------
	void SetHook(HWND hWnd, CXTPHookManagerHookAble* pHook);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes a hook associated with a window.
	// Parameters:
	//     hWnd  - A handle to a window that hooks need to remove
	//     pHook - Hookable class that hooks need to remove
	//-----------------------------------------------------------------------
	void RemoveHook(HWND hWnd, CXTPHookManagerHookAble* pHook);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all hooks associated with a window.
	// Parameters:
	//     hWnd  - A handle to a window that hooks need to remove
	//     pHook - Hookable class that hooks need to remove
	//-----------------------------------------------------------------------
	void RemoveAll(HWND hWnd);
	void RemoveAll(CXTPHookManagerHookAble* pHook); //<combine CXTPHookManager::RemoveAll@HWND>

	//-----------------------------------------------------------------------
	// Summary:
	//     Searches collection of Hookable interfaces that receive hooks of specified window.
	// Parameters:
	//     hWnd  - A handle to a window need to test
	// Returns:
	//     CHookSink pointer if found; otherwise returns NULL;
	//-----------------------------------------------------------------------
	CHookSink* Lookup(HWND hWnd);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Calls the default window procedure.
	//-----------------------------------------------------------------------
	LRESULT Default();
	LRESULT Default(WPARAM wParam, LPARAM lParam); // combine CXTPHookManager::Default>

private:
	static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void RemoveAll();
	CMap<HWND, HWND, LPVOID, LPVOID> m_mapHooks;

	friend _XTP_EXT_CLASS CXTPHookManager* AFX_CDECL XTPHookManager();
	friend class CHookSink;
};


//---------------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPHookManager members.
//     Since this class is designed as a single instance object you can
//     only access version info through this method. You <b>cannot</b>
//     directly instantiate an object of type CXTPHookManager.
// Example:
//     <code>XTPHookManager()->SetHook(hWnd, this);</code>
//---------------------------------------------------------------------------
_XTP_EXT_CLASS CXTPHookManager* AFX_CDECL XTPHookManager();



//===========================================================================
// Summary:
//     CXTPKeyboardManager class used to handle keyboard hooks
//===========================================================================
class _XTP_EXT_CLASS CXTPKeyboardManager : public CNoTrackObject
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPKeyboardManager object.
	//-----------------------------------------------------------------------
	CXTPKeyboardManager();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPKeyboardManager object, handles clean up and deallocation
	//-------------------------------------------------------------------------
	~CXTPKeyboardManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to start/end keyboard hooks
	// Parameters:
	//     bSetup - TRUE to start keyboard hooks
	// See Also: SetupCallWndProcHook
	//-----------------------------------------------------------------------
	void SetupKeyboardHook(BOOL bSetup = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to start/end message hooks
	// Parameters:
	//     bSetup - TRUE to start message hooks
	// See Also: SetupKeyboardHook
	//-----------------------------------------------------------------------
	void SetupCallWndProcHook(BOOL bSetup = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to hook keyboard notifications
	// Parameters:
	//     pHook - CXTPHookManagerHookAble object that will receive keyboard notifications
	// See Also: UnhookKeyboard
	//-----------------------------------------------------------------------
	void HookKeyboard(CXTPHookManagerHookAble* pHook);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this method to unhook keyboard notifications
	// Parameters:
	//      pHook - CXTPHookManagerHookAble object to remove
	// See Also: HookKeyboard
	//-----------------------------------------------------------------------
	void UnhookKeyboard(CXTPHookManagerHookAble* pHook);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if keyboard is hooked
	// Returns:
	//     TRUE if keyboard is hooked
	//-----------------------------------------------------------------------
	BOOL IsKeyboardHooked() { return m_lstKeyboardHooks.GetCount() > 0; }

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to process keyboard hook
	// Parameters:
	//     nMessage - Message to process
	//     wParam - Message specified parameter
	//     lParam - Message specified parameter
	// Returns:
	//     TRUE if message was processed
	//-----------------------------------------------------------------------
	BOOL ProcessKeyboardHooks(UINT nMessage, WPARAM wParam, LPARAM lParam = 0);

protected:
//{{AFX_CODEJOCK_PRIVATE
	static LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK CallWndProc(int code, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK GetMessageProc(int code, WPARAM wParam, LPARAM lParam);

//}}AFX_CODEJOCK_PRIVATE

protected:
	HHOOK m_hHookKeyboard;          // Keyboard hook
	HHOOK m_hHookCallWndProc;       // Message hook
	HHOOK m_hHookGetMessage;       // Message hook

#ifdef _AFXDLL
	AFX_MODULE_STATE* m_pModuleState; // Module state
#endif

	static CThreadLocal<CXTPKeyboardManager> _xtpKeyboardThreadState;           // Instance of Keyboard hook
	CList<CXTPHookManagerHookAble*, CXTPHookManagerHookAble*> m_lstKeyboardHooks;   // List of keyboard hooks

private:
	friend CXTPKeyboardManager* XTPKeyboardManager();
	friend class CXTPCommandBars;

};


AFX_INLINE CXTPKeyboardManager* XTPKeyboardManager() {
	return CXTPKeyboardManager::_xtpKeyboardThreadState.GetData();
}


#endif //#if !defined(__XTPHOOKMANAGER_H__)
