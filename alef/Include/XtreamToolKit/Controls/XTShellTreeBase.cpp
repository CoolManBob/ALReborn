// XTShellTreeView.cpp : implementation file
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

#include "XTFunctions.h"
#include "XTDropSource.h"

#include "XTTreeBase.h"
#include "XTShellSettings.h"
#include "XTShellPidl.h"
#include "XTShellTreeBase.h"
#include "XTComboBoxEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTShellTreeBase

CXTShellTreeBase::CXTShellTreeBase()
: m_bTunneling(false)
{
	m_bContextMenu = TRUE;
	m_pComboBox = NULL;
	m_uFlags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS;

	if (m_shSettings.ShowAllFiles() && !m_shSettings.ShowSysFiles())
	{
		m_uFlags |= SHCONTF_INCLUDEHIDDEN;
	}

	m_nRootFolder = CSIDL_DESKTOP;
	m_bShowFiles = FALSE;
	m_bShowShellLinkIcons = FALSE;
}

CXTShellTreeBase::~CXTShellTreeBase()
{

}

/////////////////////////////////////////////////////////////////////////////
// CXTShellTreeBase message handlers

HTREEITEM CXTShellTreeBase::InsertDesktopItem(int nFolder /*= CSIDL_DESKTOP*/)
{
	HTREEITEM hItem = TVI_ROOT;

	CShellMalloc lpMalloc;

	if (!lpMalloc)
		return NULL;

	// Get ShellFolder Pidl
	LPITEMIDLIST pidlDesktop = NULL;
	if (FAILED(::SHGetSpecialFolderLocation(NULL, nFolder, &pidlDesktop)))
	{
		pidlDesktop = NULL;
	}

	// insert the desktop.
	if (pidlDesktop)
	{
		SHFILEINFO fileInfo;
		::ZeroMemory(&fileInfo, sizeof(fileInfo));

		::SHGetFileInfo((LPCTSTR)pidlDesktop, NULL, &fileInfo, sizeof(fileInfo),
			SHGFI_PIDL | SHGFI_ATTRIBUTES | SHGFI_DISPLAYNAME);

		TV_ITEM  tvi;
		tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

		// Allocate memory for ITEMDATA struct
		XT_TVITEMDATA* lptvid = (XT_TVITEMDATA*)lpMalloc.Alloc(sizeof(XT_TVITEMDATA));
		if (lptvid != NULL)
		{
			GetNormalAndSelectedIcons(pidlDesktop, &tvi);

			// Now, make a copy of the ITEMIDLIST and store the parent folders SF.
			lptvid->lpi = DuplicateItem(lpMalloc, pidlDesktop);
			lptvid->lpsfParent = NULL;
			lptvid->lpifq = ConcatPidls(lpMalloc, NULL, pidlDesktop);

			TCHAR szBuff[MAX_PATH];
			STRCPY_S(szBuff, MAX_PATH, fileInfo.szDisplayName);

			tvi.lParam = (LPARAM)lptvid;
			tvi.pszText = szBuff;
			tvi.cchTextMax = MAX_PATH;

			// Populate the TreeVeiw Insert Struct
			// The item is the one filled above.
			// Insert it after the last item inserted at this level.
			// And indicate this is a root entry.
			TV_INSERTSTRUCT tvins;
			tvins.item = tvi;
			tvins.hInsertAfter = hItem;
			tvins.hParent = hItem;

			// Add the item to the tree
			hItem = m_pTreeCtrl->InsertItem(&tvins);
		}

		if (pidlDesktop)
		{
			lpMalloc.Free(pidlDesktop);
		}
	}

	return hItem;
}


void CXTShellTreeBase::PopulateTreeView()
{
	// Get a pointer to the desktop folder.
	CShellSpecialFolder lpsfFolder(m_nRootFolder);
	if (!lpsfFolder)
		return;

	// turn off redraw and remove all tree items.
	m_pTreeCtrl->SetRedraw(FALSE);
	m_pTreeCtrl->DeleteAllItems();

	HTREEITEM hItemDesktop = InsertDesktopItem(m_nRootFolder);

	// Fill in the tree view from the root.
	InitTreeViewItems(lpsfFolder, NULL, hItemDesktop);

	// Sort the items in the tree view
	SortChildren(hItemDesktop);

	HTREEITEM hItemRoot = m_pTreeCtrl->GetRootItem();
	m_pTreeCtrl->Expand(hItemRoot, TVE_EXPAND);

	if (hItemDesktop != TVI_ROOT)
	{
		HTREEITEM hItemChild = m_pTreeCtrl->GetChildItem(hItemDesktop);
		m_pTreeCtrl->Select(hItemChild, TVGN_CARET);

		if ((::GetWindowLong(m_pTreeCtrl->m_hWnd, GWL_STYLE) & TVS_SINGLEEXPAND) == 0)
		{
			m_pTreeCtrl->Expand(hItemChild, TVE_EXPAND);
		}
	}
	else
	{
		m_pTreeCtrl->Select(hItemRoot, TVGN_CARET);
	}

	// turn on redraw and refresh tree.
	m_pTreeCtrl->SetRedraw(TRUE);
	m_pTreeCtrl->RedrawWindow();
	m_pTreeCtrl->SetFocus();
}

