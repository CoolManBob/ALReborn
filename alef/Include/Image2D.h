#pragma once

#include <rwplcore.h>
#include <rwcore.h>

class Image2D
{
public:
	Image2D( VOID );
	~Image2D( VOID );

	VOID       SetCameraTrans	( const RwV3d& vCameraPos , FLOAT fRecipz , FLOAT fScreenZ );

	VOID       SetColor			( BYTE byAlpha , BYTE byRed , BYTE byGreen , BYTE byBlue );
	VOID       SetColor			( RwRGBA& rwColor );

	VOID       SetPos			( FLOAT nX , FLOAT nY , FLOAT nWidth , FLOAT nHeight );
	VOID       SetPos			( const RwV2d& v0, const RwV2d& v1, const RwV2d& v2, const RwV2d& v3 );

	VOID       SetUV			( FLOAT fLeft    , FLOAT fTop, FLOAT fRight, FLOAT fBottom, FLOAT fZ );
	VOID       SetUV			( const RwV2d& v0, const RwV2d& v1, const RwV2d& v2, const RwV2d& v3, float fZ );

	VOID	   Render			( RwPrimitiveType eRenderType, RwRaster* pRaster = NULL );

private:
	RwIm2DVertex	m_vVertex[ 4 ];
};

