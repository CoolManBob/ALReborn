// XTPCalendarThemeOffice2007.cpp : implementation file
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
#include "Resource.h"

#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPOffice2007Image.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPPropExchange.h"

#include "XTPCalendarThemeOffice2007.h"
#include "XTPCalendarPaintManager.h"

#include "XTPCalendarControl.h"
#include "XTPCalendarDayViewEvent.h"
#include "XTPCalendarEvent.h"
#include "XTPCalendarEventLabel.h"
#include "XTPCalendarDayView.h"
#include "XTPCalendarWeekView.h"
#include "XTPCalendarMonthView.h"
#include "XTPCalendarData.h"
#include "XTPCalendarDayViewTimeScale.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
#define XTP_CALENDAR_OFFICE2007_FONT_NAME   _T("Segoe UI")

#define XTP_CALENDAR_OFFICE2007_BASE_BLUE   RGB(93, 140, 201) // default base color for a "blue" theme
#define XTP_CALENDAR_OFFICE2007_BASE_GREEN  RGB(114, 164, 90) // default base color for a "green" theme

#define XTP_CALENDAR_OFFICE2007_TODAYBORDER RGB(238, 147, 17)

//===========================================================================
// Should be a prime number:
// 37, 53, 79 , 101, 127, 199, 503, 1021, 1511, 2003, 3001, 4001, 5003, 6007, 8009, 12007, 16001, 32003, 48017, 64007
#define XTP_CALENDAR_OFFICE2007_DVG_DEV_AVE_COUNT 37

const int CXTPCalendarThemeOffice2007::cnGripperSize = 5;

//////////////////////////////////////////////////////////////////////////
//{{AFX_CODEJOCK_PRIVATE
struct XTP_CALENDAR_EVENT_ICON_INFO
{
	// common data
	RECT                 m_rcDestination;

	// image data
	CXTPOffice2007Image* m_pImage;
	RECT                 m_rcSrc;

	// icon data
	CXTPImageManagerIcon* m_pIcon;
};
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
class CXTPTextDC
{
public:

	CXTPTextDC(CDC* pDC, COLORREF clrText = COLORREF_NULL, COLORREF clrBk = COLORREF_NULL)
	{
		ASSERT(pDC);

		m_pDC = pDC;
		m_clrText_prev = COLORREF_NULL;
		m_clrBk_prev = COLORREF_NULL;

		SetColors(clrText, clrBk);
	}

	virtual ~CXTPTextDC()
	{
		SetColors(m_clrText_prev, m_clrBk_prev);
	}

	void SetColors(COLORREF clrText, COLORREF clrBk = COLORREF_NULL)
	{
		ASSERT(m_pDC);
		if (!m_pDC)
			return;

		if (clrText != COLORREF_NULL)
		{
			COLORREF clrText_prev = m_pDC->SetTextColor(clrText);
			if (m_clrText_prev == COLORREF_NULL)
				m_clrText_prev = clrText_prev;
		}

		if (clrBk != COLORREF_NULL)
		{
			COLORREF clrBk_prev = m_pDC->SetBkColor(clrBk);
			if (m_clrBk_prev == COLORREF_NULL)
				m_clrBk_prev = clrBk_prev;
		}
	}

protected:
	CDC*     m_pDC;

	COLORREF m_clrText_prev;
	COLORREF m_clrBk_prev;
};
//}}AFX_CODEJOCK_PRIVATE

//////////////////////////////////////////////////////////////////////////
//class CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder
//{{AFX_CODEJOCK_PRIVATE
class CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder
{
public:
	CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder();

	virtual void BiuldEventPaces(CXTPCalendarDayViewGroup* pViewGroup);

protected:
	class CViewEventsMap : public CMap<CXTPCalendarDayViewEvent*, CXTPCalendarDayViewEvent*, int, int>
	{
	public:
		CViewEventsMap();
		int GetMinFreeEventPlace();
		int GetMaxEventPlace();
		int GetMaxEventPlaceCountEx();
	};

	virtual void _Clear();
	virtual void _UpdateDataFor(CXTPCalendarDayViewEvent* pViewEvent, CXTPCalendarDayViewGroup* pViewGroup);
	virtual void _UpdateEventsPaces(CXTPCalendarDayViewGroup* pViewGroup);

	//CXTPCalendarTypedPtrAutoDeleteMap<CXTPCalendarDayViewEvent*, CViewEventsMap*> m_mapEvToOverlapped;
	// to avoid warning C4786 CObject* used.
	CXTPCalendarTypedPtrAutoDeleteMap<CXTPCalendarDayViewEvent*, CObject*> m_mapEvToOverlapped;

	COleDateTimeSpan m_spCellDuration;
	COleDateTime     m_dtMinStartTime;
protected:
	virtual CViewEventsMap* _XGetDataFor(CXTPCalendarDayViewEvent* pViewEvent);

	virtual void _XAddOverlapped(CXTPCalendarDayViewEvent* pVEv_base, CXTPCalendarDayViewEvent* pVEv_overlapped);
};
//}}AFX_CODEJOCK_PRIVATE

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CXTPCalendarViewEventSubjectEditor2007, CXTPCalendarViewEventSubjectEditor)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
END_MESSAGE_MAP()

CXTPCalendarViewEventSubjectEditor2007::CXTPCalendarViewEventSubjectEditor2007(CXTPCalendarControl* pOwner,
		CXTPCalendarViewEvent* pViewEvent, CXTPCalendarThemeOffice2007* pTheme2007) :
			CXTPCalendarViewEventSubjectEditor(pOwner)
{
	ASSERT(pViewEvent);
	ASSERT(pTheme2007);

	m_pViewEvent = pViewEvent;
	m_pTheme2007 = pTheme2007;

	if (!pViewEvent || !pTheme2007)
	{
		return;
	}

	CXTPCalendarThemeOffice2007::CTOEvent* pEventPart = m_pTheme2007->GetThemePartForEvent(m_pViewEvent);
	ASSERT(pEventPart);

	if (pEventPart)
	{
		COLORREF clrBorderNotSel;
		COLORREF clrBorder;
		pEventPart->GetEventColors(pViewEvent, clrBorder, clrBorderNotSel, m_grclrBk, FALSE);

		if (m_grclrBk.IsNull())
		{
			m_grclrBk.SetStandardValue(RGB(255,255,255));
		}
	}
}


CXTPCalendarViewEventSubjectEditor2007::~CXTPCalendarViewEventSubjectEditor2007()
{
}

BOOL CXTPCalendarViewEventSubjectEditor2007::OnEraseBkgnd(CDC* pDC)
{
	UNUSED_ALWAYS(pDC);

	if (!m_pViewEvent || !m_pOwner)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CRect rcEvent = m_pViewEvent->GetEventRect();

	m_pOwner->ClientToScreen(&rcEvent);
	ScreenToClient(&rcEvent);

	CXTPClientRect rcClient(this);

	XTPDrawHelpers()->GradientFill(pDC, &rcEvent, m_grclrBk, FALSE, &rcClient);

	return TRUE;
}

HBRUSH CXTPCalendarViewEventSubjectEditor2007::CtlColor(CDC* pDC, UINT nCtlColor)
{
	ASSERT(nCtlColor == CTLCOLOR_EDIT);
	UNREFERENCED_PARAMETER(nCtlColor);

	static CBrush s_null_brush;
	if (s_null_brush.m_hObject == NULL)
	{
		s_null_brush.CreateStockObject(NULL_BRUSH);
	}

	pDC->SetBkMode(TRANSPARENT);

	DWORD dwStyle = GetStyle();
	BOOL bMultiLine = (dwStyle & ES_MULTILINE) != 0;

	return bMultiLine ? (HBRUSH)s_null_brush : (HBRUSH)NULL;
}

void CXTPCalendarViewEventSubjectEditor2007::OnChange()
{
	Invalidate();
}



/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPCalendarThemeOffice2007, CXTPCalendarTheme)

////////////////////////////////////////////////////////////////////////
CSize CXTPCalendarThemeOffice2007::DrawText_Auto2SL3ML(CDC* pDC,
		LPCTSTR pcszText1, LPCTSTR pcszText2, LPCTSTR pcszText3,
		CThemeFontColorSet* pFontColor1, CThemeFontColorSet* pFontColor2,
		CThemeFontColorSet* pFontColor3,
		CRect& rcRect, LPCTSTR pcszText1Separator)

{
	CXTPFontDC autoFont(pDC, pFontColor2->Font());
	CSize sz2 = pDC->GetTextExtent(pcszText2);

	autoFont.SetFont(pFontColor1->Font());
	CSize sz1 = pDC->GetTextExtent(pcszText1);

	if ((sz1.cy + sz2.cy) > rcRect.Height())
	{
		CString strText1 = pcszText1;
		if (_tcslen(pcszText2) > 0)
		{
			strText1 += pcszText1Separator;
		}
		return DrawLine2_CenterLR(pDC, strText1, pcszText2, pFontColor1,
										 pFontColor2, rcRect, DT_VCENTER | DT_LEFT,
										 DT_VCENTER | DT_LEFT);
	}

	//=======================================================================
	UINT uFormat = DT_NOPREFIX | DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EDITCONTROL;
	int nYspace = sz2.cy/4;

	int nHeight1 = 0, nHeight2 = 0, nHeight3 = 0;
	//--------------------
	autoFont.SetColor(pFontColor1->Color());
	CRect rcText1 = rcRect;
	nHeight1 = pDC->DrawText(pcszText1, &rcText1, uFormat);

	//--------------------
	CRect rcText2 = rcRect;
	rcText2.top = min(rcRect.top + nHeight1 + nYspace, rcRect.bottom);

	if (sz2.cy < rcText2.Height() && _tcslen(pcszText2))
	{
		autoFont.SetFontColor(pFontColor2->Font(), pFontColor2->Color());
		nHeight2 = pDC->DrawText(pcszText2, &rcText2, uFormat);
	}

	int nTLen3 = (int)_tcslen(pcszText3);
	if (pcszText3 && nTLen3)
	{
		autoFont.SetFontColor(pFontColor3->Font(), pFontColor3->Color());

		CRect rcText3 = rcRect;
		rcText3.top = min(rcText2.top + nHeight2 + nYspace, rcRect.bottom);

		if (sz2.cy < rcText3.Height())
		{
			nHeight3 = pDC->DrawText(pcszText3, &rcText3, uFormat);
		}
	}

	//--------------------
	return CSize(rcRect.Width(), nHeight1 + nHeight2 + nHeight3 + 2 * nYspace);
}

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarThemeOffice2007::CXTPCalendarThemeOffice2007()
{
	Init();

	RefreshMetrics();

}

CXTPCalendarThemeOffice2007::~CXTPCalendarThemeOffice2007()
{

}

CXTPCalendarViewEventSubjectEditor* CXTPCalendarThemeOffice2007::StartEditSubject(CXTPCalendarViewEvent* pViewEvent)
{
	if (!pViewEvent || !pViewEvent->GetCalendarControl())
	{
		ASSERT(FALSE);
		return NULL;
	}

	CXTPCalendarViewEventSubjectEditor2007* pSE;
	pSE = new CXTPCalendarViewEventSubjectEditor2007(pViewEvent->GetCalendarControl(),
													 pViewEvent, this);
	if (!pSE)
	{
		return NULL;
	}

	DWORD dwEditStyles = WS_CHILD | WS_VISIBLE | ES_NOHIDESEL | ES_MULTILINE;

	if (pViewEvent->IsMultidayEvent())
	{
		dwEditStyles |= ES_AUTOHSCROLL /*| ES_CENTER*/;
	}
	else
	{
		dwEditStyles |= ES_AUTOVSCROLL | ES_MULTILINE;
	}

	CRect rcEditor = pViewEvent->GetSubjectEditorRect();
	pSE->Create(dwEditStyles, rcEditor, pViewEvent->GetCalendarControl(), 0);

	pSE->SetFocus();
	CFont* pEditorFont = NULL;

	CXTPCalendarThemeOffice2007::CTOEvent* pEventPart = GetThemePartForEvent(pViewEvent);
	ASSERT(pEventPart);

	if (pEventPart)
	{
		pEditorFont = pEventPart->m_fcsetNormal.fcsetSubject.Font();
	}
	ASSERT(pEditorFont);

	if (pEditorFont)
	{
		pSE->SetFont(pEditorFont);
	}

	pSE->SetWindowText(XTP_SAFE_GET2(pViewEvent, GetEvent(), GetSubject(), _T("")));
	pSE->SetSel(0, 0);

	return pSE;
}

CXTPCalendarThemeOffice2007::CTOEvent* CXTPCalendarThemeOffice2007::GetThemePartForEvent(CXTPCalendarViewEvent* pViewEvent)
{
	if (!pViewEvent)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CXTPCalendarThemeOffice2007::CTOEvent* pEventPart = NULL;

	CXTPCalendarDayViewEvent* pVEDay = DYNAMIC_DOWNCAST(CXTPCalendarDayViewEvent, pViewEvent);
	CXTPCalendarWeekViewEvent* pVEWeek= DYNAMIC_DOWNCAST(CXTPCalendarWeekViewEvent, pViewEvent);
	CXTPCalendarMonthViewEvent* pVEMonth = DYNAMIC_DOWNCAST(CXTPCalendarMonthViewEvent, pViewEvent);
	if (pVEDay)
	{
		if (pViewEvent->IsMultidayEvent())
		{
			pEventPart = GetDayViewPartX()->GetDayPartX()->GetGroupPartX()->GetMultiDayEventPartX();
		}
		else
		{
			pEventPart = GetDayViewPartX()->GetDayPartX()->GetGroupPartX()->GetSingleDayEventPartX();
		}
	}
	else if (pVEWeek)
	{
		if (pViewEvent->IsMultidayEvent())
		{
			pEventPart = GetWeekViewPartX()->GetDayPartX()->GetMultiDayEventPartX();
		}
		else
		{
			pEventPart = GetWeekViewPartX()->GetDayPartX()->GetSingleDayEventPartX();
		}
	}
	else if (pVEMonth)
	{
		if (pViewEvent->IsMultidayEvent())
		{
			pEventPart = GetMonthViewPartX()->GetDayPartX()->GetMultiDayEventPartX();
		}
		else
		{
			pEventPart = GetMonthViewPartX()->GetDayPartX()->GetSingleDayEventPartX();
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	return pEventPart;
}

void CXTPCalendarThemeOffice2007::GetItemTextIfNeed(int nItem, CString* pstrText,
													CXTPCalendarViewDay* pViewDay)
{
	if (!pstrText || !pViewDay)
	{
		ASSERT(FALSE);
		return;
	}

	DWORD dwFlags = GetAskItemTextFlags();

	if (dwFlags & nItem)
	{
		XTP_CALENDAR_GETITEMTEXT_PARAMS objRequest;
		::ZeroMemory(&objRequest, sizeof(objRequest));

		objRequest.nItem = nItem;
		objRequest.pstrText = pstrText;
		objRequest.pViewDay = pViewDay;

		SendNotificationAlways(XTP_NC_CALENDAR_GETITEMTEXT, (WPARAM)&objRequest, 0);
	}
}

void CXTPCalendarThemeOffice2007::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarTheme::RefreshMetrics(FALSE);

	m_clrBaseColor.SetStandardValue(XTP_CALENDAR_OFFICE2007_BASE_BLUE);
	//m_clrBaseColor.SetStandardValue(XTP_CALENDAR_OFFICE2007_BASE_GREEN);

	// Try to set default Office 2007 font,
	// otherwise icon font used.
	BOOL bFontExists = XTPDrawHelpers()->FontExists(XTP_CALENDAR_OFFICE2007_FONT_NAME);
	if (bFontExists)
	{
		LOGFONT lfBaseFont;
		::ZeroMemory(&lfBaseFont, sizeof(lfBaseFont));

		STRCPY_S(lfBaseFont.lfFaceName, LF_FACESIZE, XTP_CALENDAR_OFFICE2007_FONT_NAME);
		lfBaseFont.lfCharSet = DEFAULT_CHARSET;
		lfBaseFont.lfHeight = -11;
		lfBaseFont.lfWeight = FW_NORMAL;
		if (XTPSystemVersion()->IsClearTypeTextQualitySupported()) lfBaseFont.lfQuality = 5;


		m_fntBaseFont.SetStandardValue(&lfBaseFont);

		LOGFONT lfBaseFontBold = lfBaseFont;
		lfBaseFontBold.lfWeight = FW_BOLD;

		m_fntBaseFontBold.SetStandardValue(&lfBaseFontBold);
	}

	if (!GetColorsSetPart() || !m_pImagesStd || !m_pImageList)
	{
		return;
	}

	//-- set dynamic colors -------------------------
	// selected cell for blue, green
	// 41, 76, 122  {93, 140, 201}
	// 63, 91, 50 [114, 164, 90]  114

	GetColorsSetPart()->SetCLR_DarkenOffset(xtpCLR_DarkenOffset);

	GetColorsSetPart()->SetColor(xtpCLR_SelectedBk, 114); // 51, 77, 111

	GetColorsSetPart()->SetColor(xtpCLR_HeaderBorder, 76); // 141, 174, 217

	// Day View colors
	GetColorsSetPart()->SetColor(xtpCLR_DayBorder,                      0);   // 93, 140, 201

	GetColorsSetPart()->SetColor(xtpCLR_DayViewCellWorkBk,              255); // 255, 255, 255
	GetColorsSetPart()->SetColor(xtpCLR_DayViewCellNonWorkBk,           216); // 230, 237, 247

	GetColorsSetPart()->SetColor(xtpCLR_DayViewCellWorkBorderBottomInHour, 216); // 230, 237, 247
	GetColorsSetPart()->SetColor(xtpCLR_DayViewCellWorkBorderBottomHour,   113); // 165, 191, 225

	GetColorsSetPart()->SetColor(xtpCLR_DayViewCellNonWorkBorderBottomInHour, 189); // 213, 225, 241
	GetColorsSetPart()->SetColor(xtpCLR_DayViewCellNonWorkBorderBottomHour,   113); // 165, 191, 225

	GetColorsSetPart()->SetColor(xtpCLR_DayViewAllDayEventsBk,                113); // 165, 191, 225
	GetColorsSetPart()->SetColor(xtpCLR_DayViewAllDayEventsBorderBottom,      0);   // 93, 140, 201

	GetColorsSetPart()->SetColor(xtpCLR_MultiDayEventBorder,         0);   // 93, 140, 201
	GetColorsSetPart()->SetColor(xtpCLR_MultiDayEventSelectedBorder, 255); // 0, 0, 0
	GetColorsSetPart()->SetColor(xtpCLR_MultiDayEventBkGRfrom,       249); // 251, 252, 254  multi-day event gradient fill Light
	GetColorsSetPart()->SetColor(xtpCLR_MultiDayEventBkGRto,         161); // 195, 213, 235 multi-day event gradient fill Dark

	GetColorsSetPart()->SetColor(xtpCLR_MultiDayEventFromToDates,     38); // 117, 157, 209

	GetColorsSetPart()->SetColor(xtpCLR_DayViewSingleDayEventBorder,          0);   // 93, 140, 201
	GetColorsSetPart()->SetColor(xtpCLR_DayViewSingleDayEventSelectedBorder,  255); // 0, 0, 0
	GetColorsSetPart()->SetColor(xtpCLR_DayViewSingleDayEventBkGRfrom,        255); // 255, 255, 255 // single-day event gradient fill Light
	GetColorsSetPart()->SetColor(xtpCLR_DayViewSingleDayEventBkGRto,          155); // 191, 210, 234 // single-day event gradient fill Dark

	// Month view colors
	GetColorsSetPart()->SetColor(xtpCLR_MonthViewDayBkLight,    255); // 255, 255, 255
	GetColorsSetPart()->SetColor(xtpCLR_MonthViewDayBkDark,     113); // 165, 191, 225
	GetColorsSetPart()->SetColor(xtpCLR_MonthViewDayBkSelected, 216); // 230, 237, 247

	GetColorsSetPart()->SetColor(xtpCLR_MonthViewEventTime,  114); // 51, 77, 111

	GetColorsSetPart()->SetColor(xtpCLR_MonthViewSingleDayEventBorder,          217); // 230, 238, 247
	GetColorsSetPart()->SetColor(xtpCLR_MonthViewSingleDayEventSelectedBorder,  255); // 0, 0, 0
	GetColorsSetPart()->SetColor(xtpCLR_MonthViewSingleDayEventBkGRfrom,        250); // 252, 253, 254
	GetColorsSetPart()->SetColor(xtpCLR_MonthViewSingleDayEventBkGRto,          181); // 208, 222, 240

	// Week view colors are same as month view by default
	GetColorsSetPart()->SetColor(xtpCLR_WeekViewDayBkLight,    255); // 255, 255, 255
	GetColorsSetPart()->SetColor(xtpCLR_WeekViewDayBkDark,     113); // 165, 191, 225
	GetColorsSetPart()->SetColor(xtpCLR_WeekViewDayBkSelected, 216); // 230, 237, 247

	GetColorsSetPart()->SetColor(xtpCLR_WeekViewEventTime,  114); // 51, 77, 111

	GetColorsSetPart()->SetColor(xtpCLR_WeekViewSingleDayEventBorder,          217); // 230, 238, 247
	GetColorsSetPart()->SetColor(xtpCLR_WeekViewSingleDayEventSelectedBorder,  255); // 0, 0, 0
	GetColorsSetPart()->SetColor(xtpCLR_WeekViewSingleDayEventBkGRfrom,        250); // 252, 253, 254
	GetColorsSetPart()->SetColor(xtpCLR_WeekViewSingleDayEventBkGRto,          181); // 208, 222, 240

	//----------------------------------------------------------------------
	if (m_pImagesStd)
	{
		VERIFY( m_pImagesStd->InitResourceHandle(_T("Office2007_Calendar_Header.bmp")) );
	}

	//-----------------------------------------------------------------------
	_LoadStdBitmap(xtpIMG_DayViewHeader,        _T("Office2007_Calendar_Header.bmp"));
	_LoadStdBitmap(xtpIMG_DayViewHeaderTooday,  _T("Office2007_Calendar_TodayHeader.bmp"));
	_LoadStdBitmap(xtpIMG_DayViewGroupHeader,   _T("Office2007_Calendar_HeaderDVGroup.bmp"));

	_LoadStdBitmap(xtpIMG_MonthViewDayHeader,       _T("Office2007_Calendar_HeaderMVDay.bmp"));
	_LoadStdBitmap(xtpIMG_MonthViewDayHeaderToday,  _T("Office2007_Calendar_TodayHeaderMVDay.bmp"));

	//_LoadStdBitmap(xtpIMG_MonthViewWeekDayHeader,   _T("Office2007_Calendar_HeaderMVWeekDay.bmp"));

	_LoadStdBitmap(xtpIMG_MonthViewWeekHeader,      _T("Office2007_Calendar_HeaderMVWeek.bmp"));
	_LoadStdBitmap(xtpIMG_MonthViewWeekHeader,      _T("Office2007_Calendar_HeaderMVWeek.bmp"));

	_LoadStdBitmap(xtpIMG_WeekViewDayHeader,        _T("Office2007_Calendar_HeaderWVDay.bmp"));

	_LoadStdBitmap(xtpIMG_ExpandDayButtons,         _T("Office2007_Calendar_ExpandDay.bmp"));


	//-----------------------------------------------------------------------
	if (bRefreshChildren)
	{
		CXTPCalendarThemePart::RefreshMetrics();
	}
}

int CXTPCalendarThemeOffice2007::CTODay::HitTestExpandDayButton(CXTPCalendarViewDay* pViewDay, const CPoint* pPoint)
{
	if (!pViewDay || !GetTheme() || !GetTheme()->GetCalendarControl())
	{
		ASSERT(FALSE);
		return 0;
	}

	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pViewDay);
	CRect rc = dayLayout.m_rcDay;
	rc.right -= 5;
	rc.bottom -= 5;

	return _HitTestExpandDayButton(pViewDay, rc, pPoint);
}

void CXTPCalendarThemeOffice2007::CTODayViewHeader::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	TBase::RefreshMetrics(bRefreshChildren);

	RefreshFromParent(pThemeX->GetHeaderPartX());
}

void CXTPCalendarThemeOffice2007::CTODayView::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	TBase::RefreshMetrics(bRefreshChildren);
}


int CXTPCalendarThemeOffice2007::CTODayViewTimeScale::CalcMinRowHeight(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale)
{
	if (!pDC || !pTimeScale)
	{
		ASSERT(FALSE);
		return 0;
	}

	int nHeight = TBase::CalcMinRowHeight(pDC, pTimeScale);

	if (GETTOTAL_MINUTES_DTS(pTimeScale->GetScaleInterval()) % 60 == 0 && nHeight > 0)
	{
		nHeight -= 1;
	}

	return nHeight;
}

