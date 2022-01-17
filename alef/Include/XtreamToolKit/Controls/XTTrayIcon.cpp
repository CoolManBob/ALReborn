// XTTrayIcon.cpp : implementation of the CXTTrayIcon class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPSystemHelpers.h"

#include "XTDefines.h"
#include "XTUtil.h"
#include "XTTrayIcon.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

const UINT XT_WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

#ifndef IDANI_CAPTION
#define IDANI_CAPTION               3
#endif//IDANI_CAPTION

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon

CXTTrayIcon::CXTTrayIcon()
{
	SetDefaultValues();
}

CXTTrayIcon::~CXTTrayIcon()
{
	RemoveIcon();
	DestroyWindow();
	RemoveAnimationIcons();
}

BEGIN_MESSAGE_MAP(CXTTrayIcon, CWnd)
	//{{AFX_MSG_MAP(CXTTrayIcon)
	ON_WM_TIMER()
	ON_WM_SETTINGCHANGE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(XT_WM_TASKBARCREATED, OnTaskbarCreated)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon initialization

void CXTTrayIcon::SetDefaultValues()
{
	::ZeroMemory(&m_niData, sizeof(NOTIFYICONDATAEX));

	// Default initialization for the NOTIFYICONDATA base struct.

	m_niData.cbSize = sizeof(NOTIFYICONDATAEX);
	m_niData.hWnd = NULL;
	m_niData.uID = 0;
	m_niData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_niData.uCallbackMessage = TIN_XT_TRAYICON;
	m_niData.hIcon = NULL;
	m_niData.szTip[0] = 0;

	// Initialize the remaining member data.

	m_iMaxTipSize = 64;
	m_nIconID = 0;
	m_strToolTip = _T("");
	m_nIDEvent = 1001;
	m_nCounter = 0;
	m_uDefMenuItemID = 0;
	m_bDefMenuItemByPos = true;
	m_bHidden = true;
	m_bRemoved = true;
	m_bShowPending = false;
	m_hWndNotify = NULL;
	m_hIcon = NULL;
}

BOOL CXTTrayIcon::ShellNotify(DWORD dwMessage)
{
	return ::Shell_NotifyIcon(dwMessage, (PNOTIFYICONDATA)&m_niData);
}

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon notification window initialization

bool CXTTrayIcon::SetNotificationWnd(CWnd* pWndNotify)
{
	ASSERT_VALID(pWndNotify);

	// Make sure Notification window is valid
	if (!pWndNotify || !::IsWindow(pWndNotify->GetSafeHwnd()))
	{
		return false;
	}

	// assign values
	m_niData.hWnd = pWndNotify->GetSafeHwnd();
	m_niData.uFlags = 0;

	if (!ShellNotify(NIM_MODIFY))
	{
		return false;
	}

	return true;
}

