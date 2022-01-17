// PickUser.h: interface for the CPickUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICKUSER_H__21634823_4D6A_47EF_9D2D_03517679484D__INCLUDED_)
#define AFX_PICKUSER_H__21634823_4D6A_47EF_9D2D_03517679484D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwcore.h"

#include "arrow.h"

class CPickUser  
{
private:
	CPickUser();

public:
	virtual ~CPickUser();
	static CPickUser& GetInst(void);

	LPARROW			bGetPtrArrow()					{ return &m_cArrow; };
	const RwV3d*	bGetPtrRay(void)const			{ return &m_uv3dRay;	};
	const RwV3d*	bGetPtrCamPos(void)const		{ return &m_v3dCamPos;	};
	const POINT&	bGetMousePos(void)const			{ return m_ptMouse;		};
	const RwV3d*	bGetPtrPickedPoint(void)const	{ return &m_v3dPicked;	};
	RwReal			bGetDistance(void)const			{ return m_fDistance;	};
	void			bSetMousePos(const POINT& ptMouse)	{ m_ptMouse = ptMouse; };
		
	VOID	Make_Ray(  RwV3d* lpuv3dRay, RwV3d* lpv3dCamPos, RwCamera* lpRwCam, const POINT& ptMouse);
	RwBool	IntersectTriangle(const RwV3d *pRay, const RwV3d *pCamPos, 
							const RwV3d *pv0, const RwV3d *pv1, const RwV3d *pv2,
							RwReal* t, RwReal* u, RwReal* v, 
							RwBool bCalcPickedPt = FALSE );
	RwBool	bSphereCheck(const RwV3d *pRay, const RwV3d *pCamPos, const RwV3d *pvTargetPos, const RwReal fSBR);
	RwBool	bRwSphereCheck(const RwV3d* pRay, const RwV3d* pCamPos, const RwSphere& bsphere, const RwMatrix* pMat = NULL );
	RwBool	bRwBBoxCheck(const RwV3d* pRay, const RwV3d* pCamPos, const RwBBox& bbox, const RwMatrix* pMat = NULL );
	VOID	bGetRayCamPosWithMat( RwV3d* puvRayOut,RwV3d* pvCamPosOut, const RwMatrix* pMat , const RwV3d* puvRayIn	= NULL, const RwV3d* pvCamPosIn = NULL );
	VOID	bCalcPickedPoint(RwV3d& v3dOut, const RwV3d& v0, const RwV3d& v1, const RwV3d& v2, RwReal u, RwReal v);

private:
	RwV3d		m_uv3dRay;		//unit vector3 ray
	RwV3d		m_v3dCamPos;	//camera position
	RwV3d		m_v3dPicked;	//picked point
	RwReal		m_fDistance;	//distance between camera pos and picked point
	POINT		m_ptMouse;

	ARROW		m_cArrow;		//for visual picked point - eye pos
};

#define G_PICK	CPickUser::GetInst()

#endif // !defined(AFX_PICKUSER_H__21634823_4D6A_47EF_9D2D_03517679484D__INCLUDED_)
