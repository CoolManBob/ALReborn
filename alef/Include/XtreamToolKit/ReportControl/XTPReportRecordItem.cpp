// XTPReportRecordItem.cpp : implementation of the CXTPReportRecordItem class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
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

#include "XTPReportRow.h"
#include "XTPReportRecords.h"
#include "XTPReportColumns.h"
#include "XTPReportColumn.h"
#include "XTPReportControl.h"
#include "XTPReportPaintManager.h"
#include "XTPReportHyperlink.h"
#include "XTPReportRecord.h"
#include "XTPReportRecordItem.h"
#include "XTPReportInplaceControls.h"

#include "Common/XTPPropExchange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

//////////////////////////////////////////////////////////////////////////
// XTP_REPORTRECORDITEM_ARGS

XTP_REPORTRECORDITEM_ARGS::XTP_REPORTRECORDITEM_ARGS()
{
	pControl = NULL;
	pRow = NULL;
	pColumn = NULL;
	pItem = NULL;
	rcItem.SetRectEmpty();
}

XTP_REPORTRECORDITEM_ARGS::XTP_REPORTRECORDITEM_ARGS(CXTPReportControl* pControl, CXTPReportRow* pRow, CXTPReportColumn* pColumn)
{
	ASSERT(pControl && pRow && pColumn);

	this->pControl = pControl;
	this->pRow = pRow;
	this->pColumn = pColumn;
	this->pItem = pRow->GetRecord()->GetItem(pColumn);
	this->rcItem = pRow->GetItemRect(pItem);
}
XTP_REPORTRECORDITEM_ARGS::~XTP_REPORTRECORDITEM_ARGS()
{

}

CXTPReportInplaceButtons::CXTPReportInplaceButtons()
{

}

//////////////////////////////////////////////////////////////////////////
// CXTPReportRecordItemConstraint

CXTPReportRecordItemConstraint::CXTPReportRecordItemConstraint()
{

	m_dwData = 0;
	m_nIndex = 0;
}

int CXTPReportRecordItemConstraint::GetIndex() const
{
	return m_nIndex;
}

//////////////////////////////////////////////////////////////////////////
// CXTPReportRecordItemConstraints

CXTPReportRecordItemConstraints::CXTPReportRecordItemConstraints()
{
}
CXTPReportRecordItemConstraints::~CXTPReportRecordItemConstraints()
{
	RemoveAll();
}


int CXTPReportRecordItemConstraints::GetCount() const
{
	return (int)m_arrConstraints.GetSize();
}

CXTPReportRecordItemConstraint* CXTPReportRecordItemConstraints::GetAt(int nIndex) const
{
	return m_arrConstraints.GetAt(nIndex);
}

