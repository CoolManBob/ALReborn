#include "stdafx.h"
#include "ModelTool.h"

BOOL CModelToolApp::ScaleBlocking(RwV2d *pv2dDelta)
{
	AuBLOCKING *pstBlocking = m_csAgcEngine.GetAgcmBlockingDlgModule()->GetCurrentBlocking();
	if(!pstBlocking)
		return FALSE;

	BOOL	bHieght		= FALSE;
	RwReal	fDelta;
	RwV3d	v3dScale	= {0.0f, 0.0f, 0.0f};
	pv2dDelta->y			/= 10.0f;

	if(m_csAgcEngine.GetAgcmBlockingDlgModule()->EditIsHeight())
	{
		fDelta = (RwReal)((pv2dDelta->y) * (m_csEditOffset.m_fScale));
		v3dScale.x += fDelta;
		v3dScale.y += fDelta;
		v3dScale.z += fDelta;

		bHieght =  TRUE;
	}
	else if(m_csAgcEngine.GetAgcmBlockingDlgModule()->EditIsAll())
	{
		fDelta = (RwReal)((pv2dDelta->y) * (m_csEditOffset.m_fScale));
		v3dScale.x += fDelta;
		v3dScale.y += fDelta;
		v3dScale.z += fDelta;
	}
	else if(m_csAgcEngine.GetAgcmBlockingDlgModule()->EditIsRight())
	{
		v3dScale.x += (RwReal)((pv2dDelta->y) * (m_csEditOffset.m_fScale));
	}
	else if(m_csAgcEngine.GetAgcmBlockingDlgModule()->EditIsUp())
	{
		v3dScale.y += (RwReal)((pv2dDelta->y) * (m_csEditOffset.m_fScale));
	}
	else if(m_csAgcEngine.GetAgcmBlockingDlgModule()->EditIsAt())
	{
		v3dScale.z += (RwReal)((pv2dDelta->y) * (m_csEditOffset.m_fScale));
	}
	else
	{
		return FALSE;
	}

	return ScaleBlocking(pstBlocking, &v3dScale, bHieght);
}

