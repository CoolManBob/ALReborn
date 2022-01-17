#include "StdAfx.h"
#include "LangControl.h"

LanguageType g_lt      = LT_ENGLISH;
HINSTANCE	 g_resInst = NULL;

LanguageType GetLanguageType(void)
{
	HKEY key;
	
	char szLang[128] = {0, };
	DWORD bufferSize = 128;

	LanguageType lang = LT_ENGLISH;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\ArchLord", 0, KEY_QUERY_VALUE, &key))
	{
		if (ERROR_SUCCESS == RegQueryValueEx(key, "Language", NULL, NULL, (LPBYTE)szLang, &bufferSize))
		{
			if ( !stricmp(szLang, "french") )
				lang = LT_FRENCH;
			
			if ( !stricmp(szLang, "german") )
				lang = LT_GERMAN;

			// 영어는 기본 값이니 대입 필요없다.
		}
	}

	RegCloseKey(key);
	return lang;
}