void CXTPReportRecordItemConstraints::RemoveAll()
{
	for (int i = 0; i < GetCount(); i++)
		m_arrConstraints[i]->InternalRelease();
	m_arrConstraints.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
// CXTPReportRecordItemEditOptions

CXTPReportRecordItemEditOptions::CXTPReportRecordItemEditOptions()
{
	m_bAllowEdit = TRUE;
	m_bConstraintEdit = FALSE;
	m_pConstraints = new CXTPReportRecordItemConstraints();
	m_bSelectTextOnEdit = FALSE;
	m_dwEditStyle = ES_AUTOHSCROLL;
	m_nMaxLength = 0;


}

CXTPReportRecordItemEditOptions::~CXTPReportRecordItemEditOptions()
{
	RemoveButtons();

	CMDTARGET_RELEASE(m_pConstraints);
}

void CXTPReportRecordItemEditOptions::RemoveButtons()
{
	for (int j = 0; j < arrInplaceButtons.GetSize(); j++)
		arrInplaceButtons[j]->InternalRelease();

	arrInplaceButtons.RemoveAll();
}

void CXTPReportRecordItemEditOptions::AddComboButton()
{
	arrInplaceButtons.Add(new CXTPReportInplaceButton(XTP_ID_REPORT_COMBOBUTTON));
}

void CXTPReportRecordItemEditOptions::AddExpandButton()
{
	arrInplaceButtons.Add(new CXTPReportInplaceButton(XTP_ID_REPORT_EXPANDBUTTON));
}

CXTPReportRecordItemConstraint* CXTPReportRecordItemEditOptions::FindConstraint(DWORD_PTR dwData)
{
	for (int i = 0; i < m_pConstraints->GetCount(); i++)
	{
		CXTPReportRecordItemConstraint* pConstaint = m_pConstraints->GetAt(i);
		if (pConstaint->m_dwData == dwData)
			return pConstaint;
	}
	return NULL;
}

CXTPReportRecordItemConstraint* CXTPReportRecordItemEditOptions::FindConstraint(LPCTSTR lpszConstraint)
{
	for (int i = 0; i < m_pConstraints->GetCount(); i++)
	{
		CXTPReportRecordItemConstraint* pConstaint = m_pConstraints->GetAt(i);
		if (pConstaint->m_strConstraint == lpszConstraint)
			return pConstaint;
	}
	return NULL;
}

CXTPReportRecordItemConstraint* CXTPReportRecordItemEditOptions::AddConstraint(LPCTSTR lpszConstraint, DWORD_PTR dwData /*= 0*/)
{
	CXTPReportRecordItemConstraint* pConstaint = new CXTPReportRecordItemConstraint();

	pConstaint->m_strConstraint = lpszConstraint;
	pConstaint->m_dwData = dwData;
	pConstaint->m_nIndex = (int)m_pConstraints->m_arrConstraints.Add(pConstaint);

	return pConstaint;
}

//////////////////////////////////////////////////////////////////////////
// CXTPReportRecordItem

IMPLEMENT_SERIAL(CXTPReportRecordItem, CCmdTarget, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

CXTPReportRecordItem::CXTPReportRecordItem()
{
	static const CString g_strFormatStringDef = _T("%s");

	m_strFormatString = g_strFormatStringDef;
	m_bEditable = TRUE;
	m_pFontCaption = NULL;
	m_bBoldText = FALSE;
	m_clrText = XTP_REPORT_COLOR_DEFAULT;
	m_clrBackground = XTP_REPORT_COLOR_DEFAULT;

	m_nIconIndex = XTP_REPORT_NOICON;

	m_pRecord = 0;
	m_dwData = 0;

	m_nSortPriority = -1;
	m_nGroupPriority = -1;

	m_bChecked = FALSE;
	m_bHasCheckbox = FALSE;
	m_pEditOptions = NULL;

	m_bFocusable = TRUE;

	m_pHyperlinks = NULL;


}


CXTPReportRecordItem::~CXTPReportRecordItem()
{
	CMDTARGET_RELEASE(m_pEditOptions);
	CMDTARGET_RELEASE(m_pHyperlinks);
}

int CXTPReportRecordItem::AddHyperlink(CXTPReportHyperlink* pHyperlink)
{
	return GetHyperlinks() ? (int)GetHyperlinks()->Add(pHyperlink) : -1;
}

int CXTPReportRecordItem::GetHyperlinksCount() const
{
	return m_pHyperlinks ? (int)m_pHyperlinks->GetSize() : 0;
}

CXTPReportHyperlink* CXTPReportRecordItem::GetHyperlinkAt(int nHyperlink) const
{
	return m_pHyperlinks ? m_pHyperlinks->GetAt(nHyperlink) : NULL;
}

void CXTPReportRecordItem::RemoveHyperlinkAt(int nHyperlink)
{
	if (m_pHyperlinks)
		m_pHyperlinks->RemoveAt(nHyperlink);
}

void CXTPReportRecordItem::DoMouseButtonClick()
{
	mouse_event(GetSystemMetrics(SM_SWAPBUTTON) ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(GetSystemMetrics(SM_SWAPBUTTON) ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void CXTPReportRecordItem::GetCaptionRect(XTP_REPORTRECORDITEM_ARGS* pDrawArgs, CRect& rcItem)
{
	ASSERT(pDrawArgs->pControl);

	if (GetIconIndex() != XTP_REPORT_NOICON)
		rcItem.left += pDrawArgs->pControl->GetPaintManager()->DrawBitmap(NULL, pDrawArgs->pControl, rcItem, GetIconIndex());

	if (m_bHasCheckbox)
		rcItem.left += 15;
}

void CXTPReportRecordItem::DrawCheckBox(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, CRect& rcItem)
{
	BOOL bEditable = IsEditable() && (pDrawArgs->pColumn == NULL || pDrawArgs->pColumn->IsEditable());

	CDC* pDC = pDrawArgs->pDC;
	int eState = (bEditable ? 0: 2) + (IsChecked() ? 1 : 0);

	CXTPReportPaintManager* pPaintManager = pDrawArgs->pControl->GetPaintManager();

	int nIconAlign = pDrawArgs->pColumn ?
		pDrawArgs->pColumn->GetAlignment() & xtpColumnIconMask : 0;

	m_rcGlyph = rcItem;
	int nGlyphWidth = pPaintManager->DrawGlyph(NULL, m_rcGlyph, eState + 2);

	switch(nIconAlign)
	{
	case xtpColumnIconRight:
		m_rcGlyph.left = rcItem.right - nGlyphWidth - 2;
		// shift text box to the left
		rcItem.right -= (2 + pPaintManager->DrawGlyph(pDC, m_rcGlyph, eState + 2));
		break;
	case xtpColumnIconCenter:
		m_rcGlyph.left = rcItem.CenterPoint().x - nGlyphWidth / 2 - 1;
		m_rcGlyph.right = m_rcGlyph.left + pPaintManager->DrawGlyph(pDC, m_rcGlyph, eState + 2);
		break; // left text box as is - draw on image...
	case xtpColumnIconLeft:
	default:
		m_rcGlyph.left += 2;
		// shift text box to the right
		rcItem.left += 2 + pPaintManager->DrawGlyph(pDC, m_rcGlyph, eState + 2);
		m_rcGlyph.right = rcItem.left - 1;
		break;
	}
}

void CXTPReportRecordItem::OnDrawCaption(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pMetrics)
{
	ASSERT(pDrawArgs->pItem == this);
	pDrawArgs->pControl->GetPaintManager()->DrawItemCaption(pDrawArgs, pMetrics);
}


int CXTPReportRecordItem::Draw(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs)
{
	CDC* pDC = pDrawArgs->pDC;
	CRect& rcItem = pDrawArgs->rcItem;
	CXTPReportPaintManager* pPaintManager = pDrawArgs->pControl->GetPaintManager();

	CRgn rgn;
	rgn.CreateRectRgn(rcItem.left, rcItem.top - 1, rcItem.right, rcItem.bottom);

	if (!pDC->IsPrinting())
		pDC->SelectClipRgn(&rgn);

	XTP_REPORTRECORDITEM_METRICS* pMetrics = new XTP_REPORTRECORDITEM_METRICS();
	pMetrics->strText = GetCaption(pDrawArgs->pColumn);
	pDrawArgs->pRow->GetItemMetrics(pDrawArgs, pMetrics);

	ASSERT(pMetrics->pFont);
	ASSERT(pMetrics->clrForeground != XTP_REPORT_COLOR_DEFAULT);

	if (pMetrics->clrBackground != XTP_REPORT_COLOR_DEFAULT)
		pDC->FillSolidRect(rcItem, pMetrics->clrBackground);

	int nItemGlyphs = rcItem.left;

	// draw tree inside item rect (see also HitTest function)
	if (pDrawArgs->pColumn && pDrawArgs->pColumn->IsTreeColumn())
	{
		int nTreeDepth = pDrawArgs->pRow->GetTreeDepth() - pDrawArgs->pRow->GetGroupLevel();
		if (nTreeDepth > 0)
			nTreeDepth++;
		rcItem.left += pDrawArgs->pControl->GetIndent(nTreeDepth);

		BOOL bHasChildren = pDrawArgs->pRow->HasChildren();

		pPaintManager->DrawTreeStructure(pDrawArgs, pMetrics, rcItem);

		CRect rcBitmap(rcItem);
		int nBitmapWidth = pPaintManager->DrawCollapsedBitmap(bHasChildren ? pDC : NULL, pDrawArgs->pRow, rcBitmap);

		if (!pDC->IsPrinting() && bHasChildren)
			pDrawArgs->pRow->SetCollapseRect(rcBitmap);

		rcItem.left += nBitmapWidth + 1;
	}

	pDC->SetTextColor(pMetrics->clrForeground);

	CFont* pOldFont = (CFont*)pDC->SelectObject(pMetrics->pFont);

	if (pMetrics->clrBackground != XTP_REPORT_COLOR_DEFAULT)
		pDC->SetBkColor(pMetrics->clrBackground);
	else
		pDC->SetBkColor(pPaintManager->m_clrControlBack);

	if (m_bHasCheckbox)
		DrawCheckBox(pDrawArgs, rcItem);

	// Do the draw bitmap pDC, rcItem, GetIconIndex()
	if (GetIconIndex() != XTP_REPORT_NOICON)
	{
		pPaintManager->DrawItemBitmap(pDrawArgs, rcItem, GetIconIndex());
	}

	nItemGlyphs = (rcItem.left - nItemGlyphs);

	OnDrawCaption(pDrawArgs, pMetrics);

	int nItemTextWidth = nItemGlyphs + pDC->GetTextExtent(pMetrics->strText).cx + 4;

	pDC->SelectObject(pOldFont);

	pMetrics->InternalRelease();

	if (!pDC->IsPrinting())
		pDC->SelectClipRgn(NULL);

	return nItemTextWidth;
}


void CXTPReportRecordItem::OnClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs)
{
	InternalAddRef();
	CMDTARGET_ADDREF(pClickArgs->pColumn);
	CMDTARGET_ADDREF(pClickArgs->pControl);

	CXTPReportRow* pRow = pClickArgs->pRow;
	CMDTARGET_ADDREF(pRow);

	BOOL bCheckBoxClicked = FALSE;
	CXTPReportControl* pControl = pClickArgs->pControl;

	if (IsEditable() && m_bHasCheckbox && (pClickArgs->pColumn == NULL || pClickArgs->pColumn->IsEditable())
		&& OnRequestEdit(pClickArgs))
	{
		// adjust vertical coordinates if virtual mode
		if (pControl->IsVirtualMode())
		{
			m_rcGlyph.top = pClickArgs->rcItem.top;
			m_rcGlyph.bottom = pClickArgs->rcItem.bottom;
		}
		// check whether we're clicking glyph area
		if (m_rcGlyph.PtInRect(pClickArgs->ptClient))
		{
			if (pControl->IsAutoCheckItems()) SetChecked(!IsChecked());
			pControl->RedrawControl();

			pControl->SendMessageToParent(pClickArgs->pRow, this, pClickArgs->pColumn, XTP_NM_REPORT_CHECKED, &pClickArgs->ptClient);
			bCheckBoxClicked = TRUE;
		}
	}

	if (!bCheckBoxClicked && IsAllowEdit(pClickArgs) && pControl->IsEditOnClick())
	{
		pControl->EditItem(pClickArgs);

		CPoint pt(pClickArgs->ptClient);
		pControl->ClientToScreen(&pt);

		CXTPReportInplaceEdit* pEdit = DYNAMIC_DOWNCAST(CXTPReportInplaceEdit, CWnd::FromHandle(WindowFromPoint(pt)));
		if (pEdit && pEdit->GetItem() == this)
		{
			CXTPReportRecordItemEditOptions* pEditOptions = GetEditOptions(pClickArgs->pColumn);

			if (pEditOptions->m_bSelectTextOnEdit)
			{
				pEdit->SetSel(0, -1);
			}
			else
			{
				DoMouseButtonClick();
			}
		}
	}

	pClickArgs->pControl->SendMessageToParent(pClickArgs->pRow, this, pClickArgs->pColumn, NM_CLICK, &pClickArgs->ptClient);

	// Determine Hyperlink Click
	int nHyperlink = HitTestHyperlink(pClickArgs->ptClient);
	if (nHyperlink >= 0)
	{
		pClickArgs->pControl->SendMessageToParent(pClickArgs->pRow, this, pClickArgs->pColumn, XTP_NM_REPORT_HYPERLINK, &pClickArgs->ptClient, nHyperlink);
	}

	CMDTARGET_RELEASE(pRow);
	CMDTARGET_RELEASE(pClickArgs->pColumn);
	CMDTARGET_RELEASE(pClickArgs->pControl);
	InternalRelease();
}

int CXTPReportRecordItem::HitTestHyperlink(CPoint ptClick)
{
	int nHyperlinks = GetHyperlinksCount();
	CXTPReportHyperlink* pHyperlink;
	for (int nHyperlink = 0; nHyperlink < nHyperlinks; nHyperlink++)
	{
		pHyperlink = GetHyperlinkAt(nHyperlink);
		ASSERT(pHyperlink);
		if (pHyperlink->m_rcHyperSpot.PtInRect(ptClick))
			return nHyperlink;
	}
	return -1;
}

void CXTPReportRecordItem::OnDblClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs)
{
	CXTPReportControl* pControl = pClickArgs->pControl;

	if (IsAllowEdit(pClickArgs) && !pControl->IsEditOnClick() && pControl->GetInplaceEdit()->GetItem() != this)
	{
		pControl->EditItem(pClickArgs);

		CPoint pt(pClickArgs->ptClient);
		pControl->ClientToScreen(&pt);

		CXTPReportInplaceEdit* pEdit = DYNAMIC_DOWNCAST(CXTPReportInplaceEdit, CWnd::FromHandle(WindowFromPoint(pt)));
		if (pEdit && pEdit->GetItem() == this)
		{
			CXTPReportRecordItemEditOptions* pEditOptions = GetEditOptions(pClickArgs->pColumn);

			if (pEditOptions->m_bSelectTextOnEdit)
			{
				pEdit->SetSel(0, -1);
			}
			else
			{
				DoMouseButtonClick();
			}
		}
	}

	pControl->SendMessageToParent(pClickArgs->pRow, this, pClickArgs->pColumn, NM_DBLCLK, &pClickArgs->ptClient, -1);
}

void CXTPReportRecordItem::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	int nHyperlink = HitTestHyperlink(point);
	if (nHyperlink >= 0)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	}
}

void CXTPReportRecordItem::SetFont(CFont* pFont)
{
	m_pFontCaption = pFont;
}

void CXTPReportRecordItem::SetBold(BOOL bBold)
{
	m_bBoldText = bBold;
}

void CXTPReportRecordItem::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
}

