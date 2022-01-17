#include "AgcuEffVBMng.h"
#include "AgcdEffGlobal.h"
#include "AgcuEffTable.h"

#include "ApMemoryTracker.h"

#include "AcuSinTbl.h"
USING_ACUMATH;

const	RwUInt32	MAX_PARTICLE		= 1800;
const	RwUInt32	MAX_TAIL			= 400;
const	RwReal		DEFAULT_BOARDSIZE	= 7.064f*2.f;
const	RwUInt32	MAX_TERRAINBOARDSIZE = 10;


const char	g_vs11_particleRender[] =
{
	"vs.1.1\n"
	"dcl_position	v0\n"						//position
	"dcl_color		v1\n"						//color
	"dcl_texcoord0	v2\n"						//uv
	"dcl_texcoord1	v3\n"						//sin,cos,pscale_x,pscale_y

	"mul		r0.xy,		v3.zw,	c6.xy\n"	//halfWidth, halfHeight

	"mul		r3.xyz,		v3.y,	c4.xyz\n"	//fcos * (*(D3DXVECTOR3*)(&mat._11))
	"mul		r4.xyz,		v3.x,	c5.xyz\n"	//fsin * (*(D3DXVECTOR3*)(&mat._21))
	"add		r5.xyz,		r3,	   -r4\n"		//(fcos * (*(D3DXVECTOR3*)(&mat._11)) - fsin * (*(D3DXVECTOR3*)(&mat._21)))
	"mul		r1.xyz,		r0.x,	r5.xyz\n"	//halfWidth  * (fcos * (*(D3DXVECTOR3*)(&mat._11)) - fsin * (*(D3DXVECTOR3*)(&mat._21)))
		
	"mul		r3.xyz,		v3.x,	c4.xyz\n"	//fsin * (*(D3DXVECTOR3*)(&mat._11))
	"mul		r4.xyz,		v3.y,	c5.xyz\n"	//fcos * (*(D3DXVECTOR3*)(&mat._21))
	"add		r5.xyz,		r3,		r4\n"		//(fsin * (*(D3DXVECTOR3*)(&mat._11)) + fcos * (*(D3DXVECTOR3*)(&mat._21)))
	"mul		r2.xyz,		r0.y,	r5.xyz\n"	//halfWidth  * (fcos * (*(D3DXVECTOR3*)(&mat._11)) - fsin * (*(D3DXVECTOR3*)(&mat._21)))
		
	"add		r3.xyz,		r1.xyz,	r2.xyz\n"
	"add		r4.xyz,		v0.xyz,	r3.xyz\n"

	"mov		r4.w,		c6.z\n"				//r4.w	= 1.f;

	"m4x4		oPos,		r4,		c0\n"
	"mov		oD0,		v1\n"
	"mov		oT0,		v2\n"
};
static PVOID	getShader(const char* src)
{
	PVOID			ret	= NULL;
	HRESULT			hr	= 0;
	LPD3DXBUFFER	pShader	= NULL;
	LPD3DXBUFFER	pErrMsg	= NULL;

	hr	= ::D3DXAssembleShader( src, strlen(src), NULL, NULL, 0, &pShader, &pErrMsg );
	if( SUCCEEDED( hr ) )
	{
		RwBool br = RwD3D9CreateVertexShader( (const RwUInt32*)(pShader->GetBufferPointer()), &ret );
		if( !br )
		{
			ASSERT( !"kday" && "RwD3D9CreateVertexShader failed" );
			ret = NULL;
		}
	}
	else if( pErrMsg )
	{
		ASSERT( !"kday" && "pErrMsg == NULL" );
		OutputDebugString( (LPCSTR)(pErrMsg->GetBufferPointer()) );
	}

	DEF_SAFERELEASE( pErrMsg );
	DEF_SAFERELEASE( pShader );

	return ret;
};



AgcuEffVBMng	g_agcuEffVBMng;

