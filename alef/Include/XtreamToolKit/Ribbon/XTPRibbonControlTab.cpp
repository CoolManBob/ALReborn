// XTPRibbonTab.cpp: implementation of the CXTPRibbonTab class.
//
// This file is a part of the XTREME RIBBON MFC class library.
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

#include "Common/XTPImageManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPToolTipContext.h"

#include "CommandBars/XTPCommandBars.h"
#include "CommandBars/XTPMouseManager.h"
#include "CommandBars/XTPPopupBar.h"

#include "XTPRibbonControlTab.h"
#include "XTPRibbonBar.h"
#include "XTPRibbonTab.h"
#include "XTPRibbonTheme.h"
#include "XTPRibbonGroups.h"
#include "XTPRibbonGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


class CXTPRibbonTabPopupToolBar : public CXTPPopupToolBar, public CXTPRibbonScrollableBar
{
	DECLARE_DYNAMIC(CXTPRibbonTabPopupToolBar)
public:
	CXTPRibbonTabPopupToolBar(CXTPRibbonTab* pRibbonTab)
	{

		InitScrollableBar(this);

		m_pRibbonBar = pRibbonTab->GetRibbonBar();
		m_pRibbonTab = (CXTPRibbonTab*)pRibbonTab->GetRuntimeClass()->CreateObject();
		m_pRibbonTab->m_pParent = this;
		m_pRibbonTab->Copy(pRibbonTab);

		m_pRibbonTab->SetRect(pRibbonTab->GetRect());

		m_bTrackOnHover = FALSE;
		SetFlags(xtpFlagSmartLayout);

		EnableAnimation(m_pRibbonBar->IsAnimationEnabled());
	}

	~CXTPRibbonTabPopupToolBar()
	{
		m_pRibbonTab->OnRemoved();
		m_pRibbonTab->InternalRelease();
	}

	CRect GetGroupsRect() const
	{
		return m_rcGroups;
	}

	void OnGroupsScroll(BOOL bScrollLeft)
	{
		int nGroupsScrollPos = m_pRibbonTab->GetGroups()->GetScrollPos();

		SetPopuped(-1);

		if (bScrollLeft)
		{
			nGroupsScrollPos -= 40;
			if (nGroupsScrollPos < 0)
				nGroupsScrollPos = 0;
		}
		else
		{
			nGroupsScrollPos += 40;
		}

		if (nGroupsScrollPos != m_nGroupsScrollPos)
		{
			m_nGroupsScrollPos = nGroupsScrollPos;
			OnRecalcLayout();
		}
	}

	void EnsureVisible(CXTPControl* pControl)
	{
		if (pControl->GetRibbonGroup() &&
			(m_pControlScrollGroupsLeft->IsVisible() || m_pControlScrollGroupsRight->IsVisible()))
		{
			ShowScrollableRect(m_pRibbonTab->GetGroups(), pControl->GetRect());
		}
	}

	void CreateKeyboardTips()
	{
		CreateGroupKeyboardTips(m_pRibbonTab);
	}

	CSize CalcDynamicLayout(int, DWORD /*nMode*/)
	{
		CRect rcRibbonBar;
		m_pRibbonBar->GetWindowRect(rcRibbonBar);

		CRect rcRibbonTab = m_pRibbonTab->GetRect();
		m_pRibbonBar->ClientToScreen(rcRibbonTab);

		CRect rcWork = XTPMultiMonitor()->GetWorkArea(rcRibbonTab);

		int nHeight = m_pRibbonBar->CalcGroupsHeight();
		int nWidth = rcRibbonBar.Width();

		m_ptPopup.y = m_rcExclude.bottom;
		m_ptPopup.x = rcRibbonBar.left;

		if (m_ptPopup.x < rcWork.left)
		{
			nWidth -= rcWork.left - m_ptPopup.x;
			m_ptPopup.x = rcWork.left;
		}

		if (m_ptPopup.x + nWidth > rcWork.right)
		{
			nWidth = rcWork.right - m_ptPopup.x - 4;
		}

		if (m_ptPopup.y + nHeight > rcWork.bottom)
		{
			m_ptPopup.y = m_rcExclude.top - nHeight;
		}

		CClientDC dc(this);
		CXTPFontDC font(&dc, GetPaintManager()->GetCommandBarFont(this));

		UpdateShortcuts();

		m_rcGroups.SetRect(0, 0, nWidth, nHeight);
		m_pRibbonTab->GetGroups()->CalcDynamicSize(&dc, m_rcGroups.Width(), LM_COMMIT,
			CRect(m_rcGroups.left + 4, m_rcGroups.top + 3, 6, 3));

		m_rcTearOffGripper.SetRectEmpty();


		return CSize(nWidth, nHeight);
	}

