// XTPPropertyGridItem.cpp : implementation of the CXTPPropertyGridItem class.
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

#include "StdAfx.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGrid.h"
#include "XTPPropertyGridDefines.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItem

IMPLEMENT_DYNAMIC(CXTPPropertyGridItem, CCmdTarget)

CXTPPropertyGridItem::CXTPPropertyGridItem(LPCTSTR lpszCaption, LPCTSTR strValue, CString* pBindString)
{

	Init();
	SetPrompt(lpszCaption);
	m_strDefaultValue = m_strValue = strValue;

	BindToString(pBindString);
}


CXTPPropertyGridItem::CXTPPropertyGridItem(UINT nID, LPCTSTR strValue, CString* pBindString)
{
	Init();
	m_nID = nID;

	CString strPrompt;
	if (strPrompt.LoadString(nID))
	{
		SetPrompt(strPrompt);
	}

	m_strDefaultValue = m_strValue = strValue;
	BindToString(pBindString);
}

CXTPPropertyGridItem::~CXTPPropertyGridItem()
{
	Clear();
}

void CXTPPropertyGridItem::Clear()
{
	if (m_pGrid && m_pGrid->m_pSelected == this)
	{
		m_pGrid->m_pSelected = NULL;
	}

	if (m_pGrid && GetInplaceList().m_pItem == this)
	{
		GetInplaceList().DestroyItem();
	}
	if (m_pGrid && GetInplaceEdit().m_pItem == this)
	{
		GetInplaceEdit().DestroyItem();
	}

	if (m_pChilds)
	{
		m_pChilds->Clear();
		CMDTARGET_RELEASE(m_pChilds);
	}

	if (m_pConstraints)
	{
		m_pConstraints->m_pItem = NULL;
		CMDTARGET_RELEASE(m_pConstraints);
	}

	CMDTARGET_RELEASE(m_pValueMetrics);
	CMDTARGET_RELEASE(m_pCaptionMetrics);
	CMDTARGET_RELEASE(m_pInplaceButtons);

	SAFE_DELETE(m_pInplaceControls);

	m_pGrid = NULL;
}

void CXTPPropertyGridItem::SetPrompt(LPCTSTR lpszPrompt)
{
	if (lpszPrompt == 0)
		return;

	LPCTSTR lpszDescription = _tcschr(lpszPrompt, _T('\n'));

	if (lpszDescription == NULL)
	{
		m_strCaption = lpszPrompt;
	}
	else
	{
		int nLen = (int)(lpszDescription - lpszPrompt);
		LPTSTR lpszCaption = m_strCaption.GetBufferSetLength(nLen);

		MEMCPY_S(lpszCaption, lpszPrompt, nLen * sizeof(TCHAR));
		m_strDescription = lpszDescription + 1;
	}
}


void CXTPPropertyGridItem::Init()
{
	m_bHidden = FALSE;
	m_bExpanded = FALSE;
	m_nIndex = -1;
	m_bVisible = FALSE;
	m_pParent = 0;
	m_pGrid = 0;
	m_nIndent = 0;
	m_strValue = "";
	m_bReadOnly = FALSE;
	m_bCategory = FALSE;
	m_nID = 0;
	m_nFlags = xtpGridItemHasEdit;
	m_pConstraints = new CXTPPropertyGridItemConstraints(this);
	m_pInplaceButtons = new CXTPPropertyGridInplaceButtons(this);
	m_pInplaceControls = new CXTPPropertyGridInplaceControls();
	m_pConstraints->SetCurrent(-1);
	m_pBindString = 0;
	m_bConstraintEdit = FALSE;

	m_nLinesCount = 1;
	m_nHeight = -1;

	m_nDropDownItemCount = 10;


	m_bUseMask = FALSE;
	m_bPassword = FALSE;
	m_chPrompt = _T('*');

	m_dwData = 0;
	m_dwEditStyle = 0;

	EnableAutomation();

	m_pValueMetrics = 0;
	m_pCaptionMetrics = 0;


	m_pChilds = new CXTPPropertyGridItems();
}

void CXTPPropertyGridItem::OnAddChildItem()
{
	if (!m_bCategory) SetFlags(m_nFlags);
}

void CXTPPropertyGridItem::SetFlags(UINT nFlags)
{
	ASSERT(!m_bCategory);
	m_nFlags = nFlags;

	m_pInplaceButtons->Remove(XTP_ID_PROPERTYGRID_EXPANDBUTTON);
	m_pInplaceButtons->Remove(XTP_ID_PROPERTYGRID_COMBOBUTTON);

	if (m_nFlags & xtpGridItemHasExpandButton)
		m_pInplaceButtons->AddExpandButton();

	if (m_nFlags & xtpGridItemHasComboButton)
		m_pInplaceButtons->AddComboButton();
}

UINT CXTPPropertyGridItem::GetFlags() const
{
	return m_nFlags;
}

