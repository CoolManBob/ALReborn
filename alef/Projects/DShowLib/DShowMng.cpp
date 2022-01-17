#include "stdafx.h"
#include "DShowMng.h"

void SaveRenderState( LPDIRECT3DDEVICE9 pd3dDevice, char* szFilename )
{
	if( !pd3dDevice || !szFilename )	return;

	FILE* fp = fopen( szFilename, "w" );
	if( !fp )							return;

	DWORD dwValue;
	char szText[256];

	#define SAVE_RENDER_STATE( Deivce, Type, Value, File, Text )		\
				Value = 0;		Deivce->GetRenderState( Type, &Value );	\
				sprintf( Text, "%s : %d\n", #Type, Value );				\
				fwrite( Text, 1, strlen(Text), File );

	for( int i=0; i<=D3DRS_BLENDOPALPHA; ++i )
	{
		DWORD value = 0;
		HRESULT hr = pd3dDevice->GetRenderState( (D3DRENDERSTATETYPE)i, &value );
		if( D3DERR_INVALIDCALL == hr  )
			_asm nop;
		if( value )
			_asm nop;
	}
	
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ZENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_FILLMODE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_SHADEMODE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ZWRITEENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ALPHATESTENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_LASTPIXEL, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_SRCBLEND, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_DESTBLEND, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ZFUNC, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ALPHAREF, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ALPHAFUNC, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_DITHERENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ALPHABLENDENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_FOGENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_SPECULARENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_FOGCOLOR, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_FOGTABLEMODE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_FOGSTART, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_FOGEND, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_FOGDENSITY, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_RANGEFOGENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_STENCILENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_STENCILFAIL, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_STENCILZFAIL, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_STENCILPASS, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_STENCILFUNC, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_STENCILREF, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_STENCILMASK, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_STENCILWRITEMASK, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_TEXTUREFACTOR, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP0, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP1, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP2, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP3, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP4, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP5, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP6, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP7, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_CLIPPING, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_LIGHTING, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_AMBIENT, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_FOGVERTEXMODE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_COLORVERTEX, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_LOCALVIEWER, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_NORMALIZENORMALS, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_DIFFUSEMATERIALSOURCE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_SPECULARMATERIALSOURCE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_AMBIENTMATERIALSOURCE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_EMISSIVEMATERIALSOURCE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_VERTEXBLEND, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_CLIPPLANEENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_POINTSIZE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_POINTSIZE_MIN, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_POINTSPRITEENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_POINTSCALEENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_POINTSCALE_A, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_POINTSCALE_B, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_POINTSCALE_C, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_MULTISAMPLEANTIALIAS, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_MULTISAMPLEMASK, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_PATCHEDGESTYLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_DEBUGMONITORTOKEN, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_POINTSIZE_MAX, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_INDEXEDVERTEXBLENDENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_COLORWRITEENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_TWEENFACTOR, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_BLENDOP, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_POSITIONDEGREE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_NORMALDEGREE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_SCISSORTESTENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_SLOPESCALEDEPTHBIAS, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ANTIALIASEDLINEENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_MINTESSELLATIONLEVEL, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_MAXTESSELLATIONLEVEL, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ADAPTIVETESS_X, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ADAPTIVETESS_Y, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ADAPTIVETESS_Z, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ADAPTIVETESS_W, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_ENABLEADAPTIVETESSELLATION, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_TWOSIDEDSTENCILMODE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_CCW_STENCILFAIL, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_CCW_STENCILZFAIL, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_CCW_STENCILPASS, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_CCW_STENCILFUNC, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_COLORWRITEENABLE1, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_COLORWRITEENABLE2, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_COLORWRITEENABLE3, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_BLENDFACTOR, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_SRGBWRITEENABLE, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_DEPTHBIAS, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP8, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP9, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP10, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP11, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP12, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP13, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP14, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_WRAP15, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_SRCBLENDALPHA, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_DESTBLENDALPHA, dwValue, fp, szText );
	SAVE_RENDER_STATE( pd3dDevice, D3DRS_BLENDOPALPHA, dwValue, fp, szText );

	fclose( fp );
}
    


