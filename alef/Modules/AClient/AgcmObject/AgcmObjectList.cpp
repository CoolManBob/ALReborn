#include "AgcmObjectList.h"

#include "ApMemoryTracker.h"

#define	AGCM_OBJECT_GROUP_POOL				4500
#define	AGCM_OBJECT_TEMPLATE_GROUP_POOL		1000

AgcmObjectList::AgcmObjectList()
{
	m_csGroupPool.Initialize(sizeof(AgcdObjectGroupList), AGCM_OBJECT_GROUP_POOL);

//	m_pstAgcdAnimObjectGroupDataList = NULL;
}

AgcmObjectList::~AgcmObjectList()
{
}

///////////////////////////////////////////////////////////////////////////////
// Animation
/*BOOL AgcmObjectList::RemoveAllAnimObjectGroupData()
{
	AgcdAnimObjectGroupDataList	*pstEntry	= m_pstAgcdAnimObjectGroupDataList;
    AgcdAnimObjectGroupDataList	*pstTemp;

    while(pstEntry)
    {
        pstTemp = pstEntry->m_pstNext;
        free(pstEntry);
	    pstEntry = pstTemp;
    }

	m_pstAgcdAnimObjectGroupDataList		= NULL;

	return TRUE;
}

BOOL AgcmObjectList::AddAnimObjectGroupData(AgcdObjectGroupData *pcsData)
{
	if(CheckAnimObjectGroupData(pcsData))
		return FALSE;

	AgcdAnimObjectGroupDataList	*pstEntry		= (AgcdAnimObjectGroupDataList *)malloc(sizeof(AgcdAnimObjectGroupDataList));

	pstEntry->m_pcsData							= pcsData;
	pstEntry->m_pstNext							= m_pstAgcdAnimObjectGroupDataList;
	pstEntry->m_ulPrevTick						= 0;

	m_pstAgcdAnimObjectGroupDataList			= pstEntry;
	
	return TRUE;
}

BOOL AgcmObjectList::CheckAnimObjectGroupData(AgcdObjectGroupData *pcsData)
{
	AgcdAnimObjectGroupDataList	*pstEntry		= m_pstAgcdAnimObjectGroupDataList;

	while(pstEntry)
	{
		if (pcsData == pstEntry->m_pcsData)
			return TRUE;

		pstEntry								= pstEntry->m_pstNext;
	}

	return FALSE;
}

BOOL AgcmObjectList::RemoveAnimObjectGroupData(AgcdObjectGroupData *pcsData)
{
	AgcdAnimObjectGroupDataList	*pstEntry		= m_pstAgcdAnimObjectGroupDataList;
	AgcdAnimObjectGroupDataList	*pstTemp;

	if(!pstEntry)
		return FALSE;

	if(pstEntry->m_pcsData == pcsData)
	{
		m_pstAgcdAnimObjectGroupDataList		= pstEntry->m_pstNext;
		free(pstEntry);
		return TRUE;
	}

	pstTemp										= pstEntry;
	pstEntry									= pstTemp->m_pstNext;

	while(pstEntry)
	{
		if(pstEntry->m_pcsData == pcsData)
		{
			pstTemp->m_pstNext					= pstEntry->m_pstNext; 
			free(pstEntry);
			return TRUE;
		}
		else 
		{
			pstTemp								= pstEntry;
			pstEntry							= pstTemp->m_pstNext;
		}
	}

	return FALSE;
}*/
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Object Group
BOOL AgcmObjectList::RemoveAllObjectGroup(AgcdObjectGroup *pstGroup)
{
	AgcdObjectGroupList	*pstList	= pstGroup->m_pstList;
    AgcdObjectGroupList	*pstTemp;

	// 리스트가 한개라도 있으면 돌아라!!!
    while(pstList)
    {
		// 다음 리스트를 temp에 저장
        pstTemp						= pstList->m_pstNext;
		// 현재 리스트를 날리고,
        m_csGroupPool.Free(pstList);
		// temp에 저장된 다음 리스트를 설정!
	    pstList						= pstTemp;
    }

	// 당현히 NULL이겠지만, 확인사살!
	pstGroup->m_pstList							= NULL;
	// 그룹의 갯수는 없당...
	pstGroup->m_lNum				= 0;

	return TRUE;
}

