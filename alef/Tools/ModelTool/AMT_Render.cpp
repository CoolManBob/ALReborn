#include "stdafx.h"
#include "ModelTool.h"

//@{ kday 20050305
//for matrix debugging
#ifdef _DEBUG
#include "AcuIMDraw.h"
#include "AcuObjecWire.h"
#endif //_DEBUG

#include "AgcuEffUtil.h"
//@} kday

//바닥에 Texture를 그려주고 싶은데.. ^^;

VOID CModelToolApp::RenderScene()
{
	m_csAgcEngine.OnRender();

	CRenderWare* pcsRenderWare = CModelToolDlg::GetInstance()->GetRenderForm()->GetRenderWare();
	if( !pcsRenderWare )	return;

	pcsRenderWare->BegineScene();

	m_csAgcEngine.GetAgcmRenderModule()->m_fRotAngle = m_fUpRotAngle;

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE ,(void*) true);

	m_csAgcEngine.GetAgcmRenderModule()->Render();

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);

	if (m_csAgcEngine.GetAgcmBlockingDlgModule()->IsOpenEditBlocking())
		RenderBlocking();

	if (m_csRenderOption.m_bShowCollisionClump)
		RenderObjectCollisionAtomic();

	if (m_csRenderOption.m_bShowPickClump)
		RenderPickingAtomic();

	if ( m_csRenderOption.m_bShowHitRange )
		RenderHitRange();

	if ( m_csRenderOption.m_bShowSiegeWarCollObj )
		RenderSiegeWarCollisionObject();

	if (m_csRenderOption.m_bShowTemp)
		RenderTemp();

	if(	(m_pcsPickedAtomic) &&
		((CModelToolDlg::GetInstance()->IsOpenEditEquipmentsDlg()) ||
		(m_csAgcEngine.GetAgcmUDADlgModule()->IsOpenMainUDADlg()) ||
		(m_csAgcEngine.GetAgcmSetFaceDlgModule()->IsOpenMainFaceDlg()))	)
	{
		RwUtilAtomicRenderBoundingBox(m_pcsPickedAtomic);
		RwUtilAtomicRenderAxis(m_pcsPickedAtomic);
	}

	RenderBShpere();

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);

	//@{ kday 20050305
	#ifdef _DEBUG
	AXISVIEW::RenderAxisContainer();
	AXISVIEW::ClearAxis();
	#endif //_DEBUG
	//@} kday

	if(m_csRenderOption.m_bShowCameraPos)
		RenderCameraPosBuffer(pcsRenderWare);

	if(m_csRenderOption.m_bShowClumpPolygons)
		RenderClumpPolygons(pcsRenderWare);

	if(m_csRenderOption.m_bShowClumpVertices)
		RenderClumpVertices(pcsRenderWare);

	if(m_csRenderOption.m_bShowFPS)
		pcsRenderWare->ShowFPS();

	if(m_pcsPickedAtomic)
		RenderPickedAtomicInfo(pcsRenderWare);

	if( GetEngine()->GetAgcmCustomizeRenderModule() ) {
		if( GetEngine()->GetAgcmCustomizeRenderModule()->IsStart() ) {
			GetEngine()->GetAgcmCustomizeRenderModule()->Render();
			GetEngine()->GetAgcmCustomizeRenderModule()->DrawQuad();
		}
	}

	pcsRenderWare->EndScene();
}

VOID CModelToolApp::RenderCameraPosBuffer(CRenderWare *pcsRenderWare)
{
	if( pcsRenderWare->GetCharset() )
		RwUtilPrintText(pcsRenderWare->GetCharset(), (RwChar *)(m_szCameraPosBuffer), 0, 1, rsPRINTPOSTOPRIGHT);
}

