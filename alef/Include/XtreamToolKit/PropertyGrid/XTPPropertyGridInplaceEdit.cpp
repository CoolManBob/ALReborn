// PropertyGripInplaceEdit.cpp : implementation of the CPropertyGripInplaceEdit class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
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

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGrid.h"
#include "XTPPropertyGridDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridInplaceEdit

CXTPPropertyGridInplaceEdit::CXTPPropertyGridInplaceEdit()
: m_pItem(0)
, m_pGrid(0)
{
	m_clrBack = 0;
	m_bDelayCreate = FALSE;
	m_bCreated = FALSE;
	m_bCancel = FALSE;
	m_bEditMode = FALSE;
}

CXTPPropertyGridInplaceEdit::~CXTPPropertyGridInplaceEdit()
{
}

IMPLEMENT_DYNAMIC(CXTPPropertyGridInplaceEdit, CEdit)

BEGIN_MESSAGE_MAP(CXTPPropertyGridInplaceEdit, CXTMaskEditT<CEdit>)
	ON_MASKEDIT_REFLECT()
	//{{AFX_MSG_MAP(CXTPPropertyGridInplaceEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnEnSetfocus)
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_CHAR()
	ON_WM_SYSKEYDOWN()
	ON_WM_STYLECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridInplaceEdit message handlers

void CXTPPropertyGridInplaceEdit::SetValue(LPCTSTR strValue)
{
	m_strValue = strValue;
}

void CXTPPropertyGridInplaceEdit::HideWindow()
{
	if (m_hWnd)
	{
		ShowWindow(SW_HIDE);
	}
}

void CXTPPropertyGridInplaceEdit::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	if (nStyleType == GWL_EXSTYLE && lpStyleStruct->styleNew & WS_EX_LAYOUTRTL)
	{
		HideWindow();
		m_bDelayCreate = TRUE;
	}

	CEdit::OnStyleChanged(nStyleType, lpStyleStruct);
}

void CXTPPropertyGridInplaceEdit::Create(CXTPPropertyGridItem* pItem, CRect rect)
{
	ASSERT(pItem && pItem->GetGrid());
	if (!pItem || !pItem->GetGrid())
		return;

	m_pGrid = pItem->GetGrid();
	m_pItem = pItem;
	m_bCreated = FALSE;
	m_bEditMode = FALSE;
	m_bCancel = FALSE;

	DWORD dwEditStyle = m_pItem->GetEditStyle();

	if (m_hWnd)
	{
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_RIGHT | ES_CENTER | ES_READONLY;

		if ((GetStyle() | dwStyle) != (dwEditStyle | dwStyle))
			m_bDelayCreate = TRUE;

		if (m_bDelayCreate)
			DestroyWindow();
	}

	if (!m_hWnd)
	{
		CEdit::Create(dwEditStyle, rect, m_pGrid, 0);
	}

	m_bDelayCreate = FALSE;

	if (m_pItem->m_bPassword)
		CEdit::SetPasswordChar(pItem->m_chPrompt);

	if (m_pItem->GetMetrics(TRUE, FALSE))
	{
		CEdit::SetLimitText(m_pItem->GetMetrics(TRUE, FALSE)->m_nMaxLength);
	}
	else
	{
		CEdit::SetLimitText(0);
	}

	SetFont(m_pGrid->GetPaintManager()->GetItemFont(pItem, TRUE));
	SetWindowText(m_strValue);
	SetWindowPos(0, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	SetMargins(3, 3);

	SetUseMask(m_pItem->m_bUseMask);
	if (m_pItem->m_bUseMask)
	{
		SetEditMask(m_pItem->m_strMask, m_pItem->m_strLiteral, m_pItem->m_strValue);
		SetPromptChar(m_pItem->m_chPrompt);
	}

	m_bCreated = TRUE;
}

HBRUSH CXTPPropertyGridInplaceEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	CXTPPropertyGridPaintManager* pPaintManager = m_pGrid ? m_pGrid->GetPaintManager() : 0;

	if (!m_pItem || !pPaintManager)
		return 0;

	pDC->SetTextColor(pPaintManager->GetItemTextColor(m_pItem, TRUE));

	COLORREF clr = pPaintManager->GetItemBackColor(m_pItem, TRUE);

	if (clr != m_clrBack || !m_brBack.GetSafeHandle())
	{
		m_brBack.DeleteObject();
		m_brBack.CreateSolidBrush(clr);
		m_clrBack = clr;

	}

	pDC->SetBkColor(m_clrBack);


	return m_brBack;
}

