// XTColorPageCustom.cpp : implementation file
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

#include "Common/XTPDrawHelpers.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPSystemHelpers.h"

#include "XTNotify.h"
#include "XTColorRef.h"
#include "XTColorDialog.h"
#include "XTColorPageCustom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_LUMINANCE 0.50f

/////////////////////////////////////////////////////////////////////////////
// CXTColorBase

CXTColorBase::CXTColorBase()
{
	m_nLum = 0.0;
	m_nSat = 0.0;
	m_nHue = 0.0;
	m_ptMousePos = CPoint(0, 0);
	m_bPreSubclassInit = true;
}

CXTColorBase::~CXTColorBase()
{
}

CXTColorBase::FocusedControl CXTColorBase::m_eHasFocus = focusNone;

BEGIN_MESSAGE_MAP(CXTColorBase, CStatic)
	//{{AFX_MSG_MAP(CXTColorBase)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTColorBase message handlers

bool CXTColorBase::Init()
{
	if (::IsWindow(m_hWnd))
	{
		// Set the style to SS_NOTIFY.
		DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
		::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);

		return true;
	}

	return false;
}

void CXTColorBase::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();

	if (m_bPreSubclassInit)
	{
		// Initialize the control.
		Init();
	}
}

int CXTColorBase::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Initialize the control.
	Init();

	return 0;
}

BOOL CXTColorBase::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	// When creating controls dynamically Init() must
	// be called from OnCreate() and not from
	// PreSubclassWindow().

	m_bPreSubclassInit = false;

	return TRUE;
}

void CXTColorBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	SetCapture();
	UpdateCursorPos(point);

	if (GetFocus() != this)
	{
		SetFocus();
	}
}

void CXTColorBase::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	CStatic::OnLButtonUp(nFlags, point);
}

void CXTColorBase::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	if (nFlags & MK_LBUTTON)
	{
		UpdateCursorPos(point);

		if (GetFocus() != this)
		{
			SetFocus();
		}
	}
}

void CXTColorBase::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDblClk(nFlags, point);

	UpdateCursorPos(point);

	CPropertyPage* pParentPage = (CPropertyPage*)GetParent();
	ASSERT_VALID(pParentPage);

	CPropertySheet* pParentSheet = (CPropertySheet*)pParentPage->GetParent();
	ASSERT_VALID(pParentSheet);

	pParentSheet->EndDialog(IDOK);
}

void CXTColorBase::UpdateCursorPos(CPoint /*point*/)
{

}

COLORREF CXTColorBase::HLStoRGB(double h, double l, double s)
{
	return CXTColorRef::fromHSL(h, s, l);
}

void CXTColorBase::RGBtoHSL(COLORREF color, double* h, double* s, double* l)
{
	CXTColorRef(color).toHSL(*h, *s, *l);
}


/////////////////////////////////////////////////////////////////////////////
// CXTColorWnd

CXTColorWnd::CXTColorWnd()
{
}

CXTColorWnd::~CXTColorWnd()
{
}

BEGIN_MESSAGE_MAP(CXTColorWnd, CXTColorBase)
	//{{AFX_MSG_MAP(CXTColorWnd)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTColorWnd message handlers

void CXTColorWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);
	CXTPBufferDC memDC(dc);

	int cy = rc.Height();
	int cx = rc.Width();

	if (m_bmpPicker.GetSafeHandle() == NULL)
	{
		// create bitmap
		m_bmpPicker.CreateCompatibleBitmap(&memDC, cx, cy);

		// create picker DC
		CXTPCompatibleDC dcPicker(&memDC, m_bmpPicker);

		// fill color picker bitmap
		int x;
		for (x = 0; x < cx; x++)
		{
			int y;
			for (y = 0; y < cy; y++)
			{
				COLORREF clr = HLStoRGB((double)x/(double)cx,
					DEFAULT_LUMINANCE, (double)y/(double)cy);

				dcPicker.SetPixelV(x, (cy-1)-y, clr);
			}
		}
	}

	memDC.DrawState(CPoint(0, 0), CSize(0, 0), &m_bmpPicker, DSS_NORMAL, NULL);
	DrawCrossHair(&memDC);
}

