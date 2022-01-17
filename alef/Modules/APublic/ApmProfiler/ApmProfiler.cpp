// ApmProfiler.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "resource.h"
#include "ApmProfiler.h"

HINSTANCE		ApmProfiler::m_hInstance = NULL;
HWND			ApmProfiler::m_hDialog = NULL;
HWND			ApmProfiler::m_hList = NULL;
HWND			ApmProfiler::m_hEntry = NULL;
CProfileNode *	ApmProfiler::m_pRoot = NULL;
CProfileNode *	ApmProfiler::m_pCurNode = NULL;
CHAR			ApmProfiler::m_szEntry[1024];
BOOL			ApmProfiler::m_bDestroy = FALSE;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	ApmProfiler::m_hInstance = (HINSTANCE) hModule;

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


// This is the constructor of a class that has been exported.
// see ApmProfiler.h for the class definition
ApmProfiler::ApmProfiler()
{
	SetModuleName("ApmProfiler");
	EnableIdle(TRUE);

	m_ulLastTick = 0;
	m_pCurNode = NULL;

	m_bDestroy = FALSE;

	m_hDialog = NULL;

	return; 
}

ApmProfiler::~ApmProfiler()
{
	return;
}

BOOL	ApmProfiler::OnAddModule()
{
	m_hThread = CreateThread(NULL,
						  0,
						  DialogThread,
						  this,
						  0,
						  &m_ulThreadID);

	return TRUE;
}

BOOL	ApmProfiler::OnInit()
{
	return TRUE;
}

BOOL	ApmProfiler::OnIdle(UINT32 ulClockCount)
{
	AuProfileManager::Increment_Frame_Counter();

	if(!m_hDialog || ulClockCount - m_ulLastTick < 1000)
		return TRUE;

	m_ulLastTick = ulClockCount;

	//UpdateStatistics(FALSE);
	if (m_hDialog)
		SendMessage(m_hDialog, WM_TIMER, 0, 0);

	return TRUE;
}

VOID	ApmProfiler::UpdateStatistics(BOOL bChangeEntry)
{
	CHAR			szTemp[256];
	CProfileNode *	cnode;
	FLOAT			time = AuProfileManager::Get_Time_Since_Reset();
	INT32			y = 50;
	INT32			index = 0;
	INT32			lRootIndex;
	BOOL			bRoot = FALSE;
	FLOAT			tf;
	LVITEM			LI;

	AuProfileManager::Lock();

	if (!m_pCurNode || !strcmp("Root", m_pCurNode->Get_Name()))
		bRoot = TRUE;

	if (bChangeEntry)
		SendMessage(m_hList, LVM_DELETEALLITEMS, 0, 0);

	for (lRootIndex = 0; lRootIndex < AUPROFILEMANAGER_MAX_THREADS; ++lRootIndex)
	{
		if (bRoot)
		{
			m_pCurNode = AuProfileManager::Roots[lRootIndex];

			if (!m_pCurNode)
				continue;
		}

		cnode = m_pCurNode;

		if (!bRoot)
		{
			if (bChangeEntry)
			{
				LI.mask = LVIF_TEXT | LVIF_PARAM;
				LI.state = 0;
				LI.stateMask = 0;

				LI.iItem = index;
				LI.iSubItem = 0;
				LI.pszText = "../";
				LI.lParam = (LPARAM) cnode->Get_Parent();
				SendMessage(m_hList, LVM_INSERTITEM, index, (LPARAM) &LI);
			}

			++index;
		}

		cnode = cnode->Get_Child();

		while (cnode)
		{
			tf = cnode->Get_Total_Time() / time;

			LI.mask = LVIF_TEXT | LVIF_PARAM;
			LI.state = 0;
			LI.stateMask = 0;

			LI.iItem = index;
			LI.iSubItem = 0;

			if (bChangeEntry)
			{
				LI.pszText = (CHAR *) cnode->Get_Name();
				LI.lParam = (LPARAM) cnode;
				SendMessage(m_hList, LVM_INSERTITEM, index, (LPARAM) &LI);
			}
			else
			{
				LI.pszText = szTemp;
				LI.pszText[0] = '\0';
				LI.cchTextMax = 255;

				if (!SendMessage(m_hList, LVM_GETITEM, index, (LPARAM) &LI) || strcmp(LI.pszText, cnode->Get_Name()))
				{
					AuProfileManager::Unlock();

					UpdateStatistics(TRUE);
					return;
				}
			}

			sprintf(szTemp, "%d", cnode->Get_Thread_ID());
			LI.mask = LVIF_TEXT;
			LI.iSubItem = 1;
			LI.pszText = szTemp;
			SendMessage(m_hList, LVM_SETITEM, index, (LPARAM) &LI);

			sprintf(szTemp, "%d", cnode->Get_Total_Calls());
			LI.mask = LVIF_TEXT;
			LI.iSubItem = 2;
			LI.pszText = szTemp;
			SendMessage(m_hList, LVM_SETITEM, index, (LPARAM) &LI);

			sprintf(szTemp, "%f", cnode->Get_Total_Time());
			LI.mask = LVIF_TEXT;
			LI.iSubItem = 3;
			LI.pszText = szTemp;
			SendMessage(m_hList, LVM_SETITEM, index, (LPARAM) &LI);

			sprintf(szTemp, "%d", (INT32) (tf * 100.0f));
			LI.mask = LVIF_TEXT;
			LI.iSubItem = 4;
			LI.pszText = szTemp;
			SendMessage(m_hList, LVM_SETITEM, index, (LPARAM) &LI);
			
			++index;
			cnode = cnode->Get_Sibling();
		}

		if (!bRoot)
			break;
	}

	AuProfileManager::Reset();

	AuProfileManager::Unlock();
}

