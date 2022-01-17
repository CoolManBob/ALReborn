// XTPCalendarTheme.cpp : implementation file
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
#include "Resource.h"

#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceManager.h"

#include "Common/XTPImageManager.h"
#include "Common/XTPOffice2007Image.h"
#include "Common/XTPPropExchange.h"

#include "XTPCalendarTheme.h"
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
//    Should be a prime number:
// 37, 53, 79 , 101, 127, 199, 503, 1021, 1511, 2003, 3001, 4001, 5003, 6007, 8009, 12007, 16001, 32003, 48017, 64007
#define XTP_CALENDAR_COLORS_SET_HASH_TABLE_SIZE 199

const int CXTPCalendarTheme::cnCornerSize = 7;

IMPLEMENT_DYNAMIC(CXTPCalendarThemePart, CCmdTarget)
IMPLEMENT_DYNAMIC(CXTPCalendarTheme, CXTPCalendarThemePart)

BOOL xtp_dbg_IsFontValid(HANDLE hFont)
{
	LOGFONT lfFnt;
	int nRes = GetObject(hFont, sizeof(lfFnt), &lfFnt);

	return nRes == sizeof(lfFnt);
}

//===========================================================================
CXTPCalendarTheme::CXTPCalendarTheme()
{
	m_pCalendar = NULL;

	m_pCustomIcons = new CXTPImageManager();
	m_pImageList = new CXTPOffice2007ImageList();

	m_pImagesStd = new CXTPOffice2007Images();


	m_dwBeforeDrawFlags = 0;
	m_dwAskItemTextFlags = 0;

	//DBG
//  #pragma XTPNOTE("DBG - m_dwBeforeDrawFlags")
//  m_dwBeforeDrawFlags = (DWORD)-1; //ALL set

//#ifdef _XTP_ACTIVEX
	//EnableAutomation();
	//EnableTypeLib();
//#endif
}

CXTPCalendarTheme::~CXTPCalendarTheme()
{
	SAFE_DELETE(m_pImagesStd);

	CMDTARGET_RELEASE(m_pCustomIcons);
	CMDTARGET_RELEASE(m_pImageList);
}

void CXTPCalendarTheme::Init()
{
	Create(this, this);
}

////////////////////////////////////////////////////////////////////////
void CXTPCalendarTheme::CopySettings(CXTPPaintManagerColor& refDest, const CXTPPaintManagerColor& refSrc)
{
	refDest.SetStandardValue((COLORREF)refSrc);
//  refDest.SetStandardValue(refSrc.GetStandardColor());

//  if (!refDest.IsCustomValue() && refSrc.IsCustomValue())
//  {
//      refDest.SetCustomValue(refSrc);
//  }
}

void CXTPCalendarTheme::CopySettings(CXTPPaintManagerColorGradient& refDest, const CXTPPaintManagerColorGradient& refSrc)
{
	CopySettings(refDest.clrLight, refSrc.clrLight);
	CopySettings(refDest.clrDark,  refSrc.clrDark);
}


////////////////////////////////////////////////////////////////////////
CXTPCalendarView::XTP_VIEW_LAYOUT& CXTPCalendarTheme::GetViewLayout(CXTPCalendarView* pView)
{
	ASSERT(pView);
	if (pView)
	{
		return pView->GetLayout_();
	}
	static CXTPCalendarView::XTP_VIEW_LAYOUT m_errLayout = {0};
	return m_errLayout;
}

CXTPCalendarDayView::XTP_DAY_VIEW_LAYOUT& CXTPCalendarTheme::GetDayViewLayout(CXTPCalendarDayView* pDayView)
{
	ASSERT(pDayView);
	if (pDayView)
	{
		return pDayView->GetLayout();
	}
	static CXTPCalendarDayView::XTP_DAY_VIEW_LAYOUT m_errLayoutX;
	return m_errLayoutX;
}

CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& CXTPCalendarTheme::GetViewDayLayout(CXTPCalendarViewDay* pViewDay)
{
	ASSERT(pViewDay);
	if (pViewDay)
	{
		return pViewDay->GetLayout_();
	}
	static CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT m_errLayout;
	return m_errLayout;
}

CXTPCalendarDayViewTimeScale::XTP_TIMESCALE_LAYOUT& CXTPCalendarTheme::GetTimeScaleLayout(CXTPCalendarDayViewTimeScale* pTimeScale)
{
	ASSERT(pTimeScale);
	if (pTimeScale)
	{
		return pTimeScale->GetLayout();
	}
	static CXTPCalendarDayViewTimeScale::XTP_TIMESCALE_LAYOUT m_errLayout;
	return m_errLayout;
}

CXTPCalendarViewGroup::XTP_VIEW_GROUP_LAYOUT& CXTPCalendarTheme::GetViewGroupLayout(CXTPCalendarViewGroup* pViewGroup)
{
	ASSERT(pViewGroup);
	if (pViewGroup)
	{
		return pViewGroup->GetLayout_();
	}
	static CXTPCalendarViewGroup::XTP_VIEW_GROUP_LAYOUT m_errLayout;
	return m_errLayout;
}

CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& CXTPCalendarTheme::GetDayViewGroupLayout(CXTPCalendarDayViewGroup* pDayViewGroup)
{
	ASSERT(pDayViewGroup);
	if (pDayViewGroup)
	{
		return pDayViewGroup->GetLayout();
	}
	static CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT m_errLayoutX;
	return m_errLayoutX;
}

CXTPCalendarWeekView::XTP_WEEK_VIEW_LAYOUT& CXTPCalendarTheme::GetWeekViewLayout(CXTPCalendarWeekView* pWeekView)
{
	ASSERT(pWeekView);
	if (pWeekView)
	{
		return pWeekView->GetLayout();
	}
	static CXTPCalendarWeekView::XTP_WEEK_VIEW_LAYOUT m_errLayoutX = {0};
	return m_errLayoutX;
}

////////////////////////////////////////////////////////////////////////
CSize CXTPCalendarTheme::DrawLine_CenterLR(CDC* pDC, LPCTSTR pcszText, CRect& rcRect, UINT nFormat)
{
	nFormat |= DT_NOPREFIX | DT_SINGLELINE;

	int nLeftRight = nFormat & (DT_LEFT | DT_RIGHT);
	nFormat &= ~(DT_CENTER | DT_LEFT | DT_RIGHT);

	CSize sz = pDC->GetTextExtent(pcszText);
	if (sz.cx < labs(rcRect.right - rcRect.left) )
	{
		nFormat |= DT_CENTER;
	}
	else
	{
		nFormat |= nLeftRight ? nLeftRight : DT_LEFT;
	}

	pDC->DrawText(pcszText, &rcRect, nFormat);
	return sz;
}

CSize CXTPCalendarTheme::DrawLine2_CenterLR(CDC* pDC, LPCTSTR pcszText1, LPCTSTR pcszText2,
										 CThemeFontColorSet* pFontColor1, CThemeFontColorSet* pFontColor2,
										 CRect& rcRect, UINT nShortAlign, UINT nFullAlign)
{
	UINT nFormat = DT_NOPREFIX | DT_SINGLELINE;

	CXTPFontDC autoFont(pDC, pFontColor2->Font());
	CSize sz2 = pDC->GetTextExtent(pcszText2);

	autoFont.SetFont(pFontColor1->Font());
	CSize sz1 = pDC->GetTextExtent(pcszText1);

	int nLen1 = (int)_tcslen(pcszText1);
	int nXspace = nLen1 ? sz1.cx/nLen1/2 + 1 : 0;
	int nText12_cx = sz1.cx + nXspace + sz2.cx + 2;

	// calculate only
	if ((nShortAlign | nFullAlign) & DT_CALCRECT)
	{
		return CSize(nText12_cx, max(sz1.cy, sz2.cy));
	}

	CRect rcText = rcRect;
	rcText.right = min(rcText.left + nText12_cx, rcRect.right);

	// Align
	BOOL bCenter = (nFullAlign & DT_CENTER) != 0;
	if (nText12_cx < rcRect.Width() && bCenter)
	{
		nFormat |= DT_LEFT | (nFullAlign & ~DT_CENTER);

		int nOffset = (rcRect.Width() - nText12_cx) / 2;
		rcText.left += nOffset;
		rcText.right += nOffset;
	}
	else
	{
		nFormat |= nShortAlign;
	}

	// Draw
	if (nFormat & DT_RIGHT)
	{
		autoFont.SetFontColor(pFontColor2->Font(), pFontColor2->Color());
		CRect rcText1 = rcText;
		pDC->DrawText(pcszText2, &rcText1, nFormat);

		autoFont.SetFontColor(pFontColor1->Font(), pFontColor1->Color());
		CRect rcText2 = rcText;
		rcText2.right = max(rcText.right - sz2.cx - nXspace, rcText.left);
		pDC->DrawText(pcszText1, &rcText2, nFormat);
	}
	else
	{
		autoFont.SetColor(pFontColor1->Color());
		CRect rcText1 = rcText;
		pDC->DrawText(pcszText1, &rcText1, nFormat);

		autoFont.SetFontColor(pFontColor2->Font(), pFontColor2->Color());
		CRect rcText2 = rcText;
		rcText2.left = min(rcText.left + sz1.cx + nXspace, rcText.right);

		pDC->DrawText(pcszText2, &rcText2, nFormat);
	}

	return CSize(nText12_cx, max(sz1.cy, sz2.cy));
}


void CXTPCalendarTheme::DrawRoundRect(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,
					BOOL bBoldBorder, CXTPPaintManagerColorGradient& grclrBk)
{
	_DrawRoundRect(pDC, rcRect, clrBorder, bBoldBorder, &grclrBk, NULL);
}

void CXTPCalendarTheme::DrawRoundRect(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,
					BOOL bBoldBorder, CBrush* pBrushBk)
{
	ASSERT(pBrushBk);
	_DrawRoundRect(pDC, rcRect, clrBorder, bBoldBorder, NULL, pBrushBk);
}


void CXTPCalendarTheme::_DrawRoundRect(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,
					BOOL bBoldBorder, CXTPPaintManagerColorGradient* pgrclrBk, CBrush* pBrushBk)
{
	if (!pgrclrBk && !pBrushBk)
	{
		ASSERT(FALSE);
		return;
	}

	CPoint ptCorrner(cnCornerSize, cnCornerSize);

	// fill event background
	CGdiObject* pOldBrush = pDC->SelectStockObject(NULL_BRUSH);
	CBrush brBk;

	if (pDC->IsPrinting() || pBrushBk)
	{
		if (!pBrushBk)
		{
			if (pgrclrBk)
			{
				COLORREF clrColorBk = XTPDrawHelpers()->BlendColors(pgrclrBk->clrLight, pgrclrBk->clrDark, 0.5f);
				brBk.CreateSolidBrush(clrColorBk);

				pDC->SelectObject(&brBk);
			}
		}
		else
		{
			pDC->SelectObject(pBrushBk);
		}
	}
	else
	{
		CRect rcClip = rcRect;
		rcClip.right++;
		rcClip.bottom++;
		pDC->LPtoDP(&rcClip);

		CRgn rgnEvent;
		BOOL b = rgnEvent.CreateRoundRectRgn(rcClip.left, rcClip.top,
											rcClip.right, rcClip.bottom,
											ptCorrner.x, ptCorrner.y);
		VERIFY(b);

		pDC->SelectClipRgn(&rgnEvent);

		CRect rcBk = rcRect;
		rcBk.DeflateRect(0, 1, 0, 1);
		XTPDrawHelpers()->GradientFill(pDC, &rcBk, *pgrclrBk, FALSE);

		pDC->SelectClipRgn(NULL);
	}

	//
	if (clrBorder == COLORREF_NULL)
	{
		CGdiObject* pOldPen = pDC->SelectStockObject(NULL_PEN);

		pDC->RoundRect(&rcRect, ptCorrner);

		pDC->SelectObject(pOldPen);
		return;
	}

	// draw border
	CXTPPenDC penDC(pDC->GetSafeHdc(), clrBorder);

	pDC->RoundRect(&rcRect, ptCorrner);

	if (bBoldBorder)
	{
		CRect rcBorder2 = rcRect;
		rcBorder2.DeflateRect(1,1,1,1);
		pDC->RoundRect(&rcBorder2, ptCorrner);

		pDC->SetPixel(rcRect.left + 3, rcRect.top + 1, clrBorder);
		pDC->SetPixel(rcRect.left + 1, rcRect.top + 3, clrBorder);

		pDC->SetPixel(rcRect.right - 4, rcRect.top + 1, clrBorder);
		pDC->SetPixel(rcRect.right - 2, rcRect.top + 3, clrBorder);

		pDC->SetPixel(rcRect.right - 4, rcRect.bottom - 2, clrBorder);
		pDC->SetPixel(rcRect.right - 2, rcRect.bottom - 4, clrBorder);

		pDC->SetPixel(rcRect.left + 3, rcRect.bottom - 2, clrBorder);
		pDC->SetPixel(rcRect.left + 1, rcRect.bottom - 4, clrBorder);
	}

	pDC->SelectObject(pOldBrush);
}

BOOL CXTPCalendarTheme::_LoadStdBitmap(int nBmpID, LPCTSTR pcszStdBmpName)
{
	if (!m_pImagesStd || !m_pImageList || !pcszStdBmpName)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPOffice2007Image* pBmp = m_pImagesStd->LoadFile(pcszStdBmpName);
	ASSERT(pBmp);
	if (pBmp)
		m_pImageList->SetBitmap(pBmp, (UINT)nBmpID, TRUE);

	return !!pBmp;
}

void CXTPCalendarTheme::DrawRectPartly(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,
					int nBorderWidth, CRect isBorders)
{
	//CRect rcLn;

	CPen penBorder(PS_SOLID, 1, clrBorder);
	CXTPPenDC autoPen(pDC, &penBorder);

//  int nD = 1;
	int nD = (pDC->IsPrinting() && pDC->GetMapMode() != MM_TEXT) ? 0 : 1;

	for (int i = 0; i < nBorderWidth; i++)
	{
		if (isBorders.left)
		{
			pDC->MoveTo(rcRect.left+i, rcRect.top);
			pDC->LineTo(rcRect.left+i, rcRect.bottom);

			//rcLn = rcRect;
			//rcLn.right = rcLn.left + nBorderWidth;
			//pDC->FillSolidRect(&rcLn, clrBorder);
		}
		if (isBorders.right)
		{
			pDC->MoveTo(rcRect.right-nD-i, rcRect.top);
			pDC->LineTo(rcRect.right-nD-i, rcRect.bottom);
			//rcLn = rcRect;
			//rcLn.left = rcLn.right - nBorderWidth;
			//pDC->FillSolidRect(&rcLn, clrBorder);
		}
		if (isBorders.top)
		{
			pDC->MoveTo(rcRect.left, rcRect.top+i);
			pDC->LineTo(rcRect.right, rcRect.top+i);
			//rcLn = rcRect;
			//rcLn.bottom = rcLn.top + nBorderWidth;
			//pDC->FillSolidRect(&rcLn, clrBorder);
		}
		if (isBorders.bottom)
		{
			pDC->MoveTo(rcRect.left, rcRect.bottom-nD-i);
			pDC->LineTo(rcRect.right, rcRect.bottom-nD-i);
			//rcLn = rcRect;
			//rcLn.top = rcLn.bottom - nBorderWidth;
			//pDC->FillSolidRect(&rcLn, clrBorder);
		}
	}
}

void CXTPCalendarTheme::DrawRectPartly(CDC* pDC, const CRect& rcRect, COLORREF clrBorder,
					BOOL bBoldBorder, CXTPPaintManagerColorGradient& grclrBk, CRect isBorders)
{
	// fill event background
	if (pDC->IsPrinting())
	{
		COLORREF clrColorBk = XTPDrawHelpers()->BlendColors(grclrBk.clrLight, grclrBk.clrDark, 0.5f);
		pDC->FillSolidRect(&rcRect, clrColorBk);
	}
	else
	{
		XTPDrawHelpers()->GradientFill(pDC, &rcRect, grclrBk, FALSE);
	}

	// draw border
	int nWidth = bBoldBorder ? 2 : 1;
	DrawRectPartly(pDC, rcRect, clrBorder, nWidth, isBorders);
}
// TODO - replace.
CFont* CXTPCalendarTheme::GetMaxHeightFont(CArray<CFont*, CFont*>& arFonts, CDC* pDC,
										   int* pnMaxHeight)
{
	return CXTPCalendarUtils::GetMaxHeightFont(arFonts, pDC, pnMaxHeight);
}

void CXTPCalendarTheme::RemoveBoldAttrFromStd(CXTPCalendarThemeFontValue& rFont)
{
	if ((CFont*)rFont)
	{
		LOGFONT lfBase;
		if (rFont->GetLogFont(&lfBase))
		{
			lfBase.lfWeight = FW_NORMAL;
			rFont.SetStandardValue(&lfBase);
		}
	}

	if (!(CFont*)rFont || !rFont->m_hObject)
	{
		CFont* pFontDef = m_fntBaseFont;
		rFont.SetStandardValue(pFontDef);
	}
}

CXTPCalendarThemePart::CXTPCalendarThemePart()
{
	m_pTheme = NULL;
	m_pOwner = NULL;

	m_bSendBeforeDrawThemeObjectEnabled = TRUE;
}

CXTPCalendarThemePart::~CXTPCalendarThemePart()
{
}

CXTPCalendarThemePart* CXTPCalendarThemePart::GetOwner()
{
	return m_pOwner;
}

CXTPCalendarTheme* CXTPCalendarThemePart::GetTheme()
{
	return m_pTheme;
}

LPCTSTR CXTPCalendarThemePart::GetInstanceName()
{
	return m_strInstanceName;
}

void CXTPCalendarThemePart::SetInstanceName(LPCTSTR pcszInstanceName)
{
	m_strInstanceName = pcszInstanceName;
}

