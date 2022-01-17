#include "AcuTexture.h"
#include "AuMD5Encrypt.h"
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <RtPITexD.h>

#define RASTEREXTFROMRASTER(raster) ((_rwD3D9RasterExt *)(((RwUInt8 *)(raster)) + _RwD3D9RasterExtOffset))
#define RASTEREXTFROMCONSTRASTER(raster) ((const _rwD3D9RasterExt *) (((const RwUInt8 *)(raster)) + _RwD3D9RasterExtOffset))

typedef struct _rwD3D9Palette _rwD3D9Palette;
struct _rwD3D9Palette
{
	PALETTEENTRY entries[256];
	RwInt32 globalindex;
};

typedef LPDIRECT3DSURFACE9 LPSURFACE;
typedef LPDIRECT3DTEXTURE9 LPTEXTURE;

typedef struct _rwD3D9RasterExt _rwD3D9RasterExt;
struct _rwD3D9RasterExt
{
	LPTEXTURE		texture;
	_rwD3D9Palette*	palette;
	RwUInt8			alpha; /* This texture has alpha */
	RwUInt8			cube : 4; /* This texture is a cube texture */
	RwUInt8			face : 4; /* The active face of a cube texture */
	RwUInt8			automipmapgen : 4; /* This texture uses automipmap generation */
	RwUInt8			compressed : 4; /* This texture is compressed */
	RwUInt8			lockedMipLevel;
	LPSURFACE		lockedSurface;
	D3DLOCKED_RECT	lockedRect;
	D3DFORMAT		d3dFormat; /* D3D format */
	LPDIRECT3DSWAPCHAIN9	swapChain;
	HWND			window;
};

// 32비트(8888)포맷이 아니면 해당 bitdepth와 format으로 변환한다~
// ex)ChangeTextureFormat(pTex,16,rwRASTERFORMAT1555 | rwRASTERFORMATMIPMAP | rwRASTERFORMATAUTOMIPMAP );
// 밉맵이 있는 1555 포맷으로 변환한다.~~
BOOL	AcuTexture::ChangeTextureFormat( RwTexture* tex, int depth, int format )
{
	RwRaster*	ras = RwTextureGetRaster(tex);
	if( !ras ) return FALSE;

	int rasformat = RwRasterGetFormat( ras );
	rasformat = rasformat & rwRASTERFORMATPIXELFORMATMASK;
	if( rwRASTERFORMAT8888 == rasformat )		// alpha 포함 맵 제외
		return FALSE;		

	int w = RwRasterGetWidth(ras);
	int h = RwRasterGetHeight(ras);
	RwRaster* nwRas = RwRasterCreate( w, h, depth, rwRASTERTYPETEXTURE | (RwRasterFormat)format );
	RwImage* img = RwImageCreate( w, h, 32 );
    if( img )
    {
        RwImageAllocatePixels( img );
        RwImageSetFromRaster( img, ras );
	}
	
	RwRasterSetFromImage (nwRas,img);
	RwImageDestroy(img);

 	RwTextureSetRaster (tex,nwRas);
	RwRasterDestroy(ras);

	return TRUE;
}

BOOL AcuTexture::RwD3D9DDSTextureWrite( CHAR* szSrcName, CHAR* szDestName, INT32 lFormat, INT32 lReduceRatio, AcuTextureCopyType eType, AcCallbackData1 fptrErrCB, BOOL bUseTX1, BOOL bMipmap )
{
	RwTexture* pTexture = RwTextureRead( szSrcName, NULL );
	if( !pTexture )		return FALSE;

	RwD3D9DDSTextureWrite( pTexture, szDestName, lFormat, lReduceRatio, eType, fptrErrCB, bUseTX1, bMipmap );
	RwTextureDestroy( pTexture );
	pTexture = NULL;
	return TRUE;
}

