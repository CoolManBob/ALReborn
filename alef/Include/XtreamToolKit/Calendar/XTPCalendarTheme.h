// XTPCalendarTheme.h: interface for the CXTPCalendarControlPaintManager class.
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
#if !defined(_XTP_CALENDAR_THEME_H__)
#define _XTP_CALENDAR_THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPColorManager.h"

#include "XTPCalendarViewPart.h"
#include "XTPCalendarPtrCollectionT.h"

#include "XTPCalendarView.h"
#include "XTPCalendarDayView.h"
#include "XTPCalendarDayViewTimeScale.h"
#include "XTPCalendarWeekView.h"

#if (_MSC_VER > 1100)
#pragma warning(push)
#endif

#pragma warning(disable: 4250)

#pragma warning(disable : 4100)// TODO: remove when themes will be finished

/////////////////////////////////////////////////////////////////////////////
class CXTPCalendarControl;
class CXTPCalendarViewDay;
class CXTPCalendarDayView;
class CXTPCalendarDayViewDay;
class CXTPCalendarDayViewGroup;
class CXTPCalendarMonthView;
class CXTPCalendarWeekView;
class CXTPCalendarDayViewEvent;
class CXTPCalendarMonthViewEvent;
class CXTPCalendarWeekViewEvent;
class CXTPCalendarViewEvent;
class CXTPCalendarDayViewTimeScale;
class CXTPImageManager;
class CXTPOffice2007Images;
class CXTPOffice2007ImageList;

class CXTPCalendarTheme;

/////////////////////////////////////////////////////////////////////////////
//{{AFX_CODEJOCK_PRIVATE
//===========================================================================
// In VS 7.1 the call TBase::memberFunc sometimes crash.
// The Service Pack 1 for VS 7.1 fix this compiler bug.
// XTP_SAFE_CALL_BASE macro helps to avoid this problem in any VS version.
//===========================================================================
#if (_MSC_VER == 1310)
	#define XTP_SAFE_CALL_BASE(_TBase, memberFunc) ((_TBase*)this)->_TBase::memberFunc
#else
	#define XTP_SAFE_CALL_BASE(_TBase, memberFunc) _TBase::memberFunc
#endif

#define DECLARE_THEMEPART_MEMBER_BASE
#undef DECLARE_THEMEPART_MEMBER_BASE
#define DECLARE_THEMEPART_MEMBER_BASE(cClassBase, memberName) \
public:                             \
	virtual cClassBase* Get##memberName##Part() {   \
		ASSERT(FALSE);                          \
		return NULL;                            \
	}

#define DECLARE_THEMEPART_MEMBER_BASE2
#undef DECLARE_THEMEPART_MEMBER_BASE2
#define DECLARE_THEMEPART_MEMBER_BASE2(cClassBase, memberName) \
public:                             \
	virtual cClassBase* Get##memberName##Part() {   \
		return NULL;                            \
	}

//=============================================================
#define DECLARE_THEMEPART
#undef DECLARE_THEMEPART

#define DECLARE_THEMEPART_CUSTOM
#undef DECLARE_THEMEPART_CUSTOM

#define DECLARE_THEMEPART2
#undef DECLARE_THEMEPART2

