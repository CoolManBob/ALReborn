// AcuObject.cpp: implementation of the AcuObject class.
//
//////////////////////////////////////////////////////////////////////

#include "AcuObject.h"
#include <string.h>
#include <rpskin.h>

#include <algorithm>
#include <rtintsec.h>
#include "AcuMathFunc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHAR *		AcuObject::m_szName = "Type"; // 마고자 추가.
RwCamera *	AcuObject::m_pstCamera = NULL;

//INT32			AcuObject::m_iByteAllocated = 0;	
//INT32			AcuObject::m_iCurAllocated = 0;
//UINT8*			AcuObject::m_pMemoryBlock = NULL;
//UINT8*			AcuObject::m_pFrame = NULL;

//ApCriticalSection		AcuObject::m_csMutexFrameMemory;

#ifdef _DEBUG
#define _DEBUG_ACUOBJECT
#endif

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL AcuObject_RwUtilAtomicHasSkin(RpAtomic *pstAtomic)
{
	RpGeometry *pGeom = RpAtomicGetGeometry(pstAtomic);
	if(pGeom)
	{
		RpSkin *pSkin = RpSkinGeometryGetSkin(pGeom);
		if(pSkin)
			return TRUE;
	}

	return FALSE;
}


AcuObject::AcuObject()
{
	//ASSERT( AcuObject::m_iByteAllocated == 0 && "AcuObject 인스턴스가 더 생성이 돼어 선 안안됀다." );

	m_szName = "Type";
	m_pstCamera = NULL;

	//m_pMemoryBlock	= new UINT8[ALLOC_FRAME_MEM_ACUOBJECT];	// 3mb 할당
	//m_pFrame = m_pMemoryBlock;
	//m_iByteAllocated = ALLOC_FRAME_MEM_ACUOBJECT;

	//m_csMutexFrameMemory.Init();
}

AcuObject::~AcuObject()
{
	//delete []m_pMemoryBlock; 
	//m_pMemoryBlock = m_pFrame = NULL;
	//m_iByteAllocated = 0;
	//m_iCurAllocated = 0;
}

//void*	AcuObject::AllocFrameMemory(INT32 nBytes)
//{
//	UINT8*		pMem;
//
//	pMem = m_pFrame;
//
//	m_iCurAllocated += nBytes;
//	ASSERT( m_iCurAllocated < m_iByteAllocated );
//	if( m_iCurAllocated >= m_iByteAllocated ) return NULL;
//	
//	m_pFrame += nBytes;
//
//	return (void*)pMem;
//}
//
//void	AcuObject::DeallocFrameMemory(INT32	nBytes)
//{
//	m_pFrame -= nBytes;
//	m_iCurAllocated -= nBytes;
//}

VOID			AcuObject::SetAtomicType(RpAtomic *pAtomic, INT32 eType, INT32 lID, PVOID pvObject, PVOID pvTemplate, PVOID pvCustData, PVOID pvPickAtomic, RwSphere*	sphere,PVOID pvOctreeData,PVOID pvOcTreeIDList)
{
	RpClump*	pClump = RpAtomicGetClump(pAtomic);
	if (pClump)
	{
		SetClumpType(pClump,eType,lID,pvObject,pvTemplate,pvCustData,(RpAtomic*)pvPickAtomic,
							sphere,(RpAtomic*)pvPickAtomic,pvOctreeData,pvOcTreeIDList);
	}
	else
	{
		if(!pAtomic->stType)
		{
			pAtomic->stType = (AgcdType*)malloc(sizeof(AgcdType));

			memset(pAtomic->stType,0,sizeof(AgcdType));

			//pAtomic->stType->eType = 0;
			//pAtomic->stType->lID = 0;
			//pAtomic->stType->pObject = NULL;
			//pAtomic->stType->pTemplate = NULL;
			//pAtomic->stType->pCustData = NULL;
			//pAtomic->stType->pPickAtomic = NULL;
			//pAtomic->stType->pOcTreeData = NULL;
			//pAtomic->stType->pOcTreeIDList = NULL;
			//
			//pAtomic->stType->boundingSphere.center.x = 0.0f;
			//pAtomic->stType->boundingSphere.center.y = 0.0f;
			//pAtomic->stType->boundingSphere.center.z = 0.0f;
			//pAtomic->stType->boundingSphere.radius = 0.0f;

			//pAtomic->stType->pUpdateList = NULL;
			//pAtomic->stType->updateTick = 0;

			//pAtomic->stType->frustumTestTick = 0;
			//pAtomic->stType->frustumTestResult = 0;
			//pAtomic->stType->viewSectorDistance = 0;
			//
			//pAtomic->stType->pCollisionAtomic = NULL;
			//pAtomic->stType->pRenderAddedNode = NULL;
		}

		pAtomic->stType->eType = eType;
		pAtomic->stType->lID = lID;
		pAtomic->stType->pObject = pvObject;
		pAtomic->stType->pTemplate = pvTemplate;
		pAtomic->stType->pCustData = pvCustData;
		pAtomic->stType->pOcTreeData = pvOctreeData;
		pAtomic->stType->pOcTreeIDList = pvOcTreeIDList;
		pAtomic->stType->updateTick = 0;
		pAtomic->stType->pUpdateList = NULL;
		pAtomic->stType->pPickAtomic = pvPickAtomic;

		// 콜리젼 아토믹 설정.. 아토믹은 그냥 콜리젼도 같은걸 사용함..
		pAtomic->stType->pCollisionAtomic = pvPickAtomic;

		if(sphere)
		{
			pAtomic->stType->boundingSphere.center.x = sphere->center.x;
			pAtomic->stType->boundingSphere.center.y = sphere->center.y;
			pAtomic->stType->boundingSphere.center.z = sphere->center.z;
			pAtomic->stType->boundingSphere.radius = sphere->radius;
		}
	}
}

RpAtomic *	AcuObject::GetAtomicGetCollisionAtomic	( RpAtomic * pstAtomic )
{
	// 유져데이타에서 콜리젼 아토믹 뽑아내기.
	if (!pstAtomic)		return NULL;

	RpClump*	pClump = RpAtomicGetClump(pstAtomic);
	if(pClump)
	{
		return ( RpAtomic* ) pClump->stType.pCollisionAtomic;
	}
	else
	{
		return ( RpAtomic* ) pstAtomic->stType->pCollisionAtomic;
	}
}

RpAtomic *	AcuObject::GetAtomicGetPickingAtomic	( RpAtomic * pstAtomic )
{
	// 유져데이타에서 콜리젼 아토믹 뽑아내기.
	if (!pstAtomic)		return NULL;

	RpClump*	pClump = RpAtomicGetClump(pstAtomic);
	if(pClump)
	{
		return ( RpAtomic* ) pClump->stType.pPickAtomic;
	}
	else
	{
		return ( RpAtomic* ) pstAtomic->stType->pPickAtomic;
	}
}


INT32	AcuObject::GetAtomicType(RpAtomic *pAtomic, INT32 *plID, PVOID *ppvObject, PVOID *ppvTemplate, PVOID *ppvCustData, PVOID *ppvPickAtomic)
{
	// 마고자 (2003-12-16 오후 12:32:39) : 인자 체크좀 부탁해요 -_-;
	if (!pAtomic)		return NULL;

	INT32			eType = ACUOBJECT_TYPE_NONE;
	AgcdType*		lpType = NULL;
	
	RpClump*	pClump = RpAtomicGetClump(pAtomic);
	if(pClump)
	{
		lpType = &pClump->stType;
	}
	else
	{
		lpType = pAtomic->stType;
	}

	if(lpType)
	{
		eType = lpType->eType;
		if (plID)	*plID = lpType->lID;
		if (ppvObject)	*ppvObject = lpType->pObject;
		if (ppvTemplate) *ppvTemplate = lpType->pTemplate;
		if (ppvCustData) *ppvCustData = lpType->pCustData;
		if (ppvPickAtomic) *ppvPickAtomic = lpType->pPickAtomic;
	}

	return eType;
}

