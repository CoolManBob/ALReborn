// XTPSyntaxEditView.cpp : implementation file
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

// common includes
#include "Common/XTPImageManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceManager.h"

// syntax editor includes
#include "XTPSyntaxEditDefines.h"
#include "XTPSyntaxEditStruct.h"
#include "XTPSyntaxEditUndoManager.h"
#include "XTPSyntaxEditLineMarksManager.h"
#include "XTPSyntaxEditLexPtrs.h"
#include "XTPSyntaxEditLexClassSubObjT.h"
#include "XTPSyntaxEditTextIterator.h"
#include "XTPSyntaxEditSectionManager.h"
#include "XTPSyntaxEditLexCfgFileReader.h"
#include "XTPSyntaxEditLexClassSubObjDef.h"
#include "XTPSyntaxEditLexClass.h"
#include "XTPSyntaxEditLexParser.h"
#include "XTPSyntaxEditLexColorFileReader.h"
#include "XTPSyntaxEditBufferManager.h"
#include "XTPSyntaxEditToolTipCtrl.h"
#include "XTPSyntaxEditAutoCompleteWnd.h"
#include "XTPSyntaxEditFindReplaceDlg.h"
#include "XTPSyntaxEditCtrl.h"
#include "XTPSyntaxEditDoc.h"
#include "XTPSyntaxEditView.h"
#include "XTPSyntaxEditPaintManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD_PTR           CXTPSyntaxEditView::ms_dwSignature = 0;
BOOL                CXTPSyntaxEditView::ms_bDroppedHere = FALSE;
POINT              CXTPSyntaxEditView::ms_ptDropPos;
CXTPSyntaxEditView* CXTPSyntaxEditView::ms_pTargetView = NULL;

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditView

IMPLEMENT_DYNCREATE(CXTPSyntaxEditView, CView)

CXTPSyntaxEditView::CXTPSyntaxEditView()
: m_bVertScrollBar(TRUE)
, m_bHorzScrollBar(TRUE)
, m_pParentWnd(NULL)
, m_iTopRow(1)
, m_szPage(CSize(850,1100))
, m_nPrevTopRow(1)
, m_bOleDragging(FALSE)
, m_bFilesDragging(FALSE)
, m_bDraggingStartedHere(FALSE)
, m_bDraggingOver(FALSE)
, m_bSetPageSize(FALSE)
, m_bScrollBars(FALSE)
, m_nParserThreadPriority_WhenActive(THREAD_PRIORITY_LOWEST)
, m_nParserThreadPriority_WhenInactive(THREAD_PRIORITY_IDLE)
, m_bInitialUpdateWasCalled(FALSE)
{
	m_pEditCtrl = NULL;
	ZeroMemory(&m_lfPrevFont, sizeof(m_lfPrevFont));

	GetRegValues();
}

CXTPSyntaxEditView::~CXTPSyntaxEditView()
{
	CMDTARGET_RELEASE(m_pEditCtrl);
}

BOOL CXTPSyntaxEditView::GetRegValues()
{
	CWinApp* pWinApp = AfxGetApp();
	if (!pWinApp)
		return FALSE;

	m_bVertScrollBar = pWinApp->GetProfileInt(
		XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_VSCROLLBAR, m_bVertScrollBar);

	m_bHorzScrollBar = pWinApp->GetProfileInt(
		XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_HSCROLLBAR, m_bHorzScrollBar);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CXTPSyntaxEditView, CView)
	//{{AFX_MSG_MAP(CXTPSyntaxEditView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, OnUpdateEditReplace)
	ON_COMMAND(ID_EDIT_REPEAT, OnEditRepeat)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPEAT, OnUpdateEditRepeat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditView drawing

void CXTPSyntaxEditView::OnDraw(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	// Do nothing because actual drawing is inside the child edit control
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditView diagnostics

#ifdef _DEBUG
void CXTPSyntaxEditView::AssertValid() const
{
	CView::AssertValid();
}

void CXTPSyntaxEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditView command handlers

void CXTPSyntaxEditView::OnEditUndo()
{
	GetEditCtrl().Undo();
	UpdateSiblings();
	Invalidate(FALSE);
}

void CXTPSyntaxEditView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetEditCtrl().CanUndo());
}

void CXTPSyntaxEditView::OnEditRedo()
{
	GetEditCtrl().Redo();
	UpdateSiblings();
	Invalidate(FALSE);
}

void CXTPSyntaxEditView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetEditCtrl().CanRedo());
}

void CXTPSyntaxEditView::OnEditCut()
{
	GetEditCtrl().Cut();
}

void CXTPSyntaxEditView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetEditCtrl().IsSelectionExist());
}

void CXTPSyntaxEditView::OnEditCopy()
{
	GetEditCtrl().Copy();
}

void CXTPSyntaxEditView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetEditCtrl().IsSelectionExist());
}

void CXTPSyntaxEditView::OnEditPaste()
{
	GetEditCtrl().Paste();
}

void CXTPSyntaxEditView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsClipboardFormatAvailable(CF_TEXT));
}

void CXTPSyntaxEditView::OnEditDelete()
{
	GetEditCtrl().DeleteSelection();
}

void CXTPSyntaxEditView::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetEditCtrl().IsSelectionExist());
}

void CXTPSyntaxEditView::OnEditSelectAll()
{
	GetEditCtrl().SelectAll();
}

void CXTPSyntaxEditView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CXTPSyntaxEditView::OnEditFind()
{
	CString csSelText;
	GetEditCtrl().GetSelectionText(csSelText);

	if (csSelText.IsEmpty())
	{
		CPoint pt(GetCaretPos());
		pt.y += 2;

		GetEditCtrl().SelectWord(pt);
		GetEditCtrl().GetSelectionText(csSelText);
	}

	csSelText = csSelText.SpanExcluding(_T("\r\n"));
	REPLACE_S(csSelText, _T("\t"), _T("    "));

	if (!csSelText.IsEmpty())
	{
		m_dlgFindReplace.m_csFindText = csSelText;
	}

	m_dlgFindReplace.ShowDialog(this);
}

