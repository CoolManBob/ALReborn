
/****************************************************************************
 *
 * camera.h
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

#if (!defined(_CAMERA_H))
#define _CAMERA_H

#include "rwcore.h"

/*
#ifdef    __cplusplus
extern "C"
{
#endif                         //  __cplusplus 

extern void 
CameraMove(RwCamera *camera, RwV3d *offset);

extern void 
CameraTilt(RwCamera *camera, const RwV3d *pos, RwReal angle);

extern void 
CameraPan(RwCamera *camera, const RwV3d *pos, RwReal angle);

extern void 
CameraRotate(RwCamera *camera, const RwV3d *pos, RwReal angle);

extern void 
CameraSize(RwCamera *camera, RwRect *rect, RwReal viewWindow, RwReal aspectRatio);

extern void 
CameraDestroy(RwCamera *camera);

extern RwCamera *
CameraCreate(RwInt32 width, RwInt32 height, RwBool zBuffer);

extern RwRaster *
CameraCreateCrossHair(void);

#ifdef    __cplusplus
}
#endif                          // __cplusplus 
*/


/*****************************************************************
*   Comment  : camera.cpp 클래스 화 
*                    
*   Date&Time : 2002-04-02, 오전 10:37
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/

class AcuCamera
{

public:
	void CameraMove(RwCamera *camera, RwV3d *offset);

	void CameraTilt(RwCamera *camera, const RwV3d *pos, RwReal angle);

	void CameraPan(RwCamera *camera, const RwV3d *pos, RwReal angle);

	void CameraRotate(RwCamera *camera, const RwV3d *pos, RwReal angle);

	void CameraSize(RwCamera *camera, RwRect *rect, RwReal viewWindow, RwReal aspectRatio);

	void CameraDestroy(RwCamera *camera);

	RwCamera *CameraCreate(RwInt32 width, RwInt32 height, RwBool zBuffer);

	RwRaster *CameraCreateCrossHair(void);

};


#endif // (!defined(_CAMERA_H)) 