void CXTPReportRecordItem::SetBackgroundColor(COLORREF clrBackground)
{
	m_clrBackground = clrBackground;
}

int CXTPReportRecordItem::Compare(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem)
{
	if (!pItem)
		return 0;

	if (m_nSortPriority != -1 || pItem->m_nSortPriority != -1)
		return m_nSortPriority - pItem->m_nSortPriority;

	return m_pRecord->GetRecords()->Compare(GetCaption(pColumn), pItem->GetCaption(pColumn));
}

CFont* CXTPReportRecordItem::GetFont()
{
	return m_pFontCaption;
}

CString CXTPReportRecordItem::GetGroupCaption(CXTPReportColumn* pColumn)
{
	if (!m_strGroupCaption.IsEmpty())
		return m_strGroupCaption;

	int nID = GetGroupCaptionID(pColumn);

	if (nID > 0)
	{
		CString str;
		if (str.LoadString(nID))
		{
			return str;
		}
	}

	return pColumn->GetCaption() + _T(": ") + GetCaption(pColumn);
}

int CXTPReportRecordItem::CompareGroupCaption(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem)
{
	if (m_nGroupPriority != -1)
		return m_nGroupPriority - pItem->m_nGroupPriority;

	if (!m_strGroupCaption.IsEmpty())
		return m_pRecord->GetRecords()->Compare(m_strGroupCaption, pItem->m_strGroupCaption);

	int nID = GetGroupCaptionID(pColumn);

	if (nID > 0)
		return nID - pItem->GetGroupCaptionID(pColumn);

	return Compare(pColumn, pItem);
}

