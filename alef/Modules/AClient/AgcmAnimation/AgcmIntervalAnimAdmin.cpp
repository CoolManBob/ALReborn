#include "AgcmIntervalAnimAdmin.h"

/*IntervalAnimList::IntervalAnimList()
{
	m_pcsNext			= NULL;
	m_pcsParentModule	= NULL;
}

IntervalAnimList::~IntervalAnimList()
{
}*/

/*AgcmIntervalAnimAdmin::AgcmIntervalAnimAdmin()
{
	m_ppstHierarchy				= NULL;
//	m_pfIntervalAnimationCB		= NULL;
	m_pcsList					= NULL;
	m_lCurListNum				= 0;
	m_lMaxListNum				= 0;
}

AgcmIntervalAnimAdmin::~AgcmIntervalAnimAdmin()
{
	ResetIntervalData();
}

RtAnimInterpolator *AgcmIntervalAnimAdmin::IntervalAnimationCB(RtAnimInterpolator *animInstance, PVOID pvData)
{
	AgcdAnimationCallbackList	*pcsList	= (AgcdAnimationCallbackList *)(pvData);
	if (pcsList)
	{
//		if (pcsList->m_pcsParentModule)
		{
			pcsList->m_csData.m_pfCallback((PVOID)(&pcsList->m_csData));
//			pcsList->m_pcsParentModule->m_pfIntervalAnimationCB((PVOID)(&pcsList->m_csData));

			// 루프이면, 헤드를 마지막으로 옮긴다.
			if (pcsList->m_csData.m_bLoop)
				pcsList = (AgcdAnimationCallbackList *)(pcsList->m_pfLoopFunc((PVOID)(pcsList)));
//				pcsList = pcsList->m_pcsParentModule->HeadMoveToTail(pcsList);
			else // 루프가 아니면, 삭제한다.
				pcsList = (AgcdAnimationCallbackList *)(pcsList->m_pfEndFunc((PVOID)(pcsList)));
				//pcsList = pcsList->m_pcsParentModule->RemoveAnimList(pcsList);

			// 다음 콜백을 설정한다.
			if ((pcsList) && (pcsList->m_pfCustFunc))
				pcsList->m_pfCustFunc(pcsList->m_pvClass);
			else
				RtAnimInterpolatorSetAnimCallBack(animInstance, NULL, NULL, NULL);
		}
	}

	return animInstance;
}

BOOL AgcmIntervalAnimAdmin::AddIntervalData(AgcdAnimationCallbackData *pcsData)
{
	// 꽉 찼다!!!
	if ((m_lMaxListNum) && (m_lMaxListNum <= m_lCurListNum))
		return FALSE;

	if ((!m_ppstHierarchy) || (!*m_ppstHierarchy))
		return FALSE;

	AgcdAnimationCallbackList	*pcsCurList, *pcsPreList;

	// 현재 애니매이션의 진행시간보다 크거나 같으면...
	if ((*m_ppstHierarchy)->currentAnim->currentTime <= pcsData->m_fTime)
	{
		// 리스트가 있으면...
		if (m_pcsList)
		{
			// 최초 리스트에 들어있는 시간보다 작다면...
			if (m_pcsList->m_csData.m_fTime > pcsData->m_fTime)
			{
				// 잠시 pcsCurList에 저장한다.
				pcsCurList = m_pcsList;

				// 최초 리스트에 현재 녀석을 추가한다.
				if (!AddAnimList(NULL, &m_pcsList, pcsCurList, pcsData))
					return FALSE;

				// 콜백을 다시 설정한다.
				SetIntervalAnimationCallback();

				return TRUE;
			}
			else if (m_pcsList->m_csData.m_fTime == pcsData->m_fTime) // 최초 리스트에 들어있는 시간과 같다면...
			{
				// replace!!!
				memcpy(&m_pcsList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

				return TRUE;
			}

			pcsPreList	= m_pcsList;
			pcsCurList	= m_pcsList->m_pcsNext;
			// 리스트를 돈다...
			while (pcsCurList)
			{
				// 리스트의 시간보다 작다면...
				if (pcsCurList->m_csData.m_fTime > pcsData->m_fTime)
				{
					// 전 리스트와 현재 리스트 사이에 추가한다...
					return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, pcsCurList, pcsData);
				}
				else if (pcsCurList->m_csData.m_fTime == pcsData->m_fTime) // 리스트의 시간과 같다면...
				{
					// replace!!!
					memcpy(&pcsCurList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

					return TRUE;
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

			// 콜백을 등록한다.
			return SetIntervalAnimationCallback();
		}
	}
	else // 현재 애니매이션의 진행시간보다 작으면...
	{
		// 리스트가 없으면...
		if (!m_pcsList)
		{
			if(!AddAnimList(NULL, &m_pcsList, NULL, pcsData))
				return FALSE;

			// 콜백을 등록한다.
			return SetIntervalAnimationCallback();
		}

		pcsPreList	= m_pcsList;
		pcsCurList	= m_pcsList->m_pcsNext;
		// 리스트를 돈다...
		while (pcsCurList)
		{
			// 리스트의 시간보다 크다면...
			if (pcsCurList->m_csData.m_fTime < pcsData->m_fTime)
			{
				// 현재 리스트와 다음 리스트 사이에 추가한다...
				return AddAnimList(pcsCurList, &pcsCurList->m_pcsNext, pcsCurList->m_pcsNext, pcsData);
			}
			else if (pcsCurList->m_csData.m_fTime == pcsData->m_fTime) // 리스트의 시간과 같다면...
			{
				// replace!!!
				memcpy(&pcsCurList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

				return TRUE;
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

VOID AgcmIntervalAnimAdmin::SetIntervalAnimHierarchy(RpHAnimHierarchy **ppstHierarchy)
{
	m_ppstHierarchy = ppstHierarchy;
}

VOID AgcmIntervalAnimAdmin::SetMaxIntervalData(INT32 lMax)
{
	m_lMaxListNum = lMax;
}

VOID AgcmIntervalAnimAdmin::ResetIntervalData()
{
	// 리스트에 있는 녀석들을 다 날리고,
	// RtAnimInterpolatorSetAnimCallBack()을 초기화한다!
	AgcdAnimationCallbackList	*pcsNext = m_pcsList;
	AgcdAnimationCallbackList	*pcsTemp;
	while (pcsNext)
	{
		pcsTemp = pcsNext->m_pcsNext;

		delete pcsNext;

		pcsNext = pcsTemp;
	}

	m_pcsList		= NULL;
	m_lCurListNum	= 0;

	if ((m_ppstHierarchy) && (*m_ppstHierarchy))
		RtAnimInterpolatorSetAnimCallBack((*m_ppstHierarchy)->currentAnim, NULL, 0.0f, NULL);
}

BOOL AgcmIntervalAnimAdmin::AddAnimList(AgcdAnimationCallbackList *pstPreList, AgcdAnimationCallbackList **ppstCurList, AgcdAnimationCallbackList *pstNextList, AgcdAnimationCallbackData *pcsData)
{
	if(ppstCurList)
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
//		(*(ppstCurList))->m_pcsParentModule	= this;

		// 필요한 callback을 지정한다.
		(*(ppstCurList))->m_pfLoopFunc	= SetLoopAnimCB;
		(*(ppstCurList))->m_pfEndFunc	= SetEndAnimCB;
		(*(ppstCurList))->m_pfCustFunc	= SetSetNextAnimCB;

		// 전 리스트의 다음을 지정한다.
		if (pstPreList)
		{
			pstPreList->m_pcsNext = *(ppstCurList);
		}

		++m_lCurListNum;
	}

	return TRUE;
}

AgcdAnimationCallbackList *AgcmIntervalAnimAdmin::RemoveAnimList(AgcdAnimationCallbackList *pstCurList)
{
	if (!pstCurList)
		return NULL;
	// 다음을 지정하고...
	m_pcsList = pstCurList->m_pcsNext;
	// 날려버린다~ 햏햏~ -_-;;
	delete pstCurList;

	--m_lCurListNum;

	return m_pcsList;
}

BOOL AgcmIntervalAnimAdmin::SetIntervalAnimationCallback()
{
	if ((!m_ppstHierarchy) || (!*m_ppstHierarchy))
		return FALSE;

	if (!m_pcsList)
		RtAnimInterpolatorSetAnimCallBack((*m_ppstHierarchy)->currentAnim, NULL, NULL, NULL);
	else
		RtAnimInterpolatorSetAnimCallBack((*m_ppstHierarchy)->currentAnim, IntervalAnimationCB, m_pcsList->m_csData.m_fTime, (PVOID)(m_pcsList));

	return TRUE;
}

AgcdAnimationCallbackList *AgcmIntervalAnimAdmin::HeadMoveToTail(AgcdAnimationCallbackList *pstHead)
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

PVOID AgcmIntervalAnimAdmin::SetLoopAnimCB(PVOID pvData)
{
	if (!pvData)
		return NULL;

	AgcdAnimationCallbackList	*pcsHead	= (AgcdAnimationCallbackList *)(pvData);

	return (PVOID)(
		((AgcmIntervalAnimAdmin *)(pcsHead->m_pvClass))->HeadMoveToTail(pcsHead)
		);
}

PVOID AgcmIntervalAnimAdmin::SetEndAnimCB(PVOID pvData)
{
	if (!pvData)
		return NULL;

	AgcdAnimationCallbackList	*pcsCurrent	= (AgcdAnimationCallbackList *)(pvData);

	return (PVOID)(
		((AgcmIntervalAnimAdmin *)(pcsCurrent->m_pvClass))->RemoveAnimList(pcsCurrent)
		);
}

PVOID AgcmIntervalAnimAdmin::SetSetNextAnimCB(PVOID pvData)
{
	if (!pvData)
		return NULL;

	AgcmIntervalAnimAdmin		*pcsThis	= (AgcmIntervalAnimAdmin *)(pvData);

	pcsThis->SetIntervalAnimationCallback();

	return NULL;
}*/