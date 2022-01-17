// XTPCalendarMAPIDataProvider.cpp: implementation of the CXTPCalendarMAPIDataProvider class.
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

#include "Common/XTPVC80Helpers.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPPropExchange.h"
#include "Common/XTPNotifyConnection.h"

#include "XTPCalendarMAPIDataProvider.h"
#include "XTPCalendarMemoryDataProvider.h"
#include "XTPCalendarCustomProperties.h"
#include "XTPCalendarNotifications.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define XTP_TRACE_READ_EVENTS TRACE
#ifndef XTP_TRACE_READ_EVENTS
	#define XTP_TRACE_READ_EVENTS //1 ? (void)0 : TRACE
#endif

//#define XTP_TRACE_READ_IDS TRACE
#ifndef XTP_TRACE_READ_IDS
	#define XTP_TRACE_READ_IDS //1 ? (void)0 : TRACE
#endif

//#define XTP_TRACE_MAPI_NF TRACE
#ifndef XTP_TRACE_MAPI_NF
	#define XTP_TRACE_MAPI_NF //1 ? (void)0 : TRACE
#endif

#define MAPI_FREEBUFFER(pProvider, pBuffer) if (pProvider && pBuffer) { pProvider->MAPIFreeBuffer(pBuffer); pBuffer = NULL;}
#define MAPI_RELEASE(pProvider, x) if (pProvider && x) { pProvider->UlRelease(x); x = 0;}

//long xtp_dbg_SafeRelease(void* pp)
//{
//  IUnknown** ppInterface = (IUnknown**)pp;
//
//  long nRes = -1;
//  if (ppInterface && *ppInterface)
//  {
//      nRes = (*ppInterface)->Release();
//      *ppInterface = 0;
//  }
//  return nRes;
//}
//#define MAPI_RELEASE(pProvider, x) xtp_dbg_SafeRelease(&x);

#define cFileTimeUnitsPerSecond 10000000i64
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CXTPCalendarMAPIDataProvider, CXTPCalendarData)

/////////////////////////////////////////////////////////////////////////////
template<class _T>
_T xtpGetDataT(const CByteArray& arData, int nOffset)
{
	if (nOffset < 0)
	{
		return 0;
	}
	if ((int)(nOffset + sizeof(_T)) > (int)arData.GetSize())
	{
		ASSERT(FALSE);
		return 0;
	}

	const _T* pDataT = (const _T*)(arData.GetData() + nOffset);

	return *pDataT;
}

//---------------------------------------------------------------------------
template<class _T>
_T xtpGetNextDataT(const BYTE*& rpData, const BYTE* pMax = NULL)
{
	if (!rpData || pMax && (rpData + sizeof(_T)) > pMax)
	{
		ASSERT(FALSE);
		return 0;
	}

	const _T* pDataT = (const _T*)(rpData);
	rpData += sizeof(_T);

	return *pDataT;
}

//---------------------------------------------------------------------------
CString xtpGetNextData_MapiExcStr8(const BYTE*& rpData, const BYTE* pMax = NULL)
{
	if (!rpData)
	{
		ASSERT(FALSE);
		return _T("");
	}

	// layout: [w:buf-size][w:str-len][ascii-char: count=len]
	int nBuffer = (int)xtpGetNextDataT<WORD>(rpData, pMax);
	UNUSED_ALWAYS(nBuffer);
	int nStrLen = (int)xtpGetNextDataT<WORD>(rpData, pMax);

	if (pMax && (rpData + nStrLen) > pMax)
	{
		ASSERT(FALSE);
		return _T("");
	}

	CString strData = CONSTRUCT_S((char*)rpData, nStrLen);
	rpData += nStrLen;

	return strData;
}

//===========================================================================
template<class _T>
void xtpAddDataT(CByteArray& rarData, _T tValue)
{
	int nSize = sizeof(_T);
	BYTE* pBytes = (BYTE*)&tValue;
	for (int i = 0; i < nSize; i++)
	{
		rarData.Add(pBytes[i]);
	}
}

//---------------------------------------------------------------------------
template<class _T>
int xtpSetDataT(CByteArray& rarData, int nOffset, _T tValue)
{
	if (nOffset < 0)
	{
		return nOffset;
	}

	int nValueSize = sizeof(_T);
	if (rarData.GetSize() < nOffset + nValueSize)
	{
		rarData.SetSize(nOffset + nValueSize, 4096);
	}

	_T* pValue = &tValue;
	_T* pDest = (_T*)(rarData.GetData() + nOffset);

	*pDest = *pValue;

	return nOffset + nValueSize;
}

//---------------------------------------------------------------------------
void xtpAddData_MapiExcStr8(CByteArray& rarData, LPCTSTR pcszString)
{
	// layout: [w:buf-size][w:str-len][ascii-char: count=len]
	int nStrLenW = (int)_tcslen(pcszString);
	int nStrSize = nStrLenW*2 + 1;
	CHAR* pStr = new CHAR[nStrSize];
	if (!pStr)
	{
		xtpAddDataT<WORD>(rarData, 1);
		xtpAddDataT<WORD>(rarData, 0);
		return;
	}

	ZeroMemory(pStr, nStrSize);

	WCSTOMBS_S(pStr, pcszString, nStrSize-1);

	int nStrLen = (int)strlen(pStr);
	int nBuffer = nStrLen + 1;

	xtpAddDataT<WORD>(rarData, (WORD)nBuffer);
	xtpAddDataT<WORD>(rarData, (WORD)nStrLen);

	int nSize = (int)rarData.GetSize();
	rarData.SetSize(nSize + nStrLen, 4096);
	void* pDest = rarData.GetData() + nSize;

	MEMCPY_S(pDest, pStr, nStrLen);

	delete[] pStr;
}

//===========================================================================
void xtp_GetWinTZInfo(const CByteArray& arMapiTZInfo,
					  TIME_ZONE_INFORMATION* pTZInfo)
{
	if (arMapiTZInfo.GetSize() < sizeof(XTP_MAPI_TIME_ZONE_INFORMATION) || !pTZInfo)
	{
		ASSERT(FALSE);
		return;
	}
	XTP_MAPI_TIME_ZONE_INFORMATION* pMapiTZInfo;
	pMapiTZInfo = (XTP_MAPI_TIME_ZONE_INFORMATION*)arMapiTZInfo.GetData();

	ZeroMemory(pTZInfo, sizeof(TIME_ZONE_INFORMATION));

	pTZInfo->Bias         = pMapiTZInfo->Bias;
	pTZInfo->StandardBias = pMapiTZInfo->StandardBias;
	pTZInfo->DaylightBias = pMapiTZInfo->DaylightBias;
	pTZInfo->StandardDate = pMapiTZInfo->StandardDate;
	pTZInfo->DaylightDate = pMapiTZInfo->DaylightDate;
}

void xtp_GetMapiTZInfo(TIME_ZONE_INFORMATION* pTZInfo, CByteArray& arMapiTZInfo)
{
	XTP_MAPI_TIME_ZONE_INFORMATION* pMapiTZInfo;

	arMapiTZInfo.SetSize(sizeof(XTP_MAPI_TIME_ZONE_INFORMATION));
	pMapiTZInfo = (XTP_MAPI_TIME_ZONE_INFORMATION*)arMapiTZInfo.GetData();

	ZeroMemory(pMapiTZInfo, sizeof(XTP_MAPI_TIME_ZONE_INFORMATION));
	pMapiTZInfo->Bias         = pTZInfo->Bias;
	pMapiTZInfo->StandardBias = pTZInfo->StandardBias;
	pMapiTZInfo->DaylightBias = pTZInfo->DaylightBias;
	pMapiTZInfo->StandardDate = pTZInfo->StandardDate;
	pMapiTZInfo->DaylightDate = pTZInfo->DaylightDate;
}

COleDateTime xtp_UtcToTzTime(const TIME_ZONE_INFORMATION* pTZI,
							 const SYSTEMTIME& stUtcTime)
{
	SYSTEMTIME stTzTime;

	if (!CXTPCalendarUtils::SystemTimeToTzSpecificLocalTime(pTZI, &stUtcTime, &stTzTime))
	{
		return (DATE)0;
	}

	COleDateTime dtTzTime(stTzTime);

	return dtTzTime;
}

SYSTEMTIME xtp_TimeToUtc(const COleDateTime& dtTzTime)
{
	SYSTEMTIME stUtcTime;
	ZeroMemory(&stUtcTime, sizeof(stUtcTime));

	SYSTEMTIME stTzTime;
	if (!GETASSYSTEMTIME_DT(dtTzTime, stTzTime) )
	{
		return stUtcTime;
	}

	FILETIME ftTz;
	if (!SystemTimeToFileTime(&stTzTime, &ftTz))
	{
		return stUtcTime;
	}

	FILETIME ftUtc;
	if (!LocalFileTimeToFileTime(&ftTz, &ftUtc))
	{
		return stUtcTime;
	}

	if (!FileTimeToSystemTime(&ftUtc, &stUtcTime))
	{
		ZeroMemory(&stUtcTime, sizeof(stUtcTime));
	}

	return stUtcTime;
}

/////////////////////////////////////////////////////////////////////////////
class CXTPCalendarMAPI_Recurrence
{
public:

	// Holds byte offsets of data fields within MAPI binary field
	struct XTP_RCDATA_LAYOUT
	{
		int     nType;          // radioSelector;
		int     nType2;         // type
		int     nRCShiftX;      // magic MAPI number. Probably used to generate occurrences.
		int     nInterval;      // interval;
		int     nRegenerate;    // regenerate   // for tasks only
		int     nWeekDayMask;   // weekDayMask  // week Of Month
		int     nInstance;      // instance
		int     nEndByType;     // endByType
		int     nOccurrences;
		int     nDynData;       // Unaligned data offset
	};

	// 0 - Daily, 1 - Weekly, 2 - MonYearly, 3 - MonYearly_Nth
	static XTP_RCDATA_LAYOUT s_arRCDataLayout[4];

	//=======================================================================
	class CXTPRcException
	{
	public:
		DWORD   dwNewStart;
		DWORD   dwNewEnd;
		DWORD   dwOrigStart;

		BOOL    bDeleted;

		WORD    wFlags;

		CString strSubject;     // xtpMAPIRcED_Subject
		BOOL    bIsMeeting;     // xtpMAPIRcED_IsMeeting
		int     nReminderTime;  // xtpMAPIRcED_ReminderTime
		BOOL    bIsReminder;    // xtpMAPIRcED_IsReminder
		CString strLocation;    // xtpMAPIRcED_Location
		int     nBusyStatus;    // xtpMAPIRcED_BusyStatus - enum XTPCalendarEventBusyStatus
		BOOL    bIsAllDay;      // xtpMAPIRcED_IsAllDay
		int     nLabel;         // xtpMAPIRcED_Label

		CXTPRcException();
		const CXTPRcException& operator=(const CXTPRcException& rSrc);
		void Set(CXTPCalendarEvent* pExc, CXTPCalendarEvent* pMaster);
	};

	class CXTPRcExceptions : public CArray<CXTPRcException, CXTPRcException&>
	{
	public:
		CXTPRcExceptions(){};
		virtual ~CXTPRcExceptions(){};

		int FindByOrigStart(DWORD dwOrigStart);

		void SortByOrigStart();
		void SortByNewStart();
	protected:
		typedef int (_cdecl* T_CompareFunc)(const CXTPRcException* pExc1, const CXTPRcException* pExc2);
		void _Sort(T_CompareFunc);

		static int _cdecl CompareAsc_ByOrigStart(const CXTPRcException* pExc1, const CXTPRcException* pExc2);
		static int _cdecl CompareAsc_ByNewStart(const CXTPRcException* pExc1, const CXTPRcException* pExc2);
	};

	//=======================================================================
	//struct XTP_RCDATA
	class CXTPRcData
	{
	public:
		WORD    wType;
		BYTE    ucType2;
		DWORD   dwRCShiftX;
		DWORD   dwInterval;
		DWORD   dwWeekDayMask;
		BYTE    ucInstance;

		DWORD   dwEndByType;
		DWORD   dwOccurrences;
		DWORD   dwEndDate;

		DWORD   dwStartDate;

		DWORD   dwOccStartTime;
		DWORD   dwOccEndTime;

		CXTPRcExceptions arExceptions;

		CXTPRcData();
		virtual ~CXTPRcData(){};

	};

	static BOOL ReadRCData(CXTPRcData& rRCData, const CByteArray& arRCData);

	static void SetRecurrenceOptions(CXTPCalendarRecurrencePattern* pPattern,
								const CXTPRcData& RCData);
	static void SetRecurrenceExceptions(CXTPCalendarEvent* pMasterEvent,
								CXTPCalendarRecurrencePattern* pPattern,
								const CXTPRcData& RCData);

	//-----------------------------------------------------------------------
	static BOOL FillRCData( CXTPRcData& rRCData,
							CXTPCalendarRecurrencePattern* pPattern,
							CXTPCalendarEvent* pMaster);

	static BOOL RCDataToBin(CXTPRcData& rRCData, CByteArray& rarRCData);

	static COleDateTime ConvertRecurrenceMinutesToDate(DWORD dwMinutes);
	static DWORD ConvertRecurrenceDateToMinutes(COleDateTime date);

protected:
	static void _ReadRCDataExceptions (CXTPRcData& rRCData, const BYTE* pExcData,
								const BYTE* pMaxPtr, const BYTE* pExclusions);

	static DWORD _CalcRCShiftX(CXTPCalendarRecurrencePattern* pPattern);
};

//
//  MAPI recurrence layouts definitions
//
CXTPCalendarMAPI_Recurrence::XTP_RCDATA_LAYOUT
CXTPCalendarMAPI_Recurrence::s_arRCDataLayout[] =
{
	//  daily
	{
		4,  // type
		6,  // type2
		10, // RCShiftX
		14, // interval
		18, // regenerate
		-1, // weekDayMask
		-1, // instance
		22, // endByType
		26, // nOccurrences
		26 + 2 * sizeof(DWORD) // nDynData
	},
	//  weakly
	{
		4,  // type
		6,  // type2
		10, // RCShiftX
		14, // interval
		18, // regenerate
		22, // weekDayMask
		-1, // instance
		26, // endByType
		30, // nOccurrences
		30 + 2 * sizeof(DWORD) // nDynData
	},
	//  monthly
	{
		4,  // type
		6,  // type2
		10, // RCShiftX
		14, // interval
		18, // regenerate
		-1, // weekDayMask
		22, // instance
		26, // endByType
		30, // nOccurrences
		30 + 2 * sizeof(DWORD) // nDynData
	},
	//  monthly Nth ()
	{
		4,  // type
		6,  // type2
		10, // RCShiftX
		14, // interval
		18, // regenerate
		22, // weekDayMask
		26, // instance
		30, // endByType
		34, // nOccurrences
		34 + 2 * sizeof(DWORD) // nDynData
	},
};

//////////////////////////////////////////////////////////////////////////
CXTPCalendarMAPI_Recurrence::CXTPRcException::CXTPRcException()
{
	dwNewStart = 0;
	dwNewEnd = 0;
	dwOrigStart = 0;

	bDeleted = FALSE;

	wFlags = 0;
	//strSubject;
	bIsMeeting = FALSE;
	nReminderTime = 0;
	bIsReminder = FALSE;
	//strLocation;
	nBusyStatus = 0;
	bIsAllDay = FALSE;
	nLabel = 0;
}

void CXTPCalendarMAPI_Recurrence::CXTPRcException::Set(CXTPCalendarEvent* pExc, CXTPCalendarEvent* pMaster)
{
	CXTPRcException rexZero;
	*this = rexZero;

	if (!pExc || !pMaster)
	{
		ASSERT(FALSE);
		return;
	}

	wFlags = 0;

	bDeleted = pExc->IsRExceptionDeleted();

	dwOrigStart = CXTPCalendarMAPI_Recurrence::ConvertRecurrenceDateToMinutes(pExc->GetRException_StartTimeOrig());

	if (bDeleted)
	{
		return;
	}

	dwNewStart = CXTPCalendarMAPI_Recurrence::ConvertRecurrenceDateToMinutes(pExc->GetStartTime());
	dwNewEnd = CXTPCalendarMAPI_Recurrence::ConvertRecurrenceDateToMinutes(pExc->GetEndTime());

	if (pExc->GetSubject() != pMaster->GetSubject())
	{
		wFlags |= xtpMAPIRcED_Subject;
		strSubject = pExc->GetSubject();
	}
	if (pExc->IsMeeting() != pMaster->IsMeeting())
	{
		wFlags |= xtpMAPIRcED_IsMeeting;
		bIsMeeting = pExc->IsMeeting();
	}
	if (pExc->GetReminderMinutesBeforeStart() != pMaster->GetReminderMinutesBeforeStart())
	{
		wFlags |= xtpMAPIRcED_ReminderTime;
		nReminderTime = pExc->GetReminderMinutesBeforeStart();
	}
	if (pExc->IsReminder() != pMaster->IsReminder())
	{
		wFlags |= xtpMAPIRcED_IsReminder;
		bIsReminder = pExc->IsReminder();
	}
	if (pExc->GetLocation() != pMaster->GetLocation())
	{
		wFlags |= xtpMAPIRcED_Location;
		strLocation = pExc->GetLocation();
	}
	if (pExc->GetBusyStatus() != pMaster->GetBusyStatus())
	{
		wFlags |= xtpMAPIRcED_BusyStatus;
		nBusyStatus = pExc->GetBusyStatus();
	}
	if (pExc->IsAllDayEvent() != pMaster->IsAllDayEvent())
	{
		wFlags |= xtpMAPIRcED_IsAllDay;
		bIsAllDay = pExc->IsAllDayEvent();
	}
	if (pExc->GetLabelID() != pMaster->GetLabelID())
	{
		wFlags |= xtpMAPIRcED_Label;
		nLabel = pExc->GetLabelID();
	}
}

