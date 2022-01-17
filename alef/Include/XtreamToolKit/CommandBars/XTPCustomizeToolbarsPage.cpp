// XTPCustomizeToolbarsPage.cpp : implementation of the CXTPCustomizeToolbarsPage class.
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

#include "XTPCustomizeToolbarsPage.h"
#include "XTPCustomizeSheet.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"
#include "XTPCustomizeTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPCustomizeToolbarsPageCheckListBox
CXTPCustomizeToolbarsPageCheckListBox::CXTPCustomizeToolbarsPageCheckListBox()
{
	m_themeHelper.OpenThemeData(NULL, L"BUTTON");

	SIZE size;
	if (m_themeHelper.IsAppThemeReady() && SUCCEEDED(m_themeHelper.GetThemePartSize(NULL, BP_CHECKBOX, 0, NULL, TS_TRUE, &size)))
	{
		m_sizeCheck = size;
	}
	else
	{
		m_sizeCheck = CSize(13, 13);
		m_themeHelper.CloseThemeData();
	}
}

CXTPCustomizeToolbarsPageCheckListBox::~CXTPCustomizeToolbarsPageCheckListBox()
{
}

void CXTPCustomizeToolbarsPageCheckListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// You must override DrawItem and MeasureItem for LBS_OWNERDRAWVARIABLE
	ASSERT((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS)) ==
		(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS));

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		int cyItem = GetItemHeight(lpDrawItemStruct->itemID);
		BOOL fDisabled = !IsWindowEnabled() || !IsEnabled(lpDrawItemStruct->itemID);

		COLORREF newTextColor = fDisabled ?
			RGB(0x80, 0x80, 0x80) : GetSysColor(COLOR_WINDOWTEXT);  // light gray
		COLORREF oldTextColor = pDC->SetTextColor(newTextColor);

		COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

		if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
		{
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		}

		if (m_cyText == 0)
			VERIFY(cyItem >= CalcMinimumItemHeight());

		CString strText;
		GetText(lpDrawItemStruct->itemID, strText);

		pDC->ExtTextOut(lpDrawItemStruct->rcItem.left + 2,
			lpDrawItemStruct->rcItem.top + max(0, (cyItem - m_cyText) / 2),
			ETO_OPAQUE, &(lpDrawItemStruct->rcItem), strText, (int)strText.GetLength(), NULL);

		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);
	}

	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));
}


void CXTPCustomizeToolbarsPageCheckListBox::PreDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	DRAWITEMSTRUCT drawItem;
	memcpy(&drawItem, lpDrawItemStruct, sizeof(DRAWITEMSTRUCT));

	if ((((LONG)drawItem.itemID) >= 0) &&
		((drawItem.itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) != 0))
	{
		int cyItem = GetItemHeight(drawItem.itemID);

		CDC* pDC = CDC::FromHandle(drawItem.hDC);

		int nCheck = GetCheck(drawItem.itemID);

		CRect rectCheck = drawItem.rcItem;
		rectCheck.left += 1;
		rectCheck.top += 1 + max(0, (cyItem - m_sizeCheck.cy) / 2);
		rectCheck.right = rectCheck.left + m_sizeCheck.cx;
		rectCheck.bottom = rectCheck.top + m_sizeCheck.cy;

		CRect rectItem = drawItem.rcItem;
		rectItem.right = rectItem.left + m_sizeCheck.cx + 2;
		CRect rectCheckBox = OnGetCheckPosition(rectItem, rectCheck);

		if (m_themeHelper.IsAppThemeReady())
		{
			m_themeHelper.DrawThemeBackground(pDC->m_hDC, BP_CHECKBOX,
				nCheck ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL, &rectCheckBox, NULL);
		}
		else
		{
			ASSERT(rectCheck.IntersectRect(rectItem, rectCheckBox));
			ASSERT((rectCheck == rectCheckBox) && (rectCheckBox.Size() == m_sizeCheck));

			pDC->DrawFrameControl(rectCheckBox, DFC_BUTTON, DFCS_BUTTONCHECK | (nCheck ? DFCS_CHECKED : 0));
		}
	}

	drawItem.rcItem.left = drawItem.rcItem.left + m_sizeCheck.cx + 3;

	DrawItem(&drawItem);
}

BOOL CXTPCustomizeToolbarsPageCheckListBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam,
	LRESULT* pResult)
{
	if (message == WM_DRAWITEM)
	{
		ASSERT(pResult == NULL);       // no return value expected
		PreDrawItem((LPDRAWITEMSTRUCT)lParam);
		return TRUE;
	}

	return CCheckListBox::OnChildNotify(message, wParam, lParam, pResult);
}


