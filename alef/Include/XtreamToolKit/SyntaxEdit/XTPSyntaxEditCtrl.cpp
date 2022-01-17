// XTPSyntaxEditCtrl.cpp
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
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPVC50Helpers.h"
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

#include <math.h>

#define XTP_IDC_EDIT_DRAG_MOVE 0xE12D
#define XTP_IDC_EDIT_DRAG_COPY 0xE12E

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

namespace XTPSyntaxEditLexAnalyser
{
	CString DBG_TraceTB_StartEndCls(CXTPSyntaxEditLexTextBlock* pTB);
}

const int MARGIN_LENGTH             = 20;
const int NODES_WIDTH               = 10;
const int TEXT_LEFT_OFFSET          = 4;
const int TIMER_SELECTION_ID        = 100;
const int TIMER_REDRAW_WHEN_PARSE   = 200;

enum XTPSyntaxEditFlags
{
	xtpEditRedraw       = 0x0001,
	xtpEditForceRedraw  = 0x0002,
	xtpEditTextAsBlock  = 0x0004,
	xtpEditDispCol      = 0x0008,
};

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditCtrl

CXTPSyntaxEditCtrl::CXTPSyntaxEditCtrl()
: m_bVertScrollBar(TRUE)
, m_bHorzScrollBar(TRUE)
, m_bSyntaxColor(TRUE)
, m_bAutoIndent(TRUE)
, m_bSelMargin(TRUE)
, m_bLineNumbers(TRUE)
, m_bWideCaret(TRUE)
, m_bTabWithSpace(FALSE)
, m_bSelectionExist(FALSE)
, m_bSelecting(FALSE)
, m_bColumnSelect(FALSE)
, m_bSelectionStarted(FALSE)
, m_bIsOnSelection(FALSE)
, m_bDragging(FALSE)
, m_bDroppable(FALSE)
, m_bShiftPressed(FALSE)
, m_bTokensLoaded(FALSE)
, m_bIsScrollingEndRow(FALSE)
, m_bSelectingFromBookmark(FALSE)
, m_bRightButtonDrag(FALSE)
, m_bIsSmartIndent(TRUE)
, m_bIsSelectingWord(FALSE)
, m_bEnableOleDrag(FALSE)
, m_bEnableWhiteSpace(FALSE)
, m_bCaseSensitive(TRUE)
, m_bScrolling(FALSE)
, m_bDrawNodes(TRUE)
, m_bCaretCreated(FALSE)
, m_bFocused(FALSE)
, m_bIsActive(FALSE)
, m_nTopCalculatedRow(-1)
, m_nBottomCalculatedRow(-1)
, m_nTopRow(1)
, m_nCurrentCol(1)
, m_nMaxCol(1)
, m_nCurrentDocumentRow(1)
, m_nDispCol(1)
, m_nDirtyRow(-1)
, m_nCurrentPage(0)
, m_nPrvDirtyRow(-1)
, m_nAutoIndentCol(0)
, m_nBookmarkSelStartRow(0)
, m_nTopMargin(0)
, m_nBottomMargin(0)
, m_nRightMargin(0)
, m_nLeftMargin(0)
, m_nNumPages(0)
, m_nMarginLength(MARGIN_LENGTH)
, m_nLineNumLength(0)
, m_nNodesWidth(NODES_WIDTH)
, m_nEditbarLength(MARGIN_LENGTH + NODES_WIDTH)
, m_nCollapsedTextRowsCount(0)
, m_nWheelScroll(4)
, m_nAverageLineLen(XTP_EDIT_AVELINELEN)
, m_dwInsertPos(0)
, m_dwLastRedrawTime(0)
, m_szPage(CSize(850,1100))
, m_ptStartSel(CPoint(0,0))
, m_ptEndSel(CPoint(0,0))
, m_ptPrevMouse(CPoint(0,0))
, m_pBuffer(NULL)
, m_pParentWnd(NULL)
{
	m_bCreateScrollbarOnParent = TRUE;

	GetRegValues();
	RegisterWindowClass();

	::ZeroMemory(&m_tm, sizeof(TEXTMETRIC));
	::ZeroMemory(&m_siHorz, sizeof(SCROLLINFO));

	m_strDefaultCfgFilePath = GetModulePath() + XTP_EDIT_LEXPARSER_CFG_FILENAME;
}

CXTPSyntaxEditCtrl::~CXTPSyntaxEditCtrl()
{
	if (m_pBuffer && m_pBuffer->GetLexParser())
		m_pBuffer->GetLexParser()->CloseParseThread();

	m_Sink.UnadviseAll();

	m_arOnScreenSchCache.RemoveAll();
	CMDTARGET_RELEASE(m_pBuffer);

	DestroyWindow();
}

IMPLEMENT_DYNAMIC(CXTPSyntaxEditCtrl, CWnd)

BEGIN_MESSAGE_MAP(CXTPSyntaxEditCtrl, CWnd)
	//{{AFX_MSG_MAP(CXTPSyntaxEditCtrl)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_NCACTIVATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(XTP_IDC_EDIT_DRAG_COPY, OnDragCopy)
	ON_COMMAND(XTP_IDC_EDIT_DRAG_MOVE, OnDragMove)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditCtrl message handlers

BOOL CXTPSyntaxEditCtrl::GetRegValues()
{
	CWinApp* pWinApp = AfxGetApp();
	if (pWinApp != NULL)
	{
		m_bVertScrollBar = pWinApp->GetProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_VSCROLLBAR,   m_bVertScrollBar);
		m_bHorzScrollBar = pWinApp->GetProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_HSCROLLBAR,   m_bHorzScrollBar);
		m_bSyntaxColor   = pWinApp->GetProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_SYNTAXCOLOR,  m_bSyntaxColor);
		m_bAutoIndent    = pWinApp->GetProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_AUTOINDENT,   m_bAutoIndent);
		m_bSelMargin     = pWinApp->GetProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_SELMARGIN,    m_bSelMargin);
		m_bLineNumbers   = pWinApp->GetProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_LINENUMBERS,  m_bLineNumbers);
		m_bWideCaret     = pWinApp->GetProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_WIDECARET,    m_bWideCaret);
		m_bTabWithSpace  = pWinApp->GetProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_TABWITHSPACE, m_bTabWithSpace);
		return TRUE;
	}

	return FALSE;
}

BOOL CXTPSyntaxEditCtrl::SetValueInt(LPCTSTR lpszValue, int nNewValue, int& nRefValue, BOOL bUpdateReg)
{
	nRefValue = nNewValue;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, lpszValue, nNewValue))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::SetValueBool(LPCTSTR lpszValue, BOOL bNewValue, BOOL& bRefValue, BOOL bUpdateReg)
{
	bRefValue = bNewValue;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, lpszValue, (int)bNewValue))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::SetAutoIndent(BOOL bAutoIndent, BOOL bUpdateReg/*=FALSE*/)
{
	if (!SetValueBool(XTP_EDIT_REG_AUTOINDENT, bAutoIndent, m_bAutoIndent, bUpdateReg))
		return FALSE;

	return TRUE;
}

void CXTPSyntaxEditCtrl::OnPaint()
{
	DWORD dwStartTime = ::GetTickCount();

	CPaintDC dc(this); // device context for painting
	DrawEditControl(&dc);

	DWORD dwEndTime = ::GetTickCount();

	m_aveRedrawScreenTime.AddValue(::labs(dwEndTime - dwStartTime));
}

AFX_STATIC void AFX_CDECL XTPSECollapsedBlockDeleteFn(void* pPtr)
{
	XTP_EDIT_COLLAPSEDBLOCK* pBlock = (XTP_EDIT_COLLAPSEDBLOCK*)pPtr;
	SAFE_DELETE(pBlock);
}

void CXTPSyntaxEditCtrl::DrawEditControl(CDC *pDC)
{
	int iMaxRow = 0;
	CSize szRect;

	CXTPFontDC *pFontDC = NULL;
	TEXTMETRIC tm;
	BOOL bSelMargin = GetSelMargin();

	if (pDC->IsPrinting())
	{
		pFontDC = new CXTPFontDC(pDC, XTPSyntaxEditPaintManager()->GetFont());

		MEMCPY_S(&tm, &m_tm, sizeof(TEXTMETRIC));
		VERIFY(pDC->GetTextMetrics(&m_tm));
		m_tm.tmHeight++;

		SetSelMargin(FALSE);
	}

	CSize szPage = GetEffectivePageSize();

	if (pDC->IsPrinting())
	{
		if (m_tm.tmHeight)
		{
			iMaxRow = (szPage.cy / m_tm.tmHeight) - 1;
		}
		m_nTopRow = max(1, CalculateDocumentRow(1, ((m_nCurrentPage - 1) * iMaxRow) + 1));
	}

	CXTPClientRect rc(this);
	szRect = pDC->IsPrinting()? szPage: rc.Size();

	GetScrollInfo(SB_HORZ, &m_siHorz);

	m_fcCollapsable.Update(m_nTopRow);
	m_fcRowColors.Update(m_nTopRow);

	if (m_nDirtyRow <= 0)
	{
		m_nCollapsedTextRowsCount = 0;
	}

	//-------------------
	int cx = pDC->IsPrinting()? szPage.cx: rc.Width();
	int cy = pDC->IsPrinting()? (szPage.cy/m_tm.tmHeight)*m_tm.tmHeight: rc.Height();

	m_nDirtyRow = TRUE;

	// The on screen bitmap with drawn text
	CDC dcText;
	dcText.CreateCompatibleDC(pDC);
	CBitmap bmpCachedText;
	VERIFY(bmpCachedText.CreateCompatibleBitmap(pDC, cx, cy));
	CXTPBitmapDC textBitmapDC(&dcText, &bmpCachedText);

	// The on screen bitmap with drawn line marks and system info
	CDC dcMarks;
	dcMarks.CreateCompatibleDC(pDC);
	CBitmap bmpCachedMarks;
	VERIFY(bmpCachedMarks.CreateCompatibleBitmap(pDC, cx, cy));
	CXTPBitmapDC marksBitmapDC(&dcMarks, &bmpCachedMarks);

	CDC* pDrawTextDC  = pDC->IsPrinting()? pDC: &dcText;
	CDC* pDrawMarksDC = pDC->IsPrinting()? pDC: &dcMarks;

	int iRowStart = -1;
	int nRowDrawn = 0;
	int nFirstDirtyRow = min(m_nDirtyRow, m_nPrvDirtyRow);
	int nSecDirtyRow   = max(m_nDirtyRow, m_nPrvDirtyRow);

	if (!pDC->IsPrinting() && (m_tm.tmHeight > 0))
	{
		iMaxRow = (rc.Height() / m_tm.tmHeight) + 1;
	}

	if (pDC->IsPrinting())
	{
		CRect rcFill(CPoint(m_nLeftMargin, m_nTopMargin), szPage);
		pDC->FillSolidRect(&rcFill, XTPSyntaxEditPaintManager()->GetBackColor());
	}

	CXTPSyntaxEditLexTextSchema* ptrTxtSch = m_pBuffer->GetLexParser()->GetTextSchema();
	int nSkipLines = 0;
	m_nCollapsedTextRowsCount = 0;

	for (int iRow = 0; iRow < iMaxRow; iRow++)
	{
		int nRowActual = m_nTopRow + iRow + nSkipLines;
		bool bSkipRow = FALSE;

		XTP_EDIT_LMPARAM LMCoParam;
		BOOL bIsRowCollapsed = HasRowMark(
			nRowActual, xtpEditLMT_Collapsed, &LMCoParam);

		// retrieve row nodes
		CXTPSyntaxEditRowsBlockArray arCoBlocks;
		DWORD dwType = XTP_EDIT_ROWNODE_NOTHING;
		if (ptrTxtSch)
		{
			ptrTxtSch->GetCollapsableBlocksInfo(
				nRowActual, arCoBlocks, &m_fcCollapsable.ptrTBStart);

			BOOL bCollapsedProcessed = FALSE;
			int nCount = (int)arCoBlocks.GetSize();
			for (int i = 0; i < nCount; i++)
			{
				XTP_EDIT_ROWSBLOCK coBlk = arCoBlocks[i];
				if (coBlk.lcStart.nLine == nRowActual)
				{
					if (bIsRowCollapsed && !bCollapsedProcessed)
					{
						XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk = (XTP_EDIT_COLLAPSEDBLOCK*)LMCoParam.GetPtr();
						if (!pCoDrawBlk)
						{
							pCoDrawBlk = new XTP_EDIT_COLLAPSEDBLOCK;
							LMCoParam.SetPtr(pCoDrawBlk, XTPSECollapsedBlockDeleteFn);
						}
						pCoDrawBlk->collBlock = coBlk;
						GetLineMarksManager()->SetLineMark(
							nRowActual, xtpEditLMT_Collapsed, &LMCoParam);

						m_arCollapsedTextRows.SetAtGrow(m_nCollapsedTextRowsCount, nRowActual);
						m_nCollapsedTextRowsCount++;
						bCollapsedProcessed = TRUE;
						dwType |= XTP_EDIT_ROWNODE_COLLAPSED;
					}
					else
					{
						dwType |= XTP_EDIT_ROWNODE_EXPANDED;
					}
				}
				if (coBlk.lcEnd.nLine == nRowActual)
				{
					dwType |= XTP_EDIT_ROWNODE_ENDMARK;
				}
				if (coBlk.lcStart.nLine < nRowActual)
				{
					dwType |= XTP_EDIT_ROWNODE_NODEUP;
				}
				if (coBlk.lcEnd.nLine > nRowActual && !bCollapsedProcessed)
				{
					dwType |= XTP_EDIT_ROWNODE_NODEDOWN;
				}
				// check whether to skip the row
				if ((coBlk.lcStart.nLine < nRowActual) &&
					(coBlk.lcEnd.nLine >= nRowActual))
				{
					if (HasRowMark(coBlk.lcStart.nLine, xtpEditLMT_Collapsed))
					{
						bSkipRow = TRUE;
					}
				}
			}
		}

		if (bSkipRow)
		{
			nSkipLines++;
			iRow--;
			continue;
		}

		// start drawing
		if (m_nDirtyRow <= 0 ||
			(nRowActual >= m_nDirtyRow-1 || nRowActual <= m_nDirtyRow+1) ||
			(nFirstDirtyRow != -1 && (nRowActual >= nFirstDirtyRow && nRowActual <= nSecDirtyRow)) ||
			(m_bIsScrollingEndRow && nRowActual == (m_nTopRow + iMaxRow - 1)) )
		{
			COLORREF clrFont = 0, crBack = 0;

			bool bSwitchColor = !pDC->IsPrinting();// && GetSelMargin(); Commented out by Leva to show breakpoints even if selection margin unchecked
			if (bSwitchColor)
			{
				clrFont = GetRowColor(nRowActual);
				crBack = GetRowBkColor(nRowActual);

				XTPSyntaxEditPaintManager()->DrawLineMarks(this, pDrawMarksDC, iRow, nRowActual);
			}

			int iStartCol = XTPSyntaxEditPaintManager()->DrawLineNumber(this, pDrawMarksDC, iRow, nRowActual);

			// draw row node sign
			if (!pDC->IsPrinting())
			{
				XTPSyntaxEditPaintManager()->DrawLineNode(this, pDrawMarksDC, iRow, dwType);
			}

			// Print the row
			BOOL bPrintRes = TRUE;
			if (m_nDirtyRow != 0)
			{
				bPrintRes = PrintRow(m_nTopRow + nSkipLines, nRowActual, pDrawTextDC, iStartCol);
			}

			if (bSwitchColor)
			{
				SetRowColor(nRowActual, clrFont);
				SetRowBkColor(nRowActual, crBack);
			}

			if (!bPrintRes)
			{
				break;
			}

			nRowDrawn++;
			if (iRowStart < 0)
			{
				iRowStart = iRow;
			}
		}
	}

	m_nPrvDirtyRow = m_nDirtyRow;
	m_nDirtyRow = 0;

	//---------------------------------------------------------------------------
	XTPSyntaxEditPaintManager()->DrawCollapsedTextMarks(this, pDrawTextDC);
	//---------------------------------------------------------------------------

	if (!pDC->IsPrinting())
	{
		pDC->BitBlt(m_nEditbarLength, 0, rc.Width(), rc.Height(), &dcText, 0, 0, SRCCOPY);
		pDC->BitBlt(0, 0, m_nEditbarLength, rc.Height(), &dcMarks, 0, 0, SRCCOPY);
	}
	else
	{
		MEMCPY_S(&m_tm, &tm, sizeof(TEXTMETRIC));
		SetSelMargin(bSelMargin);
	}

	SAFE_DELETE(pFontDC);
}

BOOL CXTPSyntaxEditCtrl::RegisterWindowClass(HINSTANCE hInstance /*= NULL*/)
{
	return XTPDrawHelpers()->RegisterWndClass(hInstance,
			XTP_EDIT_CLASSNAME_EDITCTRL, CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS);
}

BOOL CXTPSyntaxEditCtrl::Create(CWnd* pParentWnd, BOOL bHorzScroll, BOOL bVertScroll, CXTPSyntaxEditBufferManager *pBuffer, CCreateContext *lpCS)
{
	if (!pBuffer)
	{
		m_pBuffer = new CXTPSyntaxEditBufferManager;
	}
	else
	{
		m_pBuffer = pBuffer;
		m_pBuffer->InternalAddRef();
	}

	if (!m_pBuffer)
		return FALSE;

	if (GetConfigFile().IsEmpty())
	{
		CString csCfgFilePath = GetDefaultCfgFilePath();
		if (!SetConfigFile(csCfgFilePath))
		{
			TRACE1("\n*** Could not locate config file '%s'.\n\n", (LPCTSTR)csCfgFilePath);
		}
	}

	DWORD dwStyle = (WS_CHILD | WS_VISIBLE);

	if (bVertScroll)
		dwStyle |= WS_VSCROLL;

	if (bHorzScroll)
		dwStyle |= WS_HSCROLL;

	m_bVertScrollBar = bVertScroll;
	m_bHorzScrollBar = bHorzScroll;

	ASSERT_VALID(pParentWnd); // must be valid.
	m_pParentWnd = pParentWnd;

	if (!CWnd::CreateEx(WS_EX_ACCEPTFILES, XTP_EDIT_CLASSNAME_EDITCTRL, NULL,
		dwStyle, 0,0,0,0, m_pParentWnd->GetSafeHwnd(), NULL, (LPVOID)lpCS))
	{
		TRACE0("Failed to create edit window.\n");
		return FALSE;
	}

	// Set editor fonts.
	SetFontIndirect(NULL);

	m_bCaretCreated = TRUE;

	// Create AutoComplete window.
	VERIFY(m_wndAutoComplete.Create(this));

	// Advise to events
	m_Sink.UnadviseAll();

	CXTPNotifyConnection* ptrConnParser = m_pBuffer->GetLexParser()->GetConnection();
	ASSERT(ptrConnParser);
	if (ptrConnParser)
	{
		m_Sink.Advise(ptrConnParser, xtpEditOnParserStarted, &CXTPSyntaxEditCtrl::OnParseEvent);
		m_Sink.Advise(ptrConnParser, xtpEditOnTextBlockParsed, &CXTPSyntaxEditCtrl::OnParseEvent);
		m_Sink.Advise(ptrConnParser, xtpEditOnParserEnded, &CXTPSyntaxEditCtrl::OnParseEvent);
	}

	CXTPNotifyConnection* ptrConnBufMan = m_pBuffer->GetConnection();
	ASSERT(ptrConnBufMan);
	if (ptrConnBufMan)
	{
		m_Sink.Advise(ptrConnBufMan, xtpEditClassSchWasChanged, &CXTPSyntaxEditCtrl::OnLexCfgWasChanged);
		m_Sink.Advise(ptrConnBufMan, xtpEditThemeWasChanged, &CXTPSyntaxEditCtrl::OnLexCfgWasChanged);
		m_Sink.Advise(ptrConnBufMan, xtpEditAllConfigWasChanged, &CXTPSyntaxEditCtrl::OnLexCfgWasChanged);
	}

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	UNREFERENCED_PARAMETER(pWnd);
	UNREFERENCED_PARAMETER(nHitTest);
	UNREFERENCED_PARAMETER(message);

	CPoint pt;
	::GetCursorPos(&pt);
	ScreenToClient(&pt);

	CXTPClientRect rcClient(this);
	CRect rcText(rcClient);
	rcText.left += m_nEditbarLength;

	CRect rcBookmark(0, 0, GetSelMargin() ? m_nMarginLength : 0, rcClient.Height());
	CRect rcLineNumAndNodes(rcBookmark.right, 0, m_nEditbarLength, rcClient.Height());
	CXTPEmptyRect rcNode;

	if (m_bDrawNodes)
	{
		// calculate node icon rect
		int nRow = 0, nDispRow = 0;
		RowColFromPoint(pt, &nRow, NULL, &nDispRow);

		DWORD dwType = XTP_EDIT_ROWNODE_NOTHING;
		if (GetRowNodes(nRow, dwType) && (dwType & (XTP_EDIT_ROWNODE_COLLAPSED | XTP_EDIT_ROWNODE_EXPANDED)) )
		{
			GetLineNodeRect(nDispRow-1, rcNode);
		}
	}

	if (m_bDragging)
	{
		if (rcText.PtInRect(pt))
		{
			if ((::GetKeyState(VK_CONTROL) & KF_UP) == 0)
			{
				SetCursor(XTPSyntaxEditPaintManager()->GetCurMove());
				return TRUE;
			}
			else
			{
				SetCursor(XTPSyntaxEditPaintManager()->GetCurCopy());
				return TRUE;
			}
		}
		else
		{
			SetCursor(XTPSyntaxEditPaintManager()->GetCurNO());
			return TRUE;
		}
	}

	else if (rcText.PtInRect(pt))
	{
		if (m_bIsOnSelection || m_bRightButtonDrag)
		{
			SetCursor(XTPSyntaxEditPaintManager()->GetCurArrow());
			return TRUE;
		}
		else
		{
			if (!GetBlockFromPt(pt))
			{
				SetCursor(XTPSyntaxEditPaintManager()->GetCurIBeam());
				return TRUE;
			}
		}
	}

	else if (rcBookmark.PtInRect(pt))
	{
		SetCursor(XTPSyntaxEditPaintManager()->GetCurArrow());
		return TRUE;
	}

	else if (rcNode.PtInRect(pt))
	{
		SetCursor(XTPSyntaxEditPaintManager()->GetCurArrow());
		return TRUE;
	}

	else if (rcLineNumAndNodes.PtInRect(pt))
	{
		SetCursor(XTPSyntaxEditPaintManager()->GetCurLine());
		return TRUE;
	}

	SetCursor(XTPSyntaxEditPaintManager()->GetCurArrow());
	return TRUE;
}

LRESULT CXTPSyntaxEditCtrl::OnSetText(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	LPCTSTR szText = (LPCTSTR) lParam;

	int nLastRow = m_pBuffer->GetRowCount();
	m_pBuffer->DeleteText(1, 1, nLastRow+1, 1);
	m_pBuffer->InsertText(szText, 1, 1);

	RecalcScrollBars();

	return TRUE;
}

void CXTPSyntaxEditCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UNREFERENCED_PARAMETER(pScrollBar); UNREFERENCED_PARAMETER(nPos);

	SCROLLINFO info;
	GetScrollInfo(SB_HORZ, &info);

	int iMin = 0, iMax = 0;
	GetScrollRange(SB_HORZ, &iMin, &iMax);

	int iPos = GetScrollPos(SB_HORZ);

	switch(nSBCode) {
	case SB_LINELEFT:
		SetScrollPos(SB_HORZ, iPos-1);
		break;
	case SB_LINERIGHT:
		SetScrollPos(SB_HORZ, iPos+1);
		break;
	case SB_PAGELEFT:
		SetScrollPos(SB_HORZ, (iPos - info.nPage));
		break;
	case SB_PAGERIGHT:
		SetScrollPos(SB_HORZ, (iPos + info.nPage));
		break;
	case SB_LEFT:
		SetScrollPos(SB_HORZ, iMin);
		break;
	case SB_RIGHT:
		SetScrollPos(SB_HORZ, iMax);
	case SB_ENDSCROLL:
		return;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			SCROLLINFO si;
			ZeroMemory(&si, sizeof(SCROLLINFO));
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_TRACKPOS;

			if (GetScrollInfo(SB_HORZ, &si))
			{
				SetScrollPos(SB_HORZ, si.nTrackPos);
			}
		}
		break;
	default:
		break;
	}

	if (m_bFocused)
	{
		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, FALSE);
	}

	m_nDirtyRow = -1;
	Invalidate(FALSE);
	UpdateWindow();
}

void CXTPSyntaxEditCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UNREFERENCED_PARAMETER(nPos);
	UNREFERENCED_PARAMETER(pScrollBar);

	SCROLLINFO info;
	GetScrollInfo(SB_VERT, &info);

	int iMin = 0, iMax = 0;
	GetScrollRange(SB_VERT, &iMin, &iMax);

	int iPos = GetScrollPos(SB_VERT);
	BOOL bChanged = FALSE;

	switch(nSBCode) {
	case SB_LINEDOWN:
		bChanged = ShiftCurrentVisibleRowDown(1);
		if (bChanged)
			SetScrollPos(SB_VERT, iPos + 1);
		break;
	case SB_LINEUP:
		bChanged = ShiftCurrentVisibleRowUp(1);
		if (bChanged)
		SetScrollPos(SB_VERT, iPos - 1);
		break;
	case SB_PAGEUP:
		bChanged = ShiftCurrentVisibleRowUp(info.nPage);
		if (bChanged)
			SetScrollPos(SB_VERT, max(1, iPos - info.nPage));
		break;
	case SB_PAGEDOWN:
		bChanged = ShiftCurrentVisibleRowDown(info.nPage);
		if (bChanged)
			SetScrollPos(SB_VERT, (iPos + info.nPage));
		break;
	case SB_TOP:
		SetScrollPos(SB_VERT, iMin);
		SetCurrentDocumentRow(iMin);
		m_nCurrentCol = m_nDispCol = m_nMaxCol = 1;
		bChanged = TRUE;
		break;
	case SB_BOTTOM:
		SetScrollPos(SB_VERT, iMax);
		SetCurrentDocumentRow(iMax);
		m_nCurrentCol = m_nDispCol = m_nMaxCol = 1;
		bChanged = TRUE;
		break;
	case SB_THUMBTRACK:
		{
			SCROLLINFO si;
			ZeroMemory(&si, sizeof(SCROLLINFO));
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_TRACKPOS;

			if (GetScrollInfo(SB_VERT, &si))
			{
				int nShift = si.nTrackPos - GetVisibleRowsCount(m_nTopRow);
				if (nShift > 0)
				{
					bChanged = ShiftCurrentVisibleRowDown(nShift);
				}
				else if (nShift < 0)
				{
					bChanged = ShiftCurrentVisibleRowUp(-nShift);
				}
				SetScrollPos(SB_VERT, si.nTrackPos);
		}
		}
		break;
	case SB_ENDSCROLL:
		return;
	default:
		break;
	}

	//EnsureVisible(iVisibleRow, 0, TRUE);
	int nTopDocRow = GetDocumentRow(1);
	int nBottomDocRow = GetDocumentRow(GetRowPerPage());
	int nCurDocRow = GetCurrentDocumentRow();
	if (nCurDocRow >= nTopDocRow && nCurDocRow <= nBottomDocRow)
	{
		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, FALSE);
	}

	if (bChanged)
	{
		Invalidate(FALSE);
		UpdateWindow();
	}
}


BOOL CXTPSyntaxEditCtrl::PrintRow(int iTopRow, int iRow, CDC *pDC, int iStartCol)
{
	if (iRow < 1)
		return FALSE;

	// Draw the line
	int iAddTop = 0;
	if (pDC->IsPrinting())
		iAddTop = m_nTopMargin;

	if (!pDC->IsPrinting())
		iStartCol = 0;

	XTPSyntaxEditPaintManager()->DrawLine(this, pDC, GetLineText(iRow),
		(iAddTop + ((iRow - iTopRow) * m_tm.tmHeight)), iRow, iStartCol);

	return TRUE;
}

void CXTPSyntaxEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_wndToolTip.Hide();

	if (m_bDragging || m_bRightButtonDrag || m_bSelecting || m_bSelectionStarted)
	{
		OnSetCursor(0,0,0);
		return;
	}

	BOOL bChanged = FALSE;
	BOOL bRedraw = FALSE;

	BOOL bCtrlKey  = (::GetKeyState(VK_CONTROL) & KF_UP) != 0;
	BOOL bShiftKey  = (::GetKeyState(VK_SHIFT) & KF_UP) != 0;
	BOOL bAltKey  = (::GetKeyState(VK_MENU) & KF_UP) != 0;

	m_bShiftPressed = bShiftKey;

	if (bShiftKey)
	{
		if (!m_bSelectionExist)
		{
			m_bColumnSelect = bAltKey;
			//m_bSelectionExist = TRUE;
			m_ptStartSel = CPoint(m_nDispCol, GetCurrentDocumentRow());
		}

		bRedraw = TRUE;
	}

	m_nPrvDirtyRow = GetCurrentDocumentRow();

	BOOL bSelectionKey = FALSE;
	BOOL bUpdateAll = FALSE;

	switch(nChar) {

	case VK_UP:
		m_nAutoIndentCol = 0;
		bSelectionKey = TRUE;
		{
			if (GetCurrentDocumentRow() <= 1)
			{
				// Do nothing
			}
			else if (bCtrlKey)
			{
				bChanged = bRedraw = ShiftCurrentVisibleRowUp(1, TRUE);
				m_nDispCol = m_nMaxCol;
			}
			else if (GetCurrentDocumentRow() > 1)
			{
				int iPrvRow = GetCurrentVisibleRow();
				m_nPrvDirtyRow = GetDocumentRow(iPrvRow);
				MoveCurrentVisibleRowUp(1);

				LPCTSTR szText = GetLineText(m_nDirtyRow);
				m_nCurrentCol = CalcAbsCol(szText, m_nMaxCol);
				const int nColLength = (int)_tcsclen(szText);

				if (m_nCurrentCol > (nColLength+1))
					m_nCurrentCol = nColLength + 1;

				m_nDispCol = CalcDispCol(szText, m_nCurrentCol);

				bChanged = TRUE;
				bRedraw = TRUE;
			}
		}

		if (!bShiftKey && m_bSelectionExist)
		{
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			bChanged = TRUE;
			m_nDirtyRow = -1;
		}
		break;

	case VK_DOWN:
		m_nAutoIndentCol = 0;
		bSelectionKey = TRUE;
		if (bCtrlKey)
		{
			bChanged = bRedraw = ShiftCurrentVisibleRowDown(1, TRUE);
			m_nDispCol = m_nMaxCol;
		}
		else if (GetCurrentDocumentRow() < GetRowCount())
		{
			int iPrvRow = GetCurrentDocumentRow();
			MoveCurrentVisibleRowDown(1);

			if (bShiftKey)
			{
				m_nPrvDirtyRow = iPrvRow;
				bRedraw = TRUE;
				bChanged = TRUE;
			}

			LPCTSTR szText = GetLineText(m_nDirtyRow);
			m_nCurrentCol = CalcAbsCol(szText, m_nMaxCol);
			const int nColLength = (int)_tcsclen(szText);

			if (m_nCurrentCol > (nColLength + 1))
				m_nCurrentCol = nColLength + 1;

			m_nDispCol = CalcDispCol(szText, m_nCurrentCol);

			bChanged = TRUE;
			bRedraw = TRUE;
		}
		else
		{
			LPCTSTR szText = GetLineText(GetCurrentDocumentRow());
			m_nDispCol = CalcMaximumWidth(szText);
			m_nMaxCol = m_nDispCol;
			m_nCurrentCol = CalcAbsCol(szText, m_nDispCol);
			SetCurrentDocumentRow(GetRowCount());

			m_nDirtyRow = GetCurrentDocumentRow();
			m_nPrvDirtyRow = -1;

			bRedraw = TRUE;
			bChanged = TRUE;
		}

		if (!bShiftKey && m_bSelectionExist)
		{
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			bChanged = TRUE;
			m_nDirtyRow = -1;
		}
		break;

	case VK_LEFT:
		if (m_nAutoIndentCol > 0)
		{
			m_nAutoIndentCol = 0;
			m_nCurrentCol = m_nDispCol = m_nMaxCol = 1;
			m_nDirtyRow = GetCurrentDocumentRow();
			bChanged = TRUE;
			break;
		}

		bSelectionKey = TRUE;

		if (!bShiftKey && m_bSelectionExist)
		{
			// move cursor to the end of the selection
			XTP_EDIT_LINECOL lcRight = min(XTP_EDIT_LINECOL::MakeLineCol(m_ptStartSel.y, m_ptStartSel.x),
				XTP_EDIT_LINECOL::MakeLineCol(m_ptEndSel.y, m_ptEndSel.x));
			SetCurrentDocumentRow(lcRight.nLine);
			m_nDispCol = m_nCurrentCol = lcRight.nCol;
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			bChanged = TRUE;
			m_nDirtyRow = -1;
			break;
		}

		if (!bCtrlKey)
		{
			int nCurDocRow = GetCurrentDocumentRow();
			if ((nCurDocRow > 1 && m_nCurrentCol >= 1) ||
				(nCurDocRow == 1 && m_nCurrentCol > 1))
			{
				m_nCurrentCol--;
			}

			if (m_nCurrentCol < 1 && nCurDocRow > 1)
			{
				MoveCurrentVisibleRowUp(1);
				m_nDirtyRow = -1;
			}
			else
				m_nDirtyRow = nCurDocRow;

			LPCTSTR szText = GetLineText(GetCurrentDocumentRow());
			if (m_nCurrentCol < 1)
				m_nCurrentCol = (int)_tcsclen(szText) + 1;

			m_nDispCol = CalcDispCol(szText, m_nCurrentCol);
			m_nMaxCol = m_nDispCol;
		}
		else
		{
			// This will modify Current Row, m_nCurrentCol, m_nDispCol and m_nMaxCol
			FindWord(XTP_EDIT_FINDWORD_PREV);
			m_nDirtyRow = GetCurrentDocumentRow();
		}

		bChanged = TRUE;

		if (!bShiftKey && m_bSelectionExist)
		{
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			bChanged = TRUE;
			m_nDirtyRow = -1;
		}

		break;
	case VK_RIGHT:
		m_nAutoIndentCol = 0;
		bSelectionKey = TRUE;

		if (!bShiftKey && m_bSelectionExist)
		{
			// move cursor to the start of the selection
			XTP_EDIT_LINECOL lcLeft = max(XTP_EDIT_LINECOL::MakeLineCol(m_ptStartSel.y, m_ptStartSel.x),
									XTP_EDIT_LINECOL::MakeLineCol(m_ptEndSel.y, m_ptEndSel.x));
			SetCurrentDocumentRow(lcLeft.nLine);
			m_nDispCol = m_nCurrentCol = lcLeft.nCol;
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			bChanged = TRUE;
			m_nDirtyRow = -1;
			break;
		}

		if (!bCtrlKey)
		{
			int nCurDocRow = GetCurrentDocumentRow();
			LPCTSTR szText = GetLineText(nCurDocRow);
			const int nTextLen = (int)_tcsclen(szText);
			if (m_nCurrentCol > nTextLen && (nCurDocRow < GetRowCount()))
			{
				int nOldTopRow = m_nTopRow;
				MoveCurrentVisibleRowDown(1);
				m_nCurrentCol = 1;
				m_nDirtyRow = -1;
				if (m_nTopRow != nOldTopRow)
					bRedraw = TRUE;
			}
			else if (m_nCurrentCol <= nTextLen)
			{
				m_nCurrentCol++;
				m_nDirtyRow = nCurDocRow;
			}

			m_nDispCol = CalcDispCol(szText, m_nCurrentCol);
			m_nMaxCol = m_nDispCol;

			bChanged = TRUE;
		}
		else
		{
			FindWord(XTP_EDIT_FINDWORD_NEXT);
			bChanged = TRUE;
			m_nDirtyRow = GetCurrentDocumentRow();
		}

		if (!bShiftKey && m_bSelectionExist)
		{
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			m_nDirtyRow = -1;
		}

		break;
	case VK_HOME:
		bSelectionKey = TRUE;

		if (bCtrlKey)
		{
			SetCurrentDocumentRow(1);
			m_nCurrentCol = 1;
			m_nDispCol = m_nMaxCol = 1;

			bChanged = TRUE;
			bRedraw = TRUE;
			m_nDirtyRow = -1;
		}
		else
		{
			LPCTSTR szText = GetLineText(GetCurrentDocumentRow());

			// find number of tabs and spaces from the left
			int iCol = 1;
			LPCTSTR szPtr = szText;

			while (szPtr)
			{
				if (*szPtr == _T('\t'))
				{
					iCol++;
				}
				else if (*szPtr == _T(' '))
				{
					iCol++;
				}
				else
				{
					break;
				}

				szPtr++;
			}

			int iPrevCol = m_nCurrentCol;

			if (m_nCurrentCol == iCol)
			{
				m_nCurrentCol = 1;
				m_nDispCol = 1;
				m_nMaxCol = 1;
			}
			else
			{
				m_nCurrentCol = iCol;
				m_nDispCol = CalcDispCol(szText, iCol);
				m_nMaxCol = m_nDispCol;
			}

			if (iPrevCol != m_nCurrentCol)
			{
				bChanged = TRUE;
				bRedraw = TRUE;
				int nCurDocRow = GetCurrentDocumentRow();
				m_nDirtyRow = IsRowVisible(nCurDocRow) ? nCurDocRow : -1;
			}
		}

		if ((!bShiftKey && m_bSelectionExist) || m_nAutoIndentCol)
		{
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			bChanged = TRUE;
			m_nDirtyRow = -1;
			m_nAutoIndentCol = 0;
		}

		break;
	case VK_END:
		bSelectionKey = TRUE;
		{
			int nPrevRow = GetCurrentDocumentRow();
			if (bCtrlKey)
			{
				SetCurrentDocumentRow(GetRowCount());
				m_nDirtyRow = -1;
			}
			else
				m_nDirtyRow = nPrevRow;

			LPCTSTR szText = GetLineText(GetCurrentDocumentRow());
			const int iLen = (int)_tcsclen(szText);

			if (m_nCurrentCol < (iLen + 1) || nPrevRow != GetCurrentDocumentRow())
			{
				m_nCurrentCol = iLen + 1;
				m_nDispCol = CalcDispCol(szText, m_nCurrentCol);
				m_nMaxCol = m_nDispCol;

				bChanged = TRUE;
				bRedraw = TRUE;
			}
		}

		if ((!bShiftKey && m_bSelectionExist) || m_nAutoIndentCol)
		{
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			bChanged = TRUE;
			m_nDirtyRow = -1;
			m_nAutoIndentCol = 0;
		}

		break;
	case VK_PRIOR:
		m_nAutoIndentCol = 0;
		bSelectionKey = TRUE;
		{
			int iRowPerPage = GetRowPerPage();
			int nPrevRow = GetCurrentDocumentRow();

			if ((m_nTopRow - iRowPerPage) < 1)
			{
				SetCurrentDocumentRow(1);
			}
			else
			{
				MoveCurrentVisibleRowUp(iRowPerPage);
			}

			if (nPrevRow != GetCurrentDocumentRow())
			{
				LPCTSTR szText = GetLineText(GetCurrentDocumentRow());

				m_nCurrentCol = CalcAbsCol(szText, m_nMaxCol);;
				m_nDispCol = CalcDispCol(szText, m_nCurrentCol);

				m_nDirtyRow = -1;
				bChanged = TRUE;
				bRedraw = TRUE;
			}
		}

		if (!bShiftKey && m_bSelectionExist)
		{
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			m_nDirtyRow = -1;
		}

		break;
	case VK_NEXT:
		m_nAutoIndentCol = 0;
		bSelectionKey = TRUE;
		{
			int iRowPerPage = GetRowPerPage();
			int nPrevRow = GetCurrentDocumentRow();

			if ((m_nTopRow + iRowPerPage) > GetRowCount())
			{
				SetCurrentDocumentRow(GetRowCount());
			}
			else
			{
				MoveCurrentVisibleRowDown(iRowPerPage);
			}

			if (nPrevRow != GetCurrentDocumentRow())
			{
				LPCTSTR szText = GetLineText(GetCurrentDocumentRow());

				m_nCurrentCol = CalcAbsCol(szText, m_nMaxCol);;
				m_nDispCol = CalcDispCol(szText, m_nCurrentCol);

				m_nDirtyRow = -1;
				bChanged = TRUE;
				bRedraw = TRUE;
			}
		}
		if (!bShiftKey && m_bSelectionExist)
		{
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			m_nDirtyRow = -1;
		}

		break;
	case VK_DELETE:

		{
			BOOL bDeleted = TRUE;
			if (m_bSelectionExist && !bCtrlKey)
			{
				bDeleted = DeleteSelection();
			}
			else if (!bCtrlKey)
			{
				if (GetAutoIndent() && m_nAutoIndentCol > 0)
				{
					CString strInsertText(
						CString(_T('\t'), m_nInsertTabCount) +
						CString(_T(' '), m_nInsertSpaceCount));

					int iNewDispCol = (m_nInsertTabCount * GetTabSize()) + m_nInsertSpaceCount + 1;
					int iNewCol = m_nInsertTabCount + m_nInsertSpaceCount + 1;

					m_pBuffer->InsertText(strInsertText, GetCurrentDocumentRow(), m_nCurrentCol);

					m_nCurrentCol = iNewCol;
					m_nDispCol = iNewDispCol;

					m_nAutoIndentCol = 0;
				}

				bDeleted = DeleteChar(GetCurrentDocumentRow(), m_nCurrentCol, xtpEditDelPosAfter);
			}
			else
			{
				BOOL bSelectionExist = m_bSelectionExist;
				int nDelFlags = 0;

				if (m_bSelectionExist)
				{
					nDelFlags |= (m_ptStartSel.y != m_ptEndSel.y) ? xtpEditForceRedraw : 0;
					DeleteSelection();
				}

				int iStartRow = GetCurrentDocumentRow();
				int iStartCol = m_nCurrentCol;

				FindWord(XTP_EDIT_FINDWORD_NEXT);

				int iEndRow = GetCurrentDocumentRow();
				int iEndCol = m_nCurrentCol;

				DeleteBuffer(iStartRow, iStartCol, iEndRow, iEndCol, nDelFlags);

				if (bSelectionExist)
					m_pBuffer->GetUndoRedoManager()->ChainLastCommand();
			}

			bUpdateAll = TRUE;
		}

		break;
	case VK_BACK:
		if (m_nAutoIndentCol > 0)
		{
			if (m_nInsertSpaceCount == 0)
				m_nInsertTabCount--;
			else
				m_nInsertSpaceCount--;

			m_nAutoIndentCol = (m_nInsertTabCount * GetTabSize()) + m_nInsertSpaceCount;

			LPCTSTR szText = GetLineText(GetCurrentDocumentRow());

			int iDispCol = CalcValidDispCol(szText, m_nAutoIndentCol);

			if (m_nAutoIndentCol <= iDispCol)
			{
				const int iLen = (int)_tcsclen(szText);
				m_nAutoIndentCol = 0;
				m_nCurrentCol = iLen + 1;
				m_nDispCol = CalcDispCol(szText, m_nCurrentCol);
				m_nMaxCol = m_nDispCol;
			}

			if (m_nAutoIndentCol)
				m_nAutoIndentCol++;

			bChanged = TRUE;
		}
		else if (!bCtrlKey)
		{
			if (m_bSelectionExist)
			{
				DeleteSelection();
			}
			else
			{
				DeleteChar(GetCurrentDocumentRow(), m_nCurrentCol, xtpEditDelPosBefore);
			}

			m_bSelectionExist = FALSE;
			bRedraw = FALSE;
		}
		else
		{
			BOOL bSelectionExist = m_bSelectionExist;

			if (m_bSelectionExist)
			{
				DeleteSelection();
			}

			int iStartRow = GetCurrentDocumentRow();
			int iStartCol = m_nCurrentCol;

			FindWord(XTP_EDIT_FINDWORD_PREV);

			int iEndRow = GetCurrentDocumentRow();
			int iEndCol = m_nCurrentCol;

			DeleteBuffer(iStartRow, iStartCol, iEndRow, iEndCol);

			if (bSelectionExist)
				m_pBuffer->GetUndoRedoManager()->ChainLastCommand();

		}

		bUpdateAll = TRUE;

		break;
	case VK_INSERT:
		{
			NotifyParent(XTP_EDIT_NM_INSERTKEY);
			SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol);
			SetOverwriteMode(!m_pBuffer->GetOverwriteFlag());
		}
		break;
	case VK_SPACE:
		if (bCtrlKey)
		{
			CPoint pt(CWnd::GetCaretPos());
			pt.y += m_tm.tmHeight;
			ClientToScreen(&pt);

			CString strText(GetLineText(GetCurrentDocumentRow()));
			int nTextPos = m_nCurrentCol - 2;

			CString strSearch;
			while (strText.GetLength() > 0 && nTextPos >= 0 && m_wndAutoComplete.m_strDelims.Find(strText.GetAt(nTextPos)) < 0)
			{
				strSearch = strText.GetAt(nTextPos--) + strSearch;
			}
			SetAutoCompleteList();
			m_wndAutoComplete.Show(pt, strSearch);
		}
		break;

	}

	if (bSelectionKey && bShiftKey && (GetCurrentDocumentRow() != m_ptStartSel.y || m_ptStartSel.x != m_nDispCol))
		m_bSelectionExist = TRUE;

	if (bChanged)
	{
		int nCurDocRow = GetCurrentDocumentRow();
		ValidateCol(nCurDocRow, m_nDispCol, m_nCurrentCol);

		m_ptEndSel = CPoint(m_nDispCol, nCurDocRow);
		EnsureVisible(nCurDocRow, m_nDispCol, bRedraw);
		m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);
		UpdateWindow();

		DWORD dwUpdate = (XTP_EDIT_UPDATE_HORZ|XTP_EDIT_UPDATE_VERT);

		if (bUpdateAll)
			dwUpdate |= XTP_EDIT_UPDATE_DIAG;

		UpdateScrollPos(dwUpdate);
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CXTPSyntaxEditCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_bDragging || m_bRightButtonDrag || m_bSelecting || m_bSelectionStarted)
	{
		OnSetCursor(0,0,0);
		return;
	}

	NotifySelInit();

	BOOL bCtrlKey  = (::GetKeyState(VK_CONTROL) & KF_UP) != 0;
	BOOL bShiftKey  = (::GetKeyState(VK_SHIFT) & KF_UP) != 0;
	CString strCurCRLF = m_pBuffer->GetCurCRLF();
	LPCTSTR szCurCRLF = strCurCRLF;

	BOOL bProcessed = FALSE;

	if (nChar == VK_TAB)
	{
		if (bShiftKey)
			bProcessed = DecreaseIndent();
		else
			bProcessed = IncreaseIndent();
	}

	if (nChar != 0x08 && nChar != VK_ESCAPE && !bCtrlKey && !bProcessed)
	{
		if (!CanEditDoc())
		{
			return;
		}
		BOOL bModified = FALSE;
		//**----------------------
		OnBeforeEditChanged(GetCurrentDocumentRow(), m_nCurrentCol);
		//**----------------------

		int nChainActionCount = 1;
		BOOL bRedraw = FALSE;
		BOOL bPrevOverwriteMode = m_pBuffer->GetOverwriteFlag();

		if (m_bSelectionExist)
		{
			if (!m_pBuffer->GetOverwriteFlag() || (m_pBuffer->GetOverwriteFlag() && nChar != VK_RETURN))
			{
				BOOL bRes = DeleteSelection();
				bModified |= bRes;

				m_nDirtyRow = -1;
				nChainActionCount = 2;
				bRedraw = TRUE;
				m_pBuffer->SetOverwriteFlag(FALSE);
			}
		}

		m_bSelectionExist = FALSE;

		TCHAR szText[3];
		szText[0] = (TCHAR)nChar;
		szText[1] = NULL;

		if (nChar == '\r')
		{
			lstrcpy(szText, szCurCRLF);
			szText[2] = NULL;
			m_nDirtyRow = -1;
			// By Leva: here we sould expand collapsed block if we are on it
			if (GetLineMarksManager()->HasRowMark(GetCurrentDocumentRow(), xtpEditLMT_Collapsed))
			{
				GetLineMarksManager()->DeleteLineMark(GetCurrentDocumentRow(), xtpEditLMT_Collapsed);
			}
		}

		int iNewDispCol  = m_nDispCol;
		int iNewCol      = m_nCurrentCol;
		int iNewMaxCol   = m_nMaxCol;
		int iNewRow      = GetCurrentDocumentRow();
		int iEditRowFrom = iNewRow;
		int iEditRowTo   = iNewRow;

		CString strTextToIns;

		// Create text to insert
		BOOL bCanProcess =
			CreateInsertText(szText,
							strTextToIns,
							iNewRow,
							iNewCol,
							iNewDispCol,
							iNewMaxCol,
							iEditRowFrom,
							iEditRowTo,
							nChainActionCount);

		if (nChar != VK_RETURN && _tcsclen(szText) == 1 && !bRedraw)
			m_nDirtyRow = GetCurrentDocumentRow();

		BOOL bGroupInsMode = FALSE;

		if (!bPrevOverwriteMode)
		{
			static LPCTSTR szSeps = _T(" [{()}];.,\t\r\n\"");

			if (_tcschr(szSeps, (TCHAR)nChar))
				bGroupInsMode = FALSE;
			else
				bGroupInsMode = TRUE;

			m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(bGroupInsMode);
		}
		else
			m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);

		if (bCanProcess)
		{
			BOOL bInsRes = m_pBuffer->InsertText(strTextToIns, GetCurrentDocumentRow(), m_nCurrentCol, TRUE);
			bModified |= bInsRes;

			if (nChainActionCount > 1)
			{
				m_pBuffer->GetUndoRedoManager()->ChainLastCommand();
				m_pBuffer->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_TYPING);
			}
		}

		//m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);
		m_pBuffer->SetOverwriteFlag(bPrevOverwriteMode);

		//**----------------------
		OnEditChanged(GetCurrentDocumentRow(), m_nCurrentCol, iNewRow, iNewCol, xtpEditActInsert);
		//**----------------------

		m_nCurrentCol = iNewCol;
		m_nDispCol    = iNewDispCol;
		m_nMaxCol     = iNewMaxCol;

		SetCurrentDocumentRow(iNewRow);

		BOOL bNewRow = FALSE;

		if (nChar == VK_RETURN)
			bNewRow = TRUE;

		UINT nAction = XTP_EDIT_EDITACTION_MODIFYROW;

		if (bNewRow)
			nAction |= XTP_EDIT_EDITACTION_INSERTROW;

		NotifyEditChanged(iEditRowFrom, iEditRowTo, nAction);

		EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE);
		RecalcScrollBars();
		UpdateWindow();

		if (bModified)
			SetDocModified();

	}
	else if (!bProcessed && bCtrlKey && nChar == 0x0C /* 0x0C is 'l' or 'L' */)
	{
		// Delete the entire line or the selection
		DeleteSelectedLines(GetCurrentDocumentRow());
	}
	else if (!bProcessed && nChar == VK_ESCAPE)
	{
		Unselect();
		UpdateScrollPos();
	}

	// AutoComplete Processing
	if (m_wndAutoComplete.IsOpenTag((TCHAR)nChar))
	{
		CPoint pt(CWnd::GetCaretPos());
		pt.y += m_tm.tmHeight;
		ClientToScreen(&pt);
		SetAutoCompleteList();
		m_wndAutoComplete.Show(pt);
	}

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CXTPSyntaxEditCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_bDragging)
	{
		OnSetCursor(0,0,0);
		return;
	}

	BOOL bShiftKey  = (::GetKeyState(VK_SHIFT) & KF_UP) != 0;

	if (!bShiftKey && m_bShiftPressed)
		m_bShiftPressed = FALSE;

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CXTPSyntaxEditCtrl::EnsureVisible(int iRow, int iCol, BOOL bRedraw, BOOL bRowColNotify)
{
	int iPrevTopRow = m_nTopRow;

	if (m_nAutoIndentCol > 0)
	{
		iCol = m_nAutoIndentCol;
	}

	if (m_ptStartSel.x == m_ptEndSel.x &&
		m_ptStartSel.y == m_ptEndSel.y)
	{
		m_bSelectionExist = FALSE;
	}

	CXTPClientRect rc(this);
	if (rc.IsRectEmpty())
		return;

	if (m_tm.tmHeight == 0 || m_tm.tmAveCharWidth == 0)
		return;

	const int nMaxRowsPerPage = (rc.Height() / m_tm.tmHeight);
	int nCurPageMaxDocRow = GetDocumentRow(nMaxRowsPerPage);

	if (iRow > nCurPageMaxDocRow)
	{
		m_nTopRow += iRow - nCurPageMaxDocRow;
	}

	BOOL bScrollingToTop = FALSE;
	if (iRow < m_nTopRow)
	{
		bScrollingToTop = TRUE;
		m_nPrvDirtyRow = m_nTopRow;
		m_nTopRow = iRow;
	}

	int iXPos = 0, iYPos = 0;

	int iHorzScrollPos = GetScrollPos(SB_HORZ);

	int iMaxHorzPos = iHorzScrollPos + (rc.Width() - m_nEditbarLength) / m_tm.tmAveCharWidth;

	int iHorzStartPos = iHorzScrollPos ;
	int iPrevHorzScrollPos = iHorzScrollPos;

	iXPos = (iCol - iHorzStartPos - 1) * m_tm.tmAveCharWidth;
	iYPos = (GetVisibleRow(iRow) - 1) * m_tm.tmHeight;

	int iXShift = 0;
	if (iCol > iMaxHorzPos)
	{
		iXShift = ((iCol + 20) - iMaxHorzPos);
		iXPos -= (iXShift * m_tm.tmAveCharWidth);
	}
	else if (iCol < iHorzStartPos)
	{
		iXShift = -(iHorzStartPos - (iCol - 10));
		if ((iHorzScrollPos + iXShift) < 0)
			iXShift = -iHorzScrollPos;

		if (iCol >= 10)
			iXPos = ((10 - 1) * m_tm.tmAveCharWidth);
		else
			iXPos = ((iCol - 1) * m_tm.tmAveCharWidth);
	}

	if (iCol > 0)
	{
		SetScrollPos(SB_HORZ, iHorzScrollPos + iXShift);
	}

	BOOL bEnableVertScrl = (GetRowCount() >= nMaxRowsPerPage);
	EnableScrollBarCtrl(SB_VERT, bEnableVertScrl);
	if (bEnableVertScrl)
	{
		SetScrollPos(SB_VERT,  CalculateVisibleRow(1, m_nTopRow));
	}

	if (m_nDirtyRow >= 1 && (iPrevTopRow - m_nTopRow) != 0)
	{
		m_bIsScrollingEndRow = !bScrollingToTop;

		//CWnd::ScrollWindow(0, ((iPrevTopRow - m_nTopRow) * m_tm.tmHeight));
	}
	else
		m_bIsScrollingEndRow = FALSE;

	if (iXPos < 0)
		iXPos = 0;

	if (iYPos < 0)
		iYPos = 0;

	iXPos += m_nEditbarLength;

	if (bRowColNotify)
	{
		if (m_nAutoIndentCol > 0)
			NotifyCurRowCol(GetCurrentDocumentRow(), iCol);
		else
			NotifyCurRowCol(GetCurrentDocumentRow(), m_nDispCol);
	}

	if (iXShift != 0)
		m_nDirtyRow = -1;

	if ((m_nTopRow != iPrevTopRow) ||
		((iHorzScrollPos + iXShift) != iPrevHorzScrollPos) ||
		bRedraw)
	{
		Invalidate(FALSE);
	}

	if (GetFocus() == this)
	{
		CWnd::CreateSolidCaret(GetWideCaret()? m_tm.tmAveCharWidth: 2, m_tm.tmHeight);
		CWnd::SetCaretPos(CPoint(iXPos, iYPos));
		CWnd::ShowCaret();
	}
}

int CXTPSyntaxEditCtrl::CalcDispCol(LPCTSTR szText, int nActualCol)
{
	int iDispCol = 0;
	const int nLength = (int)_tcsclen(szText);
	for (int i = 0; i < nLength; i++)
	{
		if ((i + 1) >= nActualCol)
			break;

		if (szText[i] == _T('\t'))
		{
			// Now calculate tab size
			iDispCol += (GetTabSize() - (iDispCol % GetTabSize()));
		}
		else
			iDispCol++;
	}

	iDispCol++;

	return iDispCol;
}

int CXTPSyntaxEditCtrl::CalcAbsCol(LPCTSTR szText, int iDispCol)
{
	int nAbsCol = 0;
	int nDispColCalc = 0;

	const int nLength = (int)_tcsclen(szText);

	for (int i = 0; i <= nLength; i++,nAbsCol++)
	{
		if (nDispColCalc >= iDispCol)
			break;

		if (szText[i] == _T('\t'))
			nDispColCalc += (GetTabSize() - (nDispColCalc % GetTabSize()));
		else
			nDispColCalc++;
	}

	if (nAbsCol == 0)
		nAbsCol = 1;

	return nAbsCol;
}

int CXTPSyntaxEditCtrl::CalcValidDispCol(LPCTSTR szText, int iCol)
{
	int iDispCol = 0;
	const int nLength = (int)_tcsclen(szText);
	for (int i = 0; i < nLength; i++)
	{
		if (iDispCol >= iCol-1)
			break;

		if (szText[i] == _T('\t'))
		{
			// Now calculate tab size
			iDispCol += (GetTabSize() - (iDispCol % GetTabSize()));
		}
		else
			iDispCol++;
	}

	return ++iDispCol;
}

int CXTPSyntaxEditCtrl::CalcMaximumWidth(LPCTSTR szText)
{
	int iMaxWidth = 1;

	const int nLength = (int)_tcsclen(szText);

	for (int i = 0; i < nLength; i++)
	{
		if (szText[i] == _T('\t'))
		{
			// Now calculate tab size
			iMaxWidth += ((GetTabSize() + 1) - (iMaxWidth - ((iMaxWidth / GetTabSize()) * GetTabSize())));
		}
		else
			iMaxWidth++;
	}

	return iMaxWidth;
}

void CXTPSyntaxEditCtrl::GetLineNodeRect(int nRow, CRect& rcNode, CRect* prcNodeFull)
{
	// calculate node rect
	int nYPos = nRow * m_tm.tmHeight;

	int nLeft = GetSelMargin() ? m_nMarginLength : 0;
	nLeft += GetLineNumbers() ? m_nLineNumLength : 0;

	int nRight = nLeft + m_nNodesWidth;

	int nTop = nYPos + (m_tm.tmHeight - m_nNodesWidth) / 2;
	rcNode.SetRect(nLeft+1, nTop, nRight, nTop + m_nNodesWidth - 1);

	if (prcNodeFull)
	{
		prcNodeFull->SetRect(nLeft, nYPos, m_nEditbarLength, nYPos + m_tm.tmHeight);
	}
}

BOOL CXTPSyntaxEditCtrl::ProcessCollapsedTextEx(CDC* pDC, XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk,
								const XTP_EDIT_TEXTBLOCK& txtBlk,
								int iLine, int iCol)
{
	if (!pCoDrawBlk)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	if (txtBlk.nPos < pCoDrawBlk->collBlock.lcStart.nCol)
	{
		return FALSE;
	}

	ProcessCollapsedText(pDC, pCoDrawBlk, iLine, iCol);

	return TRUE;
}

void CXTPSyntaxEditCtrl::ProcessCollapsedText(CDC* pDC, XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk,
										int iLine, int iCol)
{
	CString strText = pCoDrawBlk->collBlock.strCollapsedText; // "[..]"
	int nTextLen = strText.GetLength();

	int nHorzShift = m_siHorz.nPos * m_tm.tmAveCharWidth;
	int nOutCol = (iCol * m_tm.tmAveCharWidth) - nHorzShift;

	if (pDC->IsPrinting())
	{
		nOutCol += m_nLeftMargin;
	}

	nOutCol += 1;
	CRect rcText(nOutCol, iLine,
			nOutCol + nTextLen * m_tm.tmAveCharWidth, iLine + m_tm.tmHeight);

	pCoDrawBlk->rcCollMark = rcText;
	pCoDrawBlk->rcCollMark.left -= 1;
	pCoDrawBlk->rcCollMark.right += 2;
}

CString CXTPSyntaxEditCtrl::GetCollapsedText(XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk, int nMaxLinesCount)
{
	if (!pCoDrawBlk || !m_pBuffer)
	{
		ASSERT(FALSE);
		return _T("");
	}
	CString strCoText, strTmp;

	int nLine1 = pCoDrawBlk->collBlock.lcStart.nLine;
	int nLine2 = pCoDrawBlk->collBlock.lcEnd.nLine;
	int nLine2max = min(nLine2, nLine1 + nMaxLinesCount-1);

	int nCol1 = pCoDrawBlk->collBlock.lcStart.nCol;
	int nCol2 = pCoDrawBlk->collBlock.lcEnd.nCol;

	for (int nLine = nLine1; nLine <= nLine2max; nLine++)
	{
		CString strTmp = m_pBuffer->GetLineText(nLine);
		int nTextLen = strTmp.GetLength();

		if (nTextLen)
		{
			if (nLine == nLine2)
			{
				ASSERT(nCol2 < nTextLen+2);
				nCol2 = max(0, min(nCol2, nTextLen-1));
				strTmp = strTmp.Left(nCol2+1);
			}

			if (nLine == nLine1)
			{
				nTextLen = strTmp.GetLength();
				ASSERT(nCol1 < nTextLen+2);

				if (nCol1 < nTextLen)
				{
					strTmp = strTmp.Mid(nCol1);
				}
				else
				{
					strTmp.Empty();
				}
			}
		}
		if (!strCoText.IsEmpty())
		{
			strCoText += _T("\r\n");
		}
		strCoText += strTmp;
	}

	return strCoText;
}