CWnd* CXTTrayIcon::GetNotificationWnd()
{
	return CWnd::FromHandle(m_niData.hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon notification message handling

bool CXTTrayIcon::SetCallbackMessage(UINT uNewCallbackMessage)
{
	// Make sure we avoid conflict with other messages
	ASSERT(m_niData.uCallbackMessage >= WM_APP);

	m_niData.uCallbackMessage = uNewCallbackMessage;
	m_niData.uFlags = NIF_MESSAGE;

	if (!ShellNotify(NIM_MODIFY))
	{
		return false;
	}

	return true;
}

UINT CXTTrayIcon::GetCallbackMessage()
{
	return m_niData.uCallbackMessage;
}

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon icon initialization

bool CXTTrayIcon::SetIcon(HICON hIcon)
{
	// check to see if this icon was already set.
	if (m_niData.hIcon == hIcon)
	{
		return true;
	}

	m_niData.uFlags = NIF_ICON;
	m_niData.hIcon = hIcon;

	if (m_bHidden)
	{
		return true;
	}

	if (!ShellNotify(NIM_MODIFY))
	{
		return false;
	}

	return true;
}

bool CXTTrayIcon::SetIcon(LPCTSTR lpszIconName)
{
	int cx = ::GetSystemMetrics(SM_CXSMICON);
	int cy = ::GetSystemMetrics(SM_CYSMICON);

	m_hIcon = (HICON)::LoadImage(AfxGetResourceHandle(),
		lpszIconName, IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);

	return SetIcon(m_hIcon);
}

bool CXTTrayIcon::SetIcon(UINT nIDResource)
{
	return SetIcon(MAKEINTRESOURCE(nIDResource));
}

void CXTTrayIcon::RemoveAnimationIcons()
{
	while (!m_arTrayIcons.IsEmpty())
	{
		TRAYICONDATA ti = m_arTrayIcons.RemoveHead();
		::DestroyIcon(ti.hIcon);
	}
}

bool CXTTrayIcon::SetAnimationIcons(const UINT* lpIDArray, int nIDCount, const CString* lpStrTipArray/*= NULL*/)
{
	RemoveAnimationIcons();

	int cx = ::GetSystemMetrics(SM_CXSMICON);
	int cy = ::GetSystemMetrics(SM_CYSMICON);

	int iIcon;
	for (iIcon = 0; iIcon < nIDCount; ++iIcon)
	{
		TRAYICONDATA ti;

		if (lpStrTipArray != NULL)
			ti.strToolTip = lpStrTipArray[ iIcon ];

		ti.hIcon = (HICON)::LoadImage(AfxGetResourceHandle(),
			MAKEINTRESOURCE(lpIDArray[ iIcon ]), IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);

		if (ti.hIcon == NULL)
		{
			return false;
		}

		m_arTrayIcons.AddTail(ti);
	}

	return true;
}

HICON CXTTrayIcon::GetIcon() const
{
	return m_niData.hIcon;
}

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon tooltip initialization



bool CXTTrayIcon::SetTooltipText(LPCTSTR lpszTipText)
{
	m_strToolTip = lpszTipText;

	if (m_strToolTip.GetLength() >= (int)m_iMaxTipSize)
		m_strToolTip = m_strToolTip.Left((int)m_iMaxTipSize-1);

	return SetShellTooltip(m_strToolTip);
}

bool CXTTrayIcon::SetShellTooltip(LPCTSTR lpszTipText)
{
	ASSERT(AfxIsValidString(lpszTipText));
	ASSERT(_tcslen(lpszTipText) < m_iMaxTipSize);

	m_niData.uFlags = NIF_TIP;
	STRNCPY_S(m_niData.szTip, _countof(m_niData.szTip), lpszTipText, m_iMaxTipSize-1);

	if (m_bHidden)
	{
		return true;
	}

	if (!ShellNotify(NIM_MODIFY))
	{
		return false;
	}

	return true;
}

bool CXTTrayIcon::SetTooltipText(UINT nTipText)
{
	CString strTipText;
	if (!strTipText.LoadString(nTipText))
	{
		return false;
	}

	return SetTooltipText(strTipText);
}

CString CXTTrayIcon::GetTooltipText() const
{
	return m_strToolTip;
}

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon manipulation

bool CXTTrayIcon::AddIcon()
{
	if (!m_bRemoved)
	{
		RemoveIcon();
	}

	m_niData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	if (!ShellNotify(NIM_ADD))
	{
		m_bShowPending = true;
		return false;
	}

	m_bRemoved = m_bHidden = false;

	return true;
}

bool CXTTrayIcon::RemoveIcon()
{
	m_bShowPending = false;

	if (m_bRemoved)
	{
		return true;
	}

	m_niData.uFlags = 0;
	if (!ShellNotify(NIM_DELETE))
	{
		return false;
	}

	m_bRemoved = m_bHidden = true;

	return true;
}

bool CXTTrayIcon::HideIcon()
{
	if (m_bRemoved || m_bHidden)
	{
		return true;
	}

	if (XTOSVersionInfo()->IsWin2KOrGreater())
	{
		m_niData.uFlags = NIF_STATE;
		m_niData.dwState = NIS_HIDDEN;
		m_niData.dwStateMask = NIS_HIDDEN;

		if (!ShellNotify(NIM_MODIFY))
		{
			m_bHidden = false;
		}
		else
		{
			m_bHidden = true;
		}

		return m_bHidden;
	}

	return RemoveIcon();
}

bool CXTTrayIcon::ShowIcon()
{
	if (m_bRemoved)
		return AddIcon();

	if (!m_bHidden)
		return TRUE;

	if (XTOSVersionInfo()->IsWin2KOrGreater())
	{
		m_niData.uFlags = NIF_STATE;
		m_niData.dwState = 0;
		m_niData.dwStateMask = NIS_HIDDEN;

		if (!ShellNotify(NIM_MODIFY))
			return false;

		m_bRemoved = m_bHidden = false;
		return true;
	}

	return AddIcon();
}

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon menu initialization

bool CXTTrayIcon::SetDefaultMenuItem(UINT uItem, bool bByPos)
{
	// if the values already exist, return true.
	if ((m_uDefMenuItemID == uItem) && (m_bDefMenuItemByPos == bByPos))
	{
		return true;
	}

	m_uDefMenuItemID = uItem;
	m_bDefMenuItemByPos = bByPos;

	// verify that we can load the menu defined by uID.
	CMenu menu;
	if (!menu.LoadMenu(m_niData.uID))
	{
		return false;
	}

	// see if we can access the submenu
	CMenu* pSubMenu = menu.GetSubMenu(0);
	if (!pSubMenu)
	{
		return false;
	}

	// check to see if we can set the submenu for the popup.  This is just a check to ensure
	// that everything has been correctly set.
	if (!::SetMenuDefaultItem(pSubMenu->m_hMenu, m_uDefMenuItemID, m_bDefMenuItemByPos))
	{
		return false;
	}

	return true;
}

void CXTTrayIcon::GetDefaultMenuItem(UINT& uItem, bool& bByPos)
{
	uItem = m_uDefMenuItemID;
	bByPos = m_bDefMenuItemByPos;
}

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon message handlers

void CXTTrayIcon::OnTimer(UINT_PTR nIDEvent)
{
	// Update the tray icon and tooltip text.
	if (nIDEvent == m_nIDEvent)
	{
		POSITION pos = m_arTrayIcons.FindIndex(m_nCounter);
		if (pos != NULL)
		{
			TRAYICONDATA& ti = m_arTrayIcons.GetAt(pos);

			if (!ti.strToolTip.IsEmpty())
			{
				SetShellTooltip(ti.strToolTip);
			}
			else
			{
				SetShellTooltip(m_strToolTip);
			}

			if (ti.hIcon != NULL)
			{
				SetIcon(ti.hIcon);
			}
		}

		m_nCounter = (m_nCounter + 1) % (int)m_arTrayIcons.GetCount();
	}
}

bool CXTTrayIcon::Create(
		LPCTSTR lpszCaption,
		CWnd* pParentWnd,
		UINT nIconID,
		UINT uMenuID/*= 0*/,
		UINT uDefMenuItemID/*= 0*/,
		bool bDefMenuItemByPos/*= false*/)
{
	ASSERT(pParentWnd); // must be valid.

	m_nIconID = nIconID;
	m_strToolTip = lpszCaption;
	m_hWndNotify = pParentWnd->GetSafeHwnd();

	m_iMaxTipSize = _countof(m_niData.szTip) - 1;

	// Set the tray icon and tooltip text
	SetIcon(m_nIconID);
	SetTooltipText(m_strToolTip);

	// Create an invisible window
	CWnd::CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP, 0, 0, 0, 0, NULL, 0);

	m_niData.hWnd = m_hWnd;
	m_niData.uID = uMenuID;
	m_uDefMenuItemID = uDefMenuItemID;
	m_bDefMenuItemByPos = bDefMenuItemByPos;

	m_uFlags = m_niData.uFlags;

	return AddIcon();
}

bool CXTTrayIcon::ShowBalloonTip(LPCTSTR lpszInfo, LPCTSTR lpszInfoTitle/*= NULL*/, DWORD dwInfoFlags/*= NIIF_NONE*/, UINT uTimeout/*= 10*/)
{
	bool bResult = false;

	if (XTOSVersionInfo()->IsWin2KOrGreater() && lpszInfo)
	{
		// The balloon tooltip text can be up to 255 chars long.
		ASSERT(AfxIsValidString(lpszInfo));
		ASSERT(lstrlen(lpszInfo) < 256);

		// The balloon title text can be up to 63 chars long.
		if (lpszInfoTitle)
		{
			ASSERT(AfxIsValidString(lpszInfoTitle));
			ASSERT(lstrlen(lpszInfoTitle) < 64);
		}

		// dwInfoFlags must be valid.
		ASSERT(NIIF_NONE == dwInfoFlags ||
				NIIF_INFO == dwInfoFlags ||
				NIIF_WARNING == dwInfoFlags ||
				NIIF_ERROR == dwInfoFlags);

		// The timeout must be between 10 and 30 seconds.
		ASSERT(uTimeout >= 10 && uTimeout <= 30);

		m_niData.uFlags |= NIF_INFO;

		STRNCPY_S(m_niData.szInfo, _countof(m_niData.szInfo), lpszInfo, 255);

		if (lpszInfoTitle)
		{
			STRNCPY_S(m_niData.szInfoTitle, _countof(m_niData.szInfoTitle), lpszInfoTitle, 63);
		}
		else
		{
			m_niData.szInfoTitle[0] = _T('\0');
		}

		m_niData.uTimeout = (uTimeout * 1000); // convert time to millisecs
		m_niData.dwInfoFlags = dwInfoFlags;

		if (ShellNotify(NIM_MODIFY))
		{
			bResult = true;
		}

		// Zero out the balloon text string so that later operations won't redisplay
		// the balloon.
		m_niData.szInfo[0] = _T('\0');
	}

	return bResult;
}

void CXTTrayIcon::StartAnimation(UINT uElapse/*= 500*/)
{
	CWnd::SetTimer(m_nIDEvent, uElapse, NULL);
}

void CXTTrayIcon::StopAnimation()
{
	CWnd::KillTimer(m_nIDEvent);

	SetShellTooltip(m_strToolTip);
	SetIcon(m_nIconID);
}

LRESULT CXTTrayIcon::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	// Return quickly if its not for this tray icon
	if (wParam != m_niData.uID)
	{
		return 0L;
	}

	if (m_hWndNotify == NULL)
	{
		return 0L;
	}

	// Check to see if our notification window has already handled this
	// message, if so then return success.
	if (::SendMessage(m_hWndNotify, TIN_XT_TRAYICON, wParam, lParam))
	{
		return 1;
	}

	switch (LOWORD(lParam))
	{

	// Sent when the balloon is shown (balloons are queued).
	case NIN_BALLOONSHOW:
		break;

	// Sent when the balloon disappears-for example, when the
	// icon is deleted. This message is not sent if the balloon
	// is dismissed because of a timeout or a mouse click.
	case NIN_BALLOONHIDE:
		break;

	// Sent when the balloon is dismissed because of a timeout.
	case NIN_BALLOONTIMEOUT:
		break;

	// Sent when the balloon is dismissed because of a mouse click.
	case NIN_BALLOONUSERCLICK:
		break;

	// The version 5.0 Shell sends the associated application a NIN_SELECT message
	// if a user selects a notify icon with the mouse and activates it with the ENTER key

	case NIN_SELECT:
		// intentional fall thru...

	// The version 5.0 Shell sends the associated application a NIN_KEYSELECT message
	// if a user selects a notify icon with the keyboard and activates it with the space bar or ENTER key

	case NIN_KEYSELECT:
		// intentional fall thru...

	// The version 5.0 Shell sends the associated application a WM_CONTEXTMENU
	// If a user requests a notify icon's shortcut menu with the keyboard
	case WM_CONTEXTMENU:
		// intentional fall thru...

	case WM_RBUTTONUP:
		{
			CMenu menu;
			if (!menu.LoadMenu(m_niData.uID))
			{
				return 0;
			}

			CMenu* pSubMenu = menu.GetSubMenu(0);
			if (pSubMenu == NULL)
			{
				return 0;
			}

			// Make chosen menu item the default (bold font)
			::SetMenuDefaultItem(pSubMenu->m_hMenu,
				m_uDefMenuItemID, m_bDefMenuItemByPos);

			// Display the menu at the current mouse location. There's a "bug"
			// (Microsoft calls it a feature) in Windows 95 that requires calling
			// SetForegroundWindow. To find out more, search for Q135788 in MSDN.
			//
			CPoint pos;
			GetCursorPos(&pos);
			::SetForegroundWindow(m_hWndNotify);

			::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y,
				0, m_hWndNotify, NULL);

			::PostMessage(m_hWndNotify, WM_NULL, 0, 0);

			menu.DestroyMenu();

			ShellNotify(NIM_SETFOCUS);
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			// double click received, the default action is to execute default menu item
			::SetForegroundWindow(m_hWndNotify);

			UINT uItem;
			if (m_bDefMenuItemByPos)
			{
				CMenu menu;
				if (!menu.LoadMenu(m_niData.uID))
				{
					return 0;
				}

				CMenu* pSubMenu = menu.GetSubMenu(0);
				if (pSubMenu == NULL)
				{
					return 0;
				}

				uItem = pSubMenu->GetMenuItemID(m_uDefMenuItemID);

				menu.DestroyMenu();
			}
			else
			{
				uItem = m_uDefMenuItemID;
			}

			::SendMessage(m_hWndNotify, WM_COMMAND, uItem, 0);
		}
	}

	return 1;
}

