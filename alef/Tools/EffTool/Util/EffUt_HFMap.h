// EffUt_HFMap.h: interface for the CEffUt_HFMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFUT_HFMAP_H__6473D2BD_9498_4002_A9AB_C6D8FFB45E09__INCLUDED_)
#define AFX_EFFUT_HFMAP_H__6473D2BD_9498_4002_A9AB_C6D8FFB45E09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "rwcore.h"
#include "EffUt_Geo.h"
#include "BitmapLoader.h"

#include "PickUser.h"
#include "Camera.h"

#ifndef EFFUT_BEGIN
#define EFFUT_BEGIN	namespace __EffUtil__ {
#define EFFUT_END	};
#define USING_EFFUT	using namespace __EffUtil__;
#define EFFUT		__EffUtil__
#endif

EFFUT_BEGIN

template<class VTX_TYPE>
class CEffUt_HFMap  
{
	typedef std::vector<VTX_TYPE>			STLVEC_VTX;
	typedef typename STLVEC_VTX::iterator			STLVEC_VTX_ITR;
	typedef typename STLVEC_VTX::const_iterator		STLVEC_VTX_CITR;

	typedef std::vector<WORD>						STLVEC_INDEX;
	typedef typename STLVEC_INDEX::iterator			STLVEC_INDEX_ITR;
	typedef typename STLVEC_INDEX::const_iterator	STLVEC_INDEX_CITR;


	STLVEC_VTX		m_VB;
	STLVEC_INDEX	m_IBs;
	STLVEC_INDEX	m_IBw;

	LPDIRECT3DVERTEXBUFFER9	m_d3dVB;
	LPDIRECT3DINDEXBUFFER9	m_d3dIBs;
	LPDIRECT3DINDEXBUFFER9	m_d3dIBw;

	FLOAT			m_fTileSize;
	FLOAT			m_fHeightMin;
	FLOAT			m_fHeightMax;

	D3DXVECTOR2		m_v2dOrigin; // = { minx, minz }

	SIZE			m_sizeMap;
	D3DXVECTOR3		m_v3dCenter;

	RwMatrix		m_matId;

public:
	INT		bInit(LPCSTR szFname_HeightBmp, FLOAT fTileSize=400.f, FLOAT fHeightMin=-1000.f, FLOAT fHeightMax=2000.f);

	INT		bRenderS(LPDIRECT3DDEVICE9 lpDev, const RwMatrix* pMat=NULL);
	INT		bRenderW(LPDIRECT3DDEVICE9 lpDev, const RwMatrix* pMat=NULL);

	INT		bRenderD3dS();
	INT		bRenderD3dW();

	VOID	bClear();

	BOOL	bPick( RwV3d* pPicked );

	
	BOOL	bChkBlock(RwV3d* pMinPos, const RwV3d& eye, const RwV3d& lookat);

	//get height
	FLOAT	bGetHeightOfCenter(void);
	FLOAT	bGetHeight(FLOAT x, FLOAT z);

private:

	POINT	vGetTileIndex(FLOAT x, FLOAT z);
	VOID	vGetTileOrgin(LPD3DXVECTOR2 pOrgin, INT nx, INT nz);
	VOID	vGetTri( LPD3DXVECTOR3 p1
					, LPD3DXVECTOR3 p2
					, LPD3DXVECTOR3 p3
					, INT nx
					, INT nz
					, FLOAT x
					, FLOAT z
					, bool bSlash=true
				   );
	FLOAT	vGetHeightFromPlaneEq( const D3DXVECTOR3& v1
								 , const D3DXVECTOR3& v2
								 , const D3DXVECTOR3& v3
								 , FLOAT fx
								 , FLOAT fz
								 );

	INT		vInitSize(INT nx, INT ny);
	INT		vInitXZ();
	INT		vInitY(BMPLOADER& bmpLoader);
	INT		vInitIB();
	INT		vInitIBs();
	INT		vInitIBw();

	INT		vInitTuTv(INT nLoopx, INT nLoopy);

	INT		vInitD3dVB();
	INT		vInitD3dIBs();
	INT		vInitD3dIBw();

public:
	CEffUt_HFMap();
	virtual ~CEffUt_HFMap(){};
};



