#ifndef __CLASS_TEXT_FILTER_H__
#define __CLASS_TEXT_FILTER_H__




#include "ContainerUtil.h"
#include "Windows.h"
#include <string>



struct stTextFilterEntry
{
	char											m_strFilter[ 256 ];
	int												m_nTextLegnth;

	stTextFilterEntry( void )
	{
		memset( m_strFilter, 0, sizeof( char ) * 256 );
		m_nTextLegnth = 0;
	}
};

class CTextFilter
{
private :
	ContainerMap< std::string, stTextFilterEntry >	m_mapFilter;
	ContainerMap< std::string, stTextFilterEntry >	m_mapFilterCustom;
	char											m_strBlindText[ 2 ];

public :
	CTextFilter( void );
	virtual ~CTextFilter( void );

public :
	BOOL			OnLoadFilterFile				( char* pFileName, BOOL bIsEncrypt );

	BOOL			OnAddFilter						( char* pFilterText, BOOL bIsCustom, BOOL bIsSave = FALSE );
	BOOL			OnRemoveFilter					( char* pFilterText, BOOL bIsCustom );
	BOOL			OnClearFilter					( BOOL bIsCustom );

	BOOL			OnSaveFilterFileCustom			( char* pFileName, char* pFilter );
	BOOL			OnFiltering						( char* pString );

private :
	BOOL			_LoadCustomFilterFile			( char* pFileName );
};




#endif