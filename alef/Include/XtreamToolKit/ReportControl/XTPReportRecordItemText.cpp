// XTPReportRecordItemText.cpp : implementation of the CXTPReportRecordItemText class.
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
#include <math.h>
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPPropExchange.h"

#include "XTPReportRecordItem.h"
#include "XTPReportControl.h"
#include "XTPReportPaintManager.h"
#include "XTPReportRecordItemText.h"
#include "XTPReportInplaceControls.h"
#include "XTPReportRow.h"
#include "XTPReportRecord.h"
#include "XTPReportRecords.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPReportRecordItemText

IMPLEMENT_SERIAL(CXTPReportRecordItemText, CXTPReportRecordItem, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

CXTPReportRecordItemText::CXTPReportRecordItemText(LPCTSTR szText)
	: CXTPReportRecordItem(), m_strText(szText)
{
}


CString CXTPReportRecordItemText::GetCaption(CXTPReportColumn* /*pColumn*/)
{
	if (!m_strCaption.IsEmpty())
		return m_strCaption;

	if (m_strFormatString == _T("%s"))
		return m_strText;

	CString strCaption;
	strCaption.Format(m_strFormatString, (LPCTSTR)m_strText);
	return strCaption;
}

void CXTPReportRecordItemText::OnEditChanged(XTP_REPORTRECORDITEM_ARGS* /*pItemArgs*/, LPCTSTR szText)
{
	SetValue(szText);
}

void CXTPReportRecordItemText::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPReportRecordItem::DoPropExchange(pPX);

	PX_String(pPX, _T("Text"), m_strText, _T(""));
}

//////////////////////////////////////////////////////////////////////////
// CXTPReportRecordItemNumber

IMPLEMENT_SERIAL(CXTPReportRecordItemNumber, CXTPReportRecordItem, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

CXTPReportRecordItemNumber::CXTPReportRecordItemNumber(double dValue)
	: CXTPReportRecordItem(), m_dValue(dValue)
{
	static const CString cstrNumberFormatDefault(_T("%0.f")); // to avoid new string data allocation for each record
	m_strFormatString = cstrNumberFormatDefault;
}

CXTPReportRecordItemNumber::CXTPReportRecordItemNumber(double dValue, LPCTSTR strFormat)
	: CXTPReportRecordItem(), m_dValue(dValue)
{
	m_strFormatString = strFormat;
}

CString CXTPReportRecordItemNumber::GetCaption(CXTPReportColumn* /*pColumn*/)
{
	if (!m_strCaption.IsEmpty())
		return m_strCaption;

	CString strCaption;
	strCaption.Format(m_strFormatString, m_dValue);
	return strCaption;
}

int CXTPReportRecordItemNumber::Compare(CXTPReportColumn*, CXTPReportRecordItem* pItem)
{
	CXTPReportRecordItemNumber* pItemNumber = DYNAMIC_DOWNCAST(CXTPReportRecordItemNumber, pItem);
	if (!pItemNumber)
		return 0;

	if (m_dValue == pItemNumber->m_dValue)
		return 0;
	else if (m_dValue > pItemNumber->m_dValue)
		return 1;
	else
		return -1;
}

void CXTPReportRecordItemNumber::OnEditChanged(XTP_REPORTRECORDITEM_ARGS* /*pItemArgs*/, LPCTSTR szText)
{
#ifdef _UNICODE
	char astring[20];
	WideCharToMultiByte (CP_ACP, 0, szText, -1, astring, 20, NULL, NULL);
	SetValue((double)atof(astring));
#else
	SetValue((double)atof(szText));
#endif
}

void CXTPReportRecordItemNumber::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPReportRecordItem::DoPropExchange(pPX);

	PX_Double(pPX, _T("Value"), m_dValue);
}


//////////////////////////////////////////////////////////////////////////
// CXTPReportRecordItemDateTime

IMPLEMENT_SERIAL(CXTPReportRecordItemDateTime, CXTPReportRecordItem, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

CXTPReportRecordItemDateTime::CXTPReportRecordItemDateTime(COleDateTime odtValue)
	: CXTPReportRecordItem(), m_odtValue(odtValue)
{
	static const CString cstrDateFormatDefault(_T("%a %b/%d/%Y %I:%M %p")); // to avoid new string data allocation for each record
	m_strFormatString = cstrDateFormatDefault;
}

CString CXTPReportRecordItemDateTime::GetCaption(CXTPReportColumn* /*pColumn*/)
{
	if (!m_strCaption.IsEmpty())
		return m_strCaption;

	//return m_odtValue.Format(m_strFormatString);
	return CXTPReportControlLocale::FormatDateTime(m_odtValue, m_strFormatString);
}

int CXTPReportRecordItemDateTime::Compare(CXTPReportColumn*, CXTPReportRecordItem* pItem)
{
	CXTPReportRecordItemDateTime* pItemDateTime = DYNAMIC_DOWNCAST(CXTPReportRecordItemDateTime, pItem);
	if (!pItemDateTime)
		return 0;

	if (m_odtValue == pItemDateTime->m_odtValue)
		return 0;

	if (m_odtValue.GetStatus() != COleDateTime::valid || pItemDateTime->m_odtValue.GetStatus() != COleDateTime::valid)
		return int(m_odtValue.m_dt - pItemDateTime->m_odtValue.m_dt);

	if (m_odtValue > pItemDateTime->m_odtValue)
		return 1;

	return -1;
}

void CXTPReportRecordItemDateTime::OnEditChanged(XTP_REPORTRECORDITEM_ARGS* /*pItemArgs*/, LPCTSTR szText)
{
	m_odtValue.ParseDateTime(szText);
}

void CXTPReportRecordItemDateTime::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPReportRecordItem::DoPropExchange(pPX);

	PX_DateTime(pPX, _T("Value"), m_odtValue);
}

