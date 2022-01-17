#include "AuSingleProcessChecker.h"

AuSingleProcessChecker::AuSingleProcessChecker(const char *i_szProcessName, const char *i_szAppWindowTile /* = NULL */)
{
	m_hMutex			= NULL;
	m_szProcessName		= i_szProcessName;
	m_szAppWindowTile	= i_szAppWindowTile;
}

AuSingleProcessChecker::~AuSingleProcessChecker(void)
{
	CleanChecker();
}

BOOL AuSingleProcessChecker::InitChecker()
{
	if (NULL != m_hMutex)
	{
		return FALSE;
	}

	m_hMutex = CreateMutex(NULL, FALSE, m_szProcessName);
	if (NULL == m_hMutex)
	{
		return FALSE;
	}

	if (ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		// Application is running already...
		return FALSE;
	}

	return TRUE;
}

void AuSingleProcessChecker::CleanChecker()
{
	if (m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}

BOOL AuSingleProcessChecker::IsAlreadyRunning()
{
	HANDLE hMutex = NULL;	
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, m_szProcessName);
	if (hMutex == NULL)
	{
		return FALSE;
	}	

	return TRUE;
}

void AuSingleProcessChecker::ActivateFirstApplication()
{
	CWnd *pWndPrev, *pWndChild;

	// Determine if a window with the class name exists...
	if (pWndPrev = CWnd::FindWindow(NULL, _T(m_szAppWindowTile)))
	{
		// If so, does it have any popups?
		pWndChild = pWndPrev->GetLastActivePopup();

		// If iconic, restore the main window
		if (pWndPrev->IsIconic())
			pWndPrev->ShowWindow(SW_RESTORE);

		// Bring the main window or its popup to the foreground
		pWndChild->SetForegroundWindow();
	}
}