VOID			AcuObject::SetClumpType(RpClump *pClump, INT32 eType, INT32 lID, PVOID pvObject, PVOID pvTemplate, PVOID pvCustData, RpAtomic * pvPickAtomic,RwSphere*	sphere , RpAtomic * pvCollisionAtomic, PVOID pvOctreeData, PVOID pvOcTreeIDList)
{
	if (pClump)
	{
		pClump->stType.eType = eType;
		pClump->stType.lID = lID;
		pClump->stType.pObject = pvObject;
		pClump->stType.pTemplate = pvTemplate;
		pClump->stType.pCustData = pvCustData;
		pClump->stType.pOcTreeData = pvOctreeData;
		pClump->stType.pOcTreeIDList = pvOcTreeIDList;
		pClump->stType.updateTick = 0;
		pClump->stType.pUpdateList = NULL;

		// 피킹 아토믹 설정..
		// 마고자 (2004-03-19 오후 5:54:43) : 피킹 아토믹이 있을때만 설정하고..
		// 평소에는 콜리젼 아토믹과 같이 처리한다.. ( 쫑석아저씨랑 합의 )
		if( pvPickAtomic )
			pClump->stType.pPickAtomic = pvPickAtomic;
		else 
			pClump->stType.pPickAtomic = pvCollisionAtomic;

		pClump->stType.pCollisionAtomic = pvCollisionAtomic;

		if(sphere)
		{
			pClump->stType.boundingSphere.center.x = sphere->center.x;
			pClump->stType.boundingSphere.center.y = sphere->center.y;
			pClump->stType.boundingSphere.center.z = sphere->center.z;
			pClump->stType.boundingSphere.radius = sphere->radius;
		}
	}
}

VOID			AcuObject::SetClumpType(RpClump *pClump, INT32 eType)
{
	if(pClump)
	{
		pClump->stType.eType = eType;
		
		pClump->stType.lID = 0;
		pClump->stType.pObject = NULL;
		pClump->stType.pTemplate = NULL;
		pClump->stType.pCustData = NULL;
		pClump->stType.pOcTreeData = NULL;
		pClump->stType.pOcTreeIDList = NULL;
		pClump->stType.updateTick = 0;
		pClump->stType.pUpdateList = NULL;

		pClump->stType.pPickAtomic = NULL;
		pClump->stType.pCollisionAtomic = NULL;

		pClump->stType.boundingSphere.center.x = 0.0f;
		pClump->stType.boundingSphere.center.y = 0.0f;
		pClump->stType.boundingSphere.center.z = 0.0f;
		pClump->stType.boundingSphere.radius = 0.0f;
	}
}

INT32	AcuObject::GetClumpType(RpClump *pClump, INT32 *plID, PVOID *ppvObject, PVOID *ppvTemplate, PVOID *ppvCustData, PVOID *ppvPickAtomic)
{
	INT32			eType = ACUOBJECT_TYPE_NONE;

	if(pClump)
	{
		eType = pClump->stType.eType;
		if (plID)	*plID = pClump->stType.lID;
		if (ppvObject)	*ppvObject = pClump->stType.pObject;
		if (ppvTemplate) *ppvTemplate = pClump->stType.pTemplate;
		if (ppvCustData) *ppvCustData = pClump->stType.pCustData;
		if (ppvPickAtomic) *ppvPickAtomic = pClump->stType.pPickAtomic;
	}

	return eType;
}

VOID	AcuObject::GetAtomicBSphere(RpAtomic* pAtomic, RwSphere* pSphere)
{
	RpClump*	pClump = RpAtomicGetClump(pAtomic);
	AgcdType*	lpType = NULL;
	if(pClump)
	{
		lpType = &pClump->stType;
	}
	else
	{
		lpType = pAtomic->stType; 
	}

	if(lpType)
	{
		pSphere->center.x = lpType->boundingSphere.center.x;
		pSphere->center.y = lpType->boundingSphere.center.y;
		pSphere->center.z = lpType->boundingSphere.center.z;
		pSphere->radius = lpType->boundingSphere.radius;
	}
}

// AgcdType은 rpWorld.h에 정의..
AgcdType*	AcuObject::GetAtomicTypeStruct( RpAtomic* pAtomic)
{
	RpClump*	pClump = RpAtomicGetClump(pAtomic);
	if(pClump)
	{
		return &pClump->stType;
	}
	else
	{
		return pAtomic->stType; 
	}
}

//############# IsBillBoard(lodatomic에만 붙으므로 공용으로 안붙인다) #########################
VOID	AcuObject::SetClumpIsBillboard(RpClump* pstClump, PVOID pvData)
{
	RpClumpForAllAtomics(pstClump,CBSetIsBillboard,pvData);
}

RpAtomic*	AcuObject::CBSetIsBillboard(RpAtomic* pAtomic,PVOID pvData)
{
	IsBillData*		data = (IsBillData*) pvData;
	int num = data->num;

	for(int i=0;i<num;++i)
	{
		SetGeometryIsBillboard(RpLODAtomicGetGeometry(pAtomic,i),data->isbill[i]);
	}

	return pAtomic;
}

VOID	AcuObject::SetGeometryIsBillboard(RpGeometry* pGeom,int isBill)
{
	if(pGeom == NULL) return;

	RpUserDataArray	*	pstUDA = NULL;
	INT32				nIndex,nMax;
	RwChar*				pszName;

	bool	bFind = false;

	nMax = RpGeometryGetUserDataArrayCount(pGeom);
	for (nIndex = 0; nIndex < nMax; ++nIndex)
	{
		pstUDA = RpGeometryGetUserDataArray(pGeom, nIndex);
		if (pstUDA)
		{
			pszName = RpUserDataArrayGetName(pstUDA);
			if (!strcmp(pszName, "IB" ))
			{
				bFind = true;
				break;
			}
		}
	}

	if (!bFind)
	{
		nIndex = RpGeometryAddUserDataArray(pGeom, "IB", rpINTUSERDATA, 1);
		if( nIndex != -1 )
		{
			// Type과 ID를 Set한다.
			pstUDA = RpGeometryGetUserDataArray(pGeom, nIndex);
		}
	}

	if (pstUDA)
	{
		RpUserDataArraySetInt(pstUDA, 0, isBill);
	}
}

