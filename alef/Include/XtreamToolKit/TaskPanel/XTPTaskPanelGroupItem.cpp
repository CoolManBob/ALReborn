// XTPTaskPanelGroupItem.cpp : implementation of the CXTPTaskPanelGroupItem class.
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
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPPropExchange.h"

#include "XTPTaskPanelGroupItem.h"
#include "XTPTaskPanelItems.h"
#include "XTPTaskPanelPaintManager.h"
#include "XTPTaskPanel.h"
#include "XTPTaskPanelGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPTaskPanelGroupItem

IMPLEMENT_SERIAL(CXTPTaskPanelGroupItem, CXTPTaskPanelItem, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

CXTPTaskPanelGroupItem::CXTPTaskPanelGroupItem()
{
	m_typeItem = xtpTaskItemTypeText;
	m_rcMargins = CXTPTaskPanelPaintManager::rectDefault;

	m_bBold = FALSE;
	m_hWnd = 0;
	m_szItem = CSize(0, 0);
	m_bSelected = FALSE;
	m_bAutoHeight = FALSE;

	m_rcItem.SetRectEmpty();
	m_szPreview = CSize(0, 0);

	m_clrText = COLORREF_NULL;

	EnableAutomation();
}

CXTPTaskPanelGroupItem::~CXTPTaskPanelGroupItem()
{
	if (m_typeItem == xtpTaskItemTypeControl && m_hWnd && ::IsWindow(m_hWnd))
	{
		::ShowWindow(m_hWnd, SW_HIDE);
	}
}

CRect CXTPTaskPanelGroupItem::OnReposition(CRect rc)
{
	if (GetItemGroup()->GetItemLayout() == xtpTaskItemLayoutImages)
	{
		return m_rcItem = rc;
	}

	CXTPTaskPanelPaintManager* pPaintManager = GetPaintManager();
	CRect rcMargins = pPaintManager->GetItemOuterMargins(this);

	rc.DeflateRect(rcMargins);

	if ((m_szItem == CSize(0)) && (m_typeItem != xtpTaskItemTypeControl))
	{
		CClientDC dc((CWnd*)m_pPanel);
		m_rcItem = GetPaintManager()->DrawGroupItem(&dc, this, rc, FALSE);
	}
	else
	{
		m_rcItem.SetRect(rc.left, rc.top, rc.left + m_szItem.cx, rc.top + m_szItem.cy);
	}

	return m_rcItem;
}

void CXTPTaskPanelGroupItem::OnDrawItem(CDC* pDC, CRect rc)
{
	if ((m_typeItem != xtpTaskItemTypeControl) || (GetItemGroup()->GetItemLayout() == xtpTaskItemLayoutImages))
		GetPaintManager()->DrawGroupItem(pDC, this, rc, TRUE);
	else
	{
		if (!IsWindowVisible(m_hWnd) && m_bmpPreview.GetSafeHandle())
		{
			pDC->DrawState(rc.TopLeft(), rc.Size(), &m_bmpPreview, DST_BITMAP);
		}
	}
}

CXTPTaskPanelGroup* CXTPTaskPanelGroupItem::GetItemGroup() const
{
	return m_pItems ? (CXTPTaskPanelGroup*) m_pItems->GetOwner() : NULL;
}

void CXTPTaskPanelGroupItem::SetBold(BOOL bBold)
{
	m_bBold = bBold;
	RepositionPanel();
}
BOOL CXTPTaskPanelGroupItem::IsBold() const
{
	return m_bBold;
}

void CXTPTaskPanelGroupItem::SetControlHandle(HWND hWnd)
{
	m_hWnd = hWnd;

	CRect rc;
	::GetWindowRect(hWnd, rc);
	m_szItem.cy = rc.Height();

	SetType(xtpTaskItemTypeControl);
}

HWND CXTPTaskPanelGroupItem::GetControlHandle() const
{
	return m_hWnd;
}


void CXTPTaskPanelGroupItem::GetPreviewBitmap(CWnd* pWnd, CBitmap& bmp)
{
	bmp.DeleteObject();
	if (!pWnd || !::IsWindow(pWnd->m_hWnd))
		return;


	{
		CXTPWindowRect rectWindow(pWnd);
		CClientDC dc(pWnd);
		CBitmap bitmapNonClient;

		VERIFY(bmp.CreateCompatibleBitmap(&dc, rectWindow.Width(), rectWindow.Height()));
		VERIFY(bitmapNonClient.CreateCompatibleBitmap(&dc, rectWindow.Width(), rectWindow.Height()));

		CXTPCompatibleDC dcMemDestination(&dc, &bmp);
		CXTPCompatibleDC dcMemSourceNonClient(&dc, &bitmapNonClient);

		dcMemSourceNonClient.FillSolidRect(0, 0, rectWindow.Width(), rectWindow.Height(), GetBackColor());
		pWnd->Print(&dcMemSourceNonClient, PRF_NONCLIENT);

		dcMemDestination.BitBlt(0, 0, rectWindow.Width(), rectWindow.Height(), &dcMemSourceNonClient, 0, 0, SRCCOPY);

		CPoint pLT(0, 0);
		pWnd->ClientToScreen(&pLT);
		dcMemDestination.SetViewportOrg(pLT.x - rectWindow.left, pLT.y - rectWindow.top);

		pWnd->Print(&dcMemDestination, PRF_CHILDREN | PRF_CLIENT | PRF_ERASEBKGND);
	}

}


void CXTPTaskPanelGroupItem::OnAnimate(int /*nStep*/)
{
	if (!(m_typeItem == xtpTaskItemTypeControl && m_hWnd && ::IsWindow(m_hWnd)))
		return;

	CXTPTaskPanelGroup* pGroup = GetItemGroup();
	if (!pGroup) return;

	CRect rc = pGroup->GetClientRect();
	rc.OffsetRect(m_rcItem.left - rc.left, m_rcItem.top - m_pPanel->GetScrollOffset() - pGroup->GetScrollOffsetPos());

	CRect rcMargins = GetPaintManager()->GetItemOuterMargins(this);
	CRect rcGroupMargin = GetPaintManager()->GetGroupInnerMargins(pGroup);

	int nWidth = rc.Width() - rcGroupMargin.left - rcGroupMargin.right -
		rcMargins.right - rcMargins.left;

	if (m_szItem.cx != 0) nWidth = min(m_szItem.cx, nWidth);


	if (!IsVisible() || !pGroup->IsVisible() || (pGroup->GetItemLayout() == xtpTaskItemLayoutImages))
	{
		m_bmpPreview.DeleteObject();
		::ShowWindow(m_hWnd, SW_HIDE);
		return;
	}

	if (pGroup->IsExpanding() || !pGroup->IsExpanded()
		|| (pGroup->GetClientRect().Height() != pGroup->GetTargetClientRect().Height())
		)
	{

		if (IsWindowVisible(m_hWnd) || m_bmpPreview.GetSafeHandle() == 0 || m_szPreview != m_rcItem.Size())
		{
			m_bmpPreview.DeleteObject();

			if (m_rcItem.Height() > 0)
			{
				SetWindowPos(m_hWnd, 0, rc.left, -m_rcItem.Height(), nWidth, m_rcItem.Height(), SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);

				GetPreviewBitmap(CWnd::FromHandle(m_hWnd), m_bmpPreview);
				m_szPreview = m_rcItem.Size();
			}

			::ShowWindow(m_hWnd, SW_HIDE);
		}
	}
	else
	{
		BOOL bVisible = pGroup->IsItemVisible(this, FALSE);

		if (!bVisible)
		{
			::ShowWindow(m_hWnd, SW_HIDE);
		}
		else
		{
			SetWindowPos(m_hWnd, 0, rc.left, rc.top, nWidth, m_rcItem.Height(), SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
		}

		m_bmpPreview.DeleteObject();
	}
}

void CXTPTaskPanelGroupItem::SetSize(CSize szItem, BOOL bAutoHeight)
{
	m_bAutoHeight = bAutoHeight;
	m_szItem = szItem;
	RepositionPanel();
}

COLORREF CXTPTaskPanelGroupItem::GetBackColor() const
{
	XTP_TASKPANEL_GROUPCOLORS* pColors = GetPaintManager()->GetGroupColors(GetItemGroup()->IsSpecialGroup());
	return pColors->clrClient;
}



BOOL CXTPTaskPanelGroupItem::IsItemSelected() const
{
	if (m_pPanel && m_pPanel->IsSingleSelection())
		return m_pPanel->GetFocusedItem() == (CXTPTaskPanelItem*)this;

	return m_bSelected;
}

void CXTPTaskPanelGroupItem::SetItemSelected(BOOL bSelected)
{
	if (m_pPanel && m_pPanel->IsSingleSelection())
	{
		if (bSelected) m_pPanel->SetFocusedItem(this);
		return;
	}

	if (bSelected != m_bSelected)
	{
		m_bSelected = bSelected;
		RedrawPanel();
	}
}

CRect CXTPTaskPanelGroupItem::GetHitTestRect() const
{
	if (!IsVisible())
		return CRect(0, 0, 0, 0);

	CXTPTaskPanelGroup* pGroup = GetItemGroup();
	CRect rcCaption = pGroup->GetCaptionRect();

	CRect rcItem = GetItemRect();
	int nOffset = m_pPanel->GetScrollOffset() - rcCaption.bottom + pGroup->GetScrollOffsetPos();

	rcItem.OffsetRect(0, -nOffset);
	return rcItem;
}

void CXTPTaskPanelGroupItem::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPTaskPanelItem::DoPropExchange(pPX);

	BOOL bSelected = IsItemSelected();

	PX_Rect(pPX, _T("Margins"), m_rcMargins, CXTPTaskPanelPaintManager::rectDefault);
	PX_Bool(pPX, _T("Bold"), m_bBold, FALSE);
	PX_Size(pPX, _T("ItemSize"), m_szItem, CSize(0, 0));
	PX_Bool(pPX, _T("Selected"), bSelected, FALSE);

	if (pPX->GetSchema() > _XTP_SCHEMA_95)
	{
		PX_Bool(pPX, _T("AutoHeight"), m_bAutoHeight, FALSE);
	}
	if (pPX->GetSchema() > _XTP_SCHEMA_97)
	{
		PX_String(pPX, _T("DragText"), m_strDragText, _T(""));
	}
	if (pPX->GetSchema() > _XTP_SCHEMA_100)
	{
		PX_DWord(pPX, _T("TextColor"), m_clrText, COLORREF_NULL);
	}


	if (pPX->IsLoading())
	{
		SetItemSelected(bSelected);
	}
}

CString CXTPTaskPanelGroupItem::GetDragText() const
{
	return m_strDragText;
}

void CXTPTaskPanelGroupItem::SetDragText(LPCTSTR lpszDragText)
{
	m_strDragText = lpszDragText;
}


BOOL CXTPTaskPanelGroupItem::PrepareDrag (COleDataSource& srcItem, BOOL bCacheTextData)
{
	HGLOBAL hGlobal = CacheGlobalData();

	if (!hGlobal)
		return FALSE;

	srcItem.CacheGlobalData (CXTPTaskPanel::GetClipboardFormat(), hGlobal);

	if (!bCacheTextData)
		return TRUE;

	CString strText = GetDragText();
	if (!strText.IsEmpty())
	{
		HGLOBAL hGlobalBuff = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, strText.GetLength() + 1);

		CHAR* szBuffer = (CHAR*)::GlobalLock(hGlobalBuff);
		WCSTOMBS_S(szBuffer, strText, strText.GetLength() + 1);
		::GlobalUnlock(hGlobalBuff);

		srcItem.CacheGlobalData (CF_TEXT, hGlobalBuff);
	}
	return TRUE;
}

