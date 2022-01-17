#include "AgcuEffTable.h"
#include "ApMemoryTracker.h"

Eff2Ut_StRandTable<unsigned char>	g_RndTblBYTE	( 0x3ff, 255.1f );
Eff2Ut_StRandTable<float>			g_RndTblFloat_1	( 0x3ff, 1.1f	);

