// XTPControlListBox.cpp : implementation of the CXTPControlListBox class.
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
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPPropExchange.h"

#include "XTPControl.h"
#include "XTPCommandBar.h"
#include "XTPPaintManager.h"
#include "XTPMouseManager.h"
#include "XTPControlListBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

class CXTPControlListBoxCtrl : public CListBox
{
	friend class CXTPControlListBox;

protected:
	void OnSelChanged();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPControlListBoxCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

public:
	CXTPControlListBox* m_pControl;
};



IMPLEMENT_XTP_CONTROL(CXTPControlListBox, CXTPControl)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPControlListBox::CXTPControlListBox()
{
	m_pListBox = new CXTPControlListBoxCtrl;
	m_pListBox->m_pControl = this;

	m_pListBox->CreateEx(0, _T("LISTBOX"), NULL, WS_POPUP | WS_VSCROLL | WS_CLIPCHILDREN | LBS_NOTIFY, CRect(0, 0, 0, 0), 0, 0);

	m_nListBoxLinesMin = 1;
	m_nListBoxLinesMax = 12;
	m_nWidth = 100;
	m_bMultiplSel = FALSE;

	m_bMouseLocked = FALSE;
}

CXTPControlListBox::~CXTPControlListBox()
{
	if (m_pListBox)
	{
		delete m_pListBox;
	}

}
CListBox* CXTPControlListBox::GetListCtrl() const
{
	ASSERT_VALID(this);
	return m_pListBox;
}

void CXTPControlListBox::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
}

void CXTPControlListBox::SetMultiplSel(BOOL bMultiplSel)
{
	m_bMultiplSel = bMultiplSel;

	m_pListBox->DestroyWindow();
	m_pListBox->CreateEx(0, _T("LISTBOX"), NULL, WS_POPUP | WS_VSCROLL | WS_CLIPCHILDREN | LBS_NOTIFY | (m_bMultiplSel ? LBS_MULTIPLESEL : 0), CRect(0, 0, 0, 0), 0, 0);
}


void CXTPControlListBox::SetLinesMinMax(int nMin, int nMax)
{
	m_nListBoxLinesMin = nMin;
	m_nListBoxLinesMax = nMax;
}

void CXTPControlListBox::SetRect(CRect rcControl)
{
	ASSERT_VALID(this);

	if (m_rcControl == rcControl && m_pListBox->GetParent() == m_pParent)
	{
		return;
	}

	m_rcControl = rcControl;

	m_pListBox->EnableWindow(GetEnabled());

	m_pListBox->ModifyStyle(WS_POPUP, WS_CHILD);
	m_pListBox->SetParent(m_pParent);
	m_pListBox->MoveWindow(rcControl);
	m_pListBox->SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | (!IsVisible() ? SWP_HIDEWINDOW : SWP_SHOWWINDOW));
}

void CXTPControlListBox::SetParent(CXTPCommandBar* pParent)
{
	if (pParent != m_pParent && (pParent && pParent->GetSafeHwnd()))
	{
		m_pListBox->ModifyStyle(WS_POPUP, WS_CHILD);
		m_pListBox->SetParent(pParent);
	}
	CXTPControl::SetParent(pParent);
}

void CXTPControlListBox::SetEnabled(BOOL bEnabled)
{
	ASSERT_VALID(this);

	if (bEnabled != m_bEnabled)
	{
		m_bEnabled = bEnabled;
		m_pListBox->EnableWindow(bEnabled);
	}
}

CSize CXTPControlListBox::GetSize(CDC* /*pDC*/)
{
	ASSERT_VALID(this);

	m_pListBox->SetFont(GetPaintManager()->GetIconFont(), FALSE);

	int nItemHeight = m_pListBox->GetItemHeight(0);
	int nHeight = min (m_nListBoxLinesMax, max(m_nListBoxLinesMin, m_pListBox->GetCount())) * nItemHeight;
	int nWidth = m_nWidth;

	return CSize(nWidth, nHeight);
}

void CXTPControlListBox::Draw(CDC* /*pDC*/)
{

}

