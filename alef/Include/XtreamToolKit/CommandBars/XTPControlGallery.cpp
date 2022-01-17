// XTPControlGallery.cpp : implementation file.
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

#include "Common/XTPImageManager.h"
#include "Common/XTPPropExchange.h"

#include "XTPControls.h"
#include "XTPCommandBar.h"
#include "XTPControlGallery.h"
#include "XTPPaintManager.h"
#include "XTPMouseManager.h"
#include "XTPCommandBars.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPControlGalleryItem

CXTPControlGalleryItem::CXTPControlGalleryItem()
{
	m_pItems = NULL;

	m_nId = 0;
	m_nIndex = -1;
	m_nImage = -1;
	m_dwData = 0;
	m_bLabel = FALSE;

	m_szItem = CSize(0, 0);

}

int CXTPControlGalleryItem::GetIndex() const
{
	return m_nIndex;
}

CSize CXTPControlGalleryItem::GetSize()
{
	return m_szItem == CSize(0, 0) ? m_pItems->m_szItem : m_szItem;
}

void CXTPControlGalleryItem::SetSize(CSize szItem)
{
	m_szItem = szItem;
}

void CXTPControlGalleryItem::Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{
	if (IsLabel())
	{
		pGallery->GetGalleryPaintManager()->
			DrawLabel(pDC, this, rcItem);
		return;
	}

	CXTPPaintManager* pPaintManager = pGallery->GetPaintManager();

	CXTPImageManagerIcon* pImage = GetImage();

	if (pImage)
	{
		pPaintManager->DrawRectangle(pDC, rcItem, bSelected, bPressed, bEnabled, bChecked, FALSE, xtpBarTypeNormal, xtpBarPopup);
		CSize szImage(pImage->GetWidth(), pImage->GetHeight());
		CPoint pt((rcItem.left + rcItem.right - szImage.cx) / 2, (rcItem.top + rcItem.bottom - szImage.cy) / 2);

		pImage->Draw(pDC, pt, pImage->GetIcon(bEnabled ? xtpImageNormal : xtpImageDisabled), szImage);
	}
	else
	{
		pPaintManager->DrawRectangle(pDC, rcItem, bSelected, FALSE, bEnabled, bChecked, FALSE, xtpBarTypePopup, xtpBarPopup);
		COLORREF clrTextColor = pPaintManager->GetRectangleTextColor(bSelected, FALSE, bEnabled, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);

		pDC->SetTextColor(clrTextColor);
		CXTPFontDC font(pDC, pPaintManager->GetRegularFont());

		CRect rcText(rcItem);
		rcText.left += 3;

		pDC->DrawText(m_strCaption, rcText, DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
	}
}

CXTPImageManagerIcon* CXTPControlGalleryItem::GetImage()
{
	if (!m_pItems)
		return NULL;

	return m_pItems->GetImageManager()->GetImage(m_nImage >= 0 ? m_nImage : m_nId, 0);
}


//////////////////////////////////////////////////////////////////////////
// CXTPControlGalleryItems

IMPLEMENT_DYNAMIC(CXTPControlGalleryItems, CCmdTarget)

CXTPControlGalleryItems::CXTPControlGalleryItems(CXTPControlAction* pAction)
	: m_pAction(pAction)
{
	m_szItem = CSize(20, 20);
	m_pImageManager = new CXTPImageManager();
	m_pImageManager->DrawReverted(2);
	m_bClipItems = TRUE;

}

CXTPControlGalleryItems::~CXTPControlGalleryItems()
{
	m_pAction = NULL;

	RemoveAll();
	CMDTARGET_RELEASE(m_pImageManager);

}

void CXTPControlGalleryItems::RemoveAll()
{
	for (int i = 0; i < m_arrItems.GetSize(); i++)
	{
		m_arrItems[i]->InternalRelease();
	}

	m_arrItems.RemoveAll();
	OnItemsChanged();
}

void CXTPControlGalleryItems::Remove(int nIndex)
{
	CXTPControlGalleryItem* pItem = GetItem(nIndex);
	if (!pItem)
		return;

	m_arrItems.RemoveAt(nIndex);
	pItem->InternalRelease();

	UpdateIndexes(nIndex);
	OnItemsChanged();
}

void CXTPControlGalleryItems::UpdateIndexes(int nStart /*= 0*/)
{
	for (int i = nStart; i < GetItemCount(); i++)
		GetItem(i)->m_nIndex = i;
}

CXTPControlGalleryItem* CXTPControlGalleryItems::InsertItem(int nIndex, CXTPControlGalleryItem* pItem, int nId, int nImage)
{
	if (!pItem)
		return NULL;

	if (nIndex < 0 || nIndex > GetItemCount())
		nIndex = GetItemCount();

	m_arrItems.InsertAt(nIndex, pItem);
	UpdateIndexes(nIndex);

	pItem->m_pItems = this;
	pItem->m_nImage = nImage;

	if (nId != -1)
	{
		pItem->m_nId = nId;

		pItem->m_strCaption.LoadString(nId);
		pItem->m_strToolTip = pItem->m_strCaption;
	}

	OnItemsChanged();

	return pItem;
}

void CXTPControlGalleryItems::OnItemsChanged()
{
	if (!m_pAction)
		return;

	for (int i = 0; i < m_pAction->GetCount(); i++)
	{
		CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, m_pAction->GetControl(i));
		if (pGallery)
		{
			pGallery->m_nSelected = -1;
			pGallery->m_nScrollPos = 0;
			pGallery->Reposition();

			if (pGallery->m_bPreview)
			{
				pGallery->m_bPreview = FALSE;
				pGallery->NotifySite(XTP_GN_PREVIEWCANCEL);
			}
		}
	}
}

CXTPControlGalleryItem* CXTPControlGalleryItems::AddItem(CXTPControlGalleryItem* pItem, int nId, int nImage)
{
	return InsertItem(GetItemCount(), pItem, nId, nImage);
}

CXTPControlGalleryItem* CXTPControlGalleryItems::AddItem(int nId, int nImage)
{
	return AddItem(new CXTPControlGalleryItem(), nId, nImage);
}

CXTPControlGalleryItem* CXTPControlGalleryItems::AddItem(LPCTSTR lpszCaption, int nImage)
{
	CXTPControlGalleryItem* pItem = AddItem(new CXTPControlGalleryItem(), -1, nImage);
	pItem->SetCaption(lpszCaption);

	return pItem;
}

CXTPControlGalleryItem* CXTPControlGalleryItems::AddLabel(int nId)
{
	CXTPControlGalleryItem* pItem = AddItem(new CXTPControlGalleryItem(), nId);
	pItem->m_bLabel = TRUE;

	return pItem;
}

CXTPControlGalleryItem* CXTPControlGalleryItems::AddLabel(LPCTSTR lpszCaption)
{
	CXTPControlGalleryItem* pItem = AddItem(new CXTPControlGalleryItem());
	pItem->SetCaption(lpszCaption);
	pItem->m_bLabel = TRUE;

	return pItem;
}

int CXTPControlGalleryItems::GetItemCount() const
{
	return (int)m_arrItems.GetSize();
}

CXTPControlGalleryItem* CXTPControlGalleryItems::GetItem(int nIndex) const
{
	return nIndex >= 0 && nIndex < GetItemCount() ? m_arrItems.GetAt(nIndex) : NULL;
}

void CXTPControlGalleryItems::SetImageManager(CXTPImageManager* pImageManager)
{
	CMDTARGET_RELEASE(m_pImageManager);

	m_pImageManager = pImageManager;
}

BOOL CXTPControlGalleryItems::PreDrawItem(CDC* pDC, CXTPControlGallery* pGallery, CXTPControlGalleryItem* pItem, CRect rcItem,
	BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pGallery);
	UNREFERENCED_PARAMETER(pItem);
	UNREFERENCED_PARAMETER(rcItem);
	UNREFERENCED_PARAMETER(bEnabled);
	UNREFERENCED_PARAMETER(bSelected);
	UNREFERENCED_PARAMETER(bPressed);
	UNREFERENCED_PARAMETER(bChecked);


	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
// CXTPControlGallery

IMPLEMENT_XTP_CONTROL(CXTPControlGallery, CXTPControlPopup)


CXTPControlGallery::CXTPControlGallery()
{
	m_nChecked = -1;
	m_nSelected = -1;
	m_bPressed = FALSE;
	m_bKeyboardSelected = FALSE;
	m_bHideSelection = FALSE;

	m_nScrollPos = 0;
	m_nTotalHeight = 0;
	m_bShowLabels = TRUE;

	m_nWidth = 120;
	m_nHeight = 60;

	m_rcMargin.SetRectEmpty();

	m_bShowScrollBar = TRUE;


	m_bShowBorders = FALSE;
	m_bShowShadow = FALSE;

	m_bPreview = FALSE;

	m_controlType = xtpControlGallery;

	SetFlags(xtpFlagShowPopupBarTip);
}