void CXTPCalendarThemeOffice2007::CTODayViewTimeScale::RefreshMetrics(BOOL)
{
	ASSERT(GetTheme());
	if (!GetTheme())
	{
		return;
	}

	TBase::RefreshMetrics();

	XTP_SAFE_CALL1(GetHeightFormulaPart(), SetStandardValue(18, 13, 5));
	m_hflaBigHourFont.SetStandardValue(1, 1, -1);

	//-----------------------------
	// 243, 243, 247 - TS bk
	// 124, 124, 148 - TS lines, text

	m_clrBackground.SetStandardValue(RGB(227, 239, 255));
	m_clrLine.SetStandardValue(RGB(101, 147, 207));

	COLORREF clrText = RGB(101, 147, 207);
	m_fcsetCaption.clrColor.SetStandardValue(clrText);

	CopySettings(m_fcsetAMPM.clrColor, clrText);
	CopySettings(m_fcsetSmall.clrColor, clrText);
	CopySettings(m_fcsetBigBase.clrColor, clrText);

	CopySettings(m_fcsetBigHour_.clrColor, m_fcsetBigBase.clrColor);

	//-----------------------
	if (IsXPTheme())
	{
		m_grclrNowLineBk.SetStandardValue(m_clrBackground, RGB(247, 202, 95));
		m_clrNowLine.SetStandardValue(RGB(187, 85, 3));
	}
	else
	{
		m_grclrNowLineBk.SetStandardValue(m_clrBackground);
		m_clrNowLine.SetStandardValue(::GetSysColor(COLOR_HIGHLIGHT));
	}
}

void CXTPCalendarThemeOffice2007::CTODayViewDay::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	m_clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_DayBorder));
	//m_clrTodayBorder.SetStandardValue(XTP_CALENDAR_OFFICE2007_TODAYBORDER);

	TBase::RefreshMetrics(bRefreshChildren);

	m_clrTodayBorder.SetStandardValue(
		GetTheme()->GetColorsSetPart()->GetColor(xtpCLR_HeaderBorder, GetHeaderPartX()->m_clrTodayBaseColor));
}

void CXTPCalendarThemeOffice2007::CTODayViewDay::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("DayViewDay")));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("BorderColor"), m_clrBorder);
	PX_Color(&secData, _T("TodayBorderColor"), m_clrTodayBorder);

	TBase::DoPropExchange(&secData);
}

void CXTPCalendarThemeOffice2007::CTODayViewDay::Serialize(CArchive& ar)
{
	m_clrBorder.Serialize(ar);
	m_clrTodayBorder.Serialize(ar);

	TBase::Serialize(ar);
}

void CXTPCalendarThemeOffice2007::CTODayViewDayHeader::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	TBase::RefreshMetrics(bRefreshChildren);

	RefreshFromParent(pThemeX->GetDayViewPartX()->GetHeaderPartX());

	m_clrTodayBaseColor.SetStandardValue(pThemeX->GetHeaderPartX()->m_clrTodayBaseColor);
}

void CXTPCalendarThemeOffice2007::CTODayViewDayGroupHeader::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	TBase::RefreshMetrics(bRefreshChildren);

	RefreshFromParent(pThemeX->GetDayViewPartX()->GetDayPartX()->GetHeaderPartX());

	m_bkNormal.nBitmapID.SetStandardValue(xtpIMG_DayViewGroupHeader);
	m_bkSelected.nBitmapID.SetStandardValue(xtpIMG_DayViewGroupHeader);
	m_bkToday.nBitmapID.SetStandardValue(xtpIMG_DayViewGroupHeader);
	m_bkTodaySelected.nBitmapID.SetStandardValue(xtpIMG_DayViewGroupHeader);
}

void CXTPCalendarThemeOffice2007::CTOHeader::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	TBase::RefreshMetrics(bRefreshChildren);

	m_clrBaseColor.SetStandardValue(pThemeX->m_clrBaseColor);
	m_clrTodayBaseColor.SetStandardValue(XTP_CALENDAR_OFFICE2007_TODAYBORDER);

	XTP_SAFE_CALL1(GetHeightFormulaPart(), SetStandardValue(14, 10, 1));
}

void CXTPCalendarThemeOffice2007::CTODayViewDayGroup::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	m_clrBorderLeft.SetStandardValue(pThemeX->GetColor(xtpCLR_DayBorder));

	TBase::RefreshMetrics(bRefreshChildren);
}

void CXTPCalendarThemeOffice2007::CTODayViewDayGroupCell::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	//TBase::RefreshMetrics(); // has no children

	m_clrsetWorkCell.clrBackground.SetStandardValue(GetTheme()->GetColor(xtpCLR_DayViewCellWorkBk));
	m_clrsetWorkCell.clrBorderBottomInHour.SetStandardValue(GetTheme()->GetColor(xtpCLR_DayViewCellWorkBorderBottomInHour));
	m_clrsetWorkCell.clrBorderBottomHour.SetStandardValue(GetTheme()->GetColor(xtpCLR_DayViewCellWorkBorderBottomHour));
	m_clrsetWorkCell.clrSelected.SetStandardValue(GetTheme()->GetColor(xtpCLR_SelectedBk));

	m_clrsetNonWorkCell.clrBackground.SetStandardValue(GetTheme()->GetColor(xtpCLR_DayViewCellNonWorkBk));
	m_clrsetNonWorkCell.clrBorderBottomInHour.SetStandardValue(GetTheme()->GetColor(xtpCLR_DayViewCellNonWorkBorderBottomInHour));
	m_clrsetNonWorkCell.clrBorderBottomHour.SetStandardValue(GetTheme()->GetColor(xtpCLR_DayViewCellNonWorkBorderBottomHour));
	m_clrsetNonWorkCell.clrSelected.SetStandardValue(GetTheme()->GetColor(xtpCLR_SelectedBk));
}

void CXTPCalendarThemeOffice2007::CTODayViewDayGroupAllDayEvents::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	//TBase::RefreshMetrics(); // has no children

	m_clrBackground.SetStandardValue(pThemeX->GetColor(xtpCLR_DayViewAllDayEventsBk));
	m_clrBackgroundSelected.SetStandardValue(pThemeX->GetColor(xtpCLR_SelectedBk));
	m_clrBottomBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_DayViewAllDayEventsBorderBottom));
}

/////////////////////////////////////////////////////////////////////////////
void CXTPCalendarThemeOffice2007::CTOEvent::CEventFontsColorsSet::CopySettings(
				const CXTPCalendarThemeOffice2007::CTOEvent::CEventFontsColorsSet& rSrc)
{
	CXTPCalendarTheme::CopySettings(clrBorder, rSrc.clrBorder);

	CXTPCalendarTheme::CopySettings(grclrBackground, rSrc.grclrBackground);

	fcsetSubject.CopySettings(rSrc.fcsetSubject);
	fcsetLocation.CopySettings(rSrc.fcsetLocation);
	fcsetBody.CopySettings(rSrc.fcsetBody);

	fcsetStartEnd.CopySettings(rSrc.fcsetStartEnd);
}

void CXTPCalendarThemeOffice2007::CTOEvent::RefreshFromParent(CTOEvent* pParentSrc)
{
	ASSERT(pParentSrc);
	if (!pParentSrc)
	{
		return;
	}

	m_fcsetNormal.CopySettings(pParentSrc->m_fcsetNormal);
	m_fcsetSelected.CopySettings(pParentSrc->m_fcsetSelected);

	CXTPCalendarTheme::CopySettings(m_clrGripperBorder, pParentSrc->m_clrGripperBorder);
	CXTPCalendarTheme::CopySettings(m_clrGripperBackground, pParentSrc->m_clrGripperBackground);

	if (pParentSrc->GetHeightFormulaPart())
		XTP_SAFE_CALL1(GetHeightFormulaPart(), RefreshFromParent(pParentSrc->GetHeightFormulaPart()) );

	if (pParentSrc->GetEventIconsToDrawPart())
		XTP_SAFE_CALL1(GetEventIconsToDrawPart(), RefreshFromParent(pParentSrc->GetEventIconsToDrawPart()) );
}

void CXTPCalendarThemeOffice2007::CTOEvent::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	m_fcsetNormal.clrBorder.SetStandardValue(RGB(0, 0, 0));
	m_fcsetNormal.grclrBackground.SetStandardValue(RGB(255, 255, 255), RGB(255, 255, 255));

	m_fcsetSelected.clrBorder.SetStandardValue(RGB(0, 0, 0));
	m_fcsetSelected.grclrBackground.SetStandardValue(m_fcsetNormal.grclrBackground);

	//----------------------------------------------
	CFont* pFontDef = pThemeX->m_fntBaseFont;
	CFont* pFontBoldDef = pThemeX->m_fntBaseFontBold;
	//----------------------------------------------

	m_fcsetNormal.fcsetSubject.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_fcsetNormal.fcsetSubject.fntFont.SetStandardValue(pFontBoldDef);

	m_fcsetNormal.fcsetLocation.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_fcsetNormal.fcsetLocation.fntFont.SetStandardValue(pFontDef);

	m_fcsetNormal.fcsetBody.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_fcsetNormal.fcsetBody.fntFont.SetStandardValue(pFontDef);

	//----------------------------------------------
	m_fcsetSelected.fcsetSubject.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_fcsetSelected.fcsetSubject.fntFont.SetStandardValue(pFontBoldDef);

	m_fcsetSelected.fcsetLocation.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_fcsetSelected.fcsetLocation.fntFont.SetStandardValue(pFontDef);

	m_fcsetSelected.fcsetBody.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_fcsetSelected.fcsetBody.fntFont.SetStandardValue(pFontDef);

	m_clrGripperBorder.SetStandardValue(m_fcsetSelected.clrBorder);
	m_clrGripperBackground.SetStandardValue(RGB(255, 255, 255));
}

void CXTPCalendarThemeOffice2007::CTOEvent::CEventFontsColorsSet::doPX(
		CXTPPropExchange* pPX, LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme)
{
	if (!pPX || !pcszPropName || !pTheme)
	{
		ASSERT(FALSE);
		return;
	}
	CXTPPropExchangeSection secData(pPX->GetSection(pcszPropName));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("BorderColor"), clrBorder);
	PX_GrColor(&secData, _T("Background"), grclrBackground);
	fcsetSubject.doPX(&secData, _T("Subject"), pTheme);
	fcsetLocation.doPX(&secData, _T("Location"), pTheme);
	fcsetBody.doPX(&secData, _T("Body"), pTheme);
	fcsetStartEnd.doPX(&secData, _T("StartEnd"), pTheme);
}

void CXTPCalendarThemeOffice2007::CTOEvent::CEventFontsColorsSet::
		Serialize(CArchive& ar)
{
	clrBorder.Serialize(ar);
	grclrBackground.Serialize(ar);
	fcsetSubject.Serialize(ar);
	fcsetLocation.Serialize(ar);
	fcsetBody.Serialize(ar);
	fcsetStartEnd.Serialize(ar);
}

void CXTPCalendarThemeOffice2007::CTOEvent::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("Event")));
	if (pPX->IsStoring())
		secData->EmptySection();

	m_fcsetNormal.doPX(&secData, _T("Normal"), GetTheme());
	m_fcsetSelected.doPX(&secData, _T("Selected"), GetTheme());

	PX_Color(&secData, _T("GripperBorderColor"),     m_clrGripperBorder);
	PX_Color(&secData, _T("GripperBackgroundColor"), m_clrGripperBackground);

	TBase::DoPropExchange(&secData);
}

void CXTPCalendarThemeOffice2007::CTOEvent::Serialize(CArchive& ar)
{
	m_fcsetNormal.Serialize(ar);
	m_fcsetSelected.Serialize(ar);

	m_clrGripperBorder.Serialize(ar);
	m_clrGripperBackground.Serialize(ar);

	TBase::Serialize(ar);
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_MultiDay::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("DayViewEvent_MultiDay")));
	if (pPX->IsStoring())
		secData->EmptySection();

	m_strDateFormatFrom.DoPropExchange(&secData, _T("DateFormatFrom"));
	m_strDateFormatTo.DoPropExchange(&secData, _T("DateFormatTo"));

	TBase::DoPropExchange(&secData);
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_MultiDay::Serialize(CArchive& ar)
{
	m_strDateFormatFrom.Serialize(ar);
	m_strDateFormatTo.Serialize(ar);

	TBase::Serialize(ar);
}

int CXTPCalendarThemeOffice2007::CTOEvent::CalcMinEventHeight(CDC* pDC)
{
	CArray<CFont*, CFont*> arFonts;

	arFonts.Add(m_fcsetNormal.fcsetSubject.Font());
	arFonts.Add(m_fcsetNormal.fcsetLocation.Font());
	arFonts.Add(m_fcsetNormal.fcsetStartEnd.Font());

	arFonts.Add(m_fcsetSelected.fcsetSubject.Font());
	arFonts.Add(m_fcsetSelected.fcsetLocation.Font());
	arFonts.Add(m_fcsetSelected.fcsetStartEnd.Font());

	int nFontHeightMax = 0;
	CXTPCalendarUtils::GetMaxHeightFont(arFonts, pDC, &nFontHeightMax);

	ASSERT(GetHeightFormulaPart());
	if (!GetHeightFormulaPart())
	{
		ASSERT(FALSE);
		return TBase::CalcMinEventHeight(pDC);
	}

	int nHeight = GetHeightFormulaPart()->Calculate(nFontHeightMax);
	return nHeight;
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	TBase::RefreshMetrics(bRefreshChildren);

	RefreshFromParent(pThemeX->GetEventPartX());

	XTP_SAFE_CALL1(GetHeightFormulaPart(), SetStandardValue(14, 10, 1));
	XTP_SAFE_CALL1(GetEventIconsToDrawPart(), SetStandardValue(FALSE, TRUE, TRUE, FALSE, TRUE));
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_MultiDay::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	RefreshFromParent(pThemeX->GetDayViewPartX()->GetEventPartX());

	m_fcsetNormal.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventBorder));
	m_fcsetNormal.grclrBackground.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventBkGRfrom),
									   pThemeX->GetColor(xtpCLR_MultiDayEventBkGRto)   );

	m_fcsetSelected.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventSelectedBorder));
	m_fcsetSelected.grclrBackground.SetStandardValue(m_fcsetNormal.grclrBackground);

	//------------------------------------------------
	//LOCALE_IDATE  -  short date format ordering
	// 0 Month-Day-Year
	// 1 Day-Month-Year
	// 2 Year-Month-Day

	int nDateOrdering = CXTPCalendarUtils::GetLocaleLong(LOCALE_IDATE);
	CString strFormatFrom, strFormatTo;
	XTPResourceManager()->LoadString(&strFormatFrom, XTP_IDS_CALENDAR_FROM);
	XTPResourceManager()->LoadString(&strFormatTo, XTP_IDS_CALENDAR_TO);

	if (nDateOrdering == 1)
	{
		strFormatFrom += _T(": MMM d");
		strFormatTo += _T(": MMM d");
	}
	else
	{
		strFormatFrom += _T(": d MMM");
		strFormatTo += _T(": d MMM");
	}

	m_strDateFormatFrom.SetStandardValue(strFormatFrom);
	m_strDateFormatTo.SetStandardValue(strFormatTo);
	//------------------------------------------------

	m_fcsetNormal.fcsetStartEnd.fntFont.SetStandardValue(pThemeX->m_fntBaseFont);
	m_fcsetNormal.fcsetStartEnd.clrColor.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventFromToDates));

	m_fcsetSelected.fcsetStartEnd.fntFont.SetStandardValue(pThemeX->m_fntBaseFont);
	m_fcsetSelected.fcsetStartEnd.clrColor.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventFromToDates));
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_SingleDay::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	RefreshFromParent(pThemeX->GetDayViewPartX()->GetEventPartX());

	m_fcsetNormal.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_DayViewSingleDayEventBorder));
	m_fcsetNormal.grclrBackground.SetStandardValue(pThemeX->GetColor(xtpCLR_DayViewSingleDayEventBkGRfrom),
													pThemeX->GetColor(xtpCLR_DayViewSingleDayEventBkGRto));

	m_fcsetSelected.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_DayViewSingleDayEventSelectedBorder));
	m_fcsetSelected.grclrBackground.SetStandardValue(m_fcsetNormal.grclrBackground);

	InitBusyStatusDefaultColors();
}

BOOL CXTPCalendarThemeOffice2007::CTOEvent::Draw_ArrowL(CXTPCalendarViewEvent* pViewEvent, CDC* pDC,
														CRect& rrcRect)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);

	if (!pThemeX)
	{
		return FALSE;
	}

	CXTPOffice2007Image* pArrowLR = pThemeX->m_pImagesStd->LoadFile(_T("Office2007_Calendar_AllDayEventArrowsLR.bmp"));

	if (!pArrowLR || !pDC || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CSize szArrow =  pArrowLR->GetExtent();
	szArrow.cy /= 2;

	CRect rcEventMax(pViewEvent->GetViewEventRectMax());
	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return FALSE;
	}

	CRect rcArrow = rcEvent;

	rcArrow.top = rcEvent.top + rcEvent.Height() / 2 - szArrow.cy / 2;
	rcArrow.bottom = rcArrow.top + szArrow.cy;
	rcArrow.left = rcEventMax.left + 5;
	rcArrow.right = rcArrow.left + szArrow.cx;

	if (rcEventMax.Width() < szArrow.cx * 4)
	{
		rcArrow.right = rcArrow.left = rcEventMax.left;
		rrcRect = rcArrow;
		return FALSE;
	}

	rrcRect = rcArrow;

	pArrowLR->DrawImage(pDC, rcArrow, pArrowLR->GetSource(0, 2), CRect(0,0,0,0), RGB(255, 0, 255));

	return TRUE;
}

BOOL CXTPCalendarThemeOffice2007::CTOEvent::Draw_ArrowR(CXTPCalendarViewEvent* pViewEvent, CDC* pDC, CRect& rrcRect)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);

	if (!pThemeX)
	{
		return FALSE;
	}

	CXTPOffice2007Image* pArrowLR = pThemeX->m_pImagesStd->LoadFile(_T("Office2007_Calendar_AllDayEventArrowsLR.bmp"));

	if (!pArrowLR || !pDC || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CSize szArrow =  pArrowLR->GetExtent();
	szArrow.cy /= 2;

	CRect rcEventMax(pViewEvent->GetViewEventRectMax());
	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return FALSE;
	}

	CRect rcArrow = rcEvent;

	rcArrow.top = rcEvent.top + rcEvent.Height() / 2 - szArrow.cy / 2;
	rcArrow.bottom = rcArrow.top + szArrow.cy;
	rcArrow.left = rcEventMax.right - 5 - szArrow.cx;
	rcArrow.right = rcArrow.left + szArrow.cx;

	if (rcEventMax.Width() < szArrow.cx * 4)
	{
		rcArrow.right = rcArrow.left = rcEventMax.right;
		rrcRect = rcArrow;
		return FALSE;
	}

	rrcRect = rcArrow;

	pArrowLR->DrawImage(pDC, rcArrow, pArrowLR->GetSource(1, 2), CRect(0,0,0,0), RGB(255, 0, 255));

	return TRUE;
}

CSize CXTPCalendarThemeOffice2007::CTOEvent::Draw_Icons(CXTPCalendarViewEvent* pViewEvent,
								CDC* pDC, const CRect& rcIconsMax, BOOL bCalculate)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());

	if (!pThemeX || !pDC || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return FALSE;
	}
	CXTPCalendarEvent* pEvent = pViewEvent->GetEvent();

	//-----------------------------------------------------
	CArray<XTP_CALENDAR_EVENT_ICON_INFO, XTP_CALENDAR_EVENT_ICON_INFO&> arIcons;

	CXTPCalendarCustomIconIDs* pCustIconsIDs = pEvent->GetCustomIcons();
	ASSERT(pCustIconsIDs);
	ASSERT(GetEventIconsToDrawPart());

	// get standard icons ===================================================

	CUIntArray arGlyphID, arGlyphIndex;

	if (GetEventIconsToDrawPart())
		GetEventIconsToDrawPart()->FillIconIDs(arGlyphID, arGlyphIndex, pEvent);

	CPoint ptNext_rb = rcIconsMax.BottomRight();
	int nIconYMax = 0;

	if (arGlyphID.GetSize())
	{
		CXTPOffice2007Image* pGlyphs = pThemeX->m_pImagesStd->LoadFile(_T("Office2007_Calendar_EventGlyphs.bmp"));
		ASSERT(pGlyphs);

		CSize szGlyph = pGlyphs ? pGlyphs->GetExtent() : CSize(0);
		szGlyph.cy /= 5;

		nIconYMax = max(nIconYMax, szGlyph.cy);

		int nCount = (int)arGlyphID.GetSize();
		for (int i = 0; i < nCount && pGlyphs; i++)
		{
			// skip if customized -------------------------------------
			if (pCustIconsIDs && pCustIconsIDs->Find(arGlyphID[i]) >= 0)
			{
				continue;
			}

			//---------------------------------------------------------
			if (ptNext_rb.x - szGlyph.cx >= rcIconsMax.left &&
				ptNext_rb.y - szGlyph.cy >= rcIconsMax.top)
			{
				XTP_CALENDAR_EVENT_ICON_INFO ii;
				::ZeroMemory(&ii, sizeof(ii));

				ii.m_pImage = pGlyphs;
				ii.m_rcSrc = pGlyphs->GetSource(arGlyphIndex[i], 5);

				ii.m_rcDestination.bottom = ptNext_rb.y;
				ii.m_rcDestination.right = ptNext_rb.x;
				ii.m_rcDestination.left = ptNext_rb.x - szGlyph.cx;
				ii.m_rcDestination.top = ptNext_rb.y - szGlyph.cy;

				ptNext_rb.x -= szGlyph.cx;

				arIcons.Add(ii);
			}
		}
	}

	// get custom icons ===================================================
	CXTPImageManager* pImageMan = pThemeX->GetCustomIconsList();
	ASSERT(pImageMan);

	if (pImageMan && pCustIconsIDs && pCustIconsIDs->GetSize())
	{
		int nCount = pCustIconsIDs->GetSize();
		for (int i = 0; i < nCount; i++)
		{
			UINT uIconID = pCustIconsIDs->GetAt(nCount - i - 1);

			CXTPImageManagerIcon* pIcon = pImageMan->GetImage(uIconID, 0);
			if (pIcon)
			{
				CSize szIcon = pIcon->GetExtent();

				//------------------------------------------------------
				if (ptNext_rb.x - szIcon.cx < rcIconsMax.left)
					break;

				if (ptNext_rb.y - szIcon.cy < rcIconsMax.top)
					continue;
				//------------------------------------------------------

				XTP_CALENDAR_EVENT_ICON_INFO ii;
				::ZeroMemory(&ii, sizeof(ii));

				ii.m_pIcon = pIcon;

				ii.m_rcDestination.bottom = ptNext_rb.y;
				ii.m_rcDestination.right = ptNext_rb.x;
				ii.m_rcDestination.left = ptNext_rb.x - szIcon.cx;
				ii.m_rcDestination.top = ptNext_rb.y - szIcon.cy;

				ptNext_rb.x -= szIcon.cx;
				nIconYMax = max(nIconYMax, szIcon.cy);

				arIcons.Add(ii);
			}
		}
	}

	//
	CSize szIcons(rcIconsMax.right - ptNext_rb.x, nIconYMax);

	if (bCalculate)
	{
		return szIcons;
	}

	//------------------------
	CRect rcIcons = rcIconsMax;

	rcIcons.left = max(rcIcons.right - szIcons.cx - 2, rcIconsMax.left);
	rcIcons.top = max(rcIcons.bottom - szIcons.cy - 2, rcIconsMax.top);

	CTOEvent* pEventPart = pThemeX->GetThemePartForEvent(pViewEvent);
	ASSERT(pEventPart);
	if (pEventPart && !rcIcons.IsRectEmpty())
	{
		pEventPart->FillEventBackgroundEx(pDC, pViewEvent, rcIcons);
	}

	// Draw icons
	int nCount = (int)arIcons.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		XTP_CALENDAR_EVENT_ICON_INFO ii = arIcons[i];
		CRect rcIcon = ii.m_rcDestination;

		if (pDC->IsPrinting())
		{
			CRect rcIcon00(0, 0, rcIcon.Width(), rcIcon.Height());

			CBitmap bmp;
			bmp.CreateCompatibleBitmap(pDC, rcIcon.Width(), rcIcon.Height());

			CXTPCompatibleDC dcMem(NULL, &bmp);

			dcMem.m_bPrinting = TRUE;
			pEventPart->FillEventBackgroundEx(&dcMem, pViewEvent, rcIcon00);
			dcMem.m_bPrinting = FALSE;

			//------------------------------
			if (ii.m_pImage)
			{
				ii.m_pImage->DrawImage(&dcMem, rcIcon00, ii.m_rcSrc, CRect(0,0,0,0), RGB(255, 0, 255));
			}
			else if (ii.m_pIcon)
			{
				ii.m_pIcon->Draw(&dcMem, CPoint(0, 0));
			}
			else ASSERT(FALSE);

			pDC->BitBlt(rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), &dcMem, 0, 0, SRCCOPY);
		}
		else
		{
			if (ii.m_pImage)
			{
				ii.m_pImage->DrawImage(pDC, ii.m_rcDestination, ii.m_rcSrc, CRect(0,0,0,0), RGB(255, 0, 255));
			}
			else if (ii.m_pIcon)
			{
				ii.m_pIcon->Draw(pDC, rcIcon.TopLeft());
			}
			else ASSERT(FALSE);
		}
	}

	return szIcons;
}

