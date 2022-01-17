
/****************************************************************************
 *
 * mouse.h
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "rwcore.h"
/*
#ifdef    __cplusplus
extern "C"
{
#endif                          // __cplusplus 
*/


/*****************************************************************
*   Comment  : mouse.cpp 클래스화 
*                    
*   Date&Time : 2002-04-02, 오후 5:48
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
class AcuMouse
{
private:

	AcuMouse();
	RwV2d        MousePos;
	RwRaster    *CursorRaster;
	RwBool       DrawMouse;

	RwBool rsMouseInit(void);
	RwBool rsMouseTerm(void);
	void rsMouseAddDelta(RwV2d *delta);
	void rsMouseGetPos(RwV2d *pos);
	void rsMouseSetPos(RwV2d *pos);
	void rsMouseRender(RwCamera *camera);
	void rsMouseVisible(RwBool visible);
};

/*
#ifdef    __cplusplus
}
#endif                          // __cplusplus 
*/

#endif //_MOUSE_H_
