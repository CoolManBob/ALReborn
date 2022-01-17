// XTPHookManager.cpp : implementation of the CXTPHookManager class.
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

#include "stdafx.h"

#include "XTPToolBar.h"
#include "XTPHookManager.h"
#include "XTPMouseManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CXTPHookManagerHookAble::CXTPHookManagerHookAble()
{
	m_bAutoDestroy = FALSE;
}

CXTPHookManagerHookAble::~CXTPHookManagerHookAble()
{

}



class CXTPHookManager::CHookSink : public CArray<CXTPHookManagerHookAble*, CXTPHookManagerHookAble*>
{
	friend class CXTPHookManager;

public:
	CHookSink(HWND hWnd);

	~CHookSink();

public:
	int Find(CXTPHookManagerHookAble* pItem)
	{
		for (int i = 0; i < GetSize(); i++)
		{
			if (pItem == GetAt(i))
				return  i;
		}
		return -1;
	}

	void RemoveHook(CXTPHookManagerHookAble* pHook)
	{
		int nIndex = Find(pHook);
		if (nIndex != -1)
		{
			RemoveAt(nIndex);

			if (pHook->m_bAutoDestroy)
				delete pHook;

			if (GetSize() == 0)
			{
				XTPHookManager()->m_mapHooks.RemoveKey(m_hWnd);
				delete this;
			}
		}
	}

public:
	BOOL OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

protected:
	WNDPROC m_pOldWndProc;
	HWND m_hWnd;
#ifdef _AFXDLL
	AFX_MODULE_STATE* m_pModuleState;
#endif
};


CXTPHookManager::CHookSink::CHookSink(HWND hWnd)
{
#ifdef _AFXDLL
	m_pModuleState = 0;
#endif

	m_hWnd = hWnd;
	m_pOldWndProc = (WNDPROC)GetWindowLongPtr(
		hWnd, GWLP_WNDPROC);
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)HookWndProc);
}

CXTPHookManager::CHookSink::~CHookSink()
{
	SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_pOldWndProc);

	for (int i = (int)GetSize() - 1; i >= 0; i--)
	{
		CXTPHookManagerHookAble* pHookAble = GetAt(i);
		if (pHookAble->m_bAutoDestroy)
		{
			delete pHookAble;
		}
	}
}

BOOL CXTPHookManager::CHookSink::OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult)
{
	int nCount = (int)GetSize();

	for (int i = nCount - 1; i >= 0; i--)
	{
		int nResult = GetAt(i)->OnHookMessage(hWnd, nMessage, wParam, lParam, lResult);

		if (nResult == TRUE)
			return TRUE;
		if (nResult == FALSE_EXIT)
			return FALSE;
	}
	return FALSE;
}


CXTPHookManager::CXTPHookManager()
{
}

CXTPHookManager* AFX_CDECL XTPHookManager()
{
	static CXTPHookManager s_managerInstance;
	return &s_managerInstance;
}

CXTPHookManager::~CXTPHookManager()
{
	RemoveAll();
}


LRESULT CXTPHookManager::Default(WPARAM wParam, LPARAM lParam)
{
	MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;

	CHookSink* pSink = Lookup(curMsg.hwnd);
	ASSERT(pSink);
	if (!pSink)
		return 0;

	WNDPROC wndProc = pSink->m_pOldWndProc;

	LRESULT lResult = ::CallWindowProc(wndProc, curMsg.hwnd, curMsg.message, wParam, lParam);

	return lResult;
}

LRESULT CXTPHookManager::Default()
{
	MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;
	return Default(curMsg.wParam, curMsg.lParam);;
}

