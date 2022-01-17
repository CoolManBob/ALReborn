// XTPSyntaxEditPaintManager.cpp : implementation of the CXTPSyntaxEditPaintManager class.
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
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPVC50Helpers.h"

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
#include "XTPSyntaxEditCtrl.h"
#include "XTPSyntaxEditCtrl.h"
#include "XTPSyntaxEditPaintManager.h"
#include "XTPSyntaxEditBufferManager.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define XTP_SYNTAXEDIT_PRINTING_MAX_COL 5000
//===========================================================================
// CXTPSyntaxEditPaintManager class
//===========================================================================

CXTPSyntaxEditPaintManager* AFX_CDECL CXTPSyntaxEditPaintManager::Instance()
{
	static CXTPSyntaxEditPaintManager instance;
	return &instance;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPSyntaxEditPaintManager::CXTPSyntaxEditPaintManager()
{

	m_hCurLine = XTPResourceManager()->LoadCursor(XTP_IDC_EDIT_BACKARROW);
	m_hCurMove = XTPResourceManager()->LoadCursor(XTP_IDC_EDIT_MOVE);
	m_hCurCopy = XTPResourceManager()->LoadCursor(XTP_IDC_EDIT_COPY);
	m_hCurIBeam = AfxGetApp()->LoadStandardCursor(IDC_IBEAM);
	m_hCurArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hCurNO = AfxGetApp()->LoadStandardCursor(IDC_NO);

	// get non-client metrics info.
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	// create the tooltip font.
	VERIFY(SetFontToolTip(&ncm.lfStatusFont));

	// construct default fonts.
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));

	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfWeight = FW_NORMAL;
	lf.lfHeight = -13;
	STRCPY_S(lf.lfFaceName, LF_FACESIZE, _T("Courier"));

	VERIFY(CreateFontIndirect(&lf));

	// Initialize metrics
	RefreshMetrics();
	InitBitmaps();
}

CXTPSyntaxEditPaintManager::~CXTPSyntaxEditPaintManager()
{

}

void CXTPSyntaxEditPaintManager::InitBitmaps()
{
	// Check for already initialized bitmaps
	m_ilBookmark.DeleteImageList();

	// Load bookmarks bitmap
	CBitmap bmpBookMarks;       // Regular bookmark image
	if (bmpBookMarks.LoadBitmap(XTP_IDB_EDIT_BOOKMARKS))
	{
		BITMAP info;
		bmpBookMarks.GetBitmap(&info);

		if (m_ilBookmark.Create(20, info.bmHeight,
			ILC_COLOR24|ILC_MASK, 0, 1))
		{
			m_ilBookmark.Add(&bmpBookMarks, RGB(0x00,0xff,0x00));
		}
	}
}

HCURSOR CXTPSyntaxEditPaintManager::SetMoveCursor(HCURSOR hCurMove)
{
	HCURSOR hCurMoveOld = m_hCurMove;
	m_hCurMove = hCurMove;
	return hCurMoveOld;
}

HCURSOR CXTPSyntaxEditPaintManager::SetCopyCursor(HCURSOR hCurCopy)
{
	HCURSOR hCurCopyOld = m_hCurCopy;
	m_hCurCopy = hCurCopy;
	return hCurCopyOld;
}

HCURSOR CXTPSyntaxEditPaintManager::SetLineSelCursor(HCURSOR hCur)
{
	HCURSOR hCurOld = m_hCurLine;
	m_hCurLine = hCur;
	return hCurOld;
}

AFX_STATIC void GetCustomValue(CWinApp* pWinApp, LPCTSTR lpszRegKey, CXTPPaintManagerColor& pmc)
{
	COLORREF color = pWinApp->GetProfileInt(XTP_EDIT_REG_SETTINGS,
		lpszRegKey, pmc.GetStandardColor());

	if (color != pmc.GetStandardColor())
	{
		pmc = color;
	}
}

