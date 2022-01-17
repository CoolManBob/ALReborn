// XTPCalendarControlView.cpp : implementation of the CXTPCalendarControlView class.
//
// This file is a part of the XTREME CALENDAR MFC class library.
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

#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPImageManager.h"
#include "Common/resource.h"

#include "XTPCalendarControlView.h"
#include "XTPCalendarMonthView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
AFX_INLINE void MoveRectToXY(CRect& rc, int nX, int nY)
{
	rc.right = nX + rc.Width();
	rc.bottom = nY + rc.Height();

	rc.left = nX;
	rc.top = nY;
}

//////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CXTPCalendarPageSetupDialog, CPageSetupDialog)
	ON_BN_CLICKED(XTP_IDC_PRINT_DATE_HEADER, OnBnClickedPrintDateHeader)
	ON_BN_CLICKED(XTP_IDC_PRINT_DATE_HEADER_FONT_BUTTON, OnBnClickedDateHeaderFont)
	ON_BN_CLICKED(XTP_IDC_CALENDAR_HEADER_FORMAT_BTN, OnBnClickedHeaderFormat)
	ON_BN_CLICKED(XTP_IDC_CALENDAR_FOOTER_FORMAT_BTN, OnBnClickedFooterFormat)
END_MESSAGE_MAP()

CXTPCalendarPageSetupDialog::CXTPCalendarPageSetupDialog(
								CXTPCalendarControlViewPrintOptions* pOptions,
								DWORD dwFlags, CWnd* pParentWnd)
	: CPageSetupDialog(dwFlags, pParentWnd)
{
	ASSERT(pOptions);
	m_pOptions = pOptions;

	if (m_pOptions)
	{
		BOOL bIsInches = m_pOptions->IsMarginsMeasureInches();
		DWORD dwMeasure = bIsInches ? PSD_INTHOUSANDTHSOFINCHES : PSD_INHUNDREDTHSOFMILLIMETERS;

		m_psd.Flags &= ~PSD_INWININIINTLMEASURE;
		m_psd.Flags |= dwMeasure;
	}

	m_psd.Flags |= PSD_ENABLEPAGESETUPTEMPLATEHANDLE;
	m_psd.hPageSetupTemplate = XTPResourceManager()->LoadDialogTemplate2(MAKEINTRESOURCE(IDD));
	ASSERT(m_psd.hPageSetupTemplate);

	if (m_pOptions)
	{
		m_psd.rtMargin = m_pOptions->m_rcMargins;
	}

	VERIFY( ::SystemParametersInfo( SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &m_lfDateHeader, 0 ) );
}

CXTPCalendarPageSetupDialog::~CXTPCalendarPageSetupDialog()
{
}

#ifndef rad1
	#define rad1        0x0420
	#define rad2        0x0421

	#define grp4        0x0433
#endif

BOOL CXTPCalendarPageSetupDialog::OnInitDialog ()
{
	CPageSetupDialog::OnInitDialog();

	ASSERT(m_pOptions);

	VERIFY( m_ctrlPrintFromCmb.SubclassDlgItem(XTP_IDC_PRINT_FROM_COMBO, this) );
	VERIFY( m_ctrlPrintToCmb.SubclassDlgItem(XTP_IDC_PRINT_TO_COMBO, this) );

	VERIFY( m_ctrlPrintFromTo.SubclassDlgItem(XTP_IDC_PRINT_FROMTO, this) );

	VERIFY( m_ctrlPrintDateHeader.SubclassDlgItem(XTP_IDC_PRINT_DATE_HEADER, this) );
	VERIFY( m_ctrlDateHeaderFontLabel.SubclassDlgItem(XTP_IDC_PRINT_DATE_HEADER_FONT_LABEL, this) );
	VERIFY( m_ctrlDateHeaderFont.SubclassDlgItem(XTP_IDC_PRINT_DATE_HEADER_FONT, this) );
	VERIFY( m_ctrlDateHeaderFontButton.SubclassDlgItem(XTP_IDC_PRINT_DATE_HEADER_FONT_BUTTON, this) );

	VERIFY( m_ctrlBlackWhitePrinting.SubclassDlgItem(XTP_IDC_PRINT_BLACK_WHITE, this) );

	VERIFY( m_ctrlHeaderFormat.SubclassDlgItem(XTP_IDC_CALENDAR_HEADER_FORMAT, this) );
	VERIFY( m_ctrlFooterFormat.SubclassDlgItem(XTP_IDC_CALENDAR_FOOTER_FORMAT, this) );

	VERIFY( m_ctrlHeaderFormatBtn.SubclassDlgItem(XTP_IDC_CALENDAR_HEADER_FORMAT_BTN, this) );
	VERIFY( m_ctrlFooterFormatBtn.SubclassDlgItem(XTP_IDC_CALENDAR_FOOTER_FORMAT_BTN, this) );

	CString strTimeFormat_HrMin = CXTPCalendarUtils::GetTimeFormatString_HrMin();
	CString strTime;
	for (int i = 0; i < 24*60; i += 30)
	{
		strTime = CXTPCalendarUtils::GetTimeFormat((int)(i / 60), (int)(i % 60), strTimeFormat_HrMin);

		m_ctrlPrintFromCmb.AddString(strTime);
		m_ctrlPrintToCmb.AddString(strTime);
	}

	if(m_pOptions)
	{
		strTime = CXTPCalendarUtils::GetTimeFormat(m_pOptions->m_dtPrintFrom, strTimeFormat_HrMin);
		m_ctrlPrintFromCmb.SetWindowText(strTime);

		strTime = CXTPCalendarUtils::GetTimeFormat(m_pOptions->m_dtPrintTo, strTimeFormat_HrMin);
		m_ctrlPrintToCmb.SetWindowText(strTime);

		m_ctrlPrintFromTo.SetCheck(m_pOptions->m_bPrintFromToExactly ? 1 : 0);

		m_ctrlPrintDateHeader.SetCheck(m_pOptions->m_bPrintDateHeader ? 1 : 0);
		m_ctrlBlackWhitePrinting.SetCheck(m_pOptions->m_bBlackWhitePrinting? 1 : 0);

		m_lfDateHeader = m_pOptions->m_lfDateHeaderFont;

		CString strFont = FormatFontName(m_lfDateHeader);
		m_ctrlDateHeaderFont.SetWindowText(strFont);

		if (m_pOptions->GetPageHeader())
			m_ctrlHeaderFormat.SetWindowText(m_pOptions->GetPageHeader()->m_strFormatString);

		if (m_pOptions->GetPageFooter())
			m_ctrlFooterFormat.SetWindowText(m_pOptions->GetPageFooter()->m_strFormatString);

		OnBnClickedPrintDateHeader();
	}

	XTP_SAFE_CALL1(GetDlgItem(rad1), EnableWindow(TRUE)); //1056 Portrait
	XTP_SAFE_CALL1(GetDlgItem(rad2), EnableWindow(TRUE)); //1057 Landscape

	BOOL bIsInches = m_pOptions ? m_pOptions->IsMarginsMeasureInches() : FALSE;

	UINT uStrID = bIsInches ? XTP_IDS_CALENDAR_MARGINS_INCH : XTP_IDS_CALENDAR_MARGINS_MM;
	CString strCaption = CXTPCalendarUtils::LoadString(uStrID);

	if (!strCaption.IsEmpty() && GetDlgItem(grp4))
	{
		GetDlgItem(grp4)->SetWindowText(strCaption);
	}

	HICON hBtnIcon = XTPResourceManager()->LoadIcon(XTP_IDI_BTN_ARROW_R, CSize(0,0));
	ASSERT(hBtnIcon);
	if (hBtnIcon)
	{
		m_ctrlHeaderFormatBtn.SetIcon(hBtnIcon);
		m_ctrlFooterFormatBtn.SetIcon(hBtnIcon);
	}

	return FALSE;
}

