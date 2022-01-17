// XTPVC50Helpers.h : Visual C++ 5.0 helpers
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#if !defined(__XTPVC50HELPERS_H__)
#define __XTPVC50HELPERS_H__

#if (_MSC_VER >= 1000)
#pragma once
#endif // _MSC_VER >= 1000

class CXTPPushRoutingFrame
{
protected:
	CFrameWnd* pOldRoutingFrame;
	_AFX_THREAD_STATE* pThreadState;

public:
	CXTPPushRoutingFrame(CFrameWnd* pNewRoutingFrame)
	{
		pThreadState = AfxGetThreadState();
		pOldRoutingFrame = pThreadState->m_pRoutingFrame;
		pThreadState->m_pRoutingFrame = pNewRoutingFrame;
	}
	~CXTPPushRoutingFrame()
	{
		pThreadState->m_pRoutingFrame = pOldRoutingFrame;
	}
};

#if (_MSC_VER <= 1100)


class CXTPStringHelper
{
	class CXTPString : public CString {
		friend class CXTPStringHelper;
	};

public:
	static int Remove(CString& strRemove, TCHAR chRemove)
	{
		CXTPString& str = (CXTPString&) strRemove;

		str.CopyBeforeWrite();

		LPTSTR pstrSource = str.m_pchData;
		LPTSTR pstrDest = str.m_pchData;
		LPTSTR pstrEnd = str.m_pchData + str.GetData()->nDataLength;

		while (pstrSource < pstrEnd)
		{
			if (*pstrSource != chRemove)
			{
				*pstrDest = *pstrSource;
				pstrDest = _tcsinc(pstrDest);
			}
			pstrSource = _tcsinc(pstrSource);
		}
		*pstrDest = '\0';
		int nCount = pstrSource - pstrDest;
		str.GetData()->nDataLength -= nCount;

		return nCount;
	}

	static int Find(const CString& strFind, LPCTSTR lpszSub, int nStart)
	{
		CXTPString& str = (CXTPString&) strFind;

		ASSERT(AfxIsValidString(lpszSub));

		int nLength = str.GetData()->nDataLength;
		if (nStart > nLength)
			return -1;

		// find first matching substring
		LPTSTR lpsz = _tcsstr(str.m_pchData + nStart, lpszSub);

		// return -1 for not found, distance from beginning otherwise
		return (lpsz == NULL) ? -1 : (int)(lpsz - str.m_pchData);
	}
	static int Find(const CString& strFind, TCHAR ch, int nStart)
	{
		CXTPString& str = (CXTPString&) strFind;

		int nLength = str.GetData()->nDataLength;
		if (nStart >= nLength)
			return -1;

		// find first single character
		LPTSTR lpsz = _tcschr(str.m_pchData + nStart, (_TUCHAR)ch);

		// return -1 if not found and index otherwise
		return (lpsz == NULL) ? -1 : (int)(lpsz - str.m_pchData);
	}

	static int Replace(CString& strReplace, TCHAR chOld, TCHAR chNew)
	{
		CXTPString& str = (CXTPString&) strReplace;
		int nCount = 0;

		// short-circuit the nop case
		if (chOld != chNew)
		{
			// otherwise modify each character that matches in the string
			str.CopyBeforeWrite();
			LPTSTR psz = str.m_pchData;
			LPTSTR pszEnd = psz + str.GetData()->nDataLength;
			while (psz < pszEnd)
			{
				// replace instances of the specified character only
				if (*psz == chOld)
				{
					*psz = chNew;
					nCount++;
				}
				psz = _tcsinc(psz);
			}
		}
		return nCount;
	}


