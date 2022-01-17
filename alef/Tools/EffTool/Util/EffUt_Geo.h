// EffUt_Geo.h: interface for the EffUt_Geo class.
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFUT_GEO_H__E8AA32E2_D52D_4EC4_AD16_0CD7AF9ECBFA__INCLUDED_)
#define AFX_EFFUT_GEO_H__E8AA32E2_D52D_4EC4_AD16_0CD7AF9ECBFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef EFFUT_BEGIN
#define EFFUT_BEGIN	namespace __EffUtil__ {
#define EFFUT_END	};
#define USING_EFFUT	using namespace __EffUtil__;
#define EFFUT		__EffUtil__
#endif


#include "rwcore.h"
#include "EffUt_Vtx.h"
#include <d3dx9math.h>
#include <vector>

#include "PickUser.h"
#include "Camera.h"

#pragma warning(disable: 4786)

EFFUT_BEGIN
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
class CEffUt_GeoBase
{
protected:
	//embeded type
	enum	E_TYPE
	{
		E_BOX		= 0,
		E_SPHERE	,
		E_TRUMPET	,

		E_TYPE_NUM	,//추가시마다.. TYPENAME 초기화도 잊지말고.
	};

	enum	{ E_NAME_LEN	= 30, };


	typedef std::vector<VTX_TYPE>			STLVEC_VTX;
	typedef typename STLVEC_VTX::iterator			STLVEC_VTX_ITR;
	typedef typename STLVEC_VTX::const_iterator		STLVEC_VTX_CITR;

	typedef std::vector<WORD>				STLVEC_INDEX;
	typedef typename STLVEC_INDEX::iterator			STLVEC_INDEX_ITR;
	typedef typename STLVEC_INDEX::const_iterator	STLVEC_INDEX_CITR;

	//variable
protected:
	static const char*		TYPENAME[E_TYPE_NUM];
	static const RwMatrix	IMAT;
	const E_TYPE	m_eType;
	char			m_szName[E_NAME_LEN];

	STLVEC_VTX		m_VB;
	STLVEC_INDEX	m_IBs;	//solid mode
	STLVEC_INDEX	m_IBw;	//wire mode

	RwBBox			m_bbox;
	RwSphere		m_bsphere;

protected:
	VOID	tCalcBoundInfo()
	{
		if( m_VB.empty() )
			return;

		D3DXVECTOR3	inf,sup;

		inf	= m_VB[0].pos;
		sup	= m_VB[0].pos;

		for( size_t i=1; i<m_VB.size(); ++i )
		{
			if( m_VB[i].pos.x < inf.x )
				inf.x = m_VB[i].pos.x;
			else if( m_VB[i].pos.x > sup.x )
				sup.x = m_VB[i].pos.x;
			
			if( m_VB[i].pos.y < inf.y )
				inf.y = m_VB[i].pos.y;
			else if( m_VB[i].pos.y > sup.y )
				sup.y = m_VB[i].pos.y;
			
			if( m_VB[i].pos.z < inf.z )
				inf.z = m_VB[i].pos.z;
			else if( m_VB[i].pos.z > sup.z )
				sup.z = m_VB[i].pos.z;
		}

		m_bbox.inf.x = inf.x, m_bbox.inf.y = inf.y, m_bbox.inf.z = inf.z;
		m_bbox.sup.x = sup.x, m_bbox.sup.y = sup.y, m_bbox.sup.z = sup.z;

		m_bsphere.center.x	= (inf.x + sup.x) * 0.5f;
		m_bsphere.center.y	= (inf.y + sup.y) * 0.5f;
		m_bsphere.center.z	= (inf.z + sup.z) * 0.5f;
		m_bsphere.radius	= sqrtf( 
			(m_bsphere.center.x - inf.x)*(m_bsphere.center.x - inf.x) +
			(m_bsphere.center.y - inf.y)*(m_bsphere.center.y - inf.y) +
			(m_bsphere.center.z - inf.z)*(m_bsphere.center.z - inf.z) );
	};