void CXTPPropertyGridInplaceEdit::OnEnSetfocus()
{
	if (!m_pGrid)
		return;

	m_pGrid->Invalidate(FALSE);


	if (m_pItem && !m_pItem->OnRequestEdit())
		m_pGrid->SetFocus();

	m_bEditMode = TRUE;
}

void CXTPPropertyGridInplaceEdit::OnEnKillfocus()
{
	if (!m_pGrid)
		return;

	if (m_pItem)
	{
		m_pItem->OnValidateEdit();
		if (m_pGrid) m_pGrid->Invalidate(FALSE);
	}

}

void CXTPPropertyGridInplaceEdit::OnEnChange()
{
	if (!m_bCreated)
		return;

	if (m_pItem && m_pGrid)
	{
		ASSERT(m_pItem);
		m_pGrid->SendNotifyMessage(XTP_PGN_EDIT_CHANGED, (LPARAM)this);
	}
}

UINT CXTPPropertyGridInplaceEdit::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

void CXTPPropertyGridInplaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_TAB) return;

	if ((nChar == VK_RETURN) && (GetStyle() & ES_WANTRETURN))
	{
		CXTMaskEditT<CEdit>::OnChar(nChar, nRepCnt, nFlags);
		return;
	}

	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		m_pGrid->SetFocus();
		return;
	}

	if (m_pItem && !m_pItem->GetReadOnly() && m_pItem->GetConstraintEdit())
	{
		CXTPPropertyGridItemConstraints* pList = m_pItem->GetConstraints();
		if (!pList->IsEmpty())
		{

			CString strWindowText;
			GetWindowText(strWindowText);
			int nIndex = pList->FindConstraint(strWindowText);
			int nIndexStart = nIndex == -1 ? pList->GetCount() - 1 : nIndex;

			CString strSeach ((TCHAR)nChar);

			do
			{

				nIndex = nIndex < pList->GetCount() - 1 ? nIndex + 1 : 0;
				CString str = pList->GetAt(nIndex);

				if (strSeach.CompareNoCase(str.Left(1)) == 0)
				{
					SetWindowText(str);
					pList->SetCurrent(nIndex);
					SetSel(0, -1);
					return;
				}

			}
			while (nIndex != nIndexStart);

			return;
		}
	}

	CXTMaskEditT<CEdit>::OnChar(nChar, nRepCnt, nFlags);
}

void CXTPPropertyGridInplaceEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_pItem)
		return;

	if (nChar == VK_TAB && m_pGrid)
	{
		((CXTPPropertyGrid*)m_pGrid->GetParent())->OnNavigate(
			xtpGridUIInplaceEdit, GetKeyState(VK_SHIFT) >= 0, m_pItem);
		return;
	}
	else if (nChar == VK_ESCAPE)
	{
		m_bCancel = TRUE;
		SetWindowText(m_strValue);
		return ;
	}
	else if (nChar == VK_RETURN)
	{
		return ;
	}
	else if (nChar == VK_F4)
	{
		CXTPPropertyGridInplaceButton* pButton = m_pItem->GetInplaceButtons()->Find(XTP_ID_PROPERTYGRID_COMBOBUTTON);
		if (pButton)
			m_pItem->OnInplaceButtonDown(pButton);
	}
	else if (nChar == VK_DOWN || nChar == VK_UP)
	{
		if (m_pItem != NULL && !m_pItem->GetReadOnly())
		{
			if (SelectConstraint(nChar == VK_DOWN ? +1 : -1, FALSE))
			{
				SetSel(0, -1);
				return ;
			}
		}
	}

	CXTMaskEditT<CEdit>::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CXTPPropertyGridInplaceEdit::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar == VK_DOWN || nChar == VK_UP) && m_pItem)
	{
		CXTPPropertyGridInplaceButton* pButton = m_pItem->GetInplaceButtons()->Find(XTP_ID_PROPERTYGRID_COMBOBUTTON);
		if (pButton)
			m_pItem->OnInplaceButtonDown(pButton);
	}

	CXTMaskEditT<CEdit>::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CXTPPropertyGridInplaceEdit::SelectConstraint(int nDirection, BOOL bCircle)
{
	CXTPPropertyGridItemConstraints* pList = m_pItem->GetConstraints();
	if (pList->IsEmpty())
		return FALSE;

	CString str;
	GetWindowText(str);
	int nIndex = pList->FindConstraint(str);

	if (nIndex == -1)
		return FALSE;

	nIndex += nDirection;
	if (nIndex >= pList->GetCount()) nIndex = bCircle ? 0 : (ULONG)pList->GetCount() - 1;
	if (nIndex < 0) nIndex = bCircle ? (ULONG)pList->GetCount() - 1 : 0;

	SetWindowText(pList->GetAt(nIndex));
	pList->SetCurrent(nIndex);

	return TRUE;
}

void CXTPPropertyGridInplaceEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_pGrid && m_pItem != NULL && !m_pItem->GetReadOnly() && SelectConstraint(+1, TRUE))
	{
		m_pGrid->SetFocus();
		return;
	}
	CXTMaskEditT<CEdit>::OnLButtonDblClk(nFlags, point);
}

void CXTPPropertyGridInplaceEdit::DestroyItem()
{
	// reset variables to defaults.
	m_pItem = NULL;
	m_pGrid = NULL;
	m_strValue.Empty();
	m_brBack.DeleteObject();

	// destroy the window.
	//DestroyWindow();
	if (::IsWindow(m_hWnd)) ShowWindow(SW_HIDE);
}


BOOL CXTPPropertyGridInplaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && IsDialogMessage(pMsg))
		return TRUE;

	return CXTMaskEditT<CEdit>::PreTranslateMessage(pMsg);
}


//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridInplaceControl

CXTPPropertyGridInplaceControl::CXTPPropertyGridInplaceControl(CXTPPropertyGridItem* pItem)
{
	m_pItem = pItem;
	m_nWidth = 0;
}

void CXTPPropertyGridInplaceControl::OnFinalRelease()
{
	if (m_hWnd != NULL)
		DestroyWindow();

	CCmdTarget::OnFinalRelease();
}

void CXTPPropertyGridInplaceControl::OnDestroyWindow()
{
	DestroyWindow();
}

void CXTPPropertyGridInplaceControl::OnValueChanged()
{

}

BEGIN_MESSAGE_MAP(CXTPPropertyGridInplaceControl, CWnd)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()


UINT CXTPPropertyGridInplaceControl::OnGetDlgCode()
{
	return CWnd::OnGetDlgCode() | DLGC_WANTTAB;
}