BOOL CXTPCalendarThemeOffice2007::CTODayViewEvent_MultiDay::Draw_ArrowLtext(CXTPCalendarViewEvent* pViewEvent,
				CDC* pDC, CThemeFontColorSetValue* pfcsetText, CRect& rrcRect, int nLeft_x)
{
	if (!pDC || !pViewEvent || !pViewEvent->GetEvent() || !pfcsetText)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPFontDC autoFont(pDC, pfcsetText->Font(), pfcsetText->Color());

	CString strText = Format_FromToDate(pViewEvent, 1);

	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return FALSE;
	}

	CSize szText = pDC->GetTextExtent(strText);

	CRect rcText = rcEvent;
	rcText.left = nLeft_x + 5;
	rcText.right = rcText.left + szText.cx;

	if (rcEvent.Width() / 2 < szText.cx * 3)
	{
		rcText.left = rcText.right = nLeft_x;
		rrcRect = rcText;
		return FALSE;
	}

	rrcRect = rcText;

	pDC->DrawText(strText, &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	return TRUE;
}

BOOL CXTPCalendarThemeOffice2007::CTODayViewEvent_MultiDay::Draw_ArrowRtext(CXTPCalendarViewEvent* pViewEvent,
				CDC* pDC, CThemeFontColorSetValue* pfcsetText, CRect& rrcRect, int nRight_x)
{
	if (!pDC || !pViewEvent || !pViewEvent->GetEvent() || !pfcsetText)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPFontDC autoFont(pDC, pfcsetText->Font(), pfcsetText->Color());

	CString strText = Format_FromToDate(pViewEvent, 2);

	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return FALSE;
	}

	CSize szText = pDC->GetTextExtent(strText);

	CRect rcText = rcEvent;
	rcText.right = nRight_x - 5;
	rcText.left = rcText.right - szText.cx;

	if (rcEvent.Width() / 2 < szText.cx * 3)
	{
		rcText.left = rcText.right = nRight_x;
		rrcRect = rcText;
		return FALSE;
	}

	rrcRect = rcText;

	pDC->DrawText(strText, &rcText, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	return TRUE;
}

CString CXTPCalendarThemeOffice2007::CTODayViewEvent_MultiDay::Format_FromToDate(CXTPCalendarViewEvent* pViewEvent, int nStart1End2)
{
	if (!pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return _T("");
	}

	ASSERT(nStart1End2 == 1 || nStart1End2 == 2);

	COleDateTime dtDate = nStart1End2 == 1 ? pViewEvent->GetEvent()->GetStartTime() : pViewEvent->GetEvent()->GetEndTime();

	if (nStart1End2 == 2 && CXTPCalendarUtils::IsZeroTime(dtDate))
	{
		dtDate -= COleDateTimeSpan(0, 0, 1, 0);
	}

	const CString& strFormat = nStart1End2 == 1 ? m_strDateFormatFrom : m_strDateFormatTo;

	CString strDate = CXTPCalendarUtils::GetDateFormat(dtDate, strFormat);

	return strDate;
}


///*** ======================================================================
CRect CXTPCalendarThemeOffice2007::CTODayViewDay::ExcludeDayBorder(
				CXTPCalendarDayViewDay* pDayViewDay, const CRect& rcDay)
{
	BOOL bIsToday = GetHeaderPart()->IsToday(pDayViewDay);
	int nWidthH = bIsToday ? 2 : 1;

	CRect rcGroups = rcDay;
	rcGroups.DeflateRect(nWidthH, 0, nWidthH, 1);
	return rcGroups;
}

void CXTPCalendarThemeOffice2007::CTODayViewDay::Draw_DayBorder(CXTPCalendarDayViewDay* pDayViewDay, CDC* pDC)
{
	CRect rcDay = pDayViewDay->GetDayRect();
	CRect rcGroup = pDayViewDay->GetViewGroup(0)->GetRect();

	CRect rcDayDetails = rcDay;
	rcDayDetails.top = rcGroup.top;

	CRect rcDayDetails2 = ExcludeDayBorder(pDayViewDay, rcDayDetails);

	BOOL bIsToday = GetHeaderPart()->IsToday(pDayViewDay);
	COLORREF clrBorder = bIsToday ? m_clrTodayBorder : m_clrBorder;

	CRect rcLeft = rcDayDetails, rcRight = rcDayDetails, rcBottom = rcDayDetails;
	rcLeft.right = min(rcDayDetails2.left, rcDayDetails.right);
	rcRight.left = max(rcDayDetails2.right, rcDayDetails.left);
	rcBottom.top = max(rcDayDetails2.bottom, rcDayDetails.top);

	pDC->FillSolidRect(&rcLeft, clrBorder);
	pDC->FillSolidRect(&rcRight, clrBorder);
	pDC->FillSolidRect(&rcBottom, clrBorder);
}

void CXTPCalendarThemeOffice2007::CTODayViewDayHeader::AdjustLayout(CCmdTarget* pObject,
													CDC* pDC, const CRect& rcRect)
{
	//TBase::AdjustLayout(pObject, pDC, rcRect);

	CXTPCalendarDayViewDay* pDayViewDay = DYNAMIC_DOWNCAST(CXTPCalendarDayViewDay, pObject);
	if (!pDC || !pDayViewDay)
	{
		ASSERT(FALSE);
		return;
	}
	if (pDayViewDay->GetDayIndex() != 0)
	{
		return;
	}

//  int nWfactor = rcRect.Width() / rcRect.Height();
//
//  if (nWfactor <= 5)
//  {
//      m_nWeekDayFormat = 0;
//  }
//  else if (nWfactor <= 10)
//  {
//      m_nWeekDayFormat = 1;
//  }
//  else
//  {
//      m_nWeekDayFormat = 2;
//  }

	int nWDLong_max = 0;
	int nWDShort_max = 0;

	CXTPFontDC autoFnt(pDC, m_TextCenter.fcsetNormal.Font());

	for (int i = 0; i < 7; i++)
	{
		CString strWDLong = CXTPCalendarUtils::GetLocaleString(LOCALE_SDAYNAME1 + i, 100);
		CString strWDShort = CXTPCalendarUtils::GetLocaleString(LOCALE_SABBREVDAYNAME1 + i, 100);

		nWDLong_max = max(nWDLong_max, pDC->GetTextExtent(strWDLong).cx);
		nWDShort_max = max(nWDShort_max, pDC->GetTextExtent(strWDShort).cx);
	}

	autoFnt.SetFont(m_TextLeftRight.fcsetNormal.Font());

	int nLR_width = pDC->GetTextExtent(_T("88"), 2).cx * 2;

	int nCenterTextWidth = max(rcRect.Width() - nLR_width, 0);

	if (nCenterTextWidth >= nWDLong_max + 4)
	{
		m_nWeekDayFormat = 2;
	}
	else if (nCenterTextWidth >= nWDShort_max + 4)
	{
		m_nWeekDayFormat = 1;
	}
	else
	{
		m_nWeekDayFormat = 0;
	}
}

void CXTPCalendarThemeOffice2007::CTODayViewDayHeader::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarDayViewDay* pDayViewDay = DYNAMIC_DOWNCAST(CXTPCalendarDayViewDay, pObject);
	if (!pDC || !pDayViewDay || !pDayViewDay->GetView() || !GetTheme() || !pThemeX)
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& rLayout = GetTheme()->GetViewDayLayout(pDayViewDay);
	COleDateTime dtDay = pDayViewDay->GetDayDate();

	CPoint pt(0, 0);
	if (GetCursorPos(&pt))
		pThemeX->GetCalendarControl()->ScreenToClient(&pt);

	BOOL bIsToday    = IsToday(pDayViewDay);
	BOOL bIsSelected = IsSelected(pDayViewDay);

	BOOL bFirstDay = pDayViewDay->GetDayIndex() == 0;
	BOOL bLastDay = pDayViewDay->GetDayIndex() == pDayViewDay->GetView()->GetViewDayCount() - 1;

	int nFlags = bIsToday ? CTOHeader::xtpCalendarItemState_Today : CTOHeader::xtpCalendarItemState_Normal;
	nFlags |= bIsSelected ? CTOHeader::xtpCalendarItemState_Selected : 0;
	nFlags |= bFirstDay ? CTOHeader::xtpCalendarItemFirst : 0;
	nFlags |= bLastDay ? CTOHeader::xtpCalendarItemLast : 0;

	if (rLayout.m_rcDayHeader.PtInRect(pt))
	{
		nFlags |=  CTOHeader::xtpCalendarItemState_Hot;
		rLayout.m_nHotState |= CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotDayHeader;
	}
	else
	{
		rLayout.m_nHotState &= ~CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotDayHeader;
	}

	CRect rcRect = rLayout.m_rcDayHeader;

	//------------------------------------------------------
	CString strTextLeft;
	CString strTextCenter;
	CString strTextRight;

	strTextLeft.Format(_T("%d"), dtDay.GetDay());

	if (m_nWeekDayFormat > 0)
	{
		LCTYPE lctWDName1 = m_nWeekDayFormat > 1 ? LOCALE_SDAYNAME1 : LOCALE_SABBREVDAYNAME1;

		int nWD = dtDay.GetDayOfWeek();
		strTextCenter = CXTPCalendarUtils::GetLocaleString(lctWDName1 + ((nWD-2+7)%7), 100);
	}

	//------------------------------------------------------
	pThemeX->GetItemTextIfNeed(xtpCalendarItemText_DayViewDayHeaderLeft, &strTextLeft, pDayViewDay);
	pThemeX->GetItemTextIfNeed(xtpCalendarItemText_DayViewDayHeaderCenter, &strTextCenter, pDayViewDay);
	pThemeX->GetItemTextIfNeed(xtpCalendarItemText_DayViewDayHeaderRight, &strTextRight, pDayViewDay);
	//------------------------------------------------------

	Draw_Header(pDC, rcRect, nFlags, strTextLeft, strTextCenter, strTextRight);
}

void CXTPCalendarThemeOffice2007::CTODayViewDayGroupHeader::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarDayViewGroup* pDayViewGroup = DYNAMIC_DOWNCAST(CXTPCalendarDayViewGroup, pObject);

	if (!pThemeX || !pDayViewGroup || !pDayViewGroup->GetViewDay())
	{
		ASSERT(FALSE);
		return;
	}

	const CXTPCalendarViewGroup::XTP_VIEW_GROUP_LAYOUT layoutGoup = CXTPCalendarTheme::GetViewGroupLayout(pDayViewGroup);
	CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& groupLayoutX = CXTPCalendarTheme::GetDayViewGroupLayout(pDayViewGroup);
	CRect rcRect = layoutGoup.m_rcGroupHeader;

	CPoint pt(0, 0);
	if (GetCursorPos(&pt))
		pThemeX->GetCalendarControl()->ScreenToClient(&pt);

	BOOL bIsToday    = IsToday(pDayViewGroup->GetViewDay());
	BOOL bIsSelected = IsSelected(pDayViewGroup);
	BOOL bHot = rcRect.PtInRect(pt);

	BOOL bFirstDay = pDayViewGroup->GetGroupIndex() == 0;
	BOOL bLastDay = pDayViewGroup->GetGroupIndex() == pDayViewGroup->GetViewDay()->GetViewGroupsCount() - 1;

	int nFlags = bIsToday ? CTOHeader::xtpCalendarItemState_Today : CTOHeader::xtpCalendarItemState_Normal;
	nFlags |= bIsSelected ? CTOHeader::xtpCalendarItemState_Selected : 0;
	nFlags |= bFirstDay ? CTOHeader::xtpCalendarItemFirst : 0;
	nFlags |= bLastDay ? CTOHeader::xtpCalendarItemLast : 0;

	if (bHot)
	{
		nFlags |=  CTOHeader::xtpCalendarItemState_Hot;
		groupLayoutX.m_nHotState |= CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotHeader;
	}
	else
	{
		groupLayoutX.m_nHotState &= ~CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotHeader;
	}

	CString strCaption = pDayViewGroup->GetCaption();

	Draw_Header(pDC, rcRect, nFlags, _T(""), strCaption);

}

void CXTPCalendarThemeOffice2007::CTODayViewDay::OnMouseMove(CCmdTarget* pObject, UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarViewDay* pViewDay = DYNAMIC_DOWNCAST(CXTPCalendarViewDay, pObject);
	if (!pThemeX || !pViewDay || !pThemeX->GetCalendarControl())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pViewDay);

	BOOL bHotPrev = !!(dayLayout.m_nHotState & CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotDayHeader);
	if (dayLayout.m_rcDayHeader.PtInRect(point) != bHotPrev)
	{
		CXTPCalendarControl::CUpdateContext updateContext(pThemeX->GetCalendarControl(), xtpCalendarUpdateRedraw);
	}
}

void CXTPCalendarThemeOffice2007::CTODayViewDayGroup::AdjustDayEvents(
								CXTPCalendarDayViewGroup* pDayViewGroup, CDC* pDC)
{
	if (!pDayViewGroup || !pDC)
	{
		ASSERT(FALSE);
		return;
	}

	ASSERT(pDC);

	CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder placeBuilder;
	placeBuilder.BiuldEventPaces(pDayViewGroup);

	CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& rLayoutX = GetTheme()->GetDayViewGroupLayout(pDayViewGroup);

	int nCount = pDayViewGroup->GetViewEventsCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarDayViewEvent* pViewEvent = pDayViewGroup->GetViewEvent(i);
		ASSERT(pViewEvent);
		if (!pViewEvent || pViewEvent->IsMultidayEvent())
			continue;

		int nPlaceNumber = pViewEvent->GetEventPlaceNumber();
		pViewEvent->AdjustLayout2(pDC, rLayoutX.m_rcDayDetails, nPlaceNumber);
	}
}

CRect CXTPCalendarThemeOffice2007::CTODayViewDayGroup::GetScrollButtonRect(
		const CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& groupLayout, int nButton)
{
	CRect rc;

	switch (nButton)
	{
	case CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollUp:
		rc = groupLayout.m_rcDayDetails;
		rc.right -= 5;
		rc.top += 5;
		break;
	case CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollDown:
		rc = groupLayout.m_rcDayDetails;
		rc.right -= 5;
		rc.bottom -= 5;
		break;
	default:
		ASSERT(FALSE);
		rc.SetRect(0, 0, 0, 0);
	}

	return rc;
}

void CXTPCalendarThemeOffice2007::CTODayViewDayGroup::Draw(CCmdTarget* pObject, CDC* pDC)
{
	TBase::Draw(pObject, pDC);

	CXTPCalendarDayViewGroup* pDVGroup = DYNAMIC_DOWNCAST(CXTPCalendarDayViewGroup, pObject);
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	if (!pDC || !pDVGroup || !pThemeX)
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& groupLayout = GetTheme()->GetDayViewGroupLayout(pDVGroup);

	if (pDC->IsPrinting())
		return;

	groupLayout.m_nHotState &= ~(CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollUp | CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollDown);

	int nHit = HitTestScrollButton(pDVGroup);

	if (pDVGroup->IsExpandUp())
	{
		int nButton = CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollUp;
		CRect rc = GetScrollButtonRect(groupLayout, nButton);

		BOOL bHot = !!(nHit & xtpCalendarHitTestDayViewScrollUp);

		pThemeX->DrawExpandDayButton(pDC, rc, TRUE, bHot);

		if (bHot)
			groupLayout.m_nHotState |= nButton;
	}

	if (pDVGroup->IsExpandDown())
	{
		int nButton = CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollDown;
		CRect rc = GetScrollButtonRect(groupLayout, nButton);

		BOOL bHot = !!(nHit & xtpCalendarHitTestDayViewScrollDown);

		pThemeX->DrawExpandDayButton(pDC, rc, FALSE, bHot);

		if (bHot)
			groupLayout.m_nHotState |= nButton;
	}
}

void CXTPCalendarThemeOffice2007::CTODayViewDayGroup::OnMouseMove(CCmdTarget* pObject, UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarDayViewGroup* pDVGroup = DYNAMIC_DOWNCAST(CXTPCalendarDayViewGroup, pObject);
	if (!pDVGroup || !pThemeX || !pThemeX->GetCalendarControl())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarViewGroup::XTP_VIEW_GROUP_LAYOUT groupLayout = GetTheme()->GetViewGroupLayout(pDVGroup);
	CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& groupLayoutX = GetTheme()->GetDayViewGroupLayout(pDVGroup);
	BOOL bRedraw = FALSE;

	int nHit = HitTestScrollButton(pDVGroup, &point);

	BOOL bHot = !!(nHit & xtpCalendarHitTestDayViewScrollUp);
	BOOL bHotPrev = !!(groupLayoutX.m_nHotState & CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollUp);
	bRedraw |= (bHot != bHotPrev);

	bHot = !!(nHit & xtpCalendarHitTestDayViewScrollDown);
	bHotPrev = !!(groupLayoutX.m_nHotState & CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollDown);
	bRedraw |= (bHot != bHotPrev);

	//-----------------------------------------------------------------------
	bHot = groupLayoutX.m_bShowHeader && groupLayout.m_rcGroupHeader.PtInRect(point);
	bHotPrev = !!(groupLayoutX.m_nHotState & CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotHeader);
	bRedraw |= (bHot != bHotPrev);

	if (bRedraw)
	{
		CXTPCalendarControl::CUpdateContext updateContext(pThemeX->GetCalendarControl(), xtpCalendarUpdateRedraw);
	}
}

BOOL CXTPCalendarThemeOffice2007::CTODayViewDayGroup::OnLButtonDown(CCmdTarget* pObject, UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarDayViewGroup* pDVGroup = DYNAMIC_DOWNCAST(CXTPCalendarDayViewGroup, pObject);
	if (!pDVGroup || !pDVGroup->GetViewDay() || !pThemeX || !pThemeX->GetCalendarControl())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nHit = HitTestScrollButton(pDVGroup, &point);

	if (nHit & xtpCalendarHitTestDayViewScrollUp)
	{
		pThemeX->GetCalendarControl()->PostMessage(xtp_wm_UserAction,
			(WPARAM)(xtpCalendarUserAction_OnScrollDay | xtpCalendarScrollDayButton_DayViewUp),
			MAKELPARAM((WORD)pDVGroup->GetGroupIndex(), pDVGroup->GetViewDay()->GetDayIndex()) );
		return TRUE;
	}

	if (nHit & xtpCalendarHitTestDayViewScrollDown)
	{
		pThemeX->GetCalendarControl()->PostMessage(xtp_wm_UserAction,
			(WPARAM)(xtpCalendarUserAction_OnScrollDay | xtpCalendarScrollDayButton_DayViewDown),
			MAKELPARAM((WORD)pDVGroup->GetGroupIndex(), pDVGroup->GetViewDay()->GetDayIndex()) );
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//class CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder
CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder()
{
	m_mapEvToOverlapped.InitHashTable(XTP_CALENDAR_OFFICE2007_DVG_DEV_AVE_COUNT, FALSE);
	m_spCellDuration.SetDateTimeSpan(0, 0, 30, 0);
	m_dtMinStartTime = (DATE)0;
}

void CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::BiuldEventPaces(CXTPCalendarDayViewGroup* pViewGroup)
{
	ASSERT(pViewGroup);
	if (!pViewGroup)
		return;

	//----------------------------------------------------
	m_spCellDuration = XTP_SAFE_GET3(pViewGroup, GetViewDay(), GetView(), GetCellDuration(), 0);
	if (double(m_spCellDuration) <= XTP_HALF_SECOND)
	{
		m_spCellDuration.SetDateTimeSpan(0, 0, 30, 0);
	}

	m_dtMinStartTime = XTP_SAFE_GET2(pViewGroup, GetViewDay(), GetDayDate(), (DATE)0) +
					   COleDateTimeSpan(1, 0, 0, 1) - m_spCellDuration;
	//----------------------------------------------------

	_Clear();

	int nCount = pViewGroup->GetViewEventsCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarDayViewEvent* pViewEvent = pViewGroup->GetViewEvent(i);
		ASSERT(pViewEvent);
		if (!pViewEvent || pViewEvent->IsMultidayEvent())
			continue;

		// Reset Events Paces
		pViewEvent->SetEventPlacePos(XTP_EVENT_PLACE_POS_UNDEFINED);
		pViewEvent->SetEventPlaceCount(1);

		// Update X Data for each event
		_UpdateDataFor(pViewEvent, pViewGroup);
	}

	// Update Events Paces using X Data
	_UpdateEventsPaces(pViewGroup);

	// Delete intermediate X data
	_Clear();
}

void CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::_Clear()
{
	m_mapEvToOverlapped.RemoveAll();
}

void CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::_UpdateDataFor(CXTPCalendarDayViewEvent* pViewEvent,
													CXTPCalendarDayViewGroup* pViewGroup)
{
	ASSERT(pViewEvent);
	ASSERT(pViewGroup);

	COleDateTime dtStart = pViewEvent->GetEvent()->GetStartTime();
	COleDateTime dtEnd = pViewEvent->GetEvent()->GetEndTime();

	// adjust end time if need
	if (pViewEvent->GetEvent()->GetDuration() < m_spCellDuration)
	{
		dtEnd = dtStart + m_spCellDuration;
	}

	// adjust start time if need
	if (dtStart > m_dtMinStartTime)
	{
		dtStart = m_dtMinStartTime;
	}

	//=====================================================
	int nCount = pViewGroup->GetViewEventsCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarDayViewEvent* pVEv_i = pViewGroup->GetViewEvent(i);
		ASSERT(pVEv_i);
		if (!pVEv_i || pVEv_i->IsMultidayEvent() || !pVEv_i->GetEvent())
			continue;

		COleDateTime dtStart_i = pVEv_i->GetEvent()->GetStartTime();
		COleDateTime dtEnd_i = pVEv_i->GetEvent()->GetEndTime();

		// adjust end time if need
		if (pVEv_i->GetEvent()->GetDuration() < m_spCellDuration)
		{
			dtEnd_i = dtStart_i + m_spCellDuration;
		}

		// adjust start time if need
		if (dtStart_i > m_dtMinStartTime)
		{
			dtStart_i = m_dtMinStartTime;
		}

		// is Overlapped
		if (!(dtEnd_i < dtStart || dtStart_i > dtEnd ||
			  CXTPCalendarUtils::IsEqual(dtEnd_i, dtStart) ||
			  CXTPCalendarUtils::IsEqual(dtStart_i, dtEnd) )
		   )
		{
			_XAddOverlapped(pViewEvent, pVEv_i);
		}
	}
}

void CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::_UpdateEventsPaces(CXTPCalendarDayViewGroup* pViewGroup)
{
	UNREFERENCED_PARAMETER(pViewGroup);

	// assign event places
	int i;
	int nCount = pViewGroup->GetViewEventsCount();

	for (i = 0; i < nCount; i++)
	{
		CXTPCalendarDayViewEvent* pViewEvent = pViewGroup->GetViewEvent(i);
		ASSERT(pViewEvent);
		if (!pViewEvent || pViewEvent->IsMultidayEvent() || !pViewEvent->GetEvent())
			continue;

		ASSERT(pViewEvent->GetEventPlacePos() == XTP_EVENT_PLACE_POS_UNDEFINED);

		if (pViewEvent->GetEventPlacePos() == XTP_EVENT_PLACE_POS_UNDEFINED)
		{
			CViewEventsMap* pMapOvr = _XGetDataFor(pViewEvent);
			ASSERT(pMapOvr);
			if (pMapOvr)
			{
				int nPlace = pMapOvr->GetMinFreeEventPlace();
				ASSERT(nPlace >= 0 && nPlace != XTP_EVENT_PLACE_POS_UNDEFINED);
				pViewEvent->SetEventPlacePos(nPlace);
			}
		}
	}

	// update event places count
	for (i = 0; i < nCount; i++)
	{
		CXTPCalendarDayViewEvent* pViewEvent = pViewGroup->GetViewEvent(i);
		ASSERT(pViewEvent);
		if (!pViewEvent || pViewEvent->IsMultidayEvent() || !pViewEvent->GetEvent())
			continue;

		ASSERT(pViewEvent->GetEventPlacePos() != XTP_EVENT_PLACE_POS_UNDEFINED);

		if (pViewEvent->GetEventPlacePos() != XTP_EVENT_PLACE_POS_UNDEFINED)
		{
			CViewEventsMap* pMapOvr = _XGetDataFor(pViewEvent);
			ASSERT(pMapOvr);
			if (pMapOvr)
			{
				int nPlaceMax = pMapOvr->GetMaxEventPlace();
				ASSERT(nPlaceMax >= 0);
				pViewEvent->SetEventPlaceCount(nPlaceMax + 1);
			}
		}
	}

	// update event places count to maximum from overlapped set
	// TODO: update to recurrence
	for (int k = 0; k < min(nCount, 3); k++)
	{
		for (i = 0; i < nCount; i++)
		{
			CXTPCalendarDayViewEvent* pViewEvent = pViewGroup->GetViewEvent(i);
			ASSERT(pViewEvent);
			if (!pViewEvent || pViewEvent->IsMultidayEvent() || !pViewEvent->GetEvent())
				continue;

			ASSERT(pViewEvent->GetEventPlacePos() != XTP_EVENT_PLACE_POS_UNDEFINED);

			if (pViewEvent->GetEventPlacePos() != XTP_EVENT_PLACE_POS_UNDEFINED)
			{
				CViewEventsMap* pMapOvr = _XGetDataFor(pViewEvent);
				ASSERT(pMapOvr);
				if (pMapOvr)
				{
					int nPlaceCountMax = pMapOvr->GetMaxEventPlaceCountEx();
					ASSERT(nPlaceCountMax >= pViewEvent->GetEventPlaceCount());
					pViewEvent->SetEventPlaceCount(nPlaceCountMax);
				}
			}
		}
	}
}

CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::CViewEventsMap* CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::
			_XGetDataFor(CXTPCalendarDayViewEvent* pViewEvent)
{
	// CViewEventsMap* pVEvMap = NULL;
	// to avoid warning C4786 CObject* used.
	CObject* pVEvMap = NULL;

	if (m_mapEvToOverlapped.Lookup(pViewEvent, pVEvMap) && pVEvMap)
	{
		return (CViewEventsMap*)pVEvMap;
	}

	// Create a new sub-data map
	pVEvMap = new CViewEventsMap();

	if (pVEvMap)
	{
		m_mapEvToOverlapped[pViewEvent] = pVEvMap;
	}

	return (CViewEventsMap*)pVEvMap;
}

void CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::_XAddOverlapped(
					CXTPCalendarDayViewEvent* pVEv_base,
					CXTPCalendarDayViewEvent* pVEv_overlapped)
{
	CViewEventsMap* pVEvMap = _XGetDataFor(pVEv_base);
	if (!pVEvMap) // no memory to create
		return;

	pVEvMap->SetAt(pVEv_overlapped, 0);
}

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::CViewEventsMap::CViewEventsMap()
{
	InitHashTable(XTP_CALENDAR_OFFICE2007_DVG_DEV_AVE_COUNT, FALSE);
}

int CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::CViewEventsMap::GetMinFreeEventPlace()
{
	int nMinFreeEventPlace = 0;

	BOOL bBysy = FALSE;
	do
	{
		bBysy = FALSE;

		POSITION pos = GetStartPosition();
		while (pos)
		{
			CXTPCalendarDayViewEvent* pVEv = NULL;
			int nTmp = 0;
			GetNextAssoc(pos, pVEv, nTmp);

			if (pVEv && pVEv->GetEventPlacePos() != XTP_EVENT_PLACE_POS_UNDEFINED &&
				pVEv->GetEventPlacePos() == nMinFreeEventPlace)
			{
				nMinFreeEventPlace++;
				bBysy = TRUE;
				break;
			}
		}
	}
	while (bBysy);

	return nMinFreeEventPlace;
}

int CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::CViewEventsMap::GetMaxEventPlace()
{
	int nMaxEventPlace = 0;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		CXTPCalendarDayViewEvent* pVEv = NULL;
		int nTmp = 0;
		GetNextAssoc(pos, pVEv, nTmp);

		ASSERT(pVEv && pVEv->GetEventPlacePos() != XTP_EVENT_PLACE_POS_UNDEFINED);

		if (pVEv && pVEv->GetEventPlacePos() != XTP_EVENT_PLACE_POS_UNDEFINED &&
			pVEv->GetEventPlacePos() > nMaxEventPlace)
		{
			nMaxEventPlace = pVEv->GetEventPlacePos();
		}
	}

	return nMaxEventPlace;
}

int CXTPCalendarTheme_DayViewSingleDayEventPlaceBuilder::CViewEventsMap::GetMaxEventPlaceCountEx()
{
	int nMaxEventPlaceCount = 1;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		CXTPCalendarDayViewEvent* pVEv = NULL;
		int nTmp = 0;
		GetNextAssoc(pos, pVEv, nTmp);

		ASSERT(pVEv && pVEv->GetEventPlacePos() != XTP_EVENT_PLACE_POS_UNDEFINED);

		if (pVEv && pVEv->GetEventPlaceCount() > nMaxEventPlaceCount)
		{
			nMaxEventPlaceCount = pVEv->GetEventPlaceCount();
		}
	}

	return nMaxEventPlaceCount;
}

//////////////////////////////////////////////////////////////////////////
BOOL CXTPCalendarThemeOffice2007::CTODayViewDayGroup::IsSelected(CXTPCalendarViewGroup* pViewGroup)
{
	if (!pViewGroup || !pViewGroup->GetViewDay_() || !pViewGroup->GetViewDay_()->GetView_())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	COleDateTime dtBegin;
	COleDateTime dtEnd;

	if (pViewGroup->GetViewDay_()->GetView_()->GetSelection(&dtBegin, &dtEnd))
	{
		COleDateTime dtDay = pViewGroup->GetViewDay_()->GetDayDate();
		ASSERT(CXTPCalendarUtils::IsZeroTime(dtDay));
		dtDay = CXTPCalendarUtils::ResetTime(dtDay);

		COleDateTimeSpan spSel = dtBegin - dtEnd;
		int nSelDays = abs(GETTOTAL_DAYS_DTS(spSel));

		dtBegin = CXTPCalendarUtils::ResetTime(dtBegin);
		int nCorrector = CXTPCalendarUtils::IsZeroTime(dtEnd) ? 0 : 1;
		dtEnd = CXTPCalendarUtils::ResetTime(dtEnd) + COleDateTimeSpan(nCorrector, 0, 0, 0);

		return nSelDays > 0 && dtBegin <= dtDay && dtDay < dtEnd;
	}
	return FALSE;
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_MultiDay::CalcEventYs(CRect& rrcRect, int nEventPlaceNumber)
{
	CXTPCalendarDayView* pView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, XTP_SAFE_GET2(GetTheme(), GetCalendarControl(), GetDayView(), NULL));
	if (!pView)
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarDayView::XTP_DAY_VIEW_LAYOUT& rLayoutX = GetTheme()->GetDayViewLayout(pView);
	int nAllDayEventHeight = rLayoutX.m_nAllDayEventHeight;

	rrcRect.top = rLayoutX.m_rcAllDayEvents.top + nAllDayEventHeight * nEventPlaceNumber;
	rrcRect.bottom = rrcRect.top + nAllDayEventHeight;
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_MultiDay::AdjustLayout(CCmdTarget* pObject, CDC* pDC,
														 const CRect& rcRect, int nEventPlaceNumber)
{
	UNREFERENCED_PARAMETER(rcRect);

	CXTPCalendarDayViewEvent* pDayViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarDayViewEvent, pObject);
	CXTPCalendarDayView* pView = XTP_SAFE_GET3(pDayViewEvent, GetViewGroup(), GetViewDay(), GetView(), NULL);
	CXTPCalendarEvent* pEvent = XTP_SAFE_GET1(pDayViewEvent, GetEvent(), NULL);
	if (!pDC || !pDayViewEvent || !pView || !pEvent)
	{
		ASSERT(FALSE);
		return;
	}

	if (!pDayViewEvent->IsMultidayEvent())
	{
		ASSERT(FALSE);
		return;
	}

	//**********************
	int nMDEFlags = pDayViewEvent->GetMultiDayEventFlags();

	CalcEventYs(pDayViewEvent->m_rcEventMax, nEventPlaceNumber);

	pDayViewEvent->m_rcEvent.CopyRect(&pDayViewEvent->m_rcEventMax);
	pDayViewEvent->m_rcText.CopyRect(&pDayViewEvent->m_rcEventMax);

	if ((nMDEFlags & xtpCalendarMultiDayFirst) != 0)
	{
		pDayViewEvent->m_rcEvent.left += 8;
	}

	if ((nMDEFlags & xtpCalendarMultiDayLast) != 0)
	{
		pDayViewEvent->m_rcEvent.right -= 8;
	}

	pDayViewEvent->m_rcText.CopyRect(pDayViewEvent->m_rcEvent);
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_MultiDay::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarDayViewEvent* pViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarDayViewEvent, pObject);
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());

	if (!pDC || !pThemeX || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bNoVisibleMDEvent = pViewEvent->IsMultidayEvent() && !pViewEvent->IsVisible();

	int nMDEFlags = pViewEvent->GetMultiDayEventFlags();
	if ((nMDEFlags & xtpCalendarMultiDaySlave) || bNoVisibleMDEvent)
	{
		return;// was drawn in the Master (or out of events rect)
	}

	CRect rcEventMax(pViewEvent->GetViewEventRectMax());
	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return;
	}

	// fill event background
	BOOL bSelected = pViewEvent->IsSelected();

	COLORREF clrBorderNotSel;
	COLORREF clrBorder;
	CXTPPaintManagerColorGradient grclrBk;

	GetEventColors(pViewEvent, clrBorder, clrBorderNotSel, grclrBk);

	//----------------
	pThemeX->DrawRoundRect(pDC, rcEvent, clrBorder, bSelected, grclrBk);

	int nSelOffset = bSelected ? 2 : 1;

	if (nMDEFlags & xtpCalendarMultiDayFirst)
		pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcEvent.left + nSelOffset, rcEvent.top + nSelOffset),
						  rcEvent.right - nSelOffset, grclrBk, cnCornerSize / 2, rcEvent.bottom);

	CRect rcText(pViewEvent->GetTextRect());

	BOOL bHideArrow = FALSE;
	BOOL bHideArrowText = FALSE;

	if ((nMDEFlags & xtpCalendarMultiDayFirst) == 0)
	{
		rcText.left = rcEventMax.left;

		CRect rcSQ = rcEventMax;
		rcSQ.right = min(rcEvent.left + cnCornerSize, rcEventMax.right);
		CXTPCalendarTheme::DrawRectPartly(pDC, rcSQ, clrBorder, bSelected, grclrBk, CRect(1,1,0,1));

		pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcEvent.left + nSelOffset, rcEvent.top + nSelOffset),
						  rcEvent.right - nSelOffset, grclrBk, 0, rcEvent.bottom - nSelOffset);
		//---------------------------
		CRect rcArrow;

		BOOL bArrow = Draw_ArrowL(pViewEvent, pDC, rcArrow);
		bHideArrow = !bArrow;
		if (bArrow)
		{
			rcText.left = rcArrow.right;
		}

		if (bArrow && (nMDEFlags & xtpCalendarMultiDayLast) == 0)
		{
			CRect rcArrowText;
			CThemeFontColorSetValue* pFCsetFromTo = bSelected ? &m_fcsetSelected.fcsetStartEnd : &m_fcsetNormal.fcsetStartEnd;

			BOOL bArrowText = Draw_ArrowLtext(pViewEvent, pDC, pFCsetFromTo,
											  rcArrowText, rcArrow.right);
			bHideArrowText = !bArrowText;
			if (bArrowText)
			{
				rcText.left = rcArrowText.right;
			}
		}
	}
	else if (bSelected)
	{
		// draw left Gripper
		CRect rcGripper;
		rcGripper.top = rcEvent.top + rcEvent.Height() / 2 - cnGripperSize / 2;
		rcGripper.bottom = rcGripper.top + cnGripperSize;
		rcGripper.left = rcEvent.left - 1;
		rcGripper.right = rcGripper.left + cnGripperSize;

		pDC->FillSolidRect(&rcGripper, m_clrGripperBackground);
		CBrush brBorder(m_clrGripperBorder);
		pDC->FrameRect(&rcGripper, &brBorder);
	}

	if ((nMDEFlags & xtpCalendarMultiDayLast) == 0)
	{
		rcText.right = rcEventMax.right;

		CRect rcSQ = rcEventMax;
		rcSQ.left = max(rcEvent.right - cnCornerSize, rcEventMax.left);
		CXTPCalendarTheme::DrawRectPartly(pDC, rcSQ, clrBorder, bSelected, grclrBk, CRect(0,1,1,1));

		pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcSQ.left, rcEvent.top + nSelOffset),
						  rcEventMax.right - nSelOffset, grclrBk, 0, INT_MAX, FALSE);
		if (!bHideArrow)
		{
			CRect rcArrow;

			BOOL bArrow = Draw_ArrowR(pViewEvent, pDC, rcArrow);
			if (bArrow)
			{
				rcText.right = rcArrow.left;
			}

			if (bArrow && (nMDEFlags & xtpCalendarMultiDayFirst) == 0 && !bHideArrowText)
			{
				CRect rcArrowText;
				CThemeFontColorSetValue* pFCsetFromTo = bSelected ? &m_fcsetSelected.fcsetStartEnd : &m_fcsetNormal.fcsetStartEnd;

				BOOL bArrowText = Draw_ArrowRtext(pViewEvent, pDC, pFCsetFromTo,
												  rcArrowText, rcArrow.left);
				if (bArrowText)
				{
					rcText.right = rcArrowText.left;
				}
			}
		}
	}
	else if (bSelected)
	{
		// draw right gripper
		CRect rcGripper;
		rcGripper.top = rcEvent.top + rcEvent.Height() / 2 - cnGripperSize / 2;
		rcGripper.bottom = rcGripper.top + cnGripperSize;
		rcGripper.right = rcEvent.right + 1;
		rcGripper.left = rcGripper.right - cnGripperSize;;

		pDC->FillSolidRect(&rcGripper, m_clrGripperBackground);
		CBrush brBorder(m_clrGripperBorder);
		pDC->FrameRect(&rcGripper, &brBorder);
	}

	//=======================================================================
	CString strSubj = pViewEvent->GetItemTextSubject();
	CString strLoc = pViewEvent->GetItemTextLocation();

	if (!strLoc.IsEmpty())
	{
		strSubj += _T(";");
	}

	CThemeFontColorSet* pFntClr1 = bSelected ? &m_fcsetSelected.fcsetSubject : &m_fcsetNormal.fcsetSubject;
	CThemeFontColorSet* pFntClr2 = bSelected ? &m_fcsetSelected.fcsetLocation : &m_fcsetNormal.fcsetLocation;
	rcText.DeflateRect(3, 2, 3, 2);

	pViewEvent->m_rcSubjectEditor.CopyRect(rcText);
	//pViewEvent->AdjustSubjectEditorEx(FALSE);
	pViewEvent->AdjustSubjectEditor();

	UINT uTextShortAlign = DT_VCENTER | DT_LEFT;

	//--- calculate rects and fix to be centered if enough space ---
	CSize szText = pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
								rcText, uTextShortAlign | DT_CALCRECT);

	CRect rcIconsMax = rcText;
	rcIconsMax.left = min(rcIconsMax.left + rcText.Height(), rcIconsMax.right);

	CSize szIcons = Draw_Icons(pViewEvent, pDC, rcIconsMax, TRUE);

	if (rcIconsMax.Height() > szIcons.cy)
	{
		rcIconsMax.bottom -= (rcIconsMax.Height() - szIcons.cy) / 2;
	}

	int nTIcx = szText.cx + szIcons.cx;
	if (nTIcx + 5 < rcText.Width())
	{
		int ncx_2 = (rcText.Width() - nTIcx) / 2 - 2;
		rcText.left += ncx_2;
		rcText.right = rcText.left + szText.cx + 2;

		rcIconsMax.right -= ncx_2;
	}
	else
	{
		rcText.right -= szIcons.cx + 2;
	}

	//- Draw ================================
	pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
								rcText, uTextShortAlign);

	Draw_Icons(pViewEvent, pDC, rcIconsMax);

	// TODO:
/*
	BOOL bOutOfBorders = nY_need > rcText.Height() ||
						 nX_need > rcText.Width() ||
						 bTimeOutOfBorders;

	pViewEvent->SetTextOutOfBorders(bOutOfBorders);
*/
}

void CXTPCalendarThemeOffice2007::CTOEvent::FillEventBackgroundEx(CDC* pDC,
							CXTPCalendarViewEvent* pViewEvent, const CRect& rcRect)
{
	if (!pDC || !pViewEvent)
	{
		ASSERT(FALSE);
	}

	CRect rcEventFull = pViewEvent->GetEventRect();

	COLORREF clrBorderNotSel;
	COLORREF clrBorder;
	CXTPPaintManagerColorGradient grclrBk;

	GetEventColors(pViewEvent, clrBorder, clrBorderNotSel, grclrBk);

	if (pDC->IsPrinting())
	{
		CXTPCalendarTheme::DrawRectPartly(pDC, rcRect, 0, FALSE, grclrBk, CRect(0, 0, 0, 0));
	}
	else
	{
		XTPDrawHelpers()->GradientFill(pDC, &rcEventFull, grclrBk, FALSE, &rcRect);
	}
}

void CXTPCalendarThemeOffice2007::CTOEvent::GetEventColors(CXTPCalendarViewEvent* pViewEvent,
		COLORREF& rClrBorder, COLORREF& rClrBorderNotSel, CXTPPaintManagerColorGradient& rGrclrBk,
		BOOL bSelected, int* /*pnColorsSrc_1Cat2Lbl*/)
{
	rClrBorderNotSel = m_fcsetNormal.clrBorder;
	rClrBorder = m_fcsetNormal.clrBorder;
	rGrclrBk = m_fcsetNormal.grclrBackground;

	if (!pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return;
	}

	if (bSelected == -1)
	{
		bSelected = pViewEvent->IsSelected();
	}

	rClrBorder = bSelected ? m_fcsetSelected.clrBorder : m_fcsetNormal.clrBorder;
	rGrclrBk = bSelected ? m_fcsetSelected.grclrBackground : m_fcsetNormal.grclrBackground;

	CXTPCalendarEventLabel* ptrLabel = pViewEvent->GetEvent()->GetLabel();
	// TODO:
	if (ptrLabel && ptrLabel->m_nLabelID != XTP_CALENDAR_NONE_LABEL_ID)
	{
		COLORREF crBkLight = XTPDrawHelpers()->LightenColor(120, ptrLabel->m_clrColor);
		COLORREF crBkDark = XTPDrawHelpers()->DarkenColor(20, ptrLabel->m_clrColor);

		rGrclrBk.SetStandardValue(crBkLight, crBkDark);

		rClrBorderNotSel = XTPDrawHelpers()->DarkenColor(88, ptrLabel->m_clrColor);
		if (!bSelected)
		{
			rClrBorder = rClrBorderNotSel;
		}
	}
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_SingleDay::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarDayViewEvent* pViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarDayViewEvent, pObject);
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());

	if (!pDC || !pThemeX || !pViewEvent || !pViewEvent->GetEvent() ||
		!pViewEvent->GetViewGroup())
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bNoVisibleMDEvent = pViewEvent->IsMultidayEvent() && !pViewEvent->IsVisible();

	int nMDEFlags = pViewEvent->GetMultiDayEventFlags();
	if ((nMDEFlags & xtpCalendarMultiDaySlave) || bNoVisibleMDEvent)
	{
		return;// was drawn in the Master (or out of events rect)
	}

	CRect rcEvent(pViewEvent->GetEventRect());
	CRect rcTimeframe(pViewEvent->m_rcTimeframe);

	if (rcEvent.IsRectEmpty())
	{
		return;
	}

	// fill event background
	int nTopRow = pViewEvent->GetViewGroup()->GetTopRow();
	CRect rcTopRow = pViewEvent->GetViewGroup()->GetCellRect(nTopRow);

	BOOL bShortEvent = rcTimeframe.Height() + 1 < rcTopRow.Height();
	BOOL bSelected = pViewEvent->IsSelected();

	COLORREF clrBorderNotSel;
	COLORREF clrBorder;
	CXTPPaintManagerColorGradient grclrBk;

	GetEventColors(pViewEvent, clrBorder, clrBorderNotSel, grclrBk);

	//----------------
	CRect rcEventX(rcEvent);
	if (bShortEvent)
	{
		rcEventX.left += 5;
	}
	pThemeX->DrawRoundRect(pDC, rcEventX, clrBorder, bSelected, grclrBk);

	//----------------
	CRect rcText(rcEventX);
	rcText.left += 2;

	int nSelOffset = bSelected ? 2 : 1;

	int nBusyStatus = pViewEvent->GetEvent()->GetBusyStatus();
	CBrush* pBSBrush = GetBusyStatusBrush(nBusyStatus);

	if (!bShortEvent)
	{
		if (pBSBrush)
		{
			CRect rcBS = rcEvent;
			rcBS.right = min(rcBS.left + 12, rcEvent.right);

			CXTPTextDC autoText(pDC, clrBorderNotSel, RGB(255, 255, 255));
			pThemeX->DrawRoundRect(pDC, rcBS, clrBorder, bSelected, pBSBrush);

			rcBS.right++;
			rcBS.left = min(rcBS.left + 5, rcBS.right);

			CXTPCalendarTheme::DrawRectPartly(pDC, rcBS, clrBorder, bSelected, grclrBk, CRect(0, 1, 0, 1));

			rcBS.right = rcBS.left + 1;
			pDC->FillSolidRect(&rcBS, clrBorder);

			rcText.left = rcEvent.left + 6;

			pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcText.left, rcEvent.top + nSelOffset),
								  rcEvent.right - nSelOffset, grclrBk);
		}
		else
		{
			pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcEvent.left + nSelOffset, rcEvent.top + nSelOffset),
						  rcEvent.right - nSelOffset, grclrBk, cnCornerSize / 2, rcEvent.bottom);
		}
	}
	else
	{
		CRect rcBS = rcEvent;
		rcBS.right = rcEventX.left + 6;

		CRect rcBSfull = rcBS;
		rcBS.bottom = rcTimeframe.bottom;

		// draw BK's
		if (pDC->IsPrinting())
		{
			CXTPCalendarTheme::DrawRectPartly(pDC, rcBS, 0, FALSE, grclrBk, CRect(0, 0, 0, 0));
		}
		else
		{
			XTPDrawHelpers()->GradientFill(pDC, &rcBSfull, grclrBk, FALSE, &rcBS);
		}

		if (pBSBrush)
		{
			CRect rcBSfill = rcBS;
			rcBSfill.right = rcEventX.left;

			CXTPTextDC autoText(pDC, clrBorderNotSel, RGB(255, 255, 255));
			pDC->FillRect(&rcBSfill, pBSBrush);
		}

		// draw borders
		int nBorderWidth = bSelected ? 2 : 1;
		CXTPCalendarTheme::DrawRectPartly(pDC, rcBS, clrBorder, nBorderWidth, CRect(1, 1, 0, 0));

		rcBS.right = rcEventX.left + 1;
		rcBS.top += nBorderWidth;
		if (pBSBrush)
		{
			CXTPCalendarTheme::DrawRectPartly(pDC, rcBS, clrBorderNotSel, 1, CRect(0, 0, 1, 0));
		}

		rcBS.right += bSelected ? 1 : 0;
		CXTPCalendarTheme::DrawRectPartly(pDC, rcBS, clrBorder, nBorderWidth, CRect(0, 0, 0, 1));

		rcText.left = rcEventX.left;

		if (pBSBrush)
		{
			pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcEventX.left + 1, rcEvent.top + nSelOffset),
						  rcEvent.right - nSelOffset, grclrBk);
		}
		else
		{
			pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcEvent.left + nSelOffset, rcEvent.top + nSelOffset),
						  rcEvent.right - nSelOffset, grclrBk, 0, rcTimeframe.bottom - nSelOffset);
		}
	}

	//-------------------------------------------------------
	if (bSelected)
	{
		CBrush brBorder(m_clrGripperBorder);
		CRect rcGripper;

		rcGripper.left = rcEvent.left + rcEvent.Width() / 2 - cnGripperSize / 2;
		rcGripper.right = rcGripper.left + cnGripperSize;

		// draw top Gripper
		rcGripper.top = rcEvent.top - 1; //cnGripperSize / 2 + 1;
		rcGripper.bottom = rcGripper.top + cnGripperSize;

		pDC->FillSolidRect(&rcGripper, m_clrGripperBackground);
		pDC->FrameRect(&rcGripper, &brBorder);

		// draw bottom Gripper
		rcGripper.bottom = rcEvent.bottom + 1; //cnGripperSize / 2 - 1;
		rcGripper.top = rcGripper.bottom - cnGripperSize;


		pDC->FillSolidRect(&rcGripper, m_clrGripperBackground);
		pDC->FrameRect(&rcGripper, &brBorder);
	}

	//=====================================
	CString strSubj = pViewEvent->GetItemTextSubject();
	CString strLoc = pViewEvent->GetItemTextLocation();
	CString strBody = pViewEvent->GetItemTextBody();

	CThemeFontColorSet* pFntClr1 = bSelected ? &m_fcsetSelected.fcsetSubject  : &m_fcsetNormal.fcsetSubject;
	CThemeFontColorSet* pFntClr2 = bSelected ? &m_fcsetSelected.fcsetLocation : &m_fcsetNormal.fcsetLocation;
	CThemeFontColorSet* pFntClr3 = bSelected ? &m_fcsetSelected.fcsetBody     : &m_fcsetNormal.fcsetBody;
	rcText.DeflateRect(3, 3, 3, 3);

	pViewEvent->m_rcSubjectEditor.CopyRect(rcText);
	//pViewEvent->AdjustSubjectEditorEx(FALSE);
	pViewEvent->AdjustSubjectEditor();

	//----------------------
	CRect rcIconsMax = rcText;

	CSize szIcons = Draw_Icons(pViewEvent, pDC, rcIconsMax, TRUE);

	if (rcIconsMax.Height() >= rcTopRow.Height())
	{
		rcIconsMax.bottom -= 4;
	}
	else
	{
		if (rcTopRow.Height() - 4 > szIcons.cy)
		{
			rcIconsMax.bottom = rcIconsMax.top + rcIconsMax.Height()/2 + szIcons.cy/2;
		}

		rcIconsMax.left = min(rcIconsMax.left + rcTopRow.Height(), rcIconsMax.right);
		rcText.right = max(rcText.right - szIcons.cx - 2, rcText.left + rcTopRow.Height() - 2);
	}

	//------------------------------------------------------------
	pThemeX->DrawText_Auto2SL3ML(pDC, strSubj, strLoc, strBody,
								 pFntClr1, pFntClr2, pFntClr3, rcText, _T(";"));

	Draw_Icons(pViewEvent, pDC, rcIconsMax);