	void DrawCommandBar(CDC* pDC, CRect rcClipBox)
	{
		CXTPRibbonTheme* pPaintManager = (CXTPRibbonTheme*)GetPaintManager();

		pPaintManager->FillTabPopupToolBar(pDC, this);
		pPaintManager->FillGroupRect(pDC, m_pRibbonTab, m_rcGroups);
		pDC->SetBkMode(TRANSPARENT);

		CXTPRibbonGroups* pGroups = m_pRibbonTab->GetGroups();
		for (int i = 0; i < pGroups->GetCount(); i++)
		{
			CXTPRibbonGroup* pGroup = pGroups->GetAt(i);

			if (pGroup->IsVisible() && CRect().IntersectRect(rcClipBox, pGroup->GetRect()))
			{
				pGroup->Draw(pDC, rcClipBox);
			}
		}

		if (m_pControlScrollGroupsLeft->IsVisible())
		{
			m_pControlScrollGroupsLeft->Draw(pDC);
		}

		if (m_pControlScrollGroupsRight->IsVisible())
		{
			m_pControlScrollGroupsRight->Draw(pDC);
		}
	}

	virtual CSize GetButtonSize() const
	{
		return m_pRibbonBar->GetButtonSize();
	}
	virtual CSize GetIconSize() const
	{
		return m_pRibbonBar->GetIconSize();

	}

	virtual CRect CalculatePopupRect(CSize sz)
	{
		return CRect(m_ptPopup, sz);
	}

public:
	CXTPRibbonTab* m_pRibbonTab;
	CXTPRibbonBar* m_pRibbonBar;
	CRect m_rcGroups;

};

IMPLEMENT_DYNAMIC(CXTPRibbonTabPopupToolBar, CXTPPopupToolBar)

CXTPRibbonScrollableBar* AFX_CDECL GetScrollableBar(CXTPCommandBar* pCommandBar)
{
	if (pCommandBar->IsRibbonBar())
		return (CXTPRibbonScrollableBar*)(CXTPRibbonBar*)(pCommandBar);

	if (pCommandBar->IsKindOf(RUNTIME_CLASS(CXTPRibbonTabPopupToolBar)))
		return (CXTPRibbonScrollableBar*)(CXTPRibbonTabPopupToolBar*)(pCommandBar);

	return 0;
}

IMPLEMENT_XTP_CONTROL(CXTPRibbonControlTab, CXTPControl)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPRibbonControlTab::CXTPRibbonControlTab()
{
	m_bAllowReorder = FALSE;
	m_bFocused = FALSE;
	SetFlags(xtpFlagNoMovable);
}

CXTPRibbonControlTab::~CXTPRibbonControlTab()
{
}

CXTPRibbonTab* CXTPRibbonControlTab::FindTab(int nID) const
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		CXTPRibbonTab* pTab = (CXTPRibbonTab*)GetItem(i);
		if (pTab->GetID() == nID)
			return pTab;
	}
	return NULL;
}

CXTPRibbonTab* CXTPRibbonControlTab::GetTab(int nIndex) const
{
	return (CXTPRibbonTab*)GetItem(nIndex);
}

void CXTPRibbonControlTab::Reposition()
{
	if (!m_pParent->GetSafeHwnd())
		return;

	m_pParent->OnRecalcLayout();
}

BOOL CXTPRibbonControlTab::IsMouseLocked() const
{
	return GetRibbonBar()->GetCommandBars()->GetMouseManager()->IsMouseLocked();
}

CXTPTabPaintManager* CXTPRibbonControlTab::GetPaintManager() const
{
	return GetRibbonBar()->GetTabPaintManager();
}