VOID CModelToolApp::RenderClumpPolygons(CRenderWare *pcsRenderWare)
{
	if(pcsRenderWare->GetCharset())
	{
		CHAR szTemp[AMT_MAX_STR];
		CHAR szTemp2[AMT_MAX_STR];
		sprintf( szTemp, "%s :", AMT_RENDER_NAME_CLUMP_POLYGONS );
		for( int i = 0; i < (int)m_vPolyNums.size(); i++ )
		{
			sprintf( szTemp2, " %d", m_vPolyNums[i] );
			strcat( szTemp, szTemp2 );
		}

		RwUtilPrintText(pcsRenderWare->GetCharset(), (RwChar *)(szTemp), 0, 2, rsPRINTPOSTOPRIGHT);
	}
}

VOID CModelToolApp::RenderClumpVertices(CRenderWare *pcsRenderWare)
{
	if(pcsRenderWare->GetCharset())
	{
		CHAR szTemp[AMT_MAX_STR];
		sprintf(szTemp, "%s : %d", AMT_RENDER_NAME_CLUMP_VERTICES, m_lVerNums);
		RwUtilPrintText(pcsRenderWare->GetCharset(), (RwChar *)(szTemp), 0, 3, rsPRINTPOSTOPRIGHT);
	}
}

VOID CModelToolApp::RenderPickedAtomicInfo(CRenderWare *pcsRenderWare)
{
	if(pcsRenderWare->GetCharset())
	{
		RwMatrix	*pMat;
		pMat = RwFrameGetMatrix(RpAtomicGetFrame(m_pcsPickedAtomic));

		CHAR szTemp[AMT_MAX_STR];

		sprintf(szTemp, "%f %f %f", pMat->right.x, pMat->right.y, pMat->right.z);
		RwUtilPrintText(pcsRenderWare->GetCharset(), (RwChar *)(szTemp), 0, 4, rsPRINTPOSTOPRIGHT);

		sprintf(szTemp, "%f %f %f", pMat->up.x, pMat->up.y, pMat->up.z);
		RwUtilPrintText(pcsRenderWare->GetCharset(), (RwChar *)(szTemp), 0, 5, rsPRINTPOSTOPRIGHT);

		sprintf(szTemp, "%f %f %f", pMat->at.x, pMat->at.y, pMat->at.z);
		RwUtilPrintText(pcsRenderWare->GetCharset(), (RwChar *)(szTemp), 0, 6, rsPRINTPOSTOPRIGHT);

		sprintf(szTemp, "%f %f %f", pMat->pos.x, pMat->pos.y, pMat->pos.z);
		RwUtilPrintText(pcsRenderWare->GetCharset(), (RwChar *)(szTemp), 0, 7, rsPRINTPOSTOPRIGHT);
	}
}

VOID CModelToolApp::RenderBlocking()
{
	AuBLOCKING	*pstBlocking;
	INT16		nBlockingNum;

	if(m_csAgcEngine.GetAgcmBlockingDlgModule()->IsOpenEditBlocking())
	{
		pstBlocking		= m_csAgcEngine.GetAgcmBlockingDlgModule()->GetBlocking();
		nBlockingNum	= m_csAgcEngine.GetAgcmBlockingDlgModule()->GetBlockingNum();
	}
	else
	{
		switch(m_eObjectType)
		{
		case AMT_OBJECT_TYPE_CHARACTER:
			{
				AgpdCharacterTemplate *pcsTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(m_lTID);
				if((!pcsTemplate) || (pcsTemplate->m_nBlockInfo < 1))
					return;

				pstBlocking		= pcsTemplate->m_astBlockInfo;
				nBlockingNum	= pcsTemplate->m_nBlockInfo;
			}
			break;

		case AMT_OBJECT_TYPE_ITEM:
			{
				return;
//				AgpdItemTemplate *pcsTemplate = m_csAgpmItem.GetItemTemplate(m_lTID);
//				if((!pcsTemplate) || (pcsTemplate->m_nBlockInfo < 1))
//					return;

//				pstBlocking		= pcsTemplate->m_astBlockInfo;
//				nBlockingNum	= pcsTemplate->m_nBlockInfo;
			}
			break;

		case AMT_OBJECT_TYPE_OBJECT:
			{
				ApdObjectTemplate *pcsTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(m_lTID);
				if((!pcsTemplate) || (pcsTemplate->m_nBlockInfo < 1))
					return;

				pstBlocking		= pcsTemplate->m_astBlockInfo;
				nBlockingNum	= pcsTemplate->m_nBlockInfo;
			}
			break;

		default:
			return;
		}
	}

	if(pstBlocking)
	{
		for(INT16 nCount = 0; nCount < nBlockingNum; ++nCount)
		{
			if(pstBlocking[nCount].type == AUBLOCKING_TYPE_SPHERE)
			{
				RwMatrix LTM = *(RwFrameGetLTM(RwCameraGetFrame(GetRenderWare()->GetCamera())));
				memcpy(&LTM.pos, &pstBlocking[nCount].data.sphere.center, sizeof(AuPOS));

				RenderBlocking(pstBlocking + nCount, nCount, &LTM);
			}
			else
			{
				RenderBlocking(pstBlocking + nCount, nCount, NULL);
			}
		}
	}
}

