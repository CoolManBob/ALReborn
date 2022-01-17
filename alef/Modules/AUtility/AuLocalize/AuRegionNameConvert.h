#pragma once

#include <string>
#include <map>

class AuRegionNameConvert
{
public:
	~AuRegionNameConvert();

	bool Load(char* filename, bool encrypt);
	std::string GetStr(char* key);

	friend AuRegionNameConvert& RegionLocalName();

private:
	AuRegionNameConvert();

	char* GetDecryptStr( char* str, unsigned size );

	typedef std::map<std::string, std::string> StrMap;
	StrMap m_map;
};

AuRegionNameConvert& RegionLocalName();