BOOL AcuTexture::RwD3D9DDSTextureWrite( RwTexture* pstTexture, CHAR* szDestName, INT32 lFormat, INT32 lReduceRatio, AcuTextureCopyType eType, AcCallbackData1 fptrErrCB, BOOL bUseTX1, BOOL bMipmap )
{
    RwRaster* raster = RwTextureGetRaster( pstTexture );
	RwRaster* topRaster = raster;

	while( (topRaster = RwRasterGetParent(topRaster)) != RwRasterGetParent(topRaster) )	{	};

	LPDIRECT3DBASETEXTURE9 texCurrent = GetDXTextureFromRaster(topRaster);
	
	bool followRasterFormatStrictly = lFormat == -2 ? true : false;

	D3DFORMAT eD3DFMT;
	if( lFormat == -1 || lFormat == -2 )
	{
		switch( ( RwRasterGetFormat(topRaster) & rwRASTERFORMATPIXELFORMATMASK ) )
		{
		//case rwRASTERFORMAT1555:
		//case rwRASTERFORMAT565:
		//case rwRASTERFORMAT555:
		//case rwRASTERFORMATLUM8:
		//case rwRASTERFORMAT888:
		//case rwRASTERFORMAT16:
		//case rwRASTERFORMAT24:
		//case rwRASTERFORMAT32:
		case rwRASTERFORMAT4444:
		case rwRASTERFORMAT8888:
			eD3DFMT = D3DFMT_DXT3;
			break;
		case rwRASTERFORMAT888:		// DXT루 바꿨더니 Alpha가 안나온다.-_-;
			eD3DFMT = RwRasterGetDepth(topRaster) == 24 || followRasterFormatStrictly ? D3DFMT_DXT1 : D3DFMT_DXT3;		// 'followRasterFormatStrictly' check added.
			break;
		default:
			eD3DFMT = D3DFMT_DXT1;
		}
	}
	else
		eD3DFMT = (D3DFORMAT) lFormat;

	// Set Mipmap Level
	INT32 lSize = RwRasterGetWidth(topRaster);
	if (RwRasterGetHeight(topRaster) < lSize)
		lSize = RwRasterGetHeight(topRaster);

	DWORD dwMipmap = 1;
	if (bMipmap)
	{
		for (dwMipmap = 1; lSize != 1; lSize = lSize / 2)
			++dwMipmap;

		dwMipmap -= lReduceRatio - 1;
		if (dwMipmap <= 0)
			dwMipmap = 1;
	}

	
	INT32 lWidth = pstTexture->raster->width;
	lWidth = (INT32) (lWidth / pow((double)2, lReduceRatio - 1));
	if( lWidth == 0 )
		lWidth = 1;

	INT32 lHeight = pstTexture->raster->height;
	lHeight = (INT32) (lHeight / pow((double)2, lReduceRatio - 1));

    if( (eD3DFMT == D3DFMT_DXT1 || eD3DFMT == D3DFMT_DXT2 || eD3DFMT == D3DFMT_DXT3 || eD3DFMT == D3DFMT_DXT4 || eD3DFMT == D3DFMT_DXT5) &&
		( pstTexture->raster->width % 2 || pstTexture->raster->height % 2 ) )
    {
        ASSERT(!"RwD3D9DDSTextureWrite() Error changing format!!!");

		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "not avaliable texturesize( width = 4x, height = 4x )";
			fptrErrCB( (PVOID)(errBuff) );
		}
        return FALSE;
    }

	LPDIRECT3DDEVICE9	pd3dDevice = (LPDIRECT3DDEVICE9) RwD3D9GetCurrentD3DDevice();

	LPDIRECT3DTEXTURE9	texNew;
    HRESULT hr = pd3dDevice->CreateTexture( lWidth, lHeight, dwMipmap, 0, eD3DFMT, D3DPOOL_MANAGED, &texNew, NULL );
    if( FAILED( hr ) )
	{
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "";
			sprintf( errBuff, "CreateTexture failed : %s, lWidth(%d), lHeight(%d), dwMipmap(%d)", szDestName, lWidth, lHeight, dwMipmap );				
			fptrErrCB( (PVOID)(errBuff) );
		}
        return FALSE;
	}

    if( FAILED( BltAllLevels( D3DCUBEMAP_FACE_FORCE_DWORD, texCurrent, texNew, dwMipmap, eType ) ) )
	{
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "";
			sprintf( errBuff, "%s failed : %s", "BltAllLevels", szDestName );
			fptrErrCB( (PVOID)(errBuff) );
		}

        return FALSE;
	}

	hr = D3DXSaveTextureToFile( szDestName, D3DXIFF_DDS, texNew, NULL );
	if( FAILED( hr ) )
	{
        ASSERT(!"RwD3D9DDSTextureWrite() Error saving texture!!!");
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "";
			sprintf( errBuff, "%s failed : %s", "D3DXSaveTextureToFile", szDestName );
			fptrErrCB( (PVOID)(errBuff) );
		}
		return FALSE;
	}

    texNew->Release();

	if (bUseTX1)
	{
		// 이제 썼으니깐, 읽어서 TexDictionary로 바꾸자.

		CHAR	szDrive[256], szDir[256], szFName[256];
		_splitpath( szDestName, szDrive, szDir, szFName, NULL );

		CHAR	szName2[128];
		sprintf( szName2, "%s%s%s", szDrive, szDir, szFName );
		RwTexture* pTextureNew = RwD3D9DDSTextureRead( szName2, NULL );
		if (!pTextureNew)
		{
			if( fptrErrCB )
			{
				RwChar	errBuff[MAX_PATH] = "";
				sprintf( errBuff, "%s failed : %s", "RwD3D9DDSTextureRead", szName2 );
				fptrErrCB( (PVOID)(errBuff) );
			}

			return FALSE;
		}

		memset( pTextureNew->mask, 0, rwTEXTUREBASENAMELENGTH );		// Clear the mask name. If you not, the output texture may differ in each exporting.

		if( RwTextureWriteWithTexDict( pTextureNew, szName2, ACUTEXTURE_TYPE_DDS, 0, 0, fptrErrCB ) )
			DeleteFile( szDestName );
	}

	return TRUE;
}