void CXTPPropertyGridInplaceControl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_TAB) return;

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CXTPPropertyGridInplaceControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_TAB && m_pItem)
	{
		((CXTPPropertyGrid*)m_pItem->GetGrid()->GetParent())->OnNavigate(
			xtpGridUIInplaceControl, GetKeyState(VK_SHIFT) >= 0, m_pItem);
		return;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridInplaceSlider

CXTPPropertyGridInplaceSlider::CXTPPropertyGridInplaceSlider(CXTPPropertyGridItem* pItem)
	: CXTPPropertyGridInplaceControl(pItem)
{
	m_nMin = 0;
	m_nMax = 100;

	m_nValue = 0;

	m_clrBack = COLORREF_NULL;


}

BEGIN_MESSAGE_MAP(CXTPPropertyGridInplaceSlider, CXTPPropertyGridInplaceControl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

int CXTPPropertyGridInplaceSlider::CalcualteEditWidth()
{
	if (m_nWidth != 0)
		return m_nWidth;

	CWindowDC dc(m_pItem->GetGrid());
	CXTPFontDC font (&dc, m_pItem->GetGrid()->GetFont());

	CString strMax;
	strMax.Format(_T("%i"), m_nMax);

	CString strMin;
	strMin.Format(_T("%i"), m_nMin);

	int nWidth = max(dc.GetTextExtent(strMax).cx, dc.GetTextExtent(strMin).cx) + 6;

	return nWidth;
}

void CXTPPropertyGridInplaceSlider::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW lpCustDraw = (LPNMCUSTOMDRAW)pNMHDR;

	if(lpCustDraw->dwDrawStage == CDDS_PREPAINT)
	{
		int nValue = GetSliderCtrl()->GetPos();
		if (nValue != m_nValue)
		{
			m_nValue = nValue;
			CString strValue;
			strValue.Format(_T("%i"), nValue);

			m_pItem->SetValue(strValue);
			m_pItem->OnValueChanged(strValue);

			m_pItem->GetGrid()->Invalidate(FALSE);
		}
	}

	*pResult = CDRF_DODEFAULT;
}

void CXTPPropertyGridInplaceSlider::OnCreateWindow(CRect& rcValue)
{
	CRect rcSlider(rcValue);

	if (m_nWidth > 0)
	{
		rcSlider.left = rcSlider.right - m_nWidth;

		if (rcSlider.left < rcValue.left)
			return;

		rcValue.right -= m_nWidth;
	}
	else
	{
		int nWidth = CalcualteEditWidth();
		rcSlider.left += nWidth;

		if (rcSlider.right < rcSlider.left)
			return;

		rcValue.right = rcValue.left + nWidth;
	}

	if (!m_hWnd)
	{
		VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_BAR_REG));

		Create(TRACKBAR_CLASS, NULL, WS_CHILD | TBS_HORZ | WS_TABSTOP, rcSlider, (CWnd*)m_pItem->GetGrid(), 0);
	}

	m_nValue = _ttol(m_pItem->GetValue());
	GetSliderCtrl()->SetRange(m_nMin, m_nMax);
	GetSliderCtrl()->SetPos(m_nValue);
	GetSliderCtrl()->EnableWindow(!m_pItem->GetReadOnly());

	MoveWindow(rcSlider);
	ShowWindow(SW_SHOW);
}

HBRUSH CXTPPropertyGridInplaceSlider::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	CXTPPropertyGridView* pGrid = m_pItem ? m_pItem->GetGrid() : 0;
	CXTPPropertyGridPaintManager* pPaintManager = pGrid ? pGrid->GetPaintManager() : 0;

	if (!m_pItem || !pPaintManager)
		return 0;

	pDC->SetTextColor(pPaintManager->GetItemTextColor(m_pItem, TRUE));

	COLORREF clr = pPaintManager->GetItemBackColor(m_pItem, TRUE);

	if (clr != m_clrBack || !m_brBack.GetSafeHandle())
	{
		m_brBack.DeleteObject();
		m_brBack.CreateSolidBrush(clr);
		m_clrBack = clr;
	}

	pDC->SetBkColor(m_clrBack);


	return m_brBack;
}

void CXTPPropertyGridInplaceSlider::OnAdjustValueRect(CRect& rcValue)
{
	if (!m_hWnd)
		return;

	if (m_nWidth > 0)
	{
		rcValue.right -= m_nWidth;
		return;
	}

	int nWidth = CalcualteEditWidth();

	if (rcValue.left + nWidth > rcValue.right)
		return;

	rcValue.right = rcValue.left + nWidth;
}

