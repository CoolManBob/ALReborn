// AgcuBasisGeo.inl
// -----------------------------------------------------------------------------
//                            ____            _      _____                                 
//     /\                    |  _ \          (_)    / ____|                                
//    /  \    __ _  ___ _   _| |_) | __ _ ___ _ ___| |  __  ___  ___       ___ _ __  _ __  
//   / /\ \  / _` |/ __| | | |  _ < / _` / __| / __| | |_ |/ _ \/ _ \     / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| |_| | |_) | (_| \__ \ \__ \ |__| |  __/ (_) | _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|____/ \__,_|___/_|___/\_____|\___|\___/ (_) \___| .__/| .__/ 
//            __/ |                                                           | |   | |    
//           |___/                                                            |_|   |_|    
//
// uiLogin
//
// -----------------------------------------------------------------------------
// Originally created on 02/03/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcuBasisGeo.h"

#define RHORDER		//right hand order

// =============================================================================
//* GeoBuff
// =============================================================================
template<typename TVTX>
GeoBuff<TVTX>::GeoBuff(INT vtxnum, INT trinum)
{
	tReserve(vtxnum, trinum);
}
template<typename TVTX>
GeoBuff<TVTX>::~GeoBuff()
{
	m_VB.clear();
	m_IB.clear();
	m_IBW.clear();
}
template<typename TVTX>
VOID GeoBuff<TVTX>::tReserve(INT vtxnum, INT trinum)
{
	m_VB.reserve(vtxnum);
	m_IB.reserve(trinum*3);
	m_IBW.reserve(trinum*3*2);
}
template<typename TVTX>
VOID GeoBuff<TVTX>::tInitIB_wire()
{
	ASSERT( m_VB.size() && m_IB.size() && m_IBW.size() == 0 );
	
	for(size_t	i=0; i<m_IB.size(); i+=3)
	{
		m_IBW.push_back(m_IB.at(i+0));
		m_IBW.push_back(m_IB.at(i+1));

		m_IBW.push_back(m_IB.at(i+1));
		m_IBW.push_back(m_IB.at(i+2));

		m_IBW.push_back(m_IB.at(i+2));
		m_IBW.push_back(m_IB.at(i+0));
	}

	ASSERT( "kday" && m_IBW.size() == m_IB.size()*2 );
}

// =============================================================================
//* AgcuGeoBase
// =============================================================================
template<typename TVTX>
AgcuGeoBase<TVTX>::AgcuGeoBase(INT vtxnum, INT trinum
							   , GEOINFO::etype type, LPCSTR szName=NULL)
	: m_buff(vtxnum, trinum)
	, m_geoinfo(type, szName)
{
}

//template<typename TVTX>
//AgcuGeoBase<TVTX>::~AgcuGeoBase()
//{
//}

template<typename TVTX>
HRESULT AgcuGeoBase<TVTX>::bRender(LPDIRECT3DDEVICE9 dev, const LPD3DXMATRIX pmat)
{
	ASSERT(m_buff.m_VB.size() && m_buff.m_IB.size()>3);

	const D3DXMATRIX	mat(1.f,0.f,0.f,0.f,
							0.f,1.f,0.f,0.f,
							0.f,0.f,1.f,0.f,
							0.f,0.f,0.f,1.f);
	dev->SetTransform(D3DTS_WORLD, pmat ? pmat : &mat);
	dev->SetFVF( TVTX::FVF );

	HRESULT	hr = dev->DrawIndexedPrimitiveUP (
		D3DPT_TRIANGLELIST
		, 0U
		, static_cast<UINT>(m_buff.m_VB.size())
		, static_cast<UINT>(m_buff.m_IB.size()/3)
		, static_cast<LPCVOID>(&m_buff.m_IB.front())
		, D3DFMT_INDEX16
		, static_cast<LPCVOID>(&m_buff.m_VB.front())
		, TVTX::SIZE
		);

	return hr;
}

template<typename TVTX>
HRESULT AgcuGeoBase<TVTX>::bRenderW(LPDIRECT3DDEVICE9 dev, const LPD3DXMATRIX pmat)
{
	ASSERT(m_buff.m_VB.size() && m_buff.m_IBW.size()>6);

	const D3DXMATRIX	mat(1.f,0.f,0.f,0.f,
							0.f,1.f,0.f,0.f,
							0.f,0.f,1.f,0.f,
							0.f,0.f,0.f,1.f);
	dev->SetTransform(D3DTS_WORLD, pmat ? pmat : &mat);
	dev->SetFVF( TVTX::FVF );
	dev->SetTexture(0LU, NULL);

	HRESULT	hr = dev->DrawIndexedPrimitiveUP (
		D3DPT_TRIANGLELIST
		, 0U
		, static_cast<UINT>(m_buff.m_VB.size())
		, static_cast<UINT>(m_buff.m_IB.size()/3)
		, static_cast<LPCVOID>(&m_buff.m_IBW.front())
		, D3DFMT_INDEX16
		, static_cast<LPCVOID>(&m_buff.m_VB.front())
		, TVTX::SIZE
		);

	return hr;
}


