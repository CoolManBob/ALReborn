// XTPCalendarDatabaseDataProvider.cpp: implementation of the CXTPCalendarDatabaseDataProvider class.
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
#include "Common/XTPVC80Helpers.h"

#include "XTPCalendarDatabaseDataProvider.h"
#include "XTPCalendarUtils.h"
#include "XTPCalendarNotifications.h"
#include "XTPCalendarControl.h"
#include "XTPCalendarCustomProperties.h"
#include "XTPCalendarResource.h"

#ifndef CT2CW
#define CT2CW(x) T2CW((LPTSTR)(LPCTSTR)x)
#endif

#pragma warning(disable: 4571) // warning C4571: catch(...) blocks compiled with /EHs do not catch or re-throw Structured Exceptions


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE CString MakeSQLDateTime(DATE t)
{
	COleDateTime ct(t);
	return ct.Format(_T("#%Y-%m-%d %H:%M:%S#"));
}

AFX_INLINE const _variant_t DATE2VAR(DATE dt)
{
	return _variant_t(dt, VT_DATE);
}

AFX_INLINE const _variant_t STRING2VAR(const CString& str)
{
	VARIANT vtStr;
	vtStr.vt = VT_BSTR;
	vtStr.bstrVal = str.AllocSysString();

	return _variant_t(vtStr, false); // Will deallocate in destructor
}

template<class _TData>
_TData _GetField(XTPADODB::_Recordset* pRS, LPCWSTR pcszFName, _TData defVal)
{
	_TData dataValue;
	try
	{

		XTPADODB::FieldPtr ptrField = pRS->Fields->GetItem(pcszFName);
		if (ptrField)
		{
			_variant_t vtValue = ptrField->Value;

			if (vtValue.vt == VT_NULL || vtValue.vt == VT_EMPTY ||
				vtValue.vt == VT_ERROR)
			{
				return defVal;
			}

			dataValue = (_TData)vtValue;
			return dataValue;
		}
	}
	catch (_com_error &e)
	{
		e;
#ifdef _DEBUG
		if (pRS->GetActiveConnection().vt == VT_DISPATCH &&
			pRS->GetActiveConnection().pdispVal)
		{
			XTPADODB::_Connection* pConnDB = NULL;
			HRESULT hr = pRS->GetActiveConnection().pdispVal->QueryInterface(__uuidof(XTPADODB::_Connection), (void**)&pConnDB);
			if (SUCCEEDED(hr))
			{
				CXTPCalendarDatabaseDataProvider::TRACE_ProviderError(pConnDB);
			}
		}
		CXTPCalendarDatabaseDataProvider::TRACE_ComError(e);
#endif
	}
	catch(...)
	{}

	TRACE(_T("ERROR: CalendarDB provider: Cannot Get field '%s' \n"), pcszFName);
	return defVal;
}

inline CString VariantToString(VARIANT vtValue, LPCTSTR lpszDefault)
{
	if (vtValue.vt != VT_BSTR)
	{
		if (FAILED(::VariantChangeType(&vtValue, &vtValue, 0, VT_BSTR)))
		{
			return lpszDefault;
		}
	}

	return CString(V_BSTR(&vtValue));
}

CString _GetFieldString(XTPADODB::_Recordset* pRS, LPCWSTR pcszFName, LPCTSTR defVal = NULL)
{
	try
	{
		XTPADODB::FieldPtr ptrField = pRS->Fields->GetItem(pcszFName);
		if (ptrField)
		{
			_variant_t vtValue = ptrField->Value;

			if (vtValue.vt == VT_NULL || vtValue.vt == VT_EMPTY ||
				vtValue.vt == VT_ERROR)
			{
				return defVal;
			}
			return VariantToString(vtValue, defVal);
		}
	}
	catch (_com_error &e)
	{
		e;
#ifdef _DEBUG
		if (pRS->GetActiveConnection().vt == VT_DISPATCH &&
			pRS->GetActiveConnection().pdispVal)
		{
			XTPADODB::_Connection* pConnDB = NULL;
			HRESULT hr = pRS->GetActiveConnection().pdispVal->QueryInterface(__uuidof(XTPADODB::_Connection), (void**)&pConnDB);
			if (SUCCEEDED(hr))
			{
				CXTPCalendarDatabaseDataProvider::TRACE_ProviderError(pConnDB);
			}
		}
		CXTPCalendarDatabaseDataProvider::TRACE_ComError(e);
#endif
	}
	catch(...)
	{}

	TRACE(_T("ERROR: CalendarDB provider: Cannot Get field '%s' \n"), pcszFName);
	return defVal;
}




class CXTPCalendarDatabaseDataProvider::CADOCommand : public CCmdTarget
{
public:

	CADOCommand(XTPADODB::_ConnectionPtr pConnection, LPCWSTR pcszSQLText);
	~CADOCommand();

	void CreateParameter(LPCWSTR pcszParamName, XTPADODB::DataTypeEnum adDataType,
		XTPADODB::ParameterDirectionEnum adDirection, long nSize,
		const _variant_t& vtValue);

	void SetParameter(XTPADODB::DataTypeEnum adDataType, long nSize,
		const _variant_t& vtValue);

	void SetSQLString(LPCWSTR pcszSQLText);

	void SetParameterValue(LPCWSTR pcszParamName, const _variant_t& vtValue);

	XTPADODB::_RecordsetPtr Execute();

	void ClearParams();

	void TraceComError(_com_error &e);

private:
	XTPADODB::_CommandPtr     m_pcmdSQL;
	XTPADODB::_ConnectionPtr  m_pconnDb;
};

//===========================================================================
// Summary:
//     Define Empty DB string.
//===========================================================================
#define EMPTY_DB_STR L"abcdefghabcdefghabcdefghabcdefghabcde"


//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPCalendarDatabaseDataProvider, CXTPCalendarData)
//////////////////////////////////////////////////////////////////////
void AFX_CDECL CXTPCalendarDatabaseDataProvider::TRACE_ComError(_com_error &e)
{
	LPCWSTR bstrSource(e.Source());

	// Print COM errors.
	TRACE(_T("COM_ERROR: HRESULT = %08lx, Code meaning = %s, Source = %ls\n"),
		  e.Error(), (LPCTSTR)e.ErrorMessage(), (LPCWSTR)bstrSource);
}

//===========================================================================
void AFX_CDECL CXTPCalendarDatabaseDataProvider::TRACE_ProviderError(XTPADODB::_Connection* pConnDB)
{
	// Print Provider Errors from Connection object.

	ASSERT(pConnDB);
	if (!pConnDB)
		return;

	if (pConnDB->Errors->Count > 0)
	{
		long nCount = pConnDB->Errors->Count;

		TRACE(_T("DB ERROR(s): "));

		for (long i = 0; i < nCount; i++)
		{
			XTPADODB::ErrorPtr ptrErr = pConnDB->Errors->GetItem(i);
			TRACE(_T(" (Error- %x, '%ls'); "), ptrErr->Number, (LPCWSTR)ptrErr->Description);
		}
		TRACE(_T(" \n"));
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPCalendarDatabaseDataProvider::CXTPCalendarDatabaseDataProvider()
{
	m_pconnDb = NULL;
	m_bCloseDbConnection = TRUE;

	m_bTraceOptions = FALSE;

	m_typeProvider = xtpCalendarDataProviderDatabase;

	SetCacheMode(xtpCalendarDPCacheModeOnRepeat);

	m_pcmdAddEvent = NULL;
	m_pcmdDelEvent = NULL;
	m_pcmdUpdEvent = NULL;
	m_pcmdUpdEventPatternID = NULL;
	m_pcmdGetDayEvents = NULL;
	m_pcmdGetRExceptions = NULL;
	m_pcmdGetLastID = NULL;
	m_pcmdGetEvent = NULL;
	m_pcmdAddRPattern = NULL;
	m_pcmdDelRPattern = NULL;
	m_pcmdUpdRPattern = NULL;
	m_pcmdGetRPattern = NULL;

	m_pcmdRemoveAllEvents = NULL;
	m_pcmdRemoveAllRPatterns = NULL;
	m_pcmdRemoveAllOptions = NULL;

	m_pcmdGetOptions = NULL;
	m_pcmdAddOptions = NULL;

	m_pcmdAddSchedule = NULL;
	m_pcmdUpdSchedule = NULL;

	try
	{
		HRESULT hr = m_pconnDb.CreateInstance(__uuidof(XTPADODB::Connection));
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			TRACE(_T("ERROR: Cannot create XTPADODB::Connection. (HRESULT = %08lx) \n"), hr);
		}
	}
	catch(...)
	{
		ASSERT(FALSE);
		TRACE(_T("ERROR: Cannot create XTPADODB::Connection. Exception was happend. \n"));
	}
}

CXTPCalendarDatabaseDataProvider::~CXTPCalendarDatabaseDataProvider()
{
	try
	{
		DeleteAllDBCommands();

		Close();

		return;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
		ASSERT(FALSE);
	}
	catch (...)
	{
		ASSERT(FALSE);
	}
	TRACE(_T("ERROR: Exeption in ~CXTPCalendarDatabaseDataProvider()"));
}

void CXTPCalendarDatabaseDataProvider::DeleteAllDBCommands()
{
	CMDTARGET_RELEASE(m_pcmdAddEvent);

	CMDTARGET_RELEASE(m_pcmdDelEvent);

	CMDTARGET_RELEASE(m_pcmdUpdEvent);

	CMDTARGET_RELEASE(m_pcmdGetDayEvents);

	CMDTARGET_RELEASE(m_pcmdGetLastID);

	CMDTARGET_RELEASE(m_pcmdAddRPattern);

	CMDTARGET_RELEASE(m_pcmdDelRPattern);

	CMDTARGET_RELEASE(m_pcmdUpdRPattern);

	CMDTARGET_RELEASE(m_pcmdGetRPattern);

	CMDTARGET_RELEASE(m_pcmdGetRExceptions);

	CMDTARGET_RELEASE(m_pcmdGetEvent);

	CMDTARGET_RELEASE(m_pcmdUpdEventPatternID);

	CMDTARGET_RELEASE(m_pcmdRemoveAllEvents);
	CMDTARGET_RELEASE(m_pcmdRemoveAllRPatterns);
	CMDTARGET_RELEASE(m_pcmdRemoveAllOptions);

	CMDTARGET_RELEASE(m_pcmdGetOptions);
	CMDTARGET_RELEASE(m_pcmdAddOptions);

	CMDTARGET_RELEASE(m_pcmdAddSchedule);
	CMDTARGET_RELEASE(m_pcmdUpdSchedule);
}

BOOL CXTPCalendarDatabaseDataProvider::Open()
{
	USES_CONVERSION;

	if (m_strConnectionString.IsEmpty())
		return FALSE;

	if (IsOpen())
	{
		TRACE(_T("Provider is alredy opened. CXTPCalendarDatabaseDataProvider::Open(ConnectionString='%s') \n"), (LPCTSTR)m_strConnectionString);
		return TRUE;
	}

	if (m_pconnDb == NULL)
	{
		return FALSE;
	}

	// Check is the special mode to use OpenEx later.
	CString strConn = m_strConnectionString;
	strConn.MakeLower();
	if (strConn.Find(_T("provider=access;")) >= 0 || strConn == _T("provider=access") ||
		strConn.Find(_T("provider=ado;")) >= 0 || strConn == _T("provider=ado"))
	{
		return TRUE;
	}
	//-----------------------------------------------------------

	try
	{
		m_bCloseDbConnection = TRUE;
		HRESULT hr = m_pconnDb->Open(_bstr_t(CT2CW(m_strConnectionString)), L"", L"", -1);
		ASSERT(SUCCEEDED(hr));

		if (SUCCEEDED(hr) && _Open())
		{
			return TRUE;
		}

		TRACE(_T("ERROR: Cannot open DB Connection. (HRESULT = %08lx, ConnectionString='%s') \n"), hr, (LPCTSTR)m_strConnectionString);
		return FALSE;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch (...)
	{}
	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::Open(ConnectionString='%s') \n"), (LPCTSTR)m_strConnectionString);

	return FALSE;

}

BOOL CXTPCalendarDatabaseDataProvider::OpenEx(LPDISPATCH pdispADOConnection)
{
	USES_CONVERSION;

	if (IsOpen())
	{
		TRACE(_T("Provider is alredy opened. CXTPCalendarDatabaseDataProvider::OpenEx() \n"));
		return TRUE;
	}

	if (m_pconnDb == NULL) // Object was not created on constructor. ADO is not installed ???
	{
		return FALSE;
	}

	try
	{
		_variant_t vtADOConn(pdispADOConnection);
		m_pconnDb = vtADOConn;
		m_bCloseDbConnection = FALSE;

		if (_Open())
		{
			return TRUE;
		}

		TRACE(_T("ERROR: Cannot open using External ADO Connection. \n") );
		return FALSE;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch (...)
	{}
	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::OpenEx() \n"), (LPCTSTR)m_strConnectionString);

	return FALSE;

}

BOOL CXTPCalendarDatabaseDataProvider::_Open()
{
	USES_CONVERSION;

	if (m_pconnDb == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	VERIFY(UpdateDBStructure(m_pconnDb));

	CreateGetLastIDCommand();
	CreateAddEventCommand();
	CreateDelEventCommand();
	CreateUpdEventCommand();
	CreateGetDayEventCommand();
	CreateAddRPatternCommand();
	CreateDelRPatternCommand();
	CreateUpdRPatternCommand();
	CreateGetRPatternCommand();
	CreateGetRExceptionsCommand();
	CreateGetEventCommand();
	CreateUpdEventPatternIDCommand();
	CreateRemoveAllCommands();

	CreateGetOptionsCommand();
	CreateAddOptionsCommand();

	CreateAddScheduleCommand();
	CreateUpdScheduleCommand();

	CXTPCalendarData::Open();

	LoadOptions();

	CXTPCalendarSchedulesPtr ptrSchedules = LoadSchedules();
	if (ptrSchedules)
	{
		CMDTARGET_RELEASE(m_pSchedules);
		m_pSchedules = ptrSchedules;
		CMDTARGET_ADDREF(m_pSchedules);
	}
	else if (m_pSchedules)
	{
		m_pSchedules->RemoveAll();
	}

	return IsOpen();
}


BOOL CXTPCalendarDatabaseDataProvider::IsTableExist(XTPADOX::_CatalogPtr ptrCatalog, LPCWSTR pwszTableName)
{
	try
	{
		XTPADOX::_TablePtr ptrTable = ptrCatalog->Tables->GetItem(pwszTableName);

		return ptrTable != NULL;
	}
	catch (...)
	{}

	return FALSE;
}


void CXTPCalendarDatabaseDataProvider::CreateSchedulesTable(XTPADOX::_CatalogPtr ptrCatalog)
{
	XTPADOX::_TablePtr ptrTable;

	// Event table
	ptrTable.CreateInstance(__uuidof (XTPADOX::Table));
	ptrTable->Name = L"Schedules";

	// ------ Columns -------
	// ScheduleID
	XTPADOX::_ColumnPtr ptrID(__uuidof(XTPADOX::Column));
	ptrID->Name = L"ScheduleID";
	ptrID->Type = XTPADOX::adInteger;
	ptrID->ParentCatalog = ptrCatalog;
	ptrID->Properties->GetItem(L"AutoIncrement")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrID, true), XTPADOX::adInteger, 0);

	// Name
	XTPADOX::_ColumnPtr ptrName(__uuidof(XTPADOX::Column));
	ptrName->Name = L"Name";
	ptrName->Type = XTPADOX::adWChar;
	ptrName->ParentCatalog = ptrCatalog;
	ptrName->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrName, true), XTPADOX::adWChar, 255);

	// XMLData
	XTPADOX::_ColumnPtr ptrXMLData(__uuidof(XTPADOX::Column));
	ptrXMLData->Name = L"CustomPropertiesXMLData";
	ptrXMLData->Type = XTPADOX::adLongVarWChar;
	ptrXMLData->ParentCatalog = ptrCatalog;
	ptrXMLData->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrXMLData, true), XTPADOX::adLongVarWChar, 0);

	// SaveDateTime
	XTPADOX::_ColumnPtr ptrSaveDateTime(__uuidof(XTPADOX::Column));
	ptrSaveDateTime->Name = L"Modified";
	ptrSaveDateTime->Type = XTPADOX::adDate;
	ptrSaveDateTime->ParentCatalog = ptrCatalog;
	ptrSaveDateTime->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrSaveDateTime, true),   XTPADOX::adDate,   0);

	// --------- Keys ---------
	// Table key
	ptrTable->Keys->Append(L"PK", XTPADOX::adKeyPrimary, L"ScheduleID", L"", L"");

	ptrCatalog->Tables->Append(_variant_t((IDispatch *)ptrTable));
	ptrTable = NULL;
}