	//init
protected:
	virtual VOID		tInitVB(){};//	= 0;
	virtual VOID		tInitIBs(){};//	= 0;
	virtual VOID		tInitIBw()
	{
		if( m_IBs.empty() ){//err
			return;
		}
		for( int i=0,j=-1; i<(int)m_IBs.size(); i+=3 )
		{
			m_IBw[++j] = m_IBs[i+0], m_IBw[++j] = m_IBs[i+1];
			m_IBw[++j] = m_IBs[i+1], m_IBw[++j] = m_IBs[i+2];
			m_IBw[++j] = m_IBs[i+2], m_IBw[++j] = m_IBs[i+0];
		}
	};

	VOID	tResize(INT nVtx, INT nTri)
	{
		m_VB.resize(nVtx);
		m_IBs.resize(nTri*3);
		m_IBw.resize(nTri*3*2);
		
		tInitVB();
		tInitIBs();
		tInitIBw();
	};

private:
	BOOL vPrePick(CPickUser& cpick, RwFrame* pFrm, RwReal* pLen)
	{
		BOOL br	= cpick.bRwSphereCheck( cpick.bGetPtrRay()
			, CCamera::bGetInst().bGetPtrV3dEyePos()
			, m_bsphere
			, pFrm ? RwFrameGetLTM( pFrm ) : NULL );

		if( br ){
			br = cpick.bRwBBoxCheck( cpick.bGetPtrRay()
				, CCamera::bGetInst().bGetPtrV3dEyePos()
				, m_bbox
				, pFrm ? RwFrameGetLTM( pFrm ) : NULL );
		}

		return br;
	};
public:
	CEffUt_GeoBase(E_TYPE eType, INT nVtx, INT nTri, LPCSTR szName="unknown")
		: m_eType(eType), m_VB(nVtx), m_IBs(nTri*3), m_IBw(nTri*3*2)
	{
	};
	virtual	~CEffUt_GeoBase(){};

	virtual INT bRender_s(const RwMatrix* pMat=NULL)
	{
		RwD3D9SetFVF (VTX_TYPE::FVF);
		RwD3D9SetTransformWorld ( pMat ? pMat : &IMAT );

		RwD3D9DrawIndexedPrimitiveUP ( 
			D3DPT_TRIANGLELIST
			, 0LU
			, m_VB.size()
			, m_IBs.size()/3
			, &m_IBs[0]
			, &m_VB[0]
			, VTX_TYPE::SIZE
			);
		return 0;
	};
	virtual	INT bRender_w(const RwMatrix* pMat=NULL)
	{
		RwD3D9SetTexture ( NULL, 0LU );
		RwD3D9SetFVF (VTX_TYPE::FVF);
		RwD3D9SetTransformWorld ( pMat ? pMat : &IMAT );


		RwD3D9DrawIndexedPrimitiveUP ( 
			D3DPT_LINELIST
			, 0LU
			, m_VB.size()
			, m_IBw.size()/2
			, &m_IBw[0]
			, &m_VB[0]
			, VTX_TYPE::SIZE
			);
		return 0;
	};

	BOOL bPick(CPickUser& cpick, RwFrame* pFrm, RwReal* pLen)
	{
		if( m_IBs.empty() )
			return FALSE;

		if( vPrePick( cpick, pFrm, pLen ) )
		{
			RwV3d	uvRay	= *cpick.bGetPtrRay();
			RwV3d	vEyePos = *CCamera::bGetInst().bGetPtrV3dEyePos();
			if( pFrm )
			{
				cpick.bGetRayCamPosWithMat( &uvRay, &vEyePos, RwFrameGetLTM( pFrm ) );
			}


			int nTri	= static_cast<int>(m_IBs.size() / 3);
			RwV3d*	pv0 = NULL;
			RwV3d*	pv1 = NULL;
			RwV3d*	pv2 = NULL;

			RwReal	ft	= 0.f,
					fu	= 0.f,
					fv	= 0.f;
			RwReal	ftmin	= 0.f;

			BOOL	br	= FALSE;

			for( int i=0; i<nTri; ++i )
			{
				pv0 = (RwV3d*)(&m_VB[m_IBs[i*3 + 0]].pos);
				pv1 = (RwV3d*)(&m_VB[m_IBs[i*3 + 1]].pos);
				pv2 = (RwV3d*)(&m_VB[m_IBs[i*3 + 2]].pos);
				
				if( br )
				{
					if( cpick.IntersectTriangle( &uvRay
						, &vEyePos
						, pv0, pv1, pv2
						, &ft, &fu, &fv ) )
					{
						if( ftmin > ft )
							ftmin = ft;
					}
				}
				else
				{
					br = cpick.IntersectTriangle( &uvRay
							, &vEyePos
							, pv0, pv1, pv2
							, &ftmin, &fu, &fv );

					if( br && !pLen )
						return TRUE;
				}
			}

			if( br )
			{
				if( pLen )
					*pLen = ftmin;
				return TRUE;
			}
		}

		return FALSE;
	};