CXTPPropertyGridItemMetrics* CXTPPropertyGridItem::GetValueMetrics() const
{
	if (m_pValueMetrics == 0)
		m_pValueMetrics = new CXTPPropertyGridItemMetrics(m_pGrid->GetPaintManager());
	return m_pValueMetrics;
}

CXTPPropertyGridItemMetrics* CXTPPropertyGridItem::GetCaptionMetrics() const
{
	if (m_pCaptionMetrics == 0)
		m_pCaptionMetrics = new CXTPPropertyGridItemMetrics(m_pGrid->GetPaintManager());
	return m_pCaptionMetrics;
}

CXTPPropertyGridItemMetrics* CXTPPropertyGridItem::GetMetrics(BOOL bValueMetrics, BOOL bCreate /*= TRUE*/) const
{
	if (bCreate)
		return bValueMetrics ? GetValueMetrics() : GetCaptionMetrics();

	return bValueMetrics ? m_pValueMetrics : m_pCaptionMetrics;
}

CXTPPropertyGridItem* CXTPPropertyGridItem::AddChildItem(CXTPPropertyGridItem* pItem)
{
	return InsertChildItem(m_pChilds->GetCount(), pItem);
}

CXTPPropertyGridItem* CXTPPropertyGridItem::InsertChildItem(int nIndex, CXTPPropertyGridItem* pItem)
{
	ASSERT(pItem);
	if (!pItem)
		return NULL;

	ASSERT(!pItem->IsCategory() || IsCategory());
	ASSERT(m_pGrid != NULL);
	if (!m_pGrid)
		return pItem;

	if (nIndex < 0 || nIndex > m_pChilds->GetCount())
		nIndex = m_pChilds->GetCount();

	m_pChilds->InsertAt(nIndex, pItem);
	pItem->m_pGrid = m_pGrid;

	ASSERT(pItem->m_pParent == NULL);
	pItem->m_pParent = this;
	pItem->m_nIndent = m_nIndent + 1;

	pItem->OnAddChildItem();

	if (m_bVisible)
	{
		if (m_bExpanded) m_pGrid->SetPropertySort(m_pGrid->m_properetySort, TRUE);
		else if (m_pGrid->GetSafeHwnd() && m_pChilds->GetCount() == 1) m_pGrid->Invalidate(FALSE);
	}

	return pItem;
}

void CXTPPropertyGridItem::Collapse()
{
	if (!m_bExpanded) return;

	if (m_pGrid->m_nLockUpdate == 0)
	{
		if (m_pGrid->GetSafeHwnd() && m_bVisible)
		{
			m_pGrid->_DoCollapse(this);
			m_pGrid->OnSelectionChanged();
		}
	}

	m_bExpanded = FALSE;

	m_pGrid->SendNotifyMessage(XTP_PGN_ITEMEXPANDCHANGED, (LPARAM)this);
}

void CXTPPropertyGridItem::Expand()
{
	if (m_bExpanded) return;

	if (m_pGrid->m_nLockUpdate == 0)
	{
		if (m_pGrid->GetSafeHwnd() && m_bVisible)
		{
			m_pGrid->_DoExpand(this, m_nIndex);
			m_pGrid->_RefreshIndexes();
			m_pGrid->OnSelectionChanged();
		}
	}

	m_bExpanded = TRUE;

	m_pGrid->SendNotifyMessage(XTP_PGN_ITEMEXPANDCHANGED, (LPARAM)this);

}

BOOL CXTPPropertyGridItem::HasParent(CXTPPropertyGridItem* pParent)
{
	if (m_pParent == NULL) return FALSE;
	if (m_pParent == pParent) return TRUE;
	return m_pParent->HasParent(pParent);
}

void CXTPPropertyGridItem::Select()
{
	ASSERT(m_pGrid);
	if (!m_pGrid)
		return;

	if (m_bVisible)
	{
		m_pGrid->SetCurSel(m_nIndex);
		m_pGrid->OnSelectionChanged();
	}
}

void CXTPPropertyGridItem::OnDeselect()
{
	if (m_pGrid) m_pGrid->FocusInplaceButton(NULL);
	OnValidateEdit();

	for (int i = m_pInplaceControls->GetCount() - 1; i >= 0; i--)
	{
		CXTPPropertyGridInplaceControl* pControl = m_pInplaceControls->GetAt(i);
		pControl->OnDestroyWindow();
	}
}

void CXTPPropertyGridItem::OnValidateEdit()
{
	CXTPPropertyGridInplaceEdit& wndEdit = GetInplaceEdit();
	wndEdit.HideWindow();

	if (wndEdit.GetSafeHwnd() && wndEdit.m_pItem == this)
	{
		CString strValue;
		wndEdit.GetWindowText(strValue);
		wndEdit.m_pItem = NULL;

		if (wndEdit.m_bCancel)
		{
			OnCancelEdit();
		}
		else if (wndEdit.m_bEditMode && OnAfterEdit(strValue))
		{
			if (m_strValue != strValue && !wndEdit.m_bCancel)
			{
				OnValueChanged(strValue);
			}
		}
	}
}

