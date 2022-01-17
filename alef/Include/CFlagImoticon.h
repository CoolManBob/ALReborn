#ifndef __CLASS_FLAG_IMOTICON_H__
#define __CLASS_FLAG_IMOTICON_H__




#include "ApBase.h"
#include "ContainerUtil.h"
#include "AuXmlParser/TinyXML/TinyXML.h"


#define STRING_LENGTH_NAME					64
#define STRING_LENGTH_FILENAME				256


struct stFlagEntry
{
	char										m_strName[ STRING_LENGTH_NAME ];
	char										m_strImageFileName[ STRING_LENGTH_FILENAME ];
	void*										m_pTexture;
	int											m_nEventCode;

	stFlagEntry( void )
	{
		memset( m_strName, 0, sizeof( char ) * STRING_LENGTH_NAME );
		memset( m_strImageFileName, 0, sizeof( char ) * STRING_LENGTH_FILENAME );
		m_pTexture = NULL;
		m_nEventCode = 0;
	}
};

struct stOffsetEntry
{
	char										m_strName[ STRING_LENGTH_NAME ];
	float										m_fDefaultOffsetX;
	float										m_fDefaultOffsetY;

	stOffsetEntry( void )
	{
		memset( m_strName, 0, sizeof( char ) * STRING_LENGTH_NAME );
		m_fDefaultOffsetX = 0.0f;
		m_fDefaultOffsetY = 0.0f;
	}
};

class CFlagImoticon
{
private :
	ContainerMap< std::string, stFlagEntry >	m_mapFlag;
	ContainerMap< std::string, stOffsetEntry >	m_mapOffset;

public :
	CFlagImoticon( void );
	~CFlagImoticon( void );

public :
	BOOL			LoadFlagFromFile			( char* pFileName );
	BOOL			ReloadFlag					( char* pFileName );
	BOOL			ClearAllFlag				( void );

private :
	BOOL			_LoadXmlFile				( TiXmlDocument* pDoc, char* pFileName, BOOL bIsEncrypt = FALSE );

public :
	void*			GetFlagTexture				( char* pFlagName );
	void*			GetFlagTexture				( int nEventCode );

	float			GetFlagOffsetX				( char* pStateName );
	float			GetFlagOffsetY				( char* pStateName );

	float			GetFlagOffsetX				( BOOL bHaveGuild, BOOL bHaveCharisma );
	float			GetFlagOffsetY				( BOOL bHaveGuild, BOOL bHaveCharisma );
};




#endif