void CXTPSyntaxEditView::OnUpdateEditFind(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CXTPSyntaxEditView::OnEditReplace()
{
	CString csSelText;
	GetEditCtrl().GetSelectionText(csSelText);

	csSelText = csSelText.SpanExcluding(_T("\r\n"));
	REPLACE_S(csSelText, _T("\t"), _T("    "));

	if (!csSelText.IsEmpty())
	{
		m_dlgFindReplace.m_csFindText = csSelText;
	}

	m_dlgFindReplace.ShowDialog(this, TRUE);
}

void CXTPSyntaxEditView::OnUpdateEditReplace(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CXTPSyntaxEditView::OnEditRepeat()
{
	BOOL bShiftKey  = (::GetKeyState(VK_SHIFT) & KF_UP) != 0;

	if (GetEditCtrl().Find(
		m_dlgFindReplace.m_csFindText,
		m_dlgFindReplace.m_bMatchWholeWord,
		m_dlgFindReplace.m_bMatchCase, !bShiftKey))
	{
		SetDirty();
	}
	else
	{
		AfxMessageBox(XTPResourceManager()->LoadString(XTP_IDS_EDIT_MSG_FSEARCH));
	}
}

void CXTPSyntaxEditView::OnUpdateEditRepeat(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_dlgFindReplace.m_csFindText.IsEmpty());
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditView message handlers

int CXTPSyntaxEditView::OnCreate(LPCREATESTRUCT lpCS)
{
	if (CView::OnCreate(lpCS) == -1)
		return -1;

	// initialize parent window pointer.
	m_pParentWnd = CWnd::FromHandlePermanent(lpCS->hwndParent);
	if (!::IsWindow(m_pParentWnd->GetSafeHwnd()))
		return -1;

	// get data manager and context pointers.
	CXTPSyntaxEditBufferManager* pDataManager = GetDataManager();
	CCreateContext* pContext = (CCreateContext*)lpCS->lpCreateParams;

	// if the parent window is a splitter do not use scrollbars.
	m_bScrollBars = !m_pParentWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd));

	// if the parent window is a frame window do not use scrollbars as well.
	m_bScrollBars &= !m_pParentWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd));
	m_bScrollBars = m_bScrollBars || !GetEditCtrl().IsCreateScrollbarOnParent();

	// create the edit control.
	if (!GetEditCtrl().Create(this, m_bScrollBars, m_bScrollBars, pDataManager, pContext))
	{
		TRACE0("Failed to create edit control.\n");
		return -1;
	}

	GetEditCtrl().RecalcScrollBars();
	GetEditCtrl().SetTopRow(m_iTopRow);
	GetEditCtrl().EnableOleDrag(m_dropTarget.Register(this));

	// get the document.
	CXTPSyntaxEditDoc *pDoc = GetDocument();
	if (!pDoc)
	{
		GetEditCtrl().GetEditBuffer()->InsertText(_T(""), 1,1, FALSE);
		return 0;
	}

	// Inherit any special back and foreground color lines
	CXTPSyntaxEditView *pView = pDoc->GetFirstView();
	if (pView != this && ::IsWindow(pView->GetSafeHwnd()))
		GetEditCtrl() = pView->GetEditCtrl();

	if (!pDataManager)
		pDoc->SetDataManager(GetEditCtrl().GetEditBuffer());

	return 0;
}

void CXTPSyntaxEditView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (::IsWindow(GetEditCtrl().m_hWnd))
	{
		CXTPClientRect rc(this);
		GetEditCtrl().MoveWindow(&rc, FALSE);
		GetEditCtrl().RecalcScrollBars();
	}
}

void CXTPSyntaxEditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (GetEditCtrl().SendMessage(WM_KEYDOWN,
		nChar, nFlags) != 0)
	{
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CXTPSyntaxEditView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (GetEditCtrl().SendMessage(WM_CHAR,
		(WPARAM)nChar, (LPARAM)nFlags) != 0)
	{
		CView::OnChar(nChar, nRepCnt, nFlags);
	}
}

void CXTPSyntaxEditView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (GetEditCtrl().SendMessage(WM_VSCROLL,
		MAKEWPARAM(nSBCode, nPos), (LPARAM)pScrollBar->GetSafeHwnd()) != 0)
	{
		CView::OnVScroll(nSBCode, nPos, pScrollBar);
	}
}

void CXTPSyntaxEditView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (GetEditCtrl().SendMessage(WM_HSCROLL,
		MAKEWPARAM(nSBCode, nPos), (LPARAM)pScrollBar->GetSafeHwnd()) != 0)
	{
		CView::OnHScroll(nSBCode, nPos, pScrollBar);
	}
}

void CXTPSyntaxEditView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	OnUpdate(NULL, xtpEditHintInitView);

	m_bInitialUpdateWasCalled = TRUE;
}

BOOL CXTPSyntaxEditView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

int CXTPSyntaxEditView::GetTopRow()
{
	return GetEditCtrl().GetTopRow();
}