const CXTPCalendarMAPI_Recurrence::CXTPRcException&
	CXTPCalendarMAPI_Recurrence::CXTPRcException::operator= (
		const CXTPCalendarMAPI_Recurrence::CXTPRcException& rSrc)
{
	dwNewStart = rSrc.dwNewStart;
	dwNewEnd = rSrc.dwNewEnd;
	dwOrigStart = rSrc.dwOrigStart;

	bDeleted = rSrc.bDeleted;

	wFlags = rSrc.wFlags;

	strSubject = rSrc.strSubject;
	bIsMeeting = rSrc.bIsMeeting;
	nReminderTime = rSrc.nReminderTime;
	bIsReminder = rSrc.bIsReminder;
	strLocation = rSrc.strLocation;
	nBusyStatus = rSrc.nBusyStatus;
	bIsAllDay = rSrc.bIsAllDay;
	nLabel = rSrc.nLabel;

	return *this;
}

int CXTPCalendarMAPI_Recurrence::CXTPRcExceptions::FindByOrigStart(DWORD dwOrigStart)
{
	int nCount = (int)GetSize();
	for (int i = 0; i < nCount; i++)
	{
		const CXTPRcException& rce = ElementAt(i);

		if (rce.dwOrigStart == dwOrigStart)
		{
			return i;
		}
	}
	return -1;
}

void CXTPCalendarMAPI_Recurrence::CXTPRcExceptions::SortByOrigStart()
{
	_Sort(CompareAsc_ByOrigStart);
}

void CXTPCalendarMAPI_Recurrence::CXTPRcExceptions::SortByNewStart()
{
	_Sort(CompareAsc_ByNewStart);
}

void CXTPCalendarMAPI_Recurrence::CXTPRcExceptions::_Sort(T_CompareFunc pCompareFunc)
{
	typedef int (_cdecl *GENERICCOMPAREFUNC)(const void *, const void*);

	qsort(GetData(), (size_t)GetSize(), sizeof(CXTPRcException),
		 (GENERICCOMPAREFUNC)pCompareFunc);
}

int _cdecl CXTPCalendarMAPI_Recurrence::CXTPRcExceptions::CompareAsc_ByOrigStart(const CXTPRcException* pExc1, const CXTPRcException* pExc2)
{
	if (pExc1->dwOrigStart < pExc2->dwOrigStart)
	{
		return -1;
	}
	if (pExc1->dwOrigStart > pExc2->dwOrigStart)
	{
		return 1;
	}
	return 0;
}

