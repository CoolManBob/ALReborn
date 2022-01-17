// XTFontCombo.cpp : implementation of the CXTFontCombo class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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

#include "Common/XTPVC80Helpers.h"  // Visual Studio 2005 helper functions
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTVC50Helpers.h"

#include "XTFlatComboBox.h"
#include "XTListBox.h"
#include "XTFontCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// CXTFontEnum
//////////////////////////////////////////////////////////////////////

const int BMW = 16;

CXTFontEnum::CXTFontEnum()
{
	Init();
}

CXTFontEnum::~CXTFontEnum()
{

}

CXTFontEnum& CXTFontEnum::Get()
{
	static CXTFontEnum fontEnum;
	return fontEnum;
}

bool CXTFontEnum::DoesFontExist(CString& strFaceName)
{
	POSITION pos;
	for (pos = m_listFonts.GetHeadPosition(); pos; m_listFonts.GetNext(pos))
	{
		CXTLogFont& lf = m_listFonts.GetAt(pos);
		if (strFaceName.CompareNoCase(lf.lfFaceName) == 0)
		{
			strFaceName = lf.lfFaceName; // adjust case if necessary
			return true;
		}
	}
	return false;
}

CXTLogFont* CXTFontEnum::GetLogFont(const CString& strFaceName)
{
	POSITION pos;
	for (pos = m_listFonts.GetHeadPosition(); pos; m_listFonts.GetNext(pos))
	{
		CXTLogFont& lf = m_listFonts.GetAt(pos);
		if (strFaceName == lf.lfFaceName)
		{
			return &lf;
		}
	}
	return NULL;
}

bool CXTFontEnum::AddFont(const LOGFONT* pLF, DWORD dwType)
{
	// See if the font name has already been added.
	CXTStringHelper strFaceName = pLF->lfFaceName;
	strFaceName.Remove('@');

	if (DoesFontExist(strFaceName) != 0)
	{
		return false;
	}

	LOGFONT lfGlobal;
	XTAuxData().font.GetLogFont(&lfGlobal);

	// Save the CXTLogFont struct.
	CXTLogFont lf;

	MEMCPY_S((void*)&lf, (const void*)pLF, sizeof(CXTLogFont));

	lf.dwType = dwType;
	lf.lfHeight = -(::GetSystemMetrics(SM_CYVTHUMB)-(::GetSystemMetrics(SM_CYEDGE)*2));
	lf.lfWidth = 0;

	m_listFonts.AddTail(lf);

	return true;
}

BOOL CALLBACK CXTFontEnum::EnumFontFamExProc(ENUMLOGFONTEX* pelf,
	NEWTEXTMETRICEX* /*lpntm*/, DWORD dwFontType, LPARAM lParam)
{
	CXTFontEnum* pFontEnum = (CXTFontEnum*)lParam;
	if (pFontEnum != NULL)
	{
		if (pFontEnum->AddFont(&pelf->elfLogFont, dwFontType))
		{
			// TODO:
		}
	}

	return TRUE;
}

int CXTFontEnum::GetMaxWidth()
{
	CWindowDC dc(NULL);
	CFont* p = dc.SelectObject(&XTAuxData().font);

	int iMaxWidth = 0;

	POSITION pos;
	for (pos = m_listFonts.GetHeadPosition(); pos; m_listFonts.GetNext(pos))
	{
		CXTLogFont& lf = m_listFonts.GetAt(pos);

		int nWidth = dc.GetTextExtent(lf.lfFaceName).cx;
		iMaxWidth = __max(iMaxWidth, nWidth);
	}

	dc.SelectObject(p);
	return iMaxWidth;
}