int		AcuObject::GetAtomicIsBillboard(RpAtomic* pAtomic)
{
	if (!pAtomic)
		return 0;

	RpGeometry* pGeom = RpAtomicGetGeometry(pAtomic);
	if(pGeom == NULL) return 0;

	RpUserDataArray	*	pstUDA;
	INT32				nIndex,nMax;
	RwChar*				pszName;

	nMax = RpGeometryGetUserDataArrayCount(pGeom);
	for (nIndex = 0; nIndex < nMax; ++nIndex)
	{
		pstUDA = RpGeometryGetUserDataArray(pGeom, nIndex);
		if (pstUDA)
		{
			pszName = RpUserDataArrayGetName(pstUDA);
			if (!strcmp(pszName, "IB" ))
			{
				return RpUserDataArrayGetInt(pstUDA, 0);
			}
		}
	}
	return 0;		// false
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID	AcuObject::SetClumpDInfo_Distance	( RpClump *pstClump, float Distance, UINT32 UpdateTick)
{
	if (pstClump)
	{
		pstClump->stUserData.calcDistance = Distance;
		pstClump->stUserData.calcDistanceTick = UpdateTick;
	}
}

VOID	AcuObject::SetClumpDInfo_Shadow	( RpClump *pstClump, int	ShadowLevel)
{
	if (pstClump)
	{
		pstClump->stUserData.characterShadowLevel = ShadowLevel;
	}
}

VOID	AcuObject::SetClumpDInfo_LODLevel	( RpClump *pstClump, int	LODLevel)
{
	if (pstClump)
	{
		pstClump->stUserData.maxLODLevel = LODLevel;
	}
}

/*INT32*	AcuObject::GetClumpDInfo	( RpClump *pstClump	)
{
	if (!pstClump)
		return NULL;

	RpUserDataArray	*pstUDA;
	pstUDA = RwFrameGetUserDataArray(RpClumpGetFrame(pstClump), DISTANCEINFOSLOT);	

#ifdef	_DEBUG_ACUOBJECT
	if(pstUDA)
	{
		RwChar*		pszName = RpUserDataArrayGetName(pstUDA);		// check!!
		RwInt32		nCount = RpUserDataArrayGetNumElements(pstUDA);

		if (strcmp(pszName, DINFONAME) || nCount != DI_ALLOC_NUM)
		{
			ASSERT(!"GetClumpDInfo Check!!");
		}
	}
#endif

	if( pstUDA )
	{
		return (INT32*)pstUDA->data;
	}
	else return NULL;
}*/

////////////////////////////////////////////////////////////////////////////////////////////////// add by taihyung 03.4.17
VOID			AcuObject::SetAtomicRenderUDA(RpAtomic *pstAtomic,UINT32 rendertype,RpAtomicCallBackRender cbRender,INT32 bmode,BOOL bBill,INT32 shadertype)
{
	pstAtomic->stRenderInfo.renderType = rendertype;
	pstAtomic->stRenderInfo.backupCB = (PVOID)cbRender;
	pstAtomic->stRenderInfo.blendMode = bmode;
	pstAtomic->stRenderInfo.isHasBillboard = bBill;
	pstAtomic->stRenderInfo.shaderUseType = shadertype;	
	pstAtomic->stRenderInfo.beforeLODLevel= -2;
}

/*RpAtomicCallBackRender	AcuObject::GetAtomicRenderCallBack(RpAtomic* pstAtomic)
{
	RwFrame *		pstFrame = RpAtomicGetFrame(pstAtomic);
	if(pstFrame == NULL) return NULL;

	RpUserDataArray	*	pstUDA;

#ifdef	_DEBUG_ACUOBJECT
	if(pstUDA)
	{
		RwChar*		pszName = RpUserDataArrayGetName(pstUDA);		// check!!
		RwInt32		nCount = RpUserDataArrayGetNumElements(pstUDA);

		if (strcmp(pszName, RENDERINFONAME) || nCount != RI_ALLOC_NUM)
		{
			ASSERT(!"GetAtomicRenderCallBack Check!!");
		}
	}
#endif

	pstUDA = RwFrameGetUserDataArray(pstFrame, RENDERINFOSLOT);
	if (pstUDA)
	{
		return (RpAtomicCallBackRender)RpUserDataArrayGetInt(pstUDA,RI_CB);
	}
	
	return NULL;
}

INT32*		AcuObject::GetAtomicRenderArray(RpAtomic *pstAtomic)
{
	RwFrame *		pstFrame = RpAtomicGetFrame(pstAtomic);
	if(pstFrame == NULL) return false;

	RpUserDataArray	*	pstUDA;
	pstUDA = RwFrameGetUserDataArray(pstFrame, RENDERINFOSLOT);

#ifdef	_DEBUG_ACUOBJECT
	if(pstUDA)
	{
		RwChar*		pszName = RpUserDataArrayGetName(pstUDA);		// check!!
		RwInt32		nCount = RpUserDataArrayGetNumElements(pstUDA);

		if (strcmp(pszName, RENDERINFONAME) || nCount != RI_ALLOC_NUM)
		{
			ASSERT(!"GetAtomicRenderArray Check!!");
		}
	}
#endif
		
	if(pstUDA)	return (INT32*)pstUDA->data;
	else return NULL;
}

INT32*		AcuObject::GetClumpRenderArray(RpClump*	pstClump)
{
	RwFrame *		pstFrame = RpClumpGetFrame(pstClump);
	if(pstFrame == NULL) return false;

	RpUserDataArray	*	pstUDA;
	pstUDA = RwFrameGetUserDataArray(pstFrame, RENDERINFOSLOT);

#ifdef	_DEBUG_ACUOBJECT
	if(pstUDA)
	{
		RwChar*		pszName = RpUserDataArrayGetName(pstUDA);		// check!!
		RwInt32		nCount = RpUserDataArrayGetNumElements(pstUDA);

		if (strcmp(pszName, RENDERINFONAME) || nCount != RI_ALLOC_NUM)
		{
			ASSERT(!"GetClumpRenderArray Check!!");
		}
	}
#endif

	if(pstUDA)	return (INT32*)pstUDA->data;
	else return NULL;
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////
float*		AcuObject::MakeAtomicBackFrameInfo(RpAtomic*	pstAtomic)
{
	RwFrame *			pFrame;
	pFrame = RpAtomicGetFrame(pstAtomic);

	RpUserDataArray	*	pstUDA = NULL;
	INT32				nIndex,nMax;
	RwChar*				pszName;

	nMax = RwFrameGetUserDataArrayCount(pFrame);
	for (nIndex = nMax-1; nIndex >= 0; --nIndex)
	{
		pstUDA = RwFrameGetUserDataArray(pFrame, nIndex);
		if (pstUDA)
		{
			// 그중에 UserDataArray의 이름이 BackFrame인 것을 검사해서
			pszName = RpUserDataArrayGetName(pstUDA);
			if (!strcmp(pszName , BACKFRAMEINFONAME ))
			{
				break;
			}
		}
		pstUDA = NULL;
	}

	if (!pstUDA)
	{
		nIndex = RwFrameAddUserDataArray(pFrame, BACKFRAMEINFONAME, rpREALUSERDATA , 9);
		if( nIndex != -1 )
		{
			pstUDA = RwFrameGetUserDataArray(pFrame, nIndex);
		}
	}

	if (pstUDA)
	{
		return (float*)pstUDA->data;
	}

	return NULL;
}

float*		AcuObject::GetAtomicBackFrameInfo(RpAtomic *pstAtomic)
{
	RwFrame*	pFrame;
	pFrame = RpAtomicGetFrame(pstAtomic);
	
	if(pFrame == NULL) return false;

	RpUserDataArray	*	pstUDA;
	INT32				nIndex,nMax;
	RwChar*				pszName;

	nMax = RwFrameGetUserDataArrayCount(pFrame);
	// 주로 뒤에 있다 
	for (nIndex = nMax-1; nIndex >= 0 ; --nIndex)
	{
		pstUDA = RwFrameGetUserDataArray(pFrame, nIndex);
		if (pstUDA)
		{
			pszName = RpUserDataArrayGetName(pstUDA);
			if (!strcmp(pszName, BACKFRAMEINFONAME ))
			{
				return (float*)pstUDA->data;
			}
		}

		pstUDA = NULL;
	}
	
	return NULL;
}

float*		AcuObject::MakeAtomicSectorModeBackup(RpAtomic*	pstAtomic)	
{
	RwFrame *			pFrame;
	pFrame = RpAtomicGetFrame(pstAtomic);

	RpUserDataArray	*	pstUDA = NULL;
	INT32				nIndex,nMax;
	RwChar*				pszName;

	nMax = RwFrameGetUserDataArrayCount(pFrame);
	for (nIndex = nMax-1; nIndex >= 0; --nIndex)
	{
		pstUDA = RwFrameGetUserDataArray(pFrame, nIndex);
		if (pstUDA)
		{
			// 그중에 UserDataArray의 이름이 BackFrame인 것을 검사해서
			pszName = RpUserDataArrayGetName(pstUDA);
			if (!strcmp(pszName , SECTORMODEBACKUPNAME ))
			{
				break;
			}
		}
		pstUDA = NULL;
	}

	if (!pstUDA)
	{
		nIndex = RwFrameAddUserDataArray(pFrame, SECTORMODEBACKUPNAME, rpREALUSERDATA , 3);
		if( nIndex != -1 )
		{
			pstUDA = RwFrameGetUserDataArray(pFrame, nIndex);
		}
	}

	if (pstUDA)
	{
		return (float*)pstUDA->data;
	}

	return NULL;
}

float*		AcuObject::GetAtomicSectorModeBackup(RpAtomic *pstAtomic)
{
	RwFrame*	pFrame;
	pFrame = RpAtomicGetFrame(pstAtomic);
	
	if(pFrame == NULL) return false;

	RpUserDataArray	*	pstUDA;
	INT32				nIndex,nMax;
	RwChar*				pszName;

	nMax = RwFrameGetUserDataArrayCount(pFrame);
	// 주로 뒤에 있다 
	for (nIndex = nMax-1; nIndex >= 0 ; --nIndex)
	{
		pstUDA = RwFrameGetUserDataArray(pFrame, nIndex);
		if (pstUDA)
		{
			pszName = RpUserDataArrayGetName(pstUDA);
			if (!strcmp(pszName, SECTORMODEBACKUPNAME ))
			{
				return (float*)pstUDA->data;
			}
		}

		pstUDA = NULL;
	}
	
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 030303 Bob님 수정-LOD 없는 놈땜시...
RpGeometry *		AcuObject::CBSetGeometryPreLitLum(RpGeometry *pstGeometry, PVOID pvData)
{
	RwRGBA *		pstPreLitLum = (RwRGBA *) pvData;
/*	INT32			nIndex;

	// Geometry에 Vertices 정보가 있으면
	if (pstGeometry && pstGeometry->numVertices)
	{
		// Pre Light가 NULL이면, 메모리를 할당해준다.
		if (!pstGeometry->preLitLum)
		{
			pstGeometry->preLitLum = (RwRGBA *) malloc(sizeof(RwRGBA) * pstGeometry->numVertices);
		}

		if (pstGeometry->preLitLum)
		{
			// 인자로 받은 RGBA 값으로 세팅
			for (nIndex = 0; nIndex < pstGeometry->numVertices; nIndex++)
			{
				memcpy(pstGeometry->preLitLum + nIndex, pstPreLitLum, sizeof(RwRGBA));
			}

			// prelight가 먹도록 flag를 바꿔준다.
			RpGeometrySetFlags(pstGeometry, RpGeometryGetFlags(pstGeometry) | rpGEOMETRYPRELIT);
		}

		// 
		RpGeometryLock(pstGeometry, rpGEOMETRYLOCKALL);
		RpGeometryUnlock(pstGeometry);
	}

	return pstGeometry;*/

	return AcuObject::SetGeometryPreLitLum(pstGeometry, pstPreLitLum);
}

