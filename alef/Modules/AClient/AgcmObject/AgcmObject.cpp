// AgcmObject.cpp: implementation of the AgcmObject class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmObject.h"
#include "AgcdObject.h"

#include "ApmObject.h"
#include "ApdObject.h"

#include "AcuObject.h"
#include "RpLODAtm.h"
#include "RpUsrDat.h"

#include "AcuTexture.h"

#include "RtPITexD.h"

#include <winbase.h>

#include "AgcmUIConsole.h"
#include "ApModuleStream.h"

#include "AgcmPreLODManager.h"
#include "RwUtil.h"


//#include "AgcmMap.h"

#ifdef USE_DPVS
#include "rpdpvs.h"
#endif //USE_DPVS

//@{ Jaewon 20040706
#include "AgcmShadowmap.h"
//@} Jaewon

#include "ApMemoryTracker.h"

////@{ kday 20050513
//#ifdef USE_MFC
//#include "AcuObjecWire.h"
//#endif //USE_MFC
////@} kday

BOOL		g_bShowCollisionAtomic	= FALSE;
#define GET_HEIGHT_OFFSET	(100.0f)

AgcmObject * AgcmObject::m_pThis	= NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmObject::AgcmObject()
{
	SetModuleName("AgcmObject");

	EnableIdle(TRUE);

	// 이 Module은 Datamodule이다.
	m_uType							= AgcModule::DATAMODULE	;

	m_pWorld						= NULL					;

	m_nObjectAttachIndex			= -1					;
	m_nObjectTemplateAttachIndex	= -1					;

	m_bRangeApplied					= FALSE					;

	m_bUseTexDict					= FALSE;

	m_szTexDict[0]					= '\0';
	m_pstTexDict					= NULL;

	m_lMaxAnimNum					= 0;
	m_ulCurTick						= 0;

	SetPacketType(AGPMOBJECT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,			// operation type
							AUTYPE_INT32,		1,			// object id
							AUTYPE_INT8,		1,			// object status
							AUTYPE_END,			0
							);

	ASSERT( NULL == AgcmObject::m_pThis );
	AgcmObject::m_pThis	= this;

	m_bAutoLoad						= TRUE	;

	m_bSetupNormalObject			= TRUE	;
	m_bSetupSystemObject			= TRUE	;

	ZeroMemory(m_szClumpPath, sizeof(CHAR) * AGCMOBJECT_MAX_PATH_CLUMP);
	ZeroMemory(m_szAnimationPath, sizeof(CHAR) * AGCMOBJECT_MAX_STR);

	m_fRangeStartX	= 0.0f;
	m_fRangeEndX	= 0.0f;
	m_fRangeStartZ	= 0.0f;
	m_fRangeEndZ	= 0.0f;

	m_CurObject		= NULL; 
	m_CurGroupNode	= NULL;

	m_lLoaderTemplateIndex			= 0;
	m_lLoaderInitIndex				= 0;

	m_pcsApmObject				= NULL;
	m_pcsAgcmRender				= NULL;
	m_pcsAgcmLODManager			= NULL;
	m_pcsApmMap					= NULL;
	m_pcsAgcmMap				= NULL;
	m_pcsAgcmResourceLoader		= NULL;
	m_pcsAgcmShadow				= NULL;
	m_pcsAgcmOcTree				= NULL;
	m_pcsApmOcTree				= NULL;

	m_szTexturePath[0] = 0;

	m_ulModuleFlags				= E_AGCM_OBJECT_FLAGS_NONE;

	m_bShowPickingInfo			= FALSE;

	m_fnDefaultTextureReadCB = NULL;
}

AgcmObject::~AgcmObject()
{

}

// 012803 Bob님 작업(Modify, InitObject관련 수정)
BOOL AgcmObject::OnAddModule()
{
	// Parent Module인 ApmObject를 찾는다.
	m_pcsApmObject = (ApmObject *) GetModule("ApmObject");
	if (!m_pcsApmObject)
		return FALSE;

	m_pcsAgcmRender			= ( AgcmRender *		) GetModule("AgcmRender"	);
	m_pcsAgcmLODManager		= ( AgcmLODManager *	) GetModule("AgcmLODManager");
	m_pcsApmMap				= ( ApmMap *			) GetModule("ApmMap"		);
	m_pcsAgcmMap			= ( AgcmMap *			) GetModule("AgcmMap"		);
	m_pcsAgcmResourceLoader	= ( AgcmResourceLoader *) GetModule("AgcmResourceLoader");
	m_pcsAgcmShadow			= (	AgcmShadow *		) GetModule("AgcmShadow");

	m_pcsAgcmOcTree			= (	AgcmOcTree *		) GetModule("AgcmOcTree");
	m_pcsApmOcTree			= (	ApmOcTree*			) GetModule("ApmOcTree");

	//AgcmMap	* pCmMap	= ( AgcmMap *			) GetModule("AgcmMap"		);

	// ApmObject에 Data 붙인다.
	m_nObjectTemplateAttachIndex = m_pcsApmObject->AttachObjectTemplateData(this, sizeof(AgcdObjectTemplate), OnObjectTemplateCreate, OnObjectTemplateDestroy);
	if (m_nObjectTemplateAttachIndex == -1)
		return FALSE;

	m_nObjectAttachIndex = m_pcsApmObject->AttachObjectData(this, sizeof(AgcdObject), OnObjectCreate, OnObjectDestroy);
	if (m_nObjectAttachIndex == -1)
		return FALSE;

	// Callback도 등록한다.
	if(m_pcsApmObject)
	{
		if (!m_pcsApmObject->AddStreamCallback(APMOBJECT_DATA_OBJECT_TEMPLATE, ObjectTemplateStreamReadCB, ObjectTemplateStreamWriteCB, this))
			return FALSE;

		if (!m_pcsApmObject->AddStreamCallback(APMOBJECT_DATA_OBJECT, ObjectStreamReadCB, ObjectStreamWriteCB, this))
			return FALSE;

		if (!m_pcsApmObject->SetCallbackInitObject(CBInitObject, this))
			return FALSE;
		
		if (!m_pcsApmObject->SetCallbackRemoveObject(CBRemoveObject, this))
			return FALSE;
		//m_pcsApmObject->SetCallbackAddObject( CBOnAddObject , this );
	}

	if (m_pcsAgcmMap)
	{
		if (!m_pcsAgcmMap->SetCallbackLoadMap(CBLoadMap, this))
			return FALSE;

		if (!m_pcsAgcmMap->SetCallbackUnLoadMap(CBUnLoadMap, this))
			return FALSE;

		if (!m_pcsAgcmMap->SetCallbackGetHeight(CBOnGetHeight, this))
			return FALSE;
	}

	if (m_pcsAgcmResourceLoader)
	{
		m_lLoaderTemplateIndex = m_pcsAgcmResourceLoader->RegisterLoader(this, CBLoaderTemplate, CBLoaderInit);
		if (m_lLoaderTemplateIndex < 0)
			return FALSE;

		/*
		m_lLoaderInitIndex = m_pcsAgcmResourceLoader->RegisterLoader(this, NULL, CBLoaderInit);
		if (m_lLoaderInitIndex < 0)
			return FALSE;
		*/
	}

	SetRpWorld(GetWorld());

	if( m_pcsAgcmRender )
	{
		if(!m_pcsAgcmRender->SetCallbackOctreeUDASet(CBOcTreeIDSet,this))
			return FALSE;
	}

	return TRUE;
}

BOOL AgcmObject::OnInit()
{
/*	if (!m_csAnimation.Initialize(m_lMaxAnimNum, AGCDOBJECT_MAX_ALLOC_ANIM_DATA))
	{
		OutputDebugString("AgcmObject::OnInit() Error (1) !!!\n");
		return FALSE;
	}

	if (!m_csAnimation.AttachData(AGCDOBJECT_ANIMATION_ATTACHED_DATA_KEY, sizeof(AgcdObjectAnimationAttachedData)))
	{
		OutputDebugString("AgcmObject::OnInit() Error (2) !!!\n");
		return FALSE;
	}*/

	//m_csAnimation2.SetAnimationPathAndExtension(m_szAnimationPath, AGCD_ANIMATION_DEFAULT_EXT);
	m_csAnimation2.SetAnimationPathAndExtension(
		m_szAnimationPath,
		(m_ulModuleFlags & E_AGCM_OBJECT_FLAGS_EXPORT) ? (AC_EXPORT_EXT_ANIMATION) : (NULL)	);
	m_csAnimation2.SetMaxRtAnim(m_lMaxAnimNum);

	m_CurObject	= NULL; 
	m_CurGroupNode	= NULL;

	AS_REGISTER_TYPE_BEGIN(AgcmObject, AgcmObject);
		AS_REGISTER_METHOD0(void, ShowPickingInfo);
		AS_REGISTER_METHOD0(void, HidePickingInfo);
	AS_REGISTER_TYPE_END;

	return TRUE;
}

BOOL AgcmObject::OnDestroy()
{
	m_csAnimation2.RemoveAllRtAnim();

/*	if(!m_csAnimation.RemoveAll())
	{
		OutputDebugString("AgcmObject::OnDestroy() Error (1) !!!\n");
		return FALSE;
	}*/

/*	if(!m_csObjectList.RemoveAllAnimObjectGroupData())
	{
		OutputDebugString("AgcmObject::OnDestroy() Error (2) !!!\n");
		return FALSE;
	}*/

	return TRUE;
}

BOOL AgcmObject::AddObjectTemplateCreateCallBack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECT_CB_ID_OBJECT_TEMPLATE_CREATE, pfCallback, pClass);
}

BOOL AgcmObject::AddObjectCreateCallBack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECT_CB_ID_OBJECT_CREATE, pfCallback, pClass);
}

AgcdObjectTemplate * AgcmObject::GetTemplateData(ApdObjectTemplate *pstObjectTemplate)
{
	return (AgcdObjectTemplate *) m_pcsApmObject->GetAttachedModuleData(m_nObjectTemplateAttachIndex, pstObjectTemplate);
}

AgcdObject * AgcmObject::GetObjectData(ApdObject *pstObject)
{
	return (AgcdObject *) m_pcsApmObject->GetAttachedModuleData(m_nObjectAttachIndex, pstObject);
}

ApdObject * AgcmObject::GetObject(AgcdObject *pstAgcdObject)
{
	return (ApdObject *) m_pcsApmObject->GetParentModuleData(m_nObjectAttachIndex, pstAgcdObject);
}

RpAtomic *AgcmObject::CBInitTemplateAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	RpClump *	pstClump;

	pstClump = RpAtomicGetClump(pstAtomic);
	if (pstClump)
	{
		if (RpAtomicGetFrame(pstAtomic) == RpClumpGetFrame(pstClump))
		{
			m_pThis->LockFrame();

			RwFrame *	pstFrame = RwFrameCreate();

			RpAtomicSetFrame(pstAtomic, pstFrame);

			RwFrameAddChild(RpClumpGetFrame(pstClump), pstFrame);

			m_pThis->UnlockFrame();
		}
	}

	// LODCache의 renderCallBack이 NULL인 경우가 있어서.. 2005.2.11 gemani
	//RpLODAtomicUnHookRender(pstAtomic);

	return pstAtomic;
}

RpClump * AgcmObject::LoadClump(CHAR *szDFF, CHAR *szTexDict)
{
	RwStream *stream = (RwStream *)NULL;
	RpClump *clump = (RpClump *)NULL;
	RwChar *path = (RwChar *)NULL;
	CHAR	szPath[AGCMOBJECT_MAX_PATH_CLUMP];

	sprintf(szPath, "%s%s", m_szClumpPath, szDFF);

	if (m_pcsAgcmResourceLoader)
	{
		clump = m_pcsAgcmResourceLoader->LoadClump(szPath, NULL, NULL, rwFILTERLINEARMIPLINEAR, m_szTexturePath);
	}

	if (clump)
	{
		RpClumpForAllAtomics(clump, CBInitTemplateAtomic, NULL);
	}

	return clump;
}

BOOL AgcmObject::SaveClump(RpClump *pstClump, CHAR *szDFF)
{
	RwStream *stream = (RwStream *)NULL;
	RpClump *clump = (RpClump *)NULL;
	RwChar *path = (RwChar *)NULL;
	CHAR	szPath[AGCMOBJECT_MAX_PATH_CLUMP];

	sprintf(szPath, "%s%s", m_szClumpPath, szDFF);

	stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, RWSTRING(szPath));
	if( !stream )
		return FALSE;

	if (!RpClumpStreamWrite(pstClump, stream))
		return FALSE;

	RwStreamClose(stream, NULL);

	return TRUE;
}

BOOL AgcmObject::OnObjectCreate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject		*pThis			= (AgcmObject *)(pClass);
	ApdObject		*pstApdObject	= (ApdObject *)(pData);
	AgcdObject		*pstAgcdObject	= pThis->GetObjectData(pstApdObject);

	memset(pstAgcdObject, 0, sizeof(AgcdObject));

	return TRUE;
}

BOOL AgcmObject::OnObjectDestroy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject		*pThis			= (AgcmObject *)(pClass);
	ApdObject		*pstApdObject	= (ApdObject *)(pData);
	AgcdObject		*pstAgcdObject	= pThis->GetObjectData(pstApdObject);

//	pThis->GroupDelete(pstAgcdObject);

	return TRUE;
}

BOOL AgcmObject::OnObjectTemplateCreate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject			*pThis				= (AgcmObject *)(pClass);
	ApdObjectTemplate	*pcsApdTemplate		= (ApdObjectTemplate *)(pData);
	AgcdObjectTemplate	*pcsAgcdTemplate	= (AgcdObjectTemplate *)(pThis->GetTemplateData(pcsApdTemplate));

	memset(pcsAgcdTemplate, 0, sizeof(AgcdObjectTemplate));

	pcsAgcdTemplate->m_eStatus = AGCDOBJECT_STATUS_INIT;

	pcsAgcdTemplate->m_stPreLight.alpha	= 255;
	pcsAgcdTemplate->m_stPreLight.red	= 127;
	pcsAgcdTemplate->m_stPreLight.green	= 127;
	pcsAgcdTemplate->m_stPreLight.blue	= 127;

	pcsAgcdTemplate->m_stOcTreeData.pTopVerts	= NULL					;

	// 마고자 (2005-05-02 오후 4:38:40) : 밝았을때 나오는 사운드 타입.. Ridable에서만 사용.
	pcsAgcdTemplate->m_eRidableMaterialType		= APMMAP_MATERIAL_SOIL	;

//	pcsAgcdTemplate->m_fAnimSpeed		= 1.0f;

//	AgcdTemplate->m_stLODData.m_bHasLOD = FALSE;
//	memset(AgcdTemplate->m_stLODData.m_aulLODDistance, 0, sizeof(UINT32) * AGPDLOD_MAX_NUM);

	return TRUE;
}

BOOL AgcmObject::OnObjectTemplateDestroy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject *pThis = (AgcmObject *) pClass;
	ApdObjectTemplate *pstApdObjectTemplate = (ApdObjectTemplate *) pData;
	AgcdObjectTemplate *pstAgcdObjectTemplate = pThis->GetTemplateData(pstApdObjectTemplate);

//	pThis->m_pcsAgcmRender->m_csRenderType.DeleteClumpRenderType(&pstAgcdObjectTemplate->m_csClumpRenderType);

	// Octree MinBox 동적 할당 해제 2005.2.18 gemani 
	if(pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts)
	{
		delete	[]pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts;
		pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts = NULL;
	}

	if(!pThis->ReleaseObjectTemplateData(pstAgcdObjectTemplate))
		return FALSE;

	if(pstAgcdObjectTemplate->m_stLOD.m_pstList)
	{
		if (pThis->m_pcsAgcmLODManager)
			pThis->m_pcsAgcmLODManager->m_csLODList.RemoveAllLODData(&pstAgcdObjectTemplate->m_stLOD);

		pstAgcdObjectTemplate->m_stLOD.m_pstList = NULL;
	}

/*	INT32						lIndex;
	AgcdObjectTemplateGroupList *pstList = pstAgcdObjectTemplate->m_stGroup.m_pstList;
	AgcdObjectTemplateGroupData *pstData;

	while(pstList)
	{
		pstData = &pstList->m_csData;
		for (lIndex = 0; lIndex < E_OBJECT_ANIM_TYPE_NUM; ++lIndex)
		{
			if (pstData->m_aszAnimName[lIndex])
			{
				delete [] pstData->m_aszAnimName[lIndex];
				pstData->m_aszAnimName[lIndex] = NULL;
			}
		}

		pstList = pstList->m_pstNext;
	}*/

/*	AgcdObjectTemplateGroupList *pstList = pstAgcdObjectTemplate->m_stGroup.m_pstList;
	while (pstList)
	{
		if (pstList->m_csData.m_pszDFFName)
			delete [] pstList->m_csData.m_pszDFFName;

		if (pstList->m_csData.m_pszAnimName)
			delete [] pstList->m_csData.m_pszAnimName;

		pstList = pstList->m_pstNext;
	}*/

	pThis->RemoveObjectTemplateGroup(&pstAgcdObjectTemplate->m_stGroup);
	pThis->m_csObjectList.RemoveAllObjectTemplateGroup(&pstAgcdObjectTemplate->m_stGroup);

	return TRUE;
}

AgcdObjectTemplateGroupData	*AgcmObject::GetObjectTemplateGroupData(AgcdObjectTemplateGroup *pstGroup, INT32 lIndex, BOOL bAdd)
{
	AgcdObjectTemplateGroupData *pstGroupData = m_csObjectList.GetObjectTemplateGroup(pstGroup, lIndex);
	if ((!pstGroupData) && (bAdd))
		pstGroupData = m_csObjectList.AddObjectTemplateGroup(pstGroup);

	return pstGroupData;
}

INT32 AgcmObject::ObjectTemplateGroupStreamRead(ApModuleStream *pcsStream, INT32 lTID, AgcdObjectTemplateGroup *pstGroup)
{
	AgcdObjectTemplateGroupData	*pstGroupData;
	INT32						lIndex;
	CHAR						szValue[256];
	INT32						lTemp1, lTemp2, lTemp3;
	CHAR						szTemp[256];
	FLOAT						fTemp;
	FLOAT						fTemp1, fTemp2, fTemp3, fTemp4;

	if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_DFF))
	{
		szTemp[0] = NULL;

		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%s", &lIndex, szTemp);

		if (strcmp(szTemp, ""))
		{
			pstGroupData = GetObjectTemplateGroupData(pstGroup, lIndex);
			if (!pstGroupData)
				return E_OTGSR_RESULT_ERROR;

			pstGroupData->m_pszDFFName	= new CHAR [strlen(szTemp) + 1];
			strcpy(pstGroupData->m_pszDFFName, szTemp);
		}

//		strcpy(pstGroupData->m_szDFFName, szTemp);

		return E_OTGSR_RESULT_READ;
	}
	else if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_ANIMATION))
	{
		szTemp[0] = NULL;

		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%d:%d:%d:%s", &lIndex, &lTemp1, &lTemp2, &lTemp3, szTemp);

		if (strcmp(szTemp, ""))
		{
			pstGroupData = GetObjectTemplateGroupData(pstGroup, lIndex);
			if (!pstGroupData)
				return E_OTGSR_RESULT_ERROR;

			if (!m_csAnimation2.AddAnimation(&pstGroupData->m_pcsAnimation, szTemp))
			{
				ASSERT(!"Failed - m_csAnimation2.AddAnimation()");
				return FALSE;
			}
		}

		//		pstGroupData->m_pszAnimName	= new CHAR [strlen(szTemp) + 1];
	//		strcpy(pstGroupData->m_pszAnimName, szTemp);

/*		pstGroupData->m_aszAnimName[lTemp1] = new CHAR[strlen(szTemp) + 1];
		strcpy(pstGroupData->m_aszAnimName[lTemp1], szTemp);
		pstGroupData->m_aszAnimName[lTemp1][strlen(szTemp)] = 0;

		if (!pstGroupData->m_pastAnim[lTemp1])
		{
			pstGroupData->m_pastAnim[lTemp1] = GetAnimation(lTID, lIndex, lTemp1, TRUE);
			if (!pstGroupData->m_pastAnim[lTemp1])
				return E_OTGSR_RESULT_ERROR;
		}

		AgcdObjectAnimationAttachedData	*pstObjAnimAttachedData	=
			(AgcdObjectAnimationAttachedData *)(m_csAnimation.GetAttachedData(
			pstGroupData->m_pastAnim[lTemp1],
			AGCDOBJECT_ANIMATION_ATTACHED_DATA_KEY								)	);

		if (pstObjAnimAttachedData)
		{
			pstObjAnimAttachedData->m_stAnimFlag.m_ulAnimFlag	= lTemp2;
			pstObjAnimAttachedData->m_stAnimFlag.m_ulPreference	= lTemp3;
		}
		else
		{
			ASSERT(!"AgcmCharacter::ObjectTemplateGroupStreamRead() m_csAnimation.GetAttachedData() Failed !!!");
		}*/

/*		pstGroupData->m_pastAnim[lTemp1]->m_stAnimFlag.m_ulAnimFlag		= lTemp2;
		pstGroupData->m_pastAnim[lTemp1]->m_stAnimFlag.m_ulPreference	= lTemp3;*/

		return E_OTGSR_RESULT_READ;
	}
	else if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_ANIM_SPEED))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%d:%f", &lIndex, &lTemp1, &fTemp);

		pstGroupData = GetObjectTemplateGroupData(pstGroup, lIndex);
		if (!pstGroupData)
			return E_OTGSR_RESULT_ERROR;

		pstGroupData->m_fAnimSpeed	= fTemp;
//		pstGroupData->m_afAnimSpeed[lTemp1] = fTemp;

		return E_OTGSR_RESULT_READ;
	}
	else if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_BSPHERE))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%f:%f:%f:%f", &lIndex, &fTemp1, &fTemp2, &fTemp3, &fTemp4);

		pstGroupData = GetObjectTemplateGroupData(pstGroup, lIndex);
		if (!pstGroupData)
			return E_OTGSR_RESULT_ERROR;

		pstGroupData->m_stBSphere.center.x	= fTemp1;
		pstGroupData->m_stBSphere.center.y	= fTemp2;
		pstGroupData->m_stBSphere.center.z	= fTemp3;
		pstGroupData->m_stBSphere.radius	= fTemp4;

		return E_OTGSR_RESULT_READ;
	}
	else if (!strncmp(pcsStream->GetValueName(), AGCM_CLUMP_RENDER_TYPE_STREAM_CUSTOM_DATA1, strlen(AGCM_CLUMP_RENDER_TYPE_STREAM_CUSTOM_DATA1)))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d", &lIndex);

		pstGroupData = GetObjectTemplateGroupData(pstGroup, lIndex);
		if (!pstGroupData)
			return E_OTGSR_RESULT_ERROR;

		while (pcsStream->ReadNextValue())
		{
			if (!strncmp(pcsStream->GetValueName(), AGCM_CLUMP_RENDER_TYPE_STREAM_CUSTOM_DATA2, strlen(AGCM_CLUMP_RENDER_TYPE_STREAM_CUSTOM_DATA2)))
				return E_OTGSR_RESULT_READ;

			if (E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_READ != m_pcsAgcmRender->StreamReadClumpRenderType(pcsStream, &pstGroupData->m_csClumpRenderType))
				return E_OTGSR_RESULT_ERROR;
		}
	}

	return E_OTGSR_RESULT_PASS;
}

