// XTDirWatcher.cpp : implementation file
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
#include "Common/XTPVC80Helpers.h"  // Visual Studio 2005 helper functions

#include "XTDefines.h"
#include "XTShellPidl.h"
#include "XTDirWatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTDirWatcher

CXTDirWatcher::CXTDirWatcher()
{
	m_bAutoDelete = TRUE;
	m_dwMonitorEvents[0] = INVALID_HANDLE_VALUE;
	m_dwMonitorEvents[1] = INVALID_HANDLE_VALUE;
	m_dwMonitorEvents[2] = CreateEvent(NULL, TRUE, FALSE, 0);   // Path was changed event.
	m_dwMonitorEvents[3] = CreateEvent(NULL, TRUE, FALSE, 0);   // Stop notifications event.

	ZeroMemory(&m_tvid, sizeof(m_tvid));
}

CXTDirWatcher::~CXTDirWatcher()
{
	CloseHandle(m_dwMonitorEvents[2]);
	CloseHandle(m_dwMonitorEvents[3]);
}

void CXTDirWatcher::StopNotifications()
{
	SetEvent(m_dwMonitorEvents[3]);
	WaitForSingleObject(m_hThread, INFINITE);
}


IMPLEMENT_DYNCREATE(CXTDirWatcher, CWinThread)

BEGIN_MESSAGE_MAP(CXTDirWatcher, CWinThread)
	//{{AFX_MSG_MAP(CXTDirWatcher)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CXTDirWatcher::WaitPathChangedEvent()
{
	DWORD dw = ::WaitForMultipleObjects(2,
		&m_dwMonitorEvents[2], FALSE, INFINITE);

	return dw != WAIT_OBJECT_0 + 1;
}