	static int Replace(CString& strReplace, LPCTSTR lpszOld, LPCTSTR lpszNew)
	{
		CXTPString& str = (CXTPString&) strReplace;
		// can't have empty or NULL lpszOld

		int nSourceLen = str.SafeStrlen(lpszOld);
		if (nSourceLen == 0)
			return 0;
		int nReplacementLen = str.SafeStrlen(lpszNew);

		// loop once to figure out the size of the result string
		int nCount = 0;
		LPTSTR lpszStart = str.m_pchData;
		LPTSTR lpszEnd = str.m_pchData + str.GetData()->nDataLength;
		LPTSTR lpszTarget;
		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL)
			{
				nCount++;
				lpszStart = lpszTarget + nSourceLen;
			}
			lpszStart += lstrlen(lpszStart) + 1;
		}

		// if any changes were made, make them
		if (nCount > 0)
		{
			str.CopyBeforeWrite();

			// if the buffer is too small, just
			//   allocate a new buffer (slow but sure)
			int nOldLength = str.GetData()->nDataLength;
			int nNewLength =  nOldLength + (nReplacementLen-nSourceLen)*nCount;
			if (str.GetData()->nAllocLength < nNewLength || str.GetData()->nRefs > 1)
			{
				CStringData* pOldData = str.GetData();
				LPTSTR pstr = str.m_pchData;
				str.AllocBuffer(nNewLength);
				memcpy(str.m_pchData, pstr, pOldData->nDataLength*sizeof(TCHAR));
				CXTPString::Release(pOldData);
			}
			// else, we just do it in-place
			lpszStart = str.m_pchData;
			lpszEnd = str.m_pchData + str.GetData()->nDataLength;

			// loop again to actually do the work
			while (lpszStart < lpszEnd)
			{
				while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL)
				{
					int nBalance = nOldLength - (lpszTarget - str.m_pchData + nSourceLen);
					memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
						nBalance * sizeof(TCHAR));
					memcpy(lpszTarget, lpszNew, nReplacementLen*sizeof(TCHAR));
					lpszStart = lpszTarget + nReplacementLen;
					lpszStart[nBalance] = '\0';
					nOldLength += (nReplacementLen - nSourceLen);
				}
				lpszStart += lstrlen(lpszStart) + 1;
			}
			ASSERT(str.m_pchData[nNewLength] == '\0');
			str.GetData()->nDataLength = nNewLength;
		}

		return nCount;
	}

	static int Delete(CString& strSource, int nIndex, int nCount)
	{
		CXTPString& str = (CXTPString&) strSource;

		if (nIndex < 0)
			nIndex = 0;
		int nNewLength = str.GetData()->nDataLength;
		if (nCount > 0 && nIndex < nNewLength)
		{
			str.CopyBeforeWrite();
			int nBytesToCopy = nNewLength - (nIndex + nCount) + 1;

			memcpy(str.m_pchData + nIndex,
				str.m_pchData + nIndex + nCount, nBytesToCopy * sizeof(TCHAR));
			str.GetData()->nDataLength = nNewLength - nCount;
		}

		return nNewLength;
	}

	static int Insert(CString& strSource, int nIndex, LPCTSTR pstr)
	{
		if (nIndex < 0)
			nIndex = 0;

		CXTPString& str = (CXTPString&) strSource;

		int nInsertLength = str.SafeStrlen(pstr);
		int nNewLength = str.GetData()->nDataLength;
		if (nInsertLength > 0)
		{
			str.CopyBeforeWrite();
			if (nIndex > nNewLength)
				nIndex = nNewLength;
			nNewLength += nInsertLength;

			if (str.GetData()->nAllocLength < nNewLength)
			{
				CStringData* pOldData = str.GetData();
				LPTSTR pstr = str.m_pchData;
				str.AllocBuffer(nNewLength);
				memcpy(str.m_pchData, pstr, (pOldData->nDataLength+1)*sizeof(TCHAR));
				CXTPString::Release(pOldData);
			}

			// move existing bytes down
			memmove(str.m_pchData + nIndex + nInsertLength,
				str.m_pchData + nIndex,
				(nNewLength-nIndex-nInsertLength+1)*sizeof(TCHAR));
			memmove(str.m_pchData + nIndex,
				pstr, nInsertLength*sizeof(TCHAR));
			str.GetData()->nDataLength = nNewLength;
		}

		return nNewLength;
	}

	static void TrimRight(CString& strSource, LPCTSTR lpszTargetList)
	{
		// find beginning of trailing matches
		// by starting at beginning (DBCS aware)
		CXTPString& str = (CXTPString&) strSource;

		str.CopyBeforeWrite();
		LPTSTR lpsz = str.m_pchData;
		LPTSTR lpszLast = NULL;

		while (*lpsz != '\0')
		{
			if (_tcschr(lpszTargetList, *lpsz) != NULL)
			{
				if (lpszLast == NULL)
					lpszLast = lpsz;
			}
			else
				lpszLast = NULL;
			lpsz = _tcsinc(lpsz);
		}

		if (lpszLast != NULL)
		{
			// truncate at left-most matching character
			*lpszLast = '\0';
			str.GetData()->nDataLength = lpszLast - str.m_pchData;
		}
	}
};