HRESULT CXTPTaskPanelGroupItem::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	CXTPTaskPanelItem::GetAccessibleState(varChild, pvarState);

	pvarState->lVal |= STATE_SYSTEM_SELECTABLE;

	if (!GetItemGroup()->IsItemVisible(this, TRUE))
		pvarState->lVal |= STATE_SYSTEM_INVISIBLE;


	if (IsItemSelected())
		pvarState->lVal |= STATE_SYSTEM_SELECTED;

	return S_OK;
}

HRESULT CXTPTaskPanelGroupItem::AccessibleSelect(long flagsSelect, VARIANT varChild)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	if (flagsSelect == SELFLAG_TAKEFOCUS)
	{
		GetTaskPanel()->SetFocusedItem(this);
	}

	if (flagsSelect == SELFLAG_TAKESELECTION)
	{
		SetItemSelected(TRUE);
	}

	return S_OK;
}

HRESULT CXTPTaskPanelGroupItem::GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	*pszDefaultAction = SysAllocString(L"Click");

	return S_OK;
}

HRESULT CXTPTaskPanelGroupItem::AccessibleDoDefaultAction(VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	if (IsAcceptFocus())
	{
		GetTaskPanel()->SetFocusedItem(this);
	}

	GetTaskPanel()->OnClick(this);

	return S_OK;
}