CXTPControlGalleryItems* CXTPControlGalleryItems::CreateItems(CXTPCommandBars* pCommandBars, int nId)
{
	CXTPControlAction* pAction = pCommandBars->CreateAction(nId);

	if (pAction->m_pTarget == NULL)
	{
		pAction->m_pTarget = new CXTPControlGalleryItems(pAction);
	}

	ASSERT_KINDOF(CXTPControlGalleryItems, pAction->m_pTarget);

	return (CXTPControlGalleryItems*)pAction->m_pTarget;
}

CXTPControlGalleryItems* CXTPControlGallery::GetItems() const
{
	CXTPControlAction* pAction = GetAction();
	if (!pAction)
		return NULL;

	return (CXTPControlGalleryItems*)pAction->m_pTarget;
}

CXTPControlGallery::~CXTPControlGallery()
{
}

void CXTPControlGallery::SetAction(CXTPControlAction* pAction)
{
	CXTPControlPopup::SetAction(pAction);
	Reposition();
}

BOOL CXTPControlGallery::IsShowAsButton() const
{
	return GetStyle() != xtpButtonAutomatic;
}

void CXTPControlGallery::SetItems(CXTPControlGalleryItems* pItems)
{
	if (pItems)
	{
		SetAction(pItems->m_pAction);
	}
}

void CXTPControlGallery::Reposition()
{
	if (GetParent() == NULL)
		return;

	if (IsShowAsButton())
		return;

	CRect rcItems = GetItemsRect();

	int x = rcItems.left;
	int y = rcItems.top;
	BOOL bFirstItem = TRUE;

	int nCount = GetItemCount();

	m_arrRects.SetSize(nCount);
	int nRowHeight = 0;

	for (int i = 0 ; i < nCount; i++)
	{
		CXTPControlGalleryItem* pItem = GetItem(i);
		m_arrRects[i].pItem = pItem;
		m_arrRects[i].bBeginRow = FALSE;

		if (pItem->IsLabel())
		{
			if (!m_bShowLabels)
			{
				m_arrRects[i].rcItem = CRect(0, 0, 0, 0);
				continue;
			}

			if (!bFirstItem)
			{
				y += nRowHeight;
			}

			CRect rcItem(rcItems.left, y, rcItems.right, y + 19);
			m_arrRects[i].rcItem = rcItem;
			m_arrRects[i].bBeginRow = TRUE;

			y += 20;
			nRowHeight = 0;
			x = rcItems.left;
			bFirstItem = TRUE;

		}
		else
		{
			CSize szItem = pItem->GetSize();
			if (szItem.cx == 0)
				szItem.cx = rcItems.Width();

			if (bFirstItem)
			{
				m_arrRects[i].bBeginRow = TRUE;
			}

			if (!bFirstItem && x + szItem.cx > rcItems.right)
			{
				y += nRowHeight;
				x = rcItems.left;
				m_arrRects[i].bBeginRow = TRUE;
				nRowHeight = 0;
			}

			CRect rcItem(x, y, x + szItem.cx, y + szItem.cy);
			m_arrRects[i].rcItem = rcItem;

			nRowHeight = max(nRowHeight, szItem.cy);

			bFirstItem = FALSE;
			x += szItem.cx;
		}
	}

	if (!bFirstItem)
		y += nRowHeight;

	m_nTotalHeight = y - rcItems.top;

	SetupScrollInfo();
}

void CXTPControlGallery::SetCheckedItem(int nId)
{
	if (m_nChecked != nId)
	{
		m_nChecked = nId;
		InvalidateItems();
	}
}

int CXTPControlGallery::GetCheckedItem() const
{
	return m_nChecked;
}

int CXTPControlGallery::GetSelectedItem() const
{
	return m_nSelected;
}

int CXTPControlGallery::GetItemCount() const
{
	CXTPControlGalleryItems* pItems = GetItems();

	return pItems ? pItems->GetItemCount() : 0;
}

CXTPControlGalleryItem* CXTPControlGallery::GetItem(int nIndex) const
{
	CXTPControlGalleryItems* pItems = GetItems();

	return pItems ? pItems->GetItem(nIndex) : NULL;
}

void CXTPControlGallery::DrawItems(CDC* pDC)
{
	CXTPControlGalleryItems* pItems = GetItems();
	if (!pItems)
		return;

	ASSERT(m_arrRects.GetSize() == pItems->GetItemCount());

	CRect rcItems = GetItemsRect();

	CRgn rgn;
	CPoint point = pDC->GetViewportOrg();
	CRect rcClipBox(rcItems);
	rcClipBox.OffsetRect(point);

	rgn.CreateRectRgnIndirect(rcClipBox);
	pDC->SelectClipRgn(&rgn);

	int nSelected = m_bHideSelection ? -1 : m_nSelected;

	for (int i = 0; i < m_arrRects.GetSize(); i++)
	{
		const GALLERYITEM_POSITION& pos = m_arrRects[i];

		CRect rcItem = pos.rcItem;
		rcItem.OffsetRect(0, -m_nScrollPos);

		if (rcItem.bottom < rcItems.top)
			continue;

		if (!rcItem.IsRectEmpty())
		{
			if (!pItems->PreDrawItem(pDC, this, pos.pItem, rcItem, GetEnabled(), nSelected == i, nSelected == i && m_bPressed, m_nChecked == pos.pItem->GetID()))
			{
				pos.pItem->Draw(pDC, this, rcItem, GetEnabled(), nSelected == i, nSelected == i && m_bPressed, m_nChecked == pos.pItem->GetID());
			}
		}

		if (rcItem.top > rcItems.bottom)
			break;
	}

	pDC->SelectClipRgn(NULL);
}

int CXTPControlGallery::HitTestItem(CPoint point, LPRECT lpRect) const
{
	if (!GetEnabled())
		return -1;

	if (IsShowAsButton())
		return -1;

	ASSERT(m_arrRects.GetSize() == GetItemCount());

	CRect rcItems = GetItemsRect();

	for (int i = 0; i < m_arrRects.GetSize(); i++)
	{
		const GALLERYITEM_POSITION& pos = m_arrRects[i];

		CRect rcItem = pos.rcItem;
		rcItem.OffsetRect(0, -m_nScrollPos);

		if (rcItem.top >= rcItems.bottom)
			break;

		if (rcItem.PtInRect(point))
		{
			if (lpRect)
			{
				*lpRect = rcItem;
			}
			return pos.pItem->IsLabel() ? -1 : i;
		}
	}

	return -1;
}

void CXTPControlGallery::InvalidateItems(LPCRECT lpRect, BOOL bAnimate)
{
	GetParent()->Redraw(lpRect ? lpRect : GetRect(), bAnimate);
}

void CXTPControlGallery::OnExecute()
{
	if (m_bPreview)
	{
		NotifySite(XTP_GN_PREVIEWAPPLY);
		m_bPreview = FALSE;
	}

	CXTPControlPopup::OnExecute();
}

BOOL CXTPControlGallery::OnSetPopup(BOOL bPopup)
{
	if (m_bPreview)
	{
		NotifySite(XTP_GN_PREVIEWCANCEL);
		m_bPreview = FALSE;
	}

	return CXTPControlPopup::OnSetPopup(bPopup);
}


void CXTPControlGallery::OnMouseHover()
{
	if (IsItemSelected() && !m_bPreview && GetSelected())
	{
		m_bPreview = TRUE;
		NotifySite(XTP_GN_PREVIEWSTART);

		NotifySite(XTP_GN_PREVIEWCHANGE);
	}

	CXTPControlPopup::OnMouseHover();
}

void CXTPControlGallery::PerformMouseMove(CPoint point)
{
	int nItem = HitTestItem(point);

	if (nItem == -1 && m_bKeyboardSelected)
		return;

	if (nItem == -1 && point == CPoint(-1, -1) && m_nSelected != -1)
	{
		HideSelection();
		return;
	}

	int nSelected = m_nSelected;

	if (nItem != m_nSelected || m_bHideSelection || m_bKeyboardSelected)
	{
		m_nSelected = nItem;
		m_bHideSelection = FALSE;
		m_bKeyboardSelected = FALSE;
		m_bPressed = FALSE;

		CXTPControlGalleryItems* pItems = GetItems();

		if (pItems && pItems->m_bClipItems)
		{
			if (nSelected != -1)
			{
				InvalidateItems(GetItemDrawRect(nSelected), TRUE);
			}

			if (m_nSelected != -1)
			{
				InvalidateItems(GetItemDrawRect(m_nSelected), FALSE);
			}
		}
		else
		{
			InvalidateItems();
		}

		if (!m_bPreview && m_nSelected != -1)
		{
			m_pParent->SetTimer(XTP_TID_HOVER, 200, NULL);
		}

		if (m_bPreview)
		{
			NotifySite(XTP_GN_PREVIEWCHANGE);
		}

		if (m_nSelected == -1 && m_bPreview)
		{
			m_bPreview = FALSE;
			NotifySite(XTP_GN_PREVIEWCANCEL);
		}
	}
}
void CXTPControlGallery::PerformMouseDown(CPoint /*point*/)
{
	if (m_nSelected != -1 && !m_bKeyboardSelected)
	{
		m_bPressed = TRUE;
		InvalidateItems();
	}
}

