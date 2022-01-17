#include "AgcaEffectData.h"

#include "ApMemoryTracker.h"

/*
AgcaEffectData::AgcaEffectData()
{
	m_lMax		= 0;
	m_lCount	= 0;
}

AgcaEffectData::~AgcaEffectData()
{
}

BOOL AgcaEffectData::Initialize(INT32 lMaxDataNum)
{
	return InitializeObject(sizeof(AgcdUseEffectSetData), m_lMax = lMaxDataNum);
}

AgcdUseEffectSetData *AgcaEffectData::CreateData()
{
	if(m_lCount >= m_lMax)
		return NULL;

	AgcdUseEffectSetData stData;
	memset(&stData, 0, sizeof(AgcdUseEffectSetData));

	stData.m_fScale		= 1.0f;

	return (AgcdUseEffectSetData *)(AddObject((PVOID)(&stData), ++m_lCount));
}

BOOL AgcaEffectData::RemoveAll()
{
	m_lCount	= 0;
	m_lMax		= 0;

	return RemoveObjectAll();
}
*/

/******************************************************************************
* Purpose :
*
* 111004 BOB
******************************************************************************/
AgcaEffectData2::AgcaEffectData2()
{
	m_ulCount	= 0;
}

/******************************************************************************
* Purpose :
*
* 111004 BOB
******************************************************************************/
AgcaEffectData2::~AgcaEffectData2()
{
}

/******************************************************************************
* Purpose :
*
* 111004 BOB
******************************************************************************/
AgcdUseEffectSetData *AgcaEffectData2::Create(UINT32 ulIndex, AgcdUseEffectSetData *pcsSrc)
{
	//@{ kday 20050325
	//AgcdUseEffectSetData	*pcsData	= (	(pcsSrc) ? (new AgcdUseEffectSetData(pcsSrc)) : (new AgcdUseEffectSetData())	);
	ASSERT( !pcsSrc && "check" );
	AgcdUseEffectSetData	*pcsData	= new AgcdUseEffectSetData;
	//@} kday

	pcsData->m_ulIndex					= ulIndex;

	++m_ulCount;

	return pcsData;
}

/******************************************************************************
* Purpose :
*
* 121004 BOB
******************************************************************************/
VOID AgcaEffectData2::Delete(AgcdUseEffectSetData *pcsData, BOOL bFree)
{
	Delete(pcsData);
	if (bFree)
		delete pcsData;
}

/******************************************************************************
* Purpose :
*
* 111004 BOB
******************************************************************************/
VOID AgcaEffectData2::Delete(AgcdUseEffectSetData *pcsData)
{
	if (pcsData->m_pcsRotation)
	{
		delete pcsData->m_pcsRotation;
		pcsData->m_pcsRotation = NULL;
	}

	if (pcsData->m_pszCustData)
	{
		delete [] pcsData->m_pszCustData;
		pcsData->m_pszCustData = NULL;
	}

	if (pcsData->m_pszSoundName)
	{
		delete [] pcsData->m_pszSoundName;
		pcsData->m_pszSoundName = NULL;
	}

	if (pcsData->m_pv3dOffset)
	{
		delete pcsData->m_pv3dOffset;
		pcsData->m_pv3dOffset = NULL;
	}
}

/******************************************************************************
* Purpose :
*
* 111004 BOB
******************************************************************************/
VOID AgcaEffectData2::Delete(AgcdUseEffectSet *pcsSet, AgcdUseEffectSetData *pcsDeleteData)
{
	AgcdUseEffectSetList	*pcsPre		= NULL;
	AgcdUseEffectSetList	*pcsCurrent	= pcsSet->m_pcsHead;

	while (pcsCurrent)
	{
		if ((&pcsCurrent->m_csData) == pcsDeleteData)
		{
			if (!pcsPre)
				pcsSet->m_pcsHead		= pcsCurrent->m_pcsNext;
			else
				pcsPre->m_pcsNext		= pcsCurrent->m_pcsNext;

			Delete(&pcsCurrent->m_csData);
			delete pcsCurrent;

			return;
		}

		pcsPre		= pcsCurrent;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}
}

/******************************************************************************
* Purpose :
*
* 111004 BOB
******************************************************************************/
VOID AgcaEffectData2::DeleteAll(AgcdUseEffectSet *pcsSet)
{
	if (!pcsSet)
		return;

	AgcdUseEffectSetList	*pcsCurrent	= pcsSet->m_pcsHead;
	AgcdUseEffectSetList	*pcsNext;

	while (pcsCurrent)
	{
		pcsNext							= pcsCurrent->m_pcsNext;

		Delete(&pcsCurrent->m_csData);
		delete pcsCurrent;

		// 틀렸음. 여기서 죽는다 (Parn 04/10/13 04:15)
		//pcsCurrent						= pcsCurrent->m_pcsNext;
		pcsCurrent						= pcsNext;
	}

	pcsSet->m_pcsHead = NULL;
}

