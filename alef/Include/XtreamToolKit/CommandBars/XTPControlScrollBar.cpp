// XTPControlScrollBar.cpp : implementation of the CXTPControlScrollBar class.
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

#include "XTPControlScrollBar.h"
#include "XTPPaintManager.h"
#include "XTPCommandBar.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_XTP_CONTROL(CXTPControlScrollBar, CXTPControl)

CXTPControlScrollBar::CXTPControlScrollBar()
{
	m_nWidth = 150;
	m_pScrollBarCtrl = new CXTPControlScrollBarCtrl;
	m_pScrollBarCtrl->m_pControl = this;
	m_bHoriz = TRUE;

	m_nMax = 100;
	m_nMin = 0;
	m_nPos = 0;
	m_barStyle = xtpScrollStyleDefault;
}

CXTPControlScrollBar::~CXTPControlScrollBar()
{
	delete m_pScrollBarCtrl;
}

void CXTPControlScrollBar::Copy(CXTPControl* pControl, BOOL bRecursive)
{
	CXTPControlScrollBar* pControlScrollBar = DYNAMIC_DOWNCAST(CXTPControlScrollBar, pControl);
	ASSERT(pControlScrollBar);

	SetRange(pControlScrollBar->m_nMin, pControlScrollBar->m_nMax);
	SetPos(pControlScrollBar->GetPos());
	m_barStyle = pControlScrollBar->m_barStyle;

	CXTPControl::Copy(pControl, bRecursive);
}

void CXTPControlScrollBar::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControl::DoPropExchange(pPX);

	PX_Int(pPX, _T("Min"), m_nMin, 0);
	PX_Int(pPX, _T("Max"), m_nMax, 100);
	PX_Int(pPX, _T("Pos"), m_nPos, 0);
	PX_Enum(pPX, _T("ScrollBarStyle"), m_barStyle, xtpScrollStyleDefault);
}

CSize CXTPControlScrollBar::GetSize(CDC* /*pDC*/)
{
	if (m_pScrollBarCtrl->GetSafeHwnd() && m_pScrollBarCtrl->GetStyle() & SBS_VERT)
	{
		return CSize(GetSystemMetrics(SM_CXVSCROLL) + 4, GetWidth());
	}
	return CSize(GetWidth(), GetSystemMetrics(SM_CYHSCROLL) + 4);
}

void CXTPControlScrollBar::Draw(CDC* /*pDC*/)
{

}


void CXTPControlScrollBar::SetRect(CRect rcControl)
{
	if (m_rcControl == rcControl && !(m_pScrollBarCtrl && !m_pScrollBarCtrl->GetSafeHwnd()))
	{
		if (!(m_pScrollBarCtrl && m_pScrollBarCtrl->GetSafeHwnd() && m_pScrollBarCtrl->GetParent() != m_pParent))
			return;
	}

	if (m_pScrollBarCtrl && m_pScrollBarCtrl->GetSafeHwnd() && m_pScrollBarCtrl->GetParent() != m_pParent)
	{
		m_pScrollBarCtrl->DestroyWindow();
	}

	m_rcControl = rcControl;

	if (m_pScrollBarCtrl)
	{
		if (!m_pScrollBarCtrl->GetSafeHwnd())
		{
			m_pScrollBarCtrl->SetCommandBars(m_pParent->GetCommandBars());
			m_pScrollBarCtrl->Create(WS_CHILD | (m_bHoriz ? SBS_HORZ : SBS_VERT), CRect(0, 0, 0, 0), m_pParent, 0);
			m_pScrollBarCtrl->SetScrollRange(m_nMin, m_nMax);
			m_pScrollBarCtrl->SetScrollPos(m_nPos);
		}
		m_pScrollBarCtrl->SetScrollBarStyle(m_barStyle);

		rcControl.DeflateRect(2, 2, 2, 2);

		if (m_pParent->GetType() == xtpBarTypePopup && m_pParent->GetPosition() == xtpBarPopup)
		{
			CXTPPaintManager* pPaintManager = GetPaintManager();
			rcControl.left += pPaintManager->GetPopupBarGripperWidth(GetParent())
				+ pPaintManager->m_nPopupBarTextPadding - 2;
		}

		m_pScrollBarCtrl->MoveWindow(rcControl);
		m_pScrollBarCtrl->SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | ((m_dwHideFlags != xtpNoHide) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW));

		m_pScrollBarCtrl->EnableWindow(m_bEnabled);
		m_pScrollBarCtrl->Invalidate(FALSE);
	}
}

void CXTPControlScrollBar::SetPos(int nPos)
{
	if (m_pScrollBarCtrl->GetSafeHwnd() && m_pScrollBarCtrl->m_pSBTrack != NULL)
		return;

	m_nPos = nPos;

	if (m_pScrollBarCtrl->GetSafeHwnd())
	{
		m_pScrollBarCtrl->SetScrollPos(nPos);
	}
}

int CXTPControlScrollBar::GetPos()
{
	if (m_pScrollBarCtrl->GetSafeHwnd())
	{
		return m_nPos = m_pScrollBarCtrl->GetScrollPos();
	}

	return m_nPos;
}

