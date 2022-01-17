// XTPCalendarDayViewTimeScale.cpp: implementation of the CXTPCalendarDayViewTimeScale class.
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
#include <math.h>

#include "Common/XTPResourceManager.h"
#include "Common/XTPVC50Helpers.h"

#include "XTPCalendarDayViewTimeScale.h"
#include "XTPCalendarDayView.h"
#include "XTPCalendarOptions.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPCalendarDayViewTimeScale, CCmdTarget)

CXTPCalendarDayViewTimeScale::CXTPCalendarDayViewTimeScale(CXTPCalendarDayView* pDayView, BOOL bVisible, BOOL bDrawNowLine)
		: m_pDayView(pDayView), m_bVisible(bVisible), m_bDrawNowLine(bDrawNowLine)
{
	m_Layout.m_rcHeader.SetRectEmpty();
	m_Layout.m_rcTimeZone.SetRectEmpty();
	m_Layout.m_nXPosBase = 0;
	m_nTimeshift_min = 0;

	m_nWidth = 48; // default value for default WinXP settings.
	               // Actual width to be recalculated later on call of CalcWidth.

	m_bDrawNowLineDyn = FALSE;
}

int CXTPCalendarDayViewTimeScale::CalcWidth(CDC* pDC)
{
	// calculate actual correct width
	int nWidth1 = m_nWidth, nWidth2 = m_nWidth;

	// calculate width for midnight
	COleDateTime dtMid(0, 0);
	CString strHour, strMin;
	Format(dtMid, TRUE, &strHour, &strMin);

	COleDateTimeSpan spScaleInterval = GetScaleInterval();
	int nRowHeight = XTP_SAFE_GET1(m_pDayView, GetRowHeight(), 1);
	int nHourHeight = nRowHeight * (GETTOTAL_MINUTES_DTS(spScaleInterval) < 60 ? 2 : 1);

	XTP_SAFE_CALL3(m_pDayView, GetPaintManager(), GetDayViewTimeScaleCellPart(),
		CalcWidth(pDC, strHour, strMin, nHourHeight, nWidth1));

	// calculate width for midday
	dtMid += COleDateTimeSpan(0, 12, 0, 0);
	Format(dtMid, TRUE, &strHour, &strMin);

	XTP_SAFE_CALL3(m_pDayView, GetPaintManager(), GetDayViewTimeScaleCellPart(),
		CalcWidth(pDC, strHour, strMin, nHourHeight, nWidth2));

	// choose the widest one
	m_nWidth = max(nWidth1, nWidth2);

	return m_nWidth;
}