BOOL CModelToolApp::RenderBlocking(AuBLOCKING *pstBlocking, INT16 nIndex, RwMatrix *pLTM)
{
	static RwRGBA astRGBA[16] =
	{		
		255, 255,30, 255,	
		30, 255, 255, 255,	
		255, 30, 255, 255,	
		255, 30, 30, 255,		
		30, 255, 30, 255,		
		30, 30, 255, 255,		
		128, 30, 255, 255,	
		255, 128, 128, 255,
		128, 255, 128, 255,	
		128, 128, 255, 255,	
		198, 128, 128, 255,	
		255, 180, 140, 255,		
		130, 42, 190, 255,		
		35, 244, 180, 255,		
		50, 160, 32, 255,	
		128, 255, 0, 255
	};

	if(pstBlocking->type == AUBLOCKING_TYPE_NONE)
	{
		OutputDebugString("AuRwUtil_RenderBlocking() Error (1) !!!\n");
		return FALSE;
	}

	switch(pstBlocking->type)
	{
	case AUBLOCKING_TYPE_BOX:
		RenderBlocking(&pstBlocking->data.box, pLTM, astRGBA + nIndex);
		break;
	case AUBLOCKING_TYPE_SPHERE:
		RenderBlocking(&pstBlocking->data.sphere, pLTM, astRGBA + nIndex);
		break;
	case AUBLOCKING_TYPE_CYLINDER:
		RenderBlocking(&pstBlocking->data.cylinder, pLTM, astRGBA + nIndex);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CModelToolApp::RenderBlocking(AuBOX *pstBox, RwMatrix *pLTM, RwRGBA *pstRGBA)
{
	static RwImVertexIndex index[24] = 
    {
        0, 1,  1, 2,  2, 3,  3, 0,  4, 5,  5, 6,
        6, 7,  7, 4,  0, 4,  3, 7,  1, 5,  2, 6
    };

	RwIm3DVertex	boxVertices[8];
	RwUInt8			red, green, blue, alpha;

	red		= pstRGBA->red;
	green	= pstRGBA->green;
    blue	= pstRGBA->blue;
    alpha	= pstRGBA->alpha;

    RwIm3DVertexSetRGBA(&boxVertices[0], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[0], pstBox->inf.x, pstBox->inf.y, pstBox->inf.z);

    RwIm3DVertexSetRGBA(&boxVertices[1], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[1], pstBox->sup.x, pstBox->inf.y, pstBox->inf.z);

    RwIm3DVertexSetRGBA(&boxVertices[2], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[2], pstBox->sup.x, pstBox->sup.y, pstBox->inf.z);

    RwIm3DVertexSetRGBA(&boxVertices[3], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[3], pstBox->inf.x, pstBox->sup.y, pstBox->inf.z);

    RwIm3DVertexSetRGBA(&boxVertices[4], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[4], pstBox->inf.x, pstBox->inf.y, pstBox->sup.z);

    RwIm3DVertexSetRGBA(&boxVertices[5], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[5], pstBox->sup.x, pstBox->inf.y, pstBox->sup.z);

    RwIm3DVertexSetRGBA(&boxVertices[6], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[6], pstBox->sup.x, pstBox->sup.y, pstBox->sup.z);

    RwIm3DVertexSetRGBA(&boxVertices[7], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[7], pstBox->inf.x, pstBox->sup.y, pstBox->sup.z);

    if(RwIm3DTransform(boxVertices, 8, pLTM, rwIM3D_ALLOPAQUE))
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, index, 24);
        RwIm3DEnd();
    }

	return TRUE;
}