BOOL CXTPControlListBox::IsFocused() const
{
	return m_bSelected;
}

BOOL  CXTPControlListBox::OnSetSelected(int bSelected)
{
	if (!CXTPControl::OnSetSelected(bSelected))
		return FALSE;

	if (bSelected && !m_bMouseLocked)
	{
		XTPMouseManager()->LockMouseMove();
		m_bMouseLocked = TRUE;
	}
	else if (!bSelected && m_bMouseLocked)
	{
		XTPMouseManager()->UnlockMouseMove();
		m_bMouseLocked = FALSE;

	}

	return TRUE;
}

BOOL CXTPControlListBox::OnHookKeyDown(UINT nChar, LPARAM lParam)
{
	ASSERT_VALID(this);

	if (nChar == VK_RETURN || nChar == VK_TAB)
		return FALSE;

	if (nChar == VK_ESCAPE)
		return FALSE;


	int nSel = m_pListBox->GetCurSel();

	m_pListBox->SendMessage(WM_KEYDOWN, nChar, lParam);

	if (nSel != m_pListBox->GetCurSel())
	{
		m_pListBox->OnSelChanged();
	}


	return TRUE;
}

void CXTPControlListBox::OnClick(BOOL bKeyboard , CPoint pt)
{
	if (bKeyboard)
	{
		OnExecute();
	}
	else CXTPControl::OnClick(bKeyboard, pt);
}

void CXTPControlListBox::OnCalcDynamicSize(DWORD dwMode)
{
	CXTPControl::OnCalcDynamicSize(dwMode);

	if (m_bMultiplSel)
	{
		m_pListBox->SelItemRange(FALSE, 0, m_pListBox->GetCount() - 1);
	}
	else
	{
		m_pListBox->SetCurSel(-1);
	}

	NotifySite(XTP_LBN_POPUP);
}


void CXTPControlListBox::Copy(CXTPControl* pControl, BOOL bRecursive)
{
	ASSERT_KINDOF(CXTPControlListBox, pControl);

	CXTPControl::Copy(pControl, bRecursive);

	m_nWidth = ((CXTPControlListBox*)pControl)->m_nWidth;
	SetMultiplSel(((CXTPControlListBox*)pControl)->m_bMultiplSel);
	m_nListBoxLinesMax = ((CXTPControlListBox*)pControl)->m_nListBoxLinesMax;
	m_nListBoxLinesMin = ((CXTPControlListBox*)pControl)->m_nListBoxLinesMin;


	CListBox* pListBox = ((CXTPControlListBox*)pControl)->GetListCtrl();
	DWORD dwCount = pListBox->m_hWnd ? pListBox->GetCount() : 0;
	m_pListBox->ResetContent();

	for (UINT i = 0; i < dwCount; i++)
	{
		CString str;
		pListBox->GetText(i, str);
		m_pListBox->AddString(str);
	}

}

void CXTPControlListBox::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControl::DoPropExchange(pPX);
	CString str;

	PX_Int(pPX, _T("Width"), m_nWidth, 100);
	PX_Bool(pPX, _T("MultiplSel"), m_bMultiplSel, FALSE);
	PX_Int(pPX, _T("ListBoxLinesMax"), m_nListBoxLinesMax, 12);
	PX_Int(pPX, _T("ListBoxLinesMin"), m_nListBoxLinesMin, 1);

	CXTPPropExchangeSection secItems(pPX->GetSection(_T("Items")));

	if (pPX->IsStoring())
	{
		DWORD dwCount = m_pListBox->m_hWnd ? m_pListBox->GetCount() : 0;
		secItems->WriteCount(dwCount);
		for (UINT i = 0; i < dwCount; i++)
		{
			m_pListBox->GetText(i, str);

			CString strSection;
			strSection.Format(_T("Item%i"), i);
			PX_String(&secItems, strSection, str, _T(""));
		}

	}
	else
	{
		SetMultiplSel(m_bMultiplSel);

		DWORD nNewCount = secItems->ReadCount();
		for (DWORD i = 0; i < nNewCount; i++)
		{
			CString strSection;
			strSection.Format(_T("Item%i"), i);

			PX_String(&secItems, strSection, str, _T(""));
			m_pListBox->AddString(str);
		}

	}

}



