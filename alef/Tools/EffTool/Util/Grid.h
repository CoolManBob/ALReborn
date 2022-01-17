// Grid.h: interface for the CGrid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRID_H__A2E03AE5_3BF5_4D84_B23F_C4D96734D4A2__INCLUDED_)
#define AFX_GRID_H__A2E03AE5_3BF5_4D84_B23F_C4D96734D4A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//----------------------------------------------------------------------------- m!!
//	include
//----------------------------------------------------------------------------- !!m
//#include <d3dx8math.h>

#pragma warning(disable: 4786)
#include <vector>
using std::vector;

#include "rwcore.h"

//----------------------------------------------------------------------------- m!!
//	class
//----------------------------------------------------------------------------- !!m
class CGrid  
{
	typedef struct tagstLine{
		union {
			struct {
				RwIm3DVertex	m_rwIm3dVtx1,
								m_rwIm3dVtx2;
			};
			RwIm3DVertex	m_aRwIm3dVtx[2];
		};

		tagstLine();
		tagstLine( const RwV3d& v3d1, const RwV3d& v3d2, RwUInt32 dwCol );
		tagstLine( const tagstLine& cpy );
		
		tagstLine& operator= (const tagstLine& cpy);

		void	Update( RwV3d* pv3d1, RwV3d* pv3d2, RwUInt32* pDwCol=NULL );
	}STLINE, *LPSTLINE;
	typedef std::vector<STLINE>	STLVEC_STLINE;

	STLVEC_STLINE				m_stlvecStLine;

	SIZE						m_sizeWorld;	
	RwV2d						m_v2dTileSize;
	RwUInt32					m_dwCol;
	RwV3d						m_v3dCenter;

	RwMatrix					m_mat;

	//rhw
	RwUInt32					m_dwLineNum;
	RwUInt32					m_dwTransformFlags;
public:
	CGrid();
	virtual ~CGrid();

	RwBool			bRender(RwMatrix* pMat, RwUInt32* pTransformFlags);

	INT				bBuildGrid(	RwInt32 wwidth, RwInt32 wdepth, 
								RwV2d v2dTileSize, 
								RwUInt32 col, RwV3d center);

	RwInt32			bSetColor	( RwUInt32 dwCol );
	RwInt32			bSetCenter	( RwV3d v3dCenter );
	RwInt32			bSetSzieTile( RwV2d v2dTileSize );
	RwInt32			bSetSzieMap	( RwInt32 wwidth, RwInt32 wdepth );

private:
	void	vClear();
	RwInt32	vUpdate( void );
}; typedef class CGrid GRID, *LPGRID;

#endif // !defined(AFX_GRID_H__A2E03AE5_3BF5_4D84_B23F_C4D96734D4A2__INCLUDED_)
