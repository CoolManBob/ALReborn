// XTPCalendarCustomProperties.cpp: implementation of the
// CXTPCalendarCustomProperties class.
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

#include "Common/XTPPropExchange.h"
#include "Common/XTPVC50Helpers.h"

#include "XTPCalendarCustomProperties.h"

//#ifndef _tstof
//  #ifdef _UNICODE
//      #include <stdlib.h>
//      #define _tstof _wtof
//  #else
//      #define _tstof atof
//  #endif
//#endif

#define XTP_CUSTOM_PROP_DATA_VER 1

// 37, 53, 79 , 101, 127, 199, 503
#define XTP_CUSTOM_PROP_HASH_TABLE_SIZE 127

IMPLEMENT_DYNAMIC(CXTPCalendarCustomProperties, CCmdTarget)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPCalendarCustomProperties::CXTPCalendarCustomProperties(BOOL bNameIgnoreCase)
{
	m_bNameIgnoreCase = bNameIgnoreCase;
	m_mapProperties.InitHashTable(XTP_CUSTOM_PROP_HASH_TABLE_SIZE, FALSE);

}

CXTPCalendarCustomProperties::~CXTPCalendarCustomProperties()
{
}

CString CXTPCalendarCustomProperties::PrepareName(LPCTSTR pcszName) const
{
	CString strName = pcszName;
	if (m_bNameIgnoreCase)
	{
		strName.MakeLower();
	}
	return strName;
}

BOOL CXTPCalendarCustomProperties::GetProperty(LPCTSTR pcszName, COleVariant& rVarValue) const
{
	return m_mapProperties.Lookup(PrepareName(pcszName), rVarValue);
}

BOOL CXTPCalendarCustomProperties::SetProperty(LPCTSTR pcszName, const COleVariant& varValue)
{
	COleVariant varVal = varValue;
	m_mapProperties.SetAt(PrepareName(pcszName), varVal);
	return TRUE;
}

BOOL CXTPCalendarCustomProperties::RemoveProperty(LPCTSTR pcszName)
{
	return m_mapProperties.RemoveKey(PrepareName(pcszName));
}

void CXTPCalendarCustomProperties::RemoveAll()
{
	m_mapProperties.RemoveAll();
}

int CXTPCalendarCustomProperties::GetCount()
{
	return (int)m_mapProperties.GetCount();
}

POSITION CXTPCalendarCustomProperties::GetStartPosition() const
{
	return m_mapProperties.GetStartPosition();
}

void CXTPCalendarCustomProperties::GetNextProperty(POSITION& rPos, CString& rStrName, COleVariant& rVarValue) const
{
	m_mapProperties.GetNextAssoc(rPos, rStrName, rVarValue);
}

void CXTPCalendarCustomProperties::DoPropExchange(CXTPPropExchange* pPX)
{
	if (!pPX)
	{
		ASSERT(FALSE);
		return;
	}

	if (pPX->IsLoading())
	{
		_Load(pPX);
	}
	else
	{
		_Save(pPX);
	}
}

void CXTPCalendarCustomProperties::_Save(CXTPPropExchange* pPX)
{
	if (!pPX || !pPX->IsStoring())
	{
		ASSERT(FALSE);
		return;
	}

	CXTPPropExchangeSection secProps(pPX->GetSection(_T("CustomProperties")));
	secProps->EmptySection();

	long nVersion = XTP_CUSTOM_PROP_DATA_VER;
	PX_Long(&secProps, _T("Version"), nVersion, XTP_CUSTOM_PROP_DATA_VER);

	int nPropsCount = GetCount();
	CXTPPropExchangeEnumeratorPtr pEnumerator(secProps->GetEnumerator(_T("CustomProperty")));
	POSITION posStorage = pEnumerator->GetPosition(nPropsCount);

	int nPropSaved = 0;
	POSITION pos = m_mapProperties.GetStartPosition();
	while (pos)
	{
		CString strName;
		COleVariant varValue;
		m_mapProperties.GetNextAssoc(pos, strName, varValue);

		CXTPPropExchangeSection secProp(pEnumerator->GetNext(posStorage));
		PX_String(&secProp, _T("Name"), strName);
		PX_Variant(&secProp, _T("Value"), varValue, COleVariant((long)0));
		long lVt = varValue.vt;
		PX_Long(&secProp, _T("VariantType"), lVt);

		nPropSaved++;
	}
	ASSERT(nPropSaved == nPropsCount);
}