BOOL	ApmProfiler::OnDestroy()
{
	if (m_hDialog)
		SendMessage(m_hDialog, WM_CLOSE, 0, 0);

	return TRUE;
}

VOID	ApmProfiler::SetProfileManager(CProfileNode **ppRoots)
{
	if (AuProfileManager::Roots != ppRoots)
	{
		AuProfileManager::Roots = ppRoots;
	}
}

BOOL	ApmProfiler::ProfilerProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		if (!SetupDialog(hDlg))
			return FALSE;

		return TRUE;

	case WM_COMMAND:
		return TRUE;

	case WM_TIMER:
		UpdateStatistics(FALSE);
		return TRUE;

	case WM_CLOSE:
		m_bDestroy = TRUE;
		return TRUE;

	case WM_NOTIFY:
		{
			LPNMHDR			hdr = (LPNMHDR) lParam;
			LPNMLISTVIEW	pnmv = (LPNMLISTVIEW) lParam;
			LVITEM			item;

			memset(&item, 0, sizeof(LVITEM));

			if (hdr->hwndFrom == m_hList)
			{
				switch (hdr->code)
				{
				case LVN_ITEMCHANGED:
					if (pnmv->uChanged == LVIF_STATE && pnmv->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
					{
						item.mask = LVIF_TEXT | LVIF_PARAM;
						item.iItem = pnmv->iItem;
						SendMessage(m_hList, LVM_GETITEM, 0, (LPARAM) &item);

						if (item.lParam)
						{
							m_pCurNode = (CProfileNode *) item.lParam;
							SetCurrentEntry();
							UpdateStatistics(TRUE);
						}
					}

					return TRUE;
				}
			}

			return TRUE;
		}

	case IDCANCEL:
	case IDOK:
		EndDialog(hDlg, iMessage);
		return TRUE;
	}

	return FALSE;
}

DWORD	ApmProfiler::DialogThread(PVOID pvParam)
{
	ApmProfiler *	pThis = (ApmProfiler *) pvParam;
	MSG				stMessage;

	m_hDialog = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG_PROFILER), NULL, ProfilerProc);

	if (m_hDialog)
	{
		ShowWindow(m_hDialog, SW_SHOW);

		while (!pThis->m_bDestroy)
		{
			if (GetMessage(&stMessage, 0, 0, 0))
			{
				IsDialogMessage(m_hDialog, &stMessage);
			}
		}

		EndDialog(m_hDialog, IDOK);

		m_hDialog = NULL;
	}

	return 0;
}

BOOL	ApmProfiler::SetupDialog(HWND hDlg)
{
	LVCOLUMN	COL;

	m_hList = GetDlgItem(hDlg, IDC_LIST_PROFILER);
	m_hEntry = GetDlgItem(hDlg, IDC_EDIT_ENTRY);

	if (!m_hList || !m_hEntry)
		return FALSE;

	COL.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	COL.fmt = LVCFMT_LEFT;
	COL.cx = 240;
	COL.pszText = "Function";
	COL.iSubItem = 0;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 0, (LPARAM) &COL);

	COL.cx = 60;
	COL.pszText = "Thread";
	COL.iSubItem = 1;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 1, (LPARAM) &COL);

	COL.cx = 60;
	COL.pszText = "Calls";
	COL.iSubItem = 2;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 2, (LPARAM) &COL);

	COL.cx = 75;
	COL.pszText = "Elapsed msec";
	COL.iSubItem = 3;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 3, (LPARAM) &COL);

	COL.cx = 50;
	COL.pszText = "%";
	COL.iSubItem = 4;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 4, (LPARAM) &COL);

	m_pCurNode = AuProfileManager::Root;

	SetCurrentEntry();
	UpdateStatistics(TRUE);

	return TRUE;
}

VOID	ApmProfiler::SetCurrentEntry()
{
	CHAR			szTemp[1024];
	CProfileNode *	cnode = m_pCurNode;

	sprintf(m_szEntry, "/");

	while (cnode != AuProfileManager::Root)
	{
		strcpy(szTemp, m_szEntry);
		sprintf(m_szEntry, "/%s%s", cnode->Get_Name(), szTemp);
		cnode = cnode->Get_Parent();
	}

	SetWindowText(m_hEntry, m_szEntry);
}