void CXTPPropertyGridInplaceSlider::OnValueChanged()
{
	int nValue = _ttol(m_pItem->GetValue());
	int nValueRange = max(m_nMin, min(m_nMax, nValue));

	if (nValueRange != m_nValue || nValueRange != nValue)
	{
		m_nValue = nValueRange;

		CString str;
		str.Format(_T("%i"), nValueRange);
		m_pItem->SetValue(str);

		if (m_hWnd) GetSliderCtrl()->SetPos(m_nValue);
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridInplaceSpinButton

CXTPPropertyGridInplaceSpinButton::CXTPPropertyGridInplaceSpinButton(CXTPPropertyGridItem* pItem)
	: CXTPPropertyGridInplaceControl(pItem)
{
	m_nMin = 0;
	m_nMax = 100;
	m_nWidth = GetSystemMetrics(SM_CXVSCROLL);


}

BEGIN_MESSAGE_MAP(CXTPPropertyGridInplaceSpinButton, CXTPPropertyGridInplaceControl)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
END_MESSAGE_MAP()

#define AFX_WNDCOMMCTL_UPDOWN_REG 0x00040

void CXTPPropertyGridInplaceSpinButton::OnCreateWindow(CRect& rcValue)
{
	CRect rcButton(rcValue);

	rcButton.left = rcButton.right - m_nWidth;
	rcValue.right -= m_nWidth;

	if (!m_hWnd)
	{
		VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_UPDOWN_REG));

		Create(UPDOWN_CLASS, NULL, WS_CHILD | UDS_ARROWKEYS, rcButton, (CWnd*)m_pItem->GetGrid(), 0);
	}

	int nValue = _ttol(m_pItem->GetValue());
	GetSpinButtonCtrl()->SetPos(nValue);
	GetSpinButtonCtrl()->SendMessage(UDM_SETRANGE32, (WPARAM)m_nMin, (LPARAM)m_nMax);
	GetSpinButtonCtrl()->EnableWindow(!m_pItem->GetReadOnly());

	MoveWindow(rcButton);
	ShowWindow(SW_SHOW);
}

void CXTPPropertyGridInplaceSpinButton::OnDeltapos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_pItem->OnValidateEdit();
	long nValue = _ttol(m_pItem->GetValue()) + pNMUpDown->iDelta;

	nValue = max(m_nMin, min(m_nMax, nValue));

	CString str;
	str.Format(_T("%i"), nValue);
	m_pItem->OnValueChanged(str);

	*pResult = 1;
}


void CXTPPropertyGridInplaceSpinButton::OnAdjustValueRect(CRect& rcValue)
{
	if (m_hWnd)
	{
		rcValue.right -= m_nWidth;
	}
}

void CXTPPropertyGridInplaceSpinButton::OnValueChanged()
{
	int nValue = _ttol(m_pItem->GetValue());
	int nValueRange = max(m_nMin, min(m_nMax, nValue));

	if (nValue != nValueRange)
	{
		CString str;
		str.Format(_T("%i"), nValueRange);

		m_pItem->SetValue(str);
	}
}
//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridInplaceControls


CXTPPropertyGridInplaceControls::CXTPPropertyGridInplaceControls()
{

}

CXTPPropertyGridInplaceControls::~CXTPPropertyGridInplaceControls()
{
	RemoveAll();
}

void CXTPPropertyGridInplaceControls::RemoveAll()
{
	for (int i = 0; i < (int)m_arrControls.GetSize(); i++)
	{
		m_arrControls[i]->InternalRelease();
	}
	m_arrControls.RemoveAll();
}

int CXTPPropertyGridInplaceControls::GetCount() const
{
	return (int)m_arrControls.GetSize();
}

void CXTPPropertyGridInplaceControls::Add(CXTPPropertyGridInplaceControl* pWindow)
{
	m_arrControls.Add(pWindow);
}

CXTPPropertyGridInplaceControl* CXTPPropertyGridInplaceControls::GetAt(int nIndex)
{
	return m_arrControls[nIndex];
}