void CXTPCalendarThemePart::Create(CXTPCalendarTheme* pTheme, CXTPCalendarThemePart* pOwner)
{
	ASSERT(pTheme);
	ASSERT(pOwner);

	m_pTheme = pTheme;
	m_pOwner = (pOwner != this) ? pOwner : NULL;

	_CreateMembers();

	for (int i = 0; i < m_arMembers.GetCount(); i++)
	{
		if (m_arMembers.GetAt(i))
		{
			ASSERT(_tcslen(m_arMembers.GetAt(i)->GetInstanceName()) > 0);
			m_arMembers.GetAt(i)->Create(pTheme, this);
		}
	}
}

void CXTPCalendarThemePart::_CreateMembers()
{
	m_arMembers.RemoveAll();
	m_arMembers.SetSize(10);

	CXTPCalendarThemePart* pM0 = CreateMember0();
	CXTPCalendarThemePart* pM1 = CreateMember1();
	CXTPCalendarThemePart* pM2 = CreateMember2();
	CXTPCalendarThemePart* pM3 = CreateMember3();
	CXTPCalendarThemePart* pM4 = CreateMember4();
	CXTPCalendarThemePart* pM5 = CreateMember5();
	CXTPCalendarThemePart* pM6 = CreateMember6();
	CXTPCalendarThemePart* pM7 = CreateMember7();
	CXTPCalendarThemePart* pM8 = CreateMember8();
	CXTPCalendarThemePart* pM9 = CreateMember9();

	m_arMembers.SetAt(0, pM0);
	m_arMembers.SetAt(1, pM1);
	m_arMembers.SetAt(2, pM2);
	m_arMembers.SetAt(3, pM3);
	m_arMembers.SetAt(4, pM4);
	m_arMembers.SetAt(5, pM5);
	m_arMembers.SetAt(6, pM6);
	m_arMembers.SetAt(7, pM7);
	m_arMembers.SetAt(8, pM8);
	m_arMembers.SetAt(9, pM9);
}

void CXTPCalendarThemePart::RefreshMetrics(BOOL bRefreshChildren)
{
	if (!bRefreshChildren)
	{
		return;
	}

	int nCount = m_arMembers.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarThemePart* pMember = m_arMembers.GetAt(i);
		if (pMember)
		{
			pMember->RefreshMetrics();
		}
	}
}

void CXTPCalendarThemePart::DoPropExchange(CXTPPropExchange* pPX)
{
	int nCount = m_arMembers .GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarThemePart* pMember = m_arMembers.GetAt(i, FALSE);
		if (pMember)
		{
			CString strSection = pMember->GetInstanceName();
			CXTPPropExchangeSection secData(pPX->GetSection(strSection));
			if (pPX->IsStoring())
				secData->EmptySection();

			pMember->DoPropExchange(&secData);
		}
	}
}

void CXTPCalendarThemePart::Serialize(CArchive& ar)
{
	int nCount = m_arMembers .GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarThemePart* pMember = m_arMembers.GetAt(i, FALSE);
		if (pMember)
		{
			pMember->Serialize(ar);
		}
	}
}

void CXTPCalendarThemePart::SendNotificationAlways(XTP_NOTIFY_CODE EventCode, WPARAM wParam , LPARAM lParam)
{
	if (!GetTheme() || !GetTheme()->GetCalendarControl())
	{
		ASSERT(FALSE);
		return;
	}
	GetTheme()->GetCalendarControl()->SendNotificationAlways(EventCode, wParam , lParam);
}

#ifdef _DEBUG
int CXTPCalendarThemePart::CXTPPropsState::s_dbg_nActiveStartes = 0;
#endif

BOOL CXTPCalendarThemePart::CXTPPropsState::s_bUseSerialize = TRUE;

CXTPCalendarThemePart::CXTPPropsState::CXTPPropsState()
{
	//m_pDataFile = new CMemFile();
}

CXTPCalendarThemePart::CXTPPropsState::~CXTPPropsState()
{
	//SAFE_DELETE(m_pDataFile);
}

void CXTPCalendarThemePart::CXTPPropsState::SaveState()
{
	METHOD_PROLOGUE_(CXTPCalendarThemePart, PropsState)

	CMemFile* pDataFile = new CMemFile();
	if (!pDataFile)
	{
		return;
	}

	try
	{
		CArchive ar(pDataFile, CArchive::store);

		if (s_bUseSerialize)
		{
			pThis->Serialize(ar);
		}
		else
		{
			CXTPPropExchangeArchive px(ar);
			pThis->DoPropExchange(&px);
		}

		ar.Close(); // perform Flush() and detach from file

		m_arStates.Add(pDataFile);

		#ifdef _DEBUG
		s_dbg_nActiveStartes++;
		#endif
	}
	catch(CArchiveException* pE)
	{
		ASSERT(FALSE); //if (pE->GetErrorMessage(szErrText, cErrTextSize))
		pE->Delete();
	}
	catch(CFileException* pE)
	{
		ASSERT(FALSE); //if (pE->GetErrorMessage(szErrText, cErrTextSize))
		pE->Delete();
	}
	catch(...)
	{
		ASSERT(FALSE);
	}
}

void CXTPCalendarThemePart::CXTPPropsState::RestoreState(BOOL bClearData)
{
	METHOD_PROLOGUE_(CXTPCalendarThemePart, PropsState)

	int nDataCount = (int)m_arStates.GetSize();
	if (!nDataCount || !m_arStates[nDataCount-1])
	{
		ASSERT(FALSE);
		return;
	}

	CMemFile* pDataFile = m_arStates[nDataCount-1];
	if (!pDataFile)
	{
		return;
	}

	try
	{
		pDataFile->Seek(0, CFile::begin);

		CArchive ar(pDataFile, CArchive::load);

		if (s_bUseSerialize)
		{
			pThis->Serialize(ar);
		}
		else
		{
			CXTPPropExchangeArchive px(ar);
			pThis->DoPropExchange(&px);
		}

		ar.Close(); // detach from file

		if (bClearData)
		{
			SAFE_DELETE(pDataFile);
			m_arStates.RemoveAt(nDataCount-1);

			#ifdef _DEBUG
			s_dbg_nActiveStartes--;
			#endif
		}
	}
	catch(CArchiveException* pE)
	{
		ASSERT(FALSE); //if (pE->GetErrorMessage(szErrText, cErrTextSize))
		pE->Delete();
	}
	catch(CFileException* pE)
	{
		ASSERT(FALSE); //if (pE->GetErrorMessage(szErrText, cErrTextSize))
		pE->Delete();
	}
	catch(...)
	{
		ASSERT(FALSE);
	}
}

void CXTPCalendarThemePart::CXTPPropsState::ClearLastState()
{
	int nDataCount = (int)m_arStates.GetSize();
	ASSERT(nDataCount);
	if (nDataCount)
	{
		CMemFile* pDataFile = m_arStates[nDataCount-1];
		SAFE_DELETE(pDataFile);

		m_arStates.RemoveAt(nDataCount-1);

		#ifdef _DEBUG
		s_dbg_nActiveStartes--;
		#endif
	}
}

void CXTPCalendarThemePart::CXTPPropsState::ClearAll()
{
	#ifdef _DEBUG
		s_dbg_nActiveStartes -= (int)m_arStates.GetSize();
	#endif

	m_arStates.RemoveAll();
}

//=================================================================
CXTPCalendarThemePart::CXTPPropsStateContext::CXTPPropsStateContext(
				CXTPCalendarThemePart* pPart, int eBDFlag, BOOL bClearAll)
{
	m_pPart = NULL;
	m_eBeroreDrawFlag = eBDFlag;
//  m_bAttached = FALSE;

	if (pPart)
	{
		if (bClearAll)
			pPart->m_xPropsState.ClearAll();

		if (eBDFlag == 0 || pPart->GetTheme() && pPart->GetTheme()->IsBeforeDraw(eBDFlag))
		{
			m_pPart = pPart;
			m_pPart->m_xPropsState.SaveState();
		}
	}
}

CXTPCalendarThemePart::CXTPPropsStateContext::~CXTPPropsStateContext()
{
	if (m_pPart)
		m_pPart->m_xPropsState.RestoreState(TRUE); //!m_bAttached
}

void CXTPCalendarThemePart::CXTPPropsStateContext::SetData(CXTPCalendarThemePart* pPart,
														   int eBDFlag, BOOL bClearAll)
{
	ASSERT(pPart);
	m_pPart = NULL;
	m_eBeroreDrawFlag = eBDFlag;

	if (pPart)
	{
		if (bClearAll)
			pPart->m_xPropsState.ClearAll();

		if (eBDFlag == 0 || pPart->GetTheme() && pPart->GetTheme()->IsBeforeDraw(eBDFlag))
		{
			m_pPart = pPart;
			m_pPart->m_xPropsState.SaveState();
		}
	}
}

void CXTPCalendarThemePart::CXTPPropsStateContext::Clear()
{
	if (m_pPart)
	{
		m_pPart->m_xPropsState.ClearLastState();
	}

	m_pPart = NULL;
}

void CXTPCalendarThemePart::CXTPPropsStateContext::RestoreState()
{
	if (m_pPart)
		m_pPart->m_xPropsState.RestoreState(FALSE);
}

void CXTPCalendarThemePart::CXTPPropsStateContext::SendBeforeDrawThemeObject(LPARAM lParam, int eBDFlag)
{
	if (!m_pPart || !m_pPart->GetTheme() || !m_pPart->GetTheme()->GetCalendarControl())
		return;

	if (!m_pPart->m_bSendBeforeDrawThemeObjectEnabled)
		return;

	ASSERT(eBDFlag == 0 || m_pPart->GetTheme()->IsBeforeDraw(m_eBeroreDrawFlag));

	if (eBDFlag == 0)
		eBDFlag = m_eBeroreDrawFlag;

	if (m_pPart->GetTheme()->IsBeforeDraw(eBDFlag))
	{
		m_pPart->GetTheme()->GetCalendarControl()->SendNotificationAlways(
			XTP_NC_CALENDAR_BEFORE_DRAW_THEMEOBJECT, (WPARAM)eBDFlag, lParam);
	}

}



//#ifdef _XTP_ACTIVEX
//void CXTPCalendarThemePart::AddAxPartToCache(CXTPCalendarThemeAxPart* pAxPart)
//{
//  m_arAxParts.Add(pAxPart);
//}
//
//void CXTPCalendarThemePart::RemoveAxPartFromCache(CXTPCalendarThemeAxPart* pAxPart)
//{
//  int nCount = m_arAxParts.GetSize();
//  for (int i = nCount - 1; i >= 0; i--)
//  {
//      if (pAxPart == m_arAxParts.GetAt(i))
//      {
//          m_arAxParts.RemoveAt(i);
//          break;
//      }
//  }
//}
//
//void CXTPCalendarThemePart::ClearAxPartsCache()
//{
//  int nCount = m_arAxParts.GetSize();
//  for (int i = nCount - 1; i >= 0; i--)
//  {
//      CXTPCalendarThemeAxPart* pAxPart = m_arAxParts.GetAt(i);
//      if (pAxPart)
//      {
//          pAxPart->ClearCache();
//      }
//  }
//  m_arAxParts.RemoveAll();
//}
//#endif

/////////////////////////////////////////////////////////////////////////////
// class CTOFormula_MulDivC : virtual public CXTPCalendarThemePart
CXTPCalendarTheme::CTOFormula_MulDivC::CTOFormula_MulDivC()
{
	SetStandardValue(14, 10, 1);
}

void CXTPCalendarTheme::CTOFormula_MulDivC::RefreshFromParent(class CTOFormula_MulDivC* pParentSrc)
{
	if (!pParentSrc)
	{
		ASSERT(FALSE);
		return;
	}

	m_Multiplier.CopySettings(pParentSrc->m_Multiplier);
	m_Divisor.CopySettings(pParentSrc->m_Divisor);
	m_Constant.CopySettings(pParentSrc->m_Constant);
}

void CXTPCalendarTheme::CTOFormula_MulDivC::SetStandardValue(int nMul, int nDiv, int nC)
{
	m_Multiplier.SetStandardValue(nMul);
	m_Divisor.SetStandardValue(nDiv);
	m_Constant.SetStandardValue(nC);
}

int CXTPCalendarTheme::CTOFormula_MulDivC::Calculate(int nY) const
{
	ASSERT((int)m_Divisor);

	int nMul = (int)m_Multiplier;
	int nDiv = (int)m_Divisor ? (int)m_Divisor : (int)1;
	int nC   = (int)m_Constant;

	int nResult = nY * nMul / nDiv + nC;

	return nResult;
}

void CXTPCalendarTheme::CTOFormula_MulDivC::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPPropExchangeSection secData(pPX->GetSection(_T("Formula_MulDivC")));
	if (pPX->IsStoring())
		secData->EmptySection();

	m_Multiplier.DoPropExchange(&secData, _T("Multiplier"));
	m_Divisor.DoPropExchange(&secData, _T("Divisor"));
	m_Constant.DoPropExchange(&secData, _T("Constant"));

	CXTPCalendarThemePart::DoPropExchange(&secData);
}

void CXTPCalendarTheme::CTOFormula_MulDivC::Serialize(CArchive& ar)
{
	m_Multiplier.Serialize(ar);
	m_Divisor.Serialize(ar);
	m_Constant.Serialize(ar);

	CXTPCalendarThemePart::Serialize(ar);
}

CXTPCalendarTheme::CTOEventIconsToDraw::CTOEventIconsToDraw()
{
	SetStandardValue(FALSE, FALSE, FALSE, FALSE, FALSE);
}

void CXTPCalendarTheme::CTOEventIconsToDraw::RefreshFromParent(CTOEventIconsToDraw* pParentSrc)
{
	if (!pParentSrc)
	{
		return;
	}
	m_ShowReminder.CopySettings(pParentSrc->m_ShowReminder);
	m_ShowOccurrence.CopySettings(pParentSrc->m_ShowOccurrence);
	m_ShowException.CopySettings(pParentSrc->m_ShowException);
	m_ShowMeeting.CopySettings(pParentSrc->m_ShowMeeting);
	m_ShowPrivate.CopySettings(pParentSrc->m_ShowPrivate);
}

void CXTPCalendarTheme::CTOEventIconsToDraw::SetStandardValue(BOOL bRmd, BOOL bOcc, BOOL bExc, BOOL bMee, BOOL bPrv)
{
	m_ShowReminder.SetStandardValue(bRmd);
	m_ShowOccurrence.SetStandardValue(bOcc);
	m_ShowException.SetStandardValue(bExc);
	m_ShowMeeting.SetStandardValue(bMee);
	m_ShowPrivate.SetStandardValue(bPrv);
}

void CXTPCalendarTheme::CTOEventIconsToDraw::FillIconIDs(CUIntArray& rarGlyphIDs,
														 CUIntArray& rarGlyphIndex, CXTPCalendarEvent* pEvent)
{
	rarGlyphIDs.RemoveAll();
	rarGlyphIndex.RemoveAll();

	if (!pEvent)
	{
		ASSERT(FALSE);
		return;
	}

	if ((BOOL)m_ShowReminder && pEvent->IsReminder())
	{
		rarGlyphIDs.Add((UINT)xtpCalendarEventIconIDReminder);
		rarGlyphIndex.Add(0);
	}
	if ((BOOL)m_ShowOccurrence &&
		pEvent->GetRecurrenceState() == xtpCalendarRecurrenceOccurrence )
	{
		rarGlyphIDs.Add((UINT)xtpCalendarEventIconIDOccurrence);
		rarGlyphIndex.Add(1);
	}
	if ((BOOL)m_ShowException &&
		pEvent->GetRecurrenceState() == xtpCalendarRecurrenceException )
	{
		rarGlyphIDs.Add((UINT)xtpCalendarEventIconIDException);
		rarGlyphIndex.Add(2);
	}
	if ((BOOL)m_ShowMeeting && pEvent->IsMeeting())
	{
		rarGlyphIDs.Add((UINT)xtpCalendarEventIconIDMeeting);
		rarGlyphIndex.Add(3);
	}
	if ((BOOL)m_ShowPrivate && pEvent->IsPrivate())
	{
		rarGlyphIDs.Add((UINT)xtpCalendarEventIconIDPrivate);
		rarGlyphIndex.Add(4);
	}
}

void CXTPCalendarTheme::CTOEventIconsToDraw::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPPropExchangeSection secData(pPX->GetSection(_T("EventIconsToDraw")));
	if (pPX->IsStoring())
		secData->EmptySection();

	m_ShowReminder  .DoPropExchange(&secData, _T("ShowReminder"));
	m_ShowOccurrence.DoPropExchange(&secData, _T("ShowOccurrence"));
	m_ShowException .DoPropExchange(&secData, _T("ShowException"));
	m_ShowMeeting   .DoPropExchange(&secData, _T("ShowMeeting"));
	m_ShowPrivate   .DoPropExchange(&secData, _T("ShowPrivate"));

	CXTPCalendarThemePart::DoPropExchange(&secData);
}

