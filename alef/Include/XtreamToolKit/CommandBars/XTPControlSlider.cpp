// XTPControlSlider.cpp : implementation of the CXTPControlSlider class.
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

#include "XTPControlSlider.h"
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

IMPLEMENT_XTP_CONTROL(CXTPControlSlider, CXTPControl)

CXTPControlSlider::CXTPControlSlider()
{
	m_nWidth = 150;
	m_pSliderCtrl = new CXTPControlSliderCtrl;
	m_pSliderCtrl->m_pControl = this;

	m_nMax = 100;
	m_nMin = 0;
	m_nPos = 0;
}

CXTPControlSlider::~CXTPControlSlider()
{
	delete m_pSliderCtrl;
}

void CXTPControlSlider::Copy(CXTPControl* pControl, BOOL bRecursive)
{
	CXTPControlSlider* pControlSlider = DYNAMIC_DOWNCAST(CXTPControlSlider, pControl);
	ASSERT(pControlSlider);

	SetRange(pControlSlider->m_nMin, pControlSlider->m_nMax);
	SetPos(pControlSlider->GetPos());

	CXTPControl::Copy(pControl, bRecursive);
}

void CXTPControlSlider::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControl::DoPropExchange(pPX);

	PX_Int(pPX, _T("Min"), m_nMin, 0);
	PX_Int(pPX, _T("Max"), m_nMax, 100);
	PX_Int(pPX, _T("Pos"), m_nPos, 0);
}

CSize CXTPControlSlider::GetSize(CDC* /*pDC*/)
{
	int nButtonHeight = GetPaintManager()->GetControlHeight();

	if (m_pSliderCtrl->GetSafeHwnd() && m_pSliderCtrl->GetStyle() & TBS_VERT)
	{
		return CSize(nButtonHeight, GetWidth());
	}
	return CSize(GetWidth(), nButtonHeight);
}

void CXTPControlSlider::Draw(CDC* /*pDC*/)
{

}


void CXTPControlSlider::SetRect(CRect rcControl)
{
	if (m_rcControl == rcControl && !(m_pSliderCtrl && !m_pSliderCtrl->GetSafeHwnd()))
	{
		if (!(m_pSliderCtrl && m_pSliderCtrl->GetSafeHwnd() && m_pSliderCtrl->GetParent() != m_pParent))
			return;
	}

	if (m_pSliderCtrl && m_pSliderCtrl->GetSafeHwnd() && m_pSliderCtrl->GetParent() != m_pParent)
	{
		m_pSliderCtrl->DestroyWindow();
	}

	m_rcControl = rcControl;

	if (m_pSliderCtrl)
	{
		if (!m_pSliderCtrl->GetSafeHwnd())
		{
			m_pSliderCtrl->Create(WS_CHILD|TBS_BOTH|TBS_NOTICKS, CRect(0, 0, 0, 0), m_pParent, 0);
			m_pSliderCtrl->SetRange(m_nMin, m_nMax);
			m_pSliderCtrl->SetPos(m_nPos);
		}

		rcControl.DeflateRect(2, 2, 2, 2);

		if (m_pParent->GetType() == xtpBarTypePopup && m_pParent->GetPosition() == xtpBarPopup)
		{
			CXTPPaintManager* pPaintManager = GetPaintManager();
			rcControl.left += pPaintManager->GetPopupBarGripperWidth(GetParent())
				+ pPaintManager->m_nPopupBarTextPadding - 2;
		}

		m_pSliderCtrl->MoveWindow(rcControl);
		m_pSliderCtrl->SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | ((m_dwHideFlags != xtpNoHide) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW));

		m_pSliderCtrl->EnableWindow(m_bEnabled);
		m_pSliderCtrl->Invalidate(FALSE);
	}
}

void CXTPControlSlider::SetPos(int nPos)
{
	if (m_pSliderCtrl->GetSafeHwnd() && ::GetCapture() == m_pSliderCtrl->GetSafeHwnd())
		return;

	m_nPos = nPos;

	if (m_pSliderCtrl->GetSafeHwnd())
	{
		m_pSliderCtrl->SetPos(nPos);
	}
}

int CXTPControlSlider::GetPos()
{
	if (m_pSliderCtrl->GetSafeHwnd())
	{
		return m_nPos = m_pSliderCtrl->GetPos();
	}

	return m_nPos;
}

