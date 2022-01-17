// AgcuEffD3dVtx.cpp: implementation of the AgcuEffD3dVtx class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcuEffD3dVtx.h"
#include "AgcuEffTable.h"
#include "AgcuEffUtil.h"

#include "ApMemoryTracker.h"


const float	DEFAULT_HALFSIZE	= 7.064f;
const DWORD	D3FAULT_COLOR		= 0xffffffff;//D3FAULT_COLOR;


//----------------------------- D3dVtx_PatricleExtra ------------------------
const DWORD	D3dVtx_PatricleExtra::FVF	= D3DFVF_XYZRHW;
const DWORD D3dVtx_PatricleExtra::SIZE	= sizeof( D3dVtx_PatricleExtra );
D3dVtx_PatricleExtra::D3dVtx_PatricleExtra() : m_v4extra(0.f,0.f,0.f,0.f)
{
};
D3dVtx_PatricleExtra::D3dVtx_PatricleExtra( float sin, float cos, float pscale_x, float pscale_y ) : m_v4extra(sin, cos, pscale_x, pscale_y)
{
};
D3dVtx_PatricleExtra::D3dVtx_PatricleExtra(const D3dVtx_PatricleExtra& cpy) : m_v4extra(cpy.m_v4extra)
{
};
D3dVtx_PatricleExtra& D3dVtx_PatricleExtra::operator =(const D3dVtx_PatricleExtra& cpy)
{
	m_v4extra = cpy.m_v4extra;
	return *this;
}

//----------------------------- D3dVtx_pct ------------------------
const DWORD	D3dVtx_pct::FVF	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
const DWORD D3dVtx_pct::SIZE	= sizeof( D3dVtx_pct );

D3dVtx_pct::D3dVtx_pct() :
 m_v3Pos(0.f, 0.f, 0.f),
 m_dwColr(D3FAULT_COLOR),
 m_v2Tutv(0.f, 0.f)
{
}

D3dVtx_pct::D3dVtx_pct( float fx, float fy, float fz, DWORD dwColr, float fu, float fv) : 
 m_v3Pos( fx,fy,fz ),
 m_dwColr(dwColr),
 m_v2Tutv(fu,fv)
{
}

D3dVtx_pct::D3dVtx_pct( const D3DXVECTOR3& v3Pos, DWORD dwColr, const D3DXVECTOR2& v2UV ) : 
 m_v3Pos( v3Pos ),
 m_dwColr( dwColr ),
 m_v2Tutv( v2UV )
{
}

D3dVtx_pct::D3dVtx_pct(const D3dVtx_pct& cpy) :
 m_v3Pos(cpy.m_v3Pos),
 m_dwColr(cpy.m_dwColr),
 m_v2Tutv(cpy.m_v2Tutv)
{
}

D3dVtx_pct& D3dVtx_pct::operator = (const D3dVtx_pct& cpy)
{
	m_v3Pos		= cpy.m_v3Pos;
	m_dwColr	= cpy.m_dwColr;
	m_v2Tutv	= cpy.m_v2Tutv;

	return *this;
}

//----------------------------- D3dVtx_pct_Rect ------------------------
const DWORD D3dVtx_pct_Rect::SIZE	= sizeof( D3dVtx_pct_Rect );
D3dVtx_pct_Rect::D3dVtx_pct_Rect(float fDeg) : 
 m_vtx0( -DEFAULT_HALFSIZE,  DEFAULT_HALFSIZE, 0.f, D3FAULT_COLOR, 0.f, 0.f ),
 m_vtx1( -DEFAULT_HALFSIZE, -DEFAULT_HALFSIZE, 0.f, D3FAULT_COLOR, 0.f, 1.f ),
 m_vtx2(  DEFAULT_HALFSIZE,  DEFAULT_HALFSIZE, 0.f, D3FAULT_COLOR, 1.f, 0.f ),
 m_vtx3(  DEFAULT_HALFSIZE, -DEFAULT_HALFSIZE, 0.f, D3FAULT_COLOR, 1.f, 1.f )
{
	if( fDeg )
	{
		D3DXMATRIX	mat;
		D3DXMatrixRotationZ( &mat, DEF_D2R( fDeg ) );
		D3DXVec3TransformCoord( &m_vtx0.m_v3Pos, &m_vtx0.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx1.m_v3Pos, &m_vtx1.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx2.m_v3Pos, &m_vtx2.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx3.m_v3Pos, &m_vtx3.m_v3Pos, &mat );
	}
}