//extern HRESULT UpgradeGeometry( LONG lActualW, LONG lTextureW, LONG lActualH, LONG lTextureH );

//-------------------------- CDShowTexture --------------------------------
CDShowTexture::CDShowTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPUNKNOWN pUnk, HRESULT *phr ) : 
 CBaseVideoRenderer( __uuidof(CLSID_TextureRenderer), NAME("Texture Renderer"), pUnk, phr ),
 m_pd3dDevice(pd3dDevice),
 m_pTexture(NULL),
 m_bUseDynamicTextures( FALSE )
{
	//// Store and AddRef the texture for our use.
	ASSERT(phr);
	if (phr)
		*phr = S_OK;
}

CDShowTexture::~CDShowTexture()
{
	Destory();
}

void	CDShowTexture::Destory()
{
	if( m_pTexture )
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

HRESULT CDShowTexture::CheckMediaType( const CMediaType *pmt )
{
	CheckPointer( pmt, E_POINTER );

	// Reject the connection if this is not a video type
	if( FORMAT_VideoInfo != *pmt->FormatType() )	return E_INVALIDARG;

	// Only accept RGB24 video
	VIDEOINFO* pvi = (VIDEOINFO*)pmt->Format();
	if( IsEqualGUID( *pmt->Type(), MEDIATYPE_Video ) && IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24 ) )
		return S_OK;

	return E_FAIL;
}

HRESULT CDShowTexture::SetMediaType( const CMediaType *pmt )
{
	// Retrive the size of this media type
	VIDEOINFO* pviBmp = (VIDEOINFO *)pmt->Format();
	m_lVidWidth  = pviBmp->bmiHeader.biWidth;
	m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
	m_lVidPitch  = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24

	// here let's check if we can use dynamic textures
	D3DCAPS9 caps;
	ZeroMemory( &caps, sizeof(D3DCAPS9));
	HRESULT hr = m_pd3dDevice->GetDeviceCaps( &caps );
	//[KTH] DynamicTexture를 사용하지 않는다
	//if( caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES )
	//{
	//	m_bUseDynamicTextures = TRUE;
	//}

	UINT uintWidth = 2;
	UINT uintHeight = 2;
	if( caps.TextureCaps & D3DPTEXTURECAPS_POW2 )
	{
		while( (LONG)uintWidth < m_lVidWidth )
			uintWidth = uintWidth << 1;

		while( (LONG)uintHeight < m_lVidHeight )
			uintHeight = uintHeight << 1;

		//UpgradeGeometry( m_lVidWidth, uintWidth, m_lVidHeight, uintHeight );
	}
	else
	{
		uintWidth	= m_lVidWidth;
		uintHeight	= m_lVidHeight;
	}

	// Create the texture that maps to this media type
	hr = E_UNEXPECTED;
	if( m_bUseDynamicTextures )
	{
		hr = m_pd3dDevice->CreateTexture( uintWidth, uintHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture, NULL );
		if( FAILED( hr ) )
			m_bUseDynamicTextures = FALSE;
	}

	if( !m_bUseDynamicTextures )
		hr = m_pd3dDevice->CreateTexture( uintWidth, uintHeight, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pTexture, NULL );

	if( FAILED( hr ) )
	{
		CDShowMng::Message( TEXT( "Could not create the D3DX texture!  hr=0x%x" ), hr );
		return hr;
	}

	// CreateTexture can silently change the parameters on us
	D3DSURFACE_DESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));

	if( FAILED( hr = m_pTexture->GetLevelDesc( 0, &ddsd ) ) )
	{
		CDShowMng::Message( TEXT("Could not get level Description of D3DX texture! hr = 0x%x"), hr );
		return hr;
	}

	CComPtr<IDirect3DSurface9> pSurf; 
	if( SUCCEEDED( hr = m_pTexture->GetSurfaceLevel( 0, &pSurf ) ) )
		pSurf->GetDesc(&ddsd);

	// Save format info
	m_cTextureFormat = ddsd.Format;

	if( D3DFMT_X8R8G8B8 != m_cTextureFormat && D3DFMT_A1R5G5B5 != m_cTextureFormat )
	{
		CDShowMng::Message( TEXT( "Texture is format we can't handle! Format = 0x%x"), ddsd.Format );
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	return S_OK;
}

