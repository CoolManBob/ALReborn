// XTPSyntaxEditColorComboBox.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
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
#include "Common/Resource.h"

// common includes
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPResourceManager.h"

// syntax editor includes
#include "XTPSyntaxEditColorComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLOR_ITEM_WIDTH   11

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditColorComboBox

CXTPSyntaxEditColorComboBox::CXTPSyntaxEditColorComboBox()
: m_bPreInit(true)
, m_crAuto(COLORREF_NULL)
, m_crUser(COLORREF_NULL)
, m_iPrevSel(CB_ERR)
{
}

CXTPSyntaxEditColorComboBox::~CXTPSyntaxEditColorComboBox()
{
}

IMPLEMENT_DYNAMIC(CXTPSyntaxEditColorComboBox, CComboBox)

BEGIN_MESSAGE_MAP(CXTPSyntaxEditColorComboBox, CComboBox)
	//{{AFX_MSG_MAP(CXTPSyntaxEditColorComboBox)
	ON_WM_CREATE()
	ON_WM_MOUSEWHEEL()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditColorComboBox message handlers

int CXTPSyntaxEditColorComboBox::SetSelColor(COLORREF crColor)
{
	int iIndex = FindColor(crColor);

	if (iIndex == CB_ERR)
	{
		iIndex = SetUserColor(crColor);

		if (iIndex == CB_ERR)
		{
			return CB_ERR;
		}
	}

	return SetCurSel(iIndex);
}

COLORREF CXTPSyntaxEditColorComboBox::GetSelColor()
{
	int iIndex = GetCurSel();

	if (iIndex == CB_ERR)
	{
		return COLORREF_NULL;
	}

	return (COLORREF)GetItemData(iIndex);
}

int CXTPSyntaxEditColorComboBox::DeleteColor(COLORREF crColor)
{
	int iIndex = FindColor(crColor);

	if (iIndex != CB_ERR)
	{
		return DeleteString(iIndex);
	}

	return CB_ERR;
}

int CXTPSyntaxEditColorComboBox::FindColor(COLORREF crColor)
{
	for (int iIndex = 0; iIndex < GetCount(); ++iIndex)
	{
		COLORREF crItem = (COLORREF)GetItemData(iIndex);
		if (crItem == crColor)
		{
			return iIndex;
		}
	}
	return CB_ERR;
}

int CXTPSyntaxEditColorComboBox::AddColor(COLORREF crColor, UINT nID)
{
	CString csName;
	XTPResourceManager()->LoadString(&csName, nID);

	if (!csName.IsEmpty())
	{
		int iIndex = AddString(csName);

		if (iIndex != CB_ERR)
			SetItemData(iIndex, (DWORD)crColor);

		return iIndex;
	}
	return CB_ERR;
}

bool CXTPSyntaxEditColorComboBox::Init()
{
	// MFCBUG: adjust height so display is the same as non-owner drawn
	// CComboBoxes. MFC sets the height of an owner-drawn CComboBox
	// 2-3 pixels larger than a non owner-drawn combo.

	SetItemHeight(-1, (::GetSystemMetrics(SM_CYVTHUMB)-::GetSystemMetrics(SM_CYEDGE)));

	ResetContent();

	AddColor( RGB(0x00,0x00,0x00), XTP_IDS_CLR_BLACK ),
	AddColor( RGB(0xff,0xff,0xff), XTP_IDS_CLR_WHITE ),
	AddColor( RGB(0x80,0x00,0x00), XTP_IDS_CLR_MAROON ),
	AddColor( RGB(0x00,0x80,0x00), XTP_IDS_CLR_DARK_GREEN ),
	AddColor( RGB(0x80,0x80,0x00), XTP_IDS_CLR_OLIVE ),
	AddColor( RGB(0x00,0x00,0x80), XTP_IDS_CLR_DARK_BLUE ),
	AddColor( RGB(0x80,0x00,0x80), XTP_IDS_CLR_PURPLE ),
	AddColor( RGB(0x00,0x80,0x80), XTP_IDS_CLR_TEAL ),
	AddColor( RGB(0xC0,0xC0,0xC0), XTP_IDS_CLR_GRAY25 ),
	AddColor( RGB(0x80,0x80,0x80), XTP_IDS_CLR_GRAY50 ),
	AddColor( RGB(0xFF,0x00,0x00), XTP_IDS_CLR_RED ),
	AddColor( RGB(0x00,0xFF,0x00), XTP_IDS_CLR_GREEN ),
	AddColor( RGB(0xFF,0xFF,0x00), XTP_IDS_CLR_YELLOW ),
	AddColor( RGB(0x00,0x00,0xFF), XTP_IDS_CLR_BLUE ),
	AddColor( RGB(0xFF,0x00,0xFF), XTP_IDS_CLR_PINK ),
	AddColor( RGB(0x00,0xFF,0xFF), XTP_IDS_CLR_TURQUOISE ),

	SetCurSel(0);
	return true;
}

