// XTPReportView.cpp : implementation of the CXTPReportView class.
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

#include "XTPReportView.h"
#include "XTPReportColumn.h"
#include "XTPReportColumns.h"
#include "XTPReportRecordItem.h"
#include "XTPReportRecordItemText.h"
#include "XTPReportInplaceControls.h"
#include "XTPReportRecord.h"
#include "XTPReportRecords.h"

#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPImageManager.h"

#include "Common/Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CXTPReportPageSetupDialog, CPageSetupDialog)
	ON_BN_CLICKED(XTP_IDC_HEADER_FORMAT_BTN, OnBnClickedHeaderFormat)
	ON_BN_CLICKED(XTP_IDC_FOOTER_FORMAT_BTN, OnBnClickedFooterFormat)
END_MESSAGE_MAP()

CXTPReportPageSetupDialog::CXTPReportPageSetupDialog(
	CXTPReportViewPrintOptions* pOptions,
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
}

CXTPReportPageSetupDialog::~CXTPReportPageSetupDialog()
{
}

#ifndef rad1
	#define rad1        0x0420
	#define rad2        0x0421

	#define grp4        0x0433
#endif

BOOL CXTPReportPageSetupDialog::OnInitDialog ()
{
	CPageSetupDialog::OnInitDialog();

	ASSERT(m_pOptions);

	VERIFY( m_ctrlHeaderFormat.SubclassDlgItem(XTP_IDC_HEADER_FORMAT, this) );
	VERIFY( m_ctrlFooterFormat.SubclassDlgItem(XTP_IDC_FOOTER_FORMAT, this) );

	VERIFY( m_ctrlHeaderFormatBtn.SubclassDlgItem(XTP_IDC_HEADER_FORMAT_BTN, this) );
	VERIFY( m_ctrlFooterFormatBtn.SubclassDlgItem(XTP_IDC_FOOTER_FORMAT_BTN, this) );

	if(m_pOptions && m_pOptions->GetPageHeader())
	{
		m_ctrlHeaderFormat.SetWindowText(m_pOptions->GetPageHeader()->m_strFormatString);
	}

	if(m_pOptions && m_pOptions->GetPageFooter())
	{
		m_ctrlFooterFormat.SetWindowText(m_pOptions->GetPageFooter()->m_strFormatString);
	}

	if (GetDlgItem(rad1))
		GetDlgItem(rad1)->EnableWindow(TRUE); //1056 Portrait
	if (GetDlgItem(rad2))
		GetDlgItem(rad2)->EnableWindow(TRUE); //1057 Landscape

	BOOL bIsInches = m_pOptions ? m_pOptions->IsMarginsMeasureInches() : FALSE;

	UINT uStrID = bIsInches ? XTP_IDS_REPORT_MARGINS_INCH : XTP_IDS_REPORT_MARGINS_MM;
	CString strCaption;
	VERIFY(XTPResourceManager()->LoadString(&strCaption, uStrID));

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

void CXTPReportPageSetupDialog::OnOK()
{
	if(m_pOptions && m_pOptions->GetPageHeader())
	{
		m_ctrlHeaderFormat.GetWindowText(m_pOptions->GetPageHeader()->m_strFormatString);
	}

	if(m_pOptions && m_pOptions->GetPageFooter())
	{
		m_ctrlFooterFormat.GetWindowText(m_pOptions->GetPageFooter()->m_strFormatString);
	}

	m_pOptions->m_rcMargins = m_psd.rtMargin;

	CPageSetupDialog::OnOK();
}

void CXTPReportPageSetupDialog::OnBnClickedHeaderFormat()
{
	CXTPPrintPageHeaderFooter::DoInsertHFFormatSpecifierViaMenu(
		this, &m_ctrlHeaderFormat, &m_ctrlHeaderFormatBtn);
}

void CXTPReportPageSetupDialog::OnBnClickedFooterFormat()
{
	CXTPPrintPageHeaderFooter::DoInsertHFFormatSpecifierViaMenu(
		this, &m_ctrlFooterFormat, &m_ctrlFooterFormatBtn);
}

/////////////////////////////////////////////////////////////////////////////
// CXTPReportView

IMPLEMENT_DYNCREATE(CXTPReportView, CView)

CXTPReportView::CXTPReportView()
{
	m_pReport = NULL;
	m_pPrintOptions = new CXTPReportViewPrintOptions();

	m_bPrintSelection = FALSE;

	m_bPrintDirect = FALSE;
	m_bResizeControlWithView = TRUE;

	m_bAllowCut = TRUE;
	m_bAllowPaste = TRUE;

	m_pScrollBar = NULL;

}

CXTPReportView::~CXTPReportView()
{
	CMDTARGET_RELEASE(m_pPrintOptions);
	SAFE_DELETE(m_pReport);
}


CXTPReportControl& CXTPReportView::GetReportCtrl() const
{
	return m_pReport == NULL ? (CXTPReportControl&)m_wndReport : *m_pReport;
}

void CXTPReportView::SetReportCtrl(CXTPReportControl* pReport)
{
	if (::IsWindow(m_wndReport.GetSafeHwnd()))
		m_wndReport.DestroyWindow();

	m_pReport = pReport;
}

void CXTPReportView::SetScrollBarCtrl(CScrollBar* pScrollBar)
{
	m_pScrollBar = pScrollBar;
}

BEGIN_MESSAGE_MAP(CXTPReportView, CView)
	//{{AFX_MSG_MAP(CXTPReportView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPReportView drawing

void CXTPReportView::OnDraw(CDC* /*pDC*/)
{
}

void CXTPReportView::OnPaint()
{
	Default();
}


/////////////////////////////////////////////////////////////////////////////
// CXTPReportView diagnostics

#ifdef _DEBUG
void CXTPReportView::AssertValid() const
{
	CView::AssertValid();
}

void CXTPReportView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXTPReportView message handlers

BOOL CXTPReportView::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

CScrollBar* CXTPReportView::GetScrollBarCtrl(int nBar) const
{
	if (nBar == SB_VERT && m_pScrollBar)
		return m_pScrollBar;

	return CView::GetScrollBarCtrl(nBar);
}

void CXTPReportView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_pScrollBar && pScrollBar == m_pScrollBar)
		GetReportCtrl().OnVScroll(nSBCode, nPos, 0);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CXTPReportView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pScrollBar && m_pScrollBar->GetSafeHwnd())
	{
		int nScroll = GetSystemMetrics(SM_CXVSCROLL);
		m_pScrollBar->MoveWindow(cx - nScroll, 0, nScroll, cy);
		cx -= nScroll;
	}

	if (m_bResizeControlWithView && GetReportCtrl().GetSafeHwnd())
	{
		GetReportCtrl().MoveWindow(0, 0, cx, cy);
	}
}

