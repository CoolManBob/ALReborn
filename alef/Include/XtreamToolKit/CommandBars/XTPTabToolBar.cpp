// XTPTabToolBar.cpp : implementation of the CXTPTabToolBar class.
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

#include "Common/XTPImageManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPControl.h"
#include "XTPControlPopup.h"
#include "XTPControls.h"
#include "XTPCommandBars.h"
#include "XTPPaintManager.h"
#include "XTPTabToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPTabToolBar

IMPLEMENT_XTP_COMMANDBAR(CXTPTabToolBar, CXTPToolBar)

class CXTPTabToolBarControls : public CXTPControls
{
	DECLARE_DYNCREATE(CXTPTabToolBarControls)
protected:
	void OnControlAdded(CXTPControl* pControl)
	{
		CXTPTabManagerItem* pItem = m_pParent ? ((CXTPTabToolBar*)m_pParent)->GetSelectedItem() : NULL;

		if (pItem)
		{
			pControl->SetHideFlag(xtpHideRibbonTab, pControl->GetCategory() != pItem->GetCaption());

		}

		CXTPControls::OnControlAdded(pControl);
	}
};
IMPLEMENT_DYNCREATE(CXTPTabToolBarControls, CXTPControls)

CXTPTabToolBar::CXTPTabToolBar()
{
	SetControls(new CXTPTabToolBarControls());

	m_pTabPaintManager = new CXTPTabPaintManager();
	m_pTabPaintManager->SetAppearance(xtpTabAppearancePropertyPage2003);
	m_pTabPaintManager->m_bFillBackground = FALSE;
	m_pTabPaintManager->m_bHotTracking = TRUE;
	m_pTabPaintManager->m_clientFrame = xtpTabFrameSingleLine;
	m_bAllowReorder = FALSE;

	m_nMinWidth = 100;


	m_rcTabControl.SetRectEmpty();

	EnableAutomation();

}

CXTPTabToolBar::~CXTPTabToolBar()
{
	CMDTARGET_RELEASE(m_pTabPaintManager);

}

CXTPTabManagerItem* CXTPTabToolBar::InsertCategory(int nItem, LPCTSTR lpszItem, UINT nIDResource, BOOL bLoadIcons)
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

	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);


	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return NULL;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return NULL;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return NULL;

	ASSERT(pData->wVersion == 1);

	UINT* pItems = new UINT[pData->wItemCount];

	for (int i = 0; i < pData->wItemCount; i++)
		pItems[i] = pData->items()[i];

	CXTPTabManagerItem* pItem = InsertCategory(nItem, lpszItem, pItems, pData->wItemCount, bLoadIcons);

	if (bLoadIcons)
	{
		CXTPImageManager* pImageManager = GetImageManager();

		pImageManager->SetIcons(nIDResource, pItems,
			pData->wItemCount, CSize(pData->wWidth, pData->wHeight));
	}

	delete[] pItems;

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return pItem;
}

CXTPTabManagerItem* CXTPTabToolBar::InsertCategory(int nItem, LPCTSTR lpszItem, UINT* pButtons, int nCount, BOOL bLoadIcons)
{
	CXTPTabManagerItem* pItem = AddItem(nItem);
	pItem->SetCaption(lpszItem);

	CWnd* pSite = GetSite();
	BOOL bSeparator = FALSE;
	CXTPImageManager* pImageManager = GetImageManager();

	for (int i = 0; i < nCount; i++)
	{
		if (pButtons[i] == 0)
			bSeparator = TRUE;
		else
		{
			XTPControlType controlType = xtpControlButton;
			XTPButtonStyle buttonStyle = xtpButtonAutomatic;
			CXTPControl* pControl = NULL;
			UINT nID = pButtons[i];

			XTP_COMMANDBARS_CREATECONTROL cs;

			if (pSite)
			{
				cs.nID = nID;
				cs.pControl = NULL;
				cs.bToolBar = TRUE;
				cs.pMenu = NULL;
				cs.nIndex = i;
				cs.strCaption = GetTitle();
				cs.controlType = controlType;
				cs.pCommandBar = this;
				cs.buttonStyle = buttonStyle;

				if (GetSite()->SendMessage(WM_XTP_BEFORECREATECONTROL, 0, (LPARAM)&cs) != 0)
				{
					pControl = cs.pControl;
					controlType = cs.controlType;
					buttonStyle = cs.buttonStyle;
					nID = cs.nID;
				}
			}

			if (pControl == NULL)
			{
				pControl = m_pControls->Add(controlType, nID);
				if (pControl)
				{
					pControl->SetStyle(buttonStyle);
					if (controlType == xtpControlPopup) pControl->SetIconId(nID);
				}
			}
			else m_pControls->Add(pControl, nID);

			if (bLoadIcons)
			{
				pImageManager->SetIcon(nID, nID);
			}

			if (bSeparator)
			{
				pControl->SetBeginGroup(TRUE);
				bSeparator = FALSE;
			}

			pControl->SetCategory(lpszItem);

			if (pSite)
			{
				cs.pControl = pControl;
				pSite->SendMessage(WM_XTP_AFTERCREATECONTROL, 0, (LPARAM)&cs);
			}
		}
	}

	if (nItem == 0)
	{
		SetCurSel(0);
	}

	UpdateTabs();
	m_pControls->CreateOriginalControls();

	return pItem;
}

