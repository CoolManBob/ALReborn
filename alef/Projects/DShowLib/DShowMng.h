#pragma once

#define REGISTER_FILTERGRAPH

#include <d3dx9.h>
#include <d3d9.h>
#include <atlbase.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include "stdafx.h"

using namespace std;

struct __declspec( uuid("{71771540-2017-11cf-ae26-0020afd79767}") )		CLSID_TextureRenderer;

class CDShowTexture : public CBaseVideoRenderer
{
	friend class CDShowNode;

public:
	CDShowTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPUNKNOWN pUnk, HRESULT *phr );
	~CDShowTexture();

	HRESULT CheckMediaType( const CMediaType* pmt );	// Format acceptable?
    HRESULT SetMediaType( const CMediaType* pmt );		// Video format notification
    HRESULT DoRenderSample( IMediaSample* pSample );	// New video sample

	LPDIRECT3DTEXTURE9	GetTexture()	{	return m_pTexture;		}
	LPDIRECT3DDEVICE9	GetDevice()		{	return m_pd3dDevice;	}

protected:
	void	Destory();

protected:
	BOOL					m_bUseDynamicTextures;
    LONG					m_lVidWidth;   // Video width
    LONG					m_lVidHeight;  // Video Height
    LONG					m_lVidPitch;   // Video Pitch

	D3DFORMAT				m_cTextureFormat;	// Texture format
	LPDIRECT3DTEXTURE9		m_pTexture;
	LPDIRECT3DDEVICE9		m_pd3dDevice;
};

//----------------------------- CDShowSprite  -----------------------
class CDShowSprite : public CDShowTexture
{
	friend class CDShowNode;

public:
	struct Vertex
	{
		D3DXVECTOR4	pos;
		D3DCOLOR	color;
		D3DXVECTOR2	uv;

		const static int D3DFVF_VERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	};

public:
	CDShowSprite( LPDIRECT3DDEVICE9 pd3dDevice, LPUNKNOWN pUnk, HRESULT *phr );
	~CDShowSprite();

	HRESULT Create( DWORD dwWidth, DWORD dwHeight );
	void	Render( float fTime );
	void	Destory();

protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
};
typedef CDShowSprite	*LPDShowSprite;
//typedef map< string, LPDShowSprite >	SpriteMap;
//typedef SpriteMap::iterator				SpriteMapItr;

//----------------- CDShowNode --------------------
class CDShowNode
{
public:
	CDShowNode();
	~CDShowNode();

	HRESULT	Create( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* szFilename, DWORD dwWidth, DWORD dwHeight );
	void	Destory();
	void	Render();

	BOOL	Start();
	BOOL	Stop();		//정지 이미지 안나옴
	BOOL	Pause();	//정지 이미지 나옴
	BOOL	IsState( FILTER_STATE eState );
	BYTE	GetEvent();

	const string& GetFilename()		{	return m_strFilename;	}
	LPDIRECT3DTEXTURE9 GetTexture()	{	return m_pDShowSprite->m_pTexture;	}

private:
	void	RemoveFromROT();
	HRESULT	AddToROT( IUnknown* pUnkGraph );
	
private:
	DWORD					m_dwROTReg;

	CComPtr<IGraphBuilder>  m_pGB;				// GraphBuilder
	CComPtr<IMediaControl>	m_pMC;				// Media Control
	CComPtr<IMediaPosition>	m_pMP;				// Media Position
	CComPtr<IMediaEvent>	m_pME;				// Media Event

	string					m_strFilename;
	LPDShowSprite			m_pDShowSprite;
};
typedef CDShowNode	*LPDShowNode;
typedef vector< LPDShowNode >	DShowNodeVec;
typedef DShowNodeVec::iterator	DShowNodeVecItr;

//----------------- CDShowMng --------------------
class CDShowMng
{
public:
	CDShowMng();
	~CDShowMng();

	HRESULT CreateDevice( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT ResetDevice();
	void	FrameMove( float fTime );
	void	FrameRender( float fTime );
	void	LoseDevice();
	void	DestoryDevice();

	BOOL	Insert( TCHAR* szFilename, DWORD dwWidth, DWORD dwHeight );
	void	Delete( TCHAR* szFilename );
	LPDShowNode	GetNode( TCHAR* szFilename );

	static void	Message( TCHAR* szFormet, ... );

private:
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	DShowNodeVec			m_vecDShowNode;
};
