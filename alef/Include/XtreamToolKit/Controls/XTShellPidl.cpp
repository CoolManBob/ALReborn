// XTShellPidl.cpp : implementation file
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

#include "XTShellPidl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTShellPidl::CShellSpecialFolder::CShellSpecialFolder(int nFolder)
{
	m_lpFolder = NULL;

	if (FAILED(::SHGetDesktopFolder(&m_lpFolder)))
	{
		m_lpFolder = NULL;
		return;
	}

	if (nFolder != CSIDL_DESKTOP)
	{
		CXTShellPidl::CShellMalloc lpMalloc;
		LPSHELLFOLDER lpFolder = NULL;

		if (lpMalloc)
		{
			LPITEMIDLIST pidlFolder = NULL;
			if (SUCCEEDED(::SHGetSpecialFolderLocation(NULL, nFolder, &pidlFolder)))
			{
				m_lpFolder->BindToObject(pidlFolder, 0, IID_IShellFolder, (LPVOID*)&lpFolder);

				lpMalloc.Free(pidlFolder);
			}
		}

		m_lpFolder->Release();
		m_lpFolder = lpFolder;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTShellPidl

CXTShellPidl::CXTShellPidl()
{
	m_bShowShellLinkIcons = TRUE;
}

CXTShellPidl::~CXTShellPidl()
{

}

/////////////////////////////////////////////////////////////////////////////
// Functions that deal with PIDLs

LPITEMIDLIST CXTShellPidl::IDLFromPath(LPCTSTR strPath)
{
	USES_CONVERSION;

	IShellFolder *psfDeskTop = NULL;
	LPITEMIDLIST  pidlPath = NULL;

	if (FAILED(::SHGetDesktopFolder(&psfDeskTop)))
	{
		return NULL;
	}

	// Get the folders IDL
	psfDeskTop->ParseDisplayName(NULL, NULL,
		T2OLE((TCHAR*)strPath), NULL, &pidlPath, NULL);

	psfDeskTop->Release();

	return pidlPath;
}

LPITEMIDLIST CXTShellPidl::OneUpPIDL(LPITEMIDLIST pidlPath)
{
	if (pidlPath)
	{
		// Get the last item
		LPITEMIDLIST pidlLast = GetLastITEM(pidlPath);
		// if not a NULL
		if (pidlLast)
		{
			// don't attempt to go any lower than 1 IDL
			if (pidlLast == pidlPath) return pidlPath;
			// set its cb to 0
			pidlLast->mkid.cb = 0;
		}
	}

	return pidlPath;
}
CString CXTShellPidl::OneUpPATH(const CString& path)
{
	// Make a string buffer
	TCHAR newPath[MAX_PATH];
	ZeroMemory(newPath, sizeof(newPath));

	if (path.GetLength())
	{
		LPITEMIDLIST pidlPath = IDLFromPath(path);
		if (OneUpPIDL(pidlPath))
		{
			SHGetPathFromIDList(pidlPath, newPath);
		}
	}

	return CString(newPath);

}
LPITEMIDLIST CXTShellPidl::GetLastITEM(LPITEMIDLIST pidl)
{
	LPSTR lpMem = NULL;
	LPITEMIDLIST pidlLast = NULL;
	if (pidl)
	{
		lpMem = (LPSTR)pidl;
		// walk the list until we find a null
		while (*lpMem != 0)
		{
			if (LPITEMIDLIST(lpMem)->mkid.cb == 0) break;// maybe ?

			pidlLast = (LPITEMIDLIST)lpMem;
			lpMem += LPITEMIDLIST(lpMem)->mkid.cb;
		}
	}

	return pidlLast;
}

LPITEMIDLIST CXTShellPidl::CopyPidlItem(LPITEMIDLIST pidl, UINT nItem)
{
	if (!pidl)
		return NULL;

	while (nItem--)
	{
		if (pidl->mkid.cb == 0)
			return NULL;

		pidl = GetNextPidlItem(pidl);
	}

	int nSize = pidl->mkid.cb + sizeof(pidl->mkid.cb);
	LPITEMIDLIST pidlCopy = CreatePidl(CShellMalloc(), nSize);

	if (pidlCopy)
	{
		ZeroMemory(pidlCopy, nSize);
		MEMCPY_S(pidlCopy, pidl, pidl->mkid.cb);
	}

	return pidlCopy;
}

LPITEMIDLIST CXTShellPidl::CopyIDList(LPITEMIDLIST pidl)
{
	LPITEMIDLIST pidlCopy = NULL;
	UINT bytes = 0;
	bytes = GetPidlItemCount(pidl);
	pidlCopy = CreatePidl(CShellMalloc(), bytes);

	if (pidlCopy)
	{
		ZeroMemory(pidlCopy, bytes);
		MEMCPY_S(pidlCopy, pidl, bytes);
	}

	return pidlCopy;
}

LPITEMIDLIST CXTShellPidl::GetNextPidlItem(LPCITEMIDLIST pidl)
{
	LPSTR lpMem = (LPSTR)pidl;
	lpMem += pidl->mkid.cb;
	return (LPITEMIDLIST)lpMem;
}

UINT CXTShellPidl::GetPidlCount(LPCITEMIDLIST pidl)
{
	UINT nCount = 0;
	if (pidl)
	{
		while (pidl->mkid.cb)
		{
			++nCount;
			pidl = GetNextPidlItem(pidl);
		}
	}

	return nCount;
}

UINT CXTShellPidl::GetPidlItemCount(LPCITEMIDLIST pidl)
{
	UINT cbTotal = 0;
	if (pidl)
	{
		cbTotal += sizeof(pidl->mkid.cb);       // Null terminator
		while (pidl->mkid.cb)
		{
			cbTotal += pidl->mkid.cb;
			pidl = GetNextPidlItem(pidl);
		}
	}

	return cbTotal;
}

LPITEMIDLIST CXTShellPidl::CreatePidl(LPMALLOC lpMalloc, UINT cbSize)
{
	if (!lpMalloc)
		return NULL;

	// zero-init for external task allocate
	LPITEMIDLIST pidl = (LPITEMIDLIST)lpMalloc->Alloc(cbSize);
	if (pidl)
	{
		memset(pidl, 0, cbSize);
	}

	return pidl;
}

void CXTShellPidl::FreePidl(LPITEMIDLIST pidl)
{
	if (!pidl)
		return;

	CShellMalloc lpMalloc;

	if (lpMalloc)
	{
		lpMalloc.Free(pidl);
	}
}

BOOL CXTShellPidl::ComparePidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, LPSHELLFOLDER pShellFolder /*NULL*/)
{
	if (pidl1 == NULL || pidl2 == NULL)
		return FALSE;

	BOOL bLPCreated = FALSE;
	if (pShellFolder == NULL)
	{
		if (FAILED(::SHGetDesktopFolder(&pShellFolder)))
			return FALSE;
		bLPCreated = TRUE;
	}

	HRESULT hr = pShellFolder->CompareIDs(0, pidl1, pidl2);

	if (bLPCreated)
	{
		pShellFolder->Release();
	}

	return ((short)hr) == 0;
}

LPITEMIDLIST CXTShellPidl::ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	return ConcatPidls(CShellMalloc(), pidl1, pidl2);
}