void CXTPTabToolBar::UpdateTabs(CXTPTabManagerItem* pItem)
{
	if (pItem == NULL)
		pItem = GetSelectedItem();

	if (!pItem)
		return;

	for (int i = 0; i < m_pControls->GetCount(); i++)
	{
		CXTPControl* pControl = m_pControls->GetAt(i);

		if (pControl->GetCategory() == pItem->GetCaption())
		{
			pControl->SetHideFlag(xtpHideRibbonTab, FALSE);
		}
		else
		{
			pControl->SetHideFlag(xtpHideRibbonTab, TRUE);
		}
	}

	DelayLayout();
}

void CXTPTabToolBar::SetSelectedItem(CXTPTabManagerItem* pItem)
{
	if (m_pSelected != pItem)
	{
		UpdateTabs(pItem);

		GetCommandBars()->ClosePopups();

		CXTPTabManager::SetSelectedItem(pItem);

	}
}

BOOL CXTPTabToolBar::OnBeforeItemClick(CXTPTabManagerItem* pItem)
{
	return CXTPTabManager::OnBeforeItemClick(pItem);
}

BEGIN_MESSAGE_MAP(CXTPTabToolBar, CXTPToolBar)
	//{{AFX_MSG_MAP(CXTPTabToolBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPTabToolBar message handlers

void CXTPTabToolBar::DrawCommandBar(CDC* pDC, CRect rcClipBox)
{
	CXTPToolBar::DrawCommandBar(pDC, rcClipBox);

	GetPaintManager()->DrawTabControl(this, pDC, m_rcTabControl);
}

void CXTPTabToolBar::RedrawControl(LPCRECT lpRect, BOOL bAnimate)
{
	Redraw(lpRect, bAnimate);
}


void CXTPTabToolBar::Reposition()
{
	if (!GetSafeHwnd())
		return;

	if (!IsCustomizeMode()) DelayLayout(); else OnRecalcLayout();
}

void CXTPTabToolBar::Reset(BOOL bShowWarningMessage)
{
	CXTPToolBar::Reset(bShowWarningMessage);
	UpdateTabs();
}

CRect CXTPTabToolBar::GetBorders()
{
	CRect rcBorder = CXTPToolBar::GetBorders();

	CRect rc(0, 0, 100, 100);
	GetPaintManager()->AdjustClientRect(this, rc);

	rcBorder.top += rc.top;

	if (CXTPToolBar::GetPosition() != xtpBarFloating)
		rcBorder.left += rc.left;

	return rcBorder;
}

CSize CXTPTabToolBar::CalcDockingLayout(int nLength, DWORD dwMode, int nWidth)
{
	GetPaintManager()->SetPosition(dwMode & LM_VERTDOCK ?
		xtpTabPositionLeft : xtpTabPositionTop);

	CRect rc(0, 0, 100, 100);
	GetPaintManager()->AdjustClientRect(this, rc);

	CRect rcBorderOriginal = CXTPToolBar::GetBorders();
	int nExpandBorder = dwMode & LM_HORZDOCK ? rcBorderOriginal.right : rcBorderOriginal.bottom;

	CSize sz = CXTPToolBar::CalcDockingLayout(nLength, dwMode, nWidth);

	if (dwMode & LM_HORZ)
		sz.cx = max(m_nMinWidth, sz.cx);
	else
		sz.cy = max(m_nMinWidth, sz.cy);

	CXTPControl* pPopupExp = m_pControls->FindControl(XTP_ID_TOOLBAR_EXPAND);

	if (pPopupExp && ((pPopupExp->GetHideFlags() & xtpHideGeneric) == 0))
	{
		nExpandBorder += 11;
		CXTPCommandBar::GetPaintManager()->DrawSpecialControl(0, xtpButtonExpandToolbar, pPopupExp, this, FALSE, &sz);
	}

	if (dwMode & LM_COMMIT)
	{
		CClientDC dc(this);

		if (dwMode & LM_VERTDOCK)
			m_rcTabControl = CRect(rcBorderOriginal.left, rcBorderOriginal.top, rc.left, sz.cy - nExpandBorder);
		else
			m_rcTabControl = CRect(rcBorderOriginal.left, rcBorderOriginal.top, sz.cx - nExpandBorder, rc.top);

		GetPaintManager()->RepositionTabControl(this, &dc, m_rcTabControl);
	}

	return sz;

}

