#pragma once
#include <string>

class AgcmRegistryManager
{
public:
	static char const * RegPath();

private:
	AgcmRegistryManager();

	std::string regPath_;

	static AgcmRegistryManager & instance();
};