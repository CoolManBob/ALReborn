#include "StrUtil.h"
#include "AuMD5Encrypt.h"

#define HashKey "1111"

bool StrUtil::StrMid(wchar_t *dest,size_t destLength,wchar_t *src,size_t start,size_t end)
{
	if ( start > end || wcslen( src ) <= end || destLength < end - start )
		return false;

	size_t stringSize = end - start + 1;
	
	wcsncpy( dest, src + start, stringSize );
	dest[stringSize] = 0;

	return true;
}

void StrUtil::RTrim(wchar_t *str)
{
	size_t i = max( wcslen( str ) - 1, 0 );
	while ( str[i] == L'\n' || str[i] == L'\t' || str[i] == L'\r' || str[i] == L' ' )
	{
		if ( i <= 0 ) break;

		str[i] = 0;
		--i;
	}
}

void StrUtil::RTrim( char* str )
{
	size_t i = max( strlen( str ) - 1, 0 );
	while ( str[i] == '\n' || str[i] == '\t' || str[i] == '\r' || str[i] == ' ' )
	{
		if ( i <= 0 ) break;

		str[i] = 0;
		--i;
	}
}

unsigned StrUtil::GetOneLine(wchar_t* dest, wchar_t* src)
{
	size_t offset = 0;
	while( src[offset] != 0 && src[offset] != L'\n' )
		++offset;

	if ( offset == 0 ) return 0;

	++offset;

	memcpy(dest, src, offset * 2);
	dest[offset] = 0;

	return static_cast<unsigned>(offset);
}

unsigned StrUtil::GetOneLine(char* dest, char* src)
{
	size_t offset = 0;
	while( src[offset] != 0 && src[offset] != '\n' )
		++offset;

	if ( offset == 0 ) return 0;

	++offset;

	memcpy( dest, src, offset );
	dest[offset] = 0;

	return static_cast<unsigned>(offset);
}

bool StrUtil::Decrypt( char* buffer, unsigned bufferSize )
{
	AuMD5Encrypt md5;
#ifdef _AREA_CHINA_
	return md5.DecryptString( MD5_HASH_KEY_STRING, buffer, bufferSize ) ? true : false;
#else
	return md5.DecryptString( HashKey, buffer, bufferSize ) ? true : false;
#endif
}

bool StrUtil::CheckHan( const wchar_t* str, unsigned length )
{
	bool isHan = false;

	for ( unsigned i = 0; i < length; ++i )
	{
		if ( str[i] >= 0xAC00 && str[i] <= 0xD7AF )
			isHan = true;
	}

	return isHan;
}

			//// 기호1
			//if ( str[i] >= 1 && str[i] <= 64 )
			//	continue;

			//// 기호2
			//if ( str[i] >= 91 && str[i] <= 96 )
			//	continue;

			//// 기호3
			//if ( str[i] >= 123 && str[i] <= 127 )
			//	continue;
