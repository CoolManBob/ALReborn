// XTPRibbonSystemButton.cpp : implementation file
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
#include "Resource.h"

#include "Common/XTPOffice2007Image.h"
#include "Common/XTPIntel80Helpers.h"
#include "CommandBars/XTPCommandBars.h"
#include "CommandBars/XTPControlExt.h"

#include "XTPRibbonSystemButton.h"
#include "XTPRibbonBar.h"
#include "XTPRibbonTheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_XTP_CONTROL(CXTPRibbonControlSystemButton, CXTPControlPopup)

CXTPRibbonControlSystemButton::CXTPRibbonControlSystemButton()
{
	SetFlags(xtpFlagNoMovable | xtpFlagManualUpdate);
}

void CXTPRibbonControlSystemButton::Draw(CDC* pDC)
{
	((CXTPRibbonBar*)GetParent())->GetRibbonPaintManager()->DrawRibbonFrameSystemButton(pDC, this, GetRect());
}

BOOL CXTPRibbonControlSystemButton::OnLButtonDblClk(CPoint /*point*/)
{
	GetParent()->GetCommandBars()->ClosePopups();
	GetParent()->GetSite()->SendMessage(WM_SYSCOMMAND, SC_CLOSE | HTSYSMENU, 0);

	return TRUE;
}

void CXTPRibbonControlSystemButton::AdjustExcludeRect(CRect& rc, BOOL bVertical)
{
	CXTPRibbonBar* pRibbonBar = DYNAMIC_DOWNCAST(CXTPRibbonBar, GetParent());
	if (!pRibbonBar)
	{
		CXTPControlPopup::AdjustExcludeRect(rc, bVertical);
		return;
	}

	if (pRibbonBar->IsCaptionVisible() && pRibbonBar->IsTabsVisible() &&
		DYNAMIC_DOWNCAST(CXTPRibbonSystemPopupBar, GetCommandBar()))
	{
		rc.bottom -= 18;
		return;
	}

	//rc.bottom -= 20;
	CXTPControlPopup::AdjustExcludeRect(rc, bVertical);
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_XTP_COMMANDBAR(CXTPRibbonSystemPopupBar, CXTPPopupBar)

BEGIN_MESSAGE_MAP(CXTPRibbonSystemPopupBar, CXTPPopupBar)
	ON_WM_NCHITTEST_EX()
END_MESSAGE_MAP()

CXTPRibbonSystemPopupBar::CXTPRibbonSystemPopupBar()
{
	m_rcBorders.SetRect(6, 18, 6, 29);
	SetShowGripper(FALSE);
}

CRect CXTPRibbonSystemPopupBar::GetBorders()
{
	return m_rcBorders;
}

void CXTPRibbonSystemPopupBar::FillCommandBarEntry(CDC* pDC)
{
	CXTPClientRect rc(this);
	CXTPOffice2007Theme* pPaintManager = (CXTPOffice2007Theme*)GetPaintManager();

	CXTPOffice2007Image* pImage = pPaintManager->LoadImage(_T("RIBBONSYSTEMMENU"));
	if (!pImage)
		return;

	pImage->DrawImage(pDC, rc, pImage->GetSource(), m_rcBorders, COLORREF_NULL);

	if (!DYNAMIC_DOWNCAST(CXTPRibbonControlSystemButton, m_pControlPopup))
		return;

	CXTPRibbonBar* pRibbonBar = DYNAMIC_DOWNCAST(CXTPRibbonBar, m_pControlPopup->GetParent());
	if (!pRibbonBar)
		return;

	CRect rcPopup = m_pControlPopup->GetRect();
	m_pControlPopup->GetParent()->ClientToScreen(rcPopup);
	ScreenToClient(rcPopup);

	if (pRibbonBar->IsCaptionVisible() && CRect().IntersectRect(rcPopup, rc))
	{
		((CXTPRibbonTheme*)pPaintManager)->DrawRibbonFrameSystemButton(pDC, m_pControlPopup, rcPopup);
	}

	for (int i = 0; i < GetControlCount(); i++)
	{
		CXTPControl* pControl = GetControl(i);
		if (pControl && pControl->GetWrap())
		{
			CRect rcRow = pControl->GetRowRect();
			pDC->FillSolidRect(rcRow, pPaintManager->GetImages()->GetImageColor(_T("Ribbon"), _T("RecentFileListBackground")));
			break;
		}
	}

}

LRESULT CXTPRibbonSystemPopupBar::OnNcHitTest(CPoint point)
{
	if (!DYNAMIC_DOWNCAST(CXTPRibbonControlSystemButton, m_pControlPopup))
		return CXTPPopupBar::OnNcHitTest(point);

	CRect rcPopup = m_pControlPopup->GetRect();
	m_pControlPopup->GetParent()->ClientToScreen(rcPopup);
	if (rcPopup.PtInRect(point))
		return HTTRANSPARENT;

	return CXTPPopupBar::OnNcHitTest(point);
}

CSize CXTPRibbonSystemPopupBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CArray<CXTPControl*, CXTPControl*> arrButtons;
	int i;

	for (i = 0; i < GetControlCount(); i++)
	{
		CXTPControl* pControl = GetControl(i);
		if (pControl && pControl->IsKindOf(RUNTIME_CLASS(CXTPRibbonControlSystemPopupBarButton)))
		{
			arrButtons.Add(pControl);
			pControl->SetHideFlag(xtpHideDockingPosition, TRUE);
		}
	}

	CSize sz = CXTPPopupBar::CalcDynamicLayout(nLength, dwMode);

	if (arrButtons.GetSize() == 0)
		return sz;

	CClientDC dc(this);
	CXTPFontDC font(&dc, GetPaintManager()->GetCommandBarFont(this));

	int nRight = sz.cx - m_rcBorders.right + 1;

	for (i = (int)arrButtons.GetSize() - 1; i >= 0; i--)
	{
		CXTPControl* pControl = arrButtons[i];
		pControl->SetHideFlag(xtpHideDockingPosition, FALSE);

		CSize szControl = pControl->GetSize(&dc);
		pControl->SetRect(CRect(nRight - szControl.cx, sz.cy - m_rcBorders.bottom + 4, nRight, sz.cy - 3));
		nRight -= szControl.cx + 6;
	}

	return sz;
}