AgcdObjectGroupData *AgcmObjectList::AddObjectGroup(AgcdObjectGroup *pstGroup/*, AgcdObjectGroupData *pcsData*/)
{
	// Group-info 생성
	AgcdObjectGroupList	*pstList		= (AgcdObjectGroupList *) m_csGroupPool.Alloc();
	// 데이터 셋팅!
	memset(&pstList->m_csData, 0, sizeof(AgcdObjectGroupData));
	// 인데스 설정
	pstList->m_csData.m_lIndex			= pstGroup->m_lNum;
	// Next 설정
	pstList->m_pstNext					= pstGroup->m_pstList;
	// List 포인터 설정
	pstGroup->m_pstList					= pstList;	
	// 갯수 증가
	++pstGroup->m_lNum;
	
	return &pstList->m_csData;
}

/*BOOL AgcmObjectList::RemoveObjectGroup(AgcdObjectGroup *pstGroup, INT32 lIndex)
{
	AgcdObjectGroupList	*pstList		= pstGroup->m_pstList;	

	if(!pstList)
		return FALSE; // 지울께 없당.. ㅠ.ㅠ

	// 찾는 데이터가 현재 리스트의 데이터라면..
	if(pstList->m_csData.m_lIndex == lIndex)
	{
		// 현재 리스트를 다음 리스트로 넘기고...
		pstGroup->m_pstList				= pstList->m_pstNext;
		// 현재 리스트를 지워버린다!
		m_csGroupPool.Free(pstList);

		// 인덱스를 감소시킨다.
		pstList = pstGroup->m_pstList;
		while(pstList)
		{
			--pstList->m_csData.m_lIndex;

			pstList = pstList->m_pstNext;
		}

		--pstGroup->m_lNum;

		return TRUE;
	}

	// 임시 변수에 현재 리스트를 저장하고...
	AgcdObjectGroupList	*pstTemp		= pstList;
	// 현재 리스트는 다음 리스트로 넘긴다...
	pstList								= pstTemp->m_pstNext;

	// 현재 리스트가 있을때까지 돌아라!
	while(pstList)
	{
		// 찾는 데이터가 현재 리스트의 데이터라면..
		if(pstList->m_csData.m_lIndex == lIndex)
		{
			// 전 리스트의 다음 리스트를 새로 설정하고.
			pstTemp->m_pstNext			= pstList->m_pstNext; 
			// 현재 리스트를 날려BoA요~
			m_csGroupPool.Free(pstList);

			// 인덱스를 감소시킨다.
			pstList = pstTemp->m_pstNext;
			while(pstList)
			{
				--pstList->m_csData.m_lIndex;

				pstList = pstList->m_pstNext;
			}

			--pstGroup->m_lNum;

			return TRUE;
		}
		else
		{
			// 임시변수의 현재리스트를 저장하고...
			pstTemp						= pstList;
			// 현재리스트는 다음으로 넘긴다!
			pstList						= pstTemp->m_pstNext;
		}
	}

	return FALSE;
}*/

AgcdObjectGroupData *AgcmObjectList::GetObjectGroup(AgcdObjectGroup *pstGroup, INT32 lIndex)
{
	AgcdObjectGroupList	*pstList	= pstGroup->m_pstList;

	// 리스트가 있을 때까지 돌아BoA요~
	while(pstList)
	{
		// 찾는 인덱스가 리스트에 있당!
		if(pstList->m_csData.m_lIndex == lIndex)
			return &pstList->m_csData;

		// 현재 리스트를 다음으로 넘긴다!
		pstList						= pstList->m_pstNext;
	}

	return NULL;
}