void CXTPCalendarPageSetupDialog::OnBnClickedPrintDateHeader()
{
	int nCheck = m_ctrlPrintDateHeader.GetCheck();

	m_ctrlDateHeaderFont.EnableWindow(nCheck != 0);
	m_ctrlDateHeaderFontLabel.EnableWindow(nCheck != 0);
	m_ctrlDateHeaderFontButton.EnableWindow(nCheck != 0);
}

void CXTPCalendarPageSetupDialog::OnOK()
{
	ASSERT(m_pOptions);
	if (!m_pOptions)
	{
		CPageSetupDialog::OnOK();
		return;
	}

	CString strFrom, strTo;
	m_ctrlPrintFromCmb.GetWindowText(strFrom);
	int nFrom_min = CXTPCalendarUtils::ParceTimeString_min(strFrom);

	m_ctrlPrintToCmb.GetWindowText(strTo);
	int nTo_min = CXTPCalendarUtils::ParceTimeString_min(strTo);

	m_pOptions->m_dtPrintFrom.SetTime(nFrom_min / 60, nFrom_min % 60, 0);
	m_pOptions->m_dtPrintTo.SetTime(nTo_min / 60, nTo_min % 60, 0);

	m_pOptions->m_bPrintFromToExactly = m_ctrlPrintFromTo.GetCheck() != 0;
	m_pOptions->m_bBlackWhitePrinting = m_ctrlBlackWhitePrinting.GetCheck() != 0;

	m_pOptions->m_bPrintDateHeader = m_ctrlPrintDateHeader.GetCheck() != 0;
	if (m_pOptions->m_bPrintDateHeader)
	{
		m_pOptions->m_lfDateHeaderFont = m_lfDateHeader;
	}

	m_pOptions->m_rcMargins = m_psd.rtMargin;

	if (m_pOptions->GetPageHeader())
		m_ctrlHeaderFormat.GetWindowText(m_pOptions->GetPageHeader()->m_strFormatString);

	if (m_pOptions->GetPageFooter())
		m_ctrlFooterFormat.GetWindowText(m_pOptions->GetPageFooter()->m_strFormatString);

	CPageSetupDialog::OnOK();
}

CString CXTPCalendarPageSetupDialog::FormatFontName(const LOGFONT& lfFont)
{
	CClientDC dc(this);
	double dLogPixelY = dc.GetDeviceCaps(LOGPIXELSY);
	double dPointSize = abs(lfFont.lfHeight) * 72.0 / dLogPixelY;
	int nPointSize = (int)(dPointSize + 0.5);

	CString strFont;
	strFont.Format(_T(" %d pt. %s"), nPointSize, lfFont.lfFaceName);

	CString strAttr;
	if (lfFont.lfWeight >= FW_BOLD)
	{
		strAttr += _T(" Bold");
	}
	if (lfFont.lfItalic)
	{
		if (!strAttr.IsEmpty())
		{
			strAttr += _T(",");
		}
		strAttr += _T(" Italic");;
	}

	if (!strAttr.IsEmpty())
	{
		strFont += _T("\n");
		strFont += strAttr;
	}

	return strFont;
}

void CXTPCalendarPageSetupDialog::OnBnClickedDateHeaderFont()
{
	CFontDialog dlgFont(&m_lfDateHeader);
	int nDlgRes = (int)dlgFont.DoModal();

	if (nDlgRes == IDOK)
	{
		dlgFont.GetCurrentFont(&m_lfDateHeader);

		CString strFont = FormatFontName(m_lfDateHeader);
		m_ctrlDateHeaderFont.SetWindowText(strFont);
	}
}