void CXTPCalendarDayViewTimeScale::Draw(CDC* pDC)
{
	ASSERT(pDC && m_pDayView);

	if (!pDC || !m_pDayView)
	{
		return;
	}

	pDC->FillSolidRect(m_Layout.m_rcHeader, GetSysColor(COLOR_3DFACE));
	pDC->SetBkMode(TRANSPARENT);

	int nRowHeight = XTP_SAFE_GET1(m_pDayView, GetRowHeight(), 0);

	if (nRowHeight <= 0)
		return;

	const int nRowCount = m_pDayView->GetVisibleRowCount();
	const int nTopRow = m_pDayView->GetTopRow();
	COleDateTimeSpan spScaleInterval = GetScaleInterval();

	if (GETTOTAL_MINUTES_DTS(spScaleInterval) < 1)
	{
		ASSERT(FALSE);
		spScaleInterval.SetDateTimeSpan(0, 0, 5, 0);
	}

	//=== Calculate 'now' line. Draw it's BK ================================
	int nNowLineY = 0;
	if (m_bDrawNowLineDyn)
	{
		int nBottomRow = nTopRow + m_pDayView->GetVisibleRowCount();
		COleDateTime dtTopTime = m_pDayView->GetCellTime(nTopRow);
		COleDateTime dtBottomTime = m_pDayView->GetCellTime(nBottomRow);
		COleDateTimeSpan spView = dtBottomTime - dtTopTime;

		COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
		dtNow = CXTPCalendarUtils::ResetDate(dtNow);

		COleDateTimeSpan spNow = dtNow - dtTopTime;

		double dPixelPerSecond = m_Layout.m_rcTimeZone.Height() / (double)GETTOTAL_SECONDS_DTS(spView);

		int nDlta = int(GETTOTAL_SECONDS_DTS(spNow) * dPixelPerSecond);
		nNowLineY = m_Layout.m_rcTimeZone.top + nDlta;

		XTP_SAFE_CALL3(m_pDayView, GetPaintManager(), GetDayViewTimeScaleHeaderPart(),
			DrawNowLine(pDC, m_pDayView, m_Layout.m_rcTimeZone, nNowLineY, TRUE) );
	}

	//================================================================
	int nTimeShiftCorrectorY = 0;
	COleDateTimeSpan spShiftCorrector(0);
	int nScaleInterval_min = max(1, (int)GETTOTAL_MINUTES_DTS(spScaleInterval));
	double dPixelPerMin = (double)nRowHeight / (double)nScaleInterval_min;

	if (GETTOTAL_HOURS_DTS(spScaleInterval) < 1)
	{
		int nMul = m_nTimeshift_min / 60;
		int nMod = m_nTimeshift_min % 60;
		int nShiftCorrector_min = (nMul + (nMod ? 1 :0))*60 - m_nTimeshift_min;

		//-------------------------------------------------------------------
		int nMul2 = nShiftCorrector_min / nScaleInterval_min;
		int nMod2 = nShiftCorrector_min % nScaleInterval_min;
		int nShiftCorrector2_min = (nMul2 + (nMod2 ? 1 :0)) * nScaleInterval_min;

		nTimeShiftCorrectorY = (int)(dPixelPerMin * nShiftCorrector2_min);

		spShiftCorrector = CXTPCalendarUtils::Minutes2Span(nShiftCorrector_min);
	}

	// draw time scale header
	COleDateTimeSpan spShift = CXTPCalendarUtils::Minutes2Span(m_nTimeshift_min);

	// draw time scale rows
	if (GETTOTAL_HOURS_DTS(spScaleInterval) >= 1)
	{
		// draw time scale cells one by one
		for (int i = 0; i < nRowCount; i++)
		{
			CRect rcCell(m_Layout.m_rcTimeZone.left,
						m_Layout.m_rcTimeZone.top + nRowHeight * i,
						m_Layout.m_rcTimeZone.right,
						m_Layout.m_rcTimeZone.top + nRowHeight * (i + 1));

			COleDateTime dtTime = m_pDayView->GetCellTime(nTopRow + i);

			dtTime += spShift;
			CString str = Format(dtTime, FALSE);

			XTP_SAFE_CALL3(m_pDayView, GetPaintManager(), GetDayViewTimeScaleCellPart(),
							DrawHourCell(pDC, rcCell, str, FALSE));
		}
	}
	else
	{
		COleDateTimeSpan spHour(0, 1, 0, 0);

		spShift += spShiftCorrector;

		int nRowPerHour = (int)((double)spHour / (double)spScaleInterval + XTP_HALF_SECOND);
		ASSERT(nRowPerHour > 0);
		nRowPerHour = max(1, nRowPerHour);
		const int nHourCellHeight = nRowHeight*nRowPerHour;

		// Adjust time font size
		CRect rcStartCell(m_Layout.m_rcTimeZone);

		rcStartCell.top += nTimeShiftCorrectorY;

		rcStartCell.bottom = rcStartCell.top + nHourCellHeight;

		//XTP_SAFE_CALL3(m_pDayView, GetPaintManager(), GetDayViewTimeScaleCellPart(),
		//              AdjustTimeFont(pDC, rcStartCell));

		//-------------------------------------------------------------------
		int nHrCellDiv = m_Layout.m_rcTimeZone.Height() / rcStartCell.Height();
		int nHrCellMod = m_Layout.m_rcTimeZone.Height() / rcStartCell.Height();

		int nDrawCellCount = nHrCellDiv + (nHrCellMod ? 1 :0);

		COleDateTime dtStartCell = m_pDayView->GetCellTime(nTopRow);
		dtStartCell += spShift;
		int nSCMinute = dtStartCell.GetMinute();
		if (nSCMinute != 0)
		{
			int nMinutesShiftY = (int)(dPixelPerMin * nSCMinute);
			rcStartCell.top -= nMinutesShiftY;
			rcStartCell.bottom -= nMinutesShiftY;

			dtStartCell.SetTime(dtStartCell.GetHour(), 0, 0);

			nDrawCellCount++;
		}

		//while (rcStartCell.top > m_rcTimeZone.top)
		for (int n = 0; rcStartCell.top > m_Layout.m_rcTimeZone.top && n < 100; n++)
		{
			dtStartCell -= spHour;

			rcStartCell.top -= nHourCellHeight;
			rcStartCell.bottom -= nHourCellHeight;

			nDrawCellCount++;
		}

		// draw time scale cells with only one big hour number
		BOOL bTopCell = TRUE;
		CRect rcCell = rcStartCell;
		for (int i = 0; i < nDrawCellCount; i++)
		{
			COleDateTime dtCell = dtStartCell + COleDateTimeSpan(i * (double)spHour);
			ASSERT(dtCell.GetMinute() == 0);

			CString strHour, strMin;
			Format(dtCell, bTopCell, &strHour, &strMin);

			if (rcCell.top >= m_Layout.m_rcTimeZone.top)
			{
				bTopCell = FALSE;
			}

			// draw text
			XTP_SAFE_CALL3(m_pDayView, GetPaintManager(), GetDayViewTimeScaleCellPart(),
							DrawBigHourCell(pDC, rcCell, strHour, strMin, nRowPerHour, FALSE));

			rcCell.top += nHourCellHeight;
			rcCell.bottom += nHourCellHeight;
		}
	}

	//*** draw 'now' line
	if (m_bDrawNowLineDyn)
	{
		XTP_SAFE_CALL3(m_pDayView, GetPaintManager(), GetDayViewTimeScaleHeaderPart(),
			DrawNowLine(pDC, m_pDayView, m_Layout.m_rcTimeZone, nNowLineY, FALSE) );
	}

	//*** Draw Expand Signs
	XTP_SAFE_CALL3(m_pDayView, GetPaintManager(), GetDayViewTimeScaleHeaderPart(),
			DrawExpandSigns(pDC, m_pDayView, m_Layout.m_rcTimeZone) );

	//*** Draw header
	CRect rcHeader(m_Layout.m_rcHeader.left, m_Layout.m_rcHeader.top, m_Layout.m_rcHeader.right, m_Layout.m_rcTimeZone.top);

	XTP_SAFE_CALL3(m_pDayView, GetPaintManager(), GetDayViewTimeScaleHeaderPart(),
					OnDraw(pDC, m_pDayView, rcHeader, m_strCaption));
}