LPITEMIDLIST CXTShellPidl::ConcatPidls(LPMALLOC lpMalloc, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	LPITEMIDLIST pidlNew;
	UINT cb1;
	UINT cb2;

	// May be NULL
	if (pidl1)
	{
		cb1 = GetPidlItemCount(pidl1) - sizeof(pidl1->mkid.cb);
	}
	else
	{
		cb1 = 0;
	}

	cb2 = GetPidlItemCount(pidl2);
	pidlNew = CreatePidl(lpMalloc, cb1 + cb2);

	if (pidlNew)
	{
		if (pidl1)
		{
			MEMCPY_S(pidlNew, pidl1, cb1);
		}

		MEMCPY_S(((LPSTR)pidlNew) + cb1, pidl2, cb2);
	}

	return pidlNew;
}

LPITEMIDLIST CXTShellPidl::DuplicateItem(LPMALLOC lpMalloc, LPITEMIDLIST lpi)
{
	LPITEMIDLIST lpiTemp = (LPITEMIDLIST)lpMalloc->Alloc(lpi->mkid.cb + sizeof(lpi->mkid.cb));

	MEMCPY_S((PVOID)lpiTemp, (CONST VOID *)lpi, lpi->mkid.cb + sizeof(lpi->mkid.cb));

	return lpiTemp;
}