void CXTPCalendarPageSetupDialog::OnBnClickedHeaderFormat()
{
	CXTPPrintPageHeaderFooter::DoInsertHFFormatSpecifierViaMenu(
		this, &m_ctrlHeaderFormat, &m_ctrlHeaderFormatBtn);
}

void CXTPCalendarPageSetupDialog::OnBnClickedFooterFormat()
{
	CXTPPrintPageHeaderFooter::DoInsertHFFormatSpecifierViaMenu(
		this, &m_ctrlFooterFormat, &m_ctrlFooterFormatBtn);
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CXTPCalendarControlViewPrintOptions, CXTPPrintOptions)

CXTPCalendarControlViewPrintOptions::CXTPCalendarControlViewPrintOptions()
{
	m_dtPrintFrom.SetTime(8, 0, 0);
	m_dtPrintTo.SetTime(19, 0, 0);
	m_bPrintFromToExactly = TRUE;

	LOGFONT lfIcon;
	VERIFY( ::SystemParametersInfo( SPI_GETICONTITLELOGFONT, sizeof( lfIcon ), &lfIcon, 0 ) );

	m_bPrintDateHeader = TRUE;

	m_lfDateHeaderFont = lfIcon;
	m_lfDateHeaderFont.lfHeight = -24;
	m_lfDateHeaderFont.lfWeight = FW_BOLD;

	m_lfDateHeaderWeekDayFont = lfIcon;
	m_lfDateHeaderWeekDayFont.lfHeight = -max(abs(lfIcon.lfHeight), abs(m_lfDateHeaderFont.lfHeight*2/3));

	m_lfDateHeaderCalendarFont = lfIcon;

}

LCID CXTPCalendarControlViewPrintOptions::GetActiveLCID()
{
	return CXTPCalendarUtils::GetActiveLCID();
}

void CXTPCalendarControlViewPrintOptions::Set(const CXTPCalendarControlViewPrintOptions* pSrc)
{
	if (!pSrc)
	{
		return;
	}
	CXTPPrintOptions::Set(pSrc);

	m_dtPrintFrom = pSrc->m_dtPrintFrom;
	m_dtPrintTo = pSrc->m_dtPrintTo;
	m_bPrintFromToExactly = pSrc->m_bPrintFromToExactly;

	m_bPrintDateHeader = pSrc->m_bPrintDateHeader;
	m_lfDateHeaderFont = pSrc->m_lfDateHeaderFont;
	m_lfDateHeaderCalendarFont = pSrc->m_lfDateHeaderCalendarFont;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarControlView

IMPLEMENT_DYNCREATE(CXTPCalendarControlView, CView)

CXTPCalendarControlView::CXTPCalendarControlView()
{
	m_pCalendar = NULL;
	m_pUpdateContect = NULL;

	m_bReleaseCalendarWhenDestroy = FALSE;
	m_bPrintDirect = FALSE;
	m_bResizeControlWithView = TRUE;

	m_pScrollBar = NULL;

	m_pPrintOptions = new CXTPCalendarControlViewPrintOptions();
}

CXTPCalendarControlView::~CXTPCalendarControlView()
{
	if (m_bReleaseCalendarWhenDestroy)
	{
		CMDTARGET_RELEASE(m_pCalendar);
	}

	ASSERT(m_pUpdateContect == NULL);
	SAFE_DELETE(m_pUpdateContect);

	CMDTARGET_RELEASE(m_pPrintOptions);
}

CXTPCalendarControl& CXTPCalendarControlView::GetCalendarCtrl()
{
	return m_pCalendar == NULL ? m_wndCalendar : *m_pCalendar;
}

void CXTPCalendarControlView::SetCalendarCtrl(CXTPCalendarControl* pCalendar,
											  BOOL bReleaseWhenDestroy)
{
	if (::IsWindow(m_wndCalendar.GetSafeHwnd()))
		m_wndCalendar.DestroyWindow();

	m_pCalendar = pCalendar;
	m_bReleaseCalendarWhenDestroy = bReleaseWhenDestroy;
}

void CXTPCalendarControlView::SetScrollBarCtrl(CScrollBar* pScrollBar)
{
	m_pScrollBar = pScrollBar;
}

BEGIN_MESSAGE_MAP(CXTPCalendarControlView, CView)
	//{{AFX_MSG_MAP(CXTPCalendarControlView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_BN_CLICKED(ID_EDIT_UNDO, OnUndo)
	ON_BN_CLICKED(ID_EDIT_CUT, OnCut)
	ON_BN_CLICKED(ID_EDIT_COPY, OnCopy)
	ON_BN_CLICKED(ID_EDIT_PASTE, OnPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateCommand)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCommand)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCommand)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateCommand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarControlView drawing

void CXTPCalendarControlView::OnDraw(CDC* /*pDC*/)
{
}

void CXTPCalendarControlView::OnPaint()
{
	Default();
}


/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarControlView diagnostics

#ifdef _DEBUG
void CXTPCalendarControlView::AssertValid() const
{
	CView::AssertValid();
}

void CXTPCalendarControlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarControlView message handlers

BOOL CXTPCalendarControlView::OnEraseBkgnd(CDC* )
{
	return TRUE;
}

CScrollBar* CXTPCalendarControlView::GetScrollBarCtrl(int nBar) const
{
	if (nBar == SB_VERT && m_pScrollBar)
		return m_pScrollBar;

	return CView::GetScrollBarCtrl(nBar);
}