int CXTPSyntaxEditCtrl::ExpandChars(CDC* pDC, LPCTSTR pszChars, int nCurPos, CString& strBuffer)
{
	int nTabSize = GetTabSize();

	const int nLength = (int)_tcsclen(pszChars);
	if (nLength == 0)
		return 0;

	int nActualOffset = nCurPos;
	int I;
	for (I = 0; I < nLength; I++)
	{
		if (pszChars[I] == _T('\t'))
			nActualOffset += (nTabSize - nActualOffset % nTabSize);
		else
			nActualOffset ++;
	}

	int nActualLength = nActualOffset - nCurPos;

	for (I = 0; I < nLength; I++)
	{
		if (pszChars[I] == _T('\t'))
		{
			int nSpaces = nTabSize - (nCurPos % nTabSize);

			BOOL bFirstChar = TRUE;
			while (nSpaces > 0)
			{
				if (bFirstChar && m_bEnableWhiteSpace && !pDC->IsPrinting())
				{
					strBuffer += (TCHAR)(unsigned char)0xBB;
					bFirstChar = FALSE;
				}
				else
					strBuffer += _T(' ');

				nSpaces--;
				nCurPos++;
			}
		}
		else
		{
			if (m_bEnableWhiteSpace && pszChars[I] == _T(' ') && !pDC->IsPrinting())
				strBuffer += (TCHAR)(unsigned char)0xB7;
			else
				strBuffer += pszChars[I];

			nCurPos++;
		}
	}

	return nActualLength;
}

void CXTPSyntaxEditCtrl::RecalcScrollBars()
{
	if (!XTPSyntaxEditPaintManager()->GetFont()->GetSafeHandle())
		return;

	// create scroll bar info
	SCROLLINFO info;
	ZeroMemory(&info, sizeof(SCROLLINFO));

	int nTopRow = m_nTopCalculatedRow = GetDocumentRow(1);
	int nBottomRow = m_nBottomCalculatedRow = GetDocumentRow(GetRowPerPage());

	int iMax = m_pBuffer->CalcMaxLineTextLength(nTopRow, nBottomRow);

	int nVisRows = GetVisibleRowsCount();
	int iPageX = 20, iPageY = 20;

	if (::IsWindow(m_hWnd))
	{
		CXTPClientRect rcWnd(this);

		iPageX = rcWnd.Width() / m_tm.tmAveCharWidth;
		iPageY = rcWnd.Height() / m_tm.tmHeight;

		iMax += iPageX/2;
	}

	if (GetRowCount() <= 0)
	{
		GetScrollInfo(SB_VERT, &info);

		info.cbSize = sizeof(info);
		info.fMask = SIF_DISABLENOSCROLL | SIF_PAGE;
		info.nPage = iPageY;

		SetScrollInfo(SB_VERT, &info);
		EnableScrollBarCtrl(SB_VERT);

		GetScrollInfo(SB_HORZ, &info);

		info.nMax = iMax;
		info.nPage = iPageX;
		info.fMask = SIF_DISABLENOSCROLL | SIF_PAGE;

		SetScrollInfo(SB_HORZ, &info);
	}
	else
	{
		GetScrollInfo(SB_HORZ, &info);

		int nMaxRowInPage = GetRowPerPage();

		info.nMax = iMax;
		info.nPage = iPageX;
		info.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;

		SetScrollInfo(SB_HORZ, &info);
		EnableScrollBarCtrl(SB_HORZ);

		GetScrollInfo(SB_VERT, &info);
		info.nPage = iPageY;

		BOOL bVertScrl = (nVisRows >= nMaxRowInPage);

		if (bVertScrl)
		{
			info.nMin = 1;
			info.nMax = nVisRows;
			info.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;

			SetScrollInfo(SB_VERT, &info);
		}

		EnableScrollBarCtrl(SB_VERT, bVertScrl);
	}

	CalculateEditbarLength();
}

void CXTPSyntaxEditCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	// Manage AutoCompleteView
	if (m_wndAutoComplete.IsActive())
	{
		CRect rcACcomplete;
		m_wndAutoComplete.GetWindowRect(&rcACcomplete);
		ScreenToClient(&rcACcomplete);
		if (rcACcomplete.PtInRect(point))
			return;
		else
			m_wndAutoComplete.Hide();
	}

	InitiateSelectionVars(point);
	m_bDragging = FALSE;
	m_bSelecting = FALSE;   //m_bColumnSelect = FALSE;
	m_bSelectionStarted = FALSE;
	m_bSelectingFromBookmark = FALSE;
	m_nAutoIndentCol = 0;

	CString strCachedText;

	if (m_bSelectionExist)
	{
		Invalidate(FALSE);
	}

	NotifySelInit();

	SetCapture();

	m_bRightButtonDrag = FALSE;

	BOOL bCtrlKey  = (::GetKeyState(VK_CONTROL) & KF_UP) != 0;
	BOOL bShiftKey  = (::GetKeyState(VK_SHIFT) & KF_UP) != 0;
	BOOL bAltKey  = (::GetKeyState(VK_MENU) & KF_UP) != 0;
	CXTPClientRect rcClient(this);

	int iMaxRow = (((int)((float)(rcClient.bottom - rcClient.top) / m_tm.tmHeight)) * m_tm.tmHeight);
	rcClient.bottom = rcClient.top + iMaxRow;

	CRect rcBookmark(0, 0, GetSelMargin() ? m_nMarginLength : 0, rcClient.Height());
	CRect rcLineNumAndNodes(rcBookmark.right, 0, m_nEditbarLength, rcClient.Height());

	if (!m_bIsOnSelection)
	{
		int nDocRow, nCol, nDispRow;
		RowColFromPoint(point, &nDocRow, &nCol, &nDispRow);
		if (GetCurrentDocumentRow() != nDocRow || m_nDispCol != nCol)
		{
			// reset undo buffer group mode processing
			m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);
		}

		if (m_bDrawNodes)
		{
			CXTPEmptyRect rcNode;

			DWORD dwType = XTP_EDIT_ROWNODE_NOTHING;
			if (GetRowNodes(nDocRow, dwType) && (dwType & (XTP_EDIT_ROWNODE_COLLAPSED | XTP_EDIT_ROWNODE_EXPANDED)))
			{
				GetLineNodeRect(nDispRow-1, rcNode);
			}

			if (rcNode.PtInRect(point) && m_pBuffer)
			{
				m_bSelectionExist = FALSE;

				// expand/collapse a node
				CollapseExpandBlock(nDocRow);

				CWnd::OnLButtonDown(nFlags, point);
				return;
			}
		}

		//--------------------------------------------------------------------
		if (rcBookmark.PtInRect(point))
		{
			m_bSelectionExist = FALSE;

			int nRow_raw = GetDocumentRow(nDispRow);
			if (!NotifyMarginLBtnClick(nRow_raw, nDispRow))
			{
				if (nRow_raw == nDocRow)
				{
					AddRemoveBreakPoint(nDocRow);
					// By Leva: Set cursor here
					SetCurPos(nDocRow, 0);
				}
			}

			CWnd::OnLButtonDown(nFlags, point);
			return;
		}

		//--------------------------------------------------------------------
		if (bCtrlKey && !bAltKey)
		{
			m_bSelectionStarted = TRUE;
			m_bIsSelectingWord = TRUE;
			m_ptWordSelStarted = point;

			SelectWord(point);
			CWnd::OnLButtonDown(nFlags, point);
			return;
		}

		m_bIsSelectingWord = FALSE;

		/////////////////////////////////////////////////////////////////////
		m_bSelectionStarted = TRUE;

		if (!bShiftKey && !bAltKey)
			m_bSelectionExist = FALSE;

		CString strLineText = GetLineText(nDocRow);
		strCachedText = strLineText;

		if (bShiftKey)
		{
			// Shift is pressed so select up to the new row, col
			// from the current row, col
			if (!m_bSelectionExist)
			{
				m_ptStartSel.x = m_nDispCol;//m_nCurrentCol;
				m_ptStartSel.y = GetCurrentDocumentRow();
			}
		}

		SetCurrentDocumentRow(nDocRow);
		int iMaxWidth = CalcMaximumWidth(strLineText);
		m_nDispCol = (iMaxWidth > nCol) ? nCol : iMaxWidth;
		m_nDispCol = CalcValidDispCol(strLineText, m_nDispCol);
		m_nPrvDirtyRow = nDocRow;

		if (!bShiftKey)
		{
			int nCurDocRow = GetCurrentDocumentRow();
			m_ptStartSel = CPoint(m_nDispCol, nCurDocRow);
			m_ptEndSel = CPoint(m_nDispCol, nCurDocRow);
		}

		m_nMaxCol = m_nDispCol;
		m_nCurrentCol = CalcAbsCol(strLineText, m_nDispCol);

		m_bColumnSelect = bAltKey;

		if (bShiftKey || bAltKey)
		{
			// Set the end row col

			m_ptEndSel.x = m_nDispCol;
			m_ptEndSel.y = GetCurrentDocumentRow();

			m_bSelectionExist = TRUE;
			m_bSelectionStarted = FALSE;
			m_bSelecting = TRUE;
		}
		else if (rcLineNumAndNodes.PtInRect(point))
		{
			m_bSelectingFromBookmark = TRUE;

			m_bSelectionExist = TRUE;
			m_ptStartSel.x = 1;
			m_ptStartSel.y = nDocRow;
			m_nBookmarkSelStartRow = nDocRow;

			m_ptEndSel.x = 1;
			m_ptEndSel.y = nDocRow + 1;

			m_nDirtyRow = -1;
			SetCurrentDocumentRow(nDocRow);
			m_nCurrentCol = 1;

			Invalidate(FALSE);

			m_bSelectionStarted = TRUE;
		}

		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, TRUE);

		SetTimer(TIMER_SELECTION_ID, 100, NULL);
	}
	else    // Clicked on selection start dragging
	{
		if (!m_bEnableOleDrag)
			m_bDragging = TRUE;
		else
		{
			NotifyParent(XTP_EDIT_NM_STARTOLEDRAG);
		}

		m_bSelectionStarted = FALSE;
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CXTPSyntaxEditCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	KillTimer(TIMER_SELECTION_ID);

	int nRow = ProcessCollapsedBlockDblClick(point);
	if (nRow > 0)
	{
		// close tooltip.
		ShowCollapsedToolTip(CPoint(0,0));
	}
	else
	{
		// Select the word
		m_bSelectionStarted = TRUE;
		m_bIsSelectingWord = TRUE;
		m_ptWordSelStarted = point;
		m_bSelectingFromBookmark = FALSE;

		//SetCapture();

		SelectWord(point);
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CXTPSyntaxEditCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!GetCapture() || GetCapture() != this || GetFocus() != this)
	{
		return;
	}

	if (GetCapture() == this)
	{
		ReleaseCapture();
	}

	CXTPClientRect rcClient(this);
	CRect rcBookmark( 0, 0, m_nEditbarLength, rcClient.Height());

	KillTimer(TIMER_SELECTION_ID);

	int nCurDocRow = GetCurrentDocumentRow();
	if (m_nCurrentCol <= 1 &&
		nCurDocRow < GetRowCount() &&
		nCurDocRow >= m_nBookmarkSelStartRow &&
		m_bSelectingFromBookmark)
	{
		SetCurrentDocumentRow(nCurDocRow + 1);
		//m_ptEndSel.y++;
	}

	m_bSelecting = FALSE;
	m_bSelectionStarted = FALSE;

	if (m_bDragging && m_bDroppable)
	{
		BOOL bCtrlKey  = (::GetKeyState(VK_CONTROL) & KF_UP) != 0;
		CopyOrMoveText(bCtrlKey);
	}

	if (m_bDragging && !m_bDroppable)
	{
		m_bSelectionExist = FALSE;
	}

	m_bDragging = FALSE;
	m_bDroppable = FALSE;
	m_bIsOnSelection = FALSE;
	m_bIsSelectingWord = FALSE;

	m_nDirtyRow = -1;

	EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE);
	UpdateWindow();

	CWnd::OnLButtonUp(nFlags, point);
}

void CXTPSyntaxEditCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	static CString strMMText;
	static int nPrevRow = -1;
	static BOOL bSelectionStarted = FALSE;

	CXTPClientRect rcClient(this);

	int iMaxRow = (((int)((float)(rcClient.bottom - rcClient.top) / m_tm.tmHeight)) * m_tm.tmHeight);
	rcClient.bottom = rcClient.top + iMaxRow;

	if (m_bSelecting)
	{
		if ((::GetKeyState(VK_LBUTTON) & KF_UP) == 0)
		{
			m_bSelecting = m_bColumnSelect = FALSE;
			m_bSelectionStarted = FALSE;

			CWnd::OnMouseMove(nFlags, point);
			return;
		}

		//--------------------------------------------------------------------
		int nRow = 0, nCol = 0;
		RowColFromPoint(point, &nRow, &nCol);
		if (m_ptEndSel.y == nRow && m_ptEndSel.x == nCol)
		{
			CWnd::OnMouseMove(nFlags, point);
			return;
		}

		//--------------------------------------------------------------------
		CRect rcBookmark(0, 0, m_nEditbarLength, rcClient.Height());

		if (!m_bIsSelectingWord)
		{
			int nCurDocRow = GetCurrentDocumentRow();
			RowColFromPoint(point, &nCurDocRow, &m_nDispCol);
			SetCurrentDocumentRow(nCurDocRow);

			if (m_nDispCol < 1)
				nPrevRow = -1;

			if (nPrevRow == -1 || nPrevRow != nCurDocRow)
			{
				GetLineText(nCurDocRow, strMMText);
				nPrevRow = nCurDocRow;
			}

			if (m_bColumnSelect)
			{
				m_nCurrentCol = m_nMaxCol = m_nDispCol;
			}
			else
			{
				m_nDispCol = CalcValidDispCol(strMMText, m_nDispCol);
				m_nCurrentCol = CalcAbsCol(strMMText, m_nDispCol);

				if (m_nDispCol < 1)
					m_nDispCol = 1;

				int iMaxWidth = CalcMaximumWidth(strMMText);
				m_nMaxCol = m_nDispCol = min(iMaxWidth, m_nDispCol);
			}

			int iStartRow = m_ptStartSel.y;
			int iStartCol = m_ptStartSel.x;
			int iEndRow = GetCurrentDocumentRow();
			int iEndCol = m_nDispCol;

			if (m_nCurrentCol <= 1 && rcBookmark.PtInRect(point) && m_bSelectingFromBookmark)
			{
				nCurDocRow = GetCurrentDocumentRow();

				if (nCurDocRow < m_nBookmarkSelStartRow)
				{
					iEndRow = nCurDocRow;
					iStartRow = m_nBookmarkSelStartRow+1;
				}
				else if (nCurDocRow > m_nBookmarkSelStartRow)
				{
					iEndRow = nCurDocRow + 1;
					iStartRow = m_nBookmarkSelStartRow;
				}
				else
				{
					iStartRow = nCurDocRow;
					iEndRow = nCurDocRow + 1;
				}

				if (iStartRow < 1)
				{
					iStartRow = 1;
					iStartCol = 1;
				}

				if (iEndRow > GetRowCount())
				{
					iEndRow = GetRowCount();
					iEndCol = CalcMaximumWidth(GetLineText(iEndRow));
				}
			}

			m_bSelectionExist = TRUE;

			m_ptStartSel = CPoint(iStartCol, iStartRow);
			m_ptEndSel = CPoint(iEndCol, iEndRow);

			m_nCurrentCol = CalcAbsCol(strMMText, m_nDispCol);

			m_bFocused = TRUE;
		}
		else
		{
			SelectWord(point);
		}

		if (bSelectionStarted)
			m_nPrvDirtyRow = m_ptStartSel.y;

		m_nDirtyRow = GetCurrentDocumentRow();
		nPrevRow = m_nDirtyRow;

		if (m_bSelectingFromBookmark && !rcBookmark.PtInRect(point))
		{
			OnSetCursor(0,0,0);

			m_bSelectingFromBookmark = FALSE;
			m_nDirtyRow = -1;
		}

		if (m_bColumnSelect)
		{
			m_nDirtyRow = -1;
			m_nPrvDirtyRow = -1;
		}

		if (m_bScrolling)
		{
			m_nDirtyRow = -1;
			m_nPrvDirtyRow = -1;
			m_bScrolling = FALSE;
		}

		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, TRUE);
		UpdateWindow();
		CWnd::ShowCaret();

		UpdateScrollPos(XTP_EDIT_UPDATE_HORZ|XTP_EDIT_UPDATE_VERT);

		bSelectionStarted = FALSE;
	}
	else if (m_bSelectionStarted)
	{
		m_bSelectionStarted = FALSE;
		m_bSelecting = TRUE;

		if (!m_bIsSelectingWord)
		{
			int nCurDocRow = GetCurrentDocumentRow();
			GetLineText(nCurDocRow, strMMText);
			RowColFromPoint(point, &nCurDocRow, &m_nDispCol);
			SetCurrentDocumentRow(nCurDocRow);
			m_nDispCol = CalcValidDispCol(strMMText, m_nDispCol);
		}

		bSelectionStarted = TRUE;
	}
	else if (m_bSelectionExist && !m_bDragging && !m_bRightButtonDrag)
	{
	}
	else if (m_bDragging || m_bRightButtonDrag)
	{
		m_bDragging = TRUE;

		CXTPClientRect rcWnd(this);

		rcWnd.left += m_nEditbarLength;

		if (rcWnd.PtInRect(point))
		{
			int iPrevDropRow = m_ptDropPos.y;
			int iPrevDropCol = m_ptDropPos.x;

			RowColFromPoint(point, (int *)&m_ptDropPos.y, (int *)&m_ptDropPos.x);

			if (m_ptDropPos.y < 1)
				m_ptDropPos.y = 1;

			if (m_ptDropPos.y > GetRowCount())
				m_ptDropPos.y = GetRowCount();

			GetLineText(m_ptDropPos.y, strMMText);

			m_nDispCol = CalcValidDispCol(strMMText, m_ptDropPos.x);

			m_ptDropPos.x = m_nDispCol;

			if (iPrevDropRow != m_ptDropPos.y || iPrevDropCol != m_ptDropPos.x)
			{
				EnsureVisible(m_ptDropPos.y, m_ptDropPos.x, FALSE);
				NotifyCurRowCol(m_ptDropPos.y, m_ptDropPos.x);
			}

			m_bDroppable = !RowColInSel(m_ptDropPos.y, m_ptDropPos.x);

			OnSetCursor(0,0,0);
		}
		else
		{
			m_bDroppable = FALSE;
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
	}

	InitiateSelectionVars(point);

	ShowCollapsedToolTip(point);

	CWnd::OnMouseMove(nFlags, point);
}

XTP_EDIT_COLLAPSEDBLOCK* CXTPSyntaxEditCtrl::GetBlockFromPt(const CPoint& ptMouse)
{
	// enumerate all collapsed blocks coordinates
	// to find the one where mouse into
	XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk = NULL;
	XTP_EDIT_LMPARAM LMCoParam;
	int nActualRow = 0;

	for (int i = 0; i < m_nCollapsedTextRowsCount; i++)
	{
		int nRow = m_arCollapsedTextRows[i];

		if (nRow <= nActualRow)
		{
			continue;
		}

		if (!HasRowMark(nRow, xtpEditLMT_Collapsed, &LMCoParam))
		{
			continue;
		}

		// get count of collapsed rows under this row
		int nHiddenRows = 0;
		if (!GetCollapsedBlockLen(nRow, nHiddenRows))
		{
			continue;
		}

		nActualRow = nRow + nHiddenRows;

		// get collapsed block pointer
		pCoDrawBlk = (XTP_EDIT_COLLAPSEDBLOCK*)LMCoParam.GetPtr();

		if (!pCoDrawBlk)
		{
			ASSERT(FALSE);
			continue;
		}

		CRect crCollapsed(pCoDrawBlk->rcCollMark);
		crCollapsed += CSize(m_nEditbarLength, 0);

		if (crCollapsed.PtInRect(ptMouse))
		{
			return pCoDrawBlk;
		}
	}
	return NULL;
}

int CXTPSyntaxEditCtrl::ProcessCollapsedBlockDblClick(const CPoint& ptMouse)
{
	// Find required collapsed block by coordinates
	XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk = GetBlockFromPt(ptMouse);
	if (pCoDrawBlk)
	{
		int nRow = pCoDrawBlk->collBlock.lcStart.nLine;
		CollapseExpandBlock(nRow);
		return nRow;
	}
	return 0;
}

AFX_STATIC CString AFX_CDECL TrimLeftIndent(const CString& csText)
{
	CString csTrimmedText(csText);
	int nLastLine = csTrimmedText.ReverseFind('\n');
	CString csTrimText(csTrimmedText.Right(csTrimmedText.GetLength() - nLastLine).SpanIncluding(_T("\n \t")));
	REPLACE_S(csTrimmedText, csTrimText, _T("\n"));
	return csTrimmedText;
}

void CXTPSyntaxEditCtrl::ShowCollapsedToolTip(const CPoint& ptMouse)
{
	if (ptMouse == m_ptPrevMouse)
	{
		return;
	}

	// Find required collapsed block by coordinates
	XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk = GetBlockFromPt(ptMouse);
	if (pCoDrawBlk)
	{
			m_ptPrevMouse = ptMouse;

			// set tooltip rect
			CRect rcTip(ptMouse, ptMouse);
			rcTip.right += 200;
			rcTip.bottom += 200;
			rcTip += CSize(8, 2);
			ClientToScreen(&rcTip);
			m_wndToolTip.SetHoverRect(rcTip);

			// set tooltip text
			CString strToolText(GetCollapsedText(pCoDrawBlk));
			m_wndToolTip.Activate(TrimLeftIndent(strToolText));
	}

	// Then Hide Tip if it is displayed for some other block
	if (ptMouse != m_ptPrevMouse)
	{
		m_wndToolTip.Hide();
	}
}

void CXTPSyntaxEditCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	InitiateSelectionVars(point);
	m_bDragging = FALSE;
	m_bSelecting = FALSE;
	m_bSelectionStarted = FALSE;
	m_nAutoIndentCol = 0;
	int iRow, iCol;
	RowColFromPoint(point, &iRow, &iCol);

	if (!m_bSelectionExist || (m_bSelectionExist && !RowColInSel(iRow, iCol)))
	{
		LPCTSTR szLineText = GetLineText(iRow);

		SetCurrentDocumentRow(iRow);

		m_nDispCol = CalcValidDispCol(szLineText, iCol);
		m_nMaxCol = m_nDispCol;

		m_nCurrentCol = CalcAbsCol(szLineText, iCol);
		m_nMaxCol = m_nDispCol;

		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol);
		Unselect();
	}

	NotifySelInit();

	KillTimer(TIMER_SELECTION_ID);

	SetCapture();

	if (m_bIsOnSelection && m_bSelectionExist)
	{
		m_bRightButtonDrag = TRUE;

		if (m_bEnableOleDrag)
		{
			NotifyParent(XTP_EDIT_NM_STARTOLEDRAG);
		}
	}

	if (!m_bSelectionExist && !m_bEnableOleDrag)
	{
		RowColFromPoint(point, &iRow, &iCol);

		// SetCurPos will determine valid column
		SetCurPos(iRow, iCol);
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CXTPSyntaxEditCtrl::Scroll(int x, int y)
{
	if (y > 0)
		ShiftCurrentVisibleRowDown(y);
	else if (y < 0)
		ShiftCurrentVisibleRowUp(-y);

	m_nDispCol += x;

	RecalcVertScrollPos();
	RecalcHorzScrollPos();
	Invalidate(FALSE);

	SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, FALSE);

	DWORD dwUpdate = 0;

	if (y != 0)
		dwUpdate |= XTP_EDIT_UPDATE_HORZ;

	if (x != 0)
		dwUpdate |= XTP_EDIT_UPDATE_VERT;

	m_bScrolling = TRUE;

	if (dwUpdate != 0)
		UpdateScrollPos(dwUpdate);
}

BOOL CXTPSyntaxEditCtrl::RowColFromPoint(CPoint pt, int *pRow, int *pCol,
									int *pDispRow, int *pDispCol) //, int iSelectionMode)
{
	BOOL bRet = FALSE;

	pt.x -= m_nEditbarLength;

	int iScrollPos = (GetScrollPos(SB_HORZ) * m_tm.tmAveCharWidth);
	int iStartCol  = 1 + (iScrollPos / m_tm.tmAveCharWidth);

	float fOverload = 0.0f;

	int iVisibleRow = (int)(ceil((float)pt.y / (float)m_tm.tmHeight) + fOverload);
	int iVisibleCol = pt.x / m_tm.tmAveCharWidth +
		((pt.x % m_tm.tmAveCharWidth) > (m_tm.tmAveCharWidth / 2) ? 1 : 0);

	if (pCol)
	{
		*pCol = iVisibleCol + iStartCol;

		if (pt.x < 0)
		{
			*pCol = 0;
			bRet = TRUE;
		}
	}

	if (pRow)
	{
		*pRow = GetDocumentRow(iVisibleRow);

		if (*pRow > GetRowCount())
		{
			*pRow = GetRowCount();
			bRet = TRUE;
		}
		if (*pRow < 1)
		{
			*pRow = 1;
			bRet = TRUE;
		}
	}

	if (pDispRow)
	{
		*pDispRow = iVisibleRow;
	}
	if (pDispCol)
	{
		*pDispCol = iVisibleCol;
	}
	return bRet;
}

int CXTPSyntaxEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create tip window
	m_wndToolTip.Create(this);

	return 0;
}

void CXTPSyntaxEditCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (!XTPSyntaxEditPaintManager()->GetFont()->GetSafeHandle())
		return;

	m_nDirtyRow = -1;

	RecalcVertScrollPos();
	RecalcHorzScrollPos();

	UpdateWindow();
}

int CXTPSyntaxEditCtrl::GetScrollPos(int nBar)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		return pParentWnd->GetScrollPos(nBar);
	}

	return CWnd::GetScrollPos(nBar);
}

void CXTPSyntaxEditCtrl::GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		pParentWnd->GetScrollRange(nBar, lpMinPos, lpMaxPos);
	}
	else
	{
		CWnd::GetScrollRange(nBar, lpMinPos, lpMaxPos);
	}
}

void CXTPSyntaxEditCtrl::ScrollWindow(int xAmount, int yAmount, LPCRECT lpRect, LPCRECT lpClipRect)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		pParentWnd->ScrollWindow(xAmount, yAmount, lpRect, lpClipRect);
	}
	else
	{
		CWnd::ScrollWindow(xAmount, yAmount, lpRect, lpClipRect);
	}
}

int CXTPSyntaxEditCtrl::ScrollWindowEx(int dx, int dy, LPCRECT lpRectScroll, LPCRECT lpRectClip, CRgn *prgnUpdate, LPRECT lpRectUpdate, UINT flags)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		return pParentWnd->ScrollWindowEx(dx, dy, lpRectScroll, lpRectClip, prgnUpdate, lpRectUpdate, flags);
	}

	return CWnd::ScrollWindowEx(dx, dy, lpRectScroll, lpRectClip, prgnUpdate, lpRectUpdate, flags);
}

BOOL CXTPSyntaxEditCtrl::GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		return pParentWnd->GetScrollInfo(nBar, lpScrollInfo, nMask);
	}

	return CWnd::GetScrollInfo(nBar, lpScrollInfo, nMask);
}

int CXTPSyntaxEditCtrl::GetScrollLimit(int nBar)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		return pParentWnd->GetScrollLimit(nBar);
	}

	return CWnd::GetScrollLimit(nBar);
}

BOOL CXTPSyntaxEditCtrl::SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		return pParentWnd->SetScrollInfo(nBar, lpScrollInfo, bRedraw);
	}

	return CWnd::SetScrollInfo(nBar, lpScrollInfo, bRedraw);
}

int CXTPSyntaxEditCtrl::SetScrollPos(int nBar, int nPos, BOOL bRedraw)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		return pParentWnd->SetScrollPos(nBar, nPos, bRedraw);
	}

	return CWnd::SetScrollPos(nBar, nPos, bRedraw);
}

void CXTPSyntaxEditCtrl::SetScrollRange(int nBar, int nMinPos, int nMaxPos, BOOL bRedraw)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		pParentWnd->SetScrollRange(nBar, nMinPos, nMaxPos, bRedraw);
	}
	else
	{
		CWnd::SetScrollRange(nBar, nMinPos, nMaxPos, bRedraw);
	}
}

void CXTPSyntaxEditCtrl::ShowScrollBar(UINT nBar, BOOL bShow)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		pParentWnd->ShowScrollBar(nBar, bShow);
	}
	else
	{
		CWnd::ShowScrollBar(nBar, bShow);
	}
}

void CXTPSyntaxEditCtrl::EnableScrollBarCtrl(int nBar, BOOL bEnable)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		pParentWnd->EnableScrollBarCtrl(nBar, bEnable);
	}
	else
	{
		CWnd::EnableScrollBarCtrl(nBar, bEnable);
	}
}

CScrollBar* CXTPSyntaxEditCtrl::GetScrollBarCtrl(int nBar) const
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		return pParentWnd->GetScrollBarCtrl(nBar);
	}

	return CWnd::GetScrollBarCtrl(nBar);
}

void CXTPSyntaxEditCtrl::RepositionBars(UINT nIDFirst, UINT nIDLast, UINT nIDLeftOver, UINT nFlag, LPRECT lpRectParam, LPCRECT lpRectClient, BOOL bStretch)
{
	CWnd* pParentWnd = DYNAMIC_DOWNCAST(CView, m_pParentWnd);

	if (pParentWnd != NULL && IsCreateScrollbarOnParent())
	{
		pParentWnd->RepositionBars(nIDFirst, nIDLast, nIDLeftOver, nFlag, lpRectParam, lpRectClient, bStretch);
	}
	else
	{
		CWnd::RepositionBars(nIDFirst, nIDLast, nIDLeftOver, nFlag, lpRectParam, lpRectClient, bStretch);
	}
}