	const char*	bGetName() { return m_szName; };
	const char* bGetTypeName() { return TYPENAME[m_eType]; };
	int bGetVtxNum() const { return static_cast<int>(m_VB.size()); };
	int bGetTriNum() const { return (static_cast<int>(m_IBs.size()) / 3); };
	VTX_TYPE*	bGetVB() { return m_VB.empty() ? NULL : &m_VB[0]; };
	WORD*		bGetIB() { return m_IB.empty() ? NULL : &m_IB[0]; };
};

template<class VTX_TYPE>
const char* CEffUt_GeoBase<VTX_TYPE>::TYPENAME[CEffUt_GeoBase<VTX_TYPE>::E_TYPE_NUM]
= {
	"E_BOX",
	"E_SPHERE",
	"E_TRUMPET",
};
template<class VTX_TYPE>
const RwMatrix CEffUt_GeoBase<VTX_TYPE>::IMAT
= { 
	{1.f, 0.f, 0.f}, 0 ,
	{0.f, 1.f, 0.f}, 0 ,
	{0.f, 0.f, 1.f}, 0 ,
	{0.f, 0.f, 0.f}, 0 ,
};



///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
class CEffUt_GeoBox : public CEffUt_GeoBase<VTX_TYPE>
{
	float	m_width,
			m_height,
			m_depth;

public:
	CEffUt_GeoBox( float width, float height, float depth, LPCSTR szName="unknown" )
		: CEffUt_GeoBase<VTX_TYPE>( E_BOX, 8LU, 12LU, szName )
		, m_width( width )
		, m_height( height )
		, m_depth( depth )
	{
		tInitVB();
		tInitIBs();
		tInitIBw();
	};
	~CEffUt_GeoBox(){};
protected:
	virtual VOID tInitVB()
	{
		int	i=-1;
		//top
		++i; m_VB[i].pos.x = -0.5f * m_width, m_VB[i].pos.y =  0.5f * m_height, m_VB[i].pos.z =  0.5f * m_depth * -1.f;
		++i; m_VB[i].pos.x =  0.5f * m_width, m_VB[i].pos.y =  0.5f * m_height, m_VB[i].pos.z =  0.5f * m_depth * -1.f;
		++i; m_VB[i].pos.x =  0.5f * m_width, m_VB[i].pos.y =  0.5f * m_height, m_VB[i].pos.z = -0.5f * m_depth * -1.f;
		++i; m_VB[i].pos.x = -0.5f * m_width, m_VB[i].pos.y =  0.5f * m_height, m_VB[i].pos.z = -0.5f * m_depth * -1.f;
		//bottom
		++i; m_VB[i].pos.x = -0.5f * m_width, m_VB[i].pos.y = -0.5f * m_height, m_VB[i].pos.z =  0.5f * m_depth * -1.f;
		++i; m_VB[i].pos.x =  0.5f * m_width, m_VB[i].pos.y = -0.5f * m_height, m_VB[i].pos.z =  0.5f * m_depth * -1.f;
		++i; m_VB[i].pos.x =  0.5f * m_width, m_VB[i].pos.y = -0.5f * m_height, m_VB[i].pos.z = -0.5f * m_depth * -1.f;
		++i; m_VB[i].pos.x = -0.5f * m_width, m_VB[i].pos.y = -0.5f * m_height, m_VB[i].pos.z = -0.5f * m_depth * -1.f;

		tCalcBoundInfo();

	};
	virtual VOID tInitIBs()
	{	//right hand's law
		int	i=-1;
		//top
		m_IBs[++i] = 0, m_IBs[++i] = 3, m_IBs[++i] = 1;
		m_IBs[++i] = 1, m_IBs[++i] = 3, m_IBs[++i] = 2;
		//bottom
		m_IBs[++i] = 7, m_IBs[++i] = 4, m_IBs[++i] = 6;
		m_IBs[++i] = 6, m_IBs[++i] = 4, m_IBs[++i] = 5;
		//front
		m_IBs[++i] = 3, m_IBs[++i] = 7, m_IBs[++i] = 2;
		m_IBs[++i] = 2, m_IBs[++i] = 7, m_IBs[++i] = 6;
		//back
		m_IBs[++i] = 1, m_IBs[++i] = 5, m_IBs[++i] = 0;
		m_IBs[++i] = 0, m_IBs[++i] = 5, m_IBs[++i] = 4;
		//left
		m_IBs[++i] = 0, m_IBs[++i] = 4, m_IBs[++i] = 3;
		m_IBs[++i] = 3, m_IBs[++i] = 4, m_IBs[++i] = 7;
		//right
		m_IBs[++i] = 2, m_IBs[++i] = 6, m_IBs[++i] = 1;
		m_IBs[++i] = 1, m_IBs[++i] = 6, m_IBs[++i] = 5;

	};

};