void CXTPCalendarControlView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_pScrollBar && pScrollBar == m_pScrollBar)
		GetCalendarCtrl().OnVScroll(nSBCode, nPos, 0);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CXTPCalendarControlView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pScrollBar && m_pScrollBar->GetSafeHwnd())
	{
		int nScroll = GetSystemMetrics(SM_CXVSCROLL);
		m_pScrollBar->MoveWindow(cx - nScroll, 0, nScroll, cy);
		cx -= nScroll;
	}

	if (m_bResizeControlWithView && GetCalendarCtrl().GetSafeHwnd())
	{
		GetCalendarCtrl().MoveWindow(0, 0, cx, cy);
	}
}

int CXTPCalendarControlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!GetCalendarCtrl().Create(WS_CHILD | WS_TABSTOP | WS_VISIBLE,
								  CRect(0, 0, 0, 0), this, XTP_ID_CALENDAR_CONTROL) )
	{
		TRACE(_T("Failed to create Calendar control window\n"));
		return -1;
	}
	return 0;
}

void CXTPCalendarControlView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	GetCalendarCtrl().SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CReportSampleView printing

BOOL CXTPCalendarControlView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->m_bDirect = m_bPrintDirect;

	pInfo->m_nNumPreviewPages = 1;
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(1);

	// default preparation
	return DoPreparePrinting(pInfo);
}

void CXTPCalendarControlView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	UNREFERENCED_PARAMETER(pDC);    UNREFERENCED_PARAMETER(pInfo);

	m_pritAdjustContext.Reset();

	GetCalendarCtrl().EnableSendNotifications(FALSE);

	ASSERT(m_pUpdateContect == NULL);
	SAFE_DELETE(m_pUpdateContect);
	m_pUpdateContect = new CXTPCalendarControl::CUpdateContext(&GetCalendarCtrl(), xtpCalendarUpdateRedraw);

	m_beforePrintState.Save(this);
}

void CXTPCalendarControlView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_beforePrintState.Restore(this);
	m_beforePrintState.ClearData();

	GetCalendarCtrl().AdjustLayout();
	GetCalendarCtrl().EnableSendNotifications();

	m_pritAdjustContext.Reset();

	m_bmpGrayDC.DeleteObject();

	ASSERT(m_pUpdateContect);
	SAFE_DELETE(m_pUpdateContect);
}

CRect CXTPCalendarControlView::AdjustCalendarForPrint(CDC* pDC, CRect rcCalendar,
								CXTPCalendarControlViewPrintOptions* pOptions)

{
	//===========================================================================
	GetCalendarCtrl().AdjustLayout(pDC, rcCalendar);

	//===========================================================================
	int nViewType = GetCalendarCtrl().GetActiveView()->GetViewType();
	if (nViewType == xtpCalendarDayView || nViewType == xtpCalendarWorkWeekView)
	{
		CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, GetCalendarCtrl().GetDayView());
		ASSERT(pDayView);
		if (!pDayView)
		{
			return rcCalendar;
		}

		int dbg_nAdjustSmall = 0;
		int dbg_nAdjustScale = 0;

		int nHeightNeed_prev = 0;
		for (int nLoop = 0; nLoop < 100; nLoop++)
		{
			int nStartCell = pDayView->GetCellNumber(pOptions->m_dtPrintFrom, FALSE);
			int nEndCell = pDayView->GetCellNumber(pOptions->m_dtPrintTo, TRUE);
			int nCellCount = nEndCell - nStartCell + 1;

			int nCelHeight = pDayView->GetRowHeight();
			CRect rcHeader = pDayView->GetDayHeaderRectangle();
			CRect rcAllDayArea = pDayView->GetAllDayEventsRectangle();

			int nHeightNeed = nCellCount * nCelHeight + rcHeader.Height() +
				rcAllDayArea.Height();

			if (rcCalendar.Height() >= nHeightNeed)
			{
				if (!pOptions->m_bPrintFromToExactly)
				{
					pDayView->_ScrollV(nStartCell, nStartCell);
					break;
				}
				else
				{
					dbg_nAdjustSmall++;

					rcCalendar.bottom = rcCalendar.top + nHeightNeed;

					GetCalendarCtrl().AdjustLayout(pDC, rcCalendar);

					pDayView->_ScrollV(nStartCell, nStartCell);

					if (nHeightNeed_prev == nHeightNeed)
					{
						//GetCalendarCtrl().AdjustLayout(pDC, rcCalendar);
						break;
					}
					nHeightNeed_prev = nHeightNeed;
				}
			}
			else
			{
				COleDateTimeSpan spScale = pDayView->GetScaleInterval();
				int nScale_min = GETTOTAL_MINUTES_DTS(spScale);

				if (nScale_min < 60)
				{
					dbg_nAdjustScale++;

					//int arIntervals[] = {5, 10, 15, 30, 60, 120, 150, 180};
					int arIntervals[] = {5, 10, 15, 30, 60};
					for (int i = 0; i < _countof(arIntervals); i++)
					{
						if (arIntervals[i] > nScale_min)
						{
							spScale = CXTPCalendarUtils::Minutes2Span(arIntervals[i]);
							break;
						}
					}

					pDayView->SetScaleInterval(spScale);
					GetCalendarCtrl().AdjustLayout(pDC, rcCalendar);
				}
				else
				{
					break;
				}
			}
		}
		GetCalendarCtrl().AdjustLayout(pDC, rcCalendar);

		//TRACE(_T("DBG. Calendar, PrePrintAdjust Counts: Scale = %d, small = %d, All Loops (max 100)= %d \n"),
		//  dbg_nAdjustScale, dbg_nAdjustSmall, nLoop+1);
	}
	return rcCalendar;
}