/******************************************************************************
* Purpose :
*
* 111004 BOB
******************************************************************************/
AgcdUseEffectSetData *AgcaEffectData2::GetData(AgcdUseEffectSet *pcsSet, UINT32 ulFindIndex, BOOL bCreate)
{
	if ( !pcsSet )
		return NULL;

	AgcdUseEffectSetList	*pcsPrev		= NULL;
	AgcdUseEffectSetList	*pcsCurrent		= pcsSet->m_pcsHead;

	while (pcsCurrent)
	{
		if (pcsCurrent->m_csData.m_ulIndex == ulFindIndex)
			return &pcsCurrent->m_csData;

		pcsPrev		= pcsCurrent;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	if (bCreate)
	{
		if (pcsPrev)
		{
			pcsPrev->m_pcsNext	= new AgcdUseEffectSetList();
			pcsPrev->m_pcsNext->m_csData.m_ulIndex	= ulFindIndex;
			return &pcsPrev->m_pcsNext->m_csData;
		}
		else
		{
			pcsSet->m_pcsHead	= new AgcdUseEffectSetList();
			pcsSet->m_pcsHead->m_csData.m_ulIndex	= ulFindIndex;
			return &pcsSet->m_pcsHead->m_csData;
		}
	}

	return NULL;
}

/******************************************************************************
* Purpose :
*
* 121004 BOB
******************************************************************************/
VOID AgcaEffectData2::CopyData(AgcdUseEffectSetData *pcsDest, AgcdUseEffectSetData *pcsSrc)
{
	//. 2005. 09. 12 Nonstopdj
	//. m_ulIndex도 함께 copy
	pcsDest->m_ulIndex			= pcsSrc->m_ulIndex;

	pcsDest->m_fScale			= pcsSrc->m_fScale;
	pcsDest->m_lParentNodeID	= pcsSrc->m_lParentNodeID;
	pcsDest->m_ulConditionFlags	= pcsSrc->m_ulConditionFlags;
	pcsDest->m_ulEID			= pcsSrc->m_ulEID;
	pcsDest->m_ulStartGap		= pcsSrc->m_ulStartGap;
	pcsDest->m_fParticleNumScale= pcsSrc->m_fParticleNumScale;
	pcsDest->m_rgbScale			= pcsSrc->m_rgbScale;
	pcsDest->m_bAtomicEmitter	= pcsSrc->m_bAtomicEmitter;
	pcsDest->m_bClumpEmitter	= pcsSrc->m_bClumpEmitter;

	// 인덱스는 copy하지 않는다. (pcsDest->m_ulIndex = pcsSrc->m_ulIndex)

	if (pcsSrc->m_pcsRotation)
	{
		if (!pcsDest->m_pcsRotation)
			pcsDest->m_pcsRotation	= new AgcdUseEffectSetDataRotation();

		memcpy(pcsDest->m_pcsRotation, pcsSrc->m_pcsRotation, sizeof(AgcdUseEffectSetDataRotation));
	}
	else if (pcsDest->m_pcsRotation)
	{
		delete pcsDest->m_pcsRotation;
		pcsDest->m_pcsRotation	= NULL;
	}

	if (pcsSrc->m_pszCustData)
	{
		if (pcsDest->m_pszCustData)
			delete [] pcsDest->m_pszCustData;

		pcsDest->m_pszCustData	= new CHAR[strlen(pcsSrc->m_pszCustData) + 1];

		strcpy(pcsDest->m_pszCustData, pcsSrc->m_pszCustData);
	}
	else if (pcsDest->m_pszCustData)
	{
		delete [] pcsDest->m_pszCustData;
		pcsDest->m_pszCustData	= NULL;
	}

	if (pcsSrc->m_pszSoundName)
	{
		if (pcsDest->m_pszSoundName)
			delete [] pcsDest->m_pszSoundName;

		pcsDest->m_pszSoundName	= new CHAR[strlen(pcsSrc->m_pszSoundName) + 1];

		strcpy(pcsDest->m_pszSoundName, pcsSrc->m_pszSoundName);
	}
	else if (pcsDest->m_pszSoundName)
	{
		delete [] pcsDest->m_pszSoundName;
		pcsDest->m_pszSoundName	= NULL;
	}

	if (pcsSrc->m_pv3dOffset)
	{
		if (!pcsDest->m_pv3dOffset)
			pcsDest->m_pv3dOffset	= new RwV3d();

		memcpy(pcsDest->m_pv3dOffset, pcsSrc->m_pv3dOffset, sizeof(RwV3d));
	}
	else if (pcsDest->m_pv3dOffset)
	{
		delete pcsDest->m_pv3dOffset;
		pcsDest->m_pv3dOffset	= NULL;
	}
}

BOOL AgcaEffectData2::CopyDataSet(AgcdUseEffectSet *pcsDestSet, AgcdUseEffectSet *pcsSrcSet)
{
	DeleteAll(pcsDestSet);

	pcsDestSet->m_ulConditionFlags	= pcsSrcSet->m_ulConditionFlags;

	AgcdUseEffectSetData	*pcsDestData	= NULL;
	AgcdUseEffectSetList	*pcsSrcCurrent	= pcsSrcSet->m_pcsHead;
	while (pcsSrcCurrent)
	{
		pcsDestData		= GetData(pcsDestSet, pcsSrcCurrent->m_csData.m_ulIndex, TRUE);
		if (!pcsDestData)
			return FALSE;

		CopyData(pcsDestData, &pcsSrcCurrent->m_csData);

		pcsSrcCurrent	= pcsSrcCurrent->m_pcsNext;
	}
	
	return TRUE;
}

/******************************************************************************
******************************************************************************/