void CXTShellTreeBase::SetAttributes(HTREEITEM hItem, DWORD dwAttributes)
{
	MapShellFlagsToItemAttributes(m_pTreeCtrl, hItem, dwAttributes);
}

BOOL CXTShellTreeBase::InitTreeViewItems(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpifq, HTREEITEM     hParent)
{
	CWaitCursor wait; // show wait cursor.

	// Allocate a shell memory object.
	CShellMalloc lpMalloc;

	if (!lpMalloc)
		return FALSE;

	// Get the IEnumIDList object for the given folder.
	LPENUMIDLIST lpe = NULL;
	if (FAILED(lpsf->EnumObjects(::GetParent(m_pTreeCtrl->m_hWnd), m_uFlags, &lpe)))
		return FALSE;

	ULONG        ulFetched = 0;
	HTREEITEM    hPrev = NULL;
	LPITEMIDLIST lpi = NULL;

	// Enumerate through the list of folder and non-folder objects.
	while (lpe->Next(1, &lpi, &ulFetched) == S_OK)
	{
		// Create a fully qualified path to the current item
		// the SH* shell api's take a fully qualified path pidl,
		// (see GetIcon above where I call SHGetFileInfo) whereas the
		// interface methods take a relative path pidl.
		ULONG ulAttrs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_REMOVABLE;

		// Determine what type of object we have.
		lpsf->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lpi, &ulAttrs);

		// We need this next if statement so that we don't add things like
		// the MSN to our tree.  MSN is not a folder, but according to the
		// shell it has subfolders.
		if ((ulAttrs & SFGAO_FOLDER) || m_bShowFiles)
		{
			TV_ITEM  tvi;
			tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

			if ((ulAttrs & SFGAO_HASSUBFOLDER) || (m_bShowFiles && (ulAttrs & SFGAO_FOLDER)))
			{
				//This item has sub-folders, so let's put the + in the TreeView.
				//The first time the user clicks on the item, we'll populate the
				//sub-folders.
				tvi.cChildren = 1;
				tvi.mask |= TVIF_CHILDREN;
			}

			// Allocate memory for ITEMDATA struct
			CString szBuff;
			XT_TVITEMDATA* lptvid = (XT_TVITEMDATA*)lpMalloc.Alloc(sizeof(XT_TVITEMDATA));
			if (lptvid == NULL || GetName(lpsf, lpi, SHGDN_NORMAL, szBuff) == FALSE)
			{
				if (lptvid)
				{
					lpMalloc.Free(lptvid);
				}
				if (lpe)
				{
					lpe->Release();
				}
				if (lpi)
				{
					lpMalloc.Free(lpi);
				}
				return FALSE;
			}


			// Now, make a copy of the ITEMIDLIST and store the parent folders SF.
			lptvid->lpi = DuplicateItem(lpMalloc, lpi);
			lptvid->lpsfParent = lpsf;
			lptvid->lpifq = ConcatPidls(lpMalloc, lpifq, lpi);
			lpsf->AddRef();

			GetNormalAndSelectedIcons(lptvid->lpifq, &tvi);

			tvi.lParam = (LPARAM)lptvid;
			tvi.pszText = (LPTSTR)(LPCTSTR)szBuff;
			tvi.cchTextMax = 0;

			// Populate the TreeVeiw Insert Struct
			// The item is the one filled above.
			// Insert it after the last item inserted at this level.
			// And indicate this is a root entry.
			TV_INSERTSTRUCT tvins;
			tvins.item = tvi;
			tvins.hInsertAfter = hPrev;
			tvins.hParent = hParent;

			// Add the item to the tree
			hPrev = m_pTreeCtrl->InsertItem(&tvins);
			SetAttributes(hPrev, ulAttrs);
		}

		// Free the pidl that the shell gave us.
		if (lpi)
		{
			lpMalloc.Free(lpi);
			lpi = 0;
		}
	}
	if (lpi)
	{
		lpMalloc.Free(lpi);
	}

	if (lpe)
	{
		lpe->Release();
	}

	return TRUE;
}