void CXTPControlGallery::PerformMouseUp(CPoint /*point*/)
{
	if (m_bPressed)
	{
		m_bPressed = FALSE;
		InvalidateItems();

		OnExecute();
	}
}

BOOL CXTPControlGallery::HasBottomSeparator() const
{
	if (GetParent()->GetType() != xtpBarTypePopup)
		return FALSE;

	if (m_bShowBorders)
		return FALSE;

	int nIndex = GetIndex();
	int nNextIndex = GetParent()->GetControls()->GetNext(nIndex, +1, TRUE);

	return nNextIndex > nIndex;
}

CRect CXTPControlGallery::GetItemDrawRect(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_arrRects.GetSize())
		return CRect(0, 0, 0, 0);

	CRect rc(m_arrRects[nIndex].rcItem);
	rc.OffsetRect(0, -m_nScrollPos);

	rc.IntersectRect(rc, GetItemsRect());

	return rc;
}

CRect CXTPControlGallery::GetBorders() const
{
	CRect rcBorders(0, 0, 0, 0);
	if (m_bShowBorders)
	{
		rcBorders.SetRect(1, 1, 1, 1);
	}
	if (m_bShowScrollBar)
	{
		rcBorders.right += (GetCommandBar() ? GetGalleryPaintManager()->m_cxPopup : GetGalleryPaintManager()->m_cxVScroll);
	}

	if (HasBottomSeparator())
	{
		rcBorders.bottom = 2;
	}

	rcBorders.DeflateRect(m_rcMargin.left, m_rcMargin.top, -m_rcMargin.right, -m_rcMargin.bottom);

	return rcBorders;
}

CXTPControlGalleryPaintManager* CXTPControlGallery::GetGalleryPaintManager() const
{
	return GetPaintManager()->GetGalleryPaintManager();
}

CRect CXTPControlGallery::GetItemsRect() const
{
	CRect rc = GetRect();
	rc.DeflateRect(GetBorders());

	return rc;
}

void CXTPControlGallery::FillControl(CDC* pDC, CRect rcControl)
{
	GetGalleryPaintManager()->FillControl(pDC, this, rcControl);
}

void CXTPControlGallery::SetScrollPos(int nScrollPos)
{
	CRect rcItems = GetItemsRect();

	if (nScrollPos > m_nTotalHeight - rcItems.Height())
		nScrollPos = m_nTotalHeight - rcItems.Height();

	if (nScrollPos < 0)
		nScrollPos = 0;

	if (m_nScrollPos == nScrollPos)
		return;

	m_nScrollPos = nScrollPos;
	Reposition();

	OnScrollChanged();
	InvalidateItems(NULL, FALSE);
}


void CXTPControlGallery::EnsureVisible(int nIndex)
{
	if (!GetParent()->GetSafeHwnd())
		return;

	if (GetItemCount() == 0 || nIndex == -1 || nIndex >= GetItemCount())
		return;

	const GALLERYITEM_POSITION& pos = m_arrRects[nIndex];

	CRect rcItem = pos.rcItem;
	rcItem.OffsetRect(0, -m_nScrollPos);

	CRect rcItems = GetItemsRect();

	if (rcItem.top < rcItems.top)
	{
		int nScrollPos = pos.rcItem.top - rcItems.top;

		if (m_bShowLabels)
		{
			for (int i = nIndex - 1; i >= 0; i--)
			{
				const GALLERYITEM_POSITION& posLabel = m_arrRects[i];

				if (posLabel.pItem->IsLabel() && pos.rcItem.top - posLabel.rcItem.top <= rcItems.Height() - rcItem.Height())
				{
					nScrollPos = posLabel.rcItem.top - rcItems.top;
					break;
				}

				if (pos.rcItem.top - posLabel.rcItem.top > rcItems.Height()  - rcItem.Height())
					break;
			}
		}

		SetScrollPos(nScrollPos);
	}
	else if (rcItem.bottom > rcItems.bottom)
	{
		SetScrollPos(pos.rcItem.bottom - rcItems.bottom);
	}
}

long CXTPControlGallery::GetNextInRow(long nIndex, int nDirection) const
{
	if (GetItemCount() == 0 || nIndex == -1 || nIndex >= GetItemCount())
		return -1;

	CRect rcItems = GetItemsRect();
	CSize szItem = GetItems()->GetItemSize();

	if (szItem.cx * 2 > rcItems.Width() || szItem.cx == 0)
		return nIndex;
	int i;

	if (nDirection < 0)
	{
		if (!m_arrRects[nIndex].bBeginRow)
		{
			for (i = nIndex - 1; i >= 0; i--)
			{
				const GALLERYITEM_POSITION& pos = m_arrRects[i];

				if (!IsRectEmpty(&pos.rcItem))
				{
					return i;
				}
			}
		}

		for (i = nIndex + 1; i < m_arrRects.GetSize(); i++)
		{
			const GALLERYITEM_POSITION& pos = m_arrRects[i];

			if (pos.bBeginRow)
			{
				return nIndex;
			}

			if (!IsRectEmpty(&pos.rcItem))
			{
				nIndex = i;
			}
		}
		return nIndex;
	}
	else
	{
		for (i = nIndex + 1; i < GetItemCount(); i++)
		{
			const GALLERYITEM_POSITION& pos = m_arrRects[i];

			if (!IsRectEmpty(&pos.rcItem) && !pos.bBeginRow)
			{
				return i;
			}

			if (pos.bBeginRow)
				break;
		}

		for (i = nIndex; i >= 0; i--)
		{
			const GALLERYITEM_POSITION& pos = m_arrRects[i];

			if (pos.bBeginRow)
			{
				return i;
			}
		}
	}

	return nIndex;
}

long CXTPControlGallery::GetNextInPage(long nIndex, int nDirection) const
{
	if (GetItemCount() == 0)
		return -1;

	if (nIndex == -1 || nIndex >= GetItemCount())
		nIndex = GetNext(-1, nDirection);

	int i;
	RECT rcItem = m_arrRects[nIndex].rcItem;
	CRect rcItems = GetItemsRect();

	if (nDirection < 0)
	{
		for (i = nIndex - 1; i >= 0; i--)
		{
			const GALLERYITEM_POSITION& pos = m_arrRects[i];

			if (!pos.pItem->IsLabel() &&
				rcItem.top - pos.rcItem.top< rcItems.Height())
			{
				nIndex = i;
			}

			if (rcItem.top - pos.rcItem.top> rcItems.Height())
				return nIndex;
		}
	}
	else
	{
		for (i = nIndex + 1; i < GetItemCount(); i++)
		{
			const GALLERYITEM_POSITION& pos = m_arrRects[i];

			if (!pos.pItem->IsLabel() &&
				pos.rcItem.bottom - rcItem.bottom < rcItems.Height())
			{
				nIndex = i;
			}

			if (pos.rcItem.bottom - rcItem.bottom > rcItems.Height())
				return nIndex;
		}
	}

	return nIndex;

}

long CXTPControlGallery::GetNextInColumn(long nIndex, int nDirection) const
{
	if (GetItemCount() == 0)
		return -1;

	if (nIndex == -1 || nIndex >= GetItemCount())
		return GetNext(-1, nDirection);

	int i;
	RECT rcItem = m_arrRects[nIndex].rcItem;

	if (nDirection < 0)
	{
		for (i = nIndex - 1; i >= 0; i--)
		{
			const GALLERYITEM_POSITION& pos = m_arrRects[i];

			if (!pos.pItem->IsLabel() && pos.rcItem.left == rcItem.left)
			{
				return i;
			}
		}
	}
	else
	{
		for (i = nIndex + 1; i < GetItemCount(); i++)
		{
			const GALLERYITEM_POSITION& pos = m_arrRects[i];

			if (!pos.pItem->IsLabel() && pos.rcItem.left == rcItem.left)
			{
				return i;
			}
		}

	}

	return -1;
}