/*
BOOL AgcmObjectList::CheckObjectGroup(AgcdObjectGroup *pstGroup, INT32 lIndex)
{
	AgcdObjectGroupList	*pstList	= pstGroup->m_pstList;

	// 리스트가 있을 때까지 돌아BoA요~
	while(pstList)
	{
		// 찾는 인덱스가 리스트에 있당!
		if(pstList->m_stData.m_lIndex == lIndex)
			return TRUE;

		// 현재 리스트를 다음으로 넘긴다!
		pstList								= pstList->m_pstNext;
	}

	return FALSE;
}*/
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ObjectTemplate Group
BOOL AgcmObjectList::RemoveAllObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup)
{
	AgcdObjectTemplateGroupList	*pstList	= pstGroup->m_pstList;
    AgcdObjectTemplateGroupList	*pstTemp;

	// 리스트가 한개라도 있으면 돌아라!!!
    while(pstList)
    {
		// 다음 리스트를 temp에 저장
        pstTemp								= pstList->m_pstNext;
		// 현재 리스트를 날리고,
        delete pstList;
		// temp에 저장된 다음 리스트를 설정!
	    pstList								= pstTemp;
    }

	// 당현히 NULL이겠지만, 확인사살!
	pstGroup->m_pstList						= NULL;
	// 그룹의 갯수는 없당...
	pstGroup->m_lNum						= 0;

	return TRUE;
}

AgcdObjectTemplateGroupData	*AgcmObjectList::AddObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup/*, AgcdObjectTemplateGroupData *pcsData*/)
{
	// Group-info 생성
	AgcdObjectTemplateGroupList	*pstList		= new AgcdObjectTemplateGroupList;
	// 데이터 셋팅!
/*	if (pcsData)
		memcpy(&pstList->m_csData, pcsData, sizeof(AgcdObjectTemplateGroupData));
	else*/
		memset(&pstList->m_csData, 0, sizeof(AgcdObjectTemplateGroupData));
	// 인데스 설정
	pstList->m_csData.m_lIndex			= pstGroup->m_lNum;
	// Next 설정
	pstList->m_pstNext					= pstGroup->m_pstList;
	// List 포인터 설정
	pstGroup->m_pstList					= pstList;	
	// 갯수 증가
	++pstGroup->m_lNum;
	
	return &pstList->m_csData;
}

BOOL AgcmObjectList::RemoveObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup, INT32 lIndex)
{
	if (!pstGroup->m_pstList)
		return FALSE; // 지울게 없당~

	AgcdObjectTemplateGroupList *pstTemp;
	AgcdObjectTemplateGroupList	*pstCurrent		= pstGroup->m_pstList;
	AgcdObjectTemplateGroupList *pstPrevious	= NULL;

	while (pstCurrent)
	{
		// 찾고자 하는 인덱스가 맞다면...
		if (pstCurrent->m_csData.m_lIndex == lIndex)
		{
			if (!pstPrevious)
			{ // 리스트의 처음이라면 Group의 head정보를 설정한다.
				pstGroup->m_pstList				= pstCurrent->m_pstNext;
			}
			else
			{ // 전 리스트의 다음 리스트를 현재 리스트의 다음 리스트로 설정한다.
				pstPrevious->m_pstNext			= pstCurrent->m_pstNext;

				
				{ // 인덱스를 다시 설정한다.
					pstTemp							= pstGroup->m_pstList;
					while (pstTemp)
					{
						--pstTemp->m_csData.m_lIndex;

						if (pstTemp == pstPrevious)
							break;

						pstTemp						= pstTemp->m_pstNext;
					}
				}
			}

			// 찾은 리스트를 삭제한다.
			delete pstCurrent;
			// 그룹이 가지고 있는 리스트의 갯수를 차감한다.
			--pstGroup->m_lNum;

			return TRUE;
		}

		// 전 리스트 정보를 저장하고...
		pstPrevious								= pstCurrent;
		// 현재 리스트를 다음 리스트로 넘긴다.
		pstCurrent								= pstPrevious->m_pstNext;
	}

	return FALSE;

/*	AgcdObjectTemplateGroupList	*pstList		= pstGroup->m_pstList;	

	if (!pstList)
		return FALSE; // 지울께 없당.. ㅠ.ㅠ

	// 찾는 데이터가 현재 리스트의 데이터라면..
	if (pstList->m_csData.m_lIndex == lIndex)
	{
		// 현재 리스트를 다음 리스트로 넘기고...
		pstGroup->m_pstList						= pstList->m_pstNext;
		// 현재 리스트를 지워버린다!
		free(pstList);

		// 인덱스를 감소시킨다.
		pstList = pstGroup->m_pstList;
		while (pstList)
		{
			--pstList->m_csData.m_lIndex;

			pstList = pstList->m_pstNext;
		}

		--pstGroup->m_lNum;

		return TRUE;
	}

	// 임시 변수에 현재 리스트를 저장하고...
	AgcdObjectTemplateGroupList	*pstTemp		= pstList;
	// 현재 리스트는 다음 리스트로 넘긴다...
	pstList										= pstTemp->m_pstNext;

	// 현재 리스트가 있을때까지 돌아라!
	while (pstList)
	{
		// 찾는 데이터가 현재 리스트의 데이터라면..
		if (pstList->m_csData.m_lIndex == lIndex)
		{
			// 전 리스트의 다음 리스트를 새로 설정하고.
			pstTemp->m_pstNext					= pstList->m_pstNext; 
			// 현재 리스트를 날려BoA요~
			free(pstList);

			// 인덱스를 감소시킨다.
			pstList = pstTemp->m_pstNext;
			while (pstList)
			{
				--pstList->m_csData.m_lIndex;

				pstList = pstList->m_pstNext;
			}

			--pstGroup->m_lNum;

			return TRUE;
		}
		else
		{
			// 임시변수의 현재리스트를 저장하고...
			pstTemp								= pstList;
			// 현재리스트는 다음으로 넘긴다!
			pstList								= pstTemp->m_pstNext;
		}
	}

	return FALSE;*/
}

