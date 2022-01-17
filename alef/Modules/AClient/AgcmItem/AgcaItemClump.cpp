#include "AgcaItemClump.h"
#include "AgcmItem.h"

#include "ApMemoryTracker.h"

RwTextureCallBackRead AgcaItemClump::m_fnDefaultTextureReadCB = NULL;
/******************************************************************************
* Purpose : Construct.
*
* 091202. Bob Jung
******************************************************************************/
AgcaItemClump::AgcaItemClump()
{
	m_pcsAgcmResourceLoader	= NULL;
}

/******************************************************************************
* Purpose : Destruct.
*
* 091202. Bob Jung
******************************************************************************/
AgcaItemClump::~AgcaItemClump()
{
}

/******************************************************************************
* Purpose :
*
* 091202. Bob Jung
******************************************************************************/
RpClump *AgcaItemClump::GetItemClump(CHAR *szFile)
{
/*	AgcdItemClump	*pClump = (AgcdItemClump *)(GetObject(szFile));
	if(!pClump)
		return NULL;

	++pClump->m_lRefCount;

	return pClump->m_pstClump;*/

	AgcdItemClump	**ppClump = (AgcdItemClump **)(GetObject(szFile));
	if ((!ppClump) || (!(*ppClump)))
		return NULL;

	(*ppClump)->m_csCSection.Lock();

	if (0 > (*ppClump)->m_lRefCount)
	{
		(*ppClump)->m_csCSection.Unlock();

		return NULL;
	}

	++(*ppClump)->m_lRefCount;

	(*ppClump)->m_csCSection.Unlock();

	return (*ppClump)->m_pstClump;
}

/******************************************************************************
* Purpose :
*
* 091202. Bob Jung
******************************************************************************/
RpClump *AgcaItemClump::AddItemClump(CHAR *szFile)
{
	AgcdItemClump	*pcsClump;
	pcsClump					= new AgcdItemClump();
	if (!pcsClump)
		return NULL;

	if (m_pcsAgcmResourceLoader)
		pcsClump->m_pstClump	= m_pcsAgcmResourceLoader->LoadClump(szFile, "Item.txd", NULL, rwFILTERMIPLINEAR, AgcmItem::m_pcsThis->m_szTexturePath);

	if (pcsClump->m_pstClump)
	{
		if (!AddObject((PVOID)(&pcsClump), szFile))
		{
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyClump(pcsClump->m_pstClump);

			return NULL;
		}

		// 이거 왜 안했나요? (BOB, 21O504)
		pcsClump->m_lRefCount = 1;
	}

	return pcsClump->m_pstClump;
}

/******************************************************************************
* Purpose :
*
* 091202. Bob Jung
******************************************************************************/
BOOL AgcaItemClump::RemoveItemClump(CHAR* szFile)
{
/*	AgcdItemClump	*pClump = (AgcdItemClump *)(GetObject(szFile));
	if (pClump)
	{
		--pClump->m_lRefCount;

		if (pClump->m_lRefCount <= 0)
		{
			RemoveObject(szFile);

			if (AgcmItem::m_pcsThis->m_pcsAgcmResourceLoader)
				AgcmItem::m_pcsThis->m_pcsAgcmResourceLoader->AddDestroyClump(pClump->m_pstClump);
//			else
//				RpClumpDestroy(pClump->m_pstClump);
//
//			free(pClump);
		}

		return TRUE;
	}

	return FALSE;*/

	AgcdItemClump	**ppClump	= (AgcdItemClump **)(GetObject(szFile));
	if ((!ppClump) || (!(*ppClump)))
		return FALSE;

	(*ppClump)->m_csCSection.Lock();

	--(*ppClump)->m_lRefCount;

	if ((*ppClump)->m_lRefCount <= 0)
	{
		RemoveObject(szFile);

		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->AddDestroyClump((*ppClump)->m_pstClump);

		(*ppClump)->m_csCSection.Unlock();

		delete (*ppClump);

		return TRUE;
	}

	(*ppClump)->m_csCSection.Unlock();

	return FALSE;
}

/******************************************************************************
* Purpose :
*
* 301103. BOB
******************************************************************************/
BOOL AgcaItemClump::RemoveAllItemClump()
{
	INT32	lIndex = 0;

	for (	AgcdItemClump **ppClump = (AgcdItemClump **)(GetObjectSequence(&lIndex));
			ppClump;
			ppClump = (AgcdItemClump **)(GetObjectSequence(&lIndex))					)
	{
		if (*ppClump)
		{
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyClump((*ppClump)->m_pstClump);

			delete (*ppClump);
		}
	}

	return RemoveObjectAll();

/*	for(RpClump	**ppClump = (RpClump **)(GetObjectSequence(&lIndex)); ppClump; ppClump = (RpClump **)(GetObjectSequence(&lIndex)))
	{
		if(*ppClump)
		{
			if (AgcmItem::m_pcsThis->m_pcsAgcmResourceLoader)
				AgcmItem::m_pcsThis->m_pcsAgcmResourceLoader->AddDestroyClump(*ppClump);
			else
				RpClumpDestroy(*ppClump);
		}
		else
			return FALSE;
	}

	return RemoveObjectAll();*/
}

/******************************************************************************
* Purpose :
*
* 190403. BOB
******************************************************************************/
BOOL AgcaItemClump::Initialize(INT32 lMaxDataNum)
{
	return InitializeObject(sizeof(AgcdItemClump *), lMaxDataNum);
}

/******************************************************************************
******************************************************************************/