long CXTPControlGallery::GetNext(long nIndex, int nDirection) const
{
	ASSERT(nDirection == +1 || nDirection == -1);
	if (GetItemCount() == 0) return -1;

	long nNext = nIndex + nDirection;

	if (nDirection == -1 && nIndex == -1)
	{
		nNext = GetItemCount() - 1;
	}

	BOOL bCircle = FALSE;

	while (nNext != nIndex)
	{
		if (nNext >= GetItemCount())
		{
			if ((nIndex == -1 && nDirection == +1) || bCircle) return -1;
			nNext = 0;
			bCircle = TRUE;
		}
		if (nNext < 0)
		{
			if ((nIndex == -1 && nDirection == -1) || bCircle) return -1;
			nNext = GetItemCount() - 1;
			bCircle = TRUE;
		}

		if (!GetItem(nNext)->IsLabel())
		{
			return nNext;
		}
		nNext += nDirection;
	}
	return nNext;
}

void CXTPControlGallery::SetSelectedItem(int nItem)
{
	int nSelected = m_nSelected;

	m_bHideSelection = FALSE;
	m_nSelected = nItem;
	m_bPressed = FALSE;
	m_bKeyboardSelected = TRUE;

	CXTPControlGalleryItems* pItems = GetItems();

	if (pItems && pItems->m_bClipItems && nSelected != -1)
	{
		InvalidateItems(GetItemDrawRect(nSelected), TRUE);
	}

	EnsureVisible(m_nSelected);

	if (pItems && pItems->m_bClipItems && m_nSelected != -1)
	{
		InvalidateItems(GetItemDrawRect(m_nSelected), FALSE);
	}

	if (!pItems || !pItems->m_bClipItems)
	{
		InvalidateItems();
	}

	if (m_nSelected != -1)
	{
		AccessibleNotifyWinEvent(EVENT_OBJECT_FOCUS , GetParent()->GetSafeHwnd(), GetID(), m_nSelected + 1);
	}

	if (m_nSelected != -1 && !m_bPreview)
	{
		m_bPreview = TRUE;
		NotifySite(XTP_GN_PREVIEWSTART);
	}

	if (m_bPreview)
	{
		NotifySite(XTP_GN_PREVIEWCHANGE);
	}

	if (m_nSelected == -1 && m_bPreview)
	{
		m_bPreview = FALSE;
		NotifySite(XTP_GN_PREVIEWCANCEL);
	}
}

int CXTPControlGallery::FindItem(int nStartAfter, LPCTSTR lpszItem, BOOL bExact) const
{
	if (GetItemCount() == 0)
		return -1;

	UINT len1 = lpszItem ? (UINT)_tcslen(lpszItem) : 0;
	if (len1 < 1)
		return -1;

	int nItem = nStartAfter + 1;
	if (nItem >= GetItemCount())
		nItem = 0;

	if (nItem < 0)
		nItem = 0;

	int nStop = nItem;

	do
	{
		CXTPControlGalleryItem* pItem = GetItem(nItem);

		if (bExact )
		{
			if (pItem->GetCaption().CompareNoCase(lpszItem) == 0)
				return nItem;
		}
		else
		{
			UINT len2 = pItem->GetCaption().GetLength();

			int nResult = CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, lpszItem, min(len1,len2),
				pItem->GetCaption(), min(len1,len2));

			if (nResult == CSTR_EQUAL)
			{
				if (len1 <= len2)
				{
					return nItem;
				}
			}


		}

		if (++nItem == GetItemCount())
			nItem = 0;

	} while (nItem != nStop);

	return -1;
}

int CXTPControlGallery::GetScrollPos() const
{
	return m_nScrollPos;
}

int CXTPControlGallery::SetTopIndex(int nIndex)
{
	if (nIndex == -1 || nIndex > m_arrRects.GetSize())
		return -1;

	SetScrollPos(m_arrRects[nIndex].rcItem.top);
	return 0;
}

CString CXTPControlGallery::GetItemCaption(int nIndex)
{
	CXTPControlGalleryItem* pItem = GetItem(nIndex);

	if (pItem)
		return pItem->GetCaption();

	return _T("");
}

void CXTPControlGallery::DoScroll(int cmd, int pos)
{
	int y = m_nScrollPos;
	CRect rcItems = GetItemsRect();

	CXTPControlGalleryItems* pItems = GetItems();
	if (!pItems)
		return;

	switch (cmd)
	{
	case SB_TOP:
		y = 0;
		break;
	case SB_BOTTOM:
		y = m_nTotalHeight;
		break;
	case SB_LINEUP:
		y -= pItems->GetItemSize().cy;
		break;
	case SB_LINEDOWN:
		y += pItems->GetItemSize().cy;
		break;
	case SB_PAGEUP:
		y -= rcItems.Height();
		break;
	case SB_PAGEDOWN:
		y += rcItems.Height();
		break;
	case SB_THUMBTRACK:
		y = pos;
		break;
	}

	SetScrollPos(y);
}

void CXTPControlGallery::GetScrollInfo(SCROLLINFO* pSI)
{
	CRect rcItems = GetItemsRect();

	pSI->nMin = 0;
	pSI->nMax = max(0, m_nTotalHeight - 1);
	pSI->nPos = m_nScrollPos;
	pSI->nPage = rcItems.Height();
}


//////////////////////////////////////////////////////////////////////////

CSize CXTPControlGallery::GetSize(CDC* pDC)
{
	if (IsShowAsButton())
	{
		int nWidth = m_nWidth, nHeight = m_nHeight;
		m_nWidth = m_nHeight = 0;

		CSize sz = CXTPControlPopup::GetSize(pDC);
		m_nWidth = nWidth; m_nHeight = nHeight;
		return sz;
	}

	return CSize(m_nWidth, m_nHeight);
}

void CXTPControlGallery::RedrawScrollBar()
{
	InvalidateItems();
}

void CXTPControlGallery::DrawScrollBar(CDC* pDC)
{
	if (GetCommandBar())
		GetGalleryPaintManager()->DrawPopupScrollBar(pDC, this);
	else
		GetGalleryPaintManager()->DrawScrollBar(pDC, this);
}

void CXTPControlGallery::Draw(CDC* pDC)
{
	if (IsShowAsButton())
	{
		CXTPControlPopup::Draw(pDC);
		return;
	}
	FillControl(pDC, m_rcControl);

	DrawItems(pDC);

	if (m_bShowScrollBar)
	{
		DrawScrollBar(pDC);
	}
}

void CXTPControlGallery::OnMouseMove(CPoint point)
{
	if (IsShowAsButton())
	{
		CXTPControlPopup::OnMouseMove(point);
		return;
	}

	PerformMouseMove(point);

	int ht = HitTestScrollBar(point);

	if (ht != m_spi.ht)
	{
		m_spi.ht = ht;
		RedrawParent();
	}
}

void CXTPControlGallery::HideSelection()
{
	m_bHideSelection = TRUE;
	m_bKeyboardSelected = FALSE;

	if (m_nSelected != -1)
	{
		CXTPControlGalleryItems* pItems = GetItems();

		if (pItems && pItems->m_bClipItems)
		{
			InvalidateItems(GetItemDrawRect(m_nSelected), FALSE);
		}
		else
		{
			InvalidateItems();
		}
	}

	if (m_bPreview && m_nSelected != -1)
	{
		NotifySite(XTP_GN_PREVIEWCHANGE);
	}

	if (m_bPreview)
	{
		m_bPreview = FALSE;
		NotifySite(XTP_GN_PREVIEWCANCEL);
	}
}

BOOL CXTPControlGallery::OnSetSelected(int bSelected)
{
	if (IsShowAsButton())
	{
		return CXTPControlPopup::OnSetSelected(bSelected);
	}

	if (!bSelected)
	{
		m_spi.ht = HTNOWHERE;
		HideSelection();
	}

	if (IsKeyboardSelected(bSelected) && !IsItemSelected())
	{
		SetSelectedItem(GetNext(-1, bSelected == TRUE_KEYBOARD_PREV ? -1 : +1));
	}

	return CXTPControl::OnSetSelected(bSelected);
}


BOOL CXTPControlGallery::IsFocused() const
{
	return IsShowAsButton() ? FALSE : GetSelected();
}


