// XTPReportControl.cpp : implementation of the CXTPReportControl class.
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
#include "Resource.h"

#include "Common/XTPDrawHelpers.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPPropExchange.h"
#include "Common/XTPToolTipContext.h"
#include "Common/XTPResourceManager.h"

#include "XTPReportRecordItem.h"
#include "XTPReportRecordItemText.h"
#include "XTPReportRecord.h"
#include "XTPReportRecords.h"
#include "XTPReportHeader.h"
#include "XTPReportColumn.h"
#include "XTPReportColumns.h"
#include "XTPReportRow.h"
#include "XTPReportRows.h"
#include "XTPReportControl.h"
#include "XTPReportPaintManager.h"
#include "XTPReportNavigator.h"
#include "XTPReportFilterEditControl.h"
#include "XTPReportSubListControl.h"
#include "XTPReportGroupRow.h"
#include "XTPReportInplaceControls.h"

#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTP_REPORT_HSCROLL_STEP 7

#define XTP_REPORT_AUTO_SCROLL_TIMER_ID 7
#define XTP_REPORT_AUTO_SCROLL_TIMER_RESOLUTION_MS  200

#define XTP_REPORT_CB_RECORDS_DATA_VER 1

#define ASSERT_DBG_REMOVE_RECORD_EX
//#define XTP_DBG_REMOVE_RECORD_EX_ASSERT ASSERT


//////////////////////////////////////////////////////////////////////////
XTP_IMPLEMENT_HEAP_ALLOCATOR(CXTPReportDataAllocator, FALSE)
XTP_IMPLEMENT_HEAP_ALLOCATOR(CXTPReportRowAllocator, FALSE)

// to allocate in app default heap
XTP_IMPLEMENT_HEAP_ALLOCATOR(CXTPReportAllocatorDefault, FALSE)


class CXTPReportRow_Batch : public CXTPBatchAllocObjT<CXTPReportRow, CXTPReportRow_BatchData> {};
class CXTPReportGroupRow_Batch : public CXTPBatchAllocObjT<CXTPReportGroupRow, CXTPReportGroupRow_BatchData> {};

XTP_IMPLEMENT_BATCH_ALLOC_OBJ_DATA(CXTPReportRow_BatchData, CXTPReportRow_Batch, FALSE)
XTP_IMPLEMENT_BATCH_ALLOC_OBJ_DATA(CXTPReportGroupRow_BatchData, CXTPReportGroupRow_Batch, FALSE)

//===========================================================================
BOOL CXTPReportControl::UseReportCustomHeap()
{
	ASSERT(CXTPReportDataAllocator::ms_dwRefs == 0 || CXTPReportDataAllocator::ms_bUseCustomHeap);
	ASSERT(CXTPReportRowAllocator::ms_dwRefs == 0 || CXTPReportRowAllocator::ms_bUseCustomHeap);

	if (CXTPReportDataAllocator::ms_dwRefs == 0)
		CXTPReportDataAllocator::ms_bUseCustomHeap = TRUE;

	if (CXTPReportRowAllocator::ms_dwRefs == 0)
		CXTPReportRowAllocator::ms_bUseCustomHeap = TRUE;

	return CXTPReportDataAllocator::ms_bUseCustomHeap &&
			CXTPReportRowAllocator::ms_bUseCustomHeap;
}

BOOL CXTPReportControl::UseRowBatchAllocation()
{
	ASSERT(CXTPReportRow_BatchData::IsDataEmpty() || CXTPReportRow_BatchData::m_bBatchAllocationEnabled);
	ASSERT(CXTPReportGroupRow_BatchData::IsDataEmpty() || CXTPReportGroupRow_BatchData::m_bBatchAllocationEnabled);

	if (CXTPReportRow_BatchData::IsDataEmpty())
		CXTPReportRow_BatchData::m_bBatchAllocationEnabled = TRUE;

	if (CXTPReportGroupRow_BatchData::IsDataEmpty())
		CXTPReportGroupRow_BatchData::m_bBatchAllocationEnabled = TRUE;

	return CXTPReportRow_BatchData::m_bBatchAllocationEnabled &&
			CXTPReportGroupRow_BatchData::m_bBatchAllocationEnabled;
}

void CXTPReportControl::FreeRowBatchExtraData()
{
	CXTPReportRow_Batch::FreeExtraData();
	CXTPReportGroupRow_Batch::FreeExtraData();
}

//////////////////////////////////////////////////////////////////////////
BOOL CXTPReportControlLocale::s_bUseResourceFileLocale = FALSE;

CArray<CXTPReportControlLocale::XTP_TIMESPEC, CXTPReportControlLocale::XTP_TIMESPEC&>
	CXTPReportControlLocale::s_arMappedSpecs;

//===========================================================================
BOOL CXTPReportControlLocale::IsUseResourceFileLocale()
{
	return s_bUseResourceFileLocale;
}

void CXTPReportControlLocale::SetUseResourceFileLocale(BOOL bUseResourceFileLocale)
{
	s_bUseResourceFileLocale = bUseResourceFileLocale;
}

LCID CXTPReportControlLocale::GetActiveLCID()
{
	LCID lcidCurr = LOCALE_USER_DEFAULT;

	if (s_bUseResourceFileLocale)
		lcidCurr = MAKELCID(XTPResourceManager()->GetResourcesLangID(), SORT_DEFAULT);

	return lcidCurr;
}

BOOL AFX_CDECL CXTPReportControlLocale::VariantChangeTypeEx(VARIANT& rVarValue, VARTYPE vartype, BOOL bThrowError)
{
	if (vartype != rVarValue.vt)
	{
		LCID lcID = GetActiveLCID();

		HRESULT hr = ::VariantChangeTypeEx(&rVarValue, &rVarValue, lcID, 0, vartype);

		if (bThrowError && FAILED(hr))
		{
			if (hr == E_OUTOFMEMORY)
				AfxThrowMemoryException();
			else
				AfxThrowOleException(hr);
		}


		return SUCCEEDED(hr);
	}
	return TRUE;
}

CString AFX_CDECL CXTPReportControlLocale::FormatDateTime(const COleDateTime& dt, LPCTSTR lpcszFormatString)
{
	return _FormatDateTime(dt, lpcszFormatString, GetActiveLCID());
}

CString CXTPReportControlLocale::_FormatDateTime(const COleDateTime& dt, LPCTSTR lpcszFormatString, LCID lcLocaleID)
{
	if (dt.GetStatus() != COleDateTime::valid)
	{
		ASSERT(dt.GetStatus() == COleDateTime::null);
		return _T("");
	}

	CString strDT = lpcszFormatString;

	SYSTEMTIME sysTime;
	if (!GETASSYSTEMTIME_DT(dt, sysTime))
	{
		ASSERT(FALSE);
		return _T("");
	}

	//  %% Percent sign
	REPLACE_S(strDT, _T("%%"), _T("\0x1"));

	_ProcessMappedSpecs(strDT, &sysTime, lcLocaleID);
	_ProcessDateTimeSpecs(strDT, &sysTime, lcLocaleID);

	// All locale dependent specifiers already processed
	_ProcessOtherSpecs(strDT, dt);

	REPLACE_S(strDT, _T("\0x1"), _T("%"));

	return strDT;
}

void CXTPReportControlLocale::_ProcessMappedSpecs(CString& rstrFormat, const SYSTEMTIME* pST, LCID lcLocaleID)
{
	_InitMappedSpecs();

	const int cnBufferSize = 96;
	TCHAR szBuffer[cnBufferSize];

	int nCount = (int)s_arMappedSpecs.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		const XTP_TIMESPEC& specI = s_arMappedSpecs.ElementAt(i);
		if (FIND_S(rstrFormat, specI.pcszSpec, 0) < 0)
			continue;

		::ZeroMemory(szBuffer, sizeof(szBuffer));

		int nResult;
		if (specI.bTime)
		{
			nResult = ::GetTimeFormat(lcLocaleID, 0, pST, specI.pcszFormat, szBuffer, cnBufferSize);
		}
		else
		{
			nResult = ::GetDateFormat(lcLocaleID, 0, pST, specI.pcszFormat, szBuffer, cnBufferSize);
		}
		ASSERT(nResult);
		if (nResult)
		{
			REPLACE_S(rstrFormat, specI.pcszSpec, szBuffer);
		}
	}
}

void CXTPReportControlLocale::_ProcessDateTimeSpecs(CString& rstrFormat, const SYSTEMTIME* pST, LCID lcLocaleID)
{
//  %c  Date and time representation appropriate for locale
//  %#c Long date and time representation

//  %x Date representation for current locale
//  %#x Long date representation for current locale

//  %X Time representation for current locale

	REPLACE_S(rstrFormat, _T("%c"), _T("%x %X"));
	REPLACE_S(rstrFormat, _T("%#c"), _T("%#x %X"));

	__ProcessDate_x(rstrFormat, pST, lcLocaleID);
	__ProcessTime_X(rstrFormat, pST, lcLocaleID);
}

void CXTPReportControlLocale::__ProcessDate_x(CString& rstrFormat, const SYSTEMTIME* pST, LCID lcLocaleID)
{
	const int cnBufferSize = 96;
	TCHAR szBuffer[cnBufferSize];

	//  %x Date representation for current locale
	//  %#x Long date representation for current locale

	TCHAR* arSpec2[2] = {_T("%x"), _T("%#x")};

	for (int i = 0; i < 2; i++)
	{
		if (FIND_S(rstrFormat, arSpec2[i], 0) >= 0)
		{
			::ZeroMemory(szBuffer, sizeof(szBuffer));

			DWORD dwFlags = (i == 0) ? DATE_SHORTDATE : DATE_LONGDATE;

			int nRes = ::GetDateFormat(lcLocaleID, dwFlags, pST, NULL, szBuffer, cnBufferSize);

			ASSERT(nRes);
			REPLACE_S(rstrFormat, arSpec2[i], szBuffer);
		}
	}
}

void CXTPReportControlLocale::__ProcessTime_X(CString& rstrFormat, const SYSTEMTIME* pST, LCID lcLocaleID)
{
	if (FIND_S(rstrFormat, _T("%X"), 0) >= 0)
	{
		const int cnBufferSize = 96;
		TCHAR szBuffer[cnBufferSize];

		::ZeroMemory(szBuffer, sizeof(szBuffer));
		int nRes = ::GetTimeFormat(lcLocaleID, 0, pST, NULL, szBuffer, cnBufferSize);

		ASSERT(nRes);
		REPLACE_S(rstrFormat, _T("%X"), szBuffer);
	}
}

void CXTPReportControlLocale::_ProcessOtherSpecs(CString& rstrFormat, const COleDateTime& dt)
{
	//  %j Day of year as decimal number (001 – 366)
	//  %w Weekday as decimal number (0 – 6; Sunday is 0)
	//  %U Week of year as decimal number, with Sunday as first day of week (00 – 53)
	//  %W Week of year as decimal number, with Monday as first day of week (00 – 53)
	//  %z, %Z  Either the time-zone name or time zone abbreviation, depending on registry settings; no characters if time zone is unknown

	static LPCTSTR arszSpecs[] = {  _T("%j"), _T("%#j"),
									_T("%w"), _T("%#w"),
									_T("%U"), _T("%#U"),
									_T("%W"), _T("%#W"),
									_T("%z"), _T("%Z") };

	int nCount = _countof(arszSpecs);
	for (int i = 0; i < nCount; i++)
	{
		if (FIND_S(rstrFormat, arszSpecs[i], 0) < 0)
			continue;

		CString str = dt.Format(arszSpecs[i]);

		REPLACE_S(rstrFormat, arszSpecs[i], str);
	}
}

void CXTPReportControlLocale::_InitMappedSpecs()
{
	if (s_arMappedSpecs.GetSize())
		return;

	// date
	_AddsMappedSpec(_T("%a"), _T("ddd"),    FALSE);
	_AddsMappedSpec(_T("%A"), _T("dddd"),   FALSE);

	_AddsMappedSpec(_T("%b"), _T("MMM"),    FALSE);
	_AddsMappedSpec(_T("%B"), _T("MMMM"),   FALSE);

	_AddsMappedSpec(_T("%d"),  _T("dd"),    FALSE);
	_AddsMappedSpec(_T("%#d"), _T("d"),     FALSE);

	_AddsMappedSpec(_T("%m"),  _T("MM"),    FALSE);
	_AddsMappedSpec(_T("%#m"), _T("M"),     FALSE);

	_AddsMappedSpec(_T("%y"),  _T("yy"),    FALSE);
	_AddsMappedSpec(_T("%#y"), _T("y"),     FALSE);
	_AddsMappedSpec(_T("%Y"), _T("yyyy"),   FALSE);

	// time
	_AddsMappedSpec(_T("%H"),  _T("HH"),    TRUE);
	_AddsMappedSpec(_T("%#H"), _T("H"),     TRUE);

	_AddsMappedSpec(_T("%I"),  _T("hh"),    TRUE);
	_AddsMappedSpec(_T("%#I"), _T("h"),     TRUE);

	_AddsMappedSpec(_T("%M"),  _T("mm"),    TRUE);
	_AddsMappedSpec(_T("%#M"), _T("m"),     TRUE);

	_AddsMappedSpec(_T("%S"),  _T("ss"),    TRUE);
	_AddsMappedSpec(_T("%#S"), _T("s"),     TRUE);

	_AddsMappedSpec(_T("%p"), _T("tt"),     TRUE);
}

void CXTPReportControlLocale::_AddsMappedSpec(LPCTSTR pcszSpec, LPCTSTR pcszFormat, BOOL bTime)
{
	XTP_TIMESPEC tmpSpec = {pcszSpec, pcszFormat, bTime};
	s_arMappedSpecs.Add(tmpSpec);
}

//////////////////////////////////////////////////////////////////////////
// CReportDropTarget
class CXTPReportControl::CReportDropTarget : public COleDropTarget
{
public:
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
	{
		CXTPReportControl* pReport = DYNAMIC_DOWNCAST(CXTPReportControl, pWnd);
		if (!pReport)
			return DROPEFFECT_NONE;

		return pReport->OnDragOver(pDataObject, dwKeyState, point);
	}

	virtual void OnDragLeave(CWnd* pWnd)
	{
		CXTPReportControl* pReport = DYNAMIC_DOWNCAST(CXTPReportControl, pWnd);

		if (pReport)
		{
			pReport->OnDragOver(NULL, 0, CPoint(-1, -1));
		}
	}
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point)
	{
		CXTPReportControl* pReport = DYNAMIC_DOWNCAST(CXTPReportControl, pWnd);

		if (pReport)
		{
			return pReport->OnDrop(pDataObject, dropEffect, point);
		}
		return FALSE;
	}
};

/////////////////////////////////////////////////////////////////////////////
void XTPStrSplit(LPCTSTR pcszString, LPCTSTR pcszSeparator, CStringArray& rarStrings)
{
	rarStrings.RemoveAll();

	int nSeparatorLen = (int)_tcslen(pcszSeparator);
	CString strString(pcszString);
	CString strItem;
	int nIndex_start = 0;

	// parse data
	BOOL bBreak = FALSE;
	do
	{
		int nIndex = FIND_S(strString, pcszSeparator, nIndex_start);
		if (nIndex >= 0)
		{
			strItem = strString.Mid(nIndex_start, nIndex - nIndex_start);
		}
		else
		{
			strItem = strString.Mid(nIndex_start);
			bBreak = TRUE;
		}

		//---------------------------------------
		rarStrings.Add(strItem);

		nIndex_start = nIndex + nSeparatorLen;
	}
	while (!bBreak);
}