int CXTPReportView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!GetReportCtrl().Create(WS_CHILD | WS_TABSTOP | WS_VISIBLE ,
								CRect(0, 0, 0, 0), this, XTP_ID_REPORT_CONTROL))
	{
		TRACE(_T("Failed to create report control window\n"));
		return -1;
	}
	return 0;
}

void CXTPReportView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	GetReportCtrl().SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CReportSampleView printing

BOOL CXTPReportView::OnPreparePrinting(CPrintInfo* pInfo)
{
	if (GetReportCtrl().GetSelectedRows()->GetCount() > 0)
	{
		pInfo->m_pPD->m_pd.Flags &= ~PD_NOSELECTION;
	}

	pInfo->m_bDirect = m_bPrintDirect;

	// default preparation
	if (!DoPreparePrinting(pInfo))
		return FALSE;

	m_bPrintSelection = pInfo->m_pPD->PrintSelection();

	return TRUE;
}

void CXTPReportView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	ASSERT(m_aPageStart.GetSize() == 0);
	m_aPageStart.RemoveAll();
	m_aPageStart.Add(0);

	CString str1, str2;
	if (m_pPrintOptions && m_pPrintOptions->GetPageHeader())
		str1 = m_pPrintOptions->GetPageHeader()->m_strFormatString;
	if (m_pPrintOptions && m_pPrintOptions->GetPageFooter())
		str2 = m_pPrintOptions->GetPageFooter()->m_strFormatString;

	if (str1.Find(_T("&P")) >= 0 || str2.Find(_T("&P")) >= 0)
	{
		int nCurPage = pInfo->m_nCurPage;

		pInfo->m_nCurPage = 65535;

		if (PaginateTo(pDC, pInfo))
		{
			pInfo->SetMaxPage((int)m_aPageStart.GetSize() - 1);
		}

		pInfo->m_nCurPage = nCurPage;
	}
}

