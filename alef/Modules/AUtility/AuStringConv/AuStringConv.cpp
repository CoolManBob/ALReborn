#include "AuStringConv.h"
#include "ApDefine.h"

#include <vector>

// 설정에 따른 언어 코드를 가져온다.
unsigned GetLangID( void )
{
	UINT code = 949;

	switch( g_eServiceArea )
	{
	case AP_SERVICE_AREA_CHINA:
		code = 936;
		break;
	case AP_SERVICE_AREA_WESTERN:
		code = 1252;
		break;
	case AP_SERVICE_AREA_JAPAN:
		code = 932;
		break;
	default:
		code = 949;	// 기본은 한글
	}

	return code;
}

// src에 영문자가 있다면 소문자로 변환한다.
// 일본어에서 _mbclwr가 이상하게 작동해서 문자열을 유니코드로 변환한후
// 영문 대문자를 소문자로 만들고 다시 mbcs로 변환한다.
void ConvertToLower( char* src, char* lower, int lowerSize )
{
	if ( src != NULL && strlen( src ) != 0 )
	{
		const int wideSize = 64;

		// to Unicode
		wchar_t wide[wideSize] = {0, };
		ToWide( src, wide, wideSize );

		// lower characters
		_wcslwr_s( wide, wideSize );

		// to mbcs
		ToMBCS( wide, lower, lowerSize );
	}
}

int ToWide( char* mbcs, wchar_t* wide, int wideCharCount )
{
	return MultiByteToWideChar( GetLangID(), 0, mbcs, -1, wide, wideCharCount );
}

int ToMBCS( wchar_t* wide, char* mbcs, int mbcsCharCount )
{
	int needBufferSize = WideCharToMultiByte( GetLangID(), 0, wide, -1, 0, 0, 0, 0 );
	
	if ( needBufferSize <= mbcsCharCount )
	{
		WideCharToMultiByte( GetLangID(), 0, wide, -1, mbcs, mbcsCharCount, 0, 0 );
		return true;
	}

	return false;
}

int GetMbcsLength( char* mbcs )
{
	return MultiByteToWideChar( GetLangID(), 0, mbcs, -1, 0, 0 );
}

void ToLowerExceptFirst( char* str, int const length )
{
	if ( 0 != length )
	{
		// Unicode로 바꾸자
		std::vector<wchar_t> wide( length );
		ToWide( str, &wide[0], length );

		// 첫글자가 영문자라면 소문자로
		if ( wide[0] >= L'a' && wide[0] <= L'z' )
			wide[0] = L'A' + wide[0] - L'a';

		// 나머지 영문자를 소문자로 바꾸자
		unsigned strSize = static_cast<unsigned>( wcslen( &wide[0] ) );

		for ( unsigned i = 1; i < strSize; ++i )
		{
			if ( wide[i] >= L'A' && wide[i] <= L'Z' )
				wide[i] = L'a' + wide[i] - 'A';
		}

		// 다시 MBCS로
		ToMBCS( &wide[0], str, length );
	}
}

// UIMessage를 읽을때 다른 문자를 코마(,)와 따옴표(")로 바꾸자.
// 일본어 MBCS의 경우에 trail byte에 ,가 나올 수 있다.
void ChangeCommaAndQutationMark( char* src, char comma, char qutation )
{
	std::vector<wchar_t> wideString(2048);
	const int charCount = ToWide( src, &wideString[0], 2048 );

	for ( int i = 0; i < charCount; ++i )
	{
		if ( wideString[i] == comma )
			wideString[i] = L',';

		if ( wideString[i] == qutation )
			wideString[i] = L'"';
	}

	// AGCMUIMANAGER2_MAX_UI_MESSAGE_DATA를 위해서 AgcmUIManager2Stream.h를
	// include하기에는 문제가 있어, 384를 하드코딩한다.
	ToMBCS( &wideString[0], src, /*AGCMUIMANAGER2_MAX_UI_MESSAGE_DATA*/384 );
}
