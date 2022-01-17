#include "Image2D.h"

Image2D::Image2D(void)
{
	ZeroMemory( m_vVertex , sizeof(m_vVertex) );
}

Image2D::~Image2D(void)
{
}

void	Image2D::SetCameraTrans( const RwV3d& vCameraPos, float fRecipZ, float fScreenZ )
{
	for( int i=0; i<4; ++i )
	{
		RwIm2DVertexSetCameraX( &m_vVertex[i], vCameraPos.x );
		RwIm2DVertexSetCameraY( &m_vVertex[i], vCameraPos.y );
		RwIm2DVertexSetCameraZ( &m_vVertex[i], vCameraPos.z );

		RwIm2DVertexSetRecipCameraZ( &m_vVertex[i], fRecipZ );
		RwIm2DVertexSetScreenZ( &m_vVertex[i], fScreenZ );
	}
}

void	Image2D::SetPos( FLOAT nX, FLOAT nY, FLOAT nWidth, FLOAT nHeight )
{
	RwIm2DVertexSetScreenX( &m_vVertex[0], nX );
	RwIm2DVertexSetScreenY( &m_vVertex[0], nY );

	RwIm2DVertexSetScreenX( &m_vVertex[1], nX + nWidth );
	RwIm2DVertexSetScreenY( &m_vVertex[1], nY );

	RwIm2DVertexSetScreenX( &m_vVertex[2], nX + nWidth );
	RwIm2DVertexSetScreenY( &m_vVertex[2], nY + nHeight);

	RwIm2DVertexSetScreenX( &m_vVertex[3], nX );
	RwIm2DVertexSetScreenY( &m_vVertex[3], nY + nHeight );
}

void	Image2D::SetUV( FLOAT fLeft, FLOAT fTop, FLOAT fRight, FLOAT fBottom, FLOAT fZ )
{
	RwIm2DVertexSetU( &m_vVertex[0], fLeft, fZ );
	RwIm2DVertexSetV( &m_vVertex[0], fTop, fZ );

	RwIm2DVertexSetU( &m_vVertex[1], fRight, fZ );
	RwIm2DVertexSetV( &m_vVertex[1], fTop, fZ );

	RwIm2DVertexSetU( &m_vVertex[2], fRight, fZ );
	RwIm2DVertexSetV( &m_vVertex[2], fBottom, fZ );

	RwIm2DVertexSetU( &m_vVertex[3], fLeft, fZ );
	RwIm2DVertexSetV( &m_vVertex[3], fBottom, fZ );
}

void	Image2D::SetPos( const RwV2d& v0, const RwV2d& v1, const RwV2d& v2, const RwV2d& v3 )
{
	RwIm2DVertexSetScreenX( &m_vVertex[0], v0.x );
	RwIm2DVertexSetScreenY( &m_vVertex[0], v0.y );

	RwIm2DVertexSetScreenX( &m_vVertex[1], v1.x );
	RwIm2DVertexSetScreenY( &m_vVertex[1], v1.x );

	RwIm2DVertexSetScreenX( &m_vVertex[2], v2.x );
	RwIm2DVertexSetScreenY( &m_vVertex[2], v2.x );

	RwIm2DVertexSetScreenX( &m_vVertex[3], v3.x );
	RwIm2DVertexSetScreenY( &m_vVertex[3], v3.x );
}

void	Image2D::SetUV( const RwV2d& v0, const RwV2d& v1, const RwV2d& v2, const RwV2d& v3, float fZ )
{
	RwIm2DVertexSetU( &m_vVertex[0], v0.x, fZ );
	RwIm2DVertexSetV( &m_vVertex[0], v0.y, fZ );

	RwIm2DVertexSetU( &m_vVertex[1], v1.x, fZ );
	RwIm2DVertexSetV( &m_vVertex[1], v1.y, fZ );

	RwIm2DVertexSetU( &m_vVertex[2], v2.x, fZ );
	RwIm2DVertexSetV( &m_vVertex[2], v2.y, fZ );

	RwIm2DVertexSetU( &m_vVertex[3], v3.x, fZ );
	RwIm2DVertexSetV( &m_vVertex[3], v3.y, fZ );
}

void	Image2D::SetColor( BYTE byAlpha, BYTE byRed, BYTE byGreen, BYTE byBlue )
{
	for( int i=0; i<4; ++i )
		RwIm2DVertexSetIntRGBA( &m_vVertex[i], byRed, byGreen, byBlue, byAlpha );
}

void    Image2D::SetColor( RwRGBA& rwColor )
{
	SetColor( rwColor.alpha , rwColor.red , rwColor.green , rwColor.blue );
}

void	Image2D::Render( RwPrimitiveType eRenderType, RwRaster* pRaster )
{
	if( pRaster )
		RwRenderStateSet( rwRENDERSTATETEXTURERASTER, (void*)pRaster );

	RwIm2DRenderPrimitive( rwPRIMTYPETRIFAN, m_vVertex, 4 );
}