/////////////////////////////////////////////////////////////////////////////
// CXTPCustomizeToolbarsPage property page

IMPLEMENT_DYNCREATE(CXTPCustomizeToolbarsPage, CPropertyPage)

CXTPCustomizeToolbarsPage::CXTPCustomizeToolbarsPage(CXTPCustomizeSheet* pSheet)
	: CPropertyPage(CXTPCustomizeToolbarsPage::IDD), m_pSheet(pSheet)
{
	//{{AFX_DATA_INIT(CXTPCustomizeToolbarsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CXTPCustomizeToolbarsPage::~CXTPCustomizeToolbarsPage()
{
}

void CXTPCustomizeToolbarsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTPCustomizeToolbarsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, XTP_IDC_LIST_TOOLBARS, m_checkList);
	DDX_Control(pDX, XTP_IDC_BUTTON_RENAME, m_btnRename);
	DDX_Control(pDX, XTP_IDC_BUTTON_NEW, m_btnNew);
	DDX_Control(pDX, XTP_IDC_BUTTON_DELETE, m_btnDelete);
	DDX_Control(pDX, XTP_IDC_BUTTON_RESET, m_btnReset);
	//}}AFX_DATA_MAP
}


BOOL CXTPCustomizeToolbarsPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if (m_pSheet->GetCommandBars()->IsLayoutRTL())
	{
		m_checkList.ModifyStyleEx(0, WS_EX_LAYOUTRTL);
	}

	RefreshToolbarsList();

	return FALSE;
}

void CXTPCustomizeToolbarsPage::RefreshToolbarsList()
{
	int nIndexSel = 0;
	int nIndex = m_checkList.GetCurSel();
	if (nIndex != LB_ERR)
	{
		nIndexSel = (int)m_checkList.GetItemData(nIndex);
	}
	int nTopIndex = m_checkList.GetTopIndex();

	m_checkList.ResetContent();

	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();
	int nCount = pCommandBars->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPToolBar* pToolBar = pCommandBars->GetAt(i);

		if (!pToolBar->IsCustomizeDialogPresent())
			continue;

		if (!pToolBar->IsCloseable() && !pToolBar->IsVisible())
			continue;

		nIndex = m_checkList.AddString(pToolBar->GetTitle());
		m_checkList.SetCheck(nIndex, pToolBar->IsVisible());
		m_checkList.SetItemData(nIndex, i);

		if (i == nIndexSel) m_checkList.SetCurSel(nIndex);
	}

	if (m_checkList.GetTopIndex() != nTopIndex)
		m_checkList.SetTopIndex(nTopIndex);

	OnSelectionChanged();
}