int CXTPReportRecordItem::GetGroupCaptionID(CXTPReportColumn* /*pColumn*/)
{
	return m_nGroupPriority;
}


void CXTPReportRecordItem::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	if (m_clrBackground != XTP_REPORT_COLOR_DEFAULT)
		pItemMetrics->clrBackground = m_clrBackground;

	if (m_clrText != XTP_REPORT_COLOR_DEFAULT)
		pItemMetrics->clrForeground = m_clrText;

	if (m_pFontCaption != NULL)
		pItemMetrics->pFont = m_pFontCaption;
	else if (m_bBoldText)
		pItemMetrics->pFont = &pDrawArgs->pControl->GetPaintManager()->m_fontBoldText;

}

int CXTPReportRecordItem::GetIndex() const
{
	return m_pRecord ? m_pRecord->IndexOf(this) : -1;
}

BOOL CXTPReportRecordItem::IsEditable() const
{
	if (this == NULL)
		return FALSE;

	ASSERT(m_pRecord);
	return m_pRecord ? m_pRecord->m_bEditable && m_bEditable && IsFocusable() : FALSE;
}


BOOL CXTPReportRecordItem::IsFocusable() const
{
	if (this == NULL)
		return FALSE;

	return m_bFocusable;
}

void CXTPReportRecordItem::OnBeginEdit(XTP_REPORTRECORDITEM_ARGS* pItemArgs)
{
	ASSERT(pItemArgs);

	CXTPReportControl* pControl = pItemArgs->pControl;
	CXTPReportInplaceEdit* pEdit = pControl->GetInplaceEdit();

	if (IsEditable() && OnRequestEdit(pItemArgs))
	{
		CXTPReportRecordItemEditOptions* pEditOptions = GetEditOptions(pItemArgs->pColumn);
		ASSERT(pEditOptions);

		CXTPReportInplaceButtons* pInpaceButtons = &pEditOptions->arrInplaceButtons;
		CRect rcButtons(pItemArgs->rcItem);

		for (int i = 0; i < pInpaceButtons->GetSize(); i++)
		{
			CXTPReportInplaceButton* pButton = pInpaceButtons->GetAt(i);
			pButton->Create(pItemArgs, rcButtons);

			pControl->GetInplaceButtons()->Add(pButton);
		}

		if (pEditOptions->m_bAllowEdit)
		{
			pEdit->Create(pItemArgs);
		}
		else if (pInpaceButtons->GetSize() > 0)
		{
			OnInplaceButtonDown(pInpaceButtons->GetAt(0));
		}
	}
}