BOOL	AcuTexture::RwTextureWriteWithTexDict(CHAR *szSrcName, CHAR *szDestName, AcuTextureType	eType, BOOL bUsePITexD, BOOL bUseMipmap, AcCallbackData1 fptrErrCB)
{
	return RwTextureWriteWithTexDict( RwTextureRead(szSrcName, NULL), szDestName, eType, bUsePITexD, bUseMipmap, fptrErrCB );
}

BOOL	AcuTexture::RwTextureWriteWithTexDict( RwTexture *pstTexture, CHAR *szDestName, AcuTextureType eType, BOOL bUsePITexD, BOOL bUseMipmap, AcCallbackData1 fptrErrCB )
{
	if( !pstTexture )
	{
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "!pstTexture";
			fptrErrCB( (PVOID)(errBuff) );
		}

		return FALSE;
	}

	RwTexDictionary* pTexDictionary = RwTexDictionaryCreate();
	if (!pTexDictionary)
	{
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "!pTexDictionary";
			fptrErrCB( (PVOID)(errBuff) );
		}

		return FALSE;
	}

	if ( eType != ACUTEXTURE_TYPE_DDS && bUseMipmap )
		RwTextureSetFilterMode( pstTexture, rwFILTERLINEARMIPLINEAR );
	else if (!bUseMipmap)
		RwTextureSetFilterMode( pstTexture, rwFILTERNEAREST );

	RwTexDictionaryAddTexture( pTexDictionary, pstTexture );
	
	RwMemory stMemory;
	RwStream* pStream = RwStreamOpen( rwSTREAMMEMORY, rwSTREAMWRITE, &stMemory );
	if (!pStream)
	{
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "Failed  - RwStreamOpen";
			fptrErrCB( (PVOID)(errBuff) );
		}
		return FALSE;
	}

	if (bUsePITexD)
	{
		if (!RtPITexDictionaryStreamWrite(pTexDictionary, pStream))
		{
			if( fptrErrCB )
			{
				RwChar	errBuff[MAX_PATH] = "Failed - RtPITexDictionaryStreamWrite";
				fptrErrCB( (PVOID)(errBuff) );
			}
			return FALSE;
		}
	}
	else if (!RwTexDictionaryStreamWrite(pTexDictionary, pStream))
	{
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "Failed - RwTexDictionaryStreamWrite";
			fptrErrCB( (PVOID)(errBuff) );
		}

		return FALSE;
	}

	RwStreamClose( pStream, &stMemory );

	AuMD5Encrypt csMD5;
	if( !csMD5.EncryptString( ACU_TEXTURE_MD5_HASH_STRING, (CHAR *) stMemory.start, stMemory.length ) )
	{
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "Failed - csMD5.EncryptString";
			fptrErrCB( (PVOID)(errBuff) );
		}

		return FALSE;
	}
	
	// 메모리에서 파일로...
	CHAR	szName[128];
	sprintf( szName, "%s.tx%d", szDestName, 1 );
	INT32 hFile = _open( szName, _O_CREAT | _O_WRONLY | _O_BINARY, _S_IWRITE );
	if(hFile == -1)
	{
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "Failed - csMD5.EncryptString";
			sprintf( errBuff, "%s failed : %s", "_open", szName );
			fptrErrCB( (PVOID)(errBuff) );
		}

		return FALSE;
	}

	if( _write(hFile, stMemory.start, stMemory.length) != (INT32) stMemory.length )
	{
		if( fptrErrCB )
		{
			RwChar	errBuff[MAX_PATH] = "Failed - _write";
			fptrErrCB( (PVOID)(errBuff) );
		}

		return FALSE;
	}

	_close(hFile);

	free( stMemory.start );

	RwTexDictionaryDestroy( pTexDictionary );

	return TRUE;
}