BOOL CXTPSyntaxEditView::SetTopRow(int iRow)
{
	m_iTopRow = iRow;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnEditChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	//::SetWindowLongPtr(pNMHDR->hwndFrom, GWLP_USERDATA, CanChangeReadonlyFile());

	*pResult = !CanChangeReadonlyFile();
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnEditChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	XTP_EDIT_NMHDR_EDITCHANGED* pNMHDR_EC = (XTP_EDIT_NMHDR_EDITCHANGED*)pNMHDR;
	UpdateSiblings(pNMHDR_EC);

	*pResult = 1;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnSetDocModified(NMHDR* pNMHDR, LRESULT* pResult)
{
	XTP_EDIT_NMHDR_DOCMODIFIED* pNMHDR_DM = (XTP_EDIT_NMHDR_DOCMODIFIED*)pNMHDR;

	CDocument *pDoc = GetDocument();
	if (!pDoc)
		return TRUE;

	pDoc->SetModifiedFlag(pNMHDR_DM->bModified);
	const CString& strTitle = pDoc->GetTitle();

	int nPos = strTitle.ReverseFind(_T('*'));

	if (pNMHDR_DM->bModified)
	{
		if (nPos == -1)
		{
			CString strNewTitle = strTitle;
			strNewTitle += _T(" *");
			pDoc->SetTitle(strNewTitle);
		}
	}
	else
	{
		if (nPos > 0)
		{
			CString strNewTitle = strTitle.Left(nPos - 1);
			pDoc->SetTitle(strNewTitle);
		}
	}

	*pResult = 1;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnDrawBookmark(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = 0;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnRowColChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	XTP_EDIT_NMHDR_ROWCOLCHANGED* pNMHDR_RCC = (XTP_EDIT_NMHDR_ROWCOLCHANGED*)pNMHDR;
	UNREFERENCED_PARAMETER(pNMHDR_RCC);

	*pResult = 0;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnUpdateScrollPos(NMHDR* pNMHDR, LRESULT* pResult)
{
	XTP_EDIT_NMHDR_SETSCROLLPOS* pNMHDR_SSP = (XTP_EDIT_NMHDR_SETSCROLLPOS*)pNMHDR;

	CDocument *pDoc = GetDocument();
	if (pDoc)
	{
		pDoc->UpdateAllViews(NULL, xtpEditHintUpdateScrollPos, (CObject*)pNMHDR_SSP);
	}

	*pResult = 1;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnInsertKey(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = 0;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnSelInit(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	UpdateSiblings();

	*pResult = 0;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnStartOleDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	StartOleDrag();

	*pResult = 0;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnMarginClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
	XTP_EDIT_NMHDR_MARGINCLICKED* pNMMarginClicked = (XTP_EDIT_NMHDR_MARGINCLICKED*)pNMHDR;
	UNREFERENCED_PARAMETER(pNMMarginClicked);

	*pResult = 0;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnParseEvent(NMHDR* pNMHDR, LRESULT* pResult)
{
	XTP_EDIT_NMHDR_PARSEEVENT* pNMParseEvent = (XTP_EDIT_NMHDR_PARSEEVENT*)pNMHDR;
	UNREFERENCED_PARAMETER(pNMParseEvent);

	*pResult = 0;
	return TRUE;
}

BOOL CXTPSyntaxEditView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;

	if (pNMHDR && pNMHDR->hwndFrom == GetEditCtrl().m_hWnd)
	{
		switch (pNMHDR->code)
		{
		case XTP_EDIT_NM_EDITCHANGING:
			return OnEditChanging(pNMHDR, pResult);

		case XTP_EDIT_NM_EDITCHANGED:
			return OnEditChanged(pNMHDR, pResult);

		case XTP_EDIT_NM_SETDOCMODIFIED:
			return OnSetDocModified(pNMHDR, pResult);

		case XTP_EDIT_NM_DRAWBOOKMARK:
			return OnDrawBookmark(pNMHDR, pResult);

		case XTP_EDIT_NM_ROWCOLCHANGED:
			return OnRowColChanged(pNMHDR, pResult);

		case XTP_EDIT_NM_UPDATESCROLLPOS:
			return OnUpdateScrollPos(pNMHDR, pResult);

		case XTP_EDIT_NM_INSERTKEY:
			return OnInsertKey(pNMHDR, pResult);

		case XTP_EDIT_NM_SELINIT:
			return OnSelInit(pNMHDR, pResult);

		case XTP_EDIT_NM_STARTOLEDRAG:
			return OnStartOleDrag(pNMHDR, pResult);

		case XTP_EDIT_NM_MARGINCLICKED:
			return OnMarginClicked(pNMHDR, pResult);

		case XTP_EDIT_NM_PARSEEVENT:
			return OnParseEvent(pNMHDR, pResult);
		}
	}

	return CView::OnNotify(wParam, lParam, pResult);
}

void CXTPSyntaxEditView::Refresh()
{
	CWaitCursor wait;

	GetEditCtrl().RefreshColors();

	m_iTopRow = GetEditCtrl().GetTopRow();

	if (m_iTopRow != -1)
		GetEditCtrl().SetTopRow(m_iTopRow);

	GetEditCtrl().RecalcScrollBars();
	GetEditCtrl().NotifyCurRowCol(GetEditCtrl().GetCurRow(), GetEditCtrl().GetCurCol());
}

void CXTPSyntaxEditView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	GetEditCtrl().SetFocus();
}

void CXTPSyntaxEditView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	UNREFERENCED_PARAMETER(pInfo);

//  CSize sizeDoc;
//  PRINTDLG printdlg;
//
//  CWinApp* pWinApp = AfxGetApp();
//  if (pWinApp && pWinApp->GetPrinterDeviceDefaults(&printdlg))
//  {
//      DEVMODE *pdevMode = (DEVMODE *)::GlobalLock(printdlg.hDevMode);
//      sizeDoc.cx = (int)((float)pdevMode->dmPaperWidth / 2.00f);
//      sizeDoc.cy = (int)((float)pdevMode->dmPaperLength / 2.00f);
//      ::GlobalUnlock(printdlg.hDevMode);
//  }
//  else
//  {
//      sizeDoc = m_szPage;
//  }
//
//  if (sizeDoc == CSize(0, 0))
//      sizeDoc = m_szPage;
//
//  pDC->SetMapMode(MM_ANISOTROPIC);
//
//  pDC->SetWindowExt(sizeDoc);
//
//  CSize sizeNum(1,1), sizeDenom(1,1);
//
//  int x = pDC->GetDeviceCaps(LOGPIXELSX);
//  int y = pDC->GetDeviceCaps(LOGPIXELSY);
//
//  long xExt = (long)sizeDoc.cx * x * sizeNum.cx;
//  xExt /= 100 * (long)sizeDenom.cx;
//  long yExt = (long)sizeDoc.cy * y * sizeNum.cy;
//  yExt /= 100 * (long)sizeDenom.cy;
//
//  pDC->SetViewportExt((int)xExt, (int)yExt);

	pDC->SetMapMode(MM_ANISOTROPIC);

	pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),
						pDC->GetDeviceCaps(LOGPIXELSY));

	pDC->SetWindowExt(100, 100);

	// ptOrg is in logical coordinates
	pDC->OffsetWindowOrg(0, 0);
}

BOOL CXTPSyntaxEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	XTPSyntaxEditPaintManager()->GetFont()->GetLogFont(&m_lfPrevFont);
	LOGFONT lfPrint = m_lfPrevFont;
	STRCPY_S(lfPrint.lfFaceName, LF_FACESIZE, _T("Courier New"));
	XTPSyntaxEditPaintManager()->SetFont(&lfPrint);

	m_bSetPageSize = TRUE;

	m_nPrevTopRow = GetEditCtrl().GetTopRow();
	int nPageCount = GetEditCtrl().CalcPageCount();

	CWinApp* pWinApp = AfxGetApp();
	if (pWinApp)
		pWinApp->m_nNumPreviewPages = nPageCount;

	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(nPageCount);

	XTPSyntaxEditPaintManager()->SetFont(&m_lfPrevFont);

	return DoPreparePrinting(pInfo);
}