#endif

#define XTP_VC50_HALF_SECOND (1.0/172800.0)
#include <math.h>

class CXTPDateTimeHelper
{
public:
#if (_MSC_VER <= 1200) // Using Visual C++ 5.0, 6.0
	static BOOL TmFromOleDate(DATE dtSrc, struct tm& tmDest)
	{
		const DATE MIN_DATE =               (-657434L);  // about year 100
		const DATE MAX_DATE =               2958465L;    // about year 9999

		static int nMonthDays[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

		// The legal range does not actually span year 0 to 9999.
		if (dtSrc > MAX_DATE || dtSrc < MIN_DATE) // about year 100 to about 9999
			return FALSE;

		long nDays;             // Number of days since Dec. 30, 1899
		long nDaysAbsolute;     // Number of days since 1/1/0
		long nSecsInDay;        // Time in seconds since midnight
		long nMinutesInDay;     // Minutes in day
		long n400Years;         // Number of 400 year increments since 1/1/0
		long n400Century;       // Century within 400 year block (0, 1, 2 or 3)
		long n4Years;           // Number of 4 year increments since 1/1/0
		long n4Day;             // Day within 4 year block

		//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
		long n4Yr;              // Year within 4 year block (0, 1, 2 or 3)
		BOOL bLeap4 = TRUE;     // TRUE if 4 year block includes leap year

		double dblDate = dtSrc; // temporary serial date

		// If a valid date, then this conversion should not overflow
		nDays = (long)dblDate;

		// Round to the second
		dblDate += ((dtSrc > 0.0) ? XTP_VC50_HALF_SECOND : -XTP_VC50_HALF_SECOND);

		nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

		dblDate = fabs(dblDate);
		nSecsInDay = (long)((dblDate - floor(dblDate)) * 86400.);

		// Calculate the day of week (sun = 1, mon = 2...)
		// -1 because 1/1/0 is Sat. +1 because we want 1-based
		tmDest.tm_wday = (int)((nDaysAbsolute - 1) % 7L) + 1;

		// Leap years every 4 yrs except centuries not multiples of 400.
		n400Years = (long)(nDaysAbsolute / 146097L);

		// Set nDaysAbsolute to day within 400-year block
		nDaysAbsolute %= 146097L;

		// -1 because first century has extra day
		n400Century = (long)((nDaysAbsolute - 1) / 36524L);

		// Non-leap century
		if (n400Century != 0)
		{
			// Set nDaysAbsolute to day within century
			nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

			// +1 because 1st 4 year increment has 1460 days
			n4Years = (long)((nDaysAbsolute + 1) / 1461L);

			if (n4Years != 0)
				n4Day = (long)((nDaysAbsolute + 1) % 1461L);
			else
			{
				bLeap4 = FALSE;
				n4Day = (long)nDaysAbsolute;
			}
		}
		else
		{
			// Leap century - not special case!
			n4Years = (long)(nDaysAbsolute / 1461L);
			n4Day = (long)(nDaysAbsolute % 1461L);
		}

		if (bLeap4)
		{
			// -1 because first year has 366 days
			n4Yr = (n4Day - 1) / 365;

			if (n4Yr != 0)
				n4Day = (n4Day - 1) % 365;
		}
		else
		{
			n4Yr = n4Day / 365;
			n4Day %= 365;
		}

		// n4Day is now 0-based day of year. Save 1-based day of year, year number
		tmDest.tm_yday = (int)n4Day + 1;
		tmDest.tm_year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

		// Handle leap year: before, on, and after Feb. 29.
		if (n4Yr == 0 && bLeap4)
		{
			// Leap Year
			if (n4Day == 59)
			{
				/* Feb. 29 */
				tmDest.tm_mon = 2;
				tmDest.tm_mday = 29;
				goto DoTime;
			}

			// Pretend it's not a leap year for month/day comp.
			if (n4Day >= 60)
				--n4Day;
		}

		// Make n4DaY a 1-based day of non-leap year and compute
		//  month/day for everything but Feb. 29.
		++n4Day;

		// Month number always >= n/32, so save some loop time */
		for (tmDest.tm_mon = (n4Day >> 5) + 1;
		n4Day > nMonthDays[tmDest.tm_mon]; tmDest.tm_mon++);

		tmDest.tm_mday = (int)(n4Day - nMonthDays[tmDest.tm_mon-1]);

	DoTime:
		if (nSecsInDay == 0)
			tmDest.tm_hour = tmDest.tm_min = tmDest.tm_sec = 0;
		else
		{
			tmDest.tm_sec = (int)nSecsInDay % 60L;
			nMinutesInDay = nSecsInDay / 60L;
			tmDest.tm_min = (int)nMinutesInDay % 60;
			tmDest.tm_hour = (int)nMinutesInDay / 60;
		}

		return TRUE;
	}

	static BOOL GetAsSystemTime(const COleDateTime& dtTime, SYSTEMTIME& sysTime)
	{
		if (dtTime.GetStatus() != COleDateTime::valid)
			return FALSE;

		struct tm tmTemp;
		if (!TmFromOleDate(dtTime, tmTemp))
			return FALSE;

		sysTime.wYear = (WORD)tmTemp.tm_year;
		sysTime.wMonth = (WORD)tmTemp.tm_mon;
		sysTime.wDayOfWeek = (WORD)(tmTemp.tm_wday - 1);
		sysTime.wDay = (WORD)tmTemp.tm_mday;
		sysTime.wHour = (WORD)tmTemp.tm_hour;
		sysTime.wMinute = (WORD)tmTemp.tm_min;
		sysTime.wSecond = (WORD)tmTemp.tm_sec;
		sysTime.wMilliseconds = 0;

		return TRUE;
	};

	AFX_INLINE static LONG GetTotalDays(const COleDateTimeSpan& spSpan)
	{
		double dSpan = _RoundTime(spSpan);
		return  LONG(dSpan);
	};

	AFX_INLINE static LONG GetTotalHours(const COleDateTimeSpan& spSpan)
	{
		double dSpan = _RoundTime(spSpan);
		LONG nHours = LONG(dSpan * 24);
		return nHours;
	};

	static BOOL AFX_CDECL GetDateTimeCtrlTime(HWND hWnd, COleDateTime& timeDest)
	{
		SYSTEMTIME sysTime;
		BOOL bRetVal = TRUE;

		LRESULT result = ::SendMessage(hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM) &sysTime);
		if (result == GDT_VALID)
		{
			timeDest = COleDateTime(sysTime);
			bRetVal = TRUE;
			ASSERT(timeDest.GetStatus() == COleDateTime::valid);
		}
		else if (result == GDT_NONE)
		{
			timeDest.SetStatus(COleDateTime::null);
			bRetVal = TRUE;
		}
		else
			timeDest.SetStatus(COleDateTime::invalid);
		return bRetVal;
	}