void CXTPCalendarDatabaseDataProvider::CreateOptionsTable(XTPADOX::_CatalogPtr ptrCatalog)
{
	XTPADOX::_TablePtr ptrTable;

	// Event table
	ptrTable.CreateInstance(__uuidof (XTPADOX::Table));
	ptrTable->Name = L"Options";

	// ------ Columns -------
	// EventID
	XTPADOX::_ColumnPtr ptrOID(__uuidof(XTPADOX::Column));
	ptrOID->Name = L"OptionsID";
	ptrOID->Type = XTPADOX::adInteger;
	ptrOID->ParentCatalog = ptrCatalog;
	ptrOID->Properties->GetItem(L"AutoIncrement")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrOID, true), XTPADOX::adInteger, 0);

	// SaveDateTime
	XTPADOX::_ColumnPtr ptrSaveDateTime(__uuidof(XTPADOX::Column));
	ptrSaveDateTime->Name = L"SaveDateTime";
	ptrSaveDateTime->Type = XTPADOX::adDate;
	ptrSaveDateTime->ParentCatalog = ptrCatalog;
	ptrSaveDateTime->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrSaveDateTime, true),   XTPADOX::adDate,   0);

	// XMLData
	XTPADOX::_ColumnPtr ptrXMLData(__uuidof(XTPADOX::Column));
	ptrXMLData->Name = L"XMLData";
	ptrXMLData->Type = XTPADOX::adLongVarWChar;
	ptrXMLData->ParentCatalog = ptrCatalog;

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrXMLData, true), XTPADOX::adLongVarWChar, 0);

	// --------- Keys ---------
	// Table key
	ptrTable->Keys->Append(L"PK", XTPADOX::adKeyPrimary, L"OptionsID", L"", L"");

	ptrCatalog->Tables->Append(_variant_t((IDispatch *)ptrTable));
	ptrTable = NULL;
}

void CXTPCalendarDatabaseDataProvider::CreateEventTable(XTPADOX::_CatalogPtr ptrCatalog)
{
	XTPADOX::_TablePtr ptrTable;

	// Event table
	ptrTable.CreateInstance(__uuidof (XTPADOX::Table));
	ptrTable->Name = L"Event";

	// ------ Columns -------
	// EventID
	XTPADOX::_ColumnPtr ptrEID(__uuidof(XTPADOX::Column));
	ptrEID->Name = L"EventID";
	ptrEID->Type = XTPADOX::adInteger;
	ptrEID->ParentCatalog = ptrCatalog;
	ptrEID->Properties->GetItem(L"AutoIncrement")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrEID, true), XTPADOX::adInteger, 0);

	// StartDateTime
	XTPADOX::_ColumnPtr ptrStartDateTime(__uuidof(XTPADOX::Column));
	ptrStartDateTime->Name = L"StartDateTime";
	ptrStartDateTime->Type = XTPADOX::adDate;
	ptrStartDateTime->ParentCatalog = ptrCatalog;
	ptrStartDateTime->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrStartDateTime, true),   XTPADOX::adDate,   0);

	// EndDateTime
	XTPADOX::_ColumnPtr ptrEndDateTime(__uuidof(XTPADOX::Column));
	ptrEndDateTime->Name = L"EndDateTime";
	ptrEndDateTime->Type = XTPADOX::adDate;
	ptrEndDateTime->ParentCatalog = ptrCatalog;
	ptrEndDateTime->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrEndDateTime, true), XTPADOX::adDate,   0);

	// RecurrenceState
	XTPADOX::_ColumnPtr ptrRS(__uuidof(XTPADOX::Column));
	ptrRS->Name = L"RecurrenceState";
	ptrRS->Type = XTPADOX::adInteger;
	ptrRS->ParentCatalog = ptrCatalog;
	ptrRS->Properties->GetItem(L"Nullable")->PutValue(_variant_t(false));
	ptrRS->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrRS->Properties->GetItem(L"Description")->PutValue(_variant_t(L"normal - 0, master 1, exception 3"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrRS, true), XTPADOX::adInteger, 0);

	// IsAllDayEvent
	XTPADOX::_ColumnPtr ptrIsADE(__uuidof(XTPADOX::Column));
	ptrIsADE->Name = L"IsAllDayEvent";
	ptrIsADE->Type = XTPADOX::adInteger;
	ptrIsADE->ParentCatalog = ptrCatalog;
	ptrIsADE->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrIsADE->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrIsADE->Properties->GetItem(L"Description")->PutValue(_variant_t(L"all day event 1, 0 - otherwise"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrIsADE, true), XTPADOX::adInteger, 0);

	// Subject
	XTPADOX::_ColumnPtr ptrSubject(__uuidof(XTPADOX::Column));
	ptrSubject->Name = L"Subject";
	ptrSubject->Type = XTPADOX::adWChar;
	ptrSubject->ParentCatalog = ptrCatalog;
	ptrSubject->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrSubject, true), XTPADOX::adWChar, 255);

	// Location
	XTPADOX::_ColumnPtr ptrLocation(__uuidof(XTPADOX::Column));
	ptrLocation->Name = L"Location";
	ptrLocation->Type = XTPADOX::adWChar;
	ptrLocation->ParentCatalog = ptrCatalog;
	ptrLocation->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrLocation, true), XTPADOX::adWChar, 255);

	// RemainderSoundFile
	XTPADOX::_ColumnPtr ptrRSF(__uuidof(XTPADOX::Column));
	ptrRSF->Name = L"RemainderSoundFile";
	ptrRSF->Type = XTPADOX::adWChar;
	ptrRSF->ParentCatalog = ptrCatalog;
	ptrRSF->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrRSF, true), XTPADOX::adWChar, 255);

	// Body
	XTPADOX::_ColumnPtr ptrBody(__uuidof(XTPADOX::Column));
	ptrBody->Name = L"Body";
	ptrBody->Type = XTPADOX::adLongVarWChar;
	ptrBody->ParentCatalog = ptrCatalog;
	ptrBody->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrBody, true), XTPADOX::adLongVarWChar, 0);

	// Created
	XTPADOX::_ColumnPtr ptrCreated(__uuidof(XTPADOX::Column));
	ptrCreated->Name = L"Created";
	ptrCreated->Type = XTPADOX::adDate;
	ptrCreated->ParentCatalog = ptrCatalog;
	ptrCreated->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrCreated, true), XTPADOX::adDate,   0);

	// Modified
	XTPADOX::_ColumnPtr ptrModified(__uuidof(XTPADOX::Column));
	ptrModified->Name = L"Modified";
	ptrModified->Type = XTPADOX::adDate;
	ptrModified->ParentCatalog = ptrCatalog;
	ptrModified->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrModified, true),    XTPADOX::adDate,   0);

	// BusyStatus
	XTPADOX::_ColumnPtr ptrBusyStatus(__uuidof(XTPADOX::Column));
	ptrBusyStatus->Name = L"BusyStatus";
	ptrBusyStatus->Type = XTPADOX::adInteger;
	ptrBusyStatus->ParentCatalog = ptrCatalog;
	ptrBusyStatus->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrBusyStatus->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrBusyStatus, true), XTPADOX::adInteger, 0);

	// ImportanceLevel
	XTPADOX::_ColumnPtr ptrImportanceLevel(__uuidof(XTPADOX::Column));
	ptrImportanceLevel->Name = L"ImportanceLevel";
	ptrImportanceLevel->Type = XTPADOX::adInteger;
	ptrImportanceLevel->ParentCatalog = ptrCatalog;
	ptrImportanceLevel->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrImportanceLevel->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrImportanceLevel, true), XTPADOX::adInteger, 0);

	// LabelID
	XTPADOX::_ColumnPtr ptrLabelID(__uuidof(XTPADOX::Column));
	ptrLabelID->Name = L"LabelID";
	ptrLabelID->Type = XTPADOX::adInteger;
	ptrLabelID->ParentCatalog = ptrCatalog;
	ptrLabelID->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrLabelID->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrLabelID, true), XTPADOX::adInteger, 0);

	// RecurrencePatternID
	XTPADOX::_ColumnPtr ptrRecurrencePatternID(__uuidof(XTPADOX::Column));
	ptrRecurrencePatternID->Name = L"RecurrencePatternID";
	ptrRecurrencePatternID->Type = XTPADOX::adInteger;
	ptrRecurrencePatternID->ParentCatalog = ptrCatalog;
	ptrRecurrencePatternID->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrRecurrencePatternID->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrRecurrencePatternID, true), XTPADOX::adInteger, 0);

	// ScheduleID
	XTPADOX::_ColumnPtr ptrScheduleID(__uuidof(XTPADOX::Column));
	ptrScheduleID->Name = L"ScheduleID";
	ptrScheduleID->Type = XTPADOX::adInteger;
	ptrScheduleID->ParentCatalog = ptrCatalog;
	ptrScheduleID->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrScheduleID->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrScheduleID, true), XTPADOX::adInteger, 0);

	// ISRecurrenceExceptionDeleted
	XTPADOX::_ColumnPtr ptrIsRED(__uuidof(XTPADOX::Column));
	ptrIsRED->Name = L"ISRecurrenceExceptionDeleted";
	ptrIsRED->Type = XTPADOX::adInteger;
	ptrIsRED->ParentCatalog = ptrCatalog;
	ptrIsRED->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrIsRED->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrIsRED, true), XTPADOX::adInteger, 0);

	// RExceptionStartTimeOrig
	XTPADOX::_ColumnPtr ptrREStart(__uuidof(XTPADOX::Column));
	ptrREStart->Name = L"RExceptionStartTimeOrig";
	ptrREStart->Type = XTPADOX::adDate;
	ptrREStart->ParentCatalog = ptrCatalog;
	ptrREStart->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrREStart, true), XTPADOX::adDate,   0);

	// RExceptionEndTimeOrig
	XTPADOX::_ColumnPtr ptrREEnd(__uuidof(XTPADOX::Column));
	ptrREEnd->Name = L"RExceptionEndTimeOrig";
	ptrREEnd->Type = XTPADOX::adDate;
	ptrREEnd->ParentCatalog = ptrCatalog;
	ptrREEnd->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrREEnd, true),   XTPADOX::adDate,   0);

	// IsMeeting
	XTPADOX::_ColumnPtr ptrIsMeeting(__uuidof(XTPADOX::Column));
	ptrIsMeeting->Name = L"IsMeeting";
	ptrIsMeeting->Type = XTPADOX::adInteger;
	ptrIsMeeting->ParentCatalog = ptrCatalog;
	ptrIsMeeting->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrIsMeeting->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrIsMeeting, true), XTPADOX::adInteger, 0);

	// IsPrivate
	XTPADOX::_ColumnPtr ptrIsPrivate(__uuidof(XTPADOX::Column));
	ptrIsPrivate->Name = L"IsPrivate";
	ptrIsPrivate->Type = XTPADOX::adInteger;
	ptrIsPrivate->ParentCatalog = ptrCatalog;
	ptrIsPrivate->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrIsPrivate->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrIsPrivate, true), XTPADOX::adInteger, 0);

	// IsReminder
	XTPADOX::_ColumnPtr ptrIsReminder(__uuidof(XTPADOX::Column));
	ptrIsReminder->Name = L"IsReminder";
	ptrIsReminder->Type = XTPADOX::adInteger;
	ptrIsReminder->ParentCatalog = ptrCatalog;
	ptrIsReminder->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrIsReminder->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrIsReminder, true), XTPADOX::adInteger, 0);

	// ReminderMinutesBeforeStart
	XTPADOX::_ColumnPtr ptrReminderMin(__uuidof(XTPADOX::Column));
	ptrReminderMin->Name = L"ReminderMinutesBeforeStart";
	ptrReminderMin->Type = XTPADOX::adInteger;
	ptrReminderMin->ParentCatalog = ptrCatalog;
	ptrReminderMin->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrReminderMin->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrReminderMin, true), XTPADOX::adInteger, 0);

	// CustomProperties
	XTPADOX::_ColumnPtr ptrXMLData(__uuidof(XTPADOX::Column));
	ptrXMLData->Name = L"CustomPropertiesXMLData";
	ptrXMLData->Type = XTPADOX::adLongVarWChar;
	ptrXMLData->ParentCatalog = ptrCatalog;

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrXMLData, true), XTPADOX::adLongVarWChar, 0);

	// --------- Keys ---------
	// Table key
	ptrTable->Keys->Append(L"PK", XTPADOX::adKeyPrimary, L"EventID", L"", L"");

	ptrCatalog->Tables->Append(_variant_t((IDispatch *)ptrTable));
	ptrTable = NULL;

}

