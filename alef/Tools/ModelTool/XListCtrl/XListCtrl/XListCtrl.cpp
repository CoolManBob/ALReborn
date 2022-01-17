// XListCtrl.cpp  Version 1.4 - article available at www.codeproject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// History
//     Version 1.4 - 2006 September 1
//     - See article for changes
//
//     Version 1.3 - 2005 February 9
//     - See article for changes
//
//     Version 1.0 - 2002 February 4
//     - Initial public release
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XListCtrl.h"
#include "SortCStringArray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_COMBO_SELECTION  = ::RegisterWindowMessage(_T("WM_XLISTCTRL_COMBO_SELECTION"));
XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_EDIT_END         = ::RegisterWindowMessage(_T("WM_XLISTCTRL_EDIT_END"));
XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_CHECKBOX_CLICKED = ::RegisterWindowMessage(_T("WM_XLISTCTRL_CHECKBOX_CLICKED"));

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl

BEGIN_MESSAGE_MAP(CXListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CXListCtrl)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_NCLBUTTONDOWN()
	//}}AFX_MSG_MAP
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	ON_WM_TIMER()
	ON_REGISTERED_MESSAGE(WM_XCOMBOLIST_VK_ESCAPE, OnComboEscape)
	ON_REGISTERED_MESSAGE(WM_XCOMBOLIST_COMPLETE, OnComboComplete)
#endif
#ifndef NO_XLISTCTRL_TOOL_TIPS
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
#endif
	ON_REGISTERED_MESSAGE(WM_XEDIT_KILL_FOCUS, OnXEditKillFocus)
	ON_REGISTERED_MESSAGE(WM_XEDIT_VK_ESCAPE, OnXEditEscape)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXListCtrl::CXListCtrl()
{

#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	m_bComboIsClicked       = FALSE;
	m_nComboItem            = 0;
	m_nComboSubItem         = 0;
	m_pCombo                = NULL;
	m_bFontIsCreated        = FALSE;
#endif

	m_dwExtendedStyleX      = 0;
	m_bHeaderIsSubclassed   = FALSE;
	m_bUseEllipsis          = TRUE;			//+++
	m_bListModified         = FALSE;		//+++
	m_bInitialCheck         = FALSE;
	m_strInitialString      = _T("");
	m_nPadding              = 5;			//+++
	m_pEdit                 = NULL;			//+++
	m_nEditItem             = 0;			//+++
	m_nEditSubItem          = 0;			//+++

	GetColors();
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXListCtrl::~CXListCtrl()
{
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	if (m_pCombo)
		delete m_pCombo;
#endif
	if (m_pEdit)
		delete m_pEdit;
}

///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CXListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	// for Dialog based applications, this is a good place
	// to subclass the header control because the OnCreate()
	// function does not get called.

	SubclassHeaderControl();
}

///////////////////////////////////////////////////////////////////////////////
// OnCreate
int CXListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
	{
		ASSERT(FALSE);
		return -1;
	}

	// When the CXListCtrl object is created via a call to Create(), instead
	// of via a dialog box template, we must subclass the header control
	// window here because it does not exist when the PreSubclassWindow()
	// function is called.

	SubclassHeaderControl();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// SubclassHeaderControl