BOOL CModelToolApp::RenderBlocking(AuSPHERE *pstSphere, RwMatrix *pLTM, RwRGBA *pstRGBA)
{
	RwIm3DVertex	circle[AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + 1];
    RwV3d			point;
    RwInt32			i;
    RwMatrix		ltm;

	if(pLTM)
	{
		RwMatrixCopy(&ltm, pLTM);
	}
	else
	{
		RwMatrixSetIdentity(&ltm);
	}

    for(i = 0; i < AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + 1; i++)
    {
		point.x = (RwReal)RwCos(i / (AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS / 2.0f) * rwPI) * pstSphere->radius;
        point.y = (RwReal)RwSin(i / (AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS / 2.0f) * rwPI) * pstSphere->radius;
        point.z = 0.0f;

        RwIm3DVertexSetPos(&circle[i], point.x, point.y, point.z);
        RwIm3DVertexSetRGBA(&circle[i], pstRGBA->red, pstRGBA->green, pstRGBA->blue, pstRGBA->alpha);
    }

	if(RwIm3DTransform(circle, AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + 1, &ltm, rwIM3D_ALLOPAQUE))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DEnd();
    }

	return TRUE;
}

BOOL CModelToolApp::RenderBlocking(AuCYLINDER *pstCylinder, RwMatrix *pLTM, RwRGBA *pstRGBA)
{
	RwIm3DVertex	circle1[AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + 1];
	RwIm3DVertex	circle2[AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + 1];
	RwIm3DVertex	line[AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS];

	RwV3d			point, center;
	memcpy(&center, &pstCylinder->center, sizeof(AuPOS));

	for(RwInt32 i = 0; i < AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + 1; i++)
    {
        point.x = center.x + (RwReal)RwCos(i / (AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS / 2.0f) * rwPI) * pstCylinder->radius;
        point.z = center.z + (RwReal)RwSin(i / (AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS / 2.0f) * rwPI) * pstCylinder->radius;
        point.y = center.y;

        RwIm3DVertexSetPos(&circle1[i], point.x, point.y, point.z);
        RwIm3DVertexSetRGBA(&circle1[i], pstRGBA->red, pstRGBA->green, pstRGBA->blue, pstRGBA->alpha);

		point.y += pstCylinder->height;

		RwIm3DVertexSetPos(&circle2[i], point.x, point.y, point.z);
        RwIm3DVertexSetRGBA(&circle2[i], pstRGBA->red, pstRGBA->green, pstRGBA->blue, pstRGBA->alpha);
    }

	for( RwInt32 i = 0; i < AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS; i++)
	{
		memcpy(&line[i * 2 + 0], &circle1[i], sizeof(RwIm3DVertex));
		memcpy(&line[i * 2 + 1], &circle2[i], sizeof(RwIm3DVertex));
	}

	if(RwIm3DTransform(circle1, AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + 1, pLTM, rwIM3D_ALLOPAQUE))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DEnd();
    }

	if(RwIm3DTransform(circle2, AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + 1, pLTM, rwIM3D_ALLOPAQUE))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DEnd();
    }

	if(RwIm3DTransform(line, AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS + AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS, pLTM, rwIM3D_ALLOPAQUE))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
        RwIm3DEnd();
    }

	return TRUE;
}