BOOL CXTColorWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTColorWnd::UpdateCursorPos(CPoint point)
{
	CRect rcClient;
	GetClientRect(rcClient);

	if (point.y < 0) point.y = 0;
	if (point.x < 0) point.x = 0;
	if (point.y > rcClient.bottom) point.y = rcClient.bottom;
	if (point.x > rcClient.right) point.x = rcClient.right;
	m_ptMousePos = point;
	
	m_nHue = (double)point.x / (double)rcClient.Width();
	m_nSat = (double)(rcClient.Height() - point.y) / (double)rcClient.Height();

	GetParent()->SendMessage(XTWM_UPDATECOLOR, 0, (LPARAM)m_hWnd);

	Invalidate(FALSE);
}

BOOL CXTColorWnd::PreTranslateMessage(MSG* pMsg)
{
	if (m_eHasFocus == focusColorWheel)
	{
		switch (pMsg->message)
		{
		case WM_KEYDOWN:
			{
				CRect rect;
				GetClientRect(&rect);

				TCHAR vkey = (TCHAR)pMsg->wParam;
				switch (vkey)
				{
				case VK_LEFT:
					m_ptMousePos.x--;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;

				case VK_UP:
					m_ptMousePos.y--;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;

				case VK_RIGHT:
					m_ptMousePos.x++;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;

				case VK_DOWN:
					m_ptMousePos.y++;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;
				case VK_RETURN:
				case VK_SPACE:
					CPropertyPage* pParentPage = (CPropertyPage*)GetParent();
					ASSERT_VALID(pParentPage);

					CPropertySheet* pParentSheet = (CPropertySheet*)pParentPage->GetParent();
					ASSERT_VALID(pParentSheet);

					pParentSheet->EndDialog(IDOK);
					break;
				}
			}
			break;
		}
	}

	return CStatic::PreTranslateMessage(pMsg);
}

void CXTColorWnd::DrawCrossHair(CDC* pDC)
{
	int x = m_ptMousePos.x;
	int y = m_ptMousePos.y;

	CPen pen1(PS_SOLID, 1, RGB(0xff, 0xff, 0xff));
	CPen pen2(PS_SOLID, 1, RGB(0x00, 0x00, 0x00));

	CPen* pOldPen = pDC->SelectObject((m_eHasFocus == focusColorWheel) ? &pen1 : &pen2);

	pDC->MoveTo(x-5, y-1);
	pDC->LineTo(x-10, y-1);
	pDC->MoveTo(x-5, y);
	pDC->LineTo(x-10, y);
	pDC->MoveTo(x-5, y + 1);
	pDC->LineTo(x-10, y + 1);

	pDC->MoveTo(x + 5, y-1);
	pDC->LineTo(x + 10, y-1);
	pDC->MoveTo(x + 5, y);
	pDC->LineTo(x + 10, y);
	pDC->MoveTo(x + 5, y + 1);
	pDC->LineTo(x + 10, y + 1);

	pDC->MoveTo(x-1, y-5);
	pDC->LineTo(x-1, y-10);
	pDC->MoveTo(x, y-5);
	pDC->LineTo(x, y-10);
	pDC->MoveTo(x + 1, y-5);
	pDC->LineTo(x + 1, y-10);

	pDC->MoveTo(x-1, y + 5);
	pDC->LineTo(x-1, y + 10);
	pDC->MoveTo(x, y + 5);
	pDC->LineTo(x, y + 10);
	pDC->MoveTo(x + 1, y + 5);
	pDC->LineTo(x + 1, y + 10);

	pDC->SelectObject(pOldPen);
}

void CXTColorWnd::SetColor(double nHue, double nSat)
{
	SetHue(nHue);
	SetSaturation(nSat);
}

void CXTColorWnd::SetHue(double h)
{
	m_nHue = h;
	CRect rc;
	GetClientRect(&rc);
	m_ptMousePos.x = (long)((double)rc.Width() * h);
	Invalidate(FALSE);
}

void CXTColorWnd::SetSaturation(double s)
{
	m_nSat = s;
	CRect rc;
	GetClientRect(&rc);
	m_ptMousePos.y = rc.Height()-(long)((double)rc.Height() * s);
	Invalidate(FALSE);
}

void CXTColorWnd::OnSetFocus(CWnd* pOldWnd)
{
	CXTColorBase::OnSetFocus(pOldWnd);
	m_eHasFocus = focusColorWheel;

	Invalidate(FALSE);
}