BOOL CXTPControlGallery::OnHookKeyDown(UINT nChar, LPARAM /*lParam*/)
{
	ASSERT(IsFocused());

	if (!IsFocused())
		return FALSE;

	int nSelected;
	CXTPDrawHelpers::KeyToLayout(CXTPControl::GetParent(), nChar);

	switch (nChar)
	{
	case VK_HOME:
		SetSelectedItem(GetNext(-1, +1));
		return TRUE;

	case VK_END:
		SetSelectedItem(GetNext(-1, -1));
		return TRUE;

	case VK_LEFT:
		SetSelectedItem(GetNextInRow(m_nSelected, -1));
		return TRUE;

	case VK_RIGHT:
		SetSelectedItem(GetNextInRow(m_nSelected, +1));
		return TRUE;

	case VK_UP:
		nSelected = GetNextInColumn(m_nSelected, -1);
		if (nSelected != -1)
		{
			SetSelectedItem(nSelected);
			return TRUE;
		}
		return FALSE;

	case VK_PRIOR:
		SetSelectedItem(GetNextInPage(m_nSelected, -1));
		return TRUE;

	case VK_NEXT:
		SetSelectedItem(GetNextInPage(m_nSelected, +1));
		return TRUE;

	case VK_DOWN:
		nSelected = GetNextInColumn(m_nSelected, +1);
		if (nSelected != -1)
		{
			SetSelectedItem(nSelected);
			return TRUE;
		}
		return FALSE;

	case VK_TAB:
		if (GetKeyState(VK_SHIFT) >= 0)
		{
			nSelected = GetNext(m_nSelected, +1);
			if (nSelected > m_nSelected)
			{
				SetSelectedItem(nSelected);
				return TRUE;
			}
		}
		else
		{
			nSelected = GetNext(m_nSelected, -1);
			if (nSelected < m_nSelected)
			{
				SetSelectedItem(nSelected);
				return TRUE;
			}
		}
		return FALSE;

	case VK_RETURN:
		OnExecute();
		break;
	}

	return FALSE;
}

BOOL CXTPControlGallery::OnHookMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/)
{
	CXTPControlGalleryItems* pItems = GetItems();
	if (!pItems)
		return FALSE;

	SetScrollPos(m_nScrollPos + (zDelta < 0 ? 3 * pItems->GetItemSize().cy : -3 * pItems->GetItemSize().cy));
	return TRUE;
}

void CXTPControlGallery::OnScrollChanged()
{
	SetupScrollInfo();
}

void CXTPControlGallery::OnClick(BOOL bKeyboard /*= FALSE*/, CPoint point /*= CPoint(0, 0)*/)
{
	if (IsShowAsButton())
	{
		CXTPControlPopup::OnClick(bKeyboard, point);
		return;
	}

	if (IsCustomizeMode())
	{
		m_pParent->SetPopuped(-1);
		m_pParent->SetSelected(-1);
		CustomizeStartDrag(point);
		return;
	}

	if (!GetEnabled())
		return;

	if (!bKeyboard)
	{
		OnMouseMove(point);

		if (m_spi.ht == XTP_HTSCROLLPOPUP)
		{
			m_pParent->SetPopuped(m_nIndex);
		}
		else if (m_spi.ht != HTNOWHERE)
		{
			PerformTrackInit(CXTPControl::GetParent()->GetSafeHwnd(), point, &m_spi, (GetKeyState(VK_SHIFT) < 0) ? TRUE : FALSE);
		}
		else
		{
			m_pParent->SetPopuped(-1);
			PerformMouseDown(point);
		}
	}
	else
	{

	}
}

void CXTPControlGallery::OnLButtonUp(CPoint point)
{
	if (IsShowAsButton())
	{
		CXTPControlPopup::OnLButtonUp(point);
	}
	else
	{
		PerformMouseUp(point);
	}
}


CRect CXTPControlGallery::GetScrollBarRect()
{
	if (!m_bShowScrollBar)
		return CRect(0, 0, 0, 0);

	CRect rc(m_rcControl);

	rc.left = rc.right - (GetCommandBar() ? GetGalleryPaintManager()->m_cxPopup :
		GetGalleryPaintManager()->m_cxVScroll);

	if (m_bShowBorders)
	{
		rc.top++;
		rc.bottom--;
		rc.right--;
		rc.left--;
	}

	if (HasBottomSeparator())
		rc.bottom -= 2;

	return rc;
}

void CXTPControlGallery::CalcScrollBarInfo(LPRECT lprc, SCROLLBARPOSINFO* pSBInfo, SCROLLINFO* pSI)
{
	if (GetCommandBar())
	{
		pSBInfo->rc = *lprc;
		pSBInfo->pxTop = lprc->top;
		pSBInfo->pxBottom = lprc->bottom;
		pSBInfo->pxLeft = lprc->left;
		pSBInfo->pxRight = lprc->right;

		pSBInfo->pos = pSI->nPos;
		pSBInfo->page = pSI->nPage;
		pSBInfo->posMin = pSI->nMin;
		pSBInfo->posMax = pSI->nMax;

		pSBInfo->pxDownArrow = pSBInfo->pxUpArrow  = pSBInfo->pxTop + GetGalleryPaintManager()->m_cyPopupUp;
		pSBInfo->pxPopup = pSBInfo->pxDownArrow + GetGalleryPaintManager()->m_cyPopupDown;
	}
	else
	{
		CXTPScrollBase::CalcScrollBarInfo(lprc, pSBInfo, pSI);
	}
}

CWnd* CXTPControlGallery::GetParentWindow() const
{
	return CXTPControl::GetParent();
}

void CXTPControlGallery::SetRect(CRect rcControl)
{
	if (m_rcControl == rcControl)
		return;

	CXTPControl::SetRect(rcControl);
	Reposition();
}

void CXTPControlGallery::Copy(CXTPControl* pControl, BOOL bRecursive)
{
	ASSERT(DYNAMIC_DOWNCAST(CXTPControlGallery, pControl));

	CXTPControlPopup::Copy(pControl, bRecursive);

	CXTPControlGallery* pGallery = (CXTPControlGallery*)pControl;

	m_bShowLabels = pGallery->m_bShowLabels;
	m_bShowBorders = pGallery->m_bShowBorders;
	m_bShowScrollBar = pGallery->m_bShowScrollBar;

	m_rcMargin = pGallery->m_rcMargin;
}

void CXTPControlGallery::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControlPopup::DoPropExchange(pPX);

	PX_Bool(pPX, _T("ShowLabels"), m_bShowLabels, TRUE);
	PX_Bool(pPX, _T("ShowBorders"), m_bShowBorders, TRUE);
	PX_Bool(pPX, _T("ShowScrollBar"), m_bShowScrollBar, TRUE);

	PX_Rect(pPX, _T("GalleryMargin"), m_rcMargin, CRect(0, 0, 0, 0));
}

CString CXTPControlGallery::GetTooltip(LPPOINT pPoint, LPRECT lpRectTip, INT_PTR* nHit) const
{
	if (pPoint == NULL || lpRectTip == NULL || nHit == NULL)
		return  _T("");

	if (IsShowAsButton())
	{
		return CXTPControlPopup::GetTooltip(pPoint, lpRectTip, nHit);
	}

	RECT rcItem;
	int nIndex = HitTestItem(*pPoint, &rcItem);
	if (nIndex != -1)
	{
		*nHit = GetItem(nIndex)->GetID() + 1;
		*lpRectTip = rcItem;

		return GetItem(nIndex)->GetToolTip();
	}
	return _T("");
}

BOOL CXTPControlGallery::IsScrollBarEnabled() const
{
	return GetEnabled();
}

BOOL CXTPControlGallery::IsScrollButtonEnabled(int ht)
{
	if (!GetEnabled())
		return FALSE;

	if (!GetCommandBar())
		return TRUE;

	if (ht == XTP_HTSCROLLUP)
	{
		return m_nScrollPos > 0;
	}
	else if (ht == XTP_HTSCROLLDOWN)
	{
		CRect rcItems = GetItemsRect();
		return m_nScrollPos < m_nTotalHeight - rcItems.Height();
	}
	else
	{
		ASSERT(FALSE);
	}

	return TRUE;
}

void CXTPControlGallery::SetItemsMargin(int nLeft, int nTop, int nRight, int nBottom)
{
	m_rcMargin.SetRect(nLeft, nTop, nRight, nBottom);
}

void CXTPControlGallery::AdjustExcludeRect(CRect& rc, BOOL bVertical)
{
	if (IsShowAsButton())
	{
		CXTPControlPopup::AdjustExcludeRect(rc, bVertical);
	}
	else
	{
		rc.SetRect(m_rcControl.left - 1, m_rcControl.top, m_rcControl.left - 1, m_rcControl.top);
	}
}

HRESULT CXTPControlGallery::GetAccessibleChildCount(long FAR* pChildCount)
{
	if (pChildCount == 0)
		return E_INVALIDARG;

	*pChildCount = GetItemCount();

	return S_OK;
}
HRESULT CXTPControlGallery::GetAccessibleChild(VARIANT /*varChild*/, IDispatch* FAR* ppdispChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	*ppdispChild = NULL;
	return S_FALSE;
}

