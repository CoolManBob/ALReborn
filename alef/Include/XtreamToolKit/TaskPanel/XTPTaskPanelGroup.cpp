// XTPTaskPanelGroup.cpp : implementation of the CXTPTaskPanelGroup class.
//
// This file is a part of the XTREME TASKPANEL MFC class library.
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
#include "Common/XTPPropExchange.h"

#include "XTPTaskPanelGroup.h"
#include "XTPTaskPanelGroupItem.h"
#include "XTPTaskPanelItems.h"
#include "XTPTaskPanel.h"
#include "XTPTaskPanelPaintManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPTaskPanelGroup

IMPLEMENT_SERIAL(CXTPTaskPanelGroup, CXTPTaskPanelItem, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

CXTPTaskPanelGroup::CXTPTaskPanelGroup(CXTPTaskPanel* pPanel)
{
	m_typeItem = xtpTaskItemTypeGroup;

	m_bSpecial = FALSE;

	m_rcMarginsOuter = CXTPTaskPanelPaintManager::rectDefault;
	m_rcMarginsInner = CXTPTaskPanelPaintManager::rectDefault;


	m_rcGroupTarget.SetRectEmpty();
	m_rcGroupCurrent.SetRectEmpty();

	m_pItems = new CXTPTaskPanelGroupItems(pPanel);
	m_pItems->m_pOwner = this;

	m_nCaptionHeight = 0;
	m_nExpandedClientHeight = 0;
	m_bCaptionVisible = TRUE;

	m_bExpandable = TRUE;
	m_bExpanded = TRUE;
	m_bExpanding = FALSE;
	m_itemLayout = xtpTaskItemLayoutDefault;

	m_nScrollOffset = 0;
	m_nItemsInRow = 1;

	m_szItemIcon = CSize(0);
	m_szClientBitmap = CSize(0);

	m_nMinClientHeight = 0;

	EnableAutomation();
}

CXTPTaskPanelGroup::~CXTPTaskPanelGroup()
{
	m_pItems->InternalRelease();
}

void CXTPTaskPanelGroup::OnRemoved()
{
	m_pItems->Clear(FALSE);
	m_pItems->m_pPanel = NULL;
}

int CXTPTaskPanelGroup::GetMinimumClientHeight() const
{
	return max(m_nMinClientHeight, GetTaskPanel()->GetMinimumGroupClientHeight());
}

void CXTPTaskPanelGroup::SetMinimumClientHeight(int nMinClientHeight)
{
	m_nMinClientHeight = nMinClientHeight;
}


CXTPTaskPanelGroupItem* CXTPTaskPanelGroup::AddLinkItem(UINT nID, int nImage)
{
	CXTPTaskPanel::CRepositionContext context(m_pPanel);

	CXTPTaskPanelGroupItem* pItem = (CXTPTaskPanelGroupItem*)m_pItems->Add(new CXTPTaskPanelGroupItem(), nID);
	pItem->SetIconIndex(nImage);
	pItem->SetType(xtpTaskItemTypeLink);

	return pItem;
}
CXTPTaskPanelGroupItem* CXTPTaskPanelGroup::AddTextItem(LPCTSTR strText)
{
	CXTPTaskPanel::CRepositionContext context(m_pPanel);

	CXTPTaskPanelGroupItem* pItem = (CXTPTaskPanelGroupItem*)m_pItems->Add(new CXTPTaskPanelGroupItem(), 0);
	pItem->SetCaption(strText);

	return pItem;
}

CXTPTaskPanelGroupItem* CXTPTaskPanelGroup::AddControlItem(HWND hWnd)
{
	CXTPTaskPanel::CRepositionContext context(m_pPanel);

	CXTPTaskPanelGroupItem* pItem = (CXTPTaskPanelGroupItem*)m_pItems->Add(new CXTPTaskPanelGroupItem(), 0);
	pItem->SetControlHandle(hWnd);

	return pItem;
}


CXTPTaskPanelGroupItem* CXTPTaskPanelGroup::GetAt(int nIndex) const
{
	return (CXTPTaskPanelGroupItem*)m_pItems->GetAt(nIndex);
}
CXTPTaskPanelGroupItem* CXTPTaskPanelGroup::FindItem(int nID) const
{
	return (CXTPTaskPanelGroupItem*)m_pItems->Find(nID);
}

BOOL CXTPTaskPanelGroup::IsDirty() const
{
	return (m_rcGroupCurrent != m_rcGroupTarget) && IsVisible();
}

int CXTPTaskPanelGroup::CalcInsideHeight()
{
	CRect rc;
	m_pPanel->GetClientRect(&rc);
	int nInsideHeight = rc.Height();

	CXTPTaskPanelPaintManager* pPaintManager = GetPaintManager();

	int nGroupSpacing = pPaintManager->GetGroupSpacing(m_pPanel);
	CRect rcMargins = pPaintManager->GetControlMargins(m_pPanel);

	int nGroupCount = m_pPanel->GetGroupCount();
	int nVisibleCount = 0;

	for (int i = 0; i < nGroupCount; i++)
	{
		CXTPTaskPanelGroup* pGroup = m_pPanel->GetAt(i);
		if (!pGroup->IsVisible())
			continue;

		nVisibleCount++;

		CRect rcMarginsOuter = pPaintManager->GetGroupOuterMargins(pGroup);

		nInsideHeight -= pGroup->m_nCaptionHeight + rcMarginsOuter.top + rcMarginsOuter.bottom;
		if (pGroup->IsExpanded() && pGroup != m_pPanel->GetActiveGroup())
			nInsideHeight -= pGroup->m_nExpandedClientHeight;
	}
	nInsideHeight -= nVisibleCount > 0 ? nGroupSpacing * (nVisibleCount - 1) : 0;
	nInsideHeight -= rcMargins.top + rcMargins.bottom;

	return nInsideHeight;
}

CSize CXTPTaskPanelGroup::GetItemIconSize() const
{
	return m_szItemIcon == CSize(0) ? GetTaskPanel()->GetItemIconSize() : m_szItemIcon;

}

void CXTPTaskPanelGroup::SetIconSize(CSize szItemIcon)
{
	m_szItemIcon = szItemIcon;
	GetTaskPanel()->Reposition();
}


void CXTPTaskPanelGroup::OnCalcClientHeight(CDC* pDC, CRect rc)
{
	CXTPTaskPanelPaintManager* pPaintManager = GetPaintManager();
	CRect rcMarginsOuter = pPaintManager->GetGroupOuterMargins(this);
	CRect rcMarginsInner = pPaintManager->GetGroupInnerMargins(this);

	rc.DeflateRect(rcMarginsOuter);
	CRect rcItems(rc);

	rcItems.DeflateRect(rcMarginsInner);
	m_nItemsInRow = 1;
	int nItemCount = GetItemCount();

	if (GetItemLayout() == xtpTaskItemLayoutImages)
	{
		CSize szIcon = GetItemIconSize();
		CRect rcInnerMargins(pPaintManager->m_rcImageLayoutIconPadding);
		CSize szButton(szIcon.cx + rcInnerMargins.right + rcInnerMargins.left,
			szIcon.cy + rcInnerMargins.top + rcInnerMargins.bottom);

		int nRowIcons = max(1, rcItems.Width() / szButton.cx), x = 0, y = 0;
		BOOL bWrap = FALSE;

		for (int i = 0; i < nItemCount; i++)
		{
			CXTPTaskPanelGroupItem* pItem = GetAt(i);

			if (!pItem->IsVisible())
				continue;

			if (bWrap)
			{
				x = 0;
				y++;
				bWrap = FALSE;
			}

			CRect rcItem(CPoint(rcItems.left + x * szButton.cx, rcItems.top + y * szButton.cy), szButton);
			pItem->OnReposition(rcItem);

			if (++x == nRowIcons)
				bWrap = TRUE;
		}
		rcItems.bottom = rcItems.top + rcMarginsInner.bottom + szButton.cy * (y + 1);
		m_nItemsInRow = nRowIcons;

		SetOffsetItem(m_nScrollOffset);
	}
	else
	{
		BOOL bMultiColumn = m_pPanel->IsMultiColumn();
		int nRowHeight = 0;
		CRect rcItemOuterMargins(0, 0, 0, 0);
		int nColumnWidth = m_pPanel->GetColumnWidth();
		ASSERT(nColumnWidth > 0);
		int x = 0;
		BOOL bWrap = FALSE;

		int nRowIcons = max(1, rcItems.Width() / nColumnWidth);

		for (int i = 0; i < nItemCount; i++)
		{
			CXTPTaskPanelGroupItem* pItem = GetAt(i);

			if (!pItem->IsVisible())
				continue;

			if (bMultiColumn)
			{
				if (bWrap)
				{
					rcItems.top += nRowHeight;

					nRowHeight = 0;
					x = 0;
					bWrap = FALSE;
				}

				CRect rcItem = pItem->OnReposition(CRect(rcItems.left + x * nColumnWidth, rcItems.top, rcItems.left + (x + 1) * nColumnWidth, rcItems.bottom));


				rcItemOuterMargins = pPaintManager->GetItemOuterMargins(pItem);

				nRowHeight = max(nRowHeight, rcItem.Height() + rcItemOuterMargins.bottom);

				if (++x == nRowIcons)
					bWrap = TRUE;
			}
			else
			{
				CRect rcItem = pItem->OnReposition(rcItems);
				rcItems.top = rcItem.bottom + pPaintManager->GetItemOuterMargins(pItem).bottom;
			}
		}
		if (bMultiColumn)
		{
			rcItems.top += nRowHeight;
			m_nItemsInRow = nRowIcons;
		}
		rcItems.bottom = rcItems.top + rcMarginsInner.bottom;
	}

	int nMinClientHeight = GetMinimumClientHeight();
	if (rcItems.bottom - rc.top < nMinClientHeight)
		rcItems.bottom = rc.top + nMinClientHeight;

	m_nExpandedClientHeight = rcItems.bottom - rc.top;

	m_nCaptionHeight = pPaintManager->DrawGroupCaption(pDC, this, FALSE);
}

CXTPTaskPanelGroupItem* CXTPTaskPanelGroup::GetNextVisibleItem(int nIndex, int nDirection) const
{
	return GetAt(m_pItems->GetNextVisibleIndex(nIndex, nDirection));
}

void CXTPTaskPanelGroup::RepositionScrollOffset()
{
	int nItemCount = GetItemCount();
	int nLastVisibleItem = m_pItems->GetNextVisibleIndex(nItemCount, -1);

	int nScrollOffset = min(GetOffsetItem(), nLastVisibleItem);

	if (nScrollOffset > 0)
	{
		CRect rcItem(GetAt(nLastVisibleItem)->GetItemRect());

		int nTopMargin = GetPaintManager()->GetGroupInnerMargins(this).top;

		while ((nScrollOffset > 0) && ((rcItem.bottom - GetAt(nScrollOffset - 1)->GetItemRect().top + nTopMargin
			< m_nExpandedClientHeight) || !GetAt(nScrollOffset - 1)->IsVisible()))
		{
			nScrollOffset--;
		}
	}
	SetOffsetItem(nScrollOffset, FALSE);

	for (int i = 0; i < nItemCount; i++)
	{
		CXTPTaskPanelGroupItem* pItem = GetAt(i);

		if (pItem->IsItemFocused() || (pItem->IsItemSelected() && GetTaskPanel()->m_bSelectItemOnFocus))
		{
			EnsureVisible(pItem, FALSE);
			break;
		}
	}
}

void CXTPTaskPanelGroup::RepositionAutoHeightControls(int nHeightOffset)
{
	if (GetItemLayout() == xtpTaskItemLayoutImages)
		return;

	int nCount = 0, i, nTotalHeight = 0, nItemCount = GetItemCount(), nOffset = 0;

	for (i = 0; i < nItemCount; i++)
	{
		CXTPTaskPanelGroupItem* pItem = GetAt(i);
		if (pItem->IsAutoHeight() && pItem->IsVisible())
		{
			nCount++;
			nTotalHeight += max(1, pItem->GetSize().cy);
		}
	}

	if (nCount == 0)
		return;

	for (i = 0; i < nItemCount; i++)
	{
		CXTPTaskPanelGroupItem* pItem = GetAt(i);
		pItem->m_rcItem.OffsetRect(0, nOffset);

		if (pItem->IsAutoHeight() && pItem->IsVisible())
		{
			int nItemHeight = max(1, pItem->GetSize().cy);
			int nItemHeightOffset = nCount > 1 ? (int)(nItemHeight* nHeightOffset / nTotalHeight) : nHeightOffset;

			nCount--;
			nTotalHeight -= nItemHeight;
			nHeightOffset -= nItemHeightOffset;

			pItem->m_rcItem.bottom += nItemHeightOffset;
			nOffset += nItemHeightOffset;
		}
	}


}


CRect CXTPTaskPanelGroup::OnReposition(CRect rc, BOOL bRecalcOnly)
{
	if (m_pPanel->IsExplorerBehaviour())
		SetOffsetItem(0, FALSE);

	CXTPTaskPanelPaintManager* pPaintManager = GetPaintManager();
	CRect rcMarginsOuter = pPaintManager->GetGroupOuterMargins(this);

	rc.DeflateRect(rcMarginsOuter);

	m_rcGroupTarget = rc;

	if (!m_pPanel->IsExplorerBehaviour() && (m_pPanel->GetActiveGroup() == this || IsExpanding()))
	{
		CRect rcMarginsInner = pPaintManager->GetGroupInnerMargins(this);
		int nInsideHeight = CalcInsideHeight();
		int nButtonHeight = max(32, GetItemIconSize().cy + rcMarginsInner.bottom);

		int nExpandedClientHeight = max(nButtonHeight + rcMarginsInner.top + 2, nInsideHeight);

		int nMinClientHeight = GetMinimumClientHeight();
		nExpandedClientHeight = max(nMinClientHeight, nExpandedClientHeight);

		if (nExpandedClientHeight > m_nExpandedClientHeight)
		{
			RepositionAutoHeightControls(nExpandedClientHeight - m_nExpandedClientHeight);
		}

		m_nExpandedClientHeight = nExpandedClientHeight;
		RepositionScrollOffset();
	}

	int nClientHeight = IsExpanded() ? m_nExpandedClientHeight : 0;

	m_rcGroupTarget.bottom = m_rcGroupTarget.top + m_nCaptionHeight + nClientHeight;

	m_rcGroupCurrent.left = m_rcGroupTarget.left;
	m_rcGroupCurrent.right = m_rcGroupTarget.right;

	if (!bRecalcOnly || !m_pPanel->m_bAnimation) OnAnimate(0);
	else if (!IsDirty()) OnAnimate(0);


	return m_rcGroupTarget;
}

#define MOVETO(A, B, Step) if (A != B) A += max(1, abs(A - B)/Step) * (A > B ? -1 : 1);

void CXTPTaskPanelGroup::OnAnimate(int nStep)
{
	if (nStep < 1)
	{
		m_rcGroupCurrent = m_rcGroupTarget;
	}
	else
	{
		MOVETO(m_rcGroupCurrent.top, m_rcGroupTarget.top, nStep);
		MOVETO(m_rcGroupCurrent.bottom, m_rcGroupTarget.bottom, nStep);
	}

	if (!IsDirty() && m_bExpanding)
	{
		m_bExpanding = FALSE;
		m_pPanel->NotifyOwner(XTP_TPN_GROUPEXPANDED, (LPARAM)this);
	}

	for (int i = 0; i < GetItemCount(); i++)
	{
		CXTPTaskPanelGroupItem* pItem = GetAt(i);

		pItem->OnAnimate(nStep);
	}
}

void CXTPTaskPanelGroup::OnPaint(CDC* pDC)
{
	GetPaintManager()->DrawGroupCaption(pDC, this, TRUE);

	GetPaintManager()->DrawGroupClient(pDC, this);
}

CRect CXTPTaskPanelGroup::GetCaptionRect() const
{
	int nCaptionRight = m_rcGroupCurrent.right;
	if (m_pPanel->GetScrollButton(TRUE)->pGroupCaption == this)
		nCaptionRight = m_pPanel->GetScrollButton(TRUE)->rcButton.left - GetPaintManager()->GetGroupSpacing(m_pPanel) - 1;
	if (m_pPanel->GetScrollButton(FALSE)->pGroupCaption == this)
		nCaptionRight = m_pPanel->GetScrollButton(FALSE)->rcButton.left - GetPaintManager()->GetGroupSpacing(m_pPanel) - 1;

	return CRect(m_rcGroupCurrent.left, m_rcGroupCurrent.top, nCaptionRight, m_rcGroupCurrent.top + m_nCaptionHeight);
}

CRect CXTPTaskPanelGroup::GetTargetCaptionRect() const
{
	int nCaptionRight = m_rcGroupTarget.right;
	if (m_pPanel->GetScrollButton(TRUE)->pGroupCaption == this)
		nCaptionRight = m_pPanel->GetScrollButton(TRUE)->rcButton.left - GetPaintManager()->GetGroupSpacing(m_pPanel) - 1;
	if (m_pPanel->GetScrollButton(FALSE)->pGroupCaption == this)
		nCaptionRight = m_pPanel->GetScrollButton(FALSE)->rcButton.left - GetPaintManager()->GetGroupSpacing(m_pPanel) - 1;

	return CRect(m_rcGroupTarget.left, m_rcGroupTarget.top, nCaptionRight, m_rcGroupTarget.top + m_nCaptionHeight);
}

CRect CXTPTaskPanelGroup::GetClientRect() const
{
	return CRect(m_rcGroupCurrent.left, m_rcGroupCurrent.top + m_nCaptionHeight, m_rcGroupCurrent.right, m_rcGroupCurrent.bottom);
}

CRect CXTPTaskPanelGroup::GetTargetClientRect() const
{
	return CRect(m_rcGroupTarget.left, m_rcGroupTarget.top + m_nCaptionHeight, m_rcGroupTarget.right, m_rcGroupTarget.bottom);
}

BOOL CXTPTaskPanelGroup::IsAcceptFocus() const
{
	return CXTPTaskPanelItem::IsAcceptFocus() && IsExpandable();
}

void CXTPTaskPanelGroup::SetSpecialGroup(BOOL bSpecial)
{
	m_bSpecial = bSpecial;
	RedrawPanel();
}
BOOL CXTPTaskPanelGroup::IsSpecialGroup() const
{
	return m_bSpecial;
}

void CXTPTaskPanelGroup::SetExpandable(BOOL bExpandable)
{
	m_bExpandable = bExpandable;

	RedrawPanel();
}
BOOL CXTPTaskPanelGroup::IsExpandable() const
{
	return m_bExpandable && m_pPanel->m_bExpandable && m_bEnabled;
}

void CXTPTaskPanelGroup::SetExpanded(BOOL bExpanded)
{
	if (m_pPanel)
		m_pPanel->ExpandGroup(this, bExpanded);
}

BOOL CXTPTaskPanelGroup::IsExpanded() const
{
	return m_pPanel ? m_pPanel->IsGroupExpanded(this) : FALSE;
}

BOOL CXTPTaskPanelGroup::IsExpanding() const
{
	return m_bExpanding;
}

void CXTPTaskPanelGroup::ShowCaption(BOOL bShow)
{
	if (m_bCaptionVisible != bShow)
	{
		m_bCaptionVisible = bShow;
		RepositionPanel();
	}
}

BOOL CXTPTaskPanelGroup::IsCaptionVisible() const
{
	return m_bCaptionVisible;
}

void CXTPTaskPanelGroup::SetClientBitmap(UINT nID, COLORREF clrTransparent, BOOL bAlpha /*= FALSE*/)
{
	m_ilClient.DeleteImageList();

	CBitmap bmp;
	bmp.LoadBitmap(nID);

	BITMAP bmpInfo;
	bmp.GetBitmap(&bmpInfo);
	CSize szBitmap(bmpInfo.bmWidth, bmpInfo.bmHeight);


	if (bAlpha)
	{
		m_ilClient.Create(szBitmap.cx, szBitmap.cy, ILC_COLOR32 | ILC_MASK, 0, 1);
		m_ilClient.Add(&bmp, (CBitmap*)NULL);
	}
	else
	{
		m_ilClient.Create(szBitmap.cx, szBitmap.cy, ILC_COLOR24 | ILC_MASK, 0, 1);
		m_ilClient.Add(&bmp, clrTransparent);
	}
	m_szClientBitmap = szBitmap;
}
void CXTPTaskPanelGroup::OnFillClient(CDC* pDC, CRect rc)
{
	if (m_ilClient.GetSafeHandle() && m_ilClient.GetImageCount() == 1)
	{
		CPoint pt(rc.right - m_szClientBitmap.cx, rc.bottom - m_szClientBitmap.cy);
		m_ilClient.Draw(pDC, 0, pt, ILD_NORMAL);

	}
}

CRect CXTPTaskPanelGroup::GetHitTestRect() const
{
	CRect rc = GetCaptionRect();
	rc.OffsetRect(0, -m_pPanel->GetScrollOffset());

	return rc;
}

CXTPTaskPanelGroupItem* CXTPTaskPanelGroup::HitTest(CPoint pt, CRect* lpRect) const
{
	if (!IsExpanded())
		return NULL;

	int nOffset = m_pPanel->GetScrollOffset() - m_nCaptionHeight - m_rcGroupCurrent.top + GetScrollOffsetPos();
	pt.y += nOffset;

	for (int i = 0; i < GetItemCount(); i++)
	{
		CXTPTaskPanelGroupItem* pItem = GetAt(i);
		CRect rcItem = pItem->GetItemRect();

		if (rcItem.PtInRect(pt) && pItem->IsVisible())
		{
			if (lpRect)
			{
				rcItem.OffsetRect(0, -nOffset);
				*lpRect = rcItem;
			}
			return pItem;
		}
	}
	return NULL;
}

XTPTaskPanelItemLayout CXTPTaskPanelGroup::GetItemLayout() const
{
	if (m_itemLayout != xtpTaskItemLayoutDefault)
		return m_itemLayout;

	return m_pPanel->GetItemLayout();
}

void CXTPTaskPanelGroup::SetItemLayout(XTPTaskPanelItemLayout itemLayout)
{
	if (itemLayout != m_itemLayout)
	{
		m_itemLayout = itemLayout;
		RepositionPanel();
	}
}


void CXTPTaskPanelGroup::SetSelectedItem(CXTPTaskPanelGroupItem* pSelectedItem)
{
	int nCount = GetItemCount();

	for (int i = 0; i < nCount; i++)
	{
		CXTPTaskPanelGroupItem* pItem = GetAt(i);
		pItem->SetItemSelected(pSelectedItem == pItem);
	}
}

BOOL CXTPTaskPanelGroup::IsChildItemDragOver() const
{
	return m_pPanel && m_pPanel->m_pItemDragOver &&
		m_pPanel->m_pItemDragOver->GetParentItems() == m_pItems;
}

int CXTPTaskPanelGroup::GetItemCount() const
{
	return m_pItems->GetCount();
}

BOOL CXTPTaskPanelGroup::IsScrollButtonEnabled(BOOL bScrollButtonUp) const
{
	CXTPTaskPanelGroupItem* pLastVisibleItem = (CXTPTaskPanelGroupItem*)m_pItems->GetLastVisibleItem();

	if (pLastVisibleItem == NULL)
		return FALSE;

	if (bScrollButtonUp)
		return m_nScrollOffset != 0;

	CRect rcItem(pLastVisibleItem->GetItemRect());

	return rcItem.bottom - GetScrollOffsetPos() > m_rcGroupTarget.Height() - m_nCaptionHeight;
}

int CXTPTaskPanelGroup::GetOffsetItem() const
{
	if (m_nScrollOffset == 0)
		return 0;

	if (m_nScrollOffset < GetItemCount())
	{
		CXTPTaskPanelGroupItem* pItem = GetAt(m_nScrollOffset);

		if (pItem->IsVisible())
			return m_nScrollOffset;

		int nScrollOffset = m_pItems->GetNextVisibleIndex(m_nScrollOffset, +1);

		if (nScrollOffset != -1)
			return nScrollOffset;

		nScrollOffset = m_pItems->GetNextVisibleIndex(m_nScrollOffset, -1);
		if (nScrollOffset != -1)
			return nScrollOffset;
	}

	return 0;
}

int CXTPTaskPanelGroup::GetScrollOffsetPos() const
{
	int nScrollOffset = GetOffsetItem();

	if (nScrollOffset > 0 && nScrollOffset < GetItemCount())
	{
		return GetAt(nScrollOffset)->GetItemRect().top - GetPaintManager()->GetGroupInnerMargins(this).top;
	}
	return 0;
}

void CXTPTaskPanelGroup::SetOffsetItem(int nScrollOffset, BOOL bUpdateScrollButtons /*= TRUE*/)
{
	nScrollOffset = nScrollOffset - (nScrollOffset % m_nItemsInRow);

	if (nScrollOffset > GetItemCount() - 1)
		nScrollOffset = GetItemCount() - 1;

	if (nScrollOffset < 0)
		nScrollOffset = 0;

	if (m_nScrollOffset != nScrollOffset)
	{
		m_nScrollOffset = nScrollOffset;

		if (bUpdateScrollButtons)
		{
			OnAnimate(0);
			m_pPanel->UpdateScrollButtons();
		}
	}
}

void CXTPTaskPanelGroup::Scroll(int nDelta)
{
	SetOffsetItem(m_pItems->GetNextVisibleIndex(GetOffsetItem(), nDelta));
}

BOOL CXTPTaskPanelGroup::IsItemVisible(CXTPTaskPanelGroupItem* pItem, BOOL bAllowPart) const
{
	if (!IsExpanded())
		return FALSE;

	if (!pItem || pItem->GetItemGroup() != this || !pItem->IsVisible())
		return FALSE;

	if (m_pPanel->IsExplorerBehaviour())
		return TRUE;

	int nIndex = pItem->GetIndex();

	if (m_nScrollOffset > nIndex)
		return FALSE;

	CRect rcItem(pItem->GetItemRect());
	if ((bAllowPart ? rcItem.top : rcItem.bottom) - GetScrollOffsetPos() > m_rcGroupTarget.Height() - m_nCaptionHeight)
		return FALSE;

	return TRUE;
}

void CXTPTaskPanelGroup::EnsureVisible(CXTPTaskPanelGroupItem* pItem, BOOL bUpdateScrollButtons /*= TRUE*/)
{
	if (m_pPanel->IsExplorerBehaviour() || GetItemCount() == 0 || !IsExpanded())
		return;

	if (!pItem || pItem->GetItemGroup() != this || !pItem->IsVisible())
		return;

	int nIndex = pItem->GetIndex();

	if (m_nScrollOffset > nIndex)
	{
		SetOffsetItem(nIndex, bUpdateScrollButtons);
		return;
	}

	CRect rcItem(pItem->GetItemRect());
	if (rcItem.bottom - GetScrollOffsetPos() > m_nExpandedClientHeight)
	{
		int nTopMargin = GetPaintManager()->GetGroupInnerMargins(this).top;
		if (rcItem.top - GetScrollOffsetPos() <= nTopMargin)
			return;

		int nScrollOffset = m_nScrollOffset + 1;
		for (; nScrollOffset <= nIndex; nScrollOffset++)
		{
			CXTPTaskPanelGroupItem* pItemScroll = GetAt(nScrollOffset);
			if (!pItemScroll || !pItemScroll->IsVisible())
				continue;

			int nScrollOffsetPos = pItemScroll->GetItemRect().top - nTopMargin;

			if (rcItem.bottom - nScrollOffsetPos <= m_nExpandedClientHeight)
				break;

			if (rcItem.top - nScrollOffsetPos <= nTopMargin)
				break;
		}
		SetOffsetItem(nScrollOffset, bUpdateScrollButtons);
	}
}

void CXTPTaskPanelGroup::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPTaskPanelItem::DoPropExchange(pPX);

	BOOL bExpanded = IsExpanded();

	PX_Bool(pPX, _T("CaptionVisible"), m_bCaptionVisible, TRUE);
	PX_Bool(pPX, _T("Expandable"), m_bExpandable, TRUE);
	PX_Bool(pPX, _T("Expanded"), bExpanded, FALSE);
	PX_Bool(pPX, _T("Special"), m_bSpecial, FALSE);
	PX_Enum(pPX, _T("Layout"), m_itemLayout, xtpTaskItemLayoutDefault);
	PX_Rect(pPX, _T("MarginsInner"), m_rcMarginsInner, CXTPTaskPanelPaintManager::rectDefault);
	PX_Rect(pPX, _T("MarginsOuter"), m_rcMarginsOuter, CXTPTaskPanelPaintManager::rectDefault);
	PX_Size(pPX, _T("IconSize"), m_szItemIcon, CSize(0));

	if (pPX->IsLoading())
	{
		m_pItems->m_pPanel = m_pPanel;
		SetExpanded(bExpanded);
	}

	CXTPPropExchangeSection secItems(pPX->GetSection(_T("Items")));
	m_pItems->DoPropExchange(&secItems);
}

HRESULT CXTPTaskPanelGroup::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	CXTPTaskPanelItem::GetAccessibleState(varChild, pvarState);

	if (IsExpanded())
		pvarState->lVal |= STATE_SYSTEM_EXPANDED;
	else
		pvarState->lVal |= STATE_SYSTEM_COLLAPSED;

	return S_OK;
}

HRESULT CXTPTaskPanelGroup::GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	*pszDefaultAction = SysAllocString(L"Expand");

	return S_OK;
}

HRESULT CXTPTaskPanelGroup::AccessibleDoDefaultAction(VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	SetExpanded(!IsExpanded());

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPTaskPanelGroup message handlers