BOOL CModelToolApp::RenderObjectCollisionAtomic()
{
	if(m_eObjectType != AMT_OBJECT_TYPE_OBJECT)
		return TRUE; // skip

	ApdObject *pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
	if(!pcsApdObject)
		return FALSE;

	AgcdObject *pcsAgcdObject = m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pcsApdObject);
	if(!pcsAgcdObject)
		return FALSE;

	if(!pcsAgcdObject->m_pstCollisionAtomic)
		return TRUE; // skip

	RwUtilAtomicRenderWireMesh(pcsAgcdObject->m_pstCollisionAtomic, &RWUTIL_RED);
	
	return TRUE;
}

BOOL CModelToolApp::RenderPickingAtomic()
{
//	if(m_eObjectType != AMT_OBJECT_TYPE_CHARACTER)
//		return TRUE; // skip

//@{ 2006/02/16 burumal

/*
	RpAtomic	*pstAtomic = NULL;

	switch (m_eObjectType)
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacter *pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
			if (!pcsAgpdCharacter)
				return FALSE;

			AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
			if (!pcsAgcdCharacter)
				return FALSE;

			if (!pcsAgcdCharacter->m_pPickAtomic)
				return TRUE; // skip

			pstAtomic						= pcsAgcdCharacter->m_pPickAtomic;
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObject *pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
			if (!pcsApdObject)
				return FALSE;

			AgcdObject *pcsAgcdObject = m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pcsApdObject);
			if (!pcsAgcdObject)
				return FALSE;

			if (!pcsAgcdObject->m_pstPickAtomic)
				return TRUE; // skip

			pstAtomic						= pcsAgcdObject->m_pstPickAtomic;
		}
		break;

	default:
		return FALSE;
	}

	if (pstAtomic)
		RwUtilAtomicRenderWireMesh(pstAtomic, &RWUTIL_BLUE);
*/

	RpAtomic* pstAtomic = NULL;

	switch ( m_eObjectType )
	{
	case AMT_OBJECT_TYPE_CHARACTER :
		{
			AgpdCharacter* pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
			if (!pcsAgpdCharacter)
				return FALSE;

			AgcdCharacter* pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
			if (!pcsAgcdCharacter)
				return FALSE;

			if ( !pcsAgcdCharacter->m_pPickAtomic )
				return TRUE; // skip

			//@{ 2006/03/28 burumal
			RpAtomic* pFirst = pcsAgcdCharacter->m_pPickAtomic;
			pstAtomic = pFirst;		

			do {
				RwUtilAtomicRenderWireMesh(pstAtomic, &RWUTIL_BLUE);
				
				/*
				AcuObjecWire::bGetInst().bRenderSphere(pstAtomic->boundingSphere);
				AcuObjecWire::bGetInst().bRenderSphere(*RpAtomicGetBoundingSphere(pstAtomic));
				*/

				pstAtomic = pstAtomic->next;
			} while ( pstAtomic != pFirst );			
			//@}
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT :
		{
			ApdObject* pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
			if (!pcsApdObject)
				return FALSE;

			AgcdObject* pcsAgcdObject = m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pcsApdObject);
			if (!pcsAgcdObject)
				return FALSE;

			if (!pcsAgcdObject->m_pstPickAtomic)
				return TRUE; // skip

			pstAtomic = pcsAgcdObject->m_pstPickAtomic;

			if ( pstAtomic )
				RwUtilAtomicRenderWireMesh(pstAtomic, &RWUTIL_BLUE);
		}
		break;

	default :
		return FALSE;
	}	

//@} 

/*	RwSphere stSph = *(RpAtomicGetBoundingSphere(pcsAgcdCharacter->m_pPickAtomic));
	RwMatrix stLTM = *(RwFrameGetLTM(RwCameraGetFrame(GetRenderWare()->GetCamera())));

	stLTM.pos.x = stSph.center.x;
	stLTM.pos.y = stSph.center.y;
	stLTM.pos.z = stSph.center.z;
	
	RwUtilRenderSphere(&stSph, &stLTM);*/

	return TRUE;
}