void CXTPPropertyGridItem::SetEditText(const CString& str)
{
	if (!m_pGrid) return;

	CXTPPropertyGridInplaceEdit& wndEdit = GetInplaceEdit();
	if (wndEdit.GetSafeHwnd() && wndEdit.m_pItem == this)
	{
		wndEdit.SetWindowText(str);
	}
}

void CXTPPropertyGridItem::SetValue(CString strValue)
{
	m_strValue = strValue;

	if (m_pBindString)
	{
		*m_pBindString = strValue;
	}

	SetEditText(m_strValue);

	for (int i = m_pInplaceControls->GetCount() - 1; i >= 0; i--)
	{
		CXTPPropertyGridInplaceControl* pControl = m_pInplaceControls->GetAt(i);
		pControl->OnValueChanged();
	}

	SAFE_INVALIDATE(m_pGrid);
}

void CXTPPropertyGridItem::OnValueChanged(CString strValue)
{
	if (!m_pGrid)
		return;

	SetValue(strValue);
	SAFE_INVALIDATE(m_pGrid);
	m_pGrid->SendNotifyMessage(XTP_PGN_ITEMVALUE_CHANGED, (LPARAM)this);
}

BOOL CXTPPropertyGridItem::OnRequestEdit()
{
	XTP_PROPERTYGRID_NOTIFYEDIT notify;
	notify.bCancel = FALSE;
	notify.pItem = this;

	if (m_pGrid)
	{
		m_pGrid->SendNotifyMessage(XTP_PGN_REQUESTEDIT, (LPARAM)&notify);
	}
	return !notify.bCancel;
}

BOOL CXTPPropertyGridItem::OnAfterEdit(CString& strValue)
{
	if (!m_pGrid)
		return FALSE;

	XTP_PROPERTYGRID_NOTIFYEDIT notify;
	notify.bCancel = FALSE;
	notify.strValue = strValue;
	notify.pItem = this;

	if (m_pGrid)
	{
		m_pGrid->SendNotifyMessage(XTP_PGN_AFTEREDIT, (LPARAM)&notify);
		strValue = notify.strValue;
	}
	return !notify.bCancel;
}

void CXTPPropertyGridItem::OnCancelEdit()
{
	XTP_PROPERTYGRID_NOTIFYEDIT notify;
	notify.bCancel = FALSE;
	notify.pItem = this;

	if (m_pGrid)
	{
		m_pGrid->SendNotifyMessage(XTP_PGN_CANCELEDIT, (LPARAM)&notify);
	}
}


void CXTPPropertyGridItem::OnSelect()
{
	ASSERT(m_bVisible);

	CString strValue(m_strValue);

	if (m_pInplaceControls->GetCount() > 0)
	{
		CRect rcValue(GetInplaceWindowsRect());

		for (int i = m_pInplaceControls->GetCount() - 1; i >= 0; i--)
		{
			CXTPPropertyGridInplaceControl* pControl = m_pInplaceControls->GetAt(i);
			pControl->OnCreateWindow(rcValue);
		}
	}

	if (m_nFlags & xtpGridItemHasEdit)
	{
		CXTPPropertyGridInplaceEdit& wndEdit = GetInplaceEdit();

		wndEdit.SetValue(strValue);
		wndEdit.Create(this, GetValueRect());
		wndEdit.SetReadOnly(GetReadOnly() || m_bConstraintEdit);
	}
	else
	{
		GetInplaceEdit().HideWindow();
	}
}

DWORD CXTPPropertyGridItem::GetEditStyle() const
{
	return WS_CHILD | ES_AUTOHSCROLL | (m_bPassword ? ES_PASSWORD: 0) | (IsMultiLine() ? ES_MULTILINE : 0) | m_dwEditStyle;
}

void CXTPPropertyGridItem::OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton)
{
	if (GetReadOnly())
		return;

	if (m_pGrid->SendNotifyMessage(XTP_PGN_INPLACEBUTTONDOWN, (LPARAM)pButton) == TRUE)
		return;

	if ((pButton->GetID() == XTP_ID_PROPERTYGRID_COMBOBUTTON) && !m_pConstraints->IsEmpty() && OnRequestEdit())
	{
		GetInplaceList().Create(this, GetItemRect());
		m_pGrid->Invalidate(FALSE);
	}
}

CRect CXTPPropertyGridItem::GetInplaceWindowsRect()
{
	ASSERT(m_bVisible);
	CRect rc = GetItemRect();
	CRect rcValue(m_pGrid->GetDividerPos() + 1, rc.top + 1, rc.right, rc.bottom - 1);

	if (IsInplaceButtonsVisible())
	{
		for (int i = 0; i < m_pInplaceButtons->GetCount(); i++)
			rcValue.right -= m_pInplaceButtons->GetAt(i)->GetWidth();
	}

	m_pGrid->GetPaintManager()->AdjustItemValueRect(this, rcValue);

	return rcValue;
}