int _cdecl CXTPCalendarMAPI_Recurrence::CXTPRcExceptions::CompareAsc_ByNewStart(const CXTPRcException* pExc1, const CXTPRcException* pExc2)
{
	if (pExc1->dwNewStart < pExc2->dwNewStart)
	{
		return -1;
	}
	if (pExc1->dwNewStart > pExc2->dwNewStart)
	{
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarMAPI_Recurrence::CXTPRcData::CXTPRcData()
{
	wType = 0;
	ucType2 = 0;
	dwRCShiftX = 0;
	dwInterval = 0;
	dwWeekDayMask = 0;
	ucInstance = 0;

	dwEndByType = 0;
	dwOccurrences = 0;
	dwEndDate = 0;

	dwStartDate = 0;

	dwOccStartTime = 0;
	dwOccEndTime = 0;

	//arExceptions;
}


BOOL CXTPCalendarMAPI_Recurrence::ReadRCData(
						CXTPCalendarMAPI_Recurrence::CXTPRcData& rRCData,
						const CByteArray& arRCData)
{
	XTPCalendarMAPIRcType eRCType;
	XTPCalendarMAPIRcType2 eRCType2;
	eRCType =  (XTPCalendarMAPIRcType)xtpGetDataT<WORD>(arRCData, s_arRCDataLayout[0].nType);
	eRCType2 = (XTPCalendarMAPIRcType2)xtpGetDataT<BYTE>(arRCData, s_arRCDataLayout[0].nType2);

	BOOL bTypeValid = eRCType == xtpMAPIRcType_Daily    || eRCType == xtpMAPIRcType_Weekly ||
					  eRCType == xtpMAPIRcType_Monthly    || eRCType == xtpMAPIRcType_Yearly;

	if (eRCType2 < xtpMAPIRcType2_First || eRCType2 > xtpMAPIRcType2_Last || !bTypeValid)
	{
		return FALSE;
	}

	XTP_RCDATA_LAYOUT RCLayout;
	::ZeroMemory(&RCLayout, sizeof(RCLayout));

	// select layout
	RCLayout = s_arRCDataLayout[eRCType2];

	rRCData.wType = xtpGetDataT<WORD>(arRCData, RCLayout.nType);
	rRCData.ucType2 = xtpGetDataT<BYTE>(arRCData, RCLayout.nType2);
	rRCData.dwRCShiftX = xtpGetDataT<DWORD>(arRCData, RCLayout.nRCShiftX);
	rRCData.dwInterval = xtpGetDataT<DWORD>(arRCData, RCLayout.nInterval);
	rRCData.dwWeekDayMask = xtpGetDataT<DWORD>(arRCData, RCLayout.nWeekDayMask);
	rRCData.ucInstance = xtpGetDataT<BYTE>(arRCData, RCLayout.nInstance);
	rRCData.dwEndByType = xtpGetDataT<DWORD>(arRCData, RCLayout.nEndByType);
	rRCData.dwOccurrences = xtpGetDataT<DWORD>(arRCData, RCLayout.nOccurrences);

	//
	const BYTE* p = arRCData.GetData() + RCLayout.nDynData;
	const BYTE* pMax = arRCData.GetData() + arRCData.GetSize();

	const BYTE* pExclusions = p;

	// skip old and new dates
	p += sizeof(DWORD) * (xtpGetNextDataT<DWORD>(p, pMax)); // skip old start dates
	p += sizeof(DWORD) * (xtpGetNextDataT<DWORD>(p, pMax)); // old new start dates

	// pick up start/end dates

	//Pattern Start Date
	rRCData.dwStartDate = xtpGetNextDataT<DWORD>(p, pMax);
	// /*DBG*/ ConvertRecurrenceMinutesToDate(rRCData.dwStartDate);

	//Pattern End Date
	rRCData.dwEndDate = xtpGetNextDataT<DWORD>(p, pMax);
	// /*DBG*/ ConvertRecurrenceMinutesToDate(rRCData.dwEndDate);

	// skip unknown reserved fields
	xtpGetNextDataT<DWORD>(p, pMax);
	xtpGetNextDataT<DWORD>(p, pMax);

	// Occurrence start/end time - in minutes from the day begin
	// hour = time / 60, minute = time % 60
	rRCData.dwOccStartTime = xtpGetNextDataT<DWORD>(p, pMax);
	rRCData.dwOccEndTime = xtpGetNextDataT<DWORD>(p, pMax);

	//p - points to detailed exceptions information.

	_ReadRCDataExceptions(rRCData, p, pMax, pExclusions);

	return TRUE;
}

void CXTPCalendarMAPI_Recurrence::_ReadRCDataExceptions(
						CXTPCalendarMAPI_Recurrence::CXTPRcData& rRCData,
						const BYTE* pExcData, const BYTE* pMaxPtr,
						const BYTE* pExclusions
						)
{
	// read detailed recurrence exceptions information
	int nCount = (int)xtpGetNextDataT<WORD>(pExcData, pMaxPtr);
	int i;
	for (i = 0; i < nCount; i++)
	{
		CXTPRcException RCExc;

		RCExc.dwNewStart = xtpGetNextDataT<DWORD>(pExcData, pMaxPtr);
		RCExc.dwNewEnd = xtpGetNextDataT<DWORD>(pExcData, pMaxPtr);
		RCExc.dwOrigStart = xtpGetNextDataT<DWORD>(pExcData, pMaxPtr);

		RCExc.wFlags = xtpGetNextDataT<WORD>(pExcData, pMaxPtr);

		if (RCExc.wFlags & xtpMAPIRcED_Subject)
		{
			// layout: [w:buf-size][w:str-len][ascii-char: count=len]
			RCExc.strSubject = xtpGetNextData_MapiExcStr8(pExcData, pMaxPtr);
		}
		if (RCExc.wFlags & xtpMAPIRcED_IsMeeting)
		{
			// layout: [dw] - bool(0|1)
			RCExc.bIsMeeting = xtpGetNextDataT<DWORD>(pExcData, pMaxPtr) != 0;
		}
		if (RCExc.wFlags & xtpMAPIRcED_ReminderTime)
		{
			// layout: [dw] minutes before start
			RCExc.nReminderTime = xtpGetNextDataT<DWORD>(pExcData, pMaxPtr);
		}
		if (RCExc.wFlags & xtpMAPIRcED_IsReminder)
		{
			// layout: [dw] - bool(0|1)
			RCExc.bIsReminder = xtpGetNextDataT<DWORD>(pExcData, pMaxPtr) != 0;
		}
		if (RCExc.wFlags & xtpMAPIRcED_Location)
		{
			// layout: [w:buf-size][w:str-len][ascii-char: count=len]
			RCExc.strLocation = xtpGetNextData_MapiExcStr8(pExcData, pMaxPtr);
		}
		if (RCExc.wFlags & xtpMAPIRcED_BusyStatus)
		{
			// layout: [dw] - busy status [0..3] = XTPCalendarEventBusyStatus
			RCExc.nBusyStatus = xtpGetNextDataT<DWORD>(pExcData, pMaxPtr);
		}
		if (RCExc.wFlags & xtpMAPIRcED_IsAllDay)
		{
			// layout: [dw] - bool(0|1)
			RCExc.bIsAllDay = xtpGetNextDataT<DWORD>(pExcData, pMaxPtr) != 0;
		}
		if (RCExc.wFlags & xtpMAPIRcED_Label)
		{
			// layout: [dw] -
			RCExc.nLabel = xtpGetNextDataT<DWORD>(pExcData, pMaxPtr);
		}

		rRCData.arExceptions.Add(RCExc);
	}

	ASSERT(rRCData.dwOccEndTime >= rRCData.dwOccStartTime);

	// determine and add deleted exceptions
	nCount = (int)xtpGetNextDataT<DWORD>(pExclusions, pMaxPtr);
	for (i = 0; i < nCount; i++)
	{
		CXTPRcException RCExc;
		RCExc.dwOrigStart = xtpGetNextDataT<DWORD>(pExclusions, pMaxPtr);
		RCExc.dwOrigStart += rRCData.dwOccStartTime;
		RCExc.bDeleted = TRUE;

		if (rRCData.arExceptions.FindByOrigStart(RCExc.dwOrigStart) < 0)
		{
			rRCData.arExceptions.Add(RCExc);
		}
	}
}


void CXTPCalendarMAPI_Recurrence::SetRecurrenceOptions(
						CXTPCalendarRecurrencePattern* pPattern,
						const CXTPCalendarMAPI_Recurrence::CXTPRcData& RCData)
{
	ASSERT(pPattern);
	if (!pPattern)
	{
		return;
	}

	XTP_CALENDAR_RECURRENCE_OPTIONS opt;

	// Recurrence type
	switch(RCData.wType)
	{
		case xtpMAPIRcType_Daily:
		{
			opt.m_nRecurrenceType = xtpCalendarRecurrenceDaily;
			opt.m_Daily.bEveryWeekDayOnly = (RCData.ucType2 == xtpMAPIRcType2_Weekly);

			if (opt.m_Daily.bEveryWeekDayOnly)
			{
				opt.m_Daily.nIntervalDays = 1;
			}
			else
			{
				ASSERT(RCData.dwInterval % (24 * 60) == 0);
				opt.m_Daily.nIntervalDays = (int)RCData.dwInterval / (24 * 60);
			}
		}
		break;

		case xtpMAPIRcType_Weekly:
		{
			opt.m_nRecurrenceType = xtpCalendarRecurrenceWeekly;
			ASSERT(RCData.ucType2 == xtpMAPIRcType2_Weekly); // == 1, 0 only for tasks

			opt.m_Weekly.nIntervalWeeks = (int)RCData.dwInterval;
			opt.m_Weekly.nDayOfWeekMask = (int)RCData.dwWeekDayMask;
		}
		break;

		case xtpMAPIRcType_Monthly:
		{
			if (RCData.ucType2 == xtpMAPIRcType2_MonYearly)
			{
				opt.m_nRecurrenceType = xtpCalendarRecurrenceMonthly;
				opt.m_Monthly.nIntervalMonths = (int)RCData.dwInterval;
				opt.m_Monthly.nDayOfMonth = (int)RCData.ucInstance;
			}
			else if (RCData.ucType2 == xtpMAPIRcType2_MonYearly_Nth)
			{
				opt.m_nRecurrenceType = xtpCalendarRecurrenceMonthNth;
				opt.m_MonthNth.nIntervalMonths = (int)RCData.dwInterval;
				opt.m_MonthNth.nWhichDay = (int)RCData.ucInstance; // ???
				opt.m_MonthNth.nWhichDayMask = (int)RCData.dwWeekDayMask; // ???
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		break;

		case xtpMAPIRcType_Yearly:
		{
			ASSERT(RCData.dwInterval % 12 == 0 && RCData.dwInterval);

			//---------------------------------------------------------------
			int nMonthOfYear = 1;
			// days per month   - jan feb mar apr may jun   jul aug sep oct nov dec
			static int arDays[] = {31, 28, 31, 30, 31, 30,   31, 31, 30, 31, 30, 31};
			int nDaysInMinutes = 0;
			for (int i = 0; i < 12; i++)
			{
				if (nDaysInMinutes >= (int)RCData.dwRCShiftX)
				{
					nMonthOfYear = i+1;
					break;
				}
				nDaysInMinutes += arDays[i] * 24 * 60;
			}
			ASSERT(nDaysInMinutes == (int)RCData.dwRCShiftX);

			//---------------------------------------------------------------
			if (RCData.ucType2 == xtpMAPIRcType2_MonYearly)
			{
				opt.m_nRecurrenceType = xtpCalendarRecurrenceYearly;
				opt.m_Yearly.nDayOfMonth = RCData.ucInstance;
				opt.m_Yearly.nMonthOfYear = nMonthOfYear;
			}
			else if (RCData.ucType2 == xtpMAPIRcType2_MonYearly_Nth)
			{
				opt.m_nRecurrenceType = xtpCalendarRecurrenceYearNth;
				opt.m_YearNth.nWhichDay = RCData.ucInstance;
				opt.m_YearNth.nWhichDayMask = RCData.dwWeekDayMask;
				opt.m_YearNth.nMonthOfYear = nMonthOfYear;
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		break;
	default:
		ASSERT(FALSE);
	}
	pPattern->SetRecurrenceOptions(opt);

	pPattern->SetPatternStartDate(ConvertRecurrenceMinutesToDate(RCData.dwStartDate));

	XTP_CALENDAR_PATTERN_END rpEnd;
	switch(RCData.dwEndByType)
	{
	case xtpMAPIRcEnd_Date:
		rpEnd.m_nUseEnd = xtpCalendarPatternEndDate;
		rpEnd.m_dtPatternEndDate = ConvertRecurrenceMinutesToDate(RCData.dwEndDate);
		break;
	case xtpMAPIRcEnd_Number:
		rpEnd.m_nUseEnd = xtpCalendarPatternEndAfterOccurrences;
		rpEnd.m_nEndAfterOccurrences = RCData.dwOccurrences;
		break;
	case xtpMAPIRcEnd_Never:
		rpEnd.m_nUseEnd = xtpCalendarPatternEndNoDate;
		break;
	default: // Use no end date by default
		rpEnd.m_nUseEnd = xtpCalendarPatternEndNoDate;
	}
	pPattern->SetPatternEnd(rpEnd);

	COleDateTime dtOccStartTime;
	dtOccStartTime.SetTime(RCData.dwOccStartTime / 60, RCData.dwOccStartTime % 60, 0);

	pPattern->SetStartTime(dtOccStartTime);

	int nDuration = RCData.dwOccEndTime - RCData.dwOccStartTime;
	ASSERT(nDuration >= 0);
	pPattern->SetDurationMinutes(nDuration);
}

void CXTPCalendarMAPI_Recurrence::SetRecurrenceExceptions(
								CXTPCalendarEvent* pMasterEvent,
								CXTPCalendarRecurrencePattern* pPattern,
								const CXTPRcData& RCData)
{
	if (!pMasterEvent || !pPattern)
	{
		ASSERT(FALSE);
		return;
	}

	int nCount = (int)RCData.arExceptions.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		const CXTPRcException& RCExc = RCData.arExceptions[i];

		COleDateTime dtOrigStart = ConvertRecurrenceMinutesToDate(RCExc.dwOrigStart);

		COleDateTime dtOrigEnd = dtOrigStart + pPattern->GetDuration();

		CXTPCalendarEventPtr ptrExc = pMasterEvent->CloneForOccurrence(dtOrigStart, dtOrigEnd);
		if (!ptrExc)
		{
			return;
		}
		VERIFY( ptrExc->MakeAsRException() );

		if (RCExc.bDeleted)
		{
			ptrExc->SetRExceptionDeleted(TRUE);
			VERIFY( pPattern->SetException(ptrExc) );
			continue;
		}

		//---------------------------------------------------------------
		COleDateTime dtNewStart = dtOrigStart;
		COleDateTime dtNewEnd = dtOrigEnd;

		if (RCExc.dwNewStart)
		{
			dtNewStart = ConvertRecurrenceMinutesToDate(RCExc.dwNewStart);
		}
		if (RCExc.dwNewEnd)
		{
			dtNewEnd = ConvertRecurrenceMinutesToDate(RCExc.dwNewEnd);
		}

		ptrExc->SetStartTime(dtNewStart);
		ptrExc->SetEndTime(dtNewEnd);

		//---------------------------------------------------------------
		if (RCExc.wFlags & xtpMAPIRcED_Subject)
		{
			ptrExc->SetSubject(RCExc.strSubject);
		}
		if (RCExc.wFlags & xtpMAPIRcED_IsMeeting)
		{
			ptrExc->SetMeeting(RCExc.bIsMeeting);
		}
		if (RCExc.wFlags & xtpMAPIRcED_ReminderTime)
		{
			ptrExc->SetReminderMinutesBeforeStart(RCExc.nReminderTime);
		}
		if (RCExc.wFlags & xtpMAPIRcED_IsReminder)
		{
			ptrExc->SetReminder(RCExc.bIsReminder);
		}
		if (RCExc.wFlags & xtpMAPIRcED_Location)
		{
			ptrExc->SetLocation(RCExc.strLocation);
		}
		if (RCExc.wFlags & xtpMAPIRcED_BusyStatus)
		{
			ASSERT(RCExc.nBusyStatus < 4);
			if (RCExc.nBusyStatus >= 0 && RCExc.nBusyStatus < 4)
			{
				ptrExc->SetBusyStatus(RCExc.nBusyStatus);
			}
		}

		ASSERT((RCExc.wFlags & xtpMAPIRcED_Reserved) == 0);

		if (RCExc.wFlags & xtpMAPIRcED_IsAllDay)
		{
			ptrExc->SetAllDayEvent(RCExc.bIsAllDay);
		}

		if (RCExc.wFlags & xtpMAPIRcED_Label)
		{
			ptrExc->SetLabelID(RCExc.nLabel);
		}

		VERIFY( pPattern->SetException(ptrExc) );
	}
}

BOOL CXTPCalendarMAPI_Recurrence::FillRCData(CXTPRcData& rRCData,
									CXTPCalendarRecurrencePattern* pPattern,
									CXTPCalendarEvent* pMaster)
{
	// FileTime zero date
	static const COleDateTime cdtFTZero(1601, 1, 1, 0, 0, 0);

	if (!pPattern || !pMaster)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	XTP_CALENDAR_RECURRENCE_OPTIONS opt = pPattern->GetRecurrenceOptions();

	switch(opt.m_nRecurrenceType)
	{
		case xtpCalendarRecurrenceDaily:
		{
			rRCData.wType = xtpMAPIRcType_Daily;

			if (opt.m_Daily.bEveryWeekDayOnly)
			{
				rRCData.ucType2 = xtpMAPIRcType2_Weekly;
				rRCData.dwInterval = 1;
				rRCData.dwWeekDayMask = xtpCalendarDayMo_Fr;
			}
			else
			{
				rRCData.ucType2 = xtpMAPIRcType2_Daily;
				rRCData.dwInterval = opt.m_Daily.nIntervalDays * 24 * 60;
			}
		}
		break;

		case xtpCalendarRecurrenceWeekly:
		{
			rRCData.wType = xtpMAPIRcType_Weekly;
			rRCData.ucType2 = xtpMAPIRcType2_Weekly; // = 1, 0 only for tasks

			rRCData.dwInterval = opt.m_Weekly.nIntervalWeeks;
			rRCData.dwWeekDayMask = opt.m_Weekly.nDayOfWeekMask;
		}
		break;

		case xtpCalendarRecurrenceMonthly:
		{
			rRCData.wType = xtpMAPIRcType_Monthly;
			rRCData.ucType2 = xtpMAPIRcType2_MonYearly;

			rRCData.dwInterval = (DWORD)opt.m_Monthly.nIntervalMonths;
			rRCData.ucInstance = (BYTE)opt.m_Monthly.nDayOfMonth;
		}
		break;

		case xtpCalendarRecurrenceMonthNth:
		{
			rRCData.wType = xtpMAPIRcType_Monthly;
			rRCData.ucType2 = xtpMAPIRcType2_MonYearly_Nth;

			rRCData.dwInterval = (DWORD)opt.m_MonthNth.nIntervalMonths;
			rRCData.ucInstance = (BYTE)opt.m_MonthNth.nWhichDay;
			rRCData.dwWeekDayMask = (DWORD)opt.m_MonthNth.nWhichDayMask;
		}
		break;

		case xtpCalendarRecurrenceYearly:
		{
			rRCData.wType = xtpMAPIRcType_Yearly;
			rRCData.ucType2 = xtpMAPIRcType2_MonYearly;

			rRCData.ucInstance = (BYTE)opt.m_Yearly.nDayOfMonth;

			rRCData.dwInterval = 12;
		}
		break;

		case xtpCalendarRecurrenceYearNth:
		{
			rRCData.wType = xtpMAPIRcType_Yearly;
			rRCData.ucType2 = xtpMAPIRcType2_MonYearly_Nth;

			rRCData.ucInstance = (BYTE)opt.m_YearNth.nWhichDay;
			rRCData.dwWeekDayMask = (DWORD)opt.m_YearNth.nWhichDayMask;

			rRCData.dwInterval = 12;
		}
		break;
	default:
		ASSERT(FALSE);
	}
	rRCData.dwRCShiftX = _CalcRCShiftX(pPattern);

	rRCData.dwStartDate = ConvertRecurrenceDateToMinutes(pPattern->GetPatternStartDate());

	CXTPCalendarDatesArray arOccDates;
	XTP_CALENDAR_PATTERN_END rpEnd = pPattern->GetPatternEnd();
	switch(rpEnd.m_nUseEnd)
	{
	case xtpCalendarPatternEndDate:
		rRCData.dwEndByType  = xtpMAPIRcEnd_Date;
		rRCData.dwEndDate = ConvertRecurrenceDateToMinutes(rpEnd.m_dtPatternEndDate);
		pPattern->GetOccurrencesDates(arOccDates, pPattern->GetPatternStartDate(), pPattern->GetPatternEndDate());
		rRCData.dwOccurrences = (DWORD)arOccDates.GetSize();
		break;
	case xtpCalendarPatternEndAfterOccurrences:
		rRCData.dwEndByType  = xtpMAPIRcEnd_Number;
		rRCData.dwOccurrences = rpEnd.m_nEndAfterOccurrences;
		rRCData.dwEndDate = ConvertRecurrenceDateToMinutes(pMaster->GetEndTime());
		break;
	case xtpCalendarPatternEndNoDate:
		rRCData.dwEndByType  = xtpMAPIRcEnd_Never;
		break;
	default: // Use No end date by default
		rRCData.dwEndByType  = xtpMAPIRcEnd_Never;
	}

	COleDateTime dtOccStartTime = pPattern->GetStartTime();
	rRCData.dwOccStartTime = dtOccStartTime.GetHour() * 60 + dtOccStartTime.GetMinute();

	int nOccDuration = pPattern->GetDurationMinutes();
	if (pMaster->IsAllDayEvent() && ( (nOccDuration % (24 * 60)) || nOccDuration == 0) )
	{
		// align occurrence duration for full day(s) duration
		nOccDuration = (nOccDuration / (24 * 60) + 1) * 24 * 60;
	}
	rRCData.dwOccEndTime = rRCData.dwOccStartTime + nOccDuration;

	//------------------------------------------
	CXTPCalendarEventsPtr ptrExcAr = pPattern->GetExceptions();
	int nExcCount = ptrExcAr ? ptrExcAr->GetCount() : 0;
	for (int i = 0; i < nExcCount; i++)
	{
		CXTPCalendarEvent* pExc = ptrExcAr->GetAt(i);

		CXTPRcException rceTmp;
		rceTmp.Set(pExc, pMaster);

		rRCData.arExceptions.Add(rceTmp);
	}

	return TRUE;
}

BOOL CXTPCalendarMAPI_Recurrence::RCDataToBin(CXTPRcData& RCData, CByteArray& rarData)
{
	XTP_RCDATA_LAYOUT RCLayout;
	::ZeroMemory(&RCLayout, sizeof(RCLayout));

	ASSERT(RCData.ucType2 <= 3);
	if (RCData.ucType2 > 3)
	{
		return FALSE;
	}
	// select layout
	RCLayout = s_arRCDataLayout[RCData.ucType2];

	rarData.SetSize(RCLayout.nDynData, 4096);

	// Set header
	 xtpSetDataT<DWORD>(rarData, 0, 0x30043004);

	// Set generic recurrence fields
	xtpSetDataT<WORD> (rarData, RCLayout.nType,         RCData.wType);
	xtpSetDataT<BYTE> (rarData, RCLayout.nType2,        RCData.ucType2);
	xtpSetDataT<DWORD>(rarData, RCLayout.nRCShiftX,     RCData.dwRCShiftX);
	xtpSetDataT<DWORD>(rarData, RCLayout.nInterval,     RCData.dwInterval);
	xtpSetDataT<DWORD>(rarData, RCLayout.nWeekDayMask,  RCData.dwWeekDayMask);
	xtpSetDataT<BYTE> (rarData, RCLayout.nInstance,     RCData.ucInstance);
	xtpSetDataT<DWORD>(rarData, RCLayout.nEndByType,    RCData.dwEndByType);
	xtpSetDataT<DWORD>(rarData, RCLayout.nOccurrences,   RCData.dwOccurrences);

	// Add information about exceptions
	//
	ASSERT(rarData.GetSize() == RCLayout.nDynData);

	int i = 0;

	//- add ALL exceptions dates -----------------------------------------
	RCData.arExceptions.SortByOrigStart();

	int nExcCount = (int)RCData.arExceptions.GetSize();
	xtpAddDataT<DWORD>(rarData, nExcCount);
	for (i = 0; i < nExcCount; i++)
	{
		const CXTPRcException& rce = RCData.arExceptions[i];
		DWORD dwOrigSartDate = (rce.dwOrigStart / 24 / 60) * 24 * 60;
		xtpAddDataT<DWORD>(rarData, dwOrigSartDate);
	}

	//- add changed exception dates -----------------------------------------
	RCData.arExceptions.SortByNewStart();

	int nChangExcCountOffset = (int)rarData.GetSize();
	xtpAddDataT<DWORD>(rarData, 0);
	DWORD dwChangExcCount = 0;

	for (i = 0; i < nExcCount; i++)
	{
		const CXTPRcException& rce = RCData.arExceptions[i];
		if (!rce.bDeleted)
		{
			ASSERT(rce.dwNewStart != 0 && rce.dwNewEnd != 0);
			DWORD dwNewSartDate = (rce.dwNewStart / 24 / 60) * 24 * 60;
			xtpAddDataT<DWORD>(rarData, dwNewSartDate);
			dwChangExcCount++;
		}
	}
	xtpSetDataT<DWORD>(rarData, nChangExcCountOffset, dwChangExcCount);

	// start/end date
	xtpAddDataT<DWORD>(rarData, RCData.dwStartDate);

	if (RCData.dwEndByType == xtpMAPIRcEnd_Date || RCData.dwEndByType == xtpMAPIRcEnd_Number)
	{
		xtpAddDataT<DWORD>(rarData, RCData.dwEndDate);
	}
	else
	{
		ASSERT(RCData.dwEndByType == xtpMAPIRcEnd_Never);
		xtpAddDataT<DWORD>(rarData, 0x5AE980DF);
	}

	// unknown
	xtpAddDataT<DWORD>(rarData, 0x00003006);
	xtpAddDataT<DWORD>(rarData, 0x00003006);

	// start/end time
	xtpAddDataT<DWORD>(rarData, RCData.dwOccStartTime);
	xtpAddDataT<DWORD>(rarData, RCData.dwOccEndTime);

	// detailed exceptions
	xtpAddDataT<WORD>(rarData, (WORD)dwChangExcCount);

	for (i = 0; i < nExcCount; i++)
	{
		const CXTPRcException& rce = RCData.arExceptions[i];
		if (rce.bDeleted)
		{
			continue;
		}

		ASSERT(rce.dwNewStart && rce.dwNewEnd && rce.dwOrigStart);

		xtpAddDataT<DWORD>(rarData, rce.dwNewStart);
		xtpAddDataT<DWORD>(rarData, rce.dwNewEnd);
		xtpAddDataT<DWORD>(rarData, rce.dwOrigStart);

		xtpAddDataT<WORD>(rarData, rce.wFlags);

		if (rce.wFlags & xtpMAPIRcED_Subject)
		{
			xtpAddData_MapiExcStr8(rarData, rce.strSubject);
		}
		if (rce.wFlags & xtpMAPIRcED_IsMeeting)
		{
			// layout: [dw] - bool(0|1)
			xtpAddDataT<DWORD>(rarData, rce.bIsMeeting);
		}
		if (rce.wFlags & xtpMAPIRcED_ReminderTime)
		{
			// layout: [dw] minutes before start
			xtpAddDataT<DWORD>(rarData, rce.nReminderTime);
		}
		if (rce.wFlags & xtpMAPIRcED_IsReminder)
		{
			// layout: [dw] - bool(0|1)
			xtpAddDataT<DWORD>(rarData, rce.bIsReminder);
		}
		if (rce.wFlags & xtpMAPIRcED_Location)
		{
			xtpAddData_MapiExcStr8(rarData, rce.strLocation);
		}
		if (rce.wFlags & xtpMAPIRcED_BusyStatus)
		{
			// layout: [dw] - busy status [0..3] = XTPCalendarEventBusyStatus
			xtpAddDataT<DWORD>(rarData, rce.nBusyStatus);
		}
		if (rce.wFlags & xtpMAPIRcED_IsAllDay)
		{
			// layout: [dw] - bool(0|1)
			xtpAddDataT<DWORD>(rarData, rce.bIsAllDay);
		}
		if (rce.wFlags & xtpMAPIRcED_Label)
		{
			// layout: [dw] -
			xtpAddDataT<DWORD>(rarData, rce.nLabel);
		}
	}

	return TRUE;
}



DWORD CXTPCalendarMAPI_Recurrence::_CalcRCShiftX(CXTPCalendarRecurrencePattern* pPattern)
{
	// FileTime zero date
	static const COleDateTime cdtFTZero(1601, 1, 1, 0, 0, 0);

	ASSERT(pPattern);
	if (!pPattern)
	{
		return 0;
	}
	DWORD dwRCShiftX = 0;

	COleDateTime dtPatternStart = pPattern->GetPatternStartDate();
	COleDateTimeSpan spX;

	XTP_CALENDAR_RECURRENCE_OPTIONS opt = pPattern->GetRecurrenceOptions();

	// xtpMAPIRcType2_Daily
	if (opt.m_nRecurrenceType == xtpCalendarRecurrenceDaily && !opt.m_Daily.bEveryWeekDayOnly)
	{

		spX = dtPatternStart - cdtFTZero;
		int nIntervalDays = max(1, opt.m_Daily.nIntervalDays);
		int nDay = (int)GETTOTAL_DAYS_DTS(spX);

		dwRCShiftX = nDay % nIntervalDays;
		dwRCShiftX *= 24 * 60; // convert to minutes
	}
	// xtpMAPIRcType2_Weekly
	else if (opt.m_nRecurrenceType == xtpCalendarRecurrenceDaily && opt.m_Daily.bEveryWeekDayOnly ||
			 opt.m_nRecurrenceType == xtpCalendarRecurrenceWeekly)
	{
		int nIntervalWeeks = 1;
		if (opt.m_nRecurrenceType == xtpCalendarRecurrenceWeekly)
		{
			nIntervalWeeks = opt.m_Weekly.nIntervalWeeks;
		}

		spX = dtPatternStart - cdtFTZero;
		int nWeek = ((int)(GETTOTAL_DAYS_DTS(spX) + 1)) / 7;

		dwRCShiftX = (nWeek * 7 - 1) % (nIntervalWeeks * 7);
		dwRCShiftX *= 24 * 60; // convert to minutes
	}
	else
	{
		//xtpMAPIRcType2_MonYearly
		//xtpMAPIRcType2_MonYearly_Nth

		int nIntervalMonths = -1;

		if (opt.m_nRecurrenceType == xtpCalendarRecurrenceMonthly)
		{
			nIntervalMonths = opt.m_Monthly.nIntervalMonths;
		}
		else if (opt.m_nRecurrenceType == xtpCalendarRecurrenceMonthNth)
		{
			nIntervalMonths = opt.m_MonthNth.nIntervalMonths;
		}
		else if (opt.m_nRecurrenceType == xtpCalendarRecurrenceYearly ||
				 opt.m_nRecurrenceType == xtpCalendarRecurrenceYearNth)
		{
			nIntervalMonths = 12;
		}
		else
		{
			ASSERT(FALSE);
			return 0;
		}
		nIntervalMonths = max(1, nIntervalMonths);

		// days per month   - jan feb mar apr may jun   jul aug sep oct nov dec
		static int arDays[] = {31, 28, 31, 30, 31, 30,   31, 31, 30, 31, 30, 31};
		int nMonth = dtPatternStart.GetMonth() + 12 * (dtPatternStart.GetYear() - 1601);

		for (int i = 1 ; i < ((nMonth - 1) % nIntervalMonths) + 1; i++ )
		{
			dwRCShiftX += arDays[(i - 1) % 12];
		}
		dwRCShiftX *= 24 * 60; // convert to minutes
	}

	return dwRCShiftX;
}

COleDateTime CXTPCalendarMAPI_Recurrence::ConvertRecurrenceMinutesToDate(DWORD dwMinutes)
{
	COleDateTime dt = (DATE)0;

	ULONGLONG   ullDT = (ULONGLONG) dwMinutes * 60i64 * cFileTimeUnitsPerSecond;
	FILETIME    ft = *((FILETIME*)&ullDT); // { (DWORD) ( fileTimeUnitsBeforeStart & 0xFFFFFFFF), (DWORD) ( fileTimeUnitsBeforeStart >> 32)};

	SYSTEMTIME  st;
	BOOL bRes = ::FileTimeToSystemTime(&ft, &st);
	ASSERT(bRes);
	if (bRes)
	{
		dt = st;
	}
//  TRACE(_T("MAPItime to DT %d = %s\n"), dwMinutes, dt.Format());

	return dt;
}

DWORD CXTPCalendarMAPI_Recurrence::ConvertRecurrenceDateToMinutes(COleDateTime dt)
{
	SYSTEMTIME st;
	FILETIME ft = {0, 0};

	if (!GETASSYSTEMTIME_DT(dt, st))
	{
		ASSERT(FALSE);
		return 0;
	}
	if (!SystemTimeToFileTime(&st, &ft))
	{
		ASSERT(FALSE);
		return 0;
	}

	ULONGLONG ullMinutes =  *((ULONGLONG*)&ft); //( ( (ULONGLONG) ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
	ullMinutes = ullMinutes / 60i64;
	ullMinutes = ullMinutes / cFileTimeUnitsPerSecond;
	return (DWORD)ullMinutes;
}

//////////////////////////////////////////////////////////////////////////
// see KB239795

STDMETHODIMP CXTPCalendarMAPIDataProvider::OpenInbox(
	LPMDB lpMDB, LPMAPIFOLDER *lpInboxFolder, LPMAPIFOLDER *lpCalendarFolder, CXTPCalendarMAPIDataProvider* pProvider)
{
	if (!lpMDB || !pProvider)
		return E_POINTER;

	ULONG        cbInbox;
	LPENTRYID    lpbInbox;
	ULONG        ulObjType;
	HRESULT      hRes = S_OK;
	LPMAPIFOLDER    lpTempFolder = NULL;

	ULONG pCount; SPropValue *props;
	SizedSPropTagArray(1, spec) = {1, {XTP_TAG_ID_MAPI_CALENDAR_FOLDER}};

	*lpInboxFolder = NULL;
	*lpCalendarFolder = NULL;

	// The Inbox is usually the default receive folder for the message store
	// You call this function as a shortcut to get it's Entry ID
	hRes = lpMDB->GetReceiveFolder(
		NULL,      // Get default receive folder
		NULL,      // Flags
		&cbInbox,  // Size and ...
		&lpbInbox, // Value of the EntryID to be returned
		NULL);     // You don't care to see the class returned
	if (SUCCEEDED(hRes))
	{
		hRes = lpMDB->OpenEntry(
			cbInbox,                      // Size and...
			lpbInbox,                     // Value of the Inbox's EntryID
			NULL,                         // We want the default interface    (IMAPIFolder)
			MAPI_BEST_ACCESS,             // Flags
			&ulObjType,                   // Object returned type
			(LPUNKNOWN *) &lpTempFolder); // Returned folder
		if (SUCCEEDED(hRes))
		{
			// Assign the out parameter
			*lpInboxFolder = lpTempFolder;

			// Open Calendar Folder
			hRes = (*lpInboxFolder)->GetProps((SPropTagArray*)&spec, 0, &pCount, &props);

			if (hRes == S_OK || hRes == MAPI_W_ERRORS_RETURNED)
			{
				if (props[0].ulPropTag != PT_ERROR)
				{
					hRes = lpMDB->OpenEntry(
						props[0].Value.bin.cb,            // Size and...
						(ENTRYID*)props[0].Value.bin.lpb, // Value of the Calendar's EntryID
						NULL,                         // We want the default interface (IMAPIFolder)
						MAPI_BEST_ACCESS,             // Flags
						&ulObjType,                   // Object returned type
						(LPUNKNOWN *) &lpTempFolder); // Returned folder
					if (SUCCEEDED(hRes))
					{
						// Assign the out parameter
						*lpCalendarFolder = lpTempFolder;
					}
				}
			}

			if (props)
			{
				pProvider->MAPIFreeBuffer(props);
				props = NULL;
			}
		}
	}

	// Cleanup
	pProvider->MAPIFreeBuffer(lpbInbox);
	return hRes;
}

STDMETHODIMP CXTPCalendarMAPIDataProvider::OpenDefaultMessageStore(
   LPMAPISESSION lpMAPISession, LPMDB * lpMDB, CXTPCalendarMAPIDataProvider* pProvider)
{
	if (!lpMAPISession || !pProvider)
		return E_POINTER;

	LPMAPITABLE pStoresTbl = NULL;
	LPSRowSet   pRow = NULL;
	static      SRestriction sres;
	SPropValue  spv;
	HRESULT     hRes;
	LPMDB       lpTempMDB = NULL;

	enum {EID, NAME, NUM_COLS};
	static SizedSPropTagArray(NUM_COLS,sptCols) = {NUM_COLS, PR_ENTRYID, PR_DISPLAY_NAME};

	*lpMDB = NULL;

	//Get the table of all the message stores available
	hRes = lpMAPISession->GetMsgStoresTable(0, &pStoresTbl);
	if (SUCCEEDED(hRes))
	{
		//Set up restriction for the default store
		sres.rt = RES_PROPERTY; //Comparing a property
		sres.res.resProperty.relop = RELOP_EQ; //Testing equality
		sres.res.resProperty.ulPropTag = PR_DEFAULT_STORE; //Tag to compare
		sres.res.resProperty.lpProp = &spv; //Prop tag and value to compare against

		spv.ulPropTag = PR_DEFAULT_STORE; //Tag type
		spv.Value.b   = TRUE; //Tag value

		//Convert the table to an array which can be stepped through
		//Only one message store should have PR_DEFAULT_STORE set to true, so only one will be returned
		hRes = pProvider->HrQueryAllRows(
			pStoresTbl, //Table to query
			(LPSPropTagArray) &sptCols, //Which columns to get
			&sres, //Restriction to use
			NULL, //No sort order
			0, //Max number of rows (0 means no limit)
			&pRow); //Array to return
		if (SUCCEEDED(hRes))
		{
			//Open the first returned (default) message store
			hRes = lpMAPISession->OpenMsgStore(
			  NULL,//Window handle for dialogs
			  pRow->aRow[0].lpProps[EID].Value.bin.cb,//size and...
			  (LPENTRYID)pRow->aRow[0].lpProps[EID].Value.bin.lpb,//value of entry to open
			  NULL,//Use default interface (IMsgStore) to open store
			  MAPI_BEST_ACCESS,//Flags
			  &lpTempMDB);//Pointer to place the store in
			if (SUCCEEDED(hRes))
			{
				//Assign the out parameter
				*lpMDB = lpTempMDB;
			}
		}
	}

	// clean up
	pProvider->FreeProws(pRow);
	MAPI_RELEASE(pProvider, pStoresTbl);
	if (FAILED(hRes))
	{
		HRESULT hr;
		LPMAPIERROR lpError;
		hr = lpMAPISession->GetLastError(hRes, 0, &lpError);
		if (!hr && lpError)
		{
			TRACE(_T("%s\n%s\n"), lpError->lpszError, lpError->lpszComponent);
			pProvider->MAPIFreeBuffer(lpError);
		}
	}
	return hRes;
}

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarMAPIDataProvider::CXTPCalendarMemDPInternal::CXTPCalendarMemDPInternal(CXTPCalendarData* pDPExternal)
{
	ASSERT(pDPExternal);
	m_pDPExternal = pDPExternal;
}

void CXTPCalendarMAPIDataProvider::CXTPCalendarMemDPInternal::SendNotification(XTP_NOTIFY_CODE EventCode, WPARAM wParam , LPARAM lParam)
{
	class CXTPCalendarDataSnNf : public CXTPCalendarData
	{
	public:
		using CXTPCalendarData::SendNotification;
	};
	//===================================
	if (m_bDisableNotificationsSending)
	{
		return;
	}
	if (m_pDPExternal)
	{
		((CXTPCalendarDataSnNf*)m_pDPExternal)->SendNotification(EventCode, wParam, lParam);
	}
	else
	{
		CXTPCalendarData::SendNotification(EventCode, wParam, lParam);
	}
}
//////////////////////////////////////////////////////////////////////////
CXTPCalendarMAPIDataProvider::CXTPCalendarMAPIDataProvider()
{
	m_MapiHelper.m_pProvider = this;

	m_typeProvider = xtpCalendarDataProviderMAPI;

	m_bMAPIInitialized = FALSE;
	m_lpMAPISession = NULL;
	m_lpMDB = NULL;
	m_lpCalendarFolder = NULL;

	m_ulMAPIConID0 = 0;
	m_ulMAPIConID1 = 0;

	m_lpAdviseSink = NULL;
	m_lpAdviseSink_ThrSafe = NULL;

	//m_pMemDP = new CXTPCalendarMemoryDataProvider();
	m_pMemDP = new CXTPCalendarMemDPInternal(this);

}

CXTPCalendarMAPIDataProvider::~CXTPCalendarMAPIDataProvider()
{
	CMDTARGET_RELEASE(m_pMemDP);
}

CXTPCalendarEventsPtr CXTPCalendarMAPIDataProvider::DoRetrieveDayEvents(COleDateTime dtDay)
{
	return m_pMemDP ? m_pMemDP->RetrieveDayEvents(dtDay) : NULL;
}

void CXTPCalendarMAPIDataProvider::DoRemoveAllEvents()
{
	if (m_pMemDP)
	{
		m_pMemDP->RemoveAllEvents();
	}
}

CXTPCalendarEventPtr CXTPCalendarMAPIDataProvider::DoRead_Event(DWORD dwEventID)
{
	return m_pMemDP ? m_pMemDP->GetEvent(dwEventID) : NULL;
}

CXTPCalendarRecurrencePatternPtr CXTPCalendarMAPIDataProvider::DoRead_RPattern(DWORD dwPatternID)
{
	return m_pMemDP ? m_pMemDP->GetRecurrencePattern(dwPatternID) : NULL;
}

BOOL CXTPCalendarMAPIDataProvider::DoCreate_Event(CXTPCalendarEvent* pEvent, DWORD& rdwNewEventID)
{
	CXTPAutoResetValue<BOOL> autoReset(m_bDisableNotificationsSending, FALSE);
	m_bDisableNotificationsSending = TRUE;

	if (!IsOpen())
	{
		return FALSE;
	}

	if (!m_lpCalendarFolder || !m_pMemDP || !pEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	DWORD dwEventID = pEvent->GetEventID();

	int nRState = pEvent->GetRecurrenceState();

	if (nRState != xtpCalendarRecurrenceNotRecurring &&
		nRState != xtpCalendarRecurrenceMaster)
	{
		ASSERT(nRState == xtpCalendarRecurrenceException);
		rdwNewEventID = GetNextUniqueEventID(dwEventID);
		return TRUE;
	}

	LPMESSAGE pMessage = NULL;
	HRESULT hr = m_lpCalendarFolder->CreateMessage(NULL, 0, &pMessage);
	if (FAILED(hr) || !pMessage)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (!UpdateMAPIEvent(pMessage, pEvent))
	{
		MAPI_RELEASE(this, pMessage);
		return FALSE;
	}

	// to ensure that PR_SEARCH_KEY property is valid
	hr = pMessage->SaveChanges(KEEP_OPEN_READONLY);
	if (FAILED(hr))
	{
		ASSERT(FALSE);

		MAPI_RELEASE(this, pMessage);
		return FALSE;
	}

	_SetEventRuntimeProps(pEvent->GetCustomProperties(), pMessage);

	//-----------------------------------------------------------------------
	CByteArray arSearchKey;
	HRESULT hrID = _getPropVal(pMessage, PR_SEARCH_KEY, arSearchKey);

	if (FAILED(hrID))
	{
		ASSERT(FALSE);

		MAPI_RELEASE(this, pMessage);
		return NULL;
	}

	rdwNewEventID = GetEventID((int)arSearchKey.GetSize(),
							   (LPENTRYID)arSearchKey.GetData(), TRUE);
	//-----------------------------------------------------------------------
	pEvent->SetEventID(rdwNewEventID);
	BOOL bRes = m_pMemDP->AddEvent(pEvent);

	MAPI_RELEASE(this, pMessage);

	return bRes;
}

BOOL CXTPCalendarMAPIDataProvider::DoUpdate_Event(CXTPCalendarEvent* pEvent)
{
	CXTPAutoResetValue<BOOL> autoReset(m_bDisableNotificationsSending, FALSE);
	m_bDisableNotificationsSending = TRUE;

	if (!IsOpen())
	{
		return FALSE;
	}

	if (!m_lpCalendarFolder || !m_pMemDP || !pEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//=================================================
	int nRState = pEvent->GetRecurrenceState();

	if (nRState != xtpCalendarRecurrenceNotRecurring &&
		nRState != xtpCalendarRecurrenceMaster)
	{
		ASSERT(nRState == xtpCalendarRecurrenceException);
		return TRUE;
	}

	//=================================================
	BOOL bRes = m_pMemDP->ChangeEvent(pEvent);
	if (!bRes)
	{
		return FALSE;
	}

	DWORD dwEventID = pEvent->GetEventID();
	CXTPMAPIBinary eKey;

	if (!m_mapID.Lookup(dwEventID, eKey))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	LPMESSAGE pMessage = NULL;
	ULONG ulObjType = NULL;

	// Get event ID by its SearchKey
	CXTPMAPIBinary eid = GetEntryID(eKey);

	// open event from Calendar Folder storage
	HRESULT hr = m_lpCalendarFolder->OpenEntry(
						eid.GetBinarySize(), (LPENTRYID)eid.GetBinaryData(),
						NULL,//default interface
						MAPI_BEST_ACCESS,
						&ulObjType,
						(LPUNKNOWN*)&pMessage);

	ASSERT(ulObjType == 5);
	ASSERT(pMessage);
	if (FAILED(hr) || !pMessage)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (!UpdateMAPIEvent(pMessage, pEvent))
	{
		MAPI_RELEASE(this, pMessage);
		return FALSE;
	}

	hr = pMessage->SaveChanges(KEEP_OPEN_READONLY);
	if (FAILED(hr))
	{
		ASSERT(FALSE);

		MAPI_RELEASE(this, pMessage);
		return FALSE;
	}
	//-----------------------------------------------------------------------
//  BOOL bRes = m_pMemDP->ChangeEvent(pEvent);

	MAPI_RELEASE(this, pMessage);

	return TRUE;
}

BOOL CXTPCalendarMAPIDataProvider::DoDelete_Event(CXTPCalendarEvent* pEvent)
{
	CXTPAutoResetValue<BOOL> autoReset(m_bDisableNotificationsSending, FALSE);
	m_bDisableNotificationsSending = TRUE;

	if (!IsOpen())
	{
		return FALSE;
	}

	if (!m_lpCalendarFolder || !m_pMemDP || !pEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nRState = pEvent->GetRecurrenceState();

	if (nRState != xtpCalendarRecurrenceNotRecurring &&
		nRState != xtpCalendarRecurrenceMaster)
	{
		ASSERT(nRState == xtpCalendarRecurrenceException);
		return TRUE;
	}

	CXTPMAPIBinary eKey;
	DWORD dwEventID = pEvent->GetEventID();

	if (!m_mapID.Lookup(dwEventID, eKey))
	{
		// ASSERT(FALSE); // possible when copied appointments
		return FALSE;
	}

	// get event ID by its SearchKey
	CXTPMAPIBinary eid = GetEntryID(eKey);

	// delete event
	ENTRYLIST eidList1;
	SBinary   eidBinary;

	eidList1.cValues = 1;
	eidList1.lpbin = &eidBinary;

	eidBinary.cb = eid.GetBinarySize();
	eidBinary.lpb = (BYTE*)eid.GetBinaryData();

	HRESULT hr = m_lpCalendarFolder->DeleteMessages(&eidList1, 0, NULL, 0);

	if (FAILED(hr))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	return m_pMemDP ? m_pMemDP->DeleteEvent(pEvent) : FALSE;
}

BOOL CXTPCalendarMAPIDataProvider::DoCreate_RPattern(CXTPCalendarRecurrencePattern* pPattern, DWORD& rdwNewPatternID)
{
	rdwNewPatternID = pPattern->GetPatternID();
	return TRUE;
}

BOOL CXTPCalendarMAPIDataProvider::DoUpdate_RPattern(CXTPCalendarRecurrencePattern* pPattern)
{
	UNREFERENCED_PARAMETER(pPattern);
	return TRUE;
}

BOOL CXTPCalendarMAPIDataProvider::DoDelete_RPattern(CXTPCalendarRecurrencePattern* pPattern)
{
	UNREFERENCED_PARAMETER(pPattern);
	return TRUE;
}

CXTPCalendarEventsPtr CXTPCalendarMAPIDataProvider::DoGetUpcomingEvents(COleDateTime dtFrom, COleDateTimeSpan spPeriod)
{
	UNREFERENCED_PARAMETER(dtFrom); UNREFERENCED_PARAMETER(spPeriod);
	return DoGetAllEvents_raw();
}


CXTPCalendarEventsPtr CXTPCalendarMAPIDataProvider::DoGetAllEvents_raw()
{
	return m_pMemDP ? m_pMemDP->GetAllEvents_raw() : NULL;
}


BOOL CXTPCalendarMAPIDataProvider::Open()
{
	if (m_pMemDP)
	{
		m_pMemDP->Open();
	}

	HRESULT       hRes;
	LPMAPIFOLDER  lpInboxFolder = NULL;
	LPSPropValue  lpCalendarFolderEntryID = NULL;

	hRes = this->MAPIInitialize(NULL);
	if (SUCCEEDED(hRes))
	{
		m_bMAPIInitialized = TRUE;

		hRes = this->MAPILogonEx(0,
							NULL,//profile name
							NULL,//password - This parameter should ALWAYS be NULL
							MAPI_LOGON_UI, //Allow a profile picker box to show if not logged in
							&m_lpMAPISession);//handle of session

		if (SUCCEEDED(hRes))
		{
			hRes = OpenDefaultMessageStore(m_lpMAPISession, &m_lpMDB, this);
			if (SUCCEEDED(hRes))
			{
				hRes = OpenInbox(m_lpMDB, &lpInboxFolder, &m_lpCalendarFolder, this);

				if (SUCCEEDED(hRes))
				{
					//Checking to see that we did get the Calendar folder
					hRes = this->HrGetOneProp(m_lpCalendarFolder, PR_ENTRYID,
										&lpCalendarFolderEntryID);

					if (SUCCEEDED(hRes))
					{
						m_eidCalendarFolder.Set(lpCalendarFolderEntryID);
					}
				}
			}
		}
	}

	if (SUCCEEDED(hRes))
	{
		// copy all events to the cache data provider
		if (m_pMemDP)
		{
			CXTPAutoResetValue<BOOL> autoReset(m_bDisableNotificationsSending, FALSE);
			m_bDisableNotificationsSending = TRUE;

			m_pMemDP->AddEvents(ImportAllEvents());
		}

		// advise to MAPI notifications
		hRes = this->HrAllocAdviseSink(MAPICallBack_OnNotify, this, &m_lpAdviseSink);
		ASSERT(SUCCEEDED(hRes) && m_lpAdviseSink);

		hRes = this->HrThisThreadAdviseSink(m_lpAdviseSink, &m_lpAdviseSink_ThrSafe);
		ASSERT(SUCCEEDED(hRes) && m_lpAdviseSink_ThrSafe);

		if (SUCCEEDED(hRes) && m_lpAdviseSink_ThrSafe)
		{
			ULONG uEventsMask = fnevObjectCreated | fnevObjectDeleted | fnevObjectModified |
								fnevObjectMoved; // | fnevObjectCopied;

			ULONG uEIDsize =  lpCalendarFolderEntryID->Value.bin.cb;
			LPENTRYID pEIDdata = (LPENTRYID)lpCalendarFolderEntryID->Value.bin.lpb;

			hRes = m_lpMDB->Advise(0, NULL, uEventsMask, m_lpAdviseSink_ThrSafe,
									&m_ulMAPIConID0);
			ASSERT(SUCCEEDED(hRes));

			// to receive fnevObjectDeleted notification
			hRes = m_lpMDB->Advise(uEIDsize, pEIDdata, uEventsMask,
									m_lpAdviseSink_ThrSafe, &m_ulMAPIConID1);
			ASSERT(SUCCEEDED(hRes));

			// to force Advise call in the RPC.
			ULONG nValues = 0;
			LPSPropValue pPropArray;
			hRes = m_lpMDB->GetProps(NULL, 0, &nValues, &pPropArray);
			ASSERT(SUCCEEDED(hRes));

			MAPI_FREEBUFFER(this, pPropArray);
		}
	}

	MAPI_FREEBUFFER(this, lpCalendarFolderEntryID);
	MAPI_RELEASE(this, lpInboxFolder);

	if (FAILED(hRes))
	{
		// cleanup
		m_bOpened = TRUE; // to let Close run.
		Close();
	}

	if (SUCCEEDED(hRes))
	{
		CXTPCalendarData::Open();
	}

	return SUCCEEDED(hRes);
}

BOOL CXTPCalendarMAPIDataProvider::Create()
{
	return Open();
}

BOOL CXTPCalendarMAPIDataProvider::Save()
{
	CXTPCalendarData::Save();
	return TRUE;
}

void CXTPCalendarMAPIDataProvider::Close()
{
	if (!m_bOpened)
		return;

	if (m_pMemDP)
	{
		m_pMemDP->Close();
	}

	HRESULT hRes = 0;

	// Unadvise -------------------------------------
	if (m_ulMAPIConID0)
	{
		hRes = m_lpMDB->Unadvise(m_ulMAPIConID0);
		ASSERT(SUCCEEDED(hRes));
		m_ulMAPIConID0 = 0;
	}

	if (m_ulMAPIConID1)
	{
		hRes = m_lpMDB->Unadvise(m_ulMAPIConID1);
		ASSERT(SUCCEEDED(hRes));
		m_ulMAPIConID1 = 0;
	}

	// release opened objects -----------------------
	MAPI_RELEASE(this, m_lpAdviseSink_ThrSafe);
	MAPI_RELEASE(this, m_lpAdviseSink);

	MAPI_RELEASE(this, m_lpCalendarFolder);
	MAPI_RELEASE(this, m_lpMDB);

	// LogOff session -------------------------------
	if (m_lpMAPISession)
	{
		hRes = m_lpMAPISession->Logoff(0, 0, 0);
		ASSERT(SUCCEEDED(hRes));
	}
	MAPI_RELEASE(this, m_lpMAPISession);

	// Initialize MAPI dll --------------------------
	if (m_bMAPIInitialized)
	{
		this->MAPIUninitialize();
		m_bMAPIInitialized = FALSE;
	}

	CXTPCalendarData::Close();
}

BOOL CXTPCalendarMAPIDataProvider::IsOpen()
{
	return m_lpCalendarFolder != NULL;
}

//LPSRestriction CXTPCalendarMAPIDataProvider::BuildDayRestriction(const COleDateTime dtDay)
//{
//  return 0;
///*    static SRestriction sres;
//  static SPropValue spv[2];
//  static SRestriction sr[2];
//  ZeroMemory(spv, 2 * sizeof(SPropValue));
//  // Set up restriction for the current day event
//  sres.rt = RES_AND; // Comparing a property
//  sres.res.resAnd.cRes = 2; // Count of restrictions
//  sres.res.resAnd.lpRes = sr;
//
//  // Start time is current or less
//  sr[0].rt = RES_PROPERTY; // Comparing a property
//  sr[0].res.resProperty.relop = RELOP_LE; // lower or equal
//  sr[0].res.resProperty.ulPropTag = XTP_PR_MAPI_EVENT_START_TIME; // Tag to compare
//  sr[0].res.resProperty.lpProp = &spv[0]; // Prop tag and value to compare against
//
//  spv[0].ulPropTag = XTP_PR_MAPI_EVENT_START_TIME; // Tag type
//  SYSTEMTIME st;
//  COleDateTime dtEndDay = CXTPCalendarUtils::ResetTime(dtDay) + COleDateTimeSpan(0, 23, 59, 59);
//  if (dtEndDay.GetAsSystemTime(st))
//  {
//      SystemTimeToFileTime(&st, &spv[0].Value.ft); // Tag value
//  }
//
//  // End time is more or current
//  sr[1].rt = RES_PROPERTY; // Comparing a property
//  sr[1].res.resProperty.relop = RELOP_GE; // greater or equal
//  sr[1].res.resProperty.ulPropTag = XTP_PR_MAPI_EVENT_END_TIME; // Tag to compare
//  sr[1].res.resProperty.lpProp = &spv[1]; // Prop tag and value to compare against
//
//  spv[1].ulPropTag = XTP_PR_MAPI_EVENT_END_TIME; // Tag type
//  if (CXTPCalendarUtils::ResetTime(dtDay).GetAsSystemTime(st))
//  {
//      SystemTimeToFileTime(&st, &spv[1].Value.ft); // Tag value
//  }
//
//  return &sres;
//  */
//}

LPSRestriction CXTPCalendarMAPIDataProvider::BuildBinaryRestriction(ULONG cbSize, LPBYTE lpData, ULONG ulPropTag)
{
	static SRestriction sres;
	static SPropValue spv;
	ZeroMemory(&spv, sizeof(SPropValue));
	// Set up restriction for the current day event
	sres.rt = RES_PROPERTY; // Comparing a property
	sres.res.resProperty.relop = RELOP_EQ; // Equal
	sres.res.resProperty.ulPropTag = ulPropTag; // Entry ID
	sres.res.resProperty.lpProp = &spv; // Prop tag and value to compare against

	spv.ulPropTag = ulPropTag; // Tag type
	spv.Value.bin.cb = cbSize;
	spv.Value.bin.lpb = lpData;

	return &sres;
}

void CXTPCalendarMAPIDataProvider::_SetEventRuntimeProps(CXTPCalendarCustomProperties* pEventProps, LPMESSAGE pMessage)
{
	ASSERT(pEventProps && pMessage);
	if (!pEventProps || !pMessage)
		return;

	CByteArray arEntryID, arSearchKey;

	HRESULT hrID = _getPropVal(pMessage, PR_ENTRYID, arEntryID);
	if (SUCCEEDED(hrID))
	{
		COleVariant varEntryID(arEntryID);
		pEventProps->SetProperty(cszMAPIpropVal_EntryID, varEntryID);
	}

	hrID = _getPropVal(pMessage, PR_SEARCH_KEY, arSearchKey);
	if (SUCCEEDED(hrID))
	{
		COleVariant varSearchKey(arSearchKey);
		pEventProps->SetProperty(cszMAPIpropVal_SearchKey, varSearchKey);
	}
}

void CXTPCalendarMAPIDataProvider::_MoveMAPIEventRuntimeProps(CXTPCalendarCustomProperties* pDest,
																CXTPCalendarCustomProperties* pSrc)
{
	ASSERT(pSrc && pDest);
	if (!pSrc || !pDest)
		return;

	COleVariant varValue;

	if (pDest && pSrc->GetProperty(cszMAPIpropVal_EntryID, varValue))
	{
		pDest->SetProperty(cszMAPIpropVal_EntryID, varValue);
	}
	pSrc->RemoveProperty(cszMAPIpropVal_EntryID);

	if (pDest && pSrc->GetProperty(cszMAPIpropVal_SearchKey, varValue))
	{
		pDest->SetProperty(cszMAPIpropVal_SearchKey, varValue);
	}
	pSrc->RemoveProperty(cszMAPIpropVal_SearchKey);
}

CXTPCalendarEventPtr CXTPCalendarMAPIDataProvider::ImportEvent_FromCalendarFolderOnly(
											ULONG cbEntryID, LPENTRYID lpEntryID)
{
	LPSRestriction pRestriction = BuildBinaryRestriction(cbEntryID, (LPBYTE)lpEntryID, PR_ENTRYID);
	CXTPCalendarEventsPtr ptrEvents = ImportAllEvents(pRestriction);

	CXTPCalendarEventPtr ptrEvent;
	if (ptrEvents && ptrEvents->GetCount())
	{
		ASSERT(ptrEvents->GetCount() == 1);
		ptrEvent = ptrEvents->GetAt(0, TRUE);
	}
	return ptrEvent;
}

CXTPCalendarEventPtr CXTPCalendarMAPIDataProvider::ImportEvent(LPSRow lpRow)
{
	if (!lpRow)
		return NULL;

	// ------- ID --------
	if (PR_ENTRYID != lpRow->lpProps[0].ulPropTag)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CXTPCalendarEventPtr ptrEvent = ImportEvent(lpRow->lpProps[0].Value.bin.cb,
									(LPENTRYID)lpRow->lpProps[0].Value.bin.lpb);

	return ptrEvent;
}

CXTPCalendarEventPtr CXTPCalendarMAPIDataProvider::ImportEvent(ULONG cbEntryID,
														LPENTRYID lpEntryID)
{
	if (cbEntryID == 0 || !lpEntryID)
	{
		ASSERT(FALSE);
		return NULL;
	}

	LPMESSAGE pMessage = NULL;
	ULONG ulObjType = NULL;

	// open event from Calendar Folder storage
	m_lpCalendarFolder->OpenEntry(cbEntryID, lpEntryID,
						NULL,//default interface
						MAPI_BEST_ACCESS,
						&ulObjType,
						(LPUNKNOWN*)&pMessage);

	ASSERT(ulObjType == 5);
	ASSERT(pMessage);
	if (!pMessage)
		return NULL;

	CXTPCalendarEventPtr ptrEvent = ImportEvent(pMessage);

	MAPI_RELEASE(this, pMessage);

	return ptrEvent;
}


CXTPCalendarEventPtr CXTPCalendarMAPIDataProvider::ImportEvent(LPMESSAGE pMessage)
{
	ASSERT(pMessage);
	if (!pMessage)
		return NULL;

	//-----------------------------------------------------------------------
	CByteArray arSearchKey;
	HRESULT hrID = _getPropVal(pMessage, PR_SEARCH_KEY, arSearchKey);
	if (FAILED(hrID))
	{
		ASSERT(FALSE);
		return NULL;
	}

	DWORD dwEventID = GetEventID((int)arSearchKey.GetSize(),
								 (LPENTRYID)arSearchKey.GetData(), TRUE);

	//============================================================
	CXTPCalendarEventPtr ptrEvent(CreateNewEvent(dwEventID));

	// ------- Is Recurrence --------
	int nIsRecurrence = _getPropVal_int(pMessage, xtpMAPIpropEvent_IsRecuring);

	// get TimeZone
	TIME_ZONE_INFORMATION tziEvent;
	GetTimeZoneInformation(&tziEvent);

	if (nIsRecurrence)
	{
		ULONG ulTZTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_TimeZone);
		CByteArray arTZIData;
		HRESULT hrTZ = _getPropVal(pMessage, ulTZTag, arTZIData);

		if (SUCCEEDED(hrTZ))
		{
			xtp_GetWinTZInfo(arTZIData, &tziEvent);
		}
	}

	XTP_TRACE_READ_EVENTS(_T("\nEvent:------------------\n"));

	// ------- Subject --------
	CString strTmp = _getPropVal_str(pMessage, PR_SUBJECT);
	ptrEvent->SetSubject(strTmp);
	XTP_TRACE_READ_EVENTS(_T("Subject: %s\n"), strTmp);

	// ------- Body --------
	strTmp = _getPropVal_str(pMessage, PR_BODY);
	ptrEvent->SetBody(strTmp);
	XTP_TRACE_READ_EVENTS(_T("Body: %s\n"), strTmp.Left(255));

	// ------- StartTime --------
	SYSTEMTIME stUTC = _getPropVal_UtcTime(pMessage, xtpMAPIpropEvent_StartTime);
	//TRACE(_T("StartTime orig: %s\n"), dtTmp.Format());
	COleDateTime dtTmp = xtp_UtcToTzTime(&tziEvent, stUTC);
	ptrEvent->SetStartTime(dtTmp);
	XTP_TRACE_READ_EVENTS(_T("StartTime tz  : %s\n"), dtTmp.Format());

	// ------- EndTime --------
	stUTC = _getPropVal_UtcTime(pMessage, xtpMAPIpropEvent_EndTime);
	dtTmp = xtp_UtcToTzTime(&tziEvent, stUTC);
	ptrEvent->SetEndTime(dtTmp);
	//XTP_TRACE_READ_EVENTS(_T("EndTime: %s\n"), ptrEvent->GetEndTime().Format());

	// ------- LOCATION --------
	strTmp = _getPropVal_str(pMessage, xtpMAPIpropEvent_Location);
	ptrEvent->SetLocation(strTmp);
	XTP_TRACE_READ_EVENTS(_T("LOCATION: %s\n"), strTmp.Left(255));

	// ------- AllDayEvent --------
	int nTmp = _getPropVal_int(pMessage, xtpMAPIpropEvent_AllDay);
	ptrEvent->SetAllDayEvent(nTmp != 0);
	XTP_TRACE_READ_EVENTS(_T("All day: %d\n"), nTmp);

	// ------- IsReminder --------
	nTmp = _getPropVal_int(pMessage, xtpMAPIpropCommon_ReminderSet);
	ptrEvent->SetReminder(nTmp);

	// ------- ReminderMinutesBeforeStart --------
	nTmp = _getPropVal_int(pMessage, xtpMAPIpropCommon_ReminderMinutesBefore);
	ptrEvent->SetReminderMinutesBeforeStart(nTmp);

	// ------- BusyStatus --------
	nTmp = _getPropVal_int(pMessage, xtpMAPIpropEvent_BusyStatus);
	ptrEvent->SetBusyStatus(nTmp);

	// ------- Importance --------
	nTmp = _getPropVal_int(pMessage, PR_IMPORTANCE);
	ptrEvent->SetImportance(nTmp);

	// ------- Label --------
	nTmp = _getPropVal_int(pMessage, xtpMAPIpropEvent_Color);
	ptrEvent->SetLabelID(nTmp);

	// ------- Meeting --------
	nTmp = _getPropVal_int(pMessage, xtpMAPIpropEvent_MeetingStatus);
	ptrEvent->SetMeeting(nTmp != 0);

	// ------- Private --------
	nTmp = _getPropVal_int(pMessage, xtpMAPIpropCommon_IsPrivate);
	ptrEvent->SetPrivate(nTmp != 0);

	// ------- CreationTime --------
	stUTC = _getPropVal_UtcTime(pMessage, PR_CREATION_TIME);
	dtTmp = xtp_UtcToTzTime(&tziEvent, stUTC);
	ptrEvent->SetCreationTime(dtTmp);

	// ------- LastModificationTime --------
	stUTC = _getPropVal_UtcTime(pMessage, PR_LAST_MODIFICATION_TIME);
	dtTmp = xtp_UtcToTzTime(&tziEvent, stUTC);
	ptrEvent->SetLastModificationTime(dtTmp);

	// ------- Custom Properties -------
	CXTPCalendarCustomProperties* pProps = ptrEvent->GetCustomProperties();
	if (pProps)
	{
		strTmp = _getPropVal_str(pMessage, xtpMAPIpropEvent_CustomProperties);
		pProps->LoadFromXML(strTmp);

		_SetEventRuntimeProps(pProps, pMessage);
	}

	// nIsRecurrence
	if (nIsRecurrence)
	{
		ULONG ulPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_RecurrenceState);
		CByteArray arRecData;
		HRESULT hr = _getPropVal(pMessage, ulPropTag, arRecData);
		ASSERT(SUCCEEDED(hr));

		VERIFY(ptrEvent->MakeEventAsRecurrence());
		ImportRecurrence(ptrEvent, arRecData);
	}
	// -------

	return ptrEvent;
}

int CXTPCalendarMAPIDataProvider::GetEventGlobalPropVal_int(LPCTSTR pcszPropName, int nDefault)
{
	CXTPCalendarCustomProperties* pProps = GetCustomProperties();

	ASSERT(pProps);
	if (!pProps)
		return nDefault;

	COleVariant varValue;
	BOOL bRes = pProps->GetProperty(pcszPropName, varValue);
	if (!bRes)
		return nDefault;

	if (varValue.vt != VT_I4)
	{
		if (FAILED(::VariantChangeType(&varValue, &varValue, 0, VT_I4)))
			return nDefault;
	}
	return V_I4(&varValue);
}

CString CXTPCalendarMAPIDataProvider::GetEventGlobalPropVal_str(LPCTSTR pcszPropName, LPCTSTR pcszDefault)
{
	CXTPCalendarCustomProperties* pProps = GetCustomProperties();

	ASSERT(pProps);
	if (!pProps)
		return pcszDefault;

	COleVariant varValue;
	BOOL bRes = pProps->GetProperty(pcszPropName, varValue);
	if (!bRes)
		return pcszDefault;

	if (varValue.vt != VT_BSTR)
	{
		if (FAILED(::VariantChangeType(&varValue, &varValue, 0, VT_BSTR)))
			return pcszDefault;
	}

	return CString(V_BSTR(&varValue));
}

void CXTPCalendarMAPIDataProvider::_SetMAPIEventGlobalPropsIfNeed(LPMESSAGE pMessage,
												   CXTPCalendarEvent* pEvent)
{
	if (!pMessage || !pEvent)
	{
		ASSERT(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	int nPropTag;

	//**** Common customizable properties
	int nRState = pEvent->GetRecurrenceState();
	int nApptIcon = (nRState == xtpCalendarRecurrenceMaster) ? cnMAPIpropVal_AppointmentIconRecurr :
									cnMAPIpropVal_AppointmentIcon;

	VERIFY(_setPropTagVal_int(pMessage, XTP_PR_MAPI_EVENT_ICON_INDEX, nApptIcon));

	//---------------------------------------
	CString strMsgClass = GetEventGlobalPropVal_str(cszMAPIpropName_AppointmentMessageClass,
								cszMAPIpropVal_AppointmentMessageClass);
	VERIFY(_setPropTagVal_str(pMessage, PR_MESSAGE_CLASS_A, strMsgClass));

	//---------------------------------------
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropCommon_OutlookInternalVersion);

	int nOutlookVer = _getPropVal_int(pMessage, nPropTag);
	if (nOutlookVer == 0)
	{
		nOutlookVer = GetEventGlobalPropVal_int(cszMAPIpropName_OutlookInternalVersion,
							cnMAPIpropVal_OutlookInternalVersionDef);
		VERIFY(_setPropTagVal_int(pMessage, nPropTag, nOutlookVer));
	}

	//---------------------------------------
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropCommon_OutlookVersion);
	CString strOutlookVer = _getPropVal_str(pMessage, nPropTag);
	if (strOutlookVer.IsEmpty())
	{
		strOutlookVer = GetEventGlobalPropVal_str(cszMAPIpropName_OutlookVersion,
								cszMAPIpropVal_OutlookVersionDef);
		VERIFY(_setPropTagVal_str(pMessage, nPropTag, strOutlookVer));
	}
}

BOOL CXTPCalendarMAPIDataProvider::UpdateMAPIEvent(LPMESSAGE pMessage,
												   CXTPCalendarEvent* pEvent)
{
	if (!pMessage || !pEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ------- Is Recurrence --------
	int nRState = pEvent->GetRecurrenceState();

	int nIsRecurrence = (nRState == xtpCalendarRecurrenceMaster);

	if (nRState != xtpCalendarRecurrenceNotRecurring &&
		nRState != xtpCalendarRecurrenceMaster)
	{
		ASSERT(FALSE);
		return TRUE;
	}

	// get TimeZone
//  TIME_ZONE_INFORMATION tziEvent;
//  GetTimeZoneInformation(&tziEvent);

	int nPropTag;
	//-------------------

	//**** Common customizable properties
	_SetMAPIEventGlobalPropsIfNeed(pMessage, pEvent);
	//****

	// ------- Subject --------
	CString strTmp = pEvent->GetSubject();
	VERIFY(_setPropTagVal_str(pMessage, PR_SUBJECT_A, strTmp));

	// ------- Body --------
	strTmp = pEvent->GetBody();
	VERIFY(_setPropTagVal_str(pMessage, PR_BODY_A, strTmp));

	// ------- StartTime --------
	COleDateTime dtTmp = pEvent->GetStartTime();
	SYSTEMTIME stUTCstart = xtp_TimeToUtc(dtTmp);
	ASSERT(stUTCstart.wYear);
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_StartTime);
	VERIFY(_setPropTagVal_UtcTime(pMessage, nPropTag, stUTCstart));

	// ------- EndTime --------
	dtTmp = pEvent->GetEndTime();
	SYSTEMTIME stUTCend = xtp_TimeToUtc(dtTmp);
	ASSERT(stUTCend.wYear);
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_EndTime);
	VERIFY(_setPropTagVal_UtcTime(pMessage, nPropTag, stUTCend));

	// ------- LOCATION --------
	strTmp = pEvent->GetLocation();
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_Location);
	VERIFY(_setPropTagVal_str(pMessage, nPropTag, strTmp));

	// ------- AllDayEvent --------
	int nTmp = pEvent->IsAllDayEvent();
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_AllDay);
	VERIFY(_setPropTagVal_int(pMessage, nPropTag, nTmp));

	// ------- IsReminder --------
	nTmp = pEvent->IsReminder();
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropCommon_ReminderSet);
	VERIFY(_setPropTagVal_int(pMessage, nPropTag, nTmp));

	// ------- ReminderMinutesBeforeStart --------
	nTmp = pEvent->GetReminderMinutesBeforeStart();
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropCommon_ReminderMinutesBefore);
	VERIFY(_setPropTagVal_int(pMessage, nPropTag, nTmp));

	// ------- ReminderDate --------
	ULONG nPTagRmdDate = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropCommon_ReminderDate);

	if (pEvent->IsReminder())
	{
		VERIFY(_setPropTagVal_UtcTime(pMessage, nPTagRmdDate, stUTCstart)); // always equal to start time
	}
	else
	{
		_deletePropTag(pMessage, nPTagRmdDate);
	}

	// ------- BusyStatus --------
	nTmp = pEvent->GetBusyStatus();
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_BusyStatus);
	VERIFY(_setPropTagVal_int(pMessage, nPropTag, nTmp));

	// ------- Importance --------
	nTmp = pEvent->GetImportance();
	VERIFY(_setPropTagVal_int(pMessage, PR_IMPORTANCE, nTmp));

	// ------- Label --------
	nTmp = pEvent->GetLabelID();
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_Color);
	VERIFY(_setPropTagVal_int(pMessage, nPropTag, nTmp));

	// ------- Meeting --------
	nTmp = pEvent->IsMeeting();
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_MeetingStatus);
	VERIFY(_setPropTagVal_int(pMessage, nPropTag, nTmp));

	// ------- Private --------
	nTmp = pEvent->IsPrivate();
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropCommon_IsPrivate);
	VERIFY(_setPropTagVal_int(pMessage, nPropTag, nTmp));
	VERIFY(_setPropTagVal_int(pMessage, PR_SENSITIVITY, nTmp ? SENSITIVITY_PRIVATE : SENSITIVITY_NONE));

	// ------- CreationTime --------
	dtTmp = pEvent->GetCreationTime();
	SYSTEMTIME stUTC = xtp_TimeToUtc(dtTmp);
	ASSERT(stUTC.wYear);
	VERIFY(_setPropTagVal_UtcTime(pMessage, PR_CREATION_TIME, stUTC));

	// ------- LastModificationTime --------
	dtTmp = pEvent->GetLastModificationTime();
	stUTC = xtp_TimeToUtc(dtTmp);
	ASSERT(stUTC.wYear);
	VERIFY(_setPropTagVal_UtcTime(pMessage, PR_LAST_MODIFICATION_TIME, stUTC));

	// -------- Custom Properties -------
	CXTPCalendarCustomProperties* pProps = pEvent->GetCustomProperties();
	if (pProps)
	{
		CXTPCalendarCustomProperties tmpStorage;
		_MoveMAPIEventRuntimeProps(&tmpStorage, pProps);

		pProps->SaveToXML(strTmp);

		nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_CustomProperties);
		VERIFY(_setPropTagVal_str(pMessage, nPropTag, strTmp));

		_MoveMAPIEventRuntimeProps(pProps, &tmpStorage);
	}

	// nIsRecurrence
	nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_IsRecuring);
	VERIFY(_setPropTagVal_int(pMessage, nPropTag, nIsRecurrence));

	if (nIsRecurrence)
	{
		CByteArray arRecData;
		CXTPCalendarMAPI_Recurrence::CXTPRcData RCData;
		CXTPCalendarRecurrencePatternPtr ptrPattern = pEvent->GetRecurrencePattern();

		BOOL bRes = CXTPCalendarMAPI_Recurrence::FillRCData(RCData, ptrPattern, pEvent);
		ASSERT(bRes);
		if (bRes)
		{
			bRes = CXTPCalendarMAPI_Recurrence::RCDataToBin(RCData, arRecData);
			ASSERT(bRes);
			if (bRes)
			{
				nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_RecurrenceState);
				VERIFY(_setPropTagVal_bin(pMessage, nPropTag, arRecData));
			}
		}

		//---------------------------------------------------------------------
		TIME_ZONE_INFORMATION tziEvent;
		GetTimeZoneInformation(&tziEvent);

		CByteArray arTZIData;
		xtp_GetMapiTZInfo(&tziEvent, arTZIData);

		ULONG ulTZTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_TimeZone);
		VERIFY(_setPropTagVal_bin(pMessage, ulTZTag, arTZIData));

		//- recurrence Start/end ----------------------------------------------
		nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_RecurrenceStart);
		VERIFY(_setPropTagVal_UtcTime(pMessage, nPropTag, stUTCstart));

		nPropTag = m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_RecurrenceEnd);
		VERIFY(_setPropTagVal_UtcTime(pMessage, nPropTag, stUTCend));
	}
	else
	{
		CUIntArray arPropsTags;
		arPropsTags.Add( (ULONG)m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_RecurrenceState));
		arPropsTags.Add( (ULONG)m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_RecurrenceStart));
		arPropsTags.Add( (ULONG)m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_RecurrenceEnd));
		arPropsTags.Add( (ULONG)m_MapiHelper.GetPropTagByID(pMessage, xtpMAPIpropEvent_TimeZone));

		_deletePropsTags(pMessage, arPropsTags);
	}

	// -------

	return TRUE;
}