void CXTColorWnd::OnKillFocus(CWnd* pNewWnd)
{
	CXTColorBase::OnKillFocus(pNewWnd);
	m_eHasFocus = focusNone;

	Invalidate(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CXTColorLum

CXTColorLum::CXTColorLum()
{
	m_ptMousePos.y = 0;
}

CXTColorLum::~CXTColorLum()
{
}

BEGIN_MESSAGE_MAP(CXTColorLum, CXTColorBase)
	//{{AFX_MSG_MAP(CXTColorLum)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTColorLum message handlers

void CXTColorLum::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);
	CXTPBufferDC memDC(dc);

	HBRUSH hBrush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)memDC.GetSafeHdc(), (LPARAM)m_hWnd);
	if (hBrush)
	{
		::FillRect(memDC.GetSafeHdc(), rc, hBrush);
	}
	else
	{
		memDC.FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));
	}


	DrawLuminanceBar(&memDC);
	DrawSliderArrow(&memDC);
}

BOOL CXTColorLum::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTColorLum::GetLumBarRect(CRect& rect)
{
	GetClientRect(&rect);
	rect.DeflateRect(0, 4);
	rect.right = rect.left + 14;
}

void CXTColorLum::SetColor(double nHue, double nSat)
{
	m_nHue = nHue;
	m_nSat = nSat;
	Invalidate(FALSE);
}


void CXTColorLum::SetLuminance(double l)
{
	m_nLum = l;

	CRect rcBar;
	GetLumBarRect(rcBar);

	// Set the slider position based on the current
	// luminance.
	m_ptMousePos.y = rcBar.top + rcBar.Height()-(long)((double)rcBar.Height() * l);
	Invalidate(FALSE);
}

void CXTColorLum::DrawLuminanceBar(CDC* pDC)
{
	CRect rcBar;
	GetLumBarRect(rcBar);

	int  cy = rcBar.Height();
	int  cx = rcBar.Width();

	// fill color picker bitmap
	for (int y = 0; y < cy; y++)
	{
		COLORREF clr;
		if (y == cy)
		{
			clr = RGB(0x00, 0x00, 0x00);
		}
		else if (y == 0)
		{
			clr = RGB(0xff, 0xff, 0xff);
		}
		else
		{
			clr = HLStoRGB(m_nHue, (double)((double)(cy - y)/(double)cy), m_nSat);
		}

		pDC->FillSolidRect(rcBar.left, y + rcBar.top, cx, 1, clr);
	}
}

void CXTColorLum::DrawSliderArrow(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	rc.left += 17;

	int x = rc.left;
	int y = m_ptMousePos.y;

	CRect rcBar;
	GetLumBarRect(rcBar);
	y = max(rcBar.top, y);
	y = min(rcBar.bottom, y);

	CPen pen1(PS_SOLID, 1, RGB(0x00, 0x00, 0x00));
	CPen pen2(PS_SOLID, 1, GetXtremeColor(COLOR_3DSHADOW));

	CPen* pOldPen = pDC->SelectObject((m_eHasFocus == focusLumination) ? &pen1 : &pen2);

	pDC->MoveTo(x + 7, y-4);
	pDC->LineTo(x + 9, y-4);
	pDC->MoveTo(x + 5, y-3);
	pDC->LineTo(x + 9, y-3);
	pDC->MoveTo(x + 3, y-2);
	pDC->LineTo(x + 9, y-2);
	pDC->MoveTo(x + 1, y-1);
	pDC->LineTo(x + 9, y-1);
	pDC->MoveTo(x, y);
	pDC->LineTo(x + 9, y);
	pDC->MoveTo(x + 2, y + 1);
	pDC->LineTo(x + 9, y + 1);
	pDC->MoveTo(x + 4, y + 2);
	pDC->LineTo(x + 9, y + 2);
	pDC->MoveTo(x + 6, y + 3);
	pDC->LineTo(x + 9, y + 3);
	pDC->MoveTo(x + 8, y + 4);
	pDC->LineTo(x + 9, y + 4);

	if (m_eHasFocus != focusLumination)
	{
		COLORREF clr = RGB(0x00, 0x00, 0x00);
		pDC->SetPixel(x, y, clr);
		pDC->SetPixel(x + 1, y, clr);
		pDC->SetPixel(x + 1, y-1, clr);
		pDC->SetPixel(x + 2, y-1, clr);
		pDC->SetPixel(x + 3, y-2, clr);
		pDC->SetPixel(x + 4, y-2, clr);
		pDC->SetPixel(x + 5, y-3, clr);
		pDC->SetPixel(x + 6, y-3, clr);
		pDC->SetPixel(x + 7, y-4, clr);
		pDC->SetPixel(x + 8, y-4, clr);
		pDC->SetPixel(x + 8, y-3, clr);
		pDC->SetPixel(x + 8, y-2, clr);
		pDC->SetPixel(x + 8, y-1, clr);
		pDC->SetPixel(x + 8, y, clr);
		pDC->SetPixel(x + 8, y + 1, clr);
		pDC->SetPixel(x + 8, y + 2, clr);
		pDC->SetPixel(x + 8, y + 3, clr);
		pDC->SetPixel(x + 8, y + 4, clr);
		pDC->SetPixel(x + 7, y + 3, clr);
		pDC->SetPixel(x + 6, y + 3, clr);
		pDC->SetPixel(x + 5, y + 2, clr);
		pDC->SetPixel(x + 4, y + 2, clr);
		pDC->SetPixel(x + 3, y + 1, clr);
		pDC->SetPixel(x + 2, y + 1, clr);
	}

	pDC->SelectObject(pOldPen);
}