BOOL	GetRange( CModelToolApp* pApp, AgpdCharacter* pCharacter, INT& nRange )
{
	if( !pCharacter )	return FALSE;

	AgpmFactors* pFacterMgr = pApp->GetEngine()->GetAgpmFactorsModule();
	pFacterMgr->GetValue( &pCharacter->m_pcsCharacterTemplate->m_csFactor, &nRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HITRANGE );
	return nRange >= 0 ? TRUE : FALSE;
}

void	DrawHitRange( AgcdCharacter* pCharacter, INT& nRange )
{
	if( !pCharacter )					return;
	if( !pCharacter->m_pClump )			return;
	if( !pCharacter->m_pPickAtomic )	return;
	if( pCharacter->m_pPickAtomic != pCharacter->m_pPickAtomic->next )
		return;

	RwFrame* pFrame = RpClumpGetFrame( pCharacter->m_pClump );
	if( !pFrame )	return;

	RwSphere stSphere;
	stSphere.radius = (RwReal)nRange;
	stSphere.center.x = stSphere.center.z = 0;
	stSphere.center.y = pCharacter->m_pClump->stType.boundingSphere.center.y * pCharacter->m_pstAgcdCharacterTemplate->m_fScale;
	Eff2Ut_RenderSphereXZ( stSphere, 0xffff0000, pFrame );
}

BOOL CModelToolApp::RenderHitRange()
{	
	if( AMT_OBJECT_TYPE_CHARACTER != m_eObjectType )		return FALSE;
		
	if( !GetEngine() )										return TRUE;
	if( !GetEngine()->GetAgpmFactorsModule() )				return TRUE;
	if( !GetEngine()->GetAgcmCharacterModule() )			return TRUE;

	//My
	INT lRange = -1;
	AgpdCharacter* pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter( m_lID );
	if( GetRange( this, pcsAgpdCharacter, lRange ) )
	{
		AgcdCharacter* pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
		DrawHitRange( pcsAgcdCharacter, lRange );
	}

	//Target
	INT32 lOffset = m_lStartTargetID;
	for( INT32 lCount = 0; lCount < m_lNumTargets; ++lCount, --lOffset )
	{
		AgpdCharacter* pcsAgpdTarget = GetEngine()->GetAgpmCharacterModule()->GetCharacter( lOffset );		
		if( !GetRange( this, pcsAgpdTarget, lRange ) )	continue;

		AgcdCharacter* pcsAgcdTarget = GetEngine()->GetAgcmCharacterModule()->GetCharacterData( pcsAgpdTarget );
		DrawHitRange( pcsAgcdTarget, lRange );
	}

	return TRUE;
}