//////////////////////////////////////////////////////////////////////////
// CXTPReportRecordItemPreview
IMPLEMENT_SERIAL(CXTPReportRecordItemPreview, CXTPReportRecordItem, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

CXTPReportRecordItemPreview::CXTPReportRecordItemPreview(LPCTSTR szPreviewText)
	: CXTPReportRecordItem(), m_strPreviewText(szPreviewText)
{
}

CString CXTPReportRecordItemPreview::GetCaption(CXTPReportColumn* /*pColumn*/)
{
	return GetPreviewText();
}

void CXTPReportRecordItemPreview::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	pItemMetrics->clrForeground = pDrawArgs->pControl->GetPaintManager()->m_clrPreviewText;
	pItemMetrics->pFont = &pDrawArgs->pControl->GetPaintManager()->m_fontPreview;
}

void CXTPReportRecordItemPreview::OnEditChanged(XTP_REPORTRECORDITEM_ARGS* /*pItemArgs*/, LPCTSTR szText)
{
	m_strPreviewText = szText;
}

void CXTPReportRecordItemPreview::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPReportRecordItem::DoPropExchange(pPX);

	PX_String(pPX, _T("PreviewText"), m_strPreviewText);
}

int CXTPReportRecordItemPreview::GetPreviewHeight(CDC* pDC, CXTPReportRow* pRow, int nWidth)
{
	XTP_REPORTRECORDITEM_METRICS* pMetrics = new XTP_REPORTRECORDITEM_METRICS;
	pMetrics->strText = GetPreviewText();
	pRow->FillMetrics(NULL, this, pMetrics);

	CString strPreviewText = pMetrics->strText;

	if (strPreviewText.IsEmpty())
	{
		pMetrics->InternalRelease();
		return 0;
	}

	CXTPReportControl* pControl = pRow->GetControl();

	int nIndentWidth = pControl->GetHeaderIndent();


	CRect& rcIndent = pControl->GetPaintManager()->m_rcPreviewIndent;

	CRect rcPreviewItem(nIndentWidth + rcIndent.left, 0, nWidth - rcIndent.right, 0);

	CXTPFontDC font(pDC, pMetrics->pFont);

	int nMaxPreviewLines = pControl->GetPaintManager()->GetPreviewLinesCount(pDC, rcPreviewItem, strPreviewText);
	int nFontHeight = pDC->GetTextExtent(_T(" "), 1).cy;

	pMetrics->InternalRelease();

	return nFontHeight * nMaxPreviewLines + rcIndent.top + rcIndent.bottom;
}

void CXTPReportRecordItemPreview::GetCaptionRect(XTP_REPORTRECORDITEM_ARGS* pDrawArgs, CRect& rcItem)
{
	ASSERT(pDrawArgs->pControl);

	CRect& rcIndent = pDrawArgs->pControl->GetPaintManager()->m_rcPreviewIndent;
	rcItem.DeflateRect(rcIndent.left - 2, -1, rcIndent.right, -rcIndent.bottom);
}


void CXTPReportRecordItemPreview::OnDrawCaption(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pMetrics)
{
	ASSERT(pDrawArgs->pControl);

	if(!pDrawArgs->pControl->GetPaintManager()->OnDrawAction(pDrawArgs))
		return;

	CString strText = pMetrics->strText;

	// draw item text
	if (!strText.IsEmpty())
	{
		CRect rcItem(pDrawArgs->rcItem);

		CRect& rcIndent = pDrawArgs->pControl->GetPaintManager()->m_rcPreviewIndent;

		rcItem.DeflateRect(rcIndent.left, rcIndent.top, rcIndent.right, rcIndent.bottom);

		pDrawArgs->pDC->DrawText(strText, rcItem, DT_WORDBREAK | DT_LEFT | DT_NOPREFIX);
	}
}




#if _MFC_VER >= 0x0600 // Not supported by Visual Studio 5.0

int CXTPReportRecordItemVariant::m_nSortLocale = LOCALE_USER_DEFAULT;


//////////////////////////////////////////////////////////////////////////
// CXTPReportRecordItemVariant