	static BOOL AFX_CDECL SetDateTimeCtrlTime(HWND hWnd, const COleDateTime& timeNew)
	{
		BOOL bRetVal = FALSE;

		// make sure the time isn't invalid
		ASSERT(timeNew.GetStatus() != COleDateTime::invalid);
		ASSERT(::IsWindow(hWnd));

		SYSTEMTIME sysTime;
		WPARAM wParam = GDT_NONE;
		if (timeNew.GetStatus() == COleDateTime::valid &&
			GetAsSystemTime(timeNew, sysTime))
		{
			wParam = GDT_VALID;
		}

		bRetVal = (BOOL) ::SendMessage(hWnd,
			DTM_SETSYSTEMTIME, wParam, (LPARAM) &sysTime);

		return bRetVal;
	}
#endif

	AFX_INLINE static LONG GetTotalMinutes(const COleDateTimeSpan& spSpan)
	{
		double dSpan = _RoundTime(spSpan);
		double dMinutes = dSpan * 24 * 60;
		LONG nMinutes = _DoubleToLONG(dMinutes);
		return nMinutes;
	};

	AFX_INLINE static LONG GetTotalSeconds(const COleDateTimeSpan& spSpan)
	{
		double dSpan = _RoundTime(spSpan);
		double dSeconds= dSpan * 24 * 60 * 60;
		LONG nSeconds = _DoubleToLONG(dSeconds);
		return nSeconds;
	};
protected:
	AFX_INLINE static double _RoundTime(double dTime)
	{
		dTime += dTime < 0 ? -XTP_VC50_HALF_SECOND : XTP_VC50_HALF_SECOND;
		return dTime;
	};
	AFX_INLINE static LONG _DoubleToLONG(double dValue)
	{
		return LONG(dValue > 0 ? min(dValue, double(LONG_MAX)) : max(dValue, double(LONG_MIN)) );
	};
};

