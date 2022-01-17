#ifndef _AUXMLPARSER_H
#define _AUXMLPARSER_H

#include <ApDefine.h>
#include <ApMemoryPool.h>

#include <AuXmlParser/TinyXml/tinyxml.h>

#include <string>

using namespace std;

typedef TiXmlNode		AuXmlNode;
typedef TiXmlElement	AuXmlElement;
typedef TiXmlDocument	AuXmlDocument;

class AuXmlParser 
	:	public AuXmlDocument
{
public:
	AuXmlParser							( void );
	AuXmlParser							( const string& strFileName );
	AuXmlParser							( char* pBuffer , int nSize );
	virtual ~AuXmlParser				( void );

	bool				LoadXMLFile		( const string& strFileName );
	bool				LoadXMLMemory	( char* pBuffer , int nSize );

private:
	static ApMemoryPool		m_MemoryPool;

};


#endif // _AUXMLPARSER_H
