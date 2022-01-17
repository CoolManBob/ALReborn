#include "AgcmAnimation.h"
#include "ApMemoryTracker.h"

/*
AgcmAnimation::AgcmAnimation()
{
	m_pcsList					= NULL;
	m_lNumCurCB					= 0;
	m_lNumMaxCB					= 0;
	m_fCurrentTime				= 0;
	m_fDuration					= 0;
}

AgcmAnimation::~AgcmAnimation()
{
	ResetAnimCB(FALSE);
}

VOID AgcmAnimation::SetMaxCallback(INT32 lMax)
{
	m_lNumMaxCB	= lMax;
}

BOOL AgcmAnimation::AddAnimList(AgcdAnimationCallbackList *pstPreList, AgcdAnimationCallbackList **ppstCurList, AgcdAnimationCallbackList *pstNextList, AgcdAnimationCallbackData *pcsData)
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

BOOL AgcmAnimation::AddAnimCB(AgcdAnimationCallbackData *pcsData)
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

			// 다음 리스트를 설정한다...
			pcsPreList	= pcsCurList;
			pcsCurList	= pcsCurList->m_pcsNext;
		}

		// 설정하려는 시간보다 리스트에 있는 시간들이 모두 작지 않으면, 맨 마지막에 넣는다.
		return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, NULL, pcsData);
	}

	return FALSE;
}

VOID AgcmAnimation::ResetAnimCB(BOOL InfluenceNextAnimation)
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

AgcdAnimationCallbackList *AgcmAnimation::HeadMoveToTail(AgcdAnimationCallbackList *pstHead)
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

AgcdAnimationCallbackList *AgcmAnimation::RemoveAnimList(AgcdAnimationCallbackList *pstCurList)
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

BOOL AgcmAnimation::AddTime(FLOAT fTime)
{
	m_fCurrentTime	+= fTime;
	if (m_fCurrentTime > m_fDuration)
		m_fCurrentTime = m_fDuration;

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

VOID AgcmAnimation::InitializeAnimation(FLOAT fDuration)
{
	ResetAnimCB();

	m_fCurrentTime	= 0.0f;
	m_fDuration		= fDuration;
}

INT32 AgcmAnimation::ParseAnimPoint(CHAR *szAnimPoint, INT32 *plDest)
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
}*/