void CXListCtrl::SubclassHeaderControl()
{
	if (m_bHeaderIsSubclassed)
		return;

	// if the list control has a header control window, then
	// subclass it

	// Thanks to Alberto Gattegno and Alon Peleg  and their article
	// "A Multiline Header Control Inside a CListCtrl" for easy way
	// to determine if the header control exists.

	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if (pHeader)
	{
		VERIFY(m_HeaderCtrl.SubclassWindow(pHeader->m_hWnd));
		m_bHeaderIsSubclassed = TRUE;
		m_HeaderCtrl.SetListCtrl(this);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnClick
BOOL CXListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnClick\n"));

	pNMHDR   = pNMHDR;
	*pResult = 0;
	return FALSE;		// return FALSE to send message to parent also -
						// NOTE:  MSDN documentation is incorrect
}

///////////////////////////////////////////////////////////////////////////////
// OnCustomDraw
void CXListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.

	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
	{
		// This is the prepaint stage for a subitem. Here's where we set the
		// item's text and background colors. Our return value will tell
		// Windows to draw the subitem itself, but it will use the new colors
		// we set here.

		int nItem = static_cast<int> (pLVCD->nmcd.dwItemSpec);
		int nSubItem = pLVCD->iSubItem;

		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) pLVCD->nmcd.lItemlParam;
		ASSERT(pXLCD);

		COLORREF crText  = m_crWindowText;
		COLORREF crBkgnd = m_crWindow;

		if (pXLCD)
		{
			crText  = pXLCD[nSubItem].crText;
			crBkgnd = pXLCD[nSubItem].crBackground;

			if (!pXLCD[0].bEnabled)
				crText = m_crGrayText;
		}

		// store the colors back in the NMLVCUSTOMDRAW struct
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBkgnd;

		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		CRect rect;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);

		if (pXLCD && (pXLCD[nSubItem].bShowProgress))
		{
			DrawProgress(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else if (pXLCD && (pXLCD[nSubItem].nCheckedState != -1))
		{
			DrawCheckbox(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else
		{
			rect.left += DrawImage(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			DrawText(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawProgress
void CXListCtrl::DrawProgress(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF /*crBkgnd*/,
							  CRect& rect,
							  XLISTCTRLDATA *pXLCD)
{
	UNUSED_ALWAYS(nItem);

	ASSERT(pDC);
	ASSERT(pXLCD);

	if (rect.IsRectEmpty())
	{
		return;
	}

	rect.bottom -= 1;
	rect.left += 1;		// leave margin in case row is highlighted

	// fill interior with light gray
	pDC->FillSolidRect(rect, RGB(224,224,224));

	// draw border
	pDC->Draw3dRect(&rect, RGB(0,0,0), m_crBtnShadow);

	if (pXLCD[nSubItem].nProgressPercent > 0)
	{
		// draw progress bar and text

		CRect LeftRect, RightRect;
		LeftRect = rect;
		LeftRect.left += 1;
		LeftRect.top += 1;
		LeftRect.bottom -= 1;
		RightRect = LeftRect;
		int w = (LeftRect.Width() * pXLCD[nSubItem].nProgressPercent) / 100;
		LeftRect.right = LeftRect.left + w - 1;
		RightRect.left = LeftRect.right;
		pDC->FillSolidRect(LeftRect, m_crHighLight);

		if (pXLCD[nSubItem].bShowProgressMessage)
		{
			CString str, format;
			format = pXLCD[nSubItem].strProgressMessage;
			if (format.IsEmpty())
				str.Format(_T("%d%%"), pXLCD[nSubItem].nProgressPercent);
			else
				str.Format(format, pXLCD[nSubItem].nProgressPercent);

			pDC->SetBkMode(TRANSPARENT);

			CRect TextRect;
			TextRect = rect;
			TextRect.DeflateRect(1, 1);

			CRgn rgn;
			rgn.CreateRectRgn(LeftRect.left, LeftRect.top, LeftRect.right, 
					LeftRect.bottom);
			pDC->SelectClipRgn(&rgn);
			pDC->SetTextColor(m_crHighLightText);//crBkgnd);
			pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			rgn.DeleteObject();
			rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, 
					RightRect.bottom);
			pDC->SelectClipRgn(&rgn);
			pDC->SetTextColor(crText);
			pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			rgn.DeleteObject();
			pDC->SelectClipRgn(NULL);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawCheckbox
void CXListCtrl::DrawCheckbox(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF crBkgnd,
							  CRect& rect,
							  XLISTCTRLDATA *pXLCD)
{
	ASSERT(pDC);
	ASSERT(pXLCD);

	if (rect.IsRectEmpty())
	{
		return;
	}

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rect, crBkgnd);

	CRect chkboxrect;
	chkboxrect = rect;
	chkboxrect.bottom -= 1;
	chkboxrect.left += 9;		// line up checkbox with header checkbox
	chkboxrect.right = chkboxrect.left + chkboxrect.Height();	// width = height

	CString str;
	str = GetItemText(nItem, nSubItem);

	if (str.IsEmpty())
	{
		// center the checkbox

		chkboxrect.left = rect.left + rect.Width()/2 - chkboxrect.Height()/2 - 1;
		chkboxrect.right = chkboxrect.left + chkboxrect.Height();
	}

	// fill rect around checkbox with white
	pDC->FillSolidRect(&chkboxrect, m_crWindow);

	// draw border
	CBrush brush(RGB(51,102,153));
	pDC->FrameRect(&chkboxrect, &brush);

	if (pXLCD[nSubItem].nCheckedState == 1)
	{
		CPen *pOldPen = NULL;

		CPen graypen(PS_SOLID, 1, m_crGrayText);
		CPen blackpen(PS_SOLID, 1, RGB(51,153,51));

		if (pXLCD[0].bEnabled)
			pOldPen = pDC->SelectObject(&blackpen);
		else
			pOldPen = pDC->SelectObject(&graypen);

		// draw the checkmark
		int x = chkboxrect.left + 9;
		ASSERT(x < chkboxrect.right);
		int y = chkboxrect.top + 3;
		int i;
		for (i = 0; i < 4; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);
			x--;
			y++;
		}
		for (i = 0; i < 3; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);
			x--;
			y--;
		}

		if (pOldPen)
			pDC->SelectObject(pOldPen);
	}

	if (!str.IsEmpty())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		CRect textrect;
		textrect = rect;
		textrect.left = chkboxrect.right + 4;

		UINT nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;	//+++
		if (m_bUseEllipsis)
			nFormat |= DT_END_ELLIPSIS;

		pDC->DrawText(str, &textrect, nFormat);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetDrawColors
void CXListCtrl::GetDrawColors(int nItem,
							   int nSubItem,
							   COLORREF& colorText,
							   COLORREF& colorBkgnd)
{
	DWORD dwStyle    = GetStyle();
	DWORD dwExStyle  = GetExtendedStyle();

	COLORREF crText  = colorText;
	COLORREF crBkgnd = colorBkgnd;

	if (GetItemState(nItem, LVIS_SELECTED))
	{
		if (dwExStyle & LVS_EX_FULLROWSELECT)
		{
			// selected?  if so, draw highlight background
			crText  = m_crHighLightText;
			crBkgnd = m_crHighLight;

			// has focus?  if not, draw gray background
			if (m_hWnd != ::GetFocus())
			{
				if (dwStyle & LVS_SHOWSELALWAYS)
				{
					crText  = m_crWindowText;
					crBkgnd = m_crBtnFace;
				}
				else
				{
					crText  = colorText;
					crBkgnd = colorBkgnd;
				}
			}
		}
		else	// not full row select
		{
			if (nSubItem == 0)
			{
				// selected?  if so, draw highlight background
				crText  = m_crHighLightText;
				crBkgnd = m_crHighLight;

				// has focus?  if not, draw gray background
				if (m_hWnd != ::GetFocus())
				{
					if (dwStyle & LVS_SHOWSELALWAYS)
					{
						crText  = m_crWindowText;
						crBkgnd = m_crBtnFace;
					}
					else
					{
						crText  = colorText;
						crBkgnd = colorBkgnd;
					}
				}
			}
		}
	}

	colorText = crText;
	colorBkgnd = crBkgnd;
}

///////////////////////////////////////////////////////////////////////////////
// DrawImage
int CXListCtrl::DrawImage(int nItem,
						  int nSubItem,
						  CDC* pDC,
						  COLORREF crText,
						  COLORREF crBkgnd,
						  CRect rect,
  						  XLISTCTRLDATA *pXLCD)
{
	if (rect.IsRectEmpty())
	{
		return 0;
	}

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rect, crBkgnd);

	int nWidth = 0;
	rect.left += m_HeaderCtrl.GetSpacing();

	CImageList* pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		SIZE sizeImage;
		sizeImage.cx = sizeImage.cy = 0;
		IMAGEINFO info;

		int nImage = -1;
		if (pXLCD)
			nImage = pXLCD[nSubItem].nImage;

		if (nImage == -1)
			return 0;

		if (pImageList->GetImageInfo(nImage, &info))
		{
			sizeImage.cx = info.rcImage.right - info.rcImage.left;
			sizeImage.cy = info.rcImage.bottom - info.rcImage.top;
		}

		if (nImage >= 0)
		{
			if (rect.Width() > 0)
			{
				POINT point;

				point.y = rect.CenterPoint().y - (sizeImage.cy >> 1);
				point.x = rect.left;

				SIZE size;
				size.cx = rect.Width() < sizeImage.cx ? rect.Width() : sizeImage.cx;
				size.cy = rect.Height() < sizeImage.cy ? rect.Height() : sizeImage.cy;

				// save image list background color
				COLORREF rgb = pImageList->GetBkColor();

				// set image list background color
				pImageList->SetBkColor(crBkgnd);
				pImageList->DrawIndirect(pDC, nImage, point, size, CPoint(0, 0));
				pImageList->SetBkColor(rgb);

				nWidth = sizeImage.cx + m_HeaderCtrl.GetSpacing();
			}
		}
	}

	return nWidth;
}

///////////////////////////////////////////////////////////////////////////////
// DrawText
void CXListCtrl::DrawText(int nItem,
						  int nSubItem,
						  CDC *pDC,
						  COLORREF crText,
						  COLORREF crBkgnd,
						  CRect& rect,
						  XLISTCTRLDATA *pXLCD)
{
	ASSERT(pDC);
	ASSERT(pXLCD);

	if (rect.IsRectEmpty())
	{
		return;
	}

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rect, crBkgnd);

	CString str;
	str = GetItemText(nItem, nSubItem);

	if (!str.IsEmpty())
	{
		// get text justification
		HDITEM hditem;
		hditem.mask = HDI_FORMAT;
		m_HeaderCtrl.GetItem(nSubItem, &hditem);
		int nFmt = hditem.fmt & HDF_JUSTIFYMASK;
		UINT nFormat = DT_VCENTER | DT_SINGLELINE;
		if (m_bUseEllipsis)								//+++
			nFormat |= DT_END_ELLIPSIS;
		if (nFmt == HDF_CENTER)
			nFormat |= DT_CENTER;
		else if (nFmt == HDF_LEFT)
			nFormat |= DT_LEFT;
		else
			nFormat |= DT_RIGHT;

		CFont *pOldFont = NULL;
		CFont boldfont;

		// check if bold specified for subitem
		if (pXLCD && pXLCD[nSubItem].bBold)
		{
			CFont *font = pDC->GetCurrentFont();
			if (font)
			{
				LOGFONT lf;
				font->GetLogFont(&lf);
				lf.lfWeight = FW_BOLD;
				boldfont.CreateFontIndirect(&lf);
				pOldFont = pDC->SelectObject(&boldfont);
			}
		}
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		rect.DeflateRect(m_nPadding, 0);			//+++
		pDC->DrawText(str, &rect, nFormat);
		rect.InflateRect(m_nPadding, 0);			//+++
		if (pOldFont)
			pDC->SelectObject(pOldFont);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetSubItemRect
BOOL CXListCtrl::GetSubItemRect(int nItem,
								int nSubItem,
								int nArea,
								CRect& rect)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	BOOL bRC = CListCtrl::GetSubItemRect(nItem, nSubItem, nArea, rect);

	// if nSubItem == 0, the rect returned by CListCtrl::GetSubItemRect
	// is the entire row, so use left edge of second subitem

	if (nSubItem == 0)
	{
		if (GetColumns() > 1)
		{
			CRect rect1;
			bRC = GetSubItemRect(nItem, 1, LVIR_BOUNDS, rect1);
			rect.right = rect1.left;
		}
	}

	//+++
	if (nSubItem == 0)
	{
		if (GetColumns() > 1)
		{
			CRect rect1;
			// in case 2nd col width = 0
			for (int i = 1; i < GetColumns(); i++)
			{
				bRC = GetSubItemRect(nItem, i, LVIR_BOUNDS, rect1);
				if (rect1.Width() > 0)
				{
					rect.right = rect1.left;
					break;
				}
			}
		}
	}

	return bRC;
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CXListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnLButtonDown\n"));

	int nItem = -1;

	//+++
	LVHITTESTINFO lvhit;
	lvhit.pt = point;
	SubItemHitTest(&lvhit);
	if (lvhit.flags & LVHT_ONITEMLABEL)
	{
		XLISTCTRL_TRACE(_T("lvhit.iItem=%d  lvhit.iSubItem=%d  ~~~~~\n"), lvhit.iItem, lvhit.iSubItem);

		nItem = lvhit.iItem;
	}

	if (nItem == -1)
	{
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
		if (m_pCombo)
			OnComboEscape(0, 0);
#endif
	}
	else
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (!pXLCD)
		{
			return;
		}

		if (!pXLCD[0].bEnabled)
			return;

		CRect rect;

		int nSubItem = -1;

		// check if a subitem checkbox was clicked

		for (int i = 0; i < GetColumns(); i++)
		{
			GetSubItemRect(nItem, i, LVIR_BOUNDS, rect);
			if (rect.PtInRect(point))
			{
				nSubItem = i;
				break;
			}
		}

		if (nSubItem == -1)
		{
			// -1 = no checkbox for this subitem

#ifndef DO_NOT_INCLUDE_XCOMBOLIST
			if (m_pCombo)
			{
				OnComboEscape(0, 0);
			}
#endif
		}
		else
		{
			if (pXLCD[nSubItem].nCheckedState >= 0)
			{
				int nChecked = pXLCD[nSubItem].nCheckedState;

				nChecked = (nChecked == 0) ? 1 : 0;

				pXLCD[nSubItem].nCheckedState = nChecked;
				pXLCD[nSubItem].bModified = TRUE;
				m_bListModified = TRUE;

				UpdateSubItem(nItem, nSubItem);

				CWnd *pWnd = GetParent();
				if (!pWnd)
					pWnd = GetOwner();
				if (pWnd && ::IsWindow(pWnd->m_hWnd))
					pWnd->SendMessage(WM_XLISTCTRL_CHECKBOX_CLICKED, 
								nItem, nSubItem);

				// now update checkbox in header

				// -1 = no checkbox in column header
				if (GetHeaderCheckedState(nSubItem) != XHEADERCTRL_NO_IMAGE)
				{
					int nCheckedCount = CountCheckedItems(nSubItem);

					if (nCheckedCount == GetItemCount())
						SetHeaderCheckedState(nSubItem, XHEADERCTRL_CHECKED_IMAGE);
					else
						SetHeaderCheckedState(nSubItem, XHEADERCTRL_UNCHECKED_IMAGE);
				}
			}
			else if (pXLCD[nSubItem].bCombo)
			{
				CListCtrl::OnLButtonDown(nFlags, point);
				DrawComboBox(nItem, nSubItem);
			}
			else if (pXLCD[nSubItem].bEdit)
			{
				CListCtrl::OnLButtonDown(nFlags, point);
				DrawEdit(nItem, nSubItem);
			}
		}
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnRButtonDown - added so we can ignore disabled items
void CXListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnRButtonDown\n"));

	int nItem = -1;

	//+++
	LVHITTESTINFO lvhit;
	lvhit.pt = point;
	SubItemHitTest(&lvhit);
	if (lvhit.flags & LVHT_ONITEMLABEL)
	{
		XLISTCTRL_TRACE(_T("lvhit.iItem=%d  lvhit.iSubItem=%d  ~~~~~\n"), lvhit.iItem, lvhit.iSubItem);

		nItem = lvhit.iItem;
	}

	if (nItem != -1)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (!pXLCD)
		{
			return;
		}

		if (!pXLCD[0].bEnabled)
			return;
	}

	CListCtrl::OnRButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnNcLButtonDown
void CXListCtrl::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	TRACE(_T("in CXListCtrl::OnNcLButtonDown\n"));
	
	if (m_pEdit)
	{
		OnXEditKillFocus(0, 0);
	}

	CListCtrl::OnNcLButtonDown(nHitTest, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXListCtrl::OnPaint()
{
    Default();
	if (GetItemCount() <= 0)
	{
		CDC* pDC = GetDC();
		int nSavedDC = pDC->SaveDC();

		CRect rc;
		GetWindowRect(&rc);
		ScreenToClient(&rc);
		CHeaderCtrl* pHC = GetHeaderCtrl();
		if (pHC != NULL)
		{
			CRect rcH;
			pHC->GetItemRect(0, &rcH);
			rc.top += rcH.bottom;
		}
		rc.top += 10;
		CString strText;
		strText = _T("There are no items to show in this view.");

		COLORREF crText = m_crWindowText;
		COLORREF crBkgnd = m_crWindow;

		CBrush brush(crBkgnd);
		pDC->FillRect(rc, &brush);

		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		pDC->SelectStockObject(ANSI_VAR_FONT);
		pDC->DrawText(strText, -1, rc, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);
		pDC->RestoreDC(nSavedDC);
		ReleaseDC(pDC);
	}
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXListCtrl::InsertItem(const LVITEM* pItem)
{
	ASSERT(pItem->iItem >= 0);
	if (pItem->iItem < 0)
		return -1;

	int index = CListCtrl::InsertItem(pItem);

	if (index < 0)
		return index;

	XLISTCTRLDATA *pXLCD = new XLISTCTRLDATA [GetColumns()];
	ASSERT(pXLCD);
	if (!pXLCD)
		return -1;

	pXLCD[0].crText       = m_crWindowText;
	pXLCD[0].crBackground = m_crWindow;
	pXLCD[0].nImage       = pItem->iImage;
	pXLCD[0].dwItemData   = pItem->lParam;	//+++

	CListCtrl::SetItemData(index, (DWORD) pXLCD);

	return index;
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXListCtrl::InsertItem(int nItem, LPCTSTR lpszItem)
{
	ASSERT(nItem >= 0);
	if (nItem < 0)
		return -1;

	return InsertItem(nItem,
					  lpszItem,
					  m_crWindowText,
					  m_crWindow);
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXListCtrl::InsertItem(int nItem,
						   LPCTSTR lpszItem,
						   COLORREF crText,
						   COLORREF crBackground)
{
	ASSERT(nItem >= 0);
	if (nItem < 0)
		return -1;

	int index = CListCtrl::InsertItem(nItem, lpszItem);

	if (index < 0)
		return index;

	XLISTCTRLDATA *pXLCD = new XLISTCTRLDATA [GetColumns()];
	ASSERT(pXLCD);
	if (!pXLCD)
		return -1;

	pXLCD[0].crText       = crText;
	pXLCD[0].crBackground = crBackground;
	pXLCD[0].nImage       = -1;

	CListCtrl::SetItemData(index, (DWORD) pXLCD);

	return index;
}

///////////////////////////////////////////////////////////////////////////////
// SetItem
int CXListCtrl::SetItem(const LVITEM* pItem)
{
	ASSERT(pItem->iItem >= 0);
	if (pItem->iItem < 0)
		return -1;

	BOOL rc = CListCtrl::SetItem(pItem);

	if (!rc)
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(pItem->iItem);
	if (pXLCD)
	{
		pXLCD[pItem->iSubItem].nImage = pItem->iImage;
		UpdateSubItem(pItem->iItem, pItem->iSubItem);
		rc = TRUE;
	}
	else
	{
		rc = FALSE;
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemImage
BOOL CXListCtrl::SetItemImage(int nItem, int nSubItem, int nImage)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	BOOL rc = TRUE;

	if (nItem < 0)
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		pXLCD[nSubItem].nImage = nImage;
	}

	UpdateSubItem(nItem, nSubItem);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemText
BOOL CXListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	BOOL rc = CListCtrl::SetItemText(nItem, nSubItem, lpszText);

	UpdateSubItem(nItem, nSubItem);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemText
//
// This function will set the text and colors for a subitem.  If lpszText
// is NULL, only the colors will be set.  If a color value is -1, the display
// color will be set to the default Windows color.
//
BOOL CXListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText,
					COLORREF crText, COLORREF crBackground)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	BOOL rc = TRUE;

	if (nItem < 0)
		return FALSE;

	if (lpszText)
		rc = CListCtrl::SetItemText(nItem, nSubItem, lpszText);

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		pXLCD[nSubItem].crText       = (crText == -1) ? m_crWindowText : crText;
		pXLCD[nSubItem].crBackground = (crBackground == -1) ? m_crWindow : crBackground;
	}

	UpdateSubItem(nItem, nSubItem);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// DeleteItem
BOOL CXListCtrl::DeleteItem(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
		delete [] pXLCD;
	CListCtrl::SetItemData(nItem, 0);
	return CListCtrl::DeleteItem(nItem);
}

///////////////////////////////////////////////////////////////////////////////
// DeleteAllItems
BOOL CXListCtrl::DeleteAllItems()
{
	int n = GetItemCount();
	for (int i = 0; i < n; i++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(i);
		if (pXLCD)
			delete [] pXLCD;
		CListCtrl::SetItemData(i, 0);
	}

	return CListCtrl::DeleteAllItems();
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CXListCtrl::OnDestroy()
{
	int n = GetItemCount();
	for (int i = 0; i < n; i++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(i);
		if (pXLCD)
			delete [] pXLCD;
		CListCtrl::SetItemData(i, 0);
	}

	m_bHeaderIsSubclassed = FALSE;

	CListCtrl::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////////
// SetEdit
BOOL CXListCtrl::SetEdit(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[nSubItem].bEdit = TRUE;

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetProgress
//
// This function creates a progress bar in the specified subitem.  The
// UpdateProgress function may then be called to update the progress
// percent.  If bShowProgressText is TRUE, either the default text
// of "n%" or the custom percent text (lpszProgressText) will be
// displayed.  If bShowProgressText is FALSE, only the progress bar
// will be displayed, with no text.
//
// Note that the lpszProgressText string should include the format
// specifier "%d":  e.g., "Pct %d%%"
//
BOOL CXListCtrl::SetProgress(int nItem,
							 int nSubItem,
							 BOOL bShowProgressText /*= TRUE*/,
							 LPCTSTR lpszProgressText /*= NULL*/)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[nSubItem].bShowProgress        = TRUE;
	pXLCD[nSubItem].nProgressPercent     = 0;
	pXLCD[nSubItem].bShowProgressMessage = bShowProgressText;
	pXLCD[nSubItem].strProgressMessage   = lpszProgressText;

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// DeleteProgress
void CXListCtrl::DeleteProgress(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return;
	}

	pXLCD[nSubItem].bShowProgress = FALSE;
	pXLCD[nSubItem].nProgressPercent = 0;

	UpdateSubItem(nItem, nSubItem);
}

///////////////////////////////////////////////////////////////////////////////
// UpdateProgress
void CXListCtrl::UpdateProgress(int nItem, int nSubItem, int nPercent)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	ASSERT(nPercent >= 0 && nPercent <= 100);

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return;
	}

	pXLCD[nSubItem].nProgressPercent = nPercent;

	UpdateSubItem(nItem, nSubItem);
}

///////////////////////////////////////////////////////////////////////////////
// SetCheckbox
BOOL CXListCtrl::SetCheckbox(int nItem, int nSubItem, int nCheckedState)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;
	ASSERT(nCheckedState == 0 || nCheckedState == 1 || nCheckedState == -1);

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update checkbox in subitem

	pXLCD[nSubItem].nCheckedState = nCheckedState;

	UpdateSubItem(nItem, nSubItem);

	// now update checkbox in column header

	// -1 = no checkbox in column header
	if (GetHeaderCheckedState(nSubItem) != XHEADERCTRL_NO_IMAGE)
	{
		int nCheckedCount = CountCheckedItems(nSubItem);

		if (nCheckedCount == GetItemCount())
			SetHeaderCheckedState(nSubItem, XHEADERCTRL_CHECKED_IMAGE);
		else
			SetHeaderCheckedState(nSubItem, XHEADERCTRL_UNCHECKED_IMAGE);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetCheckbox
int CXListCtrl::GetCheckbox(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return -1;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return -1;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return -1;
	}

	return pXLCD[nSubItem].nCheckedState;
}

///////////////////////////////////////////////////////////////////////////////
// GetEnabled
//
// Note that GetEnabled and SetEnabled only Get/Set the enabled flag from
// subitem 0, since this is a per-row flag.
//
BOOL CXListCtrl::GetEnabled(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	return pXLCD[0].bEnabled;
}

///////////////////////////////////////////////////////////////////////////////
// SetEnabled
BOOL CXListCtrl::SetEnabled(int nItem, BOOL bEnable)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[0].bEnabled = bEnable;

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);
	InvalidateRect(&rect);
	UpdateWindow();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetBold
BOOL CXListCtrl::SetBold(int nItem, int nSubItem, BOOL bBold)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update bold flag
	pXLCD[nSubItem].bBold = bBold;

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetBold
BOOL CXListCtrl::GetBold(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// return bold flag
	return pXLCD[nSubItem].bBold;
}

///////////////////////////////////////////////////////////////////////////////
// GetModified
BOOL CXListCtrl::GetModified(int nItem, int nSubItem)					//+++
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// return modified flag
	return pXLCD[nSubItem].bModified;
}

///////////////////////////////////////////////////////////////////////////////
// SetModified
void CXListCtrl::SetModified(int nItem, int nSubItem, BOOL bModified)	//+++
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		// set modified flag
		pXLCD[nSubItem].bModified = bModified;
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetItemCheckedState
int CXListCtrl::GetItemCheckedState(int nItem, int nSubItem)		//+++
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return -1;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return -1;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return -1;
	}

	// return checked state
	return pXLCD[nSubItem].nCheckedState;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemCheckedState
void CXListCtrl::SetItemCheckedState(int nItem, int nSubItem, int nCheckedState)		//+++
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		// update checked state
		pXLCD[nSubItem].nCheckedState = nCheckedState;

		UpdateSubItem(nItem, nSubItem);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetItemColors
BOOL CXListCtrl::GetItemColors(int nItem,								//+++
							   int nSubItem, 
							   COLORREF& crText, 
							   COLORREF& crBackground)
{
	crText = RGB(0,0,0);
	crBackground = RGB(0,0,0);
	
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	crText = pXLCD[nSubItem].crText;
	crBackground = pXLCD[nSubItem].crBackground;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemColors
void CXListCtrl::SetItemColors(int nItem,								//+++
							   int nSubItem, 
							   COLORREF crText, 
							   COLORREF crBackground)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		pXLCD[nSubItem].crText = crText;
		pXLCD[nSubItem].crBackground = crBackground;
	}
}

///////////////////////////////////////////////////////////////////////////////
// SetComboBox
//
// Note:  SetItemText may also be used to set the initial combo selection.
//
BOOL CXListCtrl::SetComboBox(int nItem,
							 int nSubItem,
							 BOOL bEnableCombo,
							 CStringArray *psa,		// should not be allocated on stack
							 int nComboListHeight,
							 int nInitialComboSel,
							 BOOL bSort /*= FALSE*/)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;
	ASSERT(psa);
	if (!psa)
		return FALSE;
	ASSERT(nComboListHeight > 0);
	ASSERT(nInitialComboSel >= 0 && nInitialComboSel < psa->GetSize());
	if ((nInitialComboSel < 0) || (nInitialComboSel >= psa->GetSize()))
		nInitialComboSel = 0;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update flag
	pXLCD[nSubItem].bCombo = bEnableCombo;

	if (bEnableCombo)
	{
		// sort CStringArray before setting initial selection
		if (bSort)
			CSortCStringArray::SortCStringArray(psa);

		pXLCD[nSubItem].psa = psa;
		pXLCD[nSubItem].nComboListHeight = nComboListHeight;
		pXLCD[nSubItem].nInitialComboSel = nInitialComboSel;
		pXLCD[nSubItem].bSort = bSort;

		CString str = _T("");

		if ((pXLCD[nSubItem].nInitialComboSel >= 0) &&
			(pXLCD[nSubItem].psa->GetSize() > pXLCD[nSubItem].nInitialComboSel))
		{
			int index = pXLCD[nSubItem].nInitialComboSel;
			str = pXLCD[nSubItem].psa->GetAt(index);
		}

		SetItemText(nItem, nSubItem, str);
	}

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetComboText
//
// Actually this does nothing more than GetItemText()
//
CString	CXListCtrl::GetComboText(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return _T("");
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return _T("");

	CString str;
	str = _T("");

	str = GetItemText(nItem, nSubItem);

	return str;
}

///////////////////////////////////////////////////////////////////////////////
// SetCurSel
BOOL CXListCtrl::SetCurSel(int nItem, BOOL bEnsureVisible /*= FALSE*/)
{
	BOOL bRet = SetItemState(nItem, LVIS_FOCUSED | LVIS_SELECTED,
		LVIS_FOCUSED | LVIS_SELECTED);

	//+++
	if (bEnsureVisible)
		EnsureVisible(nItem, FALSE);

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// GetCurSel - returns selected item number, or -1 if no item selected
//
// Note:  for single-selection lists only
//
int CXListCtrl::GetCurSel()
{
	POSITION pos = GetFirstSelectedItemPosition();
	int nSelectedItem = -1;
	if (pos != NULL)
		nSelectedItem = GetNextSelectedItem(pos);
	return nSelectedItem;
}

///////////////////////////////////////////////////////////////////////////////
// UpdateSubItem
void CXListCtrl::UpdateSubItem(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	CRect rect;
	if (nSubItem == -1)
	{
		GetItemRect(nItem, &rect, LVIR_BOUNDS);
	}
	else
	{
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
	}

	rect.InflateRect(2, 2);

	InvalidateRect(&rect);
	UpdateWindow();
}

///////////////////////////////////////////////////////////////////////////////
// GetColumns
int CXListCtrl::GetColumns()
{
	return GetHeaderCtrl()->GetItemCount();
}

///////////////////////////////////////////////////////////////////////////////
// GetItemData
//
// The GetItemData and SetItemData functions allow for app-specific data
// to be stored, by using an extra field in the XLISTCTRLDATA struct.
//
DWORD CXListCtrl::GetItemData(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return 0;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return 0;
	}

	return pXLCD->dwItemData;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemData
BOOL CXListCtrl::SetItemData(int nItem, DWORD dwData)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD->dwItemData = dwData;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetHeaderCheckedState
//
// The GetHeaderCheckedState and SetHeaderCheckedState may be used to toggle
// the checkbox in a column header.
//     0 = no checkbox
//     1 = unchecked
//     2 = checked
//
int CXListCtrl::GetHeaderCheckedState(int nSubItem)
{
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return -1;

	HDITEM hditem;

	// use the image index (0 or 1) to indicate the checked status
	hditem.mask = HDI_IMAGE;
	m_HeaderCtrl.GetItem(nSubItem, &hditem);
	return hditem.iImage;
}

///////////////////////////////////////////////////////////////////////////////
// SetHeaderCheckedState
BOOL CXListCtrl::SetHeaderCheckedState(int nSubItem, int nCheckedState)
{
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;
	ASSERT(nCheckedState == 0 || nCheckedState == 1 || nCheckedState == 2);

	HDITEM hditem;

	hditem.mask = HDI_IMAGE;
	hditem.iImage = nCheckedState;
	m_HeaderCtrl.SetItem(nSubItem, &hditem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// OnColumnClick
BOOL CXListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnColumnClick\n"));

	NMLISTVIEW* pnmlv = (NMLISTVIEW*)pNMHDR;

	int nSubItem = pnmlv->iSubItem;

	int nCheckedState = GetHeaderCheckedState(nSubItem);

	// 0 = no checkbox
	if (nCheckedState != XHEADERCTRL_NO_IMAGE)
	{
		nCheckedState = (nCheckedState == 1) ? 2 : 1;
		SetHeaderCheckedState(nSubItem, nCheckedState);

		m_HeaderCtrl.UpdateWindow();

		for (int nItem = 0; nItem < GetItemCount(); nItem++)
		{
			XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
			if (!pXLCD)
			{
				continue;
			}

			if (pXLCD[nSubItem].nCheckedState != -1)
			{
				pXLCD[nSubItem].nCheckedState = nCheckedState - 1;
				pXLCD[nSubItem].bModified = TRUE;
				m_bListModified = TRUE;
				UpdateSubItem(nItem, nSubItem);
			}
		}
	}

	*pResult = 0;
	return FALSE;		// return FALSE to send message to parent also -
						// NOTE:  MSDN documentation is incorrect
}

///////////////////////////////////////////////////////////////////////////////
// CountCheckedItems
int CXListCtrl::CountCheckedItems(int nSubItem)
{
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return 0;

	int nCount = 0;

	for (int nItem = 0; nItem < GetItemCount(); nItem++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (!pXLCD)
		{
			continue;
		}

		if (pXLCD[nSubItem].nCheckedState == 1)
			nCount++;
	}

	return nCount;
}

///////////////////////////////////////////////////////////////////////////////
// GetColors
void CXListCtrl::GetColors()
{
	m_cr3DFace              = ::GetSysColor(COLOR_3DFACE);
	m_cr3DHighLight         = ::GetSysColor(COLOR_3DHIGHLIGHT);
	m_cr3DShadow            = ::GetSysColor(COLOR_3DSHADOW);
	m_crActiveCaption       = ::GetSysColor(COLOR_ACTIVECAPTION);
	m_crBtnFace             = ::GetSysColor(COLOR_BTNFACE);
	m_crBtnShadow           = ::GetSysColor(COLOR_BTNSHADOW);
	m_crBtnText             = ::GetSysColor(COLOR_BTNTEXT);
	m_crGrayText            = ::GetSysColor(COLOR_GRAYTEXT);
	m_crHighLight           = ::GetSysColor(COLOR_HIGHLIGHT);
	m_crHighLightText       = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_crInactiveCaption     = ::GetSysColor(COLOR_INACTIVECAPTION);
	m_crInactiveCaptionText = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);
	m_crWindow              = ::GetSysColor(COLOR_WINDOW);
	m_crWindowText          = ::GetSysColor(COLOR_WINDOWTEXT);
}

///////////////////////////////////////////////////////////////////////////////
// OnSysColorChange
void CXListCtrl::OnSysColorChange()
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnSysColorChange\n"));

	CListCtrl::OnSysColorChange();

	GetColors();
}

#ifndef DO_NOT_INCLUDE_XCOMBOLIST

///////////////////////////////////////////////////////////////////////////////
// OnTimer
//
// Timer usage:
//    1 - Unlock window updates - used to avoid flashing after combo is created
//        used to check if combo button needs to be unpressed,set in
//        OnLButtonDown (when combo button is clicked)
//    2 - used to close combo listbox, set in OnComboEscape (user hits Escape
//        or listbox loses focus)
//    3 - used to get combo listbox selection, then close combo listbox,
//        set in OnComboReturn and OnComboLButtonUp (user hits Enter
//        or clicks on item in listbox)
//    4 - used to get combo listbox selection, set in OnComboKeydown (for
//        example, user hits arrow key in listbox)
//
void CXListCtrl::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1)			// timer set when combo is created
	{
		KillTimer(nIDEvent);
		UnlockWindowUpdate();
	}
	else if (nIDEvent == 2)		// close combo listbox
	{
		KillTimer(nIDEvent);

		XLISTCTRL_TRACE(_T("timer 2 ~~~~~\n"));

		if (m_pCombo)
		{
			UpdateSubItem(m_nComboItem, m_nComboSubItem);
			m_pCombo->DestroyWindow();
			delete m_pCombo;
		}
		m_pCombo = NULL;
	}
	else if (nIDEvent == 3)		// get combo listbox selection, then close combo listbox
	{
		KillTimer(nIDEvent);

		XLISTCTRL_TRACE(_T("timer 3 ~~~~~\n"));

		if (m_pCombo)
		{
			CString str;
			int i = m_pCombo->GetCurSel();
			if (i != LB_ERR)
			{
				m_pCombo->GetLBText(i, str);

				if ((m_nComboItem >= 0 && m_nComboItem < GetItemCount()) &&
					(m_nComboSubItem >= 0 && m_nComboSubItem < GetColumns()))
				{
					SetItemText(m_nComboItem, m_nComboSubItem, str);

					UpdateSubItem(m_nComboItem, m_nComboSubItem);

					if (str != m_strInitialString)
					{
						// string is not the same, mark item as modified

						XLISTCTRLDATA *pXLCD = 
							(XLISTCTRLDATA *) CListCtrl::GetItemData(m_nComboItem);

						if (pXLCD)
						{
							pXLCD[m_nComboSubItem].bModified = TRUE;
							m_bListModified = TRUE;
						}
					}

					CWnd *pWnd = GetParent();
					if (!pWnd)
						pWnd = GetOwner();
					if (pWnd && ::IsWindow(pWnd->m_hWnd))
						pWnd->SendMessage(WM_XLISTCTRL_COMBO_SELECTION, 
									m_nComboItem, m_nComboSubItem);
				}
			}

			m_pCombo->DestroyWindow();
			delete m_pCombo;
		}
		m_pCombo = NULL;
	}
	else if (nIDEvent == 4)		// get combo listbox selection
	{
		KillTimer(nIDEvent);

		XLISTCTRL_TRACE(_T("timer 4 ~~~~~\n"));

		if (m_pCombo)
		{
			CString str;
			int i = m_pCombo->GetCurSel();
			if (i != LB_ERR)
			{
				m_pCombo->GetLBText(i, str);

				if ((m_nComboItem >= 0 && m_nComboItem < GetItemCount()) &&
					(m_nComboSubItem >= 0 && m_nComboSubItem < GetColumns()))
				{
					SetItemText(m_nComboItem, m_nComboSubItem, str);

					UpdateSubItem(m_nComboItem, m_nComboSubItem);
				}
			}
		}
	}

	CListCtrl::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// OnComboEscape
LRESULT CXListCtrl::OnComboEscape(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnComboEscape\n"));
	SetTimer(2, 50, NULL);
	//UpdateSubItem(m_nComboItem, m_nComboSubItem);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnComboReturn
LRESULT CXListCtrl::OnComboComplete(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnComboComplete\n"));
	SetTimer(3, 50, NULL);
	return 0;
}

#endif		// #ifndef DO_NOT_INCLUDE_XCOMBOLIST

#ifndef NO_XLISTCTRL_TOOL_TIPS

///////////////////////////////////////////////////////////////////////////////
// OnToolHitTest
int CXListCtrl::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	LVHITTESTINFO lvhitTestInfo;
	
	lvhitTestInfo.pt = point;
	
	int nItem = ListView_SubItemHitTest(this->m_hWnd, &lvhitTestInfo);
	int nSubItem = lvhitTestInfo.iSubItem;
	//XLISTCTRL_TRACE(_T("in CToolTipListCtrl::OnToolHitTest: %d,%d\n"), nItem, nSubItem);

	UINT nFlags = lvhitTestInfo.flags;

	// nFlags is 0 if the SubItemHitTest fails
	// Therefore, 0 & <anything> will equal false
	if (nFlags & LVHT_ONITEMLABEL)
	{
		// If it did fall on a list item,
		// and it was also hit one of the
		// item specific subitems we wish to show tool tips for
		
		// get the client (area occupied by this control
		RECT rcClient;
		GetClientRect(&rcClient);
		
		// fill in the TOOLINFO structure
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT) (nItem * 1000 + nSubItem + 1);
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		pTI->rect = rcClient;
		
		return pTI->uId;	// By returning a unique value per listItem,
							// we ensure that when the mouse moves over another
							// list item, the tooltip will change
	}
	else
	{
		//Otherwise, we aren't interested, so let the message propagate
		return -1;
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnToolTipText
BOOL CXListCtrl::OnToolTipText(UINT /*id*/, NMHDR * pNMHDR, LRESULT * pResult)
{
	UINT nID = pNMHDR->idFrom;
	//XLISTCTRL_TRACE(_T("in CXListCtrl::OnToolTipText: id=%d\n"), nID);
	
	// check if this is the automatic tooltip of the control
	if (nID == 0) 
		return TRUE;	// do not allow display of automatic tooltip,
						// or our tooltip will disappear
	
	// handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	
	*pResult = 0;
	
	// get the mouse position
	const MSG* pMessage;
	pMessage = GetCurrentMessage();
	ASSERT(pMessage);
	CPoint pt;
	pt = pMessage->pt;		// get the point from the message
	ScreenToClient(&pt);	// convert the point's coords to be relative to this control
	
	// see if the point falls onto a list item
	
	LVHITTESTINFO lvhitTestInfo;
	
	lvhitTestInfo.pt = pt;
	
	int nItem = SubItemHitTest(&lvhitTestInfo);
	int nSubItem = lvhitTestInfo.iSubItem;
	
	UINT nFlags = lvhitTestInfo.flags;
	
	// nFlags is 0 if the SubItemHitTest fails
	// Therefore, 0 & <anything> will equal false
	if (nFlags & LVHT_ONITEMLABEL)
	{
		// If it did fall on a list item,
		// and it was also hit one of the
		// item specific subitems we wish to show tooltips for
		
		CString strToolTip;
		strToolTip = _T("");

		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (pXLCD)
		{
			strToolTip = pXLCD[nSubItem].strToolTip;
		}

		if (!strToolTip.IsEmpty())
		{
			// If there was a CString associated with the list item,
			// copy it's text (up to 80 characters worth, limitation 
			// of the TOOLTIPTEXT structure) into the TOOLTIPTEXT 
			// structure's szText member
			
#ifndef _UNICODE
			if (pNMHDR->code == TTN_NEEDTEXTA)
				lstrcpyn(pTTTA->szText, strToolTip, 80);
			else
				_mbstowcsz(pTTTW->szText, strToolTip, 80);
#else
			if (pNMHDR->code == TTN_NEEDTEXTA)
				_wcstombsz(pTTTA->szText, strToolTip, 80);
			else
				lstrcpyn(pTTTW->szText, strToolTip, 80);
#endif
			return FALSE;	 // we found a tool tip,
		}
	}
	
	return FALSE;	// we didn't handle the message, let the 
					// framework continue propagating the message
}

///////////////////////////////////////////////////////////////////////////////
// SetItemToolTipText
BOOL CXListCtrl::SetItemToolTipText(int nItem, int nSubItem, LPCTSTR lpszToolTipText)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[nSubItem].strToolTip = lpszToolTipText;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetItemToolTipText
CString CXListCtrl::GetItemToolTipText(int nItem, int nSubItem)
{
	CString strToolTip;
	strToolTip = _T("");

	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return strToolTip;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return strToolTip;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		strToolTip = pXLCD[nSubItem].strToolTip;
	}

	return strToolTip;
}

///////////////////////////////////////////////////////////////////////////////
// DeleteAllToolTips
void CXListCtrl::DeleteAllToolTips()
{
	int nRow = GetItemCount();
	int nCol = GetColumns();

	for (int nItem = 0; nItem < nRow; nItem++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (pXLCD)
			for (int nSubItem = 0; nSubItem < nCol; nSubItem++)
				pXLCD[nSubItem].strToolTip = _T("");
	}
}

#endif

#ifndef DO_NOT_INCLUDE_XCOMBOLIST
///////////////////////////////////////////////////////////////////////////////
// DrawComboBox
void CXListCtrl::DrawComboBox(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	// Make sure that nSubItem is valid
	ASSERT(GetColumnWidth(nSubItem) >= 5);
	if (GetColumnWidth(nSubItem) < 5)
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		ASSERT(FALSE);
		return;
	}

	if (!pXLCD[0].bEnabled)
		return;

#ifdef _DEBUG
	DWORD dwExStyle = GetExtendedStyle();
	if ((dwExStyle & LVS_EX_FULLROWSELECT) == 0)
	{
		XLISTCTRL_TRACE(_T("XListCtrl: combo boxes require LVS_EX_FULLROWSELECT style\n"));
		ASSERT(FALSE);
	}
#endif

	// Get the column offset
	int offset = 0;
	for (int i = 0; i < nSubItem; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);

	CRect rectClient;
	GetClientRect(&rectClient);

	m_pCombo = new CXCombo(this);
	ASSERT(m_pCombo);

	if (m_pCombo)
	{
		m_nComboItem = nItem;
		m_nComboSubItem = nSubItem;

		rect.top -= 1;
		m_rectComboList = rect;
		ClientToScreen(&m_rectComboList);
		m_rectComboList.left += 1;

		DWORD dwStyle = CBS_DROPDOWNLIST | WS_POPUP | WS_VISIBLE ;

		BOOL bSuccess = m_pCombo->CreateEx(WS_EX_CONTROLPARENT, 
										   ADVCOMBOBOXCTRL_CLASSNAME, 
										   _T(""),
										   dwStyle,
										   m_rectComboList,
										   this, 
										   0, 
										   NULL);

		if (bSuccess)
		{
			LockWindowUpdate();
			if (pXLCD[nSubItem].psa)
			{
				// add strings to combo

				CString s = _T("");
				try
				{
					for (int i = 0; i < pXLCD[nSubItem].psa->GetSize(); i++)
					{
						s = pXLCD[nSubItem].psa->GetAt(i);
						if (!s.IsEmpty())
							m_pCombo->AddString(s);
					}
				}
				catch(...)
				{
					TRACE(_T("ERROR - exception in CXListCtrl::DrawComboBox\n"));
					TRACE(_T("==>  Attempting to access psa pointer;  string array must be a class\n"));
					TRACE(_T("==>  variable, a global variable, or allocated on the heap.\n"));
				}
			}

			m_pCombo->SetDefaultVisibleItems(pXLCD[nSubItem].nComboListHeight);

			int index = 0;

			CString str = _T("");

			// Note that strings in combo are sorted in CXListCtrl::SetComboBox()

			if (pXLCD[nSubItem].psa)
			{
				try
				{
					if ((pXLCD[nSubItem].nInitialComboSel >= 0) &&
						(m_pCombo->GetCount() > pXLCD[nSubItem].nInitialComboSel))
					{
						index = pXLCD[nSubItem].nInitialComboSel;
						m_pCombo->GetLBText(index, str);
						XLISTCTRL_TRACE(_T("nInitialComboSel=%d  str=<%s>\n"), index, str);
						SetItemText(nItem, nSubItem, str);
						pXLCD[nSubItem].nInitialComboSel = -1; // default after first time
					}
				}
				catch(...)
				{
					TRACE(_T("ERROR - exception in CXListCtrl::DrawComboBox\n"));
					TRACE(_T("==>  Attempting to access psa pointer;  string array must be a class\n"));
					TRACE(_T("==>  variable, a global variable, or allocated on the heap.\n"));
				}
			}

			if (str.IsEmpty())
				str = GetItemText(nItem, nSubItem);

			if (str.IsEmpty())
			{
				// str is empty, try to get from first listbox string
				if (m_pCombo->GetCount() > 0)
				{
					m_pCombo->GetLBText(0, str);
					index = 0;
				}

				SetItemText(nItem, nSubItem, str);
			}
			else
			{
				// set listbox selection from subitem text
				index = m_pCombo->FindStringExact(-1, str);
				XLISTCTRL_TRACE(_T("FindStringExact returned %d\n"), index);

				if (index == LB_ERR)
					index = 0;
			}
			m_pCombo->SetCurSel(index);
			m_pCombo->GetLBText(index, m_strInitialString);

			SetTimer(1, 50, NULL);
		}

		m_pCombo->Invalidate();
		m_pCombo->RedrawWindow();
		m_pCombo->BringWindowToTop();
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
// DrawEdit - Start edit of a sub item label
// nItem        - The row index of the item to edit
// nSubItem     - The column of the sub item.
void CXListCtrl::DrawEdit(int nItem, int nSubItem)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::DrawEdit\n"));

	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	// Make sure that nSubItem is valid
	ASSERT(GetColumnWidth(nSubItem) >= 5);
	if (GetColumnWidth(nSubItem) < 5)
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		ASSERT(FALSE);
		return;
	}

	if (!pXLCD[0].bEnabled)
		return;

#ifdef _DEBUG
	DWORD dwExStyle = GetExtendedStyle();
	if ((dwExStyle & LVS_EX_FULLROWSELECT) == 0)
	{
		XLISTCTRL_TRACE(_T("XListCtrl: edit boxes require LVS_EX_FULLROWSELECT style\n"));
		ASSERT(FALSE);
	}
#endif

	// make sure that the item is visible
	if (!EnsureVisible(nItem, TRUE)) 
		return;

	// get the column offset
	int offset = 0;
	for (int i = 0; i < nSubItem; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// now scroll if we need to expose the column
	CRect rectClient;
	GetClientRect(&rectClient);
	if (offset + rect.left < 0 || offset + rect.left > rectClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(nSubItem, &lvcol);
	DWORD dwStyle = 0;
	if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	dwStyle |= WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT /*| WS_BORDER*/ ;

	//rect.top -= 2;
	rect.bottom += 1;
	rect.left += offset + 2;
	rect.right = rect.left + GetColumnWidth(nSubItem) - 4;

	if (rect.right > rectClient.right) 
		rect.right = rectClient.right;

	m_strInitialString = GetItemText(nItem, nSubItem);

	ASSERT(m_pEdit == NULL);

	m_pEdit = new CXEdit(this, m_strInitialString);

	if (m_pEdit)
	{
		BOOL bSuccess = m_pEdit->Create(dwStyle, rect, this, 99);

		m_nEditItem = nItem;
		m_nEditSubItem = nSubItem;

		if (bSuccess)
		{
			m_pEdit->SetFocus();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnXEditEscape
LRESULT CXListCtrl::OnXEditEscape(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnXEditEscape\n"));

	if (m_pEdit && ::IsWindow(m_pEdit->m_hWnd))
	{
		m_pEdit->DestroyWindow();
		delete m_pEdit;
	}
	m_pEdit = NULL;

	// restore original string
	SetItemText(m_nEditItem, m_nEditSubItem, m_strInitialString);

	UpdateSubItem(m_nEditItem, m_nEditSubItem);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnXEditKillFocus
LRESULT CXListCtrl::OnXEditKillFocus(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnXEditKillFocus\n"));

	CString str = _T("");
	if (m_pEdit && ::IsWindow(m_pEdit->m_hWnd))
	{
		m_pEdit->GetWindowText(str);
		if (str.IsEmpty())
		{
			// restore original string
			str = m_strInitialString;
		}

		m_pEdit->DestroyWindow();
		delete m_pEdit;
	}
	m_pEdit = NULL;

	// set new string in subitem
	SetItemText(m_nEditItem, m_nEditSubItem, str);

	if (str != m_strInitialString)
	{
		XLISTCTRL_TRACE(_T("m_strInitialString=<%s>\n"), m_strInitialString);

		// string is not the same, mark item as modified

		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(m_nEditItem);
		if (pXLCD)
		{
			pXLCD[m_nEditSubItem].bModified = TRUE;
			m_bListModified = TRUE;
		}
	}

	UpdateSubItem(m_nEditItem, m_nEditSubItem);

	CWnd *pWnd = GetParent();
	if (!pWnd)
		pWnd = GetOwner();
	if (pWnd && ::IsWindow(pWnd->m_hWnd))
		pWnd->SendMessage(WM_XLISTCTRL_EDIT_END, 
					m_nEditItem, m_nEditSubItem);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnEraseBkgnd
BOOL CXListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rectClip, rectTop, rectBottom, rectRight;
	int nItemCount = GetItemCount();

	if (!nItemCount) // Empty XListCtrl, nothing to do, CListCtrl will
		return CListCtrl::OnEraseBkgnd(pDC); // erase the Background

	if (pDC->GetClipBox(&rectClip) == ERROR)
	{
		ASSERT(false);
		return CListCtrl::OnEraseBkgnd(pDC);
	}

	int nFirstRow = GetTopIndex();
	int nLastRow = nFirstRow + GetCountPerPage();
	nLastRow = min (nLastRow, nItemCount - 1); // Last Item displayed in Ctrl

	CListCtrl::GetSubItemRect(nFirstRow, 0, LVIR_BOUNDS, rectTop);
	CListCtrl::GetSubItemRect(nLastRow, 0, LVIR_BOUNDS, rectBottom);

	CRect rectEraseTop = rectClip;
	rectEraseTop.bottom = rectTop.top;
	pDC->FillSolidRect(rectEraseTop, m_crWindow);

	CRect rectEraseBottom = rectClip;
	rectEraseBottom.top = rectBottom.bottom;
	pDC->FillSolidRect(rectEraseBottom, m_crWindow);

	CRect rectEraseRight = rectClip;
	rectEraseRight.top = rectTop.top;
	rectEraseRight.bottom = rectBottom.bottom;
	rectEraseRight.left = rectTop.right;
	pDC->FillSolidRect(rectEraseRight, m_crWindow);

	return TRUE;

	//return CListCtrl::OnEraseBkgnd(pDC);
}

///////////////////////////////////////////////////////////////////////////////
// FindDataItem
//+++
int CXListCtrl::FindDataItem(DWORD dwData)
{
	for (int nItem = 0; nItem < GetItemCount(); nItem++) 
	{
		if (GetItemData(nItem) == dwData)
			return nItem;
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
// OnKeyDown - check for disabled items
void CXListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnKeyDown\n"));

	int nOldItem = GetCurSel();

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	int nNewItem = GetCurSel();

	if (nNewItem >= 0)
	{
		int nTrial = nNewItem;

		BOOL bEnabled = GetEnabled(nTrial);

		if (!bEnabled)
		{
			// item is disabled, try another

			int nCount = GetItemCount();

			if (nChar == VK_DOWN || nChar == VK_NEXT)
			{
				int nDirection = 1;

				while (!bEnabled)
				{
					nTrial += 1 * nDirection;

					if (nTrial >= nCount)
					{
						// at the end, back up
						nTrial = nCount;
						nDirection = -1;
						continue;
					}
					else if (nTrial < 0)
					{
						// at beginning - must have been backing up
						nTrial = nOldItem;
						break;
					}

					bEnabled = GetEnabled(nTrial);
				}
			}
			else if (nChar == VK_UP || nChar == VK_PRIOR)
			{
				int nDirection = -1;

				while (!bEnabled)
				{
					nTrial += 1 * nDirection;

					if (nTrial < 0)
					{
						// at the beginning, go forward
						nTrial = 0;
						nDirection = 1;
						continue;
					}
					else if (nTrial >= nCount)
					{
						// at end - must have been going forward
						nTrial = nOldItem;
						break;
					}

					bEnabled = GetEnabled(nTrial);
				}
			}
			else
			{
				// don't know how user got here, just go back to previous
				nTrial = nOldItem;
			}
		}

		SetCurSel(nTrial, TRUE);	// set new selection, scroll into view
	}
}