BOOL AgcmObject::ObjectTemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	AgcmObject			*pThis					= (AgcmObject *)(pClass);
	ApdObjectTemplate	*pstApdObjectTemplate	= (ApdObjectTemplate *)(pData);
	AgcdObjectTemplate	*pstAgcdObjectTemplate	= (AgcdObjectTemplate *)(pThis->GetTemplateData(pstApdObjectTemplate));

	CHAR	szValue[AGCMOBJECT_MAX_STR];
	INT32	lTemp1, lTemp2, lTemp3, lTemp4, lRt;
	FLOAT	afTemp[12];

	INT32	iOccluderIndex;

	//@{ 2006/05/04 burumal
	pstAgcdObjectTemplate->m_nDNF = 0;
	//@}
		
	while (pcsStream->ReadNextValue())
	{
		lRt = pThis->ObjectTemplateGroupStreamRead(pcsStream, pstApdObjectTemplate->m_lID, &pstAgcdObjectTemplate->m_stGroup);
		if (lRt == E_OTGSR_RESULT_READ)
			continue;
		else if (lRt == E_OTGSR_RESULT_ERROR)
			return FALSE;

		if (pThis->m_pcsAgcmLODManager)
		{
			lRt = pThis->m_pcsAgcmLODManager->StreamRead(pcsStream, &pstAgcdObjectTemplate->m_stLOD);
			if (lRt == E_AGCD_LOD_STREAM_READ_RESULT_READ)
				continue;
			else if (lRt == E_AGCD_LOD_STREAM_READ_RESULT_ERROR)
				return FALSE;
		}

		if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_CATEGORY))
		{
			pcsStream->GetValue(pstAgcdObjectTemplate->m_szCategory, AGCDOBJECT_CATEGORY_LENGTH);
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_OBJECT_TYPE))
		{
			pcsStream->GetValue(&pstAgcdObjectTemplate->m_lObjectType);

			// 죽는다.-_-; 제대로 작동 안한다.... 넣지 말것... 확실하게 되면 컨펌 받을것.(Parn)
			//pstAgcdObjectTemplate->m_lObjectType &= ~ACUOBJECT_TYPE_USE_FADE_IN_OUT;
			//pstAgcdObjectTemplate->m_lObjectType |= ACUOBJECT_TYPE_USE_FADE_IN_OUT;			

			iOccluderIndex = 0;
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_COLLISION_DFF))
		{
			pcsStream->GetValue(pstAgcdObjectTemplate->m_szCollisionDFFName, AGCDOBJECT_DFF_NAME_LENGTH);
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_PICK_DFF))
		{
			pcsStream->GetValue(pstAgcdObjectTemplate->m_szPickDFFName, AGCDOBJECT_DFF_NAME_LENGTH);
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_OCTREEDATANUM))
		{
			pcsStream->GetValue(szValue, AGCMOBJECT_MAX_STR);
			sscanf(szValue,"%d",&lTemp1);

			pstAgcdObjectTemplate->m_stOcTreeData.nOccluderBox = lTemp1;
			if ( pstAgcdObjectTemplate->m_stOcTreeData.nOccluderBox > 0 )
				pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts = new RwV3d[lTemp1 * 4];
		}
		else if (!strncmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_OCTREEDATA,strlen(AGCMOBJECT_INI_NAME_OCTREEDATA)))
		{
			pcsStream->GetValue(szValue, AGCMOBJECT_MAX_STR);
			sscanf(szValue,
				"%d:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
				&lTemp1,
				afTemp + 0,	afTemp + 1,	afTemp + 2,	afTemp + 3,
				afTemp + 4,	afTemp + 5,	afTemp + 6,	afTemp + 7,
				afTemp + 8,	afTemp + 9,	afTemp + 10,afTemp + 11	);

			INT32	iTempIndex = iOccluderIndex*4;

			pstAgcdObjectTemplate->m_stOcTreeData.isOccluder	= lTemp1;
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].x	= afTemp[0];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].y	= afTemp[1];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex++].z	= afTemp[2];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].x	= afTemp[3];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].y	= afTemp[4];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex++].z	= afTemp[5];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].x	= afTemp[6];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].y	= afTemp[7];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex++].z	= afTemp[8];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].x	= afTemp[9];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].y	= afTemp[10];
			pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].z	= afTemp[11];

			++iOccluderIndex;
		}
		else if(!strncmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_OCTREEDATA_MAXBBOX,strlen(AGCMOBJECT_INI_NAME_OCTREEDATA_MAXBBOX)))
		{
			pcsStream->GetValue(szValue, AGCMOBJECT_MAX_STR);
			sscanf(szValue,
				"%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
				afTemp + 0,	afTemp + 1,	afTemp + 2,	afTemp + 3,
				afTemp + 4,	afTemp + 5,	afTemp + 6,	afTemp + 7,
				afTemp + 8,	afTemp + 9,	afTemp + 10,afTemp + 11);

			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[0].x	= afTemp[0];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[0].y	= afTemp[1];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[0].z	= afTemp[2];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[1].x	= afTemp[3];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[1].y	= afTemp[4];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[1].z	= afTemp[5];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[2].x	= afTemp[6];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[2].y	= afTemp[7];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[2].z	= afTemp[8];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[3].x	= afTemp[9];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[3].y	= afTemp[10];
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[3].z	= afTemp[11];
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_PRE_LIGHT))
		{
			pcsStream->GetValue(szValue, AGCMOBJECT_MAX_STR);
			sscanf(szValue, "%d %d %d %d", &lTemp1, &lTemp2, &lTemp3, &lTemp4);

			pstAgcdObjectTemplate->m_stPreLight.red		= lTemp1;
			pstAgcdObjectTemplate->m_stPreLight.green	= lTemp2;
			pstAgcdObjectTemplate->m_stPreLight.blue	= lTemp3;
			pstAgcdObjectTemplate->m_stPreLight.alpha	= lTemp4;
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_RIDABLEMATERIALTYPE))
		{
			// 마고자 (2005-05-02 오후 4:43:11) : 
			// Ridable 오브젝트 타입 저장.	
			INT32	nRidableMaterialType;
			pcsStream->GetValue( &nRidableMaterialType );
			pstAgcdObjectTemplate->m_eRidableMaterialType = ( eApmMapMaterial ) nRidableMaterialType;
		}
		//@{ 2006/05/03 burumal
		else if ( !strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_DNF_1) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 1;

			if ( nDNF > 0 )
				pstAgcdObjectTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_DNF_2) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 2;

			if ( nDNF > 0 )
				pstAgcdObjectTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_DNF_3) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 3;

			if ( nDNF > 0 )
				pstAgcdObjectTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(pcsStream->GetValueName(), AGCMOBJECT_INI_NAME_DNF_4) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 4;

			if ( nDNF > 0 )
				pstAgcdObjectTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		//@}
	}

	pThis->EnumCallback(AGCMOBJECT_CB_ID_OBJECT_TEMPLATE_CREATE, pstApdObjectTemplate, NULL);

	return TRUE;
}

BOOL AgcmObject::ObjectTemplateGroupStreamWrite(ApModuleStream *pcsStream, AgcdObjectTemplate *pcsAgcdObjectTemplate)
{
	CHAR							szValueName[256], szValue[256];

	AgcdObjectTemplateGroupData	*pstData;
	for (INT32 lIndex = 0;; ++lIndex)
	{
		pstData = m_csObjectList.GetObjectTemplateGroup(&pcsAgcdObjectTemplate->m_stGroup, lIndex);		
		if (!pstData)
			break;

		if (pstData->m_pszDFFName)
		{
			sprintf(szValue, "%d:%s", pstData->m_lIndex, pstData->m_pszDFFName);
			pcsStream->WriteValue(AGCMOBJECT_INI_NAME_DFF, szValue);
		}

/*		if (pstData->m_pszAnimName)
		{
			sprintf(szValue, "%d:%d:%d:%d:%s",	pstData->m_lIndex, 0, 0, 0, pstData->m_pszAnimName);
			pcsStream->WriteValue(AGCMOBJECT_INI_NAME_ANIMATION, szValue);
		}*/
		if (	(pstData->m_pcsAnimation) &&
				(pstData->m_pcsAnimation->m_pcsHead) &&
				(pstData->m_pcsAnimation->m_pcsHead->m_pszRtAnimName)	)
		{
			sprintf(
				szValue,
				"%d:%d:%d:%d:%s",
				pstData->m_lIndex,
				0,
				0,
				0,
				pstData->m_pcsAnimation->m_pcsHead->m_pszRtAnimName	);

			pcsStream->WriteValue(AGCMOBJECT_INI_NAME_ANIMATION, szValue);

			sprintf(
				szValue,
				"%d:%d:%f",
				pstData->m_lIndex,
				0,
				pstData->m_fAnimSpeed	);
			pcsStream->WriteValue(AGCMOBJECT_INI_NAME_ANIM_SPEED, szValue);
		}

		// DFF Name
/*		sprintf(szValue, "%d:%s",	pstData->m_lIndex,
									pstData->m_szDFFName	);
		pcsStream->WriteValue(AGCMOBJECT_INI_NAME_DFF, szValue);*/
		// Animation
/*		for (INT32 lAnimType = 0; lAnimType < E_OBJECT_ANIM_TYPE_NUM; ++lAnimType)
		{
			AgcdObjectAnimationAttachedData	*pstObjAnimAttachedData	=
				(AgcdObjectAnimationAttachedData *)(m_csAnimation.GetAttachedData(
				pstData->m_pastAnim[lAnimType],
				AGCDOBJECT_ANIMATION_ATTACHED_DATA_KEY								)	);

			if (	(pstData->m_pastAnim[lAnimType]) &&
					(pstData->m_aszAnimName[lAnimType])	)
			{
				sprintf(szValue, "%d:%d:%d:%d:%s",	pstData->m_lIndex,
													lAnimType,
													(pstObjAnimAttachedData) ?
													(pstObjAnimAttachedData->m_stAnimFlag.m_ulAnimFlag) :
													(0),
													(pstObjAnimAttachedData) ?
													(pstObjAnimAttachedData->m_stAnimFlag.m_ulPreference) :
													(0),
//														pstData->m_pastAnim[lAnimType]->m_stAnimFlag.m_ulAnimFlag,
//														pstData->m_pastAnim[lAnimType]->m_stAnimFlag.m_ulPreference,
													pstData->m_aszAnimName[lAnimType]								);
				pcsStream->WriteValue(AGCMOBJECT_INI_NAME_ANIMATION, szValue);

				sprintf(szValue, "%d:%d:%f",	pstData->m_lIndex,
												lAnimType,
												pstData->m_afAnimSpeed[lAnimType]	);
				pcsStream->WriteValue(AGCMOBJECT_INI_NAME_ANIM_SPEED, szValue);
			}
		}*/

		// B-Sphere
		print_compact_format(szValue, "%d:%f:%f:%f:%f",	pstData->m_lIndex,
											pstData->m_stBSphere.center.x,
											pstData->m_stBSphere.center.y,
											pstData->m_stBSphere.center.z,
											pstData->m_stBSphere.radius		);

		pcsStream->WriteValue(AGCMOBJECT_INI_NAME_BSPHERE, szValue);

		if ((pstData->m_csClumpRenderType.m_lSetCount > 0))
		{
			// ClumpRenderType의 stream 부분의 시작을 표시! - group의 특성 때문에 필요하다.
			sprintf(szValueName, "%s%d", AGCM_CLUMP_RENDER_TYPE_STREAM_CUSTOM_DATA1, lIndex);
			sprintf(szValue, "%d", pstData->m_lIndex);
			pcsStream->WriteValue(szValueName, szValue);

			m_pcsAgcmRender->StreamWriteClumpRenderType(pcsStream, &pstData->m_csClumpRenderType);

			// ClumpRenderType의 stream 부분의 끝을 표시!
			sprintf(szValueName, "%s%d", AGCM_CLUMP_RENDER_TYPE_STREAM_CUSTOM_DATA2, pstData->m_lIndex);
			sprintf(szValue, "%d", lIndex);
			pcsStream->WriteValue(szValueName, szValue);
		}
	}

	return TRUE;
}

BOOL AgcmObject::ObjectTemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	AgcmObject			*pThis					= (AgcmObject *)(pClass);
	ApdObjectTemplate	*pstApdObjectTemplate	= (ApdObjectTemplate *)(pData);
	AgcdObjectTemplate	*pstAgcdObjectTemplate	= (AgcdObjectTemplate *)(pThis->GetTemplateData(pstApdObjectTemplate));

	CHAR	szValueName[AGCMOBJECT_MAX_STR];
	CHAR	szValue[AGCMOBJECT_MAX_STR];

	if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_CATEGORY, pstAgcdObjectTemplate->m_szCategory))
		return FALSE;

	if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_OBJECT_TYPE, pstAgcdObjectTemplate->m_lObjectType))
		return FALSE;

	if (pstAgcdObjectTemplate->m_szCollisionDFFName[0])
	{
		if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_COLLISION_DFF, pstAgcdObjectTemplate->m_szCollisionDFFName))
			return FALSE;
	}

	if (pstAgcdObjectTemplate->m_szPickDFFName[0])
	{
		if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_PICK_DFF, pstAgcdObjectTemplate->m_szPickDFFName))
			return FALSE;
	}

	if (pstAgcdObjectTemplate->m_szPickDFFName[0])
	{
		if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_PICK_DFF, pstAgcdObjectTemplate->m_szPickDFFName))
			return FALSE;
	}
	
	{
		// 마고자 (2005-05-02 오후 4:43:11) : 
		// Ridable 오브젝트 타입 저장.
		INT32	nRidableMaterialType = ( UINT32 ) pstAgcdObjectTemplate->m_eRidableMaterialType;
		if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_RIDABLEMATERIALTYPE, nRidableMaterialType ))
			return FALSE;
	}

	// OcTree 관련
	{
		if(pstAgcdObjectTemplate->m_lObjectType & ACUOBJECT_TYPE_OCCLUDER)
		{
			// 임시로 octreenum 구하자
/*			INT32 nMinBox = 0;
			{
				for(int i=0;i<pstApdObjectTemplate->m_nBlockInfo;++i)
				{
					if(pstApdObjectTemplate->m_astBlockInfo[i].type == AUBLOCKING_TYPE_MINBOX)
					{
						++nMinBox;
					}
				}
			}
			pstAgcdObjectTemplate->m_stOcTreeData.nOccluderBox = nMinBox;*/

			sprintf(szValue,
				"%d",pstAgcdObjectTemplate->m_stOcTreeData.nOccluderBox); 

			if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_OCTREEDATANUM, szValue))
			return FALSE;
				
			for(int i=0;i<pstAgcdObjectTemplate->m_stOcTreeData.nOccluderBox;++i)
			{
				INT32	iTempIndex = i*4;
				print_compact_format(szValue,
					"%d:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
					(pstAgcdObjectTemplate->m_lObjectType & ACUOBJECT_TYPE_OCCLUDER)?1:0,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].x,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].y,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex].z,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex+1].x,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex+1].y,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex+1].z,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex+2].x,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex+2].y,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex+2].z,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex+3].x,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex+3].y,
					pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts[iTempIndex+3].z);

				sprintf(szValueName, "%s%d", AGCMOBJECT_INI_NAME_OCTREEDATA, i);
				if (!pcsStream->WriteValue(szValueName, szValue))
				return FALSE;
			}
		}
/*		else
		{
			sprintf(szValue,
				"%d",pstAgcdObjectTemplate->m_stOcTreeData.nOccluderBox); 

			if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_OCTREEDATANUM, szValue))
			return FALSE;

			int i=0;
			sprintf(szValue,
				"%d:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
				(pstAgcdObjectTemplate->m_lObjectType & ACUOBJECT_TYPE_OCCLUDER)?1:0,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][0].x,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][0].y,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][0].z,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][1].x,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][1].y,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][1].z,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][2].x,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][2].y,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][2].z,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][3].x,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][3].y,
				pstAgcdObjectTemplate->m_stOcTreeData.topVerts[i][3].z);

			sprintf(szValueName, "%s%d", AGCMOBJECT_INI_NAME_OCTREEDATA, i);
			if (!pcsStream->WriteValue(szValueName, szValue))
			return FALSE;
		}*/

		if( pThis->IsInValidOctreeData( pstAgcdObjectTemplate ) )
		{
			pThis->ReCalulateOctreeData( "Object", pstAgcdObjectTemplate );
		}

		// 최대 BBOX저장
		print_compact_format(szValue,
			"%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[0].x,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[0].y,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[0].z,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[1].x,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[1].y,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[1].z,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[2].x,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[2].y,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[2].z,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[3].x,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[3].y,
			pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX[3].z					);

		if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_OCTREEDATA_MAXBBOX, szValue))
			return FALSE;
	}

	// Group에 포함된 것덜을 write한당!
	pThis->ObjectTemplateGroupStreamWrite(pcsStream, pstAgcdObjectTemplate);

	// LOD에 관계된 것덜을 WRITE한당!
	if (pThis->m_pcsAgcmLODManager)
		pThis->m_pcsAgcmLODManager->StreamWrite(pcsStream, &pstAgcdObjectTemplate->m_stLOD);

	//////////////////////////////////
	// pre-light
	if (pstAgcdObjectTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)
	{
		sprintf(szValue, "%d %d %d %d",	pstAgcdObjectTemplate->m_stPreLight.red,
											pstAgcdObjectTemplate->m_stPreLight.green,
											pstAgcdObjectTemplate->m_stPreLight.blue,
											pstAgcdObjectTemplate->m_stPreLight.alpha	);

		if (!pcsStream->WriteValue(AGCMOBJECT_INI_NAME_PRE_LIGHT, szValue))
		{
			OutputDebugString("AgcmObject::ObjectTemplateStreamWriteCB() Error (5) !!!\n");
			return FALSE;
		}
	}

	//@{ 2006/05/04 burumal	
	pcsStream->WriteValue(AGCMOBJECT_INI_NAME_DNF_1, (pstAgcdObjectTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_KOREA		)) ? 1 : 0);
	pcsStream->WriteValue(AGCMOBJECT_INI_NAME_DNF_2, (pstAgcdObjectTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_CHINA		)) ? 1 : 0);
	pcsStream->WriteValue(AGCMOBJECT_INI_NAME_DNF_3, (pstAgcdObjectTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_WESTERN	)) ? 1 : 0);
	pcsStream->WriteValue(AGCMOBJECT_INI_NAME_DNF_4, (pstAgcdObjectTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_JAPAN		)) ? 1 : 0);
	//@}
	
	return TRUE;
}

BOOL AgcmObject::ObjectStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgcmObject	*pThis = (AgcmObject *) pClass;
	ApdObject	*pstApdObject = (ApdObject *) pData;
	AgcdObject	*pstAgcdObject = pThis->GetObjectData((ApdObject *) pData);

	pThis->EnumCallback(AGCMOBJECT_CB_ID_OBJECT_CREATE, pstApdObject, NULL);

	const CHAR *szValueName;
	//CHAR szValue[128];
	INT32	nGroupCount;

	AuPOS pos,scale;
	FLOAT xrot,yrot;

	while(pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGCMOBJECT_INI_NAME_GROUP_COUNT))
		{
			pStream->GetValue ((INT32 *) &nGroupCount);
			/*
			pStream->ReadNextValue();

			for(INT32 i=0;i<nGroupCount;++i)
			{
				pStream->GetValue(szValue, 128);
				pThis->m_pcsApmObject->ParsePOS(szValue, &scale);
				pStream->ReadNextValue();
			
				pStream->GetValue(szValue, 128);
				pThis->m_pcsApmObject->ParsePOS(szValue, &pos);
				pStream->ReadNextValue();
		
				pStream->GetValue(&xrot );
				pStream->ReadNextValue();
				pStream->GetValue(&yrot );
				pStream->ReadNextValue();

				pThis->GroupAdd(pstAgcdObject,pos,scale,xrot,yrot, FALSE);
			}
			*/
		}
		else if (!strcmp(szValueName, AGCMOBJECT_INI_NAME_OBJECT_TYPE))
		{
			pStream->GetValue(&pstAgcdObject->m_lObjectType);
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_SCALE))
		{
			pStream->GetValue(&scale);
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_POSITION))
		{
			pStream->GetValue(&pos);
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_DEGREE_X))
		{
			pStream->GetValue(&xrot);
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_DEGREE_Y))
		{
			pStream->GetValue(&yrot);

			pThis->GroupAdd(pstAgcdObject, pos, scale, xrot, yrot, FALSE);
		}
	}

// 마고자 (2003-02-04 오전 7:41:49) : 
	return TRUE; // pThis->SetupObjectClump(pstApdObject);
}

BOOL AgcmObject::ObjectStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgcmObject	*pThis = (AgcmObject *) pClass;
	AgcdObject	*pstAgcdObject = pThis->GetObjectData((ApdObject *) pData);
	
	INT32		nGroupCount;
	nGroupCount = pThis->GetGroupCount(pstAgcdObject);

	CHAR		szTemp[128];
	sprintf(szTemp, "%d", nGroupCount);
	if (!pStream->WriteValue(AGCMOBJECT_INI_NAME_GROUP_COUNT,szTemp))
		return FALSE;

	if (!pStream->WriteValue(AGCMOBJECT_INI_NAME_OBJECT_TYPE,pstAgcdObject->m_lObjectType))
		return FALSE;

	/* 풀처리는 막겠음 (Parn)
	if(nGroupCount > 0)
	{
		AgcdObject *curGroupNode = pstAgcdObject->m_listNext;

		while(curGroupNode)
		{
			sprintf(szTemp, "%f,%f,%f", curGroupNode->m_pInfoGroup->m_stScale.x, curGroupNode->m_pInfoGroup->m_stScale.y, curGroupNode->m_pInfoGroup->m_stScale.z);
			if (!pStream->WriteValue(APMOBJECT_INI_NAME_SCALE, szTemp))
				return FALSE;

			sprintf(szTemp, "%f,%f,%f", curGroupNode->m_pInfoGroup->m_stPosition.x, curGroupNode->m_pInfoGroup->m_stPosition.y, curGroupNode->m_pInfoGroup->m_stPosition.z);
			if (!pStream->WriteValue(APMOBJECT_INI_NAME_POSITION, szTemp))
				return FALSE;
	
			if (!pStream->WriteValue(APMOBJECT_INI_NAME_DEGREE_X, curGroupNode->m_pInfoGroup->m_fDegreeX))
				return FALSE;
			if (!pStream->WriteValue(APMOBJECT_INI_NAME_DEGREE_Y, curGroupNode->m_pInfoGroup->m_fDegreeY))
				return FALSE;

			curGroupNode = curGroupNode->m_listNext;
		}
	}
	*/

	return TRUE;
}

BOOL AgcmObject::SetClumpPath(CHAR *szPath)
{
	strncpy(m_szClumpPath, szPath, AGCMOBJECT_MAX_PATH_CLUMP);
	m_szClumpPath[AGCMOBJECT_MAX_PATH_CLUMP - 1] = '\0';

	return TRUE;
}

VOID AgcmObject::SetAnimationPath(CHAR *szAnim)
{
	strcpy(m_szAnimationPath, szAnim);
}

BOOL AgcmObject::SetRpWorld(RpWorld *pWorld)
{
	m_pWorld = pWorld;

	return TRUE;
}

void AgcmObject::SetTexDictFile(CHAR *szTexDict)
{
	m_bUseTexDict = TRUE;

	strncpy(m_szTexDict, szTexDict, AGCMOBJECT_MAX_PATH_CLUMP);
	m_szTexDict[AGCMOBJECT_MAX_PATH_CLUMP - 1] = '\0';
}

void AgcmObject::SetTexturePath(CHAR *szTexturePath)
{
	strncpy(m_szTexturePath, szTexturePath, AGCMOBJECT_MAX_PATH_CLUMP);
	m_szTexturePath[AGCMOBJECT_MAX_PATH_CLUMP - 1] = '\0';
}

/******************************************************************************
* Purpose :
*
* 291003. BOB
******************************************************************************/
AgcdObjectGroupData *AgcmObject::GetObjectGroupData(AgcdObjectGroup *pstGroup, INT32 lIndex, BOOL bAdd)
{
	AgcdObjectGroupData *pstData = m_csObjectList.GetObjectGroup(pstGroup, lIndex);
	if ((!pstData) && (bAdd))
	{
/*		AgcdObjectGroupData stData;
		
		memset(&stData, 0, sizeof(stData));

		stData.m_lIndex = lIndex;*/

		pstData = m_csObjectList.AddObjectGroup(pstGroup/*, &stData*/);
	}

	return pstData;
}