// 030303 Bob님 추가-LOD 없는 놈땜시...
RpGeometry *AcuObject::SetGeometryPreLitLum(RpGeometry *pstGeometry, RwRGBA *pstPreLitLum)
{
	// 마고자 (2004-05-10 오후 5:58:33) : 	기능 블럭..
	return pstGeometry;

	// Prelight 정보가 있다면 Setting하고,
	if (pstPreLitLum)
	{
		// Geometry에 Vertices 정보가 있으면
		if(pstGeometry && pstGeometry->numVertices && RpGeometryGetTriangles(pstGeometry))
		{
			// Pre Light가 NULL이면, 메모리를 할당해준다.
			if(!pstGeometry->preLitLum)
			{
				pstGeometry->preLitLum = (RwRGBA *)(malloc(sizeof(RwRGBA) * pstGeometry->numVertices));
			}

			if(pstGeometry->preLitLum)
			{
				// 인자로 받은 RGBA 값으로 세팅
				for( INT32 nIndex = 0; nIndex < pstGeometry->numVertices; ++nIndex )
				{
					memcpy(pstGeometry->preLitLum + nIndex, pstPreLitLum, sizeof(RwRGBA));
				}

				// prelight가 먹도록 flag를 바꿔준다.
				RpGeometrySetFlags(pstGeometry, RpGeometryGetFlags(pstGeometry) | rpGEOMETRYPRELIT);
			}

			RpGeometryLock(pstGeometry, rpGEOMETRYLOCKALL);
			RpGeometryUnlock(pstGeometry);
		}
	}
	else if (RpGeometryGetFlags(pstGeometry) & rpGEOMETRYPRELIT)
	{
		RpGeometrySetFlags(pstGeometry, RpGeometryGetFlags(pstGeometry) & ~rpGEOMETRYPRELIT);

		RpGeometryLock(pstGeometry, rpGEOMETRYLOCKALL);
		RpGeometryUnlock(pstGeometry);
	}

	return pstGeometry;
}

// 030303 Bob님 수정-LOD 없는 놈땜시...
VOID			AcuObject::SetAtomicPreLitLim(RpAtomic *pstAtomic, RwRGBA *pstPreLitLum)
{
	// 마고자 (2004-05-10 오후 5:58:33) : 	기능 블럭..
	return;

	if (!pstAtomic || !pstPreLitLum)
		return;

	RpGeometry *pstGeometry = RpAtomicGetGeometry(pstAtomic);
	if(pstGeometry)
	{
		AcuObject::SetGeometryPreLitLum(pstGeometry, pstPreLitLum);
	}
	
//	RpGeometry *pGeom;

	RpLODAtomicForAllLODGeometries(pstAtomic, CBSetGeometryPreLitLum, pstPreLitLum);
}

RpAtomic *		AcuObject::CBSetAtomicPreLitLim(RpAtomic *pstAtomic, PVOID pvPreLitLum)
{
	// 마고자 (2004-05-10 오후 5:58:33) : 	기능 블럭..
	return pstAtomic;

	SetAtomicPreLitLim(pstAtomic, (RwRGBA *) pvPreLitLum);

	return pstAtomic;
}

VOID			AcuObject::SetClumpPreLitLim(RpClump *pstClump, RwRGBA *pstPreLitLum)
{
	// 마고자 (2004-05-10 오후 5:58:33) : 	기능 블럭..
	return;

	RpClumpForAllAtomics(pstClump, CBSetAtomicPreLitLim, pstPreLitLum);

	return;
}

VOID			AcuObject::SetCamera(RwCamera *pstCamera)
{
	// Geometry Instancing에 필요한 Camera를 Set한다.
	m_pstCamera = pstCamera;
}

BOOL			AcuObject::InstanceAtomic(RpAtomic *pstAtomic)
{
	if (!RpAtomicGetGeometry(pstAtomic) || (RpGeometryGetFlags(RpAtomicGetGeometry(pstAtomic)) & rpGEOMETRYNATIVE))
		return TRUE;

	// Camera가 없으면, 당근 안된다.
	if (!m_pstCamera)
		return FALSE;

	// Camera Begin Update가 안되도 안된다.
	if (!RwCameraBeginUpdate(m_pstCamera))
		return FALSE;

	RwCamera *pstCamera = RwCameraGetCurrentCamera();

	// Instancing
//	RpAtomicRender(pstAtomic);
	RpAtomicInstance(pstAtomic);

	RwCameraEndUpdate(m_pstCamera);

	return TRUE;
}

RpAtomic *		AcuObject::CBInstanceAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	INT32	lIndex;
	BOOL	bIsSkin;
	BOOL	bLODInstance;

	bIsSkin = AcuObject_RwUtilAtomicHasSkin(pstAtomic);
	if (!InstanceAtomic(pstAtomic))
		return NULL;

	for (lIndex = 1; lIndex < RPLODATOMICMAXLOD; ++lIndex)
	{
		if (!RpLODAtomicSetCurrentLOD(pstAtomic, lIndex))
			continue;

		if((bIsSkin) && (!AcuObject_RwUtilAtomicHasSkin(pstAtomic)))
			bLODInstance = FALSE;
		else
			bLODInstance = TRUE;

		if(bLODInstance)
		{
			if (!InstanceAtomic(pstAtomic))
				return NULL;
		}
	}

	return pstAtomic;
}

BOOL			AcuObject::InstanceClump(RpClump *pstClump)
{
	if (!RpClumpForAllAtomics(pstClump, CBInstanceAtomic, NULL))
		return FALSE;

	return TRUE;
}

RpGeometry *AcuObject::CBSetGeometryMaterialAlphaFlagOff(RpGeometry *geom, void *data)
{
	return AcuObject::SetGeometryMaterialAlphaFlagOff(geom);
}

RpGeometry *AcuObject::SetGeometryMaterialAlphaFlagOff(RpGeometry *geom)
{
	RpMaterial*		pMaterial;
	RwRGBA			pNewRGBA;
	INT32			lMaterialCount;	
	
	RpGeometrySetFlags( geom, RpGeometryGetFlags( geom ) & ~rpGEOMETRYMODULATEMATERIALCOLOR );
	
	for ( lMaterialCount = 0 ; lMaterialCount < RpGeometryGetNumMaterials(geom); ++lMaterialCount )
	{
		pMaterial = RpGeometryGetMaterial( geom, lMaterialCount );
		if ( !pMaterial ) return NULL;

		pNewRGBA.red = 255;
		pNewRGBA.green = 255;
		pNewRGBA.blue = 255;
		pNewRGBA.alpha =  120;

		pMaterial->surfaceProps.specular = 1.0f;

		RpMaterialSetColor( pMaterial, &pNewRGBA );
	}

	return geom;
}

/*****************************************************************
*   Callback Function : CBSetAtomicMaterialAlphaGeometryFlagOff
*   Comment  : Atomic Material에 Alpha값을 준다. Geometry의 Materialmodulate~ Flag를 off한다. 
*   Date&Time : 2003-03-05, 오후 10:31
*   Code By : Seong Yon-jun@NHN Studio
*	Modify : 2003-06-20, 오전 11:22(Bob) 
*****************************************************************/
RpAtomic* AcuObject::CBSetAtomicMaterialAlphaGeometryFlagOff( RpAtomic* atomic, void *data )
{
	RpGeometry*		pGeometry;
		
	pGeometry = RpAtomicGetGeometry(atomic);
	if(pGeometry)
	{
		AcuObject::SetGeometryMaterialAlphaFlagOff(pGeometry);
	}

	RpLODAtomicForAllLODGeometries(atomic, CBSetGeometryMaterialAlphaFlagOff, NULL);

	return atomic;
}

VOID AcuObject::SetClumpMaterialAlphaGeometryFlagOff(RpClump *clump)
{
	RpClumpForAllAtomics(clump, CBSetAtomicMaterialAlphaGeometryFlagOff, NULL);

	return;
}

INT32	AcuObject::GetType			( int index )
{
	// 비트플래그만 적용함..
	return	index & ACUOBJECT_TYPE_TYPEFILEDMASK;
}

INT32	AcuObject::GetProperty		( int index )
{
	// 비트플래그만 적용함..
	return	index & ACUOBJECT_TYPE_PROPERTY_FILTER;
}