BOOL CXTDirWatcher::MonitorNotifications()
{
	BOOL bContinueThread = TRUE;

	::ResetEvent(m_dwMonitorEvents[2]);
	::ResetEvent(m_dwMonitorEvents[3]);

	if (!IsPathValid(m_strFolderPath))
		return WaitPathChangedEvent();

	// Watch the directory for file creation and
	// deletion.

	m_dwMonitorEvents[0] = ::FindFirstChangeNotification(
		m_strFolderPath, // directory to watch
		FALSE, // do not watch the subtree
		FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes

	if (m_dwMonitorEvents[0] == INVALID_HANDLE_VALUE)
		return WaitPathChangedEvent();

	// Watch the tree for directory creation and
	// deletion.

	m_dwMonitorEvents[1] = ::FindFirstChangeNotification(
		m_strFolderPath, // directory to watch
		FALSE, // watch the subtree
		FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir. name changes

	if (m_dwMonitorEvents[1] == INVALID_HANDLE_VALUE)
		return WaitPathChangedEvent();

	// Change notification is set. Now wait on both notification
	// handles and refresh accordingly.

	BOOL bConinueNotifications = TRUE;

	while (bConinueNotifications)
	{
		// Wait for notification.

		DWORD dwWaitStatus = ::WaitForMultipleObjects(_countof(m_dwMonitorEvents),
			m_dwMonitorEvents, FALSE, INFINITE);


		switch (dwWaitStatus)
		{
		case WAIT_OBJECT_0:

			// A file was created or deleted in C:\WINDOWS.
			// Refresh this directory and restart the
			// change notification. RefreshDirectory is an
			// application-defined function.

			RefreshFolder();

			if (!::FindNextChangeNotification(m_dwMonitorEvents[0]))
				bConinueNotifications = FALSE;

			if (!IsPathValid(m_strFolderPath))
				bConinueNotifications = FALSE;

			break;

		case WAIT_OBJECT_0 + 1:

			// A directory was created or deleted in C:\.
			// Refresh the directory tree and restart the
			// change notification. RefreshTree is an
			// application-defined function.

			RefreshTree();

			if (!::FindNextChangeNotification(m_dwMonitorEvents[1]))
				bConinueNotifications = FALSE;

			if (!IsPathValid(m_strFolderPath))
				bConinueNotifications = FALSE;

			break;

		case WAIT_OBJECT_0 + 2:
			bContinueThread = TRUE;
			bConinueNotifications = FALSE;
			break;

		case WAIT_OBJECT_0 + 3:
			bContinueThread = FALSE;
			bConinueNotifications = FALSE;
			break;
		}
	}

	// Close the file change notification handle and return.
	::FindCloseChangeNotification (m_dwMonitorEvents[0]);
	::FindCloseChangeNotification (m_dwMonitorEvents[1]);

	m_dwMonitorEvents[0] = INVALID_HANDLE_VALUE;
	m_dwMonitorEvents[1] = INVALID_HANDLE_VALUE;

	return bContinueThread;
}

BOOL CXTDirWatcher::InitInstance()
{
	BOOL bContinueThread = TRUE;

	while (bContinueThread)
	{
		bContinueThread = MonitorNotifications();
	}

	return FALSE;
}

BOOL CXTDirWatcher::IsPathValid(LPCTSTR lpszFolderPath)
{
	if (_tcslen(lpszFolderPath) == 0)
		return FALSE;

	if (!DIRECTORYEXISTS_S(lpszFolderPath))
		return FALSE;

	return TRUE;
}

BOOL CXTDirWatcher::SetFolderPath(CWnd* pMainWnd, LPCTSTR lpszFolderPath)
{
	if (IsPathValid(lpszFolderPath))
	{
		if (GetFolderData(lpszFolderPath, m_tvid))
		{
			m_pMainWnd = pMainWnd;
			m_strFolderPath = lpszFolderPath;

			SetEvent(m_dwMonitorEvents[2]);  // Folder was changed event

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CXTDirWatcher::SetFolderData(CWnd* pMainWnd, XT_TVITEMDATA* lpTVID)
{
	if (!lpTVID)
		return FALSE;

	TCHAR szFolderPath[_MAX_PATH];
	if (::SHGetPathFromIDList(lpTVID->lpifq, szFolderPath))
	{
		return SetFolderPath(pMainWnd, szFolderPath);
	}

	return FALSE;
}

BOOL CXTDirWatcher::GetFolderData(LPCTSTR lpszFolderPath, XT_TVITEMDATA& lpTVID)
{
	LPITEMIDLIST  pidl;
	LPSHELLFOLDER pDesktopFolder;
	OLECHAR       szOleChar[MAX_PATH];
	ULONG         chEaten;
	ULONG         dwAttributes;

	if (!IsPathValid(lpszFolderPath))
		return FALSE;

	// Get a pointer to the Desktop's IShellFolder interface.
	if (SUCCEEDED(::SHGetDesktopFolder(&pDesktopFolder)))
	{
		// IShellFolder::ParseDisplayName requires the file name be in
		// Unicode.

#if !defined(_UNICODE)
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszFolderPath, -1,
			szOleChar, MAX_PATH);
#else
		STRCPY_S(szOleChar, MAX_PATH, lpszFolderPath);
#endif

		// Convert the path to an ITEMIDLIST.
		if (SUCCEEDED(pDesktopFolder->ParseDisplayName(NULL, NULL, szOleChar,
			&chEaten, &pidl, &dwAttributes)))
		{
			IShellFolder *psfMyFolder;

			lpTVID.lpi = lpTVID.lpifq = pidl;

			pDesktopFolder->BindToObject(lpTVID.lpifq, NULL,
				IID_IShellFolder, (LPVOID*)&psfMyFolder);

			lpTVID.lpsfParent = psfMyFolder;
			pDesktopFolder->Release();

			return TRUE;
		}

		pDesktopFolder->Release();
	}

	return FALSE;
}

void CXTDirWatcher::RefreshFolder()
{
	m_pMainWnd->SendMessage(XTWM_SHELL_NOTIFY,
		SHN_XT_REFRESHFOLDER, (LPARAM)&m_tvid);
}

void CXTDirWatcher::RefreshTree()
{
	m_pMainWnd->SendMessage(XTWM_SHELL_NOTIFY,
		SHN_XT_REFRESHTREE, (LPARAM)&m_tvid);
}