void CXTPSyntaxEditPaintManager::RefreshMetrics()
{
	// Initialize default colors.
	m_clrValues.crText.SetStandardValue(::GetSysColor(COLOR_WINDOWTEXT));
	m_clrValues.crBack.SetStandardValue(::GetSysColor(COLOR_WINDOW));
	m_clrValues.crHiliteText.SetStandardValue(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	m_clrValues.crHiliteBack.SetStandardValue(::GetSysColor(COLOR_HIGHLIGHT));
	m_clrValues.crInactiveHiliteText.SetStandardValue(::GetSysColor(COLOR_INACTIVECAPTIONTEXT));
	m_clrValues.crInactiveHiliteBack.SetStandardValue(::GetSysColor(COLOR_INACTIVECAPTION));
	m_clrValues.crLineNumberText.SetStandardValue(RGB(0x00,0x82,0x84));
	m_clrValues.crLineNumberBack.SetStandardValue(::GetSysColor(COLOR_WINDOW));

	// Restore user settings from registry.
	CWinApp* pWinApp = AfxGetApp();
	if (pWinApp != NULL)
	{
		GetCustomValue(pWinApp, XTP_EDIT_REG_TEXTCOLOR, m_clrValues.crText);
		GetCustomValue(pWinApp, XTP_EDIT_REG_BACKCOLOR, m_clrValues.crBack);
		GetCustomValue(pWinApp, XTP_EDIT_REG_SELTEXTCOLOR, m_clrValues.crHiliteText);
		GetCustomValue(pWinApp, XTP_EDIT_REG_SELBACKCOLOR, m_clrValues.crHiliteBack);
		GetCustomValue(pWinApp, XTP_EDIT_REG_INSELTEXTCOLOR, m_clrValues.crInactiveHiliteText);
		GetCustomValue(pWinApp, XTP_EDIT_REG_INSELBACKCOLOR, m_clrValues.crInactiveHiliteBack);
		GetCustomValue(pWinApp, XTP_EDIT_REG_LINENUMTEXTCOLOR, m_clrValues.crLineNumberText);
		GetCustomValue(pWinApp, XTP_EDIT_REG_LINENUMBACKCOLOR, m_clrValues.crLineNumberBack);

		UINT nSize = 0;
		LPBYTE pbtData = 0;
		if (pWinApp->GetProfileBinary(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_LOGFONT, &pbtData, &nSize))
		{
			LOGFONT* pLogFont = reinterpret_cast<LOGFONT*>(pbtData);
			CreateFontIndirect(pLogFont);
			delete [] pbtData;
		}
	}

	// Initialize gray pen.
	if (m_penGray.m_hObject)
		m_penGray.DeleteObject();
	m_penGray.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
}

//////////////////////////////////////////////////////////////////////////

CFont* CXTPSyntaxEditPaintManager::GetFont()
{
	return &m_font;
}

BOOL CXTPSyntaxEditPaintManager::CreateFontIndirect(LPLOGFONT pLogfont, BOOL bUpdateReg/*=FALSE*/)
{
	if (!pLogfont)
		return FALSE;

	if (!SetFont(pLogfont))
		return FALSE;

	if (!SetFontText(pLogfont))
		return FALSE;

	if (!SetFontLineNumber(pLogfont))
		return FALSE;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileBinary(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_LOGFONT,
				reinterpret_cast<LPBYTE>(pLogfont), sizeof(LOGFONT)))
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditPaintManager::SetFont(LPLOGFONT pLogfont)
{
	if (m_font.m_hObject)
	{
		m_font.DeleteObject();
	}
	return m_font.CreateFontIndirect(pLogfont);
}

CFont* CXTPSyntaxEditPaintManager::GetFontText()
{
	return &m_fontText;
}

BOOL CXTPSyntaxEditPaintManager::SetFontText(LPLOGFONT pLogfont)
{
	if (m_fontText.m_hObject)
	{
		m_fontText.DeleteObject();
	}
	return m_fontText.CreateFontIndirect(pLogfont);
}

CFont* CXTPSyntaxEditPaintManager::GetFontLineNumber()
{
	return &m_fontLineNumber;
}

BOOL CXTPSyntaxEditPaintManager::SetFontLineNumber(LPLOGFONT pLogfont)
{
	if (m_fontLineNumber.m_hObject)
	{
		m_fontLineNumber.DeleteObject();
	}
	return m_fontLineNumber.CreateFontIndirect(pLogfont);
}

CFont* CXTPSyntaxEditPaintManager::GetFontToolTip()
{
	return &m_fontToolTip;
}

BOOL CXTPSyntaxEditPaintManager::SetFontToolTip(LPLOGFONT pLogfont)
{
	if (m_fontToolTip.m_hObject)
	{
		m_fontToolTip.DeleteObject();
	}
	return m_fontToolTip.CreateFontIndirect(pLogfont);
}

//////////////////////////////////////////////////////////////////////////

HCURSOR CXTPSyntaxEditPaintManager::GetCurLine()
{
	return m_hCurLine;
}

HCURSOR CXTPSyntaxEditPaintManager::GetCurMove()
{
	return m_hCurMove;
}

HCURSOR CXTPSyntaxEditPaintManager::GetCurCopy()
{
	return m_hCurCopy;
}

HCURSOR CXTPSyntaxEditPaintManager::GetCurNO()
{
	return m_hCurNO;
}

HCURSOR CXTPSyntaxEditPaintManager::GetCurIBeam()
{
	return m_hCurIBeam;
}

HCURSOR CXTPSyntaxEditPaintManager::GetCurArrow()
{
	return m_hCurArrow;
}

BOOL CXTPSyntaxEditPaintManager::SetTextColor(COLORREF color, BOOL bUpdateReg/*=FALSE*/)
{
	m_clrValues.crText = color;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_TEXTCOLOR, (int)color))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditPaintManager::SetBackColor(COLORREF color, BOOL bUpdateReg/*=FALSE*/)
{
	m_clrValues.crBack = color;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_BACKCOLOR, (int)color))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditPaintManager::SetHiliteTextColor(COLORREF color, BOOL bUpdateReg/*=FALSE*/)
{
	m_clrValues.crHiliteText = color;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_SELTEXTCOLOR, (int)color))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditPaintManager::SetHiliteBackColor(COLORREF color, BOOL bUpdateReg/*=FALSE*/)
{
	m_clrValues.crHiliteBack = color;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_SELBACKCOLOR, (int)color))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditPaintManager::SetInactiveHiliteTextColor(COLORREF color, BOOL bUpdateReg/*=FALSE*/)
{
	m_clrValues.crInactiveHiliteText = color;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_INSELTEXTCOLOR, (int)color))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditPaintManager::SetInactiveHiliteBackColor(COLORREF color, BOOL bUpdateReg/*=FALSE*/)
{
	m_clrValues.crInactiveHiliteBack = color;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_INSELBACKCOLOR, (int)color))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditPaintManager::SetLineNumberTextColor(COLORREF color, BOOL bUpdateReg/*=FALSE*/)
{
	m_clrValues.crLineNumberText = color;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_LINENUMTEXTCOLOR, (int)color))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditPaintManager::SetLineNumberBackColor(COLORREF color, BOOL bUpdateReg/*=FALSE*/)
{
	m_clrValues.crLineNumberBack = color;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_LINENUMBACKCOLOR, (int)color))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