//////////////////////////////////////////////////////////////////////////
// CXTPRibbonControlSystemPopupBarButton

IMPLEMENT_XTP_CONTROL(CXTPRibbonControlSystemPopupBarButton, CXTPControlButton)

CXTPRibbonControlSystemPopupBarButton::CXTPRibbonControlSystemPopupBarButton()
{

}

BOOL CXTPRibbonControlSystemPopupBarButton::IsTransparent() const
{
	return TRUE;
}

void CXTPRibbonControlSystemPopupBarButton::Draw(CDC* pDC)
{
	CXTPOffice2007Theme* pPaintManager = (CXTPOffice2007Theme*)GetPaintManager();
	CXTPOffice2007Image* pImage = pPaintManager->LoadImage(_T("RIBBONSYSTEMMENUBUTTON"));

	if (pImage)
	{
		pImage->DrawImage(pDC, GetRect(), pImage->GetSource(GetSelected() ? 1 : 0, 2), CRect(2, 2, 2, 2), 0xFF00FF);
	}

	GetPaintManager()->DrawControlToolBarParent(pDC, this, TRUE);
}

CSize CXTPRibbonControlSystemPopupBarButton::GetSize(CDC* pDC)
{
	return GetPaintManager()->DrawControlToolBarParent(pDC, this, FALSE);
}

CSize CXTPRibbonControlSystemPopupBarButton::GetButtonSize() const
{
	return CSize(22, 22);
}

CSize CXTPRibbonControlSystemPopupBarButton::GetIconSize() const
{
	return CSize(16, 16);
}


//////////////////////////////////////////////////////////////////////////
// CXTPControlRecentFileList
IMPLEMENT_XTP_CONTROL(CXTPRibbonControlSystemPopupBarListItem, CXTPControlButton)

CXTPRibbonControlSystemPopupBarListItem::CXTPRibbonControlSystemPopupBarListItem()
{
	m_nWidth = 300;
	m_nHeight = 21;
}

CSize CXTPRibbonControlSystemPopupBarListItem::GetSize(CDC* /*pDC*/)
{
	return CSize(m_nWidth, m_nHeight);
}

void CXTPRibbonControlSystemPopupBarListItem::Draw(CDC* pDC)
{
	CXTPPaintManager* pPaintManager = (CXTPPaintManager*)GetPaintManager();

	pPaintManager->DrawControlEntry(pDC, this);

	COLORREF clrText = pPaintManager->GetControlTextColor(this);

	pDC->SetTextColor(clrText);
	pDC->SetBkMode (TRANSPARENT);

	CRect rc(GetRect());
	CRect rcText(rc.left + 7, rc.top, rc.right, rc.bottom);

	pDC->DrawText(GetCaption(), &rcText, DT_SINGLELINE | DT_VCENTER);
}


IMPLEMENT_XTP_CONTROL(CXTPRibbonControlSystemPopupBarListCaption, CXTPControl)

CXTPRibbonControlSystemPopupBarListCaption::CXTPRibbonControlSystemPopupBarListCaption()
{
	SetFlags(xtpFlagManualUpdate | xtpFlagSkipFocus | xtpFlagNoMovable | xtpFlagWrapRow);
	m_nWidth = 300;
	m_nHeight = 27;
}

CSize CXTPRibbonControlSystemPopupBarListCaption::GetSize(CDC* /*pDC*/)
{
	return CSize(m_nWidth, m_nHeight);
}