UINT CXTPCalendarMAPIDataProvider::_getStreamSize(IStream* pStream)
{
	ASSERT(pStream);
	if (!pStream)
	{
		return 0;
	}

	ULARGE_INTEGER  uliSize = {0, 0};
	LARGE_INTEGER   liZero = {0, 0};

	HRESULT hr = pStream->Seek(liZero, STREAM_SEEK_END, &uliSize);
	if (FAILED(hr))
		return 0;

	hr = pStream->Seek(liZero, STREAM_SEEK_SET, NULL);
	if (FAILED(hr))
	{
		return 0;
	}
	return uliSize.u.LowPart;
}

int CXTPCalendarMAPIDataProvider::_getSimpleMAPITypeSize(int nType)
{
	switch(nType)
	{
	case PT_I2:     return 2;       /* Signed 16-bit value */
	case PT_LONG:   return 4;       /* Signed 32-bit value */
	case PT_R4:     return 4;       /* 4-byte floating point */
	case PT_DOUBLE:     return 8;   /* Floating point double */
	case PT_CURRENCY:   return 8;   /* Signed 64-bit int (decimal w/    4 digits right of decimal pt) */
	case PT_APPTIME:    return sizeof(double);  /* Application time */
	case PT_ERROR:      return 4;   /* 32-bit error value */
	case PT_BOOLEAN:    return 2;   /* 16-bit boolean (non-zero true) */
	case PT_I8:         return 8;   /* 8-byte signed integer */
	case PT_SYSTIME:    return sizeof(FILETIME);    /* FILETIME 64-bit int w/ number of 100ns periods since Jan 1,1601 */
	case PT_CLSID:      return sizeof(GUID);        /* OLE GUID */

	case PT_OBJECT:     /* Embedded object in a property */

	case PT_STRING8:    /* Null terminated 8-bit character string */
	case PT_UNICODE:    /* Null terminated Unicode string */
	case PT_BINARY:     /* Uninterpreted (counted byte array) */
		return 0;
	default:
		ASSERT(FALSE);
	}
	return 0;
}