HRESULT CDShowTexture::DoRenderSample( IMediaSample* pSample )
{
	BYTE  *pBmpBuffer, *pTxtBuffer; // Bitmap buffer, texture buffer

	BYTE* pbS = NULL;
	DWORD* pdwS = NULL;
	DWORD* pdwD = NULL;

	CheckPointer( pSample, E_POINTER );
	CheckPointer( m_pTexture, E_UNEXPECTED );

	// Get the video bitmap buffer
	pSample->GetPointer( &pBmpBuffer );

	// Lock the Texture
	D3DLOCKED_RECT d3dlr;
	if( m_bUseDynamicTextures )
	{
		if( FAILED( m_pTexture->LockRect( 0, &d3dlr, 0, D3DLOCK_DISCARD ) ) )
			return E_FAIL;
	}
	else
	{
		if ( FAILED( m_pTexture->LockRect( 0, &d3dlr, 0, 0 ) ) )
			return E_FAIL;
	}
	// Get the texture buffer & pitch
	pTxtBuffer = static_cast<byte *>(d3dlr.pBits);
	LONG lTxtPitch = d3dlr.Pitch;

	// Copy the bits    
	if( D3DFMT_X8R8G8B8 == m_cTextureFormat )
	{
		// Instead of copying data bytewise, we use DWORD alignment here.
		// We also unroll loop by copying 4 pixels at once.
		//
		// original BYTE array is [b0][g0][r0][b1][g1][r1][b2][g2][r2][b3][g3][r3]
		//
		// aligned DWORD array is     [b1 r0 g0 b0][g2 b2 r1 g1][r3 g3 b3 r2]
		//
		// We want to transform it to [ff r0 g0 b0][ff r1 g1 b1][ff r2 g2 b2][ff r3 b3 g3]
		// below, bitwise operations do exactly this.

		UINT dwordWidth = m_lVidWidth / 4; // aligned width of the row, in DWORDS
		// (pixel by 3 bytes over sizeof(DWORD))

		//마지막으로 이동후
		for( UINT row = 0; row< (UINT)m_lVidHeight; row++)
			pBmpBuffer += m_lVidPitch;

		for( UINT row = 0; row< (UINT)m_lVidHeight; row++)
		{
			pdwS = (DWORD*)pBmpBuffer;
			pdwD = (DWORD*)pTxtBuffer;

			for( UINT col = 0; col < dwordWidth; col ++ )
			{
				pdwD[0] = pdwS[0] | 0xFF000000;
				pdwD[1] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
				pdwD[2] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
				pdwD[3] = 0xFF000000 | (pdwS[2]>>8);
				pdwD +=4;
				pdwS +=3;
			}

			// 남은 바이트 처리
			pbS = (BYTE*) pdwS;
			for( UINT col = 0; col < (UINT)m_lVidWidth % 4; col++)
			{
				*pdwD = 0xFF000000 | (pbS[2] << 16) | (pbS[1] <<  8) | (pbS[0]);
				pdwD++;
				pbS += 3;           
			}

			pBmpBuffer -= m_lVidPitch;
			pTxtBuffer += lTxtPitch;
		}// for rows
	}

	if( D3DFMT_A1R5G5B5 == m_cTextureFormat )
	{
		for(int y = 0; y < m_lVidHeight; y++ )
		{
			BYTE *pBmpBufferOld = pBmpBuffer;
			BYTE *pTxtBufferOld = pTxtBuffer;   

			for (int x = 0; x < m_lVidWidth; x++)
			{
				*(WORD*)pTxtBuffer =
					(WORD)(0x8000 +
					((pBmpBuffer[2] & 0xF8) << 7) +
					((pBmpBuffer[1] & 0xF8) << 2) +
					(pBmpBuffer[0] >> 3));

				pTxtBuffer += 2;
				pBmpBuffer += 3;
			}

			pBmpBuffer = pBmpBufferOld + m_lVidPitch;
			pTxtBuffer = pTxtBufferOld + lTxtPitch;
		}
	}

	// Unlock the Texture
	return FAILED( m_pTexture->UnlockRect(0) ) ? E_FAIL : S_OK;
}
//-------------------------- CDShowSprite --------------------------------
CDShowSprite::CDShowSprite( LPDIRECT3DDEVICE9 pd3dDevice, LPUNKNOWN pUnk, HRESULT* phr ) :
 CDShowTexture( pd3dDevice, pUnk, phr ),
 m_pVB(NULL)
{
}