void CXTPControlSlider::SetRange( int nMin, int nMax)
{
	m_nMin = nMin;
	m_nMax = nMax;

	if (m_pSliderCtrl->GetSafeHwnd())
	{
		m_pSliderCtrl->SetRange(nMin, nMax);
	}
}

BOOL CXTPControlSlider::HasFocus() const
{
	return ::GetCapture() == m_pSliderCtrl->GetSafeHwnd();
}

void CXTPControlSlider::SetEnabled(BOOL bEnabled)
{
	if (bEnabled != m_bEnabled)
	{
		m_bEnabled = bEnabled;
		if (m_pSliderCtrl && m_pSliderCtrl->GetSafeHwnd())
			m_pSliderCtrl->EnableWindow(bEnabled);

		DelayRedrawParent();
	}
}

void CXTPControlSlider::SetParent(CXTPCommandBar* pParent)
{
	CXTPControl::SetParent(pParent);

	if (!pParent && m_pSliderCtrl && m_pSliderCtrl->GetSafeHwnd())
	{
		m_pSliderCtrl->DestroyWindow();
	}
}

void CXTPControlSlider::SetHideFlags(DWORD dwFlags)
{
	if (m_dwHideFlags != dwFlags)
	{
		m_dwHideFlags = dwFlags;

		if (m_pSliderCtrl && m_pSliderCtrl->GetSafeHwnd())
			m_pSliderCtrl->SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | ((m_dwHideFlags != xtpNoHide) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW));
		DelayLayoutParent();
	}
}

void CXTPControlSlider::OnCalcDynamicSize(DWORD dwMode)
{
	if (m_pSliderCtrl->GetSafeHwnd())
	{
		if (dwMode & LM_VERTDOCK) m_pSliderCtrl->ModifyStyle(0, TBS_VERT);
		else m_pSliderCtrl->ModifyStyle(TBS_VERT, 0);
	}
}

void CXTPControlSlider::OnClick(BOOL bKeyboard, CPoint pt)
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

void CXTPControlSlider::OnThemeChanged()
{
	if (m_pSliderCtrl->GetSafeHwnd())
		m_pSliderCtrl->Invalidate(FALSE);
}




BEGIN_MESSAGE_MAP(CXTPControlSliderCtrl, CSliderCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()

	ON_NOTIFY_REFLECT(NM_RELEASEDCAPTURE, OnPositionChanged)

END_MESSAGE_MAP()

BOOL CXTPControlSliderCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

HBRUSH CXTPControlSliderCtrl::CtlColor ( CDC* pDC, UINT /*nCtlColor*/ )
{
	if (pDC->m_hAttribDC == NULL) pDC->m_hAttribDC = pDC->m_hDC;

	CXTPWindowRect rcOrig(this);
	GetParent()->ScreenToClient(&rcOrig);

	CPoint pt = pDC->SetViewportOrg(-rcOrig.TopLeft());
	((CXTPCommandBar*)GetParent())->DrawCommandBar(pDC, rcOrig);
	pDC->SetViewportOrg(pt);

	SendMessage(TBM_CLEARSEL, 0, 0); // minor hacking. don't ask me why
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void CXTPControlSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pControl->IsCustomizeMode())
	{
		if (m_pControl->GetParent()->IsCustomizable() && ((m_pControl->GetFlags() & xtpFlagNoMovable) == 0))
			m_pControl->OnClick();
	}
	else CSliderCtrl::OnLButtonDown(nFlags, point);
}

void CXTPControlSliderCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pControl->IsCustomizeMode())
	{
		ClientToScreen(&point);
		m_pControl->GetParent()->ScreenToClient(&point);

		m_pControl->GetParent()->OnRButtonDown(nFlags, point);
		return;
	}


	CSliderCtrl::OnRButtonDown(nFlags, point);
}


void CXTPControlSliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CSliderCtrl::OnMouseMove(nFlags, point);

	MapWindowPoints(m_pControl->GetParent(), &point, 1);
	m_pControl->GetParent()->OnMouseMove(nFlags, point);
}

void CXTPControlSliderCtrl::OnPositionChanged (NMHDR * /*pNotifyStruct*/, LRESULT* /*result*/)
{
	m_pControl->m_nPos = GetPos();
	m_pControl->OnExecute();
}