LPDIRECT3DTEXTURE9	AcuTexture::GetDXTextureFromRaster(RwRaster *raster)
{
	RwInt32	_RwD3D9RasterExtOffset = RwRasterGetPluginOffset(rwID_DEVICEMODULE);
	return RASTEREXTFROMRASTER(raster)->texture;
}

HRESULT AcuTexture::BltAllLevels( D3DCUBEMAP_FACES FaceType, LPDIRECT3DBASETEXTURE9 ptexSrc, LPDIRECT3DBASETEXTURE9 ptexDest, DWORD dwMipmap, AcuTextureCopyType eType )
{
    HRESULT hr;

    LPDIRECT3DTEXTURE9 texSrc = (LPDIRECT3DTEXTURE9)ptexSrc;
    LPDIRECT3DTEXTURE9 texDst = (LPDIRECT3DTEXTURE9)ptexDest;
    for( DWORD iLevel = 0; iLevel < dwMipmap; ++iLevel )
    {
        LPDIRECT3DSURFACE9 surfaceSrc = NULL;
		texSrc->GetSurfaceLevel( 0, &surfaceSrc );

        LPDIRECT3DSURFACE9 surfaceDst = NULL;
		texDst->GetSurfaceLevel( iLevel, &surfaceDst );
        
		if( surfaceSrc && surfaceDst )
		{
			switch( eType )
			{
			case ACUTEXTURE_COPY_UPDATE:
				hr = ((LPDIRECT3DDEVICE9) RwD3D9GetCurrentD3DDevice())->UpdateSurface(surfaceSrc, NULL, surfaceDst, NULL );
				break;
			case ACUTEXTURE_COPY_RENDERTARGETDATA:
				hr = ((LPDIRECT3DDEVICE9) RwD3D9GetCurrentD3DDevice())->GetRenderTargetData(surfaceSrc, surfaceDst );
				break;
			case ACUTEXTURE_COPY_STRETCH:
				hr = ((LPDIRECT3DDEVICE9) RwD3D9GetCurrentD3DDevice())->StretchRect(surfaceSrc, NULL, surfaceDst, NULL, D3DTEXF_NONE );
				break;
			case ACUTEXTURE_COPY_LOAD_TRIAGLE:
				hr = D3DXLoadSurfaceFromSurface( surfaceDst, NULL, NULL, surfaceSrc, NULL, NULL,  D3DX_FILTER_TRIANGLE, 0 );
				break;
			default:
				hr = D3DXLoadSurfaceFromSurface( surfaceDst, NULL, NULL, surfaceSrc, NULL, NULL, D3DX_FILTER_LINEAR, 0 );
				break;
			}
		}

		if (surfaceSrc)
			surfaceSrc->Release();

		if (surfaceDst)
			surfaceDst->Release();
    }

    return S_OK;
}

BOOL		AcuTexture::CopyRaster(RwRaster *pstRasterDst, RwRaster *pstRasterSrc, AcuTextureCopyType eType)
{
    LPDIRECT3DTEXTURE9	pD3DTexSrc = GetDXTextureFromRaster(pstRasterSrc);
    LPDIRECT3DTEXTURE9	pD3DTexDst = GetDXTextureFromRaster(pstRasterDst);

	if ( FAILED( BltAllLevels( D3DCUBEMAP_FACE_FORCE_DWORD, pD3DTexSrc, pD3DTexDst, 1, eType ) ) )
		return FALSE;

	return TRUE;
}

//LG_BASE 일때만 알파 Data셋팅
BOOL AcuTexture::DetermineDetailAlphaData( CHAR *szSrcName )
{
	char str[ 256 ];
	strncpy( str , szSrcName , 256 );
	for( int i = 0 ; i < ( int ) strlen( str ); i ++ )
		str[ i ] = toupper( str[ i ] );
	str[ 7 ] = '\0';

	return strcmp( str , "LG_BASE" ) ? FALSE : TRUE;
}