void CXTPReportRecordItem::OnCancelEdit(CXTPReportControl* pControl, BOOL bApply)
{
	CXTPReportInplaceButtons* pInpaceButtons = pControl->GetInplaceButtons();

	for (int i = 0; i < pInpaceButtons->GetSize(); i++)
	{
		CXTPReportInplaceButton* pButton = pInpaceButtons->GetAt(i);
		pButton->DestroyWindow();
		pButton->SetItemArgs(0);
	}
	pInpaceButtons->RemoveAll();

	CXTPReportInplaceList* pInpaceList = pControl->GetInplaceList();
	if (pInpaceList->GetSafeHwnd())
	{
		pInpaceList->DestroyWindow();
	}


	CXTPReportInplaceEdit* pEdit = pControl->GetInplaceEdit();

	if (bApply && pEdit->GetSafeHwnd() && pEdit->GetItem() == this)
	{
		OnValidateEdit((XTP_REPORTRECORDITEM_ARGS*)pEdit);
	}

	pEdit->HideWindow();
	pEdit->SetItemArgs(0);
}

void CXTPReportRecordItem::OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraint* pConstraint)
{
	OnEditChanged(pItemArgs, pConstraint->m_strConstraint);
}

void CXTPReportRecordItem::OnValidateEdit(XTP_REPORTRECORDITEM_ARGS* pItemArgs)
{
	ASSERT(pItemArgs);
	if (!pItemArgs)
		return;

	CXTPReportControl* pControl = pItemArgs->pControl;
	CXTPReportInplaceEdit* pEdit = pControl->GetInplaceEdit();
	XTP_REPORTRECORDITEM_ARGS args = *pItemArgs;

	if (pEdit->GetSafeHwnd() && pEdit->GetItem() == this)
	{
		BOOL bRedraw = FALSE;
		pEdit->pItem = NULL;

		if (GetEditOptions(args.pColumn)->m_bConstraintEdit)
		{
			if (pEdit->m_pSelectedConstraint)
			{
				OnConstraintChanged(&args, pEdit->m_pSelectedConstraint);
				bRedraw = TRUE;
			}
		}
		else
		{
			CString strValue;
			pEdit->GetWindowText(strValue);

			if (GetCaption(args.pColumn) != strValue)
			{
				OnEditChanged(&args, strValue);
				bRedraw = TRUE;
			}
		}
		if (bRedraw)
		{
			pControl->RedrawControl();
			pControl->SendMessageToParent(args.pRow, this, args.pColumn, XTP_NM_REPORT_VALUECHANGED, 0);
		}
		else
		{
			OnEditCanceled(pItemArgs);
			pControl->SendMessageToParent(args.pRow, this, args.pColumn, XTP_NM_REPORT_EDIT_CANCELED, 0);
		}

		pEdit->SetItemArgs(0);
	}
}