D3dVtx_pct_Rect::D3dVtx_pct_Rect( DWORD dwColr, const STUVRECT& stUVRect ) :
 m_vtx0( -DEFAULT_HALFSIZE,  DEFAULT_HALFSIZE, 0.f, dwColr, stUVRect.m_fLeft , stUVRect.m_fTop ),
 m_vtx1( -DEFAULT_HALFSIZE, -DEFAULT_HALFSIZE, 0.f, dwColr, stUVRect.m_fLeft , stUVRect.m_fBottom ),
 m_vtx2(  DEFAULT_HALFSIZE,  DEFAULT_HALFSIZE, 0.f, dwColr, stUVRect.m_fRight, stUVRect.m_fTop ),
 m_vtx3(  DEFAULT_HALFSIZE, -DEFAULT_HALFSIZE, 0.f, dwColr, stUVRect.m_fRight, stUVRect.m_fBottom )
{
};

D3dVtx_pct_Rect::D3dVtx_pct_Rect(float fwidth, float fheight, float fDeg) :
 m_vtx0( fwidth*(-0.5f), fheight*0.5f	 , 0.f, D3FAULT_COLOR, 0.f, 0.f ),
 m_vtx1( fwidth*(-0.5f), fheight*(-0.5f), 0.f, D3FAULT_COLOR, 0.f, 1.f ),
 m_vtx2( fwidth*0.5f	, fheight*0.5f	 , 0.f, D3FAULT_COLOR, 1.f, 0.f ),
 m_vtx3( fwidth*0.5f   , fheight*(-0.5f), 0.f, D3FAULT_COLOR, 1.f, 1.f )
{
	if( fDeg )
	{
		D3DXMATRIX	mat;
		D3DXMatrixRotationZ( &mat, DEF_D2R( fDeg ) );
		D3DXVec3TransformCoord( &m_vtx0.m_v3Pos, &m_vtx0.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx1.m_v3Pos, &m_vtx1.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx2.m_v3Pos, &m_vtx2.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx3.m_v3Pos, &m_vtx3.m_v3Pos, &mat );
	}
}

D3dVtx_pct_Rect::D3dVtx_pct_Rect(float fwidth, float fheight, DWORD dwColr, float fDeg):
 m_vtx0( fwidth*(-0.5f), fheight*0.5f	 , 0.f, dwColr, 0.f, 0.f ),
 m_vtx1( fwidth*(-0.5f), fheight*(-0.5f), 0.f, dwColr, 0.f, 1.f ),
 m_vtx2( fwidth*0.5f	, fheight*0.5f	 , 0.f, dwColr, 1.f, 0.f ),
 m_vtx3( fwidth*0.5f   , fheight*(-0.5f), 0.f, dwColr, 1.f, 1.f )
{
	if( fDeg )
	{
		D3DXMATRIX	mat;
		D3DXMatrixRotationZ( &mat, DEF_D2R( fDeg ) );
		D3DXVec3TransformCoord( &m_vtx0.m_v3Pos, &m_vtx0.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx1.m_v3Pos, &m_vtx1.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx2.m_v3Pos, &m_vtx2.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx3.m_v3Pos, &m_vtx3.m_v3Pos, &mat );
	}
}