//--------------------------- AgcuEffVBMng -----------------------
AgcuEffVBMng::AgcuEffVBMng():
 m_nVtxBoard(4),
 m_nVtxParticle(0),
 m_nVtxTail(0),
 m_nVtxTerrainBoard(0),
 m_pVShaderParticleSyst(NULL),
 m_pVDclParticleSyst(NULL)
{
}

AgcuEffVBMng::~AgcuEffVBMng()
{
}

VOID AgcuEffVBMng::Clear()
{
	m_agcuEffVB_Board.bClear();
	m_agcuEffVB_Particle.bClear();
	m_agcuEffVB_Tail.bClear();
	m_agcuEffVB_TerrainBoard.bClear();

	m_agcuEffVB_ParticleExtra.bClear();

	if( m_pVShaderParticleSyst )
	{
		RwD3D9DeleteVertexShader(m_pVShaderParticleSyst);
		m_pVShaderParticleSyst = NULL;
	}

	if( m_pVDclParticleSyst )
	{
		RwD3D9DeleteVertexDeclaration(m_pVDclParticleSyst);
		m_pVDclParticleSyst = NULL;
	}
}

#include <dxerr.h>
#pragma comment (lib, "dxerr.lib" )
HRESULT AgcuEffVBMng::InitVB(DWORD dwMaxParticle, DWORD dwMaxTail)
{
	Clear();

	m_pVShaderParticleSyst = getShader(g_vs11_particleRender);
	ASSERT( "kday" && m_pVShaderParticleSyst );
	
	D3DVERTEXELEMENT9 SDec[] =
	{
		{	  0, 0 , D3DDECLTYPE_FLOAT3		, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION	, 0},
		{	  0, 12, D3DDECLTYPE_D3DCOLOR	, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR		, 0},
		{	  0, 16, D3DDECLTYPE_FLOAT2		, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD	, 0},


		//x=sin, y=cos, z=paticle.scale.x, w=paticle.scale.y
		//z=paticle.scale.x, w=paticle.scale.y 요부분에서..
		//
		//pVtxRect->m_vtx0.m_v3Pos	= *((D3DXVECTOR3*)&v3dPos) - vx + vy;
		//pVtxRect->m_vtx1.m_v3Pos	= *((D3DXVECTOR3*)&v3dPos) - vx - vy;
		//pVtxRect->m_vtx2.m_v3Pos	= *((D3DXVECTOR3*)&v3dPos) + vx + vy;
		//pVtxRect->m_vtx3.m_v3Pos	= *((D3DXVECTOR3*)&v3dPos) + vx - vy;
		//요넘을 생각하야...
		//부호 셋팅을 잊지말도록.
		{	  1, 0, D3DDECLTYPE_FLOAT4		, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD	, 1},

	D3DDECL_END()
	};

	RwD3D9CreateVertexDeclaration ( SDec, &m_pVDclParticleSyst );
	ASSERT( "kday" && m_pVDclParticleSyst );

	LPRECTINDEX	pRectIndex	= NULL;

	m_nVtxParticle	= (INT)(dwMaxParticle * 4);
	m_nVtxTail		= (INT)(dwMaxTail*2+2);

	//board
	if( m_agcuEffVB_Board.bInit( 4, 6 ) )
	{
		DWORD	dwOffset = 0;
		if( SUCCEEDED( m_agcuEffVB_Board.bLockIB( (void**)&pRectIndex ) ) )
		{
			pRectIndex->SetIndex();
			m_agcuEffVB_Board.bUnlockIB();
		}else
		{
			Eff2Ut_ERR( "m_agcuEffVB_Board.bLockIB failed @ AgcuEffVBMng::bInitVB" );
			return E_FAIL;
		}
	}
	else
	{
		Eff2Ut_ERR( "m_agcuEffVB_Board.bInit failed @ AgcuEffVBMng::bInitVB" );
		return E_FAIL;
	}

	//init update
	STUVRECT			uvRect;
	D3DVTX_PCT_RECT		board( DEFAULT_BOARDSIZE, DEFAULT_BOARDSIZE, 0xFFFFFFFF, uvRect );
	LPD3DVTX_PCT_RECT	pBoard	= NULL;

	if( SUCCEEDED( LockVB_Board( (void**)&pBoard ) ) )
	{
		*pBoard	= board;
	}
	else
	{
		Eff2Ut_ERR( "bLockVB_Board failed @ AgcuEffVBMng::bInitVB" );
		return E_FAIL;
	}

	//particle
	if( m_agcuEffVB_Particle.bInit(dwMaxParticle*4LU, dwMaxParticle*6LU) )
	{
		if( SUCCEEDED( m_agcuEffVB_Particle.bLockIB( (void**)&pRectIndex ) ) )
		{
			for( DWORD i=0; i<dwMaxParticle; ++i, ++pRectIndex )
				pRectIndex->SetIndex(static_cast<WORD>(i*4));

			m_agcuEffVB_Particle.bUnlockIB();
		}
		else
		{
			Eff2Ut_ERR( "m_agcuEffVB_Particle.bLockIB failed @ AgcuEffVBMng::bInitVB" );
			return E_FAIL;
		}
	}
	else
	{
		Eff2Ut_ERR( "m_agcuEffVB_Board.bInit failed @ AgcuEffVBMng::bInitVB" );
		return E_FAIL;
	}

	if( m_agcuEffVB_ParticleExtra.bInit(dwMaxParticle*4LU, dwMaxParticle*6LU) )
	{
		//do nothing!
	}


	//init update
	//실시간 업데이트

	//tail
	if( m_agcuEffVB_Tail.bInit(dwMaxTail*2LU+2LU, dwMaxTail*6LU) )
	{
		if( SUCCEEDED( m_agcuEffVB_Tail.bLockIB( (void**)&pRectIndex ) ) )
		{
			for( DWORD i=0; i<dwMaxTail; ++i, ++pRectIndex )
			{
				pRectIndex->SetIndex(static_cast<WORD>(i*2));
			}

			m_agcuEffVB_Tail.bUnlockIB();
		}else
		{
			Eff2Ut_ERR( "m_agcuEffVB_Tail.bLockIB failed @ AgcuEffVBMng::bInitVB" );
			return E_FAIL;
		}
	}
	else
	{
		Eff2Ut_ERR( "m_agcuEffVB_Tail.bInit failed @ AgcuEffVBMng::bInitVB" );
		return E_FAIL;
	}
	//init update
	//실시간 업데이트

	//terrain board
	if( !m_agcuEffVB_TerrainBoard.bInit( (MAX_TERRAINBOARDSIZE+1)*(MAX_TERRAINBOARDSIZE+1)
		, (MAX_TERRAINBOARDSIZE)*(MAX_TERRAINBOARDSIZE) * 6 ) )
	{
		Eff2Ut_ERR( "m_agcuEffVB_Board.m_agcuEffVB_TerrainBoard failed @ AgcuEffVBMng::bInitVB" );
		return E_FAIL;
	}
	//init update
	//실시간 업데이트

	m_agcuEffVB_Board.bInitWire();
	m_agcuEffVB_Particle.bInitWire();
	m_agcuEffVB_Tail.bInitWire();
	
	return D3D_OK;
}