LRESULT CALLBACK CXTPHookManager::HookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;
	MSG  oldMsg = curMsg;
	curMsg.hwnd = hWnd;
	curMsg.message = message;
	curMsg.wParam = wParam;
	curMsg.lParam = lParam;

	// Get hook object for this window. Get from hook map
	CHookSink* pSink = XTPHookManager()->Lookup(hWnd);
	ASSERT(pSink);
	if (!pSink)
		return 0;

	SAFE_MANAGE_STATE(pSink->m_pModuleState);

	LRESULT lResult = 0;
	WNDPROC wndProc = pSink->m_pOldWndProc;

	// Window is being destroyed: unhook all hooks (for this window)
	// and pass message to orginal window proc
	if (message == WM_NCDESTROY)
	{
		XTPHookManager()->RemoveAll(hWnd);
	}
	// pass to message hook
	else
	{
		if (pSink->OnHookMessage(hWnd, message, wParam, lParam, lResult))
		{
			curMsg = oldMsg;
			return lResult;
		}
	}
	lResult = ::CallWindowProc(wndProc, hWnd, message, wParam, lParam);

	curMsg = oldMsg;
	return lResult;
}

void CXTPHookManager::SetHook(HWND hWnd, CXTPHookManagerHookAble* pHook)
{
	CHookSink* pSink = Lookup(hWnd);

	if (pSink)
	{
		if (pSink->Find(pHook) == -1)
		{
			pSink->Add(pHook);
		}

	}
	else
	{
		pSink = new CHookSink(hWnd);
		pSink->Add(pHook);

		m_mapHooks[hWnd] = pSink;
	}

#ifdef _AFXDLL
	pSink->m_pModuleState = AfxGetModuleState();
#endif


}
void CXTPHookManager::RemoveAll()
{
	HWND hWnd;
	POSITION pos = m_mapHooks.GetStartPosition();
	CHookSink* pSink;

	while (pos)
	{
		m_mapHooks.GetNextAssoc(pos, hWnd, (void*&)pSink);
		delete pSink;
	}
	m_mapHooks.RemoveAll();
}

void CXTPHookManager::RemoveAll(HWND hWnd)
{
	CHookSink* pSink = Lookup(hWnd);
	if (pSink)
	{
		m_mapHooks.RemoveKey(hWnd);
		delete pSink;
	}
}

void CXTPHookManager::RemoveAll(CXTPHookManagerHookAble* pHook)
{
	ASSERT(pHook);
	HWND hWnd;
	CHookSink* pSink = NULL;

	POSITION pos = m_mapHooks.GetStartPosition();
	while (pos)
	{
		m_mapHooks.GetNextAssoc(pos, hWnd, (void*&)pSink);
		pSink->RemoveHook(pHook);
	}
}

CXTPHookManager::CHookSink* CXTPHookManager::Lookup(HWND hWnd)
{
	CHookSink* pSink;

	if (m_mapHooks.Lookup(hWnd, (void*&)pSink))
		return pSink;
	return NULL;
}

void CXTPHookManager::RemoveHook(HWND hWnd, CXTPHookManagerHookAble* pHook)
{
	ASSERT(hWnd);
	CHookSink* pSink = Lookup(hWnd);

	if (pSink)
	{
		pSink->RemoveHook(pHook);
	}
}


//////////////////////////////////////////////////////////////////////////
//

CThreadLocal<CXTPKeyboardManager> CXTPKeyboardManager::_xtpKeyboardThreadState;


CXTPKeyboardManager::CXTPKeyboardManager()
{
	m_hHookKeyboard = 0;
	m_hHookCallWndProc = 0;
	m_hHookGetMessage = 0;

#ifdef _AFXDLL
	m_pModuleState = 0;
#endif
}

void CXTPKeyboardManager::SetupKeyboardHook(BOOL bSetup)
{
	if (bSetup && m_hHookKeyboard == 0)
	{
		m_hHookKeyboard = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, AfxGetInstanceHandle(), GetCurrentThreadId());
	}
	if (!bSetup && m_hHookKeyboard)
	{
		UnhookWindowsHookEx(m_hHookKeyboard);
		m_hHookKeyboard = 0;
	}
}