CDShowSprite::~CDShowSprite()
{
}

HRESULT CDShowSprite::Create( DWORD dwWidth, DWORD dwHeight )
{
	UINT iLength = 4 * sizeof(Vertex);
	HRESULT hr = m_pd3dDevice->CreateVertexBuffer( iLength, 0, Vertex::D3DFVF_VERTEX, D3DPOOL_MANAGED, &m_pVB, NULL );
	//HRESULT hr = m_pd3dDevice->CreateVertexBuffer( iLength, 0, Vertex::D3DFVF_VERTEX, D3DPOOL_DEFAULT, &m_pVB, NULL );
	if( hr != S_OK )	return E_FAIL;

    Vertex* pVertices;
    hr = m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );
	if( hr != S_OK )	return E_FAIL;

	//270도 회한 내용
	//pVertices[0].pos	= D3DXVECTOR4( (float)dwWidth - .5f, (float)dwHeight - .5f, .5f, 1.f );	
	//pVertices[0].color	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	//pVertices[0].uv		= D3DXVECTOR2( 0.f, 1.f );

	//pVertices[1].pos	= D3DXVECTOR4( -.5f, dwHeight -.5f, .5f, 1.f );
	//pVertices[1].color	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	//pVertices[1].uv		= D3DXVECTOR2( 0.f, 0.f );

	//pVertices[2].pos	= D3DXVECTOR4( (float)dwWidth - .5f, -.5f, .5f, 1.f );
	//pVertices[2].color	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	//pVertices[2].uv		= D3DXVECTOR2( 1.f, 1.f );
	//
	//pVertices[3].pos	= D3DXVECTOR4( -.5f, -.5f, .5f, 1.f );
	//pVertices[3].color	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	//pVertices[3].uv		= D3DXVECTOR2( 1.f, 0.f );

	//원래대로 그리는것 ^^
	pVertices[0].pos	= D3DXVECTOR4( -.5f, dwHeight -.5f, .5f, 1.f );
	pVertices[0].color	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	pVertices[0].uv		= D3DXVECTOR2( 0.f, 1.f );

	pVertices[1].pos	= D3DXVECTOR4( -.5f, -.5f, .5f, 1.f );
	pVertices[1].color	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	pVertices[1].uv		= D3DXVECTOR2( 0.f, 0.f );

	pVertices[2].pos	= D3DXVECTOR4( (float)dwWidth - .5f, (float)dwHeight - .5f, .5f, 1.f );	
	pVertices[2].color	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	pVertices[2].uv		= D3DXVECTOR2( 1.f, 1.f );
	
	pVertices[3].pos	= D3DXVECTOR4( (float)dwWidth - .5f, -.5f, .5f, 1.f );
	pVertices[3].color	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	pVertices[3].uv		= D3DXVECTOR2( 1.f, 0.f );

    m_pVB->Unlock();

	return S_OK;
}