CImageList* CXTPSyntaxEditPaintManager::GetBookmarks()
{
	return &m_ilBookmark;
}

//////////////////////////////////////////////////////////////////////////

BOOL CXTPSyntaxEditPaintManager::UpdateTextFont(CXTPSyntaxEditCtrl* pEditCtrl, const XTP_EDIT_FONTOPTIONS& lf)
{
	LOGFONT lfOpt;
	m_font.GetLogFont(&lfOpt);

	if (pEditCtrl && pEditCtrl->GetSyntaxColor())
	{
		if (lf.lfItalic != XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION)
		{
			lfOpt.lfItalic = lf.lfItalic;
		}
		if (lf.lfWeight != XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION)
		{
			lfOpt.lfWeight = lf.lfWeight;
		}
		if (lf.lfUnderline != XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION)
		{
			lfOpt.lfUnderline = lf.lfUnderline;
		}
	}

	return SetFontText(&lfOpt);
}

int CXTPSyntaxEditPaintManager::DrawLineNumber(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, int nRow, int nRowActual)
{
	if (!pEditCtrl)
		return 0;

	int nStartCol = 0;

	if (nRowActual >= 1 && pEditCtrl->GetLineNumbers())
	{
		int nStartX = 0;

		if (pDC->IsPrinting())
			nStartX = pEditCtrl->m_nLeftMargin;
		else
			nStartX = (pEditCtrl->GetSelMargin()) ? pEditCtrl->m_nMarginLength : 0;

		int nLine = nRow * pEditCtrl->m_tm.tmHeight;

		if (pDC->IsPrinting())
			nLine += pEditCtrl->m_nTopMargin;

		CXTPFontDC fontDC(pDC, GetFontLineNumber());

		TEXTMETRIC tm;
		VERIFY(pDC->GetTextMetrics(&tm));
		tm.tmHeight++;

		CRect rcLineNum(nStartX, nLine,
			nStartX + pEditCtrl->m_nLineNumLength - 2,
			nLine + tm.tmHeight);

		CRect rcLineNumFull(nStartX, nLine,
			nStartX + pEditCtrl->m_nLineNumLength,
			nLine + tm.tmHeight);

		if (!pDC->IsPrinting())
			pDC->FillSolidRect(&rcLineNumFull, m_clrValues.crLineNumberBack);

		if (nRowActual <= pEditCtrl->GetRowCount())
		{
			// Print line number
			CString strLineNum;
			strLineNum.Format(_T("%d"), nRowActual);

			pDC->SetBkMode(OPAQUE);
			pDC->SetBkColor(m_clrValues.crLineNumberBack);
			pDC->SetTextColor(m_clrValues.crLineNumberText);
			pDC->DrawText(strLineNum, &rcLineNum, DT_VCENTER|DT_RIGHT);
		}

		DrawLineNumberBorder(pDC, rcLineNum, m_clrValues.crLineNumberText);

		if (pDC->IsPrinting())
		{
			int nRowNumLen = (int)log10l(pEditCtrl->GetRowCount()) + 1;
			nStartCol = nRowNumLen + 2;
		}
	}
	return nStartCol;
}

void CXTPSyntaxEditPaintManager::DrawLineNumberBorder(CDC *pDC, const CRect& rcLineNum, const COLORREF clrBorder)
{
	int nDotLineX = rcLineNum.right+1;

	for (int y = rcLineNum.top; y < rcLineNum.bottom; y++)
	{
		if (y % 2)
		{
			pDC->SetPixelV(nDotLineX, y, clrBorder);
		}
	}
}