CString CXTPCalendarDayViewTimeScale::Format(COleDateTime dtTime, BOOL bTopCell,
											 CString* pHour, CString* pMin)
{
	long localeItlZero = CXTPCalendarUtils::GetLocaleLong(LOCALE_ITLZERO);
	long localeITime = CXTPCalendarUtils::GetLocaleLong(LOCALE_ITIME);

	CString strHour = localeITime == 0 ? dtTime.Format(_T("%I")) : dtTime.Format(_T("%H"));

	if ((strHour.GetLength() == 2) && (strHour[0] == _T('0')) && (localeItlZero == 0))
	{
		DELETE_S(strHour, 0, 1);
	}

	CString strTime = dtTime.Format(_T("%M"));

	if ((localeITime == 0) && ((bTopCell || strHour == _T("12")) && dtTime.GetMinute() == 0))
	{
		CString localeAmPm = CXTPCalendarUtils::GetLocaleString(dtTime.GetHour() < 12 ? LOCALE_S1159 : LOCALE_S2359, 20);
		localeAmPm.MakeLower();

		if (pHour)
		{
			*pHour = strHour;
		}

		if (pMin)
		{
			*pMin = localeAmPm;
		}
		strHour += _T(" ");
		strHour += localeAmPm;

		return strHour;
	}

	if (pHour)
	{
		*pHour = strHour;
	}
	if (pMin)
	{
		*pMin = strTime;
	}

	return strHour + CXTPCalendarUtils::GetLocaleString(LOCALE_STIME, 5) + strTime;
}

CXTPCalendarDayViewTimeScale::~CXTPCalendarDayViewTimeScale()
{
}

void CXTPCalendarDayViewTimeScale::MoveTo(int nXPos)
{
	int nWidth = m_Layout.m_rcHeader.Width();

	m_Layout.m_rcHeader.left = m_Layout.m_nXPosBase + nXPos;
	m_Layout.m_rcHeader.right = m_Layout.m_rcHeader.left + nWidth;

	m_Layout.m_rcTimeZone.left = m_Layout.m_rcHeader.left;
	m_Layout.m_rcTimeZone.right = m_Layout.m_rcHeader.right;
}