HRESULT CXTPControlGallery::GetAccessibleName(VARIANT varChild, BSTR* pszName)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	int nIndex = GetChildIndex(&varChild);

	if (nIndex == CHILDID_SELF)
		return CXTPControl::GetAccessibleName(varChild, pszName);

	CXTPControlGalleryItem* pItem = GetItem(nIndex - 1);
	if (!pItem)
		return E_INVALIDARG;

	CString strCaption = pItem->GetCaption();
	CXTPDrawHelpers::StripMnemonics(strCaption);

	if (strCaption.IsEmpty())
	{
		strCaption = pItem->GetToolTip();
	}

	*pszName = strCaption.AllocSysString();
	return S_OK;
}

HRESULT CXTPControlGallery::GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole)
{
	pvarRole->vt = VT_I4;
	pvarRole->lVal = GetChildIndex(&varChild) == CHILDID_SELF ? ROLE_SYSTEM_LISTITEM : ROLE_SYSTEM_LIST;
	return S_OK;
}

HRESULT CXTPControlGallery::AccessibleSelect(long flagsSelect, VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	int nIndex = GetChildIndex(&varChild);
	if (nIndex == CHILDID_SELF)
		return CXTPControl::AccessibleSelect(flagsSelect, varChild);

	SetSelectedItem(nIndex - 1);

	return S_OK;
}

HRESULT CXTPControlGallery::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
{
	int nIndex = GetChildIndex(&varChild);

	if (nIndex == CHILDID_SELF)
		return CXTPControl::GetAccessibleState(varChild, pvarState);

	CXTPControlGalleryItem* pItem = GetItem(nIndex - 1);
	if (!pItem)
		return E_INVALIDARG;

	pvarState->vt = VT_I4;
	pvarState->lVal = STATE_SYSTEM_FOCUSABLE | STATE_SYSTEM_SELECTABLE;

	if (m_nSelected == pItem->GetIndex())
		pvarState->lVal |= STATE_SYSTEM_FOCUSED;

	if (m_nChecked == pItem->GetID())
		pvarState->lVal |= STATE_SYSTEM_SELECTED;

	return S_OK;
}

HRESULT CXTPControlGallery::GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction)
{
	if (GetChildIndex(&varChild) == CHILDID_SELF)
			return CXTPControl::GetAccessibleDefaultAction(varChild, pszDefaultAction);

	*pszDefaultAction = SysAllocString(L"Click");

	return S_OK;
}

HRESULT CXTPControlGallery::AccessibleDoDefaultAction(VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	int nIndex = GetChildIndex(&varChild);

	if (nIndex == CHILDID_SELF)
		return CXTPControl::AccessibleDoDefaultAction(varChild);

	CXTPControlGalleryItem* pItem = GetItem(nIndex - 1);
	if (!pItem)
		return E_INVALIDARG;

	SetSelectedItem(nIndex - 1);
	return S_OK;
}


HRESULT CXTPControlGallery::AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
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
		rcControl = GetItemDrawRect(nIndex - 1);
	}

	m_pParent->ClientToScreen(&rcControl);

	*pxLeft = rcControl.left;
	*pyTop = rcControl.top;
	*pcxWidth = rcControl.Width();
	*pcyHeight = rcControl.Height();

	return S_OK;
}

HRESULT CXTPControlGallery::AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarID)
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

	int nIndex = HitTestItem(pt);
	if (nIndex != -1)
	{
		pvarID->lVal = nIndex + 1;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// CXTPControlComboBoxGalleryPopupBar

IMPLEMENT_XTP_COMMANDBAR(CXTPControlComboBoxGalleryPopupBar, CXTPControlComboBoxPopupBar)

CXTPControlComboBoxGalleryPopupBar::CXTPControlComboBoxGalleryPopupBar()
{
}

CXTPControlComboBoxGalleryPopupBar* AFX_CDECL CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(CXTPCommandBars* pCommandBars)
{
	CXTPControlComboBoxGalleryPopupBar* pPopupBar = new CXTPControlComboBoxGalleryPopupBar();
	pPopupBar->SetCommandBars(pCommandBars);
	return pPopupBar;
}

BEGIN_MESSAGE_MAP(CXTPControlComboBoxGalleryPopupBar, CXTPControlComboBoxPopupBar)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CXTPControlGallery* CXTPControlComboBoxGalleryPopupBar::GetGalleryItem() const
{
	CXTPControl* pItem = GetControl(0);
	ASSERT_KINDOF(CXTPControlGallery, pItem);

	return (CXTPControlGallery*)pItem;
}

void CXTPControlComboBoxGalleryPopupBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetControls()->HitTest(point) == GetGalleryItem())
	{
		CXTPControlComboBox* pComboBox = (CXTPControlComboBox*)m_pControlPopup;
		pComboBox->OnSelChanged();
		pComboBox->OnExecute();
		return;
	}

	CXTPPopupBar::OnLButtonUp(nFlags, point);
}

BOOL CXTPControlComboBoxGalleryPopupBar::SetTrackingMode(int bMode, BOOL bSelectFirst, BOOL bKeyboard)
{
	CXTPControlComboBox* pComboBox = ((CXTPControlComboBox*)m_pControlPopup);

	if (!CXTPPopupBar::SetTrackingMode(bMode, bSelectFirst, bKeyboard))
		return FALSE;

	if (!bMode)
	{
		pComboBox->NotifySite(CBN_CLOSEUP);

	}
	else
	{
		SetSelected(0, TRUE);

		pComboBox->UpdatePopupSelection();
		pComboBox->NotifySite(CBN_DROPDOWN);

	}

	return TRUE;
}

BOOL CXTPControlComboBoxGalleryPopupBar::OnHookKeyDown(UINT nChar, LPARAM lParam)
{
	ASSERT(m_pControlPopup);
	CXTPControlComboBox* pComboBox = ((CXTPControlComboBox*)m_pControlPopup);
	if (!pComboBox)
		return CXTPCommandBar::OnHookKeyDown(nChar, lParam);

	if (nChar == VK_ESCAPE)
	{
		if (pComboBox->IsFocused())
			return FALSE;

		return CXTPCommandBar::OnHookKeyDown(nChar, lParam);
	}

	if (nChar == VK_TAB) return FALSE;

	if (nChar == VK_RETURN)
	{
		if (GetControl(m_nSelected) == GetGalleryItem())
		{
			m_pControlPopup->OnExecute();
			return TRUE;
		}
	}

	return ProcessHookKeyDown(pComboBox, nChar, lParam);
}

//////////////////////////////////////////////////////////////////////////
//
CXTPControlGalleryPaintManager::CXTPControlGalleryPaintManager(CXTPPaintManager* pPaintManager)
{
	m_pPaintManager = pPaintManager;

	m_cxHScroll = GetSystemMetrics(SM_CXHSCROLL);
	m_cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	m_cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	m_cyVScroll = GetSystemMetrics(SM_CYVSCROLL);

	m_cyPopupUp = m_cyPopupDown = 19;
	m_cxPopup = 16;
}