///////////////////////////////////////////////////////////////////////////////
// CEffUt_GeoSphere
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
class CEffUt_GeoSphere : public CEffUt_GeoBase<VTX_TYPE>
{
	float	m_radius;
	int		m_nAngle;

	BOOL	vCheckAvailable()
	{
		BOOL	br	= TRUE;
		if( m_nAngle & 0x80000000 )//음수
		{
			m_nAngle = ::abs(m_nAngle);
			vCheckAvailable();
			br	= FALSE;
		}
		if( m_nAngle & 0x00000001 )//홀수
		{
			++m_nAngle;
			vCheckAvailable();
			br	= FALSE;
		}
		if( m_nAngle<4 )//최소각갯수
		{
			m_nAngle=4;
			br	= FALSE;
		}

		return br;
	};
public:
	explicit CEffUt_GeoSphere( float radius, int nAngle=13, LPCSTR szName="unknown" )
		: CEffUt_GeoBase<VTX_TYPE>( E_SPHERE
						, (nAngle/2-1)*nAngle + 2
						, ((nAngle/2-1)-1)*nAngle*2 + nAngle*2
						, szName )
		, m_radius( radius )
		, m_nAngle( nAngle )
	{	
		if( !vCheckAvailable() )
		{
			tResize( 
				(m_nAngle/2-1)*m_nAngle + 2
				, ((m_nAngle/2-1)-1)*m_nAngle*2 + m_nAngle*2 );
		}
		else
		{
			tInitVB();
			tInitIBs();
			tInitIBw();	
		}
	};
	~CEffUt_GeoSphere(){};
protected:
	virtual VOID tInitVB()
	{
		if( !vCheckAvailable() )
			return;

		int i=0;
		//+x pole
		m_VB[i].pos.x	= m_radius;
		m_VB[i].pos.y	= 0.f;
		m_VB[i].pos.z	= 0.f;

		++i;
		//midle
		int j = 0;
		int k = 0;
		float radius = 0.f;
		float x		 = 0.f;
		const float f2pi	= 6.283185307179587f;
		for( j=0; j<(m_nAngle/2-1); ++j )
		{
			x		= m_radius * cosf( (j+1)*f2pi/m_nAngle );
			radius	= m_radius * sinf( (j+1)*f2pi/m_nAngle );
			for( k=0; k<m_nAngle; ++k,++i )
			{
				m_VB[i].pos.x	= x;
				m_VB[i].pos.y	= radius * sinf(k*f2pi/m_nAngle);
				m_VB[i].pos.z	= radius * cosf(k*f2pi/m_nAngle) * -1.f;
			}
		}

		//-x pole
		m_VB[i].pos.x	= -m_radius;
		m_VB[i].pos.y	= 0.f;
		m_VB[i].pos.z	= 0.f;

		tCalcBoundInfo();
	};
	virtual VOID tInitIBs()
	{
		if( !vCheckAvailable() )
			return;
		
		//+x pole
		int i=-1,
			j=0;
		for( j=0; j<m_nAngle-1; ++j )
		{
			m_IBs[++i]	= 0;//+x pole
			m_IBs[++i]	= j+1;
			m_IBs[++i]	= j+2;
		}
		m_IBs[++i]	= 0;//+x pole
		m_IBs[++i]	= j+1;
		m_IBs[++i]	= 1;

		//middle
		int k=0;
		int nStartIndex	= 1-m_nAngle;

		int	nSize	= m_IBs.size();
		for( j=0; j<((m_nAngle/2-1)-1); ++j )
		{
			nStartIndex += m_nAngle;
			if( 1 )
			for( k=0; k<m_nAngle-1; ++k )
			{
				m_IBs[++i] = nStartIndex+k;
				m_IBs[++i] = nStartIndex+k+m_nAngle;
				m_IBs[++i] = nStartIndex+k+m_nAngle+1;
				
				m_IBs[++i] = nStartIndex+k;
				m_IBs[++i] = nStartIndex+k+m_nAngle+1;
				m_IBs[++i] = nStartIndex+k+1;
			}
			else
			for( k=0; k<m_nAngle-1; ++k )
			{
				m_IBs[++i] = 
				m_IBs[++i] = 
				m_IBs[++i] = 
				
				m_IBs[++i] = 
				m_IBs[++i] = 
				m_IBs[++i] = 0;
			}
			
			m_IBs[++i] = nStartIndex+k;
			m_IBs[++i] = nStartIndex+k+m_nAngle;
			m_IBs[++i] = nStartIndex+m_nAngle;
			
			m_IBs[++i] = nStartIndex+k;
			m_IBs[++i] = nStartIndex+m_nAngle;
			m_IBs[++i] = nStartIndex;
		}

		//-x pole
		nStartIndex += m_nAngle;
		for( j=0; j<m_nAngle-1; ++j )
		{
			m_IBs[++i]	= m_VB.size()-1;//-x pole
			m_IBs[++i]	= nStartIndex+j+1;
			m_IBs[++i]	= nStartIndex+j;
		}
		m_IBs[++i]	= m_VB.size()-1;//-x pole
		m_IBs[++i]	= nStartIndex;
		m_IBs[++i]	= nStartIndex+j;

	};
};


