// AgcuBasisGeo.h
// -----------------------------------------------------------------------------
//                            ____            _      _____                _     
//     /\                    |  _ \          (_)    / ____|              | |    
//    /  \    __ _  ___ _   _| |_) | __ _ ___ _ ___| |  __  ___  ___     | |__  
//   / /\ \  / _` |/ __| | | |  _ < / _` / __| / __| | |_ |/ _ \/ _ \    | '_ \ 
//  / ____ \| (_| | (__| |_| | |_) | (_| \__ \ \__ \ |__| |  __/ (_) | _ | | | |
// /_/    \_\\__, |\___|\__,_|____/ \__,_|___/_|___/\_____|\___|\___/ (_)|_| |_|
//            __/ |                                                             
//           |___/                                                              
//
// uiLogin
//
// -----------------------------------------------------------------------------
// Originally created on 02/03/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCUBASISGEO_20050203
#define _H_AGCUBASISGEO_20050203

#pragma once

#include <vector>
#include <windows.h>
#include <d3dx9math.h>
#include "rwcore.h"
#include "rwplcore.h"

//namespace
//AGCUGEOUD : Alef Global Client Util Geometry UserDefined
#ifndef AGCUGEOUD_BEGIN
#define AGCUGEOUD_BEGIN	namespace __AGCUGEOUD__ \
						{
#define AGCUGEOUD_END	};
#define USING_AGCUGEOUD	using namespace __AGCUGEOUD__;
#endif

AGCUGEOUD_BEGIN //##

// -----------------------------------------------------------------------------

// =============================================================================
//* type info
// =============================================================================
typedef struct stGeoInfo
{
	enum etype
	{
		E_BOX		= 0,
		E_SPHERE	,
		E_CONE		,
		E_CYLINDER	,

		E_NUM		,
	};
protected:
	enum	{NAME_LEN = 32};

	static LPCSTR	SZTYPE[E_NUM];

	const	etype	m_etype;
	CHAR			m_szName[NAME_LEN];

public:
	stGeoInfo(etype type, LPCSTR szName="unknown");

	LPCSTR	GetTypeName()const;
	LPCSTR	GetName()const;

}GEOINFO, *PGEOINFO, *LPGEOINFO;

// =============================================================================
//* GeoBuff
// =============================================================================
template<typename TVTX>class AgcuGeoBase;
template<typename TVTX>class AgcuGeo_Box;
template<typename TVTX>class AgcuGeo_Sphere;
template<typename TVTX>
class GeoBuff
{
protected:
	typedef std::vector<TVTX>				VectorVec;
	typedef std::vector<WORD>				STLVEC_INDEX;

	VectorVec		m_VB;
	STLVEC_INDEX	m_IB;
	STLVEC_INDEX	m_IBW;

	GeoBuff(INT vtxnum, INT trinum);
	virtual ~GeoBuff();

	virtual	VOID	tReserve(INT vtxnum, INT trinum);
	virtual VOID	tInitIB_wire();

	friend class AgcuGeoBase<TVTX>;
	friend class AgcuGeo_Box<TVTX>;
	friend class AgcuGeo_Sphere<TVTX>;
};

// =============================================================================
//* AgcuGeoBase
// =============================================================================
//* base geo
class AgcuRenderStateForWire
{
	RwUInt32 m_lighting;
	RwUInt32 m_alpha;
	RwUInt32 m_alphaTest;
public:
	AgcuRenderStateForWire()
		:m_lighting(0LU), m_alpha(0LU), m_alphaTest(0LU)
	{		
		RwD3D9GetRenderState(D3DRS_LIGHTING, (void*)&m_lighting);
		RwD3D9GetRenderState(D3DRS_ALPHABLENDENABLE, (void*)&m_alpha);
		RwD3D9GetRenderState(D3DRS_ALPHATESTENABLE, (void*)&m_alphaTest);

		RwD3D9SetVertexShader(NULL);
		RwD3D9SetPixelShader(NULL);
		RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);
		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		RwD3D9SetTexture( NULL, 0LU );
	}
	~AgcuRenderStateForWire()
	{
		RwD3D9SetRenderState(D3DRS_LIGHTING, m_lighting);
		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, m_alpha);
		RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, m_alphaTest);
	}
};

template<typename TVTX>
class AgcuGeoBase
{
	class CRenderState
	{
		RwUInt32 m_lighting;
		AgcuRenderStateForWire
				 m_renderStateForWire;
	public:
		CRenderState( const RwMatrix* pmat=NULL )
		{		
			RwD3D9SetTexture( NULL, 0LU );
			
			RwMatrix	mat;
			RwMatrixSetIdentity(&mat);
			RwD3D9SetTransformWorld( pmat? pmat : &mat );
			RwD3D9SetFVF( TVTX::FVF );
		}
		~CRenderState()
		{
		}
	};
public:
	// Construction/Destruction
	explicit AgcuGeoBase(INT vtxnum, INT trinum, GEOINFO::etype type, LPCSTR szName="unknown");
	virtual	~AgcuGeoBase(){};

	// Operators

	// Accessors

	// Interface methods
	virtual HRESULT	bRender(LPDIRECT3DDEVICE9 dev, const LPD3DXMATRIX pmat=NULL);
	virtual HRESULT	bRenderW(LPDIRECT3DDEVICE9 dev, const LPD3DXMATRIX pmat=NULL);
	
	virtual void	bRender(const RwMatrix* pmat=NULL);
	virtual void	bRenderW(const RwMatrix* pmat=NULL);
	
	virtual void	bRwRender(const RwSphere& sphere){};
	virtual void	bRwRenderW(const RwSphere& sphere){};


protected:
	// Data members
	GeoBuff<TVTX>	m_buff;
	GEOINFO			m_geoinfo;

	// Implementation methods

};

// =============================================================================
//* AgcuGeo_Box
// =============================================================================
template<typename TVTX>
class AgcuGeo_Box : public AgcuGeoBase<TVTX>
{
public:
	// Construction/Destruction
	explicit AgcuGeo_Box(FLOAT width=1.f, FLOAT height=1.f, FLOAT depth=1.f, LPCSTR szName="unknown" );
	virtual	~AgcuGeo_Box();

	// Operators

	// Accessors

	// Interface methods

protected:
	// Data members
	D3DXVECTOR3		m_size;

	// Implementation methods
	virtual VOID	tInitVB();
	virtual VOID	tInitIB_solid();
};

// =============================================================================
//* AgcuGeo_Sphere
// =============================================================================
template<typename TVTX>
class AgcuGeo_Sphere : public AgcuGeoBase<TVTX>
{
public:
	// Construction/Destruction
	explicit AgcuGeo_Sphere(FLOAT radius, INT anglesegment=14, LPCSTR szName="unknown" );
	virtual	~AgcuGeo_Sphere();

	// Operators

	// Accessors

	// Interface methods
	void	bRwRender(const RwSphere& sphere);
	void	bRwRenderW(const RwSphere& sphere);

protected:
	// Data members
	FLOAT	m_radius;
	INT		m_anglesegment;

	// Implementation methods
	virtual VOID	tInitVB();
	virtual VOID	tInitIB_solid();
private:
	BOOL			vValidationChk();
};


#include "AgcuBasisGeo.inl"

AGCUGEOUD_END


#endif // _H_AGCUBASISGEO_20050203
// -----------------------------------------------------------------------------
// AgcuBasisGeo.h - End of file
// -----------------------------------------------------------------------------

