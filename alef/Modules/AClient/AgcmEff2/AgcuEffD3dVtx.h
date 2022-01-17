#if !defined(AFX_AGCUEFFD3DVTX_H__7ECA470D_F88F_43C2_B411_537176BACC30__INCLUDED_)
#define AFX_AGCUEFFD3DVTX_H__7ECA470D_F88F_43C2_B411_537176BACC30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffUtil.h"
#include "AgcdEffAnim.h"
#include "rwcore.h"
#include "d3dx9math.h"

struct D3dVtx_PatricleExtra
{
	D3dVtx_PatricleExtra();
	D3dVtx_PatricleExtra( float sin, float cos, float psacle_x, float pscale_y );
	D3dVtx_PatricleExtra(const D3dVtx_PatricleExtra& cpy);

	D3dVtx_PatricleExtra& operator = (const D3dVtx_PatricleExtra& cpy);

	static const	DWORD	FVF;
	static const	DWORD	SIZE;

	D3DXVECTOR4		m_v4extra;	//x:sin, y:cos, z:particle_scale_x, w:particle_scale_y
};
typedef D3dVtx_PatricleExtra D3DXVTX_PARTICLEEXTRA;

//-------------------- D3dVtx_pct --------------------
struct D3dVtx_pct
{
	D3dVtx_pct();
	D3dVtx_pct( float fx, float fy, float fz, DWORD dwColr, float fu, float fv);
	D3dVtx_pct( const D3DXVECTOR3& v3Pos, DWORD dwColr, const D3DXVECTOR2& v2UV );
	D3dVtx_pct(const D3dVtx_pct& cpy);

	D3dVtx_pct& operator = (const D3dVtx_pct& cpy);

	static const	DWORD	FVF;
	static const	DWORD	SIZE;

	D3DXVECTOR3		m_v3Pos;
	DWORD			m_dwColr;
	D3DXVECTOR2		m_v2Tutv;
};
typedef D3dVtx_pct D3DVTX_PCT, *PD3DVTX_PCT, *LPD3DVTX_PCT;

//-------------------- D3dVtx_pct_Rect --------------------
struct D3dVtx_pct_Rect	//
{
	// 0        2
	//  ********
	//  *    * *
	//  *  *   *
	//  * *    *
	//  ********  
	// 1        3
	D3dVtx_pct_Rect( float fDeg=0.f );
	D3dVtx_pct_Rect( DWORD dwColr, const STUVRECT& stUVRect );
	D3dVtx_pct_Rect( float fwidth, float fheight, DWORD dwColr, float fDeg=0.f);
	D3dVtx_pct_Rect( float fwidth, float fheight, float fDeg=0.f);
	D3dVtx_pct_Rect( float fwidth, float fheight, DWORD dwColr, const STUVRECT& stUVRect, float fDeg=0.f);
	D3dVtx_pct_Rect( float fwidth, float fheight, const D3DXVECTOR3& v3dCenter, DWORD dwColr, const STUVRECT& stUVRect, float fDeg=0.f );
	D3dVtx_pct_Rect( const D3dVtx_pct_Rect& cpy );

	D3dVtx_pct_Rect& operator = ( const D3dVtx_pct_Rect& cpy );

	void SetColr(DWORD dwColr)	
	{ 
		m_vtx0.m_dwColr = 
		m_vtx1.m_dwColr = 
		m_vtx2.m_dwColr = 
		m_vtx3.m_dwColr = dwColr; 
	};
	void SetUV( const STUVRECT& stUVRect )
	{
		m_vtx0.m_v2Tutv.x = stUVRect.m_fLeft , m_vtx0.m_v2Tutv.y = stUVRect.m_fTop;
		m_vtx1.m_v2Tutv.x = stUVRect.m_fLeft , m_vtx1.m_v2Tutv.y = stUVRect.m_fBottom;
		m_vtx2.m_v2Tutv.x = stUVRect.m_fRight, m_vtx2.m_v2Tutv.y = stUVRect.m_fTop;
		m_vtx3.m_v2Tutv.x = stUVRect.m_fRight, m_vtx3.m_v2Tutv.y = stUVRect.m_fBottom;
	};
	void TransForm( const D3DXMATRIX* pMat );
	void Translate( const D3DXVECTOR3& v3dPos );

	static const	DWORD	SIZE;

	D3DVTX_PCT	m_vtx0,	m_vtx1,	m_vtx2,	m_vtx3;
};
typedef D3dVtx_pct_Rect D3DVTX_PCT_RECT, *PD3DVTX_PCT_RECT, *LPD3DVTX_PCT_RECT;

//-------------------- RectIndex --------------------
struct RectIndex
{
	// 0        2,3
	//  ********
	//  *      *
	//  *      *
	//  *      *
	//  ********
	// 1,4     5

	RectIndex();
	RectIndex( WORD wStartIndex );
	RectIndex( const RectIndex& cpy );

	RectIndex& operator = ( const RectIndex& cpy );

	void SetIndex( WORD wStartIndex=0 );

	// 0,1,2, 2,1,3
	static const DWORD		USAGE;
	static const D3DFORMAT	FORMAT;
	static const DWORD		SIZE;

	union
	{
		struct
		{
			WORD m_wIndex0,
				 m_wIndex1,
				 m_wIndex2,
				 m_wIndex3,
				 m_wIndex4,
				 m_wIndex5;
		};

		WORD m_awIndex[6];
		WORD m_aawIndex[2][3];
	};
};
typedef RectIndex RECTINDEX, *PRECTINDEX, *LPRECTINDEX;

//-------------------- D3dVtx_pct_2 --------------------
struct D3dVtx_pct_2
{
	D3dVtx_pct_2();
	D3dVtx_pct_2( const D3DXVECTOR3& v3pt1, const D3DXVECTOR3& v3pt2, DWORD dwColr, const D3DXVECTOR2& v2uv1, const D3DXVECTOR2& v2uv2 );
	D3dVtx_pct_2( const D3dVtx_pct_2& cpy );

	D3dVtx_pct_2& operator = ( const D3dVtx_pct_2& cpy );

	void SetColr(DWORD dwColr)		{	m_vtx0.m_dwColr = m_vtx1.m_dwColr = dwColr;	}
	void SetUV( const D3DXVECTOR2& v2uv1, const D3DXVECTOR2& v2uv2)
	{
		m_vtx0.m_v2Tutv	= v2uv1,
		m_vtx1.m_v2Tutv	= v2uv2;
	}

	static const	DWORD	SIZE;

	D3DVTX_PCT	m_vtx0,	m_vtx1;
};
typedef D3dVtx_pct_2 D3DVTX_PCT_2, *PD3DVTX_PCT_2, *LPD3DVTX_PCT_2;

#endif