AgcdObjectTemplateGroupData	*AgcmObjectList::GetObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup, INT32 lIndex)
{
	if( pstGroup->m_lNum <= lIndex )	return NULL;

	AgcdObjectTemplateGroupList	*pstList	= pstGroup->m_pstList;

	// 리스트가 있을 때까지 돌아BoA요~
	while(pstList)
	{
		// 찾는 인덱스가 리스트에 있당!
		if(pstList->m_csData.m_lIndex == lIndex)
			return &pstList->m_csData;

		// 현재 리스트를 다음으로 넘긴다!
		pstList								= pstList->m_pstNext;
	}

	return NULL;
}

// 주의!!! : pstDest->m_pstList와 pstDest->m_lNum을 초기화 시킨다.
/*BOOL AgcmObjectList::CopyObjectTemplateGroup(AgcdObjectTemplateGroup *pstDest, AgcdObjectTemplateGroup *pstSrc)
{
	pstDest->m_pstList						= NULL;
	pstDest->m_lNum							= 0;

//	AgcdObjectTemplateGroupList	*pstList	= pstSrc->m_pstList;

	// 리스트가 있을 때까지 돌아BoA요~
//	while(pstList)
	AgcdObjectTemplateGroupData *pcsData;
	for(INT32 lIndex = 0;; ++lIndex)
	{
		pcsData = GetObjectTemplateGroup(pstSrc, lIndex);
		if(!pcsData)
			break;

		// SRC 내용으로 DEST에 Add한당~
		if(!AddObjectTemplateGroup(pstDest, pcsData))
			return FALSE;

		// 현재 리스트를 다음으로 넘긴다!
//		pstList					= pstList->m_pstNext;
	}

	return TRUE;
}*/

/*
BOOL AgcmObjectList::CheckObjectTemplateGroup(AgcdObjectTemplateGroupInfo *pstList, INT32 lIndex)
{
	AgcdObjectTemplateGroupList	*pstList	= pstGroup->m_pstList;

	// 리스트가 있을 때까지 돌아BoA요~
	while(pstList)
	{
		// 찾는 인덱스가 리스트에 있당!
		if(pstList->m_stData.m_lIndex == lIndex)
			return TRUE;

		// 현재 리스트를 다음으로 넘긴다!
		pstList								= pstList->m_pstNext;
	}

	return FALSE;
}*/
///////////////////////////////////////////////////////////////////////////////

/******************************************************************************
******************************************************************************/