///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
CEffUt_HFMap<VTX_TYPE>::CEffUt_HFMap()
: m_d3dVB(NULL)
, m_d3dIBs(NULL)
, m_d3dIBw(NULL)
, m_v2dOrigin(0.f,0.f)
, m_v3dCenter(0.f,0.f,0.f)
, m_fTileSize(400.f)
, m_fHeightMin(0.f)
, m_fHeightMax(1000.f)
{
	m_sizeMap.cx	= 
	m_sizeMap.cy	= 0;

	//
	memset( &m_matId, 0, sizeof( m_matId ) );
	m_matId.right.x =
	m_matId.up.y	=
	m_matId.at.z	= 1.f;
	//RwMatrixSetIdentity ( &m_matId );
};


///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
VOID CEffUt_HFMap<VTX_TYPE>::bClear()
{
	if( m_d3dIBs )
	{
		m_d3dIBs->Release();
		m_d3dIBs	= NULL;
	}
	if( m_d3dIBw )
	{
		m_d3dIBw->Release();
		m_d3dIBw	= NULL;
	}

	if( m_d3dVB )
	{
		RwD3D9DestroyVertexBuffer( VTX_TYPE::SIZE
			, VTX_TYPE::SIZE * m_VB.size()
			, (LPVOID)m_d3dVB, 0LU );
		m_d3dVB	= NULL;
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::bInit(LPCSTR szFname_HeightBmp
								  , FLOAT fTileSize
								  , FLOAT fHeightMin
								  , FLOAT fHeightMax)
{
	m_fTileSize		= fTileSize;
	m_fHeightMin	= fHeightMin;
	m_fHeightMax	= fHeightMax;

	BMPLOADER	bmpLoader( szFname_HeightBmp );

	if( !bmpLoader.IsRead() )
	{
		return -1;
	}

	if( bmpLoader.GetBitCount() != 24 )
		return -1;


	vInitSize( bmpLoader.GetBmpWidth(), bmpLoader.GetBmpHeight() );

	vInitXZ();
	vInitY(bmpLoader);

	vInitIB();

	ksutGeo_CalcIntpNormal( &m_VB.front(), m_VB.size(), &m_IBs.front(), m_IBs.size() );

	vInitTuTv( 1, 1 );//bmpLoader.GetBmpWidth()/8, bmpLoader.GetBmpHeight()/8 );


	
	bClear();
	vInitD3dVB();
	vInitD3dIBs();
	vInitD3dIBw();
	
	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitSize(INT nx, INT ny)
{
	if( ny < 0 )
		ny = -ny;

	m_VB.clear();
	m_IBs.clear();
	m_IBw.clear();

	m_VB.resize(nx * ny);
	m_IBs.resize( (nx-1)*(ny-1)*2*3 );
	m_IBw.resize( m_IBs.size()*2 );

	m_sizeMap.cx	= nx-1;
	m_sizeMap.cy	= ny-1;	//tile 단위

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitXZ()
{
	int	i=0,j=0,k=0;

	float	fz	= 0.f;

	D3DXVECTOR3	v3dTempCenter( m_sizeMap.cx*m_fTileSize*0.5f
						  , 0.f
						  , m_sizeMap.cy*m_fTileSize*0.5f );
	D3DXVECTOR3 v3dOffset( m_v3dCenter - v3dTempCenter );

	m_v2dOrigin.x = v3dOffset.x;
	m_v2dOrigin.y = v3dOffset.z;

	for( i=0; i<m_sizeMap.cy+1; ++i )
	{
		fz	= i*m_fTileSize + v3dOffset.z;

		for( j=0; j<m_sizeMap.cx+1; ++j, ++k )
		{
			m_VB[k].pos.x	= j*m_fTileSize + v3dOffset.x;
			m_VB[k].pos.z	= fz;
		}
	}

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitY(BMPLOADER& bmpLoader)
{
	FLOAT	fScale	= m_fHeightMax - m_fHeightMin;

	BYTE*	pBuff	= bmpLoader.GetBuffAddress();
	int i=0, j=0, k=0;
	for( i=0; i<m_sizeMap.cy+1; ++i )
	{
		for( j=0; j<m_sizeMap.cx+1; ++j, ++k )
		{
			m_VB[k].pos.y	
				= m_v3dCenter.y+m_fHeightMin+fScale*((float)(pBuff[k*3]))/255.f;
		}
	}

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitIB()
{
	vInitIBs();
	vInitIBw();
	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitIBs()
{
	INT	i=0, j=0, k=0;
	INT	nPitch	= m_sizeMap.cx+1;

	for( i=0; i<m_sizeMap.cy; ++i )
	{
		INT nLineStrtIndex	= i*nPitch;
		for( j=0; j<m_sizeMap.cx; ++j, k+=6 )
		{
			
			
			//counter clock wise
			m_IBs[k+0]	= nLineStrtIndex + j;
			m_IBs[k+1]	= nLineStrtIndex + j + nPitch;
			m_IBs[k+2]	= nLineStrtIndex + j + 1;
			
			m_IBs[k+3]	= nLineStrtIndex + j + 1;
			m_IBs[k+4]	= nLineStrtIndex + j + nPitch;
			m_IBs[k+5]	= nLineStrtIndex + j + nPitch + 1;
			/**//*
			//clock wise
			m_IBs[k+0]	= nLineStrtIndex + j;
			m_IBs[k+1]	= nLineStrtIndex + j + 1;
			m_IBs[k+2]	= nLineStrtIndex + j + nPitch;
			
			m_IBs[k+3]	= nLineStrtIndex + j + 1;
			m_IBs[k+4]	= nLineStrtIndex + j + nPitch + 1;
			m_IBs[k+5]	= nLineStrtIndex + j + nPitch;
			/**/
		}
	}

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitIBw()
{
	int	i=0,j=-1;
	for( ; i<(int)m_IBs.size(); i+=3 )
	{
		m_IBw[++j] = m_IBs[i+0]; m_IBw[++j] = m_IBs[i+1];
		m_IBw[++j] = m_IBs[i+1]; m_IBw[++j] = m_IBs[i+2];
		m_IBw[++j] = m_IBs[i+2]; m_IBw[++j] = m_IBs[i+0];
	}

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitTuTv(INT nLoopx, INT nLoopy)
{
	int	i=0,j=0,k=0;

	float	fstepx	= (float)nLoopx / (float)m_sizeMap.cx;
	float	fstepy	= (float)nLoopy / (float)m_sizeMap.cy;

	float fy	= 0.f;
	float fx	= 0.f;
	for( i=0; i<m_sizeMap.cy+1; ++i, fy+=fstepy )
	{
		for( j=0,fx=0.f; j<m_sizeMap.cx+1; ++j,fx+=fstepx, ++k )
		{
			m_VB[k].uv.x	= fx;
			m_VB[k].uv.y	= (float)nLoopy - fy;
		}
	}
	return 0;
};


///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitD3dVB()
{
	UINT	ulOffSet=0;
	RwD3D9CreateVertexBuffer( VTX_TYPE::SIZE
		, VTX_TYPE::SIZE*m_VB.size()
		, (void**)&m_d3dVB
		, &ulOffSet );

	if( !m_d3dVB )
		return -1;

	VTX_TYPE*	pvb	= NULL;
	if( SUCCEEDED( m_d3dVB->Lock( 0LU, 0LU, (void**)&pvb, 0 ) ) )
	{
		memcpy( pvb, &m_VB[0], VTX_TYPE::SIZE * m_VB.size() );
		m_d3dVB->Unlock();
	}

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitD3dIBs()
{
	RwD3D9IndexBufferCreate( m_IBs.size(), (void**)&m_d3dIBs );

	if( !m_d3dIBs )
		return -1;

	WORD*	pib	= NULL;
	if( SUCCEEDED( m_d3dIBs->Lock( 0LU, 0LU, (void**)&pib, 0LU ) ) )
	{
		memcpy( pib, &m_IBs[0], sizeof(WORD) * m_IBs.size() );
		m_d3dIBs->Unlock();
	}


	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::vInitD3dIBw()
{
	RwD3D9IndexBufferCreate( m_IBw.size(), (void**)&m_d3dIBw );

	if( !m_d3dIBs )
		return -1;

	WORD*	pib	= NULL;
	if( SUCCEEDED( m_d3dIBw->Lock( 0LU, 0LU, (void**)&pib, 0LU ) ) )
	{
		memcpy( pib, &m_IBw[0], sizeof(WORD) * m_IBw.size() );
		m_d3dIBw->Unlock();
	}

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::bRenderS(LPDIRECT3DDEVICE9 lpDev, const RwMatrix* pMat)
{
	RwD3D9SetFVF (VTX_TYPE::FVF);
	RwD3D9SetTransformWorld ( pMat ? pMat : &m_matId );


	/*
	lpDev->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST
		, 0LU
		, m_VB.size()
		, m_IBs.size()/3
		, &m_IBs[0]
		, D3DFMT_INDEX16
		, &m_VB[0]
		, VTX_TYPE::SIZE
		);
		/**/

	RwD3D9SetRenderState(D3DRS_LIGHTING, TRUE);

	RwD3D9DrawIndexedPrimitiveUP ( 
		D3DPT_TRIANGLELIST
		, 0LU
		, m_VB.size()
		, m_IBs.size()/3
		, &m_IBs[0]
		, &m_VB[0]
		, VTX_TYPE::SIZE
		);/**/

	RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);
	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::bRenderW(LPDIRECT3DDEVICE9 lpDev, const RwMatrix* pMat)
{
	RwD3D9SetTexture ( NULL, 0LU );
	RwD3D9SetFVF (VTX_TYPE::FVF);
	RwD3D9SetTransformWorld ( pMat ? pMat : &m_matId );


	lpDev->DrawIndexedPrimitiveUP(
		D3DPT_LINELIST
		, 0LU
		, m_VB.size()
		, m_IBw.size()/2
		, &m_IBw[0]
		, D3DFMT_INDEX16
		, &m_VB[0]
		, VTX_TYPE::SIZE
		);/**/
	/*
	RwD3D9DrawIndexedPrimitiveUP ( 
		D3DPT_LINELIST
		, 0LU
		, m_VB.size()
		, m_IBw.size()/2
		, &m_IBw[0]
		, &m_VB[0]
		, VTX_TYPE::SIZE
		);/**/
	return 0;
};


///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::bRenderD3dS()
{
	RwD3D9SetFVF( VTX_TYPE::FVF );
	RwD3D9SetStreamSource( 0LU, m_d3dVB, 0LU, VTX_TYPE::SIZE );
	RwD3D9SetIndices( m_d3dIBs );

	RwD3D9SetRenderState(D3DRS_LIGHTING, TRUE);
	RwD3D9SetVertexShader(NULL);
	RwD3D9SetPixelShader(NULL);


	RwD3D9DrawIndexedPrimitive( D3DPT_TRIANGLELIST
		, 0LU
		, 0LU
		, m_VB.size()
		, 0LU
		, m_IBs.size()/3
		);


	RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);
	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
INT CEffUt_HFMap<VTX_TYPE>::bRenderD3dW()
{
	RwD3D9SetFVF( VTX_TYPE::FVF );
	RwD3D9SetStreamSource( 0LU, m_d3dVB, 0LU, VTX_TYPE::SIZE );
	RwD3D9SetIndices( m_d3dIBw );

	RwD3D9DrawIndexedPrimitive( D3DPT_LINELIST
		, 0LU
		, 0LU
		, m_VB.size()
		, 0LU
		, m_IBw.size()/2
		);

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
BOOL CEffUt_HFMap<VTX_TYPE>::bPick( RwV3d* pPicked )
{
	int	nTri	= m_IBs.size()/3;
	int	i=0,j=0;

	float	ft=0.f, fu=0.f, fv=0.f;
	float	fmin=0.f;
	BOOL	bPick	= FALSE;

	RwV3d*	pv1 = NULL;
	RwV3d*	pv2 = NULL;
	RwV3d*	pv3 = NULL;

	for( ; i<nTri; ++i, j+=3 )
	{
		if( bPick )
		{
			if( CPickUser::GetInst().IntersectTriangle( CPickUser::GetInst().bGetPtrRay()
				, CCamera::bGetInst().bGetPtrV3dEyePos()
				, (RwV3d*)&m_VB[m_IBs[j+0]].pos
				, (RwV3d*)&m_VB[m_IBs[j+1]].pos
				, (RwV3d*)&m_VB[m_IBs[j+2]].pos
				, &ft, &fu, &fv ) )
			{
				if( fmin > ft )
				{
					fmin = ft;
					CPickUser::GetInst().bCalcPickedPoint( *pPicked
						, *(RwV3d*)&m_VB[m_IBs[j+0]].pos
						, *(RwV3d*)&m_VB[m_IBs[j+1]].pos
						, *(RwV3d*)&m_VB[m_IBs[j+2]].pos
						, fu, fv );
					pv1 = (RwV3d*)&m_VB[m_IBs[j+0]].pos;
					pv2 = (RwV3d*)&m_VB[m_IBs[j+1]].pos;
					pv3 = (RwV3d*)&m_VB[m_IBs[j+2]].pos;

				}
			}
		}
		else
		{
			bPick = CPickUser::GetInst().IntersectTriangle( CPickUser::GetInst().bGetPtrRay()
				, CCamera::bGetInst().bGetPtrV3dEyePos()
				, (RwV3d*)&m_VB[m_IBs[j+0]].pos
				, (RwV3d*)&m_VB[m_IBs[j+1]].pos
				, (RwV3d*)&m_VB[m_IBs[j+2]].pos
				, &fmin, &fu, &fv );
			if( bPick )
			{
				CPickUser::GetInst().bCalcPickedPoint( *pPicked
					, *(RwV3d*)&m_VB[m_IBs[j+0]].pos
					, *(RwV3d*)&m_VB[m_IBs[j+1]].pos
					, *(RwV3d*)&m_VB[m_IBs[j+2]].pos
					, fu, fv );
				
					pv1 = (RwV3d*)&m_VB[m_IBs[j+0]].pos;
					pv2 = (RwV3d*)&m_VB[m_IBs[j+1]].pos;
					pv3 = (RwV3d*)&m_VB[m_IBs[j+2]].pos;
			}
		}
	}

//	if( bPick )
//	{
//		TOWND( "-------------------------------------------------------");
//		TOWND( Eff2Ut_ShowRwV3d(pPicked, "picked point" ));
//
//		float height = bGetHeight( pPicked->x, pPicked->z);
//		TOWND( Eff2Ut_FmtMsg( "bGetHeight : %.4f, pPicked.y - height : %.4f",height,pPicked->y - height ));
//
//		g_MyEngine.SETPOS_TEST1(*pPicked);
//		RwV3d	vpos = *pPicked;
//		vpos.y = height;
//		g_MyEngine.SETPOS_TEST2(vpos);
//	}

	return bPick;
};


///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
POINT CEffUt_HFMap<VTX_TYPE>::vGetTileIndex(FLOAT x, FLOAT z)
{
	POINT pt =  { static_cast<INT>((x-m_v2dOrigin.x) / m_fTileSize)
				, static_cast<INT>((z-m_v2dOrigin.y) / m_fTileSize) };
	return pt;
};			
///////////////////////////////////////////////////////////////////////////////
// --------------------------------------- +x
// | x,y(left,top) ------
// |               |    |
// |               |    |
// |               ------ z,w(right,bottom)
// |
// +
// z
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
VOID CEffUt_HFMap<VTX_TYPE>::vGetTileOrgin(LPD3DXVECTOR2 pOrgin, INT nx, INT nz)
{
	pOrgin->x = m_v2dOrigin.x + static_cast<FLOAT>(nx) * m_fTileSize;
	pOrgin->y = m_v2dOrigin.y + static_cast<FLOAT>(nz) * m_fTileSize;
};	
///////////////////////////////////////////////////////////////////////////////
// right hand coordinate
// bSlash('/')		!bSlash (== back-slash '\')
// org ********* +x   org ********* +x	
//     * 1   * *		  * *   1 *		
//     *   *   *		  *   *   *		
//     * *   2 *		  * 2   * *		
//     *********		  *********		
//    +z				 +z				
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
VOID CEffUt_HFMap<VTX_TYPE>::vGetTri(  LPD3DXVECTOR3 p1
									 , LPD3DXVECTOR3 p2
									 , LPD3DXVECTOR3 p3
									 , INT nx
									 , INT nz
									 , FLOAT x
									 , FLOAT z
									 , bool	bSlash
									 )
{
	D3DXVECTOR2	v2TileOrigin(0.f,0.f);
	vGetTileOrgin(&v2TileOrigin, nx, nz);

	INT	lt/*left-top*/		= nx+(nz+0)*(m_sizeMap.cx+1);
	INT	rt/*right-top*/		= lt+1;
	INT	lb/*left-bottom*/	= nx+(nz+1)*(m_sizeMap.cx+1);
	INT	rb/*right-bottom*/	= lb+1;


	if( bSlash )
	{
		if( (x-v2TileOrigin.x) < (m_fTileSize - (z-v2TileOrigin.y)) )
		{//1
			*p1 = m_VB[lt].pos;
			*p2 = m_VB[lb].pos;
			*p3 = m_VB[rt].pos;
		}
		else
		{//2
			*p1 = m_VB[rb].pos;
			*p2 = m_VB[rt].pos;
			*p3 = m_VB[lb].pos;
		}
	}
	else//back-slash
	{
		if( (x-v2TileOrigin.x) < (z-v2TileOrigin.y) )
		{//2
			*p1 = m_VB[lb].pos;
			*p2 = m_VB[rb].pos;
			*p3 = m_VB[lt].pos;
		}
		else
		{//1
			*p1 = m_VB[rt].pos;
			*p2 = m_VB[lt].pos;
			*p3 = m_VB[rb].pos;
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
FLOAT CEffUt_HFMap<VTX_TYPE>::vGetHeightFromPlaneEq( const D3DXVECTOR3& v1
							 , const D3DXVECTOR3& v2
							 , const D3DXVECTOR3& v3
							 , FLOAT fx
							 , FLOAT fz
							 )
{
	D3DXVECTOR3 normal;
	D3DXVECTOR3 v1to2(v2-v1);
	D3DXVECTOR3 v1to3(v3-v1);
	D3DXVec3Cross (&normal, &v1to2, &v1to3);

	if( normal.y == 0.f )
	{
		//err
		//y-z 평면..
		//height map에서는 불가.
		return -9999.f;
	}

	return ( v1.y - ( normal.x*(fx-v1.x) + normal.z*(fz-v1.z) )/normal.y );
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
//#define ISODD4(a)			( a & 0x00000001 )
template<class VTX_TYPE>
FLOAT CEffUt_HFMap<VTX_TYPE>::bGetHeightOfCenter(void)
{
	float cx(m_sizeMap.cx * m_fTileSize * 0.5f);
	float cz(m_sizeMap.cy * m_fTileSize * 0.5f);

	POINT ptIndex = vGetTileIndex(cx, cz);

	D3DXVECTOR3	v1, v2, v3;
	vGetTri(&v1, &v2, &v3, ptIndex.x, ptIndex.y, cx, cz, TRUE);

	return vGetHeightFromPlaneEq(v1,v2,v3,cx,cz);
};
	
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
FLOAT CEffUt_HFMap<VTX_TYPE>::bGetHeight(FLOAT x, FLOAT z)
{
	POINT ptIndex = vGetTileIndex(x, z);

	D3DXVECTOR3	v1, v2, v3;
	vGetTri(&v1, &v2, &v3, ptIndex.x, ptIndex.y, x, z, TRUE);

	return vGetHeightFromPlaneEq(v1,v2,v3,x,z);
};
	
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
enum eSlope
{
	//지준축을 x ( 1.f, 0.f, 0.f ) 로하며
	//각은 ccw를 양의 방향 : 회전축 y (0.f, 1.f, 0.f)
	e_point			= 0,
	e_0				,
	e_45			,
	e_90			,
	e_135			,
	e_180			,
	e_225			,
	e_270			,
	e_315			,

	e_general		,
};
// 카메라의 레이와 지형의 충돌 체크로 바라보는 지점과 가장 가까운 위치를 계산한다.
template<class VTX_TYPE>
BOOL CEffUt_HFMap<VTX_TYPE>::bChkBlock(RwV3d* pMinPos, const RwV3d& eye, const RwV3d& lookat)
{
	//projection
	RwV2d	v2Eye	= { eye.x, eye.z };
	RwV2d	v2At	= { lookat.x, lookat.z };

	//해당 인덱스를 먼저 구한다.
	POINT	ptEye = vGetTileIndex( v2Eye.x, v2Eye.y );
	POINT	ptLookat = vGetTileIndex( v2At.x, v2At.y );

	POINT	ptStart	= { Eff2Ut_MINR_T( ptEye.x, ptLookat.x ), Eff2Ut_MINR_T( ptEye.y, ptLookat.y ) };
	POINT	ptEnd	= { Eff2Ut_MAXR_T( ptEye.x, ptLookat.x ), Eff2Ut_MAXR_T( ptEye.y, ptLookat.y ) };

	//
	//const float radius

	return FALSE;
};

EFFUT_END

#endif // !defined(AFX_EFFUT_HFMAP_H__6473D2BD_9498_4002_A9AB_C6D8FFB45E09__INCLUDED_)