void CXTPSyntaxEditView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	GetEditCtrl().SetRedraw(FALSE);
	m_nPrevTopRow = GetEditCtrl().GetTopRow();

	CView::OnBeginPrinting(pDC, pInfo);
}

void CXTPSyntaxEditView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnEndPrinting(pDC, pInfo);

	XTPSyntaxEditPaintManager()->SetFont(&m_lfPrevFont);

	GetEditCtrl().SetTopRow(m_nPrevTopRow);
	GetEditCtrl().SetDirtyRow(-1);
	GetEditCtrl().SetRedraw(TRUE);

}

void CXTPSyntaxEditView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	if (m_bSetPageSize)
	{
		GetEditCtrl().SetPageSize(pInfo->m_rectDraw.Size());
		m_bSetPageSize = FALSE;
	}

	CRect rcClient = pInfo->m_rectDraw;
	rcClient.DeflateRect(10, 10, 10, 10);

	pDC->IntersectClipRect(&rcClient);

	LOGFONT lfPrint = m_lfPrevFont;
	STRCPY_S(lfPrint.lfFaceName, LF_FACESIZE, _T("Courier New"));
	XTPSyntaxEditPaintManager()->SetFont(&lfPrint);

	GetEditCtrl().SetCurrentPage(pInfo->m_nCurPage);
	GetEditCtrl().SetDirtyRow(-1);
	GetEditCtrl().DrawEditControl(pDC);

	XTPSyntaxEditPaintManager()->SetFont(&m_lfPrevFont);

	pDC->SelectClipRgn(NULL);
}

CXTPSyntaxEditCtrl& CXTPSyntaxEditView::GetEditCtrl()
{
	return m_pEditCtrl ? *m_pEditCtrl : m_wndEditCtrl;
}

void CXTPSyntaxEditView::SetEditCtrl(CXTPSyntaxEditCtrl* pControl)
{
	if (::IsWindow(m_wndEditCtrl.GetSafeHwnd()))
		m_wndEditCtrl.DestroyWindow();

	m_pEditCtrl = pControl;
}


void CXTPSyntaxEditView::UpdateScrollPos(DWORD dwUpdate/* = XTP_EDIT_UPDATE_ALL*/)
{
	CSplitterWnd *pSplitterWnd = GetParentSplitter(this, FALSE);
	if (pSplitterWnd != NULL && GetEditCtrl().IsCreateScrollbarOnParent())
	{
		//  See CSplitterWnd::IdFromRowCol() implementation for details
		int nCurrentRow = (GetDlgCtrlID() - AFX_IDW_PANE_FIRST) / 16;
		int nCurrentCol = (GetDlgCtrlID() - AFX_IDW_PANE_FIRST) % 16;

		if (nCurrentRow >= pSplitterWnd->GetRowCount())
			nCurrentRow = (pSplitterWnd->GetRowCount() - 1);

		if (nCurrentCol >= pSplitterWnd->GetColumnCount())
			nCurrentCol = (pSplitterWnd->GetColumnCount() - 1);

		ASSERT(nCurrentRow >= 0 && nCurrentRow < pSplitterWnd->GetRowCount());
		ASSERT(nCurrentCol >= 0 && nCurrentCol < pSplitterWnd->GetColumnCount());

		int nCols = pSplitterWnd->GetColumnCount();
		int nRows = pSplitterWnd->GetRowCount();

		for (int nRow = 0; nRow < nRows; nRow++)
		{
			for (int nCol = 0; nCol < nCols; nCol++)
			{
				CXTPSyntaxEditView *pSiblingView = GetSplitterView(nRow, nCol);
				if (!pSiblingView || pSiblingView == this)
				{
					continue;
				}

				if ((dwUpdate & XTP_EDIT_UPDATE_HORZ) && (nRow == nCurrentRow) && (nCol != nCurrentCol))
					pSiblingView->UpdateSiblingScrollPos(this, XTP_EDIT_UPDATE_HORZ);

				if ((dwUpdate & XTP_EDIT_UPDATE_VERT) && (nRow != nCurrentRow) && (nCol == nCurrentCol))
					pSiblingView->UpdateSiblingScrollPos(this, XTP_EDIT_UPDATE_VERT);

				if ((dwUpdate & XTP_EDIT_UPDATE_DIAG) && (nRow != nCurrentRow) && (nCol != nCurrentCol))
					pSiblingView->UpdateSiblingScrollPos(this, XTP_EDIT_UPDATE_DIAG);
			}
		}
	}
	else
	{
		GetEditCtrl().RecalcScrollBars();
		GetEditCtrl().SetDirtyRow(-1);
		GetEditCtrl().Invalidate(FALSE);
		GetEditCtrl().UpdateWindow();
	}
}