/******************************************************************************
* Purpose :
*
* 041703. Bob Jung
******************************************************************************/
BOOL AgcmObject::SetupObjectClump(ApdObject *pstApdObject)
{
	AgcdObject *pstAgcdObject = GetObjectData(pstApdObject);
	if(!pstAgcdObject)
	{
		OutputDebugString("AgcmObject::SetupObjectClump() Error (1) !!!\n");
		return FALSE;
	}

	return SetupObjectClump(pstApdObject, pstAgcdObject);
}
BOOL	AgcmObject::SetupObjectClump_TransformOnly(ApdObject *pstApdObject)
{
	AgcdObject *pstAgcdObject = GetObjectData(pstApdObject);
	if(!pstAgcdObject)
	{
		OutputDebugString("AgcmObject::SetupObjectClump() Error (1) !!!\n");
		return FALSE;
	}

	ApdObjectTemplate	*	pstApdObjectTemplate	;
	AgcdObjectTemplate	*	pstAgcdObjectTemplate	;
	RwFrame				*	pFrame					;
	RwV3d					stScale					;
	RwV3d					stPosition				;

	static	RwV3d	AxisX	= { 1.0f , 0.0f , 0.0f };
	static	RwV3d	AxisY	= { 0.0f , 1.0f , 0.0f };

	pstApdObjectTemplate = m_pcsApmObject->GetObjectTemplate(pstApdObject->m_lTID);
	if (!pstApdObjectTemplate)
		return FALSE;

	pstAgcdObjectTemplate = GetTemplateData(pstApdObjectTemplate);

	LoadTemplateData( pstAgcdObjectTemplate );

//	AgcdObjectTemplateGroupList	*pstTGroupList = pstAgcdObjectTemplate->m_stGroup.m_pstList;
	AgcdObjectTemplateGroupData	*pstTGroupData;
	AgcdObjectGroupData			*pstOGroupData;

//	while(pstTGroupList)
	for(INT32 lGroupIndex = 0;; ++lGroupIndex)
	{
//		pstTGroupData = &pstTGroupList->m_csData;
		pstTGroupData = m_csObjectList.GetObjectTemplateGroup(&pstAgcdObjectTemplate->m_stGroup, lGroupIndex);
		if(!pstTGroupData)
			break;

		pstOGroupData = GetObjectGroupData(&pstAgcdObject->m_stGroup, pstTGroupData->m_lIndex);
		if(!pstOGroupData)
			return FALSE;

		if(pstTGroupData->m_pstClump)
		{
			bool bAdd = false;
			if(!pstOGroupData->m_pstClump)
			{
				LockFrame();

				pstOGroupData->m_pstClump = RpClumpClone(pstTGroupData->m_pstClump);

				UnlockFrame();

				if(!pstOGroupData->m_pstClump)
				{
					return FALSE;
				}

				pstOGroupData->m_pstClump->pvApBase = pstApdObject;
#ifdef _DEBUG
				//@{ Jaewon 20050905
				// ;)
				//pstOGroupData->m_pstClump->szName = pstApdObject->m_pcsTemplate->m_szName;
				RpClumpSetName(pstOGroupData->m_pstClump, pstApdObject->m_pcsTemplate->m_szName);
				//@} Jaewon
#endif

				pstOGroupData->m_pstInHierarchy = AgcuObject_GetHierarchy(pstOGroupData->m_pstClump);
				if(pstOGroupData->m_pstInHierarchy)
				{
					RpHAnimHierarchySetFlags(pstOGroupData->m_pstInHierarchy,
						(RpHAnimHierarchyFlag)
						( RpHAnimHierarchyGetFlags(pstOGroupData->m_pstInHierarchy) |
						rpHANIMHIERARCHYUPDATELTMS |
						rpHANIMHIERARCHYUPDATEMODELLINGMATRICES | rpHANIMHIERARCHYLOCALSPACEMATRICES) );
					
//					RpClumpForAllAtomics(pstOGroupData->m_pstClump, AgcuObject_SetHierarchyForSkinAtomic, (void *) pstOGroupData->m_pstInHierarchy);
					
/*					RpHAnimHierarchyFlag flags = (RpHAnimHierarchyFlag) (pstOGroupData->m_pstInHierarchy)->flags;

					pstOGroupData->m_pstOutHierarchy = RpHAnimHierarchyCreateFromHierarchy(pstOGroupData->m_pstInHierarchy, flags, AGCMOBJECT_HIERARCHY_MAX_KEYFRAME);
					pstOGroupData->m_pstInHierarchy2 = RpHAnimHierarchyCreateFromHierarchy(pstOGroupData->m_pstInHierarchy, flags, AGCMOBJECT_HIERARCHY_MAX_KEYFRAME);
					
					pstOGroupData->m_pstOutHierarchy->parentFrame = pstOGroupData->m_pstInHierarchy->parentFrame;
					pstOGroupData->m_pstInHierarchy2->parentFrame = pstOGroupData->m_pstInHierarchy->parentFrame;
					
					RpHAnimHierarchySetKeyFrameCallBacks(pstOGroupData->m_pstOutHierarchy, rpHANIMSTDKEYFRAMETYPEID);
					
					RpHAnimHierarchyAttach(pstOGroupData->m_pstOutHierarchy);
					RpHAnimHierarchyAttach(pstOGroupData->m_pstInHierarchy2);*/

					LockFrame();

					RpHAnimHierarchyAttach(pstOGroupData->m_pstInHierarchy);

					UnlockFrame();
				}
			}

			// 마고자 (2003-11-14 오전 10:48:41) : 이동시 섹터 사이에 정보가 꼬이는 증상이
			// 있어서 일단 제거한후 , 이동 , 다시추가 작업을 한다.			

			if(m_pcsAgcmRender)
			{
				m_pcsAgcmRender->RemoveClumpFromWorld(pstOGroupData->m_pstClump);
				////@{ kday 20050513
				//#ifdef USE_MFC
				//AcuObjecWire::bGetInst().bRemClump( pstOGroupData->m_pstClump );
				//#endif //USE_MFC
				////@} kday
			}
			else
				RpWorldRemoveClump(m_pWorld, pstOGroupData->m_pstClump);

			bAdd = true;

			if(	AcuObject::GetType( pstAgcdObject->m_lObjectType ) != ACUOBJECT_TYPE_OBJECT	)
			{
				pstAgcdObject->m_lObjectType	|= ACUOBJECT_TYPE_OBJECT;
			}

			if(pstAgcdObject->m_bGroupChild)
			{
				stScale.x		= pstAgcdObject->m_pInfoGroup->m_stScale.x		;
				stScale.y		= pstAgcdObject->m_pInfoGroup->m_stScale.y		;
				stScale.z		= pstAgcdObject->m_pInfoGroup->m_stScale.z		;

				stPosition.x	= pstAgcdObject->m_pInfoGroup->m_stPosition.x	;
				stPosition.y	= pstAgcdObject->m_pInfoGroup->m_stPosition.y	;
				stPosition.z	= pstAgcdObject->m_pInfoGroup->m_stPosition.z	;


				pFrame = RpClumpGetFrame(pstOGroupData->m_pstClump);
				if(!pFrame)
				{
					return FALSE;
				}

				RwMatrix*	pModelling = RwFrameGetMatrix(pFrame);

				LockFrame();

				RwMatrixScale	( pModelling , &stScale		, rwCOMBINEREPLACE );
				RwMatrixRotate	( pModelling , &AxisX		, pstAgcdObject->m_pInfoGroup->m_fDegreeX , rwCOMBINEPOSTCONCAT );
				RwMatrixRotate	( pModelling , &AxisY		, pstAgcdObject->m_pInfoGroup->m_fDegreeY , rwCOMBINEPOSTCONCAT );
				RwMatrixTranslate( pModelling , &stPosition	, rwCOMBINEPOSTCONCAT );
				RwFrameUpdateObjects( pFrame );

				UnlockFrame();
			}
			else
			{
				stScale.x		= pstApdObject->m_stScale.x		;
				stScale.y		= pstApdObject->m_stScale.y		;
				stScale.z		= pstApdObject->m_stScale.z		;

				stPosition.x	= pstApdObject->m_stPosition.x	;
				stPosition.y	= pstApdObject->m_stPosition.y	;
				stPosition.z	= pstApdObject->m_stPosition.z	;


				pFrame = RpClumpGetFrame(pstOGroupData->m_pstClump);
				if(!pFrame)
				{
					return FALSE;
				}

				RwMatrix*	pModelling = RwFrameGetMatrix(pFrame);

				LockFrame();

				RwMatrixScale	( pModelling , &stScale		, rwCOMBINEREPLACE );
				RwMatrixRotate	( pModelling , &AxisX		, pstApdObject->m_fDegreeX , rwCOMBINEPOSTCONCAT );
				RwMatrixRotate	( pModelling , &AxisY		, pstApdObject->m_fDegreeY , rwCOMBINEPOSTCONCAT );
				RwMatrixTranslate( pModelling , &stPosition	, rwCOMBINEPOSTCONCAT );
				RwFrameUpdateObjects( pFrame );

				UnlockFrame();
			}

			/////////////////////////////////////////////////////////////////////////////////
			// 230403 BOB(Collision atomic 설정)
			if(	pstAgcdObject->m_pstCollisionAtomic )
			{
				RwFrame *pstCollisionAtomicFrame = RpAtomicGetFrame(pstAgcdObject->m_pstCollisionAtomic);

				LockFrame();

				if(!pstCollisionAtomicFrame)
				{
					pstCollisionAtomicFrame = RwFrameCreate();
					RpAtomicSetFrame(pstAgcdObject->m_pstCollisionAtomic, pstCollisionAtomicFrame);

					if(!pstCollisionAtomicFrame)
					{
						UnlockFrame();
						return FALSE;
					}
				}

				if (pstCollisionAtomicFrame)
				{
					RwFrameAddChild(RpClumpGetFrame(pstOGroupData->m_pstClump), pstCollisionAtomicFrame);

					/*
					RwMatrix*	pstCAModelling = RwFrameGetMatrix(pstCollisionAtomicFrame);

					RwMatrixScale(pstCAModelling, &stScale, rwCOMBINEREPLACE);
					RwMatrixRotate(pstCAModelling, &AxisX, pstApdObject->m_fDegreeX, rwCOMBINEPOSTCONCAT);
					RwMatrixRotate(pstCAModelling, &AxisY, pstApdObject->m_fDegreeY, rwCOMBINEPOSTCONCAT);
					RwMatrixTranslate(pstCAModelling, &stPosition, rwCOMBINEPOSTCONCAT);
					*/

					RwFrameUpdateObjects( pstCollisionAtomicFrame );
				}

				UnlockFrame();
			}
			/////////////////////////////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////////////////////
			// 230403 BOB(Picking atomic 설정)
			if(pstAgcdObject->m_pstPickAtomic)
			{
				RwFrame *pstPickAtomicFrame = RpAtomicGetFrame(pstAgcdObject->m_pstPickAtomic);

				LockFrame();

				if(!pstPickAtomicFrame)
				{
					pstPickAtomicFrame = RwFrameCreate();
					if(pstPickAtomicFrame)
						RpAtomicSetFrame(pstAgcdObject->m_pstPickAtomic, pstPickAtomicFrame);
				}

				if (pstPickAtomicFrame)
				{
					RwFrameAddChild(RpClumpGetFrame(pstOGroupData->m_pstClump), pstPickAtomicFrame);

					/*
					RwMatrix*	pstPickAtomicModelling = RwFrameGetMatrix(pstPickAtomicFrame);

					RwMatrixScale(pstPickAtomicModelling, &stScale, rwCOMBINEREPLACE);
					RwMatrixRotate(pstPickAtomicModelling, &AxisX, pstApdObject->m_fDegreeX, rwCOMBINEPOSTCONCAT);
					RwMatrixRotate(pstPickAtomicModelling, &AxisY, pstApdObject->m_fDegreeY, rwCOMBINEPOSTCONCAT);
					RwMatrixTranslate(pstPickAtomicModelling, &stPosition, rwCOMBINEPOSTCONCAT);
					*/

					RwFrameUpdateObjects(pstPickAtomicFrame);
				}

				UnlockFrame();
			}

			/////////////////////////////////////////////////////////////////////////////////

			if(bAdd)
			{
				if (m_pWorld && CheckingRange( pstApdObject ) )
				{
					if (m_pcsAgcmRender)
					{
						if (m_bShowPickingInfo && pstOGroupData->m_pstClump)
						{
							if (pstAgcdObject->m_pstPickAtomic && !RpAtomicGetClump(pstAgcdObject->m_pstPickAtomic))
								RpClumpAddAtomic(pstOGroupData->m_pstClump, pstAgcdObject->m_pstPickAtomic);

							if (pstAgcdObject->m_pstCollisionAtomic && !RpAtomicGetClump(pstAgcdObject->m_pstCollisionAtomic))
								RpClumpAddAtomic(pstOGroupData->m_pstClump, pstAgcdObject->m_pstCollisionAtomic);
						}

						m_pcsAgcmRender->AddClumpToWorld(pstOGroupData->m_pstClump);
						m_pcsAgcmRender->AddUpdateInfotoClump(pstOGroupData->m_pstClump,this, CBUpdateAnimation,NULL, pstOGroupData->m_pstClump,(PVOID)(pstOGroupData));

						////@{ kday 20050513
						//#ifdef USE_MFC
						//AcuObjecWire::bGetInst().bAddClump( pstOGroupData->m_pstClump );
						//#endif //USE_MFC
						////@} kday

					}
					else
					{
						RpWorldAddClump(m_pWorld, pstOGroupData->m_pstClump);
					}
				}
			}
		}

//		pstTGroupList = pstTGroupList->m_pstNext;
	}

	// 마고자 (2005-04-12 오후 3:26:11) : 임시 테스트 코드
	// 오브젝트에 붙어있는 이펙트 
	/*
	EnumCallback(AGCMOBJECT_CB_ID_PRE_REMOVE_DATA, (PVOID)(pstApdObject), (PVOID)(pstAgcdObject));
	EnumCallback(AGCMOBJECT_CB_ID_INIT_OBJECT, (PVOID)(pstApdObject), NULL);
	*/
	return TRUE;
}

BOOL AgcmObject::SetupObjectClump(ApdObject *pstApdObject, AgcdObject *pstAgcdObject)
{
	++(pstAgcdObject->m_pstTemplate->m_lRefCount);

	// RefCount가 0이고 Loader가 있으면, Loader에게 Load를 맡긴다.
	if (m_pcsAgcmResourceLoader)
	{
		m_pcsAgcmResourceLoader->AddLoadEntry(m_lLoaderTemplateIndex, pstApdObject, pstAgcdObject);
		//m_pcsAgcmResourceLoader->AddLoadEntry(m_lLoaderInitIndex, pstApdObject, pstAgcdObject);
	}
	else
	{
		LoadTemplateData( pstAgcdObject->m_pstTemplate );
		SetupObjectClump1(pstApdObject, pstAgcdObject);
		return SetupObjectClump2(pstApdObject, pstAgcdObject);
	}

	return TRUE;
}

RpAtomic *___Test___a(RpAtomic *atomic, void *data)
{
	INT32 *plCount = (INT32 *)(data);

	if(*plCount == 3)
		AgcmObject::m_pThis->GetAgcmRenderModule()->AddAtomicToWorld(atomic,ONLY_NONALPHA);	
	else 
		AgcmObject::m_pThis->GetAgcmRenderModule()->AddAlphaBModeAtomicToWorld(atomic,R_BLENDSRCAINVA_ADD,FALSE);
		

	++(*plCount);

	return atomic;
}

BOOL AgcmObject::SetupObjectClump2(ApdObject *pstApdObject, AgcdObject *pstAgcdObject)
{
	PROFILE("AgcmObject::SetupObjectClump2");

	if (pstAgcdObject->m_lStatus & AGCDOBJECT_STATUS_REMOVED)
		return TRUE;

	ApdObjectTemplate	*	pstApdObjectTemplate	= pstApdObject->m_pcsTemplate;
	AgcdObjectTemplate	*	pstAgcdObjectTemplate	= pstAgcdObject->m_pstTemplate;

	//RwFrame				*	pFrame					;
	//RwV3d					stScale					;
	//RwV3d					stPosition				;
	//INT32					lCount					;

	static	RwV3d	AxisX	= { 1.0f , 0.0f , 0.0f };
	static	RwV3d	AxisY	= { 0.0f , 1.0f , 0.0f };

//	AgcdObjectTemplateGroupList	*pstTGroupList = pstAgcdObjectTemplate->m_stGroup.m_pstList;
	AgcdObjectTemplateGroupData	*pstTGroupData;
	AgcdObjectGroupData			*pstOGroupData;
	AgcdLODData					*pstLODData = NULL;

//	while(pstTGroupList)
	for(INT32 lGroupIndex = 0;; ++lGroupIndex)
	{
		{
			PROFILE("AgcmObject::SetupObjectClump2 - AddClump1");

			pstTGroupData = m_csObjectList.GetObjectTemplateGroup(&pstAgcdObjectTemplate->m_stGroup, lGroupIndex);
			if(!pstTGroupData)
				break;

			pstOGroupData	= GetObjectGroupData(&pstAgcdObject->m_stGroup, lGroupIndex);
			if(!pstOGroupData)
			{
				return FALSE;
			}

			if( pstAgcdObjectTemplate->m_lObjectType & ACUOBJECT_TYPE_DONOT_CULL)
			{
				// 이녀석은 컬링을 하지 않고 그냥 뿌림..
				if( pstOGroupData->m_pstClump )
				{
					pstOGroupData->m_pstClump->ulFlag |= RWFLAG_DONOT_CULL;
				}
			}

			if (m_pcsAgcmLODManager)
				pstLODData		= m_pcsAgcmLODManager->GetLODData(&pstAgcdObjectTemplate->m_stLOD, pstTGroupData->m_lIndex, FALSE);
	//		if(!pstLODData)
	//			return FALSE;

			/* 이젠 PreLight를 안써요.... 04/10/09 Parn
			if(pstAgcdObjectTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)
			{
				if( pstOGroupData->m_pstClump )
				{
					AcuObject::SetClumpPreLitLim(pstOGroupData->m_pstClump, &pstAgcdObjectTemplate->m_stPreLight);
				}
				else
				{
					TRACE( "AgcmObject::SetupObjectClump2 , pstOGroupData->m_pstClump == NULL ");
				}
			}
			*/
		}

		if(pstOGroupData->m_pstClump && !pstOGroupData->m_bAddToWorld)
		{
			PROFILE("AgcmObject::SetupObjectClump2 - AddClump2");

			if (m_pWorld && CheckingRange( pstApdObject ) )
			{
				if(((pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_IS_SYSTEM_OBJECT) && m_bSetupSystemObject) ||
					(!(pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_IS_SYSTEM_OBJECT) && m_bSetupNormalObject))
				{

					if (m_pcsAgcmRender)
					{
						{
							PROFILE("AgcmObject::SetupObjectClump2 - AddClump2 - 1");

							//TRACE("Object Add To Render - %x\n",pstOGroupData->m_pstClump);

							if (m_bShowPickingInfo && pstOGroupData->m_pstClump)
							{
								if (pstAgcdObject->m_pstPickAtomic && !RpAtomicGetClump(pstAgcdObject->m_pstPickAtomic))
									RpClumpAddAtomic(pstOGroupData->m_pstClump, pstAgcdObject->m_pstPickAtomic);

								if (pstAgcdObject->m_pstCollisionAtomic && !RpAtomicGetClump(pstAgcdObject->m_pstCollisionAtomic))
									RpClumpAddAtomic(pstOGroupData->m_pstClump, pstAgcdObject->m_pstCollisionAtomic);
							}

							m_pcsAgcmRender->AddClumpToWorld(pstOGroupData->m_pstClump); 
							m_pcsAgcmRender->AddUpdateInfotoClump(pstOGroupData->m_pstClump,this, 
								CBUpdateAnimation,NULL, pstOGroupData->m_pstClump,(PVOID)(pstOGroupData));
							
							////@{ kday 20050513
							//#ifdef USE_MFC
							//AcuObjecWire::bGetInst().bAddClump( pstOGroupData->m_pstClump );
							//#endif //USE_MFC
							////@} kday
						}
					}
					else
						RpWorldAddClump(m_pWorld, pstOGroupData->m_pstClump);

					//AgcmLODManager로 변경 (유의)
					if((pstLODData) && (pstLODData->m_ulMaxLODLevel))
					{
						PROFILE("AgcmObject::SetupObjectClump2 - SetLOD");
						if(m_pcsAgcmLODManager)
						{
							m_pcsAgcmLODManager->SetDistanceRate(pstLODData);
							m_pcsAgcmLODManager->SetLODCallback(pstOGroupData->m_pstClump);
							AcuObject::SetClumpDInfo_LODLevel(pstOGroupData->m_pstClump, pstLODData->m_ulMaxLODLevel);
						}
					}

					{
					PROFILE("AgcmObject::SetupObjectClump2 - shadowmap");
					//@{ Jaewon 20040706
					// register the clump of the object as a shadow map receiver.
					AgcmShadowmap* pAgcmShadowmap = (AgcmShadowmap*)GetModule("AgcmShadowmap");
					if(pAgcmShadowmap)
						pAgcmShadowmap->registerClump(pstOGroupData->m_pstClump);
					//@} Jaewon
					}

					pstOGroupData->m_bAddToWorld = TRUE;
				}
			}
		}
	}

	{
		PROFILE("AgcmObject::SetupObjectClump2 - EnumCallback");

		EnumCallback(AGCMOBJECT_CB_ID_INIT_OBJECT, (PVOID)(pstApdObject), NULL);

		//. 2006. 5. 15. nonstopdj
		//. 
	}

	CBOnAddObject(pstApdObject, this, NULL);

	return TRUE;
}