BEGIN_MESSAGE_MAP(CXTPCustomizeToolbarsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CXTPCustomizeToolbarsPage)
		// NOTE: the ClassWizard will add message map macros here
	ON_LBN_SELCHANGE(XTP_IDC_LIST_TOOLBARS, OnSelectionChanged)
	ON_CLBN_CHKCHANGE(XTP_IDC_LIST_TOOLBARS, OnChkChange)
	ON_BN_CLICKED(XTP_IDC_BUTTON_NEW, OnNewToolbar)
	ON_BN_CLICKED(XTP_IDC_BUTTON_RENAME, OnRenameToolbar)
	ON_BN_CLICKED(XTP_IDC_BUTTON_DELETE, OnDeleteToolbar)
	ON_BN_CLICKED(XTP_IDC_BUTTON_RESET, OnResetToolbar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPCustomizeToolbarsPage message handlers


void CXTPCustomizeToolbarsPage::OnSelectionChanged()
{
	int nIndex = m_checkList.GetCurSel();

	if (nIndex != LB_ERR)
	{
		CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();
		int i = (int)m_checkList.GetItemData(nIndex);

		if (i >= 0 && i < pCommandBars->GetCount())
		{
			CXTPToolBar* pToolBar = pCommandBars->GetAt(i);

			BOOL bBuiltIn = pToolBar->IsBuiltIn();
			m_btnReset.EnableWindow(bBuiltIn);
			m_btnRename.EnableWindow(!bBuiltIn);
			m_btnDelete.EnableWindow(!bBuiltIn);

		}
	}
	else
	{
		m_btnReset.EnableWindow(FALSE);
		m_btnRename.EnableWindow(FALSE);
		m_btnDelete.EnableWindow(FALSE);
	}
}

void CXTPCustomizeToolbarsPage::OnChkChange()
{
	int nIndex = m_checkList.GetCurSel();
	if (nIndex == LB_ERR)
		return;

	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();
	int i = (int)m_checkList.GetItemData(nIndex);
	if (i >= 0 && i < pCommandBars->GetCount())
	{
		CXTPToolBar* pToolBar = pCommandBars->GetAt(i);
		if (!pToolBar->IsCloseable())
		{
			m_checkList.SetCheck(nIndex, 1);

		}
		else
		{
			BOOL bVisible = m_checkList.GetCheck(nIndex);
			pToolBar->SetVisible(bVisible);
		}
	}
	RefreshToolbarsList();
}

void CXTPCustomizeToolbarsPage::OnNewToolbar()
{
	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();

	CXTPNewToolbarDlg dlg(m_pSheet, pCommandBars);
	if (dlg.DoModal() == IDOK)
	{
		CXTPToolBar* pToolBar = pCommandBars->AddCustomBar(dlg.m_strToolbar, dlg.m_nNewID);

		RefreshToolbarsList();

		pCommandBars->GetSite()->SendMessage(WM_XTP_CUSTOMIZATION_ADDTOOLBAR, 0, (LPARAM)pToolBar);
	}
}

void CXTPCustomizeToolbarsPage::OnRenameToolbar()
{
	int nIndex = m_checkList.GetCurSel();
	if (nIndex == LB_ERR)
		return;

	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();

	int i = (int)m_checkList.GetItemData(nIndex);
	if (i < 0 || i >= pCommandBars->GetCount())
		return;

	CXTPToolBar* pToolBar = pCommandBars->GetAt(i);

	if (pToolBar->IsBuiltIn())
		return;


	CXTPNewToolbarDlg dlg(m_pSheet, pCommandBars, pToolBar);
	if (dlg.DoModal() == IDOK)
	{
		pToolBar->SetTitle(dlg.m_strToolbar);

		m_checkList.DeleteString(nIndex);
		nIndex = m_checkList.InsertString(nIndex, pToolBar->GetTitle());
		m_checkList.SetCheck(nIndex, pToolBar->IsVisible());
		m_checkList.SetItemData(nIndex, i);
		m_checkList.SetCurSel(nIndex);

	}
}


void CXTPCustomizeToolbarsPage::OnDeleteToolbar()
{
	int nIndex = m_checkList.GetCurSel();
	if (nIndex == LB_ERR)
		return;

	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();

	int i = (int)m_checkList.GetItemData(nIndex);
	if (i < 0 || i >= pCommandBars->GetCount())
		return;

	CXTPToolBar* pToolBar = pCommandBars->GetAt(i);

	if (pToolBar->IsBuiltIn())
		return;

	CString strMessage;

	CString strDelete;
	VERIFY(XTPResourceManager()->LoadString(&strDelete, XTP_IDS_CONFIRM_DELETE));
	strMessage.Format(strDelete, (LPCTSTR)pToolBar->GetTitle());

	if (m_pSheet->MessageBox(strMessage, MB_ICONWARNING | MB_OKCANCEL) == IDOK)
	{
		pCommandBars->GetSite()->SendMessage(WM_XTP_CUSTOMIZATION_DELETETOOLBAR, 0, (LPARAM)pToolBar);

		pCommandBars->Remove(pToolBar);
		RefreshToolbarsList();
	}
}

void CXTPCustomizeToolbarsPage::OnResetToolbar()
{
	int nIndex = m_checkList.GetCurSel();
	if (nIndex == LB_ERR)
		return;

	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();

	int i = (int)m_checkList.GetItemData(nIndex);
	if (i < 0 || i >= pCommandBars->GetCount())
		return;

	CXTPToolBar* pToolBar = pCommandBars->GetAt(i);

	if (!pToolBar->IsBuiltIn())
		return;

	CString strMessage, strTitle = pToolBar->GetTitle(), strReset;

	VERIFY(XTPResourceManager()->LoadString(&strReset, XTP_IDS_CONFIRM_RESET));
	strMessage.Format(strReset, (LPCTSTR)strTitle);

	if (m_pSheet->MessageBox(strMessage, MB_ICONWARNING | MB_OKCANCEL) != IDOK)
		return;

	pToolBar->Reset(FALSE);
	RefreshToolbarsList();
}