void CXTPSyntaxEditColorComboBox::PreSubclassWindow()
{
	CComboBox::PreSubclassWindow();

	if (m_bPreInit)
	{
		// Initialize the control.
		Init();
	}
}

int CXTPSyntaxEditColorComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Initialize the control.
	Init();

	return 0;
}

BOOL CXTPSyntaxEditColorComboBox::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CComboBox::PreCreateWindow(cs))
		return FALSE;

	// When creating controls dynamically Init() must
	// be called from OnCreate() and not from
	// PreSubclassWindow().

	m_bPreInit = false;

	return TRUE;
}

int CXTPSyntaxEditColorComboBox::SelectUserColor()
{
	CColorDialog dlg(GetSelColor(), CC_RGBINIT, this);
	if (dlg.DoModal() == IDOK)
	{
		COLORREF crUser = dlg.GetColor();
		if (FindColor(crUser) == CB_ERR)
		{
			SetUserColor(crUser);
		}
		else
		{
			SetUserColor(COLORREF_NULL);
		}
		return SetSelColor(crUser);
	}
	return CB_ERR;
}

int CXTPSyntaxEditColorComboBox::SetUserColor(COLORREF crColor, LPCTSTR lpszUserText/*=NULL*/)
{
	CString csCustom;
	if (lpszUserText == NULL)
	{
		XTPResourceManager()->LoadString(
			&csCustom, XTP_IDS_EDIT_CUSTOM);
	}
	else
	{
		csCustom = lpszUserText;
	}

	if (crColor == COLORREF_NULL)
	{
		m_crUser = COLORREF_NULL;

		int iIndex = FindStringExact(-1, csCustom);
		if (iIndex != CB_ERR)
		{
			return DeleteString(iIndex);
		}

		return CB_ERR;
	}

	if (m_crUser == COLORREF_NULL)
	{
		int iIndex = AddString(csCustom);
		if (iIndex != CB_ERR)
		{
			m_crUser = crColor;
			SetItemData(iIndex, (DWORD)m_crUser);
			RedrawWindow();
			return iIndex;
		}
	}
	else
	{
		int iIndex = FindStringExact(-1, csCustom);
		if (iIndex != CB_ERR)
		{
			m_crUser = crColor;
			SetItemData(iIndex, (DWORD)m_crUser);
			RedrawWindow();
			return iIndex;
		}
	}

	return CB_ERR;
}

