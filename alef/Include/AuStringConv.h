#pragma once

unsigned GetLangID( void );

void ToLowerExceptFirst( char* src, int const length );
void ConvertToLower( char* src, char* lower, int lowerSize );
void ChangeCommaAndQutationMark( char* src, char comma, char qutation );

int ToWide( char* mbcs, wchar_t* wide, int wideCharCount );
int ToMBCS( wchar_t* wide, char* mbcs, int mbcsCharCount );
int GetMbcsLength( char* mbcs );