BOOL CXTShellPidl::GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, CString& strFriendlyName)
{
	STRRET str;
	if (lpsf->GetDisplayNameOf(lpi, dwFlags, &str) == NOERROR)
	{
		switch (str.uType)
		{
		case STRRET_WSTR:
			{
				strFriendlyName = str.pOleStr;

				CShellMalloc pMalloc;
				if (pMalloc) pMalloc.Free(str.pOleStr);
			}
			break;

		case STRRET_OFFSET:
			{
				LPSTR lpszOffset = (LPSTR)lpi + str.uOffset;
				strFriendlyName = lpszOffset;
			}
			break;

		case STRRET_CSTR:
			strFriendlyName = (LPSTR)str.cStr;
			break;

		default:
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

LPITEMIDLIST CXTShellPidl::GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi)
{
	USES_CONVERSION;

	CString strBuff;
	LPSHELLFOLDER lpsfDeskTop;
	LPITEMIDLIST  lpifq;
	ULONG ulEaten, ulAttribs;

	if (!GetName(lpsf, lpi, SHGDN_FORPARSING, strBuff))
	{
		return NULL;
	}

	if (FAILED(::SHGetDesktopFolder(&lpsfDeskTop)))
	{
		return NULL;
	}

	if (FAILED(lpsfDeskTop->ParseDisplayName(NULL, NULL, T2OLE((LPTSTR)(LPCTSTR)strBuff),
		&ulEaten, &lpifq, &ulAttribs)))
	{
		lpsfDeskTop->Release();
		return NULL;
	}

	lpsfDeskTop->Release();
	return lpifq;
}

// Static variables used for passing data to the subclassing wndProc

WNDPROC        CXTShellPidl::m_pOldWndProc = NULL;  // regular window proc
LPCONTEXTMENU2 CXTShellPidl::m_pIContext2 = NULL;  // active shell context menu

BOOL CXTShellPidl::ShowContextMenu(HWND hwnd, LPSHELLFOLDER lpsfParent, LPITEMIDLIST lpi, LPPOINT lppt)
{
	return ShowContextMenu(hwnd, lpsfParent, (LPCITEMIDLIST*)&lpi, 1, lppt);
}

void CXTShellPidl::OnShowContextMenu(int /*idCmd*/, CMINVOKECOMMANDINFO& /*cmi*/)
{
	// handle in derived class.
}

BOOL CXTShellPidl::ShowContextMenu(HWND hwnd, LPSHELLFOLDER lpsfParent, LPCITEMIDLIST* lpi, int nCount, LPPOINT lppt)
{
	LPCONTEXTMENU       lpcm;
	CMINVOKECOMMANDINFO cmi;
	BOOL                bSuccess = TRUE;
	int                 cmType; // "version" # of context menu

	// assume that psfFolder and pidl are valid
	HRESULT hr = GetSHContextMenu(lpsfParent, lpi, nCount, (void**)&lpcm, &cmType);

	if (SUCCEEDED(hr))
	{
		HMENU hMenu = ::CreatePopupMenu();

		if (hMenu != NULL)
		{
			hr = lpcm->QueryContextMenu(hMenu, 0, 1, 0x7fff, CMF_EXPLORE);
			if (SUCCEEDED(hr))
			{
				// install the subclassing "hook", for versions 2 or 3
				if (cmType > 1)
				{
					m_pOldWndProc = (WNDPROC)
					SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)HookWndProc);
					m_pIContext2 = (LPCONTEXTMENU2)lpcm; // cast ok for ICMv3
				}
				else
					m_pOldWndProc = NULL;

				int idCmd = ::TrackPopupMenu(hMenu,
					TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON,
					lppt->x, lppt->y, 0, hwnd, NULL);

				if (m_pOldWndProc) // restore old wndProc
					SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)m_pOldWndProc);

				if (idCmd != 0)
				{
					cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
					cmi.fMask = 0;
					cmi.hwnd = hwnd;
					cmi.lpVerb = (LPCSTR)MAKEINTRESOURCE(idCmd-1);
					cmi.lpParameters = NULL;
					cmi.lpDirectory = NULL;
					cmi.nShow = SW_SHOWNORMAL;
					cmi.dwHotKey = 0;
					cmi.hIcon = NULL;

					if (SUCCEEDED(lpcm->InvokeCommand(&cmi)))
					{
						OnShowContextMenu(idCmd-1, cmi);
					}
				}
				m_pIContext2 = NULL; // prevents accidental use
			}
			else
			{
				bSuccess = FALSE;
			}

			::DestroyMenu(hMenu);
		}
		else
			bSuccess = FALSE;

		lpcm->Release();
	}
	else
	{
		CString strMessage;
		strMessage.Format(_T("GetUIObjectOf failed! hr=%lx"), hr);
		AfxMessageBox(strMessage);
		bSuccess = FALSE;
	}

	return bSuccess;
}