BOOL CXTPCalendarMAPIDataProvider::_setPropTagVal_int(LPMESSAGE pMessage,
									  ULONG ulPropTag, int nValue)
{
	int nType = PROP_TYPE(ulPropTag);
	int nTypeSize = _getSimpleMAPITypeSize(nType);

	if (nTypeSize <= 0 || nTypeSize > 4)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	SPropValue PropVal;
	ZeroMemory(&PropVal, sizeof(PropVal));

	PropVal.ulPropTag = ulPropTag;

	switch(nType)
	{
	case PT_I2:
		PropVal.Value.i = (short int)nValue;
		break;
	case PT_LONG:           /* Signed 32-bit value */
		PropVal.Value.l = (long)nValue;
		break;
	case PT_ERROR:      /* 32-bit error value */
		PropVal.Value.err = (SCODE)nValue;
		break;
	case PT_BOOLEAN:        /* 16-bit boolean (non-zero true) */
		PropVal.Value.b = (unsigned short int)nValue;
		break;

//  case PT_R4:     return 4;       /* 4-byte floating point */
//  case PT_DOUBLE:     return 8;   /* Floating point double */
//  case PT_CURRENCY:   return 8;   /* Signed 64-bit int (decimal w/    4 digits right of decimal pt) */
//  case PT_APPTIME:    return sizeof(double);  /* Application time */

//  case PT_I8:         return 8;   /* 8-byte signed integer */
//  case PT_SYSTIME:    return sizeof(FILETIME);    /* FILETIME 64-bit int w/ number of 100ns periods since Jan 1,1601 */
//  case PT_CLSID:      return sizeof(GUID);        /* OLE GUID */

//  case PT_OBJECT:     /* Embedded object in a property */

//  case PT_STRING8:    /* Null terminated 8-bit character string */
//  case PT_UNICODE:    /* Null terminated Unicode string */
//  case PT_BINARY:     /* Uninterpreted (counted byte array) */
//      return 0;
	default:
		ASSERT(FALSE);
		return FALSE;
	}

	HRESULT hRes = this->HrSetOneProp(pMessage, &PropVal);
	return SUCCEEDED(hRes);
}