/*
	BOOL bOutOfBorders = nY_need > rcText.Height() ||
						 nX_need > rcText.Width() ||
						 bTimeOutOfBorders;
	pViewEvent->SetTextOutOfBorders(bOutOfBorders);
	*/


}

void CXTPCalendarThemeOffice2007::_DrawEventLTHighlight(CDC* pDC, CPoint ptLT, int nRTx, const CXTPPaintManagerColorGradient& grclrBk,
									int nLTCorner, int nLBy, BOOL bRCorner)
{
	COLORREF clrTop = XTPDrawHelpers()->LightenColor(120, grclrBk.clrLight);

	CRect rcTop(ptLT.x + nLTCorner, ptLT.y, nRTx, ptLT.y + 1);
	if (bRCorner)
		rcTop.right -= cnCornerSize / 2 + 1;

	pDC->FillSolidRect(&rcTop, clrTop);

	if(nLBy != INT_MAX)
	{
		CRect rcLLVert(ptLT.x, ptLT.y + nLTCorner, ptLT.x + 1, nLBy - nLTCorner);
		rcLLVert.top += nLTCorner ? 0 : 1;

		COLORREF clrLight = XTPDrawHelpers()->LightenColor(76, grclrBk.clrLight);
		COLORREF clrDrak = XTPDrawHelpers()->LightenColor(15, grclrBk.clrDark);

		XTPDrawHelpers()->GradientFill(pDC, rcLLVert, clrLight, clrDrak, FALSE);

		CPen pen(PS_SOLID, 1, clrLight);
		CXTPPenDC autoPen(pDC, &pen);

		pDC->MoveTo(rcLLVert.TopLeft());
		pDC->LineTo(rcTop.TopLeft());
	}

	if (bRCorner)
	{
		COLORREF clrLight2 = XTPDrawHelpers()->LightenColor(80, grclrBk.clrLight);
		CPen pen(PS_SOLID, 1, clrLight2);
		CXTPPenDC autoPen(pDC, &pen);

		pDC->MoveTo(rcTop.right, rcTop.top);
		pDC->LineTo(rcTop.right + cnCornerSize / 2 + 1, rcTop.top + cnCornerSize / 2 + 1);
	}
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_SingleDay::InitBusyStatusDefaultColors()
{
	TBase2::InitBusyStatusDefaultColors();

	SetBusyStatusBrush(xtpCalendarBusyStatusBusy, NULL, FALSE);

	// Tentative ---------------------------
	m_bmpTentativePattern.DeleteObject();
	m_brushTentative.DeleteObject();

	VERIFY(XTPResourceManager()->LoadBitmap(&m_bmpTentativePattern, XTP_IDB_CALENDAR_TENATIVE_PATTERN_2007));
	VERIFY(m_brushTentative.CreatePatternBrush(&m_bmpTentativePattern));

	CXTPCalendarViewPartBrushValue* pBVal_Tentative = _GetBusyStatusBrushValue(xtpCalendarBusyStatusTentative, TRUE);
	if (pBVal_Tentative)
	{
		pBVal_Tentative->SetStandardValue(&m_brushTentative, FALSE);
	}
}

void CXTPCalendarThemeOffice2007::CTODayViewEvent_SingleDay::AdjustLayout(CCmdTarget* pObject,
							CDC* pDC, const CRect& rcRect, int nEventPlaceNumber)
{
	UNREFERENCED_PARAMETER(nEventPlaceNumber);

	CXTPCalendarDayViewEvent* pViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarDayViewEvent, pObject);
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());

	if (!pDC || !pThemeX || !pViewEvent || !pViewEvent->GetEvent() || !pViewEvent->GetViewGroup() ||
		!pViewEvent->GetViewGroup()->GetViewDay())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarEvent* pEvent = pViewEvent->GetEvent();
	CXTPCalendarDayViewGroup* pViewGroup = pViewEvent->GetViewGroup();
	CXTPCalendarDayViewDay* pViewDay = pViewGroup->GetViewDay();

	pViewEvent->m_nMultiDayEventFlags |= xtpCalendarMultiDayNoMultiDay;

	// non all-day event
	int nPace = pViewEvent->GetEventPlacePos();
	int nPaceCount = max(1, pViewEvent->GetEventPlaceCount());

	ASSERT(pViewEvent->GetEventPlaceCount() >= 1);
	ASSERT(nPace >= 0 && nPace < nPaceCount);

	pViewEvent->m_rcEvent.CopyRect(rcRect);
	int nEventsAreaX = pViewEvent->m_rcEvent.Width() - 5;
	int nEventWidth = nEventsAreaX / nPaceCount;

	// skip event views if they are not fit even by 5 pixel bar width
	// and correct some first event views heights
	if (nEventWidth <= 5)
	{
		while (nPaceCount > 1 && nEventWidth <= 5)
		{
			nPaceCount--;
			nEventWidth = nEventsAreaX / nPaceCount;
		}
		if (nPace > nPaceCount)
		{
			pViewEvent->m_rcEvent.SetRectEmpty();
			return;
		}
	}

	// calc other rect coordinates
	pViewEvent->m_rcEvent.left += nEventWidth * nPace;
	pViewEvent->m_rcEvent.right = pViewEvent->m_rcEvent.left + nEventWidth;

	//-------------------------------------------------------------------
	COleDateTime dtStartTime(pEvent->GetStartTime());
	COleDateTime dtEndTime(pEvent->GetEndTime());

	int nTopRow = pViewGroup->GetTopRow();

	CRect rcTopRow = pViewGroup->GetCellRect(nTopRow);
	CRect rcRow0 = pViewGroup->GetCellRect(0);
	CRect rcRowLast = pViewGroup->GetCellRect(pViewGroup->GetRowCount()-1);

	COleDateTime dtTopRowTime = pViewDay->GetView()->GetCellTime(nTopRow);
	dtTopRowTime = (double)pViewDay->GetDayDate() + (double)dtTopRowTime;

	double dblCellDur = (double)pViewDay->GetView()->GetCellDuration();
	if (dblCellDur <= XTP_HALF_SECOND)
	{
		dblCellDur = XTP_HALF_SECOND * 2 * 60 * 30;
	}

	double dblEventOffsetTime1 = double(dtStartTime - dtTopRowTime);
	double dblEventOffsetTime2 = double(dtEndTime - dtTopRowTime);
	double dblPixelPerTime = rcTopRow.Height() / dblCellDur;

	int nEventY1 = int(dblPixelPerTime * dblEventOffsetTime1);
	nEventY1 = max(rcRow0.top,  rcTopRow.top + nEventY1);
	nEventY1 = min(rcRowLast.top, nEventY1);

	int nEventY2 = int(dblPixelPerTime * dblEventOffsetTime2);
	nEventY2 = rcTopRow.top + nEventY2;
	nEventY2 = min(rcRowLast.bottom, nEventY2);

	int nEvent_dy = nEventY2 - nEventY1;

	BOOL bShortEvent = nEvent_dy + 1 < rcTopRow.Height();
	nEvent_dy = max(5, nEvent_dy);

	if (bShortEvent)
	{
		nEventY2 = nEventY1 + rcTopRow.Height();
	}

	///%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	pViewEvent->m_rcEvent.top = nEventY1;
	pViewEvent->m_rcEvent.bottom = nEventY2;

	pViewEvent->m_rcEventMax.top = max(pViewEvent->m_rcEvent.top-5, rcRect.top);
	pViewEvent->m_rcEventMax.bottom = min(pViewEvent->m_rcEvent.bottom + 5, rcRect.bottom);

	// adjust subject area
	pViewEvent->m_rcText.CopyRect(pViewEvent->m_rcEvent);
	pViewEvent->m_rcText.left += 10; // correct to busy area
	pViewEvent->m_rcText.left += 4;

	pViewEvent->m_rcSubjectEditor.CopyRect(pViewEvent->m_rcText);

	// Set time frame rect
	pViewEvent->m_rcTimeframe.top = pViewEvent->m_rcEvent.top;
	pViewEvent->m_rcTimeframe.bottom = pViewEvent->m_rcTimeframe.top + nEvent_dy;

	pViewEvent->m_rcTimeframe.left = pViewEvent->m_rcEvent.left + 1;
	pViewEvent->m_rcTimeframe.right = pViewEvent->m_rcTimeframe.left + 4;

	// set expanded signs for day view
	CRect rcEvents = rcRect;

	if (pViewEvent->m_rcEvent.top >= rcEvents.bottom-10)
		pViewEvent->GetViewGroup()->SetExpandDown();

	if (pViewEvent->m_rcEvent.bottom <= rcEvents.top + 10)
		pViewEvent->GetViewGroup()->SetExpandUp();
}

/////////////////////////////////////////////////////////////////////////////
//
// ***** Month View *****
//

void CXTPCalendarThemeOffice2007::CTOMonthView::RefreshMetrics(BOOL bRefreshChildren)
{
	TBase::RefreshMetrics(bRefreshChildren);
}

void CXTPCalendarThemeOffice2007::CTOMonthViewEvent::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	TBase::RefreshMetrics(bRefreshChildren);

	RefreshFromParent(pThemeX->GetEventPartX());

	XTP_SAFE_CALL1(GetHeightFormulaPart(), SetStandardValue(14, 10, 1));
	XTP_SAFE_CALL1(GetEventIconsToDrawPart(), SetStandardValue(FALSE, FALSE, FALSE, FALSE, FALSE) );
}

void CXTPCalendarThemeOffice2007::CTOMonthViewEvent_MultiDay::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	RefreshFromParent(pThemeX->GetMonthViewPartX()->GetEventPartX());

	m_fcsetNormal.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventBorder));
	m_fcsetNormal.grclrBackground.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventBkGRfrom),
									   pThemeX->GetColor(xtpCLR_MultiDayEventBkGRto)   );

	m_fcsetSelected.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventSelectedBorder));
	m_fcsetSelected.grclrBackground.SetStandardValue(m_fcsetNormal.grclrBackground);

	//---------------------------------
	m_fcsetNormal.fcsetStartEnd.fntFont.SetStandardValue(pThemeX->m_fntBaseFont);
	m_fcsetNormal.fcsetStartEnd.clrColor.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventFromToDates));

	m_fcsetSelected.fcsetStartEnd.fntFont.SetStandardValue(pThemeX->m_fntBaseFont);
	m_fcsetSelected.fcsetStartEnd.clrColor.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventFromToDates));
}

void CXTPCalendarThemeOffice2007::CTOMonthViewEvent_SingleDay::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	RefreshFromParent(pThemeX->GetMonthViewPartX()->GetEventPartX());

	CFont* pFontDef = pThemeX->m_fntBaseFont;

	m_fcsetNormal.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_MonthViewSingleDayEventBorder));
	m_fcsetNormal.grclrBackground.SetStandardValue(pThemeX->GetColor(xtpCLR_MonthViewSingleDayEventBkGRfrom),
												   pThemeX->GetColor(xtpCLR_MonthViewSingleDayEventBkGRto) );

	m_fcsetSelected.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_MonthViewSingleDayEventSelectedBorder));
	m_fcsetSelected.grclrBackground.SetStandardValue(pThemeX->GetColor(xtpCLR_MonthViewSingleDayEventBkGRfrom),
													 pThemeX->GetColor(xtpCLR_MonthViewSingleDayEventBkGRto) );

	//----------------------------------------------
	pThemeX->RemoveBoldAttrFromStd(m_fcsetNormal.fcsetSubject.fntFont);
	pThemeX->RemoveBoldAttrFromStd(m_fcsetSelected.fcsetSubject.fntFont);

	//----------------------------------------------
	m_fcsetNormal.fcsetStartEnd.clrColor.SetStandardValue(pThemeX->GetColor(xtpCLR_MonthViewEventTime));
	m_fcsetNormal.fcsetStartEnd.fntFont.SetStandardValue(pFontDef);

	m_fcsetSelected.fcsetStartEnd.clrColor.SetStandardValue(pThemeX->GetColor(xtpCLR_MonthViewEventTime));
	m_fcsetSelected.fcsetStartEnd.fntFont.SetStandardValue(pFontDef);
}

int CXTPCalendarThemeOffice2007::CTOMonthViewWeekHeader::CalcWidth(CDC* pDC)
{
	return CalcHeight(pDC, -1);
}

void CXTPCalendarThemeOffice2007::CTOMonthView::AdjustLayout(CDC* pDC, const CRect& rcRect, BOOL bCallPostAdjustLayout)
{
	CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView,
		XTP_SAFE_GET2(GetTheme(), GetCalendarControl(), GetMonthView(), NULL));

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());

	if (!pDC || !pThemeX || !pMonthView || !pMonthView->GetGrid())
	{
		ASSERT(FALSE);
		return;
	}

	int nWHeaderWidth = GetWeekHeaderPartX()->CalcWidth(pDC);

	CRect rcRectWHeader = rcRect;
	rcRectWHeader.right = min(rcRectWHeader.left + nWHeaderWidth, rcRect.right);

	GetWeekHeaderPartX()->AdjustLayout(pMonthView, pDC, rcRectWHeader);

	CRect rcRectGrid = rcRect;
	rcRectGrid.left = rcRectWHeader.right;

	TBase::AdjustLayout(pDC, rcRectGrid, bCallPostAdjustLayout);
}

void CXTPCalendarThemeOffice2007::CTOMonthViewEvent::AdjustLayout(CCmdTarget* pObject,
																  CDC* pDC, const CRect& rcRect, int nEventPlaceNumber)
{
	CXTPCalendarMonthViewEvent* pViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarMonthViewEvent, pObject);

	if (!pDC || !GetTheme() || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return;
	}

	int nEventHeight = XTP_SAFE_GET4(pViewEvent, GetViewGroup(), GetViewDay(), GetView(), GetRowHeight(), 0);

	pViewEvent->m_rcEventMax.top = rcRect.top + nEventPlaceNumber * nEventHeight;
	pViewEvent->m_rcEventMax.bottom = pViewEvent->m_rcEventMax.top + nEventHeight;

	pViewEvent->m_rcEvent.top = pViewEvent->m_rcEventMax.top;
	pViewEvent->m_rcEvent.bottom = pViewEvent->m_rcEventMax.bottom;
	pViewEvent->m_rcEvent.left = pViewEvent->m_rcEventMax.left + 4 + 2;
	pViewEvent->m_rcEvent.right = pViewEvent->m_rcEventMax.right - 5 - 2;

	pViewEvent->m_rcToolTip.CopyRect(pViewEvent->m_rcEventMax);

	// adjust subject area
	int nDyx = pViewEvent->IsSelected() ? 5 : 2;

	pViewEvent->m_rcText.CopyRect(pViewEvent->m_rcEvent);
	pViewEvent->m_rcText.DeflateRect(nDyx + 3, 3, nDyx + 2, 3);

	// set times rects
	pViewEvent->m_rcStartTime.SetRectEmpty();
	pViewEvent->m_rcEndTime.SetRectEmpty();

	pViewEvent->m_rcSubjectEditor.CopyRect(pViewEvent->m_rcEvent);
	pViewEvent->m_rcSubjectEditor.top++;
	pViewEvent->m_rcSubjectEditor.bottom--;
}

void CXTPCalendarThemeOffice2007::CTOMonthView::Draw(CDC* pDC)
{
	CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView,
		XTP_SAFE_GET2(GetTheme(), GetCalendarControl(), GetMonthView(), NULL));

	if (!pDC || !pMonthView)
	{
		ASSERT(FALSE);
		return;
	}

	pDC->SetBkMode(TRANSPARENT);

	GetWeekHeaderPartX()->Draw(pMonthView, pDC);

	//TBase::Draw(pDC);

	GetWeekDayHeaderPart()->Draw(pMonthView, pDC);

	CXTPPropsStateContext autoStateCnt1_day(GetDayPart(), xtpCalendarBeforeDraw_MonthViewDay, TRUE);

	//-- Draw Days with events ------------------------------
	int nCount = pMonthView->GetViewDayCount();
	int i;

	for (i = nCount - 1; i >= 0; i--)
	{
		CXTPCalendarMonthViewDay* pViewDay = pMonthView->GetViewDay(i);
		ASSERT(pViewDay);
		if (pViewDay)
		{
			autoStateCnt1_day.SendBeforeDrawThemeObject((LPARAM)pViewDay);

			GetDayPart()->Draw(pViewDay, pDC);

			autoStateCnt1_day.RestoreState();
		}
	}
	autoStateCnt1_day.Clear();

	// All saved data must be freed after draw!
	ASSERT(CXTPPropsState::s_dbg_nActiveStartes == 0);
}

