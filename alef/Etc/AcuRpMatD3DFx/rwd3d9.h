//@{ Jaewon 20040813
// copied & modified.
// get a d3d texture from a rw texture.
//@} Jaewon

/****************************************************************************
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
 * Copyright (c) 2004 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * rwd3d9.h
 *
 * Copyright (C) 2004 Criterion Technologies.
 *
 * Original author: Matt Reynolds.
 *
 * Purpose: Any structs, variables etc. needed from the RW that are not exposed.
 *
 ****************************************************************************/

#ifndef __RWD3D9_H__
#define __RWD3D9_H__

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern LPDIRECT3DBASETEXTURE9
D3DTextureFromRwTexture(RwTexture *texture);

extern RwBool
RwTextureHasAlpha(RwTexture *texture);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* __RWD3D9_H__ */