BOOL CXTPSyntaxEditCtrl::DeleteSelection(BOOL bRedraw)
{
	if (!m_bSelectionExist)
		return TRUE;

	int nY1, nY2;

	nY1 = min(m_ptStartSel.y, m_ptEndSel.y);
	nY2 = max(m_ptStartSel.y, m_ptEndSel.y);

	if (nY1 <= 0)
		nY1 = 1;

	if (nY2 <= 0)
		nY2 = 1;

	int nFlags = (m_bColumnSelect ? xtpEditTextAsBlock : 0) | xtpEditDispCol;
	if (!DeleteBuffer(m_ptStartSel.y, m_ptStartSel.x,
					  m_ptEndSel.y, m_ptEndSel.x, nFlags))
	{
		return FALSE;
	}

	m_bSelectionExist = FALSE;

	if (bRedraw)
	{
		m_nDirtyRow = -1;
		EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE);
	}

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::DeleteBuffer(int iRowFrom, int iColFrom, int iRowTo, int iColTo,
								 int nFlags )
{
	int iTempRow1 = iRowFrom, iTempRow2 = iRowTo;
	int iTempCol1 = iColFrom, iTempCol2 = iColTo;

	if (iTempRow2 < iTempRow1)
	{
		iRowFrom = iTempRow2;
		iColFrom = iTempCol2;
		iRowTo = iTempRow1;
		iColTo = iTempCol1;
	}
	else if (iTempRow1 == iTempRow2)
	{
		iColFrom = min(iTempCol1, iTempCol2);
		iColTo = max(iTempCol1, iTempCol2);
	}

	if ((nFlags & xtpEditTextAsBlock) &&
		(iColTo < iColFrom))
	{
		// swap them
		int nTemp = iColTo;
		iColTo = iColFrom;
		iColFrom = nTemp;
	}

	if (!CanEditDoc())
	{
		return FALSE;
	}
	BOOL bDispCol = (nFlags & xtpEditDispCol) != 0;
	int nColFrom_str = bDispCol ? CalcAbsCol(GetLineText(iRowFrom), iColFrom) : iColFrom;
	int nColTo_str = bDispCol ? CalcAbsCol(GetLineText(iRowTo), iColTo) : iColTo;

	//**----------------------
	OnBeforeEditChanged(iRowFrom, nColFrom_str);
	//**----------------------

	ASSERT((nFlags & xtpEditTextAsBlock) && bDispCol || !(nFlags & xtpEditTextAsBlock));

	if ((nFlags & xtpEditTextAsBlock) && iRowFrom != iRowTo)
	{
		m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(TRUE);

		for (int i = iRowFrom; i <= iRowTo; i++)
		{
			m_pBuffer->DeleteText(i, iColFrom, i, iColTo, TRUE, bDispCol);
		}
		m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);

		m_pBuffer->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_DELETE_TEXT_BLOCK);
	}
	else
	{
		m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);
		m_pBuffer->DeleteText(iRowFrom, iColFrom, iRowTo, iColTo, TRUE, bDispCol);
	}

	m_nPrvDirtyRow = -1;
	int iDirtyRow = -1;

	if (iRowFrom == iRowTo && !(nFlags & xtpEditForceRedraw))
		iDirtyRow = iRowFrom;

	LPCTSTR szLineText = GetLineText(iRowFrom);
	SetCurrentDocumentRow(iRowFrom);
	m_nCurrentCol = nColFrom_str;
	m_nDispCol = CalcDispCol(szLineText, m_nCurrentCol);
	m_nMaxCol = m_nDispCol;

	//**----------------------
	OnEditChanged(iRowFrom, nColFrom_str, iRowTo, nColTo_str, xtpEditActDelete);
	//**----------------------

	NotifyEditChanged(iRowFrom, iRowTo, (XTP_EDIT_EDITACTION_DELETEROW | XTP_EDIT_EDITACTION_MODIFYROW));

	m_nDirtyRow = iDirtyRow;

	SetDocModified();

	RecalcScrollBars();
	SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol);

	if (nFlags & xtpEditRedraw)
	{
		EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE, TRUE);
		UpdateWindow();
	}

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::DeleteChar(int iRow, int iCol, XTPSyntaxEditDeletePos pos)
{
	CString strText;

	int iRowTo = iRow, iColTo = iCol;

	if (pos == xtpEditDelPosAfter) // If deleting using DEL key
	{
		GetLineText(iRow, strText);

		if (iCol > (int)_tcsclen(strText))
		{
			iRowTo = iRow + 1;
			iColTo = 1;

			if (iRowTo > GetRowCount())
				return FALSE;
		}
		else
			iColTo++;
	}
	else // If deleting using back space
	{
		if (iCol == 1)
		{
			if (iRow > 1)
			{
				iRow--;
				// By Leva: if we are in the end of collapsed block, expand it...
				CXTPSyntaxEditLexTextSchema* ptrTxtSch = m_pBuffer->GetLexParser()->GetTextSchema();
				if (ptrTxtSch)
				{
					CXTPSyntaxEditRowsBlockArray arCoBlocks;
					ptrTxtSch->GetCollapsableBlocksInfo(
						iRow, arCoBlocks, &m_fcCollapsable.ptrTBStart);

					int nCount = (int)arCoBlocks.GetSize();
					for(int i = 0; i < nCount; i++)
					{
						XTP_EDIT_ROWSBLOCK coBlk = arCoBlocks[i];
						if (coBlk.lcEnd.nLine == iRow) // Expand!
						{
							if (HasRowMark(coBlk.lcStart.nLine, xtpEditLMT_Collapsed))
							GetLineMarksManager()->DeleteLineMark(coBlk.lcStart.nLine, xtpEditLMT_Collapsed);
						}
					}
				}

				GetLineText(iRow, strText);
				iCol = (int)_tcsclen(strText) + 1;
			}
			else
				return FALSE;
		}
		else
			iCol--;
	}

	if (!DeleteBuffer(iRow, iCol, iRowTo, iColTo))
		return FALSE;

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::DoUndoRedo(int nActionsCount, BOOL bUndoRedo)
{
	CWaitCursor wait;

	if (!CanEditDoc())
		return FALSE;

	CXTPSyntaxEditUndoRedoManager* pUndoMgr = m_pBuffer->GetUndoRedoManager();
	if (!pUndoMgr)
		return FALSE;

	m_bSelectionExist = FALSE;
	m_nAutoIndentCol = 0;
	int nCurDocRow = GetCurrentDocumentRow();
	XTP_EDIT_LINECOL lcTotalFrom = XTP_EDIT_LINECOL::MAXPOS,
		lcTotalTo = XTP_EDIT_LINECOL::MINPOS;

	for (int nAction = 0; nAction < nActionsCount; nAction++)
	{
		XTP_EDIT_LINECOL lcFrom, lcTo;

		// perform undo
		int nEditAction = bUndoRedo ?
			pUndoMgr->DoUndo(lcFrom, lcTo, this) :
			pUndoMgr->DoRedo(lcFrom, lcTo, this);

		// perform some total calculations
		lcTotalFrom = min(lcTotalFrom, lcFrom);
		lcTotalTo = max(lcTotalTo, lcTo);

		// send update notifications
/*      if (nEditAction & XTP_EDIT_EDITACTION_INSERTTEXT)
		{
			OnEditChanged(lcFrom, lcTo, xtpEditActInsert);
		}
		else if (nEditAction & XTP_EDIT_EDITACTION_DELETETEXT)
		{
			OnEditChanged(lcFrom, lcTo, xtpEditActDelete);
		}*/
		// Commented out by Leva to eliminate double call of OnEditChanged that
		// leads to improper calculation of collapsible blocks borders

		NotifyEditChanged(lcFrom.nLine, lcTo.nLine, nEditAction);
	}

	int nRowsAffected = lcTotalTo.nLine - lcTotalFrom.nLine;

	m_nDispCol = CalcDispCol(GetLineText(GetCurrentDocumentRow()), m_nCurrentCol);
	m_nMaxCol = m_nDispCol;

	RecalcScrollBars();

	if (nActionsCount > 1)
	{
		m_nDirtyRow = -1;
		nCurDocRow = GetCurrentDocumentRow();
		ValidateCol(nCurDocRow, m_nDispCol, m_nCurrentCol);
		EnsureVisible(nCurDocRow, m_nDispCol, TRUE);
	}

	if (nRowsAffected == 0 && nActionsCount == 1)
		m_nDirtyRow = GetCurrentDocumentRow();
	else
	{
		m_nDirtyRow = -1;

		if (GetCurrentDocumentRow() > GetRowCount())
		{
			SetCurrentDocumentRow(GetRowCount());
			m_nCurrentCol = m_nDispCol = m_nMaxCol = 1;
		}
	}

	nCurDocRow = GetCurrentDocumentRow();
	if ((nCurDocRow > 0 && m_nCurrentCol > 0) || m_nDirtyRow == -1)
		EnsureVisible(nCurDocRow, m_nDispCol);

	UpdateWindow();
	UpdateScrollPos();
	SetDocModified(m_pBuffer->IsModified());

	Invalidate(FALSE);

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::Undo(int nActionsCount)
{
	if (CanUndo())
	{
		return DoUndoRedo(nActionsCount, TRUE);
	}
	return FALSE;
}

BOOL CXTPSyntaxEditCtrl::Redo(int nActionsCount)
{
	if (CanRedo())
	{
		return DoUndoRedo(nActionsCount, FALSE);
	}
	return FALSE;
}

BOOL CXTPSyntaxEditCtrl::CanUndo()
{
	return m_pBuffer ? m_pBuffer->GetUndoRedoManager() ? m_pBuffer->GetUndoRedoManager()->CanUndo()
		: FALSE : FALSE;
}

BOOL CXTPSyntaxEditCtrl::CanRedo()
{
	return m_pBuffer ? m_pBuffer->GetUndoRedoManager() ? m_pBuffer->GetUndoRedoManager()->CanRedo()
		: FALSE : FALSE;
}

void CXTPSyntaxEditCtrl::Cut()
{
	if (!m_bSelectionExist)
		return;

	Copy();
	DeleteSelection();
}

void CXTPSyntaxEditCtrl::Copy()
{
	CWaitCursor wait;

	if (!m_bSelectionExist)
		return;

	if (!OpenClipboard())
	{
#ifdef _DEBUG
		LPVOID lpMsgBuf;
		::FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
		);
		// Process any inserts in lpMsgBuf.
		// ...
		// Display the string.
		AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONINFORMATION );

		// Free the buffer.
		::LocalFree( lpMsgBuf );
#endif
		return;
	}

	// prepare clipboard
	::EmptyClipboard();

	// retrieve the text string from buffer
	CMemFile file(CalcAveDataSize(m_ptStartSel.y, m_ptEndSel.y));
	if (!m_pBuffer->GetBuffer(m_ptStartSel.y, m_ptStartSel.x,
					m_ptEndSel.y, m_ptEndSel.x, file, m_bColumnSelect, TRUE))
	{
		return;
	}

	// Copy the buffer to clipboard
	DWORD dwBytes = (DWORD)file.GetLength();
	BYTE *pBytes = file.Detach();

	HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dwBytes+sizeof(TCHAR));
	LPVOID lpSource = (LPVOID)::GlobalLock(hMem);

	if (lpSource)
	{
		MEMCPY_S(lpSource, pBytes, dwBytes);
		::ZeroMemory(((BYTE *)lpSource+dwBytes), sizeof(TCHAR));
	}

	free(pBytes);

	UINT uCodePage = m_pBuffer->GetCodePage();

#ifdef _UNICODE
	// Determine the byte requirement
	int nLen  = ::WideCharToMultiByte(uCodePage, 0, (LPWSTR)lpSource, -1, NULL, 0, NULL, NULL);

	HGLOBAL hMBCSMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (nLen + 2));    // Maximum number of bytes is dwBytes
	LPSTR lpMBCSSource = (LPSTR)::GlobalLock(hMBCSMem);

	int nBytes = ::WideCharToMultiByte(uCodePage, 0, (LPWSTR)lpSource, -1, lpMBCSSource, nLen, NULL, NULL);

	ASSERT(nBytes <= (int)dwBytes);

	lpMBCSSource[nBytes] = _T('\0');

	::GlobalUnlock(hMem);
	::GlobalUnlock(hMBCSMem);

	// lpSource is Unicode text
	::SetClipboardData(CF_UNICODETEXT, hMem);
	::SetClipboardData(CF_TEXT, hMBCSMem);
	::SetClipboardData(CF_OEMTEXT, hMBCSMem);
#else
	int nUBytes = (dwBytes+2) * sizeof(WCHAR);
	HGLOBAL hUnicodeMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, nUBytes);
	LPWSTR lpWSource = (LPWSTR)::GlobalLock(hUnicodeMem);

	//int iLen = ::MultiByteToWideChar(uCodePage, 0, (LPCSTR)lpSource, -1, lpWSource, 0);

	int nLen = ::MultiByteToWideChar(uCodePage, 0, (LPCSTR)lpSource, -1, lpWSource, nUBytes);

	ASSERT(nLen <= (int)(nUBytes/sizeof(WCHAR)));

	lpWSource[nLen] = _T('\0');

	::GlobalUnlock(hMem);
	::GlobalUnlock(hUnicodeMem);

	// lpSource is MBCS text
	::SetClipboardData(CF_TEXT, hMem);
	::SetClipboardData(CF_OEMTEXT, hMem);
	::SetClipboardData(CF_UNICODETEXT, hUnicodeMem);
#endif

	if (m_bColumnSelect)
	{
		HGLOBAL hMem_BLK = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 1);
		BYTE* pMem_BLK = (BYTE*)::GlobalLock(hMem_BLK);
		if (pMem_BLK)
		{
			*pMem_BLK = 1;
		}
		::GlobalUnlock(hMem_BLK);

		CLIPFORMAT uCF_MSDEVColumnSelect = (CLIPFORMAT)::RegisterClipboardFormat(XTP_EDIT_CFMSDEVCOLSEL);
		::SetClipboardData(uCF_MSDEVColumnSelect, hMem_BLK);
	}

	CloseClipboard();
}

void CXTPSyntaxEditCtrl::Paste()
{
	CWaitCursor wait;

	if (!OpenClipboard())
		return;

	BOOL bCBTextIsBlock = FALSE;
	CLIPFORMAT uCF_MSDEVColumnSelect = (CLIPFORMAT)::RegisterClipboardFormat(XTP_EDIT_CFMSDEVCOLSEL);
	HANDLE hMem_TextBlok = ::GetClipboardData(uCF_MSDEVColumnSelect);
	if (hMem_TextBlok)
	{
		int nSize = (int)::GlobalSize(hMem_TextBlok);
		BYTE* arColBlk = (BYTE*)::GlobalLock(hMem_TextBlok);
		if (arColBlk && nSize)
		{
			bCBTextIsBlock = TRUE;
			TRACE(_T("MSDEVColumnSelect data [size=%d] = %d \n"), nSize, (int)arColBlk[0]);
		}
	}

#ifdef _UNICODE
	HANDLE hMem = ::GetClipboardData(CF_UNICODETEXT);
#else
	HANDLE hMem = ::GetClipboardData(CF_TEXT);
#endif

	if (!hMem)
	{
		return;
	}

	LPTSTR szData = (LPTSTR)::GlobalLock(hMem);
	BOOL bReplace = GetTabWithSpace();
	BOOL bOverwrite = m_pBuffer->GetOverwriteFlag();

	SetTabWithSpace(FALSE);
	m_pBuffer->SetOverwriteFlag(FALSE);

	if (bCBTextIsBlock)
	{
		InsertTextBlock(szData, GetCurrentDocumentRow(), m_nCurrentCol, m_bSelectionExist);
	}
	else
	{
		InsertString(szData, GetCurrentDocumentRow(), m_nCurrentCol, m_bSelectionExist);
	}

	SetTabWithSpace(bReplace);
	m_pBuffer->SetOverwriteFlag(bOverwrite);

	m_pBuffer->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_PASTE);

	m_nDirtyRow = -1;

	::GlobalUnlock(hMem);

	CloseClipboard();

	EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE);

	SetDocModified();
}

void CXTPSyntaxEditCtrl::OnSetFocus(CWnd* pOldWnd)
{
	if (m_bCaretCreated)
		::DestroyCaret();

	Invalidate(FALSE);

	RestoreCursor();
	NotifyCurRowCol(GetCurrentDocumentRow(), m_nDispCol);

	UpdateWindow();

	CWnd::OnSetFocus(pOldWnd);

	m_bFocused = TRUE;

#ifdef XTP_SITENOTIFY_ONFOCUS
	XTP_SITENOTIFY_ONFOCUS(this, this, TRUE)
#endif
}

void CXTPSyntaxEditCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	if (m_bCaretCreated)
		::DestroyCaret();

	m_wndAutoComplete.Hide();
	m_bFocused = FALSE;

#ifdef XTP_SITENOTIFY_ONFOCUS
	XTP_SITENOTIFY_ONFOCUS(this, this, FALSE)
#endif
}

BOOL CXTPSyntaxEditCtrl::OnNcActivate(BOOL bActive)
{
	return CWnd::OnNcActivate(bActive);
}

int CXTPSyntaxEditCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
//  SetFocus();

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

BOOL CXTPSyntaxEditCtrl::IsSelectionExist()
{
	return m_bSelectionExist;
}

void CXTPSyntaxEditCtrl::SetPageSize(CSize szPage)
{
	m_szPage = szPage;
}

int CXTPSyntaxEditCtrl::CalcPageCount()
{
	if (!XTPSyntaxEditPaintManager()->GetFont()->GetSafeHandle())
		return 1;

	CSize szPage = GetEffectivePageSize();

	CWindowDC dc(NULL);
	CXTPFontDC fontDC(&dc, XTPSyntaxEditPaintManager()->GetFont());

	TEXTMETRIC tm;
	VERIFY(dc.GetTextMetrics(&tm));

	int nLinesPerPage = (int)(szPage.cy / tm.tmHeight) - 1;
	int nLinesTotal = GetVisibleRowsCount();
	int nNumPages = (nLinesTotal / nLinesPerPage) +
		(((nLinesTotal % nLinesPerPage) > 0) ? 1 : 0);

	if (nNumPages < 1)
		nNumPages = 1;

	m_nNumPages = nNumPages;

	return nNumPages;
}

void CXTPSyntaxEditCtrl::SetCurrentPage(int iPage)
{
	if (iPage == m_nCurrentPage)
		return;

	m_nCurrentPage = iPage;
}

void CXTPSyntaxEditCtrl::SelectAll()
{
	int iLastRow = GetRowCount();
	LPCTSTR szLineText = GetLineText(iLastRow);
	const int iEndCol = (int)_tcsclen(szLineText) + 1;

	m_bSelectionExist = TRUE;

	SetCurrentDocumentRow(iLastRow);
	m_nCurrentCol = iEndCol;
	m_nMaxCol = m_nCurrentCol;

	m_nDispCol = CalcDispCol(szLineText, m_nCurrentCol);

	m_ptStartSel.x = 1;
	m_ptStartSel.y = 1;
	m_ptEndSel.x = m_nDispCol;
	m_ptEndSel.y = iLastRow;
	m_nDirtyRow = -1;
	m_nAutoIndentCol = 0;

	EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE);
}

void CXTPSyntaxEditCtrl::SetCurPos(int iRow, int iCol, BOOL bRemainSelected, BOOL bForceVisible)
{
	if (iRow > GetRowCount())
		iRow = GetRowCount();

	SetCurrentDocumentRow(iRow);

	m_nDispCol = iCol;
	ValidateCol(iRow, m_nDispCol, m_nCurrentCol);
	m_nMaxCol = m_nDispCol;

	BOOL bRedraw = FALSE;

	if (!bRemainSelected)
	{
		if (m_bSelectionExist)
		{
			m_bSelectionExist = FALSE;
			bRedraw = TRUE;
			m_nDirtyRow = -1;
		}

		EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, bRedraw, bForceVisible);
		UpdateWindow();

		UpdateScrollPos(XTP_EDIT_UPDATE_HORZ|XTP_EDIT_UPDATE_VERT);
	}
	else
	{
		if (bForceVisible)
		{
			SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, bForceVisible);

			UpdateWindow();
			UpdateScrollPos(XTP_EDIT_UPDATE_HORZ|XTP_EDIT_UPDATE_VERT);
		}
	}
}

void CXTPSyntaxEditCtrl::SetCurSel(int iRow1, int iCol1, int iRow2, int iCol2)
{
	int iTempAbsCol = 0;

	ValidateCol(iRow1, iCol1, iTempAbsCol);
	ValidateCol(iRow2, iCol2, iTempAbsCol);

	m_ptStartSel = CPoint(iCol1, iRow1);
	m_ptEndSel = CPoint(iCol2, iRow2);

	NormalizeSelection();

	if (m_ptStartSel != m_ptEndSel)
		m_bSelectionExist = TRUE;
}

BOOL CXTPSyntaxEditCtrl::GetCurSel(CPoint &ptStart, CPoint &ptEnd)
{
	if (!m_bSelectionExist)
		return FALSE;

	NormalizeSelection();

	ptStart = m_ptStartSel;
	ptEnd = m_ptEndSel;

	return TRUE;
}

void CXTPSyntaxEditCtrl::SetFontIndirect(LPLOGFONT pLogfont, BOOL bUpdateReg/*=FALSE*/)
{
	// If pLogFont is not NULL update paint manager fonts.
	if (pLogfont != NULL)
	{
		XTPSyntaxEditPaintManager()->CreateFontIndirect(pLogfont, bUpdateReg);
	}

	CWnd::SetFont(XTPSyntaxEditPaintManager()->GetFont());

	CWindowDC dc(NULL);
	CXTPFontDC fontDC(&dc, XTPSyntaxEditPaintManager()->GetFont());
	VERIFY(dc.GetTextMetrics(&m_tm));
	m_tm.tmHeight++;

	// ToolTip font - get default
	m_wndToolTip.SetFont(XTPSyntaxEditPaintManager()->GetFontToolTip());

	CalculateEditbarLength();

	// Refresh picture
	if (::IsWindow(m_hWnd))
	{
		SetDirtyRow(-1);
		Invalidate(FALSE);
	}
}

LRESULT CXTPSyntaxEditCtrl::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return (LRESULT)XTPSyntaxEditPaintManager()->GetFont()->GetSafeHandle();
}

void CXTPSyntaxEditCtrl::SelectWord(CPoint point)
{
	int iRow = 0, iCol = 0;

	if (RowColFromPoint(point, &iRow, &iCol)) // By Leva: point was out of bound, do not selecet anything
			return;

	const CString strSeps(_T("&%\"|\t[](){}:,;=+-/\\<>. "));

	if (iRow >= 1 && iRow <= GetRowCount())
	{
		CString strBuffer;
		GetLineText(iRow, strBuffer);

		iCol = CalcValidDispCol(strBuffer, iCol);

		m_nDispCol = iCol;
		m_nMaxCol = m_nDispCol;
		iCol = CalcAbsCol(strBuffer, iCol);
		m_nCurrentCol = iCol;

		const int iLen = (int)_tcsclen(strBuffer);

		int iColFrom = iCol - 1,
			iColTo   = iCol - 1;
		BOOL bStartFound = FALSE, bEndFound = FALSE;

		while (iColFrom >= 0 || iColTo < iLen)
		{
			if (iColFrom >= 0 && iColFrom < iLen &&
				strSeps.Find(strBuffer.GetAt(iColFrom)) >= 0)   // Separators Found
			{
				bStartFound = TRUE;
			}

			if (iColTo >= 0 && iColTo < iLen &&
				strSeps.Find(strBuffer.GetAt(iColTo)) >= 0) // Separators Found
			{
				bEndFound = TRUE;
			}

			if (!bStartFound)
			{
				if (iColFrom >= 1)
					iColFrom--;
				else
				{
					iColFrom = -1;
					bStartFound = TRUE;
				}
			}

			if (!bEndFound)
			{
				if (iColTo < iLen)
					iColTo++;
				else
				{
					iColTo = iLen;
					bEndFound = TRUE;
				}
			}

			if (bStartFound && bEndFound)
				break;
		}

		if (iColFrom == iColTo)
		{
			iColFrom--;
			iColTo++;
		}

		int iDispCol1 = CalcDispCol(strBuffer, iColFrom+2);
		int iDispCol2 = CalcDispCol(strBuffer, iColTo+1);

		int iCursorDispCol = iDispCol2;
		int iCursorCol = iColTo;

		BOOL bSelectingBackward = FALSE;
		if (m_bSelecting &&
			( (iRow < m_ptStartSel.y) ||
			  (iRow == m_ptStartSel.y &&
			   iDispCol1 < m_ptStartSel.x)))
		{
			bSelectingBackward = TRUE;
			iCursorDispCol = iDispCol1;
			iCursorCol = iColFrom;
		}

		if (!m_bSelecting)
		{
			m_ptStartSel = CPoint(iDispCol1, iRow);
			m_ptEndSel   = CPoint(iDispCol2, iRow);

			SetTimer(TIMER_SELECTION_ID, 100, NULL);
		}
		else
		{
			CPoint ptEndSel   = (bSelectingBackward) ? CPoint(iDispCol1, iRow) : CPoint(iDispCol2, iRow);

			// Now try to find out the next or previous separator
			// for start selection depending on bSelectingBackward

			if (!RowColInSel(ptEndSel.y, ptEndSel.x))
			{
				CString strStartLineText;
				GetLineText(m_ptStartSel.y, strStartLineText);

				int iRow, iCol;
				RowColFromPoint(m_ptWordSelStarted, &iRow, &iCol);

				iCol = CalcValidDispCol(strStartLineText, iCol);

				int nActualCol = CalcAbsCol(strStartLineText, iCol);
				const int iLen = (int)_tcsclen(strStartLineText);

				int nCalculatedCol = nActualCol;

				if (bSelectingBackward)
				{
					if ((nCalculatedCol <= iLen) &&
						 (strSeps.Find(strStartLineText.GetAt(nCalculatedCol - 1)) >= 0))
					{
						nCalculatedCol++;
					}
					else
					{
						while (nCalculatedCol <= iLen)
						{
							if (strSeps.Find(strStartLineText.GetAt(nCalculatedCol - 1)) >= 0)
								break;

							nCalculatedCol++;
						}
					}
				}
				else
				{
					if (nCalculatedCol == 1 ||
						(nCalculatedCol < strStartLineText.GetLength() &&
						 strSeps.Find(strStartLineText.GetAt(nCalculatedCol - 1)) != -1))
					{
						nCalculatedCol--;
					}
					else
					{
						while (nCalculatedCol >= 1)
						{
							if (nCalculatedCol < strStartLineText.GetLength() &&
								strSeps.Find(strStartLineText.GetAt(nCalculatedCol - 1)) >= 0)
							{
								break;
							}

							nCalculatedCol--;
						}
					}

					nCalculatedCol++;
				}

				int iCalculatedDispCol = CalcDispCol(strStartLineText, nCalculatedCol);
				m_ptStartSel = CPoint(iCalculatedDispCol, m_ptStartSel.y);
			}

			// Now prepare the end selection point
			m_ptEndSel = ptEndSel;
		}

		m_bSelectionExist = TRUE;

		SetCurrentDocumentRow(iRow);
		m_nCurrentCol = iCursorCol;
		m_nDispCol = iCursorDispCol;
		m_nMaxCol = m_nDispCol;

		if (!m_bSelecting)
		{
			m_nDirtyRow = -1;

			SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, TRUE);
			UpdateWindow();

			UpdateScrollPos(XTP_EDIT_UPDATE_HORZ);
		}
	}
}

void CXTPSyntaxEditCtrl::SetDocModified(BOOL bModified)
{
	XTP_EDIT_NMHDR_DOCMODIFIED dm;

	// NMHDR codes
	dm.nmhdr.code = XTP_EDIT_NM_SETDOCMODIFIED;
	dm.nmhdr.hwndFrom = m_hWnd;
	dm.nmhdr.idFrom = GetDlgCtrlID();

	// modified flag
	dm.bModified = bModified;

	// Notify the parent window
	if (::IsWindow(m_pParentWnd->GetSafeHwnd()))
	{
		m_pParentWnd->SendMessage(
			WM_NOTIFY, (WPARAM)dm.nmhdr.idFrom, (LPARAM)&dm);
	}
}

BOOL CXTPSyntaxEditCtrl::RowColInSel(int iRow, int iCol)
{
	if (!m_bSelectionExist)
		return FALSE;

	if (iCol < 1)
		return FALSE;

	CPoint ptStart = m_ptStartSel;
	CPoint ptEnd   = m_ptEndSel;

	CPoint ptTemp;

//  if (ptEnd.y >= ptStart.y && ptEnd.x >= ptStart.x)
//  {
		// Leave as it is
//  }

	if (ptEnd.y < ptStart.y)
	{
		CPoint temp = ptEnd;
		ptEnd = ptStart;
		ptStart = temp;
	}

	if ((ptEnd.x < ptStart.x) && (ptStart.y == ptEnd.y || m_bColumnSelect))
	{
		int temp = ptEnd.x;
		ptEnd.x = ptStart.x;
		ptStart.x = temp;
	}

	if (ptStart.y == ptEnd.y || m_bColumnSelect)
	{
		if (iRow >= ptStart.y && iRow <= ptEnd.y)
		{
			return (iCol >= ptStart.x && iCol < ptEnd.x);
		}
	}

	if ((iRow == ptStart.y) &&
		(iCol >= ptStart.x))
	{
		return TRUE;
	}

	if ((iRow == ptEnd.y) &&
		(iCol <= ptEnd.x))
	{
		return TRUE;
	}

	if (iRow > ptStart.y && iRow < ptEnd.y)
		return TRUE;

	return FALSE;
}

void CXTPSyntaxEditCtrl::InsertString(LPCTSTR szText, int iRow, int iCol, BOOL bDeleteSelection, BOOL bRedraw)
{
	if (!CanEditDoc())
		return;

	int nPrevRow = GetCurrentDocumentRow();

	if (nPrevRow != iRow)
		SetCurrentDocumentRow(iRow);
	m_nCurrentCol = iCol;

	if (bDeleteSelection)
		DeleteSelection(bRedraw);

	// At first determine what type of CRLF it has
	const int nTextLen = (int)_tcsclen(szText);

	if (nTextLen == 0)
		return;

	int nRowFrom = GetCurrentDocumentRow();
	int nColFrom = m_nCurrentCol;

	//**----------------------
	OnBeforeEditChanged(nRowFrom, nColFrom);
	//**----------------------

	if (GetAutoIndent() && m_nAutoIndentCol > 0)
	{
		CString strInsertText(
			CString(_T('\t'), m_nInsertTabCount) +
			CString(_T(' '), m_nInsertSpaceCount));

		m_pBuffer->InsertText(strInsertText, GetCurrentDocumentRow(), m_nCurrentCol);

		m_nCurrentCol = m_nInsertTabCount + m_nInsertSpaceCount + 1;

		m_nAutoIndentCol = 0;
	}


	XTP_EDIT_LINECOL finalLC;
	BOOL bInsRes = m_pBuffer->InsertText(szText, GetCurrentDocumentRow(),
										m_nCurrentCol, TRUE, &finalLC);

	if (bDeleteSelection)
		m_pBuffer->GetUndoRedoManager()->ChainLastCommand();

	if (bInsRes)
	{
		SetCurrentDocumentRow(finalLC.nLine);
		m_nCurrentCol = finalLC.nCol;

		m_nDispCol = CalcDispCol(GetLineText(finalLC.nLine), m_nCurrentCol);
	}

	m_nMaxCol = m_nDispCol;

	RecalcScrollBars();

	UINT nAction = XTP_EDIT_EDITACTION_MODIFYROW;

	int nCurDocRow = GetCurrentDocumentRow();
	if (nCurDocRow > nPrevRow)
		nAction |= XTP_EDIT_EDITACTION_INSERTROW;

	//**----------------------
	OnEditChanged(nRowFrom, nColFrom, nCurDocRow, m_nCurrentCol, xtpEditActInsert);
	//**----------------------

	NotifyEditChanged(nPrevRow, nCurDocRow, nAction);

	if (bRedraw)
	{
		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, bRedraw);

		UpdateWindow();
	}
}