void CXTPCalendarTheme::CTOEventIconsToDraw::Serialize(CArchive& ar)
{
	m_ShowReminder.Serialize(ar);
	m_ShowOccurrence.Serialize(ar);
	m_ShowException.Serialize(ar);
	m_ShowMeeting.Serialize(ar);
	m_ShowPrivate.Serialize(ar);

	CXTPCalendarThemePart::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// class

CXTPCalendarTheme::CTOColorsSet::CTOColorsSet()
{
	m_clrBase = RGB(128, 128, 128);

	m_nCLR_DarkenOffset = 10 * 1000;

	m_mapScale.InitHashTable(XTP_CALENDAR_COLORS_SET_HASH_TABLE_SIZE);
}

void CXTPCalendarTheme::CTOColorsSet::RefreshMetrics(BOOL bRefreshChildren)
{
	UNREFERENCED_PARAMETER(bRefreshChildren);

	m_clrBase = GetTheme()->m_clrBaseColor;
	//m_mapScale[xtpCLR_base] = 0;
}

COLORREF CXTPCalendarTheme::CTOColorsSet::GetColor(int eCLR, COLORREF clrBase)
{
	if (clrBase == (COLORREF)-1)
	{
		clrBase = m_clrBase;
	}

	int nScale = GetScale(eCLR);

	if (eCLR < m_nCLR_DarkenOffset)
	{
		COLORREF clrColor = XTPDrawHelpers()->LightenColor(nScale, clrBase);
		return clrColor;
	}

	COLORREF clrColor = XTPDrawHelpers()->DarkenColor(nScale, clrBase);
	return clrColor;
}

int CXTPCalendarTheme::CTOColorsSet::GetScale(int eCLR)
{
	int nScale = 0;
	if (m_mapScale.Lookup(eCLR, nScale))
	{
		return nScale;
	}

	return 0;
}

//===========================================================================
CXTPCalendarTheme::CThemeFontColorSet*
	CXTPCalendarTheme::CTOHeader::CHeaderText::GetFontColor(int nItemState)
{
	nItemState &= xtpCalendarItemState_mask;
	nItemState &= ~xtpCalendarItemState_Hot; // Hot item has no separate font

	switch(nItemState)
	{
	case CTOHeader::xtpCalendarItemState_Normal:
		return &fcsetNormal;
	case CTOHeader::xtpCalendarItemState_Selected:
		return &fcsetSelected;
	case CTOHeader::xtpCalendarItemState_Today:
		return &fcsetToday;
	case CTOHeader::xtpCalendarItemState_Today | CTOHeader::xtpCalendarItemState_Selected:
		return &fcsetTodaySelected;
	}
	ASSERT(FALSE);
	return &fcsetNormal;
}

void CXTPCalendarTheme::CTOHeader::CHeaderText::CopySettings(
	const CXTPCalendarTheme::CTOHeader::CHeaderText& rSrc)
{
	fcsetNormal.CopySettings(rSrc.fcsetNormal);
	fcsetSelected.CopySettings(rSrc.fcsetSelected);
	fcsetToday.CopySettings(rSrc.fcsetToday);
	fcsetTodaySelected.CopySettings(rSrc.fcsetTodaySelected);
}

void CXTPCalendarTheme::CThemeFontColorSetValue::doPX(CXTPPropExchange* pPX,
									LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme)
{
	if (!pPX || !pcszPropName || !pTheme)
	{
		ASSERT(FALSE);
		return;
	}
	CXTPPropExchangeSection secData(pPX->GetSection(pcszPropName));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("Color"), clrColor);
	PX_Font(&secData, _T("Font"),   fntFont);
}

void CXTPCalendarTheme::CThemeFontColorSetValue::Serialize(CArchive& ar)
{
	clrColor.Serialize(ar);
	fntFont.Serialize(ar);
}

void CXTPCalendarTheme::CTOHeader::CHeaderText::doPX(CXTPPropExchange* pPX,
									LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme)
{
	if (!pPX || !pcszPropName || !pTheme)
	{
		ASSERT(FALSE);
		return;
	}
	CXTPPropExchangeSection secData(pPX->GetSection(pcszPropName));
	if (pPX->IsStoring())
		secData->EmptySection();

	fcsetNormal.doPX(&secData,        _T("Normal"),         pTheme);
	fcsetSelected.doPX(&secData,      _T("Selected"),       pTheme);
	fcsetToday.doPX(&secData,         _T("Today"),          pTheme);
	fcsetTodaySelected.doPX(&secData, _T("TodaySelected"),  pTheme);
}

void CXTPCalendarTheme::CTOHeader::CHeaderText::Serialize(CArchive& ar)
{
	fcsetNormal.Serialize(ar);
	fcsetSelected.Serialize(ar);
	fcsetToday.Serialize(ar);
	fcsetTodaySelected.Serialize(ar);
}

void CXTPCalendarTheme::CTOHeader::CHeaderBackground::doPX(CXTPPropExchange* pPX,
									LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme)
{
	if (!pPX || !pcszPropName || !pTheme)
	{
		ASSERT(FALSE);
		return;
	}
	CXTPPropExchangeSection secData(pPX->GetSection(pcszPropName));
	if (pPX->IsStoring())
		secData->EmptySection();

	nBitmapID.DoPropExchange(&secData, _T("BitmapID"));
	rcBitmapBorder.DoPropExchange(&secData, _T("BitmapBorder"));
}

void CXTPCalendarTheme::CTOHeader::CHeaderBackground::Serialize(CArchive& ar)
{
	nBitmapID.Serialize(ar);
	rcBitmapBorder.Serialize(ar);
}

BOOL CXTPCalendarTheme::CTOHeader::IsToday(CXTPCalendarViewDay* pViewDay)
{
	ASSERT(pViewDay);
	if (!pViewDay)
	{
		return FALSE;
	}

	COleDateTime dtToday =  CXTPCalendarUtils::GetCurrentTime();
	dtToday = CXTPCalendarUtils::ResetTime(dtToday);

	COleDateTime dtDay = pViewDay->GetDayDate();
	ASSERT(CXTPCalendarUtils::IsZeroTime(dtDay));

	dtDay = CXTPCalendarUtils::ResetTime(dtDay);

	BOOL bIsToday = (dtDay == dtToday);
	return bIsToday;
}

BOOL CXTPCalendarTheme::CTOHeader::IsSelected(CXTPCalendarViewDay* pViewDay)
{
	if (!pViewDay || !pViewDay->GetView_())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	COleDateTime dtBegin;
	COleDateTime dtEnd;

	if (pViewDay->GetView_()->GetSelection(&dtBegin, &dtEnd))
	{
		COleDateTime dtDay = pViewDay->GetDayDate();
		ASSERT(CXTPCalendarUtils::IsZeroTime(dtDay));
		dtDay = CXTPCalendarUtils::ResetTime(dtDay);

		dtBegin = CXTPCalendarUtils::ResetTime(dtBegin);
		dtEnd = CXTPCalendarUtils::ResetTime(dtEnd);

		return dtBegin <= dtDay && dtDay < dtEnd || (dtBegin == dtDay && dtDay == dtEnd);
	}
	return FALSE;
}

BOOL CXTPCalendarTheme::CTOHeader::IsSelected(CXTPCalendarViewGroup* pViewGroup)
{
	if (!pViewGroup || !pViewGroup->GetViewDay_() || !pViewGroup->GetViewDay_()->GetView_())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nSelGroupIndex;

	BOOL bSel = pViewGroup->GetViewDay_()->GetView_()->GetSelection(NULL, NULL, NULL, &nSelGroupIndex);
	if (!bSel)
	{
		return FALSE;
	}

	int nGroupIndex = pViewGroup->GetGroupIndex();
	if (nGroupIndex != nSelGroupIndex)
	{
		return FALSE;
	}

	return IsSelected(pViewGroup->GetViewDay_());
}

void CXTPCalendarTheme::CTOHeader::Draw_Background(CDC* pDC, const CRect& rcRect,
												   int nState)
{
	if (!GetTheme() || !GetTheme()->GetImageList())
	{
		ASSERT(FALSE);
		return;
	}

	int nIndex = (nState & CTOHeader::xtpCalendarItemFirst) ? 0 :
				 ((nState & CTOHeader::xtpCalendarItemLast) ? 2 : 1);

	BOOL bHot = !!(nState & CTOHeader::xtpCalendarItemState_Hot);
	if (bHot)
		nIndex += 3;

	BOOL bTooday = !!(nState & CTOHeader::xtpCalendarItemState_Today);
	BOOL bSelected = !!(nState & CTOHeader::xtpCalendarItemState_Selected);
	CHeaderBackground* pBkSet = bSelected ? &m_bkSelected : &m_bkNormal;

	if (bTooday)
	{
		pBkSet = bSelected ? &m_bkTodaySelected : &m_bkToday;
	}

	// Draw base color
	COLORREF clrBase = bTooday ? m_clrTodayBaseColor : m_clrBaseColor;
	pDC->FillSolidRect(rcRect, clrBase);

	UINT nButmapID = (UINT)(int)pBkSet->nBitmapID;
	// Lookup for bitmap
	CXTPOffice2007Image* pImage = GetTheme()->GetImageList()->GetBitmap(nButmapID);

	if (!pImage)
	{
		static BOOL s_dbg_ImageAssert_was = FALSE;
		if (!s_dbg_ImageAssert_was && nButmapID != 0)
		{
			s_dbg_ImageAssert_was = TRUE;
			ASSERT(pImage);
		}
		return;
	}

	CRect rcBmp = pImage->GetSource(nIndex, 6);

	// Draw bitmap pattern
	int nFLmask = CTOHeader::xtpCalendarItemFirst | CTOHeader::xtpCalendarItemLast;
	BOOL bFirstLast = (nState & nFLmask) == nFLmask;
	if (!bFirstLast)
	{
		pImage->DrawImage(pDC, rcRect, rcBmp, pBkSet->rcBitmapBorder);
	}
	else
	{
		int nIndex2 = nIndex + 2;
		ASSERT(nIndex2 < 6);

		CRect rcBmp1 = rcBmp;
		CRect rcBmp2 = pImage->GetSource(nIndex2, 6);
		rcBmp1.right = rcBmp1.left + rcBmp.Width()/2;
		rcBmp2.left = rcBmp1.right;

		CRect rcRect1 = rcRect;
		CRect rcRect2 = rcRect;
		rcRect1.right = rcRect1.left + rcRect.Width()/2;
		rcRect2.left = rcRect1.right;

		CRect rcBorder1 = pBkSet->rcBitmapBorder;
		CRect rcBorder2 = pBkSet->rcBitmapBorder;
		rcBorder1.right = 0;
		rcBorder2.left = 0;

		pImage->DrawImage(pDC, rcRect1, rcBmp1, rcBorder1);
		pImage->DrawImage(pDC, rcRect2, rcBmp2, rcBorder2);
	}

	//---------------------------------------------------------------
	// dbg. calc bitmap layers
	//
/*  CXTPOffice2007Image* pBmpHeader = GetTheme()->m_pImagesStd->LoadFile(_T("hotHeader_1.bmp"));
	ASSERT(pBmpHeader);
	if (pBmpHeader)
	{
		CRect rcBmp = pBmpHeader->GetSource(0, 1);
		//COLORREF clrBase = RGB(165, 191, 225);
		pDC->FillSolidRect(rcRect, clrBase);


		CRect rcRectX = rcRect;
		rcRectX.left += 20;

		CXTPCompatibleDC dcSrc(pDC, pBmpHeader->GetBitmap());
		XTPImageManager()->AlphaBlend_calc(pDC->GetSafeHdc(), rcRectX, dcSrc.GetSafeHdc(), rcBmp, clrBase);
	}
*/
}

void CXTPCalendarTheme::CTOHeader::Draw_TextLR(CDC* pDC, const CRect& rcRect, int nFlags,
											   LPCTSTR pcszText, UINT uFormat, int* pnWidth)
{
	CXTPFontDC autoFont(pDC, m_TextLeftRight.GetFontColor(nFlags)->Font(),
							 m_TextLeftRight.GetFontColor(nFlags)->Color());

	CRect rcText = rcRect;
	pDC->DrawText(pcszText, &rcText, uFormat);

	if (pnWidth)
	{
		*pnWidth = pDC->GetTextExtent(pcszText, (int)_tcslen(pcszText)).cx;
		*pnWidth = min(*pnWidth, rcRect.Width());
	}
}

void CXTPCalendarTheme::CTOHeader::Draw_TextCenter(CDC* pDC, const CRect& rcRect, int nFlags,
							LPCTSTR pcszCenterText, int* pnWidth)
{
	CXTPCalendarTheme* pTheme = GetTheme();
	ASSERT(pTheme);
	if (!pTheme)
		return;

	CXTPFontDC autoFont(pDC, m_TextCenter.GetFontColor(nFlags)->Font(),
							 m_TextCenter.GetFontColor(nFlags)->Color());
	CRect rcText = rcRect;
	CSize szText = pTheme->DrawLine_CenterLR(pDC, pcszCenterText, rcText, DT_VCENTER);

	if (pnWidth)
	{
		*pnWidth = min(szText.cx, rcRect.Width());
	}
}


void CXTPCalendarTheme::CTOHeader::Draw_Header(CDC* pDC, const CRect& rcRect, int nFlags,
			LPCTSTR pcszLeftText, LPCTSTR pcszCenterText, LPCTSTR pcszRightText)
{
	Draw_Background(pDC, rcRect, nFlags);

	CRect rcText = rcRect;
	rcText.DeflateRect(5, 1, 3, 3);

	int nWidth_L = 0;
	int nWidth_R = 0;

	CRect rcTextL = rcText;

	if (pcszLeftText && _tcslen(pcszLeftText))
	{
		Draw_TextLR(pDC, rcTextL, nFlags, pcszLeftText,
				DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER, &nWidth_L);
	}

	CRect rcTextR = rcText;
	rcTextR.left = min(rcTextR.left + nWidth_L, rcTextR.right);
	if (pcszRightText && _tcslen(pcszRightText))
	{
		Draw_TextLR(pDC, rcTextR, nFlags, pcszRightText,
				DT_RIGHT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER, &nWidth_R);
	}

	CRect rcTextC = rcText;
	rcTextC.left = min(rcTextC.left + nWidth_L + 1, rcText.right);
	rcTextC.right = max(rcTextC.right - nWidth_R - 1, rcTextC.left);
	if (pcszCenterText && _tcslen(pcszCenterText))
	{
		Draw_TextCenter(pDC, rcTextC, nFlags, pcszCenterText);
	}
}


void CXTPCalendarTheme::CTOHeader::RefreshMetrics(BOOL bRefreshChildren)
{
	CXTPCalendarTheme* pTheme = GetTheme();
	ASSERT(pTheme);
	if (!pTheme)
	{
		return;
	}

	//TBase::RefreshMetrics(bRefreshChildren);
	XTP_SAFE_CALL_BASE(TBase, RefreshMetrics(bRefreshChildren));

	m_clrBaseColor.SetStandardValue(pTheme->m_clrBaseColor);
	m_clrTodayBaseColor.SetStandardValue(pTheme->m_clrBaseColor);

	XTP_SAFE_CALL1(GetHeightFormulaPart(), SetStandardValue(13, 10, 1));

	//***
	m_bkNormal.nBitmapID.SetStandardValue(xtpIMG_DayViewHeader);
	m_bkSelected.nBitmapID.SetStandardValue(xtpIMG_DayViewHeader);
	m_bkToday.nBitmapID.SetStandardValue(xtpIMG_DayViewHeaderTooday);
	m_bkTodaySelected.nBitmapID.SetStandardValue(xtpIMG_DayViewHeaderTooday);

	m_bkNormal.rcBitmapBorder.       SetStandardValue(CRect(1, 1, 1, 1));
	m_bkSelected.rcBitmapBorder.     SetStandardValue(CRect(1, 1, 1, 1));
	m_bkToday.rcBitmapBorder.        SetStandardValue(CRect(1, 1, 1, 1));
	m_bkTodaySelected.rcBitmapBorder.SetStandardValue(CRect(1, 1, 1, 1));

	//-----------------------------------------------
	CFont* pFontLeft = pTheme->m_fntBaseFontBold;
	CFont* pFontCenter = pTheme->m_fntBaseFont;
	//----------------

	m_TextLeftRight.fcsetNormal.fntFont.SetStandardValue(pFontLeft);
	m_TextLeftRight.fcsetSelected.fntFont.SetStandardValue(pFontLeft);
	m_TextLeftRight.fcsetToday.fntFont.SetStandardValue(pFontLeft);
	m_TextLeftRight.fcsetTodaySelected.fntFont.SetStandardValue(pFontLeft);

	m_TextCenter.fcsetNormal.fntFont.SetStandardValue(pFontCenter);
	m_TextCenter.fcsetSelected.fntFont.SetStandardValue(pFontCenter);
	m_TextCenter.fcsetToday.fntFont.SetStandardValue(pFontCenter);
	m_TextCenter.fcsetTodaySelected.fntFont.SetStandardValue(pFontCenter);

	//-------------------------------------------------
	m_TextLeftRight.fcsetNormal.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_TextLeftRight.fcsetSelected.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_TextLeftRight.fcsetToday.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_TextLeftRight.fcsetTodaySelected.clrColor.SetStandardValue(RGB(0, 0, 0));

	m_TextCenter.fcsetNormal.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_TextCenter.fcsetSelected.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_TextCenter.fcsetToday.clrColor.SetStandardValue(RGB(0, 0, 0));
	m_TextCenter.fcsetTodaySelected.clrColor.SetStandardValue(RGB(0, 0, 0));
}

void CXTPCalendarTheme::CTOHeader::RefreshFromParent(CTOHeader* pParentSrc)
{
	ASSERT(pParentSrc);
	if (!pParentSrc)
	{
		return;
	}

	m_clrBaseColor.SetStandardValue(pParentSrc->m_clrBaseColor);
	m_clrTodayBaseColor.SetStandardValue(pParentSrc->m_clrTodayBaseColor);

	m_bkNormal.CopySettings(pParentSrc->m_bkNormal);
	m_bkSelected.CopySettings(pParentSrc->m_bkSelected);
	m_bkToday.CopySettings(pParentSrc->m_bkToday);
	m_bkTodaySelected.CopySettings(pParentSrc->m_bkTodaySelected);

	m_TextLeftRight.CopySettings(pParentSrc->m_TextLeftRight);
	m_TextCenter.CopySettings(pParentSrc->m_TextCenter);

	if (pParentSrc->GetHeightFormulaPart())
		XTP_SAFE_CALL1(GetHeightFormulaPart(), RefreshFromParent(pParentSrc->GetHeightFormulaPart()) );
}