LRESULT CXTTrayIcon::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == m_niData.uCallbackMessage)
	{
		return OnTrayNotification(wParam, lParam);
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

// This is called whenever the taskbar is created (eg after explorer crashes
// and restarts. Please note that the WM_TASKBARCREATED message is only passed
// to TOP LEVEL windows (like WM_QUERYNEWPALETTE)

LRESULT CXTTrayIcon::OnTaskbarCreated(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	InstallIconPending();
	return 0L;
}

void CXTTrayIcon::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CWnd::OnSettingChange(uFlags, lpszSection);

	if (uFlags == SPI_SETWORKAREA)
	{
		InstallIconPending();
	}
}

void CXTTrayIcon::InstallIconPending()
{
	// Is the icon display pending, and it's not been set as "hidden"?
	if (!m_bShowPending || m_bHidden)
	{
		return;
	}

	// Reset the flags to what was used at creation
	m_niData.uFlags = m_uFlags;

	// Try and recreate the icon
	m_bHidden = !ShellNotify(NIM_ADD);

	// If it's STILL hidden, then have another go next time...
	m_bShowPending = !m_bHidden;

	ASSERT(m_bHidden == FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon minimize and restore helper functions

BOOL CALLBACK CXTTrayIcon::FindTrayWnd(HWND hWnd, LPARAM lParam)
{
	TCHAR szClassName[256];
	::GetClassName(hWnd, szClassName, 255);

	// Did we find the main system tray ? If so, then get its size and keep going
	if (_tcscmp(szClassName, _T("TrayNotifyWnd")) == 0)
	{
		CRect *pRect = (CRect*) lParam;
		::GetWindowRect(hWnd, pRect);
		return TRUE;
	}

	// Did we find the System Clock ? If so, then adjust the size of the rectangle
	// we have and quit (clock will be found after the system tray)
	if (_tcscmp(szClassName, _T("TrayClockWClass")) == 0)
	{
		CRect *pRect = (CRect*)lParam;
		CRect rectClock;
		::GetWindowRect(hWnd, rectClock);

		// if clock is above system tray adjust accordingly
		if (rectClock.bottom < pRect->bottom-5)    // 10 = random fudge factor.
		{
			pRect->top = rectClock.bottom;
		}
		else
		{
			pRect->right = rectClock.left;
		}

		return FALSE;
	}

	return TRUE;
}

bool CXTTrayIcon::GetTrayWindowRect(CRect& rect)
{
	int cx = 150;
	int cy = 30;

	HWND hWndTray = ::FindWindow(_T("Shell_TrayWnd"), NULL);
	if (hWndTray)
	{
		::GetWindowRect(hWndTray, &rect);
		::EnumChildWindows(hWndTray, FindTrayWnd, (LPARAM)&rect);
		return true;
	}

	// OK, we failed to get the rect from the quick hack. Either explorer isn't
	// running or it's a new version of the shell with the window class names
	// changed (how dare Microsoft change these undocumented class names!) So, we
	// try to find out what side of the screen the taskbar is connected to. We
	// know that the system tray is either on the right or the bottom of the
	// taskbar, so we can make a good guess at where to minimize to

	APPBARDATA ad;
	::ZeroMemory(&ad, sizeof(APPBARDATA));
	ad.cbSize = sizeof(ad);

	if (::SHAppBarMessage(ABM_GETTASKBARPOS, &ad))
	{
		// We know the edge the taskbar is connected to, so guess the rect of the
		// system tray. Use various fudge factor to make it look good

		switch (ad.uEdge)
		{
		case ABE_LEFT:
		case ABE_RIGHT:
			// We want to minimize to the bottom of the taskbar
			rect.top = ad.rc.bottom-100;
			rect.bottom = ad.rc.bottom-16;
			rect.left = ad.rc.left;
			rect.right = ad.rc.right;
			break;

		case ABE_TOP:
		case ABE_BOTTOM:
			// We want to minimize to the right of the taskbar
			rect.top = ad.rc.top;
			rect.bottom = ad.rc.bottom;
			rect.left = ad.rc.right-100;
			rect.right = ad.rc.right-16;
			break;
		}

		return true;
	}

	// Blimey, we really aren't in luck. It's possible that a third party shell
	// is running instead of explorer. This shell might provide support for the
	// system tray, by providing a Shell_TrayWnd window (which receives the
	// messages for the icons) So, look for a Shell_TrayWnd window and work out
	// the rect from that. Remember that explorer's taskbar is the Shell_TrayWnd,
	// and stretches either the width or the height of the screen. We can't rely
	// on the 3rd party shell's Shell_TrayWnd doing the same, in fact, we can't
	// rely on it being any size. The best we can do is just blindly use the
	// window rect, perhaps limiting the width and height to, say 150 square.
	// Note that if the 3rd party shell supports the same configuration as
	// explorer (the icons hosted in NotifyTrayWnd, which is a child window of
	// Shell_TrayWnd), we would already have caught it above

	if (hWndTray)
	{
		::GetWindowRect(hWndTray, &rect);

		if (rect.right - rect.left > cx)
		{
			rect.left = rect.right - cx;
		}
		if (rect.bottom - rect.top > cy)
		{
			rect.top = rect.bottom - cy;
		}

		return true;
	}

	// OK. Haven't found a thing. Provide a default rect based on the current work
	// area
	if (::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0))
	{
		rect.left = rect.right - cx;
		rect.top = rect.bottom - cy;
		return true;
	}

	return false;
}

bool CXTTrayIcon::CanAnimate() const
{
	ANIMATIONINFO ai;
	::ZeroMemory(&ai, sizeof(ANIMATIONINFO));
	ai.cbSize = sizeof(ANIMATIONINFO);

	::SystemParametersInfo(SPI_GETANIMATION,
		sizeof(ANIMATIONINFO), &ai, 0);

	return ai.iMinAnimate ? true : false;
}

/////////////////////////////////////////////////////////////////////////////
// CXTTrayIcon minimize and restore functions

bool CXTTrayIcon::CreateMinimizeWnd(CWnd* pWndApp)
{
	// Create the minimize window
	if (!::IsWindow(m_wndMinimize.m_hWnd))
	{
		if (!m_wndMinimize.CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0))
		{
			return false;
		}
	}

	pWndApp->SetParent(&m_wndMinimize);
	return true;
}

