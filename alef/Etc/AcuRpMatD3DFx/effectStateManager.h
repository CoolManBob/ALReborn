//@{ Jaewon 20041007
// created.
//@} Jaewon

#ifndef __EFFECTSTATEMANAGER_H__
#define __EFFECTSTATEMANAGER_H__

#include <d3dx9effect.h>

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
	
void EffectSetStageManager(LPD3DXEFFECT pEffect);

//@{ Jaewon 20041110
BOOL DecryptMD5(char *data, unsigned long size);
//@} Jaewon
#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif