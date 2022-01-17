// Axis.h: interface for the CAxis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AXIS_H__097B0997_3D3C_44E0_96AF_16187A03549C__INCLUDED_)
#define AFX_AXIS_H__097B0997_3D3C_44E0_96AF_16187A03549C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//----------------------------------------------------------------------------- m!!
//	include
//----------------------------------------------------------------------------- !!m
#include <d3dx9math.h>

#include "rwcore.h"
//#include "./../stdIncludeAll.h"
#include "PickUser.h"

class CCamera;
//----------------------------------------------------------------------------- m!!
//	define constant
//----------------------------------------------------------------------------- !!m
#define	D3DFVF_AXIS			(D3DFVF_XYZ|D3DFVF_DIFFUSE)




//----------------------------------------------------------------------------- m!!
//	class
//----------------------------------------------------------------------------- !!m
typedef class CAxis  
{
public:
	//enum
	enum tageAxisOrder{
		AXIS_X = 0, AXIS_Y, AXIS_Z,
		AXIS_DIMENSION,
	};

	enum tageAxisShowMode{
		SHOW_NONE = 0, SHOW_LINE, SHOW_CONE, SHOW_ALL,
		SHOW_NUM,
	};

	//struct
	struct tagstAxisHeader{
		RwUInt32	m_dwNumOfAngle;
		RwReal		m_fAxisLen;
		RwReal		m_fConeHeight;
		RwReal		m_fConeRadius;

		union{
			RwUInt32	m_adwCol[2][AXIS_DIMENSION];
			struct {
				RwUInt32	m_dwNColX, m_dwNColY, m_dwNColZ;	//negative
				RwUInt32	m_dwPColX, m_dwPColY, m_dwPColZ;	//positive
			};
		};
	};

	//struct
	struct	tagstAxisWorld{
	private:
		union{
			RwV3d	m_av3dAxis[3];
			struct {
				RwV3d	m_v3dUnitX, m_v3dUnitY, m_v3dUnitZ;
			};
		};

		tagstAxisWorld(){	
			memset(this, 0, sizeof(*this));
			m_v3dUnitX.x	= 
			m_v3dUnitY.y	= 
			m_v3dUnitZ.z	= 1.f;
		};
	public:		
		const RwV3d& bGetX()const { return m_v3dUnitX; };
		const RwV3d& bGetY()const { return m_v3dUnitY; };
		const RwV3d& bGetZ()const { return m_v3dUnitZ; };

		static const tagstAxisWorld& bGetInst();// { static tagstAxisWorld inst; return inst; };
	};

	struct tagstRwIm3dVtx{
		RwIm3DVertex	m_vtx;
		
		tagstRwIm3dVtx(){
			memset( this, 0, sizeof(*this) );
		};
		tagstRwIm3dVtx( const tagstRwIm3dVtx& cpy ){		
			memcpy( this, &cpy, sizeof(*this) );
		};
		tagstRwIm3dVtx& operator=( const tagstRwIm3dVtx& cpy ){
			if( this == &cpy )	return *this;			
			memcpy( this, &cpy, sizeof(*this) );
			return *this;
		};

		void Update( const RwV3d& objVertex		) { m_vtx.objVertex	= objVertex;};
		void Update( const RwUInt32 dwCol		) { //m_vtx.color		= dwCol;	
		RwIm3DVertexSetRGBA( &m_vtx, DEF_GET_RED(dwCol), DEF_GET_GREEN(dwCol), DEF_GET_BLUE(dwCol), DEF_GET_ALPHA(dwCol) );  };
		void Update( const RwV3d& objVertex, RwUInt32 dwCol ){
			m_vtx.objVertex	= objVertex;
			//m_vtx.color		= dwCol;
			RwIm3DVertexSetRGBA( &m_vtx, DEF_GET_RED(dwCol), DEF_GET_GREEN(dwCol), DEF_GET_BLUE(dwCol), DEF_GET_ALPHA(dwCol) );
		};
	};



	//typedef  RpMaterial
	typedef enum tageAxisOrder			EAXISORDER;
	typedef enum tageAxisShowMode		EAXISSHOWMODE;
	typedef	struct tagstAxisHeader		AXISHEADER, *LPAXISHEADER;
	typedef struct tagstAxisWorld		AXISWORLD;
	typedef struct tagstRwIm3dVtx		RWIM3DVTX,	*LPRWIM3DVTX;
	typedef std::vector<RWIM3DVTX>		STLVEC_RWIM3DVTX;

private:
	RwBool				m_bSemi;
	RwBool				m_bShowLine,
						m_bShowCone;
	RwMatrix			m_mat;
	RwUInt32			m_dwTransformFlags;
	AXISHEADER			m_stAxisHeader;

	STLVEC_RWIM3DVTX	m_stlvecVtx;

public:
	CAxis();
	CAxis(const CAxis& cpy);
	virtual ~CAxis();

	CAxis& operator = (const CAxis& cpy);

	VOID		bSetupNumOfAngle( RwUInt32 dwNumOfAngle );
	VOID		bSetupSize		( RwReal fAxisLen
								, RwReal fConeHeight
								, RwReal fConeRadius );
	VOID		bSetupColorPositive( RwUInt32 dwColX, RwUInt32 dwColY, RwUInt32 dwColZ );
	VOID		bSetupColorNegative( RwUInt32 dwColX, RwUInt32 dwColY, RwUInt32 dwColZ );
	VOID		bSetupSemi( RwBool bSemi );
	VOID		bSetupShowLine( RwBool bShowLine )	{ m_bShowLine	= bShowLine; };
	VOID		bSetupShowCone( RwBool bShowCone )	{ m_bShowCone	= bShowCone; };

	VOID		bSetPos( const RwV3d& v3dPos )	{ m_mat.pos	= v3dPos;	};

	RwBool		bRender(RwMatrix* pMat=NULL, RwUInt32* pTransformFlags=NULL);
	RwBool		bRenderXZLine(RwMatrix* pMat=NULL, RwUInt32* pTransformFlags=NULL);

	RwBool		bPick( RwReal& fDist, EAXISORDER& eAxis, RwMatrix* pMat, CPickUser* pPick );

private:
	VOID		vClear();
	VOID		vUpdata();
	VOID		vUpdataX();
	VOID		vUpdataY();
	VOID		vUpdataZ();

	LPRWIM3DVTX	vGetPtrLine()			{ return &m_stlvecVtx[0];	};
	LPRWIM3DVTX	vGetPtrLineX()			{ return &m_stlvecVtx[0];	};
	LPRWIM3DVTX	vGetPtrLineY()			{ return &m_stlvecVtx[2];	};
	LPRWIM3DVTX	vGetPtrLineZ()			{ return &m_stlvecVtx[4];	};
	LPRWIM3DVTX	vGetPtrConeX()			{ return &m_stlvecVtx[6];	};
	LPRWIM3DVTX	vGetPtrConeY()			{ return &m_stlvecVtx[6 + m_stAxisHeader.m_dwNumOfAngle * 3 * 2];	};
	LPRWIM3DVTX	vGetPtrConeZ()			{ return &m_stlvecVtx[6 + m_stAxisHeader.m_dwNumOfAngle * 3 * 2 * 2];	};
	LPRWIM3DVTX	vGetPtrConeBottomX()	{ return vGetPtrConeX() + m_stAxisHeader.m_dwNumOfAngle * 3;	};
	LPRWIM3DVTX	vGetPtrConeBottomY()	{ return vGetPtrConeY() + m_stAxisHeader.m_dwNumOfAngle * 3;	};
	LPRWIM3DVTX	vGetPtrConeBottomZ()	{ return vGetPtrConeZ() + m_stAxisHeader.m_dwNumOfAngle * 3;	};
}AXIS, *LPAXIS;

#define G_WORLDAXIS	CAxis::tagstAxisWorld::bGetInst()
#endif // !defined(AFX_AXIS_H__097B0997_3D3C_44E0_96AF_16187A03549C__INCLUDED_)
