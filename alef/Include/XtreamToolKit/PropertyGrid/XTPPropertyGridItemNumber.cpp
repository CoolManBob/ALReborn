// XTPPropertyGridItemNumber.cpp : implementation of the CXTPPropertyGridItemNumber class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
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

#include "StdAfx.h"
#include "Common/XTPVC80Helpers.h"

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGridItemNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemNumber
IMPLEMENT_DYNAMIC(CXTPPropertyGridItemNumber, CXTPPropertyGridItem)


CXTPPropertyGridItemNumber::CXTPPropertyGridItemNumber(LPCTSTR strCaption, long nValue, long* pBindNumber)
	: CXTPPropertyGridItem(strCaption)
{
	m_pBindNumber = pBindNumber;
	SetNumber(nValue);
	m_strDefaultValue = m_strValue;
}
CXTPPropertyGridItemNumber::CXTPPropertyGridItemNumber(UINT nID, long nValue, long* pBindNumber)
	: CXTPPropertyGridItem(nID)
{
	m_pBindNumber = pBindNumber;
	SetNumber(nValue);
	m_strDefaultValue = m_strValue;
}

CXTPPropertyGridItemNumber::~CXTPPropertyGridItemNumber()
{
}

/////////////////////////////////////////////////////////////////////////////
//

void CXTPPropertyGridItemNumber::SetValue(CString strValue)
{
	SetNumber(_ttol(strValue));
}

void CXTPPropertyGridItemNumber::SetNumber(long nValue)
{
	m_nValue = nValue;

	if (m_pBindNumber)
	{
		*m_pBindNumber = m_nValue;
	}

	CString strValue;
	strValue.Format(_T("%i"), nValue);
	CXTPPropertyGridItem::SetValue(strValue);
}

void CXTPPropertyGridItemNumber::BindToNumber(long* pBindNumber)
{
	m_pBindNumber = pBindNumber;
	if (m_pBindNumber)
	{
		*m_pBindNumber = m_nValue;
	}
}

void CXTPPropertyGridItemNumber::OnBeforeInsert()
{
	if (m_pBindNumber && *m_pBindNumber != m_nValue)
	{
		SetNumber(*m_pBindNumber);
	}
}





/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemDouble
IMPLEMENT_DYNAMIC(CXTPPropertyGridItemDouble, CXTPPropertyGridItem)


CXTPPropertyGridItemDouble::CXTPPropertyGridItemDouble(LPCTSTR strCaption, double fValue, LPCTSTR strFormat, double* pBindDouble)
	: CXTPPropertyGridItem(strCaption)
{
	m_pBindDouble = pBindDouble;
	m_strFormat = strFormat;
	m_bUseSystemDecimalSymbol = m_strFormat.IsEmpty();

	SetDouble(fValue);
	m_strDefaultValue = m_strValue;

	EnableAutomation();
}

CXTPPropertyGridItemDouble::CXTPPropertyGridItemDouble(UINT nID, double fValue, LPCTSTR strFormat, double* pBindDouble)
	: CXTPPropertyGridItem(nID)
{
	m_pBindDouble = pBindDouble;
	m_strFormat = strFormat;
	m_bUseSystemDecimalSymbol = m_strFormat.IsEmpty();

	SetDouble(fValue);
	m_strDefaultValue = m_strValue;

	EnableAutomation();
}

CXTPPropertyGridItemDouble::~CXTPPropertyGridItemDouble()
{
}

/////////////////////////////////////////////////////////////////////////////
//

void CXTPPropertyGridItemDouble::SetValue(CString strValue)
{
	SetDouble(StringToDouble(strValue));
}

double CXTPPropertyGridItemDouble::StringToDouble(LPCTSTR strValue)
{
	if (m_bUseSystemDecimalSymbol)
	{
		TRY
		{
			COleVariant oleString(strValue);
			oleString.ChangeType(VT_R8);
			return oleString.dblVal;
		}
		CATCH(COleException, e)
		{
		}
		END_CATCH
	}

#ifdef _UNICODE
	char astring[20];
	WideCharToMultiByte (CP_ACP, 0, strValue, -1, astring, 20, NULL, NULL);
	return (double)atof(astring);
#else
	return (double)atof(strValue);
#endif
}

CString CXTPPropertyGridItemDouble::DoubleToString(double dValue)
{
	if (m_bUseSystemDecimalSymbol)
	{
		TRY
		{
			COleVariant oleString(dValue);
			oleString.ChangeType(VT_BSTR);
			return CString(oleString.bstrVal);
		}
		CATCH(COleException, e)
		{
		}
		END_CATCH
	}

	CString strFormat(m_strFormat);
	if (strFormat.IsEmpty())
		strFormat = _T("%0.2f");

	CString strValue;
	strValue.Format(m_strFormat, dValue);
	return strValue;
}

void CXTPPropertyGridItemDouble::SetDouble(double fValue)
{
	m_fValue = fValue;

	if (m_pBindDouble)
	{
		*m_pBindDouble = m_fValue;
	}

	CXTPPropertyGridItem::SetValue(DoubleToString(fValue));
}

void CXTPPropertyGridItemDouble::BindToDouble(double* pBindNumber)
{
	m_pBindDouble = pBindNumber;
	if (m_pBindDouble)
	{
		*m_pBindDouble = m_fValue;
	}
}

void CXTPPropertyGridItemDouble::OnBeforeInsert()
{
	if (m_pBindDouble && *m_pBindDouble != m_fValue)
	{
		SetDouble(*m_pBindDouble);
	}
}

