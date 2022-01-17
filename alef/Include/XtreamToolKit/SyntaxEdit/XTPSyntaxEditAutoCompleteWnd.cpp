// XTPSyntaxEditAutoCompleteWnd.cpp: implementation of the CXTPSyntaxEditAutoCompleteWnd class.
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
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"

// common includes
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"

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

#include <search.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTP_SYNTAXEDIT_AUTOCOMPLETE_HEIGHT_MAX  160
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CXTPSyntaxEditAutoCompleteWnd, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_VSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CXTPSyntaxEditAutoCompleteWnd::CXTPSyntaxEditAutoCompleteWnd()
{
	RegisterWindowClass();

	m_strSearch = _T("");
	//m_strSelected = _T("");
	m_pParentWnd = NULL;
	m_nLineHeight = 0;
	m_nLines = 0;
	m_nWndHeight = 0;
	m_nHighLightLine = -1;
	m_nFirstDisplayedStr = 0;
	m_nBordersHeight = 0;
	m_bActive = FALSE;

	m_strDelims = _T("");
	VERIFY( m_ilACGlyphs.Create(XTP_IDB_EDIT_GLYPHS, 16, 16, RGB(255,255,255)) );
}

CXTPSyntaxEditAutoCompleteWnd::~CXTPSyntaxEditAutoCompleteWnd()
{
	RemoveAll();
	DestroyWindow();
}

BOOL CXTPSyntaxEditAutoCompleteWnd::RegisterWindowClass(HINSTANCE hInstance /*= NULL*/)
{
	WNDCLASS wndcls;
	if (hInstance == NULL) hInstance = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInstance, XTP_EDIT_CLASSNAME_AUTOCOMPLETEWND, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style = CS_SAVEBITS | CS_DBLCLKS;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInstance;
		wndcls.hIcon = NULL;
		wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = XTP_EDIT_CLASSNAME_AUTOCOMPLETEWND;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CXTPSyntaxEditAutoCompleteWnd::Create(CWnd* pParentWnd)
{
	if (!CWnd::CreateEx(WS_EX_TOOLWINDOW, XTP_EDIT_CLASSNAME_AUTOCOMPLETEWND, NULL,
		WS_CHILD | WS_DLGFRAME | WS_VSCROLL | WS_DISABLED, CRect(0,0,160,160),
		GetDesktopWindow(), NULL))
	{
		TRACE0("Failed to create Auto Complete View.\n");
		return FALSE;
	}

	CXTPClientRect rcClient(this);
	CXTPWindowRect rcWin(this);
	m_nBordersHeight = (rcWin.Height()-rcClient.Height());
	m_pParentWnd = DYNAMIC_DOWNCAST(CXTPSyntaxEditCtrl, pParentWnd);

	return TRUE;
}

void CXTPSyntaxEditAutoCompleteWnd::Show(CPoint pt, CString strSearch)
{
	if (Filter(strSearch) < 1)
	{
		return;
	}
	m_nFirstDisplayedStr = 0;
	int nSearch = Search(strSearch);
	m_nHighLightLine = nSearch;

	ScrollTo(nSearch);

	RefreshMetrics();

	BOOL bVScrollEnabled = AdjusLayout();

	SetWindowPos(NULL, pt.x - 21, pt.y, 160, m_nWndHeight, SWP_SHOWWINDOW);
	ShowScrollBar(SB_VERT, bVScrollEnabled);

	RedrawWindow();

	EnableWindow(TRUE);
	SetFocus();

	m_bActive = TRUE;
	int nSearchLen = strSearch.GetLength();
	m_bFilteredMode = nSearchLen > 0;
	m_nStartReplacePos = m_pParentWnd->GetCurCol() - nSearchLen;
	m_nEndReplacePos = m_nStartReplacePos + nSearchLen;
	m_strSearch = strSearch;
}

void CXTPSyntaxEditAutoCompleteWnd::OnLButtonDown(UINT nFlags, CPoint point)
{

	CWnd::OnLButtonDown(nFlags, point);

	int nHitLine = HitTest(point);
	if (nHitLine > -1)
	{   m_bHighLight = TRUE;
		m_nHighLightLine = m_nFirstDisplayedStr + nHitLine;
		RedrawWindow();
	}

}

void CXTPSyntaxEditAutoCompleteWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
	int nHitLine = HitTest(point);
	if (nHitLine > -1)
	{
		m_nHighLightLine = m_nFirstDisplayedStr + nHitLine;
		RedrawWindow();
	}
}

void CXTPSyntaxEditAutoCompleteWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);
	m_bHighLight = TRUE;
	ReturnSelected(FALSE);
	Hide();
}

void CXTPSyntaxEditAutoCompleteWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nDataMax = (int)m_arrACDataFiltered.GetSize();
	int nPage = m_nLines - 1;
	int nLastDisp = m_nFirstDisplayedStr + nPage;
	m_bHighLight = TRUE;
	switch(nChar)
	{
	case VK_UP:
		m_nHighLightLine -= nRepCnt;

		if (m_nHighLightLine >= 0)
		{
			if (m_nFirstDisplayedStr > m_nHighLightLine && ScrollTo(m_nHighLightLine))
			{
					SetScrollPos(SB_VERT, m_nFirstDisplayedStr, TRUE);
			}

			RedrawWindow();
		}
		else
		{
			m_nHighLightLine = 0;
		}

		break;
	case VK_DOWN:
		m_nHighLightLine += nRepCnt;
		if (m_nHighLightLine < nDataMax)
		{
			int nL = m_nLines - 1;
			nLastDisp = m_nFirstDisplayedStr + nL;
			if (nLastDisp < m_nHighLightLine)
			{
				if (ScrollTo(m_nHighLightLine - nL))
				{
					SetScrollPos(SB_VERT, m_nFirstDisplayedStr, TRUE);
				}
			}

			RedrawWindow();
		}
		else
		{
			m_nHighLightLine = nDataMax - 1;
		}
		break;
	case VK_PRIOR:
		if (m_nFirstDisplayedStr < m_nHighLightLine)
		{
			m_nHighLightLine = m_nFirstDisplayedStr;
		}
		else
		{
			if (m_nHighLightLine > 0)
				m_nHighLightLine = max(0, m_nHighLightLine - m_nLines + 1);
			else
				m_nHighLightLine -= m_nLines + 1;
		}

		if (m_nHighLightLine >= 0)
		{
			if (m_nFirstDisplayedStr > m_nHighLightLine && ScrollTo(m_nHighLightLine))
			{
					SetScrollPos(SB_VERT, m_nFirstDisplayedStr, TRUE);
			}

			RedrawWindow();
		}
		else
		{
			m_nHighLightLine = 0;
		}
		break;
	case VK_NEXT:

		if (m_nFirstDisplayedStr + nPage > m_nHighLightLine)
		{
			m_nHighLightLine = m_nFirstDisplayedStr + nPage;
		}
		else
		{
			if (m_nHighLightLine < nDataMax)
				m_nHighLightLine = min(nDataMax - 1, m_nHighLightLine + nPage);
			else
				m_nHighLightLine += nPage;
		}

		if (m_nHighLightLine < nDataMax)
		{
			if (nLastDisp < m_nHighLightLine)
			{
				if (ScrollTo(m_nHighLightLine - nPage))
				{
					SetScrollPos(SB_VERT, m_nFirstDisplayedStr, TRUE);
				}
			}

			RedrawWindow();
		}
		else
		{
			m_nHighLightLine = nDataMax - 1;
		}

		break;
	case VK_END:
		if (m_nHighLightLine != nDataMax - 1 && ScrollTo(nDataMax - m_nLines))
		{
			SetScrollPos(SB_VERT, m_nFirstDisplayedStr, TRUE);
			m_nHighLightLine = nDataMax - 1;
			RedrawWindow();
		}
		break;
	case VK_HOME:
		if (m_nHighLightLine != 0 && ScrollTo(0))
		{
			SetScrollPos(SB_VERT, m_nFirstDisplayedStr, TRUE);
			m_nHighLightLine = 0;
			RedrawWindow();
		}
		break;
	case VK_BACK:

		m_nEndReplacePos--;
		if (m_strSearch.GetLength() > 0)
			m_strSearch = m_strSearch.Left(m_strSearch.GetLength() - 1);

		if (m_nStartReplacePos >= m_nEndReplacePos)
		{
			Hide();
			m_pParentWnd->SendMessage(WM_LBUTTONUP, 0, 0);
		}
		break;
	case VK_LEFT :
	case VK_RIGHT :
	case VK_MENU :
	case VK_CONTROL:
	case VK_ESCAPE:
		Hide();
		break;
	case VK_RETURN:
		ReturnSelected(FALSE);
		Hide();
		break;
	case VK_SPACE:
		if (!m_bFilteredMode)
			ReturnSelected(FALSE);
		Hide();
		break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CXTPSyntaxEditAutoCompleteWnd::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{

	if (IsCloseTag((TCHAR)nChar))
	{
		ReturnSelected(FALSE);
		Hide();
	}

	if (isalnum((TCHAR)nChar))
	{
		m_nEndReplacePos++;
		m_strSearch += (TCHAR)nChar;
		int nFound = Search(m_strSearch);
		if (nFound >= 0)
		{
			m_nHighLightLine = nFound;
			m_bHighLight = TRUE;
			if (m_nFirstDisplayedStr > m_nHighLightLine)
			{
				if (ScrollTo(m_nHighLightLine))
					SetScrollPos(SB_VERT, m_nFirstDisplayedStr, TRUE);
			}
			if (m_nFirstDisplayedStr + m_nLines <= m_nHighLightLine)
			{
				if (ScrollTo(m_nHighLightLine - m_nLines + 1))
					SetScrollPos(SB_VERT, m_nFirstDisplayedStr, TRUE);
			}
			RedrawWindow();
			if (m_bFilteredMode && m_arrACDataFiltered.GetAt(m_nHighLightLine)->m_strText.CompareNoCase(m_strSearch) == 0)
			{
				ReturnSelected(TRUE);
				Hide();

			}

		}
		else if (m_bFilteredMode)
		{
			Hide();
		}
		else
		{
			m_bHighLight = FALSE;
				RedrawWindow();
		}
	}

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CXTPSyntaxEditAutoCompleteWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UNREFERENCED_PARAMETER(nPos);
	UNREFERENCED_PARAMETER(pScrollBar);

	SCROLLINFO info;
	GetScrollInfo(SB_VERT, &info);

	switch(nSBCode) {
	case SB_LINEDOWN:
		if (ScrollTo(m_nFirstDisplayedStr + 1))
		{
			SetScrollPos(SB_VERT, m_nFirstDisplayedStr);
			RedrawWindow();
		}

		break;
	case SB_LINEUP:
		if (ScrollTo(m_nFirstDisplayedStr - 1))
		{
			SetScrollPos(SB_VERT, m_nFirstDisplayedStr);
			RedrawWindow();
		}

		break;
	case SB_PAGEDOWN:
		if (ScrollTo(m_nFirstDisplayedStr + m_nLines - 1))
		{
			SetScrollPos(SB_VERT, m_nFirstDisplayedStr);
			RedrawWindow();
		}
		break;
	case SB_PAGEUP:
		if (ScrollTo(m_nFirstDisplayedStr - m_nLines + 1))
		{
			SetScrollPos(SB_VERT, m_nFirstDisplayedStr);
			RedrawWindow();
		}
		break;
	case SB_TOP:
		SetScrollPos(SB_VERT, info.nMin);
		RedrawWindow();
		break;
	case SB_BOTTOM:
		SetScrollPos(SB_VERT, info.nMax);
		RedrawWindow();
		break;
	case SB_THUMBTRACK:
		{
			SCROLLINFO si;
			ZeroMemory(&si, sizeof(SCROLLINFO));
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_TRACKPOS;

			if (GetScrollInfo(SB_VERT, &si))
			{
				if (si.nTrackPos != m_nFirstDisplayedStr && ScrollTo(si.nTrackPos))
				{
					SetScrollPos(SB_VERT, si.nTrackPos);
					RedrawWindow();
				}

			}
		}
		break;
	case SB_ENDSCROLL:
		return;
	default:
		break;
	}
}


BOOL CXTPSyntaxEditAutoCompleteWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(nHitTest);
	UNREFERENCED_PARAMETER(pWnd);
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return TRUE;
}