VOID AgcuEffVBMng::RenderBoard(const RwMatrix*  pmatWorld)
{
	m_agcuEffVB_Board.bRender( 4, 2, pmatWorld );

	if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_SHOWWIRE) )
		m_agcuEffVB_Board.bRenderW( 4, 2, pmatWorld );
}

VOID AgcuEffVBMng::RenderParticle( DWORD dwNumParticle, const RwMatrix*  pmatWorld, BOOL throughVertexShader)
{
	if(dwNumParticle)
	{
		if( throughVertexShader )
		{
			ASSERT( "kday" && m_pVShaderParticleSyst );

			RwMatrix	matcomposed;
			_rwD3D9VSSetActiveWorldMatrix( pmatWorld );
			_rwD3D9VSGetComposedTransformMatrix( &matcomposed );
			RwD3D9SetVertexShaderConstant( 0, (FLOAT*)&matcomposed, 4 );
			RwD3D9SetVertexDeclaration( m_pVDclParticleSyst );
			RwD3D9SetVertexShader( m_pVShaderParticleSyst );
			RwD3D9SetStreamSource( 1, m_agcuEffVB_ParticleExtra.bGetPtrVB() , m_agcuEffVB_ParticleExtra.bGetOffset(), D3DXVTX_PARTICLEEXTRA::SIZE );
			
			m_agcuEffVB_Particle.bRenderVS( dwNumParticle*4, dwNumParticle*2 );
			
			if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_SHOWWIRE) )
			m_agcuEffVB_Particle.bRenderWVS( dwNumParticle*4, dwNumParticle*2 );

			RwD3D9SetStreamSource( 1, NULL, 0, 0 );
			RwD3D9SetVertexShader(NULL);
			RwD3D9SetVertexDeclaration( NULL );
		}
		else
		{
			m_agcuEffVB_Particle.bRender( dwNumParticle*4, dwNumParticle*2, pmatWorld );
			
			if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_SHOWWIRE) )
				m_agcuEffVB_Particle.bRenderW( dwNumParticle*4, dwNumParticle*2, pmatWorld );
		}
	}
}