IMPLEMENT_SERIAL(CXTPReportRecordItemVariant, CXTPReportRecordItem, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

CXTPReportRecordItemVariant::CXTPReportRecordItemVariant(const VARIANT& lpValue)
{

	m_oleValue = lpValue;
	m_oleValue.ChangeType((VARTYPE)(m_oleValue.vt & ~VT_BYREF));
}

void CXTPReportRecordItemVariant::SetValue(COleVariant& var)
{
	m_oleValue = var;
}

CString CXTPReportRecordItemVariant::GetCaption(CXTPReportColumn* pColumn)
{
	if (!m_strCaption.IsEmpty())
		return m_strCaption;

	COleVariant var(m_oleValue);

	CXTPReportRecordItemEditOptions* pEditOptions = GetEditOptions(pColumn);
	BOOL bConstraintEdit = pEditOptions->m_bConstraintEdit;

	TRY
	{
		if (var.vt == VT_DATE && !bConstraintEdit && m_strFormatString != _T("%s"))
		{
			COleDateTime dt(var);
			//return dt.Format(m_strFormatString);
			return CXTPReportControlLocale::FormatDateTime(dt, m_strFormatString);
		}
		if (var.vt == VT_NULL)
		{
			var.vt = VT_I4;
			var.lVal = 0;
		}
		else
		{
			//var.ChangeType((VARTYPE) (bConstraintEdit ? VT_I4 : VT_BSTR));
			CXTPReportControlLocale::VariantChangeTypeEx(var, (VARTYPE) (bConstraintEdit ? VT_I4 : VT_BSTR));
		}
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL

	if (bConstraintEdit)
	{
		CXTPReportRecordItemConstraint* pConstraint = pEditOptions->FindConstraint(var.lVal);
		return pConstraint ? pConstraint->m_strConstraint : _T("");
	}

	if (var.vt != VT_BSTR)
		return _T("");

	CString strVariant(var.bstrVal);
	if (m_strFormatString == _T("%s"))
		return strVariant;

	CString strCaption;
	strCaption.Format(m_strFormatString, (LPCTSTR)strVariant);
	return strCaption;
}

int CXTPReportRecordItemVariant::Compare(CXTPReportColumn*, CXTPReportRecordItem* pItem)
{
	if (GetSortPriority() != -1 || pItem->GetSortPriority() != -1)
		return GetSortPriority() - pItem->GetSortPriority();

	CXTPReportRecordItemVariant* pItemVariant = DYNAMIC_DOWNCAST(CXTPReportRecordItemVariant, pItem);
	if (!pItemVariant)
		return 0;

	ULONG dwFlags = m_pRecord->GetRecords()->IsCaseSensitive() ? 0 : NORM_IGNORECASE;

	LCID lcidnSortLocale = m_nSortLocale;
	if (lcidnSortLocale == LOCALE_USER_DEFAULT)
	{
		lcidnSortLocale = CXTPReportControlLocale::GetActiveLCID();
	}
	return VarCmp(m_oleValue, pItemVariant->m_oleValue, lcidnSortLocale, dwFlags) - VARCMP_EQ;
}

BOOL CXTPReportRecordItemVariant::OnValueChanging(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPVARIANT lpNewValue)
{
	UNREFERENCED_PARAMETER(pItemArgs);
	UNREFERENCED_PARAMETER(lpNewValue);
	return TRUE;

}

void CXTPReportRecordItemVariant::OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs,
												LPCTSTR szText)
{
	COleVariant var(szText);

	if (OnValueChanging(pItemArgs, &var))
	{
		TRY
		{
			CXTPReportControlLocale::VariantChangeTypeEx(var, m_oleValue.vt);
		}
		CATCH_ALL(e)
		{
			return;
		}
		END_CATCH_ALL

		m_oleValue = var;
	}
}


void CXTPReportRecordItemVariant::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPReportRecordItem::DoPropExchange(pPX);

	COleVariant varDefault(_T(""));
	PX_Variant(pPX, _T("Value"), m_oleValue, varDefault);
}

void CXTPReportRecordItemVariant::OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraint* pConstraint)
{
	if (GetEditOptions(pItemArgs->pColumn)->m_bConstraintEdit)
	{
		COleVariant var((long)pConstraint->m_dwData);
		//var.ChangeType(m_oleValue.vt);
		BOOL bChanged = CXTPReportControlLocale::VariantChangeTypeEx(var, m_oleValue.vt, FALSE);

		if (bChanged && OnValueChanging(pItemArgs, &var))
		{
			m_oleValue = var;
		}
	}
	else
	{
		OnEditChanged(pItemArgs, pConstraint->m_strConstraint);
	}
}

DWORD CXTPReportRecordItemVariant::GetSelectedConstraintData(XTP_REPORTRECORDITEM_ARGS* pItemArgs)
{
	if (GetEditOptions(pItemArgs->pColumn)->m_bConstraintEdit)
	{
		COleVariant var(m_oleValue);
		TRY
		{
			//var.ChangeType(VT_I4);
			CXTPReportControlLocale::VariantChangeTypeEx(var, VT_I4);
		}
		CATCH_ALL(e)
		{
			return (DWORD)-1;
		}
		END_CATCH_ALL

		return var.lVal;

	}
	else
	{
		return (DWORD)-1;
	}
}

#endif