BOOL AgcmObject::SetupObjectClump1(ApdObject *pstApdObject, AgcdObject *pstAgcdObject)
{
	PROFILE("AgcmObject::SetupObjectClump1");

	ApdObjectTemplate	*	pstApdObjectTemplate	= pstApdObject->m_pcsTemplate;
	AgcdObjectTemplate	*	pstAgcdObjectTemplate	= pstAgcdObject->m_pstTemplate;

	RwFrame				*	pFrame					;
	RwV3d					stScale					;
	RwV3d					stPosition				;
//	INT32					lCount					;

	static	RwV3d	AxisX	= { 1.0f , 0.0f , 0.0f };
	static	RwV3d	AxisY	= { 0.0f , 1.0f , 0.0f };

//	AgcdObjectTemplateGroupList	*pstTGroupList = pstAgcdObjectTemplate->m_stGroup.m_pstList;
	AgcdObjectTemplateGroupData	*pstTGroupData;
	AgcdObjectGroupData			*pstOGroupData;
	AgcdLODData					*pstLODData = NULL;

//	while(pstTGroupList)
	for (INT32 lGroupIndex = 0;; ++lGroupIndex)
	{
//		pstTGroupData	= &pstTGroupList->m_csData;
		pstTGroupData = m_csObjectList.GetObjectTemplateGroup(&pstAgcdObjectTemplate->m_stGroup, lGroupIndex);
		if (!pstTGroupData)
			break;

		pstOGroupData	= GetObjectGroupData(&pstAgcdObject->m_stGroup, lGroupIndex);
		if (!pstOGroupData)
		{
			return FALSE;
		}

		pstOGroupData->m_pvThis					= this;
		pstOGroupData->m_pstApdObject			= pstApdObject;
		pstOGroupData->m_pcsTemplateGroupData	= pstTGroupData;

		if (m_pcsAgcmLODManager)
			pstLODData		= m_pcsAgcmLODManager->GetLODData(&pstAgcdObjectTemplate->m_stLOD, pstTGroupData->m_lIndex, FALSE);
//		if(!pstLODData)
//			return FALSE;

		if (pstTGroupData->m_pstClump)
		{
			bool bAdd = false;

			if (!pstOGroupData->m_pstClump)
			{
				LockFrame();
				pstOGroupData->m_pstClump = RpClumpClone(pstTGroupData->m_pstClump);
				UnlockFrame();

				if (!pstOGroupData->m_pstClump)
				{
					return FALSE;
				}

				pstOGroupData->m_pstClump->pvApBase = pstApdObject;
#ifdef _DEBUG
				//@{ Jaewon 20050905
				// ;)
				//pstOGroupData->m_pstClump->szName = pstApdObject->m_pcsTemplate->m_szName;
				RpClumpSetName(pstOGroupData->m_pstClump, pstApdObject->m_pcsTemplate->m_szName);
				//@} Jaewon
#endif

				pstOGroupData->m_pstInHierarchy = AgcuObject_GetHierarchy(pstOGroupData->m_pstClump);
				if (pstOGroupData->m_pstInHierarchy)
				{
					RpHAnimHierarchySetFlags(pstOGroupData->m_pstInHierarchy,
						(RpHAnimHierarchyFlag)
						( RpHAnimHierarchyGetFlags(pstOGroupData->m_pstInHierarchy) |
						rpHANIMHIERARCHYUPDATELTMS |
						rpHANIMHIERARCHYUPDATEMODELLINGMATRICES | rpHANIMHIERARCHYLOCALSPACEMATRICES) );
					
//					RpClumpForAllAtomics(pstOGroupData->m_pstClump, AgcuObject_SetHierarchyForSkinAtomic, (void *) pstOGroupData->m_pstInHierarchy);
					
					/*
					RpHAnimHierarchyFlag flags = (RpHAnimHierarchyFlag) (pstOGroupData->m_pstInHierarchy)->flags;

					pstOGroupData->m_pstOutHierarchy = RpHAnimHierarchyCreateFromHierarchy(pstOGroupData->m_pstInHierarchy, flags, AGCMOBJECT_HIERARCHY_MAX_KEYFRAME);
					pstOGroupData->m_pstInHierarchy2 = RpHAnimHierarchyCreateFromHierarchy(pstOGroupData->m_pstInHierarchy, flags, AGCMOBJECT_HIERARCHY_MAX_KEYFRAME);
					
					pstOGroupData->m_pstOutHierarchy->parentFrame = pstOGroupData->m_pstInHierarchy->parentFrame;
					pstOGroupData->m_pstInHierarchy2->parentFrame = pstOGroupData->m_pstInHierarchy->parentFrame;
					
					RpHAnimHierarchySetKeyFrameCallBacks(pstOGroupData->m_pstOutHierarchy, rpHANIMSTDKEYFRAMETYPEID);
					
					RpHAnimHierarchyAttach(pstOGroupData->m_pstOutHierarchy);
					RpHAnimHierarchyAttach(pstOGroupData->m_pstInHierarchy2);
					*/
					LockFrame();
					RpHAnimHierarchyAttach(pstOGroupData->m_pstInHierarchy);
					UnlockFrame();
				}

				bAdd = true;
			}

			// 마고자 (2003-11-14 오전 10:48:41) : 이동시 섹터 사이에 정보가 꼬이는 증상이
			// 있어서 일단 제거한후 , 이동 , 다시추가 작업을 한다.

			/*
			if(m_pcsAgcmRender)
				m_pcsAgcmRender->RemoveClumpFromWorld(pstOGroupData->m_pstClump);
			else
				RpWorldRemoveClump(m_pWorld, pstOGroupData->m_pstClump);
			*/

			bAdd = true;

			pstOGroupData->m_fAnimSpeed		= pstTGroupData->m_fAnimSpeed;
//			for (lCount = 0; lCount < E_OBJECT_ANIM_TYPE_NUM; ++lCount)
//				pstOGroupData->m_afAnimSpeed[lCount] = pstTGroupData->m_afAnimSpeed[lCount];

			// 마고자 (2004-01-12 오후 3:21:51) : 데이타 잠깐 수정.
			// 라이더블 수정이 안돼길레.
			if( pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_OBJECT )
			{
				// do nothing
			}
			else
			{
				pstAgcdObject->m_lObjectType = pstAgcdObjectTemplate->m_lObjectType | ACUOBJECT_TYPE_OBJECT;
				//pstAgcdObject->m_lObjectType = pstAgcdObject->m_lObjectType | ACUOBJECT_TYPE_OBJECT;
			}
			ASSERT( pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_OBJECT );

			memcpy(&pstOGroupData->m_stBSphere, &pstTGroupData->m_stBSphere, sizeof(RwSphere));

			if (pstAgcdObject->m_bGroupChild)
			{
				stScale.x		= pstAgcdObject->m_pInfoGroup->m_stScale.x		;
				stScale.y		= pstAgcdObject->m_pInfoGroup->m_stScale.y		;
				stScale.z		= pstAgcdObject->m_pInfoGroup->m_stScale.z		;

				if (stScale.x > stScale.y)
				{
					if (stScale.x > stScale.z)
					{
						pstOGroupData->m_stBSphere.radius *= stScale.x;
					}
					else // (stScale.x <= stScale.z)
					{
						pstOGroupData->m_stBSphere.radius *= stScale.z;
					}
				}
				else // (stScale.x <= stScale.y)
				{
					if(stScale.y > stScale.z)
					{
						pstOGroupData->m_stBSphere.radius *= stScale.y;
					}
					else // (stScale.y <= stScale.z)
					{
						pstOGroupData->m_stBSphere.radius *= stScale.z;
					}
				}

				stPosition.x	= pstAgcdObject->m_pInfoGroup->m_stPosition.x	;
				stPosition.y	= pstAgcdObject->m_pInfoGroup->m_stPosition.y	;
				stPosition.z	= pstAgcdObject->m_pInfoGroup->m_stPosition.z	;


				pFrame = RpClumpGetFrame(pstOGroupData->m_pstClump);
				RwMatrix*	pModelling = RwFrameGetMatrix(pFrame);

				LockFrame();

				RwMatrixScale	( pModelling , &stScale		, rwCOMBINEREPLACE );
				RwMatrixRotate	( pModelling , &AxisX		, pstAgcdObject->m_pInfoGroup->m_fDegreeX , rwCOMBINEPOSTCONCAT );
				RwMatrixRotate	( pModelling , &AxisY		, pstAgcdObject->m_pInfoGroup->m_fDegreeY , rwCOMBINEPOSTCONCAT );
				RwMatrixTranslate( pModelling , &stPosition	, rwCOMBINEPOSTCONCAT );

				RwFrameUpdateObjects( pFrame ) ;

				UnlockFrame();
			}
			else
			{
				stScale.x		= pstApdObject->m_stScale.x		;
				stScale.y		= pstApdObject->m_stScale.y		;
				stScale.z		= pstApdObject->m_stScale.z		;

				if (stScale.x > stScale.y)
				{
					if (stScale.x > stScale.z)
					{
						pstOGroupData->m_stBSphere.radius *= stScale.x;
					}
					else // (stScale.x <= stScale.z)
					{
						pstOGroupData->m_stBSphere.radius *= stScale.z;
					}
				}
				else // (stScale.x <= stScale.y)
				{
					if (stScale.y > stScale.z)
					{
						pstOGroupData->m_stBSphere.radius *= stScale.y;
					}
					else // (stScale.y <= stScale.z)
					{
						pstOGroupData->m_stBSphere.radius *= stScale.z;
					}
				}

				stPosition.x	= pstApdObject->m_stPosition.x	;
				stPosition.y	= pstApdObject->m_stPosition.y	;
				stPosition.z	= pstApdObject->m_stPosition.z	;


				pFrame = RpClumpGetFrame(pstOGroupData->m_pstClump);
				RwMatrix*	pModelling = RwFrameGetMatrix(pFrame);

				LockFrame();

				RwMatrixScale	( pModelling , &stScale		, rwCOMBINEREPLACE );
				RwMatrixRotate	( pModelling , &AxisX		, pstApdObject->m_fDegreeX , rwCOMBINEPOSTCONCAT );
				RwMatrixRotate	( pModelling , &AxisY		, pstApdObject->m_fDegreeY , rwCOMBINEPOSTCONCAT );
				RwMatrixTranslate( pModelling , &stPosition	, rwCOMBINEPOSTCONCAT );

				RwFrameUpdateObjects( pFrame );

				UnlockFrame();
			}

			// Occluder Box 와 최대 BBOX 변환
			// 현재 시점에서 옥트리정보가 날아갔으면 다시 재계산해준다.
			if( IsInValidOctreeData( pstAgcdObjectTemplate ) )
			{
				ReCalulateOctreeData( "Object", pstAgcdObjectTemplate );
			}

			pstAgcdObject->m_stOcTreeData.bMove = pstAgcdObjectTemplate->m_stOcTreeData.bMove;
			pstAgcdObject->m_stOcTreeData.isOccluder = pstAgcdObjectTemplate->m_stOcTreeData.isOccluder;
			pstAgcdObject->m_stOcTreeData.nOccluderBox = pstAgcdObjectTemplate->m_stOcTreeData.nOccluderBox;
			memcpy(pstAgcdObject->m_stOcTreeData.topVerts_MAX,pstAgcdObjectTemplate->m_stOcTreeData.topVerts_MAX,sizeof(pstAgcdObject->m_stOcTreeData.topVerts_MAX));

			pstAgcdObject->m_stOcTreeData.pTopVerts = new RwV3d[4 * pstAgcdObject->m_stOcTreeData.nOccluderBox];
			memcpy(pstAgcdObject->m_stOcTreeData.pTopVerts,pstAgcdObjectTemplate->m_stOcTreeData.pTopVerts,sizeof(RwV3d) * 4 * pstAgcdObject->m_stOcTreeData.nOccluderBox);

			if(pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_DUNGEON_STRUCTURE)
			{
				int	i;

				//던전안 octree box들 조정 2005.3.5 gemani
				for(i=0;i<4;++i)
				{
					if(pstAgcdObject->m_stOcTreeData.topVerts_MAX[i].y > 1600.0f) 
						pstAgcdObject->m_stOcTreeData.topVerts_MAX[i].y = 1600.0f;
				}

				int		start_index;
				for(i=0;i<pstAgcdObject->m_stOcTreeData.nOccluderBox;++i)
				{
					start_index = i<<2;
					for(int j=0;j<4;++j)
					{
						if(pstAgcdObject->m_stOcTreeData.pTopVerts[start_index + j].y > 400.0f) 
							pstAgcdObject->m_stOcTreeData.pTopVerts[start_index + j].y = 400.0f;
					}
				}
			}
			else
			{
				int		start_index;
				int		i;

				for(i=0;i<pstAgcdObject->m_stOcTreeData.nOccluderBox;++i)
				{
					start_index = i<<2;
					for(int j=0;j<4;++j)
					{
						if(pstAgcdObject->m_stOcTreeData.pTopVerts[start_index + j].y > 1600.0f) 
							pstAgcdObject->m_stOcTreeData.pTopVerts[start_index + j].y = 1600.0f;
					}
				}
			}

			RwMatrix*	pMatrix = RwFrameGetMatrix(pFrame);
			RwV3dTransformPoints (pstAgcdObject->m_stOcTreeData.topVerts_MAX,
				pstAgcdObject->m_stOcTreeData.topVerts_MAX,4,pMatrix);

			for (int i=0;i<pstAgcdObject->m_stOcTreeData.nOccluderBox;++i)
			{
				RwV3dTransformPoints (&pstAgcdObject->m_stOcTreeData.pTopVerts[i<<2],
				&pstAgcdObject->m_stOcTreeData.pTopVerts[i<<2],4,pMatrix);
			}
			
			/////////////////////////////////////////////////////////////////////////////////
			// 230403 BOB(Collision atomic 설정)
			if ((!pstAgcdObject->m_pstCollisionAtomic) && (pstAgcdObjectTemplate->m_pstCollisionAtomic))
			{
				LockFrame();

				pstAgcdObject->m_pstCollisionAtomic = RpAtomicClone(pstAgcdObjectTemplate->m_pstCollisionAtomic);

				if (!pstAgcdObject->m_pstCollisionAtomic)
				{
					UnlockFrame();
					return FALSE;
				}

				RwFrame *pstCollisionAtomicFrame = RpAtomicGetFrame(pstAgcdObject->m_pstCollisionAtomic);

				if (!pstCollisionAtomicFrame)
				{
					pstCollisionAtomicFrame = RwFrameCreate();
					if (!pstCollisionAtomicFrame)
					{
						UnlockFrame();
						return FALSE;
					}

					//. 2006. 3. 13. nonstopdj
					//. AgcdObject::collision Atomic valiadation check.
					if(pstAgcdObject->m_pstCollisionAtomic)
						RpAtomicSetFrame(pstAgcdObject->m_pstCollisionAtomic, pstCollisionAtomicFrame);
				}

				RwFrameAddChild(RpClumpGetFrame(pstOGroupData->m_pstClump), pstCollisionAtomicFrame);

				/*
				RwMatrix*	pstCAModelling = RwFrameGetMatrix( pstCollisionAtomicFrame );

				RwMatrixScale(pstCAModelling, &stScale, rwCOMBINEREPLACE);
				RwMatrixRotate(pstCAModelling, &AxisX, pstApdObject->m_fDegreeX, rwCOMBINEPOSTCONCAT);
				RwMatrixRotate(pstCAModelling, &AxisY, pstApdObject->m_fDegreeY, rwCOMBINEPOSTCONCAT);
				RwMatrixTranslate(pstCAModelling, &stPosition, rwCOMBINEPOSTCONCAT);
				*/

				RwFrameUpdateObjects( pstCollisionAtomicFrame );

				UnlockFrame();
			}
			/////////////////////////////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////////////////////
			// 230403 BOB(Picking atomic 설정)
			if ((!pstAgcdObject->m_pstPickAtomic) && (pstAgcdObjectTemplate->m_pstPickingAtomic))
			{
				LockFrame();

				pstAgcdObject->m_pstPickAtomic = RpAtomicClone(pstAgcdObjectTemplate->m_pstPickingAtomic);
				if (pstAgcdObject->m_pstPickAtomic)
				{
					RwFrame *pstPickAtomicFrame = RpAtomicGetFrame(pstAgcdObject->m_pstPickAtomic);
					if (!pstPickAtomicFrame)
					{
						pstPickAtomicFrame = RwFrameCreate();
						if (!pstPickAtomicFrame)
						{
							UnlockFrame();
							return FALSE;
						}

						RpAtomicSetFrame(pstAgcdObject->m_pstPickAtomic, pstPickAtomicFrame);
					}

					RwFrameAddChild(RpClumpGetFrame(pstOGroupData->m_pstClump), pstPickAtomicFrame);

					/*
					RwMatrix*	pstPickAtomicModelling = RwFrameGetMatrix( pstPickAtomicFrame );

					RwMatrixScale(pstPickAtomicModelling, &stScale, rwCOMBINEREPLACE);
					RwMatrixRotate(pstPickAtomicModelling, &AxisX, pstApdObject->m_fDegreeX, rwCOMBINEPOSTCONCAT);
					RwMatrixRotate(pstPickAtomicModelling, &AxisY, pstApdObject->m_fDegreeY, rwCOMBINEPOSTCONCAT);
					RwMatrixTranslate(pstPickAtomicModelling, &stPosition, rwCOMBINEPOSTCONCAT);
					*/

					RwFrameUpdateObjects( pstPickAtomicFrame );
				}

				UnlockFrame();
			}
			/////////////////////////////////////////////////////////////////////////////////

			AcuObject::SetClumpType(
				pstOGroupData->m_pstClump												,
				( enum AcuObjectType ) pstAgcdObject->m_lObjectType						,
				pstApdObject->m_lID														,
				pstApdObject															,
				pstApdObjectTemplate													,
				((pstLODData) && (pstLODData->m_ulMaxLODLevel)) ? (pstLODData) : (NULL)	,
				pstAgcdObject->m_pstPickAtomic											,
				&pstOGroupData->m_stBSphere												,
				pstAgcdObject->m_pstCollisionAtomic										,
				&pstAgcdObject->m_stOcTreeData											,
				&pstApdObject->m_listOcTreeID										);

/*			if (	(pstTGroupData->m_pastAnim[E_OBJECT_ANIM_TYPE_NORMAL]) &&
					(pstTGroupData->m_pastAnim[E_OBJECT_ANIM_TYPE_NORMAL]->m_ppastAnimData) &&
					(pstTGroupData->m_pastAnim[E_OBJECT_ANIM_TYPE_NORMAL]->m_ppastAnimData[0])	)
			{
				if (!SetAnimation(pstOGroupData, pstTGroupData->m_pastAnim[E_OBJECT_ANIM_TYPE_NORMAL], E_OBJECT_ANIM_TYPE_NORMAL))
				{
					DropAnimation(pstOGroupData);
					OutputDebugString("AgcmObject::SetupObjectClump() Error SetAnimation 실패!!!\n");
				}
				else
				{
					pstOGroupData->m_pstDefaultAnim		= pstTGroupData->m_pastAnim[E_OBJECT_ANIM_TYPE_NORMAL];
					pstOGroupData->m_eDefaultAnimType	= E_OBJECT_ANIM_TYPE_NORMAL;
				}
			}*/

			if (	(pstTGroupData->m_pcsAnimation) &&
					(pstTGroupData->m_pcsAnimation->m_pcsHead) &&
					(pstTGroupData->m_pcsAnimation->m_pcsHead->m_pcsRtAnim)	)
			{
				SetAnimation(pstOGroupData, pstTGroupData->m_pcsAnimation->m_pcsHead->m_pcsRtAnim->m_pstAnimation);
			}
			else
			{
				pstOGroupData->m_bStopAnimation	= TRUE;
			}
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 012803 Bob Jung.
******************************************************************************/
BOOL AgcmObject::CBInitObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject *pThis = (AgcmObject *)(pClass);

	ApdObject			*pcsApdObject			= (ApdObject *)(pData);
//	ApdObjectTemplate	*pcsApdObjectTemplate	= (ApdObjectTemplate *)(pCustData);

	if(!pThis->InitObject(pcsApdObject))
	{
		OutputDebugString("AgcmObject::CBInitObject() Error !!!\n");
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmObject::CBRemoveObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmObject::CBRemoveObject");

	AgcmObject	*pThis			= (AgcmObject *) pClass;
	ApdObject	*pstApdObject	= (ApdObject *) pData;
	AgcdObject	*pstAgcdObject	= pThis->GetObjectData(pstApdObject);

	pstAgcdObject->m_lStatus |= AGCDOBJECT_STATUS_REMOVED;

	if (pThis->m_pcsAgcmResourceLoader)
	{
		pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry(pThis->m_lLoaderTemplateIndex, (PVOID) pstApdObject, pstAgcdObject);
	}

	pThis->GroupDelete(pstAgcdObject);

	//OctreeData 삭제~
	if(pstAgcdObject->m_stOcTreeData.pTopVerts)
	{
		delete []pstAgcdObject->m_stOcTreeData.pTopVerts;
		pstAgcdObject->m_stOcTreeData.pTopVerts = NULL;

		// 2005.3.20 gemani
		// 중요.. 렌더에 삽입되 잇는데 pTopVerts가 지워진 경우가 있다. 
		// 아무래도 2번 추가후 1번째 삭제 랑 엉킨거 같은데 이럴 경우를 대비에 pTopVerts체크 안하게 false세팅
		pstAgcdObject->m_stOcTreeData.isOccluder = FALSE;  
	}

/*	if(pstAgcdObject->m_pstInHierarchy)
	{
		RpHAnimHierarchyDestroy(pstAgcdObject->m_pstInHierarchy);
		pstAgcdObject->m_pstInHierarchy	= NULL;
	}
	if(pstAgcdObject->m_pstInHierarchy2)
	{
		RpHAnimHierarchyDestroy(pstAgcdObject->m_pstInHierarchy2);
		pstAgcdObject->m_pstInHierarchy2 = NULL;
	}
	if(pstAgcdObject->m_pstOutHierarchy)
	{
		RpHAnimHierarchyDestroy(pstAgcdObject->m_pstOutHierarchy);
		pstAgcdObject->m_pstOutHierarchy = NULL;
	} */

	// 라이더블 오브젝트 처리~
	if(	AcuObject::GetProperty( pstAgcdObject->m_lObjectType ) & ACUOBJECT_TYPE_BLOCKING	||
		AcuObject::GetProperty( pstAgcdObject->m_lObjectType ) & ACUOBJECT_TYPE_RIDABLE		)
	{
		AuNode< AgcmObject::RidableOverlapping >	* pNode = pThis->m_listRidableObjectlapping.GetHeadNode();
		AgcmObject::RidableOverlapping				* pRidable;
		while( pNode )
		{
			pRidable	= &pNode->GetData();

			if( pRidable->nObjectIndex == pstApdObject->m_lID )
			{
				pThis->m_listRidableObjectlapping.RemoveNode( pNode );
				break;
			}
			pNode = pNode->GetNextNode();
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose : Init 콜백 때 불린당...
*
* 041703 Bob Jung.
******************************************************************************/
BOOL AgcmObject::InitObject(ApdObject *pcsApdObject)
{
	INT32		lGroupCount		= 0; // Debug를 위해 잠시...
	AgcdObject	*pstAgcdObject	= GetObjectData(pcsApdObject);
	if(!pstAgcdObject)
	{
		OutputDebugString("AgcmObject::InitObject() Error (1) !!!\n");
		return FALSE;
	}

	pstAgcdObject->m_lStatus = AGCDOBJECT_STATUS_INIT;
	pstAgcdObject->m_pvClass = (PVOID)(this);
	pstAgcdObject->m_pstTemplate = GetTemplateData(pcsApdObject->m_pcsTemplate);

	pstAgcdObject->m_stOcTreeData.pTopVerts = NULL;

	// 타입복사

	// 마고자 (2004-10-13 오후 2:37:58) : 
	// 던젼 플래그는 유지된다..

	BOOL	bDungeon	= FALSE;
	BOOL	bDome		= FALSE;
	if ( pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_DUNGEON_STRUCTURE )
		bDungeon = TRUE	;
	else
		bDungeon = FALSE;

	if ( pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_DUNGEON_DOME )
		bDome = TRUE	;
	else
		bDome = FALSE;
		
	pstAgcdObject->m_lObjectType = pstAgcdObject->m_pstTemplate->m_lObjectType | ACUOBJECT_TYPE_OBJECT;

	if( bDungeon )
		pstAgcdObject->m_lObjectType |= ACUOBJECT_TYPE_DUNGEON_STRUCTURE;
	if( bDome )
		pstAgcdObject->m_lObjectType |= ACUOBJECT_TYPE_DUNGEON_DOME;

	/* 이젠 Type을 Template꺼로 무조건 쓴다. 04/10/09
	if (!(pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_OBJECT))
	{
		pstAgcdObject->m_lObjectType = pstAgcdObject->m_pstTemplate->m_lObjectType | ACUOBJECT_TYPE_OBJECT;

		if (pcsApdObject->m_nObjectType & APDOBJECT_TYPE_BLOCKING)
			pstAgcdObject->m_lObjectType |= ACUOBJECT_TYPE_BLOCKING;

		if (pcsApdObject->m_nObjectType & APDOBJECT_TYPE_RIDABLE)
			pstAgcdObject->m_lObjectType |= ACUOBJECT_TYPE_RIDABLE;
	}
	*/

	if(((pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_IS_SYSTEM_OBJECT) && m_bSetupSystemObject) ||
		(!(pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_IS_SYSTEM_OBJECT) && m_bSetupNormalObject))
	{
		if( m_bRangeApplied )
		{
			// 범위가 설정돼어있으면 범위 내의 녀석들만 세팅한다.
			if( 
				m_fRangeStartX					<	pcsApdObject->m_stPosition.x	&&
				pcsApdObject->m_stPosition.x	<=	m_fRangeEndX					&&
				m_fRangeStartZ					<	pcsApdObject->m_stPosition.z	&&
				pcsApdObject->m_stPosition.z	<=	m_fRangeEndZ				
			)
			{
				// 레인지 아닐경우 그냥 호출함.
				if(!SetupObjectClump(pcsApdObject, pstAgcdObject))
				{
					OutputDebugString("AgcmObject::InitObject() Error (2) !!!\n");
					return FALSE;
				}

				++lGroupCount;

				/* 풀처리는 막겠음 (Parn)
				AgcdObject* child_node = pstAgcdObject->m_listNext;
				while(child_node)
				{
					SetupObjectClump(pcsApdObject, child_node);
					child_node = child_node->m_listNext;
					++lGroupCount;
				}
				*/
			}
			else
			{
				// InitObject를 호출해주지 않음..
			}
		}
		else
		{
			// 레인지 아닐경우 그냥 호출함.
			if(!SetupObjectClump(pcsApdObject, pstAgcdObject))
			{
				OutputDebugString("AgcmObject::InitObject() Error (2) !!!\n");
				return FALSE;
			}

			/* 풀처리는 막겠음 (Parn)
			AgcdObject* child_node = pstAgcdObject->m_listNext;
			while(child_node)
			{
				SetupObjectClump(pcsApdObject, child_node);
				child_node = child_node->m_listNext;
			}
			*/
		}
	}

	return TRUE;
}

BOOL	AgcmObject::SetRange( FLOAT x1 , FLOAT z1 , FLOAT x2 , FLOAT z2	)
{
	m_fRangeStartX	= x1;
	m_fRangeStartZ	= z1;
	m_fRangeEndX	= x2;
	m_fRangeEndZ	= z2;

	return TRUE;
}

BOOL	AgcmObject::UseRange( BOOL bUse									)
{
	return m_bRangeApplied = bUse;
}


BOOL	AgcmObject::CheckingRange( ApdObject * pObject )
{
	// 범위체크함..
	if( IsRangeApplied() )
	{
		if( pObject->m_stPosition.x >=	m_fRangeStartX	&&
			pObject->m_stPosition.x <	m_fRangeEndX	&&
			pObject->m_stPosition.z >=	m_fRangeStartZ	&&
			pObject->m_stPosition.z <	m_fRangeEndZ	)
		{
			return TRUE;
		}
		else
			return FALSE;
	}
	else return TRUE;
}

BOOL	AgcmObject::DumpTexDict()
{
	if (!m_pstTexDict)
		return FALSE;

	// Texture Dictionary가 있으면, StreamWrite한다. Platform Independent Data로
	if (m_pstTexDict && m_szTexDict[0])
	{
		RwStream *		stream;

		stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMWRITE, m_szTexDict );
		if( stream )
		{
			RtPITexDictionaryStreamWrite( m_pstTexDict , stream );
   
			RwStreamClose(stream, NULL);
		}
		else
			return FALSE;
	}

	return TRUE;
}

BOOL	AgcmObject::UpdateAmbient(AgcdObjectTemplate *pstAgcdTemplate, BOOL bAmbient)
{
	if ((pstAgcdTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_AMBIENT) ? (TRUE) : (FALSE) == bAmbient)
		return TRUE;

	INT32					lIndex;
	ApdObject				*pcsObject;
	AgcdObject				*pstAgcdObject;
	
	AgcdObjectGroupList		*pstOGroupList;
	AgcdObjectGroupData		*pstOGroupData;

	lIndex = 0;
	for (pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex); pcsObject; pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex))
	{
		if (pstAgcdTemplate != GetTemplateData(pcsObject->m_pcsTemplate))
			continue;

		pstAgcdObject = GetObjectData(pcsObject);
		pstOGroupList = pstAgcdObject->m_stGroup.m_pstList;

		while(pstOGroupList)
		{
			pstOGroupData = &pstOGroupList->m_csData;

			if (pstOGroupData->m_pstClump && m_pWorld)
			{
				if (m_pcsAgcmRender)
				{
					m_pcsAgcmRender->RemoveClumpFromWorld(pstOGroupData->m_pstClump);
					m_pcsAgcmRender->AddClumpToWorld(pstOGroupData->m_pstClump);
				}
				else
				{
					RpWorldRemoveClump(m_pWorld, pstOGroupData->m_pstClump);
					RpWorldAddClump(m_pWorld, pstOGroupData->m_pstClump);
				}
			}

			pstOGroupList = pstOGroupList->m_pstNext;
		}
	}

	if(bAmbient)
	{
		pstAgcdTemplate->m_lObjectType |= ACUOBJECT_TYPE_USE_AMBIENT;
	}
	else
	{
		pstAgcdTemplate->m_lObjectType &= ~ACUOBJECT_TYPE_USE_AMBIENT;
	}

	return TRUE;
}

BOOL	AgcmObject::UpdateAlpha(AgcdObjectTemplate *pstAgcdTemplate, BOOL bAlpha)
{
	if ((pstAgcdTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_ALPHA) ? (TRUE) : (FALSE) == bAlpha)
		return TRUE;

	INT32					lIndex;
	ApdObject				*pcsObject;
	AgcdObject				*pstAgcdObject;

	AgcdObjectGroupList		*pstOGroupList;
	AgcdObjectGroupData		*pstOGroupData;

	lIndex = 0;
	for (pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex); pcsObject; pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex))
	{
		if (pstAgcdTemplate != GetTemplateData(pcsObject->m_pcsTemplate))
			continue;

		pstAgcdObject = GetObjectData(pcsObject);
		pstOGroupList = pstAgcdObject->m_stGroup.m_pstList;

		while(pstOGroupList)
		{
			pstOGroupData = &pstOGroupList->m_csData;

			if (pstOGroupData->m_pstClump)
			{
				if (m_pcsAgcmRender)
				{
					m_pcsAgcmRender->RemoveClumpFromWorld(pstOGroupData->m_pstClump);
					m_pcsAgcmRender->AddClumpToWorld(pstOGroupData->m_pstClump);
				}
				else
				{
					RpWorldRemoveClump(m_pWorld, pstOGroupData->m_pstClump);
					RpWorldAddClump(m_pWorld, pstOGroupData->m_pstClump);
				}
			}

			pstOGroupList = pstOGroupList->m_pstNext;
		}
	}

	if(bAlpha)
	{
		pstAgcdTemplate->m_lObjectType |= ACUOBJECT_TYPE_USE_ALPHA;
	}
	else
	{
		pstAgcdTemplate->m_lObjectType &= ~ACUOBJECT_TYPE_USE_ALPHA;
	}	

	return TRUE;
}

BOOL	AgcmObject::UpdateObjectType(AgcdObjectTemplate *pstAgcdTemplate)
{
	INT32					lIndex;
	ApdObject				*pcsObject;
	AgcdObject				*pstAgcdObject;

	AgcdObjectGroupList		*pstOGroupList;
	AgcdObjectGroupData		*pstOGroupData;

	lIndex = 0;
	for (pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex); pcsObject; pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex))
	{
		if (pstAgcdTemplate != GetTemplateData(pcsObject->m_pcsTemplate))
			continue;

		pstAgcdObject = GetObjectData(pcsObject);
		pstAgcdObject = GetObjectData(pcsObject);
		pstOGroupList = pstAgcdObject->m_stGroup.m_pstList;

		while(pstOGroupList)
		{
			pstOGroupData = &pstOGroupList->m_csData;

			if (pstOGroupData->m_pstClump)
			{
				AcuObject::SetClumpType(pstOGroupData->m_pstClump, pstAgcdObject->m_lObjectType);
			}

			pstOGroupList = pstOGroupList->m_pstNext;
		}
	}

	return TRUE;
}

