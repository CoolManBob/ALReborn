// XTPCustomizeCommandsPage.cpp : implementation of the CXTPCustomizeCommandsPage class.
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
#include "Resource.h"

#include "Common/XTPResourceManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPControls.h"
#include "XTPControl.h"
#include "XTPControlPopup.h"
#include "XTPPaintManager.h"
#include "XTPCustomizeTools.h"
#include "XTPCustomizeSheet.h"
#include "XTPCommandBar.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"
#include "XTPCustomizeCommandsPage.h"
#include "XTPMouseManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

XTP_COMMANDBARS_CATEGORYINFO::XTP_COMMANDBARS_CATEGORYINFO(LPCTSTR _strCategory, CXTPCommandBars* pCommandBars)
{
	strCategory = _strCategory;
	pControls = new CXTPControls;
	pControls->SetCommandBars(pCommandBars);
}
XTP_COMMANDBARS_CATEGORYINFO::~XTP_COMMANDBARS_CATEGORYINFO()
{
	pControls->InternalRelease();
}



void CXTPCustomizeCommandsListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rc(lpDrawItemStruct->rcItem);
	CXTPControl* pControl = (CXTPControl*)lpDrawItemStruct->itemData;
	if (pControl)
	{
		BOOL bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED);
		CXTPCommandBars* pCommandBars = m_pCommandBars;
		pCommandBars->GetPaintManager()->DrawListBoxControl(pDC, pControl, rc, bSelected, TRUE, pCommandBars);
	}
}

BEGIN_MESSAGE_MAP(CXTPCustomizeCommandsListBox, CListBox)
	//{{AFX_MSG_MAP(CXTPCustomizeCommandsPage)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


LRESULT CXTPCustomizeCommandsListBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			CPaintDC dc(this);
			CXTPClientRect rc(this);
			CXTPBufferDC memDC(dc, rc);

			m_pCommandBars->GetPaintManager()->DrawListBoxControl(&memDC, NULL, CXTPClientRect(this), FALSE, TRUE, m_pCommandBars);

			return CListBox::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
		}

	case WM_ERASEBKGND:
		CDC* pDC = CDC::FromHandle((HDC)wParam);

		m_pCommandBars->GetPaintManager()->DrawListBoxControl(pDC, NULL, CXTPClientRect(this), FALSE, TRUE, m_pCommandBars);
		return TRUE;
	}

	return CListBox::WindowProc(message, wParam, lParam);
}


void CXTPCustomizeCommandsListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CXTPCommandBars* pCommandBars = m_pCommandBars;
	ASSERT(pCommandBars);

	pCommandBars->SetDragControl(NULL);

	BOOL bOutside;
	int nIndex = (int)ItemFromPoint(point, bOutside);
	CXTPControl* pControl = NULL;
	if (!bOutside && m_bDragable)
	{
		if (SetCurSel(nIndex) != LB_ERR)
		{
			// Already selected, start drag operation
			pControl = (CXTPControl*)GetItemData(nIndex);

			// since we are eating the message, if the selection has changed, notify the owner.
			CWnd* pWndOwner = GetOwner();
			if (pWndOwner)
			{
				SetFocus();

				pWndOwner->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),
					LBN_SELCHANGE), (LPARAM)m_hWnd);
			}

			// Special case for New Menu
			if (pControl && pControl->GetCommandBar() &&
				pControl->GetCommandBar()->GetControlCount() == 0)
			{
				pControl = pControl->Clone(TRUE);

				CXTPCustomizeDropSource* pDropSource = pCommandBars->GetDropSource();
				pDropSource->DoDragDrop(pControl, TRUE);

				pControl->InternalRelease();
				return;
			}

			if (pControl)
			{

				CXTPCustomizeDropSource* pDropSource = pCommandBars->GetDropSource();
				pDropSource->DoDragDrop(pControl, TRUE);

				return;
			}
		}
	}

	// process as a click first
	CListBox::OnLButtonDown(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
// CXTPCustomizeCommandsPage property page

IMPLEMENT_DYNCREATE(CXTPCustomizeCommandsPage, CPropertyPage)

CXTPCustomizeCommandsPage::CXTPCustomizeCommandsPage(CXTPCustomizeSheet* pSheet)
	: CPropertyPage(XTP_IDD_PAGE_COMMANDS), m_pSheet(pSheet)
{
	//{{AFX_DATA_INIT(CXTPCustomizeCommandsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CXTPCustomizeCommandsPage::~CXTPCustomizeCommandsPage()
{
	for (int i = 0; i < m_arrCategories.GetSize(); i++)
	{
		delete m_arrCategories[i];
	}
}

void CXTPCustomizeCommandsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTPCustomizeCommandsPage)
	DDX_Control(pDX, XTP_IDC_LIST_COMMANDS, m_lstCommands);
	DDX_Control(pDX, XTP_IDC_LIST_CATEGORIES, m_lstCategories);
	//}}AFX_DATA_MAP
}