CRect CXTPPropertyGridItem::GetValueRect()
{
	CRect rcValue = GetInplaceWindowsRect();

	for (int i = m_pInplaceControls->GetCount() - 1; i >= 0; i--)
	{
		CXTPPropertyGridInplaceControl* pControl = m_pInplaceControls->GetAt(i);
		pControl->OnAdjustValueRect(rcValue);
	}

	return rcValue;
}

CRect CXTPPropertyGridItem::GetItemRect() const
{
	CRect rc;
	m_pGrid->GetItemRect(m_nIndex, rc);
	return rc;
}

BOOL CXTPPropertyGridItem::OnChar(UINT nChar)
{
	ASSERT(nChar != VK_TAB);

	if ((m_nFlags & xtpGridItemHasEdit) && (nChar != VK_ESCAPE))
	{
		OnSelect();

		CXTPPropertyGridInplaceEdit& wndEdit = GetInplaceEdit();
		if (wndEdit.GetSafeHwnd() && wndEdit.IsWindowVisible() && wndEdit.GetItem() == this)
		{
			wndEdit.SetFocus();
			wndEdit.SetSel(0, -1);
			if ((nChar != VK_TAB) && (wndEdit.GetItem() == this)) wndEdit.SendMessage(WM_CHAR, nChar);
		}

		return TRUE;
	}
	else if (nChar == VK_TAB)
	{
		if (m_pInplaceButtons->GetCount() > 0)
		{
			m_pGrid->GetPropertyGrid()->OnNavigate(xtpGridUIInplaceEdit, TRUE, this);
		}
	}
	return FALSE;
}

BOOL CXTPPropertyGridItem::IsValueChanged() const
{
	return m_strDefaultValue != m_strValue;
}

void CXTPPropertyGridItem::ResetValue()
{
	OnValueChanged(m_strDefaultValue);
}

void CXTPPropertyGridItem::SetDefaultValue(LPCTSTR lpszDefaultValue)
{
	m_strDefaultValue = lpszDefaultValue;
}

void CXTPPropertyGridItem::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{

}

BOOL CXTPPropertyGridItem::PtInValueRect(CPoint point)
{
	CRect rc(GetValueRect());
	return rc.PtInRect(point);
}

void CXTPPropertyGridItem::OnLButtonDblClk(UINT /*nFlags*/, CPoint point)
{
	CXTPPropertyGridInplaceButton* pInplaceButton = m_pInplaceButtons->HitTest(point);
	if (pInplaceButton && IsInplaceButtonsVisible())
	{
		OnSelect();
		return;
	}

	if (HasChilds())
	{
		if (m_bExpanded)
			Collapse();
		else
			Expand();
	}
	else
	{
		OnSelect();

		if (m_nFlags & xtpGridItemHasEdit)
		{
			CXTPPropertyGridInplaceEdit& wndEdit = GetInplaceEdit();
			if (wndEdit.GetSafeHwnd() && wndEdit.IsWindowVisible() && wndEdit.GetItem() == this)
			{
				wndEdit.SetFocus();

				if (wndEdit.GetItem() == this && !GetReadOnly() && wndEdit.SelectConstraint(+1, TRUE))
				{
					OnValidateEdit();

				}
				else
				{
					wndEdit.SetSel(0, -1);
				}
			}
		}
		else if (!GetReadOnly())
		{
			SelectNextConstraint();
		}
	}
}

BOOL CXTPPropertyGridItem::SelectNextConstraint()
{
	CXTPPropertyGridItemConstraints* pList = GetConstraints();
	if (pList->IsEmpty())
		return FALSE;

	if (!OnRequestEdit())
		return FALSE;

	int nIndex = pList->FindConstraint(m_strValue);

	nIndex += +1;
	if (nIndex >= pList->GetCount()) nIndex = 0;
	if (nIndex < 0) nIndex = (ULONG)pList->GetCount() - 1;

	CString strValue = pList->GetAt(nIndex);

	if (!OnAfterEdit(strValue))
		return FALSE;

	pList->SetCurrent(nIndex);
	OnValueChanged(strValue);
	return TRUE;
}

BOOL CXTPPropertyGridItem::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL bOldSelected = IsSelected();
	Select();

	CXTPPropertyGridInplaceButton* pInplaceButton = m_pInplaceButtons->HitTest(point);
	if (pInplaceButton && IsInplaceButtonsVisible())
	{
		if (!bOldSelected && !m_pGrid->GetPropertyGrid()->GetShowInplaceButtonsAlways())
			return TRUE;
		pInplaceButton->OnLButtonDown(nFlags, point);
		return TRUE;
	}

	int nOffset = (m_nIndent - (IsCategory() ? 0 : 1)) * XTP_PGI_EXPAND_BORDER;

	if (point.x >= nOffset && point.x <= XTP_PGI_EXPAND_BORDER + nOffset)
	{
		if (m_bExpanded)
			Collapse();
		else
			Expand();
	}

	if ((m_nFlags & xtpGridItemHasEdit) && PtInValueRect(point))
	{
		mouse_event(GetSystemMetrics(SM_SWAPBUTTON) ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	}

	return TRUE;
}