void CXTPCalendarTheme::CTOHeader::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("Header")));
	if (pPX->IsStoring())
		secData->EmptySection();

	m_TextLeftRight.doPX(&secData, _T("TextLeftRight"), GetTheme());
	m_TextCenter.doPX(&secData, _T("TextCenter"), GetTheme());

	COLORREF clrBase0 = m_clrBaseColor.GetStandardColor();
	COLORREF clrBase1 = m_clrTodayBaseColor.GetStandardColor();

	PX_Color(&secData, _T("BaseColor"), m_clrBaseColor);
	PX_Color(&secData, _T("TodayBaseColor"), m_clrTodayBaseColor);

	if (pPX->IsLoading() && !m_clrBaseColor.IsStandardValue())
		m_clrBaseColor.SetStandardValue(clrBase0);

	if (pPX->IsLoading() && !m_clrTodayBaseColor.IsStandardValue())
		m_clrTodayBaseColor.SetStandardValue(clrBase1);

	if (pPX->GetSchema() >= _XTP_SCHEMA_110)
	{
		m_bkNormal       .doPX(&secData, _T("bkNormal"),    GetTheme());
		m_bkSelected     .doPX(&secData, _T("bkSelected"),  GetTheme());
		m_bkToday        .doPX(&secData, _T("bkToday"),     GetTheme());
		m_bkTodaySelected.doPX(&secData, _T("bkTodaySelected"), GetTheme());
	}

	//TBase::DoPropExchange(&secData);
	XTP_SAFE_CALL_BASE(TBase, DoPropExchange(&secData));
}

void CXTPCalendarTheme::CTOHeader::Serialize(CArchive& ar)
{
	m_TextLeftRight.Serialize(ar);
	m_TextCenter.Serialize(ar);

	m_clrBaseColor.Serialize(ar);
	m_clrTodayBaseColor.Serialize(ar);

	m_bkNormal.Serialize(ar);
	m_bkSelected.Serialize(ar);
	m_bkToday.Serialize(ar);
	m_bkTodaySelected.Serialize(ar);

	//TBase::Serialize(ar);
	XTP_SAFE_CALL_BASE(TBase, Serialize(ar));
}

int CXTPCalendarTheme::CTOHeader::CalcHeight(CDC* pDC, int nCellWidth)
{
	UNREFERENCED_PARAMETER(nCellWidth);

	CArray<CFont*, CFont*> arFonts;

	arFonts.Add(m_TextLeftRight.fcsetNormal.Font());
	arFonts.Add(m_TextLeftRight.fcsetSelected.Font());
	arFonts.Add(m_TextLeftRight.fcsetToday.Font());
	arFonts.Add(m_TextLeftRight.fcsetTodaySelected.Font());

	arFonts.Add(m_TextCenter.fcsetNormal.Font());
	arFonts.Add(m_TextCenter.fcsetSelected.Font());
	arFonts.Add(m_TextCenter.fcsetToday.Font());
	arFonts.Add(m_TextCenter.fcsetTodaySelected.Font());

	int nFontHeightMax = 0;
	CXTPCalendarUtils::GetMaxHeightFont(arFonts, pDC, &nFontHeightMax);

	ASSERT(GetHeightFormulaPart());
	if (!GetHeightFormulaPart())
	{
		ASSERT(FALSE);
		return 19;
	}

	int nHeight = GetHeightFormulaPart()->Calculate(nFontHeightMax);
	return nHeight;
}


/////////////////////////////////////////////////////////////////////////////
/*CXTPCalendarTheme::CTOEventIconsToDraw::CTOEventIconsToDraw()
{
	m_pEventIconsToDraw = new CXTPCalendarIconIDs();
}

CXTPCalendarTheme::CTOEventIconsToDraw::~CTOEventIconsToDraw()
{
	CMDTARGET_RELEASE(m_pEventIconsToDraw);
}

void CXTPCalendarTheme::CTOEventIconsToDraw::Set(UINT uID)
{
	ASSERT(IconIDs());
	if (IconIDs())
		IconIDs()->AddIfNeed(uID);
}

void CXTPCalendarTheme::CTOEventIconsToDraw::Remove(UINT uID)
{
	ASSERT(IconIDs());
	if (IconIDs())
		IconIDs()->RemoveID(uID);
}

BOOL CXTPCalendarTheme::CTOEventIconsToDraw::IsSet(UINT uID)
{
	ASSERT(IconIDs());
	return IconIDs() && IconIDs()->Find(uID) >= 0;
}
*/

void CXTPCalendarTheme::RefreshMetrics(BOOL bRefreshChildren)
{
//  ClearAxPartsCache();

	m_clrBaseColor.SetStandardValue(RGB(50, 70, 200));

	LOGFONT lfIcon;
	VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0));

	LOGFONT lfBold = lfIcon;
	lfBold.lfWeight = FW_BOLD;

	m_fntBaseFont.SetStandardValue(&lfIcon);
	m_fntBaseFontBold.SetStandardValue(&lfBold);

	//----------------------------------------------------------------------
	if (m_pImagesStd)
	{
		HMODULE hResource = XTPResourceManager()->GetResourceHandle();
		if (hResource == 0)
			hResource = AfxGetInstanceHandle();

		m_pImagesStd->SetHandle(hResource);
	}

	//----------------------------------------------------------------------
	if (bRefreshChildren)
	{
		CXTPCalendarThemePart::RefreshMetrics();
	}
}


/*
CXTPCalendarTheme::CXTPThemeExchangeHelper::CXTPThemeExchangeHelper()
{
}

void CXTPCalendarTheme::CXTPThemeExchangeHelper::Clear()
{
	m_mapFonts.RemoveAll();
}

void CXTPCalendarTheme::CXTPThemeExchangeHelper::AddFont(const LOGFONT& logFont, CFont* pFont)
{
	ASSERT(pFont);
	ASSERT(m_mapFonts[logFont] == NULL);
	m_mapFonts[logFont] = pFont;
}

CFont* CXTPCalendarTheme::CXTPThemeExchangeHelper::GetFont(const LOGFONT& logFont)
{
}
*/

void CXTPCalendarTheme::DoPropExchange(CXTPPropExchange* pPX)
{
	_DoPropExchange(pPX, TRUE);
}

void CXTPCalendarTheme::_DoPropExchange(CXTPPropExchange* pPX, BOOL bExchangeChildren)
{
	pPX->ExchangeSchemaSafe();

	CXTPPropExchangeSection secData(pPX->GetSection(_T("XTPCalendarTheme")));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("BaseColor"), m_clrBaseColor);
	PX_Font(&secData, _T("BaseFont"), m_fntBaseFont);
	PX_Font(&secData, _T("BaseFontBold"), m_fntBaseFontBold);

	if (bExchangeChildren)
		CXTPCalendarThemePart::DoPropExchange(&secData);
}

//===========================================================================
int CXTPCalendarTheme::CTODayView::CalcMinCellHeight(CDC* pDC, CXTPCalendarDayView* pDayView)
{
	if (!pDC || !pDayView || !GetTheme())
	{
		ASSERT(FALSE);
		return 19;
	}

	int nTimeScaleRowHeight = 0;
	int nTimeScale2RowHeight = 0;

	if (pDayView->GetTimeScale())
	{
		nTimeScaleRowHeight = GetTimeScalePart()->CalcMinRowHeight(pDC, pDayView->GetTimeScale());
	}

	// alternative time scale
	if (pDayView->IsScale2Visible() && pDayView->GetTimeScale(2))
	{
		nTimeScale2RowHeight = GetTimeScalePart()->CalcMinRowHeight(pDC, pDayView->GetTimeScale(2));
	}

	int nMinEventHeight = GetDayPart()->GetGroupPart()->GetSingleDayEventPart()->CalcMinEventHeight(pDC);

	return max(nMinEventHeight, max(nTimeScaleRowHeight, nTimeScale2RowHeight));
}

void CXTPCalendarTheme::CTODayView::AdjustLayout(CDC* pDC, const CRect& rcRect)
{
	CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, XTP_SAFE_GET2(GetTheme(), GetCalendarControl(), GetDayView(), NULL));

	if (!pDC || !pDayView || !GetTheme())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarView::XTP_VIEW_LAYOUT& rLayout = GetTheme()->GetViewLayout(pDayView);
	CXTPCalendarDayView::XTP_DAY_VIEW_LAYOUT& rLayoutX = GetTheme()->GetDayViewLayout(pDayView);
	CRect rcView = rcRect;

	int nDaysCount = pDayView->GetViewDayCount();
	if (nDaysCount <= 0)
	{
		return;
	}
	int nGroupsCount = max(1, XTP_SAFE_GET2(pDayView, GetViewDay_(0), GetViewGroupsCount(), 1));

	int nCellWidth = max(0, rcView.Width() / max(nDaysCount, 1) - (pDayView->IsScale2Visible() ? 84 : 42));
	BOOL bGroupHeaderVisible = pDayView->IsGroupHeaderVisible();

	int nMinCellHeight = CalcMinCellHeight(pDC, pDayView);
	int nAllDayEventHeight = GetDayPart()->GetGroupPart()->GetMultiDayEventPart()->CalcMinEventHeight(pDC);
	rLayoutX.m_nAllDayEventHeight = nAllDayEventHeight;

	int nDayHeaderHeight = GetDayPart()->GetHeaderPart()->CalcHeight(pDC, nCellWidth);
	int nGroupHeaderHeight = 0;
	if (bGroupHeaderVisible)
	{
		nGroupHeaderHeight = GetDayPart()->GetGroupPart()->GetHeaderPart()->CalcHeight(pDC, nCellWidth / nGroupsCount);
	}

	//----------------------------------------------------------
	int nAllDayEventCount = rLayoutX.m_nAllDayEventsCountMax;
	int nRowCount = pDayView->GetRowCount();

	//***
	int nEventsHeight = rcView.Height() - nDayHeaderHeight - nGroupHeaderHeight;

	int nAllDayEventsHeight = max(1, nAllDayEventCount) * nAllDayEventHeight + nAllDayEventHeight / 3;
	nAllDayEventsHeight = min(nAllDayEventsHeight, nEventsHeight / 2);

	int nDayDetailsHeight = nEventsHeight - nAllDayEventsHeight;

	rLayout.m_nRowHeight = max(nMinCellHeight, nDayDetailsHeight / max(nRowCount, 1));

	rLayoutX.m_nVisibleRowCount = nDayDetailsHeight / max(rLayout.m_nRowHeight, 1);
	rLayoutX.m_nVisibleRowCount = max(0, min(nRowCount, rLayoutX.m_nVisibleRowCount));

	if (rLayoutX.m_nTopRow + rLayoutX.m_nVisibleRowCount >= rLayoutX.m_nRowCount)
	{
		rLayoutX.m_nTopRow = rLayoutX.m_nRowCount - rLayoutX.m_nVisibleRowCount;
	}

	nAllDayEventsHeight = max(0, nEventsHeight - (rLayoutX.m_nVisibleRowCount * rLayout.m_nRowHeight));

	//--------------------------------------------------------------
	int nTimeScaleWidth = 0;
	int nTimeScale2Width = 0;

	// regular time scale
	if (pDayView->GetTimeScale())
	{
		nTimeScaleWidth = GetTimeScalePart()->CalcWidth(pDC, pDayView->GetTimeScale(), rLayout.m_nRowHeight);
	}

	// alternative time scale
	if (pDayView->IsScale2Visible() && pDayView->GetTimeScale(2))
	{
		nTimeScale2Width = GetTimeScalePart()->CalcWidth(pDC, pDayView->GetTimeScale(2), rLayout.m_nRowHeight);
	}
	rcView.left += nTimeScaleWidth + nTimeScale2Width;

	//-----------------------------------------------------------------
	rLayoutX.m_rcDayHeader.SetRect(rcView.left, rcView.top, rcView.right, rcView.top + nDayHeaderHeight);

	rLayoutX.m_rcAllDayEvents = rcView;
	rLayoutX.m_rcAllDayEvents.top = rcView.top + nDayHeaderHeight + nGroupHeaderHeight;
	rLayoutX.m_rcAllDayEvents.bottom = rLayoutX.m_rcAllDayEvents.top + nAllDayEventsHeight;

	//---------------------------------------
	// regular time scales
	CRect rcTimeScale = rcRect;

	if (pDayView->IsScale2Visible() && pDayView->GetTimeScale(2))
	{
		rcTimeScale.right = rcTimeScale.left + nTimeScale2Width;
		GetTimeScalePart()->AdjustLayout(pDayView->GetTimeScale(2), pDC, rcTimeScale);
	}

	if (pDayView->GetTimeScale())
	{
		rcTimeScale.left += nTimeScale2Width;
		rcTimeScale.right = rcTimeScale.left + nTimeScaleWidth;

		GetTimeScalePart()->AdjustLayout(pDayView->GetTimeScale(), pDC, rcTimeScale);
	}

	//======================================
	CRect rcView2 = rcView;

	int nColumnsCount = pDayView->GetTotalGroupsCount();

	int nCols = 0;
	for (int i = 0; i < nDaysCount; i++)
	{
		CXTPCalendarDayViewDay* pDay = pDayView->GetViewDay(i);
		if (!pDay)
		{
			ASSERT(FALSE);
			continue;
		}

		//int nWidth = (i == nDaysCount - 1) ? rcView2.Width() : rcView2.Width() / (nDaysCount - i);
		int nWidth = (rcView2.Width() / (nColumnsCount - nCols)) * pDay->GetViewGroupsCount();
		if (i == nDaysCount - 1)
			nWidth = rcView2.Width();

		CRect rcDay(rcView2.left, rcView2.top, rcView2.left + nWidth, rcView2.bottom);
		pDay->AdjustLayout2(pDC, rcDay);

		rcView2.left += nWidth;
		nCols += pDay->GetViewGroupsCount();
	}

	//----------------------------------------------
	pDayView->AdjustAllDayEvents();
}

void CXTPCalendarTheme::CTODayView::OnPostAdjustLayout()
{
}

//===========================================================================
void CXTPCalendarTheme::CTODayViewDay::AdjustLayout(CCmdTarget* pObject, CDC* pDC,
													const CRect& rcRect)
{
	CXTPCalendarDayViewDay* pDayViewDay = DYNAMIC_DOWNCAST(CXTPCalendarDayViewDay, pObject);
	if (!pDC || !pDayViewDay || !pDayViewDay->GetView() || !GetTheme())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& rLayout = GetTheme()->GetViewDayLayout(pDayViewDay);
	CRect rcDayHeader = pDayViewDay->GetView()->GetDayHeaderRectangle();

	rLayout.m_rcDayHeader.SetRect(rcRect.left, rcDayHeader.top, rcRect.right, rcDayHeader.bottom);
	GetHeaderPart()->AdjustLayout(pObject, pDC, rLayout.m_rcDayHeader);

	CRect rcGroups = rcRect;
	rcGroups.top = rLayout.m_rcDayHeader.bottom;
	rcGroups = ExcludeDayBorder(pDayViewDay, rcGroups);

	// adjust layout of events of this day
	int nGroupsCount = pDayViewDay->GetViewGroupsCount();
	int nGroupWidth = rcGroups.Width() / max(1, nGroupsCount);

	for (int i = 0; i < nGroupsCount; i++)
	{
		CXTPCalendarDayViewGroup* pViewGroup = pDayViewDay->GetViewGroup(i);
		ASSERT(pViewGroup);

		if (pViewGroup)
		{
			CRect rcGroupI = rcGroups;
			rcGroupI.left += nGroupWidth * i;
			if (i < nGroupsCount - 1)
			{
				rcGroupI.right = rcGroupI.left + nGroupWidth;
			}

			pViewGroup->AdjustLayout2(pDC, rcGroupI);
		}
	}
}

CXTPCalendarTheme::CTODayViewTimeScale::XTP_LC_TIMEFORMAT::XTP_LC_TIMEFORMAT()
{
	bAMPM_timeFormat = TRUE;
	bLeadingZeroTime = TRUE;
	strSeparator = _T(":");
	strAM = _T("am");
	strPM = _T("pm");
}

void CXTPCalendarTheme::CTODayViewTimeScale::XTP_LC_TIMEFORMAT::RefreshMetrics()
{
	bAMPM_timeFormat = 0 == CXTPCalendarUtils::GetLocaleLong(LOCALE_ITIME);
	bLeadingZeroTime = 0 != CXTPCalendarUtils::GetLocaleLong(LOCALE_ITLZERO);
	strSeparator = CXTPCalendarUtils::GetLocaleString(LOCALE_STIME, 5);

	if (bAMPM_timeFormat)
	{
		strAM = CXTPCalendarUtils::GetLocaleString(LOCALE_S1159, 20);
		strPM = CXTPCalendarUtils::GetLocaleString(LOCALE_S2359, 20);
	}
	else
	{
		strAM = _T("");
		strPM = _T("");
	}
}

void CXTPCalendarTheme::CTODayViewTimeScale::RefreshMetrics(BOOL)
{
	ASSERT(GetTheme());
	if (!GetTheme())
	{
		return;
	}

	XTP_SAFE_CALL1(GetHeightFormulaPart(), SetStandardValue(14, 10, 1));
	m_hflaBigHourFont.SetStandardValue(2, 1, -13);

	m_localeTimeFormat.RefreshMetrics();

	//-----------------------------
	m_clrBackground.SetStandardValue(GetSysColor(COLOR_BTNFACE));
	m_clrLine.SetStandardValue(GetSysColor(COLOR_3DSHADOW));

	m_fcsetCaption.clrColor.SetStandardValue(GetSysColor(COLOR_BTNTEXT));
	m_fcsetCaption.fntFont.SetStandardValue(GetTheme()->m_fntBaseFont);

	m_fcsetAMPM.CopySettings(m_fcsetCaption);
	m_fcsetSmall.CopySettings(m_fcsetCaption);;
	m_fcsetBigBase.CopySettings(m_fcsetCaption);

	CopySettings(m_fcsetBigHour_.clrColor, m_fcsetBigBase.clrColor);

	if (m_fcsetBigHour_.Font() && m_fcsetBigHour_.Font()->m_hObject)
	{
		RefreshBigHourFontIfNeed(TRUE);
	}
	else
	{
		// copy by value (no by ref).
		LOGFONT lfBigFontBase;
		if (m_fcsetBigBase.Font()->GetLogFont(&lfBigFontBase))
			m_fcsetBigHour_.fntFont.SetStandardValue(&lfBigFontBase);
	}

	//-----------------------
	if (IsXPTheme())
	{
		m_grclrNowLineBk.SetStandardValue(::GetSysColor(COLOR_BTNFACE), RGB(250, 203, 91));
		m_clrNowLine.SetStandardValue(RGB(187, 85, 3));
	}
	else
	{
		m_grclrNowLineBk.SetStandardValue(::GetSysColor(COLOR_BTNFACE));
		m_clrNowLine.SetStandardValue(::GetSysColor(COLOR_HIGHLIGHT));
	}

	m_ShowMinutes.SetStandardValue(XTP_SAFE_GET3(GetTheme(), GetCalendarControl(), GetCalendarOptions(), bDayView_TimeScaleShowMinutes, FALSE));
}

