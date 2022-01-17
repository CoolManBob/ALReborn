#include "AgcaEffectSet.h"
#include "ApMemoryTracker.h"

VOID AgcaEffectSet::AddNode(AgcdEffectSetNodeData *pcsData, EFFCTRL_SET *pcsNode)
{
	List_AgcmEffectSetNode* pcsNext = pcsData->m_pcsHead;

	pcsData->m_pcsHead								= new List_AgcmEffectSetNode();
	pcsData->m_pcsHead->m_pcsNode					= pcsNode;
	pcsData->m_pcsHead->m_pcsNext					= pcsNext;
}

BOOL AgcaEffectSet::AddNode(INT32 lKey, EFFCTRL_SET *pcsNode)
{
	AgcdEffectSetNodeData				*pcsTemp		= NULL;
	AgcdEffectSetNodeData				*pcsCur			= m_pcsHead;

	while (pcsCur)
	{
		if (pcsCur->m_lKey == lKey)
			break;

		pcsCur	= pcsCur->m_pcsNext;
	}

	if( !pcsCur )
	{
		pcsTemp	= m_pcsHead;
		m_pcsHead = new AgcdEffectSetNodeData();
		m_pcsHead->m_lKey = lKey;
		m_pcsHead->m_pcsNext = pcsTemp;
		pcsCur = m_pcsHead;
	}

	AddNode(pcsCur, pcsNode);

	return TRUE;
}

VOID AgcaEffectSet::GetNode(INT32 lKey, EFFCTRL_SET **ppNode)
{
	AgcdEffectSetNodeData* pcsNodeData	= m_pcsHead;
	while (pcsNodeData)
	{
		if (pcsNodeData->m_lKey == lKey)
			break;

		pcsNodeData	= pcsNodeData->m_pcsNext;
	}

	if ( !pcsNodeData )		return;

	INT32	lNum			= 0;
	List_AgcmEffectSetNode* pcsNodeList = pcsNodeData->m_pcsHead;
	while (pcsNodeList)
	{
		ppNode[lNum] = pcsNodeList->m_pcsNode;
		++lNum;

		pcsNodeList	= pcsNodeList->m_pcsNext;
	}
}

BOOL AgcaEffectSet::GetNode(INT32 lKey, AES_EFFCTRL_SET_LIST **ppstList)
{
	AgcdEffectSetNodeData* pcsNodeData	= m_pcsHead;
	while (pcsNodeData)
	{
		if ( pcsNodeData->m_lKey == lKey )
			break;

		pcsNodeData	= pcsNodeData->m_pcsNext;
	}

	if ( !pcsNodeData )
		return TRUE;

	List_AgcmEffectSetNode* pcsNodeList	= pcsNodeData->m_pcsHead;
	while (pcsNodeList)
	{
		if (*ppstList)
			return FALSE;

		(*ppstList)										= new AES_EFFCTRL_SET_LIST();
		(*ppstList)->m_pstNode							= pcsNodeList->m_pcsNode;

		ppstList										= &((*ppstList)->m_pcsNext);
		pcsNodeList										= pcsNodeList->m_pcsNext;
	}

	return TRUE;
}

INT32 AgcaEffectSet::GetHeadKey()
{
	return m_pcsHead ? m_pcsHead->m_lKey : -1;
}

VOID AgcaEffectSet::RemoveNode(AgcdEffectSetNodeData *pstData)
{
	if ( !pstData->m_pcsHead )		return;

	List_AgcmEffectSetNode* pcsTemp	= NULL;
	List_AgcmEffectSetNode* pcsCur = pstData->m_pcsHead;

	while ( pcsCur )
	{
		pcsTemp			= pcsCur->m_pcsNext;
		delete pcsCur;
		pcsCur			= pcsTemp;
	}

	pstData->m_pcsHead	= NULL;

	return;
}

BOOL AgcaEffectSet::RemoveAllNode()
{
	AgcdEffectSetNodeData					*pcsTemp	= NULL;
	AgcdEffectSetNodeData					*pcsCur		= m_pcsHead;

	while (pcsCur)
	{
		RemoveNode(pcsCur);

		pcsTemp											= pcsCur->m_pcsNext;
		delete pcsCur;
		pcsCur											= pcsTemp;
	}

	m_pcsHead											= NULL;

	return TRUE;
}

BOOL AgcaEffectSet::RemoveNode(INT32 lKey)
{
	if (!m_pcsHead)
		return TRUE; // skip

	AgcdEffectSetNodeData* pcsPre = NULL;
	AgcdEffectSetNodeData* pcsCur = m_pcsHead;

	while (pcsCur)
	{
		if ( pcsCur->m_lKey == lKey )
			break;

		pcsPre = pcsCur;
		pcsCur = pcsCur->m_pcsNext;
	}

	if ( !pcsCur )		return FALSE;

	RemoveNode(pcsCur);

	if (pcsPre)
	{
		pcsPre->m_pcsNext = pcsCur->m_pcsNext;
	}
	else
	{
		m_pcsHead = pcsCur->m_pcsNext;
	}

	delete pcsCur;

	return TRUE;
}