void	CDShowSprite::Render( float fTime )
{
	//일단 저장한번 해보자 ^^
	//D3DXSaveTextureToFile( "MyTest.jpg", D3DXIFF_JPG, m_pTexture, NULL );

	//m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	//m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	//m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

	//SaveRenderState( m_pd3dDevice, "RenderState.txt" );

	m_pd3dDevice->SetTexture( 0, m_pTexture );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(Vertex) );
	m_pd3dDevice->SetFVF( Vertex::D3DFVF_VERTEX );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

void	CDShowSprite::Destory()
{
	CDShowTexture::Destory();

	if( m_pVB )
	{
		m_pVB->Release();
		m_pVB = NULL;
	}
}

//----------------- CDShowNode --------------------
CDShowNode::CDShowNode() : 
 m_dwROTReg(0xfedcba98)
{
}

CDShowNode::~CDShowNode()
{
}

HRESULT	CDShowNode::Create( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* szFilename, DWORD dwWidth, DWORD dwHeight )
{
	ASSERT( pd3dDevice );

	HRESULT hr = S_OK;
	
	// Create the filter graph
	if( FAILED( m_pGB.CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC ) ) )
		return E_FAIL;

#ifdef REGISTER_FILTERGRAPH
	// Register the graph in the Running Object Table (for debug purposes)
	AddToROT( m_pGB );
#endif

	// Create the Texture Renderer object
	m_pDShowSprite = new CDShowSprite( pd3dDevice, NULL, &hr );
	m_pDShowSprite->Create( dwWidth, dwHeight );

	// Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
	if( FAILED( hr = m_pGB->AddFilter( m_pDShowSprite, L"TEXTURERENDERER" ) ) )
	{
		CDShowMng::Message(TEXT("Could not add renderer filter to graph!  hr=0x%x"), hr);
		return hr;
	}

	m_strFilename = szFilename;
	// Determine the file to load based on DirectX Media path (from SDK)
	// Use a helper function included in DXUtils.cpp
	TCHAR strFileName[MAX_PATH];
	lstrcpyn( strFileName, szFilename, MAX_PATH - 1 );
	//lstrcat( strFileName, szFilename );
	strFileName[MAX_PATH-1] = 0;  // NULL-terminate

	USES_CONVERSION;
	WCHAR wFileName[MAX_PATH];
	wFileName[MAX_PATH-1] = 0;    // NULL-terminate
	wcsncpy(wFileName, T2W(strFileName), NUMELMS(wFileName));

	// Add the source filter to the graph.
	CComPtr<IBaseFilter>    pFSrc;          // Source Filter
	hr = m_pGB->AddSourceFilter( wFileName, L"SOURCE", &pFSrc );

	// If the media file was not found, inform the user.
	if (hr == VFW_E_NOT_FOUND)
	{
		CDShowMng::Message(TEXT("Could not add source filter to graph!  (hr==VFW_E_NOT_FOUND)\r\n\r\n")
			TEXT("This sample reads a media file from the DirectX SDK's media path.\r\n")
			TEXT("Please install the DirectX 9 SDK on this machine."));
		return hr;
	}
	else if(FAILED(hr))
	{
		CDShowMng::Message(TEXT("Could not add source filter to graph!  hr=0x%x"), hr);
		return hr;
	}

	CComPtr<IPin>	pFSrcPinOut;    // Source Filter Output Pin   
	if (FAILED(hr = pFSrc->FindPin(L"Output", &pFSrcPinOut)))
	{
		CDShowMng::Message(TEXT("Could not find output pin!  hr=0x%x"), hr);
		return hr;
	}

#ifdef NO_AUDIO_RENDERER

	// If no audio component is desired, directly connect the two video pins
	// instead of allowing the Filter Graph Manager to render all pins.

	CComPtr<IPin> pFTRPinIn;      // Texture Renderer Input Pin

	// Find the source's output pin and the renderer's input pin
	if (FAILED(hr = pFTR->FindPin(L"In", &pFTRPinIn)))
	{
		CDShowMng::Message(TEXT("Could not find input pin!  hr=0x%x"), hr);
		return hr;
	}

	// Connect these two filters
	if (FAILED(hr = m_pGB->Connect(pFSrcPinOut, pFTRPinIn)))
	{
		CDShowMng::Message(TEXT("Could not connect pins!  hr=0x%x"), hr);
		return hr;
	}

