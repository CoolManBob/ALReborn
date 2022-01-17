// AgcuEffVBMng.h: interface for the AgcuEffVBMng class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCUEFFVBMNG_H__BD7E9485_88BB_42C3_89DE_300F28ACFA31__INCLUDED_)
#define AFX_AGCUEFFVBMNG_H__BD7E9485_88BB_42C3_89DE_300F28ACFA31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffVB.h"

extern const	RwUInt32	MAX_PARTICLE;
extern const	RwUInt32	MAX_TAIL;
extern const	RwReal		DEFAULT_BOARDSIZE;

class AgcuEffVBMng  
{
public:
	AgcuEffVBMng( VOID );
	virtual ~AgcuEffVBMng( VOID );

	VOID		Clear					( VOID );

	HRESULT		InitVB					( DWORD dwMaxParticle=MAX_PARTICLE, DWORD dwMaxTail=MAX_TAIL );

	VOID		RenderBoard				( const RwMatrix*  pmatWorld=NULL );
	VOID		RenderParticle			( DWORD dwNumParticle, const RwMatrix*  pmatWorld=NULL, BOOL throughVertexShader=TRUE );

	VOID		RenderTail				( DWORD dwNumOfRect, const RwMatrix*  pmatWorld=NULL );
	VOID		RenderTerrainBoard		( const RwV3d& pos, RwReal radius, RwReal angle, RwRGBA rgba );

	HRESULT		LockVB_Board			( VOID** ppVtx		 , DWORD dwNumVtx=0LU );
	HRESULT		LockVB_Particle			( VOID** ppVtx		 , DWORD dwNumVtx=0LU );
	HRESULT		LockVB_Tail				( VOID** ppVtx		 , DWORD dwNumVtx=0LU );
	HRESULT		LockVB_TerrainBaord		( VOID** ppVtx		 , DWORD dwNumVtx=0LU );
	HRESULT		LockVB_ParticleExtra	( D3DXVECTOR4** ppVtx, DWORD dwNumVtx=0LU );

	HRESULT		UnlockVB_Board			( VOID )		{ return m_agcuEffVB_Board.bUnlockVB(); };
	HRESULT		UnlockVB_Particle		( VOID )		{ return m_agcuEffVB_Particle.bUnlockVB(); };
	HRESULT		UnlockVB_Tail			( VOID )		{ return m_agcuEffVB_Tail.bUnlockVB(); };
	HRESULT		UnlockVB_TerrainBoard	( VOID )		{ return m_agcuEffVB_TerrainBoard.bUnlockVB(); };
	HRESULT		UnlockVB_ParticleExtra	( VOID )		{ return m_agcuEffVB_ParticleExtra.bUnlockVB(); };

	//access
	INT			GetNumVtx_Board			( VOID )const	{ return m_nVtxBoard;			}
	INT			GetNumVtx_Particle		( VOID )const	{ return m_nVtxParticle;		}
	INT			GetNumVtx_Tail			( VOID )const	{ return m_nVtxTail;			}
	INT			GetNumVtx_TerrainBoard	( VOID )const	{ return m_nVtxTerrainBoard;	}

private:
	AgcuEffVB< D3DVTX_PCT >				m_agcuEffVB_Board;
	AgcuEffVB< D3DVTX_PCT >				m_agcuEffVB_Particle;
	AgcuEffVB< D3DVTX_PCT >				m_agcuEffVB_Tail;
	AgcuEffVB< D3DVTX_PCT >				m_agcuEffVB_TerrainBoard;
	AgcuEffVB< D3DXVTX_PARTICLEEXTRA >	m_agcuEffVB_ParticleExtra;
							
	INT									m_nVtxBoard;
	INT									m_nVtxParticle;
	INT									m_nVtxTail;
	INT									m_nVtxTerrainBoard;
	PVOID								m_pVShaderParticleSyst;
	PVOID								m_pVDclParticleSyst;
};

extern AgcuEffVBMng	g_agcuEffVBMng;

#endif