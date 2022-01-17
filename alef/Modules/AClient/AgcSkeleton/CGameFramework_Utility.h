#ifndef __CLASS_GAME_FRAMEWORK_UTILITY_H__
#define __CLASS_GAME_FRAMEWORK_UTILITY_H__




#include "Windows.h"
#include "ContainerUtil.h"



namespace FrameworkUtil
{
	char**		CommandLineToArgv		( int* pCommandCount );
	void		DeleteCommandLine		( char** ppArgv, int nCommandCount );
}




#endif