void CXTPSyntaxEditView::UpdateSiblingScrollPos(CXTPSyntaxEditView *pSender, DWORD dwUpdate)
{
	ASSERT_VALID(pSender);
	ASSERT_KINDOF(CXTPSyntaxEditView, pSender);

	if (pSender == this)
		return;

	CFrameWnd *pFrameWnd1 = GetParentFrame();
	CFrameWnd *pFrameWnd2 = pSender->GetParentFrame();

	GetEditCtrl().SetDirtyRow(-1);
	GetEditCtrl().RecalcScrollBars();

	switch (dwUpdate & XTP_EDIT_UPDATE_ALL)
	{
	case XTP_EDIT_UPDATE_HORZ:
		{
			if (pFrameWnd1 == pFrameWnd2)
			{
				int nDispRow = pSender->GetEditCtrl().GetTopRow();
				GetEditCtrl().SetTopRow(nDispRow);
			}
			GetEditCtrl().Invalidate(FALSE);
			GetEditCtrl().UpdateWindow();
		}
		break;

	case XTP_EDIT_UPDATE_VERT:
		{
			if (pFrameWnd1 == pFrameWnd2)
			{
				GetEditCtrl().EnsureVisible(GetEditCtrl().GetTopRow(),
					pSender->GetEditCtrl().GetCurCol(), TRUE, FALSE);
				GetEditCtrl().SetCurCaretPos(GetEditCtrl().GetCurRow(),
					GetEditCtrl().GetCurCol(), FALSE);
			}
			GetEditCtrl().Invalidate(FALSE);
			GetEditCtrl().UpdateWindow();
		}
		break;

	case XTP_EDIT_UPDATE_DIAG:
		{
			GetEditCtrl().Invalidate(FALSE);
			GetEditCtrl().UpdateWindow();

			if (pFrameWnd1 == pFrameWnd2)
			{
				GetEditCtrl().SetCurCaretPos(GetEditCtrl().GetCurRow(),
					GetEditCtrl().GetCurCol(), FALSE);
			}
		}
		break;
	}
}

CXTPSyntaxEditView *CXTPSyntaxEditView::GetSplitterView(int nRow, int nCol)
{
	CSplitterWnd *pSplitterWnd = GetParentSplitter(this, FALSE);

	if (pSplitterWnd != NULL)
	{
		return DYNAMIC_DOWNCAST(CXTPSyntaxEditView, pSplitterWnd->GetPane(nRow, nCol));
	}

	return NULL;
}

BOOL CXTPSyntaxEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// As CView provides its own window class and own drawing code
	// do not call CView::PreCreateWindow

	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

void CXTPSyntaxEditView::UpdateSiblings(XTP_EDIT_NMHDR_EDITCHANGED* pNMHDR_EC/* = NULL*/)
{
	if (!GetDocument())
		return;

	CXTPSyntaxEditView* pView = GetDocument()->GetFirstView();
	if (!pView)
	{
		return;
	}

	CSplitterWnd *pSplitterWnd = GetParentSplitter(pView, FALSE);
	if (!pSplitterWnd || !GetEditCtrl().IsCreateScrollbarOnParent())
	{
		return;
	}

	//  See CSplitterWnd::IdFromRowCol() implementation for details
	int nCurrentRow = (GetDlgCtrlID() - AFX_IDW_PANE_FIRST) / 16;
	int nCurrentCol = (GetDlgCtrlID() - AFX_IDW_PANE_FIRST) % 16;

	if (nCurrentRow >= pSplitterWnd->GetRowCount())
		nCurrentRow = (pSplitterWnd->GetRowCount() - 1);

	if (nCurrentCol >= pSplitterWnd->GetColumnCount())
		nCurrentCol = (pSplitterWnd->GetColumnCount() - 1);

	ASSERT(nCurrentRow >= 0 && nCurrentRow < pSplitterWnd->GetRowCount());
	ASSERT(nCurrentCol >= 0 && nCurrentCol < pSplitterWnd->GetColumnCount());

	int nCols = pSplitterWnd->GetColumnCount();
	int nRows = pSplitterWnd->GetRowCount();

	for (int nCol = 0; nCol < nCols; nCol ++)
	{
		for (int nRow = 0; nRow < nRows; nRow ++)
		{
			//  We don't need to update ourselves
			if ((nCol == nCurrentCol && nRow == nCurrentRow) /*|| pView == this*/)
			{
				continue;
			}

			CXTPSyntaxEditView *pSiblingView = pView->GetSplitterView(nRow, nCol);
			if (!pSiblingView || pSiblingView == this)
			{
				continue;
			}

			if (pNMHDR_EC)
			{
				CXTPSyntaxEditCtrl& wndEditCtrl = pSiblingView->GetEditCtrl();
				wndEditCtrl.SetDirtyRow(pNMHDR_EC->nRowFrom);
				wndEditCtrl.InvalidateRows(pNMHDR_EC->nRowFrom);
				wndEditCtrl.UpdateWindow();
			}
			else
			{
				CXTPSyntaxEditCtrl& wndEditCtrl = pSiblingView->GetEditCtrl();
				wndEditCtrl.Invalidate(FALSE);
				wndEditCtrl.SetDirtyRow(-1);
				wndEditCtrl.ClearSelection();
			}
		}
	}
}

void CXTPSyntaxEditView::SetDirty()
{
	GetEditCtrl().SetDirtyRow(-1);
	GetEditCtrl().Invalidate(FALSE);
}

AFX_STATIC BOOL AFX_CDECL EnsureViewIsActive(CView* pView)
{
	// Get a pointer to the view's parent frame.
	CFrameWnd* pParentFrame = DYNAMIC_DOWNCAST(
		CMDIChildWnd, pView->GetParentFrame());

	if (pParentFrame)
	{
		// If the top level frame is MDI activate.
		CMDIFrameWnd* pMDIFrameWnd = DYNAMIC_DOWNCAST(
			CMDIFrameWnd, pView->GetTopLevelFrame());

		if (pMDIFrameWnd)
		{
			if (pMDIFrameWnd->MDIGetActive() != pParentFrame)
				pMDIFrameWnd->MDIActivate(pParentFrame);
		}

		// If nested in a splitter set active pane.
		CSplitterWnd* pSplitterWnd = DYNAMIC_DOWNCAST(
			CSplitterWnd, pView->GetParent());

		if (pSplitterWnd)
		{
			if (pSplitterWnd->GetActivePane() != pView)
				pSplitterWnd->SetActivePane(0, 0, pView);
		}
		else
		{
			if (pParentFrame->GetActiveView() != pView)
				pParentFrame->SetActiveView(pView);
		}

		return TRUE;
	}

	return FALSE;
}