CString XTPStrMake(const CStringArray& arStrings, LPCTSTR pcszSeparator)
{
	CString strString;

	int nCount = (int)arStrings.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		if (i > 0)
		{
			strString += pcszSeparator;
		}
		strString += arStrings[i];
	}

	return strString;
}

// CXTPReportControl
IMPLEMENT_DYNCREATE(CXTPReportControl, CWnd)

BEGIN_MESSAGE_MAP(CXTPReportControl, CWnd)
//{{AFX_MSG_MAP(CXTPReportControl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SYSKEYDOWN()
	ON_WM_CAPTURECHANGED()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYUP()
	ON_WM_SYSKEYUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETDLGCODE()
	ON_WM_CHAR()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_STYLECHANGED()
	ON_WM_ENABLE()
	ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXTPReportControl::CXTPReportControl()
{
	RegisterWindowClass();

	m_nLockUpdateCount = 0;
	m_nRowsPerWheel = GetMouseScrollLines();

	m_pRows = new CXTPHeapObjectT<CXTPReportRows, CXTPReportAllocatorDefault>;
	m_pPlainTree = new CXTPHeapObjectT<CXTPReportRows, CXTPReportAllocatorDefault>;
	m_pRecords = new CXTPHeapObjectT<CXTPReportRecords, CXTPReportAllocatorDefault>;

	m_pColumns = new CXTPReportColumns(this);
	m_pReportHeader = new CXTPReportHeader(this, m_pColumns);

	m_pPaintManager = new CXTPReportPaintManager();
	m_pNavigator = new CXTPReportNavigator(this);
	m_nTopRow = 0;

	m_nFocusedRow = -1;
	m_mouseMode = xtpReportMouseNothing;

	m_pSelectedRows = new CXTPReportSelectedRows(this);

	m_bChanged = TRUE;
	m_bRefreshIndexes = FALSE;

	m_bGroupByEnabled = FALSE;
	m_bHeaderVisible = TRUE;
	m_bFooterVisible = FALSE;
	m_bMultipleSelection = TRUE;

	m_bShowTooltips = TRUE;

	m_bSkipGroupsFocus = TRUE;

	m_pImageManager = new CXTPImageManager();

	m_pFocusedColumn = NULL;
	m_pActiveItem = NULL;

	m_bFocusSubItems = FALSE;
	m_bEditOnClick = TRUE;
	m_bAllowEdit = FALSE;
	m_bAutoCheckItems = TRUE;

	m_pInplaceEdit = new CXTPReportInplaceEdit();
	m_pInplaceButtons = new CXTPReportInplaceButtons();
	m_pInplaceList = new CXTPReportInplaceList();

	m_rcGroupByArea.SetRectEmpty();
	m_rcHeaderArea.SetRectEmpty();
	m_rcReportArea.SetRectEmpty();
	m_rcFooterArea.SetRectEmpty();

	m_nFreezeColumnsCount = 0;
	m_nLeftOffset = 0;
	m_pHotRow = 0;
	m_nOLEDropMode = 0;
	m_bFullColumnScrolling = FALSE;
	m_nHScrollStep = XTP_REPORT_HSCROLL_STEP;

	m_bVScrollBarVisible = FALSE;
	m_bHScrollBarVisible = FALSE;

	m_bPrepareDrag = FALSE;
	m_pointDrag = CPoint(0, 0);

	m_pToolTipContext = new CXTPToolTipContext;
	m_pCachedToolTipInfo = new XTP_NM_REPORTTOOLTIPINFO;

	m_nPopulatedRecordsCount = 0;

	m_cfReport = NULL;
	m_pDropTarget = new CReportDropTarget;
	m_bDragMode = FALSE;
	m_dwDragDropFlags = 0;
	m_nDropPos = -1;
	m_pSelectOnUpRow = NULL;

	m_bOnSizeRunning = FALSE;
	m_uAutoScrollTimerID = 0;

	m_bSortRecordChilds = FALSE;

	m_pRowsCompareFunc = NULL;

	m_ptrVirtualEditingRow = NULL;
}

BOOL CXTPReportControl::RegisterWindowClass(HINSTANCE hInstance /*= NULL*/)
{
	return XTPDrawHelpers()->RegisterWndClass(hInstance, XTPREPORTCTRL_CLASSNAME, CS_DBLCLKS);
}

CXTPReportControl::~CXTPReportControl()
{
	if (::IsWindow(m_wndTip.GetSafeHwnd()))
		m_wndTip.DestroyWindow();

	EditItem(0);

	//m_arrScreenRows.Clear();
	ResetContent(FALSE);

	CMDTARGET_RELEASE(m_pRows);
	CMDTARGET_RELEASE(m_pPlainTree);
	CMDTARGET_RELEASE(m_pSelectedRows);
	CMDTARGET_RELEASE(m_pRecords);
	CMDTARGET_RELEASE(m_pColumns);
	CMDTARGET_RELEASE(m_pPaintManager);
	CMDTARGET_RELEASE(m_pNavigator);

	CMDTARGET_RELEASE(m_pImageManager);
	CMDTARGET_RELEASE(m_pReportHeader);

	CMDTARGET_RELEASE(m_pToolTipContext);
	SAFE_DELETE(m_pCachedToolTipInfo);

	SAFE_DELETE(m_pInplaceEdit);
	SAFE_DELETE(m_pInplaceButtons);
	SAFE_DELETE(m_pInplaceList);

	CMDTARGET_RELEASE(m_pDropTarget);
	CMDTARGET_RELEASE(m_ptrVirtualEditingRow);
}

void CXTPReportControl::ResetContent(BOOL bUpdateControl)
{
	m_arrScreenRows.Clear();

	if (m_pRows) m_pRows->Clear();
	if (m_pPlainTree) m_pPlainTree->Clear();
	if (m_pSelectedRows) m_pSelectedRows->Clear();

	if (m_pRecords) m_pRecords->RemoveAll();

	if (bUpdateControl)
	{
		AdjustIndentation();
		AdjustScrollBars();
		RedrawControl();
	}
}


void CXTPReportControl::SetReportHeader(CXTPReportHeader* pReportHeader)
{
	if (pReportHeader)
	{
		m_pReportHeader->InternalRelease();
		m_pReportHeader = pReportHeader;
		AdjustLayout();
	}
}

void CXTPReportControl::SetImageManager(CXTPImageManager* pImageManager)
{
	if (pImageManager)
	{
		m_pImageManager->InternalRelease();
		m_pImageManager = pImageManager;
	}

}

void CXTPReportControl::SetImageList(CImageList* pImageList)
{
	for (int i = 0; i < pImageList->GetImageCount(); i++)
	{
		HICON hIcon = pImageList->ExtractIcon(i);
		m_pImageManager->SetIcon(hIcon, i);

		DestroyIcon(hIcon);
	}
}

void CXTPReportControl::SetPaintManager(CXTPReportPaintManager* pPaintManager)
{
	if (pPaintManager)
	{
		m_pPaintManager->InternalRelease();
		m_pPaintManager = pPaintManager;

		AdjustLayout();
		AdjustScrollBars();
	}
}