void CXTPRibbonControlTab::ShowPopupBar(BOOL bKeyboard)
{
	CXTPRibbonBar* pRibbonBar = GetRibbonBar();

	if (pRibbonBar->m_nPopuped == m_nIndex)
	{
		pRibbonBar->m_nPopuped = -1;
		if (m_pCommandBar) m_pCommandBar->SetTrackingMode(FALSE);
	}

	pRibbonBar->SetTrackingMode(TRUE, FALSE);
	pRibbonBar->SetPopuped(m_nIndex, bKeyboard);
}

void CXTPRibbonControlTab::SetEnabled(BOOL /*bEnabled*/)
{

}

CString CXTPRibbonControlTab::GetItemTooltip(const CXTPTabManagerItem* pItem) const
{
	CString strToolTip = CXTPTabManager::GetItemTooltip(pItem);
	if (!strToolTip.IsEmpty())
		return strToolTip;

	return pItem->GetCaption();
}

void CXTPRibbonControlTab::SetSelectedItem(CXTPTabManagerItem* pItem)
{
	if (!IsFocused() && pItem && !GetRibbonBar()->IsRibbonMinimized())
		m_pParent->GetCommandBars()->ClosePopups();

	if (m_pSelected == pItem)
		return;

	if (m_pParent->GetCommandBars()->m_keyboardTips.nLevel > 1)
		m_pParent->GetCommandBars()->HideKeyboardTips();

	CXTPRibbonTab* pTab = (CXTPRibbonTab*)pItem;

	CXTPRibbonBar* pRibbonBar = GetRibbonBar();

	if (pRibbonBar->OnTabChanging(pTab))
		return;

	pRibbonBar->LockRedraw();

	pRibbonBar->RebuildControls(pTab);

	CXTPTabManager::SetSelectedItem(pTab);

	pRibbonBar->OnIdleUpdateCmdUI(0, 0);

	pRibbonBar->UnlockRedraw();

	pRibbonBar->OnTabChanged(pTab);

	if (pRibbonBar->IsRibbonMinimized() && pTab)
	{
		ShowPopupBar(pRibbonBar->m_bKeyboardSelect);
	}

	NotifySite(CBN_XTP_EXECUTE);

	if (pItem)
	{
		AccessibleNotifyWinEvent(EVENT_OBJECT_FOCUS , GetParent()->GetSafeHwnd(), GetID(), pItem->GetIndex() + 1);
	}

}


BOOL CXTPRibbonControlTab::DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon) const
{
	if (!pItem)
		return GetPaintManager()->m_bShowIcons;

	if (pItem->GetImageIndex() == -1 || GetPaintManager()->m_bShowIcons == FALSE)
		return FALSE;

	CXTPImageManagerIcon* pImage = GetImageManager()->GetImage(pItem->GetImageIndex(), szIcon.cx);

	if (!pImage)
		return FALSE;

	if (!bDraw)
	{
		return TRUE;
	}

	pItem->DrawImage(pDC, CRect(pt, szIcon), pImage);

	return TRUE;
}

void CXTPRibbonControlTab::RedrawControl(LPCRECT lpRect, BOOL bAnimate)
{
	m_pParent->Redraw(lpRect, bAnimate);
}

void CXTPRibbonControlTab::OnUnderlineActivate()
{
	SetFocused(TRUE);
}


BOOL CXTPRibbonControlTab::HeaderHasFocus() const
{
	return IsFocused() && m_pParent->IsTrackingMode();
}

BOOL  CXTPRibbonControlTab::OnSetSelected(int bSelected)
{
	if (bSelected == m_bSelected)
		return FALSE;

	if (IsKeyboardSelected(bSelected) && !GetRibbonBar()->IsRibbonMinimized())
		m_pParent->SetPopuped(-1);

	m_bSelected = bSelected;

	m_pParent->OnControlSelected(bSelected, this);

	if (IsKeyboardSelected(bSelected))
	{
		SetFocused(TRUE);
	}

	if (!bSelected)
	{
		SetFocused(FALSE);
		PerformMouseMove(m_pParent->GetSafeHwnd(), CPoint(-1, -1));
	}
	else
	{
		AccessibleNotifyWinEvent(EVENT_OBJECT_FOCUS , GetParent()->GetSafeHwnd(), GetID(), GetCurSel() + 1);
	}

	return TRUE;
}

