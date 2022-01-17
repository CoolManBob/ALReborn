#ifndef ACUTEXTURE_H
#define ACUTEXTURE_H

#include "rwcore.h"
#include "rpworld.h"

#include "ApBase.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9tex.h>

#include "RpNormMap.h"
#include "RtNormMap.h"

#include "AcDefine.h"

#pragma comment( lib, "RpNormMap" )
#pragma comment( lib, "RtNormMap" )

#define ACU_TEXTURE_MD5_HASH_STRING	"asdfqwer"

enum AcuTextureType
{
	ACUTEXTURE_TYPE_NONE,
	ACUTEXTURE_TYPE_PNG = 1,
	ACUTEXTURE_TYPE_DDS	= 2,
	ACUTEXTURE_TYPE_TX1 = 3,
	ACUTEXTURE_TYPE_TIF = 4,
	ACUTEXTURE_TYPE_BMP = 5,
	ACUTEXTURE_MAX_TYPE
};

enum AcuTextureCopyType
{
	ACUTEXTURE_COPY_UPDATE,
	ACUTEXTURE_COPY_RENDERTARGETDATA,
	ACUTEXTURE_COPY_STRETCH,
	ACUTEXTURE_COPY_LOAD_LINEAR,
	ACUTEXTURE_COPY_LOAD_TRIAGLE
};

class AcuTexture
{
public:
	static BOOL			ChangeTextureFormat( RwTexture* tex, int depth, int format );
	static BOOL			RwD3D9DDSTextureWrite( CHAR* szSrcName, CHAR* szDestName, INT32 lFormat = -1, INT32 lReduceRatio = 1, AcuTextureCopyType eType = ACUTEXTURE_COPY_LOAD_LINEAR, AcCallbackData1 fptrErrCB=NULL, BOOL bUseTX1 = TRUE, BOOL bMipmap = TRUE );
	static BOOL			RwD3D9DDSTextureWrite( RwTexture* pstTexture, CHAR* szDestName, INT32 lFormat = -1, INT32 lReduceRatio = 1, AcuTextureCopyType eType = ACUTEXTURE_COPY_LOAD_LINEAR, AcCallbackData1 fptrErrCB=NULL, BOOL bUseTX1 = TRUE, BOOL bMipmap = TRUE );
	static BOOL			RwTextureWriteWithTexDict(CHAR *szSrcName, CHAR *szDestName, AcuTextureType eType, BOOL bUsePITexD = FALSE, BOOL bUseMipmap = FALSE, AcCallbackData1 fptrErrCB=NULL );
	static BOOL			RwTextureWriteWithTexDict( RwTexture *pstTexture, CHAR *szDestName, AcuTextureType eType, BOOL bUsePITexD = FALSE, BOOL bUseMipmap = FALSE, AcCallbackData1 fptrErrCB=NULL );

	static LPDIRECT3DTEXTURE9	GetDXTextureFromRaster( RwRaster* raster );
	static HRESULT		BltAllLevels( D3DCUBEMAP_FACES FaceType, LPDIRECT3DBASETEXTURE9 ptexSrc, LPDIRECT3DBASETEXTURE9 ptexDest, DWORD numMips, AcuTextureCopyType eType = ACUTEXTURE_COPY_LOAD_LINEAR );
	static BOOL			CopyRaster( RwRaster* pstRasterDst, RwRaster* pstRasterSrc, AcuTextureCopyType eType = ACUTEXTURE_COPY_LOAD_LINEAR );

	static BOOL			DetermineDetailAlphaData( CHAR* szSrcName );

	//------------ NormalMap -----------------------
	static BOOL			ApplyNormalMap(RpMaterial *material, FLOAT fBumpness);
	static RwTexture*	CreateNormalMapTexture(RwTexture *texture, RwReal bumpness);

private:
	static RwRaster*	NormalMapTextureSpaceCreateFromImage(RwImage *image, RwUInt32 rasterFlags, RwBool clamp, RwReal bumpness);
};

#endif /* ACURPMTEXTURE_H */