template<typename TVTX>
void AgcuGeoBase<TVTX>::bRender(const RwMatrix* pmat)
{
	CRenderState	setRenderState(pmat);

	RwD3D9DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST
		, 0LU
		, static_cast<UINT>(m_buff.m_VB.size())
		, static_cast<UINT>(m_buff.m_IB.size()/2)
		, static_cast<LPCVOID>(&m_buff.m_IB.front())
		, static_cast<LPCVOID>(&m_buff.m_VB.front())
		, TVTX::SIZE
		);
};

template<typename TVTX>
void AgcuGeoBase<TVTX>::bRenderW(const RwMatrix* pmat)
{
	CRenderState	setRenderState(pmat);

	RwD3D9DrawIndexedPrimitiveUP( D3DPT_LINELIST
		, 0LU
		, static_cast<UINT>(m_buff.m_VB.size())
		, static_cast<UINT>(m_buff.m_IBW.size()/2)
		, static_cast<LPCVOID>(&m_buff.m_IBW.front())
		, static_cast<LPCVOID>(&m_buff.m_VB.front())
		, TVTX::SIZE
		);
};
// =============================================================================
//* AgcuGeo_Box
// =============================================================================
template<typename TVTX>
AgcuGeo_Box<TVTX>::AgcuGeo_Box(  FLOAT width
							   , FLOAT height
							   , FLOAT depth
							   , LPCSTR szName )
	: AgcuGeoBase<TVTX>(8, 6*2, GEOINFO::E_BOX, szName)
	, m_size(width, height, depth)
{
	tInitVB();
	tInitIB_solid();
	m_buff.tInitIB_wire();
}
template<typename TVTX>
AgcuGeo_Box<TVTX>::~AgcuGeo_Box()
{
}
template<typename TVTX>
VOID AgcuGeo_Box<TVTX>::tInitVB()
{
	ASSERT( m_buff.m_VB.size() == 0 );

	TVTX	vtx;
	//top
	vtx.pos	= D3DXVECTOR3( -0.5f*m_size.x,  0.5f*m_size.y,  0.5f*m_size.z );
	m_buff.m_VB.push_back(vtx);
	vtx.pos	= D3DXVECTOR3(  0.5f*m_size.x,  0.5f*m_size.y,  0.5f*m_size.z );
	m_buff.m_VB.push_back(vtx);
	vtx.pos	= D3DXVECTOR3(  0.5f*m_size.x,  0.5f*m_size.y,  -0.5f*m_size.z );
	m_buff.m_VB.push_back(vtx);
	vtx.pos	= D3DXVECTOR3( -0.5f*m_size.x,  0.5f*m_size.y,  -0.5f*m_size.z );
	m_buff.m_VB.push_back(vtx);
	//bottom
	vtx.pos	= D3DXVECTOR3( -0.5f*m_size.x, -0.5f*m_size.y,  0.5f*m_size.z );
	m_buff.m_VB.push_back(vtx);
	vtx.pos	= D3DXVECTOR3(  0.5f*m_size.x, -0.5f*m_size.y,  0.5f*m_size.z );
	m_buff.m_VB.push_back(vtx);
	vtx.pos	= D3DXVECTOR3(  0.5f*m_size.x, -0.5f*m_size.y,  -0.5f*m_size.z );
	m_buff.m_VB.push_back(vtx);
	vtx.pos	= D3DXVECTOR3( -0.5f*m_size.x, -0.5f*m_size.y,  -0.5f*m_size.z );
	m_buff.m_VB.push_back(vtx);
}
template<typename TVTX>
VOID AgcuGeo_Box<TVTX>::tInitIB_solid()
{
	ASSERT( m_buff.m_IB.size() == 0 );
#ifdef	RHORDER //right hand order
	//top
	m_buff.m_IB.push_back(0); m_buff.m_IB.push_back(3); m_buff.m_IB.push_back(1);
	m_buff.m_IB.push_back(1); m_buff.m_IB.push_back(3); m_buff.m_IB.push_back(2);
	//bottom															  
	m_buff.m_IB.push_back(7); m_buff.m_IB.push_back(4); m_buff.m_IB.push_back(6);
	m_buff.m_IB.push_back(6); m_buff.m_IB.push_back(4); m_buff.m_IB.push_back(5);
	//front																  
	m_buff.m_IB.push_back(3); m_buff.m_IB.push_back(7); m_buff.m_IB.push_back(2);
	m_buff.m_IB.push_back(2); m_buff.m_IB.push_back(7); m_buff.m_IB.push_back(6);
	//back																  
	m_buff.m_IB.push_back(1); m_buff.m_IB.push_back(5); m_buff.m_IB.push_back(0);
	m_buff.m_IB.push_back(0); m_buff.m_IB.push_back(5); m_buff.m_IB.push_back(4);
	//left																  
	m_buff.m_IB.push_back(0); m_buff.m_IB.push_back(4); m_buff.m_IB.push_back(3);
	m_buff.m_IB.push_back(3); m_buff.m_IB.push_back(4); m_buff.m_IB.push_back(7);
	//right																  
	m_buff.m_IB.push_back(2); m_buff.m_IB.push_back(6); m_buff.m_IB.push_back(1);
	m_buff.m_IB.push_back(1); m_buff.m_IB.push_back(6); m_buff.m_IB.push_back(5);
#else	//left hand order
#endif 
}