void CXTPCalendarDayViewTimeScale::AdjustLayout(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);

	ASSERT(pDC);
	ASSERT(m_pDayView);
	if (!m_pDayView || !pDC)
	{
		return;
	}

	CRect rcAllDayEvents = m_pDayView->GetAllDayEventsRectangle();

	m_Layout.m_nXPosBase = m_Layout.m_rcHeader.left;

	m_Layout.m_rcTimeZone = m_Layout.m_rcHeader;
	m_Layout.m_rcTimeZone.top = rcAllDayEvents.bottom;

	//----------------------------------------------------
	int nNowLineFlags = XTP_SAFE_GET3(m_pDayView, GetCalendarControl(), GetCalendarOptions(), nDayView_CurrentTimeMarkVisible, 0);

	m_bDrawNowLineDyn = m_bDrawNowLine;
	if (m_bDrawNowLineDyn)
	{
		if (m_pDayView->IsTodayVisible())
		{
			int nTAMask = xtpCalendarCurrentTimeMarkVisibleForToday | xtpCalendarCurrentTimeMarkVisibleAlways;
			m_bDrawNowLineDyn = m_bDrawNowLineDyn && (nNowLineFlags & nTAMask);
		}
		else
		{
			m_bDrawNowLineDyn = m_bDrawNowLineDyn && (nNowLineFlags & xtpCalendarCurrentTimeMarkVisibleAlways);
		}

		if (pDC->IsPrinting())
		{
			m_bDrawNowLineDyn = m_bDrawNowLineDyn && (nNowLineFlags & xtpCalendarCurrentTimeMarkPrinted);
		}
	}
}


BOOL CXTPCalendarDayViewTimeScale::HitTestDateTime(XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pHitTest)
{
	if (!pHitTest || !m_pDayView)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nVisibleRowCount = m_pDayView->GetVisibleRowCount();

	if (m_Layout.m_rcTimeZone.PtInRect(pHitTest->pt) && nVisibleRowCount > 0)
	{
		int nCell = (pHitTest->pt.y - m_Layout.m_rcTimeZone.top) / max(1, m_pDayView->GetRowHeight());

		if (nCell >= 0 && nCell < nVisibleRowCount)
		{
			pHitTest->dt = m_pDayView->GetCellTime(nCell + m_pDayView->GetTopRow());
			pHitTest->uHitCode |= xtpCalendarHitTestDayViewTimeScale;

			return TRUE;
		}

	}
	return FALSE;
}

COleDateTimeSpan CXTPCalendarDayViewTimeScale::GetScaleInterval() const
{
	return XTP_SAFE_GET1(m_pDayView, GetScaleInterval(), COleDateTimeSpan(0, 0, 1, 0));
}

//////////////////////////////////////////////////////////////////////////
// CXTPCalendarTimeScalePropertiesDlg dialog

CXTPCalendarTimeScalePropertiesDlg::CXTPCalendarTimeScalePropertiesDlg(CWnd* pParent)
{
	InitModalIndirect(XTPResourceManager()->LoadDialogTemplate(IDD), pParent);

	m_bShowScale2 = FALSE;

	TIME_ZONE_INFORMATION m_tziScale2;
	::ZeroMemory(&m_tziScale2, sizeof(m_tziScale2));

	VERIFY(::GetTimeZoneInformation(&m_tziScale2) != TIME_ZONE_ID_INVALID);

	m_bAutoAdjustDayLight2 = FALSE;
}

void CXTPCalendarTimeScalePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, XTP_IDC_LABEL1, m_strCaption1);
	DDX_Text(pDX, XTP_IDC_LABEL2, m_strCaption2);

	DDX_Check(pDX, XTP_IDC_CHECK_ADDITIONAL_TIMEZONE, m_bShowScale2);
	DDX_Check(pDX, XTP_IDC_CHECK_ADJUST_DAYLIGHT_TIMEZONE2, m_bAutoAdjustDayLight2);

	DDX_Control(pDX, XTP_IDC_LABEL1, m_edtCaption1);
	DDX_Control(pDX, XTP_IDC_LABEL2, m_edtCaption2);

	DDX_Control(pDX, XTP_IDC_COMBO_TIMEZONE1, m_cmbZones1);
	DDX_Control(pDX, XTP_IDC_COMBO_TIMEZONE2, m_cmbZones2);

	DDX_Control(pDX, XTP_IDC_CHECK_ADJUST_DAYLIGHT_TIMEZONE1, m_btnAutoAdjustDayLight1);
	DDX_Control(pDX, XTP_IDC_CHECK_ADJUST_DAYLIGHT_TIMEZONE2, m_btnAutoAdjustDayLight2);
}