void CXTPSyntaxEditCtrl::InsertTextBlock(LPCTSTR szText, int iRow, int iCol,
									  BOOL bDeleteSelection, BOOL bRedraw)
{
	if (!CanEditDoc())
		return;

	int nPrevRow = GetCurrentDocumentRow();

	if (nPrevRow != iRow)
	{
		SetCurrentDocumentRow(iRow);
	}
	m_nCurrentCol = iCol;

	if (bDeleteSelection)
	{
		DeleteSelection(bRedraw);
	}

	// At first determine what type of CRLF it has
	const int nTextLen = (int)_tcsclen(szText);

	if (nTextLen == 0)
		return;

	int nRowFrom = GetCurrentDocumentRow();
	int nColFrom = m_nCurrentCol;

	//**----------------------
	OnBeforeEditChanged(nRowFrom, nColFrom);
	//**----------------------

	XTP_EDIT_LINECOL finalLC;
	BOOL bInsRes = m_pBuffer->InsertTextBlock(szText, GetCurrentDocumentRow(),
												iCol, TRUE, &finalLC);
	UNREFERENCED_PARAMETER(bInsRes);

	if (bDeleteSelection)
		m_pBuffer->GetUndoRedoManager()->ChainLastCommand();

	RecalcScrollBars();

	UINT nAction = XTP_EDIT_EDITACTION_MODIFYROW;

	int nCurDocRow = GetCurrentDocumentRow();
	if (nCurDocRow > nPrevRow)
		nAction |= XTP_EDIT_EDITACTION_INSERTROW;

	//**----------------------
	OnEditChanged(nRowFrom, nColFrom, nCurDocRow, m_nCurrentCol, xtpEditActInsert);
	//**----------------------

	NotifyEditChanged(nPrevRow, nCurDocRow, nAction);

	if (bRedraw)
	{
		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, bRedraw);

		UpdateWindow();
	}
}

void CXTPSyntaxEditCtrl::UpdateScrollPos(DWORD dwUpdate/* = XTP_EDIT_UPDATE_ALL*/)
{
	XTP_EDIT_NMHDR_SETSCROLLPOS ssp;

	// NMHDR codes
	ssp.nmhdr.code = XTP_EDIT_NM_UPDATESCROLLPOS;
	ssp.nmhdr.hwndFrom = m_hWnd;
	ssp.nmhdr.idFrom = GetDlgCtrlID();

	// Update flag
	ssp.dwUpdate = dwUpdate;

	// Notify the parent window
	if (::IsWindow(m_pParentWnd->GetSafeHwnd()))
	{
		m_pParentWnd->SendMessage(
			WM_NOTIFY, (WPARAM)ssp.nmhdr.idFrom, (LPARAM)&ssp);
	}
}

void CXTPSyntaxEditCtrl::SetCurCaretPos(int nRow, int nCol, BOOL bMakeVisible)
{
	CXTPClientRect rc(this);

	BOOL bHideCaret = FALSE;

	int iHorzScrollPos = GetScrollPos(SB_HORZ);

	int iMaxHorzPos    = ((( (iHorzScrollPos * m_tm.tmAveCharWidth) + rc.Width() + m_nEditbarLength) / m_tm.tmAveCharWidth) * m_tm.tmAveCharWidth) - (GetSystemMetrics(SM_CXVSCROLL) + 25);
	iMaxHorzPos /= m_tm.tmAveCharWidth;

	int iXPos = (nCol - iHorzScrollPos - 1) * m_tm.tmAveCharWidth;
	int iYPos = (nRow - 1) * m_tm.tmHeight;

	int iXShift = 0;
	if ((nCol) > iMaxHorzPos)
	{
		iXShift = (((nCol + 10) * m_tm.tmAveCharWidth) - iMaxHorzPos);
		iXPos -= iXShift;
	}

	if ((nCol * m_tm.tmAveCharWidth) < iHorzScrollPos)
	{
		iXShift = -(iHorzScrollPos - ((nCol - 1) * m_tm.tmAveCharWidth));
		iXPos = -1;
	}

	if (iXPos < 0)
		bHideCaret = TRUE;

	iXPos += m_nEditbarLength;

	if (bMakeVisible)
	{
		EnsureVisible(GetDocumentRow(nRow), nCol, TRUE);
		return;
	}
	else
	{
		if (bHideCaret)
			CWnd::HideCaret();
		else if (GetFocus() == this)
		{
			CWnd::CreateSolidCaret(GetWideCaret()? m_tm.tmAveCharWidth: 2, m_tm.tmHeight);
			CWnd::SetCaretPos(CPoint(iXPos, iYPos));
			CWnd::ShowCaret();
		}
	}
}

void CXTPSyntaxEditCtrl::SetOverwriteMode(BOOL bOverwriteMode)
{
	if (bOverwriteMode != m_pBuffer->GetOverwriteFlag() )
	{
		m_pBuffer->SetOverwriteFlag(bOverwriteMode);

		CPoint ptCarPos = CWnd::GetCaretPos();

		CWnd::CreateSolidCaret(GetWideCaret()? m_tm.tmAveCharWidth: 2, m_tm.tmHeight);
		CWnd::SetCaretPos(ptCarPos);
		CWnd::ShowCaret();
	}
}

BOOL CXTPSyntaxEditCtrl::OnEraseBkgnd(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

void CXTPSyntaxEditCtrl::InitiateSelectionVars(CPoint& point)
{
	m_bIsOnSelection = FALSE;
	if (!m_bSelecting && m_bSelectionExist && !m_bDragging)
	{
		int nLine = 0, nCol = 0;
		RowColFromPoint(point, &nLine, &nCol);
		m_ptDropPos = CPoint(nCol, nLine);

		if (RowColInSel(m_ptDropPos.y, m_ptDropPos.x))
		{
			m_ptDropPos.y = -1;
			m_ptDropPos.x = -1;
			m_bDroppable = FALSE;
			m_bIsOnSelection = TRUE;
		}
	}
}

void CXTPSyntaxEditCtrl::CopyOrMoveText(BOOL bCopy)
{
	if (!m_bSelectionExist)
		return;

	if (RowColInSel(m_ptDropPos.y, m_ptDropPos.x))
		return;

	CString strText;

	m_pBuffer->GetLineText(m_ptStartSel.y, strText);
	int nAbsStartCol = CalcAbsCol(strText, m_ptStartSel.x);

	if (m_ptStartSel.y != m_ptEndSel.y)
		m_pBuffer->GetLineText(m_ptEndSel.y, strText);

	int nAbsEndCol = CalcAbsCol(strText, m_ptEndSel.x);

	CMemFile memfile(CalcAveDataSize(m_ptStartSel.y, m_ptEndSel.y));
	if (m_pBuffer->GetBuffer(m_ptStartSel.y, m_ptStartSel.x,
		m_ptEndSel.y, m_ptEndSel.x, memfile, m_bColumnSelect))
	{
		memfile.Write((const void *)_T("\0"), sizeof(TCHAR));
		BYTE *pBytes = memfile.Detach();

		CString strDropText = (LPTSTR) pBytes;
		free(pBytes);

		// First insert the text
		if (m_ptDropPos.y > GetRowCount())
			m_ptDropPos.y = GetRowCount();

		// Determine if the selection row, col has changed or not
		int iSelRow1 = m_ptStartSel.y;
		int iSelCol1 = nAbsStartCol;
		int iSelRow2 = m_ptEndSel.y;
		int iSelCol2 = nAbsEndCol;

		if (iSelRow1 == iSelRow2 || m_bColumnSelect)
		{
			int nTempCol1 = iSelCol1;
			int nTempCol2 = iSelCol2;

			iSelCol1 = min(nTempCol1, nTempCol2);
			iSelCol2 = max(nTempCol1, nTempCol2);
		}

		if (iSelRow1 > iSelRow2)
		{
			int nTempRow2 = iSelRow2;
			int nTempCol2 = iSelCol2;

			iSelRow2 = iSelRow1;
			iSelCol2 = iSelCol1;

			iSelRow1 = nTempRow2;
			iSelCol1 = nTempCol2;
		}

		BOOL bSingleLineSel = (iSelRow1 == iSelRow2);
		int iLineCount = (iSelRow2 - iSelRow1);

		m_pBuffer->GetLineText(m_ptDropPos.y, strText);
		int nAbsDropCol = CalcAbsCol(strText, m_ptDropPos.x);

		if (!bCopy)
		{
			// delete selection if control is not pressed
			DeleteSelection(FALSE);
		}

		//**----------------------
		OnBeforeEditChanged(m_ptDropPos.y, nAbsDropCol);
		//**----------------------

		if ((m_ptDropPos.y == iSelRow1 && nAbsDropCol < iSelCol1) || (m_ptDropPos.y < iSelRow1))
		{
			// Dropped before the current selection
			int iSelCount = (iSelCol2 - iSelCol1);

			iSelRow1 = m_ptDropPos.y;
			iSelCol1 = nAbsDropCol;

			iSelRow2 = iSelRow1 + iLineCount;
			if (bSingleLineSel ||m_bColumnSelect)
			{
				iSelCol2 = iSelCol1 + iSelCount;
			}
			else
			{
				// iSelCol2 will be unchanged
			}
		}
		else if ((m_ptDropPos.y == iSelRow2 && m_ptDropPos.x >= iSelCol2) || (m_ptDropPos.y > iSelRow2))
		{
			if (!bCopy)
			{
				if (m_ptDropPos.y == iSelRow2)
				{
					nAbsDropCol = iSelCol1 + (nAbsDropCol - iSelCol2);
				}

				if (!m_bColumnSelect)
				{
					m_ptDropPos.y -= iLineCount;
				}
			}

			// Determine the selection rect
			int iSelCount = (iSelCol2 - iSelCol1);

			iSelRow1 = m_ptDropPos.y;
			iSelCol1 = nAbsDropCol;

			iSelRow2 = iSelRow1 + iLineCount;
			if (bSingleLineSel || m_bColumnSelect)
			{
				iSelCol2 = (iSelCol1 + iSelCount);
			}
			else
			{
				// iSelCol2 will be unchanged
			}

			m_pBuffer->GetLineText(m_ptDropPos.y, strText);
		}
		else if (m_bColumnSelect &&
				(m_ptDropPos.y >= iSelRow1 && m_ptDropPos.y <= iSelRow2 &&
				 m_ptDropPos.x >= iSelCol2) )
		{
			int iSelCount = (iSelCol2 - iSelCol1);

			if (!bCopy)
			{
				nAbsDropCol -= iSelCount;
			}

			// Determine the selection rect
			iSelRow1 = m_ptDropPos.y;
			iSelCol1 = nAbsDropCol;
			iSelCol2 = (iSelCol1 + iSelCount);

			iSelRow2 = iSelRow1 + iLineCount;
		}

		BOOL bReplace = GetTabWithSpace();
		SetTabWithSpace(FALSE);

		XTP_EDIT_LINECOL finalLC;
		BOOL bInsRes = FALSE;
		if (m_bColumnSelect)
		{
			bInsRes = m_pBuffer->InsertTextBlock(strDropText, m_ptDropPos.y, nAbsDropCol,
													TRUE, &finalLC);
		}
		else
		{
			bInsRes = m_pBuffer->InsertText(strDropText, m_ptDropPos.y, nAbsDropCol,
											TRUE, &finalLC);
		}

		SetTabWithSpace(bReplace);

		if (!bCopy)
		{
			m_pBuffer->GetUndoRedoManager()->ChainLastCommand();
			m_pBuffer->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_MOVE);
		}
		else
		{
			// It's a drag copy
			m_pBuffer->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_COPY);
		}

		int nColTo = 0;
		if (bInsRes)
		{
			LPCTSTR szDropLineText = GetLineText(m_ptDropPos.y);
			iSelCol1 = CalcDispCol(szDropLineText, iSelCol1);

			iSelRow2 = finalLC.nLine;
			iSelCol2 = finalLC.nCol;
			nColTo = iSelCol2;

			LPCTSTR szSelRow2Text = GetLineText(iSelRow2);
			iSelCol2 = CalcDispCol(szSelRow2Text, iSelCol2);
		}

		m_ptStartSel = CPoint(iSelCol1, iSelRow1);
		m_ptEndSel = CPoint(iSelCol2, iSelRow2);

		int nCurDocRow = GetCurrentDocumentRow();

		if (!m_bColumnSelect)
		{
			SetCurrentDocumentRow(iSelRow2);
			m_nDispCol = iSelCol2;
			m_nMaxCol = m_nDispCol;

			ValidateCol(nCurDocRow, m_nDispCol, m_nCurrentCol);
		}
		m_bSelectionExist = TRUE;

		CalculateEditbarLength();
		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, TRUE);

		SetDocModified();

		int iRowFrom = min(nCurDocRow, m_ptDropPos.y);
		int iRowTo = max(nCurDocRow, m_ptDropPos.y);
		UINT nAction = (XTP_EDIT_EDITACTION_MODIFYROW | XTP_EDIT_EDITACTION_INSERTROW);

		//**----------------------
		OnEditChanged(iRowFrom, nAbsDropCol, iRowTo, nColTo, xtpEditActInsert);
		//**----------------------

		UpdateWindow();

		NotifyEditChanged(iRowFrom, iRowTo, nAction);
	}
}

void CXTPSyntaxEditCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (GetCapture() == this)
	{
		ReleaseCapture();
	}

	if (m_bRightButtonDrag && m_bDragging && m_bDroppable)
	{
		HandleDrop(FALSE);
	}
	else
	{
		CXTPClientRect rcWnd(this);
		ClientToScreen(&rcWnd);

		m_bDragging = FALSE;

		if (m_pParentWnd && rcWnd.PtInRect(point))
		{
			CWnd::HideCaret();
			m_pParentWnd->SendMessage(WM_CONTEXTMENU, (WPARAM)pWnd->GetSafeHwnd(), (LPARAM)MAKELONG(point.x, point.y));
			CWnd::ShowCaret();
			m_nDirtyRow = -1;
			Invalidate(FALSE);
			UpdateWindow();
		}
	}

	m_bRightButtonDrag = FALSE;
}

void CXTPSyntaxEditCtrl::OnDragCopy()
{
	CopyOrMoveText(TRUE);
	m_bRightButtonDrag = FALSE;
}

void CXTPSyntaxEditCtrl::OnDragMove()
{
	CopyOrMoveText(FALSE);
	m_bRightButtonDrag = FALSE;
}

void CXTPSyntaxEditCtrl::RestoreCursor()
{
	CWnd::CreateSolidCaret(GetWideCaret()? m_tm.tmAveCharWidth: 2, m_tm.tmHeight);
	CWnd::ShowCaret();

	Invalidate(FALSE);

	SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, FALSE);
}

void CXTPSyntaxEditCtrl::SetRowBkColor(int nRow, COLORREF crBack)
{
	if (crBack == RGB(255,255,255))
		m_mapRowBkColor.RemoveKey(nRow);
	else
		m_mapRowBkColor[nRow] = crBack;
}

void CXTPSyntaxEditCtrl::SetRowColor(int nRow, COLORREF clrFront)
{
	if (clrFront == RGB(0,0,0))
		m_mapRowColor.RemoveKey(nRow);
	else
		m_mapRowColor[nRow] = clrFront;
}

COLORREF CXTPSyntaxEditCtrl::GetRowColor(int nRow)
{
	COLORREF clrFront;

	if (!m_mapRowColor.Lookup(nRow, clrFront))
	{
		clrFront = RGB(0,0,0);
	}

	return clrFront;
}

COLORREF CXTPSyntaxEditCtrl::GetRowBkColor(int nRow)
{
	COLORREF crBack;
	if (!m_mapRowBkColor.Lookup(nRow, crBack))
	{
		crBack = RGB(255,255,255);
	}

	return crBack;
}

AFX_STATIC void CopyRowColorMap(CXTPSyntaxEditRowColorMap& dest, const CXTPSyntaxEditRowColorMap& src)
{
	dest.RemoveAll();

	for (POSITION pos = src.GetStartPosition(); pos;)
	{
		int nRow;
		COLORREF crBack;
		src.GetNextAssoc(pos, nRow, crBack);
		dest[nRow] = crBack;
	}
}

const CXTPSyntaxEditCtrl& CXTPSyntaxEditCtrl::operator=(const CXTPSyntaxEditCtrl& src)
{
	if (::IsWindow(src.m_hWnd))
	{
		SetCurrentDocumentRow(src.m_nCurrentDocumentRow);
		m_nCurrentCol = src.m_nCurrentCol;
		m_nDispCol    = src.m_nDispCol;
		m_nMaxCol     = src.m_nDispCol;

		CopyRowColorMap(m_mapRowBkColor, src.m_mapRowBkColor);
		CopyRowColorMap(m_mapRowColor, src.m_mapRowColor);

		SetSelMargin(src.m_bSelMargin);
		SetLineNumbers(src.m_bLineNumbers);
		CalculateEditbarLength();

		EnableWhiteSpace(src.m_bEnableWhiteSpace);
		SetTabWithSpace(src.m_bTabWithSpace);
		SetAutoIndent(src.m_bAutoIndent);
		SetSyntaxColor(src.m_bSyntaxColor);

		RecalcScrollBars();
		SetScrollPos(SB_HORZ, 0);
	}

	return *this;
}

CWnd * CXTPSyntaxEditCtrl::GetEffectiveParent() const
{
	CWnd *pScrollWnd = GetParent();

	if (GetParent())
	{
		pScrollWnd = GetParent();

		if (!IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
		{
			pScrollWnd = GetParent();
		}
	}
	else
	{
		pScrollWnd = const_cast<CXTPSyntaxEditCtrl *>(this);
	}

	return pScrollWnd;
}

void CXTPSyntaxEditCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_SELECTION_ID)
	{
		if ((::GetKeyState(VK_LBUTTON) & KF_UP) == 0)
		{
			m_bSelecting = m_bColumnSelect = FALSE;
			m_bSelectionStarted = FALSE;

			KillTimer(TIMER_SELECTION_ID);
			return;
		}

		int iPrvRow = GetCurrentDocumentRow();
		int iPrvCol = m_nDispCol;

		CXTPClientRect rcClient(this);

		int iMaxRow = (((int)((float)(rcClient.bottom - rcClient.top) / m_tm.tmHeight)) * m_tm.tmHeight);
		rcClient.bottom = rcClient.top + iMaxRow;

		CPoint ptCursor;
		GetCursorPos(&ptCursor);

		ScreenToClient(&ptCursor);

		int nNewTopRow = m_nTopRow;
		int nNewBottomRow = (nNewTopRow + rcClient.Height() / m_tm.tmHeight);

		int nRowToVisible = 0;
		//int nNewCol = 0;

		if (ptCursor.y < rcClient.top)
		{
			nNewTopRow -= 3;
			if (nNewTopRow <= 0)
				nNewTopRow = 1;

			nRowToVisible = nNewTopRow;
		}

		if (ptCursor.y > rcClient.bottom)
		{
			nNewBottomRow += 3;
			nRowToVisible = nNewBottomRow;
		}

		if (nRowToVisible > 0)
		{
			int nCurDocRow = GetCurrentDocumentRow();
			RowColFromPoint(ptCursor, &nCurDocRow, &m_nDispCol);
			SetCurrentDocumentRow((nRowToVisible > GetRowCount()) ? GetRowCount() : nRowToVisible);

			LPCTSTR szLineText = GetLineText(GetCurrentDocumentRow());
			int iMaxWidth = CalcMaximumWidth(szLineText);
			m_nMaxCol = CalcDispCol(szLineText, iMaxWidth);

			if (nNewBottomRow >= GetRowCount())
			{
				nNewBottomRow = GetRowCount();
			}

			if (m_bColumnSelect)
			{
				m_nCurrentCol = m_nMaxCol = m_nDispCol;
			}
			else
			{
				m_nDispCol = CalcValidDispCol(szLineText, m_nDispCol);

				if (m_nDispCol < 1)
					m_nDispCol = 1;

				m_nDispCol = (iMaxWidth > m_nDispCol) ? m_nDispCol : iMaxWidth;
			}

			int iEndRow = GetCurrentDocumentRow();

			CRect rcBookmark(0,0,m_nEditbarLength, rcClient.Height());

			if (m_nCurrentCol <= 1 && rcBookmark.PtInRect(ptCursor))
				iEndRow++;

			int iStartRow = m_ptStartSel.y;
			int iStartCol = m_ptStartSel.x;

			if (m_nCurrentCol <= 1 && m_bSelectingFromBookmark)
			{
				nCurDocRow = GetCurrentDocumentRow();
				if (nCurDocRow < m_nBookmarkSelStartRow)
				{
					iEndRow = nCurDocRow;
					iStartRow = m_nBookmarkSelStartRow+1;
				}
				else if (nCurDocRow > m_nBookmarkSelStartRow)
				{
					iEndRow = nCurDocRow + 1;
					iStartRow = m_nBookmarkSelStartRow;
				}
				else
				{
					iStartRow = nCurDocRow;
					iEndRow = nCurDocRow + 1;
				}
			}

			int iEndCol = m_nDispCol;

			m_bSelectionExist = TRUE;
			m_ptStartSel = CPoint(iStartCol, iStartRow);
			m_ptEndSel = CPoint(iEndCol, iEndRow);

			m_nCurrentCol = CalcAbsCol(szLineText, m_nDispCol);

			m_nDirtyRow = -1;
			m_nPrvDirtyRow = -1;

			if (GetCurrentDocumentRow() != iPrvRow || m_nDispCol != iPrvCol)
			{
				EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE);
				UpdateWindow();
				UpdateScrollPos(XTP_EDIT_UPDATE_HORZ|XTP_EDIT_UPDATE_VERT);
			}
		}
	}
	else if (nIDEvent == TIMER_REDRAW_WHEN_PARSE)
	{
		KillTimer(TIMER_REDRAW_WHEN_PARSE);

		SetDirtyRow(-1);
		Invalidate(FALSE);
		UpdateWindow();
	}

	CWnd::OnTimer(nIDEvent);
}

void CXTPSyntaxEditCtrl::ClearSelection()
{
	m_bSelectionExist = FALSE;
}

BOOL CXTPSyntaxEditCtrl::Select(int nRow1, int nCol1, int nRow2, int nCol2, BOOL bRedraw)
{
	if (nRow1 < 1 || nRow1 > GetRowCount())
		return FALSE;

	if (nRow2 < 1 || nRow2 > GetRowCount())
		return FALSE;

	int nAbsCol1, nAbsCol2;

	int nValidCol1 = nCol1;
	ValidateCol(nRow1, nValidCol1, nAbsCol1);

	int nValidCol2 = nCol2;
	ValidateCol(nRow2, nValidCol2, nAbsCol2);

	m_bSelectionExist = TRUE;

	m_ptStartSel.x = nValidCol1;
	m_ptStartSel.y = nRow1;

	m_ptEndSel.x = nValidCol2;
	m_ptEndSel.y = nRow2;

	if (nRow2 > nRow1)
	{
		SetCurrentDocumentRow(nRow2);
		m_nDispCol = nValidCol2;
		m_nCurrentCol = nAbsCol2;
	}
	else if (nRow2 < nRow1)
	{
		SetCurrentDocumentRow(nRow1);
		m_nDispCol = nValidCol1;
		m_nCurrentCol = nAbsCol1;
	}
	else
	{
		SetCurrentDocumentRow(nRow1);
		m_nDispCol = max(nValidCol1, nValidCol2);
		m_nCurrentCol = max(nAbsCol1, nAbsCol2);
	}

	if (bRedraw)
	{
		EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE);
		UpdateWindow();
		UpdateScrollPos(XTP_EDIT_UPDATE_HORZ);
	}

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::Find(LPCTSTR szText, BOOL bMatchWholeWord, BOOL bMatchCase, BOOL bSearchDown, BOOL bRedraw)
{
	// First check the current row
	CString strLineText;

	int iStartCol = m_nCurrentCol;
	int iStartRow = GetCurrentDocumentRow();
	int nTopRow = m_nTopRow;
	int nBottomRow = GetDocumentRow(GetRowPerPage());

	NormalizeSelection();

	if (m_bSelectionExist)
	{
		if (bSearchDown)
		{
			iStartCol = m_ptEndSel.x;
			iStartRow = m_ptEndSel.y;
		}
		else
		{
			iStartCol = m_ptStartSel.x;
			iStartRow = m_ptStartSel.y;
		}
	}

	m_pBuffer->GetLineText(iStartRow, strLineText);

	if (m_bSelectionExist)
		iStartCol = CalcAbsCol(strLineText, iStartCol);

	BOOL bResult = MatchText(iStartRow, strLineText, szText, iStartCol, bMatchWholeWord, bMatchCase, bSearchDown, bRedraw);

	if (bResult)
		return TRUE;

	if (bSearchDown)
	{
		for (int i = (iStartRow + 1); i <= GetRowCount(); i++)
		{
			m_pBuffer->GetLineText(i, strLineText);

			bResult = MatchText(i, strLineText, szText, 0, bMatchWholeWord, bMatchCase, TRUE, bRedraw);

			if (bResult)
				break;
		}
	}
	else
	{
		for (int i = (iStartRow - 1); i >= 1; i--)
		{
			m_pBuffer->GetLineText(i, strLineText);

			bResult = MatchText(i, strLineText, szText, (int)_tcsclen(strLineText) + 1, bMatchWholeWord, bMatchCase, FALSE, bRedraw);

			if (bResult)
				break;
		}
	}

	if (bResult)
	{
		if (m_ptStartSel.y > nBottomRow)
		{
			ShiftCurrentVisibleRowDown(2 * GetRowPerPage() / 3);
		}
		else if (m_ptStartSel.y < nTopRow)
		{
			ShiftCurrentVisibleRowUp(GetRowPerPage() / 3);
		}
	}

	return bResult;
}

BOOL CXTPSyntaxEditCtrl::MatchText(int nRow, LPCTSTR szLineText, LPCTSTR szMatchText, int nStartPos, BOOL bMatchWholeWord, BOOL bMatchCase, BOOL bSearchForward, BOOL bRedraw)
{
	BOOL bMatched = FALSE;

	const int nLen = (int)_tcsclen(szLineText);
	const int nSearchLen = (int)_tcsclen(szMatchText);

	// By Leva
	static LPCTSTR szSeps = _T("() \t<>{}:;,.=%\"'!@#$^&*-\\|[]/?");
//  static LPCTSTR szSeps = _T("() \t<>{}:;,.");

	if (bSearchForward)
	{
		//nStartPos++;
		for (int i = nStartPos; i <= (nLen - nSearchLen + 1); i++)
		{
			int nFound = -1;
			if (bMatchCase)
				nFound = _tcsncmp((szLineText + i - 1), szMatchText, nSearchLen);
			else
				nFound = _tcsnicmp((szLineText + i - 1), szMatchText, nSearchLen);

			bMatched = (nFound == 0);

			if (bMatchWholeWord && nFound == 0)
			{
				BOOL bSepAfter = FALSE;
				BOOL bSepBefore = FALSE;

				if ((i - 1 + nSearchLen) >= nLen)
				{
					bSepAfter = TRUE;
				}
				else
				{
					LPCTSTR pcszTextAfter = (szLineText + i - 1) + nSearchLen;
					bSepAfter = (_tcschr(szSeps, *pcszTextAfter) != 0);
				}

				if (i == 1)
					bSepBefore = TRUE;
				else
					bSepBefore = (_tcschr(szSeps, *(szLineText + i - 2)) != 0);

				if (!bSepAfter || !bSepBefore)
					bMatched = FALSE;
			}

			if (bMatched)
			{
				int nDispCol1 = CalcDispCol(szLineText, i);
				int nDispCol2 = CalcDispCol(szLineText, (i + nSearchLen));
				Select(nRow, nDispCol1, nRow, nDispCol2, bRedraw);

				m_nDispCol = nDispCol2;
				m_nCurrentCol = (i + nSearchLen);

				break;
			}
		}
	}
	else
	{
		for (int i = (nStartPos-nSearchLen); i >= 1 ; i--)
		{
			int nFound = -1;
			if (bMatchCase)
				nFound = _tcsncmp((szLineText + i - 1), szMatchText, nSearchLen);
			else
				nFound = _tcsnicmp((szLineText + i - 1), szMatchText, nSearchLen);

			bMatched = (nFound == 0);

			if (bMatchWholeWord && nFound == 0)
			{
				BOOL bSepAfter = FALSE;
				BOOL bSepBefore = FALSE;

				if ((i - 1 + nSearchLen) >= nLen)
				{
					bSepAfter = TRUE;
				}
				else
				{
					LPCTSTR pcszTextAfter = (szLineText + i - 1) + nSearchLen;
					bSepAfter = (_tcschr(szSeps, *pcszTextAfter) != 0);
				}

				if (i == 1)
					bSepBefore = TRUE;
				else
					bSepBefore = (_tcschr(szSeps, *(szLineText + i - 2)) != 0);

				if (!bSepAfter || !bSepBefore)
					bMatched = FALSE;
			}

			if (bMatched)
			{
				int nDispCol1 = CalcDispCol(szLineText, i);
				int nDispCol2 = CalcDispCol(szLineText, (i + nSearchLen));
				Select(nRow, nDispCol2, nRow, nDispCol1, bRedraw);
				m_nDispCol = nDispCol1;
				m_nCurrentCol = i;
				break;
			}
		}
	}

	return bMatched;
}

void CXTPSyntaxEditCtrl::ShowHScrollBar(BOOL bShow)
{
	ShowScrollBar(SB_HORZ, bShow);
}