#else

	// Render the source filter's output pin.  The Filter Graph Manager
	// will connect the video stream to the loaded CTextureRenderer
	// and will load and connect an audio renderer (if needed).

	if( FAILED( hr = m_pGB->Render( pFSrcPinOut ) ) )
	{
		CDShowMng::Message(TEXT("Could not render source output pin!  hr=0x%x"), hr);
		return hr;
	}

#endif

	// Get the graph's media control, event & position interfaces
	m_pGB.QueryInterface( &m_pMC );
	m_pGB.QueryInterface( &m_pMP );
	m_pGB.QueryInterface( &m_pME );

	// Start the graph running;
	if( FAILED( hr = m_pMC->Run() ) )
	{
		CDShowMng::Message( TEXT("Could not run the DirectShow graph!  hr=0x%x"), hr );
		return hr;
	}

	return S_OK;
}

void	CDShowNode::Destory()
{
#ifdef REGISTER_FILTERGRAPH
	RemoveFromROT();		// Pull graph from Running Object Table (Debug)
#endif

	if( m_pDShowSprite )
		m_pDShowSprite->Destory();

	// Shut down the graph
	if (!(!m_pMC))			m_pMC->Stop();
	if (!(!m_pMC))			m_pMC.Release();
	if (!(!m_pME))			m_pME.Release();
	if (!(!m_pMP))			m_pMP.Release();
	if (!(!m_pGB))			m_pGB.Release();
}

void	CDShowNode::Render()
{
	if( m_pDShowSprite )
		m_pDShowSprite->Render( 0.f );

	//CheckMovieStatus();
}

BOOL	CDShowNode::Start()
{
	if( !m_pMC )	return FALSE;

	HRESULT hr = m_pMC->Run();
	return S_OK == hr ? TRUE : FALSE;
}

BOOL	CDShowNode::Stop()
{
	if( !m_pMC )	return FALSE;

	HRESULT hr = m_pMC->Stop();
	return S_OK == hr ? TRUE : FALSE;
}

BOOL	CDShowNode::Pause()
{
	if( !m_pMC )	return FALSE;

	HRESULT hr = m_pMC->Pause();
	return S_OK == hr ? TRUE : FALSE;
}

BOOL	CDShowNode::IsState( FILTER_STATE eState )
{
	if( !m_pMC )	return FALSE;

	OAFilterState filter;
	if( SUCCEEDED( m_pMC->GetState( INFINITE, &filter ) ) )
		return eState == filter;

	return FALSE;
}

BYTE	CDShowNode::GetEvent()
{
	if( !m_pME )	return EC_SYSTEMBASE;
	
	BYTE byEvent = EC_SYSTEMBASE;
	long lEventCode;
	LONG_PTR lParam1, lParam2;
	HRESULT hr = m_pME->GetEvent( &lEventCode, &lParam1, &lParam2, 0);
	if( SUCCEEDED( hr ) )
	{
		byEvent = (BYTE)lEventCode;
		//if( EC_COMPLETE == lEventCode )
		// hr = m_pMP->put_CurrentPosition(0);
		// Free any memory associated with this event
		m_pME->FreeEventParams( lEventCode, lParam1, lParam2 );
	}

	return byEvent;
}