void CXTTrayIcon::MinimizeToTray(CWnd* pWndApp)
{
	if (CanAnimate())
	{
		CRect rectFrom, rectTo;

		pWndApp->GetWindowRect(rectFrom);
		GetTrayWindowRect(rectTo);

		::DrawAnimatedRects(pWndApp->m_hWnd,
			IDANI_CAPTION, rectFrom, rectTo);
	}

	CreateMinimizeWnd(pWndApp);
	pWndApp->ShowWindow(SW_HIDE);
}

void CXTTrayIcon::MaximizeFromTray(CWnd* pWndApp)
{
	if (CanAnimate())
	{
		CRect rectTo;
		pWndApp->GetWindowRect(rectTo);

		CRect rectFrom;
		GetTrayWindowRect(rectFrom);

		pWndApp->SetParent(NULL);
		::DrawAnimatedRects(pWndApp->m_hWnd,
			IDANI_CAPTION, rectFrom, rectTo);
	}
	else
	{
		pWndApp->SetParent(NULL);
	}

	pWndApp->ShowWindow(SW_SHOW);
	pWndApp->RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME |
		RDW_INVALIDATE | RDW_ERASE);

	// Move focus away and back again to ensure taskbar icon is recreated
	if (::IsWindow(m_wndMinimize.m_hWnd))
	{
		m_wndMinimize.SetActiveWindow();
	}

	pWndApp->SetActiveWindow();
	pWndApp->SetForegroundWindow();
}