BOOL CXTPCalendarMAPIDataProvider::_setPropTagVal_str(LPMESSAGE pMessage,
									  ULONG ulPropTag, LPCTSTR pcszValue)
{
	int nType = PROP_TYPE(ulPropTag);

	LPSPropValue pPropVal = NULL;

	if (nType == PT_STRING8) /* Null terminated 8-bit character string */
	{
		int nStrBuffSize = (int)_tcslen(pcszValue) * sizeof(TCHAR) + 2;
		int nBufSize = sizeof(SPropValue) + nStrBuffSize + 4;

		SCODE sc = this->MAPIAllocateBuffer(nBufSize, (void**)&pPropVal);
		if (sc != S_OK || !pPropVal)
		{
			return FALSE;
		}
		ZeroMemory(pPropVal, nBufSize);
		pPropVal->ulPropTag = ulPropTag;
		pPropVal->Value.lpszA = (LPSTR)(pPropVal+1);

		WCSTOMBS_S(pPropVal->Value.lpszA, pcszValue, nStrBuffSize);
	}
	else if (nType == PT_UNICODE) /* Null terminated Unicode string */
	{
		ASSERT(FALSE); //not implemented
		return FALSE;
	}
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}

	HRESULT hRes = this->HrSetOneProp(pMessage, pPropVal);

	MAPI_FREEBUFFER(this, pPropVal);

	return SUCCEEDED(hRes);
}

BOOL CXTPCalendarMAPIDataProvider::_setPropTagVal_UtcTime(LPMESSAGE pMessage,
									  ULONG ulPropTag, const SYSTEMTIME& stTime)
{
	int nType = PROP_TYPE(ulPropTag);

	SPropValue PropVal;
	ZeroMemory(&PropVal, sizeof(SPropValue));

	if (nType == PT_SYSTIME) // FILETIME
	{
		PropVal.ulPropTag = ulPropTag;

		FILETIME ft;
		ZeroMemory(&ft, sizeof(ft));

		if (!SystemTimeToFileTime(&stTime, &ft))
		{
			ASSERT(FALSE);
			return FALSE;
		}
		PropVal.Value.ft = ft;
	}
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}

	HRESULT hRes = this->HrSetOneProp(pMessage, &PropVal);
	return SUCCEEDED(hRes);
}

BOOL CXTPCalendarMAPIDataProvider::_setPropTagVal_bin(LPMESSAGE pMessage,
									ULONG ulPropTag, const CByteArray& arData)
{
	int nType = PROP_TYPE(ulPropTag);

	SPropValue PropVal;
	ZeroMemory(&PropVal, sizeof(SPropValue));

	if (nType == PT_BINARY)
	{
		PropVal.ulPropTag = ulPropTag;
		PropVal.Value.bin.cb = (ULONG)arData.GetSize();
		PropVal.Value.bin.lpb = (BYTE*)arData.GetData();
	}
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}

	HRESULT hRes = this->HrSetOneProp(pMessage, &PropVal);

	return SUCCEEDED(hRes);
}