// ############################### RELEASE DATAS !!! ##########################
VOID			AcuObject::DestroyClumpData(RpClump *pstClump)
{
/*	RpUserDataArray	*	pstUDA;
	INT32			nIndex,nMax;
//	RwChar*			pszName;

	// 모든 UserDataArray를 제거한다.
	nMax = RwFrameGetUserDataArrayCount(RpClumpGetFrame(pstClump));
	for (nIndex = 0; nIndex < nMax; ++nIndex)
	{
		//pstUDA = RwFrameGetUserDataArray(RpClumpGetFrame(pstClump), nIndex);
		//if( pstUDA )
		//{
		//	pszName = RpUserDataArrayGetName(pstUDA);
		//	if (!strcmp(pszName, DINFONAME ))
		//	{
			//	 new는 추가시에 .. 삭제는 여기서 해준다 
		//		DInfo*	pData = (DInfo*) RpUserDataArrayGetInt(pstUDA,0);
		//		delete []pData;
		//	}
		//}
		RwFrameRemoveUserDataArray(RpClumpGetFrame(pstClump), nIndex);
	}

	RpClumpForAllAtomics(pstClump, AcuObject::DestroyAtomicDataCB, NULL);*/
}

RpAtomic*	AcuObject::DestroyAtomicDataCB(RpAtomic *atomic, void *data)
{
	AcuObject::DestroyAtomicData(atomic);

	return atomic;
}

VOID			AcuObject::DestroyAtomicData(RpAtomic *pstAtomic)
{
/*	INT32			nIndex,nMax;

	RpGeometry *pGeom = RpAtomicGetGeometry(pstAtomic);
	if(!pGeom)
		return;

	nMax = RpGeometryGetUserDataArrayCount(pGeom);
	RpUserDataArray	*	pstUDA;

	for (nIndex = 0; nIndex < nMax; ++nIndex)
	{
		pstUDA = RpGeometryGetUserDataArray(pGeom, nIndex);
		RpGeometryRemoveUserDataArray(pGeom, nIndex);
	}

	if(!RpAtomicGetClump(pstAtomic))
	{
		RwFrame*	pFrame = RpAtomicGetFrame(pstAtomic);
		nMax = RwFrameGetUserDataArrayCount(pFrame);

		for(nIndex = 0; nIndex < nMax; ++nIndex)
		{
			pstUDA = RwFrameGetUserDataArray(pFrame, nIndex);
			RwFrameRemoveUserDataArray(pFrame, nIndex);
		}
	}*/
}

struct RpUserDataList
{
    RwInt32             numElements;
    RpUserDataArray    *userData;
};

void		AcuObject::DestroyAtomicUD(RpAtomic*	pAtomic)
{
	RpGeometry *		pGeom = RpAtomicGetGeometry(pAtomic);
	INT32				lPluginOffset = RpGeometryGetPluginOffset(rwID_USERDATAPLUGIN);
	RpUserDataList *	pstUserData;

	if(!pGeom)
		return;

	pstUserData = (RpUserDataList *) (((CHAR *) pGeom) + lPluginOffset);

	pstUserData->numElements = 0;
	pstUserData->userData = NULL;

	/*
	INT32			nIndex,nMax;

	RpGeometry *pGeom = RpAtomicGetGeometry(pAtomic);
	if(!pGeom)
		return;

	nMax = RpGeometryGetUserDataArrayCount(pGeom);
	RpUserDataArray	*	pstUDA;

	for (nIndex = 0; nIndex < nMax; ++nIndex)
	{
		pstUDA = RpGeometryGetUserDataArray(pGeom, nIndex);
		RpGeometryRemoveUserDataArray(pGeom, nIndex);
	}

	RpClump*	pClump = RpAtomicGetClump(pAtomic);
	if(pClump)
	{
		RwFrame*	pFrame1 = RpClumpGetFrame(pClump);
		RwFrame*	pFrame2 = RpAtomicGetFrame(pAtomic);
		if(pFrame1 != pFrame2)	// frame이 같으면 안지운다
		{
			nMax = RwFrameGetUserDataArrayCount(pFrame2);

			for(nIndex = 0; nIndex < nMax; ++nIndex)
			{
				pstUDA = RwFrameGetUserDataArray(pFrame2, nIndex);
				RwFrameRemoveUserDataArray(pFrame2, nIndex);
			}
		}
	}
	else
	{
		RwFrame*	pFrame2 = RpAtomicGetFrame(pAtomic);
		nMax = RwFrameGetUserDataArrayCount(pFrame2);

		for(nIndex = 0; nIndex < nMax; ++nIndex)
		{
			pstUDA = RwFrameGetUserDataArray(pFrame2, nIndex);
			RwFrameRemoveUserDataArray(pFrame2, nIndex);
		}
	}
	*/
}

INT32*	AcuObject::GetOcTreeID(RpClump*	clump)
{
	INT32			nIndex,nMax;
	RwChar*			pszName;
	RpUserDataArray	*pstUDA = NULL;

	RwFrame*		frame = RpClumpGetFrame(clump);

	nMax = RwFrameGetUserDataArrayCount(frame);
	for (nIndex = 0; nIndex < nMax; ++nIndex)
	{
		pstUDA = RwFrameGetUserDataArray(frame, nIndex);
		if( pstUDA )
		{
			pszName = RpUserDataArrayGetName(pstUDA);

			if (!strcmp(pszName, OCTREEID ))
			{
				break;
			}
		}

		pstUDA = NULL;
	}

	if (!pstUDA)
	{
		nIndex = RwFrameAddUserDataArray(frame, OCTREEID, rpINTUSERDATA, ALLOC_OCTREE_ID_TEST_UDA_NUM);
		if( nIndex != -1 )
		{
			pstUDA = RwFrameGetUserDataArray(frame, nIndex);
		}

		for(int i =0;i<ALLOC_OCTREE_ID_TEST_UDA_NUM;++i)
		{
			RpUserDataArraySetInt(pstUDA, i, 0);
		}
	}

	if(pstUDA)
		return (INT32*)pstUDA->data;
	else
		return NULL;
}

RwFrame*	AcuObject::CB_GetHeirarchy( RwFrame *lpFrm, void *lplpHierarch )
{
	ASSERT( lpFrm );
	if( !lpFrm )
		return NULL;
	
	RpHAnimHierarchy **pHierarchy = (RpHAnimHierarchy **)lplpHierarch;

    *pHierarchy = RpHAnimGetHierarchy(lpFrm);

    if (*pHierarchy == NULL)
    {
        RwFrameForAllChildren(lpFrm, CB_GetHeirarchy, lplpHierarch);
        return lpFrm;
    }

    return (RwFrame *)NULL;
}

RpHAnimHierarchy*	AcuObject::GetHierarchyFromFrame( RwFrame* pFrm )
{
	ASSERT( pFrm );
	if( !pFrm )
		return NULL;

	RpHAnimHierarchy *pr	=
		RpHAnimGetHierarchy( pFrm );
	if( !pr )
	{
		RwFrameForAllChildren( pFrm, CB_GetHeirarchy, static_cast<void*>(&pr) );
	}

	return pr;
};

RpAtomic*	AcuObject::SetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
#if (defined(RPSKIN310_H))
    RpSkinSetHAnimHierarchy(RpSkinAtomicGetSkin(atomic),
                            (RpHAnimHierarchy *)data);
#else // (defined(RPSKIN310_H))
    RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy *)data);
#endif // (defined(RPSKIN310_H)) 
	return atomic;
}



INT32 AcuObject::GetAllTriIndexThatIsInTile
(CONTAINER_TRIINDEX& container, RpAtomic* atom, const RwV3d& leftTop, const RwV3d& rightBottom)
{
	USING_ACUMATH;

	if( !atom )
		return -1;
	RpGeometry*	 geo	= RpAtomicGetGeometry( atom );
	if( !geo )
		return -1;
	RpMorphTarget*	morph	= RpGeometryGetMorphTarget(geo, 0);
	if( !morph )
		return -1;
	RwV3d*			verts	= RpMorphTargetGetVertices( morph );
	if( !verts )
		return -1;
	RpTriangle*		tri		= RpGeometryGetTriangles(geo);
	RwInt32			numtri	= RpGeometryGetNumTriangles(geo);
	RwInt32			numvtx	= RpGeometryGetNumVertices(geo);

	const RwMatrix*	ltm = NULL;
	if(RpAtomicGetFrame(atom))
		ltm = RwFrameGetLTM (RpAtomicGetFrame(atom));
	else
		return -1;
	if( !ltm )
		return -1;



	RwV3d	vtxTri[3] = {	{0.f,0.f,0.f,},
						{0.f,0.f,0.f,},
						{0.f,0.f,0.f,},
	};
	const RwV3d	center	= {	(leftTop.x + rightBottom.x)*0.5f,
							(leftTop.y + rightBottom.y)*0.5f,
							(leftTop.z + rightBottom.z)*0.5f, };
	const RwV3d tmp		= { rightBottom.x - leftTop.x,
							0.f,
							rightBottom.z - leftTop.z,};
	const RwReal radius = RwV3dLength(&tmp)*0.5f;

	for( RwInt32 i=0; i<numtri; ++i, ++tri )
	{
		RwV3dTransformPoint (vtxTri+0, &verts[ tri->vertIndex[0] ], ltm);
		RwV3dTransformPoint (vtxTri+1, &verts[ tri->vertIndex[1] ], ltm);
		RwV3dTransformPoint (vtxTri+2, &verts[ tri->vertIndex[2] ], ltm);

		if( intsct2D_TriCircle( *(vtxTri+0), *(vtxTri+1), *(vtxTri+2), center, radius ) )
			container.push_back(i);
	}

	return container.size();
};

