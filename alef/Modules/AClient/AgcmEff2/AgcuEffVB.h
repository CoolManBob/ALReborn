// AgcuEffVB.h: interface for the AgcuEffVB class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCUEFFVB_H__C896AE5C_E340_4C6E_8E16_980ADDF5AF67__INCLUDED_)
#define AFX_AGCUEFFVB_H__C896AE5C_E340_4C6E_8E16_980ADDF5AF67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffUtil.h"
#include "AgcuEffD3dVtx.h"

///////////////////////////////////////////////////////////////////////////////
//
// lock
// updatebuffer
// unlock
// setrenderstate
// settexture
//
// and render
//
// 

//RwD3D9SetTexture
//RwD3D9SetFVF
//RwD3D9SetStreamSource
//RwD3D9SetIndices
//RwD3D9DrawIndexedPrimitive
//RwD3D9SetTransform
//RwD3D9SetTransformWorld
//RwD3D9IndexBufferCreate
//RwD3D9CreateVertexBuffer
//RwD3D9DestroyVertexBuffer
//
//RwD3D9SetMaterial
//RwD3D9SetSurfaceProperties

///////////////////////////////////////////////////////////////////////////////
// class
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
class AgcuEffVB
{
public:
	AgcuEffVB();
	~AgcuEffVB();

	LPDIRECT3DVERTEXBUFFER9	bGetPtrVB		( VOID ) { return m_lpVB; };
	RwUInt32				bGetOffset		( VOID ) const { return m_ulOffsetFromLock*VTX_TYPE::SIZE; }

	VOID					bClear			( VOID )	{ vClear(); };
	
	HRESULT					bLockVB			( VOID** ppVtx  , DWORD dwNumVtx=0LU );
	HRESULT					bLockIB			( VOID** ppIndex, DWORD dwNumindices=0LU );
	HRESULT					bLockIBW		( VOID** ppIndex, DWORD dwNumindices=0LU );

	HRESULT					bUnlockVB		( VOID );
	HRESULT					bUnlockIB		( VOID );	
	HRESULT					bUnlockIBW		(VOID);

	RwBool					bInit			( DWORD numVtx, DWORD numIndex );
	RwBool					bInitWire		( VOID );

	VOID					bRender			( DWORD	dwNumVertices, DWORD dwPrimitiveCnt, const RwMatrix* pmat);
	VOID					bRenderW		( DWORD	dwNumVertices, DWORD dwPrimitiveCnt, const RwMatrix* pmat);	
	VOID					bRenderVS		( DWORD	dwNumVertices, DWORD dwPrimitiveCnt );
	VOID					bRenderWVS		( DWORD	dwNumVertices, DWORD dwPrimitiveCnt );

private:
	VOID					vClear			( VOID );

private:
	//do not use
	AgcuEffVB(const AgcuEffVB& cpy) { cpy; };
	AgcuEffVB& operator = (const AgcuEffVB& cpy) { cpy; return NULL; };

private:
	LPDIRECT3DVERTEXBUFFER9		m_lpVB;
	LPDIRECT3DINDEXBUFFER9		m_lpIB;
	LPDIRECT3DINDEXBUFFER9		m_lpIBW;

	typedef std::vector<VTX_TYPE>							STLVEC_VTX;
	typedef typename std::vector<VTX_TYPE>::iterator		STLVEC_VTX_ITR;
	typedef typename std::vector<VTX_TYPE>::const_iterator	STLVEC_VTX_CITR;

	typedef std::vector<WORD>								STLVEC_WORD;
	typedef typename std::vector<WORD>::iterator			STLVEC_WORD_ITR;
	typedef typename std::vector<WORD>::const_iterator		STLVEC_WORD_CITR;

	STLVEC_VTX		m_VB;
	STLVEC_WORD		m_IB;
	STLVEC_WORD		m_IBW;

	DWORD			m_dwNumOfVertex;
	DWORD			m_dwNumOfIndex;
	RwUInt32		m_ulOffsetFromLock;
};

template<class VTX_TYPE>
AgcuEffVB<VTX_TYPE>::AgcuEffVB() : 
 m_lpVB(NULL), 
 m_lpIB(NULL), 
 m_lpIBW(NULL), 
 m_dwNumOfVertex(0LU), 
 m_dwNumOfIndex(0LU), 
 m_ulOffsetFromLock(0LU)
{
};

template<class VTX_TYPE>
AgcuEffVB<VTX_TYPE>::~AgcuEffVB()
{
	vClear();
};

