// XTPCustomizeMenusPage.cpp : implementation of the CXTPCustomizeMenusPage class.
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

#include "Common/XTPDrawHelpers.h"

#include "XTPCustomizeMenusPage.h"
#include "XTPToolBar.h"
#include "XTPPopupBar.h"
#include "XTPCommandBars.h"
#include "XTPCustomizeSheet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CXTPFloatingPopupBar : public CXTPToolBar
{
	DECLARE_XTP_COMMANDBAR(CXTPFloatingPopupBar)
public:
	CXTPFloatingPopupBar()
	{
		m_barType = xtpBarTypePopup;
		SetWidth(0);
	}

};

IMPLEMENT_XTP_COMMANDBAR(CXTPFloatingPopupBar, CXTPToolBar)

/////////////////////////////////////////////////////////////////////////////
// CXTPCustomizeMenusPage property page

CXTPCustomizeMenusPage::CXTPCustomizeMenusPage(CXTPCustomizeSheet* pSheet)
	: CPropertyPage(CXTPCustomizeMenusPage::IDD), m_pSheet(pSheet)
{
	//{{AFX_DATA_INIT(CXTPCustomizeMenusPage)
	//}}AFX_DATA_INIT
	m_pContextMenu = NULL;
	m_ptContextMenu = CPoint(0, 0);
}

CXTPCustomizeMenusPage::~CXTPCustomizeMenusPage()
{
}

void CXTPCustomizeMenusPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTPCustomizeMenusPage)
	DDX_Control(pDX, XTP_IDC_STATIC_ICON, m_wndIcon);
	DDX_Control(pDX, XTP_IDC_COMBO_CONTEXTMENUS, m_cmbContextMenus);
	DDX_Control(pDX, XTP_IDC_COMBO_APPMENUS, m_cmbAppMenus);
	DDX_Control(pDX, XTP_IDC_STATIC_APPLICATIONMENUS, m_wndApplicationMenus);
	DDX_Control(pDX, XTP_IDC_STATIC_CONTEXTMENUS, m_wndContextMenus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXTPCustomizeMenusPage, CPropertyPage)
	//{{AFX_MSG_MAP(CXTPCustomizeMenusPage)
	ON_CBN_SELCHANGE(XTP_IDC_COMBO_APPMENUS, OnSelectionChangedAppmenus)
	ON_CBN_SELCHANGE(XTP_IDC_COMBO_CONTEXTMENUS, OnSelectionChangedContextmenus)
	ON_WM_DESTROY()
	ON_BN_CLICKED(XTP_IDC_BUTTON_RESETCONTEXT, OnButtonResetContextMenu)
	ON_BN_CLICKED(XTP_IDC_BUTTON_RESETAPP, OnButtonResetAppMenu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPCustomizeMenusPage message handlers


BOOL CXTPCustomizeMenusPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();

	CString strDefaultMenu;
	GetDlgItemText(XTP_IDC_STATIC_DESCRIPTION, strDefaultMenu);

	if (_tcschr(strDefaultMenu, _T('\n')) != NULL)
	{
		AfxExtractSubString(m_strDefaultMenuDesc, strDefaultMenu, 1);
		AfxExtractSubString(m_strDefaultMenu, strDefaultMenu, 0);
	}
	else
	{
		m_strDefaultMenu = m_strDefaultMenuDesc = strDefaultMenu;
	}

	int nIndex = m_cmbAppMenus.AddString(m_strDefaultMenu);
	m_cmbAppMenus.SetItemData(nIndex, 0);
	int nCurSel = 0;

	if (pCommandBars->GetMenuBar() && ((pCommandBars->GetMenuBar()->GetFlags() & xtpFlagIgnoreSetMenuMessage) == 0))
	{
		CXTPMenuBar* pMenuBar = pCommandBars->GetMenuBar();
		CXTPMenuBarMDIMenus* pMDIControls = pMenuBar->GetMDIMenus();

		POSITION pos = pMDIControls->GetStartPosition();
		CXTPMenuBarMDIMenuInfo* pInfo;
		while (pos)
		{
			pMDIControls->GetNextMenu(pos, pInfo);
			if (pInfo->m_nIDResource == pMenuBar->GetDefaultMenuResource())
				continue;

			if (!pInfo->m_strTitle.IsEmpty())
			{
				nIndex = m_cmbAppMenus.AddString(pInfo->m_strTitle);
				m_cmbAppMenus.SetItemData(nIndex, (DWORD_PTR)pInfo);

				if (pInfo->m_nIDResource == pMenuBar->GetCurrentMenuResource())
				{
					nCurSel = nIndex;
				}
			}
		}
	}

	m_cmbAppMenus.SetCurSel(nCurSel);
	OnSelectionChangedAppmenus();

	nIndex = m_cmbContextMenus.AddString(_T(""));
	m_cmbContextMenus.SetItemData(nIndex, 0);

	for (int i = 0; i < pCommandBars->GetContextMenus()->GetCount(); i++)
	{
		CXTPPopupBar* pPopupBar = (CXTPPopupBar*)pCommandBars->GetContextMenus()->GetAt(i);

		nIndex = m_cmbContextMenus.AddString(pPopupBar->GetTitle());
		m_cmbContextMenus.SetItemData(nIndex, (DWORD_PTR)pPopupBar);
	}
	m_cmbContextMenus.SetCurSel(0);
	OnSelectionChangedContextmenus();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CXTPCustomizeMenusPage::OnSelectionChangedAppmenus()
{
	int nSel = m_cmbAppMenus.GetCurSel();

	if (nSel == CB_ERR)
		return;

	CXTPMenuBar* pMenuBar = m_pSheet->GetCommandBars()->GetMenuBar();
	UINT nIDResource = pMenuBar->GetDefaultMenuResource();

	CXTPMenuBarMDIMenuInfo* pInfo = (CXTPMenuBarMDIMenuInfo*)m_cmbAppMenus.GetItemData(nSel);

	if (pInfo)
	{
		SetDlgItemText(XTP_IDC_STATIC_DESCRIPTION, pInfo->m_strDescription);
		nIDResource = pInfo->m_nIDResource;
	}
	else
	{
		SetDlgItemText(XTP_IDC_STATIC_DESCRIPTION, m_strDefaultMenuDesc);
	}

	HICON hIcon = NULL;

	if (pInfo)
	{
		hIcon = pInfo->m_hIcon;
	}
	else
	{
		HWND hWnd = m_pSheet->GetCommandBars()->GetSite()->GetSafeHwnd();
		hIcon = (HICON)::SendMessage(hWnd, WM_GETICON, ICON_BIG, 0);
		if (!hIcon) hIcon = (HICON)(ULONG_PTR)::GetClassLongPtr(hWnd, GCLP_HICON);
	}

	if (hIcon)
	{
		m_wndIcon.SetIcon(hIcon);
	}

	if (m_cmbAppMenus.GetCount() > 1)
	{
		pMenuBar->SwitchMDIMenu(nIDResource);
	}
}

void CXTPCustomizeMenusPage::CommitContextMenu()
{
	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();
	if (m_pContextMenu)
	{
		CXTPPopupBar* pPopupBar = (CXTPPopupBar*)m_pContextMenu->GetCommandBarData();

		CXTPControls* pControls = m_pContextMenu->GetControls()->Duplicate();
		pPopupBar->SetControls(pControls);

		m_ptContextMenu = CXTPWindowRect(m_pContextMenu).TopLeft();

		pCommandBars->Remove(m_pContextMenu);
		m_pContextMenu = NULL;
	}
}

void CXTPCustomizeMenusPage::OnSelectionChangedContextmenus()
{
	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();
	CommitContextMenu();

	int nSel = m_cmbContextMenus.GetCurSel();

	if (nSel < 1)
	{
		GetDlgItem(XTP_IDC_BUTTON_RESETCONTEXT)->EnableWindow(FALSE);
		return;
	}

	GetDlgItem(XTP_IDC_BUTTON_RESETCONTEXT)->EnableWindow(TRUE);
	CXTPPopupBar* pPopupBar = (CXTPPopupBar*)m_cmbContextMenus.GetItemData(nSel);


	CXTPToolBar* pContextMenu = (CXTPToolBar*)pCommandBars->Add(_T("Standard"), xtpBarTop, RUNTIME_CLASS(CXTPFloatingPopupBar));
	if (!pContextMenu)
	{
		TRACE0("Failed to create toolbar\n");
		return;
	}

	CXTPControls* pControls = pPopupBar->GetControls()->Duplicate();
	pContextMenu->SetControls(pControls);


	pContextMenu->SetBarID(pPopupBar->GetBarID());
	pContextMenu->SetTitle(pPopupBar->GetTitle());
	pContextMenu->SetCustomizeDialogPresent(FALSE);

	pContextMenu->SetPosition(xtpBarFloating);
	pContextMenu->EnableDocking(xtpFlagFloating);

	pContextMenu->OnRecalcLayout();

	CXTPWindowRect rcContext(pContextMenu);
	CXTPWindowRect rcParent(GetParent());

	CPoint pt = m_ptContextMenu;
	if (pt == CPoint(0, 0))
		pt = CPoint(rcParent.left - rcContext.Width(), rcParent.top);

	pContextMenu->MoveWindow(pt.x, pt.y, rcContext.Width(), rcContext.Height());

	m_pContextMenu = pContextMenu;

	pContextMenu->SetCommandBarData((DWORD_PTR)pPopupBar);
	pContextMenu->OnUpdateCmdUI();
}

void CXTPCustomizeMenusPage::OnDestroy()
{
	CommitContextMenu();

	if (m_cmbAppMenus.GetCount() > 1)
	{
		CXTPMenuBar* pMenuBar = m_pSheet->GetCommandBars()->GetMenuBar();
		if (pMenuBar) pMenuBar->SwitchActiveMenu();
	}

	CPropertyPage::OnDestroy();
}

void CXTPCustomizeMenusPage::OnButtonResetContextMenu()
{
	if (m_pContextMenu)
	{
		m_pContextMenu->Reset(TRUE);
	}
}

void CXTPCustomizeMenusPage::OnButtonResetAppMenu()
{
	CXTPToolBar* pMenuBar = m_pSheet->GetCommandBars()->GetMenuBar();
	pMenuBar->Reset(TRUE);
}