void CXTPCalendarControlView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	if (!m_pPrintOptions || !pDC || !pInfo)
	{
		ASSERT(FALSE);
		return;
	}

	CRect rcMargins = m_pPrintOptions->GetMarginsLP(pDC);
	CRect rcPrint = pInfo->m_rectDraw;
	rcPrint.DeflateRect(rcMargins);

	//----------------------------------------------------------
	if (!m_pPrintOptions->m_bBlackWhitePrinting)
	{
		_OnPrint2(pDC, pInfo, rcPrint);
	}
	else
	{
		CRect rcPrint00(0, 0, rcPrint.Width(), rcPrint.Height());

		CDC memDC;
		VERIFY(memDC.CreateCompatibleDC(pDC));
		memDC.m_bPrinting = TRUE;

		if (!m_bmpGrayDC.m_hObject || m_bmpGrayDC.GetBitmapDimension() != rcPrint00.Size())
		{
			m_bmpGrayDC.DeleteObject();
			m_bmpGrayDC.CreateCompatibleBitmap(pDC, rcPrint00.Width(), rcPrint00.Height());
		}

		CXTPBitmapDC autpBmp(&memDC, &m_bmpGrayDC);

		//----------------------------------------------------
		memDC.FillSolidRect(rcPrint00, RGB(255, 255, 255));

		_OnPrint2(&memDC, pInfo, rcPrint00);

		int nCC = max(0, min(m_pPrintOptions->m_nBlackWhiteContrast, 255));
		XTPImageManager()->BlackWhiteBitmap(memDC, rcPrint00, nCC);

		pDC->BitBlt(rcPrint.left, rcPrint.top, rcPrint.Width(), rcPrint.Height(),
					&memDC, 0, 0, SRCCOPY);
	}
}

void CXTPCalendarControlView::_OnPrint2(CDC* pDC, CPrintInfo* pInfo, CRect rcPrint)
{
	UNREFERENCED_PARAMETER(pInfo);

	m_pritAdjustContext.m_rcHeader = rcPrint;
	m_pritAdjustContext.m_rcFooter = rcPrint;

	CString strTitle = CXTPPrintPageHeaderFooter::GetParentFrameTitle(this);

	m_pPrintOptions->GetPageHeader()->FormatTexts(pInfo, strTitle);
	m_pPrintOptions->GetPageFooter()->FormatTexts(pInfo, strTitle);

	m_pPrintOptions->GetPageFooter()->Draw(pDC, m_pritAdjustContext.m_rcFooter, TRUE);
	m_pPrintOptions->GetPageHeader()->Draw(pDC, m_pritAdjustContext.m_rcHeader);

	rcPrint.top += m_pritAdjustContext.m_rcHeader.Height();
	rcPrint.bottom -= m_pritAdjustContext.m_rcFooter.Height();

	//-----------------------------------------------------------------------
	CRect rcCalendar = rcPrint;

	if (m_pPrintOptions->m_bPrintDateHeader)
	{
		m_pritAdjustContext.m_rcDateHeader = rcPrint;
		DrawDateHeader(FALSE, pDC, m_pPrintOptions, m_pritAdjustContext.m_rcDateHeader);

		rcCalendar.top = m_pritAdjustContext.m_rcDateHeader.bottom + 2;
	}

	rcCalendar.DeflateRect(2, 2, 2, 2);

	if (m_pritAdjustContext.m_bAdjust ||
		m_pritAdjustContext.m_rectDraw_prev != rcCalendar)
	{
		m_pritAdjustContext.m_rcCalendar = AdjustCalendarForPrint(pDC, rcCalendar, m_pPrintOptions);

		m_pritAdjustContext.m_rectDraw_prev = rcCalendar;

		m_pritAdjustContext.m_bAdjust = FALSE;
	}
	//===========================================================================

	pDC->IntersectClipRect(&m_pritAdjustContext.m_rcCalendar);

	//************************************************************************
	GetCalendarCtrl().OnDraw(pDC);

	//************************************************************************
	pDC->SelectClipRgn(NULL);

	CRect rcBorder = m_pritAdjustContext.m_rcCalendar;

	rcBorder.InflateRect(1, 1, 1, 1);
	pDC->Draw3dRect(&rcBorder, 0, 0);

	rcBorder.InflateRect(1, 1, 1, 1);
	pDC->Draw3dRect(&rcBorder, 0, 0);

	m_pPrintOptions->GetPageFooter()->Draw(pDC, m_pritAdjustContext.m_rcFooter);
}

void CXTPCalendarControlView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	UNREFERENCED_PARAMETER(pInfo);

	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT(pInfo != NULL);  // overriding OnPaint -- never get this.

	pDC->SetMapMode(MM_ANISOTROPIC);

	pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),
						pDC->GetDeviceCaps(LOGPIXELSY));

	pDC->SetWindowExt(100, 100);

	// ptOrg is in logical coordinates
	pDC->OffsetWindowOrg(0, 0);
}

