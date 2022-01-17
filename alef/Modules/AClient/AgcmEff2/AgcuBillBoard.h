#if !defined(AFX_AGUBILLBOARD_H__4D706458_9743_4867_ABE4_34F6558B3E7A__INCLUDED_)
#define AFX_AGUBILLBOARD_H__4D706458_9743_4867_ABE4_34F6558B3E7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "rwcore.h"
#include "rpworld.h"
#include "MagDebug.h"

#include "d3dx9math.h"

struct AXISWORLD
{
private:
	AXISWORLD();

public:
	const RwV3d*		bGetPtrWorldX		( VOID )	const	{ return &m_x; };
	const RwV3d*		bGetPtrWorldY		( VOID )	const	{ return &m_y; };
	const RwV3d*		bGetPtrWorldZ		( VOID )	const	{ return &m_z; };
	
	const RwV3d*		bGetPtrWorldNegX	( VOID )	const	{ return &m_nx; };
	const RwV3d*		bGetPtrWorldNegY	( VOID )	const	{ return &m_ny; };
	const RwV3d*		bGetPtrWorldNegZ	( VOID )	const	{ return &m_nz; };

	static AXISWORLD&	bGetInst			( VOID )			{ static AXISWORLD inst; return inst; };

private:
	RwV3d	m_x	, m_y , m_z ;
	RwV3d	m_nx, m_ny, m_nz;
};

#define PTRAXISWX	AXISWORLD::bGetInst().bGetPtrWorldX()
#define PTRAXISWY	AXISWORLD::bGetInst().bGetPtrWorldY()
#define PTRAXISWZ	AXISWORLD::bGetInst().bGetPtrWorldZ()

#define AXISWX	* AXISWORLD::bGetInst().bGetPtrWorldX()
#define AXISWY	* AXISWORLD::bGetInst().bGetPtrWorldY()
#define AXISWZ	* AXISWORLD::bGetInst().bGetPtrWorldZ()

struct BMATRIX
{
private:
	BMATRIX();

public:
	VOID Update( const RwCamera* pCam );

	const RwMatrix* const	bGetPtrMatB		( VOID )	const	{	return &m_matB;		}
	const RwMatrix* const	bGetPtrMatBY	( VOID )	const	{	return &m_matBY;	}
	RwMatrix*				bGetPtrMatB		( VOID )			{	return &m_matB;		}
	RwMatrix*				bGetPtrMatBY	( VOID )			{	return &m_matBY;	}

	const D3DXMATRIX* const bGetPtrD3DMatB	( VOID )	const	{	return &m_d3dmatB;	}
	const D3DXMATRIX* const bGetPtrD3DMatBY	( VOID )	const	{	return &m_d3dmatBY;	}
	D3DXMATRIX*				bGetPtrD3DMatB	( VOID )			{	return &m_d3dmatB;	}
	D3DXMATRIX*				bGetPtrD3DMatBY	( VOID )			{	return &m_d3dmatBY;	}

	static BMATRIX&			bGetInst		( VOID )			{ static BMATRIX inst; return *m_pThis; };

private:
	static BMATRIX*	m_pThis;

	RwMatrix	m_matB, m_matBY;
	D3DXMATRIX	m_d3dmatB, m_d3dmatBY;
};

class AgcuBillBoard  
{
private:
	AgcuBillBoard();

public:
	virtual ~AgcuBillBoard()		{		}

	RwBool	RenderIdle				( RwFrame* pFrm		, RwBool bFixedY=FALSE );
	RwBool	RenderIdle				( RwMatrix* pMat	, RwBool bFixedY=FALSE );

	static AgcuBillBoard& bGetInst	( VOID ) { static AgcuBillBoard inst; return *m_pThis; };

private:
	static AgcuBillBoard*	m_pThis;

	const RwMatrix*	m_pMatB;
	const RwMatrix*	m_pMatBY;
};

#endif