void CXTPCalendarTheme::CTODayViewTimeScale::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("DayViewTimeScale")));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("BackgroundColor"), m_clrBackground);
	PX_Color(&secData, _T("LineColor"),      m_clrLine);
	m_fcsetCaption.doPX(&secData, _T("Caption"), GetTheme());
	m_fcsetAMPM.doPX(&secData, _T("AMPM"), GetTheme());
	m_fcsetSmall.doPX(&secData, _T("Small"), GetTheme());
	m_fcsetBigBase.doPX(&secData, _T("BigBase"), GetTheme());

	PX_GrColor(&secData, _T("NowLineBk"), m_grclrNowLineBk);
	PX_Color(&secData, _T("NowLineColor"),    m_clrNowLine);

	m_ShowMinutes.DoPropExchange(&secData, _T("ShowMinutes"));

	CXTPCalendarThemePart::DoPropExchange(&secData);
}

void CXTPCalendarTheme::CTODayViewTimeScale::Serialize(CArchive& ar)
{
	m_clrBackground.Serialize(ar);
	m_clrLine.Serialize(ar);

	m_fcsetCaption.Serialize(ar);
	m_fcsetAMPM.Serialize(ar);
	m_fcsetSmall.Serialize(ar);
	m_fcsetBigBase.Serialize(ar);

	m_grclrNowLineBk.Serialize(ar);
	m_clrNowLine.Serialize(ar);

	m_ShowMinutes.Serialize(ar);

	CXTPCalendarThemePart::Serialize(ar);
}

CString CXTPCalendarTheme::CTODayViewTimeScale::FormatTime
		(
		CXTPCalendarTheme::CTODayViewTimeScale::XTP_LC_TIMEFORMAT& lcTimeFormat,
		COleDateTime dtTime, BOOL bAmPmEnshure,
		CString& rstrHour, CString& rstrMin, CString& rstrAmPm)
{
	rstrHour = lcTimeFormat.bAMPM_timeFormat ? dtTime.Format(_T("%I")) : dtTime.Format(_T("%H"));

	if (!lcTimeFormat.bLeadingZeroTime &&
		 rstrHour.GetLength() == 2 && rstrHour[0] == _T('0'))
	{
		DELETE_S(rstrHour, 0, 1);
	}

	rstrMin = dtTime.Format(_T("%M"));
	rstrAmPm.Empty();

	if (lcTimeFormat.bAMPM_timeFormat &&
		(bAmPmEnshure || rstrHour == _T("12") && dtTime.GetMinute() == 0))
	{
		rstrAmPm = dtTime.GetHour() < 12 ? lcTimeFormat.strAM : lcTimeFormat.strPM;
		rstrAmPm.MakeLower();
	}

	CString strTime = rstrHour + lcTimeFormat.strSeparator + rstrMin;
	return strTime;
}

void CXTPCalendarTheme::CTODayViewTimeScale::AdjustLayout(CCmdTarget* pObject, CDC* pDC,
														   const CRect& rcRect)
{
	CXTPCalendarDayViewTimeScale* pTS = DYNAMIC_DOWNCAST(CXTPCalendarDayViewTimeScale, pObject);
	if (!pDC || !pTS || !pTS->GetDayView() || !GetTheme())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarDayViewTimeScale::XTP_TIMESCALE_LAYOUT& rTSLayout = GetTheme()->GetTimeScaleLayout(pTS);

	CRect rcAllDayEvents = pTS->GetDayView()->GetAllDayEventsRectangle();

	rTSLayout.m_rcHeader = rcRect;
	rTSLayout.m_nXPosBase = rTSLayout.m_rcHeader.left;

	rTSLayout.m_rcTimeZone = rcRect;
	rTSLayout.m_rcTimeZone.top = rcAllDayEvents.bottom;

	//-------------------------------------------------
	int nRowHeight = pTS->GetDayView()->GetRowHeight();
	CRect rcDefaultCell(0, 0, 500, nRowHeight);
	AdjustBigHourFont(pDC, rcDefaultCell);

	m_nWidthAmPmMinutesPart_ = CalcWidth_AmPmMinutesPart(pDC, pTS, &m_nCharWidthAmPmMinutesPart_);
}

BOOL CXTPCalendarTheme::CTODayViewTimeScale::IsDrawNowLine(CDC* pDC, CXTPCalendarDayViewTimeScale* pTS)
{
	if (!pDC || !pTS || !pTS->GetDayView() || !GetTheme())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nNowLineFlags = XTP_SAFE_GET4(pTS, GetDayView(), GetCalendarControl(), GetCalendarOptions(), nDayView_CurrentTimeMarkVisible, 0);

	BOOL bDrawNowLine = pTS->IsDrawNowLine();
	if (bDrawNowLine)
	{
		if (pTS->GetDayView()->IsTodayVisible())
		{
			int nTAMask = xtpCalendarCurrentTimeMarkVisibleForToday | xtpCalendarCurrentTimeMarkVisibleAlways;
			bDrawNowLine = bDrawNowLine && (nNowLineFlags & nTAMask);
		}
		else
		{
			bDrawNowLine = bDrawNowLine && (nNowLineFlags & xtpCalendarCurrentTimeMarkVisibleAlways);
		}

		if (pDC->IsPrinting())
		{
			bDrawNowLine = bDrawNowLine && (nNowLineFlags & xtpCalendarCurrentTimeMarkPrinted);
		}
	}

	return bDrawNowLine;
}

int CXTPCalendarTheme::CTODayViewTimeScale::CalcMinRowHeight(CDC* pDC,
								CXTPCalendarDayViewTimeScale* pTimeScale)
{
	if (!pDC || !pTimeScale)
	{
		ASSERT(FALSE);
		return 0;
	}

	CXTPFontDC autoFont(pDC);

	CSize szAMPM(0, 0);
	if (m_localeTimeFormat.bAMPM_timeFormat)
	{
		autoFont.SetFont(m_fcsetAMPM.Font());
		szAMPM = pDC->GetTextExtent(m_localeTimeFormat.strAM + m_localeTimeFormat.strPM);
	}

	if (GETTOTAL_MINUTES_DTS(pTimeScale->GetScaleInterval()) % 60 != 0)
	{
		autoFont.SetFont(m_fcsetBigBase.Font());
	}
	else
	{
		autoFont.SetFont(m_fcsetSmall.Font());
	}
	CSize szMinutes = pDC->GetTextExtent(_T("01"), 2);

	//-------------------------------------------------------------
	int nFontHeightMax = max(szAMPM.cy, szMinutes.cy);

	if (!GetHeightFormulaPart())
	{
		ASSERT(FALSE);
		return nFontHeightMax * 12 / 10 + 1;
	}
	int nHeight = GetHeightFormulaPart()->Calculate(nFontHeightMax);
	return nHeight;
}

int CXTPCalendarTheme::CTODayViewTimeScale::CalcWidth(CDC* pDC,
				CXTPCalendarDayViewTimeScale* pTimeScale, int nRowHeight)
{
	if (!pDC || !pTimeScale)
	{
		ASSERT(FALSE);
		return 0;
	}

	int nCharWidth;
	int nPart2_width = CalcWidth_AmPmMinutesPart(pDC, pTimeScale, &nCharWidth);
	nPart2_width += nCharWidth;

	CRect rcDefaultCell(0, 0, 500, nRowHeight);
	AdjustBigHourFont(pDC, rcDefaultCell);

	CXTPFontDC autoFont(pDC);

	if (GETTOTAL_MINUTES_DTS(pTimeScale->GetScaleInterval()) % 60 != 0)
	{
		autoFont.SetFont(m_fcsetBigHour_.Font());
	}
	else
	{
		nPart2_width += nCharWidth;
		autoFont.SetFont(m_fcsetSmall.Font());
	}

	CSize szHour = pDC->GetTextExtent(_T("88"), 2);

	int nWidth = szHour.cx + nPart2_width + 4 + GetRightOffsetX(pDC, pTimeScale);
	return max(42, nWidth);
}

int CXTPCalendarTheme::CTODayViewTimeScale::CalcWidth_AmPmMinutesPart(CDC* pDC,
		CXTPCalendarDayViewTimeScale* pTimeScale, int* pnCharWidth)
{
	if (!pDC || !pTimeScale)
	{
		ASSERT(FALSE);
		return 0;
	}

	CXTPFontDC autoFont(pDC);

	int nAmPm_width = 0;
	if (m_localeTimeFormat.bAMPM_timeFormat)
	{
		autoFont.SetFont(m_fcsetAMPM.Font());

		CSize szPart2_1 = pDC->GetTextExtent(m_localeTimeFormat.strAM);
		CSize szPart2_2 = pDC->GetTextExtent(m_localeTimeFormat.strPM);
		CSize szSpace = pDC->GetTextExtent(_T(" "), 1);

		nAmPm_width = max(szPart2_1.cx, szPart2_2.cx) + szSpace.cx;
	}

	if (GETTOTAL_MINUTES_DTS(pTimeScale->GetScaleInterval()) % 60 != 0)
	{
		autoFont.SetFont(m_fcsetBigBase.Font());
	}
	else
	{
		autoFont.SetFont(m_fcsetSmall.Font());
	}
	CSize szPart2_3 = pDC->GetTextExtent(_T(" 00"), 3);

	int nCharWidth = szPart2_3.cx / 3;
	if (pnCharWidth)
	{
		*pnCharWidth = nCharWidth;
	}

	int nPart2_width = max(nAmPm_width, szPart2_3.cx);

	return nPart2_width;
}

void CXTPCalendarTheme::CTODayViewTimeScale::RefreshBigHourFontIfNeed(BOOL bRefreshToStandard)
{
	LOGFONT lfBigHour_;
	LOGFONT lfBigFontBase;

	if (!m_fcsetBigBase.Font()->GetLogFont(&lfBigFontBase) ||
		!m_fcsetBigHour_.Font()->GetLogFont(&lfBigHour_))
	{
		ASSERT(FALSE);
		return;
	}

	lfBigFontBase.lfHeight = lfBigHour_.lfHeight;

	if (memcmp(&lfBigFontBase, &lfBigHour_, sizeof(lfBigHour_)))
	{
		if (bRefreshToStandard)
		{
			m_fcsetBigHour_.fntFont.SetStandardValue(&lfBigFontBase);
			ASSERT(xtp_dbg_IsFontValid(m_fcsetBigHour_.Font()->m_hObject));
		}
		else
		{
			m_fcsetBigHour_.fntFont.SetCustomValue(&lfBigFontBase);
			ASSERT(xtp_dbg_IsFontValid(m_fcsetBigHour_.Font()->m_hObject));
		}
	}
}

void CXTPCalendarTheme::CTODayViewTimeScale::AdjustBigHourFont(CDC* pDC, const CRect& rcRowCell)
{
	ASSERT(pDC);
	if (!pDC)
	{
		return;
	}
	CRect rcCell = rcRowCell;
	rcCell.bottom = rcCell.top + m_hflaBigHourFont.Calculate(rcRowCell.Height());

	if (!m_fcsetBigHour_.Font())
	{
		ASSERT(FALSE);
		return;
	}

	LOGFONT lfBigFont;
	m_fcsetBigHour_.Font()->GetLogFont(&lfBigFont);
	LOGFONT lfBigFont_new = lfBigFont;

	CXTPFontDC fontBig(pDC, m_fcsetBigHour_.Font());
	CSize szText = pDC->GetTextExtent(_T("88"), 2);

	lfBigFont_new.lfHeight = ::MulDiv(lfBigFont.lfHeight, rcCell.Height(), szText.cy);
	//-------------------------------------------------------------
	if (lfBigFont_new.lfHeight != lfBigFont.lfHeight)
	{
		fontBig.ReleaseFont(); // to unselect font from the DC before delete it to create a new one.
		m_fcsetBigHour_.fntFont.SetStandardValue(&lfBigFont_new);
		ASSERT(xtp_dbg_IsFontValid(m_fcsetBigHour_.Font()->m_hObject));
	}
}

void CXTPCalendarTheme::CTODayViewTimeScale::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarDayViewTimeScale* pTimeScale = DYNAMIC_DOWNCAST(CXTPCalendarDayViewTimeScale, pObject);
	if (!pDC || !pTimeScale || !pTimeScale->GetDayView())
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarDayView* pDayView = pTimeScale->GetDayView();
	LPARAM dwTSnumber = pTimeScale == pDayView->GetTimeScale(1) ? 1 : 2;

	CXTPPropsStateContext autoStateCnt1(this, xtpCalendarBeforeDraw_DayViewTimeScale, TRUE);
	autoStateCnt1.SendBeforeDrawThemeObject((LPARAM)dwTSnumber);

	const CXTPCalendarDayViewTimeScale::XTP_TIMESCALE_LAYOUT& layoutTS = CXTPCalendarTheme::GetTimeScaleLayout(pTimeScale);

	pDC->FillSolidRect(layoutTS.m_rcHeader, m_clrBackground);
	pDC->SetBkMode(TRANSPARENT);

	CRect rcNowLine(0, 0, 0, 0);
	BOOL bDrawNowLine = IsDrawNowLine(pDC, pTimeScale);

	// draw 'now' line Bk
	if (bDrawNowLine)
	{
		rcNowLine = Calc_NowLineRect(pDC, pTimeScale);
		Draw_NowLineBk(pDC, rcNowLine);
	}

	// Draw RIGHT border ------------------------------------------
	if (pTimeScale == pDayView->GetTimeScale(2))
	{
		CRect rcBorderR = layoutTS.m_rcTimeZone;
		rcBorderR.left = rcBorderR.right - 1;

		pDC->FillSolidRect(&rcBorderR, m_clrLine);
	}

	int nRowHeight = XTP_SAFE_GET1(pDayView, GetRowHeight(), 0);
	if (nRowHeight <= 0)
		return;

	const int nRowCount = pDayView->GetVisibleRowCount();
	const int nTopRow = pDayView->GetTopRow();
	COleDateTimeSpan spScaleInterval = pTimeScale->GetScaleInterval();

	if (GETTOTAL_MINUTES_DTS(spScaleInterval) < 1)
	{
		ASSERT(FALSE);
		spScaleInterval.SetDateTimeSpan(0, 0, 5, 0);
	}

	int nTimeShiftCorrectorY = 0;
	COleDateTimeSpan spShiftCorrector(0);
	int nScaleInterval_min = max(1, (int)GETTOTAL_MINUTES_DTS(spScaleInterval));
	double dPixelPerMin = (double)nRowHeight / (double)nScaleInterval_min;
	int nTimeshift_min = pTimeScale->GetTimeshift();

	if (GETTOTAL_HOURS_DTS(spScaleInterval) < 1)
	{
		int nMul = nTimeshift_min / 60;
		int nMod = nTimeshift_min % 60;
		int nShiftCorrector_min = (nMul + (nMod ? 1 :0))*60 - nTimeshift_min;

		//-------------------------------------------------------------------
		int nMul2 = nShiftCorrector_min / nScaleInterval_min;
		int nMod2 = nShiftCorrector_min % nScaleInterval_min;
		int nShiftCorrector2_min = (nMul2 + (nMod2 ? 1 :0)) * nScaleInterval_min;

		nTimeShiftCorrectorY = (int)(dPixelPerMin * nShiftCorrector2_min);

		spShiftCorrector = CXTPCalendarUtils::Minutes2Span(nShiftCorrector_min);
	}

	COleDateTimeSpan spShift = CXTPCalendarUtils::Minutes2Span(nTimeshift_min);

	CXTPPropsStateContext autoStateCnt2(this, xtpCalendarBeforeDraw_DayViewTimeScaleCell);

	// draw time scale rows
	if (GETTOTAL_HOURS_DTS(spScaleInterval) >= 1)
	{
		// draw time scale cells one by one
		for (int i = 0; i < nRowCount; i++)
		{
			CRect rcCell(layoutTS.m_rcTimeZone.left,
				layoutTS.m_rcTimeZone.top + nRowHeight * i,
				layoutTS.m_rcTimeZone.right,
				layoutTS.m_rcTimeZone.top + nRowHeight * (i + 1));

			COleDateTime dtCell = pDayView->GetCellTime(nTopRow + i);

			dtCell += spShift;
			BOOL bAmPmEnshure = dtCell.GetHour() == 12 || dtCell.GetHour() == 0;
			CString strHour, strMin, strAmPm;
			FormatTime(m_localeTimeFormat, dtCell, bAmPmEnshure,
										 strHour, strMin, strAmPm);

			BOOL bAmPm = !strAmPm.IsEmpty();

			CString strPart2;
			if (bAmPm)
			{
				strPart2 = _T(" ") + strAmPm;
			}
			else
			{
				strPart2 = m_localeTimeFormat.strSeparator + strMin;
			}

			XTP_CALENDAR_THEME_DAYVIEWTIMESCALECELL_PARAMS tsCell;

			if (GetTheme()->IsBeforeDraw(xtpCalendarBeforeDraw_DayViewTimeScaleCell))
			{
				::ZeroMemory(&tsCell, sizeof(tsCell));

				tsCell.nTimeScale = (int)dwTSnumber;
				tsCell.nIndex = nTopRow + i;
				tsCell.dtBeginTime = dtCell;
				tsCell.nMinutes = -1;

				autoStateCnt2.SendBeforeDrawThemeObject((LPARAM)&tsCell);
			}

			Draw_SmallHourCell(pDC, pTimeScale, rcCell, strHour, strPart2,
							   &m_fcsetSmall, bAmPm ? &m_fcsetAMPM : &m_fcsetSmall,
							   autoStateCnt2, tsCell);

			autoStateCnt2.RestoreState();
		}
	}
	else
	{
		COleDateTimeSpan spHour(0, 1, 0, 0);

		spShift += spShiftCorrector;

		int nRowPerHour = (int)((double)spHour / (double)spScaleInterval + XTP_HALF_SECOND);
		ASSERT(nRowPerHour > 0);
		nRowPerHour = max(1, nRowPerHour);
		const int nHourCellHeight = nRowHeight * nRowPerHour;

		// Adjust time font size
		CRect rcStartCell(layoutTS.m_rcTimeZone);

		rcStartCell.top += nTimeShiftCorrectorY;
		rcStartCell.bottom = rcStartCell.top + nHourCellHeight;

		//-------------------------------------------------------------------
		int nHrCellDiv = layoutTS.m_rcTimeZone.Height() / rcStartCell.Height();
		int nHrCellMod = layoutTS.m_rcTimeZone.Height() / rcStartCell.Height();

		int nDrawCellCount = nHrCellDiv + (nHrCellMod ? 1 : 0);

		COleDateTime dtStartCell = pDayView->GetCellTime(nTopRow);
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

		for (int n = 0; rcStartCell.top > layoutTS.m_rcTimeZone.top && n < 100; n++)
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

			BOOL bAmPmEnshure = bTopCell; //dtTime.GetHour() == 12 || dtTime.GetHour() == 0;

			if (bTopCell && rcCell.top >= layoutTS.m_rcTimeZone.top)
			{
				bTopCell = FALSE;
			}

			CString strHour, strMin, strAmPm;
			FormatTime(m_localeTimeFormat, dtCell, bAmPmEnshure,
										 strHour, strMin, strAmPm);

			BOOL bAmPm = !strAmPm.IsEmpty();
			XTP_CALENDAR_THEME_DAYVIEWTIMESCALECELL_PARAMS tsCell;

			if (GetTheme()->IsBeforeDraw(xtpCalendarBeforeDraw_DayViewTimeScaleCell))
			{
				::ZeroMemory(&tsCell, sizeof(tsCell));

				tsCell.nTimeScale = (int)dwTSnumber;
				tsCell.nIndex = dtCell.GetHour();
				tsCell.dtBeginTime = dtCell;
				tsCell.nMinutes = -1;

				autoStateCnt2.SendBeforeDrawThemeObject((LPARAM)&tsCell);

				m_fcsetBigHour_.clrColor = m_fcsetBigBase.clrColor;
				RefreshBigHourFontIfNeed(FALSE);
			}

			// draw text
			Draw_BigHourCell(pDC, pTimeScale, rcCell, nRowPerHour, strHour,
							 bAmPm ? strAmPm : strMin, &m_fcsetBigHour_,
							 bAmPm ? &m_fcsetAMPM : &m_fcsetBigBase,
							 autoStateCnt2, tsCell);

			autoStateCnt2.RestoreState();

			rcCell.top += nHourCellHeight;
			rcCell.bottom += nHourCellHeight;
		}
	}

	autoStateCnt2.Clear();

	// restore from changes which was made in RefreshBigHourFontIfNeed() method calls.
	m_fcsetBigHour_.fntFont.SetDefaultValue();
	m_fcsetBigHour_.clrColor = m_fcsetBigBase.clrColor;
	ASSERT(xtp_dbg_IsFontValid(m_fcsetBigHour_.Font()->m_hObject));

	// draw 'now' line
	if (bDrawNowLine)
	{
		Draw_NowLine(pDC, rcNowLine);
	}

	// draw time scale header -------------------------------------
	autoStateCnt1.SendBeforeDrawThemeObject((LPARAM)dwTSnumber, xtpCalendarBeforeDraw_DayViewTimeScaleCaption);

	CRect rcHeader = layoutTS.m_rcHeader;
	rcHeader.bottom = layoutTS.m_rcTimeZone.top;
	Draw_Caption(pDC, rcHeader, pTimeScale->GetCaption(), &m_fcsetCaption);

	// Draw TOP border --------------------------------------------
	CRect rcBorderT = layoutTS.m_rcTimeZone;
	rcBorderT.top -= 1;
	rcBorderT.bottom = rcBorderT.top + 1;
	pDC->FillSolidRect(&rcBorderT, m_clrLine);
}