BOOL CXTPReportRecordItem::IsAllowEdit(XTP_REPORTRECORDITEM_ARGS* pItemArgs)
{
	return pItemArgs->pControl->IsAllowEdit() && IsEditable() &&
		((pItemArgs->pColumn && pItemArgs->pColumn->IsEditable()) || (!pItemArgs->pColumn && m_pEditOptions &&
		m_pEditOptions->m_bAllowEdit));
}

BOOL CXTPReportRecordItem::OnRequestEdit(XTP_REPORTRECORDITEM_ARGS* pItemArgs)
{
	CXTPReportControl* pControl = pItemArgs->pControl;

	XTP_NM_REPORTREQUESTEDIT nm;
	::ZeroMemory(&nm, sizeof(nm));

	nm.bCancel = FALSE;
	nm.pItem = this;
	nm.pRow = pItemArgs->pRow;
	nm.pColumn = pItemArgs->pColumn;

	pControl->SendNotifyMessage(XTP_NM_REPORT_REQUESTEDIT, (NMHDR*)&nm);

	return !nm.bCancel;
}

BOOL CXTPReportRecordItem::OnChar(XTP_REPORTRECORDITEM_ARGS* pItemArgs, UINT nChar)
{
	CXTPReportControl* pControl = pItemArgs->pControl;

	if ((nChar == VK_SPACE) && IsEditable() && m_bHasCheckbox && (pItemArgs->pColumn == NULL || pItemArgs->pColumn->IsEditable())
		&& OnRequestEdit(pItemArgs))
	{
		if (pControl->IsAutoCheckItems()) SetChecked(!IsChecked());
		pControl->RedrawControl();

		pControl->SendMessageToParent(pItemArgs->pRow, this, pItemArgs->pColumn, XTP_NM_REPORT_CHECKED, NULL);
		return TRUE;
	}

	if (IsAllowEdit(pItemArgs))
	{
		pControl->EditItem(pItemArgs);

		CXTPReportInplaceEdit* pEdit = pControl->GetInplaceEdit();

		if (pEdit->GetSafeHwnd() && pEdit->GetItem() == this)
		{
			pEdit->SetFocus();
			pEdit->SetSel(0, -1);
			if (nChar != VK_TAB) pEdit->SendMessage(WM_CHAR, nChar);
		}
		return TRUE;

	}
	return FALSE;
}