void CXTPCalendarControlView::DrawDateHeader(BOOL bCalculate, CDC* pDC,
							CXTPCalendarControlViewPrintOptions* pOptions,
							CRect& rrcHeader)
{
	COleDateTime dtYearMonth1, dtYearMonth2;
	CString strHeader1, strHeader2, strHeaderWeekDay;

	int nMonths = _GetDateHeaderMonths(dtYearMonth1, dtYearMonth2, strHeader1,
									   strHeader2, strHeaderWeekDay);

	if (!strHeader2.IsEmpty())
	{
		strHeader1 += _T(" -");
	}

	CRect rcCalendar1 = rrcHeader;
	CRect rcCalendar2(0, 0, 0, 0);

	_DrawDateHeaderCalendar(TRUE, pDC, pOptions, dtYearMonth1, rcCalendar1);

	int nCalendar1XPos = rrcHeader.right - rcCalendar1.Width() * 11/10;
	int nCalendar2XPos = nCalendar1XPos;

	CFont fntDateHeader;
	VERIFY( fntDateHeader.CreateFontIndirect(&pOptions->m_lfDateHeaderFont) );
	CXTPFontDC fntDC(pDC, &fntDateHeader);
	CSize sizeText = pDC->GetTextExtent(strHeader1);

	CFont fntDateHeaderWeekDay;
	VERIFY( fntDateHeaderWeekDay.CreateFontIndirect(&pOptions->m_lfDateHeaderWeekDayFont) );
	fntDC.SetFont(&fntDateHeaderWeekDay);
	CSize sizeTextWD = pDC->GetTextExtent(_T("WD"), 2);

	int nDatesCaptionHeight = sizeText.cy * 225/100 + sizeTextWD.cy * 125/100;
	int nMaxHeight = max(rcCalendar1.Height() * 11/10, nDatesCaptionHeight);

	rrcHeader.bottom = rrcHeader.top + nMaxHeight;

	int nCalendarYPos = rrcHeader.top + nMaxHeight/2 - rcCalendar1.Height() / 2;
	MoveRectToXY(rcCalendar1, nCalendar1XPos, nCalendarYPos);

	if (bCalculate)
	{
		return;
	}

	//===========================================================================
	COLORREF clrPrev = pDC->SetTextColor(0);
	int nBkModePrev = pDC->SetBkMode(TRANSPARENT);

	//---------------------------------------------------------------------------
	if (nMonths > 1)
	{
		rcCalendar2 = rcCalendar1;
		nCalendar2XPos = nCalendar1XPos - rcCalendar1.Width() * 11 / 10;
		MoveRectToXY(rcCalendar1, nCalendar2XPos, nCalendarYPos);
	}

	if (!m_pPrintOptions->m_bBlackWhitePrinting)
		pDC->FillSolidRect(&rrcHeader, RGB(229, 229, 229));

	pDC->Draw3dRect(&rrcHeader, 0, 0);

	_DrawDateHeaderCalendar(FALSE, pDC, pOptions, dtYearMonth1, rcCalendar1);

	if (nMonths > 1)
	{
		_DrawDateHeaderCalendar(FALSE, pDC, pOptions, dtYearMonth2, rcCalendar2);
	}

	CRect rcHeader1(0, 0, 0, 0);
	CRect rcHeader2(0, 0, 0, 0);
	CRect rcHeader3(0, 0, 0, 0);

	rcHeader1 = rrcHeader;
	rcHeader1.DeflateRect(10, 5, 10, 5);
	rcHeader1.right = nCalendar2XPos - 20;

	rcHeader2 = rcHeader1;

	fntDC.SetFont(&fntDateHeader);

	UINT uDrawFlags = DT_TOP | DT_LEFT | DT_WORDBREAK | DT_NOPREFIX;
	int nHeight1 = pDC->DrawText(strHeader1, &rcHeader1, uDrawFlags);

	if (!strHeader2.IsEmpty())
	{
		rcHeader2.top += nHeight1 + sizeText.cy / 4;
		if (rcHeader2.top < rcHeader2.bottom)
		{
			pDC->DrawText(strHeader2, &rcHeader2, uDrawFlags);
		}
	}

	if (!strHeaderWeekDay.IsEmpty())
	{
		fntDC.SetFont(&fntDateHeaderWeekDay);

		rcHeader3 = rcHeader2;
		rcHeader3.top += nHeight1 + sizeTextWD.cy / 5;

		if (rcHeader3.top < rcHeader3.bottom)
		{
			pDC->DrawText(strHeaderWeekDay, &rcHeader3, uDrawFlags);
		}
	}

	//---------------------------------------------------------------------------
	pDC->SetTextColor(clrPrev);
	pDC->SetBkMode(nBkModePrev);
}