void CXTPReportView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_aPageStart.RemoveAll();
	m_bmpGrayDC.DeleteObject();
}

CXTPReportPaintManager* CXTPReportView::GetPaintManager() const
{
	return GetReportCtrl().GetPaintManager();
}

int CXTPReportView::GetColumnWidth(CXTPReportColumn* pColumnTest, int nTotalWidth)
{
	return pColumnTest->GetPrintWidth(nTotalWidth);
}

void CXTPReportView::PrintHeader(CDC* pDC, CRect rcHeader)
{
	GetPaintManager()->FillHeaderControl(pDC, rcHeader);

	int x = rcHeader.left;

	CXTPReportColumns* pColumns = GetReportCtrl().GetColumns();

	for (int i = 0; i < pColumns->GetCount(); i++)
	{
		CXTPReportColumn* pColumn = pColumns->GetAt(i);
		if (!pColumn->IsVisible())
			continue;

		int nWidth = GetColumnWidth(pColumn, rcHeader.Width());
		CRect rcItem(x, rcHeader.top, x + nWidth, rcHeader.bottom);

		GetPaintManager()->DrawColumn(pDC, pColumn, GetReportCtrl().GetReportHeader(), rcItem);

		x += nWidth;
	}
}

void CXTPReportView::PrintFooter(CDC* pDC, CRect rcFooter)
{
	GetPaintManager()->FillFooter(pDC, rcFooter);

	int x = rcFooter.left;

	CXTPReportColumns* pColumns = GetReportCtrl().GetColumns();

	for (int i = 0; i < pColumns->GetCount(); i++)
	{
		CXTPReportColumn* pColumn = pColumns->GetAt(i);
		if (!pColumn->IsVisible())
			continue;

		int nWidth = GetColumnWidth(pColumn, rcFooter.Width());
		CRect rcItem(x, rcFooter.top, x + nWidth, rcFooter.bottom);

		GetPaintManager()->DrawColumnFooter(pDC, pColumn, GetReportCtrl().GetReportHeader(), rcItem);

		x += nWidth;
	}
}