D3dVtx_pct_Rect::D3dVtx_pct_Rect(float fwidth, float fheight, DWORD dwColr, const STUVRECT& stUVRect, float fDeg) :
 m_vtx0( fwidth*(-0.5f), fheight*0.5f	 , 0.f, dwColr, stUVRect.m_fLeft , stUVRect.m_fTop ),
 m_vtx1( fwidth*(-0.5f), fheight*(-0.5f), 0.f, dwColr, stUVRect.m_fLeft , stUVRect.m_fBottom ),
 m_vtx2( fwidth*0.5f	, fheight*0.5f	 , 0.f, dwColr, stUVRect.m_fRight, stUVRect.m_fTop ),
 m_vtx3( fwidth*0.5f   , fheight*(-0.5f), 0.f, dwColr, stUVRect.m_fRight, stUVRect.m_fBottom )
{
	if( fDeg )
	{
		D3DXMATRIX	mat;
		D3DXMatrixRotationZ( &mat, DEF_D2R( fDeg ) );
		D3DXVec3TransformCoord( &m_vtx0.m_v3Pos, &m_vtx0.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx1.m_v3Pos, &m_vtx1.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx2.m_v3Pos, &m_vtx2.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx3.m_v3Pos, &m_vtx3.m_v3Pos, &mat );
	}
}

D3dVtx_pct_Rect::D3dVtx_pct_Rect(float fwidth, float fheight, const D3DXVECTOR3& v3dCenter, DWORD dwColr, const STUVRECT& stUVRect, float fDeg):
 m_vtx0( fwidth*(-0.5f)+v3dCenter.x, fheight*0.5f	 +v3dCenter.y, v3dCenter.z, dwColr, stUVRect.m_fLeft , stUVRect.m_fTop ),
 m_vtx1( fwidth*(-0.5f)+v3dCenter.x, fheight*(-0.5f)+v3dCenter.y, v3dCenter.z, dwColr, stUVRect.m_fLeft , stUVRect.m_fBottom ),
 m_vtx2( fwidth*0.5f	+v3dCenter.x, fheight*0.5f	 +v3dCenter.y, v3dCenter.z, dwColr, stUVRect.m_fRight, stUVRect.m_fTop ),
 m_vtx3( fwidth*0.5f   +v3dCenter.x, fheight*(-0.5f)+v3dCenter.y, v3dCenter.z, dwColr, stUVRect.m_fRight, stUVRect.m_fBottom )
{
	if( fDeg )
	{
		m_vtx0.m_v3Pos	-= v3dCenter;
		m_vtx1.m_v3Pos	-= v3dCenter;
		m_vtx2.m_v3Pos	-= v3dCenter;
		m_vtx3.m_v3Pos	-= v3dCenter;

		D3DXMATRIX	mat;
		D3DXMatrixRotationZ( &mat, DEF_D2R( fDeg ) );
		D3DXVec3TransformCoord( &m_vtx0.m_v3Pos, &m_vtx0.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx1.m_v3Pos, &m_vtx1.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx2.m_v3Pos, &m_vtx2.m_v3Pos, &mat );
		D3DXVec3TransformCoord( &m_vtx3.m_v3Pos, &m_vtx3.m_v3Pos, &mat );
		
		m_vtx0.m_v3Pos	+= v3dCenter;
		m_vtx1.m_v3Pos	+= v3dCenter;
		m_vtx2.m_v3Pos	+= v3dCenter;
		m_vtx3.m_v3Pos	+= v3dCenter;
	}
}

D3dVtx_pct_Rect::D3dVtx_pct_Rect(const D3dVtx_pct_Rect& cpy):
 m_vtx0(cpy.m_vtx0),
 m_vtx1(cpy.m_vtx1),
 m_vtx2(cpy.m_vtx2),
 m_vtx3(cpy.m_vtx3)
{
}

D3dVtx_pct_Rect& D3dVtx_pct_Rect::operator = (const D3dVtx_pct_Rect& cpy)
{
	m_vtx0 = cpy.m_vtx0;
	m_vtx1 = cpy.m_vtx1;
	m_vtx2 = cpy.m_vtx2;
	m_vtx3 = cpy.m_vtx3;

	return *this;
}

void D3dVtx_pct_Rect::TransForm( const D3DXMATRIX* pMat )
{
	D3DXVec3TransformCoord( &m_vtx0.m_v3Pos, &m_vtx0.m_v3Pos, pMat );
	D3DXVec3TransformCoord( &m_vtx1.m_v3Pos, &m_vtx1.m_v3Pos, pMat );
	D3DXVec3TransformCoord( &m_vtx2.m_v3Pos, &m_vtx2.m_v3Pos, pMat );
	D3DXVec3TransformCoord( &m_vtx3.m_v3Pos, &m_vtx3.m_v3Pos, pMat );
}

