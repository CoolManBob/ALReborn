// XTPCalendarThemeOffice2007.h: interface for the CXTPCalendarControlPaintManager class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(_XTP_CALENDAR_THEME_OFFICE_2007_H__)
#define _XTP_CALENDAR_THEME_OFFICE_2007_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarTheme.h"

#if (_MSC_VER > 1100)
#pragma warning(push)
#endif

#pragma warning(disable: 4250)
//#pragma warning(disable: 4097)

#pragma warning(disable : 4100)// TODO: remove when themes will be finished

class CXTPOffice2007Images;
class CXTPImageManager;

/////////////////////////////////////////////////////////////////////////////
// Initial version. Will be expanded in the feature.
//{{AFX_CODEJOCK_PRIVATE
class _XTP_EXT_CLASS CXTPCalendarThemeOffice2007 : public CXTPCalendarTheme
{
	//{{ AFX_CODEJOCK_PRIVATE
	DECLARE_DYNCREATE(CXTPCalendarThemeOffice2007)
	//}} AFX_CODEJOCK_PRIVATE

	typedef CXTPCalendarTheme TBase;
public:

	DECLARE_THEMEPART(CTOHeader, CXTPCalendarTheme::CTOHeader)

		DECLARE_THEMEPART_MEMBER(0, CTOFormula_MulDivC, HeightFormula)
		//===========================================
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

		virtual void Draw(CCmdTarget* pObject, CDC* pDC) {ASSERT(FALSE);};