void CXTPCalendarCustomProperties::_Load(CXTPPropExchange* pPX)
{
	if (!pPX || !pPX->IsLoading())
	{
		ASSERT(FALSE);
		return;
	}
	m_mapProperties.RemoveAll();

	CXTPPropExchangeSection secProps(pPX->GetSection(_T("CustomProperties")));

	long nVersion;
	PX_Long(&secProps, _T("Version"), nVersion, XTP_CUSTOM_PROP_DATA_VER);

	if (nVersion != XTP_CUSTOM_PROP_DATA_VER)
	{
		TRACE(_T("ERROR! XTPCalendarCustomProperties: Unsupported data file version. (%d) \n"), nVersion);
		return;
	}

	CXTPPropExchangeEnumeratorPtr pEnumerator(secProps->GetEnumerator(_T("CustomProperty")));
	POSITION posStorage = pEnumerator->GetPosition();

	while (posStorage)
	{
		CString strName;
		COleVariant varValue;

		CXTPPropExchangeSection secProp(pEnumerator->GetNext(posStorage));
		PX_String(&secProp, _T("Name"), strName);
		PX_Variant(&secProp, _T("Value"), varValue, COleVariant((long)0));

		long lVt;
		PX_Long(&secProp, _T("VariantType"), lVt, VT_EMPTY);
		if (lVt != VT_EMPTY && varValue.vt != lVt)
		{
			LCID lcidID = pPX->m_lcidDateTime;
			if (lcidID == XTP_LOCALE_ISO8601)
			{
				lcidID = LOCALE_NEUTRAL;
			}
			COleVariant varValue2;

			HRESULT hr = VariantChangeTypeEx(&varValue2, &varValue,
										lcidID, 0, (VARTYPE)lVt);
			if (FAILED(hr))
			{
				hr = VariantChangeType(&varValue2, &varValue, 0, (VARTYPE)lVt);
			}
			if (FAILED(hr))
			{
				hr = VariantChangeType(&varValue2, &varValue, VARIANT_NOUSEROVERRIDE, (VARTYPE)lVt);
			}

			if (SUCCEEDED(hr))
			{
				varValue = varValue2;
			}
			else if (varValue.vt == VT_BSTR && (lVt == VT_R4 || lVt == VT_R8 || lVt == VT_DATE))
			{
				USES_CONVERSION;

				CString strValue = varValue.bstrVal;
				REPLACE_S(strValue, _T(','), _T('.'));
				double dblVal = atof(T2CA((LPTSTR)(LPCTSTR)strValue));

				if (lVt == VT_R4)
				{
					varValue = (float)dblVal;
				}
				else if (lVt == VT_R8)
				{
					varValue = (double)dblVal;
				}
				else if (lVt == VT_DATE)
				{
					varValue = COleDateTime((DATE)dblVal);
				}
			}
			else
			{
				ASSERT(FALSE);
			}
		}

		SetProperty(strName, varValue);
	}
}

////////////////////////////////////////////////////////////////////////////
void CXTPCalendarCustomProperties::CopyTo(CXTPCalendarCustomProperties* pDest)
{
	if (!pDest)
	{
		ASSERT(FALSE);
		return;
	}

	pDest->RemoveAll();

	POSITION pos = GetStartPosition();
	while (pos)
	{

		CString strName;
		COleVariant varValue;
		GetNextProperty(pos, strName, varValue);

		VERIFY(pDest->SetProperty(strName, varValue));
	}
}

BOOL CXTPCalendarCustomProperties::LoadFromXML(LPCTSTR pcszXMLData)
{
	if (!pcszXMLData)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	RemoveAll();

	if (*pcszXMLData == _T('\0'))
	{
		return TRUE;
	}

#ifndef _XTP_EXCLUDE_XML
	CXTPPropExchangeXMLNode propPX(TRUE, 0, _T("Calendar"));
	if (!propPX.LoadFromString(pcszXMLData))
		return FALSE;

	propPX.ExchangeLocale();

	DoPropExchange(&propPX);

	return TRUE;
#else
	return FALSE;
#endif
}

BOOL CXTPCalendarCustomProperties::SaveToXML(CString& rstrXMLData)
{

#ifndef _XTP_EXCLUDE_XML
	CXTPPropExchangeXMLNode propPX(FALSE, 0, _T("Calendar"));
	propPX.ExchangeLocale();

	DoPropExchange(&propPX);

	rstrXMLData = propPX.GetXML();
	return TRUE;
#else
	return FALSE;
#endif
}

/////////////////////////////////////////////////////////////////////////////
