#include "AgcaAnimation.h"

#include "AgcmResourceLoader.h"

#include "ApMemoryTracker.h"

extern	AgcmResourceLoader*		g_pcsAgcmResourceLoader;

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcaAnimation2::AgcaAnimation2()
{
	m_ulNumFlags					= 0;
	m_ulNumAttachedData				= 0;
	m_pszPath						= NULL;
	m_pszExtension					= NULL;
	
	memset(m_apszKeyAttachedData, 0, sizeof (CHAR *) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_aulSizeAttachedData, 0, sizeof (UINT32) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_pafRemoveAttachedDataCB, 0, sizeof (AcCallbackData2) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_pavRemoveAttachedDataCBData, 0, sizeof (PVOID) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcaAnimation2::~AgcaAnimation2()
{
	if (m_pszPath)
		delete [] m_pszPath;
	if (m_pszExtension)
		delete [] m_pszExtension;

	for (UINT32 lIndex = 0; lIndex < m_ulNumAttachedData; ++lIndex)
	{
		if (m_apszKeyAttachedData[lIndex])
		{
			delete [] m_apszKeyAttachedData[lIndex];
			m_apszKeyAttachedData[lIndex]	= NULL;
		}
	}

	RemoveAllRtAnim();

	m_csAdminFlags.RemoveObjectAll();
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
VOID AgcaAnimation2::SetAnimationPathAndExtension(CHAR *pszPath, CHAR *pszExtension)
{
	if (m_pszPath)
		delete [] m_pszPath;
	if (m_pszExtension)
		delete [] m_pszExtension;

	m_pszPath		= new CHAR [strlen(pszPath) + 1];
	strcpy(m_pszPath, pszPath);

	if (pszExtension)
	{
		m_pszExtension	= new CHAR [strlen(pszExtension) + 1];
		strcpy(m_pszExtension, pszExtension);
	}
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::SetMaxRtAnim(UINT32 ulMaxAnimation)
{
	return m_csAdminRtAnim.InitializeObject(sizeof (AgcdRtAnim), ulMaxAnimation);
}

/******************************************************************************
* Purpose :
*
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::SetMaxFlags(UINT32 ulMaxFlags)
{
	return m_csAdminFlags.InitializeObject(sizeof (AgcdAnimationFlag), ulMaxFlags);
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::SetAttachedData(CHAR *pszKey, UINT32 ulDataSize, AcCallbackData2 pfRemoveCB, PVOID pvRemoveCBData)
{
	if (m_ulNumAttachedData >= D_AGCA_ANIMATION_MAX_ATTACHED_DATA)
		return FALSE;

	if (	(m_apszKeyAttachedData[m_ulNumAttachedData]) &&
			(m_aulSizeAttachedData[m_ulNumAttachedData]) &&
			(m_pafRemoveAttachedDataCB[m_ulNumAttachedData]) &&
			(m_pavRemoveAttachedDataCBData[m_ulNumAttachedData])	)
		return FALSE;

	m_apszKeyAttachedData[m_ulNumAttachedData]	= new CHAR [strlen(pszKey) + 1];
	strcpy(m_apszKeyAttachedData[m_ulNumAttachedData], pszKey);

	m_aulSizeAttachedData[m_ulNumAttachedData]			= ulDataSize;
	m_pafRemoveAttachedDataCB[m_ulNumAttachedData]		= pfRemoveCB;
	m_pavRemoveAttachedDataCBData[m_ulNumAttachedData]	= pvRemoveCBData;

	++m_ulNumAttachedData;

	return TRUE;
}

/******************************************************************************
* Purpose :
* Desc : 이녀석이 reference count를 감소시킨다.
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveRtAnim(AgcdAnimData2 *pcsAnim)
{
	if ((!pcsAnim) || (!pcsAnim->m_pcsRtAnim))
		return FALSE;

	AuAutoLock	lock(m_csCSection);
	if (!lock.Result()) return FALSE;
	//m_csCSection.Lock();

	--pcsAnim->m_pcsRtAnim->m_lReference;

	/*
	FILE	*fp = fopen("anim_log.txt", "at");
	fprintf(fp, "Remove Animation '%s' %x (RefCount = %d)\n", pcsAnim->m_pszRtAnimName, pcsAnim->m_pcsRtAnim, pcsAnim->m_pcsRtAnim->m_lReference);
	fclose(fp);
	*/

	if (pcsAnim->m_pcsRtAnim->m_lReference < 1)
	{
		if (pcsAnim->m_pcsRtAnim->m_pstAnimation)
		{
			RtAnimAnimationDestroy(pcsAnim->m_pcsRtAnim->m_pstAnimation);
			pcsAnim->m_pcsRtAnim->m_pstAnimation = NULL;
		}

		m_csAdminRtAnim.RemoveObject(pcsAnim->m_pszRtAnimName);
	}

	pcsAnim->m_pcsRtAnim	= NULL;

	//m_csCSection.Unlock();

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveRtAnim(AgcdAnimation2 *pcsAnimation2)
{
	if (!pcsAnimation2)
		return FALSE;

	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;
	while (pcsCurrent)
	{
		RemoveRtAnim(pcsCurrent);

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
* Desc : 이녀석이 reference count를 증가시킨다.
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::ReadRtAnim(AgcdAnimData2 *pcsAnim)
{
	if (!pcsAnim)
		return FALSE;

	AuAutoLock	lock(m_csCSection);
	if (!lock.Result()) return FALSE;
	//m_csCSection.Lock();

	/*
	FILE	*fp = fopen("anim_log.txt", "at");
	fprintf(fp, "Read Animation '%s' %x (RefCount = %d)\n", pcsAnim->m_pszRtAnimName, pcsAnim->m_pcsRtAnim, pcsAnim->m_pcsRtAnim ? pcsAnim->m_pcsRtAnim->m_lReference : 0);
	fclose(fp);
	*/

	if (!pcsAnim->m_pcsRtAnim)
	{
		if (!pcsAnim->m_pszRtAnimName)
		{
			ASSERT(!"AgcaAnimation2::ReadRtAnim() No AnimName!!!");
			//m_csCSection.Unlock();

			return FALSE;
		}

		pcsAnim->m_pcsRtAnim	= GetRtAnim(pcsAnim->m_pszRtAnimName);
		if (!pcsAnim->m_pcsRtAnim)
		{
			ASSERT(!"AgcaAnimation2::ReadRtAnim() No Anim!!!");
			//m_csCSection.Unlock();

			return FALSE;
		}
	}
	else if (!pcsAnim->m_pszRtAnimName)
	{
		ASSERT(!"AgcaAnimation2::ReadRtAnim() No AnimName!!!");
		//m_csCSection.Unlock();

		return FALSE;
	}

	++pcsAnim->m_pcsRtAnim->m_lReference;

	if (pcsAnim->m_pcsRtAnim->m_pstAnimation)
	{
		//m_csCSection.Unlock();
		return TRUE;
	}

	//m_csCSection.Unlock();

	CHAR		szFullPath[256];
	memset(szFullPath, 0, sizeof (CHAR) * 256);
	
	if (m_pszExtension)
		sprintf(szFullPath, "%s%s.%s", m_pszPath, pcsAnim->m_pszRtAnimName, m_pszExtension);
	else
		sprintf(szFullPath, "%s%s", m_pszPath, pcsAnim->m_pszRtAnimName);

	//pcsAnim->m_pcsRtAnim->m_pstAnimation	= RtAnimAnimationRead(szFullPath);
	pcsAnim->m_pcsRtAnim->m_pstAnimation	= g_pcsAgcmResourceLoader->LoadRtAnim(szFullPath);
	if (!pcsAnim->m_pcsRtAnim->m_pstAnimation)
		return FALSE;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 071204. BOB
******************************************************************************/
BOOL AgcaAnimation2::ReadRtAnim(AgcdAnimation2 *pcsAnimation2)
{
	if (!pcsAnimation2)
		return FALSE;

	if (!m_pszPath)
	{
		ASSERT(!"Path를 먼저 설정해야됨!!!");
		return FALSE;
	}

//	CHAR		szFullPath[256];
//	memset(szFullPath, 0, sizeof (CHAR) * 256);

	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;
	while (pcsCurrent)
	{
		if (!ReadRtAnim(pcsCurrent))
			return FALSE;

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 091204. BOB
******************************************************************************/
AgcdRtAnim *AgcaAnimation2::AddRtAnim(CHAR *pszName)
{
	AgcdRtAnim	csRtAnim;
	AgcdRtAnim	*pcsRtAnim;

	pcsRtAnim	= (AgcdRtAnim *)(m_csAdminRtAnim.AddObject((PVOID)(&csRtAnim), pszName));

	return pcsRtAnim;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcdRtAnim *AgcaAnimation2::GetRtAnim(CHAR *pszName, BOOL bAdd)
{
	AgcdRtAnim	*pcsRtAnim	= (AgcdRtAnim *)(m_csAdminRtAnim.GetObject(pszName));

	if ((!pcsRtAnim) && (bAdd))
	{
		pcsRtAnim	= AddRtAnim(pszName);
	}

	return pcsRtAnim;
}

/******************************************************************************
* Purpose :
*
* 071204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::CreateAnimData(AgcdRtAnim *pcsRtAnim, BOOL bAttachData)
{
	AgcdAnimData2	*pcsAnimData2	= new AgcdAnimData2();
	pcsAnimData2->m_pcsRtAnim		= pcsRtAnim;

	if ((bAttachData) && (m_ulNumAttachedData > 0))
	{
		pcsAnimData2->m_pavAttachedData	= new PVOID [m_ulNumAttachedData];

		for (UINT32 lIndex = 0; lIndex < m_ulNumAttachedData; ++lIndex)
		{
			if (m_aulSizeAttachedData[lIndex])
			{
				pcsAnimData2->m_pavAttachedData[lIndex]	= new BYTE [m_aulSizeAttachedData[lIndex]];
				memset(pcsAnimData2->m_pavAttachedData[lIndex], 0, m_aulSizeAttachedData[lIndex]);
			}
		}
	}

	return pcsAnimData2;
}

/******************************************************************************
* Purpose :
*
* 071204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::AddAnimData(CHAR *pszName, BOOL bAttachData)
{
	/*AgcdRtAnim		*pcsRtAnim		= GetRtAnim(pszName);
	if (!pcsRtAnim)
		return NULL;

	AgcdAnimData2	*pcsAnimData	= CreateAnimData(pcsRtAnim, bAttachData);*/
	AgcdAnimData2	*pcsAnimData	= CreateAnimData(NULL, bAttachData);
	if (!pcsAnimData)
		return NULL;

	if (pcsAnimData->m_pszRtAnimName)
		return NULL;

	pcsAnimData->m_pszRtAnimName	= new CHAR [strlen(pszName) + 1];
	strcpy(pcsAnimData->m_pszRtAnimName, pszName);

	return pcsAnimData;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::AddAnimation(AgcdAnimation2 *pcsAnimation2, AgcdRtAnim *pcsRtAnim, CHAR *pszName)
{
	AgcdAnimData2	*pcsAnimData2	= CreateAnimData(pcsRtAnim, TRUE);
	if (!pcsAnimData2)
		return NULL;

	if (pcsAnimData2->m_pszRtAnimName)
		return NULL;
	
	pcsAnimData2->m_pszRtAnimName	= new CHAR [strlen(pszName) + 1];
	strcpy(pcsAnimData2->m_pszRtAnimName, pszName);

	if (!pcsAnimation2->m_pcsTail)
	{
		pcsAnimation2->m_pcsHead	= pcsAnimation2->m_pcsTail	= pcsAnimData2;
	}
	else
	{
		pcsAnimation2->m_pcsTail->m_pcsNext	= pcsAnimData2;
		pcsAnimation2->m_pcsTail	= pcsAnimData2;
	}

	return pcsAnimData2;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::AddAnimation(AgcdAnimation2 **ppcsAnimation2, CHAR *pszName)
{
	if (!(*ppcsAnimation2))
		(*ppcsAnimation2)	= new AgcdAnimation2();

	/*AgcdRtAnim	*pcsRtAnim	= GetRtAnim(pszName);
	if (!pcsRtAnim)
		return NULL;

	return AddAnimation(*ppcsAnimation2, pcsRtAnim, pszName);*/
	return AddAnimation(*ppcsAnimation2, NULL, pszName);
}

/******************************************************************************
* Purpose :
*
* 240205. BOB
******************************************************************************/
BOOL AgcaAnimation2::CopyAnimation(AgcdAnimation2 *pcsSrc, AgcdAnimation2 **ppcsDest)
{
	if (!pcsSrc)
		return TRUE; // skip

	if (!(*ppcsDest))
		(*ppcsDest)	= new AgcdAnimation2();

	AgcdAnimData2	*pcsCurrentSrcAnimData	= pcsSrc->m_pcsHead;
	while (pcsCurrentSrcAnimData)
	{
		if (pcsCurrentSrcAnimData->m_pszRtAnimName)
		{
			if (!AddAnimation(*(ppcsDest), NULL, pcsCurrentSrcAnimData->m_pszRtAnimName))
				return FALSE;
		}

		pcsCurrentSrcAnimData	= pcsCurrentSrcAnimData->m_pcsNext;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::DestroyAnimData(AgcdAnimData2 *pcsAnimData)
{
	if (!pcsAnimData)
		return FALSE;

	{ // AgcdRtAnim 관련.
		RemoveRtAnim(pcsAnimData);

		if (pcsAnimData->m_pszRtAnimName)
		{
			delete [] pcsAnimData->m_pszRtAnimName;
			pcsAnimData->m_pszRtAnimName	= NULL;
		}
	}

	if (pcsAnimData->m_pavAttachedData)
	{ // AttachedData 관련.

		for (UINT32 lIndex = 0; lIndex < m_ulNumAttachedData; ++lIndex)
		{
			if (pcsAnimData->m_pavAttachedData[lIndex])
			{
				if (m_pafRemoveAttachedDataCB[lIndex])
				{
					m_pafRemoveAttachedDataCB[lIndex](
						pcsAnimData->m_pavAttachedData[lIndex],
						m_pavRemoveAttachedDataCBData[lIndex]		);
				}

				delete [] pcsAnimData->m_pavAttachedData[lIndex];
				pcsAnimData->m_pavAttachedData[lIndex]	= NULL;
			}
		}

		delete [] pcsAnimData->m_pavAttachedData;		
		pcsAnimData->m_pavAttachedData	= NULL;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 221204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveAnimData(AgcdAnimData2 **ppcsAnimData)
{
	if (!ppcsAnimData)
		return FALSE;

	if (!(*ppcsAnimData))
		return TRUE; // skip

	if (!DestroyAnimData(*ppcsAnimData))
		return FALSE;

	delete (*ppcsAnimData);
	(*ppcsAnimData)	= NULL;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 301204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveAnimation(AgcdAnimation2 *pcsAnimation2, CHAR *pszName)
{
	if (!pcsAnimation2)
		return FALSE;

	AgcdAnimData2	*pcsPrev	= NULL;
	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;
	while (pcsCurrent)
	{
		if (!strcmp(pcsCurrent->m_pszRtAnimName, pszName))
		{
			DestroyAnimData(pcsCurrent);

			if (pcsPrev)
			{
				pcsPrev->m_pcsNext			= pcsCurrent->m_pcsNext;
			}
			else
			{
				pcsAnimation2->m_pcsHead	= pcsCurrent->m_pcsNext;
				if( !pcsAnimation2->m_pcsHead )
					pcsAnimation2->m_pcsTail = NULL;
			}

			delete pcsCurrent;

			return TRUE;
		}

		pcsPrev		= pcsCurrent;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return FALSE;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveAllAnimation(AgcdAnimation2 **ppcsAnimation2)
{
	if (	(!ppcsAnimation2) ||
			(!(*ppcsAnimation2))	)
		return TRUE; // skip.

	AgcdAnimData2	*pcsNext	= NULL;
	AgcdAnimData2	*pcsCurrent	= (*ppcsAnimation2)->m_pcsHead;
	while (pcsCurrent)
	{
		pcsNext		= pcsCurrent->m_pcsNext;

		DestroyAnimData(pcsCurrent);

		delete pcsCurrent;

		pcsCurrent	= pcsNext;
	}

	delete (*ppcsAnimation2);

	(*ppcsAnimation2)	= NULL;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveAllRtAnim()
{
	INT32	lIndex = 0;

	for (	AgcdRtAnim *pcsRtAnim = (AgcdRtAnim *)(m_csAdminRtAnim.GetObjectSequence(&lIndex));
			pcsRtAnim;
			pcsRtAnim = (AgcdRtAnim *)(m_csAdminRtAnim.GetObjectSequence(&lIndex))				)
	{
		if (pcsRtAnim->m_pstAnimation)
		{
			RtAnimAnimationDestroy(pcsRtAnim->m_pstAnimation);
			pcsRtAnim->m_pstAnimation	= NULL;
		}
	}

	m_csAdminRtAnim.RemoveObjectAll();

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 071204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::GetAnimData(AgcdAnimation2 *pcsAnimation2, INT32 lIndex)
{
	if (!pcsAnimation2)
		return NULL;

	INT32			lCount		= 0;
	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;

	while (pcsCurrent)
	{
		if (lCount == lIndex)
			return pcsCurrent;

		++lCount;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return NULL;
}

/******************************************************************************
* Purpose :
* Desc : 실패시 -1을 리턴한다.
* 071204. BOB
******************************************************************************/
INT32 AgcaAnimation2::GetAnimDataIndex(AgcdAnimation2 *pcsAnimation2, AgcdAnimData2 *pcsAnimData2)
{
	if (!pcsAnimation2)
		return -1;

	INT32			lCount		= 0;
	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;

	while (pcsCurrent)
	{
		if (pcsCurrent == pcsAnimData2)
			return lCount;

		++lCount;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return -1;
}

/******************************************************************************
* Purpose :
*
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::AddFlags(AgcdAnimationFlag **ppFlags)
{
	++m_ulNumFlags;

	AgcdAnimationFlag	csFlags;
	*(ppFlags)	= (AgcdAnimationFlag *)(m_csAdminFlags.AddObject((PVOID)(&csFlags), m_ulNumFlags));

	return (*ppFlags) ? (TRUE) : (FALSE);
}

/******************************************************************************
* Purpose :
*
* 171204. BOB
******************************************************************************/
PVOID AgcaAnimation2::GetAttachedData(CHAR *pszKey, AgcdAnimData2 *pcsData)
{
	//@{ kday 20050403
	//check validation
	//if (!pcsData->m_pavAttachedData)
	if (!pcsData || !pcsData->m_pavAttachedData)
	//@} kday
		return NULL;

	UINT32 lIndex;
	for (lIndex = 0; lIndex < m_ulNumAttachedData; ++lIndex)
	{
		if (!m_apszKeyAttachedData[lIndex])
			return NULL;

		if (!strcmp(m_apszKeyAttachedData[lIndex], pszKey))
			break;
	}

	return pcsData->m_pavAttachedData[lIndex];
}

/******************************************************************************
******************************************************************************/



AgcdAnimationTime::AgcdAnimationTime()
{
	m_pcsList					= NULL;
	m_lNumCurCB					= 0;
	m_lNumMaxCB					= 0;
	m_fCurrentTime				= 0;
	m_fDuration					= 0;
}

AgcdAnimationTime::~AgcdAnimationTime()
{
	ResetAnimCB(FALSE);
}

VOID AgcdAnimationTime::SetMaxCallback(INT32 lMax)
{
	m_lNumMaxCB	= lMax;
}

BOOL AgcdAnimationTime::AddAnimList(AgcdAnimationCallbackList *pstPreList, AgcdAnimationCallbackList **ppstCurList, AgcdAnimationCallbackList *pstNextList, AgcdAnimationCallbackData *pcsData)
{
	if (ppstCurList)
	{
		// 리스트를 할당한다.
		*(ppstCurList)	= new AgcdAnimationCallbackList();

		if (!(*ppstCurList))
			return FALSE;

		// 할당된 리스트에 데이터를 copy한다...
		memcpy(&((*(ppstCurList))->m_csData), pcsData, sizeof(AgcdAnimationCallbackData));

		// 다음 리스트를 지정한다.
		(*(ppstCurList))->m_pcsNext		= pstNextList;

		// 모듈을 지정한다.
		(*(ppstCurList))->m_pvClass		= this;

		// 필요한 callback을 지정한다.
		(*(ppstCurList))->m_pfLoopFunc	= NULL;
		(*(ppstCurList))->m_pfEndFunc	= NULL;
		(*(ppstCurList))->m_pfCustFunc	= NULL;

		// 전 리스트의 다음을 지정한다.
		if (pstPreList)
		{
			pstPreList->m_pcsNext = *(ppstCurList);
		}

		++m_lNumCurCB;
	}

	return TRUE;
}

BOOL AgcdAnimationTime::AddAnimCB(AgcdAnimationCallbackData *pcsData)
{
	// 전체 duration보다 큰지 검사.
	if (m_fDuration < pcsData->m_fTime)
		return FALSE;

	// 꽉 찼냐?
	if ((m_lNumMaxCB) && (m_lNumMaxCB <= m_lNumCurCB))
		return FALSE;

	AgcdAnimationCallbackList	*pcsCurList, *pcsPreList;

	// 현재 애니매이션의 진행시간보다 크거나 같으면...
	if (m_fCurrentTime <= pcsData->m_fTime)
	{
		// 리스트가 있으면...
		if (m_pcsList)
		{
			// 최초 리스트에 들어있는 시간보다 작다면...
			if (m_pcsList->m_csData.m_fTime >= pcsData->m_fTime)
			{
				// 잠시 pcsCurList에 저장한다.
				pcsCurList = m_pcsList;

				// 최초 리스트에 현재 녀석을 추가한다.
				if (!AddAnimList(NULL, &m_pcsList, pcsCurList, pcsData))
					return FALSE;

				return TRUE;
			}
/*			else if (m_pcsList->m_csData.m_fTime == pcsData->m_fTime) // 최초 리스트에 들어있는 시간과 같다면...
			{
				// replace!!! -_-/
				memcpy(&m_pcsList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

				return TRUE;
			}*/

			pcsPreList	= m_pcsList;
			pcsCurList	= m_pcsList->m_pcsNext;
			// 리스트를 돈다...
			while (pcsCurList)
			{
				// 리스트의 시간보다 작다면...
				if (pcsCurList->m_csData.m_fTime >= pcsData->m_fTime)
				{
					// 전 리스트와 현재 리스트 사이에 추가한다...
					return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, pcsCurList, pcsData);
				}
/*				else if (pcsCurList->m_csData.m_fTime == pcsData->m_fTime) // 리스트의 시간과 같다면...
				{
					// replace!!! -_-/
					memcpy(&pcsCurList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

					return TRUE;
				}*/

				// 다음 리스트를 설정한다...
				pcsPreList	= pcsCurList;
				pcsCurList	= pcsCurList->m_pcsNext;
			}

			return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, NULL, pcsData);
		}
		else // 리스트가 없으면...
		{
			if (!AddAnimList(NULL, &m_pcsList, NULL, pcsData))
				return FALSE;

			return TRUE;
		}
	}
	else // 현재 애니매이션의 진행시간보다 작으면...
	{
		// 리스트가 없으면...
		if (!m_pcsList)
		{
			if(!AddAnimList(NULL, &m_pcsList, NULL, pcsData))
				return FALSE;

			return TRUE;
		}

		pcsPreList	= m_pcsList;
		pcsCurList	= m_pcsList->m_pcsNext;
		// 리스트를 돈다...
		while (pcsCurList)
		{
			// 리스트의 시간보다 크다면...
			if (pcsCurList->m_csData.m_fTime <= pcsData->m_fTime)
			{
				// 현재 리스트와 다음 리스트 사이에 추가한다...
				return AddAnimList(pcsCurList, &pcsCurList->m_pcsNext, pcsCurList->m_pcsNext, pcsData);
			}
/*			else if (pcsCurList->m_csData.m_fTime == pcsData->m_fTime) // 리스트의 시간과 같다면...
			{
				// replace!!! -_-/
				memcpy(&pcsCurList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

				return TRUE;
			}*/

			// 다음 리스트를 설정한다...
			pcsPreList	= pcsCurList;
			pcsCurList	= pcsCurList->m_pcsNext;
		}

		// 설정하려는 시간보다 리스트에 있는 시간들이 모두 작지 않으면, 맨 마지막에 넣는다.
		return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, NULL, pcsData);
	}

	return FALSE;
}

VOID AgcdAnimationTime::ResetAnimCB(BOOL InfluenceNextAnimation)
{
	INT32						lNum = 0;
	AgcdAnimationCallbackList	*pcsCurrent = m_pcsList;
	AgcdAnimationCallbackList	*pcsPre = NULL, *pcsNext = NULL;
	while (pcsCurrent)
	{
		pcsNext = pcsCurrent->m_pcsNext;

		if (InfluenceNextAnimation)
		{
			if (!pcsCurrent->m_csData.m_bInfluenceNextAnimation)
			{
				if (pcsPre)
					pcsPre->m_pcsNext	= pcsCurrent->m_pcsNext;
				else
					m_pcsList			= pcsNext;

				delete pcsCurrent;
			}
			else
			{
				++lNum;

				pcsCurrent->m_csData.m_fTime					-= m_fCurrentTime;
				if (pcsCurrent->m_csData.m_fTime < 0.0f)
					pcsCurrent->m_csData.m_fTime				= 0.0f;
				// 무한 반복 방지~ -_-;;
				pcsCurrent->m_csData.m_bInfluenceNextAnimation	= FALSE;

				pcsPre											= pcsCurrent;
			}
		}
		else
		{
			delete pcsCurrent;
		}

		pcsCurrent	= pcsNext;
	}

//	m_pcsList		= NULL;
//	m_lNumCurCB		= 0;

	if (!InfluenceNextAnimation)
		m_pcsList	= NULL;

	m_lNumCurCB		= lNum;
}

AgcdAnimationCallbackList *AgcdAnimationTime::HeadMoveToTail(AgcdAnimationCallbackList *pstHead)
{
	// 헤드의 다음 리스트가 있으면...(없으면 리스트가 1개이므로, 움직일 필요없다.)
	if (pstHead->m_pcsNext)
	{
		// 다음 리스트를 지정한다.
		m_pcsList								= pstHead->m_pcsNext;

		AgcdAnimationCallbackList	*pstTemp	= m_pcsList;

		// 마지막 리스트까지 돈다.
		while (pstTemp->m_pcsNext)
		{
			pstTemp								= pstTemp->m_pcsNext;
		}

		// 헤드를 마지막 리스트로 설정하고.
		pstTemp->m_pcsNext						= pstHead;
		// 헤드의 다음 리스트를 NULL로 초기화한다.
		pstHead->m_pcsNext						= NULL;

		// 다음 리스트를 리턴한다!
		return m_pcsList;
	}

	// 헤드가 곧 다음 리스트이다! (리스트가 1개)
	return pstHead;
}

AgcdAnimationCallbackList *AgcdAnimationTime::RemoveAnimList(AgcdAnimationCallbackList *pstCurList)
{
	if (!pstCurList)
		return NULL;
	// 다음을 지정하고...
	m_pcsList = pstCurList->m_pcsNext;
	// 날려버린다~ 햏햏~ -_-;;
	delete pstCurList;

	--m_lNumCurCB;

	return m_pcsList;
}

BOOL AgcdAnimationTime::AddTime(FLOAT fTime)
{
	m_fCurrentTime	+= fTime;
	if (m_fCurrentTime > m_fDuration)
		m_fCurrentTime = m_fDuration;

/*	if (m_pcsList)
	{
		if (m_fCurrentTime >= m_pcsList->m_csData.m_fTime)
		{
			BOOL bInit = (BOOL)(m_pcsList->m_csData.m_pfCallback((PVOID)(m_pcsList->m_csData.m_pavData)));

			if (m_pcsList->m_csData.m_bLoop)
				HeadMoveToTail(m_pcsList);
			else if (!bInit)
				RemoveAnimList(m_pcsList);
		}
	}*/

	AgcdAnimationCallbackList	*pcsCur		= m_pcsList;
	AgcdAnimationCallbackList	*pcsNext;
	while (pcsCur)
	{
		pcsNext	= pcsCur->m_pcsNext;

		if (m_fCurrentTime >= pcsCur->m_csData.m_fTime)
		{
			BOOL bInit = (BOOL)(pcsCur->m_csData.m_pfCallback((PVOID)(pcsCur->m_csData.m_pavData)));
			if (bInit)
				break;

			if (pcsCur->m_csData.m_bLoop)
				HeadMoveToTail(pcsCur);
			else
				RemoveAnimList(pcsCur);
		}
		else
		{
			break;
		}

		pcsCur = pcsNext;
	}

	return TRUE;
}

VOID AgcdAnimationTime::InitializeAnimation(FLOAT fDuration)
{
	ResetAnimCB();

	m_fCurrentTime	= 0.0f;
	m_fDuration		= fDuration;
}

INT32 AgcdAnimationTime::ParseAnimPoint(CHAR *szAnimPoint, INT32 *plDest)
{
	CHAR	szTemp[256];
	INT32	lProgress, lIndex, lLen, lNumField;

	lLen	= strlen(szAnimPoint);

	for (lProgress = 0, lIndex = 0, lNumField = 0; ; ++lProgress)
	{
		if ((szAnimPoint[lProgress] == ':') || ((lProgress != 0) && (lProgress == lLen)))
		{
			szTemp[lIndex]		= '\0';
			plDest[lNumField]	= atoi(szTemp);

			lIndex = 0;
			++lNumField;

			if (lProgress == lLen)
				break;
			else
				continue;
		}

		szTemp[lIndex] = szAnimPoint[lProgress];
		++lIndex;
	}

	return lNumField;
}

/******************************************************************************
******************************************************************************/










/******************************************************************************
* Purpose :
*
* 041503. Bob Jung
*******************************************************************************
AgcaAnimation::AgcaAnimation()
{
	m_lNumAttachedData			= 0;
	m_lAllocAnimData			= 0;

	memset(m_pavRemoveAnimationCBClass, 0, sizeof(PVOID) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_pafRemoveAnimationCB, 0, sizeof(ApModuleDefaultCallBack) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_aulAttachedDataSize, 0, sizeof(INT32) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_apszAttachedDataKey, 0, sizeof(CHAR *) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
}

/******************************************************************************
* Purpose :
*
* 041503. Bob Jung
*******************************************************************************
AgcaAnimation::~AgcaAnimation()
{
	for (INT32 lIndex = 0; lIndex < D_AGCA_ANIMATION_MAX_ATTACHED_DATA; ++lIndex)
	{
		if (m_apszAttachedDataKey[lIndex])
		{
			delete [] m_apszAttachedDataKey[lIndex];
			m_apszAttachedDataKey[lIndex] = NULL;
		}
	}
}

/******************************************************************************
* Purpose :
*
* 041603. Bob Jung
*******************************************************************************
BOOL AgcaAnimation::Initialize(INT32 lMaxAdminAnimation, INT32 lAllocAnimData)
{
	if (!InitializeObject(sizeof(AgcdAnimation), lMaxAdminAnimation))
		return FALSE;

	m_lAllocAnimData	= lAllocAnimData;

	return m_csAnimDataAdmin.InitializeObject(sizeof(AgcdAnimData), lMaxAdminAnimation * lAllocAnimData);
}

/******************************************************************************
* Purpose :
*
* 041503. Bob Jung
*******************************************************************************
/*AgcdAnimation *AgcaAnimation::AddAnimation(AgcdAnimation *pstAnim, CHAR *szFile, CHAR *szPath)
{
	CHAR			szFullPath[256];	
	strncpy(stAnim.m_szANMName, szFile, AGCD_ANIMATION_NAME_LENGTH);
	sprintf(szFullPath, "%s%s", szPath, szFile);
	stAnim.m_pAnim = RtAnimAnimationRead(szFullPath);

	if(!stAnim.m_pAnim)
		return NULL;

	pstAnim = (AgcdAnimation *) AddObject((PVOID) &stAnim, szFile);
	if(!pstAnim)
		return NULL;

//	pstAnim->m_lCustData					= -1;
	pstAnim->m_stAnimFlag.m_unAnimFlag		= AGCD_ANIMATION_FLAG_EMPTY;
	pstAnim->m_stAnimFlag.m_unPreference	= 0;

	return pstAnim;
}*/

/******************************************************************************
* Purpose :
*
* 041503. Bob Jung
*******************************************************************************
/*AgcdAnimation *AgcaAnimation::GetAnimation(CHAR *szFile, CHAR *szPath)
{
	AgcdAnimation *pstAnim = NULL;

	pstAnim = (AgcdAnimation *) GetObject(szFile);
	if((!pstAnim) && (szPath[0]))
	{
		pstAnim = AddAnimation(szFile, szPath);
	}

	return pstAnim;
}*/

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
AgcdAnimation *AgcaAnimation::AddAnimation(CHAR *szName)
{
	AgcdAnimation	*pstAnimation;
	AgcdAnimation	stAnimation;

	// 초기화...
	memset(&stAnimation, 0, sizeof(AgcdAnimation));

	// Attached한 custom data를 할당한다.
	if (!AllocateAttachedData(&stAnimation))
		return NULL;

	// AgcdAnimData의 포인터 배열을 할당한다.
	stAnimation.m_ppastAnimData			= (AgcdAnimData **) new PVOID[m_lAllocAnimData];
	memset(stAnimation.m_ppastAnimData, 0, sizeof(AgcdAnimData *) * m_lAllocAnimData);

//	stAnimation.m_ppastBlendingAnimData	= (AgcdAnimData **)(malloc(sizeof(AgcdAnimData *) * m_lAllocAnimData));
//	memset(stAnimation.m_ppastBlendingAnimData, 0, sizeof(AgcdAnimData *) * m_lAllocAnimData);

	// Admin에 추가한다.
	pstAnimation = (AgcdAnimation *)(AddObject((PVOID)(&stAnimation), szName));

	return pstAnimation;
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
AgcdAnimation *AgcaAnimation::GetAnimation(CHAR *szName, BOOL bAdd)
{
	AgcdAnimation *pstAnimation = NULL;
	pstAnimation = (AgcdAnimation *)(GetObject(szName));

	if ((!pstAnimation) && (bAdd))
	{
		pstAnimation = AddAnimation(szName);
	}

	return pstAnimation;
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
AgcdAnimData *AgcaAnimation::AddAnimData(CHAR *szName, CHAR *szPath)
{
	AgcdAnimData	*pstAnimData;
	AgcdAnimData	stAnimData;
	CHAR			szFullPath[256];

//	strncpy(stAnimData.m_szANMName, szName, AGCD_ANIMATION_NAME_LENGTH);

	// 애니매이션 데이터를 읽는다.
	sprintf(szFullPath, "%s%s", szPath, szName);
	stAnimData.m_pstAnim	= RtAnimAnimationRead(szFullPath);	
	if (!stAnimData.m_pstAnim)
		return NULL;

	// 레퍼런스 카운트를 초기화한다.
	stAnimData.m_ulRefCount	= 1;

	// Admin에 추가한다.
	pstAnimData = (AgcdAnimData *)(m_csAnimDataAdmin.AddObject((PVOID)(&stAnimData), szName));

	return pstAnimData;
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
AgcdAnimData *AgcaAnimation::GetAnimData(CHAR *szName, CHAR *szPath)
{
	AgcdAnimData *pstAnimData = NULL;

	// 이미 있는지 찾는다.
	pstAnimData = (AgcdAnimData *)(m_csAnimDataAdmin.GetObject(szName));

	// 찾는 데이터가 없고...
	if (!pstAnimData)
	{
		// 경로로 주어지면 새로 추가한다.
		if(szPath)
			pstAnimData = AddAnimData(szName, szPath);
		else // 찾는 데이터도 없고, 경로도 없으면 데이터를 가져올 수 없다.
			return NULL;
	}
	else // 찾는 데이터가 있으면 해당 데이터의 레퍼런스 카운트를 증가한다.
	{
		++pstAnimData->m_ulRefCount;
	}

	return pstAnimData;
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
VOID AgcaAnimation::RemoveAnimData(AgcdAnimData *pstAnimData, CHAR *szKey)
{
	// 애니매이션 데이터가 설정되어 있으면 해제한다!
	if (pstAnimData->m_pstAnim)
	{
		RtAnimAnimationDestroy(pstAnimData->m_pstAnim);
	}

	// Admin에서도 삭제한다.
	if (szKey)
		m_csAnimDataAdmin.RemoveObject(szKey);
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
BOOL AgcaAnimation::RemoveAnimData(CHAR *szKey)
{
	// 데이터를 찾는다.
	AgcdAnimData *pstData;
	pstData = (AgcdAnimData *)(m_csAnimDataAdmin.GetObject(szKey));
	if (!pstData)
		return FALSE;

	// 래퍼런스 카운트를 차감한다.
	--pstData->m_ulRefCount;

	// 더이상 사용하는 녀석이 없다면?
	if (pstData->m_ulRefCount < 1)
		RemoveAnimData(pstData, szKey);

	return TRUE;

//	return RemoveAnimData(pstData);
}

/******************************************************************************
* Purpose :
*
* 090804. BOB
*******************************************************************************
VOID AgcaAnimation::RemoveAnimation(AgcdAnimation *pstAnim, CHAR *szKey)
{
	if (pstAnim->m_ppastAnimData)
	{
		delete [] pstAnim->m_ppastAnimData;
		pstAnim->m_ppastAnimData = NULL;
	}

//	if (pstAnim->m_ppastBlendingAnimData)
//	{
//		free(pstAnim->m_ppastBlendingAnimData);
//		pstAnim->m_ppastBlendingAnimData = NULL;
//	}

	if (pstAnim->m_pavAttachedData)
	{
		// 붙힌 갯수만큼 날린다~
		for (INT32 lIndex = 0; lIndex < m_lNumAttachedData; ++lIndex)
		{
			if (m_pafRemoveAnimationCB[lIndex])
				m_pafRemoveAnimationCB[lIndex](pstAnim->m_pavAttachedData[lIndex], m_pavRemoveAnimationCBClass[lIndex], NULL);

			delete [] pstAnim->m_pavAttachedData[lIndex];
		}

		// 포인터배열도 날린다~
		delete [] pstAnim->m_pavAttachedData;

		pstAnim->m_pavAttachedData	= NULL;
	}

	// Key가 있으면 admin에서도 날린다~
	if (szKey);
		RemoveObject(szKey);
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
BOOL AgcaAnimation::RemoveAnimation(CHAR *szKey)
{
	AgcdAnimation	*pstAnim	= (AgcdAnimation *)(GetObject(szKey));
	if (!pstAnim)
		return FALSE;

	RemoveAnimation(pstAnim, szKey);

	return TRUE;

//	return RemoveObject(szKey);
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
BOOL AgcaAnimation::RemoveAll()
{
	INT32 lIndex = 0;
	for (	AgcdAnimData *pstData = (AgcdAnimData *)(m_csAnimDataAdmin.GetObjectSequence(&lIndex));
			pstData;
			pstData = (AgcdAnimData *)(m_csAnimDataAdmin.GetObjectSequence(&lIndex))				)
	{
		RemoveAnimData(pstData);
	}

	if (!m_csAnimDataAdmin.RemoveObjectAll())
		return FALSE;

	lIndex	= 0;
	for (	AgcdAnimation *pstAnim	= (AgcdAnimation *)(GetObjectSequence(&lIndex));
			pstAnim;
			pstAnim					= (AgcdAnimation *)(GetObjectSequence(&lIndex))		)
	{
		RemoveAnimation(pstAnim);
	}

	return RemoveObjectAll();
}

/******************************************************************************
* Purpose :
*
* 090804. BOB
*******************************************************************************
BOOL AgcaAnimation::AttachData(CHAR *szKey, UINT32 ulDataSize)
{
	// Key 길이를 조사한다.
	if (strlen(szKey) >= D_AGCA_ANIMATION_MAX_ATTACHED_DATA_KEY)
		return FALSE;

	// 이미 할당되어 있으면 안된다.
	if (m_apszAttachedDataKey[m_lNumAttachedData])
		return FALSE;

	// 할당한다.
	m_apszAttachedDataKey[m_lNumAttachedData]	= new CHAR[D_AGCA_ANIMATION_MAX_ATTACHED_DATA_KEY];
	if (!m_apszAttachedDataKey[m_lNumAttachedData])
		return FALSE;

	// 키를 copy한다.
	strcpy(m_apszAttachedDataKey[m_lNumAttachedData], szKey);
	// Size를 지정한다.
	m_aulAttachedDataSize[m_lNumAttachedData]	= ulDataSize;

	// Attched한 개수를 증가한다.
	++m_lNumAttachedData;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 090804. BOB
*******************************************************************************
BOOL AgcaAnimation::AllocateAttachedData(AgcdAnimation *pstAnimation)
{
	// Attached된 데이터가 없으면 skip~
	if (!m_lNumAttachedData)
		return TRUE; // skip

	// 이미 할당되어 있으면 문제! -_-;;
	if (pstAnimation->m_pavAttachedData)
		return FALSE;

	// 포인터 배열을 미리 할당~
	pstAnimation->m_pavAttachedData = new PVOID[m_lNumAttachedData];
	if (!pstAnimation->m_pavAttachedData)
		return FALSE;

	// 할당된 포인터 배열을 각각 설정된 크기만큼 다시 할당한다.
	for (INT32 lIndex = 0; lIndex < m_lNumAttachedData; ++lIndex)
	{
		pstAnimation->m_pavAttachedData[lIndex]		= new BYTE[m_aulAttachedDataSize[lIndex]];
		if (!pstAnimation->m_pavAttachedData[lIndex])
			return FALSE;

		memset(pstAnimation->m_pavAttachedData[lIndex], 0, m_aulAttachedDataSize[lIndex]);
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
* Desc : 키값으로 찾지 못하는 경우 -1을 리턴한다.(failed)
* 180804. BOB
*******************************************************************************
INT32 AgcaAnimation::GetAttachedDataIndex(CHAR *szKey)
{
	for (INT32 lIndex = 0; lIndex < m_lNumAttachedData; ++lIndex)
	{
		if (!strcmp(m_apszAttachedDataKey[lIndex], szKey))
			break;
	}

	return (lIndex == m_lNumAttachedData) ? (-1) : (lIndex);
}

/******************************************************************************
* Purpose :
*
* 100804. BOB
*******************************************************************************
PVOID AgcaAnimation::GetAttachedData(AgcdAnimation *pstAnim, CHAR *szKey)
{
	if (!pstAnim) 
		return NULL;

	INT32 lIndex = GetAttachedDataIndex(szKey);
	if (lIndex == -1)
		return NULL;

	return pstAnim->m_pavAttachedData[lIndex];
}

/******************************************************************************
* Purpose :
*
* 110804. BOB
*******************************************************************************
BOOL AgcaAnimation::SetCallbackRemoveAnimation(CHAR *szKey, ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	INT32 lIndex = GetAttachedDataIndex(szKey);
	if (lIndex == -1)
		return FALSE;

	m_pavRemoveAnimationCBClass[lIndex]	= pClass;
	m_pafRemoveAnimationCB[lIndex]		= pfCallback;

	return TRUE;
}

/******************************************************************************
******************************************************************************/