BOOL CXTPRibbonControlTab::IsFocusable() const
{
	return TRUE;
}


BOOL CXTPRibbonControlTab::IsFocused() const
{
	return m_bFocused;
}

void CXTPRibbonControlTab::Draw(CDC* /*pDC*/)
{

}

void CXTPRibbonControlTab::SetFocused(BOOL bFocused)
{
	if (m_bFocused == bFocused)
		return;

	m_bFocused = bFocused;

	if (m_bFocused && GetSelectedItem() == NULL)
	{
		SetSelectedItem(FindNextFocusable(-1, +1));
	}

	RedrawParent();
}

BOOL CXTPRibbonControlTab::OnHookKeyDown(UINT nChar, LPARAM /*lParam*/)
{
	if (!IsFocused())
		return FALSE;

	UINT nCharLayout(nChar);
	CXTPDrawHelpers::KeyToLayout(m_pParent, nCharLayout);

	if (nCharLayout == VK_LEFT && FindNextFocusable(GetCurSel(), -1))
	{
		if (PerformKeyDown(m_pParent->GetSafeHwnd(), nChar))
			return TRUE;
	}

	if (nCharLayout == VK_RIGHT && FindNextFocusable(GetCurSel(), +1))
	{
		if (PerformKeyDown(m_pParent->GetSafeHwnd(), nChar))
			return TRUE;
	}

	if (nCharLayout == VK_UP)
	{
		m_pParent->SetSelected(m_pControls->GetNext(-1, +1), TRUE_KEYBOARD_PREV);
		return TRUE;
	}

	if (nCharLayout == VK_DOWN)
	{
		m_pParent->SetSelected(m_pControls->GetNext(CXTPControl::GetIndex(), +1), TRUE_KEYBOARD_NEXT);
		return TRUE;
	}
	return FALSE;
}

void CXTPRibbonControlTab::OnClick(BOOL /*bKeyboard*/, CPoint /*pt*/)
{
	// Do Nothing
}

BOOL CXTPRibbonControlTab::OnSetPopup(BOOL bPopup)
{
	m_bPopup = bPopup;
	if (bPopup)
	{
		ASSERT(GetSelectedItem());
		CXTPRibbonTab* pSelectedTab = (CXTPRibbonTab*)GetSelectedItem();
		if (!pSelectedTab || !GetRibbonBar()->IsRibbonMinimized())
		{
			m_bPopup = FALSE;
			return FALSE;
		}

		if (m_pCommandBar)
			m_pCommandBar->InternalRelease();

		CXTPCommandBars* pCommandBars = GetRibbonBar()->GetCommandBars();
		ASSERT(pCommandBars);

		m_pCommandBar = new CXTPRibbonTabPopupToolBar(pSelectedTab);
		m_pCommandBar->SetCommandBars(pCommandBars);

		m_pCommandBar->EnableCustomization(FALSE);
		m_pCommandBar->Popup(this, IsKeyboardSelected(GetSelected()));
	}
	else if (m_pCommandBar)
	{
		ASSERT(m_pCommandBar);
		m_pCommandBar->SetTrackingMode(FALSE);

		if (GetRibbonBar()->IsRibbonMinimized())
		{
			SetSelectedItem(NULL);
		}

	}
	RedrawParent();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Accessible


HRESULT CXTPRibbonControlTab::GetAccessibleChildCount(long FAR* pChildCount)
{
	if (pChildCount == 0)
		return E_INVALIDARG;

	*pChildCount = GetItemCount();

	return S_OK;
}
HRESULT CXTPRibbonControlTab::GetAccessibleChild(VARIANT /*varChild*/, IDispatch* FAR* ppdispChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	*ppdispChild = NULL;
	return S_FALSE;
}

HRESULT CXTPRibbonControlTab::GetAccessibleName(VARIANT varChild, BSTR* pszName)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	int nIndex = GetChildIndex(&varChild);

	if (nIndex == CHILDID_SELF)
		return CXTPControl::GetAccessibleName(varChild, pszName);

	CXTPTabManagerItem* pItem = GetItem(nIndex - 1);
	if (!pItem)
		return E_INVALIDARG;

	CString strCaption = pItem->GetCaption();
	CXTPDrawHelpers::StripMnemonics(strCaption);

	*pszName = strCaption.AllocSysString();
	return S_OK;
}