void CXTPRibbonControlSystemPopupBarListCaption::Draw(CDC* pDC)
{
	CXTPOffice2007Theme* pPaintManager = (CXTPOffice2007Theme*)GetPaintManager();
	CXTPFontDC dont(pDC, pPaintManager->GetRegularBoldFont());

	COLORREF clrText = pPaintManager->GetControlTextColor(this);

	pDC->SetTextColor(clrText);
	pDC->SetBkMode (TRANSPARENT);

	CRect rc(GetRect());
	CRect rcText(rc.left + 7, rc.top + 4, rc.right, rc.bottom - 5);

	pDC->DrawText(GetCaption(), &rcText, DT_SINGLELINE | DT_TOP);

	pPaintManager->HorizontalLine(pDC, rc.left, rc.bottom - 6, rc.right,
		pPaintManager->GetImages()->GetImageColor(_T("Ribbon"), _T("RecentFileListEdgeShadow")));
	pPaintManager->HorizontalLine(pDC, rc.left, rc.bottom - 5, rc.right,
		pPaintManager->GetImages()->GetImageColor(_T("Ribbon"), _T("RecentFileListEdgeHighLight")));
}

IMPLEMENT_XTP_CONTROL(CXTPRibbonControlSystemRecentFileList, CXTPRibbonControlSystemPopupBarListCaption)

CXTPRibbonControlSystemRecentFileList::CXTPRibbonControlSystemRecentFileList()
{
}

CRecentFileList* CXTPRibbonControlSystemRecentFileList::GetRecentFileList()
{
	USES_PROTECTED_ACCESS(CXTPRibbonControlSystemRecentFileList, CWinApp, CRecentFileList*, m_pRecentFileList)

	return PROTECTED_ACCESS(CWinApp, AfxGetApp(), m_pRecentFileList);

}

int CXTPRibbonControlSystemRecentFileList::GetFirstMruID()
{
	return ID_FILE_MRU_FILE1;
}

UINT AFXAPI AfxGetFileTitle(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);

void CXTPRibbonControlSystemRecentFileList::OnCalcDynamicSize(DWORD /*dwMode*/)
{
	CRecentFileList* pRecentFileList = GetRecentFileList();

	if (!pRecentFileList)
		return;

	ASSERT(pRecentFileList->m_arrNames != NULL);
	if (!pRecentFileList->m_arrNames)
		return;

	while (m_nIndex + 1 < m_pControls->GetCount())
	{
		CXTPControl* pControl = m_pControls->GetAt(m_nIndex + 1);
		if (pControl->GetID() >= GetFirstMruID() && pControl->GetID() <= GetFirstMruID() + pRecentFileList->m_nSize)
		{
			m_pControls->Remove(pControl);
		}
		else break;
	}

	if (m_pParent->IsCustomizeMode())
	{
		m_dwHideFlags = 0;
		SetEnabled(TRUE);
		return;
	}

	CString strName;

	for (int iMRU = 0; iMRU < pRecentFileList->m_nSize; iMRU++)
	{
		if (pRecentFileList->m_arrNames[iMRU].IsEmpty())
			break;

		// copy file name only since directories are same
		AfxGetFileTitle(pRecentFileList->m_arrNames[iMRU], strName.GetBuffer(_MAX_PATH), _MAX_PATH);
		strName.ReleaseBuffer();

		CXTPControl* pControl = m_pControls->Add(new CXTPRibbonControlSystemPopupBarListItem(), iMRU + GetFirstMruID(), _T(""), m_nIndex + iMRU + 1 , TRUE);

		pControl->SetCaption(CXTPControlWindowList::ConstructCaption(strName, iMRU + 1));
		pControl->SetFlags(xtpFlagManualUpdate|xtpFlagShowPopupBarTip);
		pControl->SetTooltip(pRecentFileList->m_arrNames[iMRU]);
		pControl->SetDescription(NULL);
	}
}

BOOL CXTPRibbonControlSystemRecentFileList::IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint /*point*/, DROPEFFECT& dropEffect)
{
	if (pCommandBar->GetType() != xtpBarTypePopup)
	{
		dropEffect = DROPEFFECT_NONE;
		return FALSE;
	}
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
// CXTPRibbonSystemPopupBarPage


IMPLEMENT_XTP_COMMANDBAR(CXTPRibbonSystemPopupBarPage, CXTPPopupBar)

CXTPRibbonSystemPopupBarPage::CXTPRibbonSystemPopupBarPage()
{

}

CSize CXTPRibbonSystemPopupBarPage::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CSize sz = CXTPPopupBar::CalcDynamicLayout(nLength, dwMode);

	if (!m_pControlPopup->GetParent()->IsKindOf(RUNTIME_CLASS(CXTPRibbonSystemPopupBar)))
		return sz;

	CXTPRibbonSystemPopupBar* pParent = ((CXTPRibbonSystemPopupBar*)m_pControlPopup->GetParent());

	int nHeight = CXTPClientRect(pParent).Height() - (pParent->GetBorders().top + pParent->GetBorders().bottom) - 2;

	if (nHeight > sz.cy)
		sz.cy = nHeight;

	return sz;
}

void CXTPRibbonSystemPopupBarPage::AdjustExcludeRect(CRect& rc, BOOL bVertical)
{
	if (!m_pControlPopup->GetParent()->IsKindOf(RUNTIME_CLASS(CXTPRibbonSystemPopupBar)))
	{
		CXTPPopupBar::AdjustExcludeRect(rc, bVertical);
		return;
	}

	rc.top = ((CXTPRibbonSystemPopupBar*)m_pControlPopup->GetParent())->GetBorders().top + 1;
	rc.right += 1;
}