BOOL CXTPSyntaxEditAutoCompleteWnd::AdjusLayout()
{

	CWindowDC dc(NULL);
	CXTPFontDC fontDC(&dc, &m_fontBasic);

	int nDataLines = (int)m_arrACDataFiltered.GetSize();
	m_nLineHeight = dc.GetTextExtent(_T("A"), 1).cy + 6;

	if (nDataLines * m_nLineHeight < XTP_SYNTAXEDIT_AUTOCOMPLETE_HEIGHT_MAX)
	{
		m_nLines = nDataLines;
	}
	else
	{
		m_nLines = (int) XTP_SYNTAXEDIT_AUTOCOMPLETE_HEIGHT_MAX / m_nLineHeight;
	}

	BOOL bVScrollEnabled = nDataLines > m_nLines;

	ShowScrollBar(SB_VERT, bVScrollEnabled);
	EnableScrollBarCtrl(SB_VERT, bVScrollEnabled);

	//--------------------------------------------------
	m_nWndHeight = m_nLines * m_nLineHeight + m_nBordersHeight;

	CXTPClientRect rcClient(this);

	CRect rc(0,0, rcClient.Width(), m_nLineHeight);
	m_arrGrid.RemoveAll();

	for (int nLine = 0; nLine < m_nLines; nLine++)
	{
		m_arrGrid.Add(rc);
		rc.OffsetRect(0, m_nLineHeight);
	}

	//--------------------------------------------------
	if (bVScrollEnabled)
	{
		SCROLLINFO scrollInfo;

		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_ALL;
		scrollInfo.nMin = 0;
		scrollInfo.nMax = max(0, nDataLines-1);
		scrollInfo.nPage = m_nLines;
		scrollInfo.nPos = m_nFirstDisplayedStr;

		SetScrollInfo(SB_VERT, &scrollInfo, FALSE);
	}

	return bVScrollEnabled;
}