//void CXTPCalendarThemeOffice2007::CTOMonthViewWeekDayHeader::_RefreshColors()
//{
//  m_clrsetNormal.grclrBkTop.SetStandardValue(m_clrBaseColor);
//  m_clrsetNormal.grclrBkBottom.SetStandardValue(m_clrBaseColor);
//
//  m_clrsetNormal.clrBorder.SetStandardValue(m_clrBaseColor); // no border
//
//  _RefreshToodayColors();
//}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekDayHeader::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	RefreshFromParent(pThemeX->GetHeaderPartX());

	m_clrBaseColor.SetStandardValue(RGB(227, 239, 255));

	m_bkNormal.nBitmapID.       SetStandardValue(0); //xtpIMG_MonthViewWeekDayHeader);
	m_bkSelected.nBitmapID.     SetStandardValue(0); //xtpIMG_MonthViewWeekDayHeader);
	m_bkToday.nBitmapID.        SetStandardValue(0); //xtpIMG_MonthViewWeekDayHeader);
	m_bkTodaySelected.nBitmapID.SetStandardValue(0); //xtpIMG_MonthViewWeekDayHeader);

	m_TextCenter.fcsetNormal.clrColor.SetStandardValue(RGB(101, 147, 201));

	// 0 - Sunday, 1 - Monday, ...; 7 - Sat/Sun
	m_arWeekDayNamesLong.RemoveAll();
	m_arWeekDayNamesShort.RemoveAll();

	m_arWeekDayNamesLong.SetSize(8);
	m_arWeekDayNamesShort.SetSize(8);

	for (int i = 0; i < 7; i++)
	{
		CString sWDlong = CXTPCalendarUtils::GetLocaleString(LOCALE_SDAYNAME1 + i, 100);
		CString sWDshort = CXTPCalendarUtils::GetLocaleString(LOCALE_SABBREVDAYNAME1 + i, 100);

		m_arWeekDayNamesLong [(i + 1) % 7] = sWDlong;
		m_arWeekDayNamesShort[(i + 1) % 7] = sWDshort;
	}

	m_arWeekDayNamesLong [7] = m_arWeekDayNamesLong[6] + _T("/") + m_arWeekDayNamesLong[0];
	m_arWeekDayNamesShort[7] = m_arWeekDayNamesShort[6] + _T("/") + m_arWeekDayNamesShort[0];

}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekHeader::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	RefreshFromParent(pThemeX->GetHeaderPartX());

	m_clrFreeSpaceBk.SetStandardValue(RGB(227, 239, 255));

	m_bkNormal.nBitmapID.SetStandardValue(xtpIMG_MonthViewWeekHeader);
	m_bkSelected.nBitmapID.SetStandardValue(xtpIMG_MonthViewWeekHeader);
	m_bkToday.nBitmapID.SetStandardValue(xtpIMG_MonthViewWeekHeader);
	m_bkTodaySelected.nBitmapID.SetStandardValue(xtpIMG_MonthViewWeekHeader);

	LOGFONT lfVert;
	VERIFY( m_TextCenter.fcsetNormal.fntFont->GetLogFont(&lfVert) );
	lfVert.lfOrientation = 900;
	lfVert.lfEscapement = 900;
	if (XTPDrawHelpers()->FontExists(XTP_CALENDAR_OFFICE2007_FONT_NAME))
	{
		STRCPY_S(lfVert.lfFaceName, LF_FACESIZE, XTP_CALENDAR_OFFICE2007_FONT_NAME);
	}
	else
	{
		STRCPY_S(lfVert.lfFaceName, LF_FACESIZE, XTPDrawHelpers()->GetVerticalFontName(TRUE));
	}

	m_TextCenter.fcsetNormal.fntFont.SetStandardValue(&lfVert);
	m_TextCenter.fcsetNormal.clrColor.SetStandardValue(RGB(0, 0, 0)); //pThemeX->GetColor(xtpCLR_HeaderBorder));
}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekDayHeader::AdjustLayout(
					CCmdTarget* pObject, CDC* pDC, const CRect& rcRect)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView, pObject);

	if (!pDC || !pThemeX || !pMonthView || !pMonthView->GetGrid())
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarMonthView::CMonthViewGrid* pGrid = pMonthView->GetGrid();
	int nColsCount = pGrid->GetColsCount();
	int nCellWidth = rcRect.Width() / max(1, nColsCount);

	int nHeaderHeight = CalcHeight(pDC, nCellWidth);
	pGrid->SetColHeaderHeight(nHeaderHeight);

	if (!nColsCount || m_arWeekDayNamesLong.GetSize() < 8 ||
		m_arWeekDayNamesShort.GetSize() < 8)
	{
		ASSERT(FALSE);
		return;
	}

	CString strDWMaxLong;
	for (int i = 0; i < 7; i++)
	{
		if (strDWMaxLong.GetLength() < m_arWeekDayNamesLong[i].GetLength())
		{
			strDWMaxLong = m_arWeekDayNamesLong[i];
		}
	}

	CXTPFontDC autoFont(pDC, m_TextCenter.fcsetNormal.Font());

	CSize szWDlong = pDC->GetTextExtent(strDWMaxLong);
	CSize szSaSuLong = pDC->GetTextExtent(m_arWeekDayNamesLong[7]);

	int nRectSize = nCellWidth;

	m_bWeekDayNamesLong = szWDlong.cx + szWDlong.cx/4 + nRectSize/5 < nRectSize;
	m_bWeekDayNameSaSuLong = szSaSuLong.cx + szSaSuLong.cx/4 + nRectSize/5 < nRectSize;
}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekDayHeader::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView, pObject);
	if (!pDC || !pThemeX || !pMonthView || !pMonthView->GetGrid())
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarMonthView::CMonthViewGrid* pGrid = pMonthView->GetGrid();

	CRect rcWDHeader = pMonthView->GetViewRect();
	rcWDHeader.bottom = rcWDHeader.top + pGrid->GetColHeaderHeight();

	CXTPPropsStateContext autoStateCnt1(this, xtpCalendarBeforeDraw_MonthViewWeekDayHeader, TRUE);
	autoStateCnt1.SendBeforeDrawThemeObject((LPARAM)-1);

	//===========================================
	CStringArray arColHeaderText;
	CUIntArray arWeekDays;
	arColHeaderText.SetSize(7);
	arWeekDays.SetSize(7);

	for (int i = 0; i < 7; i++)
	{
		int nCol = pGrid->GetWeekDayCol(i + 1);

		CString sVal = arColHeaderText[nCol];
		if (!sVal.IsEmpty())
		{
			// compressed col - always Sat/Sun
			arColHeaderText[nCol] = m_bWeekDayNameSaSuLong ? m_arWeekDayNamesLong[7] : m_arWeekDayNamesShort[7];
			arWeekDays[nCol] = 7;
		}
		else
		{
			CString sWD = m_bWeekDayNamesLong ? m_arWeekDayNamesLong[i] : m_arWeekDayNamesShort[i];
			arColHeaderText[nCol] = sWD;
			arWeekDays[nCol] = i + 1;
		}
	}

	CXTPPropsStateContext autoStateCnt2_wd(this, xtpCalendarBeforeDraw_MonthViewWeekDayHeader);
	//===========================================
	int nColsCount = pGrid->GetColsCount();
	for (int nCol = 0; nCol < nColsCount; nCol++)
	{
		CRect rcCell = pGrid->GetCelRect(0, nCol, TRUE);
		rcCell.top = rcWDHeader.top;
		rcCell.bottom = rcWDHeader.bottom;

		CString strWD = arColHeaderText[nCol];
		int nWeekDay = arWeekDays[nCol];

		autoStateCnt2_wd.SendBeforeDrawThemeObject((LPARAM)nWeekDay);

		//--------------------------------------------------------
		pMonthView->GetWeekDayTextIfNeed(&strWD, nWeekDay);

		Draw_Header(pDC, rcCell, 0, _T(""), strWD);

		//--------------------------------------------------------
		autoStateCnt2_wd.RestoreState();
	}
	autoStateCnt2_wd.Clear();
}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekHeader::AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView, pObject);
	if (!pDC || !pThemeX || !pMonthView || !pMonthView->GetGrid())
	{
		ASSERT(FALSE);
		return;
	}

	//***
	m_rcHeader = rcRect;

	//***
	int nWeeksCount = pMonthView->GetGrid()->GetWeeksCount();
	if (!nWeeksCount)
	{
		return;
	}

	CXTPCalendarMonthViewDay* pViewDay = pMonthView->GetGrid()->GetViewDay(0, 0);
	ASSERT(pViewDay);
	if (!pViewDay)
	{
		return;
	}
	const CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = pThemeX->GetViewDayLayout(pViewDay);

	COleDateTime dtWDay1 = pViewDay->GetDayDate();

	CRect rcWHeaderTextVert = m_rcHeader;
	rcWHeaderTextVert.top = dayLayout.m_rcDayHeader.bottom;
	rcWHeaderTextVert.bottom = dayLayout.m_rcDay.bottom;

	//---
	CString strTmp, strCaptionLong;
	FormatWeekCaption(dtWDay1, strTmp, strCaptionLong, 2);

	CXTPFontDC autoFont(pDC, m_TextCenter.fcsetNormal.fntFont, m_TextCenter.fcsetNormal.clrColor);
	CSize szText1 = pDC->GetTextExtent(strCaptionLong);

	int nRectSize = abs(rcWHeaderTextVert.Height());

	m_bDateFormatShort = TRUE;
	if (szText1.cx + szText1.cx/3 + nRectSize/10 < nRectSize)
	{
		m_bDateFormatShort = FALSE;
	}
}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekDayHeader::Draw_Borders2(
			CDC* /*pDC*/, const CRect& /*rcRect*/, BOOL /*bDrawRightBorder*/)
{

}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekHeader::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("MonthViewWeekHeader")));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("FreeSpaceBkColor"), m_clrFreeSpaceBk);

	TBase::DoPropExchange(pPX);
}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekHeader::Serialize(CArchive& ar)
{
	m_clrFreeSpaceBk.Serialize(ar);

	TBase::Serialize(ar);
}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekHeader::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView, pObject);
	if (!pDC || !pThemeX || !pMonthView || !pMonthView->GetGrid())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPPropsStateContext autoStateCnt1(this, xtpCalendarBeforeDraw_MonthViewWeekHeader, TRUE);
	autoStateCnt1.SendBeforeDrawThemeObject((LPARAM)-1);

	pDC->FillSolidRect(&m_rcHeader, m_clrFreeSpaceBk);

	CXTPFontDC autoFont(pDC, m_TextCenter.fcsetNormal.fntFont, m_TextCenter.fcsetNormal.clrColor);

	CXTPPropsStateContext autoStateCnt2_week(this, xtpCalendarBeforeDraw_MonthViewWeekHeader);

	int nWeeksCount = pMonthView->GetGrid()->GetWeeksCount();
	for (int i = 0; i < nWeeksCount; i++)
	{
		CXTPCalendarMonthViewDay* pViewDay = pMonthView->GetGrid()->GetViewDay(i, 0);
		ASSERT(pViewDay);
		if (!pViewDay)
		{
			continue;
		}
		const CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = pThemeX->GetViewDayLayout(pViewDay);

		COleDateTime dtWDay1 = pViewDay->GetDayDate();

		CRect rcWHeader = m_rcHeader;
		rcWHeader.left+= 3;
		rcWHeader.top = dayLayout.m_rcDayHeader.bottom - 1;
		rcWHeader.bottom = dayLayout.m_rcDay.bottom;

		autoStateCnt2_week.SendBeforeDrawThemeObject((LPARAM)i);
		//--------------------------------------------------
		CString strWeekCaption;
		FormatWeekCaption(dtWDay1, strWeekCaption, strWeekCaption, m_bDateFormatShort ? 1 : 2);

		//--------------------------------------------------
		Draw_Background(pDC, rcWHeader, 0);

		Draw_TextCenter(pDC, rcWHeader, 0, strWeekCaption);


		if (i +1 < nWeeksCount)
			autoStateCnt2_week.RestoreState();
	}
	autoStateCnt2_week.Clear();
}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekHeader::Draw_TextCenter(CDC* pDC, const CRect& rcRect, int nState,
									 LPCTSTR pcszCenterText, int* pnWidth)
{
	UNREFERENCED_PARAMETER(nState);
	UNREFERENCED_PARAMETER(pnWidth);

	// Warning: not supported.
	ASSERT(pnWidth == NULL);

	CRect rcWHeaderTextVert = rcRect;
	rcWHeaderTextVert.DeflateRect(3, 2, 1, 3);

	int nTmp = rcWHeaderTextVert.top;
	rcWHeaderTextVert.top = rcWHeaderTextVert.bottom;
	rcWHeaderTextVert.bottom = nTmp;

	int nRectSize = abs(rcWHeaderTextVert.Height());

	//---
	CSize szText = pDC->GetTextExtent(pcszCenterText);

	// Center text vertically
	if (szText.cx + 7 < nRectSize)
	{
		rcWHeaderTextVert.top = max(rcWHeaderTextVert.top - (nRectSize - szText.cx)/2, rcWHeaderTextVert.bottom);
	}

	pDC->DrawText(CString(pcszCenterText), &rcWHeaderTextVert, DT_NOPREFIX | DT_SINGLELINE); //| DT_NOCLIP
}

void CXTPCalendarThemeOffice2007::CTOMonthViewWeekHeader::FormatWeekCaption(COleDateTime dtWDay1,
							CString& rstrShort, CString& rstrLong, int nShort1Long2 )
{
	//LOCALE_IDATE  -  short date format ordering
	// 0 Month-Day-Year
	// 1 Day-Month-Year
	// 2 Year-Month-Day

	int nDateOrdering = CXTPCalendarUtils::GetLocaleLong(LOCALE_IDATE);
	CString strDSeparator = CXTPCalendarUtils::GetLocaleString(LOCALE_SDATE, 100);

	// SHORT
	COleDateTime dtWDay6 = dtWDay1 + COleDateTimeSpan(6, 0, 0, 0);

	const int cnBuffSize = 128;

	TCHAR szWDay1[cnBuffSize], szMonth1[cnBuffSize];
	TCHAR szWDay6[cnBuffSize], szMonth6[cnBuffSize];

	ITOT_S(dtWDay1.GetDay(), szWDay1, cnBuffSize);
	ITOT_S(dtWDay6.GetDay(), szWDay6, cnBuffSize);

	if (nShort1Long2 == 0 || nShort1Long2 == 1)
	{
		ITOT_S(dtWDay1.GetMonth(), szMonth1, cnBuffSize);
		ITOT_S(dtWDay6.GetMonth(), szMonth6, cnBuffSize);

		rstrShort = _FormatWCaption(szWDay1, szMonth1, szWDay6, szMonth6, strDSeparator, nDateOrdering);
	}

	if (nShort1Long2 == 0 || nShort1Long2 == 2)
	{
		CString strMonth1 = CXTPCalendarUtils::GetLocaleString(LOCALE_SABBREVMONTHNAME1 + dtWDay1.GetMonth() - 1, 100);
		CString strMonth6 = CXTPCalendarUtils::GetLocaleString(LOCALE_SABBREVMONTHNAME1 + dtWDay6.GetMonth() - 1, 100);

		rstrLong = _FormatWCaption(szWDay1, strMonth1, szWDay6, strMonth6, _T(" "), nDateOrdering);
	}
}

CString CXTPCalendarThemeOffice2007::CTOMonthViewWeekHeader::_FormatWCaption(LPCTSTR pcszDay1, LPCTSTR pcszMonth1,
						LPCTSTR pcszDay6, LPCTSTR pcszMonth6,
						LPCTSTR pcszDayMonthSeparator, int nDateOrdering)
{
	//LOCALE_IDATE  -  short date format ordering
	// 0 Month-Day-Year
	// 1 Day-Month-Year
	// 2 Year-Month-Day

	CString strCaption;
	if (_tcsicmp(pcszMonth1, pcszMonth6) == 0)
	{
		if (nDateOrdering == 1)
		{
			strCaption.Format(_T("%s - %s%s%s"), pcszDay1, pcszDay6,
							  pcszDayMonthSeparator, pcszMonth6);
		}
		else
		{
			strCaption.Format(_T("%s%s%s - %s"), pcszMonth1, pcszDayMonthSeparator,
							  pcszDay1, pcszDay6);
		}
	}
	else
	{
		if (nDateOrdering == 1)
		{
			strCaption.Format(_T("%s%s%s - %s%s%s"), pcszDay1, pcszDayMonthSeparator,
							  pcszMonth1, pcszDay6, pcszDayMonthSeparator, pcszMonth6);
		}
		else
		{
			strCaption.Format(_T("%s%s%s - %s%s%s"), pcszMonth1, pcszDayMonthSeparator,
							  pcszDay1, pcszMonth6, pcszDayMonthSeparator, pcszDay6);
		}
	}
	return strCaption;
}

void CXTPCalendarThemeOffice2007::CTOMonthViewDay::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	m_clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_DayBorder));
	//m_clrTodayBorder.SetStandardValue(XTP_CALENDAR_OFFICE2007_TODAYBORDER);

	m_clrBackgroundLight.SetStandardValue(pThemeX->GetColor(xtpCLR_MonthViewDayBkLight));
	m_clrBackgroundDark.SetStandardValue(pThemeX->GetColor(xtpCLR_MonthViewDayBkDark));
	m_clrBackgroundSelected.SetStandardValue(pThemeX->GetColor(xtpCLR_MonthViewDayBkSelected));

	TBase::RefreshMetrics(bRefreshChildren);

	m_clrTodayBorder.SetStandardValue(
		GetTheme()->GetColorsSetPart()->GetColor(xtpCLR_HeaderBorder, GetHeaderPartX()->m_clrTodayBaseColor));
}

void CXTPCalendarThemeOffice2007::CTOMonthViewDay::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthViewDay* pMViewDay = DYNAMIC_DOWNCAST(CXTPCalendarMonthViewDay, pObject);
	if (!pDC || !pThemeX || !pMViewDay || !pMViewDay->GetView() ||
		!pMViewDay->GetView()->GetGrid() || pMViewDay->GetViewGroupsCount() == 0 ||
		!pMViewDay->GetViewGroup(0) || !pThemeX->GetCalendarControl())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarMonthView::CMonthViewGrid* pGrid = pMViewDay->GetView()->GetGrid();
	CXTPCalendarMonthViewGroup* pMViewGroup = pMViewDay->GetViewGroup(0);
	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pMViewDay);

	CRect rcDay = dayLayout.m_rcDay;
	CRect rcBorders = rcDay;
	rcBorders.top = dayLayout.m_rcDayHeader.bottom;

	COleDateTime dtDay = pMViewDay->GetDayDate();
	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();

	BOOL bIsToday = GetHeaderPart()->IsToday(pMViewDay);
	BOOL bIsSelected = GetHeaderPart()->IsSelected(pMViewDay);
	//-----------------
	int nCurrentMonth = dtNow.GetMonth();
	int nLiteBkColorIdx = nCurrentMonth % 2;

	int nMonth = dtDay.GetMonth();
	COLORREF clrBackground = nMonth % 2 == nLiteBkColorIdx ? m_clrBackgroundLight : m_clrBackgroundDark;
	if (bIsSelected)
	{
		clrBackground = m_clrBackgroundSelected;
	}

	pDC->FillSolidRect(rcDay, clrBackground);

	if (bIsToday)
	{
		CXTPCalendarTheme::DrawRectPartly(pDC, rcBorders, m_clrTodayBorder, 2, CRect(1, 0, 1, 1));
		CXTPCalendarTheme::DrawRectPartly(pDC, rcBorders, m_clrBorder, 1, CRect(0, 0, 1, 0));
	}
	else
	{
		int nColIdx = pGrid->GetWeekDayCol(dtDay.GetDayOfWeek());
		int nIsLBorder = nColIdx == 0 ? 1 : 0;

		CXTPCalendarTheme::DrawRectPartly(pDC, rcBorders, m_clrBorder, 1, CRect(nIsLBorder, 0, 1, 0));
		CXTPCalendarTheme::DrawRectPartly(pDC, rcBorders, m_clrBorder, 2, CRect(0, 0, 0, 1));
	}
	//------------------
	GetHeaderPart()->Draw(pMViewDay, pDC);

	//=====================
	BOOL bCnt_EvS = FALSE;
	BOOL bCnt_EvMul = FALSE;

	CXTPPropsStateContext autoStateCnt_EvS;
	CXTPPropsStateContext autoStateCnt_EvMul;

	int nCount = pMViewGroup->GetViewEventsCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewEvent* pViewEvent = pMViewGroup->GetViewEvent_(i);
		ASSERT(pViewEvent);
		if (pViewEvent && pViewEvent->IsVisible())
		{
			if (pViewEvent->IsMultidayEvent())
			{
				if (!bCnt_EvMul)
					autoStateCnt_EvMul.SetData(GetMultiDayEventPartX(), xtpCalendarBeforeDraw_MonthViewEvent, TRUE);
				bCnt_EvMul = TRUE;

				autoStateCnt_EvMul.SendBeforeDrawThemeObject((LPARAM)pViewEvent);

				GetMultiDayEventPartX()->Draw(pViewEvent, pDC);

				autoStateCnt_EvMul.RestoreState();
			}
			else
			{
				if (!bCnt_EvS)
					autoStateCnt_EvS.SetData(GetSingleDayEventPartX(), xtpCalendarBeforeDraw_MonthViewEvent, TRUE);
				bCnt_EvS = TRUE;

				autoStateCnt_EvS.SendBeforeDrawThemeObject((LPARAM)pViewEvent);

				GetSingleDayEventPartX()->Draw(pViewEvent, pDC);

				autoStateCnt_EvS.RestoreState();
			}
		}
	}

	if (!pDC->IsPrinting() && pMViewDay->NoAllEventsAreVisible())
	{
		CRect rc = dayLayout.m_rcDay;
		rc.right -= 5;
		rc.bottom -= 5;

		BOOL bHot = HitTestExpandDayButton(pMViewDay) != 0;

		dayLayout.m_rcExpandSign = pThemeX->DrawExpandDayButton(pDC, rc, FALSE, bHot);

		if (bHot)
			dayLayout.m_nHotState |= CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotExpandButton;
		else
			dayLayout.m_nHotState &= ~CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotExpandButton;
	}

	autoStateCnt_EvS.Clear();
	autoStateCnt_EvMul.Clear();
}

void CXTPCalendarThemeOffice2007::CTOMonthViewDay::OnMouseMove(CCmdTarget* pObject, UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthViewDay* pMViewDay = DYNAMIC_DOWNCAST(CXTPCalendarMonthViewDay, pObject);
	if (!pThemeX || !pMViewDay || !pThemeX->GetCalendarControl())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pMViewDay);

	if (pMViewDay->NoAllEventsAreVisible())
	{
		BOOL bHot = pThemeX->ExpandDayButton_HitTest(point, dayLayout.m_rcExpandSign, FALSE);
		BOOL bHotPrev = !!(dayLayout.m_nHotState & CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotExpandButton);

		if (bHot != bHotPrev)
		{
			CXTPCalendarControl::CUpdateContext updateContext(pThemeX->GetCalendarControl(), xtpCalendarUpdateRedraw);
		}
	}

	BOOL bHotPrev = !!(dayLayout.m_nHotState & CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotDayHeader);
	if (dayLayout.m_rcDayHeader.PtInRect(point) != bHotPrev)
	{
		CXTPCalendarControl::CUpdateContext updateContext(pThemeX->GetCalendarControl(), xtpCalendarUpdateRedraw);
	}
}

BOOL CXTPCalendarThemeOffice2007::CTOMonthViewDay::OnLButtonDown(CCmdTarget* pObject, UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthViewDay* pMViewDay = DYNAMIC_DOWNCAST(CXTPCalendarMonthViewDay, pObject);
	if (!pThemeX || !pMViewDay || !pThemeX->GetCalendarControl())
	{
		ASSERT(FALSE);
		return FALSE;
	}
	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pMViewDay);

	if (pThemeX->ExpandDayButton_HitTest(point, dayLayout.m_rcExpandSign, FALSE))
	{
		pThemeX->GetCalendarControl()->PostMessage(xtp_wm_UserAction,
			(WPARAM)(xtpCalendarUserAction_OnExpandDay | xtpCalendarExpandDayButton_MonthView),
			(LPARAM)(LONG)(DATE)pMViewDay->GetDayDate());

		return TRUE;
	}

	return FALSE;
}

void CXTPCalendarThemeOffice2007::CTOMonthViewDay::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("MonthViewDay")));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("BorderColor"),       m_clrBorder);
	PX_Color(&secData, _T("TodayBorderColor"),  m_clrTodayBorder);

	PX_Color(&secData, _T("BackgroundLightColor"),   m_clrBackgroundLight);
	PX_Color(&secData, _T("BackgroundDarkColor"),    m_clrBackgroundDark);
	PX_Color(&secData, _T("BackgroundSelectedColor"), m_clrBackgroundSelected);

	TBase::DoPropExchange(&secData);
}

void CXTPCalendarThemeOffice2007::CTOMonthViewDay::Serialize(CArchive& ar)
{
	m_clrBorder.Serialize(ar);
	m_clrTodayBorder.Serialize(ar);

	m_clrBackgroundLight.Serialize(ar);
	m_clrBackgroundDark.Serialize(ar);
	m_clrBackgroundSelected.Serialize(ar);

	TBase::Serialize(ar);
}

void CXTPCalendarThemeOffice2007::CTOMonthViewDayHeader::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	RefreshFromParent(pThemeX->GetMonthViewPartX()->GetHeaderPartX());

	m_clrTodayBaseColor.SetStandardValue(pThemeX->GetHeaderPartX()->m_clrTodayBaseColor);
	m_bkNormal.nBitmapID.SetStandardValue(xtpIMG_MonthViewDayHeader);
	m_bkSelected.nBitmapID.SetStandardValue(xtpIMG_MonthViewDayHeader);
	m_bkToday.nBitmapID.SetStandardValue(xtpIMG_MonthViewDayHeaderToday);
	m_bkTodaySelected.nBitmapID.SetStandardValue(xtpIMG_MonthViewDayHeaderToday);

	m_bkToday.rcBitmapBorder.        SetStandardValue(CRect(1, 1, 2, 1));
	m_bkTodaySelected.rcBitmapBorder.SetStandardValue(CRect(1, 1, 2, 1));
}

void CXTPCalendarThemeOffice2007::CTOMonthViewDayHeader::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthViewDay* pMViewDay = DYNAMIC_DOWNCAST(CXTPCalendarMonthViewDay, pObject);
	if (!pDC || !pThemeX || !pMViewDay || !pThemeX->GetCalendarControl() ||
		!pMViewDay->GetView() || !pMViewDay->GetView()->GetGrid())
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pMViewDay);
	COleDateTime dtDay = pMViewDay->GetDayDate();
	BOOL bCompressWeD = XTP_SAFE_GET3(pMViewDay, GetView(), GetCalendarControl(), MonthView_IsCompressWeekendDays(), FALSE);

	BOOL bIsToday    = IsToday(pMViewDay);
	BOOL bIsSelected = IsSelected(pMViewDay);

	int nDImod7 = pMViewDay->GetDayIndex() % 7;
	BOOL bFirstDay = nDImod7 == 0 || (nDImod7 == 6 && bCompressWeD);
	BOOL bLastDay = nDImod7 >= (bCompressWeD ? 5 : 6);

	CPoint pt(0, 0);
	if (GetCursorPos(&pt))
		pThemeX->GetCalendarControl()->ScreenToClient(&pt);

	int nFlags = bIsToday ? CTOHeader::xtpCalendarItemState_Today : CTOHeader::xtpCalendarItemState_Normal;
	nFlags |= bIsSelected ? CTOHeader::xtpCalendarItemState_Selected : 0;
	if (dayLayout.m_rcDayHeader.PtInRect(pt))
	{
		nFlags |=  CTOHeader::xtpCalendarItemState_Hot;
		dayLayout.m_nHotState |= CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotDayHeader;
	}
	else
	{
		dayLayout.m_nHotState &= ~CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotDayHeader;
	}

	nFlags |= bFirstDay ? CTOHeader::xtpCalendarItemFirst : 0;
	nFlags |= bLastDay ? CTOHeader::xtpCalendarItemLast : 0;

	CRect rcRect = dayLayout.m_rcDay;

	//***
	rcRect.bottom = dayLayout.m_rcDayHeader.bottom;

	CString strTextLeft;
	CString strTextCenter;
	CString strTextRight;

	int nMonth = dtDay.GetMonth();
	int nDay = dtDay.GetDay();

	if (nDay == 1 || pMViewDay->GetDayIndex() == 0)
	{
		CString strMonth = CXTPCalendarUtils::GetLocaleString(LOCALE_SABBREVMONTHNAME1 + nMonth - 1, 100);

		//LOCALE_IDATE  -  short date format ordering
		// 0 Month-Day-Year
		// 1 Day-Month-Year
		// 2 Year-Month-Day
		int nDateOrdering = CXTPCalendarUtils::GetLocaleLong(LOCALE_IDATE);

		if (nDateOrdering == 1)
		{
			strTextLeft.Format(_T("%d %s"), nDay, (LPCTSTR)strMonth);
		}
		else
		{
			strTextLeft.Format(_T("%s %d"), (LPCTSTR)strMonth, nDay);
		}
	}
	else
	{
		strTextLeft.Format(_T("%d"), nDay);
	}

	//------------------------------------------------------
	pThemeX->GetItemTextIfNeed(xtpCalendarItemText_MonthViewDayHeaderLeft, &strTextLeft, pMViewDay);
	pThemeX->GetItemTextIfNeed(xtpCalendarItemText_MonthViewDayHeaderCenter, &strTextCenter, pMViewDay);
	pThemeX->GetItemTextIfNeed(xtpCalendarItemText_MonthViewDayHeaderRight, &strTextRight, pMViewDay);
	//------------------------------------------------------

	Draw_Header(pDC, rcRect, nFlags, strTextLeft, strTextCenter, strTextRight);
}

