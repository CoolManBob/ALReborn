// XTPCalendarDayViewTimeScale.h: interface for the CXTPCalendarDayViewTimeScale class.
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
#if !defined(_XTPCALENDARDAYVIEWTIMESCALE_H__)
#define _XTPCALENDARDAYVIEWTIMESCALE_H__
//}}AFX_CODEJOCK_PRIVATE

#include "Resource.h"
#include "XTPCalendarTimeZoneHelper.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct XTP_CALENDAR_HITTESTINFO_DAY_VIEW;
class CXTPCalendarDayView;

//===========================================================================
// Summary:
//     This class implements a specific view portion of the Calendar
//     Day view - called <b>Time scale</b>.
// Remarks:
//     The Day view contains two time scales defining those intervals
//     at the left of the view, which is represented by the
//     CXTPCalendarDayViewTimeScale class. One time scale is the main and
//     is visible by default, second one is the alternative and could be
//     enabled programmatically. Time scale interval could be changed
//     using SetScaleInterval member function.
//
// See Also: CXTPCalendarDayViewDay
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarDayViewTimeScale : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarDayViewTimeScale)
	friend class CXTPCalendarDayView;
	friend class CXTPCalendarTheme;
	//}}AFX_CODEJOCK_PRIVATE

public:
	// ---------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//      pDayView :      Pointer to associated CXTPCalendarDayView object.
	//      bVisible :      Specifies initial visibility mode for the time
	//                      scale.
	//      bDrawNowLine :  If TRUE current time mark is drawn.
	// See Also:
	//     ~CXTPCalendarDayViewTimeScale()
	// ---------------------------------------------------------------------
	CXTPCalendarDayViewTimeScale(CXTPCalendarDayView* pDayView, BOOL bVisible = TRUE, BOOL bDrawNowLine = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	// See Also: CXTPCalendarDayViewTimeScale
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarDayViewTimeScale();


	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the time scale width in pixels.
	// Parameters:
	//     pDC     - Pointer to a valid device context.
	// Returns:
	//     An int that contains the time scale width in pixels.
	//-----------------------------------------------------------------------
	int CalcWidth(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get the time scale width.
	// Returns:
	//     An int that contains the time scale width in pixels.
	//-----------------------------------------------------------------------
	int GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to perform all drawing logic of
	//     the time scale.
	// Parameters:
	//     pDC - A pointer to a valid device context.
	//-----------------------------------------------------------------------
	void Draw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function is used to adjust the time scale layout.
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to format the time scale text.
	// Parameters:
	//     dtTime   - A COleDateTime object that contains the time to format to text.
	//     bTopCell - If TRUE and minutes value is 0 the minutes
	//                value is replaced on am/pm value. This valid only for
	//                AM/PM 12-hour time format. If this parameter FALSE, the
	//                minutes value 0 is replace only if hour value is 12.
	//     pHour    - A pointer to the CString object to write the hour part of the value.
	//     pMin     - A pointer to the CString object to write the minutes part of the value.
	// Returns:
	//     A string containing formatted text.
	//-----------------------------------------------------------------------
	virtual CString Format(COleDateTime dtTime, BOOL bTopCell,
							CString* pHour = NULL, CString* pMin = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine is Current Time Mark
	//     can be drawn for this time scale.
	// Remarks:
	//     By default it returns TRUE for primary time scale and
	//     FALSE for secondary.
	// Returns:
	//     Returns TRUE if Current Time Mark may be drawn for this time scale,
	//     FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsDrawNowLine() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function obtains the time scale cell interval.
	// Remarks:
	//     Use this member function to retrieve the current time scale
	//     cell interval.
	// Returns:
	//     A COleDateTimeSpan object that contains the cell interval for
	//     the time scale.
	//-----------------------------------------------------------------------
	COleDateTimeSpan GetScaleInterval() const;


	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the text caption for the
	//     time scale.
	// Parameters:
	//     strCaption - A CString object that contains the new text
	//                  caption for the time scale.
	// Remarks:
	//     Use this function to set the current time scale caption.
	// See Also: GetCaption
	//-----------------------------------------------------------------------
	void SetCaption(LPCTSTR strCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the text caption for
	//     the time scale.
	// Remarks:
	//     Use GetCaption() to get the current time scale caption.
	// Returns:
	//     A CString object that contains the text caption for the time scale.
	// See Also: SetCaption
	//-----------------------------------------------------------------------
	CString GetCaption();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new time shift in
	//     hours for the time scale.
	// Parameters:
	//     nMinutes - An int that contains the new time shift for the
	//                time scale in minutes.
	// Remarks:
	//     Use this member function to set the new time shift for
	//     the time scale.
	// See Also: GetTimeshift
	//-----------------------------------------------------------------------
	void SetTimeshift(int nMinutes);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the time shift in
	//     hours for the time scale.
	// Remarks:
	//     Use this member function to retrieve current time shift for
	//     the time scale.
	// Returns:
	//     An int that contains the time shift for the time scale in minutes.
	// See Also: SetTimeshift
	//-----------------------------------------------------------------------
	int GetTimeshift();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the time scale visible
	//     mode (TRUE - visible, FALSE - invisible).
	// Parameters:
	//     bVisible - A BOOL that contains the new time scale visible state.
	//                TRUE -  Visible.
	//                FALSE - Invisible.
	// See Also: GetVisible()
	//-----------------------------------------------------------------------
	void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the current time scale
	//     visibility mode.
	// Returns:
	//     A BOOL that contains the time scale visibility scale value.
	//     TRUE if the time scale is visible.
	//     FALSE otherwise.
	// See Also: SetVisible(BOOL)
	//-----------------------------------------------------------------------
	BOOL GetVisible();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill an
	//     XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Parameters:
	//     pHitTest - A pointer to an XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Remarks:
	//     Call this member function to gather hit test information from
	//     the day view.
	//     It uses pHitTest->pt point for gathering hit test information.
	// See Also: XTP_CALENDAR_HITTESTINFO_DAY_VIEW
	//-----------------------------------------------------------------------
	BOOL HitTestDateTime(XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pHitTest);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function obtains the time scale owner view.
	// Remarks:
	//     Use this member function to get day view object which owns the time
	//     scale.
	// Returns:
	//     A pointer to CXTPCalendarDayView object.
	//-----------------------------------------------------------------------
	CXTPCalendarDayView* GetDayView();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function shift the time scale object.
	// Parameters:
	//     nXPos - Offset from base position.
	// Remarks:
	//     Used by horizontal scrolling to place timescale always visible
	//     on the left border.
	//-----------------------------------------------------------------------
	void MoveTo(int nXPos);
public:
	//{{AFX_CODEJOCK_PRIVATE
	struct XTP_TIMESCALE_LAYOUT
	{
		CRect m_rcHeader;                // Time scale header rectangle.
		CRect m_rcTimeZone;              // Time zone rectangle.
		int   m_nXPosBase;
	};
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	virtual XTP_TIMESCALE_LAYOUT& GetLayout();
	//}}AFX_CODEJOCK_PRIVATE

protected:
	XTP_TIMESCALE_LAYOUT m_Layout; // Store time scale layout.

	CXTPCalendarDayView* m_pDayView; // Pointer to the parent Calendar Day view object.
	BOOL m_bDrawNowLine;             // Specifies whether we should draw the "Now" line on the time scale.

	CString m_strCaption;               // Caption under time scale.
	int m_nTimeshift_min;               // Time zone time shift in minutes.
	BOOL m_bVisible;                    // Show this timescale or not.

private:
	int m_nWidth;            // Calculated time scale width.
	BOOL m_bDrawNowLineDyn;  // DrawNowLine attribute dynamically calculated on AdjustLayout.
};

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarTimeScalePropertiesDlg dialog
//
// This dialog allows to customize Calendar Time Scale part, set a proper
// time zone and it's description.
// It is a simplified version of similar Outlook dialog.
//
/////////////////////////////////////////////////////////////////////////////
class _XTP_EXT_CLASS CXTPCalendarTimeScalePropertiesDlg : public CDialog
{
public:
	enum { IDD = XTP_IDD_CALENDAR_TIMESCALE_PROPERTIES };

	CXTPCalendarTimeScalePropertiesDlg(CWnd* pParent = NULL);

public:
	CString m_strCaption1;  // [in, out]

	BOOL m_bShowScale2;     // [in, out]
	CString m_strCaption2;  // [in, out]

	TIME_ZONE_INFORMATION m_tziScale2; // [in, out]

protected:

	// Generated message map functions
	//{{AFX_CODEJOCK_PRIVATE
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeZones2();
	afx_msg void OnBnClickedScale2();
	//}}AFX_CODEJOCK_PRIVATE

	DECLARE_MESSAGE_MAP()

protected:
	CEdit       m_edtCaption1;          // An edit control for main time zone caption
	CComboBox   m_cmbZones1;            // A combobox to display main time zone
	CButton     m_btnAutoAdjustDayLight1;   // A check box control to show 'Auto Adjust Day Light' option for main timezone

	CEdit       m_edtCaption2;          // An edit control for additional time zone caption
	CComboBox   m_cmbZones2;            // A combobox to edit additional time zone
	CButton     m_btnAutoAdjustDayLight2;   // A check box control to edit 'Auto Adjust Day Light' option for additional timezone

	BOOL m_bAutoAdjustDayLight2;            // A BOOL member which used to edit 'Auto Adjust Day Light' option for additional timezone

	CXTPCalendarTimeZones m_arTimeZones;    // A collection of timezones.
};

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE void CXTPCalendarDayViewTimeScale::SetCaption(LPCTSTR strCaption) {
	m_strCaption = strCaption;
}

AFX_INLINE CString CXTPCalendarDayViewTimeScale::GetCaption() {
	return m_strCaption;
}

AFX_INLINE void CXTPCalendarDayViewTimeScale::SetTimeshift(int nMinutes) {
	m_nTimeshift_min = nMinutes;
}

AFX_INLINE int CXTPCalendarDayViewTimeScale::GetTimeshift() {
	return m_nTimeshift_min;
}

AFX_INLINE void CXTPCalendarDayViewTimeScale::SetVisible(BOOL bVisible) {
	m_bVisible = bVisible;
}

AFX_INLINE BOOL CXTPCalendarDayViewTimeScale::GetVisible() {
	return m_bVisible;
}

AFX_INLINE CXTPCalendarDayView* CXTPCalendarDayViewTimeScale::GetDayView() {
	return m_pDayView;
}

AFX_INLINE CXTPCalendarDayViewTimeScale::XTP_TIMESCALE_LAYOUT&
									CXTPCalendarDayViewTimeScale::GetLayout() {
	return m_Layout;
}

AFX_INLINE BOOL CXTPCalendarDayViewTimeScale::IsDrawNowLine() const {
	return m_bDrawNowLine;
}

AFX_INLINE int CXTPCalendarDayViewTimeScale::GetWidth() const {
	return m_Layout.m_rcHeader.Width();
}
#endif // !defined(_XTPCALENDARDAYVIEWTIMESCALE_H__)