AFX_INLINE CString CONSTRUCT_S(LPCSTR lpsz, int nLength) {
	#if (_MSC_VER <= 1100) && defined(_UNICODE) // Using Visual C++ 5.0
		CString str;
		if (nLength != 0)
		{
			LPTSTR lpzsData = str.GetBuffer(nLength);
			int n = ::MultiByteToWideChar(CP_ACP, 0, lpsz, nLength, lpzsData, nLength + 1);
			str.ReleaseBuffer(n >= 0 ? n : -1);
		}
		return str;
	#else
		return CString(lpsz, nLength);
	#endif
}

AFX_INLINE CString CONSTRUCT_S(LPCWSTR lpsz, int nLength) {
	#if (_MSC_VER <= 1100) && !defined(_UNICODE) // Using Visual C++ 5.0
		CString str;
		if (nLength != 0)
		{
			LPTSTR lpzsData = str.GetBuffer(nLength*2);
			int n = ::WideCharToMultiByte(CP_ACP, 0, lpsz, nLength, lpzsData,
				(nLength*2)+1, NULL, NULL);
			str.ReleaseBuffer(n >= 0 ? n : -1);
		}
		return str;
	#else
		return CString(lpsz, nLength);
	#endif
}

AFX_INLINE int REPLACE_S(CString& str, LPCTSTR lpszOld, LPCTSTR lpszNew) {
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
		return CXTPStringHelper::Replace(str, lpszOld, lpszNew);
	#else
		return str.Replace(lpszOld, lpszNew);
	#endif
}
AFX_INLINE int REPLACE_S(CString& str, TCHAR chOld, TCHAR chNew) {
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
		return CXTPStringHelper::Replace(str, chOld, chNew);
	#else
		return str.Replace(chOld, chNew);
	#endif
}
AFX_INLINE int FIND_S(const CString& str, LPCTSTR lpszSub, int nStart) {
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
		return CXTPStringHelper::Find(str, lpszSub, nStart);
	#else
		return str.Find(lpszSub, nStart);
	#endif
}
AFX_INLINE int FIND_S(const CString& str, TCHAR ch, int nStart) {
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
		return CXTPStringHelper::Find(str, ch, nStart);
	#else
		return str.Find(ch, nStart);
	#endif
}
AFX_INLINE int REMOVE_S(CString& str, TCHAR chRemove) {
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
		return CXTPStringHelper::Remove(str, chRemove);
	#else
		return str.Remove(chRemove);
	#endif
}
AFX_INLINE int INSERT_S(CString& str, int nIndex, LPCTSTR pstr) {
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
		return CXTPStringHelper::Insert(str, nIndex, pstr);
	#else
		return str.Insert(nIndex, pstr);
	#endif
}
AFX_INLINE int DELETE_S(CString& str, int nIndex, int nCount = 1) {
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
		return CXTPStringHelper::Delete(str, nIndex, nCount);
	#else
		return str.Delete(nIndex, nCount);
	#endif
}

AFX_INLINE void TRIMRIGHT_S(CString& str, LPCTSTR lpszTargetList) {
#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
	CXTPStringHelper::TrimRight(str, lpszTargetList);
#else
	str.TrimRight(lpszTargetList);
#endif
}