void CXTPSyntaxEditPaintManager::DrawLineNode(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, int nRow, const DWORD& dwType)
{
	if (!pEditCtrl || !pEditCtrl->m_bDrawNodes)
		return;

	// calculate node rect
	CRect rcNode, rcNodeFull;
	pEditCtrl->GetLineNodeRect(nRow, rcNode, &rcNodeFull);

	// fill node background
	pDC->FillSolidRect(&rcNodeFull, m_clrValues.crBack);

	// draw node picture
	if (dwType != XTP_EDIT_ROWNODE_NOTHING)
	{
		CXTPPenDC penGray(pDC,
			&m_penGray);

		if (dwType & XTP_EDIT_ROWNODE_NODEUP)
		{
			// draw up line
			CPoint ptEnd(rcNode.CenterPoint().x, rcNodeFull.top);
			pDC->MoveTo(ptEnd);
			ptEnd.y += rcNodeFull.Height() / 2;
			pDC->LineTo(ptEnd);
		}

		if (dwType & XTP_EDIT_ROWNODE_NODEDOWN)
		{
			// draw down line
			CPoint ptEnd(rcNode.CenterPoint().x, rcNodeFull.bottom);
			pDC->MoveTo(ptEnd);
			ptEnd.y -= rcNodeFull.Height() / 2 + 2;
			pDC->LineTo(ptEnd);
		}

		if (dwType & XTP_EDIT_ROWNODE_ENDMARK)
		{
			// draw down line
			CPoint ptEnd(rcNode.CenterPoint());
			pDC->MoveTo(ptEnd);
			ptEnd.x += rcNodeFull.Width() / 2;
			pDC->LineTo(ptEnd);
		}

		if (dwType & XTP_EDIT_ROWNODE_COLLAPSED ||
			dwType & XTP_EDIT_ROWNODE_EXPANDED)
		{
			// draw collapsed sign
			pDC->Rectangle(&rcNode);
			CPoint ptSign(rcNode.CenterPoint());
			ptSign.x -= 2;
			pDC->MoveTo(ptSign);
			ptSign.x += 5;
			pDC->LineTo(ptSign);
		}

		if (dwType & XTP_EDIT_ROWNODE_COLLAPSED)
		{
			// draw vertical line of the expanded sign
			CPoint ptSign(rcNode.CenterPoint());
			ptSign.y -= 2;
			pDC->MoveTo(ptSign);
			ptSign.y += 5;
			pDC->LineTo(ptSign);

		}
	}
}

void CXTPSyntaxEditPaintManager::DrawCollapsedTextMarks(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC)
{
	COLORREF clrFrame = GetSysColor(COLOR_BTNSHADOW);
	COLORREF crText = GetSysColor(COLOR_BTNSHADOW);

	CXTPFontDC fontDC(pDC, GetFont());

	const XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk = NULL;
	XTP_EDIT_LMPARAM LMCoParam;
	int nActualRow = 0;

	for (int i = 0; i < pEditCtrl->m_nCollapsedTextRowsCount; i++)
	{
		// get next collapsed row
		int nRow = pEditCtrl->m_arCollapsedTextRows[i];

		if (nRow <= nActualRow)
		{
			continue;
		}

		if (!pEditCtrl->HasRowMark(nRow, xtpEditLMT_Collapsed, &LMCoParam))
		{
			continue;
		}

		// get count of collapsed rows under this row
		int nHiddenRows = 0;
		if (!pEditCtrl->GetCollapsedBlockLen(nRow, nHiddenRows))
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

		CString strText = pCoDrawBlk->collBlock.strCollapsedText; // "[..]"
		CRect rcFrame(pCoDrawBlk->rcCollMark);
		rcFrame.InflateRect(0, 0, 0, 1);

		CRect rcText = pCoDrawBlk->rcCollMark;
		rcText.DeflateRect(1, 0, 1, 0);

		pDC->FillSolidRect(&rcText, m_clrValues.crBack);

		pDC->SetTextColor(crText);
		pDC->SetBkColor(m_clrValues.crBack);

		pDC->DrawText(strText, &rcText, 0);

		pDC->Draw3dRect(&rcFrame, clrFrame, clrFrame);
	}
}