void CXTShellTreeBase::GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem)
{
	// Note that we don't check the return value here because if GetIcon()
	// fails, then we're in big trouble...
	lptvitem->iImage = GetItemIcon(lpifq,
		SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	lptvitem->iSelectedImage = GetItemIcon(lpifq,
		SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);
}

void CXTShellTreeBase::OnFolderExpanding(NM_TREEVIEW* pNMTreeView)
{
	if (!(pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE))
	{
		// Long pointer to TreeView item data
		XT_TVITEMDATA* lptvid = (XT_TVITEMDATA*)pNMTreeView->itemNew.lParam;
		if (lptvid != NULL && lptvid->lpsfParent != NULL)
		{
			LPSHELLFOLDER lpsf = NULL;
			if (SUCCEEDED(lptvid->lpsfParent->BindToObject(lptvid->lpi,
				0, IID_IShellFolder, (LPVOID *)&lpsf)))
			{
				InitTreeViewItems(lpsf, lptvid->lpifq, pNMTreeView->itemNew.hItem);
			}

			SortChildren(pNMTreeView->itemNew.hItem);

			m_pTreeCtrl->SetItemState(pNMTreeView->itemNew.hItem, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);
		}
	}
}

HTREEITEM CXTShellTreeBase::GetContextMenu()
{
	CPoint point;
	::GetCursorPos(&point);
	m_pTreeCtrl->ScreenToClient(&point);

	TV_HITTESTINFO tvhti;
	tvhti.pt = point;
	m_pTreeCtrl->HitTest(&tvhti);

	if (tvhti.flags & (TVHT_ONITEMLABEL | TVHT_ONITEMICON))
	{
		// Long pointer to TreeView item data
		XT_TVITEMDATA*  lptvid = (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(tvhti.hItem);

		m_pTreeCtrl->ClientToScreen(&point);

		if (lptvid->lpsfParent == NULL)
		{
			LPSHELLFOLDER lpShellFolder;
			if (FAILED(::SHGetDesktopFolder(&lpShellFolder)))
			{
				return NULL;
			}

			ShowContextMenu(m_pTreeCtrl->m_hWnd,
				lpShellFolder, lptvid->lpi, &point);

			if (lpShellFolder)
			{
				lpShellFolder->Release();
			}
		}
		else
		{
			ShowContextMenu(m_pTreeCtrl->m_hWnd,
				lptvid->lpsfParent, lptvid->lpi, &point);
		}

		return tvhti.hItem;
	}

	return NULL;
}

void CXTShellTreeBase::SortChildren(HTREEITEM hParent)
{
	TV_SORTCB tvscb;
	tvscb.hParent = hParent;
	tvscb.lParam = 0;
	tvscb.lpfnCompare = TreeViewCompareProc;
	m_pTreeCtrl->SortChildrenCB(&tvscb);
}

BOOL CXTShellTreeBase::OnFolderSelected(NM_TREEVIEW* pNMTreeView, CString &strFolderPath)
{
	BOOL bRet = FALSE;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem == NULL)
		return FALSE;

	LPSHELLFOLDER lpsf = NULL;

	// Long pointer to TreeView item data
	XT_TVITEMDATA*  lptvid = (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(hItem);
	if (lptvid && lptvid->lpsfParent && lptvid->lpi)
	{
		if (SUCCEEDED(lptvid->lpsfParent->BindToObject(lptvid->lpi,
			0, IID_IShellFolder, (LPVOID*)&lpsf)))
		{
			ULONG ulAttrs = SFGAO_FILESYSTEM;

			// Determine what type of object we have.
			lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);

			if (ulAttrs & (SFGAO_FILESYSTEM))
			{
				TCHAR szBuff[MAX_PATH];
				if (::SHGetPathFromIDList(lptvid->lpifq, szBuff))
				{
					strFolderPath = szBuff;
					bRet = TRUE;
				}
			}

			if (m_pTreeCtrl->ItemHasChildren(pNMTreeView->itemNew.hItem) && !(pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE))
			{
				InitTreeViewItems(lpsf, lptvid->lpifq, pNMTreeView->itemNew.hItem);

				SortChildren(pNMTreeView->itemNew.hItem);

				m_pTreeCtrl->SetItemState(pNMTreeView->itemNew.hItem, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);
			}

			if (lpsf)
			{
				lpsf->Release();
			}
		}
	}

	return bRet;
}

BOOL CXTShellTreeBase::InitSystemImageLists()
{
	SHFILEINFO sfi;
	HIMAGELIST himlSmall = (HIMAGELIST)::SHGetFileInfo(_T("C:\\"), 0, &sfi,
		sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	if (himlSmall)
	{
		m_pTreeCtrl->SetImageList(CImageList::FromHandle(himlSmall), TVSIL_NORMAL);
		return TRUE;
	}

	return FALSE;
}

BOOL CXTShellTreeBase::GetSelectedFolderPath(CString &strFolderPath)
{
	BOOL bRet = FALSE;

	HTREEITEM hItem = m_pTreeCtrl->GetSelectedItem();
	if (hItem != NULL)
	{
		// Long pointer to TreeView item data
		XT_TVITEMDATA*  lptvid = (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(hItem);
		if (lptvid && lptvid->lpsfParent && lptvid->lpi)
		{
			LPSHELLFOLDER lpsf = NULL;
			if (SUCCEEDED(lptvid->lpsfParent->BindToObject(lptvid->lpi,
				0, IID_IShellFolder, (LPVOID *)&lpsf)))
			{
				ULONG ulAttrs = SFGAO_FILESYSTEM;

				// Determine what type of object we have.
				lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);

				if (ulAttrs & (SFGAO_FILESYSTEM))
				{
					TCHAR szBuff[MAX_PATH];
					if (::SHGetPathFromIDList(lptvid->lpifq, szBuff))
					{
						strFolderPath = szBuff;
						bRet = TRUE;
					}
				}
				if (lpsf)
				{
					lpsf->Release();
				}
			}
		}
	}

	return bRet;
}

BOOL CXTShellTreeBase::FindTreeItem(HTREEITEM hItem, XT_LVITEMDATA* lplvid, BOOL bRecursively)
{
	if (lplvid == NULL)
	{
		return FALSE;
	}

	if (!bRecursively)
	{
		hItem = m_pTreeCtrl->GetChildItem(hItem);
	}

	while (hItem)
	{
		// Long pointer to TreeView item data
		XT_TVITEMDATA*  lptvid = (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(hItem);
		if (lptvid)
		{
			if (SCODE_CODE(GetScode(lplvid->lpsfParent->CompareIDs(
				0, lplvid->lpi, lptvid->lpi))) == 0)
			{
				m_pTreeCtrl->EnsureVisible(hItem);
				m_pTreeCtrl->SelectItem(hItem);
				return TRUE;
			}
		}

		if (bRecursively)
		{
			HTREEITEM hNextItem = m_pTreeCtrl->GetChildItem(hItem);
			if (hNextItem)
			{
				if (FindTreeItem(hNextItem, lplvid))
				{
					return TRUE;
				}
			}
		}

		hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
	}

	return FALSE;
}

void CXTShellTreeBase::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	OnFolderExpanding(pNMTreeView);
	*pResult = 0;
}

void CXTShellTreeBase::SelectionChanged(HTREEITEM hItem, CString strFolderPath)
{
	if (hItem != NULL && !m_bTunneling)
	{
		if (!m_pComboBox || !::IsWindow(m_pComboBox->m_hWnd))
			return;

		// update combo box association.
		if (m_pComboBox->IsKindOf(RUNTIME_CLASS(CXTComboBoxEx)))
		{
			CXTComboBoxEx* pComboBoxEx = DYNAMIC_DOWNCAST(CXTComboBoxEx, m_pComboBox);
			ASSERT_VALID(pComboBoxEx);

			int nFound = CB_ERR;
			int nIndex;
			for (nIndex = 0; nIndex < pComboBoxEx->GetCount(); ++nIndex)
			{
				CString strText;
				pComboBoxEx->GetLBText(nIndex, strText);

				if (strFolderPath.Compare(strText) == 0)
				{
					nFound = nIndex;
					pComboBoxEx->SetCurSel(nIndex);
					break;
				}
			}

			if (nFound == CB_ERR)
			{
				HTREEITEM hti = m_pTreeCtrl->GetSelectedItem();
				ASSERT(hti);

				if (strFolderPath.IsEmpty())
				{
					strFolderPath = m_pTreeCtrl->GetItemText(hti);
				}

				int nImage, nSelectedImage;
				m_pTreeCtrl->GetItemImage(hti, nImage, nSelectedImage);

				pComboBoxEx->InsertItem(0, strFolderPath, 0, nImage, nImage);
				pComboBoxEx->SetCurSel(0);
				pComboBoxEx->SetItemDataPtr(0, hti);
			}
		}

		else if (m_pComboBox->IsKindOf(RUNTIME_CLASS(CComboBox)))
		{
			CComboBox* pComboBox = DYNAMIC_DOWNCAST(CComboBox, m_pComboBox);
			ASSERT_VALID(pComboBox);

			int nFound = pComboBox->FindStringExact(-1, strFolderPath);
			if (nFound == CB_ERR)
			{
				HTREEITEM hti = m_pTreeCtrl->GetSelectedItem();
				ASSERT(hti);

				pComboBox->InsertString(0, strFolderPath);
				pComboBox->SetCurSel(0);
				pComboBox->SetItemDataPtr(0, (HTREEITEM)hti);
			}
			else
			{
				pComboBox->SetCurSel(nFound);
			}
		}
	}
}
void CXTShellTreeBase::OnDeleteTreeItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	XT_TVITEMDATA* lptvid = (XT_TVITEMDATA*)pNMTreeView->itemOld.lParam;
	if (lptvid != NULL)
	{
		CShellMalloc lpMalloc;

		if (lptvid->lpi)
		{
			lpMalloc.Free(lptvid->lpi);
			lptvid->lpi = NULL;
		}
		if (lptvid->lpsfParent)
		{
			lptvid->lpsfParent->Release();
			lptvid->lpsfParent = NULL;
		}
		if (lptvid->lpifq)
		{
			lpMalloc.Free(lptvid->lpifq);
			lptvid->lpifq = NULL;
		}

		lpMalloc.Free(lptvid);
	}

	*pResult = 0;
}

void CXTShellTreeBase::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CString strFolderPath;
	OnFolderSelected(pNMTreeView, strFolderPath);

	if (strFolderPath.IsEmpty())
	{
		strFolderPath = m_pTreeCtrl->GetItemText(pNMTreeView->itemNew.hItem);
	}

	// currently selected TreeItem
	SelectionChanged(m_pTreeCtrl->GetSelectedItem(), strFolderPath);

	*pResult = 0;
}