HRESULT CDShowNode::AddToROT( IUnknown* pUnkGraph )
{
	IRunningObjectTable *pROT;
	if( FAILED( GetRunningObjectTable( 0, &pROT ) ) )	return E_FAIL;

	WCHAR szText[256];
	wsprintfW(szText, L"FilterGraph %08x  pid %08x\0", (DWORD_PTR) 0, GetCurrentProcessId());

	IMoniker* pmk;
	HRESULT hr = CreateItemMoniker( L"!", szText, &pmk );
	if (SUCCEEDED(hr)) 
	{
		// Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
		// to the object.  Using this flag will cause the object to remain
		// registered until it is explicitly revoked with the Revoke() method.
		//
		// Not using this flag means that if GraphEdit remotely connects
		// to this graph and then GraphEdit exits, this object registration 
		// will be deleted, causing future attempts by GraphEdit to fail until
		// this application is restarted or until the graph is registered again.
		hr = pROT->Register( ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,  pmk, &m_dwROTReg );
		pmk->Release();
	}

	pROT->Release();
	return hr;
}


void CDShowNode::RemoveFromROT()
{
	IRunningObjectTable *pirot=0;
	if ( SUCCEEDED( GetRunningObjectTable( 0, &pirot ) ) ) 
	{
		pirot->Revoke( m_dwROTReg );
		pirot->Release();
	}
}

//-------------------------- CDShowMng --------------------------------
CDShowMng::CDShowMng() :
 m_pd3dDevice(NULL)
{
}

CDShowMng::~CDShowMng()
{
	
}

HRESULT CDShowMng::CreateDevice( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return S_OK;
}

HRESULT CDShowMng::ResetDevice()
{
	//m_pd3dDevice = pd3dDevice;
	return S_OK;
}

void	CDShowMng::FrameMove( float fTime )
{
}

void	CDShowMng::FrameRender( float fTime )
{
	for_each( m_vecDShowNode.begin(), m_vecDShowNode.end(), mem_fun( &CDShowNode::Render ) );
}

void	CDShowMng::LoseDevice()
{
}

void	CDShowMng::DestoryDevice()
{
	for( DShowNodeVecItr Itr = m_vecDShowNode.begin(); Itr != m_vecDShowNode.end(); ++Itr )
	{
		(*Itr)->Destory();
		delete (*Itr);
	}

	m_vecDShowNode.clear();
}

BOOL	CDShowMng::Insert( TCHAR* szFilename, DWORD dwWidth, DWORD dwHeight )
{
	//같은 파일은 안돼..
	for( DShowNodeVecItr Itr = m_vecDShowNode.begin(); Itr != m_vecDShowNode.end(); ++Itr )
		if( (*Itr)->GetFilename() == szFilename )
			return FALSE;

	LPDShowNode pNode = new CDShowNode;
	pNode->Create( m_pd3dDevice, szFilename, dwWidth, dwHeight );
	m_vecDShowNode.push_back( pNode );
	return TRUE;
}

void	CDShowMng::Delete( TCHAR* szFilename )
{
	for( DShowNodeVecItr Itr = m_vecDShowNode.begin(); Itr != m_vecDShowNode.end(); ++Itr )
	{
		if( (*Itr)->GetFilename() == szFilename )
		{
			(*Itr)->Destory();
			delete (*Itr);
			m_vecDShowNode.erase( Itr );
			return;
		}
	}
}

LPDShowNode	CDShowMng::GetNode( TCHAR* szFilename )
{
	for( DShowNodeVecItr Itr = m_vecDShowNode.begin(); Itr != m_vecDShowNode.end(); ++Itr )
	{
		if( (*Itr)->GetFilename() == szFilename )
			return (LPDShowNode)(*Itr);
	}

	return NULL;
}

void CDShowMng::Message(TCHAR *szFormat, ...)
{
	TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
	const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	const int LASTCHAR = NUMCHARS - 1;

	// Format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);

	// Use a bounded buffer size to prevent buffer overruns.  Limit count to
	// character size minus one to allow for a NULL terminating character.
	_vsntprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);

	// Ensure that the formatted string is NULL-terminated
	szBuffer[LASTCHAR] = TEXT('\0');

	::MessageBox( NULL, szBuffer, TEXT( "DirectShow Texture3D9 Sample" ),  MB_OK | MB_ICONERROR );
}