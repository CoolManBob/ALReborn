#pragma once

enum LanguageType
{
	LT_ENGLISH = 0,
	LT_GERMAN,
	LT_FRENCH,
};

LanguageType GetLanguageType(void);

extern LanguageType g_lt;	// language type
extern HINSTANCE	g_resInst;