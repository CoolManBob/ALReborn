#include "AgcmRegistryManager.h"
#include <cstdio>

//-----------------------------------------------------------------------
//

AgcmRegistryManager::AgcmRegistryManager()
{
	FILE * f = fopen( "AlphaTest.arc", "rb" );

	if( !f )
		regPath_ = "SOFTWARE\\Webzen\\ArchLord_KR";
	else
		regPath_ = "SOFTWARE\\Webzen\\ArchLord_KR_Alpha";

	if( f )
		fclose(f);
}

//-----------------------------------------------------------------------
//

char const * AgcmRegistryManager::RegPath()
{
	return instance().regPath_.c_str();
}

//-----------------------------------------------------------------------
//

AgcmRegistryManager & AgcmRegistryManager::instance() 
{
	static AgcmRegistryManager inst;
	return inst;
}

//-----------------------------------------------------------------------