void CXTPSyntaxEditCtrl::FindWord(UINT nFindWhat)
{
	int nRowToCalc = GetCurrentDocumentRow();

	CString strLineText;

	const TCHAR szSeps[]    = _T("[]{}()<>.,;:=#-\"\'\t/");
	const TCHAR szSpaceSeps[] = _T("\t ");

	if (nFindWhat == XTP_EDIT_FINDWORD_PREV)
	{
		BOOL bGoToEnd = FALSE;

		do
		{
			if (m_nCurrentCol == 1)
			{
				nRowToCalc--;
				bGoToEnd = TRUE;
			}

			if (nRowToCalc <= 0)
				return;

			m_pBuffer->GetLineText(nRowToCalc, strLineText);

			if (bGoToEnd)
				m_nCurrentCol = (int)_tcsclen(strLineText) + 1;

			while (m_nCurrentCol > 1 &&
				_tcschr(szSpaceSeps, strLineText.GetAt(m_nCurrentCol - 2)) != NULL)
			{
				m_nCurrentCol--;
			}
		} while (!bGoToEnd && m_nCurrentCol <= 1);

		BOOL bFound = FALSE;
		int i = 0;
		if (m_nCurrentCol > 1 && strLineText.GetLength() > 0)
		{
			BOOL bSeps = _tcschr(szSeps, strLineText.GetAt(m_nCurrentCol - 2)) != NULL;

			for (i = (m_nCurrentCol - 2); i >= 0; i--)
			{
				BOOL bSepFound = _tcschr(szSeps, strLineText.GetAt(i)) != NULL;
				BOOL bSpaceSepFound = _tcschr(szSpaceSeps, strLineText.GetAt(i)) != NULL;
				if (bSpaceSepFound ||
					(bSeps && !bSepFound) ||
					(!bSeps && bSepFound))
				{
					bFound = TRUE;
					break;
				}
			}
		}

		if (bFound)
			i++;

		m_nCurrentCol = (bFound) ? (i + 1) : 1;
		m_nDirtyRow = (!bFound) ? -1 : m_nDirtyRow;
		SetCurrentDocumentRow(nRowToCalc);
		m_nDispCol = CalcDispCol(strLineText, m_nCurrentCol);
		m_nMaxCol = m_nDispCol;
	}
	else
	{
		m_pBuffer->GetLineText(GetCurrentDocumentRow(), strLineText);
		BOOL bGoToFirst = FALSE;
		int nMaxCol = (int)_tcsclen(strLineText);

		if (m_nCurrentCol >= nMaxCol)
		{
			nRowToCalc++;
			bGoToFirst = TRUE;
		}

		if (nRowToCalc > GetRowCount())
			return;

		if (nRowToCalc != GetCurrentDocumentRow())
		{
			// get new line string and recalculate its length
			m_pBuffer->GetLineText(nRowToCalc, strLineText);
			nMaxCol = (int)_tcsclen(strLineText);
		}

		if (bGoToFirst)
			m_nCurrentCol = 1;
		else
		{
			BOOL bSeps = _tcschr(szSeps, strLineText.GetAt(m_nCurrentCol - 1)) != NULL;
			BOOL bFound = FALSE;

		int i;
		for (i = (m_nCurrentCol - 1); i < nMaxCol; i++)
		{
				BOOL bSepFound = _tcschr(szSeps, strLineText.GetAt(i)) != NULL;
				BOOL bSpaceSepFound = _tcschr(szSpaceSeps, strLineText.GetAt(i)) != NULL;
				if (bSpaceSepFound ||
					(bSeps && !bSepFound) ||
					(!bSeps && bSepFound))
			{
				bFound = TRUE;
				break;
			}
			}

		m_nCurrentCol = (bFound) ? (i + 1) : (nMaxCol + 1);
		m_nDirtyRow = (!bFound) ? -1 : m_nDirtyRow;
		}

		while (m_nCurrentCol < nMaxCol &&
			_tcschr(szSpaceSeps, strLineText.GetAt(m_nCurrentCol - 1)) != NULL)
		{
			m_nCurrentCol++;
		}

		SetCurrentDocumentRow(nRowToCalc);
		m_nDispCol = CalcDispCol(strLineText, m_nCurrentCol);
		m_nMaxCol = m_nDispCol;
	}
}

void CXTPSyntaxEditCtrl::DeleteSelectedLines(int iForceDeleteRow)
{
	int iLineFrom, iLineTo;
	int iColTo = 1;

	NormalizeSelection();

	if (m_bSelectionExist)
	{
		iLineFrom = m_ptStartSel.y;
		iLineTo = m_ptEndSel.y + 1;
	}
	else
	{
		iLineFrom = iForceDeleteRow;
		iLineTo = iLineFrom + 1;
	}

	if (iLineTo > GetRowCount())
	{
		iLineTo = GetRowCount();

		const int iLen = (int)_tcsclen(GetLineText(iLineTo));

		if (iLen == 0 && (iLineTo < 1 || iLineTo < iLineFrom))
			iColTo = 1;
		else
			iColTo = iLen+1;

		if (iLineFrom == iLineTo && iColTo == 1 && iLen == 0)   // Nothing to delete
			return;
	}

	m_bSelectionExist = FALSE;

	DeleteBuffer(iLineFrom, 1, iLineTo, iColTo);
}

BOOL CXTPSyntaxEditCtrl::IncreaseIndent()
{
	if (!m_bSelectionExist || (m_ptStartSel.y == m_ptEndSel.y && m_ptStartSel.y < GetRowCount()))
	{
		if (!m_pBuffer->GetOverwriteFlag())
			return FALSE;

		m_bSelectionExist = FALSE;

		LPCTSTR szLineText = GetLineText(GetCurrentDocumentRow());
		const int iLineLen = (int)_tcsclen(szLineText);

		int iMaxDispCol = CalcDispCol(szLineText, (iLineLen + 1));

		if (m_nDispCol >= iMaxDispCol)
			return FALSE;

		if (m_nCurrentCol > (iLineLen + 1))
			m_nCurrentCol = (iLineLen + 1);

		m_nDispCol += (GetTabSize() - (m_nDispCol % GetTabSize()));     // Align by tab size

		if (m_nDispCol < 1)
			m_nDispCol = 1;

		m_nDispCol = CalcValidDispCol(szLineText, m_nDispCol);
		m_nCurrentCol = CalcAbsCol(szLineText, m_nDispCol);

		m_nMaxCol = m_nDispCol;

		m_nDirtyRow = GetCurrentDocumentRow();
		EnsureVisible(m_nDirtyRow, m_nDispCol, TRUE);

		return TRUE;
	}

	int iRowFrom = min(m_ptStartSel.y, m_ptEndSel.y);
	int iRowTo = max(m_ptStartSel.y, m_ptEndSel.y);

	if (m_ptEndSel.x == 1 && iRowTo > iRowFrom)
		iRowTo--;

	if (!CanEditDoc())
		return FALSE;

	BOOL bModified = FALSE;

	// First insert tabs in front
	// If overwrite flag is not set
	if (!m_pBuffer->GetOverwriteFlag())
	{
		const CString strText(GetTabWithSpace() ?
			CString((TCHAR)(' '), GetTabSize()) : _T("\x09"));

		for (int i = iRowFrom; i <= iRowTo; i++)
		{
			//**----------------------
			OnBeforeEditChanged(i, 1);
			//**----------------------

			m_pBuffer->InsertText(strText, i, 1);

			bModified = TRUE;

			if (i > iRowFrom)
			{
				m_pBuffer->GetUndoRedoManager()->ChainLastCommand();
				m_pBuffer->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_INDENTATION);
			}

			//**----------------------
			OnEditChanged(i, 1, i, 2, xtpEditActInsert);
			//**----------------------
		}

		if (bModified)
		{
			NotifyEditChanged(iRowFrom, iRowTo, XTP_EDIT_EDITACTION_MODIFYROW);
		}
	}

	// Select the lines

	// By Leva
	if (m_ptStartSel.y <= m_ptEndSel.y) // Top to bottom selection
	{
		m_ptStartSel.y = iRowFrom;
		m_ptStartSel.x = 1;

		if ((m_ptEndSel.x > 1 && m_ptEndSel.y < GetRowCount()) || m_ptEndSel.y == GetRowCount())
		{
			if (iRowTo < GetRowCount())
			{
				m_ptEndSel.y = iRowTo + 1;
				m_ptEndSel.x = 1;

				SetCurrentDocumentRow(m_ptEndSel.y);
				m_nCurrentCol = m_nDispCol = m_nMaxCol = 1;
			}
			else
			{
				LPCTSTR szLineText = GetLineText(iRowTo);
				m_nCurrentCol = (int)_tcsclen(szLineText) + 1;

				SetCurrentDocumentRow(iRowTo);
				m_nDispCol = m_nMaxCol = CalcDispCol(szLineText, m_nCurrentCol);

				m_ptEndSel.y = iRowTo;
				m_ptEndSel.x = m_nDispCol;
			}
		}
	}

	m_nDirtyRow = -1;

	if (bModified)
		SetDocModified();

	EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE);
	UpdateWindow();
	UpdateScrollPos();

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::DecreaseIndent()
{
	if (!m_bSelectionExist || m_ptStartSel.y == m_ptEndSel.y)
	{
		m_bSelectionExist = FALSE;

		if (m_nCurrentCol < 1)
			m_nCurrentCol = 1;

		LPCTSTR szLineText = GetLineText(GetCurrentDocumentRow());

		int iAddFactor = 1;

		if ((m_nDispCol - 1) % GetTabSize())
			iAddFactor = 0;

		m_nDispCol = 1 + ((((m_nDispCol - 1) / GetTabSize()) - iAddFactor) * GetTabSize());     // Align by tab size

		if (m_nDispCol < 1)
			m_nDispCol = 1;

		m_nDispCol = CalcValidDispCol(szLineText, m_nDispCol);
		m_nCurrentCol = CalcAbsCol(szLineText, m_nDispCol);

		m_nMaxCol = m_nDispCol;

		m_nDirtyRow = GetCurrentDocumentRow();
		EnsureVisible(m_nDirtyRow, m_nDispCol, TRUE);

		return TRUE;
	}

	int iRowFrom = min(m_ptStartSel.y, m_ptEndSel.y);
	int iRowTo = max(m_ptStartSel.y, m_ptEndSel.y);

	if (m_ptEndSel.x == 1)
		iRowTo--;

	if (!CanEditDoc())
		return FALSE;

	CString strLineText;

	BOOL bFrontDeleted = FALSE;
	BOOL bModified = FALSE;

	// First tabs or blank spaces from front
	for (int i = iRowFrom; i <= iRowTo; i++)
	{
		//**----------------------
		OnBeforeEditChanged(i, 1);
		//**----------------------

		GetLineText(i, strLineText);
		const int iLen = (int)_tcsclen(strLineText);

		if (strLineText.GetAt(0) == VK_TAB)
		{
			m_pBuffer->DeleteText(i, 1, i, 2);
			bFrontDeleted = TRUE;
			bModified = TRUE;
		}
		else
		{
			// No tab found in the front
			// try to find blank spaces
			int iSpaceCount = 0;
			for (int iCol = 0; iCol < GetTabSize() && iCol < iLen; iCol++)
			{
				if (strLineText.GetAt(iCol) == 0x20)
					iSpaceCount++;
				else
					break;
			}
			if (iSpaceCount)
			{
				m_pBuffer->DeleteText(i, 1, i, (iSpaceCount + 1));
				bFrontDeleted = TRUE;
				bModified = TRUE;
			}
		}

		if (bFrontDeleted)
		{
			m_pBuffer->GetUndoRedoManager()->ChainLastCommand();
			m_pBuffer->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_INDENT_DECREASE);
		}

		//**----------------------
		OnEditChanged(i, 1, i, 1, xtpEditActDelete);
		//**----------------------
	}

	if (bModified)
	{
		NotifyEditChanged(iRowFrom, iRowTo, XTP_EDIT_EDITACTION_MODIFYROW);
	}

	// Select the lines

	// By Leva
	if (m_ptStartSel.y <= m_ptEndSel.y) // Top to bottom selection
	{
		m_ptStartSel.y = iRowFrom;
		m_ptStartSel.x = 1;

		if (m_ptEndSel.x > 1)
		{
			if (iRowTo < GetRowCount())
			{
				m_ptEndSel.y = iRowTo + 1;
				m_ptEndSel.x = 1;

				SetCurrentDocumentRow(m_ptEndSel.y);
				m_nCurrentCol = m_nDispCol = m_nMaxCol = 1;
			}
			else
			{
				GetLineText(iRowTo, strLineText);

				m_nCurrentCol = (int)_tcsclen(strLineText);
				SetCurrentDocumentRow(iRowTo);
				m_nDispCol = m_nMaxCol = CalcDispCol(strLineText, m_nCurrentCol);

				m_ptEndSel.y = iRowTo;
				m_ptEndSel.x = m_nDispCol;
			}
		}
	}

	m_nDirtyRow = -1;

	if (bModified)
		SetDocModified();

	EnsureVisible(GetCurrentDocumentRow(), m_nDispCol, TRUE);
	UpdateWindow();
	UpdateScrollPos();

	return TRUE;
}

void CXTPSyntaxEditCtrl::NormalizeSelection()
{
	if (!m_bSelectionExist)
		return;

	int iStartRow, iStartCol, iEndRow, iEndCol;

	if (m_ptStartSel.y < m_ptEndSel.y)
	{
		iStartRow = m_ptStartSel.y;
		iStartCol = m_ptStartSel.x;
		iEndRow = m_ptEndSel.y;
		iEndCol = m_ptEndSel.x;
	}
	else if (m_ptStartSel.y == m_ptEndSel.y)
	{
		iStartRow = m_ptStartSel.y;
		iStartCol = min(m_ptStartSel.x, m_ptEndSel.x);
		iEndRow = m_ptEndSel.y;
		iEndCol = max(m_ptStartSel.x, m_ptEndSel.x);
	}
	else
	{
		iStartRow = m_ptEndSel.y;
		iStartCol = m_ptEndSel.x;
		iEndRow = m_ptStartSel.y;
		iEndCol = m_ptStartSel.x;
	}

	m_ptStartSel = CPoint(iStartCol, iStartRow);
	m_ptEndSel = CPoint(iEndCol, iEndRow);
}

void CXTPSyntaxEditCtrl::RecalcVertScrollPos()
{
	if (!::IsWindow(m_hWnd))
		return;

	if (!XTPSyntaxEditPaintManager()->GetFont()->GetSafeHandle())
		return;

	int nPageRows = GetRowPerPage();
	int nTotalVisRows = GetVisibleRowsCount();
	int nVisRow = GetVisibleRowsCount(m_nTopRow);

	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin = 1;
	si.nMax = nTotalVisRows;
	si.nPage = nPageRows;
	si.nPos = nVisRow;

	BOOL bVertScrl = (nTotalVisRows >= nPageRows);

	if (bVertScrl)
	{
		VERIFY(SetScrollInfo(SB_VERT, &si));
	}

	EnableScrollBarCtrl(SB_VERT, bVertScrl);
}

void CXTPSyntaxEditCtrl::RecalcHorzScrollPos()
{
	if (!::IsWindow(m_hWnd))
		return;

	if (!XTPSyntaxEditPaintManager()->GetFont()->GetSafeHandle())
		return;

	if (!m_tm.tmAveCharWidth)
		return;

	CXTPClientRect rc(this);

	int iMaxChar = (rc.Width() - m_nEditbarLength) / m_tm.tmAveCharWidth;

	int nPos = GetScrollPos(SB_HORZ);

	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin = 0;
	si.nMax = m_pBuffer->GetMaxLineTextLength();
	si.nPage = iMaxChar;
	si.nPos = nPos;

	if (GetHorzScrollBar())
		VERIFY(SetScrollInfo(SB_HORZ, &si, FALSE));
}

void CXTPSyntaxEditCtrl::NotifyCurRowCol(int iRow, int iCol)
{
	// Notify the parent class that position has been changed
	XTP_EDIT_NMHDR_ROWCOLCHANGED src;

	// NMHDR codes
	src.nmhdr.code = XTP_EDIT_NM_ROWCOLCHANGED;
	src.nmhdr.hwndFrom = m_hWnd;
	src.nmhdr.idFrom = GetDlgCtrlID();

	// Row col specific codes
	src.nRow = iRow;
	src.nCol = iCol;

	// Notify the parent window
	if (::IsWindow(m_pParentWnd->GetSafeHwnd()))
	{
		m_pParentWnd->SendMessage(
			WM_NOTIFY, (WPARAM)src.nmhdr.idFrom, (LPARAM)&src);
	}
}

void CXTPSyntaxEditCtrl::ValidateCol(const int nRow, int &nCol, int& nAbsCol)
{
	CString strText;

	GetLineText(nRow, strText);

	nCol = CalcValidDispCol(strText, nCol);

	nAbsCol = CalcAbsCol(strText, nCol);
}

void CXTPSyntaxEditCtrl::EnableOleDrag(BOOL bEnableDrag)
{
	m_bEnableOleDrag = bEnableDrag;
}

HGLOBAL CXTPSyntaxEditCtrl::GetSelectionBuffer(UINT nFormat)
{
	if (!m_bSelectionExist)
		return 0;

	if (nFormat != CF_TEXT && nFormat != CF_UNICODETEXT)
		return 0;

	CMemFile file(CalcAveDataSize(m_ptStartSel.y, m_ptEndSel.y));
	//m_pBuffer->GetBuffer(m_ptStartSel.y, iCol1, m_ptEndSel.y, iCol2,
	m_pBuffer->GetBuffer(m_ptStartSel.y, m_ptStartSel.x, m_ptEndSel.y, m_ptEndSel.x,
						 file, m_bColumnSelect, TRUE);

	file.Seek(0, CFile::end);
	file.Write(_T("\0"), sizeof(TCHAR));
	int nLen = (int)file.GetLength();
	BYTE *pBytes = file.Detach();

#ifdef _UNICODE
	// If Unicode defined then for CF_TEXT conversion is needed
	if (nFormat == CF_TEXT)
	{
		nLen = ::WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pBytes, -1, NULL, 0, NULL, NULL);

	}
#else
	if (nFormat == CF_UNICODETEXT)
	{
		nLen = ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pBytes, -1, NULL, 0);
		nLen *= sizeof(wchar_t);
	}
#endif

	HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, nLen);

	if (!hMem)
		return 0;

	void *pText = ::GlobalLock(hMem);

#ifdef _UNICODE
	if (nFormat == CF_UNICODETEXT)
		MEMCPY_S(pText, pBytes, nLen);
	else
	{
		::WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pBytes, -1, (LPSTR)pText, nLen, NULL, NULL);
	}
#else
	if (nFormat == CF_TEXT)
		MEMCPY_S(pText, pBytes, nLen);
	else
	{
		::MultiByteToWideChar(CP_ACP, 0, (LPSTR)pBytes, -1, (LPWSTR)pText, (nLen / sizeof(wchar_t)));
	}
#endif

	::GlobalUnlock(hMem);

	::free(pBytes);

	return hMem;
}

int CXTPSyntaxEditCtrl::GetSelectionText(CString &strText)
{
	if (!m_bSelectionExist)
		return 0;

	CMemFile file(CalcAveDataSize(m_ptStartSel.y, m_ptEndSel.y));
	//m_pBuffer->GetBuffer(m_ptStartSel.y, iCol1, m_ptEndSel.y, iCol2,
	m_pBuffer->GetBuffer(m_ptStartSel.y, m_ptStartSel.x, m_ptEndSel.y, m_ptEndSel.x,
						 file, m_bColumnSelect, TRUE);

	file.Write((LPVOID)_T("\0"), sizeof(TCHAR));

	int iLen = (int)file.GetLength();

	BYTE *pBytes = file.Detach();

	strText = (LPTSTR)pBytes;

	free(pBytes);

	return iLen;
}

void CXTPSyntaxEditCtrl::SetDropPos(int iRow, int iCol)
{
	m_ptDropPos.y = iRow;
	m_ptDropPos.x = iCol;
}

void CXTPSyntaxEditCtrl::Unselect()
{
	if (!m_bSelectionExist)
		return;

	m_bSelectionExist = FALSE;
	m_nDirtyRow = -1;
	Invalidate(FALSE);
}

BOOL CXTPSyntaxEditCtrl::ReplaceSel(LPCTSTR szNewText, BOOL bRedraw)
{
	if (!m_bSelectionExist)
		return FALSE;

	NormalizeSelection();

	SetCurrentDocumentRow(m_ptStartSel.y);
	m_nDispCol = m_ptStartSel.x;
	m_nMaxCol = m_nDispCol;

	m_nCurrentCol = CalcAbsCol(GetLineText(m_ptStartSel.y), m_nDispCol);

	BOOL bOverwriteMode = m_pBuffer->GetOverwriteFlag();
	m_pBuffer->SetOverwriteFlag(FALSE);

	InsertString(szNewText, GetCurrentDocumentRow(), m_nCurrentCol, TRUE, bRedraw);

	m_pBuffer->SetOverwriteFlag(bOverwriteMode);

	return TRUE;
}

void CXTPSyntaxEditCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);

	if (!bShow)
		m_nDirtyRow = -1;
}

void CXTPSyntaxEditCtrl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE)
		m_nDirtyRow = -1;

	SetActive(nState != WA_INACTIVE);
}

void CXTPSyntaxEditCtrl::SetActive(BOOL bIsActive)
{
	m_bIsActive = bIsActive;

	if (::IsWindow(m_hWnd))
	{
		RedrawWindow();
	}
}

void CXTPSyntaxEditCtrl::HandleDrop(BOOL bCopy)
{
	if (!CanEditDoc())
		return;

	if (!m_bRightButtonDrag)
		CopyOrMoveText(bCopy);
	else
	{
		CMenu menu;
		menu.CreatePopupMenu();

		CString csMenuText;

		XTPResourceManager()->LoadString(&csMenuText, XTP_IDS_EDIT_MOVE_HERE);
		menu.AppendMenu(MF_STRING, XTP_IDC_EDIT_DRAG_MOVE, csMenuText);

		XTPResourceManager()->LoadString(&csMenuText, XTP_IDS_EDIT_COPY_HERE);
		menu.AppendMenu(MF_STRING, XTP_IDC_EDIT_DRAG_COPY, csMenuText);

		menu.AppendMenu(MF_SEPARATOR);

		XTPResourceManager()->LoadString(&csMenuText, XTP_IDS_EDIT_CANCEL);
		menu.AppendMenu(MF_STRING, IDCANCEL, csMenuText);

		CPoint pt;
		GetCursorPos(&pt);

		int nID = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, pt.x, pt.y, this);

		if (nID == XTP_IDC_EDIT_DRAG_MOVE || nID == XTP_IDC_EDIT_DRAG_COPY)
			CopyOrMoveText(nID == XTP_IDC_EDIT_DRAG_COPY);
	}

	m_bRightButtonDrag = FALSE;
	m_bDragging = FALSE;
}

void CXTPSyntaxEditCtrl::SetPrintMargins(int iTop, int iLeft, int iBottom, int iRight)
{
	m_nTopMargin = iTop;
	m_nBottomMargin = iBottom;
	m_nRightMargin = iRight;
	m_nLeftMargin = iLeft;
}

CSize CXTPSyntaxEditCtrl::GetEffectivePageSize()
{
	CSize szPage = m_szPage;
	szPage.cx -= (m_nRightMargin + m_nLeftMargin);
	szPage.cy -= (m_nTopMargin + m_nBottomMargin);

	return szPage;
}

void CXTPSyntaxEditCtrl::ShowDefaultContextMenu()
{
	CPoint ptCur;
	GetCursorPos(&ptCur);
	OnContextMenu(this, ptCur);
}

void CXTPSyntaxEditCtrl::CancelRightButtonDrag()
{
	m_bRightButtonDrag = FALSE;
}

BOOL CXTPSyntaxEditCtrl::IsRightButtonDrag()
{
	return m_bRightButtonDrag;
}

void CXTPSyntaxEditCtrl::SetRightButtonDrag(BOOL bRightButtonDrag)
{
	m_bRightButtonDrag = bRightButtonDrag;
}

void CXTPSyntaxEditCtrl::EnableWhiteSpace(BOOL bShow)
{
	m_bEnableWhiteSpace = bShow;
	m_nDirtyRow = -1;
	Invalidate(FALSE);
	//UpdateWindow();
}

BOOL CXTPSyntaxEditCtrl::IsEnabledWhiteSpace()
{
	return m_bEnableWhiteSpace;
}

BOOL CXTPSyntaxEditCtrl::SetTabWithSpace(BOOL bTabWithSpace, BOOL bUpdateReg/*=FALSE*/)
{
	if (!SetValueBool(XTP_EDIT_REG_TABWITHSPACE, bTabWithSpace, m_bTabWithSpace, bUpdateReg))
		return FALSE;

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::GetTabWithSpace() const
{
	return m_bTabWithSpace;
}

void CXTPSyntaxEditCtrl::NotifySelInit()
{
	NotifyParent(XTP_EDIT_NM_SELINIT);
}

BOOL CXTPSyntaxEditCtrl::SetTabSize(int nTabSize, BOOL bUpdateReg/*=FALSE*/)
{
	if (!m_pBuffer->SetTabSize(nTabSize, bUpdateReg))
		return FALSE;

	CString strText;
	GetLineText(GetCurrentDocumentRow(), strText);

	m_nDispCol = CalcDispCol(strText, m_nCurrentCol);
	m_nMaxCol = CalcValidDispCol(strText, m_nMaxCol);

	SetCurCaretPos(GetCurrentDocumentRow(), m_nDispCol, FALSE);

	return TRUE;
}

int CXTPSyntaxEditCtrl::GetTabSize() const
{
	return m_pBuffer->GetTabSize();
}

BOOL CXTPSyntaxEditCtrl::SetSyntaxColor(BOOL bSyntaxColor, BOOL bUpdateReg/*=FALSE*/)
{
	if (!SetValueBool(XTP_EDIT_REG_SYNTAXCOLOR, bSyntaxColor, m_bSyntaxColor, bUpdateReg))
		return FALSE;

	m_arOnScreenSchCache.RemoveAll();
	m_pBuffer->EnableParser(bSyntaxColor);

	return TRUE;
}

LRESULT CXTPSyntaxEditCtrl::NotifyParent(UINT uCode)
{
	ASSERT_VALID(m_pParentWnd);

	if (::IsWindow(m_pParentWnd->GetSafeHwnd()))
	{
		NMHDR hdr;
		hdr.hwndFrom = m_hWnd;
		hdr.code = uCode;
		hdr.idFrom = GetDlgCtrlID();

		return m_pParentWnd->SendMessage(
			WM_NOTIFY, (WPARAM)hdr.idFrom, (LPARAM)&hdr);
	}

	return -1;
}

BOOL CXTPSyntaxEditCtrl::SetSelMargin(BOOL bSelMargin, BOOL bUpdateReg/*=FALSE*/)
{
	if (!SetValueBool(XTP_EDIT_REG_SELMARGIN, bSelMargin, m_bSelMargin, bUpdateReg))
		return FALSE;

	CalculateEditbarLength();

	m_nDirtyRow = -1;
	SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, FALSE);
	//UpdateWindow();
	Invalidate();

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::SetScrollBars(BOOL bHorzSBar, BOOL bVertSBar, BOOL bUpdateReg/*=FALSE*/)
{
	if (!SetValueBool(XTP_EDIT_REG_HSCROLLBAR, bHorzSBar, m_bHorzScrollBar, bUpdateReg))
		return FALSE;

	if (!SetValueBool(XTP_EDIT_REG_VSCROLLBAR, bVertSBar, m_bVertScrollBar, bUpdateReg))
		return FALSE;

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE) &
		~(WS_HSCROLL|WS_VSCROLL);

	if (bHorzSBar)
		dwStyle |= WS_HSCROLL;

	if (bVertSBar)
		dwStyle |= WS_VSCROLL;

	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::CanEditDoc()
{
	BOOL bCancel = (BOOL)NotifyParent(XTP_EDIT_NM_EDITCHANGING);
	return !bCancel;

	//BOOL ulResult = (BOOL)::GetWindowLongPtr(m_hWnd, GWLP_USERDATA);

	//if (!ulResult)
	//  return FALSE;

	//return TRUE;
}

void CXTPSyntaxEditCtrl::NotifyEditChanged(int iRowFrom, int iRowTo, UINT nActions)
{
	XTP_EDIT_NMHDR_EDITCHANGED sec;

	sec.nmhdr.code = XTP_EDIT_NM_EDITCHANGED;
	sec.nmhdr.hwndFrom = m_hWnd;
	sec.nmhdr.idFrom = GetDlgCtrlID();
	sec.nRowFrom = iRowFrom;
	sec.nRowTo = iRowTo;
	sec.nAction = nActions;

	RefreshLineMarks(&sec);

	// Notify the parent window
	if (::IsWindow(m_pParentWnd->GetSafeHwnd()))
	{
		m_pParentWnd->SendMessage(
			WM_NOTIFY, (WPARAM)sec.nmhdr.idFrom, (LPARAM)&sec);
	}
}

BOOL CXTPSyntaxEditCtrl::CreateInsertText(LPTSTR szText,
								  CString& strTextToIns,
								  int& iNewRow,
								  int& iNewCol,
								  int& iNewDispCol,
								  int& iNewMaxCol,
								  int& iEditRowFrom,
								  int& iEditRowTo,
								  int& iChainActionCount)
{
	// As this function will always be called by OnChar
	// We can assume szText will contain only one character
	// or the CRLF

	CString strLineText;
	GetLineText(GetCurrentDocumentRow(), strLineText);

	const int iLen = (int)_tcsclen(strLineText);
	int iMaxCol = iLen + 1;

	BOOL bTextCRLF = m_pBuffer->IsTextCRLF(szText);

	BOOL bProcess = TRUE;

	if (!bTextCRLF)
	{
		// Normal chars with auto indent goes here
		int iValidDispCol = CalcDispCol(strLineText, m_nCurrentCol);
		int iValidAbsCol  = CalcAbsCol(strLineText, iValidDispCol);

		int iMaxDispCol = iValidDispCol;
		iMaxCol = iValidAbsCol;

		m_nCurrentCol = iValidAbsCol;

		if (GetAutoIndent() && m_nAutoIndentCol > 0 && iValidDispCol < m_nAutoIndentCol)
		{
			iMaxDispCol = CalcDispCol(strLineText, iMaxCol);
			FillTabs(iNewRow, FALSE, iMaxDispCol, iMaxCol, strTextToIns, FALSE);
			iMaxCol--;
		}

		m_nAutoIndentCol = 0;

		int iAddChar = 1;
		int iAddAbsCol = 1;

		if (szText[0] == _T('\t'))
			iAddChar = (GetTabSize() - ((iMaxDispCol - 1) % GetTabSize()));

		if (GetTabWithSpace() && szText[0] == 0x09)
		{
			for (int i = 0; i < iAddChar; i++)
				strTextToIns += _T(" ");   // Blank

			iAddAbsCol = iAddChar;
		}
		else
		{
			strTextToIns += szText;
		}

		iNewRow = GetCurrentDocumentRow();
		iNewDispCol = iMaxDispCol + iAddChar;
		iNewCol = iMaxCol + iAddAbsCol;
		iNewMaxCol = iNewDispCol;

		if (!GetTabWithSpace() && szText[0] == _T('\t'))
		{
			int iBlanks;
			for (iBlanks = (m_nCurrentCol - 2); iBlanks >= 0; iBlanks--)
			{
				if (strLineText.GetAt(iBlanks) != _T(' '))
					break;
			}

			iBlanks += 2;

			if (iBlanks < m_nCurrentCol)
			{
				int iNoTabs = 0;
				int iStartDispCol = m_nDispCol - (m_nCurrentCol - iBlanks);

				for (int iCol = iBlanks; iCol < m_nCurrentCol; iCol++)
				{
					if ((iStartDispCol % GetTabSize()) == 0)
						iNoTabs++;
					else
						iNewCol--;

					iStartDispCol++;
				}

				m_pBuffer->SetOverwriteFlag(FALSE);

				int nCurDocRow = GetCurrentDocumentRow();
				m_pBuffer->DeleteText(nCurDocRow, iBlanks, nCurDocRow, m_nCurrentCol);

				m_pBuffer->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_FORMAT);

				CString strTabs;

				for (int i = 0; i < iNoTabs; i++)
					strTabs += _T('\t');

				if (!strTabs.IsEmpty())
					strTextToIns = strTabs + strTextToIns;

				m_nCurrentCol = iBlanks;
				iChainActionCount = 2;
			}
		}

		iEditRowFrom = iEditRowTo = GetCurrentDocumentRow();
	}
	else if (bTextCRLF)
	{
		// Enter pressed
		int nCurDocRow = GetCurrentDocumentRow();

		if (m_pBuffer->GetOverwriteFlag() && nCurDocRow == GetRowCount() && (m_nCurrentCol < iMaxCol))
		{
			return FALSE;
		}

		if (m_pBuffer->GetOverwriteFlag() &&
			((nCurDocRow < GetRowCount() && (m_nCurrentCol <= iMaxCol)) ||
			(nCurDocRow == GetRowCount() && (m_nCurrentCol < iMaxCol))))
		{
			bProcess = FALSE;
		}

		iNewRow = nCurDocRow + 1;
		iNewCol = iNewDispCol = iNewMaxCol = 1;

		iEditRowFrom = nCurDocRow;
		if (m_nCurrentCol >= iMaxCol)
			iEditRowFrom++;

		iEditRowTo = iEditRowFrom + 1;
		iNewMaxCol = iNewDispCol;

		strTextToIns = szText;

		if (!GetAutoIndent())
			return TRUE;

		// If enter pressed at the position where no chars found except TAB and SPACE at the left
		// then go for checking the previous lines for indentation

		BOOL bInsertAtBegining = !m_pBuffer->GetOverwriteFlag();
		BOOL bEmptyLine = TRUE;

		int i;
		for (i = 0; i < iLen; i++)
		{
			if (strLineText.GetAt(i) != 0x20 && strLineText.GetAt(i) != 0x09) // SPACE && TAB
			{
				bEmptyLine = FALSE;
				break;
			}
		}

		for (i = (m_nCurrentCol - 2); i >= 0; i--)
		{
			if (strLineText.GetAt(i) != 0x20 && strLineText.GetAt(i) != 0x09)       // SPACE && TAB
			{
				bInsertAtBegining = FALSE;
				break;
			}
		}

		m_nAutoIndentCol = 0;

		if (bInsertAtBegining || bEmptyLine)
		{
			CString strTempLineText;
			BOOL bFound = FALSE;

			for (int iRow = (GetCurrentDocumentRow() - 1); iRow >= 1; iRow--)
			{
				GetLineText(iRow, strTempLineText);
				const int iTempLineLen = (int)_tcsclen(strTempLineText);

				m_nAutoIndentCol = 0;

				for (i = 0; i < iTempLineLen; i++)
				{
					if (strTempLineText.GetAt(i) == 0x09)
						m_nAutoIndentCol += (GetTabSize() - (m_nAutoIndentCol % GetTabSize()));
					else if (strTempLineText.GetAt(i) == 0x20)
						m_nAutoIndentCol++;
					else
					{
						bFound = TRUE;
						break;
					}
				}

				if (bFound)
					break;
			}

			if (!bFound)
				m_nAutoIndentCol = 0;
			else
			{
				// Find out the starting char pos
				int iStartCharPos = 0;
				for (i = 0; i < iLen; i++)
				{
					if (strLineText.GetAt(i) != 0x09 && strLineText.GetAt(i) != 0x20)
						break;

					iStartCharPos++;
				}

				if (!m_pBuffer->GetOverwriteFlag())
				{
					if (iStartCharPos+1 > 1 && iStartCharPos+1 < iMaxCol)
					{
						nCurDocRow = GetCurrentDocumentRow();
						DeleteBuffer(nCurDocRow, 1, nCurDocRow, iStartCharPos + 1);
						if (iChainActionCount > 1)
							m_pBuffer->GetUndoRedoManager()->ChainLastCommand();

						iChainActionCount = 2;
					}
				}
			}
		}
		else
		{
			//int iStartChar = 0;

			for (i = 0; i < iLen; i++)
			{
				if (strLineText.GetAt(i) == 0x09)
					m_nAutoIndentCol += (GetTabSize() - (m_nAutoIndentCol % GetTabSize()));
				else if (strLineText.GetAt(i) == 0x20)
					m_nAutoIndentCol++;
				else
					break;
			}
		}

		if (m_nAutoIndentCol > 0)
			m_nAutoIndentCol++;

		BOOL bInsertMid = (m_nCurrentCol < iMaxCol);

		if (bInsertMid || bInsertAtBegining)
		{
			int iMaxDispCol = 1;
			int iMaxCol = 0;

			BOOL bOverwriteFlag = m_pBuffer->GetOverwriteFlag();

			if (bEmptyLine)
				bOverwriteFlag = TRUE;

			FillTabs(iNewRow, FALSE, iMaxDispCol, iMaxCol, strTextToIns, bOverwriteFlag);

			if (iMaxCol == 0)
				iMaxCol = 1;

			iNewDispCol = iMaxDispCol;
			iNewCol = iMaxCol;
			iNewMaxCol = iNewDispCol;
		}
		else
		{
			int iMaxDispCol = 1;
			iMaxCol = 0;

			FillTabs(iNewRow, TRUE, iMaxDispCol, iMaxCol, strTextToIns, TRUE);  // Just to calculate tab and spaces

			if (iMaxCol == 0)
				iMaxCol = 1;

			iNewDispCol = iMaxDispCol;
			iNewCol = iMaxCol;
			iNewMaxCol = iNewDispCol;
		}
	}

	if (m_nAutoIndentCol)
		iNewMaxCol = m_nAutoIndentCol;
	else
		iNewMaxCol = iNewDispCol;

	return bProcess;
}