void CXTPControlGalleryPaintManager::DrawLabel(CDC* pDC, CXTPControlGalleryItem* pLabel, CRect rcItem)
{
	CXTPPaintManager* pPaintManager = m_pPaintManager;
	pDC->FillSolidRect(rcItem, pPaintManager->GetXtremeColor(XPCOLOR_LABEL));

	pDC->FillSolidRect(rcItem.left, rcItem.bottom - 1, rcItem.Width(), 1, pPaintManager->GetXtremeColor(XPCOLOR_3DSHADOW));

	CXTPFontDC fnt(pDC, pPaintManager->GetRegularBoldFont());

	CRect rcText(rcItem);
	rcText.DeflateRect(10, 0);
	pDC->SetTextColor(pPaintManager->GetXtremeColor(XPCOLOR_MENUBAR_TEXT));
	pDC->DrawText(pLabel->GetCaption(), rcText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void CXTPControlGalleryPaintManager::FillControl(CDC* pDC, CXTPControlGallery* pGallery, CRect rcControl)
{
	pDC->FillSolidRect(rcControl, m_pPaintManager->GetXtremeColor(XPCOLOR_MENUBAR_FACE));

	if (pGallery->IsShowBorders())
	{
		pDC->Draw3dRect(rcControl, m_pPaintManager->GetXtremeColor(XPCOLOR_3DSHADOW),
			m_pPaintManager->GetXtremeColor(XPCOLOR_3DSHADOW));
	}

	if (pGallery->HasBottomSeparator())
	{
		pDC->FillSolidRect(rcControl.left, rcControl.bottom - 2, rcControl.Width(), 1, m_pPaintManager->GetXtremeColor(XPCOLOR_SEPARATOR));
	}
}

void CXTPControlGalleryPaintManager::RefreshMetrics()
{
	m_cxHScroll = GetSystemMetrics(SM_CXHSCROLL);
	m_cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	m_cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	m_cyVScroll = GetSystemMetrics(SM_CYVSCROLL);

	m_themeScrollBar.OpenTheme(0, L"SCROLLBAR");

	m_cyPopupUp = m_cyPopupDown = 19;
	m_cxPopup = 16;
}

void DrawArrowGlyph(CDC* pDC, CRect rcArrow, BOOL bHorz, BOOL bUpArrow, BOOL bEnabled)
{
	int nHeight = min(rcArrow.Width(), rcArrow.Height());
	if (nHeight < 6) return;

	int x = rcArrow.left + ((rcArrow.Width() - nHeight) / 2) + 2;
	int y = rcArrow.top + ((rcArrow.Height() - nHeight) / 2) + 2;
	nHeight -= 4;

	COLORREF clr = bEnabled ? RGB(0, 0, 0) : GetSysColor(COLOR_3DSHADOW);

	HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, SYMBOL_CHARSET, 0, 0, 0, 0, _T("MARLETT"));
	HFONT hOldFont = (HFONT)SelectObject(pDC->GetSafeHdc(), hFont);

	SetTextColor(pDC->GetSafeHdc(), clr);
	pDC->SetBkMode(TRANSPARENT);
	TextOut(pDC->GetSafeHdc(), x, y, bHorz ? (bUpArrow ? _T("3") : _T("4")) : (bUpArrow ? _T("5") : _T("6")), 1);

	SelectObject(pDC->GetSafeHdc(), hOldFont);
	DeleteObject(hFont);
}

void CXTPControlGalleryPaintManager::DrawScrollBar(CDC* pDC, CXTPScrollBase* pGallery)
{
	#define GETPARTSTATE(ht, pressed, hot, normal, disabled) \
		(!bEnabled ? disabled : nPressetHt == ht ? pressed : nHotHt == ht ? hot : normal)

	CXTPControlGallery::SCROLLBARTRACKINFO* pSBTrack = pGallery->GetScrollBarTrackInfo();
	CXTPControlGallery::SCROLLBARPOSINFO* pSBInfo = pGallery->GetScrollBarPosInfo();

	BOOL nPressetHt = pSBTrack ? (pSBTrack->bTrackThumb || pSBTrack->fHitOld ? pSBInfo->ht : -1) : -1;
	BOOL nHotHt = pSBTrack ? -1 : pSBInfo->ht;


	int cWidth = (pSBInfo->pxRight - pSBInfo->pxLeft);

	if (cWidth <= 0)
	{
		return;
	}

	BOOL bEnabled = (pSBInfo->posMax - pSBInfo->posMin - pSBInfo->page + 1 > 0) && pGallery->IsScrollBarEnabled();

	int nBtnTrackSize =   pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
	int nBtnTrackPos = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;

	if (!bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
		nBtnTrackPos = nBtnTrackSize = 0;
	XTPScrollBarStyle style = pGallery->GetScrollBarStyle();

	if (pSBInfo->fVert)
	{
		CRect rcVScroll(pSBInfo->rc);

		CRect rcArrowUp(rcVScroll.left, rcVScroll.top, rcVScroll.right, pSBInfo->pxUpArrow);
		CRect rcArrowDown(rcVScroll.left, pSBInfo->pxDownArrow, rcVScroll.right, rcVScroll.bottom);
		CRect rcTrack(rcVScroll.left, rcArrowUp.bottom, rcVScroll.right, rcArrowDown.top);

		CRect rcLowerTrack(rcTrack.left, rcTrack.top, rcTrack.right, rcTrack.top + nBtnTrackPos);
		CRect rcBtnTrack(rcTrack.left, rcLowerTrack.bottom, rcTrack.right, rcLowerTrack.bottom + nBtnTrackSize);
		CRect rcUpperTrack(rcTrack.left, rcBtnTrack.bottom, rcTrack.right, rcTrack.bottom);


		if (style == xtpScrollStyleSystem  && m_themeScrollBar.IsAppThemed())
		{
			m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_ARROWBTN, GETPARTSTATE(XTP_HTSCROLLUP, ABS_UPPRESSED, ABS_UPHOT, ABS_UPNORMAL, ABS_UPDISABLED),  rcArrowUp, NULL);
			m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_ARROWBTN, GETPARTSTATE(XTP_HTSCROLLDOWN, ABS_DOWNPRESSED, ABS_DOWNHOT, ABS_DOWNNORMAL, ABS_DOWNDISABLED),  rcArrowDown, NULL);

			if (!rcTrack.IsRectEmpty())
			{
				if (!rcLowerTrack.IsRectEmpty())
					m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_LOWERTRACKVERT, GETPARTSTATE(XTP_HTSCROLLUPPAGE, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED),  rcLowerTrack, NULL);

				if (!rcBtnTrack.IsRectEmpty())
				{
					m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_THUMBBTNVERT, GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED),  rcBtnTrack, NULL);
					if (rcBtnTrack.Height() > 13)
						m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_GRIPPERVERT, GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED),  rcBtnTrack, NULL);
				}

				if (!rcUpperTrack.IsRectEmpty())
					m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_UPPERTRACKVERT, GETPARTSTATE(XTP_HTSCROLLDOWNPAGE, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED),  rcUpperTrack, NULL);
			}
		}
		else
		{
			if (style == xtpScrollStyleFlat)
			{
				pDC->FillSolidRect(rcArrowUp, GetXtremeColor(COLOR_3DFACE));
				if (bEnabled && nPressetHt == XTP_HTSCROLLUP)
					pDC->Draw3dRect(rcArrowUp,GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHIGHLIGHT));
				else
					pDC->Draw3dRect(rcArrowUp, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_3DSHADOW));

				DrawArrowGlyph(pDC, rcArrowUp, FALSE, TRUE, bEnabled);

				pDC->FillSolidRect(rcArrowDown, GetXtremeColor(COLOR_3DFACE));
				if (bEnabled && nPressetHt == XTP_HTSCROLLDOWN)
					pDC->Draw3dRect(rcArrowDown,GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHIGHLIGHT));
				else
					pDC->Draw3dRect(rcArrowDown, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_3DSHADOW));
				DrawArrowGlyph(pDC, rcArrowDown, FALSE, FALSE, bEnabled);

			}
			else
			{
				DrawFrameControl(pDC->GetSafeHdc(), &rcArrowUp, DFC_SCROLL, DFCS_SCROLLUP | (!bEnabled ? DFCS_INACTIVE : 0) | (nPressetHt == XTP_HTSCROLLUP ? DFCS_PUSHED : 0));
				DrawFrameControl(pDC->GetSafeHdc(), &rcArrowDown, DFC_SCROLL, DFCS_SCROLLDOWN | (!bEnabled ? DFCS_INACTIVE : 0)  | (nPressetHt == XTP_HTSCROLLDOWN ? DFCS_PUSHED : 0));
			}

			CWnd* pParent = pGallery->GetParentWindow();
			HBRUSH hbrRet = (HBRUSH)DefWindowProc(pParent->GetSafeHwnd(), WM_CTLCOLORSCROLLBAR, (WPARAM)pDC->GetSafeHdc(), (LPARAM)pParent->GetSafeHwnd());

			::FillRect(pDC->GetSafeHdc(), &rcTrack, hbrRet);

			if (nPressetHt == XTP_HTSCROLLUPPAGE)
			{
				::InvertRect(pDC->GetSafeHdc(), &rcLowerTrack);
			}

			if (!rcTrack.IsRectEmpty() && !rcBtnTrack.IsRectEmpty())
			{
				pDC->FillSolidRect(rcBtnTrack, GetXtremeColor(COLOR_3DFACE));

				if (style == xtpScrollStyleFlat)
				{
					pDC->Draw3dRect(rcBtnTrack, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_3DSHADOW));
				}
				else
				{
					DrawEdge(pDC->GetSafeHdc(), &rcBtnTrack, EDGE_RAISED, (UINT)(BF_ADJUST | BF_RECT));
				}
			}

			if (nPressetHt == XTP_HTSCROLLDOWNPAGE)
			{
				::InvertRect(pDC->GetSafeHdc(), &rcUpperTrack);
			}
		}
	}
	else
	{
		CRect rcHScroll(pSBInfo->rc);

		CRect rcArrowLeft(rcHScroll.left, rcHScroll.top, pSBInfo->pxUpArrow, rcHScroll.bottom);
		CRect rcArrowRight(pSBInfo->pxDownArrow, rcHScroll.top, rcHScroll.right, rcHScroll.bottom);
		CRect rcTrack(rcArrowLeft.right, rcHScroll.top, rcArrowRight.left, rcHScroll.bottom);

		CRect rcLowerTrack(rcTrack.left, rcTrack.top, rcTrack.left + nBtnTrackPos, rcTrack.bottom);
		CRect rcBtnTrack(rcLowerTrack.right, rcTrack.top, rcLowerTrack.right + nBtnTrackSize, rcTrack.bottom);
		CRect rcUpperTrack(rcBtnTrack.right, rcTrack.top, rcTrack.right, rcTrack.bottom);


		if (style == xtpScrollStyleSystem  && m_themeScrollBar.IsAppThemed())
		{
			m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_ARROWBTN, GETPARTSTATE(XTP_HTSCROLLUP, ABS_LEFTPRESSED, ABS_LEFTHOT, ABS_LEFTNORMAL, ABS_LEFTDISABLED),  rcArrowLeft, NULL);
			m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_ARROWBTN, GETPARTSTATE(XTP_HTSCROLLDOWN, ABS_RIGHTPRESSED, ABS_RIGHTHOT, ABS_RIGHTNORMAL, ABS_RIGHTDISABLED),  rcArrowRight, NULL);

			if (!rcTrack.IsRectEmpty())
			{
				if (!rcLowerTrack.IsRectEmpty())
					m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_LOWERTRACKHORZ, GETPARTSTATE(XTP_HTSCROLLUPPAGE, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED),  rcLowerTrack, NULL);

				if (!rcBtnTrack.IsRectEmpty())
				{
					m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_THUMBBTNHORZ, GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED),  rcBtnTrack, NULL);
					if (rcBtnTrack.Width() > 13)
						m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_GRIPPERHORZ, GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED),  rcBtnTrack, NULL);
				}

				if (!rcUpperTrack.IsRectEmpty())
					m_themeScrollBar.DrawThemeBackground(pDC->GetSafeHdc(), SBP_UPPERTRACKHORZ, GETPARTSTATE(XTP_HTSCROLLDOWNPAGE, SCRBS_PRESSED, SCRBS_HOT, SCRBS_NORMAL, SCRBS_DISABLED),  rcUpperTrack, NULL);
			}
		}
		else
		{
			if (style == xtpScrollStyleFlat)
			{
				pDC->FillSolidRect(rcArrowLeft, GetXtremeColor(COLOR_3DFACE));
				if (bEnabled && nPressetHt == XTP_HTSCROLLUP)
					pDC->Draw3dRect(rcArrowLeft,GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHIGHLIGHT));
				else
					pDC->Draw3dRect(rcArrowLeft, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_3DSHADOW));

				DrawArrowGlyph(pDC, rcArrowLeft, TRUE, TRUE, bEnabled);

				pDC->FillSolidRect(rcArrowRight, GetXtremeColor(COLOR_3DFACE));
				if (bEnabled && nPressetHt == XTP_HTSCROLLDOWN)
					pDC->Draw3dRect(rcArrowRight,GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHIGHLIGHT));
				else
					pDC->Draw3dRect(rcArrowRight, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_3DSHADOW));
				DrawArrowGlyph(pDC, rcArrowRight, TRUE, FALSE, bEnabled);

			}
			else
			{
				DrawFrameControl(pDC->GetSafeHdc(), &rcArrowLeft, DFC_SCROLL, DFCS_SCROLLLEFT | (!bEnabled ? DFCS_INACTIVE : 0) | (nPressetHt == XTP_HTSCROLLUP ? DFCS_PUSHED : 0));
				DrawFrameControl(pDC->GetSafeHdc(), &rcArrowRight, DFC_SCROLL, DFCS_SCROLLRIGHT | (!bEnabled ? DFCS_INACTIVE : 0)  | (nPressetHt == XTP_HTSCROLLDOWN ? DFCS_PUSHED : 0));
			}

			CWnd* pParent = pGallery->GetParentWindow();
			HBRUSH hbrRet = (HBRUSH)DefWindowProc(pParent->GetSafeHwnd(), WM_CTLCOLORSCROLLBAR, (WPARAM)pDC->GetSafeHdc(), (LPARAM)pParent->GetSafeHwnd());

			::FillRect(pDC->GetSafeHdc(), &rcTrack, hbrRet);

			if (nPressetHt == XTP_HTSCROLLUPPAGE)
			{
				::InvertRect(pDC->GetSafeHdc(), &rcLowerTrack);
			}

			if (!rcTrack.IsRectEmpty() && !rcBtnTrack.IsRectEmpty())
			{
				pDC->FillSolidRect(rcBtnTrack, GetXtremeColor(COLOR_3DFACE));

				if (style == xtpScrollStyleFlat)
				{
					pDC->Draw3dRect(rcBtnTrack, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_3DSHADOW));
				}
				else
				{
					DrawEdge(pDC->GetSafeHdc(), &rcBtnTrack, EDGE_RAISED, (UINT)(BF_ADJUST | BF_RECT));
				}
			}

			if (nPressetHt == XTP_HTSCROLLDOWNPAGE)
			{
				::InvertRect(pDC->GetSafeHdc(), &rcUpperTrack);
			}
		}
	}
}