void CXTPKeyboardManager::SetupCallWndProcHook(BOOL bSetup)
{
	if (bSetup && m_hHookCallWndProc == 0)
	{
		m_hHookCallWndProc = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, AfxGetInstanceHandle(), GetCurrentThreadId());
	}
	if (!bSetup && m_hHookCallWndProc)
	{
		UnhookWindowsHookEx(m_hHookCallWndProc);
		m_hHookCallWndProc = 0;
	}
}

CXTPKeyboardManager::~CXTPKeyboardManager()
{
	SetupKeyboardHook(FALSE);
	SetupCallWndProcHook(FALSE);
}

void CXTPKeyboardManager::HookKeyboard(CXTPHookManagerHookAble* pHook)
{
	SetupKeyboardHook(TRUE);
	SetupCallWndProcHook(TRUE);

#ifdef _AFXDLL
	m_pModuleState = AfxGetModuleState();
#endif

	if (m_lstKeyboardHooks.Find(pHook) == NULL)
	{
		m_lstKeyboardHooks.AddTail(pHook);
	}

}
void CXTPKeyboardManager::UnhookKeyboard(CXTPHookManagerHookAble* pHook)
{
	POSITION pos = m_lstKeyboardHooks.Find(pHook);

	if (pos != NULL)
	{
		m_lstKeyboardHooks.RemoveAt(pos);
	}

	if (m_lstKeyboardHooks.IsEmpty())
	{
		SetupCallWndProcHook(FALSE);
		SetupKeyboardHook(FALSE);
	}

}
BOOL CXTPKeyboardManager::ProcessKeyboardHooks(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	POSITION pos = m_lstKeyboardHooks.GetTailPosition();
	while (pos)
	{
		CXTPHookManagerHookAble* pHook = m_lstKeyboardHooks.GetPrev(pos);
		LRESULT lResult = 0;
		BOOL bResult = pHook->OnHookMessage(0, nMessage, wParam, lParam, lResult);
		if (bResult)
		{
			return bResult != FALSE_EXIT;
		}
	}
	return TRUE;
}


LRESULT CALLBACK CXTPKeyboardManager::CallWndProc(int code, WPARAM wParam, LPARAM lParam)
{
	CXTPKeyboardManager* pKeyboardManager = XTPKeyboardManager();

	CWPSTRUCT* pMSG = (CWPSTRUCT*)lParam;

	if (pMSG && pMSG->message == WM_ACTIVATEAPP && !pMSG->wParam)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(pKeyboardManager->m_pModuleState);
#endif

		XTPMouseManager()->SendTrackLost();
	}

	return CallNextHookEx(pKeyboardManager->m_hHookCallWndProc, code, wParam, lParam);
}


LRESULT CALLBACK CXTPKeyboardManager::KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	CXTPKeyboardManager* pKeyboardManager = XTPKeyboardManager();

	if (code != HC_ACTION)
		return CallNextHookEx(pKeyboardManager->m_hHookKeyboard, code, wParam, lParam);

	if (pKeyboardManager->IsKeyboardHooked() && (!(HIWORD(lParam) & KF_UP)))
	{
		SAFE_MANAGE_STATE(pKeyboardManager->m_pModuleState);

		if (!(HIWORD(lParam) & KF_ALTDOWN))
		{
			if (pKeyboardManager->ProcessKeyboardHooks(WM_KEYDOWN, wParam, lParam))
			{
				return TRUE;
			}
		}
		else
		{
			if (wParam == VK_MENU && !XTPMouseManager()->IsMouseLocked())
			{
				if (!(HIWORD(lParam) & KF_REPEAT))
				{
					XTPMouseManager()->SendTrackLost();
					return TRUE;
				}
			}
			else
			{
				if (pKeyboardManager->ProcessKeyboardHooks(WM_KEYDOWN, wParam, lParam))
				{
					return TRUE;
				}
			}
		}
	}


	return CallNextHookEx(pKeyboardManager->m_hHookKeyboard, code, wParam, lParam);

}