BOOL CXTPReportControl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	if (!CWnd::Create(XTPREPORTCTRL_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;

	return TRUE;
}

int CXTPReportControl::GetIndent(int nLevel) const
{
	return max(0, (nLevel - 1) * m_pPaintManager->m_nTreeIndent);
}

void CXTPReportControl::SetTreeIndent(int nIndent)
{
	m_pPaintManager->m_nTreeIndent = nIndent;
}


void CXTPReportControl::BeginUpdate()
{
	m_nLockUpdateCount++;
	m_bRefreshIndexes = FALSE;

}

void CXTPReportControl::EndUpdate()
{
	SetChanged();
	m_nLockUpdateCount--;

	if (m_nLockUpdateCount == 0)
	{
		if (m_bRefreshIndexes)
		{
			m_bRefreshIndexes = FALSE;
			_RefreshIndexes();
		}

		RedrawControl();
	}
}

void CXTPReportControl::RedrawControl()
{
	SetChanged();
	if (m_nLockUpdateCount == 0 && GetSafeHwnd())
	{
		Invalidate(FALSE);
	}
}

void CXTPReportControl::UpdateSubList()
{
	CXTPReportHeader* pHeader = GetReportHeader();
	if (pHeader && pHeader->m_pSubList &&
		(pHeader->m_pSubList->GetReportCtrl() == this))
	{
		pHeader->m_pSubList->UpdateList();
	}
}

void CXTPReportControl::_DoCollapse(CXTPReportRow* pRow)
{
	int nIndex = pRow->GetIndex() + 1;
	int nCount = 0;

	while (nIndex < m_pRows->GetCount())
	{
		CXTPReportRow* pRowChild = m_pRows->GetAt(nIndex);
		if (!pRowChild->HasParent(pRow))
			break;

		pRowChild->m_bVisible = FALSE;
		pRowChild->m_nIndex = -1;
		m_pRows->RemoveAt(nIndex);
		nCount++;
	}

	if (nCount > 0)
	{
		m_pSelectedRows->_OnCollapsed(nIndex - 1, nCount);

		if (m_nFocusedRow >= nIndex)
			m_nFocusedRow = max(nIndex - 1, m_nFocusedRow - nCount);

	}
}

void CXTPReportControl::_DoExpand(CXTPReportRow* pRow)
{
	int nIndex = pRow->m_nIndex;
	int nCount = _DoExpand(nIndex, pRow);

	if (nCount > 0)
	{
		m_pSelectedRows->_OnExpanded(nIndex, nCount);

		if (m_nFocusedRow > nIndex)
			m_nFocusedRow += nCount;

	}
}

int CXTPReportControl::_DoExpand(int nIndex, CXTPReportRow* pRow)
{
	if (!pRow->HasChildren())
		return 0;

	int nStartIndex = nIndex;
	for (int i = 0; i < pRow->GetChilds()->GetCount(); i++)
	{
		CXTPReportRow* pRowChild = pRow->GetChilds()->GetAt(i);

		pRowChild->m_nRowLevel = pRow->m_nRowLevel + 1;
		pRowChild->m_nGroupLevel = pRow->m_nGroupLevel + (pRow->IsGroupRow() ? 1 : 0);

		nIndex += InsertRow(nIndex + 1, pRowChild);
	}
	return nIndex - nStartIndex;
}

void CXTPReportControl::_RefreshIndexes(BOOL bAdjustLayout)
{
	if (m_nLockUpdateCount != 0)
	{
		m_bRefreshIndexes = TRUE;
		return;

	}

	int nRowCount = m_pRows->GetCount();
	for (int nIndex = 0; nIndex < nRowCount; nIndex++)
	{
		CXTPReportRow* pRow = m_pRows->GetAt(nIndex);
		pRow->m_nIndex = nIndex;
		ASSERT(pRow->m_bVisible);
	}

	if (bAdjustLayout)
		AdjustScrollBars();
}

int CXTPReportControl::InsertRow(int nIndex, CXTPReportRow* pRow)
{
	m_pRows->InsertAt(nIndex, pRow);
	pRow->InternalAddRef();
	pRow->m_bVisible = TRUE;


	int nRowsInserted = 1;

	if (pRow->IsExpanded() && pRow->HasChildren())
	{
		nRowsInserted += _DoExpand(nIndex, pRow);
	}
	return nRowsInserted;
}

void CXTPReportControl::BuildTree(CXTPReportRows* pTree, CXTPReportRow* pParentRow, CXTPReportRecords* pRecords)
{
	for (int i = 0; i < pRecords->GetCount(); i++)
	{
		CXTPReportRecord* pRecord = pRecords->GetAt(i);

		// add record if all conditions are met
		if (pRecord->IsLocked() ||
			(pRecord->IsVisible() && !ApplyFilter(pRecord, GetFilterText(), IsPreviewMode())))
		{
			CXTPReportRow* pRow = CreateRow();
			pRow->InitRow(this, pRecord);

			pRow->m_pParentRow = pParentRow;
			pTree->Add(pRow);

			if (pRecord->HasChildren())
			{
				BuildTree(pRow->GetChilds(), pRow, pRecord->GetChilds());
				if (pRow->GetChilds() && m_bSortRecordChilds)
				{
					SortRows(pRow->GetChilds());
				}
			}
		}
	}
}

CLIPFORMAT CXTPReportControl::EnableDragDrop(LPCTSTR lpszClipboardFormat, DWORD dwFlags)
{
	if (m_dwDragDropFlags != 0)
	{
		if (m_pDropTarget)
			m_pDropTarget->Revoke();
	}

	m_dwDragDropFlags = dwFlags;
	m_cfReport = NULL;

	if (m_dwDragDropFlags != 0)
	{
		if (m_pDropTarget)
		{
			m_cfReport = (CLIPFORMAT)::RegisterClipboardFormat(lpszClipboardFormat);

			m_pDropTarget->Register(this);
		}
	}

	return m_cfReport;
}

BOOL CXTPReportControl::ApplyFilter(CXTPReportRecord* pRecord, CString strFilterText, BOOL bIncludePreview)
{
	// not filtered if filter text is empty
	if (!pRecord)
		return FALSE;

	if (pRecord->IsFiltered())
		return TRUE;

	if (strFilterText.IsEmpty())
		return FALSE;

	// process each token in the filter string
	TCHAR szSeps[] = _T(" \t");
	TCHAR *szToken, *lpszContext = 0;
	//int nCurPos = 0;

	szToken = STRTOK_S(strFilterText.GetBuffer(strFilterText.GetLength()), szSeps, &lpszContext);
	while (szToken != NULL)
	{
		CString strToken(szToken);
		strToken.MakeLower();
		BOOL bTokenFound = FALSE;
		// enumerate all visible columns
		int nColumnsCount = m_pColumns->GetCount();
		for (int nCol = 0; nCol < nColumnsCount; nCol++)
		{
			CXTPReportColumn* pCol = m_pColumns->GetAt(nCol);
			if (pCol && pCol->IsVisible() && pCol->IsFiltrable())
			{
				CXTPReportRecordItem* pItem = pRecord->GetItem(pCol);
				if (pItem)
				{
					CString sItemText = pItem->GetCaption(pCol);
					// case-insensitive search
					sItemText.MakeLower();
					bTokenFound = sItemText.Find(strToken) != -1;

					if (bTokenFound)
					{
						// stop search current token - passed
						break;
					}
				}
			}
		}
		// also check preview text
		if (bIncludePreview && !bTokenFound && pRecord->GetItemPreview())
		{
			CString sItemText = pRecord->GetItemPreview()->GetCaption(NULL);
			// case-insensitive search
			sItemText.MakeLower();
			bTokenFound = sItemText.Find(strToken) != -1;
		}

		// Token not found - filter this record
		if (!bTokenFound)
		{
			return TRUE;
		}

		// get next token
		szToken = STRTOK_S(NULL, szSeps, &lpszContext);
	}

	return FALSE;
}

void CXTPReportControl::Populate()
{
	EditItem(FALSE);

	BeginUpdate();

	m_nPopulatedRecordsCount = 0;

	// save focused items
	CXTPReportRecord* pFocusedRecord = GetFocusedRow() ? GetFocusedRow()->GetRecord() : NULL;
	m_pSelectedRows->Clear();
	m_nFocusedRow = -1;
	m_pRows->Clear();
	m_pPlainTree->Clear();
	m_arrScreenRows.Clear(FALSE);

	if (IsVirtualMode())
	{
		CXTPReportRow* pRow = CreateRow();
		pRow->InitRow(this, GetRecords()->m_pVirtualRecord);
		pRow->m_bVisible = TRUE;

		m_pRows->SetVirtualMode(pRow, GetRecords()->GetCount());
		m_nPopulatedRecordsCount = GetRecords()->GetCount();
	}
	else
	{
		BuildTree(m_pPlainTree, NULL, m_pRecords);
		m_nPopulatedRecordsCount = m_pPlainTree->GetCount();

		SortRows(m_pPlainTree);

		if (m_pColumns->GetGroupsOrder()->GetCount() > 0)
		{
			CXTPReportRows* pGroupTree = new CXTPHeapObjectT<CXTPReportRows, CXTPReportAllocatorDefault>;
			CXTPReportGroupRow* pLastGroup = NULL;

			for (int nPlainRow = 0; nPlainRow < m_pPlainTree->GetCount(); nPlainRow++)
			{
				CXTPReportRow* pRow = m_pPlainTree->GetAt(nPlainRow);
				CXTPReportGroupRow* pGroupToAdd = NULL;

				for (int nColumn = 0; nColumn < m_pColumns->GetGroupsOrder()->GetCount(); nColumn++)
				{
					CXTPReportColumn* pColumn = m_pColumns->GetGroupsOrder()->GetAt(nColumn);
					CXTPReportRecordItem* pItem = pRow->GetRecord()->GetItem(pColumn);
					CString strGroup = pItem ? pItem->GetGroupCaption(pColumn) : _T("");

					if (pLastGroup && GetRecords()->Compare(pLastGroup->GetCaption(), strGroup) == 0)
					{
						pGroupToAdd = pLastGroup;

						if (pGroupToAdd->HasChildren())
							pLastGroup = (CXTPReportGroupRow*)pGroupToAdd->GetChilds()->GetAt(pGroupToAdd->GetChilds()->GetCount() - 1);

					}
					else
					{
						CXTPReportGroupRow* pGroup = CreateGroupRow();

						pGroup->InitRow(this, NULL);
						pGroup->SetCaption(strGroup);

						if (pGroupToAdd)
						{
							pGroupToAdd->AddChild(pGroup);
						}
						else
						{
							pGroupTree->Add(pGroup);
						}

						pGroupToAdd = pGroup;
						pLastGroup = NULL;
					}

				}
				if (pGroupToAdd) pGroupToAdd->AddChild(pRow);
				pRow->InternalAddRef();
				pLastGroup = (CXTPReportGroupRow*)pGroupTree->GetAt(pGroupTree->GetCount() - 1);
			}

			m_pPlainTree->InternalRelease();
			m_pPlainTree = pGroupTree;
		}

		for (int nGroupRow = 0; nGroupRow < m_pPlainTree->GetCount(); nGroupRow++)
		{
			CXTPReportRow* pRow = m_pPlainTree->GetAt(nGroupRow);
			InsertRow(m_pRows->GetCount(), pRow);
			pRow->m_nChildIndex = nGroupRow;
			ASSERT(pRow->m_pParentRows == m_pPlainTree);
		}


		m_nFocusedRow = -1;
		// Update indexes on virtual rows
		int nRowCount = m_pRows->GetCount();
		for (int nRowIndex = 0; nRowIndex < nRowCount; nRowIndex++)
		{
			CXTPReportRow* pRow = m_pRows->GetAt(nRowIndex);
			if (pRow)
			{
				pRow->SetIndex(nRowIndex);
				pRow->m_bVisible = TRUE;

				if (pFocusedRecord && pRow->GetRecord() == pFocusedRecord)
				{
					m_nFocusedRow = pRow->GetIndex();
					m_pSelectedRows->Select(pRow);
				}
			}
		}
	}

	AdjustIndentation();
	AdjustLayout();
	AdjustScrollBars();

	UpdateSubList();

	if (m_nFocusedRow == -1)
	{
		SetTopRow(0);

		if (GetRows()->GetCount() > 0)
		{
			m_nFocusedRow = 0;
			m_pSelectedRows->Select(m_pRows->GetAt(0));
		}

	}
	else
	{
		EnsureVisible(GetFocusedRow());
	}

	if (m_pFocusedColumn == NULL && m_bFocusSubItems)
	{
		m_pFocusedColumn = m_pColumns->GetFirstVisibleColumn();
	}

	EndUpdate();
}

void CXTPReportControl::SortRows(CXTPReportRows* pRows)
{
	if (pRows->GetCount() == 0)
		return;

	if (m_pRowsCompareFunc && m_pRowsCompareFunc != CXTPReportRows::CompareRows)
	{
		pRows->SortEx(m_pRowsCompareFunc);
		return;
	}

	if (m_pColumns->GetSortOrder()->GetCount() == 0 &&
		m_pColumns->GetGroupsOrder()->GetCount() == 0)
		return;

	pRows->Sort();
}

void CXTPReportControl::AdjustIndentation()
{
	XTP_TRACE(_T("AdjustIndentation()\n"));

	GetReportHeader()->m_nIndentLevel =
		m_pColumns->GetGroupsOrder()->GetCount();
}

CXTPReportColumn* CXTPReportControl::AddColumn(CXTPReportColumn* pColumn)
{
	ASSERT(pColumn);
	m_pColumns->Add(pColumn);

	return pColumn;
}

CXTPReportRecord* CXTPReportControl::AddRecord(CXTPReportRecord* pRecord)
{
	ASSERT(pRecord);
	m_pRecords->Add(pRecord);

	return pRecord;
}

BOOL CXTPReportControl::RemoveRowEx(CXTPReportRow* pRow, BOOL bAdjustLayout)
{
	ASSERT(pRow);
	if (!pRow)
		return FALSE;

	//------------------------------------------------
	if (!pRow->IsGroupRow())
	{
		ASSERT(pRow->GetRecord());
		return RemoveRecordEx(pRow->GetRecord(), bAdjustLayout);
	}

	//------------------------------------------------
	CWaitCursor _WC;

	int nCount = pRow->GetChilds()->GetCount();
	ASSERT(nCount);

	for (int i = nCount - 1; i >= 0 ; i--)
	{
		CXTPReportRow* pRowI = pRow->GetChilds()->GetAt(i);
		RemoveRowEx(pRowI, bAdjustLayout);
	}

	return TRUE;
}

BOOL CXTPReportControl::RemoveRecordEx(CXTPReportRecord* pRecord, BOOL bAdjustLayout)
{
	ASSERT(pRecord && m_pRecords);

	if (!pRecord || !m_pRecords)
		return FALSE;

	if (pRecord->HasChildren())
	{
		for (int i = pRecord->GetChilds()->GetCount() - 1; i >= 0 ; i--)
		{
			RemoveRecordEx(pRecord->GetChilds()->GetAt(i), bAdjustLayout);
		}

		return RemoveRecordEx(pRecord, bAdjustLayout);
	}

	BOOL bResult = FALSE;

	//--------------------------------------------------------
	CXTPReportRow* pRow0 = m_pPlainTree->FindInTree(pRecord);

	ASSERT_DBG_REMOVE_RECORD_EX(pRow0);

	if (pRow0)
	{
		CXTPReportRow* pRow = pRow0;
		pRow0 = NULL;

		do
		{
			CXTPReportRow* pRow_parent = pRow->GetParentRow();

			ASSERT(pRow->GetParentRows());

			// 1. remove from Rows Tree
			if (pRow->GetParentRows())
				VERIFY(pRow->GetParentRows()->RemoveRow(pRow) >= 0);

			// 2. remove from Display Rows array
			m_pRows->RemoveRow(pRow);

			pRow = pRow_parent;

		} while (pRow && pRow->IsGroupRow() && pRow->GetChilds()->GetCount() == 0);

		bResult = TRUE;
	}

	//-------------------------------------------------------
	//pRecord->Delete(); // the code below is more safe when record already removed!

	ASSERT(pRecord->GetRecords());

	if (pRecord->GetRecords())
	{
		BOOL bRecordRem = pRecord->GetRecords()->RemoveRecord(pRecord) >= 0;
		pRecord = NULL;
		ASSERT_DBG_REMOVE_RECORD_EX(bRecordRem);

		bResult |= bRecordRem;
	}

	if (m_nFocusedRow >= 0 && m_nFocusedRow >= m_pRows->GetCount())
		m_nFocusedRow = m_pRows->GetCount() - 1;

	//-------------------------------------------------------
	if (bResult)
		_RefreshIndexes(bAdjustLayout);

	return bResult;
}

void CXTPReportControl::DrawRows(CDC* pDC, CRect& rcClient)
{
	m_arrScreenRows.Clear(FALSE);

	pDC->SetBkMode(TRANSPARENT);

	int y = rcClient.top;

	int nRowCount = m_pRows->GetCount();

	if (0 == nRowCount)
	{
		pDC->SetTextColor(GetPaintManager()->m_clrWindowText);
		CString strNoItems = GetPaintManager()->m_strNoItems;

		if (!strNoItems.IsEmpty())
		{
			CRect rcText(rcClient);
			rcText.DeflateRect(5, 5, 5, 5);
			CXTPFontDC font(pDC, &GetPaintManager()->m_fontText);
			UINT uFlags = DT_CENTER | DT_TOP | DT_NOPREFIX | DT_WORDBREAK |
						  DT_WORD_ELLIPSIS | DT_END_ELLIPSIS | DT_EDITCONTROL;

			pDC->DrawText(strNoItems, rcText, uFlags);
		}
	}

	if (GetReportHeader()->GetNextVisibleColumn(-1, 1) == NULL)
		return;

	int nHeaderWidth = GetReportHeader()->GetWidth();


	for (int i = m_nTopRow; i < nRowCount; i++)
	{
		CXTPReportRow* pRow = m_pRows->GetAt(i);

		if (y > rcClient.bottom)
			break;

		CRect rcRow(rcClient.left, y, rcClient.left + nHeaderWidth,
					y + pRow->GetHeight(pDC, nHeaderWidth));

		pRow->Draw(pDC, rcRow, m_nLeftOffset);

		y += rcRow.Height();


		CXTPReportRow* pScreenRow = NULL;
		if (IsVirtualMode())
		{
			pScreenRow = CreateRow();
			pScreenRow->InitRow(pRow);
		}
		else
		{
			pScreenRow = pRow;
			pScreenRow->InternalAddRef();

		}
		m_arrScreenRows.InsertAt(m_arrScreenRows.GetCount(), pScreenRow);
	}

}

void CXTPReportControl::OnSelectionChanged()
{
	SendNotifyMessage(XTP_NM_REPORT_SELCHANGED);
	m_pSelectedRows->SetChanged(FALSE);
}

void CXTPReportControl::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs,
	XTP_REPORTRECORDITEM_METRICS* pMetrics)
{
	XTP_NM_REPORTITEMMETRICS nmData;
	nmData.pDrawArgs = pDrawArgs;
	nmData.pItemMetrics = pMetrics;

	SendNotifyMessage(XTP_NM_REPORT_GETITEMMETRICS, (NMHDR*)&nmData);
}



void CXTPReportControl::SetFocusedColumn(CXTPReportColumn* pColumn)
{
	if (m_pFocusedColumn != pColumn)
	{
		m_pFocusedColumn = pColumn;

		if (m_pFocusedColumn && m_bFocusSubItems)
		{
			CRect rc(m_pFocusedColumn->GetRect());

			if (rc.right >= m_rcReportArea.Width())
			{
				SetLeftOffset(m_nLeftOffset + min(rc.left, rc.right - m_rcReportArea.Width()));
			}
			else
			{
				int nFreezeColumnWidth = 0;
				if (m_nFreezeColumnsCount > 0)
				{
					int nFreezeColumnsCount = m_nFreezeColumnsCount;
					for (int i = 0; (i < m_pColumns->GetCount()) && (nFreezeColumnsCount > 0); i++)
					{
						CXTPReportColumn* pColumnCheck = m_pColumns->GetAt(i);

						if (pColumnCheck == m_pFocusedColumn)
						{
							nFreezeColumnWidth = 0;
							break;
						}

						if (pColumnCheck && pColumnCheck->IsVisible())
						{
							nFreezeColumnsCount--;
							nFreezeColumnWidth = pColumnCheck->GetRect().right;
						}
					}
				}

				if (rc.left - nFreezeColumnWidth <= 0 && m_nLeftOffset != 0)
				{
					SetLeftOffset(m_nLeftOffset + rc.left - nFreezeColumnWidth);
				}
			}
		}

		if (m_bFocusSubItems)
		{
			OnSelectionChanged();
		}
	}
}

void CXTPReportControl::SetFocusedRow(CXTPReportRow* pRow, BOOL bIgnoreSelection)
{
	EditItem(NULL);
	BeginUpdate();

	if (!bIgnoreSelection || !m_bMultipleSelection)
	{
		m_pSelectedRows->Select(pRow);
	}

	if (pRow)
	{
		m_nFocusedRow = pRow->GetIndex();
		EnsureVisible(pRow);
	}

	EndUpdate();

	if (m_pSelectedRows->IsChanged())
		OnSelectionChanged();
}

void CXTPReportControl::SetFocusedRow(CXTPReportRow* pRow, BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	if (!pRow)
		return;

	EditItem(NULL);
	BeginUpdate();

	if (m_bMultipleSelection)
	{
		if (bSelectBlock && m_nFocusedRow != -1)
		{
			m_pSelectedRows->SelectBlock(m_nFocusedRow, pRow->GetIndex());

		}
		else if (!bIgnoreSelection)
		{
			m_pSelectedRows->Select(pRow);
		}
	}
	else
	{
		m_pSelectedRows->Select(pRow);
	}

	m_nFocusedRow = pRow->GetIndex();

	EnsureVisible(pRow);
	EndUpdate();

	if (m_pSelectedRows->IsChanged())
		OnSelectionChanged();
}


void CXTPReportControl::SetLeftOffset(int nOffset)
{
	if (nOffset < 0)
		nOffset = 0;

	if (nOffset == m_nLeftOffset)
		return;

	m_nLeftOffset = nOffset;

	if (!m_bFullColumnScrolling)
		SetScrollPos(SB_HORZ, nOffset);

	AdjustScrollBars();
}

void CXTPReportControl::SetTopRow(int nIndex)
{
	if (nIndex == m_nTopRow)
		return;

	ASSERT(nIndex >= 0);

	if (nIndex < 0)
		nIndex = 0;

	m_nTopRow = nIndex;
	SetScrollPos(SB_VERT, nIndex);
	AdjustScrollBars();
}

