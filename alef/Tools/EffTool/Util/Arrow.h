// Arrow.h: interface for the CArrow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARROW_H__1307C683_4D52_48B6_BD77_4BEC3C2449D6__INCLUDED_)
#define AFX_ARROW_H__1307C683_4D52_48B6_BD77_4BEC3C2449D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffUtil.h"

#include "rwcore.h"

//#include "./../stdIncludeAll.h"

class CArrow  
{
public:
	//struct
	struct tagstArrowHeader{
		RwUInt32	m_dwNumOfAngle;
		RwReal		m_fLineLen;
		RwReal		m_fConeHeight;
		RwReal		m_fConeRadius;

		RwV3d		m_uv3dDir;
		RwV3d		m_v3dStart;

		union{
			RwUInt32	m_adwCol[2];
			struct {
				RwUInt32	m_dwColBegin, m_dwColEnd;
			};
		};

		tagstArrowHeader();
	};

	struct tagstRwIm3dVtx : public RwIm3DVertex{
		
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

		void Update( const RwV3d& v3dObj	) { objVertex	= v3dObj;	};
		void Update( const RwUInt32 dwCol	) { color		= dwCol;	};
		void Update( const RwV3d& v3dObj, RwUInt32 dwCol ){
			objVertex	= v3dObj;
			color		= dwCol;
		};
	};

	//typedef
	typedef	struct tagstArrowHeader		ARROWHEADER, *LPARROWHEADER;
	typedef struct tagstRwIm3dVtx		RWIM3DVTX,	*LPRWIM3DVTX;
	typedef std::vector<RWIM3DVTX>		STLVEC_RWIM3DVTX;

private:
	RwMatrix			m_mat;
	RwUInt32			m_dwTransformFlags;
	ARROWHEADER			m_stArrowHeader;

	STLVEC_RWIM3DVTX	m_stlvecVtx;



	VOID		vClear();
	VOID		vUpdate();
	
	LPRWIM3DVTX	vGetPtrLine()			{ return &m_stlvecVtx[0];	};
	LPRWIM3DVTX	vGetPtrCone()			{ return &m_stlvecVtx[2];	};
	LPRWIM3DVTX	vGetPtrConeBottom()		{ return vGetPtrCone() + m_stArrowHeader.m_dwNumOfAngle * 3;	};
public:
	CArrow();
	virtual ~CArrow();

	VOID		bSetupNumOfAngle( RwUInt32 dwNumOfAngle );
	VOID		bSetupSize		( RwReal fAxisLen
								, RwReal fConeHeight
								, RwReal fConeRadius );
	VOID		bSetupColor( RwUInt32 dwColBegin, RwUInt32 dwColEnd);

	VOID		bSetPos( const RwV3d& v3dStartPos )	;
	VOID		bSetDir( const RwV3d& uv3dDir )		;

	VOID		bSetPosDirLen( const RwV3d& v3dStartPos
							 , const RwV3d& uv3dDir
							 , RwReal fLineLen );
	VOID		bSetStartNTarget( const RwV3d& v3dStartPos
								, const RwV3d& v3dTarget );

	RwBool		bRender(RwMatrix* pMat=NULL, RwUInt32* pTransformFlags=NULL);
};

typedef class CArrow ARROW, *PARROW, *LPARROW;

#endif // !defined(AFX_ARROW_H__1307C683_4D52_48B6_BD77_4BEC3C2449D6__INCLUDED_)