class fnctrIntsctLineTri
{
private:
	const RwLine*	pLine_;
	RwLine			theLine_;
	RwV3d*			pVerts_;
	RpTriangle*		pTri_;
	RwInt32			numVerts_;
	RwInt32			numTri_;
	RwMatrix*		ltm_;

	RwReal*			pDist_;

	RwV3d			lineDisplacement_;
	RwBool			isOK_;
	RwBool			isIntsct_;

public:
	fnctrIntsctLineTri(RpAtomic* atom, const RwLine* line, RwReal* dist) 
		: pLine_(line)
		, pDist_(dist)
		, isOK_(FALSE)
		, isIntsct_(FALSE)
	{
		if( atom && pLine_ && pDist_)
		{
			ltm_ = RwFrameGetLTM(RpAtomicGetFrame(atom));
			ASSERT( ltm_ );
			RwMatrix mat;
			RwMatrixInvert(&mat, ltm_);
			RwV3dTransformPoints(&theLine_.start, &pLine_->start, 2, &mat);

			RwV3dSub( &lineDisplacement_, &theLine_.end, &theLine_.start );
			*pDist_	= RwV3dDotProduct(&lineDisplacement_, &lineDisplacement_);
			if( *pDist_ < 1.f )
				*pDist_ = sqrtf(*pDist_);

			RpGeometry* geo = RpAtomicGetGeometry( atom );
			if( geo )
			{
				RpMorphTarget*	morph = RpGeometryGetMorphTarget(geo, 0);

				if(morph)
				{
					pVerts_		= RpMorphTargetGetVertices( morph );
					pTri_		= RpGeometryGetTriangles(geo);
					numVerts_	= RpGeometryGetNumVertices(geo);
					numTri_		= RpGeometryGetNumTriangles(geo);
					if( pVerts_ && pTri_ )
					{
						isOK_ = TRUE;
					}
				}
			}
		}

		ASSERT( isOK_ );
	};

	RwBool IsOk(void)const
	{
		return isOK_;
	};
	RwBool IsIntersect(void)const
	{
		return isIntsct_;
	};