BOOL CModelToolApp::RenderTemp()
{
	//CRenderWare* pcsRenderWare = CModelToolDlg::GetInstance()->GetRenderForm()->GetRenderWare();
	//if( pcsRenderWare&& pcsRenderWare->GetCharset() )
	//{
	//	char szDir[512];
	//	::GetCurrentDirectory( 512, szDir );
	//	RwUtilPrintText( pcsRenderWare->GetCharset(), szDir, 0, 1, rsPRINTPOSTOPRIGHT );
	//}

	//이곳에서.. NodeID를 출력해주는 작업을 하자..

	RpHAnimHierarchy* pHierarchy = NULL;
	switch ( m_eObjectType )
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacter* pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
			if( !pcsAgpdCharacter )		return FALSE;

			AgcdCharacter* pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
			if( !pcsAgcdCharacter )		return FALSE;
			if( !pcsAgcdCharacter->m_pClump )	return FALSE;

			pHierarchy = m_csAgcEngine.GetAgcmCharacterModule()->GetHierarchy( pcsAgcdCharacter->m_pClump );
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItem* pcsAgpdItem = m_csAgcEngine.GetAgpmItemModule()->GetItem(m_lID);
			if( !pcsAgpdItem )		return FALSE;
			AgcdItem* pcsAgcdItem = m_csAgcEngine.GetAgcmItemModule()->GetItemData(pcsAgpdItem);
			if( !pcsAgcdItem )		return FALSE;
			if( !pcsAgcdItem->m_pstHierarchy )	return FALSE;

			pHierarchy = pcsAgcdItem->m_pstHierarchy;
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObject* pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
			if( !pcsApdObject )		return FALSE;

			AgcdObject* pcsAgcdObject = m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pcsApdObject);
			if( !pcsAgcdObject )	return FALSE;
			if( !pcsAgcdObject->m_stGroup.m_lNum || !pcsAgcdObject->m_stGroup.m_pstList )	return FALSE;

			pHierarchy = pcsAgcdObject->m_stGroup.m_pstList->m_csData.m_pstInHierarchy;
		}
		break;

	default :
		return FALSE;
	}

	if( !pHierarchy )	return FALSE;

	for( int i=0; i != (RwUInt32)pHierarchy->numNodes; i++ )
	{
		char szID[16];
		itoa( pHierarchy->pNodeInfo[i].nodeID, szID, 10 );

		RwV3d vPos = *RwMatrixGetPos( RwFrameGetLTM( const_cast<RwFrame*>(pHierarchy->pNodeInfo[i].pFrame) ) );
		m_csAgcEngine.GetAgcmTextBoardModule()->RenderText( szID, vPos, pHierarchy->pNodeInfo[i].pFrame );
		

		//RwSphere stSphere;
		//stSphere.radius = (RwReal)10;
		//stSphere.center.x = stSphere.center.y = stSphere.center.z = 0;
		//Eff2Ut_RenderSphereXZ( stSphere, 0xffff0000, pHierarchy->pNodeInfo[i].pFrame );
	}

	return TRUE;
}

VOID CModelToolApp::RenderBSphere(RwSphere *pBS)
{
	//@{ kday 20050609
	//AcuObjecWire::bGetInst().bRenderSphere( *pBS );
	::Eff2Ut_RenderBSphere( *pBS, 0xff00ffff, NULL );

	//if (pBS->radius == 0.0f)
	//	return;

	//RwMatrix	stLTM	= *(RwFrameGetLTM(RwCameraGetFrame(GetRenderWare()->GetCamera())));
	//stLTM.pos.x			= pBS->center.x;
	//stLTM.pos.y			= pBS->center.y;
	//stLTM.pos.z			= pBS->center.z;
	//
	//RwUtilRenderSphere(pBS, &stLTM, &RWUTIL_BLUE);
	//@{ kday
}

VOID CModelToolApp::RenderBShpere()
{
	if( m_csRenderOption.m_bShowBoundingSphere == FALSE )
		return;

	switch (m_eObjectType)
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	= m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(m_lTID);
			if (!pcsAgpdCharacterTemplate)
				return;

			AgcdCharacterTemplate	*pcsAgcdCharacterTemplate	= m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(pcsAgpdCharacterTemplate);
			if (!pcsAgcdCharacterTemplate)
				return;

			for (INT32	lIndex = 0; lIndex < m_stClumpArray.num; ++lIndex)
			{
				//@{ kday 20050609
				//AgcmRender::OcTreeModeRender 에서의 역활이 필요하다. 

				//@{ 2006/08/28 burumal
				if ( IsBadReadPtr(m_stClumpArray.array[lIndex]->atomicList, sizeof(RpAtomic*)) )
					continue;
				//@}

				RwSphere	sphere	= m_stClumpArray.array[lIndex]->stType.boundingSphere;
				RwV3dTransformPoint(&sphere.center,&sphere.center,RwFrameGetLTM(RpClumpGetFrame(m_stClumpArray.array[lIndex])));
				RenderBSphere(&sphere);
				//RenderBSphere(&m_stClumpArray.array[lIndex]->stType.boundingSphere);
				//@} kday
			}

//			RenderBSphere(&pcsAgcdCharacterTemplate->m_stBSphere);
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItemTemplate	*pcsAgpdItemTemplate	= m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(m_lTID);
			if (!pcsAgpdItemTemplate)
				return;

			AgcdItemTemplate	*pcsAgcdItemTemplate	= m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(pcsAgpdItemTemplate);
			if (!pcsAgcdItemTemplate)
				return;

			RenderBSphere(&pcsAgcdItemTemplate->m_stBSphere);
		}
		break;
		
	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObjectTemplate	*pcsApdObjectTemplate	= m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(m_lTID);
			if (!pcsApdObjectTemplate)
				return;

			AgcdObjectTemplate	*pcsAgcdObjectTemplate	= m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(pcsApdObjectTemplate);
			if (!pcsAgcdObjectTemplate)
				return;

			AgcdObjectTemplateGroupList	*pcsList		= pcsAgcdObjectTemplate->m_stGroup.m_pstList;
			while (pcsList)
			{
				RenderBSphere(&pcsList->m_csData.m_stBSphere);
				pcsList									= pcsList->m_pstNext;
			}
		}
		break;
	}
}