void CXTPReportControl::EnsureVisible(CXTPReportRow* pCheckRow)
{
	int nCheckIndex = pCheckRow ? pCheckRow->GetIndex() : -1;

	if (nCheckIndex == -1 || !pCheckRow->m_bVisible || nCheckIndex >= m_pRows->GetCount())
		return;

	if (nCheckIndex < m_nTopRow)
	{
		SetTopRow(nCheckIndex);
		return;
	}

	CClientDC dc (this);

	int top = m_rcReportArea.top;
	int nHeaderWidth = GetReportHeader()->GetWidth();

	for (int i = m_nTopRow; i < m_pRows->GetCount(); i++)
	{
		CXTPReportRow* pRow = m_pRows->GetAt(i);
		int nRowHeight = pRow->GetHeight(&dc, nHeaderWidth);

		if (top + nRowHeight > m_rcReportArea.bottom)
			break;

		if (i == nCheckIndex)
			return;

		top += nRowHeight;
	}

	int nHeight = m_rcReportArea.Height();
	for (top = nCheckIndex; top >= 0; top--)
	{
		CXTPReportRow* pRow = m_pRows->GetAt(top);
		int nRowHeight = pRow->GetHeight(&dc, nHeaderWidth);

		if (nHeight - nRowHeight < 0)
		{
			if (top != nCheckIndex)
				top++;
			break;
		}

		nHeight -= nRowHeight;
	}
	SetTopRow(top);
	RedrawControl();
}

CXTPReportRow* CXTPReportControl::HitTest(CPoint pt) const
{
	if (!m_rcReportArea.PtInRect(pt))
		return NULL;

	for (int i = 0; i < m_arrScreenRows.GetCount(); i++)
	{
		CXTPReportRow* pRow = m_arrScreenRows.GetAt(i);

		CRect rc = pRow->GetRect();

		if (rc.PtInRect(pt))
			return pRow;
	}
	return NULL;
}

int CXTPReportControl::GetReportAreaRows(int nStartRow, BOOL bMoveDown)
{
	int nDirection = bMoveDown ? +1 : -1;

	int top = m_rcReportArea.top;

	CClientDC dc(this);
	int nHeaderWidth = GetReportHeader()->GetWidth();

	for (int i = nStartRow; (i < m_pRows->GetCount() && i >= 0); i += nDirection)
	{
		CXTPReportRow* pRow = m_pRows->GetAt(i);

		int rowHeight = pRow->GetHeight(&dc, nHeaderWidth);

		if (top + rowHeight > m_rcReportArea.bottom)
			return bMoveDown ? i - nStartRow - 1 : nStartRow - i - 1;

		top += rowHeight;
	}

	return bMoveDown ? m_pRows->GetCount() - nStartRow : nStartRow;
}

void CXTPReportControl::AdjustLayout()
{
	if (GetSafeHwnd() == 0)
		return;

	CXTPClientRect rc(this);

	int nHeaderWidth = m_rcHeaderArea.Width();

	CXTPReportHeader* pHeader = GetReportHeader();
	int nGroupByHeight = (m_bGroupByEnabled && pHeader) ? pHeader->GetGroupByHeight() : 0;
	m_rcGroupByArea.SetRect(0, 0, rc.Width(), nGroupByHeight);

	int nHeaderHeight = 0;
	int nFooterHeight = 0;

	if (m_bHeaderVisible)
	{
		CWindowDC dc (this);
		nHeaderHeight = GetPaintManager()->GetHeaderHeight(this, &dc);
	}

	if (m_bFooterVisible)
	{
		CWindowDC dc (this);
		nFooterHeight = GetPaintManager()->GetFooterHeight(this, &dc);
	}

	m_rcHeaderArea.SetRect(0, m_rcGroupByArea.bottom, rc.Width(), m_rcGroupByArea.bottom + nHeaderHeight);
	m_rcReportArea.SetRect(0, m_rcHeaderArea.bottom, rc.Width(), rc.Height() - nFooterHeight);
	m_rcFooterArea.SetRect(0, rc.Height() - nFooterHeight, rc.Width(), rc.Height());

	if (nHeaderWidth != m_rcHeaderArea.Width() && pHeader)
	{
		pHeader->AdjustColumnsWidth(m_rcHeaderArea.Width());
	}
}

CScrollBar* CXTPReportControl::GetScrollBarCtrl(int nBar) const
{
	if (DYNAMIC_DOWNCAST(CView, GetParent()))
		return GetParent()->GetScrollBarCtrl(nBar);

	return 0;
}

void CXTPReportControl::AdjustScrollBars()
{
	if (GetSafeHwnd() == 0)
		return;

	EditItem(NULL);

	int nHeight = m_rcReportArea.Height();

	if (nHeight <= 0)
		return;

	BeginUpdate();

	int nCount = m_pRows->GetCount() - 1;
	int nLinesCount = nCount;
	int nHeaderWidth = GetReportHeader()->GetWidth();

	{
		CClientDC dc(this);

		for (; nLinesCount >= 0; nLinesCount--)
		{

			nHeight -= m_pRows->GetAt(nLinesCount)->GetHeight(&dc, nHeaderWidth);

			if (nHeight < 0)
			{
				if (nLinesCount != nCount)
					nLinesCount++;

				break;
			}
		}
	}

	nLinesCount = max(nLinesCount, 0);

	if (m_nTopRow > nLinesCount)
	{
		m_nTopRow = nLinesCount;
		SetScrollPos(SB_VERT, m_nTopRow);
	}

	BOOL bEnabled = nLinesCount > 0;

	if (bEnabled)
	{
		SCROLLINFO  si ;
		si.cbSize = sizeof(SCROLLINFO);
		si.nPage = nCount - nLinesCount + 1;
		si.nMax = nCount;
		si.nMin = 0 ;
		si.fMask = SIF_PAGE | SIF_RANGE ;
		SetScrollInfo(SB_VERT, &si) ;
	}

	EnableScrollBarCtrl(SB_VERT, bEnabled);

	::EnableScrollBar(m_hWnd, SB_VERT, (bEnabled && IsWindowEnabled()) ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);

	m_bVScrollBarVisible = bEnabled;


	if (!GetReportHeader()->m_bAutoColumnSizing)
	{
		if (m_bFullColumnScrolling)
		{
			CXTPReportColumn *pPrev = NULL, *pCurr = NULL, *pNext = NULL;
			int nScrollPos, nScrollMax;
			GetReportHeader()->GetFulColScrollInfo(pPrev, pCurr, pNext, nScrollPos, nScrollMax);

			bEnabled = nScrollMax > 1;

			if (bEnabled)
			{
				SCROLLINFO  si ;
				si.cbSize = sizeof(SCROLLINFO);
				si.nPage = 1;
				si.nMax = nScrollMax - 1;
				si.nMin = 0;
				si.nPos = nScrollPos;
				si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
				SetScrollInfo(SB_HORZ, &si) ;
			}
			else
			{
				m_nLeftOffset = 0;
				SetScrollPos(SB_HORZ, m_nLeftOffset);
			}
		}
		else
		{
			nHeaderWidth = max(GetReportHeader()->GetWidth() - m_rcReportArea.Width(), 0);

			if (m_nLeftOffset > nHeaderWidth)
			{
				m_nLeftOffset = nHeaderWidth;
				SetScrollPos(SB_HORZ, m_nLeftOffset);
			}

			bEnabled = nHeaderWidth > 0;

			if (bEnabled)
			{
				SCROLLINFO  si ;
				si.cbSize = sizeof(SCROLLINFO);
				si.nPage = m_rcReportArea.Width();
				si.nMax = si.nPage + nHeaderWidth;
				si.nMin = 0 ;
				si.fMask = SIF_PAGE | SIF_RANGE ;
				SetScrollInfo(SB_HORZ, &si) ;
			}
		}

		EnableScrollBarCtrl(SB_HORZ, bEnabled);
		m_bHScrollBarVisible = bEnabled;
		::EnableScrollBar(m_hWnd, SB_HORZ, (bEnabled && IsWindowEnabled()) ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
	}


	AdjustLayout();

	EndUpdate();
}

void CXTPReportControl::SetFullColumnScrolling(BOOL bSet)
{
	m_bFullColumnScrolling = bSet;

	CXTPReportControl::OnHScroll(SB_TOP, 0, NULL);

	if (m_hWnd)
	{
		RedrawControl();
		UpdateWindow();
	}

	AdjustScrollBars();
}

//////////////////////////////////////////////////////////////////////////
// CXTPReportControl message handlers

void CXTPReportControl::OnPaint()
{
	CPaintDC dc(this);      // device context for painting
	CXTPClientRect rc(this);

	// start counting drawing time
#ifdef XTP_DEBUG
	LARGE_INTEGER iStartCount;
	QueryPerformanceCounter(&iStartCount);
#endif

	if (m_nLockUpdateCount == 0 && (IsChanged() || m_bmpCache.GetSafeHandle() == 0))
	{
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		m_bmpCache.DeleteObject();
		m_bmpCache.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());


		CBitmap* pOldBitmap = memDC.SelectObject(&m_bmpCache);

		OnDraw(&memDC);

		if (!IsWindowEnabled())
		{
			XTPImageManager()->DisableBitmap(memDC, rc, RGB(250, 250, 250), RGB(128, 128, 128));
		}


		dc.BitBlt(0, 0, rc.right, rc.bottom, &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldBitmap);
	}
	else
	{
		CXTPCompatibleDC memDC(&dc, &m_bmpCache);
		dc.BitBlt(0, 0, rc.right, rc.bottom, &memDC, 0, 0, SRCCOPY);
	}

	// count drawing time
#ifdef XTP_DEBUG
	LARGE_INTEGER iEndCount;
	QueryPerformanceCounter(&iEndCount);
	XTP_TRACE(_T("Draw counter ticks: %d\n"), iEndCount.LowPart-iStartCount.LowPart);
#endif

}

BOOL CXTPReportControl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;    // Don't erase the background.
}

void CXTPReportControl::OnDraw(CDC* pDC)
{
	if (GetExStyle() & WS_EX_RTLREADING)
	{
		pDC->SetTextAlign(TA_RTLREADING);
	}

	// draw new image
	pDC->FillSolidRect(CXTPClientRect(this), GetPaintManager()->GetControlBackColor(this));

	CXTPReportHeader* pHeader = GetReportHeader();

	CRect rcHeader = m_rcHeaderArea;
	rcHeader.right = rcHeader.left + pHeader->GetWidth();

	// draw header
	if (pHeader)
		pHeader->Draw(pDC, rcHeader, m_nLeftOffset);

	// draw group by box
	if (pHeader)
		pHeader->DrawGroupByControl(pDC, m_rcGroupByArea);

	DrawRows(pDC, m_rcReportArea);

	CRect rcFooter = m_rcFooterArea;
	if (rcFooter.Height() > 0)
		pHeader->DrawFooter(pDC, rcFooter, m_nLeftOffset);

	if (m_nDropPos != -1)
	{
		DrawDropMarker(pDC);
	}


	// update flag
	SetChanged(FALSE);
}

void CXTPReportControl::DrawDropMarker(CDC* pDC)
{
	CRect rc(m_rcReportArea.left, m_nDropPos, m_rcReportArea.right, m_nDropPos + 1);

	pDC->FillSolidRect(rc, GetPaintManager()->m_clrHotDivider);

	CXTPPenDC pen (*pDC, GetPaintManager()->m_clrHotDivider);
	CXTPBrushDC brush (*pDC, GetPaintManager()->m_clrHotDivider);

	int x = rc.left;
	int y = m_nDropPos;

	POINT ptsLeftArrow[] =
	{
		{x, y - 2}, {x + 2, y - 2}, {x + 2, y - 5}, {x + 7, y},
		{x + 2, y + 5}, {x + 2, y + 2}, {x, y + 2}
	};
	pDC->Polygon(ptsLeftArrow, 7);

	x = rc.right - 1;

	POINT ptsRightArrow[] =
	{
		{x, y - 2}, {x - 2, y - 2}, {x - 2, y - 5}, {x - 7, y},
		{x - 2, y + 5}, {x - 2, y + 2}, {x, y + 2}
	};
	pDC->Polygon(ptsRightArrow, 7);


}

void CXTPReportControl::OnSize(UINT nType, int cx, int cy)
{
	if (m_bOnSizeRunning)
		return;

	m_bOnSizeRunning = TRUE;

	CWnd::OnSize(nType, cx, cy);

	AdjustLayout();
	AdjustScrollBars();

	m_bOnSizeRunning = FALSE;
}

void CXTPReportControl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);

}

void CXTPReportControl::OnRButtonUp(UINT nFlags, CPoint point)
{
	HWND hWnd = m_hWnd;
	CWnd::OnRButtonUp(nFlags, point);

	if (!IsWindow(hWnd)) // Can be destroyed in WM_CONTEXTMENU
		return;

}


void CXTPReportControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	SetFocus();
	EditItem(NULL);

	// columns processing
	CXTPReportHeader* pHeader = GetReportHeader();
	if (pHeader)
	{
		pHeader->OnLButtonDown(point);
	}

	// rows selection
	CXTPReportRow* pRow = HitTest(point);
	if (pRow)
	{
		XTP_REPORTRECORDITEM_CLICKARGS clickArgs;
		clickArgs.pControl = this;
		clickArgs.pRow = pRow;
		clickArgs.ptClient = point;
		clickArgs.pColumn = NULL;

		// find clicked item
		clickArgs.pItem = pRow->HitTest(point, &clickArgs.rcItem, &clickArgs.pColumn);

		if (pRow->OnLButtonDown(&clickArgs))
			return;

		bool bIgnoreSelection = GetKeyState(VK_CONTROL) < 0;
		if (bIgnoreSelection)
			m_pSelectedRows->Invert(pRow);

		bool bSelectBlock = GetKeyState(VK_SHIFT) < 0;


		CXTPReportColumn* pFocusedColumn = clickArgs.pColumn;


		CUpdateContext updateContext(this);

		if (m_bFocusSubItems && pFocusedColumn && pRow->GetRecord() && pRow->GetRecord()->GetItem(pFocusedColumn)->IsFocusable())
		{
			SetFocusedColumn(pFocusedColumn);
		}

		if (pRow->IsSelected() && !bIgnoreSelection && !bSelectBlock)
		{
			m_pSelectOnUpRow = pRow;
		}
		else
		{
			SetFocusedRow(pRow, bSelectBlock, bIgnoreSelection);
		}

		m_pointDrag = point;
		m_bPrepareDrag = TRUE;

	}

}

void CXTPReportControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);

	m_bPrepareDrag = FALSE;

	EnsureStopAutoVertScroll();

	// columns processing
	CXTPReportHeader* pHeader = GetReportHeader();
	if (pHeader)
	{
		pHeader->OnLButtonUp(nFlags, point);
	}

	CXTPReportRow* pRow = HitTest(point);

	if (pRow && pRow == m_pSelectOnUpRow)
	{
		SetFocusedRow(pRow, FALSE, FALSE);
	}

	m_pSelectOnUpRow = NULL;

	if (!m_bMultipleSelection || (GetKeyState(VK_SHIFT) >= 0 && GetKeyState(VK_CONTROL) >= 0))
	{
		// rows processing
		if (pRow && pRow->IsFocused())
		{
			pRow->OnClick(point);
		}
	}


}