void CXTPCalendarControlView::_DrawDateHeaderCalendar(BOOL bCalculate,
									CDC* pDC,
									CXTPCalendarControlViewPrintOptions* pOptions,
									COleDateTime dtYearMonth,
									CRect& rrcHeaderCalendar)
{
	LOGFONT lfCalWeek = pOptions->m_lfDateHeaderCalendarFont;
	lfCalWeek.lfHeight = lfCalWeek.lfHeight * 8 / 10;
	CFont fntCalendarWeek;
	VERIFY( fntCalendarWeek.CreateFontIndirect(&lfCalWeek) );

	CFont fntCalendar;
	VERIFY( fntCalendar.CreateFontIndirect(&pOptions->m_lfDateHeaderCalendarFont) );
	CXTPFontDC fntDC(pDC, &fntCalendar);


	CSize sizeCellText = pDC->GetTextExtent(_T("88"), 2);

	CSize sizeCell = sizeCellText;
	sizeCell.cx = sizeCell.cx * 13 / 10;
	sizeCell.cy = sizeCell.cy * 11 / 10;
	int nMonthRowY = sizeCellText.cy;
	int nWSaysRowY = sizeCellText.cy * 13 / 10;

	rrcHeaderCalendar.right = rrcHeaderCalendar.left + sizeCell.cx * 9;
	rrcHeaderCalendar.bottom = rrcHeaderCalendar.top + nMonthRowY + nWSaysRowY
								+ sizeCell.cy * 6;

	if (bCalculate)
	{
		return;
	}

	int nFirstDayOfWeek = GetCalendarCtrl().GetFirstDayOfWeek();

	COleDateTime dtMonthFirst = dtYearMonth;
	VERIFY( CXTPCalendarUtils::UpdateMonthDay(dtMonthFirst, 1) );
	int nMaxMonthDay = CXTPCalendarUtils::GetMaxMonthDay(dtYearMonth);

	CStringArray arWDNames;
	int i;
	for (i = 0; i < 7; i++)
	{
		CString sWD = CXTPCalendarUtils::GetLocaleString(LOCALE_SABBREVDAYNAME1 + ((i-1+7)%7), 100);
		arWDNames.Add(sWD);
	}
	int nMonth = dtYearMonth.GetMonth();
	int nYear = dtYearMonth.GetYear();
	CString strMonthName = CXTPCalendarUtils::GetLocaleString(LOCALE_SMONTHNAME1 + nMonth - 1, 100);
	CString strMonthYear;
	strMonthYear.Format(_T("%s  %d"), (LPCTSTR)strMonthName, nYear);

	//===========================================================================
	COLORREF clrPrev = pDC->SetTextColor(0);
	int nBkModePrev = pDC->SetBkMode(TRANSPARENT);
	//---------------------------------------------------------------------------
	CRect rcText = rrcHeaderCalendar;
	rcText.left += sizeCell.cx;
	pDC->DrawText(strMonthYear, &rcText, DT_TOP | DT_CENTER | DT_SINGLELINE);

	rcText = rrcHeaderCalendar;
	rcText.top += sizeCell.cy;
	for (i = 0; i < 7; i++)
	{
		rcText.left += sizeCell.cx;
		rcText.right = rcText.left + sizeCell.cx;

		int nMidx = (nFirstDayOfWeek-1+i) % 7;
		CString strWD = arWDNames[nMidx];

		pDC->DrawText((LPCTSTR)strWD, 1, &rcText, DT_TOP | DT_RIGHT | DT_SINGLELINE);
	}

	CRect rcLine1 = rrcHeaderCalendar;
	rcLine1.left += sizeCell.cx - 0;
	rcLine1.top += nMonthRowY + nWSaysRowY - 1;
	rcLine1.bottom = rcLine1.top + 1;

	CRect rcLine2 = rrcHeaderCalendar;
	rcLine2.left = rcLine1.left;
	rcLine2.right = rcLine2.left + 1;
	rcLine2.top = rcLine1.top;

	pDC->FillSolidRect(&rcLine1, 0);
	pDC->FillSolidRect(&rcLine2, 0);

	int nDay = 1 - (dtMonthFirst.GetDayOfWeek() - nFirstDayOfWeek + 7) % 7;

	int nFirstWeekOfYearDays = 1;
	COleDateTimeSpan spFWofY(nFirstWeekOfYearDays - 1);
	COleDateTime dtWeekLast = dtMonthFirst + COleDateTimeSpan(nDay+6-1);
	int nFirstWeekNumber = ((dtWeekLast - spFWofY).GetDayOfYear() - 1) / 7 + 1;

	//--------------------------
	int nTodayDay_toMark = 0;
	COleDateTime dtToday = CXTPCalendarUtils::GetCurrentTime();
	if (dtYearMonth.GetYear() == dtToday.GetYear() &&
		dtYearMonth.GetMonth() == dtToday.GetMonth())
	{
		nTodayDay_toMark = dtToday.GetDay();
	}

	CString strDay, strWeek;
	for (int w = 0; w < 6; w++)
	{
		rcText.top = rrcHeaderCalendar.top + nMonthRowY + nWSaysRowY + sizeCell.cy * w;

		if (nDay <= nMaxMonthDay)
		{
			CRect rcWeek = rcText;
			rcWeek.left = rrcHeaderCalendar.left;
			rcWeek.right = rcWeek.left + sizeCell.cx - 2;

			strWeek.Format(_T("%d"), nFirstWeekNumber + w);

			CXTPFontDC fntDCweek(pDC, &fntCalendarWeek);
			pDC->DrawText(strWeek, &rcWeek, DT_TOP | DT_RIGHT | DT_SINGLELINE);
		}

		//--------------------------------------------------------------------
		for (i = 0; i < 7; i++)
		{
			if (nDay > 0 && nDay <= nMaxMonthDay)
			{
				rcText.left = rrcHeaderCalendar.left + sizeCell.cx * (i + 1);
				rcText.right = rcText.left + sizeCell.cx;

				strDay.Format(_T("%d"), nDay);

				pDC->DrawText(strDay, &rcText, DT_TOP | DT_RIGHT | DT_SINGLELINE);

				if (nDay == nTodayDay_toMark)
				{
					CRect rcToday = rcText;
					rcToday.left += max(1, sizeCell.cx/10);
					rcToday.right += max(1, sizeCell.cx/10);
					rcToday.bottom = rcToday.top + sizeCell.cy;

					CBrush brFrame(RGB(82, 82, 82));
					pDC->FrameRect(&rcToday, &brFrame);
				}
			}
			nDay++;
		}
	}

	//---------------------------------------------------------------------------
	pDC->SetTextColor(clrPrev);
	pDC->SetBkMode(nBkModePrev);
}