CRect CXTPCalendarTheme::CTODayViewTimeScale::Calc_NowLineRect(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale)
{
	UNREFERENCED_PARAMETER(pDC);

	if (!pTimeScale || !pTimeScale->GetDayView())
	{
		ASSERT(FALSE);
		return CRect(0, 0, 0, 0);
	}

	CXTPCalendarDayView* pDayView = pTimeScale->GetDayView();
	const CXTPCalendarDayViewTimeScale::XTP_TIMESCALE_LAYOUT& layoutTS = CXTPCalendarTheme::GetTimeScaleLayout(pTimeScale);

	const int nTopRow = pDayView->GetTopRow();
	COleDateTimeSpan spScaleInterval = pTimeScale->GetScaleInterval();

	int nBottomRow = nTopRow + pDayView->GetVisibleRowCount();
	COleDateTime dtTopTime = pDayView->GetCellTime(nTopRow);
	COleDateTime dtBottomTime = pDayView->GetCellTime(nBottomRow);
	COleDateTimeSpan spView = dtBottomTime - dtTopTime;

	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
	dtNow = CXTPCalendarUtils::ResetDate(dtNow);

	COleDateTimeSpan spNow = dtNow - dtTopTime;

	double dPixelPerSecond = layoutTS.m_rcTimeZone.Height() / (double)GETTOTAL_SECONDS_DTS(spView);

	int nDelta = int(GETTOTAL_SECONDS_DTS(spNow) * dPixelPerSecond);
	int y = layoutTS.m_rcTimeZone.top + nDelta;

	// draw 'now' line
	int nHeight = GETTOTAL_MINUTES_DTS(spScaleInterval) >= 60 ? 5 : 10;

	CRect rcNowLine = layoutTS.m_rcHeader;
	rcNowLine.top = y - nHeight;
	rcNowLine.bottom = y + 1;
	rcNowLine.left += 5;
	rcNowLine.right -= 1;

	return rcNowLine;
}

void CXTPCalendarTheme::CTODayViewTimeScale::Draw_NowLineBk(CDC* pDC, const CRect& rcRect)
{
	CRect rcNowBk = rcRect;
	rcNowBk.bottom--;
	XTPDrawHelpers()->GradientFill(pDC, &rcNowBk, m_grclrNowLineBk, FALSE);
}

void CXTPCalendarTheme::CTODayViewTimeScale::Draw_NowLine(CDC* pDC, const CRect& rcRect)
{
	CRect rcNowLine2 = rcRect;
	rcNowLine2.top = rcNowLine2.bottom - 1;

	pDC->FillSolidRect(&rcNowLine2, m_clrNowLine);
}

int CXTPCalendarTheme::CTODayViewTimeScale::GetRightOffsetX(CDC* pDC, CXTPCalendarDayViewTimeScale* pTimeScale)
{
	if (!pDC || !pTimeScale || !pTimeScale->GetDayView())
	{
		ASSERT(FALSE);
		return 4;
	}

	return (pTimeScale == pTimeScale->GetDayView()->GetTimeScale(2)) ? 0 : 4;
}

void CXTPCalendarTheme::CTODayViewTimeScale::Draw_SmallHourCell(CDC* pDC,
		CXTPCalendarDayViewTimeScale* pTimeScale,
		const CRect& rcCell, const CString& strPart1, const CString& strPart2,
		CThemeFontColorSetValue* pfcsetPart1, CThemeFontColorSetValue* pfcsetPart2,
		CXTPPropsStateContext& autoStateCnt, XTP_CALENDAR_THEME_DAYVIEWTIMESCALECELL_PARAMS& tsCell)
{
	if (!pDC || !pTimeScale || !pfcsetPart1 || !pfcsetPart2)
	{
		ASSERT(FALSE);
		return;
	}

	//-----------------------------------------------
	int nROffsetX = GetRightOffsetX(pDC, pTimeScale);

	CRect rcPart1 = rcCell;
	rcPart1.right -= m_nWidthAmPmMinutesPart_ + nROffsetX + m_nCharWidthAmPmMinutesPart_;
	rcPart1.right -= nROffsetX ? 0 : m_nCharWidthAmPmMinutesPart_ / 2;
	rcPart1.right = max(rcPart1.left, rcPart1.right);

	CRect rcPart2 = rcCell;
	rcPart2.left = rcPart1.right + max(1, m_nCharWidthAmPmMinutesPart_ / 4);

	//-----------------------------------------------
	CXTPFontDC autoFont(pDC, pfcsetPart1->Font(), pfcsetPart1->Color());
	pDC->DrawText(strPart1, rcPart1, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);

	//-----------------------------------------------
	// draw minutes text
	tsCell.nMinutes = 0;
	autoStateCnt.SendBeforeDrawThemeObject((LPARAM)&tsCell);

	autoFont.SetFontColor(pfcsetPart2->Font(), pfcsetPart2->Color());
	pDC->DrawText(strPart2, rcPart2, DT_VCENTER | DT_LEFT | DT_SINGLELINE);

	//autoStateCnt.RestoreState();

	//------------------------------------------------
	CRect rcBorderB = rcCell;
	rcBorderB.right -= GetRightOffsetX(pDC, pTimeScale);
	rcBorderB.left += 5;
	rcBorderB.top = rcBorderB.bottom - 1;

	pDC->FillSolidRect(&rcBorderB, m_clrLine);
}

void CXTPCalendarTheme::CTODayViewTimeScale::Draw_BigHourCell(CDC* pDC,
							  CXTPCalendarDayViewTimeScale* pTimeScale,
							  const CRect& rcCell, int nRowPerHour,
							  const CString& strHour, const CString& strMinAmPm,
							  CThemeFontColorSetValue* pfcsetHour,
							  CThemeFontColorSetValue* pfcsetMinAmPm,
							  CXTPPropsStateContext& autoStateCnt,
							  XTP_CALENDAR_THEME_DAYVIEWTIMESCALECELL_PARAMS& tsCell)
{
	if (!pDC || !pTimeScale || !pfcsetHour || !pfcsetMinAmPm)
	{
		ASSERT(FALSE);
		return;
	}

	ASSERT(nRowPerHour > 0);
	int nRowHeight = rcCell.Height() / max(1, nRowPerHour);

	int nROffsetX = GetRightOffsetX(pDC, pTimeScale);
	CRect rcMinutes(rcCell);
	rcMinutes.top += 2;
	rcMinutes.right -= nROffsetX ? nROffsetX : m_nCharWidthAmPmMinutesPart_ / 2;
	rcMinutes.left = max(rcCell.left, rcMinutes.right - m_nWidthAmPmMinutesPart_ + m_nCharWidthAmPmMinutesPart_ / 2);
	//rcMinutes.bottom = rcMinutes.top + nRowHeight;

	CRect rcHour(rcCell);
	rcHour.top += 2;
	//rcHour.right = max(rcHour.left, rcCell.right - m_nWidthAmPmMinutesPart_ - nROffsetX);
	rcHour.right = rcMinutes.left - m_nCharWidthAmPmMinutesPart_/2;
	//rcHour.bottom = rcHour.top + nRowHeight* 2 - 7;

	// Draw hours text
	CXTPFontDC autoFont(pDC, pfcsetHour->Font(), pfcsetHour->Color());
	pDC->DrawText(strHour, &rcHour, DT_TOP | DT_RIGHT | DT_SINGLELINE | DT_NOCLIP);

	BOOL bShowMinutes0 = m_ShowMinutes;
	// draw minutes text
	tsCell.nMinutes = 0;
	autoStateCnt.SendBeforeDrawThemeObject((LPARAM)&tsCell);

	if (!autoStateCnt.IsActive() || m_ShowMinutes)
	{
		autoFont.SetFontColor(pfcsetMinAmPm->Font(), pfcsetMinAmPm->Color());
		pDC->DrawText(strMinAmPm, &rcMinutes, DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
	}

	autoStateCnt.RestoreState();

	// draw small lines
	if (nRowPerHour > 2)
	{
		CString strMinEx;

		int nLeftX = max(rcHour.left, rcCell.right - m_nWidthAmPmMinutesPart_ - max(nROffsetX, 1) );
		for (int i = 1; i < nRowPerHour; i++)
		{
			int y = rcCell.top + i * nRowHeight;
			pDC->FillSolidRect(nLeftX, y - 1, m_nWidthAmPmMinutesPart_, 1, m_clrLine);

			//------------------------------------------
			int nMinEx = 60 / nRowPerHour * i;

			if (bShowMinutes0)
			{
				tsCell.nMinutes = nMinEx;
				autoStateCnt.SendBeforeDrawThemeObject((LPARAM)&tsCell);

				if (m_ShowMinutes)
				{
					strMinEx.Format(_T("%02d"), nMinEx);

					CRect rcMinEx = rcMinutes;
					rcMinEx.top = y;
					rcMinEx.bottom = y + nRowHeight;

					if (autoStateCnt.IsActive())
						autoFont.SetFontColor(pfcsetMinAmPm->Font(), pfcsetMinAmPm->Color());

					pDC->DrawText(strMinEx, &rcMinEx, DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
				}

				autoStateCnt.RestoreState();
			}
		}
	}

	// draw RIGHT border
	// pDC->FillSolidRect(rcCell.right - 1, rcCell.top, 1, rcCell.Height(), m_clrLine);

	// draw line below
	CRect rcBottom = rcCell;
	rcBottom.DeflateRect(5, 0, nROffsetX ? nROffsetX : m_nCharWidthAmPmMinutesPart_ / 2, 0);
	pDC->FillSolidRect(rcBottom.left, rcBottom.bottom - 1, rcBottom.Width(), 1, m_clrLine);
}

void CXTPCalendarTheme::CTODayViewTimeScale::Draw_Caption(CDC* pDC, const CRect& rcRect,
					const CString& strCaption, CThemeFontColorSetValue* pfcsetCaption)
{
	if (!pDC || !pfcsetCaption)
	{
		ASSERT(FALSE);
		return;
	}

	pDC->FillSolidRect(&rcRect, m_clrBackground);

	CRect rcText = rcRect;
	rcText.DeflateRect(1, 2, 2, 2);

	CXTPFontDC autoFont(pDC, pfcsetCaption->Font(), pfcsetCaption->Color());
	pDC->DrawText(strCaption, &rcText, DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
}

void CXTPCalendarTheme::CTODayViewDayGroupCell::XTP_GROUP_CELL_COLOR_SET::
		doPX(CXTPPropExchange* pPX, LPCTSTR pcszPropName, CXTPCalendarTheme* pTheme)
{
	if (!pPX || !pcszPropName || !pTheme)
	{
		ASSERT(FALSE);
		return;
	}
	CXTPPropExchangeSection secData(pPX->GetSection(pcszPropName));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("BackgroundColor"),        clrBackground);
	PX_Color(&secData, _T("BorderBottomInHourColor"), clrBorderBottomInHour);
	PX_Color(&secData, _T("BorderBottomHourColor"),  clrBorderBottomHour);
	PX_Color(&secData, _T("SelectedColor"),          clrSelected);
}

void CXTPCalendarTheme::CTODayViewDayGroupCell::XTP_GROUP_CELL_COLOR_SET::
		Serialize(CArchive& ar)
{
	clrBackground.Serialize(ar);
	clrBorderBottomInHour.Serialize(ar);
	clrBorderBottomHour.Serialize(ar);
	clrSelected.Serialize(ar);
}

void CXTPCalendarTheme::CTODayViewDayGroupCell::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("DayViewDayGroupCell")));

	m_clrsetWorkCell.doPX(&secData, _T("WorkCell"), GetTheme());
	m_clrsetNonWorkCell.doPX(&secData, _T("NonWorkCell"), GetTheme());

	CXTPCalendarThemePart::DoPropExchange(&secData);
}

void CXTPCalendarTheme::CTODayViewDayGroupCell::Serialize(CArchive& ar)
{
	m_clrsetWorkCell.Serialize(ar);
	m_clrsetNonWorkCell.Serialize(ar);

	CXTPCalendarThemePart::Serialize(ar);
}

void CXTPCalendarTheme::CTODayViewDayGroupAllDayEvents::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("DayViewDayGroupAllDayEvents")));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("BackgroundColor"),       m_clrBackground);
	PX_Color(&secData, _T("BackgroundSelectedColor"),m_clrBackgroundSelected);
	PX_Color(&secData, _T("BottomBorderColor"),     m_clrBottomBorder);

	CXTPCalendarThemePart::DoPropExchange(&secData);
}

void CXTPCalendarTheme::CTODayViewDayGroupAllDayEvents::Serialize(CArchive& ar)
{
	m_clrBackground.Serialize(ar);
	m_clrBackgroundSelected.Serialize(ar);
	m_clrBottomBorder.Serialize(ar);

	CXTPCalendarThemePart::Serialize(ar);;
}

void CXTPCalendarTheme::CTODayViewDayGroup::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
		return;

	CXTPPropExchangeSection secData(pPX->GetSection(_T("DayViewDayGroup")));
	if (pPX->IsStoring())
		secData->EmptySection();

	PX_Color(&secData, _T("BorderLeftColor"), m_clrBorderLeft);

	CXTPCalendarThemePart::DoPropExchange(&secData);
}

void CXTPCalendarTheme::CTODayViewDayGroup::Serialize(CArchive& ar)
{
	m_clrBorderLeft.Serialize(ar);

	CXTPCalendarThemePart::Serialize(ar);
}