XTP_COMMANDBARS_CATEGORYINFO* CXTPCustomizeCommandsPage::FindCategory(LPCTSTR strCategory) const
{
	for (int i = 0; i < m_arrCategories.GetSize(); i++)
	{
		if (m_arrCategories[i]->strCategory.Compare(strCategory) == 0)
			return m_arrCategories[i];
	}
	return NULL;
}

XTP_COMMANDBARS_CATEGORYINFO* CXTPCustomizeCommandsPage::GetCategoryInfo(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_arrCategories.GetSize())
		return m_arrCategories[nIndex];
	return NULL;
}

BOOL CXTPCustomizeCommandsPage::AddCategory(LPCTSTR strCategory, CMenu* pMenu, BOOL bListSubItems)
{
	CXTPControls* pCategoryControls = InsertCategory(strCategory);

	int nCount = pMenu->GetMenuItemCount();

	for (int i = 0; i < nCount; i++)
	{
		if (pMenu->GetMenuItemID(i) > 0)
		{
			CXTPControlPopup* pControl = DYNAMIC_DOWNCAST(CXTPControlPopup,
				pCategoryControls->AddMenuItem(pMenu, i));

			if (pControl && bListSubItems)
			{
				CXTPControls* pControls = pControl->GetCommandBar()->GetControls();
				for (int j = 0; j < pControls->GetCount(); j++)
				{
					pCategoryControls->AddClone(pControls->GetAt(j));
				}
			}
		}
	}

	return TRUE;
}

BOOL CXTPCustomizeCommandsPage::_LoadToolBar(CXTPCommandBar* pCommandBar, UINT nIDResource)
{
	LPCTSTR lpszResourceName = MAKEINTRESOURCE(nIDResource);

	struct CToolBarData
	{
		WORD wVersion;
		WORD wWidth;
		WORD wHeight;
		WORD wItemCount;
		WORD* items()
		{ return (WORD*)(this + 1); }
	};

	ASSERT(lpszResourceName != NULL);


	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;

	ASSERT(pData->wVersion == 1);
	ASSERT(pData->wWidth == 16);

	int i;
	UINT* pItems = new UINT[pData->wItemCount];

	for (i = 0; i < pData->wItemCount; i++)
		pItems[i] = pData->items()[i];

	BOOL bResult = pCommandBar->SetButtons(pItems, pData->wItemCount);

	delete[] pItems;

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return bResult;
}