	protected:
	};
	friend class CTOHeader;

	//=======================================================================
	DECLARE_THEMEPART(CTOEvent, CXTPCalendarTheme::CTOEvent)
		struct CEventFontsColorsSet
		{
			CXTPPaintManagerColor           clrBorder;
			CXTPPaintManagerColorGradient   grclrBackground;

			CThemeFontColorSetValue         fcsetSubject;
			CThemeFontColorSetValue         fcsetLocation;
			CThemeFontColorSetValue         fcsetBody;

			CThemeFontColorSetValue         fcsetStartEnd; // for a month view single day event times
			                                               // or DayView multiday event From/To
			virtual void CopySettings(const CEventFontsColorsSet& rSrc);

			virtual void doPX(CXTPPropExchange* pPX, LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme);
			virtual void Serialize(CArchive& ar);
		};

		CEventFontsColorsSet    m_fcsetNormal;
		CEventFontsColorsSet    m_fcsetSelected;

		CXTPPaintManagerColor   m_clrGripperBorder;
		CXTPPaintManagerColor   m_clrGripperBackground;

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void RefreshFromParent(CTOEvent* pParentSrc);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual int CalcMinEventHeight(CDC* pDC);

		virtual void GetEventColors(CXTPCalendarViewEvent* pViewEvent, COLORREF& rClrBorder,
									COLORREF& rClrBorderNotSel, CXTPPaintManagerColorGradient& rGrclrBk,
									BOOL bSelected = -1, int* pnColorsSrc_1Cat2Lbl = NULL);

		virtual void FillEventBackgroundEx(CDC* pDC, CXTPCalendarViewEvent* pViewEvent, const CRect& rcRect);
	protected:
		virtual BOOL Draw_ArrowL(CXTPCalendarViewEvent* pViewEvent, CDC* pDC, CRect& rrcRect);
		virtual BOOL Draw_ArrowR(CXTPCalendarViewEvent* pViewEvent, CDC* pDC, CRect& rrcRect);

		virtual CSize Draw_Icons(CXTPCalendarViewEvent* pViewEvent, CDC* pDC, const CRect& rcIconsMax, BOOL bCalculate = FALSE);
	};
	friend class CTOEvent;

	DECLARE_THEMEPART(CTODay, CXTPCalendarTheme::CTODay)
		virtual int HitTestExpandDayButton(CXTPCalendarViewDay* pViewDay, const CPoint* pPoint = NULL); // return 0 or xtpCalendarHitTestDayExpandButton
	};
	/////////////////////////////////////////////////////////////////////////
	// ******** Day View *********

	//=======================================================================
	DECLARE_THEMEPART2(CTODayViewEvent, CTOEvent, CXTPCalendarTheme::CTODayViewEvent)

		DECLARE_THEMEPART_MEMBER(0, CTOEventIconsToDraw, EventIconsToDraw)
		DECLARE_THEMEPART_MEMBER(1, CTOFormula_MulDivC,  HeightFormula)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
	protected:
	};

	DECLARE_THEMEPART2(CTODayViewEvent_MultiDay, CTODayViewEvent, CXTPCalendarTheme::CTODayViewEvent_MultiDay)

		CXTPCalendarThemeStringValue m_strDateFormatFrom;
		CXTPCalendarThemeStringValue m_strDateFormatTo;

		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber);
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

		virtual void CalcEventYs(CRect& rcRect, int nEventPlaceNumber);

	protected:
		virtual BOOL Draw_ArrowLtext(CXTPCalendarViewEvent* pViewEvent, CDC* pDC, CThemeFontColorSetValue* pfcsetText, CRect& rrcRect, int nLeft_x);
		virtual BOOL Draw_ArrowRtext(CXTPCalendarViewEvent* pViewEvent, CDC* pDC, CThemeFontColorSetValue* pfcsetText, CRect& rrcRect, int nRight_x);

		virtual CString Format_FromToDate(CXTPCalendarViewEvent* pViewEvent, int nStart1End2);
	};
	friend class CTODayViewEvent_MultiDay;

	DECLARE_THEMEPART2(CTODayViewEvent_SingleDay, CTODayViewEvent, CXTPCalendarTheme::CTODayViewEvent_SingleDay)
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	protected:
		virtual void InitBusyStatusDefaultColors();
	};
	friend class CTODayViewEvent_SingleDay;

	//=======================================================================
	DECLARE_THEMEPART(CTODayViewTimeScale, CXTPCalendarTheme::CTODayViewTimeScale)
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

		virtual int CalcMinRowHeight(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale);

		DECLARE_THEMEPART_MEMBER(0, CTOFormula_MulDivC, HeightFormula)
	};

	//=======================================================================
	DECLARE_THEMEPART2(CTODayViewDayGroupHeader, CTOHeader, CXTPCalendarTheme::CTODayViewDayGroupHeader)
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};

	DECLARE_THEMEPART(CTODayViewDayGroupAllDayEvents, CXTPCalendarTheme::CTODayViewDayGroupAllDayEvents)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
	};

	DECLARE_THEMEPART(CTODayViewDayGroupCell, CXTPCalendarTheme::CTODayViewDayGroupCell)
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual int CalcHeight(CDC* pDC, int nCellWidth) {return 23;}
	};

	//=======================================================================
	DECLARE_THEMEPART(CTODayViewDayGroup, CXTPCalendarTheme::CTODayViewDayGroup)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

		virtual BOOL IsSelected(CXTPCalendarViewGroup* pViewGroup);

		DECLARE_THEMEPART_MEMBER(0, CTODayViewDayGroupHeader,       Header)
		DECLARE_THEMEPART_MEMBER(1, CTODayViewDayGroupAllDayEvents, AllDayEvents)
		DECLARE_THEMEPART_MEMBER(2, CTODayViewDayGroupCell,         Cell)
		DECLARE_THEMEPART_MEMBER(3, CTODayViewEvent_MultiDay,       MultiDayEvent)
		DECLARE_THEMEPART_MEMBER(4, CTODayViewEvent_SingleDay,      SingleDayEvent)

	public:
		virtual void OnMouseMove(CCmdTarget* pObject, UINT nFlags, CPoint point);
		virtual BOOL OnLButtonDown(CCmdTarget* pObject, UINT nFlags, CPoint point);

		virtual CRect GetScrollButtonRect(const CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& groupLayout, int nButton);

	protected:
		virtual void AdjustDayEvents(CXTPCalendarDayViewGroup* pDayViewGroup, CDC* pDC);
	};

	DECLARE_THEMEPART2(CTODayViewDayHeader, CTOHeader, CXTPCalendarTheme::CTODayViewDayHeader)

		CTODayViewDayHeader() {
			m_nWeekDayFormat = 0;
		}

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

	private:
		int m_nWeekDayFormat; // 0 - no Week Day, 1 - short, 2 - long
	};

	//=======================================================================
	DECLARE_THEMEPART(CTODayViewDay, CXTPCalendarTheme::CTODayViewDay)

		CXTPPaintManagerColor         m_clrBorder;
		CXTPPaintManagerColor         m_clrTodayBorder;

		DECLARE_THEMEPART_MEMBER(0, CTODayViewDayHeader,            Header)
		DECLARE_THEMEPART_MEMBER(1, CTODayViewDayGroup,             Group)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual void Draw_DayBorder(CXTPCalendarDayViewDay* pDayViewDay, CDC* pDC);
		virtual CRect ExcludeDayBorder(CXTPCalendarDayViewDay* pDayViewDay, const CRect& rcDay);
	public:
		virtual void OnMouseMove(CCmdTarget* pObject, UINT nFlags, CPoint point);
	};

	DECLARE_THEMEPART(CTODayViewHeader, CTOHeader)
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
	};


	//=======================================================================
	DECLARE_THEMEPART(CTODayView, CXTPCalendarTheme::CTODayView)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

		DECLARE_THEMEPART_MEMBER_(0, CTODayViewHeader,   Header, CXTPCalendarTheme::CTOHeader)
		DECLARE_THEMEPART_MEMBER(1, CTODayViewEvent,     Event)
		DECLARE_THEMEPART_MEMBER(2, CTODayViewTimeScale, TimeScale)

		DECLARE_THEMEPART_MEMBER(3, CTODayViewDay, Day)

		// theme specific control options
		virtual BOOL IsUseCellAlignedDraggingInTimeArea() {return TRUE;};
	protected:
	};

	/////////////////////////////////////////////////////////////////////////
	// ******** Month View *********

	// ======= MonthViewEvent ======
	DECLARE_THEMEPART2(CTOMonthViewEvent, CTOEvent, CXTPCalendarTheme::CTOMonthViewEvent)
		DECLARE_THEMEPART_MEMBER(0, CTOEventIconsToDraw, EventIconsToDraw)
		DECLARE_THEMEPART_MEMBER(1, CTOFormula_MulDivC,  HeightFormula)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber);
	};
	friend class CTOMonthViewEvent;

	// ---- MonthViewEvent_SingleDay ----
	DECLARE_THEMEPART2(CTOMonthViewEvent_SingleDay, CTOMonthViewEvent, CXTPCalendarTheme::CTOMonthViewEvent_SingleDay)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

	protected:
		virtual void Draw_Background(CDC* pDC, const CRect& rcEventRect, CXTPCalendarMonthViewEvent* pViewEvent);
		virtual CSize Draw_Time(CDC* pDC, const CRect& rcEventRect, CXTPCalendarMonthViewEvent* pViewEvent);
		virtual void Draw_Caption(CDC* pDC, const CRect& rcTextRect, CXTPCalendarMonthViewEvent* pViewEvent);

	protected:
	};
	friend class CTOMonthViewEvent_SingleDay;

	// ---- MonthViewEvent_MultiDay ----
	DECLARE_THEMEPART2(CTOMonthViewEvent_MultiDay, CTOMonthViewEvent, CXTPCalendarTheme::CTOMonthViewEvent_MultiDay)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

	protected:
		virtual void Draw_Time(CDC* pDC, const CRect& rcEventRect, CXTPCalendarMonthViewEvent* pViewEvent);
	};
	friend class CTOMonthViewEvent_MultiDay;

	// ===== MonthViewDayHeader ====
	DECLARE_THEMEPART2(CTOMonthViewDayHeader, CTOHeader, CXTPCalendarTheme::CTOMonthViewDayHeader)
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

	};
	friend class CTOMonthViewDayHeader;

	// ======= MonthViewDay =======
	DECLARE_THEMEPART2(CTOMonthViewDay, CTODay, CXTPCalendarTheme::CTOMonthViewDay)

		CXTPPaintManagerColor m_clrBorder;
		CXTPPaintManagerColor m_clrTodayBorder;

		CXTPPaintManagerColor m_clrBackgroundLight;
		CXTPPaintManagerColor m_clrBackgroundDark;
		CXTPPaintManagerColor m_clrBackgroundSelected;

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		DECLARE_THEMEPART_MEMBER(0, CTOMonthViewDayHeader,       Header);
		DECLARE_THEMEPART_MEMBER(1, CTOMonthViewEvent_MultiDay,  MultiDayEvent);
		DECLARE_THEMEPART_MEMBER(2, CTOMonthViewEvent_SingleDay, SingleDayEvent);

	public:
		virtual void OnMouseMove(CCmdTarget* pObject, UINT nFlags, CPoint point);
		virtual BOOL OnLButtonDown(CCmdTarget* pObject, UINT nFlags, CPoint point);
	};
	friend class CTOMonthViewDay;

	// ======= MonthViewWeekDayHeader =======
	DECLARE_THEMEPART2(CTOMonthViewWeekDayHeader, CTOHeader, CXTPCalendarTheme::CTOMonthViewWeekDayHeader)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

	protected:
		virtual void Draw_Borders2(CDC* pDC, const CRect& rcRect, BOOL bDrawRightBorder);
	protected:
		BOOL m_bWeekDayNamesLong;
		BOOL m_bWeekDayNameSaSuLong;

		CStringArray m_arWeekDayNamesLong;  // 0 - Sunday, 1 - Monday, ...; 7 - Sat/Sun
		CStringArray m_arWeekDayNamesShort; // 0 - Sunday, 1 - Monday, ...; 7 - Sat/Sun

	public:
		CTOMonthViewWeekDayHeader()
		{
			m_bWeekDayNamesLong = FALSE;
			m_bWeekDayNameSaSuLong = FALSE;
		}
	};
	friend class CTOMonthViewWeekDayHeader;

	// ======= MonthViewWeekHeader =======
	DECLARE_THEMEPART(CTOMonthViewWeekHeader, CTOHeader)
		CXTPPaintManagerColor   m_clrFreeSpaceBk;
		// CTOHeader::m_TextCenter.fcsetNormal is used to draw header caption;

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual int CalcWidth(CDC* pDC);

		CTOMonthViewWeekHeader()
		{
			m_rcHeader.SetRect(0,0,0,0);
			m_bDateFormatShort = TRUE;
		}
	protected:
		CRect m_rcHeader;
		BOOL  m_bDateFormatShort;



		virtual void FormatWeekCaption(COleDateTime dtWDay1, CString& rstrShort, CString& rstrLong, int nShort1Long2 = 0);

		virtual CString _FormatWCaption(LPCTSTR pcszDay1, LPCTSTR pcszMonth1, LPCTSTR pcszDay7,
										LPCTSTR pcszMonth7, LPCTSTR pcszDayMonthSeparator, int nDateOrdering);

		virtual void Draw_TextCenter(CDC* pDC, const CRect& rcRect, int nState,
									 LPCTSTR pcszCenterText, int* pnWidth = NULL);
	};
	friend class CTOMonthViewWeekHeader;

	// ******* MonthView theme part object *******
	DECLARE_THEMEPART(CTOMonthView, CXTPCalendarTheme::CTOMonthView)

		// TODO:
		// BOOL m_bShowWeekNumbers;
		//
		//virtual void DoPropExchange(CXTPPropExchange* pPX);
		//virtual void Serialize(CArchive& ar);

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void AdjustLayout(CDC* pDC, const CRect& rcRect, BOOL bCallPostAdjustLayout);
		virtual void Draw(CDC* pDC);

		DECLARE_THEMEPART_MEMBER( 0, CTOHeader,                 Header);
		DECLARE_THEMEPART_MEMBER( 1, CTOMonthViewEvent,         Event);
		DECLARE_THEMEPART_MEMBER( 2, CTOMonthViewWeekDayHeader, WeekDayHeader);
		DECLARE_THEMEPART_MEMBER_(3, CTOMonthViewWeekHeader,    WeekHeader, CTOHeader);
		DECLARE_THEMEPART_MEMBER( 4, CTOMonthViewDay,           Day);
	};
	friend class CTOMonthView;


	/////////////////////////////////////////////////////////////////////////
	// ******** WeekView *********

	// ======= WeekViewEvent =======
	DECLARE_THEMEPART2(CTOWeekViewEvent, CTOEvent, CXTPCalendarTheme::CTOWeekViewEvent)
		DECLARE_THEMEPART_MEMBER(0, CTOEventIconsToDraw, EventIconsToDraw)
		DECLARE_THEMEPART_MEMBER(1, CTOFormula_MulDivC,  HeightFormula)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber);
	};
	friend class CTOWeekViewEvent;

	// ==== WeekViewEvent_SingleDay ====
	DECLARE_THEMEPART2(CTOWeekViewEvent_SingleDay, CTOWeekViewEvent, CXTPCalendarTheme::CTOWeekViewEvent_SingleDay)
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	protected:
		virtual void Draw_Background(CDC* pDC, const CRect& rcEventRect, CXTPCalendarWeekViewEvent* pViewEvent);
		virtual CSize Draw_Time(CDC* pDC, const CRect& rcEventRect, CXTPCalendarWeekViewEvent* pViewEvent);
		virtual void Draw_Caption(CDC* pDC, const CRect& rcTextRect, CXTPCalendarWeekViewEvent* pViewEvent);
	};
	friend class CTOWeekViewEvent_SingleDay;

	// ==== WeekViewEvent_MultiDay ====
	DECLARE_THEMEPART2(CTOWeekViewEvent_MultiDay, CTOWeekViewEvent, CXTPCalendarTheme::CTOWeekViewEvent_MultiDay)
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTOWeekViewEvent_MultiDay;

	// ==== WeekViewDayHeader ====
	DECLARE_THEMEPART2(CTOWeekViewDayHeader, CTOHeader, CXTPCalendarTheme::CTOWeekViewDayHeader)
		CXTPCalendarThemeStringValue m_strHeaderFormat;

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);
	};
	friend class CTOWeekViewDayHeader;

	// ======= WeekViewDay =======
	DECLARE_THEMEPART2(CTOWeekViewDay, CTODay, CXTPCalendarTheme::CTOWeekViewDay)

		CXTPPaintManagerColor m_clrBorder;
		CXTPPaintManagerColor m_clrTodayBorder;

		CXTPPaintManagerColor m_clrBackgroundLight;
		CXTPPaintManagerColor m_clrBackgroundDark;
		CXTPPaintManagerColor m_clrBackgroundSelected;

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual void OnMouseMove(CCmdTarget* pObject, UINT nFlags, CPoint point);
		virtual BOOL OnLButtonDown(CCmdTarget* pObject, UINT nFlags, CPoint point);

		DECLARE_THEMEPART_MEMBER(0, CTOWeekViewDayHeader,       Header);
		DECLARE_THEMEPART_MEMBER(1, CTOWeekViewEvent_MultiDay,  MultiDayEvent);
		DECLARE_THEMEPART_MEMBER(2, CTOWeekViewEvent_SingleDay, SingleDayEvent);
	};
	friend class CTOWeekViewDay;

	// ==== WeekView theme part object ====
	DECLARE_THEMEPART(CTOWeekView, CXTPCalendarTheme::CTOWeekView)
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

		DECLARE_THEMEPART_MEMBER(0, CTOWeekViewEvent, Event);
		DECLARE_THEMEPART_MEMBER(1, CTOWeekViewDay,   Day);
	};

	//=======================================================================
	DECLARE_THEMEPART_MEMBER(0, CTOColorsSet,   ColorsSet)

	DECLARE_THEMEPART_MEMBER(1, CTOHeader,      Header)
	DECLARE_THEMEPART_MEMBER(2, CTOEvent,       Event)

	DECLARE_THEMEPART_MEMBER(3, CTODayView,     DayView)
	DECLARE_THEMEPART_MEMBER(4, CTOMonthView,   MonthView)
	DECLARE_THEMEPART_MEMBER(5, CTOWeekView,    WeekView)

	//=======================================================================
	CXTPCalendarThemeOffice2007();
	virtual ~CXTPCalendarThemeOffice2007();

	// If enough space on the rect - draw 3 strings one under other
	// as multi-line text.
	// If rect height allow draw only 1 line - string 1 an 2 are drawn
	// as single line with a separator.
	// Separate font settings are used to draw each text.
	virtual CSize DrawText_Auto2SL3ML(CDC* pDC,
					LPCTSTR pcszText1, LPCTSTR pcszText2, LPCTSTR pcszText3,
					CThemeFontColorSet* pFontColor1, CThemeFontColorSet* pFontColor2,
					CThemeFontColorSet* pFontColor3,
					CRect& rcRect, LPCTSTR pcszText1Separator);


	// Common    colors id's: {0   - 99}
	// DayViev   colors id's: {100 - 499}
	// MonthViev colors id's: {500 - 999}
	// WeekViev  colors id's: {1000 - 1499}
	//
	// For darken colors: id + 5000

	enum XTPEnumThemeColorsSet
	{
		xtpCLR_DarkenOffset = 5000,

		xtpCLR_SelectedBk   = 1 + xtpCLR_DarkenOffset,

		xtpCLR_HeaderBorder = 2,
		xtpCLR_DayBorder    = 3,

		xtpCLR_MultiDayEventBorder          = 10,
		xtpCLR_MultiDayEventSelectedBorder  = 11 + xtpCLR_DarkenOffset,
		xtpCLR_MultiDayEventBkGRfrom        = 12,
		xtpCLR_MultiDayEventBkGRto          = 13,

		xtpCLR_MultiDayEventFromToDates     = 20,

		//xtpCLR_DayView_ = 100,
		xtpCLR_DayViewCellWorkBk                = 101,
		xtpCLR_DayViewCellNonWorkBk             = 102,

		xtpCLR_DayViewCellWorkBorderBottomInHour    = 103,
		xtpCLR_DayViewCellWorkBorderBottomHour      = 104,
		xtpCLR_DayViewCellNonWorkBorderBottomInHour = 105,
		xtpCLR_DayViewCellNonWorkBorderBottomHour   = 106,

		xtpCLR_DayViewAllDayEventsBk            = 110,
		xtpCLR_DayViewAllDayEventsBorderBottom  = 111,

		xtpCLR_DayViewSingleDayEventBorder      = 120,
		xtpCLR_DayViewSingleDayEventSelectedBorder= 121 + xtpCLR_DarkenOffset,
		xtpCLR_DayViewSingleDayEventBkGRfrom    = 122,
		xtpCLR_DayViewSingleDayEventBkGRto      = 123,

		//xtpCLR_MonthView_ = 500,
		xtpCLR_MonthViewDayBkLight                  = 501,
		xtpCLR_MonthViewDayBkDark                   = 502,
		xtpCLR_MonthViewDayBkSelected               = 503,

		xtpCLR_MonthViewEventTime                   = 510 + xtpCLR_DarkenOffset,

		xtpCLR_MonthViewSingleDayEventBorder        = 520,
		xtpCLR_MonthViewSingleDayEventSelectedBorder= 521 + xtpCLR_DarkenOffset,
		xtpCLR_MonthViewSingleDayEventBkGRfrom      = 522,
		xtpCLR_MonthViewSingleDayEventBkGRto        = 523,

		//xtpCLR_WeekView_ = 1000,
		xtpCLR_WeekViewDayBkLight                  = 1001,
		xtpCLR_WeekViewDayBkDark                   = 1002,
		xtpCLR_WeekViewDayBkSelected               = 1003,

		xtpCLR_WeekViewEventTime                   = 1010 + xtpCLR_DarkenOffset,

		xtpCLR_WeekViewSingleDayEventBorder         = 1020,
		xtpCLR_WeekViewSingleDayEventSelectedBorder = 1021 + xtpCLR_DarkenOffset,
		xtpCLR_WeekViewSingleDayEventBkGRfrom       = 1022,
		xtpCLR_WeekViewSingleDayEventBkGRto         = 1023,

	};