int CXTShellPidl::GetItemIcon(LPITEMIDLIST lpi, UINT uFlags)
{
	SHFILEINFO sfi;
	::SHGetFileInfo((TCHAR*)lpi, 0, &sfi,
		sizeof(SHFILEINFO), uFlags);
	return sfi.iIcon;
}

HRESULT CXTShellPidl::GetSHContextMenu(LPSHELLFOLDER psfFolder, LPCITEMIDLIST* localPidl, int nCount, void** ppCM, int* pcmType)
{
	*ppCM = NULL;
	LPCONTEXTMENU pICv1 = NULL; // plain version

	// try to obtain the lowest possible IContextMenu
	HRESULT hr = psfFolder->GetUIObjectOf(NULL, nCount, localPidl,
		IID_IContextMenu, NULL, (void**)&pICv1);

	// try to obtain a higher level pointer, first 3 then 2
	if (pICv1)
	{
		hr = pICv1->QueryInterface(IID_IContextMenu3, ppCM);

		if (NOERROR == hr)
			*pcmType = 3;

		else
		{
			hr = pICv1->QueryInterface(IID_IContextMenu2, ppCM);
			if (NOERROR == hr) *pcmType = 2;
		}

		// free initial "v1.0" interface
		if (*ppCM)
			pICv1->Release();

		// no higher version supported
		else
		{
			*pcmType = 1;
			*ppCM = pICv1;
			hr = NOERROR; // never mind the query failures, this'll do
		}
	}

	return hr;
}

LRESULT CALLBACK CXTShellPidl::HookWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	UINT  uItem;
	TCHAR szBuf[MAX_PATH];

	switch (msg)
	{
	case WM_DRAWITEM:
	case WM_MEASUREITEM:
		{
			if (wp)
				break; // not menu related
		}

	case WM_INITMENUPOPUP:
		{
			m_pIContext2->HandleMenuMsg(msg, wp, lp);
			return (msg == WM_INITMENUPOPUP ? 0 : TRUE); // handled
		}

	case WM_MENUSELECT:
		{
			// if this is a shell item, get it's descriptive text
			uItem = (UINT) LOWORD(wp);

			if (0 == (MF_POPUP & HIWORD(wp)) && uItem >= 1 && uItem <= 0x7fff)
			{
				CFrameWnd * pWnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd));
				if (!pWnd)
					return 0;

				// set the status bar text
				if (SUCCEEDED(m_pIContext2->GetCommandString(uItem-1, GCS_HELPTEXT,
					NULL, (LPSTR)szBuf, _countof(szBuf))))

				{
					pWnd->SetMessageText(szBuf);
				}
				else
				{
					pWnd->SetMessageText(_T(""));
				}

				return 0;
			}
			break;
		}

	default:
		break;
	}

	// for all untreated messages, call the original wndproc
	return ::CallWindowProc(m_pOldWndProc, hWnd, msg, wp, lp);
}