///////////////////////////////////////////////////////////////////////////////
// CEffUt_GeoTrumpet
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
class CEffUt_GeoTrumpet : public CEffUt_GeoBase<VTX_TYPE>
{
	float	m_length,
			m_conAngle,
			m_radius;
	int		m_nAngle;

	BOOL	vCheckAvailable()
	{
		BOOL	br = TRUE;
		if( m_conAngle < 0.0001f )
		{
			m_conAngle = fabs(m_conAngle);
			br = FALSE;
		}
		if( m_conAngle > 3.1415f )
		{
			m_conAngle = 3.1415f;
			br = FALSE;
		}

		if( br == FALSE )
		{
			m_radius	= m_length * atanf(m_conAngle/2.f);
		}

		return br;
	}

public:
	explicit CEffUt_GeoTrumpet( float length, float conAngle/*radian*/, int nAngle=13, LPCSTR szName="unknown" )
		: CEffUt_GeoBase<VTX_TYPE>( E_TRUMPET
						, nAngle + 1 + 1
						, nAngle + nAngle
						, szName )
		, m_length( length )
		, m_conAngle( conAngle )
		, m_radius( length * atanf(conAngle/2.f) )
		, m_nAngle( nAngle )
	{
		if( !vCheckAvailable() )
		{
			tResize( nAngle + 1 + 1, nAngle + nAngle );
		}
		else
		{
			tInitVB();
			tInitIBs();
			tInitIBw();	
		}
	};
	~CEffUt_GeoTrumpet(){};
protected:
	virtual VOID tInitVB()
	{
		if( !vCheckAvailable() )
			return;

		//org
		int i=0;
		m_VB[i].pos.x	= 
		m_VB[i].pos.y	= 
		m_VB[i].pos.z	= 0.f;
		//circle
		int j=0;
		const float f2pi	= 6.283185307179587f;
		for( j=0,i=1; j<m_nAngle; ++j, ++i )
		{
			m_VB[i].pos.x	= m_radius * cosf( j*f2pi/m_nAngle );
			m_VB[i].pos.y	= m_radius * sinf( j*f2pi/m_nAngle );
			m_VB[i].pos.z	= m_length;

			//m_VB[i].pos.x	= m_length;
			//m_VB[i].pos.y	= m_radius * sinf( j*f2pi/m_nAngle );
			//m_VB[i].pos.z	= m_radius * cosf( j*f2pi/m_nAngle ) * -1.f;
		}
		//circle center
		m_VB[i].pos.x	= 
		m_VB[i].pos.y	= 0.f;
		m_VB[i].pos.z	= m_length;
		//m_VB[i].pos.x	= m_length;
		//m_VB[i].pos.y	= 
		//m_VB[i].pos.z	= 0.f;
		
		tCalcBoundInfo();
	};
	virtual VOID tInitIBs()
	{
		if( !vCheckAvailable() )
			return;

		//peaked tri(side)
		int i=0;
		int	j=-1;
		for( i=0; i<m_nAngle-1; ++i )
		{
			m_IBs[++j]	= 0;
			m_IBs[++j]	= i+2;
			m_IBs[++j]	= i+1;
		};
		m_IBs[++j]	= 0;
		m_IBs[++j]	= 1;
		m_IBs[++j]	= i+1;

		//plane tri(circle)
		for( i=0; i<m_nAngle-1; ++i )
		{
			m_IBs[++j]	= m_VB.size()-1;
			m_IBs[++j]	= i+1;
			m_IBs[++j]	= i+2;
		};
		m_IBs[++j]	= m_VB.size()-1;
		m_IBs[++j]	= i+1;
		m_IBs[++j]	= 1;
	};
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
void ksutGeo_CalcTriNormal(  const VTX_TYPE& vtx0
							, const VTX_TYPE& vtx1
							, const VTX_TYPE& vtx2
							, D3DXVECTOR3* pvtnrm)
{
	D3DXVECTOR3	vt1(vtx1.pos - vtx0.pos);
	D3DXVECTOR3	vt2(vtx2.pos - vtx0.pos);

	D3DXVec3Cross( pvtnrm, &vt1, &vt2 );
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
void ksutGeo_CalcIntpNormal(VTX_TYPE vtx[], INT nVtx, WORD ib[], INT nIndex)
{
	int	i,j;

	D3DXVECTOR3	vtnrm(0.f, 0.f, 0.f);
	D3DXVECTOR3	vtnrmAccumulated(0.f, 0.f, 0.f);

	for( i=0; i<nVtx; ++i )
	{
		vtnrmAccumulated.x =
		vtnrmAccumulated.y =
		vtnrmAccumulated.z = 0.f;

		for( j=0; j<nIndex; j+=3 )
		{
			if( i == static_cast<INT>(ib[j+0]) ||
				i == static_cast<INT>(ib[j+1]) ||
				i == static_cast<INT>(ib[j+2]) )
			{
				ksutGeo_CalcTriNormal( vtx[(ib[j+0])]
					, vtx[(ib[j+1])]
					, vtx[(ib[j+2])]
					, &vtnrm );

				vtnrmAccumulated	+= vtnrm;
			}
		}

		D3DXVec3Normalize ( &vtx[i].nrm, &vtnrmAccumulated );		
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
void ksutGeo_SetDiffuse(VTX_TYPE vtx[], INT nSize, UINT ulColr)
{
	if( vtx[0].diff == ulColr )
		return;

	int i=0;
	for( ; i<nSize; ++i )
		vtx[i].diff	= ulColr;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
void ksutGeo_SetSpecular(VTX_TYPE vtx[], INT nSize, UINT ulColr)
{
	if( vtx[0].spec == ulColr )
		return;

	int i=0;
	for( ; i<nSize; ++i )
		vtx[i].spec	= ulColr;
};

EFFUT_END

#endif // !defined(AFX_EFFUT_GEO_H__E8AA32E2_D52D_4EC4_AD16_0CD7AF9ECBFA__INCLUDED_)