template<class VTX_TYPE>
VOID AgcuEffVB<VTX_TYPE>::vClear()
{
	DEF_SAFERELEASE( m_lpIB );
	DEF_SAFERELEASE( m_lpIBW );
	m_dwNumOfVertex	= 0;
	m_dwNumOfIndex	= 0;
};

template<class VTX_TYPE>
HRESULT AgcuEffVB<VTX_TYPE>::bLockVB(VOID** ppVtx, DWORD dwNumVtx)
{
	ASSERT( "kday" && (dwNumVtx<=m_dwNumOfVertex) );
	(*ppVtx)	= &m_VB[0];
	return D3D_OK;
};

template<class VTX_TYPE>
HRESULT AgcuEffVB<VTX_TYPE>::bUnlockVB(VOID)
{
	return D3D_OK;
};

template<class VTX_TYPE>
HRESULT AgcuEffVB<VTX_TYPE>::bLockIB(VOID** ppIndex, DWORD dwNumindices)
{
	(*ppIndex)	= &m_IB[0];
	return D3D_OK;
};

template<class VTX_TYPE>
HRESULT AgcuEffVB<VTX_TYPE>::bUnlockIB(VOID)
{
	return D3D_OK;
};

template<class VTX_TYPE>
HRESULT AgcuEffVB<VTX_TYPE>::bLockIBW(VOID** ppIndex, DWORD dwNumindices)
{
	(*ppIndex)	= &m_IBW[0];
	return D3D_OK;
};

template<class VTX_TYPE>
HRESULT AgcuEffVB<VTX_TYPE>::bUnlockIBW(VOID)
{
	return D3D_OK;
};

template<class VTX_TYPE>
RwBool AgcuEffVB<VTX_TYPE>::bInit( DWORD numVtx, DWORD numIndex )
{
	vClear();

	m_VB.resize(numVtx);
	VTX_TYPE* pVtx	= NULL;
	if( SUCCEEDED( bLockVB( (void**)&pVtx ) ) )
	{
		for( DWORD i=0; i<numVtx; ++i, ++pVtx )
			new(pVtx)	VTX_TYPE;

		bUnlockVB();
	}
	m_dwNumOfVertex	= numVtx;

	m_IB.resize(numIndex);
	m_dwNumOfIndex	= numIndex;

	m_IBW.resize(numIndex*2);

	return TRUE;
};

template<class VTX_TYPE>
RwBool AgcuEffVB<VTX_TYPE>::bInitWire()
{
	HRESULT	hr		= D3D_OK;
	WORD*	pIB		= NULL;
	WORD*	pIBW	= NULL;

	if( SUCCEEDED( hr = bLockIB( (VOID**)&pIB ) ) )
	{
		if( SUCCEEDED( hr = bLockIBW( (VOID**)&pIBW ) ) )
		{		
			size_t	i=0,j=-1;
			for( ; i<m_dwNumOfIndex/*m_IB.size()*/; i+=3 )
			{
				pIBW[++j] = pIB[i+0]; pIBW[++j] = pIB[i+1];
				pIBW[++j] = pIB[i+1]; pIBW[++j] = pIB[i+2];
				pIBW[++j] = pIB[i+2]; pIBW[++j] = pIB[i+0];
			}
			bUnlockIBW();
		}

		bUnlockIB();
	}



	return TRUE;
};

template<class VTX_TYPE>
VOID AgcuEffVB<VTX_TYPE>::bRender( DWORD dwNumVertices, DWORD dwPrimitiveCnt, const RwMatrix* pmat)
{
	RwD3D9SetFVF( VTX_TYPE::FVF );
	RwD3D9SetTransformWorld( pmat );
	RwD3D9DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, dwNumVertices, dwPrimitiveCnt, &m_IB[0], &m_VB[0], VTX_TYPE::SIZE );
};

template<class VTX_TYPE>
VOID AgcuEffVB<VTX_TYPE>::bRenderW(DWORD dwNumVertices, DWORD dwPrimitiveCnt, const RwMatrix* pmat )
{
	RwD3D9SetTexture( NULL, 0 );
	RwD3D9SetFVF( VTX_TYPE::FVF );
	RwD3D9SetTransformWorld( pmat );
	RwD3D9DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, dwNumVertices, dwPrimitiveCnt*3, &m_IBW[0], &m_VB[0], VTX_TYPE::SIZE );
};

