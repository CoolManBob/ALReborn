#include "AcuLOD.h"
#include "AcuObject.h"

static RwFrame	*g_pstMainFrame			= NULL;
//static RwFrame	*g_pstCameraFrame		= NULL;
static UINT32	*g_pulDefaultDistance	= NULL;

typedef struct RpClumpForAllAtomicsSet
{
	RpAtomic	*m_pastAtomic[ACULOD_CLUMP_MAX_ATOMIC];
	RwInt32		m_anAtomicIndex[ACULOD_CLUMP_MAX_ATOMIC];
	UINT16		m_unCount;
} RpClumpForAllAtomicsSet;

/******************************************************************************
* Purpose : LOD 거리를 계산할 때 쓰이는 카메라 frame을 설정한다.
*
* 030503. Bob Jung
******************************************************************************/
/*VOID AcuLOD::SetCameraFrame(RwFrame *pFrame)
{
	g_pstCameraFrame = pFrame;
}*/

/******************************************************************************
* Purpose : LOD 거리를 계산할 기준 frame을 설정한다.
*
* 022403. Bob Jung
******************************************************************************/
VOID AcuLOD::SetMainFrame(RwFrame *pFrame)
{
	g_pstMainFrame = pFrame;
}

/******************************************************************************
* Purpose : 오브젝 타입이 없는 놈들은 g_pulDefaultDistance으로 거리값 비교!
*
* Desc : g_pulDefaultDistance가 NULL이면, LOD callbakc의 리턴값은 0 level이 된다.
*
* 022403. Bob Jung
******************************************************************************/
VOID AcuLOD::SetDefaultDistance(UINT32 *pulDistance)
{
	g_pulDefaultDistance = pulDistance;
}

/******************************************************************************
* Purpose : 0 ~ (RPLODATOMICMAXLOD - 1)까지 기본 geometry를 설정한다.
*
* params :	szInitDFFPath	-	1레벨이후 LOD data로 쓰일 DFF 경로.
*								(해당 clump의 atomic 갯수는 1개이고, bSetNextLevel은 FALSE여야만 한다.
								그리고, szInitDFFPath가 NULL인 경우는 1레벨 이후의 geometry를 NULL로 설정한다.)
*			bSetNextLevel	-	TRUE이면 1레벨 이후의 geometry를 기본 geometry로 설정하고,
*								FALSE이면 NULL로 설정한다.
*
* 022403. Bob Jung
******************************************************************************/
BOOL AcuLOD::Initailize(RpClump *pstClump, CHAR *szInitDFFPath, BOOL bSetNextLevel)
{
	RpGeometry *pstInitGeom = NULL;

	if(szInitDFFPath)
	{
		RpClump *pstInitLODClump = ReadDFF(szInitDFFPath);
		if(!pstInitLODClump)
		{
			OutputDebugString("AcuLOD::Initailize() Error (1) !!!\n");
			return FALSE;
		}

		RpClumpForAllAtomicsSet LODAtomicSet;
		memset(&LODAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));	
		RpClumpForAllAtomics(pstInitLODClump, SetBaseAtomicCB, &LODAtomicSet);

		pstInitGeom = RpAtomicGetGeometry(LODAtomicSet.m_pastAtomic[0]);
		if(!pstInitGeom)
		{
			OutputDebugString("AcuLOD::Initailize() Error (2) !!!\n");
			return FALSE;
		}
	}

	INT16		nLODIndex		= 0;
	INT16		nAtomicCount	= 0;

	RpClumpForAllAtomicsSet BaseAtomicSet;
	memset(&BaseAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstClump, SetBaseAtomicCB, &BaseAtomicSet);

	// 0레벨은 기본 geometry로 설정!
	for(nAtomicCount = 0; nAtomicCount < BaseAtomicSet.m_unCount; ++nAtomicCount)
	{
		RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount], nLODIndex, RpAtomicGetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount]));
	}

	// 1에서 (RPLODATOMICMAXLOD - 1)까지의 레벨을 설정한다.
	for(nLODIndex = 1; nLODIndex < RPLODATOMICMAXLOD; ++nLODIndex)
	{
		for(nAtomicCount = 0; nAtomicCount < BaseAtomicSet.m_unCount; ++nAtomicCount)
		{
			if(bSetNextLevel)
			{
				RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount], nLODIndex, RpAtomicGetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount]));
			}
			else
			{
				if(pstInitGeom)
				{
					RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount], nLODIndex, pstInitGeom);
				}
				else
				{
					RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount], nLODIndex, NULL);
				}
			}
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose : Clump의 atomic 포인터와 atomic갯수를 저장하는 callback.
*
* 022403. Bob Jung
******************************************************************************/
RpAtomic *AcuLOD::SetBaseAtomicCB(RpAtomic *atomic, void *data)
{
	RpClumpForAllAtomicsSet *pBaseAtomicSet = (RpClumpForAllAtomicsSet *)(data);

	pBaseAtomicSet->m_pastAtomic[pBaseAtomicSet->m_unCount] = atomic;
	++pBaseAtomicSet->m_unCount;

	return atomic;
}