void CXTColorLum::UpdateCursorPos(CPoint point)
{
	point.x = 0;

	CRect rcBar;
	GetLumBarRect(rcBar);

	point.y = max(rcBar.top, point.y);
	point.y = min(rcBar.bottom, point.y);
	double cy = (double)rcBar.Height();
	
	m_nLum = double(double(cy - (point.y - rcBar.top))/cy); 

	GetParent()->SendMessage(XTWM_UPDATECOLOR, 0, (LPARAM)m_hWnd);
	
	m_ptMousePos = point;
	
	Invalidate(FALSE);
}

BOOL CXTColorLum::PreTranslateMessage(MSG* pMsg)
{
	if (m_eHasFocus == focusLumination)
	{
		CRect rcBar;
		GetLumBarRect(rcBar);

		switch (pMsg->message)
		{
		case WM_KEYDOWN:
			{
				TCHAR vkey = (TCHAR)pMsg->wParam;
				switch (vkey)
				{
				case VK_UP:
					m_ptMousePos.y--;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;

				case VK_DOWN:
					m_ptMousePos.y++;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;

				case VK_RETURN:
				case VK_SPACE:
					CPropertyPage* pParentPage = (CPropertyPage*)GetParent();
					ASSERT_VALID(pParentPage);

					CPropertySheet* pParentSheet = (CPropertySheet*)pParentPage->GetParent();
					ASSERT_VALID(pParentSheet);

					pParentSheet->EndDialog(IDOK);
					break;
				}
			}
			break;
		}
	}

	return CStatic::PreTranslateMessage(pMsg);
}

void CXTColorLum::OnSetFocus(CWnd* pOldWnd)
{
	CXTColorBase::OnSetFocus(pOldWnd);
	m_eHasFocus = focusLumination;

	Invalidate(FALSE);
}

