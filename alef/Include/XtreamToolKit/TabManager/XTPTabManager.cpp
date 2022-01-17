// XTPTabManager.cpp: implementation of the CXTPTabManager class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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

#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPToolTipContext.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPImageManager.h"

#include "XTPTabManager.h"
#include "XTPTabPaintManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CXTPTabManagerItem, CCmdTarget)

//////////////////////////////////////////////////////////////////////
// CXTPTabManagerItem

CXTPTabManagerItem::CXTPTabManagerItem()
{
	EnableAutomation();


	m_bVisible = TRUE;
	m_bEnabled = TRUE;


	m_hIcon = 0;
	m_hWnd = 0;
	m_clrItem = 0;
	m_nIndex = -1;
	m_nImage = -1;
	m_dwData = 0;
	m_rcItem.SetRectEmpty();
	m_nItemRow = 0;
	m_nButtonLength = m_nContentLength = 0;
	m_bClosable = TRUE;
	m_pTabManager = NULL;
	m_bFound = FALSE;
}

void CXTPTabManagerItem::OnRemoved()
{
	for (int j = 0; j < m_arrNavigateButtons.GetSize(); j++)
		delete m_arrNavigateButtons[j];

	m_arrNavigateButtons.RemoveAll();
}


CXTPTabManagerItem::~CXTPTabManagerItem()
{
	for (int j = 0; j < m_arrNavigateButtons.GetSize(); j++)
		delete m_arrNavigateButtons[j];
}



void CXTPTabManagerItem::Reposition()
{
	if (m_pTabManager) m_pTabManager->Reposition();
}

void CXTPTabManagerItem::SetCaption(LPCTSTR lpszCaption)
{
	if (m_strCaption != lpszCaption)
	{
		m_strCaption = lpszCaption;
		Reposition();
	}
}
void CXTPTabManagerItem::SetColor(COLORREF clr)
{
	m_clrItem = clr;
	m_pTabManager->RedrawControl(GetRect(), FALSE);
}

CString CXTPTabManagerItem::GetCaption() const
{
	return m_pTabManager->GetItemCaption(this);
}

CString CXTPTabManagerItem::GetTooltip() const
{
	return m_pTabManager->GetItemTooltip(this);
}

COLORREF CXTPTabManagerItem::GetColor()
{
	return m_pTabManager->GetItemColor(this);
}
void CXTPTabManagerItem::SetHandle(HWND hWnd)
{
	m_hWnd = hWnd;
}

void CXTPTabManagerItem::Remove()
{
	m_pTabManager->DeleteItem(m_nIndex);
}
void CXTPTabManagerItem::Select()
{
	m_pTabManager->OnItemClick(this);
}
HWND CXTPTabManagerItem::GetHandle() const
{
	return m_hWnd;
}
BOOL CXTPTabManagerItem::IsVisible() const
{
	return m_bVisible;
}

void CXTPTabManagerItem::SetVisible(BOOL bVisible)
{
	if (m_bVisible != bVisible)
	{
		m_bVisible = bVisible;
		Reposition();
	}
}

BOOL CXTPTabManagerItem::IsEnabled() const
{
	return m_bEnabled;
}

void CXTPTabManagerItem::SetEnabled(BOOL bEnabled)
{
	if (m_bEnabled != bEnabled)
	{
		m_bEnabled = bEnabled;
		Reposition();
	}
}

void CXTPTabManagerItem::Move(int nIndex)
{
	if (GetIndex() != nIndex)
	{
		m_pTabManager->MoveItem(this, nIndex);
	}
}

BOOL CXTPTabManagerItem::DrawRotatedImage(CDC* pDC, CRect rcItem, CXTPImageManagerIcon* pImage)
{
	CXTPImageManagerIconHandle& imageHandle = !IsEnabled() ? pImage->GetDisabledIcon(): IsHighlighted() ? pImage->GetHotIcon() :
		IsSelected() ? pImage->GetCheckedIcon() : pImage->GetIcon();

	const int cx = rcItem.Width();
	const int cy = rcItem.Height();
	ASSERT(cx == cy);

	BITMAPINFO BMI;
	// Fill in the header info.
	ZeroMemory (&BMI, sizeof(BITMAPINFO));
	BMI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BMI.bmiHeader.biWidth = cx;
	BMI.bmiHeader.biHeight = cy;
	BMI.bmiHeader.biPlanes = 1;
	BMI.bmiHeader.biBitCount = 32;
	BMI.bmiHeader.biCompression = BI_RGB;   // No compression

	UINT* pSrcBits = NULL, *pDestBits = NULL;
	HBITMAP hbmSrc = CreateDIBSection (NULL, &BMI, DIB_RGB_COLORS, (void **)&pSrcBits, 0, 0l);
	if (!pSrcBits)
		return FALSE;

	HBITMAP hbmDest = CreateDIBSection (NULL, &BMI, DIB_RGB_COLORS, (void **)&pDestBits, 0, 0l);
	if (!pDestBits)
		return FALSE;

	CDC dc;
	dc.CreateCompatibleDC(NULL);
	HGDIOBJ hbmpOld = ::SelectObject(dc, hbmSrc);

	CRect rcDraw(0, 0, cx, cy);

	dc.BitBlt(0, 0, cx, cy, pDC, rcItem.left, rcItem.top, SRCCOPY);
	//dc.FillSolidRect(rcDraw, GetXtremeColor(COLOR_3DFACE));
	::SelectObject(dc, hbmpOld);

	UINT* pSrcInv = pDestBits;
	UINT* pDestInv = pSrcBits;
	UINT* pSrc = pSrcBits;
	int i;

	for (i = 0; i < cy; i++)
	{
		pSrcBits = pSrc;
		for (int j = 0; j < cx; j++)
		{
			*pDestBits++ = *pSrcBits;
			pSrcBits += cy;
		}
		pSrc += 1;
	}

	hbmpOld = ::SelectObject(dc, hbmDest);
	pImage->Draw(&dc, rcDraw.TopLeft(), imageHandle, rcDraw.Size());
	::SelectObject(dc, hbmpOld);

	UINT* pDest = &pDestInv[cx * (cy - 1)];

	for (i = 0; i < cy; i++)
	{
		pDestInv = pDest;
		for (int j = 0; j < cx; j++)
		{
			*pDestInv = *pSrcInv;
			pSrcInv += 1;
			pDestInv -= cy;
		}
		pDest += 1;
	}

	pDC->DrawState(rcItem.TopLeft(), rcItem.Size(), hbmSrc, DST_BITMAP);

	DeleteObject(hbmSrc);
	DeleteObject(hbmDest);

	return TRUE;
}