DROPEFFECT CXTPSyntaxEditView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
#ifdef _UNICODE
	m_bOleDragging = pDataObject->IsDataAvailable(CF_UNICODETEXT);
#else
	m_bOleDragging = pDataObject->IsDataAvailable(CF_TEXT);
#endif

	m_bFilesDragging = pDataObject->IsDataAvailable(CF_HDROP);

	ms_bDroppedHere = FALSE;

	EnsureViewIsActive(this);

	return CView::OnDragEnter(pDataObject, dwKeyState, point);
}

void CXTPSyntaxEditView::OnDragLeave()
{
	m_bOleDragging = FALSE;
	m_bFilesDragging = FALSE;
	ms_bDroppedHere = FALSE;

	CView::OnDragLeave();
}

DROPEFFECT CXTPSyntaxEditView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	UNREFERENCED_PARAMETER(pDataObject);

	if (m_bFilesDragging)
	{
		return DROPEFFECT_COPY;
	}

	if (!m_bOleDragging)
		return DROPEFFECT_NONE;

	DROPEFFECT effect;

	if (dwKeyState & MK_CONTROL)
		effect = DROPEFFECT_COPY;
	else
		effect = DROPEFFECT_MOVE;

	int nRow, nCol, nAbsCol;

	GetEditCtrl().RowColFromPoint(point, &nRow, &nCol);
	GetEditCtrl().ValidateCol(nRow, nCol, nAbsCol);

	int iPrevRow = GetEditCtrl().GetCurRow();
	int iPrevCol = GetEditCtrl().GetCurCol();

	int nTopRow = GetEditCtrl().GetTopRow();
	int nRowPerPage = GetEditCtrl().GetRowPerPage();
	int nEndRow = GetEditCtrl().GetRowCount();

	if (nRow == nTopRow && nRow > 1)
		nRow--;

	if (nRow == (nTopRow + nRowPerPage - 1) && nRow < nEndRow)
		nRow++;

	if (nRow != iPrevRow || nCol != iPrevCol)
	{
		GetEditCtrl().SetDirtyRow(-1);
		GetEditCtrl().SetCurPos(nRow, nCol, TRUE, TRUE);
	}

	TRACE(_T("\nOnDragOver top row = %d"), GetEditCtrl().GetTopRow());

	if (::GetFocus() != GetEditCtrl().m_hWnd)
		::SetFocus(GetEditCtrl().m_hWnd);

	m_bDraggingOver = TRUE;
	ms_bDroppedHere = FALSE;

	return effect;
}

BOOL CXTPSyntaxEditView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect,
							   CPoint point)
{
	UNREFERENCED_PARAMETER(dropEffect); UNREFERENCED_PARAMETER(point);

	if (m_bFilesDragging)
	{
		HGLOBAL hDropInfo = pDataObject->GetGlobalData(CF_HDROP);
		OnDropFiles((HDROP)hDropInfo);

		m_bFilesDragging = FALSE;
		return TRUE;
	}

	m_bOleDragging = FALSE;

	TRACE(_T("\nOnDragDrop top row = %d"), GetEditCtrl().GetTopRow());

	if (ms_dwSignature != (DWORD_PTR)(GetEditCtrl().GetEditBuffer()))
	{
#ifdef _UNICODE
		HGLOBAL hMem = pDataObject->GetGlobalData(CF_UNICODETEXT);
#else
		HGLOBAL hMem = pDataObject->GetGlobalData(CF_TEXT);
#endif
		LPTSTR szText = (LPTSTR)GlobalLock(hMem);

		int iRow = GetEditCtrl().GetCurRow();
		int iAbsCol = GetEditCtrl().GetCurAbsCol();

		GetEditCtrl().Unselect();
		GetEditCtrl().InsertString(szText, iRow, iAbsCol, FALSE);
		GetEditCtrl().GetEditBuffer()->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_PASTE);

		if (GetDocument())
			GetDocument()->SetModifiedFlag(TRUE);

		GlobalUnlock(hMem);
		GlobalFree(hMem);

		GetEditCtrl().CancelRightButtonDrag();

		ms_bDroppedHere = FALSE;
	}
	else
	{
		int iRow = GetEditCtrl().GetCurRow();
		int iCol = GetEditCtrl().GetCurCol();

		if (GetEditCtrl().RowColInSel(iRow, iCol))
		{
			GetEditCtrl().Unselect();
			return FALSE;
		}

		ms_ptDropPos.x = iCol;
		ms_ptDropPos.y = iRow;

		ms_bDroppedHere = TRUE;
		ms_pTargetView = this;
	}

	m_bDraggingStartedHere = FALSE;

	return TRUE;
}