public:
	virtual CXTPCalendarViewEventSubjectEditor* StartEditSubject(CXTPCalendarViewEvent* pViewEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Thin method determine event view type (day, week, month) and
	//     return corresponding drawing part.
	// Returns:
	//     Theme drawing part for editing event.
	//-----------------------------------------------------------------------
	virtual CTOEvent* GetThemePartForEvent(CXTPCalendarViewEvent* pViewEvent);

	virtual void GetItemTextIfNeed(int nItem, CString* pstrText, CXTPCalendarViewDay* pViewDay);

	static const int cnGripperSize;
protected:
	virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

	virtual void _DrawEventLTHighlight(CDC* pDC, CPoint ptLT, int nRTx, const CXTPPaintManagerColorGradient& grclrBk,
										int nLTCorner = 0, int nLBy = INT_MAX, BOOL bRCorner = TRUE);


protected:

};
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//      This class is used to edit event subject in Office 2007 Theme.
// Remarks:
//      This class is derived from CXTPCalendarViewEventSubjectEditor and
//      draw editor background in Office 2007 Theme event object style
//      or use no background (transparent background).
// See Also:
//      CXTPCalendarViewEventSubjectEditor
//===========================================================================
class _XTP_EXT_CLASS  CXTPCalendarViewEventSubjectEditor2007 : public CXTPCalendarViewEventSubjectEditor
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pOwner     - A pointer to an owner calendar control object.
	//     pViewEvent - A pointer to an editing event view object.
	//     pTheme2007 - A pointer to an owner theme.
	// See Also: ~CXTPCalendarViewEventSubjectEditor2007()
	//-----------------------------------------------------------------------
	CXTPCalendarViewEventSubjectEditor2007(CXTPCalendarControl* pOwner, CXTPCalendarViewEvent* pViewEvent, CXTPCalendarThemeOffice2007* pTheme2007);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarViewEventSubjectEditor2007();

protected:
	CXTPCalendarViewEvent* m_pViewEvent;        // Stored pointer to the editing event object.
	CXTPCalendarThemeOffice2007* m_pTheme2007;  // Stored pointer to the owner theme.
	CXTPPaintManagerColorGradient m_grclrBk;    // Subject Editor background color (or gradient colors).

	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	afx_msg virtual BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg virtual HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg virtual void OnChange();
	//}}AFX_CODEJOCK_PRIVATE
};

/////////////////////////////////////////////////////////////////////////////

#if (_MSC_VER > 1100)
#pragma warning(pop)
#endif

#endif // !defined(_XTP_CALENDAR_THEME_OFFICE_2007_H__)