void CXTPPropertyGridItem::BindToString(CString* pBindString)
{
	m_pBindString = pBindString;
	if (m_pBindString)
	{
		*m_pBindString = m_strValue;
	}
}

void CXTPPropertyGridItem::OnBeforeInsert()
{
	if (m_pBindString && *m_pBindString != m_strValue)
	{
		m_strValue = *m_pBindString;
	}
}

void CXTPPropertyGridItem::EnsureVisible()
{
	if (!m_bVisible)
	{
		CXTPPropertyGridItem* pItem = GetParentItem();

		while (pItem)
		{
			pItem->Expand();
			pItem = pItem->GetParentItem();
		}
	}

	if (!m_bVisible || (m_nIndex == -1))
		return;

	if (m_nIndex < m_pGrid->GetTopIndex())
	{
		m_pGrid->SetTopIndex(m_nIndex);
		return;
	}

	int top = 0;
	int nTopIndex = m_pGrid->GetTopIndex();
	int bottom = CXTPClientRect(m_pGrid).bottom;

	for (int i = nTopIndex; i < m_pGrid->GetCount(); i++)
	{
		int nRowHeight = m_pGrid->GetItemHeight(i);

		if (top + nRowHeight > bottom)
			break;

		if (i == m_nIndex)
			return;

		top += nRowHeight;
	}

	int nHeight = bottom;
	for (top = m_nIndex; top >= 0; top--)
	{
		int nRowHeight = m_pGrid->GetItemHeight(top);

		if (nHeight - nRowHeight < 0)
		{
			if (top != m_nIndex)
				top++;
			break;
		}

		nHeight -= nRowHeight;
	}
	m_pGrid->SetTopIndex(top);
}

CXTPPropertyGridInplaceEdit& CXTPPropertyGridItem::GetInplaceEdit()
{
	if (m_pGrid) return *m_pGrid->m_pGrid->m_pInplaceEdit;

	static CXTPPropertyGridInplaceEdit  wndEdit;
	return wndEdit;
}

CXTPPropertyGridInplaceList& CXTPPropertyGridItem::GetInplaceList()
{
	if (m_pGrid) return *m_pGrid->m_pGrid->m_pInplaceListBox;

	static CXTPPropertyGridInplaceList  wndListBox;
	return wndListBox;
}

BOOL CXTPPropertyGridItem::IsInplaceButtonsVisible() const
{
	if (!m_pGrid)
		return FALSE;

	if (!IsSelected() && !m_pGrid->GetPropertyGrid()->GetShowInplaceButtonsAlways())
		return FALSE;

	return !GetReadOnly() && m_pInplaceButtons->GetCount() > 0;
}

CXTPPropertyGridInplaceButtons* CXTPPropertyGridItem::GetInplaceButtons() const
{
	return m_pInplaceButtons;
}

CXTPPropertyGridInplaceControls* CXTPPropertyGridItem::GetInplaceControls() const
{
	return m_pInplaceControls;
}

// Deletes the item.
void CXTPPropertyGridItem::Remove()
{
	CXTPPropertyGridItems* pItems = GetParentItem() == 0 ? m_pGrid->m_pCategories : GetParentItem()->m_pChilds;
	ASSERT(pItems);

	if (pItems)
	{
		pItems->m_arrItems.RemoveAt(pItems->Find(this));
	}

	m_pGrid->Refresh();
	InternalRelease();
}

BOOL CXTPPropertyGridItem::IsHidden() const
{
	return m_bHidden;
}
void CXTPPropertyGridItem::SetHidden(BOOL bHidden)
{
	if (bHidden == m_bHidden)
		return;

	m_bHidden = bHidden;

	m_pGrid->Refresh();
}

BOOL CXTPPropertyGridItem::HasVisibleChilds() const
{
	for (int i = 0; i < m_pChilds->GetCount(); i++)
	{
		if (!m_pChilds->GetAt(i)->IsHidden())
			return TRUE;
	}
	return FALSE;
}

BOOL CXTPPropertyGridItem::IsSelected() const
{
	return m_pGrid && m_pGrid->GetSelectedItem() == this;
}

void CXTPPropertyGridItem::Refresh(BOOL bInvalidate /*= TRUE*/)
{
	if (m_pGrid && m_pGrid->GetSafeHwnd())
	{
		if (IsSelected())
		{
			OnDeselect();
			OnSelect();

		}
		if (bInvalidate)
		{
			m_pGrid->Invalidate(FALSE);
		}
	}
}

void CXTPPropertyGridItem::SetReadOnly(BOOL bReadOnly)
{
	if (m_bReadOnly != bReadOnly)
	{
		m_bReadOnly = bReadOnly;
		Refresh(TRUE);
	}
}

void CXTPPropertyGridItem::SetMask(LPCTSTR strMask, LPCTSTR strLiteral, TCHAR chPrompt /*= _T('_')*/)
{
	m_bUseMask = TRUE;
	m_strMask = strMask;
	m_strLiteral = strLiteral;
	m_chPrompt = chPrompt;
	Refresh(FALSE);
}