void CXTPSyntaxEditView::StartOleDrag()
{
	HGLOBAL hMemText = GetEditCtrl().GetSelectionBuffer(CF_TEXT);
	if (!hMemText)
		return;

	HGLOBAL hMemUnicodeText = GetEditCtrl().GetSelectionBuffer(CF_UNICODETEXT);
	if (!hMemUnicodeText)
		return;

	COleDataSource dataSource;

	dataSource.CacheGlobalData(CF_TEXT, hMemText);
	dataSource.CacheGlobalData(CF_UNICODETEXT, hMemUnicodeText);

	ms_bDroppedHere = FALSE;

	m_bDraggingStartedHere = TRUE;

	ms_dwSignature = (DWORD_PTR)GetEditCtrl().GetEditBuffer();
	ms_pTargetView = NULL;

	DROPEFFECT effect = dataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);
	TRACE(_T("\nStartOleDrag top row = %d"), GetEditCtrl().GetTopRow());

	if (effect != DROPEFFECT_NONE)
	{
		if (ms_bDroppedHere)
		{
			GetEditCtrl().SetDropPos(ms_ptDropPos.y, ms_ptDropPos.x);
			GetEditCtrl().HandleDrop(effect == DROPEFFECT_COPY);

			if (ms_pTargetView && ms_pTargetView != this)
				GetEditCtrl().Unselect();

			CXTPSyntaxEditCtrl& wndEditCtrl = ms_pTargetView->GetEditCtrl();
			wndEditCtrl.NotifyCurRowCol(wndEditCtrl.GetCurRow(), wndEditCtrl.GetCurCol());
		}
		else if (effect == DROPEFFECT_MOVE)
		{
			GetEditCtrl().DeleteSelection();
			GetEditCtrl().SetDirtyRow(-1);
			GetEditCtrl().Invalidate(FALSE);
			GetEditCtrl().CancelRightButtonDrag();
		}
		else
		{
			GetEditCtrl().CancelRightButtonDrag();
		}

		if (!ms_bDroppedHere)
		{
			CPoint ptStartSel, ptEndSel;

			if (GetEditCtrl().GetCurSel(ptStartSel, ptEndSel))
			{
				GetEditCtrl().SetCurPos(ptEndSel.y, ptEndSel.x, TRUE);
			}
		}
	}

	if (effect == DROPEFFECT_NONE)
	{
		if (!m_bDraggingOver)
		{
			if (!GetEditCtrl().IsRightButtonDrag())
				GetEditCtrl().Unselect();

			int nRow, nCol;

			CPoint point;
			GetCursorPos(&point);

			GetEditCtrl().ScreenToClient(&point);

			int nAbsCol;

			GetEditCtrl().RowColFromPoint(point, &nRow, &nCol);
			GetEditCtrl().ValidateCol(nRow, nCol, nAbsCol);

			if (!GetEditCtrl().IsRightButtonDrag())
			{
				GetEditCtrl().SetCurPos(nRow, nCol, TRUE, TRUE);
			}

			if (GetEditCtrl().IsRightButtonDrag())
			{
				GetEditCtrl().CancelRightButtonDrag();
				GetEditCtrl().ShowDefaultContextMenu();
			}
		}
		else
		{
			CPoint ptStartSel, ptEndSel;

			if (GetEditCtrl().GetCurSel(ptStartSel, ptEndSel))
			{
				GetEditCtrl().SetCurPos(ptEndSel.y, ptEndSel.x, TRUE);
			}
		}

		if (GetEditCtrl().IsRightButtonDrag())
			GetEditCtrl().CancelRightButtonDrag();
	}

	m_bDraggingStartedHere = FALSE;
	m_bDraggingOver = FALSE;
	ms_bDroppedHere = FALSE;
	ms_dwSignature = 0;
}

void CXTPSyntaxEditView::UpdateAllViews()
{
	GetEditCtrl().SetDirtyRow(-1);
	Invalidate(FALSE);
	UpdateWindow();
	UpdateScrollPos();
}

void CXTPSyntaxEditView::SetSyntaxColor(BOOL bEnable)
{
	CDocument* pDoc = GetDocument();
	if (pDoc != NULL)
	{
		pDoc->UpdateAllViews(NULL, xtpEditHintSetSyntaxColor, (CObject*)(BOOL*)&bEnable);
	}
}

void CXTPSyntaxEditView::SetAutoIndent(BOOL bEnable)
{
	CDocument* pDoc = GetDocument();
	if (pDoc != NULL)
	{
		pDoc->UpdateAllViews(NULL, xtpEditHintSetAutoIndent, (CObject*)(BOOL*)&bEnable);
	}
}

void CXTPSyntaxEditView::SetSelMargin(BOOL bEnable)
{
	CDocument* pDoc = GetDocument();
	if (pDoc != NULL)
	{
		pDoc->UpdateAllViews(NULL, xtpEditHintSetSelMargin, (CObject*)(BOOL*)&bEnable);
	}
}

BOOL CXTPSyntaxEditView::SetScrollBars(BOOL bHorz, BOOL bVert, BOOL bUpdateReg/*=FALSE*/, BOOL bRecalcLayout/*=TRUE*/)
{
	CSplitterWnd *pSplitterWnd = GetParentSplitter(this, TRUE);
	if (pSplitterWnd && GetEditCtrl().IsCreateScrollbarOnParent())
	{
		if (!GetEditCtrl().SetValueBool(XTP_EDIT_REG_HSCROLLBAR, bHorz, m_bHorzScrollBar, bUpdateReg))
			return FALSE;

		if (!GetEditCtrl().SetValueBool(XTP_EDIT_REG_VSCROLLBAR, bVert, m_bVertScrollBar, bUpdateReg))
			return FALSE;

		DWORD dwStyle = 0;

		if (bVert)
			dwStyle |= WS_VSCROLL;

		if (bHorz)
			dwStyle |= WS_HSCROLL;

		pSplitterWnd->SetScrollStyle(dwStyle);

		if (bRecalcLayout)
		{
			// Check have all splitter panes already created
			int nRowsCount = pSplitterWnd->GetRowCount();
			int nColsCount = pSplitterWnd->GetColumnCount();

			for (int nRow = 0; nRow < nRowsCount; nRow++)
			{
				for (int nCol = 0; nCol < nColsCount; nCol++)
				{
					if (!GetSplitterView(nRow, nCol))
					{
						return FALSE;
					}
				}
			}

			pSplitterWnd->RecalcLayout();
		}
		return TRUE;
	}

	if (m_bScrollBars)
	{
		return GetEditCtrl().SetScrollBars(bHorz, bVert, bUpdateReg);
	}

	return TRUE;
}

BOOL CXTPSyntaxEditView::GetVertScrollBar()
{
	return m_bVertScrollBar;
}

BOOL CXTPSyntaxEditView::GetHorzScrollBar()
{
	return m_bHorzScrollBar;
}

BOOL CXTPSyntaxEditView::CanChangeReadonlyFile()
{
	return TRUE;
}