void CXTShellTreeBase::OnRclick(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	// Display the shell context menu.
	if (m_bContextMenu == TRUE)
	{
		HTREEITEM hItem = GetContextMenu();
		if (hItem != NULL)
		{
			// TODO: Additional error handling.
		}
	}

	*pResult = 0;
}


BOOL CXTShellTreeBase::InitializeTree(DWORD dwStyle/*= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT*/)
{
	if (m_pTreeCtrl->GetImageList(TVSIL_NORMAL) == NULL)
	{
		// Initialize the image list for the list view and populate it.
		if (!InitSystemImageLists())
			return FALSE;

		// Set the style for the tree control.
		m_pTreeCtrl->ModifyStyle(0, dwStyle);

		// Make sure multi-select mode is disabled.
		EnableMultiSelect(FALSE);

		return TRUE;
	}

	return FALSE;
}

void CXTShellTreeBase::InitTreeNode(HTREEITEM hItem, XT_TVITEMDATA* lptvid)
{
	m_pTreeCtrl->SetRedraw(FALSE);
	if (lptvid)
	{
		LPSHELLFOLDER lpsf = NULL;
		if (SUCCEEDED(lptvid->lpsfParent->BindToObject(lptvid->lpi,
			0, IID_IShellFolder, (LPVOID *)&lpsf)))
		{
			InitTreeViewItems(lpsf, lptvid->lpifq, hItem);
		}

		SortChildren(hItem);
	}

	m_pTreeCtrl->SetRedraw(TRUE);
}