void CXTPSyntaxEditAutoCompleteWnd::OnPaint()
{
	CPaintDC dc(this);
	CXTPFontDC fontDC(&dc, &m_fontBasic);

	CRect rc;
	CRect rcText;
	CString str;

	int nData = 0;
	int nHighLightedLine = m_nHighLightLine - m_nFirstDisplayedStr;

	for (int nLine = 0; nLine < m_nLines; nLine++)
	{
		rc.CopyRect(m_arrGrid.GetAt(nLine));

		int nIcon = min(m_arrACDataFiltered.GetAt(nData)->m_nIcon, m_ilACGlyphs.GetImageCount() - 1);
		m_ilACGlyphs.Draw(&dc, nIcon, CPoint(rc.left, rc.top), ILD_NORMAL);
		rc.DeflateRect(19,0,0,0);

		if (nLine == nHighLightedLine && m_bHighLight)
		{
			dc.FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_clrHighLight);
			dc.SetTextColor(m_clrHighLightText);
		}
		else
		{
			dc.SetTextColor(m_clrWindowText);
			dc.FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_clrWindow);
		}

		rcText.CopyRect(rc);
		rcText.DeflateRect(0,3,0,3);
		nData = m_nFirstDisplayedStr + nLine;

		dc.TextOut(rcText.left,rcText.top, m_arrACDataFiltered.GetAt(nData)->m_strText);

		if (nLine == nHighLightedLine)
		{
			dc.DrawFocusRect(rc);
		}
	}

}

void CXTPSyntaxEditAutoCompleteWnd::RefreshMetrics()
{
	m_fontBasic.CreateStockObject(DEFAULT_GUI_FONT);
	m_clrHighLight = GetSysColor(COLOR_HIGHLIGHT);
	m_clrWindow = GetSysColor(COLOR_WINDOW);
	m_clrWindowText = GetSysColor(COLOR_WINDOWTEXT);
	m_clrHighLightText = GetSysColor(COLOR_HIGHLIGHTTEXT);
}

int CXTPSyntaxEditAutoCompleteWnd::HitTest(CPoint ptTest)
{
	int nHitLine = -1;
	for (int nLine = 0; nLine < m_nLines; nLine++)
	{
		if (m_arrGrid.GetAt(nLine).PtInRect(ptTest))
		{
			nHitLine = nLine;
		}
	}

	return nHitLine;
}

void CXTPSyntaxEditAutoCompleteWnd::SetList(CXTPSyntaxEditACDataArray& parrData)
{
	RemoveAll();
	m_arrACData.Copy(parrData);
	Sort();
}