void CXTPControlGalleryPaintManager::DrawPopupScrollBar(CDC* pDC, CXTPControlGallery* pGallery)
{
	#define GETPARTSTATE3(ht, bEnabled) \
	(!bEnabled ? 4 : nPressetHt == ht ? 3 : nHotHt == ht ? 2 : 0)

	CXTPControlGallery::SCROLLBARTRACKINFO* pSBTrack = pGallery->GetScrollBarTrackInfo();
	CXTPControlGallery::SCROLLBARPOSINFO* pSBInfo = pGallery->GetScrollBarPosInfo();

	CRect rcScroll = pSBInfo->rc;
	pDC->FillSolidRect(rcScroll, m_pPaintManager->GetXtremeColor(XPCOLOR_TOOLBAR_FACE));

	CRect rcScrollUp(rcScroll.left, rcScroll.top, rcScroll.right, rcScroll.top + m_cyPopupUp);
	CRect rcScrollDown(rcScroll.left, rcScrollUp.bottom, rcScroll.right, rcScrollUp.bottom + m_cyPopupDown);
	CRect rcScrollPopup(rcScroll.left, rcScrollDown.bottom, rcScroll.right, rcScroll.bottom);

	BOOL bControlEnabled = pGallery->GetEnabled();
	BOOL nPressetHt = pSBTrack ? pSBInfo->ht : -1;
	BOOL nHotHt = pSBTrack ? -1 : pSBInfo->ht;

	int nState = GETPARTSTATE3(XTP_HTSCROLLUP, (bControlEnabled && pGallery->IsScrollButtonEnabled(XTP_HTSCROLLUP)));
	m_pPaintManager->DrawRectangle(pDC, rcScrollUp, nState == 3 || nState == 2, nState == 3, nState != 4, FALSE, FALSE, xtpBarTypeNormal, xtpBarPopup);
	COLORREF clr = m_pPaintManager->GetRectangleTextColor(nState == 2, nState == 3, nState != 4, FALSE, FALSE, xtpBarTypeNormal, xtpBarPopup);
	CPoint pt = rcScrollUp.CenterPoint();
	m_pPaintManager->Triangle(pDC, CPoint(pt.x - 4 , pt.y + 2), CPoint(pt.x + 4, pt.y + 2), CPoint (pt.x, pt.y - 2), clr);

	nState = GETPARTSTATE3(XTP_HTSCROLLDOWN, (bControlEnabled && pGallery->IsScrollButtonEnabled(XTP_HTSCROLLDOWN)));
	m_pPaintManager->DrawRectangle(pDC, rcScrollDown, nState == 3 || nState == 2, nState == 3, nState != 4, FALSE, FALSE, xtpBarTypeNormal, xtpBarPopup);
	clr = m_pPaintManager->GetRectangleTextColor(nState == 2, nState == 3, nState != 4, FALSE, FALSE, xtpBarTypeNormal, xtpBarPopup);
	pt = rcScrollDown.CenterPoint();
	m_pPaintManager->Triangle(pDC, CPoint(pt.x - 4 , pt.y - 2), CPoint(pt.x + 4, pt.y - 2), CPoint (pt.x, pt.y + 2), clr);

	nState = GETPARTSTATE3(XTP_HTSCROLLPOPUP, bControlEnabled);
	m_pPaintManager->DrawRectangle(pDC, rcScrollPopup, nState == 3 || nState == 2, nState == 3, nState != 4, FALSE, FALSE, xtpBarTypeNormal, xtpBarPopup);
	clr = m_pPaintManager->GetRectangleTextColor(nState == 2, nState == 3, nState != 4, FALSE, FALSE, xtpBarTypeNormal, xtpBarPopup);
	pt = rcScrollPopup.CenterPoint();
	m_pPaintManager->Triangle(pDC, CPoint(pt.x - 4 , pt.y - 1), CPoint(pt.x + 4, pt.y - 1), CPoint (pt.x, pt.y + 3), clr);
	pDC->FillSolidRect(pt.x - 4, pt.y - 4, 9, 2, clr);
}