void CXTPControlScrollBar::SetRange( int nMin, int nMax)
{
	m_nMin = nMin;
	m_nMax = nMax;

	if (m_pScrollBarCtrl->GetSafeHwnd())
	{
		m_pScrollBarCtrl->SetScrollRange(nMin, nMax);
	}
}

BOOL CXTPControlScrollBar::HasFocus() const
{
	return ::GetCapture() == m_pScrollBarCtrl->GetSafeHwnd();
}

void CXTPControlScrollBar::SetEnabled(BOOL bEnabled)
{
	if (bEnabled != m_bEnabled)
	{
		m_bEnabled = bEnabled;
		if (m_pScrollBarCtrl && m_pScrollBarCtrl->GetSafeHwnd())
			m_pScrollBarCtrl->EnableWindow(bEnabled);

		DelayRedrawParent();
	}
}

void CXTPControlScrollBar::SetParent(CXTPCommandBar* pParent)
{
	CXTPControl::SetParent(pParent);

	if (!pParent && m_pScrollBarCtrl && m_pScrollBarCtrl->GetSafeHwnd())
	{
		m_pScrollBarCtrl->DestroyWindow();
	}
}

void CXTPControlScrollBar::SetHideFlags(DWORD dwFlags)
{
	if (m_dwHideFlags != dwFlags)
	{
		m_dwHideFlags = dwFlags;

		if (m_pScrollBarCtrl && m_pScrollBarCtrl->GetSafeHwnd())
			m_pScrollBarCtrl->SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | ((m_dwHideFlags != xtpNoHide) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW));
		DelayLayoutParent();
	}
}

void CXTPControlScrollBar::OnCalcDynamicSize(DWORD dwMode)
{
	BOOL bHoriz = !(dwMode & LM_VERTDOCK);

	if (bHoriz != m_bHoriz)
	{
		m_pScrollBarCtrl->DestroyWindow();
		m_bHoriz = bHoriz;
	}
}

void CXTPControlScrollBar::OnClick(BOOL bKeyboard, CPoint pt)
{
	if (IsCustomizeMode() && !bKeyboard)
	{
		m_pParent->SetPopuped(-1);
		m_pParent->SetSelected(-1);
		CustomizeStartDrag(pt);
	}
	else
	{
	}
}

void CXTPControlScrollBar::OnScroll(UINT nSBCode, UINT nPos)
{
	int nCurPos = GetPos();

	switch (nSBCode)
	{
		case SB_TOP: nCurPos = 0; break;
		case SB_BOTTOM: nCurPos = m_nMax; break;

		case SB_LINEUP: nCurPos = max(nCurPos - 1, 0); break;
		case SB_LINEDOWN: nCurPos = min(nCurPos + 1, m_nMax); break;

		case SB_THUMBTRACK: nCurPos = nPos; break;
		case SB_PAGEUP: nCurPos = max(nCurPos - m_nMax / 10, 0); break;
		case SB_PAGEDOWN: nCurPos = min(nCurPos + m_nMax / 10, m_nMax); break;
	}

	if (nCurPos == m_nPos)
		return;

	m_pScrollBarCtrl->SetScrollPos(m_nPos = nCurPos);

	NotifySite(CBN_XTP_EXECUTE);
}



//////////////////////////////////////////////////////////////////////////
// CXTPControlScrollBarCtrl

CXTPControlScrollBarCtrl::CXTPControlScrollBarCtrl()
{

}
CXTPControlScrollBarCtrl::~CXTPControlScrollBarCtrl()
{
}

BEGIN_MESSAGE_MAP(CXTPControlScrollBarCtrl, CXTPScrollBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()

	ON_WM_VSCROLL_REFLECT()
	ON_WM_HSCROLL_REFLECT()

END_MESSAGE_MAP()

void CXTPControlScrollBarCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pControl->IsCustomizeMode())
	{
		if (m_pControl->GetParent()->IsCustomizable() && ((m_pControl->GetFlags() & xtpFlagNoMovable) == 0))
			m_pControl->OnClick();
	}
	else CXTPScrollBar::OnLButtonDown(nFlags, point);
}

void CXTPControlScrollBarCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pControl->IsCustomizeMode())
	{
		ClientToScreen(&point);
		m_pControl->GetParent()->ScreenToClient(&point);

		m_pControl->GetParent()->OnRButtonDown(nFlags, point);
		return;
	}


	CXTPScrollBar::OnRButtonDown(nFlags, point);
}


void CXTPControlScrollBarCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CXTPScrollBar::OnMouseMove(nFlags, point);

	MapWindowPoints(m_pControl->GetParent(), &point, 1);
	m_pControl->GetParent()->OnMouseMove(nFlags, point);
}

void CXTPControlScrollBarCtrl::HScroll(UINT nSBCode, UINT nPos)
{
	OnScroll(nSBCode, nPos);
}

void CXTPControlScrollBarCtrl::VScroll(UINT nSBCode, UINT nPos)
{
	OnScroll(nSBCode, nPos);
}

void CXTPControlScrollBarCtrl::OnScroll(UINT nSBCode, UINT nPos)
{
	m_pControl->OnScroll(nSBCode, nPos);
}