CString CXTPPropertyGridItem::GetViewValue()
{
	CString str = GetValue();
	if (m_bPassword)
	{
		return CString(m_chPrompt, str.GetLength());
	}
	return str;
}

CString CXTPPropertyGridItem::GetMaskedText() const
{
	if (m_bUseMask)
	{
		int nLength = m_strValue.GetLength();

		CString strBuffer;
		for (int i = 0; i < nLength; ++i)
		{
			if (m_strLiteral[i] == m_chPrompt)
			{
				strBuffer += m_strValue.GetAt(i);
			}
		}

		return strBuffer;
	}
	else
	{
		return m_strValue;
	}
}

void CXTPPropertyGridItem::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (m_nHeight != -1)
	{
		lpMeasureItemStruct->itemHeight = m_nHeight;
	}
	else if (m_nLinesCount > 1)
	{
		lpMeasureItemStruct->itemHeight = (lpMeasureItemStruct->itemHeight - 4) * m_nLinesCount + 4;

	}
}


void CXTPPropertyGridItem::SetMaskedText(LPCTSTR lpszMaskedText)
{
	CString strMaskedText = lpszMaskedText;

	if (m_bUseMask)
	{
		int nLen = strMaskedText.GetLength();
		int x = 0;

		m_strValue = m_strLiteral;

		for (int i = 0; (i < m_strValue.GetLength()) && (x < nLen); i++)
		{
			if (strMaskedText[x] == m_strValue[i])
			{
				x++;
			}
			else if (m_strValue[i] == m_chPrompt)
			{
				m_strValue.SetAt(i, strMaskedText[x]);
				x++;
			}
		}
	}
	else
	{
		m_strValue = strMaskedText;
	}

	SetValue(m_strValue);
}

void CXTPPropertyGridItem::SetDescription(LPCTSTR lpszDescription)
{
	m_strDescription = lpszDescription;
	if (IsSelected()) GetGrid()->GetParent()->Invalidate(FALSE);
}

int CXTPPropertyGridItem::GetIndex() const
{
	return m_nIndex;
}


void CXTPPropertyGridItem::OnDrawItemConstraint(CDC* pDC, CXTPPropertyGridItemConstraint* pConstraint, CRect rc, BOOL bSelected)
{
	GetGrid()->GetPaintManager()->DrawInplaceListItem(pDC, pConstraint, rc, bSelected);
}

CSize CXTPPropertyGridItem::OnMergeItemConstraint(CDC* pDC, CXTPPropertyGridItemConstraint* pConstraint)
{
	int nThumbLength = GetSystemMetrics(SM_CXHTHUMB);

	CSize sz = pDC->GetTextExtent(pConstraint->m_strConstraint);

	return CSize(sz.cx + nThumbLength * 2, sz.cy + 3);
}

void CXTPPropertyGridItem::SetFocusToInplaceControl()
{
	CXTPPropertyGridInplaceEdit& wndEdit = GetInplaceEdit();

	if (wndEdit.GetSafeHwnd() && wndEdit.GetItem() == this)
	{
		wndEdit.SetFocus();
		wndEdit.SetSel(0, -1);
	}
}

void CXTPPropertyGridItem::AddComboButton()
{
	GetInplaceButtons()->AddComboButton();
}

void CXTPPropertyGridItem::AddExpandButton()
{
	GetInplaceButtons()->AddExpandButton();
}

CXTPPropertyGridInplaceSlider* CXTPPropertyGridItem::AddSliderControl()
{
	CXTPPropertyGridInplaceSlider* pSlider = new CXTPPropertyGridInplaceSlider(this);
	m_pInplaceControls->Add(pSlider);

	return pSlider;
}

CXTPPropertyGridInplaceSpinButton* CXTPPropertyGridItem::AddSpinButton()
{
	CXTPPropertyGridInplaceSpinButton* pSpinButton = new CXTPPropertyGridInplaceSpinButton(this);
	m_pInplaceControls->Add(pSpinButton);

	return pSpinButton;
}


//////////////////////////////////////////////////////////////////////////
// Accessible

CCmdTarget* CXTPPropertyGridItem::GetAccessible()
{
	return this;
}

HRESULT CXTPPropertyGridItem::GetAccessibleParent(IDispatch* FAR* ppdispParent)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	*ppdispParent = NULL;

	return m_pGrid->InternalQueryInterface(&IID_IAccessible, (void**)ppdispParent);
}

HRESULT CXTPPropertyGridItem::GetAccessibleName(VARIANT varChild, BSTR* pszName)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	CString strCaption = GetCaption();

	*pszName = strCaption.AllocSysString();
	return S_OK;
}

HRESULT CXTPPropertyGridItem::GetAccessibleDescription(VARIANT varChild, BSTR* pszDescription)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	*pszDescription = GetDescription().AllocSysString();
	return S_OK;
}