void CXTPSyntaxEditPaintManager::DrawLineCalcSel(const XTP_EDIT_TEXTBLOCK& txtBlk,
									  int nSelStartX, int nSelEndX,
									  XTP_EDIT_TEXTBLOCK (&arTxtBlk)[4], BOOL (&bUseBlk)[4] )
{
	ASSERT(nSelStartX < nSelEndX);

	if (txtBlk.nPos < nSelStartX && txtBlk.nNextBlockPos-1 >= nSelStartX &&
		txtBlk.nNextBlockPos <= nSelEndX)
	{
		arTxtBlk[1] = arTxtBlk[2] = txtBlk;

		arTxtBlk[1].nNextBlockPos = nSelStartX;
		arTxtBlk[2].nPos = nSelStartX;

		bUseBlk[1] = bUseBlk[2] = TRUE;
	}
	else if (txtBlk.nPos >= nSelStartX && txtBlk.nPos < nSelEndX &&
		txtBlk.nNextBlockPos > nSelEndX)
	{
		arTxtBlk[2] = arTxtBlk[3] = txtBlk;

		arTxtBlk[2].nNextBlockPos = nSelEndX;
		arTxtBlk[3].nPos = nSelEndX;

		bUseBlk[2] = bUseBlk[3] = TRUE;
	}
	else if (txtBlk.nPos >= nSelStartX && txtBlk.nNextBlockPos <= nSelEndX)
	{
		arTxtBlk[2] = txtBlk;
		bUseBlk[2] = TRUE;
	}
	else if (txtBlk.nPos < nSelStartX && txtBlk.nNextBlockPos > nSelEndX)
	{
		arTxtBlk[1] = arTxtBlk[2] = arTxtBlk[3] = txtBlk;

		arTxtBlk[1].nNextBlockPos = nSelStartX;
		arTxtBlk[2].nPos = nSelStartX;
		arTxtBlk[2].nNextBlockPos = nSelEndX;
		arTxtBlk[3].nPos = nSelEndX;

		bUseBlk[1] = bUseBlk[2] = bUseBlk[3] = TRUE;
	}
	else
	{
		ASSERT(txtBlk.nPos >= nSelEndX || txtBlk.nNextBlockPos <= nSelStartX);

		arTxtBlk[1] = txtBlk;
		bUseBlk[1] = TRUE;
	}
}

void CXTPSyntaxEditPaintManager::DrawLineMark(CXTPSyntaxEditCtrl* pEditCtrl, XTP_EDIT_SENMBOOKMARK* pBookmark)
{
	enum { bookMark, bookMarkSel, breakpoint };

	POINT ptStart;
	ptStart.x = pBookmark->rcBookmark.left;
	ptStart.y = pBookmark->rcBookmark.top;

	SIZE szRect;
	szRect.cx = pBookmark->rcBookmark.right - pBookmark->rcBookmark.left;
	szRect.cy = pBookmark->rcBookmark.bottom - pBookmark->rcBookmark.top;

	CDC *pDC = CDC::FromHandle(pBookmark->hDC);

	IMAGEINFO bmpInfo;
	ZeroMemory(&bmpInfo, sizeof(bmpInfo));

	if (pEditCtrl->HasRowMark(pBookmark->nRow, xtpEditLMT_Bookmark))
	{
		int iImage = (pEditCtrl->GetCurrentDocumentRow() == pBookmark->nRow)? bookMarkSel: bookMark;
		if (GetBookmarks()->GetImageInfo(iImage, &bmpInfo))
		{
			CRect rcBmp(bmpInfo.rcImage);
			szRect.cx = min(szRect.cx, rcBmp.Width());
			szRect.cy = min(szRect.cy, rcBmp.Height());
		}

		IMAGELISTDRAWINDIRECT_S(GetBookmarks(), pDC, iImage, ptStart, szRect);
	}

	if (pEditCtrl->HasRowMark(pBookmark->nRow, xtpEditLMT_Breakpoint))
	{
		if (GetBookmarks()->GetImageInfo(breakpoint, &bmpInfo))
		{
			CRect rcBmp(bmpInfo.rcImage);
			szRect.cx = min(szRect.cx, rcBmp.Width());
			szRect.cy = min(szRect.cy, rcBmp.Height());
		}

		IMAGELISTDRAWINDIRECT_S(GetBookmarks(), pDC, breakpoint, ptStart, szRect);

		pEditCtrl->SetRowBkColor(pBookmark->nRow, RGB(128,0,0));
		pEditCtrl->SetRowColor(pBookmark->nRow, RGB(255,255,255));
	}
}

void CXTPSyntaxEditPaintManager::DrawLineMarks(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, int nRow, int nRowActual)
{
	// NMHDR codes
	XTP_EDIT_SENMBOOKMARK bookmark;
	bookmark.nmhdr.code = XTP_EDIT_NM_DRAWBOOKMARK;
	bookmark.nmhdr.hwndFrom = pEditCtrl->GetSafeHwnd();
	bookmark.nmhdr.idFrom = 0;

	bookmark.hDC = pDC->GetSafeHdc();

	int iYPos = nRow * pEditCtrl->m_tm.tmHeight;

	bookmark.rcBookmark = CRect(0, iYPos,
		pEditCtrl->GetSelMargin() ? pEditCtrl->m_nMarginLength : -1,
		iYPos + pEditCtrl->m_tm.tmHeight);
	bookmark.nRow = nRowActual;

	CRect rcLine = bookmark.rcBookmark;
	rcLine.left = rcLine.right-1;

	pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
	pDC->SetTextColor(::GetSysColor(COLOR_BTNFACE));
	pDC->FillRect(&bookmark.rcBookmark, pDC->GetHalftoneBrush());
	pDC->FillSolidRect(rcLine, ::GetSysColor(COLOR_3DSHADOW));

	BOOL bHandled = FALSE;
	CWnd *pParent = pEditCtrl->GetParent();
	if (pParent)
	{
		bHandled = (BOOL)pParent->SendMessage(WM_NOTIFY, 0, (LPARAM)&bookmark);
	}

	if (!bHandled)
	{
		DrawLineMark(pEditCtrl, &bookmark);
	}
}