/******************************************************************************
* Purpose : Clump에 LOD를 적용시킨다.
*
* Params :	pstClump		- 적용시키고자하는 clump
*			szLODDffName	- LOD 데이터의 DFF name
*			nLevel			- LOD level
*			szDummyDffPath	- 기본 클럼프의 아토믹의 갯수가 LOD 클럼프의 아토믹 갯수 보다 많을 경우,
							이 녀석의 지오메트리로 설정된다(아토믹 갯수는 한개라고 가정한다)
*			bSetNextLevel	- nLevel 이후 레벨의 geometry를 LOD data의 geometry로 설정할건지?
*
* 022403. Bob Jung
******************************************************************************/
BOOL AcuLOD::MakeLODLevel(RpClump *pstClump, CHAR *szLODDffPath, INT16 nLevel, CHAR *szDummyDffPath, BOOL bSetNextLevel)
{
	RpClump		*pstLODClump	= NULL;
	RpClump		*pstDummyClump	= NULL;

	// Base clump
	RpClumpForAllAtomicsSet BaseAtomicSet, LODAtomicSet, DummyAtomicSet;
	memset(&BaseAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstClump, SetBaseAtomicCB, &BaseAtomicSet);

	// LOD data
	pstLODClump = ReadDFF(szLODDffPath);
	if(!pstLODClump)
	{
		OutputDebugString("AcuLOD::MakeLODLevel() Error (1) !!!\n");
		return FALSE;
	}

	memset(&LODAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstLODClump, SetBaseAtomicCB, &LODAtomicSet);

	// Dummy
	pstDummyClump = ReadDFF(szDummyDffPath);
	if(!pstDummyClump)
	{
		RpClumpDestroy(pstLODClump);

		OutputDebugString("AcuLOD::MakeLODLevel() Error (2) !!!\n");
		return FALSE;
	}

	memset(&DummyAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstDummyClump, SetBaseAtomicCB, &DummyAtomicSet);

	INT16 nNextLODIndex = nLevel + 1;
	if(bSetNextLevel)
		nNextLODIndex = RPLODATOMICMAXLOD;

	for(INT16 nLODIndex = nLevel; nLODIndex < nNextLODIndex; ++nLODIndex)	
	{
		for(INT16 nBaseAtomicCount = 0; nBaseAtomicCount < BaseAtomicSet.m_unCount; ++nBaseAtomicCount)
		{
			RpGeometry *pstGeom =	(nBaseAtomicCount >= LODAtomicSet.m_unCount) ? 
									(RpAtomicGetGeometry(DummyAtomicSet.m_pastAtomic[0])) : // Dummy의 atomic 갯수는 1개라고 가정한다.
									(RpAtomicGetGeometry(LODAtomicSet.m_pastAtomic[nBaseAtomicCount]));

			if(!pstGeom)
			{
				RpClumpDestroy(pstDummyClump);
				RpClumpDestroy(pstLODClump);

				OutputDebugString("AcuLOD::MakeLODLevel() Error (3) !!!\n");
				return FALSE;
			}
									
			RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nBaseAtomicCount], nLODIndex, pstGeom);
		}
	}

	RpClumpDestroy(pstDummyClump);
	RpClumpDestroy(pstLODClump);

	return TRUE;
}

/******************************************************************************
* Purpose : DFF file을 읽는다.
*
* 022403. Bob Jung
******************************************************************************/
RpClump *AcuLOD::ReadDFF(CHAR *szDFFPath)
{
	RwStream	*pstStream;
    RpClump		*pstClump = NULL;

    pstStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, szDFFPath);

    if(pstStream)
    {
        if(RwStreamFindChunk(pstStream, rwID_CLUMP, NULL, NULL)) 
        {
            pstClump = RpClumpStreamRead(pstStream);
        }

        RwStreamClose(pstStream, NULL);
    }

    return pstClump;
}

