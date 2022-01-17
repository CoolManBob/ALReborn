#pragma once

#include <winsock2.h>

class StrUtil
{
public:
	 StrUtil() {}
	~StrUtil() {}

	static bool StrMid(wchar_t *dest, size_t destLength, wchar_t *src, size_t start, size_t end);
	static void RTrim(wchar_t* str);
	static void RTrim(char* str);
	static bool Decrypt( char* buffer, unsigned bufferSize );
	static bool CheckHan( const wchar_t* str, unsigned length );

	static unsigned GetOneLine(wchar_t* dest, wchar_t* src);
	static unsigned GetOneLine(char* dest, char* src);
};