void CXTPSyntaxEditView::SetLineNumbers(BOOL bEnable)
{
	CDocument* pDoc = GetDocument();
	if (pDoc != NULL)
	{
		pDoc->UpdateAllViews(NULL, xtpEditHintSetLineNum, (CObject*)(BOOL*)&bEnable);
	}
}

void CXTPSyntaxEditView::SetFontIndirect(LOGFONT *pLogFont, BOOL bUpdateReg/*=FALSE*/)
{
	GetEditCtrl().SetFontIndirect(pLogFont, bUpdateReg);

	CDocument* pDoc = GetDocument();
	if (pDoc != NULL)
	{
		pDoc->UpdateAllViews(this, xtpEditHintSetFont, (CObject*)pLogFont);
	}
}

void CXTPSyntaxEditView::OnDropFiles(HDROP hDropInfo)
{
	if (AfxGetApp() && AfxGetApp()->m_pMainWnd)
	{
		AfxGetApp()->m_pMainWnd->PostMessage(WM_DROPFILES,(WPARAM)hDropInfo);
	}
	else
	{
		::DragFinish(hDropInfo);
	}
}

int CXTPSyntaxEditView::ReplaceAll(LPCTSTR szFindText, LPCTSTR szReplaceText,
							  BOOL bMatchWholeWord, BOOL bMatchCase)
{
	int nMatchFound = 0;

	if (_tcsclen(szFindText) == 0)
	{
		return 0;
	}

	int iTopRow = GetEditCtrl().GetTopRow();
	int iCurRow = GetEditCtrl().GetCurRow();
	int iCurCol = GetEditCtrl().GetCurCol();

	GetEditCtrl().SetCurPos(1,1,FALSE,FALSE);

	CWaitCursor wait;

	BOOL bFirst = TRUE;

	while (GetEditCtrl().GetCurRow() <= GetEditCtrl().GetRowCount())
	{
		BOOL bFound = GetEditCtrl().Find(szFindText,
			bMatchWholeWord, bMatchCase,
			TRUE, FALSE);

		if (bFound)
		{
			GetEditCtrl().ReplaceSel(szReplaceText, FALSE);
			if (!bFirst)
				GetEditCtrl().GetEditBuffer()->GetUndoRedoManager()->ChainLastCommand();
		}
		else
			break;

		bFirst = FALSE;

		nMatchFound++;
	}

	if (!bFirst)
		GetEditCtrl().GetEditBuffer()->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_REPLACE_ALL);

	GetEditCtrl().SetTopRow(iTopRow);
	GetEditCtrl().SetCurPos(iCurRow, iCurCol);

	return nMatchFound;
}

void CXTPSyntaxEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (pSender == this)
	{
		return;
	}

	switch (lHint)
	{
	case xtpEditHintUpdateScrollPos:
		{
			XTP_EDIT_NMHDR_SETSCROLLPOS* pNMHDR_SSP = (XTP_EDIT_NMHDR_SETSCROLLPOS*)pHint;
			UpdateScrollPos(pNMHDR_SSP->dwUpdate);
		}
		break;

	case xtpEditHintSetSyntaxColor:
		{
			BOOL bEnable = *(BOOL*)pHint;
			GetEditCtrl().SetSyntaxColor(bEnable);
		}
		break;

	case xtpEditHintSetAutoIndent:
		{
			BOOL bEnable = *(BOOL*)pHint;
			GetEditCtrl().SetAutoIndent(bEnable);
		}
		break;

	case xtpEditHintSetSelMargin:
		{
			BOOL bEnable = *(BOOL*)pHint;
			GetEditCtrl().SetSelMargin(bEnable);
		}
		break;

	case xtpEditHintSetLineNum:
		{
			BOOL bEnable = *(BOOL*)pHint;
			GetEditCtrl().SetLineNumbers(bEnable);
		}
		break;

	case xtpEditHintSetFont:
		{
			LOGFONT* pLogFont = (LOGFONT*)pHint;
			GetEditCtrl().SetFontIndirect(pLogFont);
		}
		break;

	case xtpEditHintInitView:
		{
			GetEditCtrl().GetRegValues();

			// Update font.
			LOGFONT lf;
			XTPSyntaxEditPaintManager()->GetFont()->GetLogFont(&lf);
			GetEditCtrl().SetFontIndirect(&lf);
			GetEditCtrl().RestoreCursor();

			// Update scrollbars.
			GetRegValues();
			SetScrollBars(m_bHorzScrollBar, m_bVertScrollBar, FALSE, FALSE);
			Refresh();
		}
		break;

	case xtpEditHintRefreshView:
		{
			Refresh();
		}
		break;
	}
}

void CXTPSyntaxEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	int nPriority = bActivate ? m_nParserThreadPriority_WhenActive :
								m_nParserThreadPriority_WhenInactive;

	CXTPSyntaxEditLexParser* pParser = GetLexParser();
	if (pParser)
	{
		pParser->SetParseThreadPriority(nPriority);
	}

	GetEditCtrl().SetActive(bActivate);

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

BOOL CXTPSyntaxEditView::IsInitialUpdateWasCalled()
{
	return m_bInitialUpdateWasCalled;
}

CXTPSyntaxEditDoc* CXTPSyntaxEditView::GetDocument()
{
	return DYNAMIC_DOWNCAST(CXTPSyntaxEditDoc, m_pDocument);
}

CXTPSyntaxEditBufferManager* CXTPSyntaxEditView::GetDataManager()
{
	CXTPSyntaxEditDoc* pDoc = GetDocument();

	if (pDoc)
	{
		return pDoc->GetDataManager();
	}

	return NULL;
}

XTPSyntaxEditLexAnalyser::CXTPSyntaxEditLexParser* CXTPSyntaxEditView::GetLexParser()
{
	CXTPSyntaxEditBufferManager* pDataManager = GetDataManager();

	if (!pDataManager)
		pDataManager = GetEditCtrl().GetEditBuffer();

	if (pDataManager)
		return pDataManager->GetLexParser();

	return NULL;
}