HTREEITEM CXTShellTreeBase::SearchTree(HTREEITEM hItem, LPCITEMIDLIST pABSPidl)
{
	XT_TVITEMDATA *pItem = NULL;
	HTREEITEM hChildItem = m_pTreeCtrl->GetChildItem(hItem);

	CShellSpecialFolder pShellFolder;
	if (!pShellFolder)
		return NULL;

	while (hChildItem)
	{
		// Get the pidl that is stored in the tree node
		pItem = (XT_TVITEMDATA *)m_pTreeCtrl->GetItemData(hChildItem);

		// See if it matches the one we're looking for
		if (ComparePidls(pItem->lpifq, pABSPidl, pShellFolder))
		{
			m_pTreeCtrl->Select(hChildItem, TVGN_CARET);

			// Ensure that we are expanded
			UINT uState = m_pTreeCtrl->GetItemState(hChildItem, TVIS_EXPANDEDONCE);
			if (!(uState & TVIS_EXPANDEDONCE))
			{
				InitTreeNode(hChildItem, (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(hChildItem));
				m_pTreeCtrl->SetItemState(hChildItem, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);
			}
			break;
		}

		// Didn't compare... try next one
		hChildItem = m_pTreeCtrl->GetNextSiblingItem(hChildItem);
	}

	return hChildItem;
}

BOOL CXTShellTreeBase::TunnelTree(CString strFindPath)
{
	if (strFindPath.IsEmpty())
		return false;

	m_bTunneling = true;

	BOOL bLock = m_pTreeCtrl->LockWindowUpdate();
	BOOL bFound = false;

	LPITEMIDLIST pidlPath;

	// Attempt to get the folder's item list
	pidlPath = IDLFromPath(strFindPath);

	// If it is NULL then see if it is one of the special folders
	if (pidlPath == NULL)
	{
		// These are the ones we care about.
		const int nCSIDLMax = 0x001b;
		for (int i = 0; i <= nCSIDLMax; i++)
		{
			LPITEMIDLIST pidlSpecialPath = NULL;
			if (::SHGetSpecialFolderLocation(NULL, i, &pidlSpecialPath) != NOERROR)
				continue;

			SHFILEINFO fileInfo;
			::ZeroMemory(&fileInfo, sizeof(fileInfo));
			::SHGetFileInfo((LPCTSTR)pidlSpecialPath, NULL, &fileInfo, sizeof(fileInfo),
				SHGFI_PIDL | SHGFI_DISPLAYNAME);

			CString cs = fileInfo.szDisplayName;
			if (cs.CompareNoCase(strFindPath) == 0)
			{
				// Found the pidl for the special folder
				pidlPath = pidlSpecialPath;
				break;
			}
		}
	}

	if (pidlPath != NULL)
	{
		// Now work through the list and tree nodes until we compare
		int nItems = GetPidlCount(pidlPath);
		LPITEMIDLIST pPartPidl;
		LPITEMIDLIST pABSPidl = NULL;
		HTREEITEM hItem = m_pTreeCtrl->GetRootItem();
		bFound = false;

		// Loop through all the parts and see if we can find a match in the tree.  It should
		// be there unless something got added to the namespace after we built the tree last
		// and we didn't catch it, but that's pretty unlikely.
		for (int i = 0; i < nItems; i++)
		{
			pPartPidl = CopyPidlItem(pidlPath, i);
			pABSPidl = ConcatPidls(pABSPidl, pPartPidl);
			FreePidl(pPartPidl);

			hItem = SearchTree(hItem, pABSPidl);
			if (!hItem)
				break; // Our partial path should still found
		}

		// If it was found the final path should compare to the full path entered
		// and the hItem should be set to the place in the tree where the path ends
		if (hItem && (nItems == 0 || ComparePidls(pidlPath, pABSPidl, NULL)))
		{
			m_pTreeCtrl->Select(hItem, TVGN_CARET);
			// Ensure that we are expanded
			UINT uState = m_pTreeCtrl->GetItemState(hItem, TVIS_EXPANDEDONCE);
			if (!(uState & TVIS_EXPANDEDONCE))
			{
				InitTreeNode(hItem, (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(hItem));
				m_pTreeCtrl->SetItemState(hItem, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);
			}
			bFound = true;
		}

		// Clean up
		if (pidlPath)
			FreePidl(pidlPath);

		if (pABSPidl)
			FreePidl(pABSPidl);
	}
	else
	{
		// Must not be a special folder (stand alone) or a path that ParseDisplayName() doesn't recognize so look through tree
		// directly to see if we can find it that way.
		// Start at Desktop ...
		HTREEITEM hItemRoot = m_pTreeCtrl->GetRootItem();
		TCHAR* pszNext = NULL;
		TCHAR szBuff[MAX_PATH];

		STRCPY_S(szBuff, MAX_PATH, strFindPath);

		TCHAR* lpszContext = 0;
		pszNext = STRTOK_S(szBuff, _T("\\/"), &lpszContext);

		bFound = false;

		if (pszNext != NULL)
		{
			CString strItemText(m_pTreeCtrl->GetItemText(hItemRoot));

			// Are we looking from desktop?
			if (strItemText.CompareNoCase(pszNext) == 0)
				pszNext = STRTOK_S(NULL, _T("\\/"), &lpszContext);

			hItemRoot = m_pTreeCtrl->GetChildItem(hItemRoot);

			// Ensure that we are expanded
			UINT uState = m_pTreeCtrl->GetItemState(hItemRoot, TVIS_EXPANDEDONCE);
			if (!(uState & TVIS_EXPANDEDONCE))
			{
				InitTreeNode(hItemRoot, (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(hItemRoot));
				m_pTreeCtrl->SetItemState(hItemRoot, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);
			}

			while (pszNext && hItemRoot)
			{
				strItemText = m_pTreeCtrl->GetItemText(hItemRoot);
				if (strItemText.CompareNoCase(pszNext) == 0)
				{
					// Found it
					// We know this was successful - expand at this new root
					m_pTreeCtrl->Select(hItemRoot, TVGN_CARET);

					// Ensure that we are expanded
					uState = m_pTreeCtrl->GetItemState(hItemRoot, TVIS_EXPANDEDONCE);
					if (!(uState & TVIS_EXPANDEDONCE))
					{
						InitTreeNode(hItemRoot, (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(hItemRoot));
						m_pTreeCtrl->SetItemState(hItemRoot, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);
					}
					pszNext = STRTOK_S(NULL, _T("\\/"), &lpszContext);
					if (pszNext)
					{
						// Move down a level
						hItemRoot = m_pTreeCtrl->GetChildItem(hItemRoot);
					}
				}
				else
				{
					hItemRoot = m_pTreeCtrl->GetNextSiblingItem(hItemRoot);
				}
			}
		}
	}

	if (bLock)
	{
		m_pTreeCtrl->UnlockWindowUpdate();
	}

	m_bTunneling = false;

	// make sure list gets updated.
	SelectionChanged(m_pTreeCtrl->GetSelectedItem(), strFindPath);
	return bFound;
}



BOOL CXTShellTreeBase::GetFolderItemPath(HTREEITEM hItem, CString &strFolderPath)
{
	BOOL bRet = FALSE;

	// Long pointer to TreeView item data
	XT_TVITEMDATA*  lptvid = (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(hItem);
	if (lptvid && lptvid->lpsfParent && lptvid->lpi)
	{
		LPSHELLFOLDER lpsf = NULL;
		if (SUCCEEDED(lptvid->lpsfParent->BindToObject(lptvid->lpi,
			0, IID_IShellFolder, (LPVOID *)&lpsf)))
		{
			ULONG ulAttrs = SFGAO_FILESYSTEM;

			// Determine what type of object we have.
			lptvid->lpsfParent->GetAttributesOf(1,
				(const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);

			if (ulAttrs & (SFGAO_FILESYSTEM))
			{
				TCHAR szBuff[MAX_PATH];
				if (::SHGetPathFromIDList(lptvid->lpifq, szBuff))
				{
					strFolderPath = szBuff;
					bRet = TRUE;
				}
			}
		}
		if (lpsf)
		{
			lpsf->Release();
		}
	}

	return bRet;
}

//-----------------------------------------------------------------------------
// FUNCTION: XTFuncPathFindNextComponent()
//-----------------------------------------------------------------------------

CString CXTShellTreeBase::PathFindNextComponent(const CString& pszPath)
{
	// Find the path delimiter
	int nIndex = pszPath.Find(_T('\\'));

	if (nIndex == -1)
		return _T("");

	return pszPath.Mid(nIndex + 1);
}

void CXTShellTreeBase::PopulateTree(LPCTSTR lpszPath)
{
	CString strFolder = lpszPath;
	CString strNextFolder;
	CString strPath;

	CShellMalloc lpMalloc;
	if (!lpMalloc)
		return;

	// Get a pointer to the desktop folder.
	LPSHELLFOLDER lpSF = NULL;
	if (FAILED(::SHGetDesktopFolder(&lpSF)))
		return;

	LPITEMIDLIST lpIDL = NULL;
	USES_CONVERSION;

	// turn off redraw and remove all tree items.
	m_pTreeCtrl->SetRedraw(FALSE);
	m_pTreeCtrl->DeleteAllItems();

	do
	{
		// Get the Next Component
		strNextFolder = PathFindNextComponent(strFolder);
		if (!strNextFolder.IsEmpty())
		{
			strPath = strFolder.Left(strFolder.GetLength() -
				strNextFolder.GetLength());
		}
		else
		{
			strPath = strFolder;
			strNextFolder.Empty();
		}

		// Get ShellFolder Pidl
		ULONG eaten;
		if (FAILED(lpSF->ParseDisplayName(NULL, NULL, T2OLE((TCHAR*)(LPCTSTR)strPath),
			&eaten, &lpIDL, NULL)))
		{
			break;
		}

		LPSHELLFOLDER lpSF2 = NULL;
		if (FAILED(lpSF->BindToObject(lpIDL, 0, IID_IShellFolder, (LPVOID*)&lpSF2)))
		{
			break;
		}

		lpMalloc.Free(lpIDL);

		// Release the Parent Folder pointer.
		lpSF->Release();

		// Change Folder Info
		lpSF = lpSF2;
		strFolder = strNextFolder;
	}
	while (!strNextFolder.IsEmpty());

	// get the base folders item ide list.
	lpIDL = IDLFromPath(lpszPath);

	SHFILEINFO fileInfo;
	::ZeroMemory(&fileInfo, sizeof(fileInfo));

	::SHGetFileInfo((LPCTSTR)lpIDL, NULL, &fileInfo, sizeof(fileInfo),
		SHGFI_PIDL | SHGFI_ATTRIBUTES | SHGFI_DISPLAYNAME);

	TV_ITEM  tvi;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	// Allocate memory for ITEMDATA struct
	XT_TVITEMDATA* lptvid = (XT_TVITEMDATA*)lpMalloc.Alloc(sizeof(XT_TVITEMDATA));
	if (lptvid != NULL)
	{
		HTREEITEM hItem = TVI_ROOT;

		// get the normal and selected icons for the path.
		GetNormalAndSelectedIcons(lpIDL, &tvi);

		// Now, make a copy of the ITEMIDLIST and store the parent folders SF.
		lptvid->lpi = DuplicateItem(lpMalloc, lpIDL);
		lptvid->lpsfParent = NULL;
		lptvid->lpifq = ConcatPidls(lpMalloc, NULL, lpIDL);

		TCHAR szBuff[MAX_PATH];
		STRCPY_S(szBuff, MAX_PATH, fileInfo.szDisplayName);

		tvi.lParam = (LPARAM)lptvid;
		tvi.pszText = szBuff;
		tvi.cchTextMax = MAX_PATH;

		// Populate the TreeView Insert Struct
		// The item is the one filled above.
		// Insert it after the last item inserted at this level.
		// And indicate this is a root entry.
		TV_INSERTSTRUCT tvins;
		tvins.item = tvi;
		tvins.hInsertAfter = hItem;
		tvins.hParent = hItem;

		// Add the item to the tree
		hItem = m_pTreeCtrl->InsertItem(&tvins);

		// insert child items.
		InitTreeViewItems(lpSF, lpIDL, hItem);

		// Sort the items in the tree view
		SortChildren(TVI_ROOT);

		// expand parent.
		m_pTreeCtrl->Expand(hItem, TVE_EXPAND);
	}

	// turn on redraw and refresh tree.
	m_pTreeCtrl->SetRedraw(TRUE);
	m_pTreeCtrl->RedrawWindow();

	lpMalloc.Free(lpIDL);

	if (lpSF)
	{
		lpSF->Release();
	}
}

void CXTShellTreeBase::AssociateCombo(CWnd* pWnd)
{
	ASSERT_VALID(pWnd); // must be a valid window.

	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		m_pComboBox = pWnd;

		if (m_pComboBox->IsKindOf(RUNTIME_CLASS(CXTComboBoxEx)))
		{
			SHFILEINFO sfi;
			HIMAGELIST hImageList = (HIMAGELIST)::SHGetFileInfo(_T("C:\\"), 0, &sfi,
				sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

			if (hImageList != NULL)
			{
				CXTComboBoxEx* pComboBoxEx = DYNAMIC_DOWNCAST(CXTComboBoxEx, m_pComboBox);
				ASSERT_VALID(pComboBoxEx);
				pComboBoxEx->SetImageList(CImageList::FromHandle(hImageList));
			}
		}
	}
}

BOOL CXTShellTreeBase::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTShellTreeBase::OnPaint()
{
	CPaintDC dc(m_pTreeCtrl);
	DoPaint(dc, FALSE);
}

void CXTShellTreeBase::OnDragDrop(NM_TREEVIEW* pNMTreeView)
{
	// Long pointer to ListView item data
	XT_TVITEMDATA*  lplvid = (XT_TVITEMDATA*)m_pTreeCtrl->GetItemData(pNMTreeView->itemNew.hItem);
	ASSERT(lplvid);

	if (lplvid && lplvid->lpsfParent)
	{
		LPDATAOBJECT lpdo;

		HRESULT hResult = lplvid->lpsfParent->GetUIObjectOf(AfxGetMainWnd()->m_hWnd, 1,
			(const struct _ITEMIDLIST**)&lplvid->lpi, IID_IDataObject, 0, (LPVOID*)&lpdo);

		if (SUCCEEDED(hResult))
		{
			LPDROPSOURCE lpds = new CXTDropSource();
			ASSERT(lpds != NULL);

			DWORD dwEffect;
			::DoDragDrop(lpdo, lpds,
				DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK, &dwEffect);

			lpdo->Release();
			lpds->Release();
		}
	}
}