void CXTPSyntaxEditCtrl::FillTabs(int iNewRow, BOOL bInsertAtEnd, int& iMaxDispCol, int& iMaxCol, CString &strTextToIns, BOOL bOverwrite)
{
	m_nInsertSpaceCount = m_nInsertTabCount = 0;

	iMaxDispCol--;

	while ((iMaxDispCol + 1) < m_nAutoIndentCol)
	{
		int iNextTabbedDispCol = iMaxDispCol + (GetTabSize() - (iMaxDispCol % GetTabSize()));

		if ((iNextTabbedDispCol + 1) > m_nAutoIndentCol || GetTabWithSpace())
		{
			int iBlankNeeded = m_nAutoIndentCol - iMaxDispCol;

			for (int i = 0; i < (iBlankNeeded - 1); i++)
			{
				if (!bOverwrite)
					strTextToIns += _T(" ");

				iMaxDispCol++;
				iMaxCol++;
				m_nInsertSpaceCount++;
			}
		}
		else
		{
			if (!bOverwrite)
				strTextToIns += _T("\t");

			iMaxDispCol = iNextTabbedDispCol;
			iMaxCol++;
			m_nInsertTabCount++;
		}
	}

	iMaxDispCol++;
	iMaxCol++;

	if (!bOverwrite)
	{
		if (bInsertAtEnd)
			iMaxDispCol = iMaxCol = 1;
		else
			m_nAutoIndentCol = 0;
	}
	else
	{
		if (iNewRow <= GetRowCount() && m_pBuffer->GetOverwriteFlag())
		{
			ValidateCol(iNewRow, iMaxDispCol, iMaxCol);

			if (iMaxDispCol >= m_nAutoIndentCol)
				m_nAutoIndentCol = 0;
		}
		else
		{
			iMaxDispCol = iMaxCol = 1;
		}
	}
}

BOOL CXTPSyntaxEditCtrl::IsRowVisible(int iRow)
{
	if (iRow > GetRowCount() || iRow < 1)
		return FALSE;

	int iMaxRowInPage = GetRowPerPage();

	return (iRow >= m_nTopRow && iRow <= (m_nTopRow + iMaxRowInPage));
}

BOOL CXTPSyntaxEditCtrl::LowercaseSelection()
{
	return DoChangeSelectionCase(FALSE);
}

BOOL CXTPSyntaxEditCtrl::UppercaseSelection()
{
	return DoChangeSelectionCase(TRUE);
}

BOOL CXTPSyntaxEditCtrl::DoChangeSelectionCase(BOOL bUpper)
{
	if (!m_bSelectionExist)
		return FALSE;

	NormalizeSelection();

	m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);
	m_bSelectionExist = FALSE;
	m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(TRUE);

	for (int nRow = m_ptStartSel.y; nRow <= m_ptEndSel.y; nRow++)
	{
		int nVisFrom = m_bColumnSelect || (nRow == m_ptStartSel.y) ?
					m_ptStartSel.x : 0;
		int nVisTo = m_bColumnSelect || (nRow == m_ptEndSel.y) ?
					m_ptEndSel.x : INT_MAX;

		m_pBuffer->ChangeCase(nRow, nVisFrom, nVisTo, bUpper);
	}

	m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);
	m_bSelectionExist = TRUE;
	m_pBuffer->GetUndoRedoManager()->SetLastCommandText(XTP_IDS_EDIT_CASE);
	m_nDirtyRow = m_ptStartSel.y;
	InvalidateRows(m_ptStartSel.y, m_ptEndSel.y);
	Invalidate(FALSE);

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::DoChangeSelectionTabify(BOOL bTabify)
{
	if (!m_bSelectionExist)
		return FALSE;

	NormalizeSelection();

	m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);
	m_bSelectionExist = FALSE;
	m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(TRUE);

	for (int nRow = m_ptStartSel.y; nRow <= m_ptEndSel.y; nRow++)
	{
		int nVisFrom = m_bColumnSelect || (nRow == m_ptStartSel.y) ?
			m_ptStartSel.x : 0;
		int nVisTo = m_bColumnSelect || (nRow == m_ptEndSel.y) ?
			m_ptEndSel.x : INT_MAX;

		m_pBuffer->ChangeTabification(nRow, nVisFrom, nVisTo, bTabify);
	}

	m_pBuffer->GetUndoRedoManager()->SetGroupInsertMode(FALSE);
	m_bSelectionExist = TRUE;
	m_pBuffer->GetUndoRedoManager()->SetLastCommandText(bTabify ? XTP_IDS_EDIT_TABIFY : XTP_IDS_EDIT_UNTABIFY);
	m_nDirtyRow = m_ptStartSel.y;
	InvalidateRows(m_ptStartSel.y, m_ptEndSel.y);
	Invalidate(FALSE);

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::TabifySelection()
{
	return DoChangeSelectionTabify(TRUE);
}

BOOL CXTPSyntaxEditCtrl::UnTabifySelection()
{
	return DoChangeSelectionTabify(FALSE);
}

BOOL CXTPSyntaxEditCtrl::SetLineNumbers(BOOL bLineNumbers, BOOL bUpdateReg/*=FALSE*/)
{
	if (!SetValueBool(XTP_EDIT_REG_LINENUMBERS, bLineNumbers, m_bLineNumbers, bUpdateReg))
		return FALSE;

	CalculateEditbarLength();
	SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, TRUE);

	return TRUE;
}

BOOL CXTPSyntaxEditCtrl::GetLineNumbers() const
{
	return m_bLineNumbers;
}

void CXTPSyntaxEditCtrl::CalculateEditbarLength()
{
	int nPrvEditbarLength = m_nEditbarLength;

	// gutter
	m_nEditbarLength = GetSelMargin() ? m_nMarginLength : 0;

	// line numbers
	m_nLineNumLength = 0;

	if (GetLineNumbers())
	{
		int iRowNumLen = (int)log10l(GetRowCount()) + 1;

		CWindowDC dc(NULL);
		CXTPFontDC fontDC(&dc, XTPSyntaxEditPaintManager()->GetFontLineNumber());

		TEXTMETRIC tm;
		VERIFY(dc.GetTextMetrics(&tm));

		m_nLineNumLength = (iRowNumLen + 1) * (tm.tmAveCharWidth) + 2;
		m_nEditbarLength += m_nLineNumLength;
	}

	// collapsible nodes area
	if (m_bDrawNodes)
	{
		m_nEditbarLength += m_nNodesWidth;
	}

	m_nEditbarLength += TEXT_LEFT_OFFSET;

	// update window if necessary
	if (nPrvEditbarLength != m_nEditbarLength)
	{
		SetDirtyRow(-1);
		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol);
		//UpdateWindow();
		Invalidate();
	}
}


void CXTPSyntaxEditCtrl::AddRemoveBookmark(int nRow)
{
	if (GetLineMarksManager())
	{
		GetLineMarksManager()->AddRemoveLineMark(nRow, xtpEditLMT_Bookmark);
	}

	RedrawLineMarks();
}

void CXTPSyntaxEditCtrl::AddRemoveBreakPoint(int nRow)
{
	if (GetLineMarksManager())
	{
		GetLineMarksManager()->AddRemoveLineMark(nRow, xtpEditLMT_Breakpoint);
	}

	RedrawLineMarks();
}

void CXTPSyntaxEditCtrl::CollapseExpandBlock(int nRow)
{
	CXTPSyntaxEditLineMarksManager* pMgr = GetLineMarksManager();
	if (!pMgr)
		return;

	// nLineEnd: adjust also column, for example when collapsing to the string of lower length
	int nLineEnd = m_nDispCol;

	if (pMgr->HasRowMark(nRow, xtpEditLMT_Collapsed))
	{
		// try expand
		pMgr->DeleteLineMark(nRow, xtpEditLMT_Collapsed);
	}
	else
	{
		CXTPSyntaxEditLexTextSchema* ptrTxtSch = m_pBuffer->GetLexParser()->GetTextSchema();
		if (!ptrTxtSch)
			return;

		// try collapse
		XTP_EDIT_LMPARAM LMCoParam;
		CXTPSyntaxEditRowsBlockArray arCoBlocks;
		ptrTxtSch->GetCollapsableBlocksInfo(nRow, arCoBlocks);

		int nCount = (int)arCoBlocks.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			XTP_EDIT_ROWSBLOCK coBlk = arCoBlocks[i];
			if (coBlk.lcStart.nLine == nRow)
			{
				XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk = (XTP_EDIT_COLLAPSEDBLOCK*)LMCoParam.GetPtr();
				if (!pCoDrawBlk)
				{
					pCoDrawBlk = new XTP_EDIT_COLLAPSEDBLOCK;
					if (!pCoDrawBlk)
					{
						return;
					}
					LMCoParam.SetPtr(pCoDrawBlk, XTPSECollapsedBlockDeleteFn);
				}
				pCoDrawBlk->collBlock = coBlk;
				pMgr->SetLineMark(nRow, xtpEditLMT_Collapsed, &LMCoParam);
				m_arCollapsedTextRows.SetAtGrow(m_nCollapsedTextRowsCount, nRow);
				nLineEnd = min(nLineEnd, pCoDrawBlk->collBlock.lcEnd.nCol);
				m_nCollapsedTextRowsCount++;
			}
		}
	}

	// refresh picture
	SetDirtyRow(-1);

	SetCurrentDocumentRow(nRow);
	m_nDispCol = nLineEnd;
	SetCurCaretPos(GetVisibleRow(nRow), m_nDispCol);

	Invalidate(FALSE);

	RecalcScrollBars();
}

void CXTPSyntaxEditCtrl::DeleteBookmark(int nRow)
{
	if (GetLineMarksManager())
	{
		GetLineMarksManager()->DeleteLineMark(nRow, xtpEditLMT_Bookmark);
	}
}

void CXTPSyntaxEditCtrl::PrevBookmark()
{
	CXTPSyntaxEditLineMarksManager* pMgr = GetLineMarksManager();
	if (!pMgr)
	{
		ASSERT(FALSE);
		return;
	}
	int nRow = GetCurRow();
	int nVisRow = GetCurrentVisibleRow();

	int nPrevRow = nRow;
	do {
		pMgr->FindPrevLineMark(--nPrevRow, xtpEditLMT_Bookmark);
	} while (nPrevRow > 0 && GetVisibleRow(nPrevRow) == nVisRow);

	if (nPrevRow < 0)
	{
		POSITION posLast = pMgr->GetLastLineMark(xtpEditLMT_Bookmark);
		XTP_EDIT_LMDATA* pData = pMgr->GetLineMarkAt(posLast, xtpEditLMT_Bookmark);
		nPrevRow = pData ? pData->m_nRow : -1;
	}
	if (GetVisibleRow(nPrevRow) == nVisRow)
	{
		nPrevRow = -1;
	}

	if (nPrevRow >= 0)
	{
		SetCurPos(nPrevRow, 1);

		m_nDirtyRow = nPrevRow;
		m_nPrvDirtyRow = nRow;
		Invalidate(FALSE);
	}
}

void CXTPSyntaxEditCtrl::NextBookmark()
{
	CXTPSyntaxEditLineMarksManager* pMgr = GetLineMarksManager();
	if (!pMgr)
	{
		ASSERT(FALSE);
		return;
	}

	int nRow = GetCurRow();
	int nVisRow = GetCurrentVisibleRow();

	// move down to the next bookmark after the current collapsed block area
	int nNextRow = nRow;
	do {
		pMgr->FindNextLineMark(++nNextRow, xtpEditLMT_Bookmark);
	} while (nNextRow > 0 && GetVisibleRow(nNextRow) == nVisRow);

	if (nNextRow < 0)
	{
		// find first line mark
		POSITION posFirst = pMgr->GetFirstLineMark(xtpEditLMT_Bookmark);
		XTP_EDIT_LMDATA* pData = pMgr->GetNextLineMark(posFirst, xtpEditLMT_Bookmark);
		nNextRow = pData ? pData->m_nRow : -1;
	}

	if (GetVisibleRow(nNextRow) == nVisRow)
	{
		nNextRow = -1;
	}

	if (nNextRow >= 0)
	{
		SetCurPos(nNextRow, 1);

		m_nDirtyRow = nNextRow;
		m_nPrvDirtyRow = nRow;
		Invalidate(FALSE);
	}
}

BOOL CXTPSyntaxEditCtrl::HasRowMark(int nRow, XTP_EDIT_LINEMARKTYPE lmType,
											XTP_EDIT_LMPARAM* pParam)
{
	if (GetLineMarksManager())
	{
		return GetLineMarksManager()->HasRowMark(nRow, lmType, pParam);
	}
	return FALSE;
}


void CXTPSyntaxEditCtrl::DeleteBreakpoint(int nRow)
{
	if (GetLineMarksManager())
	{
		GetLineMarksManager()->DeleteLineMark(nRow, xtpEditLMT_Breakpoint);
	}
}

void CXTPSyntaxEditCtrl::RefreshLineMarks(XTP_EDIT_NMHDR_EDITCHANGED* pEditChanged)
{
	if (!GetLineMarksManager())
	{
		ASSERT(FALSE);
		return ;
	}

	XTPSyntaxEditLineMarksRefreshType eRefreshType = xtpEditLMRefresh_Unknown;

	if (pEditChanged->nAction & XTP_EDIT_EDITACTION_DELETEROW)
	{
		eRefreshType = xtpEditLMRefresh_Delete;
	}
	else if (pEditChanged->nAction & XTP_EDIT_EDITACTION_INSERTROW)
	{
		eRefreshType = xtpEditLMRefresh_Insert;
	}
	else
	{
		return;
	}

	GetLineMarksManager()->RefreshLineMarks(pEditChanged->nRowFrom,
										pEditChanged->nRowTo,  eRefreshType);

	RedrawLineMarks();
}


void CXTPSyntaxEditCtrl::RedrawLineMarks()
{
	SetDirtyRow(-1);
	Invalidate(FALSE);
}

BOOL CXTPSyntaxEditCtrl::HasBookmarks()
{
	CXTPSyntaxEditLineMarksManager* pMgr = GetLineMarksManager();
	if (pMgr)
	{
		int nCount = pMgr->GetCount(xtpEditLMT_Bookmark);
		return nCount > 0;
	}
	return FALSE;
}

BOOL CXTPSyntaxEditCtrl::HasBreakpoints()
{
	CXTPSyntaxEditLineMarksManager* pMgr = GetLineMarksManager();
	if (pMgr)
	{
		int nCount = pMgr->GetCount(xtpEditLMT_Breakpoint);
		return nCount > 0;
	}
	return FALSE;
}

CXTPSyntaxEditLineMarksManager* CXTPSyntaxEditCtrl::GetLineMarksManager()
{
	ASSERT(m_pBuffer);
	return m_pBuffer ? m_pBuffer->GetLineMarksManager() : NULL;
}

CString CXTPSyntaxEditCtrl::GetModulePath()
{
	TCHAR zsFileName[_MAX_PATH];

	//DWORD dwRes = ::GetModuleFileName(NULL, zsFileName, _MAX_PATH);
	DWORD dwRes = ::GetModuleFileName(AfxGetInstanceHandle(), zsFileName, _MAX_PATH);

	ASSERT(dwRes);

	CString csFilePath = zsFileName;
	int nFLs = csFilePath.ReverseFind(_T('\\'));
	if (nFLs > 0)
	{
		csFilePath.ReleaseBuffer(nFLs + 1);
	}

	return csFilePath;
}

CString CXTPSyntaxEditCtrl::GetDefaultCfgFilePath()
{
	return m_strDefaultCfgFilePath;
}

void CXTPSyntaxEditCtrl::RefreshColors()
{
	m_arOnScreenSchCache.RemoveAll();

	if (!m_pBuffer)
	{
		ASSERT(FALSE);
		return;
	}

	if (GetSyntaxColor())
	{
		XTP_EDIT_LINECOL pos1_0 = {1,0};
		BOOL bParseInThread = m_pBuffer->GetLexParser()->GetSchemaOptions(
						m_pBuffer->GetFileExt() )->m_bFirstParseInSeparateThread;
		if (bParseInThread)
		{
			m_pBuffer->GetLexParser()->StartParseInThread(m_pBuffer, &pos1_0, NULL, 0, TRUE);
		}
		else
		{
			CXTPSyntaxEditLexTextSchema* ptrTextSch = m_pBuffer->GetLexParser()->GetTextSchema();
			if (ptrTextSch)
			{
				CXTPSyntaxEditTextIterator txtIter(m_pBuffer);
				ptrTextSch->RunParseUpdate(TRUE, &txtIter, &pos1_0, NULL);
			}
		}
	}

	// Load data into AC
	SetAutoCompleteList();

	SetDirtyRow(-1);
	Invalidate();
}

CString CXTPSyntaxEditCtrl::GetConfigFile()
{
	if (!m_pBuffer)
		return _T("");

	return m_pBuffer->GetConfigFile();
}

BOOL CXTPSyntaxEditCtrl::SetConfigFile(LPCTSTR szPath)
{
	m_strDefaultCfgFilePath = szPath;

	if (!m_pBuffer || !FILEEXISTS_S(szPath))
	{
		return FALSE;
	}
	m_pBuffer->SetConfigFile(szPath);
	return TRUE;
}

const CString& CXTPSyntaxEditCtrl::GetCurrentTheme()
{
	if (!m_pBuffer || !m_pBuffer->GetLexConfigurationManager() || !GetSyntaxColor())
	{
		static CString strEmpty;
		return strEmpty;
	}
	return m_pBuffer->GetLexConfigurationManager()->GetCurrentTheme();
}

CStringArray& CXTPSyntaxEditCtrl::GetThemes()
{
	if (!m_pBuffer || !m_pBuffer->GetLexConfigurationManager())
	{
		static CStringArray s_ar;
		return s_ar;
	}
	return m_pBuffer->GetLexConfigurationManager()->GetThemeManager().GetThemes();
}

void CXTPSyntaxEditCtrl::ApplyTheme(CString strTheme)
{
	if (!m_pBuffer || !m_pBuffer->GetLexConfigurationManager() || !GetSyntaxColor())
	{
		return;
	}
	CXTPSyntaxEditLexTextSchema* ptrTextSch = m_pBuffer->GetLexParser()->GetTextSchema();
	m_pBuffer->GetLexConfigurationManager()->SetTheme(strTheme, ptrTextSch);

	m_arOnScreenSchCache.RemoveAll();

	SetDirtyRow(-1);
	UpdateWindow();
}


void CXTPSyntaxEditCtrl::OnBeforeEditChanged(int nRow, int nCol)
{
	UNREFERENCED_PARAMETER(nRow); UNREFERENCED_PARAMETER(nCol);

	if (GetSyntaxColor())
	{
		m_pBuffer->GetLexParser()->OnBeforeEditChanged();
	}
}

void CXTPSyntaxEditCtrl::OnEditChanged(const XTP_EDIT_LINECOL& LCFrom,
								  const XTP_EDIT_LINECOL& LCTo, int eEditAction)
{
	OnEditChanged(LCFrom.nLine, LCFrom.nCol, LCTo.nLine, LCTo.nCol, eEditAction);
}

void CXTPSyntaxEditCtrl::OnEditChanged(int nRowFrom, int nColFrom,
								  int nRowTo, int nColTo, int eEditAction)
{
	InvalidateRows(nRowFrom);

	if (GetSyntaxColor())
	{
		XTP_EDIT_LINECOL posFrom = {nRowFrom, max(0, nColFrom-1) };
		XTP_EDIT_LINECOL posTo = {nRowTo, max(0, nColTo-1) };

		m_pBuffer->GetLexParser()->OnEditChanged(posFrom, posTo, eEditAction, m_pBuffer);
	}
}

BOOL CXTPSyntaxEditCtrl::NotifyParseEvent(XTP_NOTIFY_CODE code, WPARAM wParam, LPARAM lParam)
{
	// Notify the parent class that position has been changed
	XTP_EDIT_NMHDR_PARSEEVENT pe;

	// NMHDR codes
	pe.nmhdr.code = XTP_EDIT_NM_PARSEEVENT;
	pe.nmhdr.hwndFrom = m_hWnd;
	pe.nmhdr.idFrom = GetDlgCtrlID();

	// notification event code.
	pe.code   = code;
	pe.wParam = wParam;
	pe.lParam = lParam;

	// Notify the parent window
	if (::IsWindow(m_pParentWnd->GetSafeHwnd()))
	{
		return (BOOL)m_pParentWnd->SendMessage(
			WM_NOTIFY, (WPARAM)pe.nmhdr.idFrom, (LPARAM)&pe);
	}

	return FALSE;
}