void CXTFontEnum::Init(CDC* pDC/*= NULL*/, BYTE nCharSet/*= DEFAULT_CHARSET*/)
{
	m_listFonts.RemoveAll();

	// Enumerate all styles of all fonts for the ANSI character set
	CXTLogFont lf;
	lf.lfFaceName[ 0 ] = '\0';
	lf.lfCharSet = nCharSet;

	if (pDC == NULL || pDC->m_hDC == NULL)
	{
		CWindowDC dc(NULL);
		::EnumFontFamiliesEx(dc.m_hDC, &lf,
			(FONTENUMPROC)EnumFontFamExProc, (LPARAM)this, 0);
	}
	else
	{
		::EnumFontFamiliesEx(pDC->m_hDC, &lf,
			(FONTENUMPROC)EnumFontFamExProc, (LPARAM)this, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTFontListBox
/////////////////////////////////////////////////////////////////////////////

CXTFontListBox::CXTFontListBox()
{
	m_csSymbol = _T("AaBbCc");
	m_dwStyle  = xtFontBoth;

	VERIFY(m_ilFontType.Create(
		XT_IDB_FONTTYPE, BMW, 1, RGB(255, 0, 255)));
}

CXTFontListBox::~CXTFontListBox()
{
}

BEGIN_MESSAGE_MAP(CXTFontListBox, CXTListBox)
	//{{AFX_MSG_MAP(CXTFontListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTFontListBox message handlers

void CXTFontListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC*  pDC       = CDC::FromHandle(lpDIS->hDC);
	UINT  itemState = lpDIS->itemState;
	UINT  itemID    = lpDIS->itemID;
	CRect rcItem    = lpDIS->rcItem;

	if (itemID == (UINT)-1)
	{
		return;
	}

	CXTLogFont* pLF = (CXTLogFont*)lpDIS->itemData;
	if (pLF == NULL)
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

	CString csFaceName = pLF->lfFaceName;

	// true type or device flag set by SetItemData
	if (pLF->dwType & TRUETYPE_FONTTYPE)
	{
		m_ilFontType.Draw(pDC, 1, rcItem.TopLeft(), ILD_NORMAL);
	}

	// Define the size of the text area to draw
	CRect rcText(rcItem);
	rcText.left += BMW + 6;

	pDC->SetBkMode(TRANSPARENT);

	if (m_dwStyle & xtFontGUI)
	{
		CXTPFontDC fontDC(pDC, &XTAuxData().font);
		pDC->DrawText(csFaceName, rcText, DT_VCENTER | DT_SINGLELINE);
	}

	if (m_dwStyle & xtFontSample)
	{
		if (m_dwStyle & xtFontGUI)
			rcText.left = rcText.left + CXTFontEnum::Get().GetMaxWidth() + 5;

		if ((m_dwStyle & xtFontGUI) || (pLF->lfCharSet == SYMBOL_CHARSET))
			csFaceName = m_csSymbol;

		// Create the font to be displayed and initialize the device context.
		CFont font;
		font.CreateFontIndirect(pLF);

		// Draw the text item.
		CXTPFontDC fontDC(pDC, &font);
		pDC->DrawText(csFaceName, rcText, DT_VCENTER | DT_SINGLELINE);
	}
}

void CXTFontListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	ASSERT(lpMIS->CtlType == ODT_LISTBOX);
	lpMIS->itemHeight = (::GetSystemMetrics(SM_CYVTHUMB)-::GetSystemMetrics(SM_CYEDGE));
	lpMIS->itemWidth = 0;
}

int CXTFontListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
{
	ASSERT(lpCIS->CtlType == ODT_LISTBOX);

	int iItem1 = (int)(WORD)lpCIS->itemID1;
	if (iItem1 == -1)
	{
		return -1;
	}

	CString strItem1;
	GetText(iItem1, strItem1);

	int iItem2 = (int)(WORD)lpCIS->itemID2;
	if (iItem2 == -1)
	{
		return -1;
	}

	CString strItem2;
	GetText(iItem2, strItem2);

	return strItem1.Collate(strItem2);
}

void CXTFontListBox::Initialize(bool /*bAutoFont = true*/)
{
	CXTListBox::Initialize(false);

	CXTFontList& fontList = CXTFontEnum::Get().GetFontList();

	POSITION pos;
	for (pos = fontList.GetHeadPosition(); pos; fontList.GetNext(pos))
	{
		CXTLogFont& lf = fontList.GetAt(pos);
		int iIndex = AddString(lf.lfFaceName);

		SetItemDataPtr(iIndex, &lf);
	}
}

bool CXTFontListBox::GetSelFont(CXTLogFont& lf)
{
	int iCurSel = GetCurSel();
	if (iCurSel == LB_ERR)
	{
		return false;
	}

	CXTLogFont* pLF = (CXTLogFont*)GetItemDataPtr(iCurSel);
	if (pLF == NULL)
	{
		return false;
	}

	MEMCPY_S((void*)&lf, (const void*)pLF, sizeof(CXTLogFont));

	return true;
}

bool CXTFontListBox::GetSelFont(CString& strFaceName)
{
	CXTLogFont lf;
	if (GetSelFont(lf))
	{
		strFaceName = lf.lfFaceName;
		return true;
	}
	return false;
}

bool CXTFontListBox::SetSelFont(CXTLogFont& lf)
{
	return SetSelFont(lf.lfFaceName);
}

bool CXTFontListBox::SetSelFont(const CString& strFaceName)
{
	int iCount = GetCount();
	int i;
	for (i = 0; i < iCount; ++i)
	{
		CXTLogFont* pLF = (CXTLogFont*)GetItemDataPtr(i);
		if (pLF != NULL)
		{
			if (strFaceName.CompareNoCase(pLF->lfFaceName) == 0)
			{
				SetCurSel(i);
				return true;
			}
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////
// CXTFontCombo
///////////////////////////////////////////////////////////////////////////


CXTFontCombo::CXTFontCombo()
{
	m_bFlatLook = TRUE;
	m_csSymbol  = _T("AaBbCc");
	m_dwStyle   = xtFontBoth;
}

CXTFontCombo::~CXTFontCombo()
{

}

IMPLEMENT_DYNAMIC(CXTFontCombo, CXTFlatComboBox)

BEGIN_MESSAGE_MAP(CXTFontCombo, CXTFlatComboBox)
	//{{AFX_MSG_MAP(CXTFontCombo)
	ON_CONTROL_REFLECT_EX(CBN_DROPDOWN, OnDropDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXTFontCombo::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC*  pDC       = CDC::FromHandle(lpDIS->hDC);
	UINT  itemState = lpDIS->itemState;
	UINT  itemID    = lpDIS->itemID;
	CRect rcItem    = lpDIS->rcItem;

	if (itemID == (UINT)-1)
	{
		return;
	}

	CXTLogFont* pLF = (CXTLogFont*)lpDIS->itemData;
	if (pLF == NULL)
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

	CString csFaceName = pLF->lfFaceName;

	// true type or device flag set by SetItemData
	if (pLF->dwType & TRUETYPE_FONTTYPE)
	{
		m_ilFontType.Draw(pDC, 1, rcItem.TopLeft(), ILD_NORMAL);
	}

	// Define the size of the text area to draw
	CRect rcText(rcItem);
	rcText.left += BMW + 6;

	pDC->SetBkMode(TRANSPARENT);

	if (m_dwStyle & xtFontGUI)
	{
		CXTPFontDC fontDC(pDC, &XTAuxData().font);
		pDC->DrawText(csFaceName, rcText, DT_VCENTER | DT_SINGLELINE);
	}

	if (m_dwStyle & xtFontSample)
	{
		if (m_dwStyle & xtFontGUI)
			rcText.left = rcText.left + CXTFontEnum::Get().GetMaxWidth() + 5;

		if ((m_dwStyle & xtFontGUI) || (pLF->lfCharSet == SYMBOL_CHARSET))
			csFaceName = m_csSymbol;

		// Create the font to be displayed and initialize the device context.
		CFont font;
		font.CreateFontIndirect(pLF);

		// Draw the text item.
		CXTPFontDC fontDC(pDC, &font);
		pDC->DrawText(csFaceName, rcText, DT_VCENTER | DT_SINGLELINE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTFontCombo diagnostics

#ifdef _DEBUG
void CXTFontCombo::AssertValid() const
{
	CXTFlatComboBox::AssertValid();

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);

	// combo box is owner drawn, and must be created with the
	// following styles:

	ASSERT(dwStyle & CBS_OWNERDRAWFIXED);
	ASSERT(dwStyle & CBS_DROPDOWN);
	ASSERT(dwStyle & CBS_SORT);
	ASSERT(dwStyle & CBS_HASSTRINGS);
}
#endif

void CXTFontCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	ASSERT(lpMIS->CtlType == ODT_COMBOBOX);
	lpMIS->itemHeight = (::GetSystemMetrics(SM_CYVTHUMB)-::GetSystemMetrics(SM_CYEDGE));
	lpMIS->itemWidth = 0;
}

int CXTFontCombo::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
{
	ASSERT_VALID(this);
	ASSERT(lpCIS->CtlType == ODT_LISTBOX);

	int iItem1 = (int)(WORD)lpCIS->itemID1;
	if (iItem1 == -1)
	{
		return -1;
	}

	CString strItem1;
	GetLBText(iItem1, strItem1);

	int iItem2 = (int)(WORD)lpCIS->itemID2;
	if (iItem2 == -1)
	{
		return -1;
	}

	CString strItem2;
	GetLBText(iItem2, strItem2);

	return strItem1.Collate(strItem2);
}

void CXTFontCombo::InitControl(LPCTSTR lpszFaceName, UINT nWidth/*= 0*/, BOOL bEnable/*= TRUE*/)
{
	ASSERT_VALID(this);
	ResetContent();

	// MFCBUG: adjust height so display is the same as non-owner drawn
	// CComboBoxes. MFC adjusts the height of an owner-drawn CComboBox
	// 2-3 pixels larger than a non owner-drawn combo.

	SetItemHeight(-1, (::GetSystemMetrics(SM_CYVTHUMB)-::GetSystemMetrics(SM_CYEDGE)));

	if (nWidth != 0)
	{
		SetDroppedWidth(nWidth);
	}

	// moved from constructor so resource is loaded when this gets called
	if (!m_ilFontType.m_hImageList)
	{
		VERIFY(m_ilFontType.Create(
			XT_IDB_FONTTYPE, BMW, 1, RGB(255, 0, 255)));
	}

	EnableAutoCompletion(bEnable);

	CXTFontList& fontList = CXTFontEnum::Get().GetFontList();

	POSITION pos;
	for (pos = fontList.GetHeadPosition(); pos; fontList.GetNext(pos))
	{
		CXTLogFont& lf = fontList.GetAt(pos);
		int iIndex = AddString(lf.lfFaceName);

		SetItemDataPtr(iIndex, &lf);
	}

	if (lpszFaceName && _tcslen(lpszFaceName))
	{
		SetSelFont(lpszFaceName);
	}
}

bool CXTFontCombo::GetSelFont(CXTLogFont& lf)
{
	int iCurSel = GetCurSel();
	if (iCurSel == CB_ERR)
	{
		CString strWindowText;
		GetWindowText(strWindowText);

		iCurSel = SelectString(-1, strWindowText);

		if (iCurSel == CB_ERR)
		{
			return false;
		}
	}

	CXTLogFont* pLF = (CXTLogFont*)GetItemDataPtr(iCurSel);
	if (pLF == NULL)
	{
		return false;
	}

	MEMCPY_S((void*)&lf, (const void*)pLF, sizeof(CXTLogFont));

	return true;
}

bool CXTFontCombo::GetSelFont(CString& strFaceName)
{
	CXTLogFont lf;
	if (GetSelFont(lf))
	{
		strFaceName = lf.lfFaceName;
		return true;
	}
	return false;
}

bool CXTFontCombo::SetSelFont(CXTLogFont& lf)
{
	return SetSelFont(lf.lfFaceName);
}

bool CXTFontCombo::SetSelFont(const CString& strFaceName)
{
	int iCount = GetCount();
	int i;
	for (i = 0; i < iCount; ++i)
	{
		CXTLogFont* pLF = (CXTLogFont*)GetItemDataPtr(i);
		if (pLF != NULL)
		{
			if (strFaceName.CompareNoCase(pLF->lfFaceName) == 0)
			{
				SetCurSel(i);
				return true;
			}
		}
	}

	return false;
}

BOOL CXTFontCombo::OnDropDown()
{
	GetSelFont(m_csSelected);
	return FALSE; // continue routing.
}

void CXTFontCombo::NotifyOwner(UINT nCode)
{
	CWnd* pWndOwner = GetOwner();
	if (::IsWindow(pWndOwner->GetSafeHwnd()))
	{
		pWndOwner->SendMessage(WM_COMMAND,
			MAKEWPARAM(GetDlgCtrlID(), nCode), (LPARAM)m_hWnd);
	}
}

BOOL CXTFontCombo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP || pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
			{
				if (GetDroppedState() == TRUE)
				{
					ShowDropDown(FALSE);
					SetSelFont(m_csSelected);
					NotifyOwner(CBN_SELENDCANCEL);
				}
				return TRUE;
			}

		case VK_SPACE:
		case VK_RETURN:
			{
				if (GetDroppedState() == TRUE)
				{
					ShowDropDown(FALSE);
					NotifyOwner(CBN_SELENDOK);
				}
				return TRUE;
			}

		case VK_UP:
		case VK_DOWN:
			{
				if (GetDroppedState() == FALSE)
				{
					ShowDropDown(TRUE);
					return TRUE;
				}
				break;
			}
		}
	}

	return CXTFlatComboBox::PreTranslateMessage(pMsg);
}