void CXTPTabManagerItem::DrawImage(CDC* pDC, CRect rcIcon, CXTPImageManagerIcon* pImage)
{
	if (rcIcon.Width() == rcIcon.Height() && GetTabManager()->GetPaintManager()->m_bRotateImageOnVerticalDraw
		&& (GetTabManager()->GetPosition() == xtpTabPositionLeft || GetTabManager()->GetPosition() == xtpTabPositionRight)
		&& DrawRotatedImage(pDC, rcIcon, pImage))
	{

	}
	else
	{
		CXTPImageManagerIconHandle& imageHandle = !IsEnabled() ? pImage->GetDisabledIcon(): IsHighlighted() ? pImage->GetHotIcon() :
			IsSelected() ? pImage->GetCheckedIcon() : pImage->GetIcon();

		pImage->Draw(pDC, rcIcon.TopLeft(), imageHandle, rcIcon.Size());
	}
}

void CXTPTabManagerItem::SetRect(CRect rcItem)
{
	m_rcItem = rcItem;

	for (int i = (int)m_arrNavigateButtons.GetSize() - 1; i >= 0; i--)
	{
		CXTPTabManagerNavigateButton* pButton = m_arrNavigateButtons.GetAt(i);
		pButton->SetRect(CRect(0, 0, 0, 0));
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPTabManagerNavigateButton

CXTPTabManagerNavigateButton::CXTPTabManagerNavigateButton(CXTPTabManager* pManager, UINT nID, XTPTabNavigateButtonFlags dwFlags)
{
	m_pManager = pManager;
	m_nID = nID;
	m_dwFlags = dwFlags;

	m_rcButton.SetRectEmpty();

	m_bEnabled = TRUE;
	m_bPressed = FALSE;
	m_pItem = NULL;
}

CXTPTabManagerNavigateButton::~CXTPTabManagerNavigateButton()
{
	if (m_pManager->m_pHighlightedNavigateButton == this)
		m_pManager->m_pHighlightedNavigateButton = NULL;
}

CSize CXTPTabManagerNavigateButton::GetSize() const
{
	return m_pManager->GetPaintManager()->m_szNavigateButton;
}

void CXTPTabManagerNavigateButton::AdjustWidth(int& nWidth)
{
	if ((m_dwFlags == xtpTabNavigateButtonAlways) ||
		(m_dwFlags == xtpTabNavigateButtonAutomatic && m_pManager->IsNavigateButtonAutomaticVisible(this)))
	{
		if ((m_pManager->GetPosition() == xtpTabPositionTop) || (m_pManager->GetPosition() == xtpTabPositionBottom))
		{
			nWidth -= GetSize().cx;
		}
		else
		{
			nWidth -= GetSize().cy;
		}
	}
}

void CXTPTabManagerNavigateButton::SetRect(CRect rcButton)
{
	m_rcButton = rcButton;
}

void CXTPTabManagerNavigateButton::Reposition(CRect& rcNavigateButtons)
{

	if ((m_dwFlags == xtpTabNavigateButtonAlways) ||
		(m_dwFlags == xtpTabNavigateButtonAutomatic && m_pManager->IsNavigateButtonAutomaticVisible(this)))
	{
		CSize szButton = GetSize();

		if (m_pManager->IsHorizontalPosition())
		{
			m_rcButton.SetRect(rcNavigateButtons.right - szButton.cx, rcNavigateButtons.CenterPoint().y + szButton.cy / 2 - szButton.cy, rcNavigateButtons.right, rcNavigateButtons.CenterPoint().y + szButton.cy / 2);
			rcNavigateButtons.right -= szButton.cx;
		}
		else
		{
			m_rcButton.SetRect(rcNavigateButtons.CenterPoint().x - szButton.cx / 2, rcNavigateButtons.bottom - szButton.cy, rcNavigateButtons.CenterPoint().x - szButton.cx / 2 + szButton.cx, rcNavigateButtons.bottom);
			rcNavigateButtons.bottom -= szButton.cy;
		}
	}
	else
		m_rcButton.SetRectEmpty();

}

void CXTPTabManagerNavigateButton::Draw(CDC* pDC)
{
	if (!m_rcButton.IsRectEmpty())
	{
		CRect rc(m_rcButton);
		m_pManager->GetPaintManager()->DrawNavigateButton(pDC, this, rc);
	}
}

void CXTPTabManagerNavigateButton::PerformClick(HWND hWnd, CPoint pt)
{
	if ((::GetCapture() != NULL) || !m_bEnabled)
		return;

	::SetCapture(hWnd);

	BOOL  bClick = FALSE;

	DWORD dwStart = GetTickCount();
	for (;;)
	{
		if (m_bEnabled && GetTickCount() - dwStart > 20)
		{
			dwStart = GetTickCount();
			OnExecute(TRUE);
		}

		BOOL bPressed = m_rcButton.PtInRect(pt);

		if (bPressed != m_bPressed)
		{
			m_bPressed = bPressed;
			m_pManager->RedrawControl(m_rcButton, TRUE);
		}
		MSG msg;

		if (!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
			continue;

		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (::GetCapture() != hWnd)
		{
			DispatchMessage (&msg);
			goto ExitLoop;
		}
		switch (msg.message)
		{
			case WM_MOUSEMOVE:
				pt = CPoint((short signed)LOWORD(msg.lParam), (short signed)HIWORD(msg.lParam));
				break;

			case WM_LBUTTONUP:
				bClick = m_bPressed;
				goto ExitLoop;

			case WM_KEYDOWN:
				if (msg.wParam != VK_ESCAPE)
					break;

			case WM_CANCELMODE:
			case WM_RBUTTONDOWN:
				goto ExitLoop;

			default:
				DispatchMessage (&msg);
				break;
		}
	}

ExitLoop:
	ReleaseCapture();
	m_bPressed = FALSE;
	m_pManager->PerformMouseMove(hWnd, pt);
	m_pManager->RedrawControl(NULL, FALSE);

	if (bClick)
	{
		OnExecute(FALSE);
	}
}

void CXTPTabManagerNavigateButton::OnExecute(BOOL bTick)
{
	if (!bTick)
	{
		m_pManager->OnNavigateButtonClick(this);
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPTabManager::CRowIndexer


CXTPTabManager::CRowIndexer::CRowIndexer(CXTPTabManager* pManager)
{
	m_nRowCount = 0;
	m_pRowItems = 0;
	m_pManager = pManager;

	CreateIndexer(1);
}

CXTPTabManager::CRowIndexer::~CRowIndexer()
{
	SAFE_DELETE_AR(m_pRowItems);
}

CXTPTabManager::ROW_ITEMS* CXTPTabManager::CRowIndexer::CreateIndexer(int nRowCount)
{
	if (m_nRowCount != nRowCount)
	{
		SAFE_DELETE_AR(m_pRowItems);
		m_pRowItems = new ROW_ITEMS[nRowCount];
		m_nRowCount = nRowCount;
	}

	if (nRowCount > 0)
	{
		m_pRowItems[0].nFirstItem = 0;
		m_pRowItems[0].nLastItem = m_pManager->GetItemCount() - 1;
	}
	return m_pRowItems;
}

//////////////////////////////////////////////////////////////////////////
// CXTPTabManager::CNavigateButtonArrow

class CXTPTabManager::CNavigateButtonArrow : public CXTPTabManagerNavigateButton
{
protected:
	CNavigateButtonArrow(CXTPTabManager* pManager, XTPTabNavigateButton nID, XTPTabNavigateButtonFlags dwFlags)
		: CXTPTabManagerNavigateButton(pManager, nID, dwFlags)
	{
	}
	virtual void Reposition(CRect& rcNavigateButtons);
	virtual void AdjustWidth(int& nWidth);

};

class CXTPTabManager::CNavigateButtonArrowLeft : public CNavigateButtonArrow
{
public:
	CNavigateButtonArrowLeft(CXTPTabManager* pManager, XTPTabNavigateButtonFlags dwFlags)
		: CNavigateButtonArrow(pManager, xtpTabNavigateButtonLeft, dwFlags)
	{
		XTPResourceManager()->LoadString(&m_strToolTip, XTP_IDS_TABNAVIGATEBUTTON_LEFT);
	}
protected:
	virtual void DrawEntry(CDC* pDC, CRect rc);
	virtual void Reposition(CRect& rcNavigateButtons);
	virtual void OnExecute(BOOL bTick);
};

class CXTPTabManager::CNavigateButtonArrowRight : public CNavigateButtonArrow
{
public:
	CNavigateButtonArrowRight(CXTPTabManager* pManager, XTPTabNavigateButtonFlags dwFlags)
		: CNavigateButtonArrow(pManager, xtpTabNavigateButtonRight, dwFlags)
	{
		XTPResourceManager()->LoadString(&m_strToolTip, XTP_IDS_TABNAVIGATEBUTTON_RIGHT);
	}
protected:
	virtual void DrawEntry(CDC* pDC, CRect rc);
	virtual void Reposition(CRect& rcNavigateButtons);
	virtual void OnExecute(BOOL bTick);
};

class CXTPTabManager::CNavigateButtonClose : public CXTPTabManagerNavigateButton
{
public:
	CNavigateButtonClose(CXTPTabManager* pManager, XTPTabNavigateButtonFlags dwFlags)
		: CXTPTabManagerNavigateButton(pManager, xtpTabNavigateButtonClose, dwFlags)
	{
		XTPResourceManager()->LoadString(&m_strToolTip, XTP_IDS_TABNAVIGATEBUTTON_CLOSE);
	}
protected:
	virtual void DrawEntry(CDC* pDC, CRect rc);
};

class CXTPTabManager::CNavigateButtonTabClose : public CXTPTabManager::CNavigateButtonClose
{
public:
	CNavigateButtonTabClose(CXTPTabManagerItem* pItem, XTPTabNavigateButtonFlags dwFlags)
		: CXTPTabManager::CNavigateButtonClose(pItem->GetTabManager(), dwFlags)
	{
		m_pItem = pItem;
	}
};


void CXTPTabManager::CNavigateButtonClose::DrawEntry(CDC* pDC, CRect rc)
{
	CPoint pt = rc.CenterPoint();

	pDC->MoveTo(pt.x - 4, pt.y - 3);
	pDC->LineTo(pt.x + 3, pt.y + 4);
	pDC->MoveTo(pt.x - 3, pt.y - 3);
	pDC->LineTo(pt.x + 4, pt.y + 4);

	pDC->MoveTo(pt.x - 4, pt.y + 3);
	pDC->LineTo(pt.x + 3, pt.y - 4);
	pDC->MoveTo(pt.x - 3, pt.y + 3);
	pDC->LineTo(pt.x + 4, pt.y - 4);
}

void CXTPTabManager::CNavigateButtonArrowRight::DrawEntry(CDC* pDC, CRect rc)
{
	CPoint pt = rc.CenterPoint();

	if (m_pManager->IsHorizontalPosition())
		CXTPDrawHelpers::Triangle(pDC, CPoint(pt.x - 2, pt.y - 5), CPoint(pt.x + 2, pt.y - 1), CPoint(pt.x - 2, pt.y + 3));
	else
		CXTPDrawHelpers::Triangle(pDC, CPoint(pt.x - 5, pt.y - 2), CPoint(pt.x - 1, pt.y + 2), CPoint(pt.x + 3, pt.y - 2));
}

void CXTPTabManager::CNavigateButtonArrow::AdjustWidth(int& nWidth)
{
	if (m_dwFlags == xtpTabNavigateButtonAlways)
	{
		if ((m_pManager->GetPosition() == xtpTabPositionTop) || (m_pManager->GetPosition() == xtpTabPositionBottom))
		{
			nWidth -= GetSize().cx;
		}
		else
		{
			nWidth -= GetSize().cy;
		}
	}
}
void CXTPTabManager::CNavigateButtonArrow::Reposition(CRect& rcNavigateButtons)
{
	if (m_pManager->GetLayout() == xtpTabLayoutMultiRow)
	{
		m_rcButton.SetRectEmpty();
		return;
	}

	if (m_dwFlags == xtpTabNavigateButtonAutomatic)
	{
		if (m_pManager->GetLayout() == xtpTabLayoutSizeToFit)
		{
			m_rcButton.SetRectEmpty();
			return;
		}

		CRect rc = m_pManager->GetAppearanceSet()->GetHeaderMargin();

		if (!(m_pManager->GetItemsLength() + m_pManager->GetHeaderOffset() - rc.left - 1 > m_pManager->GetRectLength(rcNavigateButtons) - (rc.left + rc.right)
			|| m_pManager->GetHeaderOffset() < 0))
		{
			m_rcButton.SetRectEmpty();
			return;
		}
	}

	CXTPTabManagerNavigateButton::Reposition(rcNavigateButtons);

}
void CXTPTabManager::CNavigateButtonArrowRight::Reposition(CRect& rcNavigateButtons)
{
	CRect rcHeaderMargin = m_pManager->GetPaintManager()->GetAppearanceSet()->GetHeaderMargin();

	m_bEnabled = m_pManager->GetItemsLength() + m_pManager->GetHeaderOffset() >
		m_pManager->GetRectLength(rcNavigateButtons) - (rcHeaderMargin.left + rcHeaderMargin.right) - 28;

	CNavigateButtonArrow::Reposition(rcNavigateButtons);
}

void CXTPTabManager::CNavigateButtonArrowRight::OnExecute(BOOL bTick)
{
	if (bTick)
	{
		m_pManager->OnScrollHeader(TRUE);
	}
}

void CXTPTabManager::OnScrollHeader(BOOL bRight)
{
	if (bRight)
	{
		SetHeaderOffset(GetHeaderOffset() - m_nScrollDelta);
	}
	else
	{
		SetHeaderOffset(GetHeaderOffset() + m_nScrollDelta);
	}
}

void CXTPTabManager::CNavigateButtonArrowLeft::DrawEntry(CDC* pDC, CRect rc)
{
	CPoint pt = rc.CenterPoint();

	if (m_pManager->IsHorizontalPosition())
		CXTPDrawHelpers::Triangle(pDC, CPoint(pt.x + 2, pt.y - 5), CPoint(pt.x - 2, pt.y - 1), CPoint(pt.x + 2, pt.y + 3));
	else
		CXTPDrawHelpers::Triangle(pDC, CPoint(pt.x - 5, pt.y + 2), CPoint(pt.x - 1, pt.y - 2), CPoint(pt.x + 3, pt.y + 2));
}
void CXTPTabManager::CNavigateButtonArrowLeft::Reposition(CRect& rcNavigateButtons)
{
	m_bEnabled = m_pManager->GetHeaderOffset() < 0;

	CNavigateButtonArrow::Reposition(rcNavigateButtons);
}

void CXTPTabManager::CNavigateButtonArrowLeft::OnExecute(BOOL bTick)
{
	if (bTick)
	{
		m_pManager->OnScrollHeader(FALSE);
	}
}



//////////////////////////////////////////////////////////////////////
// CXTPTabManager

CXTPTabManager::CXTPTabManager()
{

	m_pSelected = 0;
	m_pHighlighted = 0;
	m_pPressed = 0;

	m_nHeaderOffset = 0;

	m_bAllowReorder = TRUE;

	m_bActive = TRUE;
	m_nScrollDelta = 20;

	m_rcHeaderRect.SetRectEmpty();
	m_rcControl.SetRectEmpty();
	m_rcClient.SetRectEmpty();

	m_bCloseItemButton = xtpTabNavigateButtonNone;

	m_pHighlightedNavigateButton = NULL;

	m_pRowIndexer = new CRowIndexer(this);

	m_arrNavigateButtons.Add(new CNavigateButtonArrowLeft(this, xtpTabNavigateButtonAutomatic));
	m_arrNavigateButtons.Add(new CNavigateButtonArrowRight(this, xtpTabNavigateButtonAutomatic));
	m_arrNavigateButtons.Add(new CNavigateButtonClose(this, xtpTabNavigateButtonNone));
}

CXTPTabManager::~CXTPTabManager()
{
	for (int i = 0; i < m_arrItems.GetSize(); i++)
	{
		CXTPTabManagerItem* pItem = m_arrItems[i];
		pItem->OnRemoved();
		pItem->InternalRelease();
	}

	for (int j = 0; j < m_arrNavigateButtons.GetSize(); j++)
		delete m_arrNavigateButtons[j];

	delete m_pRowIndexer;
}

void CXTPTabManager::SetActive(BOOL bActive)
{
	if (m_bActive != bActive)
	{
		m_bActive = bActive;
		Reposition();
	}
}

BOOL CXTPTabManager::IsNavigateButtonAutomaticVisible(CXTPTabManagerNavigateButton* pButton)
{
	if (pButton->GetID() == xtpTabNavigateButtonClose)
	{
		return m_pSelected ? m_pSelected->IsClosable() : FALSE;
	}

	return TRUE;
}

void CXTPTabManager::EnableTabThemeTexture(HWND hWnd, BOOL bEnable)
{
	m_themeTabControl.OpenTheme(hWnd, L"TAB");
	m_themeTabControl.EnableThemeDialogTexture(hWnd, bEnable ? ETDT_ENABLETAB : ETDT_DISABLE | ETDT_USETABTEXTURE);

}

CXTPTabManagerNavigateButton* CXTPTabManager::FindNavigateButton(UINT nID) const
{
	for (int i = 0; i < m_arrNavigateButtons.GetSize(); i++)
	{
		if (m_arrNavigateButtons[i]->m_nID == nID)
			return m_arrNavigateButtons[i];
	}

	return NULL;
}

void CXTPTabManager::SetHeaderOffset(int nOffset)
{
	if (nOffset > 0) nOffset = 0;

	if (nOffset != m_nHeaderOffset)
	{
		m_nHeaderOffset = nOffset;
		Reposition();
	}
}

int CXTPTabManager::GetItemsLength() const
{
	int nLength = 0;

	if (GetLayout() == xtpTabLayoutRotated)
	{
		nLength = GetPaintManager()->GetAppearanceSet()->GetButtonHeight(this)
			* GetItemCount();
	}
	else
	{
		for (int i = 0; i < GetItemCount(); i++)
			nLength += GetItem(i)->GetButtonLength();
	}

	return nLength;
}


void CXTPTabManager::DeleteAllItems()
{
	for (int i = 0; i < m_arrItems.GetSize(); i++)
	{
		CXTPTabManagerItem* pItem = m_arrItems[i];
		pItem->OnRemoved();
		pItem->InternalRelease();
	}

	m_arrItems.RemoveAll();

	m_pHighlighted = m_pSelected = m_pPressed = NULL;

	OnItemsChanged();
}

BOOL CXTPTabManager::DeleteItem(int nItem)
{
	if (nItem < 0 || nItem >= GetItemCount())
		return FALSE;

	CXTPTabManagerItem* pItem = m_arrItems[nItem];

	BOOL bSelected = (m_pSelected == pItem);

	if (m_pHighlighted == pItem)
		m_pHighlighted = NULL;

	m_arrItems.RemoveAt(nItem);

	pItem->OnRemoved();
	pItem->InternalRelease();

	if (bSelected)
	{
		SetCurSel(nItem);
	}


	OnItemsChanged();

	return TRUE;
}

CXTPTabManagerItem* CXTPTabManager::AddItem(int nItem, CXTPTabManagerItem* pItem /*= NULL*/)
{
	if (!pItem)
		pItem = new CXTPTabManagerItem();

	pItem->m_pTabManager = this;

	if (nItem < 0 || nItem > GetItemCount())
		nItem = GetItemCount();

	m_arrItems.InsertAt(nItem, pItem);
	pItem->m_clrItem = xtpTabColorBlue + (GetPaintManager()->m_nItemColor++ % 8);

	if (m_bCloseItemButton != xtpTabNavigateButtonNone)
	{
		pItem->m_arrNavigateButtons.Add(new CNavigateButtonTabClose(pItem, m_bCloseItemButton));
	}

	OnItemsChanged();

	return pItem;
}

void CXTPTabManager::OnItemsChanged()
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		GetItem(i)->m_nIndex = i;
	}
	Reposition();
}

CString CXTPTabManager::GetItemCaption(const CXTPTabManagerItem* pItem) const
{
	return pItem->m_strCaption;
}

HICON CXTPTabManager::GetItemIcon(const CXTPTabManagerItem* pItem) const
{
	return pItem->m_hIcon;
}

void CXTPTabManager::ShowIcons(BOOL bShowIcons)
{
	GetPaintManager()->m_bShowIcons = bShowIcons;
	Reposition();
}

COLORREF CXTPTabManager::GetItemColor(const CXTPTabManagerItem* pItem) const
{
	COLORREF nColor = pItem->m_clrItem;

	if (nColor >= xtpTabColorBlue && nColor <= xtpTabColorMagenta)
		return CXTPTabPaintManager::GetOneNoteColor((XTPTabOneNoteColor)nColor);

	return nColor;
}

void CXTPTabManager::SetCurSel(int nIndex)
{
	if (GetItemCount() != 0)
	{
		nIndex = nIndex < 0 ? 0: nIndex >= GetItemCount() ? GetItemCount() - 1 : nIndex;

		SetSelectedItem(GetItem(nIndex));
	}
	else
	{
		SetSelectedItem(NULL);
	}
}

int CXTPTabManager::GetCurSel() const
{
	if (m_pSelected)
	{
		return m_pSelected->GetIndex();
	}
	return -1;
}

void CXTPTabManager::SetSelectedItem(CXTPTabManagerItem* pItem)
{
	if (m_pSelected != pItem)
	{
		m_pSelected = pItem;
		Reposition();
		EnsureVisible(pItem);
	}
}

void CXTPTabManager::EnsureVisible(CXTPTabManagerItem* pItem)
{
	if (!pItem)
		return;

	GetPaintManager()->EnsureVisible(this, pItem);
}

void CXTPTabManager::SetPosition(XTPTabPosition tabPosition)
{
	GetPaintManager()->m_tabPosition = tabPosition;
	Reposition();
}

void CXTPTabManager::SetLayoutStyle(XTPTabLayoutStyle tabLayout)
{
	GetPaintManager()->m_tabLayout = tabLayout;
	Reposition();
}


CXTPTabManagerItem* CXTPTabManager::HitTest(CPoint point) const
{
	if (!m_rcControl.PtInRect(point))
		return NULL;

	if (!m_rcHeaderRect.IsRectEmpty() && !m_rcHeaderRect.PtInRect(point))
		return NULL;

	for (int i = 0; i < GetItemCount(); i++)
	{
		CXTPTabManagerItem* pItem = GetItem(i);

		if (pItem->GetRect().PtInRect(point) && pItem->IsEnabled() && pItem->IsVisible())
		{
			return pItem;
		}
	}
	return NULL;
}

XTPTabPosition CXTPTabManager::GetPosition() const
{
	return GetPaintManager()->m_tabPosition;
}

XTPTabLayoutStyle CXTPTabManager::GetLayout() const
{
	return GetPaintManager()->m_tabLayout;
}

void CXTPTabManager::MoveItem(CXTPTabManagerItem* pItem, int nIndex)
{
	ASSERT(pItem && pItem->GetTabManager() == this);

	if (!pItem || pItem->GetTabManager() != this)
		return;

	int nOldIndex = pItem->GetIndex();
	if (nOldIndex == nIndex)
		return;

	ASSERT(nOldIndex >= 0);
	ASSERT(nIndex >= 0 && nIndex < GetItemCount());

	if (nIndex < 0 || nIndex >= GetItemCount())
		nIndex = GetItemCount() - 1;

	m_arrItems.RemoveAt(nOldIndex);
	m_arrItems.InsertAt(nIndex, pItem);

	OnItemsChanged();
}

void CXTPTabManager::TrackClick(HWND hWnd, CPoint pt, CXTPTabManagerItem* pItem)
{
	if (GetPaintManager()->m_bHotTracking)
		m_pHighlighted = pItem;

	m_pPressed = pItem;
	BOOL bHighlighted = TRUE;

	Reposition();
	BOOL bAccept = FALSE;

	::SetCapture(hWnd);

	while (::GetCapture() == hWnd)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (::GetCapture() != hWnd)
		{
			DispatchMessage (&msg);
			break;
		}
		switch (msg.message)
		{
			case WM_MOUSEMOVE:
				{
					pt = CPoint((short signed)LOWORD(msg.lParam), (short signed)HIWORD(msg.lParam));

					bHighlighted = pItem->GetRect().PtInRect(pt);

					if (GetPaintManager()->m_bHotTracking)
					{
						CXTPTabManagerItem* pHighlighted = bHighlighted ? pItem : NULL;
						if (pHighlighted != m_pHighlighted)
						{
							m_pHighlighted = pHighlighted;
							RedrawControl(pItem->GetRect(), TRUE);
						}
					}

				}

				break;

			case WM_KEYDOWN:
				if (msg.wParam != VK_ESCAPE)
					break;
			case WM_CANCELMODE:
			case WM_RBUTTONDOWN:
				goto ExitLoop;

			case WM_LBUTTONUP:
				bAccept = TRUE;
				goto ExitLoop;

			default:
				DispatchMessage (&msg);
				break;
		}
	}

ExitLoop:
	ReleaseCapture();

	m_pPressed = FALSE;

	PerformMouseMove(hWnd, pt);

	RedrawControl(NULL, FALSE);

	if (bAccept && bHighlighted)
	{
		OnItemClick(pItem);
	}
}

void CXTPTabManager::ReOrder(HWND hWnd, CPoint pt, CXTPTabManagerItem* pItem)
{
	CXTPTabManagerItem* pSelected = m_pSelected;
	m_pSelected = pItem;

	if (GetPaintManager()->m_bHotTracking)
		m_pHighlighted = pItem;

	Reposition();

	CArray<CRect, CRect&> arrRects;

	for (int j = 0; j < GetItemCount(); j++)
	{
		CRect rc = GetItem(j)->GetRect();
		arrRects.Add(rc);
	}

	::SetCapture(hWnd);

	while (::GetCapture() == hWnd)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (::GetCapture() != hWnd)
		{
			DispatchMessage (&msg);
			break;
		}
		switch (msg.message)
		{
			case WM_MOUSEMOVE:
				{
					pt = CPoint((short signed)LOWORD(msg.lParam), (short signed)HIWORD(msg.lParam));

					for (int i = 0; i < arrRects.GetSize(); i++)
					{
						if (i != pItem->GetIndex() && arrRects[i].PtInRect(pt))
						{

							CXTPTabManagerItem* p = pItem;
							m_arrItems[pItem->GetIndex()] = m_arrItems[i];
							m_arrItems[i] = p;

							OnItemsChanged();
							break;
						}
					}
				}

				break;

			case WM_KEYDOWN:
				if (msg.wParam != VK_ESCAPE)
					break;
			case WM_CANCELMODE:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
				goto ExitLoop;

			default:
				DispatchMessage (&msg);
				break;
		}
	}

ExitLoop:
	ReleaseCapture();
	PerformMouseMove(hWnd, pt);

	m_pSelected = pSelected;
	OnItemClick(pItem);
}

void CXTPTabManager::OnItemClick(CXTPTabManagerItem* pItem)
{
	SetSelectedItem(pItem);
}

void CXTPTabManager::SetFocusedItem(CXTPTabManagerItem* pItem)
{
	if (!OnBeforeItemClick(pItem))
		return;

	if (pItem)
	{
		OnItemClick(pItem);
	}
}

CXTPTabManagerItem* CXTPTabManager::FindNextFocusable(int nIndex, int nDirection) const
{
	CXTPTabManagerItem* pItem = NULL;

	do
	{
		nIndex += nDirection;

		pItem = GetItem(nIndex);
		if (!pItem)
			return NULL;

	}
	while (!(pItem->IsVisible() && pItem->IsEnabled()));

	return pItem;
}

BOOL CXTPTabManager::PerformKeyDown(HWND hWnd, UINT nChar)
{
	const int nCount = GetItemCount();
	if (nCount < 1)
		return FALSE;

	if (nChar == VK_LEFT && (DWORD)GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
		nChar = VK_RIGHT;
	else if (nChar == VK_RIGHT && (DWORD)GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
		nChar = VK_LEFT;

	switch (nChar)
	{
	case VK_HOME:
		SetFocusedItem(FindNextFocusable(-1, +1));
		return TRUE;

	case VK_END:
		SetFocusedItem(FindNextFocusable(nCount, -1));
		return TRUE;

	case VK_LEFT:
		if (IsHorizontalPosition() && m_pSelected && m_pSelected->GetIndex() > 0)
			SetFocusedItem(FindNextFocusable(m_pSelected->GetIndex(), -1));
		return TRUE;

	case VK_UP:
		if (!IsHorizontalPosition() && m_pSelected && m_pSelected->GetIndex() > 0)
			SetFocusedItem(FindNextFocusable(m_pSelected->GetIndex(), -1));
		return TRUE;

	case VK_RIGHT:
		if (IsHorizontalPosition() && m_pSelected && m_pSelected->GetIndex() < nCount - 1)
			SetFocusedItem(FindNextFocusable(m_pSelected->GetIndex(), + 1));
		return TRUE;

	case VK_DOWN:
		if (!IsHorizontalPosition() && m_pSelected && m_pSelected->GetIndex() < nCount - 1)
			SetFocusedItem(FindNextFocusable(m_pSelected->GetIndex(), + 1));
		return TRUE;
	}
	return FALSE;
}

BOOL CXTPTabManager::OnBeforeItemClick(CXTPTabManagerItem* /*pItem*/)
{
	return TRUE;
}


CXTPTabManagerNavigateButton* CXTPTabManager::HitTestNavigateButton(CPoint point, BOOL bHeaderOnly, int* pnIndex) const
{
	int i;
	for (i = 0; i < (int)m_arrNavigateButtons.GetSize(); i++)
	{
		CXTPTabManagerNavigateButton* pButton = m_arrNavigateButtons[i];
		if (pButton->m_rcButton.PtInRect(point))
		{
			if (!pButton->IsEnabled())
				return NULL;

			if (pnIndex)
			{
				*pnIndex = i;
			}
			return pButton;
		}
	}

	if (bHeaderOnly)
		return NULL;

	CXTPTabManagerItem* pItem = HitTest(point);
	if (!pItem)
		return NULL;

	for (i = 0; i < (int)pItem->GetNavigateButtons()->GetSize(); i++)
	{
		CXTPTabManagerNavigateButton* pButton = pItem->GetNavigateButtons()->GetAt(i);
		if (pButton->m_rcButton.PtInRect(point))
		{
			if (!pButton->IsEnabled())
				return NULL;

			if (pnIndex)
			{
				*pnIndex = i;
			}
			return pButton;
		}
	}

	return NULL;
}

BOOL CXTPTabManager::PerformClick(HWND hWnd, CPoint pt, BOOL bNoItemClick)
{
	CXTPTabManagerNavigateButton* pNavigateButton = HitTestNavigateButton(pt, FALSE);
	if (pNavigateButton)
	{
		pNavigateButton->PerformClick(hWnd, pt);
		return TRUE;
	}

	if (bNoItemClick)
		return FALSE;

	CXTPTabManagerItem* pItem = HitTest(pt);

	if (pItem)
	{
		if (!OnBeforeItemClick(pItem))
			return FALSE;

		if (IsAllowReorder())
		{
			ReOrder(hWnd, pt, pItem);
		}
		else if (GetPaintManager()->m_bSelectOnButtonDown)
		{
			OnItemClick(pItem);
		}
		else
		{
			TrackClick(hWnd, pt, pItem);
		}
		return TRUE;
	}

	return FALSE;
}


void CXTPTabManager::PerformMouseMove(HWND hWnd, CPoint pt)
{
	 CXTPTabPaintManager::CAppearanceSet* pAppearance = GetPaintManager()->GetAppearanceSet();

	 if (!CXTPDrawHelpers::IsTopParentActive(hWnd) || IsMouseLocked())
	{
		if (m_pHighlighted)
		{
			CRect rcRedraw(pAppearance->GetButtonDrawRect(m_pHighlighted));
			m_pHighlighted = NULL;

			RedrawControl(rcRedraw, TRUE);
		}
		return;
	}

	if (GetPaintManager()->m_bHotTracking)
	{
		CXTPTabManagerItem* pItem = HitTest(pt);

		if (pItem != m_pHighlighted)
		{
			if (m_pHighlighted)
			{
				CRect rcRedraw(pAppearance->GetButtonDrawRect(m_pHighlighted));
				m_pHighlighted = NULL;

				RedrawControl(rcRedraw, TRUE);
			}

			m_pHighlighted = pItem;

			if (m_pHighlighted)
			{
				RedrawControl(pAppearance->GetButtonDrawRect(m_pHighlighted), FALSE);
			}

			if (pItem)
			{
				TRACKMOUSEEVENT tme =
				{
					sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd
				};
				_TrackMouseEvent(&tme);
			}
		}
	}

	CXTPTabManagerNavigateButton* pNavigateButton = HitTestNavigateButton(pt, FALSE);

	if (pNavigateButton != m_pHighlightedNavigateButton)
	{
		if (m_pHighlightedNavigateButton)
		{
			RedrawControl(m_pHighlightedNavigateButton->GetRect(), TRUE);
		}

		m_pHighlightedNavigateButton = pNavigateButton;

		if (m_pHighlightedNavigateButton)
		{
			RedrawControl(m_pHighlightedNavigateButton->GetRect(), FALSE);
		}

		if (pNavigateButton)
		{
			TRACKMOUSEEVENT tme =
			{
				sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd
			};
			_TrackMouseEvent(&tme);
		}
	}
}

CString CXTPTabManager::GetItemTooltip(const CXTPTabManagerItem* pItem) const
{
	return pItem->m_strToolTip;
}

INT_PTR CXTPTabManager::PerformToolHitTest(HWND hWnd, CPoint point, TOOLINFO* pTI) const
{
	if (IsMouseLocked())
		return -1;

	int nIndex = -1;
	CXTPTabManagerNavigateButton* pNavigateButton = HitTestNavigateButton(point, FALSE, &nIndex);
	if (pNavigateButton)
	{
		ASSERT(nIndex != -1);
		CString strTip = pNavigateButton->GetTooltip();
		if (strTip.IsEmpty())
			return -1;

		CXTPToolTipContext::FillInToolInfo(pTI, hWnd, pNavigateButton->GetRect(), nIndex, strTip);

		return nIndex;
	}

	CXTPTabManagerItem* pItem = HitTest(point);

	if (pItem)
	{
		if (GetPaintManager()->m_toolBehaviour == xtpTabToolTipNever)
			return -1;

		if (GetPaintManager()->m_toolBehaviour == xtpTabToolTipShrinkedOnly && !pItem->IsItemShrinked())
			return -1;

		CString strTip = GetItemTooltip(pItem);
		if (strTip.IsEmpty())
			return -1;

		CXTPToolTipContext::FillInToolInfo(pTI, hWnd, pItem->GetRect(), pItem->GetIndex(), strTip, pItem->GetCaption(), strTip);

		return pItem->GetIndex();
	}
	return -1;
}

void CXTPTabManager::GetItemMetrics(CSize* lpszNormal, CSize* lpszMin /*= NULL*/, CSize* lpszMax /*= NULL*/) const
{
	CXTPTabPaintManager* pPaintManager = GetPaintManager();
	if (lpszNormal) *lpszNormal = CSize(pPaintManager->m_nFixedTabWidth, 0);
	if (lpszMin) *lpszMin = CSize(pPaintManager->m_nMinTabWidth, 0);
	if (lpszMax) *lpszMax = CSize(pPaintManager->m_nMaxTabWidth, 0);
}

void CXTPTabManager::SetItemMetrics(CSize szNormal, CSize szMin /*= CSize(0, 0)*/, CSize szMax /*= CSize(0, 0)*/)
{
	CXTPTabPaintManager* pPaintManager = GetPaintManager();
	pPaintManager->m_nFixedTabWidth = szNormal.cx;
	pPaintManager->m_nMinTabWidth = szMin.cx;
	pPaintManager->m_nMaxTabWidth = szMax.cx;

	Reposition();
}

BOOL CXTPTabManager::IsDrawStaticFrame() const
{
	return GetPaintManager()->m_bStaticFrame;
}


CXTPTabPaintManager::CColorSet* CXTPTabManager::SetColor(XTPTabColorStyle tabColor)
{
	CXTPTabPaintManager::CColorSet* pColorSet = GetPaintManager()->SetColor(tabColor);
	Reposition();
	return pColorSet;
}

CXTPTabPaintManager::CColorSet* CXTPTabManager::SetColorSet(CXTPTabPaintManager::CColorSet* pColorSet)
{
	GetPaintManager()->SetColorSet(pColorSet);
	Reposition();
	return pColorSet;
}
CXTPTabPaintManager::CAppearanceSet* CXTPTabManager::SetAppearance(XTPTabAppearanceStyle tabAppearance)
{
	CXTPTabPaintManager::CAppearanceSet* pAppearanceSet = GetPaintManager()->SetAppearance(tabAppearance);
	Reposition();
	return pAppearanceSet;
}
CXTPTabPaintManager::CAppearanceSet* CXTPTabManager::SetAppearanceSet(CXTPTabPaintManager::CAppearanceSet* pAppearanceSet)
{
	GetPaintManager()->SetAppearanceSet(pAppearanceSet);
	Reposition();
	return pAppearanceSet;
}