void CXTPCalendarThemeOffice2007::CTOMonthViewEvent_MultiDay::Draw_Time(CDC* pDC, const CRect& rcEventRect,
																		CXTPCalendarMonthViewEvent* pViewEvent)
{
	if (!pDC || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bSelected = pViewEvent->IsSelected();

	CFont* pFont = bSelected ? m_fcsetSelected.fcsetStartEnd.Font() : m_fcsetNormal.fcsetStartEnd.Font();
	COLORREF clrColor = bSelected ? m_fcsetSelected.fcsetStartEnd.Color() : m_fcsetNormal.fcsetStartEnd.Color();

	CXTPFontDC autoFont(pDC, pFont, clrColor);

	//--------------------------
	COleDateTime dtStart(pViewEvent->GetEvent()->GetStartTime());
	COleDateTime dtEnd(pViewEvent->GetEvent()->GetEndTime());

	//*****************
	// not implemented for office 2007 theme.
	//if (pViewEvent->IsTimeAsClock()) {}
	//*****************

	CString strStartTime = pViewEvent->FormatEventTime(dtStart);
	CString strEndTime = pViewEvent->FormatEventTime(dtEnd);

	CSize szStart = pDC->GetTextExtent(strStartTime);
	CSize szEnd = pDC->GetTextExtent(strEndTime);

	CRect rcStart(rcEventRect);
	CRect rcEnd(rcEventRect);

	BOOL bDrawStartEndTimes = szStart.cx + szEnd.cx + 6 * 2 < rcEventRect.Width()/2;

	if (!bDrawStartEndTimes)
	{
		return;
	}

	rcStart.left = min(rcStart.left + 6, rcStart.right);
	rcStart.right = min(rcStart.left + szStart.cx + 2, rcStart.right);

	rcEnd.right = max(rcEnd.right - 6, rcEventRect.left);
	rcEnd.left = max(rcEnd.right - szEnd.cx - 2, rcEventRect.left);

	//-------------------------------------------------
	UINT uAlign = DT_VCENTER | DT_SINGLELINE ;
	pDC->DrawText(strStartTime, &rcStart, uAlign | DT_LEFT);
	pDC->DrawText(strEndTime, &rcEnd, uAlign | DT_RIGHT);

	//-------------------------------------------------
	pViewEvent->m_rcStartTime.CopyRect(&rcStart);
	pViewEvent->m_rcEndTime.CopyRect(&rcEnd);

	pViewEvent->m_rcText.left = rcStart.right;
	pViewEvent->m_rcText.right = rcEnd.left;
}

void CXTPCalendarThemeOffice2007::CTOMonthViewEvent_MultiDay::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthViewEvent* pViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarMonthViewEvent, pObject);

	CXTPCalendarMonthViewDay* pMVDay = XTP_SAFE_GET2(pViewEvent, GetViewGroup(), GetViewDay(), NULL);
	CXTPCalendarMonthView* pMView = XTP_SAFE_GET1(pMVDay, GetView(), NULL);

	if (!pDC || !pThemeX || !pViewEvent || !pViewEvent->GetEvent() ||
		!pMView || !pMVDay)
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bNoVisibleMDEvent = pViewEvent->IsMultidayEvent() && !pViewEvent->IsVisible();

	int nMDEFlags = pViewEvent->GetMultiDayEventFlags();
	if ((nMDEFlags & xtpCalendarMultiDaySlave) || bNoVisibleMDEvent)
	{
		return;// was drawn in the Master (or out of events rect)
	}

	CRect rcEventMax(pViewEvent->GetViewEventRectMax());
	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return;
	}

	// fill event background
	BOOL bSelected = pViewEvent->IsSelected();
	COLORREF clrBorderNotSel;
	COLORREF clrBorder;
	CXTPPaintManagerColorGradient grclrBk;

	GetEventColors(pViewEvent, clrBorder, clrBorderNotSel, grclrBk);

	//----------------
	pThemeX->DrawRoundRect(pDC, rcEvent, clrBorder, bSelected, grclrBk);

	int nSelOffset = bSelected ? 2 : 1;

	if (nMDEFlags & xtpCalendarMultiDayFirst)
		pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcEvent.left + nSelOffset, rcEvent.top + nSelOffset),
						  rcEvent.right - nSelOffset, grclrBk, cnCornerSize / 2, rcEvent.bottom);

	//------------------
	int nViewDayIndex = pMVDay->GetDayIndex();

	BOOL bCompressWD = XTP_SAFE_GET2(pViewEvent, GetCalendarControl(), MonthView_IsCompressWeekendDays(), FALSE);
	int nLastDayDx = bCompressWD ? 1 : 7;

	BOOL bFirstOnScreen = nViewDayIndex == 0;
	BOOL bLastOnScreen = nViewDayIndex == (pMView->GetViewDayCount() - nLastDayDx);

	CRect rcText(pViewEvent->GetTextRect());

	if ((nMDEFlags & xtpCalendarMultiDayFirst) == 0)
	{
		CRect rcSQ = rcEventMax;
		rcSQ.right = min(rcEvent.left + cnCornerSize, rcEventMax.right);
		CXTPCalendarTheme::DrawRectPartly(pDC, rcSQ, clrBorder, bSelected, grclrBk, CRect(1,1,0,1));

		pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcEvent.left + nSelOffset, rcEvent.top + nSelOffset),
						  rcEvent.right - nSelOffset, grclrBk, 0, rcEvent.bottom - nSelOffset);
		//-------------------------------------------------------------------
		if (bFirstOnScreen)
		{
			CRect rcArrow;
			BOOL bArrow = Draw_ArrowL(pViewEvent, pDC, rcArrow);
			if (bArrow)
				rcText.left = rcArrow.right;
		}
	}
	else if (bSelected)
	{
		// draw left Gripper
		CRect rcGripper;
		rcGripper.top = rcEvent.top + rcEvent.Height() / 2 - cnGripperSize / 2;
		rcGripper.bottom = rcGripper.top + cnGripperSize;
		rcGripper.left = rcEvent.left - 1;
		rcGripper.right = rcGripper.left + cnGripperSize;

		pDC->FillSolidRect(&rcGripper, m_clrGripperBackground);
		CBrush brBorder(m_clrGripperBorder);
		pDC->FrameRect(&rcGripper, &brBorder);
	}


	if ((nMDEFlags & xtpCalendarMultiDayLast) == 0)
	{
		CRect rcSQ = rcEventMax;
		rcSQ.left = max(rcEvent.right - cnCornerSize, rcEventMax.left);
		CXTPCalendarTheme::DrawRectPartly(pDC, rcSQ, clrBorder, bSelected, grclrBk, CRect(0,1,1,1));

		pThemeX->_DrawEventLTHighlight(pDC, CPoint(rcSQ.left, rcEvent.top + nSelOffset),
						  rcEventMax.right - nSelOffset, grclrBk, 0, INT_MAX, FALSE);
		//-------------------------------------------------------------------
		if (bLastOnScreen)
		{
			CRect rcArrow;
			BOOL bArrow = Draw_ArrowR(pViewEvent, pDC, rcArrow);
			if (bArrow)
				rcText.right = rcArrow.left;
		}
	}
	else if (bSelected)
	{
		// draw right gripper
		CRect rcGripper;
		rcGripper.top = rcEvent.top + rcEvent.Height() / 2 - cnGripperSize / 2;
		rcGripper.bottom = rcGripper.top + cnGripperSize;
		rcGripper.right = rcEvent.right + 1;
		rcGripper.left = rcGripper.right - cnGripperSize;

		pDC->FillSolidRect(&rcGripper, m_clrGripperBackground);
		CBrush brBorder(m_clrGripperBorder);
		pDC->FrameRect(&rcGripper, &brBorder);
	}

	int nMDFirstLastMask = (xtpCalendarMultiDayFirst|xtpCalendarMultiDayLast);
	if ((nMDEFlags & nMDFirstLastMask) == nMDFirstLastMask)
	{
		COleDateTime dtStart = pViewEvent->GetEvent()->GetStartTime();
		COleDateTime dtEnd = pViewEvent->GetEvent()->GetEndTime();

		if (!CXTPCalendarUtils::IsZeroTime(dtStart) && !CXTPCalendarUtils::IsZeroTime(dtEnd))
		{
			Draw_Time(pDC, rcEvent, pViewEvent);
			rcText = pViewEvent->GetTextRect();
		}
	}

	//****
	pViewEvent->m_rcText.CopyRect(rcText);

	//=====================================
	CString strSubj = pViewEvent->GetItemTextSubject();
	CString strLoc = pViewEvent->GetItemTextLocation();

	if (!strLoc.IsEmpty())
	{
		strSubj += _T(";");
	}

	CThemeFontColorSet* pFntClr1 = bSelected ? &m_fcsetSelected.fcsetSubject : &m_fcsetNormal.fcsetSubject;
	CThemeFontColorSet* pFntClr2 = bSelected ? &m_fcsetSelected.fcsetLocation : &m_fcsetNormal.fcsetLocation;

	rcText.DeflateRect(3, 0, 3, 0);

	pViewEvent->m_rcSubjectEditor.CopyRect(rcText);
	pViewEvent->m_rcSubjectEditor.top = rcEvent.top + 2;
	pViewEvent->m_rcSubjectEditor.bottom = rcEvent.bottom - 2;
	pViewEvent->AdjustSubjectEditor();

	//----------------------------------------------------
	UINT uTextShortAlign = DT_VCENTER | DT_LEFT;
	CSize szText = pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
								rcText, uTextShortAlign | DT_CALCRECT);

	CRect rcIconsMax = rcText;
	rcIconsMax.left = min(rcIconsMax.left + rcText.Height(), rcIconsMax.right);

	//--- calculate rects and fix to be centered if enough space ---
	CSize szIcons = Draw_Icons(pViewEvent, pDC, rcIconsMax, TRUE);

	if (rcIconsMax.Height() > szIcons.cy)
	{
		rcIconsMax.bottom -= (rcIconsMax.Height() - szIcons.cy) / 2;
	}

	int nTIcx = szText.cx + szIcons.cx;
	if (nTIcx + 5 < rcText.Width())
	{
		int ncx_2 = (rcText.Width() - nTIcx) / 2 - 2;
		rcText.left += ncx_2;
		rcText.right = rcText.left + szText.cx + 2;

		rcIconsMax.right -= ncx_2;
	}
	else
	{
		rcText.right -= szIcons.cx + 2;
	}

	pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
		rcText, uTextShortAlign);

	Draw_Icons(pViewEvent, pDC, rcIconsMax);
}

void CXTPCalendarThemeOffice2007::CTOMonthViewEvent_SingleDay::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarMonthViewEvent* pViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarMonthViewEvent, pObject);
	if (!pDC || !pThemeX || !pViewEvent)
	{
		ASSERT(FALSE);
		return;
	}

	if (!pViewEvent->IsVisible())
	{
		return;
	}

	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return;
	}

	// fill event background
	Draw_Background(pDC, rcEvent, pViewEvent);

	Draw_Time(pDC, rcEvent, pViewEvent);

	CRect rcText(pViewEvent->GetTextRect());
	Draw_Caption(pDC, rcText, pViewEvent);
}

void CXTPCalendarThemeOffice2007::CTOMonthViewEvent_SingleDay::
		Draw_Background(CDC* pDC, const CRect& rcEventRect, CXTPCalendarMonthViewEvent* pViewEvent)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());

	if (!pDC || !pThemeX || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return;
	}
	BOOL bSelected = pViewEvent->IsSelected();

	COLORREF clrBorderNotSel, clrBorder;
	CXTPPaintManagerColorGradient grclrBk;
	GetEventColors(pViewEvent, clrBorder, clrBorderNotSel, grclrBk);

	CXTPCalendarEventLabel* ptrLabel = pViewEvent->GetEvent()->GetLabel();
	if (!bSelected && ptrLabel && ptrLabel->m_nLabelID != XTP_CALENDAR_NONE_LABEL_ID)
	{
		clrBorder = grclrBk.clrLight;
	}

	pThemeX->DrawRoundRect(pDC, rcEventRect, clrBorder, bSelected, grclrBk);
}

CSize CXTPCalendarThemeOffice2007::CTOMonthViewEvent_SingleDay::Draw_Time(
		CDC* pDC, const CRect& rcEventRect, CXTPCalendarMonthViewEvent* pViewEvent)
{
	CSize szResult(0, 0);

	if (!pDC || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return szResult;
	}

	BOOL bSelected = pViewEvent->IsSelected();

	CFont* pFont = bSelected ? m_fcsetSelected.fcsetStartEnd.Font() : m_fcsetNormal.fcsetStartEnd.Font();
	COLORREF clrColor = bSelected ? m_fcsetSelected.fcsetStartEnd.Color() : m_fcsetNormal.fcsetStartEnd.Color();

	CXTPFontDC autoFont(pDC, pFont, clrColor);

	//--------------------------
	COleDateTime dtStart(pViewEvent->GetEvent()->GetStartTime());
	COleDateTime dtEnd(pViewEvent->GetEvent()->GetEndTime());

	//*****************
	// not implemented for office 2007 theme.
	//if (pViewEvent->IsTimeAsClock()) {}
	//*****************

	CString strStartTime = pViewEvent->FormatEventTime(dtStart);
	CString strEndTime = pViewEvent->FormatEventTime(dtEnd);

	CSize szStart = pDC->GetTextExtent(strStartTime);
	CSize szEnd = pDC->GetTextExtent(strEndTime);

	CRect rcStart(rcEventRect);
	CRect rcEnd(rcEventRect);

	BOOL bShowEndTime = XTP_SAFE_GET2(pViewEvent, GetCalendarControl(), MonthView_IsShowEndDate(), TRUE);
	BOOL bDrawStartTime = szStart.cx + 6 < rcEventRect.Width()/2;
	BOOL bDrawEndTime = bShowEndTime && szStart.cx + szEnd.cx + 6 * 2 < rcEventRect.Width()/2;

	if (!bDrawStartTime && !bDrawEndTime)
	{
		return szResult;
	}

	rcStart.left = min(rcStart.left + 4, rcStart.right);
	rcStart.right = min(rcStart.left + szStart.cx + 6, rcStart.right);

	rcEnd.left = rcStart.right;
	rcEnd.right = min(rcEnd.left + szEnd.cx + 6, rcEnd.right);
	if (!bDrawEndTime)
	{
		rcEnd.right = rcEnd.left;
	}

	//-------------------------------------------------
	UINT uAlign = DT_VCENTER | DT_SINGLELINE | DT_LEFT;
	pDC->DrawText(strStartTime, &rcStart, uAlign);

	if (bDrawEndTime)
	{
		pDC->DrawText(strEndTime, &rcEnd, uAlign);
	}

	//-------------------------------------------------
	pViewEvent->m_rcStartTime.CopyRect(&rcStart);
	pViewEvent->m_rcEndTime.CopyRect(&rcEnd);

	pViewEvent->m_rcText.left = rcEnd.right;

	//-------------------------------------------------
	szResult.cx = max(0, rcEnd.right - rcStart.left);
	szResult.cy = szStart.cy;

	return szResult;
}

void CXTPCalendarThemeOffice2007::CTOMonthViewEvent_SingleDay::Draw_Caption(
		CDC* pDC, const CRect& rcTextRect, CXTPCalendarMonthViewEvent* pViewEvent)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	if (!pDC || !pViewEvent || !pThemeX )
	{
		ASSERT(FALSE);
		return;
	}

	CString strSubj = pViewEvent->GetItemTextSubject();
	CString strLoc = pViewEvent->GetItemTextLocation();

	if (!strLoc.IsEmpty())
	{
		strSubj += _T(";");
	}

	BOOL bSelected = pViewEvent->IsSelected();
	CThemeFontColorSet* pFntClr1 = bSelected ? &m_fcsetSelected.fcsetSubject : &m_fcsetNormal.fcsetSubject;
	CThemeFontColorSet* pFntClr2 = bSelected ? &m_fcsetSelected.fcsetLocation : &m_fcsetNormal.fcsetLocation;

	CRect rcText = rcTextRect;
	//rcText.DeflateRect(1, 2, 3, 2);

	pViewEvent->m_rcSubjectEditor.CopyRect(rcText);
	pViewEvent->AdjustSubjectEditor();

	UINT nAlign = DT_VCENTER | DT_LEFT | DT_SINGLELINE;
	CSize szText = pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
								rcText, nAlign  | DT_CALCRECT, nAlign);

	//----------------------------------------------------
	CRect rcIconsMax = rcText;
	rcIconsMax.left = min(rcIconsMax.left + rcTextRect.Height(), rcIconsMax.right);

	//--- calculate rects and fix to be centered if enough space ---
	CSize szIcons = Draw_Icons(pViewEvent, pDC, rcIconsMax, TRUE);

	if (rcIconsMax.Height() > szIcons.cy)
	{
		rcIconsMax.bottom -= (rcIconsMax.Height() - szIcons.cy) / 2;
	}

	int nTIcx = szText.cx + szIcons.cx;
	if (nTIcx > rcText.Width())
	{
		rcText.right = max(rcIconsMax.left, rcIconsMax.right - szIcons.cx) - 2;
	}

	pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
								rcText, nAlign, nAlign);

	Draw_Icons(pViewEvent, pDC, rcIconsMax);
}

/////////////////////////////////////////////////////////////////////////////
//
// ***** Week View *****
//

void CXTPCalendarThemeOffice2007::CTOWeekView::RefreshMetrics(BOOL bRefreshChildren)
{
	TBase::RefreshMetrics(bRefreshChildren);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewEvent::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	TBase::RefreshMetrics(bRefreshChildren);

	RefreshFromParent(pThemeX->GetEventPartX());

	XTP_SAFE_CALL1(GetHeightFormulaPart(), SetStandardValue(14, 10, 1));
	XTP_SAFE_CALL1(GetEventIconsToDrawPart(), SetStandardValue(FALSE, FALSE, FALSE, FALSE, FALSE) );
}

void CXTPCalendarThemeOffice2007::CTOWeekViewEvent_MultiDay::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	RefreshFromParent(pThemeX->GetWeekViewPartX()->GetEventPartX());

	m_fcsetNormal.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventBorder));
	m_fcsetNormal.grclrBackground.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventBkGRfrom),
												   pThemeX->GetColor(xtpCLR_MultiDayEventBkGRto));

	m_fcsetSelected.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_MultiDayEventSelectedBorder));
	m_fcsetSelected.grclrBackground.SetStandardValue(m_fcsetNormal.grclrBackground);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewEvent_SingleDay::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	RefreshFromParent(pThemeX->GetWeekViewPartX()->GetEventPartX());
	CFont* pFontDef = pThemeX->m_fntBaseFont;

	m_fcsetNormal.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_WeekViewSingleDayEventBorder));
	m_fcsetNormal.grclrBackground.SetStandardValue(pThemeX->GetColor(xtpCLR_WeekViewSingleDayEventBkGRfrom),
												   pThemeX->GetColor(xtpCLR_WeekViewSingleDayEventBkGRto) );

	m_fcsetSelected.clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_WeekViewSingleDayEventSelectedBorder));
	m_fcsetSelected.grclrBackground.SetStandardValue(pThemeX->GetColor(xtpCLR_WeekViewSingleDayEventBkGRfrom),
													 pThemeX->GetColor(xtpCLR_WeekViewSingleDayEventBkGRto) );

	//----------------------------------------------
	pThemeX->RemoveBoldAttrFromStd(m_fcsetNormal.fcsetSubject.fntFont);
	pThemeX->RemoveBoldAttrFromStd(m_fcsetSelected.fcsetSubject.fntFont);

	//----------------------------------------------
	m_fcsetNormal.fcsetStartEnd.clrColor.SetStandardValue(pThemeX->GetColor(xtpCLR_WeekViewEventTime));
	m_fcsetNormal.fcsetStartEnd.fntFont.SetStandardValue(pFontDef);

	m_fcsetSelected.fcsetStartEnd.clrColor.SetStandardValue(pThemeX->GetColor(xtpCLR_WeekViewEventTime));
	m_fcsetSelected.fcsetStartEnd.fntFont.SetStandardValue(pFontDef);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewDay::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	m_clrBorder.SetStandardValue(pThemeX->GetColor(xtpCLR_DayBorder));
	//m_clrTodayBorder.SetStandardValue(XTP_CALENDAR_OFFICE2007_TODAYBORDER);

	m_clrBackgroundLight.SetStandardValue(pThemeX->GetColor(xtpCLR_WeekViewDayBkLight));
	m_clrBackgroundDark.SetStandardValue(pThemeX->GetColor(xtpCLR_WeekViewDayBkDark));
	m_clrBackgroundSelected.SetStandardValue(pThemeX->GetColor(xtpCLR_WeekViewDayBkSelected));

	TBase::RefreshMetrics(bRefreshChildren);

	m_clrTodayBorder.SetStandardValue(
		GetTheme()->GetColorsSetPart()->GetColor(xtpCLR_HeaderBorder, GetHeaderPartX()->m_clrTodayBaseColor));
}

void CXTPCalendarThemeOffice2007::CTOWeekViewDayHeader::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	ASSERT(pThemeX);
	if (!pThemeX)
	{
		return;
	}

	//LOCALE_IDATE  -  short date format ordering
	// 0 Month-Day-Year
	// 1 Day-Month-Year
	// 2 Year-Month-Day
	int nDateOrdering = CXTPCalendarUtils::GetLocaleLong(LOCALE_IDATE);

	if (nDateOrdering == 1)
	{
		m_strHeaderFormat.SetStandardValue(_T("dddd, dd MMMM"));
	}
	else
	{
		m_strHeaderFormat.SetStandardValue(_T("dddd, MMMM dd"));
	}

	RefreshFromParent(pThemeX->GetHeaderPartX());

	m_clrTodayBaseColor.SetStandardValue(pThemeX->GetHeaderPartX()->m_clrTodayBaseColor);
	m_bkNormal.nBitmapID.SetStandardValue(xtpIMG_WeekViewDayHeader);
	m_bkSelected.nBitmapID.SetStandardValue(xtpIMG_WeekViewDayHeader);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewEvent::AdjustLayout(CCmdTarget* pObject,
																  CDC* pDC, const CRect& rcRect, int nEventPlaceNumber)
{
	CXTPCalendarWeekViewEvent* pViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarWeekViewEvent, pObject);

	if (!pDC || !GetTheme() || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return;
	}

	int nEventHeight = XTP_SAFE_GET4(pViewEvent, GetViewGroup(), GetViewDay(), GetView(), GetRowHeight(), 0);

	pViewEvent->m_rcEventMax.top = rcRect.top + nEventPlaceNumber * nEventHeight;
	pViewEvent->m_rcEventMax.bottom = pViewEvent->m_rcEventMax.top + nEventHeight;

	pViewEvent->m_rcEvent.top = pViewEvent->m_rcEventMax.top;
	pViewEvent->m_rcEvent.bottom = pViewEvent->m_rcEventMax.bottom;
	pViewEvent->m_rcEvent.left = pViewEvent->m_rcEventMax.left + 4 + 2;
	pViewEvent->m_rcEvent.right = pViewEvent->m_rcEventMax.right - 5 - 2;

	pViewEvent->m_rcToolTip.CopyRect(pViewEvent->m_rcEventMax);

	// adjust subject area
	int nDyx = pViewEvent->IsSelected() ? 5 : 2;

	pViewEvent->m_rcText.CopyRect(pViewEvent->m_rcEvent);
	pViewEvent->m_rcText.DeflateRect(nDyx + 3, 3, nDyx + 2, 3);

	// set times rects
	pViewEvent->m_rcStartTime.SetRectEmpty();
	pViewEvent->m_rcEndTime.SetRectEmpty();

	pViewEvent->m_rcSubjectEditor.CopyRect(pViewEvent->m_rcEvent);
	pViewEvent->m_rcSubjectEditor.top++;
	pViewEvent->m_rcSubjectEditor.bottom--;
}

