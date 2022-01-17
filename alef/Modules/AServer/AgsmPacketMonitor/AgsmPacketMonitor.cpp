// AgsmPacketMonitor.cpp : Defines the entry point for the DLL application.
//

//#include "stdafx.h"

#include <stdio.h>
#include "ApBase.h"
#include <commctrl.h>
#include "resource.h"
#include "AgsmPacketMonitor.h"

HINSTANCE			AgsmPacketMonitor::m_hInstance = NULL;
HWND				AgsmPacketMonitor::m_hDialog = NULL;
HWND				AgsmPacketMonitor::m_hList = NULL;
HWND				AgsmPacketMonitor::m_hRadioSend = NULL;
HWND				AgsmPacketMonitor::m_hRadioRecv = NULL;
AgsmPMPacketType	AgsmPacketMonitor::m_ePacketType = AGSMPACKETMONITOR_PACKET_TYPE_SEND;
AgsmPacketMonitor *	AgsmPacketMonitor::m_pThis = NULL;
BOOL				AgsmPacketMonitor::m_bDestroy = FALSE;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	AgsmPacketMonitor::m_hInstance = (HINSTANCE) hModule;

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
// see AgsmPacketMonitor.h for the class definition
AgsmPacketMonitor::AgsmPacketMonitor()
{ 
	SetModuleName("AgsmPacketMonitor");
	EnableIdle(TRUE);

	m_ulLastTick = 0;

	m_bDestroy = FALSE;

	m_hDialog = NULL;
	m_pThis = this;

	return; 
}

AgsmPacketMonitor::~AgsmPacketMonitor()
{
	return;
}

BOOL	AgsmPacketMonitor::OnAddModule()
{
	m_hThread = CreateThread(NULL,
						  0,
						  DialogThread,
						  this,
						  0,
						  &m_ulThreadID);

	return TRUE;
}

BOOL	AgsmPacketMonitor::OnInit()
{
	return TRUE;
}

BOOL	AgsmPacketMonitor::OnIdle(UINT32 ulClockCount)
{
	if(!m_hDialog || ulClockCount - m_ulLastTick < 1000)
		return TRUE;

	m_ulLastTick = ulClockCount;

	//UpdateStatistics(FALSE);
	if (m_hDialog)
		SendMessage(m_hDialog, WM_TIMER, 0, 0);

	return TRUE;
}

VOID	AgsmPacketMonitor::UpdateStatistics(BOOL bChangeEntry)
{
	CHAR				szTemp[256];
	INT32				lIndex = 0;
	INT32				lPacketID;
	LVITEM				LI;
	pstPacketStatistics	pstPS;

	if (bChangeEntry)
		SendMessage(m_hList, LVM_DELETEALLITEMS, 0, 0);

	for (lPacketID = 0; lPacketID < APPACKET_MAX_PACKET_TYPE; ++lPacketID)
	{
		switch (m_ePacketType)
		{
		case AGSMPACKETMONITOR_PACKET_TYPE_SEND:
			pstPS = m_pThis->GetSendPacketStatistic(lPacketID);
			break;

		case AGSMPACKETMONITOR_PACKET_TYPE_RECV:
			pstPS = m_pThis->GetRecvPacketStatistic(lPacketID);
			break;

		default:
			pstPS = NULL;
		}

		if (!pstPS)
			continue;

		if (bChangeEntry)
		{
			LI.mask = LVIF_TEXT;
			LI.state = 0;
			LI.stateMask = 0;

			sprintf(szTemp, "%d", lPacketID);
			LI.iItem = lIndex;
			LI.iSubItem = 0;
			LI.pszText = szTemp;
			SendMessage(m_hList, LVM_INSERTITEM, lIndex, (LPARAM) &LI);
		}

		LI.iItem = lIndex;

		sprintf(szTemp, "%d", pstPS->ullPacketSize);
		LI.mask = LVIF_TEXT;
		LI.iSubItem = 1;
		LI.pszText = szTemp;
		SendMessage(m_hList, LVM_SETITEM, lIndex, (LPARAM) &LI);

		sprintf(szTemp, "%d", pstPS->ulAveragePacketSize);
		LI.mask = LVIF_TEXT;
		LI.iSubItem = 2;
		LI.pszText = szTemp;
		SendMessage(m_hList, LVM_SETITEM, lIndex, (LPARAM) &LI);

		sprintf(szTemp, "%d", pstPS->ulPacketCount);
		LI.mask = LVIF_TEXT;
		LI.iSubItem = 3;
		LI.pszText = szTemp;
		SendMessage(m_hList, LVM_SETITEM, lIndex, (LPARAM) &LI);

		sprintf(szTemp, "%d", pstPS->ulTotalThroughputBPS);
		LI.mask = LVIF_TEXT;
		LI.iSubItem = 4;
		LI.pszText = szTemp;
		SendMessage(m_hList, LVM_SETITEM, lIndex, (LPARAM) &LI);

		sprintf(szTemp, "%d", pstPS->ulPeakThroughputBPS);
		LI.mask = LVIF_TEXT;
		LI.iSubItem = 5;
		LI.pszText = szTemp;
		SendMessage(m_hList, LVM_SETITEM, lIndex, (LPARAM) &LI);

		++lIndex;
	}

	//m_pThis->ResetPacketStatistics();
}

BOOL	AgsmPacketMonitor::OnDestroy()
{
	if (m_hDialog)
		SendMessage(m_hDialog, WM_CLOSE, 0, 0);

	return TRUE;
}

BOOL	AgsmPacketMonitor::MonitorProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		if (!SetupDialog(hDlg))
			return FALSE;

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RADIO_PACKET_SEND:
			m_ePacketType = AGSMPACKETMONITOR_PACKET_TYPE_SEND;
			UpdateStatistics(TRUE);
			break;

		case IDC_RADIO_PACKET_RECV:
			m_ePacketType = AGSMPACKETMONITOR_PACKET_TYPE_RECV;
			UpdateStatistics(TRUE);
			break;
		}

		return TRUE;

	case WM_TIMER:
		UpdateStatistics(FALSE);
		return TRUE;

	case WM_CLOSE:
		m_bDestroy = TRUE;
		return TRUE;

/*
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
				}
			}
		}

		return TRUE;
*/

	case IDCANCEL:
	case IDOK:
		EndDialog(hDlg, iMessage);
		return TRUE;
	}

	return FALSE;
}

DWORD	AgsmPacketMonitor::DialogThread(PVOID pvParam)
{
	AgsmPacketMonitor *	pThis = (AgsmPacketMonitor *) pvParam;
	MSG				stMessage;

	m_hDialog = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG_PACKET_MONITOR), NULL, MonitorProc);

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

BOOL	AgsmPacketMonitor::SetupDialog(HWND hDlg)
{
	LVCOLUMN	COL;

	m_hList = GetDlgItem(hDlg, IDC_LIST_PACKET);
	m_hRadioSend = GetDlgItem(hDlg, IDC_RADIO_PACKET_SEND);
	m_hRadioRecv = GetDlgItem(hDlg, IDC_RADIO_PACKET_RECV);

	if (!m_hList || !m_hRadioSend || !m_hRadioRecv)
		return FALSE;

	COL.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	COL.fmt = LVCFMT_LEFT;
	COL.cx = 60;
	COL.pszText = "Packet Type";
	COL.iSubItem = 0;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 0, (LPARAM) &COL);

	COL.cx = 60;
	COL.pszText = "Size";
	COL.iSubItem = 1;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 1, (LPARAM) &COL);

	COL.cx = 70;
	COL.pszText = "AVR Size";
	COL.iSubItem = 2;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 2, (LPARAM) &COL);

	COL.cx = 70;
	COL.pszText = "Count";
	COL.iSubItem = 3;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 3, (LPARAM) &COL);

	COL.cx = 70;
	COL.pszText = "Total BPS";
	COL.iSubItem = 4;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 4, (LPARAM) &COL);

	COL.cx = 70;
	COL.pszText = "Peak BPS";
	COL.iSubItem = 5;
	SendMessage(m_hList, LVM_INSERTCOLUMN, 5, (LPARAM) &COL);

	SendMessage(m_hRadioSend, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(m_hRadioRecv, BM_SETCHECK, BST_UNCHECKED, 0);

	UpdateStatistics(TRUE);

	return TRUE;
}