HRESULT CXTPPropertyGridItem::GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole)
{
	pvarRole->vt = VT_EMPTY;

	if (GetChildIndex(&varChild) == CHILDID_SELF)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_ROW;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPPropertyGridItem::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	pvarState->vt = VT_I4;
	pvarState->lVal = STATE_SYSTEM_FOCUSABLE | STATE_SYSTEM_SELECTABLE |
		(IsSelected() ? STATE_SYSTEM_FOCUSED : 0);

	if (!IsVisible())
		pvarState->lVal |= STATE_SYSTEM_INVISIBLE;

	if (GetReadOnly())
		pvarState->lVal |= STATE_SYSTEM_READONLY;

	return S_OK;
}


HRESULT CXTPPropertyGridItem::AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
{
	*pxLeft = *pyTop = *pcxWidth = *pcyHeight = 0;

	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	if (!m_pGrid->GetSafeHwnd())
		return S_OK;

	CRect rcItem;
	m_pGrid->GetItemRect(GetIndex(), &rcItem);
	m_pGrid->ClientToScreen(&rcItem);

	*pxLeft = rcItem.left;
	*pyTop = rcItem.top;
	*pcxWidth = rcItem.Width();
	*pcyHeight = rcItem.Height();

	return S_OK;
}

HRESULT CXTPPropertyGridItem::AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarID)
{
	if (pvarID == NULL)
		return E_INVALIDARG;

	pvarID->vt = VT_EMPTY;

	if (!m_pGrid->GetSafeHwnd())
		return S_FALSE;

	if (!CXTPWindowRect(m_pGrid).PtInRect(CPoint(xLeft, yTop)))
		return S_FALSE;

	pvarID->vt = VT_I4;
	pvarID->lVal = 0;

	CRect rcItem;
	m_pGrid->GetItemRect(GetIndex(), &rcItem);
	m_pGrid->ClientToScreen(&rcItem);

	if (!rcItem.PtInRect(CPoint(xLeft, yTop)))
		return S_FALSE;

	return S_OK;
}

HRESULT CXTPPropertyGridItem::AccessibleSelect(long /*flagsSelect*/, VARIANT varChild)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	Select();

	return S_OK;
}

HRESULT CXTPPropertyGridItem::GetAccessibleValue(VARIANT varChild, BSTR* pszValue)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	*pszValue = GetValue().AllocSysString();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItems

CXTPPropertyGridItems::CXTPPropertyGridItems()
{

}

void CXTPPropertyGridItems::Clear()
{
	CXTPPropertyGridView* pGrid = GetCount() > 0 ? m_arrItems[0]->m_pGrid : 0;

	if (pGrid)
	{
		if (pGrid->m_nLockUpdate == 0)
			pGrid->ResetContent();
		pGrid->m_nLockUpdate++;
	}

	for (int i = 0; i < GetCount(); i++)
	{
		m_arrItems[i]->Clear();
		m_arrItems[i]->InternalRelease();
	}
	m_arrItems.RemoveAll();

	if (pGrid)
	{
		pGrid->m_nLockUpdate--;
		if (pGrid->m_nLockUpdate == 0)
			pGrid->Refresh();

	}

}

CXTPPropertyGridItem* CXTPPropertyGridItems::GetAt(int nIndex) const
{
	if (nIndex >= 0 && nIndex < GetCount())
	{
		return m_arrItems.GetAt(nIndex);
	}
	return 0;
}

void CXTPPropertyGridItems::RemoveAt (int nIndex)
{
	CXTPPropertyGridItem* pItem = GetAt(nIndex);

	if (pItem)
	{
		m_arrItems.RemoveAt(nIndex);

		if (pItem->GetGrid())
		{
			pItem->GetGrid()->Refresh();
		}

		pItem->InternalRelease();
	}
}

int CXTPPropertyGridItems::Find(CXTPPropertyGridItem* pItem) const
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (m_arrItems[i] == pItem)
			return i;
	}
	return -1;
}


void CXTPPropertyGridItems::AddTail(CXTPPropertyGridItems* pItems)
{
	ASSERT(this != pItems);

	for (int i = 0; i < pItems->GetCount(); i++)
	{
		AddTail(pItems->GetAt(i));
	}
}

int CXTPPropertyGridItems::CompareFunc(const CXTPPropertyGridItem** ppItem1, const CXTPPropertyGridItem** ppItem2)
{
	return (*ppItem1)->GetCaption().Compare((*ppItem2)->GetCaption());
}

void CXTPPropertyGridItems::Sort()
{
	typedef int (_cdecl *GENERICCOMPAREFUNC)(const void *, const void*);

	qsort(m_arrItems.GetData(), m_arrItems.GetSize(), sizeof(CXTPPropertyGridItem*), (GENERICCOMPAREFUNC)CompareFunc);
}

CXTPPropertyGridItem* CXTPPropertyGridItems::FindItem(LPCTSTR strCaption) const
{
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPPropertyGridItem* pItem = GetAt(i);
		if (pItem->GetCaption().Compare(strCaption) == 0)
			return pItem;

		CXTPPropertyGridItem* pChild = pItem->m_pChilds->FindItem(strCaption);
		if (pChild) return pChild;
	}
	return NULL;
}