HRESULT CXTPCalendarMAPIDataProvider::_getPropVal(LPMESSAGE pMessage,
												  ULONG ulPropTag, CByteArray& rData)
{
	rData.RemoveAll();

	LPSTREAM pStream = NULL;
	LPSPropValue pProp = NULL;

	HRESULT hRes = this->HrGetOneProp(pMessage, ulPropTag, &pProp);

	if (hRes == MAPI_E_NOT_ENOUGH_MEMORY)
	{
		hRes = pMessage->OpenProperty(ulPropTag, &IID_IStream, STGM_READ,
										NULL, (LPUNKNOWN*)&pStream);

		if (FAILED(hRes) || !pStream)
		{
			hRes = FAILED(hRes) ? hRes : E_FAIL;
		}
		else
		{
			UINT nDataSize = _getStreamSize(pStream);
			if (!nDataSize)
			{
				hRes = E_FAIL;
			}
			else
			{
				rData.SetSize((int)nDataSize);

				ULONG nDataSize2 = 0;
				hRes = pStream->Read(rData.GetData(), nDataSize, &nDataSize2);

				ASSERT(nDataSize == nDataSize2);
				rData.SetSize((int)nDataSize2);
			}
			pStream->Release();
		}

	}
	else if (SUCCEEDED(hRes))
	{
		int nType = PROP_TYPE(pProp->ulPropTag);
		int nTypeSize = _getSimpleMAPITypeSize(nType);

		void* pData_src = NULL;
		if (nTypeSize > 0)
		{
			pData_src = (void*)&pProp->Value;
		}
		else
		{
			if (nType == PT_STRING8) /* Null terminated 8-bit character string */
			{
				pData_src = (void*)pProp->Value.lpszA;
				nTypeSize = (int)strlen(pProp->Value.lpszA) + 1;
			}
			else if (nType == PT_UNICODE) /* Null terminated Unicode string */
			{
				pData_src = (void*)pProp->Value.lpszW;
				nTypeSize = (int)wcslen(pProp->Value.lpszW) * 2 + 2;
			}
			else if (nType == PT_BINARY) /* Uninterpreted (counted byte array) */
			{
				pData_src = (void*)pProp->Value.bin.lpb;
				nTypeSize = pProp->Value.bin.cb;
			}
		}

		rData.SetSize(nTypeSize);
		MEMCPY_S(rData.GetData(), pData_src, nTypeSize);
	}

	MAPI_FREEBUFFER(this, pProp);

	return hRes;
}

CString CXTPCalendarMAPIDataProvider::_getPropVal_str(LPMESSAGE pMessage,
													  ULONG ulPropTag)
{
	CString str;
	CByteArray arData;

	int nType = PROP_TYPE(ulPropTag);
	ASSERT(nType == PT_STRING8 || nType == PT_UNICODE);

	HRESULT hr = _getPropVal(pMessage, ulPropTag, arData);

	if (FAILED(hr) || arData.GetSize() == 0)
	{
		return str;
	}

	if (nType == PT_STRING8) /* Null terminated 8-bit character string */
	{
		str = (LPCSTR)arData.GetData();
	}
	else if (nType == PT_UNICODE) /* Null terminated Unicode string */
	{
		str = (LPCWSTR)arData.GetData();
	}
	return str;
}

int CXTPCalendarMAPIDataProvider::_getPropVal_int(LPMESSAGE pMessage, ULONG ulPropTag)
{
	CByteArray arData;

	int nType = PROP_TYPE(ulPropTag);
	ASSERT( nType == PT_I2 ||       nType == PT_LONG ||
			nType == PT_ERROR ||    nType == PT_BOOLEAN ||
			nType == PT_I8);

	HRESULT hr = _getPropVal(pMessage, ulPropTag, arData);

	if (FAILED(hr) || arData.GetSize() == 0)
	{
		return 0;
	}

	int nSize = (int)arData.GetSize();
	int nValue = 0;

	ASSERT(sizeof(ULONGLONG) == 8);

	if (nSize == 1)
	{
		nValue = (int)arData[0];
	}
	else if (nSize == 2)
	{
		nValue = (int)*((WORD*)arData.GetData());
	}
	else if (nSize == 3)
	{
		nValue = (int)RGB(arData[0], arData[1], arData[2]);
	}
	else if (nSize == 4)
	{
		nValue = (int)*((DWORD*)arData.GetData());
	}
	else if (nSize == 8)
	{
		nValue = (int)*((ULONGLONG*)arData.GetData());
	}
	else
	{
		ASSERT(FALSE);
	}

	return nValue;
}

SYSTEMTIME CXTPCalendarMAPIDataProvider::_getPropVal_UtcTime(LPMESSAGE pMessage,
													   ULONG ulPropTag)
{
	SYSTEMTIME stUTCTime, stUTCTime0;
	ZeroMemory(&stUTCTime, sizeof(stUTCTime));
	ZeroMemory(&stUTCTime0, sizeof(stUTCTime0));

	if (ulPropTag == 0)
	{
		return stUTCTime0;
	}

	int nType = PROP_TYPE(ulPropTag);

	if (nType != PT_SYSTIME)
	{
		ASSERT(FALSE);
		return stUTCTime0;
	}

	CByteArray arData;
	HRESULT hr = _getPropVal(pMessage, ulPropTag, arData);

	if (FAILED(hr) || arData.GetSize() == 0)
	{
		return stUTCTime0;
	}
	ASSERT(arData.GetSize() == sizeof(FILETIME));

	FILETIME* pUTCTime = ((FILETIME*)arData.GetData());

	if (FileTimeToSystemTime(pUTCTime, &stUTCTime))
	{
		return stUTCTime;
	}

	return stUTCTime0;
}

CString CXTPCalendarMAPIDataProvider::_getPropVal_str(LPMESSAGE pMessage,
										const XTP_MAPI_PROP_NAME& propNameEx)
{
	ULONG ulPropTag = m_MapiHelper.GetPropTagByID(pMessage, propNameEx);

	return _getPropVal_str(pMessage, ulPropTag);
}

int CXTPCalendarMAPIDataProvider::_getPropVal_int(LPMESSAGE pMessage,
										const XTP_MAPI_PROP_NAME& propNameEx)
{
	ULONG ulPropTag = m_MapiHelper.GetPropTagByID(pMessage, propNameEx);

	return _getPropVal_int(pMessage, ulPropTag);
}

SYSTEMTIME CXTPCalendarMAPIDataProvider::_getPropVal_UtcTime(LPMESSAGE pMessage,
										const XTP_MAPI_PROP_NAME& propNameEx)
{
	ULONG ulPropTag = m_MapiHelper.GetPropTagByID(pMessage, propNameEx);

	return _getPropVal_UtcTime(pMessage, ulPropTag);
}

HRESULT CXTPCalendarMAPIDataProvider::_deletePropTag(LPMESSAGE pMessage, ULONG ulPropTag)
{
	CUIntArray arPropsTags;
	arPropsTags.Add(ulPropTag);

	return _deletePropsTags(pMessage, arPropsTags);
}

HRESULT CXTPCalendarMAPIDataProvider::_deletePropsTags(LPMESSAGE pMessage, CUIntArray& arPropsTags)
{
	if (!pMessage || arPropsTags.GetSize() == 0)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	int nTagsCount = (int)arPropsTags.GetSize();
	SPropTagArray* pPorpTagsArray = NULL;

	int nBufSize = sizeof(SPropTagArray) + nTagsCount * sizeof(ULONG) + 4;
	SCODE sc = this->MAPIAllocateBuffer(nBufSize, (void**)&pPorpTagsArray);
	if (sc != S_OK || !pPorpTagsArray)
	{
		return E_OUTOFMEMORY;
	}

	//---------------------------------------------
	pPorpTagsArray->cValues = nTagsCount;
	for (int i = 0; i < nTagsCount; i++)
	{
		pPorpTagsArray->aulPropTag[i] = (ULONG)arPropsTags[i];
	}

	HRESULT hrDelete = pMessage->DeleteProps(pPorpTagsArray, NULL);

	//---------------------------------------------
	MAPI_FREEBUFFER(this, pPorpTagsArray);

	return hrDelete;
}

void CXTPCalendarMAPIDataProvider::ImportRecurrence(CXTPCalendarEvent* pMasterEvent,
													const CByteArray& arRCData)
{
	ASSERT(pMasterEvent->GetRecurrenceState() == xtpCalendarRecurrenceMaster);

	CXTPCalendarRecurrencePatternPtr ptrPattern = pMasterEvent->GetRecurrencePattern();
	ASSERT(ptrPattern);
	if (!ptrPattern)
	{
		return;
	}

	CXTPCalendarMAPI_Recurrence::CXTPRcData RCData;

	BOOL bRead = CXTPCalendarMAPI_Recurrence::ReadRCData(RCData, arRCData);
	ASSERT(bRead);

	if (!bRead)
	{
		return;
	}
	ASSERT((RCData.wType - xtpMAPIRcType_Daily) >= 0 && (RCData.wType - xtpMAPIRcType_Daily) <= 3);

	CXTPCalendarMAPI_Recurrence::SetRecurrenceOptions(ptrPattern, RCData);

	CXTPCalendarMAPI_Recurrence::SetRecurrenceExceptions(pMasterEvent, ptrPattern, RCData);

	VERIFY( pMasterEvent->UpdateRecurrence(ptrPattern) );
}

CXTPCalendarEventsPtr CXTPCalendarMAPIDataProvider::ImportAllEvents(LPSRestriction lpRestriction)
{
	HRESULT hRes = S_OK;
	LPMAPITABLE lpContentsTable = NULL;
	LPSRowSet pRows = NULL;
	ULONG i;
	CXTPCalendarEventsPtr ptrEvents = new CXTPCalendarEvents();

	static SizedSPropTagArray(1, sptCols) = {1, {PR_ENTRYID} };

	hRes = m_lpCalendarFolder->GetContentsTable(0, &lpContentsTable);

	if (SUCCEEDED(hRes))
	{
		hRes = this->HrQueryAllRows(lpContentsTable, (LPSPropTagArray) &sptCols,
							  lpRestriction, //restriction...current day
							  NULL,//sort order...we're not using this parameter
							  0, &pRows);

		if (SUCCEEDED(hRes))
		{
			for (i = 0; i < pRows->cRows; i++)
			{
				CXTPCalendarEventPtr ptrNextEvent = ImportEvent(&pRows->aRow[i]);
				ASSERT(ptrNextEvent);
				if (ptrNextEvent)
				{
					ptrEvents->Add(ptrNextEvent);
				}
			}
		}
	}

	//-------------------------------------------------------------------------
	if (pRows)
	{
		this->FreeProws(pRows);
	}

	MAPI_RELEASE(this, lpContentsTable);

	return ptrEvents;
}

BOOL CXTPCalendarMAPIDataProvider::ImportNewEvents()
{
	ASSERT(m_pMemDP);
	if (!m_pMemDP)
	{
		return FALSE;
	}

	BOOL bChanged = FALSE;

	HRESULT hRes = S_OK;
	LPMAPITABLE lpContentsTable = NULL;
	LPSRowSet pRows = NULL;
	ULONG i;
	CMap_EventIDs mapExistingEntries;
	CMap_EventIDs mapMissedEntries;

	// Retrieve EntryID's of all items from the MAPI Calendar folder
	static SizedSPropTagArray(2, sptCols) = {2, {PR_ENTRYID, PR_SEARCH_KEY} };

	hRes = m_lpCalendarFolder->GetContentsTable(0, &lpContentsTable);

	if (SUCCEEDED(hRes))
	{
		hRes = this->HrQueryAllRows(lpContentsTable, (LPSPropTagArray) &sptCols,
			NULL, //lpRestriction, //restriction...current day??
			NULL,//sort order...we're not using this parameter
			0, &pRows);

		if (SUCCEEDED(hRes))
		{
			// Iterate all appointment items read from data source
			for (i = 0; i < pRows->cRows; i++)
			{
				// ------- SEARCH_KEY --------
				if (PR_SEARCH_KEY != pRows->aRow[i].lpProps[1].ulPropTag)
				{
					ASSERT(FALSE);
					continue;
				}
				// trying to find this eventID
				DWORD dwEventID = GetEventID(pRows->aRow[i].lpProps[1].Value.bin.cb,
					(LPENTRYID)pRows->aRow[i].lpProps[1].Value.bin.lpb);

				// if not found - import
				if (XTP_CALENDAR_UNKNOWN_EVENT_ID == dwEventID)
				{
					CXTPCalendarEventPtr ptrNextEvent = ImportEvent(&pRows->aRow[i]);
					ASSERT(ptrNextEvent);
					if (ptrNextEvent)
					{
						bChanged = TRUE;
						VERIFY(m_pMemDP->AddEvent(ptrNextEvent));
					}
				}
				else
				{
					// if event found - add its ID into a map
					mapExistingEntries.SetAt(dwEventID, TRUE);
				}
			}

			// Delete all items which exists in internal data storage, but
			// were not just found in the contents table of the external folder.
			m_mapID.FindMissing(mapExistingEntries, mapMissedEntries);
			POSITION pos = mapMissedEntries.GetStartPosition();
			DWORD dwKey;
			BOOL bValue = FALSE;
			while (pos != NULL)
			{
				bChanged = TRUE;
				mapMissedEntries.GetNextAssoc(pos, dwKey, bValue);

				CXTPCalendarEventPtr ptrEventToDel = m_pMemDP->GetEvent(dwKey);
				// ASSERT(ptrEventToDel); // could be NULL if processing a few notifications in a row
				if (ptrEventToDel)
				{
					VERIFY(m_pMemDP->DeleteEvent(ptrEventToDel));
				}
			}
		}
	}

	// Cleanup
	if (pRows)
	{
		this->FreeProws(pRows);
	}
	MAPI_RELEASE(this, lpContentsTable);

	return bChanged;
}

LONG STDAPICALLTYPE CXTPCalendarMAPIDataProvider::MAPICallBack_OnNotify(
	LPVOID          lpvContext,
	ULONG           cNotif,
	LPNOTIFICATION  lpNotif)
{
	BOOL bProcessed = FALSE;
	// get current MAPI data provider
	CXTPCalendarMAPIDataProvider* pThis = (CXTPCalendarMAPIDataProvider*)lpvContext;
	ASSERT(pThis);
	if (!pThis)
		return MAPI_E_INVALID_PARAMETER;

	CXTPCalendarData* pMemDP = pThis->m_pMemDP;

	if (!pMemDP)
	{
		return S_OK;
	}

	//***********************
	SAFE_MANAGE_STATE(pThis->m_pModuleState);
	//***********************

	XTP_TRACE_MAPI_NF(_T("\n MAPICallBack_OnNotify [%s] (notifications count = %d)\n"),
					  COleDateTime::GetCurrentTime().Format(), (int)cNotif);

	for (int i = 0; i < (int)cNotif; i++)
	{
		ULONG ulNfSender = lpNotif[i].info.obj.ulObjType;
		ULONG ulNfType = lpNotif[i].ulEventType;

		XTP_TRACE_MAPI_NF(_T("    NfSender = %d (%s)\n"), (int)ulNfSender,
							ulNfSender == MAPI_FOLDER ? _T("MAPI_FOLDER") : (ulNfSender == MAPI_MESSAGE ? _T("MAPI_MESSAGE") : _T("")) );

		// Check is notification related to active calendar folder ----------
		CString str_dbg_CalRel;
		CXTPMAPIBinary eidObj;
		if (ulNfSender == MAPI_FOLDER)
		{
			eidObj.Set(lpNotif[i].info.obj.cbEntryID, (LPBYTE)lpNotif[i].info.obj.lpEntryID);
			str_dbg_CalRel = _T("this");
		}
		else if (ulNfSender == MAPI_MESSAGE)
		{
			eidObj.Set(lpNotif[i].info.obj.cbParentID, (LPBYTE)lpNotif[i].info.obj.lpParentID);
			str_dbg_CalRel = _T("Parent folder");
		}

		if (!pThis->Equal(eidObj, pThis->m_eidCalendarFolder))
		{
			if (ulNfSender == MAPI_MESSAGE)
			{
				eidObj.Set(lpNotif[i].info.obj.cbOldParentID, (LPBYTE)lpNotif[i].info.obj.lpOldParentID);
				if (!pThis->Equal(eidObj, pThis->m_eidCalendarFolder))
				{
					XTP_TRACE_MAPI_NF(_T("    Calendar is: not related! SKIP. \n"));
					continue;
				}
				else
				{
					str_dbg_CalRel = _T("OLD Parent folder");
					pThis->ImportNewEvents();
					bProcessed = TRUE;
				}
			}
			else
			{
				XTP_TRACE_MAPI_NF(_T("    Calendar is: not related! SKIP. \n"));
				continue;
			}
		}

		XTP_TRACE_MAPI_NF(_T("    Calendar is: %s\n"), str_dbg_CalRel);

		CString str_dbg_NF;
		str_dbg_NF += (ulNfType & fnevObjectCreated) ? _T(" | Created") : _T("");
		str_dbg_NF += (ulNfType & fnevObjectDeleted) ? _T(" | Deleted") : _T("");
		str_dbg_NF += (ulNfType & fnevObjectModified) ?_T(" | Modified") : _T("");
		str_dbg_NF += (ulNfType & fnevObjectMoved)   ? _T(" | Moved") : _T("");
		str_dbg_NF += (ulNfType & fnevObjectCopied)  ? _T(" | Copied") : _T("");

		XTP_TRACE_MAPI_NF(_T("    fnevObject =%s \n"), str_dbg_NF);

		if (bProcessed)
		{
			return SUCCESS_SUCCESS;
		}
		//-------------------------------------------------------------------
		CXTPMAPIBinary keyID = pThis->GetSearchKey(lpNotif[i].info.obj.cbEntryID, lpNotif[i].info.obj.lpEntryID);
		DWORD dwEventID = pThis->GetEventID(keyID.GetBinarySize(), (LPENTRYID)keyID.GetBinaryData());
		CXTPCalendarEventPtr ptrEvent = pMemDP->GetEvent(dwEventID);

		XTP_TRACE_MAPI_NF(_T("    event ID = %d (%s)\n"), dwEventID,
							(LPCTSTR)(ptrEvent ? _T("exists") : _T("non-exists")) );

		// look what type of change was performed
		if (ulNfType & fnevObjectDeleted)
		{
			// Delete the event
			// Note that this message will never be received for deleted item for Exchange 5.5 and 2000.
			// More info at MS KB261172
			if (ptrEvent)
			{
				VERIFY( pMemDP->DeleteEvent(ptrEvent) );
				XTP_TRACE_MAPI_NF(_T("    Action = DELETE\n"));

				//#pragma NOTE("Advice to DP notification and resend")
				//pThis->m_pConnect->SendEvent(XTP_NC_CALENDAREVENTWASDELETED, (WPARAM)dwEventID, (LPARAM)(CXTPCalendarEvent*)ptrEvent);
			}
			continue;
		}

		if ((ulNfType & fnevObjectCreated) && ulNfSender == MAPI_MESSAGE)
		{
			if (dwEventID != XTP_CALENDAR_UNKNOWN_EVENT_ID)
			{
				//ASSERT(FALSE);
				XTP_TRACE_MAPI_NF(_T("    Action = skip\n"));
				return SUCCESS_SUCCESS;
			}

			// Add a new event
			CXTPCalendarEventPtr ptrEvent2 = pThis->ImportEvent(
										lpNotif[i].info.obj.cbEntryID,
										lpNotif[i].info.obj.lpEntryID);

			if (ptrEvent && ptrEvent2)
			{
				//Event already exists
				VERIFY( pMemDP->ChangeEvent(ptrEvent2) );
				XTP_TRACE_MAPI_NF(_T("    Action = CHANGE\n"));
			}
			else if (ptrEvent2)
			{
				VERIFY( pMemDP->AddEvent(ptrEvent2) );
				XTP_TRACE_MAPI_NF(_T("    Action = ADD\n"));
			}

			continue;
		}

		if ((ulNfType & fnevObjectMoved) && ulNfSender == MAPI_MESSAGE)
		{
			CXTPCalendarEventPtr ptrEvent2 = pThis->ImportEvent_FromCalendarFolderOnly(
										lpNotif[i].info.obj.cbEntryID,
										lpNotif[i].info.obj.lpEntryID);

			if (ptrEvent && ptrEvent2)
			{
				//Event was moved inside Calendar folder
				VERIFY( pMemDP->ChangeEvent(ptrEvent2) );
				XTP_TRACE_MAPI_NF(_T("    Action = CHANGE\n"));
			}
			else if (ptrEvent)
			{
				//Event was moved Out of Calendar folder (to "Deleted Items" folder)
				VERIFY( pMemDP->DeleteEvent(ptrEvent) );
				XTP_TRACE_MAPI_NF(_T("    Action = DELETE (moved to other folder)\n"));
			}

			continue;
		}

		if ((ulNfType & fnevObjectModified) && ulNfSender == MAPI_MESSAGE)
		{
			// is Our event changed
			if (ptrEvent)
			{
				// Read new event data
				CXTPCalendarEventPtr ptrEvent2 = pThis->ImportEvent(
											lpNotif[i].info.obj.cbEntryID,
											lpNotif[i].info.obj.lpEntryID);
				ASSERT(ptrEvent2);

				// Update event properties in the cache
				if (ptrEvent2)
				{
					ASSERT(ptrEvent->GetEventID() == ptrEvent2->GetEventID());

					DWORD dwPatternID = ptrEvent->GetRecurrencePatternID();
					ptrEvent2->SetRecurrencePatternID(dwPatternID);

					pMemDP->ChangeEvent(ptrEvent2);
					XTP_TRACE_MAPI_NF(_T("    Action = CHANGE.2\n"));
				}
			}
			continue;
		}
	}

	return SUCCESS_SUCCESS;
}