/*
static RpMaterial *	CBChangeAmbientMaterial(RpMaterial *pstMaterial, PVOID pvData)
{
	RpMaterialSetColor(pstMaterial, (RwRGBA *) pvData);

	return pstMaterial;
}

static RpAtomic * CBChangeAmbientAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	RpGeometryForAllMaterials(RpAtomicGetGeometry(pstAtomic), CBChangeAmbientMaterial, pvData);

	RpGeometryLock(RpAtomicGetGeometry(pstAtomic), rpGEOMETRYLOCKPRELIGHT);
	RpGeometryUnlock(RpAtomicGetGeometry(pstAtomic));

	return pstAtomic;
}

BOOL	AgcmObject::ChangeAmbient(RwRGBA *pstRGBA)
{
	INT32					lIndex;
	ApdObjectTemplate		*pcsTemplate;
	AgcdObjectTemplate		*pcsAgcdTemplate;

	lIndex = 0;
	for (pcsTemplate = m_pcsApmObject->GetObjectTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = m_pcsApmObject->GetObjectTemplateSequence(&lIndex))
	{
		pcsAgcdTemplate = GetTemplateData(pcsTemplate);

		if (pcsAgcdTemplate->m_bUseAmbient && pcsAgcdTemplate->m_pClump)
		{
			RpClumpForAllAtomics(pcsAgcdTemplate->m_pClump, CBChangeAmbientAtomic, pstRGBA);
		}
	}

	return TRUE;
}
*/

BOOL	AgcmObject::ReloadObjectClump( char * pTemplateFileName , ProgressCallback pfCallback , void * pData )
{
	static	char	strMessage[] = "오브젝트 템플릿 재로딩";

	INT32					lIndex			;

	ApdObject			*	pcsObject		;

	lIndex = 0;

	// 오브젝트 모두 제거..
	if( pfCallback ) pfCallback( "로드된 클럼프 제거" , 0 , 4 , pData );

	for (	pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject												;
			pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		AgcdObject *curGroupNode = GetObjectData(pcsObject);
		while(curGroupNode)
		{
			ReleaseObjectData(pcsObject, curGroupNode);  // Bob님이 수정(070403)
			if(curGroupNode->m_pstTemplate->m_lRefCount <= 0) // Bob님이 수정(070403)
				ReleaseObjectTemplateData(curGroupNode->m_pstTemplate);

			if(curGroupNode->m_bGroupChild)
			{
				curGroupNode = curGroupNode->m_listNext;
			}
			else
				curGroupNode = curGroupNode->m_listNext;
		}
	}

	// 오브젝트 템플릿의 클럼프 모두 제거..
	m_pcsApmObject->DestroyAllTemplate();

	if( pfCallback ) pfCallback( "로드된 클럼프 제거" , 2 , 4 , pData );
	// 오브젝트 템플릿 다시로드.
	if( m_pcsApmObject->StreamReadTemplate( pTemplateFileName , pfCallback , pData ) == FALSE ) return FALSE;

	// 오브젝트 다시 등록..

	AgcdObject	*curGroupNode	;
	
	lIndex = 0;

	if( pfCallback ) pfCallback( "없어진 템플리트의 오브젝트 정리" , 3 , 4 , pData );

	for (	pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject												;
			pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		pcsObject->m_pcsTemplate = m_pcsApmObject->GetObjectTemplate( pcsObject->m_lTID );
		if( NULL == pcsObject->m_pcsTemplate )
		{
			curGroupNode	= GetObjectData( pcsObject );
			while(curGroupNode)
			{
				curGroupNode->m_pstTemplate = NULL;
				curGroupNode = curGroupNode->m_listNext;
			}

			GroupDelete( curGroupNode );
		}
		else
		{
			curGroupNode	= GetObjectData( pcsObject );
			while(curGroupNode)
			{
				curGroupNode->m_pstTemplate = GetTemplateData( pcsObject->m_pcsTemplate );
				curGroupNode = curGroupNode->m_listNext;
			}

			InitObject( pcsObject );
		}
	}
			
	if( pfCallback ) pfCallback( "끝" , 4 , 4 , pData );

	return	TRUE;
}

/******************************************************************************
* Purpose :
*
* 041603. Bob Jung
******************************************************************************/
VOID AgcmObject::SetMaxAnimation(INT32 lMaxAnimNum)
{
	m_lMaxAnimNum = lMaxAnimNum;
}


/******************************************************************************
* Purpose :
*
* 052003. Bob Jung
******************************************************************************/
BOOL AgcmObject::SetCallbackInitObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECT_CB_ID_INIT_OBJECT, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 052003. Bob Jung
******************************************************************************/
BOOL AgcmObject::SetCallbackWorkObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECT_CB_ID_WORK_OBJECT, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 052003. Bob Jung
******************************************************************************/
BOOL AgcmObject::SetCallbackBuffObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECT_CB_ID_BUFF_OBJECT, pfCallback, pClass);
}

BOOL AgcmObject::SetCallbackLoadClump(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECT_CB_ID_LOAD_CLUMP, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 080104. BOB
******************************************************************************/
BOOL AgcmObject::SetCallbackPreRemoveData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECT_CB_ID_PRE_REMOVE_DATA, pfCallback, pClass);
}

BOOL AgcmObject::SetCallbackRidableUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECT_CB_ID_RIDABLEUPDATE, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 052003. Bob Jung
******************************************************************************/
VOID AgcmObject::ForceEnumCallback(AgcmObjectCallbackPoint ePoint, ApdObject *pcsApdObject)
{
	EnumCallback(ePoint, pcsApdObject, NULL);
}

/******************************************************************************
******************************************************************************/

BOOL AgcmObject::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8		cOperation		= (-1);
	INT32		lOID			= 0;
	INT8		cStatus			= (-1);

	m_csPacket.GetField(TRUE, pvPacket, nSize, &cOperation, &lOID, &cStatus);

	switch (cOperation)
	{
	case AGCMOBJECT_OPERATION_UPDATE:
		m_pcsApmObject->UpdateStatus(lOID, cStatus);
		break;

	default:
		break;
	}

	return TRUE;
}

BOOL AgcmObject::CBUpdateStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmObject		*pThis		= (AgcmObject *)	pClass;
	ApdObject		*pcsObject	= (ApdObject *)		pData;
	INT32			lPrevStatus	= *((INT32 *)		pCustData);

	// 오브젝트 상태가 바뀌었다. 그려줘라..... 빨리 그려줘라... 멀로 그려주냐고? 멀라.. 데~~~ 배째~~~
	//
	//
	//

	return TRUE;
}

RpAtomic *AgcmObject::LoadAtomic(CHAR *szDffName)
{
	RwStream *stream = (RwStream *)NULL;
	RpAtomic *atomic = (RpAtomic *)NULL;
	RwChar *path = (RwChar *)NULL;
	CHAR	szPath[AGCMOBJECT_MAX_PATH_CLUMP];

	sprintf(szPath, "%s%s", m_szClumpPath, szDffName);

	if (m_pcsAgcmResourceLoader)
	{
		atomic = m_pcsAgcmResourceLoader->LoadAtomic(szPath, NULL, NULL, -1, m_szTexturePath);
	}

	return atomic;
}

RpAtomic *	AgcmObject::CBGetCollisionAtomic( RpClump	*	pstClump	, PVOID pvData )
{
	ASSERT( NULL != AgcmObject::m_pThis );

	int oid;
	ApdObject		*	pstApdObject	;
	AgcdObject		*	pstAgcdObject	;

	if (!pstClump)
		return NULL;

	INT32	nType = AcuObject::GetClumpType( pstClump, &oid );

	if( AcuObject::GetType( nType ) != ACUOBJECT_TYPE_OBJECT )
		return NULL;

	pstApdObject	= AgcmObject::m_pThis->m_pcsApmObject->GetObject	( oid			);

//	ASSERT( NULL != pstApdObject	);

	if( !pstApdObject )
		return NULL;

	pstAgcdObject	= AgcmObject::m_pThis->GetObjectData				( pstApdObject	);
	ASSERT( NULL != pstAgcdObject	);

	return pstAgcdObject->m_pstCollisionAtomic;
}

RpAtomic *	AgcmObject::CBGetCollisionAtomicFromAtomic( RpAtomic	*	pstAtomic	, PVOID pvData )
{
	ASSERT( NULL != AgcmObject::m_pThis );

	int oid;
//	ApdObject		*	pstApdObject	;
//	AgcdObject		*	pstAgcdObject	;

	RpAtomic		*	pCollisionAtomic = NULL;

	INT32	nType = AcuObject::GetAtomicType( pstAtomic, &oid , NULL , NULL , NULL, ( void ** ) &pCollisionAtomic );

	return pCollisionAtomic;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Grouping Functions
BOOL	AgcmObject::GroupAdd	( AgcdObject* pObj, AuPOS& pos, AuPOS& scale,FLOAT&	xrot,FLOAT& yrot, BOOL bSetupClump)
{
	//  풀처리는 막겠음 (Parn)
	return TRUE;
	//ASSERT( pObj->m_listNext == NULL);

	ApdObject* pApdObject = ( ApdObject * ) this->m_pcsApmObject->GetParentModuleData( m_nObjectAttachIndex , pObj );
	
	AgcdObject* pAddObj = new AgcdObject;
	memset(pAddObj, 0, sizeof(AgcdObject));

	AgcdObject* curNode = pObj;
	
	while(curNode->m_listNext)
	{
		curNode = curNode->m_listNext;
	}

	curNode->m_listNext = pAddObj;
	pAddObj->m_listNext = NULL;

	pAddObj->m_bGroupChild = TRUE;

	GroupChildInfo*		AddInfo = new GroupChildInfo;

	AddInfo->m_stPosition = pos;
	AddInfo->m_stScale = scale;
	AddInfo->m_fDegreeX = xrot;
	AddInfo->m_fDegreeY = yrot;

	pAddObj->m_pInfoGroup = AddInfo;

	pAddObj->m_pstTemplate = GetTemplateData(pApdObject->m_pcsTemplate);

	if (bSetupClump)
		SetupObjectClump(pApdObject,pAddObj);

	return TRUE;
}


INT32	AgcmObject::ClumpDelete( RpClump* pClump , BOOL bForce )
{
	ApdObject	*pcsObject		;
	AgcdObject	*pstAgcdObject	;
	INT32		lIndex = 0		;

	INT32		oid;

	AgcdObjectGroupList	*pstOGroupList;
	AgcdObjectGroupData *pstOGroupData;

	for (	pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject												;
			pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		pstAgcdObject = GetObjectData(pcsObject);

		// Group Search
		AgcdObject *curGroupNode = pstAgcdObject;
		AgcdObject *beforeNode = pstAgcdObject;
		while(curGroupNode)
		{
			pstOGroupList = curGroupNode->m_stGroup.m_pstList;

			while(pstOGroupList)
			{
				pstOGroupData = &pstOGroupList->m_csData;

				if(pstOGroupData->m_pstClump == pClump)	// delete
				{
					// 마고자 (2004-07-09 오전 10:57:04) : 
					// 레퍼런스 카운트가 두번 지워지는 현상이 있다.
					// 템플릿 삭제 주석처리.
					//ReleaseObjectData(pcsObject, curGroupNode); // Bob님 수정.(070403)

					if(curGroupNode->m_bGroupChild == FALSE)		
					{
						oid	= pcsObject->m_lID;
						m_pcsApmObject->DeleteObject( pcsObject , bForce );
					}
					else
					{
						oid	= -1;
						beforeNode->m_listNext = curGroupNode->m_listNext;
						delete curGroupNode->m_pInfoGroup;
						delete curGroupNode;
					}
					
					return oid;
				}

				pstOGroupList = pstOGroupList->m_pstNext;
			}

			beforeNode = curGroupNode;
			curGroupNode = curGroupNode->m_listNext;
		}
	}

	return -2;
}

AgcdObject*		AgcmObject::GetObjectData	( RpClump * pClump )
{
	ApdObject	*pcsObject		;
	AgcdObject	*pstAgcdObject	;
	INT32		lIndex = 0		;

	if( NULL == pClump ) return NULL;

	for (	pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject												;
			pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		pstAgcdObject = GetObjectData(pcsObject);

		// Group Search
		AgcdObject *curGroupNode = pstAgcdObject;
		AgcdObject *beforeNode = pstAgcdObject;

		AgcdObjectGroupList *pstOGroupList;
		AgcdObjectGroupData *pstOGroupData;

		while(curGroupNode)
		{
			pstOGroupList = pstAgcdObject->m_stGroup.m_pstList;
			while(pstOGroupList)
			{
				pstOGroupData = &pstOGroupList->m_csData;

				if(pstOGroupData->m_pstClump == pClump)
				{
					return curGroupNode;
				}

				pstOGroupList = pstOGroupList->m_pstNext;
			}

			beforeNode = curGroupNode;
			curGroupNode = curGroupNode->m_listNext;
		}
	}

	return NULL;
}

AgcdObject*		AgcmObject::GetEnumValue ( AgcdObject *pcsObject )		// while ( data = GetEnumValue(obj)) 이런 형식으로 사용가능
{
//	ApdObject* pApdObj = ( ApdObject * ) 
//		this->m_pcsApmObject->GetParentModuleData( m_nObjectAttachIndex , pcsObject );
//
//	if( NULL == m_CurObject )
//	{
//		m_CurObject		= pApdObj	;
//		m_CurGroupNode	= pcsObject	;
//	}
//	else
//	if(	pApdObj != m_CurObject	||
//		NULL ==m_CurGroupNode	)
//	{
//		m_CurGroupNode = pcsObject;
//	}
//
//	AgcdObject*	 retval = m_CurGroupNode;
//
//	if( m_CurGroupNode )
//		m_CurGroupNode = m_CurGroupNode->m_listNext;
//
//	return retval;

	if( NULL == pcsObject )
		return NULL;
	else
		return pcsObject->m_listNext;
}

INT32	AgcmObject::GetGroupCount ( AgcdObject *pstObject )
{
	AgcdObject *curGroupNode = pstObject->m_listNext;
	INT32	count = 0;

	while(curGroupNode)
	{
		++count;
		curGroupNode = curGroupNode->m_listNext;
	}

	return	count;
}

BOOL	AgcmObject::GroupDelete ( AgcdObject *pstObject )
{
	PROFILE("AgcmObject::GroupDelete");

	INT32		lGroupCount		= 0;
	AgcdObject	*curGroupNode	= pstObject;
	ApdObject	*pstApdObject	= GetObject(pstObject);

	while(curGroupNode)
	{
		ReleaseObjectData(pstApdObject, curGroupNode); // Bob님 수정(070403)

		if(curGroupNode->m_pstTemplate && curGroupNode->m_pstTemplate->m_lRefCount <= 0)
			ReleaseObjectTemplateData(curGroupNode->m_pstTemplate); // Bob님 수정(070403)

		if(curGroupNode->m_bGroupChild)
		{
			if(curGroupNode->m_pInfoGroup)
			{
				delete curGroupNode->m_pInfoGroup;
				curGroupNode->m_pInfoGroup = NULL;
			}

			AgcdObject* removeNode = curGroupNode;
			curGroupNode = curGroupNode->m_listNext;
			delete removeNode;
		}
		else
		{
			curGroupNode = curGroupNode->m_listNext;
		}

		++lGroupCount;
	}

	// 이거 NULL로 안만들어 놓으면 죽는다.
	pstObject->m_listNext = NULL;

	return TRUE;
}

AgcdObject*		AgcmObject::FindGroupStart( FLOAT x1,FLOAT z1,FLOAT x2,FLOAT z2,UINT32	tID)
{
	ApdObject	*pcsObject		;
	AgcdObject	*pstAgcdObject	;
	INT32		lIndex = 0;

	for (	pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject												;
			pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		if(pcsObject->m_stPosition.x >= x1 && pcsObject->m_stPosition.x < x2 &&
			pcsObject->m_stPosition.z >= z1 && pcsObject->m_stPosition.z < z2
			&& pcsObject->m_lTID == ( INT32 ) tID)
		{
			pstAgcdObject = GetObjectData(pcsObject);
			return pstAgcdObject;
		}
	}

	return NULL;
}

AgcdObject*		AgcmObject::FindGroupStart( AuPOS& pos, UINT32 tID)
{
	INT32	six = PosToSectorIndexX(pos.x);
	INT32	siz = PosToSectorIndexZ(pos.z);
	FLOAT	x1 = GetSectorStartX( six );
	FLOAT	z1 = GetSectorStartZ( siz );
	FLOAT	x2 = GetSectorStartX( six + 1);
	FLOAT	z2 = GetSectorStartZ( siz + 1);

	return FindGroupStart(x1,z1,x2,z2,tID);
}

BOOL AgcmObject::LoadAllTemplateData(BOOL bUseTexDict, INT32 *plIndex, CHAR *szErrorMessage)
{
	AgcdObjectTemplate	*pcsAgcdObjectTemplate	= NULL;
	INT32				lIndex					= (plIndex) ? *(plIndex) : (0);
	for(ApdObjectTemplate *pcsApdObjectTemplate = m_pcsApmObject->GetObjectTemplateSequence(&lIndex); pcsApdObjectTemplate; pcsApdObjectTemplate = m_pcsApmObject->GetObjectTemplateSequence(&lIndex))
	{
		pcsAgcdObjectTemplate = GetTemplateData(pcsApdObjectTemplate);
		if(!pcsAgcdObjectTemplate)
		{
			if(szErrorMessage)
				sprintf(szErrorMessage, "[%d] AgcdObjectTemplate을 가져올수 없습니다.", pcsApdObjectTemplate->m_lID);
			if(plIndex)
				*(plIndex) = lIndex;

			return FALSE;
		}

		++pcsAgcdObjectTemplate->m_lRefCount;
		if(!LoadTemplateData(pcsAgcdObjectTemplate, bUseTexDict, szErrorMessage))
		{
			if(szErrorMessage)
				sprintf(szErrorMessage, "[%d] AgcdObjectTemplate을 가져올수 없습니다.", pcsApdObjectTemplate->m_lID);
			if(plIndex)
				*(plIndex) = lIndex;

			return FALSE;
		}

	}

	return TRUE;
}

BOOL AgcmObject::LoadTemplateData(AgcdObjectTemplate * pstAgcdObjectTemplate, BOOL bUseTexDict, CHAR *szErrorMessage)
{
//	INT32	lIndex;

	ASSERT( NULL != pstAgcdObjectTemplate );

	AgcdObjectTemplateGroupList *pstList = pstAgcdObjectTemplate->m_stGroup.m_pstList;
	AgcdObjectTemplateGroupData *pstData;

	ASSERT(pstAgcdObjectTemplate->m_lRefCount >= 0);

	if (pstAgcdObjectTemplate->m_lRefCount >= 1)
	{
		while (pstList)
		{
			pstData = &pstList->m_csData;

			if ((!pstData->m_pstClump) && (pstData->m_pszDFFName))
			{
				pstData->m_pstClump = LoadClump(pstData->m_pszDFFName, bUseTexDict ? pstAgcdObjectTemplate->m_szCategory : NULL);
				if ( NULL == pstData->m_pstClump ) return FALSE;

				BOOL br = EnumCallback(AGCMOBJECT_CB_ID_LOAD_CLUMP, m_pcsApmObject->GetParentModuleData(m_nObjectTemplateAttachIndex, pstAgcdObjectTemplate), m_szClumpPath);
				if( FALSE == br ) return FALSE;

				if (pstData->m_csClumpRenderType.m_lSetCount > 0)
				{
					m_pcsAgcmRender->ClumpSetRenderType(pstData->m_pstClump, &pstData->m_csClumpRenderType);
				}
			}

			if (pstData->m_pcsAnimation)
			{
				if (!m_csAnimation2.ReadRtAnim(pstData->m_pcsAnimation))
				{
					ASSERT(!"FAILED - m_csAnimation2.ReadRtAnim()");

					if (!m_csAnimation2.RemoveAllAnimation(&pstData->m_pcsAnimation))
					{
						ASSERT(!"FAILED - m_csAnimation2.RemoveAllAnimation()");
						return FALSE;
					}
				}
			}

			pstList = pstList->m_pstNext;
		}

		if (!pstAgcdObjectTemplate->m_pstCollisionAtomic && pstAgcdObjectTemplate->m_szCollisionDFFName[0])
		{
			pstAgcdObjectTemplate->m_pstCollisionAtomic = LoadAtomic(pstAgcdObjectTemplate->m_szCollisionDFFName);
			if (!pstAgcdObjectTemplate->m_pstCollisionAtomic) return FALSE;
		}

		if (!pstAgcdObjectTemplate->m_pstPickingAtomic && pstAgcdObjectTemplate->m_szPickDFFName[0])
		{
			pstAgcdObjectTemplate->m_pstPickingAtomic = LoadAtomic(pstAgcdObjectTemplate->m_szPickDFFName);
		}
	}

	pstAgcdObjectTemplate->m_eStatus = AGCDOBJECT_STATUS_LOAD_TEMPLATE;
	return TRUE;
}

BOOL AgcmObject::ReleaseObjectData(ApdObject *pstAgdObject, AgcdObject *pstAgcdObject)
{
	PROFILE("AgcmObject::ReleaseObjectData");

	if ((!pstAgdObject) || (!pstAgcdObject))
		return FALSE;

	EnumCallback(AGCMOBJECT_CB_ID_PRE_REMOVE_DATA, (PVOID)(pstAgdObject), (PVOID)(pstAgcdObject));

	if ((pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_IS_SYSTEM_OBJECT) && !m_bSetupSystemObject)
		return TRUE;

	if ((pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_IS_SYSTEM_OBJECT) && !m_bSetupNormalObject)
		return TRUE;

	{
		RwFrame *	pstFrame;

		PROFILE("AgcmObject::ReleaseObjectData - Destroy Atomics");

		if (pstAgcdObject->m_pstCollisionAtomic && !RpAtomicGetClump(pstAgcdObject->m_pstCollisionAtomic))
		{
			pstFrame = RpAtomicGetFrame(pstAgcdObject->m_pstCollisionAtomic);
			if (pstFrame)
			{
				RpAtomicSetFrame(pstAgcdObject->m_pstCollisionAtomic, NULL);
				//@{ 2006/05/03 burumal
				//RwFrameRemoveChild(pstFrame);
				if ( RwFrameGetParent(pstFrame) )
					RwFrameRemoveChild(pstFrame);
				//@}
				RwFrameDestroy(pstFrame);
			}

			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAgcdObject->m_pstCollisionAtomic);
			else
				RpAtomicDestroy(pstAgcdObject->m_pstCollisionAtomic);

			pstAgcdObject->m_pstCollisionAtomic = NULL;
		}

		if (pstAgcdObject->m_pstPickAtomic && !RpAtomicGetClump(pstAgcdObject->m_pstPickAtomic))
		{
			pstFrame = RpAtomicGetFrame(pstAgcdObject->m_pstPickAtomic);
			if (pstFrame)
			{
				RpAtomicSetFrame(pstAgcdObject->m_pstPickAtomic, NULL);
				//@{ 2006/05/03 burumal
				//RwFrameRemoveChild(pstFrame);
				if ( RwFrameGetParent(pstFrame) )
					RwFrameRemoveChild(pstFrame);
				//@}
				RwFrameDestroy(pstFrame);
			}

			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAgcdObject->m_pstPickAtomic);
			else
				RpAtomicDestroy(pstAgcdObject->m_pstPickAtomic);

			pstAgcdObject->m_pstPickAtomic = NULL;
		}
	}

	AgcdObjectGroupList *pstList = pstAgcdObject->m_stGroup.m_pstList;
	AgcdObjectGroupData *pcsData;

	while (pstList)
	{
		pcsData = &pstList->m_csData;

		if (pcsData->m_pstClump)
		{
			if (m_pWorld)
			{
				PROFILE("AgcmObject::ReleaseObjectData - RemoveWorld");

				if (m_pcsAgcmRender)
				{
					TRACE("Object - ReleaseObjectData - %x\n",pcsData->m_pstClump);
					m_pcsAgcmRender->RemoveClumpFromWorld(pcsData->m_pstClump);

					//@{ Jaewon 20040706
					// unregister the clump of the object as a shadow map receiver.
					AgcmShadowmap* pAgcmShadowmap = (AgcmShadowmap*)GetModule("AgcmShadowmap");
					if(pAgcmShadowmap)
						pAgcmShadowmap->unregisterClump(pcsData->m_pstClump);
					//@} Jaewon
			
					////@{ kday 20050513
					//#ifdef USE_MFC
					//AcuObjecWire::bGetInst().bRemClump( pcsData->m_pstClump );
					//#endif //USE_MFC
					////@} kday
				}
				else
					RpWorldRemoveClump(m_pWorld, pcsData->m_pstClump);
			}

			if ( pcsData->m_pstInHierarchy												&&
				!IsBadReadPtr( pcsData->m_pstInHierarchy , sizeof RpHAnimHierarchy )	)
			{
				PROFILE("AgcmObject::ReleaseObjectData - Destroy Hierarchy");

				if (m_pcsAgcmResourceLoader)
					m_pcsAgcmResourceLoader->AddDestroyHierarchy(pcsData->m_pstInHierarchy);
				else
					RpHAnimHierarchyDestroy(pcsData->m_pstInHierarchy);
			}

			pcsData->m_pstInHierarchy = NULL;

//			if(pcsData->m_pstCurAnim)
//			{
//				m_csObjectList.RemoveAnimObjectGroupData(pstData);
//			}

			{
				PROFILE("AgcmObject::ReleaseObjectData - Destroy Clump");

				if (m_pcsAgcmResourceLoader)
					m_pcsAgcmResourceLoader->AddDestroyClump(pcsData->m_pstClump);
				else
					RpClumpDestroy(pcsData->m_pstClump);

				pcsData->m_pstClump = NULL;
			}
		}

		pstList = pstList->m_pstNext;
	}

	{
		PROFILE("AgcmObject::ReleaseObjectData - Remove Group");

		m_csObjectList.RemoveAllObjectGroup(&pstAgcdObject->m_stGroup);
	}

	// 마고자 (2003-10-15 오전 11:17:34) : 템플릿이 널인 경우도 있다.
	if( pstAgcdObject->m_pstTemplate )
	{
		PROFILE("AgcmObject::ReleaseObjectData - Template");

		pstAgcdObject->m_pstTemplate->m_eStatus = AGCDOBJECT_STATUS_INIT;

		ASSERT( pstAgcdObject->m_pstTemplate->m_lRefCount > 0 );
		--(pstAgcdObject->m_pstTemplate->m_lRefCount);

		if (pstAgcdObject->m_pstTemplate->m_lRefCount < 1)
			ReleaseObjectTemplateData(pstAgcdObject->m_pstTemplate);

/*		if((!pstAgcdObject->m_bStop) && (pstAgcdObject->m_pstTemplate->m_pstInitAnim))
		{
			if(!m_csAnimObjectList.RemoveAnimObject(pstAgcdObject))
			{
				OutputDebugString("AgcmObject::ReleaseObjectData() Error 애니메이션 관리 실패!!!\n");
				return FALSE;
			}
		}*/
	}

	return TRUE;
}