void CXTPCalendarTheme::CTODayViewDayGroup::AdjustLayout(CCmdTarget* pObject, CDC* pDC,
														 const CRect& rcRect)
{
	CXTPCalendarDayViewGroup* pDayViewGroup = DYNAMIC_DOWNCAST(CXTPCalendarDayViewGroup, pObject);
	if (!pDC || !pDayViewGroup || !pDayViewGroup->GetViewDay()->GetView() || !GetTheme())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarDayView* pView = pDayViewGroup->GetViewDay()->GetView();

	CXTPCalendarViewGroup::XTP_VIEW_GROUP_LAYOUT& rLayout = GetTheme()->GetViewGroupLayout(pDayViewGroup);
	CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& rLayoutX = GetTheme()->GetDayViewGroupLayout(pDayViewGroup);

//  if (pDayViewGroup->GetGroupIndex() == 0)
//  {
//      rcGroups = ExcludeDayBorder(pDayViewGroup->GetViewDay(), rcRect);
//  }

	CRect rcAllDayEvents = pView->GetAllDayEventsRectangle();

	rLayoutX.m_bShowHeader = pView->IsGroupHeaderVisible();
	if (rLayoutX.m_bShowHeader)
	{
		rLayout.m_rcGroupHeader.bottom = rcAllDayEvents.top;
	}

	rLayoutX.m_rcAllDayEvents = rcRect;
	rLayoutX.m_rcAllDayEvents.top = rLayout.m_rcGroupHeader.bottom;
	rLayoutX.m_rcAllDayEvents.bottom = rLayoutX.m_rcAllDayEvents.top + rcAllDayEvents.Height();

	rLayoutX.m_rcDayDetails = rcRect;
	rLayoutX.m_rcDayDetails.top = rLayoutX.m_rcAllDayEvents.bottom;

	int nAllDayEventPlace = 0;

	// adjust layout of events of this day
	//m_bExpandUP = FALSE;
	//m_bExpandDOWN = FALSE;

	int nEventsCount = pDayViewGroup->GetViewEventsCount();
	for (int i = 0; i < nEventsCount; i++)
	{
		CXTPCalendarDayViewEvent* pViewEvent = pDayViewGroup->GetViewEvent(i);
		ASSERT(pViewEvent);

		if (pViewEvent && pViewEvent->IsMultidayEvent())
		{
			rcAllDayEvents = rLayoutX.m_rcAllDayEvents;
			//rcAllDayEvents.top += 1;
			pViewEvent->AdjustLayout2(pDC, rcAllDayEvents, nAllDayEventPlace);
			nAllDayEventPlace++;
		}
	}

	//----------------------------------------------
	AdjustDayEvents(pDayViewGroup, pDC);
}

void CXTPCalendarTheme::CTODayViewDayGroup::AdjustDayEvents(CXTPCalendarDayViewGroup* pDayViewGroup, CDC* pDC)
{
	if (!pDayViewGroup || !pDC)
	{
		ASSERT(FALSE);
		return;
	}
	pDayViewGroup->AdjustDayEvents(pDC);
}

void CXTPCalendarTheme::CTODayViewEvent::AdjustLayout(CCmdTarget* pObject, CDC* pDC,
														 const CRect& rcRect, int nEventPlaceNumber)
{
	UNREFERENCED_PARAMETER(rcRect);
	UNREFERENCED_PARAMETER(nEventPlaceNumber);

	CXTPCalendarDayViewEvent* pDayViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarDayViewEvent, pObject);
	if (!pDC || !XTP_SAFE_GET3(pDayViewEvent, GetViewGroup(), GetViewDay(), GetView(), NULL))
	{
		ASSERT(FALSE);
		return;
	}

	//CXTPCalendarDayView* pView = pDayViewGroup->GetViewDay()->GetView();
}

void CXTPCalendarTheme::CTODayViewEvent_MultiDay::AdjustLayout(CCmdTarget* pObject, CDC* pDC,
														 const CRect& rcRect, int nEventPlaceNumber)
{
	UNREFERENCED_PARAMETER(rcRect);
	UNREFERENCED_PARAMETER(nEventPlaceNumber);

	CXTPCalendarDayViewEvent* pDayViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarDayViewEvent, pObject);
	if (!pDC || !XTP_SAFE_GET3(pDayViewEvent, GetViewGroup(), GetViewDay(), GetView(), NULL))
	{
		ASSERT(FALSE);
		return;
	}

}

void CXTPCalendarTheme::CTODayViewEvent_SingleDay::AdjustLayout(CCmdTarget* pObject, CDC* pDC,
														 const CRect& rcRect, int nEventPlaceNumber)
{
	UNREFERENCED_PARAMETER(rcRect);
	UNREFERENCED_PARAMETER(nEventPlaceNumber);

	CXTPCalendarDayViewEvent* pDayViewEvent = DYNAMIC_DOWNCAST(CXTPCalendarDayViewEvent, pObject);
	if (!pDC || !XTP_SAFE_GET3(pDayViewEvent, GetViewGroup(), GetViewDay(), GetView(), NULL))
	{
		ASSERT(FALSE);
		return;
	}

	//CXTPCalendarDayView* pView = pDayViewGroup->GetViewDay()->GetView();
}

CXTPCalendarViewPartBrushValue* CXTPCalendarTheme::CTODayViewEvent_SingleDay::_GetBusyStatusBrushValue(
									int nBusyStatus, BOOL bCreateIfNotExist)
{
	CXTPCalendarViewPartBrushValue* pBrushVal = NULL;
	if (m_mapBusyStatusBrushes.Lookup(nBusyStatus, pBrushVal) && pBrushVal)
	{
		return pBrushVal;
	}

	pBrushVal = NULL;
	if (bCreateIfNotExist)
	{
		pBrushVal = new CXTPCalendarViewPartBrushValue();
		if (pBrushVal)
		{
			m_mapBusyStatusBrushes[nBusyStatus] = pBrushVal;
		}
	}
	return pBrushVal;
}

void CXTPCalendarTheme::CTODayViewDayGroupAllDayEvents::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarDayViewGroup* pDayViewGroup = DYNAMIC_DOWNCAST(CXTPCalendarDayViewGroup, pObject);
	if (!pDC || !pDayViewGroup || !pDayViewGroup->GetViewDay())
	{
		ASSERT(FALSE);
		return;
	}
	CRect rcRectBk = pDayViewGroup->GetAllDayEventsRect();
	CRect rcRect2 = rcRectBk;

	if (rcRectBk.IsRectEmpty())
	{
		return;
	}

	rcRectBk.bottom--;
	rcRect2.top = rcRectBk.bottom;

	BOOL bIsSelected = GetTheme()->GetDayViewPart()->GetDayPart()->GetGroupPart()->IsSelected(pDayViewGroup);

	COLORREF clrBk = bIsSelected ? m_clrBackgroundSelected : m_clrBackground;
	pDC->FillSolidRect(&rcRectBk, clrBk);
	pDC->FillSolidRect(&rcRect2, m_clrBottomBorder);
}


CBrush* CXTPCalendarTheme::CTODayViewEvent_SingleDay::GetBusyStatusBrush(int nBusyStatus)
{
	CXTPCalendarViewPartBrushValue* pBVal = _GetBusyStatusBrushValue(nBusyStatus, FALSE);
	if (pBVal)
	{
		CBrush* pBrush = pBVal->GetValue();
		return pBrush;
	}
	return NULL;
}

void CXTPCalendarTheme::CTODayViewEvent_SingleDay::SetBusyStatusBrush(
								int nBusyStatus, CBrush* pBrush, BOOL bAutodeleteBrush)
{
	if (nBusyStatus < 0)
	{
		ASSERT(FALSE);
		return;
	}

	if (pBrush)
	{
		CXTPCalendarViewPartBrushValue* pBVal = _GetBusyStatusBrushValue(nBusyStatus, TRUE);
		if (pBVal)
		{
			pBVal->SetCustomValue(pBrush, bAutodeleteBrush);
		}
	}
	else
	{
		m_mapBusyStatusBrushes.RemoveKey(nBusyStatus);
	}
}

void CXTPCalendarTheme::CTODayViewEvent_SingleDay::InitBusyStatusDefaultColors()
{
	m_brushTentative.DeleteObject();
	m_bmpTentativePattern.DeleteObject();

	VERIFY(XTPResourceManager()->LoadBitmap(&m_bmpTentativePattern, XTP_IDB_CALENDAR_TENATIVE_PATTERN));
	VERIFY(m_brushTentative.CreatePatternBrush(&m_bmpTentativePattern));

	CXTPCalendarViewPartBrushValue* pBVal_Free = _GetBusyStatusBrushValue(xtpCalendarBusyStatusFree, TRUE);
	CXTPCalendarViewPartBrushValue* pBVal_Tentative = _GetBusyStatusBrushValue(xtpCalendarBusyStatusTentative, TRUE);
	CXTPCalendarViewPartBrushValue* pBVal_Busy = _GetBusyStatusBrushValue(xtpCalendarBusyStatusBusy, TRUE);
	CXTPCalendarViewPartBrushValue* pBVal_OutOO = _GetBusyStatusBrushValue(xtpCalendarBusyStatusOutOfOffice, TRUE);

	if (pBVal_Free)
	{
		pBVal_Free->SetStandardValue(new CBrush(RGB(0xFF, 0xFF, 0xFF)), TRUE);
	}
	if (pBVal_Tentative)
	{
		pBVal_Tentative->SetStandardValue(&m_brushTentative, FALSE);
	}
	if (pBVal_Busy)
	{
		pBVal_Busy->SetStandardValue(new CBrush(RGB(0, 0, 0xFF)), TRUE);
	}
	if (pBVal_OutOO)
	{
		pBVal_OutOO->SetStandardValue(new CBrush(RGB(132, 0, 132)), TRUE);
	}
}


void CXTPCalendarTheme::CTODayView::Draw(CDC* pDC)
{
	CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, XTP_SAFE_GET2(GetTheme(), GetCalendarControl(), GetDayView(), NULL));

	if (!pDC || !pDayView)
	{
		ASSERT(FALSE);
		return;
	}

	pDC->SetBkMode(TRANSPARENT);

	CXTPPropsStateContext autoStateCnt1(GetDayPart(), xtpCalendarBeforeDraw_DayViewDay, TRUE);

	// days
	int nCount = pDayView->GetViewDayCount();
	for (int i = nCount - 1; i >= 0; i--)
	{
		CXTPCalendarDayViewDay* pViewDay = pDayView->GetViewDay(i);
		if (pViewDay)
		{
			autoStateCnt1.SendBeforeDrawThemeObject((LPARAM)pViewDay);

			GetDayPart()->Draw(pViewDay, pDC);

			autoStateCnt1.RestoreState();
		}
	}

	autoStateCnt1.Clear();

	// All saved data must be freed after draw!
	ASSERT(CXTPPropsState::s_dbg_nActiveStartes == 0);

	// alternative time scale
	if (pDayView->IsScale2Visible() && pDayView->GetTimeScale(2))
	{
		GetTimeScalePart()->Draw(pDayView->GetTimeScale(2), pDC);
	}

	// regular time scale
	GetTimeScalePart()->Draw(pDayView->GetTimeScale(), pDC);
}

void CXTPCalendarTheme::CTODayViewDay::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarDayViewDay* pDayViewDay = DYNAMIC_DOWNCAST(CXTPCalendarDayViewDay, pObject);
	if (!pDC || !pDayViewDay || !pDayViewDay->GetView())
	{
		ASSERT(FALSE);
		return;
	}

	// draw day border
	Draw_DayBorder(pDayViewDay, pDC);

	CXTPPropsStateContext autoStateCntGroup(GetGroupPart(), xtpCalendarBeforeDraw_DayViewDayGroup);

	// draw groups ----------------------------------------------------------
	int nGroupsCount = pDayViewDay->GetViewGroupsCount();
	for (int i = 0; i < nGroupsCount; i++)
	{
		CXTPCalendarDayViewGroup* pViewGroup = pDayViewDay->GetViewGroup(i);
		ASSERT(pViewGroup);

		if (pViewGroup)
		{
			autoStateCntGroup.SendBeforeDrawThemeObject((LPARAM)pViewGroup);

			GetGroupPart()->Draw(pViewGroup, pDC);

			autoStateCntGroup.RestoreState();
		}
	}
	autoStateCntGroup.Clear();

	// draw header part
	GetHeaderPart()->Draw(pDayViewDay, pDC);
}

void CXTPCalendarTheme::CTODayViewDayGroup::Draw(CCmdTarget* pObject, CDC* pDC)
{
	CXTPCalendarDayViewGroup* pDayViewGroup = DYNAMIC_DOWNCAST(CXTPCalendarDayViewGroup, pObject);
	if (!pDC || !pDayViewGroup || !pDayViewGroup->GetViewDay()->GetView())
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarDayView* pDayView = pDayViewGroup->GetViewDay()->GetView();

	int nRowHeight = pDayViewGroup->GetRowHeight();

	if (nRowHeight <= 0)
	{
		return;
	}

	//-----------------------------------------------------------------------
	int nVisibleRowCount = pDayViewGroup->GetVisibleRowCount();
	int nTopRow = pDayViewGroup->GetTopRow();

	BOOL bHasSelectedEvents = pDayView->HasSelectedViewEvent();
	int nGroupIndex = pDayViewGroup->GetGroupIndex();

	XTP_CALENDAR_THEME_DAYVIEWCELL_PARAMS tsCell;
	::ZeroMemory(&tsCell, sizeof(tsCell));

	tsCell.pViewGroup = pDayViewGroup;

	CXTPPropsStateContext autoStateCnt2Cell(GetCellPart(), xtpCalendarBeforeDraw_DayViewCell);

	// draw rows grid
	for (int i = 0; i < nVisibleRowCount; i++)
	{
		CRect rcCell = pDayViewGroup->GetCellRect(nTopRow + i);
		COleDateTime dtCell = pDayViewGroup->GetCellDateTime(nTopRow + i) + COleDateTimeSpan(0, 0, 0, 1);

		tsCell.nIndex = nTopRow + i;
		tsCell.dtBeginTime = dtCell;
		tsCell.bSelected = !bHasSelectedEvents && pDayView->SelectionContains(dtCell, nGroupIndex);
		tsCell.bWorkCell = pDayViewGroup->IsWorkDateTime(dtCell);


		autoStateCnt2Cell.SendBeforeDrawThemeObject((LPARAM)&tsCell, xtpCalendarBeforeDraw_DayViewCell);

		GetCellPart()->Draw(pDC, rcCell, tsCell);

		autoStateCnt2Cell.RestoreState();
	}

	autoStateCnt2Cell.Clear();

	CXTPPropsStateContext autoStateCnt3SE(GetSingleDayEventPart(), xtpCalendarBeforeDraw_DayViewEvent, TRUE);
	//1. draw no multiday, no selected events
	int nEvent = 0;
	int nEventsCount = pDayViewGroup->GetViewEventsCount();

	for (nEvent = 0; nEvent < nEventsCount; nEvent++)
	{
		CXTPCalendarDayViewEvent* pViewEvent = pDayViewGroup->GetViewEvent(nEvent);
		if (pViewEvent && !pViewEvent->IsMultidayEvent() && !pViewEvent->IsSelected())
		{
			autoStateCnt3SE.SendBeforeDrawThemeObject((LPARAM)pViewEvent);

			GetSingleDayEventPart()->Draw(pViewEvent, pDC);

			autoStateCnt3SE.RestoreState();
		}
	}

	//2. draw no multiday, selected events
	for (nEvent = 0; nEvent < nEventsCount; nEvent++)
	{
		CXTPCalendarDayViewEvent* pViewEvent = pDayViewGroup->GetViewEvent(nEvent);
		if (pViewEvent && !pViewEvent->IsMultidayEvent() && pViewEvent->IsSelected())
		{
			autoStateCnt3SE.SendBeforeDrawThemeObject((LPARAM)pViewEvent);

			GetSingleDayEventPart()->Draw(pViewEvent, pDC);

			autoStateCnt3SE.RestoreState();
		}
	}
	autoStateCnt3SE.Clear();

	// draw all day events area
	GetAllDayEventsPart()->Draw(pDayViewGroup, pDC);

	if (nGroupIndex)
	{
		CRect rcLeft = pDayViewGroup->GetRect();
		rcLeft.right = rcLeft.left + 1;
		pDC->FillSolidRect(&rcLeft, m_clrBorderLeft);
	}

	CXTPPropsStateContext autoStateCnt4MD(GetMultiDayEventPart(), xtpCalendarBeforeDraw_DayViewEvent, TRUE);
	// draw all day events
	for (nEvent = 0; nEvent < nEventsCount; nEvent++)
	{
		CXTPCalendarDayViewEvent* pViewEvent = pDayViewGroup->GetViewEvent(nEvent);
		if (pViewEvent && pViewEvent->IsMultidayEvent())
		{
			autoStateCnt4MD.SendBeforeDrawThemeObject((LPARAM)pViewEvent);

			GetMultiDayEventPart()->Draw(pViewEvent, pDC);

			autoStateCnt4MD.RestoreState();
		}
	}
	autoStateCnt4MD.Clear();

	// draw header part
	if (pDayViewGroup->IsGroupHeaderVisible())
	{
		GetHeaderPart()->Draw(pDayViewGroup, pDC);
	}
}

void CXTPCalendarTheme::CTODayViewDayGroupCell::Draw(CDC* pDC,
		const CRect& rcRect, const XTP_CALENDAR_THEME_DAYVIEWCELL_PARAMS& cellParams)
{
	if (!pDC || !cellParams.pViewGroup || !cellParams.pViewGroup->GetViewDay() ||
		!cellParams.pViewGroup->GetViewDay()->GetView())
	{
		ASSERT(FALSE);
		return;
	}
	COleDateTime dtCellNext = cellParams.dtBeginTime + cellParams.pViewGroup->GetViewDay()->GetView()->GetCellDuration();
	BOOL bInHour = dtCellNext.GetMinute() != 0;

	CRect rcRect1 = rcRect, rcRect2 = rcRect;
	rcRect1.bottom--;
	rcRect2.top = rcRect1.bottom;

	COLORREF clr1 = cellParams.bWorkCell ? m_clrsetWorkCell.clrBackground : m_clrsetNonWorkCell.clrBackground;
	if (cellParams.bSelected)
	{
		clr1 = cellParams.bWorkCell ? m_clrsetWorkCell.clrSelected : m_clrsetNonWorkCell.clrSelected;
	}

	COLORREF clr2 = cellParams.bWorkCell ? (bInHour ? m_clrsetWorkCell.clrBorderBottomInHour : m_clrsetWorkCell.clrBorderBottomHour) : 0;
	if (!cellParams.bWorkCell)
	{
		clr2 = bInHour ? m_clrsetNonWorkCell.clrBorderBottomInHour : m_clrsetNonWorkCell.clrBorderBottomHour;
	}

	pDC->FillSolidRect(&rcRect1, clr1);
	pDC->FillSolidRect(&rcRect2, clr2);
}