BOOL CModelToolApp::ScaleBlocking(AuBLOCKING *pstBlocking, RwV3d *pstScale, BOOL bHeight)
{
	switch(pstBlocking->type)
	{
	case AUBLOCKING_TYPE_BOX:
		{
			RwV3d		v3dTempSup, v3dTempInf;
			
			AuBOX *pstBox = &pstBlocking->data.box;

			memcpy(&v3dTempSup, &pstBox->sup, sizeof(AuPOS));
			RwV3dAdd(&v3dTempSup, &v3dTempSup, pstScale);

			memcpy(&v3dTempInf, &pstBox->inf, sizeof(AuPOS));
			RwV3dSub(&v3dTempInf, &v3dTempInf, pstScale);

			if((v3dTempSup.x <= v3dTempInf.x) || (v3dTempSup.y <= v3dTempInf.y) || (v3dTempSup.z <= v3dTempInf.z))
				return TRUE; // skip

			memcpy(&pstBox->sup, &v3dTempSup, sizeof(AuPOS));
			memcpy(&pstBox->inf, &v3dTempInf, sizeof(AuPOS));
		}
		break;

	case AUBLOCKING_TYPE_SPHERE:
		{
			AuSPHERE *pstSphere = &pstBlocking->data.sphere;

			pstSphere->radius += pstScale->x; // x, y, z중에 하나
		}
		break;

	case AUBLOCKING_TYPE_CYLINDER:
		{
			AuCYLINDER *pstCylinder = &pstBlocking->data.cylinder;

			if(bHeight)
			{
				pstCylinder->height += pstScale->y; // x, y, z중에 하나
			}
			else
			{
				pstCylinder->radius += pstScale->x; // x, y, z중에 하나
			}
		}
		break;

	default:
		{
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL CModelToolApp::TranslateBlocking(RwV2d *pv2dDelta)
{
	AuBLOCKING *pstBlocking = m_csAgcEngine.GetAgcmBlockingDlgModule()->GetCurrentBlocking();
	if(!pstBlocking)
		return FALSE;

	BOOL	bAllMode = FALSE;
	RwV3d	v3dTranslate;
	RwV3d	v3dDir;

	if(m_csAgcEngine.GetAgcmBlockingDlgModule()->EditIsAll())
	{
		RwMatrix	*pstCameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(GetRenderWare()->GetCamera()));
		RwV3d		av3dAxis[2];

		av3dAxis[0] = *(RwMatrixGetRight(pstCameraMatrix));
		av3dAxis[1] = *(RwMatrixGetUp(pstCameraMatrix));

		RwV3dScale(av3dAxis + 0, av3dAxis + 0, (RwReal)((-pv2dDelta->x) * (m_csEditOffset.m_fPosit)));
		RwV3dScale(av3dAxis + 1, av3dAxis + 1, (RwReal)((-pv2dDelta->y) * (m_csEditOffset.m_fPosit)));
		RwV3dAdd(&v3dTranslate, av3dAxis + 0, av3dAxis + 1);

		bAllMode = TRUE;
	}
	else if(m_csAgcEngine.GetAgcmBlockingDlgModule()->EditIsRight())
	{
		v3dDir = RwUtilMakeV3d(1.0f, 0.0f, 0.0f);
	}
	else if(m_csAgcEngine.GetAgcmBlockingDlgModule()->EditIsUp())
	{
		v3dDir = RwUtilMakeV3d(0.0f, 1.0f, 0.0f);
	}
	else if(m_csAgcEngine.GetAgcmBlockingDlgModule()->EditIsAt())
	{
		v3dDir = RwUtilMakeV3d(0.0f, 0.0f, 1.0f);
	}
	else
	{
		return FALSE;
	}

	if(!bAllMode)
		RwV3dScale(&v3dTranslate, &v3dDir, (RwReal)((pv2dDelta->y) * (m_csEditOffset.m_fPosit)));

	return TranslateBlocking(pstBlocking, &v3dTranslate);
}

BOOL CModelToolApp::TranslateBlocking(AuBLOCKING *pstBlocking, RwV3d *pstTranslate)
{
	switch(pstBlocking->type)
	{
	case AUBLOCKING_TYPE_BOX:
		{
			AuBOX *pstBox = &pstBlocking->data.box;

			pstBox->inf.x += pstTranslate->x;
			pstBox->inf.y += pstTranslate->y;
			pstBox->inf.z += pstTranslate->z;

			pstBox->sup.x += pstTranslate->x;
			pstBox->sup.y += pstTranslate->y;
			pstBox->sup.z += pstTranslate->z;
		}
		break;

	case AUBLOCKING_TYPE_SPHERE:
		{
			AuSPHERE *pstSphere = &pstBlocking->data.sphere;

			pstSphere->center.x += pstTranslate->x;
			pstSphere->center.y += pstTranslate->y;
			pstSphere->center.z += pstTranslate->z;
		}
		break;

	case AUBLOCKING_TYPE_CYLINDER:
		{
			AuCYLINDER *pstCylinder = &pstBlocking->data.cylinder;

			pstCylinder->center.x += pstTranslate->x;
			pstCylinder->center.y += pstTranslate->y;
			pstCylinder->center.z += pstTranslate->z;
		}
		break;

	default:
		{
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL CModelToolApp::InitBlockingCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CModelToolApp	*pThis		= (CModelToolApp *)(pClass);
	AuBLOCKING		*pBlocking	= (AuBLOCKING *)(pData);

	pThis->MakeClumpBlocking(pThis->GetRenderClump(), pBlocking);

	return TRUE;
}

BOOL CModelToolApp::FinishUpEditingBlockingCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData))
		return FALSE;

	CModelToolApp			*pThis		= (CModelToolApp *)(pClass);
	AgcdGeometryDataLink	*pcsLink	= (AgcdGeometryDataLink *)(pData);

	if ((pcsLink) && (pcsLink->m_pcsHead))
		pThis->GetEngine()->GetAgcmGeometryDataManagerModule()->ResetGeometryList(&(pcsLink->m_pcsHead));

	for (INT32 lCount = 0; lCount < AMT_MAX_GEOMETRY_DATA_ARRAY; ++lCount)
	{
		if (pThis->m_astBlocking[lCount].type != AUBLOCKING_TYPE_NONE)
		{
			pThis->GetEngine()->GetAgcmGeometryDataManagerModule()->AddGeometry(pcsLink, (AcuGeometry *)(pThis->m_astBlocking + lCount));
		}
	}

	return TRUE;
}

BOOL CModelToolApp::MakeClumpBlocking(RwUtilClumpArray *pstClumpArray, AuBLOCKING *pstBlocking)
{
	if (pstClumpArray->num < 1)
		return FALSE;

	switch(pstBlocking->type)
	{
	case AUBLOCKING_TYPE_BOX:
		MakeClumpBlocking( pstClumpArray->array[0], &pstBlocking->data.box );
		break;
	case AUBLOCKING_TYPE_SPHERE:
		MakeClumpBlocking( pstClumpArray->array[0], &pstBlocking->data.sphere );
		break;
	case AUBLOCKING_TYPE_CYLINDER:
		MakeClumpBlocking( pstClumpArray->array[0], &pstBlocking->data.cylinder );
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CModelToolApp::MakeClumpBlocking(RpClump *pstClump, AuBOX *pstBox)
{
	/*
     * Get the atomic's vertices to calculate its bounding box...
     */	
	RwSphere sphere;
	RwUtilClumpGetBoundingSphere(pstClump, &sphere);

	pstBox->inf.x = sphere.center.x - sphere.radius;
	pstBox->inf.y = sphere.center.y - sphere.radius;
	pstBox->inf.z = sphere.center.z - sphere.radius;

	pstBox->sup.x = sphere.center.x + sphere.radius;
	pstBox->sup.y = sphere.center.y + sphere.radius;
	pstBox->sup.z = sphere.center.z + sphere.radius;

	return TRUE;
}

BOOL CModelToolApp::MakeClumpBlocking(RpClump *pstClump, AuSPHERE *pstSphere)
{
	RwSphere sphere;
	RwUtilClumpGetBoundingSphere(pstClump, &sphere);

	pstSphere->center.x = sphere.center.x;
	pstSphere->center.y = sphere.center.y;
	pstSphere->center.z = sphere.center.z;

	pstSphere->radius	= sphere.radius;

	return TRUE;
}

BOOL CModelToolApp::MakeClumpBlocking(RpClump *pstClump, AuCYLINDER *pstCylinder)
{
	RwSphere sphere;
	RwUtilClumpGetBoundingSphere(pstClump, &sphere);

	pstCylinder->height		= sphere.radius + sphere.radius;
	pstCylinder->center.x	= sphere.center.x;
	pstCylinder->center.z	= sphere.center.z;
	pstCylinder->center.y	= sphere.center.y - sphere.radius;

	pstCylinder->radius		= sphere.radius;

	return TRUE;
}