int CXTShellPidl::TreeViewCompareProc(LPARAM lparam1, LPARAM lparam2, LPARAM /*lparamSort*/)
{
	XT_TVITEMDATA* lptvid1 = (XT_TVITEMDATA*)lparam1;
	XT_TVITEMDATA* lptvid2 = (XT_TVITEMDATA*)lparam2;

	HRESULT hr = lptvid1->lpsfParent->CompareIDs(0, lptvid1->lpi, lptvid2->lpi);
	if (FAILED(hr))
	{
		return 0;
	}

	return (short)SCODE_CODE(GetScode(hr));
}

int CXTShellPidl::ListViewCompareProc(LPARAM lparam1, LPARAM lparam2, LPARAM lparamSort)
{
	BOOL bAscending = (BOOL)lparamSort;

	XT_LVITEMDATA* lplvid1 = (XT_LVITEMDATA*)lparam1;
	XT_LVITEMDATA* lplvid2 = (XT_LVITEMDATA*)lparam2;

	HRESULT hr = lplvid1->lpsfParent->CompareIDs(0,
		bAscending ? lplvid1->lpi : lplvid2->lpi,
		bAscending ? lplvid2->lpi : lplvid1->lpi);

	if (FAILED(hr))
	{
		return 0;
	}

	return (short)SCODE_CODE(GetScode(hr));
}

void CXTShellPidl::MapShellFlagsToItemAttributes(CTreeCtrl* pTreeCtrl, HTREEITEM hItem, DWORD dwAttributes)
{
	// Display with 'cut' feedback if the item is ghosted (about to be moved to a different location or a hidden one)
	if (dwAttributes & SFGAO_GHOSTED)
	{
		pTreeCtrl->SetItemState(hItem, TVIS_CUT, TVIS_CUT);
	}
	else
	{
		pTreeCtrl->SetItemState(hItem, 0, TVIS_CUT);
	}

	UINT nImage; // assign proper overlay image (link, share)
	if ((dwAttributes & SFGAO_LINK) && m_bShowShellLinkIcons)
	{
		nImage = INDEXTOOVERLAYMASK(2);
	}
	else
	if (dwAttributes & SFGAO_SHARE)
	{
		nImage = INDEXTOOVERLAYMASK(1);
	}
	else
	{
		nImage = 0; // no overlay
	}
	// NB: we use here TVIS_OVERLAYMASK (as for the tree control) though
	// the list view control also calls this function and LVIS_OVERLAYMASK
	// should be used in that case. Reason for that is both of them refer
	// to results of INDEXTOOVERLAYMASK macro and hence they have to be the same,
	// namely INDEXTOOVERLAYMASK(15)
	pTreeCtrl->SetItemState(hItem, nImage, TVIS_OVERLAYMASK);
}

void CXTShellPidl::MapShellFlagsToItemAttributes(CListCtrl* pListCtrl, int iItem, DWORD dwAttributes)
{
	// Display with 'cut' feedback if the item is ghosted (about to be moved to a different location or a hidden one)
	if (dwAttributes & SFGAO_GHOSTED)
	{
		pListCtrl->SetItemState(iItem, LVIS_CUT, LVIS_CUT);
	}
	else
	{
		pListCtrl->SetItemState(iItem, 0, LVIS_CUT);
	}

	UINT nImage; // assign proper overlay image (link, share)
	if ((dwAttributes & SFGAO_LINK) && m_bShowShellLinkIcons)
	{
		nImage = INDEXTOOVERLAYMASK(2);
	}
	else
	if (dwAttributes & SFGAO_SHARE)
	{
		nImage = INDEXTOOVERLAYMASK(1);
	}
	else
	{
		nImage = 0; // no overlay
	}
	// NB: we use here TVIS_OVERLAYMASK (as for the tree control) though
	// the list view control also calls this function and LVIS_OVERLAYMASK
	// should be used in that case. Reason for that is both of them refer
	// to results of INDEXTOOVERLAYMASK macro and hence they have to be the same,
	// namely INDEXTOOVERLAYMASK(15)
	pListCtrl->SetItemState(iItem, nImage, TVIS_OVERLAYMASK);
}
