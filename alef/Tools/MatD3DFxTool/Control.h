//@{ Jaewon 20040824
// created.
// input handler & clump controller of MatD3DFxTool.
//@} Jaewon

#ifndef __MATD3DFXTOOL_CONTROL_H__
#define __MATD3DFXTOOL_CONTROL_H__

extern RwBool _bSpinOn;

void resetClumpControl();
void updateClumpControl(RwReal delta);

RwBool attachInputDevices();

#endif