CSize CXTPTabToolBar::CalcDynamicLayout(int nLength, DWORD nMode)
{
	GetPaintManager()->SetPosition(xtpTabPositionTop);

	CRect rc(0, 0, 100, 100);
	GetPaintManager()->AdjustClientRect(this, rc);

	ASSERT(!(nMode & LM_HORZDOCK || nMode & LM_VERTDOCK));

	CClientDC dc(this);
	CXTPFontDC font(&dc, CXTPToolBar::GetPaintManager()->GetCommandBarFont(this));

	int nTitleSize = CXTPToolBar::GetPaintManager()->DrawCommandBarGripper(&dc, this, FALSE).cy;

	CRect rcBorderOriginal(3, 3 + nTitleSize + 1, 3, 3);
	CRect rcBorder(rcBorderOriginal);
	rcBorder.top += rc.top;

	nLength = max(m_nMinWidth, nLength);

	CSize sz = CXTPToolBar::CalcDynamicLayout(nLength, nMode);

	sz.cx = max(m_nMinWidth, sz.cx);

	int nLeft = sz.cx - 3;

	CXTPControl* pHide = m_pControls->FindControl(XTP_ID_TOOLBAR_HIDE);;
	CXTPControl* pPopupExp = m_pControls->FindControl(XTP_ID_TOOLBAR_EXPAND);
	if (pHide)
	{
		pHide->SetHideFlags(xtpNoHide);
		pHide->SetRect(CRect(nLeft - nTitleSize, 3, nLeft, 3 + nTitleSize));
		nLeft -= nTitleSize;

		if (pPopupExp) pPopupExp->SetWrap(TRUE);
	}

	BOOL bShowExpandButtonAlways = TRUE;
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (pCommandBars) bShowExpandButtonAlways = pCommandBars->GetCommandBarsOptions()->bShowExpandButtonAlways;

	if (pPopupExp && nLeft - nTitleSize > 0 && bShowExpandButtonAlways)
	{
		pPopupExp->SetWrap(TRUE);
		pPopupExp->SetHideFlags(xtpNoHide);
		pPopupExp->SetRect(CRect(nLeft - nTitleSize, 3, nLeft, 3 + nTitleSize));
	}

	if (nMode & LM_COMMIT)
	{
		m_rcTabControl = CRect(rcBorderOriginal.left, rcBorderOriginal.top, sz.cx - rcBorderOriginal.right,
			rcBorderOriginal.top + rc.top);

		GetPaintManager()->RepositionTabControl(this, &dc, m_rcTabControl);
	}

	return sz;
}

void CXTPTabToolBar::OnCustomizeDrop(CXTPControl* pDataObject, DROPEFFECT& dropEffect, CPoint ptDrop, CPoint ptDrag)
{
	CXTPToolBar::OnCustomizeDrop(pDataObject, dropEffect, ptDrop, ptDrag);

	CXTPCommandBars* pCommandBars = GetCommandBars();
	ASSERT(pCommandBars);

	if (pCommandBars->GetDragControl() && GetSelectedItem())
	{
		pCommandBars->GetDragControl()->SetCategory(GetSelectedItem()->GetCaption());
	}
}


void CXTPTabToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!PerformClick(m_hWnd, point))
	{
		CXTPToolBar::OnLButtonDown(nFlags, point);
	}
}

void CXTPTabToolBar::OnSysColorChange()
{
	CXTPToolBar::OnSysColorChange();

	GetPaintManager()->RefreshMetrics();
	Reposition();
}

INT_PTR CXTPTabToolBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	INT_PTR nHit = PerformToolHitTest(m_hWnd, point, pTI);
	if (nHit != -1)
		return nHit;

	return CXTPToolBar::OnToolHitTest(point, pTI);
}

void CXTPTabToolBar::OnMouseMove(UINT nFlags, CPoint point)
{
	PerformMouseMove(m_hWnd, point);
	CXTPToolBar::OnMouseMove(nFlags, point);
}

void CXTPTabToolBar::OnMouseLeave()
{
	PerformMouseMove(m_hWnd, CPoint(-1, -1));
	CXTPToolBar::OnMouseLeave();
}

BOOL CXTPTabToolBar::DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon) const
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