HRESULT CXTPRibbonControlTab::GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole)
{
	pvarRole->vt = VT_I4;
	pvarRole->lVal = GetChildIndex(&varChild) == CHILDID_SELF ? ROLE_SYSTEM_PAGETABLIST : ROLE_SYSTEM_PAGETAB;
	return S_OK;
}

HRESULT CXTPRibbonControlTab::AccessibleSelect(long flagsSelect, VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	int nIndex = GetChildIndex(&varChild);
	if (nIndex == CHILDID_SELF)
		return CXTPControl::AccessibleSelect(flagsSelect, varChild);

	SetCurSel(nIndex - 1);

	return S_OK;
}

HRESULT CXTPRibbonControlTab::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
{
	int nIndex = GetChildIndex(&varChild);

	if (nIndex == CHILDID_SELF)
	{
		pvarState->vt = VT_I4;
		pvarState->lVal = STATE_SYSTEM_FOCUSABLE;

		if (!m_pParent->IsVisible() || !IsVisible())
			pvarState->lVal |= STATE_SYSTEM_INVISIBLE;
		return S_OK;
	}

	CXTPTabManagerItem* pItem = GetItem(nIndex - 1);
	if (!pItem)
		return E_INVALIDARG;

	pvarState->vt = VT_I4;
	pvarState->lVal = STATE_SYSTEM_FOCUSABLE | STATE_SYSTEM_SELECTABLE;

	if (pItem->IsSelected())
		pvarState->lVal |= STATE_SYSTEM_SELECTED | STATE_SYSTEM_FOCUSED;

	if (!pItem->IsVisible())
		pvarState->lVal |= STATE_SYSTEM_INVISIBLE;

	return S_OK;
}

HRESULT CXTPRibbonControlTab::GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction)
{
	if (GetChildIndex(&varChild) == CHILDID_SELF)
			return CXTPControl::GetAccessibleDefaultAction(varChild, pszDefaultAction);

	*pszDefaultAction = SysAllocString(L"Switch");

	return S_OK;
}

HRESULT CXTPRibbonControlTab::AccessibleDoDefaultAction(VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	int nIndex = GetChildIndex(&varChild);

	if (nIndex == CHILDID_SELF)
		return CXTPControl::AccessibleDoDefaultAction(varChild);

	CXTPTabManagerItem* pItem = GetItem(nIndex - 1);
	if (!pItem)
		return E_INVALIDARG;

	pItem->Select();
	return S_OK;
}


HRESULT CXTPRibbonControlTab::AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
{
	*pxLeft = *pyTop = *pcxWidth = *pcyHeight = 0;

	int nIndex = GetChildIndex(&varChild);

	if (!m_pParent->GetSafeHwnd())
		return S_OK;

	if (!IsVisible())
		return S_OK;

	CRect rcControl = GetRect();

	if (nIndex != CHILDID_SELF)
	{
		rcControl = GetItem(nIndex - 1)->GetRect();
	}

	m_pParent->ClientToScreen(&rcControl);

	*pxLeft = rcControl.left;
	*pyTop = rcControl.top;
	*pcxWidth = rcControl.Width();
	*pcyHeight = rcControl.Height();

	return S_OK;
}

HRESULT CXTPRibbonControlTab::AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarID)
{
	if (pvarID == NULL)
		return E_INVALIDARG;

	pvarID->vt = VT_EMPTY;

	if (!m_pParent->GetSafeHwnd())
		return S_FALSE;

	if (!CXTPWindowRect(m_pParent).PtInRect(CPoint(xLeft, yTop)))
		return S_FALSE;

	pvarID->vt = VT_I4;
	pvarID->lVal = CHILDID_SELF;

	CPoint pt(xLeft, yTop);
	m_pParent->ScreenToClient(&pt);

	if (!GetRect().PtInRect(pt))
		return S_FALSE;

	CXTPTabManagerItem* pItem = HitTest(pt);
	if (pItem)
	{
		pvarID->lVal = pItem->GetIndex() + 1;
	}

	return S_OK;
}
