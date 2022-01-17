#include "AcUITexture.h"

extern AgcEngine *	g_pEngine;

AcUITexture::AcUITexture( void )
{
	m_pTexture = NULL;	
	m_cAlphaValue =	255;
	m_lColorValue =	0xffffffff;
}
	
AcUITexture::~AcUITexture( void )
{
}

void AcUITexture::SetTexture( RwTexture* pTexture, UINT32 lValidWidth, UINT32 lValidHeight )
{
	m_pTexture = pTexture;
	MoveWindow( x, y, lValidWidth, lValidHeight );
}

void AcUITexture::SetAlphaValue( UINT8 cAlphaValue )
{
	m_cAlphaValue = cAlphaValue;	
}

void AcUITexture::SetColorValue( DWORD lColorValue )
{
	m_lColorValue = lColorValue;
}

void AcUITexture::OnWindowRender( void )
{
	PROFILE("AcUITexture::OnWindowRender");

	if( m_pTexture && m_pTexture->raster )
	{
		int nAbsolute_x = 0;
		int nAbsolute_y = 0;
		ClientToScreen( &nAbsolute_x, &nAbsolute_y );		
				
		g_pEngine->DrawIm2D( m_pTexture, ( float )nAbsolute_x, ( float )nAbsolute_y, 
			( float )( RwRasterGetWidth( m_pTexture->raster ) ),
			( float )( RwRasterGetHeight( m_pTexture->raster ) ),
			0.0f, 0.0f, 1.0f, 1.0f, m_lColorValue, m_cAlphaValue );
	}
}

RwTexture* AcUITexture::GetTexturePointer( void )
{
	return m_pTexture;	
}