// mini hack.
BEGIN_MESSAGE_MAP(CXTPControlListBoxCtrl, CListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


void CXTPControlListBoxCtrl::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/)
{
}
void CXTPControlListBoxCtrl::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	m_pControl->OnExecute();
}

void CXTPControlListBoxCtrl::OnLButtonDblClk(UINT /*nFlags*/, CPoint /*point*/)
{
}

void CXTPControlListBoxCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetRedraw(FALSE);
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
	SetRedraw(TRUE);

	Invalidate();
	UpdateWindow();
	// repaint the scroll bar.
	::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}

void CXTPControlListBoxCtrl::OnPaint()
{
	CPaintDC dcPaint(this);
	CXTPClientRect rc(this);
	CXTPBufferDC dc(dcPaint, rc);

	dc.FillSolidRect(rc, GetXtremeColor(COLOR_WINDOW));
	CWnd::DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, 0);
}

BOOL CXTPControlListBoxCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}


void CXTPControlListBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{

	BOOL bOutside;
	UINT nItem = ItemFromPoint(point, bOutside);

	if (!bOutside)
	{
		SetCurSel(nItem);
		SetSel(nItem, TRUE);

		OnSelChanged();
	}

	MapWindowPoints(m_pControl->GetParent(), &point, 1);
	m_pControl->GetParent()->OnMouseMove(nFlags, point);

}

void CXTPControlListBoxCtrl::OnSelChanged()
{
	if (m_pControl->m_bMultiplSel)
	{
		int nSel = GetCurSel();

		if (nSel != LB_ERR)
		{
			SelItemRange(FALSE, nSel + 1, GetCount() - 1);
			if (nSel != 0) SelItemRange(TRUE, 0, nSel);
			else  SetSel(0, TRUE);

		}
		else
		{
			SelItemRange(FALSE, 0, GetCount() - 1);
		}
	}

	m_pControl->NotifySite(XTP_LBN_SELCHANGE);
}




///////////////////////////////////////////////////////////////////
// CXTPControlListBoxInfo

IMPLEMENT_XTP_CONTROL(CXTPControlStatic, CXTPControl)

CXTPControlStatic::CXTPControlStatic()
{
	m_nWidth = 0;

}
void CXTPControlStatic::SetWidth(int nWidth)
{
	m_nWidth = nWidth;

}

CSize CXTPControlStatic::GetSize(CDC* pDC)
{
	BOOL bVert = GetParent()->GetPosition() == xtpBarRight || GetParent()->GetPosition() == xtpBarLeft;

	CXTPEmptyRect rcText;

	CSize sz = GetPaintManager()->DrawControlText(pDC, this, &rcText, FALSE, bVert, FALSE, FALSE);

	if (bVert) return CSize(max(22, sz.cx), max(m_nWidth, sz.cy));

	return CSize(max(m_nWidth, sz.cx), max(22, sz.cy));

}

void CXTPControlStatic::Draw(CDC* pDC)
{
	BOOL bVert = GetParent()->GetPosition() == xtpBarRight || GetParent()->GetPosition() == xtpBarLeft;

	CRect rcText = GetRect();
	pDC->SetTextColor(GetPaintManager()->GetControlTextColor(this));
	GetPaintManager()->DrawControlText(pDC, this, &rcText, TRUE, bVert, TRUE, FALSE);
}


void CXTPControlStatic::Copy(CXTPControl* pControl, BOOL bRecursive)
{
	ASSERT_KINDOF(CXTPControlStatic, pControl);

	CXTPControl::Copy(pControl, bRecursive);

	m_nWidth = ((CXTPControlStatic*)pControl)->m_nWidth;

}
void CXTPControlStatic::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControl::DoPropExchange(pPX);

	PX_Int(pPX, _T("Width"), m_nWidth, 0);
}