int CXTPSyntaxEditPaintManager::DrawLineText(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, const CString& strText, int nTextLen,
								  const XTP_EDIT_TEXTBLOCK& block, CFont** ppPrevFont,
								  int iLine, int iStartCol, int iCol)
{
	UNREFERENCED_PARAMETER(nTextLen);
	UNREFERENCED_PARAMETER(ppPrevFont);

	int nPrintTextLen = block.nNextBlockPos - block.nPos;
	CString strPrintText(strText.Mid(block.nPos, nPrintTextLen));
	strPrintText += '\0';

	CString strPrintText2;

	int nShift = pEditCtrl->ExpandChars(pDC, strPrintText, (iCol - iStartCol), strPrintText2);
	int nOutCol = (iCol - pEditCtrl->m_siHorz.nPos) * pEditCtrl->m_tm.tmAveCharWidth;

	if (pDC->IsPrinting())
	{
		nOutCol += pEditCtrl->m_nLeftMargin;
	}

	int nTxtWidth = strPrintText2.GetLength() * pEditCtrl->m_tm.tmAveCharWidth;
	int nTxtBottom = iLine + pEditCtrl->m_tm.tmHeight;
	CRect rcBGex(nOutCol, nTxtBottom - 1, nOutCol + nTxtWidth, nTxtBottom);

	//------ Set font options ------------------
	UpdateTextFont(pEditCtrl, block.lf);
	CXTPFontDC fontDC(pDC, GetFontText());

	//=============================== text out =======================
	CXTPClientRect rcClient(pEditCtrl);
	int nPageMaxChars = max(10, rcClient.Width() / max(pEditCtrl->m_tm.tmAveCharWidth, 1)+1);
	if (pDC->IsPrinting())
		nPageMaxChars = XTP_SYNTAXEDIT_PRINTING_MAX_COL;
	int nOutTextLen = min(nPageMaxChars, strPrintText2.GetLength());

	LPCTSTR pOutText = (LPCTSTR)strPrintText2;
	if (nOutCol < 0)
	{
		int nPos = -1*nOutCol / pEditCtrl->m_tm.tmAveCharWidth;
		pOutText = _tcsninc(pOutText, nPos);
		nOutTextLen = min(nPageMaxChars, strPrintText2.GetLength() - nPos);

		nOutCol = 0;
	}

	pDC->FillSolidRect(&rcBGex, pDC->GetBkColor());

	pDC->TextOut(nOutCol, iLine, pOutText, nOutTextLen);

	return nShift;
}