CXTPReportRecordItemEditOptions* CXTPReportRecordItem::GetEditOptions(CXTPReportColumn* pColumn)
{
	if (m_pEditOptions)
		return m_pEditOptions;

	if (pColumn)
		return pColumn->GetEditOptions();

	return m_pEditOptions = new CXTPReportRecordItemEditOptions();
}

void CXTPReportRecordItem::OnInplaceButtonDown(CXTPReportInplaceButton* pButton)
{
	CXTPReportControl* pControl = pButton->pControl;

	XTP_NM_REPORTINPLACEBUTTON nm;
	::ZeroMemory(&nm, sizeof(nm));

	nm.pButton = pButton;
	nm.pItem = this;

	if (pControl->SendNotifyMessage(XTP_NM_REPORT_INPLACEBUTTONDOWN, (NMHDR*)&nm) == TRUE)
		return;

	if (pButton->GetID() == XTP_ID_REPORT_COMBOBUTTON)
	{
		CXTPReportInplaceList* pList = pControl->GetInplaceList();

		XTP_REPORTRECORDITEM_ARGS itemArgs = *pButton;
		if (!itemArgs.pColumn && !itemArgs.pControl && !itemArgs.pItem && !itemArgs.pRow)
		{
			return;
		}
		ASSERT(itemArgs.pItem == this);

		CXTPWindowRect rcButton(pButton);
		pControl->ScreenToClient(&rcButton);
		itemArgs.rcItem.right = rcButton.right;
		itemArgs.rcItem.bottom  = rcButton.bottom;

		CXTPReportRecordItemEditOptions* pEditOptions = GetEditOptions(itemArgs.pColumn);

		if (pEditOptions->GetConstraints()->GetCount() > 0)
		{
			pList->Create(&itemArgs, pEditOptions->GetConstraints());
		}
	}
}