CXTPPropertyGridItem* CXTPPropertyGridItems::FindItem(UINT nID) const
{
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPPropertyGridItem* pItem = GetAt(i);
		if (pItem->GetID() == nID)
			return pItem;

		CXTPPropertyGridItem* pChild = pItem->m_pChilds->FindItem(nID);
		if (pChild) return pChild;
	}
	return NULL;
}



//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemConstraint
CXTPPropertyGridItemConstraint::CXTPPropertyGridItemConstraint()
{
	m_nIndex = 0;
	m_dwData = 0;
	m_nImage = -1;
	m_pItem = 0;
}

CXTPImageManagerIcon* CXTPPropertyGridItemConstraint::GetImage() const
{
	ASSERT(m_pItem);

	if (!m_pItem)
		return NULL;

	if (m_nImage == -1)
		return NULL;

	return m_pItem->GetGrid()->GetImageManager()->GetImage(m_nImage, 0);
}

/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemConstraints

// Summary: Constructs a CXTPPropertyGridItemConstraints object
CXTPPropertyGridItemConstraints::CXTPPropertyGridItemConstraints(CXTPPropertyGridItem* pItem)
{
	m_pItem = pItem;
	m_nCurrent = -1;

}

CXTPPropertyGridItemConstraints::~CXTPPropertyGridItemConstraints()
{
	RemoveAll();
}

CXTPPropertyGridItemConstraint* CXTPPropertyGridItemConstraints::AddConstraint(LPCTSTR str, DWORD_PTR dwData, int nImage)
{
	CXTPPropertyGridItemConstraint* pConstaint = new CXTPPropertyGridItemConstraint();

	pConstaint->m_strConstraint = str;
	pConstaint->m_dwData = dwData;
	pConstaint->m_nIndex = (int)m_arrConstraints.Add(pConstaint);
	pConstaint->m_pItem = m_pItem;
	pConstaint->m_nImage = nImage;

	SAFE_CALLPTR(m_pItem, OnConstraintsChanged());

	return pConstaint;
}

CString CXTPPropertyGridItemConstraints::GetAt(int nIndex) const
{
	return nIndex >= 0 && nIndex < GetCount() ? m_arrConstraints.GetAt(nIndex)->m_strConstraint : _T("");
}

void CXTPPropertyGridItemConstraints::RemoveAll()
{
	for (int i = 0; i < GetCount(); i++)
		m_arrConstraints[i]->InternalRelease();
	m_arrConstraints.RemoveAll();

	SAFE_CALLPTR(m_pItem, OnConstraintsChanged());
}

void CXTPPropertyGridItemConstraints::RemoveAt(int nIndex)
{
	m_arrConstraints[nIndex]->InternalRelease();
	m_arrConstraints.RemoveAt(nIndex);

	for (int i = nIndex; i < GetCount(); i++)
		m_arrConstraints[i]->m_nIndex = i;

	SAFE_CALLPTR(m_pItem, OnConstraintsChanged());
}

int CXTPPropertyGridItemConstraints::FindConstraint(LPCTSTR str) const
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (GetAt(i).CompareNoCase(str) == 0)
			return i;
	}
	return -1;
}

int CXTPPropertyGridItemConstraints::FindConstraint(DWORD_PTR dwData) const
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (dwData == GetConstraintAt(i)->m_dwData)
			return i;
	}
	return -1;
}

CXTPPropertyGridItemConstraint* CXTPPropertyGridItemConstraints::GetConstraintAt(int nIndex) const
{
	return m_arrConstraints[nIndex];
}

int CXTPPropertyGridItemConstraints::CompareFunc(const CXTPPropertyGridItemConstraint** ppConstraint1, const CXTPPropertyGridItemConstraint** ppConstraint2)
{
	return (*ppConstraint1)->m_strConstraint.Compare((*ppConstraint2)->m_strConstraint);

}

void CXTPPropertyGridItemConstraints::Sort()
{
	typedef int (_cdecl *GENERICCOMPAREFUNC)(const void *, const void*);

	qsort(m_arrConstraints.GetData(), (size_t)m_arrConstraints.GetSize(), sizeof(CXTPPropertyGridItemConstraint*), (GENERICCOMPAREFUNC)CompareFunc);
}


//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemCategory

CXTPPropertyGridItemCategory::CXTPPropertyGridItemCategory(LPCTSTR strCaption)
	:CXTPPropertyGridItem(strCaption)
{
	m_bCategory = TRUE;
	m_nFlags = 0;
}

CXTPPropertyGridItemCategory::CXTPPropertyGridItemCategory(UINT nID)
	:CXTPPropertyGridItem(nID)
{
	m_bCategory = TRUE;
	m_nFlags = 0;
}





///////////////////////////////////////////////////////////////////////////////////
//  Acteve X Methods
//


BEGIN_INTERFACE_MAP(CXTPPropertyGridItem, CCmdTarget)
	INTERFACE_PART(CXTPPropertyGridItem, IID_IAccessible, ExternalAccessible)
END_INTERFACE_MAP()