BOOL CXTPCustomizeCommandsPage::AddToolbarCategory(LPCTSTR strCategory, UINT nIDResource)
{
	CXTPControls* pCategoryControls = InsertCategory(strCategory);
	ASSERT(pCategoryControls);

	CXTPCommandBar* pCommandBar = (CXTPCommandBar*)CXTPCommandBar::CreateObject();
	pCommandBar->SetCommandBars(m_pSheet->GetCommandBars());

	if (!_LoadToolBar(pCommandBar, nIDResource))
	{
		pCommandBar->InternalRelease();
		return FALSE;
	}

	for (int i = 0; i < pCommandBar->GetControlCount(); i++)
	{
		pCategoryControls->AddClone(pCommandBar->GetControl(i));
	}
	pCommandBar->InternalRelease();

	return TRUE;
}


BOOL CXTPCustomizeCommandsPage::AddCategories(UINT nIDResource, BOOL bListSubItems)
{
	CMenu menu;
	if (!XTPResourceManager()->LoadMenu(&menu, nIDResource))
		return FALSE;

	int nCount = menu.GetMenuItemCount();

	for (int i = 0; i < nCount; i++)
	{
		CString strCategory;
		if (menu.GetMenuString(i, strCategory, MF_BYPOSITION) > 0)
		{
			CMenu* pMenu = menu.GetSubMenu(i);
			if (pMenu)
			{
				CXTPPaintManager::StripMnemonics(strCategory);

				int iPos = strCategory.Find(_T('\t'));
				if (iPos > 0)
				{
					strCategory.ReleaseBuffer(iPos);
				}


				if (!AddCategory(strCategory, pMenu, bListSubItems))
					return FALSE;
			}
		}

	}

	return TRUE;
}

BOOL CXTPCustomizeCommandsPage::AddCategories(CXTPControls* pControls)
{
	for (int i = 0; i < pControls->GetCount(); i++)
	{
		CXTPControl* pControl = pControls->GetAt(i);
		CString strCategory = pControl->GetCategory();

		if (!strCategory.IsEmpty())
		{
			CXTPControls* pCategoryControls = InsertCategory(strCategory);
			pCategoryControls->AddClone(pControl);
		}
	}
	return TRUE;
}


BOOL CXTPCustomizeCommandsPage::InsertBuiltInMenus(UINT nIDResourceMenu, UINT nIDResourceCategory, int nIndex)
{
	CMenu menu;
	if (!XTPResourceManager()->LoadMenu(&menu, nIDResourceMenu))
		return FALSE;

	CString strCategory;
	XTPResourceManager()->LoadString(&strCategory, nIDResourceCategory);

	ASSERT(FindCategory(strCategory) == NULL);

	XTP_COMMANDBARS_CATEGORYINFO* pInfo = new XTP_COMMANDBARS_CATEGORYINFO(strCategory, m_pSheet->GetCommandBars());
	m_arrCategories.InsertAt(nIndex == -1 ? m_arrCategories.GetSize() : nIndex, pInfo);

	int nCount = menu.GetMenuItemCount();

	for (int i = 0; i < nCount; i++)
	{
		pInfo->pControls->AddMenuItem(&menu, i);
	}
	return TRUE;
}

void CXTPCustomizeCommandsPage::InsertNewMenuCategory(UINT nIDResource, int nIndex)
{
	CString strCategory;
	XTPResourceManager()->LoadString(&strCategory, nIDResource);

	ASSERT(FindCategory(strCategory) == NULL);

	XTP_COMMANDBARS_CATEGORYINFO* pInfo = new XTP_COMMANDBARS_CATEGORYINFO(strCategory, m_pSheet->GetCommandBars());
	m_arrCategories.InsertAt(nIndex == -1 ? m_arrCategories.GetSize() : nIndex, pInfo);

	CXTPControl* pControl = pInfo->pControls->Add(xtpControlPopup, 0);
	pControl->SetCaption(strCategory);
}

CXTPControls* CXTPCustomizeCommandsPage::InsertCategory(LPCTSTR strCategory, int nIndex)
{
	XTP_COMMANDBARS_CATEGORYINFO* pInfo = FindCategory(strCategory);
	if (!pInfo)
	{
		pInfo = new XTP_COMMANDBARS_CATEGORYINFO(strCategory, m_pSheet->GetCommandBars());
		m_arrCategories.InsertAt(nIndex == -1 ? m_arrCategories.GetSize() : nIndex, pInfo);
	}
	return pInfo->pControls;
}