void CXTColorLum::OnKillFocus(CWnd* pNewWnd)
{
	CXTColorBase::OnKillFocus(pNewWnd);
	m_eHasFocus = focusNone;

	Invalidate(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CXTColorPageCustom

BEGIN_MESSAGE_MAP(CXTColorPageCustom, CPropertyPage)
	//{{AFX_MSG_MAP(CXTColorPageCustom)
	ON_EN_CHANGE(XT_IDC_EDIT_RED, OnChangeEdit)
	ON_EN_CHANGE(XT_IDC_EDIT_GREEN, OnChangeEdit)
	ON_EN_CHANGE(XT_IDC_EDIT_BLUE, OnChangeEdit)
	ON_EN_CHANGE(XT_IDC_EDIT_LUM, OnChangeEditLum)
	ON_EN_CHANGE(XT_IDC_EDIT_HUE, OnChangeEditHue)
	ON_EN_CHANGE(XT_IDC_EDIT_SAT, OnChangeEditSat)
	//}}AFX_MSG_MAP
	ON_MESSAGE(XTWM_UPDATECOLOR, OnUpdateColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTColorPageCustom construction/destruction

CXTColorPageCustom::CXTColorPageCustom(CXTColorDialog* pParentSheet)
	: CPropertyPage(CXTColorPageCustom::IDD)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
	m_pParentSheet = pParentSheet;


	//{{AFX_DATA_INIT(CXTColorPageCustom)
	m_nR = 0;
	m_nB = 0;
	m_nG = 0;
	m_nH = 0;
	m_nL = 0;
	m_nS = 0;
	//}}AFX_DATA_INIT
}

CXTColorPageCustom::~CXTColorPageCustom()
{
}

void CXTColorPageCustom::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTColorPageCustom)
	DDX_Control(pDX, XT_IDC_CLR_WND, m_colorWnd);
	DDX_Control(pDX, XT_IDC_CLR_LUM, m_colorLum);
	DDX_Control(pDX, XT_IDC_TXT_SAT, m_txtSat);
	DDX_Control(pDX, XT_IDC_TXT_RED, m_txtRed);
	DDX_Control(pDX, XT_IDC_TXT_LUM, m_txtLum);
	DDX_Control(pDX, XT_IDC_TXT_HUE, m_txtHue);
	DDX_Control(pDX, XT_IDC_TXT_GREEN, m_txtGreen);
	DDX_Control(pDX, XT_IDC_TXT_BLUE, m_txtBlue);
	DDX_Control(pDX, XT_IDC_SPIN_SAT, m_spinSat);
	DDX_Control(pDX, XT_IDC_SPIN_RED, m_spinRed);
	DDX_Control(pDX, XT_IDC_SPIN_LUM, m_spinLum);
	DDX_Control(pDX, XT_IDC_SPIN_HUE, m_spinHue);
	DDX_Control(pDX, XT_IDC_SPIN_GREEN, m_spinGreen);
	DDX_Control(pDX, XT_IDC_SPIN_BLUE, m_spinBlue);
	DDX_Control(pDX, XT_IDC_EDIT_HUE, m_editHue);
	DDX_Control(pDX, XT_IDC_EDIT_GREEN, m_editGreen);
	DDX_Control(pDX, XT_IDC_EDIT_BLUE, m_editBlue);
	DDX_Control(pDX, XT_IDC_EDIT_LUM, m_editLum);
	DDX_Control(pDX, XT_IDC_EDIT_RED, m_editRed);
	DDX_Control(pDX, XT_IDC_EDIT_SAT, m_editSat);
	DDX_Text(pDX, XT_IDC_EDIT_RED, m_nR);
	DDV_MinMaxInt(pDX, m_nR, 0, 255);
	DDX_Text(pDX, XT_IDC_EDIT_BLUE, m_nB);
	DDV_MinMaxInt(pDX, m_nB, 0, 255);
	DDX_Text(pDX, XT_IDC_EDIT_GREEN, m_nG);
	DDV_MinMaxInt(pDX, m_nG, 0, 255);
	DDX_Text(pDX, XT_IDC_EDIT_HUE, m_nH);
	DDV_MinMaxInt(pDX, m_nH, 0, 255);
	DDX_Text(pDX, XT_IDC_EDIT_LUM, m_nL);
	DDV_MinMaxInt(pDX, m_nL, 0, 255);
	DDX_Text(pDX, XT_IDC_EDIT_SAT, m_nS);
	DDV_MinMaxInt(pDX, m_nS, 0, 255);
	//}}AFX_DATA_MAP
}

BOOL CXTColorPageCustom::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here
	m_spinSat.SetBuddy(&m_editSat);
	m_spinSat.SetRange(0, 255);

	m_spinLum.SetBuddy(&m_editLum);
	m_spinLum.SetRange(0, 255);

	m_spinHue.SetBuddy(&m_editHue);
	m_spinHue.SetRange(0, 255);

	m_spinRed.SetBuddy(&m_editRed);
	m_spinRed.SetRange(0, 255);

	m_spinGreen.SetBuddy(&m_editGreen);
	m_spinGreen.SetRange(0, 255);

	m_spinBlue.SetBuddy(&m_editBlue);
	m_spinBlue.SetRange(0, 255);

	CRect rc;
	m_colorLum.GetWindowRect(&rc);
	ScreenToClient(rc);
	rc.InflateRect(0, 4);
	m_colorLum.MoveWindow(&rc);


	return TRUE;
}

BOOL CXTColorPageCustom::OnSetActive()
{
	COLORREF clr = m_pParentSheet->GetColor();
	RGBtoHSL(clr, &m_nL, &m_nS, &m_nH);

	m_colorWnd.SetColor((double)m_nH/255, (double)m_nS/255);
	m_colorLum.SetColor((double)m_nH/255, (double)m_nS/255);
	m_colorLum.SetLuminance((double)m_nL/255);
	
	UpdateRGB(clr);

	return CPropertyPage::OnSetActive();
}