int CXTPCalendarControlView::_GetDateHeaderMonths(COleDateTime& dtYearMonth1, COleDateTime& dtYearMonth2,
												  CString& strHeader1, CString& strHeader2,
												  CString& strHeaderWeekDay)
{
	strHeader1 = strHeader2 = strHeaderWeekDay = _T("");
	dtYearMonth1 = dtYearMonth2 = (DATE)0;

	SYSTEMTIME st;

	int nViewType = GetCalendarCtrl().GetActiveView()->GetViewType();
	if (nViewType == xtpCalendarDayView || nViewType == xtpCalendarWorkWeekView)
	{
		CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, GetCalendarCtrl().GetDayView());
		ASSERT(pDayView);
		if (!pDayView)
		{
			return 0;
		}

		dtYearMonth1 = pDayView->GetViewDayDate(0);
		GETASSYSTEMTIME_DT(dtYearMonth1, st);
		strHeader1 = CXTPCalendarUtils::GetDateFormat(&st, _T("d MMMM"));

		int nCount = pDayView->GetViewDayCount();
		if (nCount > 1)
		{
			dtYearMonth2 = pDayView->GetViewDayDate(nCount-1);
			GETASSYSTEMTIME_DT(dtYearMonth2, st);
			strHeader2 = CXTPCalendarUtils::GetDateFormat(&st, _T("d MMMM"));

			if (dtYearMonth1.GetMonth() != dtYearMonth2.GetMonth())
			{
				return 2;
			}
		}

		if (nCount == 1)
		{
			COleDateTime dtDay0 = pDayView->GetViewDayDate(0);
			int nWd = dtDay0.GetDayOfWeek();
			int nWdShift = (nWd-1 + 6)%7;

			strHeaderWeekDay = CXTPCalendarUtils::GetLocaleString(LOCALE_SDAYNAME1 + nWdShift, 100);
		}

		return 1;
	}
	else if (nViewType == xtpCalendarWeekView)
	{
		CXTPCalendarView* pView = GetCalendarCtrl().GetActiveView();
		ASSERT(pView);
		if (!pView)
		{
			return 0;
		}

		dtYearMonth1 = pView->GetViewDayDate(0);
		dtYearMonth2 = pView->GetViewDayDate(pView->GetViewDayCount()-1);

		CString strHeaderFormat = pView->GetDayHeaderFormat();

		GETASSYSTEMTIME_DT(dtYearMonth1, st);
		strHeader1 = CXTPCalendarUtils::GetDateFormat(&st, strHeaderFormat);

		GETASSYSTEMTIME_DT(dtYearMonth2, st);
		strHeader2 = CXTPCalendarUtils::GetDateFormat(&st, strHeaderFormat);

		return 1;
	}
	else if (nViewType == xtpCalendarMonthView)
	{
		CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView, GetCalendarCtrl().GetMonthView());

		if (!pMonthView || !pMonthView->GetGrid())
		{
			ASSERT(FALSE);
			return 0;
		}

		int nWeeksCount = pMonthView->GetGrid()->GetWeeksCount();
		ASSERT(nWeeksCount);
		if (nWeeksCount)
		{
			CXTPCalendarMonthViewDay* pMVday1 = pMonthView->GetGrid()->GetViewDay(0, 6);
			CXTPCalendarMonthViewDay* pMVday2 = pMonthView->GetGrid()->GetViewDay(nWeeksCount-1, 0);
			if (pMVday1 && pMVday2)
			{
				dtYearMonth1 = pMVday1->GetDayDate();

				GETASSYSTEMTIME_DT(dtYearMonth1, st);
				strHeader1 = CXTPCalendarUtils::GetDateFormat(&st, _T("MMMM yyyy"));

				COleDateTime dtLast = pMVday2->GetDayDate();
				if (dtYearMonth1.GetMonth() != dtLast.GetMonth())
				{
					dtYearMonth2 = dtLast;
					GETASSYSTEMTIME_DT(dtYearMonth2, st);
					strHeader2 = CXTPCalendarUtils::GetDateFormat(&st, _T("MMMM yyyy"));
					return 2;
				}
				return 1;
			}
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	return 0;
}

CRect CXTPCalendarControlView::_HimetricToLP(CDC* pDC, const CRect rcHimetric)
{
	CSize szLT(rcHimetric.left, rcHimetric.top);
	CSize szRB(rcHimetric.right, rcHimetric.bottom);

	pDC->HIMETRICtoLP(&szLT);
	pDC->HIMETRICtoLP(&szRB);

	CRect rcLP(szLT.cx, szLT.cy, szRB.cx, szRB.cy);

	return rcLP;
}


//////////////////////////////////////////////////////////////////////////
// Clipboard operations

void CXTPCalendarControlView::OnUndo()
{
	GetCalendarCtrl().OnUndo();
}

void CXTPCalendarControlView::OnCut()
{
	CWnd* pFocusWnd = GetFocus();
	if (pFocusWnd)
	{
		LRESULT lRes = pFocusWnd->SendMessage(WM_COMMAND, ID_EDIT_CUT, NULL);
		if (!lRes)
		{
			pFocusWnd->SendMessage(WM_CUT);
		}
	}
}

void CXTPCalendarControlView::OnCopy()
{
	CWnd* pFocusWnd = GetFocus();
	if (pFocusWnd)
	{
		LRESULT lRes = pFocusWnd->SendMessage(WM_COMMAND, ID_EDIT_COPY, NULL);
		if (!lRes)
		{
			pFocusWnd->SendMessage(WM_COPY);
		}
	}
}

void CXTPCalendarControlView::OnPaste()
{
	CWnd* pFocusWnd = GetFocus();
	if (pFocusWnd)
	{
		LRESULT lRes = pFocusWnd->SendMessage(WM_COMMAND, ID_EDIT_PASTE, NULL);
		if (!lRes)
		{
			pFocusWnd->SendMessage(WM_PASTE);
		}
	}
}

void CXTPCalendarControlView::OnUpdateCommand(CCmdUI* pCmdUI)
{
	GetCalendarCtrl().OnUpdateCmdUI(pCmdUI);
}

void CXTPCalendarControlView::OnFilePageSetup()
{
	DWORD dwFlags = PSD_MARGINS | PSD_INWININIINTLMEASURE;
	CXTPCalendarPageSetupDialog dlgPageSetup(GetPrintOptions(), dwFlags, this);

	XTPGetPrinterDeviceDefaults(dlgPageSetup.m_psd.hDevMode, dlgPageSetup.m_psd.hDevNames);

	int nDlgRes = (int)dlgPageSetup.DoModal();

	if (nDlgRes == IDOK)
	{
		AfxGetApp()->SelectPrinter(dlgPageSetup.m_psd.hDevNames, dlgPageSetup.m_psd.hDevMode, FALSE);
	}
}