BOOL AgcmObject::ReleaseObjectTemplateData(AgcdObjectTemplate *pstAgcdObjectTemplate)
{
	PROFILE("AgcmObject::ReleaseObjectTemplateData");

	AgcdObjectTemplateGroupList *pstList = pstAgcdObjectTemplate->m_stGroup.m_pstList;
	AgcdObjectTemplateGroupData *pstData;
//	INT32						 lIndex;

	while (pstList)
	{
		PROFILE("AgcmObject::ReleaseObjectTemplateData - 1");

		pstData = &pstList->m_csData;

		if (pstData->m_pstClump)
		{
			//AcuObject::DestroyClumpData(pstAgcdObjectTemplate->m_pstClump );RenderWare가 UD를 자동으로 지워줌
			
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyClump(pstData->m_pstClump);
			else
				RpClumpDestroy(pstData->m_pstClump);

			pstData->m_pstClump = NULL;
			
/*			for (lIndex = 0; lIndex < E_OBJECT_ANIM_TYPE_NUM; ++lIndex)
			{
				if (pstData->m_aszAnimName[lIndex])
					m_csAnimation.RemoveAnimData(pstData->m_aszAnimName[lIndex]);

				if (	(pstData->m_pastAnim[lIndex]) &&
						(pstData->m_pastAnim[lIndex]->m_ppastAnimData) &&
						(pstData->m_pastAnim[lIndex]->m_ppastAnimData[0])	)
				{
					pstData->m_pastAnim[lIndex]->m_ppastAnimData[0] = NULL;
				}
			}*/

			if (pstData->m_pcsAnimation)
				m_csAnimation2.RemoveRtAnim(pstData->m_pcsAnimation);
		}

		pstList = pstList->m_pstNext;
	}

	if (pstAgcdObjectTemplate->m_pstCollisionAtomic)
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAgcdObjectTemplate->m_pstCollisionAtomic);
		else
			RpAtomicDestroy(pstAgcdObjectTemplate->m_pstCollisionAtomic);

		pstAgcdObjectTemplate->m_pstCollisionAtomic = NULL;	
	}

	if (pstAgcdObjectTemplate->m_pstPickingAtomic)
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAgcdObjectTemplate->m_pstPickingAtomic);
		else
			RpAtomicDestroy(pstAgcdObjectTemplate->m_pstPickingAtomic);

		pstAgcdObjectTemplate->m_pstPickingAtomic = NULL;
	}

	return TRUE;
}

/*
BOOL AgcmObject::LoadAllTemplateClump(BOOL bUseTexDict)
{
	AgcdObjectTemplate	*pstAgcdObjectTemplate;
	INT32				lIndex = 0;

	AgcdObjectTemplateGroupList *pstList;

	for(ApdObjectTemplate *pstApdObjectTemplate = m_pcsApmObject->GetObjectTemplateSequence(&lIndex);
		pstApdObjectTemplate;
		pstApdObjectTemplate = m_pcsApmObject->GetObjectTemplateSequence(&lIndex)						)
	{
		pstAgcdObjectTemplate = GetTemplateData(pstApdObjectTemplate);
		if(!pstAgcdObjectTemplate)
			return FALSE;

		pstList = pstAgcdObjectTemplate->m_stGroup.m_pstList;

		while(pstList)
		{
			VERIFY(pstList->m_csData.m_pstClump = LoadClump(pstList->m_csData.m_szDFFName, bUseTexDict ? pstAgcdObjectTemplate->m_szCategory : NULL));
			pstList = pstList->m_pstNext;
		}

		VERIFY(pstAgcdObjectTemplate->m_pstCollisionAtomic = LoadAtomic(pstAgcdObjectTemplate->m_szCollisionDFFName));
	}

	return TRUE;
}
*/

VOID AgcmObject::RefreshObjects(INT32 lTID)
{
	ApdObject *			pstApdObject;
	AgcdObject *		pstAgcdObject;
	INT32				lIndex = 0;

	for(pstApdObject = m_pcsApmObject->GetObjectSequence(&lIndex); pstApdObject; pstApdObject = m_pcsApmObject->GetObjectSequence(&lIndex))
	{
		if (pstApdObject->m_lTID == lTID)
		{
			pstAgcdObject = GetObjectData(pstApdObject);

			ReleaseObjectData(pstApdObject, pstAgcdObject);
			SetupObjectClump(pstApdObject);
		}
	}
}

/*AgcdAnimation *AgcmObject::GetAnimation(INT32 lTID, INT32 lIndex, INT32 lType, BOOL bAdd)
{
	CHAR			szTemp[AGCMOBJECT_MAX_STR];

	sprintf(szTemp, "%d_%d_%d_%s", lTID, lIndex, lType, AGCMOBJECT_GET_ANIM_NAME);

	return  m_csAnimation.GetAnimation(szTemp, bAdd);
}

AgcdAnimData *AgcmObject::GetAnimData(CHAR *szName, CHAR *szPath)
{
	return m_csAnimation.GetAnimData(szName, szPath);
}

RtAnimInterpolator *AgcmObject::OnAnimEndCB(RtAnimInterpolator *animInstance, PVOID pvData)
{
	AgcdObjectGroupData *pcsData = (AgcdObjectGroupData *)(pvData);
	AgcmObject			*pcsThis = (AgcmObject *)(pcsData->m_pvThis);

	if(pcsData->m_pstDefaultAnim)
		pcsThis->SetAnimation(pcsData, pcsData->m_pstDefaultAnim, pcsData->m_eDefaultAnimType);
	else
		pcsThis->StopAnimation(pcsData);		

	return animInstance;
}

RtAnimInterpolator *AgcmObject::OnAnimEndThenChangeCB(RtAnimInterpolator *animInstance, PVOID pvData)
{
	AgcdObjectGroupData *pcsData = (AgcdObjectGroupData *)(pvData);
	AgcmObject			*pcsThis = (AgcmObject *)(pcsData->m_pvThis);

	if(pcsData->m_pstNextAnim)
		pcsThis->SetAnimation(pcsData, pcsData->m_pstNextAnim, pcsData->m_eNextAnimType);
	else
		pcsThis->StopAnimation(pcsData);

	return animInstance;
}

BOOL AgcmObject::StopAnimation(AgcdObjectGroupData *pcsData)
{
	pcsData->m_bStop = TRUE;

//	m_csObjectList.RemoveAnimObjectGroupData(pcsData);

	return TRUE;
}

BOOL AgcmObject::DropAnimation(AgcdObjectGroupData *pcsData)
{
	pcsData->m_bStop			= TRUE;
	pcsData->m_pstCurAnim		= NULL;
	pcsData->m_pstNextAnim		= NULL;
	pcsData->m_eCurAnimType		= E_OBJECT_ANIM_TYPE_NONE;	
	pcsData->m_eNextAnimType	= E_OBJECT_ANIM_TYPE_NONE;

	RpClumpForAllAtomics(pcsData->m_pstClump, AgcuObject_SetHierarchyForSkinAtomic, NULL);

//	m_csObjectList.RemoveAnimObjectGroupData(pcsData);

	return TRUE;
}*/

BOOL AgcmObject::SetAnimation(AgcdObjectGroupData *pcsData, RtAnimAnimation *pstAnim)
{
	if (!pcsData->m_pstInHierarchy)
		return FALSE;

	RpHAnimHierarchySetCurrentAnim(pcsData->m_pstInHierarchy, pstAnim);
	RpHAnimHierarchySetCurrentAnimTime(pcsData->m_pstInHierarchy, 0.0f);

	LockFrame();
	RpClumpForAllAtomics(pcsData->m_pstClump, AgcuObject_SetHierarchyForSkinAtomic, (void *)(pcsData->m_pstInHierarchy));
	RpHAnimUpdateHierarchyMatrices(pcsData->m_pstInHierarchy);
	UnlockFrame();

	RpHAnimHierarchySetAnimLoopCallBack(pcsData->m_pstInHierarchy, NULL, NULL);

	pcsData->m_bStopAnimation	= FALSE;

	return TRUE;
}
/*BOOL AgcmObject::SetAnimation(AgcdObjectGroupData *pcsData, AgcdAnimation *pstAnim, eObjectAnimationType eType)
{
	// 마고자 (2004-05-31 오후 4:30:23) : 맵툴 에서 카메라 돌리다가 죽는 문제가 있어서
	// 숭환씨랑 상의로 널체크 추가.
	if ((!pcsData->m_pstInHierarchy) || (!pstAnim) || (!pstAnim->m_ppastAnimData) || (!pstAnim->m_ppastAnimData[0]))
		return FALSE;
	
	RpHAnimHierarchySetCurrentAnim(pcsData->m_pstInHierarchy, pstAnim->m_ppastAnimData[0]->m_pstAnim);
	RpHAnimHierarchySetCurrentAnimTime(pcsData->m_pstInHierarchy, 0.0f);

	LockFrame();

	RpClumpForAllAtomics(pcsData->m_pstClump, AgcuObject_SetHierarchyForSkinAtomic, (void *)(pcsData->m_pstInHierarchy));
	RpHAnimUpdateHierarchyMatrices(pcsData->m_pstInHierarchy);

	UnlockFrame();

	AgcdObjectAnimationAttachedData	*pstObjAnimAttachedData	=
			(AgcdObjectAnimationAttachedData *)(m_csAnimation.GetAttachedData(
			pstAnim,
			AGCDOBJECT_ANIMATION_ATTACHED_DATA_KEY								)	);

//	if (pstAnim->m_stAnimFlag.m_ulAnimFlag & AGCD_ANIMATION_FLAG_LOOP)
	if ((pstObjAnimAttachedData) && (pstObjAnimAttachedData->m_stAnimFlag.m_ulAnimFlag & AGCD_ANIMATION_FLAG_LOOP))
	{
		RpHAnimHierarchySetAnimLoopCallBack(pcsData->m_pstInHierarchy, NULL, NULL);
	}
	else
	{
		RpHAnimHierarchySetAnimLoopCallBack(pcsData->m_pstInHierarchy, OnAnimEndCB, (PVOID)(pcsData));
	}

	pcsData->m_bStop			= FALSE;
	pcsData->m_pstCurAnim		= pstAnim;	
	pcsData->m_eCurAnimType		= eType;
	pcsData->m_pstNextAnim		= NULL;
	pcsData->m_eNextAnimType	= E_OBJECT_ANIM_TYPE_NONE;

	return TRUE;
}*/

/*BOOL AgcmObject::StartAnimation(AgcdObject *pstAgcdObject, eObjectAnimationType eAnimType, BOOL bEndThenChange)
{
	// 템플릿이 없으면 안된다!
	AgcdObjectTemplate *pstAgcdObjectTemplate = pstAgcdObject->m_pstTemplate;
	if(!pstAgcdObjectTemplate)
		return FALSE;

	
	AgcdObjectGroupData			*pstOData;
	AgcdObjectTemplateGroupData	*pstTData;

	AgcdObjectGroupList *pstList = pstAgcdObject->m_stGroup.m_pstList;
	while(pstList)
	{
		pstOData = &pstList->m_csData;
		pstTData = m_csObjectList.GetObjectTemplateGroup(&pstAgcdObjectTemplate->m_stGroup, pstList->m_csData.m_lIndex);

		if(	(pstOData->m_pstClump) &&
			(pstTData) &&
			(pstTData->m_pastAnim[eAnimType]) &&
			(pstTData->m_pastAnim[eAnimType]->m_ppastAnimData) &&
			(pstTData->m_pastAnim[eAnimType]->m_ppastAnimData[0])	)
		{
			if((pstOData->m_pstCurAnim) && (bEndThenChange))
			{
				// 현재 같은 애니메이션이다.
				if(pstOData->m_pstCurAnim->m_ppastAnimData[0] == pstTData->m_pastAnim[eAnimType]->m_ppastAnimData[0])
					return TRUE;

				pstOData->m_pstNextAnim		= pstTData->m_pastAnim[eAnimType];
				pstOData->m_eNextAnimType	= eAnimType;
				RpHAnimHierarchySetAnimLoopCallBack(pstOData->m_pstInHierarchy, OnAnimEndThenChangeCB, (PVOID)(pstOData));
			}
			else
			{
				SetAnimation(pstOData, pstTData->m_pastAnim[eAnimType], eAnimType);
			}
		}

		pstList = pstList->m_pstNext;
	}

	return TRUE;
}
*/
BOOL AgcmObject::CBUpdateAnimation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject			*pcsThis	= (AgcmObject *)(pClass);
	AgcdObjectGroupData *pcsData	= (AgcdObjectGroupData *)(pCustData);

	// 혹시 있을지 모르니깐 한번 더 체크한다.
/*	if((pcsData->m_bStop) || (!pcsData->m_pstCurAnim))
	{
		return TRUE;
	}*/
	if (pcsData->m_bStopAnimation)
		return TRUE;

	pcsThis->LockFrame();

	if(pcsData->m_ulPrevTick == 0)
	{
		RpHAnimHierarchyAddAnimTime(pcsData->m_pstInHierarchy, 0.0f);
		RpHAnimHierarchyUpdateMatrices(pcsData->m_pstInHierarchy);
	}
	else
	{
//		RwReal fOffset	= (RwReal)(1000.0f / pcsData->m_afAnimSpeed[pcsData->m_eCurAnimType]);
		RwReal fOffset	= (RwReal)(1000.0f / pcsData->m_fAnimSpeed);
		RwReal fAlpha	= (	(AgcmObject::m_pThis->m_ulCurTick - pcsData->m_ulPrevTick) + 0.0f	) / fOffset;

		RpHAnimHierarchyAddAnimTime(pcsData->m_pstInHierarchy, fAlpha);
		RpHAnimHierarchyUpdateMatrices(pcsData->m_pstInHierarchy);
	}

	//@{ kday 20050822
	// ;)
	if(pcsData->m_pstClump)
		RwFrameUpdateObjects( RpClumpGetFrame(pcsData->m_pstClump) );
	//@} kday

	pcsThis->UnlockFrame();

	pcsData->m_ulPrevTick = AgcmObject::m_pThis->m_ulCurTick;

	return TRUE;
}

/*BOOL AgcmObject::UpdateAnimation(AgcdObjectGroupData *pcsData, UINT32 ulDeltaTime)
{
	if((pcsData->m_bStop) || (!pcsData->m_pstCurAnim))
		return m_csObjectList.RemoveAnimObjectGroupData(pcsData);

	RwReal fOffset	= (RwReal)(1000.0f / pcsData->m_afAnimSpeed[pcsData->m_eCurAnimType]);
	RwReal fAlpha	= (ulDeltaTime + 0.0f) / fOffset;

	RpHAnimHierarchyAddAnimTime(pcsData->m_pstInHierarchy, fAlpha);
	RpHAnimHierarchyUpdateMatrices(pcsData->m_pstInHierarchy);

	return TRUE;
}*/

BOOL AgcmObject::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmObejct::OnIdle");

/*	// Update animation
	AgcdAnimObjectGroupDataList	*pstList		= m_csObjectList.GetObjectGroupDataAnimList();
	INT32*  lDi;
	while(pstList)
	{
		lDi = AcuObject::GetClumpDInfo(pstList->m_pcsData->m_pstClump);
		
		if(lDi[DI_DISTANCETICK] == ( INT32 ) m_ulPrevTime)
		{
			if(pstList->m_pcsData)
			{
//				UpdateAnimation(pstList->m_pcsData, ulClockCount - pstList->m_ulPrevTick);
			}
			else
			{
				OutputDebugString("AgcmObject::OnIdle() Error (1) !!!\n");
				return FALSE;
			}

			pstList->m_ulPrevTick		= ulClockCount;
		}
		
		pstList						= pstList->m_pstNext;
	}

	m_ulPrevTime = ulClockCount;
*/
	m_ulCurTick = ulClockCount;

	// 2005/01/27
	// ::CBLoadMap 에서 하던부분이 백스레드에서 도는 경우가 있어서..
	// 아이들로 옮김..
	// 그러면서 뮤텍스 적용.
	SectorRidableObjectIndexUpdate();

	ObjectDeleteIdleProcess();

	return TRUE;
}

void	AgcmObject::SectorRidableObjectIndexUpdate()
{
	//STOPWATCH2TIME( _T("Object") , _T("SectorRidableObjectIndexUpdate") , 100 );
	// 오브젝트 인덱스 업데이트.
	ApWorldSector * pSector;
	if( m_listSectorObjectIndexUpdate.GetHeadNode() )
	{
		// 뮤텍스 지정..
		m_MutexlistSectorObjectIndexUpdate.Lock		();

		while( m_listSectorObjectIndexUpdate.GetHeadNode() )
		{
			pSector = m_listSectorObjectIndexUpdate.GetHeadNode()->GetData();

			SectorObjectIndexUpdate( pSector );

			m_listSectorObjectIndexUpdate.RemoveHead();
		}

		m_MutexlistSectorObjectIndexUpdate.Unlock	();
	}
}

AgcmObject::DeleteObjectQueue	*	AgcmObject::GetDeleteObjectQueue()
{
	vector< DeleteObjectQueue >::iterator Iter;
	for ( Iter = m_vecDeleteObjectQueue.begin( ) ; Iter != m_vecDeleteObjectQueue.end( ) ; Iter++ )
	{
		DeleteObjectQueue	*pQueue = &(* Iter );

		if( !pQueue->IsProcessed() )
		{
			return pQueue;
		}
	}

	return NULL;
}

BOOL	AgcmObject::WaitForQueueFlush()
{
	if( this->m_pcsAgcmResourceLoader->IsBackThread() )
	{
		INT32	nRepeatMax = 1024;
		while( nRepeatMax-- )
		{
			BOOL	bRet = IsDeleteObjectQueueAvailable();
			if( bRet )
			{
				// 한틱 기다릴수 있을까.
				Sleep( 1 );
			}
			else
			{
				// 끈나따.
				return TRUE;
			}
		};

		MD_SetErrorMessage( "아이들타임처리를 기다리지 못했음" );
		return FALSE;
	}
	else
	{
		// 메인스레드면 걍 날려버림.
		ObjectDeleteIdleProcess();
		return TRUE;
	}
}