void CXTColorPageCustom::OnChangeEdit()
{
	UpdateData();
	COLORREF clr = (COLORREF)RGB(m_nR, m_nG, m_nB);

	OnUpdateColor((WPARAM)clr, 0);
}

void CXTColorPageCustom::OnChangeEditLum()
{
	UpdateData();
	m_colorLum.SetLuminance((double)m_nL/255);

	COLORREF clr = m_colorLum.HLStoRGB((double)m_nH/255, (double)m_nL/255, (double)m_nS/255);

	if (clr != m_pParentSheet->GetColor())
		m_pParentSheet->SetNewColor(clr, FALSE);

	UpdateRGB(clr);
}

void CXTColorPageCustom::UpdateRGB(COLORREF clr)
{
	m_nR = GetRValue(clr);
	m_nG = GetGValue(clr);
	m_nB = GetBValue(clr);

	UpdateData(FALSE);
}

void CXTColorPageCustom::OnChangeEditHue()
{
	UpdateData();
	m_colorWnd.SetHue((double)m_nH/255);

	COLORREF clr = m_colorWnd.HLStoRGB((double)m_nH/255, (double)m_nL/255, (double)m_nS/255);

	m_colorLum.SetColor((double)m_nH/255, (double)m_nS/255);

	if (clr != m_pParentSheet->GetColor())
		m_pParentSheet->SetNewColor(clr, FALSE);

	UpdateRGB(clr);
}

void CXTColorPageCustom::OnChangeEditSat()
{
	UpdateData();
	m_colorWnd.SetSaturation((double)m_nS/255);

	COLORREF clr = m_colorWnd.HLStoRGB((double)m_nH/255, (double)m_nL/255, (double)m_nS/255);

	m_colorLum.SetColor((double)m_nH/255, (double)m_nS/255);

	if (clr != m_pParentSheet->GetColor())
		m_pParentSheet->SetNewColor(clr, FALSE);

	UpdateRGB(clr);
}

LRESULT CXTColorPageCustom::OnUpdateColor(WPARAM wParam, LPARAM lParam)
{
	COLORREF clr = (COLORREF)wParam;

	if ((HWND)lParam == m_colorWnd)
	{
		m_nS = int(m_colorWnd.m_nSat * 255);
		m_nH = int(m_colorWnd.m_nHue * 255);

		clr = m_colorWnd.HLStoRGB((double)m_nH/255, (double)m_nL/255, (double)m_nS/255);
		m_colorLum.SetColor((double)m_nH/255, (double)m_nS/255);
	}

	if ((HWND)lParam == m_colorLum)
	{
		m_nL = int(m_colorLum.m_nLum * 255);

		clr = m_colorWnd.HLStoRGB((double)m_nH/255, (double)m_nL/255, (double)m_nS/255);
	}

	if (lParam == 0)
	{
		RGBtoHSL(clr, &m_nL, &m_nS, &m_nH);

		m_colorWnd.SetColor((double)m_nH/255, (double)m_nS/255); 
		m_colorLum.SetColor((double)m_nH/255, (double)m_nS/255);
		m_colorLum.SetLuminance((double)m_nL/255);
	}

	if (clr != m_pParentSheet->GetColor())
		m_pParentSheet->SetNewColor(clr, FALSE);

	UpdateRGB(clr);
	return 0;
}

void CXTColorPageCustom::RGBtoHSL(COLORREF color, int* lum, int* sat, int* hue)
{
	double r = (double)GetRValue(color)/255;
	double g = (double)GetGValue(color)/255;
	double b = (double)GetBValue(color)/255;

	double maxcolor = __max(r, __max(g, b));
	double mincolor = __min(r, __min(g, b));

	double l = (maxcolor + mincolor)/2;

	if (maxcolor == mincolor)
	{
		*hue = 0;
		*sat = 0;
	}
	else
	{
		double s;
		double h;

		if (l < 0.5)
			s = (maxcolor-mincolor)/(maxcolor + mincolor);
		else
			s = (maxcolor-mincolor)/(2.0-maxcolor-mincolor);

		if (r == maxcolor)
			h = (g-b)/(maxcolor-mincolor);
		else if (g == maxcolor)
			h = 2.0+(b-r)/(maxcolor-mincolor);
		else
			h = 4.0+(r-g)/(maxcolor-mincolor);

		h /= 6.0;
		if (h < 0.0)
			h += 1;

		*hue = (int)((double)h * 255);
		*sat = (int)((double)s * 255);
	}

	*lum = (int)((double)l * 255);
}