void CXTPReportControl::OnLButtonDblClk(UINT nFlags, CPoint ptDblClick)
{
	CWnd::OnLButtonDblClk(nFlags, ptDblClick);

	// columns processing
	CXTPReportHeader* pHeader = GetReportHeader();
	if (pHeader)
	{
		pHeader->OnLButtonDblClk(ptDblClick);
	}

	// rows processing
	CXTPReportRow* pRow = HitTest(ptDblClick);
	if (pRow)
	{
		pRow->OnDblClick(ptDblClick);
	}
	else
	{
		// just notify parent
		SendMessageToParent(NULL, NULL, NULL, NM_DBLCLK, &ptDblClick);
	}

}
void CXTPReportControl::OnContextMenu(CWnd* /*pWnd*/, CPoint pos)
{
	if (GetMouseMode() != xtpReportMouseNothing)
		return;

	CPoint ptClient = pos;
	ScreenToClient(&ptClient);

	// call context menu handler for report header if clicked inside
	if (m_rcHeaderArea.PtInRect(ptClient) ||
		m_rcGroupByArea.PtInRect(ptClient))
	{
		CXTPReportHeader* pHeader = GetReportHeader();
		if (pHeader)
			pHeader->OnContextMenu(ptClient);
		return;
	}

	// call context menu handler for report area if clicked inside
	if (m_rcReportArea.PtInRect(ptClient))
	{
		// rows processing
		CXTPReportRow* pRow = HitTest(ptClient);
		if (pRow)
		{
			SetFocusedRow(pRow, pRow->IsSelected());
			pRow->OnContextMenu(ptClient);
		}
		else
		{
			SendMessageToParent(NULL, NULL, NULL, NM_RCLICK, &pos);
		}

		return;
	}

	if (pos == CPoint(-1, -1))
	{
		CXTPReportRow* pFocusedRow = GetFocusedRow();
		if (pFocusedRow)
		{
			ptClient = CPoint(pFocusedRow->GetRect().left, pFocusedRow->GetRect().bottom);
			pFocusedRow->OnContextMenu(ptClient);
		}
		else
		{
			pos = m_rcReportArea.TopLeft();
			ClientToScreen(&pos);
			SendMessageToParent(NULL, NULL, NULL, NM_RCLICK, &pos);
		}
	}
}

CXTPReportColumn* CXTPReportControl::GetNextFocusableColumn(CXTPReportRow* pRow, int nColumnIndex, int nDirection)
{
	if (!pRow->GetRecord())
		return NULL;

	for (;;)
	{
		CXTPReportColumn* pColumn = GetReportHeader()->GetNextVisibleColumn(nColumnIndex, nDirection);
		if (!pColumn)
			return NULL;

		CXTPReportRecordItem* pItem = pRow->GetRecord()->GetItem(pColumn);

		if (pItem && pItem->IsFocusable())
			return pColumn;

		nColumnIndex = pColumn->GetIndex();
	}
}

BOOL CXTPReportControl::OnPreviewKeyDown(UINT& rnChar, UINT nRepCnt, UINT nFlags)
{
	XTP_NM_REPORTPREVIEWKEYDOWN nmParams;
	::ZeroMemory(&nmParams, sizeof(nmParams));

	nmParams.nChar = rnChar;
	nmParams.nRepCnt = nRepCnt;
	nmParams.nFlags = nFlags;
	nmParams.bCancel = FALSE;

	SendNotifyMessage(XTP_NM_REPORT_PREVIEWKEYDOWN, (NMHDR*)&nmParams);
	rnChar = nmParams.nChar;

	return !nmParams.bCancel;
}

void CXTPReportControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
//  TRACE(_T("ReportControl, OnKeyDown('%d') \n"), nChar);

// Moved to PreTranslateMessage
//  if (!OnPreviewKeyDown(nChar, nRepCnt, nFlags))
//  {
//      return;
//  }

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);

	BOOL  bSelectBlock = GetKeyState(VK_SHIFT) < 0;
	BOOL  bSignoreSelection = GetKeyState(VK_CONTROL) < 0;

	CXTPReportRow* pFocusedRow = GetFocusedRow();

	CXTPDrawHelpers::KeyToLayout(this, nChar);

	if (m_pRows->GetCount() != 0)
	switch (nChar)
	{
		case VK_ADD:
			if (bSignoreSelection)
				ExpandAll();
			else if (pFocusedRow && pFocusedRow->HasChildren() && !pFocusedRow->IsExpanded())
			{
				pFocusedRow->SetExpanded(TRUE);
			}
			break;

		case VK_SUBTRACT:
			if (bSignoreSelection)
				CollapseAll();
			else if (pFocusedRow && pFocusedRow->HasChildren() && pFocusedRow->IsExpanded())
			{
				pFocusedRow->SetExpanded(FALSE);
			}
			break;

		case VK_RIGHT:
			if (pFocusedRow && pFocusedRow->HasChildren() && !pFocusedRow->IsExpanded())
			{
				pFocusedRow->SetExpanded(TRUE);
				break;
			}
			if (pFocusedRow && m_bFocusSubItems && m_pFocusedColumn)
			{
				CXTPReportColumn* pColumn = GetNextFocusableColumn(pFocusedRow, m_pFocusedColumn->GetIndex(), +1);
				if (pColumn)
				{
					SetFocusedColumn(pColumn);
					SetFocusedRow(GetFocusedRow());
				}
				break;
			}

		case VK_DOWN:
			GetNavigator()->MoveDown(bSelectBlock, bSignoreSelection);
			break;

		case  VK_LEFT:
			if (pFocusedRow && pFocusedRow->HasChildren() && pFocusedRow->IsExpanded())
			{
				pFocusedRow->SetExpanded(FALSE);
				break;
			}
			if (pFocusedRow && m_bFocusSubItems && m_pFocusedColumn)
			{
				CXTPReportColumn* pColumn = GetNextFocusableColumn(pFocusedRow, m_pFocusedColumn->GetIndex(), -1);
				if (pColumn)
				{
					SetFocusedColumn(pColumn);
					SetFocusedRow(GetFocusedRow());
				}
				break;
			}
			if (pFocusedRow && !pFocusedRow->HasChildren() && pFocusedRow->GetParentRow() && !bSelectBlock)
			{
				SetFocusedRow(pFocusedRow->GetParentRow());
				pFocusedRow->SetExpanded(FALSE);
				break;
			}

		case VK_UP:
			GetNavigator()->MoveUp(bSelectBlock, bSignoreSelection);
			break;

		case VK_HOME:
			GetNavigator()->MoveFirstRow(bSelectBlock, FALSE);
			break;

		case VK_END:
			GetNavigator()->MoveLastRow(bSelectBlock, FALSE);
			break;

		case VK_NEXT:
			GetNavigator()->MovePageDown(bSelectBlock, FALSE);
			break;

		case VK_PRIOR:
			GetNavigator()->MovePageUp(bSelectBlock, FALSE);
			break;

		case VK_RETURN:
			if (pFocusedRow && pFocusedRow->HasChildren())
			{
				pFocusedRow->SetExpanded(!pFocusedRow->IsExpanded());
			}
			break;

		case VK_ESCAPE:
			if (m_mouseMode != xtpReportMouseNothing)
			{
				GetReportHeader()->CancelMouseMode();
			}
			break;

		case VK_F2:
			GetNavigator()->BeginEdit();
			break;
	}

	NMKEY nmgv;
	nmgv.nVKey = nChar;
	nmgv.uFlags = nFlags;
	SendNotifyMessage(NM_KEYDOWN, (NMHDR*)&nmgv);

}

void CXTPReportControl::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CXTPReportControl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CXTPReportControl::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CXTPReportControl::OnChar(UINT nChar, UINT nRepCntr, UINT nFlags)
{
//  TRACE(_T("ReportControl, OnChar('%d') \n"), nChar);

// Moved to PreTranslateMessage
//  if (!OnPreviewKeyDown(nChar, nRepCntr, nFlags))
//  {
//      return;
//  }

	NMCHAR nmgv;
	ZeroMemory(&nmgv, sizeof(NMCHAR));
	nmgv.ch = nChar;
	SendNotifyMessage(NM_CHAR, (NMHDR*)&nmgv);


	CXTPReportRow* pFocusedRow = GetFocusedRow();

	if (m_bFocusSubItems && pFocusedRow && (nChar == VK_TAB))
	{
		EditItem(NULL);

		BOOL bBack = GetKeyState(VK_SHIFT) < 0;

		GetNavigator()->MoveLeftRight(bBack);

		return;
	}

	if (m_pFocusedColumn && pFocusedRow && pFocusedRow->GetRecord() && (nChar != VK_RETURN) && (nChar != VK_ESCAPE))
	{
		XTP_REPORTRECORDITEM_ARGS itemArgs(this, pFocusedRow, m_pFocusedColumn) ;

		if (itemArgs.pItem && itemArgs.pItem->OnChar(&itemArgs, nChar))
			return;
	}

	CWnd::OnChar(nChar, nRepCntr, nFlags);
}

void CXTPReportControl::OnCaptureChanged(CWnd* pWnd)
{
	if (m_mouseMode != xtpReportMouseNothing)
	{
		GetReportHeader()->CancelMouseMode();
	}
	CWnd::OnCaptureChanged(pWnd);
}

void CXTPReportControl::OnEnable(BOOL bEnable)
{
	UNREFERENCED_PARAMETER(bEnable);

	AdjustScrollBars();
}

void CXTPReportControl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar != NULL)
	{
		CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	int nCurPos = m_nTopRow;

	// decide what to do for each diffrent scroll event
	switch (nSBCode)
	{
		case SB_TOP: nCurPos = 0; break;
		case SB_BOTTOM: nCurPos = GetScrollLimit(SB_VERT); break;
		case SB_LINEUP: nCurPos = max(nCurPos - 1, 0); break;
		case SB_LINEDOWN: nCurPos = min(nCurPos + 1, GetScrollLimit(SB_VERT)); break;

		case SB_PAGEUP: nCurPos = max(nCurPos - GetReportAreaRows(nCurPos, false), 0); break;
		case SB_PAGEDOWN: nCurPos = min(nCurPos + GetReportAreaRows(nCurPos, true), GetScrollLimit(SB_VERT)); break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			{
				SCROLLINFO si;
				ZeroMemory(&si, sizeof(SCROLLINFO));
				si.cbSize = sizeof(SCROLLINFO);
				si.fMask = SIF_TRACKPOS;

				if (!GetScrollInfo(SB_VERT, &si))
					return;
				nCurPos = si.nTrackPos;
			}
			break;
	}

	SetTopRow(nCurPos);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CXTPReportControl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar != NULL)
	{
		CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	int nCurPos = m_nLeftOffset;

	// FCS - full column scrolling
	int nLeftOffset_new = m_nLeftOffset;

	CXTPReportColumn *pPrev = NULL, *pCurr = NULL, *pNext = NULL;
	int nScrollPos = 0, nScrollMax = 0;
	int nFreezOffset = 0;
	if (m_bFullColumnScrolling)
		nFreezOffset = GetReportHeader()->GetFulColScrollInfo(pPrev, pCurr, pNext, nScrollPos, nScrollMax);

	// decide what to do for each different scroll event
	switch (nSBCode)
	{
		case SB_TOP:
			nCurPos = 0;
			nLeftOffset_new = 0;
			nScrollPos = 0;
			break;

		case SB_BOTTOM:
			nCurPos = GetScrollLimit(SB_HORZ);
			if (m_bFullColumnScrolling)
			{
				int nVisColCount = GetColumns()->GetVisibleColumnsCount();
				CXTPReportColumn *pLast = GetColumns()->GetVisibleAt(max(nVisColCount-1, 0));
				ASSERT(pLast);
				if (!pLast)
					return;

				nLeftOffset_new = labs(GetReportHeader()->m_rcHeader.left - pLast->GetRect().left) - nFreezOffset;
				nScrollPos = max(nScrollMax - 1, 0);
			}
			break;

		case SB_LINEUP:
			nCurPos = max(nCurPos - m_nHScrollStep, 0);
			if (m_bFullColumnScrolling && pPrev)
			{
				nLeftOffset_new = labs(GetReportHeader()->m_rcHeader.left - pPrev->GetRect().left) - nFreezOffset;
				nScrollPos = max(nScrollPos - 1, 0);
			}
			break;

		case SB_LINEDOWN:
			nCurPos = min(nCurPos + m_nHScrollStep, GetScrollLimit(SB_HORZ));
			if (m_bFullColumnScrolling && pNext)
			{
				nLeftOffset_new = labs(GetReportHeader()->m_rcHeader.left - pNext->GetRect().left) - nFreezOffset;
				nScrollPos = nScrollPos + 1;
			}
			break;

		case SB_PAGEUP:
			nCurPos = max(nCurPos - m_rcReportArea.Width(), 0);
			if (m_bFullColumnScrolling && pPrev)
			{
				nLeftOffset_new = labs(GetReportHeader()->m_rcHeader.left - pPrev->GetRect().left) - nFreezOffset;
				nScrollPos = max(nScrollPos - 1, 0);
			}
			break;

		case SB_PAGEDOWN:
			nCurPos = min(nCurPos + m_rcReportArea.Width(), GetScrollLimit(SB_HORZ));
			if (m_bFullColumnScrolling && pNext)
			{
				nLeftOffset_new = labs(GetReportHeader()->m_rcHeader.left - pNext->GetRect().left) - nFreezOffset;
				nScrollPos = nScrollPos + 1;
			}
			break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			nCurPos = nPos;
			if (m_bFullColumnScrolling)
			{
				int nVisColCount = GetColumns()->GetVisibleColumnsCount();

				int nCurrCol = nVisColCount - nScrollMax + nPos;
				CXTPReportColumn *pCurrScroll = GetColumns()->GetVisibleAt(nCurrCol);

				ASSERT(pCurrScroll);
				if (!pCurrScroll)
					return;

				nLeftOffset_new = labs(GetReportHeader()->m_rcHeader.left - pCurrScroll->GetRect().left) - nFreezOffset;
				nScrollPos = nPos;
			}
			break;
	}

	if (m_bFullColumnScrolling)
	{
		SetLeftOffset(nLeftOffset_new);
		SetScrollPos(SB_HORZ, nScrollPos);
	}
	else
	{
		SetLeftOffset(nCurPos);
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

UINT CXTPReportControl::GetMouseScrollLines()
{
	int nScrollLines = 3; // default value

	if (XTPSystemVersion()->IsWin95())
	{

		HKEY hKey;
		if (ERROR_SUCCESS == RegOpenKeyEx(
				HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 0, KEY_QUERY_VALUE, &hKey))
		{
			TCHAR szData[128];
			DWORD dwKeyDataType;
			DWORD dwDataBufSize = sizeof(szData);

			if (ERROR_SUCCESS == RegQueryValueEx(
					hKey, _T("WheelScrollLines"), NULL, &dwKeyDataType, (LPBYTE) &szData, &dwDataBufSize))
			{
				nScrollLines = _tcstoul(szData, NULL, 10);
			}
			RegCloseKey(hKey);
		}
	}
	// win98 or greater
	else
		SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);

	return nScrollLines;
}

BOOL CXTPReportControl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (!m_bVScrollBarVisible)
		return CWnd::OnMouseWheel(nFlags, zDelta, pt);

	UINT uiMsg;
	int nScrollsCount = 0;
	// calculate what should be sent
	if (m_nRowsPerWheel == -1)
	{
		// A m_nRowsPerWheel value less than 0 indicates that the mouse wheel scrolls whole pages, not just lines.
		int nPagesScrolled = zDelta / 120;
		uiMsg = nPagesScrolled > 0 ? SB_PAGEUP : SB_PAGEDOWN;
		nScrollsCount = nPagesScrolled > 0 ? nPagesScrolled : -nPagesScrolled;
	}
	else
	{
		int nRowsScrolled = m_nRowsPerWheel * zDelta / 120;
		uiMsg = nRowsScrolled > 0 ? SB_LINEUP : SB_LINEDOWN;
		nScrollsCount = nRowsScrolled > 0 ? nRowsScrolled : -nRowsScrolled;
	}

	BeginUpdate();

	// send scroll messages
	for (int i = 0; i < nScrollsCount; i++)
	{
		OnVScroll(uiMsg, 0, NULL);
	}

	EndUpdate();
	UpdateWindow();

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CXTPReportControl::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPReportControl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	CXTPReportHeader* pHeader = GetReportHeader();
	if (pHeader)
	{
		pHeader->OnMouseMove(nFlags, point);
	}

	if (GetMouseMode() == xtpReportMouseNothing)
	{
		CXTPReportRow* pRow = HitTest(point);
		if (pRow)
		{
			pRow->OnMouseMove(nFlags, point);

			if (m_bShowTooltips && nFlags == 0)
			{
				pRow->ShowToolTip(point, &m_wndTip);
			}
		}

		if (m_pHotRow != pRow)
		{
			TRACKMOUSEEVENT tme =
			{
				sizeof(TRACKMOUSEEVENT), TME_LEAVE, GetSafeHwnd(), 0
			};
			_TrackMouseEvent (&tme);

			m_pHotRow = pRow;
		}

		// If mouse moved some since down...
		if (m_bPrepareDrag && (labs (point.x - m_pointDrag.x) > 3 ||
			labs (point.y - m_pointDrag.y) > 3))
		{
			// Prevent duplicate
			m_bPrepareDrag = FALSE;

			m_pSelectOnUpRow = NULL;

			// Begin a drag operation
			OnBeginDrag(m_pointDrag);
		}
	}



}