AFX_INLINE BOOL GETASSYSTEMTIME_DT(const COleDateTime& dtTime, SYSTEMTIME& sysTime) {
#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
	return CXTPDateTimeHelper::GetAsSystemTime(dtTime, sysTime);
#else
	::ZeroMemory(&sysTime, sizeof(sysTime));
	return dtTime.GetAsSystemTime(sysTime);
#endif
}

AFX_INLINE LONG GETTOTAL_DAYS_DTS(const COleDateTimeSpan& spSpan) {
#if (_MSC_VER <= 1200) // Using Visual C++ 5.0, 6.0
	return CXTPDateTimeHelper::GetTotalDays(spSpan);
#else
	return (LONG)spSpan.GetTotalDays();
#endif
}

AFX_INLINE LONG GETTOTAL_HOURS_DTS(const COleDateTimeSpan& spSpan) {
#if (_MSC_VER <= 1200) // Using Visual C++ 5.0, 6.0
	return CXTPDateTimeHelper::GetTotalHours(spSpan);
#else
	return (LONG)spSpan.GetTotalHours();
#endif
}

AFX_INLINE LONG GETTOTAL_MINUTES_DTS(const COleDateTimeSpan& spSpan) {
	return CXTPDateTimeHelper::GetTotalMinutes(spSpan);
}

AFX_INLINE LONG GETTOTAL_SECONDS_DTS(const COleDateTimeSpan& spSpan) {
	return CXTPDateTimeHelper::GetTotalSeconds(spSpan);
}

#if (_MSC_VER <= 1100)

AFX_INLINE LONG AFXAPI AfxDelRegTreeHelper(HKEY hParentKey, const CString& strKeyName)
{
	TCHAR   szSubKeyName[256];
	HKEY    hCurrentKey;
	DWORD   dwResult;

	if ((dwResult = RegOpenKey(hParentKey, strKeyName, &hCurrentKey)) ==
		ERROR_SUCCESS)
	{
		// Remove all subkeys of the key to delete
		while ((dwResult = RegEnumKey(hCurrentKey, 0, szSubKeyName, 255)) ==
				ERROR_SUCCESS)
		{
			if ((dwResult = AfxDelRegTreeHelper(hCurrentKey, szSubKeyName)) != ERROR_SUCCESS)
				break;
		}

		// If all went well, we should now be able to delete the requested key
		if ((dwResult == ERROR_NO_MORE_ITEMS) || (dwResult == ERROR_BADKEY))
		{
			dwResult = RegDeleteKey(hParentKey, strKeyName);
		}
	}

	RegCloseKey(hCurrentKey);
	return dwResult;
}

#endif

AFX_INLINE BOOL IMAGELISTDRAWINDIRECT_S(CImageList* pImageList, CDC* pDC, int nImage, POINT pt,
		SIZE sz, POINT ptOrigin = CPoint(0, 0), UINT fStyle = ILD_NORMAL,
		DWORD dwRop  = SRCCOPY, COLORREF rgbBack = CLR_DEFAULT,
		COLORREF rgbFore = CLR_DEFAULT)
{
#if (_MSC_VER <= 1100)
	ASSERT_POINTER(pDC, CDC);
	ASSERT(pDC->m_hDC != NULL);

	IMAGELISTDRAWPARAMS drawing;
	drawing.cbSize = sizeof(IMAGELISTDRAWPARAMS);
	drawing.himl = pImageList->m_hImageList;
	drawing.i = nImage;
	drawing.hdcDst = pDC->GetSafeHdc();
	drawing.x = pt.x;
	drawing.y = pt.y;
	drawing.cx = sz.cx;
	drawing.cy = sz.cy;
	drawing.xBitmap = ptOrigin.x;
	drawing.yBitmap = ptOrigin.y;
	drawing.rgbBk = rgbBack;
	drawing.rgbFg = rgbFore;
	drawing.fStyle = fStyle;
	drawing.dwRop = dwRop;

	return ImageList_DrawIndirect(&drawing);
#else
	return pImageList->DrawIndirect(pDC, nImage, pt, sz, ptOrigin, fStyle, dwRop, rgbBack, rgbFore);
#endif
}

//}}AFX_CODEJOCK_PRIVATE

//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTPVC50HELPERS_H__)