CXTPControls* CXTPCustomizeCommandsPage::GetControls(LPCTSTR strCategory)
{
	XTP_COMMANDBARS_CATEGORYINFO* pInfo = FindCategory(strCategory);
	return pInfo ? pInfo->pControls : NULL;
}


void CXTPCustomizeCommandsPage::InsertAllCommandsCategory(UINT nIDResource, int nIndex)
{
	CString strCategory;
	XTPResourceManager()->LoadString(&strCategory, nIDResource);

	ASSERT(FindCategory(strCategory) == NULL);

	XTP_COMMANDBARS_CATEGORYINFO* pInfo = new XTP_COMMANDBARS_CATEGORYINFO(strCategory, m_pSheet->GetCommandBars());

	for (int i = 0; i < m_arrCategories.GetSize(); i++)
	{
		CXTPControls* pControls = m_arrCategories[i]->pControls;
		for (int j = 0; j < pControls->GetCount(); j++)
		{
			pInfo->pControls->AddClone(pControls->GetAt(j));
		}
	}


	m_arrCategories.InsertAt(nIndex == -1 ? m_arrCategories.GetSize() : nIndex, pInfo);

}


BEGIN_MESSAGE_MAP(CXTPCustomizeCommandsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CXTPCustomizeCommandsPage)
	ON_LBN_SELCHANGE(XTP_IDC_LIST_CATEGORIES, OnCategoriesSelectionChanged)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPCustomizeCommandsPage message handlers


BOOL CXTPCustomizeCommandsPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();


	for (int i = 0; i < m_arrCategories.GetSize(); i++)
	{
		int nIndex = m_lstCategories.AddString(m_arrCategories[i]->strCategory);
		m_lstCategories.SetItemData(nIndex, i);

		if (i == 0) m_lstCategories.SetCurSel(0);
	}

	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();
	CSize sz = pCommandBars->GetPaintManager()->DrawListBoxControl(NULL, NULL, CRect(0, 0, 0, 0), FALSE, FALSE, pCommandBars);
	m_lstCommands.SetItemHeight(-1, sz.cy);

	m_lstCommands.m_pCommandBars = m_pSheet->GetCommandBars();
	m_lstCommands.m_bDragable = TRUE;

	if (pCommandBars->IsLayoutRTL())
	{
		m_lstCommands.ModifyStyleEx(0, WS_EX_LAYOUTRTL);
		m_lstCategories.ModifyStyleEx(0, WS_EX_RIGHT | WS_EX_LEFTSCROLLBAR | WS_EX_RTLREADING);
	}

	pCommandBars->GetMouseManager()->AddTrustedWindow(m_hWnd);


	OnCategoriesSelectionChanged();

	return FALSE;
}

void CXTPCustomizeCommandsPage::OnDestroy()
{
	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();

	pCommandBars->GetMouseManager()->RemoveTrustedWindow(m_hWnd);

	CPropertyPage::OnDestroy();
}

void CXTPCustomizeCommandsPage::OnCategoriesSelectionChanged()
{

	m_lstCommands.ResetContent();

	int nIndex = m_lstCategories.GetCurSel();

	if (nIndex == LB_ERR)
		return;

	XTP_COMMANDBARS_CATEGORYINFO* pInfo = GetCategoryInfo((int)m_lstCategories.GetItemData(nIndex));

	if (pInfo == NULL)
		return;

	for (int i = 0; i < pInfo->pControls->GetCount(); i++)
	{
		CXTPControl* pControl = pInfo->pControls->GetAt(i);
		m_lstCommands.SendMessage(LB_INSERTSTRING, m_lstCommands.GetCount(), (LPARAM)pControl);
	}
}