void CXTPCalendarTimeScalePropertiesDlg::OnBnClickedScale2()
{
	UpdateData();

	m_edtCaption2.EnableWindow(m_bShowScale2);
	m_cmbZones2.EnableWindow(m_bShowScale2);
	m_btnAutoAdjustDayLight2.EnableWindow(m_bShowScale2);

	if(m_bShowScale2) {
		OnSelchangeZones2();
	}
}

BEGIN_MESSAGE_MAP(CXTPCalendarTimeScalePropertiesDlg, CDialog)
	ON_BN_CLICKED(XTP_IDC_CHECK_ADDITIONAL_TIMEZONE, OnBnClickedScale2)
	ON_CBN_SELCHANGE(XTP_IDC_COMBO_TIMEZONE2, OnSelchangeZones2)
END_MESSAGE_MAP()


BOOL CXTPCalendarTimeScalePropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//===========================================================================
	if(!m_arTimeZones.InitFromRegistry()) {
		ASSERT(FALSE);
		return TRUE;
	}

	TIME_ZONE_INFORMATION tziCurrent;
	::ZeroMemory(&tziCurrent, sizeof(tziCurrent));

	if(::GetTimeZoneInformation(&tziCurrent) != TIME_ZONE_ID_INVALID)
	{
		CXTPCalendarTimeZone* pTZIcurr;
		pTZIcurr = m_arTimeZones.Find(&tziCurrent);

		if(pTZIcurr)
		{
			CString strGMT = pTZIcurr->GetDisplayString();
			m_cmbZones1.SetWindowText(strGMT);
		}

		BOOL bIsAutoAdjustDT = tziCurrent.StandardBias != 0 || tziCurrent.DaylightBias != 0;
		m_btnAutoAdjustDayLight1.SetCheck(bIsAutoAdjustDT);

		m_cmbZones1.EnableWindow(FALSE);
		m_btnAutoAdjustDayLight1.EnableWindow(FALSE);
	}
	else
	{
		ASSERT(FALSE);
	}

	int nSelIndex2 = 0;
	int nCount = m_arTimeZones.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		CXTPCalendarTimeZone* pTZI2;
		pTZI2 = m_arTimeZones.GetAt(i);
		if(pTZI2)
		{
			CString strGMT = pTZI2->GetDisplayString();
			int nIdx = m_cmbZones2.AddString(strGMT);
			m_cmbZones2.SetItemDataPtr(nIdx, pTZI2);

			if(pTZI2->IsEqual(&m_tziScale2)) {
				nSelIndex2 = i;
			}
		}
	}
	m_cmbZones2.SetCurSel(nSelIndex2);

	m_bAutoAdjustDayLight2 = m_tziScale2.StandardBias != 0 ||
							 m_tziScale2.DaylightBias != 0;

	// update data
	UpdateData(FALSE);

	OnBnClickedScale2();

	return TRUE;
}

void CXTPCalendarTimeScalePropertiesDlg::OnOK()
{
	UpdateData();

	if(m_bShowScale2)
	{
		int nSelIndex2 = m_cmbZones2.GetCurSel();
		if(nSelIndex2 != CB_ERR)
		{
			CXTPCalendarTimeZone* pTZI2;
			pTZI2 = (CXTPCalendarTimeZone*)m_cmbZones2.GetItemData(nSelIndex2);
			if(pTZI2) {
				m_tziScale2 = *pTZI2;
			}
		}

		if(!m_bAutoAdjustDayLight2)
		{
			m_tziScale2.StandardBias    = 0;
			m_tziScale2.DaylightBias    = 0;
		}
	}

	CDialog::OnOK();
}

void CXTPCalendarTimeScalePropertiesDlg::OnSelchangeZones2()
{
	int nSelIndex2 = m_cmbZones2.GetCurSel();

	if (nSelIndex2 == CB_ERR)
		return;

	CXTPCalendarTimeZone* pTZI2;
	pTZI2 = (CXTPCalendarTimeZone*)m_cmbZones2.GetItemData(nSelIndex2);

	if(!pTZI2)
		return;

	if (pTZI2->StandardDate.wMonth == 0 && pTZI2->DaylightDate.wMonth == 0)
	{
		m_btnAutoAdjustDayLight2.EnableWindow(FALSE);
		m_btnAutoAdjustDayLight2.SetCheck(0);
	}
	else
	{
		m_btnAutoAdjustDayLight2.EnableWindow(TRUE);
	}
}