void CXTPCalendarDatabaseDataProvider::CreateRPatternTable(XTPADOX::_CatalogPtr ptrCatalog)
{
	XTPADOX::_TablePtr ptrTable;

	// Event table
	ptrTable.CreateInstance(__uuidof (XTPADOX::Table));
	ptrTable->Name = L"RecurrencePattern";

	// -------- Columns -------
	// RecurrencePatternID
	XTPADOX::_ColumnPtr ptrRPID(__uuidof(XTPADOX::Column));
	ptrRPID->Name = L"RecurrencePatternID";
	ptrRPID->Type = XTPADOX::adInteger;
	ptrRPID->ParentCatalog = ptrCatalog;
	ptrRPID->Properties->GetItem(L"AutoIncrement")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrRPID, true), XTPADOX::adInteger, 0);

	// PatternStartDateTime
	XTPADOX::_ColumnPtr ptrPatternStartDateTime(__uuidof(XTPADOX::Column));
	ptrPatternStartDateTime->Name = L"PatternStartDateTime";
	ptrPatternStartDateTime->Type = XTPADOX::adDate;
	ptrPatternStartDateTime->ParentCatalog = ptrCatalog;
	ptrPatternStartDateTime->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrPatternStartDateTime, true),    XTPADOX::adDate,   0);

	// PatternEndDateTime
	XTPADOX::_ColumnPtr ptrEndDateTime(__uuidof(XTPADOX::Column));
	ptrEndDateTime->Name = L"PatternEndDateTime";
	ptrEndDateTime->Type = XTPADOX::adDate;
	ptrEndDateTime->ParentCatalog = ptrCatalog;
	ptrEndDateTime->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrEndDateTime, true), XTPADOX::adDate,   0);

	// StartDateTime
	XTPADOX::_ColumnPtr ptrStartDateTime(__uuidof(XTPADOX::Column));
	ptrStartDateTime->Name = L"StartDateTime";
	ptrStartDateTime->Type = XTPADOX::adDate;
	ptrStartDateTime->ParentCatalog = ptrCatalog;
	ptrStartDateTime->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrStartDateTime, true),   XTPADOX::adDate,   0);

	// Duration
	XTPADOX::_ColumnPtr ptrDuration(__uuidof(XTPADOX::Column));
	ptrDuration->Name = L"Duration";
	ptrDuration->Type = XTPADOX::adInteger;
	ptrDuration->ParentCatalog = ptrCatalog;
	ptrDuration->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrDuration->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrDuration, true), XTPADOX::adInteger, 0);

	// EndAfterOccurrence
	XTPADOX::_ColumnPtr ptrEndAfterOccurrence(__uuidof(XTPADOX::Column));
	ptrEndAfterOccurrence->Name = L"EndAfterOccurrence";
	ptrEndAfterOccurrence->Type = XTPADOX::adInteger;
	ptrEndAfterOccurrence->ParentCatalog = ptrCatalog;
	ptrEndAfterOccurrence->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrEndAfterOccurrence->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrEndAfterOccurrence, true), XTPADOX::adInteger, 0);

	// UseEnd
	XTPADOX::_ColumnPtr ptrUseEnd(__uuidof(XTPADOX::Column));
	ptrUseEnd->Name = L"UseEnd";
	ptrUseEnd->Type = XTPADOX::adInteger;
	ptrUseEnd->ParentCatalog = ptrCatalog;
	ptrUseEnd->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrUseEnd->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrUseEnd, true), XTPADOX::adInteger, 0);

	// RecurrenceTypeID
	XTPADOX::_ColumnPtr ptrRecurrenceTypeID(__uuidof(XTPADOX::Column));
	ptrRecurrenceTypeID->Name = L"RecurrenceTypeID";
	ptrRecurrenceTypeID->Type = XTPADOX::adInteger;
	ptrRecurrenceTypeID->ParentCatalog = ptrCatalog;
	ptrRecurrenceTypeID->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrRecurrenceTypeID->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrRecurrenceTypeID->Properties->GetItem(L"Description")->PutValue(_variant_t(L"XTPCalendarEventRecurrenceType"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrRecurrenceTypeID, true), XTPADOX::adInteger, 0);

	// OccurrenceInterval
	XTPADOX::_ColumnPtr ptrOccurrenceInterval(__uuidof(XTPADOX::Column));
	ptrOccurrenceInterval->Name = L"OccurrenceInterval";
	ptrOccurrenceInterval->Type = XTPADOX::adInteger;
	ptrOccurrenceInterval->ParentCatalog = ptrCatalog;
	ptrOccurrenceInterval->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrOccurrenceInterval->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrOccurrenceInterval->Properties->GetItem(L"Description")->PutValue(_variant_t(L"occurs evere Nth (day, week, month)"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrOccurrenceInterval, true), XTPADOX::adInteger, 0);

	// DayOfWeekMask
	XTPADOX::_ColumnPtr ptrDayOfWeekMask(__uuidof(XTPADOX::Column));
	ptrDayOfWeekMask->Name = L"DayOfWeekMask";
	ptrDayOfWeekMask->Type = XTPADOX::adInteger;
	ptrDayOfWeekMask->ParentCatalog = ptrCatalog;
	ptrDayOfWeekMask->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrDayOfWeekMask->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrDayOfWeekMask->Properties->GetItem(L"Description")->PutValue(_variant_t(L"stores weeks day (for  weeks, month) in packed form"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrDayOfWeekMask, true), XTPADOX::adInteger, 0);

	// DayOfMonth
	XTPADOX::_ColumnPtr ptrDayOfMonth(__uuidof(XTPADOX::Column));
	ptrDayOfMonth->Name = L"DayOfMonth";
	ptrDayOfMonth->Type = XTPADOX::adInteger;
	ptrDayOfMonth->ParentCatalog = ptrCatalog;
	ptrDayOfMonth->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrDayOfMonth->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrDayOfMonth->Properties->GetItem(L"Description")->PutValue(_variant_t(L"day of month number (for monthly, yearly)"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrDayOfMonth, true), XTPADOX::adInteger, 0);

	// MonthOfYear
	XTPADOX::_ColumnPtr ptrMonthOfYear(__uuidof(XTPADOX::Column));
	ptrMonthOfYear->Name = L"MonthOfYear";
	ptrMonthOfYear->Type = XTPADOX::adInteger;
	ptrMonthOfYear->ParentCatalog = ptrCatalog;
	ptrMonthOfYear->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrMonthOfYear->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrMonthOfYear->Properties->GetItem(L"Description")->PutValue(_variant_t(L"for yearly: (month number mapped to month name)"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrMonthOfYear, true), XTPADOX::adInteger, 0);

	// IsEveryWeekDay
	XTPADOX::_ColumnPtr ptrIsEveryWeekDay(__uuidof(XTPADOX::Column));
	ptrIsEveryWeekDay->Name = L"IsEveryWeekDay";
	ptrIsEveryWeekDay->Type = XTPADOX::adInteger;
	ptrIsEveryWeekDay->ParentCatalog = ptrCatalog;
	ptrIsEveryWeekDay->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrIsEveryWeekDay->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrIsEveryWeekDay->Properties->GetItem(L"Description")->PutValue(_variant_t(L"For daily: is occured each weekday"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrIsEveryWeekDay, true), XTPADOX::adInteger, 0);

	// DayOccurrenceTypeID
	XTPADOX::_ColumnPtr ptrDayOccurrenceTypeID(__uuidof(XTPADOX::Column));
	ptrDayOccurrenceTypeID->Name = L"DayOccurrenceTypeID";
	ptrDayOccurrenceTypeID->Type = XTPADOX::adInteger;
	ptrDayOccurrenceTypeID->ParentCatalog = ptrCatalog;
	ptrDayOccurrenceTypeID->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrDayOccurrenceTypeID->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrDayOccurrenceTypeID->Properties->GetItem(L"Description")->PutValue(_variant_t(L"Day occurrence:  first, second, third, fourth or last occurrence of day  ( WhichDay)"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrDayOccurrenceTypeID, true), XTPADOX::adInteger, 0);

	// DayMask
	XTPADOX::_ColumnPtr ptrDayMask(__uuidof(XTPADOX::Column));
	ptrDayMask->Name = L"DayMask";
	ptrDayMask->Type = XTPADOX::adInteger;
	ptrDayMask->ParentCatalog = ptrCatalog;
	ptrDayMask->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrDayMask->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrDayMask, true), XTPADOX::adInteger, 0);

	// MasterEventID
	XTPADOX::_ColumnPtr ptrMasterEventID(__uuidof(XTPADOX::Column));
	ptrMasterEventID->Name = L"MasterEventID";
	ptrMasterEventID->Type = XTPADOX::adInteger;
	ptrMasterEventID->ParentCatalog = ptrCatalog;
	ptrMasterEventID->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
	ptrMasterEventID->Properties->GetItem(L"Default")->PutValue(_variant_t((long)0, VT_I4));
	ptrMasterEventID->Properties->GetItem(L"Description")->PutValue(_variant_t(L"EventID of master event"));

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrMasterEventID, true), XTPADOX::adInteger, 0);

	// CustomProperties
	XTPADOX::_ColumnPtr ptrXMLData(__uuidof(XTPADOX::Column));
	ptrXMLData->Name = L"CustomPropertiesXMLData";
	ptrXMLData->Type = XTPADOX::adLongVarWChar;
	ptrXMLData->ParentCatalog = ptrCatalog;

	ptrTable->Columns->Append(_variant_t((IDispatch*)ptrXMLData, true), XTPADOX::adLongVarWChar, 0);

	// --------- Keys ---------
	// Table primary key
	ptrTable->Keys->Append(L"PK", XTPADOX::adKeyPrimary, L"RecurrencePatternID", L"", L"");
	// Event key
	{
//      XTPADOX::_KeyPtr ptrKey(__uuidof(XTPADOX::Key));
//      ptrKey->PutName(_T("RTForeign"));
//      ptrKey->PutType(XTPADOX::adKeyForeign);
//      ptrKey->PutRelatedTable(_T("Event"));
//      XTPADOX::ColumnsPtr cols = ptrKey->GetColumns();
//      cols->Append(_T("RecurrencePatternID"), XTPADOX::adInteger, 0);
//      cols->GetItem(L"RecurrencePatternID"))->PutRelatedColumn(_T("RecurrencePatternID"));
		// reference index
//      XTPADOX::_IndexPtr ptrIndex(__uuidof(XTPADOX::Index));
//      ptrIndex->PutName(_T("ref_RecurrencePatternID"));
//      ptrIndex->PutUnique(_variant_t(false));
//      ptrIndex->Columns->Append(_T("RecurrencePatternID"), XTPADOX::adInteger, 0);
//      ptrCatalog->GetTables()->GetItem(L"Event"))->Indexes->Append(_variant_t((IDispatch *)ptrIndex));
//
//      ptrTable->Keys->Append(_T("FK1"), XTPADOX::adKeyForeign, _T("RecurrencePatternID"), _T("Event"), _T("RecurrencePatternID"));
	}

	ptrCatalog->Tables->Append(_variant_t((IDispatch *)ptrTable));
	ptrTable = NULL;

}

//void CXTPCalendarDatabaseDataProvider::CreateDicTable(XTPADOX::_CatalogPtr ptrCatalog,
//      LPCWSTR strTable, LPCWSTR strNameID, bool bAuto)
//{
//  XTPADOX::_TablePtr ptrTable;
//
//  // Event table
//  ptrTable.CreateInstance(__uuidof (XTPADOX::Table));
//  ptrTable->Name = strTable;
//
//  // -------- Columns -------
//  // ID
//  XTPADOX::_ColumnPtr ptrID(__uuidof(XTPADOX::Column));
//  ptrID->Name = strNameID;
//  ptrID->Type = XTPADOX::adInteger;
//  ptrID->ParentCatalog = ptrCatalog;
//  if (bAuto)
//  {
//      ptrID->Properties->GetItem(L"AutoIncrement")->PutValue(_variant_t(true));
//  }
//  ptrTable->Columns->Append(_variant_t((IDispatch*)ptrID, true), XTPADOX::adInteger, 0);
//
//  // Body
//  XTPADOX::_ColumnPtr ptrBody(__uuidof(XTPADOX::Column));
//  ptrBody->Name = L"Desc";
//  ptrBody->Type = XTPADOX::adWChar;
//  ptrBody->ParentCatalog = ptrCatalog;
//  ptrBody->Properties->GetItem(L"Nullable")->PutValue(_variant_t(true));
//
//  ptrTable->Columns->Append(_variant_t((IDispatch*)ptrBody, true), XTPADOX::adWChar, 255);
//
//  // --------- Keys ---------
//  // Table key
//  ptrTable->Keys->Append(L"PK", XTPADOX::adKeyPrimary, strNameID, L"", L"");
//
//  ptrCatalog->Tables->Append(_variant_t((IDispatch *)ptrTable));
//  ptrTable = NULL;
//}

BOOL CXTPCalendarDatabaseDataProvider::Create()
{
	try
	{
		USES_CONVERSION;

		XTPADOX::_CatalogPtr ptrCatalog;

		// Create database file
		if (FAILED(ptrCatalog.CreateInstance(__uuidof(XTPADOX::Catalog))))
			return FALSE;

		ptrCatalog->Create(_bstr_t(CT2CW(m_strConnectionString)));

		// Create main tables
		CreateEventTable(ptrCatalog);
		CreateRPatternTable(ptrCatalog);

		// Create Schedules table
		CreateSchedulesTable(ptrCatalog);

		// Create options table
		CreateOptionsTable(ptrCatalog);

		ptrCatalog = NULL;
	}
	catch (_com_error &e)
	{
		e;
#ifdef _DEBUG
		TRACE(_T("ERROR: CXTPCalendarDatabaseDataProvider::Create: "));
		LPCWSTR err(e.Description());
		if (err) TRACE(_T("%ls"), err);
#endif

		return FALSE;
	}

	return Open();
}

BOOL CXTPCalendarDatabaseDataProvider::UpdateDBStructure(XTPADODB::_Connection* pconnDb)
{
	try
	{
		XTPADOX::_CatalogPtr ptrCatalog;

		if (FAILED(ptrCatalog.CreateInstance(__uuidof(XTPADOX::Catalog))))
			return FALSE;

		ptrCatalog->PutRefActiveConnection((IDispatch*)pconnDb);

		if (!IsTableExist(ptrCatalog, L"Event"))
			CreateEventTable(ptrCatalog);

		if (!IsTableExist(ptrCatalog, L"RecurrencePattern"))
			CreateRPatternTable(ptrCatalog);

		if (!IsTableExist(ptrCatalog, L"Options"))
			CreateOptionsTable(ptrCatalog);

		if (!IsTableExist(ptrCatalog, L"Schedules"))
			CreateSchedulesTable(ptrCatalog);


		return TRUE;
	}
	catch (_com_error &e)
	{
		e;
#ifdef _DEBUG
		TRACE(_T("ERROR: CXTPCalendarDatabaseDataProvider::UpdateDB: "));
		LPCWSTR err(e.Description());
		if (err) TRACE(_T("%ls"), err);
#endif
	}
	return FALSE;
}

BOOL CXTPCalendarDatabaseDataProvider::Save()
{
	VERIFY( CXTPCalendarData::Save() );

	UpdateSchedules();

	// Save calendar's options
#ifndef _XTP_EXCLUDE_XML
	CXTPPropExchangeXMLNode px(FALSE, 0, _T("Calendar"));
	px.ExchangeLocale();

	CXTPPropExchangeSection secOptions(px.GetSection(_T("Options")));
	secOptions->EmptySection();
	// TODO:
	if (m_pCalendarOptions)
	{
		m_pCalendarOptions->DoPropExchange(&secOptions);
	}
	CString strXML(px.GetXML());

	return SaveOptions(strXML);
#else
	return TRUE;
#endif

}

BOOL CXTPCalendarDatabaseDataProvider::LoadOptions()
{
	// Load calendar's options

	if (!m_pCalendarOptions)
	{
		return FALSE;
	}

#ifndef _XTP_EXCLUDE_XML
	// Load data from the database
	XTPADODB::_RecordsetPtr ptrDataRS = m_pcmdGetOptions->Execute();
	if (ptrDataRS == NULL || ptrDataRS->bEOF)
		return FALSE;

	CString strXMLData = _GetFieldString(ptrDataRS, L"XMLData");
	ptrDataRS->Close();

	// Populate XML node with data
	CXTPPropExchangeXMLNode px(TRUE, 0, _T("Calendar"));
	if (!px.LoadFromString(strXMLData))
		return FALSE;

	// Apply data to the calendar
	px.ExchangeLocale();

	CXTPPropExchangeSection secOptions(px.GetSection(_T("Options")));
	m_pCalendarOptions->DoPropExchange(&secOptions);

	return TRUE;
#else
	return FALSE;
#endif
}