void D3dVtx_pct_Rect::Translate( const D3DXVECTOR3& v3dPos )
{
	m_vtx0.m_v3Pos	+= v3dPos;
	m_vtx1.m_v3Pos	+= v3dPos;
	m_vtx2.m_v3Pos	+= v3dPos;
	m_vtx3.m_v3Pos	+= v3dPos;
}

//----------------------------- RectIndex ------------------------
const DWORD		RectIndex::USAGE	= D3DUSAGE_WRITEONLY;
const D3DFORMAT	RectIndex::FORMAT	= D3DFMT_INDEX16;
const DWORD		RectIndex::SIZE		= sizeof(RectIndex);

RectIndex::RectIndex() :
 m_wIndex0(0),
 m_wIndex1(1),
 m_wIndex2(2),
 m_wIndex3(2),
 m_wIndex4(1),
 m_wIndex5(3)
{
}

RectIndex::RectIndex(WORD wStartIndex) :
 m_wIndex0(wStartIndex),
 m_wIndex1(wStartIndex+1),
 m_wIndex2(wStartIndex+2),
 m_wIndex3(wStartIndex+2),
 m_wIndex4(wStartIndex+1),
 m_wIndex5(wStartIndex+3)
{
}

RectIndex::RectIndex(const RectIndex& cpy) :
 m_wIndex0(cpy.m_wIndex0),
 m_wIndex1(cpy.m_wIndex1),
 m_wIndex2(cpy.m_wIndex2),
 m_wIndex3(cpy.m_wIndex3),
 m_wIndex4(cpy.m_wIndex4),
 m_wIndex5(cpy.m_wIndex5)
{
}

RectIndex& RectIndex::operator = (const RectIndex& cpy)
{
	m_wIndex0 = cpy.m_wIndex0,
	m_wIndex1 = cpy.m_wIndex1,
	m_wIndex2 = cpy.m_wIndex2,
	m_wIndex3 = cpy.m_wIndex3,
	m_wIndex4 = cpy.m_wIndex4,
	m_wIndex5 = cpy.m_wIndex5;

	return *this;
}

void RectIndex::SetIndex( WORD wStartIndex )
{
	m_wIndex0 = wStartIndex  ;
	m_wIndex1 = wStartIndex+1;
	m_wIndex2 = wStartIndex+2;
	m_wIndex3 = wStartIndex+2;
	m_wIndex4 = wStartIndex+1;
	m_wIndex5 = wStartIndex+3;
}

//----------------------------- D3dVtx_pct_2 ------------------------
const DWORD D3dVtx_pct_2::SIZE	= sizeof( D3dVtx_pct_2 );
D3dVtx_pct_2::D3dVtx_pct_2():
 m_vtx0( 0.f,  DEFAULT_HALFSIZE, 0.f, D3FAULT_COLOR, 0.f, 0.f ),
 m_vtx1( 0.f, -DEFAULT_HALFSIZE, 0.f, D3FAULT_COLOR, 0.f, 1.f )
{
}

D3dVtx_pct_2::D3dVtx_pct_2(const D3DXVECTOR3& v3pt1, const D3DXVECTOR3& v3pt2, DWORD dwColr, const D3DXVECTOR2& v2uv1, const D3DXVECTOR2& v2uv2 ) :
 m_vtx0( v3pt1, dwColr, v2uv1 ),
 m_vtx1( v3pt2, dwColr, v2uv2 )
{
}

D3dVtx_pct_2::D3dVtx_pct_2( const D3dVtx_pct_2& cpy ) :
 m_vtx0(cpy.m_vtx0),
 m_vtx1(cpy.m_vtx1)
{
}

D3dVtx_pct_2& D3dVtx_pct_2::operator = ( const D3dVtx_pct_2& cpy )
{
	m_vtx0 = cpy.m_vtx0;
	m_vtx1 = cpy.m_vtx1;

	return *this;
}