// ------------------------------- Normal Maps -------------------------------
BOOL AcuTexture::ApplyNormalMap(RpMaterial *material, FLOAT fBumpness)
{
	RwTexture *	texture = RpMaterialGetTexture( material );
	if( !texture )		return FALSE;

	RwTexture *	normalmap = CreateNormalMapTexture( texture, fBumpness );
	if( !normalmap )	return FALSE;
    if( !RpNormMapMaterialSetNormMapTexture( material, normalmap ) )	return FALSE;

	RwTextureDestroy(normalmap);
	normalmap = NULL;

	return TRUE;
}

RwTexture *	AcuTexture::CreateNormalMapTexture(RwTexture *texture, RwReal bumpness)
{
    RwRaster *raster;
    RwTexture *normalmap = NULL;

    raster = RwTextureGetRaster(texture);
    if (raster != NULL)
    {
        RwUInt32 width, height;
        RwImage *image;
        RwBool clamp;
        RwUInt32 rasterFlags;
        RwRaster *rasternormalmap;

        width = RwRasterGetWidth(raster);
        height = RwRasterGetHeight(raster);

	    image = RwImageCreate(width, height, 32);
	    RwImageAllocatePixels(image);
	    RwImageSetFromRaster(image, raster);

        clamp = (RwTextureGetAddressingU(texture) == rwTEXTUREADDRESSCLAMP || RwTextureGetAddressingV(texture) == rwTEXTUREADDRESSCLAMP);
                 

        rasterFlags = (RwRasterGetType(raster) |
#ifdef FAST_MODE
                       (RwRasterGetFormat(raster) & (rwRASTERFORMATMIPMAP | rwRASTERFORMATAUTOMIPMAP)) |
#else
                       (RwRasterGetFormat(raster) & rwRASTERFORMATMIPMAP) |
#endif
                       rwRASTERFORMAT888);

        rasternormalmap = NormalMapTextureSpaceCreateFromImage(image, rasterFlags, clamp, bumpness);

        /* Create texture */
        normalmap = RwTextureCreate(rasternormalmap);

        RwTextureSetFilterMode(normalmap, RwTextureGetFilterMode(texture));
        RwTextureSetAddressingU(normalmap, RwTextureGetAddressingU(texture));
        RwTextureSetAddressingV(normalmap, RwTextureGetAddressingV(texture));

        RwImageDestroy(image);
    }

    return normalmap;
}

RwRaster *	AcuTexture::NormalMapTextureSpaceCreateFromImage(RwImage *image, RwUInt32 rasterFlags, RwBool clamp, RwReal bumpness)
{
	if( !image )	return NULL;

    RwInt32 width = RwImageGetWidth( image );
    RwInt32 height = RwImageGetHeight( image );

    RwRaster* rasternormalmap = RwRasterCreate( width, height, 32, rasterFlags );
    if( !rasternormalmap )		return NULL;

    RwImage* imagenormalmap = NULL;

#ifdef FAST_MODE
    imagenormalmap = RtNormMapCreateFromImage(image, clamp, bumpness);
    if( imagenormalmap )
    {
        RwRasterSetFromImage( rasternormalmap, imagenormalmap );
        RwImageDestroy( imagenormalmap );
    }
#else
    width = RwRasterGetWidth(rasternormalmap);
    height = RwRasterGetHeight(rasternormalmap);

    RwUInt32 nummipmaps = RwRasterGetNumLevels( rasternormalmap );

    for( RwUInt32 n = 0; n < nummipmaps; ++n )
    {
        RwRasterLock( rasternormalmap, n, rwRASTERLOCKWRITE | rwRASTERLOCKNOFETCH );

        if( RwImageGetWidth(image) != width || RwImageGetHeight(image) != height )
        {
            RwImage* imagemipmap = RwImageCreateResample(image, width, height);
            if ( imagemipmap )
            {
                imagenormalmap = RtNormMapCreateFromImage(imagemipmap, clamp, bumpness);
                RwImageDestroy( imagemipmap );
            }
        }
        else
            imagenormalmap = RtNormMapCreateFromImage(image, clamp, bumpness);

        if( imagenormalmap )
        {
            RwRasterSetFromImage( rasternormalmap, imagenormalmap );
            RwImageDestroy( imagenormalmap );
            imagenormalmap = NULL;
        }

        RwRasterUnlock(rasternormalmap);

        width *= .5f;
        if( width == 0 )
            width = 1;

        height *= .5f;
        if( height == 0 )
            height = 1;

        bumpness *= 0.5f;
#endif
}

    return rasternormalmap;
}