/******************************************************************************
* Purpose : Atomic geometry의 UserData에 설정된 'AtomicIndex"를 가져온다.
*
* Desc : 'AtomicIndex'는 2부터 시작하기 때문에 return 값이 0이면 fail이다.
*
* 022403. Bob Jung
******************************************************************************/
RwInt32 AcuLOD::GetAtomicIndex(RpAtomic *pstAtomic)
{
	RwInt32 nAtomicIndex	= 0;
	RpGeometry *pGeom		= RpAtomicGetGeometry(pstAtomic);	

	if(pGeom)
	{
		RwInt32 nUsrDatNum = RpGeometryGetUserDataArrayCount(pGeom);

		RpUserDataArray *pstUserDataArray;
		RwChar			*pszUserDataArrayName;

		for(RwInt32 nCount = 0; nCount < nUsrDatNum; ++nCount)
		{
			pstUserDataArray		= RpGeometryGetUserDataArray(pGeom, nCount);

			if(pstUserDataArray)
			{
				pszUserDataArrayName	= RpUserDataArrayGetName(pstUserDataArray);

				if(!strcmp(pszUserDataArrayName, "AtomicIndex"))
				{
					nAtomicIndex = RpUserDataArrayGetInt(pstUserDataArray, 0);
					break;
				}
			}
		}
	}

	return nAtomicIndex;
}

/******************************************************************************
* Purpose : Clump의 모든 atomic에 RpLODAtomicSetLODCallBack, RpLODAtomicHookRender를 한다~
*
* 022403. Bob Jung
******************************************************************************/
VOID AcuLOD::SetLODCallback(RpClump *pstClump)
{
	RpClumpForAllAtomicsSet BaseAtomicSet;

	memset(&BaseAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstClump, SetBaseAtomicCB, &BaseAtomicSet);

	for(INT16 nAtomicCount = 0; nAtomicCount < BaseAtomicSet.m_unCount; ++nAtomicCount)
	{
		RpLODAtomicSetLODCallBack(BaseAtomicSet.m_pastAtomic[nAtomicCount], LODCallback);
		RpLODAtomicHookRender(BaseAtomicSet.m_pastAtomic[nAtomicCount]);
	}
}

/******************************************************************************
* Purpose : LOD callback!
*
* 022403. Bob Jung
******************************************************************************/
RwInt32 AcuLOD::LODCallback(RpAtomic *atomic)
{
	if((!g_pstMainFrame)/* || (!g_pstCameraFrame)*/)
	{
		OutputDebugString("LODCallback() Error AcuLOD::g_pstMainFrame이나 AcuLOD::g_pstCameraFrame이 설정되어 있지 않습니다!!!\n");
		return 0; // error
	}

	RwV3d	atomicPos, mainPos, temp;
	RwReal	distance;

/*	RwV3d	cameraAt;

    atomicPos	= RwFrameGetLTM(RpAtomicGetFrame(atomic))->pos;
	mainPos		= RwFrameGetLTM(g_pstMainFrame)->pos;
	cameraAt	= RwFrameGetLTM(g_pstCameraFrame)->at;

	RwV3dSub(&temp, &atomicPos, &mainPos);
	distance = RwV3dDotProduct(&temp, &cameraAt);*/

	atomicPos	= RwFrameGetLTM(RpAtomicGetFrame(atomic))->pos;
	mainPos		= RwFrameGetLTM(g_pstMainFrame)->pos;

	RwV3dSub(&temp, &atomicPos, &mainPos);
	distance = RwV3dLength(&temp);

	///////////////////////////////////////////////////////
	// 비교할 distance를 가져온다.
	UINT32	*pulDistance;

	PVOID	pData			= NULL;
	RpClump *parentClump	= RpAtomicGetClump(atomic);

	AcuObject::GetClumpType(parentClump, NULL, NULL, &pData);

	if(pData)
	{
		pulDistance = (UINT32 *)(pData);
	}
	else
	{
		if(!g_pulDefaultDistance)
			return 0;
		
		pulDistance = g_pulDefaultDistance;
	}
	///////////////////////////////////////////////////////

    for(RwInt32 lodIndex = 0; lodIndex < AGPDLOD_MAX_NUM; lodIndex++)
    {
		if(distance < pulDistance[lodIndex])
        {
            return lodIndex;
        }
    }

    return AGPDLOD_MAX_NUM - 1;
}

/******************************************************************************
******************************************************************************/