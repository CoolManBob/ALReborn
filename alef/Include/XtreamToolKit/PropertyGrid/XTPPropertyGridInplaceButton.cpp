// XTPPropertyGridInplaceButton.cpp : implementation of the CXTPPropertyGridInplaceButton class.
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
#include "Common/XTPImageManager.h"

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGrid.h"
#include "XTPPropertyGridPaintManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CXTColorPicker

CXTPPropertyGridInplaceButton::CXTPPropertyGridInplaceButton(UINT nID)
{
	m_pItem = NULL;
	m_nID = nID;
	m_pGrid = NULL;
	m_rcButton.SetRectEmpty();
	m_nIndex = -1;
	m_bPressed = FALSE;
	m_nWidth = GetSystemMetrics(SM_CXHTHUMB);
	m_nIconIndex = -1;


}

CXTPPropertyGridInplaceButton::~CXTPPropertyGridInplaceButton()
{

}

CXTPImageManagerIcon* CXTPPropertyGridInplaceButton::GetImage() const
{
	ASSERT(m_pItem);

	if (!m_pItem)
		return NULL;

	if (m_nIconIndex == -1)
		return NULL;

	return m_pGrid->GetImageManager()->GetImage(m_nIconIndex, 0);
}

void CXTPPropertyGridInplaceButton::SetCaption(LPCTSTR lpszCaption)
{
	m_strCaption = lpszCaption;

	CWindowDC dc(NULL);
	CXTPFontDC font(&dc, m_pGrid->GetPaintManager()->GetItemFont(m_pItem, TRUE));
	m_nWidth = dc.GetTextExtent(m_strCaption).cx + 6;
}

int CXTPPropertyGridInplaceButton::GetIndex() const
{
	return m_nIndex;
}


BOOL CXTPPropertyGridInplaceButton::IsFocused() const
{
	return m_pGrid->m_pFocusedButton == this;
}

BOOL CXTPPropertyGridInplaceButton::IsHot() const
{
	return m_pGrid->m_pHotButton == this;
}

BOOL CXTPPropertyGridInplaceButton::IsPressed() const
{
	return m_bPressed;
}


/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridInplaceButton message handlers

void CXTPPropertyGridInplaceButton::OnDraw(CDC* pDC, CRect rc)
{
	m_rcButton = rc;

	m_pGrid->GetPaintManager()->FillInplaceButton(pDC, this);
}

void CXTPPropertyGridInplaceButton::OnLButtonDown(UINT, CPoint)
{
	m_bPressed = TRUE;
	m_pGrid->SetFocus();

	m_pGrid->Invalidate(FALSE);

	m_pGrid->SetCapture();
	BOOL bClick = TRUE;

	while (::GetCapture() == m_pGrid->GetSafeHwnd())
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (msg.message == WM_LBUTTONUP)
		{
			bClick = IsHot();
			break;
		}

		if (msg.message == WM_MOUSEMOVE)
		{
			CPoint pt = CPoint(LOWORD(msg.lParam), HIWORD(msg.lParam));

			CXTPPropertyGridInplaceButton* pHotButton = m_rcButton.PtInRect(pt) ? this : NULL;

			if (m_pGrid->m_pHotButton != pHotButton)
			{
				m_pGrid->m_pHotButton = pHotButton;
				m_pGrid->Invalidate(FALSE);
			}
			continue;
		}

		DispatchMessage (&msg);
	}

	m_bPressed = FALSE;

	ReleaseCapture();
	m_pGrid->Invalidate(FALSE);

	if (bClick)
	{
		m_pItem->OnInplaceButtonDown(this);
	}
}

int CXTPPropertyGridInplaceButton::GetWidth()
{
	return m_nWidth;
}

void CXTPPropertyGridInplaceButton::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
}


void CXTPPropertyGridInplaceButton::OnKeyDown(UINT nChar)
{
	if (m_pItem && (nChar == VK_SPACE || nChar == VK_DOWN || nChar == VK_RETURN || nChar == VK_F4))
	{
		m_pItem->OnInplaceButtonDown(this);
		return;
	}
}


//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridInplaceButtons

CXTPPropertyGridInplaceButtons::CXTPPropertyGridInplaceButtons(CXTPPropertyGridItem* pItem)
{
	m_pItem = pItem;
}

CXTPPropertyGridInplaceButtons::~CXTPPropertyGridInplaceButtons()
{
	RemoveAll();
}

void CXTPPropertyGridInplaceButtons::RemoveAll()
{
	for (int i = 0; i < GetCount(); i++)
	{
		m_arrButtons[i]->InternalRelease();
	}
	m_arrButtons.RemoveAll();
}

void CXTPPropertyGridInplaceButtons::UpdateIndexes()
{
	for (int i = 0; i < GetCount(); i++)
	{
		m_arrButtons[i]->m_nIndex = i;
	}
}

void CXTPPropertyGridInplaceButtons::Remove(CXTPPropertyGridInplaceButton* pButton)
{
	if (!pButton)
		return;

	for (int i = 0; i < GetCount(); i++)
	{
		if (m_arrButtons[i] == pButton)
		{
			m_arrButtons.RemoveAt(i);
			pButton->InternalRelease();
			UpdateIndexes();
			return;
		}
	}
}

void CXTPPropertyGridInplaceButtons::Remove(UINT nID)
{
	Remove(Find(nID));
}

CXTPPropertyGridInplaceButton* CXTPPropertyGridInplaceButtons::AddButton(CXTPPropertyGridInplaceButton* pButton)
{
	pButton->m_pItem = m_pItem;
	pButton->m_pGrid = m_pItem->GetGrid();
	m_arrButtons.Add(pButton);
	UpdateIndexes();

	return pButton;
}

void CXTPPropertyGridInplaceButtons::AddComboButton()
{
	if (Find(XTP_ID_PROPERTYGRID_COMBOBUTTON) == NULL)
	{
		AddButton(new CXTPPropertyGridInplaceButton(XTP_ID_PROPERTYGRID_COMBOBUTTON));
	}
}

void CXTPPropertyGridInplaceButtons::AddExpandButton()
{
	if (Find(XTP_ID_PROPERTYGRID_EXPANDBUTTON) == NULL)
	{
		AddButton(new CXTPPropertyGridInplaceButton(XTP_ID_PROPERTYGRID_EXPANDBUTTON));
	}
}

CXTPPropertyGridInplaceButton* CXTPPropertyGridInplaceButtons::Find(UINT nID) const
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (m_arrButtons[i]->GetID() == nID)
			return m_arrButtons[i];
	}
	return NULL;
}

CXTPPropertyGridInplaceButton* CXTPPropertyGridInplaceButtons::HitTest(CPoint point) const
{
	if (!m_pItem->IsInplaceButtonsVisible())
		return NULL;

	for (int i = 0; i < GetCount(); i++)
	{
		if (m_arrButtons[i]->GetRect().PtInRect(point))
			return m_arrButtons[i];
	}
	return NULL;
}