int CXTPSyntaxEditColorComboBox::SetAutoColor(COLORREF crColor, LPCTSTR lpszAutoText/*=NULL*/)
{
	CString csAuto;
	if (lpszAutoText == NULL)
	{
		XTPResourceManager()->LoadString(
			&csAuto, XTP_IDS_CLR_AUTOMATIC);

		int nTipIndex = FIND_S(csAuto, _T('\n'), 0);
		if (nTipIndex > 0)
			csAuto.ReleaseBuffer(nTipIndex);
	}
	else
	{
		csAuto = lpszAutoText;
	}

	if (crColor == COLORREF_NULL)
	{
		m_crAuto = COLORREF_NULL;

		int iIndex = FindStringExact(-1, csAuto);
		if (iIndex != CB_ERR)
		{
			return DeleteString(iIndex);
		}

		return CB_ERR;
	}

	if (m_crAuto == COLORREF_NULL)
	{
		int iIndex = InsertString(0, csAuto);
		if (iIndex != CB_ERR)
		{
			m_crAuto = crColor;
			SetItemData(iIndex, (DWORD)m_crAuto);
			RedrawWindow();
			return iIndex;
		}
	}
	else
	{
		int iIndex = FindStringExact(-1, csAuto);
		if (iIndex != CB_ERR)
		{
			m_crAuto = crColor;
			SetItemData(iIndex, (DWORD)m_crAuto);
			RedrawWindow();
			return iIndex;
		}
	}

	return CB_ERR;
}

void CXTPSyntaxEditColorComboBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC*  pDC       = CDC::FromHandle(lpDIS->hDC);
	UINT  itemState = lpDIS->itemState;
	UINT  itemID    = lpDIS->itemID;
	CRect rcItem    = lpDIS->rcItem;

	if (itemID == (UINT)-1)
	{
		return;
	}

	BOOL bDisabled = ((itemState & ODS_DISABLED) == ODS_DISABLED);
	BOOL bSelected = ((itemState & ODS_SELECTED) == ODS_SELECTED);
	BOOL bFocus    = ((itemState & ODS_FOCUS)    == ODS_FOCUS);

	// draw background.
	if (bDisabled)
	{
		pDC->SetTextColor(GetXtremeColor(COLOR_GRAYTEXT));
		pDC->SetBkColor(GetXtremeColor(COLOR_3DFACE));
		pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_3DFACE));
	}
	else
	{
		if (bSelected)
		{
			pDC->SetTextColor(GetXtremeColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(GetXtremeColor(COLOR_WINDOW));
			pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_HIGHLIGHT));
		}
		else
		{
			pDC->SetTextColor(GetXtremeColor(COLOR_WINDOWTEXT));
			pDC->SetBkColor(GetXtremeColor(COLOR_WINDOW));
			pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_WINDOW));
		}

		// draw focus rectangle.
		if (bFocus)
		{
			pDC->DrawFocusRect(&rcItem);
		}
	}

	// determine the size of the color rectangle.
	CRect rColor(rcItem);
	rColor.DeflateRect(2,2);
	rColor.right = rColor.left + COLOR_ITEM_WIDTH;
	rColor.bottom = rColor.top + COLOR_ITEM_WIDTH;

	// draw color rectangle.
	pDC->FillSolidRect(rColor,
		bDisabled? GetXtremeColor(COLOR_3DFACE): (COLORREF)lpDIS->itemData);

	pDC->Draw3dRect(rColor,
		GetXtremeColor(bDisabled? COLOR_GRAYTEXT: COLOR_WINDOWTEXT),
		GetXtremeColor(bDisabled? COLOR_GRAYTEXT: COLOR_WINDOWTEXT));

	// determine the size of the text display.
	CRect rText(rColor);
	rText.top -= 2;
	rText.bottom = rText.top + (::GetSystemMetrics(SM_CYVTHUMB)-::GetSystemMetrics(SM_CYEDGE));
	rText.left = rText.right + 4;
	rText.right = rcItem.right;

	// draw text.
	CString csItemText;
	GetLBText(itemID, csItemText);
	if (!csItemText.IsEmpty())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(csItemText, rText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
}

void CXTPSyntaxEditColorComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	ASSERT(lpMIS->CtlType == ODT_COMBOBOX);
	lpMIS->itemHeight = (::GetSystemMetrics(SM_CYVTHUMB)-::GetSystemMetrics(SM_CYEDGE));
	lpMIS->itemWidth = 0;
}

int CXTPSyntaxEditColorComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
{
	COLORREF color1 = (COLORREF)lpCIS->itemData1;
	COLORREF color2 = (COLORREF)lpCIS->itemData2;

	// exact match
	if (color1 == color2)
		return 0;

	// first do an intensity sort, lower intensities go first
	int intensity1 = GetRValue(color1) + GetGValue(color1) + GetBValue(color1);
	int intensity2 = GetRValue(color2) + GetGValue(color2) + GetBValue(color2);

	// lower intensity goes first
	if (intensity1 < intensity2)
		return -1;

	// higher intensity goes second
	else if (intensity1 > intensity2)
		return 1;

	// if same intensity, sort by color (blues first, reds last)
	if (GetBValue(color1) > GetBValue(color2))
		return -1;

	else if (GetGValue(color1) > GetGValue(color2))
		return -1;

	else if (GetRValue(color1) > GetRValue(color2))
		return -1;

	return 1;
}

BOOL CXTPSyntaxEditColorComboBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (GetDroppedState() == TRUE)
	{
		HWND hWnd = ::FindWindow(_T("ComboLBox"), NULL);
		if (::IsWindow(hWnd))
		{
			::SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);

			BOOL bRet = CComboBox::OnMouseWheel(nFlags, zDelta, pt);

			::SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
			::RedrawWindow(hWnd, NULL, (HRGN)NULL,
				RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE | RDW_ERASE);

			return bRet;
		}
	}

	return CComboBox::OnMouseWheel(nFlags, zDelta, pt);
}

void CXTPSyntaxEditColorComboBox::NotifyOwner(UINT nCode)
{
	CWnd* pWndOwner = CWnd::GetOwner();
	if (::IsWindow(pWndOwner->GetSafeHwnd()))
	{
		pWndOwner->SendMessage(WM_COMMAND,
			MAKEWPARAM(GetDlgCtrlID(), nCode), (LPARAM)m_hWnd);
	}
}

int CXTPSyntaxEditColorComboBox::GetLBCurSel() const
{
	if (GetDroppedState() == TRUE)
	{
		HWND hWnd = ::FindWindow(_T("ComboLBox"), NULL);
		if (::IsWindow(hWnd))
		{
			return (int)::SendMessage(hWnd, LB_GETCURSEL, 0, 0);
		}
	}
	return LB_ERR;
}

BOOL CXTPSyntaxEditColorComboBox::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			switch (pMsg->wParam)
			{
			case VK_ESCAPE:
				{
					if (GetDroppedState() == TRUE)
					{
						ShowDropDown(FALSE);
						NotifyOwner(CBN_SELENDCANCEL);
						return TRUE;
					}
				}
				break;

			case VK_SPACE:
			case VK_RETURN:
				{
					if (GetDroppedState() == TRUE)
					{
						m_iPrevSel = GetLBCurSel();
						ShowDropDown(FALSE);
						NotifyOwner(CBN_SELENDOK);
						return TRUE;
					}
				}
				break;

			case VK_UP:
			case VK_DOWN:
				{
					if (GetDroppedState() == FALSE)
					{
						m_iPrevSel = GetCurSel();
						ShowDropDown(TRUE);
						return TRUE;
					}
				}
				break;
			}
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CXTPSyntaxEditColorComboBox::OnCloseUp()
{
	if (m_iPrevSel != CB_ERR)
	{
		if (m_iPrevSel != GetCurSel())
			SetCurSel(m_iPrevSel);

		m_iPrevSel = CB_ERR;
	}
}

_XTP_EXT_CLASS void AFXAPI DDX_CBSyntaxColor(CDataExchange *pDX, int nIDC, COLORREF& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	ASSERT(hWndCtrl != NULL);

	CXTPSyntaxEditColorComboBox* pColorCombo = (CXTPSyntaxEditColorComboBox*)CWnd::FromHandle(hWndCtrl);
	if (pDX->m_bSaveAndValidate)
	{
		value = pColorCombo->GetSelColor();
	}
	else
	{
		pColorCombo->SetSelColor(value);
	}
}