void CXTPSyntaxEditPaintManager::DrawLine(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, const CString& strText, int iLine,
							 int nRow, int iStartCol)
{
	int iStartColX = iStartCol * pEditCtrl->m_tm.tmAveCharWidth;

	CXTPClientRect rcWnd(pEditCtrl);

	CRect rcText(CPoint(iStartColX, iLine),
		CSize((rcWnd.Width() * pEditCtrl->m_tm.tmAveCharWidth),
		pEditCtrl->m_tm.tmHeight));

	COLORREF crBreakText;
	BOOL bBreakText = pEditCtrl->m_mapRowColor.Lookup(nRow, crBreakText);

	COLORREF crBreakBack;
	BOOL bBreakBack  = pEditCtrl->m_mapRowBkColor.Lookup(nRow, crBreakBack);

	CXTPSyntaxEditLexTextSchema* ptrTxtSch = pEditCtrl->m_pBuffer->GetLexParser()->GetTextSchema();

	if (!bBreakBack)
	{
		crBreakBack = m_clrValues.crBack;

//      if (ptrTxtSch)
//      {
//          CXTPSyntaxEditLexTextBlockPtr ptrFileTB = ptrTxtSch->GetBlocks();
//          if (ptrFileTB && ptrFileTB->m_ptrLexClass)
//          {
//              XTP_EDIT_TEXTBLOCK fileTB;
//              fileTB.clrBlock = m_clrValues;
//              ptrFileTB->m_ptrLexClass->GetTextAttributes(fileTB);
//
//              crBreakBack = fileTB.clrBlock.crBack;
//          }
//      }
	}

	if (!pDC->IsPrinting())
	{
		pDC->FillSolidRect(&rcText, crBreakBack);
	}

	if (nRow > pEditCtrl->GetRowCount())
	{
		return;
	}

	pDC->SetBkMode(OPAQUE);
	pDC->SetTextColor(m_clrValues.crText);

	int nTextLen = (int)_tcslen(strText);

	CXTPSyntaxEditTextBlockList blocks;

	if (ptrTxtSch)
	{
		CXTPSyntaxEditLexTextBlock* pScreenSchFirstTB = pEditCtrl->GetOnScreenSch(nRow);
		//CXTPSyntaxEditLexTextBlock* pScreenSchFirstTB = ptrTxtSch->GetBlocks();
		if (pScreenSchFirstTB)
		{
			CXTPSyntaxEditTextIterator txtIter(pEditCtrl->m_pBuffer);

			int nColFrom = pEditCtrl->GetEditBuffer()->ColToStrPos(nRow, pEditCtrl->m_siHorz.nPos);
			int nCol2 = rcWnd.Width() / pEditCtrl->m_tm.tmAveCharWidth;
			if (pDC->IsPrinting())
				nCol2 = XTP_SYNTAXEDIT_PRINTING_MAX_COL;
			nCol2 = pEditCtrl->GetEditBuffer()->ColToStrPos(nRow, pEditCtrl->m_siHorz.nPos + nCol2);
			nCol2 += 5;

			ptrTxtSch->GetRowColors(&txtIter, nRow, nColFrom, nCol2,
				m_clrValues, blocks,
				NULL, pScreenSchFirstTB);
		}
	}

	if (blocks.GetCount() == 0)
	{
		XTP_EDIT_TEXTBLOCK defBlk;
		defBlk.nPos = 0;
		defBlk.nNextBlockPos = nTextLen;
		defBlk.clrBlock = m_clrValues;
		blocks.AddTail(defBlk);
	}

	// draw text
	BOOL bRowInSel = FALSE;
	int nSelStartX = -1, nSelEndX = -1;

	int iCol = iStartCol;

	CPoint ptStart = pEditCtrl->m_ptStartSel;
	CPoint ptEnd   = pEditCtrl->m_ptEndSel;

	int iMinX = 0, iMaxX = 0;

	if (pEditCtrl->m_bSelectionExist && !pDC->IsPrinting())
	{
		if (ptEnd.y < ptStart.y)
		{
			// swap start and end points
			CPoint temp = ptEnd;
			ptEnd = ptStart;
			ptStart = temp;
		}

		if ((ptEnd.x < ptStart.x) && (ptStart.y == ptEnd.y))
		{
			// Swap coordinate points - make good rectangle
			int temp = ptEnd.x;
			ptEnd.x = ptStart.x;
			ptStart.x = temp;
		}

		bRowInSel = (nRow >= ptStart.y && nRow <= ptEnd.y);

		if (ptStart.y == nRow && !pEditCtrl->m_bColumnSelect)   // Selection starts in this row
		{
			nSelStartX = pEditCtrl->CalcAbsCol(strText, ptStart.x)-1;
		}
		else
		{
			nSelStartX = 0;
		}

		if (ptEnd.y == nRow && !pEditCtrl->m_bColumnSelect) // Select ends in this row
		{
			nSelEndX = pEditCtrl->CalcAbsCol(strText, ptEnd.x)-1;
		}
		else
		{
			nSelEndX = nTextLen;
		}

		if (pEditCtrl->m_bColumnSelect)
		{
			iMinX = nSelStartX = min(ptStart.x, ptEnd.x);
			iMaxX = nSelEndX = max(ptStart.x, ptEnd.x);

			nSelStartX = pEditCtrl->CalcAbsCol(strText, nSelStartX) - 1;
			nSelEndX = pEditCtrl->CalcAbsCol(strText, nSelEndX) - 1;
		}
	}

	if (nSelStartX == nSelEndX)
	{
		bRowInSel = FALSE;
	}

	//=======================================================================
	BOOL bCoBlkDrawn = FALSE;
	XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk = NULL;
	XTP_EDIT_LMPARAM LMCoParam;
	if (pEditCtrl->HasRowMark(nRow, xtpEditLMT_Collapsed, &LMCoParam))
	{
		pCoDrawBlk = (XTP_EDIT_COLLAPSEDBLOCK*)LMCoParam.GetPtr();
		ASSERT(pCoDrawBlk);
	}

	//===============================================================
	POSITION pos = blocks.GetHeadPosition();
	while (pos)
	{
		const XTP_EDIT_TEXTBLOCK& txtBlk = blocks.GetNext(pos);

		// draw [...] if needed
		if (pCoDrawBlk)
		{
			bCoBlkDrawn = pEditCtrl->ProcessCollapsedTextEx(pDC, pCoDrawBlk, txtBlk, iLine, iCol);
			if (bCoBlkDrawn)
			{
				break;
			}
		}

		// define default colors.
		COLORREF crBack    = m_clrValues.crBack.GetStandardColor();
		COLORREF crText    = m_clrValues.crText.GetStandardColor();
		COLORREF crHiliteBack = m_clrValues.crHiliteBack.GetStandardColor();
		COLORREF crHiliteText = m_clrValues.crHiliteText.GetStandardColor();

		// if syntax coloring is enabled use defined colors.
		if (pEditCtrl->GetSyntaxColor())
		{
			crBack    = txtBlk.clrBlock.crBack;
			crText    = txtBlk.clrBlock.crText;
			crHiliteBack = txtBlk.clrBlock.crHiliteBack;
			crHiliteText = txtBlk.clrBlock.crHiliteText;
		}

		if (bBreakBack)
		{
			crBack = crBreakBack;
		}

		if (bBreakText)
		{
			crText = crBreakText;
		}

		if (pEditCtrl->GetSyntaxColor())
		{
			// if the color is the same as the default
			// color, make sure the color is set to the
			// user defined color.

			if (crBack == m_clrValues.crBack.GetStandardColor())
				crBack = m_clrValues.crBack;

			if (crText == m_clrValues.crText.GetStandardColor())
				crText = m_clrValues.crText;

			if (!pEditCtrl->IsActive())
			{
				if (crHiliteBack == m_clrValues.crHiliteBack.GetStandardColor())
					crHiliteBack = m_clrValues.crInactiveHiliteBack;

				if (crHiliteText == m_clrValues.crHiliteText.GetStandardColor())
					crHiliteText = m_clrValues.crInactiveHiliteText;
			}
		}

		XTP_EDIT_TEXTBLOCK arTxtBlk[4];
		BOOL bUseBlk[4] = {FALSE, FALSE, FALSE, FALSE};

		if (bRowInSel && !pDC->IsPrinting() && nTextLen)
		{
			ASSERT(nSelStartX >= 0 && nSelStartX < nTextLen);
			ASSERT(nSelEndX >= 0 && nSelEndX <= nTextLen );
			ASSERT(nSelStartX < nSelEndX);

			DrawLineCalcSel(txtBlk, nSelStartX, nSelEndX, arTxtBlk, bUseBlk);
		}
		else if (nTextLen)
		{
			arTxtBlk[1] = txtBlk;
			bUseBlk[1] = TRUE;
		}

		//**************************************************
		for (int i = 1; i <= 3; i++)
		{
			if (!bUseBlk[i])
			{
				continue;
			}

			pDC->SetTextColor((i%2) ? crText : crHiliteText);
			pDC->SetBkColor((i%2) ? crBack : crHiliteBack);

			int nShift = DrawLineText(pEditCtrl, pDC, strText, nTextLen, arTxtBlk[i],
				NULL, iLine, iStartCol, iCol);

			iCol += nShift;
		}
	}

	//===========================================================================
	// draw [...] if needed and not yet
	if (pCoDrawBlk && !bCoBlkDrawn)
	{
		pEditCtrl->ProcessCollapsedText(pDC, pCoDrawBlk, iLine, iCol);
	}

	//===========================================================================
	if (pEditCtrl->m_bSelectionExist &&
		!pDC->IsPrinting() &&
		nRow < ptEnd.y && nRow >= ptStart.y &&
		!pEditCtrl->m_bColumnSelect)
	{
		int nHorzPos = pEditCtrl->m_siHorz.nPos;

		int iActualCol = nTextLen + 1;

		int iDispCol = pEditCtrl->CalcDispCol(strText, iActualCol);

		CRect rcExtra;
		rcExtra.left = (iStartCol * pEditCtrl->m_tm.tmAveCharWidth) +
						((iDispCol - nHorzPos - 1) * pEditCtrl->m_tm.tmAveCharWidth) ;  // Getting the actual XPOS
		rcExtra.right = (rcExtra.left + (pEditCtrl->m_tm.tmAveCharWidth * 1));
		rcExtra.top = iLine;
		rcExtra.bottom = iLine + pEditCtrl->m_tm.tmHeight;

		COLORREF crHiliteBack = m_clrValues.crHiliteBack;
		if (!pEditCtrl->IsActive())
		{
			if (crHiliteBack == m_clrValues.crHiliteBack.GetStandardColor())
				crHiliteBack = m_clrValues.crInactiveHiliteBack;
		}

		pDC->FillSolidRect(&rcExtra, crHiliteBack);
	}

	bRowInSel = (nRow >= ptStart.y && nRow <= ptEnd.y);

	// Draw selected space after the end of the line if needed
	int iEndDispCol = pEditCtrl->CalcDispCol(strText, nSelEndX + 1);
	if (pEditCtrl->m_bColumnSelect &&
		!pDC->IsPrinting()
		&& bRowInSel
		&& iEndDispCol < iMaxX)
	{
		int nStartPos = max(iEndDispCol, iMinX);
		int nEndPos = iMaxX;

		int nHorzPos = pEditCtrl->m_siHorz.nPos;

		CRect rcExtra;
		rcExtra.left = ((nStartPos - 1 - nHorzPos) * pEditCtrl->m_tm.tmAveCharWidth);
		rcExtra.right = ((nEndPos - 1) * pEditCtrl->m_tm.tmAveCharWidth);
		rcExtra.top = iLine;
		rcExtra.bottom = iLine + pEditCtrl->m_tm.tmHeight;

		COLORREF crHiliteBack = m_clrValues.crHiliteBack;
		if (!pEditCtrl->IsActive())
		{
			if (crHiliteBack == m_clrValues.crHiliteBack.GetStandardColor())
				crHiliteBack = m_clrValues.crInactiveHiliteBack;
		}

		pDC->FillSolidRect(&rcExtra, crHiliteBack);
	}

	pDC->SetTextColor(m_clrValues.crText);
}