void CXTPCalendarThemeOffice2007::CTOWeekViewDay::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarWeekViewDay* pWViewDay = DYNAMIC_DOWNCAST(CXTPCalendarWeekViewDay, pObject);
	if (!pDC || !pThemeX || !pWViewDay || !pWViewDay->GetView() ||
		pWViewDay->GetViewGroupsCount() == 0 ||
		!pWViewDay->GetViewGroup(0))
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarWeekViewGroup* pWViewGroup = pWViewDay->GetViewGroup(0);
	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pWViewDay);

	CRect rcDay = dayLayout.m_rcDay;
	CRect rcBorders = rcDay;
	rcBorders.top = dayLayout.m_rcDayHeader.bottom;

	COleDateTime dtDay = pWViewDay->GetDayDate();
	int nDayCol = GetDayCol(pWViewDay);

	BOOL bIsToday = GetHeaderPart()->IsToday(pWViewDay);
	BOOL bIsSelected = GetHeaderPart()->IsSelected(pWViewDay);

	// Draw background
	int nLiteBkColorIdx = 0;
	// for non-work days choose dark background
	if (pWViewDay->GetCalendarControl() &&
		pWViewDay->GetCalendarControl()->GetCalendarOptions())
	{
		int nMask = pWViewDay->GetCalendarControl()->GetCalendarOptions()->nWorkWeekMask;
		int nWeekDay = dtDay.GetDayOfWeek();
		if ((CXTPCalendarUtils::GetDayOfWeekMask(nWeekDay) & nMask) != 0)
		{
			nLiteBkColorIdx = 1;
		}
	}
	// for selected days choose another background
	COLORREF clrBackground = nLiteBkColorIdx ? m_clrBackgroundLight : m_clrBackgroundDark;
	if (bIsSelected)
	{
		clrBackground = m_clrBackgroundSelected;
	}

	// draw background
	pDC->FillSolidRect(rcDay, clrBackground);

	if (bIsToday)
	{
		CXTPCalendarTheme::DrawRectPartly(pDC, rcBorders, m_clrTodayBorder, 2, CRect(1, 0, 0, 1));
		CXTPCalendarTheme::DrawRectPartly(pDC, rcBorders, m_clrTodayBorder, 1, CRect(0, 0, 1, 0));
		//CXTPCalendarTheme::DrawRectPartly(pDC, rcBorders, m_clrBorder, 1, CRect(0, 0, 1, 0));
	}
	else
	{
		int nIsRBorder = nDayCol;
		int nIsLBorder = nDayCol;

		CXTPCalendarTheme::DrawRectPartly(pDC, rcBorders, m_clrBorder, 1, CRect(nIsLBorder, 0, nIsRBorder, 0));
		CXTPCalendarTheme::DrawRectPartly(pDC, rcBorders, m_clrBorder, 2, CRect(0, 0, 0, 1));
	}
	//------------------
	GetHeaderPart()->Draw(pWViewDay, pDC);

	BOOL bCnt_EvS = FALSE;
	BOOL bCnt_EvMul = FALSE;

	CXTPPropsStateContext autoStateCnt_EvS;
	CXTPPropsStateContext autoStateCnt_EvMul;

	//=====================
	int nCount = pWViewGroup->GetViewEventsCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewEvent* pViewEvent = pWViewGroup->GetViewEvent_(i);
		ASSERT(pViewEvent);
		if (pViewEvent && pViewEvent->IsVisible())
		{
			if (pViewEvent->IsMultidayEvent())
			{
				if (!bCnt_EvMul)
					autoStateCnt_EvMul.SetData(GetMultiDayEventPartX(), xtpCalendarBeforeDraw_WeekViewEvent, TRUE);
				bCnt_EvMul = TRUE;

				autoStateCnt_EvMul.SendBeforeDrawThemeObject((LPARAM)pViewEvent);

				GetMultiDayEventPartX()->Draw(pViewEvent, pDC);

				autoStateCnt_EvMul.RestoreState();
			}
			else
			{
				if (!bCnt_EvS)
					autoStateCnt_EvS.SetData(GetSingleDayEventPartX(), xtpCalendarBeforeDraw_WeekViewEvent, TRUE);
				bCnt_EvS = TRUE;

				autoStateCnt_EvS.SendBeforeDrawThemeObject((LPARAM)pViewEvent);

				GetSingleDayEventPartX()->Draw(pViewEvent, pDC);

				autoStateCnt_EvS.RestoreState();
			}
		}
	}

	if (!pDC->IsPrinting() && pWViewDay->NoAllEventsAreVisible())
	{
		CRect rc = dayLayout.m_rcDay;
		rc.right -= 5;
		rc.bottom -= 5;

		BOOL bHot = _HitTestExpandDayButton(pWViewDay, rc) != 0;

		dayLayout.m_rcExpandSign = pThemeX->DrawExpandDayButton(pDC, rc, FALSE, bHot);
		if (bHot)
			dayLayout.m_nHotState |= CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotExpandButton;
		else
			dayLayout.m_nHotState &= ~CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotExpandButton;
	}

	autoStateCnt_EvS.Clear();
	autoStateCnt_EvMul.Clear();
}

void CXTPCalendarThemeOffice2007::CTOWeekViewDay::OnMouseMove(CCmdTarget* pObject, UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarWeekViewDay* pWViewDay = DYNAMIC_DOWNCAST(CXTPCalendarWeekViewDay, pObject);
	if (!pThemeX || !pWViewDay || !pThemeX->GetCalendarControl())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pWViewDay);

	if (pWViewDay->NoAllEventsAreVisible())
	{
		BOOL bHot = pThemeX->ExpandDayButton_HitTest(point, dayLayout.m_rcExpandSign, FALSE);
		BOOL bHotPrev = !!(dayLayout.m_nHotState & CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotExpandButton);

		if (bHot != bHotPrev)
		{
			CXTPCalendarControl::CUpdateContext updateContext(pThemeX->GetCalendarControl(), xtpCalendarUpdateRedraw);
		}
	}

	BOOL bHotPrev = !!(dayLayout.m_nHotState & CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotDayHeader);
	if (dayLayout.m_rcDayHeader.PtInRect(point) != bHotPrev)
	{
		CXTPCalendarControl::CUpdateContext updateContext(pThemeX->GetCalendarControl(), xtpCalendarUpdateRedraw);
	}
}

BOOL CXTPCalendarThemeOffice2007::CTOWeekViewDay::OnLButtonDown(CCmdTarget* pObject, UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);

	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarViewDay* pViewDay = DYNAMIC_DOWNCAST(CXTPCalendarViewDay, pObject);
	if (!pThemeX || !pViewDay || !pThemeX->GetCalendarControl())
	{
		ASSERT(FALSE);
		return FALSE;
	}
	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pViewDay);

	if (pThemeX->ExpandDayButton_HitTest(point, dayLayout.m_rcExpandSign, FALSE))
	{
		pThemeX->GetCalendarControl()->PostMessage(xtp_wm_UserAction,
			(WPARAM)(xtpCalendarUserAction_OnExpandDay | xtpCalendarExpandDayButton_WeekView),
			(LPARAM)(LONG)(DATE)pViewDay->GetDayDate());

		return TRUE;
	}

	return FALSE;
}

void CXTPCalendarThemeOffice2007::CTOWeekViewDay::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("WeekViewDay")));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("BorderColor"),       m_clrBorder);
	PX_Color(&secData, _T("TodayBorderColor"),  m_clrTodayBorder);

	PX_Color(&secData, _T("BackgroundLightColor"),   m_clrBackgroundLight);
	PX_Color(&secData, _T("BackgroundDarkColor"),    m_clrBackgroundDark);
	PX_Color(&secData, _T("BackgroundSelectedColor"), m_clrBackgroundSelected);

	TBase::DoPropExchange(&secData);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewDay::Serialize(CArchive& ar)
{
	m_clrBorder.Serialize(ar);
	m_clrTodayBorder.Serialize(ar);

	m_clrBackgroundLight.Serialize(ar);
	m_clrBackgroundDark.Serialize(ar);
	m_clrBackgroundSelected.Serialize(ar);

	TBase::Serialize(ar);
}


void CXTPCalendarThemeOffice2007::CTOWeekViewDayHeader::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarWeekViewDay* pWViewDay = DYNAMIC_DOWNCAST(CXTPCalendarWeekViewDay, pObject);
	if (!pDC || !pThemeX || !pWViewDay || !pWViewDay->GetView())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pWViewDay);
	COleDateTime dtDay = pWViewDay->GetDayDate();
	int nDayCol = pThemeX->GetWeekViewPart()->GetDayPart()->GetDayCol(pWViewDay);

	CPoint pt(0, 0);
	if (GetCursorPos(&pt))
		pThemeX->GetCalendarControl()->ScreenToClient(&pt);

	BOOL bIsToday    = IsToday(pWViewDay);
	BOOL bIsSelected = IsSelected(pWViewDay);
	BOOL bFirstDay = nDayCol == 0;
	BOOL bLastDay = nDayCol == 1;

	int nFlags = bIsToday ? CTOHeader::xtpCalendarItemState_Today : CTOHeader::xtpCalendarItemState_Normal;
	nFlags |= bIsSelected ? CTOHeader::xtpCalendarItemState_Selected : 0;
	if (dayLayout.m_rcDayHeader.PtInRect(pt))
	{
		nFlags |=  CTOHeader::xtpCalendarItemState_Hot;
		dayLayout.m_nHotState |= CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotDayHeader;
	}
	else
	{
		dayLayout.m_nHotState &= ~CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT::xtpHotDayHeader;
	}

	nFlags |= bFirstDay ? CTOHeader::xtpCalendarItemFirst : 0;
	nFlags |= bLastDay ? CTOHeader::xtpCalendarItemLast : 0;

	CRect rcRect(dayLayout.m_rcDay);
	rcRect.bottom = dayLayout.m_rcDayHeader.bottom;

	CString strTextLeft;
	CString strTextCenter;
	CString strTextRight = CXTPCalendarUtils::GetDateFormat(dtDay, m_strHeaderFormat);

	//------------------------------------------------------
	pThemeX->GetItemTextIfNeed(xtpCalendarItemText_WeekViewDayHeaderLeft, &strTextLeft, pWViewDay);
	pThemeX->GetItemTextIfNeed(xtpCalendarItemText_WeekViewDayHeaderCenter, &strTextCenter, pWViewDay);
	pThemeX->GetItemTextIfNeed(xtpCalendarItemText_WeekViewDayHeaderRight, &strTextRight, pWViewDay);
	//------------------------------------------------------

	Draw_Header(pDC, rcRect, nFlags, strTextLeft, strTextCenter, strTextRight);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewDayHeader::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("WeekViewDayHeader")));
	if (pPX->IsStoring())
		secData->EmptySection();

	m_strHeaderFormat.DoPropExchange(&secData, _T("HeaderFormat"));

	TBase::DoPropExchange(&secData);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewDayHeader::Serialize(CArchive& ar)
{
	m_strHeaderFormat.Serialize(ar);

	TBase::Serialize(ar);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewEvent_MultiDay::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarWeekViewEvent* pViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarWeekViewEvent, pObject);

	CXTPCalendarWeekViewDay* pMVDay = XTP_SAFE_GET2(pViewEvent, GetViewGroup(), GetViewDay(), NULL);
	CXTPCalendarWeekView* pMView = XTP_SAFE_GET1(pMVDay, GetView(), NULL);

	if (!pDC || !pThemeX || !pViewEvent || !pViewEvent->GetEvent() ||
		!pMView || !pMVDay)
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bNoVisibleMDEvent = pViewEvent->IsMultidayEvent() && !pViewEvent->IsVisible();

	int nMDEFlags = pViewEvent->GetMultiDayEventFlags();
	ASSERT(0 == (nMDEFlags & xtpCalendarMultiDaySlave)); // error for week view.

	if (bNoVisibleMDEvent)
	{
		return;
	}

	CRect rcEventMax(pViewEvent->GetViewEventRectMax());
	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return;
	}

	// fill event background
	BOOL bSelected = pViewEvent->IsSelected();
	COLORREF clrBorderNotSel;
	COLORREF clrBorder;
	CXTPPaintManagerColorGradient grclrBk;

	GetEventColors(pViewEvent, clrBorder, clrBorderNotSel, grclrBk);

	//----------------
	pThemeX->DrawRoundRect(pDC, rcEvent, clrBorder, bSelected, grclrBk);

	//------------------
	int nViewDayIndex = pMVDay->GetDayIndex();

	BOOL bFirstOnScreen = nViewDayIndex == 0;
	BOOL bLastOnScreen = nViewDayIndex == 6;

	CRect rcText(pViewEvent->GetTextRect());

	if ((nMDEFlags & xtpCalendarMultiDayFirst) == 0)
	{
		//CRect rcSQ = rcEvent;
		////rcSQ.left += min(1, rcSQ.Width());
		CRect rcSQ = rcEventMax;
		rcSQ.left += min(1, rcSQ.Width());

		rcSQ.right = min(rcEvent.left + cnCornerSize, rcEventMax.right);
		CXTPCalendarTheme::DrawRectPartly(pDC, rcSQ, clrBorder, bSelected, grclrBk, CRect(0,1,0,1));

		//-------------------------------------------------------------------
		if (bFirstOnScreen)
		{
			CRect rcArrow;
			BOOL bArrow = Draw_ArrowL(pViewEvent, pDC, rcArrow);
			if (bArrow)
				rcText.left = rcArrow.right;
		}
	}
	else if (bSelected)
	{
		// draw left Gripper
		CRect rcGripper;
		rcGripper.top = rcEvent.top + rcEvent.Height() / 2 - cnGripperSize / 2;
		rcGripper.bottom = rcGripper.top + cnGripperSize;
		rcGripper.left = rcEvent.left - 1;
		rcGripper.right = rcGripper.left + cnGripperSize;

		pDC->FillSolidRect(&rcGripper, m_clrGripperBackground);
		CBrush brBorder(m_clrGripperBorder);
		pDC->FrameRect(&rcGripper, &brBorder);
	}


	if ((nMDEFlags & xtpCalendarMultiDayLast) == 0)
	{
//      CRect rcSQ = rcEvent;
//      rcSQ.left = max(rcEvent.right - cnCornerSize, rcEventMax.left);
//      //rcSQ.right -= min(2, rcSQ.Width());
		CRect rcSQ = rcEventMax;
		rcSQ.left = max(rcEvent.right - cnCornerSize, rcEventMax.left);
		rcSQ.right -= min(2, rcSQ.Width());
		CXTPCalendarTheme::DrawRectPartly(pDC, rcSQ, clrBorder, bSelected, grclrBk, CRect(0,1,0, 1));

		//-------------------------------------------------------------------
		if (bLastOnScreen)
		{
			CRect rcArrow;
			BOOL bArrow = Draw_ArrowR(pViewEvent, pDC, rcArrow);
			if (bArrow)
				rcText.right = rcArrow.left;
		}
	}
	else if (bSelected)
	{
		// draw right gripper
		CRect rcGripper;
		rcGripper.top = rcEvent.top + rcEvent.Height() / 2 - cnGripperSize / 2;
		rcGripper.bottom = rcGripper.top + cnGripperSize;
		rcGripper.right = rcEvent.right + 1;
		rcGripper.left = rcGripper.right - cnGripperSize;

		pDC->FillSolidRect(&rcGripper, m_clrGripperBackground);
		CBrush brBorder(m_clrGripperBorder);
		pDC->FrameRect(&rcGripper, &brBorder);
	}

	//****
	pViewEvent->m_rcText.CopyRect(rcText);

	//=====================================
	CString strSubj = pViewEvent->GetItemTextSubject();
	CString strLoc = pViewEvent->GetItemTextLocation();

	if (!strLoc.IsEmpty())
	{
		strSubj += _T(";");
	}

	CThemeFontColorSet* pFntClr1 = bSelected ? &m_fcsetSelected.fcsetSubject : &m_fcsetNormal.fcsetSubject;
	CThemeFontColorSet* pFntClr2 = bSelected ? &m_fcsetSelected.fcsetLocation : &m_fcsetNormal.fcsetLocation;
	rcText.DeflateRect(3, 0, 3, 0);

	pViewEvent->m_rcSubjectEditor.CopyRect(rcText);
	pViewEvent->m_rcSubjectEditor.top = rcEvent.top + 2;
	pViewEvent->m_rcSubjectEditor.bottom = rcEvent.bottom - 2;
	pViewEvent->AdjustSubjectEditor();

	//----------------------------------------------------
	UINT uTextShortAlign = DT_VCENTER | DT_LEFT;
	CSize szText = pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
								rcText, uTextShortAlign | DT_CALCRECT);

	CRect rcIconsMax = rcText;
	rcIconsMax.left = min(rcIconsMax.left + rcText.Height(), rcIconsMax.right);

	//--- calculate rects and fix to be centered if enough space ---
	CSize szIcons = Draw_Icons(pViewEvent, pDC, rcIconsMax, TRUE);

	if (rcIconsMax.Height() > szIcons.cy)
	{
		rcIconsMax.bottom -= (rcIconsMax.Height() - szIcons.cy) / 2;
	}

	int nTIcx = szText.cx + szIcons.cx;
	if (nTIcx + 5 < rcText.Width())
	{
		int ncx_2 = (rcText.Width() - nTIcx) / 2 - 2;
		rcText.left += ncx_2;
		rcText.right = rcText.left + szText.cx + 2;

		rcIconsMax.right -= ncx_2;
	}
	else
	{
		rcText.right -= szIcons.cx + 2;
	}

	pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
		rcText, uTextShortAlign);

	Draw_Icons(pViewEvent, pDC, rcIconsMax);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewEvent_SingleDay::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	CXTPCalendarWeekViewEvent* pViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarWeekViewEvent, pObject);
	if (!pDC || !pThemeX || !pViewEvent)
	{
		ASSERT(FALSE);
		return;
	}

	if (!pViewEvent->IsVisible())
	{
		return;
	}

	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return;
	}

	// fill event background
	Draw_Background(pDC, rcEvent, pViewEvent);

	Draw_Time(pDC, rcEvent, pViewEvent);

	CRect rcText(pViewEvent->GetTextRect());
	Draw_Caption(pDC, rcText, pViewEvent);
}

void CXTPCalendarThemeOffice2007::CTOWeekViewEvent_SingleDay::
		Draw_Background(CDC* pDC, const CRect& rcEventRect, CXTPCalendarWeekViewEvent* pViewEvent)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());

	if (!pDC || !pThemeX || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return;
	}
	BOOL bSelected = pViewEvent->IsSelected();

	COLORREF clrBorderNotSel, clrBorder;
	CXTPPaintManagerColorGradient grclrBk;
	GetEventColors(pViewEvent, clrBorder, clrBorderNotSel, grclrBk);

	CXTPCalendarEventLabel* ptrLabel = pViewEvent->GetEvent()->GetLabel();
	if (!bSelected && ptrLabel && ptrLabel->m_nLabelID != XTP_CALENDAR_NONE_LABEL_ID)
	{
		clrBorder = grclrBk.clrLight;
	}

	pThemeX->DrawRoundRect(pDC, rcEventRect, clrBorder, bSelected, grclrBk);
}

CSize CXTPCalendarThemeOffice2007::CTOWeekViewEvent_SingleDay::Draw_Time(
		CDC* pDC, const CRect& rcEventRect, CXTPCalendarWeekViewEvent* pViewEvent)
{
	CSize szResult(0, 0);

	if (!pDC || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return szResult;
	}

	BOOL bSelected = pViewEvent->IsSelected();

	CFont* pFont = bSelected ? m_fcsetSelected.fcsetStartEnd.Font() : m_fcsetNormal.fcsetStartEnd.Font();
	COLORREF clrColor = bSelected ? m_fcsetSelected.fcsetStartEnd.Color() : m_fcsetNormal.fcsetStartEnd.Color();

	CXTPFontDC autoFont(pDC, pFont, clrColor);

	//--------------------------
	COleDateTime dtStart(pViewEvent->GetEvent()->GetStartTime());
	COleDateTime dtEnd(pViewEvent->GetEvent()->GetEndTime());

	//*****************
	// not implemented for office 2007 theme.
	//if (pViewEvent->IsTimeAsClock()) {}
	//*****************

	CString strStartTime = pViewEvent->FormatEventTime(dtStart);
	CString strEndTime = pViewEvent->FormatEventTime(dtEnd);

	CSize szStart = pDC->GetTextExtent(strStartTime);
	CSize szEnd = pDC->GetTextExtent(strEndTime);

	CRect rcStart(rcEventRect);
	CRect rcEnd(rcEventRect);

	BOOL bShowEndTime = XTP_SAFE_GET2(pViewEvent, GetCalendarControl(), WeekView_IsShowEndDate(), TRUE);
	BOOL bDrawStartTime = szStart.cx + 6 < rcEventRect.Width()/2;
	BOOL bDrawEndTime = bShowEndTime && szStart.cx + szEnd.cx + 6 * 2 < rcEventRect.Width()/2;

	if (!bDrawStartTime && !bDrawEndTime)
	{
		return szResult;
	}

	rcStart.left = min(rcStart.left + 4, rcStart.right);
	rcStart.right = min(rcStart.left + szStart.cx + 6, rcStart.right);

	rcEnd.left = rcStart.right;
	rcEnd.right = min(rcEnd.left + szEnd.cx + 6, rcEnd.right);
	if (!bDrawEndTime)
	{
		rcEnd.right = rcEnd.left;
	}

	//-------------------------------------------------
	UINT uAlign = DT_VCENTER | DT_SINGLELINE | DT_LEFT;
	pDC->DrawText(strStartTime, &rcStart, uAlign);

	if (bDrawEndTime)
	{
		pDC->DrawText(strEndTime, &rcEnd, uAlign);
	}

	//-------------------------------------------------
	pViewEvent->m_rcStartTime.CopyRect(&rcStart);
	pViewEvent->m_rcEndTime.CopyRect(&rcEnd);

	pViewEvent->m_rcText.left = rcEnd.right;

	//-------------------------------------------------
	szResult.cx = max(0, rcEnd.right - rcStart.left);
	szResult.cy = szStart.cy;

	return szResult;
}

void CXTPCalendarThemeOffice2007::CTOWeekViewEvent_SingleDay::Draw_Caption(
		CDC* pDC, const CRect& rcTextRect, CXTPCalendarWeekViewEvent* pViewEvent)
{
	CXTPCalendarThemeOffice2007* pThemeX = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, GetTheme());
	if (!pDC || !pViewEvent || !pThemeX )
	{
		ASSERT(FALSE);
		return;
	}

	CString strSubj = pViewEvent->GetItemTextSubject();
	CString strLoc = pViewEvent->GetItemTextLocation();

	if (!strLoc.IsEmpty())
	{
		strSubj += _T(";");
	}

	BOOL bSelected = pViewEvent->IsSelected();
	CThemeFontColorSet* pFntClr1 = bSelected ? &m_fcsetSelected.fcsetSubject : &m_fcsetNormal.fcsetSubject;
	CThemeFontColorSet* pFntClr2 = bSelected ? &m_fcsetSelected.fcsetLocation : &m_fcsetNormal.fcsetLocation;

	CRect rcText = rcTextRect;
	//rcText.DeflateRect(1, 2, 3, 2);

	pViewEvent->m_rcSubjectEditor.CopyRect(rcText);
	pViewEvent->AdjustSubjectEditor();

	UINT nAlign = DT_VCENTER | DT_LEFT | DT_SINGLELINE;
	CSize szText = pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
								rcText, nAlign  | DT_CALCRECT, nAlign);

	//----------------------------------------------------
	CRect rcIconsMax = rcText;
	rcIconsMax.left = min(rcIconsMax.left + rcTextRect.Height(), rcIconsMax.right);

	//--- calculate rects and fix to be centered if enough space ---
	CSize szIcons = Draw_Icons(pViewEvent, pDC, rcIconsMax, TRUE);

	if (rcIconsMax.Height() > szIcons.cy)
	{
		rcIconsMax.bottom -= (rcIconsMax.Height() - szIcons.cy) / 2;
	}

	int nTIcx = szText.cx + szIcons.cx;
	if (nTIcx > rcText.Width())
	{
		rcText.right = max(rcIconsMax.left, rcIconsMax.right - szIcons.cx) - 2;
	}

	pThemeX->DrawLine2_CenterLR(pDC, strSubj, strLoc, pFntClr1, pFntClr2,
								rcText, nAlign, nAlign);

	Draw_Icons(pViewEvent, pDC, rcIconsMax);
}

/////////////////////////////////////////////////////////////////////////////