BOOL CModelToolApp::RenderSiegeWarCollisionObject()
{
	AgpdCharacterTemplate* pcsAgpdCharacterTemplate	= m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(m_lTID);
	if (!pcsAgpdCharacterTemplate)				return FALSE;

	AgcdCharacterTemplate* pcsAgcdCharacterTemplate	= m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(pcsAgpdCharacterTemplate);
	if (!pcsAgcdCharacterTemplate)				return FALSE;
	if (!pcsAgcdCharacterTemplate->m_pClump)	return FALSE;
	
	AgpdCharacter* pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if (!pcsAgpdCharacter)		return FALSE;

	AgcdCharacter* pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
	if (!pcsAgcdCharacter)		return FALSE;
	if (!pcsAgcdCharacterTemplate->m_pPickingAtomic )	return FALSE;	
	
	RwFrame* pFrame;
	if ( pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxWidth == 0.0f || pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxHeight == 0.0f )
	{
		// clump 기준으로 그림
		if ( pcsAgcdCharacter->m_pClump )
		{
			pFrame = RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
		
			RwSphere stSphere;
			stSphere.radius = pcsAgpdCharacterTemplate->m_fSiegeWarCollSphereRadius;
			stSphere.center.x = pcsAgcdCharacter->m_pClump->stType.boundingSphere.center.x + pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetX;
			stSphere.center.y = pcsAgcdCharacter->m_pClump->stType.boundingSphere.center.y;			
			stSphere.center.z = pcsAgcdCharacter->m_pClump->stType.boundingSphere.center.z + pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetZ;
			
			Eff2Ut_RenderSphereXZ(stSphere, 0xffffff00, pFrame);
		}
	}
	else
	{
		// clump 기준으로 그림
		if ( pcsAgcdCharacter->m_pClump )
		{
			pFrame = RpClumpGetFrame(pcsAgcdCharacter->m_pClump);			
			
			float fHalfWidth = pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxWidth / 2;
			float fHalfHeight = pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxHeight / 2;
		
			RwBBox stBox;
			stBox.inf.x = -fHalfWidth + pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetX;
			stBox.inf.z = fHalfHeight + pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetZ;

			stBox.sup.x = fHalfWidth + pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetX;		
			stBox.sup.z = -fHalfHeight + pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetZ;

			stBox.inf.y = pcsAgcdCharacter->m_pClump->stType.boundingSphere.radius;
			stBox.sup.y = stBox.inf.y - pcsAgcdCharacter->m_pClump->stType.boundingSphere.radius / 2;
			Eff2Ut_RenderBBox(stBox, pFrame);

			stBox.inf.y -= pcsAgcdCharacter->m_pClump->stType.boundingSphere.radius / 2;
			stBox.sup.y -= pcsAgcdCharacter->m_pClump->stType.boundingSphere.radius / 2;
			Eff2Ut_RenderBBox(stBox, pFrame);
		}
	}

	return TRUE;
}