void CXTPSyntaxEditAutoCompleteWnd::SetCloseTags(CString strCloseTags)
{
	m_strCloseTags = strCloseTags;
}

BOOL CXTPSyntaxEditAutoCompleteWnd::IsCloseTag(CString strToTest)
{
	BOOL bTagFound = FALSE;
	if (strToTest == _T("~"))
		return bTagFound;

	m_strTmpCloseTag += strToTest;
	int nTmpLen = m_strTmpCloseTag.GetLength();
	int nFound = -1;
	if ((nFound = m_strCloseTags.Find(m_strTmpCloseTag)) >=0)
	{
		if (nFound+nTmpLen == m_strCloseTags.GetLength())
		{
			bTagFound = TRUE;
		}
		else
		{
			TCHAR ch = m_strCloseTags.GetAt(nFound + nTmpLen);
			if (ch == _T('~'))
			{
				bTagFound = TRUE;
			}
		}
	}
	else
	{
		m_strTmpCloseTag = _T("");
	}

	return bTagFound;
}


void CXTPSyntaxEditAutoCompleteWnd::SetOpenTags(CString strOpenTags)
{
	m_strOpenTags = strOpenTags;
}

BOOL CXTPSyntaxEditAutoCompleteWnd::IsOpenTag(CString strToTest)
{

	BOOL bTagFound = FALSE;
	if (strToTest == _T("~"))
		return bTagFound;

	m_strTmpOpenTag += strToTest;
	int nTmpLen = m_strTmpOpenTag.GetLength();
	int nFound = -1;
	if ((nFound = m_strOpenTags.Find(m_strTmpOpenTag)) >=0 )
	{
		if (nFound+nTmpLen == m_strOpenTags.GetLength())
		{
			bTagFound = TRUE;
		}
		else
		{
			TCHAR ch = m_strOpenTags.GetAt(nFound + nTmpLen);
			if (ch == _T('~'))
			{
				bTagFound = TRUE;
			}
		}

	}
	else
	{
		m_strTmpOpenTag = _T("");
	}

	return bTagFound;
}

BOOL CXTPSyntaxEditAutoCompleteWnd::ScrollTo(int nNewLine)
{
	BOOL bScrollRequestCompleted = FALSE;

	if (nNewLine + m_nLines <= m_arrACDataFiltered.GetSize() && nNewLine >= 0)
	{
		bScrollRequestCompleted = TRUE;
		m_nFirstDisplayedStr = nNewLine;
	}

	return bScrollRequestCompleted;
}

void CXTPSyntaxEditAutoCompleteWnd::ReturnSelected(BOOL bAdjust)
{
	if (m_nHighLightLine < 0 || !m_bHighLight)
		return;

	int nCurrentRow = m_pParentWnd->GetCurrentDocumentRow();

	CString strRet = m_arrACDataFiltered.GetAt(m_nHighLightLine)->m_strText;

	if (m_strTmpCloseTag.GetLength() > 0)
	{
		strRet += m_strTmpCloseTag;
		m_strTmpCloseTag = _T("");
	}
	else if (bAdjust)
	{
		strRet = strRet.Left(strRet.GetLength() - 1);
		m_nEndReplacePos = max(m_nStartReplacePos, m_nEndReplacePos - 1);
	}

	m_pParentWnd->Select(nCurrentRow, m_nStartReplacePos, nCurrentRow, m_nEndReplacePos,FALSE);
	m_pParentWnd->ReplaceSel(strRet, FALSE);

	m_pParentWnd->EnsureVisible(m_pParentWnd->GetCurrentDocumentRow(), m_pParentWnd->m_nDispCol, TRUE);
	m_pParentWnd->SetDocModified();

}

BOOL CXTPSyntaxEditAutoCompleteWnd::IsActive()
{
	return m_bActive;
}

void CXTPSyntaxEditAutoCompleteWnd::Hide()
{
	if (!m_bActive)
		return;

	m_bActive = FALSE;
	EnableWindow(FALSE);
	ShowWindow(SW_HIDE);
	m_bFilteredMode = FALSE;
	m_pParentWnd->PostMessage(WM_LBUTTONUP,0,0);
	RedrawWindow();
}