void CXTPReportControl::OnBeginDrag(CPoint point)
{
	if (SendMessageToParent(NULL, NULL, NULL, LVN_BEGINDRAG, &point))
		return;

	if (m_cfReport ==  NULL)
		return;

	CXTPReportSelectedRows* pSelectedRows = GetSelectedRows();
	if (!pSelectedRows)
		return;

	if ((m_dwDragDropFlags & xtpReportAllowDrag) == 0)
		return;

	int nCount = pSelectedRows->GetCount();
	for (int i = nCount - 1; i >= 0; i--)
	{
		CXTPReportRow* pRow = pSelectedRows->GetAt(i);
		if (pRow->IsGroupRow())
		{
			pRow->SetExpanded(TRUE);
			pRow->SelectChilds();
		}
	}

	COleDataSource ds;

	int nRowsCount = pSelectedRows->GetCount();
	if (nRowsCount < 1)
		return;

	// minimize memory reallocs to improve performance
	UINT nAveRecordsSize = 1500; // bytes
	UINT nGrowBytes = ((nRowsCount * nAveRecordsSize) / 4096 + 1) * 4096;

	UINT nAllocFlags = GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT;

	CSharedFile fileRecords(nAllocFlags, nGrowBytes);

	BOOL bSucceed = FALSE;

	//------------------------------------------------------------------------
	const int cErrTextSize = 1024;
	TCHAR szErrText[cErrTextSize + 1];

	CXTPReportRecords* pDragRecords = new CXTPReportRecords(TRUE);

	if (!_GetSelectedRows(pDragRecords))
	{
		CMDTARGET_RELEASE(pDragRecords);
		return;
	}

	try
	{
		CArchive ar(&fileRecords, CArchive::store);
		CXTPPropExchangeArchive px(ar);

		bSucceed = _WriteRecordsData(&px, pDragRecords);

		ar.Close(); // perform Flush() and detach from file
	}
	catch(CArchiveException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPReportControl::Copy() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(CFileException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPReportControl::Copy() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(...)
	{
		TRACE(_T("EXCEPTION: CXTPReportControl::Copy() - Unhandled Exception!\n"));
	}

	if (!bSucceed)
	{
		CMDTARGET_RELEASE(pDragRecords);
		return;
	}

	HGLOBAL hGlobal = fileRecords.Detach();

	m_bDragMode = TRUE;

	DROPEFFECT dropEffectMask =
		((m_dwDragDropFlags & xtpReportAllowDragCopy) ? DROPEFFECT_COPY : 0) +
		((m_dwDragDropFlags & xtpReportAllowDragMove) ? DROPEFFECT_MOVE : 0);

	XTP_NM_REPORTDRAGDROP nmData;
	nmData.pRecords = pDragRecords;

	if (SendNotifyMessage(XTP_NM_REPORT_BEGINDRAG, (NMHDR*)&nmData) == -1)
		return;

	ds.CacheGlobalData (m_cfReport, hGlobal);
	DROPEFFECT dropEffect = ds.DoDragDrop(dropEffectMask);

	m_bDragMode = FALSE;

	if ((dropEffect == DROPEFFECT_MOVE) && (dropEffectMask & DROPEFFECT_MOVE))
	{
		Cut();
	}

	CMDTARGET_RELEASE(pDragRecords);

	EnsureStopAutoVertScroll();
}

DROPEFFECT CXTPReportControl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dropEffect = DROPEFFECT_MOVE;

	if ((dwKeyState & MK_CONTROL) == MK_CONTROL)
		dropEffect = DROPEFFECT_COPY;

	if (!m_nOLEDropMode)
	{
		if (m_cfReport == NULL)
			dropEffect = DROPEFFECT_NONE;

		if ((m_dwDragDropFlags & xtpReportAllowDrop) == 0)
			dropEffect = DROPEFFECT_NONE;

		if ((dropEffect != DROPEFFECT_NONE) && (!pDataObject || !pDataObject->IsDataAvailable(m_cfReport)))
			dropEffect = DROPEFFECT_NONE;
	}

	int nDropPos = m_nDropPos;
	m_nDropPos = -1;

	if (dropEffect != DROPEFFECT_NONE)
	{
		DoAutoVertScrollIfNeed(m_pointDrag, point);

		if (GetColumns()->GetSortOrder()->GetCount() == 0 && GetColumns()->GetGroupsOrder()->GetCount() == 0)
		{
			CXTPReportRow* pRow = HitTest(point);
			if (pRow && pRow->GetRecord())
				m_nDropPos = (pRow->GetRect().CenterPoint().y < point.y ? pRow->GetRect().bottom - 1: pRow->GetRect().top - 1);
		}
	}


	if (m_nDropPos != nDropPos)
	{
		RedrawControl();
	}

	return dropEffect;
}

void CXTPReportControl::OnTimer(UINT_PTR uTimerID)
{
	if (m_uAutoScrollTimerID == uTimerID)
	{
		CPoint ptMouse;
		if (::GetCursorPos(&ptMouse))
		{
			ScreenToClient(&ptMouse);
			DoAutoVertScrollIfNeed(m_pointDrag, ptMouse);
		}
		//TRACE(_T("AutoScrollTimer \n"));
	}

	CWnd::OnTimer(uTimerID);
}

void CXTPReportControl::EnsureStopAutoVertScroll()
{
	if (m_uAutoScrollTimerID)
	{
		KillTimer(m_uAutoScrollTimerID);
		m_uAutoScrollTimerID = 0;

		//TRACE(_T("AutoScroll STOP \n"));
	}
}

void CXTPReportControl::EnsureStartAutoVertScroll()
{
	if (!m_uAutoScrollTimerID)
	{
		m_uAutoScrollTimerID = SetTimer(XTP_REPORT_AUTO_SCROLL_TIMER_ID, XTP_REPORT_AUTO_SCROLL_TIMER_RESOLUTION_MS, NULL);
	}
}

void CXTPReportControl::DoAutoVertScrollIfNeed(CPoint ptClick, CPoint pt)
{
	if (!m_arrScreenRows.GetCount())
	{
		EnsureStopAutoVertScroll();
		return;
	}

	CXTPReportRow* pRow0 = m_arrScreenRows.GetAt(0);
	CXTPReportRow* pRow1 = m_arrScreenRows.GetAt(m_arrScreenRows.GetCount() - 1);

	if (!pRow0 || !pRow1)
	{
		EnsureStopAutoVertScroll();
		return;
	}

	int nDirection = 0;

	CRect rc0 = m_rcReportArea;   //pRow0->GetRect();
	CRect rc1 = m_rcReportArea;   //pRow1->GetRect();
	rc0.bottom = m_rcReportArea.top + 20;
	rc1.top = m_rcReportArea.bottom - 20;

	if (rc0.PtInRect(pt))
		nDirection = -1;
	else if (rc1.PtInRect(pt))
		nDirection = 1;

	if (!nDirection ||
		m_bDragMode &&
			(nDirection == 1 && pt.y - ptClick.y < 3 ||
			 nDirection == -1 && ptClick.y - pt.y < 3)
	   )
	{
		EnsureStopAutoVertScroll();
		return;
	}

	//-----------------------------------------------------
	int nTopRow = GetTopRowIndex();
	int nRowsCount = m_pRows->GetCount();

	if (nDirection == -1 && nTopRow > 0 ||
		nDirection == 1 &&
			(pRow1->GetIndex() < nRowsCount ||
			 pRow1->GetRect().bottom > m_rcReportArea.bottom)
		)
	{
		//TRACE(_T("AutoScroll direction=%d, nextRow=%d\n"), nDirection, nTopRow+nDirection);
		SetTopRow(nTopRow + nDirection);

		EnsureStartAutoVertScroll();
		return;
	}

	EnsureStopAutoVertScroll();
}

BOOL CXTPReportControl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	EnsureStopAutoVertScroll();

	CUpdateContext updateContext(this);
	int nDropPos = m_nDropPos;
	m_nDropPos = -1;

	if (dropEffect != DROPEFFECT_COPY && dropEffect != DROPEFFECT_MOVE)
		return FALSE;


	if (IsVirtualMode())
		return FALSE;

	if (!pDataObject || !pDataObject->IsDataAvailable(m_cfReport))
		return FALSE;

	if ((m_dwDragDropFlags & xtpReportAllowDrop) == 0)
		return FALSE;



	int nInsert = GetRecords()->GetCount();

	BOOL bAbove = TRUE;
	CXTPReportRecord* pTargetRecord = NULL;

	CXTPReportRow* pRow = HitTest(point);
	if (pRow)
	{
		bAbove = pRow->GetRect().CenterPoint().y < point.y ? FALSE : TRUE;

		pTargetRecord = pRow->GetRecord();
		if(pTargetRecord)
			nInsert = pTargetRecord->GetIndex() + (bAbove ? 0 : 1);
	}

	if (m_bDragMode && dropEffect == DROPEFFECT_MOVE)
	{
		if (nDropPos == -1)
			return FALSE;

		CXTPReportRecords* pDropRecords = new CXTPReportRecords(TRUE);

		if (!_GetSelectedRows(pDropRecords))
		{
			CMDTARGET_RELEASE(pDropRecords);
			return FALSE;
		}

		XTP_NM_REPORTDRAGDROP nmData;
		nmData.pRecords = pDropRecords;
		nmData.pTargetRecord = pTargetRecord;
		nmData.bAbove = bAbove;

		if (SendNotifyMessage(XTP_NM_REPORT_DROP, (NMHDR*)&nmData) == -1)
		{
			CMDTARGET_RELEASE(pDropRecords);
			return FALSE;
		}

		GetRecords()->Move(nInsert, pDropRecords);

		Populate();

		_SelectRows(pDropRecords);

		RedrawControl();

		SendNotifyMessage(XTP_NM_REPORT_RECORDS_DROPPED, (NMHDR*)&nmData);

		CMDTARGET_RELEASE(pDropRecords);

		return FALSE;
	}

	CFile* pFile = pDataObject->GetFileData(m_cfReport);
	if (!pFile)
		return FALSE;

	CXTPReportRecords* pDropRecords = new CXTPReportRecords();

	const int cErrTextSize = 1024;
	TCHAR szErrText[cErrTextSize + 1];

	try
	{
		CArchive ar(pFile, CArchive::load);
		CXTPPropExchangeArchive px(ar);

		if (!_ReadRecodsFromData(&px, *pDropRecords))
		{
			pDropRecords->RemoveAll();
		}
		ar.Close(); // detach from file
	}
	catch(CArchiveException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPReportControl::Paste() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(CFileException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPReportControl::Paste() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(...)
	{
		TRACE(_T("EXCEPTION: CXTPReportControl::Paste() - Unhandled Exception!\n"));
	}

	delete pFile;

	XTP_NM_REPORTDRAGDROP nmData;
	nmData.pRecords = pDropRecords;
	nmData.pTargetRecord = pTargetRecord;
	nmData.bAbove = bAbove;

	if (SendNotifyMessage(XTP_NM_REPORT_DROP, (NMHDR*)&nmData) == -1)
	{
		CMDTARGET_RELEASE(pDropRecords);
		return FALSE;
	}

	// Add and Populate records
	int nRecordsCount = pDropRecords->GetCount();
	if (nRecordsCount > 0)
	{
		// Add
		for (int i = 0; i < nRecordsCount; i++)
		{
			CXTPReportRecord* pRecord = pDropRecords->GetAt(i);
			if (pRecord)
			{
				CMDTARGET_ADDREF(pRecord);

				m_pRecords->InsertAt(nInsert, pRecord);
				nInsert++;
			}
		}

		Populate();

		_SelectRows(pDropRecords);

		RedrawControl();

		SendNotifyMessage(XTP_NM_REPORT_RECORDS_DROPPED, (NMHDR*)&nmData);
	}

	CMDTARGET_RELEASE(pDropRecords);

	return TRUE;

}


BOOL CXTPReportControl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		switch (m_mouseMode)
		{
		case xtpReportMouseOverColumnDivide:
			SetCursor(GetReportHeader()->m_hResizeCursor);
			return TRUE;
		}

	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}


LRESULT CXTPReportControl::SendNotifyMessage(UINT nMessage, NMHDR* pNMHDR) const
{
	if (!IsWindow(m_hWnd))
		return 0;

	NMHDR nmhdr;
	if (pNMHDR == NULL)
		pNMHDR = &nmhdr;

	pNMHDR->hwndFrom = GetSafeHwnd();
	pNMHDR->idFrom = GetDlgCtrlID();
	pNMHDR->code = nMessage;

	CWnd *pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
		return pOwner->SendMessage(WM_NOTIFY, pNMHDR->idFrom, (LPARAM)pNMHDR);
	else
		return 0;
}


LRESULT CXTPReportControl::SendMessageToParent(CXTPReportRow* pRow, CXTPReportRecordItem* pItem, CXTPReportColumn* pColumn, UINT nMessage, CPoint* pPoint, int nHyperlink) const
{
	if (!IsWindow(m_hWnd))
		return 0;

	XTP_NM_REPORTRECORDITEM nmgv;
	nmgv.pItem = pItem;
	nmgv.pColumn = pColumn;
	nmgv.pRow = pRow;
	nmgv.nHyperlink = nHyperlink;
	nmgv.pt.x = 0;
	nmgv.pt.y = 0;
	if (pPoint)
	{
		nmgv.pt = *pPoint;
	}

	return SendNotifyMessage(nMessage, (NMHDR*)&nmgv);
}

void CXTPReportControl::DoPropExchange(CXTPPropExchange* pPX)
{
	TRY
	{
		pPX->ExchangeSchemaSafe();

		CXTPPropExchangeSection secColumns(pPX->GetSection(_T("Columns")));
		m_pColumns->DoPropExchange(&secColumns);

		PX_Bool(pPX, _T("ShowGroupBox"), m_bGroupByEnabled, FALSE);
		PX_Int(pPX, _T("FreezeColumnsCount"), m_nFreezeColumnsCount, 0);

		if (pPX->GetSchema() >= _XTP_SCHEMA_110)
		{
			PX_Bool(pPX, _T("FullColumnScrolling"), m_bFullColumnScrolling, FALSE);
			PX_Int(pPX, _T("HScrollStep"), m_nHScrollStep, XTP_REPORT_HSCROLL_STEP);
		}

		CXTPPropExchangeSection secHeader(pPX->GetSection(_T("Header")));
		GetReportHeader()->DoPropExchange(&secHeader);

		if (pPX->IsLoading())
		{
			GetReportHeader()->OnColumnsChanged();
			Populate();
		}
	}
	CATCH(CArchiveException, e)
	{
	}
	END_CATCH
}

void CXTPReportControl::SerializeState(CArchive& ar)
{
	CXTPPropExchangeArchive px(ar);
	DoPropExchange(&px);
}

void CXTPReportControl::CollapseAll()
{
	BeginUpdate();
	for (int i = m_pRows->GetCount() - 1; i >= 0; i--)
		m_pRows->GetAt(i)->SetExpanded(FALSE);

	EndUpdate();
	EnsureVisible(GetFocusedRow());
}

void CXTPReportControl::ExpandAll()
{
	BeginUpdate();
	for (int i = m_pRows->GetCount() - 1; i >= 0; i--)
		m_pRows->GetAt(i)->SetExpanded(TRUE);

	EndUpdate();
	EnsureVisible(GetFocusedRow());
}

void CXTPReportControl::SetMouseMode(XTPReportMouseMode nMode)
{
	XTP_TRACE(_T("SetMouseMode: Switching from %d to %d\n"), m_mouseMode, nMode);
	m_mouseMode = nMode;
}

void CXTPReportControl::RelayToolTipEvent(UINT message)
{
	if (m_wndTip.GetSafeHwnd() && m_wndTip.IsWindowVisible())
	{
		CPoint pt;
		GetCursorPos(&pt);

		if (!m_wndTip.GetHoverRect().PtInRect(pt))
		{
			m_wndTip.SetTooltipText(NULL);
			m_wndTip.Activate(FALSE);
		}

		switch (message)
		{
			case WM_MOUSEWHEEL:
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_MOUSELEAVE:
				m_wndTip.Activate(FALSE);
		}
	}

}

BOOL CXTPReportControl::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	static BOOL bRelay = FALSE;
	if (m_wndTip.GetSafeHwnd() && m_wndTip.IsWindowVisible() && !bRelay)
	{
		bRelay = TRUE;
		RelayToolTipEvent(message);
		bRelay = FALSE;
	}

	if (m_pToolTipContext && m_bShowTooltips)
	{
		m_pToolTipContext->FilterToolTipMessage(this, message, wParam, lParam);
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

void CXTPReportControl::OnSysColorChange()
{
	CWnd::OnSysColorChange();
	m_pPaintManager->RefreshMetrics();

	RedrawControl();
}

UINT CXTPReportControl::OnGetDlgCode()
{
	return (m_bFocusSubItems ? DLGC_WANTTAB :0) | DLGC_WANTARROWS | DLGC_WANTCHARS;
}

void CXTPReportControl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	RedrawControl();

}

void CXTPReportControl::OnKillFocus (CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	EnsureStopAutoVertScroll();
	RedrawControl();

}

int CXTPReportControl::GetHeaderIndent() const
{
	return GetIndent(GetReportHeader()->m_nIndentLevel);
}

CXTPReportRow* CXTPReportControl::CreateRow()
{
	return new CXTPReportRow_Batch();
}

CXTPReportGroupRow* CXTPReportControl::CreateGroupRow()
{
	return new CXTPReportGroupRow_Batch();

}

void CXTPReportControl::EditItem(XTP_REPORTRECORDITEM_ARGS* pItemArgs)
{
	CXTPReportRecordItem* pItem = pItemArgs ? pItemArgs->pItem : NULL;

	if (m_pActiveItem != NULL)
	{
		m_pActiveItem->OnCancelEdit(this, TRUE);
		m_pActiveItem = NULL;

		if (!m_bFocusSubItems)
		{
			SetFocusedColumn(NULL);
		}
	}
	CMDTARGET_RELEASE(m_ptrVirtualEditingRow);

	if (pItem && pItemArgs && pItemArgs->pRow)
	{
		//if (!HasFocus())
		//  SetFocus();

		AdjustScrollBars();
		RedrawControl();
		UpdateWindow();

		if (IsVirtualMode())
		{
			int nRowIndex = pItemArgs->pRow->GetIndex();

			EnsureVisible(pItemArgs->pRow);

			RedrawControl();
			UpdateWindow();
			pItemArgs->pRow = NULL; // RedrawControl delete and re-create new screen rows

			BOOL bMapped = FALSE;
			int nScrCount = m_arrScreenRows.GetCount();
			for (int i = 0; i < nScrCount; i++)
			{
				CXTPReportRow* pRow = m_arrScreenRows.GetAt(i);
				if (pRow->GetIndex() == nRowIndex)
				{
					pItemArgs->pRow = pRow;
					bMapped = TRUE;

					ASSERT(m_ptrVirtualEditingRow == NULL);

					m_ptrVirtualEditingRow = pRow;
					CMDTARGET_ADDREF(m_ptrVirtualEditingRow);
					break;
				}
			}
			ASSERT(bMapped);
			if (!bMapped)
				return;
		}
		else if (GetFocusedRow() != pItemArgs->pRow)
		{
			SetFocusedRow(pItemArgs->pRow);
			UpdateWindow();
		}

		pItemArgs->rcItem = pItemArgs->pRow->GetItemRect(pItem);

		SetFocusedColumn(pItemArgs->pColumn);
		m_pActiveItem = pItem;
		pItem->OnBeginEdit(pItemArgs);
	}

	RedrawControl();
}

BOOL CXTPReportControl::HasFocus() const
{
	const CWnd* pFocusWnd = GetFocus();
	if (!pFocusWnd)
		return FALSE;

	return (pFocusWnd->GetSafeHwnd() == m_hWnd) || (pFocusWnd->GetParent()->GetSafeHwnd() == m_hWnd) || (pFocusWnd->GetOwner()->GetSafeHwnd() == m_hWnd);
}


void CXTPReportControl::ReleaseItem(int nIndex)
{
	int i;

	for (i = 0; i < m_pRecords->GetCount(); i++)
	{
		CXTPReportRecord* pRecord = m_pRecords->GetAt(i);

		pRecord->m_arrItems[nIndex]->InternalRelease();
		pRecord->m_arrItems.RemoveAt(nIndex);
	}

	CXTPReportColumns* pColumns = GetColumns();

	CXTPReportColumn* pColumnToRemove = NULL;

	int nColumnsCount = pColumns->GetCount();
	for (i = 0; i < nColumnsCount; i++)
	{
		CXTPReportColumn* pColumn = pColumns->GetAt(i);

		if (pColumn->m_nItemIndex > nIndex)
		{
			pColumn->m_nItemIndex--;
		}
		else if (pColumn->m_nItemIndex == nIndex)
		{
			pColumnToRemove = pColumn;
		}
	}

	if (pColumnToRemove)
		pColumns->Remove(pColumnToRemove);
}

void CXTPReportControl::SetVirtualMode(CXTPReportRecord* pVirtualRecord, int nCount)
{
	m_pRecords->SetVirtualMode(pVirtualRecord, nCount);
}

BOOL CXTPReportControl::IsVirtualMode() const
{
	return m_pRecords->IsVirtualMode();
}

CXTPReportRow* CXTPReportControl::GetFocusedRow() const
{
	return m_nFocusedRow != -1 ? m_pRows->GetAt(m_nFocusedRow) : NULL;
}

void CXTPReportControl::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	CWnd::OnStyleChanged(nStyleType, lpStyleStruct);

	RedrawControl();
}

