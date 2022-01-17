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
 * rwd3d9.c
 *
 * Copyright (C) 2004 Criterion Technologies.
 *
 * Original author: Matt Reynolds.
 *
 * Purpose: D3D9 pipeline providing D3DX FX file support.
 *
 ****************************************************************************/

#include <d3d9.h>
#include "rwcore.h"
#include "rwd3d9.h"
#include "myassert.h"

typedef struct _rwD3D9Palette _rwD3D9Palette;
struct _rwD3D9Palette
{
    PALETTEENTRY    entries[256];
    RwInt32     globalindex;
};

typedef LPDIRECT3DSURFACE9 LPSURFACE;
typedef LPDIRECT3DTEXTURE9 LPTEXTURE;

typedef struct _rwD3D9RasterExt _rwD3D9RasterExt;
struct _rwD3D9RasterExt
{
    LPTEXTURE               texture;
    _rwD3D9Palette          *palette;
    RwUInt8                 alpha;              /* This texture has alpha */
    RwUInt8                 cube : 4;           /* This texture is a cube texture */
    RwUInt8                 face : 4;           /* The active face of a cube texture */
    RwUInt8                 automipmapgen : 4;  /* This texture uses automipmap generation */
    RwUInt8                 compressed : 4;     /* This texture is compressed */
    RwUInt8                 lockedMipLevel;
    LPSURFACE               lockedSurface;
    D3DLOCKED_RECT          lockedRect;
    D3DFORMAT               d3dFormat;          /* D3D format */
    LPDIRECT3DSWAPCHAIN9    swapChain;
    HWND                    window;
};

#define RASTEREXTFROMRASTER(raster) \
    ((_rwD3D9RasterExt *)(((RwUInt8 *)(raster)) + _RwD3D9RasterExtOffset))

extern RwInt32 _RwD3D9RasterExtOffset;


/*
 *************************************************************************************************************
 */
LPDIRECT3DBASETEXTURE9
D3DTextureFromRwTexture(RwTexture *texture)
{
    RwRaster *raster;
    _rwD3D9RasterExt *rasExt;

    my_assert(texture);

    raster = RwTextureGetRaster(texture);
    my_assert(raster);
    rasExt = RASTEREXTFROMRASTER(raster);

    return (LPDIRECT3DBASETEXTURE9)rasExt->texture;
}

/*
 *************************************************************************************************************
 */
RwBool
RwTextureHasAlpha(RwTexture *texture)
{
    RwRaster *raster;
    _rwD3D9RasterExt *rasExt;

    my_assert(texture);

    raster = RwTextureGetRaster(texture);
    my_assert(raster);
    rasExt = RASTEREXTFROMRASTER(raster);

	return (LPDIRECT3DBASETEXTURE9)rasExt->alpha ? TRUE : FALSE;
}

/*
 *************************************************************************************************************
 */