template<class VTX_TYPE>
VOID AgcuEffVB<VTX_TYPE>::bRenderVS	( DWORD dwNumVertices, DWORD dwPrimitiveCnt )
{
	RwD3D9SetStreamSource( 0, m_lpVB, m_ulOffsetFromLock*VTX_TYPE::SIZE, VTX_TYPE::SIZE );
	RwD3D9SetIndices( m_lpIB );
	RwD3D9DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwNumVertices, 0, dwPrimitiveCnt	);
};

template<class VTX_TYPE>
VOID AgcuEffVB<VTX_TYPE>::bRenderWVS( DWORD dwNumVertices, DWORD dwPrimitiveCnt )
{
	RwD3D9SetRenderState (D3DRS_COLORVERTEX, FALSE);

	RwD3D9SetTexture( NULL, 0 );
	RwD3D9SetStreamSource( 0, m_lpVB, m_ulOffsetFromLock*VTX_TYPE::SIZE, VTX_TYPE::SIZE );
	RwD3D9SetIndices( m_lpIBW );
	RwD3D9DrawIndexedPrimitive( D3DPT_LINELIST, 0, 0, dwNumVertices, 0, dwPrimitiveCnt*3 );

	RwD3D9SetRenderState (D3DRS_COLORVERTEX, TRUE);
};

template<class VTX_TYPE>
VOID AgcuEffVtx_CalcBBox(RwBBox* pbox, const VTX_TYPE* pIn, INT nNum, const RwMatrix* pMat)
{
	D3DXVECTOR3*	inf = (D3DXVECTOR3*)&pbox->inf;
	D3DXVECTOR3*	sup = (D3DXVECTOR3*)&pbox->sup;
	if( pMat )
	{
		D3DXMATRIX	mat(pMat->right.x, pMat->right.y, pMat->right.z, 0.f,
						pMat->up.x, pMat->up.y, pMat->up.z, 0.f,
						pMat->at.x, pMat->at.y, pMat->at.z, 0.f,
						pMat->pos.x, pMat->pos.y, pMat->pos.z, 1.f);

		D3DXVECTOR3	vTemp;
		for( int i=0; i<nNum; ++i, ++pIn )
		{
			D3DXVec3TransformCoord( &vTemp, &pIn->m_v3Pos, &mat );
			D3DXVec3Minimize ( inf, inf, &vTemp );
			D3DXVec3Maximize ( sup, sup, &vTemp );
		}
	}
	else
	{
		for( int i=0; i<nNum; ++i, ++pIn )
		{
			D3DXVec3Minimize ( inf, inf, &pIn->m_v3Pos );
			D3DXVec3Maximize ( sup, sup, &pIn->m_v3Pos );
		}
	}
};

template<class VTX_TYPE>
VOID AgcuEffVtx_SetPos(VTX_TYPE* pOut, const RwV3d& pos)
{
	ASSERT( pOut );
	pOut->m_v3Pos.x	= pos.x;
	pOut->m_v3Pos.y	= pos.y;
	pOut->m_v3Pos.z	= pos.z;
};

template<class VTX_TYPE>
VOID AgcuEffVtx_SetColr(VTX_TYPE* pOut, RwRGBA rgba, INT nNum)
{
	ASSERT( pOut );
	ASSERT( nNum > 0 );

	for( int i=0; i<nNum; ++i, ++pOut )
		pOut->m_dwColr	= ARGB32(rgba.alpha, rgba.red, rgba.green, rgba.blue);
	
};

template<class VTX_TYPE>
VOID AgcuEffVtx_SetUV(VTX_TYPE* pOut, const RwV2d& uv)
{
	ASSERT( pOut );
	pOut->m_v2Tutv.x	= uv.x;
	pOut->m_v2Tutv.y	= uv.y;
};

template<class VTX_TYPE>
VOID AgcuEffVtx_SetUV(VTX_TYPE* pOut, float u, float v)
{
	ASSERT( pOut );
	pOut->m_v2Tutv.x	= u;
	pOut->m_v2Tutv.y	= v;
};

template<class VTX_TYPE>
VOID AgcuEffVtx_DrawLine( VTX_TYPE arrVtx[], INT nNum )
{	
	RwMatrix mat;
	RwMatrixSetIdentity(&mat);

	RwD3D9SetFVF( VTX_TYPE::FVF );
	RwD3D9SetTransformWorld( &mat );
	RwD3D9SetTexture(NULL, 0);

	RwD3D9DrawPrimitiveUP( D3DPT_LINESTRIP, nNum, &arrVtx[0], VTX_TYPE::SIZE );
};


#endif