BOOL AgcmObject::CBLoadMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject *	pThis = (AgcmObject *) pClass;
	ApWorldSector *	pSector = (ApWorldSector *) pData;
	INT32			lSectorType = (INT32) pCustData;

	if (pSector &&
		pThis->m_bAutoLoad &&
		lSectorType == SECTOR_LOWDETAIL &&
		pThis->m_pcsApmObject)
	{
		{
			// 이미 로딩 되어있으면..
			// 혹시나 삭제 리스트에 올라가 있을 지 모르니 삭제를 때림..
			// pThis->RemoveDeleteObjectQueue( pSector );
			pThis->WaitForQueueFlush();
		}

		{
			pSector->SetLoadingFlag( ApWorldSector::LF_OBJECT_LOADED , TRUE );

			pThis->m_pcsApmObject->StreamRead( pSector );
			
			// Height Pool을 날려버린다.. 
			// 마고자 (2003-11-16 오후 10:01:55) : 
			pSector->FreeHeightPool();

			if( pThis->m_pcsAgcmResourceLoader->m_bForceImmediate )
			{
				pThis->SectorObjectIndexUpdate( pSector );
			}
			else
			{
				// 리스트 추가...
				pThis->m_MutexlistSectorObjectIndexUpdate.Lock		();
				pThis->m_listSectorObjectIndexUpdate.AddTail( pSector );
				pThis->m_MutexlistSectorObjectIndexUpdate.Unlock	();
			}

			// 오브젝트 로딩이 끝났음을 알려줌..
			pThis->EnumCallback( AGCMOBJECT_CB_ID_POST_LOADMAP , ( void * ) pSector , NULL );

			if( pSector->GetFlag() & ApWorldSector::OP_HEIGHTUPDATEAFTERLOAD )
			{
				// 마고자 2006/12/26
				// 업데이트 플래그가 붙어있으면 라이더블 업데이트 한번더 때려줌
				pSector->SetFlag( pSector->GetFlag() & (~ApWorldSector::OP_HEIGHTUPDATEAFTERLOAD) );
				pThis->EnumCallback(AGCMOBJECT_CB_ID_RIDABLEUPDATE, (PVOID) ( pSector ) , NULL );
			}
		}
	}

	return TRUE;
}

void	AgcmObject::SectorObjectIndexUpdate( ApWorldSector * pSector )
{
	// 갸갸 ..
	// 섹터 넘어가는 오브젝트에 대해 다시 삽입과정.

	AuNode< AgcmObject::RidableOverlapping >	* pNode = m_listRidableObjectlapping.GetHeadNode();
	AgcmObject::RidableOverlapping				* pRidable;
	while( pNode )
	{
		pRidable	= &pNode->GetData();

		if( pRidable->uFlag & AgcmObject::RidableOverlapping::BLOCKING )
		{
			m_pcsAgcmMap->SetSectorIndexInsertBBox( ApWorldSector::AWS_COLLISIONOBJECT , pSector , pRidable->nObjectIndex , &pRidable->stBBox );
		}
		
		if( pRidable->uFlag & AgcmObject::RidableOverlapping::RIDABLE )
		{
			m_pcsAgcmMap->SetSectorIndexInsertBBox( ApWorldSector::AWS_RIDABLEOBJECT , pSector , pRidable->nObjectIndex , &pRidable->stBBox );
		}

		pNode = pNode->GetNextNode();
	}
}

void	AgcmObject::ObjectDeleteIdleProcess()
{
	//STOPWATCH2TIME( _T("Object") , _T("ObjectDeleteIdleProcess") , 100 );
	AuAutoLock			csLock2( m_pcsAgcmResourceLoader->m_csMutexRemoveResource );

	do
	{
		DeleteObjectQueue * pQueue;
		m_MutexDeleteObjectQueueAccess.Lock();
		pQueue = GetDeleteObjectQueue();
		if( pQueue )
		{
			DeleteObjectQueue	queue = * pQueue;
			pQueue->CheckProcessd();
			m_MutexDeleteObjectQueueAccess.Unlock();

			{
				ApWorldSector * pSector = m_pcsApmMap->GetSector( queue.nIndexX , 0 , queue.nIndexZ );

				if( NULL == pSector ) // || !pSector->GetLoadingFlag( ApWorldSector::LF_OBJECT_LOADED ) )
				{
					ApdObject **ppcsObject;
					
					vector< INT32 >::iterator IterObject;
					for ( IterObject = queue.vecObject.begin( ) ; IterObject != queue.vecObject.end( ) ; IterObject++ )
					{
						ppcsObject = (ApdObject **) m_pcsApmObject->m_clObjects.GetObject(*IterObject);

						if(ppcsObject)
						{
							m_pcsApmObject->DeleteObject(*ppcsObject, TRUE, FALSE);
						}
					}
				}
				else
				{
					m_pcsApmObject->DeleteObject( pSector, TRUE );
				}
			}
		}
		else
		{
			m_MutexDeleteObjectQueueAccess.Unlock();
			break;
		}
	}while(1);
}

INT32	AgcmObject::AddDeleteObjectQueue( ApWorldSector * pSector )
{
	vector< DeleteObjectQueue >::iterator	Iter;
	for ( Iter = m_vecDeleteObjectQueue.begin( ) ; Iter != m_vecDeleteObjectQueue.end( ) ; Iter++ )
	{
		DeleteObjectQueue	*pQueue = &(* Iter );

		if( pQueue->IsSameSector( pSector ) )
		{
			// 중복
			return m_vecDeleteObjectQueue.size();
		}
	}

	DeleteObjectQueue	newQueue;
	newQueue.SetSector( pSector );
	m_MutexDeleteObjectQueueAccess.Lock();
	m_vecDeleteObjectQueue.push_back( newQueue );

	// 처리됀 리스트 제거.
	//STOPWATCH2TIME( _T("Object") , _T("AddDeleteObjectQueue") , 100 );

	BOOL bFound = FALSE;
	do
	{
		bFound = FALSE;
		for ( Iter = m_vecDeleteObjectQueue.begin( ) ; Iter != m_vecDeleteObjectQueue.end( ) ; Iter++ )
		{
			DeleteObjectQueue	*pQueue = &(* Iter );

			if( pQueue->IsProcessed() )
			{
				bFound = TRUE;
				break;
			}
		}
		if( bFound ) m_vecDeleteObjectQueue.erase( Iter );
	}while( bFound );

	m_MutexDeleteObjectQueueAccess.Unlock();

	return m_vecDeleteObjectQueue.size();
}

BOOL AgcmObject::CBUnLoadMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmObject::CBUnLoadMap");

	AgcmObject *	pThis = (AgcmObject *) pClass;
	ApWorldSector *	pSector = (ApWorldSector *) pData;
	INT32			lSectorType = (INT32) pCustData;

	if (pThis->m_bAutoLoad && 
		lSectorType == SECTOR_LOWDETAIL && 
		pThis->m_pcsApmObject &&
		pSector )
	{
		{
			// Client에서는 Static Object가 더이상 필요 없다... 필요 있다면 다시 bForce를 FALSE로 (Parn)
			// pThis->m_pcsApmObject->DeleteObject( pSector, TRUE );
			pThis->AddDeleteObjectQueue( pSector );
			pSector->FreeHeightPool();

			// 로드 돼어 있을때만..
		}
	}

	return TRUE;
}

BOOL			AgcmObject::ObjectExport	( int x1 , int z1 , int x2 , int z2 , char * pDestinationDirectory )
{
	ASSERT( NULL != m_pcsApmObject );
	if( NULL == m_pcsApmObject ) return FALSE;

	if( pDestinationDirectory )
	{
		char strFullPath[ 1024 ];

		wsprintf( strFullPath , "%s\\Ini" , pDestinationDirectory );
		CreateDirectory( pDestinationDirectory	, NULL );
		CreateDirectory( strFullPath			, NULL );
	}

	// 오브젝트 익스포팅..
	int lzend = z2;
	int lxend = x2;

	for( int z = z1 ; z < lzend ; ++z )
	{
		for( int x = x1 ; x < lxend ; ++x )
		{
			TRACE( "작업중.. %d,%d\n" , x , z );

			char	strFileName[ 256 ] , strFullPath[ 1024 ];
			wsprintf( strFileName , APMOBJECT_INI_FILE_NAME , 
				GetDivisionIndex( x , z ) );

			if( pDestinationDirectory )
			{
				wsprintf( strFullPath , "%s\\%s%s" , pDestinationDirectory , "Ini\\" , strFileName );
			}
			else
			{
				wsprintf( strFullPath , "%s%s" , "Ini\\" , strFileName );
			}

			// BBox 만들기..
			AuBOX	bbox;
			bbox.inf.x	= GetSectorStartX	( ArrayIndexToSectorIndexX( x		) );
			bbox.inf.y	= 0.0f					;
			bbox.inf.z	= GetSectorStartZ	( ArrayIndexToSectorIndexZ( z		) );

			bbox.sup.x	= GetSectorEndX		( ArrayIndexToSectorIndexX( x		) );
			bbox.sup.y	= 0.0f					;
			bbox.sup.z	= GetSectorEndZ		( ArrayIndexToSectorIndexZ( z		) );

			m_pcsApmObject->StreamWrite( bbox ,
				(z % MAP_DEFAULT_DEPTH) * MAP_DEFAULT_DEPTH + 
				(x % MAP_DEFAULT_DEPTH) , strFullPath );
		}//for( int z = GetLoadRangeZ1() ; z < GetLoadRangeZ2() ; z += 10 )
	}//for( int x = GetLoadRangeX1() ; x < GetLoadRangeX2() ; x += 10 )

	return TRUE;
}

BOOL AgcmObject::CBEventObjectOnly(PVOID pData, PVOID pClass, PVOID pCustData)
{
	static ApmEventManager *	pEventManager	= ( ApmEventManager * ) AGCMMAP_THIS->GetModule( "ApmEventManager" );
	ASSERT( pEventManager );
	
	ApdObject *			pcsApdObject = ( ApdObject * ) pData;

	// 마고자 (2004-07-05 오후 5:23:48) : 이벤트 붙은 것만 처리.
	if( pEventManager->CheckEventAvailable( pcsApdObject ) )
		return TRUE;

	return FALSE;
}

BOOL			AgcmObject::ServerExport	( INT32 nDivision , char * pDestinationDirectory )
{
	ASSERT( NULL != m_pcsApmObject );
	if( NULL == m_pcsApmObject ) return FALSE;

	if( pDestinationDirectory )
	{
		char strFullPath[ 1024 ];

		wsprintf( strFullPath , "%s\\Ini" , pDestinationDirectory );
		CreateDirectory( pDestinationDirectory	, NULL );
		CreateDirectory( strFullPath			, NULL );
	}

	// 오브젝트 익스포팅..
	char	strFileName[ 256 ] , strFullPath[ 1024 ];
	wsprintf( strFileName , APMOBJECT_INI_SERVER_FILE_NAME , nDivision );

	if( pDestinationDirectory )
	{
		wsprintf( strFullPath , "%s\\%s%s" , pDestinationDirectory , "Ini\\" , strFileName );
	}
	else
	{
		wsprintf( strFullPath , "%s%s" , "Ini\\" , strFileName );
	}

	int nX , nZ;
	nX		= GetFirstSectorXInDivision( nDivision );
	nZ		= GetFirstSectorZInDivision( nDivision );

	FLOAT fStartX , fStartZ , fWidth;

	fStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( nX ) );
	fStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexZ( nZ ) );
	fWidth	= MAP_SECTOR_WIDTH * 16;

	AuBOX	bbox;
	bbox.inf.x	= fStartX				;
	bbox.inf.y	= 0.0f					;
	bbox.inf.z	= fStartZ				;

	bbox.sup.x	= fStartX + fWidth		;
	bbox.sup.y	= 0.0f					;
	bbox.sup.z	= fStartZ + fWidth		;

	m_pcsApmObject->StreamWrite( bbox ,	0 , strFullPath , CBEventObjectOnly);

	return TRUE;
}

BOOL	AgcmObject::CBLoaderTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject *	pThis			= (AgcmObject *) pClass;
	ApdObject *		pcsObject		= (ApdObject *) pData;
	AgcdObject *	pstAgcdObject	= (AgcdObject *) pCustData;

	if (pstAgcdObject->m_lStatus & AGCDOBJECT_STATUS_REMOVED)
	{
		return FALSE;
	}

	if (!pThis->LoadTemplateData(pstAgcdObject->m_pstTemplate))
	{
		return FALSE;
	}

	if (pstAgcdObject->m_lStatus & AGCDOBJECT_STATUS_REMOVED)
	{
		return FALSE;
	}

	pstAgcdObject->m_lStatus |= AGCDOBJECT_STATUS_LOAD_TEMPLATE;

	pThis->SetupObjectClump1(pcsObject, pstAgcdObject);

	return TRUE;
}
BOOL	AgcmObject::CBLoaderInit(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject *	pThis			= (AgcmObject *) pClass;
	ApdObject *		pcsObject		= (ApdObject *) pData;
	AgcdObject *	pstAgcdObject	= (AgcdObject *) pCustData;

	if (pstAgcdObject->m_lStatus & AGCDOBJECT_STATUS_REMOVED)
		return FALSE;

	pstAgcdObject	= (AgcdObject *) pThis->GetObjectData(pcsObject);

	pThis->SetupObjectClump2(pcsObject, pstAgcdObject);

	return TRUE;
}

struct	BoundingSphereCalcStruct
{
	RwSphere	sphere;
	FLOAT		fScale;
};




BOOL	AgcmObject::CBOnAddObject	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject		*	pCmObject		= ( AgcmObject		* ) pClass	;
	ApdObject		*	pcsApdObject	= ( ApdObject		* ) pData	;
	ApWorldSector	*	pSector			= NULL							;
	RwBBox		bbox;

	ASSERT( NULL != pCmObject		);
	ASSERT( NULL != pcsApdObject	);

	if (!pCmObject->m_pcsAgcmMap || !pCmObject->m_pcsApmMap)
	{
		//		ASSERT( !"AgcmObject::CBOnAddObject 맵모듈 또는 오브젝트 모듈이 추가돼지 않았음!" );
		//
		//		// 마고자 (2003-12-17 오후 4:19:01) : 어째서 이게 리턴 TRUE로 돼어있지? -_-

		// 툴에서는 MAP을 사용하지 않는 경우가 있으니까.
		// return TRUE를 때린다.
		// 마고자 (2003-12-23 오후 4:00:31) : 승환씨란 상의로 바꿔둠.

		TRACE( "AgcmObject::CBOnAddObject 맵모듈이 없어서 이쪽 관련 처리를 하지 않습니다.\n" );
		return TRUE;	// 강제로 리턴 TRUE.
	}

	// 자자자 한판 후드려패볼까나..

	AgcdObject		*	pcsAgcdObject	= pCmObject->GetObjectData( pcsApdObject );
	ASSERT( NULL != pcsAgcdObject );
	if( NULL == pcsAgcdObject )
	{
		// 클리언트 오브젝트 정보가없음..
		return TRUE;;
	}

	pSector	= pCmObject->m_pcsApmMap->GetSector( pcsApdObject->m_stPosition.x , pcsApdObject->m_stPosition.z );
	if( NULL == pSector )
	{
		// 섹터가 없네..
		// return FALSE;

		return TRUE; // 일단 임시로..
	}

	ApAutoWriterLock	csLock( pSector->m_RWLock );

	// 섹터가 넘어서 오브젝트가 있는경우
	BOOL		bObjectOverSectorRidable	= FALSE;
	BOOL		bObjectOverSectorBlocking	= FALSE;

	if( AcuObject::GetProperty( pcsAgcdObject->m_lObjectType ) & ACUOBJECT_TYPE_BLOCKING )
	{
		// 2005/01/15
		// 천장이 왜 블러킹이지 -_-..
		// 일단 하드코딩으로 스킵시킴..
		if( pcsApdObject->m_lTID == 1065 ||
			pcsApdObject->m_lTID == 1065 ||
			pcsApdObject->m_lTID == 1065 )
		{
			goto EndofBlockingCheck;
		}

		// 부딧하는것만 따로 블러킹 정보를 설정한다.

		// 블러킹 범위 작성..

		AgcdObjectGroupList		*pstList = pcsAgcdObject->m_stGroup.m_pstList;

		if( pcsAgcdObject->m_pstCollisionAtomic )
		{
			/*
			// 강제로 한번 호출해줌..
			vector< ApWorldSector * >	vectorSector;
			pCmObject->m_pcsAgcmMap->GetSectorList( pcsAgcdObject->m_pstCollisionAtomic , & vectorSector );

			// Set Blocking...

			ApWorldSector * pSectorTmp;
			for( int i = 0 ; i < ( int ) vectorSector.size() ; i ++ )
			{
				pSectorTmp = vectorSector[ i ];

				pCmObject->m_pcsAgcmMap->SetSectorIndexInsertBBox(
					ApWorldSector::AWS_COLLISIONOBJECT ,
					pSectorTmp			,
					pcsApdObject->m_lID	,
					&bbox				);
			}

			if( vectorSector.size() ) bObjectOverSectorBlocking = TRUE;
			*/

			// 강제로 한번 호출해줌..
			AgcmMap::GetBBoxCallback( pcsAgcdObject->m_pstCollisionAtomic , ( PVOID ) & bbox );

			// Set Blocking...
			//bObjectOverSectorBlocking |= pCmObject->m_pcsAgcmMap->SetSectorIndexInsert( ApWorldSector::AWS_COLLISIONOBJECT , pSector , pcsApdObject->m_lID , &bscs.sphere );

			// 범위를 조사함..
			INT32	uSectorX1 , uSectorX2 , uSectorZ1 , uSectorZ2;
			AuPOS	pos;
			pos.y	= 0.0f;
			pos.x	= bbox.inf.x < bbox.sup.x ? bbox.inf.x : bbox.sup.x;
			pos.z	= bbox.inf.z < bbox.sup.z ? bbox.inf.z : bbox.sup.z;
			pCmObject->m_pcsApmMap->GetSector( pos , &uSectorX1 , NULL , &uSectorZ1 );
			pos.x	= bbox.inf.x < bbox.sup.x ? bbox.sup.x : bbox.inf.x;
			pos.z	= bbox.inf.z < bbox.sup.z ? bbox.sup.z : bbox.inf.z;
			pCmObject->m_pcsApmMap->GetSector( pos , &uSectorX2 , NULL , &uSectorZ2 );

			INT32 sx , sz;
			ApWorldSector * pSectorTmp;
			int nCount = 0;
			for( sz = uSectorZ1 ; sz <= uSectorZ2 ; sz ++ )
			{
				for( sx = uSectorX1 ; sx <= uSectorX2 ; sx ++ )
				{
					nCount ++;

					pSectorTmp = pCmObject->m_pcsApmMap->GetSector( sx , sz );

					if( pSectorTmp )
					{
						pCmObject->m_pcsAgcmMap->SetSectorIndexInsertBBox(
							ApWorldSector::AWS_COLLISIONOBJECT ,
							pSectorTmp			,
							pcsApdObject->m_lID	,
							&bbox				);
					}
				}
			}

			if( nCount > 1 ) bObjectOverSectorBlocking = TRUE;
		}
	}

// Goto 용 마킹
EndofBlockingCheck:

	// 넉넉히..
	#define __MAX_FOUND_SECTOR	256
	ApWorldSector *	aSectorsNeededToBeUpdate[ __MAX_FOUND_SECTOR ];
	INT32			nSectorsNeededToBeUpdate = 0;

	if( AcuObject::GetProperty( pcsAgcdObject->m_lObjectType ) & ACUOBJECT_TYPE_RIDABLE )
	{
		AgcdObjectGroupList		*pstList = pcsAgcdObject->m_stGroup.m_pstList;

		if( pcsAgcdObject->m_pstCollisionAtomic )
		{
			/*
			// 강제로 한번 호출해줌..
			vector< ApWorldSector * >	vectorSector;
			pCmObject->m_pcsAgcmMap->GetSectorList( pcsAgcdObject->m_pstCollisionAtomic , & vectorSector );

			// Set Blocking...

			ApWorldSector * pSectorTmp;
			for( int i = 0 ; i < ( int ) vectorSector.size() ; i ++ )
			{
				pSectorTmp = vectorSector[ i ];

				if (
					pCmObject->m_pcsAgcmMap->SetSectorIndexInsertBBox(
					ApWorldSector::AWS_RIDABLEOBJECT ,
					pSectorTmp			,
					pcsApdObject->m_lID	,
					&bbox				)
					)
				{
					ASSERT( nSectorsNeededToBeUpdate < __MAX_FOUND_SECTOR );
					aSectorsNeededToBeUpdate[ nSectorsNeededToBeUpdate ++ ] = pSectorTmp;
				}
			}

			if( nSectorsNeededToBeUpdate > 1 ||
				( nSectorsNeededToBeUpdate == 1 && aSectorsNeededToBeUpdate[ 0 ] != pSector ) )
			{
				bObjectOverSectorRidable = TRUE;
			}
			*/

			// 강제로 한번 호출해줌..
			AgcmMap::GetBBoxCallback( pcsAgcdObject->m_pstCollisionAtomic , ( PVOID ) & bbox );

			// Set Blocking...

			// 범위를 조사함..
			INT32	uSectorX1 , uSectorX2 , uSectorZ1 , uSectorZ2;
			AuPOS	pos;
			pos.y	= 0.0f;
			pos.x	= ( bbox.inf.x < bbox.sup.x ? bbox.inf.x : bbox.sup.x ) ;
			pos.z	= ( bbox.inf.z < bbox.sup.z ? bbox.inf.z : bbox.sup.z ) ;
			pCmObject->m_pcsApmMap->GetSector( pos , &uSectorX1 , NULL , &uSectorZ1 );
			pos.x	= ( bbox.inf.x < bbox.sup.x ? bbox.sup.x : bbox.inf.x ) ;
			pos.z	= ( bbox.inf.z < bbox.sup.z ? bbox.sup.z : bbox.inf.z ) ;
			pCmObject->m_pcsApmMap->GetSector( pos , &uSectorX2 , NULL , &uSectorZ2 );

			INT32 sx , sz;
			ApWorldSector * pSectorTmp;
			int nCount = 0;
			int nSkip = 0;

			for( sz = uSectorZ1 ; sz <= uSectorZ2 ; sz ++ )
			{
				for( sx = uSectorX1 ; sx <= uSectorX2 ; sx ++ )
				{
					nCount ++;

					pSectorTmp = pCmObject->m_pcsApmMap->GetSector( sx , sz );

					if( pSectorTmp )
					{
						if (
							pCmObject->m_pcsAgcmMap->SetSectorIndexInsertBBox(
							ApWorldSector::AWS_RIDABLEOBJECT ,
							pSectorTmp			,
							pcsApdObject->m_lID	,
							&bbox				)
							)
						{
							ASSERT( nSectorsNeededToBeUpdate < __MAX_FOUND_SECTOR );
							aSectorsNeededToBeUpdate[ nSectorsNeededToBeUpdate ++ ] = pSectorTmp;
						}
					}
					else
					{
						nSkip++;
					}
				}
			}

			if( nSectorsNeededToBeUpdate > 1 || nSkip != 0 ||
				( nSectorsNeededToBeUpdate == 1 && aSectorsNeededToBeUpdate[ 0 ] != pSector ) )
			{
				bObjectOverSectorRidable = TRUE;
			}

		}
	}

	// 2005/01/16
	// 섹터넘어가는 오브젝트가 있는경우..

	if( bObjectOverSectorBlocking || bObjectOverSectorRidable )
	{
		// 범위가 넘는 오브젝트는 따로 리스트를 가지고 후에 삽입하는 과정이 추가가 된다.
		AgcmObject::RidableOverlapping	stLapping;
		
		stLapping.uFlag			= AgcmObject::RidableOverlapping::NONE;
		stLapping.nObjectIndex	= pcsApdObject->m_lID	;
		stLapping.stBBox		= bbox			;
		
		if( bObjectOverSectorBlocking ) // AcuObject::GetProperty( pcsAgcdObject->m_lObjectType ) & ACUOBJECT_TYPE_BLOCKING	)
			stLapping.uFlag |= AgcmObject::RidableOverlapping::BLOCKING;
		if( bObjectOverSectorRidable ) // AcuObject::GetProperty( pcsAgcdObject->m_lObjectType ) & ACUOBJECT_TYPE_RIDABLE		)
			stLapping.uFlag |= AgcmObject::RidableOverlapping::RIDABLE;

		if( stLapping.uFlag )
		{
			pCmObject->m_listRidableObjectlapping.AddTail( stLapping );

			TRACE( "AgcmObject::m_listRidableObjectlapping 들어있는 거 %d개\n" , pCmObject->m_listRidableObjectlapping.GetCount() );
		}
	}

	if( nSectorsNeededToBeUpdate )
	{
		for( int i = 0 ; i < nSectorsNeededToBeUpdate ; i ++ )
		{
			// 섹터에 있는 케릭터나 높이 정보를 갱신팔 필요가 있음..
			pCmObject->EnumCallback(AGCMOBJECT_CB_ID_RIDABLEUPDATE, (PVOID) ( aSectorsNeededToBeUpdate[ i ] ) , NULL );
		}
	}

	return TRUE;
}

