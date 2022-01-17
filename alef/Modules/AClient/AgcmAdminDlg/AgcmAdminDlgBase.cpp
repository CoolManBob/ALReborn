// AgcmAdminDlgBase.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 29.

#include "AgcmAdminDlgInclude.h"
#include "AgcmAdminDlg.h"

AgcmAdminDlgBase::AgcmAdminDlgBase()
{
	m_hInst = NULL;
	m_hWnd = NULL;
	m_hDlg = NULL;
	m_bInitialized = FALSE;

	m_iResourceID = 0;
	m_pfDlgProc = NULL;

	m_pcsCBClass = NULL;

	InitializeCriticalSection(&m_csLock);
}

AgcmAdminDlgBase::~AgcmAdminDlgBase()
{
	if(IsWindow(m_hDlg))
		DestroyWindow(m_hDlg);

	DeleteCriticalSection(&m_csLock);
}

void AgcmAdminDlgBase::SetInst(HINSTANCE hInst, HWND hWnd)
{
	m_hInst = hInst;
	m_hWnd = hWnd;
}

void AgcmAdminDlgBase::SetParentHwnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

UINT AgcmAdminDlgBase::GetResourceID()
{
	return m_iResourceID;
}

DLGPROC AgcmAdminDlgBase::GetDlgProc()
{
	return (DLGPROC)m_pfDlgProc;
}

BOOL AgcmAdminDlgBase::OpenDlg()
{
	if(m_iResourceID == 0 || m_pfDlgProc == NULL)
		return FALSE;

	if(IsWindow(m_hDlg))
	{
		ShowWindow(m_hDlg, SW_SHOW);
		SetForegroundWindow(m_hDlg);
	}
	else
	{
		m_hDlg = CreateDialog(m_hInst, MAKEINTRESOURCE(m_iResourceID), m_hWnd, (DLGPROC)m_pfDlgProc);
		if(m_hDlg)
			ShowWindow(m_hDlg, SW_SHOW);
		else
			return FALSE;
	}

	return TRUE;
}

// 닫는 다고 윈도우를 아예 닫는 건 아니다.
// 감추어 놓는 것 뿐. 2003.11.13.
BOOL AgcmAdminDlgBase::CloseDlg(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	if(!IsWindow(hDlg))
		return FALSE;

	::ShowWindow(hDlg, SW_HIDE);

	return TRUE;
}

HWND AgcmAdminDlgBase::GetHDlg()
{
	return m_hDlg;
}

BOOL AgcmAdminDlgBase::ShowData(HWND hDlg)
{
	return TRUE;
}

BOOL AgcmAdminDlgBase::SetCBClass(PVOID pClass)
{
	m_pcsCBClass = pClass;
	return TRUE;
}

void AgcmAdminDlgBase::Lock()
{
	EnterCriticalSection(&m_csLock);
}

void AgcmAdminDlgBase::Unlock()
{
	LeaveCriticalSection(&m_csLock);
}

LRESULT AgcmAdminDlgBase::OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SetInitialized(TRUE);
	return FALSE;
}

LRESULT AgcmAdminDlgBase::OnActivate(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_INACTIVE)
		::ShowCursor(FALSE);
	else
		::ShowCursor(TRUE);

	::InvalidateRect(hDlg, NULL, TRUE);
	return FALSE;
}

void AgcmAdminDlgBase::SetInitialized(BOOL bFlag)
{
	m_bInitialized = bFlag;
}

BOOL AgcmAdminDlgBase::GetInitialized()
{
	return m_bInitialized;
}

BOOL AgcmAdminDlgBase::SetWindowPosByMousePoint(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	return TRUE;
}

BOOL AgcmAdminDlgBase::SetWindowPosByParentWnd(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;
	if(!m_hWnd) return FALSE;

	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	SetWindowPos(hDlg, HWND_TOP, rect.left, rect.top, 0, 0, SWP_NOSIZE);

	return TRUE;
}