CXTPToolTipContext* CXTPReportControl::GetToolTipContext() const
{
	return m_pToolTipContext;
}

INT_PTR CXTPReportControl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// check child windows first by calling CControlBar
	INT_PTR nHit = CWnd::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

	nHit = GetReportHeader()->OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

	CXTPReportRow* pRow = HitTest(point);
	if (pRow)
	{
		nHit = pRow->OnToolHitTest(point, pTI);
	}

	return nHit;
}

void CXTPReportControl::SetLayoutRTL(BOOL bRightToLeft)
{
	if (!XTPSystemVersion()->IsLayoutRTLSupported())
		return;

	if (!m_hWnd)
		return;

	ModifyStyleEx(bRightToLeft ? 0 : WS_EX_LAYOUTRTL, !bRightToLeft ? 0 : WS_EX_LAYOUTRTL);
	GetImageManager()->DrawReverted(bRightToLeft);

	GetInplaceEdit()->DestroyWindow();

	AdjustLayout();
}

//////////////////////////////////////////////////////////////////////////
// Clipboard operations

BOOL CXTPReportControl::CanCopy()
{
	// Check whether are there any selected rows to be copied
	CXTPReportSelectedRows* pSelRows = GetSelectedRows();

	if ((pSelRows != NULL) && (pSelRows->GetCount() > 0))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CXTPReportControl::CanCut()
{
	if (IsVirtualMode())
		return FALSE;

	return CanCopy();
}

BOOL CXTPReportControl::CanPaste()
{
	if (IsVirtualMode())
		return FALSE;

	CLIPFORMAT uCF_Records = (CLIPFORMAT)::RegisterClipboardFormat(XTPREPORTCTRL_CF_RECORDS);

	BOOL bCan = FALSE;
	COleDataObject odj;
	if (odj.AttachClipboard())
	{
		bCan = odj.IsDataAvailable(CF_TEXT)
			|| odj.IsDataAvailable(CF_UNICODETEXT)
			|| odj.IsDataAvailable(uCF_Records) ;
	}
	return bCan;
}

void CXTPReportControl::Cut()
{
	CWaitCursor _WC;

	// Copy selected data into the clipboard
	Copy();

	if (IsVirtualMode())
		return;

	// Delete selected rows
	CXTPInternalCollectionT<CXTPReportRow> arSelectedRows;
	_GetSelectedRows(NULL, &arSelectedRows);
	int nSelRowsCount = (int)arSelectedRows.GetSize();
	int nFirstSelRow = INT_MAX;
	for (int i = nSelRowsCount - 1; i >= 0 ; i--)
	{
		CXTPReportRow* pRow = arSelectedRows.GetAt(i, FALSE);
		if (pRow)
		{
			nFirstSelRow = min(nFirstSelRow, pRow->GetIndex());
			VERIFY(RemoveRowEx(pRow, FALSE));
		}

		arSelectedRows.RemoveAt(i);
	}

	if (GetSelectedRows())
	{
		GetSelectedRows()->Clear();

		if (nFirstSelRow != INT_MAX)
		{
			m_nFocusedRow = min(nFirstSelRow, GetRows()->GetCount() - 1);

			if (GetFocusedRow())
			{
				SetFocusedRow(GetFocusedRow());
				GetSelectedRows()->Add(GetFocusedRow());
			}
		}
	}

	AdjustScrollBars();
	RedrawControl();

}

// We support text format with \t dividers for record items and
// \r\n dividers for records (simple tab-separated text).
// Such format is also supported by Excel and some other applications.
void CXTPReportControl::Copy()
{
	CWaitCursor _WC;

	int nRowsCount = GetSelectedRows() ? GetSelectedRows()->GetCount() : 1;

	// minimize memory reallocs to improve performance
	UINT nAveRecordsSize = 1500; // bytes
	UINT nGrowBytes = ((nRowsCount * nAveRecordsSize) / 4096 + 1) * 4096;

	UINT nAllocFlags = GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT;

	CSharedFile fileRecords(nAllocFlags, nGrowBytes);

	BOOL bIsRecordsData = FALSE;

	//------------------------------------------------------------------------
	const int cErrTextSize = 1024;
	TCHAR szErrText[cErrTextSize + 1];

	try
	{
		CArchive ar(&fileRecords, CArchive::store);
		CXTPPropExchangeArchive px(ar);

		bIsRecordsData = _WriteSelectedRowsData(&px);

		ar.Close(); // perform Flush() and detach from file
	}
	catch(CArchiveException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPReportControl::Copy() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(CFileException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPReportControl::Copy() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(...)
	{
		TRACE(_T("EXCEPTION: CXTPReportControl::Copy() - Unhandled Exception!\n"));
	}

	//*****
	CString strClipText = _GetSelectedRowsVisibleColsText();

	CLIPFORMAT uCF_Records = (CLIPFORMAT)::RegisterClipboardFormat(XTPREPORTCTRL_CF_RECORDS);

	// Put prepared text into the clipboard
	if (OpenClipboard())
	{
		::EmptyClipboard();

		// 1 - Text
		int nLen = (strClipText.GetLength() + 1) * sizeof(TCHAR);
		HGLOBAL hText = ::GlobalAlloc(nAllocFlags, nLen);

		if (hText != NULL)
		{
			LPTSTR lptstrCopy = (TCHAR*)GlobalLock(hText);
			STRCPY_S(lptstrCopy, strClipText.GetLength() + 1, (LPCTSTR)strClipText);
			GlobalUnlock(hText);

#ifndef _UNICODE
			::SetClipboardData(CF_TEXT, hText);
#else
			::SetClipboardData(CF_UNICODETEXT, hText);
#endif
		}

		// 2 - Blob data
		if (bIsRecordsData)
		{
			HGLOBAL hData = fileRecords.Detach();
			::GlobalUnlock(hData); // unlock data
			::SetClipboardData(uCF_Records, hData);
		}

		//-----------------
		::CloseClipboard();
	}
}

void CXTPReportControl::Paste()
{
	if (IsVirtualMode())
		return;

	CWaitCursor _WC;

	CLIPFORMAT uCF_Records = (CLIPFORMAT)::RegisterClipboardFormat(XTPREPORTCTRL_CF_RECORDS);

	CXTPReportRecords arRecords;
	BOOL bTryPasteFromText = TRUE;

	// Retrieve text from the clipboard
	if (!OpenClipboard())
		return;

	if (::IsClipboardFormatAvailable(uCF_Records))
	{
		HGLOBAL hPasteData = ::GetClipboardData(uCF_Records);

		if (hPasteData)
		{
			bTryPasteFromText = FALSE;

			const int cErrTextSize = 1024;
			TCHAR szErrText[cErrTextSize + 1];

			CSharedFile fileSahred;
			fileSahred.SetHandle(hPasteData, FALSE);
			CArchive ar(&fileSahred, CArchive::load);

			try
			{
				CXTPPropExchangeArchive px(ar);

				if (!_ReadRecodsFromData(&px, arRecords))
				{
					arRecords.RemoveAll();
				}
			}
			catch(CArchiveException* pE)
			{
				if (pE->GetErrorMessage(szErrText, cErrTextSize))
				{
					TRACE(_T("EXCEPTION: CXTPReportControl::Paste() - %s\n"), szErrText);
				}
				pE->Delete();
			}
			catch(CFileException* pE)
			{
				if (pE->GetErrorMessage(szErrText, cErrTextSize))
				{
					TRACE(_T("EXCEPTION: CXTPReportControl::Paste() - %s\n"), szErrText);
				}
				pE->Delete();
			}
			catch(...)
			{
				TRACE(_T("EXCEPTION: CXTPReportControl::Paste() - Unhandled Exception!\n"));
			}

			//*********
			ar.Close(); // detach from file
			fileSahred.Detach(); //detach from data
			::GlobalUnlock(hPasteData); // unlock data
		}
	}

	UINT uCF_TText = sizeof(TCHAR) == 2 ? CF_UNICODETEXT : CF_TEXT;

	if (bTryPasteFromText && ::IsClipboardFormatAvailable(uCF_TText))
	{
		// Try to get text data from the clipboard
		HGLOBAL hglbPaste = ::GetClipboardData(uCF_TText);

		// Import Text data into the control
		if (hglbPaste != NULL)
		{
			TCHAR* lpszClipboard = (TCHAR*)GlobalLock(hglbPaste);

			if (!_ReadRecodsFromText(lpszClipboard, arRecords))
			{
				arRecords.RemoveAll();
			}
			::GlobalUnlock(hglbPaste);
		}
	}

	::CloseClipboard();
	//////////////////////////////////////////////////////////////////////////

	CUpdateContext updateContext(this);

	// Add and Populate records
	int nRecordsCount = arRecords.GetCount();
	if (nRecordsCount > 0)
	{
		// Add
		for (int i = 0; i < nRecordsCount; i++)
		{
			CXTPReportRecord* pRecord = arRecords.GetAt(i);
			if (pRecord)
			{
				CMDTARGET_ADDREF(pRecord);
				AddRecord(pRecord);
			}
		}

		Populate();

		// Select added records
		_SelectRows(&arRecords);
	}
}

// We support text format with \t dividers for record items and
// \r\n dividers for records (simple tab-separated text).
// Such format is also supported by Excel and some other applications.
CString CXTPReportControl::_GetSelectedRowsVisibleColsText()
{
	CString strSelText;

	CXTPReportColumns* pColumns = GetColumns();
	if (NULL == pColumns)
		return _T("");

	const int nColumnCount = pColumns->GetVisibleColumnsCount();

	// Iterate over the selected rows and prepare corresponding records text
	CXTPReportSelectedRows* pSelectedRows = GetSelectedRows();
	if ((pSelectedRows != NULL) && (pSelectedRows->GetCount() > 0))
	{
		POSITION pos = pSelectedRows->GetFirstSelectedRowPosition();
		while (pos)
		{
			CXTPReportRow* pRow = pSelectedRows->GetNextSelectedRow(pos);
			if (NULL == pRow)
				break;

			CXTPReportRecord* pRecord = pRow->GetRecord();
			if (NULL == pRecord)
				continue;

			CStringArray arStrings;
			for (int nCol = 0; nCol < nColumnCount; nCol++)
			{
				CXTPReportColumn* pColumn = pColumns->GetVisibleAt(nCol);
				CXTPReportRecordItem* pItem = pRecord->GetItem(pColumn);
				if (NULL == pItem)
					continue;

				arStrings.Add(pItem->GetCaption(pColumn));
			}

			//===============================================================
			BOOL bCanceled = OnBeforeCopyToText(pRecord, arStrings);
			if (bCanceled)
			{
				continue;
			}

			strSelText += XTPStrMake(arStrings, _T("\t"));
			strSelText += _T("\r\n");
		}
	}

	return strSelText;
}

BOOL CXTPReportControl::_ReadRecodsFromText(LPCTSTR pcszText,
											CXTPReportRecords& rarRecords)
{
	//////////////////////////////////////////////////////////////////////////
	// Insert retrieved text into the control

	CStringArray arRecordsStrings;
	XTPStrSplit(pcszText, _T("\r\n"), arRecordsStrings);

	int nCount = (int)arRecordsStrings.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CString strRecord = arRecordsStrings[i];
		if (strRecord.IsEmpty())
		{
			continue;
		}

		CXTPReportRecord* pNewRec = _CreateRecodFromText(strRecord);
		// pNewRec = NULL - paste was handled by the user

		if (pNewRec)
		{
			rarRecords.Add(pNewRec);
		}
	}

	return TRUE;
}

CXTPReportRecord* CXTPReportControl::_CreateRecodFromText(LPCTSTR pcszRecord)
{
	if (!GetColumns())
	{
		ASSERT(FALSE);
		return NULL;
	}

	CStringArray arStrings;

	// Read each field from the initial string and set visible field one by one
	XTPStrSplit(pcszRecord, _T("\t"), arStrings);

	//=======================================================================
	CXTPReportRecord* pRecord = NULL;
	BOOL bHandled = OnBeforePasteFromText(arStrings, &pRecord);
	if (bHandled)
	{
		return pRecord;
	}

#if _MFC_VER >= 0x0600 // Not supported by Visual Studio 5.0

	if (!pRecord)
	{
		pRecord = new CXTPReportRecord();
		if (!pRecord)
		{
			return NULL;
		}

		int nDataCount = (int)arStrings.GetSize();

		// Fill record with all items
		int nCount = GetColumns()->GetCount();
		for (int i = 0; i < nCount; i++)
		{
			COleVariant varItem(_T(""));

			CXTPReportRecordItem* pItem = new CXTPReportRecordItemVariant(varItem);
			if (!pItem)
			{
				CMDTARGET_RELEASE(pRecord);
				return NULL;
			}
			pRecord->AddItem(pItem);
		}

		// Iterate all visible columns and set text for each next
		const int nColumnCount = GetColumns()->GetVisibleColumnsCount();
		for (int nCol = 0; nCol < nColumnCount; nCol++)
		{
			COleVariant varItem(nCol < nDataCount ? (LPCTSTR)arStrings[nCol] : _T(""));

			CXTPReportColumn* pColumn = GetColumns()->GetVisibleAt(nCol);
			CXTPReportRecordItem* pItem = pRecord->GetItem(pColumn);
			ASSERT(pItem);
			if (NULL == pItem)
				continue;

			CXTPReportRecordItemVariant* pItemVar = DYNAMIC_DOWNCAST(CXTPReportRecordItemVariant, pItem);
			ASSERT(pItemVar);

			if (pItemVar)
			{
				pItemVar->m_oleValue = varItem;
			}
		}
	}
#endif

	return pRecord;
}

BOOL CXTPReportControl::_WriteRecordsData(CXTPPropExchange* pPX, CXTPReportRecords* pRecords)
{
	if (!pRecords)
		return FALSE;


	long nSchema = XTP_REPORT_CB_RECORDS_DATA_VER;

	PX_Long(pPX, _T("Version"), (long&)nSchema);
	pPX->ExchangeLocale();


	CXTPPropExchangeSection secRecords(pPX->GetSection(_T("ReportRecords")));

	int nRecordsCount = (int)pRecords->GetCount();
	CXTPPropExchangeEnumeratorPtr pEnumRecords(secRecords->GetEnumerator(_T("Record")));
	POSITION posRecord = pEnumRecords->GetPosition((DWORD)nRecordsCount);

	for (int i = 0; i < nRecordsCount; i++)
	{
		CXTPPropExchangeSection secRecord(pEnumRecords->GetNext(posRecord));

		CXTPReportRecord* pRecord = pRecords->GetAt(i);
		PX_Object(&secRecord, pRecord, RUNTIME_CLASS(CXTPReportRecord));
	}

	return TRUE;
}

void CXTPReportControl::_SelectRows(CXTPReportRecords* pRecords)
{
	CXTPReportRows* pRows = GetRows();
	CXTPReportSelectedRows* pSelRows = GetSelectedRows();
	if (!pRows || !pSelRows)
		return;

	pSelRows->Clear();
	int nRecordsCount = pRecords->GetCount();

	for (int nNewRecNr = 0; nNewRecNr < nRecordsCount; nNewRecNr++)
	{
		CXTPReportRecord* pRec = pRecords->GetAt(nNewRecNr);
		CXTPReportRow* pRow = pRows->Find(pRec);
		if (pRow)
		{
			pSelRows->Add(pRow);

			if (nNewRecNr == nRecordsCount - 1)
				SetFocusedRow(pRow, TRUE);
		}
	}
}

BOOL CXTPReportControl::_GetSelectedRows(CXTPReportRecords* pRecords,
										 CXTPInternalCollectionT<CXTPReportRow> *pRows)
{
	ASSERT(!pRecords || pRecords->m_bArray == TRUE);

	CXTPReportSelectedRows* pSelectedRows = GetSelectedRows();
	if (!pSelectedRows || !pSelectedRows->GetCount() || (!pRecords && !pRows))
	{
		return FALSE;
	}

	POSITION pos = pSelectedRows->GetFirstSelectedRowPosition();
	while (pos)
	{
		CXTPReportRow* pRow = pSelectedRows->GetNextSelectedRow(pos);
		CXTPReportRecord* pRecord = pRow ? pRow->GetRecord() : NULL;
		if (NULL == pRow)
		{
			break;
		}
		if (pRecord && pRecords)
		{
			pRecords->Add(pRecord);
		}

		if (pRow && pRows)
		{
			pRows->AddPtr(pRow, TRUE);
		}
	}

	return TRUE;
}

BOOL CXTPReportControl::_WriteSelectedRowsData(CXTPPropExchange* pPX)
{
	if (!pPX || !pPX->IsStoring())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPReportRecords* pSelectedRecords= new CXTPReportRecords(TRUE);

	if (!_GetSelectedRows(pSelectedRecords) || !_WriteRecordsData(pPX, pSelectedRecords))
	{
		CMDTARGET_RELEASE(pSelectedRecords);
		return FALSE;
	}

	CMDTARGET_RELEASE(pSelectedRecords);
	return TRUE;
}

BOOL CXTPReportControl::_ReadRecodsFromData(CXTPPropExchange* pPX,
											CXTPReportRecords& rarRecords)
{
	rarRecords.RemoveAll();

	if (!pPX || !pPX->IsLoading())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPPropExchangeSection secRecords(pPX->GetSection(_T("ReportRecords")));

	long nSchema = 0;

	PX_Long(pPX, _T("Version"), (long&)nSchema);
	pPX->ExchangeLocale();

	if (nSchema != XTP_REPORT_CB_RECORDS_DATA_VER)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPPropExchangeEnumeratorPtr pEnumRecords(secRecords->GetEnumerator(_T("Record")));
	POSITION posRecord = pEnumRecords->GetPosition();

	while (posRecord)
	{
		CXTPPropExchangeSection secRecord(pEnumRecords->GetNext(posRecord));

		CXTPReportRecord* pRecord = NULL;
		PX_Object(&secRecord, pRecord, RUNTIME_CLASS(CXTPReportRecord));

		if (!pRecord)
		{
			AfxThrowArchiveException(CArchiveException::badClass);
		}
		CXTPReportRecord* pRecord2 = pRecord;
		BOOL bCanceled = OnBeforePaste(&pRecord2);

		if (bCanceled || pRecord2 != pRecord)
		{
			CMDTARGET_RELEASE(pRecord);
		}
		if (bCanceled)
		{
			continue;
		}

		rarRecords.Add(pRecord2);
	}

	return TRUE;
}

BOOL CXTPReportControl::OnBeforeCopyToText(CXTPReportRecord* pRecord, CStringArray& rarStrings)
{
	XTP_NM_REPORT_BEFORE_COPYPASTE nmParams;
	::ZeroMemory(&nmParams, sizeof(nmParams));

	CXTPReportRecord* pRecordTmp = pRecord;
	nmParams.ppRecord = &pRecordTmp;
	nmParams.parStrings = &rarStrings;

	LRESULT lResult = SendNotifyMessage(XTP_NM_REPORT_BEFORE_COPY_TOTEXT, (NMHDR*)&nmParams);

	return lResult != 0;
}

BOOL CXTPReportControl::OnBeforePasteFromText(CStringArray& arStrings,
	CXTPReportRecord** ppRecord)
{
	XTP_NM_REPORT_BEFORE_COPYPASTE nmParams;
	::ZeroMemory(&nmParams, sizeof(nmParams));

	nmParams.parStrings = &arStrings;
	nmParams.ppRecord = ppRecord;

	LRESULT lResult = SendNotifyMessage(XTP_NM_REPORT_BEFORE_PASTE_FROMTEXT, (NMHDR*)&nmParams);

	return lResult != 0;
}

BOOL CXTPReportControl::OnBeforePaste(CXTPReportRecord** ppRecord)
{
	XTP_NM_REPORT_BEFORE_COPYPASTE nmParams;
	::ZeroMemory(&nmParams, sizeof(nmParams));

	nmParams.ppRecord = ppRecord;

	LRESULT lResult = SendNotifyMessage(XTP_NM_REPORT_BEFORE_PASTE, (NMHDR*)&nmParams);

	return lResult != 0;
}

BOOL CXTPReportControl::IsEditMode()
{
	CXTPReportInplaceEdit* pEdit = GetInplaceEdit();

	BOOL bEditMode = pEdit && pEdit->GetSafeHwnd() && pEdit->IsWindowVisible();
	bEditMode |= GetInplaceList() && GetInplaceList()->GetSafeHwnd() && GetInplaceList()->IsWindowVisible();

	return bEditMode;
}


BOOL CXTPReportControl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (!OnPreviewKeyDown((UINT&)pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)) )
		{
			TRACE(_T("ReportControl, PreTranslateMessagem-OnPreviewKeyDown('%d') = CANCEL  \n"), pMsg->wParam);
			return TRUE;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CXTPReportControl::OnConstraintSelecting(CXTPReportRow* pRow, CXTPReportRecordItem* pItem, CXTPReportColumn* pColumn,
		CXTPReportRecordItemConstraint* pConstraint)
{
	XTP_NM_REPORTCONSTRAINTSELECTING    nmConstraint;
	::ZeroMemory(&nmConstraint, sizeof(nmConstraint));

	nmConstraint.pRow = pRow;
	nmConstraint.pColumn = pColumn;
	nmConstraint.pItem = pItem;
	nmConstraint.pConstraint = pConstraint;

	LRESULT lResult = SendNotifyMessage(XTP_NM_REPORT_CONSTRAINT_SELECTING, (NMHDR*)&nmConstraint);

	return lResult != 0;
}

const XTP_NM_REPORTTOOLTIPINFO& CXTPReportControl::OnGetToolTipInfo(CXTPReportRow* pRow, CXTPReportRecordItem* pItem, CString& rstrToolTipText)
{
	::ZeroMemory(m_pCachedToolTipInfo, sizeof(XTP_NM_REPORTTOOLTIPINFO));

	m_pCachedToolTipInfo->pRow = pRow;
	m_pCachedToolTipInfo->pItem = pItem;
	m_pCachedToolTipInfo->pstrText = &rstrToolTipText;

	SendNotifyMessage(XTP_NM_REPORT_GETTOOLTIPINFO, (NMHDR*)m_pCachedToolTipInfo);

	return *m_pCachedToolTipInfo;
}