CXTPCalendarSchedulesPtr CXTPCalendarDatabaseDataProvider::LoadSchedules()
{
	if (!IsOpen())
	{
		return NULL;
	}

	CXTPCalendarSchedulesPtr ptrSchedules = new CXTPCalendarSchedules();
	if (!ptrSchedules)
		return NULL;

	CADOCommand cmdGetSchedules(m_pconnDb.GetInterfacePtr(),
								L"SELECT * FROM Schedules");
	try
	{
		XTPADODB::_RecordsetPtr ptrDataRS = cmdGetSchedules.Execute();
		if (ptrDataRS == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		// process recordset
		while (!ptrDataRS->bEOF)
		{
			CXTPCalendarSchedule* pNewSch = _ReadSchedule(ptrDataRS);
			if (pNewSch)
				ptrSchedules->Add(pNewSch, FALSE);

			// next record
			ptrDataRS->MoveNext();
		}

		ptrDataRS->Close();

		return ptrSchedules;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::LoadSchedules() \n"));
	return NULL;
}

CXTPCalendarSchedule* CXTPCalendarDatabaseDataProvider::_ReadSchedule(XTPADODB::_Recordset* pRS)
{
	ASSERT(pRS);
	if (!pRS)
		return NULL;

	try
	{   ULONG ulID = (ULONG)(long)pRS->Fields->GetItem(L"ScheduleID")->Value;
		CString strName = _GetFieldString(pRS, L"Name");
		CString strPropsXMLData = _GetFieldString(pRS, L"CustomPropertiesXMLData");

		CXTPCalendarSchedule* pNewSch = new CXTPCalendarSchedule(ulID, strName);

		if (pNewSch && pNewSch->GetCustomProperties())
			pNewSch->GetCustomProperties()->LoadFromXML(strPropsXMLData);

		return pNewSch;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{
		ASSERT(FALSE);
	}

	return NULL;
}

void CXTPCalendarDatabaseDataProvider::UpdateSchedules()
{
	if (!m_pSchedules)
		return;

	CXTPCalendarSchedulesPtr ptrSchedulesOrig = LoadSchedules();

	if (!ptrSchedulesOrig || ptrSchedulesOrig->GetCount() == 0)
	{
		_AddSchedules(m_pSchedules);
		return;
	}

	CXTPCalendarSchedules arAdd;
	CXTPCalendarSchedules arUpdate;
	CUIntArray arDelete;

	int i;
	int nCount = m_pSchedules->GetCount();
	for (i = 0; i < nCount; i++)
	{
		CXTPCalendarSchedule* pSch = m_pSchedules->GetAt(i, FALSE);
		int nIdx = ptrSchedulesOrig->FindIndex(pSch->GetID());

		if (nIdx >= 0)
			arUpdate.Add(pSch, TRUE);
		else
			arAdd.Add(pSch, TRUE);
	}

	int nCount_orig = ptrSchedulesOrig->GetCount();
	for (i = 0; i < nCount_orig; i++)
	{
		CXTPCalendarSchedule* pSch = ptrSchedulesOrig->GetAt(i, FALSE);
		int nIdx = m_pSchedules->FindIndex(pSch->GetID());

		if (nIdx < 0)
			arDelete.Add(pSch->GetID());
	}

	_AddSchedules(&arAdd);
	_UpdateSchedules(&arUpdate, ptrSchedulesOrig);
	_DeleteSchedules(&arDelete);
}

void CXTPCalendarDatabaseDataProvider::_AddSchedules(CXTPCalendarSchedules* pAddSet)
{
	if (!IsOpen())
	{
		return;
	}

	if (!pAddSet || !m_pcmdAddSchedule)
	{
		return;
	}

	_variant_t vtdtNow = DATE2VAR(COleDateTime::GetCurrentTime());

	int nCount = pAddSet->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarSchedule* pSch = pAddSet->GetAt(i, FALSE);
		ASSERT(pSch);
		if (!pSch)
			continue;
		try
		{
			CString strName = pSch->GetName();
			CString strData;
			if (pSch->GetCustomProperties())
				pSch->GetCustomProperties()->SaveToXML(strData);

			m_pcmdAddSchedule->SetParameterValue(L"prm_Name", STRING2VAR(strName));
			m_pcmdAddSchedule->SetParameterValue(L"prm_Modified", vtdtNow);
			m_pcmdAddSchedule->SetParameterValue(L"prm_CustomPropertiesXMLData", STRING2VAR(strData));

			m_pcmdAddSchedule->Execute();

			// get ID
			CADOCommand cmdGetLastID(m_pconnDb.GetInterfacePtr(), L"SELECT MAX(ScheduleID) AS NewScheduleID FROM Schedules");
			XTPADODB::_RecordsetPtr ptrRS_NewID = cmdGetLastID.Execute();
			ASSERT(ptrRS_NewID != NULL);
			if (ptrRS_NewID)
			{
				ASSERT(!ptrRS_NewID->bEOF);
				if (!ptrRS_NewID->bEOF)
				{
					UINT uNewSchID = (DWORD)(long)ptrRS_NewID->Fields->GetItem(L"NewScheduleID")->Value;
					pSch->SetID(uNewSchID);
				}
				ptrRS_NewID->Close();
			}
		}
		catch (_com_error &e)
		{
			TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::_AddSchedules()"));
			TRACE_ProviderError(m_pconnDb);
			TRACE_ComError(e);
		}
		catch(...)
		{
			TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::_AddSchedules()"));
		}
	}
}

void CXTPCalendarDatabaseDataProvider::_UpdateSchedules(CXTPCalendarSchedules* pUpdateSet, CXTPCalendarSchedules* pOrigSet)
{
	if (!IsOpen())
	{
		return;
	}

	if (!pUpdateSet || !m_pcmdUpdSchedule)
	{
		return;
	}

	_variant_t vtdtNow = DATE2VAR(COleDateTime::GetCurrentTime());

	int nCount = pUpdateSet->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarSchedule* pSch = pUpdateSet->GetAt(i, FALSE);
		ASSERT(pSch);
		if (!pSch)
			continue;

		long nID = pSch->GetID();
		CString strName = pSch->GetName();
		CString strData;
		XTP_SAFE_CALL2(pSch, GetCustomProperties(), SaveToXML(strData));

		// check is changed
		if (pOrigSet)
		{
			int nIdx = pOrigSet->FindIndex(nID);
			ASSERT(nIdx >= 0);

			CXTPCalendarSchedule* pSch_orig = pOrigSet->GetAt(nIdx, FALSE);
			ASSERT(pSch_orig);
			if (pSch_orig)
			{
				CString strData_orig;
				XTP_SAFE_CALL2(pSch_orig, GetCustomProperties(), SaveToXML(strData_orig));

				if (strName == pSch_orig->GetName() &&
					strData_orig == strData)
				{
					continue; // skip update if no changes
				}
			}
		}

		try
		{
			m_pcmdUpdSchedule->SetParameterValue(L"prm_ScheduleID", nID);
			m_pcmdUpdSchedule->SetParameterValue(L"prm_Name", STRING2VAR(strName));
			m_pcmdUpdSchedule->SetParameterValue(L"prm_Modified", vtdtNow);
			m_pcmdUpdSchedule->SetParameterValue(L"prm_CustomPropertiesXMLData", STRING2VAR(strData));

			m_pcmdUpdSchedule->Execute();
		}
		catch (_com_error &e)
		{
			TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::_UpdateSchedules()"));
			TRACE_ProviderError(m_pconnDb);
			TRACE_ComError(e);
		}
		catch(...)
		{
			TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::_UpdateSchedules()"));
		}
	}
}

void CXTPCalendarDatabaseDataProvider::_DeleteSchedules(CUIntArray* pDeleteIDs)
{
	USES_CONVERSION;

	if (!IsOpen() || !pDeleteIDs || pDeleteIDs->GetSize() == 0)
	{
		return;
	}

	TCHAR szBuff[64];
	CString strIDs;

	int nCount = (int)pDeleteIDs->GetSize();
	for (int i = 0; i < nCount; i++)
	{
		int nID = (int)pDeleteIDs->GetAt(i);
		if (i)
			strIDs += _T(",");

		ITOT_S(nID, szBuff, 64, 10);
		strIDs += szBuff;
	}

	CString strSqlText = _T("DELETE FROM Schedules WHERE ScheduleID IN ( ");
	strSqlText += strIDs;
	strSqlText += _T(" )");

	try
	{
		CADOCommand cmdDeleteSchedules(m_pconnDb.GetInterfacePtr(), CT2CW(strSqlText));
		cmdDeleteSchedules.Execute();

		return;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{
	}
	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::_DeleteSchedules()"));
}

void CXTPCalendarDatabaseDataProvider::Close()
{
	if (!IsOpen())
		return;

	CXTPCalendarData::Close();

	try
	{
		DeleteAllDBCommands();

		if (m_bCloseDbConnection)
		{
			HRESULT hr = m_pconnDb->Close();
			ASSERT(SUCCEEDED(hr));

			if (FAILED(hr))
			{
				TRACE(_T("ERROR: Cannot close XTPADODB::Connection. (HRESULT = %08lx). CXTPCalendarDatabaseDataProvider::Close() \n"), hr);
			}
		}
		return;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{
	}
	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::Close"));
}

CXTPCalendarEventPtr CXTPCalendarDatabaseDataProvider::_ReadEvent_common(XTPADODB::_Recordset* pRS,
																		 BOOL bEcxception)
{
	if (!IsOpen())
	{
		return NULL;
	}

	try
	{
		DWORD dwEventID = (DWORD)(long)pRS->Fields->GetItem(L"EventID")->Value;
		CXTPCalendarEventPtr ptrEvent = CreateNewEvent(dwEventID);

		ptrEvent->SetEndTime((DATE)pRS->Fields->GetItem(L"EndDateTime")->Value);
		ptrEvent->SetStartTime((DATE)pRS->Fields->GetItem(L"StartDateTime")->Value);

		ptrEvent->SetAllDayEvent(_GetField<long>(pRS, L"IsAllDayEvent", 0) != 0);

		ptrEvent->SetSubject(_GetFieldString(pRS, L"Subject"));
		ptrEvent->SetLocation(_GetFieldString(pRS, L"Location"));
		ptrEvent->SetBody(_GetFieldString(pRS, L"Body"));

		ptrEvent->SetReminderSoundFile(_GetFieldString(pRS, L"RemainderSoundFile"));

		ptrEvent->SetBusyStatus(_GetField<long>(pRS, L"BusyStatus", 0));
		ptrEvent->SetImportance(_GetField<long>(pRS, L"ImportanceLevel", 0));
		ptrEvent->SetLabelID(_GetField<long>(pRS, L"LabelID", 0));
		ptrEvent->SetScheduleID(_GetField<long>(pRS, L"ScheduleID", 0));

		ptrEvent->SetCreationTime(_GetField<DATE>(pRS, L"Created", 0));
		ptrEvent->SetLastModificationTime(_GetField<DATE>(pRS, L"Modified", 0));

		ptrEvent->SetMeeting(_GetField<long>(pRS, L"IsMeeting", 0) != 0);
		ptrEvent->SetPrivate(_GetField<long>(pRS, L"IsPrivate", 0) != 0);

		ptrEvent->SetReminder(_GetField<long>(pRS, L"IsReminder", 0) != 0);
		ptrEvent->SetReminderMinutesBeforeStart(_GetField<long>(pRS, L"ReminderMinutesBeforeStart", 0));

		if (ptrEvent->GetCustomProperties())
		{
			CString bstrPropsXMLData = _GetFieldString(pRS, L"CustomPropertiesXMLData");
			ptrEvent->GetCustomProperties()->LoadFromXML(bstrPropsXMLData);

			COleVariant ovarIconsIDs;
			if (ptrEvent->GetCustomProperties()->GetProperty(cszEventCustProp_IconsIDs, ovarIconsIDs))
			{
				VERIFY(ptrEvent->GetCustomProperties()->RemoveProperty(cszEventCustProp_IconsIDs));

				CString strArray = VariantToString(ovarIconsIDs, NULL);
				ptrEvent->GetCustomIcons()->LoadFromString(strArray);

				ptrEvent->GetCustomProperties()->RemoveProperty(cszEventCustProp_IconsIDs);
			}
		}

		if (!bEcxception)
		{
			long nRState = _GetField<long>(pRS, L"RecurrenceState", xtpCalendarRecurrenceNotRecurring);
			long nPatternID = _GetField<long>(pRS, L"RecurrencePatternID", XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID);

			ptrEvent->GetCustomProperties()->SetProperty(cszProcess_RecurrenceState, COleVariant((long)nRState));
			ptrEvent->GetCustomProperties()->SetProperty(cszProcess_RecurrencePatternID, COleVariant((long)nPatternID));
		}

		return ptrEvent;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::ReadEvent()"));
	return NULL;
}

CXTPCalendarEventsPtr CXTPCalendarDatabaseDataProvider::DoRetrieveDayEvents(COleDateTime dtDay)
{
	if (!IsOpen())
	{
		return NULL;
	}

	dtDay = CXTPCalendarUtils::ResetTime(dtDay);

	CXTPCalendarEventsPtr ptrEvents = new CXTPCalendarEvents();
	if (!ptrEvents)
	{
		return NULL;
	}
	CXTPCalendarEventPtr  ptrEvent;

	if (!m_pcmdGetDayEvents)
	{
		ASSERT(FALSE);
		return NULL;
	}

	m_pcmdGetDayEvents->SetParameterValue(L"prm_DayDate", DATE2VAR(dtDay));

	try
	{
		XTPADODB::_RecordsetPtr ptrDataRS = m_pcmdGetDayEvents->Execute();
		if (ptrDataRS == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		// process recordset
		while (!ptrDataRS->bEOF) //
		{
			ptrEvent = _ReadEvent_common(ptrDataRS, FALSE);
			if (!ptrEvent)
			{
				ASSERT(FALSE);
				ptrDataRS->MoveNext();
				continue;
			}

			ptrEvents->Add(ptrEvent);

			// next record
			ptrDataRS->MoveNext();
		}

		ptrDataRS->Close();

		return ptrEvents;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::RetrieveDayEvents(%s) \n"), (LPCTSTR)dtDay.Format());
	return NULL;
}

CXTPCalendarEventsPtr CXTPCalendarDatabaseDataProvider::DoGetAllEvents_raw()
{
	USES_CONVERSION;

	if (!IsOpen())
	{
		return NULL;
	}

	CXTPCalendarEventsPtr ptrEvents = new CXTPCalendarEvents();
	if (!ptrEvents)
	{
		return NULL;
	}
	CString strSqlText;
	strSqlText.Format(_T("SELECT * FROM Event WHERE RecurrenceState = %d OR RecurrenceState = %d"),
						xtpCalendarRecurrenceNotRecurring, xtpCalendarRecurrenceMaster);

	CADOCommand cmdGetAllEvents_raw(m_pconnDb.GetInterfacePtr(), CT2CW(strSqlText));

	CXTPCalendarEventPtr  ptrEvent;

	try
	{
		XTPADODB::_RecordsetPtr ptrDataRS = cmdGetAllEvents_raw.Execute();
		if (ptrDataRS == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		// process recordset
		while (!ptrDataRS->bEOF) //
		{
			ptrEvent = _ReadEvent_common(ptrDataRS, FALSE);
			if (!ptrEvent)
			{
				ASSERT(FALSE);
				ptrDataRS->MoveNext();
				continue;
			}

			ptrEvents->Add(ptrEvent);

			// next record
			ptrDataRS->MoveNext();
		}

		ptrDataRS->Close();

		return ptrEvents;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::DoGetAllEvents_raw() \n"));
	return NULL;
}

CXTPCalendarEventsPtr CXTPCalendarDatabaseDataProvider::DoGetUpcomingEvents(
								COleDateTime dtFrom, COleDateTimeSpan spPeriod)
{
	USES_CONVERSION;

	if (!IsOpen())
	{
		return NULL;
	}

	//-----------------------------------------------------------------------
	if ((double)spPeriod == 0)
	{
		return DoGetAllEvents_raw();
	}

	//-----------------------------------------------------------------------
	CXTPCalendarEventsPtr ptrEvents = new CXTPCalendarEvents();
	if (!ptrEvents)
	{
		return NULL;
	}

	COleDateTime dtTo = dtFrom + spPeriod;

	CString strDateTo = MakeSQLDateTime(dtTo);

	CString strSqlText;
	strSqlText.Format(_T("SELECT * FROM Event \
		WHERE \
		(RecurrenceState = $normal$ OR RecurrenceState = $master$ ) AND \
			( IsReminder <> 0 OR \
				( RecurrenceState = $master$ AND \
				  RecurrencePatternID IN \
					( SELECT RecurrencePatternID  FROM Event \
						WHERE \
						  RecurrenceState = $exception$ And \
						  IsReminder <> 0 AND \
						  DATEADD(\"n\", -1 * ReminderMinutesBeforeStart, StartDateTime) <= %s \
					) \
				) \
			) AND \
		DATEADD(\"n\", -1 * ReminderMinutesBeforeStart, StartDateTime) <= %s \
		"),
		(LPCTSTR)strDateTo, (LPCTSTR)strDateTo);

	TCHAR szBuff[64];
	REPLACE_S(strSqlText, _T("$normal$"), ITOT_S((int)xtpCalendarRecurrenceNotRecurring, szBuff, 64, 10));
	REPLACE_S(strSqlText, _T("$master$"), ITOT_S((int)xtpCalendarRecurrenceMaster, szBuff, 64, 10));
	REPLACE_S(strSqlText, _T("$exception$"), ITOT_S((int)xtpCalendarRecurrenceException, szBuff, 64, 10));

	CADOCommand cmdGetUpcomingEvents(m_pconnDb.GetInterfacePtr(), CT2CW(strSqlText));

	CXTPCalendarEventPtr  ptrEvent;

	try
	{
		XTPADODB::_RecordsetPtr ptrDataRS = cmdGetUpcomingEvents.Execute();
		if (ptrDataRS == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		// process recordset
		while (!ptrDataRS->bEOF) //
		{
			ptrEvent = _ReadEvent_common(ptrDataRS, FALSE);
			if (!ptrEvent)
			{
				ASSERT(FALSE);
				ptrDataRS->MoveNext();
				continue;
			}

			ptrEvents->Add(ptrEvent);

			// next record
			ptrDataRS->MoveNext();
		}

		ptrDataRS->Close();

		return ptrEvents;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::DoGetAllEvents_raw() \n"));
	return NULL;

}


BOOL CXTPCalendarDatabaseDataProvider::DoCreate_Event(CXTPCalendarEvent* pEvent, DWORD& rdwNewEventID)
{
	if (!IsOpen())
	{
		return FALSE;
	}

	if (!pEvent || !m_pcmdAddEvent || !m_pcmdUpdEventPatternID)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	try
	{
		COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
		pEvent->SetLastModificationTime(dtNow);

		int nRState = (long)pEvent->GetRecurrenceState();

		ASSERT(nRState == xtpCalendarRecurrenceMaster || nRState == xtpCalendarRecurrenceNotRecurring ||
			   nRState == xtpCalendarRecurrenceException);

		m_pcmdAddEvent->SetParameterValue(L"prm_StartDateTime", DATE2VAR(pEvent->GetStartTime()));
		m_pcmdAddEvent->SetParameterValue(L"prm_EndDateTime", DATE2VAR(pEvent->GetEndTime()));
		m_pcmdAddEvent->SetParameterValue(L"prm_RecurrenceState", _variant_t((long)nRState));

		_variant_t vtIsAllDayEvent = (long)(pEvent->IsAllDayEvent() ? 1 : 0);

		m_pcmdAddEvent->SetParameterValue(L"prm_IsAllDayEvent", vtIsAllDayEvent);
		CString strData = pEvent->GetSubject();
		m_pcmdAddEvent->SetParameterValue(L"prm_Subject", STRING2VAR(strData));
		strData = pEvent->GetLocation();
		m_pcmdAddEvent->SetParameterValue(L"prm_Location", STRING2VAR(strData));
		strData = pEvent->GetReminderSoundFile();
		m_pcmdAddEvent->SetParameterValue(L"prm_RemainderSoundFile", STRING2VAR(strData));
		strData = pEvent->GetBody();
		m_pcmdAddEvent->SetParameterValue(L"prm_Body", STRING2VAR(strData));
		m_pcmdAddEvent->SetParameterValue(L"prm_Created", DATE2VAR(pEvent->GetCreationTime()));
		m_pcmdAddEvent->SetParameterValue(L"prm_Modified", DATE2VAR(pEvent->GetLastModificationTime()));
		m_pcmdAddEvent->SetParameterValue(L"prm_BusyStatus", _variant_t((long)pEvent->GetBusyStatus()));
		m_pcmdAddEvent->SetParameterValue(L"prm_ImportanceLevel", _variant_t((long)pEvent->GetImportance()));
		m_pcmdAddEvent->SetParameterValue(L"prm_LabelID", _variant_t((long)pEvent->GetLabelID()));
		m_pcmdAddEvent->SetParameterValue(L"prm_RecurrencePatternID", _variant_t((long)pEvent->GetRecurrencePatternID()));
		m_pcmdAddEvent->SetParameterValue(L"prm_ScheduleID", _variant_t((long)pEvent->GetScheduleID()));

		_variant_t vtIsRecurrenceExceptionDeleted = (long)(pEvent->IsRExceptionDeleted() ? 1 : 0);

		m_pcmdAddEvent->SetParameterValue(L"prm_IsRecurrenceExceptionDeleted", vtIsRecurrenceExceptionDeleted);
		m_pcmdAddEvent->SetParameterValue(L"prm_RExceptionStartTimeOrig", DATE2VAR(pEvent->GetRException_StartTimeOrig()) );
		m_pcmdAddEvent->SetParameterValue(L"prm_RExceptionEndTimeOrig", DATE2VAR(pEvent->GetRException_EndTimeOrig()) );

		_variant_t vtIsMeeting = (long)(pEvent->IsMeeting() ? 1 : 0);
		m_pcmdAddEvent->SetParameterValue(L"prm_IsMeeting", vtIsMeeting);

		_variant_t vtIsPrivate = (long)(pEvent->IsPrivate() ? 1 : 0);
		m_pcmdAddEvent->SetParameterValue(L"prm_IsPrivate", vtIsPrivate);

		_variant_t vtIsReminder = (long)(pEvent->IsReminder() ? 1 : 0);
		m_pcmdAddEvent->SetParameterValue(L"prm_IsReminder", vtIsReminder);
		m_pcmdAddEvent->SetParameterValue(L"prm_ReminderMinutesBeforeStart", _variant_t((long)pEvent->GetReminderMinutesBeforeStart()));

		if (pEvent->GetCustomProperties())
		{
			pEvent->GetCustomProperties()->RemoveProperty(cszEventCustProp_IconsIDs);

			if (pEvent->GetCustomIcons() && pEvent->GetCustomIcons()->GetSize())
			{
				CString strArray = pEvent->GetCustomIcons()->SaveToString();
				VERIFY(pEvent->GetCustomProperties()->SetProperty(cszEventCustProp_IconsIDs, STRING2VAR(strArray)));
			}

			pEvent->GetCustomProperties()->SaveToXML(strData);

			pEvent->GetCustomProperties()->RemoveProperty(cszEventCustProp_IconsIDs);
		}

		m_pcmdAddEvent->SetParameterValue(L"prm_CustomPropertiesXMLData", STRING2VAR(strData));

		m_pcmdAddEvent->Execute();

		// get event ID
		m_pcmdGetLastID->SetSQLString(L"SELECT MAX(EventID) AS NewEventID FROM Event");
		XTPADODB::_RecordsetPtr ptrDataRS = m_pcmdGetLastID->Execute();

		ASSERT(ptrDataRS != NULL);
		if (ptrDataRS)
		{
			ASSERT(!ptrDataRS->bEOF);
			if (!ptrDataRS->bEOF)
			{
				rdwNewEventID = (DWORD)(long)ptrDataRS->Fields->GetItem(L"NewEventID")->Value;
			}
		}
		ptrDataRS->Close();

		return TRUE;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::_AddEvent()"));
	return FALSE;
}

BOOL CXTPCalendarDatabaseDataProvider::DoDelete_Event(CXTPCalendarEvent* pEvent)
{
	if (!IsOpen())
	{
		return FALSE;
	}

	if (!m_pcmdDelEvent || !pEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	DWORD dwEventID = pEvent->GetEventID();
	ASSERT(dwEventID != XTP_CALENDAR_UNKNOWN_EVENT_ID);

	m_pcmdDelEvent->SetParameterValue(L"EventID", _variant_t((long)dwEventID));
	m_pcmdDelEvent->Execute();

	return TRUE;
}

BOOL CXTPCalendarDatabaseDataProvider::DoUpdate_Event(CXTPCalendarEvent* pEvent)
{
	if (!IsOpen())
	{
		return FALSE;
	}

	if (!pEvent || !m_pcmdUpdEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
	pEvent->SetLastModificationTime(dtNow);

	DWORD dwEventID = pEvent->GetEventID();
	ASSERT(dwEventID != XTP_CALENDAR_UNKNOWN_EVENT_ID);

	//===========================================================================
	try
	{
		// Update Event properties
		m_pcmdUpdEvent->SetParameterValue(L"prm_StartDateTime", DATE2VAR(pEvent->GetStartTime()));
		m_pcmdUpdEvent->SetParameterValue(L"prm_EndDateTime", DATE2VAR(pEvent->GetEndTime()));
		m_pcmdUpdEvent->SetParameterValue(L"prm_RecurrenceState", _variant_t((long)pEvent->GetRecurrenceState()));

		_variant_t vtIsAllDayEvent((long)0);
		if (pEvent->IsAllDayEvent())
			vtIsAllDayEvent = _variant_t((long)1);
		m_pcmdUpdEvent->SetParameterValue(L"prm_IsAllDayEvent", vtIsAllDayEvent);
		CString strData = pEvent->GetSubject();
		m_pcmdUpdEvent->SetParameterValue(L"prm_Subject", STRING2VAR(strData));
		strData = pEvent->GetLocation();
		m_pcmdUpdEvent->SetParameterValue(L"prm_Location", STRING2VAR(strData));
		strData = pEvent->GetReminderSoundFile();
		m_pcmdUpdEvent->SetParameterValue(L"prm_RemainderSoundFile", STRING2VAR(strData));
		strData = pEvent->GetBody();
		m_pcmdUpdEvent->SetParameterValue(L"prm_Body", STRING2VAR(strData));
		m_pcmdUpdEvent->SetParameterValue(L"prm_Created", DATE2VAR(pEvent->GetCreationTime()));
		m_pcmdUpdEvent->SetParameterValue(L"prm_Modified", DATE2VAR(pEvent->GetLastModificationTime()));
		m_pcmdUpdEvent->SetParameterValue(L"prm_BusyStatus", _variant_t((long)pEvent->GetBusyStatus()));
		m_pcmdUpdEvent->SetParameterValue(L"prm_ImportanceLevel", _variant_t((long)pEvent->GetImportance()));
		m_pcmdUpdEvent->SetParameterValue(L"prm_LabelID", _variant_t((long)pEvent->GetLabelID()));
		m_pcmdUpdEvent->SetParameterValue(L"prm_RecurrencePatternID", _variant_t((long)pEvent->GetRecurrencePatternID()));
		m_pcmdUpdEvent->SetParameterValue(L"prm_ScheduleID", _variant_t((long)pEvent->GetScheduleID()));

		_variant_t vtIsRecurrenceExceptionDeleted = (long)(pEvent->IsRExceptionDeleted() ? 1 : 0);

		m_pcmdUpdEvent->SetParameterValue(L"prm_IsRecurrenceExceptionDeleted", vtIsRecurrenceExceptionDeleted);
		m_pcmdUpdEvent->SetParameterValue(L"prm_RExceptionStartTimeOrig", DATE2VAR(pEvent->GetRException_StartTimeOrig()) );
		m_pcmdUpdEvent->SetParameterValue(L"prm_RExceptionEndTimeOrig", DATE2VAR(pEvent->GetRException_EndTimeOrig()) );

		_variant_t vtIsMeeting = (long)(pEvent->IsMeeting() ? 1 : 0);
		m_pcmdUpdEvent->SetParameterValue(L"prm_IsMeeting", vtIsMeeting);

		_variant_t vtIsPrivate = (long)(pEvent->IsPrivate() ? 1 : 0);
		m_pcmdUpdEvent->SetParameterValue(L"prm_IsPrivate", vtIsPrivate);

		_variant_t vtIsReminder = (long)(pEvent->IsReminder() ? 1 : 0);
		m_pcmdUpdEvent->SetParameterValue(L"prm_IsReminder", vtIsReminder);
		m_pcmdUpdEvent->SetParameterValue(L"prm_ReminderMinutesBeforeStart", _variant_t((long)pEvent->GetReminderMinutesBeforeStart()));

		if (pEvent->GetCustomProperties())
		{
			pEvent->GetCustomProperties()->RemoveProperty(cszEventCustProp_IconsIDs);

			if (pEvent->GetCustomIcons() && pEvent->GetCustomIcons()->GetSize())
			{
				CString strArray = pEvent->GetCustomIcons()->SaveToString();
				VERIFY(pEvent->GetCustomProperties()->SetProperty(cszEventCustProp_IconsIDs, STRING2VAR(strArray)));

			}

			pEvent->GetCustomProperties()->SaveToXML(strData);

			pEvent->GetCustomProperties()->RemoveProperty(cszEventCustProp_IconsIDs);
		}
		m_pcmdUpdEvent->SetParameterValue(L"prm_CustomPropertiesXMLData", STRING2VAR(strData));

		//"EventID"
		m_pcmdUpdEvent->SetParameterValue(L"prm_EventID", _variant_t((long)dwEventID));

		m_pcmdUpdEvent->Execute();

		return TRUE;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::UpdateEvent(ID = %d)"), dwEventID);
	return FALSE;
}

// ------------------- RecurrencePattern processing -----------------------------
CXTPCalendarRecurrencePatternPtr CXTPCalendarDatabaseDataProvider::DoRead_RPattern(DWORD dwPatternID)
{
	if (!IsOpen())
	{
		return NULL;
	}

	if (!m_pcmdGetRPattern)
	{
		ASSERT(FALSE);
		return NULL;
	}

	try
	{
		m_pcmdGetRPattern->SetParameterValue(L"RecurrencePatternID", _variant_t((long)dwPatternID));
		XTPADODB::_RecordsetPtr ptrDataRS = m_pcmdGetRPattern->Execute();

		// process recordset
		if (ptrDataRS == NULL || ptrDataRS->bEOF)
		{
			return NULL;
		}

		//ChangePatternID(pPattern, dwPatternID);
		CXTPCalendarRecurrencePatternPtr ptrPattern = CreateNewRecurrencePattern(dwPatternID);

		ptrPattern->SetMasterEventID((DWORD)(long)ptrDataRS->Fields->GetItem(L"MasterEventID")->Value);

		ptrPattern->SetStartTime((DATE)ptrDataRS->Fields->GetItem(L"StartDateTime")->Value);
		ptrPattern->SetDurationMinutes((long)ptrDataRS->Fields->GetItem(L"Duration")->Value);

		ptrPattern->SetPatternStartDate((DATE)ptrDataRS->Fields->GetItem(L"PatternStartDateTime")->Value);

		int nUseEnd = _GetField<long>(ptrDataRS, L"UseEnd", xtpCalendarPatternEndNoDate);
		if (nUseEnd == xtpCalendarPatternEndDate)
		{
			ptrPattern->SetPatternEndDate(_GetField<DATE>(ptrDataRS, L"PatternEndDateTime", 0));
		}
		else if (nUseEnd == xtpCalendarPatternEndAfterOccurrences)
		{
			ptrPattern->SetEndAfterOccurrences(_GetField<long>(ptrDataRS, L"EndAfterOccurrence", 1));
		}
		else if (nUseEnd == xtpCalendarPatternEndNoDate)
		{
			ptrPattern->SetNoEndDate();
		}

		XTP_CALENDAR_RECURRENCE_OPTIONS tmp_options;
		if (!_GetRPatternOptions(ptrDataRS, tmp_options))
		{
			ASSERT(FALSE);
			ptrDataRS->Close();
			return NULL;
		}
		ptrPattern->SetRecurrenceOptions(tmp_options);

		if (ptrPattern->GetCustomProperties())
		{
			CString bstrPropsXMLData = _GetFieldString(ptrDataRS, L"CustomPropertiesXMLData");
			ptrPattern->GetCustomProperties()->LoadFromXML(bstrPropsXMLData);
		}

		ptrDataRS->Close();

		if (!_ReadRExceptions(ptrPattern))
		{
			ASSERT(FALSE);
			return NULL;
		}

		return ptrPattern;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::ReadRPattern(ID = %d)"), dwPatternID);
	return FALSE;
}

BOOL CXTPCalendarDatabaseDataProvider::_ReadRExceptions(CXTPCalendarRecurrencePattern* pPattern)
{
	if (!IsOpen())
	{
		return FALSE;
	}

	if (!pPattern || !m_pcmdGetRExceptions)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	DWORD dwPatternID = pPattern->GetPatternID();
	try
	{
		m_pcmdGetRExceptions->SetParameterValue(L"PatternID", (long)dwPatternID);

		XTPADODB::_RecordsetPtr ptrDataRS = m_pcmdGetRExceptions->Execute();
		if (ptrDataRS == NULL)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		// process recordset
		while (!ptrDataRS->bEOF) //
		{
			CXTPCalendarEventPtr  ptrEvent = _ReadEvent_common(ptrDataRS, TRUE);
			if (!ptrEvent)
			{
				#ifdef _DEBUG
					long nID = _GetField<long>(ptrDataRS, L"EventID", 0);
					TRACE(_T("! Cannot Read Exception event. ID = %d"), nID);
				#endif

				ptrDataRS->MoveNext();
				continue;
			}
			int nRState = _GetField<long>(ptrDataRS, L"RecurrenceState", xtpCalendarRecurrenceUnknown);
			ASSERT(nRState == xtpCalendarRecurrenceException);

			if (nRState == xtpCalendarRecurrenceException)
			{
				if (!ptrEvent->MakeAsRException(dwPatternID))
				{
					ASSERT(FALSE);
					ptrDataRS->MoveNext();
					continue;
				}

				ptrEvent->SetRExceptionDeleted(0 != _GetField<long>(ptrDataRS, L"IsRecurrenceExceptionDeleted", 1));

				DATE dtStartOrig = _GetField<DATE>(ptrDataRS, L"RExceptionStartTimeOrig", INT_MAX);
				DATE dtEndOrig = _GetField<DATE>(ptrDataRS, L"RExceptionEndTimeOrig", INT_MAX);

				if (dtStartOrig != INT_MAX && dtEndOrig != INT_MAX)
				{
					ptrEvent->SetRException_StartTimeOrig(dtStartOrig);
					ptrEvent->SetRException_EndTimeOrig(dtEndOrig);

					VERIFY(pPattern->SetException(ptrEvent));
				}
				else
				{
					ASSERT(FALSE);
				}
			}

			// next record
			ptrDataRS->MoveNext();
		}

		ptrDataRS->Close();

		return TRUE;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::ReadRExceptions(PatternID = %d)"), dwPatternID);
	return FALSE;
}

BOOL CXTPCalendarDatabaseDataProvider::DoCreate_RPattern(CXTPCalendarRecurrencePattern* pPattern, DWORD& rdwNewPatternID)
{
	if (!IsOpen())
	{
		return FALSE;
	}

	if (!pPattern || !m_pcmdAddRPattern)
	{
		return FALSE;
	}

	try
	{
		m_pcmdAddRPattern->SetParameterValue(L"prm_MasterEventID", _variant_t((long)pPattern->GetMasterEventID()));

		m_pcmdAddRPattern->SetParameterValue(L"prm_PatternStartDateTime", DATE2VAR(pPattern->GetPatternStartDate()));

		m_pcmdAddRPattern->SetParameterValue(L"prm_StartDateTime", DATE2VAR(pPattern->GetStartTime()));
		m_pcmdAddRPattern->SetParameterValue(L"prm_Duration", _variant_t((long)pPattern->GetDurationMinutes()));

		int nUseEnd = pPattern->GetUseEndMethod();

		m_pcmdAddRPattern->SetParameterValue(L"prm_UseEnd", _variant_t((long)nUseEnd));
		m_pcmdAddRPattern->SetParameterValue(L"prm_PatternEndDateTime", DATE2VAR((nUseEnd == xtpCalendarPatternEndDate) ? (DATE)pPattern->GetPatternEndDate() : (DATE)0));
		m_pcmdAddRPattern->SetParameterValue(L"prm_EndAfterOccurrences", (long)pPattern->GetEndAfterOccurrences());

		_SetRPatternOptions(m_pcmdAddRPattern, pPattern);

		CString strData;
		if (pPattern->GetCustomProperties())
			pPattern->GetCustomProperties()->SaveToXML(strData);
		m_pcmdAddRPattern->SetParameterValue(L"prm_CustomPropertiesXMLData", STRING2VAR(strData));

		m_pcmdAddRPattern->Execute();

		// get event ID
		m_pcmdGetLastID->SetSQLString(L"SELECT MAX(RecurrencePatternID) AS NewRecurrencePatternID FROM RecurrencePattern");
		XTPADODB::_RecordsetPtr ptrDataRS = m_pcmdGetLastID->Execute();

		ASSERT(ptrDataRS != NULL);
		if (ptrDataRS)
		{
			ASSERT(!ptrDataRS->bEOF);
			if (!ptrDataRS->bEOF)
			{
				rdwNewPatternID = (DWORD)(long)ptrDataRS->Fields->GetItem(L"NewRecurrencePatternID")->Value;
			}
			ptrDataRS->Close();
		}

		return TRUE;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::AddRPattern()"));
	return FALSE;
}

BOOL CXTPCalendarDatabaseDataProvider::DoDelete_RPattern(CXTPCalendarRecurrencePattern* pPattern)
{
	if (!IsOpen())
	{
		return FALSE;
	}

	if (!m_pcmdDelRPattern || !pPattern)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	DWORD dwPatternID = pPattern->GetPatternID();

	m_pcmdDelRPattern->SetParameterValue(L"RecurrencePatternID", _variant_t((long)dwPatternID));
	m_pcmdDelRPattern->Execute();

	//-------------------------------------------
	m_pConnect->SendEvent(XTP_NC_CALENDARPATTERNWASDELETED, dwPatternID, 0);

	return TRUE;
}

BOOL CXTPCalendarDatabaseDataProvider::DoUpdate_RPattern(CXTPCalendarRecurrencePattern* pPattern)
{
	if (!IsOpen())
	{
		return FALSE;
	}

	if (!pPattern || !m_pcmdUpdRPattern)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	DWORD dwPatternID = pPattern->GetPatternID();

	//-------------------------------------
	m_pcmdUpdRPattern->SetParameterValue(L"prm_MasterEventID", _variant_t((long)pPattern->GetMasterEventID()));

	m_pcmdUpdRPattern->SetParameterValue(L"prm_StartDateTime", DATE2VAR(pPattern->GetStartTime()));
	m_pcmdUpdRPattern->SetParameterValue(L"prm_Duration", DATE2VAR(pPattern->GetDurationMinutes()));

	m_pcmdUpdRPattern->SetParameterValue(L"prm_PatternStartDateTime", DATE2VAR(pPattern->GetPatternStartDate()));

	int nUseEnd = pPattern->GetUseEndMethod();

	m_pcmdUpdRPattern->SetParameterValue(L"prm_UseEnd", _variant_t((long)nUseEnd));
	m_pcmdUpdRPattern->SetParameterValue(L"prm_PatternEndDateTime", DATE2VAR((nUseEnd == xtpCalendarPatternEndDate) ? (DATE)pPattern->GetPatternEndDate() : (DATE)0));
	m_pcmdUpdRPattern->SetParameterValue(L"prm_EndAfterOccurrences", (long)pPattern->GetEndAfterOccurrences());

	if (!_SetRPatternOptions(m_pcmdUpdRPattern, pPattern))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CString strData;
	if (pPattern->GetCustomProperties())
		pPattern->GetCustomProperties()->SaveToXML(strData);

	m_pcmdUpdRPattern->SetParameterValue(L"prm_CustomPropertiesXMLData", STRING2VAR(strData));

	m_pcmdUpdRPattern->SetParameterValue(L"prm_RecurrencePatternID", _variant_t((long)dwPatternID)); //

	m_pcmdUpdRPattern->Execute();
	//-------------------------------------

	return TRUE;
}

BOOL CXTPCalendarDatabaseDataProvider::_GetRPatternOptions(XTPADODB::_Recordset* pRS, XTP_CALENDAR_RECURRENCE_OPTIONS& rROptions)
{
	try
	{
		rROptions.m_nRecurrenceType = (XTPCalendarEventRecurrenceType)(long)pRS->Fields->GetItem(L"RecurrenceTypeID")->Value;

		switch (rROptions.m_nRecurrenceType)
		{
		case xtpCalendarRecurrenceDaily :  // = 1
			rROptions.m_Daily.bEveryWeekDayOnly = (long)pRS->Fields->GetItem(L"IsEveryWeekDay")->Value > 0;
			if (rROptions.m_Daily.bEveryWeekDayOnly)
			{
				rROptions.m_Daily.nIntervalDays = 1;
			}
			else
			{
				rROptions.m_Daily.nIntervalDays = (long)pRS->Fields->GetItem(L"OccurrenceInterval")->Value;
			}
			break;
		case xtpCalendarRecurrenceWeekly : //   = 2,
			rROptions.m_Weekly.nIntervalWeeks = (long)pRS->Fields->GetItem(L"OccurrenceInterval")->Value;
			rROptions.m_Weekly.nDayOfWeekMask = (long)pRS->Fields->GetItem(L"DayOfWeekMask")->Value;
			break;
		case xtpCalendarRecurrenceMonthly : //      = 3,
			rROptions.m_Monthly.nIntervalMonths = (long)pRS->Fields->GetItem(L"OccurrenceInterval")->Value;
			rROptions.m_Monthly.nDayOfMonth = (long)pRS->Fields->GetItem(L"DayOfMonth")->Value;
			break;
		case xtpCalendarRecurrenceMonthNth : // = 4,
			rROptions.m_MonthNth.nIntervalMonths =  (long)pRS->Fields->GetItem(L"OccurrenceInterval")->Value;
			rROptions.m_MonthNth.nWhichDay = (long)pRS->Fields->GetItem(L"DayOccurrenceTypeID")->Value;
			rROptions.m_MonthNth.nWhichDayMask = (long)pRS->Fields->GetItem(L"DayMask")->Value;
			break;
		case xtpCalendarRecurrenceYearly : //   = 5,
			rROptions.m_Yearly.nMonthOfYear = (long)pRS->Fields->GetItem(L"MonthOfYear")->Value;
			rROptions.m_Yearly.nDayOfMonth = (long)pRS->Fields->GetItem(L"DayOfMonth")->Value;
			break;
		case xtpCalendarRecurrenceYearNth : //      = 6,
			rROptions.m_YearNth.nWhichDay = (long)pRS->Fields->GetItem(L"DayOccurrenceTypeID")->Value;
			rROptions.m_YearNth.nWhichDayMask = (long)pRS->Fields->GetItem(L"DayMask")->Value;
			rROptions.m_YearNth.nMonthOfYear = (long)pRS->Fields->GetItem(L"MonthOfYear")->Value;
			break;
		default:
			ASSERT(FALSE);
			return FALSE;
		}
		return TRUE;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::GetRPatternOptions()"));
	return FALSE;
}

BOOL CXTPCalendarDatabaseDataProvider::_SetRPatternOptions(CXTPCalendarDatabaseDataProvider::CADOCommand* pCmd, CXTPCalendarRecurrencePattern* pPattern)
{
	try
	{
		int nRecurrenceTypeID = pPattern->GetRecurrenceType();
		pCmd->SetParameterValue(L"prm_RecurrenceTypeID", _variant_t((long)nRecurrenceTypeID));

		XTP_CALENDAR_RECURRENCE_OPTIONS pROptions = pPattern->GetRecurrenceOptions();

		switch (nRecurrenceTypeID)
		{
		case xtpCalendarRecurrenceDaily :  // = 1
			if (pROptions.m_Daily.bEveryWeekDayOnly)
			{
				pCmd->SetParameterValue(L"prm_IsEveryWeekDay", _variant_t((long)1));
				pCmd->SetParameterValue(L"prm_OccurrenceInterval", _variant_t((long)0));
			}
			else
			{
				pCmd->SetParameterValue(L"prm_IsEveryWeekDay", _variant_t((long)0));
				pCmd->SetParameterValue(L"prm_OccurrenceInterval", _variant_t((long)pROptions.m_Daily.nIntervalDays));
			}
			break;
		case xtpCalendarRecurrenceWeekly : //   = 2,
			pCmd->SetParameterValue(L"prm_OccurrenceInterval", _variant_t((long)pROptions.m_Weekly.nIntervalWeeks));
			pCmd->SetParameterValue(L"prm_DayOfWeekMask", _variant_t((long)pROptions.m_Weekly.nDayOfWeekMask));
			break;
		case xtpCalendarRecurrenceMonthly : //      = 3,
			pCmd->SetParameterValue(L"prm_OccurrenceInterval", _variant_t((long)pROptions.m_Monthly.nIntervalMonths));
			pCmd->SetParameterValue(L"prm_DayOfMonth", _variant_t((long)pROptions.m_Monthly.nDayOfMonth));
			break;
		case xtpCalendarRecurrenceMonthNth : // = 4,
			pCmd->SetParameterValue(L"prm_OccurrenceInterval", _variant_t((long)pROptions.m_MonthNth.nIntervalMonths));
			pCmd->SetParameterValue(L"prm_DayOccurrenceType", _variant_t((long)pROptions.m_MonthNth.nWhichDay));
			pCmd->SetParameterValue(L"prm_DayMask", _variant_t((long)pROptions.m_MonthNth.nWhichDayMask));
			break;
		case xtpCalendarRecurrenceYearly : //   = 5,
			pCmd->SetParameterValue(L"prm_MonthOfYear", _variant_t((long)pROptions.m_Yearly.nMonthOfYear));
			pCmd->SetParameterValue(L"prm_DayOfMonth", _variant_t((long)pROptions.m_Yearly.nDayOfMonth));
			break;
		case xtpCalendarRecurrenceYearNth : //      = 6,
			pCmd->SetParameterValue(L"prm_DayOccurrenceType", _variant_t((long)pROptions.m_YearNth.nWhichDay));
			pCmd->SetParameterValue(L"prm_DayMask",  _variant_t((long)pROptions.m_YearNth.nWhichDayMask));
			pCmd->SetParameterValue(L"prm_MonthOfYear", _variant_t((long)pROptions.m_YearNth.nMonthOfYear));
			break;
		default:
			ASSERT(FALSE);
			return FALSE;
		}
		return TRUE;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::SetRPatternOptions()"));
	return FALSE;
}

BOOL CXTPCalendarDatabaseDataProvider::SaveOptions(const CString& strOptionsData)
{
	if (!IsOpen())
		return FALSE;

	if (!m_pcmdAddOptions || !m_pcmdRemoveAllOptions)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	try
	{
		// Remove old options records if necessary
		if (!m_bTraceOptions)
		{
			m_pcmdRemoveAllOptions->Execute();
		}

		// Add new options record
		COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();

		m_pcmdAddOptions->SetParameterValue(L"prm_SaveDateTime", DATE2VAR(dtNow));
		m_pcmdAddOptions->SetParameterValue(L"prm_XMLData", STRING2VAR(strOptionsData));

		m_pcmdAddOptions->Execute();

		return TRUE;
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{}

	TRACE(_T("ERROR: Exeption in CXTPCalendarDatabaseDataProvider::SaveOptions()"));
	return FALSE;
}

CXTPCalendarEventPtr CXTPCalendarDatabaseDataProvider::DoRead_Event(DWORD dwEventID)
{
	if (!IsOpen())
	{
		return NULL;
	}

	if (!m_pcmdGetEvent)
	{
		ASSERT(FALSE);
		return NULL;
	}

	m_pcmdGetEvent->SetParameterValue(L"EventID", (long)dwEventID);

	XTPADODB::_RecordsetPtr ptrDataRS = m_pcmdGetEvent->Execute();
	if (ptrDataRS == NULL || ptrDataRS->bEOF)
	{
		return NULL;
	}

	CXTPCalendarEventPtr ptrEvent = _ReadEvent_common(ptrDataRS, FALSE);
	if (!ptrEvent)
	{
		ptrDataRS->Close();
		return NULL;
	}

	ptrDataRS->Close();

	return ptrEvent;
}

void CXTPCalendarDatabaseDataProvider::DoRemoveAllEvents()
{
	if (!IsOpen())
	{
		return;
	}

	if (!m_pcmdRemoveAllEvents || !m_pcmdRemoveAllRPatterns)
	{
		ASSERT(FALSE);
		return;
	}

	m_pcmdRemoveAllEvents->Execute();
	m_pcmdRemoveAllRPatterns->Execute();
}

void CXTPCalendarDatabaseDataProvider::CreateGetLastIDCommand()
{
	m_pcmdGetLastID = new CADOCommand(m_pconnDb.GetInterfacePtr(), L"");
}

void CXTPCalendarDatabaseDataProvider::CreateAddEventCommand()
{
	LPCWSTR strSqlText =
		//---------------- sql statement body -------------------------
		L"PARAMETERS \
				prm_StartDateTime   DateTime, \
				prm_EndDateTime     DateTime, \
				prm_RecurrenceState Integer, \
				prm_IsAllDayEvent   Integer, \
				prm_Subject             CHARACTER, \
				prm_Location            CHARACTER, \
				prm_RemainderSoundFile  CHARACTER,\
				prm_Body            CHARACTER, \
				prm_Created     DateTime, \
				prm_Modified    DateTime, \
				prm_BusyStatus      Integer, \
				prm_ImportanceLevel Integer, \
				prm_LabelID         Integer, \
				prm_RecurrencePatternID             Integer, \
				prm_ScheduleID                      Integer, \
				prm_IsRecurrenceExceptionDeleted    Integer, \
				prm_RExceptionStartTimeOrig         DateTime, \
				prm_RExceptionEndTimeOrig           DateTime, \
				prm_IsMeeting   Integer, \
				prm_IsPrivate   Integer, \
				prm_IsReminder  Integer, \
				prm_ReminderMinutesBeforeStart      Integer, \
				prm_CustomPropertiesXMLData         TEXT; "
	L" \
\
INSERT INTO Event ( \
\
StartDateTime, \
EndDateTime, \
RecurrenceState, \
IsAllDayEvent, \
Subject, \
Location, \
RemainderSoundFile,\
Body, \
Created, \
Modified, \
BusyStatus, \
ImportanceLevel, \
LabelID, \
RecurrencePatternID, \
ScheduleID, \
IsRecurrenceExceptionDeleted, \
RExceptionStartTimeOrig, \
RExceptionEndTimeOrig, \
IsMeeting, \
IsPrivate, \
IsReminder, \
ReminderMinutesBeforeStart, \
CustomPropertiesXMLData\
) \
VALUES ( \
	prm_StartDateTime, \
	prm_EndDateTime, \
	prm_RecurrenceState, \
	prm_IsAllDayEvent, \
	prm_Subject, \
	prm_Location, \
	prm_RemainderSoundFile,\
	prm_Body, \
	prm_Created, \
	prm_Modified, \
	prm_BusyStatus, \
	prm_ImportanceLevel, \
	prm_LabelID, \
	prm_RecurrencePatternID, \
	prm_ScheduleID, \
	prm_IsRecurrenceExceptionDeleted, \
	prm_RExceptionStartTimeOrig, \
	prm_RExceptionEndTimeOrig, \
	prm_IsMeeting, \
	prm_IsPrivate, \
	prm_IsReminder, \
	prm_ReminderMinutesBeforeStart, \
	prm_CustomPropertiesXMLData\
) \
";
	//---------------- sql statement body -------------------------

	m_pcmdAddEvent = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);

	_variant_t vtEmpty((long)0);
	_variant_t vtEmptyStr(EMPTY_DB_STR);
	//m_pcmdAddEvent->CreateParameter(L"StartDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	//m_pcmdAddEvent->CreateParameter(L"EndDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);

	m_pcmdAddEvent->CreateParameter(L"prm_StartDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_EndDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);

	m_pcmdAddEvent->CreateParameter(L"prm_RecurrenceState", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_IsAllDayEvent", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_Subject", XTPADODB::adBSTR, XTPADODB::adParamInput, 255, vtEmptyStr);
	m_pcmdAddEvent->CreateParameter(L"prm_Location", XTPADODB::adBSTR, XTPADODB::adParamInput, 255, vtEmptyStr);
	m_pcmdAddEvent->CreateParameter(L"prm_RemainderSoundFile", XTPADODB::adBSTR, XTPADODB::adParamInput, 255, vtEmptyStr);
	m_pcmdAddEvent->CreateParameter(L"prm_Body", XTPADODB::adBSTR, XTPADODB::adParamInput, 255, vtEmptyStr);
	m_pcmdAddEvent->CreateParameter(L"prm_Created", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_Modified", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_BusyStatus", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_ImportanceLevel", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_LabelID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_RecurrencePatternID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_ScheduleID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_IsRecurrenceExceptionDeleted", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_RExceptionStartTimeOrig", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_RExceptionEndTimeOrig", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_IsMeeting", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_IsPrivate", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);

	m_pcmdAddEvent->CreateParameter(L"prm_IsReminder", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddEvent->CreateParameter(L"prm_ReminderMinutesBeforeStart", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);

	m_pcmdAddEvent->CreateParameter(L"prm_CustomPropertiesXMLData", XTPADODB::adBSTR, XTPADODB::adParamInput, 0, vtEmptyStr);
}

void CXTPCalendarDatabaseDataProvider::CreateDelEventCommand()
{
	LPCWSTR strSqlText =
		//---------------- sql statement body -------------------------
		L"DELETE FROM Event WHERE EventID = ?";
	//---------------- sql statement body -------------------------
	_variant_t vtEmpty((long)0);
	m_pcmdDelEvent = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);
	m_pcmdDelEvent->CreateParameter(L"EventID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);

}

void CXTPCalendarDatabaseDataProvider::CreateUpdEventCommand()
{
	LPCWSTR strSqlText =
		//---------------- sql statement body -------------------------
		L"PARAMETERS \
				prm_StartDateTime   DateTime, \
				prm_EndDateTime     DateTime, \
				prm_RecurrenceState Integer, \
				prm_IsAllDayEvent   Integer, \
				prm_Subject             CHARACTER, \
				prm_Location            CHARACTER, \
				prm_RemainderSoundFile  CHARACTER,\
				prm_Body            CHARACTER, \
				prm_Created     DateTime, \
				prm_Modified    DateTime, \
				prm_BusyStatus      Integer, \
				prm_ImportanceLevel Integer, \
				prm_LabelID         Integer, \
				prm_RecurrencePatternID             Integer, \
				prm_ScheduleID                      Integer, \
				prm_IsRecurrenceExceptionDeleted    Integer, \
				prm_RExceptionStartTimeOrig         DateTime, \
				prm_RExceptionEndTimeOrig           DateTime, \
				prm_IsMeeting   Integer, \
				prm_IsPrivate   Integer, \
				prm_IsReminder  Integer, \
				prm_ReminderMinutesBeforeStart      Integer, \
				prm_CustomPropertiesXMLData         TEXT,\
				prm_EventID     Integer; "

	L" \
UPDATE Event SET \
   StartDateTime   = prm_StartDateTime, \
   EndDateTime     = prm_EndDateTime, \
   RecurrenceState = prm_RecurrenceState, \
   IsAllDayEvent   = prm_IsAllDayEvent, \
   Subject         = prm_Subject, \
   Location            = prm_Location, \
   RemainderSoundFile = prm_RemainderSoundFile, \
   Body                = prm_Body, \
   Created         = prm_Created, \
   Modified            = prm_Modified, \
   BusyStatus      = prm_BusyStatus, \
   ImportanceLevel = prm_ImportanceLevel, \
   LabelID         = prm_LabelID, \
   RecurrencePatternID         = prm_RecurrencePatternID, \
   ScheduleID                  = prm_ScheduleID, \
   "
   L" \
   IsRecurrenceExceptionDeleted = prm_IsRecurrenceExceptionDeleted, \
   RExceptionStartTimeOrig     = prm_RExceptionStartTimeOrig, \
   RExceptionEndTimeOrig       = prm_RExceptionEndTimeOrig, \
   IsMeeting               = prm_IsMeeting, \
   IsPrivate               = prm_IsPrivate, \
   IsReminder              = prm_IsReminder, \
   ReminderMinutesBeforeStart = prm_ReminderMinutesBeforeStart, \
   CustomPropertiesXMLData = prm_CustomPropertiesXMLData \
   \
   WHERE EventID           = prm_EventID \
";
	//---------------- sql statement body -------------------------
	m_pcmdUpdEvent = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);
	_variant_t vtEmpty((long)0);
	_variant_t vtEmptyStr(EMPTY_DB_STR);

	m_pcmdUpdEvent->CreateParameter(L"prm_StartDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_EndDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_RecurrenceState", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_IsAllDayEvent", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_Subject", XTPADODB::adBSTR, XTPADODB::adParamInput, 255, vtEmptyStr);
	m_pcmdUpdEvent->CreateParameter(L"prm_Location", XTPADODB::adBSTR, XTPADODB::adParamInput, 255, vtEmptyStr);
	m_pcmdUpdEvent->CreateParameter(L"prm_RemainderSoundFile", XTPADODB::adBSTR, XTPADODB::adParamInput, 255, vtEmptyStr);
	m_pcmdUpdEvent->CreateParameter(L"prm_Body", XTPADODB::adBSTR, XTPADODB::adParamInput, 255, vtEmptyStr);
	m_pcmdUpdEvent->CreateParameter(L"prm_Created", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_Modified", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_BusyStatus", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_ImportanceLevel", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_LabelID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_RecurrencePatternID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_ScheduleID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_IsRecurrenceExceptionDeleted", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_RExceptionStartTimeOrig", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_RExceptionEndTimeOrig", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_IsMeeting", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_IsPrivate", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_IsReminder", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_ReminderMinutesBeforeStart", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEvent->CreateParameter(L"prm_CustomPropertiesXMLData", XTPADODB::adBSTR, XTPADODB::adParamInput, 0, vtEmptyStr);

	m_pcmdUpdEvent->CreateParameter(L"prm_EventID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
}

void CXTPCalendarDatabaseDataProvider::CreateUpdEventPatternIDCommand()
{
	LPCWSTR strSqlText;
	strSqlText =
		//---------------- sql statement body -------------------------
		L"UPDATE Event \
		   SET RecurrencePatternID = ? \
		   WHERE EventID = ? \
		   ";
	//---------------- sql statement body -------------------------
	m_pcmdUpdEventPatternID = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);
	_variant_t vtEmpty((long)0);

	m_pcmdUpdEventPatternID->CreateParameter(L"RecurrencePatternID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdEventPatternID->CreateParameter(L"EventID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
}

void CXTPCalendarDatabaseDataProvider::CreateGetDayEventCommand()
{
	USES_CONVERSION;

	CString strSqlText;
	strSqlText.Format(
		//---------------- sql statement body -------------------------
		_T("PARAMETERS prm_DayDate DateTime; \
		SELECT EventID, \
		   StartDateTime, \
		   EndDateTime, \
		   RecurrenceState, \
		   IsAllDayEvent, \
		   Subject, \
		   Location, \
		   RemainderSoundFile,\
		   Body, \
		   Created, \
		   Modified, \
		   BusyStatus, \
		   ImportanceLevel, \
		   LabelID, \
		   RecurrencePatternID, \
		   ScheduleID, \
		   IsRecurrenceExceptionDeleted, \
		   IsMeeting, \
		   IsPrivate, \
		   IsReminder, \
		   ReminderMinutesBeforeStart, \
		   CustomPropertiesXMLData \
		   FROM Event \
		   WHERE DateValue(StartDateTime) <= DateValue(prm_DayDate) \
		   AND DateValue(prm_DayDate) <= DateValue(EndDateTime) \
		   AND (RecurrenceState = %d OR RecurrenceState = %d) \
		   "),
		xtpCalendarRecurrenceNotRecurring, xtpCalendarRecurrenceMaster);
	//---------------- sql statement body -------------------------

	m_pcmdGetDayEvents = new CADOCommand(m_pconnDb.GetInterfacePtr(), CT2CW(strSqlText));
	_variant_t vtEmpty((long)0);

	m_pcmdGetDayEvents->CreateParameter(L"prm_DayDate", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
//  m_pcmdGetDayEvents->CreateParameter(L"DayDate2", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
}


void CXTPCalendarDatabaseDataProvider::CreateGetEventCommand()
{
	LPCWSTR strSqlText =
		//---------------- sql statement body -------------------------
		L"SELECT EventID, \
		   StartDateTime, \
		   EndDateTime, \
		   RecurrenceState, \
		   IsAllDayEvent, \
		   Subject, \
		   Location, \
		   RemainderSoundFile,\
		   Body, \
		   Created, \
		   Modified, \
		   BusyStatus, \
		   ImportanceLevel, \
		   LabelID, \
		   RecurrencePatternID, \
		   ScheduleID, \
		   IsRecurrenceExceptionDeleted, \
		   IsMeeting, \
		   IsPrivate, \
		   IsReminder, \
		   ReminderMinutesBeforeStart, \
		   CustomPropertiesXMLData \
		   FROM Event \
		   WHERE EventID = ? \
		   ";
	//---------------- sql statement body -------------------------
	_variant_t vtEmpty((long)0);

	m_pcmdGetEvent = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);

	m_pcmdGetEvent->CreateParameter(L"EventID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
}

void CXTPCalendarDatabaseDataProvider::CreateGetRExceptionsCommand()
{
	USES_CONVERSION;

	CString strSqlText;
	strSqlText.Format(
		//---------------- sql statement body -------------------------
		_T("SELECT EventID, \
		   StartDateTime, \
		   EndDateTime, \
		   RecurrenceState, \
		   IsAllDayEvent, \
		   Subject, \
		   Location, \
		   RemainderSoundFile,\
		   Body, \
		   Created, \
		   Modified, \
		   BusyStatus, \
		   ImportanceLevel, \
		   LabelID, \
		   RecurrencePatternID, \
		   ScheduleID, \
		   IsRecurrenceExceptionDeleted, \
		   RExceptionStartTimeOrig, \
		   RExceptionEndTimeOrig, \
		   IsMeeting, \
		   IsPrivate, \
		   IsReminder, \
		   ReminderMinutesBeforeStart, \
		   CustomPropertiesXMLData \
		   FROM Event \
		   WHERE RecurrencePatternID = ? \
		   AND RecurrenceState = %d \
		   "),
		xtpCalendarRecurrenceException);
	//---------------- sql statement body -------------------------
	_variant_t vtEmpty((long)0);

	m_pcmdGetRExceptions = new CADOCommand(m_pconnDb.GetInterfacePtr(), CT2CW(strSqlText));

	m_pcmdGetRExceptions->CreateParameter(L"PatternID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
}


void CXTPCalendarDatabaseDataProvider::CreateAddRPatternCommand()
{
	LPCWSTR strSqlText =
		//---------------- sql statement body -------------------------
		L"PARAMETERS \
			   prm_PatternStartDateTime    DateTime, \
			   prm_PatternEndDateTime  DateTime, \
			   prm_StartDateTime       DateTime, \
			   prm_Duration                Integer, \
			   prm_EndAfterOccurrence  Integer, \
			   prm_UseEnd              Integer, \
			   prm_RecurrenceTypeID        Integer, \
			   prm_OccurrenceInterval  Integer, \
			   prm_DayOfWeekMask       Integer, \
			   prm_DayOfMonth          Integer, \
			   prm_MonthOfYear         Integer, \
			   prm_IsEveryWeekDay      Integer, \
			   prm_DayOccurrenceTypeID Integer, \
			   prm_DayMask             Integer, \
			   prm_MasterEventID       Integer, \
			   prm_CustomPropertiesXMLData TEXT;  "

		  L" \
			\
		INSERT INTO RecurrencePattern( \
		   PatternStartDateTime, \
		   PatternEndDateTime, \
		   StartDateTime, \
		   Duration, \
		   EndAfterOccurrence, \
		   UseEnd, \
		   RecurrenceTypeID, \
		   OccurrenceInterval, \
		   DayOfWeekMask, \
		   DayOfMonth, \
		   MonthOfYear, \
		   IsEveryWeekDay, \
		   DayOccurrenceTypeID, \
		   DayMask, \
		   MasterEventID, \
		   CustomPropertiesXMLData\
		   ) \
		   VALUES( \
		   prm_PatternStartDateTime, \
		   prm_PatternEndDateTime, \
		   prm_StartDateTime, \
		   prm_Duration, \
		   prm_EndAfterOccurrence, \
		   prm_UseEnd, \
		   prm_RecurrenceTypeID, \
		   prm_OccurrenceInterval, \
		   prm_DayOfWeekMask, \
		   prm_DayOfMonth, \
		   prm_MonthOfYear, \
		   prm_IsEveryWeekDay, \
		   prm_DayOccurrenceTypeID, \
		   prm_DayMask, \
		   prm_MasterEventID, \
		   prm_CustomPropertiesXMLData \
		   ) \
		   ";
	//---------------- sql statement body -------------------------

	m_pcmdAddRPattern = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);

	_variant_t vtEmpty((long)0);
	_variant_t vtEmptyStr(EMPTY_DB_STR);

	m_pcmdAddRPattern->CreateParameter(L"prm_PatternStartDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_PatternEndDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_StartDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_Duration", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_EndAfterOccurrences", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_UseEnd", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_RecurrenceTypeID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_OccurrenceInterval", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_DayOfWeekMask", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_DayOfMonth", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_MonthOfYear", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_IsEveryWeekDay", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_DayOccurrenceType", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_DayMask", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_MasterEventID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddRPattern->CreateParameter(L"prm_CustomPropertiesXMLData", XTPADODB::adBSTR, XTPADODB::adParamInput, 0, vtEmptyStr);
}

void CXTPCalendarDatabaseDataProvider::CreateDelRPatternCommand()
{
	LPCWSTR strSqlText =
		//---------------- sql statement body -------------------------
		L"DELETE FROM RecurrencePattern WHERE RecurrencePatternID = ?";
	//---------------- sql statement body -------------------------
	_variant_t vtEmpty((long)0);
	m_pcmdDelRPattern = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);
	m_pcmdDelRPattern->CreateParameter(L"RecurrencePatternID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);

}

void CXTPCalendarDatabaseDataProvider::CreateRemoveAllCommands()
{
	LPCWSTR strSqlText;

	strSqlText = L"DELETE FROM Event";
	m_pcmdRemoveAllEvents = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);

	strSqlText = L"DELETE FROM RecurrencePattern";
	m_pcmdRemoveAllRPatterns = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);

	strSqlText = L"DELETE FROM Options";
	m_pcmdRemoveAllOptions = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);
}

void CXTPCalendarDatabaseDataProvider::CreateUpdRPatternCommand()
{
	LPCWSTR strSqlText =
		//---------------- sql statement body -------------------------

		L"PARAMETERS \
			   prm_PatternStartDateTime    DateTime, \
			   prm_PatternEndDateTime  DateTime, \
			   prm_StartDateTime       DateTime, \
			   prm_Duration                Integer, \
			   prm_EndAfterOccurrence  Integer, \
			   prm_UseEnd              Integer, \
			   prm_RecurrenceTypeID        Integer, \
			   prm_OccurrenceInterval  Integer, \
			   prm_DayOfWeekMask       Integer, \
			   prm_DayOfMonth          Integer, \
			   prm_MonthOfYear         Integer, \
			   prm_IsEveryWeekDay      Integer, \
			   prm_DayOccurrenceTypeID Integer, \
			   prm_DayMask             Integer, \
			   prm_MasterEventID       Integer, \
			   prm_CustomPropertiesXMLData TEXT, \
			   \
			   prm_RecurrencePatternID Integer;  "

	L" \
			\
		UPDATE RecurrencePattern SET \
		   PatternStartDateTime    = prm_PatternStartDateTime, \
		   PatternEndDateTime  = prm_PatternEndDateTime, \
		   StartDateTime       = prm_StartDateTime, \
		   Duration                = prm_Duration, \
		   EndAfterOccurrence  = prm_EndAfterOccurrence, \
		   UseEnd              = prm_UseEnd, \
		   RecurrenceTypeID        = prm_RecurrenceTypeID, \
		   OccurrenceInterval  = prm_OccurrenceInterval, \
		   DayOfWeekMask       = prm_DayOfWeekMask, \
		   DayOfMonth          = prm_DayOfMonth, \
		   MonthOfYear         = prm_MonthOfYear, \
		   IsEveryWeekDay      = prm_IsEveryWeekDay, \
		   DayOccurrenceTypeID = prm_DayOccurrenceTypeID, \
		   DayMask             = prm_DayMask,  \
		   MasterEventID       = prm_MasterEventID, \
		   CustomPropertiesXMLData = prm_CustomPropertiesXMLData \
		   \
		   WHERE RecurrencePatternID = prm_RecurrencePatternID \
		   ";
	//---------------- sql statement body -------------------------
	m_pcmdUpdRPattern = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);
	_variant_t vtEmpty((long)0);
	_variant_t vtEmptyStr(EMPTY_DB_STR);

	m_pcmdUpdRPattern->CreateParameter(L"prm_PatternStartDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_PatternEndDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_StartDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_Duration", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_EndAfterOccurrences", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_UseEnd", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_RecurrenceTypeID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_OccurrenceInterval", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_DayOfWeekMask", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_DayOfMonth", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_MonthOfYear", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_IsEveryWeekDay", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_DayOccurrenceType", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_DayMask", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_MasterEventID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdRPattern->CreateParameter(L"prm_CustomPropertiesXMLData", XTPADODB::adBSTR, XTPADODB::adParamInput, 0, vtEmptyStr);

	m_pcmdUpdRPattern->CreateParameter(L"prm_RecurrencePatternID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);

}

void CXTPCalendarDatabaseDataProvider::CreateGetRPatternCommand()
{
	LPCWSTR strSqlText =
		//---------------- sql statement body -------------------------
		L"SELECT RecurrencePatternID, \
		   PatternStartDateTime, \
		   PatternEndDateTime, \
		   StartDateTime, \
		   Duration, \
		   EndAfterOccurrence, \
		   UseEnd, \
		   RecurrenceTypeID, \
		   OccurrenceInterval, \
		   DayOfWeekMask, \
		   DayOfMonth, \
		   MonthOfYear, \
		   IsEveryWeekDay, \
		   DayOccurrenceTypeID, \
		   DayMask, \
		   MasterEventID, \
		   CustomPropertiesXMLData \
		   \
		   FROM RecurrencePattern \
		   WHERE RecurrencePatternID = ? \
		   ";
	//---------------- sql statement body -------------------------

	m_pcmdGetRPattern = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);
	_variant_t vtEmpty((long)0);

	m_pcmdGetRPattern->CreateParameter(L"RecurrencePatternID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
}

void CXTPCalendarDatabaseDataProvider::CreateGetOptionsCommand()
{
	LPCWSTR strSqlText =
	//---------------- sql statement body -------------------------
		L"SELECT TOP 1 OptionsID, SaveDateTime, XMLData \
			FROM Options \
			ORDER BY OptionsID DESC;";
	//---------------- sql statement body -------------------------
	m_pcmdGetOptions = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);
}

void CXTPCalendarDatabaseDataProvider::CreateAddOptionsCommand()
{
	LPCWSTR strSqlText =
	//---------------- sql statement body -------------------------
		L"PARAMETERS prm_SaveDateTime DateTime, prm_XMLData TEXT; \
			INSERT INTO Options (SaveDateTime, XMLData) \
			VALUES (prm_SaveDateTime, prm_XMLData) ";
	//---------------- sql statement body -------------------------

	m_pcmdAddOptions = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);

	_variant_t vtEmpty((long)0);
	_variant_t vtEmptyStr(EMPTY_DB_STR);
	m_pcmdAddOptions->CreateParameter(L"prm_SaveDateTime", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdAddOptions->CreateParameter(L"prm_XMLData", XTPADODB::adBSTR, XTPADODB::adParamInput, 0, vtEmptyStr);
}

void CXTPCalendarDatabaseDataProvider::CreateAddScheduleCommand()
{
	LPCWSTR strSqlText =
	//---------------- sql statement body -------------------------
		L"PARAMETERS \
			   prm_Name                        CHARACTER, \
			   prm_CustomPropertiesXMLData TEXT, \
			   prm_Modified                    DateTime; \
			   \
			INSERT INTO Schedules ( \
				Name, \
				CustomPropertiesXMLData, \
				Modified) \
			VALUES ( \
				prm_Name, \
				prm_CustomPropertiesXMLData, \
				prm_Modified) ";

	//---------------- sql statement body -------------------------
	m_pcmdAddSchedule = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);

	_variant_t vtEmpty((long)0);
	_variant_t vtEmptyStr(EMPTY_DB_STR);

	m_pcmdAddSchedule->CreateParameter(L"prm_Name", XTPADODB::adBSTR, XTPADODB::adParamInput, 0, vtEmptyStr);
	m_pcmdAddSchedule->CreateParameter(L"prm_CustomPropertiesXMLData", XTPADODB::adBSTR, XTPADODB::adParamInput, 0, vtEmptyStr);
	m_pcmdAddSchedule->CreateParameter(L"prm_Modified", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
}

void CXTPCalendarDatabaseDataProvider::CreateUpdScheduleCommand()
{
	LPCWSTR strSqlText =
		//---------------- sql statement body -------------------------

		L"PARAMETERS \
			   prm_ScheduleID              Integer, \
			   prm_Name                        CHARACTER, \
			   prm_CustomPropertiesXMLData TEXT, \
			   prm_Modified                    DateTime;"

		L" \
		UPDATE Schedules SET \
		   Name                        = prm_Name, \
		   CustomPropertiesXMLData = prm_CustomPropertiesXMLData, \
		   Modified                    = prm_Modified \
		   \
		   WHERE ScheduleID = prm_ScheduleID \
		   ";
	//---------------- sql statement body -------------------------
	m_pcmdUpdSchedule = new CADOCommand(m_pconnDb.GetInterfacePtr(), strSqlText);
	_variant_t vtEmpty((long)0);
	_variant_t vtEmptyStr(EMPTY_DB_STR);

	m_pcmdUpdSchedule->CreateParameter(L"prm_ScheduleID", XTPADODB::adInteger, XTPADODB::adParamInput, -1, vtEmpty);
	m_pcmdUpdSchedule->CreateParameter(L"prm_Name", XTPADODB::adBSTR, XTPADODB::adParamInput, 0, vtEmptyStr);
	m_pcmdUpdSchedule->CreateParameter(L"prm_CustomPropertiesXMLData", XTPADODB::adBSTR, XTPADODB::adParamInput, 0, vtEmptyStr);
	m_pcmdUpdSchedule->CreateParameter(L"prm_Modified", XTPADODB::adDate, XTPADODB::adParamInput, -1, vtEmpty);
}

/////////////////////////////////////////////////////////////////////////////
// CADOCommand

CXTPCalendarDatabaseDataProvider::CADOCommand::CADOCommand(XTPADODB::_ConnectionPtr pConnection, LPCWSTR pcszSQLText)
{
	m_pconnDb = NULL;
	try
	{
		m_pcmdSQL.CreateInstance(__uuidof(XTPADODB::Command));
		m_pcmdSQL->ActiveConnection = pConnection.GetInterfacePtr();
		m_pcmdSQL->CommandText = pcszSQLText;
		m_pconnDb = pConnection.GetInterfacePtr();
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{
		TRACE(_T("Unknown error in CADOCommand() \n"));
	}
}



CXTPCalendarDatabaseDataProvider::CADOCommand::~CADOCommand()
{
	m_pconnDb = NULL;
}


void CXTPCalendarDatabaseDataProvider::CADOCommand::SetSQLString(LPCWSTR pcszSQLText)
{
	try
	{
		m_pcmdSQL->CommandText = pcszSQLText;

	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{
		TRACE(_T("Unknown error in CADOCommand::SetSQLString() \n"));
	}

}

void CXTPCalendarDatabaseDataProvider::CADOCommand::SetParameter(
	XTPADODB::DataTypeEnum adDataType,
	long nSize,
	const _variant_t& vtValue)
{
	CreateParameter(L"",
					adDataType,
					XTPADODB::adParamInput,
					nSize,
					vtValue);

}

void CXTPCalendarDatabaseDataProvider::CADOCommand::CreateParameter(
	LPCWSTR pcszParamName,
	XTPADODB::DataTypeEnum adDataType,
	XTPADODB::ParameterDirectionEnum adDirection,
	long nSize,
	const _variant_t& vtValue)
{

	XTPADODB::_ParameterPtr pParam = NULL;
	try
	{
		pParam = m_pcmdSQL->CreateParameter(pcszParamName,
											 adDataType,
											 adDirection,
											 nSize,
											 vtValue
										  );

		m_pcmdSQL->Parameters->Append(pParam);
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch (...)
	{
		TRACE(_T("Unknown error in CreateParameter() \n"));
	}

}

void CXTPCalendarDatabaseDataProvider::CADOCommand::SetParameterValue(LPCWSTR pcszParamName, const _variant_t& vtValue)
{
	try
	{
		m_pcmdSQL->GetParameters()->GetItem(pcszParamName)->PutValue(vtValue);
	}
	catch (_com_error &e)
	{
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{
		TRACE(_T("Unknown error in CADOCommand() \n"));
	}

}

XTPADODB::_RecordsetPtr CXTPCalendarDatabaseDataProvider::CADOCommand::Execute()
{
	XTPADODB::_RecordsetPtr pRecordSet = NULL;

	try
	{
		pRecordSet = m_pcmdSQL->Execute(NULL, NULL, XTPADODB::adCmdText);
		return pRecordSet;
	}
	catch (_com_error &e)
	{
		//ClearParams();
		TRACE_ProviderError(m_pconnDb);
		TRACE_ComError(e);
	}
	catch(...)
	{
		TRACE(_T("Unknown error in Execute() \n"));
	}
	ASSERT(FALSE);
	return NULL;
}

void CXTPCalendarDatabaseDataProvider::CADOCommand::ClearParams()
{

	long nParamCount = m_pcmdSQL->Parameters->Count;

	for (long nParamCounter = 0; nParamCount > nParamCounter; nParamCounter++)
	{
		try
		{
			m_pcmdSQL->Parameters->Delete(_variant_t((long)nParamCounter));
		}
		catch(...)
		{
			TRACE(_T("Unknown error in ClearParams() \n"));
		}
	}
}
