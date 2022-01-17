#include "AuRegionNameConvert.h"
#include "MemMappedFile.h"
#include "StrUtil.h"

// -========================================================================-

AuRegionNameConvert& RegionLocalName()
{
	static AuRegionNameConvert rnc;
	return rnc;
}

// -========================================================================-

AuRegionNameConvert::AuRegionNameConvert()
{
	Load( "ini/RegionGlossary.txt", TRUE );
}

AuRegionNameConvert::~AuRegionNameConvert()
{
}

bool AuRegionNameConvert::Load( char* filename, bool encrypt )
{
	// open file
	MemMappedFile mmf;
	if ( !mmf.OpenForReadOnly( filename ) )
		return false;

	//
	const char* start = encrypt
					  ? GetDecryptStr( (char*)mmf.GetStartPtr(), mmf.GetFileSize())
					  : (char*)mmf.GetStartPtr();

	const char* ptr = start;

	char*	 line = new char [4096];
	unsigned offset = 0;
	bool	 result = true;

	while ( offset = StrUtil::GetOneLine( line, const_cast<char*>(ptr) ) )
	{
		StrUtil::RTrim( line );

		size_t pos = strcspn(line, "\t");
		std::string key(line, line + pos);
		std::string value(line + pos + 1);

		m_map[key] = value;

		ptr += offset;
	}

	if ( encrypt )
		delete [] start;
	
	delete [] line;

	return result;
}

std::string AuRegionNameConvert::GetStr( char* key )
{
	static std::string dummy = "";

	StrMap::iterator iter = m_map.find( key );

	return (iter != m_map.end()) ? iter->second : dummy;
}

char* AuRegionNameConvert::GetDecryptStr( char* str, unsigned size )
{
	char* temp = new char [ size + 1];
	memcpy( temp, str, size );
	temp[ size ] = 0;

	StrUtil::Decrypt( temp, size );

	return temp;
}