void CXTPCalendarTheme::CTOMonthView::AdjustLayout(CDC* pDC, const CRect& rcRect, BOOL bCallPostAdjustLayout)
{
	UNREFERENCED_PARAMETER(bCallPostAdjustLayout);

	CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView,
		XTP_SAFE_GET2(GetTheme(), GetCalendarControl(), GetMonthView(), NULL));

	if (!pDC || !pMonthView || !pMonthView->GetGrid())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarView::XTP_VIEW_LAYOUT& rLayout = GetTheme()->GetViewLayout(pMonthView );
	CXTPCalendarMonthView::CMonthViewGrid* pGrid = pMonthView->GetGrid();

	CRect rcRectGrid = rcRect;

	GetWeekDayHeaderPart()->AdjustLayout(pMonthView, pDC, rcRectGrid);
	rcRectGrid.top = min(rcRectGrid.top + pGrid->GetColHeaderHeight(), rcRectGrid.bottom);

	pGrid->AdjustGrid(rcRectGrid);

	//-----------------------------------
	int nHeight1 = GetDayPart()->GetSingleDayEventPart()->CalcMinEventHeight(pDC);
	int nHeight2 = GetDayPart()->GetMultiDayEventPart()->CalcMinEventHeight(pDC);

	rLayout.m_nRowHeight = max(nHeight1, nHeight2);

	//-------------------------------------------------------------------
	int nWeeksCount = pGrid->GetWeeksCount();
	for (int nWeek = 0; nWeek < nWeeksCount; nWeek++)
	{
		for (int nDayIdx = 0; nDayIdx < 7; nDayIdx++)
		{
			CRect rcDay = pGrid->GetDayRect(nWeek, nDayIdx, TRUE);

			CXTPCalendarMonthViewDay* pViewDay = pGrid->GetViewDay(nWeek, nDayIdx);
			//----------------------------------------
			if (pViewDay)
				pViewDay->AdjustLayout2(pDC, rcDay);
		}
	}

	//-------------------------------------------------------------------
	pGrid->AdjustEvents(pDC);
}

void CXTPCalendarTheme::CTOMonthView::Draw(CDC* pDC)
{
	CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView,
		XTP_SAFE_GET2(GetTheme(), GetCalendarControl(), GetMonthView(), NULL));

	if (!pDC || !pMonthView || !pMonthView->GetGrid())
	{
		ASSERT(FALSE);
		return;
	}

	pDC->SetBkMode(TRANSPARENT);

	GetWeekDayHeaderPart()->Draw(pMonthView, pDC);

	//-- Draw Days ------------------------------
	int nCount = pMonthView->GetViewDayCount();
	for (int i = nCount - 1; i >= 0; i--)
	{
		CXTPCalendarMonthViewDay* pViewDay = pMonthView->GetViewDay(i);
		ASSERT(pViewDay);
		if (pViewDay)
		{
			GetDayPart()->Draw(pViewDay, pDC);
		}
	}

	// All saved data must be freed after draw!
	//ASSERT(CXTPPropsState::s_dbg_nActiveStartes == 0);
}

void CXTPCalendarTheme::CTOMonthViewDay::AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect)
{
	CXTPCalendarMonthViewDay* pMVDay = DYNAMIC_DOWNCAST(CXTPCalendarMonthViewDay, pObject);
	if (!pDC || !pMVDay)
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& rDayLayout = GetViewDayLayout(pMVDay);

	int nDayHeaderHeight = GetHeaderPart()->CalcHeight(pDC, rcRect.Width());

	rDayLayout.m_rcDayHeader.CopyRect(rcRect);
	rDayLayout.m_rcDayHeader.bottom = min(rDayLayout.m_rcDayHeader.top + nDayHeaderHeight, rDayLayout.m_rcDayHeader.bottom);

	CRect rcDayEvents = rcRect; //GetDayEventsRect();
	rcDayEvents.top = rDayLayout.m_rcDayHeader.bottom;
	rcDayEvents.bottom -= 2;

	int nGroups = pMVDay->GetViewGroupsCount();
	ASSERT(nGroups == 1);

	CXTPCalendarMonthViewGroup* pMVGroup = nGroups ? pMVDay->GetViewGroup(0) : NULL;
	ASSERT(pMVGroup);

	if (pMVGroup)
	{
		pMVGroup->AdjustLayout2(pDC, rcDayEvents);
	}
}



void CXTPCalendarTheme::CTOWeekView::AdjustLayout(CDC* pDC, const CRect& rcRect, BOOL bCallPostAdjustLayout)
{
	UNREFERENCED_PARAMETER(bCallPostAdjustLayout);

	CXTPCalendarWeekView* pWeekView = DYNAMIC_DOWNCAST(CXTPCalendarWeekView,
		XTP_SAFE_GET2(GetTheme(), GetCalendarControl(), GetWeekView(), NULL));

	if (!pDC || !pWeekView)
	{
		ASSERT(FALSE);
		return;
	}

	//pWeekView->AdjustLayout(pDC, rcRect, bCallPostAdjustLayout);

	CXTPCalendarView::XTP_VIEW_LAYOUT& rLayout = GetTheme()->GetViewLayout(pWeekView);
	CXTPCalendarWeekView::XTP_WEEK_VIEW_LAYOUT& rLayoutX = GetTheme()->GetWeekViewLayout(pWeekView);

	CRect rcView2 = rcRect;
	//rcView2.DeflateRect(-1, -1, -1, -1);

	// Adjust Week's Grid

	int nRows = 6/rLayoutX.m_nGridColumns;
	ASSERT(6 % rLayoutX.m_nGridColumns == 0);

	rLayoutX.m_nDayWidth = rcView2.Width() / rLayoutX.m_nGridColumns;
	rLayoutX.m_nDayHeidht = rcView2.Height() / nRows;

	rLayoutX.m_nDayHeaderHeight = GetDayPart()->GetHeaderPart()->CalcHeight(pDC, rLayoutX.m_nDayWidth);

	int nHeight1 = GetDayPart()->GetSingleDayEventPart()->CalcMinEventHeight(pDC);
	int nHeight2 = GetDayPart()->GetMultiDayEventPart()->CalcMinEventHeight(pDC);

	rLayout.m_nRowHeight = max(nHeight1, nHeight2);

	//CalculateHeaderFormat(pDC, m_LayoutX.m_nDayWidth, pWVPart);

	int nDayIndex = 0;
	CRect rc(0, 0, 0, 0);

	for (int nDayCol = 0; nDayCol < rLayoutX.m_nGridColumns; nDayCol++)
	{
		int nRowTopY = rcView2.top;

		for (int nDayRow = 0; nDayRow < nRows; nDayRow++)
		{
			int nRowHeidht = (rcView2.bottom - nRowTopY) / (nRows - nDayRow);

			CXTPCalendarWeekViewDay* pDay = pWeekView->GetViewDay(nDayIndex);
			ASSERT(pDay);
			if (!pDay)
			{
				nRowTopY += nRowHeidht;
				continue;
			}

			COleDateTime dtDayDate = pWeekView->GetViewDayDate(nDayIndex);

			rc.SetRect( rcView2.left + rLayoutX.m_nDayWidth * nDayCol,
						nRowTopY,
						rcView2.left + rLayoutX.m_nDayWidth + rLayoutX.m_nDayWidth * nDayCol,
						nRowTopY + nRowHeidht);

			nRowTopY = rc.bottom;
			ASSERT((nDayRow + 1 != nRows) || (rc.bottom == rcView2.bottom));

			if (nDayCol + 1 == rLayoutX.m_nGridColumns)
				rc.right = rcView2.right;

			int nWday = dtDayDate.GetDayOfWeek();

			ASSERT(nWday != 1);

			if (nWday == 7)
			{
				CRect rc2 = rc;
				rc2.bottom = rc.top + rc.Height()/2;

				pDay->AdjustLayout2(pDC, rc2);

				nDayIndex++;
				pDay = pWeekView->GetViewDay(nDayIndex);
				rc.top = rc2.bottom;
			}

			ASSERT(pDay);
			if (pDay)
			{
				pDay->AdjustLayout2(pDC, rc);
			}

			nDayIndex++;
		}
	}

	//---------------------------------------------------------------------------
//  if (bCallPostAdjustLayout)
//  {
//      OnPostAdjustLayout();
//  }
}

void CXTPCalendarTheme::CTOWeekViewDay::AdjustLayout(CCmdTarget* pObject, CDC* pDC, const CRect& rcRect)
{
	CXTPCalendarWeekViewDay* pWVDay = DYNAMIC_DOWNCAST(CXTPCalendarWeekViewDay, pObject);
	if (!pDC || !pWVDay)
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& rDayLayout = GetViewDayLayout(pWVDay);

	int nDayHeaderHeight = GetHeaderPart()->CalcHeight(pDC, rcRect.Width());

	rDayLayout.m_rcDayHeader.CopyRect(rcRect);
	rDayLayout.m_rcDayHeader.bottom = min(rDayLayout.m_rcDayHeader.top + nDayHeaderHeight, rDayLayout.m_rcDayHeader.bottom);

	CRect rcDayEvents = rcRect;
	rcDayEvents.top = rDayLayout.m_rcDayHeader.bottom;
	rcDayEvents.bottom -= 2;

	int nGroups = pWVDay->GetViewGroupsCount();
	ASSERT(nGroups == 1);

	CXTPCalendarWeekViewGroup* pWVGroup = nGroups ? pWVDay->GetViewGroup(0) : NULL;
	ASSERT(pWVGroup);

	if (pWVGroup)
	{
		pWVGroup->AdjustLayout2(pDC, rcDayEvents);
	}
}

int CXTPCalendarTheme::CTODay::GetExpandButtonHeight()
{
	ASSERT(GetTheme());
	return XTP_SAFE_GET1(GetTheme(), GetExpandDayButtonSize().cx, 0);
}

int CXTPCalendarTheme::CTODay::_HitTestExpandDayButton(CXTPCalendarViewDay* pViewDay, const CRect& rc,
													   const CPoint* pPoint)
{
	if (!pViewDay || !GetTheme() || !GetTheme()->GetCalendarControl())
	{
		ASSERT(FALSE);
		return 0;
	}

	if (pViewDay->_NoAllEventsAreVisible())
	{
		CPoint pt(0, 0);
		if (pPoint)
		{
			pt = *pPoint;
		}
		else if (GetCursorPos(&pt))
		{
			GetTheme()->GetCalendarControl()->ScreenToClient(&pt);
		}

		if (GetTheme()->ExpandDayButton_HitTest(pt, rc, FALSE))
			return xtpCalendarHitTestDayExpandButton;
	}

	return 0;
}

int CXTPCalendarTheme::CTODay::HitTestExpandDayButton(CXTPCalendarViewDay* pViewDay, const CPoint* pPoint)
{
	if (!pViewDay || !GetTheme() || !GetTheme()->GetCalendarControl())
	{
		ASSERT(FALSE);
		return 0;
	}

	CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& dayLayout = CXTPCalendarTheme::GetViewDayLayout(pViewDay);
	return _HitTestExpandDayButton(pViewDay, dayLayout.m_rcDay, pPoint);
}

int CXTPCalendarTheme::CTODayViewDayGroup::HitTestScrollButton(CXTPCalendarDayViewGroup* pViewGroup, const CPoint* pPoint)
{
	if (!pViewGroup || !GetTheme() || !GetTheme()->GetCalendarControl())
	{
		ASSERT(FALSE);
		return 0;
	}

	CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT& groupLayout = GetTheme()->GetDayViewGroupLayout(pViewGroup);

	CPoint pt(0, 0);
	if (pPoint)
	{
		pt = *pPoint;
	}
	else
	{
		if (!GetCursorPos(&pt))
			return 0;
		GetTheme()->GetCalendarControl()->ScreenToClient(&pt);
	}

	if (pViewGroup->IsExpandUp())
	{
		int nButton = CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollUp;
		CRect rc = GetScrollButtonRect(groupLayout, nButton);

		if (GetTheme()->ExpandDayButton_HitTest(pt, rc, TRUE))
			return xtpCalendarHitTestDayViewScrollUp;
	}

	if (pViewGroup->IsExpandDown())
	{
		int nButton = CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT::xtpHotScrollDown;
		CRect rc = GetScrollButtonRect(groupLayout, nButton);

		if (GetTheme()->ExpandDayButton_HitTest(pt, rc, FALSE))
			return xtpCalendarHitTestDayViewScrollDown;
	}
	return 0;
}

int CXTPCalendarTheme::CTOWeekViewDay::GetDayCol(CXTPCalendarWeekViewDay* pWVDay)
{
	if (!pWVDay || !pWVDay->GetView())
	{
		ASSERT(FALSE);
		return 0;
	}

	CRect rcView = pWVDay->GetView()->GetViewRect();
	CRect rcDay = pWVDay->GetDayRect();

	int nViewMx = rcView.CenterPoint().x;
	int nDayMx = rcDay.CenterPoint().x;

	return nDayMx <= nViewMx ? 0 : 1;
}

void CXTPCalendarTheme::CTOWeekView::Draw(CDC* pDC)
{
	CXTPCalendarWeekView* pWeekView = DYNAMIC_DOWNCAST(CXTPCalendarWeekView,
		XTP_SAFE_GET2(GetTheme(), GetCalendarControl(), GetWeekView(), NULL));

	if (!pDC || !pWeekView)
	{
		ASSERT(FALSE);
		return;
	}

	pDC->SetBkMode(TRANSPARENT);

	CXTPPropsStateContext autoStateCnt1(GetDayPart(), xtpCalendarBeforeDraw_WeekViewDay, TRUE);
	//-- Draw Days ------------------------------
	int nCount = pWeekView->GetViewDayCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWeekViewDay* pViewDay = pWeekView->GetViewDay(i);
		ASSERT(pViewDay);
		if (pViewDay)
		{
			autoStateCnt1.SendBeforeDrawThemeObject((LPARAM)pViewDay);

			GetDayPart()->Draw(pViewDay, pDC);

			autoStateCnt1.RestoreState();
		}
	}

	autoStateCnt1.Clear();
	// All saved data must be freed after draw!
	ASSERT(CXTPPropsState::s_dbg_nActiveStartes == 0);
}

CSize CXTPCalendarTheme::GetExpandDayButtonSize()
{
	CXTPOffice2007Image* pImage = GetImageList()->GetBitmap((UINT)xtpIMG_ExpandDayButtons);
	if (!pImage)
		return CSize(0, 0);

	CRect rcBmp = pImage->GetSource(0, 4);

	return rcBmp.Size();
}

CRect CXTPCalendarTheme::DrawExpandDayButton(CDC* pDC, const CRect& rc, BOOL bUp, BOOL bHot)
{
	// TODO: add customization: (int)nBitmapID
	CXTPOffice2007Image* pImage = GetImageList()->GetBitmap((UINT)xtpIMG_ExpandDayButtons);

	if (!pImage)
	{
		static BOOL s_dbg_ImageAssert_was = FALSE;
		if (!s_dbg_ImageAssert_was)
		{
			s_dbg_ImageAssert_was = TRUE;
			ASSERT(pImage);
		}
		return CRect(0, 0, 0, 0);
	}

	int nIndex = (bUp ? 0 : 1) + (bHot ? 2 : 0);
	CRect rcBmp = pImage->GetSource(nIndex, 4);

	if (rc.Width() < rcBmp.Width() || rc.Height() < rcBmp.Height())
		return CRect(0, 0, 0, 0);

	CRect rcButton = rc;
	rcButton.left = rc.right - rcBmp.Width();
	if (bUp)
		rcButton.bottom = rc.top + rcBmp.Height();
	else
		rcButton.top = rc.bottom - rcBmp.Height();

	// Draw
	pImage->DrawImage(pDC, rcButton, rcBmp);


/*  // dbg. calc bitmap layers
//
	CXTPOffice2007Image* pBmpHeader = m_pImagesStd->LoadFile(_T("Office2007_Calendar_ExpandDay_xxx.bmp"));
	ASSERT(pBmpHeader);
	if (pBmpHeader)
	{

		CRect rcBmp = pBmpHeader->GetSource(0, 1);

		rcButton.right = rcButton.left + rcBmp.Width() * 2 + 2;
		rcButton.bottom = rcButton.top + rcBmp.Height() + 2;

		COLORREF clrBase = RGB(165, 191, 225);
		pDC->FillSolidRect(rcButton, clrBase);

		CXTPCompatibleDC dcSrc(pDC, pBmpHeader->GetBitmap());
		XTPImageManager()->AlphaBlend_calc(pDC->GetSafeHdc(), rcButton, dcSrc.GetSafeHdc(), rcBmp, clrBase);
	}*/

	return rcButton;
}

BOOL CXTPCalendarTheme::ExpandDayButton_HitTest(const CPoint& pt, const CRect& rc, BOOL bUp)
{
	// TODO: add customization: (int)nBitmapID
	CXTPOffice2007Image* pImage = GetImageList()->GetBitmap((UINT)xtpIMG_ExpandDayButtons);

	if (!pImage)
		return FALSE;

	int nIndex = (bUp ? 0 : 1);
	CRect rcBmp = pImage->GetSource(nIndex, 4);

	CRect rcButton = rc;
	rcButton.left = rc.right - rcBmp.Width();
	if (bUp)
		rcButton.bottom = rc.top + rcBmp.Height();
	else
		rcButton.top = rc.bottom - rcBmp.Height();

	if (!rcButton.PtInRect(pt))
		return FALSE;

	CPoint ptBmp = pt - rcButton.TopLeft();
	ptBmp.y += rcBmp.top;

	COLORREF clrPixel = 0;
	if (!XTPImageManager()->GetBitmapPixel(pImage->GetBitmap(), ptBmp, clrPixel))
		return FALSE;

	RGBQUAD* pclrPixel = (RGBQUAD*)&clrPixel;
	return pclrPixel->rgbReserved != 0;
}

/////////////////////////////////////////////////////////////////////////////