void CXTPReportView::PrintRow(CDC* pDC, CXTPReportRow* pRow, CRect rcRow, int nPreviewHeight)
{
	if (pRow->IsGroupRow())
	{
		pRow->Draw(pDC, rcRow, 0);
		return;
	}

	CXTPReportControl& wndReport = GetReportCtrl();

	XTP_REPORTRECORDITEM_DRAWARGS drawArgs;
	drawArgs.pDC = pDC;
	drawArgs.pControl = &wndReport;
	drawArgs.pRow = pRow;
	int nIndentWidth = wndReport.GetHeaderIndent();
	CXTPReportPaintManager* pPaintManager = GetPaintManager();

	CXTPReportColumns* pColumns = wndReport.GetColumns();
	int nColumnCount = pColumns->GetCount();

	// paint row background
	pPaintManager->FillRow(pDC, pRow, rcRow);

	CRect rcItem(rcRow);
	rcItem.bottom = rcItem.bottom - nPreviewHeight;

	CXTPReportRecord* pRecord = pRow->GetRecord();
	if (pRecord) // if drawing record, not group
	{
		BOOL bFirstVisibleColumn = TRUE;
		int x = rcRow.left;
		// paint record items
		for (int nColumn = 0; nColumn < nColumnCount; nColumn++)
		{
			CXTPReportColumn* pColumn = pColumns->GetAt(nColumn);
			if (pColumn && pColumn->IsVisible() && pRow->IsItemsVisible())
			{
				rcItem.left = x;
				x = rcItem.right = rcItem.left + GetColumnWidth(pColumn, rcRow.Width());

				if (bFirstVisibleColumn)
				{
					rcItem.left += nIndentWidth;
					bFirstVisibleColumn = FALSE;
				}

				CRect rcGridItem(rcItem);
				rcGridItem.left--;

				CXTPReportRecordItem* pItem = pRecord->GetItem(pColumn);

				if (pItem)
				{

					// draw item
					drawArgs.pColumn = pColumn;
					drawArgs.rcItem = rcItem;
					drawArgs.nTextAlign = pColumn->GetAlignment();
					drawArgs.pItem = pItem;
					// draw item
					pItem->Draw(&drawArgs);
				}

				pPaintManager->DrawGrid(pDC, TRUE, rcGridItem);
			}
		}

		if (nIndentWidth > 0)
		{
			// draw indent column
			CRect rcIndent(rcRow);
			rcIndent.right = rcRow.left + nIndentWidth;
			pPaintManager->FillIndent(pDC, rcIndent);
		}

		if (pRow->IsPreviewVisible())
		{
			CXTPReportRecordItemPreview* pItem = pRecord->GetItemPreview();

			CRect rcPreviewItem(rcRow);
			rcPreviewItem.DeflateRect(nIndentWidth, rcPreviewItem.Height() - nPreviewHeight, 0, 0);

			drawArgs.rcItem = rcPreviewItem;
			drawArgs.nTextAlign = DT_LEFT;
			drawArgs.pItem = pItem;
			drawArgs.pColumn = NULL;

			drawArgs.pItem->Draw(&drawArgs);
		}
	}

	BOOL bGridVisible = pPaintManager->IsGridVisible(FALSE);

	CRect rcFocus(rcRow.left, rcRow.top, rcRow.right, rcRow.bottom - (bGridVisible ? 1 : 0));

	if (pRow->GetIndex() < wndReport.GetRows()->GetCount() - 1 && nIndentWidth > 0)
	{
		CXTPReportRow* pNextRow = wndReport.GetRows()->GetAt(pRow->GetIndex() + 1);
		ASSERT(pNextRow);
		rcFocus.left = rcRow.left +  min(nIndentWidth, pPaintManager->m_nTreeIndent * pNextRow->GetTreeDepth());
	}

	pPaintManager->DrawGrid(pDC, FALSE, rcFocus);
}


int CXTPReportView::PrintRows(CDC* pDC, CRect rcClient, long nIndexStart)
{
	int y = rcClient.top;
	CXTPReportRows* pRows = GetReportCtrl().GetRows();

	for (; nIndexStart < pRows->GetCount(); nIndexStart++)
	{
		CXTPReportRow* pRow = pRows->GetAt(nIndexStart);

		if (m_bPrintSelection && !pRow->IsSelected())
		{
			continue;
		}

		int nHeight = pRow->GetHeight(pDC, rcClient.Width());
		int nPreviewHeight = 0;

		if (pRow->IsPreviewVisible())
		{
			CXTPReportRecordItemPreview* pItem = pRow->GetRecord()->GetItemPreview();
			nPreviewHeight = pItem->GetPreviewHeight(pDC, pRow, rcClient.Width());
			nHeight += nPreviewHeight;
		}

		CRect rcRow(rcClient.left, y, rcClient.right, y + nHeight);

		if (rcRow.bottom > rcClient.bottom)
			break;

		PrintRow(pDC, pRow, rcRow, nPreviewHeight);

		y += rcRow.Height();
	}
	return nIndexStart;
}


long CXTPReportView::PrintReport(CDC* pDC, CPrintInfo* /*pInfo*/, CRect rcPage, long nIndexStart)
{
	int nHeaderHeight = 0;
	int nFooterHeight = 0;

	if (GetReportCtrl().IsHeaderVisible())
		nHeaderHeight = GetPaintManager()->GetHeaderHeight(&GetReportCtrl(), pDC, rcPage.Width()-2);

	if (GetReportCtrl().IsFooterVisible())
		nFooterHeight = GetPaintManager()->GetFooterHeight(&GetReportCtrl(), pDC, rcPage.Width()-2);

	CRect rcHeader(rcPage.left + 1, rcPage.top + 1, rcPage.right - 1, rcPage.top + 1 + nHeaderHeight);
	CRect rcFooter(rcPage.left + 1, rcPage.bottom - nFooterHeight - 1, rcPage.right - 1, rcPage.bottom - 1);

	if (nHeaderHeight)
		PrintHeader(pDC, rcHeader);

	CRect rcRows(rcHeader.left, rcHeader.bottom, rcHeader.right, rcFooter.top);
	nIndexStart = PrintRows(pDC, rcRows, nIndexStart);

	if (nFooterHeight)
		PrintFooter(pDC, rcFooter);

	pDC->Draw3dRect(rcPage, 0, 0);

	return nIndexStart;
}

