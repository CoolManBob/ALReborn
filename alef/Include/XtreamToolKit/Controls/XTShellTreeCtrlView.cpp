// XTShellTreeCtrl.cpp : implementation file
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

#include "Common/XTPSystemHelpers.h"

#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTVC50Helpers.h"
#include "XTShellPidl.h"
#include "XTShellSettings.h"
#include "XTTreeBase.h"
#include "XTShellTreeBase.h"
#include "XTShellTreeCtrlView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTShellTreeView

CXTShellTreeView::CXTShellTreeView()
{
	m_pTreeCtrl = (CTreeCtrl_Friendly*)&GetTreeCtrl();
}

CXTShellTreeView::~CXTShellTreeView()
{

}

IMPLEMENT_DYNCREATE(CXTShellTreeView, CTreeView)

BEGIN_MESSAGE_MAP(CXTShellTreeView, CTreeView)
	ON_SHELLTREE_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTShellTreeView message handlers

void CXTShellTreeView::SelectionChanged(HTREEITEM hItem, CString strFolderPath)
{
	if (hItem != NULL && !m_bTunneling)
	{
		CDocument* pDoc = GetDocument();
		if (pDoc != NULL)
		{
			pDoc->UpdateAllViews(this, SHN_XT_TREESELCHANGE,
				(CObject*)GetTreeCtrl().GetItemData(hItem));
		}
	}

	CXTShellTreeBase::SelectionChanged(hItem, strFolderPath);

}

void CXTShellTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	DWORD dwStyle = (TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);

	if (XTOSVersionInfo()->IsWinXPOrGreater())
		dwStyle = (TVS_HASBUTTONS | TVS_SINGLEEXPAND | TVS_TRACKSELECT | TVS_SHOWSELALWAYS);

	if (InitializeTree(dwStyle))
	{
		PopulateTreeView();
	}

	// make the parent of the selected item visible if found.
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	HTREEITEM hItemParent = GetTreeCtrl().GetParentItem(hItem);

	GetTreeCtrl().EnsureVisible(hItemParent ? hItemParent : hItem);
}

void CXTShellTreeView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{
	switch (lHint)
	{
	// Update selection of tree view to the
	case SHN_XT_SELECTCHILD:
		{
			XT_LVITEMDATA*  lplvid = (XT_LVITEMDATA*)pHint;
			ASSERT(lplvid != NULL);

			LockWindowUpdate();
			if (!FindTreeItem(GetTreeCtrl().GetSelectedItem(), lplvid, FALSE))
			{
				// The folder was not found so we send back a message
				// to the listview to execute the itemid
				CDocument* pDoc = GetDocument();
				pDoc->UpdateAllViews(this, SHN_XT_NOFOLDER, (CObject*)lplvid);
			}
			UnlockWindowUpdate();
		}
		break;

	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTShellTreeCtrl

IMPLEMENT_DYNAMIC(CXTShellTreeCtrl, CTreeCtrl)

CXTShellTreeCtrl::CXTShellTreeCtrl()
{
	m_pListCtrl = NULL;
	m_bAutoInit = TRUE;
	m_pTreeCtrl = (CTreeCtrl_Friendly*)this;
}

CXTShellTreeCtrl::~CXTShellTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CXTShellTreeCtrl, CTreeCtrl)
	ON_SHELLTREE_REFLECT()
	ON_MESSAGE(XTWM_SHELL_NOTIFY, OnUpdateShell)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTShellTreeCtrl message handlers

void CXTShellTreeCtrl::SelectionChanged(HTREEITEM hItem, CString strFolderPath)
{
	if (hItem != NULL && !m_bTunneling)
	{
		CWnd* pWndNotify = (CWnd*)m_pListCtrl;
		if (!::IsWindow(pWndNotify->GetSafeHwnd()))
		{
			pWndNotify = GetOwner();
		}

		// notify list control of selection change.
		if (::IsWindow(pWndNotify->GetSafeHwnd()))
		{
			pWndNotify->SendMessage(XTWM_SHELL_NOTIFY,
				(WPARAM)SHN_XT_TREESELCHANGE,
				(LPARAM)(CObject*)GetItemData(hItem));
		}
	}
	CXTShellTreeBase::SelectionChanged(hItem, strFolderPath);
}

LRESULT CXTShellTreeCtrl::OnUpdateShell(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	// Update selection of tree view to the
	case SHN_XT_SELECTCHILD:
		{
			XT_LVITEMDATA*  lplvid = (XT_LVITEMDATA*)lParam;
			ASSERT(lplvid != NULL);

			LockWindowUpdate();
			if (!FindTreeItem(GetSelectedItem(), lplvid, FALSE) && m_pListCtrl != NULL)
			{
				// The folder was not found so we send back a message
				// to the listview to execute the itemid
				m_pListCtrl->SendMessage(XTWM_SHELL_NOTIFY,
					(WPARAM)SHN_XT_NOFOLDER,
					(LPARAM)(CObject*)lplvid);
			}
			UnlockWindowUpdate();
		}
		break;

	default:
		break;
	}

	return 0;
}

bool CXTShellTreeCtrl::Init()
{
	if (!CXTShellTreeCtrlBase::Init())
		return false;

	if (m_bAutoInit)
	{
		DWORD dwStyle = (TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);

		if (XTOSVersionInfo()->IsWinXPOrGreater())
			dwStyle = (TVS_HASBUTTONS | TVS_SINGLEEXPAND | TVS_TRACKSELECT | TVS_SHOWSELALWAYS);

		if (InitializeTree(dwStyle))
		{
			PopulateTreeView();
		}

		// make the parent of the selected item visible if found.
		HTREEITEM hItem = GetSelectedItem();
		HTREEITEM hItemParent = GetParentItem(hItem);

		EnsureVisible(hItemParent ? hItemParent : hItem);
	}

	return true;
}

void CXTShellTreeCtrl::AssociateList(CWnd* pWnd)
{
	ASSERT_VALID(pWnd); // must be a valid window.

	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		m_pListCtrl = pWnd;

		// make sure list gets updated.
		m_pListCtrl->SendMessage(XTWM_SHELL_NOTIFY,
			(WPARAM)SHN_XT_TREESELCHANGE,
			(LPARAM)(CObject*)GetItemData(GetSelectedItem()));
	}
}

void CXTShellTreeCtrl::AssociateCombo(CWnd* pWnd)
{
	CXTShellTreeBase::AssociateCombo(pWnd);

	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		CString strSelectedPath;
		GetSelectedFolderPath(strSelectedPath);

		// make sure list gets updated.
		CXTShellTreeBase::SelectionChanged(
			GetSelectedItem(), strSelectedPath);
	}
}