	RwBool operator() (RwInt32 triIndex)
	{
		USING_ACUMATH;

		ASSERT( isOK_ );
		if( !isOK_ )
			return FALSE;

		ASSERT( triIndex < numTri_ );
		if( triIndex >= numTri_ ){
			isOK_ = FALSE;
			return FALSE;
		}
		RpTriangle* tri = pTri_ + triIndex;

		ASSERT( tri->vertIndex[0] < numVerts_ 
			 && tri->vertIndex[1] < numVerts_ 
			 && tri->vertIndex[2] < numVerts_ );
		if( tri->vertIndex[0] >= numVerts_
		 || tri->vertIndex[1] >= numVerts_
		 || tri->vertIndex[2] >= numVerts_
		 ){
			isOK_ = FALSE;
			return FALSE;
		 }

		RwReal dist = *pDist_;
		//RwV3d	vtx[3];
		//RwV3dTransformPoint (vtx+0, pVerts_ + tri->vertIndex[0], ltm_);
		//RwV3dTransformPoint (vtx+1, pVerts_ + tri->vertIndex[1], ltm_);
		//RwV3dTransformPoint (vtx+2, pVerts_ + tri->vertIndex[2], ltm_);
		RwBool br =
		RtIntersectionLineTriangle( &theLine_.start
								  , &lineDisplacement_
								  , pVerts_ + tri->vertIndex[0]
								  , pVerts_ + tri->vertIndex[1]
								  , pVerts_ + tri->vertIndex[2]
								  , &dist
								  );

		*pDist_ = T_MIN(*pDist_, dist);
		isIntsct_ |= br;
		return br;
	};
};
INT32 AcuObject::IntsctLineTriInAtomic
( RpAtomic* atom, const CONTAINER_TRIINDEX& triindices, const RwLine& line, BOOL nearest, RwReal* dist )
{
	RwReal	fdist = 0.f;
	fnctrIntsctLineTri	fnctr(atom, &line, &fdist);

	if(fnctr.IsOk())
	{
		if(nearest)
			std::for_each(triindices.begin(), triindices.end(), fnctr);
		else
			std::find_if(triindices.begin(), triindices.end(), fnctr);
	}
	
	if( !fnctr.IsOk() )
		return -1;//err

	if(fnctr.IsIntersect())
	{
		if( dist )
			*dist = fdist;
		return 1;//intersected
	}
    
	return 0;//not intersected
};
/*
void		AcuObject::SetDefaultAtomicGeometryUD(RpGeometry* pGeom,INT32	isClump)
{
	if (!pGeom)
		return;

	bool	bTypeAdd = false;
	if(!isClump)
	{
		bTypeAdd = true;
	}
	else return;

	RpUserDataArray	*pstUDA;
	RwChar*	pszName;
	RwInt32 nCount;

	UserDataBackup*		list = NULL;
	INT32	nMax = RpGeometryGetUserDataArrayCount(pGeom);
	INT32				nIndex;
	int		mem_alloced = 0;
	
	bool	bCorrect = true;
	int		bModify = 0;

	AuAutoLock	lock(m_csMutexFrameMemory);

	pstUDA = RpGeometryGetUserDataArray(pGeom, GEOM_TYPESLOT);	
	if(pstUDA)
	{
		pszName = RpUserDataArrayGetName(pstUDA);
		nCount = RpUserDataArrayGetNumElements(pstUDA);

		if (strcmp(pszName, m_szName ) || nCount != TYPE_ALLOC_NUM)
		{
			bCorrect = false;
			bModify = 1;

			UserDataBackup*		nw_BackUp = (UserDataBackup*) AllocFrameMemory(DATABACKUPSIZE);
			mem_alloced += DATABACKUPSIZE;

			for(int j=0; j<pstUDA->numElements ; ++j)
			{
				if(pstUDA->format == rpSTRINGUSERDATA )
				{
					char*	pstr = RpUserDataArrayGetString(pstUDA,j);
					strcpy(nw_BackUp->strdata[j],pstr);
				}
				else
				{
					if(pstUDA->format == rpINTUSERDATA )
					{
						INT32*	val = (INT32*)pstUDA->data;
						nw_BackUp->data[j] = (PVOID)val[j];
					}
					else if(pstUDA->format == rpREALUSERDATA )
					{
						float*	val = (float*)pstUDA->data;
						((float*)nw_BackUp->data)[j] = val[j];
					}
				}
			}

			nw_BackUp->format = pstUDA->format;
			nw_BackUp->numelements = pstUDA->numElements;
			strcpy(nw_BackUp->name ,pstUDA->name);

			nw_BackUp->next = list;
			list = nw_BackUp;
		}
	}
	else bModify = 2;

	if(bCorrect == false)		// UD 재배치
	{
		for(int i=1;i<nMax;++i)
		{
			pstUDA = RpGeometryGetUserDataArray(pGeom, i);	
			UserDataBackup*		nw_BackUp = (UserDataBackup*) AllocFrameMemory(DATABACKUPSIZE);
			mem_alloced += DATABACKUPSIZE;

			for(int j=0; j<pstUDA->numElements ; ++j)
			{
				if(pstUDA->format == rpSTRINGUSERDATA )
				{
					char*	pstr = RpUserDataArrayGetString(pstUDA,j);
					strcpy(nw_BackUp->strdata[j],pstr);
				}
				else
				{
					if(pstUDA->format == rpINTUSERDATA )
					{
						INT32*	val = (INT32*)pstUDA->data;
						nw_BackUp->data[j] = (PVOID)val[j];
					}
					else if(pstUDA->format == rpREALUSERDATA )
					{
						float*	val = (float*)pstUDA->data;
						((float*)nw_BackUp->data)[j] = val[j];
					}
				}
			}

			nw_BackUp->format = pstUDA->format;
			nw_BackUp->numelements = pstUDA->numElements;
			strcpy(nw_BackUp->name ,pstUDA->name);

			nw_BackUp->next = list;
			list = nw_BackUp;
		}

		bool	bFind = false;
		UserDataBackup*		find_node = list;
		UserDataBackup*		bef_node = find_node;

		// 0번 검사 & Add
		if(bModify > 0)
		{
			bFind = false;
			find_node= list;
			bef_node= find_node;

			while(find_node)
			{
				if(!strcmp(find_node->name ,m_szName))
				{
					bFind = true;
					if(find_node == list)
					{
						list = find_node->next;
					}
					else
					{
						bef_node->next = find_node->next;
					}

					break;
				}

				bef_node = find_node;
				find_node = find_node->next;
			}

			if(bModify == 1)
			{
				for(i=0;i<nMax;++i)
				{
					RpGeometryRemoveUserDataArray(pGeom, 0);
				}
			}

			RpGeometryAddUserDataArray(pGeom, m_szName, rpINTUSERDATA, TYPE_ALLOC_NUM); // Type 0번에 추가
			pstUDA = RpGeometryGetUserDataArray(pGeom, GEOM_TYPESLOT);
			if(bFind == true)
			{
				for(int j=0; j<pstUDA->numElements ; ++j)
				{
					INT32*	val = (INT32*)pstUDA->data;
					val[j] = (INT32)find_node->data[j];
				}
			}
			else
			{
				for(i=0;i<TYPE_ALLOC_NUM;++i)
				{
					((RwInt32*)pstUDA->data)[i] = 0;
				}
			}
		}
		
		UserDataBackup*		cur_node = list;
		while(cur_node)
		{
			nIndex = RpGeometryAddUserDataArray(pGeom, cur_node->name, cur_node->format,cur_node->numelements);
			pstUDA = RpGeometryGetUserDataArray(pGeom, nIndex);

			for(int j=0;j<cur_node->numelements;++j)
			{
				if(cur_node->format == rpINTUSERDATA )
				{
					RpUserDataArraySetInt(pstUDA,j,(INT32)cur_node->data[j]);
				}
				else if(cur_node->format == rpREALUSERDATA  )
				{
					RpUserDataArraySetReal(pstUDA,j,(FLOAT)((float*)cur_node->data)[j]);
				}
				else if(cur_node->format == rpSTRINGUSERDATA)
				{
					RpUserDataArraySetString(pstUDA,j,cur_node->strdata[j]);
				}
			}

			cur_node = cur_node->next;
		}
	}
	
	if(bModify == 2)
	{
		int		lTemp = nMax;
		
		if(lTemp == 0)
		{
			RpGeometryAddUserDataArray(pGeom, m_szName, rpINTUSERDATA, TYPE_ALLOC_NUM); // Type 0번에 추가
			pstUDA = RpGeometryGetUserDataArray(pGeom, GEOM_TYPESLOT);

			for(int i=0;i<TYPE_ALLOC_NUM;++i)
			{
				((RwInt32*)pstUDA->data)[i] = 0;
			}
		}
	}

	DeallocFrameMemory(mem_alloced);
}

void	AcuObject::SetDefaultAtomicFrameUD(RwFrame* pFrame)
{
	// 현재 default로 붙는 atomic userdata는 없다.. 

	RpUserDataArray	*pstUDA;
	RwChar*	pszName;
	RwInt32	nCount ;
	UserDataBackup*		list = NULL;
	INT32	nMax = RwFrameGetUserDataArrayCount(pFrame);
	INT32				nIndex;
	int		mem_alloced = 0;

	bool	bCorrect = true;
	int		bModify = 0;

	AuAutoLock	lock(m_csMutexFrameMemory);

	pstUDA = RwFrameGetUserDataArray(pFrame, 0);	
	if(pstUDA)
	{
		pszName = RpUserDataArrayGetName(pstUDA);
		nCount = RpUserDataArrayGetNumElements(pstUDA);

		if (strcmp(pszName, RENDERINFONAME ) || nCount != RI_ALLOC_NUM)
		{
			bCorrect = false;
			bModify = 1;
			UserDataBackup*		nw_BackUp = (UserDataBackup*) AllocFrameMemory(DATABACKUPSIZE);
			mem_alloced += DATABACKUPSIZE;

			for(int j=0; j<pstUDA->numElements ; ++j)
			{
				if(pstUDA->format == rpSTRINGUSERDATA )
				{
					char*	pstr = RpUserDataArrayGetString(pstUDA,j);
					strcpy(nw_BackUp->strdata[j],pstr);
				}
				else
				{
					if(pstUDA->format == rpINTUSERDATA )
					{
						INT32*	val = (INT32*)pstUDA->data;
						nw_BackUp->data[j] = (PVOID)val[j];
					}
					else if(pstUDA->format == rpREALUSERDATA )
					{
						float*	val = (float*)pstUDA->data;
						((float*)nw_BackUp->data)[j] = val[j];
					}
				}
			}

			nw_BackUp->format = pstUDA->format;
			nw_BackUp->numelements = pstUDA->numElements;
			strcpy(nw_BackUp->name ,pstUDA->name);

			nw_BackUp->next = list;
			list = nw_BackUp;
		}
	}
	else
	{
		bModify = 2;
	}
	
	if(bCorrect == false)		// UD 재배치
	{
		for(int i=1;i<nMax;++i)
		{
			pstUDA = RwFrameGetUserDataArray(pFrame, i);	
			UserDataBackup*		nw_BackUp = (UserDataBackup*) AllocFrameMemory(DATABACKUPSIZE);
			mem_alloced += DATABACKUPSIZE;

			for(int j=0; j<pstUDA->numElements ; ++j)
			{
				if(pstUDA->format == rpSTRINGUSERDATA )
				{
					char*	pstr = RpUserDataArrayGetString(pstUDA,j);
					strcpy(nw_BackUp->strdata[j],pstr);
				}
				else
				{
					if(pstUDA->format == rpINTUSERDATA )
					{
						INT32*	val = (INT32*)pstUDA->data;
						nw_BackUp->data[j] = (PVOID)val[j];
					}
					else if(pstUDA->format == rpREALUSERDATA )
					{
						float*	val = (float*)pstUDA->data;
						((float*)nw_BackUp->data)[j] = val[j];
					}
				}
			}

			nw_BackUp->format = pstUDA->format;
			nw_BackUp->numelements = pstUDA->numElements;
			strcpy(nw_BackUp->name ,pstUDA->name);

			nw_BackUp->next = list;
			list = nw_BackUp;
		}

		bool	bFind = false;
		UserDataBackup*		find_node = list;
		UserDataBackup*		bef_node = find_node;

		// 0번 검사 & Add
		if(bModify > 0)
		{
			bFind = false;
			find_node= list;
			bef_node= find_node;

			while(find_node)
			{
				if(!strcmp(find_node->name ,RENDERINFONAME))
				{
					bFind = true;
					if(find_node == list)
					{
						list = find_node->next;
					}
					else
					{
						bef_node->next = find_node->next;
					}

					break;
				}

				bef_node = find_node;
				find_node = find_node->next;
			}

			if(bModify == 1)
			{
				for(int i=0;i<nMax;++i)
				{
					pstUDA = RwFrameGetUserDataArray(pFrame, 0);
					if (RpUserDataArrayGetFormat(pstUDA) == rpNAUSERDATAFORMAT)
					{
						pstUDA->name = NULL;
						pstUDA->data = NULL;
					}

					RwFrameRemoveUserDataArray(pFrame, 0);
				}
			}

			RwFrameAddUserDataArray(pFrame, RENDERINFONAME, rpINTUSERDATA, RI_ALLOC_NUM); //Renderinfo 0번에 추가
		
			pstUDA = RwFrameGetUserDataArray(pFrame, 0);
			//RpUserDataArraySetInt(pstUDA, RI_REFCOUNT, 0);	// Clone시 Ref Count -- 0 이 되었을 때 삭제
			
			if(bFind == true)
			{
				for(int j=0; j<pstUDA->numElements ; ++j)
				{
					INT32*	val = (INT32*)pstUDA->data;
					val[j] = (INT32)find_node->data[j];
				}
			}
			else
			{
				for(i=0;i<RI_ALLOC_NUM;++i)
				{
					((RwInt32*)pstUDA->data)[i] = 0;
				}
			}
		}
		
		UserDataBackup*		cur_node = list;
		while(cur_node)
		{
			nIndex = RwFrameAddUserDataArray(pFrame, cur_node->name, cur_node->format,cur_node->numelements);
			pstUDA = RwFrameGetUserDataArray(pFrame, nIndex);

			for(int j=0;j<cur_node->numelements;++j)
			{
				if(cur_node->format == rpINTUSERDATA )
				{
					RpUserDataArraySetInt(pstUDA,j,(INT32)cur_node->data[j]);
				}
				else if(cur_node->format == rpREALUSERDATA  )
				{
					RpUserDataArraySetReal(pstUDA,j,(FLOAT)((float*)cur_node->data)[j]);
				}
				else if(cur_node->format == rpSTRINGUSERDATA)
				{
					RpUserDataArraySetString(pstUDA,j,cur_node->strdata[j]);
				}
			}

			cur_node = cur_node->next;
		}
	}
	
	if(bModify == 2)
	{
		RwFrameAddUserDataArray(pFrame, RENDERINFONAME, rpINTUSERDATA, RI_ALLOC_NUM); //Renderinfo 0번에 추가
		pstUDA = RwFrameGetUserDataArray(pFrame, 0);
		//RpUserDataArraySetInt(pstUDA, RI_REFCOUNT, 0);	// Clone시 Ref Count -- 0 이 되었을 때 삭제

		for(int i=0;i<RI_ALLOC_NUM;++i)
		{
			((RwInt32*)pstUDA->data)[i] = 0;
		}
	}

	DeallocFrameMemory(mem_alloced);
}

void		AcuObject::SetDefaultClumpFrameUD(RwFrame* pFrame)
{
	// 0: Type 
	RpUserDataArray	*pstUDA;
	RwChar*	pszName;
	RwInt32	nCount;
	UserDataBackup*		list = NULL;
	INT32	nMax = RwFrameGetUserDataArrayCount(pFrame);
	INT32				nIndex;

	bool	bCorrect = true;
	int		bModify = 0;
	int		i,j;

	int		mem_alloced = 0;

	pstUDA = RwFrameGetUserDataArray(pFrame, TYPEINFOSLOT);	
	if(pstUDA)
	{
		pszName = RpUserDataArrayGetName(pstUDA);
		nCount = RpUserDataArrayGetNumElements(pstUDA);

		// rpNAUSERDATAFORMAT일때에 대한 Validation Check가 없었다. Parn
		if (RpUserDataArrayGetFormat(pstUDA) == rpNAUSERDATAFORMAT || !pszName || strcmp(pszName, m_szName ) || nCount != TYPE_ALLOC_NUM)
		{
			bCorrect = false;
			bModify = 1;
		}
	}
	else bModify = 2;

	AuAutoLock	lock(m_csMutexFrameMemory);

	if(bCorrect == false)		// UD 재배치
	{
		// 백업..
		for(i=0;i<nMax;++i)
		{
			pstUDA = RwFrameGetUserDataArray(pFrame, i);

			if (RpUserDataArrayGetFormat(pstUDA) == rpNAUSERDATAFORMAT)
				continue;

			UserDataBackup*		nw_BackUp = (UserDataBackup*) AllocFrameMemory(DATABACKUPSIZE);
			mem_alloced += DATABACKUPSIZE;

			for(j=0; j<pstUDA->numElements ; ++j)
			{
				if(pstUDA->format == rpSTRINGUSERDATA )
				{
					char*	pstr = RpUserDataArrayGetString(pstUDA,j);
					strcpy(nw_BackUp->strdata[j],pstr);
				}
				else
				{
					if(pstUDA->format == rpINTUSERDATA )
					{
						INT32*	val = (INT32*)pstUDA->data;
						nw_BackUp->data[j] = (PVOID)val[j];
					}
					else if(pstUDA->format == rpREALUSERDATA )
					{
						float*	val = (float*)pstUDA->data;
						((float*)nw_BackUp->data)[j] = val[j];
					}
				}
			}

			nw_BackUp->format = pstUDA->format;
			nw_BackUp->numelements = pstUDA->numElements;
			strcpy(nw_BackUp->name ,pstUDA->name);

			nw_BackUp->next = list;
			list = nw_BackUp;
		}

		int					nInsertedIndex			;

		for(i=0;i<nMax;++i)
		{
			// ClumpClone을 할때, UserData Copy가 골때리가 된다. -_-; 그래서 어쩔 수 없이, 임시 코드 몇줄 넣음.. Parn
			pstUDA = RwFrameGetUserDataArray(pFrame, 0);

			if (RpUserDataArrayGetFormat(pstUDA) == rpNAUSERDATAFORMAT)
			{
				pstUDA->name = NULL;
				pstUDA->data = NULL;
			}

			VERIFY( RwFrameRemoveUserDataArray( pFrame, 0	) );
		}

		bool				bFind		= false		;
		UserDataBackup *	find_node	= list		;
		UserDataBackup *	bef_node	= find_node	;
		
		while(find_node)
		{
			if(!strcmp(find_node->name ,m_szName))
			{
				bFind = true;
				if(find_node == list)
				{
					list = find_node->next;
				}
				else
				{
					bef_node->next = find_node->next;
				}

				break;
			}

			bef_node = find_node;
			find_node = find_node->next;
		}

		nInsertedIndex = RwFrameAddUserDataArray(pFrame, m_szName, rpINTUSERDATA, TYPE_ALLOC_NUM); //type 0번에 추가
		ASSERT( nInsertedIndex == TYPEINFOSLOT );
		pstUDA = RwFrameGetUserDataArray(pFrame, TYPEINFOSLOT);
		if(bFind == true)
		{
			for(j=0; j<pstUDA->numElements ; ++j)
			{
				((RwInt32*)pstUDA->data)[j] = (INT32)find_node->data[j];
			}
		}
		else
		{
			for(i=0;i<TYPE_ALLOC_NUM;++i)
			{
				((RwInt32*)pstUDA->data)[i] = 0;
			}
		}
		
		UserDataBackup*		cur_node = list;
		while(cur_node)
		{
			nIndex = RwFrameAddUserDataArray(pFrame, cur_node->name, cur_node->format,cur_node->numelements);
			pstUDA = RwFrameGetUserDataArray(pFrame, nIndex);

			for(j=0;j<cur_node->numelements;++j)
			{
				if(cur_node->format == rpINTUSERDATA )
				{
					((RwInt32*)pstUDA->data)[j] = (INT32)cur_node->data[j];
				}
				else if(cur_node->format == rpREALUSERDATA  )
				{
					((RwReal*)pstUDA->data)[j] = (RwReal)((RwReal*)cur_node->data)[j];
				}
				else if(cur_node->format == rpSTRINGUSERDATA)
				{
					RpUserDataArraySetString(pstUDA,j,cur_node->strdata[j]);
				}
			}

			cur_node = cur_node->next;
		}
	}
	
	if( bModify == 2 )
	{
		// 아에 없는거라면 새로 추가함..
		nIndex = RwFrameAddUserDataArray(pFrame, m_szName, rpINTUSERDATA, TYPE_ALLOC_NUM); // Type 0번에 추가
		pstUDA = RwFrameGetUserDataArray(pFrame, TYPEINFOSLOT);
		
		for(i=0;i<TYPE_ALLOC_NUM;++i)
		{
			((RwInt32*)pstUDA->data)[i] = 0;
		}
	}

	DeallocFrameMemory(mem_alloced);
}

void	AcuObject::RearrangeAtomicUserdata(RpAtomic*	atomic)
{
	AcuObject::SetDefaultAtomicFrameUD(RpAtomicGetFrame(atomic));
	if(!RpAtomicGetClump(atomic))	// Clump가 NULL이면 Atomic의 geometry에 Type set
			AcuObject::SetDefaultAtomicGeometryUD(RpAtomicGetGeometry(atomic),0);
	else 
			AcuObject::SetDefaultAtomicGeometryUD(RpAtomicGetGeometry(atomic),1);
}

void	AcuObject::RearrangeClumpUserdata(RpClump*		clump)
{
	AcuObject::SetDefaultClumpFrameUD(RpClumpGetFrame(clump));

	RpClumpForAllAtomics(clump,CBRearrangeUD,NULL);
}

RpAtomic*	AcuObject::CBRearrangeUD(RpAtomic*	atomic,PVOID	pvData)
{
	AcuObject::RearrangeAtomicUserdata(atomic);
	return atomic;
}

  RpGeometry *	AcuObject::CBSetGeometryData( RpGeometry *pstGeometry	, PVOID pvData )
{
	INT32	 bClump  = (INT32)pvData;
	// Atomic의 geometryUD default Set
	SetDefaultAtomicGeometryUD(pstGeometry,bClump);
		
	return pstGeometry;
}

RpAtomic *		AcuObject::CBSetAtomicForGT	( RpAtomic *pstAtomic	, PVOID pvData			)
{
	RpLODAtomicForAllLODGeometries(pstAtomic, CBSetGeometryData, (PVOID)1);

	return pstAtomic;
}
*/