CXTPReportHyperlinks* CXTPReportRecordItem::GetHyperlinks()
{
	if (!m_pHyperlinks)
		m_pHyperlinks = new CXTPReportHyperlinks();

	return m_pHyperlinks;
}

void CXTPReportRecordItem::DoPropExchange(CXTPPropExchange* pPX)
{
	PX_DWord(pPX, _T("TextColor"), (DWORD&)m_clrText, XTP_REPORT_COLOR_DEFAULT);
	PX_DWord(pPX, _T("BackgroundColor"), (DWORD&)m_clrBackground, XTP_REPORT_COLOR_DEFAULT);

	PX_Bool(pPX, _T("BoldText"), m_bBoldText, FALSE);

	PX_String(pPX, _T("Format"), m_strFormatString, _T("%s"));

	PX_Bool(pPX, _T("Editable"), m_bEditable, TRUE);

	PX_Int(pPX, _T("IconIndex"), m_nIconIndex, XTP_REPORT_NOICON);

	PX_Int(pPX, _T("SortPriority"), m_nSortPriority, -1);
	PX_Int(pPX, _T("GroupPriority"), m_nGroupPriority, -1);

	PX_String(pPX, _T("GroupCaption"), m_strGroupCaption, _T(""));

	PX_Bool(pPX, _T("Focusable"), m_bFocusable, TRUE);

	PX_Bool(pPX, _T("Checked"), m_bChecked, FALSE);
	PX_Bool(pPX, _T("HasCheckbox"), m_bHasCheckbox, FALSE);

	ULONGLONG ullData = m_dwData;
	PX_UI8(pPX, _T("Data"), ullData, 0);
	m_dwData = (DWORD_PTR)ullData;

	PX_String(pPX, _T("Tooltip"), m_strTooltip, _T(""));

	if (pPX->GetSchema() > _XTP_SCHEMA_98)
	{
		PX_String(pPX, _T("Caption"), m_strCaption, _T(""));
	}


	// Hyperlinks
	CXTPPropExchangeSection secHyperlinks(pPX->GetSection(_T("Hyperlinks")));

	CXTPReportHyperlinks arHyperlinks;

	if (pPX->IsStoring() && m_pHyperlinks)
	{
		arHyperlinks.CopyFrom(m_pHyperlinks);
	}

	arHyperlinks.DoPropExchange(&secHyperlinks);

	if (pPX->IsLoading())
	{
		if (arHyperlinks.GetSize())
		{
			if (GetHyperlinks())
				GetHyperlinks()->CopyFrom(&arHyperlinks);
		}
		else if (m_pHyperlinks)
		{
			m_pHyperlinks->RemoveAll();
		}
	}
}

