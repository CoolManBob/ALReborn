#include "AgcuBillBoard.h"
#include "AgcdEffGlobal.h"
#include "ApMemoryTracker.h"

BMATRIX*		BMATRIX::m_pThis		= NULL;
AgcuBillBoard*	AgcuBillBoard::m_pThis	= NULL;


AXISWORLD::AXISWORLD()
{
	m_x.x	= 1.f,	m_x.y	= 0.f,	m_x.z	= 0.f;
	m_y.x	= 0.f,	m_y.y	= 1.f,	m_y.z	= 0.f;
	m_z.x	= 0.f,	m_z.y	= 0.f,	m_z.z	= 1.f;
	
	RwV3dNegate ( &m_nx, &m_x );
	RwV3dNegate ( &m_ny, &m_y );
	RwV3dNegate ( &m_nz, &m_z );
}

BMATRIX::BMATRIX() : 
 m_d3dmatB( 1.f,0.f,0.f,0.f,
			0.f,1.f,0.f,0.f,
			0.f,0.f,1.f,0.f,
			0.f,0.f,0.f,1.f ),
 m_d3dmatBY(1.f,0.f,0.f,0.f,
			0.f,1.f,0.f,0.f,
			0.f,0.f,1.f,0.f,
			0.f,0.f,0.f,1.f)
{
	RwMatrixSetIdentity( &m_matB );
	RwMatrixSetIdentity( &m_matBY );

	ASSERT( !m_pThis && "this is singltone! @ BMATRIX::BMATRIX()" );

	m_pThis	= this;
}

VOID BMATRIX::Update( const RwCamera* pCam )
{
	ASSERT( pCam && "@ BMATRIX::Update()");
	ASSERT( RwCameraGetFrame( pCam ) && "@ BMATRIX::Update()");

	RwMatrix* pmatcam	= RwFrameGetLTM( RwCameraGetFrame( pCam ) );

	//1st-billboard
	RwMatrixTransform( &m_matB, pmatcam, rwCOMBINEREPLACE );
	RwV3dScale( &m_matB.right, &pmatcam->right, -1.f );
	RwV3dScale( &m_matB.at	 , &pmatcam->at	  , -1.f );
	m_matB.pos.x = m_matB.pos.y = m_matB.pos.z = 0.f;

	//2nd-billboard fixed y
	RwMatrixTransform( &m_matBY, pmatcam, rwCOMBINEREPLACE );
	m_matBY.up	= * AXISWORLD::bGetInst().bGetPtrWorldY();
	RwV3dCrossProduct( &m_matBY.at, &m_matBY.right, &m_matBY.up );		   
	m_matBY.pos.x = m_matBY.pos.y = m_matBY.pos.z = 0.f;

	//d3dxmatrix
	m_d3dmatB._11 = m_matB.right.x;
	m_d3dmatB._12 = m_matB.right.y;
	m_d3dmatB._13 = m_matB.right.z;

	m_d3dmatB._21 = m_matB.up.x;
	m_d3dmatB._22 = m_matB.up.y;
	m_d3dmatB._23 = m_matB.up.z;

	m_d3dmatB._31 = m_matB.at.x;
	m_d3dmatB._32 = m_matB.at.y;
	m_d3dmatB._33 = m_matB.at.z;

	m_d3dmatBY._11 = m_matBY.right.x;
	m_d3dmatBY._12 = m_matBY.right.y;
	m_d3dmatBY._13 = m_matBY.right.z;

	m_d3dmatBY._31 = m_matBY.at.x;
	m_d3dmatBY._32 = m_matBY.at.y;
	m_d3dmatBY._33 = m_matBY.at.z;
}

AgcuBillBoard::AgcuBillBoard() : 
 m_pMatB( BMATRIX::bGetInst().bGetPtrMatB() ),
 m_pMatBY( BMATRIX::bGetInst().bGetPtrMatBY() )
{
	ASSERT( !m_pThis && "this is singleton! @ AgcuBillBoard::AgcuBillBoard()" );
	m_pThis	= this;
}

RwBool AgcuBillBoard::RenderIdle( RwFrame* pFrm, RwBool bFixedY )
{
	ASSERT( pFrm && "@ AgcuBillBoard::RenderIdle(pFrm)" );
	if( !pFrm )		return FALSE;

	RwMatrix*	pMat		= RwFrameGetMatrix( pFrm );
	RwV3d		v3dPos		= *RwMatrixGetPos ( pMat );
	RwFrame*	pFrmParent	= RwFrameGetParent( pFrm );

	RwMatrixTransform( pMat, bFixedY ? m_pMatBY : m_pMatB, rwCOMBINEREPLACE ); 
	
	if( pFrmParent )
	{
		RwMatrix	matInvParent;
		RwMatrixInvert( &matInvParent, RwFrameGetLTM( pFrmParent ) );
		RwMatrixTransform( pMat, &matInvParent, rwCOMBINEPOSTCONCAT ); 
	}

	RwFrameUpdateObjects( pFrm );

	*RwMatrixGetPos ( pMat )	= v3dPos;

	return TRUE;
}

RwBool AgcuBillBoard::RenderIdle( RwMatrix* pMat, RwBool bFixedY )
{
	ASSERT( pMat && "@ AgcuBillBoard::RenderIdle(pMat)" );
	if( !pMat )		return FALSE;
		
	RwV3d	v3dPos	= *RwMatrixGetPos ( pMat );
	RwMatrixTransform ( pMat, bFixedY ? m_pMatBY : m_pMatB, rwCOMBINEREPLACE );
	*RwMatrixGetPos ( pMat )	= v3dPos;

	return TRUE;
}