void CXTPSyntaxEditCtrl::OnParseEvent(XTP_NOTIFY_CODE code, WPARAM wParam, LPARAM lParam)
{
	NotifyParseEvent(code, wParam, lParam);

	//-----------------------------------------------------------------------
	int nRSFactor = 3;
	int nAveRedrawTime = m_aveRedrawScreenTime.GetAverageValue(333);
	int nUpdateTimeOut = max(1000, nAveRedrawTime * nRSFactor);

	BOOL bRedraw = FALSE;
	BOOL bRedrawTimeOut = TRUE;

	if (code == xtpEditOnParserStarted)
	{
		DWORD dwTime = ::GetTickCount();
		m_dwLastRedrawTime = dwTime;
	}
	else if (code == xtpEditOnTextBlockParsed)
	{
		CXTPSyntaxEditLexTextBlock* pTBended = (CXTPSyntaxEditLexTextBlock*)wParam;

		DWORD dwTime = ::GetTickCount();
		bRedrawTimeOut = labs(dwTime - m_dwLastRedrawTime) >= nUpdateTimeOut;
		if (bRedrawTimeOut)
		{
			m_dwLastRedrawTime = dwTime;
		}

		if (pTBended)
		{
			int nDispMax = GetRowPerPage() - 1;
			int nBottomRow = GetDocumentRow(nDispMax+5);

			if (!(pTBended->m_PosStartLC.nLine > nBottomRow ||
				pTBended->m_PosEndLC.nLine < m_nTopRow) )
			{
				bRedraw = TRUE;
			}

			ClearOnScreenSchCache(pTBended->m_PosStartLC.nLine);
		}
	}
	else if (code == xtpEditOnParserEnded)
	{
		bRedraw = TRUE;
	}

	//-----------------------------
	if (bRedraw)
	{
		KillTimer(TIMER_REDRAW_WHEN_PARSE);

		if (bRedrawTimeOut)
		{
			SetDirtyRow(-1);
			Invalidate(FALSE);
			UpdateWindow();
		}
		else
		{
			SetTimer(TIMER_REDRAW_WHEN_PARSE, nUpdateTimeOut*5, NULL);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Row / col functions

int CXTPSyntaxEditCtrl::GetTopRow()
{
	return m_nTopRow;
}

BOOL CXTPSyntaxEditCtrl::SetTopRow(int iRow)
{
	if (iRow > GetRowCount())
		return FALSE;

	m_nTopRow = iRow;

	return TRUE;
}

int CXTPSyntaxEditCtrl::GetCurRow()
{
	return GetCurrentDocumentRow();
}

int CXTPSyntaxEditCtrl::GetCurCol()
{
	return m_nDispCol;
}

int CXTPSyntaxEditCtrl::GetCurAbsCol()
{
	return m_nCurrentCol;
}

BOOL CXTPSyntaxEditCtrl::SetDirtyRow(int nRow)
{
	if (nRow > GetRowCount())
		return FALSE;

	m_nDirtyRow = nRow;
	m_nPrvDirtyRow = -1;
	return TRUE;
}

int CXTPSyntaxEditCtrl::GetDirtyRow()
{
	return m_nDirtyRow;
}

void CXTPSyntaxEditCtrl::SetCurRowCol(int iRow, int iCol)
{
	if (iRow < 1)
		iRow = 1;

	if (iRow > GetRowCount())
		iRow = GetRowCount();

	m_nDispCol = iCol;
	ValidateCol(iRow, m_nDispCol, m_nCurrentCol);
	m_nMaxCol = m_nDispCol;

	SetCurrentDocumentRow(iRow);
}

void CXTPSyntaxEditCtrl::GoToRow(int iRow, BOOL bSelectRow/* = FALSE*/)
{
	CString csLineText = GetLineText(iRow);
	int nMaxCol = csLineText.GetLength() + 1;

	m_nCurrentCol = 1;
	m_nDispCol = CalcDispCol(csLineText, nMaxCol);
	m_nMaxCol = m_nDispCol;

	if (bSelectRow)
	{
		m_bSelectingFromBookmark = TRUE;
		m_bSelectionExist = TRUE;
		m_ptStartSel.x = 1;
		m_ptStartSel.y = iRow;
		m_nBookmarkSelStartRow = iRow;
		m_ptEndSel.x = 1;
		m_ptEndSel.y = iRow + 1;
		m_nDirtyRow = -1;

		SetCurrentDocumentRow(iRow);
		Invalidate(FALSE);

		m_bSelectionStarted = TRUE;

		SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol, TRUE);
		SetTimer(TIMER_SELECTION_ID, 100, NULL);
	}
	else
	{
		SetCurrentDocumentRow(iRow);
		SetCurPos(iRow, 0);
	}
}

int CXTPSyntaxEditCtrl::GetCurrentVisibleRow()
{
	return GetVisibleRow(GetCurrentDocumentRow());
}

int CXTPSyntaxEditCtrl::GetCurrentDocumentRow()
{
	return m_nCurrentDocumentRow;
}

void CXTPSyntaxEditCtrl::SetCurrentDocumentRow(int nRow)
{
	if (nRow > GetRowCount())
		nRow = GetRowCount();

	if (nRow < 1)
		nRow = 1;

	m_nCurrentDocumentRow = nRow;

	// By Leva: recalc scrollbars if we get out of range
	if (m_nCurrentDocumentRow < m_nTopCalculatedRow || m_nCurrentDocumentRow > m_nBottomCalculatedRow)
		RecalcScrollBars();
}

int CXTPSyntaxEditCtrl::GetDocumentRow(int nVisibleRow)
{
	return CalculateDocumentRow(m_nTopRow, nVisibleRow);
}

int CXTPSyntaxEditCtrl::CalculateDocumentRow(int nStartDocumentRow, int nRowDelta)
{
	int nDocRow = nStartDocumentRow + nRowDelta - 1;
	int nNextCollapsedRow = nStartDocumentRow - 1;

	POSITION pos = GetLineMarksManager()->FindNextLineMark(nNextCollapsedRow, xtpEditLMT_Collapsed);
	while (pos != NULL)
	{
		XTP_EDIT_LMDATA* pData = GetLineMarksManager()->GetNextLineMark(pos, xtpEditLMT_Collapsed);
		if (pData && pData->m_nRow >= nNextCollapsedRow) // mark should be not within previous collapsed block
		{
			if (pData->m_nRow >= nDocRow) // finish if mark is greater then row to calculate for
				break;

			XTP_EDIT_COLLAPSEDBLOCK* pCoDBlk = (XTP_EDIT_COLLAPSEDBLOCK*)pData->m_Param.GetPtr();
			if (!pCoDBlk)
				continue;

			int nHiddenRows = pCoDBlk->collBlock.lcEnd.nLine - pCoDBlk->collBlock.lcStart.nLine;

			nDocRow += nHiddenRows;
			nNextCollapsedRow = pData->m_nRow + nHiddenRows;
		}
	}
	return nDocRow;
}

int CXTPSyntaxEditCtrl::GetVisibleRow(int nDocumentRow)
{
	return CalculateVisibleRow(m_nTopRow, nDocumentRow);
}

int CXTPSyntaxEditCtrl::CalculateVisibleRow(int nStartDocumentRow, int nDocumentRow)
{
	int nVisRow = nDocumentRow - nStartDocumentRow + 1;
	int nNextCollapsedRow = nStartDocumentRow - 1;
	const int nRowCount = GetRowCount();

	POSITION pos = GetLineMarksManager()->FindNextLineMark(nNextCollapsedRow, xtpEditLMT_Collapsed);
	while (pos != NULL)
	{
		XTP_EDIT_LMDATA* pData = GetLineMarksManager()->GetNextLineMark(pos, xtpEditLMT_Collapsed);
		if (pData && pData->m_nRow >= nNextCollapsedRow) // mark should be not within previous collapsed block
		{
			if (pData->m_nRow >= nDocumentRow) // finish if mark is greater then row to calculate for
				break;

			XTP_EDIT_COLLAPSEDBLOCK* pCoDBlk = (XTP_EDIT_COLLAPSEDBLOCK*)pData->m_Param.GetPtr();
			if (!pCoDBlk)
				continue;

			int nHiddenRows = min(nRowCount, pCoDBlk->collBlock.lcEnd.nLine) -
				pCoDBlk->collBlock.lcStart.nLine;

			nVisRow -= nHiddenRows;
			nNextCollapsedRow = pData->m_nRow + nHiddenRows;
		}
	}
	return max(nVisRow, 1); // TODO: make additional check, there might be an error
	//return nVisRow;
}

int CXTPSyntaxEditCtrl::MoveCurrentVisibleRowUp(int nCount)
{
	int nOldVisRow = GetCurrentVisibleRow();
	int nOldDocRow = GetCurrentDocumentRow();

	int nNewVisRow = nOldVisRow - nCount;

	// do not shift a window up
	if (nNewVisRow > 0)
	{
		SetCurrentDocumentRow(GetDocumentRow(nNewVisRow));
		m_nDirtyRow = GetCurrentDocumentRow();
		return nNewVisRow;
	}

	// find global visible row index
	int nVisRow = CalculateVisibleRow(1, nOldDocRow);
	int nTargetDocRow = CalculateDocumentRow(1, max(nVisRow - nCount, 1));
	int nDocDiff = nOldDocRow - nTargetDocRow;
	nDocDiff = min(nDocDiff, m_nTopRow - 1);
	nOldDocRow -= nDocDiff;
	m_nCurrentDocumentRow -= nDocDiff;
	m_nTopRow -= nDocDiff;

	nNewVisRow = nOldVisRow - nDocDiff + GetRowPerPage();
	SetDirtyRow(-1);

	return GetCurrentVisibleRow();
}

int CXTPSyntaxEditCtrl::MoveCurrentVisibleRowDown(int nCount)
{
	int nOldRow = GetCurrentVisibleRow();
	int nNewDocRow = min(GetDocumentRow(nOldRow + nCount), GetRowCount());
	SetCurrentDocumentRow(nNewDocRow);
	m_nPrvDirtyRow = m_nDirtyRow;
	m_nDirtyRow = nNewDocRow;
	int nMaxVisRow = GetRowPerPage();
	int nMaxDocRow = GetDocumentRow(nMaxVisRow);
	if (nNewDocRow > nMaxDocRow)
	{
		m_nTopRow = GetDocumentRow(nCount + 1);
		SetDirtyRow(-1);
	}
	return GetCurrentVisibleRow();
}

int CXTPSyntaxEditCtrl::GetRowPerPage()
{
	if (!m_tm.tmHeight)
		return 0;

	CXTPClientRect rcWnd(this);

	return (rcWnd.Height() / m_tm.tmHeight);
}

int CXTPSyntaxEditCtrl::GetVisibleRowsCount(int nMaxDocRow)
{
	if (nMaxDocRow < 0)
		nMaxDocRow = GetRowCount();

	int nVisRow = nMaxDocRow;
	CXTPSyntaxEditLineMarksManager* pMgr = GetLineMarksManager();
	if (pMgr)
	{
		int nNextCollapsedRow = -1;
		POSITION pos = pMgr->GetFirstLineMark(xtpEditLMT_Collapsed);
		while (pos != NULL)
		{
			XTP_EDIT_LMDATA* pData = pMgr->GetNextLineMark(pos, xtpEditLMT_Collapsed);
			if (pData && pData->m_nRow >= nNextCollapsedRow) // mark should be not within previous collapsed block
			{
				if (pData->m_nRow >= nMaxDocRow) // finish if mark is greater then row to calculate for
					break;

				XTP_EDIT_COLLAPSEDBLOCK* pCoDBlk = (XTP_EDIT_COLLAPSEDBLOCK*)pData->m_Param.GetPtr();
				if (!pCoDBlk)
					continue;

				int nHiddenRows = pCoDBlk->collBlock.lcEnd.nLine - pCoDBlk->collBlock.lcStart.nLine;

				nVisRow -= nHiddenRows;
				nNextCollapsedRow = pData->m_nRow + nHiddenRows;
			}
		}
	}
	return nVisRow;
}

BOOL CXTPSyntaxEditCtrl::ShiftCurrentVisibleRowUp(int nCount, BOOL bChangeCaret)
{
	BOOL bChanged = FALSE;
	if (m_nTopRow > 1)
	{
		int nOldTopRow = m_nTopRow;
		int nDocRow = GetCurrentDocumentRow();      // store old document row
		SetCurrentDocumentRow(GetDocumentRow(1));   // move to first visible row
		MoveCurrentVisibleRowUp(nCount);            // move nCount row up
		// move to old document row
		int nBottomDocRow = GetDocumentRow(GetRowPerPage());
		if (bChangeCaret)
			nDocRow = min(nDocRow, nBottomDocRow);
		SetCurrentDocumentRow(nDocRow);
		if (nDocRow > nBottomDocRow)
			CWnd::HideCaret();
		if (nOldTopRow != m_nTopRow)
			bChanged = TRUE;
	}
	return bChanged;
}

BOOL CXTPSyntaxEditCtrl::ShiftCurrentVisibleRowDown(int nCount, BOOL bChangeCaret)
{
	BOOL bChanged = FALSE;
	if (m_nTopRow < GetRowCount())
	{
		int nOldTopRow = m_nTopRow;
		int nDocRow = GetCurrentDocumentRow();
		int nLastVisRow = GetRowPerPage();
		SetCurrentDocumentRow(GetDocumentRow(nLastVisRow));
		MoveCurrentVisibleRowDown(nCount);
		int nTopDocRow = GetDocumentRow(1);
		if (bChangeCaret)
			nDocRow = max(nDocRow, nTopDocRow);
		SetCurrentDocumentRow(nDocRow);
		if (nDocRow < nTopDocRow)
			CWnd::HideCaret();
		if (nOldTopRow != m_nTopRow)
			bChanged = TRUE;
	}
	return bChanged;
}

BOOL CXTPSyntaxEditCtrl::GetCollapsedBlockLen(int nStartRow, int& rnLen)
{
	rnLen = 0;

	XTP_EDIT_LMPARAM LMCoParam;
	BOOL bIsCollapsed = HasRowMark(nStartRow, xtpEditLMT_Collapsed, &LMCoParam);
	if (!bIsCollapsed)
	{
		return FALSE;
	}

	XTP_EDIT_COLLAPSEDBLOCK* pCoDBlk = (XTP_EDIT_COLLAPSEDBLOCK*)LMCoParam.GetPtr();
	if (!pCoDBlk)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	rnLen = pCoDBlk->collBlock.lcEnd.nLine - pCoDBlk->collBlock.lcStart.nLine;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Line text internal functions

const CString& CXTPSyntaxEditCtrl::GetLineText(int nRow, BOOL bAddCRLF, int iCRLFStyle)
{
	GetLineText(nRow, m_strTmpLineTextBuff, bAddCRLF, iCRLFStyle);

	return m_strTmpLineTextBuff;
}

//LPCTSTR CXTPSyntaxEditCtrl::GetLineText(int nRow, BOOL bAddCRLF, int iCRLFStyle)
//{
//  GetLineText(nRow, m_strTmpLineTextBuff, bAddCRLF, iCRLFStyle);
//
//  return m_strTmpLineTextBuff;
//}

void CXTPSyntaxEditCtrl::GetLineText(int nRow, CString& strBuffer, BOOL bAddCRLF, int iCRLFStyle)
{
	m_pBuffer->GetLineText(nRow, strBuffer, bAddCRLF, iCRLFStyle);

	XTP_EDIT_LMPARAM LMCoParam;
	BOOL bIsCollapsed = HasRowMark(nRow, xtpEditLMT_Collapsed, &LMCoParam);
	if (bIsCollapsed)
	{
		XTP_EDIT_COLLAPSEDBLOCK* pCoDBlk = (XTP_EDIT_COLLAPSEDBLOCK*)LMCoParam.GetPtr();
		if (!pCoDBlk)
		{
			ASSERT(FALSE);
			return;
		}
		if (strBuffer.GetLength() > pCoDBlk->collBlock.lcStart.nCol)
		{
			strBuffer.SetAt(pCoDBlk->collBlock.lcStart.nCol, 0);
		}
	}
}

void CXTPSyntaxEditCtrl::SetAutoCompleteList()
{
	CXTPSyntaxEditLexTokensDefArray arTokens;
	m_pBuffer->GetLexParser()->GetTokensForAutoCompleate( arTokens, FALSE);
	CXTPSyntaxEditACDataArray arrData;
	//PXTPACDATA pACData;
	CXTPSyntaxEditLexTokensDef TokenDef;
	int nTokensDef = (int)arTokens.GetSize();
	if (!nTokensDef)
		return;

	for (int nTokenDef = 0; nTokenDef < nTokensDef; nTokenDef++)
	{
		TokenDef = (arTokens.GetAt(nTokenDef)); //ASSERT(pTokenDef);
		int nTags = (int)TokenDef.m_arTokens.GetSize();
		for (int nTag = 0; nTag < nTags; nTag++)
		{
			arrData.Add(new XTP_EDIT_ACDATA(0, TokenDef.m_arTokens.GetAt(nTag)));
		}

		int nDelims = (int)TokenDef.m_arStartSeps.GetSize();
		int nDelim;
		for (nDelim = 0; nDelim < nDelims; nDelim++)
		{
			if (m_wndAutoComplete.m_strDelims.Find(TokenDef.m_arStartSeps.GetAt(nDelim)) < 0)
				m_wndAutoComplete.m_strDelims += TokenDef.m_arStartSeps.GetAt(nDelim);
		}

		nDelims = (int)TokenDef.m_arEndSeps.GetSize();
		for (nDelim = 0; nDelim < nDelims; nDelim++)
		{
			if (m_wndAutoComplete.m_strDelims.Find(TokenDef.m_arEndSeps.GetAt(nDelim)) < 0)
				m_wndAutoComplete.m_strDelims += TokenDef.m_arEndSeps.GetAt(nDelim);
		}

	}

	//TRACE(m_wndAutoComplete.m_strDelims);
	m_wndAutoComplete.SetList(arrData);
}

BOOL CXTPSyntaxEditCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (m_wndAutoComplete.IsActive())
	{
		switch (pMsg->message)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_DEADCHAR:
			{
				m_wndAutoComplete.SendMessage(
					pMsg->message, pMsg->wParam, pMsg->lParam);
			}
			break;
		}

		if (pMsg->message == WM_KEYDOWN)
		{
			switch (pMsg->wParam)
			{
			case VK_SPACE:
			case VK_PRIOR:
			case VK_NEXT:
			case VK_END:
			case VK_HOME:
			case VK_LEFT:
			case VK_UP:
			case VK_RIGHT:
			case VK_DOWN:
			case VK_SELECT:
			case VK_PRINT:
			case VK_EXECUTE:
			case VK_SNAPSHOT:
			case VK_INSERT:
			case VK_DELETE:
			case VK_HELP:
			case VK_RETURN:
				{

				}
				return TRUE;
			}
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CXTPSyntaxEditCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_wndAutoComplete.IsActive())
	{
		m_wndAutoComplete.Hide();
	}

	m_wndToolTip.Hide();

	if (zDelta < 0)
		Scroll(0, m_nWheelScroll);
	else
		Scroll(0, -m_nWheelScroll);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CXTPSyntaxEditCtrl::CollapseAll()
{
	CXTPSyntaxEditLexTextSchema* ptrTxtSch = m_pBuffer->GetLexParser()->GetTextSchema();
	if (!ptrTxtSch)
		return;

	CXTPSyntaxEditLineMarksManager* pMgr = GetLineMarksManager();
	if (!pMgr)
		return;

	CWaitCursor _WC;

	CXTPSyntaxEditRowsBlockArray arCoBlocks;
	ptrTxtSch->GetCollapsableBlocksInfo(-1, arCoBlocks);

	for (int nRow = 0; nRow < GetRowCount(); nRow++)
	{
		XTP_EDIT_LMPARAM LMCoParam;

		int toRemove = -1;

		int nCount = (int)arCoBlocks.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			XTP_EDIT_ROWSBLOCK coBlk = arCoBlocks[i];
			if (coBlk.lcStart.nLine == nRow)
			{
				XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk = (XTP_EDIT_COLLAPSEDBLOCK*)LMCoParam.GetPtr();
				if (!pCoDrawBlk)
				{
					pCoDrawBlk = new XTP_EDIT_COLLAPSEDBLOCK;
					if (!pCoDrawBlk)
					{
						return;
					}
					LMCoParam.SetPtr(pCoDrawBlk, XTPSECollapsedBlockDeleteFn);
				}
				pCoDrawBlk->collBlock = coBlk;
				pMgr->SetLineMark(nRow, xtpEditLMT_Collapsed, &LMCoParam);
				m_arCollapsedTextRows.SetAtGrow(m_nCollapsedTextRowsCount, nRow);
				m_nCollapsedTextRowsCount++;
				if (toRemove < 0)
					toRemove = i;
			}
		}
		if (toRemove >= 0)
			arCoBlocks.RemoveAt(0, toRemove+1);
	}

	// refresh picture
	SetDirtyRow(-1);
	SetCurCaretPos(GetCurrentVisibleRow(), 1/*m_nDispCol*/);
	UpdateWindow();
	RecalcScrollBars();

	UpdateScrollPos();
}

void CXTPSyntaxEditCtrl::ExpandAll()
{
	CXTPSyntaxEditLineMarksManager* pMgr = GetLineMarksManager();
	if (!pMgr)
		return;

	CWaitCursor _WC;

	pMgr->RemoveAll(xtpEditLMT_Collapsed);

	// refresh picture
	SetDirtyRow(-1);
	SetCurCaretPos(GetCurrentVisibleRow(), m_nDispCol);
	UpdateWindow();
	RecalcScrollBars();

	UpdateScrollPos();
}

CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditCtrl::GetOnScreenSch(int nForRow)
{
	int nLifeTime = m_pBuffer->GetLexParser()->GetSchemaOptions(
						m_pBuffer->GetFileExt())->m_dwOnScreenSchCacheLifeTime_sec;
	m_arOnScreenSchCache.RemoveOld(nLifeTime);

	//========================================================================
	int nCount = (int)m_arOnScreenSchCache.GetSize();
	int i;

	for (i = 0; i < nCount; i++)
	{
		CScreenSearchBlock& rSchBlk = m_arOnScreenSchCache[i];

		if (nForRow >= rSchBlk.nRowStart && nForRow <= rSchBlk.nRowEnd)
		{
			rSchBlk.dwLastAccessTime = ::GetTickCount();
			return rSchBlk.ptrTBFirst;
		}
	}

	//---------------------------------------------------------------------------
	CXTPSyntaxEditLexTextSchema* ptrTxtSch = m_pBuffer->GetLexParser()->GetTextSchema();
	if (ptrTxtSch)
	{
		int nRowPerPage = GetRowPerPage()+1;

		CScreenSearchBlock schBlkNew;
		schBlkNew.nRowStart = nForRow;
		schBlkNew.nRowEnd = nForRow + nRowPerPage;

		//--------------------------------------------------------------------
		int nRowStartMin = 1;
		// 1. search next on screen region
		nCount = (int)m_arOnScreenSchCache.GetSize();

		for (i = 0; i < nCount; i++)
		{
			const CScreenSearchBlock& rSchBlk = m_arOnScreenSchCache[i];

			if (rSchBlk.nRowStart > schBlkNew.nRowStart && rSchBlk.nRowStart <= schBlkNew.nRowEnd)
			{
				schBlkNew.nRowEnd = rSchBlk.nRowStart-1;
			}

			if (rSchBlk.nRowEnd+1 > nRowStartMin && rSchBlk.nRowEnd+1 < nForRow)
			{
				nRowStartMin = rSchBlk.nRowEnd+1;
			}
		}
		//--------------------------------------------------------------------
		// 2. search next collapsed block start
		for (i = 0; i < m_nCollapsedTextRowsCount; i++)
		{
			int nCoStartRow = m_arCollapsedTextRows[i];

			if (nCoStartRow >= schBlkNew.nRowStart && nCoStartRow < schBlkNew.nRowEnd)
			{
				schBlkNew.nRowEnd = nCoStartRow;
			}
		}

		//===========================
		if (schBlkNew.nRowEnd - schBlkNew.nRowStart < nRowPerPage/2)
		{
			schBlkNew.nRowStart = max(schBlkNew.nRowStart-nRowPerPage, nRowStartMin);
		}

		//--------------------------------------------------------------------
		CXTPSyntaxEditTextIterator txtIter(m_pBuffer);

		BOOL bParseRes = ptrTxtSch->RunParseOnScreen(&txtIter, schBlkNew.nRowStart,
									schBlkNew.nRowEnd, schBlkNew.ptrTBFirst);
		if (bParseRes)
		{
			schBlkNew.dwLastAccessTime = ::GetTickCount();

			m_arOnScreenSchCache.Add(schBlkNew);
			return schBlkNew.ptrTBFirst;
		}
	}

	return NULL;
}

void CXTPSyntaxEditCtrl::InvalidateRows(int nRowFrom, int nRowTo)
{
	int nCountVDR = (int)m_arValidDispRows.GetSize();

	//-* Invalidate ALL ------------------------------------------------------
	if (nRowFrom < 0 && nRowTo < 0)
	{
		m_arOnScreenSchCache.RemoveAll();

		for (int i = 0; i < nCountVDR; i++) {
			m_arValidDispRows.SetAtGrow(i, 0);
		}
		return ;
	}

	//-* Invalidate Part -----------------------------------------------------
	int nRow1 = nRowFrom;
	int nRow2 = nRowTo;

	if (nRow1 < 0)
	{
		nRow1 = m_nTopRow;
	}
	if (nRow2 < nRow1)
	{
		nRow2 = GetDocumentRow(GetRowPerPage());
	}

	int nDispRow1= CalculateVisibleRow(m_nTopRow, nRow1);
	int nDispRow2 = CalculateVisibleRow(m_nTopRow, nRow2);

	int nI1 = max(min(nDispRow1,1000), 0);
	int nI2 = max(min(nDispRow2,1000), nCountVDR-1);
	for (int i = nI1; i <= nI2; i++)
	{
		m_arValidDispRows.SetAtGrow(i, 0);
	}

	//===========================================================================
	ClearOnScreenSchCache(nRowFrom);
}

void CXTPSyntaxEditCtrl::ClearOnScreenSchCache(int nRowFrom)
{
	//-* clear ALL ------------------------------------------------------
	if (nRowFrom < 0)
	{
		m_arOnScreenSchCache.RemoveAll();
		return ;
	}

	//===========================================================================
	int nCountSSC = (int)m_arOnScreenSchCache.GetSize();
	for (int i = nCountSSC-1; i >= 0; i--)
	{
		CScreenSearchBlock& rSchBlk = m_arOnScreenSchCache[i];

		BOOL bRemove = FALSE;

		if (nRowFrom >= rSchBlk.nRowStart && nRowFrom <= rSchBlk.nRowEnd)
		{
			rSchBlk.nRowEnd = nRowFrom-1;
			bRemove = !(rSchBlk.nRowStart <= rSchBlk.nRowEnd);
		}
		else if (nRowFrom < rSchBlk.nRowStart)
		{
			bRemove = TRUE;
		}

		//-------------------------------------
		if (bRemove)
		{
			m_arOnScreenSchCache.RemoveAt(i);
		}
		else
		{
			ASSERT(m_arOnScreenSchCache[i].nRowStart == rSchBlk.nRowStart);
			ASSERT(m_arOnScreenSchCache[i].nRowEnd == rSchBlk.nRowEnd);
		}
	}
}

void CXTPSyntaxEditCtrl::SetRowValid(int nDispRow)
{
	ASSERT(nDispRow < 1000);
	m_arValidDispRows.SetAtGrow(nDispRow, 1);
}

BOOL CXTPSyntaxEditCtrl::IsRowValid(int nDispRow)
{
	int nCount = (int)m_arValidDispRows.GetSize();

	if (nDispRow > 0 && nDispRow < nCount)
	{
		int nValid = m_arValidDispRows[nDispRow];
		return (nValid != 0);
	}

	return FALSE;

}

const CStringList& CXTPSyntaxEditCtrl::GetUndoTextList()
{
	return m_pBuffer->GetUndoRedoManager()->GetUndoTextList();
}

const CStringList& CXTPSyntaxEditCtrl::GetRedoTextList()
{
	return m_pBuffer->GetUndoRedoManager()->GetRedoTextList();
}

void CXTPSyntaxEditCtrl::OnLexCfgWasChanged(XTP_NOTIFY_CODE code, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam);

	if (code == xtpEditAllConfigWasChanged ||
		code == xtpEditClassSchWasChanged ||
		code == xtpEditThemeWasChanged)
	{
		m_arOnScreenSchCache.RemoveAll();

		SetDirtyRow(-1);
		Invalidate();
	}
	else
	{
		ASSERT(FALSE);
	}
}

BOOL CXTPSyntaxEditCtrl::GetRowNodes(int nRow, DWORD& dwType)
{
	dwType = XTP_EDIT_ROWNODE_NOTHING;

	CXTPSyntaxEditLexTextSchema* ptrTxtSch = m_pBuffer->GetLexParser()->GetTextSchema();
	if (!ptrTxtSch)
	{
		return FALSE;
	}

	// retrieve row nodes
	CXTPSyntaxEditRowsBlockArray arCoBlocks;

	ptrTxtSch->GetCollapsableBlocksInfo(nRow, arCoBlocks, &m_fcCollapsable.ptrTBStart);

	int nCount = (int)arCoBlocks.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		XTP_EDIT_ROWSBLOCK coBlk = arCoBlocks[i];
		if (coBlk.lcStart.nLine == nRow)
		{
			if (HasRowMark(coBlk.lcStart.nLine, xtpEditLMT_Collapsed))
			{
				dwType |= XTP_EDIT_ROWNODE_COLLAPSED;
			}
			else
			{
				dwType |= XTP_EDIT_ROWNODE_EXPANDED;
			}
		}
		if (coBlk.lcEnd.nLine == nRow)
		{
			dwType |= XTP_EDIT_ROWNODE_ENDMARK;
		}
		if (coBlk.lcStart.nLine < nRow)
		{
			dwType |= XTP_EDIT_ROWNODE_NODEUP;
		}
		if (coBlk.lcEnd.nLine > nRow && (dwType & XTP_EDIT_ROWNODE_COLLAPSED))
		{
			dwType |= XTP_EDIT_ROWNODE_NODEDOWN;
		}
	}

	return (dwType != XTP_EDIT_ROWNODE_NOTHING);
}

BOOL CXTPSyntaxEditCtrl::NotifyMarginLBtnClick(int nRow, int nDispRow)
{
	// Notify the parent class that position has been changed
	XTP_EDIT_NMHDR_MARGINCLICKED somc;

	// NMHDR codes
	somc.nmhdr.code = XTP_EDIT_NM_MARGINCLICKED;
	somc.nmhdr.hwndFrom = m_hWnd;
	somc.nmhdr.idFrom = GetDlgCtrlID();

	// Row col specific codes
	somc.nRow = nRow;
	somc.nDispRow = nDispRow;

	// Notify the parent window
	if (::IsWindow(m_pParentWnd->GetSafeHwnd()))
	{
		return (BOOL)m_pParentWnd->SendMessage(
			WM_NOTIFY, (WPARAM)somc.nmhdr.idFrom, (LPARAM)&somc);
	}

	return FALSE;
}

UINT CXTPSyntaxEditCtrl::CalcAveDataSize(int nRowStart, int nRowEnd)
{
	UINT uSize = m_nAverageLineLen * abs(nRowStart - nRowEnd);
	uSize = (uSize / 1024 + 1) * 1024;
	return uSize;
}

BOOL CXTPSyntaxEditCtrl::GetWideCaret() const
{
	return m_bWideCaret && m_pBuffer && m_pBuffer->GetOverwriteFlag();
}

BOOL CXTPSyntaxEditCtrl::SetWideCaret(BOOL bWideCaret, BOOL bUpdateReg/*=FALSE*/)
{
	if (m_bWideCaret != bWideCaret)
	{
		if (!SetValueBool(XTP_EDIT_REG_WIDECARET, bWideCaret, m_bWideCaret, bUpdateReg))
			return FALSE;

		CPoint ptCarPos = CWnd::GetCaretPos();
		CWnd::CreateSolidCaret(GetWideCaret()? m_tm.tmAveCharWidth: 2, m_tm.tmHeight);
		CWnd::SetCaretPos(ptCarPos);
		CWnd::ShowCaret();
	}

	return TRUE;
}

//===========================================================================
// CXTPSyntaxEditCtrl::CTextSearchCache
//===========================================================================

CXTPSyntaxEditCtrl::CTextSearchCache::CTextSearchCache()
{
	nForTopRow = 0;
}

void CXTPSyntaxEditCtrl::CTextSearchCache::Update(int nCurrTopRow)
{
	if (nCurrTopRow != nForTopRow)
	{
		ptrTBStart = NULL;
		nForTopRow = nCurrTopRow;
	}
}

//===========================================================================
// CXTPSyntaxEditCtrl::CScreenSearchBlock
//===========================================================================

CXTPSyntaxEditCtrl::CScreenSearchBlock::CScreenSearchBlock()
{
	nRowStart = nRowEnd = 0;
	dwLastAccessTime = 0;
}

CXTPSyntaxEditCtrl::CScreenSearchBlock::CScreenSearchBlock(const CScreenSearchBlock& rSrc)
{
	nRowStart = rSrc.nRowStart;
	nRowEnd = rSrc.nRowEnd;
	ptrTBFirst = rSrc.ptrTBFirst;
	dwLastAccessTime = rSrc.dwLastAccessTime;
}

CXTPSyntaxEditCtrl::CScreenSearchBlock::~CScreenSearchBlock()
{

}

//===========================================================================
// CXTPSyntaxEditCtrl::CScreenSearchCache
//===========================================================================

CXTPSyntaxEditCtrl::CScreenSearchCache::CScreenSearchCache()
{
	m_dwLastRemoveOldTime = 0;
}

CXTPSyntaxEditCtrl::CScreenSearchCache::~CScreenSearchCache()
{
	RemoveAll();
}

void CXTPSyntaxEditCtrl::CScreenSearchCache::RemoveAll()
{
	for (int i = 0; i < (int)GetSize(); i++)
	{
		CScreenSearchBlock& rSchBlk = ElementAt(i);
		CXTPSyntaxEditLexTextSchema::Close(rSchBlk.ptrTBFirst);
	}
	Base::RemoveAll();
}

void CXTPSyntaxEditCtrl::CScreenSearchCache::RemoveAt(int nIndex)
{
	CScreenSearchBlock& rSchBlk = ElementAt(nIndex);

	CXTPSyntaxEditLexTextSchema::Close(rSchBlk.ptrTBFirst);
	Base::RemoveAt(nIndex);
}

void CXTPSyntaxEditCtrl::CScreenSearchCache::RemoveOld(int nTimeOut_sec)
{
	int nTimeOut_ms = nTimeOut_sec * 1000;

	DWORD dwTime = ::GetTickCount();

	int cnCheckTimeOut_ms = min(nTimeOut_sec, 10*1000);

	if (nTimeOut_sec == 0 || nTimeOut_sec == -1 ||
		labs(dwTime - m_dwLastRemoveOldTime) < cnCheckTimeOut_ms)
	{
		return;
	}

	//================================================================
	m_dwLastRemoveOldTime = dwTime;
	for (int i = (int)GetSize() - 1; i >= 0; i--)
	{
		CScreenSearchBlock& rSchBlk = ElementAt(i);

		if (labs(dwTime - rSchBlk.dwLastAccessTime) >= nTimeOut_ms)
		{
			CXTPSyntaxEditLexTextSchema::Close(rSchBlk.ptrTBFirst);
			Base::RemoveAt(i);
		}
	}
}

//===========================================================================
// CXTPSyntaxEditCtrl::CAverageVal
//===========================================================================

CXTPSyntaxEditCtrl::CAverageVal::CAverageVal(int nDataSize/* = 100*/)
{
	m_nDataSize = max(1, nDataSize);
	m_nNextIndex = 0;
}

void CXTPSyntaxEditCtrl::CAverageVal::AddValue(UINT uVal)
{
	m_arData.SetAtGrow(m_nNextIndex, uVal);
	m_nNextIndex = (m_nNextIndex+1) % m_nDataSize;
}

UINT CXTPSyntaxEditCtrl::CAverageVal::GetAverageValue(UINT uDefaultIfNoData/* = 0*/)
{
	int nCount = (int)m_arData.GetSize();
	if (nCount <= 0)
	{
		return uDefaultIfNoData;
	}

	UINT uSumm = 0;
	for (int i = 0; i < nCount; i++) {
		uSumm += m_arData[i];
	}
	UINT uAveVal = uSumm / nCount;

	return uAveVal;
}