VOID AgcuEffVBMng::RenderTail( DWORD dwNumOfRect, const RwMatrix*  pmatWorld)
{
	m_agcuEffVB_Tail.bRender( dwNumOfRect*2 + 2, dwNumOfRect*2, pmatWorld );
	
	if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_SHOWWIRE) )
		m_agcuEffVB_Tail.bRenderW( dwNumOfRect*2LU+2LU, dwNumOfRect*2LU, pmatWorld);
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
HRESULT AgcuEffVBMng::LockVB_Board( VOID** ppVtx, DWORD dwNumVtx )
{
	return m_agcuEffVB_Board.bLockVB( ppVtx, dwNumVtx);
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
HRESULT AgcuEffVBMng::LockVB_Particle( VOID** ppVtx, DWORD dwNumVtx )
{
	return m_agcuEffVB_Particle.bLockVB( ppVtx, dwNumVtx);
}
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
HRESULT AgcuEffVBMng::LockVB_ParticleExtra( D3DXVECTOR4** ppVtx, DWORD dwNumVtx )
{
	return m_agcuEffVB_ParticleExtra.bLockVB( (VOID**)ppVtx, dwNumVtx);
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
HRESULT AgcuEffVBMng::LockVB_Tail( VOID** ppVtx, DWORD dwNumVtx )
{
	return m_agcuEffVB_Tail.bLockVB( ppVtx, dwNumVtx);
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
HRESULT AgcuEffVBMng::LockVB_TerrainBaord( VOID** ppVtx, DWORD dwNumVtx )
{
	return m_agcuEffVB_TerrainBoard.bLockVB( ppVtx, dwNumVtx );
};

///////////////////////////////////////////////////////////////////////////////
// util
///////////////////////////////////////////////////////////////////////////////
struct mat2by2
{
	union{
		struct
		{
			float _m11, _m12;
			float _m21, _m22;
		};
		float _m[2][2];
	};

	mat2by2() : _m11(1.f), _m12(0.f), _m21(0.f), _m22(1.f){};
	mat2by2(RwReal deg)
		: _m11( AcuSinTbl::Cos(deg) ), _m12( -AcuSinTbl::Sin(deg) )
		, _m21( AcuSinTbl::Sin(deg) ), _m22(  AcuSinTbl::Cos(deg) )
	{
	};
};
inline void RwV2dByMat2by2(RwV2d* pout, const RwV2d* pin, const mat2by2* pmat)
{
	RwV2d	in = *pin;
	pout->x = in.x * pmat->_m11 + in.y * pmat->_m21;
	pout->y = in.x * pmat->_m12 + in.y * pmat->_m22;
};

VOID AgcuEffVBMng::RenderTerrainBoard( const RwV3d& pos, RwReal radius, RwReal angle, RwRGBA rgba )
{
	const RwReal	TILESIZE	= 400.f;
	const RwReal	SQRT2		= 1.41421356f;
	const RwReal	INV_2RADISU = 1.f / (2.f * radius);

	//1. vertex num
	const RwReal	fw = SQRT2 * radius;	//충분히 큰 사정사각형의 폭/2.
	const POINT	ptstart = { static_cast<int>((pos.x - fw - ( (pos.x - fw < 0.f) ? TILESIZE : 0.f ) )/TILESIZE)
						  , static_cast<int>((pos.z - fw - ( (pos.z - fw < 0.f) ? TILESIZE : 0.f ) )/TILESIZE) };
	const POINT	ptend	= { static_cast<int>((pos.x + fw + ( (pos.x + fw < 0.f) ? 0.f : TILESIZE ) )/TILESIZE)
						  , static_cast<int>((pos.z + fw + ( (pos.z + fw < 0.f) ? 0.f : TILESIZE ) )/TILESIZE) };
	const RwV2d	lefttop = { static_cast<RwReal>(ptstart.x) * TILESIZE
						  , static_cast<RwReal>(ptstart.y) * TILESIZE };
	const RwInt32 width  = MIN( MAX_TERRAINBOARDSIZE, ptend.x - ptstart.x);
	const RwInt32 height = MIN( MAX_TERRAINBOARDSIZE, ptend.y - ptstart.y);
	m_nVtxTerrainBoard = (width+1)*(height+1);

	//2. 버텍스 재설정.
	const RwUInt32 colr = DEF_ARGB32(rgba.alpha, rgba.red, rgba.green, rgba.blue);
	const mat2by2	matuv(angle);
	RwReal		fz		= lefttop.y;
	D3DVTX_PCT*	pvtx	= NULL;
	WORD*		pindex	= NULL;
	m_agcuEffVB_TerrainBoard.bLockVB((LPVOID*)&pvtx, m_nVtxTerrainBoard);
	m_agcuEffVB_TerrainBoard.bLockIB((LPVOID*)&pindex, (width*height*6) );
	AgcmMap*	pAgcmMap = AgcdEffGlobal::bGetInst().bGetPtrAgcmMap();
	for(int i=0, acpitch=0; i<height; ++i, acpitch+=(width+1), fz+=TILESIZE )
	{
		RwReal fv=(fz - (pos.z - radius))*INV_2RADISU - 0.5f;
		RwReal fx=lefttop.x;
		for(int j=0; j<width; ++j, fx+= TILESIZE, ++pvtx, pindex+=6 )
		{
			//vertex setting
			pvtx->m_v3Pos.x = fx;
			pvtx->m_v3Pos.z = fz;
			pvtx->m_v3Pos.y = pAgcmMap ? pAgcmMap->HP_GetHeightGeometryOnly(fx, fz) + 5.f : pos.y + 0.1f;
			pvtx->m_dwColr = colr;
			pvtx->m_v2Tutv.x = (fx - (pos.x - radius))*INV_2RADISU - 0.5f;
			pvtx->m_v2Tutv.y = fv;
			//uv rotation
			RwV2dByMat2by2((RwV2d*)(&pvtx->m_v2Tutv), (RwV2d*)(&pvtx->m_v2Tutv), &matuv);
			pvtx->m_v2Tutv.x += 0.5f;
			pvtx->m_v2Tutv.y += 0.5f;

			//index setting
			BOOL bTurnEdge = TRUE;			
#ifndef USE_MFC
			{
				ApWorldSector		* pSector = pAgcmMap->m_pcsApmMap->GetSector( fx , fz );
				ApCompactSegment	* pSegment;
				if( pSector && 
					( pSegment = pSector->C_GetSegment( fx , 0.0f , fz ) ) )
				{
					bTurnEdge	= pSegment->stTileInfo.GetEdgeTurn();
				}
			}
#endif //USE_MFC
			if(bTurnEdge)
			{
				*(pindex+0) = static_cast<WORD>(acpitch+j+0);
				*(pindex+1) = static_cast<WORD>(acpitch+j+1);
				*(pindex+2) = static_cast<WORD>(acpitch+j+(width+1)+1);

				*(pindex+3) = static_cast<WORD>(acpitch+j+0);
				*(pindex+4) = static_cast<WORD>(acpitch+j+(width+1));
				*(pindex+5) = static_cast<WORD>(acpitch+j+(width+1)+1);
			}
			else
			{
				*(pindex+0) = static_cast<WORD>(acpitch+j+0);
				*(pindex+1) = static_cast<WORD>(acpitch+j+1);
				*(pindex+2) = static_cast<WORD>(acpitch+j+(width+1));

				*(pindex+3) = static_cast<WORD>(acpitch+j+1);
				*(pindex+4) = static_cast<WORD>(acpitch+j+(width+1)+1);
				*(pindex+5) = static_cast<WORD>(acpitch+j+(width+1));
			}
		}
		//one more
		{
			//vertex setting
			pvtx->m_v3Pos.x = fx;
			pvtx->m_v3Pos.z = fz;
			pvtx->m_v3Pos.y = pAgcmMap ? pAgcmMap->HP_GetHeightGeometryOnly(fx, fz) + 5.f : pos.y;
			pvtx->m_dwColr = colr;
			pvtx->m_v2Tutv.x = (fx - (pos.x - radius))*INV_2RADISU - 0.5f;
			pvtx->m_v2Tutv.y = fv;
			//uv rotation
			RwV2dByMat2by2((RwV2d*)(&pvtx->m_v2Tutv), (RwV2d*)(&pvtx->m_v2Tutv), &matuv);
			pvtx->m_v2Tutv.x += 0.5f;
			pvtx->m_v2Tutv.y += 0.5f;

			++pvtx;
		}
	}
	//one more
	{
		RwReal fv=(fz - (pos.z - radius))*INV_2RADISU - 0.5f;
		RwReal fx=lefttop.x;
		for( int j=0; j<width+1; ++j, fx+= TILESIZE, ++pvtx )
		{
			//vertex setting
			pvtx->m_v3Pos.x = fx;
			pvtx->m_v3Pos.z = fz;
			pvtx->m_v3Pos.y = pAgcmMap ? pAgcmMap->GetHeight(fx, fz , SECTOR_MAX_HEIGHT ) : pos.y;
			pvtx->m_dwColr = colr;
			pvtx->m_v2Tutv.x = (fx - (pos.x - radius))*INV_2RADISU - 0.5f;
			pvtx->m_v2Tutv.y = fv;
			//uv rotation
			RwV2dByMat2by2((RwV2d*)(&pvtx->m_v2Tutv), (RwV2d*)(&pvtx->m_v2Tutv), &matuv);
			pvtx->m_v2Tutv.x += 0.5f;
			pvtx->m_v2Tutv.y += 0.5f;
		}
	}
	m_agcuEffVB_TerrainBoard.bUnlockVB();
	m_agcuEffVB_TerrainBoard.bUnlockIB();

	//wire
	m_agcuEffVB_TerrainBoard.bInitWire();

	//3. setting samplerstate
	DWORD	dwAddressU = 0LU;
	DWORD	dwAddressV = 0LU;
	RwD3D9GetSamplerState (0, D3DSAMP_ADDRESSU, &dwAddressU);
	RwD3D9GetSamplerState (0, D3DSAMP_ADDRESSV, &dwAddressV);
	RwD3D9SetSamplerState (0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	RwD3D9SetSamplerState (0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	//4. render
	RwMatrix	matId;
	RwMatrixSetIdentity(&matId);
	m_agcuEffVB_TerrainBoard.bRender( m_nVtxTerrainBoard, (width*height*2), &matId );
	if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_SHOWWIRE ) )
		m_agcuEffVB_TerrainBoard.bRenderW( m_nVtxTerrainBoard, (width*height*2), &matId );

	//5. restor samplerstate
	RwD3D9SetSamplerState (0, D3DSAMP_ADDRESSU, dwAddressU);
	RwD3D9SetSamplerState (0, D3DSAMP_ADDRESSV, dwAddressV);
};