BOOL AgcmObject::SetPreLightForAllObject(FLOAT fOffset)
{
	if((fOffset < 0.0f) || (fOffset > 2.0f))
		return FALSE;

	ApdObject					*pstApdObject;
	AgcdObject					*pstAgcdObject;
	AgcdObjectTemplate			*pstAgcdObjectTemplate;

	INT32						lIndex	= 0;

	FLOAT						fRed, fBlue, fGreen, fAlpha;

	AgcdObjectGroupList			*pstList;
	RwRGBA						stRGBA;

	for(	pstApdObject = m_pcsApmObject->GetObjectSequence(&lIndex);
			pstApdObject;
			pstApdObject = m_pcsApmObject->GetObjectSequence(&lIndex)		)
	{
		pstAgcdObject = GetObjectData(pstApdObject);
		if(!pstAgcdObject)
			return FALSE;

		pstAgcdObjectTemplate = pstAgcdObject->m_pstTemplate;

		pstList = pstAgcdObject->m_stGroup.m_pstList;
		while(pstList)
		{
			if(pstList->m_csData.m_pstClump)
			{
				fRed			= (FLOAT)(pstAgcdObjectTemplate->m_stPreLight.red) * fOffset;
				fBlue			= (FLOAT)(pstAgcdObjectTemplate->m_stPreLight.green) * fOffset;
				fGreen			= (FLOAT)(pstAgcdObjectTemplate->m_stPreLight.blue) * fOffset;
				fAlpha			= (FLOAT)(pstAgcdObjectTemplate->m_stPreLight.alpha) * fOffset;

				if(fRed > 255.0f)
					fRed = 255.0f;
				if(fBlue > 255.0f)
					fBlue = 255.0f;
				if(fGreen > 255.0f)
					fGreen = 255.0f;
				if(fAlpha > 255.0f)
					fAlpha = 255.0f;

				stRGBA.red		= (UINT8)(fRed);
				stRGBA.blue		= (UINT8)(fBlue);
				stRGBA.green	= (UINT8)(fGreen);
				stRGBA.alpha	= (UINT8)(fAlpha);

				AcuObject::SetClumpPreLitLim(pstList->m_csData.m_pstClump, &stRGBA);
			}

			pstList = pstList->m_pstNext;
		}
	}


	return TRUE;
}

struct	stFindDistance
{
	FLOAT	fHeight	;
	FLOAT	fInitial;
	FLOAT	fGap	;
	BOOL	bFound	;

	static const float cfDefaultGap;

	stFindDistance( FLOAT fHeightInput ) : bFound( FALSE )
	{
		fInitial	= fHeight = fHeightInput;

		// 최초의 갭은 최대값으로..
		fGap		= cfDefaultGap;
	}
};

const float stFindDistance::cfDefaultGap = SECTOR_MAX_HEIGHT - INVALID_HEIGHT;

//FLOAT	stFindDistance::cfDefaultGap = SECTOR_MAX_HEIGHT * 2.0f;

//#define __MAX_RIDABLE_CHECK_HEIGHT	500.0f
static FLOAT __MAX_RIDABLE_CHECK_HEIGHT	= 400.0f;
static RpCollisionTriangle * __IntersectionCallBackFindDistance
											(RpIntersection *intersection, RpCollisionTriangle *collTriangle,
											RwReal distance, void *pData)
{
	stFindDistance	* pFD	= ( stFindDistance * ) pData;
	//FLOAT	* pHeight = ( FLOAT * ) pData;
	ASSERT( NULL != pFD );

	FLOAT	fDistance		= intersection->t.line.start.y - 
		( intersection->t.line.start.y - intersection->t.line.end.y ) * distance ;

	if( distance == 0.0f ) return collTriangle;

	/*
	if( pFD->fHeight < fDistance && fGapNew < __MAX_RIDABLE_CHECK_HEIGHT)
	{
		// 원래거보다 갭이 적으니까 이걸로 적용함..
		pFD->fHeight	= fDistance	;
		pFD->fGap		= fGapNew	;
		pFD->bFound		= TRUE		;
	}
	else
	*/

	if( pFD->fInitial >= SECTOR_MAX_HEIGHT ) 
	{
		FLOAT	fGapNew			= pFD->fInitial - fDistance;
		FLOAT	fGapOriginal	= pFD->fGap;

		// 위에서 부터..
		if( fGapNew < fGapOriginal )
		{
			pFD->fHeight	= fDistance	;
			pFD->fGap		= fGapNew	;
			pFD->bFound		= TRUE		;
		}
	}
	else
	{
		FLOAT	fGapNew			= fDistance - pFD->fInitial;
		FLOAT	fGapOriginal	= pFD->fGap;

		// 케릭터 위치 기준..
		// 위로는 __MAX_RIDABLE_CHECK_HEIGHT 만큼
		// 아래론 영원히..

		if( fGapNew < __MAX_RIDABLE_CHECK_HEIGHT )
		{
			// 그중에서 가장 위에 있는거.
			if( pFD->fHeight < fDistance || fGapOriginal == stFindDistance::cfDefaultGap )
			{
				// 원래거보다 갭이 적으니까 이걸로 적용함..
				pFD->fHeight	= fDistance	;
				pFD->fGap		= fGapNew	;
				pFD->bFound		= TRUE		;
			}
		}
	}

	return collTriangle;
}

BOOL	AgcmObject::CBOnGetHeight	( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmObject				* pcsAgcmObject	= ( AgcmObject				* ) pClass	;
	stCollisionAtomicInfo	* pCollInfo		= ( stCollisionAtomicInfo	* ) pData	;

	ApdObject		*	pstApdObject	;
	AgcdObject		*	pstAgcdObject	;

	BOOL	bExistRidableObject	= FALSE;

	RpIntersection	stIntersection		;
	stIntersection.type = rpINTERSECTLINE;

	stIntersection.t.line.start.x	= pCollInfo->fX		;
	stIntersection.t.line.start.z	= pCollInfo->fZ		;
	stIntersection.t.line.start.y	= SECTOR_MAX_HEIGHT	;

	stIntersection.t.line.end.x		= pCollInfo->fX		;
	stIntersection.t.line.end.z		= pCollInfo->fZ		;
	stIntersection.t.line.end.y		= INVALID_HEIGHT	;

	for( int i = 0 ; i < pCollInfo->nObject ; ++ i )
	{
		pstApdObject	= pcsAgcmObject->m_pcsApmObject->	GetObject		( pCollInfo->aObject[ i ]	);
		pstAgcdObject	= pcsAgcmObject->					GetObjectData	( pstApdObject				);
		
		if( 
			pstApdObject	&&
			pstAgcdObject	)
			//AcuObject::GetProperty( pstApdObject->m_pcsTemplate->m_nObjectType ) & ACUOBJECT_TYPE_RIDABLE )
		{
			if( pstAgcdObject->m_pstCollisionAtomic							&&
				//@{ kday 20050628
				// ;)
				pstAgcdObject->m_pstCollisionAtomic->geometry				&&
				//@} kday
				pstAgcdObject->m_pstCollisionAtomic->geometry->triangles	)
			{
				pcsAgcmObject->LockFrame();

				// FLOAT	fHeight = SECTOR_MAX_HEIGHT;
				stFindDistance	stHeight( pCollInfo->fCharacterHeight );
				
				RpAtomicForAllIntersections (
					pstAgcdObject->m_pstCollisionAtomic		,
					&stIntersection							,
					__IntersectionCallBackFindDistance		,
					( void * ) &stHeight				);

				if( stHeight.bFound											&&
					// stHeight.fHeight != pCollInfo->fCharacterHeight	&&	// 원래 높이와 다른점이 있어야 하며..
					pCollInfo->fHeight < stHeight.fHeight )	// 콜리젼된 높이는 지형 높이보다 높아야 하며..
					// 최대값 얻는 경우에는 높이차 검사를 하지 않아야 한다.
					// ( pCollInfo->fCharacterHeight >= ( SECTOR_MAX_HEIGHT - 100.0f) ? TRUE : ( fabs( stHeight.fHeight - pCollInfo->fCharacterHeight ) < g__fLeastRidableHeigt ) )  )
				{
					// 마고자 (2005-04-25 오후 5:55:28) : 
					// 높이 얻어낸 오브젝트 라이더블 타입을 저장해둠..
					pCollInfo->uRidableType = pstAgcdObject->m_pstTemplate->m_eRidableMaterialType;
					pCollInfo->fHeight		= stHeight.fHeight;

					bExistRidableObject = TRUE;
				}

				pcsAgcmObject->UnlockFrame();
				// 콜리젼 데이타 초기화
			}
			else
			{
				// 인스턴싱됀 녀석은 이짓 하면 죽는다 -_-;;
			}
		}
	}	

	return bExistRidableObject;
}

BOOL	AgcmObject::CBOcTreeIDSet(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmObject*	pThis = (AgcmObject*) pClass;

	pThis->SetAllOcTreeIDs();
	
	return TRUE;
}

void	AgcmObject::SetAllOcTreeIDs()
{
	INT32	index = 0;
	ApdObject*		pdObject;
	AgcdObject*		pcdObject;
	INT32*			uda_ids;
	OcTreeIDList*	nw_id;

	OcTreeIDList*	cur_id;
	OcTreeIDList*	remove_id;
	
	for(pdObject = m_pcsApmObject->GetObjectSequence(&index);pdObject;
	pdObject = m_pcsApmObject->GetObjectSequence(&index))
	{
		pcdObject = GetObjectData(pdObject);

		if(!pcdObject->m_stGroup.m_pstList || !pcdObject->m_stGroup.m_pstList->m_csData.m_pstClump) continue;
		
		//pdobject의 listoctreeid 지우자
		cur_id = pdObject->m_listOcTreeID;
		while(cur_id)
		{
			remove_id = cur_id;
			cur_id = cur_id->next;

			delete remove_id;
		}

		pdObject->m_listOcTreeID = NULL;
		
		uda_ids = AcuObject::GetOcTreeID(pcdObject->m_stGroup.m_pstList->m_csData.m_pstClump);

		pdObject->m_nOcTreeID = uda_ids[0];
		
		for(int i=0;i<uda_ids[0];++i)
		{
			nw_id = new OcTreeIDList;
			
			nw_id->six = uda_ids[1+i*3];
			nw_id->siz = uda_ids[2+i*3];
			nw_id->ID = uda_ids[3+i*3];

			nw_id->next = pdObject->m_listOcTreeID;
			pdObject->m_listOcTreeID = nw_id;
		}
	}
}

BOOL AgcmObject::CopyObjectTemplateGroup(INT32 lTID, AgcdObjectTemplateGroup *pcsDest, AgcdObjectTemplateGroup *pcsSrc)
{
	pcsDest->m_pstList						= NULL;
	pcsDest->m_lNum							= 0;

	AgcdObjectTemplateGroupData *pcsData, *pcsDestData;
	for (INT32 lIndex = 0;; ++lIndex)
	{
		pcsData = m_csObjectList.GetObjectTemplateGroup(pcsSrc, lIndex);
		if (!pcsData)
			break;

		pcsDestData = m_csObjectList.GetObjectTemplateGroup(pcsDest, lIndex);
		if (!pcsDestData)
			pcsDestData = m_csObjectList.AddObjectTemplateGroup(pcsDest);
		if (!pcsDestData)
			return FALSE;

		if (!CopyObjectTemplateGroupData(lTID, pcsDestData, pcsData))
			return FALSE;
	}

	return TRUE;
}

BOOL AgcmObject::CopyObjectTemplateGroupData(INT32 lTID, AgcdObjectTemplateGroupData *pcsDest, AgcdObjectTemplateGroupData *pcsSrc)
{
/*	strcpy(pcsDest->m_szDFFName, pcsSrc->m_szDFFName);
	
	for (INT32 lCount = 0; lCount < E_OBJECT_ANIM_TYPE_NUM; ++lCount)
	{
		if (pcsSrc->m_aszAnimName[lCount])
		{
			if (!pcsDest->m_aszAnimName[lCount])
				pcsDest->m_aszAnimName[lCount] = new CHAR[strlen(pcsSrc->m_aszAnimName[lCount]) + 1];
			if (!pcsDest->m_aszAnimName[lCount])
				return FALSE;

			strcpy(pcsDest->m_aszAnimName[lCount], pcsSrc->m_aszAnimName[lCount]);
		}
		else if (pcsDest->m_aszAnimName[lCount])
		{
			delete [] pcsDest->m_aszAnimName[lCount];
			pcsDest->m_aszAnimName[lCount] = NULL;
		}

		if (pcsSrc->m_pastAnim[lCount])
		{
			if (!pcsDest->m_pastAnim[lCount])
				pcsDest->m_pastAnim[lCount] = GetAnimation(lTID, pcsDest->m_lIndex, lCount, TRUE);
			if (!pcsDest->m_pastAnim[lCount])
				return FALSE;

//			memcpy(pcsDest->m_pastAnim[lCount], pcsSrc->m_pastAnim[lCount], sizeof(AgcdAnimation));
		}
		else if (pcsDest->m_pastAnim[lCount])
		{
			pcsDest->m_pastAnim[lCount]	= NULL;
		}

		pcsDest->m_afAnimSpeed[lCount] = pcsSrc->m_afAnimSpeed[lCount];
	}*/

	if (pcsSrc->m_pszDFFName)
	{
		if (pcsDest->m_pszDFFName)
			delete [] pcsDest->m_pszDFFName;

		pcsDest->m_pszDFFName	= new CHAR [strlen(pcsSrc->m_pszDFFName) + 1];
		strcpy(pcsDest->m_pszDFFName, pcsSrc->m_pszDFFName);
	}

/*	if (pcsSrc->m_pszAnimName)
	{
		if (pcsDest->m_pszAnimName)
			delete [] pcsDest->m_pszAnimName;

		pcsDest->m_pszAnimName	= new CHAR [strlen(pcsSrc->m_pszAnimName) + 1];
		strcpy(pcsDest->m_pszAnimName, pcsSrc->m_pszAnimName);
	}*/

	pcsDest->m_pcsAnimation = NULL;

	if (	(pcsSrc->m_pcsAnimation) &&
			(pcsSrc->m_pcsAnimation->m_pcsHead) &&
			(pcsSrc->m_pcsAnimation->m_pcsHead->m_pszRtAnimName)	)
	{
		m_csAnimation2.AddAnimation(&pcsDest->m_pcsAnimation, pcsSrc->m_pcsAnimation->m_pcsHead->m_pszRtAnimName);
	}

	pcsDest->m_fAnimSpeed	= pcsSrc->m_fAnimSpeed;

	memcpy(&pcsDest->m_stBSphere, &pcsSrc->m_stBSphere, sizeof(RwSphere));

	return TRUE;
}

BOOL AgcmObject::RemoveObjectTemplateGroup(AgcdObjectTemplateGroup *pcsGroup)
{
	AgcdObjectTemplateGroupData *pcsData;
	for (INT32 lIndex = 0;; ++lIndex)
	{
		pcsData = m_csObjectList.GetObjectTemplateGroup(pcsGroup, lIndex);
		if (!pcsData)
			break;

		RemoveObjectTemplateGroupData(pcsData);
	}

	return m_csObjectList.RemoveAllObjectTemplateGroup(pcsGroup);
}

BOOL AgcmObject::RemoveObjectTemplateGroupData(AgcdObjectTemplateGroupData *pcsData)
{
/*	if (pcsData->m_pszAnimName)
	{
		delete [] pcsData->m_pszAnimName;
		pcsData->m_pszAnimName	= NULL;
	}*/

	if (pcsData->m_pszDFFName)
	{
		delete [] pcsData->m_pszDFFName;
		pcsData->m_pszDFFName	= NULL;
	}

	if (pcsData->m_pcsAnimation)
		m_csAnimation2.RemoveAllAnimation(&pcsData->m_pcsAnimation);

/*	for (INT32 lCount = 0; lCount < E_OBJECT_ANIM_TYPE_NUM; ++lCount)
	{
		if (pcsData->m_aszAnimName[lCount])
		{
			delete [] pcsData->m_aszAnimName[lCount];
			pcsData->m_aszAnimName[lCount] = NULL;
		}
	}*/

	return TRUE;
}
/*
PVOID AgcmObject::GetAnimationAttachedData(AgcdAnimation *pstAnim, CHAR *szKey)
{
	return m_csAnimation.GetAttachedData(pstAnim, szKey);
}*/

VOID AgcmObject::ShowPickingInfo()
{
	if (m_bShowPickingInfo)
		return;

	INT32			lIndex = 0;
	ApdObject *		pcsObject;
	AgcdObject *	pstObject;

	for (pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex); pcsObject; pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex))
	{
		pstObject = GetObjectData(pcsObject);

		if (pstObject->m_stGroup.m_pstList && pstObject->m_stGroup.m_pstList->m_csData.m_pstClump)
		{
			m_pcsAgcmRender->RemoveClumpFromWorld(pstObject->m_stGroup.m_pstList->m_csData.m_pstClump);

			if (pstObject->m_pstPickAtomic)
			{
				RpClumpAddAtomic(pstObject->m_stGroup.m_pstList->m_csData.m_pstClump, pstObject->m_pstPickAtomic);
			}
			if (pstObject->m_pstCollisionAtomic)
			{
				RpClumpAddAtomic(pstObject->m_stGroup.m_pstList->m_csData.m_pstClump, pstObject->m_pstCollisionAtomic);
			}

			m_pcsAgcmRender->AddClumpToWorld(pstObject->m_stGroup.m_pstList->m_csData.m_pstClump); 
			m_pcsAgcmRender->AddUpdateInfotoClump(pstObject->m_stGroup.m_pstList->m_csData.m_pstClump,this,
				CBUpdateAnimation,NULL, pstObject->m_stGroup.m_pstList->m_csData.m_pstClump,
				(PVOID)&(pstObject->m_stGroup.m_pstList->m_csData));
		}
	}

	m_bShowPickingInfo = TRUE;
}

VOID AgcmObject::HidePickingInfo()
{
	if (!m_bShowPickingInfo)
		return;

	INT32			lIndex = 0;
	ApdObject *		pcsObject;
	AgcdObject *	pstObject;

	for (pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex); pcsObject; pcsObject = m_pcsApmObject->GetObjectSequence(&lIndex))
	{
		pstObject = GetObjectData(pcsObject);

		if (pstObject->m_stGroup.m_pstList && pstObject->m_stGroup.m_pstList->m_csData.m_pstClump)
		{
			m_pcsAgcmRender->RemoveClumpFromWorld(pstObject->m_stGroup.m_pstList->m_csData.m_pstClump);
			if (pstObject->m_pstPickAtomic)
			{
				if (RpAtomicGetClump(pstObject->m_pstPickAtomic))
					RpClumpRemoveAtomic(RpAtomicGetClump(pstObject->m_pstPickAtomic), pstObject->m_pstPickAtomic);
			}
			if (pstObject->m_pstCollisionAtomic)
			{
				if (RpAtomicGetClump(pstObject->m_pstCollisionAtomic))
					RpClumpRemoveAtomic(RpAtomicGetClump(pstObject->m_pstCollisionAtomic), pstObject->m_pstCollisionAtomic);
			}

			m_pcsAgcmRender->AddClumpToWorld(pstObject->m_stGroup.m_pstList->m_csData.m_pstClump); 
			m_pcsAgcmRender->AddUpdateInfotoClump(pstObject->m_stGroup.m_pstList->m_csData.m_pstClump,this,
				CBUpdateAnimation,NULL, pstObject->m_stGroup.m_pstList->m_csData.m_pstClump,
				(PVOID)&(pstObject->m_stGroup.m_pstList->m_csData));
		}
	}

	m_bShowPickingInfo = FALSE;
}

FLOAT AgcmObject::GetBoundingSphereRange(ApdObject *pcsObject)
{
	if (!pcsObject)
		return 0.0f;

	AgcdObject *			pstObject	= GetObjectData(pcsObject);
	AgcdObjectGroupData *	pcsData;
	FLOAT					fMaxRange = 0;

	for (INT32 lIndex = 0;; ++lIndex)
	{
		pcsData = GetObjectGroupData(&pstObject->m_stGroup, lIndex, FALSE);
		if (!pcsData)
			break;

		if (fMaxRange < pcsData->m_stBSphere.radius)
			fMaxRange = pcsData->m_stBSphere.radius;
	}

	return fMaxRange;
}

bool AgcmObject::IsValidLOD( ApdObjectTemplate* pApdObjectTemplate, AgcdObjectTemplate* pAgcdObjectTemplate )
{
	AgcdLODList	*pstLODList = pAgcdObjectTemplate->m_stLOD.m_pstList;
	AgcmPreLODManager *pAgcmPreLODManager = (AgcmPreLODManager *)GetModule("AgcmPreLODManager");

	AgcdPreLOD	*pstAgcdPreLOD = pAgcmPreLODManager->GetObjectPreLOD(pApdObjectTemplate);
	if( !pstAgcdPreLOD )		return false;

	if( !pAgcdObjectTemplate->m_stGroup.m_pstList->m_csData.m_pstClump )	return false;

	int nAtomicNum = RpClumpGetNumAtomics( pAgcdObjectTemplate->m_stGroup.m_pstList->m_csData.m_pstClump );
	AgcdPreLODList* pstList = pstAgcdPreLOD->m_pstList;
	while( pstList )
	{
		for( int i = 0; i < 5; i++ )
		{
			if( strcmp( pstList->m_csData.m_aszData[i], "" ) == 0 )
				continue;

			RpClump* pClump = LoadClump( pstList->m_csData.m_aszData[i] );
			if( pClump == NULL )
				continue;
			int nAtomicNum2 = RpClumpGetNumAtomics( pClump );
			RpClumpDestroy ( pClump );
			if( nAtomicNum2 != 1 && nAtomicNum != nAtomicNum2 ) 
				return false;
		}
		pstList = pstList->m_pstNext;
	}

	return true;
}

BOOL AgcmObject::IsInValidOctreeData( void* pObjectTemplate )
{
	AgcdObjectTemplate* pcdTemplate = ( AgcdObjectTemplate* )pObjectTemplate;
	if( !pcdTemplate ) return TRUE;

	if( pcdTemplate->m_stOcTreeData.topVerts_MAX[ 0 ].x == 0.0 && 
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 0 ].y == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 0 ].z == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 0 ].x == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 1 ].x == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 1 ].y == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 1 ].z == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 2 ].x == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 2 ].y == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 2 ].z == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 3 ].x == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 3 ].y == 0.0 &&
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 3 ].z == 0.0 ) return TRUE;

	return FALSE;
}

void AgcmObject::ReCalulateOctreeData( CHAR* pOctreeType, void* pObjectTemplate )
{
	AgcdObjectTemplate* pcdTemplate = ( AgcdObjectTemplate* )pObjectTemplate;
	if( !pcdTemplate ) return;

	RwBBox BBox;

	BBox.inf.x = 0.0f;
	BBox.inf.y = 0.0f;
	BBox.inf.z = 0.0f;

	BBox.sup.x = 0.0f;
	BBox.sup.y = 0.0f;
	BBox.sup.z = 0.0f;

	RpClump* pClumps[ 10 ] = { NULL, };

	INT32 nClumpCount = pcdTemplate->m_stGroup.m_lNum;
	for( INT32 nCount = 0 ; nCount < nClumpCount ; ++nCount )
	{
		AgcdObjectTemplateGroupList* pCurrent = pcdTemplate->m_stGroup.m_pstList;
		if( pCurrent )
		{
			pClumps[ nCount ] = pCurrent->m_csData.m_pstClump;
#ifdef USE_MFC
			// 만약 아직 클럼프가 로드되지 않았다면 클럼프를 로드해서 넘겨준다.
			// 이건 실제 클라에서 하기엔 쥐랄맞으니까 툴에서만 하자.. 어차피 툴아니면 할 이유도 없고..
			if( !pClumps[ nCount ] )
			{
				CHAR strFilePathName[ 256 ] = { 0, };
				sprintf( strFilePathName, "%s\\%s", pOctreeType, pCurrent->m_csData.m_pszDFFName );

				AgcmResourceLoader* pcmLoader = ( AgcmResourceLoader* )GetModule( "AgcmResourceLoader" );
				pClumps[ nCount ] = pcmLoader->LoadClump( strFilePathName );
				if( !pClumps[ nCount ] )
				{
					// 그래도 읽기 실패했다면..
					CHAR strMsg[ 256 ] = { 0, };
					sprintf( strMsg, "클럼프를 로드하지 못하여 옥트리정보를 생성할수 없습니다. ( File : %s )", pCurrent->m_csData.m_pszDFFName );
					MessageBox( NULL, "옥트리 정보 생성 실패!", strMsg, MB_OK );
				}
			}
#endif
			pCurrent = pCurrent->m_pstNext; 
		}
	}

	if( nClumpCount > 0 )
	{
		RwUtilClumpGetBoundingBox( pClumps, nClumpCount, &BBox);

		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 0 ].x = BBox.inf.x;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 0 ].y = BBox.sup.y;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 0 ].z = BBox.inf.z;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 1 ].x = BBox.inf.x;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 1 ].y = BBox.sup.y;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 1 ].z = BBox.sup.z;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 2 ].x = BBox.sup.x;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 2 ].y = BBox.sup.y;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 2 ].z = BBox.inf.z;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 3 ].x = BBox.sup.x;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 3 ].y = BBox.sup.y;
		pcdTemplate->m_stOcTreeData.topVerts_MAX[ 3 ].z = BBox.sup.z;
	}	
}