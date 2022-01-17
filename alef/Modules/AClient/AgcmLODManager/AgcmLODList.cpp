#include "AgcmLODList.h"

#include "ApMemoryTracker.h"

AgcmLODList::AgcmLODList()
{
}

AgcmLODList::~AgcmLODList()
{
}

BOOL AgcmLODList::RemoveAllLODData(AgcdLOD *pstLOD)
{
	AgcdLODList	*pstList	= pstLOD->m_pstList;
    AgcdLODList	*pstTemp;

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
	pstLOD->m_pstList						= NULL;
	// 그룹의 갯수는 없당...
	pstLOD->m_lNum							= 0;

	return TRUE;
}

AgcdLODData	*AgcmLODList::AddLODData(AgcdLOD *pstLOD, AgcdLODData *pcsData)
{
	// Group-info 생성
	AgcdLODList	*pstList		= new AgcdLODList;
	// 데이터 셋팅!
	if(pcsData)
		memcpy(&pstList->m_csData, pcsData, sizeof(AgcdLODData));
	else
		memset(&pstList->m_csData, 0, sizeof(AgcdLODData));
	// 인데스 설정
	pstList->m_csData.m_lIndex			= pstLOD->m_lNum;
	// Next 설정
	pstList->m_pstNext					= pstLOD->m_pstList;
	// List 포인터 설정
	pstLOD->m_pstList					= pstList;	
	// 갯수 증가
	++pstLOD->m_lNum;

	pstList->m_csData.m_pstAgcdLOD		= pstLOD;
	
	return &pstList->m_csData;
}

BOOL AgcmLODList::RemoveLODData(AgcdLOD *pstLOD, INT32 lIndex)
{
	AgcdLODList	*pstList		= pstLOD->m_pstList;	

	if(!pstList)
		return FALSE; // 지울께 없당.. ㅠ.ㅠ

	// 찾는 데이터가 현재 리스트의 데이터라면..
	if(pstList->m_csData.m_lIndex == lIndex)
	{
		// 현재 리스트를 다음 리스트로 넘기고...
		pstLOD->m_pstList		= pstList->m_pstNext;
		// 현재 리스트를 지워버린다!
		delete pstList;

		// 인덱스를 감소시킨다.
		pstList = pstLOD->m_pstList;
		while(pstList)
		{
			--pstList->m_csData.m_lIndex;

			pstList = pstList->m_pstNext;
		}

		--pstLOD->m_lNum;

		return TRUE;
	}

	// 임시 변수에 현재 리스트를 저장하고...
	AgcdLODList	*pstTemp		= pstList;
	// 현재 리스트는 다음 리스트로 넘긴다...
	pstList						= pstTemp->m_pstNext;

	// 현재 리스트가 있을때까지 돌아라!
	while(pstList)
	{
		// 찾는 데이터가 현재 리스트의 데이터라면..
		if(pstList->m_csData.m_lIndex == lIndex)
		{
			// 전 리스트의 다음 리스트를 새로 설정하고.
			pstTemp->m_pstNext	= pstList->m_pstNext; 
			// 현재 리스트를 날려BoA요~
			delete pstList;

			// 인덱스를 감소시킨다.
			pstList = pstTemp->m_pstNext;
			while(pstList)
			{
				--pstList->m_csData.m_lIndex;

				pstList = pstList->m_pstNext;
			}

			--pstLOD->m_lNum;

			return TRUE;
		}
		else
		{
			// 임시변수의 현재리스트를 저장하고...
			pstTemp				= pstList;
			// 현재리스트는 다음으로 넘긴다!
			pstList				= pstTemp->m_pstNext;
		}
	}

	return FALSE;
}

AgcdLODData	*AgcmLODList::GetLODData(AgcdLOD *pstLOD, INT32 lIndex)
{
	AgcdLODList	*pstList	= pstLOD->m_pstList;

	// 리스트가 있을 때까지 돌아BoA요~
	while(pstList)
	{
		// 찾는 인덱스가 리스트에 있당!
		if(pstList->m_csData.m_lIndex == lIndex)
			return &pstList->m_csData;

		// 현재 리스트를 다음으로 넘긴다!
		pstList				= pstList->m_pstNext;
	}

	return NULL;
}
