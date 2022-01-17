#pragma once

#include "ApDefine.h"
#include "MagDebug.h"

enum EnumCompareResult
{
	COMPARE_NONE	= -2,
	COMPARE_LESS	= -1,
	COMPARE_EQUAL	= 0,
	COMPARE_GREATER	= 1,
};

template<INT32 TLength>
class ApString
{
private:
	TCHAR m_Buffer[TLength + 1];
	INT32 m_lLength;

	void SetLength(INT32 lLength) {m_lLength = lLength;}

public:
	enum {eBufferLength = TLength};

public:
	ApString()	{Clear();}
	ApString(LPCTSTR pString)
	{
		Clear();
		SetText(pString);
	}

	ApString(const ApString<TLength>& rString)
	{
		Clear();
		SetText(rString.m_Buffer);
	}

	void Clear()
	{
		SetLength(0);
		ZeroMemory(m_Buffer, sizeof(m_Buffer));
	}

	LPCTSTR GetBuffer() const	{return m_Buffer;}
	LPTSTR GetBuffer()			{return m_Buffer;}
	INT32 GetBufferLength()		{return eBufferLength;}
	INT32 GetByteLength()		{return (TLength * sizeof(TCHAR));}
	bool IsUnicode()			{return (sizeof(TCHAR) == sizeof(wchar_t));}
	INT32 GetLength()			{return m_lLength;}
	operator LPCTSTR ()			{return m_Buffer;}
	operator LPTSTR ()			{return m_Buffer;}
	bool IsEmpty()				{return (0 == GetLength());}
	bool IsFull()				{return (GetLength() == GetBufferLength());}

	bool SetText(LPCTSTR pString)
	{
		ASSERT(NULL != pString);

		if (NULL == pString)
			return false;

		INT32 lLength = (INT32)_tcslen(pString);
		ASSERT(lLength <= GetBufferLength());
		if (lLength > GetBufferLength())
			return false;

		_tcsncpy(m_Buffer, pString, GetBufferLength());
		SetLength(lLength);

		return true;
	}

	bool Format(LPCTSTR pFormat, ...)
	{
		ASSERT(NULL != pFormat);

		if (NULL == pFormat)
			return false;

		Clear();

		va_list ap;
		va_start(ap, pFormat);
		INT32 lLength = _vstprintf(m_Buffer, pFormat, ap);
		va_end(ap);

		if (INVALID_INDEX== lLength)
			return false;

		ASSERT(lLength <= GetBufferLength());
        SetLength(lLength);

		return true;
	}

	bool Append(LPCTSTR pString)
	{
		ASSERT(NULL != pString);

		if (NULL == pString)
			return false;

		INT32 lLength = (INT32)_tcslen(pString);
		ASSERT((GetLength() + lLength) <= GetBufferLength());

		if ((GetLength() + lLength) > GetBufferLength())
			return false;

		_tcsncpy(&m_Buffer[GetLength()], pString, GetBufferLength() - GetLength());
		SetLength(GetLength() + lLength);

		return true;
	}

	bool AppendFormat(LPCTSTR pFormat, ...)
	{
		ASSERT(NULL != pFormat);

		if (NULL == pFormat)
			return false;

		va_list ap;
		va_start(ap, pFormat);
		INT32 lLength = _vstprintf(&m_Buffer[GetLength()], pFormat, ap);
		va_end(ap);

		if (INVALID_INDEX == lLength)
			return false;

		ASSERT((GetLength() + lLength) <= GetBufferLength());

		if ((GetLength() + lLength) > GetBufferLength())
			return false;

		SetLength(GetLength() + lLength);

		return true;
	}

	EnumCompareResult Compare(LPCTSTR pString) const
	{
		ASSERT(NULL != pString);

		if (NULL == pString)
			return COMPARE_NONE;

		INT32 lResult = (INT32)_tcscmp(m_Buffer, pString);

		if (0 < lResult)
			return COMPARE_GREATER;
		else if (0 > lResult)
			return COMPARE_LESS;
		else
			return COMPARE_EQUAL;
	}

	EnumCompareResult CompareNoCase(LPCTSTR pString) const
	{
		ASSERT(NULL != pString);

		if (NULL == pString)
			return COMPARE_NONE;

		INT32 lResult = (INT32)_tcsicmp(m_Buffer, pString);

		if (0 < lResult)
			return COMPARE_GREATER;
		else if (0 > lResult)
			return COMPARE_LESS;
		else
			return COMPARE_EQUAL;
	}

	bool MakeLower()
	{
		ASSERT(0 < GetLength());

		TCHAR *pChar = _tcslwr(m_Buffer);
		ASSERT(pChar);
		return (NULL != pChar);
	}

	bool MakeUpper()
	{
		ASSERT(0 < GetLength());

		TCHAR *pChar = _tcsupr(m_Buffer);
		ASSERT(pChar);
		return (NULL != pChar);
	}

	TCHAR& operator[](INT32 lIndex)
	{
		ASSERT(lIndex >= 0);
		ASSERT(lIndex < GetBufferLength());
		ASSERT(lIndex < GetLength());

		return m_Buffer[lIndex];
	}

	// operator for generic algorithm
	bool operator < (const ApString<TLength> &rString)  const	{return (COMPARE_LESS == Compare(rString.GetBuffer()));}
	bool operator == (const ApString<TLength> &rString) const	{return (COMPARE_EQUAL == Compare(rString.GetBuffer()));}
	ApString<TLength> operator + (LPCTSTR pString) const		{ApString<TLength> ret(this->m_Buffer); ret.Append(pString); return ret;}	// 버퍼가 충분하지 않으면 비정상 동작, 20051107, kelovon

	size_t do_hash() const
	{
		unsigned int hashCode = 5381;
		size_t c;
		const char *hashString = &m_Buffer[0];
		while ( c = *hashString++ )
		{
			hashCode = ( ( hashCode << 5 ) + hashCode ) + c; // hash * 33 + c 
		}	

		return ( hashCode );
	}
};


/* unordered_map 사용할때

typedef ApString<100> ApMainString;

template<>
class hash_compare<ApMainString>
{
public:
	enum
	{
		bucket_size = 4,
		min_buckets = 8
	};

	public:
	size_t operator() (const ApMainString& rString) const
	{
 		return rString.do_hash();
	}

	bool operator() (const ApMainString& rStrLeft,	const ApMainString& rStrRight) const
	{
		return (COMPARE_EQUAL == rStrLeft.Compare(rStrRight.GetBuffer()));
	}
};

*/