int CXTPSyntaxEditAutoCompleteWnd::CompareACData(const XTP_EDIT_ACDATA** p1, const XTP_EDIT_ACDATA** p2)
{
	const XTP_EDIT_ACDATA* pData1 = *p1;
	const XTP_EDIT_ACDATA* pData2 = *p2;
	return _tcsicmp((LPCTSTR)pData1->m_strText, (LPCTSTR)pData2->m_strText);
}

void CXTPSyntaxEditAutoCompleteWnd::Sort()
{
	int nCount = (int)m_arrACData.GetSize();
	if (nCount > 1)
	{
		typedef int (_cdecl *GENERICCOMPAREFUNC)(const void *,const void*);
		qsort(m_arrACData.GetData(), nCount, sizeof(XTP_EDIT_ACDATA*), (GENERICCOMPAREFUNC)CompareACData);
	}
}

int CXTPSyntaxEditAutoCompleteWnd::CompareACDataToSearch(const XTP_EDIT_ACDATA** ppKey, const XTP_EDIT_ACDATA** ppElem)
{
	int nKeyLength =  (*ppKey)->m_strText.GetLength();
	return (*ppElem)->m_strText.Left(nKeyLength).CompareNoCase((*ppKey)->m_strText);
}

int CXTPSyntaxEditAutoCompleteWnd::Search(CString strSearch)
{
	int nFirstOccurrence = -1;
	if (strSearch.GetLength() == 0)
		return 0;

	XTP_EDIT_ACDATA* key = new XTP_EDIT_ACDATA(0,strSearch);

	XTP_EDIT_ACDATA** pFirst = &m_arrACDataFiltered[0];
	UINT nNum = (UINT)m_arrACDataFiltered.GetSize();
	typedef int (_cdecl *GENERICCOMPAREFUNC)(const void *,const void*);
	XTP_EDIT_ACDATA** pRez = (XTP_EDIT_ACDATA**)_lfind(&key,
				  pFirst,
				  &nNum,
				  sizeof(XTP_EDIT_ACDATA*),
				  (GENERICCOMPAREFUNC)CompareACDataToSearch
				  );

	delete key;

	if (pRez)
	{
		nFirstOccurrence = (LONG)((LONG_PTR)pRez - (LONG_PTR)pFirst)/sizeof(XTP_EDIT_ACDATA*);
	}

	return nFirstOccurrence;
}

int CXTPSyntaxEditAutoCompleteWnd::Filter(CString strSearch)
{
	int nFound = 0;
	m_arrACDataFiltered.RemoveAll();
	if (strSearch.GetLength() == 0)
	{
		m_arrACDataFiltered.Copy(m_arrACData);
		return (int)m_arrACDataFiltered.GetSize();
	}



	if (!m_arrACData.GetSize())
	{
		return 0;
	}

	XTP_EDIT_ACDATA* key = new XTP_EDIT_ACDATA(0, strSearch);
	XTP_EDIT_ACDATA** pFirst = &m_arrACData[0];
	XTP_EDIT_ACDATA** pNext = NULL;
	UINT nNum = (UINT)m_arrACData.GetSize();
	typedef int (_cdecl *GENERICCOMPAREFUNC)(const void *,const void*);
	while (nNum > 0)
	{
		pNext = (XTP_EDIT_ACDATA**)_lfind(&key,
				  pFirst,
				  &nNum,
				  sizeof(XTP_EDIT_ACDATA*),
				  (GENERICCOMPAREFUNC)CompareACDataToSearch
				  );

		if (pNext)
		{
			nNum -= (LONG) ((LONG_PTR)pNext - (LONG_PTR)pFirst)/sizeof(XTP_EDIT_ACDATA*);
			pFirst = pNext;
			nNum --;
			pFirst++;
			m_arrACDataFiltered.Add(*pNext);
			nFound++;
		}
		else
		{
			nNum --;
			pFirst++;
		}
	}

	delete key;

	return nFound;
}

void CXTPSyntaxEditAutoCompleteWnd::RemoveAll()
{
	int nDataLines = (int)m_arrACData.GetSize();
	m_arrACDataFiltered.RemoveAll();
	XTP_EDIT_ACDATA* pACData = NULL;
	for (int nDataLine = 0; nDataLine < nDataLines; nDataLine++)
	{
		pACData = m_arrACData.GetAt(nDataLine); ASSERT(pACData);
		delete pACData;
	}
	m_arrACData.RemoveAll();
}