#if (_MSC_VER <= 1200) // Using Visual C++ 5.0, 6.0
	#define DECLARE_THEMEPART(cClass, cClassBase) \
		class _XTP_EXT_CLASS cClass##_Base : virtual public cClassBase {}; /* To access base class members (in VC 6 compiler) */ \
		class _XTP_EXT_CLASS cClass : virtual public cClass##_Base \
		{ \
			typedef cClass##_Base TBase;    \
		public:                             \

	#define DECLARE_THEMEPART_CUSTOM(cClass, cClassBase) \
		class cClass##_Base : virtual public cClassBase {}; /* To access base class members (in VC 6 compiler) */ \
		class cClass : virtual public cClass##_Base \
		{ \
			typedef cClass##_Base TBase; \
		public:

	#define DECLARE_THEMEPART2(cClass, cClassBase, cClassBase2) \
		class _XTP_EXT_CLASS cClass##_Base : virtual public cClassBase {}; /* To access base class members (in VC 6 compiler) */ \
		class _XTP_EXT_CLASS cClass##_Base2 : virtual public cClassBase2 {}; /* To access base class members (in VC 6 compiler) */ \
		class _XTP_EXT_CLASS cClass : virtual public cClass##_Base, virtual public cClass##_Base2 \
		{ \
			typedef cClass##_Base TBase; \
			typedef cClass##_Base2 TBase2; \
		public:


#else
	#define DECLARE_THEMEPART(cClass, cClassBase) \
		class _XTP_EXT_CLASS cClass : virtual public cClassBase \
		{ \
			typedef cClassBase TBase;    \
		public:                             \

	#define DECLARE_THEMEPART_CUSTOM(cClass, cClassBase) \
		class cClass : virtual public cClassBase \
		{ \
			typedef cClassBase TBase; \
		public:

	#define DECLARE_THEMEPART2(cClass, cClassBase, cClassBase2) \
		class _XTP_EXT_CLASS cClass : virtual public cClassBase, virtual public cClassBase2 \
		{ \
			typedef cClassBase TBase; \
			typedef cClassBase2 TBase2; \
		public:

#endif

//=============================================================

// TODO: custom via defines...
#define DECLARE_THEMEPART2_CUSTOM
#undef DECLARE_THEMEPART2_CUSTOM
#define DECLARE_THEMEPART2_CUSTOM(cClass, cClassBase, cClassBase2) \
	class cClass##_Base : virtual public cClassBase {}; /* To access base class members (in VC 6 compiler) */ \
	class cClass##_Base2 : virtual public cClassBase2 {}; /* To access base class members (in VC 6 compiler) */ \
	class cClass : virtual public cClass##_Base, virtual public cClass##_Base2 \
	{ \
		typedef cClass##_Base TBase; \
		typedef cClass##_Base2 TBase2; \
	public:

//=============================================================
#define DECLARE_THEMEPART_MEMBER_
#undef DECLARE_THEMEPART_MEMBER_
#define DECLARE_THEMEPART_MEMBER_(nIndex, cClass, memberName, cClassBase) \
protected:                                                  \
	virtual CXTPCalendarThemePart* CreateMember##nIndex(){  \
		ASSERT(TBase::CreateMember##nIndex() == NULL);      \
		m_p##memberName = new cClass();                     \
		if (m_p##memberName)                                \
			m_p##memberName->SetInstanceName(_T(#memberName));  \
		return m_p##memberName;     \
	}                               \
public:                             \
	cClass* m_p##memberName;        \
public:                             \
	virtual cClassBase* Get##memberName##Part() {       \
		return m_p##memberName;                         \
	}                                                   \
	virtual cClass* Get##memberName##PartX() {          \
		return m_p##memberName;                         \
	}                                                   \
	virtual void Set##memberName##Part(cClass* pPart) { \
		if (pPart)                                      \
		{                                               \
			if (!pPart->IsCreated())                    \
				pPart->Create(GetTheme(), this);        \
			pPart->SetInstanceName(_T(#memberName));    \
		}                                               \
		m_p##memberName = pPart;                        \
		m_arMembers.SetAt(nIndex, pPart);               \
	}

//=============================================================
#define DECLARE_THEMEPART_MEMBER
#undef DECLARE_THEMEPART_MEMBER
#define DECLARE_THEMEPART_MEMBER(nIndex, cClass, memberName) \
	DECLARE_THEMEPART_MEMBER_(nIndex, cClass, memberName, CXTPCalendarTheme::##cClass)
// }}AFX_CODEJOCK_PRIVATE

// {{AFX_CODEJOCK_PRIVATE
/////////////////////////////////////////////////////////////////////////////

//=============================================================================
class _XTP_EXT_CLASS CXTPCalendarThemePart : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPCalendarThemePart)

public:
	CXTPCalendarThemePart();
	virtual ~CXTPCalendarThemePart();

	virtual CXTPCalendarThemePart* GetOwner();
	virtual CXTPCalendarTheme* GetTheme();

	virtual LPCTSTR GetInstanceName();
	virtual void SetInstanceName(LPCTSTR pcszInstanceName);

	virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

	virtual void DoPropExchange(CXTPPropExchange* pPX);
	virtual void Serialize(CArchive& ar);

	//****************
	virtual void Draw(CCmdTarget* pObject, CDC* pDC){};

public:
	// messages handlers

	virtual void OnMouseMove(CCmdTarget* pObject, UINT nFlags, CPoint point){};
	virtual BOOL OnLButtonDown(CCmdTarget* pObject, UINT nFlags, CPoint point){return FALSE;};

protected:
	CXTPCalendarTheme*      m_pTheme;
	CXTPCalendarThemePart*  m_pOwner;
	CString                 m_strInstanceName;

protected:
	typedef CXTPCalendarPtrCollectionT<CXTPCalendarThemePart> CThemePartsArray;
	CThemePartsArray    m_arMembers;

public:
	virtual BOOL IsCreated();
	virtual void Create(CXTPCalendarTheme* pTheme, CXTPCalendarThemePart* pOwner);

protected:
	virtual void _CreateMembers();

	virtual CXTPCalendarThemePart* CreateMember0(){return NULL;};
	virtual CXTPCalendarThemePart* CreateMember1(){return NULL;};
	virtual CXTPCalendarThemePart* CreateMember2(){return NULL;};
	virtual CXTPCalendarThemePart* CreateMember3(){return NULL;};
	virtual CXTPCalendarThemePart* CreateMember4(){return NULL;};
	virtual CXTPCalendarThemePart* CreateMember5(){return NULL;};
	virtual CXTPCalendarThemePart* CreateMember6(){return NULL;};
	virtual CXTPCalendarThemePart* CreateMember7(){return NULL;};
	virtual CXTPCalendarThemePart* CreateMember8(){return NULL;};
	virtual CXTPCalendarThemePart* CreateMember9(){return NULL;};

protected:
	class _XTP_EXT_CLASS CXTPPropsState
	{
	public:
		CXTPPropsState();
		virtual ~CXTPPropsState();

		void SaveState();
		void RestoreState(BOOL bClearData);
		void ClearLastState();

		void ClearAll();

	#ifdef _DEBUG
		static int s_dbg_nActiveStartes;
	#endif

	protected:
		CXTPCalendarTypedPtrAutoDeleteArray<CPtrArray, CMemFile*> m_arStates;

		static BOOL s_bUseSerialize;
	};
	friend class CXTPPropsState;
	//---------------------------------------------
	class _XTP_EXT_CLASS CXTPPropsStateContext
	{
		CXTPCalendarThemePart* m_pPart;
		int                    m_eBeroreDrawFlag;

		//BOOL  m_bAttached;
	public:
		CXTPPropsStateContext(CXTPCalendarThemePart* pPart = NULL, int eBDFlag = 0, BOOL bClearAll = FALSE);
		virtual ~CXTPPropsStateContext();

		BOOL IsActive() {
			return !!m_pPart;
		}

		void SetData(CXTPCalendarThemePart* pPart, int eBDFlag = 0, BOOL bClearAll = FALSE);
		void Clear();

		//void Attach(CXTPCalendarThemePart* pPart, int eBDFlag = 0);

		void RestoreState();
		void SendBeforeDrawThemeObject(LPARAM lParam, int eBDFlag = 0);
	};
	friend class CXTPPropsStateContext;

	virtual void SendNotificationAlways(XTP_NOTIFY_CODE EventCode, WPARAM wParam , LPARAM lParam);

	CXTPPropsState m_xPropsState;
	BOOL m_bSendBeforeDrawThemeObjectEnabled;
};

/////////////////////////////////////////////////////////////////////////////
// Initial version. Will be expanded in the feature.
class _XTP_EXT_CLASS CXTPCalendarTheme: public CXTPCalendarThemePart
{
	friend class CXTPCalendarControl;
	DECLARE_DYNAMIC(CXTPCalendarTheme)

	typedef CXTPCalendarThemePart TBase;
protected:
	CXTPCalendarControl* m_pCalendar;
	virtual void SetCalendarControl(CXTPCalendarControl* pCalendar);

	virtual void Init();

public:
	CXTPCalendarTheme();
	virtual ~CXTPCalendarTheme();

	virtual CXTPCalendarControl* GetCalendarControl();

	//=======================================================================
	class _XTP_EXT_CLASS CThemeFontColorSet
	{
	public:
		virtual CFont* Font() const = 0;
		virtual COLORREF Color() const = 0;
	};

	//=======================================================================
	class _XTP_EXT_CLASS CThemeFontColorSetValue : public CThemeFontColorSet
	{
	public:
		CXTPPaintManagerColor       clrColor;
		CXTPCalendarThemeFontValue  fntFont;

		void SetStandardValue(const CThemeFontColorSet& fcsetValue)
		{
			clrColor.SetStandardValue(fcsetValue.Color());
			fntFont.SetStandardValue(fcsetValue.Font());
		}

		void CopySettings(const CThemeFontColorSetValue& refSrc) {
			CXTPCalendarTheme::CopySettings(clrColor, refSrc.clrColor);
			fntFont.CopySettings(refSrc.fntFont);
		}
		//--------------------------------------------
		virtual CFont* Font() const {
			return fntFont;
		}
		virtual COLORREF Color() const {
			return clrColor;
		}

		virtual void doPX(CXTPPropExchange* pPX, LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme);
		virtual void Serialize(CArchive& ar);
	};

	//=======================================================================
	class _XTP_EXT_CLASS CTOFormula_MulDivC : virtual public CXTPCalendarThemePart
	{
	public:
		CTOFormula_MulDivC();

		virtual void RefreshFromParent(CTOFormula_MulDivC* pParentSrc);

		virtual void SetStandardValue(int nMul, int nDiv, int nC);
		virtual int Calculate(int nY) const;

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);
		//------------------------------------------------------------

		CXTPCalendarThemeIntValue m_Multiplier;
		CXTPCalendarThemeIntValue m_Divisor;
		CXTPCalendarThemeIntValue m_Constant;
	};
	friend class CTOFormula_MulDivC;

	//=======================================================================
	class _XTP_EXT_CLASS CTOEventIconsToDraw : virtual public CXTPCalendarThemePart
	{
	public:

		CXTPCalendarThemeBOOLValue m_ShowReminder;
		CXTPCalendarThemeBOOLValue m_ShowOccurrence;
		CXTPCalendarThemeBOOLValue m_ShowException;
		CXTPCalendarThemeBOOLValue m_ShowMeeting;
		CXTPCalendarThemeBOOLValue m_ShowPrivate;

		virtual void RefreshFromParent(CTOEventIconsToDraw* pParentSrc);
		virtual void SetStandardValue(BOOL bRmd, BOOL bOcc, BOOL bExc, BOOL bMee, BOOL bPrv);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		CTOEventIconsToDraw();
		virtual void FillIconIDs(CUIntArray& rarGlyphIDs, CUIntArray& rarGlyphIndex, CXTPCalendarEvent* pEvent);
	};
	friend class CTOEventIconsToDraw;

	//=======================================================================
	class _XTP_EXT_CLASS CTOHeader : virtual public CXTPCalendarThemePart
	{
	public:

		enum XTPEnumCalendarItemState
		{
			xtpCalendarItemState_Normal     = 0,
			xtpCalendarItemState_Selected   = 0x001,
			xtpCalendarItemState_Today      = 0x002,
			xtpCalendarItemState_Hot        = 0x004,
			xtpCalendarItemState_mask       = 0x00F,

			xtpCalendarItemFirst            = 0x010,
			xtpCalendarItemLast             = 0x020,
		};

		struct _XTP_EXT_CLASS CHeaderText
		{
			CThemeFontColorSetValue fcsetNormal;
			CThemeFontColorSetValue fcsetSelected;
			CThemeFontColorSetValue fcsetToday;
			CThemeFontColorSetValue fcsetTodaySelected;

			virtual CThemeFontColorSet* GetFontColor(int nItemState);

			virtual void CopySettings(const CHeaderText& rSrc);

			virtual void doPX(CXTPPropExchange* pPX, LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme);
			virtual void Serialize(CArchive& ar);
		};

		struct _XTP_EXT_CLASS CHeaderBackground
		{
			CXTPCalendarThemeIntValue   nBitmapID;
			CXTPCalendarThemeRectValue  rcBitmapBorder;

			// effects ...
			// CXTPCalendarThemeIntValue eDrawOptions;
			//
			// CXTPCalendarThemeBOOLValue bFillBySelectedColorBeforeDraw;
			// CXTPCalendarThemeBOOLValue bFillTextBySelectedColorAfterDraw;

			virtual void CopySettings(const CHeaderBackground& rSrc)
			{
				nBitmapID.CopySettings(rSrc.nBitmapID);
				rcBitmapBorder.CopySettings(rSrc.rcBitmapBorder);
			}

			virtual void doPX(CXTPPropExchange* pPX, LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme);
			virtual void Serialize(CArchive& ar);
		};

		//===========================================
		CXTPPaintManagerColor m_clrBaseColor;
		CXTPPaintManagerColor m_clrTodayBaseColor;

		CHeaderBackground   m_bkNormal;
		CHeaderBackground   m_bkSelected;
		CHeaderBackground   m_bkToday;
		CHeaderBackground   m_bkTodaySelected;

		CHeaderText  m_TextLeftRight;
		CHeaderText  m_TextCenter;

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);
		virtual void RefreshFromParent(CTOHeader* pParentSrc);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect){};
		virtual int CalcHeight(CDC* pDC, int nCellWidth);

		//-------------------------------------------------------------------
		virtual BOOL IsToday(CXTPCalendarViewDay* pViewDay);
		virtual BOOL IsSelected(CXTPCalendarViewDay* pViewDay);
		virtual BOOL IsSelected(CXTPCalendarViewGroup* pViewGroup);

		//-------------------------------------------------------------------
		virtual void Draw_Header(CDC* pDC, const CRect& rcRect, int nFlags,
								 LPCTSTR pcszLeftText, LPCTSTR pcszCenterText = NULL, LPCTSTR pcszRightText = NULL);

		virtual void Draw_Background(CDC* pDC, const CRect& rcRect, int nState);

		virtual void Draw_TextLR(CDC* pDC, const CRect& rcRect, int nFlags,
								 LPCTSTR pcszText, UINT uFormat, int* pnWidth = NULL);

		virtual void Draw_TextCenter(CDC* pDC, const CRect& rcRect, int nState,
									 LPCTSTR pcszCenterText, int* pnWidth = NULL);

		DECLARE_THEMEPART_MEMBER_BASE(CTOFormula_MulDivC, HeightFormula)

	protected:
	};

	class _XTP_EXT_CLASS CTOEvent : virtual public CXTPCalendarThemePart
	{
	public:
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber){};
		virtual int CalcMinEventHeight(CDC* pDC) {return 19;};

		DECLARE_THEMEPART_MEMBER_BASE2(CTOFormula_MulDivC, HeightFormula)
		DECLARE_THEMEPART_MEMBER_BASE2(CTOEventIconsToDraw, EventIconsToDraw)
	};

	class _XTP_EXT_CLASS CTODay : virtual public CXTPCalendarThemePart
	{
	public:
		virtual int GetExpandButtonHeight();
		virtual int HitTestExpandDayButton(CXTPCalendarViewDay* pViewDay, const CPoint* pPoint = NULL); // return 0 or xtpCalendarHitTestDayExpandButton
	protected:
		virtual int _HitTestExpandDayButton(CXTPCalendarViewDay* pViewDay, const CRect& rc, const CPoint* pPoint = NULL); // return 0 or xtpCalendarHitTestDayExpandButton
	};

	/////////////////////////////////////////////////////////////////////////
	//
	// *** DayView theme parts ***
	//
	// Parts Hierarchy:
	//   *Theme                    CXTPCalendarTheme
	//   *--Header*                 CTOHeader
	//   *--Event*                  CTOEvent
	//
	//   *--DayView                 CTODayView
	//   *----Event*                CTODayViewEvent
	//   *----Header                CTOHeader
	//   *----TimeScale             CTODayViewTimeScale
	//   *----Day                   CTODayViewDay
	//   *------Header              CTODayViewDayHeader
	//   *------Group               CTODayViewDayGroup
	//   *--------Header            CTODayViewDayGroupHeader
	//   *--------AllDayEvents      CTODayViewDayGroupAllDayEvents
	//   *--------Cell              CTODayViewDayGroupCell
	//   *--------MultiDayEvent     CTODayViewEvent_MultiDay
	//   *--------SingleDayEvent    CTODayViewEvent_SingleDay
	//
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// DayView Time Scale
	class _XTP_EXT_CLASS CTODayViewTimeScale : virtual public CXTPCalendarThemePart
	{
	public:

		CXTPPaintManagerColor   m_clrBackground;
		CXTPPaintManagerColor   m_clrLine;
		CThemeFontColorSetValue m_fcsetCaption;

		CThemeFontColorSetValue m_fcsetAMPM;

		CThemeFontColorSetValue m_fcsetSmall;
		CThemeFontColorSetValue m_fcsetBigBase;

		CXTPPaintManagerColorGradient m_grclrNowLineBk;
		CXTPPaintManagerColor         m_clrNowLine;

		CXTPCalendarThemeBOOLValue m_ShowMinutes;

		DECLARE_THEMEPART_MEMBER_BASE(CTOFormula_MulDivC, HeightFormula)

		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

		virtual int CalcMinRowHeight(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale);
		virtual int CalcWidth(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale, int nRowHeight);
		virtual int CalcWidth_AmPmMinutesPart(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale, int* pnCharWidth = NULL);

		virtual int GetRightOffsetX(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale);

	public:
		virtual void Draw_SmallHourCell(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale,
										const CRect& rcCell,
										const CString& strPart1, const CString& strPart2,
										CThemeFontColorSetValue* pfcsetPart1,
										CThemeFontColorSetValue* pfcsetPart2,
										CXTPPropsStateContext& autoStateCnt,
										XTP_CALENDAR_THEME_DAYVIEWTIMESCALECELL_PARAMS& tsCell);

		virtual void Draw_BigHourCell(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale,
									  const CRect& rcCell, int nRowPerHour,
									  const CString& strHour, const CString& strMinAmPm,
									  CThemeFontColorSetValue* pfcsetHour,
									  CThemeFontColorSetValue* pfcsetMinAmPm,
									  CXTPPropsStateContext& autoStateCnt,
									  XTP_CALENDAR_THEME_DAYVIEWTIMESCALECELL_PARAMS& tsCell);

		virtual void Draw_Caption(CDC* pDC, const CRect& rcRect, const CString& strCaption,
								  CThemeFontColorSetValue* pfcsetCaption);

		virtual CRect Calc_NowLineRect(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale);

		virtual void Draw_NowLineBk(CDC* pDC, const CRect& rcRect);
		virtual void Draw_NowLine(CDC* pDC, const CRect& rcRect);

		virtual BOOL IsDrawNowLine(CDC* pDC, CXTPCalendarDayViewTimeScale* pTS);

		CTODayViewTimeScale()
		{
			m_nWidthAmPmMinutesPart_ = 0;
			m_nCharWidthAmPmMinutesPart_ = 0;
		}
	protected:
		CThemeFontColorSetValue m_fcsetBigHour_;
		int  m_nWidthAmPmMinutesPart_;
		int  m_nCharWidthAmPmMinutesPart_;

		CTOFormula_MulDivC m_hflaBigHourFont;
		virtual void AdjustBigHourFont(CDC* pDC, const CRect& rcRowCell);
		virtual void RefreshBigHourFontIfNeed(BOOL bRefreshToStandard);

		struct XTP_LC_TIMEFORMAT
		{
			XTP_LC_TIMEFORMAT();
			virtual void RefreshMetrics();

			BOOL    bAMPM_timeFormat;
			BOOL    bLeadingZeroTime;
			CString strSeparator;
			CString strAM;
			CString strPM;
		};

		XTP_LC_TIMEFORMAT   m_localeTimeFormat;

		CString FormatTime(XTP_LC_TIMEFORMAT& lcTimeFormat, COleDateTime dtTime,
						BOOL bAmPmEnshure, CString& rstrHour, CString& rstrMin, CString& rstrAmPm);
	};
	friend class CTODayViewTimeScale;


	/////////////////////////////////////////////////////////////////////////
	// Day View Events (different types)
	class _XTP_EXT_CLASS CTODayViewEvent : virtual public CTOEvent
	{
	public:
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber);
	};
	friend class CTODayViewEvent;

	//==========================================================================
	class _XTP_EXT_CLASS CTODayViewEvent_MultiDay : virtual public CTODayViewEvent
	{
	public:
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber);

		virtual void CalcEventYs(CRect& rcRect, int nEventPlaceNumber){};
	};
	friend class CTODayViewEvent_MultiDay;

	//==========================================================================
	class _XTP_EXT_CLASS CTODayViewEvent_SingleDay : virtual public CTODayViewEvent
	{
	public:
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber);

		virtual CBrush* GetBusyStatusBrush(int nBusyStatus);
		virtual void SetBusyStatusBrush(int nBusyStatus, CBrush* pBrush, BOOL bAutodeleteBrush);

	protected:
		virtual void InitBusyStatusDefaultColors();

		virtual CXTPCalendarViewPartBrushValue* _GetBusyStatusBrushValue(int nBusyStatus, BOOL bCreateIfNotExist);
		CXTPCalendarTypedPtrAutoDeleteMap<int, CXTPCalendarViewPartBrushValue*> m_mapBusyStatusBrushes;

		CBitmap     m_bmpTentativePattern;  // Stores standard bitmap pattern to draw Tentative event busy status.
		CBrush      m_brushTentative;       // Stores standard brush to draw Tentative event busy status.
	};
	friend class CTODayViewEvent_SingleDay;

	/////////////////////////////////////////////////////////////////////////
	// DayView Resource Group
	class _XTP_EXT_CLASS CTODayViewDayGroupHeader : virtual public CTOHeader
	{
	public:
	};
	friend class CTODayViewDayGroupHeader;

	//=======================================================================
	class _XTP_EXT_CLASS CTODayViewDayGroupAllDayEvents : virtual public CXTPCalendarThemePart
	{
	public:
		CXTPPaintManagerColor         m_clrBackground;
		CXTPPaintManagerColor         m_clrBackgroundSelected;
		CXTPPaintManagerColor         m_clrBottomBorder;

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTODayViewDayGroupAllDayEvents;

	//=======================================================================
	class _XTP_EXT_CLASS CTODayViewDayGroupCell : virtual public CXTPCalendarThemePart
	{
	public:

		struct XTP_GROUP_CELL_COLOR_SET
		{
			CXTPPaintManagerColor clrBackground;
			CXTPPaintManagerColor clrBorderBottomInHour;
			CXTPPaintManagerColor clrBorderBottomHour;
			CXTPPaintManagerColor clrSelected;

			void doPX(CXTPPropExchange* pPX, LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme);
			virtual void Serialize(CArchive& ar);
		};

		XTP_GROUP_CELL_COLOR_SET m_clrsetWorkCell;
		XTP_GROUP_CELL_COLOR_SET m_clrsetNonWorkCell;

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual void Draw(CDC* pDC, const CRect& rcRect,
						  const XTP_CALENDAR_THEME_DAYVIEWCELL_PARAMS& cellParams);

		virtual int CalcHeight(CDC* pDC, int nCellWidth) {return 19;} // pDayView->CalculateHeaderFormatAndHeight(pDC, nCellWidth);
	};
	friend class CTODayViewDayGroupCell;

	//=======================================================================
	class _XTP_EXT_CLASS CTODayViewDayGroup : virtual public CXTPCalendarThemePart
	{
	public:
		CXTPPaintManagerColor         m_clrBorderLeft;

		virtual void DoPropExchange(CXTPPropExchange* pPX);
		virtual void Serialize(CArchive& ar);

		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);
		virtual BOOL IsSelected(CXTPCalendarViewGroup* pViewGroup){return FALSE;};

		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewDayGroupHeader,         Header)
		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewDayGroupAllDayEvents,   AllDayEvents)
		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewDayGroupCell,           Cell)
		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewEvent_MultiDay,         MultiDayEvent)
		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewEvent_SingleDay,        SingleDayEvent)

		virtual int HitTestScrollButton(CXTPCalendarDayViewGroup* pViewGroup, const CPoint* pPoint = NULL); // return 0 or ... flag
		virtual CRect GetScrollButtonRect(const CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& groupLayout, int nButton) = 0;
	protected:
		virtual void AdjustDayEvents(CXTPCalendarDayViewGroup* pDayViewGroup, CDC* pDC);
	};
	friend class CTODayViewDayGroup;

	/////////////////////////////////////////////////////////////////////////
	// DayView day theme part
	class _XTP_EXT_CLASS CTODayViewDayHeader : virtual public CTOHeader
	{
	public:
		virtual void Draw(CCmdTarget* pObject, CDC* pDC){};
	};
	friend class CTODayViewDayHeader;

	//==========================================================================
	class _XTP_EXT_CLASS CTODayViewDay : virtual public CXTPCalendarThemePart
	{
	public:
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		virtual void Draw(CCmdTarget* pObject, CDC* pDC);

		virtual CRect ExcludeDayBorder(CXTPCalendarDayViewDay* pDayViewDay, const CRect& rcDay) {return rcDay;};
		virtual void Draw_DayBorder(CXTPCalendarDayViewDay* pDayViewDay, CDC* pDC) {};

		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewDayHeader,          Header)
		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewDayGroup,           Group)
	};
	friend class CTODayViewDay;

	/////////////////////////////////////////////////////////////////////////
	// DayView theme part object
	class _XTP_EXT_CLASS CTODayView : virtual public CXTPCalendarThemePart
	{
	public:
		virtual void AdjustLayout(CDC* pDC, const CRect& rcRect);
		virtual void OnPostAdjustLayout();
		virtual void Draw(CDC* pDC);

		DECLARE_THEMEPART_MEMBER_BASE(CTOHeader,            Header)
		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewEvent,      Event)
		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewTimeScale,  TimeScale)

		DECLARE_THEMEPART_MEMBER_BASE(CTODayViewDay, Day)

		// theme specific control options
		virtual BOOL IsUseCellAlignedDraggingInTimeArea() {return FALSE;};
	protected:
		virtual int CalcMinCellHeight(CDC* pDC, CXTPCalendarDayView* pDayView);
	};
	friend class CTODayView;

	/////////////////////////////////////////////////////////////////////////
	//
	// *** MonthView theme parts ***
	//
	// Parts Hierarchy:
	//   *Theme                    CXTPCalendarTheme
	//   *--Header*                 CTOHeader
	//   *--Event*                  CTOEvent
	//
	//   *--MonthView               CTOMonthView
	//   *----Event*                CTOMonthViewEvent
	//   *----Header*               CTOHeader
	//   *----WeekDayHeader         CTOMonthViewWeekDayHeader
	//   *----Day                   CTOMonthViewDay
	//   *------Header              CTOMonthViewDayHeader
	//   *------MultiDayEvent       CTOMonthViewEvent_MultiDay
	//   *------SingleDayEvent      CTOMonthViewEvent_SingleDay
	//
	/////////////////////////////////////////////////////////////////////////

	class _XTP_EXT_CLASS CTOMonthViewEvent : virtual public CTOEvent
	{
	public:
		//virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTOMonthViewEvent;

	//------------------------------------------
	class _XTP_EXT_CLASS CTOMonthViewEvent_SingleDay : virtual public CTOMonthViewEvent
	{
	public:
		//virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTOMonthViewEvent_SingleDay;

	//------------------------------------------
	class _XTP_EXT_CLASS CTOMonthViewEvent_MultiDay : virtual public CTOMonthViewEvent
	{
	public:
		//virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTOMonthViewEvent_MultiDay;

	//==========================================================================
	class _XTP_EXT_CLASS CTOMonthViewDayHeader : virtual public CTOHeader
	{
		//virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTOMonthViewDayHeader;

	//==========================================================================
	class _XTP_EXT_CLASS CTOMonthViewDay : virtual public CTODay //virtual public CXTPCalendarThemePart
	{
	public:
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC){};

		DECLARE_THEMEPART_MEMBER_BASE(CTOMonthViewDayHeader,       Header);
		DECLARE_THEMEPART_MEMBER_BASE(CTOMonthViewEvent_MultiDay,  MultiDayEvent);
		DECLARE_THEMEPART_MEMBER_BASE(CTOMonthViewEvent_SingleDay, SingleDayEvent);
	};
	friend class CTOMonthViewDay;

	//==========================================================================
	class _XTP_EXT_CLASS CTOMonthViewWeekDayHeader : virtual public CTOHeader
	{
	public:
		//virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC){};
	};
	friend class CTOMonthViewWeekDayHeader;

	/////////////////////////////////////////////////////////////////////////
	// MonthView theme part object
	class _XTP_EXT_CLASS CTOMonthView : virtual public CXTPCalendarThemePart
	{
	public:
		virtual void AdjustLayout(CDC* pDC, const CRect& rcRect, BOOL bCallPostAdjustLayout);
		virtual void Draw(CDC* pDC);

		DECLARE_THEMEPART_MEMBER_BASE(CTOHeader,            Header);
		DECLARE_THEMEPART_MEMBER_BASE(CTOMonthViewEvent,    Event);
		DECLARE_THEMEPART_MEMBER_BASE(CTOMonthViewWeekDayHeader,        WeekDayHeader);
		DECLARE_THEMEPART_MEMBER_BASE(CTOMonthViewDay,      Day);
	};
	friend class CTOMonthView;

	/////////////////////////////////////////////////////////////////////////
	//
	// *** WeekView theme parts ***
	//
	// Parts Hierarchy:
	//   *Theme                    CXTPCalendarTheme
	//   *--Event*                  CTOEvent
	//
	//   *--WeekView                CTOWeekView
	//   *----Event*                CTOWeekViewEvent
	//   *----Day                   CTOWeekViewDay
	//   *------DayHeader           CTOWeekViewDayHeader
	//   *------MultiDayEvent       CTOWeekViewEvent_MultiDay
	//   *------SingleDayEvent      CTOWeekViewEvent_SingleDay
	//
	/////////////////////////////////////////////////////////////////////////

	// === WeekViewEvent ===
	class _XTP_EXT_CLASS CTOWeekViewEvent : virtual public CTOEvent
	{
	public:
		//virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect, int nEventPlaceNumber);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTOWeekViewEvent;

	// --- WeekViewEvent_SingleDay ---
	class _XTP_EXT_CLASS CTOWeekViewEvent_SingleDay : virtual public CTOWeekViewEvent
	{
	public:
		//virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTOWeekViewEvent_SingleDay;

	// --- WeekViewEvent_MultiDay ---
	class _XTP_EXT_CLASS CTOWeekViewEvent_MultiDay : virtual public CTOWeekViewEvent
	{
	public:
		//virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTOWeekViewEvent_MultiDay;

	// === WeekViewDayHeader ===
	class _XTP_EXT_CLASS CTOWeekViewDayHeader : virtual public CTOHeader
	{
		//virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC);
	};
	friend class CTOWeekViewDayHeader;

	// === WeekViewDay ===
	class _XTP_EXT_CLASS CTOWeekViewDay : virtual public CTODay
	{
	public:
		virtual void AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect);
		//virtual void Draw(CCmdTarget* pObject, CDC* pDC){};

		DECLARE_THEMEPART_MEMBER_BASE(CTOWeekViewDayHeader,       Header);
		DECLARE_THEMEPART_MEMBER_BASE(CTOWeekViewEvent_MultiDay,  MultiDayEvent);
		DECLARE_THEMEPART_MEMBER_BASE(CTOWeekViewEvent_SingleDay, SingleDayEvent);

		virtual int GetDayCol(CXTPCalendarWeekViewDay* pWVDay); // returns {0, 1}
	};
	friend class CTOWeekViewDay;

	// ===== WeekView theme part object =====
	class _XTP_EXT_CLASS CTOWeekView : virtual public CXTPCalendarThemePart
	{
	public:
		virtual void AdjustLayout(CDC* pDC, const CRect& rcRect, BOOL bCallPostAdjustLayout);
		virtual void Draw(CDC* pDC);

		//DECLARE_THEMEPART_MEMBER_BASE(CTOHeader, Header);
		DECLARE_THEMEPART_MEMBER_BASE(CTOWeekViewEvent,         Event);
		DECLARE_THEMEPART_MEMBER_BASE(CTOWeekViewDay,           Day);
	};
	friend class CTOWeekView;

	//=======================================================================

	/////////////////////////////////////////////////////////////////////////
	class _XTP_EXT_CLASS CTOColorsSet : virtual public CXTPCalendarThemePart
	{
	public:
		CTOColorsSet();
		virtual void RefreshMetrics(BOOL bRefreshChildren = TRUE);

		virtual COLORREF GetColor(int eCLR, COLORREF clrBase = (COLORREF)-1);
		virtual int GetScale(int eCLR);

		virtual void SetColor(int eCLR, int nScale);
		virtual void SetCLR_DarkenOffset(int nCLR_DarkenOffset);
	protected:
		COLORREF m_clrBase;
		int m_nCLR_DarkenOffset;

		typedef CMap<int, int, int, int> CScalesMap;
		CScalesMap m_mapScale;
	};


	/////////////////////////////////////////////////////////////////////////
	//
	// *** Common Theme members ***
	//
	/////////////////////////////////////////////////////////////////////////

	// This base settings which used to initialize other settings by default.
	CXTPPaintManagerColor       m_clrBaseColor;
	CXTPCalendarThemeFontValue  m_fntBaseFont;
	CXTPCalendarThemeFontValue  m_fntBaseFontBold;

	DECLARE_THEMEPART_MEMBER_BASE(CTOColorsSet, ColorsSet)
	virtual COLORREF GetColor(int eCLR);

	DECLARE_THEMEPART_MEMBER_BASE(CTOHeader,    Header)
	DECLARE_THEMEPART_MEMBER_BASE(CTOEvent,     Event)
	//==========================================================================
	DECLARE_THEMEPART_MEMBER_BASE(CTODayView,   DayView)
	DECLARE_THEMEPART_MEMBER_BASE(CTOWeekView,  WeekView)
	DECLARE_THEMEPART_MEMBER_BASE(CTOMonthView, MonthView)

	virtual void RefreshMetrics();
	virtual void DoPropExchange(CXTPPropExchange* pPX);

	virtual CXTPImageManager* GetCustomIconsList() const;
	virtual CXTPOffice2007ImageList* GetImageList() const;

	virtual DWORD GetBeforeDrawFlags() const;
	virtual void SetBeforeDrawFlags(DWORD dwBeforeDrawFlags);
	virtual BOOL IsBeforeDraw(int eFlag) const;

	virtual DWORD GetAskItemTextFlags() const;
	virtual void SetAskItemTextFlags(DWORD dwFlags);

public:
	static BOOL IsXPTheme();

	// draw a single line text in the center of the rect.
	// if rect width is not enough to draw all chars -
	// text is aligned to left (or right, see nFormat) or the rect.
	// nFormat = 0 or combination of flags: DT_VCENTER, DT_LEFT, DT_RIGHT.
	static CSize DrawLine_CenterLR(CDC* pDC, LPCTSTR pcszText, CRect& rcRect, UINT nFormat);

	// draw text1 and text2 as single line using different fonts.
	// See Also: DrawLine_CenterLR
	static CSize DrawLine2_CenterLR(CDC* pDC, LPCTSTR pcszText1, LPCTSTR pcszText2,
						   CThemeFontColorSet* pFontColor1, CThemeFontColorSet* pFontColor2,
						   CRect& rcRect, UINT nShortAlign = DT_VCENTER | DT_LEFT,
						   UINT nFullAlign = DT_VCENTER | DT_CENTER);

	// draw rect with round corners and fill it using grclrBk.
	static void DrawRoundRect(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,
						BOOL bBoldBorder, CXTPPaintManagerColorGradient& grclrBk);

	static void DrawRoundRect(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,
						BOOL bBoldBorder, CBrush* pBrushBk);

	// fill rect using grclrBk and draw rect sides specified by isBorders.
	// isBorders.top  = 0 - do not draw top side, 1 - draw;
	// isBorders.left = 0 - do not draw left side, 1 - draw;
	// ...
	static void DrawRectPartly(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,
						int nBorderWidth, CRect isBorders);

	static void DrawRectPartly(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,
						BOOL bBoldBorder, CXTPPaintManagerColorGradient& grclrBk,
						CRect isBorders);
						//const CRect& isBorders);

	static const int cnCornerSize;

public:
	static CXTPCalendarView::XTP_VIEW_LAYOUT& AFX_CDECL GetViewLayout(CXTPCalendarView* pView);
	static CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& AFX_CDECL GetViewDayLayout(CXTPCalendarViewDay* pViewDay);

	static CXTPCalendarDayView::XTP_DAY_VIEW_LAYOUT& AFX_CDECL GetDayViewLayout(CXTPCalendarDayView* pDayView);
	static CXTPCalendarDayViewTimeScale::XTP_TIMESCALE_LAYOUT& AFX_CDECL GetTimeScaleLayout(CXTPCalendarDayViewTimeScale* pTimeScale);

	static CXTPCalendarViewGroup::XTP_VIEW_GROUP_LAYOUT& AFX_CDECL GetViewGroupLayout(CXTPCalendarViewGroup* pViewGroup);
	static CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& AFX_CDECL GetDayViewGroupLayout(CXTPCalendarDayViewGroup* pDayViewGroup);

	static CXTPCalendarWeekView::XTP_WEEK_VIEW_LAYOUT& AFX_CDECL GetWeekViewLayout(CXTPCalendarWeekView* pWeekView);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to copy standard and custom values.
	// Remarks:
	//     The standard value is always copied from the specified source.
	//     The custom value copied from the source only if this object has
	//     no custom value and the source object has custom value set.
	//-----------------------------------------------------------------------
	static void AFX_CDECL CopySettings(CXTPPaintManagerColor& refDest, const CXTPPaintManagerColor& refSrc);
	static void AFX_CDECL CopySettings(CXTPPaintManagerColorGradient& refDest, const CXTPPaintManagerColorGradient& refSrc); // <combine CopySettings@CXTPPaintManagerColor&@const CXTPPaintManagerColor&>

	static CFont* AFX_CDECL GetMaxHeightFont(CArray<CFont*, CFont*>& arFonts, CDC* pDC = NULL, int* pnMaxHeight = NULL);

public:
	virtual CXTPCalendarViewEventSubjectEditor* StartEditSubject(CXTPCalendarViewEvent* pViewEvent) {return NULL;};
	virtual void RemoveBoldAttrFromStd(CXTPCalendarThemeFontValue& rFont);

	// for internal usage
	enum XTPEnumThemeStdImages
	{
		xtpIMG_DayViewHeader            = -100,
		xtpIMG_DayViewHeaderTooday      = -101,
		xtpIMG_DayViewGroupHeader       = -102,

		xtpIMG_MonthViewDayHeader       = -120,
		xtpIMG_MonthViewDayHeaderToday  = -121,

		//xtpIMG_MonthViewWeekDayHeader   = 0, //-122, // the base color is enough for default WeekDay header
		xtpIMG_MonthViewWeekHeader      = -123,

		xtpIMG_WeekViewDayHeader        = -130,

		xtpIMG_ExpandDayButtons         = -140,
	};

protected:
	virtual void RefreshMetrics(BOOL bRefreshChildren);
	virtual void _DoPropExchange(CXTPPropExchange* pPX, BOOL bExchangeChildren);

	static void _DrawRoundRect(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,BOOL bBoldBorder,
							   CXTPPaintManagerColorGradient* pgrclrBk, CBrush* pBrushBk);

	virtual BOOL _LoadStdBitmap(int nBmpID, LPCTSTR pcszStdBmpName);

public:

	virtual CRect DrawExpandDayButton(CDC* pDC, const CRect& rc, BOOL bUp, BOOL bHot);


	virtual BOOL ExpandDayButton_HitTest(const CPoint& pt, const CRect& rc, BOOL bUp);
	virtual CSize GetExpandDayButtonSize();

protected:
	CXTPImageManager* m_pCustomIcons;
	CXTPOffice2007ImageList*    m_pImageList;

	CXTPOffice2007Images*       m_pImagesStd;

	DWORD   m_dwBeforeDrawFlags;
	DWORD   m_dwAskItemTextFlags;

protected:


};
//}}AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE BOOL CXTPCalendarThemePart::IsCreated() {
	return !!m_pTheme;
}

AFX_INLINE CXTPCalendarControl* CXTPCalendarTheme::GetCalendarControl() {
	return m_pCalendar;
}

AFX_INLINE void CXTPCalendarTheme::SetCalendarControl(CXTPCalendarControl* pCalendar) {
	m_pCalendar = pCalendar;
}

AFX_INLINE void CXTPCalendarTheme::RefreshMetrics() {
	RefreshMetrics(TRUE);
}

AFX_INLINE COLORREF CXTPCalendarTheme::GetColor(int eCLR) {
	return GetColorsSetPart()->GetColor(eCLR);
}

AFX_INLINE void CXTPCalendarTheme::CTOColorsSet::SetColor(int eCLR, int nScale) {
	m_mapScale[eCLR] = nScale;
}

AFX_INLINE void CXTPCalendarTheme::CTOColorsSet::SetCLR_DarkenOffset(int nCLR_DarkenOffset) {
	m_nCLR_DarkenOffset = nCLR_DarkenOffset;
}

AFX_INLINE DWORD CXTPCalendarTheme::GetBeforeDrawFlags() const {
	return m_dwBeforeDrawFlags;
}

AFX_INLINE BOOL CXTPCalendarTheme::IsBeforeDraw(int eFlag) const {
	return 0 != (m_dwBeforeDrawFlags & eFlag);
}

AFX_INLINE void CXTPCalendarTheme::SetBeforeDrawFlags(DWORD dwBeforeDrawFlags) {
	m_dwBeforeDrawFlags = dwBeforeDrawFlags;
}

AFX_INLINE DWORD CXTPCalendarTheme::GetAskItemTextFlags() const {
	return m_dwAskItemTextFlags;
}

AFX_INLINE void CXTPCalendarTheme::SetAskItemTextFlags(DWORD dwFlags) {
	m_dwAskItemTextFlags = dwFlags;
}

AFX_INLINE BOOL CXTPCalendarTheme::IsXPTheme()
{
	switch (XTPColorManager()->GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
	case xtpSystemThemeOlive:
	case xtpSystemThemeSilver:
		return TRUE;
	}
	return FALSE;
}

AFX_INLINE CXTPImageManager* CXTPCalendarTheme::GetCustomIconsList() const {
	return m_pCustomIcons;
}

AFX_INLINE CXTPOffice2007ImageList* CXTPCalendarTheme::GetImageList() const {
	return m_pImageList;
}

//*********************************
#if (_MSC_VER > 1100)
#pragma warning(pop)
#endif

#endif // !defined(_XTP_CALENDAR_THEME_H__)
