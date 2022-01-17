// XTPPropertyGridItemColor.cpp : implementation of the CXTPPropertyGridItemColor class.
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

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGridItemColor.h"
#include "XTPPropertyGrid.h"
#include "XTPPropertyGridDefines.h"

#ifdef _XTP_INCLUDE_CONTROLS
#include "Controls/XTColorDialog.h"
#include "Controls/XTColorSelectorCtrl.h"
#include "Controls/XTColorPopup.h"
#include "Controls/XTNotify.h"
#include "Controls/XTGlobal.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemColor

IMPLEMENT_DYNAMIC(CXTPPropertyGridItemColor, CXTPPropertyGridItem)

CXTPPropertyGridItemColor::CXTPPropertyGridItemColor(LPCTSTR strCaption, COLORREF clr, COLORREF* pBindColor)
	: CXTPPropertyGridItem(strCaption)
{
	m_pBindColor = pBindColor;
	SetColor(clr);
	m_nFlags = xtpGridItemHasEdit | xtpGridItemHasExpandButton;
	m_strDefaultValue = m_strValue;
	m_colorEditor = xtpGridItemColorExtendedDialog;
}

CXTPPropertyGridItemColor::CXTPPropertyGridItemColor(UINT nID, COLORREF clr, COLORREF* pBindColor)
	: CXTPPropertyGridItem(nID)
{
	m_pBindColor = pBindColor;
	SetColor(clr);
	m_nFlags = xtpGridItemHasEdit | xtpGridItemHasExpandButton;
	m_strDefaultValue = m_strValue;
	m_colorEditor = xtpGridItemColorExtendedDialog;
}

CXTPPropertyGridItemColor::~CXTPPropertyGridItemColor()
{
}

/////////////////////////////////////////////////////////////////////////////
//

BOOL CXTPPropertyGridItemColor::OnDrawItemValue(CDC& dc, CRect rcValue)
{
	COLORREF clr = dc.GetTextColor();
	CRect rcSample(rcValue.left - 2, rcValue.top + 1, rcValue.left + 18, rcValue.bottom - 1);
	CXTPPenDC pen(dc, clr);
	CXTPBrushDC brush(dc, m_clrValue);
	dc.Rectangle(rcSample);

	CRect rcText(rcValue);
	rcText.left += 25;

	dc.DrawText(m_strValue, rcText, DT_SINGLELINE | DT_VCENTER);

	return TRUE;
}

CRect CXTPPropertyGridItemColor::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	rcValue.left += 25;
	return rcValue;
}

COLORREF AFX_CDECL CXTPPropertyGridItemColor::StringToRGB(LPCTSTR str)
{
	CString strRed, strGreen, strBlue;

	AfxExtractSubString(strRed, str, 0, ';');
	AfxExtractSubString(strGreen, str, 1, ';');
	AfxExtractSubString(strBlue, str, 2, ';');

	return RGB(__min(_ttoi(strRed), 255), __min(_ttoi(strGreen), 255), __min(_ttoi(strBlue), 255));
}

CString AFX_CDECL CXTPPropertyGridItemColor::RGBToString(COLORREF clr)
{
	CString str;
	str.Format(_T("%i; %i; %i"), GetRValue(clr), GetGValue(clr), GetBValue(clr));
	return str;
}

void CXTPPropertyGridItemColor::SetValue(CString strValue)
{
	SetColor(StringToRGB(strValue));
}

void CXTPPropertyGridItemColor::SetColor(COLORREF clr)
{
	m_clrValue = clr;

	if (m_pBindColor)
	{
		*m_pBindColor = clr;
	}

	CXTPPropertyGridItem::SetValue(RGBToString(clr));
}

void CXTPPropertyGridItemColor::BindToColor(COLORREF* pBindColor)
{
	m_pBindColor = pBindColor;
	if (m_pBindColor)
	{
		*m_pBindColor = m_clrValue;
	}
}


void CXTPPropertyGridItemColor::OnBeforeInsert()
{
	if (m_pBindColor && *m_pBindColor != m_clrValue)
	{
		SetColor(*m_pBindColor);
	}
}

void CXTPPropertyGridItemColor::SetEditorStyle(XTPPropertyGridItemColorEditor editor)
{
	m_colorEditor = editor;

	if (m_colorEditor == xtpGridItemColorPopup)
		SetFlags(xtpGridItemHasEdit | xtpGridItemHasComboButton);
	else
		SetFlags(xtpGridItemHasEdit | xtpGridItemHasExpandButton);
}

XTPPropertyGridItemColorEditor CXTPPropertyGridItemColor::GetEditorStyle()
{
	return m_colorEditor;
}


void CXTPPropertyGridItemColor::OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton)
{
	if (m_pGrid->SendNotifyMessage(XTP_PGN_INPLACEBUTTONDOWN, (LPARAM)pButton) == TRUE)
		return;

	if (!OnRequestEdit())
		return;

	CWnd* pGrid = m_pGrid;

#ifdef _XTP_INCLUDE_CONTROLS
	if (m_colorEditor == xtpGridItemColorPopup)
	{
		class CPropertyGridItemColorColorPopup: public CXTColorPopup
		{
		public:
			CPropertyGridItemColorColorPopup()
				: CXTColorPopup(TRUE)
			{
				m_pItem = NULL;
			}

			BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
			{
				if (message == CPN_XT_SELENDOK)
				{
					m_pItem->OnValueChanged(m_pItem->RGBToString((COLORREF)wParam));
					*pResult = 0;
					return TRUE;
				}
				return CXTColorPopup::OnWndMsg(message, wParam, lParam, pResult);\
			}

			CXTPPropertyGridItemColor* m_pItem;
		};


		CPropertyGridItemColorColorPopup *pColorPopup = new CPropertyGridItemColorColorPopup();

		CRect rcItem= GetItemRect();
		m_pGrid->ClientToScreen(&rcItem);
		rcItem.left = rcItem.right - 158; // small hack. need to add CPS_XT_LEFTALLIGN

		COLORREF clrDefault = StringToRGB(m_strDefaultValue);
		pColorPopup->Create(rcItem, m_pGrid, CPS_XT_USERCOLORS|CPS_XT_EXTENDED|CPS_XT_MORECOLORS|CPS_XT_SHOW3DSELECTION|CPS_XT_SHOWHEXVALUE, GetColor(), clrDefault);
		pColorPopup->SetOwner(m_pGrid);
		pColorPopup->SetFocus();
		pColorPopup->AddListener(pColorPopup->GetSafeHwnd());
		pColorPopup->m_pItem = this;
	}
	else if (m_colorEditor == xtpGridItemColorExtendedDialog)
	{
		InternalAddRef();

		CXTColorDialog dlg(m_clrValue, m_clrValue, 0, m_pGrid);

		if (dlg.DoModal() == IDOK)
		{
			CString strValue = RGBToString(dlg.GetColor());

			if (OnAfterEdit(strValue))
			{
				OnValueChanged(strValue);
				SAFE_INVALIDATE(pGrid);
			}
		}
		else
		{
			OnCancelEdit();
		}

		InternalRelease();
	}
	else
#endif
	{
		InternalAddRef();

		CColorDialog dlg(m_clrValue, 0, m_pGrid);

		if (dlg.DoModal() == IDOK)
		{
			CString strValue = RGBToString(dlg.GetColor());

			if (OnAfterEdit(strValue))
			{
				OnValueChanged(strValue);
				SAFE_INVALIDATE(pGrid);
			}
		}
		else
		{
			OnCancelEdit();
		}

		InternalRelease();
	}
}