long CXTPReportView::PrintPage(CDC* pDC, CPrintInfo* pInfo, CRect rcPage, long nIndexStart)
{
	if (!m_pPrintOptions || !pDC || !pInfo)
	{
		ASSERT(FALSE);
		return INT_MAX;
	}

	CRect rcPageHeader = rcPage;
	CRect rcPageFooter = rcPage;

	CString strTitle = CXTPPrintPageHeaderFooter::GetParentFrameTitle(this);

	m_pPrintOptions->GetPageHeader()->FormatTexts(pInfo, strTitle);
	m_pPrintOptions->GetPageFooter()->FormatTexts(pInfo, strTitle);

	pDC->SetBkColor(RGB(255, 255, 255));
	m_pPrintOptions->GetPageFooter()->Draw(pDC, rcPageFooter, TRUE);
	m_pPrintOptions->GetPageHeader()->Draw(pDC, rcPageHeader);

	CRect rcReport = rcPage;

	rcReport.top += rcPageHeader.Height() + 2;
	rcReport.bottom -= rcPageFooter.Height() + 2;

	long nNextRow = PrintReport(pDC, pInfo, rcReport, nIndexStart);

	pDC->SetBkColor(RGB(255, 255, 255));
	m_pPrintOptions->GetPageFooter()->Draw(pDC, rcPageFooter);

	return nNextRow;
}

void CXTPReportView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
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

void CXTPReportView::_OnPrint2(CDC* pDC, CPrintInfo* pInfo, CRect rcPrint)
{
	if (!m_pPrintOptions || !pDC || !pInfo)
	{
		ASSERT(FALSE);
		return;
	}

	UINT nPage = pInfo->m_nCurPage;
	ASSERT(nPage <= (UINT)m_aPageStart.GetSize());
	UINT nIndex = m_aPageStart[nPage-1];

	// print as much as possible in the current page.
	nIndex = PrintPage(pDC, pInfo, rcPrint, nIndex);

	// update pagination information for page just printed
	if (nPage == (UINT)m_aPageStart.GetSize())
	{
		m_aPageStart.Add(nIndex);
	}
	else
	{
		ASSERT(nPage < (UINT)m_aPageStart.GetSize());
		m_aPageStart[nPage] = nIndex;
	}
}

extern BOOL CALLBACK _XTPAbortProc(HDC, int);

BOOL CXTPReportView::PaginateTo(CDC* pDC, CPrintInfo* pInfo)
	// attempts pagination to pInfo->m_nCurPage, TRUE == success
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	BOOL bAborted = FALSE;
	CXTPPrintingDialog dlgPrintStatus(this);
	CString strTemp;
	if (GetParentFrame())
		GetParentFrame()->GetWindowText(strTemp);

	dlgPrintStatus.SetWindowText(_T("Calculating pages..."));

	dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_DOCNAME, strTemp);
	dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME, pInfo->m_pPD->GetDeviceName());
	dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PORTNAME, pInfo->m_pPD->GetPortName());
	dlgPrintStatus.ShowWindow(SW_SHOW);
	dlgPrintStatus.UpdateWindow();

	CRect rectSave = pInfo->m_rectDraw;
	UINT nPageSave = pInfo->m_nCurPage;
	BOOL bBlackWhiteSaved = m_pPrintOptions->m_bBlackWhitePrinting;
	m_pPrintOptions->m_bBlackWhitePrinting = FALSE;

	ASSERT(nPageSave > 1);
	ASSERT(nPageSave >= (UINT)m_aPageStart.GetSize());
	VERIFY(pDC->SaveDC() != 0);

	pDC->IntersectClipRect(0, 0, 0, 0);
	pInfo->m_nCurPage = (UINT)m_aPageStart.GetSize();
	while (pInfo->m_nCurPage < nPageSave)
	{
		ASSERT(pInfo->m_nCurPage == (UINT)m_aPageStart.GetSize());
		OnPrepareDC(pDC, pInfo);
		if (!pInfo->m_bContinuePrinting)
			break;

		strTemp.Format(_T("%d"), pInfo->m_nCurPage);
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PAGENUM, strTemp);

		pInfo->m_rectDraw.SetRect(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		pDC->DPtoLP(&pInfo->m_rectDraw);
		OnPrint(pDC, pInfo);
		//if (pInfo->m_nCurPage == (UINT)m_aPageStart.GetSize())
		//  break;
		++pInfo->m_nCurPage;

		if(!_XTPAbortProc(0, 0))
		{
			bAborted = TRUE;
			break;
		}
	}
	dlgPrintStatus.DestroyWindow();

	BOOL bResult = !bAborted && (pInfo->m_nCurPage == nPageSave || nPageSave == 65535);

	pInfo->m_bContinuePrinting = bResult;

	pDC->RestoreDC(-1);
	m_pPrintOptions->m_bBlackWhitePrinting = bBlackWhiteSaved;
	pInfo->m_nCurPage = nPageSave;
	pInfo->m_rectDraw = rectSave;
	ASSERT_VALID(this);

	return bResult;
}