// =============================================================================
//* AgcuGeo_Sphere
// =============================================================================
template<typename TVTX>
AgcuGeo_Sphere<TVTX>::AgcuGeo_Sphere(  FLOAT radius
									 , INT anglesegment
									 , LPCSTR szName )
	: AgcuGeoBase<TVTX>( (anglesegment/2-1)*anglesegment + 2
				 , ((anglesegment/2-1)-1)*anglesegment*2 + anglesegment*2
				 , GEOINFO::E_SPHERE, szName)
	, m_radius(radius)
	, m_anglesegment(anglesegment)
{
	if( !vValidationChk() )
		m_buff.tReserve(  (m_anglesegment/2-1)*m_anglesegment + 2
						, ((m_anglesegment/2-1)-1)*m_anglesegment*2 + m_anglesegment*2
						);
	
	tInitVB();
	tInitIB_solid();
	m_buff.tInitIB_wire();
}
template<typename TVTX>
AgcuGeo_Sphere<TVTX>::~AgcuGeo_Sphere()
{
}
template<typename TVTX>
VOID AgcuGeo_Sphere<TVTX>::tInitVB()
{
	ASSERT( m_buff.m_VB.size() == 0 );

	TVTX	vtx;

	//+x pole
	vtx.pos	= D3DXVECTOR3(m_radius, 0.f, 0.f);
	m_buff.m_VB.push_back(vtx);

	//middle
	float radius = 0.f;
	float x		 = 0.f;
	const float f2pi	= 6.283185307179587f;
	for( int i=0; i<(m_anglesegment/2-1); ++i )
	{
		x		= m_radius * cosf( static_cast<float>(i+1)*f2pi/static_cast<float>(m_anglesegment) );
		radius	= m_radius * sinf( static_cast<float>(i+1)*f2pi/static_cast<float>(m_anglesegment) );

		for( int j=0; j<m_anglesegment; ++j )
		{
			vtx.pos	= D3DXVECTOR3(
				x
				, radius * sinf( static_cast<float>(j)*f2pi/static_cast<float>(m_anglesegment) )
				, radius * cosf( static_cast<float>(j)*f2pi/static_cast<float>(m_anglesegment) ) * (-1.f)
				);
			m_buff.m_VB.push_back(vtx);
		}
	}

	//-x pole
	vtx.pos	= D3DXVECTOR3(-m_radius, 0.f, 0.f);
	m_buff.m_VB.push_back(vtx);

	size_t numvtx = (size_t)((m_anglesegment/2-1)*m_anglesegment + 2);
	ASSERT( "kday" && numvtx == m_buff.m_VB.size() );
}
template<typename TVTX>
VOID AgcuGeo_Sphere<TVTX>::tInitIB_solid()
{
	ASSERT( m_buff.m_IB.size() == 0 );

#ifdef	RHORDER //right hand order
	//+x pole
	int	i = 0;
	for( i=0; i<m_anglesegment-1; ++i )
	{
		m_buff.m_IB.push_back((WORD)(0));	//+x pole
		m_buff.m_IB.push_back((WORD)(i+1));
		m_buff.m_IB.push_back((WORD)(i+2));
	}
	m_buff.m_IB.push_back((WORD)(0));	//+x pole
	m_buff.m_IB.push_back((WORD)(i+1));
	m_buff.m_IB.push_back((WORD)(1));

	//middle
	int startindex = 1;
	for( i=0; i<((m_anglesegment/2-1)-1); ++i, startindex+=m_anglesegment )
	{
		int j=0;
		for( j=0; j<m_anglesegment-1; ++j )
		{
			m_buff.m_IB.push_back( (WORD)(startindex+j) );
			m_buff.m_IB.push_back( (WORD)(startindex+j+m_anglesegment) );
			m_buff.m_IB.push_back( (WORD)(startindex+j+m_anglesegment+1) );
			
			m_buff.m_IB.push_back( (WORD)(startindex+j) );
			m_buff.m_IB.push_back( (WORD)(startindex+j+m_anglesegment+1) );
			m_buff.m_IB.push_back( (WORD)(startindex+j+1) );
		}//for(j)

		m_buff.m_IB.push_back( (WORD)(startindex+j) );
		m_buff.m_IB.push_back( (WORD)(startindex+j+m_anglesegment) );
		m_buff.m_IB.push_back( (WORD)(startindex+m_anglesegment) );
		
		m_buff.m_IB.push_back( (WORD)(startindex+j) );
		m_buff.m_IB.push_back( (WORD)(startindex+m_anglesegment) );
		m_buff.m_IB.push_back( (WORD)(startindex) );

	}//for(i)

	//-x pole
	for( i=0; i<m_anglesegment-1; ++i )
	{
		m_buff.m_IB.push_back((WORD)(m_buff.m_VB.size()-1));	//-x pole
		m_buff.m_IB.push_back((WORD)(startindex+i+1));
		m_buff.m_IB.push_back((WORD)(startindex+i));
	}
	m_buff.m_IB.push_back((WORD)(m_buff.m_VB.size()-1));	//+x pole
	m_buff.m_IB.push_back((WORD)(startindex));
	m_buff.m_IB.push_back((WORD)(startindex+i));

	
	size_t numtri = (size_t)(((m_anglesegment/2-1)-1)*m_anglesegment*2 + m_anglesegment*2);
	ASSERT( "kday" && numtri*3 == m_buff.m_IB.size() );

	size_t numvtx = (size_t)((m_anglesegment/2-1)*m_anglesegment + 2);
	for( size_t i=0; i<m_buff.m_IB.size(); ++i )
	{
		ASSERT( "kday" && (m_buff.m_IB.at(i) >= 0) && (m_buff.m_IB.at(i) < numvtx) );
	}

#else	//left hand order
#endif 
}
template<typename TVTX>
BOOL AgcuGeo_Sphere<TVTX>::vValidationChk()
{
	BOOL	br	= TRUE;
	if( m_anglesegment & 0x80000000 )//À½¼ö
	{
		m_anglesegment = ::abs(m_anglesegment);
		vValidationChk();
		br	= FALSE;
	}
	if( m_anglesegment & 0x00000001 )//È¦¼ö
	{
		++m_anglesegment;
		vValidationChk();
		br	= FALSE;
	}
	if( m_anglesegment<4 )//ÃÖ¼Ò°¢°¹¼ö
	{
		m_anglesegment=4;
		br	= FALSE;
	}

	return br;
}

template<typename TVTX>
void AgcuGeo_Sphere<TVTX>::bRwRender(const RwSphere& sphere)
{
	RwMatrix	mat;
	RwV3d		scale = { sphere.radius*2.f,sphere.radius*2.f,sphere.radius*2.f };
	RwMatrixScale ( &mat, &scale, rwCOMBINEREPLACE );
	RwMatrixTranslate( &mat, &sphere.center, rwCOMBINEPOSTCONCAT );

	AgcuGeoBase<TVTX>::bRender(&mat);
};
template<typename TVTX>
void AgcuGeo_Sphere<TVTX>::bRwRenderW(const RwSphere& sphere)
{
	RwMatrix	mat;
	RwV3d		scale = { sphere.radius*2.f,sphere.radius*2.f,sphere.radius*2.f };
	RwMatrixScale ( &mat, &scale, rwCOMBINEREPLACE );
	RwMatrixTranslate( &mat, &sphere.center, rwCOMBINEPOSTCONCAT );

	AgcuGeoBase<TVTX>::bRenderW(&mat);
};

// -----------------------------------------------------------------------------
// AgcuBasisGeo.inl - End of file
// -----------------------------------------------------------------------------