DWORD CXTPCalendarMAPIDataProvider::GetNextUniqueEventID(DWORD dwEventID)
{
	if (XTP_CALENDAR_UNKNOWN_EVENT_ID == dwEventID)
		dwEventID = GetNextFreeTempID();

	CXTPMAPIBinary eKeyTmp;
	while (m_mapID.Lookup(dwEventID, eKeyTmp))
	{
		dwEventID = GetNextFreeTempID();
	}
	ASSERT(m_mapID.Lookup(dwEventID, eKeyTmp) == FALSE);
	return dwEventID;
}

CXTPMAPIBinary CXTPCalendarMAPIDataProvider::GetSearchKey(ULONG cbEntryID, LPENTRYID lpEntryID)
{
	CXTPMAPIBinary keySearch;

	if (cbEntryID == 0 || !lpEntryID || !m_lpCalendarFolder)
	{
		ASSERT(FALSE);
		return keySearch;
	}

	LPMESSAGE pMessage = NULL;
	ULONG ulObjType = NULL;

	// open event from Calendar Folder storage
	m_lpCalendarFolder->OpenEntry(cbEntryID, lpEntryID,
		NULL,//default interface
		MAPI_BEST_ACCESS,
		&ulObjType,
		(LPUNKNOWN*)&pMessage);

	//ASSERT(ulObjType == 5);
	//ASSERT(pMessage); // CHECK
	if (!pMessage)
	{
		return keySearch;
	}

	// Get SearchKey
	CByteArray arSearchKey;
	HRESULT hrID = _getPropVal(pMessage, PR_SEARCH_KEY, arSearchKey);
	if (SUCCEEDED(hrID))
	{
		keySearch.Set((int)arSearchKey.GetSize(), arSearchKey.GetData());
	}

	MAPI_RELEASE(this, pMessage);

	return keySearch;
}

CXTPMAPIBinary CXTPCalendarMAPIDataProvider::GetEntryID(CXTPMAPIBinary& eSearchKey)
{
	if (eSearchKey.GetBinarySize() == 0 || !eSearchKey.GetBinaryData() || !m_lpCalendarFolder)
	{
		ASSERT(FALSE);
		return eSearchKey;
	}

	HRESULT hRes = S_OK;
	LPMAPITABLE lpContentsTable = NULL;
	LPSRowSet pRows = NULL;
	CXTPMAPIBinary eid;

	LPSRestriction pRestriction = BuildBinaryRestriction(eSearchKey.GetBinarySize(), eSearchKey.GetBinaryData(), PR_SEARCH_KEY);

	static SizedSPropTagArray(1, sptCols) = {1, {PR_ENTRYID} };

	hRes = m_lpCalendarFolder->GetContentsTable(0, &lpContentsTable);

	if (SUCCEEDED(hRes))
	{
		hRes = this->HrQueryAllRows(lpContentsTable, (LPSPropTagArray) &sptCols,
			pRestriction, //restriction...
			NULL,//sort order...we're not using this parameter
			0, &pRows);

		if (SUCCEEDED(hRes) && pRows->cRows > 0)
		{
			// ASSERT(pRows->cRows == 1); // we can get only one item by its Search Key
			ASSERT(PR_ENTRYID == pRows->aRow[0].lpProps[0].ulPropTag); // we asked only for EntryID
			eid.Set(pRows->aRow[0].lpProps[0].Value.bin.cb,
				pRows->aRow[0].lpProps[0].Value.bin.lpb);
		}
	}

	// cleanup
	if (pRows)
	{
		this->FreeProws(pRows);
	}
	MAPI_RELEASE(this, lpContentsTable);

	return eid;
}

DWORD CXTPCalendarMAPIDataProvider::GetEventID(
	ULONG cbSearchKey, LPENTRYID lpSearchKey, BOOL bAddNew)
{
	CXTPMAPIBinary eKey(cbSearchKey, lpSearchKey);

	DWORD dwNextID = XTP_CALENDAR_UNKNOWN_EVENT_ID;

	// lookup a map.
	if (m_mapID.Lookup(eKey, dwNextID))
		return dwNextID;

	// ID not found - add new one if necessary
	if (!bAddNew)
		return XTP_CALENDAR_UNKNOWN_EVENT_ID;

	dwNextID = GetNextUniqueEventID();
	m_mapID.Add(dwNextID, eKey);

	return dwNextID;
}

BOOL CXTPCalendarMAPIDataProvider::Equal(const CXTPMAPIBinary& eid1, const CXTPMAPIBinary& eid2)
{
	BOOL bRes = FALSE;

	// 1st check using simple byte-to-byte comparison
	bRes = CXTPMAPIBinary::IsBinaryEqual(eid1, eid2);
	if (bRes)
		return bRes;

	// 2nd check using CompareEntryIDs method.
	// This is required because EntryID versions can change (i.e. A single
	// provider can support more than one form of entry id) and it is up to
	// a specific provider to decide if two EntryID's compare. The way of
	// doing this is calling the CompareEntryIDs method on the on the
	// appropriate object.
	if (!m_lpMAPISession)
		return bRes;

	ULONG ulRes;
	if (SUCCEEDED(m_lpMAPISession->CompareEntryIDs(
		eid1.m_cb, (LPENTRYID)eid1.GetBinaryData(),
		eid2.m_cb, (LPENTRYID)eid2.GetBinaryData(),
		0, &ulRes)))
	{
		bRes = (BOOL)ulRes;
	}

	return bRes;
}

/////////////////////////////////////////////////////////////////////////////
//
// CXTPMAPIBinary
//

CXTPMAPIBinary::CXTPMAPIBinary()
{
	m_cb = 0;
}

CXTPMAPIBinary::CXTPMAPIBinary(ULONG cbEntryID, LPENTRYID lpEntryID)
{
	Set(cbEntryID, (LPBYTE)lpEntryID);
}

CXTPMAPIBinary::CXTPMAPIBinary(const CXTPMAPIBinary& eid)
{
	m_cb = eid.m_cb;
	m_arBytes.RemoveAll();
	m_arBytes.Append(eid.m_arBytes);
}

CXTPMAPIBinary::~CXTPMAPIBinary()
{
}

CXTPMAPIBinary& CXTPMAPIBinary::operator=(const CXTPMAPIBinary& eid)
{
	m_cb = eid.m_cb;
	m_arBytes.SetSize(m_cb);
	MEMCPY_S(m_arBytes.GetData(), eid.GetBinaryData(), m_cb);

	return *this;
}

ULONG CXTPMAPIBinary::GetBinarySize() const
{
	return m_cb;
}

LPBYTE CXTPMAPIBinary::GetBinaryData() const
{
	return (BYTE*)m_arBytes.GetData();
}

void CXTPMAPIBinary::Set(ULONG cbSize, LPBYTE lpData)
{
	m_cb = cbSize;

	m_arBytes.SetSize(m_cb);
	MEMCPY_S(m_arBytes.GetData(), lpData, m_cb);
}

void CXTPMAPIBinary::Set(LPSPropValue pPropEntryID)
{
	if (!pPropEntryID || pPropEntryID->ulPropTag != PR_ENTRYID)
	{
		ASSERT(FALSE);
		Set(0, NULL);
		return;
	}

	Set(pPropEntryID->Value.bin.cb, pPropEntryID->Value.bin.lpb);
}

BOOL CXTPMAPIBinary::IsBinaryEqual(
	const CXTPMAPIBinary& eid1, const CXTPMAPIBinary& eid2)
{
	if ((eid1.m_cb != eid2.m_cb) ||
		 (eid1.m_arBytes.GetSize() != eid2.m_arBytes.GetSize()) )
		return FALSE;

	return 0 == memcmp(
		eid1.m_arBytes.GetData(),
		eid2.m_arBytes.GetData(),
		eid1.m_arBytes.GetSize() );
}

#ifdef _DEBUG
void CXTPMAPIBinary::DebugPrint()
{
	TRACE(_T("Bin size:%d\t"), m_cb);
	for (int i = 0; i < m_arBytes.GetSize(); i++)
	{
		TRACE(_T("%d "), m_arBytes.GetAt(i));
	}
	TRACE(_T("\n"));
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
// CXTP_ID_Collection
//

CXTP_ID_Collection::CXTP_ID_Collection()
{
	m_mapEventID.InitHashTable(XTP_OBJECT_CACHE_HASH_TABLE_SIZE, FALSE);
	m_mapEntryID.InitHashTable(XTP_OBJECT_CACHE_HASH_TABLE_SIZE, FALSE);
}

BOOL CXTP_ID_Collection::Lookup(
	const DWORD dwEventID, CXTPMAPIBinary& obEntryID)
{
	return m_mapEventID.Lookup(dwEventID, obEntryID);
}

BOOL CXTP_ID_Collection::Lookup(
	CXTPMAPIBinary& obEntryID, DWORD& dwEventID)
{
	return m_mapEntryID.Lookup(obEntryID, dwEventID);
}

BOOL CXTP_ID_Collection::Add(
	const DWORD dwEventID, CXTPMAPIBinary& obEntryID)
{
	m_mapEventID.SetAt(dwEventID, obEntryID);
	m_mapEntryID.SetAt(obEntryID, dwEventID);

	return TRUE;
}

void CXTP_ID_Collection::FindMissing(CMap_EventIDs& mapExisting, CMap_EventIDs& mapMissed)
{
	POSITION pos = m_mapEventID.GetStartPosition();
	DWORD dwKey;
	CXTPMAPIBinary eID;
	BOOL bValue = FALSE;
	while (pos != NULL)
	{
		m_mapEventID.GetNextAssoc(pos, dwKey, eID);

		BOOL bFound = mapExisting.Lookup(dwKey, bValue);
		if (!bFound)
		{
			mapMissed.SetAt(dwKey, TRUE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
//
// CMAPIPropIDMap
//

CXTPCalendarMAPIDataProvider::CMAPIPropIDMap::CMAPIPropIDMap()
	: m_pProvider(NULL)
{
	m_mapPropID2Tag.InitHashTable(199, FALSE);
	m_mapPropTag2ID.InitHashTable(199, FALSE);
}

void CXTPCalendarMAPIDataProvider::CMAPIPropIDMap::_UpdateMaps(LPMESSAGE pMessage)
{
	ASSERT(pMessage);
	if (!pMessage)
		return;

	ULONG ulPropNames = 0;
	LPMAPINAMEID* ppArPropNames = NULL;
	LPSPropTagArray pPropTags = NULL;

	HRESULT hRes = pMessage->GetPropList(0, &pPropTags);

	if (SUCCEEDED(hRes) && pPropTags)
	{
		hRes = pMessage->GetNamesFromIDs(&pPropTags, NULL, MAPI_NO_STRINGS,
										 &ulPropNames, &ppArPropNames);

		if (SUCCEEDED(hRes) && ppArPropNames && ulPropNames)
		{
			ASSERT(pPropTags->cValues == ulPropNames);
			XTP_TRACE_READ_IDS(_T("Prop ID -> Tag (%d)\n"), ulPropNames);

			for (int i = 0; i < (int)ulPropNames; i++)
			{
				if (!ppArPropNames[i])
				{
					continue;
				}
				if (ppArPropNames[i]->ulKind == MNID_ID)
				{
					ULONG ulID = (ULONG)ppArPropNames[i]->Kind.lID;
					ULONG ulTag = pPropTags->aulPropTag[i];

					m_mapPropID2Tag[ulID] = ulTag;
					m_mapPropTag2ID[ulTag] = ulID;

					//if (ulID >= 0x8000)
					{
						XTP_TRACE_READ_IDS(_T("Prop ID -> Tag (%x -> %x)\n"), ulID, ulTag);
					}
				}
			}
		}
	}

	MAPI_FREEBUFFER(m_pProvider, ppArPropNames);
	MAPI_FREEBUFFER(m_pProvider, pPropTags);
}

ULONG CXTPCalendarMAPIDataProvider::CMAPIPropIDMap::_GetPropTagFromID_ex(LPMESSAGE pMessage,
												const XTP_MAPI_PROP_NAME& propNameEx)
{
	ASSERT(pMessage);
	if (!pMessage || !m_pProvider)
		return 0;

	ULONG ulPropTag = 0;

	LPMAPINAMEID pPropName = NULL;
	LPSPropTagArray pPropTag = NULL;

	HRESULT hRes = m_pProvider->MAPIAllocateBuffer(sizeof(MAPINAMEID), (void**)&pPropName);

	if (SUCCEEDED(hRes) && pPropName)
	{
		ZeroMemory(pPropName, sizeof(MAPINAMEID));
		pPropName->lpguid = (LPGUID)&propNameEx.m_GuidPS;
		pPropName->ulKind = MNID_ID;
		pPropName->Kind.lID = propNameEx.m_ulID;

		hRes = pMessage->GetIDsFromNames(1, &pPropName, MAPI_CREATE, &pPropTag);
		if (SUCCEEDED(hRes) && pPropTag && pPropTag->cValues)
		{
			ulPropTag = pPropTag->aulPropTag[0];

			// validate prop type
			ulPropTag = CHANGE_PROP_TYPE(ulPropTag, propNameEx.m_ulType);
		}
	}
	MAPI_FREEBUFFER(m_pProvider, pPropName);
	MAPI_FREEBUFFER(m_pProvider, pPropTag);

	return ulPropTag;
}

ULONG CXTPCalendarMAPIDataProvider::CMAPIPropIDMap::GetPropTagByID(
					LPMESSAGE pMessage, const XTP_MAPI_PROP_NAME& propNameEx)
{
	ULONG ulTag = 0;

	if (!m_mapPropID2Tag.Lookup(propNameEx.m_ulID, ulTag))
	{
		_UpdateMaps(pMessage);

		if (!m_mapPropID2Tag.Lookup(propNameEx.m_ulID, ulTag))
		{
			ulTag = _GetPropTagFromID_ex(pMessage, propNameEx);
			ASSERT(ulTag); // WARNING!
			if (ulTag)
			{
				m_mapPropID2Tag[propNameEx.m_ulID] = ulTag;
				m_mapPropTag2ID[ulTag] = propNameEx.m_ulID;

				return ulTag;
			}
		}
	}

	return ulTag;
}

ULONG CXTPCalendarMAPIDataProvider::CMAPIPropIDMap::GetPropIDByTag(LPMESSAGE pMessage,
																	 ULONG ulPropTag)
{
	ULONG ulID = 0;

	if (!m_mapPropTag2ID.Lookup(ulPropTag, ulID))
	{
		_UpdateMaps(pMessage);
	}
	if (!m_mapPropTag2ID.Lookup(ulPropTag, ulID))
	{
		ulID = 0;
	}
	return ulID;
}