void CXTPReportView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT(pInfo != NULL);  // overriding OnPaint -- never get this.

	if (pInfo->m_nCurPage == 1 && GetReportCtrl().GetRows()->GetCount() == 0)
	{
		pInfo->m_bContinuePrinting = TRUE;
	}
	else if (pInfo->m_nCurPage == (UINT)m_aPageStart.GetSize() &&
		m_aPageStart[pInfo->m_nCurPage - 1] >= (UINT)GetReportCtrl().GetRows()->GetCount())
	{
		// can't paginate to that page, thus cannot print it.
		pInfo->m_bContinuePrinting = FALSE;
	}
	else if (pInfo->m_nCurPage > (UINT)m_aPageStart.GetSize() &&
		!PaginateTo(pDC, pInfo))
	{
		// can't paginate to that page, thus cannot print it.
		pInfo->m_bContinuePrinting = FALSE;
	}
	pDC->SetMapMode(MM_ANISOTROPIC);

	pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),
		pDC->GetDeviceCaps(LOGPIXELSY));

	pDC->SetWindowExt(100, 100);

	// ptOrg is in logical coordinates
	pDC->OffsetWindowOrg(0, 0);

	if (GetReportCtrl().GetExStyle() & WS_EX_RTLREADING)
	{
		pDC->SetTextAlign(TA_RTLREADING);
	}

}

//////////////////////////////////////////////////////////////////////////
// Clipboard operations

void CXTPReportView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetReportCtrl().CanCopy());
}

void CXTPReportView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bAllowCut && GetReportCtrl().CanCut());
}

void CXTPReportView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bAllowPaste && GetReportCtrl().CanPaste());
}

void CXTPReportView::OnEditCut()
{
	if (m_bAllowCut) GetReportCtrl().Cut();
}

void CXTPReportView::OnEditCopy()
{
	GetReportCtrl().Copy();
}

void CXTPReportView::OnEditPaste()
{
	if (m_bAllowPaste) GetReportCtrl().Paste();
}

void CXTPReportView::OnFilePageSetup()
{
	DWORD dwFlags = PSD_MARGINS | PSD_INWININIINTLMEASURE;
	CXTPReportPageSetupDialog dlgPageSetup(GetPrintOptions(), dwFlags, this);

	XTPGetPrinterDeviceDefaults(dlgPageSetup.m_psd.hDevMode, dlgPageSetup.m_psd.hDevNames);

	int nDlgRes = (int)dlgPageSetup.DoModal();

	if (nDlgRes == IDOK)
	{
		AfxGetApp()->SelectPrinter(dlgPageSetup.m_psd.hDevNames, dlgPageSetup.m_psd.hDevMode, FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
//class CXTPReportViewPrintOptions

IMPLEMENT_DYNAMIC(CXTPReportViewPrintOptions, CXTPPrintOptions)
CXTPReportViewPrintOptions::CXTPReportViewPrintOptions()
{
}

LCID CXTPReportViewPrintOptions::GetActiveLCID()
{
	return CXTPReportControlLocale::GetActiveLCID();
}

